// ==WindhawkMod==
// @id              power-action-countdown
// @name            Power Action Countdown
// @name:de-DE      Countdown vor Energieaktionen
// @description     Shows a configurable cancellable countdown before shutdown, restart, sleep, or hibernation.
// @description:de-DE Zeigt vor Herunterfahren, Neustart, Standby oder Ruhezustand einen konfigurierbaren, abbrechbaren Countdown an.
// @version         1.2
// @author          Nerdworld
// @github          https://github.com/nerdworldDE
// @homepage        https://nerdworld.de/
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @include         RuntimeBroker.exe
// @architecture    x86-64
// @compilerOptions -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00 -luser32 -lgdi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Power action countdown

Shows an individually configurable countdown before Windows shuts down,
restarts, enters sleep, or enters hibernation. A value of `0` disables the
countdown for the corresponding action and executes it immediately. Values
above 60 seconds are limited to 60 seconds.

The countdown is cancelled by:

- any keyboard key
- a mouse button
- the mouse wheel
- a touchscreen press on the overlay

Moving the mouse alone does not cancel the countdown. The cancelling input is
consumed and isn't forwarded to the application underneath the overlay.

The countdown is displayed on the primary monitor. Keyboard and mouse-button
input on other monitors still cancels it.

## Screenshot
![Copy dialog](https://i.imgur.com/GpcwW3m.png)

## Scope and limitations

The mod targets power actions initiated through the Windows shell. It is loaded
into:

- `explorer.exe` for classic shell power paths
- `StartMenuExperienceHost.exe` for the Windows 11 Start UI
- `RuntimeBroker.exe` because current Windows 11 builds can delegate Start-menu
  power actions to a runtime broker

Power actions initiated by `shutdown.exe`, Task Manager, the Ctrl+Alt+Delete
screen, the sign-in/lock screen, or arbitrary third-party applications aren't
covered unless they ultimately call a hooked API from one of these processes.

## Troubleshooting

Enable mod logging in Windhawk's **Advanced** tab. When an action is detected,
the log states which Windows API was intercepted. The exact API used by Windows
can differ between builds.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- shutdownCountdownSeconds: 5
  $name: Shutdown countdown in seconds
  $name:de-DE: Countdown vor Herunterfahren in Sekunden
- restartCountdownSeconds: 5
  $name: Restart countdown in seconds
  $name:de-DE: Countdown vor Neustart in Sekunden
- sleepCountdownSeconds: 5
  $name: Sleep countdown in seconds
  $name:de-DE: Countdown vor Standby in Sekunden
- hibernateCountdownSeconds: 5
  $name: Hibernation countdown in seconds
  $name:de-DE: Countdown vor Ruhezustand in Sekunden
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_utils.h>
#include <objbase.h>

#include <algorithm>
#include <atomic>
#include <cwchar>
#include <iterator>

namespace {

constexpr int kMaximumCountdownSeconds = 30;
constexpr UINT kCancelCountdownMessage = WM_APP + 1;
constexpr UINT_PTR kCountdownTimerId = 1;
constexpr UINT kCountdownTimerIntervalMs = 250;
constexpr NTSTATUS kStatusCancelled = static_cast<NTSTATUS>(0xC0000120L);

enum class PowerAction {
    Shutdown,
    Restart,
    Sleep,
    Hibernate,
};

struct UiStrings {
    const wchar_t* shutdown;
    const wchar_t* restart;
    const wchar_t* sleep;
    const wchar_t* hibernate;
    const wchar_t* genericAction;
    const wchar_t* cancelHint;
};

struct CountdownThreadContext {
    PowerAction action;
    int seconds;
    HANDLE doneEvent;
    ULONGLONG endTick;
    int displayedSeconds;
    bool proceed;
    HHOOK keyboardHook;
    HHOOK mouseHook;
    bool swallowedKeys[256]{};
    bool swallowedMouseButtons[4]{};
};

std::atomic<int> g_shutdownCountdownSeconds{5};
std::atomic<int> g_restartCountdownSeconds{5};
std::atomic<int> g_sleepCountdownSeconds{5};
std::atomic<int> g_hibernateCountdownSeconds{5};
std::atomic<bool> g_unloading{false};
std::atomic<HWND> g_countdownWindow{nullptr};
std::atomic<CountdownThreadContext*> g_inputContext{nullptr};

HMODULE g_modModule = nullptr;
ATOM g_countdownWindowClassAtom = 0;
wchar_t g_countdownWindowClassName[96]{};
HANDLE g_countdownStopEvent = nullptr;
HANDLE g_countdownIdleEvent = nullptr;
SRWLOCK g_countdownLifecycleLock = SRWLOCK_INIT;

thread_local bool g_bypassHooks = false;

struct BypassHooksGuard {
    bool previous;

    BypassHooksGuard() : previous(g_bypassHooks) {
        g_bypassHooks = true;
    }

    ~BypassHooksGuard() {
        g_bypassHooks = previous;
    }
};

struct CountdownActivityGuard {
    bool active = false;

    bool Begin() {
        AcquireSRWLockExclusive(&g_countdownLifecycleLock);
        if (!g_unloading.load(std::memory_order_acquire)) {
            ResetEvent(g_countdownIdleEvent);
            ResetEvent(g_countdownStopEvent);
            active = true;
        }
        ReleaseSRWLockExclusive(&g_countdownLifecycleLock);
        return active;
    }

    ~CountdownActivityGuard() {
        if (active) {
            SetEvent(g_countdownIdleEvent);
        }
    }
};

using ExitWindowsEx_t = decltype(&ExitWindowsEx);
using InitiateShutdownW_t = decltype(&InitiateShutdownW);
using InitiateSystemShutdownExW_t = decltype(&InitiateSystemShutdownExW);
using SetSystemPowerState_t = decltype(&SetSystemPowerState);
using NtInitiatePowerAction_t = NTSTATUS(NTAPI*)(
    POWER_ACTION, SYSTEM_POWER_STATE, ULONG, BOOLEAN);
using NtSetSystemPowerState_t = NTSTATUS(NTAPI*)(
    POWER_ACTION, SYSTEM_POWER_STATE, ULONG);

ExitWindowsEx_t ExitWindowsEx_Original = nullptr;
InitiateShutdownW_t InitiateShutdownW_Original = nullptr;
InitiateSystemShutdownExW_t InitiateSystemShutdownExW_Original = nullptr;
SetSystemPowerState_t SetSystemPowerState_Original = nullptr;
NtInitiatePowerAction_t NtInitiatePowerAction_Original = nullptr;
NtSetSystemPowerState_t NtSetSystemPowerState_Original = nullptr;

int ClampCountdownSetting(int seconds) {
    return std::clamp(seconds, 0, kMaximumCountdownSeconds);
}

void LoadSettings() {
    g_shutdownCountdownSeconds.store(
        ClampCountdownSetting(
            Wh_GetIntSetting(L"shutdownCountdownSeconds")),
        std::memory_order_relaxed);
    g_restartCountdownSeconds.store(
        ClampCountdownSetting(
            Wh_GetIntSetting(L"restartCountdownSeconds")),
        std::memory_order_relaxed);
    g_sleepCountdownSeconds.store(
        ClampCountdownSetting(
            Wh_GetIntSetting(L"sleepCountdownSeconds")),
        std::memory_order_relaxed);
    g_hibernateCountdownSeconds.store(
        ClampCountdownSetting(
            Wh_GetIntSetting(L"hibernateCountdownSeconds")),
        std::memory_order_relaxed);
}

int GetCountdownSeconds(PowerAction action) {
    switch (action) {
        case PowerAction::Shutdown:
            return g_shutdownCountdownSeconds.load(
                std::memory_order_relaxed);
        case PowerAction::Restart:
            return g_restartCountdownSeconds.load(
                std::memory_order_relaxed);
        case PowerAction::Sleep:
            return g_sleepCountdownSeconds.load(
                std::memory_order_relaxed);
        case PowerAction::Hibernate:
            return g_hibernateCountdownSeconds.load(
                std::memory_order_relaxed);
    }

    return 0;
}

const UiStrings& GetUiStrings() {
    static const UiStrings english{
        .shutdown = L"Shut down",
        .restart = L"Restart",
        .sleep = L"Sleep",
        .hibernate = L"Hibernate",
        .genericAction = L"Power action",
        .cancelHint =
            L"Press any key, click a mouse button, or use the mouse wheel "
            L"to cancel. Mouse movement is ignored.",
    };

    static const UiStrings german{
        .shutdown = L"Herunterfahren",
        .restart = L"Neustart",
        .sleep = L"Standby",
        .hibernate = L"Ruhezustand",
        .genericAction = L"Energieaktion",
        .cancelHint =
            L"Taste, Mausklick oder Mausrad bricht ab. "
            L"Mausbewegungen werden ignoriert.",
    };

    const LANGID language = GetUserDefaultUILanguage();
    return PRIMARYLANGID(language) == LANG_GERMAN ? german : english;
}

const wchar_t* GetActionTitle(PowerAction action) {
    const UiStrings& strings = GetUiStrings();

    switch (action) {
        case PowerAction::Shutdown:
            return strings.shutdown;
        case PowerAction::Restart:
            return strings.restart;
        case PowerAction::Sleep:
            return strings.sleep;
        case PowerAction::Hibernate:
            return strings.hibernate;
    }

    return strings.genericAction;
}

void EnsureCountdownAlwaysOnTop(HWND hWnd) {
    if (!IsWindow(hWnd)) {
        return;
    }

    SetWindowPos(
        hWnd,
        HWND_TOPMOST,
        0,
        0,
        0,
        0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
            SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
}

void CancelCountdown(HWND hWnd) {
    auto* context = reinterpret_cast<CountdownThreadContext*>(
        GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    if (!context) {
        return;
    }

    context->proceed = false;
    DestroyWindow(hWnd);
}

bool PostCancelToCountdown() {
    HWND hWnd = g_countdownWindow.load(std::memory_order_acquire);
    return hWnd && IsWindow(hWnd) &&
           PostMessageW(hWnd, kCancelCountdownMessage, 0, 0);
}

bool IsKeyboardReleaseMessage(WPARAM wParam) {
    return wParam == WM_KEYUP || wParam == WM_SYSKEYUP;
}

bool IsKeyboardPressMessage(WPARAM wParam) {
    return wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
}

bool IsMouseReleaseMessage(WPARAM message) {
    switch (message) {
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
            return true;
        default:
            return false;
    }
}

bool IsMousePressMessage(WPARAM message) {
    switch (message) {
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
            return true;
        default:
            return false;
    }
}

int GetMouseButtonIndex(WPARAM message) {
    switch (message) {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
            return 0;
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            return 1;
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
            return 2;
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
            return 3;
        default:
            return -1;
    }
}

LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    auto* context = g_inputContext.load(std::memory_order_acquire);
    if (code == HC_ACTION && context) {
        const auto* keyboard = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
        const unsigned int vk = keyboard ? keyboard->vkCode : 0;
        if (vk < std::size(context->swallowedKeys)) {
            if (IsKeyboardPressMessage(wParam) && PostCancelToCountdown()) {
                context->swallowedKeys[vk] = true;
                return 1;
            }
            if (IsKeyboardReleaseMessage(wParam) && context->swallowedKeys[vk]) {
                context->swallowedKeys[vk] = false;
                return 1;
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wParam, LPARAM lParam) {
    auto* context = g_inputContext.load(std::memory_order_acquire);
    if (code == HC_ACTION && context) {
        const int buttonIndex = GetMouseButtonIndex(wParam);
        if (IsMousePressMessage(wParam) && PostCancelToCountdown()) {
            if (buttonIndex >= 0) context->swallowedMouseButtons[buttonIndex] = true;
            return 1;
        }
        if (IsMouseReleaseMessage(wParam) && buttonIndex >= 0 &&
            context->swallowedMouseButtons[buttonIndex]) {
            context->swallowedMouseButtons[buttonIndex] = false;
            return 1;
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

LRESULT CALLBACK CountdownWindowProc(HWND hWnd,
                                     UINT message,
                                     WPARAM wParam,
                                     LPARAM lParam) {
    auto* context = reinterpret_cast<CountdownThreadContext*>(
        GetWindowLongPtrW(hWnd, GWLP_USERDATA));

    if (message == WM_NCCREATE) {
        auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
        context = static_cast<CountdownThreadContext*>(create->lpCreateParams);
        SetWindowLongPtrW(hWnd, GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR>(context));
    }

    switch (message) {
        case WM_CREATE:
            SetTimer(hWnd, kCountdownTimerId,
                     kCountdownTimerIntervalMs, nullptr);
            return 0;

        case WM_TIMER: {
            EnsureCountdownAlwaysOnTop(hWnd);

            if (!context) {
                return 0;
            }

            const ULONGLONG now = GetTickCount64();
            if (now >= context->endTick) {
                context->displayedSeconds = 0;
                context->proceed = true;
                DestroyWindow(hWnd);
                return 0;
            }

            const ULONGLONG remainingMs = context->endTick - now;
            const int remainingSeconds =
                static_cast<int>((remainingMs + 999) / 1000);

            if (remainingSeconds != context->displayedSeconds) {
                context->displayedSeconds = remainingSeconds;
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            return 0;
        }

        case kCancelCountdownMessage:
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
        case WM_POINTERDOWN:
            CancelCountdown(hWnd);
            return 0;

        // WM_MOUSEMOVE and WM_POINTERUPDATE are deliberately ignored.

        case WM_WINDOWPOSCHANGING: {
            auto* windowPos = reinterpret_cast<WINDOWPOS*>(lParam);
            if (windowPos) {
                windowPos->hwndInsertAfter = HWND_TOPMOST;
                windowPos->flags &= ~SWP_NOZORDER;
                windowPos->flags |= SWP_NOOWNERZORDER;
            }
            break;
        }

        case WM_CLOSE:
            CancelCountdown(hWnd);
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT paint;
            HDC hdc = BeginPaint(hWnd, &paint);

            RECT clientRect;
            GetClientRect(hWnd, &clientRect);

            HBRUSH backgroundBrush = CreateSolidBrush(RGB(24, 24, 27));
            FillRect(hdc, &clientRect, backgroundBrush);
            DeleteObject(backgroundBrush);

            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(245, 245, 245));

            UINT dpi = GetDpiForWindow(hWnd);
            if (!dpi) {
                dpi = 96;
            }

            HFONT numberFont = CreateFontW(
                -MulDiv(96, dpi, 72), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE,
                FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH,
                L"Segoe UI");
            HFONT titleFont = CreateFontW(
                -MulDiv(28, dpi, 72), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE,
                FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH,
                L"Segoe UI");
            HFONT infoFont = CreateFontW(
                -MulDiv(16, dpi, 72), 0, 0, 0, FW_NORMAL, FALSE, FALSE,
                FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH,
                L"Segoe UI");

            const int centerY =
                (clientRect.bottom - clientRect.top) / 2;

            wchar_t numberText[16];
            swprintf(numberText, std::size(numberText), L"%d",
                     context ? context->displayedSeconds : 0);

            RECT numberRect = clientRect;
            numberRect.top = centerY - MulDiv(120, dpi, 96);
            numberRect.bottom = centerY + MulDiv(20, dpi, 96);

            HGDIOBJ oldFont = SelectObject(hdc, numberFont);
            DrawTextW(hdc, numberText, -1, &numberRect,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

            RECT titleRect = clientRect;
            titleRect.top = centerY + MulDiv(20, dpi, 96);
            titleRect.bottom = centerY + MulDiv(80, dpi, 96);

            SelectObject(hdc, titleFont);
            DrawTextW(
                hdc,
                context ? GetActionTitle(context->action)
                        : GetUiStrings().genericAction,
                -1,
                &titleRect,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

            RECT infoRect = clientRect;
            infoRect.left += MulDiv(40, dpi, 96);
            infoRect.right -= MulDiv(40, dpi, 96);
            infoRect.top = centerY + MulDiv(95, dpi, 96);
            infoRect.bottom = centerY + MulDiv(170, dpi, 96);

            SelectObject(hdc, infoFont);
            SetTextColor(hdc, RGB(190, 190, 195));
            DrawTextW(hdc, GetUiStrings().cancelHint, -1, &infoRect,
                      DT_CENTER | DT_VCENTER | DT_WORDBREAK | DT_NOPREFIX);

            SelectObject(hdc, oldFont);
            DeleteObject(numberFont);
            DeleteObject(titleFont);
            DeleteObject(infoFont);

            EndPaint(hWnd, &paint);
            return 0;
        }

        case WM_DESTROY:
            KillTimer(hWnd, kCountdownTimerId);
            if (g_countdownWindow.load(std::memory_order_relaxed) == hWnd) {
                g_countdownWindow.store(nullptr, std::memory_order_release);
            }
            return 0;
    }

    return DefWindowProcW(hWnd, message, wParam, lParam);
}

RECT GetPrimaryMonitorRectangle() {
    POINT primaryMonitorPoint{0, 0};
    HMONITOR primaryMonitor = MonitorFromPoint(
        primaryMonitorPoint, MONITOR_DEFAULTTOPRIMARY);

    MONITORINFO monitorInfo{
        .cbSize = sizeof(monitorInfo),
    };

    if (primaryMonitor && GetMonitorInfoW(primaryMonitor, &monitorInfo)) {
        return monitorInfo.rcMonitor;
    }

    return RECT{
        .left = 0,
        .top = 0,
        .right = GetSystemMetrics(SM_CXSCREEN),
        .bottom = GetSystemMetrics(SM_CYSCREEN),
    };
}

void InstallCountdownInputHooks(CountdownThreadContext* context) {
    context->keyboardHook = SetWindowsHookExW(
        WH_KEYBOARD_LL, LowLevelKeyboardProc, g_modModule, 0);
    if (!context->keyboardHook) {
        Wh_Log(L"SetWindowsHookExW(WH_KEYBOARD_LL) failed: %lu",
               GetLastError());
    }

    context->mouseHook = SetWindowsHookExW(
        WH_MOUSE_LL, LowLevelMouseProc, g_modModule, 0);
    if (!context->mouseHook) {
        Wh_Log(L"SetWindowsHookExW(WH_MOUSE_LL) failed: %lu",
               GetLastError());
    }
}

void UninstallCountdownInputHooks(CountdownThreadContext* context) {
    if (context->keyboardHook) {
        UnhookWindowsHookEx(context->keyboardHook);
        context->keyboardHook = nullptr;
    }

    if (context->mouseHook) {
        UnhookWindowsHookEx(context->mouseHook);
        context->mouseHook = nullptr;
    }
}

DWORD WINAPI CountdownThreadProc(void* parameter) {
    auto* context = static_cast<CountdownThreadContext*>(parameter);
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    context->proceed = true;  // Fail open until the UI is fully established.

    if (WaitForSingleObject(g_countdownStopEvent, 0) == WAIT_OBJECT_0 ||
        g_unloading.load(std::memory_order_acquire)) {
        context->proceed = false;
        SetEvent(context->doneEvent);
        return 0;
    }

    const RECT monitorRect = GetPrimaryMonitorRectangle();
    const int width = monitorRect.right - monitorRect.left;
    const int height = monitorRect.bottom - monitorRect.top;

    context->endTick =
        GetTickCount64() + static_cast<ULONGLONG>(context->seconds) * 1000;
    context->displayedSeconds = context->seconds;
    context->proceed = false;
    context->keyboardHook = nullptr;
    context->mouseHook = nullptr;

    HWND hWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        g_countdownWindowClassName,
        L"Power action countdown",
        WS_POPUP,
        monitorRect.left,
        monitorRect.top,
        width,
        height,
        nullptr,
        nullptr,
        g_modModule,
        context);

    if (!hWnd) {
        Wh_Log(L"CreateWindowExW failed: %lu", GetLastError());
        context->proceed = true;
        SetEvent(context->doneEvent);
        return 0;
    }

    g_countdownWindow.store(hWnd, std::memory_order_release);
    g_inputContext.store(context, std::memory_order_release);
    InstallCountdownInputHooks(context);

    ShowWindow(hWnd, SW_SHOW);
    SetWindowPos(hWnd, HWND_TOPMOST, monitorRect.left, monitorRect.top,
                 width, height, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
    UpdateWindow(hWnd);

    // Don't merge this thread's input queue with another process. The normal
    // foreground request is sufficient on shell-initiated power actions, and
    // the low-level input hooks provide cancellation even if activation fails.
    SetForegroundWindow(hWnd);
    SetActiveWindow(hWnd);
    SetFocus(hWnd);

    Wh_Log(L"Countdown foreground activation: %ls",
           GetForegroundWindow() == hWnd ? L"successful" : L"failed");

    bool running = true;
    while (running && IsWindow(hWnd)) {
        const DWORD waitResult = MsgWaitForMultipleObjects(
            1, &g_countdownStopEvent, FALSE, INFINITE, QS_ALLINPUT);

        if (waitResult == WAIT_OBJECT_0) {
            context->proceed = false;
            if (IsWindow(hWnd)) {
                DestroyWindow(hWnd);
            }
            break;
        }

        if (waitResult != WAIT_OBJECT_0 + 1) {
            Wh_Log(L"Countdown thread wait failed: %lu", GetLastError());
            context->proceed = true;
            if (IsWindow(hWnd)) {
                DestroyWindow(hWnd);
            }
            break;
        }

        MSG message;
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);

            if (!IsWindow(hWnd)) {
                running = false;
                break;
            }
        }
    }

    UninstallCountdownInputHooks(context);
    g_inputContext.store(nullptr, std::memory_order_release);

    if (IsWindow(hWnd)) {
        DestroyWindow(hWnd);
    }

    g_countdownWindow.store(nullptr, std::memory_order_release);
    SetEvent(context->doneEvent);
    return 0;
}

HANDLE AcquireCountdownMutex() {
    wchar_t mutexName[96];
    swprintf(mutexName, std::size(mutexName),
             L"Local\\Windhawk_%ls", WH_MOD_ID);

    HANDLE mutex = CreateMutexW(nullptr, FALSE, mutexName);
    if (!mutex) {
        return nullptr;
    }

    const DWORD waitResult = WaitForSingleObject(mutex, 0);
    if (waitResult != WAIT_OBJECT_0 && waitResult != WAIT_ABANDONED) {
        CloseHandle(mutex);
        return INVALID_HANDLE_VALUE;
    }

    return mutex;
}

void ReleaseCountdownMutex(HANDLE mutex) {
    if (mutex && mutex != INVALID_HANDLE_VALUE) {
        ReleaseMutex(mutex);
        CloseHandle(mutex);
    }
}

bool WaitForCountdownWithoutPumpingPostedMessages(HANDLE doneEvent) {
    HANDLE handles[] = {doneEvent};
    for (;;) {
        DWORD index = 0;
        HRESULT hr = CoWaitForMultipleHandles(
            COWAIT_DISPATCH_CALLS, INFINITE, std::size(handles), handles, &index);
        if (SUCCEEDED(hr) && index == 0) return true;
        if (hr == RPC_S_CALLPENDING) continue;
        Wh_Log(L"CoWaitForMultipleHandles failed: 0x%08X",
               static_cast<unsigned int>(hr));
        return false;
    }
}

bool RunCountdown(PowerAction action, int seconds) {
    HANDLE mutex = AcquireCountdownMutex();
    if (mutex == INVALID_HANDLE_VALUE) {
        Wh_Log(L"A countdown is already active; blocking duplicate action");
        return false;
    }

    if (!mutex) {
        Wh_Log(L"CreateMutexW failed: %lu; allowing power action",
               GetLastError());
        return true;
    }

    CountdownActivityGuard activity;
    if (!activity.Begin()) {
        ReleaseCountdownMutex(mutex);
        return true;
    }

    HANDLE doneEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!doneEvent) {
        Wh_Log(L"CreateEventW failed: %lu; allowing power action",
               GetLastError());
        ReleaseCountdownMutex(mutex);
        return true;
    }

    CountdownThreadContext context{
        .action = action,
        .seconds = seconds,
        .doneEvent = doneEvent,
        .endTick = 0,
        .displayedSeconds = seconds,
        .proceed = true,
        .keyboardHook = nullptr,
        .mouseHook = nullptr,
    };

    HANDLE thread = CreateThread(
        nullptr, 0, CountdownThreadProc, &context, 0, nullptr);
    if (!thread) {
        Wh_Log(L"CreateThread failed: %lu; allowing power action",
               GetLastError());
        CloseHandle(doneEvent);
        ReleaseCountdownMutex(mutex);
        return true;
    }

    if (!WaitForCountdownWithoutPumpingPostedMessages(doneEvent)) {
        SetEvent(g_countdownStopEvent);
    }

    // The event reports that cleanup has finished; waiting for the thread
    // handle additionally guarantees that no countdown-thread instruction is
    // still executing in the mod DLL.
    WaitForSingleObject(thread, INFINITE);

    CloseHandle(thread);
    CloseHandle(doneEvent);
    ReleaseCountdownMutex(mutex);

    Wh_Log(L"Countdown result: %ls",
           context.proceed ? L"execute" : L"cancel");
    return context.proceed;
}

bool IsLocalMachineName(const wchar_t* machineName) {
    if (!machineName || !*machineName) {
        return true;
    }

    while (*machineName == L'\\') {
        machineName++;
    }

    wchar_t localName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD localNameLength = std::size(localName);
    if (!GetComputerNameW(localName, &localNameLength)) {
        return false;
    }

    return _wcsicmp(machineName, localName) == 0 ||
           _wcsicmp(machineName, L"localhost") == 0 ||
           _wcsicmp(machineName, L".") == 0;
}

bool ConfirmAction(PowerAction action, const wchar_t* apiName) {
    if (g_unloading.load(std::memory_order_acquire)) {
        return true;
    }

    const int seconds = GetCountdownSeconds(action);
    if (seconds == 0) {
        Wh_Log(L"Intercepted %ls in process %lu; countdown disabled",
               apiName, GetCurrentProcessId());
        return true;
    }

    Wh_Log(L"Intercepted %ls in process %lu; countdown: %d seconds",
           apiName, GetCurrentProcessId(), seconds);
    return RunCountdown(action, seconds);
}

BOOL WINAPI ExitWindowsEx_Hook(UINT flags, DWORD reason) {
    if (g_bypassHooks || g_unloading.load(std::memory_order_acquire)) {
        return ExitWindowsEx_Original(flags, reason);
    }

    constexpr UINT kShutdownFlags =
        EWX_SHUTDOWN | EWX_POWEROFF | EWX_HYBRID_SHUTDOWN;

    const bool isShutdown = (flags & kShutdownFlags) != 0;
    const bool isRestart = (flags & EWX_REBOOT) != 0;

    PowerAction action{};
    bool handledAction = false;

    if (isRestart) {
        action = PowerAction::Restart;
        handledAction = true;
    } else if (isShutdown) {
        action = PowerAction::Shutdown;
        handledAction = true;
    }

    if (handledAction && !ConfirmAction(action, L"ExitWindowsEx")) {
        SetLastError(ERROR_CANCELLED);
        return FALSE;
    }

    BypassHooksGuard bypass;
    return ExitWindowsEx_Original(flags, reason);
}

DWORD WINAPI InitiateShutdownW_Hook(LPWSTR machineName,
                                    LPWSTR message,
                                    DWORD gracePeriod,
                                    DWORD shutdownFlags,
                                    DWORD reason) {
    if (g_bypassHooks || g_unloading.load(std::memory_order_acquire)) {
        return InitiateShutdownW_Original(machineName, message, gracePeriod,
                                          shutdownFlags, reason);
    }

    const bool isRestart = (shutdownFlags & SHUTDOWN_RESTART) != 0;

    if (IsLocalMachineName(machineName)) {
        const PowerAction action = isRestart
                                       ? PowerAction::Restart
                                       : PowerAction::Shutdown;
        if (!ConfirmAction(action, L"InitiateShutdownW")) {
            return ERROR_CANCELLED;
        }
    }

    BypassHooksGuard bypass;
    return InitiateShutdownW_Original(machineName, message, gracePeriod,
                                      shutdownFlags, reason);
}

BOOL WINAPI InitiateSystemShutdownExW_Hook(LPWSTR machineName,
                                           LPWSTR message,
                                           DWORD timeout,
                                           BOOL forceAppsClosed,
                                           BOOL rebootAfterShutdown,
                                           DWORD reason) {
    if (g_bypassHooks || g_unloading.load(std::memory_order_acquire)) {
        return InitiateSystemShutdownExW_Original(
            machineName, message, timeout, forceAppsClosed,
            rebootAfterShutdown, reason);
    }

    if (IsLocalMachineName(machineName)) {
        const PowerAction action = rebootAfterShutdown
                                       ? PowerAction::Restart
                                       : PowerAction::Shutdown;
        if (!ConfirmAction(action, L"InitiateSystemShutdownExW")) {
            SetLastError(ERROR_CANCELLED);
            return FALSE;
        }
    }

    BypassHooksGuard bypass;
    return InitiateSystemShutdownExW_Original(
        machineName, message, timeout, forceAppsClosed,
        rebootAfterShutdown, reason);
}

BOOL WINAPI SetSystemPowerState_Hook(BOOL suspend, BOOL force) {
    if (g_bypassHooks || g_unloading.load(std::memory_order_acquire)) {
        return SetSystemPowerState_Original(suspend, force);
    }

    const PowerAction action = suspend
                                   ? PowerAction::Sleep
                                   : PowerAction::Hibernate;
    if (!ConfirmAction(action, L"SetSystemPowerState")) {
        SetLastError(ERROR_CANCELLED);
        return FALSE;
    }

    BypassHooksGuard bypass;
    return SetSystemPowerState_Original(suspend, force);
}

bool MapNativePowerAction(POWER_ACTION systemAction,
                          SYSTEM_POWER_STATE minimumState,
                          PowerAction* action) {
    switch (systemAction) {
        case PowerActionSleep:
            *action = minimumState == PowerSystemHibernate
                          ? PowerAction::Hibernate
                          : PowerAction::Sleep;
            return true;

        case PowerActionHibernate:
            *action = PowerAction::Hibernate;
            return true;

        case PowerActionShutdownReset:
            *action = PowerAction::Restart;
            return true;

        case PowerActionShutdown:
        case PowerActionShutdownOff:
            *action = PowerAction::Shutdown;
            return true;

        default:
            return false;
    }
}

NTSTATUS NTAPI NtInitiatePowerAction_Hook(POWER_ACTION systemAction,
                                          SYSTEM_POWER_STATE minimumState,
                                          ULONG flags,
                                          BOOLEAN asynchronous) {
    if (g_bypassHooks || g_unloading.load(std::memory_order_acquire)) {
        return NtInitiatePowerAction_Original(systemAction, minimumState,
                                              flags, asynchronous);
    }

    PowerAction action{};
    if (MapNativePowerAction(systemAction, minimumState, &action) &&
        !ConfirmAction(action, L"NtInitiatePowerAction")) {
        return kStatusCancelled;
    }

    BypassHooksGuard bypass;
    return NtInitiatePowerAction_Original(systemAction, minimumState,
                                          flags, asynchronous);
}

NTSTATUS NTAPI NtSetSystemPowerState_Hook(POWER_ACTION systemAction,
                                          SYSTEM_POWER_STATE minimumState,
                                          ULONG flags) {
    if (g_bypassHooks || g_unloading.load(std::memory_order_acquire)) {
        return NtSetSystemPowerState_Original(systemAction, minimumState,
                                              flags);
    }

    PowerAction action{};
    if (MapNativePowerAction(systemAction, minimumState, &action) &&
        !ConfirmAction(action, L"NtSetSystemPowerState")) {
        return kStatusCancelled;
    }

    BypassHooksGuard bypass;
    return NtSetSystemPowerState_Original(systemAction, minimumState, flags);
}

bool InitializeModModuleHandle() {
    return GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&CountdownWindowProc),
        &g_modModule) != FALSE;
}

bool RegisterCountdownWindowClass() {
    swprintf(g_countdownWindowClassName,
             std::size(g_countdownWindowClassName),
             L"Windhawk_%ls_%lu",
             WH_MOD_ID,
             GetCurrentProcessId());

    WNDCLASSEXW windowClass{
        .cbSize = sizeof(windowClass),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = CountdownWindowProc,
        .hInstance = g_modModule,
        .hCursor = LoadCursorW(nullptr, IDC_ARROW),
        .lpszClassName = g_countdownWindowClassName,
    };

    g_countdownWindowClassAtom = RegisterClassExW(&windowClass);
    if (!g_countdownWindowClassAtom) {
        Wh_Log(L"RegisterClassExW failed: %lu", GetLastError());
        return false;
    }

    return true;
}

template <typename FunctionPointer>
bool HookExport(const wchar_t* moduleName, const char* functionName,
                FunctionPointer hookFunction, FunctionPointer* originalFunction) {
    HMODULE module = GetModuleHandleW(moduleName);
    if (!module) {
        Wh_Log(L"Module %ls isn't loaded; skipping %S", moduleName, functionName);
        return false;
    }
    auto target = reinterpret_cast<FunctionPointer>(GetProcAddress(module, functionName));
    if (!target) {
        Wh_Log(L"Couldn't find %S in %ls", functionName, moduleName);
        return false;
    }
    if (!WindhawkUtils::SetFunctionHook(target, hookFunction, originalFunction)) {
        Wh_Log(L"Couldn't hook %S", functionName);
        return false;
    }
    Wh_Log(L"Hooked %S", functionName);
    return true;
}

void CloseLifecycleHandles() {
    if (g_countdownStopEvent) {
        CloseHandle(g_countdownStopEvent);
        g_countdownStopEvent = nullptr;
    }

    if (g_countdownIdleEvent) {
        CloseHandle(g_countdownIdleEvent);
        g_countdownIdleEvent = nullptr;
    }
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();

    wchar_t processPath[MAX_PATH]{};
    GetModuleFileNameW(nullptr, processPath, std::size(processPath));
    Wh_Log(L"Initializing in process %lu: %ls",
           GetCurrentProcessId(), processPath);

    if (!InitializeModModuleHandle()) {
        Wh_Log(L"Couldn't resolve the mod module handle: %lu",
               GetLastError());
        return FALSE;
    }

    g_countdownStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_countdownIdleEvent = CreateEventW(nullptr, TRUE, TRUE, nullptr);
    if (!g_countdownStopEvent || !g_countdownIdleEvent) {
        Wh_Log(L"Couldn't create lifecycle events: %lu", GetLastError());
        CloseLifecycleHandles();
        return FALSE;
    }

    if (!RegisterCountdownWindowClass()) {
        // Never reuse an existing class: its WndProc might belong to an
        // unloaded instance of this mod.
        CloseLifecycleHandles();
        return FALSE;
    }

    bool hooked = false;
    hooked |= HookExport(L"user32.dll", "ExitWindowsEx",
                         ExitWindowsEx_Hook,
                         &ExitWindowsEx_Original);
    hooked |= HookExport(L"advapi32.dll", "InitiateShutdownW",
                         InitiateShutdownW_Hook,
                         &InitiateShutdownW_Original);
    hooked |= HookExport(L"advapi32.dll", "InitiateSystemShutdownExW",
                         InitiateSystemShutdownExW_Hook,
                         &InitiateSystemShutdownExW_Original);
    hooked |= HookExport(L"kernel32.dll", "SetSystemPowerState",
                         SetSystemPowerState_Hook,
                         &SetSystemPowerState_Original);
    hooked |= HookExport(L"ntdll.dll", "NtInitiatePowerAction",
                         NtInitiatePowerAction_Hook,
                         &NtInitiatePowerAction_Original);
    hooked |= HookExport(L"ntdll.dll", "NtSetSystemPowerState",
                         NtSetSystemPowerState_Hook,
                         &NtSetSystemPowerState_Original);

    if (!hooked) {
        Wh_Log(L"No supported power API could be hooked");
        UnregisterClassW(g_countdownWindowClassName, g_modModule);
        g_countdownWindowClassAtom = 0;
        CloseLifecycleHandles();
        return FALSE;
    }

    return TRUE;
}

void Wh_ModBeforeUninit() {
    AcquireSRWLockExclusive(&g_countdownLifecycleLock);
    g_unloading.store(true, std::memory_order_release);
    if (g_countdownStopEvent) {
        SetEvent(g_countdownStopEvent);
    }
    HWND hWnd = g_countdownWindow.load(std::memory_order_acquire);
    ReleaseSRWLockExclusive(&g_countdownLifecycleLock);

    if (hWnd) {
        PostMessageW(hWnd, WM_CLOSE, 0, 0);
    }

    if (g_countdownIdleEvent) {
        const DWORD waitResult =
            WaitForSingleObject(g_countdownIdleEvent, 5000);
        if (waitResult != WAIT_OBJECT_0) {
            Wh_Log(L"Timed out waiting for the countdown thread to stop: %lu",
                   waitResult == WAIT_FAILED ? GetLastError() : waitResult);
        }
    }
}

void Wh_ModUninit() {
    if (g_countdownWindowClassAtom) {
        if (!UnregisterClassW(g_countdownWindowClassName, g_modModule)) {
            Wh_Log(L"UnregisterClassW failed: %lu", GetLastError());
        }
        g_countdownWindowClassAtom = 0;
    }

    CloseLifecycleHandles();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}


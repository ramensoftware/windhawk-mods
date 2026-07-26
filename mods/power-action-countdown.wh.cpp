// ==WindhawkMod==
// @id              power-action-countdown
// @name            Power Action Countdown
// @description     Shows a configurable cancellable countdown before shutdown, restart, sleep, or hibernation.
// @version         1.0
// @author          Nerdworld
// @github          https://github.com/nerdworldDE
// @homepage        https://nerdworld.de/
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @include         RuntimeBroker.exe
// @include         sihost.exe
// @include         ShellExperienceHost.exe
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00 -luser32 -lgdi32 -ladvapi32 -lpowrprof
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Power Action Countdown

Shows an individually configurable countdown before Windows shuts down,
restarts, enters sleep, or enters hibernation. A value of `0` disables the
countdown for the corresponding action and executes it immediately.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- shutdownCountdownSeconds: 30
  $name: Shutdown countdown in seconds
- restartCountdownSeconds: 15
  $name: Restart countdown in seconds
- sleepCountdownSeconds: 5
  $name: Sleep countdown in seconds
- hibernateCountdownSeconds: 5
  $name: Hibernation countdown in seconds
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <powrprof.h>
#include <winreg.h>

#include <algorithm>
#include <atomic>
#include <cwchar>
#include <iterator>

namespace {

enum class PowerAction {
    Shutdown,
    Restart,
    Sleep,
    Hibernate,
};

std::atomic<int> g_shutdownCountdownSeconds{7};
std::atomic<int> g_restartCountdownSeconds{7};
std::atomic<int> g_sleepCountdownSeconds{7};
std::atomic<int> g_hibernateCountdownSeconds{7};
std::atomic<HWND> g_countdownWindow{nullptr};

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

struct CountdownContext {
    PowerAction action;
    ULONGLONG endTick;
    int displayedSeconds;
    bool proceed;
};

using ExitWindowsEx_t = BOOL(WINAPI*)(UINT, DWORD);
using InitiateShutdownW_t = DWORD(WINAPI*)(LPWSTR, LPWSTR, DWORD, DWORD, DWORD);
using InitiateSystemShutdownExW_t = BOOL(WINAPI*)(LPWSTR, LPWSTR, DWORD, BOOL, BOOL, DWORD);
using SetSuspendState_t = BOOLEAN(WINAPI*)(BOOLEAN, BOOLEAN, BOOLEAN);
using SetSystemPowerState_t = BOOL(WINAPI*)(BOOL, BOOL);
using NtInitiatePowerAction_t = NTSTATUS(NTAPI*)(
    POWER_ACTION, SYSTEM_POWER_STATE, ULONG, BOOLEAN);
using NtSetSystemPowerState_t = NTSTATUS(NTAPI*)(
    POWER_ACTION, SYSTEM_POWER_STATE, ULONG);

ExitWindowsEx_t ExitWindowsEx_Original = nullptr;
InitiateShutdownW_t InitiateShutdownW_Original = nullptr;
InitiateSystemShutdownExW_t InitiateSystemShutdownExW_Original = nullptr;
SetSuspendState_t SetSuspendState_Original = nullptr;
SetSystemPowerState_t SetSystemPowerState_Original = nullptr;
NtInitiatePowerAction_t NtInitiatePowerAction_Original = nullptr;
NtSetSystemPowerState_t NtSetSystemPowerState_Original = nullptr;

int ClampCountdownSetting(int seconds) {
    return std::clamp(seconds, 0, 60);
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

const wchar_t* GetActionTitle(PowerAction action) {
    switch (action) {
        case PowerAction::Shutdown:
            return L"Shutdown";
        case PowerAction::Restart:
            return L"Restart";
        case PowerAction::Sleep:
            return L"Standby";
        case PowerAction::Hibernate:
            return L"Hibernate";
    }

    return L"Power action";
}

void CancelCountdown(HWND hWnd) {
    auto* context = reinterpret_cast<CountdownContext*>(
        GetWindowLongPtrW(hWnd, GWLP_USERDATA));

    if (!context) {
        return;
    }

    context->proceed = false;
    DestroyWindow(hWnd);
}

bool WindowBelongsToProcess(HWND hWnd, const wchar_t* expectedProcessName) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (!processId) {
        return false;
    }

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,
                                 FALSE, processId);
    if (!process) {
        return false;
    }

    wchar_t processPath[MAX_PATH]{};
    DWORD processPathLength = std::size(processPath);
    const BOOL queried = QueryFullProcessImageNameW(
        process, 0, processPath, &processPathLength);
    CloseHandle(process);

    if (!queried) {
        return false;
    }

    const wchar_t* processName = wcsrchr(processPath, L'\\');
    processName = processName ? processName + 1 : processPath;
    return _wcsicmp(processName, expectedProcessName) == 0;
}

bool IsStartMenuWindow(HWND hWnd) {
    return WindowBelongsToProcess(hWnd, L"StartMenuExperienceHost.exe") ||
           WindowBelongsToProcess(hWnd, L"ShellExperienceHost.exe") ||
           WindowBelongsToProcess(hWnd, L"ShellHost.exe");
}

// The Windows 11 Start menu is an immersive shell surface in a separate
// Z-order band. A regular WS_EX_TOPMOST window can't reliably cover it while
// the shell thread is synchronously blocked inside the intercepted power API.
// Hide visible Start-menu shell surfaces before creating the overlay.
void HideStartMenuWindow(HWND hWnd) {
    HWND rootWindow = GetAncestor(hWnd, GA_ROOTOWNER);
    if (!rootWindow) {
        rootWindow = hWnd;
    }

    if (!IsWindowVisible(rootWindow) || !IsStartMenuWindow(rootWindow)) {
        return;
    }

    wchar_t className[128]{};
    GetClassNameW(rootWindow, className, std::size(className));
    Wh_Log(L"Dismissing Start menu window %p (%ls)", rootWindow,
           className[0] ? className : L"unknown class");

    PostMessageW(rootWindow, WM_CANCELMODE, 0, 0);

    const DWORD windowThread = GetWindowThreadProcessId(rootWindow, nullptr);
    if (windowThread == GetCurrentThreadId()) {
        ShowWindow(rootWindow, SW_HIDE);
    } else {
        ShowWindowAsync(rootWindow, SW_HIDE);
    }
}

BOOL CALLBACK DismissStartMenuEnumProc(HWND hWnd, LPARAM) {
    if (IsWindowVisible(hWnd) && IsStartMenuWindow(hWnd)) {
        HideStartMenuWindow(hWnd);
    }

    return TRUE;
}

void DismissStartMenu() {
    // Handle the foreground window first, then enumerate as a fallback. On
    // some Windows builds the power request has already shifted foreground
    // focus to RuntimeBroker by the time the API hook is entered.
    HWND foregroundWindow = GetForegroundWindow();
    if (foregroundWindow && IsStartMenuWindow(foregroundWindow)) {
        HideStartMenuWindow(foregroundWindow);
    }

    EnumWindows(DismissStartMenuEnumProc, 0);

    // Give shell UI threads a short chance to apply SW_HIDE before the overlay
    // is shown. No keyboard or mouse input is generated or consumed here.
    Sleep(100);
}

// Reassert the topmost state instead of relying only on WS_EX_TOPMOST at
// creation time. Windows shell surfaces such as the Start menu can change the
// Z-order while the countdown is already visible.
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

LRESULT CALLBACK CountdownWindowProc(HWND hWnd,
                                     UINT message,
                                     WPARAM wParam,
                                     LPARAM lParam) {
    auto* context = reinterpret_cast<CountdownContext*>(
        GetWindowLongPtrW(hWnd, GWLP_USERDATA));

    if (message == WM_NCCREATE) {
        auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
        context = static_cast<CountdownContext*>(create->lpCreateParams);
        SetWindowLongPtrW(hWnd, GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR>(context));
    }

    switch (message) {
        case WM_CREATE:
            SetTimer(hWnd, 1, 50, nullptr);
            return 0;

        case WM_TIMER: {
            // Keep the overlay above shell-owned windows for the complete
            // countdown, not only at the moment it is created.
            EnsureCountdownAlwaysOnTop(hWnd);

            if (!context) {
                return 0;
            }

            ULONGLONG now = GetTickCount64();
            if (now >= context->endTick) {
                context->displayedSeconds = 0;
                context->proceed = true;
                DestroyWindow(hWnd);
                return 0;
            }

            ULONGLONG remainingMs = context->endTick - now;
            int remainingSeconds =
                static_cast<int>((remainingMs + 999) / 1000);

            if (remainingSeconds != context->displayedSeconds) {
                context->displayedSeconds = remainingSeconds;
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            return 0;
        }

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
            // Prevent another window from demoting the countdown from the
            // topmost Z-order while it is active.
            auto* windowPos = reinterpret_cast<WINDOWPOS*>(lParam);
            if (windowPos) {
                windowPos->hwndInsertAfter = HWND_TOPMOST;
                windowPos->flags &= ~SWP_NOZORDER;
                windowPos->flags |= SWP_NOOWNERZORDER;
            }
            break;
        }

        case WM_ACTIVATE:
            if (LOWORD(wParam) != WA_INACTIVE) {
                EnsureCountdownAlwaysOnTop(hWnd);
            }
            break;

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

            int centerY = (clientRect.bottom - clientRect.top) / 2;

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
            DrawTextW(hdc,
                      context ? GetActionTitle(context->action) : L"Energieaktion",
                      -1, &titleRect,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

            RECT infoRect = clientRect;
            infoRect.top = centerY + MulDiv(95, dpi, 96);
            infoRect.bottom = centerY + MulDiv(150, dpi, 96);

            SelectObject(hdc, infoFont);
            SetTextColor(hdc, RGB(190, 190, 195));
            DrawTextW(hdc,
                      L"Taste, Mausklick oder Mausrad bricht ab. "
                      L"Mausbewegungen werden ignoriert.",
                      -1, &infoRect,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

            SelectObject(hdc, oldFont);
            DeleteObject(numberFont);
            DeleteObject(titleFont);
            DeleteObject(infoFont);

            EndPaint(hWnd, &paint);
            return 0;
        }

        case WM_DESTROY:
            KillTimer(hWnd, 1);
            if (g_countdownWindow.load(std::memory_order_relaxed) == hWnd) {
                g_countdownWindow.store(nullptr, std::memory_order_relaxed);
            }
            return 0;
    }

    return DefWindowProcW(hWnd, message, wParam, lParam);
}

bool BringCountdownToForeground(HWND hWnd) {
    HWND foregroundWindow = GetForegroundWindow();
    DWORD foregroundThread = foregroundWindow
                                 ? GetWindowThreadProcessId(foregroundWindow,
                                                            nullptr)
                                 : 0;
    DWORD currentThread = GetCurrentThreadId();

    bool attached = false;
    if (foregroundThread && foregroundThread != currentThread) {
        attached = AttachThreadInput(currentThread, foregroundThread, TRUE) != FALSE;
    }

    LONG_PTR extendedStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if ((extendedStyle & WS_EX_TOPMOST) == 0) {
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE,
                          extendedStyle | WS_EX_TOPMOST);
    }

    SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW |
                     SWP_FRAMECHANGED);
    BringWindowToTop(hWnd);
    SetForegroundWindow(hWnd);
    SetActiveWindow(hWnd);
    SetFocus(hWnd);

    if (attached) {
        AttachThreadInput(currentThread, foregroundThread, FALSE);
    }

    const bool isForeground = GetForegroundWindow() == hWnd;
    Wh_Log(L"Countdown foreground activation: %ls",
           isForeground ? L"successful" : L"failed");
    return isForeground;
}

HANDLE AcquireCountdownMutex() {
    DWORD sessionId = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);

    wchar_t mutexName[96];
    swprintf(mutexName, std::size(mutexName),
             L"Local\\Windhawk_%ls_Session_%lu", WH_MOD_ID, sessionId);

    HANDLE mutex = CreateMutexW(nullptr, FALSE, mutexName);
    if (!mutex) {
        return nullptr;
    }

    DWORD waitResult = WaitForSingleObject(mutex, 0);
    if (waitResult != WAIT_OBJECT_0 && waitResult != WAIT_ABANDONED) {
        CloseHandle(mutex);
        return INVALID_HANDLE_VALUE;
    }

    return mutex;
}

bool RunCountdown(PowerAction action, int seconds) {
    HANDLE mutex = AcquireCountdownMutex();
    if (mutex == INVALID_HANDLE_VALUE) {
        Wh_Log(L"A countdown is already active; blocking duplicate action");
        return false;
    }

    DismissStartMenu();

    CountdownContext context{
        .action = action,
        .endTick = GetTickCount64() + static_cast<ULONGLONG>(seconds) * 1000,
        .displayedSeconds = seconds,
        .proceed = false,
    };

    wchar_t className[96];
    swprintf(className, std::size(className), L"Windhawk_%ls_%lu",
             WH_MOD_ID, GetCurrentProcessId());

    HINSTANCE instance = GetModuleHandleW(nullptr);
    WNDCLASSEXW windowClass{
        .cbSize = sizeof(windowClass),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = CountdownWindowProc,
        .hInstance = instance,
        .hCursor = LoadCursorW(nullptr, IDC_ARROW),
        .lpszClassName = className,
    };

    ATOM classAtom = RegisterClassExW(&windowClass);
    if (!classAtom && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"RegisterClassExW failed: %lu", GetLastError());
        if (mutex) {
            ReleaseMutex(mutex);
            CloseHandle(mutex);
        }
        return true;  // Fail open: don't break Windows power actions.
    }

    // Show the countdown only on the primary monitor. Using the virtual
    // screen rectangle would center the content between multiple monitors.
    POINT primaryMonitorPoint{0, 0};
    HMONITOR primaryMonitor = MonitorFromPoint(
        primaryMonitorPoint, MONITOR_DEFAULTTOPRIMARY);

    MONITORINFO monitorInfo{
        .cbSize = sizeof(monitorInfo),
    };

    RECT monitorRect{};
    if (primaryMonitor && GetMonitorInfoW(primaryMonitor, &monitorInfo)) {
        monitorRect = monitorInfo.rcMonitor;
    } else {
        // Fallback to the primary-screen metrics if monitor lookup fails.
        monitorRect = RECT{
            .left = 0,
            .top = 0,
            .right = GetSystemMetrics(SM_CXSCREEN),
            .bottom = GetSystemMetrics(SM_CYSCREEN),
        };
    }

    const int x = monitorRect.left;
    const int y = monitorRect.top;
    const int width = monitorRect.right - monitorRect.left;
    const int height = monitorRect.bottom - monitorRect.top;

    HWND hWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        className,
        L"Power action countdown",
        WS_POPUP,
        x,
        y,
        width,
        height,
        nullptr,
        nullptr,
        instance,
        &context);

    if (!hWnd) {
        Wh_Log(L"CreateWindowExW failed: %lu", GetLastError());
        UnregisterClassW(className, instance);
        if (mutex) {
            ReleaseMutex(mutex);
            CloseHandle(mutex);
        }
        return true;  // Fail open.
    }

    g_countdownWindow.store(hWnd, std::memory_order_relaxed);

    ShowWindow(hWnd, SW_SHOW);
    EnsureCountdownAlwaysOnTop(hWnd);
    UpdateWindow(hWnd);
    BringCountdownToForeground(hWnd);
    EnsureCountdownAlwaysOnTop(hWnd);
    SetCapture(hWnd);

    MSG message;
    while (IsWindow(hWnd)) {
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == WM_QUIT) {
                PostQuitMessage(static_cast<int>(message.wParam));
                context.proceed = false;
                if (IsWindow(hWnd)) {
                    DestroyWindow(hWnd);
                }
                break;
            }

            TranslateMessage(&message);
            DispatchMessageW(&message);

            if (!IsWindow(hWnd)) {
                break;
            }
        }

        if (IsWindow(hWnd)) {
            MsgWaitForMultipleObjects(0, nullptr, FALSE, 50, QS_ALLINPUT);
        }
    }

    if (GetCapture() == hWnd) {
        ReleaseCapture();
    }

    UnregisterClassW(className, instance);

    if (mutex) {
        ReleaseMutex(mutex);
        CloseHandle(mutex);
    }

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
    if (g_bypassHooks) {
        return ExitWindowsEx_Original(flags, reason);
    }

    constexpr UINT kShutdownFlags =
        EWX_SHUTDOWN | EWX_POWEROFF | EWX_HYBRID_SHUTDOWN;

    const bool isShutdown = (flags & kShutdownFlags) != 0;
    const bool isRestart = (flags & EWX_REBOOT) != 0;

    PowerAction action;
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
    if (g_bypassHooks) {
        return InitiateShutdownW_Original(machineName, message, gracePeriod,
                                          shutdownFlags, reason);
    }

    const bool isRestart =
        (shutdownFlags & (SHUTDOWN_RESTART | SHUTDOWN_RESTARTAPPS)) != 0;

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
    if (g_bypassHooks) {
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

BOOLEAN WINAPI SetSuspendState_Hook(BOOLEAN hibernate,
                                    BOOLEAN force,
                                    BOOLEAN disableWakeEvents) {
    if (g_bypassHooks) {
        return SetSuspendState_Original(hibernate, force, disableWakeEvents);
    }

    const PowerAction action = hibernate
                                   ? PowerAction::Hibernate
                                   : PowerAction::Sleep;
    if (!ConfirmAction(action, L"SetSuspendState")) {
        SetLastError(ERROR_CANCELLED);
        return FALSE;
    }

    BypassHooksGuard bypass;
    return SetSuspendState_Original(hibernate, force, disableWakeEvents);
}

BOOL WINAPI SetSystemPowerState_Hook(BOOL suspend, BOOL force) {
    if (g_bypassHooks) {
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

NTSTATUS GetCancelledNtStatus() {
    return static_cast<NTSTATUS>(0xC0000120L);
}

NTSTATUS NTAPI NtInitiatePowerAction_Hook(POWER_ACTION systemAction,
                                          SYSTEM_POWER_STATE minimumState,
                                          ULONG flags,
                                          BOOLEAN asynchronous) {
    if (g_bypassHooks) {
        return NtInitiatePowerAction_Original(systemAction, minimumState,
                                              flags, asynchronous);
    }

    PowerAction action;
    if (MapNativePowerAction(systemAction, minimumState, &action) &&
        !ConfirmAction(action, L"NtInitiatePowerAction")) {
        return GetCancelledNtStatus();
    }

    BypassHooksGuard bypass;
    return NtInitiatePowerAction_Original(systemAction, minimumState,
                                          flags, asynchronous);
}

NTSTATUS NTAPI NtSetSystemPowerState_Hook(POWER_ACTION systemAction,
                                          SYSTEM_POWER_STATE minimumState,
                                          ULONG flags) {
    if (g_bypassHooks) {
        return NtSetSystemPowerState_Original(systemAction, minimumState,
                                              flags);
    }

    PowerAction action;
    if (MapNativePowerAction(systemAction, minimumState, &action) &&
        !ConfirmAction(action, L"NtSetSystemPowerState")) {
        return GetCancelledNtStatus();
    }

    BypassHooksGuard bypass;
    return NtSetSystemPowerState_Original(systemAction, minimumState, flags);
}

template <typename T>
bool HookExport(const wchar_t* moduleName,
                const char* functionName,
                void* hookFunction,
                T* originalFunction) {
    HMODULE module = GetModuleHandleW(moduleName);
    if (!module) {
        module = LoadLibraryW(moduleName);
    }

    if (!module) {
        Wh_Log(L"Couldn't load %ls: %lu", moduleName, GetLastError());
        return false;
    }

    FARPROC target = GetProcAddress(module, functionName);
    if (!target) {
        Wh_Log(L"Couldn't find %S in %ls", functionName, moduleName);
        return false;
    }

    if (!Wh_SetFunctionHook(reinterpret_cast<void*>(target), hookFunction,
                            reinterpret_cast<void**>(originalFunction))) {
        Wh_Log(L"Couldn't hook %S", functionName);
        return false;
    }

    Wh_Log(L"Hooked %S", functionName);
    return true;
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();

    wchar_t processPath[MAX_PATH]{};
    GetModuleFileNameW(nullptr, processPath, std::size(processPath));
    Wh_Log(L"Initializing in process %lu: %ls",
           GetCurrentProcessId(), processPath);

    bool hooked = false;
    hooked |= HookExport(L"user32.dll", "ExitWindowsEx",
                         reinterpret_cast<void*>(ExitWindowsEx_Hook),
                         &ExitWindowsEx_Original);
    hooked |= HookExport(L"advapi32.dll", "InitiateShutdownW",
                         reinterpret_cast<void*>(InitiateShutdownW_Hook),
                         &InitiateShutdownW_Original);
    hooked |= HookExport(L"advapi32.dll", "InitiateSystemShutdownExW",
                         reinterpret_cast<void*>(InitiateSystemShutdownExW_Hook),
                         &InitiateSystemShutdownExW_Original);
    hooked |= HookExport(L"powrprof.dll", "SetSuspendState",
                         reinterpret_cast<void*>(SetSuspendState_Hook),
                         &SetSuspendState_Original);
    hooked |= HookExport(L"kernel32.dll", "SetSystemPowerState",
                         reinterpret_cast<void*>(SetSystemPowerState_Hook),
                         &SetSystemPowerState_Original);
    hooked |= HookExport(L"ntdll.dll", "NtInitiatePowerAction",
                         reinterpret_cast<void*>(NtInitiatePowerAction_Hook),
                         &NtInitiatePowerAction_Original);
    hooked |= HookExport(L"ntdll.dll", "NtSetSystemPowerState",
                         reinterpret_cast<void*>(NtSetSystemPowerState_Hook),
                         &NtSetSystemPowerState_Original);

    if (!hooked) {
        Wh_Log(L"No supported power API could be hooked");
    }

    return TRUE;
}

void Wh_ModBeforeUninit() {
    HWND hWnd = g_countdownWindow.load(std::memory_order_relaxed);
    if (hWnd) {
        PostMessageW(hWnd, WM_CLOSE, 0, 0);
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

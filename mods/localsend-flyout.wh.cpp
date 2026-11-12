// ==WindhawkMod==
// @id              localsend-flyout
// @name            LocalSend Flyout
// @description     LocalSend flyout inspired by Quick Share Flyout; created for LocalSend with vibecode
// @version         1.0.0
// @author          blactin
// @github          https://github.com/blactin
// @include         localsend_app.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -ldwmapi -lshcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# LocalSend Flyout

Turns the official LocalSend for Windows window into a compact, tray-adjacent
flyout. Click LocalSend's tray icon to open it; click elsewhere to dismiss it.
Use LocalSend's own **Send** page to pick or drop files, then choose a device
as usual.

## Inspiration

**Original mod:** [Quick Share Flyout](https://windhawk.net/mods/quick-share-flyout)
by [0Allu](https://github.com/0Allu).

Quick Share Flyout inspired the tray-adjacent flyout interaction. This is a
separate LocalSend-specific implementation, created with **vibecode**: it
targets LocalSend's own Windows window and preserves LocalSend's established
transfer and approval flow. It does not copy the original mod blindly.

This mod only changes the LocalSend window. It does not inspect files, call
LocalSend's network API, start a CLI process, select recipients, or bypass a
receiver's approval settings. Transfers remain LocalSend transfers, including
its normal device discovery and HTTPS/TLS protection.

## Controls

* Left-click the LocalSend tray icon to open the flyout.
* Click the tray icon again to hide it.
* Drag files onto the LocalSend UI, or use its file picker.
* Click outside the flyout to hide it (unless disabled in settings).

The mod deliberately leaves notification windows alone. That keeps it confined
to the LocalSend process and makes disabling it completely reversible.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- width: 500
  $name: Width
  $description: Flyout width in pixels at 100% display scaling.
- height: 620
  $name: Height
  $description: Flyout height in pixels at 100% display scaling.
- edgeMargin: 12
  $name: Screen-edge margin
  $description: Margin in pixels at 100% display scaling.
- closeWhenFocusIsLost: true
  $name: Close when focus is lost
  $description: Hide the flyout after clicking elsewhere. It stays open while files are dragged onto it.
- hideFromTaskbar: true
  $name: Hide from taskbar and Alt+Tab
  $description: Takes full effect after LocalSend is hidden and shown again.
- keepTopmost: true
  $name: Keep above normal windows
  $description: Keeps the flyout convenient while dragging files from File Explorer.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <shellscalingapi.h>

#include <algorithm>
#include <atomic>

// LocalSend's current Windows runner creates a top-level
// FLUTTER_RUNNER_WIN32_WINDOW with this initial title. Both checks make the
// hook ignore Flutter/plugin windows and native file-picker dialogs.
constexpr PCWSTR kLocalSendWindowClass = L"FLUTTER_RUNNER_WIN32_WINDOW";
constexpr PCWSTR kLocalSendTitle = L"LocalSend";

// Use numeric DWM attributes so this source also builds with SDKs that predate
// Windows 11's DWM_WINDOW_CORNER_PREFERENCE enum. Older systems simply ignore
// the corner preference.
constexpr DWMWINDOWATTRIBUTE kDwmTransitionsForcedDisabled =
    static_cast<DWMWINDOWATTRIBUTE>(3);
constexpr DWMWINDOWATTRIBUTE kDwmWindowCornerPreference =
    static_cast<DWMWINDOWATTRIBUTE>(33);
constexpr int kDwmDefaultCorners = 0;
constexpr int kDwmRoundCorners = 2;

constexpr UINT_PTR kClickAwayTimer = 0x4C53464C;
constexpr UINT kClickAwayIntervalMs = 60;
constexpr DWORD kTrayFollowupGraceMs = 350;
constexpr DWORD kTrayCloseSuppressMs = 450;

struct Settings {
    int width;
    int height;
    int edgeMargin;
    bool closeWhenFocusIsLost;
    bool hideFromTaskbar;
    bool keepTopmost;
};

// g_settings is used only by the LocalSend UI thread. The Windhawk settings
// callback can run on another thread, so it writes g_pendingSettings under a
// lock and asks the UI thread to apply a fresh snapshot.
CRITICAL_SECTION g_settingsLock;
Settings g_pendingSettings{};
Settings g_settings{};

std::atomic<HWND> g_window;
std::atomic<bool> g_unloading;
UINT g_applyMessage;
UINT g_settingsMessage;
UINT g_showRequestMessage;

// Everything below is owned by LocalSend's UI thread.
bool g_flyoutEnabled;
bool g_addedToolWindow;
bool g_buttonWasDown;
DWORD g_lastShownAt;
DWORD g_ignoreShowRequestsUntil;
RECT g_flyoutRect{};

struct OriginalWindowState {
    bool captured = false;
    RECT rect{};
    bool wasTopmost = false;
};
OriginalWindowState g_original;

bool IsOwnWindow(HWND hWnd) {
    DWORD processId = 0;
    return hWnd && GetWindowThreadProcessId(hWnd, &processId) &&
           processId == GetCurrentProcessId();
}

bool IsLocalSendMainWindow(HWND hWnd) {
    if (!IsOwnWindow(hWnd) || GetAncestor(hWnd, GA_ROOT) != hWnd) return false;

    WCHAR className[64] = {};
    WCHAR title[64] = {};
    return GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
           wcscmp(className, kLocalSendWindowClass) == 0 &&
           GetWindowTextW(hWnd, title, ARRAYSIZE(title)) &&
           wcscmp(title, kLocalSendTitle) == 0;
}

bool RootHasClass(HWND hWnd, PCWSTR expectedClass) {
    HWND root = hWnd ? GetAncestor(hWnd, GA_ROOT) : nullptr;
    WCHAR className[64] = {};
    return root && GetClassNameW(root, className, ARRAYSIZE(className)) &&
           wcscmp(className, expectedClass) == 0;
}

bool IsTrayOrOverflow(HWND hWnd) {
    return RootHasClass(hWnd, L"Shell_TrayWnd") ||
           RootHasClass(hWnd, L"Shell_SecondaryTrayWnd") ||
           RootHasClass(hWnd, L"TopLevelWindowForOverflowXamlIsland") ||
           RootHasClass(hWnd, L"NotifyIconOverflowWindow");
}

bool CursorIsOverTray() {
    POINT cursor{};
    return GetCursorPos(&cursor) && IsTrayOrOverflow(WindowFromPoint(cursor));
}

HWND FindTaskbarForMonitor(HMONITOR monitor) {
    HWND primary = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (primary && MonitorFromWindow(primary, MONITOR_DEFAULTTONEAREST) == monitor) {
        return primary;
    }

    HWND taskbar = nullptr;
    while ((taskbar = FindWindowExW(nullptr, taskbar, L"Shell_SecondaryTrayWnd", nullptr))) {
        if (MonitorFromWindow(taskbar, MONITOR_DEFAULTTONEAREST) == monitor) return taskbar;
    }
    return primary;
}

enum class TaskbarEdge { Top, Bottom, Left, Right };

TaskbarEdge GetTaskbarEdge(HMONITOR monitor, const MONITORINFO& monitorInfo) {
    HWND taskbar = FindTaskbarForMonitor(monitor);
    RECT trayRect{};
    if (taskbar && GetWindowRect(taskbar, &trayRect)) {
        const RECT& screen = monitorInfo.rcMonitor;
        LONG trayWidth = trayRect.right - trayRect.left;
        LONG trayHeight = trayRect.bottom - trayRect.top;
        if (trayWidth >= trayHeight) {
            return trayRect.top <= screen.top ? TaskbarEdge::Top : TaskbarEdge::Bottom;
        }
        return trayRect.left <= screen.left ? TaskbarEdge::Left : TaskbarEdge::Right;
    }

    // The work area is the best fallback when Explorer has not created a
    // taskbar window yet. It also covers a secondary monitor without a tray.
    const RECT& work = monitorInfo.rcWork;
    const RECT& screen = monitorInfo.rcMonitor;
    if (work.top > screen.top) return TaskbarEdge::Top;
    if (work.left > screen.left) return TaskbarEdge::Left;
    if (work.right < screen.right) return TaskbarEdge::Right;
    return TaskbarEdge::Bottom;
}

int ScaleForMonitor(int value, HMONITOR monitor) {
    UINT dpiX = USER_DEFAULT_SCREEN_DPI;
    UINT dpiY = USER_DEFAULT_SCREEN_DPI;
    if (FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        dpiX = USER_DEFAULT_SCREEN_DPI;
    }
    return MulDiv(value, static_cast<int>(dpiX), USER_DEFAULT_SCREEN_DPI);
}

void CalculateFlyoutRect(HWND hWnd, bool useCursorMonitor) {
    POINT cursor{};
    GetCursorPos(&cursor);
    HMONITOR monitor = useCursorMonitor
                            ? MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST)
                            : MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfoW(monitor, &monitorInfo);

    const RECT& work = monitorInfo.rcWork;
    int gap = std::max(0, ScaleForMonitor(g_settings.edgeMargin, monitor));
    int maximumWidth = std::max(1, static_cast<int>(work.right - work.left - 2L * gap));
    int maximumHeight = std::max(1, static_cast<int>(work.bottom - work.top - 2L * gap));
    int width = std::min(ScaleForMonitor(g_settings.width, monitor), maximumWidth);
    int height = std::min(ScaleForMonitor(g_settings.height, monitor), maximumHeight);

    TaskbarEdge edge = GetTaskbarEdge(monitor, monitorInfo);
    bool fromTray = useCursorMonitor && CursorIsOverTray();
    int x = work.right - width - gap;
    int y = work.bottom - height - gap;

    switch (edge) {
        case TaskbarEdge::Top:
            y = work.top + gap;
            if (fromTray) x = cursor.x - width / 2;
            break;
        case TaskbarEdge::Bottom:
            y = work.bottom - height - gap;
            if (fromTray) x = cursor.x - width / 2;
            break;
        case TaskbarEdge::Left:
            x = work.left + gap;
            if (fromTray) y = cursor.y - height / 2;
            break;
        case TaskbarEdge::Right:
            x = work.right - width - gap;
            if (fromTray) y = cursor.y - height / 2;
            break;
    }

    int minimumX = static_cast<int>(work.left) + gap;
    int minimumY = static_cast<int>(work.top) + gap;
    int maximumX = std::max(minimumX, static_cast<int>(work.right) - width - gap);
    int maximumY = std::max(minimumY, static_cast<int>(work.bottom) - height - gap);
    x = std::clamp(x, minimumX, maximumX);
    y = std::clamp(y, minimumY, maximumY);
    g_flyoutRect = {x, y, x + width, y + height};
}

void CaptureOriginalWindowState(HWND hWnd) {
    if (g_original.captured) return;
    if (!GetWindowRect(hWnd, &g_original.rect)) return;
    g_original.wasTopmost = (GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
    g_original.captured = true;
}

void SetFlyoutDwmAttributes(HWND hWnd, bool enabled) {
    BOOL transitionsDisabled = enabled;
    DwmSetWindowAttribute(hWnd, kDwmTransitionsForcedDisabled, &transitionsDisabled,
                          sizeof(transitionsDisabled));
    int cornerPreference = enabled ? kDwmRoundCorners : kDwmDefaultCorners;
    DwmSetWindowAttribute(hWnd, kDwmWindowCornerPreference, &cornerPreference,
                          sizeof(cornerPreference));
}

void SetToolWindowStyle(HWND hWnd, bool shouldHide) {
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (shouldHide && !(exStyle & WS_EX_TOOLWINDOW)) {
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_TOOLWINDOW);
        g_addedToolWindow = true;
    } else if (!shouldHide && g_addedToolWindow) {
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle & ~WS_EX_TOOLWINDOW);
        g_addedToolWindow = false;
    }
}

void MoveToFlyoutRect(HWND hWnd, bool useCursorMonitor) {
    CalculateFlyoutRect(hWnd, useCursorMonitor);
    SetWindowPos(hWnd, g_settings.keepTopmost ? HWND_TOPMOST : HWND_NOTOPMOST,
                 g_flyoutRect.left, g_flyoutRect.top,
                 g_flyoutRect.right - g_flyoutRect.left,
                 g_flyoutRect.bottom - g_flyoutRect.top,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
}

void RestoreOriginalWindowState(HWND hWnd) {
    SetFlyoutDwmAttributes(hWnd, false);
    SetToolWindowStyle(hWnd, false);

    if (!g_original.captured) {
        SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER |
                         SWP_FRAMECHANGED);
        return;
    }

    const RECT& rect = g_original.rect;
    SetWindowPos(hWnd, g_original.wasTopmost ? HWND_TOPMOST : HWND_NOTOPMOST,
                 rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    g_original = {};
}

Settings ReadWindhawkSettings() {
    Settings settings{};
    settings.width = std::clamp(Wh_GetIntSetting(L"width"), 320, 1200);
    settings.height = std::clamp(Wh_GetIntSetting(L"height"), 360, 1200);
    settings.edgeMargin = std::clamp(Wh_GetIntSetting(L"edgeMargin"), 0, 80);
    settings.closeWhenFocusIsLost = Wh_GetIntSetting(L"closeWhenFocusIsLost") != 0;
    settings.hideFromTaskbar = Wh_GetIntSetting(L"hideFromTaskbar") != 0;
    settings.keepTopmost = Wh_GetIntSetting(L"keepTopmost") != 0;
    return settings;
}

void StorePendingSettings(const Settings& settings) {
    EnterCriticalSection(&g_settingsLock);
    g_pendingSettings = settings;
    LeaveCriticalSection(&g_settingsLock);
}

Settings TakePendingSettings() {
    EnterCriticalSection(&g_settingsLock);
    Settings settings = g_pendingSettings;
    LeaveCriticalSection(&g_settingsLock);
    return settings;
}

bool AnyMouseButtonDown(bool* wasPressedSinceLastCall = nullptr) {
    bool down = false;
    for (int button : {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON}) {
        SHORT state = GetAsyncKeyState(button);
        down = down || state < 0;
        if (wasPressedSinceLastCall) *wasPressedSinceLastCall |= (state & 1) != 0;
    }
    return down;
}

bool CursorIsOverFlyout(HWND hWnd) {
    POINT cursor{};
    return GetCursorPos(&cursor) && GetAncestor(WindowFromPoint(cursor), GA_ROOT) == hWnd;
}

void HideFlyout(HWND hWnd) {
    KillTimer(hWnd, kClickAwayTimer);
    g_buttonWasDown = false;
    // Call the original to avoid treating this deliberate hide as a tray show
    // request when LocalSend's window-manager plugin reacts to activation.
    extern decltype(&ShowWindow) ShowWindow_Original;
    ShowWindow_Original(hWnd, SW_HIDE);
}

void StartOrContinueClickAwayWatch(HWND hWnd) {
    if (!g_settings.closeWhenFocusIsLost || !IsWindowVisible(hWnd)) return;
    bool pressed = false;
    g_buttonWasDown = AnyMouseButtonDown(&pressed) || pressed;
    SetTimer(hWnd, kClickAwayTimer, kClickAwayIntervalMs, nullptr);
}

void CheckClickAway(HWND hWnd) {
    if (!IsWindowVisible(hWnd) || IsOwnWindow(GetForegroundWindow())) {
        KillTimer(hWnd, kClickAwayTimer);
        g_buttonWasDown = false;
        return;
    }

    bool pressed = false;
    bool down = AnyMouseButtonDown(&pressed);
    bool releasedOutside = !down && (g_buttonWasDown || pressed) && !CursorIsOverFlyout(hWnd);
    g_buttonWasDown = down;
    if (releasedOutside) HideFlyout(hWnd);
}

// Called on LocalSend's UI thread. Only a request originating over the tray
// toggles an already-visible window; other calls to show/focus LocalSend keep
// their normal behavior (for example an incoming transfer notification).
bool ConsumeTrayShowRequest(HWND hWnd) {
    if (!g_flyoutEnabled || !CursorIsOverTray()) return false;

    DWORD now = GetTickCount();
    if (static_cast<LONG>(now - g_ignoreShowRequestsUntil) < 0) return true;

    if (!IsWindowVisible(hWnd)) return false;

    // window_manager.show() is normally followed by focus(). Do not mistake
    // that second API call for a second tray click.
    if (now - g_lastShownAt < kTrayFollowupGraceMs) return false;

    HideFlyout(hWnd);
    g_ignoreShowRequestsUntil = now + kTrayCloseSuppressMs;
    return true;
}

bool DispatchTrayShowRequest(HWND hWnd) {
    if (!hWnd || hWnd != g_window || !g_flyoutEnabled) return false;
    if (GetWindowThreadProcessId(hWnd, nullptr) == GetCurrentThreadId()) {
        return ConsumeTrayShowRequest(hWnd);
    }
    return SendMessageW(hWnd, g_showRequestMessage, 0, 0) != 0;
}

void ApplyFlyoutMode(HWND hWnd, bool enabled) {
    if (enabled) {
        CaptureOriginalWindowState(hWnd);
        g_flyoutEnabled = true;
        SetToolWindowStyle(hWnd, g_settings.hideFromTaskbar);
        SetFlyoutDwmAttributes(hWnd, true);
        SetWindowPos(hWnd, g_settings.keepTopmost ? HWND_TOPMOST : HWND_NOTOPMOST,
                     0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER |
                         SWP_FRAMECHANGED);
        if (IsWindowVisible(hWnd)) MoveToFlyoutRect(hWnd, false);
    } else {
        g_flyoutEnabled = false;
        KillTimer(hWnd, kClickAwayTimer);
        RestoreOriginalWindowState(hWnd);
    }
}

LRESULT CALLBACK LocalSendSubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam,
                                       DWORD_PTR) {
    if (message == g_applyMessage) {
        ApplyFlyoutMode(hWnd, wParam != 0);
        return 0;
    }
    if (message == g_settingsMessage) {
        g_settings = TakePendingSettings();
        ApplyFlyoutMode(hWnd, true);
        return 0;
    }
    if (message == g_showRequestMessage) return DispatchTrayShowRequest(hWnd);

    if (message == WM_NCDESTROY) {
        KillTimer(hWnd, kClickAwayTimer);
        g_window = nullptr;
        g_flyoutEnabled = false;
        g_original = {};
        return DefSubclassProc(hWnd, message, wParam, lParam);
    }

    if (!g_flyoutEnabled) return DefSubclassProc(hWnd, message, wParam, lParam);

    switch (message) {
        case WM_NCCALCSIZE:
            // The Flutter content fills the frame; Windows still supplies the
            // shadow and DWM rounded corners.
            if (wParam) return 0;
            break;

        case WM_GETMINMAXINFO: {
            LRESULT result = DefSubclassProc(hWnd, message, wParam, lParam);
            auto* info = reinterpret_cast<MINMAXINFO*>(lParam);
            info->ptMinTrackSize = {0, 0};
            return result;
        }

        case WM_WINDOWPOSCHANGING: {
            auto* position = reinterpret_cast<WINDOWPOS*>(lParam);
            if (position->flags & SWP_HIDEWINDOW) {
                KillTimer(hWnd, kClickAwayTimer);
                break;
            }
            bool becomingVisible = !IsWindowVisible(hWnd) && (position->flags & SWP_SHOWWINDOW);
            if (becomingVisible) {
                CalculateFlyoutRect(hWnd, true);
                g_lastShownAt = GetTickCount();
            } else if (g_flyoutRect.right <= g_flyoutRect.left ||
                       g_flyoutRect.bottom <= g_flyoutRect.top) {
                CalculateFlyoutRect(hWnd, false);
            }
            position->x = g_flyoutRect.left;
            position->y = g_flyoutRect.top;
            position->cx = g_flyoutRect.right - g_flyoutRect.left;
            position->cy = g_flyoutRect.bottom - g_flyoutRect.top;
            position->flags &= ~(SWP_NOMOVE | SWP_NOSIZE);
            if (g_settings.keepTopmost && !(position->flags & SWP_NOZORDER)) {
                position->hwndInsertAfter = HWND_TOPMOST;
            }
            break;
        }

        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE && !IsOwnWindow(reinterpret_cast<HWND>(lParam))) {
                StartOrContinueClickAwayWatch(hWnd);
            }
            break;

        case WM_SYSCOMMAND:
            if ((wParam & 0xFFF0) == SC_MINIMIZE) {
                HideFlyout(hWnd);
                return 0;
            }
            if ((wParam & 0xFFF0) == SC_MAXIMIZE) return 0;
            break;

        case WM_TIMER:
            if (wParam == kClickAwayTimer) {
                CheckClickAway(hWnd);
                return 0;
            }
            break;
    }

    return DefSubclassProc(hWnd, message, wParam, lParam);
}

void Attach(HWND hWnd) {
    HWND expected = nullptr;
    if (!g_window.compare_exchange_strong(expected, hWnd)) return;

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, LocalSendSubclassProc, 0)) {
        Wh_Log(L"LocalSend Flyout: failed to subclass the LocalSend window");
        g_window = nullptr;
        return;
    }
    SendMessageW(hWnd, g_applyMessage, TRUE, 0);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR className, LPCWSTR windowName,
                                 DWORD style, int x, int y, int width, int height,
                                 HWND parent, HMENU menu, HINSTANCE instance, LPVOID parameter) {
    HWND hWnd = CreateWindowExW_Original(exStyle, className, windowName, style, x, y, width, height,
                                         parent, menu, instance, parameter);
    if (hWnd && !parent && !g_unloading && !g_window && IsLocalSendMainWindow(hWnd)) Attach(hWnd);
    return hWnd;
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int command) {
    switch (command) {
        case SW_SHOWNORMAL:
        case SW_SHOWMAXIMIZED:
        case SW_SHOWNOACTIVATE:
        case SW_SHOW:
        case SW_SHOWNA:
        case SW_RESTORE:
        case SW_SHOWDEFAULT:
            if (DispatchTrayShowRequest(hWnd)) return IsWindowVisible(hWnd);
            break;
    }
    return ShowWindow_Original(hWnd, command);
}

using SetForegroundWindow_t = decltype(&SetForegroundWindow);
SetForegroundWindow_t SetForegroundWindow_Original;
BOOL WINAPI SetForegroundWindow_Hook(HWND hWnd) {
    if (DispatchTrayShowRequest(hWnd)) return FALSE;
    return SetForegroundWindow_Original(hWnd);
}

BOOL CALLBACK AttachExistingLocalSendWindow(HWND hWnd, LPARAM) {
    if (!g_window && IsLocalSendMainWindow(hWnd)) Attach(hWnd);
    return g_window == nullptr;
}

BOOL Wh_ModInit() {
    InitializeCriticalSection(&g_settingsLock);
    Settings initialSettings = ReadWindhawkSettings();
    StorePendingSettings(initialSettings);
    g_settings = initialSettings;

    g_applyMessage = RegisterWindowMessageW(L"Windhawk_LocalSendFlyout_Apply");
    g_settingsMessage = RegisterWindowMessageW(L"Windhawk_LocalSendFlyout_Settings");
    g_showRequestMessage = RegisterWindowMessageW(L"Windhawk_LocalSendFlyout_ShowRequest");
    if (!g_applyMessage || !g_settingsMessage || !g_showRequestMessage) return FALSE;

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original);
    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook, &ShowWindow_Original);
    WindhawkUtils::SetFunctionHook(SetForegroundWindow, SetForegroundWindow_Hook,
                                   &SetForegroundWindow_Original);
    return TRUE;
}

void Wh_ModAfterInit() {
    // The app can already be running when the mod is enabled.
    EnumWindows(AttachExistingLocalSendWindow, 0);
}

void Wh_ModSettingsChanged() {
    StorePendingSettings(ReadWindhawkSettings());
    if (HWND hWnd = g_window; hWnd && IsWindow(hWnd)) {
        SendMessageW(hWnd, g_settingsMessage, 0, 0);
    }
}

void Wh_ModUninit() {
    g_unloading = true;
    if (HWND hWnd = g_window.exchange(nullptr); hWnd && IsWindow(hWnd)) {
        // This mod has no modal loops or worker callbacks. Restoring through
        // the UI thread before removing the subclass keeps its WndProc out of
        // the unload path and puts the normal LocalSend window back in place.
        SendMessageW(hWnd, g_applyMessage, FALSE, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, LocalSendSubclassProc);
    }
    DeleteCriticalSection(&g_settingsLock);
}


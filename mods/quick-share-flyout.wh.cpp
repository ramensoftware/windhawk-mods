// ==WindhawkMod==
// @id              quick-share-flyout
// @name            Quick Share Flyout
// @description     Open Google Quick Share as a flyout above the system tray
// @version         1.1.0
// @author          0Allu
// @github          https://github.com/0Allu
// @homepage        https://github.com/0Allu/quick-share-flyout
// @donateUrl       https://ko-fi.com/0allu
// @include         nearby_share.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -ldwmapi -lshcore -lshlwapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Quick Share Flyout

Opens Google's Quick Share app as a flyout above the system tray, like the
Windows 11 quick settings, instead of a window in the middle of the screen.

**Requires [Quick Share for Windows](https://www.android.com/better-together/quick-share-app/).**
The mod changes the app's own window, so it does nothing unless the app is
installed.

![Quick Share Flyout demo](https://raw.githubusercontent.com/0Allu/quick-share-flyout/main/assets/quick-share-flyout-demo.gif)

Click the Quick Share tray icon to open or close the flyout. If the icon is in
the tray overflow menu, the flyout opens beside the menu.

## Features

* Opens above the tray icon, sliding out from the taskbar and fading in
* No title bar, with rounded corners, or a title bar with only a close button
* Stays above other windows, and closes when you click somewhere else
* Drag files onto it from File Explorer
* Moves notifications out of the way while it's open
* Hidden from the taskbar and Alt+Tab
* Adjustable size

## Sending files

Drag files from File Explorer onto the flyout. It stays open while you drag
and after you drop, until you click somewhere else. Clicking another window
closes it, so pick the files first, then open the flyout and drag them in.

You can also right-click files and choose **Send with Quick Share**, or use
**Select** in the flyout.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- width: 400
  $name: Width
  $description: In pixels at 100% display scaling.
- height: 500
  $name: Height
  $description: In pixels at 100% display scaling. Limited by screen space.
- closeWhenFocusIsLost: true
  $name: Close when focus is lost
  $description: Close the flyout when you click somewhere else.
- closeButton: false
  $name: Close button
  $description: Show a title bar with a close button.
- hideFromTaskbar: true
  $name: Hide from the taskbar and Alt+Tab
- moveNotifications: true
  $name: Move notifications out of the way
  $description: Applies to notifications that would cover the open flyout.
- animation: slideAndFade
  $name: Animation
  $description: Used when opening and closing. Turning off animation effects in Windows turns this off too.
  $options:
  - slideAndFade: Slide and fade
  - slide: Slide
  - off: "Off"
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <shellscalingapi.h>
#include <shlwapi.h>
#include <tlhelp32.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <string>
#include <vector>

struct {
    int width;
    int height;
    bool closeWhenFocusIsLost;
    bool closeButton;
    bool hideFromTaskbar;
    bool moveNotifications;
    bool slide;
    bool fade;
} g_settings;

std::atomic<HWND> g_window;
SIZE g_appSize;  // Of Quick Share's window before it became a flyout.
UINT g_applyMessage;
UINT g_tickMessage;
UINT g_showRequestMessage;

// Set while the flyout moves, so the frame thread sends a tick every frame.
HANDLE g_frameEvent;
HANDLE g_frameThread;
std::atomic<bool> g_stopFrameThread;
std::atomic<bool> g_tickPending;

// The rest is only used on the window's thread.
bool g_flyout;
bool g_addedToolWindow;
LONG_PTR g_removedBoxes;  // Title bar buttons taken away.
HWND g_taskbar;           // On the flyout's screen.
RECT g_restRect;          // Where the open flyout sits.
RECT g_frameInsets;       // Window edges that aren't seen, see PlaceFlyout.
POINT g_slideDirection;   // Towards the taskbar.
int g_travel;             // How far the flyout slides to get behind the taskbar.
float g_offset;           // How far the flyout is from its resting place.
float g_motionFrom;
float g_opacity;
float g_opacityFrom;
ULONGLONG g_motionStart;  // 0 while the flyout isn't moving.
bool g_closing;
bool g_revealPending;
DWORD g_openTime;
DWORD g_taskbarFocusTime;
DWORD g_ignoredShowTime;
bool g_buttonDown;

constexpr DWORD kClickWindow = 500;
constexpr UINT_PTR kRevealTimer = 0x5153464C;
constexpr UINT_PTR kClickTimer = 0x5153464D;
// Time for Quick Share to draw itself after being shown.
constexpr UINT kRevealDelay = 40;

ULONGLONG AnimationClock() {
    static const LONGLONG frequency = [] {
        LARGE_INTEGER value;
        QueryPerformanceFrequency(&value);
        return value.QuadPart;
    }();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return static_cast<ULONGLONG>(counter.QuadPart / frequency * 1000000 +
                                  counter.QuadPart % frequency * 1000000 / frequency);
}

bool IsOwnWindow(HWND hWnd) {
    DWORD processId = 0;
    return hWnd && GetWindowThreadProcessId(hWnd, &processId) && processId == GetCurrentProcessId();
}

bool IsQuickShareWindow(HWND hWnd) {
    WCHAR className[64];
    return IsOwnWindow(hWnd) && GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
           wcscmp(className, L"Google.Windows.NearbyShare") == 0;
}

bool RootHasClass(HWND hWnd, std::initializer_list<PCWSTR> classNames) {
    HWND root = hWnd ? GetAncestor(hWnd, GA_ROOT) : nullptr;
    WCHAR className[64];
    if (!root || !GetClassNameW(root, className, ARRAYSIZE(className))) return false;
    for (PCWSTR name : classNames) {
        if (wcscmp(className, name) == 0) return true;
    }
    return false;
}

bool IsOverflowMenu(HWND hWnd) {
    return RootHasClass(hWnd, {L"TopLevelWindowForOverflowXamlIsland", L"NotifyIconOverflowWindow"});
}

// The taskbar, or the tray overflow menu.
bool IsTaskbarWindow(HWND hWnd) {
    return RootHasClass(hWnd, {L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd"}) || IsOverflowMenu(hWnd);
}

HWND FindTaskbar(HMONITOR monitor) {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (MonitorFromWindow(taskbar, MONITOR_DEFAULTTONEAREST) == monitor) return taskbar;
    HWND secondary = nullptr;
    while ((secondary = FindWindowExW(nullptr, secondary, L"Shell_SecondaryTrayWnd", nullptr))) {
        if (MonitorFromWindow(secondary, MONITOR_DEFAULTTONEAREST) == monitor) return secondary;
    }
    return taskbar;
}

bool CursorOverTaskbar() {
    POINT cursor;
    return GetCursorPos(&cursor) && IsTaskbarWindow(WindowFromPoint(cursor));
}

// Windows 11's entrance curve, cubic-bezier(0, 0, 0, 1). Its exit curve is
// the same motion run backwards.
float EntranceEase(float progress) {
    float t = std::cbrt(progress);
    return t * t * (3.0f - 2.0f * t);
}

// The tray overflow menu stays above the flyout until the pointer is well
// away from it, so a flyout opened from it goes above it. If the screen is
// too short for that, it goes beside it instead: along the taskbar, to the
// nearer side that has room.
void AvoidOverflowMenu(HWND menu, const RECT& work, int gap) {
    // The menu's window already has a margin around the menu.
    RECT avoid, overlap;
    if (!GetWindowRect(menu, &avoid) || !IntersectRect(&overlap, &g_restRect, &avoid)) return;

    int lift;
    if (g_slideDirection.y > 0) {
        lift = avoid.top - g_restRect.bottom;
    } else if (g_slideDirection.y < 0) {
        lift = avoid.bottom - g_restRect.top;
    } else if (g_slideDirection.x > 0) {
        lift = avoid.left - g_restRect.right;
    } else {
        lift = avoid.right - g_restRect.left;
    }
    RECT lifted = g_restRect;
    OffsetRect(&lifted, g_slideDirection.x ? lift : 0, g_slideDirection.y ? lift : 0);
    if (lifted.left >= work.left + gap && lifted.top >= work.top + gap &&
        lifted.right <= work.right - gap && lifted.bottom <= work.bottom - gap) {
        g_restRect = lifted;
        return;
    }

    bool horizontal = g_slideDirection.y != 0;
    LONG start = horizontal ? g_restRect.left : g_restRect.top;
    LONG size = horizontal ? g_restRect.right - g_restRect.left : g_restRect.bottom - g_restRect.top;
    LONG before = (horizontal ? avoid.left : avoid.top) - size;
    LONG after = horizontal ? avoid.right : avoid.bottom;
    bool beforeFits = before >= (horizontal ? work.left : work.top) + gap;
    bool afterFits = after + size <= (horizontal ? work.right : work.bottom) - gap;
    if (!beforeFits && !afterFits) return;
    LONG to = beforeFits && (!afterFits || start - before <= after - start) ? before : after;
    OffsetRect(&g_restRect, horizontal ? to - start : 0, horizontal ? 0 : to - start);
}

void PlaceFlyout(HWND hWnd) {
    POINT cursor{};
    GetCursorPos(&cursor);
    HWND pointed = WindowFromPoint(cursor);
    bool fromTaskbar = IsTaskbarWindow(pointed);
    HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
    g_taskbar = FindTaskbar(monitor);
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    GetMonitorInfoW(monitor, &info);
    UINT dpiX = USER_DEFAULT_SCREEN_DPI, dpiY = USER_DEFAULT_SCREEN_DPI;
    GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    auto scale = [dpiX](int value) { return MulDiv(value, dpiX, USER_DEFAULT_SCREEN_DPI); };

    const RECT& work = info.rcWork;
    const RECT& screen = info.rcMonitor;
    // Windows 11 flyouts keep the same gap from the taskbar as from the
    // screen's edge.
    int gap = scale(12);
    int width = std::min<int>(scale(g_settings.width), work.right - work.left - 2 * gap);
    int height = std::min<int>(scale(g_settings.height), work.bottom - work.top - 2 * gap);

    // The taskbar is where the work area stops short of the screen. An
    // auto-hiding taskbar is assumed to be at the bottom.
    if (work.top > screen.top) {
        g_slideDirection = {0, -1};
    } else if (work.left > screen.left) {
        g_slideDirection = {-1, 0};
    } else if (work.right < screen.right) {
        g_slideDirection = {1, 0};
    } else {
        g_slideDirection = {0, 1};
    }

    // Centered on the tray icon that was clicked, or else at the tray's end
    // of the taskbar.
    int x, y;
    if (g_slideDirection.y) {
        x = fromTaskbar ? cursor.x - width / 2 : work.right - width - gap;
        y = g_slideDirection.y > 0 ? work.bottom - height - gap : work.top + gap;
    } else {
        x = g_slideDirection.x > 0 ? work.right - width - gap : work.left + gap;
        y = fromTaskbar ? cursor.y - height / 2 : work.bottom - height - gap;
    }
    x = std::clamp<int>(x, work.left + gap, std::max<int>(work.left + gap, work.right - width - gap));
    y = std::clamp<int>(y, work.top + gap, std::max<int>(work.top + gap, work.bottom - height - gap));
    g_restRect = {x, y, x + width, y + height};
    if (IsOverflowMenu(pointed)) AvoidOverflowMenu(GetAncestor(pointed, GA_ROOT), work, gap);

    // Far enough to start out of sight behind the taskbar.
    if (g_slideDirection.y > 0) {
        g_travel = work.bottom - g_restRect.top;
    } else if (g_slideDirection.y < 0) {
        g_travel = g_restRect.bottom - work.top;
    } else if (g_slideDirection.x > 0) {
        g_travel = work.right - g_restRect.left;
    } else {
        g_travel = g_restRect.right - work.left;
    }

    // A title bar comes with an invisible resizing border on the other sides,
    // so the window is that much larger than what's seen.
    g_frameInsets = {};
    if (g_settings.closeButton) {
        RECT frame{};
        AdjustWindowRectExForDpi(&frame, GetWindowLongPtrW(hWnd, GWL_STYLE), FALSE,
                                 GetWindowLongPtrW(hWnd, GWL_EXSTYLE), dpiX);
        UINT border = 1;
        DwmGetWindowAttribute(hWnd, DWMWA_VISIBLE_FRAME_BORDER_THICKNESS, &border, sizeof(border));
        int side = std::max<int>(0, frame.right - border);
        g_frameInsets = {side, 0, side, std::max<int>(0, frame.bottom - border)};
    }
}

// Of the window, which may be larger than the flyout, see PlaceFlyout.
POINT FlyoutPosition() {
    int shift = static_cast<int>(std::lround(g_offset));
    return {g_restRect.left - g_frameInsets.left + g_slideDirection.x * shift,
            g_restRect.top - g_frameInsets.top + g_slideDirection.y * shift};
}

void MoveFlyout(HWND hWnd) {
    POINT position = FlyoutPosition();
    SetWindowPos(hWnd, nullptr, position.x, position.y, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
}

// Waits for each frame of the compositor here rather than on the window's
// thread, which Quick Share needs to draw while the flyout moves.
DWORD WINAPI FrameThread(LPVOID) {
    while (WaitForSingleObject(g_frameEvent, INFINITE) == WAIT_OBJECT_0 && !g_stopFrameThread) {
        if (FAILED(DwmFlush())) Sleep(10);
        HWND hWnd = g_window;
        if (hWnd && !g_tickPending.exchange(true) && !PostMessageW(hWnd, g_tickMessage, 0, 0)) {
            g_tickPending = false;
        }
    }
    return 0;
}

bool MotionEnabled() {
    BOOL enabled = TRUE;
    SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &enabled, 0);
    if (!g_settings.slide || !enabled) return false;
    // Started on first use. Starting it as the mod loads made a second Quick
    // Share instance crash, so it couldn't hand off to the first one.
    if (!g_frameThread) g_frameThread = CreateThread(nullptr, 0, FrameThread, nullptr, 0, nullptr);
    return g_frameThread != nullptr;
}

void StartMotion() {
    g_motionFrom = g_offset;
    g_opacityFrom = g_opacity;
    g_motionStart = AnimationClock();
    SetEvent(g_frameEvent);
}

void StopMotion() {
    g_motionStart = 0;
    ResetEvent(g_frameEvent);
}

void Cloak(HWND hWnd, bool cloak) {
    BOOL value = cloak;
    DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &value, sizeof(value));
}

// Layered only while fading, as Windows can let clicks through layered windows.
void SetOpacity(HWND hWnd, float opacity) {
    g_opacity = opacity;
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    bool layered = opacity < 1.0f;
    if (layered != ((exStyle & WS_EX_LAYERED) != 0)) {
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle ^ WS_EX_LAYERED);
    }
    if (layered) {
        SetLayeredWindowAttributes(hWnd, 0, static_cast<BYTE>(std::lround(opacity * 255.0f)), LWA_ALPHA);
    }
}

// Ends opening or closing where it would end.
void SettleFlyout(HWND hWnd) {
    bool closing = g_closing;
    StopMotion();
    if (g_revealPending) KillTimer(hWnd, kRevealTimer);
    g_revealPending = false;
    g_closing = false;
    g_offset = 0.0f;
    if (closing) ShowWindow(hWnd, SW_HIDE);
    SetOpacity(hWnd, 1.0f);
    Cloak(hWnd, false);
}

void AdvanceMotion(HWND hWnd) {
    if (!g_motionStart) return;
    if (!IsWindowVisible(hWnd)) {
        SettleFlyout(hWnd);
        return;
    }
    float elapsed = static_cast<float>(AnimationClock() - g_motionStart);
    float duration = g_closing ? 167000.0f : 250000.0f;
    float progress = std::min(1.0f, elapsed / duration);
    float target = g_closing ? static_cast<float>(g_travel) : 0.0f;
    float eased = g_closing ? 1.0f - EntranceEase(1.0f - progress) : EntranceEase(progress);
    g_offset = g_motionFrom + (target - g_motionFrom) * eased;
    // Fades in before the slide, which crawls at the end, has stopped.
    float faded = g_closing ? progress : std::min(1.0f, elapsed / 150000.0f);
    float opacity = g_closing && g_settings.fade ? 0.0f : 1.0f;
    SetOpacity(hWnd, g_opacityFrom + (opacity - g_opacityFrom) * faded);
    MoveFlyout(hWnd);
    if (progress < 1.0f) return;
    if (g_closing) {
        SettleFlyout(hWnd);
    } else {
        StopMotion();
    }
}

// Windows shows notifications in a layer above every window but its own, so
// they would cover the flyout. While the flyout is open, a notification over
// it is moved off screen, and moved back once the flyout has closed. As in
// Wireless Quick Settings.
//
// Windows reuses a notification's window: it places it for each new
// notification, and shrinks it away when one ends. One that Windows has
// placed again in the meantime is left where Windows put it.
constexpr int kNotificationAway = -32000;

struct MovedNotification {
    HWND window;
    POINT home;
};
[[clang::no_destroy]] std::vector<MovedNotification> g_movedNotifications;
HWINEVENTHOOK g_notificationHook;
DWORD g_notificationProcessId;  // ShellExperienceHost of this session.

// The title Windows gives a notification's window, in the display language.
// Other windows of the process, such as Quick Settings and the clock, open
// in the same corner and must stay where they are.
const std::wstring& NotificationTitle() {
    [[clang::no_destroy]] static const std::wstring title = [] {
        WCHAR windows[MAX_PATH] = {};
        GetWindowsDirectoryW(windows, ARRAYSIZE(windows));
        std::wstring source = L"@{" + std::wstring(windows) +
            L"\\SystemResources\\Windows.UI.ShellCommon\\Windows.UI.ShellCommon.pri"
            L"? ms-resource:///ActionCenter/AC_ToastCenter_Title}";
        WCHAR text[256] = {};
        if (SUCCEEDED(SHLoadIndirectString(source.c_str(), text, ARRAYSIZE(text), nullptr)) &&
            text[0]) {
            return std::wstring(text);
        }
        return std::wstring(L"New notification");
    }();
    return title;
}

bool IsNotificationWindow(HWND window) {
    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    if (!processId || processId != g_notificationProcessId) return false;
    WCHAR className[32] = {};
    if (!GetClassNameW(window, className, ARRAYSIZE(className)) ||
        wcscmp(className, L"Windows.UI.Core.CoreWindow") != 0) {
        return false;
    }
    // Reads the title without sending a message, which could wait on a busy
    // process.
    WCHAR title[128] = {};
    InternalGetWindowText(window, title, ARRAYSIZE(title));
    return NotificationTitle() == title;
}

DWORD FindNotificationProcess() {
    DWORD session = 0;
    if (!ProcessIdToSessionId(GetCurrentProcessId(), &session)) return 0;
    DWORD found = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return 0;
    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(entry);
    for (BOOL more = Process32FirstW(snapshot, &entry); more && !found;
         more = Process32NextW(snapshot, &entry)) {
        DWORD processSession = 0;
        if (_wcsicmp(entry.szExeFile, L"ShellExperienceHost.exe") == 0 &&
            ProcessIdToSessionId(entry.th32ProcessID, &processSession) &&
            processSession == session) {
            found = entry.th32ProcessID;
        }
    }
    CloseHandle(snapshot);
    return found;
}

// Moves another process's window without waiting for it, so a busy shell
// can't hold up the flyout.
constexpr UINT kNotificationMoveFlags = SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                                        SWP_NOOWNERZORDER | SWP_ASYNCWINDOWPOS;

void KeepNotificationAway(HWND window) {
    RECT rect;
    if (!IsWindowVisible(window) || !GetWindowRect(window, &rect)) return;
    auto moved = std::find_if(g_movedNotifications.begin(), g_movedNotifications.end(),
                              [&](const MovedNotification& item) { return item.window == window; });
    if (moved != g_movedNotifications.end()) {
        if (rect.left == kNotificationAway && rect.top == kNotificationAway) return;
        g_movedNotifications.erase(moved);  // Windows has placed it again.
    }
    RECT overlap;
    if (!IntersectRect(&overlap, &rect, &g_restRect) || !IsNotificationWindow(window)) return;
    if (SetWindowPos(window, nullptr, kNotificationAway, kNotificationAway, 0, 0,
                     kNotificationMoveFlags)) {
        g_movedNotifications.push_back({window, {rect.left, rect.top}});
    }
}

void CALLBACK NotificationEvent(HWINEVENTHOOK, DWORD event, HWND window, LONG object, LONG child,
                                DWORD, DWORD) {
    if ((event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_LOCATIONCHANGE) && window &&
        object == OBJID_WINDOW && child == CHILDID_SELF && GetAncestor(window, GA_ROOT) == window) {
        KeepNotificationAway(window);
    }
}

void KeepNotificationsAway(HWND hWnd) {
    if (!g_settings.moveNotifications) return;
    // Only the notifications' own process is watched: watching every process
    // would also report each move of the pointer.
    if (!g_notificationHook) {
        g_notificationProcessId = FindNotificationProcess();
        if (!g_notificationProcessId) return;
        g_notificationHook = SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
                                             NotificationEvent, g_notificationProcessId, 0,
                                             WINEVENT_OUTOFCONTEXT);
    }
    // Notifications showing already. They are not listed with other windows,
    // but can be found under the flyout.
    constexpr int step = 40;
    for (int y = g_restRect.top + step / 2; y < g_restRect.bottom; y += step) {
        for (int x = g_restRect.left + step / 2; x < g_restRect.right; x += step) {
            HWND window = GetAncestor(WindowFromPoint({x, y}), GA_ROOT);
            if (window && window != hWnd) KeepNotificationAway(window);
        }
    }
}

void ReturnNotifications() {
    if (g_notificationHook) {
        UnhookWinEvent(g_notificationHook);
        g_notificationHook = nullptr;
    }
    for (const MovedNotification& moved : g_movedNotifications) {
        RECT rect;
        if (IsWindow(moved.window) && GetWindowRect(moved.window, &rect) &&
            rect.left == kNotificationAway && rect.top == kNotificationAway) {
            SetWindowPos(moved.window, nullptr, moved.home.x, moved.home.y, 0, 0,
                         kNotificationMoveFlags);
        }
    }
    g_movedNotifications.clear();
}

// Called while the window is being shown. Parts of the window that are off
// the screen when Quick Share draws stay empty, so it stays hidden where the
// whole of it is on the screen until Quick Share has drawn it.
void OpenFlyout(HWND hWnd) {
    PlaceFlyout(hWnd);
    KeepNotificationsAway(hWnd);
    StopMotion();
    g_closing = false;
    g_offset = 0.0f;
    SetOpacity(hWnd, 1.0f);
    g_openTime = GetTickCount();
    Cloak(hWnd, true);
    g_revealPending = SetTimer(hWnd, kRevealTimer, kRevealDelay, nullptr) != 0;
    if (!g_revealPending) Cloak(hWnd, false);
}

void RevealFlyout(HWND hWnd) {
    KillTimer(hWnd, kRevealTimer);
    g_revealPending = false;
    if (IsWindowVisible(hWnd) && MotionEnabled()) {
        g_offset = static_cast<float>(g_travel);
        if (g_settings.fade) SetOpacity(hWnd, 0.0f);
        MoveFlyout(hWnd);
        StartMotion();
    }
    Cloak(hWnd, false);
}

void CloseFlyout(HWND hWnd) {
    if (!IsWindowVisible(hWnd) || g_closing) return;
    g_closing = true;
    if (MotionEnabled() && !g_revealPending) {
        StartMotion();
    } else {
        SettleFlyout(hWnd);
    }
}

void ReopenFlyout() {
    g_closing = false;
    g_openTime = GetTickCount();
    StartMotion();
}

// Whether a mouse button is down. Also adds to `pressed` whether one was
// pressed since the last call, to catch clicks between two calls.
bool MouseButtonDown(bool* pressed = nullptr) {
    bool down = false;
    for (int button : {VK_LBUTTON, VK_RBUTTON}) {
        SHORT state = GetAsyncKeyState(button);
        down |= state < 0;
        if (pressed) *pressed |= state & 1;
    }
    return down;
}

bool CursorOverFlyout(HWND hWnd) {
    POINT cursor;
    return GetCursorPos(&cursor) && GetAncestor(WindowFromPoint(cursor), GA_ROOT) == hWnd;
}

void CloseOnClickAway(HWND hWnd, HWND clicked) {
    // Clicking the tray icon moves focus to the taskbar first. Remember that,
    // so that the click doesn't open the flyout again.
    if (IsTaskbarWindow(clicked) || CursorOverTaskbar()) g_taskbarFocusTime = GetTickCount();
    CloseFlyout(hWnd);
}

void OnFocusLost(HWND hWnd, HWND next) {
    if (!next) next = GetForegroundWindow();
    // Quick Share's own dialogs, like the file picker, keep the flyout open.
    if (IsOwnWindow(next)) return;
    if (!g_settings.closeWhenFocusIsLost || !IsWindowVisible(hWnd) || g_closing) return;
    // Pressing on files in another window, to drag them onto the flyout,
    // takes the focus away. See where they are let go of, in CheckClicks.
    if (!IsTaskbarWindow(next) && MouseButtonDown() && SetTimer(hWnd, kClickTimer, 16, nullptr)) {
        g_buttonDown = true;
        return;
    }
    CloseOnClickAway(hWnd, next);
}

// Runs while the flyout is open but another window has the focus, like after
// files were dropped on it. Letting go of a click outside the flyout closes
// it.
void CheckClicks(HWND hWnd) {
    if (!IsWindowVisible(hWnd) || g_closing || IsOwnWindow(GetForegroundWindow())) {
        KillTimer(hWnd, kClickTimer);
        return;
    }
    bool pressed = false;
    bool down = MouseButtonDown(&pressed);
    bool released = !down && (g_buttonDown || pressed);
    g_buttonDown = down;
    if (!released || CursorOverFlyout(hWnd)) return;
    KillTimer(hWnd, kClickTimer);
    CloseOnClickAway(hWnd, GetForegroundWindow());
}

// Clicking the tray icon makes Quick Share show and focus its window. While
// the flyout is open, or that same click just closed it, close it instead.
// Returns true if the request was used up.
bool HandleShowRequest(HWND hWnd) {
    DWORD now = GetTickCount();
    bool open = IsWindowVisible(hWnd) && !g_closing;
    // Quick Share asks more than once for one click.
    if (now - g_ignoredShowTime < kClickWindow) return true;
    if (open && now - g_openTime < kClickWindow) return false;
    if (CursorOverTaskbar() && (open || now - g_taskbarFocusTime < kClickWindow)) {
        CloseFlyout(hWnd);
        g_ignoredShowTime = now;
        return true;
    }
    if (g_closing) ReopenFlyout();
    return false;
}

// Quick Share's tray icon runs on a thread of its own, so this may be called
// from any thread.
bool ConsumeShowRequest(HWND hWnd) {
    if (!hWnd || hWnd != g_window) return false;
    if (GetWindowThreadProcessId(hWnd, nullptr) == GetCurrentThreadId()) {
        return g_flyout && HandleShowRequest(hWnd);
    }
    return SendMessageW(hWnd, g_showRequestMessage, 0, 0) != 0;
}

void ApplyFlyout(HWND hWnd, bool enable) {
    SettleFlyout(hWnd);
    KillTimer(hWnd, kClickTimer);
    g_flyout = enable;

    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    bool toolWindow = enable && g_settings.hideFromTaskbar;
    if (toolWindow && !(exStyle & WS_EX_TOOLWINDOW)) {
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_TOOLWINDOW);
        g_addedToolWindow = true;
    } else if (!toolWindow && g_addedToolWindow) {
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle & ~WS_EX_TOOLWINDOW);
        g_addedToolWindow = false;
    }

    // Leaves only the close button on the title bar.
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    constexpr LONG_PTR boxes = WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    if (enable && g_settings.closeButton) {
        g_removedBoxes |= style & boxes;
        SetWindowLongPtrW(hWnd, GWL_STYLE, style & ~boxes);
    } else if (g_removedBoxes) {
        SetWindowLongPtrW(hWnd, GWL_STYLE, style | g_removedBoxes);
        g_removedBoxes = 0;
    }

    // The flyout slides in by itself.
    BOOL noTransitions = enable;
    DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &noTransitions, sizeof(noTransitions));
    DWM_WINDOW_CORNER_PREFERENCE corner = enable ? DWMWCP_ROUND : DWMWCP_DEFAULT;
    DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));

    if (enable && IsWindowVisible(hWnd)) {
        PlaceFlyout(hWnd);
        KeepNotificationsAway(hWnd);
    }
    if (!enable || !g_settings.moveNotifications) ReturnNotifications();
    if (enable) {
        // Moves an open flyout into place too, see WM_WINDOWPOSCHANGING.
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
        return;
    }

    // The closed flyout is left behind the taskbar, so Quick Share's window
    // goes back to its own size in the middle of the screen.
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    GetMonitorInfoW(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &info);
    const RECT& work = info.rcWork;
    int width = std::min<int>(g_appSize.cx, work.right - work.left);
    int height = std::min<int>(g_appSize.cy, work.bottom - work.top);
    SetWindowPos(hWnd, HWND_NOTOPMOST, work.left + (work.right - work.left - width) / 2,
                 work.top + (work.bottom - work.top - height) / 2, width, height,
                 SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
}

LRESULT CALLBACK FlyoutSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR) {
    if (uMsg == g_applyMessage) {
        ApplyFlyout(hWnd, wParam != 0);
        return 0;
    }
    if (uMsg == g_tickMessage) {
        g_tickPending = false;
        if (g_flyout) AdvanceMotion(hWnd);
        return 0;
    }
    if (uMsg == g_showRequestMessage) return g_flyout && HandleShowRequest(hWnd);
    if (uMsg == WM_TIMER && wParam == kRevealTimer) {
        RevealFlyout(hWnd);
        return 0;
    }
    if (uMsg == WM_TIMER && wParam == kClickTimer) {
        CheckClicks(hWnd);
        return 0;
    }
    if (uMsg == WM_NCDESTROY) {
        StopMotion();
        ReturnNotifications();
        g_window = nullptr;
        g_flyout = false;
    }
    if (!g_flyout) return DefSubclassProc(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_NCCALCSIZE:
            // The client area covers the whole window, which hides the title
            // bar. Windows still rounds the corners and draws the shadow.
            if (wParam && !g_settings.closeButton) return 0;
            break;

        case WM_NCHITTEST: {
            // The flyout can't be resized.
            LRESULT hit = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            return hit >= HTLEFT && hit <= HTBOTTOMRIGHT ? HTBORDER : hit;
        }

        case WM_GETMINMAXINFO: {
            // Quick Share's own minimum size is larger than a flyout.
            LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize = {0, 0};
            return result;
        }

        case WM_WINDOWPOSCHANGING: {
            auto* position = reinterpret_cast<WINDOWPOS*>(lParam);
            if (position->flags & SWP_HIDEWINDOW) {
                ReturnNotifications();
                break;
            }
            bool showing = !IsWindowVisible(hWnd);
            if (showing) {
                if (!(position->flags & SWP_SHOWWINDOW)) break;
                OpenFlyout(hWnd);
            }
            // Right below the taskbar: other windows can't cover the flyout
            // while files are dragged onto it, and it slides out from behind
            // the taskbar.
            if (showing || !(position->flags & SWP_NOZORDER)) {
                position->hwndInsertAfter = IsWindow(g_taskbar) ? g_taskbar : HWND_TOPMOST;
                position->flags &= ~SWP_NOZORDER;
            }
            POINT topLeft = FlyoutPosition();
            position->x = topLeft.x;
            position->y = topLeft.y;
            position->cx = g_restRect.right - g_restRect.left + g_frameInsets.left + g_frameInsets.right;
            position->cy = g_restRect.bottom - g_restRect.top + g_frameInsets.top + g_frameInsets.bottom;
            position->flags &= ~(SWP_NOMOVE | SWP_NOSIZE);
            break;
        }

        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE) OnFocusLost(hWnd, reinterpret_cast<HWND>(lParam));
            break;

        case WM_SYSCOMMAND:
            switch (wParam & 0xFFF0) {
                case SC_CLOSE:
                case SC_MINIMIZE:
                    CloseFlyout(hWnd);
                    return 0;
                case SC_MAXIMIZE:
                case SC_MOVE:
                case SC_SIZE:
                    return 0;
            }
            break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void Attach(HWND hWnd) {
    HWND expected = nullptr;
    if (!g_window.compare_exchange_strong(expected, hWnd)) return;
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, FlyoutSubclassProc, 0)) {
        Wh_Log(L"Failed to subclass the Quick Share window");
        g_window = nullptr;
        return;
    }
    RECT rect;
    GetWindowRect(hWnd, &rect);
    g_appSize = {rect.right - rect.left, rect.bottom - rect.top};
    SendMessageW(hWnd, g_applyMessage, TRUE, 0);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
                                 DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                 HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth,
                                         nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (hWnd && !hWndParent && !g_window && IsQuickShareWindow(hWnd)) Attach(hWnd);
    return hWnd;
}

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;
BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    switch (nCmdShow) {
        case SW_SHOWNORMAL:
        case SW_SHOWMAXIMIZED:
        case SW_SHOWNOACTIVATE:
        case SW_SHOW:
        case SW_SHOWNA:
        case SW_RESTORE:
        case SW_SHOWDEFAULT:
            if (ConsumeShowRequest(hWnd)) return IsWindowVisible(hWnd);
            break;
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

using SetForegroundWindow_t = decltype(&SetForegroundWindow);
SetForegroundWindow_t SetForegroundWindow_Original;
BOOL WINAPI SetForegroundWindow_Hook(HWND hWnd) {
    if (ConsumeShowRequest(hWnd)) return FALSE;
    return SetForegroundWindow_Original(hWnd);
}

void LoadSettings() {
    g_settings.width = std::max(Wh_GetIntSetting(L"width"), 200);
    g_settings.height = std::max(Wh_GetIntSetting(L"height"), 200);
    g_settings.closeWhenFocusIsLost = Wh_GetIntSetting(L"closeWhenFocusIsLost");
    g_settings.closeButton = Wh_GetIntSetting(L"closeButton");
    g_settings.hideFromTaskbar = Wh_GetIntSetting(L"hideFromTaskbar");
    g_settings.moveNotifications = Wh_GetIntSetting(L"moveNotifications");
    WindhawkUtils::StringSetting animation = WindhawkUtils::StringSetting::make(L"animation");
    g_settings.slide = wcscmp(animation.get(), L"off") != 0;
    g_settings.fade = g_settings.slide && wcscmp(animation.get(), L"slide") != 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_applyMessage = RegisterWindowMessageW(L"Windhawk_QuickShareFlyout_Apply");
    g_tickMessage = RegisterWindowMessageW(L"Windhawk_QuickShareFlyout_Tick");
    g_showRequestMessage = RegisterWindowMessageW(L"Windhawk_QuickShareFlyout_ShowRequest");
    g_frameEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_frameEvent) return FALSE;
    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original);
    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook, &ShowWindow_Original);
    WindhawkUtils::SetFunctionHook(SetForegroundWindow, SetForegroundWindow_Hook,
                                   &SetForegroundWindow_Original);
    return TRUE;
}

BOOL CALLBACK AttachToExistingWindow(HWND hWnd, LPARAM) {
    if (!IsQuickShareWindow(hWnd)) return TRUE;
    Attach(hWnd);
    return FALSE;
}

void Wh_ModAfterInit() {
    // Quick Share may have made its window before the mod loaded.
    EnumWindows(AttachToExistingWindow, 0);
}

void Wh_ModUninit() {
    if (HWND hWnd = g_window.exchange(nullptr)) {
        SendMessageW(hWnd, g_applyMessage, FALSE, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, FlyoutSubclassProc);
    }
    if (g_frameThread) {
        g_stopFrameThread = true;
        SetEvent(g_frameEvent);
        WaitForSingleObject(g_frameThread, INFINITE);
        CloseHandle(g_frameThread);
    }
    CloseHandle(g_frameEvent);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (HWND hWnd = g_window) SendMessageW(hWnd, g_applyMessage, TRUE, 0);
}

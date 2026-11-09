// ==WindhawkMod==
// @id              whatsapp-flyout
// @name            WhatsApp Flyout
// @description     Opens WhatsApp as a flyout above the system tray instead of a full window
// @version         1.0.0
// @author          DpakB
// @homepage        https://dpakb.com
// @github          https://github.com/DeepakBalaraman
// @twitter         https://twitter.com/dpakb_
// @donateUrl       https://buymeacoffee.com/dpakb
// @include         WhatsApp.Root.exe
// @include         WhatsApp.exe
// @include         WhatsAppBeta.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -ldwmapi -lshcore -lshlwapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# WhatsApp Flyout

![Sample Screenshot of flyout in action](https://raw.githubusercontent.com/DeepakBalaraman/whatsapp-flyout/master/assets/screenshot.png)

Opens WhatsApp as a flyout above the system tray, like the Windows 11 quick
settings, instead of a window in the middle of the screen.

**Requires WhatsApp for Windows** from the Microsoft Store or from
whatsapp.com. The mod changes the app's own window, so it does nothing unless
the app is installed.

## Opening and closing

- **Tray icon.** Click the WhatsApp icon to open or close the flyout. If the
  icon is in the overflow menu, the flyout opens beside the menu.
- **Shortcut.** Press **Ctrl+Alt+Shift+W**, or whatever is set below.
- **Clicking away.** With Close when focus is lost on, clicking any other
  window closes the flyout.

## Features

- Slides out of the screen edge it sits against and fades in
- Opens above the tray icon by default; a corner or the middle of the screen
  can be picked instead, and nudged with an offset
- Rounded corners, and a minimal title bar that leaves only a close button
- Files can be dragged onto it from File Explorer
- Notifications that would cover it are moved aside while it is open
- Hidden from the taskbar and Alt+Tab
- Resizes by dragging its edges

## Good to know

**The shortcut only works while WhatsApp is running.** Picking Quit from the
tray icon stops the app, and the shortcut with it, until you start the app
again. Closing the flyout is not quitting, so the shortcut keeps working.

**The shortcut needs Ctrl, Alt, Shift or Win in it**, so that it never takes a
bare key away from another program. If another program already owns it, the
shortcut is left alone, the tray icon still works, and the mod log says so.

**A size you drag the flyout to lasts until WhatsApp restarts.** Copy it into
the Width and Height settings to keep it for good.

**The minimal title bar leaves only a close button**, so there is nothing to
minimize with. Click somewhere else, press the shortcut, or use the tray icon.

**Windows can show the full size window for a moment** while WhatsApp starts.
If WhatsApp starts with Windows, the flyout opens then too; turn it off in
Settings, Apps, Startup to only see it when you ask for it.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- width: 420
  $name: Width
  $description: In pixels at 100% display scaling.
- height: 620
  $name: Height
  $description: In pixels at 100% display scaling. Limited by screen space.
- position: tray
  $name: Position
  $description: Where the flyout opens.
  $options:
  - tray: Above the tray icon
  - bottomRight: Bottom right
  - bottomLeft: Bottom left
  - topRight: Top right
  - topLeft: Top left
  - center: Middle of the screen
- offsetX: 0
  $name: Horizontal offset
  $description: Pixels to move the flyout sideways, negative to the left, at 100% display scaling.
- offsetY: 0
  $name: Vertical offset
  $description: Pixels to move the flyout up or down, negative upwards, at 100% display scaling.
- hotkey: Ctrl+Alt+Shift+W
  $name: Shortcut
  $description: Opens and closes the flyout while WhatsApp is running, for example Ctrl+Alt+Shift+W or Ctrl+Alt+Space. Leave it empty to turn the shortcut off; the tray icon works either way.
- closeWhenFocusIsLost: true
  $name: Close when focus is lost
  $description: Close the flyout when you click somewhere else.
- minimalTitleBar: true
  $name: Minimal title bar
  $description: Leaves only a close button on the title bar. Off shows WhatsApp's usual minimize, maximize and close buttons.
- resizable: true
  $name: Allow resizing
  $description: Drag the edges of the flyout to resize it.
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
#include <cwctype>
#include <string.h>
#include <string>
#include <vector>

// Where the flyout opens, from the Position setting.
enum FlyoutPlacement {
    kPlacementTray,
    kPlacementBottomRight,
    kPlacementBottomLeft,
    kPlacementTopRight,
    kPlacementTopLeft,
    kPlacementCenter,
};

struct {
    int width;
    int height;
    FlyoutPlacement placement;
    int offsetX;
    int offsetY;
    bool closeWhenFocusIsLost;
    bool minimalTitleBar;
    bool resizable;
    bool hideFromTaskbar;
    bool moveNotifications;
    bool slide;
    bool fade;
    UINT hotkeyModifiers;
    UINT hotkeyKey;
} g_settings;

std::atomic<HWND> g_window;
SIZE g_appSize;  // Of WhatsApp's window before it became a flyout.
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
bool g_removedMaximize;   // Windows doesn't round a maximized window.
LONG_PTR g_removedBoxes;  // Title bar buttons taken away.
HWND g_taskbar;           // On the flyout's screen.
int g_dpi;                // Of the flyout's screen, 96 at 100% scaling.
bool g_resizing;          // While an edge is being dragged.
LRESULT g_hitEdge;        // The edge under the pointer, 0 if none.
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
bool g_hotkeyRegistered;

// The flyout shows and hides itself, so the app's own calls are needed
// without going through the hooks below.
using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_Original;
using SetForegroundWindow_t = decltype(&SetForegroundWindow);
SetForegroundWindow_t SetForegroundWindow_Original;

constexpr int kToggleHotkeyId = 1;
constexpr DWORD kClickWindow = 500;
constexpr UINT_PTR kRevealTimer = 0x5741464C;
constexpr UINT_PTR kClickTimer = 0x5741464D;
// Time for WhatsApp to draw itself after being shown.
constexpr UINT kRevealDelay = 40;
// What the flyout can be dragged down to, at 100% display scaling.
constexpr int kMinFlyoutWidth = 260;
constexpr int kMinFlyoutHeight = 320;

int FlyoutDpi() {
    return g_dpi ? g_dpi : USER_DEFAULT_SCREEN_DPI;
}

LONG MinFlyoutWidth() {
    return MulDiv(kMinFlyoutWidth, FlyoutDpi(), USER_DEFAULT_SCREEN_DPI);
}

LONG MinFlyoutHeight() {
    return MulDiv(kMinFlyoutHeight, FlyoutDpi(), USER_DEFAULT_SCREEN_DPI);
}

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

bool ContainsNoCase(PCWSTR text, PCWSTR part) {
    size_t length = wcslen(part);
    if (!length) return true;
    for (PCWSTR cursor = text; *cursor; cursor++) {
        if (_wcsnicmp(cursor, part, length) == 0) return true;
    }
    return false;
}

// WhatsApp names its window after the app, which leaves out the settings
// window and the windows WinUI makes for itself.
bool IsWhatsAppWindow(HWND hWnd) {
    if (!IsOwnWindow(hWnd) || GetWindow(hWnd, GW_OWNER)) return false;
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if ((style & WS_CHILD) || (exStyle & WS_EX_NOACTIVATE)) return false;
    // Reads the title without sending a message, which could wait on a busy
    // thread.
    WCHAR title[128] = {};
    InternalGetWindowText(hWnd, title, ARRAYSIZE(title));
    return ContainsNoCase(title, L"WhatsApp");
}

// WhatsApp for Windows is a WinUI 3 app, and the desktop app before it was
// Electron. WinUI 3 gives its window the app's name only after creating it,
// so the class and a resizable frame are what tell the main window from the
// ones WinUI makes for itself.
bool IsWhatsAppWindowCreation(HWND hWnd, LPCWSTR className, LPCWSTR windowName) {
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_TOOLWINDOW) || (exStyle & WS_EX_NOACTIVATE)) return false;
    if (windowName && ContainsNoCase(windowName, L"WhatsApp")) return true;
    if (!className || IS_INTRESOURCE(className)) return false;
    if (_wcsicmp(className, L"WinUIDesktopWin32WindowClass") != 0 &&
        _wcsicmp(className, L"Chrome_WidgetWin_1") != 0) {
        return false;
    }
    return (GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_THICKFRAME) != 0;
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
    g_dpi = dpiX;
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
    POINT taskbarEdge;
    if (work.top > screen.top) {
        taskbarEdge = {0, -1};
    } else if (work.left > screen.left) {
        taskbarEdge = {-1, 0};
    } else if (work.right < screen.right) {
        taskbarEdge = {1, 0};
    } else {
        taskbarEdge = {0, 1};
    }

    // Where the flyout opens. Above the tray icon, it is centered on the icon
    // that was clicked, or else at the tray's end of the taskbar.
    int x, y;
    switch (g_settings.placement) {
        case kPlacementBottomRight:
            x = work.right - width - gap;
            y = work.bottom - height - gap;
            break;
        case kPlacementBottomLeft:
            x = work.left + gap;
            y = work.bottom - height - gap;
            break;
        case kPlacementTopRight:
            x = work.right - width - gap;
            y = work.top + gap;
            break;
        case kPlacementTopLeft:
            x = work.left + gap;
            y = work.top + gap;
            break;
        case kPlacementCenter:
            x = work.left + (work.right - work.left - width) / 2;
            y = work.top + (work.bottom - work.top - height) / 2;
            break;
        default:
            if (taskbarEdge.y) {
                x = fromTaskbar ? cursor.x - width / 2 : work.right - width - gap;
                y = taskbarEdge.y > 0 ? work.bottom - height - gap : work.top + gap;
            } else {
                x = taskbarEdge.x > 0 ? work.right - width - gap : work.left + gap;
                y = fromTaskbar ? cursor.y - height / 2 : work.bottom - height - gap;
            }
            break;
    }
    x += scale(g_settings.offsetX);
    y += scale(g_settings.offsetY);
    x = std::clamp<int>(x, work.left + gap, std::max<int>(work.left + gap, work.right - width - gap));
    y = std::clamp<int>(y, work.top + gap, std::max<int>(work.top + gap, work.bottom - height - gap));
    g_restRect = {x, y, x + width, y + height};

    // The flyout slides out from the edge it rests against, which is the
    // taskbar's edge unless a corner or an offset moved it away.
    bool againstEdge = true;
    if (g_restRect.top - work.top <= gap) {
        g_slideDirection = {0, -1};
    } else if (work.bottom - g_restRect.bottom <= gap) {
        g_slideDirection = {0, 1};
    } else if (g_restRect.left - work.left <= gap) {
        g_slideDirection = {-1, 0};
    } else if (work.right - g_restRect.right <= gap) {
        g_slideDirection = {1, 0};
    } else {
        g_slideDirection = taskbarEdge;
        againstEdge = false;
    }

    if (g_settings.placement == kPlacementTray && IsOverflowMenu(pointed)) {
        AvoidOverflowMenu(GetAncestor(pointed, GA_ROOT), work, gap);
    }

    // Far enough to start out of sight behind that edge. With no edge behind
    // it, the flyout only nudges in as it fades.
    if (!againstEdge) {
        g_travel = scale(24);
    } else if (g_slideDirection.y > 0) {
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
    if (g_settings.minimalTitleBar) {
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

// Takes the size an edge is dragged to. The window is borderless while the
// title bar is hidden, so its size is the part that's seen.
void ResizeFlyout(HWND hWnd, WINDOWPOS* position) {
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    GetMonitorInfoW(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &info);
    const RECT& work = info.rcWork;
    LONG width = std::clamp<LONG>(position->cx - g_frameInsets.left - g_frameInsets.right,
                                  MinFlyoutWidth(),
                                  std::max<LONG>(MinFlyoutWidth(), work.right - work.left));
    LONG height = std::clamp<LONG>(position->cy - g_frameInsets.top - g_frameInsets.bottom,
                                   MinFlyoutHeight(),
                                   std::max<LONG>(MinFlyoutHeight(), work.bottom - work.top));
    LONG left = position->x + g_frameInsets.left;
    LONG top = position->y + g_frameInsets.top;
    g_restRect = {left, top, left + width, top + height};
}

// Keeps the size the flyout was dragged to for the next time it opens.
void RememberSize() {
    g_settings.width =
        MulDiv(g_restRect.right - g_restRect.left, USER_DEFAULT_SCREEN_DPI, FlyoutDpi());
    g_settings.height =
        MulDiv(g_restRect.bottom - g_restRect.top, USER_DEFAULT_SCREEN_DPI, FlyoutDpi());
}

void MoveFlyout(HWND hWnd) {
    POINT position = FlyoutPosition();
    SetWindowPos(hWnd, nullptr, position.x, position.y, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
}

// Waits for each frame of the compositor here rather than on the window's
// thread, which WhatsApp needs to draw while the flyout moves.
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
    // Started on first use, so the mod can still load if threads are short.
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

// Windows 11's rounded corners, the same as its own flyouts have. Windows
// draws them itself, so they stay smooth and keep the drop shadow.
void RoundCorners(HWND hWnd, bool round) {
    DWM_WINDOW_CORNER_PREFERENCE corner = round ? DWMWCP_ROUND : DWMWCP_DEFAULT;
    DwmSetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));
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
    if (closing) ShowWindow_Original(hWnd, SW_HIDE);
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
        RoundCorners(hWnd, true);  // The fade turns layering on and off.
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
// the screen when WhatsApp draws stay empty, so it stays hidden where the
// whole of it is on the screen until WhatsApp has drawn it.
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
        RoundCorners(hWnd, true);  // Reset by the app while it was hidden.
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

// Opens the flyout for the shortcut, after the tray icon closed it or after
// WhatsApp started hidden.
void ShowFlyout(HWND hWnd) {
    if (!g_flyout) return;
    if (g_closing) {
        ReopenFlyout();
    } else if (!IsWindowVisible(hWnd)) {
        // Goes through WM_WINDOWPOSCHANGING like WhatsApp's own show, which
        // places the flyout and starts the reveal.
        ShowWindow_Original(hWnd, IsIconic(hWnd) ? SW_RESTORE : SW_SHOW);
    }
    SetForegroundWindow_Original(hWnd);
}

std::wstring Trim(const std::wstring& text) {
    size_t start = 0;
    size_t end = text.size();
    while (start < end && iswspace(text[start])) start++;
    while (end > start && iswspace(text[end - 1])) end--;
    return text.substr(start, end - start);
}

// Reads shortcuts like "Win+Shift+W", the way they are written in the
// settings.
bool ParseHotkey(const std::wstring& text, UINT& modifiers, UINT& key) {
    modifiers = 0;
    key = 0;
    size_t start = 0;
    while (start <= text.size()) {
        size_t end = text.find(L'+', start);
        if (end == std::wstring::npos) end = text.size();
        std::wstring token = Trim(text.substr(start, end - start));
        start = end + 1;
        if (token.empty()) {
            if (end == text.size()) break;
            continue;
        }
        for (wchar_t& c : token) c = static_cast<wchar_t>(towlower(c));

        if (token == L"ctrl" || token == L"control") {
            modifiers |= MOD_CONTROL;
        } else if (token == L"alt") {
            modifiers |= MOD_ALT;
        } else if (token == L"shift") {
            modifiers |= MOD_SHIFT;
        } else if (token == L"win" || token == L"windows" || token == L"meta" ||
                   token == L"super") {
            modifiers |= MOD_WIN;
        } else if (token.size() == 1 &&
                   ((token[0] >= L'a' && token[0] <= L'z') ||
                    (token[0] >= L'0' && token[0] <= L'9'))) {
            key = static_cast<UINT>(towupper(token[0]));
        } else if (token.size() >= 2 && token.size() <= 3 && token[0] == L'f' &&
                   iswdigit(token[1])) {
            int number = 0;
            for (size_t i = 1; i < token.size(); i++) {
                if (!iswdigit(token[i])) {
                    number = 0;
                    break;
                }
                number = number * 10 + static_cast<int>(token[i] - L'0');
            }
            if (number >= 1 && number <= 24) key = VK_F1 + number - 1;
        } else {
            static const struct {
                PCWSTR name;
                UINT vk;
            } kKeys[] = {
                {L"space", VK_SPACE},     {L"enter", VK_RETURN},
                {L"return", VK_RETURN},   {L"tab", VK_TAB},
                {L"backspace", VK_BACK},  {L"insert", VK_INSERT},
                {L"ins", VK_INSERT},      {L"delete", VK_DELETE},
                {L"del", VK_DELETE},      {L"home", VK_HOME},
                {L"end", VK_END},         {L"pageup", VK_PRIOR},
                {L"pgup", VK_PRIOR},      {L"pagedown", VK_NEXT},
                {L"pgdn", VK_NEXT},       {L"up", VK_UP},
                {L"down", VK_DOWN},       {L"left", VK_LEFT},
                {L"right", VK_RIGHT},     {L"printscreen", VK_SNAPSHOT},
                {L"prtsc", VK_SNAPSHOT},  {L"pause", VK_PAUSE},
                {L"`", VK_OEM_3},         {L"-", VK_OEM_MINUS},
                {L"=", VK_OEM_PLUS},      {L"[", VK_OEM_4},
                {L"]", VK_OEM_6},         {L"\\", VK_OEM_5},
                {L";", VK_OEM_1},         {L"'", VK_OEM_7},
                {L",", VK_OEM_COMMA},     {L".", VK_OEM_PERIOD},
                {L"/", VK_OEM_2},
            };
            bool found = false;
            for (const auto& entry : kKeys) {
                if (token == entry.name) {
                    key = entry.vk;
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }
    }
    // A key without a modifier would be taken from every other program.
    return key != 0 && modifiers != 0;
}

// Registered on the window itself, so WM_HOTKEY arrives on the thread that
// owns it, next to every other message the flyout needs.
bool ApplyToggleHotkey(HWND hWnd) {
    if (g_hotkeyRegistered) {
        UnregisterHotKey(hWnd, kToggleHotkeyId);
        g_hotkeyRegistered = false;
    }
    if (!g_settings.hotkeyKey) return false;
    if (RegisterHotKey(hWnd, kToggleHotkeyId, g_settings.hotkeyModifiers | MOD_NOREPEAT,
                       g_settings.hotkeyKey)) {
        g_hotkeyRegistered = true;
        return true;
    }
    DWORD error = GetLastError();
    if (error == ERROR_HOTKEY_ALREADY_REGISTERED) {
        Wh_Log(L"The shortcut is taken by another program, choose another one in the settings");
    } else {
        Wh_Log(L"Can't register the shortcut, error %u", error);
    }
    return false;
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
    // WhatsApp's own dialogs, like the file picker, keep the flyout open.
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

// Clicking the tray icon makes WhatsApp show and focus its window. While the
// flyout is open, or that same click just closed it, close it instead.
// Returns true if the request was used up.
bool HandleShowRequest(HWND hWnd) {
    DWORD now = GetTickCount();
    bool open = IsWindowVisible(hWnd) && !g_closing;
    // WhatsApp asks more than once for one click.
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

// WhatsApp's tray icon runs on a thread of its own, so this may be called
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
    g_resizing = false;
    if (!enable && g_hotkeyRegistered) {
        UnregisterHotKey(hWnd, kToggleHotkeyId);
        g_hotkeyRegistered = false;
    }

    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    bool toolWindow = enable && g_settings.hideFromTaskbar;
    if (toolWindow && !(exStyle & WS_EX_TOOLWINDOW)) {
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_TOOLWINDOW);
        g_addedToolWindow = true;
    } else if (!toolWindow && g_addedToolWindow) {
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle & ~WS_EX_TOOLWINDOW);
        g_addedToolWindow = false;
    }

    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    // Windows leaves the corners of a maximized window square, and the app
    // can leave that flag set on its window.
    if (enable && (style & WS_MAXIMIZE)) {
        g_removedMaximize = true;
        style &= ~WS_MAXIMIZE;
        SetWindowLongPtrW(hWnd, GWL_STYLE, style);
    } else if (!enable && g_removedMaximize) {
        g_removedMaximize = false;
        SetWindowLongPtrW(hWnd, GWL_STYLE, style | WS_MAXIMIZE);
    }

    // Leaves only the close button on the title bar.
    constexpr LONG_PTR boxes = WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    if (enable && g_settings.minimalTitleBar) {
        g_removedBoxes |= style & boxes;
        SetWindowLongPtrW(hWnd, GWL_STYLE, style & ~boxes);
    } else if (g_removedBoxes) {
        SetWindowLongPtrW(hWnd, GWL_STYLE, style | g_removedBoxes);
        g_removedBoxes = 0;
    }

    // The flyout slides in by itself.
    BOOL noTransitions = enable;
    DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &noTransitions, sizeof(noTransitions));
    RoundCorners(hWnd, enable);

    if (enable) {
        ApplyToggleHotkey(hWnd);
        PlaceFlyout(hWnd);
        if (IsWindowVisible(hWnd)) KeepNotificationsAway(hWnd);
        if (!g_settings.moveNotifications) ReturnNotifications();
        // Moves an open flyout into place too, see WM_WINDOWPOSCHANGING.
        SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
        return;
    }

    ReturnNotifications();
    // WhatsApp's window goes back to its own size in the middle of the
    // screen.
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
        if (g_hotkeyRegistered) {
            UnregisterHotKey(hWnd, kToggleHotkeyId);
            g_hotkeyRegistered = false;
        }
        StopMotion();
        ReturnNotifications();
        g_window = nullptr;
        g_flyout = false;
    }
    if (!g_flyout) return DefSubclassProc(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        case WM_NCCALCSIZE:
            // With the minimal title bar, the window keeps its own title bar,
            // which ApplyFlyout has cut down to the close button. Otherwise
            // the client area covers the whole window and WhatsApp's title
            // bar comes back with its own buttons.
            if (wParam && !g_settings.minimalTitleBar) return 0;
            break;

        case WM_NCHITTEST: {
            LRESULT hit = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            // The edges resize the flyout, which has no visible frame.
            bool edge = hit >= HTLEFT && hit <= HTBOTTOMRIGHT;
            g_hitEdge = edge ? hit : 0;
            if (edge && !g_settings.resizable) return HTBORDER;
            return hit;
        }

        case WM_GETMINMAXINFO: {
            // WhatsApp's own minimum size is larger than a flyout.
            LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            auto* limits = reinterpret_cast<MINMAXINFO*>(lParam);
            limits->ptMinTrackSize = {MinFlyoutWidth(), MinFlyoutHeight()};
            return result;
        }

        case WM_ENTERSIZEMOVE:
            // Not a title bar drag, and not while it slides, as the size comes
            // from where it is then.
            g_resizing = g_settings.resizable && g_hitEdge && !g_motionStart;
            break;

        case WM_EXITSIZEMOVE:
            if (g_resizing) {
                g_resizing = false;
                RememberSize();
            }
            break;

        case WM_WINDOWPOSCHANGING: {
            auto* position = reinterpret_cast<WINDOWPOS*>(lParam);
            if (position->flags & SWP_HIDEWINDOW) {
                // WhatsApp hid its window, as it does from its tray icon.
                StopMotion();
                ReturnNotifications();
                break;
            }
            bool showing = !IsWindowVisible(hWnd);
            if (showing) {
                if (!(position->flags & SWP_SHOWWINDOW)) break;
                OpenFlyout(hWnd);
            }
            // The size asked for while an edge is dragged is the one it gets.
            if (g_resizing) ResizeFlyout(hWnd, position);
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

        case WM_HOTKEY:
            if (wParam == kToggleHotkeyId) {
                if (IsWindowVisible(hWnd) && !g_closing) {
                    CloseFlyout(hWnd);
                } else {
                    ShowFlyout(hWnd);
                }
                return 0;
            }
            break;

        case WM_SYSCOMMAND:
            switch (wParam & 0xFFF0) {
                case SC_CLOSE:
                case SC_MINIMIZE:
                    CloseFlyout(hWnd);
                    return 0;
                case SC_MAXIMIZE:
                case SC_MOVE:
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
        Wh_Log(L"Failed to subclass the WhatsApp window");
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
    if (hWnd && !hWndParent && !g_window &&
        IsWhatsAppWindowCreation(hWnd, lpClassName, lpWindowName)) {
        Wh_Log(L"Caught the WhatsApp window as it was made");
        Attach(hWnd);
    }
    return hWnd;
}

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

BOOL WINAPI SetForegroundWindow_Hook(HWND hWnd) {
    if (ConsumeShowRequest(hWnd)) return FALSE;
    return SetForegroundWindow_Original(hWnd);
}

// The Position setting, by the names its options use.
FlyoutPlacement ParsePlacement(PCWSTR value) {
    if (wcscmp(value, L"bottomRight") == 0) return kPlacementBottomRight;
    if (wcscmp(value, L"bottomLeft") == 0) return kPlacementBottomLeft;
    if (wcscmp(value, L"topRight") == 0) return kPlacementTopRight;
    if (wcscmp(value, L"topLeft") == 0) return kPlacementTopLeft;
    if (wcscmp(value, L"center") == 0) return kPlacementCenter;
    return kPlacementTray;
}

void LoadSettings() {
    g_settings.width = std::max(Wh_GetIntSetting(L"width"), 200);
    g_settings.height = std::max(Wh_GetIntSetting(L"height"), 200);
    WindhawkUtils::StringSetting position = WindhawkUtils::StringSetting::make(L"position");
    g_settings.placement = ParsePlacement(position.get());
    g_settings.offsetX = std::clamp(Wh_GetIntSetting(L"offsetX"), -2000, 2000);
    g_settings.offsetY = std::clamp(Wh_GetIntSetting(L"offsetY"), -2000, 2000);
    g_settings.closeWhenFocusIsLost = Wh_GetIntSetting(L"closeWhenFocusIsLost");
    g_settings.minimalTitleBar = Wh_GetIntSetting(L"minimalTitleBar");
    g_settings.resizable = Wh_GetIntSetting(L"resizable");
    g_settings.hideFromTaskbar = Wh_GetIntSetting(L"hideFromTaskbar");
    g_settings.moveNotifications = Wh_GetIntSetting(L"moveNotifications");

    WindhawkUtils::StringSetting animation = WindhawkUtils::StringSetting::make(L"animation");
    g_settings.slide = wcscmp(animation.get(), L"off") != 0;
    g_settings.fade = g_settings.slide && wcscmp(animation.get(), L"slide") != 0;

    g_settings.hotkeyModifiers = 0;
    g_settings.hotkeyKey = 0;
    WindhawkUtils::StringSetting hotkey = WindhawkUtils::StringSetting::make(L"hotkey");
    UINT modifiers = 0;
    UINT key = 0;
    if (hotkey.get()[0] && ParseHotkey(hotkey.get(), modifiers, key)) {
        g_settings.hotkeyModifiers = modifiers;
        g_settings.hotkeyKey = key;
    } else if (hotkey.get()[0]) {
        Wh_Log(L"Can't read the shortcut '%s'", hotkey.get());
    }
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_applyMessage = RegisterWindowMessageW(L"Windhawk_WhatsAppFlyout_Apply");
    g_tickMessage = RegisterWindowMessageW(L"Windhawk_WhatsAppFlyout_Tick");
    g_showRequestMessage = RegisterWindowMessageW(L"Windhawk_WhatsAppFlyout_ShowRequest");
    g_frameEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_frameEvent) return FALSE;
    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original);
    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook, &ShowWindow_Original);
    WindhawkUtils::SetFunctionHook(SetForegroundWindow, SetForegroundWindow_Hook,
                                   &SetForegroundWindow_Original);
    return TRUE;
}

BOOL CALLBACK AttachToExistingWindow(HWND hWnd, LPARAM) {
    if (!IsWhatsAppWindow(hWnd)) return TRUE;
    Attach(hWnd);
    return FALSE;
}

void Wh_ModAfterInit() {
    // WhatsApp may have made its window before the mod loaded.
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

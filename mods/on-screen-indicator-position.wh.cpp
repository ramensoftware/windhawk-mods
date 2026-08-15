// ==WindhawkMod==
// @id              on-screen-indicator-position
// @name            On-Screen Indicator Position
// @description     Place the volume/brightness/camera on-screen indicator anywhere on the screen, not just the three positions Windows offers
// @version         1.0.0
// @author          mario0318
// @github          https://github.com/mario0318
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lshcore
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# On-Screen Indicator Position

Windows 11 shows an on-screen indicator when you change the volume or
brightness, toggle airplane mode, or when the camera or microphone privacy state
changes. Under **Settings > System > Notifications > On-screen indicators**,
Windows lets you put it in one of three places: top left, top center, or bottom
center.

This mod replaces that with a full nine-point grid — any corner, any edge
center, or dead center of the screen — plus a pixel offset for fine-tuning and a
choice of which monitor it appears on.

## Positions

```
 Top left        Top center        Top right
 Middle left     Center            Middle right
 Bottom left     Bottom center     Bottom right
```

By default the indicator is placed inside the work area, so it won't sit
underneath the taskbar. Turn off "Keep clear of the taskbar" to use the whole
screen instead.

## Notes

* The slide-in animation direction is chosen by Windows from the built-in
  setting, not by this mod. If the animation looks wrong for your new position,
  change the built-in setting to whichever of the three has the animation you
  like, then let this mod do the actual placement.
* The indicator is drawn by Explorer, so the mod targets `explorer.exe`. If
  Explorer restarts, the mod keeps working.
* Tested on Windows 11 24H2/25H2. Older builds draw this indicator differently
  and are not supported.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: topRight
  $name: Position
  $description: Where on the screen the indicator appears
  $options:
  - windowsDefault: Windows default (don't move it)
  - topLeft: Top left
  - topCenter: Top center
  - topRight: Top right
  - middleLeft: Middle left
  - center: Center
  - middleRight: Middle right
  - bottomLeft: Bottom left
  - bottomCenter: Bottom center
  - bottomRight: Bottom right
- offsetX: 0
  $name: Horizontal offset
  $description: >-
    Pixels to nudge the indicator by. Positive moves right, negative moves left.
    Scaled with the monitor's DPI.
- offsetY: 0
  $name: Vertical offset
  $description: >-
    Pixels to nudge the indicator by. Positive moves down, negative moves up.
    Scaled with the monitor's DPI.
- monitor: windowsDefault
  $name: Monitor
  $description: Which monitor the indicator appears on
  $options:
  - windowsDefault: Windows default (don't change it)
  - primary: Primary monitor
  - cursor: Monitor with the mouse cursor
  - foreground: Monitor with the active window
- useWorkArea: true
  $name: Keep clear of the taskbar
  $description: >-
    Position within the work area rather than the full screen, so the indicator
    isn't hidden behind the taskbar.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <shellscalingapi.h>

// The Explorer thread that owns the indicator window names itself. This is an
// exact match, so the mod doesn't have to guess at window class names that
// change between builds.
constexpr PCWSTR kConfirmatorThreadName = L"HardwareConfirmator UI Thread";

enum class Position {
    windowsDefault,
    topLeft,
    topCenter,
    topRight,
    middleLeft,
    center,
    middleRight,
    bottomLeft,
    bottomCenter,
    bottomRight,
};

enum class MonitorChoice {
    windowsDefault,
    primary,
    cursor,
    foreground,
};

struct {
    Position position;
    int offsetX;
    int offsetY;
    MonitorChoice monitor;
    bool useWorkArea;
} g_settings;

bool CurrentThreadHasConfirmatorName() {
    bool match = false;

    PWSTR threadDescription;
    if (SUCCEEDED(GetThreadDescription(GetCurrentThread(),
                                       &threadDescription))) {
        match = wcscmp(threadDescription, kConfirmatorThreadName) == 0;
        LocalFree(threadDescription);
    }

    return match;
}

// Explorer calls SetWindowPos constantly, so the check has to be nearly free in
// the common case. The indicator is always moved by the thread that owns it, so
// it's enough to ask whether this thread is the indicator's, and that answer is
// cached per thread. A negative is only cached briefly, because a thread names
// itself shortly after it starts and may not have done so on the first call.
bool IsCurrentThreadConfirmatorThread() {
    static thread_local bool isConfirmator = false;
    static thread_local ULONGLONG lastCheckTick = 0;

    if (isConfirmator) {
        return true;
    }

    ULONGLONG tick = GetTickCount64();
    if (lastCheckTick && tick - lastCheckTick < 2000) {
        return false;
    }

    lastCheckTick = tick;
    isConfirmator = CurrentThreadHasConfirmatorName();
    return isConfirmator;
}

bool IsIndicatorWindow(HWND hWnd) {
    if (!hWnd || !IsCurrentThreadConfirmatorThread()) {
        return false;
    }

    DWORD processId = 0;
    DWORD threadId = GetWindowThreadProcessId(hWnd, &processId);
    if (threadId != GetCurrentThreadId() ||
        processId != GetCurrentProcessId()) {
        return false;
    }

    // Only top-level windows are placed on screen.
    return GetAncestor(hWnd, GA_PARENT) == GetDesktopWindow();
}

HMONITOR PickMonitor(HWND hWnd) {
    switch (g_settings.monitor) {
        case MonitorChoice::primary:
            return MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);

        case MonitorChoice::cursor: {
            POINT pt;
            if (GetCursorPos(&pt)) {
                return MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
            }
            break;
        }

        case MonitorChoice::foreground: {
            HWND foregroundWnd = GetForegroundWindow();
            if (foregroundWnd) {
                return MonitorFromWindow(foregroundWnd,
                                         MONITOR_DEFAULTTONEAREST);
            }
            break;
        }

        case MonitorChoice::windowsDefault:
            break;
    }

    return MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
}

// Places a window of the given size within `area` according to the configured
// position, applies the DPI-scaled offsets, and clamps the result so the window
// stays inside the monitor.
void CalculatePosition(HMONITOR monitor,
                       const RECT& area,
                       int width,
                       int height,
                       int* x,
                       int* y) {
    int left = area.left;
    int centerX = area.left + (area.right - area.left - width) / 2;
    int right = area.right - width;

    int top = area.top;
    int middleY = area.top + (area.bottom - area.top - height) / 2;
    int bottom = area.bottom - height;

    switch (g_settings.position) {
        case Position::topLeft:
            *x = left, *y = top;
            break;
        case Position::topCenter:
            *x = centerX, *y = top;
            break;
        case Position::topRight:
            *x = right, *y = top;
            break;
        case Position::middleLeft:
            *x = left, *y = middleY;
            break;
        case Position::center:
            *x = centerX, *y = middleY;
            break;
        case Position::middleRight:
            *x = right, *y = middleY;
            break;
        case Position::bottomLeft:
            *x = left, *y = bottom;
            break;
        case Position::bottomCenter:
            *x = centerX, *y = bottom;
            break;
        case Position::bottomRight:
            *x = right, *y = bottom;
            break;
        case Position::windowsDefault:
            return;
    }

    UINT dpiX = 96;
    UINT dpiY = 96;
    GetDpiForMonitor(monitor, MDT_DEFAULT, &dpiX, &dpiY);

    *x += MulDiv(g_settings.offsetX, dpiX, 96);
    *y += MulDiv(g_settings.offsetY, dpiY, 96);

    // An offset large enough to push the indicator off screen would just make
    // it invisible with no way to tell why, so keep it within the monitor.
    if (*x < area.left) {
        *x = area.left;
    } else if (*x > area.right - width) {
        *x = area.right - width;
    }

    if (*y < area.top) {
        *y = area.top;
    } else if (*y > area.bottom - height) {
        *y = area.bottom - height;
    }
}

bool AdjustIndicatorPos(HWND hWnd, int width, int height, int* x, int* y) {
    HMONITOR monitor = PickMonitor(hWnd);
    if (!monitor) {
        return false;
    }

    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    if (!GetMonitorInfo(monitor, &monitorInfo)) {
        return false;
    }

    const RECT& area =
        g_settings.useWorkArea ? monitorInfo.rcWork : monitorInfo.rcMonitor;

    if (area.right - area.left < width || area.bottom - area.top < height) {
        return false;
    }

    CalculatePosition(monitor, area, width, height, x, y);
    return true;
}

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;

BOOL WINAPI SetWindowPos_Hook(HWND hWnd,
                              HWND hWndInsertAfter,
                              int X,
                              int Y,
                              int cx,
                              int cy,
                              UINT uFlags) {
    auto original = [=]() {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy,
                                     uFlags);
    };

    if (g_settings.position == Position::windowsDefault) {
        return original();
    }

    if ((uFlags & (SWP_NOSIZE | SWP_NOMOVE)) == (SWP_NOSIZE | SWP_NOMOVE)) {
        return original();
    }

    if (!IsIndicatorWindow(hWnd)) {
        return original();
    }

    RECT rc{};
    if (!GetWindowRect(hWnd, &rc)) {
        return original();
    }

    int width = (uFlags & SWP_NOSIZE) ? rc.right - rc.left : cx;
    int height = (uFlags & SWP_NOSIZE) ? rc.bottom - rc.top : cy;

    // The window is sized and moved in separate calls. Move it on the sizing
    // call too, otherwise it lands in the right place only every other time.
    int x = (uFlags & SWP_NOMOVE) ? rc.left : X;
    int y = (uFlags & SWP_NOMOVE) ? rc.top : Y;

    if (!AdjustIndicatorPos(hWnd, width, height, &x, &y)) {
        return original();
    }

    Wh_Log(L"Moving indicator to %dx%d (%dx%d)", x, y, width, height);

    return SetWindowPos_Original(hWnd, hWndInsertAfter, x, y, cx, cy,
                                 uFlags & ~SWP_NOMOVE);
}

using MoveWindow_t = decltype(&MoveWindow);
MoveWindow_t MoveWindow_Original;

BOOL WINAPI MoveWindow_Hook(HWND hWnd,
                            int X,
                            int Y,
                            int nWidth,
                            int nHeight,
                            BOOL bRepaint) {
    auto original = [=]() {
        return MoveWindow_Original(hWnd, X, Y, nWidth, nHeight, bRepaint);
    };

    if (g_settings.position == Position::windowsDefault) {
        return original();
    }

    if (!IsIndicatorWindow(hWnd)) {
        return original();
    }

    int x = X;
    int y = Y;
    if (!AdjustIndicatorPos(hWnd, nWidth, nHeight, &x, &y)) {
        return original();
    }

    Wh_Log(L"Moving indicator to %dx%d (%dx%d)", x, y, nWidth, nHeight);

    return MoveWindow_Original(hWnd, x, y, nWidth, nHeight, bRepaint);
}

Position PositionFromString(PCWSTR value) {
    if (wcscmp(value, L"topLeft") == 0) {
        return Position::topLeft;
    } else if (wcscmp(value, L"topCenter") == 0) {
        return Position::topCenter;
    } else if (wcscmp(value, L"topRight") == 0) {
        return Position::topRight;
    } else if (wcscmp(value, L"middleLeft") == 0) {
        return Position::middleLeft;
    } else if (wcscmp(value, L"center") == 0) {
        return Position::center;
    } else if (wcscmp(value, L"middleRight") == 0) {
        return Position::middleRight;
    } else if (wcscmp(value, L"bottomLeft") == 0) {
        return Position::bottomLeft;
    } else if (wcscmp(value, L"bottomCenter") == 0) {
        return Position::bottomCenter;
    } else if (wcscmp(value, L"bottomRight") == 0) {
        return Position::bottomRight;
    }

    return Position::windowsDefault;
}

MonitorChoice MonitorChoiceFromString(PCWSTR value) {
    if (wcscmp(value, L"primary") == 0) {
        return MonitorChoice::primary;
    } else if (wcscmp(value, L"cursor") == 0) {
        return MonitorChoice::cursor;
    } else if (wcscmp(value, L"foreground") == 0) {
        return MonitorChoice::foreground;
    }

    return MonitorChoice::windowsDefault;
}

void LoadSettings() {
    WindhawkUtils::StringSetting position =
        WindhawkUtils::StringSetting::make(L"position");
    g_settings.position = PositionFromString(position.get());

    g_settings.offsetX = Wh_GetIntSetting(L"offsetX");
    g_settings.offsetY = Wh_GetIntSetting(L"offsetY");

    WindhawkUtils::StringSetting monitor =
        WindhawkUtils::StringSetting::make(L"monitor");
    g_settings.monitor = MonitorChoiceFromString(monitor.get());

    g_settings.useWorkArea = Wh_GetIntSetting(L"useWorkArea");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook,
                                   &SetWindowPos_Original);
    WindhawkUtils::SetFunctionHook(MoveWindow, MoveWindow_Hook,
                                   &MoveWindow_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    LoadSettings();

    return TRUE;
}

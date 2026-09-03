// ==WindhawkMod==
// @id              edge-doubleclick-resize
// @name            Double-Click Edge to Maximize Width/Height
// @description     Double-click the left/right window edge to maximize width, the top/bottom edge to maximize height
// @description:ar-SA دبل كليك على حافة النافذة اليسرى/اليمنى يكبر العرض، والعلوية/السفلية يكبر الطول
// @version         1.0
// @author          Hamid
// @github          https://github.com/nh4700-ai
// @include         *
// @compilerOptions -ldwmapi
// @license          MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Double-Click Edge to Maximize Width/Height

Double-click on a window's border to resize it along one axis, without
affecting the other:

- Left or right edge -> maximizes the width (height stays unchanged).
- Top or bottom edge -> handled by Windows' own built-in vertical
  maximize/restore toggle (unchanged by this mod).
- A corner -> maximizes both width and height together.

This adds a horizontal counterpart to Windows' existing vertical
double-click-to-maximize behavior, which only covers the top/bottom
edges natively.

![demo](https://i.imgur.com/B4HtYLf.gif)

## Notes

- Applies to all windows (`@include *`).
- Only affects resizable windows that are not already maximized or
  minimized.
- Apps that implement their own non-client hit testing and never
  forward `WM_NCLBUTTONDBLCLK` to the default window procedure
  (e.g. many Chromium/Electron, WinUI, and Qt frameless windows) are
  not affected by this mod, since there's no reliable hook point for
  those. This is a limitation of the approach, not a bug.
- Uses the work area (excludes the taskbar) rather than the full
  monitor bounds.

---

## دبل كليك على حافة النافذة لتكبير العرض أو الطول

- الحافة اليسرى أو اليمنى -> يكبّر العرض بالكامل (يبقى الطول كما هو).
- الحافة العلوية أو السفلية -> يُترك سلوك ويندوز الأصلي (تكبير/استعادة الطول) كما هو.
- الزاوية -> يكبّر الاثنين معاً.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <dwmapi.h>
#include <windhawk_utils.h>

using DefWindowProcW_t = decltype(&DefWindowProcW);
DefWindowProcW_t DefWindowProcW_Original;

using DefWindowProcA_t = decltype(&DefWindowProcA);
DefWindowProcA_t DefWindowProcA_Original;

// Shared logic: inspects the hit-test code and resizes width and/or
// height depending on which edge was double-clicked. Returns true if
// the message was handled (caller should suppress default processing).
bool HandleEdgeDoubleClick(HWND hWnd, WPARAM wParam) {
    int hit = (int)wParam;

    // Leave pure top/bottom hits to the default window procedure so
    // Windows' built-in vertical maximize/restore toggle keeps working
    // exactly as it did before this mod was installed. Only left/right
    // edges and corners are handled here.
    if (hit == HTTOP || hit == HTBOTTOM)
        return false;

    bool onLeft   = (hit == HTLEFT   || hit == HTTOPLEFT    || hit == HTBOTTOMLEFT);
    bool onRight  = (hit == HTRIGHT  || hit == HTTOPRIGHT   || hit == HTBOTTOMRIGHT);
    bool onTop    = (hit == HTTOPLEFT  || hit == HTTOPRIGHT);
    bool onBottom = (hit == HTBOTTOMLEFT || hit == HTBOTTOMRIGHT);

    if (!(onLeft || onRight || onTop || onBottom))
        return false; // Not an edge/corner we handle.

    // Skip maximized/minimized windows, and windows that aren't
    // actually resizable (custom-chrome apps can report border hit
    // codes even when the default frame wouldn't).
    if (IsZoomed(hWnd) || IsIconic(hWnd))
        return false;

    // Skip child windows: GetWindowRect/rcWork are in screen coordinates,
    // but SetWindowPos on a child window treats x/y as parent-client
    // coordinates, so applying screen-space math would fling the child
    // out of its container.
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (style & WS_CHILD)
        return false;

    if (!(style & WS_THICKFRAME))
        return false;

    RECT wr; // Actual window rect (includes the invisible resize border).
    if (!GetWindowRect(hWnd, &wr))
        return false;

    RECT vr = wr; // Visible frame bounds.
    if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS,
                                      &vr, sizeof(vr)))) {
        vr = wr; // Fall back to zero insets if the DWM call fails.
    }

    // Invisible border thickness on each side (difference between the
    // real and visible bounds).
    int leftInset   = vr.left   - wr.left;
    int topInset    = vr.top    - wr.top;
    int rightInset  = wr.right  - vr.right;
    int bottomInset = wr.bottom - vr.bottom;

    // DWMWA_EXTENDED_FRAME_BOUNDS is always in physical pixels, while
    // GetWindowRect is DPI-virtualized for processes that aren't
    // per-monitor DPI aware. With @include * this mod runs inside such
    // processes, so on a scaled display the two rects can be in
    // different coordinate spaces. If the insets are far larger than a
    // plausible frame width, discard them rather than risk throwing the
    // window off-screen.
    const int maxInset =
        (GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER)) * 4;
    if (leftInset < 0 || topInset < 0 || rightInset < 0 || bottomInset < 0 ||
        leftInset > maxInset || topInset > maxInset ||
        rightInset > maxInset || bottomInset > maxInset) {
        leftInset = topInset = rightInset = bottomInset = 0;
    }

    HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfo(hMon, &mi))
        return false;

    int newX = wr.left;
    int newY = wr.top;
    int newW = wr.right - wr.left;
    int newH = wr.bottom - wr.top;

    if (onLeft || onRight) {
        // Align the visible edges with the screen edges exactly,
        // compensating for the invisible border.
        newX = mi.rcWork.left - leftInset;
        newW = (mi.rcWork.right + rightInset) - newX;
    }

    if (onTop || onBottom) {
        newY = mi.rcWork.top - topInset;
        newH = (mi.rcWork.bottom + bottomInset) - newY;
    }

    SetWindowPos(hWnd, nullptr, newX, newY, newW, newH,
                 SWP_NOZORDER | SWP_NOACTIVATE);

    return true; // Handled; caller should not call the original proc.
}

LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == WM_NCLBUTTONDBLCLK) {
        if (HandleEdgeDoubleClick(hWnd, wParam))
            return 0;
    }
    return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
}

LRESULT WINAPI DefWindowProcA_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == WM_NCLBUTTONDBLCLK) {
        if (HandleEdgeDoubleClick(hWnd, wParam))
            return 0;
    }
    return DefWindowProcA_Original(hWnd, Msg, wParam, lParam);
}

BOOL Wh_ModInit() {
    WindhawkUtils::SetFunctionHook(DefWindowProcW, DefWindowProcW_Hook,
                                    &DefWindowProcW_Original);
    WindhawkUtils::SetFunctionHook(DefWindowProcA, DefWindowProcA_Hook,
                                    &DefWindowProcA_Original);
    return TRUE;
}

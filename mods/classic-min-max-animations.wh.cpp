// ==WindhawkMod==
// @id              classic-min-max-animations
// @name            Classic Minimize/Maximize Animations
// @description     Restores the classic minimize/maximize animations used without DWM
// @version         1.1.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -lgdi32 -lversion -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Minimize/Maximize Animations
From Windows 95 to XP, and in Vista and above without DWM running, windows had
a minimize/maximize animation that involved the titlebar moving across the screen.
This mod makes that animation play with DWM enabled by reimplementing it.

## Previews
**Top-level window**:

![Top-level window](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-min-max-animations/toplevel.gif)

**MDI child window**:

![MDI child window](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/classic-min-max-animations/mdi.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- topmost_bug_mitigation: false
  $name: Topmost bug mitigation
  $description: Mitigation for a bug where console windows become topmost randomly. Only use with default Windows 10/11 Explorer or ep_taskbar.
    Restart Explorer after toggling.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <shellscalingapi.h>

/* Defines */
//#define SHIFT_SLOWDOWN // Hold Shift to slow down the animation

#ifdef _WIN64
#   define FASTCALL_STR L"__cdecl"
#else
#   define FASTCALL_STR L"__fastcall"
#endif
#define min(x, y)       ((x) > (y)) ? (y) : (x)
#define max(x, y)       ((x) > (y)) ? (x) : (y)
#define RECTWIDTH(rc)   ((rc).right - (rc).left)
#define RECTHEIGHT(rc)  ((rc).bottom - (rc).top)

/* Globals */
UINT       g_uMsgMinMax      =     -1;
bool       g_fAnimating      =  false;
bool       g_fDisabled       =  false;
HINSTANCE  g_hinst           =   NULL;
HWND       g_hwndAnim        =   NULL;
HANDLE     g_hAnimWndThread  =   NULL;

/* Type and function definitions */
typedef BOOL (WINAPI *GetWindowMinimizeRect_t)(HWND hwnd, LPRECT prcMin);
GetWindowMinimizeRect_t GetWindowMinimizeRect;
typedef HICON (WINAPI *InternalGetWindowIcon_t)(HWND hwnd, UINT uIconType);
InternalGetWindowIcon_t InternalGetWindowIcon;

typedef struct tagWND WND, *PWND;

typedef struct _MINMAXPARAMS
{
    HWND   hwnd;
    HICON  hIcon;
    WCHAR  szCaptionText[256];
    RECT   rcMax;
} MINMAXPARAMS, *LPMINMAXPARAMS;

BOOL (__fastcall *GetWindowBordersForDpi)(LONG lStyle, DWORD dwExStyle, BOOL fWindow, BOOL fClient, UINT dpi);
PWND (__fastcall *ValidateHwnd)(HWND hWnd);
#ifndef _M_IX86
BOOL (WINAPI *_HasCaptionIcon)(PWND pwnd);
#else // x86-32 calling convention nonsense
BOOL (WINAPI *_HasCaptionIcon)(PWND pwnd, int);
#define _HasCaptionIcon(pwnd) _HasCaptionIcon(pwnd, 0)
#endif

DWORD CALLBACK AnimWndThreadProc(HANDLE hEvent);

bool IsWindowPerMonitorDpiAware(HWND hwnd)
{
    DPI_AWARENESS_CONTEXT context = GetWindowDpiAwarenessContext(hwnd);
    return (AreDpiAwarenessContextsEqual(context, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE) ||
            AreDpiAwarenessContextsEqual(context, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2));
}

inline bool IsMinAnimate()
{
    return Wh_GetIntValue(L"MinAnimate", 1) != 0;
}

#pragma region "DWP hooks"

LPMINMAXPARAMS CreateMinMaxParams(HWND hwnd, UINT cmd)
{
    LPMINMAXPARAMS lpmmp = (LPMINMAXPARAMS)GlobalAlloc(GPTR, sizeof(MINMAXPARAMS));
    if (!lpmmp)
        return nullptr;

    lpmmp->hwnd = hwnd;

    lpmmp->hIcon = InternalGetWindowIcon(hwnd, ICON_SMALL);

    /* The window's thread is blocked by the animation, and DrawCaption is unabled
       to send WM_GETTEXT to get the text, so we draw it ourselves. The array size
       is accurate to the amount of characters Windows displays in the titlebar. */
    lpmmp->szCaptionText[0] = L'\0';
    GetWindowTextW(hwnd, lpmmp->szCaptionText, ARRAYSIZE(lpmmp->szCaptionText));

    /* We also need to get the maximize rect while we still have access to the message
       queue. We construct a default maximized rect and then send that over to the window's
       WM_GETMINMAXINFO handler to get the true maximized size, which may not be the same
       as the monitor work area (Conhost V1). */
    if (cmd == SC_MAXIMIZE || cmd == SC_RESTORE)
    {
        MINMAXINFO mmi = {};
        int cBorders = GetWindowBordersForDpi(
            GetWindowLongW(hwnd, GWL_STYLE),
            GetWindowLongW(hwnd, GWL_EXSTYLE),
            TRUE, FALSE, GetDpiForWindow(hwnd)
        );
        HMONITOR hmon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO mi = { sizeof(mi) };
        GetMonitorInfoW(hmon, &mi);
        InflateRect(&mi.rcWork, cBorders, cBorders);
        
        mmi.ptMaxPosition.x = mi.rcWork.left;
        mmi.ptMaxPosition.y = mi.rcWork.top;
        mmi.ptMaxSize.x     = RECTWIDTH(mi.rcWork);
        mmi.ptMaxSize.y     = RECTHEIGHT(mi.rcWork);

        SendMessageW(hwnd, WM_GETMINMAXINFO, 0, (LPARAM)&mmi);
        lpmmp->rcMax.left   = mmi.ptMaxPosition.x;
        lpmmp->rcMax.top    = mmi.ptMaxPosition.y;
        lpmmp->rcMax.right  = mmi.ptMaxPosition.x + mmi.ptMaxSize.x;
        lpmmp->rcMax.bottom = mmi.ptMaxPosition.y + mmi.ptMaxSize.y;
    }
    
    return lpmmp;
}

bool WaitForAnimWndThread(void)
{
    /* If we don't have the animation thread yet, start it and wait for the
        window to come up. */
    if (!g_hAnimWndThread)
    {
        HANDLE hEvent = CreateEventW(nullptr, TRUE, FALSE, L"UAH_MinMax_WaitForWindow");
        g_hAnimWndThread = CreateThread(nullptr, 0, AnimWndThreadProc, hEvent, 0, nullptr);
        // If it doesn't come up after one second, we messed up bad somehow.
        DWORD dwWait = WaitForSingleObject(hEvent, 1000);
        if (dwWait != WAIT_OBJECT_0)
        {
            MessageBoxW(
                NULL,
                L"Somehow, creating the animation window failed. For your convenience, the mod "
                L"has been disabled for this process.",
                // =================================== //
                L"Windhawk: Classic Minimize/Maximize Animations",
                MB_ICONERROR
            );
            g_fDisabled = true;
            return false;
        }
        CloseHandle(hEvent);

        // Extra sleep to circumvent a race condition
        Sleep(30);
    }
    return true;
}

void MinMaxWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (g_fDisabled)
        return;

    switch (uMsg)
    {
        case WM_SYSCOMMAND:
        {
            UINT cmd = wParam & 0xFFF0;
            switch (cmd)
            {
                case SC_MINIMIZE:
                case SC_MAXIMIZE:
                case SC_RESTORE:
                {
                    if (!IsMinAnimate() || !WaitForAnimWndThread() || g_fAnimating)
                        return;

                    LPMINMAXPARAMS lpmmp = CreateMinMaxParams(hwnd, cmd);
                    if (!lpmmp)
                        return;

                    SendMessageW(g_hwndAnim, g_uMsgMinMax, cmd, (LPARAM)lpmmp);
                    break;
                }
            }
            return;
        }
        default:
            return;
    }
}

#define DWP_HOOK_(name, defArgs, callArgs) \
LRESULT (CALLBACK *name ## _orig) defArgs; \
LRESULT CALLBACK name ## _hook  defArgs \
{ \
    MinMaxWndProc(hWnd, uMsg, wParam, lParam); \
    return name ## _orig  callArgs; \
}

#define DWP_HOOK(name, defArgs, callArgs)  \
    DWP_HOOK_(name ## A, defArgs, callArgs) \
    DWP_HOOK_(name ## W, defArgs, callArgs) \

DWP_HOOK(
    DefWindowProc,
    (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, uMsg, wParam, lParam))
DWP_HOOK(
    DefFrameProc,
    (HWND hWnd, HWND hWndMDIClient, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, hWndMDIClient, uMsg, wParam, lParam))
DWP_HOOK(
    DefMDIChildProc,
    (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, uMsg, wParam, lParam))
DWP_HOOK(
    DefDlgProc,
    (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam),
    (hWnd, uMsg, wParam, lParam))

using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t ShowWindow_orig;
BOOL WINAPI ShowWindow_hook(HWND hWnd, int nCmdShow)
{
    if (!IsMinAnimate() || g_fDisabled || g_fAnimating || !IsWindowVisible(hWnd))
        return ShowWindow_orig(hWnd, nCmdShow);

    DWORD dwStyle = GetWindowLongW(hWnd, GWL_STYLE);
    // Don't want to animate a window without a caption.
    if (!(dwStyle & WS_CAPTION))
        return ShowWindow_orig(hWnd, nCmdShow);

    UINT cmd;
    switch (nCmdShow)
    {
        case SW_MAXIMIZE:
            cmd = SC_MAXIMIZE;
            break;
        case SW_MINIMIZE:
            cmd = SC_MINIMIZE;
            break;
        case SW_RESTORE:
            cmd = SC_RESTORE;
            break;
        default:
            return ShowWindow_orig(hWnd, nCmdShow);
    }
    
    if (!WaitForAnimWndThread())
        return ShowWindow_orig(hWnd, nCmdShow);

    LPMINMAXPARAMS lpmmp = CreateMinMaxParams(hWnd, cmd);
    if (!lpmmp)
        return ShowWindow_orig(hWnd, nCmdShow);

    SendMessageW(g_hwndAnim, g_uMsgMinMax, cmd, (LPARAM)lpmmp);
    return ShowWindow_orig(hWnd, nCmdShow);
}

using ShowWindowAsync_t = decltype(&ShowWindowAsync);
ShowWindowAsync_t ShowWindowAsync_orig;
BOOL WINAPI ShowWindowAsync_hook(HWND hWnd, int nCmdShow)
{
    if (!IsMinAnimate() || g_fDisabled || g_fAnimating || !IsWindowVisible(hWnd))
        return ShowWindowAsync_orig(hWnd, nCmdShow);

    DWORD dwStyle = GetWindowLongW(hWnd, GWL_STYLE);
    // Don't want to animate a window without a caption.
    if (!(dwStyle & WS_CAPTION))
        return ShowWindowAsync_orig(hWnd, nCmdShow);

    UINT cmd;
    switch (nCmdShow)
    {
        case SW_MAXIMIZE:
            cmd = SC_MAXIMIZE;
            break;
        case SW_MINIMIZE:
            cmd = SC_MINIMIZE;
            break;
        case SW_RESTORE:
            cmd = SC_RESTORE;
            break;
        default:
            return ShowWindowAsync_orig(hWnd, nCmdShow);
    }
    
    if (!WaitForAnimWndThread())
        return ShowWindowAsync_orig(hWnd, nCmdShow);

    LPMINMAXPARAMS lpmmp = CreateMinMaxParams(hWnd, cmd);
    if (!lpmmp)
        return ShowWindowAsync_orig(hWnd, nCmdShow);

    SendMessageW(g_hwndAnim, g_uMsgMinMax, cmd, (LPARAM)lpmmp);
    return ShowWindowAsync_orig(hWnd, nCmdShow);
}

/* This hook only applies to Windows 10/11's explorer.exe. See the massive comment in
   Wh_ModInit for more details. */
using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_orig;
BOOL WINAPI SetWindowPos_hook(
    HWND hWnd,
    HWND hWndInsertAfter,
    int  X,
    int  Y,
    int  cx,
    int  cy,
    UINT uFlags
)
{
    Sleep(1);
    return SetWindowPos_orig(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

bool MinMaxSystemParametersInfo(
    UINT    uiAction,
    UINT    uiParam,
    LPVOID  pvParam,
    UINT    fWinIni,
    BOOL   *pfResult
)
{
    switch (uiAction)
    {
        case SPI_GETANIMATION:
        {
            LPANIMATIONINFO lpai = (LPANIMATIONINFO)pvParam;
            if (!lpai || uiParam != sizeof(*lpai) || lpai->cbSize != sizeof(*lpai))
            {
                *pfResult = FALSE;
                return true;
            }

            lpai->cbSize = sizeof(*lpai);
            lpai->iMinAnimate = IsMinAnimate();
            Wh_Log(L"Requesting MINANIMATE, value: %d", lpai->iMinAnimate);

            *pfResult = TRUE;
            return true;
        }
        case SPI_SETANIMATION:
        {
            LPANIMATIONINFO lpai = (LPANIMATIONINFO)pvParam;
            if (!lpai || lpai->cbSize != sizeof(*lpai))
            {
                *pfResult = FALSE;
                return true;
            }

            Wh_SetIntValue(L"MinAnimate", lpai->iMinAnimate != 0);
            Wh_Log(L"Setting MINANIMATE, value: %d", lpai->iMinAnimate);

            *pfResult = TRUE;
            return true;
        }
        default:
            return false;
    }
}

using SystemParametersInfoA_t = decltype(&SystemParametersInfoA);
SystemParametersInfoA_t SystemParametersInfoA_orig;
BOOL WINAPI SystemParametersInfoA_hook(
    UINT    uiAction,
    UINT    uiParam,
    LPVOID  pvParam,
    UINT    fWinIni
)
{
    BOOL fResult;
    if (MinMaxSystemParametersInfo(uiAction, uiParam, pvParam, fWinIni, &fResult))
        return fResult;
    return SystemParametersInfoA_orig(uiAction, uiParam, pvParam, fWinIni);
}

using SystemParametersInfoW_t = decltype(&SystemParametersInfoW);
SystemParametersInfoW_t SystemParametersInfoW_orig;
BOOL WINAPI SystemParametersInfoW_hook(
    UINT    uiAction,
    UINT    uiParam,
    LPVOID  pvParam,
    UINT    fWinIni
)
{
    BOOL fResult;
    if (MinMaxSystemParametersInfo(uiAction, uiParam, pvParam, fWinIni, &fResult))
        return fResult;
    return SystemParametersInfoW_orig(uiAction, uiParam, pvParam, fWinIni);
}

#pragma endregion // "DWP hooks"

#pragma region "Animation implementation"

void AdjustRectForWindowBorder(HWND hwnd, LPRECT prc)
{
    int cBorders = GetWindowBordersForDpi(
        GetWindowLongPtrW(hwnd, GWL_STYLE),
        GetWindowLongPtrW(hwnd, GWL_EXSTYLE),
        TRUE, FALSE,
        GetDpiForWindow(hwnd)
    );
    InflateRect(prc, -cBorders, 0);
    OffsetRect(prc, 0, cBorders);
    prc->bottom -= 2 * cBorders;
}

void GetWindowMinimizeRectForWindowDpi(HWND hwnd, LPRECT prcMin)
{
    GetWindowMinimizeRect(hwnd, prcMin);
    if (!IsWindowPerMonitorDpiAware(hwnd))
    {
        // Window might be DWM scaled and running in a different DPI than the belonging monitor.
        // We need to convert the virtualized coordinates to the physical pixels of the screen.
        MONITORINFO mi = { sizeof(mi) };
        // Monitor that the destination taskbar is on
        HMONITOR hmon = MonitorFromRect(prcMin, MONITOR_DEFAULTTOPRIMARY);
        if (hmon && GetMonitorInfoW(hmon, &mi))
        {
            // Convert to physical pixels
            UINT dpiX = 96, dpiY = 96;
            if (GetDpiForMonitor(hmon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY) == S_OK)
            {
                UINT uDpiWnd = GetDpiForWindow(hwnd);
                prcMin->left   = MulDiv(prcMin->left,    uDpiWnd, dpiX);
                prcMin->top    = MulDiv(prcMin->top,     uDpiWnd, dpiY);
                prcMin->right  = MulDiv(prcMin->right,   uDpiWnd, dpiX);
                prcMin->bottom = MulDiv(prcMin->bottom,  uDpiWnd, dpiY);
            }
        }
    }
}

LRESULT CALLBACK AnimWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == g_uMsgMinMax)
    {
#ifdef SHIFT_SLOWDOWN
        bool fShiftPressed = GetAsyncKeyState(VK_SHIFT) < 0;
        const UINT ANIMATION_DURATION_MS = fShiftPressed ? 1500 : 250;
#else
        constexpr UINT ANIMATION_DURATION_MS = 250;
#endif

        g_fAnimating = true;

        UINT cmd = (UINT)wParam;
        LPMINMAXPARAMS lpmmp = (LPMINMAXPARAMS)lParam;
        HWND hwndTarget = lpmmp->hwnd;

        // Source window DPI
        UINT uDpiWnd  = GetDpiForWindow(hwndTarget);
        // DPI of the monitor the window is on
        // Default to window DPI for per-monitor aware windows
        UINT uDpiMon  = uDpiWnd;

        if (!IsWindowPerMonitorDpiAware(hwndTarget))
        {
            MONITORINFO mi = { sizeof(mi) };
            HMONITOR hmon = MonitorFromWindow(hwndTarget, MONITOR_DEFAULTTOPRIMARY);
            GetMonitorInfo(hmon, &mi);
            UINT dpiX = 96, dpiY = 96;
            if (GetDpiForMonitor(hmon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY) == S_OK)
                uDpiMon = dpiX;
        }

        bool fAnimate = false;
        DWORD dwStyle = GetWindowLongW(hwndTarget, GWL_STYLE);
        switch (cmd)
        {
            case SC_MAXIMIZE:
                fAnimate = !(dwStyle & WS_MAXIMIZE);
                break;
            case SC_MINIMIZE:
                fAnimate = !(dwStyle & WS_MINIMIZE);
                break;
            case SC_RESTORE:
                fAnimate = (dwStyle & WS_MAXIMIZE) || (dwStyle & WS_MINIMIZE);
                break;
        }
        if (!fAnimate)
        {
            g_fAnimating = false;
            if (lpmmp->hIcon)
            {
                DestroyIcon(lpmmp->hIcon);
                lpmmp->hIcon = NULL;
            }
            GlobalFree(lpmmp);
            return 0;
        }

        /* Screenshot desktop */
        HDC hdcDesk = GetDC(NULL);
        HDC hdcMem  = CreateCompatibleDC(hdcDesk);

        int xVirtualScreen  = GetSystemMetrics(SM_XVIRTUALSCREEN);
        int yVirtualScreen  = GetSystemMetrics(SM_YVIRTUALSCREEN);
        int cxVirtualScreen = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int cyVirtualScreen = GetSystemMetrics(SM_CYVIRTUALSCREEN);

        HBITMAP hbmDesk = CreateCompatibleBitmap(hdcDesk, cxVirtualScreen, cyVirtualScreen);
        SelectObject(hdcMem, hbmDesk);
        BitBlt(
            hdcMem,
            0, 0,
            cxVirtualScreen, cyVirtualScreen,
            hdcDesk,
            xVirtualScreen, yVirtualScreen,
            SRCCOPY
        );

        DeleteDC(hdcMem);
        ReleaseDC(NULL, hdcDesk);

        /* Update window position and size */
        SetWindowPos(
            hwnd,
            HWND_TOPMOST,
            xVirtualScreen,
            yVirtualScreen,
            cxVirtualScreen,
            cyVirtualScreen,
            SWP_NOACTIVATE
        );

        RECT rcFrom, rcTo;

        /* Get initial rect */
        GetWindowRect(hwndTarget, &rcFrom);
        // Window is minimized, get the minimize rect instead of the regular
        // window rect
        if (MulDiv(rcFrom.left, uDpiWnd, uDpiMon) == -32000)
        {
            GetWindowMinimizeRectForWindowDpi(hwndTarget, &rcFrom);
        }
        AdjustRectForWindowBorder(hwndTarget, &rcFrom);

        /* Get rect after animation */
        switch (cmd)
        {
            case SC_MINIMIZE:
                GetWindowMinimizeRectForWindowDpi(hwndTarget, &rcTo);
                break;
            case SC_MAXIMIZE:
            {
GetMaximizeRect:
                CopyRect(&rcTo, &lpmmp->rcMax);
                if (!IsWindowPerMonitorDpiAware(hwndTarget))
                {
                    MONITORINFO mi = { sizeof(mi) };
                    HMONITOR hmon = MonitorFromWindow(hwndTarget, MONITOR_DEFAULTTOPRIMARY);
                    if (hmon && GetMonitorInfoW(hmon, &mi))
                    {
                        UINT dpiX = 96, dpiY = 96;
                        GetDpiForMonitor(hmon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
                        UINT uDpiWnd = GetDpiForWindow(hwndTarget);

                        UINT uWidth  = MulDiv(RECTWIDTH(rcTo), dpiX, uDpiWnd);
                        UINT uHeight = MulDiv(RECTHEIGHT(rcTo), dpiY, uDpiWnd);
                        rcTo.left   = MulDiv(rcTo.left, dpiX, uDpiWnd);
                        rcTo.top    = MulDiv(rcTo.top,  dpiY, uDpiWnd);
                        rcTo.right  = rcTo.left + uWidth;
                        rcTo.bottom = rcTo.top + uHeight;
                    }
                }
                AdjustRectForWindowBorder(hwndTarget, &rcTo);
                break;
            }
            case SC_RESTORE:
            {
                WINDOWPLACEMENT wp = { sizeof(wp) };
                GetWindowPlacement(hwndTarget, &wp);
                if ((dwStyle & WS_MINIMIZE)
                && (wp.flags & WPF_RESTORETOMAXIMIZED))
                {
                    goto GetMaximizeRect;
                }
                else
                {
                    CopyRect(&rcTo, &wp.rcNormalPosition);
                    if (!(dwStyle & WS_CHILD))
                    {
                        // If the window is not a child window (MDI), the rectangle
                        // received in rcNormalPosition after calling GetWindowPlacement
                        // has the distance from the top left of the monitor's work area
                        // to the monitor's top left edge subtracted. I don't know why they
                        // do this, and it's not referenced in any official documentation...
                        MONITORINFO mi = { sizeof(mi) };
                        HMONITOR hmon = MonitorFromWindow(hwndTarget, MONITOR_DEFAULTTOPRIMARY);
                        if (hmon && GetMonitorInfoW(hmon, &mi))
                        {
                            OffsetRect(&rcTo,
                                -(mi.rcMonitor.left - mi.rcWork.left),
                                -(mi.rcMonitor.top - mi.rcWork.top));
                        }
                    }
                    AdjustRectForWindowBorder(hwndTarget, &rcTo);
                }
                break;
            }
        }

        // Adjust child coords to screen coords (MDI)
        // For some reason only the destination rect is in child coords
        HWND hwndParent;
        HRGN hrgnClip = NULL;
        // For MDI windows, get the parent client rect for clipping
        RECT rcClip = { 0, 0, cxVirtualScreen, cyVirtualScreen }; // Default to full screen
        if ((dwStyle & WS_CHILD) && (hwndParent = GetParent(hwndTarget)))
        {
            MapWindowPoints(hwndParent, HWND_DESKTOP, (LPPOINT)&rcTo, 2);

            GetClientRect(hwndParent, &rcClip);
            MapWindowPoints(hwndParent, HWND_DESKTOP, (LPPOINT)&rcClip, 2);

            // Convert to animation window coordinates
            OffsetRect(&rcClip, -xVirtualScreen, -yVirtualScreen);

            // Create clipping region
            hrgnClip = CreateRectRgnIndirect(&rcClip);

            // Find windows above the MDI parent that intersect with it
            HWND hwndAbove = GetAncestor(hwndParent, GA_ROOT);
            while ((hwndAbove = GetWindow(hwndAbove, GW_HWNDPREV)) != NULL)
            {
                if (IsWindowVisible(hwndAbove) && hwndAbove != g_hwndAnim)
                {
                    RECT rcAbove;
                    if (GetWindowRect(hwndAbove, &rcAbove))
                    {
                        RECT rcIntersect;
                        if (IntersectRect(&rcIntersect, &rcAbove, &rcClip))
                        {
                            // This window overlaps, subtract it from clipping region
                            // Well this doesn't look great with Win10/11 fake window borders,
                            // Windows Aero transparent borders, and window shadows.
                            // But the classic min/max animations were not designed with these in mind...
                            OffsetRect(&rcAbove, -xVirtualScreen, -yVirtualScreen);
                            HRGN hrgnAbove = CreateRectRgnIndirect(&rcAbove);
                            CombineRgn(hrgnClip, hrgnClip, hrgnAbove, RGN_DIFF);
                            DeleteObject(hrgnAbove);
                        }
                    }
                }
            }
        }

        // Convert from screen coordinates to window coordinates
        OffsetRect(&rcFrom, -xVirtualScreen, -yVirtualScreen);
        OffsetRect(&rcTo,   -xVirtualScreen, -yVirtualScreen);

        /* Show window and start the animation */

        // Make window fully transparent before showing it. Otherwise, there is a chance
        // that the last frame of the previous animation (or a completely white screen
        // if no animation was played before) will show before we blit the desktop.
        SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
        ShowWindow_orig(hwnd, SW_SHOW);

        HDC hdc = GetDC(hwnd);
        // Desktop image DC:
        hdcDesk = CreateCompatibleDC(hdc);
        SelectObject(hdcDesk, hbmDesk);
        // Double buffer DC:
        hdcMem = CreateCompatibleDC(hdc);
        HBITMAP hbmMem = CreateCompatibleBitmap(hdcDesk, cxVirtualScreen, cyVirtualScreen);
        SelectObject(hdcMem, hbmMem);

        // Blit full desktop image first:
        BitBlt(hdc, 0, 0, cxVirtualScreen, cyVirtualScreen, hdcDesk, 0, 0, SRCCOPY);
        // Make window opaque after drawing the desktop
        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);

        // Then start animation:
        UINT64 uStartTime = GetTickCount64();
        UINT64 uElapsedTime = 0;
        RECT rcCurrent;
        CopyRect(&rcCurrent, &rcFrom);

        // Icon metrics:
        int cxIcon    = GetSystemMetricsForDpi(SM_CXSMICON, uDpiMon);
        int cyIcon    = GetSystemMetricsForDpi(SM_CYSMICON, uDpiMon);
        int cyCaption = GetSystemMetricsForDpi(SM_CYCAPTION, uDpiMon) - 1;

        // Offsets from the top left (or right) to the icon.
        int dxIcon = (cyCaption - cxIcon) / 2;
        int dyIcon = (cyCaption - cyIcon) / 2;
        
        // Set to caption height
        rcCurrent.bottom = rcCurrent.top + cyCaption;

        // Check if we should draw gradients on the caption
        BOOL fGradient = FALSE;
        SystemParametersInfoW(SPI_GETGRADIENTCAPTIONS, 0, &fGradient, 0);
        
        // Check if we should draw the icon and if the window is right-to-left
        PWND pwnd     = ValidateHwnd(hwndTarget);
        bool fHasIcon = _HasCaptionIcon(pwnd);
        bool fRtl     = (GetWindowLongW(hwndTarget, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) != 0;
        
        // Colors
        COLORREF clrCaption         = GetSysColor(COLOR_ACTIVECAPTION);
        COLORREF clrCaptionGradient = GetSysColor(COLOR_GRADIENTACTIVECAPTION);
        COLORREF clrCaptionText     = GetSysColor(COLOR_CAPTIONTEXT);

        // Brush used for solid fills (icon box, non-gradient caption)
        // and DrawIconEx "flicker-free brush"
        HBRUSH hbrCaption = GetSysColorBrush(COLOR_ACTIVECAPTION);

        // Caption text font
        NONCLIENTMETRICSW ncm = { sizeof(ncm) };
        SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, FALSE, uDpiMon);
        HFONT hfontCaption = CreateFontIndirectW(&ncm.lfCaptionFont);

        // Actual animation loop
        while ((uElapsedTime = GetTickCount64() - uStartTime) <= ANIMATION_DURATION_MS)
        {
            // Create a new rect using delta time:
            RECT rcOld = rcCurrent;
            rcCurrent.left   = rcFrom.left  + MulDiv(rcTo.left  - rcFrom.left,  uElapsedTime, ANIMATION_DURATION_MS);
            rcCurrent.top    = rcFrom.top   + MulDiv(rcTo.top   - rcFrom.top,   uElapsedTime, ANIMATION_DURATION_MS);
            rcCurrent.right  = rcFrom.right + MulDiv(rcTo.right - rcFrom.right, uElapsedTime, ANIMATION_DURATION_MS);
            rcCurrent.bottom = rcCurrent.top + cyCaption;

            // Combine the old and new rects to form a dirty area that needs to be
            // filled with the desktop image. We paint the desktop image over the old
            // area and the new caption bar in one blit, which lessens flickering. We
            // waste a little bit of time blitting the area not covered by the old
            // or new caption in the corners of this rect, but it's preferable to
            // the flickering that would occur without doing this.
            RECT rcDirty;
            rcDirty.left   = min(rcCurrent.left,     rcOld.left);
            rcDirty.top    = min(rcCurrent.top,       rcOld.top);
            rcDirty.right  = max(rcCurrent.right,   rcOld.right);
            rcDirty.bottom = max(rcCurrent.bottom, rcOld.bottom);

            // Paint desktop over old area:
            BitBlt(
                hdcMem,
                rcDirty.left, rcDirty.top,
                RECTWIDTH(rcDirty),
                RECTHEIGHT(rcDirty),
                hdcDesk,
                rcDirty.left, rcDirty.top,
                SRCCOPY
            );

            // Apply clipping region
            SelectClipRgn(hdcMem, hrgnClip);

            // Draw background
            if (fGradient)
            {
                // Gradient captions draw the regular caption color behind the icon,
                // the gradient caption color behind the caption buttons, and the gradient
                // in the area between those. We don't need to worry about caption buttons here,
                // since the animated caption never shows them.

                int iCaptionOffset = 0;
                if (fHasIcon)
                {
                    RECT rcIconBox;
                    rcIconBox.top = rcCurrent.top;
                    rcIconBox.bottom = rcCurrent.bottom;
                    if (fRtl)
                    {
                        rcIconBox.left = rcCurrent.right - cyCaption;
                        rcIconBox.right = rcCurrent.right;
                    }
                    else
                    {
                        rcIconBox.left = rcCurrent.left;
                        rcIconBox.right = rcCurrent.left + cyCaption;
                    }

                    iCaptionOffset = RECTWIDTH(rcIconBox);
                    FillRect(hdcMem, &rcIconBox, hbrCaption);
                }

                RECT rcGradient = rcCurrent;
                if (fRtl)
                    rcGradient.right -= iCaptionOffset;
                else
                    rcGradient.left += iCaptionOffset;

                // If the window is RTL, we need to draw the gradient
                // from the right, so just flip the colors.
                COLORREF clrLeft  = fRtl ? clrCaptionGradient : clrCaption;
                COLORREF clrRight = fRtl ? clrCaption : clrCaptionGradient;

                // TRIVERTEX structs store colors in a higher resolution than
                // standard 32-bit RGB, so we must multiply the color values
                // by 256 (shift left by 8) in order for them to show up right.
                TRIVERTEX vertices[2];
                vertices[0].x     = rcGradient.left;
                vertices[0].y     = rcGradient.top;
                vertices[0].Red   = GetRValue(clrLeft) << 8;
                vertices[0].Green = GetGValue(clrLeft) << 8;
                vertices[0].Blue  = GetBValue(clrLeft) << 8;
                vertices[0].Alpha = 0;
                vertices[1].x     = rcGradient.right;
                vertices[1].y     = rcGradient.bottom;
                vertices[1].Red   = GetRValue(clrRight) << 8;
                vertices[1].Green = GetGValue(clrRight) << 8;
                vertices[1].Blue  = GetBValue(clrRight) << 8;
                vertices[1].Alpha = 0;

                GRADIENT_RECT mesh = { 1, 0 };
                GdiGradientFill(hdcMem, vertices, ARRAYSIZE(vertices), &mesh, 1, GRADIENT_FILL_RECT_H);
            }
            else
            {
                FillRect(hdcMem, &rcCurrent, hbrCaption);
            }

            // Draw icon
            if (fHasIcon)
            {
                int xIcon;
                if (fRtl)
                    xIcon = rcCurrent.right - dxIcon - cxIcon;
                else
                    xIcon = rcCurrent.left + dxIcon;
                int yIcon = rcCurrent.top + dyIcon;

                DrawIconEx(
                    hdcMem,
                    xIcon, yIcon,
                    lpmmp->hIcon,
                    cxIcon, cyIcon,
                    0,
                    hbrCaption,
                    DI_NORMAL
                );
            }

            // Draw caption text
            if (lpmmp->szCaptionText[0])
            {
                RECT rcText;
                CopyRect(&rcText, &rcCurrent);
                
                if (fRtl)
                    rcText.right -= 1 + (fHasIcon ? cyCaption : 0);
                else
                    rcText.left  += 1 + (fHasIcon ? cyCaption : 0);
                
                HFONT hfontOld = (HFONT)SelectObject(hdcMem, hfontCaption);
                COLORREF clrOld = SetTextColor(hdcMem, clrCaptionText);
                int iBkOld = SetBkMode(hdcMem, TRANSPARENT);
                UINT uFormat = DT_SINGLELINE | DT_VCENTER;
                if (fRtl)
                    uFormat |= DT_RIGHT;

                DrawTextW(
                    hdcMem,
                    lpmmp->szCaptionText, -1,
                    &rcText, uFormat
                );

                SelectObject(hdcMem, hfontOld);
                SetTextColor(hdcMem, clrOld);
                SetBkMode(hdcMem, iBkOld);
            }

            // Remove clipping region
            SelectClipRgn(hdcMem, NULL);

            // Then blit it all to the main DC.
            BitBlt(
                hdc,
                rcDirty.left, rcDirty.top,
                RECTWIDTH(rcDirty), RECTHEIGHT(rcDirty),
                hdcMem,
                rcDirty.left, rcDirty.top,
                SRCCOPY
            );
        }

        /* Clean up and hide animation window */
        DeleteObject(hrgnClip);
        DeleteDC(hdcDesk);
        DeleteDC(hdcMem);
        DeleteObject(hbmMem);
        DeleteObject(hbmDesk);
        DeleteObject(hfontCaption);
        if (lpmmp->hIcon)
        {
            DestroyIcon(lpmmp->hIcon);
            lpmmp->hIcon = NULL;
        }
        GlobalFree(lpmmp);

        // We must set the animation window to non-topmost before returning, or else
        // the window being animated may erroneously gain topmost status.
        SetWindowPos(
            hwnd,
            HWND_BOTTOM,
            0, 0, 0, 0,
            SWP_NOACTIVATE
        );

        ShowWindow_orig(hwnd, SW_HIDE);
        g_fAnimating = false;

        return 0;
    }

    switch (uMsg)
    {
        case WM_PAINT:
        case WM_ERASEBKGND:
            return 0;
        // Make click-thru
        case WM_NCHITTEST:
            return HTNOWHERE;
        // So that we can destroy the window from another thread.
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        // Ensure the animation thread stops after mod uninitialization.
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProcW_orig(hwnd, uMsg, wParam, lParam);
    }
}

static LPCWSTR c_szAnimClassName = L"Windhawk_UAH_MinMax";

DWORD CALLBACK AnimWndThreadProc(HANDLE hEvent)
{
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = AnimWndProc;
    wc.hInstance = g_hinst;
    wc.lpszClassName = c_szAnimClassName;
    RegisterClassW(&wc);

    g_hwndAnim = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        c_szAnimClassName,  nullptr,
        WS_VISIBLE | WS_POPUP,
        0, 0, 0, 0,
        NULL, NULL, g_hinst, NULL
    );

    // Setting these styles on window creation results in the window
    // being completely invisible, rather than click through.
    DWORD dwExStyle = GetWindowLongPtrW(g_hwndAnim, GWL_EXSTYLE);
    dwExStyle |= WS_EX_LAYERED | WS_EX_TRANSPARENT;
    SetWindowLongPtrW(g_hwndAnim, GWL_EXSTYLE, dwExStyle);

    g_uMsgMinMax = RegisterWindowMessageW(L"UAH_MinMax");

    // Animation is ready. Tell the window that needs to be animated
    // it is.
    SetEvent(hEvent);

    MSG msg;
    while (GetMessageW(&msg, g_hwndAnim, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    ExitThread(0);
    return 0;
}

#pragma endregion // "Animation implementation"

HMODULE GetCurrentModule()
{
	HMODULE hModule = NULL;
	GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
						| GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
						(LPCWSTR)GetCurrentModule, &hModule);
	return hModule;
}

const WindhawkUtils::SYMBOL_HOOK user32DllHooks[] = {
    {
        {
#ifdef _WIN64
            L"GetWindowBordersForDpi"
#else
            L"_GetWindowBordersForDpi@20"
#endif
        },
        &GetWindowBordersForDpi,
        nullptr,
        false
    },
    {
        {
            L"struct tagWND * "
            FASTCALL_STR
            L" ValidateHwnd(struct HWND__ *)"
        },
        &ValidateHwnd,
        nullptr,
        false
    },
    {
        {
#ifdef _WIN64
            L"_HasCaptionIcon"
#else
            L"__HasCaptionIcon@8"
#endif
        },
        &_HasCaptionIcon,
        nullptr,
        false
    },
};

VS_FIXEDFILEINFO *GetModuleVersionInfo(HMODULE hModule, UINT *puPtrLen) 
{ 
    void *pFixedFileInfo = nullptr; 
    UINT uPtrLen = 0; 

    HRSRC hResource = 
        FindResourceW(hModule, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION); 
    if (hResource)
    { 
        HGLOBAL hGlobal = LoadResource(hModule, hResource); 
        if (hGlobal)
        { 
            void *pData = LockResource(hGlobal); 
            if (pData)
            { 
                if (!VerQueryValueW(pData, L"\\", &pFixedFileInfo, &uPtrLen)
                || uPtrLen == 0)
                { 
                    pFixedFileInfo = nullptr; 
                    uPtrLen = 0; 
                } 
            } 
        } 
    } 

    if (puPtrLen)
    { 
        *puPtrLen = uPtrLen; 
    } 
  
    return (VS_FIXEDFILEINFO *)pFixedFileInfo; 
} 

BOOL Wh_ModInit(void)
{
    /* Disable window animations */
    ANIMATIONINFO ai;
    ai.cbSize = sizeof(ai);
    ai.iMinAnimate = FALSE;
    SystemParametersInfoW(SPI_SETANIMATION, sizeof(ai), &ai, TRUE);

    HMODULE hmUser = GetModuleHandleW(L"user32.dll");
    if (!hmUser)
    {
        Wh_Log(L"Failed to get handle to user32.dll");
        return FALSE;
    }

    GetWindowMinimizeRect = (GetWindowMinimizeRect_t)GetProcAddress(hmUser, "GetWindowMinimizeRect");
    if (!GetWindowMinimizeRect)
    {
        Wh_Log(L"Failed to get address of GetWindowMinimzeRect");
        return FALSE;
    }

    InternalGetWindowIcon = (InternalGetWindowIcon_t)GetProcAddress(hmUser, "InternalGetWindowIcon");
    if (!InternalGetWindowIcon)
    {
        Wh_Log(L"Failed to get address of InternalGetWindowIcon");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hmUser,
        user32DllHooks,
        ARRAYSIZE(user32DllHooks)
    ))
    {
        Wh_Log(L"Failed to find one or more symbols in user32.dll");
        return FALSE;
    }

#define HOOK(func)                                                                         \
    if (!Wh_SetFunctionHook((void *)func, (void *)func ## _hook, (void **)&func ## _orig)) \
    {                                                                                      \
        Wh_Log(L"Failed to hook %s", L ## #func);                                          \
        return FALSE;                                                                      \
    }

#define HOOK_A_W(func) HOOK(func ## A) HOOK(func ## W)

    HOOK_A_W(DefWindowProc)
    HOOK_A_W(DefFrameProc)
    HOOK_A_W(DefMDIChildProc)
    HOOK_A_W(DefDlgProc)
    HOOK(ShowWindow)
    HOOK(ShowWindowAsync)
    HOOK_A_W(SystemParametersInfo)
    
    /**
      * There is a nasty bug where certain windows will gain the WS_EX_TOPMOST exstyle after
      * the restore/minimize animations complete. It can be mostly mitigated by making the
      * animation window non-topmost after we're done displaying the animation, but the console
      * host is still affected.
      *
      * As well, the bug only seems to occur with Windows 10/11's Explorer. Something is bad in
      * the Windows 10/11 shell implementation specifically, because this bug *does* occur with
      * ep_taskbar, and it does *not* occur with the ExplorerEx DLL build (which runs the Immersive
      * shell as well as piggybacking off 10/11's explorer.exe like ep_taskbar, so we can wipe those
      * out as potential causes).
      *
      * A gross hack that seems to combat this bug pretty well is to hook SetWindowPos to wait
      * for 1 millisecond before calling the original function. Despite how miniscule this delay
      * is, I don't want to do this system wide, so we check the following before hooking SWP:
      *  - The current process is `%SystemRoot%\explorer.exe`.
      *  - The file version is 10.0 with a build number that is NOT 10011 (this is used by the ExplorerEX EXE build).
      *  - There is no DLL named "ExplorerEx.dll" loaded (bug does not occur on ExplorerEx DLL)
      */

    if (Wh_GetIntSetting(L"topmost_bug_mitigation"))
    {
        // Check EXE path
        WCHAR szExplorerPath[MAX_PATH], szModulePath[MAX_PATH];
        ExpandEnvironmentStringsW(L"%SystemRoot%\\explorer.exe", szExplorerPath, ARRAYSIZE(szExplorerPath));
        GetModuleFileNameW(GetModuleHandleW(NULL), szModulePath, ARRAYSIZE(szModulePath));
        if (!wcsicmp(szModulePath, szExplorerPath))
        {   
            // Check file version
            VS_FIXEDFILEINFO *pVerInfo = GetModuleVersionInfo(GetModuleHandleW(NULL), nullptr);
            if (pVerInfo)
            {
                WORD wMajor = HIWORD(pVerInfo->dwFileVersionMS);
                WORD wMinor = LOWORD(pVerInfo->dwFileVersionMS);
                WORD wBuild = HIWORD(pVerInfo->dwFileVersionLS);
                // 10011 == ExplorerEx EXE version build number
                if (wMajor == 10 && wMinor == 0 && wBuild != 10011)
                {
                    // Check for presence of ExplorerEx DLL (bug does not occur with ExplorerEx)
                    HMODULE hmodTemp;
                    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, L"ExplorerEx.dll", &hmodTemp)
                    || !hmodTemp)
                    {
                        // Finally, hook SWP
                        HOOK(SetWindowPos)
                    }
                }
            }
        }
    }

    g_hinst = GetCurrentModule();

    return TRUE;
}

void Wh_ModUninit(void)
{
    /* Re-enable animations for the user if they want them. */
    if (IsMinAnimate())
    {
        ANIMATIONINFO ai;
        ai.cbSize = sizeof(ai);
        ai.iMinAnimate = TRUE;
        SystemParametersInfoW(SPI_SETANIMATION, sizeof(ai), &ai, TRUE);
    }

    if (g_hwndAnim && IsWindow(g_hwndAnim))
    {
        SendMessageW(g_hwndAnim, WM_CLOSE, 0, 0);
        UnregisterClassW(c_szAnimClassName, g_hinst);
    }
}
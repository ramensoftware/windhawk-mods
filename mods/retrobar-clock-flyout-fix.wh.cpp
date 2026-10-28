// ==WindhawkMod==
// @id              retrobar-clock-flyout-fix
// @name            RetroBar Clock Flyout Fix
// @description     This mod fixes the Aero clock flyout opened from RetroBar: thick border and floating position
// @version         1.0.0
// @author          babamohammed2022
// @github          https://github.com/babamohammed2022
// @include         RetroBar.exe
// @architecture    x86-64
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# RetroBar Clock Flyout Fix

This mod fixes the borders of the clock flyout to restore the Aero borders. It is made specifically
for [RetroBar](https://github.com/dremin/RetroBar)'s Aero (Windows Vista/7 style) clock/calendar flyout.
This modification has been tested on Windows 10 21H2.

## Screenshots

### Before

![Before](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/brokenclock.PNG)

### After

![After](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/fixedclock.PNG)

When *Clock click action* is set to the Aero calendar, RetroBar creates the flyout **inside its own
process** (it instantiates the COM class from `timedate.cpl` and calls `ShowFlyout`), then nudges
the window next to the taskbar. This mod therefore runs in `RetroBar.exe` and:

* It gives the flyout the thick Aero frame and keeps it non-resizable;
* It keeps it floating away from the work area edges, also after RetroBar repositions it.

On Windows 10 the Win32 clock flyout must be available
(`HKLM\Software\Microsoft\Windows\CurrentVersion\ImmersiveShell\UseWin32TrayClockExperience` = 1).

## Credits & Reference

* [RetroBar](https://github.com/dremin/RetroBar) by dremin — the taskbar that hosts the Aero clock flyout.
* [Aero Flyout Fix](https://windhawk.net/mods/aero-flyout-fix) by aubymori — `timedate.cpl` hooks and flyout positioning logic.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- offset: 8
  $name: Flyout offset
  $description: This setting modifies the distance, in pixels at 96 DPI, between the flyout and the edges of the work area.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

int  g_offset = 8;
HWND g_hWndFlyout = NULL;
HMODULE g_hTimedate = NULL;

void LoadSettings(void)
{
    g_offset = Wh_GetIntSetting(L"offset");
    if (g_offset < 0)
        g_offset = 0;
}

void ApplyFlyoutStyles(HWND hWnd)
{
    SetWindowLongPtrW(
        hWnd, GWL_STYLE, GetWindowLongPtrW(hWnd, GWL_STYLE) | WS_THICKFRAME
    );
    SetWindowLongPtrW(
        hWnd, GWL_EXSTYLE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW
    );
}

/* Push a rectangle away from the work area edges so it floats. */
void FloatRect(HWND hWnd, int *pX, int *pY, int cx, int cy)
{
    RECT rc = { *pX, *pY, *pX + cx, *pY + cy };

    MONITORINFO mi = { sizeof(MONITORINFO) };
    if (!GetMonitorInfoW(MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST), &mi))
        return;

    UINT dpi = GetDpiForWindow(hWnd);
    if (!dpi)
        dpi = 96;
    int offset = MulDiv(g_offset, dpi, 96);

    int dx = 0, dy = 0;
    PLONG plrc  = (PLONG)&rc;
    PLONG plwrc = (PLONG)&mi.rcWork;
    for (int i = 0; i < 4; i++)
    {
        int cur = plwrc[i] - plrc[i];
        cur = (cur < 0) ? -cur : cur;
        if (cur < offset)
        {
            int *set = (i % 2 == 0) ? &dx : &dy;
            if (i > 1)
                *set -= offset - cur;
            else
                *set += offset - cur;
        }
    }
    *pX += dx;
    *pY += dy;
}

#pragma region "timedate.cpl hooks"

/* Give the clock flyout the proper styles upon creation */
HWND (*IsolationAwareCreateWindowExW_orig)(__int64, LPCWSTR, __int64, DWORD, int, int, int, int, HWND);
HWND IsolationAwareCreateWindowExW_hook(
    __int64 i1,
    LPCWSTR lpClassName,
    __int64 i2,
    DWORD   dwStyle,
    int     X,
    int     Y,
    int     nWidth,
    int     nHeight,
    HWND    hWndParent
)
{
    HWND hWnd = IsolationAwareCreateWindowExW_orig(
        i1, lpClassName, i2, dwStyle, X, Y,
        nWidth, nHeight, hWndParent
    );

    if (hWnd
    && ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0
    && 0 == wcscmp(lpClassName, L"ClockFlyoutWindow"))
    {
        g_hWndFlyout = hWnd;
        ApplyFlyoutStyles(hWnd);
    }

    return hWnd;
}

/* Make the flyout not resizable and keep it floating */
WNDPROC CTrayClock_s_WndProc_orig;
LRESULT CALLBACK CTrayClock_s_WndProc_hook(
    HWND   hWnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    if (hWnd == g_hWndFlyout)
    {
        if (uMsg == WM_WINDOWPOSCHANGING && lParam)
        {
            WINDOWPOS *pwp = (WINDOWPOS *)lParam;
            if (!(pwp->flags & SWP_NOMOVE))
            {
                int cx = pwp->cx, cy = pwp->cy;
                if (pwp->flags & SWP_NOSIZE)
                {
                    RECT rc;
                    GetWindowRect(hWnd, &rc);
                    cx = rc.right - rc.left;
                    cy = rc.bottom - rc.top;
                }
                FloatRect(hWnd, &pwp->x, &pwp->y, cx, cy);
            }
        }
        else if (uMsg == WM_NCHITTEST)
        {
            LRESULT lr = CTrayClock_s_WndProc_orig(hWnd, uMsg, wParam, lParam);
            switch (lr)
            {
                case HTTOP:
                case HTTOPRIGHT:
                case HTRIGHT:
                case HTBOTTOMRIGHT:
                case HTBOTTOM:
                case HTBOTTOMLEFT:
                case HTLEFT:
                case HTTOPLEFT:
                    return HTBORDER;
                default:
                    return lr;
            }
        }
    }

    return CTrayClock_s_WndProc_orig(hWnd, uMsg, wParam, lParam);
}

const WindhawkUtils::SYMBOL_HOOK timedateCplHooks[] = {
    {
        { L"IsolationAwareCreateWindowExW" },
        &IsolationAwareCreateWindowExW_orig,
        IsolationAwareCreateWindowExW_hook,
        false
    },
    {
        { L"private: static __int64 __cdecl CTrayClock::s_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)" },
        &CTrayClock_s_WndProc_orig,
        CTrayClock_s_WndProc_hook,
        false
    }
};

#pragma endregion // "timedate.cpl hooks"

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
}

BOOL Wh_ModInit(void)
{
    LoadSettings();

    /* Load from System32 only: RetroBar is portable and may run from any
       folder, so a planted timedate.cpl next to RetroBar.exe must not be
       loaded. COM resolves the class from the registered System32 path, so
       restricting the search here also ensures the hooks attach to the same
       module the flyout will be created from. */
    HMODULE hTimedate = LoadLibraryExW(
        L"timedate.cpl", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32
    );
    if (!hTimedate)
    {
        Wh_Log(L"Failed to load timedate.cpl");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hTimedate,
        timedateCplHooks,
        ARRAYSIZE(timedateCplHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in timedate.cpl");
        FreeLibrary(hTimedate);
        return FALSE;
    }

    g_hTimedate = hTimedate;
    return TRUE;
}

void Wh_ModUninit(void)
{
    if (g_hTimedate)
    {
        FreeLibrary(g_hTimedate);
        g_hTimedate = NULL;
    }
    g_hWndFlyout = NULL;
}

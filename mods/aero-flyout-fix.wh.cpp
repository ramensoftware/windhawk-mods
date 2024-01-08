// ==WindhawkMod==
// @id              aero-flyout-fix
// @name            Aero Flyout Fix
// @description     Applies thick borders to legacy system flyouts and make them float
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @include         SndVol.exe
// @architecture    x86-64
// @compilerOptions -lgdi32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Aero Flyout Fix
Since Windows 10, many of the legacy tray flyouts either don't have thick borders or don't
float away from the taskbar. This mod adds back thick borders and makes flyouts float where
applicable.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/aero-flyout-fix-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/main/aero-flyout-fix-after.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <winerror.h>

#define RECTWIDTH(rect)  ((rect).right - (rect).left)
#define RECTHEIGHT(rect) ((rect).bottom - (rect).top)

const UINT FLYOUT_OFFSET = 8;

/* Adjust a window's position to be pushed away from the taskbar */
POINT AdjustWindowPosForTaskbar(HWND hWnd)
{
    HMONITOR hm = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    HDC hDC = GetDC(hWnd);
    int offset = MulDiv(FLYOUT_OFFSET, GetDeviceCaps(hDC, LOGPIXELSY), 96);

    RECT rc;
    GetWindowRect(hWnd, &rc);

    MONITORINFO mi = { sizeof(MONITORINFO) };
    GetMonitorInfoW(hm, &mi);

    int dx = 0, dy = 0;
    PLONG plrc = (PLONG)&rc;
    PLONG plwrc = (PLONG)&mi.rcWork;
    for (int i = 0; i < 4; i++)
    {
        int curOffset = plwrc[i] - plrc[i];
        curOffset = (curOffset < 0) ? -curOffset : curOffset;

        if (curOffset < offset)
        {
            int *set = (i % 2 == 0) ? &dx : &dy;
            if (i > 1)
            {
                *set -= offset - curOffset;
            }
            else
            {
                *set += offset - curOffset;
            }
        }
    }
    return { rc.left + dx, rc.top + dy };
}

#pragma region "Explorer hooks"

#pragma region "timedate.cpl hooks"

#define CTrayClock_Window(pThis) *((HWND *)pThis + 2)

/* Give clock the proper styles upon creation */
HWND (* IsolationAwareCreateWindowExW_orig)(__int64, LPCWSTR, __int64, DWORD, int, int, int, int, HWND);
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
        SetWindowLongPtrW(
            hWnd, GWL_STYLE, GetWindowLongPtrW(hWnd, GWL_STYLE) | WS_THICKFRAME
        );
        SetWindowLongPtrW(
            hWnd, GWL_EXSTYLE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW
        );
    }
    return hWnd;
}

/* Make the clock not resizable */
WNDPROC CTrayClock_s_WndProc_orig;
LRESULT CALLBACK CTrayClock_s_WndProc_hook(
    HWND   hWnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    if (uMsg == WM_NCHITTEST)
    {
        LRESULT lr = CTrayClock_s_WndProc_orig(
            hWnd, uMsg, wParam, lParam
        );
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
                return HTCLIENT;
            default:
                return lr;
        }
    }

    return CTrayClock_s_WndProc_orig(
        hWnd, uMsg, wParam, lParam
    );
}

const WindhawkUtils::SYMBOL_HOOK timedateHooks[] = {
    {
        {
            L"IsolationAwareCreateWindowExW"
        },
        &IsolationAwareCreateWindowExW_orig,
        IsolationAwareCreateWindowExW_hook,
        false
    },
    {
        {
            L"private: static __int64 __cdecl CTrayClock::s_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
        },
        &CTrayClock_s_WndProc_orig,
        CTrayClock_s_WndProc_hook,
        false
    }
};
#pragma endregion // "timedate.cpl hooks"

#pragma region "user32.dll hooks"

HWND g_hWndFlyout = NULL;

typedef HWND (WINAPI *CreateWindowInBand_t)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID, __int64, __int64, __int64, __int64);
CreateWindowInBand_t CreateWindowInBand_orig;
HWND WINAPI CreateWindowInBand_hook(
    DWORD     dwExStyle,
    LPCWSTR   lpClassName,
    LPCWSTR   lpWindowName,
    DWORD     dwStyle,
    int       X,
    int       Y,
    int       nWidth,
    int       nHeight,
    HWND      hWndParent,
    HMENU     hMenu,
    HINSTANCE hInstance,
    LPVOID    lpParam,
    __int64   i1,
    __int64   i2,
    __int64   i3,
    __int64   i4
)
{
    HWND hWnd = CreateWindowInBand_orig(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y,
        nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam,
        i1, i2, i3, i4
    );

    if (((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0)
    {
        if (0 == wcscmp(lpClassName, L"BatMeterFlyout"))
        {
            g_hWndFlyout = hWnd;
        }
        else if (0 == wcscmp(lpClassName, L"WHCFlyoutWindow"))
        {
            SetWindowLongPtrW(
                hWnd, GWL_STYLE, GetWindowLongPtrW(hWnd, GWL_STYLE) | WS_THICKFRAME
            );
            SetWindowLongPtrW(
                hWnd, GWL_EXSTYLE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE) | WS_EX_TOOLWINDOW
            );
        }
    }

    return hWnd;
}

#pragma endregion // "user32.dll hooks"

#pragma region "stobject.dll hooks"

void (* UpdateFlyoutUI_orig)(void);
void UpdateFlyoutUI_hook(void)
{
    UpdateFlyoutUI_orig();
    if (g_hWndFlyout)
    {
        SetWindowLongPtrW(
            g_hWndFlyout, GWL_STYLE, GetWindowLongPtrW(g_hWndFlyout, GWL_STYLE) | WS_THICKFRAME
        );
        SetWindowLongPtrW(
            g_hWndFlyout, GWL_EXSTYLE, GetWindowLongPtrW(g_hWndFlyout, GWL_EXSTYLE) | WS_EX_TOOLWINDOW
        );
    }
}

const WindhawkUtils::SYMBOL_HOOK stobjectHooks[] = {
    {
        {
            L"void __cdecl UpdateFlyoutUI(void)"
        },
        &UpdateFlyoutUI_orig,
        UpdateFlyoutUI_hook,
        false
    }
};
#pragma endregion // "stobject.dll hooks"

#pragma region "ActionCenter.dll hooks"

WNDPROC CHCFlyoutWindow_CHCFlyoutSTAThread_s_WndProc_orig;
LRESULT CALLBACK CHCFlyoutWindow_CHCFlyoutSTAThread_s_WndProc_hook(
    HWND   hWnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    if (uMsg == WM_NCHITTEST)
    {
        LRESULT lr = CHCFlyoutWindow_CHCFlyoutSTAThread_s_WndProc_orig(
            hWnd, uMsg, wParam, lParam
        );
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
                return HTCLIENT;
            default:
                return lr;
        }
    }

    return CHCFlyoutWindow_CHCFlyoutSTAThread_s_WndProc_orig(
        hWnd, uMsg, wParam, lParam
    );
}

const WindhawkUtils::SYMBOL_HOOK actioncenterHooks[] = {
    {
        {
            L"private: static __int64 __cdecl CHCFlyoutWindow::CHCFlyoutSTAThread::s_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
        },
        &CHCFlyoutWindow_CHCFlyoutSTAThread_s_WndProc_orig,
        CHCFlyoutWindow_CHCFlyoutSTAThread_s_WndProc_hook,
        false
    }
};

#pragma endregion // "ActionCenter.dll hooks"

BOOL Wh_ModInit_Explorer(void)
{
    HMODULE hTimedate = LoadLibraryW(L"timedate.cpl");
    if (!hTimedate)
    {
        Wh_Log(L"Failed to load timedate.cpl");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hTimedate,
        timedateHooks,
        ARRAYSIZE(timedateHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in timedate.cpl");
        return FALSE;
    }

    HMODULE hUser32 = LoadLibraryW(L"user32.dll");
    if (!hUser32)
    {
        Wh_Log(L"Failed to load user32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::Wh_SetFunctionHookT(
        (CreateWindowInBand_t)GetProcAddress(hUser32, "CreateWindowInBand"),
        CreateWindowInBand_hook,
        &CreateWindowInBand_orig
    ))
    {
        Wh_Log(L"Failed to hook CreateWindowInBand in user32.dll");
        return FALSE;
    }

    HMODULE hStobject = LoadLibraryW(L"stobject.dll");
    if (!hStobject)
    {
        Wh_Log(L"Failed to load stobject.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hStobject,
        stobjectHooks,
        ARRAYSIZE(stobjectHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in stobject.dll");
        return FALSE;
    }

    /* Attempt to late capture the battery flyout */
    g_hWndFlyout = FindWindowW(L"BatMeterFlyout", NULL);

    /**
      * It is safe to skip hooking ActionCenter.dll since the flyout is
      * brought back via replacing it with Windows 8.1's version.
      */
    HMODULE hActioncenter = LoadLibraryW(L"ActionCenter.dll");
    if (hActioncenter)
    {
        if (!WindhawkUtils::HookSymbols(
            hActioncenter,
            actioncenterHooks,
            ARRAYSIZE(actioncenterHooks)
        ))
        {
            Wh_Log(L"Failed to hook one or more symbol functions in ActionCenter.dll");
        }
    }

    return TRUE;
}

#pragma endregion // "Explorer hooks"

#pragma region "SndVol.exe hooks"

#define CDlgSimpleVolumeHost_Window(pThis) *(HWND *)((char *)pThis + 8)

/* Position volume flyout upon creation */
BOOL (* CDlgSimpleVolumeHost_OnInitDialog_orig)(void *);
BOOL CDlgSimpleVolumeHost_OnInitDialog_hook(
    void *pThis
) 
{
    BOOL bRes = CDlgSimpleVolumeHost_OnInitDialog_orig(pThis);

    HWND hDlg = CDlgSimpleVolumeHost_Window(pThis);
    if (hDlg)
    {
        POINT pt = AdjustWindowPosForTaskbar(hDlg);
        SetWindowPos(
            hDlg, NULL,
            pt.x, pt.y,
            0, 0,
            SWP_NOSIZE | SWP_NOZORDER
        );
    }

    return bRes;
}

const WindhawkUtils::SYMBOL_HOOK sndvolHooks[] = {
    {
        {
            L"public: __int64 __cdecl CDlgSimpleVolumeHost::OnInitDialog(unsigned int,unsigned __int64,__int64,int &)"
        },
        &CDlgSimpleVolumeHost_OnInitDialog_orig,
        CDlgSimpleVolumeHost_OnInitDialog_hook,
        false
    }
};

BOOL Wh_ModInit_SndVol(void)
{
    if (!WindhawkUtils::HookSymbols(
        GetModuleHandleW(NULL),
        sndvolHooks,
        ARRAYSIZE(sndvolHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in SndVol.exe");
        return FALSE;
    }
    
    return TRUE;
}

#pragma endregion // "SndVol.exe hooks"

BOOL Wh_ModInit(void)
{
    WCHAR szPath[MAX_PATH];
    GetModuleFileNameW(GetModuleHandleW(NULL), szPath, MAX_PATH);
    LPWSTR pszName = wcsrchr(szPath, L'\\');
    if (pszName)
    {
        pszName++;
        if (!wcsicmp(pszName, L"sndvol.exe"))
        {
            return Wh_ModInit_SndVol();
        }
        else if (!wcsicmp(pszName, L"explorer.exe"))
        {
            return Wh_ModInit_Explorer();
        }
        else
        {
            Wh_Log(L"Aero Flyout Fix cannot hook into this program");
            return FALSE;
        }
    }

    Wh_Log(L"Aero Flyout Fix cannot hook into this program");
    return FALSE;
}
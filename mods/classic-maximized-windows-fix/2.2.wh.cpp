// ==WindhawkMod==
// @id              classic-maximized-windows-fix
// @name            Fix Classic Theme Maximized Windows
// @description     Fix maximized windows having borders that spill out onto additional displays when using the classic theme.
// @version         2.2
// @author          ephemeralViolette
// @github          https://github.com/ephemeralViolette
// @include         *

// @exclude         windhawk.exe

// @exclude         conhost.exe
// @exclude         consent.exe
// @exclude         dwm.exe
// @exclude         lsass.exe
// @exclude         winlogon.exe
// @exclude         logonui.exe
// @exclude         ApplicationFrameHost.exe
// @exclude         gameinputsvc.exe
// @exclude         svchost.exe

// @compilerOptions -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Maximized Windows Fix

Fixes the window borders on classic theme windows when DWM is active from spilling over to other monitors.

It occurred with DWM on because DWM is only compatible with manual window clipping via SetWindowRgn, like
UxTheme uses, and not the special override that Win32k applies for rendering maximized windows with default
(classic) frames. This simply makes all windows use classic theme frames.

Command Prompt windows will still not be properly clipped, however this issue already happened on older
versions of Windows. Technically, there is a timing issue with Windhawk injecting into console windows,
so rarely they may be affected. For consistency, I manually excluded `conhost.exe`.

## Known problems

- This will not work correctly without the classic theme. Whenever a window would be maximized, its frame
  would change to the classic frames (this is a special behaviour implemented in UxTheme for compatibility
  with historical use of this function), and hittesting would not work correctly on the new classic frames,
  which would interfere with UX quite a lot I think.
*/
// ==/WindhawkModReadme==

#include <vector>
#include <algorithm>
#include <synchapi.h>
#include <windhawk_utils.h>

CRITICAL_SECTION g_criticalSection;
std::vector<HWND> g_affectedWindows;
std::vector<HWND> g_fakeUndoWindows;

// === DPI HELPERS === //
// These are only available in Windows 10, version 1607 and newer.
unsigned int (WINAPI *g_pGetDpiForWindow)(HWND);
int (WINAPI *g_pGetSystemMetricsForDpi)(int, unsigned int);

/*
 * GetSystemMetricsForWindow: Get the system metrics for a given window.
 *
 * This function is per-monitor DPI aware when available.
 */
int GetSystemMetricsForWindow(HWND hWnd, int nIndex)
{
    if (g_pGetDpiForWindow && g_pGetSystemMetricsForDpi)
    {
        unsigned int dpi = g_pGetDpiForWindow(hWnd);
        return g_pGetSystemMetricsForDpi(nIndex, dpi);
    }
    else
    {
        return GetSystemMetrics(nIndex);
    }
}

/*
 * ApplyWindowMasking: Applies a mask to the window to clip the borders drawn
 *                     by the window manager.
 */
LRESULT ApplyWindowMasking(HWND hWnd)
{
    BOOL canLock = TryEnterCriticalSection(&g_criticalSection);
    int sizeBorders =
        GetSystemMetricsForWindow(hWnd, SM_CXSIZEFRAME) +
        GetSystemMetricsForWindow(hWnd, SM_CXPADDEDBORDER);
    RECT rcWindow;
    HRGN hRgn = NULL;

    if (canLock)
    {
        GetWindowRect(hWnd, &rcWindow);

        int cxWindow = rcWindow.right - rcWindow.left;
        int cyWindow = rcWindow.bottom - rcWindow.top;

        // SetWindowRgn transfers ownership of the HRGN to the operating system, so it's not
        // our responsibility to clean.
        hRgn = CreateRectRgn(
            0 + sizeBorders, 0 + sizeBorders,
            cxWindow - sizeBorders, cyWindow - sizeBorders
        );

        g_affectedWindows.push_back(hWnd);
    
        // If the window is marked as "fake undo", then remove that flag.
        std::vector<HWND>::iterator i;
        if ((i = std::find(g_fakeUndoWindows.begin(), g_fakeUndoWindows.end(), hWnd)) != g_fakeUndoWindows.end())
        {
            std::vector<HWND>::iterator newEnd = std::remove(g_fakeUndoWindows.begin(), g_fakeUndoWindows.end(), hWnd);
            g_fakeUndoWindows.erase(newEnd, g_fakeUndoWindows.end());
        }
            
        LeaveCriticalSection(&g_criticalSection);
    }

    if (hRgn != NULL && SetWindowRgn(hWnd, hRgn, TRUE))
    {
        return S_OK;
    }

    return E_FAIL;
}

/*
 * FakeUndoWindowMasking: Fakes removing the window mask by applying a massive one
 *                        over the window.
 *
 * This is required in order to fix a small bug with Aero Snap, where removing the
 * mask properly breaks the snap positioning for the first time the window is snapped
 * after maximising (unless the window is resized).
 *
 * TODO: Find a better solution. Needless to say, this is pretty ugly and it would be
 * preferable that a mask isn't leftover at all.
 */
LRESULT FakeUndoWindowMasking(HWND hWnd)
{
    std::vector<HWND>::iterator i;

    BOOL canLock = TryEnterCriticalSection(&g_criticalSection);

    if (canLock)
    {
        if ((i = std::find(g_affectedWindows.begin(), g_affectedWindows.end(), hWnd)) != g_affectedWindows.end())
        {
            std::vector<HWND>::iterator newEnd = std::remove(g_affectedWindows.begin(), g_affectedWindows.end(), hWnd);
            g_affectedWindows.erase(newEnd, g_affectedWindows.end());
            g_fakeUndoWindows.push_back(hWnd);

            HRGN hRgn = CreateRectRgn(
                0, 0,
                99999, 99999
            );

            SetWindowRgn(hWnd, hRgn, TRUE);
        }

        LeaveCriticalSection(&g_criticalSection);

        return S_OK;
    }

    return E_FAIL;
}

/*
 * UndoWindowMasking: Removes the masking region set by ApplyWindowMasking.
 */
LRESULT UndoWindowMasking(HWND hWnd)
{
    std::vector<HWND>::iterator i;

    BOOL canLock = TryEnterCriticalSection(&g_criticalSection);

    if (canLock)
    {
        if ((i = std::find(g_affectedWindows.begin(), g_affectedWindows.end(), hWnd)) != g_affectedWindows.end())
        {
            std::vector<HWND>::iterator newEnd = std::remove(g_affectedWindows.begin(), g_affectedWindows.end(), hWnd);
            g_affectedWindows.erase(newEnd, g_affectedWindows.end());

            SetWindowRgn(hWnd, NULL, TRUE);
        }

        LeaveCriticalSection(&g_criticalSection);

        return S_OK;
    }

    return E_FAIL;
}

/*
 * HandleMaxForWindow: Handle the window position changed event and set a region 
 */
void HandleMaxForWindow(HWND hWnd, UINT uMsg)
{
    if (hWnd && (uMsg == WM_WINDOWPOSCHANGED))
    {
        /* General window style stuff */
        DWORD dwStyle = GetWindowLongPtrW(hWnd, GWL_STYLE);
        if (dwStyle & WS_MAXIMIZE && dwStyle & WS_CAPTION && !(dwStyle & WS_CHILD))
        {
            /* Check if window is truly maximized to the full size of the screen */
            int sizeBorders =
                GetSystemMetricsForWindow(hWnd, SM_CXSIZEFRAME) +
                GetSystemMetricsForWindow(hWnd, SM_CXPADDEDBORDER);

            /**
             * Get the info for the monitor the window is on.
             * This contains rcWork, which accounts for the taskbar.
             */
            HMONITOR hm = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi;
            mi.cbSize = sizeof(MONITORINFO);
            GetMonitorInfoW(hm, &mi);
            
            /* Add border size to rcWork */
            InflateRect(&mi.rcWork, sizeBorders, sizeBorders);
            
            RECT rcWnd;
            GetWindowRect(hWnd, &rcWnd);

            /* Compare the sizes. */
            if (rcWnd.right - rcWnd.left != mi.rcWork.right - mi.rcWork.left
            ||  rcWnd.bottom - rcWnd.top != mi.rcWork.bottom - mi.rcWork.top)
            {
                return;
            }

            if (std::find(g_affectedWindows.begin(), g_affectedWindows.end(), hWnd) == g_affectedWindows.end())
            {
                ApplyWindowMasking(hWnd);
            }
        }
        else
        {
            FakeUndoWindowMasking(hWnd);
        }
    }
}

typedef LRESULT (WINAPI *DefWindowProcA_t)(HWND, UINT, WPARAM, LPARAM);
DefWindowProcA_t DefWindowProcA_orig;
LRESULT WINAPI DefWindowProcA_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefWindowProcA_orig(hWnd, uMsg, wParam, lParam);

    HandleMaxForWindow(hWnd, uMsg);

    return result;
}

typedef LRESULT (WINAPI *DefWindowProcW_t)(HWND, UINT, WPARAM, LPARAM);
DefWindowProcW_t DefWindowProcW_orig;
LRESULT WINAPI DefWindowProcW_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = DefWindowProcW_orig(hWnd, uMsg, wParam, lParam);

    HandleMaxForWindow(hWnd, uMsg);

    return result;
}

/*
 * ThemePostWndProc_hook: Leverage theme hooks being available to hook window procedures globally.
 *
 * Since the theme engine is available for most applications, even for the majority of classic theme
 * users since Windows 8, it is possible to take advantage of one of its hooks in order to globally
 * hook window procedure handling.
 *
 * To my knowledge, this will only fail for 16-bit applications (which Windhawk cannot hook anyways)
 * and windows owned by certain system processes (such as CSRSS).
 */
typedef BOOL (CALLBACK *ThemePostWndProc_t)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult, void **ppvParam);
ThemePostWndProc_t ThemePostWndProc_orig;
BOOL CALLBACK ThemePostWndProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult, void **ppvParam)
{
    BOOL result = ThemePostWndProc_orig(hWnd, uMsg, wParam, lParam, plResult, ppvParam);

    HandleMaxForWindow(hWnd, uMsg);

    return result;
}

/*
 * InitCriticalSections: Setup data-access locks.
 */
bool InitCriticalSections()
{
    if (!InitializeCriticalSectionAndSpinCount(&g_criticalSection, 0x400))
        return false;

    return true;
}

/*
 * CleanCriticalSections: Clean up data-access locks.
 */
void CleanCriticalSections()
{
    DeleteCriticalSection(&g_criticalSection);
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit()
{
    Wh_Log(L"Init " WH_MOD_ID L" version " WH_MOD_VERSION);

    InitCriticalSections();

    HMODULE user32 = LoadLibraryW(L"user32.dll");
    HMODULE uxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    // Init DPI helpers:
    *(FARPROC *)&g_pGetDpiForWindow = GetProcAddress(user32, "GetDpiForWindow");
    *(FARPROC *)&g_pGetSystemMetricsForDpi = GetProcAddress(user32, "GetSystemMetricsForDpi");

    FARPROC pDefWindowProcW = GetProcAddress(user32, "DefWindowProcW");

    Wh_SetFunctionHook(
        (void *)pDefWindowProcW,
        (void *)DefWindowProcW_hook,
        (void **)&DefWindowProcW_orig
    );

    FARPROC pDefWindowProcA = GetProcAddress(user32, "DefWindowProcA");

    Wh_SetFunctionHook(
        (void *)pDefWindowProcA,
        (void *)DefWindowProcA_hook,
        (void **)&DefWindowProcA_orig
    );

    WindhawkUtils::SYMBOL_HOOK uxthemeDllHooks[] = {
        {
            {
                #ifdef _WIN64
                L"int __cdecl ThemePostWndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,__int64 *,void * *)"
                #else
                L"int __stdcall ThemePostWndProc(struct HWND__ *,unsigned int,unsigned int,long,long *,void * *)"
                #endif
            },
            &ThemePostWndProc_orig,
            ThemePostWndProc_hook
        }
    };

    HookSymbols(uxtheme, uxthemeDllHooks, ARRAYSIZE(uxthemeDllHooks));

    Wh_Log(L"Finished init");

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");

    CleanCriticalSections();
}

// ==WindhawkMod==
// @id              classic-maximized-windows-fix
// @name            Fix Classic Theme Maximized Windows
// @description     Fix maximized windows having borders that spill out onto additional displays when using the classic theme.
// @version         2.0
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
#include <unordered_map>
#include <algorithm>
#include <synchapi.h>
#include <windhawk_utils.h>

// Defines data shared by all instances of the library, even across processes.
#define SHARED_SECTION __attribute__((section(".shared")))
asm(".section .shared,\"dws\"\n");

CRITICAL_SECTION g_lockForAffectedWindows;
std::vector<HWND> g_affectedWindows;

void *g_pfnThemePostWndProc SHARED_SECTION = NULL;

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
    int sizeBorders =
        GetSystemMetricsForWindow(hWnd, SM_CXSIZEFRAME) +
        GetSystemMetricsForWindow(hWnd, SM_CXPADDEDBORDER);

    RECT rcWindow;
    GetWindowRect(hWnd, &rcWindow);

    int cxWindow = rcWindow.right - rcWindow.left;
    int cyWindow = rcWindow.bottom - rcWindow.top;

    // SetWindowRgn transfers ownership of the HRGN to the operating system, so it's not
    // our responsibility to clean.
    HRGN hRgn = CreateRectRgn(
        0 + sizeBorders, 0 + sizeBorders,
        cxWindow - sizeBorders, cyWindow - sizeBorders
    );

    g_affectedWindows.push_back(hWnd);

    if (SetWindowRgn(hWnd, hRgn, TRUE))
    {
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

    BOOL canLock = TryEnterCriticalSection(&g_lockForAffectedWindows);

    if (canLock)
    {
        if ((i = std::find(g_affectedWindows.begin(), g_affectedWindows.end(), hWnd)) != g_affectedWindows.end())
        {
            std::vector<HWND>::iterator newEnd = std::remove(g_affectedWindows.begin(), g_affectedWindows.end(), hWnd);
            g_affectedWindows.erase(newEnd, g_affectedWindows.end());

            SetWindowRgn(hWnd, NULL, TRUE);
        }

        LeaveCriticalSection(&g_lockForAffectedWindows);

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
            UndoWindowMasking(hWnd);
        }
    }
}

typedef LRESULT (WINAPI *DefWindowProcA_t)(HWND, UINT, WPARAM, LPARAM);
DefWindowProcA_t DefWindowProcA_orig;
LRESULT WINAPI DefWindowProcA_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HandleMaxForWindow(hWnd, uMsg);

    return DefWindowProcA_orig(hWnd, uMsg, wParam, lParam);
}

typedef LRESULT (WINAPI *DefWindowProcW_t)(HWND, UINT, WPARAM, LPARAM);
DefWindowProcW_t DefWindowProcW_orig;
LRESULT WINAPI DefWindowProcW_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HandleMaxForWindow(hWnd, uMsg);

    return DefWindowProcW_orig(hWnd, uMsg, wParam, lParam);
}

/*
 * InitCriticalSections: Setup data-access locks.
 */
bool InitCriticalSections()
{
    if (!InitializeCriticalSectionAndSpinCount(&g_lockForAffectedWindows, 0x400))
        return false;

    return true;
}

/*
 * CleanCriticalSections: Clean up data-access locks.
 */
void CleanCriticalSections()
{
    DeleteCriticalSection(&g_lockForAffectedWindows);
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

struct CMWF_SYMBOL_HOOK {
    std::vector<std::wstring> symbols;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;
    void **pSharedMemoryCache;
};

/*
 * CmwfHookSymbols: A custom hook wrapper which allows storing symbol hook results in the
 *                  module's shared memory for faster access.
 *
 * TODO: This is good for the current application, which hooks only one symbol, but it will
 *       be less efficient to use for multiple symbol hooks at the moment because it requeries
 *       each time. Please keep this in mind if you're looking to use this code for your own
 *       purposes.
 */
bool CmwfHookSymbols(
        HMODULE module,
        const CMWF_SYMBOL_HOOK *symbolHooks,
        size_t symbolHooksCount
)
{
    for (size_t i = 0; i < symbolHooksCount; i++)
    {
        void *address = nullptr;
        if (symbolHooks[i].pSharedMemoryCache && *(symbolHooks[i].pSharedMemoryCache) != NULL)
        {
            address = *(symbolHooks[i].pSharedMemoryCache);
            Wh_Log(
                L"CmwfHookSymbols: Hooking symbol %.*s from in-memory cache.",
                symbolHooks[i].symbols[0].length(),
                symbolHooks[i].symbols[0].data()
            );
        }
        else
        {
            address = nullptr;
            WindhawkUtils::SYMBOL_HOOK proxyHook = {
                symbolHooks[i].symbols,
                &address,
                NULL,
                symbolHooks[i].optional
            };

            if (!WindhawkUtils::HookSymbols(module, &proxyHook, 1))
            {
                return false;
            }

            if (address)
            {
                if (symbolHooks[i].pSharedMemoryCache && *(symbolHooks[i].pSharedMemoryCache) == NULL)
                {
                    *(symbolHooks[i].pSharedMemoryCache) = address;
                }
            }
            else
            {
                return false;
            }
        }

        if (address != NULL)
        {
            Wh_SetFunctionHook(
                address,
                symbolHooks[i].hookFunction,
                symbolHooks[i].pOriginalFunction
            );
        }
    }

    return true;
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit()
{
    Wh_Log(L"Init " WH_MOD_ID L" version " WH_MOD_VERSION);

    InitCriticalSections();

    HMODULE user32 = LoadLibraryW(L"user32.dll");
    HMODULE uxtheme = LoadLibraryW(L"uxtheme.dll");

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

    CMWF_SYMBOL_HOOK symbolHooks[] = {
        {
            .symbols = {
                #ifdef _WIN64
                L"int __cdecl ThemePostWndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,__int64 *,void * *)"
                #else
                L"int __stdcall ThemePostWndProc(struct HWND__ *,unsigned int,unsigned int,long,long *,void * *)"
                #endif
            },
            .pOriginalFunction = (void **)&ThemePostWndProc_orig,
            .hookFunction = (void *)ThemePostWndProc_hook,
            .pSharedMemoryCache = (void **)&g_pfnThemePostWndProc
        }
    };

    CmwfHookSymbols(uxtheme, symbolHooks, ARRAYSIZE(symbolHooks));

    Wh_Log(L"Finished init");

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");

    CleanCriticalSections();
}

// ==WindhawkMod==
// @id              classic-maximized-windows-fix
// @name            Fix Classic Theme Maximized Windows
// @description     Fix maximized windows having borders that spill out onto additional displays when using the classic theme.
// @version         1.1
// @author          ephemeralViolette
// @github          https://github.com/ephemeralViolette
// @include         *

// @exclude         windhawk.exe
// @exclude         vscodium.exe

// @exclude         conhost.exe
// @exclude         consent.exe
// @exclude         dwm.exe
// @exclude         lsass.exe
// @exclude         winlogon.exe
// @exclude         logonui.exe
// @exclude         ApplicationFrameHost.exe

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

// No critical section; this is only written to once and from only one thread.
bool g_instanceHasInit = false;

CRITICAL_SECTION g_lockForAffectedWindows;
std::vector<HWND> g_affectedWindows;

CRITICAL_SECTION g_lockForAffectedThreads;
std::unordered_map<DWORD, HHOOK> g_affectedThreads;

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
        DWORD dwStyle = GetWindowLongPtrW(hWnd, GWL_STYLE);

        if (dwStyle & WS_MAXIMIZE && dwStyle & WS_CAPTION && !(dwStyle & WS_CHILD))
        {
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
 * OnCallWndProc: Handle window procedure calls received from the hooks.
 */
LRESULT CALLBACK OnCallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    CWPSTRUCT *params = (CWPSTRUCT *)lParam;

    if (nCode < 0)
        return 0;

    HandleMaxForWindow(params->hwnd, params->message);

    return 0;
}

/*
 * InitCriticalSections: Setup data-access locks.
 */
bool InitCriticalSections()
{
    if (!InitializeCriticalSectionAndSpinCount(&g_lockForAffectedWindows, 0x400))
        return false;

    if (!InitializeCriticalSectionAndSpinCount(&g_lockForAffectedThreads, 0x400))
        return false;

    return true;
}

/*
 * CleanCriticalSections: Clean up data-access locks.
 */
void CleanCriticalSections()
{
    DeleteCriticalSection(&g_lockForAffectedWindows);
    DeleteCriticalSection(&g_lockForAffectedThreads);
}

/*
 * InstallHookForCurrentThread: Sets up a Windows Hook for listening to application messages.
 *
 * This is used as an alternative method to knowing when a window is maximized, since it doesn't
 * depend on a program calling DefWindowProc.
 */
HRESULT InstallHookForCurrentThread()
{
    HRESULT hr = S_OK;
    DWORD currentThreadId = GetCurrentThreadId();
    Wh_Log(L"Entering critical section in thread #%d", currentThreadId);
    EnterCriticalSection(&g_lockForAffectedThreads);

    if (!g_affectedThreads.contains(currentThreadId))
    {
        HHOOK hHook = SetWindowsHookExW(
            WH_CALLWNDPROC,
            (HOOKPROC)OnCallWndProc,
            NULL,
            currentThreadId
        );

        if (hHook)
        {
            Wh_Log(
                L"Installed Windows Hook #%d (%p) for thread #%d (%p)",
                hHook, hHook,
                currentThreadId, currentThreadId
            );
            g_affectedThreads.insert(
                std::make_pair(currentThreadId, hHook)
            );
        }
        else
        {
            hr = E_FAIL;
        }
    }
    Wh_Log(L"Leaving critical section in thread #%d", currentThreadId);
    LeaveCriticalSection(&g_lockForAffectedThreads);

    return hr;
}

/*
 * UninstallHooks: Uninstall all Windows Hooks for every thread.
 */
void UninstallHooks()
{
    DWORD currentThreadId = GetCurrentThreadId();
    Wh_Log(L"Entering critical section in thread #%d", currentThreadId);
    EnterCriticalSection(&g_lockForAffectedThreads);

    int installedHooks = g_affectedThreads.size();

    for (auto &it : g_affectedThreads)
    {
        HHOOK &hHook = it.second;
        BOOL success = UnhookWindowsHookEx(hHook);

        if (success)
            installedHooks--;
    }

    for (HWND &it : g_affectedWindows)
    {
        UndoWindowMasking(it);
    }

    if (installedHooks == 0)
    {
        Wh_Log(L"Successfully uninstalled all hooks.");
        g_affectedThreads.clear();
    }
    else
    {
        Wh_Log(L"Failed to uninstall %d hooks. Memory was not cleared for process.", installedHooks);
    }

    Wh_Log(L"Leaving critical section in thread #%d", currentThreadId);
    LeaveCriticalSection(&g_lockForAffectedThreads);
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit()
{
    Wh_Log(L"Init " WH_MOD_ID L" version " WH_MOD_VERSION);

    InitCriticalSections();

    HMODULE user32 = LoadLibraryW(L"user32.dll");

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

    // Windows hook as a workaround for some applications (i.e. Firefox):
    InstallHookForCurrentThread();

    g_instanceHasInit = true;

    Wh_Log(L"Finished init");

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");

    UninstallHooks();
    CleanCriticalSections();

    g_instanceHasInit = false;
}

/*
 * DllMain: Listen for thread creation for hook installation.
 */
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        // DLL_THREAD_ATTACH: Install a Windows Hook on all incoming threads (if necessary):
        case DLL_THREAD_ATTACH:
        {
            InstallHookForCurrentThread();
            break;
        }

        case DLL_THREAD_DETACH:
        {
            DWORD currentThreadId = GetCurrentThreadId();
            Wh_Log(L"Entering critical section in thread #%d", currentThreadId);
            EnterCriticalSection(&g_lockForAffectedThreads);

            if (g_affectedThreads.contains(currentThreadId))
            {
                HHOOK &hHook = g_affectedThreads.at(currentThreadId);
                BOOL status = UnhookWindowsHookEx(hHook);

                if (status)
                {
                    Wh_Log(
                        L"Successfully detached Windows Hook #%d (%p) for thread #%d (%p)!",
                        hHook, hHook,
                        currentThreadId, currentThreadId
                    );

                    g_affectedThreads.erase(currentThreadId);
                }
                else
                {
                    Wh_Log(
                        L"Failed to detach Windows Hook #%d (%p) for thread #%d (%p)",
                        hHook, hHook,
                        currentThreadId, currentThreadId
                    );
                }
            }

            Wh_Log(L"Leaving critical section in thread #%d", currentThreadId);
            LeaveCriticalSection(&g_lockForAffectedThreads);

            break;
        }
    }

    return TRUE;
}

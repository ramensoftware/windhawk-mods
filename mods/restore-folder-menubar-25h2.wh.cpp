// ==WindhawkMod==
// @id              restore-folder-menubar-25h2
// @name            ExplorerFrame and menubar fixes fork
// @description     Fixes explorer problems: menu bar, listview redraw, classic background color
// @version         2.0.1
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32 -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

This is a fork of the mod `Explorerframe fixes for Win11 22H2+` by Waldemar made due to the author's apparent inactivity.
It addresses several issues with the original mod.

**The original mod description**

This mod fixes a couple of issues with File Explorer windows appearance in Windows 11 releases 22H2, 23H2 and 24H2.

Unpredictable menu bar appearance: On Windows 11 22H2 and 23H2, the menu bar may appear unpredictably. This mod allows the user to disable the menu bar entirely, or have it be shown at all times.
You can force show/hide the menu bar in the mod settings. Forcing the menu bar to be shown is intended for the Classic theme.

Delay on navigation to a new folder when using SysListView32: this is caused by a extra erroneous message sent to the list view window. This mod supresses it, fixing the problem.

Black artifacts briefly visible in new file explorer windows under Classic theme: this is caused by incorrect handling of WM_ERASEBKGND in CabinetWClass window procedure. This mod adds a option to correct this.

**Changes from the original**

* This mod restores the menubar in recent builds of 24H2 and in 25H2 with which the original mod was not compatible. 
**!Important!** On newer releases of Windows 11 24H2 and later, you may need to use the [ViVeTool](https://github.com/thebookisclosed/ViVe/releases) utility
with the following command so to enable the menu:

    `vivetool /disable /id:55063786`

    Alternatively, you can use the [ViVeToolGUI](https://apps.microsoft.com/detail/9P1BW5WB82MH) from
    Windows Store.

    On even newer builds, replacing the file Explorerframe.dll with an older one may be required.

* This mod fixes the compatibility with the mod ```Fluid Window Engine Pro```, in cases where the previous mod would 
whiten out the window content on restore after the minimized state if the option `Classic CabinetWClass background color` was enabled.

* The fix for the folders' background in Classic theme is applied authomatically when Classic theme is detected.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- DisplayMenuBar: true
  $name: Display menu bar
  $description: Show the classic menu bar in Explorer windows.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <Windows.h>
#include <commctrl.h>
#include <uxtheme.h>
#include <shdeprecated.h>
#include <vector>

bool g_settingDisplayMenuBar;

std::vector<HWND> g_subclassedRebars;
std::vector<HWND> g_subclassedListviews;

#define WM_MENUBAR_HACKFIX (WM_USER + 1)
#define HACKFIX_FLAGS (SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOMOVE)

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

int FindMenuBand(HWND hReBar)
{
    int bandCount = (int)SendMessage(hReBar, RB_GETBANDCOUNT, 0, 0);
    for (int i = 0; i < bandCount; i++)
    {
        REBARBANDINFO rbbi = { sizeof(REBARBANDINFO) };
        rbbi.fMask = RBBIM_CHILD | RBBIM_STYLE;
        SendMessage(hReBar, RB_GETBANDINFO, i, (LPARAM)&rbbi);

        if (rbbi.hwndChild == NULL) continue;

        wchar_t className[256];
        GetClassNameW(rbbi.hwndChild, className, 256);
        if (lstrcmpW(className, L"ToolbarWindow32") != 0) continue;

        // Menu band toolbar has no image list
        if (SendMessageW(rbbi.hwndChild, TB_GETIMAGELIST, 0, 0) != NULL) continue;

        return i;
    }
    return -1;
}

// ---------------------------------------------------------------------------
// Rebar subclass — handles menu bar show/hide with hackfix for both
// old (ReBar -> WorkerW -> CabinetWClass) and
// new (ReBar -> WorkerW -> ShellTabWindowClass -> CabinetWClass) structures
// ---------------------------------------------------------------------------

LRESULT CALLBACK RebarSubclassProc(_In_ HWND hWnd, _In_ UINT uMsg,
    _In_ WPARAM wParam, _In_ LPARAM lParam, _In_ DWORD_PTR dwRefData)
{
    if (uMsg == WM_DESTROY)
    {
        g_subclassedRebars.erase(
            std::remove(g_subclassedRebars.begin(), g_subclassedRebars.end(), hWnd),
            g_subclassedRebars.end());
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    if (uMsg == WM_MENUBAR_HACKFIX)
    {
        HWND workerW = GetParent(hWnd);
        if (!workerW) return 0;

        HWND parent = GetParent(workerW);
        if (!parent) return 0;

        wchar_t cls[256] = {};
        GetClassNameW(parent, cls, 256);

        HWND target = parent;
        if (!lstrcmpW(cls, L"ShellTabWindowClass"))
        {
            // New structure (24H2+): ReBar -> WorkerW -> ShellTabWindowClass -> CabinetWClass
            SendMessageW(target, WM_WININICHANGE, 0, 0);
        }

        RECT rect;
        GetClientRect(target, &rect);
        int w = rect.right - rect.left;
        int h = rect.bottom - rect.top;
        SetWindowPos(hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        return 0;
    }

    if (uMsg == RB_SHOWBAND)
    {
        int menuBandIndex = FindMenuBand(hWnd);
        if ((int)wParam == menuBandIndex)
        {
            LRESULT toRet = DefSubclassProc(hWnd, uMsg, wParam, (LPARAM)g_settingDisplayMenuBar);
            if (g_settingDisplayMenuBar)
                PostMessage(hWnd, WM_MENUBAR_HACKFIX, 0, 0);
            return toRet;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Listview subclass — prevents WM_SETREDRAW from disabling redraws
// ---------------------------------------------------------------------------

LRESULT CALLBACK ListviewSubclassProc(_In_ HWND hWnd, _In_ UINT uMsg,
    _In_ WPARAM wParam, _In_ LPARAM lParam, _In_ DWORD_PTR dwRefData)
{
    if (uMsg == WM_DESTROY)
    {
        g_subclassedListviews.erase(
            std::remove(g_subclassedListviews.begin(), g_subclassedListviews.end(), hWnd),
            g_subclassedListviews.end());
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    // Always keep redraws enabled
    if (uMsg == WM_SETREDRAW)
    {
        wParam = TRUE;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Hook: CBandSite::_Initialize — subclass rebar for menu bar control
// ---------------------------------------------------------------------------

typedef long (*__cdecl CBSInitialize_t)(void*, HWND);
CBSInitialize_t CBSInitializeOriginal;

long __cdecl CBSInitializeHook(void* pThis, HWND hWnd)
{
    // Only target the correct ReBarWindow32 (exstyle == 0)
    if (GetWindowLong(hWnd, GWL_EXSTYLE) == 0)
    {
        HWND rb = FindWindowExW(hWnd, NULL, L"ReBarWindow32", NULL);
        if (rb != NULL)
        {
            WindhawkUtils::SetWindowSubclassFromAnyThread(rb, RebarSubclassProc, NULL);
            g_subclassedRebars.push_back(rb);
        }
    }

    return CBSInitializeOriginal(pThis, hWnd);
}

// ---------------------------------------------------------------------------
// Hook: CListViewHost::CreateControl — subclass listview for redraw fix
// ---------------------------------------------------------------------------

typedef long (*__cdecl CLVHCreateControl_t)(void*, HWND, void*, void*);
CLVHCreateControl_t CLVHCreateControlOriginal;

long __cdecl CLVHCreateControlHook(void* pThis, HWND hWnd, void* a, void* b)
{
    long ret = CLVHCreateControlOriginal(pThis, hWnd, a, b);

    HWND listview = GetWindow(hWnd, GW_CHILD);
    if (listview != NULL)
    {
        wchar_t name[32];
        GetClassNameW(listview, name, 32);
        if (lstrcmpW(name, L"SysListView32") == 0)
        {
            WindhawkUtils::SetWindowSubclassFromAnyThread(listview, ListviewSubclassProc, NULL);
            g_subclassedListviews.push_back(listview);
        }
    }

    return ret;
}

// ---------------------------------------------------------------------------
// Hook: CExplorerFrame::v_WndProc — WM_SHOWWINDOW fix + classic background
// ---------------------------------------------------------------------------

typedef long (*__cdecl CEFWndProc_t)(void*, HWND, unsigned int, WPARAM, LPARAM);
CEFWndProc_t CEFWndProcOriginal;

long __cdecl CEFWndProcHook(void* pThis, HWND hWnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_SHOWWINDOW)
    {
        // Fixes unpredictable menu bar appearance; companion hook (CSBSetFlags)
        // prevents the side-effect of blocking layout saving.
        SendMessageW(hWnd, WM_WININICHANGE, 0, 0);
    }

    if (uMsg == WM_ERASEBKGND)
    {
        // Apply classic background only when visual themes are disabled
        if (!IsAppThemed())
        {
            // Skip when window is layered (e.g. during animation/screenshot capture)
            if ((GetWindowLongW(hWnd, GWL_EXSTYLE) & WS_EX_LAYERED) == 0)
            {
                HDC hdc = (HDC)wParam;

                // Skip memory DCs (PrintWindow / off-screen rendering)
                if (GetObjectType(hdc) != OBJ_MEMDC)
                {
                    RECT rc;
                    GetClientRect(hWnd, &rc);
                    HBRUSH hbr = (HBRUSH)(COLOR_WINDOW + 1);
                    FillRect(hdc, &rc, hbr);
                    return 1;
                }
            }
        }
    }

    return CEFWndProcOriginal(pThis, hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Hook: CShellBrowser::SetFlags — block BSF_UISETBYAUTOMATION side-effect
// ---------------------------------------------------------------------------

typedef long (*__cdecl CSBSetFlags_t)(void*, unsigned long, unsigned long);
CSBSetFlags_t CSBSetFlagsOriginal;

long __cdecl CSBSetFlagsHook(void* pThis, unsigned long a, unsigned long b)
{
    // This deprecated flag prevents the browser from saving its layout
    if (a & BSF_UISETBYAUTOMATION)
        a &= ~BSF_UISETBYAUTOMATION;

    return CSBSetFlagsOriginal(pThis, a, b);
}

// ---------------------------------------------------------------------------
// Mod lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit()
{
    Wh_Log(L"Explorer Frame Fixes (Merged) Init");

    // --- explorerframe.dll hooks ---
    HMODULE hExplorerFrame = LoadLibraryW(L"explorerframe.dll");
    if (!hExplorerFrame)
    {
        Wh_Log(L"Failed to load explorerframe.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK explorerframe_dll_hooks[] =
    {
        {
            { L"private: virtual __int64 __cdecl CExplorerFrame::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)" },
            (void**)&CEFWndProcOriginal,
            (void*)CEFWndProcHook,
            FALSE
        },
        {
            { L"protected: virtual long __cdecl CBandSite::_Initialize(struct HWND__ *)" },
            (void**)&CBSInitializeOriginal,
            (void*)CBSInitializeHook,
            FALSE
        },
        {
            { L"public: virtual long __cdecl CShellBrowser::SetFlags(unsigned long,unsigned long)" },
            (void**)&CSBSetFlagsOriginal,
            (void*)CSBSetFlagsHook,
            FALSE
        }
    };

    if (!WindhawkUtils::HookSymbols(hExplorerFrame, explorerframe_dll_hooks, ARRAYSIZE(explorerframe_dll_hooks)))
    {
        Wh_Log(L"Failed to install explorerframe.dll hooks");
        return FALSE;
    }

    // --- shell32.dll hooks ---
    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK shell32_dll_hooks[] =
    {
        {
            { L"public: virtual long __cdecl CListViewHost::CreateControl(struct HWND__ *,struct IListControlHost *,struct IViewSettings *)" },
            (void**)&CLVHCreateControlOriginal,
            (void*)CLVHCreateControlHook,
            FALSE
        }
    };

    if (!WindhawkUtils::HookSymbols(hShell32, shell32_dll_hooks, ARRAYSIZE(shell32_dll_hooks)))
    {
        Wh_Log(L"Failed to install shell32.dll hooks");
        return FALSE;
    }

    g_settingDisplayMenuBar = Wh_GetIntSetting(L"DisplayMenuBar");

    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"Explorer Frame Fixes (Merged) Uninit");

    Wh_Log(L"Removing subclasses from %zu rebars.", g_subclassedRebars.size());
    for (HWND h : g_subclassedRebars)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(h, RebarSubclassProc);

    Wh_Log(L"Removing subclasses from %zu listviews.", g_subclassedListviews.size());
    for (HWND h : g_subclassedListviews)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(h, ListviewSubclassProc);
}

void Wh_ModSettingsChanged()
{
    g_settingDisplayMenuBar = Wh_GetIntSetting(L"DisplayMenuBar");
}

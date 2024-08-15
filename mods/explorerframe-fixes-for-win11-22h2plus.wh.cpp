// ==WindhawkMod==
// @id              explorerframe-fixes-for-win11-22h2plus
// @name            Explorerframe fixes for Win11 22H2+
// @description     Fixes three problems with file explorer
// @version         1.0
// @author          Waldemar
// @github          https://github.com/CyprinusCarpio
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorerframe fixes for Win11 22H2+
This mod fixes a couple of issues with File Explorer windows appearance in Windows 11 releases 22H2, 23H2 and 24H2.

Unpredictable menu bar appearance: On Windows 11 22H2 and 23H2, the menu bar may appear unpredictably. This mod allows the user to disable the menu bar entirely, or have it be shown at all times.
You can force show/hide the menu bar in the mod settings. Forcing the menu bar to be shown is intended for the Classic theme.

Delay on navigation to a new folder when using SysListView32: this is caused by a extra erroneous message sent to the list view window. This mod supresses it, fixing the problem.

Black artifacts briefly visible in new file explorer windows under Classic theme: this is caused by incorrect handling of WM_ERASEBKGND in CabinetWClass window procedure. This mod adds a option to correct this.

Changes will be visible in new file explorer windows.
*/
// ==/WindhawkModReadme==
#include <windhawk_utils.h>
#include <Windows.h>
#include <vector>

// ==WindhawkModSettings==
/*
- DisplayMenuBar: true
  $name: Display Menu Bar
  $description: Whether to display the menu bar. This overrides system behaviour, which is unpredictable.
- ClassicBackgroundColor: false
  $name: Classic CabinetWClass background color
  $description: Use classic background color for explorer windows.
*/
// ==/WindhawkModSettings==

bool g_settingDisplayMenuBar;
bool g_settingFixCWCBackground;
std::vector<HWND> g_subclassedRebars;
std::vector<HWND> g_subclassedListviews;

int FindMenuBand(HWND hReBar) 
{
    int bandCount = (int)SendMessage(hReBar, RB_GETBANDCOUNT, 0, 0);
    for (int i = 0; i < bandCount; i++) 
    {
        REBARBANDINFO rbbi = { sizeof(REBARBANDINFO) };
        rbbi.fMask = RBBIM_CHILD | RBBIM_STYLE;
        SendMessage(hReBar, RB_GETBANDINFO, i, (LPARAM)&rbbi);

        if (rbbi.hwndChild == NULL) continue;

        // Check if the child window is a ToolbarWindow32
        wchar_t className[256];
        GetClassNameW(rbbi.hwndChild, className, sizeof(className));
        if (lstrcmpW(className, L"ToolbarWindow32") != 0) continue;

        // Check for image list. Menu doesn't have one.
        if(SendMessageW(rbbi.hwndChild, TB_GETIMAGELIST, 0, 0) != NULL) continue;

        return i;
    }

    return -1;
}

#define HACKFIX_FLAGS SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOMOVE

LRESULT CALLBACK RebarSubclassProc(_In_ HWND hWnd,
        _In_ UINT uMsg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam,
        _In_ DWORD_PTR dwRefData)
{
    if(uMsg == WM_DESTROY)
    {
        g_subclassedRebars.erase(std::remove_if(
                              g_subclassedRebars.begin(), g_subclassedRebars.end(),
                              [hWnd](HWND &h)
        {
            return h == hWnd;
        }));
        return 0;
    }
    if(uMsg == RB_SHOWBAND)
    {
        if((int)wParam == FindMenuBand(hWnd))
        {
            LRESULT toRet = DefSubclassProc(hWnd, uMsg, wParam, g_settingDisplayMenuBar);
            if(g_settingDisplayMenuBar)
            {
				// If the following hack is not done, menu bar items may be invisible.
                HWND stwc = GetParent(GetParent(hWnd));
                RECT rect;
                GetClientRect(stwc, &rect);
                int wx = rect.right - rect.left;
                int wy = rect.bottom - rect.top;
                SetWindowPos(stwc, NULL, 0, 0, wx <= 0 ? 1300 : wx + 1, wy <= 0 ? 900 : wy + 1, HACKFIX_FLAGS);
                if(wx <= 0)
                    SetWindowPos(stwc, NULL, 0, 0, wx, wy, HACKFIX_FLAGS);
                RedrawWindow(stwc, NULL, NULL, RDW_INVALIDATE);
            }
            return toRet;
        }
    }
    
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

typedef long (*__cdecl CBSInitialize_t)(void*, HWND);
CBSInitialize_t CBSInitializeOriginal;
long __cdecl CBSInitializeHook(void *pThis, HWND hWnd)
{
    // We need not subclass the other ReBarWindow32.
    // At this point in time, it's exstyle is different.
    if(GetWindowLong(hWnd, GWL_EXSTYLE) == 0)
    {
        HWND rb = FindWindowExW(hWnd, NULL, L"ReBarWindow32", NULL);
        if(rb != NULL)
        {
            WindhawkUtils::SetWindowSubclassFromAnyThread(rb, RebarSubclassProc, NULL);
            g_subclassedRebars.push_back(rb);
        }
    }

    return CBSInitializeOriginal(pThis, hWnd);
}

LRESULT CALLBACK ListviewSubclassProc(_In_ HWND hWnd,
        _In_ UINT uMsg,
        _In_ WPARAM wParam,
        _In_ LPARAM lParam,
        _In_ DWORD_PTR dwRefData)
{
    if(uMsg == WM_DESTROY)
    {
        g_subclassedListviews.erase(std::remove_if(
                              g_subclassedListviews.begin(), g_subclassedListviews.end(),
                              [hWnd](HWND &h)
        {
            return h == hWnd;
        }));
        return 0;
    }
	// This is less than surgical, but it appears to have no adverse effects.
    if(uMsg == WM_SETREDRAW)
    {
        wParam = true;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

typedef long (*__cdecl CLVHCreateControl_t)(void*, HWND, void*, void*);
CLVHCreateControl_t CLVHCreateControlOriginal;
long __cdecl CLVHCreateControlHook(void* pThis, HWND hWnd, void* a, void* b)
{
    long ret = CLVHCreateControlOriginal(pThis, hWnd, a, b);
    HWND listview = GetWindow(hWnd, GW_CHILD);
    if(listview != NULL)
    {
        wchar_t name[32];
        GetClassNameW(listview, name, 32);
        if(lstrcmpW(name, L"SysListView32") == 0)
        {
            WindhawkUtils::SetWindowSubclassFromAnyThread(listview, ListviewSubclassProc, NULL);
            g_subclassedListviews.push_back(listview);
        }
    }
    return ret;
}

typedef long(*__cdecl CEFWndProc_t)(void*, HWND, unsigned int, WPARAM, LPARAM);
CEFWndProc_t CEFWndProcOriginal;
long __cdecl CEFWndProcHook(void* pThis, HWND hWnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam)
{
    if(g_settingFixCWCBackground && uMsg == WM_ERASEBKGND)
    {
        HDC hdc = (HDC)wParam;
        RECT rc;
        GetClientRect(hWnd, &rc);
        HBRUSH hbr = (HBRUSH)(COLOR_WINDOW + 1);
        FillRect(hdc, &rc, hbr);
        return 1;
    }
    return CEFWndProcOriginal(pThis, hWnd, uMsg, wParam, lParam);
}

BOOL Wh_ModInit() 
{
    Wh_Log(L"Explorerframe fixes for Win11 22H2+ Init");

    HMODULE hExplorerFrame = LoadLibraryW(L"explorerframe.dll");

    if (!hExplorerFrame)
    {
        Wh_Log(L"Failed to load explorerframe.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK explorerframe_dll_hooks[] =
    {
        {   {
                L"private: virtual __int64 __cdecl CExplorerFrame::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
            },
            (void**)&CEFWndProcOriginal,
            (void*)CEFWndProcHook,
            FALSE
        },
        {   {
                L"protected: virtual long __cdecl CBandSite::_Initialize(struct HWND__ *)"
            },
            (void**)&CBSInitializeOriginal,
            (void*)CBSInitializeHook,
            FALSE
        }
    };

    if (!WindhawkUtils::HookSymbols(hExplorerFrame, explorerframe_dll_hooks, 2))
    {
        Wh_Log(L"Failed install explorerframe hooks");
        return FALSE;
    }

    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");

    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK shell32_dll_hooks[] =
    {
        {   {
                L"public: virtual long __cdecl CListViewHost::CreateControl(struct HWND__ *,struct IListControlHost *,struct IViewSettings *)"
            },
            (void**)&CLVHCreateControlOriginal,
            (void*)CLVHCreateControlHook,
            FALSE
        }
        
    };

    if (!WindhawkUtils::HookSymbols(hShell32, shell32_dll_hooks, 1))
    {
        Wh_Log(L"Failed to hook CListViewHost::CreateControl");
        return FALSE;
    }

    g_settingDisplayMenuBar = Wh_GetIntSetting(L"DisplayMenuBar");
	g_settingFixCWCBackground = Wh_GetIntSetting(L"ClassicBackgroundColor");

    return TRUE;
}

void Wh_ModUninit() 
{
    Wh_Log(L"Explorerframe fixes for Win11 22H2+ Uninit");
    Wh_Log(L"Removing subclasses from %i rebars.", g_subclassedRebars.size());
    for(HWND& h : g_subclassedRebars)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(h, RebarSubclassProc);
    }
    Wh_Log(L"Removing subclasses from %i listviews.", g_subclassedListviews.size());
    for(HWND& h : g_subclassedListviews)
    {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(h, ListviewSubclassProc);
    }
}

void Wh_ModSettingsChanged()
{
    g_settingDisplayMenuBar = Wh_GetIntSetting(L"DisplayMenuBar");
    g_settingFixCWCBackground = Wh_GetIntSetting(L"ClassicBackgroundColor");
}
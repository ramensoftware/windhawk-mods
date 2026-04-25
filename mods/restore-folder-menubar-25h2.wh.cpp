// ==WindhawkMod==
// @id              restore-folder-menubar-25h2
// @name            Restore folder menubar in 24h2, 25h2
// @description     Restores the menubar in folder windows in 24h2, 25h2 and later
// @version         1.0.1
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Restore folder menubar

This mod in a spin-off from the mod `Explorerframe fixes for Win11 22H2+`, particularly,
its functionality related to the menubar in folders, because the author of that mod has
been recently inactive while recent builds of Windows became incompatible.

This mod restores the menubar in recent builds of 24h2 and in 25h2.

**!Important!** You would need to use the [ViVeTool](https://github.com/thebookisclosed/ViVe/releases) utility
with the following command before using this mod:

`vivetool /disable /id:55063786`

Alternatively, you can use the [ViVeToolGUI](https://apps.microsoft.com/detail/9P1BW5WB82MH) from
Windows Store.
On later builds (such as 26200.8037 and above) you will also need to replace the file Explorerframe.dll with the one from an older Windows version,
because the menubar has been removed completely.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- DisplayMenuBar: true
  $name: Display Menu Bar
  $description: Whether to display the menu bar. This overrides system behaviour, which is unpredictable.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <Windows.h>
#include <commctrl.h>
#include <shdeprecated.h>
#include <vector>

bool g_settingDisplayMenuBar;
std::vector<HWND> g_subclassedRebars;

#define WM_MENUBAR_HACKFIX (WM_USER + 1)
#define HACKFIX_FLAGS (SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOMOVE)

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

LRESULT CALLBACK RebarSubclassProc(_In_ HWND hWnd, _In_ UINT uMsg,
    _In_ WPARAM wParam, _In_ LPARAM lParam, _In_ DWORD_PTR dwRefData)
{
    if (uMsg == WM_DESTROY)
    {
        g_subclassedRebars.erase(std::remove(
            g_subclassedRebars.begin(), g_subclassedRebars.end(), hWnd),
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

        HWND target;
        if (lstrcmpW(cls, L"ShellTabWindowClass") == 0)
        {
            // New structure: ReBar -> WorkerW -> ShellTabWindowClass -> CabinetWClass
            target = parent;
            SendMessageW(target, WM_WININICHANGE, 0, 0);
        }
        else
        {
            // Old structure: ReBar -> WorkerW -> CabinetWClass
            target = parent;
        }

        RECT rect;
        GetClientRect(target, &rect);
        int w = rect.right - rect.left;
        int h = rect.bottom - rect.top;

        if (w > 0 && h > 0)
        {
            SetWindowPos(target, NULL, 0, 0, w + 1, h + 1, HACKFIX_FLAGS);
            SetWindowPos(target, NULL, 0, 0, w, h, HACKFIX_FLAGS);
            RedrawWindow(target, NULL, NULL, RDW_INVALIDATE);
        }

        return 0;
    }

    if (uMsg == RB_SHOWBAND)
    {
        int menuBandIndex = FindMenuBand(hWnd);
        if ((int)wParam == menuBandIndex)
        {
            LRESULT toRet = DefSubclassProc(hWnd, uMsg, wParam, g_settingDisplayMenuBar);
            if (g_settingDisplayMenuBar)
                PostMessage(hWnd, WM_MENUBAR_HACKFIX, 0, 0);
            return toRet;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

typedef long (*__cdecl CBSInitialize_t)(void*, HWND);
CBSInitialize_t CBSInitializeOriginal;
long __cdecl CBSInitializeHook(void* pThis, HWND hWnd)
{
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

typedef long (*__cdecl CSBSetFlags_t)(void*, unsigned long, unsigned long);
CSBSetFlags_t CSBSetFlagsOriginal;
long __cdecl CSBSetFlagsHook(void* pThis, unsigned long a, unsigned long b)
{
    // Prevent automation from changing menu bar visibility
    if (a & BSF_UISETBYAUTOMATION)
        a &= ~BSF_UISETBYAUTOMATION;
    return CSBSetFlagsOriginal(pThis, a, b);
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Explorer Menu Bar Control Init");

    HMODULE hExplorerFrame = LoadLibraryW(L"explorerframe.dll");
    if (!hExplorerFrame)
    {
        Wh_Log(L"Failed to load explorerframe.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK explorerframe_dll_hooks[] =
    {
        {{ L"protected: virtual long __cdecl CBandSite::_Initialize(struct HWND__ *)" },
            (void**)&CBSInitializeOriginal, (void*)CBSInitializeHook, FALSE },
        {{ L"public: virtual long __cdecl CShellBrowser::SetFlags(unsigned long,unsigned long)" },
            (void**)&CSBSetFlagsOriginal, (void*)CSBSetFlagsHook, FALSE }
    };

    if (!WindhawkUtils::HookSymbols(hExplorerFrame, explorerframe_dll_hooks, ARRAYSIZE(explorerframe_dll_hooks)))
    {
        Wh_Log(L"Failed to hook explorerframe.dll");
        return FALSE;
    }

    g_settingDisplayMenuBar = Wh_GetIntSetting(L"DisplayMenuBar");

    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"Explorer Menu Bar Control Uninit");

    for (HWND h : g_subclassedRebars)
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(h, RebarSubclassProc);
}

void Wh_ModSettingsChanged()
{
    g_settingDisplayMenuBar = Wh_GetIntSetting(L"DisplayMenuBar");
}

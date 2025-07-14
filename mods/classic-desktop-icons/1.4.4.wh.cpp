// ==WindhawkMod==
// @id              classic-desktop-icons
// @name            Classic Desktop Icons
// @description     Enables the classic selection style on desktop icons.
// @version         1.4.4
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @compilerOptions -luser32 -luxtheme -lcomctl32
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Desktop Icons

This mod will enable classic theme on desktop icons when themes are enabled,
and will optionally let you restore the Windows 2000 behavior of rendering
the desktop color on the items' labels, and will let restore the behavior
of desktop icons prior to Windows 7.

In general, you can use this mod for making your desktop feel like Windows
Vista, Windows XP, Windows 2000, or other old versions of Windows.

## For label backgrounds to show, you must DISABLE desktop icon label shadows.

## Screenshots

**Windows Vista style (modern selection, no margins)**:

![Windows Vista style desktop icons](https://raw.githubusercontent.com/aubymori/images/main/classic-desktop-icons-vista-style.png)

**Windows XP style (shadows, no label backgrounds)**:

![Windows XP style desktop icons](https://raw.githubusercontent.com/aubymori/images/main/classic-desktop-icons-xp-style.png)

**Windows 2000 style (no shadows, label backgrounds)**:

![Windows 2000 style desktop icons](https://raw.githubusercontent.com/aubymori/images/main/classic-desktop-icons-2k-style.png)

*Contributions from [Isabella Lulamoon (kawapure)](//github.com/kawapure).*
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- oldselection: true
  $name: Classic selection effect
  $description: >
    Instead of using a theme bitmap background, selection is shown by changing the color
    of the label background and icon, like Windows XP and prior.
- background: false
  $name: Desktop color as label background
  $description: >
    Renders the desktop color on the labels' backgrounds, like Windows 2000.
    This requires disabling desktop icon shadows in advanced system settings performance
    settings.
- noselect: false
  $name: Non-translucent selection rectangle
  $description: Force the desktop to use a non-translucent selection rectangle, like Windows XP.
- margin_top: 0
  $name: Top margin of the desktop in pixels
  $description: For Windows Vista and prior, this is 0. Since Windows 7, this is 5.
- limit_to_working_space: true
  $name: Limit the desktop view to the working space on single-monitor systems
  $description: > 
    The selection rectangle cannot go behind the taskbar or other appbars (i.e. pinned Sidebar).
    This is the case on single-monitor systems before Windows 7.
*/
// ==/WindhawkModSettings==

#include <synchapi.h>
#include <uxtheme.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <winnt.h>

struct
{
    BOOL fOldSelection;
    BOOL background;
    BOOL noselect;
    BOOL fLimitToWorkingSpace;
    RECT rcMargins;
} settings;

// This is a constant value which has not changed since Windows 7. I'll just cheat
// a little bit and store a copy, because I doubt this will change any time soon.
const RECT g_rcOriginalMargins { 4, 5, 0, 0 };

HWND g_hWndDesktop = NULL;
BOOL g_bSubclassed = FALSE;

bool ShouldLimitWorkingArea()
{
    return settings.fLimitToWorkingSpace && GetSystemMetrics(SM_CMONITORS) <= 1;
}

// Cheap hack:
void *g_pReportDisabledDoubleBufferingListView = nullptr;

bool (*CListView__IsDoubleBuffer_orig)(void *pThis) = nullptr;
bool CListView__IsDoubleBuffer_hook(void *pThis)
{
    if (g_pReportDisabledDoubleBufferingListView == pThis)
    {
        return false;
    }

    return CListView__IsDoubleBuffer_orig(pThis);
}

void (*CLVSelectionManager__DragSelect_orig)(void *pThis, LONG a2, LONG a3) = nullptr;
void CLVSelectionManager__DragSelect_hook(void *pThis, LONG a2, LONG a3)
{
    bool disableMarquee = false;

    size_t *pListView = *((size_t **)pThis + 6);
    HWND hWnd = *((HWND *)pListView + 12);

    if (hWnd == g_hWndDesktop && settings.noselect)
    {
        disableMarquee = true;
    }

    if (disableMarquee)
    {
        // I decided to cheaply store a pointer to the list view to force
        // IsDoubleBuffer to return false, since it was really simple to
        // implement.
        g_pReportDisabledDoubleBufferingListView = pListView;
    }

    CLVSelectionManager__DragSelect_orig(pThis, a2, a3);

    if (disableMarquee)
    {
        // This doesn't have to be cleared at all (in fact, the address probably never
        // changes during the desktop's lifetime), but again, just being safe.
        g_pReportDisabledDoubleBufferingListView = nullptr;
    }
}

void (*CLVDrawManager___PaintWorkArea_orig)(void *pThis, HDC hdcTarget, RECT *pRect) = nullptr;
void CLVDrawManager___PaintWorkArea_hook(void *pThis, HDC hdcTarget, RECT *pRect)
{
    bool disableMarquee = false;

    HWND hWnd = WindowFromDC(hdcTarget);

    if (hWnd == g_hWndDesktop && settings.noselect)
    {
        disableMarquee = true;
    }

    DWORD *pListView = (DWORD *)*((size_t **)pThis + 3);
    bool oldBit = 0;

    if (disableMarquee)
    {
        // I have no idea what this flag does, but setting it on the list view disables
        // the marquee selection effect (leaving nothing).
        oldBit = (*((DWORD *)pListView + 42)) & 0x40000;
        *((DWORD *)pListView + 42) &= ~0x40000;
    }

    CLVDrawManager___PaintWorkArea_orig(pThis, hdcTarget, pRect);

    if (disableMarquee)
    {
        // Just to be safe, I'll restore the old value if it was set.
        if (oldBit)
        {
            *((DWORD *)pListView + 42) |= 0x40000;
        }
    }
}

HMONITOR GetPrimaryMonitor()
{
    POINT pt = {0,0};
    return MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY); 
}

bool GetMonitorRects(HMONITOR hMonitor, LPRECT prc, bool fGetWorkingArea)
{
    if (!hMonitor || !prc)
        return false;

    MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(hMonitor, &mi))
    {
        if (fGetWorkingArea)
        {
            *prc = mi.rcWork;
        }
        else
        {
            *prc = mi.rcMonitor;
        }
        
        return true;
    }
    
    *prc = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
    return true;
}

HRESULT (*CDesktopBrowser__SetDesktopWorkAreas_orig)(void *pThis, HWND hWnd, RECT *pRect);
HRESULT CDesktopBrowser__SetDesktopWorkAreas_hook(void *pThis, HWND hWnd, RECT *pRect)
{
    // If we're limiting the size of the desktop window to the working space, then we
    // don't want to set the work area, since that will duplicate the padding.
    if (ShouldLimitWorkingArea())
    {
        RECT rc = *pRect;
        rc.right -= rc.left;
        rc.bottom -= rc.top;
        rc.left = rc.top = 0;
        return CDesktopBrowser__SetDesktopWorkAreas_orig(pThis, hWnd, &rc);
    }

    return CDesktopBrowser__SetDesktopWorkAreas_orig(pThis, hWnd, pRect);
}

/**
  * UpdateDesktop references DesktopSubclassProc
  * and vice-versa, so we need to define one before
  * the other without defining the function content.
  */
void UpdateDesktop(void);

LRESULT CALLBACK DesktopSubclassProc(
    HWND      hWnd,
    UINT      uMsg,
    WPARAM    wParam,
    LPARAM    lParam,
    DWORD_PTR dwRefData
)
{
    if (uMsg == WM_SETREDRAW || uMsg == LVM_GETITEMCOUNT || uMsg == WM_SIZE)
    {
        UpdateDesktop();
    }

    if (settings.background && uMsg == LVM_SETTEXTBKCOLOR)
    {
        lParam = GetSysColor(COLOR_BACKGROUND);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Undocumented list view messages:
// https://www.geoffchappell.com/studies/windows/shell/comctl32/controls/listview/messages/index.htm
#define LVM_SETVIEWMARGINS 0x105A
#define LVM_GETVIEWMARGINS 0x105B

void UpdateDesktop(void)
{
    if (g_hWndDesktop != NULL)
    {
        /* Untheme the desktop */
        if (settings.fOldSelection)
        {
            SetWindowTheme(g_hWndDesktop, NULL, NULL);
            SendMessageW(g_hWndDesktop, WM_THEMECHANGED, 0, 0);
        }

        // Set the margin of the desktop view:
        SendMessageW(g_hWndDesktop, LVM_SETVIEWMARGINS, 0, (LPARAM)&settings.rcMargins);

        /* Apply the desktop background color */
        if (settings.background)
        {
            SendMessageW(
                g_hWndDesktop,
                LVM_SETTEXTBKCOLOR,
                NULL,
                GetSysColor(COLOR_BACKGROUND)
            );
        }

        // Update the size if we're limiting to the working space:
        if (ShouldLimitWorkingArea())
        {
            RECT rcWorkArea;
            if (GetMonitorRects(GetPrimaryMonitor(), &rcWorkArea, true))
            {
                SetWindowPos(
                    // We want to change the position of the DefView window and not the
                    // SysListView32, or else the margin will be doubled on left and top
                    // taskbars (for some reason).
                    GetParent(g_hWndDesktop),
                    nullptr,
                    rcWorkArea.left,
                    rcWorkArea.top,
                    rcWorkArea.right - rcWorkArea.left,
                    rcWorkArea.bottom - rcWorkArea.top,
                    SWP_NOZORDER | SWP_NOACTIVATE
                );
            }
        }

        /* Subclass to update label backgrounds (they get removed normally) */
        if (!g_bSubclassed)
        {
            g_bSubclassed = WindhawkUtils::SetWindowSubclassFromAnyThread(g_hWndDesktop, DesktopSubclassProc, NULL);
        }
    }
}

BOOL CALLBACK FindDesktopEnumProc(HWND hWnd, LPARAM lParam)
{
    WCHAR szClassName[256] = { 0 };
    GetClassNameW(hWnd, szClassName, 256);
    if (0 == wcscmp(szClassName, L"WorkerW"))
    {
        HWND hwndDefView = FindWindowExW(hWnd, 0, L"SHELLDLL_DefView", NULL);
        if (hwndDefView)
        {
            HWND hwndDesktop = FindWindowExW(hwndDefView, 0, L"SysListView32", NULL);
            if (hwndDesktop)
            {
                DWORD dwPID;
                GetWindowThreadProcessId(hwndDesktop, &dwPID);
                if (dwPID == GetCurrentProcessId())
                {
                    *(HWND *)lParam = hwndDesktop;
                    return FALSE;
                }
            }
        }
    }
    return TRUE;
}

HWND FindDesktopWindow(void)
{
    HWND hwndProgman = FindWindowW(L"Progman", L"Program Manager");
    if (hwndProgman)
    {
        HWND hwndDefView = FindWindowExW(hwndProgman, 0, L"SHELLDLL_DefView", NULL);
        if (hwndDefView)
        {
            HWND hwndDesktop = FindWindowExW(hwndDefView, 0, L"SysListView32", NULL);
            if (hwndDesktop)
            {
                DWORD dwPID;
                GetWindowThreadProcessId(hwndDesktop, &dwPID);
                if (dwPID == GetCurrentProcessId())
                    return hwndDesktop;
            }
        }
        else
        {
            HWND hwndDesktop = NULL;
            EnumDesktopWindows(
                GetThreadDesktop(GetCurrentThreadId()),
                FindDesktopEnumProc,
                (LPARAM)&hwndDesktop
            );
            if (hwndDesktop)
                return hwndDesktop;
        }
    }
    return NULL;
}

#define TextualClassName(x) (((ULONG_PTR)x & ~(ULONG_PTR)0xffff) != 0)

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_orig;
HWND WINAPI CreateWindowExW_hook(
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
    LPVOID    lpParam
)
{
    HWND hRes = CreateWindowExW_orig(
        dwExStyle, lpClassName, lpWindowName,
        dwStyle, X, Y, nWidth, nHeight, hWndParent,
        hMenu, hInstance, lpParam
    );

    /**
      * Check that the following criteria is met:
      * - The desktop is not already set
      * - The window has a parent
      * - lpClassName is non-null and not a bad pointer (MANY windows pass a bad pointer to lpClassName)
      * - The window's class name is "SysListView32"
      * - The window's parent's class name is "SHELLDLL_DefView"
      * - The root window's class name is "Progman"
      */
    if (g_hWndDesktop == NULL
    && hWndParent != NULL
    && lpClassName != NULL
    && TextualClassName(lpClassName))
    {
        if (0 == wcscmp(L"SysListView32", lpClassName))
        {
            WCHAR lpPrntCls[256];
            GetClassNameW(hWndParent, lpPrntCls, 256);

            if (0 == wcscmp(lpPrntCls, L"SHELLDLL_DefView"))
            {
                HWND hwndRoot = GetAncestor(hWndParent, GA_ROOT);
                GetClassNameW(hwndRoot, lpPrntCls, 256);
                if (0 == wcscmp(lpPrntCls, L"Progman"))
                {
                    g_hWndDesktop = hRes;
                    UpdateDesktop();
                }
            }
        }
    }

    return hRes;
}

void LoadSettings(void)
{
    settings.fOldSelection = Wh_GetIntSetting(L"oldselection");
    settings.background = Wh_GetIntSetting(L"background");
    settings.noselect = Wh_GetIntSetting(L"noselect");
    settings.fLimitToWorkingSpace = Wh_GetIntSetting(L"limit_to_working_space");

    settings.rcMargins = { 0 };
    settings.rcMargins.top = Wh_GetIntSetting(L"margin_top");
}

// Hooks used for removing the transparent selection ("marquee") rectangle
// on only the desktop window.
const WindhawkUtils::SYMBOL_HOOK comctl32DllHooks[] = {
    {
        // Manages drawing of the marquee selection, needs to be disabled
        {
            L"private: void __cdecl CLVDrawManager::_PaintWorkArea(struct HDC__ *,struct tagRECT const *)"
        },
        &CLVDrawManager___PaintWorkArea_orig,
        CLVDrawManager___PaintWorkArea_hook
    },
    {
        // Manages drawing of the classic rectangle, needs to be enabled (see IsDoubleBuffer)
        {
            L"public: void __cdecl CLVSelectionManager::DragSelect(int,int)"
        },
        &CLVSelectionManager__DragSelect_orig,
        CLVSelectionManager__DragSelect_hook
    },
    {
        // Technically unrelated, but hooked in order to change branching of DragSelect
        // without reassembling
        {
            L"public: bool __cdecl CListView::IsDoubleBuffer(void)const "
        },
        &CListView__IsDoubleBuffer_orig,
        CListView__IsDoubleBuffer_hook
    }
};

const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[] = {
    {
        // Manages the working area of the desktop list view. We hook this to avoid double
        // padding when using the legacy behaviour.
        {
            L"protected: long __cdecl CDesktopBrowser::SetDesktopWorkAreas(struct HWND__ *,struct tagRECT *)"
        },
        &CDesktopBrowser__SetDesktopWorkAreas_orig,
        CDesktopBrowser__SetDesktopWorkAreas_hook
    },
};

BOOL Wh_ModInit(void) 
{
    LoadSettings();

    HMODULE hComCtl = LoadLibraryExW(L"comctl32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hComCtl)
    {
        Wh_Log(L"Failed to load comctl32.dll");
        return FALSE;
    }

    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");

    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    Wh_SetFunctionHook(
        (void *)CreateWindowExW,
        (void *)CreateWindowExW_hook,
        (void **)&CreateWindowExW_orig
    );

    if (settings.noselect)
    {
        if (!WindhawkUtils::HookSymbols(
            hComCtl,
            comctl32DllHooks,
            ARRAYSIZE(comctl32DllHooks)
        ))
        {
            Wh_Log(L"Failed to hook one or more symbol functions in comctl32.dll");
            return FALSE;
        };
    }

    if (!WindhawkUtils::HookSymbols(
        hShell32,
        shell32DllHooks,
        ARRAYSIZE(shell32DllHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in shell32.dll");
        return FALSE;
    };

    /* Initially update the desktop if it already exists. */
    g_hWndDesktop = FindDesktopWindow();
    if (g_hWndDesktop != NULL)
    {
        UpdateDesktop();
    }

    return TRUE;
}

void Wh_ModAfterInit(void)
{
    /* Initially update the desktop if it already exists. */
    g_hWndDesktop = FindDesktopWindow();
    if (g_hWndDesktop != NULL)
    {
        // Since this means that Explorer is already up, and we're initialising
        // the mod, we need to correct the working area if the user wishes.
        // We cannot broadcast this message as Explorer is starting up, as it seems
        // to have a tendency to cause an access violation within shell32.
        if (ShouldLimitWorkingArea())
        {
            SendMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);
        }
    }
}

void Wh_ModUninit(void)
{
    // Unlimit and update the working space.
    if (settings.fLimitToWorkingSpace)
    {
        settings.fLimitToWorkingSpace = false;
        SendMessageW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0);
    }

    /* Remove subclass */
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_hWndDesktop, DesktopSubclassProc);

    /* Remove desktop background color */
    if (settings.background)
    {
        SendMessageW(
            g_hWndDesktop,
            LVM_SETTEXTBKCOLOR,
            NULL,
            CLR_NONE
        );
    }

    // Restore the original desktop margins:
    SendMessageW(g_hWndDesktop, LVM_SETVIEWMARGINS, 0, (LPARAM)&g_rcOriginalMargins);

    /* Retheme desktop */
    SetWindowTheme(g_hWndDesktop, L"Desktop", NULL);
    Wh_Log(L"Unloaded Classic Desktop Icons");
}

BOOL Wh_ModSettingsChanged(BOOL *bReload)
{
    *bReload = TRUE;
    return TRUE;
}
// ==WindhawkMod==
// @id              classic-desktop-icons
// @name            Classic Desktop Icons
// @description     Enables the classic selection style on desktop icons.
// @version         1.3.0
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
the desktop color on the items' labels.

## For label backgrounds to show, you must DISABLE desktop icon label shadows.

## Screenshots

**Windows XP style (shadows, no label backgrounds)**:

![Windows XP style desktop icons](https://raw.githubusercontent.com/aubymori/images/main/classic-desktop-icons-xp-style.png)

**Windows 2000 style (no shadows, label backgrounds)**:

![Windows 2000 style desktop icons](https://raw.githubusercontent.com/aubymori/images/main/classic-desktop-icons-2k-style.png)

*Mod originally authored by Taniko Yamamoto.*
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- background: false
  $name: Desktop color as label background
  $description: Renders the desktop color on the labels' backgrounds, like Windows 2000.
- noselect: false
  $name: Non-translucent selection rectangle
  $description: Force the desktop to use a non-translucent selection rectangle, like Windows XP.
*/
// ==/WindhawkModSettings==

#include <uxtheme.h>
#include <windhawk_utils.h>

struct
{
    BOOL background;
    BOOL noselect;
} settings;

HWND g_hWndDesktop = NULL;
BOOL g_bSubclassed = FALSE;

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
    if (uMsg == WM_SETREDRAW || uMsg == LVM_GETITEMCOUNT)
    {
        UpdateDesktop();
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void UpdateDesktop(void)
{
    if (g_hWndDesktop != NULL)
    {
        /* Untheme the desktop */
        SetWindowTheme(g_hWndDesktop, NULL, NULL);
        SendMessageW(g_hWndDesktop, WM_THEMECHANGED, 0, 0);

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

        /* Subclass to update label backgrounds (they get removed normally) */
        if (!g_bSubclassed)
        {
            g_bSubclassed = WindhawkUtils::SetWindowSubclassFromAnyThread(g_hWndDesktop, DesktopSubclassProc, NULL);
        }
    }
}

HWND FindDesktopWindow(void)
{
    HWND baseWindow = FindWindowW(L"Progman", L"Program Manager");
    if (baseWindow)
    {
        HWND defView = FindWindowExW(baseWindow, 0, L"SHELLDLL_DefView", NULL);
        if (defView)
        {
            HWND desktop = FindWindowExW(defView, 0, L"SysListView32", NULL);

            if (desktop)
            {
                return desktop;
            }
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
                g_hWndDesktop = hRes;
                UpdateDesktop();
            }
        }
    }

    return hRes;
}

void LoadSettings(void)
{
    settings.background = Wh_GetIntSetting(L"background");
    settings.noselect = Wh_GetIntSetting(L"noselect");
}

#ifdef _WIN64
#   define PATHCACHE_VALNAME L"last-comctl32-v6-path"
#else
#   define PATHCACHE_VALNAME L"last-comctl32-v6-path-wow64"
#endif

#define COMCTL_582_SEARCH    L"microsoft.windows.common-controls_6595b64144ccf1df_5.82"

/* Load the ComCtl32 module */
HMODULE LoadComCtlModule(void)
{
    HMODULE hComCtl = LoadLibraryW(L"comctl32.dll");
    if (!hComCtl)
    {
        return NULL;
    }

    WCHAR szPath[MAX_PATH];
    GetModuleFileNameW(hComCtl, szPath, MAX_PATH);

    WCHAR szv6Path[MAX_PATH];
    BOOL bNoCache = FALSE;
    if (!Wh_GetStringValue(PATHCACHE_VALNAME, szv6Path, MAX_PATH))
    {
        bNoCache = TRUE;
    }

    /**
      * the !bNoCache check here is nested because we only want to fall through
      * to the cacher if the current comctl32 path is NOT 5.82.
      */
    if (wcsstr(szPath, COMCTL_582_SEARCH)
    || wcsstr(szPath, L"\\Windows\\System32")
    || wcsstr(szPath, L"\\Windows\\SysWOW64"))
    {
        if (!bNoCache)
        {
            hComCtl = LoadLibraryW(szv6Path);
        }
    }
    else if (bNoCache || wcsicmp(szPath, szv6Path))
    {
        Wh_SetStringValue(PATHCACHE_VALNAME, szPath);
    }

    return hComCtl;
}

// Hooks used for removing the transparent selection ("marquee") rectangle
// on only the desktop window.
const WindhawkUtils::SYMBOL_HOOK marqueeHooks[] = {
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

BOOL Wh_ModInit(void) 
{
    LoadSettings();

    HMODULE hComCtl = LoadComCtlModule();

    if (!hComCtl)
    {
        Wh_Log(L"Failed to load comctl32.dll");
        return FALSE;
    }

    /* Initially update the desktop if it already exists. */
    g_hWndDesktop = FindDesktopWindow();
    if (g_hWndDesktop != NULL)
    {
        UpdateDesktop();
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
            marqueeHooks,
            ARRAYSIZE(marqueeHooks)
        ))
        {
            Wh_Log(L"Failed to hook one or more symbol functions in comctl32.dll");
            return FALSE;
        };
    }

    return TRUE;
}

void Wh_ModUninit(void)
{
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

    /* Retheme desktop */
    SetWindowTheme(g_hWndDesktop, L"Desktop", NULL);
    Wh_Log(L"Unloaded Classic Desktop Icons");
}

BOOL Wh_ModSettingsChanged(BOOL *bReload)
{
    *bReload = TRUE;
    return TRUE;
}
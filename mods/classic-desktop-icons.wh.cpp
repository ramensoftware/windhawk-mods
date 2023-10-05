// ==WindhawkMod==
// @id              classic-desktop-icons
// @name            Classic Desktop Icons
// @description     Enables the classic selection style on desktop icons.
// @version         1.2.1
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @compilerOptions -luser32 -luxtheme -lcomctl32
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

HWND hDesktop = NULL;
BOOL bSubclassed = FALSE;

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
    if (hDesktop != NULL)
    {
        /* Untheme the desktop */
        SetWindowTheme(hDesktop, NULL, NULL);
        SendMessageW(hDesktop, WM_THEMECHANGED, 0, 0);

        /* Apply the desktop background color */
        if (settings.background)
        {
            SendMessageW(
                hDesktop,
                LVM_SETTEXTBKCOLOR,
                NULL,
                GetSysColor(COLOR_BACKGROUND)
            );
        }

        /* Force non-translucent selection rectangle */
        if (settings.noselect)
        {
            SendMessageW(
                hDesktop,
                LVM_SETEXTENDEDLISTVIEWSTYLE,
                LVS_EX_DOUBLEBUFFER,
                FALSE
            );
        }

        /* Subclass to update label backgrounds (they get removed normally) */
        if (!bSubclassed)
        {
            bSubclassed = WindhawkUtils::SetWindowSubclassFromAnyThread(hDesktop, DesktopSubclassProc, NULL);
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
    if (hDesktop == NULL
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
                hDesktop = hRes;
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

BOOL Wh_ModInit(void) 
{
    Wh_Log(L"Initializing Classic Desktop Icons");
    LoadSettings();

    /* Initially update the desktop if it already exists. */
    hDesktop = FindDesktopWindow();
    if (hDesktop != NULL)
    {
        UpdateDesktop();
    }

    Wh_SetFunctionHook(
        (void *)CreateWindowExW,
        (void *)CreateWindowExW_hook,
        (void **)&CreateWindowExW_orig
    );

    Wh_Log(L"Done initializing Classic Desktop Icons");
    return TRUE;
}

void Wh_ModUninit(void)
{
    /* Remove subclass*/
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hDesktop, DesktopSubclassProc);

    /* Remove desktop background color */
    if (settings.background)
    {
        SendMessageW(
            hDesktop,
            LVM_SETTEXTBKCOLOR,
            NULL,
            CLR_NONE
        );
    }

    /* Make selection rectangle translucent again */
    if (settings.noselect)
    {
        SendMessageW(
            hDesktop,
            LVM_SETEXTENDEDLISTVIEWSTYLE,
            LVS_EX_DOUBLEBUFFER,
            TRUE
        );
    }

    /* Retheme desktop */
    SetWindowTheme(hDesktop, L"Desktop", NULL);
    Wh_Log(L"Unloaded Classic Desktop Icons");
}

BOOL Wh_ModSettingsChanged(BOOL *bReload)
{
    *bReload = TRUE;
    return TRUE;
}

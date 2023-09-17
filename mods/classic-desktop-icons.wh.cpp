// ==WindhawkMod==
// @id              classic-desktop-icons
// @name            Classic Desktop Icons
// @description     Enables the classic selection style on desktop icons.
// @version         1.0.0
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
*/
// ==/WindhawkModSettings==

#include <uxtheme.h>
#include <windhawk_utils.h>

struct
{
    BOOL background;
} settings;

HWND hDesktop;

#define LABELBG settings.background     \
        ? GetSysColor(COLOR_BACKGROUND) \
        : CLR_NONE

void UpdateDesktop(void)
{
    if (hDesktop != NULL)
    {
        // Untheme the desktop
        SetWindowTheme(hDesktop, NULL, NULL);
        SendMessageW(hDesktop, WM_THEMECHANGED, 0, 0);

        // Apply the desktop background color 
        DWORD dwBgColor = LABELBG;

        SendMessageW(hDesktop, LVM_SETTEXTBKCOLOR, NULL, (LPARAM)dwBgColor);
    }
}

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

void LoadSettings(void)
{
    settings.background = Wh_GetIntSetting(L"background");
}

BOOL Wh_ModInit(void) 
{
    Wh_Log(L"Initializing Classic Desktop Icons");
    LoadSettings();

    // Initially update the desktop.
    hDesktop = FindDesktopWindow();
    UpdateDesktop();

    // Subclass to update label backgrounds (they get removed normally)
    WindhawkUtils::SetWindowSubclassFromAnyThread(hDesktop, DesktopSubclassProc, NULL);

    Wh_Log(L"Done initializing Classic Desktop Icons");
    return TRUE;
}

void Wh_ModUninit(void)
{
    // Theme the desktop icons again, and remove any text
    // background that was set
    // Also remove the subclass we set
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hDesktop, DesktopSubclassProc);
    SendMessageW(hDesktop, LVM_SETTEXTBKCOLOR, NULL, CLR_NONE);
    SetWindowTheme(hDesktop, L"Desktop", NULL);
    Wh_Log(L"Unloaded Classic Desktop Icons");
}

BOOL Wh_ModSettingsChanged(BOOL *bReload)
{
    *bReload = TRUE;
    return TRUE;
}
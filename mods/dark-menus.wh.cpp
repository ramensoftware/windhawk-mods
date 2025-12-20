// ==WindhawkMod==
// @id                dark-menus
// @version           1.3.3
// @author            Mgg Sk
// @github            https://github.com/MGGSK
// @include           *
// @compilerOptions   -lUxTheme -lGdi32

// @name              Dark mode context menus
// @description       Enables dark mode for all win32 menus.

// @name:de-DE        Dunkler Modus für Menüs
// @description:de-DE Aktiviert den Dunklen Modus für alle Win32 Menüs.
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Dark mode context menus
Forces dark mode for all win32 menus to create a more consistent UI. Requires Windows 10 build 18362 or later.

### Before:
![Before](https://i.imgur.com/bGRVJz8.png)
![Before10](https://i.imgur.com/FGyph05.png)
![MenubarBefore](https://raw.githubusercontent.com/MGGSK/DarkMenus/main/Images/menubar_before.png)

### After:
![After](https://i.imgur.com/BURKEki.png)
![After10](https://i.imgur.com/MUqVAcG.png)
![Menubar](https://raw.githubusercontent.com/MGGSK/DarkMenus/main/Images/menubar.png)

The code for dark menubars is based on [win32-darkmode](https://github.com/adzm/win32-darkmode) and [notepad++](https://github.com/notepad-plus-plus/notepad-plus-plus). Create a [issue](https://github.com/MGGSK/DarkMenus/issues) or [discussion](https://github.com/MGGSK/DarkMenus/discussions) to send feedback.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- AppMode: ForceDark
  $name: When do you want context menus in dark mode?
  $description: Set to "Only when Windows is in dark mode" if you switch between light and dark mode a lot or are using a tool like Auto Dark Mode.
  $options:
  - ForceDark: Always
  - AllowDark: Only when Windows is in dark mode

  $name:de-DE: Wann soll der Dunkle Modus aktiviert sein?
  $description:de-DE: Setzen sie auf "Systemeinstellung verwenden" wenn sie oft zwischen den Dunklen und Hellen Modus wechseln oder ein Program wie Auto Dark Mode verwenden.
  $options:de-DE:
  - ForceDark: Immer
  - AllowDark: Systemeinstellung verwenden
*/
// ==/WindhawkModSettings==

#include <uxtheme.h>
#include <vsstyle.h>
#include <windows.h>

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <winnt.h>

const COLORREF crItemForeground = 0xFFFFFF;
const COLORREF crItemDisabled = 0xAAAAAA;
const HBRUSH brBackground = CreateSolidBrush(0x262626);
const HBRUSH brItemBackgroundHot = CreateSolidBrush(0x353535);
const HBRUSH brItemBackgroundSelected = CreateSolidBrush(0x454545);

#define WM_UAHDRAWMENU         0x91
#define WM_UAHDRAWMENUITEM     0x92

union UAHMENUITEMMETRICS
{
	struct {
		DWORD cx;
		DWORD cy;
	} rgsizeBar[2];
	struct {
		DWORD cx;
		DWORD cy;
	} rgsizePopup[4];
};

struct UAHMENUPOPUPMETRICS
{
	DWORD rgcx[4];
	DWORD fUpdateMaxWidths : 2;
};

struct UAHMENU
{
	HMENU hMenu;
	HDC hdc;
	DWORD dwFlags;
};

struct UAHMENUITEM
{
	int iPosition;
	UAHMENUITEMMETRICS umim;
	UAHMENUPOPUPMETRICS umpm;
};

struct UAHDRAWMENUITEM
{
	DRAWITEMSTRUCT dis;
	UAHMENU um;
	UAHMENUITEM umi;
};

//Code based on : https://github.com/notepad-plus-plus/notepad-plus-plus/blob/bab3573be708bb908b8080e3e2007ea78a7f1932/PowerEditor/src/NppDarkMode.cpp
#pragma region CodeBasedOnNotepad++

void DrawUAHMenuNCBottomLine(HWND hWnd)
{
	MENUBARINFO mbi = { sizeof(mbi) };
	if (!GetMenuBarInfo(hWnd, OBJID_MENU, 0, &mbi))
		return;

	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	MapWindowPoints(hWnd, nullptr, (LPPOINT)&rcClient, 2);

	RECT rcWindow;
	GetWindowRect(hWnd, &rcWindow);

	OffsetRect(&rcClient, -rcWindow.left, -rcWindow.top);

	// the rcBar is offset by the window rect
	RECT rcAnnoyingLine = rcClient;
	rcAnnoyingLine.bottom = rcAnnoyingLine.top;
	rcAnnoyingLine.top--;

	HDC hdc = GetWindowDC(hWnd);
	FillRect(hdc, &rcAnnoyingLine, brBackground);
	ReleaseDC(hWnd, hdc);
}

#pragma endregion CodeBasedOnNotepad++

//Code based on https://github.com/adzm/win32-darkmode/blob/darkmenubar/win32-darkmode/win32-darkmode.cpp
#pragma region CodeBasedOnWin32DarkMode

thread_local HTHEME g_menuTheme = nullptr;

//Processes messages related to custom menubar drawing.
//Returns true if handled, false to continue with normal processing
bool CALLBACK UAHWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* lpResult)
{
    switch (uMsg)
    {
    case WM_UAHDRAWMENU:
    {
        const auto* pUdm = (UAHMENU*)lParam;
        RECT rc;

        //Get the menubar rect
        MENUBARINFO mbi { sizeof(MENUBARINFO) };
        GetMenuBarInfo(hWnd, OBJID_MENU, 0, &mbi);

        RECT rcWindow;
        GetWindowRect(hWnd, &rcWindow);

        //The rcBar is offset by the window rect
        rc = mbi.rcBar;
        OffsetRect(&rc, -rcWindow.left, -rcWindow.top);

        FillRect(pUdm->hdc, &rc, brBackground);

        return true;
    }
    case WM_UAHDRAWMENUITEM:
    {
        auto* pUDMI = (UAHDRAWMENUITEM*)lParam;
        const HBRUSH* pbrBackground = &brBackground;

        //Get the menu item string
        wchar_t menuString[256] = { 0 };
        MENUITEMINFO mii = { sizeof(mii), MIIM_STRING };
        {
            mii.dwTypeData = menuString;
            mii.cch = (sizeof(menuString) / 2) - 1;

            GetMenuItemInfoW(pUDMI->um.hMenu, pUDMI->umi.iPosition, TRUE, &mii);
        }

        //Get the item state for drawing
        DWORD dwFlags = DT_CENTER | DT_SINGLELINE | DT_VCENTER;

        int iTextStateID = 0;

        if ((pUDMI->dis.itemState & ODS_INACTIVE) || (pUDMI->dis.itemState & ODS_DEFAULT)) 
            iTextStateID = MBI_NORMAL;
        else if (pUDMI->dis.itemState & ODS_HOTLIGHT)
            iTextStateID = MBI_HOT;
        else if (pUDMI->dis.itemState & ODS_SELECTED)
            iTextStateID = MBI_PUSHED;
        else if ((pUDMI->dis.itemState & ODS_GRAYED) || (pUDMI->dis.itemState & ODS_DISABLED))
            iTextStateID = MBI_DISABLED;

        if (pUDMI->dis.itemState & ODS_HOTLIGHT)
            pbrBackground = &brItemBackgroundHot;
        else if (pUDMI->dis.itemState & ODS_SELECTED)
            pbrBackground = &brItemBackgroundSelected;

        if (pUDMI->dis.itemState & ODS_NOACCEL)
            dwFlags |= DT_HIDEPREFIX;

        if (!g_menuTheme)
            g_menuTheme = OpenThemeData(hWnd, L"Menu");

        const DTTOPTS opts { sizeof(opts), DTT_TEXTCOLOR, iTextStateID != MBI_DISABLED ? crItemForeground : crItemDisabled };

        FillRect(pUDMI->um.hdc, &pUDMI->dis.rcItem, *pbrBackground);
        DrawThemeTextEx(g_menuTheme, pUDMI->um.hdc, MENU_BARITEM, MBI_NORMAL, menuString, mii.cch, dwFlags, &pUDMI->dis.rcItem, &opts);

        return true;
    }
    case WM_THEMECHANGED:
    {
        CloseThemeData(g_menuTheme);
        g_menuTheme = nullptr;
        return false;
    }
    default:
        return false;
    }
}
#pragma endregion

using RtlGetNtVersionNumbers_T = void (WINAPI *)(LPDWORD, LPDWORD, LPDWORD);
RtlGetNtVersionNumbers_T RtlGetNtVersionNumbers;

using DefWindowProcW_T = decltype(&DefWindowProcW);
DefWindowProcW_T DefWindowProcW_Original;

enum class AppMode
{
	Default,
	AllowDark,
	ForceDark,
	ForceLight,
	Max
};

using FlushMenuThemes_T = void(WINAPI*)();
using SetPreferredAppMode_T = AppMode(WINAPI*)(AppMode);
using ShouldAppsUseDarkMode_T = bool(WINAPI*)();

FlushMenuThemes_T FlushMenuThemes;
SetPreferredAppMode_T SetPreferredAppMode;
ShouldAppsUseDarkMode_T ShouldAppsUseDarkMode;

AppMode g_currentAppMode;

LRESULT CALLBACK DefWindowProcW_Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if((g_currentAppMode == AppMode::ForceDark || (g_currentAppMode == AppMode::AllowDark && ShouldAppsUseDarkMode())) && GetMenu(hWnd))
    {
        LRESULT lResult = 0;
        if(UAHWndProc(hWnd, uMsg, wParam, lParam, &lResult))
            return lResult;
    
        if(uMsg == WM_ACTIVATE || uMsg == WM_NCPAINT)
        {
            lResult = DefWindowProcW_Original(hWnd, uMsg, wParam, lParam);
            DrawUAHMenuNCBottomLine(hWnd);
            return lResult;
        }
    }

    return DefWindowProcW_Original(hWnd, uMsg, wParam, lParam);
}

decltype(&SetMenuInfo) SetMenuInfo_Original;
WINBOOL WINAPI SetMenuInfo_Hook(HMENU hMenu, LPCMENUINFO lpInfo)
{
    LPMENUINFO lpMenuInfo = const_cast<LPMENUINFO>(lpInfo);
    lpMenuInfo->hbrBack = nullptr; //Disable custom menu backgrounds because they are broken in dark mode. (See https://github.com/MGGSK/DarkMenus/issues/16)
    return SetMenuInfo_Original(hMenu, lpInfo);
}

//Applies the theme to all menus.
void ApplyTheme(const AppMode inputTheme = AppMode::Max)
{
    AppMode theme = inputTheme;

    //Get the saved theme from the settings.
    if(theme == AppMode::Max)
    {
        const PCWSTR savedTheme = Wh_GetStringSetting(L"AppMode");

        if(wcscmp(savedTheme, L"AllowDark") == 0)
            theme = AppMode::AllowDark;
        else if(wcscmp(savedTheme, L"ForceLight") == 0)
            theme = AppMode::ForceLight;
        else
            theme = AppMode::ForceDark;

        Wh_FreeStringSetting(savedTheme);
    }

    //Apply the theme
    FlushMenuThemes();
    SetPreferredAppMode(theme);

    g_currentAppMode = theme;
}

//Checks if the windows build is 18362 or later.
bool IsAPISupported()
{
    if(!RtlGetNtVersionNumbers)
    {
        const HMODULE hNtDll = LoadLibraryExW(L"ntdll.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        const FARPROC pRtlGetNtVersionNumbers = GetProcAddress(hNtDll, "RtlGetNtVersionNumbers");
	    RtlGetNtVersionNumbers = reinterpret_cast<RtlGetNtVersionNumbers_T>(pRtlGetNtVersionNumbers);
    }

    DWORD build;
    RtlGetNtVersionNumbers(nullptr, nullptr, &build);

    build &= ~0xF0000000;
    return build >= 18362;
}

//Updates the theme of the window when the settings change.
void Wh_ModSettingsChanged()
{
    Wh_Log(L"Settings changed");
    ApplyTheme();
}

//Import functions
BOOL Wh_ModInit()
{
    if(!IsAPISupported())
    {
        Wh_Log(L"Outdated Windows version!");
        return FALSE;
    }

    Wh_Log(L"Init");

    if(!WindhawkUtils::SetFunctionHook(DefWindowProcW, DefWindowProcW_Hook, &DefWindowProcW_Original))
        return FALSE;

    if(!WindhawkUtils::SetFunctionHook(SetMenuInfo, SetMenuInfo_Hook, &SetMenuInfo_Original))
        return FALSE;

    const HMODULE hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    const FARPROC pSetPreferredAppMode = GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135));
    const FARPROC pFlushMenuThemes = GetProcAddress(hUxtheme, MAKEINTRESOURCEA(136));
    const FARPROC pShouldAppsUseDarkMode = GetProcAddress(hUxtheme, MAKEINTRESOURCEA(132));

    SetPreferredAppMode = reinterpret_cast<SetPreferredAppMode_T>(pSetPreferredAppMode);
    FlushMenuThemes = reinterpret_cast<FlushMenuThemes_T>(pFlushMenuThemes);
    ShouldAppsUseDarkMode = reinterpret_cast<ShouldAppsUseDarkMode_T>(pShouldAppsUseDarkMode);

    ApplyTheme();
    return TRUE;
}

//Fixes https://github.com/MGGSK/DarkMenus/issues/9
bool IsSystemCallDisableMitigationEnabled() 
{
    PROCESS_MITIGATION_SYSTEM_CALL_DISABLE_POLICY policy{};
    return GetProcessMitigationPolicy(GetCurrentProcess(), ProcessSystemCallDisablePolicy, &policy, sizeof(policy))
        && policy.DisallowWin32kSystemCalls != 0;
}

//Restores the default theme.
void Wh_ModUninit()
{
    Wh_Log(L"Restoring the default theme.");
    ApplyTheme(AppMode::Default);

    if(!IsSystemCallDisableMitigationEnabled())
    {
        DeleteObject(brBackground);
        DeleteObject(brItemBackgroundHot);
        DeleteObject(brItemBackgroundSelected);
    }
}

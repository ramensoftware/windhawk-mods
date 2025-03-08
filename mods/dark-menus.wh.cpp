// ==WindhawkMod==
// @id                dark-menus
// @version           1.3
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

#### The code for dark menubars is based on the [win32-darkmode](https://github.com/adzm/win32-darkmode) repository.
#### Create a [issue](https://github.com/MGGSK/DarkMenus/issues) or [discussion](https://github.com/MGGSK/DarkMenus/discussions) to send feedback.
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
  - ForceLight: Never

  $name:de-DE: Wann soll der Dunkle Modus aktiviert sein?
  $description:de-DE: Setzen sie auf "Systemeinstellung verwenden" wenn sie oft zwischen den Dunklen und Hellen Modus wechseln oder ein Program wie Auto Dark Mode verwenden.
  $options:de-DE:
  - ForceDark: Immer
  - AllowDark: Systemeinstellung verwenden
  - ForceLight: Nie
*/
// ==/WindhawkModSettings==

#include <uxtheme.h>
#include <vsstyle.h>
#include <windows.h>

#include <windhawk_api.h>

const COLORREF crItemForeground = 0xFFFFFF;
const COLORREF crItemDisabled = 0xAAAAAA;
const HBRUSH brBackground = CreateSolidBrush(0x262626);
const HBRUSH brItemBackgroundHot = CreateSolidBrush(0x353535);
const HBRUSH brItemBackgroundSelected = CreateSolidBrush(0x454545);

//Code taken from https://github.com/adzm/win32-darkmode/blob/darkmenubar/win32-darkmode/UAHMenuBar.h and https://github.com/adzm/win32-darkmode/blob/darkmenubar/win32-darkmode/win32-darkmode.cpp
//MIT license, see LICENSE
//Copyright(c) 2021 adzm / Adam D. Walling
#pragma region UAHMenuBar

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

HTHEME menuTheme = nullptr;

//Processes messages related to UAH / custom menubar drawing.
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

        if (!menuTheme)
            menuTheme = OpenThemeData(hWnd, L"Menu");

        const DTTOPTS opts { sizeof(opts), DTT_TEXTCOLOR, iTextStateID != MBI_DISABLED ? crItemForeground : crItemDisabled };

        FillRect(pUDMI->um.hdc, &pUDMI->dis.rcItem, *pbrBackground);
        DrawThemeTextEx(menuTheme, pUDMI->um.hdc, MENU_BARITEM, MBI_NORMAL, menuString, mii.cch, dwFlags, &pUDMI->dis.rcItem, &opts);

        return true;
    }
    case WM_THEMECHANGED:
    {
        CloseThemeData(menuTheme);
        menuTheme = nullptr;
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

LRESULT CALLBACK DefWindowProcW_Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = 0;
    if(UAHWndProc(hWnd, uMsg, wParam, lParam, &lResult))
        return lResult;

    return DefWindowProcW_Original(hWnd, uMsg, wParam, lParam);
}

enum AppMode
{
	Default,
	AllowDark,
	ForceDark,
	ForceLight,
	Max
};

using FlushMenuThemes_T = void (WINAPI *)();
using SetPreferredAppMode_T = HRESULT (WINAPI *)(AppMode);

FlushMenuThemes_T FlushMenuThemes;
SetPreferredAppMode_T SetPreferredAppMode;

//Applies the theme to all menus.
HRESULT ApplyTheme(const AppMode inputTheme = Max)
{
    AppMode theme = inputTheme;

    //Get the saved theme from the settings.
    if(theme == Max)
    {
        const PCWSTR savedTheme = Wh_GetStringSetting(L"AppMode");

        if(wcscmp(savedTheme, L"AllowDark") == 0)
            theme = AllowDark;
        else if(wcscmp(savedTheme, L"ForceLight") == 0)
            theme = ForceLight;
        else
            theme = ForceDark;

        Wh_FreeStringSetting(savedTheme);
    }

    //Apply the theme
    FlushMenuThemes();
    return SetPreferredAppMode(theme);
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

    return build >= 18362;
}

//Updates the theme of the window when the settings change.
void Wh_ModSettingsChanged()
{
    Wh_Log(L"Settings changed");
    ApplyTheme();
}

//Import functions
BOOL Wh_ModInit() {
    if(!IsAPISupported())
    {
        Wh_Log(L"Outdated Windows version!");
        return FALSE;
    }

    Wh_Log(L"Init");

    if(!Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&DefWindowProcW_Original))
        return FALSE;

    const HMODULE hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    const FARPROC pSetPreferredAppMode = GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135));
    const FARPROC pFlushMenuThemes = GetProcAddress(hUxtheme, MAKEINTRESOURCEA(136));

    SetPreferredAppMode = reinterpret_cast<SetPreferredAppMode_T>(pSetPreferredAppMode);
    FlushMenuThemes = reinterpret_cast<FlushMenuThemes_T>(pFlushMenuThemes);

    HRESULT hResult = ApplyTheme();
    return SUCCEEDED(hResult);
}

//Restores the default theme.
void Wh_ModUninit()
{
    Wh_Log(L"Restoring the default theme.");

    Wh_RemoveFunctionHook((void*) DefWindowProcW);
    ApplyTheme(Default);
}

// ==WindhawkMod==
// @id                dark-menus
// @version           1.4.1
// @author            Mgg Sk
// @github            https://github.com/MGGSK
// @include           *
// @compilerOptions   -lUxTheme -lGdi32 -lNtDll

// @name              Dark mode context menus
// @description       Enables dark mode for all win32 menus.

// @name:de-DE        Dunkler Modus für Menüs
// @description:de-DE Aktiviert den Dunklen Modus für alle Win32 Menüs.
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Dark mode context menus
Enables dark mode for all win32 menus to create a consistent UI. Requires Windows 10 build 18362 or later.

### Before:
![Before](https://i.imgur.com/bGRVJz8.png)
![MenubarBefore](https://raw.githubusercontent.com/MGGSK/DarkMenus/main/Images/menubar_before.png)

### After:
![After](https://i.imgur.com/BURKEki.png)
![Menubar](https://raw.githubusercontent.com/MGGSK/DarkMenus/main/Images/menubar.png)

# Feedback:

### Dark menus: [Repository](https://github.com/MGGSK/DarkMenus) [Issues](https://github.com/MGGSK/DarkMenus/issues) [Discussions](https://github.com/MGGSK/DarkMenus/discussions)
### Windhawk mods: [Repository](https://github.com/ramensoftware/windhawk-mods) [Issues](https://github.com/ramensoftware/windhawk-mods/issues) [Discussions](https://github.com/ramensoftware/windhawk-mods/discussions)

## Credits: [win32-darkmode](https://github.com/adzm/win32-darkmode), [notepad++](https://github.com/notepad-plus-plus/notepad-plus-plus) 
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
#include <windowsx.h>

#include <windhawk_api.h>
#include <windhawk_utils.h>

constexpr COLORREF DARK_MENU_COLOR = 0x2C2C2C;
constexpr COLORREF DARK_MENU_ITEM_FOREGROUND = 0xFFFFFF;
constexpr COLORREF DARK_MENU_ITEM_FOREGROUND_DISABLED = 0xAAAAAA;

const HBRUSH DARK_CONTROL_BRUSH = CreateSolidBrush(0x262626);
const HBRUSH DARK_MENU_BRUSH = CreateSolidBrush(DARK_MENU_COLOR);
const HBRUSH DARK_MENUBAR_SEPARATOR_BRUSH = CreateSolidBrush(0x222222);
const HBRUSH DARK_MENU_ITEM_BACKGROUND_HOVER = CreateSolidBrush(0x353535);
const HBRUSH DARK_MENU_ITEM_BACKGROUND_SELECTED = CreateSolidBrush(0x454545);

thread_local HTHEME g_menuBarTheme = nullptr;
HFONT g_iconFont = nullptr;
HMODULE g_hUxtheme = nullptr;

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

EXTERN_C NTSYSAPI VOID NTAPI RtlGetNtVersionNumbers(LPDWORD dwMajor, LPDWORD dwMinor, LPDWORD dwBuild);

//Checks if the windows build is 22000 or later.
bool IsWindows11()
{
    return true;
    DWORD build;
    RtlGetNtVersionNumbers(nullptr, nullptr, &build);

    build &= ~0xF0000000;
    return build >= 22000;
}

//Checks if the windows build is 18362 or later.
bool IsAPISupported()
{
    return true;
    DWORD build;
    RtlGetNtVersionNumbers(nullptr, nullptr, &build);

    build &= ~0xF0000000;
    return build >= 18362;
}

//Code based on https://github.com/notepad-plus-plus/notepad-plus-plus/blob/bab3573be708bb908b8080e3e2007ea78a7f1932/PowerEditor/src/NppDarkMode.cpp
#pragma region CodeBasedOnNotepad++

void DrawUAHMenuNCBottomLine(HWND hWnd)
{
	MENUBARINFO menuBarInfo{ sizeof(menuBarInfo) };
	if (!GetMenuBarInfo(hWnd, OBJID_MENU, 0, &menuBarInfo))
		return;

	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	MapWindowPoints(hWnd, nullptr, (LPPOINT)&rcClient, 2);

	RECT rcWindow;
	GetWindowRect(hWnd, &rcWindow);

	OffsetRect(&rcClient, -rcWindow.left, -rcWindow.top);

	//The rcBar is offset by the window rect
	RECT rcAnnoyingLine = rcClient;
	rcAnnoyingLine.bottom = rcAnnoyingLine.top;
	rcAnnoyingLine.top--;

	HDC hdc = GetWindowDC(hWnd);
	FillRect(hdc, &rcAnnoyingLine, DARK_MENUBAR_SEPARATOR_BRUSH);
	ReleaseDC(hWnd, hdc);
}

#pragma endregion CodeBasedOnNotepad++

//Code based on https://github.com/adzm/win32-darkmode/blob/darkmenubar/win32-darkmode/win32-darkmode.cpp
#pragma region CodeBasedOnWin32DarkMode

//Processes messages related to custom menubar drawing.
//Returns true if handled, false to continue with normal processing
bool CALLBACK UAHWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* lpResult)
{
    switch (uMsg)  
    {
    case WM_UAHDRAWMENU:
    {
        const auto* drawingInfo = (UAHMENU*)lParam;
        RECT rcMenu;

        //Get the menubar rect
        MENUBARINFO menuBarInfo{ sizeof(menuBarInfo) };
        if (!GetMenuBarInfo(hWnd, OBJID_MENU, 0, &menuBarInfo))
            return false;

        RECT rcWindow;
        if (!GetWindowRect(hWnd, &rcWindow))
            return false;

        //The rcBar is offset by the window rect
        rcMenu = menuBarInfo.rcBar;
        if (!OffsetRect(&rcMenu, -rcWindow.left, -rcWindow.top))
            return false;

        FillRect(drawingInfo->hdc, &rcMenu, DARK_MENU_BRUSH);
        return true;
    }

    case WM_UAHDRAWMENUITEM:
    {
        auto* drawingInfo = (UAHDRAWMENUITEM*)lParam;
        const HBRUSH* hBrBackground = &DARK_MENU_BRUSH;

        //Get the menu item string
        wchar_t menuString[256]{};
        MENUITEMINFO itemInfo{ sizeof(itemInfo), MIIM_STRING };
        itemInfo.dwTypeData = menuString;
        itemInfo.cch = (sizeof(menuString) / sizeof(wchar_t)) - 1;

        if (!GetMenuItemInfoW(drawingInfo->um.hMenu, drawingInfo->umi.iPosition, TRUE, &itemInfo))
            return false;
        menuString[255] = L'\0';

        //Get the item state for drawing
        DWORD dwFlags = DT_CENTER | DT_SINGLELINE | DT_VCENTER;
        int iTextStateID = 0;

        if ((drawingInfo->dis.itemState & ODS_INACTIVE) || (drawingInfo->dis.itemState & ODS_DEFAULT))
            iTextStateID = MBI_NORMAL;
        else if (drawingInfo->dis.itemState & ODS_HOTLIGHT)
            iTextStateID = MBI_HOT;
        else if (drawingInfo->dis.itemState & ODS_SELECTED)
            iTextStateID = MBI_PUSHED;
        else if ((drawingInfo->dis.itemState & ODS_GRAYED) || (drawingInfo->dis.itemState & ODS_DISABLED))
            iTextStateID = MBI_DISABLED;

        if (GetForegroundWindow() != hWnd)
            iTextStateID = MBI_DISABLED;

        if (drawingInfo->dis.itemState & ODS_HOTLIGHT)
            hBrBackground = &DARK_MENU_ITEM_BACKGROUND_HOVER;
        else if (drawingInfo->dis.itemState & ODS_SELECTED)
            hBrBackground = &DARK_MENU_ITEM_BACKGROUND_SELECTED;

        if (drawingInfo->dis.itemState & ODS_NOACCEL)
            dwFlags |= DT_HIDEPREFIX;

        if (!g_menuBarTheme)
            g_menuBarTheme = OpenThemeData(hWnd, L"Menu");

        const DTTOPTS textOptions{ sizeof(textOptions), DTT_TEXTCOLOR, iTextStateID != MBI_DISABLED ? DARK_MENU_ITEM_FOREGROUND : DARK_MENU_ITEM_FOREGROUND_DISABLED };
        FillRect(drawingInfo->um.hdc, &drawingInfo->dis.rcItem, *hBrBackground);

        bool isMdiCaptionButton = menuString[0] == L'\0';
        if (isMdiCaptionButton)
        {
            int offset = GetMenuItemCount(drawingInfo->um.hMenu) - drawingInfo->umi.iPosition;

            wchar_t glyph;
            if (drawingInfo->umi.iPosition == 0)
                glyph = L'\ue700'; //Menu icon
            else if (offset == 3)
                glyph = L'\ue921'; //Minimize icon
            else if (offset == 2)
                glyph = L'\ue923'; //Maximize icon
            else if (offset == 1)
                glyph = L'\ue8bb'; //Close icon
            else
                return false;

            if(!g_iconFont)
            {
                g_iconFont = CreateFontW(10, NULL, NULL, NULL, FW_NORMAL, FALSE, FALSE, FALSE,
                    DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                    (IsWindows11() ? L"Segoe Fluent Icons" : L"Segoe Mdl2 Assets"));
            }

            HFONT hOldFont = SelectFont(drawingInfo->um.hdc, g_iconFont);

            HRESULT hResult = DrawThemeTextEx(g_menuBarTheme, drawingInfo->um.hdc, MENU_BARITEM, MBI_NORMAL, &glyph, 1, dwFlags, &drawingInfo->dis.rcItem, &textOptions);
            if (FAILED(hResult))
                return false;

            SelectFont(drawingInfo->um.hdc, hOldFont);
            return true;
        }

        HRESULT hResult = DrawThemeTextEx(g_menuBarTheme, drawingInfo->um.hdc, MENU_BARITEM, MBI_NORMAL, menuString, itemInfo.cch, dwFlags, &drawingInfo->dis.rcItem, &textOptions);
        if (FAILED(hResult))
            return false;

        return true;
    }

    case WM_THEMECHANGED:
    case WM_DESTROY:
        if (g_menuBarTheme)
        {
            CloseThemeData(g_menuBarTheme);
            g_menuBarTheme = nullptr;
        }
        return false;
        
    default:
        return false;
    }
}
#pragma endregion

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

FlushMenuThemes_T FlushMenuThemes = nullptr;
SetPreferredAppMode_T SetPreferredAppMode = nullptr;
ShouldAppsUseDarkMode_T ShouldAppsUseDarkMode = nullptr;

AppMode g_currentAppMode;

#define IS_DARK_MODE(hWnd) ((g_currentAppMode == AppMode::ForceDark || (g_currentAppMode == AppMode::AllowDark && ShouldAppsUseDarkMode())) && GetMenu(hWnd))

decltype(&DefFrameProcW) DefFrameProcW_Original;
LRESULT CALLBACK DefFrameProcW_Hook(HWND hWnd, HWND hMdiClient, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(!IS_DARK_MODE(hWnd))
        return DefFrameProcW_Original(hWnd, hMdiClient, uMsg, wParam, lParam);
    
    LRESULT lResult = 0;
    if(UAHWndProc(hWnd, uMsg, wParam, lParam, &lResult))
        return lResult;

    if(uMsg == WM_ACTIVATE || uMsg == WM_NCPAINT)
    {
        lResult = DefFrameProcW_Original(hWnd, hMdiClient, uMsg, wParam, lParam);
        DrawUAHMenuNCBottomLine(hWnd);
        return lResult;
    }

    return DefFrameProcW_Original(hWnd, hMdiClient, uMsg, wParam, lParam);
}

decltype(&DefWindowProcW) DefWindowProcW_Original;
LRESULT CALLBACK DefWindowProcW_Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(!IS_DARK_MODE(hWnd))
        return DefWindowProcW_Original(hWnd, uMsg, wParam, lParam);

    LRESULT lResult = 0;
    if(UAHWndProc(hWnd, uMsg, wParam, lParam, &lResult))
        return lResult;

    if(uMsg == WM_ACTIVATE || uMsg == WM_NCPAINT)
    {
        lResult = DefWindowProcW_Original(hWnd, uMsg, wParam, lParam);
        DrawUAHMenuNCBottomLine(hWnd);
        return lResult;
    }

    return DefWindowProcW_Original(hWnd, uMsg, wParam, lParam);
}

decltype(&DefFrameProcA) DefFrameProcA_Original;
LRESULT CALLBACK DefFrameProcA_Hook(HWND hWnd, HWND hMdiClient, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return DefFrameProcA_Original(hWnd, hMdiClient, uMsg, wParam, lParam);
    if(!IS_DARK_MODE(hWnd))
        return DefFrameProcA_Original(hWnd, hMdiClient, uMsg, wParam, lParam);

    LRESULT lResult = 0;
    if(UAHWndProc(hWnd, uMsg, wParam, lParam, &lResult))
        return lResult;

    if(uMsg == WM_ACTIVATE || uMsg == WM_NCPAINT)
    {
        lResult = DefFrameProcA_Original(hWnd, hMdiClient, uMsg, wParam, lParam);
        DrawUAHMenuNCBottomLine(hWnd);
        return lResult;
    }

    return DefFrameProcA_Original(hWnd, hMdiClient, uMsg, wParam, lParam);
}

decltype(&DefWindowProcA) DefWindowProcA_Original;
LRESULT CALLBACK DefWindowProcA_Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if(!IS_DARK_MODE(hWnd))
        return DefWindowProcA_Original(hWnd, uMsg, wParam, lParam);

    LRESULT lResult = 0;
    if(UAHWndProc(hWnd, uMsg, wParam, lParam, &lResult))
        return lResult;

    if(uMsg == WM_ACTIVATE || uMsg == WM_NCPAINT)
    {
        lResult = DefWindowProcA_Original(hWnd, uMsg, wParam, lParam);
        DrawUAHMenuNCBottomLine(hWnd);
        return lResult;
    }

    return DefWindowProcA_Original(hWnd, uMsg, wParam, lParam);
}

decltype(&SetMenuInfo) SetMenuInfo_Original;
WINBOOL WINAPI SetMenuInfo_Hook(HMENU hMenu, LPCMENUINFO lpInfo)
{
    //Disable custom menu backgrounds because they are broken in dark mode. (See https://github.com/MGGSK/DarkMenus/issues/16)
    alignas(MENUINFO) BYTE buffer[256];
    if (!(lpInfo->fMask & MIM_BACKGROUND) || lpInfo->cbSize > sizeof(buffer))
        return SetMenuInfo_Original(hMenu, lpInfo);

    memcpy(buffer, lpInfo, lpInfo->cbSize);
    auto* infoCopy = reinterpret_cast<LPMENUINFO>(buffer);
    infoCopy->hbrBack = CreateSolidBrush(DARK_MENU_COLOR); //Fixes https://github.com/MGGSK/DarkMenus/issues/18
    return SetMenuInfo_Original(hMenu, infoCopy);
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

    if(!WindhawkUtils::SetFunctionHook(DefWindowProcW, DefWindowProcW_Hook, &DefWindowProcW_Original) ||
        !WindhawkUtils::SetFunctionHook(DefWindowProcA, DefWindowProcA_Hook, &DefWindowProcA_Original))
    {
        Wh_Log(L"Failed to hook DefWindowProc!");
        return FALSE;
    }

    if(!WindhawkUtils::SetFunctionHook(DefFrameProcW, DefFrameProcW_Hook, &DefFrameProcW_Original) ||
        !WindhawkUtils::SetFunctionHook(DefFrameProcA, DefFrameProcA_Hook, &DefFrameProcA_Original))
    {
        Wh_Log(L"Failed to hook DefFrameProc!");
        return FALSE;
    }

    if(!WindhawkUtils::SetFunctionHook(SetMenuInfo, SetMenuInfo_Hook, &SetMenuInfo_Original))
    {
        Wh_Log(L"Failed to hook SetMenuInfo!");
        return FALSE;
    }

    g_hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    SetPreferredAppMode = (SetPreferredAppMode_T)GetProcAddress(g_hUxtheme, MAKEINTRESOURCEA(135));
    FlushMenuThemes = (FlushMenuThemes_T)GetProcAddress(g_hUxtheme, MAKEINTRESOURCEA(136));
    ShouldAppsUseDarkMode = (ShouldAppsUseDarkMode_T)GetProcAddress(g_hUxtheme, MAKEINTRESOURCEA(132));

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
        DeleteObject(DARK_CONTROL_BRUSH);
        DeleteObject(DARK_MENU_BRUSH);
        DeleteObject(DARK_MENUBAR_SEPARATOR_BRUSH);
        DeleteObject(DARK_MENU_ITEM_BACKGROUND_HOVER);
        DeleteObject(DARK_MENU_ITEM_BACKGROUND_SELECTED);

        DeleteObject(g_iconFont);
        CloseThemeData(g_menuBarTheme);
        FreeLibrary(g_hUxtheme);
    }
}



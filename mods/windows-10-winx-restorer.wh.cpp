// ==WindhawkMod==
// @id              windows-10-winx-restorer
// @name            Windows 10 Win+X restorer on Win11 24H2
// @description     This mod restores a Windows 10-style Win+X menu on Windows 11, with immersive or native Win32 styling and support for legacy and modern taskbars
// @version         1.1.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @architecture    x86-64
// @include         explorer.exe
// @compilerOptions -luser32 -lgdi32 -lshell32 -lcomctl32 -lole32 -loleaut32 -ladvapi32 -lpowrprof
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 8.1/10 Power User Context Menu Restorer on Win11 24H2

This mod restores a Windows 8.1/10-style power user menu on Windows 11.
Open it with **Win+X** or by **right-clicking the Start button**.

The menu can be used with both the Windows 11 taskbar and a legacy Windows 10
taskbar. Clicking Start normally or pressing the Windows key alone is not
replaced: these actions continue to open the regular Start menu.


## Screenshot

Immersive style (original screenshot):

![Windows 10-style Win+X menu](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/winx.png)

Additionally, a Win32 styled one can be enabled in the mod's settings (as it was in Windows 8/8.1).

## Features

- **Immersive menu:** a recreated Windows 10-style menu with light and dark themes.
- **Native Win32 menu:** a standard Windows popup menu with system menu metrics.
  Dark colours are painted locally on this menu and its submenus only.
- **Menu positioning:** open next to Start or at the mouse pointer, with adjustable
  horizontal and vertical offsets.
- **Hide Shut down or sign out:** remove the power submenu for a Windows 8-like layout.
- **PowerShell fallback:** use Windows PowerShell and Windows PowerShell (Admin)
  when the Windows Terminal executable is unavailable.
- **Localized entries:** menu labels follow the Windows display language for the
  languages supported by the mod.

## Settings

Choose a menu style, select the menu colour theme, and adjust the position
as needed. The menu style affects appearance only: the mod replaces Win+X while
it is active regardless of the chosen style.

The power submenu and the PowerShell fallback can be configured independently.
The colour theme also applies to Win32 mode: Auto follows the Windows app theme,
Light keeps the standard system menu, and Dark uses local owner-drawn colours.
High-contrast mode keeps the system Win32 appearance for accessibility.
No process-wide dark-mode preference is changed, and the Dark mode context menus
mod is not required. Settings apply to the next menu opening.

## Requirements and compatibility

This mod is designed for Windows 11 24H2 and 25H2, x64. Native shell interception uses symbols
matching the installed Windows components. The first initialization may require
internet access to download symbols. If modern symbols cannot be resolved, only
the modern guard is disabled; the keyboard Win+X and legacy Start-button routes
remain available.

The immersive appearance is recreated, rather than loading the original Windows
10 menu component. Compatibility can depend on the Windows build and other mods
that modify Start, the taskbar, or Win+X. Disable the mod to restore shell handling.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- WinXMenuSkin: immersive
  $name: Menu style
  $description: This setting changes the appearance of the restored menu, not which shell or taskbar it replaces.
  $options:
  - immersive: Windows 10-style immersive menu
  - win32: Native Windows menu (Win32)
- WinXMenuTheme: auto
  $name: Menu theme
  $description: This setting selects the colour theme of the generated menu only. Dark Win32 colours are scoped to this menu and its submenus; other Explorer menus are not changed.
  $options:
  - auto: Follow the Windows app theme
  - light: Light
  - dark: Dark
- WinXMenuAnchor: start
  $name: Menu position
  $description: This setting chooses whether the menu opens next to Start or at the mouse pointer.
  $options:
  - start: Next to the Start button
  - cursor: At the mouse pointer
- WinXMenuOffsetX: 0
  $name: Horizontal offset (pixels)
  $description: This setting adjusts the Start-anchored horizontal position. Negative values move left; positive values move right.
- WinXMenuOffsetY: 0
  $name: Vertical offset (pixels)
  $description: This setting adjusts the Start-anchored vertical position. Negative values move up; positive values move down.
- WinXHideShutdown: false
  $name: Hide "Shut down or sign out"
  $description: This setting removes the power submenu and its preceding separator for a Windows 8-like layout.
- WinXTerminalFallback: true
  $name: Use PowerShell when Terminal is unavailable
  $description: This setting replaces both Terminal entries with Windows PowerShell when wt.exe cannot be found, including the administrator entry.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <windows.h>
#include <commctrl.h>
#include <objbase.h>
#include <powrprof.h>
#include <shldisp.h>
#include <uiautomation.h>
#include <atomic>
#include <cwctype>
#include <cwchar>
#include <cstring>
#include <climits>
#include <new>
#include <vector>

static bool ContainsNoCase(const wchar_t* haystack, const wchar_t* needle) {
    if (!haystack || !needle || !*needle) return false;
    size_t n = wcslen(needle);
    for (const wchar_t* p = haystack; *p; p++) {
        if (_wcsnicmp(p, needle, n) == 0) return true;
    }
    return false;
}

enum UiLangId {
    LANG_IT = 0, LANG_EN, LANG_FR, LANG_ES, LANG_DE,
    LANG_PT, LANG_NL, LANG_RU, LANG_JA, LANG_PL,
    LANG_DA, LANG_SV, LANG_NO, LANG_FI, LANG_TR, LANG_COUNT
};

static UiLangId DetectUiLang() {
    LANGID lid = (LANGID)(GetUserDefaultUILanguage() & 0xFFFF);
    WORD primary = PRIMARYLANGID(lid);
    switch (primary) {
        case LANG_ITALIAN:    return LANG_IT;
        case LANG_ENGLISH:    return LANG_EN;
        case LANG_FRENCH:     return LANG_FR;
        case LANG_SPANISH:    return LANG_ES;
        case LANG_GERMAN:     return LANG_DE;
        case LANG_PORTUGUESE: return LANG_PT;
        case LANG_DUTCH:      return LANG_NL;
        case LANG_RUSSIAN:    return LANG_RU;
        case LANG_JAPANESE:   return LANG_JA;
        case LANG_POLISH:     return LANG_PL;
        case LANG_DANISH:     return LANG_DA;
        case LANG_SWEDISH:    return LANG_SV;
        case LANG_NORWEGIAN:  return LANG_NO;
        case LANG_FINNISH:    return LANG_FI;
        case LANG_TURKISH:    return LANG_TR;
        default:              return LANG_EN;
    }
}

// Destroys the popup menu it owns.
class ScopedMenu {
public:
    explicit ScopedMenu(HMENU menu = nullptr) : m_menu(menu) {}
    ~ScopedMenu() { if (m_menu) DestroyMenu(m_menu); }
    ScopedMenu(const ScopedMenu&) = delete;
    ScopedMenu& operator=(const ScopedMenu&) = delete;
    HMENU get() const { return m_menu; }
private:
    HMENU m_menu;
};

// ---------------------------------------------------------------------------
// Menu presentation: the Windows 10 style menu
//
// Windows 10 draws its Win+X menu with its own owner-draw code, not with the plain
// popup of TrackPopupMenuEx. The renderer below ("ImmersiveMenu", ~430 lines) takes
// the ordinary popup menu built further down, reads
// its items back through GetMenuItemInfoW and draws them with the Windows 10
// proportions, colours and font - 32 px rows, the Windows 10 light (F9F9F9) or dark
// (2B2B2B) scheme, the shell theme when the shell offers it. Entries and commands are
// untouched: only the look changes, and it is the look of the original menu.
// ---------------------------------------------------------------------------
namespace ImmersiveMenu {

using ThemeHandle = void*;
static constexpr int kPartBackground = 9;      // MENU_POPUPBACKGROUND
static constexpr int kPartItem = 14;           // MENU_POPUPITEM
static constexpr int kStateNormal = 1;
static constexpr int kStateHot = 2;
static constexpr int kStateDisabled = 3;
static constexpr int kPropBorderColor = 3801;  // TMT_BORDERCOLOR
static constexpr int kPropFillColor = 3802;    // TMT_FILLCOLOR
static constexpr int kPropTextColor = 3803;    // TMT_TEXTCOLOR
static constexpr int kPropFont = 210;          // TMT_FONT
static constexpr UINT_PTR kSubclassId = 0x494D4D31;   // "IMM1"

struct Api {
    ThemeHandle (WINAPI* openTheme)(HWND, LPCWSTR) = nullptr;
    ThemeHandle (WINAPI* openThemeForDpi)(HWND, LPCWSTR, UINT) = nullptr;
    HRESULT (WINAPI* closeTheme)(ThemeHandle) = nullptr;
    HRESULT (WINAPI* drawBackground)(ThemeHandle, HDC, int, int, const RECT*, const RECT*) = nullptr;
    HRESULT (WINAPI* getFont)(ThemeHandle, HDC, int, int, int, LOGFONTW*) = nullptr;
    HRESULT (WINAPI* getColor)(ThemeHandle, int, int, int, COLORREF*) = nullptr;
    bool ready = false;
};
static Api g_api;

static bool LoadApi() noexcept {
    if (g_api.ready) return true;
    HMODULE module = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) return false;
    g_api.openTheme = reinterpret_cast<ThemeHandle (WINAPI*)(HWND, LPCWSTR)>(
        GetProcAddress(module, "OpenThemeData"));
    g_api.openThemeForDpi = reinterpret_cast<ThemeHandle (WINAPI*)(HWND, LPCWSTR, UINT)>(
        GetProcAddress(module, "OpenThemeDataForDpi"));
    g_api.closeTheme = reinterpret_cast<HRESULT (WINAPI*)(ThemeHandle)>(
        GetProcAddress(module, "CloseThemeData"));
    g_api.drawBackground =
        reinterpret_cast<HRESULT (WINAPI*)(ThemeHandle, HDC, int, int, const RECT*, const RECT*)>(
            GetProcAddress(module, "DrawThemeBackground"));
    g_api.getFont = reinterpret_cast<HRESULT (WINAPI*)(ThemeHandle, HDC, int, int, int, LOGFONTW*)>(
        GetProcAddress(module, "GetThemeFont"));
    g_api.getColor = reinterpret_cast<HRESULT (WINAPI*)(ThemeHandle, int, int, int, COLORREF*)>(
        GetProcAddress(module, "GetThemeColor"));
    g_api.ready = g_api.openTheme && g_api.closeTheme && g_api.getColor;
    return g_api.ready;
}

struct ItemData {
    wchar_t text[128];
    bool separator;
    bool submenu;
    // Native profile only: restore the HMENU before releasing drawing metadata.
    HMENU menu = nullptr;
    UINT position = 0;
    UINT originalType = 0;
    ULONG_PTR originalData = 0;
};

struct MenuBackground {
    HMENU menu;
    HBRUSH original;
};

struct Session {
    HWND owner = nullptr;
    ThemeHandle theme = nullptr;
    int dpi = 96;
    LOGFONTW font = {};
    HFONT hfont = nullptr;
    HBRUSH brush = nullptr;
    // Colours of the Windows 10 menu: the light scheme is the one the menu has
    // always used (F9F9F9 / black text), the dark one is the Windows 10 dark menu
    // (2B2B2B / white text, 414141 highlight) as in the reference screenshot. The
    // theme of the shell refines them when it is available, so the menu follows the
    // Windows app theme, or the scheme forced by the WinXMenuTheme setting.
    bool light = true;
    COLORREF fill = RGB(249, 249, 249);
    COLORREF textNormal = RGB(0, 0, 0);
    COLORREF textDisabled = RGB(160, 160, 160);
    COLORREF separator = RGB(205, 205, 205);
    COLORREF hotFill = RGB(229, 229, 229);
    int itemHeight = 0;
    bool nativeMetrics = false; // dark Win32 profile: system font/rows, not immersive layout
    // Proportions measured on the Windows 10 reference screenshot at 100% DPI and
    // used as they are, so the recreation has the proportions of the
    // original: 32 px rows, text 32 px inside the item rectangle, the submenu chevron
    // 6 px from its right edge, the separator line inset by 8 px, and a menu about
    // 258 px wide - the width of the reference menu, from which the shell subtracts
    // its own right-hand gutter when the items are measured.
    int padLeft = 0;
    int padRight = 0;
    int chevronInset = 0;
    int separatorInset = 0;
    std::vector<ItemData*> items;
    std::vector<MenuBackground> backgrounds; // only used by the native profile
};
static thread_local Session* g_session = nullptr;

// Which scheme the menu uses. "Auto" follows the Windows app theme (Settings >
// Personalization > Colors); "Light" / "Dark" force the Windows 10 light or dark
// menu. Both colour sets are always available: when the shell theme cannot be
// opened the menu is painted with the Windows 10 constants of the chosen scheme
// instead of becoming an unstyled popup.
static bool IsLightTheme() noexcept {
    WindhawkUtils::StringSetting scheme(Wh_GetStringSetting(L"WinXMenuTheme"));
    if (_wcsicmp(scheme.get(), L"light") == 0) return true;
    if (_wcsicmp(scheme.get(), L"dark") == 0) return false;
    DWORD value = 1, size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size) == ERROR_SUCCESS)
        return value != 0;
    return true;
}

// The sequence pnidui itself uses: light/dark variant, then neutral, then "Menu".
static ThemeHandle OpenMenuTheme(HWND owner, int dpi) noexcept {
    const wchar_t* candidates[3] = {
        IsLightTheme() ? L"LightMode_ImmersiveStart::Menu" : L"DarkMode_ImmersiveStart::Menu",
        L"ImmersiveStart::Menu",
        L"Menu" };
    for (const wchar_t* name : candidates) {
        ThemeHandle theme = nullptr;
        if (g_api.openThemeForDpi) theme = g_api.openThemeForDpi(owner, name, (UINT)dpi);
        if (!theme && g_api.openTheme) theme = g_api.openTheme(owner, name);
        if (theme) {
            static int logged = 0;
            if (logged++ < 3) Wh_Log(L"[menu] immersive menu: theme class '%s'", name);
            return theme;
        }
    }
    return nullptr;
}

static void End(Session& s) noexcept {
    if (s.nativeMetrics) {
        // All popup windows have closed before this cleanup. Detach our item-data
        // pointers and background brush while they are still alive; only then
        // release the GDI objects. This also rolls back a failed native Begin.
        for (ItemData* d : s.items) {
            MENUITEMINFOW current = {};
            current.cbSize = sizeof(current);
            current.fMask = MIIM_DATA;
            if (GetMenuItemInfoW(d->menu, d->position, TRUE, &current) &&
                current.dwItemData == reinterpret_cast<ULONG_PTR>(d)) {
                MENUITEMINFOW restore = {};
                restore.cbSize = sizeof(restore);
                restore.fMask = MIIM_FTYPE | MIIM_DATA;
                restore.fType = d->originalType;
                restore.dwItemData = d->originalData;
                SetMenuItemInfoW(d->menu, d->position, TRUE, &restore);
            }
        }
        for (const MenuBackground& back : s.backgrounds) {
            MENUINFO restore = {};
            restore.cbSize = sizeof(restore);
            restore.fMask = MIM_BACKGROUND;
            if (GetMenuInfo(back.menu, &restore) && restore.hbrBack == s.brush) {
                restore.hbrBack = back.original;
                SetMenuInfo(back.menu, &restore);
            }
        }
        s.backgrounds.clear();
    }
    for (ItemData* d : s.items) delete d;
    s.items.clear();
    if (s.brush) { DeleteObject(s.brush); s.brush = nullptr; }
    if (s.hfont) { DeleteObject(s.hfont); s.hfont = nullptr; }
    if (s.theme && g_api.closeTheme) { g_api.closeTheme(s.theme); s.theme = nullptr; }
}

static bool Prepare(Session& s, HMENU menu) noexcept {
    const int count = GetMenuItemCount(menu);
    if (s.nativeMetrics && count < 0) return false;
    for (int i = 0; i < count; ++i) {
        wchar_t buffer[128] = {};
        MENUITEMINFOW info = {};
        info.cbSize = sizeof(info);
        info.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_SUBMENU |
                     (s.nativeMetrics ? MIIM_DATA : 0);
        info.dwTypeData = buffer;
        info.cch = _countof(buffer) - 1;
        if (!GetMenuItemInfoW(menu, i, TRUE, &info)) {
            if (s.nativeMetrics) return false;
            continue;
        }
        ItemData* data = new (std::nothrow) ItemData{};
        if (!data) {
            if (s.nativeMetrics) return false;
            continue;
        }
        if (s.nativeMetrics) {
            data->menu = menu;
            data->position = static_cast<UINT>(i);
            data->originalType = info.fType;
            data->originalData = info.dwItemData;
        }
        data->separator = (info.fType & MFT_SEPARATOR) != 0;
        data->submenu = info.hSubMenu != nullptr;
        wcsncpy_s(data->text, buffer, _TRUNCATE);
        s.items.push_back(data);

        MENUITEMINFOW set = {};
        set.cbSize = sizeof(set);
        set.fMask = MIIM_FTYPE | MIIM_DATA;
        set.fType = info.fType | MFT_OWNERDRAW;
        set.dwItemData = reinterpret_cast<ULONG_PTR>(data);
        const BOOL changed = SetMenuItemInfoW(menu, i, TRUE, &set);
        if (s.nativeMetrics && !changed) return false;
        if (info.hSubMenu && !Prepare(s, info.hSubMenu)) return false;
    }
    MENUINFO mi = {};
    mi.cbSize = sizeof(mi);
    mi.fMask = MIM_BACKGROUND;
    if (s.nativeMetrics) {
        if (!GetMenuInfo(menu, &mi)) return false;
        s.backgrounds.push_back({menu, mi.hbrBack});
    }
    mi.hbrBack = s.brush;                               // theme background, never null
    const BOOL changed = SetMenuInfo(menu, &mi);
    return !s.nativeMetrics || changed;
}

static bool Begin(Session& s, HMENU menu, HWND owner, UINT dpiHint,
                  bool nativeMetrics = false) noexcept {
    if (!owner || (!nativeMetrics && !LoadApi())) return false;
    s.owner = owner;
    s.nativeMetrics = nativeMetrics;
    // The menu is measured with the scale of the monitor it opens on. The owner
    // window is parked off-screen (see GetWinXMenuOwnerWindow), so its own DPI is
    // that of the primary monitor, which is the wrong one for a secondary taskbar
    // at a different scale: the caller passes the DPI of that monitor instead, and
    // zero means that nothing better is known.
    const UINT dpi = dpiHint ? dpiHint : GetDpiForWindow(owner);
    s.dpi = dpi >= 96 && dpi <= 480 ? (int)dpi : 96;
    // Windows 10 constants for the selected scheme: they stay in place if the
    // theme cannot be opened or does not carry a colour, so both themes are
    // supported even on builds where the immersive menu theme is missing.
    s.light = nativeMetrics ? false : IsLightTheme();
    s.fill = s.light ? RGB(249, 249, 249) : RGB(43, 43, 43);
    s.textNormal = s.light ? RGB(0, 0, 0) : RGB(255, 255, 255);
    s.textDisabled = s.light ? RGB(120, 120, 120) : RGB(160, 160, 160);
    s.separator = s.light ? RGB(205, 205, 205) : RGB(128, 128, 128);
    s.hotFill = s.light ? RGB(229, 229, 229) : RGB(65, 65, 65);
    if (nativeMetrics) {
        // Palette inspired by dark-menus. Deliberately no SetPreferredAppMode,
        // FlushMenuThemes, uxtheme ordinals, or hooks of global drawing APIs.
        s.fill = RGB(44, 44, 44);
        s.textNormal = RGB(255, 255, 255);
        s.textDisabled = RGB(170, 170, 170);
        s.hotFill = RGB(69, 69, 69);
        s.separator = RGB(80, 80, 80);
    }
    s.theme = nativeMetrics ? nullptr : OpenMenuTheme(owner, s.dpi);
    if (!s.theme && !nativeMetrics) {
        // No immersive theme on this build: paint the menu with the Windows 10
        // colours anyway instead of falling back to a plain popup.
        Wh_Log(L"[menu] immersive theme unavailable: the menu keeps the Windows 10 %s colours",
               s.light ? L"light" : L"dark");
    }

    HDC dc = GetDC(owner);
    if (nativeMetrics) {
        NONCLIENTMETRICSW metrics = {};
        metrics.cbSize = sizeof(metrics);
        if (SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(metrics),
                                       &metrics, 0, static_cast<UINT>(s.dpi))) {
            s.font = metrics.lfMenuFont;
        } else if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(metrics),
                                         &metrics, 0)) {
            s.font = metrics.lfMenuFont;
            const UINT systemDpi = GetDpiForSystem();
            if (systemDpi) s.font.lfHeight = MulDiv(s.font.lfHeight, s.dpi, systemDpi);
        }
        if (!s.font.lfHeight) {
            s.font.lfHeight = -MulDiv(9, s.dpi, 72);
            wcscpy_s(s.font.lfFaceName, L"Segoe UI");
        }
    } else {
        if (!s.theme || !g_api.getFont ||
            FAILED(g_api.getFont(s.theme, dc, kPartItem, 0, kPropFont, &s.font)) ||
            s.font.lfHeight == 0) {
            NONCLIENTMETRICSW info = {};
            info.cbSize = sizeof(info);
            if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0))
                s.font = info.lfMenuFont;
            else
                s.font.lfHeight = -MulDiv(9, s.dpi, 72);
            wcscpy_s(s.font.lfFaceName, LF_FACESIZE, L"Segoe UI");
        }
    }
    if (s.theme) {
        // A theme colour is accepted only when the call succeeded: a failed
        // GetThemeColor can leave 0 (black) in the output, which would turn the menu
        // background and its text black.
        COLORREF themed = 0;
        if (g_api.getColor(s.theme, kPartBackground, 0, kPropFillColor, &themed) == S_OK)
            s.fill = themed;
        if (g_api.getColor(s.theme, kPartItem, kStateNormal, kPropTextColor, &themed) == S_OK)
            s.textNormal = themed;
        if (g_api.getColor(s.theme, kPartItem, kStateDisabled, kPropTextColor, &themed) == S_OK)
            s.textDisabled = themed;
        COLORREF hot = s.hotFill;
        if (g_api.getColor(s.theme, kPartItem, kStateHot, kPropFillColor, &hot) == S_OK)
            s.hotFill = hot;
    }
    // Text of the same colour as the background would be invisible: back to the
    // Windows 10 constants of the scheme.
    if (s.fill == s.textNormal) {
        s.fill = s.light ? RGB(249, 249, 249) : RGB(43, 43, 43);
        s.textNormal = s.light ? RGB(0, 0, 0) : RGB(255, 255, 255);
    }
    // Colour of the separator line for the scheme in use. In the light scheme the
    // border colour the menu theme gives to its own popups is used when it is
    // available (it is the colour the earlier builds showed); in the dark scheme the
    // grey measured on the Windows 10 dark reference screenshot is used, which is
    // clearly visible on the 2B2B2B background.
    if (s.light && s.theme && g_api.getColor) {
        COLORREF themed = s.separator;
        if (g_api.getColor(s.theme, kPartBackground, 0, kPropBorderColor, &themed) == S_OK &&
            themed != s.fill)
            s.separator = themed;
    }
    static int themeLogs = 0;
    if (themeLogs++ < 6)
        Wh_Log(L"[menu] %s menu: background RGB(%u,%u,%u), highlight RGB(%u,%u,%u), "
               L"text RGB(%u,%u,%u), separator RGB(%u,%u,%u)",
               s.light ? L"light" : L"dark",
               GetRValue(s.fill), GetGValue(s.fill), GetBValue(s.fill),
               GetRValue(s.hotFill), GetGValue(s.hotFill), GetBValue(s.hotFill),
               GetRValue(s.textNormal), GetGValue(s.textNormal), GetBValue(s.textNormal),
               GetRValue(s.separator), GetGValue(s.separator), GetBValue(s.separator));
    s.hfont = CreateFontIndirectW(&s.font);
    int textHeight = abs(s.font.lfHeight);
    if (s.hfont && dc) {
        HGDIOBJ old = SelectObject(dc, s.hfont);
        TEXTMETRICW tm = {};
        if (GetTextMetricsW(dc, &tm)) textHeight = tm.tmHeight;
        if (old) SelectObject(dc, old);
    }
    if (dc) ReleaseDC(owner, dc);
    // Reference proportions (see Session), in device independent pixels so the
    // menu looks the same at every display scale: rows 32 px tall, text 32 px in
    // from the item rectangle, chevron 6 px from its right edge, separator line
    // inset by 8 px. A row never becomes shorter than the text plus 8 px, so a
    // large font or an East Asian face cannot clip the glyphs.
    s.itemHeight = MulDiv(32, s.dpi, 96);
    const int minItemHeight = textHeight + MulDiv(8, s.dpi, 96);
    if (s.itemHeight < minItemHeight) s.itemHeight = minItemHeight;
    s.padLeft = MulDiv(32, s.dpi, 96);
    s.padRight = MulDiv(10, s.dpi, 96);
    s.chevronInset = MulDiv(4, s.dpi, 96);
    s.separatorInset = MulDiv(8, s.dpi, 96);
    if (nativeMetrics) {
        s.itemHeight = GetSystemMetricsForDpi(SM_CYMENU, static_cast<UINT>(s.dpi));
        const int nativeMin = textHeight + MulDiv(6, s.dpi, 96);
        if (s.itemHeight < nativeMin) s.itemHeight = nativeMin;
        s.padLeft = GetSystemMetricsForDpi(SM_CXMENUCHECK, static_cast<UINT>(s.dpi)) +
                    MulDiv(6, s.dpi, 96);
        s.padRight = MulDiv(12, s.dpi, 96);
        s.chevronInset = MulDiv(8, s.dpi, 96);
        s.separatorInset = MulDiv(3, s.dpi, 96);
    }
    s.brush = CreateSolidBrush(s.fill);
    if (!s.hfont || !s.brush) return false;
    return Prepare(s, menu);
}

static bool Owns(const Session* s, const ItemData* d) noexcept {
    if (!s || !d) return false;
    for (const ItemData* item : s->items)
        if (item == d) return true;
    return false;
}

static void Measure(Session* s, HWND hwnd, MEASUREITEMSTRUCT* m, const ItemData* d) noexcept {
    if (d->separator) {
        m->itemWidth = 1;
        m->itemHeight = MulDiv(s->nativeMetrics ? 7 : 9, s->dpi, 96);
        return;
    }
    int width = 0;
    HDC dc = GetDC(hwnd);
    if (dc) {
        HGDIOBJ old = SelectObject(dc, s->hfont);
        RECT rc = {};
        DrawTextW(dc, d->text, -1, &rc, DT_CALCRECT | DT_SINGLELINE | DT_LEFT);
        width = rc.right - rc.left;
        if (old) SelectObject(dc, old);
        ReleaseDC(hwnd, dc);
    }
    width += s->padLeft + s->padRight;
    if (d->submenu) width += s->chevronInset + MulDiv(14, s->dpi, 96);
    // 242 px plus the ~16 px gutter the shell keeps on the right of the item
    // rectangle gives the 258 px wide menu of the Windows 10 reference screenshot.
    const int minWidth = s->nativeMetrics ? 0 : MulDiv(242, s->dpi, 96);
    // Upper bounds, so a wrong measurement can never produce a menu that covers
    // the screen.
    const int maxWidth = MulDiv(520, s->dpi, 96);
    const int maxHeight = MulDiv(64, s->dpi, 96);
    if (width > maxWidth) width = maxWidth;
    int height = s->itemHeight;
    if (height > maxHeight) height = maxHeight;
    m->itemWidth = static_cast<UINT>(width < minWidth ? minWidth : width);
    m->itemHeight = static_cast<UINT>(height);
}

static void Draw(Session* s, DRAWITEMSTRUCT* di, const ItemData* d) noexcept {
    HDC dc = di->hDC;
    RECT rc = di->rcItem;
    // Start every item from a clean clip region: the submenu rows take their own
    // rectangle out of the clip below (to keep the
    // shell from painting a second arrow on top of ours), and a clip left over from
    // an earlier paint of the same row would otherwise erase parts of this one.
    SelectClipRgn(dc, nullptr);
    FillRect(dc, &rc, s->brush);
    if (d->separator) {
        // One pixel line of the separator colour of the scheme, always painted here.
        // The menu theme cannot be trusted for this part: on the build that reported
        // missing separators the themed MENU_POPUPSEPARATOR answered success without
        // painting anything, so the section is drawn directly and never depends on the
        // theme. Position and inset are the ones measured on the Windows 10 reference
        // menu (a 1 px line, 8 px inside the item rectangle, in the middle of the row).
        const int y = (rc.top + rc.bottom) / 2;
        RECT r = { rc.left + s->separatorInset, y, rc.right - s->separatorInset, y + 1 };
        HBRUSH line = CreateSolidBrush(s->separator);
        if (line) {
            FillRect(dc, &r, line);
            DeleteObject(line);
        } else {
            // Only possible when the process is out of GDI objects: the row keeps the
            // menu background (a null brush would leave a black line behind).
            Wh_Log(L"[menu] separator brush not available: the line is skipped");
        }
        return;
    }
    const bool disabled = (di->itemState & (ODS_GRAYED | ODS_DISABLED)) != 0;
    const bool selected = (di->itemState & ODS_SELECTED) != 0 && !disabled;
    COLORREF textColor = disabled ? s->textDisabled : s->textNormal;
    if (selected) {
        if (!s->theme || !g_api.drawBackground ||
            FAILED(g_api.drawBackground(s->theme, dc, kPartItem, kStateHot, &rc, nullptr))) {
            // Fallback of the selected row: the Windows 10 highlight colour of the
            // scheme in use (light 229,229,229 / dark 65,65,65), never an inverted
            // background, which used to turn the row black on a light menu.
            HBRUSH hot = CreateSolidBrush(s->hotFill);
            if (hot) { FillRect(dc, &rc, hot); DeleteObject(hot); }
        }
        COLORREF hotText = textColor;
        if (s->theme &&
            g_api.getColor(s->theme, kPartItem, kStateHot, kPropTextColor, &hotText) == S_OK)
            textColor = hotText;
    }
    HGDIOBJ old = SelectObject(dc, s->hfont);
    const int previousBkMode = SetBkMode(dc, TRANSPARENT);
    const COLORREF previousTextColor = SetTextColor(dc, textColor);
    RECT tr = rc;
    tr.left += s->padLeft;
    tr.right -= s->padRight;
    DrawTextW(dc, d->text, -1, &tr, DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_HIDEPREFIX);
    if (d->submenu) {
        if (s->nativeMetrics) {
            // A native-sized triangular arrow, using this row's text colour.
            const int size = MulDiv(4, s->dpi, 96);
            const int right = static_cast<int>(rc.right) - s->chevronInset;
            const int centerY = static_cast<int>((rc.top + rc.bottom) / 2);
            POINT points[3] = {{right - size, centerY - size},
                               {right, centerY}, {right - size, centerY + size}};
            HBRUSH arrow = CreateSolidBrush(textColor);
            if (arrow) {
                HGDIOBJ previousBrush = SelectObject(dc, arrow);
                HGDIOBJ previousPen = SelectObject(dc, GetStockObject(NULL_PEN));
                Polygon(dc, points, _countof(points));
                SelectObject(dc, previousPen);
                SelectObject(dc, previousBrush);
                DeleteObject(arrow);
            }
        } else {
            // One chevron only: the shell glyph Windows 10 shows for a submenu, at the
            // same distance from the right edge (see the reference screenshot).
            RECT ar = rc;
            ar.right -= s->chevronInset;
            HFONT arrowFont = CreateFontW(s->font.lfHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                          DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0,
                                          L"Segoe MDL2 Assets");
            if (arrowFont) {
                SelectObject(dc, arrowFont);
                DrawTextW(dc, L"\xE76C", -1, &ar, DT_SINGLELINE | DT_VCENTER | DT_RIGHT);
                SelectObject(dc, s->hfont);
                DeleteObject(arrowFont);
            }
        }
    }
    if (old) SelectObject(dc, old);
    if (s->nativeMetrics) {
        SetBkMode(dc, previousBkMode);
        SetTextColor(dc, previousTextColor);
    }
    if (d->submenu) {
        // One arrow, not two. The shell paints its own submenu arrow after
        // WM_DRAWITEM has returned, on top of the chevron drawn just above: the
        // local workaround is to take this item rectangle out of the clip
        // region of this DC before returning, so only our chevron survives.
        ExcludeClipRect(dc, rc.left, rc.top, rc.right, rc.bottom);
    }
}

// The entries are owner-drawn, so the system cannot match a pressed letter to an
// item and sends WM_MENUCHAR to the owner window instead. The Windows 10 menu is
// usable from the keyboard, so the letter is matched here: an entry is executed when
// exactly one entry of the active menu starts with it. When more than one entry
// matches, the key is ignored instead of guessing, because executing the wrong entry
// would be worse than doing nothing (in the power submenu "Sign out", "Sleep" and
// "Shut down" all start with the same letter in several languages). Separators and
// entries that open a submenu are not matched.
//
// posOut is the zero-based position of the entry in the menu: with MNC_EXECUTE the
// system chooses the item at that position - the low word of the return value is a
// position, not a command identifier - and TrackPopupMenuEx with TPM_RETURNCMD then
// returns the identifier of that item to the caller.
static bool MatchMenuChar(Session* s, HMENU menu, wchar_t ch, UINT& posOut) {
    if (!menu || !ch) return false;
    UINT pos = 0;
    int matches = 0;
    const int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; ++i) {
        MENUITEMINFOW info = {};
        info.cbSize = sizeof(info);
        info.fMask = MIIM_ID | MIIM_FTYPE | MIIM_SUBMENU | MIIM_DATA;
        if (!GetMenuItemInfoW(menu, i, TRUE, &info)) continue;
        if (info.fType & MFT_SEPARATOR) continue;
        // An entry that opens a submenu is never executed by a letter: its identifier
        // is the submenu handle and not a command index, and the arrows open it.
        if (info.hSubMenu) continue;
        const auto* d = reinterpret_cast<const ItemData*>(info.dwItemData);
        if (!Owns(s, d) || !d->text[0]) continue;
        if (towupper(d->text[0]) != towupper(ch)) continue;
        if (++matches > 1) return false;
        pos = static_cast<UINT>(i);
    }
    if (matches != 1) return false;
    posOut = pos;
    return true;
}

static LRESULT CALLBACK OwnerProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                  UINT_PTR, DWORD_PTR) {
    Session* s = g_session;
    if (s && s->owner != hwnd) return DefSubclassProc(hwnd, msg, wParam, lParam);
    if (s && msg == WM_MEASUREITEM && lParam) {
        auto* m = reinterpret_cast<MEASUREITEMSTRUCT*>(lParam);
        const auto* d = reinterpret_cast<const ItemData*>(m->itemData);
        if (m->CtlType == ODT_MENU && Owns(s, d)) {
            Measure(s, hwnd, m, d);
            return TRUE;
        }
    } else if (s && msg == WM_DRAWITEM && lParam) {
        auto* di = reinterpret_cast<DRAWITEMSTRUCT*>(lParam);
        const auto* d = reinterpret_cast<const ItemData*>(di->itemData);
        if (di->CtlType == ODT_MENU && Owns(s, d)) {
            Draw(s, di, d);
            return TRUE;
        }
    } else if (s && msg == WM_MENUCHAR) {
        if (s->nativeMetrics) {
            bool ownedMenu = false;
            for (const ItemData* item : s->items)
                if (item->menu == reinterpret_cast<HMENU>(lParam)) ownedMenu = true;
            if (!ownedMenu) return DefSubclassProc(hwnd, msg, wParam, lParam);
        }
        // LOWORD(wParam) is the character the user pressed, lParam the active menu.
        // What the system wants back with MNC_EXECUTE is the position of the entry
        // in that menu, and with TPM_RETURNCMD Track then returns its identifier.
        UINT pos = 0;
        if (MatchMenuChar(s, reinterpret_cast<HMENU>(lParam), (wchar_t)LOWORD(wParam), pos))
            return MAKELRESULT(pos, MNC_EXECUTE);
        return MAKELRESULT(0, MNC_IGNORE);
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

// Shared owner-draw tracker: immersive by default, native-sized dark profile
// when nativeMetrics is true. Uses HMENU and TrackPopupMenuEx in both cases. Must be
// called from the thread that owns "owner". Missing themes use our colour palette;
// a missing owner subclass falls back to an ordinary menu before items are altered.
static UINT Track(HMENU menu, HWND owner, int x, int y, UINT flags, UINT dpi,
                  bool nativeMetrics = false) noexcept {
    flags = (flags & ~TPM_NONOTIFY) | TPM_RETURNCMD;
    Session session;
    if (g_session || !SetWindowSubclass(owner, OwnerProc, kSubclassId, 0))
        return static_cast<UINT>(TrackPopupMenuEx(menu, flags, x, y, owner, nullptr));
    if (!Begin(session, menu, owner, dpi, nativeMetrics)) {
        if (nativeMetrics)
            Wh_Log(L"[menu] native dark setup failed; restoring our menu and using the system palette");
        RemoveWindowSubclass(owner, OwnerProc, kSubclassId);
        End(session);
        return static_cast<UINT>(TrackPopupMenuEx(menu, flags, x, y, owner, nullptr));
    }
    g_session = &session;
    const UINT result = static_cast<UINT>(TrackPopupMenuEx(menu, flags, x, y, owner, nullptr));
    RemoveWindowSubclass(owner, OwnerProc, kSubclassId);
    g_session = nullptr;
    End(session);
    return result;
}

}  // namespace ImmersiveMenu


namespace NativeMenu {

// Only the HMENU trees explicitly created and marked by this mod are eligible.
// Never change menus by class name alone or by globally detouring SetMenuInfo.
static const int kOwnedMenuTag = 0;

static bool MarkOwnedTree(HMENU menu) noexcept {
    MENUINFO info = {};
    info.cbSize = sizeof(info);
    info.fMask = MIM_MENUDATA;
    info.dwMenuData = reinterpret_cast<ULONG_PTR>(&kOwnedMenuTag);
    if (!SetMenuInfo(menu, &info)) return false;
    const int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; ++i) {
        HMENU child = GetSubMenu(menu, i);
        if (child && !MarkOwnedTree(child)) return false;
    }
    return true;
}

static bool IsOwnedTree(HMENU menu) noexcept {
    MENUINFO info = {};
    info.cbSize = sizeof(info);
    info.fMask = MIM_MENUDATA;
    if (!GetMenuInfo(menu, &info) ||
        info.dwMenuData != reinterpret_cast<ULONG_PTR>(&kOwnedMenuTag)) return false;
    const int count = GetMenuItemCount(menu);
    if (count < 0) return false;
    for (int i = 0; i < count; ++i) {
        HMENU child = GetSubMenu(menu, i);
        if (child && !IsOwnedTree(child)) return false;
    }
    return true;
}

static bool IsHighContrast() noexcept {
    HIGHCONTRASTW info = {};
    info.cbSize = sizeof(info);
    // If the accessibility state cannot be queried, keep the stock palette too.
    return !SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(info), &info, 0) ||
           (info.dwFlags & HCF_HIGHCONTRASTON);
}

static UINT Track(HMENU menu, HWND owner, int x, int y, UINT flags, UINT dpi) noexcept {
    wchar_t ownerClass[64] = {};
    const bool ownWindow = owner &&
        GetWindowThreadProcessId(owner, nullptr) == GetCurrentThreadId() &&
        GetClassNameW(owner, ownerClass, _countof(ownerClass)) &&
        !wcscmp(ownerClass, L"Windows10WinXRestorer.MenuOwner");
    if (!ownWindow || !IsOwnedTree(menu)) {
        Wh_Log(L"[menu] win32 scoped theme rejected: owner/menu is not ours");
        return static_cast<UINT>(TrackPopupMenuEx(menu, flags, x, y, owner, nullptr));
    }
    if (IsHighContrast() || ImmersiveMenu::IsLightTheme()) {
        Wh_Log(L"[menu] win32 palette: system (light/high contrast), root %p", menu);
        return static_cast<UINT>(TrackPopupMenuEx(menu, flags, x, y, owner, nullptr));
    }
    Wh_Log(L"[menu] win32 palette: dark; ONLY owned root %p and its submenus", menu);
    // Still an HMENU/TrackPopupMenuEx popup with native metrics. Only its own
    // entries are owner-drawn for colour; the 32-pixel immersive layout is NOT used.
    // The owner subclass is removed when tracking ends. No app-wide preference,
    // registry writes, permanent window subclass, or extra API detour is installed.
    return ImmersiveMenu::Track(menu, owner, x, y, flags, dpi, true);
}

} // namespace NativeMenu


// ---------------------------------------------------------------------------
// Module settings and the process this mod acts in
// ---------------------------------------------------------------------------
static std::atomic<bool> g_winXMenuAnchorCursor{false};
static std::atomic<int> g_winXMenuOffsetX{0};
static std::atomic<int> g_winXMenuOffsetY{0};
static std::atomic<bool> g_winXMenuSkinWin32{false};
static std::atomic<bool> g_winXHideShutdown{false};
static std::atomic<bool> g_winXTerminalFallback{true};

// No executable-path or shell-style gate: normal Windows 11 explorer.exe and
// private legacy explorer.exe copies are both eligible. Only the visible taskbar
// owner arms input hooks, so ordinary folder processes stay idle.
// The Shell_TrayWnd of this process, or null. FindWindowW is not enough: the
// Windows 10 shell is a private copy of explorer.exe and can have sibling processes
// started from the same folder, and only one of them owns the taskbar. Enumerating
// sends no message to any window, so it is safe to call from this mod's threads.
static BOOL CALLBACK FindOwnTaskbarProc(HWND hwnd, LPARAM param) {
    wchar_t className[32] = {};
    if (!GetClassNameW(hwnd, className, _countof(className)) ||
        _wcsicmp(className, L"Shell_TrayWnd") != 0) {
        return TRUE;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId() || !IsWindowVisible(hwnd)) return TRUE;
    *reinterpret_cast<HWND*>(param) = hwnd;
    return FALSE;
}

static HWND FindOwnTaskbarWindow() {
    HWND taskbar = nullptr;
    EnumWindows(FindOwnTaskbarProc, reinterpret_cast<LPARAM>(&taskbar));
    return taskbar;
}

// The hooks of this mod are system-wide, so they are armed only in the process that
// owns the taskbar: a second explorer.exe started from the same folder as the
// Windows 10 shell must not install a second pair of hooks answering to the same
// chord.
static bool OwnsTaskbarWindow() {
    return FindOwnTaskbarWindow() != nullptr;
}

static void LoadSettings() {
    WindhawkUtils::StringSetting value(Wh_GetStringSetting(L"WinXMenuAnchor"));
    g_winXMenuAnchorCursor = _wcsicmp(value.get(), L"cursor") == 0;
    g_winXMenuOffsetX = (int)Wh_GetIntSetting(L"WinXMenuOffsetX");
    g_winXMenuOffsetY = (int)Wh_GetIntSetting(L"WinXMenuOffsetY");
    WindhawkUtils::StringSetting skin(Wh_GetStringSetting(L"WinXMenuSkin"));
    g_winXMenuSkinWin32 = _wcsicmp(skin.get(), L"win32") == 0;
    g_winXHideShutdown = Wh_GetIntSetting(L"WinXHideShutdown") != 0;
    g_winXTerminalFallback = Wh_GetIntSetting(L"WinXTerminalFallback") != 0;
}

// ---------------------------------------------------------------------------
// State of the Win+X machinery
// ---------------------------------------------------------------------------
// Posted by the input hooks to the services thread. It is a registered message
// rather than a WM_APP value, so it cannot be confused with a message of the shell
// or of another mod.
static const wchar_t kWinXRouteMessageName[] = L"Windows10WinXRestorer.Route";
static UINT g_winXRouteMessage = 0;
static int g_winXHookModuleAnchor = 0;
static std::atomic<bool> g_winXKeyboardFallbackEnabled{false};
static std::atomic<bool> g_winXMouseRouteEnabled{false};
static bool g_winXRightButtonDownOnStart = false;
static bool g_winXKeyboardHookInstalled = false;
// g_winXMenuOpen is set for exactly as long as TrackPopupMenuEx keeps the menu on
// screen, so the input hooks can tell "the menu of this mod is already open" from
// "nothing is open" without guessing: while it is set, a second chord - or a
// right-click on Start - closes the menu instead of stacking another one.
static std::atomic<bool> g_winXMenuOpen{false};
// Owner of the open menu, the target of WM_CANCELMODE. It is written by the menu
// thread and read by the hook thread and by Wh_ModUninit, so it is atomic.
static std::atomic<HWND> g_winXMenuOwnerWindow{nullptr};
static bool g_winXSwallowXUp = false;
static bool g_winXXHeld = false;   // X is physically down: auto-repeat is ignored
static bool g_winXMouseHookInstalled = false;
static std::atomic<DWORD> g_servicesThreadId{0};
// The thread that shows the menu. It is created by the services thread and joined
// by Wh_ModUninit: the mod image must not be unloaded while the menu is on screen
// or while the chosen entry is being opened.
static HANDLE g_menuThread = nullptr;
// Set when the mod is about to be unloaded, so a menu thread that has just been
// created does not show a new menu.
static std::atomic<bool> g_unloading{false};

// ---------------------------------------------------------------------------
// Menu commands
//
// Every entry of the menu ends up here, on the menu thread; the low-level hooks
// never run any of this. Each target is handed to the shell with ShellExecuteW and
// every failure is logged.
// ---------------------------------------------------------------------------
// Sleep: SetSuspendState is called directly instead of
// "rundll32.exe powrprof.dll,SetSuspendState 0,1,0". The signature of
// SetSuspendState is not the one rundll32 expects from an entry point, and that
// call is known to hibernate instead of sleeping when hibernation is enabled.
// SE_SHUTDOWN_NAME is enabled for the call, which requires it, and the state the
// privilege had before is put back afterwards: Explorer's process token is left as
// it was found instead of keeping a privilege enabled for the rest of the session.
static void SuspendSystem() {
    TOKEN_PRIVILEGES enabled{};
    enabled.PrivilegeCount = 1;
    enabled.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    // Room for the previous state of one privilege, which is all that is changed.
    alignas(TOKEN_PRIVILEGES) BYTE previousBuffer[sizeof(TOKEN_PRIVILEGES) +
                                                  sizeof(LUID_AND_ATTRIBUTES)]{};
    auto* previous = reinterpret_cast<TOKEN_PRIVILEGES*>(previousBuffer);
    DWORD previousSize = 0;

    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
        token = nullptr;
    if (token && LookupPrivilegeValueW(nullptr, SE_SHUTDOWN_NAME,
                                       &enabled.Privileges[0].Luid)) {
        AdjustTokenPrivileges(token, FALSE, &enabled, sizeof(previousBuffer), previous,
                              &previousSize);
        const DWORD adjustError = GetLastError();
        if (adjustError == ERROR_NOT_ALL_ASSIGNED)
            Wh_Log(L"[winx] SeShutdownPrivilege could not be enabled (%lu)", adjustError);
    }

    if (!SetSuspendState(FALSE, FALSE, FALSE))
        Wh_Log(L"[winx] SetSuspendState failed (%lu)", GetLastError());

    if (token) {
        if (previousSize)
            AdjustTokenPrivileges(token, FALSE, previous, 0, nullptr, nullptr);
        CloseHandle(token);
    }
}

// CLSID of the shell automation object, {13709620-C279-11CE-A49E-444553540000}
// ("Shell.Application"). shldisp.h only declares CLSID_Shell - it is defined by
// libuuid, which this mod does not link - so the value is written out here.
static const CLSID kClsidShell = {0x13709620,
                                  0xc279,
                                  0x11ce,
                                  {0xa4, 0x9e, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}};

// Show the desktop: IShellDispatch4::ToggleDesktop is called directly instead of
// injecting a Win+D chord, whose result depends on the modifiers the user is still
// physically holding when the entry is chosen. COM is initialized on this thread
// (see WinXMenuThread).
static void ToggleDesktop() {
    IShellDispatch4* shell = nullptr;
    HRESULT hr = CoCreateInstance(kClsidShell, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&shell));
    if (FAILED(hr) || !shell) {
        Wh_Log(L"[winx] ToggleDesktop: the shell dispatch object could not be created "
               L"(0x%08X)", (unsigned)hr);
        return;
    }
    hr = shell->ToggleDesktop();
    shell->Release();
    if (FAILED(hr))
        Wh_Log(L"[winx] ToggleDesktop failed (0x%08X)", (unsigned)hr);
}

static bool OpenShellUri(const wchar_t* uri) {
    if (!uri || !*uri) return false;
    HINSTANCE result = ShellExecuteW(nullptr, L"open", uri, nullptr, nullptr, SW_SHOWNORMAL);
    const bool ok = reinterpret_cast<INT_PTR>(result) > 32;
    if (!ok) Wh_Log(L"[winx] ShellExecute failed for %s (%Id)", uri, (INT_PTR)result);
    return ok;
}

static void RunCommand(const wchar_t* file, const wchar_t* params, const wchar_t* verb,
                       const wchar_t* what) {
    if (!file || !*file) return;
    HINSTANCE result = ShellExecuteW(nullptr, verb && *verb ? verb : L"open", file, params,
                                     nullptr, SW_SHOWNORMAL);
    if (reinterpret_cast<INT_PTR>(result) <= 32)
        Wh_Log(L"[winx] %s: opening %s %s failed (%Id)", what ? what : L"entry", file,
               params ? params : L"", (INT_PTR)result);
}

static int StartButtonEdgeDistance(HWND hwnd, const RECT& taskbarRect) {
    RECT buttonRect = {};
    if (!GetWindowRect(hwnd, &buttonRect)) return INT_MAX;
    const int taskbarWidth = taskbarRect.right - taskbarRect.left;
    const int taskbarHeight = taskbarRect.bottom - taskbarRect.top;
    const bool horizontal = taskbarWidth >= taskbarHeight;
    return horizontal ? abs((int)buttonRect.left - (int)taskbarRect.left)
                      : abs((int)buttonRect.top - (int)taskbarRect.top);
}

static bool IsPlausibleStartButton(HWND hwnd, HWND taskbar, const RECT& taskbarRect,
                                   bool allowEdgeButton) {
    if (!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd) || !IsWindowEnabled(hwnd) ||
        !IsChild(taskbar, hwnd)) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return false;

    RECT rect = {};
    if (!GetWindowRect(hwnd, &rect)) return false;
    const int width = (int)(rect.right - rect.left);
    const int height = (int)(rect.bottom - rect.top);
    if (width <= 0 || height <= 0 || width > 240 || height > 240) return false;

    wchar_t cls[128] = {};
    if (!GetClassNameW(hwnd, cls, _countof(cls))) return false;
    const int controlId = GetDlgCtrlID(hwnd);
    // The criterion for the native Windows Start button: control ID 0x130. The
    // "Start"/"StartButton" class names are not accepted here, because OpenShell
    // uses exactly those classes for its own button.
    if (controlId == 0x130) return true;

    // Last resort: only for builds where the ID changed, and only when it really is a
    // "Button" at the edge of the taskbar.
    if (!allowEdgeButton || _wcsicmp(cls, L"Button") != 0) return false;
    const int edgeDistance = StartButtonEdgeDistance(hwnd, taskbarRect);
    return edgeDistance >= 0 && edgeDistance <= 180;
}

struct StartButtonSearch {
    HWND taskbar = nullptr;
    RECT taskbarRect = {};
    HWND byId = nullptr;
    HWND byClass = nullptr;
    HWND byEdge = nullptr;
    int edgeDistance = INT_MAX;
};

static BOOL CALLBACK FindStartButtonChildProc(HWND hwnd, LPARAM param) {
    auto* search = (StartButtonSearch*)param;
    if (GetDlgCtrlID(hwnd) == 0x130 &&
        IsPlausibleStartButton(hwnd, search->taskbar, search->taskbarRect, false)) {
        search->byId = hwnd;
        return FALSE;
    }

    wchar_t cls[128] = {};
    if (GetClassNameW(hwnd, cls, _countof(cls)) &&
        ((_wcsicmp(cls, L"Start") == 0) || ContainsNoCase(cls, L"StartButton")) &&
        IsPlausibleStartButton(hwnd, search->taskbar, search->taskbarRect, false)) {
        search->byClass = hwnd;
        return FALSE;
    }

    if (IsPlausibleStartButton(hwnd, search->taskbar, search->taskbarRect, true)) {
        const int distance = StartButtonEdgeDistance(hwnd, search->taskbarRect);
        if (distance < search->edgeDistance) {
            search->byEdge = hwnd;
            search->edgeDistance = distance;
        }
    }
    return TRUE;
}

// The Start button of a taskbar window (the primary one or a secondary one).
static HWND FindStartButtonInTaskbar(HWND taskbar) {
    if (!taskbar) return nullptr;

    RECT taskbarRect = {};
    if (!GetWindowRect(taskbar, &taskbarRect)) return nullptr;

    HWND byId = GetDlgItem(taskbar, 0x130);
    if (IsPlausibleStartButton(byId, taskbar, taskbarRect, false)) return byId;

    StartButtonSearch search = {};
    search.taskbar = taskbar;
    search.taskbarRect = taskbarRect;
    EnumChildWindows(taskbar, FindStartButtonChildProc, (LPARAM)&search);
    if (search.byId) return search.byId;
    if (search.byClass) return search.byClass;
    return search.byEdge;
}

// The Start button of the primary taskbar: the one the keyboard chord opens the menu
// on, as Windows 10 does.
static HWND FindNativeTaskbarStartButton() {
    return FindStartButtonInTaskbar(FindOwnTaskbarWindow());
}

// Only the services thread installs/unhooks these handles. There is deliberately
// no global destructor that can call User32 during DLL_PROCESS_DETACH.
static HHOOK g_winXKeyboardHook = nullptr;
static HHOOK g_winXMouseHook = nullptr;
static DWORD g_winXRouteRefreshMs = 500;
static ULONGLONG g_servicesStartedAt = 0;

static void ResetWinXInputHooks() {
    if (g_winXKeyboardHook) {
        UnhookWindowsHookEx(g_winXKeyboardHook);
        g_winXKeyboardHook = nullptr;
    }
    if (g_winXMouseHook) {
        UnhookWindowsHookEx(g_winXMouseHook);
        g_winXMouseHook = nullptr;
    }
}

static void ClearWinXInputRoutes() {
    g_winXKeyboardFallbackEnabled.store(false, std::memory_order_release);
    g_winXMouseRouteEnabled.store(false, std::memory_order_release);
    g_winXRightButtonDownOnStart = false;
}

// startButton is the button the right-click came from, or null for the keyboard
// chord: the menu opens on the taskbar the request came from.
enum class WinXRequestSource : WPARAM {
    KeyboardChord = 1,
    StartRightClick = 2,
    ModernMenuEntry = 3,
};

static bool PostWinXContextRequestFromHook(HWND startButton, WinXRequestSource source) {
    // The menu is not shown from here: this runs inside a low-level hook and
    // TrackPopupMenuEx blocks the thread. Windows removes a hook that does not answer
    // within LowLevelHooksTimeout (~300 ms), so the work is handed to the services
    // thread, which starts the menu thread.
    const DWORD thread = g_servicesThreadId.load(std::memory_order_acquire);
    return !g_unloading.load(std::memory_order_acquire) && thread &&
           PostThreadMessageW(thread, g_winXRouteMessage, static_cast<WPARAM>(source),
                              reinterpret_cast<LPARAM>(startButton));
}

// ---------------------------------------------------------------------------
// Sets g_winXMenuOpen for exactly as long as the menu is on screen, whatever
// TrackPopupMenuEx does: a flag that went stale would make CloseWinXMenuIfOpen()
// swallow the next Win+X, which is the one failure this mod must not have.
// ---------------------------------------------------------------------------
class WinXMenuOpenScope {
public:
    explicit WinXMenuOpenScope(HWND owner) {
        g_winXMenuOwnerWindow.store(owner, std::memory_order_release);
        g_winXMenuOpen.store(true, std::memory_order_release);
    }
    ~WinXMenuOpenScope() {
        g_winXMenuOpen.store(false, std::memory_order_release);
        g_winXMenuOwnerWindow.store(nullptr, std::memory_order_release);
    }
    WinXMenuOpenScope(const WinXMenuOpenScope&) = delete;
    WinXMenuOpenScope& operator=(const WinXMenuOpenScope&) = delete;
};

// ---------------------------------------------------------------------------
// Closes the mod's menu while it is on screen (a second Win+X, a right-click on
// Start, or any request that must not stack another menu).
//
// TrackPopupMenuEx runs a modal loop on the thread that showed the menu, so the
// menu cannot be ended from inside the low-level hook itself. WM_CANCELMODE is
// the documented way to cancel a menu and it is *posted* to the owner window, so
// that thread's modal loop does the work. The caller must then not forward the
// request again, otherwise the menu closes and immediately reopens.
// ---------------------------------------------------------------------------
static bool CloseWinXMenuIfOpen() {
    if (!g_winXMenuOpen.load(std::memory_order_acquire)) return false;
    const HWND owner = g_winXMenuOwnerWindow.load(std::memory_order_acquire);
    if (!owner || !IsWindow(owner)) return false;   // stale: let the chord through
    if (!PostMessageW(owner, WM_CANCELMODE, 0, 0)) return false;
    Wh_Log(L"[winx] the menu is open: this request closes it (as the native menu does)");
    return true;
}

// One on-demand lookup for the taskbar whose menu is being opened. This is
// called by ComputeWinXMenuAnchor on the new menu thread BEFORE that thread
// initializes its STA and creates any windows. UIA runs in a temporary MTA;
// only a RECT survives it. No UIA calls run inside input hooks or on shell UI.
static bool QueryStartButtonRectOnce(HWND taskbar, RECT* result) {
    if (!taskbar || !result || g_unloading.load(std::memory_order_acquire)) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(taskbar, &pid);
    if (pid != GetCurrentProcessId()) return false;
    const HRESULT com = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(com)) return false;

    IUIAutomation* automation = nullptr;
    IUIAutomationCondition* condition = nullptr;
    IUIAutomationElement* root = nullptr;
    IUIAutomationElement* start = nullptr;
    RECT bounds = {};
    bool haveBounds = false;
    const CLSID clsid = {0xff48dba4, 0x60ef, 0x4201,
                        {0xaa, 0x87, 0x54, 0x10, 0x3e, 0xef, 0x59, 0x4e}};
    if (SUCCEEDED(CoCreateInstance(clsid, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&automation))) && automation) {
        VARIANT value;
        VariantInit(&value);
        value.vt = VT_BSTR;
        value.bstrVal = SysAllocString(L"StartButton");
        if (value.bstrVal)
            automation->CreatePropertyCondition(UIA_AutomationIdPropertyId, value, &condition);
        VariantClear(&value);
        if (condition &&
            SUCCEEDED(automation->ElementFromHandle(taskbar, &root)) && root &&
            SUCCEEDED(root->FindFirst(TreeScope_Descendants, condition, &start)) && start) {
            BOOL offscreen = TRUE;
            haveBounds = SUCCEEDED(start->get_CurrentIsOffscreen(&offscreen)) && !offscreen &&
                         SUCCEEDED(start->get_CurrentBoundingRectangle(&bounds));
        }
    }
    if (start) start->Release();
    if (root) root->Release();
    if (condition) condition->Release();
    if (automation) automation->Release();
    CoUninitialize();

    RECT taskbarBounds = {}, clipped = {};
    GetWindowThreadProcessId(taskbar, &pid);
    if (haveBounds && !g_unloading.load(std::memory_order_acquire) &&
        pid == GetCurrentProcessId() && bounds.right > bounds.left && bounds.bottom > bounds.top &&
        GetWindowRect(taskbar, &taskbarBounds) &&
        IntersectRect(&clipped, &bounds, &taskbarBounds)) {
        *result = clipped;
        return true;
    }
    return false;
}

// The Start button under a point, or null. Low-level input hooks use only local
// HWND queries, not UIA or a guessed XAML hit area. Modern XAML context requests
// are identified by Explorer and intercepted at the real menu-creation entry.
static std::atomic<bool> g_modernNativeGuard{false};

static HWND WinXStartButtonFromPoint(const POINT& point) {
    const HWND hit = WindowFromPoint(point);
    if (!hit) return nullptr;
    DWORD pid = 0;
    GetWindowThreadProcessId(hit, &pid);
    if (pid != GetCurrentProcessId()) return nullptr;

    const HWND root = GetAncestor(hit, GA_ROOT);
    wchar_t rootClass[64] = {};
    if (!GetClassNameW(root, rootClass, _countof(rootClass))) return nullptr;
    if (_wcsicmp(rootClass, L"Shell_TrayWnd") != 0 &&
        _wcsicmp(rootClass, L"Shell_SecondaryTrayWnd") != 0) {
        return nullptr;
    }

    // The button can be the window under the pointer or one of its children.
    for (HWND hwnd = hit; hwnd && hwnd != root; hwnd = GetParent(hwnd)) {
        if (GetDlgCtrlID(hwnd) == 0x130) return hwnd;   // the native Start button
        wchar_t className[128] = {};
        if (GetClassNameW(hwnd, className, _countof(className)) &&
            (_wcsicmp(className, L"Start") == 0 ||
             ContainsNoCase(className, L"StartButton"))) {
            return hwnd;
        }
    }
    return nullptr;
}

// Native Win+X guards. These are private Microsoft PDB symbols, NOT a documented
// Windows API. Resolve full signatures against the loaded image with Windhawk's
// cached symbol helper; never reuse offsets from another build or patch a coroutine.
// Audited menu entry samples: Taskbar.View.dll 2124.12106.10.0 and 2608.26001.200.0.
// Do NOT hook CLauncherTipContextMenu::ShowLauncherTipContextMenu: on an audited
// Windows 11 24H2 image it is a feature-usage reporting stub returning S_OK,
// not a request to create a menu. The original method must remain untouched.

// Microsoft x64 ABI audit: each non-trivial, one-pointer WinRT value parameter
// is passed indirectly as the address of an owned temporary. fire_and_forget is
// empty and returns in the low byte of RAX in the audited images. Use an explicit
// ABI thunk, not fake projected C++ types with the wrong destructor semantics.
// The full undecorated signatures below identify the same audited x64 symbols.
using ShowStartButtonContextMenuAsync_t = BYTE(WINAPI*)(void*, void*, void*, void*);
static ShowStartButtonContextMenuAsync_t ShowStartButtonContextMenuAsync_Original = nullptr;
using ViewCoordinatorComPtrDestructor_t = void(WINAPI*)(void*);
static ViewCoordinatorComPtrDestructor_t ViewCoordinatorComPtrDestructor = nullptr;
static_assert(sizeof(void*) == 8, "Native Win+X interception requires x64");

// Only a context already identified as Start by the shell reaches the modern
// guard. The pointer is used for monitor selection, NEVER to decide if it is Start.
static HWND NativeRequestTaskbar(const POINT* shellPoint) {
    POINT point = {};
    if (shellPoint) point = *shellPoint;
    else GetCursorPos(&point);
    HWND hit = WindowFromPoint(point);
    HWND root = hit ? GetAncestor(hit, GA_ROOT) : nullptr;
    DWORD pid = 0;
    if (root) GetWindowThreadProcessId(root, &pid);
    wchar_t cls[64] = {};
    if (root && pid == GetCurrentProcessId() && GetClassNameW(root, cls, _countof(cls)) &&
        (!_wcsicmp(cls, L"Shell_TrayWnd") || !_wcsicmp(cls, L"Shell_SecondaryTrayWnd")))
        return root;
    return FindOwnTaskbarWindow();
}

static void RedirectNativeWinX(const wchar_t* source, const POINT* point) {
    Wh_Log(L"[winx-native] suppressed %s; redirecting to the mod", source);
    if (!CloseWinXMenuIfOpen() && !PostWinXContextRequestFromHook(NativeRequestTaskbar(point), WinXRequestSource::ModernMenuEntry))
        Wh_Log(L"[winx-native] ERROR: request queue unavailable; native menu stays suppressed");
}

// The first three projected parameters contain COM interface pointers. Destroy
// only their temporary reference, not the caller's original projected objects.
// memcpy avoids type-punning an opaque projected wrapper as an IUnknown**.
static void DestroyProjectedTemporary(void* slot) {
    if (!slot) return;
    // This ABI slot holds one pointer, not the pointed-to COM object.
    constexpr std::size_t kPointerBytes = sizeof(void*);
    IUnknown* object = nullptr;
    std::memcpy(&object, slot, kPointerBytes);
    void* empty = nullptr;
    std::memcpy(slot, &empty, kPointerBytes);
    if (object) object->Release();
}

static BYTE WINAPI ShowStartButtonContextMenuAsync_Hook(
    void* elementTemporary, void* modelTemporary,
    void* frameTemporary, void* coordinatorTemporary) {
    if (g_unloading.load(std::memory_order_acquire) ||
        !g_modernNativeGuard.load(std::memory_order_acquire))
        return ShowStartButtonContextMenuAsync_Original(
            elementTemporary, modelTemporary, frameTemporary, coordinatorTemporary);

    // Leave TaskbarController::OnStartContextRequested completely untouched.
    // Its original checks decide whether there is an actual Win+X request.
    // Only this real menu-creation entry point is replaced, before allocation
    // or execution of the native coroutine. No partially started coroutine is
    // abandoned and ordinary Start/Win-key notifications are not redirected.
    RedirectNativeWinX(L"ContextMenus::ShowStartButtonContextMenuAsync", nullptr);
    DestroyProjectedTemporary(elementTemporary);
    DestroyProjectedTemporary(modelTemporary);
    DestroyProjectedTemporary(frameTemporary);
    // ViewCoordinator is an implementation object, not an IUnknown interface
    // pointer. Use its own matching-PDB com_ptr destructor, never a guessed vtable.
    if (coordinatorTemporary) ViewCoordinatorComPtrDestructor(coordinatorTemporary);
    return 0;
}

// A single attempt per mod lifetime, including a cache miss / missing PDB.
// Plain atomic state and SRWLOCK have no DLL-detach cleanup. The lock also fences
// in-flight delayed registration before Wh_ModBeforeUninit is allowed to return.
enum class NativeGuardState { WaitingForModule, Resolving, Ready, Unavailable };
static std::atomic<NativeGuardState> g_nativeGuardState{NativeGuardState::WaitingForModule};
static SRWLOCK g_nativeGuardLock = SRWLOCK_INIT;

static HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) module = GetModuleHandleW(L"ExplorerExtensions.dll");
    return module; // borrowed: Explorer owns the load and lifetime, not this mod
}

static bool HookNativeTaskbarView(HMODULE module, bool applyNow) {
#if !defined(_M_X64)
    // The opaque WinRT entry thunk was audited only for x64.
    (void)module;
    (void)applyNow;
    return false;
#else
    if (!module || g_unloading.load(std::memory_order_acquire) ||
        g_nativeGuardState.load(std::memory_order_acquire) != NativeGuardState::WaitingForModule)
        return false;
    // Symbol loading may recursively load DLLs. Never block a reentrant loader
    // callback on the same lock; AfterInit closes the initial load/watch race.
    if (!TryAcquireSRWLockExclusive(&g_nativeGuardLock)) return false;
    struct UnlockOnExit {
        SRWLOCK* lock;
        ~UnlockOnExit() { ReleaseSRWLockExclusive(lock); }
    } unlock{&g_nativeGuardLock}; // local only; never a global DLL-detach destructor
    if (g_unloading.load(std::memory_order_acquire) ||
        g_nativeGuardState.load(std::memory_order_relaxed) != NativeGuardState::WaitingForModule) {
        return false;
    }
    g_nativeGuardState.store(NativeGuardState::Resolving, std::memory_order_release);

    // Both full names identify the exact symbols preserved in our matching-PDB
    // audit. The spaced variants differ only in demangler formatting, not ABI.
    // Required destructor lookup and entry detour share ONE cached resolution.
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {LR"(public: __cdecl winrt::com_ptr<struct winrt::Taskbar::implementation::ViewCoordinator>::~com_ptr<struct winrt::Taskbar::implementation::ViewCoordinator>(void))"},
            &ViewCoordinatorComPtrDestructor,
        },
        {
            {
                LR"(struct winrt::fire_and_forget __cdecl winrt::Taskbar::implementation::ContextMenus::ShowStartButtonContextMenuAsync(struct winrt::Windows::UI::Xaml::FrameworkElement,struct winrt::WindowsUdk::UI::Shell::TaskbarModel,struct winrt::Taskbar::TaskbarFrame,struct winrt::com_ptr<struct winrt::Taskbar::implementation::ViewCoordinator>))",
                LR"(struct winrt::fire_and_forget __cdecl winrt::Taskbar::implementation::ContextMenus::ShowStartButtonContextMenuAsync(struct winrt::Windows::UI::Xaml::FrameworkElement, struct winrt::WindowsUdk::UI::Shell::TaskbarModel, struct winrt::Taskbar::TaskbarFrame, struct winrt::com_ptr<struct winrt::Taskbar::implementation::ViewCoordinator>))",
            },
            &ShowStartButtonContextMenuAsync_Original,
            ShowStartButtonContextMenuAsync_Hook,
        },
    };
    bool ok = WindhawkUtils::HookSymbols(module, taskbarViewHooks, ARRAYSIZE(taskbarViewHooks)) &&
              ViewCoordinatorComPtrDestructor && ShowStartButtonContextMenuAsync_Original;
    if (g_unloading.load(std::memory_order_acquire)) ok = false;
    // During Init Windhawk applies queued hooks itself. Delayed loader / AfterInit
    // installation must commit them explicitly. Keep the thunk forwarding until
    // both required pointers and (when needed) the commit have succeeded.
    if (ok && applyNow) ok = Wh_ApplyHookOperations() != FALSE;
    g_modernNativeGuard.store(ok, std::memory_order_release);
    g_nativeGuardState.store(ok ? NativeGuardState::Ready : NativeGuardState::Unavailable,
                            std::memory_order_release);
    Wh_Log(L"[winx-native] modern guard %s; legacy entry untouched; keyboard route retained",
           ok ? L"registered" : L"unavailable (matching symbols/setup failed)");
    return ok;
#endif
}

static void HandleLoadedTaskbarView(HMODULE module) {
    if (!g_unloading.load(std::memory_order_acquire) &&
        g_nativeGuardState.load(std::memory_order_acquire) == NativeGuardState::WaitingForModule &&
        module && GetTaskbarViewModuleHandle() == module)
        HookNativeTaskbarView(module, true);
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t LoadLibraryExW_Original = nullptr;
static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR file, HANDLE fileHandle, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(file, fileHandle, flags);
    // A data-file/resource mapping is not the executable module handle returned
    // by GetModuleHandleW and therefore cannot pass the identity check.
    HandleLoadedTaskbarView(module);
    return module;
}

static void InstallNativeWinXGuards() {
#if !defined(_M_X64)
    g_nativeGuardState.store(NativeGuardState::Unavailable, std::memory_order_release);
    Wh_Log(L"[winx-native] unaudited architecture: native guard disabled; keyboard/legacy routes retained");
#else
    if (HMODULE module = GetTaskbarViewModuleHandle()) {
        HookNativeTaskbarView(module, false);
        return;
    }
    // Do not load a shell component into a legacy shell or folder process. Observe
    // Explorer's normal loader, then register the modern guard only if it loads it.
    HMODULE kernelbase = GetModuleHandleW(L"kernelbase.dll");
    auto loader = kernelbase ? reinterpret_cast<LoadLibraryExW_t>(
        GetProcAddress(kernelbase, "LoadLibraryExW")) : nullptr;
    if (!loader || !WindhawkUtils::SetFunctionHook(loader, LoadLibraryExW_Hook,
                                                  &LoadLibraryExW_Original)) {
        Wh_Log(L"[winx-native] loader watch unavailable; keyboard/legacy routes retained");
        return;
    }
    Wh_Log(L"[winx-native] waiting for Explorer to load its taskbar view (no forced load)");
#endif
}

static LRESULT CALLBACK WinXLowLevelKeyboardProc(int nCode, WPARAM wParam,
                                                 LPARAM lParam) {
    // Win+X belongs to the mod: the chord is consumed and the menu is requested from the
    // services thread. Windows removes a low-level hook that does not answer within
    // LowLevelHooksTimeout (~300 ms), so nothing slow may happen in here.
    if (nCode == HC_ACTION && lParam &&
        g_winXKeyboardFallbackEnabled.load(std::memory_order_acquire)) {
        const auto* key = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
        const bool keyDown = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
        if (!keyDown && key->vkCode == 'X') g_winXXHeld = false;
        if (!keyDown && g_winXSwallowXUp && key->vkCode == 'X') {
            g_winXSwallowXUp = false;
            return 1;                       // the release of X is consumed too
        }
        if (keyDown && !(key->flags & LLKHF_INJECTED) && key->vkCode == 'X') {
            const bool winDown = (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0 ||
                                 (GetAsyncKeyState(VK_RWIN) & 0x8000) != 0;
            const bool otherModifierDown =
                (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0 ||
                (GetAsyncKeyState(VK_MENU) & 0x8000) != 0 ||
                (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
            if (winDown && !otherModifierDown) {
                // A second Win+X while the mod menu is on screen closes it (the
                // native menu behaves the same way) and the request is not
                // forwarded again: otherwise the queued request would be handled
                // right after the menu closed and the menu would pop up again.
                //
                // Key auto-repeat is ignored, so holding Win+X a little too long
                // does not repeat the chord, closing and reopening the menu over
                // and over (and injecting a key each time).
                const bool repeat = g_winXXHeld;
                g_winXXHeld = true;
                if (repeat) return 1;
                // Win+X opens the menu on the primary taskbar, as Windows 10 does,
                // so no Start button is passed: the services thread resolves it.
                if (!CloseWinXMenuIfOpen())
                    PostWinXContextRequestFromHook(nullptr, WinXRequestSource::KeyboardChord);

                // The shell must not see the chord. A dummy key (VK 0xE8, unassigned,
                // injected and therefore ignored by this hook) keeps the release of
                // Win from opening the Start menu.
                g_winXSwallowXUp = true;
                INPUT dummy[2] = {};
                // VK_F24 is bound by some OEM hotkey utilities; 0xE8 is the
                // unassigned virtual key used for exactly this purpose.
                dummy[0].type = INPUT_KEYBOARD; dummy[0].ki.wVk = 0xE8;
                dummy[1].type = INPUT_KEYBOARD; dummy[1].ki.wVk = 0xE8;
                dummy[1].ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(2, dummy, sizeof(INPUT));
                return 1;
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static LRESULT CALLBACK WinXLowLevelMouseProc(int nCode, WPARAM wParam,
                                              LPARAM lParam) {
    // A right-click on the Start button is answered by the mod: press and release are
    // consumed together, so no unmatched button-up can ever be leaked to the shell.
    // Only those two events are looked at, so a mouse move costs nothing here.
    if (nCode == HC_ACTION && lParam &&
        (wParam == WM_RBUTTONDOWN || wParam == WM_RBUTTONUP) &&
        g_winXMouseRouteEnabled.load(std::memory_order_acquire)) {
        const auto* mouse = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
        if (!(mouse->flags & LLMHF_INJECTED)) {
            if (wParam == WM_RBUTTONDOWN) {
                if (WinXStartButtonFromPoint(mouse->pt)) {
                    g_winXRightButtonDownOnStart = true;
                    return 1;
                }
            } else if (g_winXRightButtonDownOnStart) {
                g_winXRightButtonDownOnStart = false;
                if (!CloseWinXMenuIfOpen())
                    PostWinXContextRequestFromHook(WinXStartButtonFromPoint(mouse->pt),
                                                  WinXRequestSource::StartRightClick);
                return 1;
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Input-hook installation, retried by the services thread.
// ---------------------------------------------------------------------------
static bool EnsureWinXInputHooks() {
    if (!g_winXKeyboardHook || !g_winXMouseHook) {
        HMODULE module = nullptr;
        const DWORD flags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
        if (GetModuleHandleExW(flags, reinterpret_cast<LPCWSTR>(&g_winXHookModuleAnchor), &module)) {
            if (!g_winXKeyboardHook) {
                g_winXKeyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, WinXLowLevelKeyboardProc, module, 0);
                if (g_winXKeyboardHook) Wh_Log(L"[winx] Win+X keyboard hook installed");
            }
            if (!g_winXMouseHook) {
                g_winXMouseHook = SetWindowsHookExW(WH_MOUSE_LL, WinXLowLevelMouseProc, module, 0);
                if (g_winXMouseHook) Wh_Log(L"[winx] Start right-click hook installed");
            }
        }
    }
    g_winXKeyboardHookInstalled = g_winXKeyboardHook != nullptr;
    g_winXMouseHookInstalled = g_winXMouseHook != nullptr;
    return g_winXKeyboardHookInstalled || g_winXMouseHookInstalled;
}

// ---------------------------------------------------------------------------
// Owner window of the custom Win+X menu.
//
// Two Win32 rules decide whether a popup menu works at all: the owner window
// must have been created by the thread that calls TrackPopupMenu, and that
// thread must hold the foreground while the menu is open (with a WM_NULL posted
// afterwards so the menu also closes when the user clicks elsewhere).
// Shell_TrayWnd belongs to the shell's taskbar thread, which is why the menu is
// shown from a thread of the mod, with an owner window of that thread.
// ---------------------------------------------------------------------------
static const wchar_t kWinXOwnerClassName[] = L"Windows10WinXRestorer.MenuOwner";
static HINSTANCE g_winXOwnerClassModule = nullptr;   // set once the class is registered
static HWND g_winXOwnerWindow = nullptr;             // menu thread only
static DWORD g_winXOwnerWindowThread = 0;            // menu thread only

static LRESULT CALLBACK WinXOwnerWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                         LPARAM lParam) {
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Registers the class of the menu owner window once, from Wh_ModInit, and is undone
// by Wh_ModUninit. Windows does not unregister the classes of a DLL when the DLL is
// unloaded, and a registration left behind would carry the window procedure of the
// previous module image, which dangles as soon as a new build is laid out
// differently. For the same reason a class that is already registered is never
// reused: if RegisterClassExW fails, the mod stays disabled.
static bool RegisterWinXOwnerWindowClass() {
    if (g_winXOwnerClassModule) return true;

    HINSTANCE instance = nullptr;
    const DWORD flags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
    if (!GetModuleHandleExW(flags, reinterpret_cast<LPCWSTR>(&WinXOwnerWndProc), &instance)) {
        Wh_Log(L"[winx] owner window: module handle unavailable (%lu)", GetLastError());
        return false;
    }

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WinXOwnerWndProc;
    wc.hInstance = instance;
    wc.lpszClassName = kWinXOwnerClassName;
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"[winx] owner window class not registered (%lu)", GetLastError());
        return false;
    }
    g_winXOwnerClassModule = instance;
    return true;
}

// Called by Wh_ModUninit, after the menu thread has been joined and has destroyed
// its own window.
static void UnregisterWinXOwnerWindowClass() {
    if (!g_winXOwnerClassModule) return;
    UnregisterClassW(kWinXOwnerClassName, g_winXOwnerClassModule);
    g_winXOwnerClassModule = nullptr;
}

static bool ForceForegroundWindow(HWND hwnd) {
    if (!hwnd) return false;
    if (GetForegroundWindow() == hwnd) return true;
    // A no-op input event is the established Windhawk low-level-hook focus
    // pattern. No key/button is pressed and no input queues are attached.
    // UIPI can still reject injection/focus; never bypass it with queue attachment.
    INPUT input{};
    SendInput(1, &input, sizeof(INPUT));
    return SetForegroundWindow(hwnd) && GetForegroundWindow() == hwnd;
}

// Creates the window that owns the menu, once per menu thread.
static HWND GetWinXMenuOwnerWindow() {
    const DWORD thread = GetCurrentThreadId();
    if (g_winXOwnerWindow && g_winXOwnerWindowThread == thread &&
        IsWindow(g_winXOwnerWindow)) {
        return g_winXOwnerWindow;
    }
    g_winXOwnerWindow = nullptr;
    if (!g_winXOwnerClassModule) {
        Wh_Log(L"[winx] owner window: the window class is not registered");
        return nullptr;
    }

    // 1x1 off-screen popup, never activated by the user: it only anchors the menu.
    HWND hwnd = CreateWindowExW(WS_EX_TOOLWINDOW,
                                kWinXOwnerClassName, L"", WS_POPUP,
                                -32000, -32000, 1, 1, nullptr, nullptr,
                                g_winXOwnerClassModule, nullptr);
    if (!hwnd) {
        Wh_Log(L"[winx] owner window not created (%lu)", GetLastError());
        return nullptr;
    }
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);  // a hidden window cannot own a popup menu
    g_winXOwnerWindow = hwnd;
    g_winXOwnerWindowThread = thread;
    return hwnd;
}

// Destroys the owner window of the calling thread. Only the menu thread can do it:
// a window must be destroyed by the thread that created it, which is why this is
// called at the end of WinXMenuThread and nowhere else.
static void DestroyWinXMenuOwnerWindow() {
    if (g_winXOwnerWindow && g_winXOwnerWindowThread == GetCurrentThreadId()) {
        DestroyWindow(g_winXOwnerWindow);
        g_winXOwnerWindow = nullptr;
        g_winXOwnerWindowThread = 0;
    }
}

// Re-arms the input hooks. Called by the
// services thread only: the taskbar can appear after the thread starts and be
// recreated later, and a hook that failed to install is retried here instead of
// being given up for the whole session.
static void UpdateWinXInputRoutes() {
    static ULONGLONG nextCheck = 0;
    static bool routeReported = false;

    const ULONGLONG now = GetTickCount64();
    if (now < nextCheck) return;
    // Fast retries are bounded to startup; even a folder process which never
    // owns a taskbar returns to the slow cadence after ten seconds.
    const bool startupRetry = now - g_servicesStartedAt < 10000;
    g_winXRouteRefreshMs = startupRetry && (!g_winXKeyboardHook || !g_winXMouseHook) ? 500 : 2500;
    nextCheck = now + g_winXRouteRefreshMs;

    // The hooks are system-wide, so they are armed only in the process that owns the
    // taskbar. Until a visible taskbar of this explorer.exe is there, the mod does
    // nothing.
    if (!OwnsTaskbarWindow()) {
        ClearWinXInputRoutes();
        routeReported = false;
        return;
    }

    EnsureWinXInputHooks();
    if (g_winXKeyboardHook && g_winXMouseHook) {
        g_winXRouteRefreshMs = 2500;
        nextCheck = now + g_winXRouteRefreshMs;
    }

    // The chord is deliberately NOT registered with RegisterHotKey: Win-key
    // combinations are reserved to the OS (the registration fails on most builds).
    // The low-level hooks handle the chord instead. The mouse hook resolves the Start
    // button from the click itself, so nothing about it has to be kept up to date
    // here.
    g_winXKeyboardFallbackEnabled.store(g_winXKeyboardHookInstalled,
                                        std::memory_order_release);
    g_winXMouseRouteEnabled.store(g_winXMouseHookInstalled,
                                  std::memory_order_release);

    if (!routeReported && (g_winXKeyboardHookInstalled || g_winXMouseHookInstalled)) {
        routeReported = true;
        Wh_Log(L"[winx] Win+X routes enabled: input hooks (keyboard %s, mouse %s)",
               g_winXKeyboardHookInstalled ? L"yes" : L"no",
               g_winXMouseHookInstalled ? L"yes" : L"no");
    } else if (!routeReported) {
        // Installation is retried on the service cadence, so this is a status note rather
        // than a give-up message.
        routeReported = true;
        Wh_Log(L"[winx] no input hook is installed yet: it is retried every few seconds");
    }
}

// An entry opens a shell namespace when it is a shell URI: "shell:::{GUID}",
// "shell:AppsFolder\\...", "ms-settings:...".
static bool IsShellNamespaceCommand(const wchar_t* command) {
    if (!command) return false;
    // "shell:" covers both shell:... and shell:::{GUID}.
    return _wcsnicmp(command, L"shell:", 6) == 0 ||
           _wcsnicmp(command, L"ms-settings:", 12) == 0;
}

// Whether Windows 10 would shut this machine down through Fast Startup: the Fast
// Startup setting and hibernation itself must both be on, and only then is
// "shutdown.exe /s /hybrid" a valid command - with either of them off it fails, and
// ShellExecuteW would only see that shutdown.exe started. The registry is read, never
// written.
static bool IsHybridShutdownAvailable() {
    if (!IsPwrHibernateAllowed()) return false;
    DWORD value = 0, size = sizeof(value);
    return RegGetValueW(HKEY_LOCAL_MACHINE,
                        L"SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Power",
                        L"HiberbootEnabled", RRF_RT_REG_DWORD, nullptr, &value,
                        &size) == ERROR_SUCCESS &&
           value != 0;
}

// Splits "file parameters" (at the first space) and hands both to ShellExecuteW.
static void RunWinXCommandSplit(const wchar_t* command, const wchar_t* verb,
                                const wchar_t* what) noexcept {
    if (!command || !*command) return;
    wchar_t file[MAX_PATH] = {};
    const wchar_t* params = nullptr;
    const wchar_t* space = wcschr(command, L' ');
    if (space) {
        const size_t n = static_cast<size_t>(space - command);
        if (n >= _countof(file)) return;
        wmemcpy(file, command, n);
        file[n] = L'\0';
        params = space + 1;
    } else {
        wcsncpy_s(file, command, _TRUNCATE);
    }
    RunCommand(file, params, verb, what);
}

// Runs the command of a Win+X menu entry, on the menu thread.
//
// Accepted formats:
//   "@sleep"      suspend the system (see SuspendSystem);
//   "@shutdown"   shut down, through Fast Startup when it is available;
//   "@desktop"    show the desktop (see ToggleDesktop);
//   "@admin:exe"  program started with elevation (the "runas" verb);
//   "shell:...", "shell:::{GUID}", "ms-settings:..."   shell namespaces;
//   "file parameters"  any other program or applet.
//
// The selected command is written to the log before it runs, so a problem with an
// entry shows up at once. ShellExecuteW requires COM to be initialized on the
// calling thread: WinXMenuThread does that before showing the menu.
static void RunWinXCommand(const wchar_t* command) {
    if (!command || !*command) return;
    Wh_Log(L"[winx] selected entry: %s", command);

    if (_wcsicmp(command, L"@sleep") == 0) {
        SuspendSystem();
        return;
    }
    if (_wcsicmp(command, L"@desktop") == 0) {
        ToggleDesktop();
        return;
    }
    if (_wcsicmp(command, L"@shutdown") == 0) {
        // The same shutdown the Windows 10 entry performs: a hybrid one when Fast
        // Startup is available, a full one otherwise (see IsHybridShutdownAvailable).
        RunWinXCommandSplit(IsHybridShutdownAvailable() ? L"shutdown.exe /s /hybrid /t 0"
                                                        : L"shutdown.exe /s /t 0",
                            L"open", L"power entry");
        return;
    }
    // No "@search" branch: the Search entry was removed from the menu (see the
    // entry table), so nothing in this dispatcher can open the search host.
    if (wcsncmp(command, L"@admin:", 7) == 0) {
        RunWinXCommandSplit(command + 7, L"runas", L"administrative entry");
        return;
    }
    if (IsShellNamespaceCommand(command)) {
        if (OpenShellUri(command))
            return;
        // Same target, second documented way of starting it: some private shells do
        // not answer ShellExecute on the URI but do open the same namespace when
        // explorer.exe receives it. No substitute target: the URI is the same, only
        // who opens it changes.
        wchar_t throughExplorer[MAX_PATH + 96] = {};
        _snwprintf_s(throughExplorer, _countof(throughExplorer), _TRUNCATE,
                     L"explorer.exe %s", command);
        RunWinXCommandSplit(throughExplorer, L"open",
                            L"winx: shell namespace (explorer.exe)");
        return;
    }
    RunWinXCommandSplit(command, L"open", L"menu entry");
}

// WIN+X menu with all standard entries
//
// This menu is a recreation of the Windows 10 power-user (Win+X) menu drawn
// from Windows 10 reference screenshots: the same entries in the same order, the same
// two separators (after "Terminal (Admin)" and before "Shut down or sign out"),
// the same wording, the Windows 10 light/dark colours, the same row height and the
// single chevron of the submenu. It is a recreation, not the original component, but
// it is meant to look and behave like it (see ImmersiveMenu for the measurements).
//
// Shows a complete WIN+X menu with all standard Windows 10 entries:
// Programs and Features, Power Options, Event Viewer, System, Device Manager,
// Network Connections, Disk Management, Computer Management, Terminal,
// Terminal (Admin), Task Manager, Settings, File Explorer, Run,
// Shut down or sign out, Desktop. The Search entry of the original menu is not
// part of this recreation (see the entry table).
// The entries are localized for the UI language of the shell (15 languages).
// ---------------------------------------------------------------------------
// Win+X anchor - the Windows 10 position.
//
// Windows 10 opens the power-user menu glued to the Start button: its
// bottom-left corner sits on the top edge of the taskbar, at the left edge of
// the Start button, and it opens there whatever the pointer is doing. Falling
// back to the raw cursor whenever the Start button cannot be resolved would drop
// the menu on top of the taskbar instead of on its top edge, so the cursor is
// used only when there is no taskbar at all.
//
// Order of preference: Start button -> taskbar edge -> monitor corner. The
// pointer is used only when there is no taskbar at all. The WinXMenuOffsetX/Y
// settings shift the result if another look is wanted (the 2015 Windows 10
// reference screenshot is X=18, Y=+16: menu bottom edge 16 px below the top
// edge of the taskbar).
// ---------------------------------------------------------------------------
struct WinXMenuAnchor {
    POINT point{};
    UINT flags = TPM_LEFTALIGN | TPM_BOTTOMALIGN;
    const wchar_t* source = L"window corner";
    bool taskbarOnTop = false;
    UINT dpi = 0;   // scale of the monitor the menu opens on, 0 when it is unknown
};

static WinXMenuAnchor ComputeWinXMenuAnchor(HWND startButton) {
    WinXMenuAnchor anchor;

    if (g_winXMenuAnchorCursor) {
        GetCursorPos(&anchor.point);
        anchor.source = L"mouse pointer (legacy option)";
        return anchor;
    }

    // The services thread can run before the button is enumerable, and the
    // taskbar thread can rebuild it: resolve it again here, at show time.
    if (!startButton || !IsWindow(startButton)) startButton = FindNativeTaskbarStartButton();

    // The menu opens on the taskbar the request came from: the one that holds the
    // Start button that was clicked, which is a secondary taskbar on a multi-monitor
    // setup. The keyboard chord passes no button, so the primary taskbar is used, as
    // Windows 10 does.
    RECT taskbarRect = {};
    HWND taskbar = (startButton && IsWindow(startButton))
                       ? GetAncestor(startButton, GA_ROOT)
                       : nullptr;
    if (!taskbar) taskbar = FindOwnTaskbarWindow();
    if (taskbar) {
        DWORD pid = 0;
        GetWindowThreadProcessId(taskbar, &pid);
        if (pid != GetCurrentProcessId() || !GetWindowRect(taskbar, &taskbarRect))
            taskbar = nullptr;
    }

    RECT buttonRect = {};
    bool haveButton = startButton && startButton != taskbar && IsWindow(startButton) &&
                      GetWindowRect(startButton, &buttonRect);
    if (!haveButton && taskbar) haveButton = QueryStartButtonRectOnce(taskbar, &buttonRect);
    const bool taskbarVertical =
        taskbar && (taskbarRect.bottom - taskbarRect.top) >
                       (taskbarRect.right - taskbarRect.left);

    int x = 0;
    int y = 0;
    if (taskbar && taskbarVertical) {
        // Side-docked taskbar: Windows 10 grows the menu from its inner corner.
        // A rotated or side-docked taskbar is unsupported by the rest of the mod;
        // this only keeps the menu on screen instead of in the old cursor spot.
        x = static_cast<int>(taskbarRect.right);
        y = static_cast<int>(taskbarRect.bottom);
        anchor.source = L"vertical taskbar edge";
    } else if (taskbar) {
        MONITORINFO mi = {};
        mi.cbSize = sizeof(mi);
        HMONITOR monitor = MonitorFromRect(&taskbarRect, MONITOR_DEFAULTTONEAREST);
        anchor.taskbarOnTop = monitor && GetMonitorInfoW(monitor, &mi) &&
                              taskbarRect.top <= mi.rcMonitor.top + 1;
        if (anchor.taskbarOnTop) {
            anchor.flags = TPM_LEFTALIGN | TPM_TOPALIGN;
            y = static_cast<int>(taskbarRect.bottom);
        } else {
            y = static_cast<int>(taskbarRect.top);   // bottom edge, Windows 10 look
        }
        x = haveButton ? static_cast<int>(buttonRect.left)
                       : static_cast<int>(taskbarRect.left);
        anchor.source = haveButton ? L"Start button, glued to the taskbar top edge"
                                   : L"taskbar top-left edge";
    } else if (haveButton) {
        x = static_cast<int>(buttonRect.left);
        y = static_cast<int>(buttonRect.top);
        anchor.source = L"Start button only (no taskbar window)";
    } else {
        GetCursorPos(&anchor.point);
        anchor.source = L"mouse pointer (no taskbar found)";
        return anchor;
    }

    // Scale of the monitor the menu opens on, taken from a window that is on it:
    // the owner window of the menu is parked off-screen, so its own DPI would be
    // that of the primary monitor.
    if (taskbar)
        anchor.dpi = GetDpiForWindow(taskbar);
    else if (haveButton)
        anchor.dpi = GetDpiForWindow(startButton);

    // The offsets are a deliberate user choice and are applied after the
    // default anchor has been resolved, never before.
    anchor.point.x = x + g_winXMenuOffsetX;
    anchor.point.y = y + g_winXMenuOffsetY;
    return anchor;
}

static bool IsTerminalInstalled() {
    wchar_t path[MAX_PATH] = {};
    const DWORD n = SearchPathW(nullptr, L"wt.exe", nullptr, _countof(path), path, nullptr);
    if (!n || n >= _countof(path)) return false;
    const DWORD attrs = GetFileAttributesW(path);
    return attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}

static void ShowCustomWinXMenuHere(const WinXMenuAnchor& anchor) {
    const UiLangId lang = DetectUiLang();
    const bool usePowerShell = g_winXTerminalFallback.load() && !IsTerminalInstalled();
    const bool hideShutdown = g_winXHideShutdown.load();
    const bool win32Skin = g_winXMenuSkinWin32.load();
    
    // Create popup menu with all WIN+X entries
    ScopedMenu menu(CreatePopupMenu());
    if (!menu.get()) {
        Wh_Log(L"[winx] custom menu: CreatePopupMenu failed");
        return;
    }
    
    // WIN+X menu items with localized text and commands
    struct WinXItem {
        const wchar_t* text[LANG_COUNT];
        const wchar_t* command;
    };
    
    static const WinXItem items[] = {
        // Programs and Features: the wording of the Windows 10 menu of the
        // reference screenshot, not "Installed apps", and the target of that entry,
        // appwiz.cpl. ms-settings:appsfeatures would open the Windows 11 "Apps &
        // features" page, which is what Windows 10 called "Apps and Features".
        { { L"Programmi e funzionalit\u00e0", L"Programs and Features", L"Programmes et fonctionnalit\u00e9s", L"Programas y caracter\u00edsticas", L"Programme und Features",
            L"Programas e Recursos", L"Programma's en onderdelen", L"\u041f\u0440\u043e\u0433\u0440\u0430\u043c\u043c\u044b \u0438 \u043a\u043e\u043c\u043f\u043e\u043d\u0435\u043d\u0442\u044b", L"\u30d7\u30ed\u30b0\u30e9\u30e0\u3068\u6a5f\u80fd", L"Programy i funkcje", L"Programmer og funktioner", L"Program och funktioner", L"Programmer og funksjoner", L"Ohjelmat ja toiminnot", L"Programlar ve Özellikler"},
          L"control.exe appwiz.cpl" },
        // Power Options
        { { L"Opzioni di alimentazione", L"Power Options", L"Options d'alimentation", L"Opciones de energ\u00eda", L"Energieoptionen",
            L"Op\u00e7\u00f5es de energia", L"Energiebeheer", L"\u041f\u0430\u0440\u0430\u043c\u0435\u0442\u0440\u044b \u043f\u0438\u0442\u0430\u043d\u0438\u044f", L"\u96fb\u6e90\u30aa\u30d7\u30b7\u30e7\u30f3", L"Opcje zasilania", L"Strømstyring", L"Energialternativ", L"Strømalternativer", L"Virranhallinta", L"Güç Seçenekleri"},
          L"control.exe powercfg.cpl" },
        // Event Viewer
        { { L"Visualizzatore eventi", L"Event Viewer", L"Observateur d'\u00e9v\u00e9nements", L"Visor de eventos", L"Ereignisanzeige",
            L"Visualizador de Eventos", L"Gebeurtenisweergave", L"\u041f\u0440\u043e\u0441\u043c\u043e\u0442\u0440 \u0441\u043e\u0431\u044b\u0442\u0438\u0439", L"\u30a4\u30d9\u30f3\u30c8 \u30d3\u30e5\u30fc\u30a2\u30fc", L"Podgl\u0105d zdarze\u0144", L"Hændelsesfremviser", L"Loggboken", L"Hendelsesvisning", L"Tapahtumakatselu", L"Olay Görüntüleyicisi"},
          L"eventvwr.msc" },
        // System
        { { L"Sistema", L"System", L"Syst\u00e8me", L"Sistema", L"System",
            L"Sistema", L"Systeem", L"\u0421\u0438\u0441\u0442\u0435\u043c\u0430", L"\u30b7\u30b9\u30c6\u30e0", L"System", L"System", L"System", L"System", L"Järjestelmä", L"Sistem"},
          L"ms-settings:about" },
        // Device Manager
        { { L"Gestione dispositivi", L"Device Manager", L"Gestionnaire de p\u00e9riph\u00e9riques", L"Administrador de dispositivos", L"Ger\u00e4temanager",
            L"Gestor de Dispositivos", L"Apparaatbeheer", L"\u0414\u0438\u0441\u043f\u0435\u0442\u0447\u0435\u0440 \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432", L"\u30c7\u30d0\u30a4\u30b9 \u30de\u30cd\u30fc\u30b8\u30e3\u30fc", L"Mened\u017cer urz\u0105dze\u0144", L"Enhedshåndtering", L"Enhetshanteraren", L"Enhetsbehandling", L"Laitehallinta", L"Aygıt Yöneticisi"},
          L"devmgmt.msc" },
        // Network Connections
        { { L"Connessioni di rete", L"Network Connections", L"Connexions r\u00e9seau", L"Conexiones de red", L"Netzwerkverbindungen",
            L"Liga\u00e7\u00f5es de Rede", L"Netwerkverbindingen", L"\u0421\u0435\u0442\u0435\u0432\u044b\u0435 \u043f\u043e\u0434\u043a\u043b\u044e\u0447\u0435\u043d\u0438\u044f", L"\u30cd\u30c3\u30c8\u30ef\u30fc\u30af\u63a5\u7d9a", L"Po\u0142\u0105czenia sieciowe", L"Netværksforbindelser", L"Nätverksanslutningar", L"Nettverkstilkoblinger", L"Verkkoyhteydet", L"Ağ Bağlantıları"},
          // ncpa.cpl does nothing on the private shell; the shell namespace below
          // is opened like every other shell operation.
          L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}" },
        // Disk Management
        { { L"Gestione disco", L"Disk Management", L"Gestion des disques", L"Administraci\u00f3n de discos", L"Datentr\u00e4gerverwaltung",
            L"Gest\u00e3o de Discos", L"Schijfbeheer", L"\u0423\u043f\u0440\u0430\u0432\u043b\u0435\u043d\u0438\u0435 \u0434\u0438\u0441\u043a\u0430\u043c\u0438", L"\u30c7\u30a3\u30b9\u30af\u306e\u7ba1\u7406", L"Zarz\u0105dzanie dyskami", L"Diskhåndtering", L"Diskhantering", L"Diskbehandling", L"Levynhallinta", L"Disk Yönetimi"},
          L"diskmgmt.msc" },
        // Computer Management
        { { L"Gestione computer", L"Computer Management", L"Gestion de l'ordinateur", L"Administraci\u00f3n del equipo", L"Computerverwaltung",
            L"Gest\u00e3o do Computador", L"Computerbeheer", L"\u0423\u043f\u0440\u0430\u0432\u043b\u0435\u043d\u0438\u0435 \u043a\u043e\u043c\u043f\u044c\u044e\u0442\u0435\u0440\u043e\u043c", L"\u30b3\u30f3\u30d4\u30e5\u30fc\u30bf\u30fc\u306e\u7ba1\u7406", L"Zarz\u0105dzanie komputerem", L"Computerstyring", L"Datorhantering", L"Datamaskinbehandling", L"Tietokoneenhallinta", L"Bilgisayar Yönetimi"},
          L"compmgmt.msc" },
        // Terminal
        { { L"Terminale", L"Terminal", L"Terminal", L"Terminal", L"Terminal",
            L"Terminal", L"Terminal", L"\u0422\u0435\u0440\u043c\u0438\u043d\u0430\u043b", L"\u30bf\u30fc\u30df\u30ca\u30eb", L"Terminal", L"Terminal", L"Terminal", L"Terminal", L"Pääte", L"Terminal"},
          L"wt.exe" },
        // Terminal (Admin)
        { { L"Terminale (Amministratore)", L"Terminal (Admin)", L"Terminal (Administrateur)", L"Terminal (Administrador)", L"Terminal (Admin)",
            L"Terminal (Administrador)", L"Terminal (Administrator)", L"\u0422\u0435\u0440\u043c\u0438\u043d\u0430\u043b (\u0410\u0434\u043c\u0438\u043d\u0438\u0441\u0442\u0440\u0430\u0442\u043e\u0440)", L"\u30bf\u30fc\u30df\u30ca\u30eb (\u7ba1\u7406\u8005)", L"Terminal (Administrator)", L"Terminal (administrator)", L"Terminal (administratör)", L"Terminal (administrator)", L"Pääte (järjestelmänvalvoja)", L"Terminal (Yönetici)"},
          L"@admin:wt.exe" },
        // Task Manager
        { { L"Gestione attivit\u00e0", L"Task Manager", L"Gestionnaire des t\u00e2ches", L"Administrador de tareas", L"Task-Manager",
            L"Gestor de Tarefas", L"Taakbeheer", L"\u0414\u0438\u0441\u043f\u0435\u0442\u0447\u0435\u0440 \u0437\u0430\u0434\u0430\u0447", L"\u30bf\u30b9\u30af \u30de\u30cd\u30fc\u30b8\u30e3\u30fc", L"Mened\u017cer zada\u0144", L"Jobliste", L"Aktivitetshanteraren", L"Oppgavebehandling", L"Tehtävienhallinta", L"Görev Yöneticisi"},
          L"taskmgr.exe" },
        // Settings
        { { L"Impostazioni", L"Settings", L"Param\u00e8tres", L"Configuraci\u00f3n", L"Einstellungen",
            L"Defini\u00e7\u00f5es", L"Instellingen", L"\u041f\u0430\u0440\u0430\u043c\u0435\u0442\u0440\u044b", L"\u8a2d\u5b9a", L"Ustawienia", L"Indstillinger", L"Inställningar", L"Innstillinger", L"Asetukset", L"Ayarlar"},
          L"ms-settings:" },
        // File Explorer
        { { L"Esplora file", L"File Explorer", L"Explorateur de fichiers", L"Explorador de archivos", L"Datei-Explorer",
            L"Explorador de Ficheiros", L"Verkenner", L"\u041f\u0440\u043e\u0432\u043e\u0434\u043d\u0438\u043a \u0444\u0430\u0439\u043b\u043e\u0432", L"\u30a8\u30af\u30b9\u30d7\u30ed\u30fc\u30e9\u30fc", L"Eksplorator plik\u00f3w", L"Stifinder", L"Utforskaren", L"Filutforsker", L"Resurssienhallinta", L"Dosya Gezgini"},
          L"explorer.exe" },
        // The Windows 10 menu has a Search entry here. It is deliberately not part
        // of this recreation: opening the Windows 11 search host from a shell menu
        // was the least reliable entry of the whole menu, so the entry was removed
        // and Windows Search is opened with Win+S, the taskbar Search box or Start.
        // Run
        { { L"Esegui", L"Run", L"Ex\u00e9cuter", L"Ejecutar", L"Ausf\u00fchren",
            L"Executar", L"Uitvoeren", L"\u0412\u044b\u043f\u043e\u043b\u043d\u0438\u0442\u044c", L"\u30d5\u30a1\u30a4\u30eb\u540d\u3092\u6307\u5b9a\u3057\u3066\u5b9f\u884c", L"Uruchom", L"Kør", L"Kör", L"Kjør", L"Suorita", L"Çalıştır"},
          L"shell:::{2559a1f3-21d7-11d4-bdaf-00c04f60b9f0}" },
        // Shut down or sign out
        { { L"Arresta o disconnetti", L"Shut down or sign out", L"Arr\u00eater ou d\u00e9connecter", L"Cerrar sesi\u00f3n o apagar", L"Herunterfahren oder abmelden",
            L"Terminar Sess\u00e3o ou Desligar", L"Afsluiten of afmelden", L"\u0417\u0430\u0432\u0435\u0440\u0448\u0435\u043d\u0438\u0435 \u0440\u0430\u0431\u043e\u0442\u044b \u0438\u043b\u0438 \u0432\u044b\u0445\u043e\u0434 \u0438\u0437 \u0441\u0438\u0441\u0442\u0435\u043c\u044b", L"\u30b7\u30e3\u30c3\u30c8\u30c0\u30a6\u30f3\u307e\u305f\u306f\u30b5\u30a4\u30f3\u30a2\u30a6\u30c8", L"Zamknij lub wyloguj", L"Luk computeren, eller log af", L"Stäng av eller logga ut", L"Slå av eller logg av", L"Sammuta tai kirjaudu ulos", L"Kapat veya oturumu kapat"},
          L"explorer.exe shell:::{85BBD9B8-4226-101A-A96C-945596089032}" },
        // Desktop
        { { L"Desktop", L"Desktop", L"Bureau", L"Escritorio", L"Desktop",
            L"\u00c1rea de Trabalho", L"Bureaublad", L"\u0420\u0430\u0431\u043e\u0447\u0438\u0439 \u0441\u0442\u043e\u043b", L"\u30c7\u30b9\u30af\u30c8\u30c3\u30d7", L"Pulpit", L"Skrivebord", L"Skrivbord", L"Skrivebord", L"Työpöytä", L"Masaüstü"},
          L"@desktop" }
    };
    
    // Shut-down entries of the submenu (as in Windows 10), in every interface
    // language: the order is the one of UiLangId.
    static const wchar_t* const kPowerText[4][LANG_COUNT] = {
        { L"Disconnetti", L"Sign out", L"Se déconnecter", L"Cerrar sesión", L"Abmelden", L"Terminar sessão", L"Afmelden", L"Выход из системы", L"サインアウト", L"Wyloguj", L"Log af", L"Logga ut", L"Logg av", L"Kirjaudu ulos", L"Oturumu kapat" },
        { L"Sospendi", L"Sleep", L"Mettre en veille", L"Suspender", L"Energiesparmodus", L"Suspender", L"Sluimerstand", L"Спящий режим", L"スリープ", L"Uśpij", L"Slumre", L"Viloläge", L"Dvalemodus", L"Lepotila", L"Uyku" },
        { L"Arresta il sistema", L"Shut down", L"Arrêter", L"Apagar", L"Herunterfahren", L"Encerrar", L"Afsluiten", L"Завершение работы", L"シャットダウン", L"Zamknij", L"Luk computeren", L"Stäng av", L"Slå av", L"Sammuta", L"Kapat" },
        { L"Riavvia il sistema", L"Restart", L"Redémarrer", L"Reiniciar", L"Neu starten", L"Reiniciar", L"Opnieuw opstarten", L"Перезагрузка", L"再起動", L"Uruchom ponownie", L"Genstart", L"Starta om", L"Start på nytt", L"Käynnistä uudelleen", L"Yeniden başlat" },
    };
    static const wchar_t* const kPowerCmd[4] = {
        L"shutdown.exe /l",
        L"@sleep",
        L"@shutdown",
        L"shutdown.exe /r /t 0",
    };
    // The language index is used by the shut-down submenu as well.

    // Same order as the Windows 10 Win+X menu, with its two separators: after the
    // administrative terminal and before "Shut down or sign out".
    for (size_t i = 0; i < _countof(items); i++) {
        if (hideShutdown && i == 14) continue;
        if (i == 10 || i == 14) AppendMenuW(menu.get(), MF_SEPARATOR, 0, nullptr);
        if (i == 14) {
            HMENU sub = CreatePopupMenu();
            if (sub) {
                for (UINT k = 0; k < 4; k++)
                    AppendMenuW(sub, MF_STRING, 101 + k, kPowerText[k][lang]);
                if (!AppendMenuW(menu.get(), MF_POPUP | MF_STRING,
                                 (UINT_PTR)sub, items[i].text[lang])) DestroyMenu(sub);
            }
            // Never turn a failed power submenu into an unrelated executable item.
            continue;
        }
        if (usePowerShell && (i == 8 || i == 9)) {
            wchar_t text[192] = {};
            if (i == 8) {
                wcscpy_s(text, L"Windows PowerShell");
            } else {
                // Keep the localized elevation suffix from Terminal (Admin).
                const wchar_t* suffix = wcschr(items[i].text[lang], L'(');
                _snwprintf_s(text, _countof(text), _TRUNCATE,
                             L"Windows PowerShell %s", suffix ? suffix : L"(Admin)");
            }
            AppendMenuW(menu.get(), MF_STRING, (UINT)(i + 1), text);
            continue;
        }
        AppendMenuW(menu.get(), MF_STRING, (UINT)(i + 1), items[i].text[lang]);
    }

    if (win32Skin && !NativeMenu::MarkOwnedTree(menu.get()))
        Wh_Log(L"[menu] could not mark the owned Win+X menu tree (%lu)", GetLastError());

    // The current opening's anchor was resolved on this same thread before STA
    // initialization / window creation, allowing one safe MTA UIA lookup if needed.
    const POINT pt = anchor.point;
    const UINT alignFlags = anchor.flags;
    Wh_Log(L"[winx] anchor: %s -> (%ld,%ld)%s, dpi %u", anchor.source, pt.x, pt.y,
           anchor.taskbarOnTop ? L" [taskbar on top]" : L"", anchor.dpi);

    // Owner: a window of this thread. Shell_TrayWnd is never used as the owner: the
    // menu must not run its modal loop on the taskbar thread.
    HWND owner = GetWinXMenuOwnerWindow();
    if (!owner) {
        // Without an owner window of this thread the menu could not be dismissed
        // reliably (there would be no window to receive WM_CANCELMODE), so the
        // request is dropped instead of showing an unowned popup.
        Wh_Log(L"[winx] no owner window for the menu: the request is dropped");
        return;
    }
    if (!ForceForegroundWindow(owner))
        Wh_Log(L"[winx] foreground activation refused (Windows/UIPI); no input queues attached");

    // The flag is set for exactly as long as the menu is on screen, so a second
    // chord can tell "already open" from "not open" (see WinXMenuOpenScope).
    Wh_Log(L"[winx] menu on thread %lu, owner 0x%p", GetCurrentThreadId(), (void*)owner);

    UINT selected = 0;
    {
        const WinXMenuOpenScope menuOpen(owner);
        // Rendering remains independent of input interception. Light Win32 mode
        // stays system-rendered; dark Win32 mode paints only our owned menu items.
        const UINT flags = TPM_RIGHTBUTTON | TPM_RETURNCMD | alignFlags;
        Wh_Log(L"[winx] skin: %s, PowerShell fallback: %s, hide power: %s",
               win32Skin ? L"win32" : L"immersive",
               usePowerShell ? L"yes" : L"no", hideShutdown ? L"yes" : L"no");
        selected = win32Skin
            ? NativeMenu::Track(menu.get(), owner, pt.x, pt.y, flags, anchor.dpi)
            : ImmersiveMenu::Track(menu.get(), owner, pt.x, pt.y, flags, anchor.dpi);
        PostMessageW(owner, WM_NULL, 0, 0);
    } // A launched command is not an open menu.


    if (selected >= 101 && selected <= 104)
        RunWinXCommand(kPowerCmd[selected - 101]);
    else if ((selected == 9 || selected == 10) && usePowerShell) {
        wchar_t path[MAX_PATH] = {};
        const UINT n = GetSystemDirectoryW(path, _countof(path));
        if (n && n < _countof(path) &&
            wcscat_s(path, L"\\WindowsPowerShell\\v1.0\\powershell.exe") == 0)
            RunCommand(path, nullptr, selected == 10 ? L"runas" : L"open", L"PowerShell");
    }
    else if (selected > 0 && selected <= _countof(items))
        RunWinXCommand(items[selected - 1].command);
}

// ---------------------------------------------------------------------------
// The thread that shows the menu.
//
// TrackPopupMenuEx runs a modal loop on the thread that calls it, so the menu is
// never shown on a thread of the shell: doing it on the taskbar thread froze the
// private shell for as long as the menu was on screen (the desktop went black for
// seconds and the tray flyouts stopped answering). A thread of the mod shows it,
// with an owner window of its own (GetWinXMenuOwnerWindow is per-thread), and
// destroys that window before exiting, on the only thread that can destroy it.
//
// Placement is resolved first, before a window exists (UIA uses a temporary MTA).
// Then COM STA is initialized for ShellExecuteW, shell namespaces, ms-settings,
// and elevated entries. The owner window is destroyed before STA uninitialization.
// ---------------------------------------------------------------------------
static DWORD WINAPI WinXMenuThread(LPVOID parameter) {
    const HWND startButton = static_cast<HWND>(parameter);
    if (g_unloading.load(std::memory_order_acquire)) return 0;
    // UIA must run on a windowless MTA thread. Resolve the value-only anchor first;
    // QueryStartButtonRectOnce releases its MTA before this thread becomes an STA.
    const WinXMenuAnchor anchor = ComputeWinXMenuAnchor(startButton);
    const HRESULT comResult =
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(comResult))
        Wh_Log(L"[winx] menu thread: CoInitializeEx failed (0x%08X)", (unsigned)comResult);
    // The owner window needs a message queue on this thread.
    MSG queueInit{};
    PeekMessageW(&queueInit, nullptr, 0, 0, PM_NOREMOVE);

    if (g_unloading.load(std::memory_order_acquire)) {
        Wh_Log(L"[winx] menu thread %lu: the mod is unloading, no menu is shown",
               GetCurrentThreadId());
    } else {
        Wh_Log(L"[winx] menu thread %lu: showing the menu (the shell thread is not used)",
               GetCurrentThreadId());
        ShowCustomWinXMenuHere(anchor);
    }

    DestroyWinXMenuOwnerWindow();
    if (SUCCEEDED(comResult)) CoUninitialize();
    return 0;
}

// Starts the thread that shows the menu. The caller is the services thread, which
// owns the low-level input hooks and must never block (Windows removes a hook that
// does not answer within LowLevelHooksTimeout), so this only creates the thread.
//
// One menu at a time, and the handle is kept: the menu thread runs code of this
// module (the owner-draw subclass and the dispatch of the chosen entry), so the
// module must not be unloaded while it is alive. Wh_ModUninit cancels it and waits
// for it. When the thread cannot be created the request is dropped: showing the menu
// on the services thread would run a modal loop on the thread that services the
// system-wide hooks, which is exactly what this mod must not do.
static void ShowCustomWinXMenu(HWND startButton) {
    if (g_menuThread) {
        if (WaitForSingleObject(g_menuThread, 0) == WAIT_TIMEOUT) {
            Wh_Log(L"[winx] the previous menu is still open: this request is dropped");
            return;
        }
        CloseHandle(g_menuThread);
        g_menuThread = nullptr;
    }
    g_menuThread = CreateThread(nullptr, 0, WinXMenuThread,
                                reinterpret_cast<LPVOID>(startButton), 0, nullptr);
    if (!g_menuThread)
        Wh_Log(L"[winx] the menu thread could not be created (%lu): the request is dropped",
               GetLastError());
}

// Handles a Win+X request posted by the input hooks, with a short debounce so that a
// burst of requests opens one menu. startButton is the button a right-click came
// from, or null for the keyboard chord.
static void HandleWinXRequest(HWND startButton, WinXRequestSource source) {
    // A call to a deprecated shell stub is NOT a fourth request source.
    const wchar_t* sourceName = nullptr;
    switch (source) {
        case WinXRequestSource::KeyboardChord: sourceName = L"keyboard Win+X"; break;
        case WinXRequestSource::StartRightClick: sourceName = L"Start right-click (HWND)"; break;
        case WinXRequestSource::ModernMenuEntry: sourceName = L"modern menu creation entry"; break;
        default:
            Wh_Log(L"[winx] unknown request source rejected");
            return;
    }
    Wh_Log(L"[winx] request source: %s", sourceName);
    static ULONGLONG lastRequest = 0;

    const ULONGLONG now = GetTickCount64();
    if (lastRequest && now - lastRequest < 400) return;
    lastRequest = now;

    if (!startButton || !IsWindow(startButton))
        startButton = FindNativeTaskbarStartButton();
    ShowCustomWinXMenu(startButton);
}

// Tears down what the services thread owns, when it exits. The hooks are removed
// here, on the thread that installed them: Wh_ModBeforeUninit does not touch them,
// because it runs on a thread of the Windhawk engine while this thread may still be
// installing or resetting them. The state of the open menu (g_winXMenuOpen and
// g_winXMenuOwnerWindow) belongs to the menu thread and is left alone:
// Wh_ModUninit needs it to cancel a menu that is still on screen.
class ScopedWinXServiceStateReset {
public:
    ScopedWinXServiceStateReset() = default;
    ~ScopedWinXServiceStateReset() {
        ClearWinXInputRoutes();
        ResetWinXInputHooks();
        g_winXKeyboardHookInstalled = false;
        g_winXMouseHookInstalled = false;
        g_servicesThreadId = 0;
    }
    ScopedWinXServiceStateReset(const ScopedWinXServiceStateReset&) = delete;
    ScopedWinXServiceStateReset& operator=(const ScopedWinXServiceStateReset&) = delete;
};

// ---------------------------------------------------------------------------
// Services thread: owns the input hooks and pumps messages. Nothing slow and
// nothing that can block is ever executed here, so the hooks are neither removed by
// Windows for exceeding LowLevelHooksTimeout nor able to hold up system input: a
// hook posts g_winXRouteMessage and this thread does the work.
// ---------------------------------------------------------------------------
static HANDLE g_stopEvent = nullptr;
static HANDLE g_servicesThread = nullptr;
static HANDLE g_servicesReadyEvent = nullptr;

static DWORD WINAPI ExplorerServicesThread(LPVOID) {
    ScopedWinXServiceStateReset stateReset;
    g_servicesThreadId = GetCurrentThreadId();
    g_servicesStartedAt = GetTickCount64();
    MSG queueInit{};
    PeekMessageW(&queueInit, nullptr, 0, 0, PM_NOREMOVE);
    // Wh_ModInit waits for an actual queue, not an arbitrary startup sleep. The
    // PDB detours must have somewhere to post as soon as Windhawk enables them.
    if (g_servicesReadyEvent) SetEvent(g_servicesReadyEvent);

    // Arms the hooks as soon as this process owns the taskbar, and keeps them armed
    // from then on.
    UpdateWinXInputRoutes();
    Wh_Log(L"[winx] input hooks: keyboard %s, Start right-click %s",
           g_winXKeyboardHook ? L"ready" : L"unavailable",
           g_winXMouseHook ? L"ready" : L"unavailable");

    // Requests and shutdown wake this thread immediately. Idle refresh is 2.5 s
    // once armed, or after bounded startup retries, not a permanent 500 ms poll.
    for (;;) {
        const DWORD waitResult = MsgWaitForMultipleObjectsEx(
            g_stopEvent ? 1 : 0, g_stopEvent ? &g_stopEvent : nullptr,
            g_winXRouteRefreshMs, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (g_stopEvent && waitResult == WAIT_OBJECT_0) break;

        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == g_winXRouteMessage) {
                if (!g_unloading.load())
                    HandleWinXRequest(reinterpret_cast<HWND>(msg.lParam),
                                      static_cast<WinXRequestSource>(msg.wParam));
            } else {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }

        // The taskbar can appear after this thread starts (and be recreated later):
        // the routes are refreshed at every wake-up, so they are in place as soon as
        // it is, and a hook that failed to install is retried.
        UpdateWinXInputRoutes();
    }
    Wh_Log(L"[winx] services finished");
    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    Wh_Log(L"[winx] init");

    g_unloading.store(false, std::memory_order_release);
    g_winXRouteMessage = RegisterWindowMessageW(kWinXRouteMessageName);
    if (!g_winXRouteMessage) {
        Wh_Log(L"[winx] the route message could not be registered (%lu)", GetLastError());
        return FALSE;
    }

    if (!RegisterWinXOwnerWindowClass())
        return FALSE;

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"[winx] stop event could not be created (%lu)", GetLastError());
        UnregisterWinXOwnerWindowClass();
        return FALSE;
    }
    g_servicesReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_servicesReadyEvent) {
        Wh_Log(L"[winx] services-ready event failed (%lu)", GetLastError());
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        UnregisterWinXOwnerWindowClass();
        return FALSE;
    }
    g_servicesThread = CreateThread(nullptr, 0, ExplorerServicesThread, nullptr, 0, nullptr);
    if (!g_servicesThread) {
        Wh_Log(L"[winx] services thread not created (%lu)", GetLastError());
        SetEvent(g_stopEvent);
        CloseHandle(g_servicesReadyEvent);
        g_servicesReadyEvent = nullptr;
        if (g_stopEvent) {
            CloseHandle(g_stopEvent);
            g_stopEvent = nullptr;
        }
        UnregisterWinXOwnerWindowClass();
        return FALSE;
    }
    if (WaitForSingleObject(g_servicesReadyEvent, 5000) != WAIT_OBJECT_0) {
        Wh_Log(L"[winx-native] INIT FAILED: services message queue did not become ready");
        g_unloading.store(true);
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_servicesThread, INFINITE);
        CloseHandle(g_servicesThread);
        CloseHandle(g_servicesReadyEvent);
        CloseHandle(g_stopEvent);
        g_servicesThread = nullptr;
        g_servicesReadyEvent = nullptr;
        g_stopEvent = nullptr;
        UnregisterWinXOwnerWindowClass();
        return FALSE;
    }
    InstallNativeWinXGuards();
    return TRUE;
}

void Wh_ModAfterInit() {
    // Close the race where Explorer loaded the DLL between Init's handle check
    // and activation of the loader watch. No forced load and no repeated PDB scan.
    if (HMODULE module = GetTaskbarViewModuleHandle()) HandleLoadedTaskbarView(module);
    Wh_Log(L"[winx-native] ACTIVE: modern menu guard %s; legacy method untouched; request queue ready",
           g_modernNativeGuard.load() ? L"yes" : L"not present");
}

void Wh_ModBeforeUninit() {
    // Stop a menu thread that has just been created from showing a menu, then wave the
    // services thread off. The hooks are taken down by the services thread itself, on
    // the thread that installed them (see ScopedWinXServiceStateReset).
    g_unloading.store(true, std::memory_order_release);
    if (g_stopEvent) SetEvent(g_stopEvent);
    AcquireSRWLockExclusive(&g_nativeGuardLock);
    ReleaseSRWLockExclusive(&g_nativeGuardLock);
}

void Wh_ModUninit() {
    // Be safe even if the host did not call BeforeUninit first.
    g_unloading.store(true, std::memory_order_release);
    if (g_stopEvent) SetEvent(g_stopEvent);
    AcquireSRWLockExclusive(&g_nativeGuardLock);
    ReleaseSRWLockExclusive(&g_nativeGuardLock);
    if (g_servicesThread) {
        // No timeout: the thread is waved off through its event, and it never waits on
        // anything this thread holds. Joining it first also means that no new menu
        // thread can be started from here on.
        WaitForSingleObject(g_servicesThread, INFINITE);
        CloseHandle(g_servicesThread);
        g_servicesThread = nullptr;
    }
    if (g_menuThread) {
        // The menu thread can still be inside the modal loop of TrackPopupMenuEx, or
        // inside ShellExecuteW for the entry that was chosen. Its return address, the
        // window procedure that draws the items and the dispatch code are in this
        // module, so the module must not be unloaded before the thread is gone: an
        // open menu is cancelled first (WM_CANCELMODE, posted to the window that owns
        // it) and the thread is then waited for.
        DWORD waitedMs = 0;
        while (WaitForSingleObject(g_menuThread, 100) == WAIT_TIMEOUT) {
            if (const HWND owner = g_winXMenuOwnerWindow.load(std::memory_order_acquire))
                PostMessageW(owner, WM_CANCELMODE, 0, 0);
            waitedMs += 100;
            if (waitedMs % 5000 == 0)
                Wh_Log(L"[winx] waiting for the menu thread to finish (%lu ms)", waitedMs);
        }
        CloseHandle(g_menuThread);
        g_menuThread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_servicesReadyEvent) {
        CloseHandle(g_servicesReadyEvent);
        g_servicesReadyEvent = nullptr;
    }
    UnregisterWinXOwnerWindowClass();
    g_servicesThreadId = 0;
    Wh_Log(L"[winx] unloaded: no hook, no thread, no window and no class of this mod is left");
}

void Wh_ModSettingsChanged() {
    // Windhawk calls this from its own thread. Every setting is read when the menu is
    // built, so reloading them here is all that is needed: the hooks belong to the
    // services thread and are left alone.
    LoadSettings();
}

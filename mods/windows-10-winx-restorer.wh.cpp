// ==WindhawkMod==
// @id              windows-10-winx-restorer
// @name            Windows 10 Win+X restorer on Win11 24H2
// @description     This mod restores the Windows 10 power user menu (Win+X) when the Windows 10 taskbar runs on Windows 11 24H2 or 25H2
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @architecture    x86-64
// @include         explorer.exe
// @compilerOptions -luser32 -lgdi32 -lshell32 -lcomctl32 -lole32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 10 Win+X restorer on Win11 24H2

This mod restores the power user menu (Win+X), without touching the registry and without replacing system files.

Win+X opens the Windows 10 style menu of this mod on every chord; the native menu is not called into play.

The menu is a recreation of the Windows 10 power user menu (Installed apps, Power Options, Event Viewer,
System, Device Manager, Network Connections, Disk Management, Computer Management,
Terminal, Terminal (Admin), Task Manager, Settings, File Explorer, Run, Shut down or sign
out, Desktop), localized in the same languages as the rebuilt Windows 10 shell menus.

This mod has been tested on Windows 11 24H2.

## How it works

* A dedicated thread owns the low-level input hooks and only pumps messages. Nothing slow
  runs on the hook thread, so Windows never removes the hook for exceeding
  `LowLevelHooksTimeout` and system input is never held up: the hook posts a private
  message and the thread does the work.
* The menu is created on a thread of the mod, with an owner window of that thread: the
  taskbar thread is never used to show it. `TrackPopupMenuEx` runs a modal loop on the
  thread that calls it, and doing that on the taskbar thread froze the private shell for
  as long as the menu was on screen (the desktop went black for seconds and the tray
  flyouts stopped answering).
* A subclass of `Shell_TrayWnd` (installed with
  `WindhawkUtils::SetWindowSubclassFromAnyThread`) is kept as the fallback route of the
  menu, and it is removed on unload.
* Nothing is written to the registry. The mod acts only inside the Windows 10 shell
  process, i.e. an `explorer.exe` that is not `%SystemRoot%\explorer.exe`.

## Requirements

This mod requires the mod [Win10 taskbar on Win11 24H2 or 25H2](https://windhawk.net/mods/win10-taskbar-on-win11-24h2):
the Windows 10 taskbar must be present. In the Windows 11 shell this mod loads and does
nothing.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- WinXMenuAnchor: start
  $name: Where the menu opens
  $description: start = glued to the Start button, like Windows 10; cursor = at the mouse
    pointer.
  $options:
  - start: Start button (Windows 10)
  - cursor: Mouse pointer
- WinXMenuOffsetX: 0
  $name: Horizontal offset (px)
  $description: Shifts the resolved anchor to the right. The Windows 10 2015 reference is 18.
- WinXMenuOffsetY: 0
  $name: Vertical offset (px)
  $description: Shifts the resolved anchor downwards. The Windows 10 2015 reference is 16.
- WinXMenuTheme: auto
  $name: Win+X menu colours
  $description: >-
    auto = the Windows 10 menu follows the Windows app theme (light or dark);
    light = the Windows 10 light menu (F9F9F9, black text); dark = the Windows 10
    dark menu (2B2B2B, white text).
  $options:
  - auto: Follow Windows
  - light: Windows 10 light
  - dark: Windows 10 dark
- LogMenuSelections: true
  $name: Log the selected entry
  $description: Writes the entry that was chosen to the mod log, so a problem with one
    target can be seen in the log.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <windows.h>
#include <commctrl.h>
#include <atomic>
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

// The classic Windows 10 taskbar Search cascade, localized for the same UI
// languages as the other reconstructed menus. These are the three values of
// HKCU\Software\Microsoft\Windows\CurrentVersion\Search\SearchboxTaskbarMode:

class ScopedMenu {
public:
    explicit ScopedMenu(HMENU menu = nullptr) : m_menu(menu) {}
    ~ScopedMenu() { if (m_menu) DestroyMenu(m_menu); }
    ScopedMenu(const ScopedMenu&) = delete;
    ScopedMenu& operator=(const ScopedMenu&) = delete;
    HMENU get() const { return m_menu; }
    HMENU release() { HMENU menu = m_menu; m_menu = nullptr; return menu; }
private:
    HMENU m_menu;
};

// ---------------------------------------------------------------------------
// Menu presentation: the Windows 10 style menu, copied from the original
//
// Windows 10 draws its Win+X menu with its own owner-draw code, not with the plain
// popup of TrackPopupMenuEx. This is the same code the mod this one comes from uses
// ("ImmersiveMenu", ~430 lines): it takes the ordinary popup menu built below, reads
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
    try {
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
    } catch (...) {
        return false;
    }
}

struct ItemData {
    wchar_t text[128];
    bool separator;
    bool submenu;
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
    COLORREF border = RGB(205, 205, 205);
    COLORREF separator = RGB(205, 205, 205);
    COLORREF hotFill = RGB(229, 229, 229);
    int itemHeight = 0;
    // Proportions measured on the Windows 10 reference screenshot (uploads/image-2.png,
    // 100% DPI) and used as they are, so the recreation has the proportions of the
    // original: 32 px rows, text 32 px inside the item rectangle, the submenu chevron
    // 6 px from its right edge, the separator line inset by 8 px, and a menu about
    // 258 px wide - the width of the reference menu, from which the shell subtracts
    // its own right-hand gutter when the items are measured.
    int padLeft = 0;
    int padRight = 0;
    int chevronInset = 0;
    int separatorInset = 0;
    int row = 0;                 // position inside the menu, for the log only
    std::vector<ItemData*> items;
};
static Session* g_session = nullptr;

// Which scheme the menu uses. "Auto" follows the Windows app theme (Settings >
// Personalization > Colors); "Light" / "Dark" force the Windows 10 light or dark
// menu. Both colour sets are always available: when the shell theme cannot be
// opened the menu is painted with the Windows 10 constants of the chosen scheme
// instead of becoming an unstyled popup.
static bool IsLightTheme() noexcept {
    try {
        WindhawkUtils::StringSetting scheme(Wh_GetStringSetting(L"WinXMenuTheme"));
        if (scheme.get()) {
            if (_wcsicmp(scheme.get(), L"light") == 0) return true;
            if (_wcsicmp(scheme.get(), L"dark") == 0) return false;
        }
    } catch (...) {
    }
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
    for (ItemData* d : s.items) delete d;
    s.items.clear();
    if (s.brush) { DeleteObject(s.brush); s.brush = nullptr; }
    if (s.hfont) { DeleteObject(s.hfont); s.hfont = nullptr; }
    if (s.theme && g_api.closeTheme) { g_api.closeTheme(s.theme); s.theme = nullptr; }
}

static void Prepare(Session& s, HMENU menu) noexcept {
    const int count = GetMenuItemCount(menu);
    for (int i = 0; i < count; ++i) {
        wchar_t buffer[128] = {};
        MENUITEMINFOW info = {};
        info.cbSize = sizeof(info);
        info.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_SUBMENU;
        info.dwTypeData = buffer;
        info.cch = _countof(buffer) - 1;
        if (!GetMenuItemInfoW(menu, i, TRUE, &info)) continue;
        ItemData* data = new (std::nothrow) ItemData{};
        if (!data) continue;
        data->separator = (info.fType & MFT_SEPARATOR) != 0;
        data->submenu = info.hSubMenu != nullptr;
        wcsncpy_s(data->text, buffer, _TRUNCATE);
        s.items.push_back(data);

        MENUITEMINFOW set = {};
        set.cbSize = sizeof(set);
        set.fMask = MIIM_FTYPE | MIIM_DATA;
        set.fType = info.fType | MFT_OWNERDRAW;        // l'inverso di ApplyClassicMenu
        set.dwItemData = reinterpret_cast<ULONG_PTR>(data);
        SetMenuItemInfoW(menu, i, TRUE, &set);
        if (info.hSubMenu) Prepare(s, info.hSubMenu);
    }
    MENUINFO mi = {};
    mi.cbSize = sizeof(mi);
    mi.fMask = MIM_BACKGROUND;
    mi.hbrBack = s.brush;                               // sfondo del tema, non nullo
    SetMenuInfo(menu, &mi);
}

static bool Begin(Session& s, HMENU menu, HWND owner) noexcept {
    try {
        if (!owner || !LoadApi()) return false;
        s.owner = owner;
        const UINT dpi = GetDpiForWindow(owner);
        s.dpi = dpi >= 96 && dpi <= 480 ? (int)dpi : 96;
        // Windows 10 constants for the selected scheme: they stay in place if the
        // theme cannot be opened or does not carry a colour, so both themes are
        // supported even on builds where the immersive menu theme is missing.
        s.light = IsLightTheme();
        s.fill = s.light ? RGB(249, 249, 249) : RGB(43, 43, 43);
        s.textNormal = s.light ? RGB(0, 0, 0) : RGB(255, 255, 255);
        s.textDisabled = s.light ? RGB(120, 120, 120) : RGB(160, 160, 160);
        s.border = s.light ? RGB(205, 205, 205) : RGB(128, 128, 128);
        s.separator = s.light ? RGB(205, 205, 205) : RGB(128, 128, 128);
        s.hotFill = s.light ? RGB(229, 229, 229) : RGB(65, 65, 65);
        s.theme = OpenMenuTheme(owner, s.dpi);
        if (!s.theme) {
            // No immersive theme on this build: paint the menu with the Windows 10
            // colours anyway instead of falling back to a plain popup.
            Wh_Log(L"[menu] immersive theme unavailable: the menu keeps the Windows 10 %s colours",
                   s.light ? L"light" : L"dark");
        }

        HDC dc = GetDC(owner);
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
        if (s.theme) {
            // 1.0.2: a theme colour is accepted only when the call succeeded. A failed
            // GetThemeColor can leave 0 (black) in the output, which used to turn the
            // menu background and its text black.
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
        // 1.0.2: text of the same colour as the background would be invisible: back to
        // the Windows 10 constants of the scheme.
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
        s.brush = CreateSolidBrush(s.fill);
        if (!s.hfont || !s.brush) return false;
        Prepare(s, menu);
        return true;
    } catch (...) {
        return false;
    }
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
        m->itemHeight = MulDiv(9, s->dpi, 96);
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
    const int minWidth = MulDiv(242, s->dpi, 96);
    // 1.0.2: upper bounds, so a wrong measurement can never produce a menu that
    // covers the screen.
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
    // rectangle out of the clip below (it is the documented way of stopping the
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
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, textColor);
    RECT tr = rc;
    tr.left += s->padLeft;
    tr.right -= s->padRight;
    DrawTextW(dc, d->text, -1, &tr, DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_HIDEPREFIX);
    if (d->submenu) {
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
    if (old) SelectObject(dc, old);
    if (d->submenu) {
        // 1.0.0 - "one arrow, not two". The shell paints its own submenu arrow after
        // WM_DRAWITEM has returned, on top of the chevron drawn just above: the
        // documented way to stop it is to take the item rectangle out of the clip
        // region of this DC before returning, so only our chevron survives.
        ExcludeClipRect(dc, rc.left, rc.top, rc.right, rc.bottom);
    }
}

static LRESULT CALLBACK OwnerProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                  UINT_PTR, DWORD_PTR) {
    try {
        Session* s = g_session;
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
        }
    } catch (...) {
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

// Like TrackPopupMenuEx with TPM_RETURNCMD, but with immersive items. It must be
// called from the thread that owns "owner"; when no theme can be opened it shows
// the standard popup.
static UINT Track(HMENU menu, HWND owner, int x, int y, UINT flags) noexcept {
    Session session;
    try {
        flags = (flags & ~TPM_NONOTIFY) | TPM_RETURNCMD;
        if (g_session || !Begin(session, menu, owner)) {
            End(session);
            return static_cast<UINT>(TrackPopupMenuEx(menu, flags, x, y, owner, nullptr));
        }
        g_session = &session;
        SetWindowSubclass(owner, OwnerProc, kSubclassId, 0);
        const UINT result = static_cast<UINT>(TrackPopupMenuEx(menu, flags, x, y, owner, nullptr));
        RemoveWindowSubclass(owner, OwnerProc, kSubclassId);
        g_session = nullptr;
        End(session);
        return result;
    } catch (...) {
        // The session must not survive an exception: the owner window would keep the
        // subclass and the next menu would be drawn with the flags of this one.
        if (g_session == &session) {
            RemoveWindowSubclass(owner, OwnerProc, kSubclassId);
            g_session = nullptr;
        }
        End(session);
        Wh_Log(L"[menu] exception while showing the menu: the standard popup is used");
        return static_cast<UINT>(TrackPopupMenuEx(menu, flags, x, y, owner, nullptr));
    }
}

}  // namespace ImmersiveMenu


// ---------------------------------------------------------------------------
// Module settings and the process this mod acts in
// ---------------------------------------------------------------------------
static bool g_winXMenuAnchorCursor = false;
static int g_winXMenuOffsetX = 0;
static int g_winXMenuOffsetY = 0;
static bool g_logMenuSelections = true;

// The Windows 10 shell: an explorer.exe that is not %SystemRoot%\explorer.exe. The
// Windows 11 shell is left completely alone.
static bool IsLegacyShellProcess() {
    wchar_t path[MAX_PATH] = {};
    if (!GetModuleFileNameW(nullptr, path, _countof(path))) return false;
    wchar_t systemRoot[MAX_PATH] = {};
    const DWORD rootLength = GetEnvironmentVariableW(L"SystemRoot", systemRoot, _countof(systemRoot));
    if (!rootLength || rootLength >= _countof(systemRoot)) return false;
    const size_t rootChars = wcslen(systemRoot);
    const size_t pathChars = wcslen(path);
    if (pathChars == rootChars + 13 && _wcsnicmp(path, systemRoot, rootChars) == 0 &&
        _wcsicmp(path + rootChars, L"\\explorer.exe") == 0) {
        return false;
    }
    return true;
}

static void LoadSettings() {
    try {
        auto value = WindhawkUtils::StringSetting::make(L"WinXMenuAnchor");
        g_winXMenuAnchorCursor = value.get() && _wcsicmp(value.get(), L"cursor") == 0;
    } catch (...) { Wh_Log(L"[winx] anchor settings exception: keeping the previous option"); }
    g_winXMenuOffsetX = (int)Wh_GetIntSetting(L"WinXMenuOffsetX");
    g_winXMenuOffsetY = (int)Wh_GetIntSetting(L"WinXMenuOffsetY");
    g_logMenuSelections = Wh_GetIntSetting(L"LogMenuSelections") != 0;
}

// ---------------------------------------------------------------------------
// State of the Win+X machinery
// ---------------------------------------------------------------------------
static const int kWinXHotkeyId = 0xA1B3;
static constexpr UINT kWinXRouteMessage = WM_APP + 0x3A1;
static constexpr UINT kWinXRefreshMessage = WM_APP + 0x3A2;   // "settings changed" wake-up
static int g_winXHookModuleAnchor = 0;
static std::atomic<bool> g_winXKeyboardFallbackEnabled{false};
static std::atomic<bool> g_winXMouseRouteEnabled{false};
static HWND g_winXStartButtonForInput = nullptr;
static RECT g_winXStartButtonRectForInput{};
static bool g_winXRightButtonDownOnStart = false;
static bool g_winXKeyboardChordHandled = false;
static bool g_winXKeyboardHookInstalled = false;
// g_winXMenuOpen is set for exactly as long as TrackPopupMenuEx keeps the menu on
// screen, so the input hooks can tell "the menu of this mod is already open" from
// "nothing is open" without guessing: while it is set, a second chord - or a
// right-click on Start - closes the menu instead of stacking another one.
static std::atomic<bool> g_winXMenuOpen{false};
static HWND g_winXMenuOwnerWindow = nullptr;   // owner of the open menu (WM_CANCELMODE target)
static ULONGLONG g_winXLastTaskbarUpTick = 0;  // last time the private shell answered
static bool g_winXSwallowXUp = false;
static bool g_winXXHeld = false;   // X is physically down: auto-repeat is ignored
static bool g_winXMouseHookInstalled = false;
static DWORD g_servicesThreadId = 0;

// ---------------------------------------------------------------------------
// Menu commands
//
// Every entry of the menu ends up here. Nothing runs on the menu thread: each
// target is handed to the shell, and every failure is logged and contained, so a
// wrong entry can never take the taskbar down. The monolith's crash guard is not
// part of this module: ShellExecuteW is documented to marshal the call, and the
// low-level hooks never run any of this.
// ---------------------------------------------------------------------------
static void InjectSystemChord(wchar_t letter) {
    INPUT input[4] = {};
    input[0].type = INPUT_KEYBOARD; input[0].ki.wVk = VK_LWIN;
    input[1].type = INPUT_KEYBOARD; input[1].ki.wVk = (WORD)towupper(letter);
    input[2].type = INPUT_KEYBOARD; input[2].ki.wVk = (WORD)towupper(letter);
    input[2].ki.dwFlags = KEYEVENTF_KEYUP;
    input[3].type = INPUT_KEYBOARD; input[3].ki.wVk = VK_LWIN;
    input[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(ARRAYSIZE(input), input, sizeof(INPUT));
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
    if (g_logMenuSelections) Wh_Log(L"[winx] %s: %s %s", what ? what : L"entry",
                                    file ? file : L"", params ? params : L"");
    if (!file || !*file) return;
    HINSTANCE result = ShellExecuteW(nullptr, verb && *verb ? verb : L"open", file, params,
                                     nullptr, SW_SHOWNORMAL);
    if (reinterpret_cast<INT_PTR>(result) <= 32)
        Wh_Log(L"[winx] opening %s failed (%Id)", file, (INT_PTR)result);
}

// ---------------------------------------------------------------------------
// Route message: the menu is created by a subclass of Shell_TrayWnd, i.e. on the
// taskbar thread, exactly like the shell's own menus. The subclass is registered
// through WindhawkUtils from the services thread and installed by the taskbar
// thread; it is removed on unload from any thread.
// ---------------------------------------------------------------------------
static HWND g_trayWnds[8] = {};
static int g_trayWndCount = 0;
static HWND g_winXRequestedStart = nullptr;

static LRESULT CALLBACK TrayRouteSubclassProc(HWND hwnd, UINT msg, WPARAM wParam,
                                              LPARAM lParam, DWORD_PTR ref);
static void InstallTrayRouteSubclass();

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
    // Unico vero criterio per il pulsante Start nativo di Windows:
    // control ID 0x130. Non accettiamo più la classe "Start"/"StartButton"
    // perché OpenShell usa esattamente quella classe per il suo pulsante.
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

static HWND FindNativeTaskbarStartButton() {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!taskbar) return nullptr;
    DWORD pid = 0;
    GetWindowThreadProcessId(taskbar, &pid);
    if (pid != GetCurrentProcessId()) return nullptr;

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

class ThreadHotKeyRegistration {
public:
    explicit ThreadHotKeyRegistration(int id) : id_(id) {}
    ~ThreadHotKeyRegistration() { Reset(); }
    ThreadHotKeyRegistration(const ThreadHotKeyRegistration&) = delete;
    ThreadHotKeyRegistration& operator=(const ThreadHotKeyRegistration&) = delete;

    bool Register(UINT modifiers, UINT virtualKey) {
        if (registered_) return true;
        if (!RegisterHotKey(nullptr, id_, modifiers, virtualKey)) return false;
        registered_ = true;
        return true;
    }
    void Reset() {
        if (registered_) {
            UnregisterHotKey(nullptr, id_);
            registered_ = false;
        }
    }
    bool IsRegistered() const { return registered_; }

private:
    int id_;
    bool registered_ = false;
};

class ScopedWindowsHook {
public:
    ScopedWindowsHook() = default;
    ~ScopedWindowsHook() { Reset(); }
    ScopedWindowsHook(const ScopedWindowsHook&) = delete;
    ScopedWindowsHook& operator=(const ScopedWindowsHook&) = delete;

    bool Install(int hookType, HOOKPROC callback, HMODULE module) {
        if (hook_) return true;
        hook_ = SetWindowsHookExW(hookType, callback, module, 0);
        return hook_ != nullptr;
    }
    void Reset() {
        if (hook_) {
            UnhookWindowsHookEx(hook_);
            hook_ = nullptr;
        }
    }
    bool IsInstalled() const { return hook_ != nullptr; }

private:
    HHOOK hook_ = nullptr;
};

// 1.0.0: the input hooks are file-scope objects of the services thread, so a
// failed installation can be retried by the service loop. The previous build
// created them as locals and gave up for the whole session on the first failure
// ("sign out and back in to enable it"), which is one of the ways Win+X could
// stay dead until the next sign-in.
static ScopedWindowsHook g_winXKeyboardHook;
static ScopedWindowsHook g_winXMouseHook;

static void ClearWinXInputRoutes() {
    g_winXKeyboardFallbackEnabled.store(false, std::memory_order_release);
    g_winXMouseRouteEnabled.store(false, std::memory_order_release);
    g_winXStartButtonForInput = nullptr;
    g_winXStartButtonRectForInput = {};
    g_winXRightButtonDownOnStart = false;
    g_winXKeyboardChordHandled = false;
}

// Forward declaration for custom WIN+X menu function
static void ShowCustomWinXMenu(HWND startButton);
static bool PostWinXContextRequestFromHook() {
    // ShowCustomWinXMenu is not called here: this runs inside a low-level hook and
    // TrackPopupMenu blocks the thread. Windows removes a hook that does not answer
    // within ~300 ms (LowLevelHooksTimeout). The work is handed to the services thread.
    if (g_servicesThreadId &&
        PostThreadMessageW(g_servicesThreadId, kWinXRouteMessage, 0, 0)) {
        return true;
    }
    return false;
}
static bool IsPopupMenuForeground();

// ---------------------------------------------------------------------------
// Sets g_winXMenuOpen for exactly as long as the menu is on screen, whatever
// happens inside TrackPopupMenuEx (an exception, an early return, a menu that
// steals the loop): a flag that goes stale would make CloseWinXMenuIfOpen()
// swallow the next Win+X, which is the one failure this release must not have.
// ---------------------------------------------------------------------------
class WinXMenuOpenScope {
public:
    explicit WinXMenuOpenScope(HWND owner) {
        g_winXMenuOwnerWindow = owner;
        g_winXMenuOpen.store(true, std::memory_order_release);
    }
    ~WinXMenuOpenScope() {
        g_winXMenuOpen.store(false, std::memory_order_release);
        g_winXMenuOwnerWindow = nullptr;
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
    const HWND owner = g_winXMenuOwnerWindow;
    if (!owner || !IsWindow(owner)) return false;   // stale: let the chord through
    if (!PostMessageW(owner, WM_CANCELMODE, 0, 0)) return false;
    Wh_Log(L"[winx] the menu is open: this request closes it (as the native menu does)");
    return true;
}

static bool IsWinXStartHitPoint(const POINT& point) {
    HWND startButton = g_winXStartButtonForInput;
    if (!startButton || !IsWindow(startButton) ||
        !PtInRect(&g_winXStartButtonRectForInput, point)) {
        return false;
    }
    HWND hitWindow = WindowFromPoint(point);
    return hitWindow == startButton ||
           (hitWindow && IsChild(startButton, hitWindow));
}

static LRESULT CALLBACK WinXLowLevelKeyboardProc(int nCode, WPARAM wParam,
                                                 LPARAM lParam) {
    // Win+X belongs to the mod: the chord is consumed and the menu is requested from the
    // services thread. Windows removes a low-level hook that does not answer within
    // LowLevelHooksTimeout (~300 ms), so nothing slow may happen in here.
    try {
        if (nCode == HC_ACTION && lParam &&
            g_winXKeyboardFallbackEnabled.load(std::memory_order_acquire)) {
            const auto* key = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
            const bool keyDown = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
            if (!keyDown && key->vkCode == 'X') g_winXXHeld = false;
            if (!keyDown && g_winXSwallowXUp && key->vkCode == 'X') {
                g_winXSwallowXUp = false;
                return 1;                       // anche il rilascio di X e' consumato
            }
            if (keyDown && !(key->flags & LLKHF_INJECTED) && key->vkCode == 'X') {
                const bool winDown = (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0 ||
                                     (GetAsyncKeyState(VK_RWIN) & 0x8000) != 0;
                const bool otherModifierDown =
                    (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0 ||
                    (GetAsyncKeyState(VK_MENU) & 0x8000) != 0 ||
                    (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
                if (winDown && !otherModifierDown) {
                    // 1.0.0: a second Win+X while the mod menu is on screen closes
                    // it (the native menu behaves the same way) and the request is
                    // not forwarded again. Otherwise the queued request would be
                    // handled right after the menu closed and the menu would pop up
                    // again, which is exactly what "must appear on every chord" must
                    // not turn into.
                    //
                    // 1.0.2: key auto-repeat is ignored. Holding Win+X a little too long
                    // used to repeat the chord, closing and reopening the menu over and
                    // over (and injecting a key each time).
                    const bool repeat = g_winXXHeld;
                    g_winXXHeld = true;
                    if (repeat) return 1;
                    if (!CloseWinXMenuIfOpen()) PostWinXContextRequestFromHook();

                    // The shell must not see the chord. A dummy key (VK 0xE8, unassigned,
                    // injected and therefore ignored by this hook) keeps the release of
                    // Win from opening the Start menu.
                    g_winXSwallowXUp = true;
                    INPUT dummy[2] = {};
                    // 1.0.2: VK_F24 is bound by some OEM hotkey utilities; 0xE8 is
                    // the unassigned virtual key used for exactly this purpose.
                    dummy[0].type = INPUT_KEYBOARD; dummy[0].ki.wVk = 0xE8;
                    dummy[1].type = INPUT_KEYBOARD; dummy[1].ki.wVk = 0xE8;
                    dummy[1].ki.dwFlags = KEYEVENTF_KEYUP;
                    SendInput(2, dummy, sizeof(INPUT));
                    return 1;
                }
            }
        }
    } catch (...) {
        // A low level hook must never let an exception escape: Windows would remove the
        // hook and the Win+X chord would stop working for the rest of the session.
        static int hookExceptions = 0;
        if (hookExceptions++ < 5)
            Wh_Log(L"[winx] exception in the keyboard hook: contained");
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static LRESULT CALLBACK WinXLowLevelMouseProc(int nCode, WPARAM wParam,
                                              LPARAM lParam) {
    // A right-click on the Start button is answered by the mod: press and release are
    // consumed together, so no unmatched button-up can ever be leaked to the shell.
    try {
        if (nCode == HC_ACTION && lParam &&
            g_winXMouseRouteEnabled.load(std::memory_order_acquire)) {
            const auto* mouse = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
            if (!(mouse->flags & LLMHF_INJECTED) && IsWinXStartHitPoint(mouse->pt)) {
                // Right click on Start: the mod answers it, not the shell (press and
                // release are consumed together).
                if (wParam == WM_RBUTTONDOWN && !IsPopupMenuForeground()) {
                    g_winXRightButtonDownOnStart = true;
                    return 1;
                }
                if (wParam == WM_RBUTTONUP && g_winXRightButtonDownOnStart) {
                    g_winXRightButtonDownOnStart = false;
                    if (!CloseWinXMenuIfOpen()) PostWinXContextRequestFromHook();
                    return 1;
                }
            }
        }
    } catch (...) {
        // Same policy as the keyboard hook: contained, logged in a few lines, never
        // propagated into the shell's message loop.
        static int hookExceptions = 0;
        if (hookExceptions++ < 5)
            Wh_Log(L"[winx] exception in the mouse hook: contained");
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static bool IsPopupMenuForeground() {
    HWND foreground = GetForegroundWindow();
    if (!foreground) return false;
    wchar_t cls[64] = {};
    return GetClassNameW(foreground, cls, _countof(cls)) &&
           _wcsicmp(cls, L"#32768") == 0;
}

// ---------------------------------------------------------------------------
// Input-hook installation, retried (1.0.0).
// ---------------------------------------------------------------------------
static bool EnsureWinXInputHooks() {
    if (!g_winXKeyboardHook.IsInstalled() || !g_winXMouseHook.IsInstalled()) {
        HMODULE module = nullptr;
        const DWORD flags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
        if (GetModuleHandleExW(flags, reinterpret_cast<LPCWSTR>(&g_winXHookModuleAnchor),
                               &module)) {
            if (!g_winXKeyboardHook.IsInstalled() &&
                g_winXKeyboardHook.Install(WH_KEYBOARD_LL, WinXLowLevelKeyboardProc, module))
                Wh_Log(L"[winx] Win+X keyboard hook installed");
            if (!g_winXMouseHook.IsInstalled() &&
                g_winXMouseHook.Install(WH_MOUSE_LL, WinXLowLevelMouseProc, module))
                Wh_Log(L"[winx] Start right-click hook installed");
        }
    }
    g_winXKeyboardHookInstalled = g_winXKeyboardHook.IsInstalled();
    g_winXMouseHookInstalled = g_winXMouseHook.IsInstalled();
    return g_winXKeyboardHookInstalled || g_winXMouseHookInstalled;
}

// ---------------------------------------------------------------------------
// Owner window of the custom Win+X menu.
//
// Two Win32 rules decide whether a popup menu works at all: the owner window
// must have been created by the thread that calls TrackPopupMenu, and that
// thread must hold the foreground while the menu is open (with a WM_NULL posted
// afterwards so the menu also closes when the user clicks elsewhere).
// Shell_TrayWnd belongs to the shell's taskbar thread, which is why the earlier
// build could not show its menu from the services thread.
// ---------------------------------------------------------------------------
static const wchar_t kWinXOwnerClassName[] = L"Win10ExplorerRestorer.WinXMenuOwner";
static HWND g_winXOwnerWindow = nullptr;
static DWORD g_winXOwnerWindowThread = 0;

static LRESULT CALLBACK WinXOwnerWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                         LPARAM lParam) {
    try {
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    } catch (...) {
        // Nothing in this window may throw into the shell's message loop.
        return 0;
    }
}

static bool ForceForegroundWindow(HWND hwnd) {
    if (!hwnd) return false;
    const HWND foreground = GetForegroundWindow();
    if (foreground == hwnd) return true;
    // 1.0.2: the plain call comes first: the chord has just been pressed, so it is
    // normally allowed and no input queue has to be touched.
    if (SetForegroundWindow(hwnd) && GetForegroundWindow() == hwnd) return true;
    DWORD foregroundPid = 0;
    const DWORD foregroundThread =
        foreground ? GetWindowThreadProcessId(foreground, &foregroundPid) : 0;
    const DWORD ourThread = GetCurrentThreadId();
    // 1.0.2: the input queue is never attached to a thread of explorer.exe itself
    // (taskbar or desktop): attaching and detaching there disturbs the focus state
    // of the shell and can freeze it.
    const bool attached = foregroundThread && foregroundThread != ourThread &&
                          foregroundPid != GetCurrentProcessId() &&
                          AttachThreadInput(ourThread, foregroundThread, TRUE);
    const bool ok = SetForegroundWindow(hwnd) != FALSE;
    if (attached) AttachThreadInput(ourThread, foregroundThread, FALSE);
    return ok;
}

static HWND GetWinXMenuOwnerWindow() {
    const DWORD thread = GetCurrentThreadId();
    if (g_winXOwnerWindow && g_winXOwnerWindowThread == thread &&
        IsWindow(g_winXOwnerWindow)) {
        return g_winXOwnerWindow;
    }
    g_winXOwnerWindow = nullptr;

    HINSTANCE instance = nullptr;
    const DWORD flags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
    if (!GetModuleHandleExW(flags, reinterpret_cast<LPCWSTR>(&WinXOwnerWndProc),
                            &instance)) {
        Wh_Log(L"[winx] owner window: module handle unavailable (%lu)", GetLastError());
        return nullptr;
    }

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    if (!GetClassInfoExW(instance, kWinXOwnerClassName, &wc)) {
        wc = {};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = WinXOwnerWndProc;
        wc.hInstance = instance;
        wc.lpszClassName = kWinXOwnerClassName;
        if (!RegisterClassExW(&wc)) {
            Wh_Log(L"[winx] owner window class not registered (%lu)", GetLastError());
            return nullptr;
        }
    }

    // 1x1 off-screen popup, never activated by the user: it only anchors the menu.
    HWND hwnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                                kWinXOwnerClassName, L"", WS_POPUP,
                                -32000, -32000, 1, 1, nullptr, nullptr,
                                instance, nullptr);
    if (!hwnd) {
        Wh_Log(L"[winx] owner window not created (%lu)", GetLastError());
        return nullptr;
    }
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);  // a hidden window cannot own a popup menu
    g_winXOwnerWindow = hwnd;
    g_winXOwnerWindowThread = thread;
    return hwnd;
}

static void DestroyWinXMenuOwnerWindow() {
    if (g_winXOwnerWindow && g_winXOwnerWindowThread == GetCurrentThreadId())
        DestroyWindow(g_winXOwnerWindow);
    g_winXOwnerWindow = nullptr;
    g_winXOwnerWindowThread = 0;
}

static void UpdateWinXInputRoutes(ThreadHotKeyRegistration& hotkey) {
    static ULONGLONG nextCheck = 0;
    static bool routeReported = false;

    const ULONGLONG now = GetTickCount64();
    if (now < nextCheck) return;
    nextCheck = now + 2500;

    // 1.0.0: a hook that failed to install is retried here instead of being
    // given up for the whole session.
    EnsureWinXInputHooks();

    // 1.0.0: once the private taskbar has been ours, the chord stays armed for
    // a few seconds even if the taskbar briefly stops answering during a shell
    // transition,
    // so Win+X does not go dead for a moment and then work again.
    if (IsLegacyShellProcess()) g_winXLastTaskbarUpTick = now;
    const bool taskbarGone = g_winXLastTaskbarUpTick == 0 ||
                             now - g_winXLastTaskbarUpTick > 10000;

    if (taskbarGone) {
        hotkey.Reset();
        ClearWinXInputRoutes();
        routeReported = false;
        return;
    }

    // The chord is deliberately NOT registered with RegisterHotKey: Win-key
    // combinations are reserved to the OS (the registration fails on most builds).
    // The low-level hooks handle the chord instead.
    hotkey.Reset();

    HWND startButton = FindNativeTaskbarStartButton();
    if (startButton && GetWindowRect(startButton, &g_winXStartButtonRectForInput)) {
        g_winXStartButtonForInput = startButton;
    } else {
        g_winXStartButtonForInput = nullptr;
    }

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
        // 1.0.0: the hooks are retried every 2.5 s, so this line is a status
        // note rather than a give-up message.
        routeReported = true;
        Wh_Log(L"[winx] no input hook is installed yet: it is retried every few seconds");
    }
}

// An entry opens a shell namespace when it is a shell URI: "shell:::{GUID}",
// "shell:AppsFolder\\...", "ms-settings:...". Those targets do not go through a direct
// ShellExecute: they go through the guard (time limit + fault containment).
static bool IsShellNamespaceCommand(const wchar_t* command) {
    if (!command) return false;
    return _wcsnicmp(command, L"shell:", 6) == 0 ||
           _wcsnicmp(command, L"ms-settings:", 12) == 0 ||
           _wcsnicmp(command, L"ms-availablenetworks:", 21) == 0;
}

// Splits "file parameters" (at the first space) and hands both to the guard.
static void RunWinXCommandSplit(const wchar_t* command, const wchar_t* verb,
                                const wchar_t* what) noexcept {
    try {
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
    } catch (...) {
        Wh_Log(L"[winx] exception while preparing a menu command: contained");
    }
}

// Runs the command of a Win+X menu entry (1.0.0).
//
// Accepted formats:
//   "@winkey:X"   injected Win+X chord (Run, Desktop) - local, opens nothing;
//   "@admin:exe"  program started with elevation (the "runas" verb);
//   "shell:..."   "shell:::{GUID}"   "ms-settings:..."   shell namespaces;
//   "file parameters"  any other program or applet.
//
// Safety rules, all of them needed for the shell to survive any entry:
//   * nothing is opened on this thread: the openings go through ShellOpGuard, which runs
//     them on a service thread with a time limit (so the shell cannot hang the menu or
//     the taskbar) and inside CrashGuard, which contains access violations (0xC0000005)
//     in the worker instead of taking explorer.exe down;
//   * the parameters are copied into the context of the worker, so a detached worker
//     never reads a buffer that no longer exists;
//   * this function lets no exception out: every error ends up in the log and the shell
//     stays up;
//   * the selected command is written to the log before it runs, so a problem with an
//     entry shows up at once.
static void RunWinXCommand(const wchar_t* command) {
    try {
        if (!command || !*command) return;
        Wh_Log(L"[winx] selected entry: %s", command);

        if (wcsncmp(command, L"@winkey:", 8) == 0) {
            if (command[8]) InjectSystemChord(command[8]);
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
        RunWinXCommandSplit(command, L"open", L"programma del menu");
    } catch (...) {
        Wh_Log(L"[winx] exception while running a menu entry: contained, the shell keeps running");
    }
}

// WIN+X menu with all standard entries
//
// 1.0.0 - this menu is a recreation of the Windows 10 power-user (Win+X) menu drawn
// from Windows 10 reference screenshots: the same entries in the same order, the same
// two separators (after "Command Prompt (Admin)" and before "Shut down or sign out"),
// the same wording, the Windows 10 light/dark colours, the same row height and the
// single chevron of the submenu. It is a recreation, not the original component, but
// it is meant to look and behave like it (see ImmersiveMenu for the measurements).
//
// Shows a complete WIN+X menu with all standard Windows 10 entries:
// Installed apps, Power Options, Event Viewer, System, Device Manager,
// Network Connections, Disk Management, Computer Management, Terminal,
// Terminal (Admin), Task Manager, Settings, File Explorer, Run,
// Shut down or sign out, Desktop. The Search entry of the original menu is not
// part of this recreation (see the entry table).
// Uses multilingual support for 10 languages (IT, EN, FR, ES, DE, PT, NL, RU, JA, PL).
// ---------------------------------------------------------------------------
// Win+X anchor - the Windows 10 position (1.0.0).
//
// Windows 10 opens the power-user menu glued to the Start button: its
// bottom-left corner sits on the top edge of the taskbar, at the left edge of
// the Start button, and it opens there whatever the pointer is doing. The
// previous build fell back to the raw cursor whenever the Start button could not
// be resolved, which dropped the menu on top of the taskbar (16 px overlap in
// the 2026-10-04 report) instead of on its top edge.
//
// Order of preference: Start button -> taskbar edge -> monitor corner. The
// pointer is used only when there is no taskbar at all. winXMenuOffsetX/Y shift
// the result if another look is wanted (the README documents the 2015 Windows 10
// reference screenshot, which is X=18, Y=+16: menu bottom edge 16 px below the
// top edge of the taskbar).
// ---------------------------------------------------------------------------
struct WinXMenuAnchor {
    POINT point{};
    UINT flags = TPM_LEFTALIGN | TPM_BOTTOMALIGN;
    const wchar_t* source = L"window corner";
    bool taskbarOnTop = false;
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

    RECT taskbarRect = {};
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (taskbar && IsWindow(taskbar)) {
        DWORD pid = 0;
        GetWindowThreadProcessId(taskbar, &pid);
        if (pid != GetCurrentProcessId() || !GetWindowRect(taskbar, &taskbarRect))
            taskbar = nullptr;
    } else {
        taskbar = nullptr;
    }

    RECT buttonRect = {};
    const bool haveButton = startButton && IsWindow(startButton) &&
                            GetWindowRect(startButton, &buttonRect);
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

    // The offsets are a deliberate user choice and are applied after the
    // default anchor has been resolved, never before.
    anchor.point.x = x + g_winXMenuOffsetX;
    anchor.point.y = y + g_winXMenuOffsetY;
    return anchor;
}

static void ShowCustomWinXMenuHere(HWND startButton) {
    try {
        const UiLangId lang = DetectUiLang();
        
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
            // Programs and Features (1.0.0): the wording of the Windows 10 menu of the
            // reference screenshot, not "Installed apps". The target is unchanged
            // (ms-settings:appsfeatures, the Apps & features page of Windows 11);
            // replace it with appwiz.cpl for the classic Control Panel applet instead.
            { { L"Programmi e funzionalit\u00e0", L"Programs and Features", L"Programmes et fonctionnalit\u00e9s", L"Programas y caracter\u00edsticas", L"Programme und Features",
                L"Programas e Recursos", L"Programma's en onderdelen", L"\u041f\u0440\u043e\u0433\u0440\u0430\u043c\u043c\u044b \u0438 \u043a\u043e\u043c\u043f\u043e\u043d\u0435\u043d\u0442\u044b", L"\u30d7\u30ed\u30b0\u30e9\u30e0\u3068\u6a5f\u80fd", L"Programy i funkcje", L"Programmer og funktioner", L"Program och funktioner", L"Programmer og funksjoner", L"Ohjelmat ja toiminnot", L"Programlar ve Özellikler"},
              L"ms-settings:appsfeatures" },
            // Power Options
            { { L"Opzioni di alimentazione", L"Power Options", L"Options d'alimentation", L"Opciones de energ\u00eda", L"Energieoptionen",
                L"Op\u00e7\u00f5es de energia", L"Energiebeheer", L"\u041f\u0430\u0440\u0430\u043c\u0435\u0442\u0440\u044b \u043f\u0438\u0442\u0430\u043d\u0438\u044f", L"\u30d1\u30ef\u30a2\u30aa\u30d7\u30b7\u30e7\u30f3", L"Opcje zasilania", L"Strømstyring", L"Energialternativ", L"Strømalternativer", L"Virranhallinta", L"Güç Seçenekleri"},
              L"control.exe powercfg.cpl" },
            // Event Viewer
            { { L"Visualizzatore eventi", L"Event Viewer", L"Observateur d'\u00e9v\u00e9nements", L"Visor de eventos", L"Ereignisanzeige",
                L"Visualizador de Eventos", L"Gebeurtenisweergave", L"\u041f\u0440\u043e\u0441\u043c\u043e\u0442\u0440 \u0441\u043e\u0431\u044b\u0442\u0438\u0439", L"\u30a4\u30d9\u30f3\u30c8\u30d3\u30e5\u30a4\u30e4\u30fc", L"Podgl\u0105d zdarze\u0144", L"Hændelsesfremviser", L"Loggboken", L"Hendelsesvisning", L"Tapahtumakatselu", L"Olay Görüntüleyicisi"},
              L"eventvwr.msc" },
            // System
            { { L"Sistema", L"System", L"Syst\u00e8me", L"Sistema", L"System",
                L"Sistema", L"Systeem", L"\u0421\u0438\u0441\u0442\u0435\u043c\u0430", L"\u30b7\u30b9\u30c6\u30e0", L"System", L"System", L"System", L"System", L"Järjestelmä", L"Sistem"},
              L"ms-settings:about" },
            // Device Manager
            { { L"Gestione dispositivi", L"Device Manager", L"Gestionnaire de p\u00e9riph\u00e9riques", L"Administrador de dispositivos", L"Ger\u00e4temanager",
                L"Gestor de Dispositivos", L"Apparaatbeheer", L"\u0414\u0438\u0441\u043f\u0435\u0442\u0447\u0435\u0440 \u0443\u0441\u0442\u0440\u043e\u0439\u0441\u0442\u0432", L"\u30c7\u30d0\u30a4\u30b9\u30de\u30fc\u30b8\u30e3", L"Mened\u017cer urz\u0105dze\u0144", L"Enhedshåndtering", L"Enhetshanteraren", L"Enhetsbehandling", L"Laitehallinta", L"Aygıt Yöneticisi"},
              L"devmgmt.msc" },
            // Network Connections
            { { L"Connessioni di rete", L"Network Connections", L"Connexions r\u00e9seau", L"Conexiones de red", L"Netzwerkverbindungen",
                L"Liga\u00e7\u00f5es de Rede", L"Netwerkverbindingen", L"\u0421\u0435\u0442\u0435\u0432\u044b\u0435 \u0441\u0432\u044f\u0437\u0438", L"\u30cd\u30c3\u30c8\u30ef\u30a2\u30af\u30bb\u30b7\u30e7\u30f3", L"Po\u0142\u0105\u0107czenia sieciowe", L"Netværksforbindelser", L"Nätverksanslutningar", L"Nettverkstilkoblinger", L"Verkkoyhteydet", L"Ağ Bağlantıları"},
              // 1.0.0: ncpa.cpl left the item doing nothing on the private shell; the
              // shell namespace below is the one requested for this entry and it is
              // opened through the guarded worker like every other shell operation.
              L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}" },
            // Disk Management
            { { L"Gestione disco", L"Disk Management", L"Gestion des disques", L"Administraci\u00f3n de discos", L"Datentr\u00e4gerverwaltung",
                L"Gest\u00e3o de Discos", L"Schijfbeheer", L"\u0423\u043f\u0440\u0430\u0432\u043b\u0435\u043d\u0438\u0435 \u0434\u0438\u0441\u043a\u0430\u043c\u0438", L"\u30c7\u30a4\u30b9\u30af\u30de\u30fc\u30b8\u30e3", L"Zarzi\u0105dzanie dyskami", L"Diskhåndtering", L"Diskhantering", L"Diskbehandling", L"Levynhallinta", L"Disk Yönetimi"},
              L"diskmgmt.msc" },
            // Computer Management
            { { L"Gestione computer", L"Computer Management", L"Gestion de l'ordinateur", L"Administraci\u00f3n del equipo", L"Computerverwaltung",
                L"Gest\u00e3o do Computador", L"Computerbeheer", L"\u0423\u043f\u0440\u0430\u0432\u043b\u0435\u043d\u0438\u0435 \u043a\u043e\u043c\u043f\u044c\u044e\u0442\u0435\u0440\u043e\u043c", L"\u30b3\u30f3\u30d4\u30e5\u30fc\u30bf\u30de\u30fc\u30b8\u30e3", L"Zarzi\u0105dzanie komputerem", L"Computerstyring", L"Datorhantering", L"Datamaskinbehandling", L"Tietokoneenhallinta", L"Bilgisayar Yönetimi"},
              L"compmgmt.msc" },
            // Terminal
            { { L"Terminale", L"Terminal", L"Terminal", L"Terminal", L"Terminal",
                L"Terminal", L"Terminal", L"\u0422\u0435\u0440\u043c\u0438\u043d\u0430\u043b", L"\u30bf\u30fc\u30df\u30ca\u30eb", L"Terminal", L"Terminal", L"Terminal", L"Terminal", L"Pääte", L"Terminal"},
              L"wt.exe" },
            // Terminal (Admin)
            { { L"Terminale (Amministratore)", L"Terminal (Admin)", L"Terminal (Administrateur)", L"Terminal (Administrador)", L"Terminal (Admin)",
                L"Terminal (Administrador)", L"Terminal (Administrator)", L"\u0422\u0435\u0440\u043c\u0438\u043d\u0430\u043b (\u0410\u0434\u043c\u0438\u043d\u0438\u0441\u0442\u0440\u0430\u0442\u043e\u0440)", L"\u30bf\u30fc\u30df\u30ca\u30eb (\u7ba1\u6743\u8005)", L"Terminal (Administrator)", L"Terminal (administrator)", L"Terminal (administratör)", L"Terminal (administrator)", L"Pääte (järjestelmänvalvoja)", L"Terminal (Yönetici)"},
              L"@admin:wt.exe" },
            // Task Manager
            { { L"Gestione attivit\u00e0", L"Task Manager", L"Gestionnaire des t\u00e2ches", L"Administrador de tareas", L"Task-Manager",
                L"Gestor de Tarefas", L"Taakbeheer", L"\u0414\u0438\u0441\u043f\u0435\u0442\u0447\u0435\u0440 \u0437\u0430\u0434\u0430\u0447", L"\u30bf\u30b9\u30af\u30de\u30fc\u30b8\u30e3", L"Mened\u017cer zada\u0144", L"Jobliste", L"Aktivitetshanteraren", L"Oppgavebehandling", L"Tehtävienhallinta", L"Görev Yöneticisi"},
              L"taskmgr.exe" },
            // Settings
            { { L"Impostazioni", L"Settings", L"Param\u00e8tres", L"Configuraci\u00f3n", L"Einstellungen",
                L"Defini\u00e7\u00f5es", L"Instellingen", L"\u041f\u0430\u0440\u0430\u043c\u0435\u0442\u0440\u044b", L"\u8a2d\u5b9a", L"Ustawienia", L"Indstillinger", L"Inställningar", L"Innstillinger", L"Asetukset", L"Ayarlar"},
              L"ms-settings:" },
            // File Explorer
            { { L"Esplora file", L"File Explorer", L"Explorateur de fichiers", L"Explorador de archivos", L"Datei-Explorer",
                L"Explorador de Ficheiros", L"Verkenner", L"\u041f\u0440\u043e\u0432\u043e\u0434\u043d\u0438\u043a \u0444\u0430\u0439\u043b\u043e\u0432", L"\u30d5\u30a1\u30a4\u30eb\u30a8\u30af\u30b9\u30d7\u30ed\u30fc\u30e9", L"Eksplorator plik\u00f3w", L"Stifinder", L"Utforskaren", L"Filutforsker", L"Resurssienhallinta", L"Dosya Gezgini"},
              L"explorer.exe" },
            // The Windows 10 menu has a Search entry here. It is deliberately not part
            // of this recreation: opening the Windows 11 search host from a shell menu
            // was the least reliable entry of the whole menu, so the entry was removed
            // and Windows Search is opened with Win+S, the taskbar Search box or Start.
            // Run
            { { L"Esegui", L"Run", L"Ex\u00e9cuter", L"Ejecutar", L"Ausf\u00fchren",
                L"Executar", L"Uitvoeren", L"\u0412\u044b\u043f\u043e\u043b\u043d\u0438\u0442\u044c", L"\u5b9f\u884c", L"Uruchom", L"Kør", L"Kör", L"Kjør", L"Suorita", L"Çalıştır"},
              L"@winkey:R" },
            // Shut down or sign out
            { { L"Disconnetti o esci", L"Shut down or sign out", L"Arr\u00eater ou d\u00e9connecter", L"Cerrar sesi\u00f3n o apagar", L"Herunterfahren oder abmelden",
                L"Terminar Sess\u00e3o ou Desligar", L"Afsluiten of afmelden", L"\u0417\u0430\u043a\u0440\u044b\u0442\u044c \u0438\u043b\u0438 \u0432\u044b\u0439\u0442\u0438", L"\u30b7\u30e3\u30c3\u30c8\u30c0\u30a6\u30f3\u307e\u30ed\u30b0\u30a2\u30a6\u30c8", L"Zamknij lub wyloguj", L"Luk computeren, eller log af", L"Stäng av eller logga ut", L"Slå av eller logg av", L"Sammuta tai kirjaudu ulos", L"Kapat veya oturumu kapat"},
              L"explorer.exe shell:::{85BBD9B8-4226-101A-A96C-945596089032}" },
            // Desktop
            { { L"Desktop", L"Desktop", L"Bureau", L"Escritorio", L"Desktop",
                L"\u00c1rea de Trabalho", L"Bureaublad", L"\u0420\u0430\u0431\u043e\u0447\u0438\u0439 \u0441\u0442\u043e\u043b", L"\u30c7\u30b7\u30af\u30c8\u30c3\u30d7", L"Pulpit", L"Skrivebord", L"Skrivbord", L"Skrivebord", L"Työpöytä", L"Masaüstü"},
              L"@winkey:D" }
        };
        
        // Shut-down entries of the submenu (as in Windows 10), in every interface
        // language: the order is the one of UiLangId.
        static const wchar_t* const kPowerText[4][LANG_COUNT] = {
            { L"Disconnetti", L"Sign out", L"Se déconnecter", L"Cerrar sesión", L"Abmelden", L"Terminar sessão", L"Afmelden", L"Выйти", L"サインアウト", L"Wyloguj", L"Log af", L"Logga ut", L"Logg av", L"Kirjaudu ulos", L"Oturumu kapat" },
            { L"Sospendi", L"Sleep", L"Mettre en veille", L"Suspender", L"Energiesparmodus", L"Suspender", L"Sluimerstand", L"Спящий режим", L"スリープ", L"Uśpij", L"Slumre", L"Viloläge", L"Dvalemodus", L"Lepotila", L"Uyku" },
            { L"Arresta il sistema", L"Shut down", L"Arrêter", L"Apagar", L"Herunterfahren", L"Encerrar", L"Afsluiten", L"Завершение работы", L"シャットダウン", L"Zamknij", L"Luk computeren", L"Stäng av", L"Slå av", L"Sammuta", L"Kapat" },
            { L"Riavvia il sistema", L"Restart", L"Redémarrer", L"Reiniciar", L"Neu starten", L"Reiniciar", L"Opnieuw opstarten", L"Перезагрузка", L"再起動", L"Uruchom ponownie", L"Genstart", L"Starta om", L"Start på nytt", L"Käynnistä uudelleen", L"Yeniden başlat" },
        };
        static const wchar_t* const kPowerCmd[4] = {
            L"shutdown.exe /l",
            L"rundll32.exe powrprof.dll,SetSuspendState 0,1,0",
            L"shutdown.exe /s /t 0",
            L"shutdown.exe /r /t 0",
        };
        // The language index is used by the shut-down submenu as well.

        // Same order as the Windows 10 Win+X menu, with its two separators: after the
        // administrative terminal and before "Shut down or sign out".
        for (size_t i = 0; i < _countof(items); i++) {
            if (i == 10 || i == 14) AppendMenuW(menu.get(), MF_SEPARATOR, 0, nullptr);
            if (i == 14) {
                HMENU sub = CreatePopupMenu();
                if (sub) {
                    for (UINT k = 0; k < 4; k++)
                        AppendMenuW(sub, MF_STRING, 101 + k, kPowerText[k][lang]);
                    AppendMenuW(menu.get(), MF_POPUP | MF_STRING, (UINT_PTR)sub, items[i].text[lang]);
                    continue;
                }
            }
            AppendMenuW(menu.get(), MF_STRING, (UINT)(i + 1), items[i].text[lang]);
        }

        // Anchor (1.0.0): the Windows 10 position, resolved from the taskbar and
        // the Start button at show time. The raw cursor is no longer used while a
        // taskbar exists: see ComputeWinXMenuAnchor.
        const WinXMenuAnchor anchor = ComputeWinXMenuAnchor(startButton);
        const POINT pt = anchor.point;
        const UINT alignFlags = anchor.flags;
        Wh_Log(L"[winx] anchor: %s -> (%ld,%ld)%s", anchor.source, pt.x, pt.y,
               anchor.taskbarOnTop ? L" [taskbar on top]" : L"");

        // Owner: Shell_TrayWnd when this runs on the taskbar thread (Windows 10 look,
        // like the audio and network menus); otherwise a window of this thread.
        HWND owner = nullptr;
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (tray && IsWindow(tray) && GetWindowThreadProcessId(tray, nullptr) == GetCurrentThreadId()) {
            owner = tray;
            SetForegroundWindow(owner);
        } else {
            owner = GetWinXMenuOwnerWindow();
            if (owner) ForceForegroundWindow(owner);
        }

        // 1.0.0: the flag is set for exactly as long as the menu is on screen, so a
        // second chord can tell "already open" from "not open" (see WinXMenuOpenScope).
        Wh_Log(L"[winx] menu on thread %lu, owner %s 0x%p", GetCurrentThreadId(),
               (tray && GetWindowThreadProcessId(tray, nullptr) == GetCurrentThreadId())
                   ? L"Shell_TrayWnd"
                   : L"a window of this thread",
               (void*)owner);

        const WinXMenuOpenScope menuOpen(owner);
        const UINT selected = ImmersiveMenu::Track(menu.get(), owner, pt.x, pt.y,
                                                   TPM_RIGHTBUTTON | alignFlags);
        if (owner) PostMessageW(owner, WM_NULL, 0, 0);

        if (selected >= 101 && selected <= 104)
            RunWinXCommand(kPowerCmd[selected - 101]);
        else if (selected > 0 && selected <= _countof(items))
            RunWinXCommand(items[selected - 1].command);
    } catch (...) {
        Wh_Log(L"[winx] exception in custom WIN+X menu");
    }
}
// The thread that shows the menu.
//
// TrackPopupMenuEx runs a modal loop on the thread that calls it. The first release
// showed the menu on the taskbar thread (a subclass of Shell_TrayWnd, "Windows 10
// look"): that froze the whole private shell for as long as the menu was on screen -
// the desktop went black for seconds and the tray flyouts stopped answering
// afterwards. The menu now runs on a thread created for it, which makes its own owner
// window (GetWinXMenuOwnerWindow is per-thread) and exits when the menu closes. The
// shell thread is never blocked, not even for a moment.
static DWORD WINAPI WinXMenuThread(LPVOID parameter) {
    const HWND startButton = static_cast<HWND>(parameter);
    // The owner window needs a message queue on this thread.
    MSG queueInit = {};
    PeekMessageW(&queueInit, nullptr, 0, 0, PM_NOREMOVE);
    try {
        Wh_Log(L"[winx] menu thread %lu: showing the menu (the shell thread is not used)",
               GetCurrentThreadId());
        ShowCustomWinXMenuHere(startButton);
    } catch (...) {
        Wh_Log(L"[winx] exception in the menu thread");
    }
    return 0;
}

// Entry point: the request goes to a thread of this module. The caller is the thread
// that owns the low-level input hooks, which must never block (Windows removes a hook
// that does not answer in time), so the entry only starts the thread.
static void ShowCustomWinXMenu(HWND startButton) {
    try {
        HANDLE thread = CreateThread(nullptr, 0, WinXMenuThread,
                                     reinterpret_cast<LPVOID>(startButton), 0, nullptr);
        if (thread) {
            CloseHandle(thread);
            return;
        }
        Wh_Log(L"[winx] the menu thread could not be created (%lu): the menu opens on this "
               L"thread", GetLastError());
    } catch (...) {
    }
    ShowCustomWinXMenuHere(startButton);
}
static void HandleWinXHotKey(ThreadHotKeyRegistration& hotkey) {
    (void)hotkey;
    static ULONGLONG lastPost = 0;

    const ULONGLONG now = GetTickCount64();
    if (lastPost && now - lastPost < 400) return;   // anti key-repeat
    lastPost = now;

    ShowCustomWinXMenu(FindNativeTaskbarStartButton());
}

class ScopedWinXServiceStateReset {
public:
    ScopedWinXServiceStateReset() = default;
    ~ScopedWinXServiceStateReset() {
        ClearWinXInputRoutes();
        DestroyWinXMenuOwnerWindow();
        g_winXKeyboardHook.Reset();
        g_winXMouseHook.Reset();
        g_winXMenuOpen.store(false, std::memory_order_release);
        g_winXMenuOwnerWindow = nullptr;
        g_winXKeyboardHookInstalled = false;
        g_winXMouseHookInstalled = false;
        g_servicesThreadId = 0;
    }
    ScopedWinXServiceStateReset(const ScopedWinXServiceStateReset&) = delete;
    ScopedWinXServiceStateReset& operator=(const ScopedWinXServiceStateReset&) = delete;
};

// ---------------------------------------------------------------------------
// Thread: owns the input hooks and the menu owner window, and pumps messages.
// Nothing slow is ever executed here, so the hooks are neither removed by
// Windows for exceeding LowLevelHooksTimeout nor able to hold up system input.
// ---------------------------------------------------------------------------
static HANDLE g_stopEvent = nullptr;
static HANDLE g_servicesThread = nullptr;

static DWORD WINAPI ExplorerServicesThread(LPVOID) {
    try {
        ScopedWinXServiceStateReset stateReset;
        g_servicesThreadId = GetCurrentThreadId();
        MSG queueInit{};
        PeekMessageW(&queueInit, nullptr, 0, 0, PM_NOREMOVE);

        ThreadHotKeyRegistration winXHotkey(kWinXHotkeyId);
        InstallTrayRouteSubclass();
        EnsureWinXInputHooks();
        Wh_Log(L"[winx] input hooks: keyboard %s, Start right-click %s",
               g_winXKeyboardHook.IsInstalled() ? L"ready" : L"unavailable",
               g_winXMouseHook.IsInstalled() ? L"ready" : L"unavailable");

        ULONGLONG nextServiceTick = 0;
        for (;;) {
            DWORD waitMs = 400;
            const ULONGLONG beforeWait = GetTickCount64();
            if (nextServiceTick > beforeWait) {
                const ULONGLONG untilTick = nextServiceTick - beforeWait;
                if (untilTick < waitMs) waitMs = static_cast<DWORD>(untilTick);
            }
            const DWORD waitResult = MsgWaitForMultipleObjectsEx(
                g_stopEvent ? 1 : 0, g_stopEvent ? &g_stopEvent : nullptr,
                waitMs, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
            if (g_stopEvent && waitResult == WAIT_OBJECT_0) break;

            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == kWinXRouteMessage)
                    HandleWinXHotKey(winXHotkey);
                else if (msg.message == kWinXRefreshMessage) {
                    InstallTrayRouteSubclass();
                    UpdateWinXInputRoutes(winXHotkey);
                }
            }

            const ULONGLONG now64 = GetTickCount64();
            if (nextServiceTick && now64 < nextServiceTick) continue;
            nextServiceTick = now64 + 400;
            // The taskbar can appear after this thread starts (and be recreated
            // later): the subclass is installed again at every tick, and it is
            // idempotent, so the route is in place as soon as the taskbar is.
            InstallTrayRouteSubclass();
            UpdateWinXInputRoutes(winXHotkey);
        }
        Wh_Log(L"[winx] services finished");
    } catch (...) {
        Wh_Log(L"[winx] exception in the services thread");
    }
    return 0;
}

// The subclass lives on the taskbar thread: it only forwards the request to the
// thread-safe menu code below.
static LRESULT CALLBACK TrayRouteSubclassProc(HWND hwnd, UINT msg, WPARAM wParam,
                                              LPARAM lParam, DWORD_PTR ref) {
    (void)ref;
    if (msg == kWinXRouteMessage) {
        try { ShowCustomWinXMenuHere(g_winXRequestedStart); } catch (...) {}
        return 0;
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

static void InstallTrayRouteSubclass() {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tray || !IsWindow(tray)) return;
    for (int i = 0; i < g_trayWndCount; ++i) {
        if (g_trayWnds[i] == tray) return;
        if (g_trayWnds[i] && !IsWindow(g_trayWnds[i])) g_trayWnds[i] = nullptr;
    }
    // WindhawkUtils::SetWindowSubclassFromAnyThread is used on purpose: this thread
    // is not the taskbar thread, and the subclass is registered through a Windhawk
    // message so the taskbar itself installs it in its own thread.
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(tray, TrayRouteSubclassProc, 0)) {
        Wh_Log(L"[winx] taskbar subclass not installed (%lu): the menu opens on the mod thread",
               GetLastError());
        return;
    }
    if (g_trayWndCount < (int)_countof(g_trayWnds)) g_trayWnds[g_trayWndCount++] = tray;
    Wh_Log(L"[winx] taskbar subclass installed: it is the fallback route of the menu "
           L"(the menu is shown by a thread of the mod)");
}

static void RemoveTrayRouteSubclass() {
    for (int i = 0; i < g_trayWndCount; ++i) {
        HWND window = g_trayWnds[i];
        g_trayWnds[i] = nullptr;
        if (window && IsWindow(window))
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(window, TrayRouteSubclassProc);
    }
    g_trayWndCount = 0;
}

BOOL Wh_ModInit() {
    try {
        LoadSettings();
        Wh_Log(L"[winx] init");
        if (!IsLegacyShellProcess()) {
            Wh_Log(L"[winx] this process is not the Windows 10 shell: nothing to do");
            return TRUE;
        }
        // The thread owns the hooks and the taskbar subclass.
        g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        g_servicesThread = CreateThread(nullptr, 0, ExplorerServicesThread, nullptr, 0, nullptr);
        if (!g_servicesThread) Wh_Log(L"[winx] services thread not created (%lu)", GetLastError());
    } catch (...) {
        Wh_Log(L"[winx] init exception");
    }
    return TRUE;
}

void Wh_ModBeforeUninit() {
    // Wave the thread off, then take the hooks down before the DLL is unloaded.
    if (g_stopEvent) SetEvent(g_stopEvent);
    g_winXKeyboardHook.Reset();
    g_winXMouseHook.Reset();
}

void Wh_ModUninit() {
    if (g_servicesThread) {
        // No timeout: the thread is waved off through its event, and it never waits
        // on anything this thread holds.
        WaitForSingleObject(g_servicesThread, INFINITE);
        CloseHandle(g_servicesThread);
        g_servicesThread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    RemoveTrayRouteSubclass();
    DestroyWinXMenuOwnerWindow();
    Wh_Log(L"[winx] unloaded: no subclass, no hook and no window of this mod is left");
    g_servicesThreadId = 0;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    // Windhawk calls this from its own thread: the hooks and the taskbar subclass are
    // owned by the services thread, so this only asks that thread to refresh them
    // (installing a low-level hook from a thread without a message loop would install
    // a hook that never answers).
    if (g_servicesThreadId)
        PostThreadMessageW(g_servicesThreadId, kWinXRefreshMessage, 0, 0);
    else
        Wh_Log(L"[winx] settings reloaded before the services thread started");
}

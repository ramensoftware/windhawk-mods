// ==WindhawkMod==
// @id            adobe-dark-menus
// @name          Adobe Dark Menus
// @description   Enables dark mode and custom colors for the menus, dropdown lists and title bars in Adobe apps.
// @version       1.1.0
// @author        Saber Naeemi
// @github        https://github.com/sabergraphics
// @twitter       https://x.com/SaberNaeemi
// @homepage      https://www.sabernaeemi.com
// @include       Photoshop.exe
// @include       Illustrator.exe
// @include       InDesign.exe
// @include       InDesign (Beta).exe
// @include       InCopy.exe
// @include       InCopy (Beta).exe
// @include       Dreamweaver.exe
// @include       AfterFX.exe
// @include       AfterFX (Beta).exe
// @include       Adobe Premiere Pro.exe
// @include       Adobe Premiere Pro (Beta).exe
// @include       Adobe Audition.exe
// @include       Adobe Audition (Beta).exe
// @include       Adobe Media Encoder.exe
// @include       Adobe Media Encoder (Beta).exe
// @include       Adobe Bridge.exe
// @include       Lightroom.exe
// @include       Acrobat.exe
// @compilerOptions -lgdi32 -lcomctl32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Adobe Dark Menus

This Windhawk mod gives Adobe apps on Windows dark menus: real menu bars, top-bar
dropdowns, context menus and dropdown lists. It also colors their standard title
bars to match.

Everything happens inside each app's own process. No system colors are changed
and no other application is affected.

## Supported apps

Photoshop, Illustrator, InDesign, InCopy, Dreamweaver, After Effects, Premiere
Pro, Audition, Media Encoder, Bridge, Lightroom, Lightroom Classic and Acrobat.

The Beta versions of Photoshop, Illustrator, InDesign, InCopy, After Effects,
Premiere Pro, Audition and Media Encoder are covered as well.

## Screenshots

**Photoshop:** a dropdown list in the options bar

![Photoshop dropdown list](https://raw.githubusercontent.com/sabergraphics/adobe-dark-menus/main/images/photoshop-dropdown-list.png)

**Illustrator:** a context menu

![Illustrator context menu](https://raw.githubusercontent.com/sabergraphics/adobe-dark-menus/main/images/illustrator-context-menu.png)

**InDesign:** the Type menu

![InDesign Type menu](https://raw.githubusercontent.com/sabergraphics/adobe-dark-menus/main/images/indesign-type-menu.png)

**After Effects:** the Window menu, with the menu bar and title bar

![After Effects Window menu](https://raw.githubusercontent.com/sabergraphics/adobe-dark-menus/main/images/after-effects-window-menu.png)

**After Effects:** the blend modes on a timeline layer

![After Effects blend modes](https://raw.githubusercontent.com/sabergraphics/adobe-dark-menus/main/images/after-effects-blend-modes.png)

**Premiere Pro:** a context menu on a timeline clip

![Premiere Pro context menu](https://raw.githubusercontent.com/sabergraphics/adobe-dark-menus/main/images/premiere-context-menu.png)

**Premiere Pro:** the Sequence menu, with the menu bar and title bar

![Premiere Pro Sequence menu](https://raw.githubusercontent.com/sabergraphics/adobe-dark-menus/main/images/premiere-sequence-menu.png)

## Related mods

These overlap with this mod. Running them together in the same app has not been
tested, so use one or the other for the apps above.

- **[Photoshop Dark Menus](https://windhawk.net/mods/photoshop-dark-menus)** covers
  Photoshop only. This mod does everything it did and reaches the other Adobe apps,
  so disable it if you use this one.
- **[Dark mode context menus](https://windhawk.net/mods/dark-menus)** darkens the
  menus Windows draws, in every app and in fixed colors. It does not reach menus an
  app draws itself, which is why Photoshop stays light with it, or dropdown lists.
  This mod uses the same technique for the menus Windows draws, so exclude the Adobe
  apps from it in its Advanced tab.
- **[After Effects Theme](https://windhawk.net/mods/after-effects-theme)** and
  **[Premiere Pro Theme](https://windhawk.net/mods/premiere-pro-theme)** recolor the
  whole interface of those two apps in palettes of their own, title bar, menu bar
  and menus included. They answer the same menu bar messages as this mod, so if you
  use one of them, exclude that app from this mod in its Advanced tab.
- **[Windows 11 Custom Title Bar Colours](https://windhawk.net/mods/win11-custom-title-bar-colours)**
  and **[Auto Custom Titlebar Colors](https://windhawk.net/mods/auto-custom-titlebar-colors)**
  color title bars in every app. If you prefer one of them, turn off **Dark Title Bars** here.

## Options

- **Menu Background Color**: Background color (Default: `#2D2D2D`).
- **Menu Text Color**: Text color for active items (Default: `#DCDCDC`).
- **Highlight Background Color**: Color when hovering over an item (Default: `#505050`).
- **Highlight Text Color**: Text color when hovering over an item (Default: `#FFFFFF`).
- **Separator Line Color**: Color for separator lines. Set to match the background color to hide them completely (Default: `#383838`).
- **Disabled Text Color**: Text color for disabled menu items (Default: `#808080`).
- **Border Color**: Color of the frame around menus an app draws itself and around dropdown lists. Leave empty to keep the system frame on those menus and to use the background color on lists (Default: `#2D2D2D`).
- **Theme Dropdown Lists**: Darken the dropdown lists an app draws itself, using the colors above (Default: on).
- **Darken Menus Windows Draws**: Use Windows' dark menu theme, and paint real menu bars dark (Default: on).
- **Scroll Long Menus Instead of Columns**: Open a menu that an app splits into columns as one dark column that scrolls, since Windows cannot darken a menu with columns. After Effects splits its Effect menu this way once it holds more than the screen fits. Turn it off to keep the columns, which stay light (Default: on).
- **Dark Title Bars**: Color standard title bars to match the menus (Default: on).
- **Title Bar Color**: Leave empty to match Menu Background Color (Default: empty).
- **Title Bar Text Color**: Leave empty to let Windows choose, which dims the text on a window that is not active (Default: empty).

Colors can be given as `#RRGGBB` or the short `#RGB` form.

The colors apply to real menu bars, to menus an app draws itself (Photoshop's) and
to dropdown lists. Menus Windows draws itself keep Windows' own dark colors.

## How it works

Adobe's apps do not all draw their menus the same way, and sharing a library does
not settle it: Illustrator and InDesign ship the same `AdobeOwl.dll` as Photoshop,
yet Windows draws their menus and Photoshop draws its own. So the mod decides
menu by menu, by whether the items are owner-drawn (`MFT_OWNERDRAW`).

### Menus Windows draws

This is most menus in most Adobe apps. Windows paints them through its theme
engine, and uxtheme has a dark variant of that drawing which is off unless a
process asks for it. The mod asks for it with `SetPreferredAppMode(ForceDark)`
and `FlushMenuThemes`, and puts the process back when it is disabled. This
technique comes from
[Dark mode context menus](https://windhawk.net/mods/dark-menus) by MGGSK, and
needs Windows 10 1903 or later.

### Real menu bars

Some apps, After Effects for one, have a real Win32 menu bar. The dark theme does
not reach it, but Windows offers it to the window first through the undocumented
`WM_UAHDRAWMENU` and `WM_UAHDRAWMENUITEM` messages, so the mod paints it from a
`DefWindowProc` hook, in the colors above. Those messages were worked out in
[win32-custom-menubar-aero-theme](https://github.com/adzm/win32-custom-menubar-aero-theme).

### Menus an app draws itself

Photoshop owner-draws its menu items with plain GDI, so the theme has nothing to
attach to. Measured in Photoshop, the popups go up via `TrackPopupMenuEx` and each
item is painted with:

- `FillRect(hdc, &rc, (HBRUSH)(COLOR_MENU + 1))` for the item background, and
  `(HBRUSH)(COLOR_HIGHLIGHT + 1)` for the hovered item. These **system color
  pseudo-handles** are resolved inside `user32` from shared memory, so they
  bypass `GetSysColor` and `GetSysColorBrush` hooks entirely.
- `GetSysColor(COLOR_MENUTEXT / COLOR_GRAYTEXT / COLOR_HIGHLIGHTTEXT)` for text.
- Its own 1px `FillRect` for separator lines.

So every pixel of a menu item is reachable from inside the process:

- `FillRect` / `PatBlt` are hooked and, while a menu item is being painted,
  sys-color pseudo-handles are re-resolved through this mod's own color table,
  and 1px fills inside a menu DC are recolored to the separator color.
- `GetSysColor` / `GetSysColorBrush` are hooked for the menu color indices, but
  **only while a menu item is actually being painted**, that is, inside a
  `WM_DRAWITEM` of type `ODT_MENU`, so the app's dialogs and panels keep the
  system colors. "While a menu is open" would be far too broad: a menu runs a
  modal message loop that keeps dispatching to every other window on the thread,
  and an unrelated repaint would pick the menu colors up.
- `MENUINFO::hbrBack` is stamped on each owner-drawn popup at `WM_INITMENUPOPUP`,
  covering the popup background that the system paints around the items. It is
  never stamped on a menu Windows draws, where it would put a custom background
  behind a theme's gutter, separators and text.
- The popup's non-client frame is drawn kernel-side, where no user-mode hook can
  reach it. So the popup is subclassed at creation and the frame is repainted
  with the border color after it.

### Dropdown lists

The dropdown lists are stock Win32 combo boxes whose list is the system's own
`ComboLBox` window. Photoshop owner-draws the rows but takes their colors from the
system, and not consistently: one list reads `COLOR_WINDOWTEXT` for its row text
where another reads `COLOR_MENUTEXT`, and one fills its rows from a color it asks
for where another fills them with a `(HBRUSH)(COLOR_X + 1)` pseudo-handle. Both
routes are therefore covered, over a table that maps every background index to
the background color and every text index to the text color. The list window is
subclassed so that all of it applies only while that list or one of its rows is
painting, which leaves every other control reading those same colors alone,
including anything the app repaints in response to a selection.

A list whose rows Windows draws is left alone, the same rule as for menus. Windows
fills it from a brush no hook can reach, so recoloring the text alone would leave
light text on white. So is a list that stays open inside a dialog, such as the ones
in the font picker, since it is part of the dialog rather than a dropdown.

### Scroll arrows

A menu too tall for the screen gets scroll arrows, which the Windows kernel draws
directly in light colors, below anything the dark theme or a hook can reach. So the
mod paints over them straight afterward, turning the light square dark and the dark
triangle light. A disabled arrow stays dimmer, since this inverts what is there
rather than drawing a new one. On a menu an app draws itself, Windows draws a bare
triangle instead of the square, and the mod recolors it to Menu Text Color or
Disabled Text Color.

### Menus split into columns

An app can split a long menu into columns. After Effects does it to a menu taller
than the screen, which is what its Effect menu becomes with enough plugins
installed. Windows turns its theme off for any menu that has a column break and
draws it the old way, in light colors. So while such a menu is open, the mod takes
its column breaks off, and Windows lays it out as one dark column that scrolls. The
breaks go back on when the menu closes, so the app finds its menu as it left it.

If the app writes a break back while the menu is open, the mod holds that write
until the menu closes. Windows would otherwise lay the open menu out in columns
again, light and scrolled back to the top.

### Title bars

Windows 11 lets a process color a standard title bar through
`DwmSetWindowAttribute`, so nothing is drawn here: the mod sets
`DWMWA_USE_IMMERSIVE_DARK_MODE` and `DWMWA_CAPTION_COLOR` on each top-level window
with a caption, and puts them back when it is disabled. On Windows 10 2004 or later
the dark flag still gives a dark title bar, without the custom color.

A window that draws its own title bar, such as the main window of Photoshop,
Illustrator or InDesign, is left alone. This is checked per window, so the dialogs
in those apps are still colored. The dialog's contents stay whatever color the app
or Windows paints them.

## Scope

Anything an app paints from its own palette rather than from a system color or
the theme is out of reach, such as Photoshop's font browser and the Glyphs panel's
character cells. Recoloring those would mean matching color values rather than
intercepting a system one, in windows where a light fill can be the content.

A dropdown list Windows draws itself keeps its normal colors, as described above.

With **Scroll Long Menus Instead of Columns** turned off, a menu that an app
splits into columns stays light, because Windows turns its theme off for any menu
that has columns.

## Source and bug reports

The mod is developed at
[github.com/sabergraphics/adobe-dark-menus](https://github.com/sabergraphics/adobe-dark-menus).

Report a problem in
[Issues](https://github.com/sabergraphics/adobe-dark-menus/issues). Menu drawing
depends on the environment more than most things do, so please say which Adobe
app and version and which Windows version you are on, what your display scaling
is, and which menu or dropdown list is wrong. A screenshot usually says more than
a description does.
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- MenuBgColor: "#2D2D2D"
  $name: "Menu Background Color"
- MenuTextColor: "#DCDCDC"
  $name: "Menu Text Color"
- HighlightBgColor: "#505050"
  $name: "Highlight Background Color"
- HighlightTextColor: "#FFFFFF"
  $name: "Highlight Text Color"
- SeparatorColor: "#383838"
  $name: "Separator Line Color"
  $description: Set this to the menu background color to hide separators.
- GrayTextColor: "#808080"
  $name: "Disabled Text Color"
- BorderColor: "#2D2D2D"
  $name: "Border Color"
  $description: >-
    Color of the frame around menus an app draws itself and around dropdown lists.
    Leave empty to keep the system frame on those menus and to use the background
    color on lists.
- ThemeDropdowns: true
  $name: "Theme Dropdown Lists"
  $description: >-
    Also darken the dropdown lists an app draws itself, using the colors above.
- ThemeWindowsMenus: true
  $name: "Darken Menus Windows Draws"
  $description: >-
    Use Windows' dark menu theme for the menus Windows draws itself, which is
    most menus in most Adobe apps, and paint real menu bars dark.
- ScrollColumnMenus: true
  $name: "Scroll Long Menus Instead of Columns"
  $description: >-
    Windows cannot darken a menu that an app splits into columns, as After
    Effects does with its Effect menu once it holds more than the screen fits.
    With this on, such a menu opens as one dark column that scrolls. Turn it off
    to keep the columns, which stay light.
- ThemeTitleBars: true
  $name: "Dark Title Bars"
  $description: >-
    Color standard title bars to match the menus, dialogs included. A window that
    draws its own title bar is left alone.
- TitleBarColor: ""
  $name: "Title Bar Color"
  $description: Leave empty to match Menu Background Color.
- TitleBarTextColor: ""
  $name: "Title Bar Text Color"
  $description: >-
    Leave empty to let Windows choose, which dims the text on a window that is
    not active.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <stddef.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include <atomic>
#include <mutex>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

// Colors read by the hooks. Each also tells PublishColor whether its brush needs
// replacing. CLR_INVALID on the border means "keep the system frame".
std::atomic<COLORREF> g_colMenu{RGB(45, 45, 45)};
std::atomic<COLORREF> g_colText{RGB(220, 220, 220)};
std::atomic<COLORREF> g_colHighlight{RGB(80, 80, 80)};
std::atomic<COLORREF> g_colHiText{RGB(255, 255, 255)};
std::atomic<COLORREF> g_colGray{RGB(128, 128, 128)};
std::atomic<COLORREF> g_colSeparator{RGB(56, 56, 56)};
std::atomic<COLORREF> g_colBorder{CLR_INVALID};

// DWMWA_COLOR_DEFAULT: whatever Windows would use.
constexpr COLORREF kDwmColorDefault = 0xFFFFFFFF;
std::atomic<COLORREF> g_colTitleBar{RGB(45, 45, 45)};
std::atomic<COLORREF> g_colTitleBarText{kDwmColorDefault};

std::atomic<bool> g_themeDropdowns{true};
std::atomic<bool> g_themeWindowsMenus{true};
std::atomic<bool> g_scrollColumnMenus{true};
std::atomic<bool> g_themeTitleBars{true};

// Whether the process is actually in dark menu mode. Stays false with the setting
// on before Windows 10 1903.
std::atomic<bool> g_darkMenusActive{false};

// Never deleted. See Wh_ModUninit.
std::atomic<HBRUSH> g_hMenuBrush{nullptr};
std::atomic<HBRUSH> g_hTextBrush{nullptr};
std::atomic<HBRUSH> g_hHighlightBrush{nullptr};
std::atomic<HBRUSH> g_hHiTextBrush{nullptr};
std::atomic<HBRUSH> g_hGrayBrush{nullptr};
std::atomic<HBRUSH> g_hSeparatorBrush{nullptr};
std::atomic<HBRUSH> g_hBorderBrush{nullptr};

// TrackPopupMenu calls on this thread's stack. This does not decide whether a color
// is overridden: a menu's modal loop keeps dispatching to every other window on the
// thread, so "a menu is open" says nothing about what is being painted.
thread_local int tl_menuDepth = 0;

// These do decide it. Raised while a menu item's WM_DRAWITEM (ODT_MENU) or a dropdown
// list's painting is on this thread's stack. Outside them a hook is one TLS read.
thread_local int tl_menuDrawDepth = 0;
thread_local int tl_comboListDepth = 0;

// Raised while the mod itself is painting, so its own drawing skips the hooks.
thread_local int tl_ourPaintDepth = 0;

// Raises a counter for a scope. A counter left raised would paint the whole thread
// in the mod's colors, so a destructor closes it even if the app's code unwinds.
struct DepthGuard {
    explicit DepthGuard(int* pDepth) : pDepth(pDepth) { (*pDepth)++; }
    ~DepthGuard() { if (*pDepth > 0) (*pDepth)--; }

    DepthGuard(const DepthGuard&) = delete;
    DepthGuard& operator=(const DepthGuard&) = delete;

private:
    int* pDepth;
};

// What the opening hook did for each WM_DRAWITEM now on this thread's stack, innermost
// last, so the closing hook undoes exactly that. Working it out again from the message
// goes wrong for a list destroyed mid-draw: it no longer looks like a list by then, and
// nothing else would lower its counter. Windows calls the two hooks in strict pairs, also
// for a window destroyed while it handles the message.
enum class DrawKind : unsigned char { Other, Menu, List };
constexpr int kMaxNestedDraws = 16;
thread_local DrawKind tl_drawKinds[kMaxNestedDraws];
thread_local int tl_drawNesting = 0;

// Windhawk does not call Wh_ModUninit when the app exits, so the thread hooks and the
// subclasses are still up while the loader destroys this module's globals, and the app's
// own DLLs are torn down after that. A window one of them destroys then comes back through
// the return hook into a container that no longer exists. So once the process is exiting,
// nothing here touches the containers.
using RtlDllShutdownInProgress_t = BOOLEAN(NTAPI*)();
RtlDllShutdownInProgress_t g_pRtlDllShutdownInProgress = nullptr;

inline bool ProcessIsExiting() {
    return g_pRtlDllShutdownInProgress && g_pRtlDllShutdownInProgress();
}

// Set at the start of Wh_ModUninit. A thread that is inside the mod at that moment can be
// about to put up a hook or a subclass, and once uninit has emptied its lists nothing
// would take that one down again. So whoever registers one checks this first, and once
// more under the lock of the list it is about to add to.
std::atomic<bool> g_unloading{false};

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

// Returns false for an empty or invalid string, so a setting can mean "leave it alone".
bool ParseHexColor(PCWSTR hexStr, COLORREF* pOut) {
    const wchar_t* p = hexStr;
    while (*p == L' ') p++;
    if (*p == L'#') p++;

    size_t len = wcslen(p);
    while (len > 0 && p[len - 1] == L' ') len--;
    if (len == 0) return false;

    bool allHex = true;
    for (size_t i = 0; i < len && allHex; i++) {
        if (!iswxdigit(p[i])) allHex = false;
    }

    unsigned int r, g, b;
    if (allHex && len == 6 && swscanf_s(p, L"%2x%2x%2x", &r, &g, &b) == 3) {
        *pOut = RGB(r, g, b);
        return true;
    }
    if (allHex && len == 3 && swscanf_s(p, L"%1x%1x%1x", &r, &g, &b) == 3) {
        *pOut = RGB(r * 17, g * 17, b * 17);  // expand short hex
        return true;
    }

    Wh_Log(L"ParseHexColor: invalid format '%s', using default.", hexStr);
    return false;
}

COLORREF ParseHexColorOr(PCWSTR hexStr, COLORREF defaultColor) {
    COLORREF c;
    return ParseHexColor(hexStr, &c) ? c : defaultColor;
}

// A replaced brush is never deleted (see Wh_ModUninit), so only the colors that
// actually changed get a new one.
void PublishColor(std::atomic<COLORREF>* pColor, std::atomic<HBRUSH>* pSlot, COLORREF color) {
    if (pColor->load(std::memory_order_relaxed) == color &&
        pSlot->load(std::memory_order_acquire)) {
        return;
    }

    HBRUSH hNew = CreateSolidBrush(color);
    if (!hNew) return;

    pColor->store(color, std::memory_order_relaxed);
    pSlot->store(hNew, std::memory_order_release);
}

void LoadSettings() {
    g_themeDropdowns.store(Wh_GetIntSetting(L"ThemeDropdowns") != 0, std::memory_order_relaxed);
    g_themeWindowsMenus.store(Wh_GetIntSetting(L"ThemeWindowsMenus") != 0, std::memory_order_relaxed);
    g_scrollColumnMenus.store(Wh_GetIntSetting(L"ScrollColumnMenus") != 0, std::memory_order_relaxed);
    g_themeTitleBars.store(Wh_GetIntSetting(L"ThemeTitleBars") != 0, std::memory_order_relaxed);

    PublishColor(&g_colMenu, &g_hMenuBrush,
                 ParseHexColorOr(WindhawkUtils::StringSetting::make(L"MenuBgColor").get(), RGB(45, 45, 45)));
    PublishColor(&g_colText, &g_hTextBrush,
                 ParseHexColorOr(WindhawkUtils::StringSetting::make(L"MenuTextColor").get(), RGB(220, 220, 220)));
    PublishColor(&g_colHighlight, &g_hHighlightBrush,
                 ParseHexColorOr(WindhawkUtils::StringSetting::make(L"HighlightBgColor").get(), RGB(80, 80, 80)));
    PublishColor(&g_colHiText, &g_hHiTextBrush,
                 ParseHexColorOr(WindhawkUtils::StringSetting::make(L"HighlightTextColor").get(), RGB(255, 255, 255)));
    PublishColor(&g_colGray, &g_hGrayBrush,
                 ParseHexColorOr(WindhawkUtils::StringSetting::make(L"GrayTextColor").get(), RGB(128, 128, 128)));
    PublishColor(&g_colSeparator, &g_hSeparatorBrush,
                 ParseHexColorOr(WindhawkUtils::StringSetting::make(L"SeparatorColor").get(), RGB(56, 56, 56)));

    // Empty means "keep the system frame".
    COLORREF colBorder;
    if (ParseHexColor(WindhawkUtils::StringSetting::make(L"BorderColor").get(), &colBorder)) {
        PublishColor(&g_colBorder, &g_hBorderBrush, colBorder);
    } else if (g_hBorderBrush.load(std::memory_order_acquire)) {
        g_colBorder.store(CLR_INVALID, std::memory_order_relaxed);
        g_hBorderBrush.store(nullptr, std::memory_order_release);
    }

    // Read after the menu background, which an empty title bar color follows.
    COLORREF colTitleBar;
    if (!ParseHexColor(WindhawkUtils::StringSetting::make(L"TitleBarColor").get(), &colTitleBar)) {
        colTitleBar = g_colMenu.load(std::memory_order_relaxed);
    }
    g_colTitleBar.store(colTitleBar, std::memory_order_relaxed);

    COLORREF colTitleBarText;
    if (!ParseHexColor(WindhawkUtils::StringSetting::make(L"TitleBarTextColor").get(), &colTitleBarText)) {
        colTitleBarText = kDwmColorDefault;
    }
    g_colTitleBarText.store(colTitleBarText, std::memory_order_relaxed);
}

// ---------------------------------------------------------------------------
// Title bars
// ---------------------------------------------------------------------------

// DWMWINDOWATTRIBUTE values, spelled out because the names need a newer SDK.
constexpr DWORD kDwmUseImmersiveDarkMode = 20;
constexpr DWORD kDwmCaptionColor = 35;
constexpr DWORD kDwmTextColor = 36;

// Windows the mod has colored. The value is whether the mod turned the dark flag on,
// so a window that was already dark stays dark when this is undone.
std::unordered_map<HWND, bool> g_titledWindows;
std::mutex g_titledWindowsMutex;

// A window that draws its own title bar extends its client area over that space, which
// leaves nothing for DWM to color. Measured per window, since the dialogs of such apps
// have ordinary title bars. The side frame is subtracted because a maximized window's
// frame hangs off the screen on all sides.
bool HasSystemTitleBar(HWND hWnd) {
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if ((style & WS_CAPTION) != WS_CAPTION || (style & WS_CHILD)) return false;

    // Both rectangles come back in screen coordinates. ClientToScreen would not do: on a
    // mirrored window client x runs right to left, so its origin is the right edge.
    WINDOWINFO info = {};
    info.cbSize = sizeof(info);
    if (!GetWindowInfo(hWnd, &info)) return false;

    constexpr int kMinCaptionHeight = 10;
    int top = info.rcClient.top - info.rcWindow.top;
    int side = info.rcClient.left - info.rcWindow.left;
    return top - side >= kMinCaptionHeight;
}

void SetTitleBarColors(HWND hWnd, COLORREF caption, COLORREF text) {
    DwmSetWindowAttribute(hWnd, kDwmCaptionColor, &caption, sizeof(caption));
    DwmSetWindowAttribute(hWnd, kDwmTextColor, &text, sizeof(text));
}

// On Windows 10 the two color calls fail harmlessly, and the dark flag alone still
// gives a dark title bar.
void ApplyTitleBar(HWND hWnd) {
    if (!g_themeTitleBars.load(std::memory_order_relaxed) || ProcessIsExiting() ||
        !HasSystemTitleBar(hWnd)) {
        return;
    }

    // Recorded before the calls below, since a frame paint can come back through here.
    {
        std::lock_guard<std::mutex> lock(g_titledWindowsMutex);
        if (!g_titledWindows.try_emplace(hWnd, false).second) return;
    }

    BOOL dark = FALSE;
    if (FAILED(DwmGetWindowAttribute(hWnd, kDwmUseImmersiveDarkMode, &dark, sizeof(dark))) || !dark) {
        BOOL on = TRUE;
        if (SUCCEEDED(DwmSetWindowAttribute(hWnd, kDwmUseImmersiveDarkMode, &on, sizeof(on)))) {
            std::lock_guard<std::mutex> lock(g_titledWindowsMutex);
            g_titledWindows[hWnd] = true;
        }
    }

    SetTitleBarColors(hWnd, g_colTitleBar.load(std::memory_order_relaxed),
                      g_colTitleBarText.load(std::memory_order_relaxed));
}

void ForgetTitleBar(HWND hWnd) {
    if (ProcessIsExiting()) return;

    std::lock_guard<std::mutex> lock(g_titledWindowsMutex);
    g_titledWindows.erase(hWnd);
}

void RestoreTitleBars() {
    std::unordered_map<HWND, bool> windows;
    {
        std::lock_guard<std::mutex> lock(g_titledWindowsMutex);
        windows.swap(g_titledWindows);
    }

    for (const auto& entry : windows) {
        SetTitleBarColors(entry.first, kDwmColorDefault, kDwmColorDefault);
        if (entry.second) {
            BOOL off = FALSE;
            DwmSetWindowAttribute(entry.first, kDwmUseImmersiveDarkMode, &off, sizeof(off));
        }
    }
}

// Hidden windows are left until they show, when their frames are final.
void SyncTitleBars() {
    if (!g_themeTitleBars.load(std::memory_order_relaxed)) {
        RestoreTitleBars();
        return;
    }

    std::vector<HWND> known;
    {
        std::lock_guard<std::mutex> lock(g_titledWindowsMutex);
        for (const auto& entry : g_titledWindows) known.push_back(entry.first);
    }
    for (HWND hWnd : known) {
        SetTitleBarColors(hWnd, g_colTitleBar.load(std::memory_order_relaxed),
                          g_colTitleBarText.load(std::memory_order_relaxed));
    }

    EnumWindows([](HWND hWnd, LPARAM) WINAPI -> BOOL {
        DWORD pid = 0;
        GetWindowThreadProcessId(hWnd, &pid);
        if (pid == GetCurrentProcessId() && IsWindowVisible(hWnd)) ApplyTitleBar(hWnd);
        return TRUE;
    }, 0);
}

// ---------------------------------------------------------------------------
// Menu scoping
// ---------------------------------------------------------------------------

inline bool MenuIsOpen() {
    return tl_menuDepth > 0;
}

// The menu popup and the list half of a combo box.
constexpr PCWSTR kMenuPopupClass = L"#32768";
constexpr PCWSTR kComboListClass = L"ComboLBox";

// Photoshop paints menu items into a memory DC with no window while a popup is laid
// out, and onto the popup's own DC on hover. A memory DC would match anything, so this
// relies on the caller having checked tl_menuDrawDepth or tl_comboListDepth first.
bool IsPaintContext(HDC hdc, PCWSTR windowClass) {
    HWND hWnd = WindowFromDC(hdc);
    if (hWnd) {
        WCHAR cls[16];
        return GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) && wcscmp(cls, windowClass) == 0;
    }
    return GetObjectType(hdc) == OBJ_MEMDC;
}

// The mod's color for a system color index, or nullptr if it does not override it. The
// 3D frame colors are not listed because the kernel draws a menu popup's frame, which
// is repainted in the popup's subclass instead.
HBRUSH BrushForSysColor(int nIndex) {
    switch (nIndex) {
        case COLOR_MENU:
        case COLOR_MENUBAR:       return g_hMenuBrush.load(std::memory_order_acquire);
        case COLOR_MENUTEXT:      return g_hTextBrush.load(std::memory_order_acquire);
        case COLOR_HIGHLIGHT:     return g_hHighlightBrush.load(std::memory_order_acquire);
        case COLOR_HIGHLIGHTTEXT: return g_hHiTextBrush.load(std::memory_order_acquire);
        case COLOR_GRAYTEXT:      return g_hGrayBrush.load(std::memory_order_acquire);
    }
    return nullptr;
}

bool ColorForSysColor(int nIndex, COLORREF* pOut) {
    switch (nIndex) {
        case COLOR_MENU:
        case COLOR_MENUBAR:       *pOut = g_colMenu.load(std::memory_order_relaxed); return true;
        case COLOR_MENUTEXT:      *pOut = g_colText.load(std::memory_order_relaxed); return true;
        case COLOR_HIGHLIGHT:     *pOut = g_colHighlight.load(std::memory_order_relaxed); return true;
        case COLOR_HIGHLIGHTTEXT: *pOut = g_colHiText.load(std::memory_order_relaxed); return true;
        case COLOR_GRAYTEXT:      *pOut = g_colGray.load(std::memory_order_relaxed); return true;
    }
    return false;
}

// The same for dropdown lists. Which index a list reads is not consistent: Photoshop's
// Layers blend list takes its row text from COLOR_WINDOWTEXT and its options bar list
// from COLOR_MENUTEXT. So every background index maps to the background and every text
// index to the text.
bool ColorForDropdown(int nIndex, COLORREF* pOut) {
    switch (nIndex) {
        case COLOR_WINDOW:
        case COLOR_MENU:
        case COLOR_MENUBAR:       *pOut = g_colMenu.load(std::memory_order_relaxed); return true;
        case COLOR_WINDOWTEXT:
        case COLOR_MENUTEXT:      *pOut = g_colText.load(std::memory_order_relaxed); return true;
        case COLOR_HIGHLIGHT:     *pOut = g_colHighlight.load(std::memory_order_relaxed); return true;
        case COLOR_HIGHLIGHTTEXT: *pOut = g_colHiText.load(std::memory_order_relaxed); return true;
        case COLOR_GRAYTEXT:      *pOut = g_colGray.load(std::memory_order_relaxed); return true;
        case COLOR_WINDOWFRAME: {
            COLORREF border = g_colBorder.load(std::memory_order_relaxed);
            *pOut = border != CLR_INVALID ? border : g_colMenu.load(std::memory_order_relaxed);
            return true;
        }
    }
    return false;
}

// The brush half of the table above. Keep the two in step, since a list can fill from
// either route.
HBRUSH BrushForDropdown(int nIndex) {
    switch (nIndex) {
        case COLOR_WINDOW:
        case COLOR_MENU:
        case COLOR_MENUBAR:       return g_hMenuBrush.load(std::memory_order_acquire);
        case COLOR_WINDOWTEXT:
        case COLOR_MENUTEXT:      return g_hTextBrush.load(std::memory_order_acquire);
        case COLOR_HIGHLIGHT:     return g_hHighlightBrush.load(std::memory_order_acquire);
        case COLOR_HIGHLIGHTTEXT: return g_hHiTextBrush.load(std::memory_order_acquire);
        case COLOR_GRAYTEXT:      return g_hGrayBrush.load(std::memory_order_acquire);
        case COLOR_WINDOWFRAME: {
            HBRUSH hBorder = g_hBorderBrush.load(std::memory_order_acquire);
            return hBorder ? hBorder : g_hMenuBrush.load(std::memory_order_acquire);
        }
    }
    return nullptr;
}

// Separator geometry scales with DPI; 1px at 100% is often 2-3px at 175%.
bool IsSeparatorGeometry(HDC hdc, int w, int h) {
    int dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    if (dpi <= 0) dpi = 96;
    return h >= 1 && h <= MulDiv(3, dpi, 96) && w > MulDiv(20, dpi, 96);
}

// A background brush set on a menu outlives the mod, so stamped menus are remembered
// to be cleared at uninit. Capped because an app can rebuild its menus over a long
// session.
constexpr size_t kMaxTrackedMenus = 1024;
std::unordered_set<HMENU> g_stampedMenus;
std::mutex g_stampedMenusMutex;

// Whether the app paints this menu or Windows does. TrackPopupMenu says nothing about
// it: Illustrator and InDesign use it too, and Windows draws their items.
bool MenuHasOwnerDrawnItems(HMENU hMenu) {
    int count = GetMenuItemCount(hMenu);
    for (int i = 0; i < count; i++) {
        MENUITEMINFOW mii = {};
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_FTYPE;
        if (GetMenuItemInfoW(hMenu, (UINT)i, TRUE, &mii) && (mii.fType & MFT_OWNERDRAW)) {
            return true;
        }
    }
    return false;
}

// Stamps one menu and records it.
void StampMenu(HMENU hMenu, HBRUSH hBrush) {
    if (ProcessIsExiting()) return;

    // Checked before stamping, since a stamped menu that is not recorded stays dark
    // after uninit.
    {
        std::lock_guard<std::mutex> lock(g_stampedMenusMutex);
        if (g_stampedMenus.size() >= kMaxTrackedMenus && !g_stampedMenus.contains(hMenu)) {
            // Nothing removes an entry when the app destroys a menu, so a full set
            // is mostly dead handles.
            std::erase_if(g_stampedMenus, [](HMENU hDead) { return !IsMenu(hDead); });
            if (g_stampedMenus.size() >= kMaxTrackedMenus) return;
        }
    }

    MENUINFO mi = {};
    mi.cbSize = sizeof(mi);
    mi.fMask = MIM_BACKGROUND;
    mi.hbrBack = hBrush;
    if (!SetMenuInfo(hMenu, &mi)) return;

    // SetMenuInfo can reach a menu owned by another thread, so it is called outside
    // the lock.
    std::lock_guard<std::mutex> lock(g_stampedMenusMutex);
    g_stampedMenus.insert(hMenu);
}

// Far deeper than any real menu nests. It only stops a walk that would never end.
constexpr int kMaxMenuDepth = 25;

// Each menu of the tree is judged on its own. MIM_APPLYTOSUBMENUS would cover the tree in
// one call, but it also stamps a submenu Windows draws below a menu the app draws. On a
// menu Windows draws, the brush replaces only the background, while the gutter, separators
// and text still come from the theme.
void StampMenuTree(HMENU hMenu, HBRUSH hBrush, int depth) {
    bool ownerDrawn = false;
    int count = GetMenuItemCount(hMenu);
    for (int i = 0; i < count; i++) {
        MENUITEMINFOW mii = {};
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_FTYPE | MIIM_SUBMENU;
        if (!GetMenuItemInfoW(hMenu, (UINT)i, TRUE, &mii)) continue;

        if (mii.fType & MFT_OWNERDRAW) ownerDrawn = true;
        if (mii.hSubMenu && depth < kMaxMenuDepth) StampMenuTree(mii.hSubMenu, hBrush, depth + 1);
    }

    if (ownerDrawn) StampMenu(hMenu, hBrush);
}

void ApplyMenuBackground(HMENU hMenu) {
    HBRUSH hBrush = g_hMenuBrush.load(std::memory_order_acquire);
    if (hMenu && hBrush) StampMenuTree(hMenu, hBrush, 0);
}

// Every stamped menu is in the set by itself, so each is cleared by itself too.
void RestoreStampedMenus() {
    std::vector<HMENU> menus;
    {
        std::lock_guard<std::mutex> lock(g_stampedMenusMutex);
        menus.assign(g_stampedMenus.begin(), g_stampedMenus.end());
        g_stampedMenus.clear();
    }

    // Outside the lock, as in StampMenu.
    for (HMENU hMenu : menus) {
        MENUINFO mi = {};
        mi.cbSize = sizeof(mi);
        mi.fMask = MIM_BACKGROUND;
        mi.hbrBack = nullptr;
        SetMenuInfo(hMenu, &mi);
    }
}

// Width of the popup's non-client frame.
int GetPopupFrameWidth(HWND hWnd, const RECT& windowRect) {
    RECT client;
    POINT origin = { 0, 0 };
    if (!GetClientRect(hWnd, &client) || !ClientToScreen(hWnd, &origin)) return 1;

    // The sides only: above and below the items, the non-client area can also hold
    // scroll arrows.
    int left = origin.x - windowRect.left;
    int right = windowRect.right - (origin.x + (client.right - client.left));
    int frame = left > right ? left : right;

    if (frame < 1) frame = 1;
    if (frame > 4) frame = 4;
    return frame;
}

// The kernel draws the popup's frame, so it is repainted once the kernel is done.
// hdcTarget is the DC the system rendered the popup into. Menus are drawn offscreen and
// blitted, so painting on the window DC instead would be overwritten by the blit.
void PaintPopupBorder(HWND hWnd, HDC hdcTarget) {
    // The system reuses popups, so this one may be showing a menu the mod leaves alone.
    if (!MenuIsOpen()) return;

    // A menu Windows draws keeps the frame of its theme.
    HMENU hPopupMenu = (HMENU)SendMessageW(hWnd, MN_GETHMENU, 0, 0);
    if (hPopupMenu && !MenuHasOwnerDrawnItems(hPopupMenu)) return;

    HBRUSH hBorder = g_hBorderBrush.load(std::memory_order_acquire);
    if (!hBorder) return;

    RECT windowRect;
    if (!GetWindowRect(hWnd, &windowRect)) return;

    int frame = GetPopupFrameWidth(hWnd, windowRect);

    RECT rc = windowRect;
    OffsetRect(&rc, -rc.left, -rc.top);
    if (rc.right <= 0 || rc.bottom <= 0) return;

    HDC hdc = hdcTarget ? hdcTarget : GetWindowDC(hWnd);
    if (!hdc) return;

    {
        DepthGuard guard(&tl_ourPaintDepth);
        for (int i = 0; i < frame && rc.right > rc.left && rc.bottom > rc.top; i++) {
            FrameRect(hdc, &rc, hBorder);
            InflateRect(&rc, -1, -1);
        }
    }

    if (!hdcTarget) ReleaseDC(hWnd, hdc);
}

// The kernel draws the scroll arrows of a long menu in the popup's non-client area, with
// no user-mode call to hook and no regard for the dark theme, so they are painted over
// afterward. Which of two shapes it drew depends on whether the menu has a background
// brush:
//
// - None (a menu Windows draws): a light square holding a dark triangle, duller when
//   disabled. Inverting keeps that difference, since nothing exposes the arrow's state.
// - A brush (an owner-drawn menu the mod stamped): a bare triangle, COLOR_MENUTEXT when
//   enabled and embossed COLOR_BTNSHADOW and COLOR_BTNHIGHLIGHT when disabled. Black is
//   close to invisible on a dark menu, so both are recolored.
//
// Neither may undo itself, because a repaint can arrive with no new arrow behind it. An
// inverted square has lost the long light run near the top of the band that marks a fresh
// one, and the recoloring matches exact system colors that are gone once it has run.
//
// The shape is taken from the menu rather than read off the pixels: on a light brush the
// whole band is one light run, which would pass for a square from edge to edge.
bool MenuHasBackgroundBrush(HMENU hMenu) {
    MENUINFO mi = {};
    mi.cbSize = sizeof(mi);
    mi.fMask = MIM_BACKGROUND;
    return hMenu && GetMenuInfo(hMenu, &mi) && mi.hbrBack;
}

inline DWORD ToDibColor(COLORREF color) {
    return ((DWORD)GetRValue(color) << 16) | ((DWORD)GetGValue(color) << 8) | GetBValue(color);
}

inline COLORREF FromDibColor(DWORD value) {
    return RGB((value >> 16) & 255, (value >> 8) & 255, value & 255);
}

void InvertArrowSquare(HDC hdc, int x, int y, int width, int height, COLORREF back) {
    BitBlt(hdc, x, y, width, height, nullptr, 0, 0, DSTINVERT);

    // The square is black now and the triangle light. OR-ing the background in lifts
    // the black to exactly that color and leaves the triangle as it is.
    constexpr DWORD kRopDestOrPattern = 0x00FA0089;
    HBRUSH hBack = CreateSolidBrush(back);
    if (!hBack) return;

    HGDIOBJ hOldBrush = SelectObject(hdc, hBack);
    PatBlt(hdc, x, y, width, height, kRopDestOrPattern);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBack);
}

// Edits the band in place and copies back only what changed. A system color that
// matches the band's own background is left out, since it can't be told from it.
void RecolorBareArrow(HDC hdc, HDC hdcBand, DWORD* pixels, int left, int top, int width, int height) {
    DWORD back = pixels[0] & 0xFFFFFF;
    DWORD enabled = ToDibColor(GetSysColor(COLOR_MENUTEXT));
    DWORD shadow = ToDibColor(GetSysColor(COLOR_BTNSHADOW));
    DWORD highlight = ToDibColor(GetSysColor(COLOR_BTNHIGHLIGHT));

    bool hasEnabled = false, hasShadow = false, hasHighlight = false;
    for (int i = 0; i < width * height; i++) {
        DWORD value = pixels[i] & 0xFFFFFF;
        if (value == enabled && value != back) hasEnabled = true;
        if (value == shadow && value != back) hasShadow = true;
        if (value == highlight && value != back) hasHighlight = true;
    }

    DWORD text = ToDibColor(g_colText.load(std::memory_order_relaxed));
    DWORD gray = ToDibColor(g_colGray.load(std::memory_order_relaxed));

    // With a text color that happens to equal COLOR_BTNSHADOW, an enabled arrow that has
    // already been recolored looks like a disabled one. Only a disabled arrow the kernel
    // has just drawn still has its highlight emboss.
    if (text == shadow && !hasHighlight) hasShadow = false;

    if (!hasEnabled && !hasShadow) return;

    int minX = width, minY = height, maxX = -1, maxY = -1;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            DWORD& pixel = pixels[y * width + x];
            DWORD value = pixel & 0xFFFFFF;
            DWORD mapped = value;

            if (hasEnabled) {
                if (value == enabled) mapped = text;
            } else if (value == shadow) {
                mapped = gray;
            } else if (value == highlight) {
                mapped = back;
            }

            if (mapped != value) {
                pixel = mapped;
                if (x < minX) minX = x;
                if (x > maxX) maxX = x;
                if (y < minY) minY = y;
                if (y > maxY) maxY = y;
            }
        }
    }

    if (maxX >= 0) {
        BitBlt(hdc, left + minX, top + minY, maxX - minX + 1, maxY - minY + 1, hdcBand, minX, minY, SRCCOPY);
    }
}

// The non-client area is at most this tall beyond the frame when it holds no arrow.
constexpr int kMaxBandWithoutArrow = 6;

void PaintOverArrow(HDC hdc, int left, int top, int width, int height, bool bareTriangle) {
    if (width <= 0 || height <= kMaxBandWithoutArrow) return;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = width;
    bi.bmiHeader.biHeight = -height;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;

    HDC hdcBand = CreateCompatibleDC(hdc);
    if (!hdcBand) return;

    void* bits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(hdcBand, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!hBitmap) {
        DeleteDC(hdcBand);
        return;
    }

    HGDIOBJ hOldBitmap = SelectObject(hdcBand, hBitmap);
    BitBlt(hdcBand, 0, 0, width, height, hdc, left, top, SRCCOPY);
    GdiFlush();

    DWORD* pixels = (DWORD*)bits;
    auto isLight = [](DWORD value) {
        int red = (value >> 16) & 255, green = (value >> 8) & 255, blue = value & 255;
        return (red * 299 + green * 587 + blue * 114) / 1000 > 0x70;
    };

    int x0 = -1, x1 = -1, y0 = -1, y1 = -1;
    for (int y = 0; !bareTriangle && y <= height / 4 && y0 < 0; y++) {
        int run = 0, longest = 0, longestEnd = 0;
        for (int x = 0; x < width; x++) {
            run = isLight(pixels[y * width + x]) ? run + 1 : 0;
            if (run > longest) {
                longest = run;
                longestEnd = x;
            }
        }
        if (longest >= height / 2) {
            x0 = longestEnd - longest + 1;
            x1 = longestEnd;
            y0 = y;
        }
    }

    if (y0 >= 0) {
        for (int y = y0; y < height; y++) {
            if (isLight(pixels[y * width + x0])) y1 = y;
        }
    }

    // A light line is not the square. Inverting one would turn the dark band around
    // it light on the next pass, and back again on the one after.
    if (y0 >= 0 && y1 - y0 + 1 >= height / 2) {
        // Matched to what the theme painted beside the arrow, so it blends in whatever
        // shade this Windows build uses.
        COLORREF back = g_colMenu.load(std::memory_order_relaxed);
        int sampleX = x1 + 2 < width ? x1 + 2 : x0 - 2;
        if (sampleX >= 0) {
            DWORD sample = pixels[(y0 + (y1 - y0) / 2) * width + sampleX];
            if (!isLight(sample)) back = FromDibColor(sample);
        }
        InvertArrowSquare(hdc, left + x0, top + y0, x1 - x0 + 1, y1 - y0 + 1, back);
    } else {
        RecolorBareArrow(hdc, hdcBand, pixels, left, top, width, height);
    }

    SelectObject(hdcBand, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcBand);
}

// A couple of window manager calls on a menu with no arrows, about a millisecond on one
// that has them.
void PaintOverScrollArrows(HWND hWnd, HDC hdcTarget) {
    RECT window, client;
    POINT origin = { 0, 0 };
    if (!GetWindowRect(hWnd, &window) || !GetClientRect(hWnd, &client) ||
        !ClientToScreen(hWnd, &origin)) {
        return;
    }

    // Without arrows the non-client area is only the frame.
    int frame = origin.x - window.left;
    int clientTop = origin.y - window.top;
    if (clientTop <= frame + kMaxBandWithoutArrow) return;

    HMENU hMenu = (HMENU)SendMessageW(hWnd, MN_GETHMENU, 0, 0);

    // On a menu that has been left light, a light arrow is the right one.
    if (!g_darkMenusActive.load(std::memory_order_relaxed) &&
        (!hMenu || !MenuHasOwnerDrawnItems(hMenu))) {
        return;
    }

    bool bareTriangle = MenuHasBackgroundBrush(hMenu);

    HDC hdc = hdcTarget ? hdcTarget : GetWindowDC(hWnd);
    if (!hdc) return;

    {
        DepthGuard guard(&tl_ourPaintDepth);
        int width = window.right - window.left - frame * 2;
        int windowHeight = window.bottom - window.top;
        int clientBottom = clientTop + client.bottom;
        PaintOverArrow(hdc, frame, frame, width, clientTop - frame, bareTriangle);
        PaintOverArrow(hdc, frame, clientBottom, width, windowHeight - frame - clientBottom, bareTriangle);
    }

    if (!hdcTarget) ReleaseDC(hWnd, hdc);
}

// Undocumented: sent to a window to paint its frame into the DC in wParam.
constexpr UINT WM_NCUAHDRAWFRAME = 0x00AF;

enum class WindowKind { Other, MenuPopup, ComboList };

WindowKind ClassifyWindow(HWND hWnd) {
    WCHAR cls[16];
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) return WindowKind::Other;
    if (wcscmp(cls, kMenuPopupClass) == 0) return WindowKind::MenuPopup;
    if (wcscmp(cls, kComboListClass) == 0) return WindowKind::ComboList;
    return WindowKind::Other;
}

// Tracked so each subclass can be removed at uninit. The system reuses popups, so one
// can outlive the mod.
std::unordered_set<HWND> g_subclassedPopups;
std::mutex g_subclassedPopupsMutex;

// MN_SELECTITEM, undocumented: moves the selection in a menu popup. Its wParam is an item
// position, or one of these two while the pointer is over a scroll arrow.
constexpr UINT kMenuSelectItem = 0x01E5;
constexpr UINT kMenuUpArrow = 0xFFFFFFFD;
constexpr UINT kMenuDownArrow = 0xFFFFFFFC;

// The popup whose scroll arrow the pointer is on, if any. The kernel redraws an arrow when
// the pointer moves onto it and again when it moves off, with no paint message either time.
thread_local HWND tl_arrowPopup = nullptr;

// A subclass rather than a message hook, because WM_PAINT is posted and a
// WH_CALLWNDPROCRET hook never sees it.
LRESULT CALLBACK MenuPopupSubclassProc(HWND hWnd,
                                       UINT uMsg,
                                       WPARAM wParam,
                                       LPARAM lParam,
                                       [[maybe_unused]] DWORD_PTR dwRefData) {
    if (uMsg == WM_NCDESTROY) {
        if (tl_arrowPopup == hWnd) tl_arrowPopup = nullptr;

        if (!ProcessIsExiting()) {
            std::lock_guard<std::mutex> lock(g_subclassedPopupsMutex);
            g_subclassedPopups.erase(hWnd);
        }
    }

    // A scroll redraws the arrows and then repaints the rows, and it is the rows that take
    // the time. Painted over only afterward, the arrows would stay as the kernel drew them
    // for the whole repaint, which is long enough to be seen.
    if (uMsg == WM_PAINT) PaintOverScrollArrows(hWnd, nullptr);

    LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        // wParam is the DC the popup was rendered into. A caller asking for the client
        // area alone gets no frame, which would land over the items.
        case WM_PRINT:
            if (!(lParam & PRF_NONCLIENT)) break;
            [[fallthrough]];
        case WM_NCUAHDRAWFRAME:
            PaintPopupBorder(hWnd, (HDC)wParam);
            break;

        // On the window's own DC, since WM_ERASEBKGND's DC is client relative.
        case WM_PAINT:
        case WM_NCPAINT:
        case WM_ERASEBKGND:
            PaintPopupBorder(hWnd, nullptr);
            break;
    }

    // The arrows have no paint message of their own: the kernel redraws one in the middle
    // of a scroll. WM_PAINT follows any scroll, and the rest cover the routes that run
    // through this window. The UAH item messages are left out, since a repaint sends one
    // per row.
    switch (uMsg) {
        case WM_PRINT:
            if (lParam & PRF_NONCLIENT) PaintOverScrollArrows(hWnd, (HDC)wParam);
            break;

        case WM_PAINT:
        case WM_NCPAINT:
        case WM_TIMER:
        case WM_KEYDOWN:
        case WM_MOUSEWHEEL:
        case WM_MOUSEMOVE:
            PaintOverScrollArrows(hWnd, nullptr);
            break;

        // Sent on every mouse move, and it only leaves an arrow redrawn when the pointer
        // has moved onto one or off one. The rest are let through: reading the arrows
        // back for every move kept a long menu busy for as long as the mouse was moving.
        case kMenuSelectItem: {
            bool onArrow = (UINT)wParam == kMenuUpArrow || (UINT)wParam == kMenuDownArrow;
            HWND hLeft = tl_arrowPopup;
            tl_arrowPopup = onArrow ? hWnd : nullptr;

            if (onArrow) PaintOverScrollArrows(hWnd, nullptr);

            // The pointer may have left for another popup, and a remembered handle is only
            // as good as the day it was stored.
            if (hLeft && (hLeft != hWnd || !onArrow) && ClassifyWindow(hLeft) == WindowKind::MenuPopup) {
                PaintOverScrollArrows(hLeft, nullptr);
            }
            break;
        }
    }

    return result;
}

// Records a window that has just been subclassed. Returns false once Wh_ModUninit has
// begun: it may already have emptied the set and would never come back for this window, so
// the caller takes the subclass off again.
bool RecordSubclassed(std::unordered_set<HWND>& windows, std::mutex& mutex, HWND hWnd) {
    std::lock_guard<std::mutex> lock(mutex);
    if (g_unloading.load(std::memory_order_relaxed)) return false;

    windows.insert(hWnd);
    return true;
}

// Subclassed even when Border Color is empty: the system reuses popups, so one skipped
// here would ignore a border color set later.
void SubclassPopup(HWND hWnd) {
    if (ProcessIsExiting() || g_unloading.load(std::memory_order_relaxed)) return;

    {
        std::lock_guard<std::mutex> lock(g_subclassedPopupsMutex);
        if (g_subclassedPopups.contains(hWnd)) return;
    }

    // Not under the lock: from another thread the helper uses SendMessage, and the
    // subclass procedure takes the same lock on WM_NCDESTROY.
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, MenuPopupSubclassProc, 0)) {
        Wh_Log(L"Failed to subclass menu popup %p; it keeps the system frame.", hWnd);
        return;
    }

    if (!RecordSubclassed(g_subclassedPopups, g_subclassedPopupsMutex, hWnd)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, MenuPopupSubclassProc);
    }
}

bool IsComboListWindow(HWND hWnd) {
    return ClassifyWindow(hWnd) == WindowKind::ComboList;
}

// Only a list whose rows the app paints can be themed. Windows fills a stock list
// from a brush no hook sees, so recoloring its text would leave light text on white.
bool IsOwnerDrawnList(HWND hWnd) {
    return (GetWindowLongPtrW(hWnd, GWL_STYLE) & (LBS_OWNERDRAWFIXED | LBS_OWNERDRAWVARIABLE)) != 0;
}

// A dropdown's list sits directly on the desktop. The list of a CBS_SIMPLE combo is a
// child of the combo and part of the dialog around it, so it is left alone too.
bool IsDropdownList(HWND hWnd) {
    return GetAncestor(hWnd, GA_PARENT) == GetDesktopWindow();
}

bool IsThemedList(HWND hWnd) {
    return IsComboListWindow(hWnd) && IsOwnerDrawnList(hWnd) && IsDropdownList(hWnd);
}

inline bool IsListPaint(UINT uMsg) {
    switch (uMsg) {
        case WM_PAINT:
        case WM_ERASEBKGND:
        case WM_NCPAINT:
        case WM_PRINT:
        case WM_PRINTCLIENT:
        case WM_NCUAHDRAWFRAME:
            return true;
    }
    return false;
}

std::unordered_set<HWND> g_subclassedLists;
std::mutex g_subclassedListsMutex;

// The fill behind the rows happens outside any WM_DRAWITEM, so the list's own paint
// messages are bracketed here. Only those: a selection sends CBN_SELCHANGE from inside
// the list's input handling, and whatever the app reads or repaints in response has to
// see the real system colors. The rows are bracketed separately in CallWndProcHook,
// which also covers a row repainted on hover.
LRESULT CALLBACK ComboListSubclassProc(HWND hWnd,
                                       UINT uMsg,
                                       WPARAM wParam,
                                       LPARAM lParam,
                                       [[maybe_unused]] DWORD_PTR dwRefData) {
    if (uMsg == WM_NCDESTROY && !ProcessIsExiting()) {
        std::lock_guard<std::mutex> lock(g_subclassedListsMutex);
        g_subclassedLists.erase(hWnd);
    }

    if (!IsListPaint(uMsg) || !g_themeDropdowns.load(std::memory_order_relaxed) ||
        !IsDropdownList(hWnd)) {
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    DepthGuard guard(&tl_comboListDepth);
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Subclassed whatever the setting says, since a list window is created once and
// reused: one skipped here would stay light if the setting were turned on later.
// Dropdown or not is checked at paint time, because a list has not been moved to the
// desktop yet when the CBT hook sees it.
void SubclassComboList(HWND hWnd) {
    if (ProcessIsExiting() || g_unloading.load(std::memory_order_relaxed) || !IsOwnerDrawnList(hWnd)) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_subclassedListsMutex);
        if (g_subclassedLists.contains(hWnd)) return;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, ComboListSubclassProc, 0)) {
        Wh_Log(L"Failed to subclass dropdown list %p; it keeps the system colors.", hWnd);
        return;
    }

    if (!RecordSubclassed(g_subclassedLists, g_subclassedListsMutex, hWnd)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, ComboListSubclassProc);
        return;
    }

    Wh_Log(L"Subclassed dropdown list %p.", hWnd);

    // A list found mid-paint has already had its background filled.
    InvalidateRect(hWnd, nullptr, TRUE);
}

void SubclassIfKnown(HWND hWnd) {
    switch (ClassifyWindow(hWnd)) {
        case WindowKind::MenuPopup: SubclassPopup(hWnd); break;
        case WindowKind::ComboList: SubclassComboList(hWnd); break;
        case WindowKind::Other:     break;
    }
}

// Catches a menu popup or a dropdown list at creation, before it has painted.
// user32 creates both for itself, so nothing exported sees them appear, only a
// CBT hook does.
LRESULT CALLBACK CbtProcHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HCBT_CREATEWND) SubclassIfKnown((HWND)wParam);
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Menus an app splits into columns
// ---------------------------------------------------------------------------

// An app can break a long menu into columns. After Effects does it to a menu taller than
// the screen, which is what its Effect menu becomes with enough plugins installed: its
// menu update puts MFT_MENUBARBREAK on every item whose position is a multiple of the
// rows that fit. Windows draws a menu that has a break the old way, without the theme,
// so the dark theme never reaches it. Without the breaks the same menu is one themed
// column that scrolls.
//
// So the breaks come off while such a menu is open and go back on when it closes, which
// leaves the menu as the app built it whenever it is not on screen.
constexpr UINT kColumnBreaks = MFT_MENUBREAK | MFT_MENUBARBREAK;

// How many such menus can be open at once, submenus included, and how many columns one
// can have. Past either, a menu is left as the app made it.
constexpr int kMaxHeldMenus = 8;
constexpr int kMaxHeldBreaks = 32;

// A menu whose breaks the mod is holding, and where they go back.
struct HeldMenu {
    HMENU hMenu;
    DWORD threadId;
    int itemCount;
    int breakCount;
    UINT positions[kMaxHeldBreaks];
    UINT breaks[kMaxHeldBreaks];
};

// A plain array rather than a container, so there is nothing here for the loader to
// destroy at process exit. See ProcessIsExiting.
HeldMenu g_heldMenus[kMaxHeldMenus];
std::mutex g_heldMenusMutex;

// Read on every SetMenuItemInfo call in the process, so that the hook costs one load
// while no such menu is open.
std::atomic<int> g_heldMenuCount{0};

decltype(&SetMenuItemInfoW) SetMenuItemInfoW_Original;
decltype(&SetMenuItemInfoA) SetMenuItemInfoA_Original;

// Called with g_heldMenusMutex held. A null handle finds a free entry.
HeldMenu* FindHeldMenu(HMENU hMenu) {
    for (HeldMenu& held : g_heldMenus) {
        if (held.hMenu == hMenu) return &held;
    }
    return nullptr;
}

// Called with g_heldMenusMutex held. Returns false when the menu has more columns than
// an entry can record.
bool AddHeldBreak(HeldMenu* held, UINT pos, UINT breaks) {
    for (int i = 0; i < held->breakCount; i++) {
        if (held->positions[i] == pos) {
            held->breaks[i] = breaks;
            return true;
        }
    }
    if (held->breakCount == kMaxHeldBreaks) return false;

    held->positions[held->breakCount] = pos;
    held->breaks[held->breakCount] = breaks;
    held->breakCount++;
    return true;
}

bool GetItemType(HMENU hMenu, UINT pos, UINT* pType) {
    MENUITEMINFOW mii = {};
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_FTYPE;
    if (!GetMenuItemInfoW(hMenu, pos, TRUE, &mii)) return false;

    *pType = mii.fType;
    return true;
}

// Past the hook, which would hold back the mod's own writes as well.
bool SetItemType(HMENU hMenu, UINT pos, UINT type) {
    MENUITEMINFOW mii = {};
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_FTYPE;
    mii.fType = type;

    auto setItemInfo = SetMenuItemInfoW_Original ? SetMenuItemInfoW_Original : SetMenuItemInfoW;
    return setItemInfo(hMenu, pos, TRUE, &mii) != FALSE;
}

// Takes the breaks off a menu that is about to be shown. Called again for a menu that is
// already held, it picks up the breaks that have appeared since.
void HoldColumnMenu(HMENU hMenu) {
    if (!hMenu || !g_scrollColumnMenus.load(std::memory_order_relaxed) ||
        !g_darkMenusActive.load(std::memory_order_relaxed) || ProcessIsExiting() ||
        g_unloading.load(std::memory_order_relaxed)) {
        return;
    }

    int itemCount = GetMenuItemCount(hMenu);
    int found = 0;
    UINT positions[kMaxHeldBreaks], types[kMaxHeldBreaks];
    for (int i = 0; i < itemCount; i++) {
        UINT type;
        if (!GetItemType(hMenu, (UINT)i, &type)) continue;

        // A menu the app paints is dark through the app's own drawing, columns and all.
        if (type & MFT_OWNERDRAW) return;

        if (type & kColumnBreaks) {
            // Windows reports the type of an old-style bitmap item with MFT_BITMAP in it
            // and then refuses that type when it is written back. Taking some of a
            // menu's breaks off and not others would only move its columns.
            if ((type & MFT_BITMAP) || found == kMaxHeldBreaks) return;
            positions[found] = (UINT)i;
            types[found] = type;
            found++;
        }
    }
    if (found == 0) return;

    bool recorded[kMaxHeldBreaks] = {};
    {
        std::lock_guard<std::mutex> lock(g_heldMenusMutex);

        // Wh_ModUninit may already have been through the table.
        if (g_unloading.load(std::memory_order_relaxed)) return;

        HeldMenu* held = FindHeldMenu(hMenu);
        if (!held) {
            held = FindHeldMenu(nullptr);
            if (!held) {
                // Nothing reports a menu the app destroyed while it was held.
                for (HeldMenu& old : g_heldMenus) {
                    if (old.hMenu && !IsMenu(old.hMenu)) {
                        old = {};
                        g_heldMenuCount.fetch_sub(1, std::memory_order_relaxed);
                    }
                }
                held = FindHeldMenu(nullptr);
            }
            if (!held) return;

            *held = {};
            held->hMenu = hMenu;
            g_heldMenuCount.fetch_add(1, std::memory_order_relaxed);
        } else if (held->itemCount != itemCount) {
            // The app has rebuilt the menu since, so the positions on record are no use.
            held->breakCount = 0;
        }

        held->threadId = GetCurrentThreadId();
        held->itemCount = itemCount;
        for (int i = 0; i < found; i++) {
            recorded[i] = AddHeldBreak(held, positions[i], types[i] & kColumnBreaks);
        }
    }

    // A break that could not be recorded stays on, since nothing would put it back.
    for (int i = 0; i < found; i++) {
        if (recorded[i]) SetItemType(hMenu, positions[i], types[i] & ~kColumnBreaks);
    }
}

// Positions say nothing once the app has added or removed items. Its own next update
// decides where the breaks go then.
void RestoreColumnBreaks(const HeldMenu& held) {
    if (!IsMenu(held.hMenu) || GetMenuItemCount(held.hMenu) != held.itemCount) return;

    for (int i = 0; i < held.breakCount; i++) {
        UINT type;
        if (GetItemType(held.hMenu, held.positions[i], &type)) {
            SetItemType(held.hMenu, held.positions[i], type | held.breaks[i]);
        }
    }
}

// Whether a popup of this thread is showing the menu.
bool MenuIsOnScreen(HMENU hMenu) {
    struct Search {
        HMENU hMenu;
        bool found;
    } search = { hMenu, false };

    EnumThreadWindows(GetCurrentThreadId(), [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
        auto* pSearch = (Search*)lParam;
        if (IsWindowVisible(hWnd) && ClassifyWindow(hWnd) == WindowKind::MenuPopup &&
            (HMENU)SendMessageW(hWnd, MN_GETHMENU, 0, 0) == pSearch->hMenu) {
            pSearch->found = true;
            return FALSE;
        }
        return TRUE;
    }, (LPARAM)&search);

    return search.found;
}

// Puts the breaks back on every menu this thread no longer shows. It asks what is on
// screen rather than trusting the message that reports one menu closed, which also covers
// a popup that closes without that message. A menu that is still open is left held:
// changing an item's type on an open menu leaves Windows hit-testing a layout it has not
// made yet, and the pointer stops finding the items.
void ReleaseClosedColumnMenus() {
    if (g_heldMenuCount.load(std::memory_order_relaxed) == 0 || ProcessIsExiting()) return;

    DWORD tid = GetCurrentThreadId();
    HMENU mine[kMaxHeldMenus];
    int count = 0;
    {
        std::lock_guard<std::mutex> lock(g_heldMenusMutex);
        for (const HeldMenu& held : g_heldMenus) {
            if (held.hMenu && held.threadId == tid) mine[count++] = held.hMenu;
        }
    }

    // Asked outside the lock: MN_GETHMENU is a message to the popup.
    for (int i = 0; i < count; i++) {
        if (MenuIsOnScreen(mine[i])) continue;

        HeldMenu released;
        {
            std::lock_guard<std::mutex> lock(g_heldMenusMutex);
            HeldMenu* held = FindHeldMenu(mine[i]);
            if (!held) continue;

            released = *held;
            *held = {};
            g_heldMenuCount.fetch_sub(1, std::memory_order_relaxed);
        }
        RestoreColumnBreaks(released);
    }
}

// For Wh_ModUninit and for the setting going off, which run on another thread and cannot
// ask what is on screen. No menu is normally open then, since the app is not in front
// while its mod is switched off or its settings are changed.
void ReleaseAllColumnMenus() {
    HeldMenu released[kMaxHeldMenus];
    {
        std::lock_guard<std::mutex> lock(g_heldMenusMutex);
        for (int i = 0; i < kMaxHeldMenus; i++) {
            released[i] = g_heldMenus[i];
            g_heldMenus[i] = {};
        }
        g_heldMenuCount.store(0, std::memory_order_relaxed);
    }

    for (const HeldMenu& held : released) {
        if (held.hMenu) RestoreColumnBreaks(held);
    }
}

// The longest item text a write is compared against. A longer one is passed on unread.
constexpr int kMaxComparedText = 512;

// Whether writing these fields would leave the item exactly as it is.
template <typename Info, typename GetInfo>
bool ItemAlreadyMatches(HMENU hMenu, UINT pos, const Info& wanted, GetInfo getInfo) {
    constexpr UINT kCompared = MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_CHECKMARKS | MIIM_TYPE |
                               MIIM_DATA | MIIM_STRING | MIIM_BITMAP | MIIM_FTYPE;
    UINT mask = wanted.fMask;
    if (mask & ~kCompared) return false;

    // With MIIM_TYPE, dwTypeData is a bitmap or a value of the app's on these.
    if ((mask & MIIM_TYPE) && (wanted.fType & (MFT_BITMAP | MFT_OWNERDRAW))) return false;

    using Char = std::remove_pointer_t<decltype(wanted.dwTypeData)>;
    Char text[kMaxComparedText];
    text[0] = 0;

    Info current = {};
    current.cbSize = sizeof(current);
    current.fMask = mask;
    current.dwTypeData = text;
    current.cch = kMaxComparedText;
    if (!getInfo(hMenu, pos, TRUE, &current)) return false;

    if ((mask & (MIIM_FTYPE | MIIM_TYPE)) && current.fType != wanted.fType) return false;
    if ((mask & MIIM_STATE) && current.fState != wanted.fState) return false;
    if ((mask & MIIM_ID) && current.wID != wanted.wID) return false;
    if ((mask & MIIM_SUBMENU) && current.hSubMenu != wanted.hSubMenu) return false;
    if ((mask & MIIM_CHECKMARKS) && (current.hbmpChecked != wanted.hbmpChecked ||
                                     current.hbmpUnchecked != wanted.hbmpUnchecked)) {
        return false;
    }
    if ((mask & MIIM_DATA) && current.dwItemData != wanted.dwItemData) return false;
    if ((mask & MIIM_BITMAP) && current.hbmpItem != wanted.hbmpItem) return false;

    bool hasText = (mask & MIIM_STRING) || ((mask & MIIM_TYPE) && !(wanted.fType & MFT_SEPARATOR));
    if (hasText) {
        // A text the buffer cut short cannot be compared.
        if (!wanted.dwTypeData || current.cch >= (UINT)kMaxComparedText - 1) return false;

        for (int i = 0;; i++) {
            if (text[i] != wanted.dwTypeData[i]) return false;
            if (!text[i]) break;
        }
    }
    return true;
}

// A break written to a menu that is open without its breaks would undo that on the spot:
// Windows lays the open popup out again in columns, light, and scrolled back to the top.
// After Effects' menu update writes one wherever it finds a break missing, so it would do
// this if it ran while the menu is open. So on a held menu the break is noted, to go on
// when the menu closes, and the rest of the write is passed on without it.
//
// Usually there is no rest: the app is writing back what it read, plus the break. Windows
// would still repaint the whole popup for that, so a write that changes nothing is dropped.
//
// Returns false for a menu that is not held, which the caller passes on as it came. Only
// a write by position is looked at. A break belongs to a position, and a command id can
// just as well name an item inside a submenu.
template <typename Info, typename SetInfo, typename GetInfo>
bool SetHeldItemInfo(HMENU hMenu, UINT pos, BOOL byPosition, const Info* pInfo, SetInfo setInfo,
                     GetInfo getInfo, BOOL* pResult) {
    // Windows accepts the structure with or without its last member.
    if (!byPosition || !hMenu || ProcessIsExiting() || pInfo->cbSize > sizeof(Info) ||
        pInfo->cbSize < offsetof(Info, hbmpItem)) {
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(g_heldMenusMutex);
        HeldMenu* held = FindHeldMenu(hMenu);
        if (!held) return false;

        AddHeldBreak(held, pos, pInfo->fType & kColumnBreaks);
    }

    Info stripped = {};
    memcpy(&stripped, pInfo, pInfo->cbSize);
    stripped.fType &= ~kColumnBreaks;

    *pResult = ItemAlreadyMatches(hMenu, pos, stripped, getInfo) ? TRUE
                                                                 : setInfo(hMenu, pos, TRUE, &stripped);
    return true;
}

// ---------------------------------------------------------------------------
// Thread hooks
// ---------------------------------------------------------------------------

// A thread whose hooks are up. The engine does not remove SetWindowsHookEx hooks, and
// their procedures live in the mod image, so each one is kept to be released at uninit.
// The handle holds the thread open, because Windows does not reuse a thread's id while a
// handle to it is open. A new thread landing on a dead one's id would otherwise count as
// hooked and never be themed.
struct HookedThread {
    HANDLE hThread = nullptr;
    HHOOK hCwpRet = nullptr;
    HHOOK hCwp = nullptr;
    HHOOK hCbt = nullptr;
};

// Global so that Wh_ModAfterInit can hook a thread from outside it, and so that
// Wh_ModUninit, which runs on another thread, can take the hooks down again:
// UnhookWindowsHookEx works from any thread. Bounded by the number of UI threads alive.
std::unordered_map<DWORD, HookedThread> g_hookedThreads;
std::mutex g_hookedThreadsMutex;

void RemoveThreadHooks(const HookedThread& thread) {
    if (thread.hCwpRet) UnhookWindowsHookEx(thread.hCwpRet);
    if (thread.hCwp) UnhookWindowsHookEx(thread.hCwp);
    if (thread.hCbt) UnhookWindowsHookEx(thread.hCbt);
}

// Windows frees a thread's hooks when the thread exits, so only its handle is left to
// release. Called with g_hookedThreadsMutex held.
void ForgetExitedThreads() {
    std::erase_if(g_hookedThreads, [](const auto& entry) {
        HANDLE hThread = entry.second.hThread;
        if (!hThread || WaitForSingleObject(hThread, 0) != WAIT_OBJECT_0) return false;

        CloseHandle(hThread);
        return true;
    });
}

LRESULT CALLBACK CallWndProcHook(int code, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CallWndRetProcHook(int code, WPARAM wParam, LPARAM lParam);

// Per thread rather than per menu, because a dropdown list paints with no menu up.
// Returns whether the thread is recorded as hooked, which is false only when every hook
// failed and a later paint should retry.
bool InstallThreadHook(DWORD tid) {
    if (ProcessIsExiting()) return true;

    {
        std::lock_guard<std::mutex> lock(g_hookedThreadsMutex);
        if (g_unloading.load(std::memory_order_relaxed) || g_hookedThreads.contains(tid)) return true;
    }

    // A thread whose handle cannot be opened is still recorded, since reinstalling on
    // every call would be worse than the id reuse the handle guards against.
    HANDLE hThread = OpenThread(SYNCHRONIZE, FALSE, tid);

    {
        std::lock_guard<std::mutex> lock(g_hookedThreadsMutex);
        ForgetExitedThreads();

        // Two callers can reach this point for one thread: the thread itself
        // from a color hook, and the engine's thread from Wh_ModAfterInit.
        if (g_unloading.load(std::memory_order_relaxed) ||
            !g_hookedThreads.try_emplace(tid, HookedThread{ hThread }).second) {
            if (hThread) CloseHandle(hThread);
            return true;
        }
    }

    // The closing hook goes up first, and the opening one never goes up alone. With only
    // the opening hook installed, a WM_DRAWITEM would raise tl_comboListDepth with nothing
    // to lower it, and the whole thread would paint in the mod's colors. In this order the
    // odd message closes a draw that was never recorded, which the closing hook ignores.
    HHOOK hCwpRet = SetWindowsHookExW(WH_CALLWNDPROCRET, CallWndRetProcHook, nullptr, tid);
    DWORD errCwpRet = hCwpRet ? 0 : GetLastError();
    HHOOK hCwp = hCwpRet ? SetWindowsHookExW(WH_CALLWNDPROC, CallWndProcHook, nullptr, tid)
                         : nullptr;
    DWORD errCwp = (hCwpRet && !hCwp) ? GetLastError() : 0;
    HHOOK hCbt = SetWindowsHookExW(WH_CBT, CbtProcHook, nullptr, tid);
    DWORD errCbt = hCbt ? 0 : GetLastError();

    // Nothing went up, so let a later paint retry. A partial failure keeps what it got,
    // since retrying would add a second copy of the hooks that did install.
    bool retry = !hCwpRet && !hCwp && !hCbt;

    // The entry is gone if the thread has exited, or if Wh_ModUninit emptied the map in
    // the meantime. Either way nothing would take these hooks down later.
    bool recorded = false;
    {
        std::lock_guard<std::mutex> lock(g_hookedThreadsMutex);
        auto it = g_hookedThreads.find(tid);
        if (it != g_hookedThreads.end()) {
            recorded = true;
            if (retry) {
                if (it->second.hThread) CloseHandle(it->second.hThread);
                g_hookedThreads.erase(it);
            } else {
                it->second.hCwpRet = hCwpRet;
                it->second.hCwp = hCwp;
                it->second.hCbt = hCbt;
            }
        }
    }
    if (!recorded) {
        RemoveThreadHooks(HookedThread{ nullptr, hCwpRet, hCwp, hCbt });
        return true;
    }

    if (hCwpRet && hCwp && hCbt) {
        Wh_Log(L"Hooked thread %u.", tid);
        return true;
    }

    Wh_Log(L"SetWindowsHookEx failed on thread %u (ret=%u cwp=%u cbt=%u); menus "
           L"and dropdowns fall back to the system colors.",
           tid, errCwpRet, errCwp, errCbt);

    return !retry;
}

// Called from the color hooks, which any thread that paints reaches early. A thread with
// no windows yet is retried at most once a second.
thread_local bool tl_threadSwept = false;
thread_local ULONGLONG tl_lastSweep = 0;

void EnsureThreadHooks() {
    if (tl_threadSwept) return;

    ULONGLONG now = GetTickCount64();
    if (tl_lastSweep && now - tl_lastSweep < 1000) return;
    tl_lastSweep = now;

    // Set before the sweep: subclassing can paint, and painting comes back through here.
    tl_threadSwept = true;

    // Windows this thread already has, which the CBT hook never saw. The system reuses
    // both kinds, so one that predates the mod would otherwise never be caught. A
    // dropdown's list sits on the desktop, so this enumeration reaches it.
    int windows = 0;
    EnumThreadWindows(GetCurrentThreadId(), [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
        (*(int*)lParam)++;
        SubclassIfKnown(hWnd);
        return TRUE;
    }, (LPARAM)&windows);

    if (windows == 0) {
        tl_threadSwept = false;  // not a UI thread yet, ask again later
        return;
    }

    if (!InstallThreadHook(GetCurrentThreadId())) {
        tl_threadSwept = false;  // hooks would not go up, ask again later
    }
}

bool IsMenuItemDraw(const DRAWITEMSTRUCT* di) {
    return di->CtlType == ODT_MENU;
}

// A dropdown row produces two of these: the inner one the list sends its parent
// (ODT_LISTBOX, hwndItem the list) and the outer one the combo forwards to the app
// (ODT_COMBOBOX, hwndItem the combo). A plain owner-drawn list box looks like the inner
// one, so the class is checked rather than the type alone. The closed combo field also
// arrives as a list draw, marked ODS_COMBOBOXEDIT, and is left to the app.
//
// Only the opening hook asks this. The closing hook goes by what was recorded then, since
// the answer can change while the row is drawn. See tl_drawKinds.
bool IsListItemDraw(const DRAWITEMSTRUCT* di) {
    if (di->itemState & ODS_COMBOBOXEDIT) return false;
    if (di->CtlType == ODT_COMBOBOX) {
        return (GetWindowLongPtrW(di->hwndItem, GWL_STYLE) & CBS_DROPDOWNLIST) != CBS_SIMPLE;
    }
    return di->CtlType == ODT_LISTBOX && IsThemedList(di->hwndItem);
}

// Some of Photoshop's list drawing paints with whatever is already on the DC, and what
// user32 left there is the real COLOR_WINDOWTEXT. An app that sets its own colors is
// unaffected, since its call lands after this one.
void PrepareListItemDC(const DRAWITEMSTRUCT* di) {
    if (!di->hDC) return;

    bool selected = (di->itemState & ODS_SELECTED) != 0;
    SetTextColor(di->hDC, selected ? g_colHiText.load(std::memory_order_relaxed)
                                   : g_colText.load(std::memory_order_relaxed));
    SetBkColor(di->hDC, selected ? g_colHighlight.load(std::memory_order_relaxed)
                                 : g_colMenu.load(std::memory_order_relaxed));
}

// Opens the counters for exactly the span in which an item is painted.
// CallWndRetProcHook closes them.
LRESULT CALLBACK CallWndProcHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code != HC_ACTION || !lParam) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    auto* cwp = (CWPSTRUCT*)lParam;

    if (cwp->message == WM_DRAWITEM && cwp->lParam) {
        auto* di = (const DRAWITEMSTRUCT*)cwp->lParam;

        // Past the end of the record, a draw is left alone rather than counted with
        // nothing to say how to uncount it.
        DrawKind kind = DrawKind::Other;
        if (tl_drawNesting < kMaxNestedDraws) {
            // Only a menu the mod is tracking, so one it leaves alone is not half-themed.
            if (IsMenuItemDraw(di) && MenuIsOpen()) {
                kind = DrawKind::Menu;
            } else if (IsListItemDraw(di) && g_themeDropdowns.load(std::memory_order_relaxed)) {
                kind = DrawKind::List;
            }
            tl_drawKinds[tl_drawNesting] = kind;
        }
        tl_drawNesting++;

        if (kind == DrawKind::Menu) {
            tl_menuDrawDepth++;
        } else if (kind == DrawKind::List) {
            tl_comboListDepth++;
            // On the inner draw hwndItem is the list, which catches one the CBT hook
            // never saw. On the outer draw it is the combo, hence the class check.
            if (IsComboListWindow(di->hwndItem)) SubclassComboList(di->hwndItem);
            PrepareListItemDC(di);
        }
    } else if (cwp->message == WM_CTLCOLORLISTBOX &&
               g_themeDropdowns.load(std::memory_order_relaxed)) {
        // Also names the list, earlier in the paint than the first row.
        if (IsComboListWindow((HWND)cwp->lParam)) SubclassComboList((HWND)cwp->lParam);
    } else if (cwp->message == WM_UNINITMENUPOPUP || cwp->message == WM_EXITMENULOOP) {
        // Ahead of the app's own handler, which then finds its menu as it left it.
        ReleaseClosedColumnMenus();
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// Closes the counters, and sees to a popup that has just been initialized.
LRESULT CALLBACK CallWndRetProcHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code != HC_ACTION || !lParam) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    auto* ret = (CWPRETSTRUCT*)lParam;

    // Just before a window is shown its frame is final and nothing of it is on screen.
    // This also reaches dialogs, which user32 creates without the exported
    // CreateWindowExW. The second message covers a show that skips the first.
    if (ret->message == WM_WINDOWPOSCHANGING || ret->message == WM_WINDOWPOSCHANGED) {
        if (ret->lParam && (((const WINDOWPOS*)ret->lParam)->flags & SWP_SHOWWINDOW)) {
            ApplyTitleBar(ret->hwnd);
        }
    } else if (ret->message == WM_NCDESTROY) {
        ForgetTitleBar(ret->hwnd);
    }

    if (ret->message == WM_INITMENUPOPUP) {
        // In the return hook, so that what the app does to the menu in its own handler is
        // seen: the items it adds, and the column breaks it sets.
        HoldColumnMenu((HMENU)ret->wParam);
        if (MenuIsOpen()) ApplyMenuBackground((HMENU)ret->wParam);
    } else if (ret->message == WM_DRAWITEM && ret->lParam) {
        // Nothing is recorded for a draw that began before the opening hook was up.
        if (tl_drawNesting > 0) {
            tl_drawNesting--;
            DrawKind kind = tl_drawNesting < kMaxNestedDraws ? tl_drawKinds[tl_drawNesting]
                                                             : DrawKind::Other;
            if (kind == DrawKind::Menu) {
                if (tl_menuDrawDepth > 0) tl_menuDrawDepth--;
            } else if (kind == DrawKind::List) {
                if (tl_comboListDepth > 0) tl_comboListDepth--;
            }
        }
    } else if (ret->message == WM_CTLCOLORLISTBOX &&
               g_themeDropdowns.load(std::memory_order_relaxed) &&
               IsThemedList((HWND)ret->lParam)) {
        // After the parent has answered, so the mod's colors stand for whatever the list
        // paints outside a row.
        SetTextColor((HDC)ret->wParam, g_colText.load(std::memory_order_relaxed));
        SetBkColor((HDC)ret->wParam, g_colMenu.load(std::memory_order_relaxed));
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// Nested TrackPopupMenu calls share the outermost entry.
void EnterMenu(HMENU hMenu) {
    tl_menuDepth++;
    EnsureThreadHooks();

    // From here as well, for a popup tracked with TPM_NONOTIFY, whose owner is sent no
    // WM_INITMENUPOPUP.
    HoldColumnMenu(hMenu);
    ApplyMenuBackground(hMenu);
}

void LeaveMenu() {
    // And no WM_UNINITMENUPOPUP either.
    ReleaseClosedColumnMenus();

    if (--tl_menuDepth > 0) return;

    // A menu dismissed mid-draw would otherwise leave the overrides stuck on.
    tl_menuDrawDepth = 0;
}

// ---------------------------------------------------------------------------
// Menus Windows draws itself
// ---------------------------------------------------------------------------

// Most Adobe apps leave their menus to Windows, which paints them through the theme
// engine. uxtheme has a dark variant of that drawing behind two exports with an ordinal
// and no name. Ordinal 135 is a different function before Windows 10 1903, hence the
// version check.
enum class PreferredAppMode { Default, AllowDark, ForceDark, ForceLight, Max };
using SetPreferredAppMode_t = PreferredAppMode(WINAPI*)(PreferredAppMode);
using FlushMenuThemes_t = void(WINAPI*)();

constexpr WORD kOrdinalSetPreferredAppMode = 135;
constexpr WORD kOrdinalFlushMenuThemes = 136;
constexpr DWORD kFirstBuildWithPreferredAppMode = 18362;

HMODULE g_hUxtheme = nullptr;
SetPreferredAppMode_t g_pSetPreferredAppMode = nullptr;
FlushMenuThemes_t g_pFlushMenuThemes = nullptr;
PreferredAppMode g_previousAppMode = PreferredAppMode::Default;

void ResolveDarkMenuApis() {
    using RtlGetVersion_t = LONG(WINAPI*)(RTL_OSVERSIONINFOW*);
    auto pRtlGetVersion =
        (RtlGetVersion_t)(void*)GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion");

    RTL_OSVERSIONINFOW version = {};
    version.dwOSVersionInfoSize = sizeof(version);
    if (!pRtlGetVersion || pRtlGetVersion(&version) != 0 ||
        version.dwBuildNumber < kFirstBuildWithPreferredAppMode) {
        Wh_Log(L"Windows build too old for dark menus; Windows-drawn menus stay light.");
        return;
    }

    g_hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_hUxtheme) return;

    g_pSetPreferredAppMode = (SetPreferredAppMode_t)(void*)GetProcAddress(
        g_hUxtheme, MAKEINTRESOURCEA(kOrdinalSetPreferredAppMode));
    g_pFlushMenuThemes = (FlushMenuThemes_t)(void*)GetProcAddress(
        g_hUxtheme, MAKEINTRESOURCEA(kOrdinalFlushMenuThemes));
}

// The flush applies the change to a running app, which otherwise keeps the menu theme
// it already opened.
void ApplyDarkMenus() {
    if (!g_pSetPreferredAppMode || !g_pFlushMenuThemes) return;

    bool want = g_themeWindowsMenus.load(std::memory_order_relaxed);
    if (want == g_darkMenusActive.load(std::memory_order_relaxed)) return;

    if (want) {
        g_previousAppMode = g_pSetPreferredAppMode(PreferredAppMode::ForceDark);
    } else {
        g_pSetPreferredAppMode(g_previousAppMode);
    }
    g_pFlushMenuThemes();

    g_darkMenusActive.store(want, std::memory_order_relaxed);
}

// GetMenu on a child window returns its control id rather than a menu.
bool HasRealMenuBar(HWND hWnd) {
    return !(GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_CHILD) && GetMenu(hWnd) != nullptr;
}

// Nothing repaints a window frame because a mod loaded or a color changed.
// RDW_INVALIDATE only queues the paint, so this is safe from any thread.
void RedrawMenuBars() {
    EnumWindows([](HWND hWnd, LPARAM) WINAPI -> BOOL {
        DWORD pid = 0;
        GetWindowThreadProcessId(hWnd, &pid);
        if (pid == GetCurrentProcessId() && HasRealMenuBar(hWnd)) {
            RedrawWindow(hWnd, nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE);
        }
        return TRUE;
    }, 0);
}

// The dark theme does not reach a real menu bar, but Windows offers each paint to
// the window first through these undocumented messages, and a window that does not
// handle them passes them to DefWindowProc. Only the fields used here are declared:
// the rest of the structure follows iPosition.
constexpr UINT WM_UAHDRAWMENU = 0x0091;
constexpr UINT WM_UAHDRAWMENUITEM = 0x0092;

struct UAHMENU {
    HMENU hmenu;
    HDC hdc;
    DWORD dwFlags;
};

struct UAHDRAWMENUITEM {
    DRAWITEMSTRUCT dis;
    UAHMENU um;
    int iPosition;
};

bool PaintMenuBarBackground(HWND hWnd, const UAHMENU* pMenu) {
    HBRUSH hBrush = g_hMenuBrush.load(std::memory_order_acquire);
    if (!hBrush || !pMenu->hdc) return false;

    MENUBARINFO info = {};
    info.cbSize = sizeof(info);
    RECT window;
    if (!GetMenuBarInfo(hWnd, OBJID_MENU, 0, &info) || !GetWindowRect(hWnd, &window)) return false;

    RECT rc = info.rcBar;
    OffsetRect(&rc, -window.left, -window.top);
    FillRect(pMenu->hdc, &rc, hBrush);
    return true;
}

bool PaintMenuBarItem(const UAHDRAWMENUITEM* pItem) {
    WCHAR label[256] = {};
    MENUITEMINFOW mii = {};
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_STRING | MIIM_FTYPE;
    mii.dwTypeData = label;
    mii.cch = ARRAYSIZE(label) - 1;
    if (!GetMenuItemInfoW(pItem->um.hmenu, (UINT)pItem->iPosition, TRUE, &mii)) return false;

    // An owner-drawn item paints itself, and one with no text is a bitmap such as
    // an MDI child's window buttons, which only the system knows how to draw.
    if ((mii.fType & MFT_OWNERDRAW) || !label[0]) return false;

    UINT state = pItem->dis.itemState;
    bool hot = (state & (ODS_HOTLIGHT | ODS_SELECTED)) != 0;
    bool disabled = (state & (ODS_GRAYED | ODS_DISABLED)) != 0;

    HBRUSH hBrush = (hot ? g_hHighlightBrush : g_hMenuBrush).load(std::memory_order_acquire);
    if (!hBrush) return false;

    COLORREF textColor = (disabled ? g_colGray
                          : hot    ? g_colHiText
                                   : g_colText).load(std::memory_order_relaxed);

    HDC hdc = pItem->um.hdc;
    RECT rc = pItem->dis.rcItem;
    FillRect(hdc, &rc, hBrush);

    int oldMode = SetBkMode(hdc, TRANSPARENT);
    COLORREF oldColor = SetTextColor(hdc, textColor);
    UINT format = DT_CENTER | DT_SINGLELINE | DT_VCENTER;
    if (state & ODS_NOACCEL) format |= DT_HIDEPREFIX;
    DrawTextW(hdc, label, -1, &rc, format);
    SetTextColor(hdc, oldColor);
    SetBkMode(hdc, oldMode);
    return true;
}

// Returns whether the message was painted here. Tied to the dark menus actually
// being on, so a dark bar is never left sitting above light dropdowns.
bool HandleMenuBarMessage(HWND hWnd, UINT uMsg, LPARAM lParam) {
    if (!lParam || !g_darkMenusActive.load(std::memory_order_relaxed)) return false;

    DepthGuard guard(&tl_ourPaintDepth);
    return uMsg == WM_UAHDRAWMENU ? PaintMenuBarBackground(hWnd, (const UAHMENU*)lParam)
                                  : PaintMenuBarItem((const UAHDRAWMENUITEM*)lParam);
}

// DefWindowProc draws a 1px light line under the menu bar, outside anything the
// messages above cover, at the top edge of the client area.
void PaintMenuBarBottomLine(HWND hWnd) {
    if (!g_darkMenusActive.load(std::memory_order_relaxed) || !HasRealMenuBar(hWnd)) return;

    HBRUSH hBrush = g_hMenuBrush.load(std::memory_order_acquire);
    if (!hBrush) return;

    RECT client, window;
    if (!GetClientRect(hWnd, &client) || !GetWindowRect(hWnd, &window)) return;
    MapWindowPoints(hWnd, nullptr, (POINT*)&client, 2);
    OffsetRect(&client, -window.left, -window.top);

    RECT line = client;
    line.bottom = line.top;
    line.top--;

    HDC hdc = GetWindowDC(hWnd);
    if (!hdc) return;
    {
        DepthGuard guard(&tl_ourPaintDepth);
        FillRect(hdc, &line, hBrush);
    }
    ReleaseDC(hWnd, hdc);
}

inline bool IsMenuBarPaint(UINT uMsg) {
    return uMsg == WM_UAHDRAWMENU || uMsg == WM_UAHDRAWMENUITEM;
}

inline bool IsFramePaint(UINT uMsg) {
    return uMsg == WM_NCPAINT || uMsg == WM_NCACTIVATE;
}

// Also the catch-all for title bars: a window that gains its caption after it is shown,
// or that was skipped while minimized, gets here the next time its frame paints.
inline void OnFramePaint(HWND hWnd) {
    PaintMenuBarBottomLine(hWnd);
    ApplyTitleBar(hWnd);
}

// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------

decltype(&TrackPopupMenu) TrackPopupMenu_Original;
BOOL WINAPI TrackPopupMenu_Hook(HMENU hMenu, UINT uFlags, int x, int y,
                                int nReserved, HWND hWnd, const RECT* prcRect) {
    EnterMenu(hMenu);
    BOOL bRes = TrackPopupMenu_Original(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
    LeaveMenu();
    return bRes;
}

decltype(&TrackPopupMenuEx) TrackPopupMenuEx_Original;
BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y,
                                  HWND hWnd, LPTPMPARAMS lptpm) {
    EnterMenu(hMenu);
    BOOL bRes = TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, lptpm);
    LeaveMenu();
    return bRes;
}

// The mod can load before the app has a window, so the thread hooks cannot all go up at
// init. A thread that creates a window is a UI thread.
decltype(&CreateWindowExW) CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, PCWSTR className, PCWSTR windowName,
                                 DWORD style, int x, int y, int width, int height,
                                 HWND hParent, HMENU hMenu, HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(exStyle, className, windowName, style,
                                         x, y, width, height, hParent, hMenu,
                                         hInstance, lpParam);
    if (hWnd) {
        EnsureThreadHooks();
        // A thread's first window can be on screen by the time its message hooks go up.
        if (IsWindowVisible(hWnd)) ApplyTitleBar(hWnd);
    }
    return hWnd;
}

// Both color hooks are also where the mod first notices a UI thread.
decltype(&GetSysColor) GetSysColor_Original;
DWORD WINAPI GetSysColor_Hook(int nIndex) {
    EnsureThreadHooks();

    COLORREF color;
    if (tl_ourPaintDepth == 0) {
        if (tl_menuDrawDepth > 0 && ColorForSysColor(nIndex, &color)) return color;
        if (tl_comboListDepth > 0 && ColorForDropdown(nIndex, &color)) return color;
    }
    return GetSysColor_Original(nIndex);
}

decltype(&GetSysColorBrush) GetSysColorBrush_Original;
HBRUSH WINAPI GetSysColorBrush_Hook(int nIndex) {
    EnsureThreadHooks();

    if (tl_ourPaintDepth == 0) {
        // Callers may cache this handle, so these brushes are never deleted.
        HBRUSH hBrush = nullptr;
        if (tl_menuDrawDepth > 0) {
            hBrush = BrushForSysColor(nIndex);
        } else if (tl_comboListDepth > 0) {
            hBrush = BrushForDropdown(nIndex);
        }
        if (hBrush) return hBrush;
    }
    return GetSysColorBrush_Original(nIndex);
}

decltype(&FillRect) FillRect_Original;
int WINAPI FillRect_Hook(HDC hdc, const RECT* lprc, HBRUSH hbr) {
    bool inMenu = tl_menuDrawDepth > 0;
    bool inList = tl_comboListDepth > 0;
    if ((!inMenu && !inList) || tl_ourPaintDepth > 0 || !lprc) {
        return FillRect_Original(hdc, lprc, hbr);
    }

    // Some owner-draw code fills with a system color pseudo-handle such as
    // (HBRUSH)(COLOR_MENU + 1). user32 resolves those from shared memory without
    // consulting GetSysColor or GetSysColorBrush, so they are re-resolved here.
    ULONG_PTR sysIndex = (ULONG_PTR)hbr - 1;
    if (sysIndex <= (ULONG_PTR)COLOR_MENUBAR) {
        HBRUSH ours = inMenu ? BrushForSysColor((int)sysIndex)
                             : BrushForDropdown((int)sysIndex);
        if (ours && IsPaintContext(hdc, inMenu ? kMenuPopupClass : kComboListClass)) {
            return FillRect_Original(hdc, lprc, ours);
        }
        return FillRect_Original(hdc, lprc, hbr);
    }

    // Separator lines are a menu shape; a list draws its dividers as part of an
    // item, so the geometry guess is not extended to one.
    int w = lprc->right - lprc->left;
    int h = lprc->bottom - lprc->top;
    HBRUSH hSep = g_hSeparatorBrush.load(std::memory_order_acquire);
    if (inMenu && hSep && IsSeparatorGeometry(hdc, w, h) && IsPaintContext(hdc, kMenuPopupClass)) {
        return FillRect_Original(hdc, lprc, hSep);
    }

    return FillRect_Original(hdc, lprc, hbr);
}

decltype(&PatBlt) PatBlt_Original;
BOOL WINAPI PatBlt_Hook(HDC hdc, int x, int y, int w, int h, DWORD rop) {
    if (tl_menuDrawDepth <= 0 || tl_ourPaintDepth > 0 || rop != PATCOPY) {
        return PatBlt_Original(hdc, x, y, w, h, rop);
    }

    HBRUSH hSep = g_hSeparatorBrush.load(std::memory_order_acquire);
    if (hSep && IsSeparatorGeometry(hdc, w, h) && IsPaintContext(hdc, kMenuPopupClass)) {
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hSep);
        BOOL bRes = PatBlt_Original(hdc, x, y, w, h, rop);
        SelectObject(hdc, hOldBrush);
        return bRes;
    }

    return PatBlt_Original(hdc, x, y, w, h, rop);
}

// These four run for every message in the process, so anything but the few that matter
// goes straight through on two compares.
decltype(&DefWindowProcW) DefWindowProcW_Original;
LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (IsMenuBarPaint(uMsg) && HandleMenuBarMessage(hWnd, uMsg, lParam)) return 0;
    LRESULT result = DefWindowProcW_Original(hWnd, uMsg, wParam, lParam);
    if (IsFramePaint(uMsg)) OnFramePaint(hWnd);
    return result;
}

decltype(&DefWindowProcA) DefWindowProcA_Original;
LRESULT WINAPI DefWindowProcA_Hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (IsMenuBarPaint(uMsg) && HandleMenuBarMessage(hWnd, uMsg, lParam)) return 0;
    LRESULT result = DefWindowProcA_Original(hWnd, uMsg, wParam, lParam);
    if (IsFramePaint(uMsg)) OnFramePaint(hWnd);
    return result;
}

decltype(&DefFrameProcW) DefFrameProcW_Original;
LRESULT WINAPI DefFrameProcW_Hook(HWND hWnd, HWND hWndClient, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (IsMenuBarPaint(uMsg) && HandleMenuBarMessage(hWnd, uMsg, lParam)) return 0;
    LRESULT result = DefFrameProcW_Original(hWnd, hWndClient, uMsg, wParam, lParam);
    if (IsFramePaint(uMsg)) OnFramePaint(hWnd);
    return result;
}

decltype(&DefFrameProcA) DefFrameProcA_Original;
LRESULT WINAPI DefFrameProcA_Hook(HWND hWnd, HWND hWndClient, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (IsMenuBarPaint(uMsg) && HandleMenuBarMessage(hWnd, uMsg, lParam)) return 0;
    LRESULT result = DefFrameProcA_Original(hWnd, hWndClient, uMsg, wParam, lParam);
    if (IsFramePaint(uMsg)) OnFramePaint(hWnd);
    return result;
}

// An app setting its own background on a menu Windows draws would put the theme's
// light text on it, or leave a strip of the theme's gutter beside it. Clearing the
// background hands the whole popup back to the theme. The mod's own stamp passes
// through untouched, and so does an app clearing its background.
decltype(&SetMenuInfo) SetMenuInfo_Original;
BOOL WINAPI SetMenuInfo_Hook(HMENU hMenu, LPCMENUINFO pInfo) {
    if (pInfo && pInfo->cbSize == sizeof(MENUINFO) && (pInfo->fMask & MIM_BACKGROUND) &&
        pInfo->hbrBack && pInfo->hbrBack != g_hMenuBrush.load(std::memory_order_acquire) &&
        g_darkMenusActive.load(std::memory_order_relaxed)) {
        MENUINFO replaced = *pInfo;
        replaced.hbrBack = nullptr;
        return SetMenuInfo_Original(hMenu, &replaced);
    }
    return SetMenuInfo_Original(hMenu, pInfo);
}

// Every SetMenuItemInfo call in the process comes through these two. One that carries no
// column break, or that comes while no menu is held, goes straight on.
template <typename Info>
inline bool CarriesColumnBreak(const Info* pInfo) {
    return pInfo && (pInfo->fMask & (MIIM_FTYPE | MIIM_TYPE)) && (pInfo->fType & kColumnBreaks);
}

BOOL WINAPI SetMenuItemInfoW_Hook(HMENU hMenu, UINT item, BOOL byPosition, LPCMENUITEMINFOW pInfo) {
    BOOL result;
    if (g_heldMenuCount.load(std::memory_order_relaxed) > 0 && CarriesColumnBreak(pInfo) &&
        SetHeldItemInfo(hMenu, item, byPosition, pInfo, SetMenuItemInfoW_Original, GetMenuItemInfoW, &result)) {
        return result;
    }
    return SetMenuItemInfoW_Original(hMenu, item, byPosition, pInfo);
}

BOOL WINAPI SetMenuItemInfoA_Hook(HMENU hMenu, UINT item, BOOL byPosition, LPCMENUITEMINFOA pInfo) {
    BOOL result;
    if (g_heldMenuCount.load(std::memory_order_relaxed) > 0 && CarriesColumnBreak(pInfo) &&
        SetHeldItemInfo(hMenu, item, byPosition, pInfo, SetMenuItemInfoA_Original, GetMenuItemInfoA, &result)) {
        return result;
    }
    return SetMenuItemInfoA_Original(hMenu, item, byPosition, pInfo);
}

// ---------------------------------------------------------------------------
// Windhawk events
// ---------------------------------------------------------------------------

void Wh_ModSettingsChanged() {
    LoadSettings();
    ApplyDarkMenus();

    // A menu still held would otherwise stay one column until it next closes, so the
    // switch would take a second open to show.
    if (!g_scrollColumnMenus.load(std::memory_order_relaxed) ||
        !g_darkMenusActive.load(std::memory_order_relaxed)) {
        ReleaseAllColumnMenus();
    }

    RedrawMenuBars();
    SyncTitleBars();
}

BOOL Wh_ModInit() {
    // No header or import library has it, so it is looked up. See ProcessIsExiting.
    g_pRtlDllShutdownInProgress = (RtlDllShutdownInProgress_t)(void*)GetProcAddress(
        GetModuleHandleW(L"ntdll.dll"), "RtlDllShutdownInProgress");

    LoadSettings();

    // A failure here only costs part of the result, since the dark theme below needs no
    // hook at all.
    if (!WindhawkUtils::SetFunctionHook(TrackPopupMenu, TrackPopupMenu_Hook, &TrackPopupMenu_Original)) Wh_Log(L"Failed to hook TrackPopupMenu");
    if (!WindhawkUtils::SetFunctionHook(TrackPopupMenuEx, TrackPopupMenuEx_Hook, &TrackPopupMenuEx_Original)) Wh_Log(L"Failed to hook TrackPopupMenuEx");
    if (!WindhawkUtils::SetFunctionHook(GetSysColor, GetSysColor_Hook, &GetSysColor_Original)) Wh_Log(L"Failed to hook GetSysColor");
    if (!WindhawkUtils::SetFunctionHook(GetSysColorBrush, GetSysColorBrush_Hook, &GetSysColorBrush_Original)) Wh_Log(L"Failed to hook GetSysColorBrush");
    if (!WindhawkUtils::SetFunctionHook(FillRect, FillRect_Hook, &FillRect_Original)) Wh_Log(L"Failed to hook FillRect");
    if (!WindhawkUtils::SetFunctionHook(PatBlt, PatBlt_Hook, &PatBlt_Original)) Wh_Log(L"Failed to hook PatBlt");
    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original)) Wh_Log(L"Failed to hook CreateWindowExW");
    if (!WindhawkUtils::SetFunctionHook(DefWindowProcW, DefWindowProcW_Hook, &DefWindowProcW_Original)) Wh_Log(L"Failed to hook DefWindowProcW");
    if (!WindhawkUtils::SetFunctionHook(DefWindowProcA, DefWindowProcA_Hook, &DefWindowProcA_Original)) Wh_Log(L"Failed to hook DefWindowProcA");
    if (!WindhawkUtils::SetFunctionHook(DefFrameProcW, DefFrameProcW_Hook, &DefFrameProcW_Original)) Wh_Log(L"Failed to hook DefFrameProcW");
    if (!WindhawkUtils::SetFunctionHook(DefFrameProcA, DefFrameProcA_Hook, &DefFrameProcA_Original)) Wh_Log(L"Failed to hook DefFrameProcA");
    if (!WindhawkUtils::SetFunctionHook(SetMenuInfo, SetMenuInfo_Hook, &SetMenuInfo_Original)) Wh_Log(L"Failed to hook SetMenuInfo");
    if (!WindhawkUtils::SetFunctionHook(SetMenuItemInfoW, SetMenuItemInfoW_Hook, &SetMenuItemInfoW_Original)) Wh_Log(L"Failed to hook SetMenuItemInfoW");
    if (!WindhawkUtils::SetFunctionHook(SetMenuItemInfoA, SetMenuItemInfoA_Hook, &SetMenuItemInfoA_Original)) Wh_Log(L"Failed to hook SetMenuItemInfoA");

    ResolveDarkMenuApis();
    ApplyDarkMenus();

    return TRUE;
}

// Hooks the threads of an app that is already running. At process start there is nothing
// to find yet, and EnsureThreadHooks picks the threads up from their first paint. This
// waits for Wh_ModAfterInit because the function hooks are not live until Wh_ModInit
// returns, which is also why the menu bars are repainted here.
void Wh_ModAfterInit() {
    EnumWindows([](HWND hWnd, LPARAM) WINAPI -> BOOL {
        DWORD pid = 0;
        DWORD tid = GetWindowThreadProcessId(hWnd, &pid);
        if (tid && pid == GetCurrentProcessId()) InstallThreadHook(tid);
        return TRUE;
    }, 0);

    RedrawMenuBars();
    SyncTitleBars();
}

void Wh_ModUninit() {
    // Before anything is taken down, so that nothing new goes up behind it.
    g_unloading.store(true, std::memory_order_relaxed);

    // Back to whatever mode the process was in, with no restart needed.
    g_themeWindowsMenus.store(false, std::memory_order_relaxed);
    ApplyDarkMenus();
    RedrawMenuBars();
    if (g_hUxtheme) FreeLibrary(g_hUxtheme);

    // Switched off first, or a window repainting in the meantime would come back through
    // ApplyTitleBar and be left dark.
    g_themeTitleBars.store(false, std::memory_order_relaxed);
    RestoreTitleBars();

    // See HookedThread.
    {
        std::vector<HookedThread> threads;
        {
            std::lock_guard<std::mutex> lock(g_hookedThreadsMutex);
            for (const auto& entry : g_hookedThreads) threads.push_back(entry.second);
            g_hookedThreads.clear();
        }
        for (const HookedThread& thread : threads) {
            RemoveThreadHooks(thread);
            if (thread.hThread) CloseHandle(thread.hThread);
        }
    }

    // Not WindhawkUtils::RemoveAllWindowSubclasses(), which needs Windhawk 1.8.
    {
        std::vector<HWND> popups;
        {
            std::lock_guard<std::mutex> lock(g_subclassedPopupsMutex);
            popups.assign(g_subclassedPopups.begin(), g_subclassedPopups.end());
            g_subclassedPopups.clear();
        }
        for (HWND hWnd : popups) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, MenuPopupSubclassProc);
        }
    }

    {
        std::vector<HWND> lists;
        {
            std::lock_guard<std::mutex> lock(g_subclassedListsMutex);
            lists.assign(g_subclassedLists.begin(), g_subclassedLists.end());
            g_subclassedLists.clear();
        }
        for (HWND hWnd : lists) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, ComboListSubclassProc);
        }
    }

    // Undo the background brush stamped on the app's menus, and hand back any column
    // breaks still held.
    RestoreStampedMenus();
    ReleaseAllColumnMenus();

    // The brushes are deliberately not deleted. Two paths hand them to the app and neither
    // can be reclaimed: MENUINFO::hbrBack on menus that outlive the mod, and GetSysColorBrush
    // results, which callers may cache for the life of the process. Deleting them would
    // leave the app painting with a freed, possibly recycled, GDI handle. A new brush is
    // only made when a color setting changes, so the count stays small.
}

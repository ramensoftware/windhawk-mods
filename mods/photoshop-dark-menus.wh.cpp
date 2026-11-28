// ==WindhawkMod==
// @id            photoshop-dark-menus
// @name          Photoshop Dark Menus
// @description   Enables dark mode and custom colors for the menus, dropdown lists, message boxes and dialog title bars in Adobe Photoshop.
// @version       1.3.0
// @author        Saber Naeemi
// @github        https://github.com/sabergraphics
// @twitter       https://x.com/SaberNaeemi
// @homepage      https://www.sabernaeemi.com
// @include       Photoshop.exe
// @compilerOptions -luser32 -lgdi32 -lcomctl32 -ldwmapi -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Photoshop Dark Menus

This Windhawk mod enables dark menus (top-bar dropdowns and context menus),
dark dropdown lists, dark message boxes and dark dialog title bars in Adobe
Photoshop on Windows, with fully customizable colors. No other mod is needed
alongside it.

Everything happens inside the Photoshop process. No system colors are changed
and no other application is affected.

## Photoshop Dark Menus or Adobe Dark Menus

[Adobe Dark Menus](https://windhawk.net/mods/adobe-dark-menus) is a newer mod
from the same author. It also covers Illustrator, InDesign, After Effects,
Premiere Pro and other Adobe apps, along with their menu bars. In Photoshop it
does what this mod does, except that it leaves message boxes light.

- **Photoshop Dark Menus** (this mod) changes Photoshop and nothing else. Pick
  it if Photoshop is the only Adobe app you want changed, or if you want its
  message boxes dark as well.
- **Adobe Dark Menus** handles all the Adobe apps in one mod. Pick it if you
  want a single mod for all of them.

**Use one or the other, not both.** Both hook the same functions inside
Photoshop, and running the two together has not been tested. If you switch to
Adobe Dark Menus, disable this mod.

## Screenshots

![Top Bar Menu Dropdown](https://raw.githubusercontent.com/sabergraphics/photoshop-dark-menus/main/images/photoshop-dark-menu-screenshot-1.png)

![Context Menu](https://raw.githubusercontent.com/sabergraphics/photoshop-dark-menus/main/images/photoshop-dark-menu-screenshot-2.png)

## Options

- **Menu Background Color**: Background color for all menu popups (Default: `#2D2D2D`).
- **Menu Text Color**: Text color for active items (Default: `#DCDCDC`).
- **Highlight Background Color**: Color when hovering over an item (Default: `#505050`).
- **Highlight Text Color**: Text color when hovering over an item (Default: `#FFFFFF`).
- **Separator Line Color**: Color for separator lines. Set to match the background color to hide them completely (Default: `#383838`).
- **Disabled Text Color**: Text color for disabled menu items (Default: `#808080`).
- **Border Color**: Color of the popup frame. Leave empty to keep the system frame (Default: `#2D2D2D`).
- **Theme Dropdown Lists**: Darken the dropdown lists as well, using the colors above (Default: on).
- **Darken Menus Windows Draws**: Use Windows' dark menu theme for the few menus Windows draws itself, such as the context menu on a document tab (Default: on).
- **Darken Message Boxes**: Darken Photoshop's message boxes, such as the "Save changes" prompt, using the colors above (Default: on).
- **Dark Title Bars**: Color the title bars of Photoshop's dialogs to match the menus (Default: on).
- **Title Bar Color**: Leave empty to match the menu background color (Default: empty).
- **Title Bar Text Color**: Leave empty to let Windows choose, which dims the text of an inactive window (Default: empty).

Colors can be given as `#RRGGBB` or the short `#RGB` form.

## Relationship to the `dark-menus` mod

Earlier versions recommended running the system-wide
[Dark mode context menus](https://windhawk.net/mods/dark-menus) (`dark-menus`)
mod alongside this one, for the few menus in Photoshop that Windows draws. This
mod now darkens those itself, the same way `dark-menus` does, so it is no
longer needed for Photoshop.

If you run `dark-menus` for your other apps, it can stay on. It can also
rewrite the menu background this mod sets, though, so if Photoshop's menus come
out in its colors rather than yours, exclude `Photoshop.exe` from it in its
Advanced settings.

## How it works

Photoshop's menus look like classic unthemed Win32 menus, but they are not:
Photoshop **owner-draws every menu item itself** through `AdobeOwl.dll`, using
plain GDI, inside its own process. Instrumenting a live Photoshop shows the
menu popups going up via `TrackPopupMenuEx` and every item being painted with:

- `FillRect(hdc, &rc, (HBRUSH)(COLOR_MENU + 1))` for the item background, and
  `(HBRUSH)(COLOR_HIGHLIGHT + 1)` for the hovered item. These **system color
  pseudo-handles** are resolved inside `user32` from shared memory, so they
  bypass `GetSysColor` and `GetSysColorBrush` hooks entirely - which is why
  earlier per-process attempts appeared to do nothing.
- `GetSysColor(COLOR_MENUTEXT / COLOR_GRAYTEXT / COLOR_HIGHLIGHTTEXT)` for text.
- Its own 1px `FillRect` for separator lines.

So every pixel of a menu item is reachable from inside the process:

- `FillRect` / `PatBlt` are hooked and, while a menu item is being painted,
  sys-color pseudo-handles are re-resolved through this mod's own color table,
  and 1px fills inside a menu DC are recolored to the separator color.
- `GetSysColor` / `GetSysColorBrush` are hooked for the menu color indices, but
  **only while a menu item is actually being painted** - that is, inside a
  `WM_DRAWITEM` of type `ODT_MENU` - so Photoshop's dialogs and panels keep the
  system colors. "While a menu is open" would be far too broad: a menu runs a
  modal message loop that keeps dispatching to every other window on the thread,
  and an unrelated repaint would pick the menu colors up.
- `MENUINFO::hbrBack` is stamped on each popup Photoshop draws, submenus
  included, at `WM_INITMENUPOPUP`, covering the popup background that the
  system paints around the owner-drawn items.
- The popup's non-client frame is the one part drawn kernel-side, from the 3D
  colors, where no user-mode hook can reach it - so the popup is subclassed at
  creation and the frame is repainted with the border color after it. Set
  **Border Color** empty to leave the system frame alone.
- The scroll arrows of a menu too long for the screen are drawn kernel-side
  too, in the system menu text color, which is close to invisible on a dark
  menu. The same subclass recolors them to the text color after each scroll.

The dropdown lists - the Layers panel blend mode list, the options bar lists,
the Glyphs panel lists - are stock Win32 combo boxes whose list is user32's own
`ComboLBox` window. Photoshop owner-draws the rows but takes their colors from
the system, and not consistently: one list reads `COLOR_WINDOWTEXT` for its row
text where another reads `COLOR_MENUTEXT`, and one fills its rows from a color it
asks for where another fills them with a `(HBRUSH)(COLOR_X + 1)` pseudo-handle.
Both routes are therefore covered, over a table that maps every background index
to the background color and every text index to the text color. The list window
is subclassed so that all of it applies only while that list is painting, which
leaves every other control reading those same colors alone. Only lists that
Photoshop draws itself and that drop down are themed: a stock list, as some
plugin dialogs use, keeps the system colors, since Windows fills its background
from a brush no hook sees.

### Menus Windows draws

A few menus in Photoshop are left to Windows, such as the context menu on a
document tab or a panel tab. Windows paints those through its theme engine, so
none of the above reaches them. The theme engine has a dark variant of that
drawing, which is off unless a process asks for it. The mod asks for it with
`SetPreferredAppMode(ForceDark)` and `FlushMenuThemes`, the technique the
`dark-menus` mod uses, and puts the process back when it is disabled. This needs
Windows 10 1903 or later.

### Message boxes

Photoshop's alerts, such as the prompt to save changes before closing, are its
own `PSDialogBox` window holding only stock Windows controls: an icon, a line
of text and push buttons. Photoshop paints the white background itself, and
it builds the box while the box is already showing. So the window is
subclassed, the background is painted in the menu color in place of
Photoshop's, the text is answered in the menu colors, and what the box holds
is checked again each time it paints, giving each push button Windows' own
dark button theme as it appears. Only a box that holds nothing but those
controls is changed, so any dialog Photoshop paints itself is left as it is.
The dark buttons need Windows 10 1809 or later.

### Title bars

Windows 11 lets a process color a standard title bar through
`DwmSetWindowAttribute`, so nothing is drawn here: the mod sets
`DWMWA_USE_IMMERSIVE_DARK_MODE` and `DWMWA_CAPTION_COLOR` on each Photoshop
window with a standard title bar, which in practice means its dialogs, and puts
them back when it is disabled. On Windows 10 2004 or later the dark flag still
gives a dark title bar, without the custom color. Photoshop's main window draws
its own title bar and is left alone.

## Scope

The menu bar itself (File, Edit, Image, ...) is drawn by Photoshop's own UI
framework and already follows Photoshop's interface theme; this mod covers the
dropdown popups and context menus.

Menus Windows draws itself take Windows' own dark colors rather than the colors
set here, and so do the buttons in a message box.

A message box holding anything beyond an icon, text and push buttons, such as a
"Don't show again" check box, stays light.

The font browser and the Glyphs panel's character cells stay light too, and are
outside what this mod can reach: Photoshop paints those from its own palette
rather than from any system color, mixing its dark chrome and their light
content in the same window with the same calls. Recoloring them would mean
matching color values rather than intercepting a system one, in a window class
Photoshop also uses for its swatches and color pickers - where a light fill is
the content.

## Source and bug reports

The mod is developed at
[github.com/sabergraphics/photoshop-dark-menus](https://github.com/sabergraphics/photoshop-dark-menus).

Report a problem in
[Issues](https://github.com/sabergraphics/photoshop-dark-menus/issues). Menu
drawing depends on the environment more than most things do, so please say which
Photoshop and Windows version you are on, what your display scaling is, which
menu or dropdown list is wrong, and whether the `dark-menus` mod is installed as
well. A screenshot usually says more than a description does.

Questions go in
[Discussions](https://github.com/sabergraphics/photoshop-dark-menus/discussions).

## Changelog

### 1.3.0
- Photoshop's message boxes, such as the prompt to save changes before closing
  and its warnings, are dark too, with dark buttons. Turn it off with
  **Darken Message Boxes**.

### 1.2.0
- The menus Windows draws itself in Photoshop, such as the context menu on a
  document tab or a panel tab, are dark too, so the Dark mode context menus mod
  is no longer needed alongside this one. Turn it off with **Darken Menus
  Windows Draws**.
- Dialog title bars are dark, in a color of your choice. Turn it off with
  **Dark Title Bars**.
- The scroll arrows of long menus are drawn in the menu text color instead of
  black, which was close to invisible on a dark menu, and the border no longer
  grows thicker above and below them.
- Fixed a possible crash when Photoshop closes.
- Fixed a case where dropdown list colors could stay on after the list was gone.
- Dropdown lists that Photoshop does not draw itself, as in some plugin dialogs,
  keep their normal colors instead of getting light text on a light background.
- Only menus Photoshop draws itself get the custom background, so a menu Windows
  draws keeps the look of its theme.
- Safer when the mod is turned off or updated while Photoshop is running.

### 1.1.0
- Dropdown lists are themed as well, using the same colors - the Layers panel
  blend mode list, the options bar lists, the Glyphs panel lists and the other
  combo boxes. Turn it off with **Theme Dropdown Lists**.
- The color overrides now apply only while a menu item or a dropdown row is
  actually being painted, rather than for as long as a menu is open. A menu runs
  a modal message loop that keeps dispatching to every other window on its
  thread, so an unrelated repaint could previously come out in menu colors.
- Menus destroyed over a long session no longer fill the tracked-menu cap and
  leave later menus unthemed.
- Changing one color no longer recreates all seven brushes.
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
    Color of the popup frame, which the system draws from the 3D colors. Leave
    empty to keep the system frame.
- ThemeDropdowns: true
  $name: "Theme Dropdown Lists"
  $description: >-
    Also darken the dropdown lists, such as the Layers panel blend mode list,
    using the colors above.
- ThemeWindowsMenus: true
  $name: "Darken Menus Windows Draws"
  $description: >-
    Use Windows' dark menu theme for the few menus in Photoshop that Windows
    draws itself, such as the context menu on a document tab or a panel tab.
- DarkMessageBoxes: true
  $name: "Darken Message Boxes"
  $description: >-
    Darken Photoshop's message boxes, such as the "Save changes" prompt and its
    warnings, using the colors above. Its other dialogs are dark already.
- ThemeTitleBars: true
  $name: "Dark Title Bars"
  $description: >-
    Color the title bars of Photoshop's dialogs to match the menus. The main
    window draws its own title bar and is left alone.
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
#include <uxtheme.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <wchar.h>
#include <wctype.h>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

// Colors read by the in-process hooks. Each one doubles as the cache that keeps
// a settings change from recreating a brush whose color did not move; they hold
// the defaults rather than a sentinel so that a color is always a real color,
// even if a brush ever fails to be created. The border is the exception -
// CLR_INVALID is how "leave the system frame alone" is spelled.
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
std::atomic<bool> g_themeTitleBars{true};
std::atomic<bool> g_darkMessageBoxes{true};

// Whether the process is actually in dark menu mode. Stays false with the
// setting on before Windows 10 1903.
std::atomic<bool> g_darkMenusActive{false};

// Brushes are published with an atomic store and never deleted - see the note
// in Wh_ModUninit.
std::atomic<HBRUSH> g_hMenuBrush{nullptr};
std::atomic<HBRUSH> g_hTextBrush{nullptr};
std::atomic<HBRUSH> g_hHighlightBrush{nullptr};
std::atomic<HBRUSH> g_hHiTextBrush{nullptr};
std::atomic<HBRUSH> g_hGrayBrush{nullptr};
std::atomic<HBRUSH> g_hSeparatorBrush{nullptr};
std::atomic<HBRUSH> g_hBorderBrush{nullptr};

// Number of menu tracking calls currently on this thread's stack. This is what
// decides whether the per-menu hooks are needed, not whether a color should be
// overridden - a menu keeps a modal message loop running that dispatches to
// every other window on the thread, so "a menu is open" says nothing about what
// is being painted right now.
thread_local int tl_menuDepth = 0;

// The two contexts that do decide it. Instrumenting Photoshop shows every menu
// item pixel - background, text and separators alike - painted inside a
// WM_DRAWITEM whose CtlType is ODT_MENU, and the whole of a dropdown list
// painted inside the ComboLBox window's own message handling. Both are narrow,
// both are thread-local, and outside them the hooks are a single TLS read.
thread_local int tl_menuDrawDepth = 0;
thread_local int tl_comboListDepth = 0;

// Raised while this thread is painting on the mod's behalf, so our own drawing
// does not re-enter the hooks.
thread_local int tl_ourPaintDepth = 0;

// Raises one of the counters above for a scope. A counter left raised is the
// one failure here that is both permanent and impossible to miss - the thread
// paints everything in the mod's colors until Photoshop restarts - so they are
// closed by a destructor rather than by a statement an unwind could skip.
struct DepthGuard {
    explicit DepthGuard(int* pDepth) : pDepth(pDepth) { (*pDepth)++; }
    ~DepthGuard() { if (*pDepth > 0) (*pDepth)--; }

    DepthGuard(const DepthGuard&) = delete;
    DepthGuard& operator=(const DepthGuard&) = delete;

private:
    int* pDepth;
};

// What the opening hook did for each WM_DRAWITEM now on this thread's stack,
// innermost last, so the closing hook undoes exactly that. Working it out again
// from the message goes wrong for a list destroyed mid-draw: it no longer looks
// like a list by then, and nothing else would lower its counter. Windows calls
// the two hooks in strict pairs, also for a window destroyed while it handles
// the message.
enum class DrawKind : unsigned char { Other, Menu, List };
constexpr int kMaxNestedDraws = 16;
thread_local DrawKind tl_drawKinds[kMaxNestedDraws];
thread_local int tl_drawNesting = 0;

// Windhawk does not call Wh_ModUninit when Photoshop exits, so the thread hooks
// and the subclasses are still up while the loader destroys this module's
// globals, and Photoshop's own DLLs are torn down after that. A window one of
// them destroys then comes back through a subclass into a container that no
// longer exists. So once the process is exiting, nothing here touches the
// containers.
using RtlDllShutdownInProgress_t = BOOLEAN(NTAPI*)();
RtlDllShutdownInProgress_t g_pRtlDllShutdownInProgress = nullptr;

inline bool ProcessIsExiting() {
    return g_pRtlDllShutdownInProgress && g_pRtlDllShutdownInProgress();
}

// Set at the start of Wh_ModUninit. A thread that is inside the mod at that
// moment can be about to put up a hook or a subclass, and once uninit has
// emptied its lists nothing would take that one down again. So whoever
// registers one checks this first, and once more under the lock of the list it
// is about to add to.
std::atomic<bool> g_unloading{false};

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

// Returns false for an empty string, so a setting can mean "leave it alone".
bool ParseHexColor(PCWSTR hexStr, COLORREF* pOut) {
    if (!hexStr) return false;

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

// The brush being replaced is not deleted, and is not remembered either: there
// is nothing that could ever be done with it. See the note in Wh_ModUninit.
void PublishBrush(std::atomic<HBRUSH>* pSlot, HBRUSH hNew) {
    pSlot->store(hNew, std::memory_order_release);
}

// A replaced brush is never deleted, so a settings change that recreated all
// seven would leak six of them for nothing. Only the colors that actually moved
// get a new brush.
void PublishColor(std::atomic<COLORREF>* pColor, std::atomic<HBRUSH>* pSlot, COLORREF color) {
    if (pColor->load(std::memory_order_relaxed) == color &&
        pSlot->load(std::memory_order_acquire)) {
        return;
    }

    HBRUSH hNew = CreateSolidBrush(color);
    if (!hNew) return;

    pColor->store(color, std::memory_order_relaxed);
    PublishBrush(pSlot, hNew);
}

void LoadSettings() {
    g_themeDropdowns.store(Wh_GetIntSetting(L"ThemeDropdowns") != 0, std::memory_order_relaxed);
    g_themeWindowsMenus.store(Wh_GetIntSetting(L"ThemeWindowsMenus") != 0, std::memory_order_relaxed);
    g_themeTitleBars.store(Wh_GetIntSetting(L"ThemeTitleBars") != 0, std::memory_order_relaxed);
    g_darkMessageBoxes.store(Wh_GetIntSetting(L"DarkMessageBoxes") != 0, std::memory_order_relaxed);

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
        PublishBrush(&g_hBorderBrush, nullptr);
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

// Windows the mod has colored. The value is whether the mod turned the dark
// flag on, so a window that was already dark stays dark when this is undone.
std::unordered_map<HWND, bool> g_titledWindows;
std::mutex g_titledWindowsMutex;

// Photoshop's main window draws its own title bar, extending its client area
// over that space, which leaves nothing for DWM to color. Its dialogs have
// ordinary title bars, so this is measured per window. The side frame is
// subtracted because a maximized window's frame hangs off the screen on all
// sides.
bool HasSystemTitleBar(HWND hWnd) {
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if ((style & WS_CAPTION) != WS_CAPTION || (style & WS_CHILD)) return false;

    // Both rectangles come back in screen coordinates. ClientToScreen would not
    // do: on a mirrored window client x runs right to left, so its origin is the
    // right edge.
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

// On Windows 10 the two color calls fail harmlessly, and the dark flag alone
// still gives a dark title bar.
void ApplyTitleBar(HWND hWnd) {
    if (!g_themeTitleBars.load(std::memory_order_relaxed) || ProcessIsExiting() ||
        g_unloading.load(std::memory_order_relaxed) || !HasSystemTitleBar(hWnd)) {
        return;
    }

    // Recorded before the calls below, since a frame paint can come back
    // through here.
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

// The two window classes the mod knows about, both created by user32 for
// itself: the menu popup, and the list half of a combo box.
constexpr PCWSTR kMenuPopupClass = L"#32768";
constexpr PCWSTR kComboListClass = L"ComboLBox";

// Photoshop paints menu items into a popup-sized memory DC while the popup is
// being laid out, and directly onto the popup's own window DC when an item is
// re-drawn on hover. The memory DC fallback would match any memory DC, so it
// leans on the caller having raised tl_menuDrawDepth or tl_comboListDepth
// first: inside a menu's WM_DRAWITEM, or inside a list's own message handling,
// this thread is painting that item and nothing else.
bool IsPaintContext(HDC hdc, PCWSTR windowClass) {
    HWND hWnd = WindowFromDC(hdc);
    if (hWnd) {
        WCHAR cls[16];
        return GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) && wcscmp(cls, windowClass) == 0;
    }
    return GetObjectType(hdc) == OBJ_MEMDC;
}

// Our color for a system color index, or nullptr / false if we don't override it.
//
// The frame's 3D colors (COLOR_BTNSHADOW and friends) are deliberately not
// listed: the popup frame is drawn kernel-side and never reaches a user-mode
// GDI call, so overriding them here would change nothing - it is repainted in
// the popup's subclass instead.
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

// The dropdown equivalents. Which index a list reaches for is not consistent:
// the Layers blend list takes its row text from COLOR_WINDOWTEXT, while the
// options bar's list takes it from COLOR_MENUTEXT - so rather than track down
// which control uses which, every background index maps to the background and
// every text index to the text. The mapping only applies while a list is
// painting, so a generous table costs nothing elsewhere.
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

// The brush half of the table above, and a mirror of it: a list is free to fill
// from either route, so an index answered by one and not the other is an index
// that comes out themed or system-colored depending on which call Photoshop
// happened to make.
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

// A background brush set on a menu is state on the menu object, not on a hook,
// so it outlives the mod: without clearing it, disabling the mod leaves every
// area the system fills from hbrBack - gaps, margins, separator bands - dark
// until Photoshop restarts. Remember what we stamped so it can be undone.
//
// Photoshop keeps a bounded set of menus, but they can be rebuilt over a long
// session, so the set is capped rather than allowed to grow without limit.
constexpr size_t kMaxTrackedMenus = 1024;
std::unordered_set<HMENU> g_stampedMenus;
std::mutex g_stampedMenusMutex;

// Whether Photoshop paints this menu or Windows does. Going through
// TrackPopupMenu says nothing about it.
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

    // The cap is checked before stamping, not after: a menu that is stamped but
    // not recorded would stay dark for the life of the process, which is the one
    // thing the tracking exists to prevent.
    {
        std::lock_guard<std::mutex> lock(g_stampedMenusMutex);
        if (g_stampedMenus.size() >= kMaxTrackedMenus && !g_stampedMenus.contains(hMenu)) {
            // Nothing removes an entry when Photoshop destroys a menu, so a full
            // set is mostly dead handles. Drop those before giving up on this
            // menu: refusing to stamp puts themed items on an unthemed popup
            // background, which is more visible than the untracked stamp the cap
            // exists to prevent.
            std::erase_if(g_stampedMenus, [](HMENU hDead) { return !IsMenu(hDead); });
            if (g_stampedMenus.size() >= kMaxTrackedMenus) return;
        }
    }

    MENUINFO mi = {};
    mi.cbSize = sizeof(mi);
    mi.fMask = MIM_BACKGROUND;
    mi.hbrBack = hBrush;
    if (!SetMenuInfo(hMenu, &mi)) return;

    // SetMenuInfo can reach a menu owned by another thread, so it is called
    // outside the lock; two threads racing past the check can leave the set one
    // entry over the cap, which is harmless.
    std::lock_guard<std::mutex> lock(g_stampedMenusMutex);
    g_stampedMenus.insert(hMenu);
}

// Far deeper than any real menu nests. It only stops a walk that would never end.
constexpr int kMaxMenuDepth = 25;

// Each menu of the tree is judged on its own. MIM_APPLYTOSUBMENUS would cover
// the tree in one call, but it would also stamp a submenu Windows draws below a
// menu Photoshop draws. On a menu Windows draws, the brush replaces only the
// background, while the gutter, separators and text still come from the theme.
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

    // SetMenuInfo can reach a menu owned by another thread, so it is called
    // with the lock released.
    for (HMENU hMenu : menus) {
        MENUINFO mi = {};
        mi.cbSize = sizeof(mi);
        mi.fMask = MIM_BACKGROUND;
        mi.hbrBack = nullptr;
        SetMenuInfo(hMenu, &mi);
    }
}

// Width of the non-client frame, i.e. how much of the window rect the system
// paints from the 3D colors.
int GetPopupFrameWidth(HWND hWnd, const RECT& windowRect) {
    RECT client;
    POINT origin = { 0, 0 };
    if (!GetClientRect(hWnd, &client) || !ClientToScreen(hWnd, &origin)) return 1;

    // The sides only: above and below the items, the non-client area of a long
    // menu also holds the scroll arrows.
    int left = origin.x - windowRect.left;
    int right = windowRect.right - (origin.x + (client.right - client.left));
    int frame = left > right ? left : right;

    if (frame < 1) frame = 1;
    if (frame > 4) frame = 4;
    return frame;
}

// The popup's non-client frame is drawn by the system from COLOR_3DDKSHADOW /
// COLOR_BTNHIGHLIGHT, which no process-local hook can change - so repaint it
// once the system is done with it.
//
// hdcTarget is the DC the system rendered the popup into. Menus are drawn
// offscreen and blitted (WM_PRINT / WM_NCUAHDRAWFRAME carry that DC in wParam),
// so painting on the window DC instead would just be overwritten by the blit.
void PaintPopupBorder(HWND hWnd, HDC hdcTarget) {
    // Popups are subclassed unconditionally and the system reuses them, so a
    // popup can be shown again later by a menu this mod doesn't theme - one that
    // never went through TrackPopupMenu(Ex). Painting only while this thread is
    // tracking a menu keeps the dark frame off an otherwise untouched menu.
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

// The kernel draws the scroll arrows of a menu too long for the screen in the
// popup's non-client area, with no user-mode call to hook. On a menu with a
// background brush, which is every menu this mod stamps, it draws a bare
// triangle in COLOR_MENUTEXT when enabled and an embossed COLOR_BTNSHADOW and
// COLOR_BTNHIGHLIGHT one when disabled. Black is close to invisible on a dark
// menu, so both are recolored once the kernel is done.
//
// The recoloring must not undo itself, because a repaint can arrive with no new
// arrow behind it. It matches exact system colors, which are gone once it has
// run.
bool MenuHasBackgroundBrush(HMENU hMenu) {
    MENUINFO mi = {};
    mi.cbSize = sizeof(mi);
    mi.fMask = MIM_BACKGROUND;
    return hMenu && GetMenuInfo(hMenu, &mi) && mi.hbrBack;
}

inline DWORD ToDibColor(COLORREF color) {
    return ((DWORD)GetRValue(color) << 16) | ((DWORD)GetGValue(color) << 8) | GetBValue(color);
}

// Edits the band in place and copies back only what changed. A system color
// that matches the band's own background is left out, since it can't be told
// from it.
void RecolorArrow(HDC hdc, HDC hdcBand, DWORD* pixels, int left, int top, int width, int height) {
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

    // With a text color that happens to equal COLOR_BTNSHADOW, an enabled arrow
    // that has already been recolored looks like a disabled one. Only a disabled
    // arrow the kernel has just drawn still has its highlight emboss.
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

void PaintOverArrow(HDC hdc, int left, int top, int width, int height) {
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

    RecolorArrow(hdc, hdcBand, (DWORD*)bits, left, top, width, height);

    SelectObject(hdcBand, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcBand);
}

// A couple of window manager calls on a menu with no arrows, about a
// millisecond on one that has them.
void PaintOverScrollArrows(HWND hWnd, HDC hdcTarget) {
    // The same rule as the border: the popup may be showing a menu the mod
    // leaves alone.
    if (!MenuIsOpen()) return;

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

    // Only a menu Photoshop draws and the mod has stamped. On any other the
    // arrow is a different shape, and a light one is the right one.
    HMENU hMenu = (HMENU)SendMessageW(hWnd, MN_GETHMENU, 0, 0);
    if (!hMenu || !MenuHasOwnerDrawnItems(hMenu) || !MenuHasBackgroundBrush(hMenu)) return;

    HDC hdc = hdcTarget ? hdcTarget : GetWindowDC(hWnd);
    if (!hdc) return;

    {
        DepthGuard guard(&tl_ourPaintDepth);
        int width = window.right - window.left - frame * 2;
        int windowHeight = window.bottom - window.top;
        int clientBottom = clientTop + client.bottom;
        PaintOverArrow(hdc, frame, frame, width, clientTop - frame);
        PaintOverArrow(hdc, frame, clientBottom, width, windowHeight - frame - clientBottom);
    }

    if (!hdcTarget) ReleaseDC(hWnd, hdc);
}

// Undocumented: sent to a window to paint its frame into the DC in wParam.
constexpr UINT WM_NCUAHDRAWFRAME = 0x00AF;

// One class lookup answers both questions. The CBT hook asks about every window
// Photoshop creates, so a window it cares about nothing for should cost a single
// call into the window manager rather than one per class.
enum class WindowKind { Other, MenuPopup, ComboList };

WindowKind ClassifyWindow(HWND hWnd) {
    WCHAR cls[16];
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) return WindowKind::Other;
    if (wcscmp(cls, kMenuPopupClass) == 0) return WindowKind::MenuPopup;
    if (wcscmp(cls, kComboListClass) == 0) return WindowKind::ComboList;
    return WindowKind::Other;
}

// The system reuses menu popup windows, so a subclassed popup can still be
// alive when the mod unloads - and its window procedure would then point into
// an unloaded DLL. Track them and remove each subclass at uninit.
std::unordered_set<HWND> g_subclassedPopups;
std::mutex g_subclassedPopupsMutex;

// MN_SELECTITEM, undocumented: moves the selection in a menu popup. Its wParam
// is an item position, or one of these two while the pointer is over a scroll
// arrow.
constexpr UINT kMenuSelectItem = 0x01E5;
constexpr UINT kMenuUpArrow = 0xFFFFFFFD;
constexpr UINT kMenuDownArrow = 0xFFFFFFFC;

// The popup whose scroll arrow the pointer is on, if any. The kernel redraws an
// arrow when the pointer moves onto it and again when it moves off, with no
// paint message either time.
thread_local HWND tl_arrowPopup = nullptr;

// Subclassing the popup rather than watching messages from a hook: WM_PAINT is
// posted, not sent, so a WH_CALLWNDPROCRET hook never sees it, and a popup that
// repaints that way would keep the system frame.
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

    // A scroll redraws the arrows and then repaints the rows, and it is the rows
    // that take the time. Painted over only afterward, the arrows would stay as
    // the kernel drew them for the whole repaint, which is long enough to be seen.
    if (uMsg == WM_PAINT) PaintOverScrollArrows(hWnd, nullptr);

    LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

    switch (uMsg) {
        // wParam carries the DC the popup was rendered into. A caller asking for
        // the client area alone is not asking for a frame, and drawing one anyway
        // would land it over the items - the system's own render passes both
        // flags, so this costs nothing on the path the border relies on.
        case WM_PRINT:
            if (!(lParam & PRF_NONCLIENT)) break;
            [[fallthrough]];
        case WM_NCUAHDRAWFRAME:
            PaintPopupBorder(hWnd, (HDC)wParam);
            break;

        // Painted on the window's own DC. WM_ERASEBKGND's DC is client
        // relative, so the frame goes on the window DC in every one of these.
        case WM_PAINT:
        case WM_NCPAINT:
        case WM_ERASEBKGND:
            PaintPopupBorder(hWnd, nullptr);
            break;
    }

    // The arrows have no paint message of their own: the kernel redraws one in
    // the middle of a scroll. WM_PAINT follows any scroll, and the rest cover the
    // routes that run through this window.
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

        // Sent on every mouse move, and it only leaves an arrow redrawn when the
        // pointer has moved onto one or off one. The rest are let through, so a
        // long menu is not read back for every move of the mouse.
        case kMenuSelectItem: {
            bool onArrow = (UINT)wParam == kMenuUpArrow || (UINT)wParam == kMenuDownArrow;
            HWND hLeft = tl_arrowPopup;
            tl_arrowPopup = onArrow ? hWnd : nullptr;

            if (onArrow) PaintOverScrollArrows(hWnd, nullptr);

            // The pointer may have left for another popup, and a remembered
            // handle is only as good as the day it was stored.
            if (hLeft && (hLeft != hWnd || !onArrow) && ClassifyWindow(hLeft) == WindowKind::MenuPopup) {
                PaintOverScrollArrows(hLeft, nullptr);
            }
            break;
        }
    }

    return result;
}

// Records a window that has just been subclassed. Returns false once
// Wh_ModUninit has begun: it may already have emptied the set and would never
// come back for this window, so the caller takes the subclass off again.
bool RecordSubclassed(std::unordered_set<HWND>& windows, std::mutex& mutex, HWND hWnd) {
    std::lock_guard<std::mutex> lock(mutex);
    if (g_unloading.load(std::memory_order_relaxed)) return false;

    windows.insert(hWnd);
    return true;
}

// Subclassed even when Border Color is empty, and regardless of whether a menu
// is up: the system reuses menu popup windows, so a popup passed over here is
// never revisited, and setting a border color later would silently do nothing
// for the rest of the session. PaintPopupBorder returns immediately when there
// is no border brush.
void SubclassPopup(HWND hWnd) {
    if (ProcessIsExiting() || g_unloading.load(std::memory_order_relaxed)) return;

    {
        std::lock_guard<std::mutex> lock(g_subclassedPopupsMutex);
        if (g_subclassedPopups.contains(hWnd)) return;
    }

    // Not subclassed under the lock: from another thread the helper gets there
    // with a SendMessage, and the subclass procedure takes the same lock on
    // WM_NCDESTROY.
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

// Only a list whose rows Photoshop paints can be themed. Windows fills a stock
// list from a brush no hook sees, so recoloring its text would leave light text
// on white.
bool IsOwnerDrawnList(HWND hWnd) {
    return (GetWindowLongPtrW(hWnd, GWL_STYLE) & (LBS_OWNERDRAWFIXED | LBS_OWNERDRAWVARIABLE)) != 0;
}

// A dropdown's list sits directly on the desktop. The list of a CBS_SIMPLE
// combo is a child of the combo and part of the dialog around it, so it is left
// alone too.
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

// The fill behind the rows happens outside any WM_DRAWITEM, so the list's own
// paint messages are bracketed here. Only those: a selection sends
// CBN_SELCHANGE from inside the list's input handling, and whatever Photoshop
// reads or repaints in response has to see the real system colors. The rows are
// bracketed separately in CallWndProcHook, which also covers a row repainted on
// hover.
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

// Subclassed whatever the setting says, for the same reason the popups are: the
// list window is created once and reused, so a list skipped here would stay
// light for the rest of the session if the setting were turned on later.
// Dropdown or not is checked at paint time, because a list has not been moved
// to the desktop yet when the CBT hook sees it.
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

    // A list is usually found in the middle of the paint that revealed it, by
    // which point the system has already filled its background. Ask for one
    // more paint so the fill lands in the mod's color straight away rather than
    // on the next time the list is opened.
    InvalidateRect(hWnd, nullptr, TRUE);
}

void SubclassIfKnown(HWND hWnd) {
    switch (ClassifyWindow(hWnd)) {
        case WindowKind::MenuPopup: SubclassPopup(hWnd); break;
        case WindowKind::ComboList: SubclassComboList(hWnd); break;
        case WindowKind::Other:     break;
    }
}

// ---------------------------------------------------------------------------
// Message boxes
// ---------------------------------------------------------------------------

// Photoshop's alerts, such as "Save changes to the Adobe Photoshop document
// before closing?", are its own PSDialogBox window holding nothing but stock
// controls: an icon, a line of text and push buttons. Measured with alerts open.
// Photoshop paints the white background itself, in its own WM_PAINT, and it
// builds the box while the box is already showing: when the mod first saw one,
// only its first button existed. Photoshop's other dialogs are dark already,
// because it paints their controls itself.
constexpr PCWSTR kPsDialogClass = L"PSDialogBox";

// Windows' dark push button theme arrived in Windows 10 1809.
constexpr DWORD kFirstBuildWithDarkButtons = 17763;
std::atomic<DWORD> g_windowsBuild{0};

// More children than any alert has. It only bounds the walk.
constexpr int kMaxAlertChildren = 32;

enum class AlertPart { Text, Button, Other };

// A static that Windows draws, text or icon, or a push button. Anything else
// means a dialog the mod has not been measured against, a check box for one, or
// a control Photoshop paints itself.
AlertPart ClassifyAlertPart(HWND hWnd) {
    WCHAR cls[16];
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) return AlertPart::Other;

    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (_wcsicmp(cls, L"Static") == 0) {
        switch (style & SS_TYPEMASK) {
            case SS_LEFT:
            case SS_CENTER:
            case SS_RIGHT:
            case SS_ICON:
            case SS_SIMPLE:
            case SS_LEFTNOWORDWRAP:
                return AlertPart::Text;
        }
        return AlertPart::Other;
    }
    if (_wcsicmp(cls, L"Button") == 0) {
        LONG_PTR type = style & BS_TYPEMASK;
        if (type == BS_PUSHBUTTON || type == BS_DEFPUSHBUTTON) return AlertPart::Button;
    }
    return AlertPart::Other;
}

inline bool IsPsDialog(HWND hWnd) {
    WCHAR cls[16];
    return GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) && wcscmp(cls, kPsDialogClass) == 0 &&
           !(GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_CHILD);
}

bool HoldsOnlyAlertParts(HWND hWnd) {
    int buttons = 0, children = 0;
    for (HWND hChild = GetWindow(hWnd, GW_CHILD); hChild; hChild = GetWindow(hChild, GW_HWNDNEXT)) {
        if (++children > kMaxAlertChildren) return false;
        switch (ClassifyAlertPart(hChild)) {
            case AlertPart::Button: buttons++; break;
            case AlertPart::Text:   break;
            case AlertPart::Other:  return false;
        }
    }
    return buttons > 0;
}

inline bool WantDarkAlert(HWND hWnd) {
    return g_darkMessageBoxes.load(std::memory_order_relaxed) &&
           !g_unloading.load(std::memory_order_relaxed) && HoldsOnlyAlertParts(hWnd);
}

// The PSDialogBox windows the mod watches, and the buttons it has given the dark
// theme, each with the box it belongs to.
std::unordered_set<HWND> g_psDialogs;
std::unordered_map<HWND, HWND> g_darkButtons;
std::mutex g_alertsMutex;

// The buttons take Windows' own dark theme, so their colors are Windows' rather
// than the settings. Setting a theme makes a button repaint, so each one is
// themed once, and passing null puts back the theme it had.
void SyncAlertButtons(HWND hDlg, bool dark) {
    std::vector<HWND> change;
    {
        std::lock_guard<std::mutex> lock(g_alertsMutex);
        if (dark) {
            int children = 0;
            for (HWND hChild = GetWindow(hDlg, GW_CHILD); hChild && ++children <= kMaxAlertChildren;
                 hChild = GetWindow(hChild, GW_HWNDNEXT)) {
                if (ClassifyAlertPart(hChild) == AlertPart::Button &&
                    g_darkButtons.try_emplace(hChild, hDlg).second) {
                    change.push_back(hChild);
                }
            }
        } else {
            for (auto it = g_darkButtons.begin(); it != g_darkButtons.end();) {
                if (it->second == hDlg) {
                    change.push_back(it->first);
                    it = g_darkButtons.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }

    // Outside the lock, since this sends the button a message.
    for (HWND hButton : change) {
        SetWindowTheme(hButton, dark ? L"DarkMode_Explorer" : nullptr, nullptr);
        InvalidateRect(hButton, nullptr, TRUE);
    }
}

// Decided again at each paint rather than once, since the box can still be
// gaining controls after the mod first sees it.
bool PrepareAlert(HWND hDlg) {
    bool dark = WantDarkAlert(hDlg);
    SyncAlertButtons(hDlg, dark);
    return dark;
}

// Posted to a box by a button that paints before it has been themed, so that
// it is themed once that paint is over rather than in the middle of it.
UINT PrepareAlertMessage() {
    static const UINT message = RegisterWindowMessageW(L"PhotoshopDarkMenus.PrepareAlert");
    return message;
}

void FillAlert(HWND hWnd, HDC hdc, const RECT* prc, HBRUSH hBrush) {
    RECT rc;
    if (!prc) {
        if (!GetClientRect(hWnd, &rc)) return;
        prc = &rc;
    }
    DepthGuard guard(&tl_ourPaintDepth);
    FillRect(hdc, prc, hBrush);
}

// The background is answered here in full, Photoshop's own paint included, and
// the statics after Photoshop has had its say. Anything else, or a box that does
// not qualify, goes straight through.
LRESULT CALLBACK AlertSubclassProc(HWND hWnd,
                                   UINT uMsg,
                                   WPARAM wParam,
                                   LPARAM lParam,
                                   [[maybe_unused]] DWORD_PTR dwRefData) {
    HBRUSH hBrush = g_hMenuBrush.load(std::memory_order_acquire);

    switch (uMsg) {
        case WM_NCDESTROY:
            if (!ProcessIsExiting()) {
                std::lock_guard<std::mutex> lock(g_alertsMutex);
                g_psDialogs.erase(hWnd);
                std::erase_if(g_darkButtons, [hWnd](const auto& entry) { return entry.second == hWnd; });
            }
            break;

        case WM_PAINT: {
            if (!hBrush || !PrepareAlert(hWnd)) break;
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            if (hdc) FillAlert(hWnd, hdc, &ps.rcPaint, hBrush);
            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_ERASEBKGND:
        case WM_PRINTCLIENT:
            if (!hBrush || !wParam || !PrepareAlert(hWnd)) break;
            FillAlert(hWnd, (HDC)wParam, nullptr, hBrush);
            return uMsg == WM_ERASEBKGND ? TRUE : 0;

        case WM_CTLCOLORDLG:
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORBTN: {
            LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            if (!hBrush || !wParam || !WantDarkAlert(hWnd)) return result;

            if (uMsg == WM_CTLCOLORBTN && lParam) {
                bool themed;
                {
                    std::lock_guard<std::mutex> lock(g_alertsMutex);
                    themed = g_darkButtons.contains((HWND)lParam);
                }
                if (!themed) PostMessageW(hWnd, PrepareAlertMessage(), 0, 0);
            }

            SetTextColor((HDC)wParam, g_colText.load(std::memory_order_relaxed));
            SetBkColor((HDC)wParam, g_colMenu.load(std::memory_order_relaxed));
            return (LRESULT)hBrush;
        }

        default:
            if (uMsg == PrepareAlertMessage()) {
                PrepareAlert(hWnd);
                return 0;
            }
            break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void RedrawAlert(HWND hWnd) {
    RedrawWindow(hWnd, nullptr, nullptr, RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN);
}

// Called as a window is shown and, as a catch-all, at every frame paint. Every
// PSDialogBox is watched, since what it holds is only known once it paints. The
// class is checked first, so any other window costs one call.
void WatchPsDialog(HWND hWnd) {
    if (!g_darkMessageBoxes.load(std::memory_order_relaxed) || ProcessIsExiting() ||
        g_unloading.load(std::memory_order_relaxed) ||
        g_windowsBuild.load(std::memory_order_relaxed) < kFirstBuildWithDarkButtons ||
        !IsPsDialog(hWnd)) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_alertsMutex);
        if (g_psDialogs.contains(hWnd)) return;
    }

    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, AlertSubclassProc, 0)) {
        Wh_Log(L"Failed to subclass Photoshop dialog %p; it stays light.", hWnd);
        return;
    }

    if (!RecordSubclassed(g_psDialogs, g_alertsMutex, hWnd)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, AlertSubclassProc);
        return;
    }

    RedrawAlert(hWnd);
}

void RestoreAlerts() {
    std::vector<HWND> dialogs, buttons;
    {
        std::lock_guard<std::mutex> lock(g_alertsMutex);
        dialogs.assign(g_psDialogs.begin(), g_psDialogs.end());
        g_psDialogs.clear();
        for (const auto& entry : g_darkButtons) buttons.push_back(entry.first);
        g_darkButtons.clear();
    }

    for (HWND hWnd : dialogs) WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, AlertSubclassProc);
    for (HWND hButton : buttons) SetWindowTheme(hButton, nullptr, nullptr);
    for (HWND hWnd : dialogs) RedrawAlert(hWnd);
}

// On a settings change: the boxes already watched repaint in the new colors, and
// one that is open picks the setting up.
void SyncAlerts() {
    if (!g_darkMessageBoxes.load(std::memory_order_relaxed)) {
        RestoreAlerts();
        return;
    }

    std::vector<HWND> known;
    {
        std::lock_guard<std::mutex> lock(g_alertsMutex);
        known.assign(g_psDialogs.begin(), g_psDialogs.end());
    }
    for (HWND hWnd : known) RedrawAlert(hWnd);

    EnumWindows([](HWND hWnd, LPARAM) WINAPI -> BOOL {
        DWORD pid = 0;
        GetWindowThreadProcessId(hWnd, &pid);
        if (pid == GetCurrentProcessId() && IsWindowVisible(hWnd)) WatchPsDialog(hWnd);
        return TRUE;
    }, 0);
}

// Catches a menu popup or a dropdown list at creation, before it has painted.
// user32 creates both for itself, so nothing exported sees them appear - only a
// CBT hook does.
LRESULT CALLBACK CbtProcHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HCBT_CREATEWND) SubclassIfKnown((HWND)wParam);
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// A thread whose hooks are up. Unlike the mod's function hooks, SetWindowsHookEx
// hooks are not removed by the Windhawk engine, and their procedures live in the
// mod image, so each one is kept to be released at uninit.
//
// The handle holds the thread open, because Windows does not reuse a thread's
// id while a handle to it is open. Photoshop churns threads, so without it a
// new UI thread landing on a dead one's id would count as hooked and its menus
// would never be themed.
struct HookedThread {
    HANDLE hThread = nullptr;
    HHOOK hCwpRet = nullptr;
    HHOOK hCwp = nullptr;
    HHOOK hCbt = nullptr;
};

// Global so that Wh_ModAfterInit can hook a thread from outside it, and so that
// Wh_ModUninit, which never runs on the thread that installed them, can take the
// hooks down again: UnhookWindowsHookEx works from any thread. Bounded by the
// number of UI threads alive.
std::unordered_map<DWORD, HookedThread> g_hookedThreads;
std::mutex g_hookedThreadsMutex;

void RemoveThreadHooks(const HookedThread& thread) {
    if (thread.hCwpRet) UnhookWindowsHookEx(thread.hCwpRet);
    if (thread.hCwp) UnhookWindowsHookEx(thread.hCwp);
    if (thread.hCbt) UnhookWindowsHookEx(thread.hCbt);
}

// Windows frees a thread's hooks when the thread exits, so only its handle is
// left to release. Called with g_hookedThreadsMutex held.
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

// The message hooks are not tied to a menu being up. A dropdown list is painted
// with no menu in sight, and the list windows are created once per combo box and
// kept for the life of the process - most of them before this mod ever loaded -
// so the only thing that reliably identifies one is the painting itself.
// Returns whether the thread is now recorded as hooked, which is false only
// when every hook failed and a later paint should retry.
bool InstallThreadHook(DWORD tid) {
    if (ProcessIsExiting()) return true;

    {
        std::lock_guard<std::mutex> lock(g_hookedThreadsMutex);
        if (g_unloading.load(std::memory_order_relaxed) || g_hookedThreads.contains(tid)) return true;
    }

    // A thread whose handle cannot be opened is still recorded, since
    // reinstalling on every call would be worse than the id reuse the handle
    // guards against.
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

    // The closing hook goes up before the opening one, never the other way
    // round. The two bracket tl_comboListDepth, and the thread is live: a
    // WM_DRAWITEM dispatched between the two calls with only the opening hook
    // installed would raise the counter with nothing left to lower it, and
    // nothing resets it the way LeaveMenu resets the menu counter. The color
    // hooks read that counter on its own, with no window class to fall back on,
    // so the whole thread would then paint in the mod's colors for the rest of
    // the session. In this order the odd message out closes a draw that was
    // never recorded, which the return hook ignores.
    //
    // Each error is taken next to its own call. A later success does not clear
    // the last error, so one reading taken after all three can just as easily
    // belong to a call that worked.
    HHOOK hCwpRet = SetWindowsHookExW(WH_CALLWNDPROCRET, CallWndRetProcHook, nullptr, tid);
    DWORD errCwpRet = hCwpRet ? 0 : GetLastError();
    // And for the same reason the opening hook is not installed alone.
    HHOOK hCwp = hCwpRet ? SetWindowsHookExW(WH_CALLWNDPROC, CallWndProcHook, nullptr, tid)
                         : nullptr;
    DWORD errCwp = (hCwpRet && !hCwp) ? GetLastError() : 0;
    HHOOK hCbt = SetWindowsHookExW(WH_CBT, CbtProcHook, nullptr, tid);
    DWORD errCbt = hCbt ? 0 : GetLastError();

    // Nothing went up, so let a later paint on this thread try again rather
    // than leaving the thread recorded as hooked when it is not. A partial
    // failure keeps what it got: retrying would only add a second copy of the
    // hooks that did install.
    bool retry = !hCwpRet && !hCwp && !hCbt;

    // The entry is gone if the thread has exited, or if Wh_ModUninit emptied the
    // map in the meantime. Either way nothing would take these hooks down later.
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

// Called from the color hooks, which any thread that paints reaches early. The
// mod can be enabled before Photoshop has a single window - Windhawk injects at
// process start - so this retries rather than giving up, but at most once a
// second per thread while the answer is still no.
thread_local bool tl_threadSwept = false;
thread_local ULONGLONG tl_lastSweep = 0;

void EnsureThreadHooks() {
    if (tl_threadSwept) return;

    ULONGLONG now = GetTickCount64();
    if (tl_lastSweep && now - tl_lastSweep < 1000) return;
    tl_lastSweep = now;

    // Set before the sweep, not after: subclassing can paint, and painting comes
    // back through here.
    tl_threadSwept = true;

    // Windows this thread already has, which the CBT hook by definition never
    // sees. Both kinds are cached and reused by the system, so one that existed
    // before the mod loaded would never be revisited - which is the usual case,
    // since a mod is normally first enabled in a Photoshop already in use. A
    // dropdown's list sits on the desktop, so this enumeration reaches it.
    int windows = 0;
    EnumThreadWindows(GetCurrentThreadId(), [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
        (*(int*)lParam)++;
        SubclassIfKnown(hWnd);
        return TRUE;
    }, (LPARAM)&windows);

    if (windows == 0) {
        tl_threadSwept = false;  // not a UI thread yet - ask again later
        return;
    }

    if (!InstallThreadHook(GetCurrentThreadId())) {
        tl_threadSwept = false;  // hooks would not go up - ask again later
    }
}

// An owner-draw callback for a menu item.
bool IsMenuItemDraw(const DRAWITEMSTRUCT* di) {
    return di->CtlType == ODT_MENU;
}

// A dropping combo box produces two of these on the way past a WH_CALLWNDPROC
// hook, and both are wanted: the inner one the ComboLBox sends its parent
// (ODT_LISTBOX, hwndItem the list) and the outer one the combo forwards to the
// application (ODT_COMBOBOX, hwndItem the combo). A plain owner-drawn list box
// looks exactly like the first of those and is not a dropdown at all, so the
// class is checked rather than the type alone: Photoshop's own lists are all
// combo boxes, while a third-party plugin dialog is free to own-draw a list box
// that fills its own light background and then draws text in whichever system
// color it asks for - which this mod would otherwise answer in list colors and
// leave light on light.
//
// The closed combo field arrives as a list draw too, marked ODS_COMBOBOXEDIT.
// Photoshop paints that one in its own colors and it already looks right. The
// rows of a CBS_SIMPLE combo, whose list is always open inside its dialog, are
// left alone as well.
//
// Only the opening hook asks this. The closing hook goes by what was recorded
// then, since the answer can change while the row is drawn. See tl_drawKinds.
bool IsListItemDraw(const DRAWITEMSTRUCT* di) {
    if (di->itemState & ODS_COMBOBOXEDIT) return false;
    if (di->CtlType == ODT_COMBOBOX) {
        return (GetWindowLongPtrW(di->hwndItem, GWL_STYLE) & CBS_DROPDOWNLIST) != CBS_SIMPLE;
    }
    return di->CtlType == ODT_LISTBOX && IsThemedList(di->hwndItem);
}

// Not every row's text color is asked for. Some of Photoshop's list drawing
// paints with whatever is already on the DC, and what user32 left there is the
// real COLOR_WINDOWTEXT, put in place through an internal path that no hook
// sees - which is how a row came out black on a themed background. Setting the
// colors before the owner-draw runs costs nothing where Photoshop does specify
// them, since its own call lands after this one.
void PrepareListItemDC(const DRAWITEMSTRUCT* di) {
    if (!di->hDC) return;

    bool selected = (di->itemState & ODS_SELECTED) != 0;
    SetTextColor(di->hDC, selected ? g_colHiText.load(std::memory_order_relaxed)
                                   : g_colText.load(std::memory_order_relaxed));
    SetBkColor(di->hDC, selected ? g_colHighlight.load(std::memory_order_relaxed)
                                 : g_colMenu.load(std::memory_order_relaxed));
}

// Before the window handles the message. Both counters are opened here and
// closed in the return hook, so they are set for exactly the span in which the
// item is painted - which is where every color the mod cares about is read.
LRESULT CALLBACK CallWndProcHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code != HC_ACTION || !lParam) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    auto* cwp = (CWPSTRUCT*)lParam;

    if (cwp->message == WM_DRAWITEM && cwp->lParam) {
        auto* di = (const DRAWITEMSTRUCT*)cwp->lParam;

        // Past the end of the record, a draw is left alone rather than counted
        // with nothing to say how to uncount it.
        DrawKind kind = DrawKind::Other;
        if (tl_drawNesting < kMaxNestedDraws) {
            // Menu items stay scoped to a menu this mod is tracking, so a menu
            // it deliberately leaves alone is not half-themed.
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
            // On the inner draw hwndItem is the list itself, which is how the
            // mod finds a list it never saw created; on the outer one it is the
            // combo box, hence the class check. The subclass is for the fill
            // behind the whole list, which happens outside any WM_DRAWITEM.
            if (IsComboListWindow(di->hwndItem)) SubclassComboList(di->hwndItem);
            PrepareListItemDC(di);
        }
    } else if (cwp->message == WM_CTLCOLORLISTBOX &&
               g_themeDropdowns.load(std::memory_order_relaxed)) {
        // The other message that names the list, and it arrives earlier in the
        // paint than the first item does.
        if (IsComboListWindow((HWND)cwp->lParam)) SubclassComboList((HWND)cwp->lParam);
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// And after, which is where the pairs close and where a popup that has just been
// initialised can be stamped.
LRESULT CALLBACK CallWndRetProcHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code != HC_ACTION || !lParam) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    auto* ret = (CWPRETSTRUCT*)lParam;

    // Just before a window is shown its frame is final and nothing of it is on
    // screen. This also reaches dialogs, which user32 creates without the
    // exported CreateWindowExW. The second message covers a show that skips the
    // first. A frame paint is the catch-all, for a window that was skipped while
    // minimized or that was already showing when its thread was hooked.
    if (ret->message == WM_WINDOWPOSCHANGING || ret->message == WM_WINDOWPOSCHANGED) {
        if (ret->lParam && (((const WINDOWPOS*)ret->lParam)->flags & SWP_SHOWWINDOW)) {
            ApplyTitleBar(ret->hwnd);
            WatchPsDialog(ret->hwnd);
        }
    } else if (ret->message == WM_NCACTIVATE || ret->message == WM_NCPAINT) {
        ApplyTitleBar(ret->hwnd);
        WatchPsDialog(ret->hwnd);
    } else if (ret->message == WM_NCDESTROY) {
        ForgetTitleBar(ret->hwnd);
    }

    if (ret->message == WM_INITMENUPOPUP) {
        // Only for a menu going up through TrackPopupMenu(Ex): stamping every
        // menu in the process would darken the background of ones whose text the
        // system paints, leaving them dark on dark.
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
        // The same DC, prepared for whatever the list paints outside a row -
        // and done after the parent has answered, so it is the mod's colors
        // that stand.
        SetTextColor((HDC)ret->wParam, g_colText.load(std::memory_order_relaxed));
        SetBkColor((HDC)ret->wParam, g_colMenu.load(std::memory_order_relaxed));
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// Nested tracking calls (submenus opened via TrackPopupMenu) share the outermost
// entry. The hooks themselves are not put up here - they belong to the thread,
// not to the menu - but this is one more chance to notice a thread that has not
// been hooked yet.
void EnterMenu(HMENU hMenu) {
    tl_menuDepth++;
    EnsureThreadHooks();
    ApplyMenuBackground(hMenu);
}

void LeaveMenu() {
    if (--tl_menuDepth > 0) return;

    tl_menuDepth = 0;

    // A menu dismissed mid-draw would otherwise leave the counter high and the
    // color overrides stuck on for this thread.
    tl_menuDrawDepth = 0;
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

// The mod can be enabled before Photoshop has a single window, so the thread
// hooks cannot all go up at init. This is the earliest reliable moment: a thread
// that creates a window is a UI thread, and Photoshop creates its own windows
// through the exported call - unlike the menu popups and dropdown lists, which
// user32 creates for itself and which this hook therefore never sees.
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
        // A thread's first window can be on screen by the time its message
        // hooks go up.
        if (IsWindowVisible(hWnd)) ApplyTitleBar(hWnd);
    }
    return hWnd;
}

// Both color hooks are also where the mod notices a UI thread for the first
// time: any thread that paints gets here early, long before it opens a menu or
// drops a list down.
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
        // Callers may cache this handle for the process lifetime, so the
        // brushes it hands out can never be deleted. See Wh_ModUninit.
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

    int w = lprc->right - lprc->left;
    int h = lprc->bottom - lprc->top;

    // Some of Photoshop's owner-draw code fills with a system color
    // pseudo-handle - (HBRUSH)(COLOR_MENU + 1), (HBRUSH)(COLOR_HIGHLIGHT + 1) -
    // which user32 resolves from shared memory, so neither GetSysColor nor
    // GetSysColorBrush is consulted; re-resolve them here instead. Both routes
    // are live: the Layers blend list asks for the color and makes its own
    // brush, while the options bar's list paints its rows with the pseudo-handle
    // and would otherwise stay light while the other went dark.
    ULONG_PTR sysIndex = (ULONG_PTR)hbr - 1;
    if (sysIndex <= (ULONG_PTR)COLOR_MENUBAR) {
        HBRUSH ours = inMenu ? BrushForSysColor((int)sysIndex)
                             : BrushForDropdown((int)sysIndex);
        if (ours && IsPaintContext(hdc, inMenu ? kMenuPopupClass : kComboListClass)) {
            Wh_Log(L"FillRect: system color %d -> mod color, %dx%d", (int)sysIndex, w, h);
            return FillRect_Original(hdc, lprc, ours);
        }
        return FillRect_Original(hdc, lprc, hbr);
    }

    // Separator lines are a menu shape; a list draws its dividers as part of an
    // item, so the geometry guess is not extended to one.
    HBRUSH hSep = g_hSeparatorBrush.load(std::memory_order_acquire);
    if (inMenu && hSep && IsSeparatorGeometry(hdc, w, h) && IsPaintContext(hdc, kMenuPopupClass)) {
        Wh_Log(L"FillRect: separator %dx%d recolored", w, h);
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

// A menu Photoshop sets its own background on would put the theme's light text
// on it, or leave a strip of the theme's gutter beside it, once the menu is in
// the dark theme. Clearing the background hands the whole popup back to the
// theme. The mod's own stamp passes through untouched, and so does a menu
// having its background cleared.
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

// ---------------------------------------------------------------------------
// Menus Windows draws
// ---------------------------------------------------------------------------

// A few of Photoshop's menus are left to Windows, which paints them through the
// theme engine: the context menu on a document tab or a panel tab, for one.
// uxtheme has a dark variant of that drawing behind two exports with an ordinal
// and no name. Ordinal 135 is a different function before Windows 10 1903,
// hence the version check. The technique comes from the dark-menus mod.
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
    if (pRtlGetVersion && pRtlGetVersion(&version) == 0) {
        g_windowsBuild.store(version.dwBuildNumber, std::memory_order_relaxed);
    }
    if (version.dwBuildNumber < kFirstBuildWithPreferredAppMode) {
        Wh_Log(L"Windows build too old for dark menus; menus Windows draws stay light.");
        return;
    }

    g_hUxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_hUxtheme) return;

    g_pSetPreferredAppMode = (SetPreferredAppMode_t)(void*)GetProcAddress(
        g_hUxtheme, MAKEINTRESOURCEA(kOrdinalSetPreferredAppMode));
    g_pFlushMenuThemes = (FlushMenuThemes_t)(void*)GetProcAddress(
        g_hUxtheme, MAKEINTRESOURCEA(kOrdinalFlushMenuThemes));
}

// The flush applies the change to a running Photoshop, which otherwise keeps the
// menu theme it already opened.
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

// ---------------------------------------------------------------------------
// Windhawk events
// ---------------------------------------------------------------------------

void Wh_ModSettingsChanged() {
    LoadSettings();
    ApplyDarkMenus();
    SyncTitleBars();
    SyncAlerts();
}

BOOL Wh_ModInit() {
    // No header or import library has it, so it is looked up. See ProcessIsExiting.
    g_pRtlDllShutdownInProgress = (RtlDllShutdownInProgress_t)(void*)GetProcAddress(
        GetModuleHandleW(L"ntdll.dll"), "RtlDllShutdownInProgress");

    LoadSettings();

    // Every other hook failing degrades the result: a menu themed less
    // completely, or a dropdown list left alone. These two are the entry points
    // the whole mod hangs off - nothing else raises tl_menuDepth, so without one
    // of them no popup is ever stamped and no color is ever overridden. Losing
    // both is worth failing over rather than running as a mod that appears
    // installed and does nothing.
    bool menuEntryHooked = false;
    if (WindhawkUtils::SetFunctionHook(TrackPopupMenu, TrackPopupMenu_Hook, &TrackPopupMenu_Original)) {
        menuEntryHooked = true;
    } else {
        Wh_Log(L"Failed to hook TrackPopupMenu");
    }
    if (WindhawkUtils::SetFunctionHook(TrackPopupMenuEx, TrackPopupMenuEx_Hook, &TrackPopupMenuEx_Original)) {
        menuEntryHooked = true;
    } else {
        Wh_Log(L"Failed to hook TrackPopupMenuEx");
    }

    if (!WindhawkUtils::SetFunctionHook(GetSysColor, GetSysColor_Hook, &GetSysColor_Original)) Wh_Log(L"Failed to hook GetSysColor");
    if (!WindhawkUtils::SetFunctionHook(GetSysColorBrush, GetSysColorBrush_Hook, &GetSysColorBrush_Original)) Wh_Log(L"Failed to hook GetSysColorBrush");
    if (!WindhawkUtils::SetFunctionHook(FillRect, FillRect_Hook, &FillRect_Original)) Wh_Log(L"Failed to hook FillRect");
    if (!WindhawkUtils::SetFunctionHook(PatBlt, PatBlt_Hook, &PatBlt_Original)) Wh_Log(L"Failed to hook PatBlt");
    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original)) Wh_Log(L"Failed to hook CreateWindowExW");
    if (!WindhawkUtils::SetFunctionHook(SetMenuInfo, SetMenuInfo_Hook, &SetMenuInfo_Original)) Wh_Log(L"Failed to hook SetMenuInfo");

    if (!menuEntryHooked) {
        Wh_Log(L"Neither TrackPopupMenu nor TrackPopupMenuEx could be hooked; "
               L"there is nothing the mod could theme.");
        return FALSE;
    }

    // Only once the mod is sure to load: Wh_ModUninit is not called after a
    // FALSE return, and nothing else would put the process back.
    ResolveDarkMenuApis();
    ApplyDarkMenus();

    return TRUE;
}

// Threads that already have windows, which is every thread when the mod is
// enabled in a Photoshop that is already running. The CBT hook has to go up for
// them here, because a dropdown list created before it would never be seen
// otherwise. A thread hook can be installed from any thread, so this works from
// the engine's. At process start there is nothing to find yet, and
// EnsureThreadHooks picks the threads up from their first paint instead.
//
// This waits for Wh_ModAfterInit rather than running in Wh_ModInit for two
// reasons. The function hooks are not applied until Wh_ModInit returns, so a
// thread found earlier would be hooked before the hooks behind it were live.
// And Wh_ModUninit is not called when Wh_ModInit returns FALSE, so anything
// installed on that path would outlive the image: a SetWindowsHookEx procedure
// still registered against an unmapped DLL is a crash on the next message for
// that thread.
void Wh_ModAfterInit() {
    EnumWindows([](HWND hWnd, LPARAM) WINAPI -> BOOL {
        DWORD pid = 0;
        DWORD tid = GetWindowThreadProcessId(hWnd, &pid);
        if (tid && pid == GetCurrentProcessId()) InstallThreadHook(tid);
        return TRUE;
    }, 0);

    SyncTitleBars();
    SyncAlerts();
}

void Wh_ModUninit() {
    // Before anything is taken down, so that nothing new goes up behind it.
    g_unloading.store(true, std::memory_order_relaxed);

    // Back to whatever mode the process was in, with no restart needed.
    g_themeWindowsMenus.store(false, std::memory_order_relaxed);
    ApplyDarkMenus();
    if (g_hUxtheme) FreeLibrary(g_hUxtheme);

    // Switched off first, or a window repainting in the meantime would come back
    // through ApplyTitleBar and be left dark.
    g_themeTitleBars.store(false, std::memory_order_relaxed);
    RestoreTitleBars();

    // The subclass procedure lives in this image, so an alert left open must
    // lose it before the image goes.
    g_darkMessageBoxes.store(false, std::memory_order_relaxed);
    RestoreAlerts();

    // The message hooks belong to Photoshop's UI threads, not to this one, so
    // they are released through the global map. See HookedThread.
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

    // Undo the background brush stamped on Photoshop's menus, so disabling the
    // mod restores the normal menus without restarting the application.
    RestoreStampedMenus();

    // The brushes are deliberately NOT deleted.
    //
    // Two paths hand these handles to Photoshop and neither can be reclaimed:
    // MENUINFO::hbrBack on menus that outlive the mod, and GetSysColorBrush
    // return values, which callers are entitled to cache for the process
    // lifetime. Deleting them means Photoshop eventually paints with a freed -
    // possibly recycled - GDI handle. A handful of brushes for the remaining
    // lifetime of the process is a trivial cost next to a crash in the host.
}

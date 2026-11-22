// ==WindhawkMod==
// @id            adobe-acrylic-menus
// @name          Adobe Acrylic Menus
// @description   Acrylic popup menus and experimental dark Lightroom Classic dialogs.
// @version       1.3.3
// @author        Marvin Menzel
// @github        https://github.com/marvintrvl
// @license       MIT
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
// @compilerOptions -lgdi32 -lcomctl32 -ldwmapi -luxtheme -lmsimg32
// ==/WindhawkMod==

/*
MIT License

Copyright (c) 2026 SABER

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// ==WindhawkModReadme==
/*
# Adobe Acrylic Menus

Adds configurable acrylic or transparent backgrounds to supported Adobe popup
menus. Based on [Adobe Dark Menus](https://github.com/sabergraphics/adobe-dark-menus)
by Saber Naeemi, retaining its dark-menu and title-bar features.

Popup tint, opacity, highlight color, inset and radius are configurable. The
color picker uses #RRGGBBAA by default; an ARGB option is available. Windows
transparency effects must be enabled. Application drawing and Windows composition
vary, so some menus and controls can remain opaque.

Lightroom Classic has optional experimental dark dialogs and acrylic backgrounds.
Enable Experimental dark Lightroom dialogs and Experimental acrylic Preferences;
the additional all-dialog switch extends the effect to captioned secondary windows.
The main Lightroom editing frame is excluded. Close and reopen dialogs after
changing these settings. Custom controls, expanded dropdowns, file pickers, browser
panes and confirmation dialogs may remain opaque or light. Inspect text, focus and
control states after enabling the experiment. The Preferences-only acrylic mode
and fixes for specific dialogs depend on English window titles. Other UI
languages can still use general dialog styling and the all-dialog acrylic mode,
but may miss these targeted fixes.

Testing so far has primarily been performed in Lightroom Classic on Windows 11.
The Adobe process targets come from the upstream mod; acrylic behavior elsewhere
has not been comprehensively verified. Disable Adobe Dark Menus for the same
applications when using this fork.

This separate submission integrates acrylic with Adobe-specific menu painting,
including clearing opaque rows and preserving composited menu text. Its
Lightroom dialog styling is also specific to Adobe. The fork shares hooks with
Adobe Dark Menus and is intended to be used independently.

## Screenshots

**Lightroom Classic Settings menu**

![Acrylic Settings menu with Copy Settings highlighted](https://raw.githubusercontent.com/marvintrvl/windhawk-mods/d1ea0c2d1fbc6502a347c3964b84f94420e960c7/images/adobe-acrylic-menus/lightroom-acrylic-settings-menu.png)

**Lightroom Classic Preferences with optional dialog acrylic**

![Acrylic Preferences dialog on the Interface tab](https://raw.githubusercontent.com/marvintrvl/windhawk-mods/d1ea0c2d1fbc6502a347c3964b84f94420e960c7/images/adobe-acrylic-menus/lightroom-acrylic-preferences.png)

Original Adobe Dark Menus copyright (c) 2026 SABER. Used under the MIT license;
the complete license is included in this source file.
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- ColorHexFormat: "rgba"
  $name: "8-Digit Color Format"
  $description: 'rgba = #RRGGBBAA (editor picker); argb = #AARRGGBB. Alpha controls popup tint or hover for their background colors.'
  $options:
  - rgba: "RGBA (#RRGGBBAA)"
  - argb: "ARGB (#AARRGGBB)"
- MenuBgColor: "#2D2D2D"
  $name: "Menu Background Color"
  $description: An 8-digit value supplies popup tint opacity unless Menu Tint Color also has alpha.
- MenuTextColor: "#FFFFFF"
  $name: "Menu Text Color"
- HighlightBgColor: "#505050"
  $name: "Highlight Background Color"
  $description: Solid selection color for dropdown lists and menus without acrylic.
- MenuBarHighlightColor: "#505050"
  $name: "Menu Bar Highlight Color"
  $description: Background for the selected top menu label; independent of popup item highlights.
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
- ExperimentalDarkPreferences: false
  $name: "Experimental dark Lightroom dialogs"
  $description: 'Opt-in for Lightroom Classic secondary windows. Some targeted fixes require the English UI. Close dialogs before changing.'
- ExperimentalPreferencesAcrylic: false
  $name: "Experimental acrylic Preferences"
  $description: 'Enables acrylic for English-language Preferences. Requires Experimental dark Lightroom dialogs. Close dialogs before changing.'
- ExperimentalAllDialogAcrylic: false
  $name: "Experimental acrylic for other Lightroom dialogs"
  $description: 'Extends acrylic to captioned Lightroom secondary windows in any UI language. Some targeted fixes need English. Requires both experimental dialog settings above.'
- PreferencesTintColor: "#2D2D2D"
  $name: "Preferences acrylic tint"
  $description: 'Independent of menu tint. Uses the selected color hex format; an 8-digit color overrides Preferences tint opacity.'
- PreferencesTintOpacity: 45
  $name: "Preferences acrylic tint opacity"
  $description: 'Tint opacity from 1 to 100. Lower values let more background show through.'
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
- MenuMaterial: "acrylic"
  $name: "Popup Menu Material"
  $description: Solid disables the experimental popup effects.
  $options:
  - acrylic: Acrylic
  - transparent: Transparent
  - system-acrylic: System Acrylic
  - mica: Mica
  - solid: Solid
- MenuTintOpacity: 35
  $name: "Menu Tint Opacity (1-100)"
  $description: 'Opacity of Acrylic or transparent tint when the color has no alpha. Text and icons are not intentionally faded.'
- MenuTintColor: "#2D2D2D"
  $name: "Menu Tint Color"
  $description: 'Optional tint color. Empty follows Menu Background Color. An 8-digit value supplies tint opacity and takes priority over Menu Background Color alpha.'
- MenuHoverColor: "#E0E0E3"
  $name: "Acrylic Menu Hover Color"
  $description: 'Color of the translucent hover overlay. Leave empty to follow the solid highlight color. An 8-digit value overrides Menu Hover Opacity.'
- MenuHoverOpacity: 30
  $name: "Menu Hover Opacity (0-100)"
  $description: 'Opacity of the acrylic hover overlay when its color has no alpha.'
- MenuHoverInset: 7
  $name: "Hover Horizontal Inset (DIP)"
  $description: 'Padding on the left and right of selected menu items, like Explorer translucent menus.'
- MenuHoverVerticalInset: 2
  $name: "Hover Vertical Inset (DIP)"
  $description: 'Padding above and below selected menu items.'
- MenuHoverRadius: 6
  $name: "Hover Corner Radius (DIP)"
  $description: 'Rounded highlight corners. 0 gives square corners.'
- MenuClearOpaqueRows: true
  $name: "Remove Solid Native Menu Rows"
  $description: 'Clear native Win32 background fills and disable forced opaque menu brushes for system-drawn menus.'
- RoundedPopupCorners: true
  $name: "Rounded Popup Corners"
  $description: 'Request small Windows 11 rounded corners for modified menus.'
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <vssym32.h>
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
std::atomic<COLORREF> g_colText{RGB(255, 255, 255)};
std::atomic<COLORREF> g_colHighlight{RGB(80, 80, 80)};
std::atomic<COLORREF> g_colMenuBarHighlight{RGB(80, 80, 80)};
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

// Declared before LoadSettings. Implementation follows the popup subclass.
extern std::atomic<bool> g_darkPreferences;
extern std::atomic<bool> g_preferencesAcrylicEnabled;
extern std::atomic<bool> g_allDialogAcrylicEnabled;
extern std::atomic<int> g_preferencesTintOpacity;
extern std::atomic<COLORREF> g_preferencesTint;
extern std::atomic<int> g_acrylicMaterial;
extern std::atomic<int> g_acrylicOpacity;
extern std::atomic<int> g_acrylicHoverOpacity;
extern std::atomic<int> g_acrylicHoverInset;
extern std::atomic<int> g_acrylicHoverVerticalInset;
extern std::atomic<int> g_acrylicHoverRadius;
extern std::atomic<bool> g_acrylicClearOpaqueRows;
extern std::atomic<COLORREF> g_acrylicTint;
extern std::atomic<COLORREF> g_acrylicHoverColor;
extern std::atomic<bool> g_acrylicRounded;
extern std::atomic<bool> g_acrylicSupported;
void AcrylicResetAllPopups();


// Whether the process is actually in dark menu mode. Stays false with the setting
// on before Windows 10 1903.
std::atomic<bool> g_darkMenusActive{false};

// Never deleted. See Wh_ModUninit.
std::atomic<HBRUSH> g_hMenuBrush{nullptr};
std::atomic<HBRUSH> g_hTextBrush{nullptr};
std::atomic<HBRUSH> g_hHighlightBrush{nullptr};
std::atomic<HBRUSH> g_hMenuBarHighlightBrush{nullptr};
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
thread_local const DRAWITEMSTRUCT* tl_drawItems[kMaxNestedDraws]{};
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
std::atomic<bool> g_colorFormatArgb{false};

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
    // Editor picker uses RRGGBBAA; legacy ARGB input is selectable.
    // COLORREF has no alpha; ParseColorAlpha reads it separately where supported.
    if (allHex && len == 8) {
        if (g_colorFormatArgb.load(std::memory_order_relaxed)) p += 2;
        len = 6;
    }
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

// Return an explicit alpha from a valid 8-digit color. Six-digit colors
// deliberately leave the separate percentage setting in control.
bool ParseColorAlpha(PCWSTR input, int* opacityPercent) {
    if (!input || !opacityPercent) return false;
    while (*input == L' ') ++input;
    if (*input == L'#') ++input;
    size_t len = wcslen(input);
    while (len && input[len - 1] == L' ') --len;
    if (len != 8) return false;
    for (size_t i = 0; i < len; ++i) if (!iswxdigit(input[i])) return false;
    const wchar_t* alpha = input + (g_colorFormatArgb.load(std::memory_order_relaxed) ? 0 : 6);
    unsigned int byte = 0;
    if (swscanf_s(alpha, L"%2x", &byte) != 1) return false;
    *opacityPercent = static_cast<int>((byte * 100 + 127) / 255);
    return true;
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
    g_darkPreferences.store(Wh_GetIntSetting(L"ExperimentalDarkPreferences") != 0, std::memory_order_relaxed);
    auto format = WindhawkUtils::StringSetting::make(L"ColorHexFormat");
    g_colorFormatArgb.store(format.get() && !_wcsicmp(format.get(), L"argb"),
                            std::memory_order_relaxed);
    g_preferencesAcrylicEnabled.store(Wh_GetIntSetting(L"ExperimentalPreferencesAcrylic") != 0,
                                      std::memory_order_relaxed);
    g_allDialogAcrylicEnabled.store(Wh_GetIntSetting(L"ExperimentalAllDialogAcrylic") != 0,
                                    std::memory_order_relaxed);
    int preferencesOpacity = Wh_GetIntSetting(L"PreferencesTintOpacity");
    g_preferencesTintOpacity.store(preferencesOpacity < 1 ? 1 : preferencesOpacity > 100 ? 100 : preferencesOpacity,
                                   std::memory_order_relaxed);
    auto preferencesTint = WindhawkUtils::StringSetting::make(L"PreferencesTintColor");
    g_preferencesTint.store(preferencesTint.get() ? ParseHexColorOr(preferencesTint.get(), RGB(45,45,45)) : RGB(45,45,45),
                            std::memory_order_relaxed);
    int preferencesAlpha = 0;
    if (preferencesTint.get() && ParseColorAlpha(preferencesTint.get(), &preferencesAlpha))
        g_preferencesTintOpacity.store(preferencesAlpha < 1 ? 1 : preferencesAlpha, std::memory_order_relaxed);
    g_themeDropdowns.store(Wh_GetIntSetting(L"ThemeDropdowns") != 0, std::memory_order_relaxed);
    g_themeWindowsMenus.store(Wh_GetIntSetting(L"ThemeWindowsMenus") != 0, std::memory_order_relaxed);
    g_scrollColumnMenus.store(Wh_GetIntSetting(L"ScrollColumnMenus") != 0, std::memory_order_relaxed);
    g_themeTitleBars.store(Wh_GetIntSetting(L"ThemeTitleBars") != 0, std::memory_order_relaxed);
    auto materialSetting = WindhawkUtils::StringSetting::make(L"MenuMaterial");
    PCWSTR material = materialSetting.get() ? materialSetting.get() : L"";
    int materialValue = !_wcsicmp(material, L"transparent") ? 1
                      : !_wcsicmp(material, L"acrylic") ? 2
                      : !_wcsicmp(material, L"system-acrylic") ? 3
                      : !_wcsicmp(material, L"mica") ? 4 : 0;
    g_acrylicMaterial.store(materialValue, std::memory_order_relaxed);
    int opacity = Wh_GetIntSetting(L"MenuTintOpacity");
    g_acrylicOpacity.store(opacity < 1 ? 1 : opacity > 100 ? 100 : opacity,
                           std::memory_order_relaxed);
    int hoverOpacity = Wh_GetIntSetting(L"MenuHoverOpacity");
    g_acrylicHoverOpacity.store(hoverOpacity < 0 ? 0 : hoverOpacity > 100 ? 100 : hoverOpacity,
                                std::memory_order_relaxed);
    auto clamp = [](int n, int lo, int hi) { return n < lo ? lo : n > hi ? hi : n; };
    g_acrylicHoverInset.store(clamp(Wh_GetIntSetting(L"MenuHoverInset"), 0, 40),
                              std::memory_order_relaxed);
    g_acrylicHoverVerticalInset.store(clamp(Wh_GetIntSetting(L"MenuHoverVerticalInset"), 0, 16),
                                      std::memory_order_relaxed);
    g_acrylicHoverRadius.store(clamp(Wh_GetIntSetting(L"MenuHoverRadius"), 0, 30),
                               std::memory_order_relaxed);
    g_acrylicClearOpaqueRows.store(Wh_GetIntSetting(L"MenuClearOpaqueRows") != 0,
                                   std::memory_order_relaxed);
    g_acrylicRounded.store(Wh_GetIntSetting(L"RoundedPopupCorners") != 0,
                           std::memory_order_relaxed);

    PublishColor(&g_colMenu, &g_hMenuBrush,
                 ParseHexColorOr(WindhawkUtils::StringSetting::make(L"MenuBgColor").get(), RGB(45, 45, 45)));
    PublishColor(&g_colText, &g_hTextBrush,
                 ParseHexColorOr(WindhawkUtils::StringSetting::make(L"MenuTextColor").get(), RGB(255, 255, 255)));
    PublishColor(&g_colHighlight, &g_hHighlightBrush,
                 ParseHexColorOr(WindhawkUtils::StringSetting::make(L"HighlightBgColor").get(), RGB(80, 80, 80)));
    PublishColor(&g_colMenuBarHighlight, &g_hMenuBarHighlightBrush,
                 ParseHexColorOr(WindhawkUtils::StringSetting::make(L"MenuBarHighlightColor").get(), RGB(80, 80, 80)));
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
    auto tintSetting = WindhawkUtils::StringSetting::make(L"MenuTintColor");
    auto backgroundSetting = WindhawkUtils::StringSetting::make(L"MenuBgColor");
    auto hoverSetting = WindhawkUtils::StringSetting::make(L"MenuHoverColor");
    int alphaPercent;
    // An explicit tint alpha wins over the background alpha. Both override
    // MenuTintOpacity; without alpha the percentage setting remains in effect.
    if (ParseColorAlpha(tintSetting.get(), &alphaPercent) ||
        ParseColorAlpha(backgroundSetting.get(), &alphaPercent))
        g_acrylicOpacity.store(alphaPercent, std::memory_order_relaxed);
    if (ParseColorAlpha(hoverSetting.get(), &alphaPercent))
        g_acrylicHoverOpacity.store(alphaPercent, std::memory_order_relaxed);
    g_acrylicHoverColor.store(
        ParseHexColorOr(hoverSetting.get() ? hoverSetting.get() : L"",
                        g_colHighlight.load(std::memory_order_relaxed)),
        std::memory_order_relaxed);
    COLORREF tintColor;
    if (!ParseHexColor(tintSetting.get(), &tintColor))
        tintColor = g_colMenu.load(std::memory_order_relaxed);
    g_acrylicTint.store(tintColor, std::memory_order_relaxed);
    COLORREF textColor = g_colText.load(std::memory_order_relaxed);
    Wh_Log(L"[Adobe Acrylic] Settings loaded: colorFormat=%s material=%d tint=#%02X%02X%02X tintOpacity=%d text=#%02X%02X%02X hoverOpacity=%d barHighlight=#%02X%02X%02X",
           g_colorFormatArgb.load(std::memory_order_relaxed) ? L"argb" : L"rgba", materialValue, GetRValue(tintColor), GetGValue(tintColor), GetBValue(tintColor),
           g_acrylicOpacity.load(std::memory_order_relaxed),
           GetRValue(textColor), GetGValue(textColor), GetBValue(textColor),
           g_acrylicHoverOpacity.load(std::memory_order_relaxed),
           GetRValue(g_colMenuBarHighlight.load(std::memory_order_relaxed)),
           GetGValue(g_colMenuBarHighlight.load(std::memory_order_relaxed)),
           GetBValue(g_colMenuBarHighlight.load(std::memory_order_relaxed)));
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
    {
        DepthGuard guard(&tl_ourPaintDepth);
        if (!SetMenuInfo(hMenu, &mi)) return;
    }

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

    // A translucent native popup also needs the menu's background brush cleared.
    if (ownerDrawn || (!g_acrylicClearOpaqueRows.load(std::memory_order_relaxed) &&
                       g_acrylicMaterial.load(std::memory_order_relaxed) &&
                       g_acrylicSupported.load(std::memory_order_relaxed)))
        StampMenu(hMenu, hBrush);
}

void ApplyMenuBackground(HMENU hMenu) {
    HBRUSH hBrush = (g_acrylicMaterial.load(std::memory_order_relaxed) &&
                     g_acrylicSupported.load(std::memory_order_relaxed))
        ? static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH))
        : g_hMenuBrush.load(std::memory_order_acquire);
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

// ---------------------------------------------------------------------------
// EXPERIMENTAL ADDITION: acrylic/transparent popup composition.
// DWM accent application and selective alpha-aware painting are intentionally
// separate: simply lowering an HWND's opacity would also fade text and icons.
// Supports Win11, x64, DWM; Win32 menus are an undocumented rendering path.
// ---------------------------------------------------------------------------

struct AcrylicAccentPolicy {
    int state;
    int flags;
    DWORD color;                 // AABBGGRR
    int animationId;
};
struct AcrylicCompositionData {
    int attribute;
    void* data;
    SIZE_T size;
};
using AcrylicCompositionFn = BOOL(WINAPI*)(HWND, AcrylicCompositionData*);
using AcrylicGetThemeClassFn = HRESULT(WINAPI*)(HTHEME, LPWSTR, int);

AcrylicCompositionFn g_acrylicSetComposition = nullptr;
AcrylicCompositionFn g_acrylicGetComposition = nullptr;
AcrylicGetThemeClassFn g_acrylicGetThemeClass = nullptr;

// 0 solid; 1 transparent; 2 acrylic; 3 system acrylic; 4 mica.
std::atomic<int> g_acrylicMaterial{2};
std::atomic<int> g_acrylicOpacity{45};
std::atomic<int> g_acrylicHoverOpacity{30};
std::atomic<int> g_acrylicHoverInset{7};
std::atomic<int> g_acrylicHoverVerticalInset{2};
std::atomic<int> g_acrylicHoverRadius{6};
std::atomic<bool> g_acrylicClearOpaqueRows{true};
std::atomic<COLORREF> g_acrylicTint{RGB(45, 45, 45)};
std::atomic<COLORREF> g_acrylicHoverColor{RGB(224, 224, 227)};
std::atomic<bool> g_acrylicRounded{true};
std::atomic<bool> g_acrylicSupported{false};
std::mutex g_acrylicWindowsMutex;
std::unordered_set<HWND> g_acrylicWindows;
thread_local int tl_acrylicDrawing = 0;
// Menu bars can invoke native popup tracking *inside user32*, never calling
// the exported TrackPopupMenu[Ex] hooks. Subclass paint messages still scope
// the same GDI/uxtheme work to the current popup.
thread_local int tl_acrylicPopupPaintDepth = 0;
thread_local int tl_acrylicPrepareDepth = 0;
thread_local unsigned tl_visibleMenuPopups = 0;
constexpr WCHAR kVisibleMenuPopupProp[] = L"Wh.AdobeAcrylicMenus.VisiblePopup";

// These transitions run on the popup's own thread. The property makes repeated
// show/hide messages idempotent when user32 reuses the same window.
void TrackMenuPopupVisibility(HWND hwnd, bool visible) {
    if (visible) {
        if (!GetPropW(hwnd, kVisibleMenuPopupProp) &&
            SetPropW(hwnd, kVisibleMenuPopupProp, reinterpret_cast<HANDLE>(1))) {
            ++tl_visibleMenuPopups;
        }
    } else if (RemovePropW(hwnd, kVisibleMenuPopupProp) && tl_visibleMenuPopups) {
        --tl_visibleMenuPopups;
    }
}

bool InAcrylicPopupPaintContext() {
    return tl_visibleMenuPopups || tl_acrylicPopupPaintDepth ||
           tl_menuDrawDepth || tl_menuDepth;
}

bool AcrylicPopupIsActive(HWND hwnd) {
    if (!hwnd || ProcessIsExiting()) return false;
    std::lock_guard<std::mutex> lock(g_acrylicWindowsMutex);
    return g_acrylicWindows.contains(hwnd);
}

// The popup effects can be undone without leaving a modified main Adobe window.
// Because Windows does not expose getters for every backdrop attribute, resetting
// some properties is best-effort. We never modify non-popup HWNDs.
void AcrylicResetPopup(HWND hwnd) {
    if (!IsWindow(hwnd) || ClassifyWindow(hwnd) != WindowKind::MenuPopup) return;
    if (g_acrylicSetComposition) {
        AcrylicAccentPolicy disabled{};
        AcrylicCompositionData data{19, &disabled, sizeof(disabled)};
        g_acrylicSetComposition(hwnd, &data);
    }
    DWM_BLURBEHIND blur{DWM_BB_ENABLE, FALSE, nullptr, FALSE};
    DwmEnableBlurBehindWindow(hwnd, &blur);
    DWORD backdrop = 0; // DWMSBT_AUTO
    DwmSetWindowAttribute(hwnd, 38, &backdrop, sizeof(backdrop));
    DWORD corner = 0; // DWMWCP_DEFAULT
    DwmSetWindowAttribute(hwnd, 33, &corner, sizeof(corner));
    MARGINS noFrame{};
    DwmExtendFrameIntoClientArea(hwnd, &noFrame);
}

void AcrylicResetAllPopups() {
    if (ProcessIsExiting()) return;
    std::vector<HWND> windows;
    {
        std::lock_guard<std::mutex> lock(g_acrylicWindowsMutex);
        windows.assign(g_acrylicWindows.begin(), g_acrylicWindows.end());
        g_acrylicWindows.clear();
    }
    for (HWND hwnd : windows) AcrylicResetPopup(hwnd);
}

bool AcrylicIsEnabled() {
    return g_acrylicMaterial.load(std::memory_order_relaxed) != 0 &&
           g_acrylicSupported.load(std::memory_order_relaxed);
}

bool AcrylicIsPopupDC(HDC hdc) {
    if (!InAcrylicPopupPaintContext() || !hdc || tl_ourPaintDepth ||
        !AcrylicIsEnabled() || ProcessIsExiting() ||
        g_unloading.load(std::memory_order_relaxed)) return false;
    HWND hwnd = WindowFromDC(hdc);
    if (hwnd) {
        return ClassifyWindow(hwnd) == WindowKind::MenuPopup &&
               AcrylicPopupIsActive(hwnd);
    }
    // For offscreen painting, require the exact synchronous owner-draw DC
    // for a menu attached to an active popup on this thread.
    int index = tl_drawNesting - 1;
    if (index < 0 || index >= kMaxNestedDraws) return false;
    const DRAWITEMSTRUCT* item = tl_drawItems[index];
    if (!item || item->CtlType != ODT_MENU || item->hDC != hdc) return false;
    if (GetObjectType(hdc) != OBJ_MEMDC) return false;
    HMENU menu = reinterpret_cast<HMENU>(item->hwndItem);
    std::vector<HWND> windows;
    {
        std::lock_guard<std::mutex> lock(g_acrylicWindowsMutex);
        windows.assign(g_acrylicWindows.begin(), g_acrylicWindows.end());
    }
    for (HWND popup : windows) {
        if (GetWindowThreadProcessId(popup, nullptr) == GetCurrentThreadId() &&
            reinterpret_cast<HMENU>(SendMessageW(popup, MN_GETHMENU, 0, 0)) == menu)
            return true;
    }
    return false;
}

bool AcrylicIsMenuTheme(HTHEME theme) {
    if (!g_acrylicGetThemeClass || !theme) return false;
    WCHAR name[64]{};
    return SUCCEEDED(g_acrylicGetThemeClass(theme, name, ARRAYSIZE(name))) &&
           _wcsicmp(name, L"Menu") == 0;
}

// Apply the composition effect before user32 has painted its first frame.
// No WS_EX_LAYERED: a layered uniform alpha would fade menu text and icons.
void AcrylicPreparePopup(HWND hwnd) {
    if (tl_acrylicPrepareDepth) return;
    DepthGuard preparing(&tl_acrylicPrepareDepth);
    // Critical: MenuIsOpen() is false for genuine menu-bar dropdowns created
    // by internal user32 code (including Lightroom Classic). Matching only
    // TrackPopupMenu[Ex] silently disabled this fork on those menus.
    if (!AcrylicIsEnabled() ||
        ClassifyWindow(hwnd) != WindowKind::MenuPopup ||
        ProcessIsExiting() || g_unloading.load(std::memory_order_relaxed)) return;

    // Menus are recycled by user32. Native menu-bar dropdowns do not go
    // through the original mod's exported TrackPopupMenu hook, so their
    // background must be prepared on each show too.
    HMENU popupMenu = (HMENU)SendMessageW(hwnd, MN_GETHMENU, 0, 0);
    // Reapply on show/layout: Windows can recycle an HWND and reset its
    // composition policy, and a tracked HWND is not proof it is still applied.

    int material = g_acrylicMaterial.load(std::memory_order_relaxed);
    if (material == 0) return;

    // Transparent menus need a composited client area; a blur region alone
    // is not a blur effect. Setting the menu's black brush (below) and clearing
    // themed backgrounds exposes the DWM material beneath the item content.
    HRGN region = CreateRectRgn(0, 0, -1, -1);
    if (region) {
        DWM_BLURBEHIND blur{DWM_BB_ENABLE | DWM_BB_BLURREGION,
                            TRUE, region, FALSE};
        DwmEnableBlurBehindWindow(hwnd, &blur);
        DeleteObject(region);
    }

    bool success = false;
    if (material == 3 || material == 4) {
        // System acrylic/Mica recipes have fixed opacity; supported on Win11.
        MARGINS frame{-1, -1, -1, -1};
        if (SUCCEEDED(DwmExtendFrameIntoClientArea(hwnd, &frame))) {
            DWORD backdrop = material == 3 ? 3 : 2;
            success = SUCCEEDED(DwmSetWindowAttribute(hwnd, 38, &backdrop,
                                                       sizeof(backdrop)));
        }
        if (success && g_acrylicSetComposition) {
            AcrylicAccentPolicy disabled{};
            AcrylicCompositionData data{19, &disabled, sizeof(disabled)};
            g_acrylicSetComposition(hwnd, &data);
        }
    } else {
        // Avoid a Windows-supplied system backdrop overriding the chosen tint.
        DWORD none = 1; // DWMSBT_NONE
        DwmSetWindowAttribute(hwnd, 38, &none, sizeof(none));
        int opacity = g_acrylicOpacity.load(std::memory_order_relaxed);
        BYTE alpha = static_cast<BYTE>((opacity * 255 + 50) / 100);
        if (alpha == 0) alpha = 1;
        COLORREF tint = g_acrylicTint.load(std::memory_order_relaxed);
        DWORD gradient = (static_cast<DWORD>(alpha) << 24) | tint;
        // Acrylic state 4 with flags 0 permits tint opacity to vary on the
        // supported composition path. Flag 2 selects a different recipe on
        // some Windows builds and can make the input opacity ineffective.
        int accentState = material == 2 ? 4 : 2;
        int accentFlags = material == 2 ? 0 : 2;
        AcrylicAccentPolicy accent{accentState, accentFlags, gradient, 0};
        AcrylicCompositionData data{19, &accent, sizeof(accent)};
        success = !!g_acrylicSetComposition(hwnd, &data);
        Wh_Log(L"[Adobe Acrylic] Accent policy: hwnd=%p state=%d flags=%d gradient=0x%08lX opacity=%d applied=%d",
               hwnd, accentState, accentFlags, gradient, opacity, success);
    }
    if (!success) {
        AcrylicResetPopup(hwnd);
        Wh_Log(L"[Adobe Acrylic] Apply backdrop FAILED: popup=%p material=%d "
               L"menu=%p error=%lu (trackDepth=%d).", hwnd, material,
               popupMenu, GetLastError(), tl_menuDepth);
        return;
    }

    // Commit the black menu brush ONLY after DWM accepted the material.
    // Otherwise the user's menu can become solid black on unsupported builds.
    if (popupMenu && IsMenu(popupMenu)) ApplyMenuBackground(popupMenu);
    Wh_Log(L"[Adobe Acrylic] Backdrop applied: popup=%p material=%d "
           L"menu=%p (trackDepth=%d).", hwnd, material, popupMenu,
           tl_menuDepth);
    DWORD corner = g_acrylicRounded.load(std::memory_order_relaxed) ? 3 : 0;
    DwmSetWindowAttribute(hwnd, 33, &corner, sizeof(corner));
    {
        std::lock_guard<std::mutex> lock(g_acrylicWindowsMutex);
        if (!g_unloading.load(std::memory_order_relaxed))
            g_acrylicWindows.insert(hwnd);
    }
}

// Blend a premultiplied 32-bit color onto the menu's alpha-aware GDI surface.
// This affects only the hovered row, leaving text and glyphs untouched.
bool AcrylicPaintHover(HDC dc, const RECT* rect) {
    if (!rect || !AcrylicIsPopupDC(dc)) return false;
    int opacity = g_acrylicHoverOpacity.load(std::memory_order_relaxed);
    if (opacity <= 0) return true; // Intentionally no selection fill.

    // All geometry is scaled by the menu's DC DPI. The native popup retains
    // its original hit testing and layout; only the highlight is inset.
    int dpi = GetDeviceCaps(dc, LOGPIXELSX);
    if (dpi <= 0) dpi = 96;
    auto toPixels = [dpi](int dip) { return (dip * dpi + 48) / 96; };
    int insetX = toPixels(g_acrylicHoverInset.load(std::memory_order_relaxed));
    int insetY = toPixels(g_acrylicHoverVerticalInset.load(std::memory_order_relaxed));
    RECT hover = *rect;
    hover.left += insetX;
    hover.right -= insetX;
    hover.top += insetY;
    hover.bottom -= insetY;
    int width = hover.right - hover.left;
    int height = hover.bottom - hover.top;
    if (width <= 0 || height <= 0 || width > 8192 || height > 2048) return true;
    float radius = static_cast<float>(toPixels(
        g_acrylicHoverRadius.load(std::memory_order_relaxed)));
    if (radius > width * 0.5f) radius = width * 0.5f;
    if (radius > height * 0.5f) radius = height * 0.5f;

    HDC buffer = CreateCompatibleDC(dc);
    if (!buffer) return false;
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* pixels = nullptr;
    HBITMAP bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS,
                                      &pixels, nullptr, 0);
    if (!bitmap || !pixels) {
        if (bitmap) DeleteObject(bitmap);
        DeleteDC(buffer);
        return false;
    }
    HGDIOBJ previous = SelectObject(buffer, bitmap);
    COLORREF color = g_acrylicHoverColor.load(std::memory_order_relaxed);
    BYTE baseAlpha = static_cast<BYTE>((opacity * 255 + 50) / 100);
    DWORD* output = static_cast<DWORD*>(pixels);

    // Four subpixel samples on rounded corners only; interior pixels are
    // uniform. Premultiplied alpha keeps the Acrylic backdrop visible.
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float coverage = 0.0f;
            for (int sy = 0; sy < 2; ++sy) {
                for (int sx = 0; sx < 2; ++sx) {
                    float px = x + (sx ? 0.75f : 0.25f);
                    float py = y + (sy ? 0.75f : 0.25f);
                    float cx = px < radius ? radius :
                               px > width - radius ? width - radius : px;
                    float cy = py < radius ? radius :
                               py > height - radius ? height - radius : py;
                    float dx = px - cx, dy = py - cy;
                    if (radius == 0.0f || dx * dx + dy * dy <= radius * radius)
                        coverage += 0.25f;
                }
            }
            DWORD a = static_cast<DWORD>(baseAlpha * coverage + 0.5f);
            DWORD r = GetRValue(color) * a / 255;
            DWORD g = GetGValue(color) * a / 255;
            DWORD b = GetBValue(color) * a / 255;
            output[static_cast<size_t>(y) * width + x] =
                (a << 24) | (r << 16) | (g << 8) | b;
        }
    }
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    BOOL ok = AlphaBlend(dc, hover.left, hover.top, width, height,
                         buffer, 0, 0, width, height, blend);
    SelectObject(buffer, previous);
    DeleteObject(bitmap);
    DeleteDC(buffer);
    return ok != FALSE;
}

// Only target GDI fills using an actual menu background brush on a verified
// native menu popup. This avoids clearing panel backgrounds, glyphs or icons.
bool AcrylicNativeBackgroundBrush(HBRUSH brush) {
    if (!brush) return false;
    ULONG_PTR sysIndex = reinterpret_cast<ULONG_PTR>(brush) - 1;
    if (sysIndex <= COLOR_MENUBAR)
        return sysIndex == COLOR_MENU || sysIndex == COLOR_MENUBAR;
    if (brush == GetStockObject(BLACK_BRUSH) ||
        brush == g_hMenuBrush.load(std::memory_order_acquire) ||
        brush == GetSysColorBrush(COLOR_MENU)) return true;
    LOGBRUSH info{};
    if (GetObjectW(brush, sizeof(info), &info) != sizeof(info) ||
        info.lbStyle != BS_SOLID) return false;
    COLORREF color = info.lbColor;
    return color == g_colMenu.load(std::memory_order_relaxed) ||
           color == GetSysColor(COLOR_MENU);
}

bool AcrylicNativeHoverBrush(HBRUSH brush) {
    ULONG_PTR index = reinterpret_cast<ULONG_PTR>(brush) - 1;
    return index == COLOR_HIGHLIGHT ||
           brush == g_hHighlightBrush.load(std::memory_order_acquire);
}

// This hook is deliberately limited to native, not owner-drawn, popup rows.
// Owner-drawn Photoshop painting has its own original color-override scope.
bool AcrylicClearNativeFill(HDC dc, const RECT* rect, HBRUSH brush) {
    if (!InAcrylicPopupPaintContext() || !rect || tl_ourPaintDepth || tl_menuDrawDepth > 0 ||
        tl_comboListDepth > 0 || !AcrylicIsEnabled() ||
        !g_acrylicClearOpaqueRows.load(std::memory_order_relaxed) ||
        !AcrylicIsPopupDC(dc)) return false;
    int w = rect->right - rect->left;
    int h = rect->bottom - rect->top;
    if (w < 8 || h < 8 || w > 8192 || h > 8192) return false;
    bool isBackground = AcrylicNativeBackgroundBrush(brush);
    bool isHover = !isBackground && AcrylicNativeHoverBrush(brush);
    if (!isBackground && !isHover) return false;
    // BLACKNESS zeros color bits; transparency depends on the target surface.
    // Release the reentrancy guard before painting the translucent hover.
    {
        DepthGuard guard(&tl_ourPaintDepth);
        if (!PatBlt(dc, rect->left, rect->top, w, h, BLACKNESS)) return false;
    }
    if (isHover) AcrylicPaintHover(dc, rect);
    return true;
}

// Preserve modern menu text's alpha. DrawThemeText may paint only RGB into
// a transparent DIB, making the label invisible after compositing.
bool AcrylicCompositedThemeText(HTHEME theme, HDC dc, int part, int state,
                               PCWSTR text, int count, DWORD flags,
                               RECT* rect, const DTTOPTS* sourceOptions,
                               HRESULT* result,
                               HRESULT (WINAPI* original)(HTHEME,HDC,int,int,
                                   LPCWSTR,int,DWORD,LPRECT,const DTTOPTS*)) {
    if (!rect || IsRectEmpty(rect) || (flags & DT_CALCRECT) || !original ||
        !AcrylicIsPopupDC(dc) || !AcrylicIsMenuTheme(theme)) return false;
    if (part != 14 /* MENU_POPUPITEM */ && part != 27 /* focusable item */) return false;
    if (sourceOptions && (sourceOptions->dwSize != sizeof(DTTOPTS) ||
        (sourceOptions->dwFlags & (DTT_CALCRECT | DTT_CALLBACK)))) return false;

    if (FAILED(BufferedPaintInit())) return false;
    BP_PAINTPARAMS paint{sizeof(paint)};
    paint.dwFlags = BPPF_ERASE;
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    paint.pBlendFunction = &blend;
    HDC bufferDC = nullptr;
    HPAINTBUFFER buffer = BeginBufferedPaint(dc, rect, BPBF_TOPDOWNDIB,
                                            &paint, &bufferDC);
    if (!buffer) { BufferedPaintUnInit(); return false; }
    HGDIOBJ font = GetCurrentObject(dc, OBJ_FONT);
    if (font) SelectObject(bufferDC, font);
    DTTOPTS options{};
    if (sourceOptions) options = *sourceOptions;
    options.dwSize = sizeof(options);
    options.dwFlags |= DTT_COMPOSITED | DTT_TEXTCOLOR;
    COLORREF color = g_colText.load(std::memory_order_relaxed);
    if (state == 3 || state == 4) color = g_colGray.load(std::memory_order_relaxed);
    else if (state == 2) color = g_colHiText.load(std::memory_order_relaxed);
    options.crText = color;
    RECT bounds = *rect;
    *result = original(theme, bufferDC, part, state, text, count,
                       flags, &bounds, &options);
    HRESULT end = EndBufferedPaint(buffer, SUCCEEDED(*result));
    BufferedPaintUnInit();
    return SUCCEEDED(*result) && SUCCEEDED(end);
}

// Owner-drawn Photoshop menu labels are plain GDI. GDI may write RGB but
// leave alpha=0 in a composited popup; buffer them separately as a white mask
// and premultiply the actual text color, preserving disabled/selected colors.
template <typename DrawFn>
bool AcrylicDrawOwnerText(HDC target, RECT* rect, DWORD flags,
                          DrawFn draw, int* output) {
    if (tl_acrylicDrawing || !AcrylicIsEnabled() || !MenuIsOpen() ||
        tl_menuDrawDepth <= 0 || !AcrylicIsPopupDC(target) ||
        !IsPaintContext(target, kMenuPopupClass) ||
        !rect || IsRectEmpty(rect) || (flags & (DT_CALCRECT | DT_MODIFYSTRING)))
        return false;
    int width = rect->right - rect->left;
    int height = rect->bottom - rect->top;
    if (width < 1 || height < 1 || width > 8192 || height > 2048) return false;

    if (FAILED(BufferedPaintInit())) return false;
    BP_PAINTPARAMS params{sizeof(params)};
    params.dwFlags = BPPF_ERASE;
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    params.pBlendFunction = &blend;
    HDC bufferDC = nullptr;
    HPAINTBUFFER buffer = BeginBufferedPaint(target, rect, BPBF_TOPDOWNDIB,
                                            &params, &bufferDC);
    if (!buffer) { BufferedPaintUnInit(); return false; }
    DepthGuard guard(&tl_acrylicDrawing);
    COLORREF intendedColor = GetTextColor(target);
    SetTextColor(bufferDC, RGB(255,255,255));
    SetBkMode(bufferDC, TRANSPARENT);
    HGDIOBJ font = GetCurrentObject(target, OBJ_FONT);
    if (font) SelectObject(bufferDC, font);
    RECT copy = *rect;
    int result = draw(bufferDC, &copy);
    RGBQUAD* pixels = nullptr;
    int stride = 0;
    bool okay = result > 0 && SUCCEEDED(GetBufferedPaintBits(buffer,
                                               &pixels, &stride)) && pixels;
    if (okay) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                RGBQUAD& p = pixels[static_cast<size_t>(y) * stride + x];
                int alpha = p.rgbRed > p.rgbGreen ? p.rgbRed : p.rgbGreen;
                if (p.rgbBlue > alpha) alpha = p.rgbBlue;
                p.rgbReserved = static_cast<BYTE>(alpha);
                p.rgbRed = static_cast<BYTE>(GetRValue(intendedColor) * alpha / 255);
                p.rgbGreen = static_cast<BYTE>(GetGValue(intendedColor) * alpha / 255);
                p.rgbBlue = static_cast<BYTE>(GetBValue(intendedColor) * alpha / 255);
            }
        }
    }
    HRESULT end = EndBufferedPaint(buffer, okay);
    BufferedPaintUnInit();
    if (okay && SUCCEEDED(end)) {
        *output = result;
        return true;
    }
    return false;
}

// Native menu areas with no selected item must write zero alpha rather than
// painting a solid rectangle over the underlying Acrylic backdrop.
bool AcrylicThemeBackground(HTHEME theme, HDC dc, int part, int state,
                            const RECT* rect, const RECT* clip) {
    if (!InAcrylicPopupPaintContext() || !rect || IsRectEmpty(rect) ||
        !AcrylicIsMenuTheme(theme) || !AcrylicIsPopupDC(dc)) return false;
    RECT area = *rect;
    if (clip && !IntersectRect(&area, rect, clip)) return true;
    if ((part == 14 || part == 27) && (state == 2 || state == 4)) {
        {
            DepthGuard guard(&tl_ourPaintDepth);
            if (!PatBlt(dc, area.left, area.top, area.right - area.left,
                        area.bottom - area.top, BLACKNESS)) return false;
        }
        // Keep the geometry relative to the full row during a partial repaint.
        int saved = SaveDC(dc);
        if (!saved) return false;
        if (clip) IntersectClipRect(dc, clip->left, clip->top, clip->right, clip->bottom);
        bool painted = AcrylicPaintHover(dc, rect);
        RestoreDC(dc, saved);
        return painted;
    }
    switch (part) {
        case 9:  // MENU_POPUPBACKGROUND
        case 10: // MENU_POPUPBORDERS
        case 13: // MENU_POPUPGUTTER
            break;
        case 14: // MENU_POPUPITEM, when not selected
        case 27: // newer Windows focusable popup item
            if (state == 2 || state == 4) return false;
            break;
        default:
            return false;
    }
    if (part == 27) {
        static std::atomic<bool> loggedFocusable{false};
        if (!loggedFocusable.exchange(true, std::memory_order_relaxed))
            Wh_Log(L"[Adobe Acrylic] Cleared focusable menu item part=27 state=%d dcType=%lu", state, GetObjectType(dc));
    }
    static std::atomic<bool> loggedFirstThemedClear{false};
    if (!loggedFirstThemedClear.exchange(true, std::memory_order_relaxed))
        Wh_Log(L"[Adobe Acrylic] First native themed menu background intercepted.");
    DepthGuard guard(&tl_ourPaintDepth);
    return !!PatBlt(dc, area.left, area.top,
                    area.right - area.left, area.bottom - area.top, BLACKNESS);
}

LRESULT CALLBACK MenuPopupSubclassProc(HWND hWnd,
                                       UINT uMsg,
                                       WPARAM wParam,
                                       LPARAM lParam,
                                       [[maybe_unused]] DWORD_PTR dwRefData) {
    if (ProcessIsExiting()) return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    // MN_SIZEWINDOW (0x01E2) is the reliable post-layout point: the menu
    // handle is attached but user32 hasn't presented its first frame.
    if (uMsg == 0x01E2) {
        LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
        AcrylicPreparePopup(hWnd);
        return result;
    }
    // Fallback for popups whose menu layout skips MN_SIZEWINDOW.
    if (uMsg == WM_WINDOWPOSCHANGING && lParam &&
        (reinterpret_cast<const WINDOWPOS*>(lParam)->flags & SWP_SHOWWINDOW)) {
        TrackMenuPopupVisibility(hWnd, true);
        AcrylicPreparePopup(hWnd);
    }
    if (uMsg == WM_WINDOWPOSCHANGED && lParam &&
        (reinterpret_cast<const WINDOWPOS*>(lParam)->flags &
         (SWP_SHOWWINDOW | SWP_HIDEWINDOW))) {
        TrackMenuPopupVisibility(hWnd, IsWindowVisible(hWnd));
    }
    if (uMsg == WM_SHOWWINDOW) TrackMenuPopupVisibility(hWnd, wParam != FALSE);
    if (uMsg == WM_NCDESTROY) {
        TrackMenuPopupVisibility(hWnd, false);
        if (!ProcessIsExiting()) {
            std::lock_guard<std::mutex> lock(g_acrylicWindowsMutex);
            g_acrylicWindows.erase(hWnd);
        }
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

    // Scope native WM_PAINT/WM_PRINT drawing to this verified popup. In
    // Lightroom, exported TrackPopupMenu hooks are not always called.
    bool paintsPopup = (uMsg == WM_PAINT || uMsg == WM_PRINT ||
                        uMsg == WM_PRINTCLIENT || uMsg == WM_ERASEBKGND ||
                        uMsg == WM_NCPAINT) && AcrylicPopupIsActive(hWnd);
    if (paintsPopup) ++tl_acrylicPopupPaintDepth;
    LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
    if (paintsPopup) --tl_acrylicPopupPaintDepth;

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

// Experimental: captioned Lightroom secondary windows, never its main frame.
// Opt in through the setting; no system-wide colors or window alpha changes.
std::atomic<bool> g_darkPreferences{false};
std::atomic<bool> g_preferencesAcrylicEnabled{false};
std::atomic<bool> g_allDialogAcrylicEnabled{false};
std::atomic<int> g_preferencesTintOpacity{45};
std::atomic<COLORREF> g_preferencesTint{RGB(45,45,45)};
bool g_isLightroomProcess = false;
std::mutex g_preferencesAcrylicMutex;
std::unordered_map<HWND, AcrylicAccentPolicy> g_dialogAcrylicWindows;

bool IsPreferencesAcrylicSurface(HWND hwnd) {
    if (!hwnd || ProcessIsExiting()) return false;
    HWND root = GetAncestor(hwnd, GA_ROOT);
    std::lock_guard<std::mutex> lock(g_preferencesAcrylicMutex);
    return root && g_dialogAcrylicWindows.contains(root);
}

void ResetPreferencesAcrylicWindow(HWND root, bool redraw) {
    if (ProcessIsExiting()) return;
    AcrylicAccentPolicy original{};
    {
        std::lock_guard<std::mutex> lock(g_preferencesAcrylicMutex);
        auto found = g_dialogAcrylicWindows.find(root);
        if (found == g_dialogAcrylicWindows.end()) return;
        original = found->second;
        g_dialogAcrylicWindows.erase(found);
    }
    if (root && IsWindow(root) && g_acrylicSetComposition) {
        AcrylicCompositionData data{19, &original, sizeof(original)};
        g_acrylicSetComposition(root, &data);
        if (redraw) RedrawWindow(root, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
    }
}

void ResetPreferencesAcrylic() {
    if (ProcessIsExiting()) return;
    std::unordered_map<HWND, AcrylicAccentPolicy> windows;
    {
        std::lock_guard<std::mutex> lock(g_preferencesAcrylicMutex);
        windows.swap(g_dialogAcrylicWindows);
    }
    for (auto& entry : windows) {
        if (!IsWindow(entry.first) || !g_acrylicSetComposition) continue;
        AcrylicCompositionData data{19, &entry.second, sizeof(entry.second)};
        g_acrylicSetComposition(entry.first, &data);
        RedrawWindow(entry.first, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
    }
}

void PreparePreferencesAcrylic(HWND root) {
    if (ProcessIsExiting() || g_unloading.load(std::memory_order_relaxed) ||
        !g_darkPreferences.load(std::memory_order_relaxed) ||
        !g_preferencesAcrylicEnabled.load(std::memory_order_relaxed) ||
        !g_acrylicSetComposition) return;
    WCHAR title[128]{};
    bool isPreferences = GetWindowTextW(root, title, ARRAYSIZE(title)) && !_wcsicmp(title, L"Preferences");
    if (!isPreferences && !g_allDialogAcrylicEnabled.load(std::memory_order_relaxed)) return;
    {
        std::lock_guard<std::mutex> lock(g_preferencesAcrylicMutex);
        if (g_dialogAcrylicWindows.contains(root)) return;
    }
    AcrylicAccentPolicy original{};
    AcrylicCompositionData previous{19, &original, sizeof(original)};
    if (!g_acrylicGetComposition || !g_acrylicGetComposition(root, &previous)) {
        // WCA_ACCENT_POLICY is not readable on some Windows versions. Use the
        // default disabled accent for best-effort cleanup rather than silently
        // skipping the user's opt-in experiment.
        original = {};
        Wh_Log(L"[Adobe Acrylic] Preferences accent unavailable; cleanup will restore the default accent.");
    }
    int opacity = g_preferencesTintOpacity.load(std::memory_order_relaxed);
    DWORD alpha = static_cast<DWORD>((opacity * 255 + 50) / 100);
    DWORD color = (alpha << 24) | g_preferencesTint.load(std::memory_order_relaxed);
    AcrylicAccentPolicy accent{4, 0, color, 0};
    AcrylicCompositionData data{19, &accent, sizeof(accent)};
    SetLastError(0);
    if (!g_acrylicSetComposition(root, &data)) {
        Wh_Log(L"Lightroom dialog acrylic composition was rejected, error=%lu.", GetLastError());
        return;
    }
    bool abort = false;
    {
        std::lock_guard<std::mutex> lock(g_preferencesAcrylicMutex);
        abort = g_unloading.load(std::memory_order_relaxed);
        if (!abort) g_dialogAcrylicWindows.emplace(root, original);
    }
    if (abort) {
        AcrylicCompositionData restore{19, &original, sizeof(original)};
        g_acrylicSetComposition(root, &restore);
        return;
    }
    Wh_Log(L"[Adobe Acrylic] Preferences acrylic applied, opacity=%d", opacity);
}
std::mutex g_preferencesMutex;
std::unordered_set<HWND> g_preferencesWindows;
struct DialogListColors { COLORREF background; COLORREF text; COLORREF textBackground; };
std::unordered_map<HWND, DialogListColors> g_dialogListColors;
thread_local bool tl_preparingPreferences = false;
thread_local HWND tl_mfcGroupPaint = nullptr;
thread_local HWND tl_optionsPanelPaint = nullptr;
thread_local HDC tl_dialogListItemDC = nullptr;
thread_local HWND tl_preferencesControlPaint = nullptr;
thread_local COLORREF tl_dialogListText = RGB(235,235,235);
std::atomic<HWND> g_watermarkOptionsPanel{nullptr};

// Lightroom also uses AfxWnd140u for the image preview and color swatch, so
// identify only the options tree and only its non-leaf MFC containers.
bool IsWatermarkOptionsContainer(HWND hwnd) {
    HWND panel = g_watermarkOptionsPanel.load(std::memory_order_relaxed);
    if (!hwnd || !panel || !g_darkPreferences.load(std::memory_order_relaxed) ||
        (hwnd != panel && !IsChild(panel, hwnd)) || !GetWindow(hwnd, GW_CHILD)) return false;
    WCHAR cls[64]{};
    return GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) && !_wcsicmp(cls, L"AfxWnd140u");
}

void FindWatermarkOptionsPanel(HWND root) {
    WCHAR title[128]{};
    GetWindowTextW(root, title, ARRAYSIZE(title));
    bool pluginManager = !_wcsicmp(title, L"Lightroom Plug-in Manager");
    if (!pluginManager && _wcsicmp(title, L"Watermark Editor") && _wcsnicmp(title, L"Export", 6)) return;
    HWND panel = nullptr;
    for (HWND child = GetWindow(root, GW_CHILD); child; child = GetWindow(child, GW_HWNDNEXT)) {
        WCHAR cls[64]{};
        if (!GetClassNameW(child, cls, ARRAYSIZE(cls)) || _wcsicmp(cls, L"AfxWnd140u")) continue;
        struct PanelControls { bool slider; bool scrollbar; bool list; } controls{};
        EnumChildWindows(child, [](HWND descendant, LPARAM data) WINAPI -> BOOL {
            auto* found = reinterpret_cast<PanelControls*>(data);
            WCHAR childClass[64]{};
            if (GetClassNameW(descendant, childClass, ARRAYSIZE(childClass))) {
                if (!_wcsicmp(childClass, L"msctls_trackbar32")) found->slider = true;
                else if (!_wcsicmp(childClass, L"ScrollBar")) found->scrollbar = true;
                else if (!_wcsicmp(childClass, L"SysListView32")) found->list = true;
            }
            return TRUE;
        }, reinterpret_cast<LPARAM>(&controls));
        if (controls.slider || (pluginManager && controls.scrollbar && !controls.list)) {
            panel = child;
            break;
        }
    }
    g_watermarkOptionsPanel.store(panel, std::memory_order_relaxed);
    Wh_Log(L"[Adobe Acrylic] Options panel hwnd=%p", panel);
}

bool IsLightroomSecondaryWindow(HWND hwnd) {
    if (!hwnd || !g_isLightroomProcess ||
        !g_darkPreferences.load(std::memory_order_relaxed)) return false;
    if (GetAncestor(hwnd, GA_ROOT) != hwnd) return false;
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if ((style & WS_CHILD) || (style & WS_CAPTION) != WS_CAPTION) return false;
    WCHAR cls[128]{};
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) return false;
    // The main Lightroom editing frame must never be recolored. Popup menus,
    // tooltips and captionless transient windows also remain outside this pass.
    if (!_wcsicmp(cls, L"AgWinMainFrame") || !_wcsicmp(cls, L"#32768") ||
        !_wcsicmp(cls, L"tooltips_class32")) return false;
    return true;
}

bool IsPreferencesGroupBox(HWND hwnd) {
    WCHAR cls[32]{};
    return GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) && !_wcsicmp(cls, L"Button") &&
           (GetWindowLongPtrW(hwnd, GWL_STYLE) & BS_TYPEMASK) == BS_GROUPBOX;
}

bool IsDialogMfcGroupContainer(HWND hwnd) {
    if (!hwnd || !g_darkPreferences.load(std::memory_order_relaxed) || ProcessIsExiting()) return false;
    {
        std::lock_guard<std::mutex> lock(g_preferencesMutex);
        if (!g_preferencesWindows.contains(hwnd)) return false;
    }
    WCHAR cls[64]{};
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) || _wcsicmp(cls, L"AfxWnd140u")) return false;
    if (IsPreferencesGroupBox(GetParent(hwnd))) return true;
    // Copy Settings hosts the masking checkboxes in several nested MFC
    // containers inside the main group box. These do not contain photo previews.
    HWND root = GetAncestor(hwnd, GA_ROOT);
    WCHAR title[64]{};
    if (!root || !GetWindowTextW(root, title, ARRAYSIZE(title)) || _wcsicmp(title, L"Copy Settings")) return false;
    for (HWND parent = GetParent(hwnd); parent && parent != root; parent = GetParent(parent))
        if (IsPreferencesGroupBox(parent) && GetParent(parent) == root) return true;
    return false;
}

bool IsWatermarkHeaderLabel(HWND hwnd) {
    HWND parent = GetParent(hwnd);
    if (!parent || !IsWatermarkOptionsContainer(parent) ||
        !(GetWindowLongPtrW(parent, GWL_STYLE) & WS_CLIPCHILDREN)) return false;
    WCHAR cls[32]{};
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) || _wcsicmp(cls, L"Static")) return false;
    RECT parentRect{};
    if (!GetClientRect(parent, &parentRect)) return false;
    HDC screen = GetDC(parent);
    int dpi = screen ? GetDeviceCaps(screen, LOGPIXELSY) : 96;
    if (screen) ReleaseDC(parent, screen);
    if (dpi <= 0) dpi = 96;
    return parentRect.bottom > 0 && parentRect.bottom <= MulDiv(64, dpi, 96);
}

bool IsOptionsHeaderArrow(HWND hwnd) {
    HWND parent = GetParent(hwnd);
    if (!parent || !IsWatermarkOptionsContainer(parent) ||
        !(GetWindowLongPtrW(parent, GWL_STYLE) & WS_CLIPCHILDREN)) return false;
    WCHAR cls[32]{};
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) || _wcsicmp(cls, L"Button") ||
        (GetWindowLongPtrW(hwnd, GWL_STYLE) & BS_TYPEMASK) != BS_PUSHBUTTON ||
        GetWindowTextLengthW(hwnd)) return false;
    RECT parentRect{}, childRect{};
    if (!GetClientRect(parent, &parentRect) || !GetClientRect(hwnd, &childRect)) return false;
    HDC dc = GetDC(hwnd);
    int dpi = dc ? GetDeviceCaps(dc, LOGPIXELSY) : 96;
    if (dc) ReleaseDC(hwnd, dc);
    if (dpi <= 0) dpi = 96;
    return parentRect.bottom > 0 && parentRect.bottom <= MulDiv(64, dpi, 96) &&
           childRect.right > 0 && childRect.bottom > 0 &&
           childRect.right <= MulDiv(40, dpi, 96) &&
           childRect.bottom <= MulDiv(40, dpi, 96);
}

// Keep the application's arrow and interaction; recolor its light neutral
// button background after its native paint. The scope excludes the preview,
// color swatch, normal buttons and other application windows.
void DarkenOptionsHeaderArrow(HWND hwnd) {
    RECT rc{};
    if (!GetClientRect(hwnd, &rc)) return;
    int width = rc.right, height = rc.bottom;
    if (width <= 0 || height <= 0 || width > 256 || height > 256) return;
    HDC target = GetDC(hwnd);
    if (!target) return;
    HDC memory = CreateCompatibleDC(target);
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bitmap = memory ? CreateDIBSection(target, &bmi, DIB_RGB_COLORS,
                                                &bits, nullptr, 0) : nullptr;
    HGDIOBJ old = bitmap ? SelectObject(memory, bitmap) : nullptr;
    if (old && old != HGDI_ERROR && bits &&
        BitBlt(memory, 0, 0, width, height, target, 0, 0, SRCCOPY) && GdiFlush()) {
        DWORD* pixels = static_cast<DWORD*>(bits);
        for (int i = 0; i < width * height; ++i) {
            DWORD p = pixels[i];
            int r = (p >> 16) & 255, g = (p >> 8) & 255, b = p & 255;
            if (abs(r-g) <= 12 && abs(g-b) <= 12 && abs(r-b) <= 12) {
                if (r >= 200 && g >= 200 && b >= 200)
                    pixels[i] = (p & 0xff000000) | 0x00262626;
                else if (r >= 65 && r < 180 && g >= 65 && g < 180 && b >= 65 && b < 180)
                    pixels[i] = (p & 0xff000000) | 0x00c8c8c8;
            }
        }
        BitBlt(target, 0, 0, width, height, memory, 0, 0, SRCCOPY);
    }
    if (old && old != HGDI_ERROR) SelectObject(memory, old);
    if (bitmap) DeleteObject(bitmap);
    if (memory) DeleteDC(memory);
    ReleaseDC(hwnd, target);
}

void PaintWatermarkHeaderLabel(HWND hwnd, HDC dc) {
    RECT rc{};
    if (!dc || !GetClientRect(hwnd, &rc)) return;
    int saved = SaveDC(dc);
    if (!saved) return;
    DepthGuard guard(&tl_ourPaintDepth);
    HBRUSH background = CreateSolidBrush(IsPreferencesAcrylicSurface(hwnd)
        ? RGB(0, 0, 0) : RGB(38, 38, 38));
    if (!background) { RestoreDC(dc, saved); return; }
    FillRect(dc, &rc, background);
    HFONT font = reinterpret_cast<HFONT>(SendMessageW(hwnd, WM_GETFONT, 0, 0));
    if (font) SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, IsWindowEnabled(hwnd) ? RGB(235, 235, 235) : RGB(150, 150, 150));
    WCHAR label[256]{};
    int length = GetWindowTextW(hwnd, label, ARRAYSIZE(label));
    if (length > 0) DrawTextW(dc, label, length, &rc, DT_SINGLELINE | DT_VCENTER | DT_LEFT);
    DeleteObject(background);
    RestoreDC(dc, saved);
}

void PaintPreferencesGroupBox(HWND hwnd, HDC dc) {
    if (!dc) return;
    RECT rc{};
    if (!GetClientRect(hwnd, &rc) || IsRectEmpty(&rc)) return;
    int saved = SaveDC(dc);
    if (!saved) return;
    DepthGuard guard(&tl_ourPaintDepth);
    HBRUSH bg = CreateSolidBrush(IsPreferencesAcrylicSurface(hwnd) ? RGB(0,0,0) : RGB(38,38,38));
    HBRUSH border = CreateSolidBrush(RGB(90, 90, 90));
    if (!bg || !border) {
        if (bg) DeleteObject(bg);
        if (border) DeleteObject(border);
        RestoreDC(dc, saved);
        return;
    }
    FillRect(dc, &rc, bg);
    HFONT font = reinterpret_cast<HFONT>(SendMessageW(hwnd, WM_GETFONT, 0, 0));
    if (font) SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, IsWindowEnabled(hwnd) ? RGB(235, 235, 235) : RGB(150, 150, 150));
    WCHAR label[256]{};
    int length = GetWindowTextW(hwnd, label, ARRAYSIZE(label));
    TEXTMETRICW metrics{};
    GetTextMetricsW(dc, &metrics);
    int margin = MulDiv(8, GetDeviceCaps(dc, LOGPIXELSX), 96);
    RECT textRect{rc.left + margin, rc.top, rc.right - margin, rc.bottom};
    if (length > 0) {
        RECT measured = textRect;
        DrawTextW(dc, label, length, &measured, DT_SINGLELINE | DT_CALCRECT);
        RECT borderRect{rc.left, rc.top + metrics.tmHeight / 2, rc.right, rc.bottom};
        int local = SaveDC(dc);
        if (local) {
            ExcludeClipRect(dc, measured.left - 2, rc.top, measured.right + 2,
                            rc.top + metrics.tmHeight + 2);
            FrameRect(dc, &borderRect, border);
            RestoreDC(dc, local);
        }
        DrawTextW(dc, label, length, &textRect, DT_SINGLELINE | DT_TOP | DT_LEFT);
    } else {
        FrameRect(dc, &rc, border);
    }
    DeleteObject(border);
    DeleteObject(bg);
    RestoreDC(dc, saved);
}

// Header items are painted through their native custom-draw notification;
// resizing, reordering, hit testing and the list contents remain native.
bool DrawDialogListHeader(HWND parent, LPARAM param, LRESULT* result) {
    if (!param || !result || ProcessIsExiting()) return false;
    auto* info = reinterpret_cast<NMCUSTOMDRAW*>(param);
    if (info->hdr.code != NM_CUSTOMDRAW || !info->hdr.hwndFrom ||
        GetParent(info->hdr.hwndFrom) != parent) return false;
    WCHAR cls[64]{};
    if (!GetClassNameW(info->hdr.hwndFrom, cls, ARRAYSIZE(cls)) ||
        _wcsicmp(cls, L"SysHeader32")) return false;
    {
        std::lock_guard<std::mutex> lock(g_preferencesMutex);
        if (!g_preferencesWindows.contains(info->hdr.hwndFrom)) return false;
    }
    if (info->dwDrawStage == CDDS_PREPAINT) {
        *result = CDRF_NOTIFYITEMDRAW;
        return true;
    }
    if (info->dwDrawStage != CDDS_ITEMPREPAINT || !info->hdc) return false;
    WCHAR text[512]{};
    HDITEMW item{};
    item.mask = HDI_TEXT | HDI_FORMAT;
    item.pszText = text;
    item.cchTextMax = ARRAYSIZE(text);
    if (!SendMessageW(info->hdr.hwndFrom, HDM_GETITEMW, info->dwItemSpec,
                      reinterpret_cast<LPARAM>(&item)) ||
        (item.fmt & (HDF_BITMAP | HDF_IMAGE | HDF_OWNERDRAW))) return false;
    int saved = SaveDC(info->hdc);
    if (!saved) return false;
    DepthGuard guard(&tl_ourPaintDepth);
    HBRUSH bg = CreateSolidBrush(RGB(45,45,45));
    HBRUSH line = CreateSolidBrush(RGB(75,75,75));
    if (!bg || !line) {
        if (bg) DeleteObject(bg);
        if (line) DeleteObject(line);
        RestoreDC(info->hdc, saved);
        return false;
    }
    FillRect(info->hdc, &info->rc, bg);
    RECT divider{info->rc.right - 1, info->rc.top, info->rc.right, info->rc.bottom};
    FillRect(info->hdc, &divider, line);
    SetBkMode(info->hdc, TRANSPARENT);
    SetTextColor(info->hdc, RGB(235,235,235));
    RECT label = info->rc;
    label.left += 6;
    label.right -= 6;
    if (item.fmt & (HDF_SORTUP | HDF_SORTDOWN)) label.right -= 14;
    UINT format = DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX;
    format |= (item.fmt & HDF_RIGHT) ? DT_RIGHT :
              (item.fmt & HDF_CENTER) ? DT_CENTER : DT_LEFT;
    DrawTextW(info->hdc, text, -1, &label, format);
    if (item.fmt & (HDF_SORTUP | HDF_SORTDOWN)) {
        int x = info->rc.right - 8;
        int y = (info->rc.top + info->rc.bottom) / 2;
        POINT triangle[3] = {{x - 4, y - 2}, {x + 4, y - 2}, {x, y + 3}};
        if (item.fmt & HDF_SORTUP) {
            triangle[0].y = triangle[1].y = y + 2;
            triangle[2].y = y - 3;
        }
        HBRUSH arrow = CreateSolidBrush(RGB(200,200,200));
        if (arrow) {
            SelectObject(info->hdc, arrow);
            SelectObject(info->hdc, GetStockObject(NULL_PEN));
            Polygon(info->hdc, triangle, ARRAYSIZE(triangle));
            SelectObject(info->hdc, GetStockObject(NULL_BRUSH));
            DeleteObject(arrow);
        }
    }
    DeleteObject(line);
    DeleteObject(bg);
    RestoreDC(info->hdc, saved);
    *result = CDRF_SKIPDEFAULT;
    return true;
}

LRESULT CALLBACK PreferencesDarkSubclass(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
                                         DWORD_PTR) {
    if (ProcessIsExiting()) return DefSubclassProc(hwnd, msg, wp, lp);
    if (msg == WM_NCDESTROY) {
        if (!ProcessIsExiting()) ResetPreferencesAcrylicWindow(hwnd, false);
        if (g_watermarkOptionsPanel.load(std::memory_order_relaxed) == hwnd)
            g_watermarkOptionsPanel.store(nullptr, std::memory_order_relaxed);
        if (!ProcessIsExiting()) {
            std::lock_guard<std::mutex> lock(g_preferencesMutex);
            g_preferencesWindows.erase(hwnd);
            g_dialogListColors.erase(hwnd);
        }
        return DefSubclassProc(hwnd, msg, wp, lp);
    }
    if (g_darkPreferences.load(std::memory_order_relaxed)) {
        LRESULT headerResult = 0;
        if (msg == WM_NOTIFY && DrawDialogListHeader(hwnd, lp, &headerResult))
            return headerResult;
        if (IsWatermarkHeaderLabel(hwnd)) {
            if (msg == WM_ERASEBKGND) return 1;
            if (msg == WM_PAINT) {
                PAINTSTRUCT ps{};
                HDC dc = BeginPaint(hwnd, &ps);
                if (dc) PaintWatermarkHeaderLabel(hwnd, dc);
                EndPaint(hwnd, &ps);
                return 0;
            }
            if (msg == WM_PRINTCLIENT && wp) {
                PaintWatermarkHeaderLabel(hwnd, reinterpret_cast<HDC>(wp));
                return 0;
            }
        }
        if (IsPreferencesGroupBox(hwnd)) {
            if (msg == WM_ERASEBKGND) return 1;
            if (msg == WM_PAINT) {
                PAINTSTRUCT ps{};
                HDC dc = BeginPaint(hwnd, &ps);
                if (dc) PaintPreferencesGroupBox(hwnd, dc);
                EndPaint(hwnd, &ps);
                return 0;
            }
            if (msg == WM_PRINTCLIENT && wp) {
                PaintPreferencesGroupBox(hwnd, reinterpret_cast<HDC>(wp));
                return 0;
            }
        }
        if (msg == WM_CTLCOLORDLG || msg == WM_CTLCOLORSTATIC ||
            msg == WM_CTLCOLOREDIT || msg == WM_CTLCOLORLISTBOX ||
            msg == WM_CTLCOLORBTN) {
            HDC dc = reinterpret_cast<HDC>(wp);
            HWND control = reinterpret_cast<HWND>(lp);
            if (dc) {
                SetTextColor(dc, control && !IsWindowEnabled(control)
                                     ? RGB(150,150,150) : RGB(240,240,240));
                SetBkColor(dc, RGB(38,38,38));
                if (msg == WM_CTLCOLORSTATIC || msg == WM_CTLCOLORBTN)
                    SetBkMode(dc, TRANSPARENT);
            }
            // Deliberately retained for the process lifetime: a dialog may
            // still be using a returned brush during mod unload.
            static HBRUSH clearBrush = CreateSolidBrush(RGB(0,0,0));
            if (msg == WM_CTLCOLORSTATIC && IsPreferencesAcrylicSurface(hwnd) && clearBrush)
                return reinterpret_cast<LRESULT>(clearBrush);
            static HBRUSH brush = CreateSolidBrush(RGB(38,38,38));
            if (brush) return reinterpret_cast<LRESULT>(brush);
        }
        WCHAR cls[64]{};
        if (msg == WM_ERASEBKGND) GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
        HWND parent = msg == WM_ERASEBKGND ? GetParent(hwnd) : nullptr;
        WCHAR parentClass[64]{};
        if (parent) GetClassNameW(parent, parentClass, ARRAYSIZE(parentClass));
        bool mfcContainer = msg == WM_ERASEBKGND && IsDialogMfcGroupContainer(hwnd);
        // This window is already a tracked dialog child. Limit the generic MFC
        // treatment to containers parented by group boxes, not photo previews.
        bool page = msg == WM_ERASEBKGND &&
                    (IsLightroomSecondaryWindow(hwnd) || !_wcsicmp(cls, L"#32770") ||
                     !_wcsicmp(cls, L"SysTabControl32") ||
                     !_wcsicmp(parentClass, L"SysTabControl32") || mfcContainer ||
                     IsWatermarkOptionsContainer(hwnd));
        if (page) {
            RECT rc{};
            if (GetClientRect(hwnd, &rc)) {
                if (IsPreferencesAcrylicSurface(hwnd)) {
                    DepthGuard guard(&tl_ourPaintDepth);
                    if (PatBlt(reinterpret_cast<HDC>(wp), rc.left, rc.top,
                               rc.right - rc.left, rc.bottom - rc.top, BLACKNESS)) return 1;
                }
                static HBRUSH brush = CreateSolidBrush(RGB(38,38,38));
                if (brush && FillRect(reinterpret_cast<HDC>(wp), &rc, brush)) return 1;
            }
        }
    }
    HDC previousListDC = tl_dialogListItemDC;
    COLORREF previousListText = tl_dialogListText;
    COLORREF savedListColor = CLR_INVALID;
    if (g_darkPreferences.load(std::memory_order_relaxed) && msg == WM_DRAWITEM && lp) {
        const auto* item = reinterpret_cast<const DRAWITEMSTRUCT*>(lp);
        if (item->CtlType == ODT_LISTVIEW && item->hDC && !(item->itemState & ODS_SELECTED)) {
            bool tracked = false;
            {
                std::lock_guard<std::mutex> lock(g_preferencesMutex);
                tracked = g_preferencesWindows.contains(item->hwndItem);
            }
            if (tracked) {
                tl_dialogListItemDC = item->hDC;
                tl_dialogListText = (item->itemState & (ODS_DISABLED | ODS_GRAYED))
                    ? RGB(150,150,150) : RGB(235,235,235);
                savedListColor = GetTextColor(item->hDC);
                SetTextColor(item->hDC, tl_dialogListText);
            }
        }
    }
    HWND previousMfcPaint = tl_mfcGroupPaint;
    HWND previousOptionsPaint = tl_optionsPanelPaint;
    HWND previousControlPaint = tl_preferencesControlPaint;
    if ((msg == WM_PAINT || msg == WM_PRINTCLIENT) && IsDialogMfcGroupContainer(hwnd))
        tl_mfcGroupPaint = hwnd;
    if ((msg == WM_PAINT || msg == WM_PRINTCLIENT) && IsWatermarkOptionsContainer(hwnd))
        tl_optionsPanelPaint = hwnd;
    if ((msg == WM_PAINT || msg == WM_PRINTCLIENT) && IsPreferencesAcrylicSurface(hwnd)) {
        WCHAR name[32]{};
        LONG_PTR type = GetWindowLongPtrW(hwnd, GWL_STYLE) & BS_TYPEMASK;
        if (GetClassNameW(hwnd, name, ARRAYSIZE(name)) &&
            ((!_wcsicmp(name, L"Button") && (type == BS_PUSHBUTTON || type == BS_DEFPUSHBUTTON)) ||
             !_wcsicmp(name, L"SysTabControl32") || !_wcsicmp(name, L"ComboBox")))
            tl_preferencesControlPaint = hwnd;
    }
    LRESULT result = DefSubclassProc(hwnd, msg, wp, lp);
    // Some dialogs receive their title after their first show/activation hook.
    if (msg == WM_SETTEXT && IsLightroomSecondaryWindow(hwnd) &&
        !IsPreferencesAcrylicSurface(hwnd)) {
        WCHAR title[64]{};
        if (GetWindowTextW(hwnd, title, ARRAYSIZE(title)) && !_wcsicmp(title, L"Preferences")) {
            PreparePreferencesAcrylic(hwnd);
            RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
        }
    }
    tl_mfcGroupPaint = previousMfcPaint;
    tl_optionsPanelPaint = previousOptionsPaint;
    tl_preferencesControlPaint = previousControlPaint;
    if (savedListColor != CLR_INVALID && tl_dialogListItemDC) {
        HDC restoreDC = tl_dialogListItemDC;
        tl_dialogListItemDC = previousListDC;
        tl_dialogListText = previousListText;
        DepthGuard guard(&tl_ourPaintDepth);
        SetTextColor(restoreDC, savedListColor);
    } else {
        tl_dialogListItemDC = previousListDC;
        tl_dialogListText = previousListText;
    }
    if (g_darkPreferences.load(std::memory_order_relaxed) && msg == WM_PAINT &&
        IsOptionsHeaderArrow(hwnd)) DarkenOptionsHeaderArrow(hwnd);
    if (g_darkPreferences.load(std::memory_order_relaxed) && msg == WM_NCPAINT &&
        (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_BORDER)) {
        WCHAR cls[64]{};
        GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
        bool containsList = false;
        if (!_wcsicmp(cls, L"AfxWnd140u")) {
            EnumChildWindows(hwnd, [](HWND child, LPARAM param) WINAPI -> BOOL {
                WCHAR name[64]{};
                if (GetClassNameW(child, name, ARRAYSIZE(name)) && !_wcsicmp(name, L"SysListView32")) {
                    *reinterpret_cast<bool*>(param) = true;
                    return FALSE;
                }
                return TRUE;
            }, reinterpret_cast<LPARAM>(&containsList));
        }
        if (containsList || IsWatermarkOptionsContainer(hwnd)) {
            RECT rc{};
            HDC dc = GetWindowDC(hwnd);
            if (dc && GetWindowRect(hwnd, &rc)) {
                OffsetRect(&rc, -rc.left, -rc.top);
                HBRUSH brush = CreateSolidBrush(RGB(75,75,75));
                if (brush) { FrameRect(dc, &rc, brush); DeleteObject(brush); }
            }
            if (dc) ReleaseDC(hwnd, dc);
        }
    }
    if (g_darkPreferences.load(std::memory_order_relaxed) && msg == WM_PAINT) {
        WCHAR cls[64]{};
        GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
        if (!_wcsicmp(cls, L"SysTabControl32") &&
            !(GetWindowLongPtrW(hwnd, GWL_STYLE) & (TCS_VERTICAL | TCS_BOTTOM))) {
            int count = TabCtrl_GetItemCount(hwnd);
            RECT last{}, client{};
            if (count > 0 && TabCtrl_GetItemRect(hwnd, count - 1, &last) &&
                GetClientRect(hwnd, &client) && last.right < client.right) {
                HDC dc = GetDC(hwnd);
                if (dc) {
                    RECT remaining{last.right + 1, client.top, client.right, last.bottom};
                    HBRUSH brush = CreateSolidBrush(IsPreferencesAcrylicSurface(hwnd)
                        ? RGB(0, 0, 0) : RGB(38, 38, 38));
                    if (brush) {
                        DepthGuard guard(&tl_ourPaintDepth);
                        FillRect(dc, &remaining, brush);
                        int dpi = GetDeviceCaps(dc, LOGPIXELSY);
                        int edge = MulDiv(2, dpi > 0 ? dpi : 96, 96);
                        if (edge < 1) edge = 1;
                        RECT top{client.left, client.top, client.right, client.top + edge};
                        RECT left{client.left, client.top, client.left + edge, last.bottom};
                        RECT right{client.right - edge, client.top, client.right, last.bottom};
                        FillRect(dc, &top, brush);
                        FillRect(dc, &left, brush);
                        FillRect(dc, &right, brush);
                        DeleteObject(brush);
                    }
                    ReleaseDC(hwnd, dc);
                }
            }
        }
    }
    return result;
}

void PreferencesAttach(HWND hwnd, bool themeControl) {
    if (!hwnd || ProcessIsExiting() || g_unloading.load(std::memory_order_relaxed)) return;
    {
        std::lock_guard<std::mutex> lock(g_preferencesMutex);
        if (g_preferencesWindows.contains(hwnd)) return;
    }
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hwnd, PreferencesDarkSubclass, 0)) return;
    if (!RecordSubclassed(g_preferencesWindows, g_preferencesMutex, hwnd)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, PreferencesDarkSubclass);
        return;
    }
    if (themeControl) {
        WCHAR cls[64]{};
        GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
        static std::atomic<unsigned> loggedControls{0};
        if (loggedControls.fetch_add(1, std::memory_order_relaxed) < 100)
            Wh_Log(L"[Adobe Acrylic] Preferences child hwnd=%p parent=%p class=%s", hwnd, GetParent(hwnd), cls);
        if (!_wcsicmp(cls, L"ScrollBar"))
            SetWindowTheme(hwnd, L"DarkMode_Explorer", nullptr);
        else if (!_wcsicmp(cls, L"SysListView32")) {
            DialogListColors old{ListView_GetBkColor(hwnd), ListView_GetTextColor(hwnd),
                                 ListView_GetTextBkColor(hwnd)};
            {
                std::lock_guard<std::mutex> lock(g_preferencesMutex);
                g_dialogListColors.emplace(hwnd, old);
            }
            SetWindowTheme(hwnd, L"DarkMode_Explorer", nullptr);
            ListView_SetBkColor(hwnd, RGB(38,38,38));
            ListView_SetTextBkColor(hwnd, RGB(38,38,38));
            ListView_SetTextColor(hwnd, RGB(235,235,235));
        }
        else if (!_wcsicmp(cls, L"SysHeader32"))
            SetWindowTheme(hwnd, L"DarkMode_DarkTheme", nullptr);
        else if (!_wcsicmp(cls, L"Button") || !_wcsicmp(cls, L"Edit") ||
            !_wcsicmp(cls, L"ComboBox") || !_wcsicmp(cls, L"SysTabControl32"))
            SetWindowTheme(hwnd, L"DarkMode_DarkTheme", nullptr);
    }
}

// Only intercept tab page backgrounds on a DC positively associated with a
// subclassed Preferences control. Unrelated app windows keep their own theme.
bool PreferencesDarkTabBackground(HTHEME theme, HDC dc, int part,
                                  const RECT* rect, const RECT* clip) {
    if (!g_darkPreferences.load(std::memory_order_relaxed) || !dc || !theme ||
        !rect || IsRectEmpty(rect) || (part != 9 && part != 10) ||
        !g_acrylicGetThemeClass || ProcessIsExiting()) return false;
    HWND hwnd = WindowFromDC(dc);
    if (!hwnd) return false;
    {
        std::lock_guard<std::mutex> lock(g_preferencesMutex);
        if (!g_preferencesWindows.contains(hwnd)) return false;
    }
    WCHAR cls[64]{};
    if (FAILED(g_acrylicGetThemeClass(theme, cls, ARRAYSIZE(cls))) ||
        _wcsicmp(cls, L"Tab")) return false;
    RECT area = *rect;
    if (clip && !IntersectRect(&area, rect, clip)) return true;
    static HBRUSH opaqueBrush = CreateSolidBrush(RGB(38,38,38));
    static HBRUSH clearBrush = CreateSolidBrush(RGB(0,0,0));
    HBRUSH brush = IsPreferencesAcrylicSurface(hwnd) ? clearBrush : opaqueBrush;
    if (!brush) return false;
    DepthGuard guard(&tl_ourPaintDepth);
    if (!FillRect(dc, &area, brush)) return false;
    static std::atomic<bool> logged{false};
    if (!logged.exchange(true, std::memory_order_relaxed))
        Wh_Log(L"[Adobe Acrylic] Darkened Preferences tab background part=%d", part);
    return true;
}

void PreparePreferences(HWND hwnd) {
    if (tl_preparingPreferences || !IsLightroomSecondaryWindow(hwnd) || ProcessIsExiting() ||
        g_unloading.load(std::memory_order_relaxed)) return;
    tl_preparingPreferences = true;
    PreparePreferencesAcrylic(hwnd);
    FindWatermarkOptionsPanel(hwnd);
    PreferencesAttach(hwnd, false);
    EnumChildWindows(hwnd, [](HWND child, LPARAM) WINAPI -> BOOL {
        PreferencesAttach(child, true);
        return TRUE;
    }, 0);
    WCHAR cls[128]{};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    Wh_Log(L"[Adobe Acrylic] Experimental Preferences styling applied hwnd=%p class=%s", hwnd, cls);
    RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
    tl_preparingPreferences = false;
}

void RemovePreferencesStyling() {
    if (ProcessIsExiting()) return;
    ResetPreferencesAcrylic();
    std::vector<HWND> windows;
    std::unordered_map<HWND, DialogListColors> listColors;
    {
        std::lock_guard<std::mutex> lock(g_preferencesMutex);
        windows.assign(g_preferencesWindows.begin(), g_preferencesWindows.end());
        g_preferencesWindows.clear();
        listColors.swap(g_dialogListColors);
    }
    for (HWND hwnd : windows) {
        if (!IsWindow(hwnd)) continue;
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hwnd, PreferencesDarkSubclass);
        WCHAR cls[64]{};
        GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
        if (!_wcsicmp(cls, L"Button") || !_wcsicmp(cls, L"Edit") ||
            !_wcsicmp(cls, L"ComboBox") || !_wcsicmp(cls, L"SysTabControl32") ||
            !_wcsicmp(cls, L"SysHeader32") || !_wcsicmp(cls, L"SysListView32") ||
            !_wcsicmp(cls, L"ScrollBar"))
            SetWindowTheme(hwnd, nullptr, nullptr);
        auto saved = listColors.find(hwnd);
        if (saved != listColors.end() && !_wcsicmp(cls, L"SysListView32")) {
            ListView_SetBkColor(hwnd, saved->second.background);
            ListView_SetTextColor(hwnd, saved->second.text);
            ListView_SetTextBkColor(hwnd, saved->second.textBackground);
        }
        RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
    }
    g_watermarkOptionsPanel.store(nullptr, std::memory_order_relaxed);
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
    if (code == HCBT_ACTIVATE) PreparePreferences((HWND)wParam);
    if (code == HCBT_CREATEWND) {
        if (ClassifyWindow((HWND)wParam) == WindowKind::MenuPopup)
            Wh_Log(L"[Adobe Acrylic] Popup created: %p", (HWND)wParam);
        SubclassIfKnown((HWND)wParam);
    }
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

// Through the hook, which only holds back a break written to a menu still on record. The
// mod's own writes either carry no break or come after the menu has left the table.
bool SetItemType(HMENU hMenu, UINT pos, UINT type) {
    MENUITEMINFOW mii = {};
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_FTYPE;
    mii.fType = type;

    return SetMenuItemInfoW(hMenu, pos, TRUE, &mii) != FALSE;
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
    if (cwp->message == WM_SHOWWINDOW && cwp->wParam) PreparePreferences(cwp->hwnd);
    // A popup created before injection will not trigger HCBT_CREATEWND.
    if (cwp->message == WM_WINDOWPOSCHANGING && cwp->lParam &&
        (((const WINDOWPOS*)cwp->lParam)->flags & SWP_SHOWWINDOW) &&
        ClassifyWindow(cwp->hwnd) == WindowKind::MenuPopup) {
        SubclassPopup(cwp->hwnd);
        AcrylicPreparePopup(cwp->hwnd);
    }

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
            tl_drawItems[tl_drawNesting] = kind == DrawKind::Menu ? di : nullptr;
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
            if (tl_drawNesting < kMaxNestedDraws) tl_drawItems[tl_drawNesting] = nullptr;
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

    HBRUSH hBrush = (hot ? g_hMenuBarHighlightBrush : g_hMenuBrush).load(std::memory_order_acquire);
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


// Acrylic-specific uxtheme hooks. Kept separate from Adobe Dark Menus' color
// hooks so the normal solid rendering stays untouched when Material = solid.
decltype(&DrawTextW) DrawTextW_AcrylicOriginal;
int WINAPI DrawTextW_AcrylicHook(HDC dc, LPCWSTR text, int count,
                                 LPRECT rect, UINT flags) {
    if (rect) {
        int result = 0;
        if (AcrylicDrawOwnerText(dc, rect, flags,
                [&](HDC buffer, RECT* bounds) -> int {
                    return DrawTextW_AcrylicOriginal(buffer, text, count, bounds, flags);
                }, &result)) return result;
    }
    return DrawTextW_AcrylicOriginal(dc, text, count, rect, flags);
}

decltype(&DrawTextExW) DrawTextExW_AcrylicOriginal;
int WINAPI DrawTextExW_AcrylicHook(HDC dc, LPWSTR text, int count,
                                   LPRECT rect, UINT flags,
                                   LPDRAWTEXTPARAMS params) {
    if (rect) {
        int result = 0;
        if (AcrylicDrawOwnerText(dc, rect, flags,
                [&](HDC buffer, RECT* bounds) -> int {
                    return DrawTextExW_AcrylicOriginal(buffer, text, count,
                                                       bounds, flags, params);
                }, &result)) return result;
    }
    return DrawTextExW_AcrylicOriginal(dc, text, count, rect, flags, params);
}

decltype(&DrawThemeBackground) DrawThemeBackground_AcrylicOriginal;
// Keep the native controls, text, arrows, focus and input handling. Replace
// only the specific themed faces of verified controls inside Preferences.
bool PreferencesAcrylicControlBackground(HTHEME theme, HDC dc, int part, int state,
                                         const RECT* rect, const RECT* clip) {
    HWND hwnd = tl_preferencesControlPaint;
    if (!hwnd || !IsPreferencesAcrylicSurface(hwnd) || !dc || !theme || !rect ||
        IsRectEmpty(rect) || !g_acrylicGetThemeClass || tl_ourPaintDepth)
        return false;
    WCHAR cls[64]{}, windowClass[64]{};
    if (FAILED(g_acrylicGetThemeClass(theme, cls, ARRAYSIZE(cls))) ||
        !GetClassNameW(hwnd, windowClass, ARRAYSIZE(windowClass))) return false;
    bool button = !_wcsicmp(cls, L"Button") && !_wcsicmp(windowClass, L"Button") && part == 1;
    bool combo = !_wcsicmp(cls, L"ComboBox") && !_wcsicmp(windowClass, L"ComboBox") && part == 5;
    bool tab = !_wcsicmp(cls, L"Tab") && !_wcsicmp(windowClass, L"SysTabControl32") && part >= 1 && part <= 8;
    if (!button && !combo && !tab) return false;
    RECT area = *rect;
    RECT visible{};
    if (clip && !IntersectRect(&visible, rect, clip)) return true;
    int width = area.right - area.left, height = area.bottom - area.top;
    if (width <= 0 || height <= 0 || width > 4096 || height > 512) return false;
    int saved = SaveDC(dc);
    if (!saved) return false;
    if (clip) IntersectClipRect(dc, clip->left, clip->top, clip->right, clip->bottom);
    DepthGuard guard(&tl_ourPaintDepth);
    HDC buffer = CreateCompatibleDC(dc);
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = 1;
    info.bmiHeader.biHeight = -1;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* pixels = nullptr;
    HBITMAP bitmap = buffer ? CreateDIBSection(dc, &info, DIB_RGB_COLORS, &pixels, nullptr, 0) : nullptr;
    HGDIOBJ old = bitmap ? SelectObject(buffer, bitmap) : nullptr;
    if (!old || old == HGDI_ERROR || !pixels) {
        if (bitmap) DeleteObject(bitmap);
        if (buffer) DeleteDC(buffer);
        RestoreDC(dc, saved);
        return false;
    }
    // More visible when hot or pressed, subdued when disabled.
    BYTE alpha = state == 3 ? 110 : state == 2 ? 90 : state == 4 ? 35 : 55;
    BYTE component = static_cast<BYTE>((200 * alpha) / 255);
    *static_cast<DWORD*>(pixels) = (static_cast<DWORD>(alpha) << 24) |
        (static_cast<DWORD>(component) << 16) | (static_cast<DWORD>(component) << 8) | component;
    PatBlt(dc, area.left, area.top, width, height, BLACKNESS);
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    BOOL drawn = AlphaBlend(dc, area.left, area.top, width, height, buffer, 0, 0, 1, 1, blend);
    if (drawn) {
        HBRUSH border = CreateSolidBrush(state == 4 ? RGB(90,90,90) : RGB(170,170,170));
        if (border) { FrameRect(dc, &area, border); DeleteObject(border); }
    }
    SelectObject(buffer, old);
    DeleteObject(bitmap);
    DeleteDC(buffer);
    RestoreDC(dc, saved);
    return drawn != FALSE;
}
HRESULT WINAPI DrawThemeBackground_AcrylicHook(HTHEME theme, HDC dc, int part,
        int state, LPCRECT rect, LPCRECT clip) {
    if (PreferencesAcrylicControlBackground(theme, dc, part, state, rect, clip)) return S_OK;
    if (PreferencesDarkTabBackground(theme, dc, part, rect, clip)) return S_OK;
    if (AcrylicThemeBackground(theme, dc, part, state, rect, clip)) return S_OK;
    return DrawThemeBackground_AcrylicOriginal(theme, dc, part, state, rect, clip);
}

decltype(&DrawThemeBackgroundEx) DrawThemeBackgroundEx_AcrylicOriginal;
HRESULT WINAPI DrawThemeBackgroundEx_AcrylicHook(HTHEME theme, HDC dc,
        int part, int state, LPCRECT rect, const DTBGOPTS* opts) {
    if ((!opts || (opts->dwSize == sizeof(*opts) && !(opts->dwFlags & ~DTBG_CLIPRECT))) &&
        PreferencesAcrylicControlBackground(theme, dc, part, state, rect,
            opts && (opts->dwFlags & DTBG_CLIPRECT) ? &opts->rcClip : nullptr)) return S_OK;
    if ((!opts || (opts->dwSize == sizeof(*opts) &&
                   !(opts->dwFlags & ~DTBG_CLIPRECT))) &&
        PreferencesDarkTabBackground(theme, dc, part, rect,
            opts && (opts->dwFlags & DTBG_CLIPRECT) ? &opts->rcClip : nullptr))
        return S_OK;
    if ((!opts || (opts->dwSize == sizeof(*opts) &&
                   !(opts->dwFlags & ~DTBG_CLIPRECT))) &&
        AcrylicThemeBackground(theme, dc, part, state, rect,
            opts && (opts->dwFlags & DTBG_CLIPRECT) ? &opts->rcClip : nullptr))
        return S_OK;
    return DrawThemeBackgroundEx_AcrylicOriginal(theme, dc, part, state, rect, opts);
}

decltype(&DrawThemeText) DrawThemeText_AcrylicOriginal;
decltype(&DrawThemeTextEx) DrawThemeTextEx_AcrylicOriginal;
HRESULT WINAPI DrawThemeText_AcrylicHook(HTHEME theme, HDC dc, int part,
        int state, LPCWSTR text, int count, DWORD flags, DWORD flags2, LPCRECT rect) {
    if (!flags2 && rect && DrawThemeTextEx_AcrylicOriginal) {
        RECT copy = *rect;
        HRESULT result = S_OK;
        if (AcrylicCompositedThemeText(theme, dc, part, state, text, count,
                flags, &copy, nullptr, &result, DrawThemeTextEx_AcrylicOriginal))
            return result;
    }
    return DrawThemeText_AcrylicOriginal(theme, dc, part, state,
                                        text, count, flags, flags2, rect);
}
HRESULT WINAPI DrawThemeTextEx_AcrylicHook(HTHEME theme, HDC dc, int part,
        int state, LPCWSTR text, int count, DWORD flags, LPRECT rect,
        const DTTOPTS* opts) {
    if (rect) {
        HRESULT result = S_OK;
        if (AcrylicCompositedThemeText(theme, dc, part, state, text, count,
                flags, rect, opts, &result, DrawThemeTextEx_AcrylicOriginal))
            return result;
    }
    return DrawThemeTextEx_AcrylicOriginal(theme, dc, part, state,
                                          text, count, flags, rect, opts);
}

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

decltype(&SetTextColor) SetTextColor_Original;
COLORREF WINAPI SetTextColor_DialogHook(HDC dc, COLORREF color) {
    if (!tl_ourPaintDepth && dc && dc == tl_dialogListItemDC && GetRValue(color) < 80 &&
        GetGValue(color) < 80 && GetBValue(color) < 80)
        color = tl_dialogListText;
    return SetTextColor_Original(dc, color);
}

decltype(&FillRect) FillRect_Original;
bool IsDialogMfcPaintDC(HDC dc) {
    if (!dc || !g_darkPreferences.load(std::memory_order_relaxed)) return false;
    HWND direct = WindowFromDC(dc);
    if (direct) return IsDialogMfcGroupContainer(direct);
    return tl_mfcGroupPaint && GetObjectType(dc) == OBJ_MEMDC;
}

bool IsOptionsPanelPaintDC(HDC dc) {
    if (!dc || !g_darkPreferences.load(std::memory_order_relaxed)) return false;
    HWND direct = WindowFromDC(dc);
    if (direct) return IsWatermarkOptionsContainer(direct);
    return tl_optionsPanelPaint && GetObjectType(dc) == OBJ_MEMDC;
}

bool IsPreferencesAcrylicPaintDC(HDC dc) {
    HWND direct = dc ? WindowFromDC(dc) : nullptr;
    if (direct) return IsPreferencesAcrylicSurface(direct);
    if (!dc || GetObjectType(dc) != OBJ_MEMDC) return false;
    return (tl_mfcGroupPaint && IsPreferencesAcrylicSurface(tl_mfcGroupPaint)) ||
           (tl_optionsPanelPaint && IsPreferencesAcrylicSurface(tl_optionsPanelPaint));
}

bool IsLightNeutralBrush(HBRUSH brush) {
    if (!brush) return false;
    COLORREF color = CLR_INVALID;
    ULONG_PTR index = reinterpret_cast<ULONG_PTR>(brush) - 1;
    if (index <= COLOR_MENUBAR) color = GetSysColor(static_cast<int>(index));
    else {
        LOGBRUSH info{};
        if (GetObjectW(brush, sizeof(info), &info) == sizeof(info) && info.lbStyle == BS_SOLID)
            color = info.lbColor;
    }
    int r = GetRValue(color), g = GetGValue(color), b = GetBValue(color);
    return color != CLR_INVALID && r >= 180 && g >= 180 && b >= 180 &&
           abs(r-g) <= 12 && abs(g-b) <= 12 && abs(r-b) <= 12;
}

int WINAPI FillRect_Hook(HDC hdc, const RECT* lprc, HBRUSH hbr) {
    // Some options-panel containers paint over WM_ERASEBKGND using light
    // solid fills. Limit recoloring to their own window DC and substantial,
    // nearly neutral fills; never recolor the photo preview or color swatch.
    if (!tl_ourPaintDepth && g_darkPreferences.load(std::memory_order_relaxed) && lprc && hbr &&
        lprc->right - lprc->left >= 16 && lprc->bottom - lprc->top >= 16 &&
        (IsOptionsPanelPaintDC(hdc) || IsDialogMfcPaintDC(hdc))) {
        if (IsLightNeutralBrush(hbr)) {
            static HBRUSH dark = CreateSolidBrush(RGB(38,38,38));
            static HBRUSH clear = CreateSolidBrush(RGB(0,0,0));
            HBRUSH replacement = IsPreferencesAcrylicPaintDC(hdc) ? clear : dark;
            if (replacement) return FillRect_Original(hdc, lprc, replacement);
        }
    }
    bool inMenu = tl_menuDrawDepth > 0;
    bool inList = tl_comboListDepth > 0;
    if (AcrylicClearNativeFill(hdc, lprc, hbr)) return 1;
    if ((!inMenu && !inList) || tl_ourPaintDepth > 0 || !lprc) {
        return FillRect_Original(hdc, lprc, hbr);
    }

    // Some owner-draw code fills with a system color pseudo-handle such as
    // (HBRUSH)(COLOR_MENU + 1). user32 resolves those from shared memory without
    // consulting GetSysColor or GetSysColorBrush, so they are re-resolved here.
    ULONG_PTR sysIndex = (ULONG_PTR)hbr - 1;
    if (inMenu && AcrylicIsPopupDC(hdc) &&
        IsPaintContext(hdc, kMenuPopupClass)) {
        if (sysIndex == COLOR_MENU || sysIndex == COLOR_MENUBAR ||
            hbr == g_hMenuBrush.load(std::memory_order_acquire) ||
            hbr == GetStockObject(BLACK_BRUSH)) {
            DepthGuard guard(&tl_ourPaintDepth);
            return PatBlt(hdc, lprc->left, lprc->top,
                          lprc->right - lprc->left,
                          lprc->bottom - lprc->top, BLACKNESS) ? 1 : 0;
        }
        if (sysIndex == COLOR_HIGHLIGHT ||
            hbr == g_hHighlightBrush.load(std::memory_order_acquire)) {
            if (AcrylicPaintHover(hdc, lprc)) return 1;
        }
    }
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
    if (!tl_ourPaintDepth && rop == PATCOPY && w >= 16 && h >= 16 &&
        (IsDialogMfcPaintDC(hdc) || IsOptionsPanelPaintDC(hdc)) &&
        IsLightNeutralBrush(reinterpret_cast<HBRUSH>(GetCurrentObject(hdc, OBJ_BRUSH)))) {
        static HBRUSH dark = CreateSolidBrush(RGB(38,38,38));
        static HBRUSH clear = CreateSolidBrush(RGB(0,0,0));
        HBRUSH replacement = IsPreferencesAcrylicPaintDC(hdc) ? clear : dark;
        if (replacement) {
            HGDIOBJ previous = SelectObject(hdc, replacement);
            if (previous && previous != HGDI_ERROR) {
                BOOL result = PatBlt_Original(hdc, x, y, w, h, rop);
                SelectObject(hdc, previous);
                return result;
            }
        }
    }
    if (InAcrylicPopupPaintContext() && rop == PATCOPY && w > 0 && h > 0) {
        RECT rect{x, y, x + w, y + h};
        if (AcrylicClearNativeFill(hdc, &rect,
                    (HBRUSH)GetCurrentObject(hdc, OBJ_BRUSH))) return TRUE;
    }
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
    if (!tl_ourPaintDepth && pInfo && pInfo->cbSize == sizeof(MENUINFO) && (pInfo->fMask & MIM_BACKGROUND) &&
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
    Wh_Log(L"[Adobe Acrylic] Settings changed; reopen menus to apply the backdrop.");
    AcrylicResetAllPopups();
    ResetPreferencesAcrylic();
    RestoreStampedMenus();
    LoadSettings();
    if (!g_darkPreferences.load(std::memory_order_relaxed)) RemovePreferencesStyling();
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

    WCHAR executable[MAX_PATH]{};
    DWORD executableLength = GetModuleFileNameW(nullptr, executable, ARRAYSIZE(executable));
    if (executableLength && executableLength < ARRAYSIZE(executable)) {
        const WCHAR* name = wcsrchr(executable, L'\\');
        g_isLightroomProcess = !_wcsicmp(name ? name + 1 : executable, L"Lightroom.exe");
    }

    LoadSettings();

    HMODULE uxtheme = GetModuleHandleW(L"uxtheme.dll");
    if (uxtheme) {
        g_acrylicGetThemeClass = reinterpret_cast<AcrylicGetThemeClassFn>(
            GetProcAddress(uxtheme, MAKEINTRESOURCEA(74)));
        if (g_acrylicGetThemeClass) {
            HTHEME probe = OpenThemeData(nullptr, L"Menu");
            WCHAR name[64]{};
            bool valid = probe && SUCCEEDED(g_acrylicGetThemeClass(
                probe, name, ARRAYSIZE(name))) && !_wcsicmp(name, L"Menu");
            if (probe) CloseThemeData(probe);
            if (!valid) g_acrylicGetThemeClass = nullptr;
        }
    }
    HMODULE user = GetModuleHandleW(L"user32.dll");
    if (user) {
        g_acrylicSetComposition = reinterpret_cast<AcrylicCompositionFn>(
            GetProcAddress(user, "SetWindowCompositionAttribute"));
        g_acrylicGetComposition = reinterpret_cast<AcrylicCompositionFn>(
            GetProcAddress(user, "GetWindowCompositionAttribute"));
    }
    g_acrylicSupported.store(g_acrylicSetComposition != nullptr &&
                             g_acrylicGetThemeClass != nullptr,
                             std::memory_order_relaxed);
    Wh_Log(L"[Adobe Acrylic] init: material=%d composition=%d themeClass=%d "
           L"enabled=%d (Lightroom menu-bar tracking does not require "
           L"TrackPopupMenu exports).",
           g_acrylicMaterial.load(std::memory_order_relaxed),
           g_acrylicSetComposition != nullptr,
           g_acrylicGetThemeClass != nullptr,
           AcrylicIsEnabled());
    if (g_acrylicMaterial.load(std::memory_order_relaxed) &&
        !g_acrylicSupported.load(std::memory_order_relaxed))
        Wh_Log(L"[Adobe Acrylic] Composition/theme detection unavailable; menus stay solid.");

    if (!WindhawkUtils::SetFunctionHook(DrawTextW,
          DrawTextW_AcrylicHook, &DrawTextW_AcrylicOriginal))
        Wh_Log(L"Failed to hook DrawTextW");
    if (!WindhawkUtils::SetFunctionHook(DrawTextExW,
          DrawTextExW_AcrylicHook, &DrawTextExW_AcrylicOriginal))
        Wh_Log(L"Failed to hook DrawTextExW");
    if (!WindhawkUtils::SetFunctionHook(DrawThemeBackground,
          DrawThemeBackground_AcrylicHook, &DrawThemeBackground_AcrylicOriginal))
        Wh_Log(L"Failed to hook DrawThemeBackground");
    if (!WindhawkUtils::SetFunctionHook(DrawThemeBackgroundEx,
          DrawThemeBackgroundEx_AcrylicHook, &DrawThemeBackgroundEx_AcrylicOriginal))
        Wh_Log(L"Failed to hook DrawThemeBackgroundEx");
    if (!WindhawkUtils::SetFunctionHook(DrawThemeText,
          DrawThemeText_AcrylicHook, &DrawThemeText_AcrylicOriginal))
        Wh_Log(L"Failed to hook DrawThemeText");
    if (!WindhawkUtils::SetFunctionHook(DrawThemeTextEx,
          DrawThemeTextEx_AcrylicHook, &DrawThemeTextEx_AcrylicOriginal))
        Wh_Log(L"Failed to hook DrawThemeTextEx");

    // A failure here only costs part of the result, since the dark theme below needs no
    // hook at all.
    if (!WindhawkUtils::SetFunctionHook(TrackPopupMenu, TrackPopupMenu_Hook, &TrackPopupMenu_Original)) Wh_Log(L"Failed to hook TrackPopupMenu");
    if (!WindhawkUtils::SetFunctionHook(TrackPopupMenuEx, TrackPopupMenuEx_Hook, &TrackPopupMenuEx_Original)) Wh_Log(L"Failed to hook TrackPopupMenuEx");
    if (!WindhawkUtils::SetFunctionHook(GetSysColor, GetSysColor_Hook, &GetSysColor_Original)) Wh_Log(L"Failed to hook GetSysColor");
    if (!WindhawkUtils::SetFunctionHook(GetSysColorBrush, GetSysColorBrush_Hook, &GetSysColorBrush_Original)) Wh_Log(L"Failed to hook GetSysColorBrush");
    if (!WindhawkUtils::SetFunctionHook(SetTextColor, SetTextColor_DialogHook, &SetTextColor_Original)) Wh_Log(L"Failed to hook SetTextColor");
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
        if (tid && pid == GetCurrentProcessId()) {
            InstallThreadHook(tid);
            // Existing hidden/reused #32768 menu popups predate HCBT_CREATEWND.
            if (ClassifyWindow(hWnd) == WindowKind::MenuPopup)
                SubclassPopup(hWnd);
        }
        return TRUE;
    }, 0);

    RedrawMenuBars();
    SyncTitleBars();
}

void Wh_ModUninit() {
    // Before anything is taken down, so that nothing new goes up behind it.
    g_unloading.store(true, std::memory_order_relaxed);
    RemovePreferencesStyling();
    AcrylicResetAllPopups();

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
            RemovePropW(hWnd, kVisibleMenuPopupProp);
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

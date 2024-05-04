// ==WindhawkMod==
// @id              chrome-ui-tweaks
// @name            Chrome UI Tweaks
// @description     Small UI tweaks to Google Chrome
// @version         1.0.0
// @author          Vasher
// @github          https://github.com/VasherMC
// @architecture    x86-64
// @include         chrome.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Chrome UI Tweaks
This mod currently implements two tweaks for Chrome, on x86-64 Windows:

## Simplified Bookmark Flow
Disable the "Simplified Bookmark Flow" feature (see [https://issues.chromium.org/issues/41496433](https://issues.chromium.org/issues/41496433)).
When creating a new bookmark, you will immediately see the "Edit Bookmark" window.

## Menu UI customization
"Chrome Refresh 2023" brought significant changes to the user interface, including rounding the corners
of menus (as well as many other UI elements) and significantly increasing the spacing between items,
meaning fewer menu items can fit on screen.

This tweak lets you configure many menu UI values.
This will affect several kinds of menus:
  - main options/settings menu (three dots at the top right, "Customize and control Google Chrome")
  - Bookmark menus (not including the bookmark bar itself)
  - Context menu (when you right-click)

Keep in mind unexpectedly large, small, or negative values may cause graphical issues.
Most values are measured in device-independent-pixels (i.e. will scale with DPI),
and you will probably want to keep them relatively small (below 50), but feel free to experiment.

`item_vertical_margin` is the only MenuConfig setting that is changed initially.
All other MenuConfig mod settings are negative one (-1) by default.
If any MenuConfig mod setting is -1, it will be ignored.
You can use this to reset a value back to default.
Actual default values are described for each option if they exist/are known;
the four potential defaults are:
* Old, for default values usually originating from before Chrome 75 (before 2019)
* CR2023, for default values of Chrome Refresh 2023
* Win10, for default values on Windows 10 (before CR2023)
* Win11, for default values on Windows 11 (before CR2023)

Note that the "Old" default values may not look the same way as in older versions of Chrome
due to other changes in the Chrome code.

# Issues
Due to implementation details, this mod may stop working on new Chrome versions.
You can report issues on github: https://github.com/VasherMC/windhawk-mods/issues

First tested Chrome version: 123.0.6312.106

Latest tested Chrome version: 124.0.6367.119 (stable), 126.0.6457.0 (canary)

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# Here you can define settings, in YAML format, that the mod users will be able
# to configure. Metadata values such as $name and $description are optional.
# Check out the documentation for more information:
# https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod#settings
- bookmark_flow: true
  $name: Disable "Simplified Bookmark Flow"
  $description: When you bookmark a new page, immediately show the editing window.
- menuconfig:
  - item:
    - item_vertical_margin: 3
      $description: Margins between the item top/bottom and its contents. Old = 4, CR2023 = 6, Win10 = 3
    - between_item_vertical_padding: -1
      $description: Padding, if any, between successive menu items. Default = 0, Win11 = 2
    - item_horizontal_padding: -1
      $description: Horizontal padding between components in a menu item. Default = 8, Win11 = 12
    - item_horizontal_border_padding: -1
      $description: Additional padding between the item left/right and its contents. Old = 0, CR2023 = 12, Win10 = -2
    - item_corner_radius: -1
      $description: Radius of selection background on menu items. Default = 0, Win11 = 4
    $description: Config related to individual Menu items
  - separators:
    - separator_height: -1
      $description: Height of a normal separator. Old = 11, CR2023 = 17, Win10 = 7, Win11 = 1
    - separator_lower_height: -1
      $description: Height of a lower separator. Old = 4, CR2023 = 7, Win10 = 5, Win11 = 1
    - separator_upper_height: -1
      $description: Height of an upper separator. Old = 3, CR2023 = 5, Win10 = 5, Win11 = 1
    - separator_spacing_height: -1
      $description: Height of a spacing separator. Old = 3, CR2023 = 4
    - separator_thickness: -1
      $description: Thickness of the drawn separator line in pixels. Default = 1
    - double_separator_height: -1
      $description: Height of a double separator. Default = 18
    - double_separator_thickness: -1
      $description: Thickness of the drawn separator line in pixels for double separator. Default = 2
    - separator_horizontal_border_padding: -1
      $description: Horizontal border padding of a separator. Default = 0
    - padded_separator_start_padding: -1
      $description: Padding at the start of a padded separator. Default = 64
    $description: Config related to Separators between groups of items in a menu
  - corners:
    - corner_radius: -1
      $description: Corner radius of menus. Also controls extra padding at the top and bottom of menus. Old = 0, CR2023 = 8, Win11 = 8
    - auxiliary_corner_radius: -1
      $description: Corner radius of Comboboxes, and also contextmenu (if use_bubble_border is false). CR2023 = 8
    $description: Roundedness of menu corners. See also `use_bubble_border`.
  - use_bubble_border: -1
    $description: Boolean (must be 0 or 1). Default = true (1) if corner_radius greater than 0, otherwise false (0).
                  Also (bug?) if true, menus will avoid an area at the bottom of the screen that is approximately the size of the Windows Taskbar.
  - shadows:
    - bubble_menu_shadow_elevation: -1
      $description: Shadow elevation of bubble menus. Default = 12
    - bubble_submenu_shadow_elevation: -1
      $description: Shadow elevation of bubble submenus. Default = 16
    $description: config for shadows behind bubble menus
  - show_delay: -1
    $description: Delay, in ms, between when menus are selected or moused over and the submenu appears. Default = 400, Windows = SPI_GETMENUSHOWDELAY (from Registry, default 400)
  - scroll_arrow_height: -1
    $description: Height of the scroll arrow. Default = 3
  - submenu:
    - arrow_size: -1
      $description: Size (width and height) of arrow bounding box. Default = 8, CR2023 = 16
    - arrow_to_edge_padding: -1
      $description: Padding between the arrow and the edge. Default = 8
    - submenu_horizontal_overlap: -1
      $description: Horizontal overlap between the submenu and its parent menu item. Old = 3, CR2023 = 0, Win11 = 1
# Actionable submenu only used in Ash = Chrome OS (commit/2291f5a4d61656904ae454009305857dffa068d5)
#    - actionable_submenu_arrow_to_edge_padding: -1
#      $description: Edge padding for an actionable submenu arrow. Default = 14
#    - actionable_submenu_vertical_separator_height: -1
#      $description: Height of the vertical separator used in an actionable submenu. Default = 18
#    - actionable_submenu_vertical_separator_width: -1
#      $description: Width of the vertical separator used in an actionable submenu. Default = 1
#    - actionable_submenu_width: -1
#      $description: Width of the submenu in an actionable submenu. Default = 37
    $description: Configs related to submenus, and arrows shown on items that will point to submenus.
                  Example - a bookmark folder item has an arrow, and the items in the folder are in a submenu.
#   - touchable:
#     - touchable_anchor_offset: -1
  $name: Menu Configuration
  $description: Override default menu UI configuration (applies to contextmenu, bookmarks, settings, etc).
                Values set to "-1" are not modified and the existing default values will be used instead.
*/
// ==/WindhawkModSettings==

#include <libloaderapi.h>
#include <windhawk_api.h>
#include <winnt.h>
#include <string_view>

using namespace std::string_view_literals;

// ===========================================================================

#pragma region "(include: Chromium) Chromium struct definitions"
// Copyright 2012 The Chromium Authors
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google LLC nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

class FontList { // gfx::FontList
    private:
    void* impl_; // scoped_refptr
};

// struct layout since chrome 121
// until at least chrome ~126
// https://github.com/chromium/chromium/blob/main/ui/views/controls/menu/menu_config.h
struct MenuConfig {
  //   MenuConfig();
  //   ~MenuConfig();

  // Menus are the only place using kGroupingPropertyKey, so any value (other
  // than 0) is fine.
  //   static constexpr int kMenuControllerGroupingId = 1001;

  //   static const MenuConfig& instance();

  // Helper methods to simplify access to MenuConfig:
  // Returns the appropriate corner radius for the menu controlled by
  // |controller|, or the default corner radius if |controller| is nullptr.
  //   int CornerRadiusForMenu(const MenuController* controller) const;

  // Returns whether |item_view| should show accelerator text. If so, returns
  // the text to show.
  //   bool ShouldShowAcceleratorText(const MenuItemView* item_view,
                                //  std::u16string* text) const;

  // Initialize menu config for CR2023
  //   void InitCR2023();
  //   void InitPlatformCR2023();

  // Font lists used by menus. (Actual type: gfx::FontList)
  FontList font_list;
  FontList context_menu_font_list;

  // Menu border sizes. Menus with rounded corners use
  // `rounded_menu_vertical_border_size` if set and fall back to the corner
  // radius otherwise.
  int nonrounded_menu_vertical_border_size = 4;
  std::optional<int> rounded_menu_vertical_border_size;
  int menu_horizontal_border_size; // = views::RoundRectPainter::kBorderWidth;

  // The horizontal overlap between the submenu and its parent menu item.
  int submenu_horizontal_overlap = 3;

  // Margins between the item top/bottom and its contents.
  int item_vertical_margin = 4;

  // Margins between the item top/bottom and its contents for ash system ui
  // layout.
  int ash_item_vertical_margin = 4;

  // Minimum dimensions used for entire items. If these are nonzero, they
  // override the vertical margin constants given above - the item's text and
  // icon are vertically centered within these heights.
  int minimum_text_item_height = 0;
  int minimum_container_item_height = 0;

  // TODO(ftirelo): Paddings should come from the layout provider, once Harmony
  // is the default behavior.

  // Horizontal padding between components in a menu item.
  int item_horizontal_padding = 8;

  // Horizontal padding between components in a touchable menu item.
  int touchable_item_horizontal_padding = 16;

  // Additional padding between the item left/right and its contents. Note that
  // the final padding will also include `item_horizontal_padding`.
  int item_horizontal_border_padding = 0;

  // Horizontal border padding in a menu item for ash system ui layout.
  int ash_item_horizontal_border_padding = 0;

  // Size (width and height) of arrow bounding box.
  int arrow_size = 8;

  // Padding between the arrow and the edge.
  int arrow_to_edge_padding = 8;

  // Height of a normal separator (ui::NORMAL_SEPARATOR).
  int separator_height = 11;

  // Height of a double separator (ui::DOUBLE_SEPARATOR).
  int double_separator_height = 18;

  // Height of a ui::UPPER_SEPARATOR.
  int separator_upper_height = 3;

  // Height of a ui::LOWER_SEPARATOR.
  int separator_lower_height = 4;

  // Height of a ui::SPACING_SEPARATOR.
  int separator_spacing_height = 3;

  // Thickness of the drawn separator line in pixels.
  int separator_thickness = 1;

  // Thickness of the drawn separator line in pixels for double separator.
  int double_separator_thickness = 2;

  // Horizontal border padding of a separator.
  int separator_horizontal_border_padding = 0;

  // Padding, if any, between successive menu items. This is not applied below
  // LOWER_SEPARATORs or above UPPER_SEPARATORs, since these are meant to be
  // flush with the respective adjacent items.
  int between_item_vertical_padding = 0;

  // Are mnemonics shown?
  bool show_mnemonics = false;

  // Are mnemonics used to activate items?
  bool use_mnemonics = true;

  //   char padding1;
  //   char padding2;

  // Height of the scroll arrow.
  int scroll_arrow_height = 3;

  // Edge padding for an actionable submenu arrow.
  int actionable_submenu_arrow_to_edge_padding = 14;

  // Width of the submenu in an actionable submenu.
  int actionable_submenu_width = 37;

  // The height of the vertical separator used in an actionable submenu.
  int actionable_submenu_vertical_separator_height = 18;

  // The width of the vertical separator used in an actionable submenu.
  int actionable_submenu_vertical_separator_width = 1;

  // Whether the keyboard accelerators are visible.
  bool show_accelerators = true;

  // True if submenu arrows should get their own column, separate from minor
  // text.
  bool reserve_dedicated_arrow_column = true;

  // True if the scroll container should add a border stroke around the menu.
  bool use_outer_border = true;

  // True if the icon is part of the label rather than in its own column.
  bool icons_in_label = false;

  // Spacing between icon and main label.
  int icon_label_spacing; // = LayoutProvider::Get()->GetDistanceMetric(DISTANCE_RELATED_LABEL_HORIZONTAL);

  // Menus lay out as if some items have checkmarks, even if none do.
  bool always_reserve_check_region = false;

  // True if a combobox menu should put a checkmark next to the selected item.
  bool check_selected_combobox_item = false;

  //   char padding3;
  //   char padding4;

  // Delay, in ms, between when menus are selected or moused over and the menu
  // appears.
  // * On Windows, is set to SPI_GETMENUSHOWDELAY (global Operating-system based value)
  int show_delay = 400;

  // Radius of the rounded corners of the menu border. Must be >= 0.
  int corner_radius; // = LayoutProvider::Get()->GetCornerRadiusMetric(ShapeContextTokens::kMenuRadius);

  // Radius of "auxiliary" rounded corners - comboboxes and context menus.
  // Must be >= 0.
  int auxiliary_corner_radius; // = LayoutProvider::Get()->GetCornerRadiusMetric(ShapeContextTokens::kMenuAuxRadius);

  // Radius of the rounded corners of the touchable menu border
  int touchable_corner_radius; // = LayoutProvider::Get()->GetCornerRadiusMetric(ShapeContextTokens::kMenuTouchRadius);

  // Radius of selection background on menu items.
  int item_corner_radius = 0;

  // Anchor offset for touchable menus created by a touch event.
  int touchable_anchor_offset = 8;

  // Height of child MenuItemViews for touchable menus.
  int touchable_menu_height = 36;

  // Minimum width of touchable menus.
  int touchable_menu_min_width = 256;

  // Maximum width of touchable menus.
  int touchable_menu_max_width = 352;

  // Shadow elevation of bubble menus.
  int bubble_menu_shadow_elevation = 12;

  // Shadow elevation of bubble submenus.
  int bubble_submenu_shadow_elevation = 16;

  // Vertical padding for touchable menus.
  int vertical_touchable_menu_item_padding = 8;

  // Padding at the start of a padded separator (ui::PADDED_SEPARATOR).
  int padded_separator_start_padding = 64;

  // Whether arrow keys should wrap around the end of the menu when selecting.
  bool arrow_key_selection_wraps = true;

  // Whether to show accelerators in context menus.
  bool show_context_menu_accelerators = true;

  // Whether all types of menus use prefix selection for items.
  bool all_menus_use_prefix_selection = false;

  //   char padding5;

  // Margins for footnotes (HIGHLIGHTED item at the end of a menu).
  int footnote_vertical_margin = 11;

  // Should use a bubble border for menus.
  bool use_bubble_border = false;

  //  private:
  // Configures a MenuConfig as appropriate for the current platform.
  //   void Init();
};
#pragma endregion Chromium_struct_definitions

// ===========================================================================

#pragma region "debug"
void log_hexdump(const unsigned char* ptr, size_t len_16byte_segments) {
#define CHAR(c) c>' '&&c<='~'?c:'.'
    for (size_t line = 0; line < len_16byte_segments; line++) {
        const unsigned char* a = ptr + 16*line;
        // if (line==0) Wh_Log(L"(guessed here)");
        Wh_Log(L"%p:             %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x  | %C%C%C%C %C%C%C%C %C%C%C%C %C%C%C%C", a,
        a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12], a[13], a[14], a[15],
        CHAR(a[0]), CHAR(a[1]), CHAR(a[2]), CHAR(a[3]),
        CHAR(a[4]), CHAR(a[5]), CHAR(a[6]), CHAR(a[7]),
        CHAR(a[8]), CHAR(a[9]), CHAR(a[10]), CHAR(a[11]),
        CHAR(a[12]), CHAR(a[13]), CHAR(a[14]), CHAR(a[15])
        );
    }
#undef CHAR
}
#pragma endregion "debug"

// ===========================================================================

#pragma region "Mod settings"
struct {
    bool bookmark_flow;
    MenuConfig menuconfig;
} settings;

MenuConfig g_original_menuconfig;
MenuConfig* g_Menuconfig_Instance = NULL;

void LoadMenuConfigSettings() {
    // for (FIELD of MenuConfig) { settings.menuconfig.FIELD = Wh_GetIntSetting(L"menuconfig.FIELD"); }
    // must conform to ==WindhawkModSettings== above
    int tmp;
#define LOAD_MENUCONFIG_INT(SECTION, FIELD) \
    tmp = Wh_GetIntSetting(L"menuconfig." SECTION #FIELD ); \
    settings.menuconfig.FIELD = (tmp == -1) ? g_original_menuconfig.FIELD : tmp; \
    Wh_Log(L"Value of field " #FIELD " read as %d, set to %d", tmp, settings.menuconfig.FIELD);

    // item paddings and margins
    LOAD_MENUCONFIG_INT("item.", item_vertical_margin)
    LOAD_MENUCONFIG_INT("item.", between_item_vertical_padding)
    LOAD_MENUCONFIG_INT("item.", item_horizontal_padding)
    LOAD_MENUCONFIG_INT("item.", item_horizontal_border_padding)
    LOAD_MENUCONFIG_INT("item.", item_corner_radius)

    // Separators
    LOAD_MENUCONFIG_INT("separators.", separator_height)
    LOAD_MENUCONFIG_INT("separators.", separator_lower_height)
    LOAD_MENUCONFIG_INT("separators.", separator_upper_height)
    LOAD_MENUCONFIG_INT("separators.", separator_spacing_height)
    LOAD_MENUCONFIG_INT("separators.", separator_thickness)
    LOAD_MENUCONFIG_INT("separators.", double_separator_height)
    LOAD_MENUCONFIG_INT("separators.", double_separator_thickness)
    LOAD_MENUCONFIG_INT("separators.", padded_separator_start_padding)

    // LOAD_MENUCONFIG_INT("", menu_horizontal_border_size)

    // LOAD_MENUCONFIG_INT("", nonrounded_menu_vertical_border_size)
    // LOAD_MENUCONFIG_INT("", rounded_menu_vertical_border_size) // std::optional<int>

    // Rounded Corners
    LOAD_MENUCONFIG_INT("corners.", corner_radius)
    LOAD_MENUCONFIG_INT("corners.", auxiliary_corner_radius) // Comboboxes and (sometimes) contextmenu

    // Bubble border and Shadows
    LOAD_MENUCONFIG_INT("", use_bubble_border) // bool
    LOAD_MENUCONFIG_INT("shadows.", bubble_menu_shadow_elevation)
    LOAD_MENUCONFIG_INT("shadows.", bubble_submenu_shadow_elevation)

    LOAD_MENUCONFIG_INT("", show_delay)

    LOAD_MENUCONFIG_INT("", scroll_arrow_height)

    // arrows (for submenus)
    LOAD_MENUCONFIG_INT("submenu.", arrow_size)
    LOAD_MENUCONFIG_INT("submenu.", arrow_to_edge_padding)
    // submenus
    LOAD_MENUCONFIG_INT("submenu.", submenu_horizontal_overlap)
    // LOAD_MENUCONFIG_INT("submenu.", actionable_submenu_arrow_to_edge_padding)
    // LOAD_MENUCONFIG_INT("submenu.", actionable_submenu_vertical_separator_height)
    // LOAD_MENUCONFIG_INT("submenu.", actionable_submenu_vertical_separator_width)
    // LOAD_MENUCONFIG_INT("submenu.", actionable_submenu_width)

    // Touchable - for touchable menus (ie, with a touchscreen).
    // Most values seem only used for Ash (Aura Shell) ChromeOS UI (ie, not on windows).
    //   Exception: touchable_anchor_offset seems to maybe be used on top-level bubble menus?
    // LOAD_MENUCONFIG_INT("touchable.", touchable_anchor_offset) // Default = 8
    // LOAD_MENUCONFIG_INT("touchable.", touchable_menu_height)
    // LOAD_MENUCONFIG_INT("touchable.", touchable_menu_max_width)
    // LOAD_MENUCONFIG_INT("touchable.", touchable_menu_min_width)
    // LOAD_MENUCONFIG_INT("touchable.", touchable_corner_radius)
    // LOAD_MENUCONFIG_INT("touchable.", touchable_item_horizontal_padding)
    // LOAD_MENUCONFIG_INT("touchable.", vertical_touchable_menu_item_padding)

    // Keyboard Selection (accessibility)
    // LOAD_MENUCONFIG_INT("", all_menus_use_prefix_selection) // Default: Windows OS setting
    // LOAD_MENUCONFIG_INT("", arrow_key_selection_wraps)

}

void LoadSettings() {
    settings.bookmark_flow = Wh_GetIntSetting(L"bookmark_flow");
    LoadMenuConfigSettings();
}
#pragma endregion "Mod settings"

// ===========================================================================

#pragma region "Code section scan for hooking without symbols"

typedef struct {
    std::string_view search; // instructions to search for
    std::string_view prologue; // prologue of the function to be hooked (instructions at entry point)
    const size_t instr_offset; // estimated location of the searched instructions relative to the entry point
} function_search;

// Wrapper for string_view::find, that checks the needle is unique within the haystack.
const char* unique_search(std::string_view haystack, std::string_view needle, LPCWSTR symbol_name) {
    size_t index1 = haystack.find(needle);
    if (index1 == std::string_view::npos) {
        Wh_Log(L"Error: Couldn't find instructions for symbol %s", symbol_name);
        return NULL;
    }
    // Can we find the same sequence again in the rest of the haystack?
    size_t index2 = haystack.find(needle, index1 + 1);
    if (index2 != std::string_view::npos) {
        Wh_Log(L"Error: Found multiple matches for %s: at %p and at %p", symbol_name, haystack.begin()+index1, haystack.begin()+index2);
        // log_hexdump((unsigned char*)haystack.begin() + index1 - 0x50, 6);
        // Wh_Log(L"----------------");
        // log_hexdump((unsigned char*)haystack.begin() + index2 - 0x50, 6);
        return NULL;
    }
    return haystack.begin() + index1;
}

// get address and size of code section via PE header info  (expect around 200 MB)
// TODO is this actually correct? (does it include superfluous sections of the DLL?)
// https://learn.microsoft.com/en-us/windows/win32/debug/pe-format
// https://learn.microsoft.com/en-us/archive/msdn-magazine/2002/february/inside-windows-win32-portable-executable-file-format-in-detail
std::string_view getCodeSection(HMODULE chromeModule) {
    if (chromeModule == NULL) return ""sv;
    IMAGE_DOS_HEADER* dos_header = (IMAGE_DOS_HEADER*) chromeModule;
    IMAGE_NT_HEADERS* pe_header = (IMAGE_NT_HEADERS*)(((char*)dos_header) + dos_header->e_lfanew);
    if (pe_header->FileHeader.Machine != 0x8664 || pe_header->OptionalHeader.Magic != 0x20b) {
        Wh_Log(L"Mod only implemented for 64-bit windows/chrome - machine was 0x%04x and magic was 0x%04x",
        pe_header->FileHeader.Machine, pe_header->OptionalHeader.Magic);
        return ""sv;
    }
    return std::string_view{
        ((char*)dos_header) + pe_header->OptionalHeader.BaseOfCode,
        pe_header->OptionalHeader.SizeOfCode
    };
}

// Find a function address by scanning for specific instruction patterns.
// We search the entire code section once per function,
// to ensure we aren't hooking the wrong location.
// Better safe than sorry, and it shouldn't cause noticeable delay on startup.
const char* search_function_instructions(std::string_view code_section, function_search fsearch, LPCWSTR symbol_name) {
    if (code_section.size()==0) return 0;
    Wh_Log(L"Searching for function %s", symbol_name);
    const char* addr = unique_search(code_section, fsearch.search, symbol_name);
    if (addr == NULL) {
        Wh_Log(L"Could not find function %s; is the mod up to date?", symbol_name);
        return NULL;
    }
    Wh_Log(L"Instructions were found at address: %p", addr);
    int offset = fsearch.instr_offset;
    const char* entry = addr - offset;
    // verify the prologue is what we expect; otherwise search for it
    // and verify it is preceded by 0xcc INT3 or 0xc3 RET (or ?? JMP)
    auto prologue = fsearch.prologue;
    if (prologue != std::string_view{entry, prologue.size()}) {
        Wh_Log(L"Prologue not found where expected, searching...");
        // maybe function length changed due to different compilation
        auto search_space = std::string_view{entry - 0x40, addr};
        size_t new_offset = search_space.rfind(prologue);
        if (new_offset != std::string_view::npos) {
            entry = (char*)search_space.begin() + new_offset;
        } else {
            entry = NULL;
        }
    }
    if (entry) {
        Wh_Log(L"Found entrypoint for function %s at addr %p", symbol_name, entry);
        if (entry[-1]!=(char)0xcc && entry[-1]!=(char)0xc3) {
            Wh_Log(L"Warn: prologue not preceded by INT3 or RET");
        }
        return entry;
    } else {
        Wh_Log(L"Err: Couldn't locate function entry point for symbol %s", symbol_name);
        // log_hexdump(addr - 0x40, 0x5);
        return NULL;
    }
}

#pragma endregion "Code section scan for hooking without symbols"

// ===========================================================================

#pragma region "BookmarkBubble"
// real type: void __thiscall (*)(ToolbarView* this, const GURL& url, bool)
using ShowBookmarkBubble_t = void __thiscall (*)(void*, void*, bool);
ShowBookmarkBubble_t ShowBookmarkBubble_Original;
void __thiscall ShowBookmarkBubble_Hook(void* _this, void* url, bool already_bookmarked) {
    // override already_bookmarked to disable "Simplified Bookmark Flow"
    ShowBookmarkBubble_Original(_this, url, already_bookmarked || settings.bookmark_flow);
}
// setting up arguments to call ShowBubble from ToolbarView::ShowBookmarkBubble
const std::string_view BookmarkBubble_instructions =
    "\x88\x5c\x24\x30"sv        // mov byte ptr [rsp+0x30], bl  (already_bookmarked = bl)
    "\x48\x89\x74\x24\x28"sv    // mov qword ptr [rsp+0x28], rsi
    "\x48\x89\x6c\x24\x20"sv    // mov qword ptr [rsp+0x20], rbp
    "\x48\x89\xf9"sv            // mov rcx, rdi
    "\x48\x89\xc2"sv            // mov rdx, rax
    "\x4d\x89\xf8"sv            // mov r8, r15
    "\x4d\x89\xe1"sv            // mov r9, r12
    "\xe8"sv;                   // call
const std::string_view BookmarkBubble_prologue = "AWAVAUATVWUS"sv;

bool hook_BookmarkBubble(std::string_view code_section) {
    const char* hook_loc = search_function_instructions(
        code_section,
        {
            .search = BookmarkBubble_instructions,
            .prologue = BookmarkBubble_prologue,
            .instr_offset = 0xb5,
        },
        L"?ShowBookmarkBubble@ToolbarView@@QEAAXAEBVGURL@@_N@Z");
    if (hook_loc == NULL) return false;
    Wh_SetFunctionHook((void*)hook_loc, (void*)ShowBookmarkBubble_Hook, (void **)&ShowBookmarkBubble_Original);
    return true;
}

#pragma endregion "BookmarkBubble"

// ===========================================================================

#pragma region "MenuConfig"
// MenuConfig instance storage is declared above, under Mod Settings

void ResetMenuConfig() {
    if (g_Menuconfig_Instance == NULL) return;
    Wh_Log(L"Restoring original MenuConfig");
    *g_Menuconfig_Instance = g_original_menuconfig;
}

void UpdateMenuConfig() {
    if (g_Menuconfig_Instance == NULL) return;
    Wh_Log(L"Updating MenuConfig");
    // we validate/bound the fields inside LoadMenuConfigSettings()
    *g_Menuconfig_Instance = settings.menuconfig;
}

void SetMenuConfig(MenuConfig* instance) {
    if (g_Menuconfig_Instance != NULL) {
        // Wh_Log(L"Error: tried to set MenuConfig twice");
        return;
    }
    Wh_Log(L"Setting MenuConfig instance at address %p", instance);
    // log_hexdump((unsigned char*)instance, sizeof(struct MenuConfig)/16);
    g_Menuconfig_Instance = instance;
    // Wh_Log(L"Backing up original MenuConfig");
    g_original_menuconfig = *instance; // save original values to restore later if necessary
    settings.menuconfig = *instance; // start from the original settings as defaults
    LoadMenuConfigSettings();
    UpdateMenuConfig();
}

using MenuConfigInstance_t = MenuConfig* (*)(void);
MenuConfigInstance_t MenuConfigInstance_Original;
MenuConfig* MenuConfig_Instance_Hook() {
    // allow default and platform-specific initialization to proceed
    MenuConfig* instance = MenuConfigInstance_Original();
    if (instance != NULL && g_Menuconfig_Instance == NULL) {
        // if (instance->touchable_menu_max_width == 352)
        SetMenuConfig(instance);
    }
    return instance;
}
// in views::MenuItemView::AddMenuItemAt
const std::string_view MenuConfig_Instance_postcall = // not unique in chrome 124.0.6367.91
    "\x8b\x80\xcc\x00\x00\x00"sv    // eax = [rax]menuconfig->footnote_vertical_margin
    "\x48\x0f\xba\xe8\x20"sv;       // BTS  rax, 0x20  (rax |= 0x1_0000_0000)


// Locate the MenuConfig::Instance() function in chrome.dll that gets the global instance.
// We hook it and wait for it to be called, to guarantee the MenuConfig has been initialized
// the function itself is hard to search for, so instead we search for
// instructions that use specific parts of the returned menuconfig.
bool hook_Menuconfig_Instance(std::string_view code_section) {
    const char* callsite = unique_search(code_section, MenuConfig_Instance_postcall, L"MenuConfig::Instance()");
    if (callsite == NULL) return false;
    if (callsite[-5] != (char)0xe8) {
        Wh_Log(L"Unexpected instruction at %p", callsite);
        log_hexdump((unsigned char*)callsite-0x20, 3);
        return false;
    }
    int32_t call_offset = ((const int32_t*)callsite)[-1];
    const char* MenuConfig_Instance_fstart = callsite + call_offset;
    Wh_Log(L"Computed address of MenuConfig::Instance is: %p", MenuConfig_Instance_fstart);
    Wh_SetFunctionHook((void*)MenuConfig_Instance_fstart, (void*)MenuConfig_Instance_Hook, (void**)&MenuConfigInstance_Original);
    return true;
}

#pragma endregion "MenuConfig"

// ===========================================================================


std::atomic<bool> g_chromeDllLoaded = false;
std::atomic<bool> g_unloading = false;

void set_hooks(HMODULE module) {
    std::string_view code_section = getCodeSection(module);
    int hooks_placed = 0;
    hooks_placed += hook_Menuconfig_Instance(code_section);
    hooks_placed += hook_BookmarkBubble(code_section);
    Wh_Log(L"Finished hooking: found %d/%d functions", hooks_placed, 2);
}

// On startup chrome.dll isn't loaded yet.
// We set a hook to wait for chrome.exe to load it
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module || g_unloading) {
        return module;
    }

    const wchar_t* pfound;
    if (!g_chromeDllLoaded &&
        // the path ends in exactly "chrome.dll"
        (pfound = wcsstr(lpLibFileName, L"\\chrome.dll"),
            pfound!=NULL && _wcsicmp(pfound, L"\\chrome.dll")==0) &&
        !g_chromeDllLoaded.exchange(true)) {
            Wh_Log(L"Loading chrome.dll from path: %s", lpLibFileName);
            set_hooks(module);
            Wh_ApplyHookOperations();
    }
    return module;
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    // Check whether we are the main chrome process
    bool mainProcess = true;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcsncmp(argv[i], L"--type=", sizeof("--type=") - 1) == 0) {
            mainProcess = false;
            break;
        }
    }
    LocalFree(argv);

    if (!mainProcess) {
        return FALSE;
    }

    // Initialize the mod for the main chrome process only
    Wh_Log(L"Init " WH_MOD_ID L" version " WH_MOD_VERSION);

    LoadSettings();

    // if chrome.dll is already loaded by the process, we expect this to succeed
    // otherwise assume we are during startup and it will be loaded later
    HMODULE chromeModule = GetModuleHandleW(L"chrome.dll");
    if (chromeModule == NULL) {
        // hook LoadLibraryExW to apply our actual hooks later when chrome.dll is loaded
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        FARPROC pKernelBaseLoadLibraryExW = GetProcAddress(kernelBaseModule, "LoadLibraryExW");

        Wh_SetFunctionHook((void*)pKernelBaseLoadLibraryExW,
                           (void*)LoadLibraryExW_Hook,
                           (void**)&LoadLibraryExW_Original);
        Wh_Log(L"Waiting for chrome.dll to be loaded");
    } else {
        set_hooks(chromeModule);
    }

    return TRUE;

#pragma region "Symbol-based hooking (disabled)"
/* Why don't we use symbol-based hooking in the first place?

1. To search the symbols, we need to download chrome.dll.pdb from google's symbol server
   for our chrome.dll version.
2. chrome.dll.pdb is upwards of **4 GB**, which is all mapped into the process memory.
3. over 50% of the time parsing the symbol table is spent in undecorating symbol names,
   even with the noUndecoratedSymbols option set - maybe I just didn't figure out the
   right way, or maybe undecorating is an unavoidable part of traversing symbols with
   the DIA library Windhawk uses (msdia140_windhawk.dll).
   The last time I tried it, it was busy parsing, undecorating, and traversing symbols
   for over 4 hours (pegging one core at 100%) before I killed the process.
4. While the symbol->address mapping can be cached, it needs to be updated every time
   the program updates. Due to relatively frequent minor version and security updates,
   this can be as often as once every few days for Chrome. This means another 4GB download,
   and limits the utility of caching addresses for symbols.
*/
    // WH_FIND_SYMBOL_OPTIONS options {
    //     .symbolServer = L"https://chromium-browser-symsrv.commondatastorage.googleapis.com",
    //     .noUndecoratedSymbols = TRUE
    // };
    // const int HOOK_COUNT = 1;
    // WindhawkUtils::SYMBOL_HOOK hooks[HOOK_COUNT] = {
    //     WindhawkUtils::SYMBOL_HOOK({L"?ShowBookmarkBubble@ToolbarView@@QEAAXAEBVGURL@@_N@Z"},
    //         &ShowBookmarkBubble_Original, ShowBookmarkBubble_Hook, false),
    // };
    // WindhawkUtils::HookSymbols(chromeModule, hooks, HOOK_COUNT, &options);
#pragma endregion "Symbol-based hooking (disabled)"
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    ResetMenuConfig();
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    LoadSettings();
    UpdateMenuConfig();
}

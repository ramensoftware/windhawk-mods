// ==WindhawkMod==
// @id              chrome-context-menu-items
// @name            Chrome Context Menu Items
// @description     Hide, add, group, and reorder Chrome context menu items, restyle Chrome's menus, and tweak tabs and extension buttons
// @version         1.0.0
// @author          DanRotaru
// @github          https://github.com/DanRotaru
// @homepage        https://dan13.me/
// @include         chrome.exe
// @architecture    x86-64
// @compilerOptions -lshell32 -lole32
// ==/WindhawkMod==

// clang-format off
// ==WindhawkModSettings==
/*
- menus:
  - applyToPageMenu: true
    $name: Apply to page context menu
    $description: Right-click menus on the web page, including link, image, and text-selection menus.
  - applyToAppMenu: true
    $name: Apply to the three-dot menu
    $description: The main Chrome menu and its submenus.
  - applyToTabMenu: true
    $name: Apply to tab context menus
    $description: Right-click menus on tabs, tab groups, and tab strip buttons.
  - applyToBookmarkMenu: true
    $name: Apply to bookmark menus
    $description: Bookmark context menus and bookmark folder drop-downs.
  - applyToOtherMenus: true
    $name: Apply to other menus
    $description: Any remaining Chrome menu, such as toolbar button, omnibox, and download menus.
  - fontSize: "default"
    $name: Font size
    $options:
    - default: Chrome default
    - "12": "12"
    - "13": "13"
    - "14": "14"
    - "15": "15"
    - "16": "16"
    - "17": "17"
    - "18": "18"
    - "19": "19"
    - "20": "20"
    - "21": "21"
    - "22": "22"
    - "23": "23"
    - "24": "24"
  - verticalSpacing: "default"
    $name: Item vertical spacing
    $description: Caps the vertical margin within each menu item.
    $options:
    - default: Chrome default
    - "0": "0"
    - "1": "1"
    - "2": "2"
    - "3": "3"
    - "4": "4"
    - "5": "5"
    - "6": "6"
  - cornerRadius: "8"
    $name: Menu item corner radius
    $description: Rounds the hover and selection background of individual menu items. Does not change the outer menu corners.
    $options:
    - default: Chrome default
    - "0": "0"
    - "1": "1"
    - "2": "2"
    - "3": "3"
    - "4": "4"
    - "5": "5"
    - "6": "6"
    - "7": "7"
    - "8": "8"
    - "9": "9"
    - "10": "10"
    - "11": "11"
    - "12": "12"
  - groupPadding: "4"
    $name: Menu group extra padding
    $description: Adds space on all four sides inside the menu frame. Close and reopen menus after changing settings.
    $options:
    - default: Chrome default
    - "0": "0"
    - "1": "1"
    - "2": "2"
    - "3": "3"
    - "4": "4"
    - "5": "5"
    - "6": "6"
    - "7": "7"
    - "8": "8"
    - "9": "9"
    - "10": "10"
    - "11": "11"
    - "12": "12"
    - "13": "13"
    - "14": "14"
    - "15": "15"
    - "16": "16"
  - groupTopSpacing: "6"
    $name: Menu group top spacing
    $description: Sets inner top spacing while preserving the menu border and shadow. Zero removes extra inner space. Chrome default keeps Chrome spacing plus group extra padding.
    $options:
    - default: Chrome default (follow group padding)
    - "0": "0"
    - "1": "1"
    - "2": "2"
    - "3": "3"
    - "4": "4"
    - "5": "5"
    - "6": "6"
    - "7": "7"
    - "8": "8"
    - "9": "9"
    - "10": "10"
    - "11": "11"
    - "12": "12"
    - "13": "13"
    - "14": "14"
    - "15": "15"
    - "16": "16"
  - groupBottomSpacing: "6"
    $name: Menu group bottom spacing
    $description: Sets inner bottom spacing while preserving the menu border and shadow. Zero removes extra inner space. Chrome default keeps Chrome spacing plus group extra padding.
    $options:
    - default: Chrome default (follow group padding)
    - "0": "0"
    - "1": "1"
    - "2": "2"
    - "3": "3"
    - "4": "4"
    - "5": "5"
    - "6": "6"
    - "7": "7"
    - "8": "8"
    - "9": "9"
    - "10": "10"
    - "11": "11"
    - "12": "12"
    - "13": "13"
    - "14": "14"
    - "15": "15"
    - "16": "16"
  - groupExtensions: true
    $name: Group extensions into one context menu item
    $description: Places extension commands inside an Extensions submenu. Close and reopen the page context menu after changing this setting.
  - extensionsGroupOrder: -2
    $name: Extensions group order
    $description: >-
      Position among visible top-level entries: 1 is first, 2 is second.
      Negative positions count from the bottom: -1 is last, -2 is second-last,
      and so on. 0 appends before negative positions are placed. Hidden entries
      and separators do not count. Used when extension grouping is enabled.
  $name: Settings
- themes:
  - customItemBackgroundColor: false
    $name: Enable custom menu item background color
  - itemBackgroundColor: "#404040"
    $name: Menu item background color (HEX)
    $description: Enter six HEX digits with or without #. Colors standard item backgrounds (including hover/selection) in the enabled menu families and their submenus. Rows without a painted background show the group color. Text and icons retain Chrome's colors. Invalid values use Chrome's color. Close and reopen menus after changing settings.
  - transparency: true
    $name: Enable menu background transparency
    $description: Makes the menu group background translucent while keeping text and icons solid. Applies to the enabled menu families and their submenus. Close and reopen menus after changing settings.
  - backgroundOpacity: "99"
    $name: Menu background opacity (%)
    $description: Used when transparency is enabled. 99 is almost solid; 70 is the most transparent option. Text and icons stay solid. Does not add blur.
    $options:
    - "99": "99"
    - "98": "98"
    - "97": "97"
    - "96": "96"
    - "95": "95"
    - "94": "94"
    - "93": "93"
    - "92": "92"
    - "91": "91"
    - "90": "90"
    - "89": "89"
    - "88": "88"
    - "87": "87"
    - "86": "86"
    - "85": "85"
    - "84": "84"
    - "83": "83"
    - "82": "82"
    - "81": "81"
    - "80": "80"
    - "79": "79"
    - "78": "78"
    - "77": "77"
    - "76": "76"
    - "75": "75"
    - "74": "74"
    - "73": "73"
    - "72": "72"
    - "71": "71"
    - "70": "70"
  - customBackgroundColor: false
    $name: Enable custom menu background color
  - backgroundColor: "#202020"
    $name: Menu background color (HEX)
    $description: Enter six HEX digits, with or without a leading # (for example, #202020). Applies to the enabled menu families and their submenus. Works with background opacity. Invalid values use Chrome's color. Close and reopen menus after changing settings.
  - groupBorder: true
    $name: Show custom menu group border
    $description: Adds a solid 1-DIP rounded outline outside the menu fill, within the shadow margin. Off restores Chrome's native frame. Applies to the enabled menu families and their submenus. Close and reopen menus after changing settings.
  - groupBorderColor: "#3b3b3b"
    $name: Menu group border color (HEX)
    $description: Enter six HEX digits with or without # (for example, #606060). Invalid values use #606060. The border stays solid when background transparency is enabled.
  $name: Themes
- hideContextMenuItems:
  - hideAskGemini: true
    $name: Hide Ask Gemini
    $description: Hides Gemini's page and selected-text context-menu entry.
  - hidePrint: true
    $name: Hide Print
  - hideCast: true
    $name: Hide Cast
  - hideSaveAs: true
    $name: Hide Save as
    $description: Hides Save page as, leaving link, image, and media saving available.
  - hideReadingMode: true
    $name: Hide Open in reading mode
  - hideGoogleLens: true
    $name: Hide Search tab with Google Lens
    $description: Hides the page search entry, leaving image and video-frame search available.
  - hideSendToDevices: true
    $name: Hide Send to your devices
  - hideQrCode: true
    $name: Hide Create QR Code
  - hideTranslate: true
    $name: Hide Translate to language
    $description: Hides page translation regardless of the target language. Selected-text translation remains available.
  - hideTranslateSelection: true
    $name: Hide Translate selection to {LANGUAGE}
    $description: Hides selected-text translation regardless of the target language. Page translation has its own toggle.
  - hideViewSource: false
    $name: Hide View page source
  - hideSaveLinkAs: false
    $name: Hide Save link as
  - hideOpenLinkInSplitView: true
    $name: Hide Open link in split view
  - hideSearchImageWithGoogleLens: true
    $name: Hide Search image with Google Lens
  - hideIcons: false
    $name: Hide menu item icons
    $description: Hides leading icons in the enabled menu families. Reopen menus after changing. Checkmarks and submenu arrows remain visible.
  - hideMainPageMenuIcons: true
    $name: Hide icons only in the main page context menu
    $description: >-
      Hides leading icons only on the first level of the web page right-click
      menu. Submenus, including Extensions, and other Chrome menus keep their
      icons. Independent of appearance scopes. Leave Hide menu item icons off
      to preserve submenu and other-menu icons. Close and reopen menus after changing.
  - hideShortcutLabels: true
    $name: Hide keyboard shortcut labels
    $description: Hides shortcut text in the enabled menu families without disabling keyboard shortcuts. Reopen menus after changing.
  - hiddenCustomItems:
    - - enabled: true
        $name: Enabled
      - label: ""
        $name: Label contains
        $description: >-
          Hides page context-menu items whose label contains this text,
          ignoring letter case. Includes Chrome commands, extension entries,
          submenus, and your custom items. Leave empty to ignore this rule.
          Add another entry for each text fragment you want to hide.
    $name: Hide items by label (partial match)
    $description: Applies to the page right-click menu and its submenus. Close and reopen the menu after changing these rules.
  $name: Hide Context menu items
- customItems:
  - - enabled: true
      $name: Enabled
      $description: Uncheck to keep the entry configured without showing it.
    - label: ""
      $name: Menu item text
      $description: Text shown in the context menu. An entry with no text and no command is ignored.
    - order: 0
      $name: Order
      $description: >-
        Position among all visible top-level menu entries: 1 is first, 2 is
        second. -1 is last, -2 is second-last, and so on. 0 appends before
        negative positions are placed. Separators and hidden entries do not
        count. Equal positions keep settings-list order. Out-of-range positions
        go to the nearest end of the menu.
    - command: ""
      $name: Program, document, or URL
      $description: >-
        Full path to an executable, a document, a folder, or a URL, for example
        C:\Windows\System32\notepad.exe. Environment variables such as
        %ProgramFiles% and %USERPROFILE% are expanded.
    - arguments: ""
      $name: Arguments
      $description: >-
        Command line passed to the program. Quote any argument that contains
        spaces. Environment variables are expanded.
    - workingDirectory: ""
      $name: Working directory
      $description: Optional directory the program starts in. Leave empty to use Chrome's.
    - runAsAdmin: false
      $name: Run as administrator
      $description: Uses the runas verb, which shows the Windows elevation prompt.
    - windowState: normal
      $name: Window state
      $options:
      - normal: Normal
      - minimized: Minimized
      - maximized: Maximized
      - hidden: Hidden
    - separatorBefore: false
      $name: Separator above this item
  $name: Custom context menu items
  $description: >-
    Adds your own entries to the web page context menu. Each entry
    runs a program, document, or URL through the Windows shell when clicked.
    Close and reopen menus after changing these settings.
- customItemsGroup:
  - enabled: false
    $name: Group custom context menu items
    $description: Place your custom commands inside one submenu. Off keeps them as separate page-menu entries. Close and reopen the menu after changing.
  - label: "Custom"
    $name: Group text
    $description: Label for the custom commands submenu. Empty text uses Custom.
  - order: -3
    $name: Group order
    $description: Position of the group in the page menu. 1 is first, -1 is last, -3 is third-last, and 0 appends. Individual command orders apply inside the group.
  $name: Custom items group
- tabs:
  - fontSize: "default"
    $name: Font size
    $options:
    - default: Chrome default
    - "12": "12"
    - "13": "13"
    - "14": "14"
    - "15": "15"
    - "16": "16"
    - "17": "17"
    - "18": "18"
    - "19": "19"
    - "20": "20"
    - "21": "21"
    - "22": "22"
    - "23": "23"
    - "24": "24"
  - hideCloseButtons: false
    $name: Hide close buttons
  - iconTitleSpacing: "8"
    $name: Icon to title spacing
    $options:
    - "2": "2"
    - "3": "3"
    - "4": "4"
    - "5": "5"
    - "6": "6"
    - "7": "7"
    - "8": "8"
  $name: Tabs
- extensions:
  - buttonWidth: "34"
    $name: Button width
    $options:
    - "28": "28"
    - "29": "29"
    - "30": "30"
    - "31": "31"
    - "32": "32"
    - "33": "33"
    - "34": "34"
  $name: Extensions
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Chrome Context Menu Items

Clean up and restyle **Google Chrome's menus**: hide the built-in context menu
entries you never use, add your own commands, group extension entries into one
submenu, and change how menus, tabs and extension buttons look.

![Chrome Context Menu Items demo](https://raw.githubusercontent.com/DanRotaru/windhawk-mods/master/chrome-context-menu-items/screenshots/main.gif)

Designed for 64-bit Chrome on Windows.

## Features

- **Hide built-in items** - individually hide Ask Gemini, Print, Cast, Save page
  as, Open in reading mode, Search with Google Lens, Send to your devices, Create
  QR Code, Translate, Translate selection, View page source, Save link as, Open
  link in split view and Search image with Google Lens.
- **Hide by label** - hide any page menu entry whose label contains a piece of
  text, ignoring case. Works for Chrome commands, extension entries, submenus and
  your own items (up to 64 rules).
- **Custom commands** - add up to 64 entries to the page right-click menu. Each
  one opens a program, document, folder or URL, with optional arguments, working
  directory and environment variables such as `%USERPROFILE%`, and can run as
  administrator or in a minimized, maximized or hidden window.
- **Custom items group** - optionally put your custom commands into one submenu
  with a label of your choice.
- **Extensions submenu** - move all extension commands into a single
  **Extensions** submenu, keeping their icons, submenus, checked states and
  actions.
- **Precise ordering** - place custom items, the Custom group and the Extensions
  submenu at an exact position: `1`, `2`, ... from the top, `-1`, `-2`, ... from the
  bottom, or `0` to append.
- **Menu appearance** - font size, item spacing, item corner radius, group
  padding, top / bottom spacing, custom background colors, background
  transparency and a custom group border.
- **Choose which menus to style** - page right-click, the three-dot menu, tab
  menus, bookmark menus and all other Chrome menus, each with their submenus.
- **Less clutter** - hide leading icons (everywhere, or only in the main page
  menu) and keyboard shortcut labels, without disabling the shortcuts.
- **Tabs and toolbar** - tab title font size, hidden tab close buttons, tab
  icon-to-title spacing and extension button width.
- **No 5 GB download on every update** - hook addresses are prepared once per
  Chrome build and saved (see [How it works](#how-it-works)).

## Screenshots

Hiding built-in context menu items:

![Hide context menu items](https://raw.githubusercontent.com/DanRotaru/windhawk-mods/master/chrome-context-menu-items/screenshots/hide-context-menu-items.gif)

Hiding any item by its label, including extension entries:

![Hide custom context menu items](https://raw.githubusercontent.com/DanRotaru/windhawk-mods/master/chrome-context-menu-items/screenshots/hide-custom-context-menu-items.gif)

Grouping extension commands into one Extensions submenu:

![Group extensions](https://raw.githubusercontent.com/DanRotaru/windhawk-mods/master/chrome-context-menu-items/screenshots/group-extensions-context-menu.gif)

## Default configuration

Out of the box the mod:

* Styles all menu families with an item corner radius of 8, group padding of 4,
  top / bottom spacing of 6, 99% background opacity and a `#3b3b3b` group border.
* Hides Ask Gemini, Print, Cast, Save page as, Open in reading mode, Search with
  Google Lens, Send to your devices, Create QR Code, both Translate entries, Open
  link in split view and Search image with Google Lens. *View page source* and
  *Save link as* stay visible.
* Hides icons in the main page right-click menu and keyboard shortcut labels.
* Groups extension commands into an **Extensions** submenu, second-last.
* Adds no custom commands and leaves tabs and extension buttons at Chrome's
  defaults.

Everything is configurable in the mod settings. Use **Chrome default** or
disable an option to restore its normal behavior.

## How it works

The mod hooks Chrome's own menu code in `chrome.dll`: the page context menu
(`RenderViewContextMenu`) to hide, add, group and reorder entries, and the
shared Views menu classes to change fonts, spacing, colors and borders. Tab and
toolbar tweaks hook Chrome's tab and toolbar views.

These functions aren't exported, so their addresses normally come from Chrome's
debug symbols - a download of several gigabytes which changes with every Chrome
update. Instead, the mod first looks for the current build in its built-in
address table and in the addresses it saved earlier. Only when the build is new
does it fall back to the symbols, then saves the result, so each Chrome build is
prepared at most once. Saved addresses are tied to the exact `chrome.dll` build
and validated before any hook is installed.

### Notes

Close and reopen menus after changing settings. Close open menus before updating
or disabling the mod.
*/
// ==/WindhawkModReadme==
// clang-format on

#include <windows.h>
#include <shellapi.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <climits>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cwchar>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

static constexpr PCWSTR kChromeSymbolServer = L"https://chromium-browser-symsrv.commondatastorage.googleapis.com";
static constexpr DWORD kChromeStartupWaitMs = 5000;
static constexpr wchar_t kChromeWidgetWindowClassPrefix[] = L"Chrome_WidgetWin_";

static constexpr int kChromeDefaultTabPreTitlePadding = 8;
static constexpr int kChromeDefaultExtensionButtonWidth = 34;

// Chrome 151-154 LayoutConstant ordinals for the tab constants used below.
static constexpr int kLayoutTabAfterTitlePadding = 33;
static constexpr int kLayoutTabAlertIndicatorCaptureIconWidth = 34;
static constexpr int kLayoutTabAlertIndicatorIconWidth = 35;
static constexpr int kLayoutTabCloseButtonSize = 36;
static constexpr int kLayoutTabHeight = 37;
static constexpr int kLayoutTabStripHeight = 38;
static constexpr int kLayoutTabStripPadding = 39;
static constexpr int kLayoutTabSeparatorHeight = 40;
static constexpr int kLayoutTabPreTitlePadding = 41;

// -----------------------------------------------------------------------------
// Opaque Chromium types
// -----------------------------------------------------------------------------

struct FontListOpaque;
struct ImageModelOpaque;

struct GfxSizeOpaque {
  int width;
  int height;
};

static_assert(sizeof(GfxSizeOpaque) == 8);

// Chromium object layouts aren't available to this mod at compile time, so use
// generously oversized storage and verify a trailing guard
// after each foreign constructor call. If a guard is overwritten, don't run
// the foreign destructor on an object whose representation may be corrupted.
static constexpr size_t kOpaqueObjectStorageSize = 4096;
static constexpr size_t kOpaqueObjectGuardSize = 256;
static constexpr unsigned char kOpaqueObjectGuardValue = 0xA5;

struct alignas(64) OpaqueObjectStorage {
  unsigned char data[kOpaqueObjectStorageSize];
  unsigned char guard[kOpaqueObjectGuardSize];
};

static void PrepareOpaqueObjectStorage(OpaqueObjectStorage& storage) {
  std::fill_n(storage.guard, kOpaqueObjectGuardSize, kOpaqueObjectGuardValue);
}

static bool IsOpaqueObjectGuardIntact(const OpaqueObjectStorage& storage) {
  return std::all_of(storage.guard, storage.guard + kOpaqueObjectGuardSize,
                     [](unsigned char value) { return value == kOpaqueObjectGuardValue; });
}

// -----------------------------------------------------------------------------
// Function types
// -----------------------------------------------------------------------------

using TypographyGetFontFn = const FontListOpaque* (*)(const void*, int, int);

using LabelSetFontListFn = void (*)(void*, const FontListOpaque*);

// Chrome 152 declares MenuItemView::GetFontList() as returning
// `const gfx::FontList` by value. On Win64 the member-function ABI uses:
// RCX = this, RDX = hidden result buffer.
using MenuItemGetFontListFn = FontListOpaque* (*)(const void*, FontListOpaque*);

using MenuItemGetVerticalMarginFn = int (*)(const void*);

using ContextMenuIsCommandIdVisibleFn = bool (*)(const void*, int);
using MenuItemSetIconFn = void (*)(void*, const ImageModelOpaque*);
using MenuShouldShowAcceleratorTextFn = bool (*)(const void*, const void*, void*);

// Menu-scope tracking. Chrome shows one menu chain per UI thread at a time, so
// the owner that is about to run a menu identifies every item and container
// painted until the next menu runs.
// views::MenuController::Run(Widget*, MenuButtonController*, MenuItemView*,
//   const gfx::Rect&, MenuAnchorPosition, MenuSourceType, MenuType, bool,
//   aura::Window*)
using MenuControllerRunFn = void (*)(void*, void*, void*, void*, const void*,
                                     int, int, int, bool, void*);
// views::View::ShowContextMenuForViewImpl(View*, const gfx::Point&,
//   ui::mojom::MenuSourceType)
using ShowContextMenuForViewFn = void (*)(void*, void*, const void*, int);
using ToolkitDelegateRunMenuAtFn = void (*)(void*, void*, const void*, int);
using AppMenuCtorFn = void (*)(void*, void*, void*, int, void*);
using AppMenuRunMenuButtonFn = void (*)(void*, void*);
using AppMenuRunMenuWidgetFn = void (*)(void*, void*, const void*);
using BookmarkContextMenuCtorFn = void (*)(void*, void*, void*, void*, int,
                                           const void*, bool, bool);
using BookmarkContextMenuRunMenuAtFn = void (*)(void*, const void*, int);
using BookmarkMenuControllerCtorFn = void (*)(void*, void*, void*, const void*,
                                              size_t, bool);
using BookmarkMenuControllerRunMenuAtFn = void (*)(void*, void*);
using TabStripControllerShowContextMenuForTabFn = void (*)(void*, void*,
                                                           const void*, int);

// ui/native_theme/native_theme.h: NativeTheme::MenuItemExtraParams.
struct MenuItemExtraParamsOpaque {
  bool isSelected;
  int cornerRadius;
};
static_assert(sizeof(MenuItemExtraParamsOpaque) == 8);

struct GfxInsetsOpaque {
  int top, left, bottom, right;
};
static_assert(sizeof(GfxInsetsOpaque) == 16);

using PaintMenuItemBackgroundFn = void (*)(const void*, void*, const void*, int,
                                         const void*, const MenuItemExtraParamsOpaque*);
// Win64 member return: RCX = this, RDX = hidden gfx::Insets result.
using MenuContainerGetInsetsFn = GfxInsetsOpaque* (*)(const void*, GfxInsetsOpaque*);
using MenuContainerHasBubbleBorderFn = bool (*)(const void*);

using MenuContainerPaintBackgroundFn = void (*)(void*, void*);
using CanvasSaveLayerAlphaFn = void (*)(void*, uint8_t);
using CanvasRestoreFn = void (*)(void*);
using CanvasDrawColorFn = void (*)(void*, uint32_t, int);
using ColorProviderGetColorFn = uint32_t (*)(const void*, int);
struct GfxRectOpaque { int x, y, width, height; };
struct GfxVector2dOpaque { int x, y; };
static_assert(sizeof(GfxRectOpaque) == 16);
using ViewGetLocalBoundsFn = GfxRectOpaque* (*)(const void*, GfxRectOpaque*);
using MenuContainerGetCornerRadiusFn = int (*)(const void*);
using MenuConfigInstanceFn = const void* (*)();
using MenuConfigCornerRadiusFn = int (*)(const void*, const void*);
using RoundRectPainterDeletingDtorFn = void* (*)(void*, unsigned int);
using RoundRectPainterCtorFn = void (*)(void*, uint32_t, int);
using RoundRectPainterPaintFn = void (*)(void*, void*, const GfxSizeOpaque*);
using CanvasTranslateFn = void (*)(void*, const GfxVector2dOpaque*);
// SkBlendMode::kClear: discard destination pixels in the current paint clip.
static constexpr int kSkBlendModeClear = 0;
// SkBlendMode::kSrcIn: replace RGB while preserving the painted alpha mask.
static constexpr int kSkBlendModeSrcIn = 5;

using FontListCopyCtorFn = void (*)(FontListOpaque*, const FontListOpaque*);

using FontListGetFontSizeFn = int (*)(const FontListOpaque*);

// gfx::FontList is returned through a hidden sret buffer on Win64:
// RCX = this
// RDX = result buffer
// R8  = size delta
using FontListDeriveWithSizeDeltaFn =
    FontListOpaque* (*)(const FontListOpaque*, FontListOpaque*, int);

using OpaqueObjectDtorFn = void (*)(void*);

using FontListDtorFn = void (*)(FontListOpaque*);

using TabTitleCtorFn = void (*)(void*);
using TabTitleDtorFn = void (*)(void*);

using TabCloseButtonCtorFn = void (*)(void*, void*, void*);

using TabCloseButtonDtorFn = void (*)(void*);

using ViewSetVisibleFn = void (*)(void*, bool);

using ViewInvalidateLayoutFn = void (*)(void*, bool);

using ViewPreferredSizeChangedFn = void (*)(void*);

using GetLayoutConstantFn = int (*)(int);

using ToolbarActionViewCtorFn = void (*)(void*, void*, void*);

// gfx::Size is returned through a hidden sret buffer for this Win64 C++
// instance method:
// RCX = this
// RDX = result buffer
// R8  = const views::SizeBounds&
using ToolbarActionViewCalculatePreferredSizeFn = GfxSizeOpaque* (*)(const void*, GfxSizeOpaque*, const void*);

using ToolbarActionViewDeletingDtorFn = void* (*)(void*, unsigned int);

using ToolbarActionViewUpdateStateFn = void (*)(void*);

// ExtensionsToolbarDesktop(Browser*, DisplayMode)
using ExtensionsToolbarDesktopCtorFn = void (*)(void*, void*, int);

using ExtensionsToolbarDesktopDeletingDtorFn = void* (*)(void*, unsigned int);

// -----------------------------------------------------------------------------
// Custom context menu items
// -----------------------------------------------------------------------------

// base::WideToUTF16 returns std::u16string by value. Win64 free-function ABI:
// RCX = hidden result buffer, RDX = const std::wstring_view& (16 bytes, so it
// is passed indirectly).
struct ChromeWideStringPieceOpaque {
  const wchar_t* data;
  size_t size;
};
static_assert(sizeof(ChromeWideStringPieceOpaque) == 16);

using WideToUTF16Fn = void* (*)(void*, const ChromeWideStringPieceOpaque*);

// RenderViewContextMenu::ExecuteCommand(int command_id, int event_flags)
using ContextMenuExecuteCommandFn = void (*)(void*, int, int);

// ToolkitDelegateViews::Init(ui::SimpleMenuModel* menu_model) receives the
// finished model and turns it into views. Taking the model from this argument
// avoids having to know where menu_model_ lives inside the menu object, and
// avoids RenderViewContextMenuBase's AddMenuItem/AddSeparator, whose `this` is
// the RenderViewContextMenuProxy subobject rather than the menu itself.
using ToolkitDelegateInitFn = void (*)(void*, void*);

// ui::SimpleMenuModel::AddItem(int command_id, const std::u16string& label)
using SimpleMenuModelAddItemFn = void (*)(void*, int, const void*);
// ui::SimpleMenuModel::AddSeparator(ui::MenuSeparatorType)
using SimpleMenuModelAddSeparatorFn = void (*)(void*, int);
using SimpleMenuModelInsertItemAtFn = void (*)(void*, size_t, int, const void*);
using SimpleMenuModelInsertSeparatorAtFn = void (*)(void*, size_t, int);
using SimpleMenuModelGetItemCountFn = size_t (*)(const void*);
using SimpleMenuModelGetTypeAtFn = int (*)(const void*, size_t);
using SimpleMenuModelIsVisibleAtFn = bool (*)(const void*, size_t);
using SimpleMenuModelGetLabelAtFn = void* (*)(const void*, void*, size_t);
using SimpleMenuModelGetSubmenuModelAtFn = void* (*)(const void*, size_t);
using AddMenuItemFromModelAtFn = void* (*)(void*, size_t, void*, size_t, int);
using MenuItemViewDtorFn = void (*)(void*);
using SimpleMenuModelCtorFn = void* (*)(void*, void*);
using SimpleMenuModelDtorFn = void (*)(void*);
using SimpleMenuModelInsertSubMenuAtFn = void (*)(void*, size_t, int, const void*, void*);
using PageContextMenuCtorFn = void* (*)(void*, void*, const void*, bool, bool);
using ExtensionMatcherCtorFn = void* (*)(void*, void*, void*, void*, void*);
using ExtensionMatcherDtorFn = void (*)(void*);
static constexpr int kMenuModelTypeSeparator = 3;

// ui::NORMAL_SEPARATOR
static constexpr int kNormalMenuSeparator = 0;

// -----------------------------------------------------------------------------
// Resolved functions
// -----------------------------------------------------------------------------

static TypographyGetFontFn g_TypographyGetFontOriginal;
static LabelSetFontListFn g_LabelSetFontList;

static MenuItemGetFontListFn g_MenuItemGetFontListOriginal;
static MenuItemGetVerticalMarginFn g_MenuItemGetVerticalMarginOriginal;
static ContextMenuIsCommandIdVisibleFn g_ContextMenuIsCommandIdVisibleOriginal;
static ContextMenuIsCommandIdVisibleFn g_ContextMenuIsCommandIdEnabledOriginal;
static ContextMenuIsCommandIdVisibleFn g_ContextMenuIsCommandIdCheckedOriginal;
static ContextMenuExecuteCommandFn g_ContextMenuExecuteCommandOriginal;
static SimpleMenuModelAddItemFn g_SimpleMenuModelAddItem;
static SimpleMenuModelAddSeparatorFn g_SimpleMenuModelAddSeparator;
static SimpleMenuModelInsertItemAtFn g_SimpleMenuModelInsertItemAt;
static SimpleMenuModelInsertSeparatorAtFn g_SimpleMenuModelInsertSeparatorAt;
static SimpleMenuModelGetItemCountFn g_SimpleMenuModelGetItemCount;
static SimpleMenuModelGetTypeAtFn g_SimpleMenuModelGetTypeAt;
static SimpleMenuModelIsVisibleAtFn g_SimpleMenuModelIsVisibleAt;
static SimpleMenuModelGetLabelAtFn g_SimpleMenuModelGetLabelAtOriginal;
static SimpleMenuModelGetSubmenuModelAtFn g_SimpleMenuModelGetSubmenuModelAtOriginal;
static AddMenuItemFromModelAtFn g_AddMenuItemFromModelAtOriginal;
static MenuItemViewDtorFn g_MenuItemViewDtorOriginal;
static SimpleMenuModelCtorFn g_SimpleMenuModelCtor;
static SimpleMenuModelDtorFn g_SimpleMenuModelDtor;
static SimpleMenuModelInsertSubMenuAtFn g_SimpleMenuModelInsertSubMenuAt;
static PageContextMenuCtorFn g_PageContextMenuCtorOriginal;
static ExtensionMatcherCtorFn g_ExtensionMatcherCtorOriginal;
static ExtensionMatcherDtorFn g_ExtensionMatcherDtorOriginal;
static WideToUTF16Fn g_WideToUTF16;
static MenuItemSetIconFn g_MenuItemSetIconOriginal;
static MenuShouldShowAcceleratorTextFn g_MenuShouldShowAcceleratorTextOriginal;

// -----------------------------------------------------------------------------
// Menu scope
// -----------------------------------------------------------------------------

enum MenuScope : int {
  kMenuScopeOther = 0,
  kMenuScopePage,
  kMenuScopeApp,
  kMenuScopeTab,
  kMenuScopeBookmark,
  kMenuScopeCount,
};

static std::atomic_bool g_menuScopeEnabled[kMenuScopeCount];

// Cleared when views::MenuController::Run can't be hooked. Without that latch
// a stale claim would leak into unrelated menus, so scoping is switched off and
// the appearance settings apply to every menu, as they did before scoping.
static std::atomic_bool g_menuScopeDetectionReady = false;

// Set by the owner that is about to build and run a menu, then consumed by
// views::MenuController::Run. It stays set across the owner's build phase so
// that item hooks running before the menu is shown see the right scope.
// -1 means no owner claimed the next menu.
static thread_local int g_pendingMenuScope = -1;
static thread_local int g_activeMenuScope = kMenuScopeOther;

static void SetPendingMenuScope(int scope) { g_pendingMenuScope = scope; }

static int CurrentMenuScope() {
  const int scope = g_pendingMenuScope >= 0 ? g_pendingMenuScope : g_activeMenuScope;
  return scope >= 0 && scope < kMenuScopeCount ? scope : kMenuScopeOther;
}

// Whether the mod's appearance settings apply to the menu being built or drawn.
static bool MenuStylingEnabled() {
  if (!g_menuScopeDetectionReady.load(std::memory_order_relaxed)) return true;
  return g_menuScopeEnabled[CurrentMenuScope()].load(std::memory_order_relaxed);
}

static MenuControllerRunFn g_MenuControllerRunOriginal;
static ToolkitDelegateInitFn g_ToolkitDelegateInitOriginal;
static ToolkitDelegateRunMenuAtFn g_ToolkitDelegateRunMenuAtOriginal;
static AppMenuCtorFn g_AppMenuCtorOriginal;
static AppMenuRunMenuButtonFn g_AppMenuRunMenuButtonOriginal;
static AppMenuRunMenuWidgetFn g_AppMenuRunMenuWidgetOriginal;
static TabStripControllerShowContextMenuForTabFn g_ShowContextMenuForTabOriginal;
static ShowContextMenuForViewFn g_TabStripContextMenuOriginal;
static ShowContextMenuForViewFn g_TabViewContextMenuOriginal;
static ShowContextMenuForViewFn g_TabGroupHeaderContextMenuOriginal;
static ShowContextMenuForViewFn g_TabGroupHeaderViewContextMenuOriginal;
static ShowContextMenuForViewFn g_TabStripComboButtonContextMenuOriginal;
static ShowContextMenuForViewFn g_BookmarkBarContextMenuOriginal;
static BookmarkContextMenuCtorFn g_BookmarkContextMenuCtorOriginal;
static BookmarkContextMenuRunMenuAtFn g_BookmarkContextMenuRunMenuAtOriginal;
static BookmarkMenuControllerCtorFn g_BookmarkMenuControllerCtorOriginal;
static BookmarkMenuControllerRunMenuAtFn g_BookmarkMenuControllerRunMenuAtOriginal;
static void (*g_EmptyImageModelCtor)(void*);
static void (*g_EmptyImageModelDtor)(void*);
static std::atomic_bool g_hideMenuIcons = false;
static std::atomic_bool g_hideMainPageMenuIcons = false;
static std::mutex g_pageMenuIconMutex;
static std::unordered_set<void*> g_pageMenuRootModels;
static std::unordered_set<void*> g_mainPageMenuItemViews;
static std::unordered_set<const void*> g_pageLabelModels;
static std::unordered_map<const void*, std::unordered_set<size_t>> g_hiddenLabelIndices;
static std::mutex g_hiddenLabelPatternsMutex;
static std::vector<std::wstring> g_hiddenLabelPatterns;
static thread_local bool g_buildingMainPageMenuItem = false;
static std::atomic_bool g_hideMenuShortcutLabels = false;
static PaintMenuItemBackgroundFn g_PaintMenuItemBackgroundOriginal;
static MenuContainerGetInsetsFn g_MenuContainerGetInsetsOriginal;
static MenuContainerGetInsetsFn g_ViewGetInsets;
static MenuContainerHasBubbleBorderFn g_MenuContainerHasBubbleBorder;
static MenuContainerPaintBackgroundFn g_MenuContainerPaintBackgroundOriginal;
static MenuContainerPaintBackgroundFn g_ViewOnPaintBorderOriginal;
static thread_local const void* g_pendingMenuBorderView = nullptr;
static CanvasSaveLayerAlphaFn g_CanvasSaveLayerAlpha;
static CanvasRestoreFn g_CanvasRestore;
static CanvasDrawColorFn g_CanvasDrawColor;
static ColorProviderGetColorFn g_ColorProviderGetColorOriginal;
static ViewGetLocalBoundsFn g_ViewGetLocalBounds;
static MenuContainerGetCornerRadiusFn g_MenuContainerGetCornerRadius;
static MenuConfigInstanceFn g_MenuConfigInstance;
static MenuConfigCornerRadiusFn g_MenuConfigCornerRadius;
static RoundRectPainterCtorFn g_RoundRectPainterCtor;
static RoundRectPainterPaintFn g_RoundRectPainterPaint;
static OpaqueObjectDtorFn g_RoundRectPainterDtor;
static RoundRectPainterDeletingDtorFn g_RoundRectPainterDeletingDtor;
static CanvasRestoreFn g_CanvasSave;
static CanvasTranslateFn g_CanvasTranslate;
// Scoped to NativeTheme's single background-color lookup on this thread.
// Never mutate the shared ColorProvider or its cached theme colors.
static thread_local const void* g_menuItemPaintColorProvider = nullptr;
static thread_local bool g_normalizeMenuBackgroundAlpha = false;
static thread_local uint32_t g_menuItemPaintColor = 0;
static FontListCopyCtorFn g_FontListCopyCtor;

static FontListGetFontSizeFn g_FontListGetFontSize;
static FontListDeriveWithSizeDeltaFn g_FontListDeriveWithSizeDelta;
static FontListDtorFn g_FontListDtor;

static TabTitleCtorFn g_TabTitleCtorOriginal;
static TabTitleDtorFn g_TabTitleDtorOriginal;
static TabCloseButtonCtorFn g_TabCloseButtonCtorOriginal;
static TabCloseButtonDtorFn g_TabCloseButtonDtorOriginal;

static ViewSetVisibleFn g_ViewSetVisibleOriginal;
static ViewInvalidateLayoutFn g_ViewInvalidateLayout;
static ViewPreferredSizeChangedFn g_ViewPreferredSizeChanged;
static GetLayoutConstantFn g_GetLayoutConstantOriginal;

static ToolbarActionViewCtorFn g_ToolbarActionViewCtorOriginal;

static ToolbarActionViewCalculatePreferredSizeFn g_ToolbarActionViewCalculatePreferredSizeOriginal;

static ToolbarActionViewDeletingDtorFn g_ToolbarActionViewDeletingDtorOriginal;

static ToolbarActionViewUpdateStateFn g_ToolbarActionViewUpdateState;

static ExtensionsToolbarDesktopCtorFn g_ExtensionsToolbarDesktopCtorOriginal;

static ExtensionsToolbarDesktopDeletingDtorFn g_ExtensionsToolbarDesktopDeletingDtorOriginal;

// -----------------------------------------------------------------------------
// State
// -----------------------------------------------------------------------------

static std::atomic_bool g_chromeSetupStarted = false;
static std::atomic_bool g_hooksActivated = false;
static std::atomic_bool g_chromePreparationSucceeded = false;
// Once the bounded startup wait is abandoned, the resolver may still finish
// to save prepared addresses, but hooks must stay inactive in this
// Chrome instance. Late activation would miss constructor-tracked UI objects.
static std::atomic_bool g_hookActivationAbandoned = false;

// Worker creation and teardown are synchronized so Wh_ModBeforeUninit can't
// miss a thread that is being created concurrently from the chrome.dll load
// hook. g_unloading and g_chromeSetupInProgress are guarded by g_workerMutex.
static std::mutex g_workerMutex;
static std::condition_variable g_workerCondition;
static bool g_unloading = false;
static bool g_chromeSetupInProgress = false;
static HANDLE g_chromePreparationThread;
static HANDLE g_chromePreparationDoneEvent;
static HANDLE g_setupNotificationThread;
static HANDLE g_setupNotificationStopEvent;

static std::atomic_int g_menuFontSize = -1;
static std::atomic_int g_tabFontSize = -1;

static std::atomic_int g_tabPreTitlePadding = kChromeDefaultTabPreTitlePadding;

static std::atomic_int g_extensionButtonWidth = kChromeDefaultExtensionButtonWidth;

// -1 = Chrome default.
static std::atomic_int g_menuVerticalSpacing = -1;

// chrome/app/chrome_command_ids.h. Scope these IDs to RenderViewContextMenu:
// IDs such as Print and Save page are also used by Chrome's application menu.
struct ContextMenuVisibilitySetting {
  PCWSTR setting;
  int commandId;
  std::atomic_bool hidden{false};
};

static ContextMenuVisibilitySetting g_contextMenuVisibility[] = {
    {L"hideContextMenuItems.hideAskGemini", 50234},   // IDC_CONTENT_CONTEXT_GLIC
    {L"hideContextMenuItems.hidePrint", 35003},       // IDC_PRINT
    {L"hideContextMenuItems.hideCast", 35011},        // IDC_ROUTE_MEDIA
    {L"hideContextMenuItems.hideSaveAs", 35004},      // IDC_SAVE_PAGE
    {L"hideContextMenuItems.hideReadingMode", 50169}, // IDC_CONTENT_CONTEXT_OPEN_IN_READING_MODE
    {L"hideContextMenuItems.hideGoogleLens", 50174},  // IDC_CONTENT_CONTEXT_LENS_REGION_SEARCH
    {L"hideContextMenuItems.hideSendToDevices", 35016}, // IDC_SEND_TAB_TO_SELF
    {L"hideContextMenuItems.hideQrCode", 51034},       // IDC_CONTENT_CONTEXT_GENERATE_QR_CODE
    {L"hideContextMenuItems.hideTranslate", 50161},    // IDC_CONTENT_CONTEXT_TRANSLATE
    {L"hideContextMenuItems.hideTranslateSelection", 50179}, // IDC_CONTENT_CONTEXT_PARTIAL_TRANSLATE
    {L"hideContextMenuItems.hideViewSource", 35002},   // IDC_VIEW_SOURCE
    {L"hideContextMenuItems.hideSaveLinkAs", 50103},   // IDC_CONTENT_CONTEXT_SAVELINKAS
    {L"hideContextMenuItems.hideOpenLinkInSplitView", 50111}, // IDC_CONTENT_CONTEXT_OPENLINKSPLITVIEW
    {L"hideContextMenuItems.hideSearchImageWithGoogleLens", 50127}, // IDC_CONTENT_CONTEXT_SEARCHLENSFORIMAGE
};
static std::atomic_int g_menuCornerRadius = -1;
// 100 bypasses the extra paint layer entirely (disabled/default).
static std::atomic_int g_menuBackgroundOpacity = 100;
// Zero means native color; custom colors always have an opaque alpha byte.
static std::atomic<uint32_t> g_menuBackgroundColor = 0;
static std::atomic<uint32_t> g_menuItemBackgroundColor = 0;
// A single atomic contains both the enabled state and opaque border color.
static std::atomic<uint32_t> g_menuGroupBorderColor = 0;
static std::atomic_int g_menuGroupPadding = -1;
static std::atomic_int g_menuGroupTopSpacing = -1;
static std::atomic_int g_menuGroupBottomSpacing = -1;

static std::atomic_bool g_tabCloseButtonsHidden = false;

static std::atomic_bool g_tabFontHooksReady = false;
static std::atomic_bool g_tabCloseHooksReady = false;

static std::atomic_bool g_extensionTrackingReady = false;
static std::atomic_bool g_extensionContainerTrackingReady = false;

static void ClearChromeRuntimeReadiness() {
  g_tabFontHooksReady.store(false, std::memory_order_release);
  g_tabCloseHooksReady.store(false, std::memory_order_release);
  g_extensionTrackingReady.store(false, std::memory_order_release);
  g_extensionContainerTrackingReady.store(false, std::memory_order_release);
}

//  0 = not checked
//  1 = compatible
// -1 = incompatible
static std::atomic_int g_tabLayoutCompatibility = 0;

static thread_local int g_tabTitleCtorDepth = 0;
static thread_local const FontListOpaque* g_pendingTabTitleOriginalFont = nullptr;

struct TabTitleInfo {
  DWORD threadId;
  std::unique_ptr<OpaqueObjectStorage> originalFontStorage;
};

static std::mutex g_tabObjectsMutex;
static std::unordered_map<void*, TabTitleInfo> g_tabTitles;
static std::unordered_map<void*, DWORD> g_tabCloseButtons;

static std::mutex g_extensionViewsMutex;
static std::unordered_map<void*, DWORD> g_extensionViews;
static std::unordered_map<void*, DWORD> g_extensionContainers;

// -----------------------------------------------------------------------------
// Custom context menu items: state, labels, and launching
// -----------------------------------------------------------------------------

// Command IDs are chosen well above every ID in chrome/app/chrome_command_ids.h
// and above the ranges Chrome reserves for plugin and extension custom items,
// so a click can be told apart from a genuine Chrome command.
static constexpr int kCustomMenuCommandIdFirst = 61000;
static constexpr int kMaxCustomMenuItems = 64;
static constexpr int kExtensionsGroupCommandId = kCustomMenuCommandIdFirst + kMaxCustomMenuItems;
static constexpr int kCustomItemsGroupCommandId = kExtensionsGroupCommandId + 1;

static std::atomic_bool g_groupExtensions = false;
static std::atomic_int g_extensionsGroupOrder = 0;
static thread_local bool g_constructingPageContextMenu = false;

struct ExtensionGroup {
  void* rootModel;
  std::unique_ptr<OpaqueObjectStorage> model;
  const void* label;
  bool inserted = false;
};
static std::mutex g_extensionGroupsMutex;
// Chrome's matcher owns the extension submenus; we own only their new parent.
static std::unordered_map<void*, ExtensionGroup> g_extensionGroups;

struct CustomMenuItem {
  int commandId;
  int order;
  std::wstring label;
  std::wstring command;
  std::wstring arguments;
  std::wstring workingDirectory;
  bool runAsAdmin;
  int showCmd;
  bool separatorBefore;
  void* submenu = nullptr;
  const void* nativeLabel = nullptr;
};

static std::mutex g_customItemsMutex;
static std::vector<CustomMenuItem> g_customItems;

struct CustomItemsGroupSettings {
  bool enabled = false;
  std::wstring label = L"Custom";
  int order = -3;
};
static CustomItemsGroupSettings g_customItemsGroupSettings;  // g_customItemsMutex
static std::mutex g_customGroupsMutex;
static bool g_customGroupTrackingEnabled = true;  // g_customGroupsMutex
// Remember Chrome's actual delegate without depending on private object layouts.
static std::unordered_map<void*, void*> g_menuModelDelegates;
// The root model owns the lifetime of our native submenu storage.
static std::unordered_map<void*, std::unique_ptr<OpaqueObjectStorage>> g_customGroups;


static std::atomic_bool g_customMenuItemsReady = false;

// Chrome-side std::u16string labels. Their internal heap buffer belongs to
// Chrome's allocator and the destructor is inlined into Chrome, so the objects
// are kept alive for the process lifetime instead of being destroyed here. The
// cache is keyed by text so repeated settings changes can't grow it without
// bound.
static std::mutex g_customLabelMutex;
static std::unordered_map<std::wstring, OpaqueObjectStorage*> g_customLabels;

static std::mutex g_customLaunchMutex;
static std::vector<HANDLE> g_customLaunchThreads;
static bool g_customLaunchDisabled = false;

static bool IsCustomMenuCommandId(int commandId) {
  return commandId >= kCustomMenuCommandIdFirst &&
         commandId < kCustomMenuCommandIdFirst + kMaxCustomMenuItems;
}

// libc++ stores a std::u16string either inline (short) or as
// {data, size, capacity}, with the "is long" flag in the top bit of the last
// byte and, for short strings, the length in that byte's low 7 bits. Confirm
// the object Chrome just built matches that before letting Chrome copy it: a
// bad size field would turn into an enormous allocation and abort the browser.
static bool IsValidChromeU16String(const unsigned char* object, size_t expectedLength) {
  constexpr size_t kU16StringSize = 24;
  constexpr size_t kShortCapacity = 10;

  const unsigned char lastByte = object[kU16StringSize - 1];

  if (!(lastByte & 0x80)) {
    return lastByte == expectedLength && expectedLength <= kShortCapacity;
  }

  size_t data = 0;
  size_t size = 0;
  memcpy(&data, object, sizeof(data));
  memcpy(&size, object + sizeof(data), sizeof(size));

  return data && size == expectedLength && expectedLength > kShortCapacity;
}

// Returns a Chrome-owned std::u16string holding |text|, or nullptr.
static const void* GetCustomMenuItemLabel(const std::wstring& text) {
  if (!g_WideToUTF16) return nullptr;

  std::lock_guard<std::mutex> lock(g_customLabelMutex);

  auto existing = g_customLabels.find(text);
  if (existing != g_customLabels.end()) {
    return existing->second ? existing->second->data : nullptr;
  }

  auto storage = std::make_unique<OpaqueObjectStorage>();
  PrepareOpaqueObjectStorage(*storage);

  const ChromeWideStringPieceOpaque piece{text.c_str(), text.size()};
  g_WideToUTF16(storage->data, &piece);

  // The string representation isn't what this mod expects. Don't hand the
  // object to Chrome, and stop offering custom items for this session.
  if (!IsOpaqueObjectGuardIntact(*storage) ||
      !IsValidChromeU16String(storage->data, text.size())) {
    Wh_Log(L"Unexpected u16string representation; disabling custom menu items");
    g_customMenuItemsReady.store(false, std::memory_order_release);
    g_customLabels[text] = nullptr;
    return nullptr;
  }

  OpaqueObjectStorage* raw = storage.release();
  g_customLabels[text] = raw;
  return raw->data;
}

static std::wstring ExpandEnvironmentPlaceholders(const std::wstring& value) {
  if (value.find(L'%') == std::wstring::npos) return value;

  DWORD needed = ExpandEnvironmentStringsW(value.c_str(), nullptr, 0);
  if (!needed) return value;

  std::wstring expanded(needed, L'\0');
  DWORD written = ExpandEnvironmentStringsW(value.c_str(), expanded.data(), needed);
  if (!written || written > needed) return value;

  expanded.resize(written - 1);
  return expanded;
}

static std::wstring TrimCustomItemSetting(PCWSTR value) {
  std::wstring result = value ? value : L"";

  static constexpr wchar_t kWhitespace[] = L" \t\r\n";

  size_t begin = result.find_first_not_of(kWhitespace);
  if (begin == std::wstring::npos) return std::wstring();

  size_t end = result.find_last_not_of(kWhitespace);
  result = result.substr(begin, end - begin + 1);

  // A path pasted from Explorer often arrives wrapped in quotes, which
  // ShellExecuteExW would treat as part of the file name.
  if (result.size() >= 2 && result.front() == L'"' && result.back() == L'"') {
    result = result.substr(1, result.size() - 2);
  }

  return result;
}

struct CustomMenuItemLaunch {
  std::wstring file;
  std::wstring arguments;
  std::wstring workingDirectory;
  bool runAsAdmin;
  int showCmd;
};

static DWORD WINAPI CustomMenuItemLaunchThread(LPVOID parameter) {
  std::unique_ptr<CustomMenuItemLaunch> request(
      static_cast<CustomMenuItemLaunch*>(parameter));

  // ShellExecuteExW needs an initialized apartment, and running it off Chrome's
  // UI thread keeps a slow shell handler from freezing the browser.
  const HRESULT comResult =
      CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

  SHELLEXECUTEINFOW info = {};
  info.cbSize = sizeof(info);
  info.fMask = SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
  info.lpVerb = request->runAsAdmin ? L"runas" : nullptr;
  info.lpFile = request->file.c_str();
  info.lpParameters =
      request->arguments.empty() ? nullptr : request->arguments.c_str();
  info.lpDirectory = request->workingDirectory.empty()
                         ? nullptr
                         : request->workingDirectory.c_str();
  info.nShow = request->showCmd;

  if (ShellExecuteExW(&info)) {
    Wh_Log(L"Launched custom menu item: %ls", request->file.c_str());
  } else {
    Wh_Log(L"Failed to launch '%ls': error %lu", request->file.c_str(),
           GetLastError());
  }

  if (info.hProcess) CloseHandle(info.hProcess);

  if (SUCCEEDED(comResult)) CoUninitialize();

  return 0;
}

static void RunCustomMenuItem(int commandId) {
  auto request = std::make_unique<CustomMenuItemLaunch>();

  {
    std::lock_guard<std::mutex> lock(g_customItemsMutex);

    auto item = std::find_if(g_customItems.begin(), g_customItems.end(),
                             [commandId](const CustomMenuItem& candidate) {
                               return candidate.commandId == commandId;
                             });

    if (item == g_customItems.end() || item->command.empty()) return;

    request->file = ExpandEnvironmentPlaceholders(item->command);
    request->arguments = ExpandEnvironmentPlaceholders(item->arguments);
    request->workingDirectory =
        ExpandEnvironmentPlaceholders(item->workingDirectory);
    request->runAsAdmin = item->runAsAdmin;
    request->showCmd = item->showCmd;
  }

  std::lock_guard<std::mutex> lock(g_customLaunchMutex);

  // No new worker may start once the mod begins unloading; the thread runs code
  // that lives in this module.
  if (g_customLaunchDisabled) return;

  // Reap workers that already finished so the list can't grow without bound.
  g_customLaunchThreads.erase(
      std::remove_if(g_customLaunchThreads.begin(), g_customLaunchThreads.end(),
                     [](HANDLE thread) {
                       if (WaitForSingleObject(thread, 0) != WAIT_OBJECT_0) {
                         return false;
                       }
                       CloseHandle(thread);
                       return true;
                     }),
      g_customLaunchThreads.end());

  HANDLE thread = CreateThread(nullptr, 0, CustomMenuItemLaunchThread,
                               request.get(), 0, nullptr);

  if (!thread) {
    Wh_Log(L"Failed to create launch thread: error %lu", GetLastError());
    return;
  }

  request.release();
  g_customLaunchThreads.push_back(thread);
}

static void WaitForCustomMenuItemLaunches() {
  std::vector<HANDLE> threads;

  {
    std::lock_guard<std::mutex> lock(g_customLaunchMutex);
    g_customLaunchDisabled = true;
    threads.swap(g_customLaunchThreads);
  }

  for (HANDLE thread : threads) {
    // Waiting without a timeout is required: abandoning a worker that is still
    // inside this module's code would crash Chrome once the mod is unloaded.
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
  }
}

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

// Windhawk settings metadata is static, while Chrome's actual default font
// sizes are only known after the native UI objects exist. If Windhawk gains a
// runtime settings-metadata/read-only-value API, the "Chrome default" labels
// could show the detected size instead of the static default label.
// Numeric options are absolute target sizes; -1 means restore Chrome's exact
// original FontList for that object.
static int ReadFontSizeSetting(PCWSTR name) {
  auto value = WindhawkUtils::StringSetting::make(name);

  if (!*value.get() || wcscmp(value.get(), L"default") == 0) return -1;

  int result = _wtoi(value.get());

  return result >= 12 && result <= 24 ? result : -1;
}

static int ReadMenuSpacingSetting() {
  auto value = WindhawkUtils::StringSetting::make(L"menus.verticalSpacing");

  if (!*value.get() || wcscmp(value.get(), L"default") == 0) return -1;

  int result = _wtoi(value.get());

  return result >= 0 && result <= 6 ? result : -1;
}

static int ReadMenuCornerRadiusSetting() {
  auto value = WindhawkUtils::StringSetting::make(L"menus.cornerRadius");

  if (!*value.get() || wcscmp(value.get(), L"default") == 0) return -1;

  wchar_t* end = nullptr;
  long result = wcstol(value.get(), &end, 10);
  return end != value.get() && !*end && result >= 0 && result <= 12
             ? static_cast<int>(result) : -1;
}

static int ReadMenuGroupPaddingSetting(PCWSTR name) {
  auto value = WindhawkUtils::StringSetting::make(name);
  if (!*value.get() || wcscmp(value.get(), L"default") == 0) return -1;
  wchar_t* end = nullptr;
  long result = wcstol(value.get(), &end, 10);
  return end != value.get() && !*end && result >= 0 && result <= 16
             ? static_cast<int>(result) : -1;
}

static int ReadMenuBackgroundOpacitySetting() {
  if (!Wh_GetIntSetting(L"themes.transparency")) return 100;
  auto value = WindhawkUtils::StringSetting::make(L"themes.backgroundOpacity");
  wchar_t* end = nullptr;
  long result = wcstol(value.get(), &end, 10);
  if (end != value.get() && !*end) {
    // Clamp older saved values below 70 instead of retaining excessive transparency.
    return static_cast<int>(std::clamp(result, 70L, 99L));
  }
  return 99;
}

static uint32_t ParseMenuBackgroundColor(PCWSTR value) {
  if (*value == L'#') ++value;
  if (wcslen(value) != 6) return 0;
  uint32_t color = 0;
  for (int i = 0; i < 6; ++i) {
    const wchar_t c = value[i];
    unsigned digit;
    if (c >= L'0' && c <= L'9') digit = c - L'0';
    else if (c >= L'a' && c <= L'f') digit = c - L'a' + 10;
    else if (c >= L'A' && c <= L'F') digit = c - L'A' + 10;
    else return 0;
    color = (color << 4) | digit;
  }
  return 0xFF000000u | color;
}

static uint32_t ReadMenuBackgroundColorSetting() {
  if (!Wh_GetIntSetting(L"themes.customBackgroundColor")) return 0;
  auto value = WindhawkUtils::StringSetting::make(L"themes.backgroundColor");
  const uint32_t color = ParseMenuBackgroundColor(value.get());
  if (!color) Wh_Log(L"Invalid menu background HEX color; using Chrome's color");
  return color;
}

static uint32_t ReadMenuItemBackgroundColorSetting() {
  if (!Wh_GetIntSetting(L"themes.customItemBackgroundColor")) return 0;
  auto value = WindhawkUtils::StringSetting::make(L"themes.itemBackgroundColor");
  const uint32_t color = ParseMenuBackgroundColor(value.get());
  if (!color) Wh_Log(L"Invalid menu item background HEX color; using Chrome's color");
  return color;
}

static uint32_t ReadMenuGroupBorderColorSetting() {
  if (!Wh_GetIntSetting(L"themes.groupBorder")) return 0;
  auto value = WindhawkUtils::StringSetting::make(L"themes.groupBorderColor");
  const uint32_t color = ParseMenuBackgroundColor(value.get());
  if (!color) Wh_Log(L"Invalid menu border HEX color; using #606060");
  return color ? color : 0xFF606060u;
}

static int ReadTabPreTitlePaddingSetting() {
  auto value = WindhawkUtils::StringSetting::make(L"tabs.iconTitleSpacing");
  int result = *value.get() ? _wtoi(value.get()) : kChromeDefaultTabPreTitlePadding;

  if (result < 2 || result > 8) result = kChromeDefaultTabPreTitlePadding;

  return result;
}

static int ReadExtensionButtonWidthSetting() {
  auto value = WindhawkUtils::StringSetting::make(L"extensions.buttonWidth");
  int result = *value.get() ? _wtoi(value.get()) : kChromeDefaultExtensionButtonWidth;

  if (result < 28 || result > 34) result = kChromeDefaultExtensionButtonWidth;

  return result;
}

static int ReadCustomItemWindowState(int index) {
  auto value =
      WindhawkUtils::StringSetting::make(L"customItems[%d].windowState", index);

  if (wcscmp(value.get(), L"minimized") == 0) return SW_SHOWMINNOACTIVE;
  if (wcscmp(value.get(), L"maximized") == 0) return SW_SHOWMAXIMIZED;
  if (wcscmp(value.get(), L"hidden") == 0) return SW_HIDE;

  return SW_SHOWNORMAL;
}

static std::vector<std::wstring> ReadHiddenCustomItemLabels() {
  std::vector<std::wstring> labels;
  for (int i = 0; i < kMaxCustomMenuItems; ++i) {
    if (!Wh_GetIntSetting(L"hideContextMenuItems.hiddenCustomItems[%d].enabled", i)) continue;
    auto value = WindhawkUtils::StringSetting::make(
        L"hideContextMenuItems.hiddenCustomItems[%d].label", i);
    std::wstring label = value.get();
    const size_t first = label.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) continue;
    const size_t last = label.find_last_not_of(L" \t\r\n");
    labels.push_back(label.substr(first, last - first + 1));
  }
  return labels;
}

static std::wstring StripMenuMnemonics(const std::wstring& label) {
  std::wstring result;
  for (size_t i = 0; i < label.size(); ++i) {
    if (label[i] == L'&') {
      if (i + 1 < label.size() && label[i + 1] == L'&') {
        result += L'&';
        ++i;
      }
    } else {
      result += label[i];
    }
  }
  return result;
}

static bool ShouldHideCustomItemLabel(const std::wstring& rawLabel,
                                       const std::vector<std::wstring>& patterns) {
  const auto label = StripMenuMnemonics(rawLabel);
  for (const auto& pattern : patterns) {
    if (pattern.empty() || pattern.size() > label.size() || pattern.size() > INT_MAX) continue;
    const int length = static_cast<int>(pattern.size());
    for (size_t offset = 0; offset <= label.size() - pattern.size(); ++offset) {
      if (CompareStringOrdinal(label.data() + offset, length, pattern.data(), length,
                               TRUE) == CSTR_EQUAL) {
        return true;
      }
    }
  }
  return false;
}

static void LoadCustomMenuItems() {
  std::vector<CustomMenuItem> items;
  const auto hiddenLabels = ReadHiddenCustomItemLabels();
  {
    std::lock_guard<std::mutex> lock(g_hiddenLabelPatternsMutex);
    g_hiddenLabelPatterns = hiddenLabels;
  }

  for (int i = 0; i < kMaxCustomMenuItems; ++i) {
    auto rawLabel = WindhawkUtils::StringSetting::make(L"customItems[%d].label", i);
    auto rawCommand =
        WindhawkUtils::StringSetting::make(L"customItems[%d].command", i);

    std::wstring label = TrimCustomItemSetting(rawLabel.get());
    std::wstring command = TrimCustomItemSetting(rawCommand.get());

    // Windhawk returns the template defaults past the end of the array, so
    // entries with neither text nor command are simply skipped.
    if (label.empty() && command.empty()) continue;

    if (!Wh_GetIntSetting(L"customItems[%d].enabled", i)) continue;
    if (ShouldHideCustomItemLabel(label, hiddenLabels)) continue;

    if (label.empty() || command.empty()) {
      Wh_Log(L"Custom menu item %d needs both text and a command; skipping", i);
      continue;
    }

    CustomMenuItem item = {};
    item.commandId = kCustomMenuCommandIdFirst + i;
    item.order = Wh_GetIntSetting(L"customItems[%d].order", i);
    item.label = std::move(label);
    item.command = std::move(command);
    item.arguments = TrimCustomItemSetting(
        WindhawkUtils::StringSetting::make(L"customItems[%d].arguments", i).get());
    item.workingDirectory = TrimCustomItemSetting(
        WindhawkUtils::StringSetting::make(L"customItems[%d].workingDirectory", i)
            .get());
    item.runAsAdmin = Wh_GetIntSetting(L"customItems[%d].runAsAdmin", i) != 0;
    item.showCmd = ReadCustomItemWindowState(i);
    item.separatorBefore =
        Wh_GetIntSetting(L"customItems[%d].separatorBefore", i) != 0;

    Wh_Log(L"Custom menu item %d: '%ls' -> '%ls'", i, item.label.c_str(),
           item.command.c_str());

    items.push_back(std::move(item));
  }

  CustomItemsGroupSettings grouping;
  grouping.enabled = Wh_GetIntSetting(L"customItemsGroup.enabled") != 0;
  grouping.label = TrimCustomItemSetting(
      WindhawkUtils::StringSetting::make(L"customItemsGroup.label").get());
  if (grouping.label.empty()) grouping.label = L"Custom";
  grouping.order = Wh_GetIntSetting(L"customItemsGroup.order");
  std::lock_guard<std::mutex> lock(g_customItemsMutex);
  g_customItems = std::move(items);
  g_customItemsGroupSettings = std::move(grouping);
}

static void LoadSettings() {
  g_groupExtensions.store(Wh_GetIntSetting(L"menus.groupExtensions") != 0);
  g_extensionsGroupOrder.store(Wh_GetIntSetting(L"menus.extensionsGroupOrder"));
  const struct {
    MenuScope scope;
    PCWSTR setting;
    PCWSTR label;
  } menuScopeSettings[] = {
      {kMenuScopePage, L"menus.applyToPageMenu", L"page"},
      {kMenuScopeApp, L"menus.applyToAppMenu", L"three-dot"},
      {kMenuScopeTab, L"menus.applyToTabMenu", L"tabs"},
      {kMenuScopeBookmark, L"menus.applyToBookmarkMenu", L"bookmarks"},
      {kMenuScopeOther, L"menus.applyToOtherMenus", L"other"},
  };
  for (const auto& item : menuScopeSettings) {
    const bool enabled = Wh_GetIntSetting(item.setting) != 0;
    g_menuScopeEnabled[item.scope].store(enabled, std::memory_order_relaxed);
    Wh_Log(L"Menu appearance for %ls menus: %ls", item.label,
           enabled ? L"enabled" : L"disabled");
  }
  LoadCustomMenuItems();

  g_hideMenuIcons.store(Wh_GetIntSetting(L"hideContextMenuItems.hideIcons") != 0, std::memory_order_relaxed);
  g_hideMainPageMenuIcons.store(Wh_GetIntSetting(L"hideContextMenuItems.hideMainPageMenuIcons") != 0,
                               std::memory_order_relaxed);
  g_hideMenuShortcutLabels.store(Wh_GetIntSetting(L"hideContextMenuItems.hideShortcutLabels") != 0,
                                 std::memory_order_relaxed);
  for (auto& item : g_contextMenuVisibility) {
    item.hidden.store(Wh_GetIntSetting(item.setting) != 0, std::memory_order_relaxed);
  }

  int menuFontSize = ReadFontSizeSetting(L"menus.fontSize");

  int menuVerticalSpacing = ReadMenuSpacingSetting();

  int menuCornerRadius = ReadMenuCornerRadiusSetting();

  int tabFontSize = ReadFontSizeSetting(L"tabs.fontSize");

  bool tabCloseButtonsHidden = Wh_GetIntSetting(L"tabs.hideCloseButtons") != 0;

  int tabPreTitlePadding = ReadTabPreTitlePaddingSetting();

  int extensionButtonWidth = ReadExtensionButtonWidthSetting();

  g_menuFontSize.store(menuFontSize, std::memory_order_relaxed);

  g_menuVerticalSpacing.store(menuVerticalSpacing, std::memory_order_relaxed);

  g_menuCornerRadius.store(menuCornerRadius, std::memory_order_relaxed);
  g_menuBackgroundOpacity.store(ReadMenuBackgroundOpacitySetting(), std::memory_order_relaxed);
  g_menuBackgroundColor.store(ReadMenuBackgroundColorSetting(), std::memory_order_relaxed);
  g_menuItemBackgroundColor.store(ReadMenuItemBackgroundColorSetting(), std::memory_order_relaxed);
  g_menuGroupBorderColor.store(ReadMenuGroupBorderColorSetting(), std::memory_order_relaxed);
  g_menuGroupPadding.store(ReadMenuGroupPaddingSetting(L"menus.groupPadding"), std::memory_order_relaxed);
  g_menuGroupTopSpacing.store(ReadMenuGroupPaddingSetting(L"menus.groupTopSpacing"), std::memory_order_relaxed);
  g_menuGroupBottomSpacing.store(ReadMenuGroupPaddingSetting(L"menus.groupBottomSpacing"), std::memory_order_relaxed);

  g_tabFontSize.store(tabFontSize, std::memory_order_relaxed);

  g_tabCloseButtonsHidden.store(tabCloseButtonsHidden, std::memory_order_relaxed);

  g_tabPreTitlePadding.store(tabPreTitlePadding, std::memory_order_relaxed);

  g_extensionButtonWidth.store(extensionButtonWidth, std::memory_order_relaxed);

  if (menuFontSize < 0) {
    Wh_Log(L"Menu font size: Chrome default");
  } else {
    Wh_Log(L"Menu font size: %d", menuFontSize);
  }

  if (menuVerticalSpacing < 0) {
    Wh_Log(L"Menu vertical spacing: Chrome default");
  } else {
    Wh_Log(L"Menu vertical spacing: %d", menuVerticalSpacing);
  }

  if (menuCornerRadius < 0) {
    Wh_Log(L"Menu item corner radius: Chrome default");
  } else {
    Wh_Log(L"Menu item corner radius: %d", menuCornerRadius);
  }

  if (tabFontSize < 0) {
    Wh_Log(L"Tab title font size: Chrome default");
  } else {
    Wh_Log(L"Tab title font size: %d", tabFontSize);
  }

  Wh_Log(L"Tab close buttons: %ls", tabCloseButtonsHidden ? L"Hidden" : L"Chrome default");

  Wh_Log(L"Tab icon/title spacing: %d", tabPreTitlePadding);

  Wh_Log(L"Extension button width: %d", extensionButtonWidth);
}

// -----------------------------------------------------------------------------
// Fonts
// -----------------------------------------------------------------------------

static const FontListOpaque* GetOwnedFontList(
    const std::unique_ptr<OpaqueObjectStorage>& storage) {
  return storage ? reinterpret_cast<const FontListOpaque*>(storage->data) : nullptr;
}

static std::unique_ptr<OpaqueObjectStorage> CopyFontListToOwnedStorage(
    const FontListOpaque* font, const wchar_t* description) {
  if (!font || !g_FontListCopyCtor || !g_FontListDtor) return nullptr;

  auto storage = std::make_unique<OpaqueObjectStorage>();
  PrepareOpaqueObjectStorage(*storage);

  auto* copy = reinterpret_cast<FontListOpaque*>(storage->data);
  g_FontListCopyCtor(copy, font);

  if (!IsOpaqueObjectGuardIntact(*storage)) {
    Wh_Log(L"%ls storage guard was overwritten; skipping destructor", description);
    return nullptr;
  }

  return storage;
}

static void DestroyOwnedFontListStorage(
    std::unique_ptr<OpaqueObjectStorage> storage, const wchar_t* description) {
  if (!storage || !g_FontListDtor) return;

  if (!IsOpaqueObjectGuardIntact(*storage)) {
    Wh_Log(L"%ls storage guard was overwritten; skipping destructor", description);
    return;
  }

  g_FontListDtor(reinterpret_cast<FontListOpaque*>(storage->data));
}

static std::unique_ptr<OpaqueObjectStorage> DeriveFontListToOwnedStorage(
    const FontListOpaque* originalFont,
    int targetSize,
    const wchar_t* description) {
  if (!originalFont || !g_FontListGetFontSize || !g_FontListDeriveWithSizeDelta ||
      !g_FontListDtor) {
    return nullptr;
  }

  int originalSize = g_FontListGetFontSize(originalFont);

  if (targetSize == originalSize) return nullptr;

  auto storage = std::make_unique<OpaqueObjectStorage>();
  PrepareOpaqueObjectStorage(*storage);

  auto* derivedFont = reinterpret_cast<FontListOpaque*>(storage->data);
  g_FontListDeriveWithSizeDelta(originalFont, derivedFont, targetSize - originalSize);

  if (!IsOpaqueObjectGuardIntact(*storage)) {
    Wh_Log(L"%ls derived FontList storage guard was overwritten; skipping destructor",
           description);
    return nullptr;
  }

  return storage;
}

static bool SetLabelFontForTargetSize(void* label,
                                      const FontListOpaque* originalFont,
                                      int targetSize,
                                      const wchar_t* description) {
  if (!label || !originalFont || !g_LabelSetFontList) return false;

  // "Chrome default" restores the exact FontList that Chrome originally chose
  // for this object. Numeric settings are absolute target sizes.
  if (targetSize < 0) {
    g_LabelSetFontList(label, originalFont);
    return true;
  }

  if (!g_FontListGetFontSize || !g_FontListDeriveWithSizeDelta || !g_FontListDtor) {
    return false;
  }

  int originalSize = g_FontListGetFontSize(originalFont);

  if (targetSize == originalSize) {
    g_LabelSetFontList(label, originalFont);
    return true;
  }

  auto derivedFontStorage =
      DeriveFontListToOwnedStorage(originalFont, targetSize, description);

  if (!derivedFontStorage) return false;

  g_LabelSetFontList(label, GetOwnedFontList(derivedFontStorage));
  DestroyOwnedFontListStorage(std::move(derivedFontStorage), description);
  return true;
}

static const FontListOpaque* TypographyGetFontHook(const void* self, int context, int style) {
  const FontListOpaque* originalFont = g_TypographyGetFontOriginal(self, context, style);

  if (g_tabTitleCtorDepth > 0 && !g_pendingTabTitleOriginalFont) {
    g_pendingTabTitleOriginalFont = originalFont;
  }

  return originalFont;
}

// -----------------------------------------------------------------------------
// Menu scope hooks
// -----------------------------------------------------------------------------

// Each owner claims the next menu before it builds and runs it. The claim is
// sticky rather than scoped to the call: Chrome builds some menus (notably the
// app menu) in a separate call from the one that shows them, and nothing else
// opens a menu on this thread in between.
static void MenuControllerRunHook(void* self, void* parent, void* button,
                                  void* rootItem, const void* bounds, int anchor,
                                  int sourceType, int menuType, bool forDrop,
                                  void* nativeView) {
  g_activeMenuScope = g_pendingMenuScope >= 0 ? g_pendingMenuScope : kMenuScopeOther;
  g_pendingMenuScope = -1;
  g_MenuControllerRunOriginal(self, parent, button, rootItem, bounds, anchor,
                              sourceType, menuType, forDrop, nativeView);
}

static void ToolkitDelegateRunMenuAtHook(void* self, void* widget,
                                         const void* point, int sourceType) {
  SetPendingMenuScope(kMenuScopePage);
  g_ToolkitDelegateRunMenuAtOriginal(self, widget, point, sourceType);
}

static void AppMenuCtorHook(void* self, void* browser, void* model, int runFlags,
                            void* callback) {
  // AppMenu::Init() populates the menu right after construction and is inlined
  // in release builds, so claim the scope from the constructor instead.
  SetPendingMenuScope(kMenuScopeApp);
  g_AppMenuCtorOriginal(self, browser, model, runFlags, callback);
}

static void AppMenuRunMenuButtonHook(void* self, void* button) {
  SetPendingMenuScope(kMenuScopeApp);
  g_AppMenuRunMenuButtonOriginal(self, button);
}

static void AppMenuRunMenuWidgetHook(void* self, void* widget, const void* bounds) {
  SetPendingMenuScope(kMenuScopeApp);
  g_AppMenuRunMenuWidgetOriginal(self, widget, bounds);
}

static void ShowContextMenuForTabHook(void* self, void* tab, const void* point,
                                      int sourceType) {
  SetPendingMenuScope(kMenuScopeTab);
  g_ShowContextMenuForTabOriginal(self, tab, point, sourceType);
}

static void TabStripContextMenuHook(void* self, void* view, const void* point,
                                    int sourceType) {
  SetPendingMenuScope(kMenuScopeTab);
  g_TabStripContextMenuOriginal(self, view, point, sourceType);
}

static void TabViewContextMenuHook(void* self, void* view, const void* point,
                                   int sourceType) {
  SetPendingMenuScope(kMenuScopeTab);
  g_TabViewContextMenuOriginal(self, view, point, sourceType);
}

static void TabGroupHeaderContextMenuHook(void* self, void* view, const void* point,
                                          int sourceType) {
  SetPendingMenuScope(kMenuScopeTab);
  g_TabGroupHeaderContextMenuOriginal(self, view, point, sourceType);
}

static void TabGroupHeaderViewContextMenuHook(void* self, void* view,
                                              const void* point, int sourceType) {
  SetPendingMenuScope(kMenuScopeTab);
  g_TabGroupHeaderViewContextMenuOriginal(self, view, point, sourceType);
}

static void TabStripComboButtonContextMenuHook(void* self, void* view,
                                               const void* point, int sourceType) {
  SetPendingMenuScope(kMenuScopeTab);
  g_TabStripComboButtonContextMenuOriginal(self, view, point, sourceType);
}

static void BookmarkBarContextMenuHook(void* self, void* view, const void* point,
                                       int sourceType) {
  SetPendingMenuScope(kMenuScopeBookmark);
  g_BookmarkBarContextMenuOriginal(self, view, point, sourceType);
}

static void BookmarkContextMenuCtorHook(void* self, void* widget, void* browser,
                                        void* profile, int launchLocation,
                                        const void* selection, bool closeOnRemove,
                                        bool isSubmenu) {
  SetPendingMenuScope(kMenuScopeBookmark);
  g_BookmarkContextMenuCtorOriginal(self, widget, browser, profile, launchLocation,
                                    selection, closeOnRemove, isSubmenu);
}

static void BookmarkContextMenuRunMenuAtHook(void* self, const void* point,
                                             int sourceType) {
  SetPendingMenuScope(kMenuScopeBookmark);
  g_BookmarkContextMenuRunMenuAtOriginal(self, point, sourceType);
}

static void BookmarkMenuControllerCtorHook(void* self, void* browser, void* widget,
                                           const void* folder, size_t startIndex,
                                           bool forDrop) {
  SetPendingMenuScope(kMenuScopeBookmark);
  g_BookmarkMenuControllerCtorOriginal(self, browser, widget, folder, startIndex,
                                       forDrop);
}

static void BookmarkMenuControllerRunMenuAtHook(void* self, void* bookmarkBar) {
  SetPendingMenuScope(kMenuScopeBookmark);
  g_BookmarkMenuControllerRunMenuAtOriginal(self, bookmarkBar);
}

// -----------------------------------------------------------------------------
// Context Menus
// -----------------------------------------------------------------------------

static FontListOpaque* MenuItemGetFontListHook(const void* self, FontListOpaque* result) {
  int targetSize = MenuStylingEnabled()
                       ? g_menuFontSize.load(std::memory_order_relaxed) : -1;

  FontListOpaque* originalResult = g_MenuItemGetFontListOriginal(self, result);

  if (targetSize < 0 || !originalResult ||
      !g_FontListCopyCtor || !g_FontListGetFontSize ||
      !g_FontListDeriveWithSizeDelta || !g_FontListDtor) {
    return originalResult;
  }

  int originalSize = g_FontListGetFontSize(originalResult);

  if (targetSize == originalSize) return originalResult;

  auto derivedFontStorage =
      DeriveFontListToOwnedStorage(originalResult, targetSize, L"Menu");

  if (!derivedFontStorage) return originalResult;

  const FontListOpaque* derivedFont = GetOwnedFontList(derivedFontStorage);

  // The original sret object is already constructed in |result|. Replace it
  // with a copy of the derived font only after destroying that original object.
  g_FontListDtor(originalResult);
  g_FontListCopyCtor(result, derivedFont);
  DestroyOwnedFontListStorage(std::move(derivedFontStorage), L"Menu");

  return result;
}

static bool ContextMenuIsCommandIdVisibleHook(const void* self, int commandId) {
  if ((commandId == kExtensionsGroupCommandId || commandId == kCustomItemsGroupCommandId)) return true;
  if (IsCustomMenuCommandId(commandId)) return true;

  for (const auto& item : g_contextMenuVisibility) {
    if (item.commandId == commandId && item.hidden.load(std::memory_order_relaxed)) {
      return false;
    }
  }

  return g_ContextMenuIsCommandIdVisibleOriginal(self, commandId);
}

static bool CustomMenuPositioningReady() {
  return g_SimpleMenuModelInsertItemAt && g_SimpleMenuModelGetItemCount &&
         g_SimpleMenuModelGetTypeAt && g_SimpleMenuModelIsVisibleAt;
}

static bool ExtensionGroupingReady() {
  return g_customMenuItemsReady.load(std::memory_order_acquire) &&
         CustomMenuPositioningReady() && g_SimpleMenuModelCtor &&
         g_SimpleMenuModelDtor && g_SimpleMenuModelInsertSubMenuAt &&
         g_PageContextMenuCtorOriginal && g_ExtensionMatcherCtorOriginal &&
         g_ExtensionMatcherDtorOriginal;
}

static void* PageContextMenuCtorHook(void* self, void* frame, const void* params,
                                     bool paste, bool pasteMatchStyle) {
  const bool previous = g_constructingPageContextMenu;
  g_constructingPageContextMenu = true;
  void* result = g_PageContextMenuCtorOriginal(self, frame, params, paste, pasteMatchStyle);
  g_constructingPageContextMenu = previous;
  return result;
}

// Redirect only a page menu's extension builder. Chrome still constructs and
// owns every extension command, icon, nested submenu, and command delegate.
// No Chrome object fields or C++ containers are read or rewritten here.
static void* ExtensionMatcherCtorHook(void* self, void* browserContext,
                                      void* delegate, void* rootModel, void* filter) {
  if (g_constructingPageContextMenu && g_groupExtensions.load() &&
      ExtensionGroupingReady()) {
    const void* label = GetCustomMenuItemLabel(L"Extensions");
    if (label) {
      auto storage = std::make_unique<OpaqueObjectStorage>();
      PrepareOpaqueObjectStorage(*storage);
      g_SimpleMenuModelCtor(storage->data, delegate);
      if (IsOpaqueObjectGuardIntact(*storage)) {
        void* model = storage->data;
        bool registered = false;
        {
          std::lock_guard<std::mutex> lock(g_extensionGroupsMutex);
          // Unload can disable grouping while the native constructor runs.
          if (g_groupExtensions.load()) {
            g_extensionGroups.emplace(self, ExtensionGroup{rootModel, std::move(storage), label});
            registered = true;
          }
        }
        if (registered) {
          return g_ExtensionMatcherCtorOriginal(self, browserContext, delegate, model, filter);
        }
        g_SimpleMenuModelDtor(model);
        return g_ExtensionMatcherCtorOriginal(self, browserContext, delegate, rootModel, filter);
      }
      Wh_Log(L"Unexpected SimpleMenuModel size; disabling extension grouping");
      g_SimpleMenuModelDtor(storage->data);
      g_groupExtensions.store(false);
    }
  }
  return g_ExtensionMatcherCtorOriginal(self, browserContext, delegate, rootModel, filter);
}

static void ExtensionMatcherDtorHook(void* self) {
  std::unique_ptr<OpaqueObjectStorage> storage;
  {
    std::lock_guard<std::mutex> lock(g_extensionGroupsMutex);
    auto it = g_extensionGroups.find(self);
    if (it != g_extensionGroups.end()) {
      storage = std::move(it->second.model);
      g_extensionGroups.erase(it);
    }
  }
  // Release references to nested submenus before the matcher destroys them.
  if (storage) {
    {
      std::lock_guard<std::mutex> lock(g_pageMenuIconMutex);
      g_pageLabelModels.erase(storage->data);
      g_hiddenLabelIndices.erase(storage->data);
    }
    g_SimpleMenuModelDtor(storage->data);
  }
  g_ExtensionMatcherDtorOriginal(self);
}

static void AppendExtensionGroupPlacement(void* rootModel,
                                         std::vector<CustomMenuItem>& items) {
  void* model = nullptr;
  const void* label = nullptr;
  {
    std::lock_guard<std::mutex> lock(g_extensionGroupsMutex);
    for (auto& [matcher, group] : g_extensionGroups) {
      if (group.rootModel == rootModel && !group.inserted) {
        model = group.model->data;
        label = group.label;
        group.inserted = true;
        break;
      }
    }
  }
  if (!model) return;
  // Do not hold the map lock while asking Chrome about visibility: those calls
  // can reenter its extension command delegate.
  const size_t count = g_SimpleMenuModelGetItemCount(model);
  bool visible = false;
  for (size_t i = 0; i < count; ++i) {
    if (g_SimpleMenuModelGetTypeAt(model, i) != kMenuModelTypeSeparator &&
        g_SimpleMenuModelIsVisibleAt(model, i)) {
      visible = true;
      break;
    }
  }
  if (!visible) return;
  CustomMenuItem item{};
  item.commandId = kExtensionsGroupCommandId;
  item.order = g_extensionsGroupOrder.load();
  item.label = L"Extensions";
  item.submenu = model;
  item.nativeLabel = label;
  items.push_back(std::move(item));
}

static void SortMenuPlacements(std::vector<CustomMenuItem>& items) {
  // Place top-relative items, append entries, then work upward from the bottom.
  // Stable ties preserve settings order; Extensions follows custom-item ties.
  std::stable_sort(items.begin(), items.end(),
                   [](const CustomMenuItem& left, const CustomMenuItem& right) {
                     const auto category = [](int order) {
                       return order > 0 ? 0 : order == 0 ? 1 : 2;
                     };
                     const int leftCategory = category(left.order);
                     const int rightCategory = category(right.order);
                     if (leftCategory != rightCategory) return leftCategory < rightCategory;
                     return left.order < 0 ? left.order > right.order : left.order < right.order;
                   });
}

// Count visible top-level entries, including previously inserted custom items.
// Hidden commands and separators don't consume a numbered position.
static size_t FindCustomMenuInsertionIndex(void* model, int order) {
  const size_t count = g_SimpleMenuModelGetItemCount(model);
  if (order < 0) {
    // -1 is the last position AFTER insertion; -2 leaves one visible entry
    // below the new item. Widen before negation so INT_MIN is safe too.
    size_t entriesAfter = static_cast<size_t>(-static_cast<int64_t>(order) - 1);
    if (entriesAfter == 0) return count;
    for (size_t index = count; index > 0; --index) {
      if (g_SimpleMenuModelGetTypeAt(model, index - 1) != kMenuModelTypeSeparator &&
          g_SimpleMenuModelIsVisibleAt(model, index - 1) && --entriesAfter == 0) {
        return index - 1;
      }
    }
    return 0;  // A position beyond the beginning clamps to the top.
  }
  size_t position = 1;
  for (size_t index = 0; index < count; ++index) {
    if (g_SimpleMenuModelGetTypeAt(model, index) == kMenuModelTypeSeparator ||
        !g_SimpleMenuModelIsVisibleAt(model, index)) {
      continue;
    }
    if (position++ == static_cast<size_t>(order)) return index;
  }
  return count;
}

static bool PageLabelFilteringReady() {
  return g_SimpleMenuModelGetLabelAtOriginal && g_SimpleMenuModelIsVisibleAt &&
         g_SimpleMenuModelGetSubmenuModelAtOriginal && g_SimpleMenuModelDtor &&
         g_ToolkitDelegateInitOriginal;
}

// Observe the string Chrome has already created for its caller. The caller
// retains ownership and destroys it normally; no cross-allocator frees or
// persistent native string copies are needed. Same libc++ layout as the
// validated custom-item labels above.
static bool ReadChromeMenuLabel(const void* object, std::wstring& label) {
  static_assert(sizeof(wchar_t) == sizeof(char16_t));
  const auto* bytes = static_cast<const unsigned char*>(object);
  const bool isLong = (bytes[23] & 0x80) != 0;
  size_t length = bytes[23];
  const wchar_t* data = reinterpret_cast<const wchar_t*>(bytes);
  if (isLong) {
    memcpy(&data, bytes, sizeof(data));
    memcpy(&length, bytes + sizeof(data), sizeof(length));
  }
  if (length > 65536 || !IsValidChromeU16String(bytes, length)) return false;
  label.assign(data, length);
  return true;
}

static void* SimpleMenuModelGetLabelAtHook(const void* self, void* result, size_t index) {
  void* originalResult = g_SimpleMenuModelGetLabelAtOriginal(self, result, index);
  {
    std::lock_guard<std::mutex> lock(g_pageMenuIconMutex);
    if (!g_pageLabelModels.contains(self)) return originalResult;
  }
  std::vector<std::wstring> patterns;
  {
    std::lock_guard<std::mutex> lock(g_hiddenLabelPatternsMutex);
    patterns = g_hiddenLabelPatterns;
  }
  std::wstring label;
  const bool hidden = !patterns.empty() && originalResult &&
      ReadChromeMenuLabel(originalResult, label) && ShouldHideCustomItemLabel(label, patterns);
  {
    std::lock_guard<std::mutex> lock(g_pageMenuIconMutex);
    if (hidden) {
      g_hiddenLabelIndices[self].insert(index);
    } else if (auto it = g_hiddenLabelIndices.find(self); it != g_hiddenLabelIndices.end()) {
      it->second.erase(index);
    }
  }
  return originalResult;
}

static bool SimpleMenuModelIsVisibleAtHook(const void* self, size_t index) {
  {
    std::lock_guard<std::mutex> lock(g_pageMenuIconMutex);
    auto it = g_hiddenLabelIndices.find(self);
    if (it != g_hiddenLabelIndices.end() && it->second.contains(index)) return false;
  }
  return g_SimpleMenuModelIsVisibleAt(self, index);
}

static void* SimpleMenuModelGetSubmenuModelAtHook(const void* self, size_t index) {
  void* submenu = g_SimpleMenuModelGetSubmenuModelAtOriginal(self, index);
  if (submenu) {
    std::lock_guard<std::mutex> lock(g_pageMenuIconMutex);
    if (g_pageLabelModels.contains(self)) g_pageLabelModels.insert(submenu);
  }
  return submenu;
}

static bool MainPageMenuIconTrackingReady() {
  return g_AddMenuItemFromModelAtOriginal && g_MenuItemViewDtorOriginal &&
         g_SimpleMenuModelDtor && g_ToolkitDelegateInitOriginal;
}

static void* SimpleMenuModelCtorHook(void* self, void* delegate) {
  void* result = g_SimpleMenuModelCtor(self, delegate);
  std::lock_guard<std::mutex> lock(g_customGroupsMutex);
  if (g_customGroupTrackingEnabled && delegate) g_menuModelDelegates[self] = delegate;
  return result;
}

static void ForgetPageMenuModel(void* model) {
  std::lock_guard<std::mutex> lock(g_pageMenuIconMutex);
  g_pageMenuRootModels.erase(model);
  g_pageLabelModels.erase(model);
  g_hiddenLabelIndices.erase(model);
}

static void SimpleMenuModelDtorHook(void* self) {
  std::unique_ptr<OpaqueObjectStorage> customGroup;
  {
    std::lock_guard<std::mutex> lock(g_customGroupsMutex);
    g_menuModelDelegates.erase(self);
    auto it = g_customGroups.find(self);
    if (it != g_customGroups.end()) {
      customGroup = std::move(it->second);
      g_customGroups.erase(it);
    }
  }
  ForgetPageMenuModel(self);
  g_SimpleMenuModelDtor(self);
  if (customGroup) {
    ForgetPageMenuModel(customGroup->data);
    g_SimpleMenuModelDtor(customGroup->data);
  }
}

static void MenuItemViewDtorHook(void* self) {
  {
    std::lock_guard<std::mutex> lock(g_pageMenuIconMutex);
    g_mainPageMenuItemViews.erase(self);
  }
  g_MenuItemViewDtorOriginal(self);
}

static void* AddMenuItemFromModelAtHook(void* model, size_t modelIndex,
                                       void* menu, size_t menuIndex, int commandId) {
  bool topLevel = false;
  {
    std::lock_guard<std::mutex> lock(g_pageMenuIconMutex);
    topLevel = g_pageMenuRootModels.contains(model);
  }
  // SetIcon runs inside the builder, before it returns the new view. A nested
  // builder for a submenu resets this flag and restores its caller's state.
  const bool previous = g_buildingMainPageMenuItem;
  g_buildingMainPageMenuItem = topLevel;
  void* item = g_AddMenuItemFromModelAtOriginal(model, modelIndex, menu, menuIndex, commandId);
  g_buildingMainPageMenuItem = previous;
  if (topLevel && item) {
    std::lock_guard<std::mutex> lock(g_pageMenuIconMutex);
    g_mainPageMenuItemViews.insert(item);
  }
  return item;
}

static bool ShouldHideMainPageMenuIcon(void* item) {
  if (!g_hideMainPageMenuIcons.load(std::memory_order_relaxed)) return false;
  if (g_buildingMainPageMenuItem) return true;
  std::lock_guard<std::mutex> lock(g_pageMenuIconMutex);
  return g_mainPageMenuItemViews.contains(item);
}

static void PopulateCustomMenuItems(void* menuModel, std::vector<CustomMenuItem>& items) {
  SortMenuPlacements(items);

  size_t nextInsertionIndex = 0;
  int previousOrder = 0;
  for (const CustomMenuItem& item : items) {
    const void* label = item.nativeLabel ? item.nativeLabel : GetCustomMenuItemLabel(item.label);

    if (!label) continue;

    if ((item.order != 0 || item.submenu) && CustomMenuPositioningReady()) {
      size_t index = g_SimpleMenuModelGetItemCount(menuModel);
      if (item.order != 0) {
        index = FindCustomMenuInsertionIndex(menuModel, item.order);
        // Bottom-relative positions must be free to move above the previous
        // item. Only ties share a forward cursor, including clamped ties.
        if (item.order > 0 || item.order == previousOrder) {
          index = std::max(nextInsertionIndex, index);
        }
      }
      if (item.submenu) {
        g_SimpleMenuModelInsertSubMenuAt(menuModel, index, item.commandId, label, item.submenu);
      } else {
        g_SimpleMenuModelInsertItemAt(menuModel, index, item.commandId, label);
      }
      if (item.separatorBefore && g_SimpleMenuModelInsertSeparatorAt &&
          index > 0 &&
          g_SimpleMenuModelGetTypeAt(menuModel, index - 1) != kMenuModelTypeSeparator) {
        const size_t countBefore = g_SimpleMenuModelGetItemCount(menuModel);
        g_SimpleMenuModelInsertSeparatorAt(menuModel, index, kNormalMenuSeparator);
        index += g_SimpleMenuModelGetItemCount(menuModel) - countBefore;
      }
      nextInsertionIndex = index + 1;
      previousOrder = item.order;
      continue;
    }

    if (item.separatorBefore && g_SimpleMenuModelAddSeparator) {
      g_SimpleMenuModelAddSeparator(menuModel, kNormalMenuSeparator);
    }

    g_SimpleMenuModelAddItem(menuModel, item.commandId, label);
  }
}

static void GroupCustomMenuItems(void* rootModel, std::vector<CustomMenuItem>& items,
                                 const CustomItemsGroupSettings& settings) {
  if (!settings.enabled || items.empty()) return;
  if (!g_SimpleMenuModelCtor || !g_SimpleMenuModelDtor ||
      !g_SimpleMenuModelInsertSubMenuAt || !CustomMenuPositioningReady()) {
    Wh_Log(L"Custom grouping unavailable; keeping separate entries");
    return;
  }
  {
    std::lock_guard<std::mutex> lock(g_hiddenLabelPatternsMutex);
    if (ShouldHideCustomItemLabel(settings.label, g_hiddenLabelPatterns)) {
      items.clear();
      return;
    }
  }
  void* delegate = nullptr;
  {
    std::lock_guard<std::mutex> lock(g_customGroupsMutex);
    if (!g_customGroupTrackingEnabled || g_customGroups.contains(rootModel)) {
      items.clear();
      return;
    }
    auto it = g_menuModelDelegates.find(rootModel);
    if (it != g_menuModelDelegates.end()) delegate = it->second;
  }
  if (!delegate) {
    Wh_Log(L"Custom grouping unavailable for this menu; keeping separate entries");
    return;
  }
  const void* label = GetCustomMenuItemLabel(settings.label);
  if (!label) return;
  auto storage = std::make_unique<OpaqueObjectStorage>();
  PrepareOpaqueObjectStorage(*storage);
  // Call the original constructor: this child reuses the root's Chrome delegate
  // and does not need a separate delegate-tracking entry.
  g_SimpleMenuModelCtor(storage->data, delegate);
  if (!IsOpaqueObjectGuardIntact(*storage)) {
    Wh_Log(L"Unexpected menu model size; keeping separate custom entries");
    g_SimpleMenuModelDtor(storage->data);
    return;
  }
  PopulateCustomMenuItems(storage->data, items);
  if (g_SimpleMenuModelGetItemCount(storage->data) == 0) {
    g_SimpleMenuModelDtor(storage->data);
    items.clear();
    return;
  }
  void* submenu = storage->data;
  bool retained = false;
  {
    std::lock_guard<std::mutex> lock(g_customGroupsMutex);
    if (g_customGroupTrackingEnabled && !g_customGroups.contains(rootModel)) {
      g_customGroups.emplace(rootModel, std::move(storage));
      retained = true;
    }
  }
  if (!retained) {
    g_SimpleMenuModelDtor(submenu);
    items.clear();
    return;
  }
  {
    std::lock_guard<std::mutex> lock(g_pageMenuIconMutex);
    if (g_pageLabelModels.contains(rootModel)) g_pageLabelModels.insert(submenu);
  }
  CustomMenuItem group{};
  group.commandId = kCustomItemsGroupCommandId;
  group.order = settings.order;
  group.label = settings.label;
  group.nativeLabel = label;
  group.submenu = submenu;
  items.clear();
  items.push_back(std::move(group));
}

// The complete model can be edited before Chrome turns it into menu views.
static void ToolkitDelegateInitHook(void* self, void* menuModel) {
  SetPendingMenuScope(kMenuScopePage);
  if (menuModel && PageLabelFilteringReady()) {
    std::lock_guard<std::mutex> lock(g_pageMenuIconMutex);
    g_pageLabelModels.insert(menuModel);
  }
  if (menuModel && MainPageMenuIconTrackingReady()) {
    std::lock_guard<std::mutex> lock(g_pageMenuIconMutex);
    g_pageMenuRootModels.insert(menuModel);
  }
  if (menuModel) {
    std::vector<CustomMenuItem> items;
    CustomItemsGroupSettings grouping;
    {
      std::lock_guard<std::mutex> lock(g_customItemsMutex);
      if (g_customMenuItemsReady.load(std::memory_order_acquire)) items = g_customItems;
      grouping = g_customItemsGroupSettings;
    }
    GroupCustomMenuItems(menuModel, items, grouping);
    AppendExtensionGroupPlacement(menuModel, items);
    PopulateCustomMenuItems(menuModel, items);

  }

  g_ToolkitDelegateInitOriginal(self, menuModel);
}

static void ContextMenuExecuteCommandHook(void* self, int commandId, int eventFlags) {
  if ((commandId == kExtensionsGroupCommandId || commandId == kCustomItemsGroupCommandId)) return;
  // Custom IDs never reach Chrome's own dispatch, which would treat them as
  // unknown commands.
  if (IsCustomMenuCommandId(commandId)) {
    RunCustomMenuItem(commandId);
    return;
  }

  g_ContextMenuExecuteCommandOriginal(self, commandId, eventFlags);
}

static bool ContextMenuIsCommandIdEnabledHook(const void* self, int commandId) {
  if ((commandId == kExtensionsGroupCommandId || commandId == kCustomItemsGroupCommandId)) return true;
  if (IsCustomMenuCommandId(commandId)) return true;

  return g_ContextMenuIsCommandIdEnabledOriginal(self, commandId);
}

static bool ContextMenuIsCommandIdCheckedHook(const void* self, int commandId) {
  if ((commandId == kExtensionsGroupCommandId || commandId == kCustomItemsGroupCommandId)) return false;
  if (IsCustomMenuCommandId(commandId)) return false;

  return g_ContextMenuIsCommandIdCheckedOriginal(self, commandId);
}

static void MenuItemSetIconHook(void* self, const ImageModelOpaque* icon) {
  const bool hideIcons = ShouldHideMainPageMenuIcon(self) ||
      (g_hideMenuIcons.load(std::memory_order_relaxed) && MenuStylingEnabled());
  if (!hideIcons ||
      !g_EmptyImageModelCtor || !g_EmptyImageModelDtor) {
    g_MenuItemSetIconOriginal(self, icon);
    return;
  }

  // Let Chrome clear the icon and update layout using a real empty ImageModel.
  // Avoid assuming its private representation or suppressing layout updates.
  OpaqueObjectStorage empty{};
  PrepareOpaqueObjectStorage(empty);
  g_EmptyImageModelCtor(empty.data);
  if (!IsOpaqueObjectGuardIntact(empty)) {
    Wh_Log(L"Empty ImageModel storage guard overwritten; disabling icon hiding");
    g_hideMenuIcons.store(false, std::memory_order_relaxed);
    g_hideMainPageMenuIcons.store(false, std::memory_order_relaxed);
    g_MenuItemSetIconOriginal(self, icon);
    return;
  }
  g_MenuItemSetIconOriginal(self, reinterpret_cast<const ImageModelOpaque*>(empty.data));
  g_EmptyImageModelDtor(empty.data);
}

static bool MenuShouldShowAcceleratorTextHook(const void* self, const void* item,
                                             void* text) {
  // This controls presentation only. Leave the accelerator lookup and dispatch
  // intact, and let GetMinorText retain any non-shortcut descriptive text.
  if (g_hideMenuShortcutLabels.load(std::memory_order_relaxed) &&
      MenuStylingEnabled()) {
    return false;
  }
  return g_MenuShouldShowAcceleratorTextOriginal(self, item, text);
}

static int MenuItemGetVerticalMarginHook(const void* self) {
  int originalMargin = g_MenuItemGetVerticalMarginOriginal(self);

  int configuredMargin = MenuStylingEnabled()
                             ? g_menuVerticalSpacing.load(std::memory_order_relaxed)
                             : -1;

  if (configuredMargin < 0) return originalMargin;

  return std::min(originalMargin, configuredMargin);
}

static uint32_t ColorProviderGetColorHook(const void* self, int colorId) {
  if (g_menuItemPaintColor && self == g_menuItemPaintColorProvider) {
    const uint32_t color = g_menuItemPaintColor;
    // NativeTheme::PaintMenuItemBackground makes one GetColor call. Consume
    // the override so later incidental queries cannot inherit this color.
    g_menuItemPaintColor = 0;
    return color;
  }
  const uint32_t color = g_ColorProviderGetColorOriginal(self, colorId);
  // The outer layer supplies the requested alpha exactly once. Preserve fully
  // transparent colors (no fill), but remove existing translucency from the
  // native background's colors so it cannot multiply the selected opacity.
  if (g_normalizeMenuBackgroundAlpha && (color >> 24) != 0) {
    return color | 0xFF000000u;
  }
  return color;
}

static void PaintMenuItemBackgroundHook(
    const void* self, void* canvas, const void* colorProvider, int state,
    const void* rect, const MenuItemExtraParamsOpaque* params) {
  if (!MenuStylingEnabled()) {
    g_PaintMenuItemBackgroundOriginal(self, canvas, colorProvider, state, rect, params);
    return;
  }
  int radius = g_menuCornerRadius.load(std::memory_order_relaxed);
  auto adjusted = *params;
  if (radius >= 0) adjusted.cornerRadius = radius;
  const auto previousProvider = g_menuItemPaintColorProvider;
  const uint32_t previousColor = g_menuItemPaintColor;
  g_menuItemPaintColorProvider = colorProvider;
  g_menuItemPaintColor = g_ColorProviderGetColorOriginal
      ? g_menuItemBackgroundColor.load(std::memory_order_relaxed) : 0;
  // This canvas is cc::PaintCanvas, not gfx::Canvas. Keep Chrome's original
  // painting path and override only its scoped background-color lookup.
  g_PaintMenuItemBackgroundOriginal(self, canvas, colorProvider, state, rect, &adjusted);
  g_menuItemPaintColor = previousColor;
  g_menuItemPaintColorProvider = previousProvider;
}

static bool MenuGroupBorderReady() {
  return g_MenuContainerPaintBackgroundOriginal && g_ViewOnPaintBorderOriginal && g_ViewGetLocalBounds &&
         (g_MenuContainerGetCornerRadius || (g_MenuConfigInstance && g_MenuConfigCornerRadius)) &&
         g_MenuContainerHasBubbleBorder &&
         g_ViewGetInsets && g_RoundRectPainterCtor && g_RoundRectPainterPaint &&
         (g_RoundRectPainterDtor || g_RoundRectPainterDeletingDtor) &&
         g_CanvasSave && g_CanvasRestore && g_CanvasTranslate;
}

static void PaintMenuGroupBorder(const void* self, void* canvas) {
  const uint32_t color = g_menuGroupBorderColor.load(std::memory_order_relaxed);
  if (!color || !MenuGroupBorderReady()) return;
  GfxRectOpaque bounds{};
  g_ViewGetLocalBounds(self, &bounds);
  GfxInsetsOpaque frame{};
  // BubbleBackground fills local bounds inset by the physical border insets.
  // Do not use the mod's menu insets: those include user-configured padding.
  if (g_MenuContainerHasBubbleBorder(self)) g_ViewGetInsets(self, &frame);
  // Expand into the shadow margin by one DIP. The painter strokes inward
  // from these bounds, so the outline is outside the filled client area.
  // Plain menus without a shadow margin must stay within their window bounds.
  const bool outside = frame.left >= 1 && frame.top >= 1 &&
                       frame.right >= 1 && frame.bottom >= 1;
  if (outside) {
    --frame.left;
    --frame.top;
    --frame.right;
    --frame.bottom;
  }
  const GfxSizeOpaque size{bounds.width - frame.left - frame.right,
                           bounds.height - frame.top - frame.bottom};
  if (size.width <= 1 || size.height <= 1) return;
  const GfxVector2dOpaque offset{bounds.x + frame.left, bounds.y + frame.top};
  // Optimized Chrome builds inline the container getter. The public config
  // method remains available and supplies the standard rounded menu radius.
  const int nativeRadius = g_MenuContainerGetCornerRadius
      ? g_MenuContainerGetCornerRadius(self)
      : g_MenuConfigCornerRadius(g_MenuConfigInstance(), nullptr);
  const int radius = std::clamp(nativeRadius + (outside ? 1 : 0), 0,
                               std::min(size.width, size.height) / 2);
  OpaqueObjectStorage painter{};
  PrepareOpaqueObjectStorage(painter);
  g_RoundRectPainterCtor(painter.data, color, radius);
  if (!IsOpaqueObjectGuardIntact(painter)) {
    Wh_Log(L"RoundRectPainter storage guard overwritten; disabling menu border");
    g_menuGroupBorderColor.store(0, std::memory_order_relaxed);
    return;
  }
  g_CanvasSave(canvas);
  g_CanvasTranslate(canvas, &offset);
  g_RoundRectPainterPaint(painter.data, canvas, &size);
  g_CanvasRestore(canvas);
  if (g_RoundRectPainterDtor) {
    g_RoundRectPainterDtor(painter.data);
  } else {
    // MSVC scalar deleting destructor, flags=0: destroy without freeing our
    // stack-owned storage. Chrome may omit the standalone destructor function.
    g_RoundRectPainterDeletingDtor(painter.data, 0);
  }
}

static void ViewOnPaintBorderHook(void* self, void* canvas) {
  const bool menu = g_pendingMenuBorderView == self;
  if (menu) g_pendingMenuBorderView = nullptr;
  g_ViewOnPaintBorderOriginal(self, canvas);
  // View::OnPaint calls background then border. Draw after the native border
  // and shadow, avoiding both shadow overpainting and child-row overlap.
  if (menu) PaintMenuGroupBorder(self, canvas);
}

static void MenuContainerPaintBackgroundHook(void* self, void* canvas) {
  if (!MenuStylingEnabled()) {
    // Leave g_pendingMenuBorderView unset so no custom border is drawn either.
    g_MenuContainerPaintBackgroundOriginal(self, canvas);
    return;
  }
  const int opacity = g_menuBackgroundOpacity.load(std::memory_order_relaxed);
  const uint32_t color = g_menuBackgroundColor.load(std::memory_order_relaxed);
  const bool recolor = color != 0 && g_CanvasDrawColor;
  if ((opacity == 100 && !recolor) || !g_CanvasSaveLayerAlpha || !g_CanvasRestore ||
      (opacity < 100 && !g_CanvasDrawColor)) {
    g_MenuContainerPaintBackgroundOriginal(self, canvas);
    g_pendingMenuBorderView = self;
    return;
  }

  // Replace the previous background instead of source-over blending another
  // translucent fill onto it. Otherwise a repaint can increase alpha (e.g.
  // 70% over 70% becomes 91%). Clear the current menu paint clip on the parent
  // canvas BEFORE the temporary layer; clearing inside it cannot remove the
  // destination pixels. Chrome paints this view's border and children later.
  if (opacity < 100) g_CanvasDrawColor(canvas, 0, kSkBlendModeClear);

  // Only the container background is drawn into this temporary layer.
  // Restore before Chrome paints children, preserving solid labels/icons and
  // hover highlights. Rounded MenuHost windows already support per-pixel alpha.
  g_CanvasSaveLayerAlpha(canvas, static_cast<uint8_t>((opacity * 255 + 50) / 100));
  const bool previousNormalizeAlpha = g_normalizeMenuBackgroundAlpha;
  g_normalizeMenuBackgroundAlpha = opacity < 100;
  g_MenuContainerPaintBackgroundOriginal(self, canvas);
  g_normalizeMenuBackgroundAlpha = previousNormalizeAlpha;
  // Recolor only this isolated background layer. SrcIn retains Chrome's
  // rounded, antialiased silhouette and doesn't fill transparent corners.
  if (recolor) g_CanvasDrawColor(canvas, color, kSkBlendModeSrcIn);
  g_CanvasRestore(canvas);
  g_pendingMenuBorderView = self;
}

static GfxInsetsOpaque* MenuContainerGetInsetsHook(const void* self, GfxInsetsOpaque* result) {
  auto* insets = g_MenuContainerGetInsetsOriginal(self, result);
  if (!MenuStylingEnabled()) return insets;
  int padding = g_menuGroupPadding.load(std::memory_order_relaxed);
  int top = g_menuGroupTopSpacing.load(std::memory_order_relaxed);
  int bottom = g_menuGroupBottomSpacing.load(std::memory_order_relaxed);
  // Bubble menus include asymmetric shadow/arrow insets in View::GetInsets().
  // Only additional_insets_ is inner spacing. Preserve the physical border
  // allocation instead of moving rows into the shadow by zeroing total insets.
  // For non-bubble menus, the native insets are the inner spacing itself.
  GfxInsetsOpaque frame = {};
  bool canSetVertical = g_ViewGetInsets && g_MenuContainerHasBubbleBorder;
  if (canSetVertical && g_MenuContainerHasBubbleBorder(self)) {
    g_ViewGetInsets(self, &frame);
  }
  if (top < 0) {
    insets->top += std::max(0, padding);
  } else if (canSetVertical) {
    insets->top = frame.top + top;
  }
  if (bottom < 0) {
    insets->bottom += std::max(0, padding);
  } else if (canSetVertical) {
    insets->bottom = frame.bottom + bottom;
  }
  if (padding > 0) {
    insets->left += padding;
    insets->right += padding;
  }
  return insets;
}

// -----------------------------------------------------------------------------
// Tabs
// -----------------------------------------------------------------------------

static void TabTitleCtorHook(void* self) {
  const FontListOpaque* previousPendingFont = g_pendingTabTitleOriginalFont;
  g_pendingTabTitleOriginalFont = nullptr;
  g_tabTitleCtorDepth++;

  g_TabTitleCtorOriginal(self);

  g_tabTitleCtorDepth--;

  const FontListOpaque* originalFont = g_pendingTabTitleOriginalFont;
  g_pendingTabTitleOriginalFont = previousPendingFont;

  if (!g_tabFontHooksReady.load(std::memory_order_relaxed)) {
    return;
  }

  auto originalFontStorage =
      CopyFontListToOwnedStorage(originalFont, L"Tab title original FontList");

  if (!originalFontStorage) {
    Wh_Log(L"Tab title original font capture failed; leaving this title unchanged");
    return;
  }

  const FontListOpaque* ownedOriginalFont = GetOwnedFontList(originalFontStorage);
  int targetSize = g_tabFontSize.load(std::memory_order_relaxed);

  if (targetSize >= 0 &&
      !SetLabelFontForTargetSize(self, ownedOriginalFont, targetSize, L"Tab title")) {
    Wh_Log(L"Tab title exact font sizing unavailable; leaving Chrome's original font");
  }

  {
    std::lock_guard<std::mutex> lock(g_tabObjectsMutex);
    g_tabTitles.insert_or_assign(
        self, TabTitleInfo{GetCurrentThreadId(), std::move(originalFontStorage)});
  }
}

static void TabTitleDtorHook(void* self) {
  std::unique_ptr<OpaqueObjectStorage> originalFontStorage;

  {
    std::lock_guard<std::mutex> lock(g_tabObjectsMutex);
    auto it = g_tabTitles.find(self);

    if (it != g_tabTitles.end()) {
      originalFontStorage = std::move(it->second.originalFontStorage);
      g_tabTitles.erase(it);
    }
  }

  DestroyOwnedFontListStorage(std::move(originalFontStorage),
                              L"Tab title original FontList");

  g_TabTitleDtorOriginal(self);
}

static void TabCloseButtonCtorHook(void* self, void* pressedCallback, void* mouseEventCallback) {
  g_TabCloseButtonCtorOriginal(self, pressedCallback, mouseEventCallback);

  if (!g_tabCloseHooksReady.load(std::memory_order_relaxed)) {
    return;
  }

  std::lock_guard<std::mutex> lock(g_tabObjectsMutex);

  g_tabCloseButtons[self] = GetCurrentThreadId();
}

static void TabCloseButtonDtorHook(void* self) {
  {
    std::lock_guard<std::mutex> lock(g_tabObjectsMutex);
    g_tabCloseButtons.erase(self);
  }

  g_TabCloseButtonDtorOriginal(self);
}

static bool ValidateTabLayoutConstants() {
  int state = g_tabLayoutCompatibility.load(std::memory_order_acquire);

  if (state != 0) return state > 0;

  if (!g_GetLayoutConstantOriginal) return false;

  int afterTitle = g_GetLayoutConstantOriginal(kLayoutTabAfterTitlePadding);

  int captureIcon = g_GetLayoutConstantOriginal(kLayoutTabAlertIndicatorCaptureIconWidth);

  int alertIcon = g_GetLayoutConstantOriginal(kLayoutTabAlertIndicatorIconWidth);

  int closeButton = g_GetLayoutConstantOriginal(kLayoutTabCloseButtonSize);

  int tabHeight = g_GetLayoutConstantOriginal(kLayoutTabHeight);

  int tabStripHeight = g_GetLayoutConstantOriginal(kLayoutTabStripHeight);

  int tabStripPadding = g_GetLayoutConstantOriginal(kLayoutTabStripPadding);

  int separatorHeight = g_GetLayoutConstantOriginal(kLayoutTabSeparatorHeight);

  int preTitle = g_GetLayoutConstantOriginal(kLayoutTabPreTitlePadding);

  bool valid = (afterTitle == 4 || afterTitle == 8) && captureIcon == 16 && (alertIcon == 12 || alertIcon == 16) &&
               (closeButton == 14 || closeButton == 16 || closeButton == 24) && tabHeight == 35 &&
               tabStripHeight == 41 && tabStripPadding == 6 && (separatorHeight == 20 || separatorHeight == 24) &&
               preTitle == 8;

  int newState = valid ? 1 : -1;

  int expected = 0;

  if (g_tabLayoutCompatibility.compare_exchange_strong(expected, newState, std::memory_order_release,
                                                       std::memory_order_relaxed)) {
    if (valid) {
      Wh_Log(
          L"Tab layout constants validated: "
          L"afterTitle=%d capture=%d alert=%d close=%d "
          L"height=%d stripHeight=%d stripPadding=%d separator=%d preTitle=%d",
          afterTitle, captureIcon, alertIcon, closeButton, tabHeight, tabStripHeight, tabStripPadding, separatorHeight,
          preTitle);
    } else {
      Wh_Log(
          L"WARNING: Tab layout constants don't match expected Chromium "
          L"layout; tab layout tweaks disabled. "
          L"Values: afterTitle=%d capture=%d alert=%d close=%d "
          L"height=%d stripHeight=%d stripPadding=%d separator=%d preTitle=%d",
          afterTitle, captureIcon, alertIcon, closeButton, tabHeight, tabStripHeight, tabStripPadding, separatorHeight,
          preTitle);
    }
  }

  return g_tabLayoutCompatibility.load(std::memory_order_acquire) > 0;
}

static int GetLayoutConstantHook(int constant) {
  int originalValue = g_GetLayoutConstantOriginal(constant);

  if (constant == kLayoutTabPreTitlePadding) {
    int configuredPadding = g_tabPreTitlePadding.load(std::memory_order_relaxed);

    if (configuredPadding != kChromeDefaultTabPreTitlePadding && ValidateTabLayoutConstants()) {
      return configuredPadding;
    }

    return originalValue;
  }

  if (!g_tabCloseHooksReady.load(std::memory_order_relaxed) ||
      !g_tabCloseButtonsHidden.load(std::memory_order_relaxed)) {
    return originalValue;
  }

  if (constant != kLayoutTabAfterTitlePadding && constant != kLayoutTabCloseButtonSize) {
    return originalValue;
  }

  if (!ValidateTabLayoutConstants()) return originalValue;

  return 0;
}

static void ViewSetVisibleHook(void* self, bool visible) {
  if (visible && g_tabCloseHooksReady.load(std::memory_order_relaxed) &&
      g_tabCloseButtonsHidden.load(std::memory_order_relaxed) && ValidateTabLayoutConstants()) {
    bool isTabCloseButton = false;

    {
      std::lock_guard<std::mutex> lock(g_tabObjectsMutex);

      isTabCloseButton = g_tabCloseButtons.find(self) != g_tabCloseButtons.end();
    }

    if (isTabCloseButton) visible = false;
  }

  g_ViewSetVisibleOriginal(self, visible);
}

// -----------------------------------------------------------------------------
// Extension toolbar
// -----------------------------------------------------------------------------

static void ToolbarActionViewCtorHook(void* self, void* viewModel, void* delegate) {
  g_ToolbarActionViewCtorOriginal(self, viewModel, delegate);

  if (!g_extensionTrackingReady.load(std::memory_order_relaxed)) return;

  std::lock_guard<std::mutex> lock(g_extensionViewsMutex);

  g_extensionViews[self] = GetCurrentThreadId();
}

static GfxSizeOpaque* ToolbarActionViewCalculatePreferredSizeHook(const void* self, GfxSizeOpaque* result,
                                                                  const void* availableSize) {
  GfxSizeOpaque* returned = g_ToolbarActionViewCalculatePreferredSizeOriginal(self, result, availableSize);

  if (!result) return returned;

  bool normalDesktopButton =
      result->width == kChromeDefaultExtensionButtonWidth && result->height == kChromeDefaultExtensionButtonWidth;

  if (normalDesktopButton) {
    int configuredWidth = g_extensionButtonWidth.load(std::memory_order_relaxed);

    if (configuredWidth < kChromeDefaultExtensionButtonWidth) {
      result->width = configuredWidth;
    }
  }

  return returned;
}

static void* ToolbarActionViewDeletingDtorHook(void* self, unsigned int flags) {
  {
    std::lock_guard<std::mutex> lock(g_extensionViewsMutex);

    g_extensionViews.erase(self);
  }

  return g_ToolbarActionViewDeletingDtorOriginal(self, flags);
}

// Track ExtensionsToolbarDesktop itself so that a settings update can
// propagate the new preferred width to ToolbarView.

static void ExtensionsToolbarDesktopCtorHook(void* self, void* browser, int displayMode) {
  g_ExtensionsToolbarDesktopCtorOriginal(self, browser, displayMode);

  if (!g_extensionContainerTrackingReady.load(std::memory_order_relaxed)) {
    return;
  }

  std::lock_guard<std::mutex> lock(g_extensionViewsMutex);

  g_extensionContainers[self] = GetCurrentThreadId();
}

static void* ExtensionsToolbarDesktopDeletingDtorHook(void* self, unsigned int flags) {
  {
    std::lock_guard<std::mutex> lock(g_extensionViewsMutex);

    g_extensionContainers.erase(self);
  }

  return g_ExtensionsToolbarDesktopDeletingDtorOriginal(self, flags);
}

// -----------------------------------------------------------------------------
// Run code on Chrome UI thread
// -----------------------------------------------------------------------------

using RunFromWindowThreadProc = void(WINAPI*)(void* parameter);

static UINT GetRunFromWindowThreadMessage() {
  static const UINT message = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

  return message;
}

static bool RunFromWindowThread(HWND hwnd, RunFromWindowThreadProc proc, void* parameter) {
  DWORD threadId = GetWindowThreadProcessId(hwnd, nullptr);

  if (!threadId) return false;

  if (threadId == GetCurrentThreadId()) {
    proc(parameter);
    return true;
  }

  struct Param {
    RunFromWindowThreadProc proc;
    void* parameter;
  };

  HHOOK hook = SetWindowsHookExW(
      WH_CALLWNDPROC,
      [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
        if (code == HC_ACTION) {
          const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);

          if (cwp->message == GetRunFromWindowThreadMessage()) {
            Param* param = reinterpret_cast<Param*>(cwp->lParam);

            param->proc(param->parameter);
          }
        }

        return CallNextHookEx(nullptr, code, wParam, lParam);
      },
      nullptr, threadId);

  if (!hook) return false;

  Param param{proc, parameter};

  SendMessageW(hwnd, GetRunFromWindowThreadMessage(), 0, reinterpret_cast<LPARAM>(&param));

  UnhookWindowsHookEx(hook);

  return true;
}

// -----------------------------------------------------------------------------
// Find Chrome window for UI thread
// -----------------------------------------------------------------------------

static HWND FindWindowForThread(DWORD threadId) {
  struct Context {
    HWND first = nullptr;
    HWND chrome = nullptr;
  } context;

  EnumThreadWindows(
      threadId,
      [](HWND hwnd, LPARAM lParam) -> BOOL {
        Context* context = reinterpret_cast<Context*>(lParam);

        if (!context->first) context->first = hwnd;

        wchar_t className[128] = {};

        if (GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
          if (wcsncmp(className, kChromeWidgetWindowClassPrefix,
                      ARRAYSIZE(kChromeWidgetWindowClassPrefix) - 1) == 0) {
            context->chrome = hwnd;
            return FALSE;
          }
        }

        return TRUE;
      },
      reinterpret_cast<LPARAM>(&context));

  return context.chrome ? context.chrome : context.first;
}

// -----------------------------------------------------------------------------
// Live tab update
// -----------------------------------------------------------------------------

struct TabApplyParams {
  bool teardown;
};

static void WINAPI ApplyTabTweaksOnCurrentThread(void* parameter) {
  const auto* params = static_cast<const TabApplyParams*>(parameter);
  bool teardown = params && params->teardown;
  DWORD threadId = GetCurrentThreadId();

  if (!teardown) {
    std::vector<std::pair<void*, const FontListOpaque*>> titles;
    std::vector<void*> closeButtons;

    {
      std::lock_guard<std::mutex> lock(g_tabObjectsMutex);

      for (const auto& [title, info] : g_tabTitles) {
        if (info.threadId == threadId) {
          titles.push_back({title, GetOwnedFontList(info.originalFontStorage)});
        }
      }

      for (const auto& [closeButton, closeButtonThreadId] : g_tabCloseButtons) {
        if (closeButtonThreadId == threadId) {
          closeButtons.push_back(closeButton);
        }
      }
    }

    int targetSize = g_tabFontSize.load(std::memory_order_relaxed);

    for (const auto& [title, originalFont] : titles) {
      if (g_tabFontHooksReady.load(std::memory_order_relaxed)) {
        SetLabelFontForTargetSize(title, originalFont, targetSize, L"Tab title");
      }

      if (g_ViewInvalidateLayout) {
        g_ViewInvalidateLayout(title, false);
      }
    }

    // A close-button-only function set still needs a live relayout path even if
    // TabTitle tracking isn't available. InvalidateLayout propagates to parents.
    if (g_ViewInvalidateLayout) {
      for (void* closeButton : closeButtons) {
        g_ViewInvalidateLayout(closeButton, false);
      }
    }

    return;
  }

  struct TeardownEntry {
    void* title;
    std::unique_ptr<OpaqueObjectStorage> originalFontStorage;
  };

  std::vector<TeardownEntry> titles;
  std::vector<void*> closeButtons;

  {
    std::lock_guard<std::mutex> lock(g_tabObjectsMutex);

    for (auto it = g_tabTitles.begin(); it != g_tabTitles.end();) {
      if (it->second.threadId == threadId) {
        titles.push_back({it->first, std::move(it->second.originalFontStorage)});
        it = g_tabTitles.erase(it);
      } else {
        ++it;
      }
    }

    for (auto it = g_tabCloseButtons.begin(); it != g_tabCloseButtons.end();) {
      if (it->second == threadId) {
        closeButtons.push_back(it->first);
        it = g_tabCloseButtons.erase(it);
      } else {
        ++it;
      }
    }
  }

  for (auto& entry : titles) {
    const FontListOpaque* originalFont = GetOwnedFontList(entry.originalFontStorage);

    // Teardown already knows these objects were tracked while the required
    // functions were available. Don't depend on runtime ready flags that a
    // resolver/abandon path can clear independently.
    if (originalFont && g_LabelSetFontList) {
      g_LabelSetFontList(entry.title, originalFont);
    }

    if (g_ViewInvalidateLayout) {
      g_ViewInvalidateLayout(entry.title, false);
    }

    DestroyOwnedFontListStorage(std::move(entry.originalFontStorage),
                                L"Tab title original FontList");
  }

  if (g_ViewInvalidateLayout) {
    for (void* closeButton : closeButtons) {
      g_ViewInvalidateLayout(closeButton, false);
    }
  }

  Wh_Log(L"Restored/released %llu tab title fonts and invalidated %llu close buttons on UI thread %lu",
         static_cast<unsigned long long>(titles.size()),
         static_cast<unsigned long long>(closeButtons.size()), threadId);
}

static void ApplyTweaksToExistingTabs(bool teardown = false) {
  std::vector<DWORD> threadIds;

  {
    std::lock_guard<std::mutex> lock(g_tabObjectsMutex);

    for (const auto& [title, info] : g_tabTitles) {
      threadIds.push_back(info.threadId);
    }

    for (const auto& [closeButton, threadId] : g_tabCloseButtons) {
      threadIds.push_back(threadId);
    }
  }

  std::sort(threadIds.begin(), threadIds.end());

  threadIds.erase(std::unique(threadIds.begin(), threadIds.end()), threadIds.end());

  TabApplyParams params{teardown};

  for (DWORD threadId : threadIds) {
    HWND hwnd = FindWindowForThread(threadId);

    if (!hwnd) {
      Wh_Log(L"Tab %ls: no window found for UI thread %lu",
             teardown ? L"teardown" : L"live update", threadId);
      continue;
    }

    if (!RunFromWindowThread(hwnd, ApplyTabTweaksOnCurrentThread, &params)) {
      Wh_Log(L"Tab %ls: failed to dispatch to UI thread %lu",
             teardown ? L"teardown" : L"live update", threadId);
    }
  }
}

// -----------------------------------------------------------------------------
// Live extension update
// -----------------------------------------------------------------------------

static void WINAPI ApplyExtensionWidthOnCurrentThread(void*) {
  DWORD threadId = GetCurrentThreadId();

  std::vector<void*> views;
  std::vector<void*> containers;

  {
    std::lock_guard<std::mutex> lock(g_extensionViewsMutex);

    for (const auto& [view, viewThreadId] : g_extensionViews) {
      if (viewThreadId == threadId) {
        views.push_back(view);
      }
    }

    for (const auto& [container, containerThreadId] : g_extensionContainers) {
      if (containerThreadId == threadId) {
        containers.push_back(container);
      }
    }
  }

  // Rebuild icon/badge image if this function is available,
  // then tell each action view that its preferred size changed.
  for (void* view : views) {
    if (g_ToolbarActionViewUpdateState) {
      g_ToolbarActionViewUpdateState(view);
    }

    if (g_ViewPreferredSizeChanged) {
      g_ViewPreferredSizeChanged(view);
    }
  }

  // Propagate the container's new preferred width to ToolbarView.
  for (void* container : containers) {
    if (g_ViewPreferredSizeChanged) {
      g_ViewPreferredSizeChanged(container);
    }
  }

  Wh_Log(L"Updated %llu extension buttons and %llu containers on UI thread %lu",
         static_cast<unsigned long long>(views.size()), static_cast<unsigned long long>(containers.size()), threadId);
}

static void ApplyWidthToExistingExtensionButtons() {
  if (!g_extensionTrackingReady.load(std::memory_order_relaxed) &&
      !g_extensionContainerTrackingReady.load(std::memory_order_relaxed)) {
    return;
  }

  std::vector<DWORD> threadIds;

  {
    std::lock_guard<std::mutex> lock(g_extensionViewsMutex);

    for (const auto& [view, threadId] : g_extensionViews) {
      threadIds.push_back(threadId);
    }

    for (const auto& [container, threadId] : g_extensionContainers) {
      threadIds.push_back(threadId);
    }
  }

  std::sort(threadIds.begin(), threadIds.end());

  threadIds.erase(std::unique(threadIds.begin(), threadIds.end()), threadIds.end());

  for (DWORD threadId : threadIds) {
    HWND hwnd = FindWindowForThread(threadId);

    if (hwnd) {
      RunFromWindowThread(hwnd, ApplyExtensionWidthOnCurrentThread, nullptr);
    }
  }
}

// -----------------------------------------------------------------------------
// Exact-build address maps. Resolve functions only when no verified map exists.
// -----------------------------------------------------------------------------

static uint64_t AddressMapHash(uint64_t hash, uint64_t value) {
  for (int i = 0; i < 8; i++, value >>= 8) {
    hash = (hash ^ (value & 0xff)) * 1099511628211ULL;
  }
  return hash;
}

static constexpr uint64_t kAddressMapHashSeed = 14695981039346656037ULL;

// Keep real target addresses separate from Windhawk's trampoline pointers.
// The fallback resolves all targets without installing hooks; both paths then
// validate the complete result and use the same hook installation code.
struct ChromeHook {
  void* address = nullptr;
  void** original;
  void* replacement;
  bool optional;
  uint64_t schema = kAddressMapHashSeed;
  WindhawkUtils::SYMBOL_HOOK chromeDllHook;

  template <typename Prototype>
  ChromeHook(std::initializer_list<std::wstring_view> names,
             Prototype** originalFunction,
             std::type_identity_t<Prototype*> hookFunction,
             bool isOptional)
      : original(reinterpret_cast<void**>(originalFunction)),
        replacement(reinterpret_cast<void*>(hookFunction)),
        optional(isOptional),
        chromeDllHook(names, &address, nullptr, isOptional) {
    for (auto name : names) {
      schema = AddressMapHash(schema, name.size());
      for (wchar_t c : name) schema = AddressMapHash(schema, c);
    }
    schema = AddressMapHash(schema, optional);
    schema = AddressMapHash(schema, replacement != nullptr);
  }
  ChromeHook(const ChromeHook&) = delete;
  ChromeHook& operator=(const ChromeHook&) = delete;
};

struct ChromeImageIdentity {
  const BYTE* base;
  const IMAGE_NT_HEADERS64* nt;
  std::wstring key;

  bool Contains(DWORD rva, size_t size) const {
    return rva < nt->OptionalHeader.SizeOfImage &&
           size <= nt->OptionalHeader.SizeOfImage - rva;
  }

  bool IsCode(DWORD rva) const {
    if (!Contains(rva, 1)) return false;
    const auto* sections = IMAGE_FIRST_SECTION(nt);
    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; i++) {
      const auto& s = sections[i];
      if ((s.Characteristics & IMAGE_SCN_MEM_EXECUTE) &&
          rva >= s.VirtualAddress && rva - s.VirtualAddress < s.Misc.VirtualSize) {
        return true;
      }
    }
    return false;
  }
};

static bool GetChromeImageIdentity(HMODULE module, ChromeImageIdentity& image) {
  image.base = reinterpret_cast<const BYTE*>(module);
  const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(image.base);
  if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0) return false;
  image.nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(image.base + dos->e_lfanew);
  if (image.nt->Signature != IMAGE_NT_SIGNATURE ||
      image.nt->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 ||
      image.nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC ||
      image.nt->OptionalHeader.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_DEBUG) return false;

  const auto& debug = image.nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
  if (!debug.VirtualAddress || !image.Contains(debug.VirtualAddress, debug.Size)) return false;
  for (size_t offset = 0; offset + sizeof(IMAGE_DEBUG_DIRECTORY) <= debug.Size;
       offset += sizeof(IMAGE_DEBUG_DIRECTORY)) {
    IMAGE_DEBUG_DIRECTORY entry;
    memcpy(&entry, image.base + debug.VirtualAddress + offset, sizeof(entry));
    if (entry.Type != IMAGE_DEBUG_TYPE_CODEVIEW || entry.SizeOfData < 24 ||
        !image.Contains(entry.AddressOfRawData, entry.SizeOfData)) continue;
    const BYTE* cv = image.base + entry.AddressOfRawData;
    if (memcmp(cv, "RSDS", 4) != 0) continue;

    // Raw build GUID, age, PE timestamp and image size. Never identify a
    // build by Chrome's marketing version or timestamp alone.
    wchar_t guid[33];
    for (int i = 0; i < 16; i++) swprintf_s(guid + i * 2, 33 - i * 2, L"%02X", cv[4 + i]);
    DWORD age;
    memcpy(&age, cv + 20, sizeof(age));
    wchar_t suffix[64];
    swprintf_s(suffix, L"-%08X-%08X-%08X", age,
               image.nt->FileHeader.TimeDateStamp, image.nt->OptionalHeader.SizeOfImage);
    image.key = std::wstring(guid) + suffix;
    return true;
  }
  return false;
}

// Precomputed addresses for supported Chrome builds. The fingerprint must
// match the current hook list before any entry can be used.
static constexpr PCWSTR kBundledChromeAddressMaps[] = {
    // Chrome 153.0.8010.53 x64; RSDS 919c3f00-37ec-14a0-4c4c-44205044422e age 1.
    L"CCMI1 003F9C91EC37A0144C4C44205044422E-00000001-6AAB63C8-1225E000 26e6d54cbfc37a29 55 c81540 c5f0e0 a5e4c90 b592470 9866150 0 0 d4bad60 d4afbf0 929bfd0 c4c2e30 928f0a0 9b4ffc0 9a35fd0 0 9a3d960 0 9a43850 9eb6c00 9eb5a80 9eb69c0 b592520 9eb6c50 2ee2f90 c409c0 a9382b0 a938570 9a34c0 33d5ef0 c40c80 2ee3190 a938fd0 9ead650 8f732e0 8f733e0 2ee2f00 35efb0 c52080 24770e0 a938670 1b9ad80 c52ce0 5866430 45aafd0 a5f0a30 a5e2cf0 a5e3c40 addba10 49af320 b7daee0 33d2220 2ed72b0 33d23d0 3cf20d0 b7dac60 5862260 563eb0 0 a5f0c10 a5f09c0 c673cd0 c673d00 0 5cd8020 2ed71b0 33d2300 b7dad00 33d2c90 33d2ce0 2ed7e70 405ae0 279cd20 2e57b00 279de00 2e573f0 51d6500 400c0e0 245210 4125170 987ef20 987fab0 9880230 987f420 0 2f4a5b0 71f5c8e825d7587d",
};
static constexpr PCWSTR kChromeAddressMapStorage = L"chrome-address-map-v1";

static uint64_t ChromeHookSchema(const ChromeHook* hooks, size_t count) {
  uint64_t schema = AddressMapHash(kAddressMapHashSeed, count);
  for (size_t i = 0; i < count; i++) schema = AddressMapHash(schema, hooks[i].schema);
  return schema;
}

static uint64_t AddressMapChecksum(std::wstring_view text) {
  uint64_t hash = kAddressMapHashSeed;
  for (wchar_t c : text) hash = AddressMapHash(hash, c);
  return hash;
}

static bool ReadChromeAddressMap(std::wstring_view record,
                                 const ChromeImageIdentity& image,
                                 ChromeHook* hooks, size_t count) {
  const size_t checksumOffset = record.rfind(L' ');
  if (checksumOffset == std::wstring_view::npos) return false;
  std::wistringstream input{std::wstring(record)};
  std::wstring format, identity;
  uint64_t schema = 0, mapCount = 0;
  if (!(input >> format >> identity >> std::hex >> schema >> mapCount) ||
      format != L"CCMI1" || identity != image.key ||
      schema != ChromeHookSchema(hooks, count) || mapCount != count) return false;

  std::vector<DWORD> offsets(count);
  for (size_t i = 0; i < count; i++) {
    uint64_t rva;
    if (!(input >> rva) || rva > MAXDWORD ||
        (rva ? !image.IsCode(static_cast<DWORD>(rva)) : !hooks[i].optional)) return false;
    offsets[i] = static_cast<DWORD>(rva);
  }
  uint64_t checksum;
  if (!(input >> checksum) ||
      checksum != AddressMapChecksum(record.substr(0, checksumOffset))) return false;
  input >> std::ws;
  if (!input.eof()) return false;

  // Commit only after every field and offset has passed validation.
  for (size_t i = 0; i < count; i++) {
    hooks[i].address = offsets[i] ? const_cast<BYTE*>(image.base) + offsets[i] : nullptr;
  }
  return true;
}

static std::wstring MakeChromeAddressMap(const ChromeImageIdentity& image,
                                       const ChromeHook* hooks, size_t count) {
  std::wostringstream output;
  output << L"CCMI1 " << image.key << L' ' << std::hex << ChromeHookSchema(hooks, count)
         << L' ' << count;
  for (size_t i = 0; i < count; i++) {
    const uintptr_t address = reinterpret_cast<uintptr_t>(hooks[i].address);
    const uintptr_t base = reinterpret_cast<uintptr_t>(image.base);
    if (address && (address < base || address - base > MAXDWORD ||
                    !image.IsCode(static_cast<DWORD>(address - base)))) return {};
    if (!address && !hooks[i].optional) return {};
    output << L' ' << (address ? address - base : 0);
  }
  const std::wstring body = output.str();
  output << L' ' << AddressMapChecksum(body);
  return output.str();
}

static bool ResolveChromeAddresses(HMODULE module, ChromeHook* hooks, size_t count) {
  ChromeImageIdentity image{};
  const bool hasIdentity = GetChromeImageIdentity(module, image);
  bool found = false;
  if (hasIdentity) {
    for (auto record : kBundledChromeAddressMaps) {
      if (ReadChromeAddressMap(record, image, hooks, count)) {
        Wh_Log(L"Using built-in Chrome configuration (%ls)", image.key.c_str());
        found = true;
        break;
      }
    }
    if (!found) {
      // Bounded read; truncated, corrupt, old-build and old-schema maps miss.
      wchar_t record[16384];
      const size_t length = Wh_GetStringValue(kChromeAddressMapStorage, record, ARRAYSIZE(record));
      if (length && length < ARRAYSIZE(record) &&
          ReadChromeAddressMap(std::wstring_view(record, length), image, hooks, count)) {
        Wh_Log(L"Using saved Chrome configuration (%ls)", image.key.c_str());
        found = true;
      }
    }
  }
  if (!found) {
    Wh_Log(L"Preparing this Chrome build");
    WH_HOOK_SYMBOLS_OPTIONS options = {};
    options.optionsSize = sizeof(options);
    options.symbolServer = kChromeSymbolServer;
    // chrome.dll.pdb is several GB and served by Chromium's symbol server, not
    // Windhawk's online cache; skipping undecoration keeps this one-time pass
    // fast. Lookups list the decorated public name first and, where needed, the
    // private function name, which the PDB already stores undecorated.
    options.noUndecoratedSymbols = TRUE;
    std::vector<WindhawkUtils::SYMBOL_HOOK> lookups;
    lookups.reserve(count);
    for (size_t i = 0; i < count; i++) lookups.push_back(std::move(hooks[i].chromeDllHook));
    if (!WindhawkUtils::HookSymbols(module, lookups.data(), lookups.size(), &options)) return false;
    if (hasIdentity) {
      const std::wstring record = MakeChromeAddressMap(image, hooks, count);
      if (record.empty()) {
        Wh_Log(L"ERROR: Resolved Chrome addresses failed image validation");
        return false;
      }
      if (!Wh_SetStringValue(kChromeAddressMapStorage, record.c_str())) {
        Wh_Log(L"Could not save Chrome preparation data; setup still succeeded");
      }
    }
  }

  // A slow fallback prepares the next launch only, preserving the existing
  // startup policy. Do not queue hooks after the startup window was abandoned.
  if (g_hookActivationAbandoned.load(std::memory_order_acquire)) return true;
  std::vector<void*> installed;
  for (size_t i = 0; i < count; i++) {
    auto& hook = hooks[i];
    if (!hook.address) continue;  // A verified unavailable optional function.
    if (hook.replacement) {
      if (!Wh_SetFunctionHook(hook.address, hook.replacement, hook.original)) {
        Wh_Log(L"ERROR: Failed to register Chrome hook %zu; cancelling registered hooks", i);
        for (void* address : installed) Wh_RemoveFunctionHook(address);
        for (size_t j = 0; j < count; j++) *hooks[j].original = nullptr;
        return false;  // Never retry resolution after a partially queued install.
      }
      installed.push_back(hook.address);
    } else {
      *hook.original = hook.address;
    }
  }
  return true;
}

static bool InstallChromeHooks(HMODULE chromeDll) {
  wchar_t path[32768] = {};

  if (GetModuleFileNameW(chromeDll, path, ARRAYSIZE(path))) {
    Wh_Log(L"Preparing Chrome hooks for: %ls", path);
  }

  ChromeHook chromeDllHooks[] = {

      {{LR"(?GetFont@TypographyProvider@views@@QEBAAEBVFontList@gfx@@HH@Z)"},
       &g_TypographyGetFontOriginal,
       TypographyGetFontHook,
       false},

      {{LR"(?SetFontList@Label@views@@UEAAXAEBVFontList@gfx@@@Z)"}, &g_LabelSetFontList, nullptr, false},

      // -----------------------------------------------------------------------
      // Context Menus: OPTIONAL
      // -----------------------------------------------------------------------

      // Menu scope: OPTIONAL. Without these, every menu counts as "other".

      {{LR"(?Run@MenuController@views@@QEAAXPEAVWidget@2@PEAVMenuButtonController@2@PEAVMenuItemView@2@AEBVRect@gfx@@W4MenuAnchorPosition@2@W4MenuSourceType@mojom@ui@@W4MenuType@12@_NPEAVWindow@aura@@@Z)",
        L"views::MenuController::Run"},
       &g_MenuControllerRunOriginal, MenuControllerRunHook, true},

      {{LR"(?RunMenuAt@ToolkitDelegateViews@@QEAAXPEAVWidget@views@@AEBVPoint@gfx@@W4MenuSourceType@mojom@ui@@@Z)"},
       &g_ToolkitDelegateRunMenuAtOriginal, ToolkitDelegateRunMenuAtHook, true},

      {{LR"(??0AppMenu@@QEAA@PEAVBrowser@@PEAVMenuModel@ui@@HV?$RepeatingCallback@$$A6AXXZ@base@@@Z)"},
       &g_AppMenuCtorOriginal, AppMenuCtorHook, true},

      {{LR"(?RunMenu@AppMenu@@QEAAXPEAVMenuButtonController@views@@@Z)"},
       &g_AppMenuRunMenuButtonOriginal, AppMenuRunMenuButtonHook, true},

      {{LR"(?RunMenu@AppMenu@@QEAAXPEAVWidget@views@@AEBVRect@gfx@@@Z)"},
       &g_AppMenuRunMenuWidgetOriginal, AppMenuRunMenuWidgetHook, true},

      {{LR"(?ShowContextMenuForTab@BrowserTabStripController@@UEAAXPEAVTab@@AEBVPoint@gfx@@W4MenuSourceType@mojom@ui@@@Z)"},
       &g_ShowContextMenuForTabOriginal, ShowContextMenuForTabHook, true},

      {{LR"(?ShowContextMenuForViewImpl@TabContextMenuController@TabStrip@@UEAAXPEAVView@views@@AEBVPoint@gfx@@W4MenuSourceType@mojom@ui@@@Z)"},
       &g_TabStripContextMenuOriginal, TabStripContextMenuHook, true},

      {{LR"(?ShowContextMenuForViewImpl@TabView@@EEAAXPEAVView@views@@AEBVPoint@gfx@@W4MenuSourceType@mojom@ui@@@Z)"},
       &g_TabViewContextMenuOriginal, TabViewContextMenuHook, true},

      {{LR"(?ShowContextMenuForViewImpl@TabGroupHeader@@UEAAXPEAVView@views@@AEBVPoint@gfx@@W4MenuSourceType@mojom@ui@@@Z)"},
       &g_TabGroupHeaderContextMenuOriginal, TabGroupHeaderContextMenuHook, true},

      {{LR"(?ShowContextMenuForViewImpl@TabGroupHeaderView@@UEAAXPEAVView@views@@AEBVPoint@gfx@@W4MenuSourceType@mojom@ui@@@Z)"},
       &g_TabGroupHeaderViewContextMenuOriginal, TabGroupHeaderViewContextMenuHook, true},

      {{LR"(?ShowContextMenuForViewImpl@TabStripComboButton@@UEAAXPEAVView@views@@AEBVPoint@gfx@@W4MenuSourceType@mojom@ui@@@Z)"},
       &g_TabStripComboButtonContextMenuOriginal, TabStripComboButtonContextMenuHook, true},

      {{LR"(?ShowContextMenuForViewImpl@BookmarkBarView@@UEAAXPEAVView@views@@AEBVPoint@gfx@@W4MenuSourceType@mojom@ui@@@Z)"},
       &g_BookmarkBarContextMenuOriginal, BookmarkBarContextMenuHook, true},

      {{LR"(??0BookmarkContextMenu@@QEAA@PEAVWidget@views@@PEAVBrowser@@PEAVProfile@@W4BookmarkLaunchLocation@@AEBV?$vector@V?$raw_ptr@$$CBVBookmarkNode@bookmarks@@$00@base@@V?$allocator@V?$raw_ptr@$$CBVBookmarkNode@bookmarks@@$00@base@@@__Cr@std@@@__Cr@std@@_N5@Z)"},
       &g_BookmarkContextMenuCtorOriginal, BookmarkContextMenuCtorHook, true},

      {{LR"(?RunMenuAt@BookmarkContextMenu@@QEAAXAEBVPoint@gfx@@W4MenuSourceType@mojom@ui@@@Z)"},
       &g_BookmarkContextMenuRunMenuAtOriginal, BookmarkContextMenuRunMenuAtHook, true},

      {{LR"(??0BookmarkMenuController@@QEAA@PEAVBrowser@@PEAVWidget@views@@AEBUBookmarkParentFolder@@_K_N@Z)"},
       &g_BookmarkMenuControllerCtorOriginal, BookmarkMenuControllerCtorHook, true},

      {{LR"(?RunMenuAt@BookmarkMenuController@@QEAAXPEAVBookmarkBarView@@@Z)"},
       &g_BookmarkMenuControllerRunMenuAtOriginal, BookmarkMenuControllerRunMenuAtHook, true},

      {{LR"(?IsCommandIdVisible@RenderViewContextMenu@@UEBA_NH@Z)",
        L"RenderViewContextMenu::IsCommandIdVisible"},
       &g_ContextMenuIsCommandIdVisibleOriginal,
       ContextMenuIsCommandIdVisibleHook,
       true},

      // Custom context menu items: OPTIONAL. Several spellings are listed for
      // the functions whose signature or access specifier has moved between
      // Chrome releases; the first match wins.
      {{LR"(?IsCommandIdEnabled@RenderViewContextMenu@@UEBA_NH@Z)"},
       &g_ContextMenuIsCommandIdEnabledOriginal,
       ContextMenuIsCommandIdEnabledHook,
       true},

      {{LR"(?IsCommandIdChecked@RenderViewContextMenu@@UEBA_NH@Z)"},
       &g_ContextMenuIsCommandIdCheckedOriginal,
       ContextMenuIsCommandIdCheckedHook,
       true},

      {{LR"(?Init@ToolkitDelegateViews@@MEAAXPEAVSimpleMenuModel@ui@@@Z)",
        LR"(?Init@ToolkitDelegateViews@@UEAAXPEAVSimpleMenuModel@ui@@@Z)"},
       &g_ToolkitDelegateInitOriginal,
       ToolkitDelegateInitHook,
       true},

      {{LR"(?ExecuteCommand@RenderViewContextMenu@@UEAAXHH@Z)"},
       &g_ContextMenuExecuteCommandOriginal,
       ContextMenuExecuteCommandHook,
       true},

      {{LR"(?AddItem@SimpleMenuModel@ui@@QEAAXHAEBV?$basic_string@_SU?$char_traits@_S@__Cr@std@@V?$allocator@_S@23@@__Cr@std@@@Z)"},
       &g_SimpleMenuModelAddItem, nullptr, true},

      {{LR"(?AddSeparator@SimpleMenuModel@ui@@QEAAXW4MenuSeparatorType@2@@Z)"},
       &g_SimpleMenuModelAddSeparator, nullptr, true},

      {{LR"(?InsertItemAt@SimpleMenuModel@ui@@QEAAX_KHAEBV?$basic_string@_SU?$char_traits@_S@__Cr@std@@V?$allocator@_S@23@@__Cr@std@@@Z)"},
       &g_SimpleMenuModelInsertItemAt, nullptr, true},

      {{LR"(?InsertSeparatorAt@SimpleMenuModel@ui@@QEAAX_KW4MenuSeparatorType@2@@Z)"},
       &g_SimpleMenuModelInsertSeparatorAt, nullptr, true},

      {{LR"(?GetItemCount@SimpleMenuModel@ui@@UEBA_KXZ)"},
       &g_SimpleMenuModelGetItemCount, nullptr, true},

      {{LR"(?GetTypeAt@SimpleMenuModel@ui@@UEBA?AW4ItemType@MenuModel@2@_K@Z)"},
       &g_SimpleMenuModelGetTypeAt, nullptr, true},

      {{LR"(?IsVisibleAt@SimpleMenuModel@ui@@UEBA_N_K@Z)"},
       &g_SimpleMenuModelIsVisibleAt, SimpleMenuModelIsVisibleAtHook, true},

      {{LR"(?GetLabelAt@SimpleMenuModel@ui@@UEBA?AV?$basic_string@_SU?$char_traits@_S@__Cr@std@@V?$allocator@_S@23@@__Cr@std@@_K@Z)"},
       &g_SimpleMenuModelGetLabelAtOriginal, SimpleMenuModelGetLabelAtHook, true},

      {{LR"(?GetSubmenuModelAt@SimpleMenuModel@ui@@UEBAPEAVMenuModel@2@_K@Z)"},
       &g_SimpleMenuModelGetSubmenuModelAtOriginal, SimpleMenuModelGetSubmenuModelAtHook, true},

      // Native extension grouping. Keep optional so unsupported Chrome builds
      // retain their normal extension menus instead of losing commands.
      {{LR"(??0RenderViewContextMenu@@QEAA@AEAVRenderFrameHost@content@@AEBUContextMenuParams@2@_N2@Z)"},
       &g_PageContextMenuCtorOriginal, PageContextMenuCtorHook, true},

      {{LR"(??0ContextMenuMatcher@extensions@@QEAA@PEAVBrowserContext@content@@PEAVDelegate@SimpleMenuModel@ui@@PEAV56@V?$RepeatingCallback@$$A6A_NPEBVMenuItem@extensions@@@Z@base@@@Z)"},
       &g_ExtensionMatcherCtorOriginal, ExtensionMatcherCtorHook, true},

      {{LR"(??1ContextMenuMatcher@extensions@@QEAA@XZ)"},
       &g_ExtensionMatcherDtorOriginal, ExtensionMatcherDtorHook, true},

      {{LR"(??0SimpleMenuModel@ui@@QEAA@PEAVDelegate@01@@Z)"},
       &g_SimpleMenuModelCtor, SimpleMenuModelCtorHook, true},

      {{LR"(??1SimpleMenuModel@ui@@UEAA@XZ)"},
       &g_SimpleMenuModelDtor, SimpleMenuModelDtorHook, true},

      {{LR"(?AddMenuItemFromModelAt@MenuModelAdapter@views@@SAPEAVMenuItemView@2@PEAVMenuModel@ui@@_KPEAV32@1H@Z)"},
       &g_AddMenuItemFromModelAtOriginal, AddMenuItemFromModelAtHook, true},

      {{LR"(??1MenuItemView@views@@UEAA@XZ)"},
       &g_MenuItemViewDtorOriginal, MenuItemViewDtorHook, true},

      {{LR"(?InsertSubMenuAt@SimpleMenuModel@ui@@QEAAX_KHAEBV?$basic_string@_SU?$char_traits@_S@__Cr@std@@V?$allocator@_S@23@@__Cr@std@@PEAVMenuModel@2@@Z)"},
       &g_SimpleMenuModelInsertSubMenuAt, nullptr, true},

      // std::u16string base::WideToUTF16(std::wstring_view). The trailing
      // scope of the parameter is a back-reference whose index depends on how
      // the return type was mangled, so list the plausible encodings.
      {{LR"(?WideToUTF16@base@@YA?AV?$basic_string@_SU?$char_traits@_S@__Cr@std@@V?$allocator@_S@23@@__Cr@std@@V?$basic_string_view@_WU?$char_traits@_W@__Cr@std@@@34@@Z)",
        LR"(?WideToUTF16@base@@YA?AV?$basic_string@_SU?$char_traits@_S@__Cr@std@@V?$allocator@_S@23@@__Cr@std@@V?$basic_string_view@_WU?$char_traits@_W@__Cr@std@@@45@@Z)",
        LR"(?WideToUTF16@base@@YA?AV?$basic_string@_SU?$char_traits@_S@__Cr@std@@V?$allocator@_S@23@@__Cr@std@@V?$basic_string_view@_WU?$char_traits@_W@__Cr@std@@@__Cr@std@@@Z)"},
       &g_WideToUTF16, nullptr, true},

      {{LR"(?SetIcon@MenuItemView@views@@QEAAXAEBVImageModel@ui@@@Z)"},
       &g_MenuItemSetIconOriginal, MenuItemSetIconHook, true},

      {{LR"(??0ImageModel@ui@@QEAA@XZ)"}, &g_EmptyImageModelCtor, nullptr, true},
      {{LR"(??1ImageModel@ui@@QEAA@XZ)"}, &g_EmptyImageModelDtor, nullptr, true},

      {{LR"(?ShouldShowAcceleratorText@MenuConfig@views@@QEBA_NPEBVMenuItemView@2@PEAV?$basic_string@_SU?$char_traits@_S@__Cr@std@@V?$allocator@_S@23@@__Cr@std@@@Z)",
        L"views::MenuConfig::ShouldShowAcceleratorText"},
       &g_MenuShouldShowAcceleratorTextOriginal, MenuShouldShowAcceleratorTextHook, true},

      {{L"views::MenuItemView::GetFontList",
        LR"(?GetFontList@MenuItemView@views@@QEBA?BVFontList@gfx@@XZ)"},
       &g_MenuItemGetFontListOriginal,
       MenuItemGetFontListHook,
       true},

      {{L"views::MenuItemView::GetVerticalMargin",
        LR"(?GetVerticalMargin@MenuItemView@views@@QEBAHXZ)"},
       &g_MenuItemGetVerticalMarginOriginal,
       MenuItemGetVerticalMarginHook,
       true},

      {{L"ui::NativeTheme::PaintMenuItemBackground"},
       &g_PaintMenuItemBackgroundOriginal,
       PaintMenuItemBackgroundHook,
       true},

      {{LR"(?GetColor@ColorProvider@ui@@QEBAIH@Z)", L"ui::ColorProvider::GetColor"},
       &g_ColorProviderGetColorOriginal,
       ColorProviderGetColorHook,
       true},

      {{LR"(?OnPaintBackground@MenuScrollViewContainer@views@@MEAAXPEAVCanvas@gfx@@@Z)",
        L"views::MenuScrollViewContainer::OnPaintBackground"},
       &g_MenuContainerPaintBackgroundOriginal,
       MenuContainerPaintBackgroundHook,
       true},

      // Use the exact overload: the other SaveLayerAlpha takes a Rect too.
      {{LR"(?SaveLayerAlpha@Canvas@gfx@@QEAAXE@Z)"},
       &g_CanvasSaveLayerAlpha,
       nullptr,
       true},

      {{LR"(?Restore@Canvas@gfx@@QEAAXXZ)"},
       &g_CanvasRestore,
       nullptr,
       true},

      // Exact two-argument overload; SkColor is a uint32_t.
      {{LR"(?DrawColor@Canvas@gfx@@QEAAXIW4SkBlendMode@@@Z)"},
       &g_CanvasDrawColor,
       nullptr,
       true},

      // Resolve the base implementation directly (not virtual dispatch back
      // into MenuContainerGetInsetsHook) to retain the native bubble frame.
      {{LR"(?GetInsets@View@views@@UEBA?AVInsets@gfx@@XZ)",
        L"views::View::GetInsets"},
       &g_ViewGetInsets,
       nullptr,
       true},

      {{LR"(?HasBubbleBorder@MenuScrollViewContainer@views@@QEBA_NXZ)",
        L"views::MenuScrollViewContainer::HasBubbleBorder"},
       &g_MenuContainerHasBubbleBorder,
       nullptr,
       true},

      {{LR"(?GetLocalBounds@View@views@@QEBA?AVRect@gfx@@XZ)"},
       &g_ViewGetLocalBounds, nullptr, true},
      {{LR"(?OnPaintBorder@View@views@@MEAAXPEAVCanvas@gfx@@@Z)",
        L"views::View::OnPaintBorder"},
       &g_ViewOnPaintBorderOriginal, ViewOnPaintBorderHook, true},
      {{LR"(?GetCornerRadius@MenuScrollViewContainer@views@@AEBAHXZ)",
        L"views::MenuScrollViewContainer::GetCornerRadius"},
       &g_MenuContainerGetCornerRadius, nullptr, true},
      {{LR"(?instance@MenuConfig@views@@SAAEBU12@XZ)"},
       &g_MenuConfigInstance, nullptr, true},
      {{LR"(?CornerRadiusForMenu@MenuConfig@views@@QEBAHPEBVMenuController@2@@Z)"},
       &g_MenuConfigCornerRadius, nullptr, true},
      {{LR"(??0RoundRectPainter@views@@QEAA@IH@Z)"},
       &g_RoundRectPainterCtor, nullptr, true},
      {{LR"(?Paint@RoundRectPainter@views@@UEAAXPEAVCanvas@gfx@@AEBVSize@4@@Z)",
        L"views::RoundRectPainter::Paint"},
       &g_RoundRectPainterPaint, nullptr, true},
      {{LR"(??1RoundRectPainter@views@@UEAA@XZ)"},
       &g_RoundRectPainterDtor, nullptr, true},
      {{LR"(??_GRoundRectPainter@views@@UEAAPEAXI@Z)"},
       &g_RoundRectPainterDeletingDtor, nullptr, true},
      {{LR"(?Save@Canvas@gfx@@QEAAXXZ)"},
       &g_CanvasSave, nullptr, true},
      {{LR"(?Translate@Canvas@gfx@@QEAAXAEBVVector2d@2@@Z)"},
       &g_CanvasTranslate, nullptr, true},

      {{LR"(?GetInsets@MenuScrollViewContainer@views@@UEBA?AVInsets@gfx@@XZ)",
        L"views::MenuScrollViewContainer::GetInsets"},
       &g_MenuContainerGetInsetsOriginal,
       MenuContainerGetInsetsHook,
       true},

      {{LR"(??0FontList@gfx@@QEAA@AEBV01@@Z)"}, &g_FontListCopyCtor, nullptr, true},

      {{LR"(?GetFontSize@FontList@gfx@@QEBAHXZ)"},
       &g_FontListGetFontSize,
       nullptr,
       true},

      {{LR"(?DeriveWithSizeDelta@FontList@gfx@@QEBA?AV12@H@Z)"},
       &g_FontListDeriveWithSizeDelta,
       nullptr,
       true},

      {{LR"(??1FontList@gfx@@QEAA@XZ)"},
       &g_FontListDtor,
       nullptr,
       true},

      // -----------------------------------------------------------------------
      // Tabs / Views: OPTIONAL
      // -----------------------------------------------------------------------

      {{L"TabTitle::TabTitle", LR"(??0TabTitle@@QEAA@XZ)"}, &g_TabTitleCtorOriginal, TabTitleCtorHook, true},

      {{L"TabTitle::~TabTitle", LR"(??1TabTitle@@UEAA@XZ)"}, &g_TabTitleDtorOriginal, TabTitleDtorHook, true},

      {{LR"(??0TabCloseButton@@QEAA@VPressedCallback@Button@views@@V?$RepeatingCallback@$$A6AXPEAVView@views@@AEBVMouseEvent@ui@@@Z@base@@@Z)",
        L"TabCloseButton::TabCloseButton"},
       &g_TabCloseButtonCtorOriginal,
       TabCloseButtonCtorHook,
       true},

      {{L"TabCloseButton::~TabCloseButton", LR"(??1TabCloseButton@@UEAA@XZ)"},
       &g_TabCloseButtonDtorOriginal,
       TabCloseButtonDtorHook,
       true},

      {{L"views::View::SetVisible", LR"(?SetVisible@View@views@@QEAAX_N@Z)"},
       &g_ViewSetVisibleOriginal,
       ViewSetVisibleHook,
       true},

      {{L"views::View::InvalidateLayout", LR"(?InvalidateLayout@View@views@@QEAAX_N@Z)"},
       &g_ViewInvalidateLayout,
       nullptr,
       true},

      {{LR"(?PreferredSizeChanged@View@views@@UEAAXXZ)", L"views::View::PreferredSizeChanged"},
       &g_ViewPreferredSizeChanged,
       nullptr,
       true},

      {{LR"(?GetLayoutConstant@@YAHW4LayoutConstant@@@Z)", L"GetLayoutConstant"},
       &g_GetLayoutConstantOriginal,
       GetLayoutConstantHook,
       true},

      // -----------------------------------------------------------------------
      // Extension toolbar: OPTIONAL
      // -----------------------------------------------------------------------

      {{LR"(??0ToolbarActionView@@QEAA@PEAVToolbarActionViewModel@@PEAVDelegate@0@@Z)"},
       &g_ToolbarActionViewCtorOriginal,
       ToolbarActionViewCtorHook,
       true},

      {{LR"(?CalculatePreferredSize@ToolbarActionView@@EEBA?AVSize@gfx@@AEBVSizeBounds@views@@@Z)",
        L"ToolbarActionView::CalculatePreferredSize"},
       &g_ToolbarActionViewCalculatePreferredSizeOriginal,
       ToolbarActionViewCalculatePreferredSizeHook,
       true},

      {{LR"(??_GToolbarActionView@@UEAAPEAXI@Z)"},
       &g_ToolbarActionViewDeletingDtorOriginal,
       ToolbarActionViewDeletingDtorHook,
       true},

      {{LR"(?UpdateState@ToolbarActionView@@QEAAXXZ)", L"ToolbarActionView::UpdateState"},
       &g_ToolbarActionViewUpdateState,
       nullptr,
       true},

      {{LR"(??0ExtensionsToolbarDesktop@@QEAA@PEAVBrowser@@W4DisplayMode@0@@Z)"},
       &g_ExtensionsToolbarDesktopCtorOriginal,
       ExtensionsToolbarDesktopCtorHook,
       true},

      {{LR"(??_GExtensionsToolbarDesktop@@UEAAPEAXI@Z)"},
       &g_ExtensionsToolbarDesktopDeletingDtorOriginal,
       ExtensionsToolbarDesktopDeletingDtorHook,
       true},
  };

  if (!ResolveChromeAddresses(chromeDll, chromeDllHooks, ARRAYSIZE(chromeDllHooks))) {
    Wh_Log(L"ERROR: Failed to prepare required Chrome addresses/hooks");

    return false;
  }

  Wh_Log(L"Menu icon hiding: %ls",
         g_MenuItemSetIconOriginal && g_EmptyImageModelCtor && g_EmptyImageModelDtor
             ? L"ready" : L"unavailable");
  Wh_Log(L"Menu shortcut label hiding: %ls",
         g_MenuShouldShowAcceleratorTextOriginal ? L"ready" : L"unavailable");
  Wh_Log(L"Page context-menu visibility toggles: %ls",
         g_ContextMenuIsCommandIdVisibleOriginal ? L"ready" : L"unavailable");
  g_menuScopeDetectionReady.store(g_MenuControllerRunOriginal != nullptr,
                                  std::memory_order_relaxed);
  Wh_Log(L"Menu scope detection: %ls",
         g_MenuControllerRunOriginal
             ? L"ready"
             : L"unavailable (appearance applies to every menu)");
  Wh_Log(L"Menu scope owners: page=%ls app=%ls tab=%ls bookmarks=%ls",
         g_ToolkitDelegateRunMenuAtOriginal ? L"ready" : L"missing",
         g_AppMenuCtorOriginal ? L"ready" : L"missing",
         g_ShowContextMenuForTabOriginal ? L"ready" : L"missing",
         g_BookmarkContextMenuCtorOriginal ? L"ready" : L"missing");

  const bool customItemsReady =
      g_ToolkitDelegateInitOriginal && g_ContextMenuExecuteCommandOriginal &&
      g_ContextMenuIsCommandIdVisibleOriginal &&
      g_ContextMenuIsCommandIdEnabledOriginal && g_SimpleMenuModelAddItem &&
      g_WideToUTF16;

  g_customMenuItemsReady.store(customItemsReady, std::memory_order_release);

  Wh_Log(L"Custom context menu items: %ls",
         customItemsReady ? L"ready" : L"unavailable");
  Wh_Log(L"Extension grouping: %ls",
         ExtensionGroupingReady() ? L"ready" : L"unavailable (native menus retained)");
  Wh_Log(L"Page menu hiding by label: %ls",
         PageLabelFilteringReady() ? L"ready" : L"unavailable (custom items only)");
  Wh_Log(L"Main page context-menu icon hiding: %ls",
         MainPageMenuIconTrackingReady() && g_MenuItemSetIconOriginal &&
                 g_EmptyImageModelCtor && g_EmptyImageModelDtor
             ? L"ready" : L"unavailable");
  Wh_Log(L"Custom item positioning: %ls; positioned separators: %ls",
         CustomMenuPositioningReady() ? L"ready" : L"unavailable (items append at bottom)",
         g_SimpleMenuModelInsertSeparatorAt ? L"ready" : L"unavailable");
  Wh_Log(L"Custom item functions: toolkitInit=%d execute=%d enabled=%d checked=%d "
         L"addItem=%d separator=%d wideToUtf16=%d",
         !!g_ToolkitDelegateInitOriginal, !!g_ContextMenuExecuteCommandOriginal,
         !!g_ContextMenuIsCommandIdEnabledOriginal,
         !!g_ContextMenuIsCommandIdCheckedOriginal, !!g_SimpleMenuModelAddItem,
         !!g_SimpleMenuModelAddSeparator, !!g_WideToUTF16);

  bool tabFontReady =
      g_TabTitleCtorOriginal && g_TabTitleDtorOriginal && g_LabelSetFontList &&
      g_FontListCopyCtor && g_FontListGetFontSize && g_FontListDeriveWithSizeDelta &&
      g_FontListDtor;

  bool tabCloseReady = g_TabCloseButtonCtorOriginal && g_TabCloseButtonDtorOriginal && g_ViewSetVisibleOriginal &&
                       g_ViewInvalidateLayout && g_GetLayoutConstantOriginal;

  bool extensionWidthReady = g_ToolbarActionViewCalculatePreferredSizeOriginal;

  Wh_Log(L"Menu tweaks: fontList=%ls exactFontSizing=%ls verticalMargin=%ls cornerRadius=%ls",
         g_MenuItemGetFontListOriginal ? L"ready" : L"MISSING",
         g_FontListCopyCtor && g_FontListGetFontSize && g_FontListDeriveWithSizeDelta &&
                 g_FontListDtor
             ? L"ready"
             : L"unavailable",
         g_MenuItemGetVerticalMarginOriginal ? L"ready" : L"MISSING",
         g_PaintMenuItemBackgroundOriginal ? L"ready" : L"MISSING");
  Wh_Log(L"Menu group padding hook: %ls",
         g_MenuContainerGetInsetsOriginal ? L"ready" : L"MISSING");
  Wh_Log(L"Menu top/bottom spacing: frameInsets=%ls bubbleCheck=%ls",
         g_ViewGetInsets ? L"ready" : L"MISSING",
         g_MenuContainerHasBubbleBorder ? L"ready" : L"MISSING");
  Wh_Log(L"Menu background transparency: %ls",
         g_MenuContainerPaintBackgroundOriginal && g_CanvasSaveLayerAlpha && g_CanvasRestore &&
                 g_CanvasDrawColor
             ? L"ready" : L"unavailable");
  Wh_Log(L"Custom menu group border: %ls",
         MenuGroupBorderReady() ? L"ready" : L"unavailable");
  Wh_Log(L"Border functions: borderPaint=%d bounds=%d radius=%d config=%d configRadius=%d bubble=%d "
         L"insets=%d ctor=%d paint=%d dtor=%d deletingDtor=%d save=%d restore=%d translate=%d",
         !!g_ViewOnPaintBorderOriginal, !!g_ViewGetLocalBounds, !!g_MenuContainerGetCornerRadius, !!g_MenuConfigInstance,
         !!g_MenuConfigCornerRadius, !!g_MenuContainerHasBubbleBorder, !!g_ViewGetInsets,
         !!g_RoundRectPainterCtor, !!g_RoundRectPainterPaint, !!g_RoundRectPainterDtor,
         !!g_RoundRectPainterDeletingDtor, !!g_CanvasSave, !!g_CanvasRestore, !!g_CanvasTranslate);
  Wh_Log(L"Menu item background color: %ls",
         g_PaintMenuItemBackgroundOriginal && g_ColorProviderGetColorOriginal
             ? L"ready" : L"unavailable");
  Wh_Log(L"Menu background color: %ls",
         g_MenuContainerPaintBackgroundOriginal && g_CanvasSaveLayerAlpha &&
                 g_CanvasRestore && g_CanvasDrawColor
             ? L"ready" : L"unavailable");

  Wh_Log(
      L"Extension live functions: actionCtor=%ls actionDeletingDtor=%ls "
      L"preferredSizeChanged=%ls",
      g_ToolbarActionViewCtorOriginal ? L"ready" : L"MISSING",
      g_ToolbarActionViewDeletingDtorOriginal ? L"ready" : L"MISSING",
      g_ViewPreferredSizeChanged ? L"ready" : L"MISSING");

  Wh_Log(L"Extension container functions: ctor=%ls deletingDtor=%ls",
         g_ExtensionsToolbarDesktopCtorOriginal ? L"ready" : L"MISSING",
         g_ExtensionsToolbarDesktopDeletingDtorOriginal ? L"ready" : L"MISSING");

  bool extensionTrackingReady = extensionWidthReady && g_ToolbarActionViewCtorOriginal &&
                                g_ToolbarActionViewDeletingDtorOriginal && g_ViewPreferredSizeChanged;

  bool extensionContainerTrackingReady = g_ExtensionsToolbarDesktopCtorOriginal &&
                                         g_ExtensionsToolbarDesktopDeletingDtorOriginal && g_ViewPreferredSizeChanged;

  g_tabFontHooksReady.store(tabFontReady, std::memory_order_release);

  g_tabCloseHooksReady.store(tabCloseReady, std::memory_order_release);

  g_extensionTrackingReady.store(extensionTrackingReady, std::memory_order_release);

  g_extensionContainerTrackingReady.store(extensionContainerTrackingReady, std::memory_order_release);

  Wh_Log(L"Chrome UI addresses resolved successfully");

  Wh_Log(L"Tab title tweak: %ls", tabFontReady ? L"ready" : L"unavailable");

  Wh_Log(L"Tab close/layout tweak: %ls", tabCloseReady ? L"ready" : L"unavailable");

  Wh_Log(L"Extension width tweak: %ls", extensionWidthReady ? L"ready" : L"unavailable");

  Wh_Log(L"Extension width live update: %ls", extensionTrackingReady ? L"ready" : L"unavailable");

  Wh_Log(L"Extension container live propagation: %ls", extensionContainerTrackingReady ? L"ready" : L"unavailable");

  Wh_Log(L"Extension icon live regeneration: %ls", g_ToolbarActionViewUpdateState ? L"ready" : L"unavailable");

  return true;
}

// -----------------------------------------------------------------------------
// Slow setup notifications
// -----------------------------------------------------------------------------

static void UpdateSetupTooltip(NOTIFYICONDATAW& notifyIcon, const wchar_t* text) {
  notifyIcon.uFlags = NIF_TIP | NIF_SHOWTIP;
  wcsncpy_s(notifyIcon.szTip, text, _TRUNCATE);

  if (!Shell_NotifyIconW(NIM_MODIFY, &notifyIcon)) {
    Wh_Log(L"Failed to update setup tooltip: %lu", GetLastError());
  }
}

static void ShowSetupNotification(NOTIFYICONDATAW& notifyIcon,
                                   const wchar_t* title,
                                   const wchar_t* text,
                                   DWORD infoFlags) {
  notifyIcon.uFlags = NIF_INFO;
  notifyIcon.dwInfoFlags = infoFlags | NIIF_NOSOUND;
  wcsncpy_s(notifyIcon.szInfoTitle, title, _TRUNCATE);
  wcsncpy_s(notifyIcon.szInfo, text, _TRUNCATE);

  if (!Shell_NotifyIconW(NIM_MODIFY, &notifyIcon)) {
    Wh_Log(L"Failed to show setup notification: %lu", GetLastError());
  }
}

static DWORD WaitForHandlesWithMessageLoop(const HANDLE* handles, DWORD handleCount) {
  for (;;) {
    DWORD waitResult = MsgWaitForMultipleObjectsEx(
        handleCount, handles, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

    if (waitResult >= WAIT_OBJECT_0 && waitResult < WAIT_OBJECT_0 + handleCount) {
      return waitResult;
    }

    if (waitResult == WAIT_OBJECT_0 + handleCount) {
      MSG message;

      while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
        if (message.message == WM_QUIT) {
          return WAIT_ABANDONED;
        }

        TranslateMessage(&message);
        DispatchMessageW(&message);
      }

      continue;
    }

    return waitResult;
  }
}

static DWORD WINAPI SetupNotificationThreadProc(void*) {
  HWND window =
      CreateWindowExW(0, L"STATIC", L"", WS_OVERLAPPED, 0, 0, 0, 0, nullptr, nullptr, nullptr, nullptr);

  if (!window) {
    Wh_Log(L"Failed to create setup notification window: %lu", GetLastError());
    return 0;
  }

  NOTIFYICONDATAW notifyIcon = {};
  notifyIcon.cbSize = sizeof(notifyIcon);
  notifyIcon.hWnd = window;
  notifyIcon.uID = 1;
  notifyIcon.uFlags = NIF_ICON | NIF_TIP | NIF_SHOWTIP;
  notifyIcon.hIcon = LoadIconW(nullptr, IDI_INFORMATION);
  wcsncpy_s(
      notifyIcon.szTip,
      L"Chrome Context Menu Items: preparing Chrome in the background",
      _TRUNCATE);

  if (!Shell_NotifyIconW(NIM_ADD, &notifyIcon)) {
    Wh_Log(L"Failed to create setup notification icon: %lu", GetLastError());
    DestroyWindow(window);
    return 0;
  }

  notifyIcon.uVersion = NOTIFYICON_VERSION_4;
  Shell_NotifyIconW(NIM_SETVERSION, &notifyIcon);

  // If setup happened to finish immediately after the 5-second
  // timeout, skip the transient "preparing" notification and show only the
  // completion result.
  if (WaitForSingleObject(g_chromePreparationDoneEvent, 0) != WAIT_OBJECT_0) {
    ShowSetupNotification(
        notifyIcon, L"Chrome Context Menu Items",
        L"Chrome is being prepared in the background, you can keep using it.",
        NIIF_INFO);
  }

  HANDLE waitHandles[] = {g_chromePreparationDoneEvent, g_setupNotificationStopEvent};
  DWORD waitResult = WaitForHandlesWithMessageLoop(waitHandles, ARRAYSIZE(waitHandles));

  if (waitResult == WAIT_OBJECT_0) {
    bool success = g_chromePreparationSucceeded.load(std::memory_order_acquire);

    if (success) {
      UpdateSetupTooltip(
          notifyIcon,
          L"Chrome Context Menu Items: ready - restart Chrome to activate the mod");
      ShowSetupNotification(
          notifyIcon, L"Chrome Context Menu Items",
          L"Chrome setup is complete. Restart Chrome to activate the mod.",
          NIIF_INFO);
    } else {
      UpdateSetupTooltip(
          notifyIcon,
          L"Chrome Context Menu Items: setup failed - see the Windhawk mod log");
      ShowSetupNotification(
          notifyIcon, L"Chrome Context Menu Items",
          L"Chrome setup failed. The mod wasn't activated; see the Windhawk mod log.",
          NIIF_ERROR);
    }

    // Keep pumping this top-level window while the status icon remains alive,
    // so system broadcasts can't block on an unresponsive notification thread.
    HANDLE stopHandles[] = {g_setupNotificationStopEvent};
    waitResult = WaitForHandlesWithMessageLoop(stopHandles, ARRAYSIZE(stopHandles));

    if (waitResult != WAIT_OBJECT_0 && waitResult != WAIT_ABANDONED) {
      Wh_Log(L"Setup notification stop wait failed: %lu", GetLastError());
    }
  } else if (waitResult != WAIT_OBJECT_0 + 1 && waitResult != WAIT_ABANDONED) {
    Wh_Log(L"Setup notification wait failed: %lu", GetLastError());
  }

  Shell_NotifyIconW(NIM_DELETE, &notifyIcon);
  DestroyWindow(window);
  return 0;
}

static void StartSetupNotifications() {
  std::lock_guard<std::mutex> lock(g_workerMutex);

  if (g_unloading || g_setupNotificationThread) return;

  g_setupNotificationStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

  if (!g_setupNotificationStopEvent) {
    Wh_Log(L"Failed to create setup notification stop event: %lu", GetLastError());
    return;
  }

  g_setupNotificationThread =
      CreateThread(nullptr, 0, SetupNotificationThreadProc, nullptr, 0, nullptr);

  if (!g_setupNotificationThread) {
    Wh_Log(L"Failed to create setup notification thread: %lu", GetLastError());
    CloseHandle(g_setupNotificationStopEvent);
    g_setupNotificationStopEvent = nullptr;
  }
}

// -----------------------------------------------------------------------------
// Bounded startup setup
// -----------------------------------------------------------------------------

static DWORD WINAPI ChromePreparationThreadProc(void* param) {
  HMODULE chromeDll = static_cast<HMODULE>(param);
  ULONGLONG startedAt = GetTickCount64();

  bool success = InstallChromeHooks(chromeDll);

  // If the 5-second startup window has already been abandoned, this resolver
  // run is cache preparation only. Don't leave feature-level "ready" flags set
  // when no hooks will be applied in this Chrome instance.
  if (g_hookActivationAbandoned.load(std::memory_order_acquire)) {
    ClearChromeRuntimeReadiness();
  }

  g_chromePreparationSucceeded.store(success, std::memory_order_release);

  ULONGLONG elapsed = GetTickCount64() - startedAt;

  Wh_Log(L"Chrome setup finished in %llu ms: %ls", elapsed,
         success ? L"success" : L"FAILED");

  SetEvent(g_chromePreparationDoneEvent);

  return 0;
}

static bool ActivatePreparedChromeHooks() {
  if (g_hookActivationAbandoned.load(std::memory_order_acquire)) {
    ClearChromeRuntimeReadiness();
    Wh_Log(L"Chrome hook activation was abandoned while setup was running");
    return false;
  }
  if (!Wh_ApplyHookOperations()) {
    Wh_Log(L"Wh_ApplyHookOperations failed");
    ClearChromeRuntimeReadiness();
    return false;
  }
  g_hooksActivated.store(true, std::memory_order_release);
  return true;
}

static void PrepareChromeHooksSynchronously(HMODULE chromeDll) {
  if (g_hookActivationAbandoned.load(std::memory_order_acquire)) return;
  if (InstallChromeHooks(chromeDll)) ActivatePreparedChromeHooks();
}

static void StartChromeHookSetup(HMODULE chromeDll) {
  {
    std::lock_guard<std::mutex> lock(g_workerMutex);

    if (g_unloading) {
      Wh_Log(L"Skipping Chrome hook setup because the mod is unloading");
      return;
    }

    if (g_chromeSetupStarted.exchange(true)) {
      return;
    }

    g_chromeSetupInProgress = true;
  }

  struct SetupCompletionGuard {
    ~SetupCompletionGuard() {
      {
        std::lock_guard<std::mutex> lock(g_workerMutex);
        g_chromeSetupInProgress = false;
      }

      g_workerCondition.notify_all();
    }
  } setupCompletionGuard;

  {
    std::lock_guard<std::mutex> lock(g_workerMutex);

    if (g_unloading) {
      Wh_Log(L"Chrome setup abandoned before worker creation");
      return;
    }

    g_chromePreparationDoneEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
  }

  if (!g_chromePreparationDoneEvent) {
    Wh_Log(L"Failed to create Chrome setup event");

    PrepareChromeHooksSynchronously(chromeDll);
    return;
  }

  {
    std::lock_guard<std::mutex> lock(g_workerMutex);

    if (!g_unloading) {
      g_chromePreparationThread =
          CreateThread(nullptr, 0, ChromePreparationThreadProc, chromeDll, 0, nullptr);
    }
  }

  if (!g_chromePreparationThread) {
    if (g_hookActivationAbandoned.load(std::memory_order_acquire)) {
      return;
    }

    Wh_Log(L"Failed to create Chrome setup thread");

    {
      std::lock_guard<std::mutex> lock(g_workerMutex);
      CloseHandle(g_chromePreparationDoneEvent);
      g_chromePreparationDoneEvent = nullptr;
    }

    PrepareChromeHooksSynchronously(chromeDll);
    return;
  }

  ULONGLONG waitStartedAt = GetTickCount64();
  DWORD waitResult = WaitForSingleObject(g_chromePreparationDoneEvent, kChromeStartupWaitMs);
  ULONGLONG waited = GetTickCount64() - waitStartedAt;

  if (waitResult == WAIT_OBJECT_0) {
    bool success = g_chromePreparationSucceeded.load(std::memory_order_acquire);

    // Keep the resolver thread/event handles owned by Wh_ModBeforeUninit. This
    // avoids a close/read race if the mod is disabled around the startup wait.
    Wh_Log(L"Chrome setup completed within startup wait (%llu ms)", waited);

    if (!success) return;

    if (ActivatePreparedChromeHooks()) Wh_Log(L"Chrome hooks activated during startup");
    return;
  }

  if (waitResult == WAIT_TIMEOUT) {
    // Don't late-apply hooks when resolution finishes. Constructor-based
    // tracking would miss UI objects that Chrome already created, producing a
    // partially active mod. The worker is intentionally allowed to finish only
    // so the next Chrome launch can use the saved addresses.
    g_hookActivationAbandoned.store(true, std::memory_order_release);
    ClearChromeRuntimeReadiness();

    Wh_Log(
        L"Chrome startup wait timed out after %llu ms; continuing Chrome "
        L"without UI tweaks. After setup finishes, restart Chrome to activate the mod",
        waited);
    StartSetupNotifications();
    return;
  }

  g_hookActivationAbandoned.store(true, std::memory_order_release);
  ClearChromeRuntimeReadiness();

  Wh_Log(L"Chrome setup wait failed: %lu; continuing Chrome without UI tweaks", GetLastError());
  StartSetupNotifications();
  return;
}

// -----------------------------------------------------------------------------
// Delayed chrome.dll loading
// -----------------------------------------------------------------------------

using LoadLibraryExWFn = decltype(&LoadLibraryExW);

static LoadLibraryExWFn g_LoadLibraryExWOriginal;

static HMODULE WINAPI LoadLibraryExWHook(LPCWSTR fileName, HANDLE file, DWORD flags) {
  HMODULE module = g_LoadLibraryExWOriginal(fileName, file, flags);

  if (!module || g_chromeSetupStarted.load()) {
    return module;
  }

  HMODULE chromeDll = GetModuleHandleW(L"chrome.dll");

  if (chromeDll && chromeDll == module) {
    Wh_Log(L"chrome.dll loaded");
    StartChromeHookSetup(chromeDll);
  }

  return module;
}

// -----------------------------------------------------------------------------
// Windhawk lifecycle
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
  int argc = 0;
  LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

  if (!argv) {
    Wh_Log(L"CommandLineToArgvW failed");
    return FALSE;
  }

  bool isSubprocess = false;
  bool isRemoteDebuggingPipe = false;

  for (int i = 1; i < argc; i++) {
    if (wcsncmp(argv[i], L"--type=", 7) == 0) {
      isSubprocess = true;
      break;
    }

    if (wcscmp(argv[i], L"--remote-debugging-pipe") == 0) {
      isRemoteDebuggingPipe = true;
    }
  }

  LocalFree(argv);

  if (isSubprocess) return FALSE;

  if (isRemoteDebuggingPipe) {
    Wh_Log(L"Skipping CDP-controlled Chrome process");
    return FALSE;
  }

  LoadSettings();

  Wh_Log(L"Main Chrome process, PID=%lu", GetCurrentProcessId());

  HMODULE chromeDll = GetModuleHandleW(L"chrome.dll");

  if (chromeDll) {
    Wh_Log(L"chrome.dll already loaded; deferring Chrome setup until Wh_ModAfterInit");
    return TRUE;
  }

  Wh_Log(L"chrome.dll not loaded yet; waiting for LoadLibraryExW");

  HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");

  if (!kernelBase) {
    Wh_Log(L"Failed to get kernelbase.dll");

    return FALSE;
  }

  auto loadLibraryExW =
      reinterpret_cast<LoadLibraryExWFn>(GetProcAddress(kernelBase, "LoadLibraryExW"));

  if (!loadLibraryExW) {
    Wh_Log(L"Failed to get kernelbase!LoadLibraryExW");

    return FALSE;
  }

  if (!WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExWHook, &g_LoadLibraryExWOriginal)) {
    Wh_Log(L"Failed to hook kernelbase!LoadLibraryExW");

    return FALSE;
  }

  return TRUE;
}

void Wh_ModAfterInit() {
  if (g_chromeSetupStarted.load()) return;

  HMODULE chromeDll = GetModuleHandleW(L"chrome.dll");

  if (!chromeDll) return;

  StartChromeHookSetup(chromeDll);
}

void Wh_ModSettingsChanged() {

  LoadSettings();

  if (!g_hooksActivated.load(std::memory_order_acquire)) return;

  ApplyTweaksToExistingTabs();
  ApplyWidthToExistingExtensionButtons();
}

void Wh_ModBeforeUninit() {
  {
    std::lock_guard<std::mutex> lock(g_customGroupsMutex);
    g_customGroupTrackingEnabled = false;
    g_menuModelDelegates.clear();
    // An open menu can still hold a child pointer. Keep its native model alive
    // until Chrome exits rather than leaving a dangling pointer after unload.
    for (auto& [root, storage] : g_customGroups) storage.release();
    g_customGroups.clear();
  }
  g_hideMainPageMenuIcons.store(false, std::memory_order_relaxed);
  {
    std::lock_guard<std::mutex> lock(g_pageMenuIconMutex);
    g_pageMenuRootModels.clear();
    g_mainPageMenuItemViews.clear();
    g_pageLabelModels.clear();
    g_hiddenLabelIndices.clear();
  }
  g_groupExtensions.store(false);
  {
    std::lock_guard<std::mutex> lock(g_extensionGroupsMutex);
    // Normally the matcher destructor reclaims these models. If a menu is
    // still open during unload, keep its native model alive: its vtable and
    // delegate point only to Chrome, never to this mod. Freeing it now would
    // leave Chrome's visible submenu and matcher with dangling pointers.
    for (auto& [matcher, group] : g_extensionGroups) group.model.release();
    g_extensionGroups.clear();
  }
  // Stop adding custom entries first, then wait for any launch worker so no
  // thread is left running code from this module.
  g_customMenuItemsReady.store(false, std::memory_order_release);

  {
    std::lock_guard<std::mutex> lock(g_customItemsMutex);
    g_customItems.clear();
  }

  WaitForCustomMenuItemLaunches();

  g_hideMenuIcons.store(false, std::memory_order_relaxed);
  g_hideMenuShortcutLabels.store(false, std::memory_order_relaxed);
  for (auto& item : g_contextMenuVisibility) {
    item.hidden.store(false, std::memory_order_relaxed);
  }

  HANDLE setupNotificationThread = nullptr;
  HANDLE setupNotificationStopEvent = nullptr;
  HANDLE preparationThread = nullptr;
  HANDLE preparationDoneEvent = nullptr;

  {
    std::unique_lock<std::mutex> lock(g_workerMutex);

    // Close the worker-start gate before inspecting any handles. If
    // StartChromeHookSetup is already running on Chrome's loader thread, wait
    // until it has finished creating/using its startup handles. No new worker
    // can be created after g_unloading becomes true.
    g_unloading = true;
    g_hookActivationAbandoned.store(true, std::memory_order_release);

    if (g_setupNotificationStopEvent) {
      SetEvent(g_setupNotificationStopEvent);
    }

    g_workerCondition.wait(lock, [] { return !g_chromeSetupInProgress; });

    // StartChromeHookSetup may have created the notification thread just before
    // teardown acquired the gate, so signal the stop event again after the
    // setup call has fully returned.
    if (g_setupNotificationStopEvent) {
      SetEvent(g_setupNotificationStopEvent);
    }

    setupNotificationThread = g_setupNotificationThread;
    setupNotificationStopEvent = g_setupNotificationStopEvent;
    preparationThread = g_chromePreparationThread;
    preparationDoneEvent = g_chromePreparationDoneEvent;
  }

  g_menuFontSize.store(-1, std::memory_order_relaxed);
  g_menuVerticalSpacing.store(-1, std::memory_order_relaxed);
  g_menuCornerRadius.store(-1, std::memory_order_relaxed);
  g_menuBackgroundOpacity.store(100, std::memory_order_relaxed);
  g_menuBackgroundColor.store(0, std::memory_order_relaxed);
  g_menuItemBackgroundColor.store(0, std::memory_order_relaxed);
  g_menuGroupBorderColor.store(0, std::memory_order_relaxed);
  g_menuGroupPadding.store(-1, std::memory_order_relaxed);
  g_menuGroupTopSpacing.store(-1, std::memory_order_relaxed);
  g_menuGroupBottomSpacing.store(-1, std::memory_order_relaxed);
  g_tabFontSize.store(-1, std::memory_order_relaxed);
  g_tabCloseButtonsHidden.store(false, std::memory_order_relaxed);
  g_tabPreTitlePadding.store(kChromeDefaultTabPreTitlePadding, std::memory_order_relaxed);
  g_extensionButtonWidth.store(kChromeDefaultExtensionButtonWidth, std::memory_order_relaxed);

  if (setupNotificationThread) {
    WaitForSingleObject(setupNotificationThread, INFINITE);
  }

  // Preparation cannot be cancelled safely. Wait for the resolver before the
  // mod DLL is unloaded so its worker can't continue executing unloaded code.
  if (preparationThread) {
    WaitForSingleObject(preparationThread, INFINITE);
  }

  if (setupNotificationThread) CloseHandle(setupNotificationThread);
  if (setupNotificationStopEvent) CloseHandle(setupNotificationStopEvent);
  if (preparationThread) CloseHandle(preparationThread);
  if (preparationDoneEvent) CloseHandle(preparationDoneEvent);

  {
    std::lock_guard<std::mutex> lock(g_workerMutex);
    g_setupNotificationThread = nullptr;
    g_setupNotificationStopEvent = nullptr;
    g_chromePreparationThread = nullptr;
    g_chromePreparationDoneEvent = nullptr;
  }

  if (!g_hooksActivated.load(std::memory_order_acquire)) return;

  ApplyTweaksToExistingTabs(true);
  ApplyWidthToExistingExtensionButtons();
}

void Wh_ModUninit() {
  Wh_Log(L"Chrome Context Menu Items unloaded");
}

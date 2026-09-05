// ==WindhawkMod==
// @id              chrome-native-ui-tweaks
// @name            Chrome Native UI Tweaks
// @description     Customize Chrome's native UI: fonts, tab close (×) buttons, menu density, extension spacing, and bookmark folder icons. Web content is untouched.
// @version         1.0
// @author          Dron007
// @github          https://github.com/Dron007
// @include         chrome.exe
// @architecture    x86-64
// @compilerOptions -lshell32
// ==/WindhawkMod==

// clang-format off
// ==WindhawkModSettings==
/*
- bookmarks:
  - fontSize: "default"
    $name: "Font size"
    $description: "Bookmark bar font size. Chrome default is usually 12."
    $options:
    - "default": "Chrome default (usually 12)"
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

  - folderIcon: "chrome"
    $name: "Folder icon"
    $description: "Folder icon used on the bookmark bar and in bookmark menus."
    $options:
    - "windows": "Windows system"
    - "chrome": "Chrome default"
  $name: "Bookmarks"

- addressBar:
  - fontSize: "default"
    $name: "Font size"
    $description: "Font size for URL and search text in the address bar. Chrome default is usually 14 and restores the original omnibox font unchanged."
    $options:
    - "default": "Chrome default (usually 14)"
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
  $name: "Address bar"

- tabs:
  - fontSize: "default"
    $name: "Title font size"
    $description: "Font size for titles in the horizontal tab strip. Chrome default is usually 12."
    $options:
    - "default": "Chrome default (usually 12)"
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
    $name: "Hide close (×) buttons"
    $description: "Hide tab close buttons and reclaim the horizontal space reserved for them."

  - iconTitleSpacing: "8"
    $name: "Icon/title spacing"
    $description: "Horizontal space between the favicon and tab title."
    $options:
    - "2": "2"
    - "3": "3"
    - "4": "4"
    - "5": "5"
    - "6": "6"
    - "7": "7"
    - "8": "8 (Chrome default)"
  $name: "Tabs"

- extensions:
  - buttonWidth: "34"
    $name: "Pinned extension spacing"
    $description: "Spacing between pinned extension icons. The value is the icon width plus horizontal padding."
    $options:
    - "28": "28"
    - "29": "29"
    - "30": "30"
    - "31": "31"
    - "32": "32"
    - "33": "33"
    - "34": "34 (Chrome default)"
  $name: "Extensions"

- menus:
  - fontSize: "default"
    $name: "Font size"
    $description: "Menu font size. Chrome default is usually 12."
    $options:
    - "default": "Chrome default (usually 12)"
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
    $name: "Vertical spacing"
    $description: "Top/bottom spacing for menu rows. Current Chrome normally uses 6. Some special rows can use smaller values."
    $options:
    - "default": "Chrome default (usually 6)"
    - "0": "0 (tightest)"
    - "1": "1"
    - "2": "2"
    - "3": "3"
    - "4": "4"
    - "5": "5"
    - "6": "6"

  - cornerRadius: "default"
    $name: "Corner radius"
    $description: "Outer corner radius of popup menus. Smaller values can also reduce the empty space above and below rounded menus."
    $options:
    - "default": "Chrome default"
    - "0": "0 (square)"
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
  $name: "Menus"
  $description: "Bookmark folder, context, and three-dot menus."
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Chrome Native UI Tweaks

Customize parts of Chrome's native browser interface without affecting web content.

![Example configuration](https://i.imgur.com/AQoyFDj.png)

## Getting started

1. Enable the mod.
2. Restart Chrome once.
3. Open the mod settings, adjust the options you want, and click **Save settings**.

All settings default to Chrome's normal UI. Once the mod is active, saved setting changes are applied immediately without restarting Chrome.

Disabling or uninstalling the mod restores the original UI in the running Chrome instance. Re-enabling the mod in that same Chrome instance requires a restart.

## Features

### Bookmarks
- Adjust the bookmark bar font size.
- Replace Chrome's bookmark folder icon with the Windows system folder icon.

### Address bar
- Adjust the font size of URL and search text in the address bar.

### Tabs
- Adjust the tab title font size.
- Hide tab close (×) buttons and reclaim the reserved space.
- Adjust the spacing between the favicon and tab title.

### Extensions
- Reduce spacing between pinned extension icons.

### Menus
- Adjust font size in bookmark folder, context, and three-dot menus.
- Reduce vertical spacing between menu items.
- Adjust popup menu corner radius.

![Menu customization](https://i.imgur.com/M6PIC6Z.png)

## Notes

- Tested with **Google Chrome 152.0.7977.76 x64**.
- The mod relies on Chrome's internal native UI symbols, so Chrome updates can occasionally require compatibility adjustments.
- After a Chrome update, Windhawk may need to download and resolve new `chrome.dll` symbols. The mod waits up to 5 seconds for cached symbols. If resolution takes longer, Chrome continues with its normal UI while symbol preparation finishes in the background. A temporary tray icon shows the current status on hover and displays short balloon messages when preparation starts and finishes. Keep that Chrome instance open until the status says the symbols are ready, then restart Chrome once to activate the mod.
- A full fresh-symbol fallback can be large. In a Chrome 152.0.7977.76 test with the local PDB and Windhawk symbol cache removed, the resulting uncompressed `chrome.dll.pdb` was **5.59 GB (5.21 GiB)** and total preparation took **139.5 seconds** on a **75 Mbit/s** connection and an **Intel Core i7-8700K**. The download finished after roughly two minutes and symbol analysis took roughly another 20 seconds. Symbol-server delivery is compressed, so the network transfer is much smaller than the final PDB. Times vary with connection, CPU, and Chrome build.
- Disabling, updating, or uninstalling the mod while fresh symbol preparation is still running can leave Windhawk in `uninitializing` until that work completes. This is intentional: the symbol resolver can't be cancelled safely while code from the mod is still executing.
- Chrome processes launched with `--remote-debugging-pipe` (for example, Playwright/CDP automation sessions) are intentionally skipped so auxiliary browser instances don't pay the native UI symbol-resolution cost.
- The optional Windows folder icon is a fixed Windows raster icon. It doesn't follow Chrome's theme colors and isn't fully DPI-aware on high-DPI or mixed-DPI monitor setups.
- The older **Chrome UI Tweaks** mod overlaps with this mod's menu spacing and corner-radius options. If both mods are used on a Chrome version where they work, configure those overlapping menu options in only one mod to avoid conflicting results.
*/
// ==/WindhawkModReadme==
// clang-format on

#include <windows.h>
#include <shellapi.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <cwchar>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

static constexpr PCWSTR kChromeSymbolServer = L"https://chromium-browser-symsrv.commondatastorage.googleapis.com";
static constexpr DWORD kChromeSymbolStartupWaitMs = 5000;
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

// The Windows-folder image is created once while the mod is loaded. Chromium
// object layouts aren't available to this mod at compile time, so use
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

static OpaqueObjectStorage g_skBitmapStorage;
static OpaqueObjectStorage g_imageSkiaStorage;

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

using BookmarkButtonBaseCtorFn = void (*)(void*, void*, const void*);

using BookmarkMenuButtonBaseCtorFn = void (*)(void*, void*, void*, const void*);

using LabelButtonLabelCtorFn = void (*)(void*, const void*, int);

using LabelButtonLabelDeletingDtorFn = void* (*)(void*, unsigned int);

using TypographyGetFontFn = const FontListOpaque* (*)(const void*, int, int);

using LabelSetFontListFn = void (*)(void*, const FontListOpaque*);

// Chrome 152 declares MenuItemView::GetFontList() as returning
// `const gfx::FontList` by value. On Win64 the member-function ABI uses:
// RCX = this, RDX = hidden result buffer.
using MenuItemGetFontListFn = FontListOpaque* (*)(const void*, FontListOpaque*);

using MenuItemGetVerticalMarginFn = int (*)(const void*);

using MenuConfigCornerRadiusForMenuFn = int (*)(const void*, const void*);

using FontListCopyCtorFn = void (*)(FontListOpaque*, const FontListOpaque*);

using FontListGetFontSizeFn = int (*)(const FontListOpaque*);

// gfx::FontList is returned through a hidden sret buffer on Win64:
// RCX = this
// RDX = result buffer
// R8  = size delta
using FontListDeriveWithSizeDeltaFn =
    FontListOpaque* (*)(const FontListOpaque*, FontListOpaque*, int);

using FontListDtorFn = void (*)(FontListOpaque*);

// OmniboxViewViews(bool, OmniboxController*, LocationBarView*,
//                  const gfx::FontList&)
using OmniboxViewViewsCtorFn =
    void (*)(void*, bool, void*, void*, const FontListOpaque*);

using OmniboxViewViewsDtorFn = void (*)(void*);

using TextfieldCtorFn = void (*)(void*);
using TextfieldSetFontListFn = void (*)(void*, const FontListOpaque*);

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

// Logical Chromium signature:
//   ui::ImageModel chrome::GetBookmarkFolderIcon(
//       BookmarkFolderIconType icon_type,
//       ui::ColorVariant color);
// Win64 ABI uses a hidden ImageModel result buffer in RCX.
using GetBookmarkFolderIconFn =
    ImageModelOpaque* (*)(ImageModelOpaque* result, int iconType, uintptr_t colorVariantOpaque);

// static SkBitmap IconUtil::CreateSkBitmapFromHICON(HICON);
using CreateSkBitmapFromHICONFn = void* (*)(void* result, HICON icon);

// static gfx::ImageSkia gfx::ImageSkia::CreateFrom1xBitmap(const SkBitmap&);
using ImageSkiaCreateFrom1xBitmapFn = void* (*)(void* result, const void* bitmap);

using OpaqueObjectDtorFn = void (*)(void*);

// static ui::ImageModel ui::ImageModel::FromImageSkia(const gfx::ImageSkia&);
using ImageModelFromImageSkiaFn = ImageModelOpaque* (*)(ImageModelOpaque* result, const void* imageSkia);

// BookmarkBarView::UpdateAppearanceForTheme() reconfigures all existing
// bookmark-bar buttons, including their folder ImageModels.
using BookmarkBarViewUpdateAppearanceForThemeFn = void (*)(void*);

using BookmarkBarViewDeletingDtorFn = void* (*)(void*, unsigned int);

// -----------------------------------------------------------------------------
// Resolved functions
// -----------------------------------------------------------------------------

static BookmarkButtonBaseCtorFn g_BookmarkButtonBaseCtorOriginal;
static BookmarkMenuButtonBaseCtorFn g_BookmarkMenuButtonBaseCtorOriginal;
static LabelButtonLabelCtorFn g_LabelButtonLabelCtorOriginal;
static LabelButtonLabelDeletingDtorFn g_LabelButtonLabelDeletingDtorOriginal;
static TypographyGetFontFn g_TypographyGetFontOriginal;
static LabelSetFontListFn g_LabelSetFontList;

static MenuItemGetFontListFn g_MenuItemGetFontListOriginal;
static MenuItemGetVerticalMarginFn g_MenuItemGetVerticalMarginOriginal;
static MenuConfigCornerRadiusForMenuFn g_MenuConfigCornerRadiusForMenuOriginal;
static FontListCopyCtorFn g_FontListCopyCtor;

static FontListGetFontSizeFn g_FontListGetFontSize;
static FontListDeriveWithSizeDeltaFn g_FontListDeriveWithSizeDelta;
static FontListDtorFn g_FontListDtor;

static OmniboxViewViewsCtorFn g_OmniboxViewViewsCtorOriginal;
static OmniboxViewViewsDtorFn g_OmniboxViewViewsDtorOriginal;
static TextfieldCtorFn g_TextfieldCtorOriginal;
static TextfieldSetFontListFn g_TextfieldSetFontListOriginal;

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

static GetBookmarkFolderIconFn g_GetBookmarkFolderIconOriginal;
static CreateSkBitmapFromHICONFn g_CreateSkBitmapFromHICON;
static ImageSkiaCreateFrom1xBitmapFn g_ImageSkiaCreateFrom1xBitmap;
static OpaqueObjectDtorFn g_SkBitmapDtor;
static OpaqueObjectDtorFn g_ImageSkiaDtor;
static ImageModelFromImageSkiaFn g_ImageModelFromImageSkia;
static BookmarkBarViewUpdateAppearanceForThemeFn g_BookmarkBarViewUpdateAppearanceForThemeOriginal;
static BookmarkBarViewDeletingDtorFn g_BookmarkBarViewDeletingDtorOriginal;

// -----------------------------------------------------------------------------
// State
// -----------------------------------------------------------------------------

static std::atomic_bool g_chromeSetupStarted = false;
static std::atomic_bool g_hooksActivated = false;
static std::atomic_bool g_symbolResolutionSucceeded = false;
// Once the bounded startup wait is abandoned, the resolver may still finish
// to populate Windhawk's symbol cache, but hooks must stay inactive in this
// Chrome instance. Late activation would miss constructor-tracked UI objects.
static std::atomic_bool g_hookActivationAbandoned = false;

// Worker creation and teardown are synchronized so Wh_ModBeforeUninit can't
// miss a thread that is being created concurrently from the chrome.dll load
// hook. g_unloading and g_chromeSetupInProgress are guarded by g_workerMutex.
static std::mutex g_workerMutex;
static std::condition_variable g_workerCondition;
static bool g_unloading = false;
static bool g_chromeSetupInProgress = false;
static HANDLE g_symbolResolutionThread;
static HANDLE g_symbolResolutionDoneEvent;
static HANDLE g_symbolNotificationThread;
static HANDLE g_symbolNotificationStopEvent;

static std::atomic_int g_bookmarkFontSize = -1;
static std::atomic_int g_addressBarFontSize = -1;
static std::atomic_int g_menuFontSize = -1;
static std::atomic_int g_tabFontSize = -1;

static std::atomic_int g_tabPreTitlePadding = kChromeDefaultTabPreTitlePadding;

static std::atomic_int g_extensionButtonWidth = kChromeDefaultExtensionButtonWidth;

// -1 = Chrome default.
static std::atomic_int g_menuVerticalSpacing = -1;
static std::atomic_int g_menuCornerRadius = -1;

static std::atomic_bool g_tabCloseButtonsHidden = false;
static std::atomic_bool g_useWindowsFolderIcon = false;

static std::atomic_bool g_addressBarFontHooksReady = false;
static std::atomic_bool g_tabFontHooksReady = false;
static std::atomic_bool g_tabCloseHooksReady = false;

static std::atomic_bool g_extensionTrackingReady = false;
static std::atomic_bool g_extensionContainerTrackingReady = false;
static std::atomic_bool g_windowsFolderReady = false;
static std::atomic_bool g_bookmarkFolderLiveUpdateReady = false;

static void ClearChromeRuntimeReadiness() {
  g_addressBarFontHooksReady.store(false, std::memory_order_release);
  g_tabFontHooksReady.store(false, std::memory_order_release);
  g_tabCloseHooksReady.store(false, std::memory_order_release);
  g_extensionTrackingReady.store(false, std::memory_order_release);
  g_extensionContainerTrackingReady.store(false, std::memory_order_release);
  g_bookmarkFolderLiveUpdateReady.store(false, std::memory_order_release);
}

static std::once_flag g_windowsFolderOnce;
static std::mutex g_windowsFolderImageMutex;
static DWORD g_windowsFolderThreadId = 0;

//  0 = not checked
//  1 = compatible
// -1 = incompatible
static std::atomic_int g_tabLayoutCompatibility = 0;

static thread_local int g_bookmarkCtorDepth = 0;
static thread_local int g_bookmarkMenuCtorDepth = 0;
static thread_local const FontListOpaque* g_pendingBookmarkOriginalFont = nullptr;
static thread_local std::vector<std::unique_ptr<OpaqueObjectStorage>>
    g_bookmarkCtorDerivedFonts;

static thread_local int g_tabTitleCtorDepth = 0;
static thread_local const FontListOpaque* g_pendingTabTitleOriginalFont = nullptr;

static thread_local int g_omniboxCtorDepth = 0;
static thread_local void* g_pendingOmniboxTextfield = nullptr;

struct BookmarkLabelInfo {
  DWORD threadId;
  std::unique_ptr<OpaqueObjectStorage> originalFontStorage;
};

static std::mutex g_labelsMutex;
static std::unordered_map<void*, BookmarkLabelInfo> g_labels;

static std::mutex g_bookmarkBarsMutex;
static std::unordered_map<void*, DWORD> g_bookmarkBars;

struct OmniboxInfo {
  DWORD threadId;
  void* textfield;
  std::unique_ptr<OpaqueObjectStorage> originalFontStorage;
};

static std::mutex g_omniboxesMutex;
static std::unordered_map<void*, OmniboxInfo> g_omniboxes;

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
// Settings
// -----------------------------------------------------------------------------

// Windhawk settings metadata is static, while Chrome's actual default font
// sizes are only known after the native UI objects exist. If Windhawk gains a
// runtime settings-metadata/read-only-value API, the "Chrome default" labels
// could show the detected size instead of the static "usually" hint below.
// Numeric options are absolute target sizes; -1 means restore Chrome's exact
// original FontList for that object.
static int ReadFontSizeSetting(PCWSTR name) {
  auto value = WindhawkUtils::StringSetting::make(name);

  if (!*value.get() || wcscmp(value.get(), L"default") == 0) return -1;

  int result = _wtoi(value.get());

  return result >= 12 && result <= 24 ? result : -1;
}

static bool ReadWindowsFolderIconSetting() {
  auto value = WindhawkUtils::StringSetting::make(L"bookmarks.folderIcon");

  return *value.get() && wcscmp(value.get(), L"windows") == 0;
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

  int result = _wtoi(value.get());

  return result >= 0 && result <= 12 ? result : -1;
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

static void LoadSettings() {
  int bookmarkFontSize = ReadFontSizeSetting(L"bookmarks.fontSize");

  bool useWindowsFolderIcon = ReadWindowsFolderIconSetting();

  int addressBarFontSize = ReadFontSizeSetting(L"addressBar.fontSize");

  int menuFontSize = ReadFontSizeSetting(L"menus.fontSize");

  int menuVerticalSpacing = ReadMenuSpacingSetting();

  int menuCornerRadius = ReadMenuCornerRadiusSetting();

  int tabFontSize = ReadFontSizeSetting(L"tabs.fontSize");

  bool tabCloseButtonsHidden = Wh_GetIntSetting(L"tabs.hideCloseButtons") != 0;

  int tabPreTitlePadding = ReadTabPreTitlePaddingSetting();

  int extensionButtonWidth = ReadExtensionButtonWidthSetting();

  g_bookmarkFontSize.store(bookmarkFontSize, std::memory_order_relaxed);

  g_useWindowsFolderIcon.store(useWindowsFolderIcon, std::memory_order_relaxed);

  g_addressBarFontSize.store(addressBarFontSize, std::memory_order_relaxed);

  g_menuFontSize.store(menuFontSize, std::memory_order_relaxed);

  g_menuVerticalSpacing.store(menuVerticalSpacing, std::memory_order_relaxed);

  g_menuCornerRadius.store(menuCornerRadius, std::memory_order_relaxed);

  g_tabFontSize.store(tabFontSize, std::memory_order_relaxed);

  g_tabCloseButtonsHidden.store(tabCloseButtonsHidden, std::memory_order_relaxed);

  g_tabPreTitlePadding.store(tabPreTitlePadding, std::memory_order_relaxed);

  g_extensionButtonWidth.store(extensionButtonWidth, std::memory_order_relaxed);

  if (bookmarkFontSize < 0) {
    Wh_Log(L"Bookmark bar font size: Chrome default");
  } else {
    Wh_Log(L"Bookmark bar font size: %d", bookmarkFontSize);
  }

  Wh_Log(L"Bookmark folder icon: %ls", useWindowsFolderIcon ? L"Windows system" : L"Chrome default");

  if (addressBarFontSize < 0) {
    Wh_Log(L"Address bar font size: Chrome default");
  } else {
    Wh_Log(L"Address bar font size: %d", addressBarFontSize);
  }

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
    Wh_Log(L"Menu corner radius: Chrome default");
  } else {
    Wh_Log(L"Menu corner radius: %d", menuCornerRadius);
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

static void ReleaseBookmarkCtorDerivedFonts() {
  for (auto& storage : g_bookmarkCtorDerivedFonts) {
    DestroyOwnedFontListStorage(std::move(storage), L"Bookmark constructor FontList");
  }

  g_bookmarkCtorDerivedFonts.clear();
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

// -----------------------------------------------------------------------------
// Address bar
// -----------------------------------------------------------------------------

static const FontListOpaque* GetOmniboxOriginalFont(const OmniboxInfo& info) {
  return GetOwnedFontList(info.originalFontStorage);
}

static bool ApplyAddressBarFont(void* textfield, const FontListOpaque* originalFont) {
  if (!textfield || !originalFont || !g_TextfieldSetFontListOriginal) return false;

  int targetSize = g_addressBarFontSize.load(std::memory_order_relaxed);

  if (targetSize < 0) {
    g_TextfieldSetFontListOriginal(textfield, originalFont);
    return true;
  }

  if (!g_FontListGetFontSize || !g_FontListDeriveWithSizeDelta || !g_FontListDtor) return false;

  int originalSize = g_FontListGetFontSize(originalFont);

  if (targetSize == originalSize) {
    g_TextfieldSetFontListOriginal(textfield, originalFont);
    return true;
  }

  auto derivedFontStorage =
      DeriveFontListToOwnedStorage(originalFont, targetSize, L"Address bar");

  if (!derivedFontStorage) return false;

  g_TextfieldSetFontListOriginal(textfield, GetOwnedFontList(derivedFontStorage));
  DestroyOwnedFontListStorage(std::move(derivedFontStorage), L"Address bar");
  return true;
}

// OmniboxViewViews reaches views::Textfield through a multiple-inheritance
// mixin, so the complete OmniboxViewViews pointer isn't necessarily a valid
// Textfield pointer. Capture Chromium's exact Textfield subobject pointer while
// the OmniboxViewViews constructor is constructing its bases. All unrelated
// Textfield constructors pass through unchanged.
static void TextfieldCtorHook(void* self) {
  g_TextfieldCtorOriginal(self);

  if (g_omniboxCtorDepth > 0 && !g_pendingOmniboxTextfield) {
    g_pendingOmniboxTextfield = self;
  }
}

static void OmniboxViewViewsCtorHook(void* self,
                                     bool popupWindowMode,
                                     void* controller,
                                     void* locationBarView,
                                     const FontListOpaque* fontList) {
  void* previousPendingTextfield = g_pendingOmniboxTextfield;
  g_pendingOmniboxTextfield = nullptr;
  g_omniboxCtorDepth++;

  int originalSize = fontList && g_FontListGetFontSize ? g_FontListGetFontSize(fontList) : -1;

  const FontListOpaque* constructorFont = fontList;
  std::unique_ptr<OpaqueObjectStorage> derivedFontStorage;

  int targetSize = g_addressBarFontSize.load(std::memory_order_relaxed);

  if (targetSize >= 0 && fontList && originalSize >= 0 && targetSize != originalSize) {
    derivedFontStorage = DeriveFontListToOwnedStorage(
        fontList, targetSize, L"Address bar constructor");

    if (derivedFontStorage) {
      constructorFont = GetOwnedFontList(derivedFontStorage);
    }
  }

  g_OmniboxViewViewsCtorOriginal(self, popupWindowMode, controller, locationBarView,
                                 constructorFont);

  g_omniboxCtorDepth--;

  DestroyOwnedFontListStorage(std::move(derivedFontStorage),
                              L"Address bar constructor");

  void* textfield = g_pendingOmniboxTextfield;
  g_pendingOmniboxTextfield = previousPendingTextfield;

  if (!textfield || !fontList) {
    Wh_Log(L"Address bar live tracking unavailable for this omnibox");
    return;
  }

  auto originalFontStorage =
      CopyFontListToOwnedStorage(fontList, L"Address bar original FontList");

  if (!originalFontStorage) {
    Wh_Log(L"Address bar original font capture failed; live restore unavailable for this omnibox");
    return;
  }

  std::lock_guard<std::mutex> lock(g_omniboxesMutex);
  g_omniboxes[self] = {GetCurrentThreadId(), textfield, std::move(originalFontStorage)};
}

static void OmniboxViewViewsDtorHook(void* self) {
  std::unique_ptr<OpaqueObjectStorage> originalFontStorage;

  {
    std::lock_guard<std::mutex> lock(g_omniboxesMutex);
    auto it = g_omniboxes.find(self);

    if (it != g_omniboxes.end()) {
      originalFontStorage = std::move(it->second.originalFontStorage);
      g_omniboxes.erase(it);
    }
  }

  DestroyOwnedFontListStorage(std::move(originalFontStorage),
                              L"Address bar original FontList");

  g_OmniboxViewViewsDtorOriginal(self);
}

// -----------------------------------------------------------------------------
// Bookmark bar
// -----------------------------------------------------------------------------

static void BookmarkButtonBaseCtorHook(void* self, void* pressedCallback, const void* title) {
  bool outermostBookmarkCtor = g_bookmarkCtorDepth == 0 && g_bookmarkMenuCtorDepth == 0;
  g_bookmarkCtorDepth++;

  g_BookmarkButtonBaseCtorOriginal(self, pressedCallback, title);

  g_bookmarkCtorDepth--;

  if (outermostBookmarkCtor) ReleaseBookmarkCtorDerivedFonts();
}

static void BookmarkMenuButtonBaseCtorHook(void* self, void* pressedCallback, void* showMenuCallback,
                                           const void* title) {
  bool outermostBookmarkCtor = g_bookmarkCtorDepth == 0 && g_bookmarkMenuCtorDepth == 0;
  g_bookmarkMenuCtorDepth++;

  g_BookmarkMenuButtonBaseCtorOriginal(self, pressedCallback, showMenuCallback, title);

  g_bookmarkMenuCtorDepth--;

  if (outermostBookmarkCtor) ReleaseBookmarkCtorDerivedFonts();
}

static void LabelButtonLabelCtorHook(void* self, const void* text, int textContext) {
  bool isBookmarkLabel = g_bookmarkCtorDepth > 0 || g_bookmarkMenuCtorDepth > 0;

  const FontListOpaque* previousPendingFont = g_pendingBookmarkOriginalFont;
  g_pendingBookmarkOriginalFont = nullptr;

  g_LabelButtonLabelCtorOriginal(self, text, textContext);

  const FontListOpaque* originalFont = g_pendingBookmarkOriginalFont;
  g_pendingBookmarkOriginalFont = previousPendingFont;

  if (!isBookmarkLabel) return;

  auto originalFontStorage =
      CopyFontListToOwnedStorage(originalFont, L"Bookmark original FontList");

  if (!originalFontStorage) {
    Wh_Log(L"Bookmark original font capture failed; live restore unavailable for this label");

    if (originalFont && g_LabelSetFontList) {
      g_LabelSetFontList(self, originalFont);
    }

    return;
  }

  const FontListOpaque* ownedOriginalFont = GetOwnedFontList(originalFontStorage);
  int targetSize = g_bookmarkFontSize.load(std::memory_order_relaxed);

  if (targetSize >= 0 &&
      !SetLabelFontForTargetSize(self, ownedOriginalFont, targetSize, L"Bookmark")) {
    Wh_Log(L"Bookmark exact font sizing unavailable; leaving Chrome's original font");
  }

  std::lock_guard<std::mutex> lock(g_labelsMutex);
  g_labels.insert_or_assign(
      self, BookmarkLabelInfo{GetCurrentThreadId(), std::move(originalFontStorage)});
}

static void* LabelButtonLabelDeletingDtorHook(void* self, unsigned int flags) {
  std::unique_ptr<OpaqueObjectStorage> originalFontStorage;

  {
    std::lock_guard<std::mutex> lock(g_labelsMutex);
    auto it = g_labels.find(self);

    if (it != g_labels.end()) {
      originalFontStorage = std::move(it->second.originalFontStorage);
      g_labels.erase(it);
    }
  }

  DestroyOwnedFontListStorage(std::move(originalFontStorage),
                              L"Bookmark original FontList");

  return g_LabelButtonLabelDeletingDtorOriginal(self, flags);
}

static const FontListOpaque* TypographyGetFontHook(const void* self, int context, int style) {
  const FontListOpaque* originalFont = g_TypographyGetFontOriginal(self, context, style);

  if (g_tabTitleCtorDepth > 0 && !g_pendingTabTitleOriginalFont) {
    g_pendingTabTitleOriginalFont = originalFont;
  }

  if (g_bookmarkCtorDepth <= 0 && g_bookmarkMenuCtorDepth <= 0) {
    return originalFont;
  }

  if (!g_pendingBookmarkOriginalFont) {
    g_pendingBookmarkOriginalFont = originalFont;
  }

  int targetSize = g_bookmarkFontSize.load(std::memory_order_relaxed);

  if (targetSize < 0 || !g_FontListCopyCtor ||
      !g_FontListGetFontSize || !g_FontListDeriveWithSizeDelta || !g_FontListDtor ||
      targetSize == g_FontListGetFontSize(originalFont)) {
    return originalFont;
  }

  auto derivedFontStorage = DeriveFontListToOwnedStorage(
      originalFont, targetSize, L"Bookmark constructor");

  if (!derivedFontStorage) return originalFont;

  const FontListOpaque* derivedFont = GetOwnedFontList(derivedFontStorage);
  g_bookmarkCtorDerivedFonts.push_back(std::move(derivedFontStorage));
  return derivedFont;
}

// -----------------------------------------------------------------------------
// Bookmark folder icon
// -----------------------------------------------------------------------------

static void CreateWindowsFolderImage() {
  std::lock_guard<std::mutex> lock(g_windowsFolderImageMutex);

  if (!g_CreateSkBitmapFromHICON || !g_ImageSkiaCreateFrom1xBitmap || !g_SkBitmapDtor || !g_ImageSkiaDtor ||
      !g_ImageModelFromImageSkia) {
    Wh_Log(L"Windows bookmark folder image helpers unavailable");
    return;
  }

  SHSTOCKICONINFO iconInfo = {};
  iconInfo.cbSize = sizeof(iconInfo);

  HRESULT hr = SHGetStockIconInfo(SIID_FOLDER, SHGSI_ICON | SHGSI_SMALLICON, &iconInfo);

  if (FAILED(hr) || !iconInfo.hIcon) {
    Wh_Log(L"SHGetStockIconInfo(SIID_FOLDER) failed: 0x%08X", static_cast<unsigned int>(hr));
    return;
  }

  PrepareOpaqueObjectStorage(g_skBitmapStorage);
  g_CreateSkBitmapFromHICON(g_skBitmapStorage.data, iconInfo.hIcon);
  DestroyIcon(iconInfo.hIcon);

  if (!IsOpaqueObjectGuardIntact(g_skBitmapStorage)) {
    Wh_Log(L"ERROR: SkBitmap exceeded reserved opaque storage; skipping destructor");
    return;
  }

  PrepareOpaqueObjectStorage(g_imageSkiaStorage);
  g_ImageSkiaCreateFrom1xBitmap(g_imageSkiaStorage.data, g_skBitmapStorage.data);
  g_SkBitmapDtor(g_skBitmapStorage.data);

  if (!IsOpaqueObjectGuardIntact(g_imageSkiaStorage)) {
    Wh_Log(L"ERROR: gfx::ImageSkia exceeded reserved opaque storage");
    return;
  }

  g_windowsFolderThreadId = GetCurrentThreadId();
  g_windowsFolderReady.store(true, std::memory_order_release);

  Wh_Log(L"Windows bookmark folder image: ready on UI thread %lu", g_windowsFolderThreadId);
}

static void DestroyWindowsFolderImageOnCurrentThread() {
  std::lock_guard<std::mutex> lock(g_windowsFolderImageMutex);

  if (!g_windowsFolderReady.load(std::memory_order_acquire)) return;

  DWORD threadId = GetCurrentThreadId();

  if (g_windowsFolderThreadId != threadId) {
    Wh_Log(L"Windows bookmark folder image cleanup skipped on non-owning thread %lu (owner %lu)",
           threadId, g_windowsFolderThreadId);
    return;
  }

  g_windowsFolderReady.store(false, std::memory_order_release);
  g_ImageSkiaDtor(g_imageSkiaStorage.data);
  g_windowsFolderThreadId = 0;

  Wh_Log(L"Windows bookmark folder image: destroyed on UI thread %lu", threadId);
}

static ImageModelOpaque* GetBookmarkFolderIconHook(ImageModelOpaque* result, int iconType,
                                                   uintptr_t colorVariantOpaque) {
  constexpr int kNormal = 0;

  if (iconType != kNormal || !g_useWindowsFolderIcon.load(std::memory_order_relaxed)) {
    return g_GetBookmarkFolderIconOriginal(result, iconType, colorVariantOpaque);
  }

  std::call_once(g_windowsFolderOnce, CreateWindowsFolderImage);

  std::lock_guard<std::mutex> lock(g_windowsFolderImageMutex);

  if (!g_useWindowsFolderIcon.load(std::memory_order_relaxed) ||
      !g_windowsFolderReady.load(std::memory_order_acquire)) {
    return g_GetBookmarkFolderIconOriginal(result, iconType, colorVariantOpaque);
  }

  return g_ImageModelFromImageSkia(result, g_imageSkiaStorage.data);
}

static void BookmarkBarViewUpdateAppearanceForThemeHook(void* self) {
  g_BookmarkBarViewUpdateAppearanceForThemeOriginal(self);

  if (!g_bookmarkFolderLiveUpdateReady.load(std::memory_order_relaxed)) return;

  std::lock_guard<std::mutex> lock(g_bookmarkBarsMutex);
  g_bookmarkBars[self] = GetCurrentThreadId();
}

static void* BookmarkBarViewDeletingDtorHook(void* self, unsigned int flags) {
  {
    std::lock_guard<std::mutex> lock(g_bookmarkBarsMutex);
    g_bookmarkBars.erase(self);
  }

  return g_BookmarkBarViewDeletingDtorOriginal(self, flags);
}

// -----------------------------------------------------------------------------
// Menus
// -----------------------------------------------------------------------------

static FontListOpaque* MenuItemGetFontListHook(const void* self, FontListOpaque* result) {
  int targetSize = g_menuFontSize.load(std::memory_order_relaxed);

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

static int MenuItemGetVerticalMarginHook(const void* self) {
  int originalMargin = g_MenuItemGetVerticalMarginOriginal(self);

  int configuredMargin = g_menuVerticalSpacing.load(std::memory_order_relaxed);

  if (configuredMargin < 0) return originalMargin;

  return std::min(originalMargin, configuredMargin);
}

static int MenuConfigCornerRadiusForMenuHook(const void* self, const void* controller) {
  int originalRadius = g_MenuConfigCornerRadiusForMenuOriginal(self, controller);

  int configuredRadius = g_menuCornerRadius.load(std::memory_order_relaxed);

  if (configuredRadius < 0 || originalRadius == 0) {
    return originalRadius;
  }

  return configuredRadius;
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

static void WINAPI DestroyWindowsFolderImageOnCurrentThreadProc(void*) {
  DestroyWindowsFolderImageOnCurrentThread();
}

static void DestroyWindowsFolderImageOnOwningThread() {
  DWORD threadId = 0;

  {
    std::lock_guard<std::mutex> lock(g_windowsFolderImageMutex);

    if (!g_windowsFolderReady.load(std::memory_order_acquire)) return;

    threadId = g_windowsFolderThreadId;
  }

  if (!threadId) return;

  HWND hwnd = FindWindowForThread(threadId);

  if (!hwnd) {
    Wh_Log(L"Windows bookmark folder image cleanup: no window found for owning UI thread %lu; leaving it alive",
           threadId);
    return;
  }

  if (!RunFromWindowThread(hwnd, DestroyWindowsFolderImageOnCurrentThreadProc, nullptr)) {
    Wh_Log(L"Windows bookmark folder image cleanup: failed to dispatch to owning UI thread %lu; leaving it alive",
           threadId);
  }
}

// -----------------------------------------------------------------------------
// Live address-bar update
// -----------------------------------------------------------------------------

struct AddressBarApplyParams {
  bool teardown;
};

static void WINAPI ApplyAddressBarFontOnCurrentThread(void* parameter) {
  const auto* params = static_cast<const AddressBarApplyParams*>(parameter);
  bool teardown = params && params->teardown;

  if (!teardown && !g_addressBarFontHooksReady.load(std::memory_order_relaxed)) return;

  DWORD threadId = GetCurrentThreadId();

  if (!teardown) {
    std::vector<std::pair<void*, const FontListOpaque*>> omniboxes;

    {
      std::lock_guard<std::mutex> lock(g_omniboxesMutex);

      for (const auto& [omnibox, info] : g_omniboxes) {
        if (info.threadId == threadId) {
          omniboxes.push_back({info.textfield, GetOmniboxOriginalFont(info)});
        }
      }
    }

    for (const auto& [textfield, originalFont] : omniboxes) {
      ApplyAddressBarFont(textfield, originalFont);
    }

    return;
  }

  struct TeardownEntry {
    void* textfield;
    std::unique_ptr<OpaqueObjectStorage> originalFontStorage;
  };

  std::vector<TeardownEntry> omniboxes;

  {
    std::lock_guard<std::mutex> lock(g_omniboxesMutex);

    for (auto it = g_omniboxes.begin(); it != g_omniboxes.end();) {
      if (it->second.threadId == threadId) {
        omniboxes.push_back({it->second.textfield, std::move(it->second.originalFontStorage)});
        it = g_omniboxes.erase(it);
      } else {
        ++it;
      }
    }
  }

  for (auto& entry : omniboxes) {
    const FontListOpaque* originalFont = GetOwnedFontList(entry.originalFontStorage);

    if (originalFont) {
      ApplyAddressBarFont(entry.textfield, originalFont);
    }

    DestroyOwnedFontListStorage(std::move(entry.originalFontStorage),
                                L"Address bar original FontList");
  }

  Wh_Log(L"Restored and released %llu address bar fonts on UI thread %lu",
         static_cast<unsigned long long>(omniboxes.size()), threadId);
}

static void ApplyFontToExistingAddressBars(bool teardown = false) {
  if (!teardown && !g_addressBarFontHooksReady.load(std::memory_order_relaxed)) return;

  std::vector<DWORD> threadIds;

  {
    std::lock_guard<std::mutex> lock(g_omniboxesMutex);

    for (const auto& [omnibox, info] : g_omniboxes) {
      threadIds.push_back(info.threadId);
    }
  }

  std::sort(threadIds.begin(), threadIds.end());
  threadIds.erase(std::unique(threadIds.begin(), threadIds.end()), threadIds.end());

  AddressBarApplyParams params{teardown};

  for (DWORD threadId : threadIds) {
    HWND hwnd = FindWindowForThread(threadId);

    if (!hwnd) {
      Wh_Log(L"Address bar %ls: no window found for UI thread %lu",
             teardown ? L"teardown" : L"live update", threadId);
      continue;
    }

    if (!RunFromWindowThread(hwnd, ApplyAddressBarFontOnCurrentThread, &params)) {
      Wh_Log(L"Address bar %ls: failed to dispatch to UI thread %lu",
             teardown ? L"teardown" : L"live update", threadId);
    }
  }
}

// -----------------------------------------------------------------------------
// Live bookmark update
// -----------------------------------------------------------------------------

struct BookmarkFontApplyParams {
  bool teardown;
};

static void WINAPI ApplyBookmarkFontOnCurrentThread(void* parameter) {
  if (!g_LabelSetFontList) return;

  const auto* params = static_cast<const BookmarkFontApplyParams*>(parameter);
  bool teardown = params && params->teardown;
  DWORD threadId = GetCurrentThreadId();

  if (!teardown) {
    int targetSize = g_bookmarkFontSize.load(std::memory_order_relaxed);
    std::vector<std::pair<void*, const FontListOpaque*>> labels;

    {
      std::lock_guard<std::mutex> lock(g_labelsMutex);

      for (const auto& [label, info] : g_labels) {
        if (info.threadId == threadId) {
          labels.push_back({label, GetOwnedFontList(info.originalFontStorage)});
        }
      }
    }

    for (const auto& [label, originalFont] : labels) {
      SetLabelFontForTargetSize(label, originalFont, targetSize, L"Bookmark");
    }

    return;
  }

  struct TeardownEntry {
    void* label;
    std::unique_ptr<OpaqueObjectStorage> originalFontStorage;
  };

  std::vector<TeardownEntry> labels;

  {
    std::lock_guard<std::mutex> lock(g_labelsMutex);

    for (auto it = g_labels.begin(); it != g_labels.end();) {
      if (it->second.threadId == threadId) {
        labels.push_back({it->first, std::move(it->second.originalFontStorage)});
        it = g_labels.erase(it);
      } else {
        ++it;
      }
    }
  }

  for (auto& entry : labels) {
    const FontListOpaque* originalFont = GetOwnedFontList(entry.originalFontStorage);

    if (originalFont) {
      g_LabelSetFontList(entry.label, originalFont);
    }

    DestroyOwnedFontListStorage(std::move(entry.originalFontStorage),
                                L"Bookmark original FontList");
  }

  Wh_Log(L"Restored and released %llu bookmark fonts on UI thread %lu",
         static_cast<unsigned long long>(labels.size()), threadId);
}

static void ApplyFontToExistingBookmarkLabels(bool teardown = false) {
  std::vector<DWORD> threadIds;

  {
    std::lock_guard<std::mutex> lock(g_labelsMutex);

    for (const auto& [label, info] : g_labels) {
      threadIds.push_back(info.threadId);
    }
  }

  std::sort(threadIds.begin(), threadIds.end());

  threadIds.erase(std::unique(threadIds.begin(), threadIds.end()), threadIds.end());

  BookmarkFontApplyParams params{teardown};

  for (DWORD threadId : threadIds) {
    HWND hwnd = FindWindowForThread(threadId);

    if (!hwnd) {
      Wh_Log(L"Bookmark font %ls: no window found for UI thread %lu",
             teardown ? L"teardown" : L"live update", threadId);
      continue;
    }

    if (!RunFromWindowThread(hwnd, ApplyBookmarkFontOnCurrentThread, &params)) {
      Wh_Log(L"Bookmark font %ls: failed to dispatch to UI thread %lu",
             teardown ? L"teardown" : L"live update", threadId);
    }
  }
}

static void WINAPI ApplyBookmarkFolderIconOnCurrentThread(void*) {
  if (!g_BookmarkBarViewUpdateAppearanceForThemeOriginal) return;

  DWORD threadId = GetCurrentThreadId();
  std::vector<void*> bookmarkBars;

  {
    std::lock_guard<std::mutex> lock(g_bookmarkBarsMutex);

    for (const auto& [bookmarkBar, bookmarkBarThreadId] : g_bookmarkBars) {
      if (bookmarkBarThreadId == threadId) bookmarkBars.push_back(bookmarkBar);
    }
  }

  for (void* bookmarkBar : bookmarkBars) {
    g_BookmarkBarViewUpdateAppearanceForThemeOriginal(bookmarkBar);
  }

  Wh_Log(L"Updated %llu bookmark bars on UI thread %lu",
         static_cast<unsigned long long>(bookmarkBars.size()), threadId);
}

static void ApplyFolderIconToExistingBookmarkBars() {
  if (!g_bookmarkFolderLiveUpdateReady.load(std::memory_order_relaxed)) return;

  std::vector<DWORD> threadIds;

  {
    std::lock_guard<std::mutex> lock(g_bookmarkBarsMutex);

    for (const auto& [bookmarkBar, threadId] : g_bookmarkBars) {
      threadIds.push_back(threadId);
    }
  }

  std::sort(threadIds.begin(), threadIds.end());
  threadIds.erase(std::unique(threadIds.begin(), threadIds.end()), threadIds.end());

  for (DWORD threadId : threadIds) {
    HWND hwnd = FindWindowForThread(threadId);

    if (hwnd) {
      RunFromWindowThread(hwnd, ApplyBookmarkFolderIconOnCurrentThread, nullptr);
    }
  }
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

    // A close-button-only symbol set still needs a live relayout path even if
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
    // symbols were available. Don't depend on runtime ready flags that a
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

  // Rebuild icon/badge image if this symbol is available,
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
// Resolve all chrome.dll symbols in ONE pass
// -----------------------------------------------------------------------------

static bool InstallChromeHooks(HMODULE chromeDll) {
  wchar_t path[32768] = {};

  if (GetModuleFileNameW(chromeDll, path, ARRAYSIZE(path))) {
    Wh_Log(L"Resolving symbols for: %ls", path);
  }

  WindhawkUtils::SYMBOL_HOOK chromeDllHooks[] = {

      // -----------------------------------------------------------------------
      // Bookmark/core: REQUIRED
      // -----------------------------------------------------------------------

      {{LR"(??0BookmarkButtonBase@@QEAA@VPressedCallback@Button@views@@V?$basic_string_view@_SU?$char_traits@_S@__Cr@std@@@__Cr@std@@@Z)"},
       &g_BookmarkButtonBaseCtorOriginal,
       BookmarkButtonBaseCtorHook,
       false},

      {{LR"(??0BookmarkMenuButtonBase@@QEAA@VPressedCallback@Button@views@@V?$RepeatingCallback@$$A6A_NXZ@base@@V?$basic_string_view@_SU?$char_traits@_S@__Cr@std@@@__Cr@std@@@Z)"},
       &g_BookmarkMenuButtonBaseCtorOriginal,
       BookmarkMenuButtonBaseCtorHook,
       false},

      {{LR"(??0LabelButtonLabel@internal@views@@QEAA@V?$basic_string_view@_SU?$char_traits@_S@__Cr@std@@@__Cr@std@@H@Z)"},
       &g_LabelButtonLabelCtorOriginal,
       LabelButtonLabelCtorHook,
       false},

      {{LR"(??_GLabelButtonLabel@internal@views@@UEAAPEAXI@Z)"},
       &g_LabelButtonLabelDeletingDtorOriginal,
       LabelButtonLabelDeletingDtorHook,
       false},

      {{LR"(?GetFont@TypographyProvider@views@@QEBAAEBVFontList@gfx@@HH@Z)"},
       &g_TypographyGetFontOriginal,
       TypographyGetFontHook,
       false},

      {{LR"(?SetFontList@Label@views@@UEAAXAEBVFontList@gfx@@@Z)"}, &g_LabelSetFontList, nullptr, false},

      // -----------------------------------------------------------------------
      // Bookmark folder icon: OPTIONAL
      // -----------------------------------------------------------------------

      {{LR"(?GetBookmarkFolderIcon@chrome@@YA?AVImageModel@ui@@W4BookmarkFolderIconType@1@VColorVariant@3@@Z)"},
       &g_GetBookmarkFolderIconOriginal,
       GetBookmarkFolderIconHook,
       true},

      {{LR"(?CreateSkBitmapFromHICON@IconUtil@@SA?AVSkBitmap@@PEAUHICON__@@@Z)"},
       &g_CreateSkBitmapFromHICON,
       nullptr,
       true},

      {{LR"(?CreateFrom1xBitmap@ImageSkia@gfx@@SA?AV12@AEBVSkBitmap@@@Z)"},
       &g_ImageSkiaCreateFrom1xBitmap,
       nullptr,
       true},

      {{LR"(??1SkBitmap@@QEAA@XZ)"}, &g_SkBitmapDtor, nullptr, true},

      {{LR"(??1ImageSkia@gfx@@QEAA@XZ)"}, &g_ImageSkiaDtor, nullptr, true},

      {{LR"(?FromImageSkia@ImageModel@ui@@SA?AV12@AEBVImageSkia@gfx@@@Z)", L"ui::ImageModel::FromImageSkia"},
       &g_ImageModelFromImageSkia,
       nullptr,
       true},

      {{L"BookmarkBarView::UpdateAppearanceForTheme",
        LR"(?UpdateAppearanceForTheme@BookmarkBarView@@AEAAXXZ)"},
       &g_BookmarkBarViewUpdateAppearanceForThemeOriginal,
       BookmarkBarViewUpdateAppearanceForThemeHook,
       true},

      {{LR"(??_GBookmarkBarView@@UEAAPEAXI@Z)"},
       &g_BookmarkBarViewDeletingDtorOriginal,
       BookmarkBarViewDeletingDtorHook,
       true},

      // -----------------------------------------------------------------------
      // Menus: OPTIONAL
      // -----------------------------------------------------------------------

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

      {{LR"(?CornerRadiusForMenu@MenuConfig@views@@QEBAHPEBVMenuController@2@@Z)",
        L"views::MenuConfig::CornerRadiusForMenu"},
       &g_MenuConfigCornerRadiusForMenuOriginal,
       MenuConfigCornerRadiusForMenuHook,
       true},

      {{LR"(??0FontList@gfx@@QEAA@AEBV01@@Z)"}, &g_FontListCopyCtor, nullptr, true},

      // -----------------------------------------------------------------------
      // Address bar: OPTIONAL
      // -----------------------------------------------------------------------

      {{LR"(??0OmniboxViewViews@@QEAA@_NPEAVOmniboxController@@PEAVLocationBarView@@AEBVFontList@gfx@@@Z)"},
       &g_OmniboxViewViewsCtorOriginal,
       OmniboxViewViewsCtorHook,
       true},

      {{LR"(??1OmniboxViewViews@@UEAA@XZ)"},
       &g_OmniboxViewViewsDtorOriginal,
       OmniboxViewViewsDtorHook,
       true},

      {{LR"(??0Textfield@views@@QEAA@XZ)"},
       &g_TextfieldCtorOriginal,
       TextfieldCtorHook,
       true},

      {{LR"(?SetFontList@Textfield@views@@QEAAXAEBVFontList@gfx@@@Z)"},
       &g_TextfieldSetFontListOriginal,
       nullptr,
       true},

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

  WH_HOOK_SYMBOLS_OPTIONS options = {};

  options.optionsSize = sizeof(options);

  options.symbolServer = kChromeSymbolServer;

  options.noUndecoratedSymbols = TRUE;

  if (!WindhawkUtils::HookSymbols(chromeDll, chromeDllHooks, ARRAYSIZE(chromeDllHooks), &options)) {
    Wh_Log(L"ERROR: Failed to resolve required Chrome symbols");

    return false;
  }

  bool folderIconReady = g_GetBookmarkFolderIconOriginal && g_CreateSkBitmapFromHICON &&
                         g_ImageSkiaCreateFrom1xBitmap && g_SkBitmapDtor && g_ImageSkiaDtor &&
                         g_ImageModelFromImageSkia;

  bool folderIconLiveUpdateReady = folderIconReady && g_BookmarkBarViewUpdateAppearanceForThemeOriginal &&
                                   g_BookmarkBarViewDeletingDtorOriginal;

  bool addressBarFontReady =
      g_OmniboxViewViewsCtorOriginal && g_OmniboxViewViewsDtorOriginal &&
      g_TextfieldCtorOriginal && g_TextfieldSetFontListOriginal && g_FontListCopyCtor &&
      g_FontListGetFontSize && g_FontListDeriveWithSizeDelta && g_FontListDtor;

  bool tabFontReady =
      g_TabTitleCtorOriginal && g_TabTitleDtorOriginal && g_LabelSetFontList &&
      g_FontListCopyCtor && g_FontListGetFontSize && g_FontListDeriveWithSizeDelta &&
      g_FontListDtor;

  bool tabCloseReady = g_TabCloseButtonCtorOriginal && g_TabCloseButtonDtorOriginal && g_ViewSetVisibleOriginal &&
                       g_ViewInvalidateLayout && g_GetLayoutConstantOriginal;

  bool extensionWidthReady = g_ToolbarActionViewCalculatePreferredSizeOriginal;

  Wh_Log(L"Bookmark exact font sizing/restore: %ls",
         g_LabelSetFontList && g_FontListCopyCtor && g_FontListGetFontSize &&
                 g_FontListDeriveWithSizeDelta && g_FontListDtor
             ? L"ready"
             : L"unavailable");
  Wh_Log(L"Bookmark folder icon tweak: %ls", folderIconReady ? L"ready" : L"unavailable");
  Wh_Log(L"Bookmark folder icon cleanup symbols: skBitmapDtor=%ls imageSkiaDtor=%ls",
         g_SkBitmapDtor ? L"ready" : L"MISSING", g_ImageSkiaDtor ? L"ready" : L"MISSING");
  Wh_Log(L"Bookmark folder icon live update: %ls", folderIconLiveUpdateReady ? L"ready" : L"unavailable");

  Wh_Log(L"Menu tweaks: fontList=%ls exactFontSizing=%ls verticalMargin=%ls cornerRadius=%ls",
         g_MenuItemGetFontListOriginal ? L"ready" : L"MISSING",
         g_FontListCopyCtor && g_FontListGetFontSize && g_FontListDeriveWithSizeDelta &&
                 g_FontListDtor
             ? L"ready"
             : L"unavailable",
         g_MenuItemGetVerticalMarginOriginal ? L"ready" : L"MISSING",
         g_MenuConfigCornerRadiusForMenuOriginal ? L"ready" : L"MISSING");

  Wh_Log(
      L"Extension live symbols: actionCtor=%ls actionDeletingDtor=%ls "
      L"preferredSizeChanged=%ls",
      g_ToolbarActionViewCtorOriginal ? L"ready" : L"MISSING",
      g_ToolbarActionViewDeletingDtorOriginal ? L"ready" : L"MISSING",
      g_ViewPreferredSizeChanged ? L"ready" : L"MISSING");

  Wh_Log(L"Extension container symbols: ctor=%ls deletingDtor=%ls",
         g_ExtensionsToolbarDesktopCtorOriginal ? L"ready" : L"MISSING",
         g_ExtensionsToolbarDesktopDeletingDtorOriginal ? L"ready" : L"MISSING");

  bool extensionTrackingReady = extensionWidthReady && g_ToolbarActionViewCtorOriginal &&
                                g_ToolbarActionViewDeletingDtorOriginal && g_ViewPreferredSizeChanged;

  bool extensionContainerTrackingReady = g_ExtensionsToolbarDesktopCtorOriginal &&
                                         g_ExtensionsToolbarDesktopDeletingDtorOriginal && g_ViewPreferredSizeChanged;

  g_addressBarFontHooksReady.store(addressBarFontReady, std::memory_order_release);

  g_tabFontHooksReady.store(tabFontReady, std::memory_order_release);

  g_tabCloseHooksReady.store(tabCloseReady, std::memory_order_release);

  g_extensionTrackingReady.store(extensionTrackingReady, std::memory_order_release);

  g_extensionContainerTrackingReady.store(extensionContainerTrackingReady, std::memory_order_release);
  g_bookmarkFolderLiveUpdateReady.store(folderIconLiveUpdateReady, std::memory_order_release);


  Wh_Log(L"Chrome UI symbols resolved successfully");

  Wh_Log(L"Address bar font tweak: %ls", addressBarFontReady ? L"ready" : L"unavailable");
  Wh_Log(
      L"Address bar symbols: ctor=%ls dtor=%ls textfieldCtor=%ls setFont=%ls copyFont=%ls "
      L"getSize=%ls derive=%ls fontDtor=%ls",
      g_OmniboxViewViewsCtorOriginal ? L"ready" : L"MISSING",
      g_OmniboxViewViewsDtorOriginal ? L"ready" : L"MISSING",
      g_TextfieldCtorOriginal ? L"ready" : L"MISSING",
      g_TextfieldSetFontListOriginal ? L"ready" : L"MISSING",
      g_FontListCopyCtor ? L"ready" : L"MISSING",
      g_FontListGetFontSize ? L"ready" : L"MISSING",
      g_FontListDeriveWithSizeDelta ? L"ready" : L"MISSING",
      g_FontListDtor ? L"ready" : L"MISSING");

  Wh_Log(L"Tab title tweak: %ls", tabFontReady ? L"ready" : L"unavailable");

  Wh_Log(L"Tab close/layout tweak: %ls", tabCloseReady ? L"ready" : L"unavailable");

  Wh_Log(L"Extension width tweak: %ls", extensionWidthReady ? L"ready" : L"unavailable");

  Wh_Log(L"Extension width live update: %ls", extensionTrackingReady ? L"ready" : L"unavailable");

  Wh_Log(L"Extension container live propagation: %ls", extensionContainerTrackingReady ? L"ready" : L"unavailable");

  Wh_Log(L"Extension icon live regeneration: %ls", g_ToolbarActionViewUpdateState ? L"ready" : L"unavailable");

  return true;
}

// -----------------------------------------------------------------------------
// Slow symbol-resolution notifications
// -----------------------------------------------------------------------------

static void UpdateSymbolTooltip(NOTIFYICONDATAW& notifyIcon, const wchar_t* text) {
  notifyIcon.uFlags = NIF_TIP | NIF_SHOWTIP;
  wcsncpy_s(notifyIcon.szTip, text, _TRUNCATE);

  if (!Shell_NotifyIconW(NIM_MODIFY, &notifyIcon)) {
    Wh_Log(L"Failed to update symbol-resolution tooltip: %lu", GetLastError());
  }
}

static void ShowSymbolNotification(NOTIFYICONDATAW& notifyIcon,
                                   const wchar_t* title,
                                   const wchar_t* text,
                                   DWORD infoFlags) {
  notifyIcon.uFlags = NIF_INFO;
  notifyIcon.dwInfoFlags = infoFlags | NIIF_NOSOUND;
  wcsncpy_s(notifyIcon.szInfoTitle, title, _TRUNCATE);
  wcsncpy_s(notifyIcon.szInfo, text, _TRUNCATE);

  if (!Shell_NotifyIconW(NIM_MODIFY, &notifyIcon)) {
    Wh_Log(L"Failed to show symbol-resolution notification: %lu", GetLastError());
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

static DWORD WINAPI SymbolNotificationThreadProc(void*) {
  HWND window =
      CreateWindowExW(0, L"STATIC", L"", WS_OVERLAPPED, 0, 0, 0, 0, nullptr, nullptr, nullptr, nullptr);

  if (!window) {
    Wh_Log(L"Failed to create symbol notification window: %lu", GetLastError());
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
      L"Chrome Native UI Tweaks: preparing Chrome symbols in the background",
      _TRUNCATE);

  if (!Shell_NotifyIconW(NIM_ADD, &notifyIcon)) {
    Wh_Log(L"Failed to create symbol notification icon: %lu", GetLastError());
    DestroyWindow(window);
    return 0;
  }

  notifyIcon.uVersion = NOTIFYICON_VERSION_4;
  Shell_NotifyIconW(NIM_SETVERSION, &notifyIcon);

  // If symbol resolution happened to finish immediately after the 5-second
  // timeout, skip the transient "preparing" notification and show only the
  // completion result.
  if (WaitForSingleObject(g_symbolResolutionDoneEvent, 0) != WAIT_OBJECT_0) {
    ShowSymbolNotification(
        notifyIcon, L"Chrome Native UI Tweaks",
        L"Chrome symbols are being prepared in the background. Chrome started "
        L"without UI tweaks; you can keep using it.",
        NIIF_INFO);
  }

  HANDLE waitHandles[] = {g_symbolResolutionDoneEvent, g_symbolNotificationStopEvent};
  DWORD waitResult = WaitForHandlesWithMessageLoop(waitHandles, ARRAYSIZE(waitHandles));

  if (waitResult == WAIT_OBJECT_0) {
    bool success = g_symbolResolutionSucceeded.load(std::memory_order_acquire);

    if (success) {
      UpdateSymbolTooltip(
          notifyIcon,
          L"Chrome Native UI Tweaks: symbols ready - restart Chrome to activate the mod");
      ShowSymbolNotification(
          notifyIcon, L"Chrome Native UI Tweaks",
          L"Chrome symbol analysis is complete. Restart Chrome to activate the mod.",
          NIIF_INFO);
    } else {
      UpdateSymbolTooltip(
          notifyIcon,
          L"Chrome Native UI Tweaks: symbol analysis failed - see the Windhawk mod log");
      ShowSymbolNotification(
          notifyIcon, L"Chrome Native UI Tweaks",
          L"Chrome symbol analysis failed. The mod wasn't activated; see the Windhawk mod log.",
          NIIF_ERROR);
    }

    // Keep pumping this top-level window while the status icon remains alive,
    // so system broadcasts can't block on an unresponsive notification thread.
    HANDLE stopHandles[] = {g_symbolNotificationStopEvent};
    waitResult = WaitForHandlesWithMessageLoop(stopHandles, ARRAYSIZE(stopHandles));

    if (waitResult != WAIT_OBJECT_0 && waitResult != WAIT_ABANDONED) {
      Wh_Log(L"Symbol notification stop wait failed: %lu", GetLastError());
    }
  } else if (waitResult != WAIT_OBJECT_0 + 1 && waitResult != WAIT_ABANDONED) {
    Wh_Log(L"Symbol notification wait failed: %lu", GetLastError());
  }

  Shell_NotifyIconW(NIM_DELETE, &notifyIcon);
  DestroyWindow(window);
  return 0;
}

static void StartSymbolNotifications() {
  std::lock_guard<std::mutex> lock(g_workerMutex);

  if (g_unloading || g_symbolNotificationThread) return;

  g_symbolNotificationStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

  if (!g_symbolNotificationStopEvent) {
    Wh_Log(L"Failed to create symbol notification stop event: %lu", GetLastError());
    return;
  }

  g_symbolNotificationThread =
      CreateThread(nullptr, 0, SymbolNotificationThreadProc, nullptr, 0, nullptr);

  if (!g_symbolNotificationThread) {
    Wh_Log(L"Failed to create symbol notification thread: %lu", GetLastError());
    CloseHandle(g_symbolNotificationStopEvent);
    g_symbolNotificationStopEvent = nullptr;
  }
}

// -----------------------------------------------------------------------------
// Bounded startup symbol resolution
// -----------------------------------------------------------------------------

static DWORD WINAPI ChromeSymbolResolutionThreadProc(void* param) {
  HMODULE chromeDll = static_cast<HMODULE>(param);
  ULONGLONG startedAt = GetTickCount64();

  bool success = InstallChromeHooks(chromeDll);

  // If the 5-second startup window has already been abandoned, this resolver
  // run is cache preparation only. Don't leave feature-level "ready" flags set
  // when no hooks will be applied in this Chrome instance.
  if (g_hookActivationAbandoned.load(std::memory_order_acquire)) {
    ClearChromeRuntimeReadiness();
  }

  g_symbolResolutionSucceeded.store(success, std::memory_order_release);

  ULONGLONG elapsed = GetTickCount64() - startedAt;

  Wh_Log(L"Chrome symbol preparation finished in %llu ms: %ls", elapsed,
         success ? L"success" : L"FAILED");

  SetEvent(g_symbolResolutionDoneEvent);

  return 0;
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
      Wh_Log(L"Chrome hook setup abandoned before starting symbol resolution");
      return;
    }

    g_symbolResolutionDoneEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
  }

  if (!g_symbolResolutionDoneEvent) {
    Wh_Log(L"Failed to create Chrome symbol resolution event");

    if (g_hookActivationAbandoned.load(std::memory_order_acquire)) return;

    bool success = InstallChromeHooks(chromeDll);

    if (!success) return;

    if (g_hookActivationAbandoned.load(std::memory_order_acquire)) {
      ClearChromeRuntimeReadiness();
      return;
    }

    if (!Wh_ApplyHookOperations()) {
      Wh_Log(L"Wh_ApplyHookOperations failed");
      ClearChromeRuntimeReadiness();
      return;
    }

    g_hooksActivated.store(true, std::memory_order_release);
    return;
  }

  {
    std::lock_guard<std::mutex> lock(g_workerMutex);

    if (!g_unloading) {
      g_symbolResolutionThread =
          CreateThread(nullptr, 0, ChromeSymbolResolutionThreadProc, chromeDll, 0, nullptr);
    }
  }

  if (!g_symbolResolutionThread) {
    if (g_hookActivationAbandoned.load(std::memory_order_acquire)) {
      return;
    }

    Wh_Log(L"Failed to create Chrome symbol resolution thread");

    {
      std::lock_guard<std::mutex> lock(g_workerMutex);
      CloseHandle(g_symbolResolutionDoneEvent);
      g_symbolResolutionDoneEvent = nullptr;
    }

    bool success = InstallChromeHooks(chromeDll);

    if (!success) return;

    if (g_hookActivationAbandoned.load(std::memory_order_acquire)) {
      ClearChromeRuntimeReadiness();
      return;
    }

    if (!Wh_ApplyHookOperations()) {
      Wh_Log(L"Wh_ApplyHookOperations failed");
      ClearChromeRuntimeReadiness();
      return;
    }

    g_hooksActivated.store(true, std::memory_order_release);
    return;
  }

  ULONGLONG waitStartedAt = GetTickCount64();
  DWORD waitResult = WaitForSingleObject(g_symbolResolutionDoneEvent, kChromeSymbolStartupWaitMs);
  ULONGLONG waited = GetTickCount64() - waitStartedAt;

  if (waitResult == WAIT_OBJECT_0) {
    bool success = g_symbolResolutionSucceeded.load(std::memory_order_acquire);

    // Keep the resolver thread/event handles owned by Wh_ModBeforeUninit. This
    // avoids a close/read race if the mod is disabled around the startup wait.
    Wh_Log(L"Chrome symbol resolution completed within startup wait (%llu ms)", waited);

    if (!success) return;

    if (g_hookActivationAbandoned.load(std::memory_order_acquire)) {
      ClearChromeRuntimeReadiness();
      Wh_Log(L"Chrome hook activation was abandoned while symbol resolution was running");
      return;
    }

    if (!Wh_ApplyHookOperations()) {
      Wh_Log(L"Wh_ApplyHookOperations failed");
      ClearChromeRuntimeReadiness();
      return;
    }

    g_hooksActivated.store(true, std::memory_order_release);
    Wh_Log(L"Chrome hooks activated during startup");
    return;
  }

  if (waitResult == WAIT_TIMEOUT) {
    // Don't late-apply hooks when resolution finishes. Constructor-based
    // tracking would miss UI objects that Chrome already created, producing a
    // partially active mod. The worker is intentionally allowed to finish only
    // so the next Chrome launch can use the populated symbol cache.
    g_hookActivationAbandoned.store(true, std::memory_order_release);
    ClearChromeRuntimeReadiness();

    Wh_Log(
        L"Chrome symbol startup wait timed out after %llu ms; continuing Chrome "
        L"without UI tweaks. After symbol analysis finishes, restart Chrome to activate the mod",
        waited);
    StartSymbolNotifications();
    return;
  }

  g_hookActivationAbandoned.store(true, std::memory_order_release);
  ClearChromeRuntimeReadiness();

  Wh_Log(L"Chrome symbol wait failed: %lu; continuing Chrome without UI tweaks", GetLastError());
  StartSymbolNotifications();
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
    Wh_Log(L"chrome.dll already loaded; deferring Chrome symbol setup until Wh_ModAfterInit");
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
  bool oldUseWindowsFolderIcon = g_useWindowsFolderIcon.load(std::memory_order_relaxed);

  LoadSettings();

  if (!g_hooksActivated.load(std::memory_order_acquire)) return;

  ApplyFontToExistingBookmarkLabels();
  ApplyFontToExistingAddressBars();

  if (oldUseWindowsFolderIcon != g_useWindowsFolderIcon.load(std::memory_order_relaxed)) {
    ApplyFolderIconToExistingBookmarkBars();
  }

  ApplyTweaksToExistingTabs();
  ApplyWidthToExistingExtensionButtons();
}

void Wh_ModBeforeUninit() {
  HANDLE symbolNotificationThread = nullptr;
  HANDLE symbolNotificationStopEvent = nullptr;
  HANDLE symbolResolutionThread = nullptr;
  HANDLE symbolResolutionDoneEvent = nullptr;

  {
    std::unique_lock<std::mutex> lock(g_workerMutex);

    // Close the worker-start gate before inspecting any handles. If
    // StartChromeHookSetup is already running on Chrome's loader thread, wait
    // until it has finished creating/using its startup handles. No new worker
    // can be created after g_unloading becomes true.
    g_unloading = true;
    g_hookActivationAbandoned.store(true, std::memory_order_release);

    if (g_symbolNotificationStopEvent) {
      SetEvent(g_symbolNotificationStopEvent);
    }

    g_workerCondition.wait(lock, [] { return !g_chromeSetupInProgress; });

    // StartChromeHookSetup may have created the notification thread just before
    // teardown acquired the gate, so signal the stop event again after the
    // setup call has fully returned.
    if (g_symbolNotificationStopEvent) {
      SetEvent(g_symbolNotificationStopEvent);
    }

    symbolNotificationThread = g_symbolNotificationThread;
    symbolNotificationStopEvent = g_symbolNotificationStopEvent;
    symbolResolutionThread = g_symbolResolutionThread;
    symbolResolutionDoneEvent = g_symbolResolutionDoneEvent;
  }

  g_bookmarkFontSize.store(-1, std::memory_order_relaxed);
  g_useWindowsFolderIcon.store(false, std::memory_order_relaxed);
  g_addressBarFontSize.store(-1, std::memory_order_relaxed);
  g_menuFontSize.store(-1, std::memory_order_relaxed);
  g_menuVerticalSpacing.store(-1, std::memory_order_relaxed);
  g_menuCornerRadius.store(-1, std::memory_order_relaxed);
  g_tabFontSize.store(-1, std::memory_order_relaxed);
  g_tabCloseButtonsHidden.store(false, std::memory_order_relaxed);
  g_tabPreTitlePadding.store(kChromeDefaultTabPreTitlePadding, std::memory_order_relaxed);
  g_extensionButtonWidth.store(kChromeDefaultExtensionButtonWidth, std::memory_order_relaxed);

  if (symbolNotificationThread) {
    WaitForSingleObject(symbolNotificationThread, INFINITE);
  }

  // HookSymbols can't be cancelled safely. Wait for the resolver before the
  // mod DLL is unloaded so its worker can't continue executing unloaded code.
  if (symbolResolutionThread) {
    WaitForSingleObject(symbolResolutionThread, INFINITE);
  }

  if (symbolNotificationThread) CloseHandle(symbolNotificationThread);
  if (symbolNotificationStopEvent) CloseHandle(symbolNotificationStopEvent);
  if (symbolResolutionThread) CloseHandle(symbolResolutionThread);
  if (symbolResolutionDoneEvent) CloseHandle(symbolResolutionDoneEvent);

  {
    std::lock_guard<std::mutex> lock(g_workerMutex);
    g_symbolNotificationThread = nullptr;
    g_symbolNotificationStopEvent = nullptr;
    g_symbolResolutionThread = nullptr;
    g_symbolResolutionDoneEvent = nullptr;
  }

  if (!g_hooksActivated.load(std::memory_order_acquire)) return;

  ApplyFontToExistingBookmarkLabels(true);
  ApplyFontToExistingAddressBars(true);
  ApplyFolderIconToExistingBookmarkBars();
  ApplyTweaksToExistingTabs(true);
  ApplyWidthToExistingExtensionButtons();
  DestroyWindowsFolderImageOnOwningThread();
}

void Wh_ModUninit() {
  Wh_Log(L"Chrome Native UI Tweaks unloaded");
}

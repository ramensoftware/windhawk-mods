// ==WindhawkMod==
// @id              chrome-native-ui-tweaks
// @name            Chrome Native UI Tweaks
// @description     Customize Chrome's native UI: fonts, tab close (×) buttons, menu density, extension button width, and bookmark folder icons. Web content is untouched.
// @version         1.0
// @author          Dron007
// @github          https://github.com/Dron007
// @include         chrome.exe
// @architecture    x86-64
// ==/WindhawkMod==

// clang-format off
// ==WindhawkModSettings==
/*
- bookmarks:
  - fontSize: "14"
    $name: "Font size"
    $description: "Bookmark bar font size. Chrome default is 12."
    $options:
    - "12": "12 (Chrome default)"
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

  - folderIcon: "windows"
    $name: "Folder icon"
    $description: "Folder icon used on the bookmark bar and in bookmark menus."
    $options:
    - "windows": "Windows system"
    - "chrome": "Chrome default"
  $name: "Bookmarks"

- tabs:
  - fontSize: "12"
    $name: "Title font size"
    $description: "Font size for titles in the horizontal tab strip. Chrome default is 12."
    $options:
    - "12": "12 (Chrome default)"
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
    $name: "Pinned button width"
    $description: "Horizontal width of pinned extension buttons. Button height stays unchanged."
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
  - fontSize: "12"
    $name: "Font size"
    $description: "Font size for bookmark folders, the three-dot menu, and Chrome context menus. Chrome default is 12."
    $options:
    - "12": "12 (Chrome default)"
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
    - "2": "2"
    - "4": "4"
    - "6": "6"
    - "8": "8"
    - "10": "10"
    - "12": "12"
  $name: "Menus"
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Chrome Native UI Tweaks

Customize several parts of Chrome's native browser interface while leaving web content untouched.

## Getting started

1. Enable the mod.
2. Restart Chrome once.
3. Open the mod settings, adjust the options, and click **Save settings**.

Saved setting changes are applied immediately without restarting Chrome, so you can quickly experiment with fonts, spacing, button widths, and icons until the UI looks right for you.

The mod doesn't make permanent changes to Chrome. If you disable or uninstall it, restart Chrome and the original UI is restored.

## Features

### Bookmarks
- Adjust the bookmark bar font size.
- Replace Chrome's bookmark folder icon with the Windows system folder icon.

### Tabs
- Adjust the tab title font size.
- Hide tab close (×) buttons and reclaim the space reserved for them.
- Adjust the spacing between the favicon and tab title.

### Extensions
- Reduce the width of pinned extension buttons to leave more room for the address bar.

### Menus
- Adjust menu font size.
- Reduce vertical spacing between menu items.
- Adjust popup menu corner radius.

## Notes

- Tested with **Google Chrome 152.0.7977.65 x64**.
- When Chrome starts, Windhawk may show an initialization window while the mod resolves Chrome UI symbols.
  This can take several seconds, especially on the first run with a new Chrome version.
- Chrome instances launched by automation tools such as **Playwright** with `--remote-debugging-pipe` are intentionally ignored.
- The mod relies on Chrome's internal native UI symbols. Chrome updates can occasionally require compatibility adjustments.
*/
// ==/WindhawkModReadme==
// clang-format on

#include <windows.h>
#include <shellapi.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cwchar>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#pragma comment(lib, "shell32.lib")

static constexpr PCWSTR kChromeSymbolServer = L"https://chromium-browser-symsrv.commondatastorage.googleapis.com";

static constexpr int kChromeDefaultFontSize = 12;
static constexpr int kChromeDefaultTabPreTitlePadding = 8;
static constexpr int kChromeDefaultExtensionButtonWidth = 34;

// Chrome 152 LayoutConstant ordinals.
static constexpr int kLayoutTabAfterTitlePadding = 33;
static constexpr int kLayoutTabAlertIndicatorCaptureIconWidth = 34;
static constexpr int kLayoutTabAlertIndicatorIconWidth = 35;
static constexpr int kLayoutTabCloseButtonSize = 36;
static constexpr int kLayoutTabHeight = 37;
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

// The Windows-folder image is created once and kept for the lifetime of the
// Chrome process. These buffers are intentionally oversized for the small
// Chromium wrapper objects used by the proven PoC.
alignas(64) static unsigned char g_skBitmapStorage[256];
alignas(64) static unsigned char g_imageSkiaStorage[256];

// -----------------------------------------------------------------------------
// Function types
// -----------------------------------------------------------------------------

using BookmarkButtonBaseCtorFn = void (*)(void*, void*, const void*);

using BookmarkMenuButtonBaseCtorFn = void (*)(void*, void*, void*, const void*);

using LabelButtonLabelCtorFn = void (*)(void*, const void*, int);

using LabelButtonLabelDeletingDtorFn = void* (*)(void*, unsigned int);

using TypographyGetFontFn = const FontListOpaque* (*)(const void*, int, int);

using ResourceBundleGetSharedFn = void* (*)();

using ResourceBundleGetFontDeltaFn = const FontListOpaque* (*)(void*, int);

using LabelSetFontListFn = void (*)(void*, const FontListOpaque*);

using MenuItemGetFontListFn = FontListOpaque* (*)(const void*, FontListOpaque*);

using MenuItemGetVerticalMarginFn = int (*)(const void*);

using MenuConfigCornerRadiusForMenuFn = int (*)(const void*, const void*);

using FontListCopyCtorFn = void (*)(FontListOpaque*, const FontListOpaque*);

using TabTitleCtorFn = void (*)(void*);
using TabTitleDtorFn = void (*)(void*);

using TabCloseButtonCtorFn = void (*)(void*, void*, void*);

using TabCloseButtonDtorFn = void (*)(void*);

using ViewSetVisibleFn = void (*)(void*, bool);

using ViewInvalidateLayoutFn = void (*)(void*, bool);

using ViewPreferredSizeChangedFn = void (*)(void*);

using GetLayoutConstantFn = int (*)(int);

// gfx::Size is returned through a hidden sret buffer on Win64:
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
static ResourceBundleGetSharedFn g_ResourceBundleGetShared;
static ResourceBundleGetFontDeltaFn g_ResourceBundleGetFontDelta;
static LabelSetFontListFn g_LabelSetFontList;

static MenuItemGetFontListFn g_MenuItemGetFontListOriginal;
static MenuItemGetVerticalMarginFn g_MenuItemGetVerticalMarginOriginal;
static MenuConfigCornerRadiusForMenuFn g_MenuConfigCornerRadiusForMenuOriginal;
static FontListCopyCtorFn g_FontListCopyCtor;

static TabTitleCtorFn g_TabTitleCtorOriginal;
static TabTitleDtorFn g_TabTitleDtorOriginal;
static TabCloseButtonCtorFn g_TabCloseButtonCtorOriginal;
static TabCloseButtonDtorFn g_TabCloseButtonDtorOriginal;

static ViewSetVisibleFn g_ViewSetVisibleOriginal;
static ViewInvalidateLayoutFn g_ViewInvalidateLayout;
static ViewPreferredSizeChangedFn g_ViewPreferredSizeChanged;
static GetLayoutConstantFn g_GetLayoutConstantOriginal;

static ToolbarActionViewCalculatePreferredSizeFn g_ToolbarActionViewCalculatePreferredSizeOriginal;

static ToolbarActionViewDeletingDtorFn g_ToolbarActionViewDeletingDtorOriginal;

static ToolbarActionViewUpdateStateFn g_ToolbarActionViewUpdateState;

static ExtensionsToolbarDesktopCtorFn g_ExtensionsToolbarDesktopCtorOriginal;

static ExtensionsToolbarDesktopDeletingDtorFn g_ExtensionsToolbarDesktopDeletingDtorOriginal;

static GetBookmarkFolderIconFn g_GetBookmarkFolderIconOriginal;
static CreateSkBitmapFromHICONFn g_CreateSkBitmapFromHICON;
static ImageSkiaCreateFrom1xBitmapFn g_ImageSkiaCreateFrom1xBitmap;
static ImageModelFromImageSkiaFn g_ImageModelFromImageSkia;
static BookmarkBarViewUpdateAppearanceForThemeFn g_BookmarkBarViewUpdateAppearanceForThemeOriginal;
static BookmarkBarViewDeletingDtorFn g_BookmarkBarViewDeletingDtorOriginal;

// -----------------------------------------------------------------------------
// State
// -----------------------------------------------------------------------------

static std::atomic_bool g_chromeSetupStarted = false;
static std::atomic_bool g_hooksResolved = false;

static std::atomic_int g_bookmarkFontSize = 14;
static std::atomic_int g_menuFontSize = 12;
static std::atomic_int g_tabFontSize = 12;

static std::atomic_int g_tabPreTitlePadding = kChromeDefaultTabPreTitlePadding;

static std::atomic_int g_extensionButtonWidth = kChromeDefaultExtensionButtonWidth;

// -1 = Chrome default.
static std::atomic_int g_menuVerticalSpacing = -1;
static std::atomic_int g_menuCornerRadius = -1;

static std::atomic_bool g_tabCloseButtonsHidden = false;
static std::atomic_bool g_useWindowsFolderIcon = true;

static std::atomic_bool g_tabFontHooksReady = false;
static std::atomic_bool g_tabCloseHooksReady = false;

static std::atomic_bool g_extensionTrackingReady = false;
static std::atomic_bool g_extensionContainerTrackingReady = false;
static std::atomic_bool g_windowsFolderReady = false;
static std::atomic_bool g_bookmarkFolderLiveUpdateReady = false;

static std::once_flag g_windowsFolderOnce;

//  0 = not checked
//  1 = compatible
// -1 = incompatible
static std::atomic_int g_tabLayoutCompatibility = 0;

static thread_local int g_bookmarkCtorDepth = 0;
static thread_local int g_bookmarkMenuCtorDepth = 0;

static std::mutex g_labelsMutex;
static std::unordered_map<void*, DWORD> g_labels;

static std::mutex g_bookmarkBarsMutex;
static std::unordered_map<void*, DWORD> g_bookmarkBars;

static std::mutex g_tabObjectsMutex;
static std::unordered_map<void*, DWORD> g_tabTitles;
static std::unordered_set<void*> g_tabCloseButtons;

static std::mutex g_extensionViewsMutex;
static std::unordered_map<void*, DWORD> g_extensionViews;
static std::unordered_map<void*, DWORD> g_extensionContainers;

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

static int ReadFontSizeSetting(PCWSTR name, int fallback) {
  PCWSTR value = Wh_GetStringSetting(name);
  int result = value ? _wtoi(value) : fallback;

  if (value) Wh_FreeStringSetting(value);

  if (result < 12 || result > 24) result = fallback;

  return result;
}

static bool ReadWindowsFolderIconSetting() {
  PCWSTR value = Wh_GetStringSetting(L"bookmarks.folderIcon");
  bool useWindows = !value || wcscmp(value, L"windows") == 0;

  if (value) Wh_FreeStringSetting(value);

  return useWindows;
}

static int ReadMenuSpacingSetting() {
  PCWSTR value = Wh_GetStringSetting(L"menus.verticalSpacing");

  if (!value) return -1;

  int result;

  if (wcscmp(value, L"default") == 0) {
    result = -1;
  } else {
    result = _wtoi(value);

    if (result < 0 || result > 6) result = -1;
  }

  Wh_FreeStringSetting(value);
  return result;
}

static int ReadMenuCornerRadiusSetting() {
  PCWSTR value = Wh_GetStringSetting(L"menus.cornerRadius");

  if (!value) return -1;

  int result;

  if (wcscmp(value, L"default") == 0) {
    result = -1;
  } else {
    result = _wtoi(value);

    if (result < 0 || result > 12) result = -1;
  }

  Wh_FreeStringSetting(value);
  return result;
}

static int ReadTabPreTitlePaddingSetting() {
  PCWSTR value = Wh_GetStringSetting(L"tabs.iconTitleSpacing");

  int result = value ? _wtoi(value) : kChromeDefaultTabPreTitlePadding;

  if (value) Wh_FreeStringSetting(value);

  if (result < 2 || result > 8) result = kChromeDefaultTabPreTitlePadding;

  return result;
}

static int ReadExtensionButtonWidthSetting() {
  PCWSTR value = Wh_GetStringSetting(L"extensions.buttonWidth");

  int result = value ? _wtoi(value) : kChromeDefaultExtensionButtonWidth;

  if (value) Wh_FreeStringSetting(value);

  if (result < 28 || result > 34) result = kChromeDefaultExtensionButtonWidth;

  return result;
}

static void LoadSettings() {
  int bookmarkFontSize = ReadFontSizeSetting(L"bookmarks.fontSize", 14);

  bool useWindowsFolderIcon = ReadWindowsFolderIconSetting();

  int menuFontSize = ReadFontSizeSetting(L"menus.fontSize", 12);

  int menuVerticalSpacing = ReadMenuSpacingSetting();

  int menuCornerRadius = ReadMenuCornerRadiusSetting();

  int tabFontSize = ReadFontSizeSetting(L"tabs.fontSize", 12);

  bool tabCloseButtonsHidden = Wh_GetIntSetting(L"tabs.hideCloseButtons") != 0;

  int tabPreTitlePadding = ReadTabPreTitlePaddingSetting();

  int extensionButtonWidth = ReadExtensionButtonWidthSetting();

  g_bookmarkFontSize.store(bookmarkFontSize, std::memory_order_relaxed);

  g_useWindowsFolderIcon.store(useWindowsFolderIcon, std::memory_order_relaxed);

  g_menuFontSize.store(menuFontSize, std::memory_order_relaxed);

  g_menuVerticalSpacing.store(menuVerticalSpacing, std::memory_order_relaxed);

  g_menuCornerRadius.store(menuCornerRadius, std::memory_order_relaxed);

  g_tabFontSize.store(tabFontSize, std::memory_order_relaxed);

  g_tabCloseButtonsHidden.store(tabCloseButtonsHidden, std::memory_order_relaxed);

  g_tabPreTitlePadding.store(tabPreTitlePadding, std::memory_order_relaxed);

  g_extensionButtonWidth.store(extensionButtonWidth, std::memory_order_relaxed);

  Wh_Log(L"Bookmark bar font size: %d", bookmarkFontSize);

  Wh_Log(L"Bookmark folder icon: %ls", useWindowsFolderIcon ? L"Windows system" : L"Chrome default");

  Wh_Log(L"Menu font size: %d", menuFontSize);

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

  Wh_Log(L"Tab title font size: %d", tabFontSize);

  Wh_Log(L"Tab close buttons: %ls", tabCloseButtonsHidden ? L"Hidden" : L"Chrome default");

  Wh_Log(L"Tab icon/title spacing: %d", tabPreTitlePadding);

  Wh_Log(L"Extension button width: %d", extensionButtonWidth);
}

// -----------------------------------------------------------------------------
// Fonts
// -----------------------------------------------------------------------------

static const FontListOpaque* GetFontForSize(int fontSize) {
  if (!g_ResourceBundleGetShared || !g_ResourceBundleGetFontDelta) {
    return nullptr;
  }

  void* resourceBundle = g_ResourceBundleGetShared();

  if (!resourceBundle) return nullptr;

  return g_ResourceBundleGetFontDelta(resourceBundle, fontSize - kChromeDefaultFontSize);
}

static const FontListOpaque* GetConfiguredBookmarkFont() {
  return GetFontForSize(g_bookmarkFontSize.load(std::memory_order_relaxed));
}

static const FontListOpaque* GetConfiguredMenuFont() {
  return GetFontForSize(g_menuFontSize.load(std::memory_order_relaxed));
}

static const FontListOpaque* GetConfiguredTabFont() {
  return GetFontForSize(g_tabFontSize.load(std::memory_order_relaxed));
}

// -----------------------------------------------------------------------------
// Bookmark bar
// -----------------------------------------------------------------------------

static void BookmarkButtonBaseCtorHook(void* self, void* pressedCallback, const void* title) {
  g_bookmarkCtorDepth++;

  g_BookmarkButtonBaseCtorOriginal(self, pressedCallback, title);

  g_bookmarkCtorDepth--;
}

static void BookmarkMenuButtonBaseCtorHook(void* self, void* pressedCallback, void* showMenuCallback,
                                           const void* title) {
  g_bookmarkMenuCtorDepth++;

  g_BookmarkMenuButtonBaseCtorOriginal(self, pressedCallback, showMenuCallback, title);

  g_bookmarkMenuCtorDepth--;
}

static void LabelButtonLabelCtorHook(void* self, const void* text, int textContext) {
  bool isBookmarkLabel = g_bookmarkCtorDepth > 0 || g_bookmarkMenuCtorDepth > 0;

  g_LabelButtonLabelCtorOriginal(self, text, textContext);

  if (!isBookmarkLabel) return;

  std::lock_guard<std::mutex> lock(g_labelsMutex);

  g_labels[self] = GetCurrentThreadId();
}

static void* LabelButtonLabelDeletingDtorHook(void* self, unsigned int flags) {
  {
    std::lock_guard<std::mutex> lock(g_labelsMutex);

    g_labels.erase(self);
  }

  return g_LabelButtonLabelDeletingDtorOriginal(self, flags);
}

static const FontListOpaque* TypographyGetFontHook(const void* self, int context, int style) {
  const FontListOpaque* originalFont = g_TypographyGetFontOriginal(self, context, style);

  if (g_bookmarkCtorDepth <= 0 && g_bookmarkMenuCtorDepth <= 0) {
    return originalFont;
  }

  const FontListOpaque* configuredFont = GetConfiguredBookmarkFont();

  return configuredFont ? configuredFont : originalFont;
}

// -----------------------------------------------------------------------------
// Bookmark folder icon
// -----------------------------------------------------------------------------

static void CreateWindowsFolderImage() {
  if (!g_CreateSkBitmapFromHICON || !g_ImageSkiaCreateFrom1xBitmap || !g_ImageModelFromImageSkia) {
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

  g_CreateSkBitmapFromHICON(g_skBitmapStorage, iconInfo.hIcon);
  DestroyIcon(iconInfo.hIcon);

  g_ImageSkiaCreateFrom1xBitmap(g_imageSkiaStorage, g_skBitmapStorage);
  g_windowsFolderReady.store(true, std::memory_order_release);

  Wh_Log(L"Windows bookmark folder image: ready");
}

static ImageModelOpaque* GetBookmarkFolderIconHook(ImageModelOpaque* result, int iconType,
                                                   uintptr_t colorVariantOpaque) {
  constexpr int kNormal = 0;

  if (iconType != kNormal || !g_useWindowsFolderIcon.load(std::memory_order_relaxed)) {
    return g_GetBookmarkFolderIconOriginal(result, iconType, colorVariantOpaque);
  }

  std::call_once(g_windowsFolderOnce, CreateWindowsFolderImage);

  if (!g_windowsFolderReady.load(std::memory_order_acquire)) {
    return g_GetBookmarkFolderIconOriginal(result, iconType, colorVariantOpaque);
  }

  return g_ImageModelFromImageSkia(result, g_imageSkiaStorage);
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

  if (targetSize == kChromeDefaultFontSize || !g_FontListCopyCtor) {
    return g_MenuItemGetFontListOriginal(self, result);
  }

  const FontListOpaque* configuredFont = GetConfiguredMenuFont();

  if (!configuredFont) {
    return g_MenuItemGetFontListOriginal(self, result);
  }

  g_FontListCopyCtor(result, configuredFont);

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
  g_TabTitleCtorOriginal(self);

  if (!g_tabFontHooksReady.load(std::memory_order_relaxed)) {
    return;
  }

  {
    std::lock_guard<std::mutex> lock(g_tabObjectsMutex);

    g_tabTitles[self] = GetCurrentThreadId();
  }

  int fontSize = g_tabFontSize.load(std::memory_order_relaxed);

  if (fontSize == kChromeDefaultFontSize) return;

  const FontListOpaque* font = GetConfiguredTabFont();

  if (font && g_LabelSetFontList) {
    g_LabelSetFontList(self, font);
  }
}

static void TabTitleDtorHook(void* self) {
  if (g_tabFontHooksReady.load(std::memory_order_relaxed)) {
    std::lock_guard<std::mutex> lock(g_tabObjectsMutex);

    g_tabTitles.erase(self);
  }

  g_TabTitleDtorOriginal(self);
}

static void TabCloseButtonCtorHook(void* self, void* pressedCallback, void* mouseEventCallback) {
  g_TabCloseButtonCtorOriginal(self, pressedCallback, mouseEventCallback);

  if (!g_tabCloseHooksReady.load(std::memory_order_relaxed)) {
    return;
  }

  std::lock_guard<std::mutex> lock(g_tabObjectsMutex);

  g_tabCloseButtons.insert(self);
}

static void TabCloseButtonDtorHook(void* self) {
  if (g_tabCloseHooksReady.load(std::memory_order_relaxed)) {
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

  int preTitle = g_GetLayoutConstantOriginal(kLayoutTabPreTitlePadding);

  bool valid = (afterTitle == 4 || afterTitle == 8) && captureIcon == 16 && (alertIcon == 12 || alertIcon == 16) &&
               (closeButton == 14 || closeButton == 16 || closeButton == 24) && tabHeight >= 34 && tabHeight <= 40 &&
               preTitle >= 4 && preTitle <= 12;

  int newState = valid ? 1 : -1;

  int expected = 0;

  if (g_tabLayoutCompatibility.compare_exchange_strong(expected, newState, std::memory_order_release,
                                                       std::memory_order_relaxed)) {
    if (valid) {
      Wh_Log(
          L"Tab layout constants validated: "
          L"afterTitle=%d capture=%d alert=%d close=%d "
          L"height=%d preTitle=%d",
          afterTitle, captureIcon, alertIcon, closeButton, tabHeight, preTitle);
    } else {
      Wh_Log(
          L"WARNING: Tab layout constants don't match expected Chromium "
          L"layout; tab layout tweaks disabled. "
          L"Values: afterTitle=%d capture=%d alert=%d close=%d "
          L"height=%d preTitle=%d",
          afterTitle, captureIcon, alertIcon, closeButton, tabHeight, preTitle);
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

static GfxSizeOpaque* ToolbarActionViewCalculatePreferredSizeHook(const void* self, GfxSizeOpaque* result,
                                                                  const void* availableSize) {
  GfxSizeOpaque* returned = g_ToolbarActionViewCalculatePreferredSizeOriginal(self, result, availableSize);

  if (!result) return returned;

  bool normalDesktopButton =
      result->width == kChromeDefaultExtensionButtonWidth && result->height == kChromeDefaultExtensionButtonWidth;

  if (normalDesktopButton && g_extensionTrackingReady.load(std::memory_order_relaxed)) {
    std::lock_guard<std::mutex> lock(g_extensionViewsMutex);

    g_extensionViews[const_cast<void*>(self)] = GetCurrentThreadId();
  }

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
  static UINT message = RegisterWindowMessageW(L"Windhawk_ChromeUITweaks_RunFromWindowThread");

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
          if (wcsncmp(className, L"Chrome_WidgetWin_", 17) == 0) {
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
// Live bookmark update
// -----------------------------------------------------------------------------

static void WINAPI ApplyBookmarkFontOnCurrentThread(void*) {
  if (!g_LabelSetFontList) return;

  const FontListOpaque* font = GetConfiguredBookmarkFont();

  if (!font) return;

  DWORD threadId = GetCurrentThreadId();

  std::vector<void*> labels;

  {
    std::lock_guard<std::mutex> lock(g_labelsMutex);

    for (const auto& [label, labelThreadId] : g_labels) {
      if (labelThreadId == threadId) {
        labels.push_back(label);
      }
    }
  }

  for (void* label : labels) {
    g_LabelSetFontList(label, font);
  }
}

static void ApplyFontToExistingBookmarkLabels() {
  std::vector<DWORD> threadIds;

  {
    std::lock_guard<std::mutex> lock(g_labelsMutex);

    for (const auto& [label, threadId] : g_labels) {
      threadIds.push_back(threadId);
    }
  }

  std::sort(threadIds.begin(), threadIds.end());

  threadIds.erase(std::unique(threadIds.begin(), threadIds.end()), threadIds.end());

  for (DWORD threadId : threadIds) {
    HWND hwnd = FindWindowForThread(threadId);

    if (hwnd) {
      RunFromWindowThread(hwnd, ApplyBookmarkFontOnCurrentThread, nullptr);
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

static void WINAPI ApplyTabTweaksOnCurrentThread(void*) {
  DWORD threadId = GetCurrentThreadId();

  std::vector<void*> titles;

  {
    std::lock_guard<std::mutex> lock(g_tabObjectsMutex);

    for (const auto& [title, titleThreadId] : g_tabTitles) {
      if (titleThreadId == threadId) {
        titles.push_back(title);
      }
    }
  }

  const FontListOpaque* font = g_tabFontHooksReady.load(std::memory_order_relaxed) ? GetConfiguredTabFont() : nullptr;

  for (void* title : titles) {
    if (font && g_LabelSetFontList) {
      g_LabelSetFontList(title, font);
    }

    if (g_ViewInvalidateLayout) {
      g_ViewInvalidateLayout(title, false);
    }
  }
}

static void ApplyTweaksToExistingTabs() {
  std::vector<DWORD> threadIds;

  {
    std::lock_guard<std::mutex> lock(g_tabObjectsMutex);

    for (const auto& [title, threadId] : g_tabTitles) {
      threadIds.push_back(threadId);
    }
  }

  std::sort(threadIds.begin(), threadIds.end());

  threadIds.erase(std::unique(threadIds.begin(), threadIds.end()), threadIds.end());

  for (DWORD threadId : threadIds) {
    HWND hwnd = FindWindowForThread(threadId);

    if (hwnd) {
      RunFromWindowThread(hwnd, ApplyTabTweaksOnCurrentThread, nullptr);
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
  if (g_chromeSetupStarted.exchange(true)) return g_hooksResolved.load();

  wchar_t path[32768] = {};

  if (GetModuleFileNameW(chromeDll, path, ARRAYSIZE(path))) {
    Wh_Log(L"Resolving symbols for: %ls", path);
  }

  WindhawkUtils::SYMBOL_HOOK hooks[] = {

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

      {{LR"(?GetSharedInstance@ResourceBundle@ui@@SAAEAV12@XZ)"}, &g_ResourceBundleGetShared, nullptr, false},

      {{LR"(?GetFontListWithDelta@ResourceBundle@ui@@QEAAAEBVFontList@gfx@@H@Z)"},
       &g_ResourceBundleGetFontDelta,
       nullptr,
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

      {{L"views::MenuItemView::GetFontList"}, &g_MenuItemGetFontListOriginal, MenuItemGetFontListHook, true},

      {{L"views::MenuItemView::GetVerticalMargin"},
       &g_MenuItemGetVerticalMarginOriginal,
       MenuItemGetVerticalMarginHook,
       true},

      {{L"views::MenuConfig::CornerRadiusForMenu"},
       &g_MenuConfigCornerRadiusForMenuOriginal,
       MenuConfigCornerRadiusForMenuHook,
       true},

      {{LR"(??0FontList@gfx@@QEAA@AEBV01@@Z)"}, &g_FontListCopyCtor, nullptr, true},

      // -----------------------------------------------------------------------
      // Tabs / Views: OPTIONAL
      // -----------------------------------------------------------------------

      {{L"TabTitle::TabTitle", LR"(??0TabTitle@@QEAA@XZ)"}, &g_TabTitleCtorOriginal, TabTitleCtorHook, true},

      {{L"TabTitle::~TabTitle", LR"(??1TabTitle@@UEAA@XZ)"}, &g_TabTitleDtorOriginal, TabTitleDtorHook, true},

      {{L"TabCloseButton::TabCloseButton"}, &g_TabCloseButtonCtorOriginal, TabCloseButtonCtorHook, true},

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

      {{L"ToolbarActionView::CalculatePreferredSize"},
       &g_ToolbarActionViewCalculatePreferredSizeOriginal,
       ToolbarActionViewCalculatePreferredSizeHook,
       true},

      {{LR"(??_GToolbarActionView@@UEAAPEAXI@Z)"},
       &g_ToolbarActionViewDeletingDtorOriginal,
       ToolbarActionViewDeletingDtorHook,
       true},

      {{L"ToolbarActionView::UpdateState"}, &g_ToolbarActionViewUpdateState, nullptr, true},

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

  if (!WindhawkUtils::HookSymbols(chromeDll, hooks, ARRAYSIZE(hooks), &options)) {
    Wh_Log(L"ERROR: Failed to resolve required Chrome symbols");

    return false;
  }

  bool folderIconReady = g_GetBookmarkFolderIconOriginal && g_CreateSkBitmapFromHICON &&
                         g_ImageSkiaCreateFrom1xBitmap && g_ImageModelFromImageSkia;

  bool folderIconLiveUpdateReady = folderIconReady && g_BookmarkBarViewUpdateAppearanceForThemeOriginal &&
                                   g_BookmarkBarViewDeletingDtorOriginal;

  bool tabFontReady = g_TabTitleCtorOriginal && g_TabTitleDtorOriginal && g_LabelSetFontList &&
                      g_ResourceBundleGetShared && g_ResourceBundleGetFontDelta;

  bool tabCloseReady = g_TabCloseButtonCtorOriginal && g_TabCloseButtonDtorOriginal && g_ViewSetVisibleOriginal &&
                       g_ViewInvalidateLayout && g_GetLayoutConstantOriginal;

  bool extensionWidthReady = g_ToolbarActionViewCalculatePreferredSizeOriginal;

  Wh_Log(L"Bookmark folder icon tweak: %ls", folderIconReady ? L"ready" : L"unavailable");
  Wh_Log(L"Bookmark folder icon live update: %ls", folderIconLiveUpdateReady ? L"ready" : L"unavailable");

  Wh_Log(
      L"Extension live symbols: actionDeletingDtor=%ls "
      L"preferredSizeChanged=%ls",
      g_ToolbarActionViewDeletingDtorOriginal ? L"ready" : L"MISSING",
      g_ViewPreferredSizeChanged ? L"ready" : L"MISSING");

  Wh_Log(L"Extension container symbols: ctor=%ls deletingDtor=%ls",
         g_ExtensionsToolbarDesktopCtorOriginal ? L"ready" : L"MISSING",
         g_ExtensionsToolbarDesktopDeletingDtorOriginal ? L"ready" : L"MISSING");

  bool extensionTrackingReady =
      extensionWidthReady && g_ToolbarActionViewDeletingDtorOriginal && g_ViewPreferredSizeChanged;

  bool extensionContainerTrackingReady = g_ExtensionsToolbarDesktopCtorOriginal &&
                                         g_ExtensionsToolbarDesktopDeletingDtorOriginal && g_ViewPreferredSizeChanged;

  g_tabFontHooksReady.store(tabFontReady, std::memory_order_release);

  g_tabCloseHooksReady.store(tabCloseReady, std::memory_order_release);

  g_extensionTrackingReady.store(extensionTrackingReady, std::memory_order_release);

  g_extensionContainerTrackingReady.store(extensionContainerTrackingReady, std::memory_order_release);
  g_bookmarkFolderLiveUpdateReady.store(folderIconLiveUpdateReady, std::memory_order_release);

  g_hooksResolved.store(true);

  Wh_Log(L"Chrome UI symbols resolved successfully");

  Wh_Log(L"Tab title tweak: %ls", tabFontReady ? L"ready" : L"unavailable");

  Wh_Log(L"Tab close/layout tweak: %ls", tabCloseReady ? L"ready" : L"unavailable");

  Wh_Log(L"Extension width tweak: %ls", extensionWidthReady ? L"ready" : L"unavailable");

  Wh_Log(L"Extension width live update: %ls", extensionTrackingReady ? L"ready" : L"unavailable");

  Wh_Log(L"Extension container live propagation: %ls", extensionContainerTrackingReady ? L"ready" : L"unavailable");

  Wh_Log(L"Extension icon live regeneration: %ls", g_ToolbarActionViewUpdateState ? L"ready" : L"unavailable");

  return true;
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

    if (InstallChromeHooks(chromeDll)) {
      if (!Wh_ApplyHookOperations()) {
        Wh_Log(L"Wh_ApplyHookOperations failed");
      }
    }
  }

  return module;
}

// -----------------------------------------------------------------------------
// Windhawk lifecycle
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
  PCWSTR commandLine = GetCommandLineW();

  if (wcsstr(commandLine, L"--type=")) {
    return FALSE;
  }

  // Skip automation/CDP-controlled Chrome instances.
  if (wcsstr(commandLine, L"--remote-debugging-pipe")) {
    Wh_Log(L"Skipping CDP-controlled Chrome process");
    return FALSE;
  }

  LoadSettings();

  Wh_Log(L"Main Chrome process, PID=%lu", GetCurrentProcessId());

  HMODULE chromeDll = GetModuleHandleW(L"chrome.dll");

  if (chromeDll) {
    Wh_Log(L"chrome.dll already loaded");

    return InstallChromeHooks(chromeDll);
  }

  Wh_Log(L"chrome.dll not loaded yet; waiting for LoadLibraryExW");

  if (!WindhawkUtils::SetFunctionHook(LoadLibraryExW, LoadLibraryExWHook, &g_LoadLibraryExWOriginal)) {
    Wh_Log(L"Failed to hook LoadLibraryExW");

    return FALSE;
  }

  return TRUE;
}

void Wh_ModAfterInit() {
  if (g_chromeSetupStarted.load()) return;

  HMODULE chromeDll = GetModuleHandleW(L"chrome.dll");

  if (!chromeDll) return;

  if (InstallChromeHooks(chromeDll)) {
    Wh_ApplyHookOperations();
  }
}

void Wh_ModSettingsChanged() {
  bool oldUseWindowsFolderIcon = g_useWindowsFolderIcon.load(std::memory_order_relaxed);

  LoadSettings();

  if (!g_hooksResolved.load()) return;

  ApplyFontToExistingBookmarkLabels();

  if (oldUseWindowsFolderIcon != g_useWindowsFolderIcon.load(std::memory_order_relaxed)) {
    ApplyFolderIconToExistingBookmarkBars();
  }

  ApplyTweaksToExistingTabs();
  ApplyWidthToExistingExtensionButtons();
}

void Wh_ModUninit() {
  Wh_Log(L"Chrome Native UI Tweaks unloaded");
}

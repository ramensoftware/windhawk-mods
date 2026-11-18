// ==WindhawkMod==
// @id              simple-wallpaper-switcher
// @name            Simple Wallpaper Switcher
// @description     Browse your wallpaper folders, preview thumbnails, and switch with smooth transitions.
// @version         0.1.3
// @author          HHeitlager
// @github          https://github.com/hheitlager
// @include         windhawk.exe
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lole32 -loleaut32 -lruntimeobject -lshell32 -lcomctl32 -lshlwapi -luuid -ldwmapi -luxtheme -lgdi32 -lmsimg32 -ld2d1 -lwindowscodecs -lgdiplus -ladvapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Simple Wallpaper Switcher

![Simple Wallpaper Switcher showing the picker and wallpaper changes](https://raw.githubusercontent.com/hheitlager/simple-wallpaper-switcher/main/assets/simple-wallpaper-switcher-banner.gif)

## Screenshots
| Solid | Blurred |
|:---:|:---:|
| ![Solid](https://raw.githubusercontent.com/hheitlager/simple-wallpaper-switcher/main/assets/simple-wallpaper-switcher-solid-material.png) | ![Blurred](https://raw.githubusercontent.com/hheitlager/simple-wallpaper-switcher/main/assets/simple-wallpaper-switcher-blur-material.png) |

| Acrylic | Quick-menu |
|:---:|:---:|
| ![Acrylic](https://raw.githubusercontent.com/hheitlager/simple-wallpaper-switcher/main/assets/simple-wallpaper-switcher-acrylic-material.png) | ![Quick-menu](https://raw.githubusercontent.com/hheitlager/simple-wallpaper-switcher/main/assets/simple-wallpaper-switcher-taskbar-quick-menu.png) |

### Your wallpaper collection, ready when you are.

Browse your wallpapers in a compact visual picker, choose a new look, and watch it transition onto your desktop.

## Find your next wallpaper

See your collection as a grid of thumbnails. Browse folders with the breadcrumb menu, click a wallpaper to apply it, or use the keyboard to move through your collection.

## Stay in the flow

Choose another wallpaper while a transition is running. The effect retargets to your latest choice, so you can keep browsing without waiting for each change to finish.

## Make it yours

Choose from the shared Emberveil themes and set the picker background to Solid, Blur, or Acrylic. Pick a transition effect, or let Random choose one for you.

## Get started

Set your wallpaper folder in the mod settings, then open the picker from its tray icon or keyboard shortcut.

Click a thumbnail to apply it. Use the arrow keys to move, Enter or Space to apply, and the mouse wheel to change pages. Click the magnifying glass in the breadcrumb, or start typing, to filter filenames in the current folder. Press Escape to clear the search.

Supported image formats: JPEG, PNG, BMP, and WebP.


*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- wallpaperFolder: ""
  $name: Wallpaper folder
  $description: Folder containing your wallpaper images
- wallpaperTransition: random
  $name: Wallpaper transition
  $description: Effect used when a wallpaper changes
  $options:
  - random: Random (avoid repeating the previous effect)
  - fade: Fade
  - circle: Circle
  - slide: Slide
  - zoom: Zoom
  - wipe: Wipe (random direction)
  - dissolve: Dissolve
  - blur-fade: Blur Fade
  - split-reveal: Split Reveal (random orientation)
- toggleHotkey: "ctrl+alt+87"
  $name: Open picker hotkey
  $description: Click to record a shortcut. If it cannot be registered, Ctrl+Alt+W is tried as a fallback. Leave empty to disable the hotkey.
  $format: hotkey
- Appearance:
  - Theme: system
    $name: Theme
    $description: Use Windows colors or a shared theme palette.
    $options:
      - system: Windows / System
      - monochrome: Monochrome
      - catppuccin: Catppuccin Mocha
      - catppuccin-latte: Catppuccin Latte
      - emberveil: Emberveil
      - emberveil-midnight: Emberveil Midnight
      - emberveil-daybreak: Emberveil Daybreak
      - emerald: Emerald
      - dracula: Dracula
      - dracula-light: Dracula Light
      - nord: Nord
      - nord-light: Nord Light
      - gruvbox: Gruvbox Dark
      - gruvbox-light: Gruvbox Light
      - tokyo-night: Tokyo Night
      - tokyo-night-day: Tokyo Night Day
      - solarized-dark: Solarized Dark
      - solarized-light: Solarized Light
      - rose-pine: Rosé Pine
      - everforest: Everforest
      - synthwave-84: SynthWave '84
      - flexoki: Flexoki
      - kanagawa: Kanagawa
      - ayu: Ayu
      - one-dark: One Dark
  - AutomaticTheme: false
    $name: Automatic light/dark theme
    $description: Switch matching theme variants when Windows changes between light and dark mode.
  - BackgroundMaterial: solid
    $name: Background material
    $description: Choose a solid, blurred, or acrylic window background. Blur and Acrylic fall back to Solid if Windows composition is unavailable.
    $options:
      - solid: Solid
      - blur: Blur
      - acrylic: Acrylic
- Picker:
  - Position: center
    $name: Picker position
    $description: Choose where the picker opens within the monitor's usable area.
    $options:
      - top-left: Top left
      - top-center: Top center
      - top-right: Top right
      - middle-left: Middle left
      - center: Center
      - middle-right: Middle right
      - bottom-left: Bottom left
      - bottom-center: Bottom center
      - bottom-right: Bottom right
  - OffsetX: 0
    $name: Horizontal offset
    $description: Move the picker left or right from its anchor position, in pixels. Positive values move right.
  - OffsetY: 0
    $name: Vertical offset
    $description: Move the picker up or down from its anchor position, in pixels. Positive values move down.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winreg.h>
#include <propidl.h>
#include <gdiplus.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <d2d1.h>
#include <dxgi.h>
#include <propsys.h>
#include <wincodec.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <uxtheme.h>
#include <windhawk_api.h>
#include <windhawk_tool_mod.h>
#include <winrt/base.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cwchar>
#include <memory>
#include <limits>
#include <random>
#include <string>
#include <vector>

namespace {

constexpr UINT kTrayCallback = WM_APP + 1;
constexpr UINT kChooseFolder = 1003;
constexpr UINT kFolderLabelId = 3001;
constexpr UINT kPageLabelId = 3002;
constexpr UINT kSearchEditId = 3003;
constexpr UINT kTrayOpen = 2001;
constexpr UINT kTrayPrevious = 2002;
constexpr UINT kTrayNext = 2003;
constexpr UINT kTrayExit = 2004;
constexpr UINT kTraySeparator = 2005;
constexpr UINT kHotkeyId = 1;
constexpr UINT kReloadSettings = WM_APP + 10;
constexpr UINT kWallpaperChangeDone = WM_APP + 11;
constexpr UINT kImageListReady = WM_APP + 12;
constexpr UINT kThumbnailPageReady = WM_APP + 13;
constexpr UINT_PTR kSearchFilterTimer = 0xB743;
constexpr UINT kSearchFilterDelayMs = 160;
constexpr UINT kFirstTileId = 4000;
constexpr size_t kPageSize = 12;
constexpr size_t kThumbnailCachePages = 3;
constexpr size_t kMaxThumbnailWorkers = 3;
constexpr size_t kGridColumns = 4;
constexpr UINT kThumbnailWidth = 128;
constexpr UINT kThumbnailHeight = 76;
constexpr int kThumbnailCornerDiameter = 16;
constexpr int kSelectedBorderWidth = 3;
constexpr int kBreadcrumbSearchButtonWidth = 30;
constexpr int kBreadcrumbSearchEditWidth = 150;
constexpr int kBreadcrumbSearchEditInset = 5;
constexpr int kSearchTextPadding = 7;
constexpr int kPickerEdgeMargin = 12;
constexpr PCWSTR kWindowClass = L"SimpleWallpaperSwitcherWindow";
constexpr PCWSTR kBackdropWindowClass = L"SimpleWallpaperSwitcherBackdrop";

COLORREF g_background = RGB(48, 45, 71);
COLORREF g_surface = RGB(68, 64, 90);
COLORREF g_text = RGB(205, 214, 244);
COLORREF g_muted = RGB(166, 173, 200);
COLORREF g_accent = RGB(207, 161, 255);
std::wstring g_configuredTheme = L"system";
std::wstring g_configuredMaterial = L"solid";
bool g_automaticTheme = false;
std::wstring g_pickerPosition = L"center";
int g_pickerOffsetX = 0;
int g_pickerOffsetY = 0;
HWND g_pickerBackdropWindow = nullptr;

HWND g_hostWindow;
HWND g_pickerWindow;
HWND g_tiles[kPageSize];
HWND g_folderLabel;
HWND g_pageLabel;
HWND g_searchEdit;
HWND g_previousForegroundWindow;
struct BreadcrumbSegment {
    std::wstring label;
    std::wstring folderPath;
    bool isFolder;
};
struct BreadcrumbHitTarget {
    RECT labelRect;
    RECT menuRect;
    RECT hoverRect;
    size_t segmentIndex;
    bool hasMenu;
};
std::vector<BreadcrumbSegment> g_breadcrumbSegments;
std::vector<BreadcrumbHitTarget> g_breadcrumbHitTargets;
int g_openBreadcrumbMenuSegment = -1;
int g_hoverBreadcrumbSegment = -1;
int g_animatedBreadcrumbMenuSegment = -1;
float g_breadcrumbChevronAngle = 0.0f;
float g_breadcrumbChevronTargetAngle = 0.0f;
constexpr UINT_PTR kBreadcrumbMenuDismissTimer = 0xB741;
constexpr UINT_PTR kBreadcrumbChevronAnimationTimer = 0xB742;
bool g_breadcrumbMenuDismissActive = false;
bool g_breadcrumbMenuCursorEntered = false;
POINT g_breadcrumbMenuDismissAnchor{};
HHOOK g_breadcrumbMenuMouseHook = nullptr;
ULONGLONG g_breadcrumbMenuOutsideSince = 0;
int g_suppressedBreadcrumbClickSegment = -1;
ULONGLONG g_suppressBreadcrumbClickUntil = 0;
bool g_pickerCloseAfterBreadcrumbMenu = false;
HANDLE g_uiThread;
DWORD g_uiThreadId;
HANDLE g_readyEvent;
UINT g_hotkeyModifiers = MOD_CONTROL | MOD_ALT;
UINT g_hotkeyKey = 'W';
std::wstring g_folder;
std::wstring g_lastSelectedPath;
std::vector<std::wstring> g_images;
std::vector<size_t> g_visibleImageIndices;
std::wstring g_searchQuery;
std::wstring g_pendingSearchQuery;
RECT g_breadcrumbSearchButtonRect{};
bool g_searchExpanded = false;
bool g_searchButtonHovered = false;
bool g_searchButtonClickHandled = false;
bool g_folderUnavailable;
bool g_imageScanInProgress;
std::atomic<std::uint64_t> g_folderGeneration{0};
struct CachedThumbnailPage {
    size_t page;
    std::vector<HBITMAP> bitmaps;
};
struct PendingThumbnailPage {
    std::uint64_t generation;
    size_t page;
};
std::vector<CachedThumbnailPage> g_thumbnailCache;
std::vector<PendingThumbnailPage> g_pendingThumbnailPages;
std::vector<HANDLE> g_backgroundThreads;
SRWLOCK g_backgroundPostLock = SRWLOCK_INIT;
size_t g_page;
size_t g_viewAnchor;
int g_currentImage = -1;
int g_selectedImage = -1;
HBRUSH g_backgroundBrush;
HBRUSH g_searchBrush;
void HidePickerBackdrop();
void HidePickerWindow(HWND hwnd);
void PositionPickerBackdrop();
void ApplyPickerMaterial(HWND hwnd);
void RefreshPickerAppearance(HWND hwnd);
HFONT g_pickerFont;
HFONT g_loadingFont;
HFONT g_breadcrumbIconFont;
ULONG_PTR g_gdiplusToken;
HICON g_trayIcon;
HANDLE g_wallpaperThread;
bool g_transitionActive;
int g_transitionMode = 8;  // Random
int g_lastRandomTransition = -1;
std::wstring g_transitionPath;
std::atomic<bool> g_shuttingDown{false};

enum class WallpaperTransition {
    Fade, Circle, Slide, Zoom, Wipe, Dissolve, BlurFade, SplitReveal
};

struct WallpaperTargetRequest {
    int imageIndex = -1;
    std::wstring path;
    POINT origin{};
    WallpaperTransition transition = WallpaperTransition::Fade;
    int wipeDirection = 0;
    std::uint32_t dissolveSeed = 0;
    int splitRevealDirection = 0;
    std::uint64_t generation = 0;
};
SRWLOCK g_wallpaperRequestLock = SRWLOCK_INIT;
WallpaperTargetRequest g_latestWallpaperRequest;
RECT g_transitionMonitor{};
bool g_transitionCurrentSuperseded = false;
NOTIFYICONDATA g_notifyIcon{};

struct ImageScanJob {
    HWND notifyWindow;
    std::wstring folder;
    std::uint64_t generation;
};

struct ImageScanResult {
    std::uint64_t generation;
    bool folderUnavailable;
    std::vector<std::wstring> images;
};

struct ThumbnailPageJob {
    HWND notifyWindow;
    std::uint64_t generation;
    size_t page;
    std::vector<std::wstring> paths;
};

struct ThumbnailPageResult {
    std::uint64_t generation;
    size_t page;
    std::vector<HBITMAP> bitmaps;
};

void StartImageScan();
void RefreshList();
void ShowPicker();
void ClearThumbnailCache();
void SetViewAnchor(size_t image);
void ApplyImageIndex(int image);
bool HandlePickerKey(UINT key);
void StepPage(int step);
size_t DisplayImageCount();
void RebuildSearchResults(bool invalidateThumbnailGeneration = true);
void SetSearchExpanded(bool expanded, WCHAR initialCharacter = L'\0');
void UpdateSearchFromEdit();
void ApplyPendingSearch();
void FocusSelectedResult();
LRESULT CALLBACK SearchEditSubclassProc(HWND hwnd, UINT message,
                                        WPARAM wParam, LPARAM lParam,
                                        UINT_PTR subclassId,
                                        DWORD_PTR referenceData);

std::wstring ReadStringSetting(PCWSTR name, size_t capacity = 2048) {
    std::wstring value(capacity, L'\0');
    if (!Wh_GetStringValue(name, value.data(), (DWORD)value.size())) {
        return {};
    }
    value.resize(wcsnlen(value.c_str(), value.size()));
    return value;
}

std::wstring ReadConfiguredStringSetting(PCWSTR name) {
    PCWSTR setting = Wh_GetStringSetting(name);
    if (!setting) {
        return {};
    }

    std::wstring value(setting);
    Wh_FreeStringSetting(setting);
    return value;
}

struct ThemeColors {
    unsigned background;
    unsigned surface;
    unsigned accent;
    unsigned secondary;
    unsigned text;
    unsigned muted;
};

COLORREF ThemeHexToColor(unsigned hex) {
    return RGB((hex >> 16) & 0xff, (hex >> 8) & 0xff, hex & 0xff);
}

unsigned ColorToThemeHex(COLORREF color) {
    return (static_cast<unsigned>(GetRValue(color)) << 16) |
           (static_cast<unsigned>(GetGValue(color)) << 8) |
           static_cast<unsigned>(GetBValue(color));
}

COLORREF MixThemeColors(COLORREF first, COLORREF second,
                        unsigned secondPercent) {
    const unsigned firstPercent = 100 - secondPercent;
    return RGB((GetRValue(first) * firstPercent +
                GetRValue(second) * secondPercent + 50) / 100,
               (GetGValue(first) * firstPercent +
                GetGValue(second) * secondPercent + 50) / 100,
               (GetBValue(first) * firstPercent +
                GetBValue(second) * secondPercent + 50) / 100);
}

bool IsWindowsLightMode() {
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                      0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
        return false;
    }
    DWORD value = 0;
    DWORD valueSize = sizeof(value);
    const LONG result = RegQueryValueExW(
        key, L"AppsUseLightTheme", nullptr, nullptr,
        reinterpret_cast<BYTE*>(&value), &valueSize);
    RegCloseKey(key);
    return result == ERROR_SUCCESS && value != 0;
}

std::wstring GetEffectivePickerTheme() {
    std::wstring theme = g_configuredTheme.empty()
                             ? L"system"
                             : g_configuredTheme;
    if (!g_automaticTheme) {
        return theme;
    }
    const bool light = IsWindowsLightMode();
    if (theme == L"monochrome")
        return light ? L"monochrome-light" : L"monochrome";
    if (theme == L"catppuccin" || theme == L"catppuccin-latte")
        return light ? L"catppuccin-latte" : L"catppuccin";
    if (theme == L"emberveil" || theme == L"emberveil-daybreak")
        return light ? L"emberveil-daybreak" : L"emberveil";
    if (theme == L"dracula" || theme == L"dracula-light")
        return light ? L"dracula-light" : L"dracula";
    if (theme == L"nord" || theme == L"nord-light")
        return light ? L"nord-light" : L"nord";
    if (theme == L"gruvbox" || theme == L"gruvbox-light")
        return light ? L"gruvbox-light" : L"gruvbox";
    if (theme == L"tokyo-night" || theme == L"tokyo-night-day")
        return light ? L"tokyo-night-day" : L"tokyo-night";
    if (theme == L"solarized-dark" || theme == L"solarized-light")
        return light ? L"solarized-light" : L"solarized-dark";
    return theme;
}

ThemeColors GetPickerThemeColors(const std::wstring& theme) {
    if (theme == L"system") {
        const bool light = IsWindowsLightMode();
        const COLORREF accent = GetSysColor(COLOR_HIGHLIGHT);
        return {light ? 0xf3f3f3u : 0x1f1f1fu,
                light ? 0xf9f9f9u : 0x252525u,
                ColorToThemeHex(accent), ColorToThemeHex(accent),
                light ? 0x202020u : 0xf4f4f4u,
                light ? 0x666666u : 0xb8b8b8u};
    }
    if (theme == L"monochrome-light")
        return {0xf2f2f2, 0xf2f2f2, 0x404040, 0x666666, 0x202020, 0x666666};
    if (theme == L"monochrome")
        return {0x202020, 0x202020, 0xd0d0d0, 0xb8b8b8, 0xf4f4f4, 0xb8b8b8};
    if (theme == L"catppuccin")
        return {0x211e32, 0x302d47, 0xcfa1ff, 0x89dceb, 0xcdd6f4, 0xa6adc8};
    if (theme == L"catppuccin-latte")
        return {0xeff1f5, 0xe6e9ef, 0x8839ef, 0x179299, 0x4c4f69, 0x6c6f85};
    if (theme == L"emberveil")
        return {0x171b20, 0x171b20, 0xffbc5e, 0xd9a441, 0xf5e6c8, 0xb8a98a};
    if (theme == L"emberveil-midnight")
        return {0x0a0c0e, 0x0a0c0e, 0xffbc5e, 0xb8a98a, 0xf5e6c8, 0xb8a98a};
    if (theme == L"emberveil-daybreak")
        return {0xfff2d8, 0xfff2d8, 0xb87924, 0x8a642d, 0x1a1f26, 0x8a642d};
    if (theme == L"emerald")
        return {0x102b27, 0x102b27, 0xd4af70, 0xb6c9bd, 0xf3e8cf, 0xb6c9bd};
    if (theme == L"dracula-light")
        return {0xf8f7fc, 0xeae7f2, 0x7545b5, 0xb72f79, 0x292537, 0x655d77};
    if (theme == L"nord-light")
        return {0xeceff4, 0xe5e9f0, 0x3b668b, 0x5e81ac, 0x2e3440, 0x4c566a};
    if (theme == L"gruvbox-light")
        return {0xfbf1c7, 0xebdbb2, 0x9d0006, 0xb57614, 0x3c3836, 0x665c54};
    if (theme == L"tokyo-night-day")
        return {0xe1e2e7, 0xd5d6db, 0x2959aa, 0x7847bd, 0x343b58, 0x61709a};
    if (theme == L"dracula")
        return {0x282a36, 0x44475a, 0xbd93f9, 0xff79c6, 0xf8f8f2, 0xbfbfd0};
    if (theme == L"nord")
        return {0x2e3440, 0x4c566a, 0x88c0d0, 0xa3be8c, 0xeceff4, 0xd8dee9};
    if (theme == L"gruvbox")
        return {0x282828, 0x504945, 0xfabd2f, 0xfe8019, 0xebdbb2, 0xa89984};
    if (theme == L"tokyo-night")
        return {0x1a1b26, 0x414868, 0x7aa2f7, 0xbb9af7, 0xc0caf5, 0xa9b1d6};
    if (theme == L"solarized-light")
        return {0xfdf6e3, 0xeee8d5, 0x268bd2, 0x859900, 0x657b83, 0x586e75};
    if (theme == L"solarized-dark")
        return {0x002b36, 0x073642, 0x268bd2, 0x859900, 0x839496, 0x586e75};
    if (theme == L"rose-pine")
        return {0x191724, 0x403b50, 0xeb6f92, 0xf6c177, 0xe0def4, 0x908caa};
    if (theme == L"everforest")
        return {0x2d353b, 0x475258, 0xa7c080, 0xdbbc7f, 0xd3c6aa, 0x9da9a0};
    if (theme == L"synthwave-84")
        return {0x0e0b1f, 0x1a1038, 0xff2d9c, 0x00e5ff, 0xf8f8ff, 0xb9a6ff};
    if (theme == L"flexoki")
        return {0xfffcf0, 0xdad8ce, 0x205ea6, 0xbc5215, 0x100f0f, 0x6f6e69};
    if (theme == L"kanagawa")
        return {0x1f1f28, 0x54546d, 0x98bb6c, 0xe6c384, 0xdcd7ba, 0xc8c093};
    if (theme == L"ayu")
        return {0x0b0f14, 0x39424c, 0xffb454, 0x59c2ff, 0xe6e1cf, 0xa6a39b};
    return {0x282c34, 0x4b5263, 0x61afef, 0x98c379, 0xabb2bf, 0x9da5b4};
}

void ApplyPickerThemeColors() {
    const ThemeColors colors = GetPickerThemeColors(GetEffectivePickerTheme());
    g_background = ThemeHexToColor(colors.surface);
    g_surface = MixThemeColors(g_background, ThemeHexToColor(colors.text), 12);
    g_accent = ThemeHexToColor(colors.accent);
    g_text = ThemeHexToColor(colors.text);
    g_muted = ThemeHexToColor(colors.muted);
    if (_wcsicmp(g_configuredMaterial.c_str(), L"blur") == 0) {
        // Blur leaves more of the desktop visible than Acrylic. Brighten its
        // labels so they keep contrast over both light and dark content.
        g_text = MixThemeColors(g_text, RGB(255, 255, 255), 45);
        g_muted = MixThemeColors(g_muted, RGB(255, 255, 255), 72);
    }
}

void LoadHotkeySetting() {
    PCWSTR setting = Wh_GetStringSetting(L"toggleHotkey");
    const std::wstring value = setting ? setting : L"ctrl+alt+87";
    if (setting) {
        Wh_FreeStringSetting(setting);
    }

    g_hotkeyModifiers = 0;
    g_hotkeyKey = 0;
    bool valid = true;
    bool keySeen = false;
    size_t start = 0;
    while (start < value.size()) {
        const size_t separator = value.find(L'+', start);
        const size_t end = separator == std::wstring::npos
                               ? value.size()
                               : separator;
        const std::wstring token = value.substr(start, end - start);
        if (_wcsicmp(token.c_str(), L"ctrl") == 0 && !keySeen) {
            g_hotkeyModifiers |= MOD_CONTROL;
        } else if (_wcsicmp(token.c_str(), L"alt") == 0 && !keySeen) {
            g_hotkeyModifiers |= MOD_ALT;
        } else if (_wcsicmp(token.c_str(), L"shift") == 0 && !keySeen) {
            g_hotkeyModifiers |= MOD_SHIFT;
        } else if (_wcsicmp(token.c_str(), L"win") == 0 && !keySeen) {
            g_hotkeyModifiers |= MOD_WIN;
        } else {
            wchar_t* parsedEnd = nullptr;
            const unsigned long key = std::wcstoul(token.c_str(), &parsedEnd, 10);
            if (keySeen || token.empty() || parsedEnd == token.c_str() ||
                !parsedEnd || *parsedEnd != L'\0' || key == 0 || key > 255) {
                valid = false;
                break;
            }
            g_hotkeyKey = (UINT)key;
            keySeen = true;
        }

        if (separator == std::wstring::npos) {
            break;
        }
        start = separator + 1;
    }

    if (!valid || (!value.empty() && !keySeen)) {
        Wh_Log(L"Invalid toggleHotkey setting; using Ctrl+Alt+W");
        g_hotkeyModifiers = MOD_CONTROL | MOD_ALT;
        g_hotkeyKey = 'W';
    }
}

bool RegisterPickerHotkey(HWND hwnd) {
    UnregisterHotKey(hwnd, kHotkeyId);
    if (g_hotkeyKey == 0) {
        return true;
    }
    if (RegisterHotKey(hwnd, kHotkeyId,
                       g_hotkeyModifiers | MOD_NOREPEAT,
                       g_hotkeyKey)) {
        return true;
    }

    const DWORD error = GetLastError();
    constexpr UINT fallbackModifiers = MOD_CONTROL | MOD_ALT;
    constexpr UINT fallbackKey = 'W';
    if (g_hotkeyModifiers == fallbackModifiers &&
        g_hotkeyKey == fallbackKey) {
        return false;
    }

    Wh_Log(L"Configured hotkey registration failed (error %lu); trying Ctrl+Alt+W fallback",
           error);
    if (RegisterHotKey(hwnd, kHotkeyId,
                       fallbackModifiers | MOD_NOREPEAT,
                       fallbackKey)) {
        return true;
    }
    Wh_Log(L"Ctrl+Alt+W fallback registration also failed (error %lu)",
           GetLastError());
    return false;
}

void LoadSettings() {
    g_configuredTheme = ReadConfiguredStringSetting(L"Appearance.Theme");
    if (g_configuredTheme.empty()) {
        g_configuredTheme = L"system";
    }
    g_automaticTheme =
        Wh_GetIntSetting(L"Appearance.AutomaticTheme") != 0;
    g_configuredMaterial =
        ReadConfiguredStringSetting(L"Appearance.BackgroundMaterial");
    if (g_configuredMaterial.empty()) {
        g_configuredMaterial = L"solid";
    }
    g_pickerPosition = ReadConfiguredStringSetting(L"Picker.Position");
    if (g_pickerPosition.empty()) {
        g_pickerPosition = L"center";
    }
    g_pickerOffsetX = std::clamp(Wh_GetIntSetting(L"Picker.OffsetX"),
                                 -1000, 1000);
    g_pickerOffsetY = std::clamp(Wh_GetIntSetting(L"Picker.OffsetY"),
                                 -1000, 1000);

    const std::wstring configuredFolder =
        ReadConfiguredStringSetting(L"wallpaperFolder");
    const std::wstring lastConfiguredFolder =
        ReadStringSetting(L"wallpaperFolderSettingSnapshot");
    const std::wstring storedRuntimeFolder =
        ReadStringSetting(L"wallpaperFolder");

    if (configuredFolder != lastConfiguredFolder) {
        // The Windhawk folder setting changed since it was last loaded, so it
        // becomes the active folder. Keep a snapshot to distinguish this from
        // a newer folder chosen inside the picker on a later restart.
        g_folder = configuredFolder;
        Wh_SetStringValue(L"wallpaperFolder", g_folder.c_str());
        Wh_SetStringValue(L"wallpaperFolderSettingSnapshot",
                          configuredFolder.c_str());
    } else {
        // No Settings change: preserve the most recently chosen folder from
        // the picker's value store, falling back to the configured path if
        // there is no saved runtime value yet.
        g_folder = storedRuntimeFolder;
        if (g_folder.empty()) {
            g_folder = configuredFolder;
        }
    }
    g_lastSelectedPath = ReadStringSetting(L"lastSelectedWallpaper");
    LoadHotkeySetting();
    PCWSTR transitionSetting = Wh_GetStringSetting(L"wallpaperTransition");
    const std::wstring transition = transitionSetting
                                        ? transitionSetting
                                        : L"random";
    if (transitionSetting) {
        Wh_FreeStringSetting(transitionSetting);
    }
    if (transition.empty() || _wcsicmp(transition.c_str(), L"random") == 0) {
        g_transitionMode = 8;
    } else if (_wcsicmp(transition.c_str(), L"fade") == 0) {
        g_transitionMode = 0;
    } else if (_wcsicmp(transition.c_str(), L"circle") == 0) {
        g_transitionMode = 1;
    } else if (_wcsicmp(transition.c_str(), L"slide") == 0) {
        g_transitionMode = 2;
    } else if (_wcsicmp(transition.c_str(), L"zoom") == 0) {
        g_transitionMode = 3;
    } else if (_wcsicmp(transition.c_str(), L"wipe") == 0) {
        g_transitionMode = 4;
    } else if (_wcsicmp(transition.c_str(), L"dissolve") == 0) {
        g_transitionMode = 5;
    } else if (_wcsicmp(transition.c_str(), L"blur-fade") == 0) {
        g_transitionMode = 6;
    } else if (_wcsicmp(transition.c_str(), L"split-reveal") == 0) {
        g_transitionMode = 7;
    } else {
        g_transitionMode = 8;
    }
}

bool IsImageFile(PCWSTR filename) {
    static constexpr PCWSTR extensions[] = {
        L".jpg", L".jpeg", L".jfif", L".png", L".bmp", L".webp"};
    PCWSTR extension = PathFindExtension(filename);
    for (PCWSTR candidate : extensions) {
        if (_wcsicmp(extension, candidate) == 0) {
            return true;
        }
    }
    return false;
}

bool ImagePathLess(const std::wstring& left, const std::wstring& right) {
    // All paths share the selected root, so comparing full paths naturally
    // groups subfolders while preserving numeric filename ordering.
    return StrCmpLogicalW(left.c_str(), right.c_str()) < 0;
}

bool PostBackgroundMessage(HWND window, UINT message, WPARAM wParam,
                           LPARAM lParam) {
    AcquireSRWLockShared(&g_backgroundPostLock);
    const bool posted = !g_shuttingDown.load() && window &&
                        PostMessageW(window, message, wParam, lParam);
    ReleaseSRWLockShared(&g_backgroundPostLock);
    return posted;
}

void ReapBackgroundThreads() {
    for (auto it = g_backgroundThreads.begin();
         it != g_backgroundThreads.end();) {
        if (WaitForSingleObject(*it, 0) == WAIT_OBJECT_0) {
            CloseHandle(*it);
            it = g_backgroundThreads.erase(it);
        } else {
            ++it;
        }
    }
}

bool StartBackgroundThread(LPTHREAD_START_ROUTINE procedure,
                           void* parameter) {
    ReapBackgroundThreads();
    HANDLE thread = CreateThread(nullptr, 0, procedure, parameter, 0, nullptr);
    if (!thread) {
        return false;
    }
    g_backgroundThreads.push_back(thread);
    return true;
}

void WaitForBackgroundThreads() {
    for (HANDLE thread : g_backgroundThreads) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
    g_backgroundThreads.clear();
}

void FreeThumbnailPageResult(ThumbnailPageResult* result) {
    for (HBITMAP bitmap : result->bitmaps) {
        if (bitmap) {
            DeleteObject(bitmap);
        }
    }
    delete result;
}

DWORD WINAPI ImageListThreadProc(void* parameter) {
    std::unique_ptr<ImageScanJob> job((ImageScanJob*)parameter);
    std::unique_ptr<ImageScanResult> result(new ImageScanResult{
        job->generation, false, {}});

    const DWORD rootAttributes = GetFileAttributesW(job->folder.c_str());
    if (rootAttributes == INVALID_FILE_ATTRIBUTES ||
        !(rootAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        result->folderUnavailable = true;
    } else {
        std::vector<std::wstring> pendingFolders{job->folder};
        bool rootWasReadable = false;
        bool cancelled = false;
        while (!pendingFolders.empty()) {
            if (g_shuttingDown.load() ||
                job->generation != g_folderGeneration.load()) {
                cancelled = true;
                break;
            }

            std::wstring folder = std::move(pendingFolders.back());
            pendingFolders.pop_back();
            std::wstring pattern = folder;
            if (!pattern.empty() && pattern.back() != L'\\' &&
                pattern.back() != L'/') {
                pattern += L'\\';
            }
            pattern += L'*';

            WIN32_FIND_DATAW data{};
            HANDLE find = FindFirstFileExW(
                pattern.c_str(), FindExInfoBasic, &data,
                FindExSearchNameMatch, nullptr, FIND_FIRST_EX_LARGE_FETCH);
            if (find == INVALID_HANDLE_VALUE) {
                if (folder == job->folder) {
                    result->folderUnavailable = true;
                }
                continue;
            }
            if (folder == job->folder) {
                rootWasReadable = true;
            }

            const size_t prefixLength = pattern.size() - 1;
            do {
                if (g_shuttingDown.load() ||
                    job->generation != g_folderGeneration.load()) {
                    cancelled = true;
                    break;
                }
                if (wcscmp(data.cFileName, L".") == 0 ||
                    wcscmp(data.cFileName, L"..") == 0) {
                    continue;
                }

                if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    if (!(data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
                        std::wstring child = pattern.substr(0, prefixLength);
                        child += data.cFileName;
                        pendingFolders.push_back(std::move(child));
                    }
                    continue;
                }
                if (!IsImageFile(data.cFileName)) {
                    continue;
                }

                std::wstring path = pattern.substr(0, prefixLength);
                path += data.cFileName;
                result->images.push_back(std::move(path));
            } while (FindNextFileW(find, &data));
            FindClose(find);
            if (cancelled) {
                break;
            }
        }
        if (cancelled) {
            return 0;
        }
        if (!rootWasReadable) {
            result->folderUnavailable = true;
        }
        std::sort(result->images.begin(), result->images.end(), ImagePathLess);
    }

    if (g_shuttingDown.load() ||
        job->generation != g_folderGeneration.load()) {
        return 0;
    }

    ImageScanResult* completed = result.release();
    if (!PostBackgroundMessage(job->notifyWindow, kImageListReady, 0,
                               (LPARAM)completed)) {
        delete completed;
    }
    return 0;
}
HBITMAP CreateThumbnailBitmap(IWICImagingFactory* factory, PCWSTR path);

DWORD WINAPI ThumbnailPageThreadProc(void* parameter) {
    std::unique_ptr<ThumbnailPageJob> job((ThumbnailPageJob*)parameter);
    const HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    winrt::com_ptr<IWICImagingFactory> factory;
    if (SUCCEEDED(com)) {
        CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                         CLSCTX_INPROC_SERVER,
                         IID_PPV_ARGS(factory.put()));
    }

    std::vector<HBITMAP> bitmaps(kPageSize, nullptr);
    for (size_t slot = 0; factory && slot < job->paths.size() &&
                           slot < kPageSize; ++slot) {
        if (g_shuttingDown.load() ||
            job->generation != g_folderGeneration.load()) {
            break;
        }
        if (job->paths[slot].empty()) {
            continue;
        }
        bitmaps[slot] = CreateThumbnailBitmap(factory.get(),
                                               job->paths[slot].c_str());
    }
    factory = nullptr;
    if (SUCCEEDED(com)) {
        CoUninitialize();
    }

    ThumbnailPageResult* result = new ThumbnailPageResult{
        job->generation, job->page, std::move(bitmaps)};
    if (!PostBackgroundMessage(job->notifyWindow, kThumbnailPageReady, 0,
                               (LPARAM)result)) {
        FreeThumbnailPageResult(result);
    }
    return 0;
}
void PersistFolder() {
    Wh_SetStringValue(L"wallpaperFolder", g_folder.c_str());
}

void PersistSelectedImage(int image) {
    if (image < 0 || image >= (int)g_images.size()) {
        return;
    }
    g_selectedImage = image;
    g_lastSelectedPath = g_images[(size_t)image];
    Wh_SetStringValue(L"lastSelectedWallpaper", g_lastSelectedPath.c_str());
}

std::wstring FolderDisplayName() {
    std::wstring path = g_folder;
    while (path.size() > 3 &&
           (path.back() == L'\\' || path.back() == L'/')) {
        path.pop_back();
    }
    if (path.empty()) {
        return L"Choose a wallpaper folder";
    }
    const PCWSTR name = PathFindFileNameW(path.c_str());
    return name && *name ? std::wstring(name) : path;
}

std::wstring TrimFolderSeparators(std::wstring path) {
    while (path.size() > 3 &&
           (path.back() == L'\\' || path.back() == L'/')) {
        path.pop_back();
    }
    return path;
}

bool IsImageInsideFolder(const std::wstring& imagePath,
                         const std::wstring& folderPath) {
    const std::wstring folder = TrimFolderSeparators(folderPath);
    if (imagePath.size() <= folder.size() ||
        CompareStringOrdinal(imagePath.c_str(), (int)folder.size(),
                             folder.c_str(), (int)folder.size(), TRUE) !=
            CSTR_EQUAL) {
        return false;
    }
    return (!folder.empty() &&
            (folder.back() == L'\\' || folder.back() == L'/')) ||
           imagePath[folder.size()] == L'\\' ||
           imagePath[folder.size()] == L'/';
}

struct FolderOption {
    std::wstring name;
    std::wstring path;
};

std::vector<FolderOption> DirectImageFolders(const std::wstring& folderPath) {
    std::vector<FolderOption> children;
    const std::wstring folder = TrimFolderSeparators(folderPath);
    for (const std::wstring& imagePath : g_images) {
        if (imagePath.size() <= folder.size() ||
            CompareStringOrdinal(imagePath.c_str(), (int)folder.size(),
                                 folder.c_str(), (int)folder.size(), TRUE) !=
                CSTR_EQUAL) {
            continue;
        }
        size_t relativeStart = folder.size();
        if (!folder.empty() && folder.back() != L'\\' &&
            folder.back() != L'/') {
            if (imagePath[relativeStart] != L'\\' &&
                imagePath[relativeStart] != L'/') {
                continue;
            }
            ++relativeStart;
        }
        if (relativeStart >= imagePath.size()) {
            continue;
        }
        const size_t separator = imagePath.find_first_of(L"\\/", relativeStart);
        if (separator == std::wstring::npos || separator == relativeStart) {
            continue;  // This image is directly inside the folder.
        }
        const std::wstring name =
            imagePath.substr(relativeStart, separator - relativeStart);
        const bool alreadyAdded = std::any_of(
            children.begin(), children.end(), [&name](const FolderOption& child) {
                return CompareStringOrdinal(child.name.c_str(), -1,
                                            name.c_str(), -1, TRUE) ==
                       CSTR_EQUAL;
            });
        if (alreadyAdded) {
            continue;
        }
        std::wstring childPath = folder;
        if (!childPath.empty() && childPath.back() != L'\\' &&
            childPath.back() != L'/') {
            childPath += L'\\';
        }
        childPath += name;
        children.push_back({name, std::move(childPath)});
    }
    return children;
}

int FirstImageInFolder(const std::wstring& folderPath) {
    for (size_t i = 0; i < g_images.size(); ++i) {
        if (IsImageInsideFolder(g_images[i], folderPath)) {
            return (int)i;
        }
    }
    return -1;
}

void NavigateToFolder(const std::wstring& folderPath) {
    const int image = FirstImageInFolder(folderPath);
    if (image < 0) {
        return;
    }
    SetViewAnchor((size_t)image);
    g_selectedImage = image;
    g_page = 0;
    RefreshList();
    const size_t slot = 0;
    if (g_tiles[slot]) {
        SetFocus(g_tiles[slot]);
    }
}

void ApplyBreadcrumbMenuCorners() {
    // The native popup menu is a separate window with class #32768.
    // Ask DWM to use rounded corners for it.
    HWND menuWindow = FindWindowW(L"#32768", nullptr);
    if (!menuWindow) {
        return;
    }

    HMODULE dwmApi = LoadLibraryW(L"dwmapi.dll");
    if (!dwmApi) {
        return;
    }

    using DwmSetWindowAttributeFn =
        HRESULT(WINAPI*)(HWND, DWORD, LPCVOID, DWORD);

    auto setWindowAttribute = (DwmSetWindowAttributeFn)GetProcAddress(
        dwmApi, "DwmSetWindowAttribute");

    if (setWindowAttribute) {
        constexpr DWORD kDwmwaWindowCornerPreference = 33;
        constexpr DWORD kDwmwcpRound = 2;
        setWindowAttribute(menuWindow, kDwmwaWindowCornerPreference,
                           &kDwmwcpRound, sizeof(kDwmwcpRound));
    }

    FreeLibrary(dwmApi);
}

Gdiplus::Color ToGdiPlusColor(COLORREF color) {
    return Gdiplus::Color(255, GetRValue(color), GetGValue(color),
                          GetBValue(color));
}

void FillRoundedMenuItem(Gdiplus::Graphics& graphics, const RECT& rect,
                         float radius, COLORREF color) {
    const float x = (float)rect.left;
    const float y = (float)rect.top;
    const float width = (float)(rect.right - rect.left);
    const float height = (float)(rect.bottom - rect.top);

    if (width <= 0 || height <= 0) {
        return;
    }

    const float diameter =
        (radius * 2.0f < (width < height ? width : height))
            ? radius * 2.0f
            : (width < height ? width : height);

    Gdiplus::GraphicsPath path;
    path.AddArc(x, y, diameter, diameter, 180, 90);
    path.AddArc(x + width - diameter, y, diameter, diameter, 270, 90);
    path.AddArc(x + width - diameter, y + height - diameter, diameter,
                diameter, 0, 90);
    path.AddArc(x, y + height - diameter, diameter, diameter, 90, 90);
    path.CloseFigure();

    Gdiplus::SolidBrush brush(ToGdiPlusColor(color));
    graphics.FillPath(&brush, &path);
}

bool MeasureBreadcrumbMenuItem(HWND hwnd, MEASUREITEMSTRUCT* item) {
    if (!item || item->CtlType != ODT_MENU) {
        return false;
    }

    const wchar_t* label =
        (const wchar_t*)(ULONG_PTR)item->itemData;
    const bool isTraySeparator = item->itemID == kTraySeparator;
    if (!label && !isTraySeparator) {
        return false;
    }

    HDC dc = GetDC(hwnd);
    if (!dc) {
        return false;
    }

    HGDIOBJ oldFont =
        g_pickerFont ? SelectObject(dc, g_pickerFont) : nullptr;

    SIZE textSize{};
    const wchar_t* measuredLabel =
        isTraySeparator ? L"Open wallpaper picker" : label;
    GetTextExtentPoint32W(dc, measuredLabel,
                          (int)wcslen(measuredLabel), &textSize);

    if (oldFont) {
        SelectObject(dc, oldFont);
    }
    ReleaseDC(hwnd, dc);

    // Reserve an icon gutter so every folder name starts in the same place.
    item->itemWidth = (UINT)((int)textSize.cx + 46);
    if (isTraySeparator) {
        item->itemHeight = 10;
    } else {
        const int height = (int)textSize.cy + 14;
        item->itemHeight = (UINT)(height < 29 ? 29 : height);
    }
    return true;
}

WCHAR BreadcrumbMenuItemGlyph(UINT itemId) {
    switch (itemId) {
        case kTrayOpen: return 0xEB9F;      // Wallpaper / picture
        case kTrayPrevious: return 0xE892;  // Previous
        case kTrayNext: return 0xE893;      // Next
        case kTrayExit: return 0xE8BB;      // Close
        default: return 0xE8B7;             // Folder
    }
}

bool DrawBreadcrumbMenuItem(const DRAWITEMSTRUCT* item) {
    if (!item || item->CtlType != ODT_MENU) {
        return false;
    }

    ApplyBreadcrumbMenuCorners();

    if (item->itemID == kTraySeparator) {
        FillRect(item->hDC, &item->rcItem, g_backgroundBrush);
        RECT dividerRect = item->rcItem;
        dividerRect.left += 12;
        dividerRect.right -= 12;
        const int centerY =
            (item->rcItem.top + item->rcItem.bottom) / 2;
        dividerRect.top = centerY;
        dividerRect.bottom = centerY + 1;
        HBRUSH dividerBrush = CreateSolidBrush(RGB(66, 62, 84));
        if (dividerBrush) {
            FillRect(item->hDC, &dividerRect, dividerBrush);
            DeleteObject(dividerBrush);
        }
        return true;
    }

    const wchar_t* label =
        (const wchar_t*)(ULONG_PTR)item->itemData;
    if (!label) {
        return false;
    }

    const bool highlighted =
        (item->itemState & (ODS_SELECTED | ODS_HOTLIGHT)) != 0;

    {
        Gdiplus::Graphics graphics(item->hDC);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

        // Clear any previous hover fill whenever this row is redrawn.
        Gdiplus::SolidBrush rowBackground(ToGdiPlusColor(g_background));
        graphics.FillRectangle(
            &rowBackground,
            (Gdiplus::REAL)item->rcItem.left,
            (Gdiplus::REAL)item->rcItem.top,
            (Gdiplus::REAL)(item->rcItem.right - item->rcItem.left),
            (Gdiplus::REAL)(item->rcItem.bottom - item->rcItem.top));

        // Leave a small inset so the selected row has visibly rounded corners.
        if (highlighted) {
            RECT highlightRect = item->rcItem;
            highlightRect.left += 4;
            highlightRect.right -= 4;
            highlightRect.top += 2;
            highlightRect.bottom -= 2;

            FillRoundedMenuItem(graphics, highlightRect, 5.0f, g_surface);
        }

        graphics.Flush(Gdiplus::FlushIntentionSync);
    }

    const int saved = SaveDC(item->hDC);
    if (!saved) {
        return true;
    }

    SetBkMode(item->hDC, TRANSPARENT);
    SetTextColor(item->hDC, g_text);

    HGDIOBJ oldFont =
        g_pickerFont ? SelectObject(item->hDC, g_pickerFont) : nullptr;

    // Draw a Fluent glyph in the shared icon gutter.
    if (g_breadcrumbIconFont) {
        HGDIOBJ oldIconFont =
            SelectObject(item->hDC, g_breadcrumbIconFont);
        const WCHAR itemGlyph[] = {BreadcrumbMenuItemGlyph(item->itemID), L'\0'};

        SetTextColor(item->hDC, g_text);
        RECT iconRect{
            item->rcItem.left + 14,
            item->rcItem.top,
            item->rcItem.left + 30,
            item->rcItem.bottom
        };
        DrawTextW(item->hDC, itemGlyph, 1, &iconRect,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

        if (oldIconFont) {
            SelectObject(item->hDC, oldIconFont);
        }
        SetTextColor(item->hDC, g_text);
    }

    RECT textRect = item->rcItem;
    textRect.left += 34;
    textRect.right -= 10;

    DrawTextW(item->hDC, label, -1, &textRect,
              DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS |
                  DT_NOPREFIX);

    if (oldFont) {
        SelectObject(item->hDC, oldFont);
    }

    RestoreDC(item->hDC, saved);
    return true;
}

void InvalidateBreadcrumbSegment(HWND hwnd, int segmentIndex) {
    if (!hwnd || segmentIndex < 0) {
        return;
    }
    for (const BreadcrumbHitTarget& target : g_breadcrumbHitTargets) {
        if ((int)target.segmentIndex == segmentIndex) {
            RECT dirty = target.hoverRect;
            InflateRect(&dirty, 2, 1);
            RECT client{};
            GetClientRect(hwnd, &client);
            RECT clipped{};
            if (IntersectRect(&clipped, &dirty, &client)) {
                InvalidateRect(hwnd, &clipped, FALSE);
            }
            return;
        }
    }
}

LRESULT CALLBACK BreadcrumbMenuMouseHookProc(int code, WPARAM message,
                                               LPARAM lParam) {
    if (code >= 0 && (message == WM_LBUTTONDOWN ||
                      message == WM_LBUTTONUP)) {
        const auto* mouse = (const MOUSEHOOKSTRUCT*)lParam;
        RECT triggerRect{
            g_breadcrumbMenuDismissAnchor.x - 16,
            g_breadcrumbMenuDismissAnchor.y - 16,
            g_breadcrumbMenuDismissAnchor.x + 16,
            g_breadcrumbMenuDismissAnchor.y + 16};
        const bool cursorAtTrigger =
            mouse && PtInRect(&triggerRect, mouse->pt);

        // TrackPopupMenuEx exits on an outside mouse-down. Keep our thread
        // hook alive until the matching up so that release cannot reopen it.
        if (g_suppressedBreadcrumbClickSegment >= 0 &&
            message == WM_LBUTTONUP) {
            const bool releaseAtTrigger = cursorAtTrigger;
            g_suppressedBreadcrumbClickSegment = -1;
            g_suppressBreadcrumbClickUntil = 0;
            if (g_breadcrumbMenuMouseHook) {
                UnhookWindowsHookEx(g_breadcrumbMenuMouseHook);
                g_breadcrumbMenuMouseHook = nullptr;
            }
            if (releaseAtTrigger) {
                return 1;
            }
        }

        if (cursorAtTrigger && g_breadcrumbMenuDismissActive &&
            g_openBreadcrumbMenuSegment >= 0) {
            g_suppressedBreadcrumbClickSegment =
                g_openBreadcrumbMenuSegment;
            g_suppressBreadcrumbClickUntil = GetTickCount64() + 5000;
            EndMenu();
            if (message == WM_LBUTTONDOWN) {
                // Stop the native menu from dismissing on press and forwarding
                // the same click to the breadcrumb.
                return 1;
            }

            // If the hook first sees the release, consume it here.
            g_suppressedBreadcrumbClickSegment = -1;
            if (g_breadcrumbMenuMouseHook) {
                UnhookWindowsHookEx(g_breadcrumbMenuMouseHook);
                g_breadcrumbMenuMouseHook = nullptr;
            }
            return 1;
        }
    }
    return CallNextHookEx(g_breadcrumbMenuMouseHook, code, message, lParam);
}

void ShowBreadcrumbFolderMenu(size_t segmentIndex, POINT anchor) {
    if (segmentIndex >= g_breadcrumbSegments.size() ||
        !g_breadcrumbSegments[segmentIndex].isFolder) {
        return;
    }

    const std::vector<FolderOption> children =
        DirectImageFolders(g_breadcrumbSegments[segmentIndex].folderPath);
    if (children.empty()) {
        return;
    }

    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return;
    }

    for (size_t i = 0; i < children.size(); ++i) {
        MENUITEMINFOW menuItem{};
        menuItem.cbSize = sizeof(menuItem);
        menuItem.fMask = MIIM_FTYPE | MIIM_ID | MIIM_DATA;
        menuItem.fType = MFT_OWNERDRAW;
        menuItem.wID = (UINT)(i + 1);
        menuItem.dwItemData =
            (ULONG_PTR)children[i].name.c_str();
        InsertMenuItemW(menu, (UINT)i, TRUE, &menuItem);
    }

    MENUINFO menuInfo{};
    menuInfo.cbSize = sizeof(menuInfo);
    menuInfo.fMask = MIM_BACKGROUND;
    menuInfo.hbrBack = g_backgroundBrush;
    SetMenuInfo(menu, &menuInfo);

    g_openBreadcrumbMenuSegment = (int)segmentIndex;
    g_animatedBreadcrumbMenuSegment = (int)segmentIndex;
    g_breadcrumbChevronTargetAngle = 90.0f;
    g_breadcrumbMenuCursorEntered = false;
    SetTimer(g_pickerWindow, kBreadcrumbChevronAnimationTimer, 16, nullptr);
    InvalidateBreadcrumbSegment(g_folderLabel, (int)segmentIndex);
    UpdateWindow(g_folderLabel);

    SetForegroundWindow(g_pickerWindow);

    g_breadcrumbMenuDismissAnchor = anchor;
    g_breadcrumbMenuOutsideSince = 0;
    g_breadcrumbMenuDismissActive = true;
    SetTimer(g_pickerWindow, kBreadcrumbMenuDismissTimer, 16, nullptr);
    g_breadcrumbMenuMouseHook = SetWindowsHookExW(
        WH_MOUSE, BreadcrumbMenuMouseHookProc, nullptr, GetCurrentThreadId());

    const UINT selected = TrackPopupMenuEx(
        menu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON,
        anchor.x - 8, anchor.y + 16, g_pickerWindow, nullptr);

    if (g_breadcrumbMenuMouseHook &&
        g_suppressedBreadcrumbClickSegment < 0) {
        UnhookWindowsHookEx(g_breadcrumbMenuMouseHook);
        g_breadcrumbMenuMouseHook = nullptr;
    }
    KillTimer(g_pickerWindow, kBreadcrumbMenuDismissTimer);
    g_breadcrumbMenuDismissActive = false;
    g_breadcrumbMenuOutsideSince = 0;
    g_breadcrumbMenuCursorEntered = false;

    DestroyMenu(menu);
    g_openBreadcrumbMenuSegment = -1;
    g_animatedBreadcrumbMenuSegment = (int)segmentIndex;
    g_breadcrumbChevronTargetAngle = 0.0f;
    SetTimer(g_pickerWindow, kBreadcrumbChevronAnimationTimer, 16, nullptr);
    InvalidateBreadcrumbSegment(g_folderLabel, (int)segmentIndex);
    UpdateWindow(g_folderLabel);

    if (g_pickerCloseAfterBreadcrumbMenu && g_pickerWindow) {
        g_pickerCloseAfterBreadcrumbMenu = false;
        HidePickerWindow(g_pickerWindow);
    }

    if (selected > 0 && selected <= children.size()) {
        NavigateToFolder(children[selected - 1].path);
    }
}

void UpdateFolderLabel() {
    if (!g_folderLabel) {
        return;
    }

    const std::wstring rootName = FolderDisplayName();
    g_hoverBreadcrumbSegment = -1;
    g_breadcrumbSegments.clear();
    g_breadcrumbHitTargets.clear();
    g_breadcrumbSegments.push_back({rootName, g_folder, true});
    if (!g_folder.empty() && g_selectedImage >= 0 &&
        g_selectedImage < (int)g_images.size()) {
        const std::wstring& imagePath = g_images[(size_t)g_selectedImage];
        std::wstring rootPath = TrimFolderSeparators(g_folder);
        size_t relativeStart = std::wstring::npos;
        if (imagePath.size() >= rootPath.size() &&
            CompareStringOrdinal(imagePath.c_str(), (int)rootPath.size(),
                                 rootPath.c_str(), (int)rootPath.size(), TRUE) ==
                CSTR_EQUAL) {
            relativeStart = rootPath.size();
            if (relativeStart < imagePath.size() &&
                (imagePath[relativeStart] == L'\\' ||
                 imagePath[relativeStart] == L'/')) {
                ++relativeStart;
            } else if (relativeStart < imagePath.size() &&
                       (rootPath.empty() ||
                        (rootPath.back() != L'\\' &&
                         rootPath.back() != L'/'))) {
                relativeStart = std::wstring::npos;
            }
        }

        if (relativeStart != std::wstring::npos &&
            relativeStart < imagePath.size()) {
            std::wstring parentPath = rootPath;
            size_t start = relativeStart;
            while (start < imagePath.size()) {
                const size_t separator = imagePath.find_first_of(L"\\/", start);
                const size_t finish = separator == std::wstring::npos
                                          ? imagePath.size()
                                          : separator;
                if (finish > start) {
                    const std::wstring component =
                        imagePath.substr(start, finish - start);
                    if (separator == std::wstring::npos) {
                        g_breadcrumbSegments.push_back({component, L"", false});
                    } else {
                        if (!parentPath.empty() && parentPath.back() != L'\\' &&
                            parentPath.back() != L'/') {
                            parentPath += L'\\';
                        }
                        parentPath += component;
                        g_breadcrumbSegments.push_back(
                            {component, parentPath, true});
                    }
                }
                if (separator == std::wstring::npos) {
                    break;
                }
                start = separator + 1;
            }
        }
    }

    std::wstring accessibleLabel;
    for (const BreadcrumbSegment& segment : g_breadcrumbSegments) {
        if (!accessibleLabel.empty()) {
            accessibleLabel += L" > ";
        }
        accessibleLabel += segment.label;
    }
    SendMessageW(g_folderLabel, WM_SETREDRAW, FALSE, 0);
    SetWindowTextW(g_folderLabel, accessibleLabel.c_str());
    SendMessageW(g_folderLabel, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(g_folderLabel, nullptr, FALSE);
}

size_t DisplayImageCount() {
    return g_visibleImageIndices.size();
}

void RebuildSearchResults(bool invalidateThumbnailGeneration) {
    g_visibleImageIndices.clear();
    g_visibleImageIndices.reserve(g_images.size());
    for (size_t i = 0; i < g_images.size(); ++i) {
        const WCHAR* filename = PathFindFileNameW(g_images[i].c_str());
        if (g_searchQuery.empty() ||
            StrStrIW(filename, g_searchQuery.c_str())) {
            g_visibleImageIndices.push_back(i);
        }
    }

    if (g_selectedImage < 0 ||
        std::find(g_visibleImageIndices.begin(), g_visibleImageIndices.end(),
                  (size_t)g_selectedImage) == g_visibleImageIndices.end()) {
        g_selectedImage = g_visibleImageIndices.empty()
                              ? -1
                              : (int)g_visibleImageIndices.front();
    }
    g_viewAnchor = 0;
    g_page = 0;
    if (g_selectedImage >= 0) {
        const auto selected = std::find(g_visibleImageIndices.begin(),
                                        g_visibleImageIndices.end(),
                                        (size_t)g_selectedImage);
        if (selected != g_visibleImageIndices.end()) {
            g_page = (size_t)std::distance(g_visibleImageIndices.begin(), selected) /
                     kPageSize;
        }
    }
    if (invalidateThumbnailGeneration && !g_imageScanInProgress) {
        ++g_folderGeneration;
    }
    ClearThumbnailCache();
    RefreshList();
}

void SetSearchExpanded(bool expanded, WCHAR initialCharacter) {
    if (!g_searchEdit) {
        return;
    }
    g_searchExpanded = expanded;
    ShowWindow(g_searchEdit, expanded ? SW_SHOW : SW_HIDE);
    if (expanded) {
        if (initialCharacter) {
            SetWindowTextW(g_searchEdit, std::wstring(1, initialCharacter).c_str());
            UpdateSearchFromEdit();
        }
        BringWindowToTop(g_searchEdit);
        SetFocus(g_searchEdit);
        SendMessageW(g_searchEdit, EM_SETSEL, MAXLONG, MAXLONG);
    } else {
        SetWindowTextW(g_searchEdit, L"");
        if (g_pickerWindow) {
            KillTimer(g_pickerWindow, kSearchFilterTimer);
        }
        g_pendingSearchQuery.clear();
        if (!g_searchQuery.empty()) {
            g_searchQuery.clear();
            RebuildSearchResults();
        }
        if (g_tiles[0] && IsWindowVisible(g_tiles[0])) {
            SetFocus(g_tiles[0]);
        } else if (g_pickerWindow) {
            SetFocus(g_pickerWindow);
        }
    }
    if (g_folderLabel) {
        InvalidateRect(g_folderLabel, nullptr, FALSE);
    }
}

void UpdateSearchFromEdit() {
    if (!g_searchEdit) {
        return;
    }
    const int length = GetWindowTextLengthW(g_searchEdit);
    std::wstring query((size_t)length + 1, L'\0');
    GetWindowTextW(g_searchEdit, query.data(), length + 1);
    query.resize((size_t)length);
    g_pendingSearchQuery = std::move(query);
    if (!g_pickerWindow || g_pendingSearchQuery == g_searchQuery) {
        if (g_pickerWindow) {
            KillTimer(g_pickerWindow, kSearchFilterTimer);
        }
        return;
    }
    KillTimer(g_pickerWindow, kSearchFilterTimer);
    if (!SetTimer(g_pickerWindow, kSearchFilterTimer,
                  kSearchFilterDelayMs, nullptr)) {
        ApplyPendingSearch();
    }
}

void ApplyPendingSearch() {
    if (g_pickerWindow) {
        KillTimer(g_pickerWindow, kSearchFilterTimer);
    }
    if (g_pendingSearchQuery != g_searchQuery) {
        g_searchQuery = std::move(g_pendingSearchQuery);
        RebuildSearchResults();
        if (g_searchEdit && IsWindowVisible(g_searchEdit)) {
            InvalidateRect(g_searchEdit, nullptr, FALSE);
        }
    }
}

LRESULT CALLBACK SearchEditSubclassProc(HWND hwnd, UINT message,
                                        WPARAM wParam, LPARAM lParam,
                                        UINT_PTR subclassId,
                                        DWORD_PTR referenceData) {
    if (message == WM_KEYDOWN) {
        if (wParam == VK_ESCAPE) {
            if (GetWindowTextLengthW(hwnd) > 0) {
                SetWindowTextW(hwnd, L"");
                UpdateSearchFromEdit();
                SendMessageW(hwnd, EM_SETSEL, 0, 0);
            } else {
                PostMessageW(GetAncestor(hwnd, GA_ROOT), WM_CLOSE, 0, 0);
            }
            return 0;
        }
        if (wParam == VK_RETURN) {
            ApplyPendingSearch();
            if (g_selectedImage >= 0) {
                ApplyImageIndex(g_selectedImage);
            }
            return 0;
        }
        if (wParam == VK_TAB) {
            FocusSelectedResult();
            return 0;
        }
        if ((wParam == VK_LEFT || wParam == VK_RIGHT ||
             wParam == VK_UP || wParam == VK_DOWN) &&
            !(GetKeyState(VK_SHIFT) & 0x8000) &&
            !(GetKeyState(VK_CONTROL) & 0x8000)) {
            ApplyPendingSearch();
            HandlePickerKey((UINT)wParam);
            return 0;
        }
    }
    if (message == WM_MOUSEWHEEL) {
        StepPage(GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? -1 : 1);
        return 0;
    }
    const LRESULT result =
        DefSubclassProc(hwnd, message, wParam, lParam);
    if (message == WM_CHAR || message == WM_KEYDOWN ||
        message == WM_KEYUP || message == WM_SETTEXT ||
        message == WM_PASTE || message == WM_CUT || message == WM_CLEAR ||
        message == WM_UNDO) {
        // Let normal message processing finish, then repaint without erasing
        // the field or synchronously flashing it.
        InvalidateRect(hwnd, nullptr, FALSE);
    }
    return result;
}

void UpdatePageLabel() {
    if (!g_pageLabel) {
        return;
    }
    const size_t count = DisplayImageCount();
    const size_t pages = (count + kPageSize - 1) / kPageSize;
    std::wstring label;
    if (g_images.empty()) {
        if (g_folder.empty()) {
            label = L"Choose a wallpaper folder to get started.";
        } else if (g_imageScanInProgress) {
            label = L"Scanning wallpaper folder...";
        } else if (g_folderUnavailable) {
            label = L"Folder unavailable or unreadable.";
        } else {
            label = L"No supported images found.";
        }
    } else if (count == 0 && !g_searchQuery.empty()) {
        label = L"No matching wallpapers.";
    } else {
        label = std::to_wstring(g_page + 1) + L" / " +
                std::to_wstring(pages);
    }
    SetWindowTextW(g_pageLabel, label.c_str());
}

bool IsPageInList(const std::vector<size_t>& pages, size_t page) {
    return std::find(pages.begin(), pages.end(), page) != pages.end();
}

std::vector<size_t> DesiredThumbnailPages() {
    std::vector<size_t> pages;
    const size_t pageCount =
        (DisplayImageCount() + kPageSize - 1) / kPageSize;
    if (!pageCount) {
        return pages;
    }

    pages.push_back(g_page);
    if (pageCount > 1 && kThumbnailCachePages > 1) {
        pages.push_back((g_page + 1) % pageCount);
    }
    if (pageCount > 2 && kThumbnailCachePages > 2) {
        pages.push_back((g_page + pageCount - 1) % pageCount);
    }
    return pages;
}

void DestroyCachedPage(CachedThumbnailPage& page) {
    for (HBITMAP bitmap : page.bitmaps) {
        if (bitmap) {
            DeleteObject(bitmap);
        }
    }
    page.bitmaps.clear();
}

void ClearThumbnailCache() {
    for (CachedThumbnailPage& page : g_thumbnailCache) {
        DestroyCachedPage(page);
    }
    g_thumbnailCache.clear();
}

HBITMAP CachedThumbnail(size_t page, size_t slot) {
    for (const CachedThumbnailPage& cached : g_thumbnailCache) {
        if (cached.page == page && slot < cached.bitmaps.size()) {
            return cached.bitmaps[slot];
        }
    }
    return nullptr;
}

bool HasCachedPage(size_t page) {
    return std::any_of(g_thumbnailCache.begin(), g_thumbnailCache.end(),
                       [page](const CachedThumbnailPage& cached) {
                           return cached.page == page;
                       });
}

bool HasPendingPage(size_t page) {
    return std::any_of(
        g_pendingThumbnailPages.begin(), g_pendingThumbnailPages.end(),
        [page](const PendingThumbnailPage& pending) {
            return pending.generation == g_folderGeneration.load() &&
                   pending.page == page;
        });
}

size_t ImageIndexForPageSlot(size_t page, size_t slot) {
    const size_t count = g_visibleImageIndices.size();
    const size_t displayPosition = page * kPageSize + slot;
    if (!count || displayPosition >= count) {
        return g_images.size();
    }
    return g_visibleImageIndices[(g_viewAnchor + displayPosition) % count];
}

size_t PageForImageIndex(size_t image) {
    if (g_visibleImageIndices.empty()) {
        return 0;
    }
    const auto found = std::find(g_visibleImageIndices.begin(),
                                 g_visibleImageIndices.end(), image);
    if (found == g_visibleImageIndices.end()) return 0;
    const size_t position = (size_t)std::distance(g_visibleImageIndices.begin(), found);
    const size_t count = g_visibleImageIndices.size();
    const size_t anchor = g_viewAnchor % count;
    const size_t offset = position >= anchor ? position - anchor
                                             : count - (anchor - position);
    return offset / kPageSize;
}

size_t SlotForImageIndex(size_t image) {
    if (g_visibleImageIndices.empty()) {
        return 0;
    }
    const auto found = std::find(g_visibleImageIndices.begin(),
                                 g_visibleImageIndices.end(), image);
    if (found == g_visibleImageIndices.end()) return 0;
    const size_t position = (size_t)std::distance(g_visibleImageIndices.begin(), found);
    const size_t count = g_visibleImageIndices.size();
    const size_t anchor = g_viewAnchor % count;
    const size_t offset = position >= anchor ? position - anchor
                                             : count - (anchor - position);
    return offset % kPageSize;
}

void SetViewAnchor(size_t image) {
    if (g_visibleImageIndices.empty()) {
        g_viewAnchor = 0;
        g_page = 0;
        return;
    }
    const auto found = std::find(g_visibleImageIndices.begin(),
                                 g_visibleImageIndices.end(), image);
    if (found == g_visibleImageIndices.end()) {
        g_viewAnchor = 0;
        g_page = 0;
        return;
    }
    const size_t anchor = (size_t)std::distance(g_visibleImageIndices.begin(), found);
    if (anchor != g_viewAnchor) {
        g_viewAnchor = anchor;
        // Cached page numbers now refer to a different sequence of images.
        ++g_folderGeneration;
        ClearThumbnailCache();
    }
    g_page = 0;
}

size_t CurrentGenerationPendingCount() {
    return (size_t)std::count_if(
        g_pendingThumbnailPages.begin(), g_pendingThumbnailPages.end(),
        [](const PendingThumbnailPage& pending) {
            return pending.generation == g_folderGeneration.load();
        });
}

void PruneThumbnailCache(const std::vector<size_t>& desiredPages) {
    for (auto it = g_thumbnailCache.begin(); it != g_thumbnailCache.end();) {
        if (!IsPageInList(desiredPages, it->page)) {
            DestroyCachedPage(*it);
            it = g_thumbnailCache.erase(it);
        } else {
            ++it;
        }
    }
}

bool StartThumbnailPageLoad(size_t page) {
    if (g_visibleImageIndices.empty()) {
        return false;
    }
    std::vector<std::wstring> paths;
    paths.reserve(kPageSize);
    for (size_t slot = 0; slot < kPageSize; ++slot) {
        const size_t image = ImageIndexForPageSlot(page, slot);
        paths.push_back(image < g_images.size() ? g_images[image]
                                                : std::wstring());
    }
    auto* job = new ThumbnailPageJob{g_hostWindow, g_folderGeneration.load(), page,
                                     std::move(paths)};
    g_pendingThumbnailPages.push_back({g_folderGeneration.load(), page});
    if (!StartBackgroundThread(ThumbnailPageThreadProc, job)) {
        g_pendingThumbnailPages.pop_back();
        delete job;
        return false;
    }
    return true;
}

void ScheduleThumbnailWindow() {
    if (g_imageScanInProgress || g_visibleImageIndices.empty() || !g_hostWindow) {
        return;
    }
    const std::vector<size_t> desiredPages = DesiredThumbnailPages();
    PruneThumbnailCache(desiredPages);
    size_t pending = CurrentGenerationPendingCount();
    for (size_t page : desiredPages) {
        if (HasCachedPage(page) || HasPendingPage(page)) {
            continue;
        }
        if (pending >= kMaxThumbnailWorkers) {
            break;
        }
        if (StartThumbnailPageLoad(page)) {
            ++pending;
        }
    }
}

void RefreshList() {
    if (!g_tiles[0]) {
        return;
    }
    g_page = std::min(g_page, g_visibleImageIndices.empty()
                                  ? size_t(0)
                                  : (g_visibleImageIndices.size() - 1) / kPageSize);
    ScheduleThumbnailWindow();

    for (size_t slot = 0; slot < kPageSize; ++slot) {
        if (g_tiles[slot]) {
            const bool slotHasImage =
                g_page * kPageSize + slot < DisplayImageCount();
            ShowWindow(g_tiles[slot], slotHasImage ? SW_SHOW : SW_HIDE);
            InvalidateRect(g_tiles[slot], nullptr, TRUE);
            if (slotHasImage) {
                UpdateWindow(g_tiles[slot]);
            }
        }
    }
    UpdateFolderLabel();
    UpdatePageLabel();
}

void StartImageScan() {
    const std::uint64_t generation = ++g_folderGeneration;
    if (g_pickerWindow) {
        KillTimer(g_pickerWindow, kSearchFilterTimer);
    }
    g_images.clear();
    g_visibleImageIndices.clear();
    g_searchQuery.clear();
    g_pendingSearchQuery.clear();
    g_searchExpanded = false;
    if (g_searchEdit) {
        SetWindowTextW(g_searchEdit, L"");
        ShowWindow(g_searchEdit, SW_HIDE);
    }
    g_viewAnchor = 0;
    g_page = 0;
    g_selectedImage = -1;
    g_currentImage = -1;
    g_folderUnavailable = false;
    g_imageScanInProgress = false;
    ClearThumbnailCache();

    if (g_folder.empty()) {
        RefreshList();
        return;
    }

    g_imageScanInProgress = true;
    UpdateFolderLabel();
    UpdatePageLabel();
    RefreshList();

    auto* job = new ImageScanJob{g_hostWindow, g_folder, generation};
    if (!StartBackgroundThread(ImageListThreadProc, job)) {
        delete job;
        g_imageScanInProgress = false;
        g_folderUnavailable = true;
        Wh_Log(L"Could not start wallpaper scan for '%ls'",
               g_folder.c_str());
        UpdatePageLabel();
    }
}

bool PickFolder(HWND owner, std::wstring* result) {
    const HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    winrt::com_ptr<IFileDialog> dialog;
    bool picked = false;
    if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, nullptr,
                                   CLSCTX_INPROC_SERVER,
                                   IID_PPV_ARGS(dialog.put())))) {
        DWORD options = 0;
        dialog->GetOptions(&options);
        dialog->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM);
        dialog->SetTitle(L"Choose your wallpaper folder");
        winrt::com_ptr<IShellItem> item;
        PWSTR path = nullptr;
        if (SUCCEEDED(dialog->Show(owner)) &&
            SUCCEEDED(dialog->GetResult(item.put())) &&
            SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
            *result = path;
            CoTaskMemFree(path);
            picked = true;
        }
    }
    dialog = nullptr;
    if (SUCCEEDED(com)) {
        CoUninitialize();
    }
    return picked;
}

struct WallpaperRenderPlan {
    UINT scaledWidth;
    UINT scaledHeight;
    WICRect crop;
    D2D1_RECT_F destination;
};

winrt::com_ptr<IWICBitmapSource> ReadOrientedFrame(
    IWICImagingFactory* factory,
    PCWSTR path) {
    winrt::com_ptr<IWICBitmapDecoder> decoder;
    winrt::com_ptr<IWICBitmapFrameDecode> frame;
    if (FAILED(factory->CreateDecoderFromFilename(
            path, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand,
            decoder.put())) ||
        FAILED(decoder->GetFrame(0, frame.put()))) {
        return nullptr;
    }

    auto pixels = frame.as<IWICBitmapSource>();
    winrt::com_ptr<IWICMetadataQueryReader> metadata;
    PROPVARIANT orientation;
    PropVariantInit(&orientation);
    const bool hasOrientation =
        SUCCEEDED(frame->GetMetadataQueryReader(metadata.put())) &&
        SUCCEEDED(metadata->GetMetadataByName(
            L"/app1/ifd/{ushort=274}", &orientation)) &&
        orientation.vt == VT_UI2;

    WICBitmapTransformOptions rotation = WICBitmapTransformRotate0;
    if (hasOrientation) {
        switch (orientation.uiVal) {
            case 3:
                rotation = WICBitmapTransformRotate180;
                break;
            case 6:
                rotation = WICBitmapTransformRotate90;
                break;
            case 8:
                rotation = WICBitmapTransformRotate270;
                break;
        }
    }
    PropVariantClear(&orientation);

    if (rotation != WICBitmapTransformRotate0) {
        winrt::com_ptr<IWICBitmapFlipRotator> rotated;
        if (SUCCEEDED(factory->CreateBitmapFlipRotator(rotated.put())) &&
            SUCCEEDED(rotated->Initialize(pixels.get(), rotation))) {
            pixels = rotated.as<IWICBitmapSource>();
        }
    }
    return pixels;
}

bool MakeWallpaperRenderPlan(UINT sourceWidth,
                             UINT sourceHeight,
                             SIZE monitorSize,
                             DESKTOP_WALLPAPER_POSITION position,
                             WallpaperRenderPlan* plan) {
    if (!sourceWidth || !sourceHeight || monitorSize.cx <= 0 ||
        monitorSize.cy <= 0 || !plan) {
        return false;
    }

    const double horizontalScale = (double)monitorSize.cx / sourceWidth;
    const double verticalScale = (double)monitorSize.cy / sourceHeight;
    const double coverScale = std::max(horizontalScale, verticalScale);
    const double containScale = std::min(horizontalScale, verticalScale);
    UINT outputWidth = sourceWidth;
    UINT outputHeight = sourceHeight;

    if (position == DWPOS_FIT) {
        outputWidth = std::max<UINT>(
            1, (UINT)std::lround(sourceWidth * containScale));
        outputHeight = std::max<UINT>(
            1, (UINT)std::lround(sourceHeight * containScale));
    } else if (position == DWPOS_STRETCH) {
        outputWidth = (UINT)monitorSize.cx;
        outputHeight = (UINT)monitorSize.cy;
    } else if (position != DWPOS_CENTER && position != DWPOS_TILE) {
        outputWidth = std::max<UINT>(
            (UINT)monitorSize.cx,
            (UINT)std::ceil(sourceWidth * coverScale));
        outputHeight = std::max<UINT>(
            (UINT)monitorSize.cy,
            (UINT)std::ceil(sourceHeight * coverScale));
    }

    const UINT visibleWidth = std::min<UINT>(outputWidth, monitorSize.cx);
    const UINT visibleHeight = std::min<UINT>(outputHeight, monitorSize.cy);
    const bool cover = position == DWPOS_FILL || position == DWPOS_SPAN;
    const INT cropTop = (INT)std::lround(
        (outputHeight - visibleHeight) / (cover ? 3.0 : 2.0));
    plan->scaledWidth = outputWidth;
    plan->scaledHeight = outputHeight;
    plan->crop = {(INT)(outputWidth - visibleWidth) / 2, cropTop,
                  (INT)visibleWidth, (INT)visibleHeight};

    const float left = (monitorSize.cx - (float)visibleWidth) * 0.5f;
    const float top = (monitorSize.cy - (float)visibleHeight) * 0.5f;
    plan->destination = {left, top, left + visibleWidth, top + visibleHeight};
    return true;
}

winrt::com_ptr<IWICBitmapSource> CreateDisplayBitmap(
    IWICImagingFactory* factory,
    IWICBitmapSource* source,
    const WallpaperRenderPlan& plan) {
    winrt::com_ptr<IWICBitmapScaler> scaled;
    winrt::com_ptr<IWICBitmapClipper> visible;
    winrt::com_ptr<IWICFormatConverter> premultiplied;
    const HRESULT scaleCreated = factory->CreateBitmapScaler(scaled.put());
    if (FAILED(scaleCreated) ||
        FAILED(scaled->Initialize(source, plan.scaledWidth,
                                  plan.scaledHeight,
                                  WICBitmapInterpolationModeFant))) {
        return nullptr;
    }
    const HRESULT clipCreated = factory->CreateBitmapClipper(visible.put());
    if (FAILED(clipCreated) ||
        FAILED(visible->Initialize(scaled.get(), &plan.crop))) {
        return nullptr;
    }
    if (FAILED(factory->CreateFormatConverter(premultiplied.put())) ||
        FAILED(premultiplied->Initialize(
            visible.get(), GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone, nullptr, 0,
            WICBitmapPaletteTypeMedianCut))) {
        return nullptr;
    }
    return premultiplied.as<IWICBitmapSource>();
}

HBITMAP CreateThumbnailBitmap(IWICImagingFactory* factory, PCWSTR path) {
    auto oriented = ReadOrientedFrame(factory, path);
    if (!oriented) {
        return nullptr;
    }

    UINT sourceWidth = 0;
    UINT sourceHeight = 0;
    if (FAILED(oriented->GetSize(&sourceWidth, &sourceHeight)) ||
        !sourceWidth || !sourceHeight ||
        sourceWidth > (UINT)std::numeric_limits<LONG>::max() ||
        sourceHeight > (UINT)std::numeric_limits<LONG>::max()) {
        return nullptr;
    }

    const double targetAspect = (double)kThumbnailWidth / kThumbnailHeight;
    const double sourceAspect = (double)sourceWidth / sourceHeight;
    WICRect crop{0, 0, (INT)sourceWidth, (INT)sourceHeight};
    if (sourceAspect > targetAspect) {
        crop.Width = std::max<INT>(
            1, (INT)std::floor(sourceHeight * targetAspect));
        crop.X = ((INT)sourceWidth - crop.Width) / 2;
    } else if (sourceAspect < targetAspect) {
        crop.Height = std::max<INT>(
            1, (INT)std::floor(sourceWidth / targetAspect));
        crop.Y = ((INT)sourceHeight - crop.Height) / 2;
    }

    winrt::com_ptr<IWICBitmapClipper> clipper;
    winrt::com_ptr<IWICBitmapScaler> scaler;
    winrt::com_ptr<IWICFormatConverter> converter;
    if (FAILED(factory->CreateBitmapClipper(clipper.put())) ||
        FAILED(clipper->Initialize(oriented.get(), &crop)) ||
        FAILED(factory->CreateBitmapScaler(scaler.put())) ||
        FAILED(scaler->Initialize(clipper.get(), kThumbnailWidth,
                                 kThumbnailHeight,
                                 WICBitmapInterpolationModeFant)) ||
        FAILED(factory->CreateFormatConverter(converter.put())) ||
        FAILED(converter->Initialize(
            scaler.get(), GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone, nullptr, 0,
            WICBitmapPaletteTypeMedianCut))) {
        return nullptr;
    }

    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = (LONG)kThumbnailWidth;
    info.bmiHeader.biHeight = -(LONG)kThumbnailHeight;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* pixels = nullptr;
    HDC screen = GetDC(nullptr);
    HBITMAP bitmap = screen
        ? CreateDIBSection(screen, &info, DIB_RGB_COLORS, &pixels, nullptr, 0)
        : nullptr;
    if (screen) {
        ReleaseDC(nullptr, screen);
    }
    if (!bitmap || !pixels ||
        FAILED(converter->CopyPixels(
            nullptr, kThumbnailWidth * 4,
            kThumbnailWidth * kThumbnailHeight * 4,
            (BYTE*)pixels))) {
        if (bitmap) {
            DeleteObject(bitmap);
        }
        return nullptr;
    }
    return bitmap;
}

// Decode, orient, size, crop, and normalize a file into a monitor-ready frame.
winrt::com_ptr<IWICBitmapSource> LoadWallpaperPicture(
    IWICImagingFactory* factory,
    PCWSTR path,
    SIZE monitorSize,
    DESKTOP_WALLPAPER_POSITION position,
    D2D1_RECT_F* destination) {
    auto oriented = ReadOrientedFrame(factory, path);
    if (!oriented) {
        return nullptr;
    }

    UINT sourceWidth = 0;
    UINT sourceHeight = 0;
    if (FAILED(oriented->GetSize(&sourceWidth, &sourceHeight))) {
        return nullptr;
    }
    WallpaperRenderPlan plan{};
    if (!MakeWallpaperRenderPlan(sourceWidth, sourceHeight, monitorSize,
                                 position, &plan)) {
        return nullptr;
    }

    auto displayFrame = CreateDisplayBitmap(factory, oriented.get(), plan);
    if (displayFrame && destination) {
        *destination = plan.destination;
    }
    return displayFrame;
}

bool SetWallpaperForMonitor(const RECT& monitor, PCWSTR path) {
    const HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(com) && com != RPC_E_CHANGED_MODE) {
        return false;
    }
    bool result = false;
    winrt::com_ptr<IDesktopWallpaper> wallpaper;
    if (SUCCEEDED(CoCreateInstance(CLSID_DesktopWallpaper, nullptr, CLSCTX_ALL,
                                   IID_PPV_ARGS(wallpaper.put())))) {
        UINT count = 0;
        if (SUCCEEDED(wallpaper->GetMonitorDevicePathCount(&count))) {
            for (UINT i = 0; i < count; ++i) {
                PWSTR id = nullptr;
                RECT rect{};
                const HRESULT gotId = wallpaper->GetMonitorDevicePathAt(i, &id);
                const bool matched = SUCCEEDED(gotId) && id &&
                    SUCCEEDED(wallpaper->GetMonitorRECT(id, &rect)) &&
                    EqualRect(&rect, &monitor);
                if (matched) {
                    result = SUCCEEDED(wallpaper->SetWallpaper(id, path));
                    CoTaskMemFree(id);
                    break;
                }
                if (id) {
                    CoTaskMemFree(id);
                }
            }
            if (!result && count == 1) {
                result = SUCCEEDED(wallpaper->SetWallpaper(nullptr, path));
            }
        }
    }
    if (SUCCEEDED(com)) {
        CoUninitialize();
    }
    return result;
}

bool IsWallpaperAlreadySet(const RECT& monitor, PCWSTR path) {
    const HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(com) && com != RPC_E_CHANGED_MODE) {
        return false;
    }
    bool matches = false;
    winrt::com_ptr<IDesktopWallpaper> wallpaper;
    if (SUCCEEDED(CoCreateInstance(CLSID_DesktopWallpaper, nullptr, CLSCTX_ALL,
                                   IID_PPV_ARGS(wallpaper.put())))) {
        UINT count = 0;
        if (SUCCEEDED(wallpaper->GetMonitorDevicePathCount(&count))) {
            for (UINT i = 0; i < count; ++i) {
                PWSTR id = nullptr;
                RECT rect{};
                const HRESULT gotId = wallpaper->GetMonitorDevicePathAt(i, &id);
                const bool found = SUCCEEDED(gotId) && id &&
                    SUCCEEDED(wallpaper->GetMonitorRECT(id, &rect)) &&
                    EqualRect(&rect, &monitor);
                if (found) {
                    PWSTR current = nullptr;
                    if (SUCCEEDED(wallpaper->GetWallpaper(id, &current)) &&
                        current) {
                        matches = CompareStringOrdinal(current, -1, path, -1,
                                                       TRUE) == CSTR_EQUAL;
                        CoTaskMemFree(current);
                    }
                    CoTaskMemFree(id);
                    break;
                }
                if (id) {
                    CoTaskMemFree(id);
                }
            }
            if (!matches && count == 1) {
                PWSTR current = nullptr;
                if (SUCCEEDED(wallpaper->GetWallpaper(nullptr, &current)) &&
                    current) {
                    matches = CompareStringOrdinal(current, -1, path, -1,
                                                   TRUE) == CSTR_EQUAL;
                    CoTaskMemFree(current);
                }
            }
        }
    }
    if (SUCCEEDED(com)) {
        CoUninitialize();
    }
    return matches;
}

class WallpaperCanvas {
   public:
    bool Create(ID2D1Factory* factory) {
        const auto properties = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED),
            96, 96);
        if (FAILED(factory->CreateDCRenderTarget(&properties, target.put()))) {
            return false;
        }
        target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
        dc = CreateCompatibleDC(nullptr);
        return dc != nullptr;
    }

    bool Begin(SIZE size) {
        if (!dc || size.cx <= 0 || size.cy <= 0) {
            return false;
        }
        if (bitmap && (size.cx != bitmapSize.cx || size.cy != bitmapSize.cy)) {
            SelectObject(dc, oldBitmap);
            DeleteObject(bitmap);
            bitmap = nullptr;
            oldBitmap = nullptr;
        }
        if (!bitmap) {
            BITMAPINFO info{};
            info.bmiHeader.biSize = sizeof(info.bmiHeader);
            info.bmiHeader.biWidth = size.cx;
            info.bmiHeader.biHeight = -size.cy;
            info.bmiHeader.biPlanes = 1;
            info.bmiHeader.biBitCount = 32;
            info.bmiHeader.biCompression = BI_RGB;
            void* bits = nullptr;
            bitmap = CreateDIBSection(dc, &info, DIB_RGB_COLORS, &bits,
                                      nullptr, 0);
            if (!bitmap) {
                return false;
            }
            oldBitmap = SelectObject(dc, bitmap);
            bitmapSize = size;
        }
        drawSize = size;
        const RECT bounds{0, 0, size.cx, size.cy};
        if (FAILED(target->BindDC(dc, &bounds))) {
            return false;
        }
        target->BeginDraw();
        target->SetTransform(D2D1::Matrix3x2F::Identity());
        target->Clear(D2D1::ColorF(0, 0, 0, 0));
        return true;
    }

    bool Present(HWND window, POINT position) {
        if (FAILED(target->EndDraw())) {
            return false;
        }
        POINT source{0, 0};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        return UpdateLayeredWindow(window, nullptr, &position, &drawSize,
                                   dc, &source, 0, &blend, ULW_ALPHA) != FALSE;
    }

    void SetAlpha(HWND window, POINT position, BYTE alpha) {
        if (!dc || !bitmap) {
            return;
        }
        POINT source{0, 0};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
        UpdateLayeredWindow(window, nullptr, &position, &drawSize,
                            dc, &source, 0, &blend, ULW_ALPHA);
    }

    void Destroy() {
        target = nullptr;
        if (dc) {
            if (oldBitmap) {
                SelectObject(dc, oldBitmap);
            }
            DeleteDC(dc);
            dc = nullptr;
        }
        if (bitmap) {
            DeleteObject(bitmap);
            bitmap = nullptr;
        }
        oldBitmap = nullptr;
    }

    winrt::com_ptr<ID2D1DCRenderTarget> target;

   private:
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ oldBitmap = nullptr;
    SIZE bitmapSize{};
    SIZE drawSize{};
};

void PlaceWallpaperOverlay(HWND window) {
    HWND progman = FindWindowW(L"Progman", nullptr);
    HWND above = progman ? GetWindow(progman, GW_HWNDPREV) : nullptr;
    for (int i = 0; above && i < 1000 &&
                    (above == window || !IsWindowVisible(above)); ++i) {
        above = GetWindow(above, GW_HWNDPREV);
    }
    const bool topmost =
        above && (GetWindowLongPtrW(above, GWL_EXSTYLE) & WS_EX_TOPMOST);
    const HWND insertAfter =
        !progman ? HWND_BOTTOM : (above && !topmost ? above : HWND_TOP);
    SetWindowPos(window, insertAfter, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

struct WallpaperChange {
    HWND notifyWindow;
    int imageIndex;
    std::wstring path;
    RECT monitor;
    WallpaperTransition transition;
    POINT origin;
    int wipeDirection;
    std::uint32_t dissolveSeed;
    int splitRevealDirection;
};

void PublishWallpaperRequest(const WallpaperChange& change) {
    AcquireSRWLockExclusive(&g_wallpaperRequestLock);
    g_latestWallpaperRequest.imageIndex = change.imageIndex;
    g_latestWallpaperRequest.path = change.path;
    g_latestWallpaperRequest.origin = change.origin;
    g_latestWallpaperRequest.transition = change.transition;
    g_latestWallpaperRequest.wipeDirection = change.wipeDirection;
    g_latestWallpaperRequest.dissolveSeed = change.dissolveSeed;
    g_latestWallpaperRequest.splitRevealDirection = change.splitRevealDirection;
    g_latestWallpaperRequest.generation++;
    ReleaseSRWLockExclusive(&g_wallpaperRequestLock);
}

WallpaperTargetRequest ReadWallpaperRequest() {
    AcquireSRWLockShared(&g_wallpaperRequestLock);
    WallpaperTargetRequest result = g_latestWallpaperRequest;
    ReleaseSRWLockShared(&g_wallpaperRequestLock);
    return result;
}

float EaseWallpaperProgress(double elapsedSeconds, double durationSeconds) {
    const double linear = std::clamp(elapsedSeconds / durationSeconds, 0.0, 1.0);
    const double eased = linear < 0.5
                             ? 4.0 * linear * linear * linear
                             : 1.0 - std::pow(2.0 - 2.0 * linear, 3.0) / 2.0;
    return (float)eased;
}

std::uint32_t DissolveNoise(std::uint32_t x, std::uint32_t y,
                            std::uint32_t seed) {
    std::uint32_t value = seed + x * 0x9E3779B9u + y * 0x85EBCA6Bu;
    value ^= value >> 16;
    value *= 0x7FEB352Du;
    value ^= value >> 15;
    value *= 0x846CA68Bu;
    value ^= value >> 16;
    return value;
}

void DrawWallpaperTransitionFrame(ID2D1Factory* geometryFactory,
                                  ID2D1RenderTarget* target,
                                  ID2D1Bitmap* bitmap,
                                  ID2D1Bitmap* blurredBitmap,
                                  const D2D1_RECT_F& pictureRect,
                                  SIZE monitorSize,
                                  POINT origin,
                                  COLORREF background,
                                  WallpaperTransition transition,
                                  int wipeDirection,
                                  std::uint32_t dissolveSeed,
                                  int splitRevealDirection,
                                  float progress,
                                  ID2D1Bitmap* baseBitmap = nullptr,
                                  const D2D1_RECT_F* basePictureRect = nullptr) {
    const float width = (float)monitorSize.cx;
    const float height = (float)monitorSize.cy;
    const D2D1_RECT_F viewport{0, 0, width, height};
    const float originX = (float)origin.x;
    const float originY = (float)origin.y;
    const float circleReach =
        std::hypot(std::max(originX, width - originX),
                   std::max(originY, height - originY));

    winrt::com_ptr<ID2D1SolidColorBrush> backgroundBrush;
    target->CreateSolidColorBrush(
        D2D1::ColorF(GetRValue(background) / 255.0f,
                     GetGValue(background) / 255.0f,
                     GetBValue(background) / 255.0f, 1.0f),
        backgroundBrush.put());

    if (baseBitmap && basePictureRect) {
        target->DrawBitmap(baseBitmap, basePictureRect, 1.0f,
                           D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    }

    auto paint = [&](float x, float opacity) {
        if (backgroundBrush) {
            backgroundBrush->SetOpacity(opacity);
            target->FillRectangle({x, 0, x + width, height},
                                  backgroundBrush.get());
        }
        const D2D1_RECT_F destination{
            pictureRect.left + x, pictureRect.top,
            pictureRect.right + x, pictureRect.bottom};
        target->DrawBitmap(bitmap, &destination, opacity,
                           D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    };

    switch (transition) {
        case WallpaperTransition::Circle: {
            winrt::com_ptr<ID2D1EllipseGeometry> aperture;
            winrt::com_ptr<ID2D1Layer> mask;
            const D2D1_POINT_2F center{originX, originY};
            if (SUCCEEDED(geometryFactory->CreateEllipseGeometry(
                    D2D1::Ellipse(center, circleReach * progress,
                                  circleReach * progress),
                    aperture.put())) &&
                SUCCEEDED(target->CreateLayer(nullptr, mask.put()))) {
                target->PushLayer(D2D1::LayerParameters(viewport,
                                                        aperture.get()),
                                  mask.get());
                paint(0, 1.0f);
                target->PopLayer();
            }
            return;
        }
        case WallpaperTransition::Slide:
            paint((1.0f - progress) * width,
                  0.4f + 0.6f * progress);
            return;
        case WallpaperTransition::Zoom: {
            const float scale = 1.12f - 0.12f * progress;
            target->SetTransform(D2D1::Matrix3x2F::Scale(
                scale, scale, {width * 0.5f, height * 0.5f}));
            paint(0, progress);
            target->SetTransform(D2D1::Matrix3x2F::Identity());
            return;
        }
        case WallpaperTransition::Wipe: {
            D2D1_RECT_F revealed{};
            switch (wipeDirection & 3) {
                case 0:
                    revealed = {0, 0, width * progress, height};
                    break;
                case 1:
                    revealed = {width * (1.0f - progress), 0, width, height};
                    break;
                case 2:
                    revealed = {0, 0, width, height * progress};
                    break;
                default:
                    revealed = {0, height * (1.0f - progress), width, height};
                    break;
            }
            target->PushAxisAlignedClip(revealed,
                                        D2D1_ANTIALIAS_MODE_ALIASED);
            paint(0, 1.0f);
            target->PopAxisAlignedClip();
            return;
        }
        case WallpaperTransition::Dissolve: {
            constexpr std::uint32_t columns = 40;
            constexpr std::uint32_t rows = 23;
            const float cellWidth = width / columns;
            const float cellHeight = height / rows;
            const float threshold = progress;
            for (std::uint32_t y = 0; y < rows; ++y) {
                for (std::uint32_t x = 0; x < columns; ++x) {
                    const float noise = (float)DissolveNoise(x, y, dissolveSeed) /
                                        4294967296.0f;
                    if (noise >= threshold) {
                        continue;
                    }
                    const D2D1_RECT_F cell{
                        x * cellWidth, y * cellHeight,
                        std::min(width, (x + 1) * cellWidth),
                        std::min(height, (y + 1) * cellHeight)};
                    target->PushAxisAlignedClip(cell,
                                                D2D1_ANTIALIAS_MODE_ALIASED);
                    paint(0, 1.0f);
                    target->PopAxisAlignedClip();
                }
            }
            return;
        }
        case WallpaperTransition::SplitReveal: {
            if ((splitRevealDirection & 3) == 0 ||
                (splitRevealDirection & 3) == 1) {
                D2D1_RECT_F revealed{};
                if ((splitRevealDirection & 3) == 0) {
                    const float halfWidth = width * progress * 0.5f;
                    revealed = {width * 0.5f - halfWidth, 0,
                                width * 0.5f + halfWidth, height};
                } else {
                    const float halfHeight = height * progress * 0.5f;
                    revealed = {0, height * 0.5f - halfHeight,
                                width, height * 0.5f + halfHeight};
                }
                target->PushAxisAlignedClip(revealed,
                                            D2D1_ANTIALIAS_MODE_ALIASED);
                paint(0, 1.0f);
                target->PopAxisAlignedClip();
                return;
            }

            // Diagonal modes reveal a centered band around the selected
            // corner-to-corner diagonal, expanding symmetrically outward.
            std::vector<D2D1_POINT_2F> polygon{
                {0, 0}, {width, 0}, {width, height}, {0, height}};
            const bool diagonalSlash = (splitRevealDirection & 3) == 3;
            const float a = 1.0f / width;
            const float b = diagonalSlash ? 1.0f / height : -1.0f / height;
            const float c = diagonalSlash ? -1.0f : 0.0f;
            auto clipHalfPlane = [&](float normalX, float normalY,
                                     float offset) {
                if (polygon.empty()) {
                    return;
                }
                std::vector<D2D1_POINT_2F> clipped;
                clipped.reserve(polygon.size() + 2);
                auto distance = [&](const D2D1_POINT_2F& point) {
                    return normalX * point.x + normalY * point.y + offset;
                };
                D2D1_POINT_2F previous = polygon.back();
                float previousDistance = distance(previous);
                bool previousInside = previousDistance <= progress;
                for (const D2D1_POINT_2F& current : polygon) {
                    const float currentDistance = distance(current);
                    const bool currentInside = currentDistance <= progress;
                    if (currentInside != previousInside) {
                        const float denominator =
                            previousDistance - currentDistance;
                        if (denominator != 0.0f) {
                            const float fraction =
                                (previousDistance - progress) / denominator;
                            clipped.push_back({
                                previous.x +
                                    (current.x - previous.x) * fraction,
                                previous.y +
                                    (current.y - previous.y) * fraction});
                        }
                    }
                    if (currentInside) {
                        clipped.push_back(current);
                    }
                    previous = current;
                    previousDistance = currentDistance;
                    previousInside = currentInside;
                }
                polygon = std::move(clipped);
            };
            clipHalfPlane(a, b, c);
            clipHalfPlane(-a, -b, -c);
            if (polygon.size() < 3) {
                return;
            }

            winrt::com_ptr<ID2D1PathGeometry> aperture;
            winrt::com_ptr<ID2D1GeometrySink> sink;
            winrt::com_ptr<ID2D1Layer> mask;
            if (SUCCEEDED(geometryFactory->CreatePathGeometry(aperture.put())) &&
                SUCCEEDED(aperture->Open(sink.put()))) {
                sink->BeginFigure(polygon.front(), D2D1_FIGURE_BEGIN_FILLED);
                sink->AddLines(polygon.data() + 1,
                               (UINT)polygon.size() - 1);
                sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                if (SUCCEEDED(sink->Close()) &&
                    SUCCEEDED(target->CreateLayer(nullptr, mask.put()))) {
                    target->PushLayer(D2D1::LayerParameters(viewport,
                                                            aperture.get()),
                                      mask.get());
                    paint(0, 1.0f);
                    target->PopLayer();
                }
            }
            return;
        }
        case WallpaperTransition::BlurFade:
            if (!blurredBitmap) {
                paint(0, progress);
                return;
            }
            if (backgroundBrush) {
                backgroundBrush->SetOpacity(progress);
                target->FillRectangle(viewport, backgroundBrush.get());
            }
            target->DrawBitmap(blurredBitmap, &pictureRect, progress,
                               D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            target->DrawBitmap(bitmap, &pictureRect,
                               progress * progress,
                               D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            return;
        case WallpaperTransition::Fade:
        default:
            paint(0, progress);
            return;
    }
}
DWORD WINAPI WallpaperChangeThreadProc(void* parameter) {
    std::unique_ptr<WallpaperChange> change((WallpaperChange*)parameter);
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    const HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool applied = false;
    const SIZE size{change->monitor.right - change->monitor.left,
                    change->monitor.bottom - change->monitor.top};
    const POINT position{change->monitor.left, change->monitor.top};
    DESKTOP_WALLPAPER_POSITION wallpaperPosition = DWPOS_FILL;
    COLORREF backgroundColor = RGB(0, 0, 0);
    winrt::com_ptr<IDesktopWallpaper> wallpaper;
    if (SUCCEEDED(CoCreateInstance(CLSID_DesktopWallpaper, nullptr, CLSCTX_ALL,
                                   IID_PPV_ARGS(wallpaper.put())))) {
        wallpaper->GetPosition(&wallpaperPosition);
        wallpaper->GetBackgroundColor(&backgroundColor);
    }

    const double duration = 0.7;
    D2D1_RECT_F pictureRect{};
    D2D1_RECT_F basePictureRect{};
    winrt::com_ptr<IWICImagingFactory> wic;
    winrt::com_ptr<ID2D1Factory> factory;
    winrt::com_ptr<ID2D1Bitmap> bitmap;
    winrt::com_ptr<ID2D1Bitmap> blurredBitmap;
    winrt::com_ptr<ID2D1Bitmap> baseBitmap;
    WallpaperCanvas canvas;
    HWND overlay = nullptr;
    bool shown = false;

    if (size.cx > 0 && size.cy > 0 &&
        SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                                   CLSCTX_INPROC_SERVER,
                                   IID_PPV_ARGS(wic.put()))) &&
        SUCCEEDED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                    factory.put())) &&
        canvas.Create(factory.get())) {
        overlay = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW |
                WS_EX_NOACTIVATE,
            L"STATIC", nullptr, WS_POPUP, 0, 0, 0, 0,
            nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
        if (overlay) {
            SetPropW(overlay, L"NonRudeHWND", (HANDLE)TRUE);
        }
    }

    WallpaperTargetRequest target = ReadWallpaperRequest();
    if (target.generation == 0) {
        target.imageIndex = change->imageIndex;
        target.path = change->path;
        target.origin = change->origin;
        target.transition = change->transition;
        target.wipeDirection = change->wipeDirection;
        target.dissolveSeed = change->dissolveSeed;
        target.splitRevealDirection = change->splitRevealDirection;
    }

    auto loadTarget = [&](const WallpaperTargetRequest& request,
                          winrt::com_ptr<ID2D1Bitmap>* loadedBitmap,
                          winrt::com_ptr<ID2D1Bitmap>* loadedBlurred,
                          D2D1_RECT_F* loadedRect) -> bool {
        if (!wic || !canvas.target || !loadedBitmap || !loadedBlurred ||
            !loadedRect) {
            return false;
        }
        winrt::com_ptr<IWICBitmapSource> picture = LoadWallpaperPicture(
            wic.get(), request.path.c_str(), size, wallpaperPosition,
            loadedRect);
        if (!picture || !canvas.Begin(size)) {
            return false;
        }
        const HRESULT bitmapResult = canvas.target->CreateBitmapFromWicBitmap(
            picture.get(), nullptr, loadedBitmap->put());
        if (SUCCEEDED(bitmapResult) &&
            request.transition == WallpaperTransition::BlurFade) {
            UINT sourceWidth = 0;
            UINT sourceHeight = 0;
            winrt::com_ptr<IWICBitmapScaler> blurScaler;
            if (SUCCEEDED(picture->GetSize(&sourceWidth, &sourceHeight)) &&
                SUCCEEDED(wic->CreateBitmapScaler(blurScaler.put())) &&
                SUCCEEDED(blurScaler->Initialize(
                    picture.get(), std::max<UINT>(1, sourceWidth / 12),
                    std::max<UINT>(1, sourceHeight / 12),
                    WICBitmapInterpolationModeFant))) {
                canvas.target->CreateBitmapFromWicBitmap(
                    blurScaler.get(), nullptr, loadedBlurred->put());
            }
        }
        const HRESULT endResult = canvas.target->EndDraw();
        return SUCCEEDED(bitmapResult) && SUCCEEDED(endResult) &&
               loadedBitmap->get() != nullptr;
    };

    if (overlay && !loadTarget(target, &bitmap, &blurredBitmap, &pictureRect)) {
        RemovePropW(overlay, L"NonRudeHWND");
        DestroyWindow(overlay);
        overlay = nullptr;
    }

    if (overlay && bitmap) {
        float progressBase = 0.0f;
        double segmentStart = (double)GetTickCount64() / 1000.0;
        bool wallpaperCommitted = false;
        bool finished = false;

        while (!finished) {
            // Keep one overlay alive and retarget it in place. Before the
            // wallpaper is committed, preserve the current reveal progress;
            // after a commit, use the last destination as the visual base.
            float progress = progressBase;
            for (;;) {
                WallpaperTargetRequest latest = ReadWallpaperRequest();
                if (latest.generation != target.generation &&
                    !latest.path.empty()) {
                    winrt::com_ptr<ID2D1Bitmap> nextBitmap;
                    winrt::com_ptr<ID2D1Bitmap> nextBlurred;
                    D2D1_RECT_F nextRect{};
                    if (loadTarget(latest, &nextBitmap, &nextBlurred,
                                   &nextRect)) {
                        if (wallpaperCommitted || progress >= 0.999f) {
                            baseBitmap = bitmap;
                            basePictureRect = pictureRect;
                            progressBase = 0.0f;
                        } else {
                            progressBase = progress;
                        }
                        target = std::move(latest);
                        bitmap = std::move(nextBitmap);
                        blurredBitmap = std::move(nextBlurred);
                        pictureRect = nextRect;
                        segmentStart = (double)GetTickCount64() / 1000.0;
                        progress = progressBase;
                    } else {
                        // Keep the last renderable destination if the newly
                        // selected file cannot be decoded.
                        target = std::move(latest);
                    }
                }

                const double now = (double)GetTickCount64() / 1000.0;
                const float segment = EaseWallpaperProgress(
                    now - segmentStart, duration);
                progress = progressBase + (1.0f - progressBase) * segment;
                if (!canvas.Begin(size)) {
                    break;
                }
                DrawWallpaperTransitionFrame(
                    factory.get(), canvas.target.get(), bitmap.get(),
                    blurredBitmap.get(), pictureRect, size,
                    {target.origin.x - change->monitor.left,
                     target.origin.y - change->monitor.top},
                    backgroundColor, target.transition, target.wipeDirection,
                    target.dissolveSeed, target.splitRevealDirection, progress,
                    baseBitmap.get(), baseBitmap ? &basePictureRect : nullptr);
                if (!canvas.Present(overlay, position)) {
                    break;
                }
                if (!shown) {
                    ShowWindow(overlay, SW_SHOWNOACTIVATE);
                    PlaceWallpaperOverlay(overlay);
                    shown = true;
                }
                if (progress >= 1.0f) {
                    break;
                }
                Sleep(16);
            }

            if (!shown || !bitmap) {
                break;
            }

            // A click can land on the final frame before the wallpaper API
            // call. Turn the completed frame into the base of the next effect.
            WallpaperTargetRequest latest = ReadWallpaperRequest();
            if (latest.generation != target.generation && !latest.path.empty()) {
                baseBitmap = bitmap;
                basePictureRect = pictureRect;
                winrt::com_ptr<ID2D1Bitmap> nextBitmap;
                winrt::com_ptr<ID2D1Bitmap> nextBlurred;
                D2D1_RECT_F nextRect{};
                if (loadTarget(latest, &nextBitmap, &nextBlurred, &nextRect)) {
                    target = std::move(latest);
                    bitmap = std::move(nextBitmap);
                    blurredBitmap = std::move(nextBlurred);
                    pictureRect = nextRect;
                    progressBase = 0.0f;
                    segmentStart = (double)GetTickCount64() / 1000.0;
                    continue;
                }
                target = std::move(latest);
            }

            // Keep the overlay capturable while Windows applies the wallpaper.
            // WDA_EXCLUDEFROMCAPTURE made OBS record the still-visible desktop
            // underneath this overlay, exposing the previous wallpaper.
            DwmFlush();
            applied = SetWallpaperForMonitor(change->monitor,
                                               target.path.c_str());
            wallpaperCommitted = applied;

            bool retargeted = false;
            const ULONGLONG settleStart = GetTickCount64();
            while (applied && GetTickCount64() - settleStart < 650) {
                Sleep(25);
                latest = ReadWallpaperRequest();
                if (latest.generation != target.generation &&
                    !latest.path.empty()) {
                    winrt::com_ptr<ID2D1Bitmap> nextBitmap;
                    winrt::com_ptr<ID2D1Bitmap> nextBlurred;
                    D2D1_RECT_F nextRect{};
                    if (loadTarget(latest, &nextBitmap, &nextBlurred,
                                   &nextRect)) {
                        baseBitmap = bitmap;
                        basePictureRect = pictureRect;
                        target = std::move(latest);
                        bitmap = std::move(nextBitmap);
                        blurredBitmap = std::move(nextBlurred);
                        pictureRect = nextRect;
                        progressBase = 0.0f;
                        segmentStart = (double)GetTickCount64() / 1000.0;
                        retargeted = true;
                        break;
                    }
                }
            }
            if (retargeted) {
                continue;
            }
            DwmFlush();

            // Fade the final effect away. If a new request arrives during the
            // fade, restore full overlay opacity and retarget over the image
            // that Windows has just committed.
            bool restart = false;
            for (int step = 8; step >= 0; --step) {
                latest = ReadWallpaperRequest();
                if (latest.generation != target.generation &&
                    !latest.path.empty()) {
                    winrt::com_ptr<ID2D1Bitmap> nextBitmap;
                    winrt::com_ptr<ID2D1Bitmap> nextBlurred;
                    D2D1_RECT_F nextRect{};
                    if (loadTarget(latest, &nextBitmap, &nextBlurred,
                                   &nextRect)) {
                        baseBitmap = bitmap;
                        basePictureRect = pictureRect;
                        target = std::move(latest);
                        bitmap = std::move(nextBitmap);
                        blurredBitmap = std::move(nextBlurred);
                        pictureRect = nextRect;
                        progressBase = 0.0f;
                        segmentStart = (double)GetTickCount64() / 1000.0;
                        restart = true;
                        break;
                    }
                    target = std::move(latest);
                }
                canvas.SetAlpha(overlay, position,
                                (BYTE)(step * 255 / 8));
                Sleep(25);
            }
            if (restart) {
                continue;
            }

            latest = ReadWallpaperRequest();
            if (latest.generation != target.generation && !latest.path.empty()) {
                // Close the small race between the last fade frame and exit.
                winrt::com_ptr<ID2D1Bitmap> nextBitmap;
                winrt::com_ptr<ID2D1Bitmap> nextBlurred;
                D2D1_RECT_F nextRect{};
                if (loadTarget(latest, &nextBitmap, &nextBlurred, &nextRect)) {
                    baseBitmap = bitmap;
                    basePictureRect = pictureRect;
                    bitmap = std::move(nextBitmap);
                    blurredBitmap = std::move(nextBlurred);
                    pictureRect = nextRect;
                    target = std::move(latest);
                    progressBase = 0.0f;
                    segmentStart = (double)GetTickCount64() / 1000.0;
                    continue;
                }
                target = std::move(latest);
            }
            finished = true;
        }
    } else {
        // Rendering setup failed: apply the most recent request directly.
        target = ReadWallpaperRequest();
        applied = SetWallpaperForMonitor(change->monitor, target.path.c_str());
    }

    if (overlay) {
        RemovePropW(overlay, L"NonRudeHWND");
        DestroyWindow(overlay);
    }
    blurredBitmap = nullptr;
    baseBitmap = nullptr;
    bitmap = nullptr;
    canvas.Destroy();
    canvas.target = nullptr;
    factory = nullptr;
    wic = nullptr;
    wallpaper = nullptr;
    if (SUCCEEDED(com)) {
        CoUninitialize();
    }
    PostMessageW(change->notifyWindow, kWallpaperChangeDone,
                 (WPARAM)target.imageIndex, applied ? 1 : 0);
    return 0;
}

WallpaperTransition ChooseWallpaperTransition() {
    static std::mt19937 random(
        (unsigned)GetTickCount() ^ (unsigned)(uintptr_t)&g_wallpaperThread);
    static std::vector<int> randomTransitionBag;
    if (g_transitionMode >= 0 && g_transitionMode < 8) {
        randomTransitionBag.clear();
        g_lastRandomTransition = g_transitionMode;
        return (WallpaperTransition)g_transitionMode;
    }

    if (randomTransitionBag.empty()) {
        for (int transition = 0; transition < 8; ++transition) {
            randomTransitionBag.push_back(transition);
        }
        std::shuffle(randomTransitionBag.begin(), randomTransitionBag.end(),
                     random);
        if (randomTransitionBag.size() > 1 &&
            randomTransitionBag.back() == g_lastRandomTransition) {
            const int previous = g_lastRandomTransition;
            const auto alternative = std::find_if(
                randomTransitionBag.begin(), randomTransitionBag.end() - 1,
                [previous](int transition) { return transition != previous; });
            if (alternative != randomTransitionBag.end() - 1) {
                std::iter_swap(alternative, randomTransitionBag.end() - 1);
            }
        }
    }

    const int choice = randomTransitionBag.back();
    randomTransitionBag.pop_back();
    g_lastRandomTransition = choice;
    return (WallpaperTransition)choice;
}
void StartWallpaperChange(int image) {
    if (g_shuttingDown.load() || image < 0 || image >= (int)g_images.size() ||
        !g_hostWindow) {
        return;
    }
    POINT cursor{};
    GetCursorPos(&cursor);
    HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info{sizeof(info)};
    if (!monitor || !GetMonitorInfoW(monitor, &info)) {
        Wh_Log(L"Could not identify target monitor for '%ls'",
               g_images[image].c_str());
        return;
    }

    if (g_transitionActive && !EqualRect(&g_transitionMonitor, &info.rcMonitor)) {
        // A single overlay belongs to one monitor. Preserve the in-flight
        // effect and apply cross-monitor requests directly to avoid misplacing it.
        if (SetWallpaperForMonitor(info.rcMonitor, g_images[image].c_str())) {
            g_currentImage = image;
            g_transitionCurrentSuperseded = true;
            PersistSelectedImage(image);
        }
        return;
    }
    if (!g_transitionActive &&
        IsWallpaperAlreadySet(info.rcMonitor, g_images[image].c_str())) {
        g_currentImage = image;
        PersistSelectedImage(image);
        for (HWND tile : g_tiles) {
            if (tile) {
                InvalidateRect(tile, nullptr, FALSE);
            }
        }
        return;
    }

    const WallpaperTransition transition = ChooseWallpaperTransition();
    static std::mt19937 variationRandom(
        (unsigned)GetTickCount() ^ (unsigned)(uintptr_t)&g_transitionPath);
    std::uniform_int_distribution<int> directionDistribution(0, 3);
    const int wipeDirection = transition == WallpaperTransition::Wipe
                                  ? directionDistribution(variationRandom)
                                  : 0;
    const int splitRevealDirection =
        transition == WallpaperTransition::SplitReveal
            ? directionDistribution(variationRandom)
            : 0;
    const std::uint32_t dissolveSeed =
        (std::uint32_t)GetTickCount() ^
        ((std::uint32_t)image * 0x9E3779B9u) ^
        (std::uint32_t)(uintptr_t)&g_transitionPath;
    WallpaperChange request{
        g_hostWindow, image, g_images[image], info.rcMonitor,
        transition, cursor, wipeDirection, dissolveSeed,
        splitRevealDirection};

    if (g_transitionActive) {
        if (request.path == g_transitionPath) {
            return;
        }
        PublishWallpaperRequest(request);
        g_transitionPath = request.path;
        return;
    }

    if (g_wallpaperThread) {
        CloseHandle(g_wallpaperThread);
        g_wallpaperThread = nullptr;
    }
    PublishWallpaperRequest(request);
    g_transitionPath = request.path;
    g_transitionMonitor = info.rcMonitor;
    g_transitionCurrentSuperseded = false;
    g_transitionActive = true;
    auto* change = new WallpaperChange(request);
    g_wallpaperThread = CreateThread(nullptr, 0, WallpaperChangeThreadProc,
                                     change, 0, nullptr);
    if (!g_wallpaperThread) {
        Wh_Log(L"Transition worker creation failed; applying wallpaper directly");
        delete change;
        g_transitionActive = false;
        g_transitionPath.clear();
        if (SetWallpaperForMonitor(info.rcMonitor, g_images[image].c_str())) {
            g_currentImage = image;
            for (HWND tile : g_tiles) {
                if (tile) {
                    InvalidateRect(tile, nullptr, FALSE);
                }
            }
        } else {
            Wh_Log(L"Failed to apply wallpaper directly for '%ls'",
                   g_images[image].c_str());
        }
    }
}

void ApplyImageIndex(int image) {
    if (image < 0 || image >= (int)g_images.size()) {
        return;
    }
    if (g_searchEdit && GetFocus() == g_searchEdit) {
        const size_t slot = SlotForImageIndex((size_t)image);
        if (slot < kPageSize && g_tiles[slot] &&
            IsWindowVisible(g_tiles[slot])) {
            SetFocus(g_tiles[slot]);
        } else if (g_pickerWindow) {
            SetFocus(g_pickerWindow);
        }
    }
    PersistSelectedImage(image);
    UpdateFolderLabel();
    for (HWND tile : g_tiles) {
        if (tile) {
            InvalidateRect(tile, nullptr, FALSE);
        }
    }

    StartWallpaperChange(image);
}

void StepImage(int step) {
    if (g_images.empty()) {
        ShowPicker();
        return;
    }
    int current = g_currentImage;
    if (g_transitionActive) {
        const auto active = std::find(g_images.begin(), g_images.end(), g_transitionPath);
        if (active != g_images.end()) {
            current = (int)std::distance(g_images.begin(), active);
        }
    }
    if (current < 0) {
        current = step > 0 ? -1 : 0;
    }
    const int next = (current + step + (int)g_images.size()) %
                     (int)g_images.size();
    g_page = PageForImageIndex((size_t)next);
    RefreshList();
    ApplyImageIndex(next);
}

void StepPage(int step) {
    const int pages = (int)std::max<size_t>(
        1, (DisplayImageCount() + kPageSize - 1) / kPageSize);
    const int page = ((int)g_page + step % pages + pages) % pages;
    if ((size_t)page == g_page) {
        return;
    }
    g_page = (size_t)page;
    g_selectedImage = g_visibleImageIndices.empty()
                          ? -1
                          : (int)ImageIndexForPageSlot(g_page, 0);
    RefreshList();
}

void MoveSelection(int delta) {
    if (g_visibleImageIndices.empty()) {
        return;
    }
    const int count = (int)g_visibleImageIndices.size();
    auto selected = std::find(g_visibleImageIndices.begin(),
                              g_visibleImageIndices.end(),
                              g_selectedImage >= 0
                                  ? (size_t)g_selectedImage
                                  : SIZE_MAX);
    int current = selected == g_visibleImageIndices.end()
                      ? 0
                      : (int)std::distance(g_visibleImageIndices.begin(), selected);
    const int nextPosition = (current + delta % count + count) % count;
    const int next = (int)g_visibleImageIndices[(size_t)nextPosition];
    g_selectedImage = next;
    const size_t page = PageForImageIndex((size_t)next);
    if (page != g_page) {
        g_page = page;
        RefreshList();
    } else {
        for (HWND tile : g_tiles) {
            if (tile) {
                InvalidateRect(tile, nullptr, FALSE);
            }
        }
        UpdateFolderLabel();
    }
    const size_t slot = SlotForImageIndex((size_t)next);
    if (g_tiles[slot]) {
        SetFocus(g_tiles[slot]);
    }
}

void FocusSelectedResult() {
    ApplyPendingSearch();
    if (g_selectedImage < 0) {
        if (g_pickerWindow) {
            SetFocus(g_pickerWindow);
        }
        return;
    }

    const size_t page = PageForImageIndex((size_t)g_selectedImage);
    if (page != g_page) {
        g_page = page;
        RefreshList();
    }
    const size_t slot = SlotForImageIndex((size_t)g_selectedImage);
    if (slot < kPageSize && g_tiles[slot] && IsWindowVisible(g_tiles[slot])) {
        SetFocus(g_tiles[slot]);
    }
}

bool HandlePickerKey(UINT key) {
    switch (key) {
        case VK_LEFT:
            MoveSelection(-1);
            return true;
        case VK_RIGHT:
            MoveSelection(1);
            return true;
        case VK_UP:
            MoveSelection(-(int)kGridColumns);
            return true;
        case VK_DOWN:
            MoveSelection((int)kGridColumns);
            return true;
        case VK_SPACE:
        case VK_RETURN:
            if (g_selectedImage >= 0) {
                ApplyImageIndex(g_selectedImage);
            }
            return true;
        default:
            return false;
    }
}

LRESULT CALLBACK TileSubclassProc(HWND hwnd, UINT message, WPARAM wParam,
                                  LPARAM lParam, UINT_PTR, DWORD_PTR) {
    if (message == WM_MOUSEWHEEL && GetDlgCtrlID(hwnd) >= kFirstTileId) {
        StepPage(GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? -1 : 1);
        return 0;
    }
    if (message == WM_KEYDOWN && wParam == VK_ESCAPE) {
        if (!g_searchQuery.empty()) {
            if (g_searchEdit) SetWindowTextW(g_searchEdit, L"");
            UpdateSearchFromEdit();
        } else {
            PostMessageW(GetAncestor(hwnd, GA_ROOT), WM_CLOSE, 0, 0);
        }
        return 0;
    }
    if (message == WM_KEYDOWN && wParam == VK_TAB &&
        (GetKeyState(VK_SHIFT) & 0x8000) && g_searchEdit &&
        g_searchExpanded) {
        SetFocus(g_searchEdit);
        SendMessageW(g_searchEdit, EM_SETSEL, MAXLONG, MAXLONG);
        return 0;
    }
    if (message == WM_CHAR && wParam >= 0x20 && wParam != 0x7F &&
        !g_searchExpanded) {
        SetSearchExpanded(true, (WCHAR)wParam);
        return 0;
    }
    if (message == WM_KEYDOWN && HandlePickerKey((UINT)wParam)) {
        return 0;
    }
    if (message == WM_KEYUP &&
        (wParam == VK_SPACE || wParam == VK_RETURN)) {
        return 0;
    }
    return DefSubclassProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK BreadcrumbSubclassProc(HWND hwnd, UINT message,
                                         WPARAM wParam, LPARAM lParam,
                                         UINT_PTR, DWORD_PTR) {
    if (message == WM_LBUTTONDOWN) {
        const POINT point{(int)(short)LOWORD(lParam),
                          (int)(short)HIWORD(lParam)};
        if (PtInRect(&g_breadcrumbSearchButtonRect, point)) {
            g_searchButtonClickHandled = true;
            SetSearchExpanded(!g_searchExpanded);
            return 0;
        }
    }
    if (message == WM_LBUTTONUP) {
        const POINT point{(int)(short)LOWORD(lParam),
                          (int)(short)HIWORD(lParam)};
        if (g_searchButtonClickHandled) {
            g_searchButtonClickHandled = false;
            return 0;
        }
        if (PtInRect(&g_breadcrumbSearchButtonRect, point)) {
            SetSearchExpanded(true);
            return 0;
        }
        if (g_suppressedBreadcrumbClickSegment >= 0) {
            const ULONGLONG now = GetTickCount64();
            if (now > g_suppressBreadcrumbClickUntil) {
                g_suppressedBreadcrumbClickSegment = -1;
            } else {
                for (const BreadcrumbHitTarget& target :
                     g_breadcrumbHitTargets) {
                    if ((int)target.segmentIndex ==
                            g_suppressedBreadcrumbClickSegment &&
                        target.hasMenu &&
                        PtInRect(&target.menuRect, point)) {
                        // Consume the release from the click that just closed
                        // the popup; it must not be treated as a new click.
                        g_suppressedBreadcrumbClickSegment = -1;
                        return 0;
                    }
                }
            }
        }
        for (const BreadcrumbHitTarget& target : g_breadcrumbHitTargets) {
            if (target.hasMenu && PtInRect(&target.menuRect, point)) {
                if (g_openBreadcrumbMenuSegment ==
                    (int)target.segmentIndex) {
                    // The popup's nested message loop can deliver this click
                    // back to the breadcrumb. End tracking without reopening.
                    EndMenu();
                    return 0;
                }
                POINT anchor = point;
                ClientToScreen(hwnd, &anchor);
                ShowBreadcrumbFolderMenu(target.segmentIndex, anchor);
                return 0;
            }
            if (PtInRect(&target.labelRect, point)) {
                if (target.segmentIndex == 0) {
                    HWND parent = GetParent(hwnd);
                    SendMessageW(parent, WM_COMMAND,
                                 MAKEWPARAM(kChooseFolder, STN_CLICKED),
                                 (LPARAM)hwnd);
                } else {
                    NavigateToFolder(
                        g_breadcrumbSegments[target.segmentIndex].folderPath);
                }
                return 0;
            }
        }
        return 0;
    }
    if (message == WM_MOUSEMOVE) {
        TRACKMOUSEEVENT track{};
        track.cbSize = sizeof(track);
        track.dwFlags = TME_LEAVE;
        track.hwndTrack = hwnd;
        TrackMouseEvent(&track);
        const POINT point{(int)(short)LOWORD(lParam),
                          (int)(short)HIWORD(lParam)};
        const bool searchHovered = PtInRect(&g_breadcrumbSearchButtonRect, point);
        if (searchHovered != g_searchButtonHovered) {
            g_searchButtonHovered = searchHovered;
            InvalidateRect(hwnd, nullptr, FALSE);
        }

        int hoveredSegment = -1;
        for (const BreadcrumbHitTarget& target : g_breadcrumbHitTargets) {
            if (PtInRect(&target.hoverRect, point)) {
                hoveredSegment = (int)target.segmentIndex;
                break;
            }
        }
        if (hoveredSegment != g_hoverBreadcrumbSegment) {
            const int previousSegment = g_hoverBreadcrumbSegment;
            g_hoverBreadcrumbSegment = hoveredSegment;
            InvalidateBreadcrumbSegment(hwnd, previousSegment);
            InvalidateBreadcrumbSegment(hwnd, hoveredSegment);
        }
    } else if (message == WM_MOUSELEAVE) {
        if (g_searchButtonHovered) {
            g_searchButtonHovered = false;
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        if (g_hoverBreadcrumbSegment != -1) {
            const int previousSegment = g_hoverBreadcrumbSegment;
            g_hoverBreadcrumbSegment = -1;
            InvalidateBreadcrumbSegment(hwnd, previousSegment);
        }
    }
    if (message == WM_SETCURSOR && LOWORD(lParam) == HTCLIENT) {
        POINT cursor{};
        GetCursorPos(&cursor);
        ScreenToClient(hwnd, &cursor);
        bool clickable = false;
        clickable = PtInRect(&g_breadcrumbSearchButtonRect, cursor);
        for (const BreadcrumbHitTarget& target : g_breadcrumbHitTargets) {
            if (PtInRect(&target.hoverRect, cursor)) {
                clickable = true;
                break;
            }
        }
        SetCursor(LoadCursorW(nullptr, clickable ? IDC_HAND : IDC_ARROW));
        return TRUE;
    }
    return DefSubclassProc(hwnd, message, wParam, lParam);
}

void PositionPickerWindow(HMONITOR monitor) {
    if (!g_pickerWindow) {
        return;
    }
    if (!monitor) {
        monitor = MonitorFromWindow(g_pickerWindow, MONITOR_DEFAULTTONEAREST);
    }
    MONITORINFO info{sizeof(info)};
    if (!monitor || !GetMonitorInfoW(monitor, &info)) {
        return;
    }

    RECT windowRect{};
    int width = 560;
    int height = 360;
    if (GetWindowRect(g_pickerWindow, &windowRect)) {
        width = windowRect.right - windowRect.left;
        height = windowRect.bottom - windowRect.top;
    }

    const RECT& work = info.rcWork;
    const int workWidth = work.right - work.left;
    const int workHeight = work.bottom - work.top;
    const int horizontalMargin =
        std::min(kPickerEdgeMargin, std::max(0, (workWidth - width) / 2));
    const int verticalMargin =
        std::min(kPickerEdgeMargin, std::max(0, (workHeight - height) / 2));
    const LONG left = work.left + horizontalMargin;
    const LONG right = std::max<LONG>(
        left, work.right - width - horizontalMargin);
    const LONG top = work.top + verticalMargin;
    const LONG bottom = std::max<LONG>(
        top, work.bottom - height - verticalMargin);

    LONG x = left + std::max<LONG>(0, (right - left) / 2);
    LONG y = top + std::max<LONG>(0, (bottom - top) / 2);
    const bool isLeft = g_pickerPosition == L"top-left" ||
                        g_pickerPosition == L"middle-left" ||
                        g_pickerPosition == L"bottom-left";
    const bool isRight = g_pickerPosition == L"top-right" ||
                         g_pickerPosition == L"middle-right" ||
                         g_pickerPosition == L"bottom-right";
    const bool isTop = g_pickerPosition == L"top-left" ||
                       g_pickerPosition == L"top-center" ||
                       g_pickerPosition == L"top-right";
    const bool isBottom = g_pickerPosition == L"bottom-left" ||
                          g_pickerPosition == L"bottom-center" ||
                          g_pickerPosition == L"bottom-right";
    if (isLeft) {
        x = left;
    } else if (isRight) {
        x = right;
    }
    if (isTop) {
        y = top;
    } else if (isBottom) {
        y = bottom;
    }
    // Middle-left and middle-right already use the vertical center; the
    // three center-column anchors already use the horizontal center.
    x = std::clamp<LONG>(x + g_pickerOffsetX, work.left,
                         std::max<LONG>(work.left, work.right - width));
    y = std::clamp<LONG>(y + g_pickerOffsetY, work.top,
                         std::max<LONG>(work.top, work.bottom - height));
    SetWindowPos(g_pickerWindow, HWND_TOPMOST, (int)x, (int)y, width, height,
                 SWP_NOACTIVATE);
}

void ShowPicker() {
    if (!g_pickerWindow) {
        return;
    }
    if (!g_folder.empty() && g_images.empty() && !g_imageScanInProgress) {
        StartImageScan();
    }
    if (!IsWindowVisible(g_pickerWindow)) {
        HWND foreground = GetForegroundWindow();
        if (foreground && GetAncestor(foreground, GA_ROOT) != g_pickerWindow &&
            foreground != g_pickerBackdropWindow) {
            g_previousForegroundWindow =
                GetAncestor(foreground, GA_ROOT);
        }
        POINT cursor{};
        GetCursorPos(&cursor);
        HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
        PositionPickerWindow(monitor);
    }
    const bool currentIsVisible = g_currentImage >= 0 &&
        std::find(g_visibleImageIndices.begin(), g_visibleImageIndices.end(),
                  (size_t)g_currentImage) != g_visibleImageIndices.end();
    if (currentIsVisible) {
        // Reopening returns to the last wallpaper applied through the mod,
        // even if the user browsed to another page before closing the picker.
        g_selectedImage = g_currentImage;
        g_page = PageForImageIndex((size_t)g_currentImage);
        RefreshList();
    } else if (g_selectedImage < 0 ||
               std::find(g_visibleImageIndices.begin(), g_visibleImageIndices.end(),
                         (size_t)g_selectedImage) == g_visibleImageIndices.end()) {
        g_selectedImage = g_visibleImageIndices.empty()
                              ? -1
                              : (int)ImageIndexForPageSlot(g_page, 0);
    }
    const bool usesBackdrop =
        _wcsicmp(g_configuredMaterial.c_str(), L"blur") == 0 ||
        _wcsicmp(g_configuredMaterial.c_str(), L"acrylic") == 0;
    if (!IsWindowVisible(g_pickerWindow)) {
        // AnimateWindow's alpha blend conflicts with the color-keyed layered
        // surface used to reveal the material backdrop. Show it directly.
        if (usesBackdrop ||
            !AnimateWindow(g_pickerWindow, 160, AW_BLEND | AW_ACTIVATE)) {
            ShowWindow(g_pickerWindow, SW_SHOWNORMAL);
        }
    }
    PositionPickerBackdrop();
    SetForegroundWindow(g_pickerWindow);
    if (g_selectedImage >= 0 && !g_transitionActive) {
        const size_t slot = SlotForImageIndex((size_t)g_selectedImage);
        if (g_tiles[slot]) {
            SetFocus(g_tiles[slot]);
        }
    }
    // AnimateWindow can show the parent before every owner-drawn child has
    // completed its first paint. Refresh the whole tree once it is visible.
    RedrawWindow(g_pickerWindow, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW);
}

void TogglePicker() {
    if (!g_pickerWindow) {
        return;
    }
    if (IsWindowVisible(g_pickerWindow)) {
        PostMessageW(g_pickerWindow, WM_CLOSE, 0, 0);
    } else {
        ShowPicker();
    }
}

HFONT CreatePickerFont(HWND hwnd) {
    UINT dpi = GetDpiForWindow(hwnd);
    if (!dpi) {
        dpi = 96;
    }
    return CreateFontW(-MulDiv(10, (int)dpi, 72), 0, 0, 0, FW_NORMAL,
                       FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                       OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       ANTIALIASED_QUALITY, VARIABLE_PITCH | FF_DONTCARE,
                       L"Segoe UI Variable Text");
}

void DrawRotatedBreadcrumbChevron(HDC dc, const RECT& rect,
                                  float angleDegrees) {
    const WCHAR chevron[] = {0xF745, L'\0'};
    if (g_breadcrumbIconFont) {
        Gdiplus::Graphics graphics(dc);
        Gdiplus::Font iconFont(dc, g_breadcrumbIconFont);
        if (graphics.GetLastStatus() == Gdiplus::Ok &&
            iconFont.GetLastStatus() == Gdiplus::Ok) {
            graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            graphics.SetTextRenderingHint(
                Gdiplus::TextRenderingHintAntiAliasGridFit);
            const Gdiplus::REAL width =
                (Gdiplus::REAL)(rect.right - rect.left);
            const Gdiplus::REAL height =
                (Gdiplus::REAL)(rect.bottom - rect.top);
            graphics.TranslateTransform(
                (Gdiplus::REAL)(rect.left + rect.right) / 2.0f,
                (Gdiplus::REAL)(rect.top + rect.bottom) / 2.0f + 1.0f);
            graphics.RotateTransform(angleDegrees);
            Gdiplus::RectF layout(-width / 2.0f, -height / 2.0f,
                                  width, height);
            Gdiplus::StringFormat format;
            format.SetAlignment(Gdiplus::StringAlignmentCenter);
            format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            Gdiplus::SolidBrush brush(ToGdiPlusColor(g_muted));
            graphics.DrawString(chevron, 1, &iconFont, layout, &format,
                                &brush);
            return;
        }
    }

    const WCHAR fallbackRight[] = {0x203A, L'\0'};
    const WCHAR fallbackDown[] = {0x25BE, L'\0'};
    const WCHAR* fallback = angleDegrees >= 45.0f
                                ? fallbackDown
                                : fallbackRight;
    SetTextColor(dc, g_muted);
    RECT fallbackRect = rect;
    DrawTextW(dc, fallback, 1, &fallbackRect,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
}

bool DrawBreadcrumb(const DRAWITEMSTRUCT* item) {
    if (!item || item->CtlType != ODT_STATIC ||
        item->CtlID != kFolderLabelId) {
        return false;
    }

    HDC targetDc = item->hDC;
    const int bufferWidth = item->rcItem.right - item->rcItem.left;
    const int bufferHeight = item->rcItem.bottom - item->rcItem.top;
    HDC bufferDc = nullptr;
    HBITMAP bufferBitmap = nullptr;
    HGDIOBJ oldBitmap = nullptr;
    HDC dc = targetDc;
    if (bufferWidth > 0 && bufferHeight > 0) {
        bufferDc = CreateCompatibleDC(targetDc);
        if (bufferDc) {
            bufferBitmap = CreateCompatibleBitmap(
                targetDc, bufferWidth, bufferHeight);
            if (bufferBitmap) {
                oldBitmap = SelectObject(bufferDc, bufferBitmap);
                SetViewportOrgEx(bufferDc, -item->rcItem.left,
                                 -item->rcItem.top, nullptr);
                dc = bufferDc;
            } else {
                DeleteDC(bufferDc);
                bufferDc = nullptr;
            }
        }
    }

    g_breadcrumbHitTargets.clear();
    FillRect(dc, &item->rcItem, g_backgroundBrush);
    const int saved = SaveDC(dc);
    if (!saved) {
        if (bufferDc && bufferBitmap) {
            if (oldBitmap) {
                SelectObject(bufferDc, oldBitmap);
            }
            DeleteObject(bufferBitmap);
            DeleteDC(bufferDc);
        }
        return true;
    }
    SetBkMode(dc, TRANSPARENT);
    HGDIOBJ oldFont = g_pickerFont
                          ? SelectObject(dc, g_pickerFont)
                          : nullptr;

    const int left = item->rcItem.left + 1;
    const int right = item->rcItem.right - kBreadcrumbSearchButtonWidth - 1;
    const int itemHeight = item->rcItem.bottom - item->rcItem.top;
    constexpr int separatorGap = 8;
    constexpr int breadcrumbIconGap = 5;
    constexpr int chipHorizontalPadding = 4;
    constexpr int separatorWidth = 16;
    int x = left;

    const WCHAR folderGlyph[] = {0xE8B7, L'\0'};
    const WCHAR pathSeparator[] = L"\\";

    if (g_breadcrumbIconFont && !g_breadcrumbSegments.empty()) {
        HGDIOBJ priorFont = SelectObject(dc, g_breadcrumbIconFont);
        SIZE iconSize{};
        GetTextExtentPoint32W(dc, folderGlyph, 1, &iconSize);
        RECT iconRect{x + chipHorizontalPadding, item->rcItem.top, right,
                      item->rcItem.bottom};
        SetTextColor(dc, g_muted);
        DrawTextW(dc, folderGlyph, 1, &iconRect,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        if (priorFont) {
            SelectObject(dc, priorFont);
        }
        x += chipHorizontalPadding + (int)iconSize.cx + breadcrumbIconGap;
    }

    for (size_t i = 0; i < g_breadcrumbSegments.size(); ++i) {
        const BreadcrumbSegment& segment = g_breadcrumbSegments[i];
        SIZE textSize{};
        GetTextExtentPoint32W(dc, segment.label.c_str(),
                              (int)segment.label.size(), &textSize);
        if (x >= right) {
            break;
        }

        const int textStart = x;
        const int visibleTextEnd =
            std::min(right, textStart + (int)textSize.cx);
        RECT labelRect{
            (i == 0 ? left : textStart - chipHorizontalPadding) -
                item->rcItem.left,
            0,
            visibleTextEnd - item->rcItem.left,
            itemHeight};
        const bool hasNextSegment = i + 1 < g_breadcrumbSegments.size();
        const bool nextIsFolder =
            hasNextSegment && g_breadcrumbSegments[i + 1].isFolder;
        const bool hasChildren =
            nextIsFolder && segment.isFolder &&
            !DirectImageFolders(segment.folderPath).empty();
        const int separatorStart = textStart + (int)textSize.cx + separatorGap;
        const bool hasMenu = nextIsFolder && hasChildren &&
                             separatorStart + separatorWidth +
                                     chipHorizontalPadding < right;
        RECT menuRect{};
        if (hasMenu) {
            // The chevron button starts right after the label; the gap before
            // its glyph belongs to the same hover and click target.
            menuRect = {
                visibleTextEnd - item->rcItem.left,
                0,
                separatorStart + separatorWidth + chipHorizontalPadding -
                    item->rcItem.left,
                itemHeight};
        } else if (segment.isFolder) {
            labelRect.right += chipHorizontalPadding;
        }

        RECT hoverRect{
            labelRect.left,
            0,
            hasMenu ? menuRect.right : labelRect.right,
            itemHeight};
        const bool chipHovered = segment.isFolder &&
            (g_hoverBreadcrumbSegment == (int)i ||
             g_openBreadcrumbMenuSegment == (int)i ||
             (hasMenu && g_animatedBreadcrumbMenuSegment == (int)i &&
              g_breadcrumbChevronAngle > 0.0f));
        if (chipHovered) {
            RECT paintRect{
                item->rcItem.left + hoverRect.left,
                item->rcItem.top + 2,
                item->rcItem.left + hoverRect.right,
                item->rcItem.bottom - 2};
            Gdiplus::Graphics graphics(dc);
            graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
            FillRoundedMenuItem(graphics, paintRect, 4.0f, g_surface);

            // The root folder glyph belongs to the hovered chip, so paint it
            // after the chip background instead of leaving it outside.
            if (i == 0 && g_breadcrumbIconFont) {
                HGDIOBJ priorFont = SelectObject(dc, g_breadcrumbIconFont);
                RECT iconRect{left + chipHorizontalPadding,
                              item->rcItem.top, right,
                              item->rcItem.bottom};
                SetTextColor(dc, g_muted);
                DrawTextW(dc, folderGlyph, 1, &iconRect,
                          DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
                if (priorFont) {
                    SelectObject(dc, priorFont);
                }
            }
        }

        SetTextColor(dc, g_text);
        RECT textRect{x, item->rcItem.top, right, item->rcItem.bottom};
        DrawTextW(dc, segment.label.c_str(),
                  (int)segment.label.size(), &textRect,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS |
                      DT_NOPREFIX);
        x += (int)textSize.cx;

        if (hasNextSegment && nextIsFolder) {
            x += separatorGap;
            RECT glyphRect{
                x,
                item->rcItem.top,
                x + separatorWidth,
                item->rcItem.bottom};
            const float angle =
                g_animatedBreadcrumbMenuSegment == (int)i
                    ? g_breadcrumbChevronAngle
                    : 0.0f;
            DrawRotatedBreadcrumbChevron(dc, glyphRect, angle);
            x += separatorWidth + separatorGap;
        } else if (hasNextSegment) {
            // Use the same separator slot before the filename.
            x += separatorGap;
            RECT separatorRect{
                x,
                item->rcItem.top,
                x + separatorWidth,
                item->rcItem.bottom};
            SetTextColor(dc, g_muted);
            DrawTextW(dc, pathSeparator, 1, &separatorRect,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            x += separatorWidth + separatorGap;
        }

        if (segment.isFolder) {
            g_breadcrumbHitTargets.push_back(
                {labelRect, menuRect, hoverRect, i, hasMenu});
        }
    }

    const int searchLeft = item->rcItem.right - kBreadcrumbSearchButtonWidth;
    if (g_searchExpanded) {
        RECT searchFieldPaint{
            searchLeft - kBreadcrumbSearchEditWidth,
            item->rcItem.top + 2,
            item->rcItem.right - 1,
            item->rcItem.bottom - 2};
        Gdiplus::Graphics graphics(dc);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        FillRoundedMenuItem(graphics, searchFieldPaint, 6.0f, g_surface);
    }
    g_breadcrumbSearchButtonRect = {
        searchLeft - item->rcItem.left, 0,
        item->rcItem.right - item->rcItem.left, itemHeight};
    RECT searchButtonPaint{searchLeft, item->rcItem.top + 2,
                           item->rcItem.right - 1,
                           item->rcItem.bottom - 2};
    if (!g_searchExpanded && g_searchButtonHovered) {
        Gdiplus::Graphics graphics(dc);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        FillRoundedMenuItem(graphics, searchButtonPaint, 4.0f, g_surface);
    }
    if (g_breadcrumbIconFont) {
        HGDIOBJ priorFont = SelectObject(dc, g_breadcrumbIconFont);
        const WCHAR searchGlyph[] = {0xE721, L'\0'};
        RECT searchGlyphRect{searchLeft, item->rcItem.top,
                            item->rcItem.right - 1, item->rcItem.bottom};
        SetTextColor(dc, g_searchButtonHovered || g_searchExpanded
                             ? g_text : g_muted);
        DrawTextW(dc, searchGlyph, 1, &searchGlyphRect,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        if (priorFont) SelectObject(dc, priorFont);
    }

    if (oldFont) {
        SelectObject(dc, oldFont);
    }
    RestoreDC(dc, saved);
    if (bufferDc && bufferBitmap) {
        BitBlt(targetDc, item->rcItem.left, item->rcItem.top,
               bufferWidth, bufferHeight, bufferDc,
               item->rcItem.left, item->rcItem.top, SRCCOPY);
        if (oldBitmap) {
            SelectObject(bufferDc, oldBitmap);
        }
        DeleteObject(bufferBitmap);
        DeleteDC(bufferDc);
    }
    return true;
}

bool DrawPageLabel(const DRAWITEMSTRUCT* item) {
    if (!item || item->CtlType != ODT_STATIC ||
        item->CtlID != kPageLabelId) {
        return false;
    }

    FillRect(item->hDC, &item->rcItem, g_backgroundBrush);
    const int saved = SaveDC(item->hDC);
    if (!saved) {
        return true;
    }
    SetBkMode(item->hDC, TRANSPARENT);
    SetTextColor(item->hDC, g_muted);
    HGDIOBJ oldFont = g_pickerFont
                          ? SelectObject(item->hDC, g_pickerFont)
                          : nullptr;

    int x = item->rcItem.left;
    const int right = item->rcItem.right;
    const int top = item->rcItem.top;
    const int bottom = item->rcItem.bottom;
    if (g_images.empty() || DisplayImageCount() == 0) {
        WCHAR message[256]{};
        GetWindowTextW(item->hwndItem, message,
                       (int)(sizeof(message) / sizeof(message[0])));
        RECT textRect = item->rcItem;
        DrawTextW(item->hDC, message, -1, &textRect,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS |
                      DT_NOPREFIX);
    } else {
        auto drawText = [&](const WCHAR* text, int length, int gap) {
            SIZE size{};
            GetTextExtentPoint32W(item->hDC, text, length, &size);
            RECT textRect{x, top, right, bottom};
            DrawTextW(item->hDC, text, length, &textRect,
                      DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            x += (int)size.cx + gap;
        };
        auto drawIcon = [&](WCHAR glyph, int gap) {
            if (!g_breadcrumbIconFont) {
                return;
            }
            HGDIOBJ priorFont = SelectObject(item->hDC,
                                             g_breadcrumbIconFont);
            const WCHAR text[] = {glyph, L'\0'};
            SIZE size{};
            GetTextExtentPoint32W(item->hDC, text, 1, &size);
            RECT iconRect{x, top, right, bottom};
            DrawTextW(item->hDC, text, 1, &iconRect,
                      DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            if (priorFont) {
                SelectObject(item->hDC, priorFont);
            }
            x += (int)size.cx + gap;
        };

        const std::wstring pageText =
            std::to_wstring(g_page + 1) + L" / " +
            std::to_wstring((DisplayImageCount() + kPageSize - 1) / kPageSize);
        drawText(pageText.c_str(), (int)pageText.size(), 10);
        const WCHAR separator[] = {0x00B7, L'\0'};
        drawText(separator, 1, 10);

        drawIcon(0xE765, 5);
        const WCHAR arrowKeys[] = L"Arrow keys move";
        drawText(arrowKeys,
                 (int)(sizeof(arrowKeys) / sizeof(arrowKeys[0]) - 1), 8);
        drawText(separator, 1, 8);
        drawIcon(0xE751, 5);
        const WCHAR apply[] = L"Enter/Space applies";
        drawText(apply, (int)(sizeof(apply) / sizeof(apply[0]) - 1), 8);
        drawText(separator, 1, 8);

        const WCHAR scrollPages[] = L"Scroll changes pages";
        drawText(scrollPages,
                 (int)(sizeof(scrollPages) / sizeof(scrollPages[0]) - 1), 0);
    }

    if (oldFont) {
        SelectObject(item->hDC, oldFont);
    }
    RestoreDC(item->hDC, saved);
    return true;
}

bool AddRoundedRectanglePath(Gdiplus::GraphicsPath* path,
                             const Gdiplus::RectF& rect,
                             Gdiplus::REAL radius) {
    if (!path || rect.Width <= 0 || rect.Height <= 0) {
        return false;
    }
    const Gdiplus::REAL diameter = std::min(
        radius * 2.0f, std::min(rect.Width, rect.Height));
    const Gdiplus::REAL cornerRadius = diameter / 2.0f;
    if (cornerRadius <= 0) {
        return false;
    }
    const Gdiplus::REAL right = rect.X + rect.Width;
    const Gdiplus::REAL bottom = rect.Y + rect.Height;
    if (path->AddArc(rect.X, rect.Y, diameter, diameter, 180, 90) !=
            Gdiplus::Ok ||
        path->AddArc(right - diameter, rect.Y, diameter, diameter, 270, 90) !=
            Gdiplus::Ok ||
        path->AddArc(right - diameter, bottom - diameter, diameter, diameter,
                     0, 90) != Gdiplus::Ok ||
        path->AddArc(rect.X, bottom - diameter, diameter, diameter, 90, 90) !=
            Gdiplus::Ok) {
        return false;
    }
    path->CloseFigure();
    return path->GetLastStatus() == Gdiplus::Ok;
}

bool CreateAntialiasedRoundedThumbnail(HBITMAP sourceBitmap,
                                       Gdiplus::Bitmap* roundedBitmap) {
    if (!sourceBitmap || !roundedBitmap) {
        return false;
    }
    Gdiplus::Bitmap source(sourceBitmap, nullptr);
    if (source.GetLastStatus() != Gdiplus::Ok) {
        return false;
    }

    {
        Gdiplus::Graphics content(roundedBitmap);
        if (content.GetLastStatus() != Gdiplus::Ok ||
            content.SetPageUnit(Gdiplus::UnitPixel) != Gdiplus::Ok ||
            content.SetInterpolationMode(
                Gdiplus::InterpolationModeHighQualityBicubic) !=
                Gdiplus::Ok ||
            content.DrawImage(&source,
                              Gdiplus::Rect(0, 0, (INT)kThumbnailWidth,
                                            (INT)kThumbnailHeight),
                              0, 0, (Gdiplus::REAL)source.GetWidth(),
                              (Gdiplus::REAL)source.GetHeight(),
                              Gdiplus::UnitPixel) != Gdiplus::Ok) {
            return false;
        }
    }

    Gdiplus::Bitmap mask(kThumbnailWidth, kThumbnailHeight,
                         PixelFormat32bppARGB);
    if (mask.GetLastStatus() != Gdiplus::Ok) {
        return false;
    }
    {
        Gdiplus::Graphics maskGraphics(&mask);
        if (maskGraphics.GetLastStatus() != Gdiplus::Ok ||
            maskGraphics.SetPageUnit(Gdiplus::UnitPixel) != Gdiplus::Ok ||
            maskGraphics.Clear(Gdiplus::Color(0, 0, 0, 0)) != Gdiplus::Ok ||
            maskGraphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias) !=
                Gdiplus::Ok) {
            return false;
        }
        Gdiplus::GraphicsPath shape;
        if (!AddRoundedRectanglePath(
                &shape,
                Gdiplus::RectF(0.5f, 0.5f,
                               (Gdiplus::REAL)kThumbnailWidth - 1.0f,
                               (Gdiplus::REAL)kThumbnailHeight - 1.0f),
                kThumbnailCornerDiameter / 2.0f)) {
            return false;
        }
        Gdiplus::SolidBrush white(Gdiplus::Color(255, 255, 255, 255));
        if (maskGraphics.FillPath(&white, &shape) != Gdiplus::Ok) {
            return false;
        }
    }

    Gdiplus::Rect lockRect(0, 0, kThumbnailWidth, kThumbnailHeight);
    Gdiplus::BitmapData imageData{};
    Gdiplus::BitmapData maskData{};
    if (roundedBitmap->LockBits(&lockRect,
                                (Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite),
                                PixelFormat32bppPARGB,
                                &imageData) != Gdiplus::Ok) {
        return false;
    }
    if (mask.LockBits(&lockRect, Gdiplus::ImageLockModeRead,
                      PixelFormat32bppARGB, &maskData) != Gdiplus::Ok) {
        roundedBitmap->UnlockBits(&imageData);
        return false;
    }

    for (int y = 0; y < (int)kThumbnailHeight; ++y) {
        BYTE* imageRow = (BYTE*)imageData.Scan0 + y * imageData.Stride;
        const BYTE* maskRow = (const BYTE*)maskData.Scan0 + y * maskData.Stride;
        for (int x = 0; x < (int)kThumbnailWidth; ++x) {
            const BYTE coverage = maskRow[x * 4 + 3];
            BYTE* pixel = imageRow + x * 4;
            for (int channel = 0; channel < 4; ++channel) {
                pixel[channel] = (BYTE)((pixel[channel] * coverage + 127) / 255);
            }
        }
    }
    mask.UnlockBits(&maskData);
    roundedBitmap->UnlockBits(&imageData);
    return true;
}

bool DrawAntialiasedThumbnail(HDC dc, const RECT& rect, HBITMAP bitmap) {
    Gdiplus::Bitmap rounded(kThumbnailWidth, kThumbnailHeight,
                            PixelFormat32bppPARGB);
    if (rounded.GetLastStatus() != Gdiplus::Ok ||
        !CreateAntialiasedRoundedThumbnail(bitmap, &rounded)) {
        return false;
    }
    Gdiplus::Graphics graphics(dc);
    if (graphics.GetLastStatus() != Gdiplus::Ok ||
        graphics.SetPageUnit(Gdiplus::UnitPixel) != Gdiplus::Ok ||
        graphics.SetInterpolationMode(
            Gdiplus::InterpolationModeHighQualityBicubic) != Gdiplus::Ok) {
        return false;
    }
    return graphics.DrawImage(
               &rounded,
               Gdiplus::RectF((Gdiplus::REAL)rect.left,
                              (Gdiplus::REAL)rect.top,
                              (Gdiplus::REAL)(rect.right - rect.left),
                              (Gdiplus::REAL)(rect.bottom - rect.top)),
               0, 0, (Gdiplus::REAL)kThumbnailWidth,
               (Gdiplus::REAL)kThumbnailHeight,
               Gdiplus::UnitPixel) == Gdiplus::Ok;
}

bool DrawAntialiasedTileBorder(HDC dc, const RECT& rect, COLORREF color,
                               int borderWidth) {
    Gdiplus::Graphics graphics(dc);
    if (graphics.GetLastStatus() != Gdiplus::Ok ||
        graphics.SetPageUnit(Gdiplus::UnitPixel) != Gdiplus::Ok ||
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias) !=
            Gdiplus::Ok ||
        graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf) !=
            Gdiplus::Ok) {
        return false;
    }
    const Gdiplus::REAL inset = borderWidth / 2.0f;
    Gdiplus::RectF border(
        (Gdiplus::REAL)rect.left + inset,
        (Gdiplus::REAL)rect.top + inset,
        (Gdiplus::REAL)(rect.right - rect.left) - borderWidth,
        (Gdiplus::REAL)(rect.bottom - rect.top) - borderWidth);
    Gdiplus::GraphicsPath path;
    if (!AddRoundedRectanglePath(
            &path, border,
            std::max(1.0f, kThumbnailCornerDiameter / 2.0f - inset))) {
        return false;
    }
    Gdiplus::Pen pen(
        Gdiplus::Color(255, GetRValue(color), GetGValue(color),
                       GetBValue(color)),
        (Gdiplus::REAL)borderWidth);
    return graphics.DrawPath(&pen, &path) == Gdiplus::Ok;
}

bool DrawBlurClippedThumbnail(HDC targetDc, const RECT& rect,
                              HBITMAP thumbnail) {
    if (!targetDc || !thumbnail) {
        return false;
    }
    BITMAP bitmapInfo{};
    if (!GetObjectW(thumbnail, sizeof(bitmapInfo), &bitmapInfo) ||
        bitmapInfo.bmWidth <= 0 || bitmapInfo.bmHeight <= 0) {
        return false;
    }
    HDC sourceDc = CreateCompatibleDC(targetDc);
    if (!sourceDc) {
        return false;
    }
    HGDIOBJ oldBitmap = SelectObject(sourceDc, thumbnail);
    const int savedDc = SaveDC(targetDc);
    bool drawn = false;
    if (savedDc) {
        HRGN rounded = CreateRoundRectRgn(
            rect.left, rect.top, rect.right, rect.bottom,
            kThumbnailCornerDiameter, kThumbnailCornerDiameter);
        if (rounded) {
            SelectClipRgn(targetDc, rounded);
            SetStretchBltMode(targetDc, HALFTONE);
            SetBrushOrgEx(targetDc, 0, 0, nullptr);
            drawn = StretchBlt(
                targetDc, rect.left, rect.top,
                rect.right - rect.left, rect.bottom - rect.top,
                sourceDc, 0, 0, bitmapInfo.bmWidth, bitmapInfo.bmHeight,
                SRCCOPY) != FALSE;
            DeleteObject(rounded);
        }
        RestoreDC(targetDc, savedDc);
    }
    SelectObject(sourceDc, oldBitmap);
    DeleteDC(sourceDc);
    return drawn;
}

bool DrawTileButton(const DRAWITEMSTRUCT* item) {
    if (!item || item->CtlType != ODT_BUTTON ||
        item->CtlID < kFirstTileId ||
        item->CtlID >= kFirstTileId + kPageSize) {
        return false;
    }
    const size_t slot = item->CtlID - kFirstTileId;
    const size_t index = ImageIndexForPageSlot(g_page, slot);
    FillRect(item->hDC, &item->rcItem, g_backgroundBrush);
    if (index >= g_images.size()) {
        return true;
    }
    HBITMAP thumbnail = CachedThumbnail(g_page, slot);
    if (!thumbnail && HasPendingPage(g_page)) {
        HGDIOBJ oldFont = g_loadingFont
                              ? SelectObject(item->hDC, g_loadingFont)
                              : nullptr;
        const int oldMode = SetBkMode(item->hDC, TRANSPARENT);
        const COLORREF oldColor = SetTextColor(item->hDC, g_muted);
        const WCHAR loadingGlyph[] = {0xF16A, L'\0'};
        RECT glyphRect = item->rcItem;
        DrawTextW(item->hDC, loadingGlyph, -1, &glyphRect,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        SetTextColor(item->hDC, oldColor);
        SetBkMode(item->hDC, oldMode);
        if (oldFont) {
            SelectObject(item->hDC, oldFont);
        }
    }
    bool antialiasedImageDrawn = false;
    const bool blurMaterial =
        _wcsicmp(g_configuredMaterial.c_str(), L"blur") == 0;
    if (thumbnail && blurMaterial) {
        // Avoid antialiased pixels matted against the color-key background.
        // The rounded clip keeps these thumbnail corners clean over Blur.
        antialiasedImageDrawn = DrawBlurClippedThumbnail(
            item->hDC, item->rcItem, thumbnail);
    } else if (thumbnail && g_gdiplusToken) {
        antialiasedImageDrawn = DrawAntialiasedThumbnail(
            item->hDC, item->rcItem, thumbnail);
    }
    if (thumbnail && !antialiasedImageDrawn) {
        BITMAP bitmapInfo{};
        if (GetObjectW(thumbnail, sizeof(bitmapInfo), &bitmapInfo) &&
            bitmapInfo.bmWidth > 0 && bitmapInfo.bmHeight > 0) {
            HDC sourceDc = CreateCompatibleDC(item->hDC);
            if (sourceDc) {
                HGDIOBJ oldBitmap = SelectObject(sourceDc, thumbnail);
                const int savedDc = SaveDC(item->hDC);
                if (savedDc) {
                    HRGN rounded = CreateRoundRectRgn(
                        item->rcItem.left, item->rcItem.top,
                        item->rcItem.right + 1, item->rcItem.bottom + 1,
                        kThumbnailCornerDiameter,
                        kThumbnailCornerDiameter);
                    if (rounded) {
                        SelectClipRgn(item->hDC, rounded);
                        const double scale = std::max(
                            (double)kThumbnailWidth / bitmapInfo.bmWidth,
                            (double)kThumbnailHeight / bitmapInfo.bmHeight);
                        const int cropWidth = std::max(
                            1, (int)std::lround(kThumbnailWidth / scale));
                        const int cropHeight = std::max(
                            1, (int)std::lround(kThumbnailHeight / scale));
                        const int cropX = (int)std::max(
                            0L, (bitmapInfo.bmWidth - cropWidth) / 2);
                        const int cropY = (int)std::max(
                            0L, (bitmapInfo.bmHeight - cropHeight) / 2);
                        SetStretchBltMode(item->hDC, HALFTONE);
                        SetBrushOrgEx(item->hDC, 0, 0, nullptr);
                        if (bitmapInfo.bmBitsPixel == 32) {
                            BLENDFUNCTION blend{AC_SRC_OVER, 0, 255,
                                                AC_SRC_ALPHA};
                            AlphaBlend(item->hDC, item->rcItem.left,
                                       item->rcItem.top, kThumbnailWidth,
                                       kThumbnailHeight, sourceDc, cropX,
                                       cropY, cropWidth, cropHeight, blend);
                        } else {
                            StretchBlt(item->hDC, item->rcItem.left,
                                       item->rcItem.top, kThumbnailWidth,
                                       kThumbnailHeight, sourceDc, cropX,
                                       cropY, cropWidth, cropHeight, SRCCOPY);
                        }
                        DeleteObject(rounded);
                    }
                    RestoreDC(item->hDC, savedDc);
                }
                SelectObject(sourceDc, oldBitmap);
                DeleteDC(sourceDc);
            }
        }
    }
    const bool selected = (int)index == g_selectedImage;
    if (selected || (item->itemState & ODS_HOTLIGHT)) {
        const COLORREF color = selected ? g_accent : g_muted;
        const int borderWidth = selected ? kSelectedBorderWidth : 1;
        const bool antialiasedBorderDrawn =
            g_gdiplusToken && DrawAntialiasedTileBorder(
                                  item->hDC, item->rcItem, color, borderWidth);
        if (!antialiasedBorderDrawn) {
            HPEN pen = CreatePen(PS_SOLID, borderWidth, color);
            HGDIOBJ oldPen = SelectObject(item->hDC, pen);
            HGDIOBJ oldBrush = SelectObject(
                item->hDC, GetStockObject(NULL_BRUSH));
            RECT border = item->rcItem;
            InflateRect(&border, -(borderWidth / 2), -(borderWidth / 2));
            RoundRect(item->hDC, border.left, border.top,
                      border.right, border.bottom,
                      kThumbnailCornerDiameter, kThumbnailCornerDiameter);
            SelectObject(item->hDC, oldBrush);
            SelectObject(item->hDC, oldPen);
            DeleteObject(pen);
        }
    }
    if (item->itemState & ODS_FOCUS) {
        RECT focus = item->rcItem;
        InflateRect(&focus, -4, -4);
        DrawFocusRect(item->hDC, &focus);
    }
    return true;
}

void StylePickerWindow(HWND hwnd) {
    const BOOL dark = TRUE;
    const DWORD corner = 2;  // DWMWCP_ROUNDSMALL
    DwmSetWindowAttribute(hwnd, 20, &dark, sizeof(dark));
    DwmSetWindowAttribute(hwnd, 19, &dark, sizeof(dark));
    DwmSetWindowAttribute(hwnd, 33, &corner, sizeof(corner));
    SetWindowTheme(hwnd, L"DarkMode_Explorer", nullptr);
}

struct PickerAccentPolicy {
    int state;
    int flags;
    DWORD gradientColor;
    int animationId;
};

struct PickerWindowCompositionAttributeData {
    int attribute;
    PVOID data;
    SIZE_T sizeOfData;
};

using SetWindowCompositionAttributeFn =
    BOOL(WINAPI*)(HWND, PickerWindowCompositionAttributeData*);

bool ConfigurePickerBackdrop(HWND hwnd) {
    if (!hwnd) {
        return false;
    }
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    auto setComposition = user32
        ? reinterpret_cast<SetWindowCompositionAttributeFn>(
              GetProcAddress(user32, "SetWindowCompositionAttribute"))
        : nullptr;
    if (!setComposition) {
        return false;
    }

    const bool acrylic = _wcsicmp(g_configuredMaterial.c_str(), L"acrylic") == 0;
    PickerAccentPolicy policy{};
    policy.state = acrylic ? 4 : 3;
    // Keep the legacy Blur state untinted. Its gradient flag renders black
    // on current Windows builds.
    policy.flags = 0;
    // Keep the theme surface visible over desktop colors. Blur needs a
    // stronger tint to blend antialiased thumbnail edges with the base color.
    const DWORD tintOpacity = acrylic ? 192 : 208;
    policy.gradientColor = (tintOpacity << 24) |
                           (static_cast<DWORD>(g_background) & 0x00ffffffu);
    PickerWindowCompositionAttributeData data{19, &policy, sizeof(policy)};
    return setComposition(hwnd, &data) != FALSE;
}

LRESULT CALLBACK PickerBackdropProc(HWND hwnd, UINT message, WPARAM wParam,
                                    LPARAM lParam) {
    switch (message) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT paint{};
            BeginPaint(hwnd, &paint);
            EndPaint(hwnd, &paint);
            return 0;
        }
        case WM_CLOSE:
            ShowWindow(hwnd, SW_HIDE);
            return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

void HidePickerBackdrop() {
    if (g_pickerBackdropWindow) {
        ShowWindow(g_pickerBackdropWindow, SW_HIDE);
    }
}

void HidePickerWindow(HWND hwnd) {
    if (!hwnd) {
        return;
    }
    const bool usesBackdrop =
        _wcsicmp(g_configuredMaterial.c_str(), L"blur") == 0 ||
        _wcsicmp(g_configuredMaterial.c_str(), L"acrylic") == 0;
    const bool wasForeground = GetForegroundWindow() == hwnd;
    if (GetFocus() == g_searchEdit) {
        SetFocus(hwnd);
    }
    KillTimer(hwnd, kSearchFilterTimer);
    const bool hadActiveSearch = !g_searchQuery.empty();
    g_searchExpanded = false;
    g_searchQuery.clear();
    g_pendingSearchQuery.clear();
    if (g_searchEdit) {
        SetWindowTextW(g_searchEdit, L"");
        ShowWindow(g_searchEdit, SW_HIDE);
    }
    if (hadActiveSearch) {
        RebuildSearchResults();
    }
    if (g_folderLabel) {
        InvalidateRect(g_folderLabel, nullptr, FALSE);
    }
    if (usesBackdrop || !AnimateWindow(hwnd, 130, AW_BLEND | AW_HIDE)) {
        ShowWindow(hwnd, SW_HIDE);
    }
    HidePickerBackdrop();
    if (wasForeground && g_previousForegroundWindow &&
        IsWindow(g_previousForegroundWindow)) {
        SetForegroundWindow(g_previousForegroundWindow);
    }
}

void SetPickerRoundRegion(HWND window, bool rounded, int diameter) {
    if (!window) {
        return;
    }
    if (!rounded) {
        SetWindowRgn(window, nullptr, TRUE);
        return;
    }
    RECT bounds{};
    if (!GetWindowRect(window, &bounds)) {
        return;
    }
    const int width = bounds.right - bounds.left;
    const int height = bounds.bottom - bounds.top;
    if (width <= 0 || height <= 0) {
        return;
    }
    HRGN region = CreateRoundRectRgn(0, 0, width, height,
                                     diameter, diameter);
    if (region && !SetWindowRgn(window, region, TRUE)) {
        DeleteObject(region);
    }
    const DWORD roundedCorners = 2;  // DWMWCP_ROUNDSMALL
    DwmSetWindowAttribute(window, 33, &roundedCorners,
                          sizeof(roundedCorners));
}

void PositionPickerBackdrop() {
    if (!g_pickerWindow || !g_pickerBackdropWindow ||
        !IsWindowVisible(g_pickerWindow)) {
        HidePickerBackdrop();
        return;
    }
    RECT bounds{};
    if (!GetWindowRect(g_pickerWindow, &bounds)) {
        HidePickerBackdrop();
        return;
    }
    const int width = bounds.right - bounds.left;
    const int height = bounds.bottom - bounds.top;
    if (width <= 0 || height <= 0) {
        HidePickerBackdrop();
        return;
    }
    // Size and clip the backdrop before showing it. Showing first briefly
    // exposes its rectangular popup surface during material changes.
    SetWindowPos(g_pickerBackdropWindow, g_pickerWindow,
                 bounds.left, bounds.top, width, height,
                 SWP_NOACTIVATE);
    SetPickerRoundRegion(g_pickerBackdropWindow, true, 18);
    ShowWindow(g_pickerBackdropWindow, SW_SHOWNOACTIVATE);
}

void SetPickerColorKey(HWND window, bool enabled) {
    if (!window) {
        return;
    }
    const LONG_PTR style = GetWindowLongPtrW(window, GWL_EXSTYLE);
    if (enabled) {
        if (!(style & WS_EX_LAYERED)) {
            SetWindowLongPtrW(window, GWL_EXSTYLE,
                              style | WS_EX_LAYERED);
            SetWindowPos(window, nullptr, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                             SWP_NOACTIVATE | SWP_FRAMECHANGED);
        }
        SetLayeredWindowAttributes(window, g_background, 0, LWA_COLORKEY);
    } else if (style & WS_EX_LAYERED) {
        SetLayeredWindowAttributes(window, 0, 255, LWA_ALPHA);
        SetWindowLongPtrW(window, GWL_EXSTYLE,
                          style & ~static_cast<LONG_PTR>(WS_EX_LAYERED));
        SetWindowPos(window, nullptr, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                         SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
}

void RedrawPickerChildren() {
    const auto redraw = [](HWND child) {
        if (child) {
            RedrawWindow(child, nullptr, nullptr,
                         RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);
        }
    };
    redraw(g_folderLabel);
    redraw(g_searchEdit);
    redraw(g_pageLabel);
    for (HWND tile : g_tiles) {
        redraw(tile);
    }
}

void ApplyPickerMaterial(HWND hwnd) {
    if (!hwnd) {
        return;
    }
    const bool wantsBackdrop =
        _wcsicmp(g_configuredMaterial.c_str(), L"blur") == 0 ||
        _wcsicmp(g_configuredMaterial.c_str(), L"acrylic") == 0;
    const bool canUseBackdrop =
        wantsBackdrop && g_pickerBackdropWindow &&
        ConfigurePickerBackdrop(g_pickerBackdropWindow);
    if (!canUseBackdrop) {
        HidePickerBackdrop();
        SetPickerColorKey(hwnd, false);
        RedrawPickerChildren();
        SetPickerRoundRegion(hwnd, false, 0);
        SetPickerRoundRegion(g_pickerBackdropWindow, false, 0);
        return;
    }

    // Reveal the backdrop through color-key transparency, then apply the
    // matching clip region. Applying the region after the layered-style change
    // avoids Windows retaining the previous rectangular shape.
    SetPickerColorKey(hwnd, true);
    SetPickerRoundRegion(hwnd, true, 18);
    PositionPickerBackdrop();
    RedrawPickerChildren();
}

void RefreshPickerAppearance(HWND hwnd) {
    ApplyPickerThemeColors();
    if (g_backgroundBrush) {
        DeleteObject(g_backgroundBrush);
    }
    g_backgroundBrush = CreateSolidBrush(g_background);
    if (g_searchBrush) {
        DeleteObject(g_searchBrush);
    }
    g_searchBrush = CreateSolidBrush(g_surface);
    ApplyPickerMaterial(hwnd);
    if (hwnd) {
        RedrawWindow(hwnd, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN |
                         RDW_UPDATENOW);
    }
}

HICON CreateWallpaperTrayIcon() {
    constexpr int size = 32;
    HDC screen = GetDC(nullptr);
    if (!screen) {
        return nullptr;
    }
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = size;
    info.bmiHeader.biHeight = -size;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP color = CreateDIBSection(screen, &info, DIB_RGB_COLORS,
                                     &bits, nullptr, 0);
    ReleaseDC(nullptr, screen);
    if (!color || !bits) {
        if (color) DeleteObject(color);
        return nullptr;
    }
    std::fill_n((DWORD*)bits, size * size, 0);
    HDC dc = CreateCompatibleDC(nullptr);
    if (!dc) {
        DeleteObject(color);
        return nullptr;
    }
    HGDIOBJ oldBitmap = SelectObject(dc, color);
    HFONT font = CreateFontW(-26, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                             VARIABLE_PITCH | FF_DONTCARE,
                             L"Segoe Fluent Icons");
    HGDIOBJ oldFont = font ? SelectObject(dc, font) : nullptr;
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(255, 255, 255));
    RECT rect{0, 0, size, size};
    const WCHAR glyph[] = {0xE8B9, L'\0'};
    DrawTextW(dc, glyph, -1, &rect,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
    GdiFlush();
    DWORD* pixels = (DWORD*)bits;
    for (int i = 0; i < size * size; ++i) {
        const BYTE red = (BYTE)(pixels[i] >> 16);
        const BYTE green = (BYTE)(pixels[i] >> 8);
        const BYTE blue = (BYTE)pixels[i];
        const BYTE coverage = std::max(red, std::max(green, blue));
        const BYTE r = (BYTE)(GetRValue(g_text) * coverage / 255);
        const BYTE g = (BYTE)(GetGValue(g_text) * coverage / 255);
        const BYTE b = (BYTE)(GetBValue(g_text) * coverage / 255);
        pixels[i] = ((DWORD)coverage << 24) | ((DWORD)r << 16) |
                    ((DWORD)g << 8) | b;
    }
    if (oldFont) SelectObject(dc, oldFont);
    if (font) DeleteObject(font);
    SelectObject(dc, oldBitmap);
    DeleteDC(dc);

    BYTE maskBits[size * 4]{};
    HBITMAP mask = CreateBitmap(size, size, 1, 1, maskBits);
    ICONINFO iconInfo{};
    iconInfo.fIcon = TRUE;
    iconInfo.hbmMask = mask;
    iconInfo.hbmColor = color;
    HICON icon = mask ? CreateIconIndirect(&iconInfo) : nullptr;
    if (mask) DeleteObject(mask);
    DeleteObject(color);
    return icon;
}

void AddTrayIcon(HWND hwnd) {
    g_notifyIcon = {};
    g_notifyIcon.cbSize = sizeof(g_notifyIcon);
    g_notifyIcon.hWnd = hwnd;
    g_notifyIcon.uID = 1;
    g_notifyIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    g_notifyIcon.uCallbackMessage = kTrayCallback;
    g_trayIcon = CreateWallpaperTrayIcon();
    g_notifyIcon.hIcon = g_trayIcon ? g_trayIcon : LoadIcon(nullptr, IDI_APPLICATION);
    wcscpy_s(g_notifyIcon.szTip, L"Simple Wallpaper Switcher");
    Shell_NotifyIconW(NIM_ADD, &g_notifyIcon);
    g_notifyIcon.uVersion = NOTIFYICON_VERSION_4;
    Shell_NotifyIconW(NIM_SETVERSION, &g_notifyIcon);
}

void ShowTrayMenu(HWND hwnd, POINT callbackPoint) {
    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return;
    }

    const auto appendOwnerDrawItem = [menu](UINT id, PCWSTR label) {
        MENUITEMINFOW item{};
        item.cbSize = sizeof(item);
        item.fMask = MIIM_FTYPE | MIIM_ID | MIIM_DATA;
        item.fType = MFT_OWNERDRAW;
        item.wID = id;
        item.dwItemData = (ULONG_PTR)label;
        InsertMenuItemW(menu, GetMenuItemCount(menu), TRUE, &item);
    };
    appendOwnerDrawItem(kTrayOpen, L"Open wallpaper picker");
    appendOwnerDrawItem(kTrayPrevious, L"Previous wallpaper");
    appendOwnerDrawItem(kTrayNext, L"Next wallpaper");
    appendOwnerDrawItem(kTraySeparator, nullptr);
    appendOwnerDrawItem(kTrayExit, L"Close switcher");

    MENUINFO menuInfo{};
    menuInfo.cbSize = sizeof(menuInfo);
    menuInfo.fMask = MIM_BACKGROUND;
    menuInfo.hbrBack = g_backgroundBrush;
    SetMenuInfo(menu, &menuInfo);

    // Use the screen point supplied with the tray callback itself. The hidden
    // icons flyout may be repositioned by shell mods, so Shell_NotifyIconGetRect
    // is not reliable for locating an icon currently shown in that flyout.
    const UINT flags = TPM_RIGHTBUTTON | TPM_LEFTALIGN | TPM_TOPALIGN |
                       TPM_WORKAREA;

    SetForegroundWindow(hwnd);
    TrackPopupMenuEx(menu, flags, callbackPoint.x, callbackPoint.y,
                     hwnd, nullptr);
    PostMessageW(hwnd, WM_NULL, 0, 0);
    DestroyMenu(menu);
}

LRESULT CALLBACK PickerProc(HWND hwnd, UINT message, WPARAM wParam,
                             LPARAM lParam) {
    switch (message) {
        case WM_CREATE: {
            Gdiplus::GdiplusStartupInput startupInput;
            if (Gdiplus::GdiplusStartup(&g_gdiplusToken, &startupInput,
                                        nullptr) != Gdiplus::Ok) {
                g_gdiplusToken = 0;
            }
            g_pickerFont = CreatePickerFont(hwnd);
            UINT dpi = GetDpiForWindow(hwnd);
            if (!dpi) dpi = 96;
            g_loadingFont = CreateFontW(
                -MulDiv(18, (int)dpi, 72), 0, 0, 0, FW_NORMAL,
                FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                ANTIALIASED_QUALITY, VARIABLE_PITCH | FF_DONTCARE,
                L"Segoe Fluent Icons");
            g_breadcrumbIconFont = CreateFontW(
                -MulDiv(9, (int)dpi, 72), 0, 0, 0, FW_NORMAL,
                FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                ANTIALIASED_QUALITY, VARIABLE_PITCH | FF_DONTCARE,
                L"Segoe Fluent Icons");
            g_folderLabel = CreateWindowExW(0, L"STATIC", L"",
                WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS |
                    SS_OWNERDRAW | SS_NOTIFY,
                16, 13, 528, 33, hwnd,
                (HMENU)(INT_PTR)kFolderLabelId, nullptr, nullptr);
            if (g_folderLabel && g_pickerFont) {
                SendMessageW(g_folderLabel, WM_SETFONT,
                             (WPARAM)g_pickerFont, TRUE);
            }
            if (g_folderLabel) {
                SetWindowSubclass(g_folderLabel, BreadcrumbSubclassProc, 3,
                                  (DWORD_PTR)hwnd);
            }
            g_searchEdit = CreateWindowExW(
                0, L"EDIT", L"",
                WS_CHILD | WS_TABSTOP | WS_CLIPSIBLINGS |
                    ES_AUTOHSCROLL | ES_LEFT |
                    ES_NOHIDESEL,
                16 + 528 - kBreadcrumbSearchButtonWidth -
                    kBreadcrumbSearchEditWidth + kBreadcrumbSearchEditInset,
                20, kBreadcrumbSearchEditWidth - kBreadcrumbSearchEditInset - 5,
                24, hwnd,
                (HMENU)(INT_PTR)kSearchEditId, GetModuleHandleW(nullptr),
                nullptr);
            if (g_searchEdit) {
                if (g_pickerFont) {
                    SendMessageW(g_searchEdit, WM_SETFONT,
                                 (WPARAM)g_pickerFont, TRUE);
                }
                SendMessageW(g_searchEdit, EM_SETCUEBANNER, TRUE,
                             (LPARAM)L"Search wallpapers");
                SendMessageW(g_searchEdit, EM_SETMARGINS,
                             EC_LEFTMARGIN | EC_RIGHTMARGIN,
                             MAKELONG(kSearchTextPadding,
                                      kSearchTextPadding));
                SetWindowTheme(g_searchEdit, L"", L"");
                SetWindowSubclass(g_searchEdit, SearchEditSubclassProc, 4,
                                  (DWORD_PTR)hwnd);
                ShowWindow(g_searchEdit, SW_HIDE);
            }
            g_breadcrumbSearchButtonRect = {498, 0, 528, 33};
            for (size_t slot = 0; slot < kPageSize; ++slot) {
                const int x = 14 + (int)(slot % kGridColumns) * 132;
                const int y = 59 + (int)(slot / kGridColumns) * 82;
                g_tiles[slot] = CreateWindowExW(0, L"BUTTON", L"",
                    WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x, y, kThumbnailWidth,
                    kThumbnailHeight, hwnd,
                    (HMENU)(INT_PTR)(kFirstTileId + slot),
                    GetModuleHandleW(nullptr), nullptr);
                if (g_tiles[slot]) {
                    SetWindowSubclass(g_tiles[slot], TileSubclassProc, 1,
                                      (DWORD_PTR)hwnd);
                }
            }
            g_pageLabel = CreateWindowExW(0, L"STATIC", L"1 / 1",
                WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, 16, 323, 528, 24,
                hwnd, (HMENU)(INT_PTR)kPageLabelId, nullptr, nullptr);
            if (g_pageLabel && g_pickerFont) {
                SendMessageW(g_pageLabel, WM_SETFONT,
                             (WPARAM)g_pickerFont, TRUE);
            }
            UpdateFolderLabel();
            if (!g_folder.empty()) {
                StartImageScan();
            } else {
                RefreshList();
            }
            return 0;
        }
        case WM_COMMAND:
            if (LOWORD(wParam) == kSearchEditId &&
                HIWORD(wParam) == EN_CHANGE) {
                UpdateSearchFromEdit();
                return 0;
            }
            switch (LOWORD(wParam)) {
                case kChooseFolder: {
                    if (g_transitionActive) {
                        return 0;
                    }
                    std::wstring folder;
                    if (PickFolder(hwnd, &folder)) {
                        g_folder = std::move(folder);
                        g_page = 0;
                        g_currentImage = -1;
                        PersistFolder();
                        StartImageScan();
                    }
                    return 0;
                }
            }
            if (LOWORD(wParam) >= kFirstTileId &&
                LOWORD(wParam) < kFirstTileId + kPageSize) {
                const size_t slot = LOWORD(wParam) - kFirstTileId;
                const size_t image = ImageIndexForPageSlot(g_page, slot);
                if (image < g_images.size()) {
                    SetFocus(g_tiles[slot]);
                    ApplyImageIndex((int)image);
                }
                return 0;
            }
            break;
        case WM_MEASUREITEM:
            if (MeasureBreadcrumbMenuItem(
                    hwnd, (MEASUREITEMSTRUCT*)lParam)) {
                return TRUE;
            }
            break;
        case WM_DRAWITEM: {
            const DRAWITEMSTRUCT* item = (const DRAWITEMSTRUCT*)lParam;
            if (item && item->CtlType == ODT_MENU) {
                return DrawBreadcrumbMenuItem(item) ? TRUE : FALSE;
            }
            if (DrawPageLabel(item)) {
                return TRUE;
            }
            if (DrawBreadcrumb(item)) {
                return TRUE;
            }
            if (DrawTileButton(item)) {
                return TRUE;
            }
            break;
        }
        case WM_CTLCOLORSTATIC:
            SetTextColor((HDC)wParam,
                         GetDlgCtrlID((HWND)lParam) == kFolderLabelId
                             ? g_text
                             : g_muted);
            SetBkMode((HDC)wParam, TRANSPARENT);
            return (LRESULT)g_backgroundBrush;
        case WM_CTLCOLOREDIT:
            SetTextColor((HDC)wParam, g_text);
            SetBkColor((HDC)wParam, g_surface);
            return (LRESULT)g_searchBrush;
        case WM_ERASEBKGND: {
            RECT client{};
            GetClientRect(hwnd, &client);
            FillRect((HDC)wParam, &client, g_backgroundBrush);
                        return 1;
        }
        case WM_WINDOWPOSCHANGED: {
            const LRESULT result =
                DefWindowProcW(hwnd, message, wParam, lParam);
            PositionPickerBackdrop();
            return result;
        }
        case WM_SHOWWINDOW:
            if (wParam) {
                PositionPickerBackdrop();
            } else {
                HidePickerBackdrop();
            }
            return 0;
        case WM_THEMECHANGED:
        case WM_SYSCOLORCHANGE:
            if (g_configuredTheme == L"system" || g_automaticTheme) {
                RefreshPickerAppearance(hwnd);
            }
            return 0;
        case WM_SETTINGCHANGE:
            if (g_configuredTheme == L"system" || g_automaticTheme) {
                RefreshPickerAppearance(hwnd);
            }
            break;
        case WM_TIMER:
            if (wParam == kSearchFilterTimer) {
                ApplyPendingSearch();
                return 0;
            }
            if (wParam == kBreadcrumbChevronAnimationTimer) {
                constexpr float animationStep = 12.0f;
                if (g_breadcrumbChevronAngle <
                    g_breadcrumbChevronTargetAngle) {
                    g_breadcrumbChevronAngle = std::min(
                        g_breadcrumbChevronAngle + animationStep,
                        g_breadcrumbChevronTargetAngle);
                } else if (g_breadcrumbChevronAngle >
                           g_breadcrumbChevronTargetAngle) {
                    g_breadcrumbChevronAngle = std::max(
                        g_breadcrumbChevronAngle - animationStep,
                        g_breadcrumbChevronTargetAngle);
                }
                if (g_folderLabel) {
                    InvalidateBreadcrumbSegment(
                        g_folderLabel, g_animatedBreadcrumbMenuSegment);
                    UpdateWindow(g_folderLabel);
                }
                if (g_breadcrumbChevronAngle ==
                    g_breadcrumbChevronTargetAngle) {
                    if (g_breadcrumbChevronTargetAngle == 0.0f &&
                        g_openBreadcrumbMenuSegment < 0) {
                        g_animatedBreadcrumbMenuSegment = -1;
                    }
                    KillTimer(hwnd, kBreadcrumbChevronAnimationTimer);
                }
                return 0;
            }
            if (wParam == kBreadcrumbMenuDismissTimer &&
                g_breadcrumbMenuDismissActive) {
                HWND menuWindow = FindWindowW(L"#32768", nullptr);
                RECT menuRect{};
                POINT cursor{};

                if (menuWindow && GetWindowRect(menuWindow, &menuRect) &&
                    GetCursorPos(&cursor)) {
                    InflateRect(&menuRect, 6, 6);

                    RECT triggerRect{
                        g_breadcrumbMenuDismissAnchor.x - 16,
                        g_breadcrumbMenuDismissAnchor.y - 16,
                        g_breadcrumbMenuDismissAnchor.x + 16,
                        g_breadcrumbMenuDismissAnchor.y + 16
                    };

                    const bool cursorInMenu = PtInRect(&menuRect, cursor) != 0;
                    const bool cursorAtTrigger =
                        PtInRect(&triggerRect, cursor) != 0;
                    if (cursorInMenu) {
                        g_breadcrumbMenuCursorEntered = true;
                        g_breadcrumbMenuOutsideSince = 0;
                    } else if (cursorAtTrigger) {
                        // Wait for the click's mouse-up hook to toggle closed.
                        // Closing on mouse-down lets that click's release be
                        // delivered to the breadcrumb as a second open click.
                        g_breadcrumbMenuOutsideSince = 0;
                    } else {
                        const ULONGLONG now = GetTickCount64();
                        if (!g_breadcrumbMenuOutsideSince) {
                            g_breadcrumbMenuOutsideSince = now;
                        } else if (now - g_breadcrumbMenuOutsideSince >= 180) {
                            EndMenu();
                        }
                    }
                }
                return 0;
            }
            break;
        case WM_MOUSEWHEEL:
            StepPage(GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? -1 : 1);
            return 0;
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                PostMessageW(hwnd, WM_CLOSE, 0, 0);
                return 0;
            }
            if (HandlePickerKey((UINT)wParam)) {
                return 0;
            }
            break;
        case WM_KEYUP:
            if (wParam == VK_SPACE || wParam == VK_RETURN) {
                return 0;
            }
            break;
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE) {
                HWND activated = (HWND)lParam;
                if (!activated ||
                    GetAncestor(activated, GA_ROOTOWNER) != hwnd) {
                    PostMessageW(hwnd, WM_CLOSE, 0, 0);
                }
            }
            return 0;
        case WM_CLOSE:
            // TrackPopupMenuEx runs its own loop; end it before hiding the
            // picker so its native popup cannot remain stranded onscreen.
            if (g_breadcrumbMenuDismissActive) {
                g_pickerCloseAfterBreadcrumbMenu = true;
                EndMenu();
                return 0;
            }
            HidePickerWindow(hwnd);
            return 0;
        case WM_DESTROY:
            if (g_breadcrumbMenuDismissActive) {
                EndMenu();
            }
            if (g_pickerBackdropWindow) {
                DestroyWindow(g_pickerBackdropWindow);
                g_pickerBackdropWindow = nullptr;
            }
            KillTimer(hwnd, kBreadcrumbMenuDismissTimer);
            KillTimer(hwnd, kBreadcrumbChevronAnimationTimer);
            KillTimer(hwnd, kSearchFilterTimer);
            for (HWND tile : g_tiles) {
                if (tile) {
                    RemoveWindowSubclass(tile, TileSubclassProc, 1);
                }
            }
            if (g_folderLabel) {
                RemoveWindowSubclass(g_folderLabel, BreadcrumbSubclassProc, 3);
                g_folderLabel = nullptr;
            }
            if (g_searchEdit) {
                RemoveWindowSubclass(g_searchEdit, SearchEditSubclassProc, 4);
                g_searchEdit = nullptr;
            }
            if (g_breadcrumbMenuMouseHook) {
                UnhookWindowsHookEx(g_breadcrumbMenuMouseHook);
                g_breadcrumbMenuMouseHook = nullptr;
            }
            ClearThumbnailCache();
            if (g_pickerFont) {
                DeleteObject(g_pickerFont);
                g_pickerFont = nullptr;
            }
            if (g_loadingFont) {
                DeleteObject(g_loadingFont);
                g_loadingFont = nullptr;
            }
            if (g_breadcrumbIconFont) {
                DeleteObject(g_breadcrumbIconFont);
                g_breadcrumbIconFont = nullptr;
            }
            if (g_gdiplusToken) {
                Gdiplus::GdiplusShutdown(g_gdiplusToken);
                g_gdiplusToken = 0;
            }
            g_pickerWindow = nullptr;
            return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

void StopBackgroundPosting() {
    AcquireSRWLockExclusive(&g_backgroundPostLock);
    g_shuttingDown.store(true);
    ReleaseSRWLockExclusive(&g_backgroundPostLock);
}

void FreeQueuedBackgroundResults(HWND window) {
    MSG queued{};
    while (PeekMessageW(&queued, window, kImageListReady,
                        kThumbnailPageReady, PM_REMOVE)) {
        if (queued.message == kImageListReady) {
            delete (ImageScanResult*)queued.lParam;
        } else if (queued.message == kThumbnailPageReady) {
            FreeThumbnailPageResult((ThumbnailPageResult*)queued.lParam);
        }
    }
}

LRESULT CALLBACK HostProc(HWND hwnd, UINT message, WPARAM wParam,
                          LPARAM lParam) {
    switch (message) {
        case kImageListReady: {
            std::unique_ptr<ImageScanResult> result(
                (ImageScanResult*)lParam);
            if (result->generation == g_folderGeneration.load()) {
                g_imageScanInProgress = false;
                g_folderUnavailable = result->folderUnavailable;
                g_images = std::move(result->images);
                g_page = 0;
                g_selectedImage = g_images.empty() ? -1 : 0;
                g_currentImage = -1;
                if (!g_lastSelectedPath.empty()) {
                    for (size_t i = 0; i < g_images.size(); ++i) {
                        if (CompareStringOrdinal(g_images[i].c_str(), -1,
                                                 g_lastSelectedPath.c_str(), -1,
                                                 TRUE) == CSTR_EQUAL) {
                            g_selectedImage = (int)i;
                            g_currentImage = (int)i;
                            g_page = PageForImageIndex(i);
                            break;
                        }
                    }
                }
                RebuildSearchResults(false);
            }
            return 0;
        }
        case kThumbnailPageReady: {
            std::unique_ptr<ThumbnailPageResult> result(
                (ThumbnailPageResult*)lParam);
            g_pendingThumbnailPages.erase(
                std::remove_if(
                    g_pendingThumbnailPages.begin(),
                    g_pendingThumbnailPages.end(),
                    [&](const PendingThumbnailPage& pending) {
                        return pending.generation == result->generation &&
                               pending.page == result->page;
                    }),
                g_pendingThumbnailPages.end());

            const std::vector<size_t> desired = DesiredThumbnailPages();
            if (result->generation == g_folderGeneration.load() &&
                IsPageInList(desired, result->page)) {
                for (auto it = g_thumbnailCache.begin();
                     it != g_thumbnailCache.end(); ++it) {
                    if (it->page == result->page) {
                        DestroyCachedPage(*it);
                        g_thumbnailCache.erase(it);
                        break;
                    }
                }
                g_thumbnailCache.push_back(
                    {result->page, std::move(result->bitmaps)});
                RefreshList();
            } else {
                FreeThumbnailPageResult(result.release());
                ScheduleThumbnailWindow();
            }
            return 0;
        }
        case kWallpaperChangeDone:
            g_transitionActive = false;
            if (lParam && !g_transitionCurrentSuperseded &&
                (int)wParam >= 0 && (size_t)wParam < g_images.size() &&
                g_images[(size_t)wParam] == g_transitionPath) {
                g_currentImage = (int)wParam;
                for (HWND tile : g_tiles) {
                    if (tile) {
                        InvalidateRect(tile, nullptr, FALSE);
                    }
                }
            }
            g_transitionCurrentSuperseded = false;
            {
                const auto latest = ReadWallpaperRequest();
                const bool superseded = !latest.path.empty() &&
                    (int)wParam >= 0 && (size_t)wParam < g_images.size() &&
                    latest.path != g_images[(size_t)wParam];
                const int latestIndex = latest.imageIndex;
                g_transitionPath.clear();
                if (superseded && latestIndex >= 0 &&
                    latestIndex < (int)g_images.size()) {
                    StartWallpaperChange(latestIndex);
                }
            }
            return 0;
        case kReloadSettings:
            RefreshPickerAppearance(g_pickerWindow);
            if (g_pickerWindow && IsWindowVisible(g_pickerWindow)) {
                PositionPickerWindow(nullptr);
            }
            if (!RegisterPickerHotkey(hwnd)) {
                Wh_Log(L"RegisterHotKey failed after settings change");
            }
            StartImageScan();
            return 0;
        case WM_HOTKEY:
            if (wParam == kHotkeyId) {
                TogglePicker();
            }
            return 0;
        case kTrayCallback:
            if (LOWORD(lParam) == WM_LBUTTONUP ||
                LOWORD(lParam) == WM_LBUTTONDBLCLK) {
                TogglePicker();
            } else if (LOWORD(lParam) == WM_RBUTTONUP) {
                // With NOTIFYICON_VERSION_4 the callback's wParam contains
                // the screen coordinates at which the icon was right-clicked.
                const POINT clickPoint{
                    (SHORT)LOWORD(wParam), (SHORT)HIWORD(wParam)};
                ShowTrayMenu(hwnd, clickPoint);
            }
            return 0;
        case WM_MEASUREITEM:
            if (MeasureBreadcrumbMenuItem(hwnd,
                                          (MEASUREITEMSTRUCT*)lParam)) {
                return TRUE;
            }
            break;
        case WM_DRAWITEM: {
            const DRAWITEMSTRUCT* item = (const DRAWITEMSTRUCT*)lParam;
            if (item && item->CtlType == ODT_MENU &&
                DrawBreadcrumbMenuItem(item)) {
                return TRUE;
            }
            break;
        }
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case kTrayOpen:
                    ShowPicker();
                    return 0;
                case kTrayPrevious:
                    StepImage(-1);
                    return 0;
                case kTrayNext:
                    StepImage(1);
                    return 0;
                case kTrayExit:
                    DestroyWindow(hwnd);
                    return 0;
            }
            break;
        case WM_DESTROY:
            StopBackgroundPosting();
            UnregisterHotKey(hwnd, kHotkeyId);
            Shell_NotifyIconW(NIM_DELETE, &g_notifyIcon);
            if (g_trayIcon) {
                DestroyIcon(g_trayIcon);
                g_trayIcon = nullptr;
            }
            if (g_pickerWindow) {
                DestroyWindow(g_pickerWindow);
            }
            FreeQueuedBackgroundResults(hwnd);
            g_hostWindow = nullptr;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

DWORD WINAPI UiThreadProc(void*) {
    INITCOMMONCONTROLSEX controls{sizeof(controls), ICC_LISTVIEW_CLASSES};
    InitCommonControlsEx(&controls);
    ApplyPickerThemeColors();
    g_backgroundBrush = CreateSolidBrush(g_background);
    g_searchBrush = CreateSolidBrush(g_surface);
    WNDCLASSEXW hostClass{sizeof(hostClass)};
    hostClass.lpfnWndProc = HostProc;
    hostClass.hInstance = GetModuleHandleW(nullptr);
    hostClass.lpszClassName = L"SimpleWallpaperSwitcherHost";
    RegisterClassExW(&hostClass);
    WNDCLASSEXW pickerClass{sizeof(pickerClass)};
    pickerClass.lpfnWndProc = PickerProc;
    pickerClass.style = CS_DROPSHADOW;
    pickerClass.hInstance = GetModuleHandleW(nullptr);
    pickerClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    pickerClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    pickerClass.lpszClassName = kWindowClass;
    RegisterClassExW(&pickerClass);

    WNDCLASSEXW backdropClass{sizeof(backdropClass)};
    backdropClass.lpfnWndProc = PickerBackdropProc;
    backdropClass.hInstance = GetModuleHandleW(nullptr);
    backdropClass.lpszClassName = kBackdropWindowClass;
    RegisterClassExW(&backdropClass);

    g_hostWindow = CreateWindowExW(0, hostClass.lpszClassName, L"", 0,
        0, 0, 0, 0, HWND_MESSAGE, nullptr, hostClass.hInstance, nullptr);
    if (g_hostWindow) {
        g_pickerWindow = CreateWindowExW(WS_EX_TOOLWINDOW,
            kWindowClass, L"Simple Wallpaper Switcher",
            WS_POPUP | WS_CLIPCHILDREN,
            CW_USEDEFAULT, CW_USEDEFAULT, 560, 360, nullptr, nullptr,
            pickerClass.hInstance, nullptr);
        if (g_pickerWindow) {
            g_pickerBackdropWindow = CreateWindowExW(
                WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
                kBackdropWindowClass, L"", WS_POPUP,
                CW_USEDEFAULT, CW_USEDEFAULT, 1, 1, nullptr, nullptr,
                pickerClass.hInstance, nullptr);
            if (g_pickerBackdropWindow) {
                const COLORREF noBorder = 0xfffffffeu;
                DwmSetWindowAttribute(
                    g_pickerBackdropWindow,
                    static_cast<DWMWINDOWATTRIBUTE>(34),
                    &noBorder, sizeof(noBorder));
            }
            StylePickerWindow(g_pickerWindow);
            ApplyPickerMaterial(g_pickerWindow);
        }
        AddTrayIcon(g_hostWindow);
        if (!RegisterPickerHotkey(g_hostWindow)) {
            Wh_Log(L"RegisterHotKey failed; tray icon remains available");
        }
    }
    SetEvent(g_readyEvent);

    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    if (g_hostWindow) {
        DestroyWindow(g_hostWindow);
        g_hostWindow = nullptr;
    }
    WaitForBackgroundThreads();
    if (g_backgroundBrush) {
        DeleteObject(g_backgroundBrush);
        g_backgroundBrush = nullptr;
    }
    if (g_searchBrush) {
        DeleteObject(g_searchBrush);
        g_searchBrush = nullptr;
    }
    return 0;
}

}

void WhTool_ModUninit();

BOOL WhTool_ModInit() {
    g_shuttingDown.store(false);
    LoadSettings();
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        return FALSE;
    }
    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0,
                              &g_uiThreadId);
    if (!g_uiThread) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }
    if (WaitForSingleObject(g_readyEvent, 5000) != WAIT_OBJECT_0 ||
        !g_hostWindow || !g_pickerWindow) {
        WhTool_ModUninit();
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    if (g_hostWindow) {
        PostMessageW(g_hostWindow, kReloadSettings, 0, 0);
    } else {
    }
}

void WhTool_ModUninit() {
    StopBackgroundPosting();
    if (g_hostWindow) {
        PostMessageW(g_hostWindow, WM_CLOSE, 0, 0);
    }
    if (g_uiThreadId) {
        PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
    }
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, INFINITE);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
    if (g_wallpaperThread) {
        WaitForSingleObject(g_wallpaperThread, INFINITE);
        CloseHandle(g_wallpaperThread);
        g_wallpaperThread = nullptr;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
}

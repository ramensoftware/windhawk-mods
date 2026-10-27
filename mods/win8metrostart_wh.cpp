// ==WindhawkMod==
 // @id              win8metrostart
// @name            Windows 8 Metro Start Screen
// @description     A native fullscreen Windows 8.1 Metro Start Screen for Windows 11 with Start button hook, anti-aliased icons, smooth staggered animations, blurred backgrounds, and clean UI.
// @version         2.2.0
// @author          Mostafa Ahmed Elshorbagy
// @github          https://github.com/elahorbagy
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lgdi32 -lmsimg32 -lshell32 -lole32 -luuid -ladvapi32 -lpowrprof -lshcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 8 Metro Start Screen (v2.2)

A native fullscreen Windows 8.1 Metro Start Screen for Windows 11, built with pure C++ and Win32/Shell APIs for high responsiveness, minimal memory usage, and authentic Metro design.

## What's New in v2.2
- **Super-Sharp Anti-Aliased Icons**:
  - Replaced nearest-neighbor `DrawIconEx` scaling with a 32-bit premultiplied ARGB icon pipeline (`IShellItemImageFactory` + auto-trimmed transparent margins + area-averaged bilinear downsampling + `GdiAlphaBlend`), eliminating jagged or pixelated icons across all tile sizes.
- **Smooth Multi-Stage Opening Animation**:
  - Combines full-window alpha fade-in (`SetLayeredWindowAttributes`) with a staggered per-group horizontal swoosh and subtle tile scale-in so the Start Screen glides in smoothly instead of popping in abruptly.
  - Configurable **Animation Style** (`Metro Staggered Swoosh`, `Smooth Glide & Fade`, `Zoom & Fade`) and **Opening Duration (ms)**.
- **Native Start Button & Windows Key Support**:
  - Hooks `XamlLauncher::ShowStartView` in `twinui.pcshell.dll` so clicking the Windows 11 taskbar **Start button** or pressing the **Windows key** opens the Metro Start Screen directly (both toggleable in Settings alongside the `Ctrl + Alt + S` shortcut).
- **Clean, Minimal Windows 8.1 Layout**:
  - Removed cluttered helper text at the top and bottom of the screen and removed `" v"` suffixes and underscores from group headers.
  - Fixed the top-right **User Profile Menu** Z-order so it always renders cleanly above all tiles and headers.
  - Added a circular Windows 8.1-style **All Apps** arrow button at the bottom-left and a sleek dark/auto-hiding search bar.
- **Expanded User Customization Options**:
  - Adjustable **Background Blur** for Desktop Wallpaper and Custom Image modes, **Tile Size Scale (%)**, **Tile Spacing (px)**, **Show/Hide Tile Labels**, **Show/Hide Group Headers**, and **Search Bar Style** (`Sleek Dark`, `Auto-Hide until typing`, or `Classic White`).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- openWithStartButton: true
  $name: Open with Taskbar Start Button
  $description: Opens the Metro Start Screen when clicking the Windows taskbar Start button
- openWithWinKey: true
  $name: Open with Windows Key
  $description: Opens the Metro Start Screen when pressing the Windows key or Ctrl+Esc
- themeScheme: wallpaper
  $name: Background Mode / Theme
  $options:
  - wallpaper: Current Windows Desktop Wallpaper
  - dark: Windows 8.1 Charcoal Dark (#1D1D1D)
  - emerald: Windows 8 Emerald Green (#004D26)
  - indigo: Windows 8 Deep Indigo (#180052)
  - teal: Windows 8 Slate Teal (#004050)
  - crimson: Metro Burgundy Crimson (#3B0918)
  - cobalt: Metro Cobalt Night (#0B2545)
  - custom_image: Custom Background Image File (path below)
  - custom_color: Custom Solid Hex Color (color below)
- backgroundBlur: false
  $name: Blur Wallpaper / Custom Image Background
  $description: Applies a smooth Gaussian-like blur to wallpaper or custom image backgrounds
- blurStrength: 45
  $name: Background Blur Strength (10 to 100)
  $description: Strength of the blur effect when Blur Wallpaper/Image is enabled
- bgOverlayOpacity: 35
  $name: Wallpaper / Image Dark Tint Opacity (0 to 90%)
  $description: Darkens wallpaper or custom image backgrounds so white text and tiles stand out
- customBgColor: "#18181B"
  $name: Custom Background Color (Hex)
  $description: Used when Background Mode is set to Custom Solid Hex Color (e.g. #18181B or #0F172A)
- customBgImagePath: ""
  $name: Custom Background Image Path
  $description: Full path to a .jpg, .png, or .bmp image when Background Mode is set to Custom Background Image File
- showBackgroundMotif: false
  $name: Show Geometric Background Motif on Solid Colors
  $description: Draws subtle concentric rings and accent bands on solid color themes
- enableAnimations: true
  $name: Enable Smooth Animations
  $description: Enables smooth staggered entrance fade/glide and smooth wheel scrolling
- animationStyle: metro_swoosh
  $name: Entrance Animation Style
  $options:
  - metro_swoosh: Metro Staggered Group Swoosh + Fade + Scale (Authentic Windows 8.1)
  - glide_fade: Smooth Horizontal Glide + Fade
  - zoom_fade: Subtle Zoom In + Fade
- openAnimationDuration: 360
  $name: Opening Animation Duration (150 to 800 ms)
  $description: Duration of the entrance animation in milliseconds (higher = slower and smoother)
- animationSpeed: 100
  $name: Scroll Animation Speed (50% to 200%)
  $description: Controls how fast horizontal wheel scrolling interpolates
- searchBarStyle: sleek_dark
  $name: Search Bar Style
  $options:
  - sleek_dark: Sleek Translucent Dark Pill
  - auto_hide: Auto-Hide (Appears only when typing or clicking the search icon)
  - classic_light: Classic White Box
- tileScalePercent: 100
  $name: Tile & Icon Scale (75% to 135%)
  $description: Scales tile dimensions and crisp anti-aliased icons up or down
- tileGapPx: 8
  $name: Tile Grid Spacing (4 to 16 px)
  $description: Gap in pixels between adjacent tiles
- showTileLabels: true
  $name: Show App Names on Tiles
  $description: Turn off for a minimal icon-only tile look
- showGroupHeaders: true
  $name: Show Group Header Titles
  $description: Displays clean group names above tile groups
- showBuiltinTiles: true
  $name: Show Built-in Desktop & Live Clock Tiles
  $description: Adds Desktop and real-time Clock/Calendar tiles to the first group
- defaultTileRows: 0
  $name: Tile Grid Row Units (0 = Automatic based on screen height)
  $description: Number of medium-tile rows (2 to 6). 0 adapts automatically to monitor resolution
- fontFamily: segoe_variable
  $name: Font Family (ClearType Natural Rendering)
  $options:
  - segoe_variable: Segoe UI Variable Display (Sharpest on Windows 11)
  - segoe_semibold: Segoe UI Semibold / Regular (High Contrast & Bold)
  - segoe_classic: Classic Segoe UI (Windows 8 Default)
  - bahnschrift: Bahnschrift (Modern Geometric Sans)
  - calibri: Calibri (Clean & Rounded)
- hotkeyModifiers: ctrl_alt
  $name: Backup Toggle Hotkey Modifiers
  $options:
  - ctrl_alt: Ctrl + Alt
  - alt_shift: Alt + Shift
  - ctrl_shift: Ctrl + Shift
  - win_shift: Win + Shift
- hotkeyKey: "S"
  $name: Backup Toggle Hotkey Key
  $description: Single letter (A-Z) or F1-F12 (default S for Ctrl+Alt+S)
- enterInSearchLaunchesFirst: true
  $name: Launch First Result on Enter While Searching
  $description: Pressing Enter while searching immediately opens the top matching app
- autoRefreshMinutes: 30
  $name: Background App List Refresh Interval (minutes)
  $description: 0 disables automatic refresh; otherwise refreshes cached app list periodically
*/
// ==/WindhawkModSettings==

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#ifndef WINVER
#define WINVER 0x0A00
#endif

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shellscalingapi.h>
#include <knownfolders.h>
#include <powrprof.h>

#ifdef __has_include
#if __has_include(<windhawk_utils.h>)
#include <windhawk_utils.h>
#define HAS_WINDHAWK_UTILS 1
#endif
#endif

#ifndef CLEARTYPE_NATURAL_QUALITY
#define CLEARTYPE_NATURAL_QUALITY 6
#endif
#ifndef SPI_GETDESKWALLPAPER
#define SPI_GETDESKWALLPAPER 0x0073
#endif
#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED 0x00080000L
#endif
#ifndef LWA_ALPHA
#define LWA_ALPHA 0x00000002
#endif
#ifndef AC_SRC_OVER
#define AC_SRC_OVER 0x00
#endif
#ifndef AC_SRC_ALPHA
#define AC_SRC_ALPHA 0x01
#endif

#include <algorithm>
#include <atomic>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <cwchar>
#include <cwctype>
#include <fstream>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

constexpr int kHotkeyId = 0x4D53;
constexpr UINT WM_APP_TOGGLE       = WM_APP + 10;
constexpr UINT WM_APP_RELOAD_CFG   = WM_APP + 11;
constexpr UINT WM_APP_APPS_UPDATED = WM_APP + 12;

constexpr UINT_PTR kTimerClock       = 1;
constexpr UINT_PTR kTimerAutoRefresh = 2;
constexpr UINT_PTR kTimerAnimation   = 3;

constexpr int kPaletteSize = 12;
const COLORREF kPalette[kPaletteSize] = {
    RGB(0, 120, 215),   // 0: Cobalt Blue
    RGB(16, 124, 16),   // 1: Emerald Green
    RGB(185, 29, 71),   // 2: Crimson Red
    RGB(104, 33, 122),  // 3: Metro Purple
    RGB(0, 130, 153),   // 4: Metro Teal
    RGB(210, 71, 38),   // 5: Metro Orange
    RGB(81, 51, 171),   // 6: Deep Indigo
    RGB(0, 153, 51),    // 7: Vibrant Green
    RGB(172, 25, 61),   // 8: Ruby Magenta
    RGB(202, 80, 16),   // 9: Amber Spice
    RGB(0, 164, 239),   // 10: Sky Cyan
    RGB(45, 85, 155)    // 11: Steel Blue
};

const wchar_t* const kPaletteNames[kPaletteSize] = {
    L"Cobalt Blue",
    L"Emerald Green",
    L"Crimson Red",
    L"Metro Purple",
    L"Metro Teal",
    L"Metro Orange",
    L"Deep Indigo",
    L"Vibrant Green",
    L"Ruby Magenta",
    L"Amber Spice",
    L"Sky Cyan",
    L"Steel Blue"
};

enum class TileSize : int {
    Small  = 0, // 1x1 sub-cells
    Medium = 1, // 2x2 sub-cells
    Wide   = 2, // 4x2 sub-cells
    Large  = 3  // 4x4 sub-cells
};

enum class BuiltinKind : int {
    None    = 0,
    Desktop = 1,
    Clock   = 2
};

enum class ThemeScheme : int {
    Emerald     = 0,
    Dark        = 1,
    Indigo      = 2,
    Teal        = 3,
    Crimson     = 4,
    Cobalt      = 5,
    Wallpaper   = 6,
    CustomImage = 7,
    CustomColor = 8
};

enum class FontChoice : int {
    SegoeVariable = 0,
    SegoeSemibold = 1,
    SegoeClassic  = 2,
    Bahnschrift   = 3,
    Calibri       = 4
};

enum class AnimStyle : int {
    MetroSwoosh = 0,
    GlideFade   = 1,
    ZoomFade    = 2
};

enum class SearchBarStyle : int {
    SleekDark    = 0,
    AutoHide     = 1,
    ClassicLight = 2
};

struct ModSettings {
    bool openWithStartButton = true;
    bool openWithWinKey = true;
    ThemeScheme theme = ThemeScheme::Wallpaper;
    bool backgroundBlur = false;
    int blurStrength = 45;
    COLORREF customBgColor = RGB(24, 24, 27);
    std::wstring customBgImagePath;
    int bgOverlayOpacity = 35;
    bool showMotif = false;
    bool enableAnimations = true;
    AnimStyle animStyle = AnimStyle::MetroSwoosh;
    int openAnimDurationMs = 360;
    int animationSpeed = 100;
    SearchBarStyle searchBarStyle = SearchBarStyle::SleekDark;
    int tileScalePercent = 100;
    int tileGapPx = 8;
    bool showTileLabels = true;
    bool showGroupHeaders = true;
    FontChoice fontChoice = FontChoice::SegoeVariable;
    bool showBuiltinTiles = true;
    int defaultTileRows = 0;
    UINT hotkeyModifiers = MOD_CONTROL | MOD_ALT | MOD_NOREPEAT;
    UINT hotkeyVk = 'S';
    bool enterInSearchLaunchesFirst = true;
    int autoRefreshMinutes = 30;
};

struct TileCustomization {
    TileSize size = TileSize::Medium;
    int colorIndex = -1;
    std::wstring group;
    int order = 0;
    bool pinned = true;
    bool hasCustomSize = false;
};

struct AppEntry {
    std::wstring id;
    std::wstring title;
    std::wstring searchTitle;
    std::wstring launchTarget;
    std::wstring fileLocation;
    std::wstring defaultGroup;
    std::wstring group;
    std::wstring searchGroup;
    HICON icon = nullptr;
    // High-resolution premultiplied 32-bit BGRA icon buffer + cached resampled HBITMAP
    std::vector<uint32_t> iconPixels;
    int iconPixelSide = 0;
    HBITMAP cachedBmp = nullptr;
    int cachedBmpSize = 0;

    TileSize size = TileSize::Medium;
    int colorIndex = -1;
    COLORREF color = RGB(0, 120, 215);
    int order = 0;
    bool pinned = true;
    BuiltinKind builtin = BuiltinKind::None;
    RECT rect{};
    int groupIdx = 0;
    wchar_t alphaBucket = L'#';
};

struct SectionHeader {
    std::wstring rawGroup;
    std::wstring displayTitle;
    int x = 0;
    int groupIdx = 0;
    RECT headerRect{};
};

struct AlphaHeader {
    wchar_t letter = L'#';
    RECT rect{};
    int firstVisibleIdx = 0;
};

enum ContextCmd : UINT {
    CMD_NONE = 0,
    CMD_LAUNCH = 100,
    CMD_RUN_ADMIN = 101,
    CMD_OPEN_LOCATION = 102,
    CMD_SIZE_SMALL = 110,
    CMD_SIZE_MEDIUM = 111,
    CMD_SIZE_WIDE = 112,
    CMD_SIZE_LARGE = 113,
    CMD_MOVE_EARLIER = 120,
    CMD_MOVE_LATER = 121,
    CMD_RENAME_GROUP = 122,
    CMD_NEW_GROUP = 123,
    CMD_UNPIN = 130,
    CMD_RESTORE_ALL = 131,
    CMD_RESET_LAYOUT = 132,
    CMD_COLOR_AUTO = 199,
    CMD_COLOR_BASE = 200, // 200..211
    CMD_GROUP_BASE = 300, // 300..399

    // Canvas Background & Animation Quick Menu commands
    CMD_BG_EMERALD   = 400,
    CMD_BG_DARK      = 401,
    CMD_BG_INDIGO    = 402,
    CMD_BG_TEAL      = 403,
    CMD_BG_CRIMSON   = 404,
    CMD_BG_COBALT    = 405,
    CMD_BG_WALLPAPER = 406,
    CMD_BG_CUSTOM_C  = 407,
    CMD_TOGGLE_MOTIF = 410,
    CMD_TOGGLE_ANIM  = 411,
    CMD_REFRESH_APPS = 412,
    CMD_TOGGLE_BLUR  = 413,
    CMD_TOGGLE_LABELS = 414,
    CMD_TOGGLE_HEADERS = 415
};

constexpr GUID kBhidEnumItems = {
    0x94f60519, 0x2850, 0x4924,
    {0xaa, 0x5a, 0xd1, 0x5e, 0x84, 0x86, 0x80, 0x39}
};

// Global UI state (owned by UI thread)
std::atomic<HWND> g_window{nullptr};
std::atomic<bool> g_openWithStartBtnAtomic{true};
std::atomic<bool> g_openWithWinKeyAtomic{true};

HWND g_prevForegroundWindow = nullptr;
ULONGLONG g_lastHideTick = 0;

HFONT g_fontTitle = nullptr;
HFONT g_fontSubtitle = nullptr;
HFONT g_fontGroup = nullptr;
HFONT g_fontTile = nullptr;
HFONT g_fontTileLarge = nullptr;
HFONT g_fontClockBig = nullptr;
HFONT g_fontList = nullptr;
HFONT g_fontButton = nullptr;
HICON g_fallbackIcon = nullptr;

// Cached background bitmap (for Wallpaper or Custom Image mode)
HBITMAP g_bgBitmap = nullptr;
int g_bgBitmapW = 0;
int g_bgBitmapH = 0;
std::wstring g_loadedBgKey;

HANDLE g_thread = nullptr;
HANDLE g_readyEvent = nullptr;
DWORD g_threadId = 0;

HANDLE g_discoveryThread = nullptr;
std::atomic<bool> g_stopDiscovery{false};
std::atomic<bool> g_discoveryRunning{false};

UINT g_dpi = 96;
ModSettings g_settings;
int g_runtimeThemeOverride = -1;
int g_runtimeAnimOverride = -1;
int g_runtimeMotifOverride = -1;
int g_runtimeBlurOverride = -1;
int g_runtimeLabelsOverride = -1;
int g_runtimeHeadersOverride = -1;

bool g_allAppsView = false;
bool g_alphaJumpView = false;
bool g_powerMenu = false;
bool g_profileMenu = false;
bool g_searchFocused = false;
bool g_renamingGroup = false;
std::wstring g_renameGroupTarget;
std::wstring g_renameBuffer;

std::wstring g_searchText;
int g_selectedApp = -1;
int g_hoverApp = -1;
int g_hoverHeader = -1;
int g_hoverPowerItem = -1;
int g_hoverProfileItem = -1;
bool g_hoverAllAppsBtn = false;
bool g_hoverPowerBtn = false;
bool g_hoverProfileBtn = false;
bool g_hoverSearchBtn = false;

int g_subRows = 6;
int g_scrollX = 0;
float g_scrollAnimX = 0.0f;
int g_scrollTargetX = 0;
int g_contentWidth = 0;

// Animation state
bool g_animTimerActive = false;
float g_openAnimProgress = 1.0f; // 0.0 -> 1.0 on open when animations are enabled
ULONGLONG g_lastAnimTick = 0;

// Mouse interaction & Drag-and-drop state
bool g_mouseDownOnTile = false;
int g_pressedTileIndex = -1;
POINT g_mouseDownPt{};
bool g_isDraggingTile = false;
POINT g_dragCurrentPt{};

bool g_draggingScrollbar = false;
int g_scrollbarDragOffset = 0;
bool g_trackingMouse = false;

std::vector<AppEntry> g_apps;
std::vector<int> g_visibleApps;
std::vector<SectionHeader> g_headers;
std::vector<AlphaHeader> g_alphaHeaders;
RECT g_alphaJumpRects[28]{};

std::unordered_map<std::wstring, TileCustomization> g_customTiles;
std::unordered_map<std::wstring, std::wstring> g_groupRenames;
int g_nextOrderCounter = 1;

////////////////////////////////////////////////////////////////////////////////
// Helpers

int S(int value) {
    return MulDiv(value, static_cast<int>(g_dpi ? g_dpi : 96), 96);
}

int STile(int value) {
    int scaled = MulDiv(value, g_settings.tileScalePercent, 100);
    return S(scaled);
}

std::wstring Lower(std::wstring value) {
    if (!value.empty()) {
        CharLowerBuffW(value.data(), static_cast<DWORD>(value.size()));
    }
    return value;
}

std::wstring Trim(const std::wstring& s) {
    size_t first = s.find_first_not_of(L" \t\r\n\"");
    if (first == std::wstring::npos) return L"";
    size_t last = s.find_last_not_of(L" \t\r\n\"");
    return s.substr(first, last - first + 1);
}

bool EndsWith(const std::wstring& value, const std::wstring& suffix) {
    if (value.size() < suffix.size()) return false;
    return Lower(value.substr(value.size() - suffix.size())) == Lower(suffix);
}

bool PointInRect(const RECT& rect, POINT point) {
    return point.x >= rect.left && point.x < rect.right &&
           point.y >= rect.top && point.y < rect.bottom;
}

int StableColorIndex(const std::wstring& id) {
    uint32_t hash = 2166136261u;
    for (wchar_t ch : id) {
        hash ^= static_cast<uint32_t>(std::towlower(ch));
        hash *= 16777619u;
    }
    return static_cast<int>(hash % kPaletteSize);
}

COLORREF LightenColor(COLORREF c, int delta) {
    int r = std::clamp(static_cast<int>(GetRValue(c)) + delta, 0, 255);
    int g = std::clamp(static_cast<int>(GetGValue(c)) + delta, 0, 255);
    int b = std::clamp(static_cast<int>(GetBValue(c)) + delta, 0, 255);
    return RGB(r, g, b);
}

COLORREF ParseHexColor(std::wstring text, COLORREF fallback) {
    text = Trim(text);
    if (!text.empty() && text[0] == L'#') text.erase(0, 1);
    if (text.size() != 6) return fallback;
    for (wchar_t ch : text) {
        if (!std::iswxdigit(ch)) return fallback;
    }
    unsigned long rgb = wcstoul(text.c_str(), nullptr, 16);
    int r = (rgb >> 16) & 0xFF;
    int g = (rgb >> 8) & 0xFF;
    int b = rgb & 0xFF;
    return RGB(r, g, b);
}

wchar_t GetAlphaBucket(const std::wstring& title) {
    for (wchar_t ch : title) {
        if (std::iswspace(ch)) continue;
        wchar_t up = static_cast<wchar_t>(std::towupper(ch));
        if (up >= L'A' && up <= L'Z') return up;
        if (ch >= 0x0080) return L'@';
        return L'#';
    }
    return L'#';
}

bool IsFolderWindowsProcess() {
    std::wstring cmd = Lower(GetCommandLineW());
    return cmd.find(L"/factory") != std::wstring::npos ||
           cmd.find(L"-embedding") != std::wstring::npos ||
           cmd.find(L"/separate") != std::wstring::npos;
}

bool AnimationsEnabled() {
    if (g_runtimeAnimOverride >= 0) return g_runtimeAnimOverride != 0;
    return g_settings.enableAnimations;
}

bool MotifEnabled() {
    if (g_runtimeMotifOverride >= 0) return g_runtimeMotifOverride != 0;
    return g_settings.showMotif;
}

bool BlurEnabled() {
    if (g_runtimeBlurOverride >= 0) return g_runtimeBlurOverride != 0;
    return g_settings.backgroundBlur;
}

bool LabelsEnabled() {
    if (g_runtimeLabelsOverride >= 0) return g_runtimeLabelsOverride != 0;
    return g_settings.showTileLabels;
}

bool GroupHeadersEnabled() {
    if (g_runtimeHeadersOverride >= 0) return g_runtimeHeadersOverride != 0;
    return g_settings.showGroupHeaders;
}

ThemeScheme ActiveTheme() {
    if (g_runtimeThemeOverride >= 0) return static_cast<ThemeScheme>(g_runtimeThemeOverride);
    return g_settings.theme;
}

std::wstring GetStringSettingSafe(PCWSTR name, PCWSTR fallback) {
    PCWSTR raw = Wh_GetStringSetting(name);
    if (!raw) return fallback ? fallback : L"";
    std::wstring res(raw);
    Wh_FreeStringSetting(raw);
    return res;
}

void FreeBackgroundBitmap() {
    if (g_bgBitmap) {
        DeleteObject(g_bgBitmap);
        g_bgBitmap = nullptr;
    }
    g_bgBitmapW = 0;
    g_bgBitmapH = 0;
    g_loadedBgKey.clear();
}

void LoadModSettings() {
    ModSettings s;
    s.openWithStartButton = Wh_GetIntSetting(L"openWithStartButton") != 0;
    s.openWithWinKey = Wh_GetIntSetting(L"openWithWinKey") != 0;

    std::wstring theme = Lower(Trim(GetStringSettingSafe(L"themeScheme", L"wallpaper")));
    if (theme == L"dark") s.theme = ThemeScheme::Dark;
    else if (theme == L"indigo") s.theme = ThemeScheme::Indigo;
    else if (theme == L"teal") s.theme = ThemeScheme::Teal;
    else if (theme == L"crimson") s.theme = ThemeScheme::Crimson;
    else if (theme == L"cobalt") s.theme = ThemeScheme::Cobalt;
    else if (theme == L"wallpaper") s.theme = ThemeScheme::Wallpaper;
    else if (theme == L"custom_image") s.theme = ThemeScheme::CustomImage;
    else if (theme == L"custom_color") s.theme = ThemeScheme::CustomColor;
    else s.theme = ThemeScheme::Emerald;

    s.backgroundBlur = Wh_GetIntSetting(L"backgroundBlur") != 0;
    int blurStr = Wh_GetIntSetting(L"blurStrength");
    s.blurStrength = (blurStr <= 0) ? 45 : std::clamp(blurStr, 10, 100);

    s.customBgColor = ParseHexColor(GetStringSettingSafe(L"customBgColor", L"#18181B"), RGB(24, 24, 27));
    s.customBgImagePath = Trim(GetStringSettingSafe(L"customBgImagePath", L""));
    s.bgOverlayOpacity = std::clamp(Wh_GetIntSetting(L"bgOverlayOpacity"), 0, 90);
    s.showMotif = Wh_GetIntSetting(L"showBackgroundMotif") != 0;
    s.enableAnimations = Wh_GetIntSetting(L"enableAnimations") != 0;

    std::wstring aStyle = Lower(Trim(GetStringSettingSafe(L"animationStyle", L"metro_swoosh")));
    if (aStyle == L"glide_fade") s.animStyle = AnimStyle::GlideFade;
    else if (aStyle == L"zoom_fade") s.animStyle = AnimStyle::ZoomFade;
    else s.animStyle = AnimStyle::MetroSwoosh;

    int openDur = Wh_GetIntSetting(L"openAnimationDuration");
    s.openAnimDurationMs = (openDur <= 0) ? 360 : std::clamp(openDur, 150, 800);

    int spd = Wh_GetIntSetting(L"animationSpeed");
    s.animationSpeed = (spd <= 0) ? 100 : std::clamp(spd, 50, 200);

    std::wstring sbStyle = Lower(Trim(GetStringSettingSafe(L"searchBarStyle", L"sleek_dark")));
    if (sbStyle == L"auto_hide") s.searchBarStyle = SearchBarStyle::AutoHide;
    else if (sbStyle == L"classic_light") s.searchBarStyle = SearchBarStyle::ClassicLight;
    else s.searchBarStyle = SearchBarStyle::SleekDark;

    int tScale = Wh_GetIntSetting(L"tileScalePercent");
    s.tileScalePercent = (tScale <= 0) ? 100 : std::clamp(tScale, 75, 135);

    int tGap = Wh_GetIntSetting(L"tileGapPx");
    s.tileGapPx = (tGap <= 0) ? 8 : std::clamp(tGap, 4, 16);

    s.showTileLabels = Wh_GetIntSetting(L"showTileLabels") != 0;
    s.showGroupHeaders = Wh_GetIntSetting(L"showGroupHeaders") != 0;

    std::wstring ff = Lower(Trim(GetStringSettingSafe(L"fontFamily", L"segoe_variable")));
    if (ff == L"segoe_semibold") s.fontChoice = FontChoice::SegoeSemibold;
    else if (ff == L"segoe_classic") s.fontChoice = FontChoice::SegoeClassic;
    else if (ff == L"bahnschrift") s.fontChoice = FontChoice::Bahnschrift;
    else if (ff == L"calibri") s.fontChoice = FontChoice::Calibri;
    else s.fontChoice = FontChoice::SegoeVariable;

    s.showBuiltinTiles = Wh_GetIntSetting(L"showBuiltinTiles") != 0;
    s.defaultTileRows = std::clamp(Wh_GetIntSetting(L"defaultTileRows"), 0, 6);
    s.enterInSearchLaunchesFirst = Wh_GetIntSetting(L"enterInSearchLaunchesFirst") != 0;
    s.autoRefreshMinutes = std::clamp(Wh_GetIntSetting(L"autoRefreshMinutes"), 0, 240);

    std::wstring mods = Lower(Trim(GetStringSettingSafe(L"hotkeyModifiers", L"ctrl_alt")));
    if (mods == L"alt_shift") s.hotkeyModifiers = MOD_ALT | MOD_SHIFT | MOD_NOREPEAT;
    else if (mods == L"ctrl_shift") s.hotkeyModifiers = MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT;
    else if (mods == L"win_shift") s.hotkeyModifiers = MOD_WIN | MOD_SHIFT | MOD_NOREPEAT;
    else s.hotkeyModifiers = MOD_CONTROL | MOD_ALT | MOD_NOREPEAT;

    std::wstring keyStr = Trim(GetStringSettingSafe(L"hotkeyKey", L"S"));
    if (!keyStr.empty()) {
        std::wstring up = keyStr;
        CharUpperBuffW(up.data(), static_cast<DWORD>(up.size()));
        if (up.size() == 1 && ((up[0] >= L'A' && up[0] <= L'Z') || (up[0] >= L'0' && up[0] <= L'9'))) {
            s.hotkeyVk = static_cast<UINT>(up[0]);
        } else if (up[0] == L'F' && up.size() >= 2) {
            int fn = _wtoi(up.c_str() + 1);
            if (fn >= 1 && fn <= 12) s.hotkeyVk = VK_F1 + (fn - 1);
        }
    }
    g_settings = s;
    g_openWithStartBtnAtomic = s.openWithStartButton;
    g_openWithWinKeyAtomic = s.openWithWinKey;

    g_runtimeThemeOverride = -1;
    g_runtimeAnimOverride = -1;
    g_runtimeMotifOverride = -1;
    g_runtimeBlurOverride = -1;
    g_runtimeLabelsOverride = -1;
    g_runtimeHeadersOverride = -1;
    FreeBackgroundBitmap();
}

////////////////////////////////////////////////////////////////////////////////
// High-Quality 32-Bit Premultiplied ARGB Icon Processing & Bilinear Scaling

void FreeAppCachedBitmaps(std::vector<AppEntry>& list) {
    for (auto& app : list) {
        if (app.cachedBmp) {
            DeleteObject(app.cachedBmp);
            app.cachedBmp = nullptr;
            app.cachedBmpSize = 0;
        }
    }
}

void FreeAppListIcons(std::vector<AppEntry>& list) {
    for (auto& app : list) {
        if (app.cachedBmp) {
            DeleteObject(app.cachedBmp);
            app.cachedBmp = nullptr;
            app.cachedBmpSize = 0;
        }
        if (app.icon && app.builtin == BuiltinKind::None) {
            DestroyIcon(app.icon);
        }
        app.icon = nullptr;
    }
}

// Normalizes 32-bit BGRA icon pixels: ensures alpha, premultiplies RGB by alpha,
// and trims excessive empty transparent borders so icons fill their canvas cleanly.
void ProcessIconPixels(std::vector<uint32_t> pixels, int width, int height, AppEntry& app) {
    if (width <= 0 || height <= 0 || pixels.size() < static_cast<size_t>(width) * static_cast<size_t>(height)) {
        return;
    }

    bool hasAlpha = false;
    for (uint32_t p : pixels) {
        if ((p >> 24) != 0) {
            hasAlpha = true;
            break;
        }
    }
    if (!hasAlpha) {
        for (uint32_t& p : pixels) p |= 0xFF000000u;
    }

    bool alreadyPremult = true;
    for (uint32_t p : pixels) {
        uint32_t a = p >> 24;
        if (((p >> 16) & 0xFF) > a || ((p >> 8) & 0xFF) > a || (p & 0xFF) > a) {
            alreadyPremult = false;
            break;
        }
    }
    if (!alreadyPremult) {
        for (uint32_t& p : pixels) {
            uint32_t a = p >> 24;
            uint32_t r = (((p >> 16) & 0xFF) * a + 127) / 255;
            uint32_t g = (((p >> 8) & 0xFF) * a + 127) / 255;
            uint32_t b = ((p & 0xFF) * a + 127) / 255;
            p = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }

    int left = width, top = height, right = -1, bottom = -1;
    for (int y = 0; y < height; ++y) {
        const uint32_t* row = &pixels[static_cast<size_t>(y) * width];
        for (int x = 0; x < width; ++x) {
            if ((row[x] >> 24) > 10) {
                left = std::min(left, x);
                right = std::max(right, x);
                top = std::min(top, y);
                bottom = std::max(bottom, y);
            }
        }
    }
    if (right < left || bottom < top) return;

    int full = std::max(width, height);
    int content = std::max(right - left + 1, bottom - top + 1);
    int side = (content < full * 0.65f) ? std::min(full, static_cast<int>(std::ceil(content * 1.12f))) : full;
    int cx = (left + right + 1) / 2;
    int cy = (top + bottom + 1) / 2;

    app.iconPixelSide = side;
    if (side == width && side == height && cx == width / 2 && cy == height / 2) {
        app.iconPixels = std::move(pixels);
        return;
    }

    app.iconPixels.assign(static_cast<size_t>(side) * side, 0);
    int x0 = cx - side / 2;
    int y0 = cy - side / 2;
    for (int y = 0; y < side; ++y) {
        int sy = y0 + y;
        if (sy < 0 || sy >= height) continue;
        for (int x = 0; x < side; ++x) {
            int sx = x0 + x;
            if (sx >= 0 && sx < width) {
                app.iconPixels[static_cast<size_t>(y) * side + x] =
                    pixels[static_cast<size_t>(sy) * width + sx];
            }
        }
    }
}

bool ExtractBitmapPixels32(HBITMAP hbmp, std::vector<uint32_t>& outPixels, int& outW, int& outH) {
    if (!hbmp) return false;
    BITMAP bm{};
    if (!GetObjectW(hbmp, sizeof(bm), &bm) || bm.bmWidth <= 0 || bm.bmHeight == 0) {
        return false;
    }
    outW = bm.bmWidth;
    outH = std::abs(bm.bmHeight);
    outPixels.resize(static_cast<size_t>(outW) * static_cast<size_t>(outH));

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = outW;
    bi.bmiHeader.biHeight = -outH; // Top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    HDC dc = GetDC(nullptr);
    int lines = GetDIBits(dc, hbmp, 0, outH, outPixels.data(), &bi, DIB_RGB_COLORS);
    ReleaseDC(nullptr, dc);
    return lines > 0;
}

void ExtractHighResAppIcon(IShellItem* item, const std::wstring& fallbackPath, int fetchSize, AppEntry& app) {
    if (item) {
        IShellItemImageFactory* factory = nullptr;
        if (SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(&factory))) && factory) {
            HBITMAP hbmp = nullptr;
            if (SUCCEEDED(factory->GetImage(SIZE{fetchSize, fetchSize}, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &hbmp)) && hbmp) {
                std::vector<uint32_t> raw;
                int w = 0, h = 0;
                if (ExtractBitmapPixels32(hbmp, raw, w, h)) {
                    ProcessIconPixels(std::move(raw), w, h, app);
                }
                DeleteObject(hbmp);
            }
            factory->Release();
        }
    }

    if (app.iconPixels.empty() && !fallbackPath.empty()) {
        IShellItem* pathItem = nullptr;
        if (SUCCEEDED(SHCreateItemFromParsingName(fallbackPath.c_str(), nullptr, IID_PPV_ARGS(&pathItem))) && pathItem) {
            IShellItemImageFactory* factory = nullptr;
            if (SUCCEEDED(pathItem->QueryInterface(IID_PPV_ARGS(&factory))) && factory) {
                HBITMAP hbmp = nullptr;
                if (SUCCEEDED(factory->GetImage(SIZE{fetchSize, fetchSize}, SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK, &hbmp)) && hbmp) {
                    std::vector<uint32_t> raw;
                    int w = 0, h = 0;
                    if (ExtractBitmapPixels32(hbmp, raw, w, h)) {
                        ProcessIconPixels(std::move(raw), w, h, app);
                    }
                    DeleteObject(hbmp);
                }
                factory->Release();
            }
            pathItem->Release();
        }
    }

    if (app.iconPixels.empty() && !fallbackPath.empty()) {
        SHFILEINFOW info{};
        if (SHGetFileInfoW(fallbackPath.c_str(), 0, &info, sizeof(info), SHGFI_ICON | SHGFI_LARGEICON) && info.hIcon) {
            ICONINFO ii{};
            if (GetIconInfo(info.hIcon, &ii)) {
                std::vector<uint32_t> raw;
                int w = 0, h = 0;
                if (ii.hbmColor && ExtractBitmapPixels32(ii.hbmColor, raw, w, h)) {
                    ProcessIconPixels(std::move(raw), w, h, app);
                }
                if (ii.hbmColor) DeleteObject(ii.hbmColor);
                if (ii.hbmMask) DeleteObject(ii.hbmMask);
            }
            app.icon = info.hIcon;
        }
    }
}

// High-quality area-averaging + bilinear resampler for premultiplied 32-bit ARGB icons.
// Eliminates staircasing and pixelation when scaling 256x256 down to 48x48 or scaling 32x32 up.
HBITMAP CreateSmoothScaledIconBitmap(const uint32_t* src, int srcSide, int dstSide) {
    if (!src || srcSide <= 0 || dstSide <= 0) return nullptr;

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = dstSide;
    bi.bmiHeader.biHeight = -dstSide; // Top-down DIB
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HDC screenDc = GetDC(nullptr);
    HBITMAP hbmp = CreateDIBSection(screenDc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    ReleaseDC(nullptr, screenDc);
    if (!hbmp || !bits) {
        if (hbmp) DeleteObject(hbmp);
        return nullptr;
    }

    uint32_t* dst = static_cast<uint32_t*>(bits);
    float scale = static_cast<float>(srcSide) / static_cast<float>(dstSide);

    if (scale > 1.0f) {
        // Area-averaging box filter for supersampled downscaling (e.g. 256 -> 52)
        for (int dy = 0; dy < dstSide; ++dy) {
            float sy0 = dy * scale;
            float sy1 = (dy + 1) * scale;
            int iy0 = std::clamp(static_cast<int>(std::floor(sy0)), 0, srcSide - 1);
            int iy1 = std::clamp(static_cast<int>(std::ceil(sy1)) - 1, 0, srcSide - 1);

            for (int dx = 0; dx < dstSide; ++dx) {
                float sx0 = dx * scale;
                float sx1 = (dx + 1) * scale;
                int ix0 = std::clamp(static_cast<int>(std::floor(sx0)), 0, srcSide - 1);
                int ix1 = std::clamp(static_cast<int>(std::ceil(sx1)) - 1, 0, srcSide - 1);

                float sumA = 0.0f, sumR = 0.0f, sumG = 0.0f, sumB = 0.0f, totalW = 0.0f;
                for (int sy = iy0; sy <= iy1; ++sy) {
                    float wy = std::min(sy1, static_cast<float>(sy + 1)) - std::max(sy0, static_cast<float>(sy));
                    if (wy <= 0.0f) continue;
                    const uint32_t* row = src + static_cast<size_t>(sy) * srcSide;
                    for (int sx = ix0; sx <= ix1; ++sx) {
                        float wx = std::min(sx1, static_cast<float>(sx + 1)) - std::max(sx0, static_cast<float>(sx));
                        if (wx <= 0.0f) continue;
                        float w = wx * wy;
                        uint32_t p = row[sx];
                        sumA += ((p >> 24) & 0xFF) * w;
                        sumR += ((p >> 16) & 0xFF) * w;
                        sumG += ((p >> 8) & 0xFF) * w;
                        sumB += (p & 0xFF) * w;
                        totalW += w;
                    }
                }
                if (totalW > 0.0f) {
                    uint32_t a = std::clamp(static_cast<int>(sumA / totalW + 0.5f), 0, 255);
                    uint32_t r = std::clamp(static_cast<int>(sumR / totalW + 0.5f), 0, static_cast<int>(a));
                    uint32_t g = std::clamp(static_cast<int>(sumG / totalW + 0.5f), 0, static_cast<int>(a));
                    uint32_t b = std::clamp(static_cast<int>(sumB / totalW + 0.5f), 0, static_cast<int>(a));
                    dst[static_cast<size_t>(dy) * dstSide + dx] = (a << 24) | (r << 16) | (g << 8) | b;
                } else {
                    dst[static_cast<size_t>(dy) * dstSide + dx] = 0;
                }
            }
        }
    } else {
        // Bilinear interpolation for upscaling smaller icons smoothly
        for (int dy = 0; dy < dstSide; ++dy) {
            float srcY = (dy + 0.5f) * scale - 0.5f;
            int y0 = std::clamp(static_cast<int>(std::floor(srcY)), 0, srcSide - 1);
            int y1 = std::clamp(y0 + 1, 0, srcSide - 1);
            float fy = std::clamp(srcY - std::floor(srcY), 0.0f, 1.0f);

            for (int dx = 0; dx < dstSide; ++dx) {
                float srcX = (dx + 0.5f) * scale - 0.5f;
                int x0 = std::clamp(static_cast<int>(std::floor(srcX)), 0, srcSide - 1);
                int x1 = std::clamp(x0 + 1, 0, srcSide - 1);
                float fx = std::clamp(srcX - std::floor(srcX), 0.0f, 1.0f);

                uint32_t p00 = src[static_cast<size_t>(y0) * srcSide + x0];
                uint32_t p10 = src[static_cast<size_t>(y0) * srcSide + x1];
                uint32_t p01 = src[static_cast<size_t>(y1) * srcSide + x0];
                uint32_t p11 = src[static_cast<size_t>(y1) * srcSide + x1];

                float w00 = (1.0f - fx) * (1.0f - fy);
                float w10 = fx * (1.0f - fy);
                float w01 = (1.0f - fx) * fy;
                float w11 = fx * fy;

                auto interp = [&](int shift) -> uint32_t {
                    float v = ((p00 >> shift) & 0xFF) * w00 +
                              ((p10 >> shift) & 0xFF) * w10 +
                              ((p01 >> shift) & 0xFF) * w01 +
                              ((p11 >> shift) & 0xFF) * w11;
                    return static_cast<uint32_t>(std::clamp(static_cast<int>(v + 0.5f), 0, 255));
                };
                uint32_t a = interp(24);
                uint32_t r = std::min(a, interp(16));
                uint32_t g = std::min(a, interp(8));
                uint32_t b = std::min(a, interp(0));
                dst[static_cast<size_t>(dy) * dstSide + dx] = (a << 24) | (r << 16) | (g << 8) | b;
            }
        }
    }
    return hbmp;
}

void DrawSmoothAppIcon(HDC dc, AppEntry& app, int x, int y, int targetSize) {
    if (targetSize <= 0) return;

    if (!app.iconPixels.empty() && app.iconPixelSide > 0) {
        if (!app.cachedBmp || app.cachedBmpSize != targetSize) {
            if (app.cachedBmp) DeleteObject(app.cachedBmp);
            app.cachedBmp = CreateSmoothScaledIconBitmap(app.iconPixels.data(), app.iconPixelSide, targetSize);
            app.cachedBmpSize = targetSize;
        }
        if (app.cachedBmp) {
            HDC memDc = CreateCompatibleDC(dc);
            HGDIOBJ old = SelectObject(memDc, app.cachedBmp);
            BLENDFUNCTION bf{};
            bf.BlendOp = AC_SRC_OVER;
            bf.BlendFlags = 0;
            bf.SourceConstantAlpha = 255;
            bf.AlphaFormat = AC_SRC_ALPHA;
            GdiAlphaBlend(dc, x, y, targetSize, targetSize, memDc, 0, 0, targetSize, targetSize, bf);
            SelectObject(memDc, old);
            DeleteDC(memDc);
            return;
        }
    }

    HICON iconToDraw = app.icon ? app.icon : g_fallbackIcon;
    if (iconToDraw) {
        DrawIconEx(dc, x, y, iconToDraw, targetSize, targetSize, 0, nullptr, DI_NORMAL);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Persistent Configuration (%LOCALAPPDATA%\Win8MetroStart\layout_v2.cfg)

std::wstring GetConfigDirectory() {
    PWSTR localAppData = nullptr;
    std::wstring dir;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_CREATE, nullptr, &localAppData)) && localAppData) {
        dir = localAppData;
        CoTaskMemFree(localAppData);
    } else {
        wchar_t buf[MAX_PATH]{};
        DWORD len = GetEnvironmentVariableW(L"LOCALAPPDATA", buf, MAX_PATH);
        if (len > 0 && len < MAX_PATH) dir = buf;
    }
    if (dir.empty()) return L"";
    dir += L"\\Win8MetroStart";
    CreateDirectoryW(dir.c_str(), nullptr);
    return dir;
}

std::wstring GetConfigFilePath() {
    std::wstring dir = GetConfigDirectory();
    if (dir.empty()) return L"";
    return dir + L"\\layout_v2.cfg";
}

std::wstring SanitizeField(std::wstring val) {
    for (wchar_t& ch : val) {
        if (ch == L'\t' || ch == L'\r' || ch == L'\n' || ch == L'|') ch = L' ';
    }
    return Trim(val);
}

void SaveCustomizations() {
    std::wstring path = GetConfigFilePath();
    if (path.empty()) return;

    std::wstring tmpPath = path + L".tmp";
    FILE* fp = _wfopen(tmpPath.c_str(), L"wb");
    if (!fp) return;

    const wchar_t bom = 0xFEFF;
    fwrite(&bom, sizeof(wchar_t), 1, fp);

    auto writeLine = [&](const std::wstring& line) {
        fwrite(line.data(), sizeof(wchar_t), line.size(), fp);
        const wchar_t nl[] = L"\r\n";
        fwrite(nl, sizeof(wchar_t), 2, fp);
    };

    writeLine(L"# Win8MetroStart Layout Configuration v2.2");
    if (g_runtimeThemeOverride >= 0) {
        writeLine(L"PREF\tTHEME\t" + std::to_wstring(g_runtimeThemeOverride));
    }
    if (g_runtimeAnimOverride >= 0) {
        writeLine(L"PREF\tANIM\t" + std::to_wstring(g_runtimeAnimOverride));
    }
    if (g_runtimeMotifOverride >= 0) {
        writeLine(L"PREF\tMOTIF\t" + std::to_wstring(g_runtimeMotifOverride));
    }
    if (g_runtimeBlurOverride >= 0) {
        writeLine(L"PREF\tBLUR\t" + std::to_wstring(g_runtimeBlurOverride));
    }
    if (g_runtimeLabelsOverride >= 0) {
        writeLine(L"PREF\tLABELS\t" + std::to_wstring(g_runtimeLabelsOverride));
    }
    if (g_runtimeHeadersOverride >= 0) {
        writeLine(L"PREF\tHEADERS\t" + std::to_wstring(g_runtimeHeadersOverride));
    }

    for (const auto& kv : g_groupRenames) {
        if (!kv.first.empty() && !kv.second.empty()) {
            writeLine(L"GROUP\t" + SanitizeField(kv.first) + L"\t" + SanitizeField(kv.second));
        }
    }

    for (const auto& kv : g_customTiles) {
        const std::wstring& id = kv.first;
        const TileCustomization& tc = kv.second;
        if (id.empty()) continue;
        std::wstringstream ss;
        ss << L"TILE\t" << SanitizeField(id)
           << L"\t" << static_cast<int>(tc.size)
           << L"\t" << tc.colorIndex
           << L"\t" << (tc.pinned ? 1 : 0)
           << L"\t" << tc.order
           << L"\t" << SanitizeField(tc.group);
        writeLine(ss.str());
    }

    fclose(fp);
    MoveFileExW(tmpPath.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
}

void LoadCustomizations() {
    g_customTiles.clear();
    g_groupRenames.clear();
    g_nextOrderCounter = 1;

    std::wstring path = GetConfigFilePath();
    if (path.empty()) return;

    FILE* fp = _wfopen(path.c_str(), L"rb");
    if (!fp) return;

    fseek(fp, 0, SEEK_END);
    long bytes = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (bytes <= 2 || bytes > 16 * 1024 * 1024) {
        fclose(fp);
        return;
    }

    std::vector<wchar_t> buf(static_cast<size_t>(bytes / sizeof(wchar_t)) + 1, L'\0');
    size_t readCount = fread(buf.data(), sizeof(wchar_t), bytes / sizeof(wchar_t), fp);
    fclose(fp);
    if (readCount == 0) return;

    size_t offset = (buf[0] == 0xFEFF) ? 1 : 0;
    std::wstring content(buf.data() + offset, readCount - offset);

    size_t pos = 0;
    while (pos < content.size()) {
        size_t end = content.find_first_of(L"\r\n", pos);
        if (end == std::wstring::npos) end = content.size();
        std::wstring line = Trim(content.substr(pos, end - pos));
        pos = end + 1;
        if (line.empty() || line[0] == L'#') continue;

        std::vector<std::wstring> parts;
        size_t tStart = 0;
        while (tStart <= line.size()) {
            size_t tEnd = line.find(L'\t', tStart);
            if (tEnd == std::wstring::npos) {
                parts.push_back(line.substr(tStart));
                break;
            }
            parts.push_back(line.substr(tStart, tEnd - tStart));
            tStart = tEnd + 1;
        }

        if (parts.size() >= 3 && parts[0] == L"PREF") {
            if (parts[1] == L"THEME") g_runtimeThemeOverride = std::clamp(_wtoi(parts[2].c_str()), 0, 8);
            else if (parts[1] == L"ANIM") g_runtimeAnimOverride = _wtoi(parts[2].c_str()) != 0 ? 1 : 0;
            else if (parts[1] == L"MOTIF") g_runtimeMotifOverride = _wtoi(parts[2].c_str()) != 0 ? 1 : 0;
            else if (parts[1] == L"BLUR") g_runtimeBlurOverride = _wtoi(parts[2].c_str()) != 0 ? 1 : 0;
            else if (parts[1] == L"LABELS") g_runtimeLabelsOverride = _wtoi(parts[2].c_str()) != 0 ? 1 : 0;
            else if (parts[1] == L"HEADERS") g_runtimeHeadersOverride = _wtoi(parts[2].c_str()) != 0 ? 1 : 0;
        } else if (parts.size() >= 3 && parts[0] == L"GROUP") {
            g_groupRenames[parts[1]] = parts[2];
        } else if (parts.size() >= 6 && parts[0] == L"TILE") {
            TileCustomization tc;
            std::wstring id = parts[1];
            int sz = _wtoi(parts[2].c_str());
            tc.size = static_cast<TileSize>(std::clamp(sz, 0, 3));
            tc.hasCustomSize = true;
            tc.colorIndex = std::clamp(_wtoi(parts[3].c_str()), -1, kPaletteSize - 1);
            tc.pinned = _wtoi(parts[4].c_str()) != 0;
            tc.order = _wtoi(parts[5].c_str());
            if (parts.size() >= 7) {
                tc.group = parts[6];
                if (tc.group == L"Pinned_System" || tc.group == L"Pinned   System") {
                    tc.group = L"Pinned";
                }
            }
            g_customTiles[id] = tc;
            g_nextOrderCounter = std::max(g_nextOrderCounter, tc.order + 1);
        }
    }
}

void SyncAppToCustomization(const AppEntry& app) {
    TileCustomization& tc = g_customTiles[app.id];
    tc.size = app.size;
    tc.hasCustomSize = true;
    tc.colorIndex = app.colorIndex;
    tc.group = app.group;
    tc.order = app.order;
    tc.pinned = app.pinned;
}

void ApplyCustomizationsToApps(std::vector<AppEntry>& apps) {
    for (size_t i = 0; i < apps.size(); ++i) {
        auto& app = apps[i];
        auto it = g_customTiles.find(app.id);
        if (it != g_customTiles.end()) {
            const TileCustomization& tc = it->second;
            if (tc.hasCustomSize) app.size = tc.size;
            app.colorIndex = tc.colorIndex;
            if (!tc.group.empty()) app.group = tc.group;
            app.order = tc.order;
            app.pinned = tc.pinned;
        } else {
            if (app.builtin == BuiltinKind::Desktop || app.builtin == BuiltinKind::Clock) {
                app.size = TileSize::Wide;
            } else {
                uint32_t h = static_cast<uint32_t>(StableColorIndex(app.id + app.title));
                app.size = (h % 5 == 0) ? TileSize::Wide : TileSize::Medium;
            }
            app.order = g_nextOrderCounter++;
            SyncAppToCustomization(app);
        }
        int cIdx = (app.colorIndex >= 0 && app.colorIndex < kPaletteSize)
                       ? app.colorIndex
                       : StableColorIndex(app.id);
        app.color = kPalette[cIdx];
        app.searchTitle = Lower(app.title);
        app.searchGroup = Lower(app.group);
        app.alphaBucket = GetAlphaBucket(app.title);
    }
}

std::wstring CleanGroupTitle(std::wstring name) {
    for (wchar_t& ch : name) {
        if (ch == L'_') ch = L' ';
    }
    if (name == L"Pinned & System" || name == L"Pinned System" || name == L"Pinned   System") {
        return L"Pinned";
    }
    return Trim(name);
}

std::wstring GetGroupDisplayName(const std::wstring& rawGroup) {
    auto it = g_groupRenames.find(rawGroup);
    if (it != g_groupRenames.end() && !it->second.empty()) {
        return CleanGroupTitle(it->second);
    }
    return CleanGroupTitle(rawGroup);
}

////////////////////////////////////////////////////////////////////////////////
// Comprehensive Application Discovery

std::wstring FileTitle(const std::wstring& path) {
    size_t slash = path.find_last_of(L"\\/");
    std::wstring name = (slash == std::wstring::npos) ? path : path.substr(slash + 1);
    if (EndsWith(name, L".lnk") || EndsWith(name, L".url") || EndsWith(name, L".exe")) {
        name.resize(name.size() - 4);
    }
    return Trim(name);
}

std::wstring GroupFromPath(const std::wstring& root, const std::wstring& path) {
    if (path.size() <= root.size()) return L"Main Apps";
    std::wstring relative = path.substr(root.size());
    while (!relative.empty() && (relative.front() == L'\\' || relative.front() == L'/')) {
        relative.erase(relative.begin());
    }
    size_t slash = relative.find_first_of(L"\\/");
    if (slash == std::wstring::npos || slash == 0) return L"Main Apps";
    std::wstring folder = Trim(relative.substr(0, slash));
    if (Lower(folder) == L"programs" || Lower(folder) == L"start menu") return L"Main Apps";
    return folder;
}

bool ShouldSkipApp(const std::wstring& title, const std::wstring& target) {
    std::wstring lt = Lower(Trim(title));
    std::wstring lp = Lower(target);
    if (lt.empty()) return true;
    // Skip broken MSIX / unexpanded resource strings like "{-brand-s..." or "ms-resource:..."
    if (lt[0] == L'{' || lt.rfind(L"ms-resource:", 0) == 0) {
        return true;
    }
    if (lt.find(L"uninstall") != std::wstring::npos ||
        lp.find(L"\\unins0") != std::wstring::npos ||
        lp.find(L"\\uninst.exe") != std::wstring::npos ||
        lp.find(L"\\uninstall.exe") != std::wstring::npos ||
        lp.rfind(L"http://", 0) == 0 ||
        lp.rfind(L"https://", 0) == 0) {
        return true;
    }
    for (PCWSTR ext : {L".chm", L".txt", L".pdf", L".rtf", L".htm", L".html", L".url", L".hlp"}) {
        if (EndsWith(lp, ext)) return true;
    }
    return false;
}

bool ValidateShortcutTarget(const std::wstring& lnkPath, std::wstring& outResolvedTarget) {
    outResolvedTarget.clear();
    IShellLinkW* psl = nullptr;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&psl))) || !psl) {
        return true;
    }
    IPersistFile* ppf = nullptr;
    bool valid = true;
    if (SUCCEEDED(psl->QueryInterface(IID_PPV_ARGS(&ppf))) && ppf) {
        if (SUCCEEDED(ppf->Load(lnkPath.c_str(), STGM_READ))) {
            wchar_t targetBuf[MAX_PATH]{};
            WIN32_FIND_DATAW wfd{};
            if (SUCCEEDED(psl->GetPath(targetBuf, MAX_PATH, &wfd, SLGP_RAWPATH)) && targetBuf[0] != L'\0') {
                wchar_t expanded[MAX_PATH]{};
                if (ExpandEnvironmentStringsW(targetBuf, expanded, MAX_PATH) > 0) {
                    outResolvedTarget = expanded;
                } else {
                    outResolvedTarget = targetBuf;
                }
                if (outResolvedTarget.size() > 3 && outResolvedTarget[1] == L':' && outResolvedTarget[2] == L'\\') {
                    DWORD attr = GetFileAttributesW(outResolvedTarget.c_str());
                    if (attr == INVALID_FILE_ATTRIBUTES) {
                        valid = false;
                    }
                }
            }
        }
        ppf->Release();
    }
    psl->Release();
    return valid;
}

struct ShortcutRecord {
    std::wstring title;
    std::wstring lnkPath;
    std::wstring resolvedExe;
    std::wstring group;
};

void ScanShortcutDir(const std::wstring& root, const std::wstring& current, int depth, std::vector<ShortcutRecord>& outRecords) {
    if (depth > 10 || g_stopDiscovery) return;
    std::wstring pattern = current + L"\\*";
    WIN32_FIND_DATAW data{};
    HANDLE find = FindFirstFileW(pattern.c_str(), &data);
    if (find == INVALID_HANDLE_VALUE) return;

    do {
        if (wcscmp(data.cFileName, L".") == 0 || wcscmp(data.cFileName, L"..") == 0) continue;
        if (data.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) continue;
        std::wstring child = current + L"\\" + data.cFileName;
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            ScanShortcutDir(root, child, depth + 1, outRecords);
        } else if (EndsWith(child, L".lnk")) {
            std::wstring title = FileTitle(child);
            std::wstring resolvedExe;
            if (!ValidateShortcutTarget(child, resolvedExe)) {
                continue;
            }
            if (ShouldSkipApp(title, resolvedExe.empty() ? child : resolvedExe)) {
                continue;
            }
            ShortcutRecord rec;
            rec.title = title;
            rec.lnkPath = child;
            rec.resolvedExe = resolvedExe;
            rec.group = GroupFromPath(root, child);
            outRecords.push_back(std::move(rec));
        }
    } while (!g_stopDiscovery && FindNextFileW(find, &data));

    FindClose(find);
}

std::vector<AppEntry>* DiscoverAllApplications(int fetchIconSize) {
    auto result = std::make_unique<std::vector<AppEntry>>();

    if (g_settings.showBuiltinTiles) {
        AppEntry desktopTile;
        desktopTile.id = L"builtin::desktop";
        desktopTile.title = L"Desktop";
        desktopTile.searchTitle = L"desktop";
        desktopTile.launchTarget = L"builtin::desktop";
        desktopTile.defaultGroup = L"Pinned";
        desktopTile.group = L"Pinned";
        desktopTile.size = TileSize::Wide;
        desktopTile.colorIndex = 4;
        desktopTile.color = kPalette[4];
        desktopTile.builtin = BuiltinKind::Desktop;
        desktopTile.icon = LoadIconW(nullptr, IDI_APPLICATION);
        result->push_back(std::move(desktopTile));

        AppEntry clockTile;
        clockTile.id = L"builtin::clock";
        clockTile.title = L"Calendar & Time";
        clockTile.searchTitle = L"calendar & time clock";
        clockTile.launchTarget = L"ms-settings:dateandtime";
        clockTile.defaultGroup = L"Pinned";
        clockTile.group = L"Pinned";
        clockTile.size = TileSize::Wide;
        clockTile.colorIndex = 2;
        clockTile.color = kPalette[2];
        clockTile.builtin = BuiltinKind::Clock;
        result->push_back(std::move(clockTile));
    }

    std::vector<ShortcutRecord> lnkRecords;
    PWSTR userPrograms = nullptr;
    PWSTR commonPrograms = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Programs, KF_FLAG_DEFAULT, nullptr, &userPrograms)) && userPrograms) {
        ScanShortcutDir(userPrograms, userPrograms, 0, lnkRecords);
        CoTaskMemFree(userPrograms);
    }
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_CommonPrograms, KF_FLAG_DEFAULT, nullptr, &commonPrograms)) && commonPrograms) {
        ScanShortcutDir(commonPrograms, commonPrograms, 0, lnkRecords);
        CoTaskMemFree(commonPrograms);
    }

    std::unordered_map<std::wstring, ShortcutRecord> lnkByTitle;
    std::unordered_map<std::wstring, ShortcutRecord> lnkByExe;
    for (const auto& rec : lnkRecords) {
        std::wstring lt = Lower(rec.title);
        if (lnkByTitle.find(lt) == lnkByTitle.end()) {
            lnkByTitle[lt] = rec;
        }
        if (!rec.resolvedExe.empty()) {
            std::wstring le = Lower(rec.resolvedExe);
            if (lnkByExe.find(le) == lnkByExe.end()) {
                lnkByExe[le] = rec;
            }
        }
    }

    std::unordered_set<std::wstring> seenIds;
    std::unordered_set<std::wstring> seenTitles;

    IShellItem* appsFolder = nullptr;
    IEnumShellItems* enumItems = nullptr;
    if (SUCCEEDED(SHCreateItemFromParsingName(L"shell:AppsFolder", nullptr, IID_PPV_ARGS(&appsFolder))) && appsFolder) {
        if (SUCCEEDED(appsFolder->BindToHandler(nullptr, kBhidEnumItems, IID_PPV_ARGS(&enumItems))) && enumItems) {
            IShellItem* item = nullptr;
            while (!g_stopDiscovery && enumItems->Next(1, &item, nullptr) == S_OK && item) {
                PWSTR displayName = nullptr;
                PWSTR parsingName = nullptr;
                if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &displayName)) &&
                    SUCCEEDED(item->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &parsingName)) &&
                    displayName && parsingName) {
                    std::wstring title = Trim(displayName);
                    std::wstring parsing = Trim(parsingName);
                    if (!ShouldSkipApp(title, parsing)) {
                        std::wstring normTitle = Lower(title);
                        std::wstring normId = L"appsfolder::" + Lower(parsing);

                        std::wstring group = L"Apps";
                        std::wstring fileLoc;
                        auto itTitle = lnkByTitle.find(normTitle);
                        auto itExe = lnkByExe.find(Lower(parsing));
                        if (itTitle != lnkByTitle.end()) {
                            group = itTitle->second.group;
                            fileLoc = itTitle->second.lnkPath;
                        } else if (itExe != lnkByExe.end()) {
                            group = itExe->second.group;
                            fileLoc = itExe->second.lnkPath;
                        } else if (parsing.find(L'!') != std::wstring::npos) {
                            group = L"Windows Apps";
                        }

                        if (seenIds.insert(normId).second && seenTitles.insert(normTitle).second) {
                            AppEntry entry;
                            entry.id = normId;
                            entry.title = title;
                            entry.launchTarget = L"shell:AppsFolder\\" + parsing;
                            entry.fileLocation = fileLoc;
                            entry.defaultGroup = group;
                            entry.group = group;
                            ExtractHighResAppIcon(item, fileLoc, fetchIconSize, entry);
                            result->push_back(std::move(entry));
                        }
                    }
                }
                if (displayName) CoTaskMemFree(displayName);
                if (parsingName) CoTaskMemFree(parsingName);
                item->Release();
            }
            enumItems->Release();
        }
        appsFolder->Release();
    }

    for (const auto& rec : lnkRecords) {
        if (g_stopDiscovery) break;
        std::wstring normTitle = Lower(rec.title);
        std::wstring normId = L"lnk::" + Lower(rec.lnkPath);
        if (seenTitles.find(normTitle) != seenTitles.end() || seenIds.find(normId) != seenIds.end()) {
            continue;
        }
        seenIds.insert(normId);
        seenTitles.insert(normTitle);

        AppEntry entry;
        entry.id = normId;
        entry.title = rec.title;
        entry.launchTarget = rec.lnkPath;
        entry.fileLocation = rec.lnkPath;
        entry.defaultGroup = rec.group;
        entry.group = rec.group;

        ExtractHighResAppIcon(nullptr, rec.lnkPath, fetchIconSize, entry);
        result->push_back(std::move(entry));
    }

    return result.release();
}

DWORD WINAPI DiscoveryThreadProc(void* param) {
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
    int fetchIconSize = static_cast<int>(reinterpret_cast<INT_PTR>(param));
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    std::vector<AppEntry>* discovered = DiscoverAllApplications(fetchIconSize);
    CoUninitialize();

    HWND hwnd = g_window.load();
    if (g_stopDiscovery || !hwnd || !IsWindow(hwnd) ||
        !PostMessageW(hwnd, WM_APP_APPS_UPDATED, 0, reinterpret_cast<LPARAM>(discovered))) {
        if (discovered) {
            FreeAppListIcons(*discovered);
            delete discovered;
        }
    }
    g_discoveryRunning = false;
    return 0;
}

void StartAsyncAppDiscovery() {
    if (g_discoveryRunning.exchange(true)) return;
    if (g_discoveryThread) {
        WaitForSingleObject(g_discoveryThread, INFINITE);
        CloseHandle(g_discoveryThread);
        g_discoveryThread = nullptr;
    }
    int fetchIconSize = std::clamp(STile(128), 96, 256);
    g_discoveryThread = CreateThread(nullptr, 0, DiscoveryThreadProc,
                                     reinterpret_cast<void*>(static_cast<INT_PTR>(fetchIconSize)), 0, nullptr);
    if (!g_discoveryThread) {
        g_discoveryRunning = false;
    }
}

void ClearApps() {
    FreeAppListIcons(g_apps);
    g_apps.clear();
    g_visibleApps.clear();
    g_headers.clear();
    g_alphaHeaders.clear();
}

////////////////////////////////////////////////////////////////////////////////
// Smooth Multi-Stage Animation Engine (Opacity + Staggered Group Swoosh + Scale)

float EaseOutCubic(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    float inv = 1.0f - t;
    return 1.0f - inv * inv * inv;
}

float EaseOutQuart(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    float inv = 1.0f - t;
    return 1.0f - inv * inv * inv * inv;
}

void UpdateWindowLayerAlpha() {
    HWND hwnd = g_window.load();
    if (!hwnd) return;
    if (!AnimationsEnabled() || g_openAnimProgress >= 1.0f) {
        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    } else {
        // Smooth opacity ramp from 30 -> 255 during opening
        float fadeT = std::clamp(g_openAnimProgress * 1.45f, 0.0f, 1.0f);
        BYTE alpha = static_cast<BYTE>(std::clamp(static_cast<int>(std::lround(35.0f + 220.0f * EaseOutCubic(fadeT))), 35, 255));
        SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA);
    }
}

void StartAnimationLoop() {
    HWND hwnd = g_window.load();
    if (!hwnd || !AnimationsEnabled()) return;
    if (!g_animTimerActive) {
        g_animTimerActive = true;
        g_lastAnimTick = GetTickCount64();
        SetTimer(hwnd, kTimerAnimation, 11, nullptr); // ~90 FPS
    }
}

void StopAnimationLoop() {
    HWND hwnd = g_window.load();
    if (g_animTimerActive && hwnd) {
        KillTimer(hwnd, kTimerAnimation);
    }
    g_animTimerActive = false;
}

void SetScrollTarget(int newTarget, bool immediate = false) {
    HWND hwnd = g_window.load();
    if (!hwnd) return;
    RECT client{};
    GetClientRect(hwnd, &client);
    int maxScroll = std::max(0, g_contentWidth - static_cast<int>(client.right) + S(60));
    g_scrollTargetX = std::clamp(newTarget, 0, maxScroll);

    if (immediate || !AnimationsEnabled()) {
        g_scrollX = g_scrollTargetX;
        g_scrollAnimX = static_cast<float>(g_scrollTargetX);
        InvalidateRect(hwnd, nullptr, FALSE);
    } else {
        StartAnimationLoop();
    }
}

void StepAnimationFrame() {
    HWND hwnd = g_window.load();
    if (!hwnd || !AnimationsEnabled()) {
        g_openAnimProgress = 1.0f;
        g_scrollX = g_scrollTargetX;
        g_scrollAnimX = static_cast<float>(g_scrollTargetX);
        UpdateWindowLayerAlpha();
        StopAnimationLoop();
        if (hwnd) InvalidateRect(hwnd, nullptr, FALSE);
        return;
    }

    ULONGLONG now = GetTickCount64();
    float dt = static_cast<float>(now - g_lastAnimTick) / 1000.0f;
    g_lastAnimTick = now;
    dt = std::clamp(dt, 0.001f, 0.05f);

    bool stillAnimating = false;

    if (g_openAnimProgress < 1.0f) {
        float durationSec = std::clamp(static_cast<float>(g_settings.openAnimDurationMs) / 1000.0f, 0.15f, 0.80f);
        g_openAnimProgress = std::min(1.0f, g_openAnimProgress + dt / durationSec);
        UpdateWindowLayerAlpha();
        if (g_openAnimProgress < 1.0f) stillAnimating = true;
    }

    float speedMult = static_cast<float>(g_settings.animationSpeed) / 100.0f;
    float diff = static_cast<float>(g_scrollTargetX) - g_scrollAnimX;
    if (std::fabs(diff) > 0.75f) {
        float stepFactor = std::min(1.0f, dt * 16.0f * speedMult);
        g_scrollAnimX += diff * stepFactor;
        g_scrollX = static_cast<int>(std::lround(g_scrollAnimX));
        stillAnimating = true;
    } else if (g_scrollX != g_scrollTargetX) {
        g_scrollAnimX = static_cast<float>(g_scrollTargetX);
        g_scrollX = g_scrollTargetX;
    }

    InvalidateRect(hwnd, nullptr, FALSE);
    if (!stillAnimating) {
        UpdateWindowLayerAlpha();
        StopAnimationLoop();
    }
}

// Computes per-group horizontal entrance offset and tile scale factor
void GetGroupEntranceTransform(int groupIdx, int& outOffsetX, float& outScale) {
    outOffsetX = 0;
    outScale = 1.0f;
    if (!AnimationsEnabled() || g_openAnimProgress >= 1.0f) return;

    if (g_settings.animStyle == AnimStyle::MetroSwoosh) {
        // Staggered per-group progress so groups cascade in smoothly from the right
        float stagger = std::min(0.36f, groupIdx * 0.075f);
        float localT = std::clamp((g_openAnimProgress - stagger) / (1.0f - stagger), 0.0f, 1.0f);
        float ease = EaseOutQuart(localT);
        outOffsetX = static_cast<int>(std::lround((1.0f - ease) * (S(130) + groupIdx * S(36))));
        outScale = 0.90f + 0.10f * ease;
    } else if (g_settings.animStyle == AnimStyle::GlideFade) {
        float ease = EaseOutCubic(g_openAnimProgress);
        outOffsetX = static_cast<int>(std::lround((1.0f - ease) * S(110)));
        outScale = 1.0f;
    } else {
        float ease = EaseOutCubic(g_openAnimProgress);
        outOffsetX = 0;
        outScale = 0.88f + 0.12f * ease;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Clean Windows 8.1 UI Rectangles & Deterministic Layout Engine

RECT ProfilePillRect(int width) {
    return RECT{width - S(256), S(34), width - S(86), S(74)};
}

RECT TopPowerButtonRect(int width) {
    return RECT{width - S(78), S(34), width - S(38), S(74)};
}

RECT SearchBoxRect(int width) {
    if (g_settings.searchBarStyle == SearchBarStyle::AutoHide && !g_searchFocused && g_searchText.empty()) {
        // Compact search icon button next to the profile pill
        return RECT{width - S(304), S(34), width - S(264), S(74)};
    }
    return RECT{std::max(S(280), width - S(520)), S(36), width - S(268), S(72)};
}

RECT SearchClearRect(int width) {
    RECT sr = SearchBoxRect(width);
    return RECT{sr.right - S(30), sr.top + S(4), sr.right - S(4), sr.bottom - S(4)};
}

RECT ProfileMenuRect(int width) {
    int itemH = S(40);
    return RECT{width - S(256), S(78), width - S(38), S(78) + itemH * 4 + S(10)};
}

RECT PowerMenuRect(int width, int /*height*/) {
    int itemH = S(40);
    return RECT{width - S(228), S(78), width - S(38), S(78) + itemH * 5 + S(10)};
}

// Authentic Windows 8.1 circular All Apps button at the bottom-left under the tiles
RECT AllAppsButtonRect(int /*width*/, int height) {
    int sz = S(40);
    return RECT{S(56), height - S(62), S(56) + sz, height - S(62) + sz};
}


RECT ScrollbarTrackRect(int width, int height) {
    return RECT{S(112), height - S(12), width - S(48), height - S(5)};
}

RECT ScrollbarThumbRect(int width, int height) {
    RECT track = ScrollbarTrackRect(width, height);
    int trackWidth = std::max(1, static_cast<int>(track.right - track.left));
    int thumbWidth = std::max(S(48), MulDiv(trackWidth, width, std::max(width, g_contentWidth)));
    thumbWidth = std::min(trackWidth, thumbWidth);
    int maxScroll = std::max(1, g_contentWidth - width + S(60));
    int thumbX = track.left + MulDiv(trackWidth - thumbWidth, std::clamp(g_scrollX, 0, maxScroll), maxScroll);
    return RECT{thumbX, track.top, thumbX + thumbWidth, track.bottom};
}

void GetTileSpan(TileSize sz, int& spanX, int& spanY) {
    switch (sz) {
        case TileSize::Small:  spanX = 1; spanY = 1; break;
        case TileSize::Medium: spanX = 2; spanY = 2; break;
        case TileSize::Wide:   spanX = 4; spanY = 2; break;
        case TileSize::Large:  spanX = 4; spanY = 4; break;
    }
}

void ClampScroll() {
    HWND hwnd = g_window.load();
    if (!hwnd) return;
    RECT client{};
    GetClientRect(hwnd, &client);
    int maxScroll = std::max(0, g_contentWidth - static_cast<int>(client.right) + S(60));
    g_scrollTargetX = std::clamp(g_scrollTargetX, 0, maxScroll);
    g_scrollX = std::clamp(g_scrollX, 0, maxScroll);
    g_scrollAnimX = static_cast<float>(g_scrollX);
}

void RebuildLayout(bool resetScroll = false) {
    if (resetScroll) {
        g_scrollX = 0;
        g_scrollTargetX = 0;
        g_scrollAnimX = 0.0f;
    }
    g_contentWidth = 0;
    g_visibleApps.clear();
    g_headers.clear();
    g_alphaHeaders.clear();
    HWND hwnd = g_window.load();
    if (!hwnd) return;

    RECT client{};
    GetClientRect(hwnd, &client);
    int height = std::max(1, static_cast<int>(client.bottom));
    int top = GroupHeadersEnabled() ? S(148) : S(116);
    int bottom = height - S(72);

    std::wstring query = Lower(Trim(g_searchText));
    bool isSearchActive = !query.empty();
    bool useListLayout = g_allAppsView || isSearchActive;

    for (size_t i = 0; i < g_apps.size(); ++i) {
        g_apps[i].rect = RECT{0, 0, 0, 0};
        if (!useListLayout && !g_apps[i].pinned) continue;
        if (!useListLayout && !g_settings.showBuiltinTiles && g_apps[i].builtin != BuiltinKind::None) continue;
        if (useListLayout && g_apps[i].builtin == BuiltinKind::Clock) continue;

        if (isSearchActive) {
            std::wstring dispGroup = Lower(GetGroupDisplayName(g_apps[i].group));
            if (g_apps[i].searchTitle.find(query) == std::wstring::npos &&
                g_apps[i].searchGroup.find(query) == std::wstring::npos &&
                dispGroup.find(query) == std::wstring::npos) {
                continue;
            }
        }
        g_visibleApps.push_back(static_cast<int>(i));
    }

    if (useListLayout) {
        std::sort(g_visibleApps.begin(), g_visibleApps.end(), [&](int a, int b) {
            if (isSearchActive) {
                bool aStarts = g_apps[a].searchTitle.rfind(query, 0) == 0;
                bool bStarts = g_apps[b].searchTitle.rfind(query, 0) == 0;
                if (aStarts != bStarts) return aStarts;
            }
            if (g_apps[a].alphaBucket != g_apps[b].alphaBucket) {
                if (g_apps[a].alphaBucket == L'#') return false;
                if (g_apps[b].alphaBucket == L'#') return true;
                return g_apps[a].alphaBucket < g_apps[b].alphaBucket;
            }
            return g_apps[a].searchTitle < g_apps[b].searchTitle;
        });

        int rowHeight = S(46);
        int columnWidth = S(300);
        int availableHeight = std::max(rowHeight * 2, bottom - top);
        int rows = std::max(2, availableHeight / rowHeight);
        g_subRows = rows;

        int col = 0;
        int row = 0;
        int left = S(56);
        wchar_t currentBucket = 0;

        for (size_t i = 0; i < g_visibleApps.size(); ++i) {
            int appIdx = g_visibleApps[i];
            wchar_t bucket = g_apps[appIdx].alphaBucket;
            if (!isSearchActive && bucket != currentBucket) {
                currentBucket = bucket;
                if (row > rows - 2) {
                    row = 0;
                    ++col;
                }
                int hx = left + col * columnWidth;
                int hy = top + row * rowHeight;
                AlphaHeader ah;
                ah.letter = bucket;
                ah.rect = RECT{hx, hy, hx + columnWidth - S(16), hy + rowHeight - S(4)};
                ah.firstVisibleIdx = static_cast<int>(i);
                g_alphaHeaders.push_back(ah);
                ++row;
                if (row >= rows) {
                    row = 0;
                    ++col;
                }
            }

            int x = left + col * columnWidth;
            int y = top + row * rowHeight;
            g_apps[appIdx].groupIdx = col / 2;
            g_apps[appIdx].rect = RECT{x, y, x + columnWidth - S(16), y + rowHeight - S(4)};
            g_contentWidth = std::max(g_contentWidth, static_cast<int>(g_apps[appIdx].rect.right));

            ++row;
            if (row >= rows) {
                row = 0;
                ++col;
            }
        }
    } else {
        auto groupRank = [](const std::wstring& g) -> int {
            if (g == L"Pinned" || g == L"Pinned & System" || g == L"Pinned_System") return 0;
            if (g == L"Main Apps") return 1;
            if (g == L"Windows Apps" || g == L"Windows & Store Apps") return 3;
            return 2;
        };

        std::sort(g_visibleApps.begin(), g_visibleApps.end(), [&](int a, int b) {
            const auto& appA = g_apps[a];
            const auto& appB = g_apps[b];
            if (appA.group != appB.group) {
                int rA = groupRank(appA.group);
                int rB = groupRank(appB.group);
                if (rA != rB) return rA < rB;
                return Lower(appA.group) < Lower(appB.group);
            }
            if (appA.order != appB.order) return appA.order < appB.order;
            return appA.searchTitle < appB.searchTitle;
        });

        int availableHeight = std::max(S(240), bottom - top);
        int mediumRows = g_settings.defaultTileRows > 0
                             ? g_settings.defaultTileRows
                             : std::clamp(availableHeight / STile(152), 2, 5);
        int subRows = mediumRows * 2;
        g_subRows = subRows;

        int gap = S(g_settings.tileGapPx);
        int rawSubCell = (availableHeight - gap * (subRows - 1)) / subRows;
        int minSubCell = STile(30);
        int maxSubCell = STile(64);
        int subCell = std::clamp(rawSubCell, minSubCell, maxSubCell);
        int step = subCell + gap;
        int groupX = S(56);
        int groupCounter = 0;

        size_t start = 0;
        while (start < g_visibleApps.size()) {
            std::wstring groupName = g_apps[g_visibleApps[start]].group;
            std::vector<int> groupApps;
            while (start < g_visibleApps.size() && g_apps[g_visibleApps[start]].group == groupName) {
                groupApps.push_back(g_visibleApps[start]);
                ++start;
            }
            if (groupApps.empty()) continue;

            int maxColsAllocated = std::max(32, static_cast<int>(groupApps.size()) * 4 + 8);
            std::vector<std::vector<bool>> occupied(
                static_cast<size_t>(subRows),
                std::vector<bool>(static_cast<size_t>(maxColsAllocated), false));

            int maxColUsed = 0;

            for (int appIdx : groupApps) {
                int spanX = 2, spanY = 2;
                GetTileSpan(g_apps[appIdx].size, spanX, spanY);
                spanY = std::min(spanY, subRows);

                int foundRow = -1, foundCol = -1;
                for (int bandStart = 0; bandStart < maxColsAllocated - spanX && foundCol < 0; bandStart += 4) {
                    for (int row = 0; row <= subRows - spanY && foundCol < 0; ++row) {
                        for (int colOffset = 0; colOffset <= 4 - spanX; ++colOffset) {
                            int col = bandStart + colOffset;
                            if (col + spanX > maxColsAllocated) continue;
                            bool free = true;
                            for (int yy = row; yy < row + spanY && free; ++yy) {
                                for (int xx = col; xx < col + spanX; ++xx) {
                                    if (occupied[static_cast<size_t>(yy)][static_cast<size_t>(xx)]) {
                                        free = false;
                                        break;
                                    }
                                }
                            }
                            if (free) {
                                foundRow = row;
                                foundCol = col;
                                break;
                            }
                        }
                    }
                }

                if (foundCol < 0) continue;
                for (int yy = foundRow; yy < foundRow + spanY; ++yy) {
                    for (int xx = foundCol; xx < foundCol + spanX; ++xx) {
                        occupied[static_cast<size_t>(yy)][static_cast<size_t>(xx)] = true;
                    }
                }
                maxColUsed = std::max(maxColUsed, foundCol + spanX);

                int x = groupX + foundCol * step;
                int y = top + foundRow * step;
                int w = spanX * step - gap;
                int h = spanY * step - gap;
                g_apps[appIdx].groupIdx = groupCounter;
                g_apps[appIdx].rect = RECT{x, y, x + w, y + h};
            }

            int groupWidthPx = std::max(step * 2, maxColUsed * step - gap);
            SectionHeader sh;
            sh.rawGroup = groupName;
            sh.displayTitle = GetGroupDisplayName(groupName);
            sh.x = groupX;
            sh.groupIdx = groupCounter;
            sh.headerRect = RECT{groupX, S(108), groupX + std::max(S(140), groupWidthPx), S(142)};
            g_headers.push_back(std::move(sh));

            groupX += groupWidthPx + S(44);
            ++groupCounter;
        }
        g_contentWidth = groupX;
    }

    if (g_selectedApp >= 0) {
        bool found = std::find(g_visibleApps.begin(), g_visibleApps.end(), g_selectedApp) != g_visibleApps.end();
        if (!found) g_selectedApp = g_visibleApps.empty() ? -1 : g_visibleApps.front();
    } else if (!g_visibleApps.empty()) {
        g_selectedApp = g_visibleApps.front();
    }

    ClampScroll();
    InvalidateRect(hwnd, nullptr, FALSE);
}

void EnsureSelectionVisible() {
    HWND hwnd = g_window.load();
    if (!hwnd || g_selectedApp < 0 || static_cast<size_t>(g_selectedApp) >= g_apps.size()) return;
    RECT client{};
    GetClientRect(hwnd, &client);
    const RECT& r = g_apps[static_cast<size_t>(g_selectedApp)].rect;
    if (r.right <= r.left) return;
    int margin = S(56);
    int target = g_scrollTargetX;
    if (r.left - target < margin) {
        target = std::max(0, static_cast<int>(r.left) - margin);
    } else if (r.right - target > client.right - margin) {
        target = static_cast<int>(r.right) - client.right + margin;
    }
    SetScrollTarget(target, false);
}

////////////////////////////////////////////////////////////////////////////////
// Background Image Loader, Blur Filter & Theme Rendering

std::wstring GetDesktopWallpaperPath() {
    wchar_t path[MAX_PATH]{};
    if (SystemParametersInfoW(SPI_GETDESKWALLPAPER, MAX_PATH, path, 0) && path[0] != L'\0') {
        return path;
    }
    return L"";
}

void BoxBlurPass32(const uint32_t* src, uint32_t* dst, int lines, int length,
                   int lineStep, int step, int radius) {
    int window = 2 * radius + 1;
    for (int line = 0; line < lines; ++line) {
        const uint32_t* s = src + static_cast<size_t>(line) * lineStep;
        uint32_t* d = dst + static_cast<size_t>(line) * lineStep;
        auto at = [&](int i) -> uint32_t {
            return s[static_cast<size_t>(std::clamp(i, 0, length - 1)) * step];
        };
        int sumR = 0, sumG = 0, sumB = 0;
        for (int i = -radius; i <= radius; ++i) {
            uint32_t p = at(i);
            sumB += p & 0xFF;
            sumG += (p >> 8) & 0xFF;
            sumR += (p >> 16) & 0xFF;
        }
        for (int i = 0; i < length; ++i) {
            d[static_cast<size_t>(i) * step] =
                0xFF000000u | ((sumR / window) << 16) | ((sumG / window) << 8) | (sumB / window);
            uint32_t inP = at(i + radius + 1);
            uint32_t outP = at(i - radius);
            sumB += static_cast<int>(inP & 0xFF) - static_cast<int>(outP & 0xFF);
            sumG += static_cast<int>((inP >> 8) & 0xFF) - static_cast<int>((outP >> 8) & 0xFF);
            sumR += static_cast<int>((inP >> 16) & 0xFF) - static_cast<int>((outP >> 16) & 0xFF);
        }
    }
}

void EnsureBackgroundBitmapLoaded(int screenW, int screenH) {
    ThemeScheme scheme = ActiveTheme();
    std::wstring desiredPath;
    if (scheme == ThemeScheme::Wallpaper) {
        desiredPath = GetDesktopWallpaperPath();
    } else if (scheme == ThemeScheme::CustomImage) {
        desiredPath = g_settings.customBgImagePath;
    } else {
        return;
    }

    if (desiredPath.empty()) return;
    bool useBlur = BlurEnabled();
    std::wstring cacheKey = desiredPath + L"|" + (useBlur ? L"B" + std::to_wstring(g_settings.blurStrength) : L"N") +
                            L"|" + std::to_wstring(g_settings.bgOverlayOpacity);
    if (g_bgBitmap && g_loadedBgKey == cacheKey && g_bgBitmapW == screenW && g_bgBitmapH == screenH) {
        return;
    }

    FreeBackgroundBitmap();

    IShellItem* item = nullptr;
    if (FAILED(SHCreateItemFromParsingName(desiredPath.c_str(), nullptr, IID_PPV_ARGS(&item))) || !item) {
        return;
    }

    IShellItemImageFactory* factory = nullptr;
    HBITMAP rawBmp = nullptr;
    if (SUCCEEDED(item->QueryInterface(IID_PPV_ARGS(&factory))) && factory) {
        factory->GetImage(SIZE{screenW, screenH}, SIIGBF_BIGGERSIZEOK, &rawBmp);
        factory->Release();
    }
    item->Release();
    if (!rawBmp) return;

    BITMAP bm{};
    if (!GetObjectW(rawBmp, sizeof(bm), &bm) || bm.bmWidth <= 0 || bm.bmHeight == 0) {
        DeleteObject(rawBmp);
        return;
    }

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = screenW;
    bi.bmiHeader.biHeight = -screenH;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;

    HDC screenDc = GetDC(nullptr);
    HDC dstDc = CreateCompatibleDC(screenDc);
    HDC srcDc = CreateCompatibleDC(screenDc);
    HBITMAP scaledBmp = CreateDIBSection(dstDc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);

    if (scaledBmp && bits) {
        HGDIOBJ oldDst = SelectObject(dstDc, scaledBmp);
        HGDIOBJ oldSrc = SelectObject(srcDc, rawBmp);
        SetStretchBltMode(dstDc, HALFTONE);
        SetBrushOrgEx(dstDc, 0, 0, nullptr);
        StretchBlt(dstDc, 0, 0, screenW, screenH, srcDc, 0, 0, bm.bmWidth, std::abs(bm.bmHeight), SRCCOPY);
        GdiFlush();

        uint32_t* px = static_cast<uint32_t*>(bits);
        size_t count = static_cast<size_t>(screenW) * static_cast<size_t>(screenH);

        if (useBlur) {
            int radius = std::clamp(g_settings.blurStrength / 3, 3, 32);
            std::vector<uint32_t> temp(count);
            for (int pass = 0; pass < 2; ++pass) {
                BoxBlurPass32(px, temp.data(), screenH, screenW, screenW, 1, radius);
                BoxBlurPass32(temp.data(), px, screenW, screenH, 1, screenW, radius);
            }
        }

        int keepPct = std::clamp(100 - g_settings.bgOverlayOpacity, 10, 100);
        for (size_t i = 0; i < count; ++i) {
            uint32_t p = px[i];
            uint32_t r = ((p >> 16) & 0xFF) * keepPct / 100;
            uint32_t g = ((p >> 8) & 0xFF) * keepPct / 100;
            uint32_t b = (p & 0xFF) * keepPct / 100;
            px[i] = 0xFF000000u | (r << 16) | (g << 8) | b;
        }

        SelectObject(srcDc, oldSrc);
        SelectObject(dstDc, oldDst);
        g_bgBitmap = scaledBmp;
        g_bgBitmapW = screenW;
        g_bgBitmapH = screenH;
        g_loadedBgKey = cacheKey;
    } else if (scaledBmp) {
        DeleteObject(scaledBmp);
    }

    DeleteDC(srcDc);
    DeleteDC(dstDc);
    ReleaseDC(nullptr, screenDc);
    DeleteObject(rawBmp);
}

void GetThemeColors(COLORREF& outBg, COLORREF& outAccent1, COLORREF& outAccent2) {
    switch (ActiveTheme()) {
        case ThemeScheme::Dark:
            outBg = RGB(29, 29, 29);
            outAccent1 = RGB(38, 38, 38);
            outAccent2 = RGB(48, 48, 48);
            break;
        case ThemeScheme::Indigo:
            outBg = RGB(24, 0, 82);
            outAccent1 = RGB(36, 8, 104);
            outAccent2 = RGB(48, 14, 124);
            break;
        case ThemeScheme::Teal:
            outBg = RGB(0, 64, 80);
            outAccent1 = RGB(0, 82, 102);
            outAccent2 = RGB(0, 96, 118);
            break;
        case ThemeScheme::Crimson:
            outBg = RGB(59, 9, 24);
            outAccent1 = RGB(78, 14, 33);
            outAccent2 = RGB(96, 18, 42);
            break;
        case ThemeScheme::Cobalt:
            outBg = RGB(11, 37, 69);
            outAccent1 = RGB(19, 53, 94);
            outAccent2 = RGB(27, 67, 118);
            break;
        case ThemeScheme::CustomColor:
            outBg = g_settings.customBgColor;
            outAccent1 = LightenColor(outBg, 14);
            outAccent2 = LightenColor(outBg, 26);
            break;
        case ThemeScheme::Emerald:
        default:
            outBg = RGB(0, 77, 38);
            outAccent1 = RGB(0, 94, 48);
            outAccent2 = RGB(6, 112, 58);
            break;
    }
}

void FillSolidRect(HDC dc, const RECT& r, COLORREF color) {
    if (r.right <= r.left || r.bottom <= r.top) return;
    COLORREF old = SetBkColor(dc, color);
    ExtTextOutW(dc, 0, 0, ETO_OPAQUE, &r, nullptr, 0, nullptr);
    SetBkColor(dc, old);
}

void DrawRectOutline(HDC dc, const RECT& r, COLORREF color, int thickness) {
    if (r.right <= r.left || r.bottom <= r.top) return;
    RECT top{r.left, r.top, r.right, r.top + thickness};
    RECT bot{r.left, r.bottom - thickness, r.right, r.bottom};
    RECT lft{r.left, r.top, r.left + thickness, r.bottom};
    RECT rgt{r.right - thickness, r.top, r.right, r.bottom};
    FillSolidRect(dc, top, color);
    FillSolidRect(dc, bot, color);
    FillSolidRect(dc, lft, color);
    FillSolidRect(dc, rgt, color);
}

void DrawTextAt(HDC dc, HFONT font, const std::wstring& text, RECT rect,
                COLORREF color, UINT flags = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS) {
    if (text.empty() || rect.right <= rect.left || rect.bottom <= rect.top) return;
    HFONT oldFont = static_cast<HFONT>(SelectObject(dc, font));
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, color);
    DrawTextW(dc, text.c_str(), static_cast<int>(text.size()), &rect, flags);
    SelectObject(dc, oldFont);
}

void DrawBackgroundCanvas(HDC dc, int width, int height) {
    ThemeScheme scheme = ActiveTheme();
    if (scheme == ThemeScheme::Wallpaper || scheme == ThemeScheme::CustomImage) {
        EnsureBackgroundBitmapLoaded(width, height);
        if (g_bgBitmap) {
            HDC memDc = CreateCompatibleDC(dc);
            HGDIOBJ old = SelectObject(memDc, g_bgBitmap);
            BitBlt(dc, 0, 0, width, height, memDc, 0, 0, SRCCOPY);
            SelectObject(memDc, old);
            DeleteDC(memDc);
            return;
        }
    }

    COLORREF bg, acc1, acc2;
    GetThemeColors(bg, acc1, acc2);
    RECT full{0, 0, width, height};
    FillSolidRect(dc, full, bg);
    if (!MotifEnabled()) return;

    int parallax = g_scrollX / 6;
    RECT b1{S(60) - parallax, 0, S(240) - parallax, S(220)};
    RECT b2{S(240) - parallax, 0, S(380) - parallax, S(140)};
    RECT b3{width - S(420) - parallax / 2, 0, width - S(260) - parallax / 2, S(260)};
    RECT b4{S(180) - parallax, height - S(180), S(480) - parallax, height};
    FillSolidRect(dc, b1, acc1);
    FillSolidRect(dc, b2, acc2);
    FillSolidRect(dc, b3, acc1);
    FillSolidRect(dc, b4, acc1);

    HPEN ringPen = CreatePen(PS_SOLID, S(2), acc2);
    HGDIOBJ oldPen = SelectObject(dc, ringPen);
    HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(HOLLOW_BRUSH));
    int cx = width - S(240) - parallax / 3;
    int cy = S(30);
    for (int rad = S(40); rad <= S(160); rad += S(24)) {
        Ellipse(dc, cx - rad, cy - rad, cx + rad, cy + rad);
    }
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(ringPen);
}

void DrawBuiltinDesktopTile(HDC dc, const RECT& r) {
    FillSolidRect(dc, r, RGB(0, 114, 130));
    RECT band1{r.left, r.top + (r.bottom - r.top) / 2, r.right, r.bottom};
    FillSolidRect(dc, band1, RGB(0, 92, 106));

    int mw = STile(58), mh = STile(38);
    int mx = (r.left + r.right - mw) / 2;
    int my = r.top + (r.bottom - r.top - mh) / 2 - (LabelsEnabled() ? S(6) : 0);
    RECT mon{mx, my, mx + mw, my + mh};
    DrawRectOutline(dc, mon, RGB(255, 255, 255), S(3));
    RECT stand{mx + mw / 2 - S(9), mon.bottom, mx + mw / 2 + S(9), mon.bottom + S(5)};
    FillSolidRect(dc, stand, RGB(255, 255, 255));
    RECT base{mx + mw / 2 - S(16), mon.bottom + S(5), mx + mw / 2 + S(16), mon.bottom + S(8)};
    FillSolidRect(dc, base, RGB(255, 255, 255));
}

void DrawBuiltinClockTile(HDC dc, const RECT& r, COLORREF baseColor) {
    FillSolidRect(dc, r, baseColor);
    SYSTEMTIME st{};
    GetLocalTime(&st);

    wchar_t timeStr[64]{};
    wchar_t dateStr[128]{};
    wchar_t dayStr[64]{};
    GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &st, nullptr, timeStr, 64);
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, L"dddd", dayStr, 64, nullptr);
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, DATE_SHORTDATE, &st, nullptr, dateStr, 128, nullptr);

    RECT timeRect{r.left + S(16), r.top + S(12), r.right - S(14), r.top + S(70)};
    DrawTextAt(dc, g_fontClockBig, timeStr, timeRect, RGB(255, 255, 255), DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT dayRect{r.left + S(16), r.bottom - S(52), r.right - S(14), r.bottom - S(28)};
    DrawTextAt(dc, g_fontSubtitle, dayStr, dayRect, RGB(240, 240, 240), DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT dateRect{r.left + S(16), r.bottom - S(30), r.right - S(14), r.bottom - S(6)};
    DrawTextAt(dc, g_fontTile, dateStr, dateRect, RGB(255, 255, 255), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

RECT ApplyTileScale(const RECT& r, float scale) {
    if (scale >= 0.999f) return r;
    int cx = (r.left + r.right) / 2;
    int cy = (r.top + r.bottom) / 2;
    int halfW = static_cast<int>(std::lround((r.right - r.left) * 0.5f * scale));
    int halfH = static_cast<int>(std::lround((r.bottom - r.top) * 0.5f * scale));
    return RECT{cx - halfW, cy - halfH, cx + halfW, cy + halfH};
}

void DrawTile(HDC dc, int index, bool isSelected, bool isHovered) {
    AppEntry& app = g_apps[static_cast<size_t>(index)];
    int animOffsetX = 0;
    float tileScale = 1.0f;
    GetGroupEntranceTransform(app.groupIdx, animOffsetX, tileScale);

    RECT r = app.rect;
    OffsetRect(&r, -g_scrollX + animOffsetX, 0);
    r = ApplyTileScale(r, tileScale);
    if (r.right <= r.left || r.bottom <= r.top) return;

    COLORREF color = isHovered ? LightenColor(app.color, 18) : app.color;
    bool showLabels = LabelsEnabled();

    if (app.builtin == BuiltinKind::Desktop) {
        DrawBuiltinDesktopTile(dc, r);
        if (showLabels) {
            RECT titleRect{r.left + S(12), r.bottom - S(30), r.right - S(10), r.bottom - S(6)};
            DrawTextAt(dc, g_fontTile, app.title, titleRect, RGB(255, 255, 255));
        }
    } else if (app.builtin == BuiltinKind::Clock) {
        DrawBuiltinClockTile(dc, r, color);
    } else {
        FillSolidRect(dc, r, color);
        // Subtle inner top highlight line for authentic Metro depth
        RECT topHighlight{r.left, r.top, r.right, r.top + S(1)};
        FillSolidRect(dc, topHighlight, LightenColor(color, 22));

        int tileW = r.right - r.left;
        int tileH = r.bottom - r.top;

        if (app.size == TileSize::Small) {
            int iconSize = std::clamp(std::min(tileW, tileH) - S(14), S(20), STile(40));
            int ix = r.left + (tileW - iconSize) / 2;
            int iy = r.top + (tileH - iconSize) / 2;
            DrawSmoothAppIcon(dc, app, ix, iy, iconSize);
        } else if (app.size == TileSize::Large) {
            int iconSize = std::clamp(STile(84), S(48), std::min(tileW, tileH) / 2);
            int ix = r.left + (tileW - iconSize) / 2;
            int iy = r.top + (tileH - iconSize) / 2 - (showLabels ? S(16) : 0);
            DrawSmoothAppIcon(dc, app, ix, iy, iconSize);

            if (showLabels) {
                RECT titleRect{r.left + S(14), r.bottom - S(36), r.right - S(14), r.bottom - S(8)};
                DrawTextAt(dc, g_fontTileLarge, app.title, titleRect, RGB(255, 255, 255));
            }
        } else if (app.size == TileSize::Wide) {
            int iconSize = std::clamp(STile(56), S(32), tileH - S(36));
            int ix = r.left + (tileW - iconSize) / 2;
            int iy = r.top + (tileH - iconSize) / 2 - (showLabels ? S(10) : 0);
            DrawSmoothAppIcon(dc, app, ix, iy, iconSize);

            if (showLabels) {
                RECT titleRect{r.left + S(12), r.bottom - S(30), r.right - S(12), r.bottom - S(6)};
                DrawTextAt(dc, g_fontTile, app.title, titleRect, RGB(255, 255, 255));
            }
        } else {
            int iconSize = std::clamp(STile(50), S(28), tileH - S(34));
            int ix = r.left + (tileW - iconSize) / 2;
            int iy = r.top + (tileH - iconSize) / 2 - (showLabels ? S(9) : 0);
            DrawSmoothAppIcon(dc, app, ix, iy, iconSize);

            if (showLabels) {
                RECT titleRect{r.left + S(10), r.bottom - S(28), r.right - S(8), r.bottom - S(5)};
                DrawTextAt(dc, g_fontTile, app.title, titleRect, RGB(255, 255, 255));
            }
        }
    }

    if (isSelected) {
        DrawRectOutline(dc, r, RGB(255, 255, 255), S(3));
    } else if (isHovered) {
        DrawRectOutline(dc, r, LightenColor(color, 70), S(2));
    }
}

void DrawAppListEntry(HDC dc, int index, bool isSelected, bool isHovered) {
    AppEntry& app = g_apps[static_cast<size_t>(index)];
    int animOffsetX = 0;
    float tileScale = 1.0f;
    GetGroupEntranceTransform(app.groupIdx, animOffsetX, tileScale);

    RECT r = app.rect;
    OffsetRect(&r, -g_scrollX + animOffsetX, 0);
    if (r.right <= r.left || r.bottom <= r.top) return;

    if (isSelected || isHovered) {
        FillSolidRect(dc, r, isSelected ? RGB(0, 120, 215) : RGB(45, 55, 65));
    }

    int boxSize = S(36);
    int boxX = r.left + S(4);
    int boxY = r.top + (r.bottom - r.top - boxSize) / 2;
    RECT iconBox{boxX, boxY, boxX + boxSize, boxY + boxSize};
    FillSolidRect(dc, iconBox, app.color);

    int iconSize = S(26);
    DrawSmoothAppIcon(dc, app, boxX + (boxSize - iconSize) / 2, boxY + (boxSize - iconSize) / 2, iconSize);

    RECT textRect{r.left + S(48), r.top, r.right - S(8), r.bottom};
    DrawTextAt(dc, g_fontList, app.title, textRect, RGB(255, 255, 255));

    if (isSelected) {
        DrawRectOutline(dc, r, RGB(255, 255, 255), S(2));
    }
}

void DrawMagnifierIcon(HDC dc, int cx, int cy, int rad, COLORREF color) {
    HPEN pen = CreatePen(PS_SOLID, std::max(1, S(2)), color);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(HOLLOW_BRUSH));
    Ellipse(dc, cx - rad, cy - rad, cx + rad, cy + rad);
    MoveToEx(dc, cx + rad - S(1), cy + rad - S(1), nullptr);
    LineTo(dc, cx + rad + S(5), cy + rad + S(5));
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(pen);
}

void DrawSearchControl(HDC dc, int width) {
    RECT sr = SearchBoxRect(width);
    if (g_settings.searchBarStyle == SearchBarStyle::AutoHide && !g_searchFocused && g_searchText.empty()) {
        if (g_hoverSearchBtn) {
            FillSolidRect(dc, sr, RGB(50, 56, 66));
        }
        int cx = (sr.left + sr.right) / 2 - S(1);
        int cy = (sr.top + sr.bottom) / 2 - S(1);
        DrawMagnifierIcon(dc, cx, cy, S(7), RGB(245, 245, 245));
        return;
    }

    bool darkStyle = (g_settings.searchBarStyle != SearchBarStyle::ClassicLight);
    COLORREF boxBg = darkStyle ? RGB(28, 32, 38) : RGB(255, 255, 255);
    COLORREF borderCol = g_searchFocused ? RGB(0, 164, 239) : (darkStyle ? RGB(82, 90, 102) : RGB(180, 180, 180));
    COLORREF textCol = darkStyle ? RGB(248, 248, 248) : RGB(20, 20, 20);
    COLORREF hintCol = darkStyle ? RGB(160, 168, 178) : RGB(115, 115, 115);

    FillSolidRect(dc, sr, boxBg);
    DrawRectOutline(dc, sr, borderCol, S(2));

    RECT textRect{sr.left + S(12), sr.top, sr.right - S(34), sr.bottom};
    if (g_searchText.empty()) {
        DrawTextAt(dc, g_fontList, g_searchFocused ? L"|" : L"Search", textRect, hintCol);
    } else {
        std::wstring display = g_searchText + (g_searchFocused ? L"|" : L"");
        DrawTextAt(dc, g_fontList, display, textRect, textCol);
    }

    RECT cr = SearchClearRect(width);
    if (!g_searchText.empty()) {
        DrawTextAt(dc, g_fontButton, L"X", cr, textCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    } else {
        int cx = (cr.left + cr.right) / 2 - S(1);
        int cy = (cr.top + cr.bottom) / 2 - S(1);
        DrawMagnifierIcon(dc, cx, cy, S(6), hintCol);
    }
}

// Draws ONLY the top-right user profile pill & power icon (dropdown menus are drawn LAST in PaintScreen)
void DrawTopRightHeaderControls(HDC dc, int width) {
    RECT pr = ProfilePillRect(width);
    if (g_profileMenu || g_hoverProfileBtn) {
        FillSolidRect(dc, pr, g_profileMenu ? RGB(0, 120, 215) : RGB(45, 52, 62));
    }

    wchar_t username[256]{};
    DWORD len = 256;
    if (!GetUserNameW(username, &len)) {
        wcscpy_s(username, L"User");
    }

    int avSize = S(32);
    RECT avRect{pr.right - avSize - S(4), pr.top + (pr.bottom - pr.top - avSize) / 2,
                pr.right - S(4), pr.top + (pr.bottom - pr.top + avSize) / 2};
    FillSolidRect(dc, avRect, RGB(0, 164, 239));
    wchar_t initial[2] = {username[0] ? static_cast<wchar_t>(std::towupper(username[0])) : L'U', L'\0'};
    DrawTextAt(dc, g_fontButton, initial, avRect, RGB(255, 255, 255), DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    RECT nameRect{pr.left + S(6), pr.top, avRect.left - S(10), pr.bottom};
    DrawTextAt(dc, g_fontList, username, nameRect, RGB(250, 250, 250), DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    // Top-right Windows 8.1 Power icon button next to the profile pill
    RECT pwr = TopPowerButtonRect(width);
    if (g_powerMenu || g_hoverPowerBtn) {
        FillSolidRect(dc, pwr, g_powerMenu ? RGB(0, 120, 215) : RGB(45, 52, 62));
    }
    int pcx = (pwr.left + pwr.right) / 2;
    int pcy = (pwr.top + pwr.bottom) / 2;
    int prad = S(8);
    HPEN pPen = CreatePen(PS_SOLID, std::max(1, S(2)), RGB(250, 250, 250));
    HGDIOBJ oldPen = SelectObject(dc, pPen);
    HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(HOLLOW_BRUSH));
    Ellipse(dc, pcx - prad, pcy - prad, pcx + prad, pcy + prad);
    MoveToEx(dc, pcx, pcy - prad - S(2), nullptr);
    LineTo(dc, pcx, pcy);
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(pPen);
}

// Drawn AFTER all tiles so the profile menu never hides behind any tile
void DrawProfileMenuOverlay(HDC dc, int width) {
    if (!g_profileMenu) return;
    RECT menu = ProfileMenuRect(width);
    RECT shadow{menu.left + S(4), menu.top + S(4), menu.right + S(4), menu.bottom + S(4)};
    FillSolidRect(dc, shadow, RGB(10, 10, 12));
    FillSolidRect(dc, menu, RGB(28, 28, 32));
    DrawRectOutline(dc, menu, RGB(255, 255, 255), S(2));

    const wchar_t* items[] = {
        L"Account settings",
        L"Open user folder",
        L"Lock PC",
        L"Sign out..."
    };
    int itemH = S(40);
    for (int i = 0; i < 4; ++i) {
        RECT row{menu.left + S(4), menu.top + S(5) + i * itemH, menu.right - S(4), menu.top + S(5) + (i + 1) * itemH};
        if (i == g_hoverProfileItem) {
            FillSolidRect(dc, row, RGB(0, 120, 215));
        }
        RECT txt{row.left + S(12), row.top, row.right - S(8), row.bottom};
        DrawTextAt(dc, g_fontButton, items[i], txt, RGB(255, 255, 255));
    }
}

// Drawn AFTER all tiles so the power menu never hides behind any tile
void DrawPowerMenuOverlay(HDC dc, int width, int height) {
    if (!g_powerMenu) return;
    RECT menu = PowerMenuRect(width, height);
    RECT shadow{menu.left + S(4), menu.top + S(4), menu.right + S(4), menu.bottom + S(4)};
    FillSolidRect(dc, shadow, RGB(10, 10, 12));
    FillSolidRect(dc, menu, RGB(28, 28, 32));
    DrawRectOutline(dc, menu, RGB(255, 255, 255), S(2));

    const wchar_t* items[] = {
        L"Lock",
        L"Sign out...",
        L"Sleep",
        L"Restart...",
        L"Shut down..."
    };
    int itemH = S(40);
    for (int i = 0; i < 5; ++i) {
        RECT row{menu.left + S(4), menu.top + S(5) + i * itemH, menu.right - S(4), menu.top + S(5) + (i + 1) * itemH};
        if (i == g_hoverPowerItem) {
            FillSolidRect(dc, row, RGB(0, 120, 215));
        }
        RECT txt{row.left + S(12), row.top, row.right - S(8), row.bottom};
        DrawTextAt(dc, g_fontButton, items[i], txt, RGB(255, 255, 255));
    }
}

// Draws the authentic Windows 8.1 circular All Apps arrow button at the bottom-left
void DrawAllAppsCircleButton(HDC dc, int width, int height) {
    RECT r = AllAppsButtonRect(width, height);
    int cx = (r.left + r.right) / 2;
    int cy = (r.top + r.bottom) / 2;
    int rad = (r.right - r.left) / 2;

    HPEN pen = CreatePen(PS_SOLID, std::max(1, S(2)), RGB(255, 255, 255));
    HBRUSH fillBrush = CreateSolidBrush((g_hoverAllAppsBtn || g_allAppsView) ? RGB(0, 120, 215) : RGB(28, 32, 38));
    HGDIOBJ oldPen = SelectObject(dc, pen);
    HGDIOBJ oldBrush = SelectObject(dc, (g_hoverAllAppsBtn || g_allAppsView) ? fillBrush : GetStockObject(HOLLOW_BRUSH));
    Ellipse(dc, cx - rad, cy - rad, cx + rad, cy + rad);

    int aw = S(6);
    if (g_allAppsView) {
        // Up arrow (return to Start tiles)
        MoveToEx(dc, cx, cy + aw, nullptr);
        LineTo(dc, cx, cy - aw);
        MoveToEx(dc, cx - aw, cy - S(1), nullptr);
        LineTo(dc, cx, cy - aw);
        LineTo(dc, cx + aw, cy - S(1));
    } else {
        // Down arrow (open All Apps)
        MoveToEx(dc, cx, cy - aw, nullptr);
        LineTo(dc, cx, cy + aw);
        MoveToEx(dc, cx - aw, cy + S(1), nullptr);
        LineTo(dc, cx, cy + aw);
        LineTo(dc, cx + aw, cy + S(1));
    }

    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(fillBrush);
    DeleteObject(pen);
}

void DrawAlphaJumpOverlay(HDC dc, int width, int height) {
    if (!g_alphaJumpView) return;
    int cols = 7;
    int rows = 4;
    int cellW = S(64);
    int cellH = S(64);
    int gap = S(8);
    int totalW = cols * cellW + (cols - 1) * gap;
    int totalH = rows * cellH + (rows - 1) * gap;
    int startX = (width - totalW) / 2;
    int startY = (height - totalH) / 2;

    RECT backdrop{startX - S(24), startY - S(56), startX + totalW + S(24), startY + totalH + S(24)};
    FillSolidRect(dc, backdrop, RGB(20, 20, 24));
    DrawRectOutline(dc, backdrop, RGB(255, 255, 255), S(2));

    RECT header{backdrop.left + S(16), backdrop.top + S(10), backdrop.right - S(16), startY - S(8)};
    DrawTextAt(dc, g_fontGroup, L"Select a letter", header, RGB(255, 255, 255));

    std::unordered_set<wchar_t> activeBuckets;
    for (const auto& ah : g_alphaHeaders) activeBuckets.insert(ah.letter);

    for (int i = 0; i < 28; ++i) {
        wchar_t ch = (i == 0) ? L'#' : (i <= 26 ? static_cast<wchar_t>(L'A' + i - 1) : L'@');
        int r = i / cols;
        int c = i % cols;
        int x = startX + c * (cellW + gap);
        int y = startY + r * (cellH + gap);
        RECT cell{x, y, x + cellW, y + cellH};
        g_alphaJumpRects[i] = cell;

        bool hasApps = activeBuckets.count(ch) > 0;
        FillSolidRect(dc, cell, hasApps ? RGB(0, 120, 215) : RGB(45, 45, 45));
        wchar_t label[4]{ch, L'\0'};
        if (ch == L'@') wcscpy_s(label, L"INT");
        DrawTextAt(dc, g_fontGroup, label, cell,
                   hasApps ? RGB(255, 255, 255) : RGB(110, 110, 110),
                   DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

void DrawRenameGroupBanner(HDC dc, int width) {
    if (!g_renamingGroup) return;
    RECT banner{S(56), S(100), std::min(width - S(56), S(620)), S(142)};
    FillSolidRect(dc, banner, RGB(24, 24, 28));
    DrawRectOutline(dc, banner, RGB(0, 164, 239), S(2));
    std::wstring prompt = L"Rename group: " + g_renameBuffer + L"|   (Enter = Save, Esc = Cancel)";
    RECT txt{banner.left + S(12), banner.top, banner.right - S(12), banner.bottom};
    DrawTextAt(dc, g_fontList, prompt, txt, RGB(255, 255, 255));
}

void PaintScreen(HWND hwnd) {
    PAINTSTRUCT ps{};
    HDC target = BeginPaint(hwnd, &ps);
    RECT client{};
    GetClientRect(hwnd, &client);
    int width = std::max(1, static_cast<int>(client.right));
    int height = std::max(1, static_cast<int>(client.bottom));

    HDC dc = CreateCompatibleDC(target);
    HBITMAP bitmap = CreateCompatibleBitmap(target, width, height);
    HGDIOBJ oldBitmap = SelectObject(dc, bitmap);

    DrawBackgroundCanvas(dc, width, height);

    int headerOffsetX = 0;
    float dummyScale = 1.0f;
    GetGroupEntranceTransform(0, headerOffsetX, dummyScale);

    // Clean Windows 8.1 "Start" or "Apps" title at top-left (no cluttered subtitle text!)
    RECT titleRect{S(56) + headerOffsetX / 3, S(24), S(340) + headerOffsetX / 3, S(86)};
    DrawTextAt(dc, g_fontTitle, g_allAppsView ? L"Apps" : L"Start", titleRect, RGB(255, 255, 255));

    DrawSearchControl(dc, width);
    DrawTopRightHeaderControls(dc, width);

    bool isSearchActive = !Trim(g_searchText).empty();
    bool useListLayout = g_allAppsView || isSearchActive;

    // Group headers (clean, without " v" or underscores)
    if (!useListLayout && GroupHeadersEnabled()) {
        for (size_t i = 0; i < g_headers.size(); ++i) {
            int grpAnimX = 0;
            float grpScale = 1.0f;
            GetGroupEntranceTransform(g_headers[i].groupIdx, grpAnimX, grpScale);

            RECT hr = g_headers[i].headerRect;
            OffsetRect(&hr, -g_scrollX + grpAnimX, 0);
            if (hr.right < 0 || hr.left > width) continue;
            COLORREF hColor = (static_cast<int>(i) == g_hoverHeader) ? RGB(0, 204, 255) : RGB(242, 242, 245);
            DrawTextAt(dc, g_fontGroup, g_headers[i].displayTitle, hr, hColor);
        }
    } else if (useListLayout && !isSearchActive) {
        for (const auto& ah : g_alphaHeaders) {
            int grpAnimX = 0;
            float grpScale = 1.0f;
            GetGroupEntranceTransform(0, grpAnimX, grpScale);

            RECT ar = ah.rect;
            OffsetRect(&ar, -g_scrollX + grpAnimX, 0);
            if (ar.right < 0 || ar.left > width) continue;
            RECT badge{ar.left, ar.top + S(2), ar.left + S(38), ar.bottom - S(2)};
            FillSolidRect(dc, badge, RGB(0, 120, 215));
            wchar_t lbl[4]{ah.letter, L'\0'};
            if (ah.letter == L'@') wcscpy_s(lbl, L"INT");
            DrawTextAt(dc, g_fontGroup, lbl, badge, RGB(255, 255, 255), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
    }

    DrawRenameGroupBanner(dc, width);

    // Draw all visible tiles / app list entries
    for (int index : g_visibleApps) {
        const RECT& r = g_apps[static_cast<size_t>(index)].rect;
        if (r.right <= r.left || r.bottom <= r.top) continue;
        if (r.right - g_scrollX < -S(300) || r.left - g_scrollX > width + S(300)) continue;

        bool selected = (index == g_selectedApp);
        bool hovered = (index == g_hoverApp);
        if (useListLayout) {
            DrawAppListEntry(dc, index, selected, hovered);
        } else {
            DrawTile(dc, index, selected, hovered);
        }
    }

    if (g_isDraggingTile && g_pressedTileIndex >= 0 && static_cast<size_t>(g_pressedTileIndex) < g_apps.size()) {
        AppEntry& dragged = g_apps[static_cast<size_t>(g_pressedTileIndex)];
        int w = dragged.rect.right - dragged.rect.left;
        int h = dragged.rect.bottom - dragged.rect.top;
        RECT ghost{g_dragCurrentPt.x - w / 2, g_dragCurrentPt.y - h / 2,
                   g_dragCurrentPt.x + w / 2, g_dragCurrentPt.y + h / 2};
        FillSolidRect(dc, ghost, LightenColor(dragged.color, 35));
        int iconSz = std::min(STile(48), std::min(w, h) - S(16));
        DrawSmoothAppIcon(dc, dragged, ghost.left + (w - iconSz) / 2, ghost.top + (h - iconSz) / 2 - S(6), iconSz);
        DrawRectOutline(dc, ghost, RGB(255, 255, 255), S(3));
        if (LabelsEnabled()) {
            RECT gt{ghost.left + S(10), ghost.bottom - S(30), ghost.right - S(10), ghost.bottom - S(6)};
            DrawTextAt(dc, g_fontTile, dragged.title, gt, RGB(255, 255, 255));
        }
    }

    if (g_visibleApps.empty()) {
        RECT emptyRect{S(56), S(180), width - S(56), S(240)};
        DrawTextAt(dc, g_fontGroup,
                   isSearchActive ? L"No apps match your search."
                                  : L"No pinned tiles. Right-click to restore.",
                   emptyRect, RGB(230, 230, 230));
    }

    // Clean bottom-left circular All Apps toggle button (no bottom helper text clutter!)
    DrawAllAppsCircleButton(dc, width, height);

    if (g_contentWidth > width - S(56)) {
        RECT track = ScrollbarTrackRect(width, height);
        FillSolidRect(dc, track, RGB(42, 42, 46));
        RECT thumb = ScrollbarThumbRect(width, height);
        FillSolidRect(dc, thumb, g_draggingScrollbar ? RGB(255, 255, 255) : RGB(180, 180, 185));
    }

    // Topmost Z-Order overlays: Profile Menu, Power Menu, and Alphabetical Jump Grid
    DrawProfileMenuOverlay(dc, width);
    DrawPowerMenuOverlay(dc, width, height);
    DrawAlphaJumpOverlay(dc, width, height);

    BitBlt(target, 0, 0, width, height, dc, 0, 0, SRCCOPY);
    SelectObject(dc, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(dc);
    EndPaint(hwnd, &ps);
}

// High-Clarity ClearType Natural Font Creation
HFONT MakeCrispFont(int pixelHeight, int weight, const wchar_t* primaryFace, const wchar_t* fallbackFace) {
    HFONT f = CreateFontW(
        -pixelHeight, 0, 0, 0, weight, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_NATURAL_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, primaryFace);
    if (!f && fallbackFace) {
        f = CreateFontW(
            -pixelHeight, 0, 0, 0, weight, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_NATURAL_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE, fallbackFace);
    }
    return f;
}

void CreateFonts(HWND hwnd) {
    if (!g_dpi) g_dpi = GetDpiForWindow(hwnd);
    if (!g_dpi) g_dpi = 96;
    auto delFont = [](HFONT& f) { if (f) { DeleteObject(f); f = nullptr; } };
    delFont(g_fontTitle);
    delFont(g_fontSubtitle);
    delFont(g_fontGroup);
    delFont(g_fontTile);
    delFont(g_fontTileLarge);
    delFont(g_fontClockBig);
    delFont(g_fontList);
    delFont(g_fontButton);

    const wchar_t* displayFace = L"Segoe UI Variable Display";
    const wchar_t* textFace    = L"Segoe UI Variable Text";
    int headingWeight = 320;
    int bodyWeight    = FW_MEDIUM;

    switch (g_settings.fontChoice) {
        case FontChoice::SegoeSemibold:
            displayFace = L"Segoe UI Semibold";
            textFace = L"Segoe UI Semibold";
            headingWeight = FW_NORMAL;
            bodyWeight = FW_SEMIBOLD;
            break;
        case FontChoice::SegoeClassic:
            displayFace = L"Segoe UI";
            textFace = L"Segoe UI";
            headingWeight = 320;
            bodyWeight = FW_NORMAL;
            break;
        case FontChoice::Bahnschrift:
            displayFace = L"Bahnschrift";
            textFace = L"Bahnschrift";
            headingWeight = FW_NORMAL;
            bodyWeight = FW_MEDIUM;
            break;
        case FontChoice::Calibri:
            displayFace = L"Calibri";
            textFace = L"Calibri";
            headingWeight = FW_NORMAL;
            bodyWeight = FW_BOLD;
            break;
        case FontChoice::SegoeVariable:
        default:
            displayFace = L"Segoe UI Variable Display";
            textFace = L"Segoe UI Variable Text";
            headingWeight = 320;
            bodyWeight = FW_MEDIUM;
            break;
    }

    g_fontTitle     = MakeCrispFont(S(46), headingWeight, displayFace, L"Segoe UI Light");
    g_fontSubtitle  = MakeCrispFont(S(14), FW_NORMAL,     textFace,    L"Segoe UI");
    g_fontGroup     = MakeCrispFont(S(21), headingWeight, displayFace, L"Segoe UI");
    g_fontTile      = MakeCrispFont(S(13), bodyWeight,    textFace,    L"Segoe UI");
    g_fontTileLarge = MakeCrispFont(S(18), FW_SEMIBOLD,   displayFace, L"Segoe UI Semibold");
    g_fontClockBig  = MakeCrispFont(S(42), headingWeight, displayFace, L"Segoe UI");
    g_fontList      = MakeCrispFont(S(15), bodyWeight,    textFace,    L"Segoe UI");
    g_fontButton    = MakeCrispFont(S(14), FW_SEMIBOLD,   textFace,    L"Segoe UI Semibold");
}

////////////////////////////////////////////////////////////////////////////////
// Window Show / Hide & Actions

void HideScreen(bool restoreFocus = true) {
    HWND hwnd = g_window.load();
    if (!hwnd) return;
    StopAnimationLoop();
    KillTimer(hwnd, kTimerClock);
    g_powerMenu = false;
    g_profileMenu = false;
    g_alphaJumpView = false;
    g_renamingGroup = false;
    g_isDraggingTile = false;
    g_mouseDownOnTile = false;
    g_draggingScrollbar = false;
    if (GetCapture() == hwnd) ReleaseCapture();
    ShowWindow(hwnd, SW_HIDE);
    g_lastHideTick = GetTickCount64();

    if (restoreFocus && g_prevForegroundWindow && IsWindow(g_prevForegroundWindow)) {
        SetForegroundWindow(g_prevForegroundWindow);
    }
    g_prevForegroundWindow = nullptr;
    SetProcessWorkingSetSize(GetCurrentProcess(), static_cast<SIZE_T>(-1), static_cast<SIZE_T>(-1));
}

void ShowScreen() {
    HWND hwnd = g_window.load();
    if (!hwnd) return;
    if (IsWindowVisible(hwnd)) {
        HideScreen(true);
        return;
    }

    HWND fg = GetForegroundWindow();
    if (fg && fg != hwnd) {
        g_prevForegroundWindow = fg;
    }

    POINT point{};
    GetCursorPos(&point);
    HMONITOR monitor = MonitorFromPoint(point, MONITOR_DEFAULTTOPRIMARY);
    UINT dpiX = 96, dpiY = 96;
    if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) && dpiX != g_dpi) {
        g_dpi = dpiX;
        CreateFonts(hwnd);
        FreeAppCachedBitmaps(g_apps);
    }

    g_powerMenu = false;
    g_profileMenu = false;
    g_alphaJumpView = false;
    g_renamingGroup = false;
    g_searchFocused = false;

    if (AnimationsEnabled()) {
        g_openAnimProgress = 0.0f;
        UpdateWindowLayerAlpha();
    } else {
        g_openAnimProgress = 1.0f;
        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    }

    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (GetMonitorInfoW(monitor, &info)) {
        SetWindowPos(hwnd, HWND_TOPMOST,
                     info.rcMonitor.left, info.rcMonitor.top,
                     info.rcMonitor.right - info.rcMonitor.left,
                     info.rcMonitor.bottom - info.rcMonitor.top,
                     SWP_SHOWWINDOW);
    } else {
        ShowWindow(hwnd, SW_SHOW);
    }

    INPUT input{};
    SendInput(1, &input, sizeof(INPUT));
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    SetTimer(hwnd, kTimerClock, 1000, nullptr);
    RebuildLayout(false);
    if (AnimationsEnabled()) {
        StartAnimationLoop();
    }
}

void LaunchApp(int index, bool asAdmin = false) {
    if (index < 0 || static_cast<size_t>(index) >= g_apps.size()) return;
    const auto& app = g_apps[static_cast<size_t>(index)];

    if (app.builtin == BuiltinKind::Desktop) {
        HideScreen(false);
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (tray) PostMessageW(tray, WM_COMMAND, 419, 0);
        return;
    }

    SHELLEXECUTEINFOW sei{};
    sei.cbSize = sizeof(sei);
    sei.hwnd = g_window.load();
    sei.fMask = SEE_MASK_ASYNCOK | SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = asAdmin ? L"runas" : L"open";
    sei.lpFile = app.launchTarget.c_str();
    sei.nShow = SW_SHOWNORMAL;

    std::wstring workDir;
    if (!app.fileLocation.empty()) {
        size_t slash = app.fileLocation.find_last_of(L"\\/");
        if (slash != std::wstring::npos) {
            workDir = app.fileLocation.substr(0, slash);
            sei.lpDirectory = workDir.c_str();
        }
    }

    AllowSetForegroundWindow(ASFW_ANY);
    if (ShellExecuteExW(&sei)) {
        HideScreen(false);
    } else {
        DWORD err = GetLastError();
        if (err != ERROR_CANCELLED) {
            MessageBoxW(g_window.load(), L"Windows could not launch this application. The shortcut or package may have been moved or removed.",
                        app.title.c_str(), MB_OK | MB_ICONWARNING);
        }
    }
}

void OpenAppFileLocation(int index) {
    if (index < 0 || static_cast<size_t>(index) >= g_apps.size()) return;
    const auto& app = g_apps[static_cast<size_t>(index)];
    std::wstring target = !app.fileLocation.empty() ? app.fileLocation : app.launchTarget;
    if (target.empty() || target.rfind(L"shell:", 0) == 0 || target.rfind(L"builtin:", 0) == 0) {
        MessageBoxW(g_window.load(), L"This is a built-in or Microsoft Store packaged application without a traditional file shortcut.",
                    app.title.c_str(), MB_OK | MB_ICONINFORMATION);
        return;
    }
    std::wstring param = L"/select,\"" + target + L"\"";
    HideScreen(false);
    ShellExecuteW(nullptr, L"open", L"explorer.exe", param.c_str(), nullptr, SW_SHOWNORMAL);
}

bool EnableShutdownPrivilege() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) return false;
    TOKEN_PRIVILEGES privileges{};
    privileges.PrivilegeCount = 1;
    if (!LookupPrivilegeValueW(nullptr, SE_SHUTDOWN_NAME, &privileges.Privileges[0].Luid)) {
        CloseHandle(token);
        return false;
    }
    privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    SetLastError(ERROR_SUCCESS);
    BOOL adjusted = AdjustTokenPrivileges(token, FALSE, &privileges, sizeof(privileges), nullptr, nullptr);
    DWORD error = GetLastError();
    CloseHandle(token);
    return adjusted && error == ERROR_SUCCESS;
}

void RunPowerAction(int action) {
    HWND hwnd = g_window.load();
    g_powerMenu = false;
    if (hwnd) InvalidateRect(hwnd, nullptr, FALSE);

    if (action == 0) {
        HideScreen(false);
        LockWorkStation();
        return;
    }
    if (action == 2) {
        HideScreen(false);
        if (!SetSuspendState(FALSE, FALSE, FALSE)) {
            MessageBoxW(nullptr, L"Windows could not enter sleep mode.", L"Power", MB_OK | MB_ICONWARNING);
        }
        return;
    }

    const wchar_t* promptText = L"Are you sure you want to sign out of Windows?";
    const wchar_t* titleText  = L"Confirm Sign Out";
    UINT flags = EWX_LOGOFF;

    if (action == 3) {
        promptText = L"Are you sure you want to restart your computer now?";
        titleText  = L"Confirm Restart";
        flags = EWX_REBOOT;
    } else if (action == 4) {
        promptText = L"Are you sure you want to shut down your computer now?";
        titleText  = L"Confirm Shut Down";
        flags = EWX_SHUTDOWN;
    }

    int resp = MessageBoxW(hwnd, promptText, titleText, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2 | MB_SETFOREGROUND);
    if (resp != IDYES) {
        if (hwnd) SetFocus(hwnd);
        return;
    }

    HideScreen(false);
    if (action == 3 || action == 4) {
        if (!EnableShutdownPrivilege()) {
            MessageBoxW(nullptr, L"Windows did not grant the required power-management privilege.", L"Power", MB_OK | MB_ICONWARNING);
            return;
        }
    }
    if (!ExitWindowsEx(flags, SHTDN_REASON_MAJOR_APPLICATION)) {
        MessageBoxW(nullptr, L"Windows could not complete the requested power action.", L"Power", MB_OK | MB_ICONWARNING);
    }
}

void RunProfileAction(int action) {
    HWND hwnd = g_window.load();
    g_profileMenu = false;
    if (hwnd) InvalidateRect(hwnd, nullptr, FALSE);
    if (action == 0) {
        HideScreen(false);
        ShellExecuteW(nullptr, L"open", L"ms-settings:yourinfo", nullptr, nullptr, SW_SHOWNORMAL);
    } else if (action == 1) {
        wchar_t userPath[MAX_PATH]{};
        DWORD len = GetEnvironmentVariableW(L"USERPROFILE", userPath, MAX_PATH);
        if (len > 0 && len < MAX_PATH) {
            HideScreen(false);
            ShellExecuteW(nullptr, L"open", userPath, nullptr, nullptr, SW_SHOWNORMAL);
        }
    } else if (action == 2) {
        RunPowerAction(0);
    } else if (action == 3) {
        RunPowerAction(1);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Context Menus (Tile Menu & Canvas Background/Animation/View Menu)

void ShowCanvasContextMenu(HWND hwnd, POINT screenPt) {
    HMENU menu = CreatePopupMenu();
    if (!menu) return;

    ThemeScheme curTheme = ActiveTheme();
    HMENU bgMenu = CreatePopupMenu();
    AppendMenuW(bgMenu, MF_STRING | (curTheme == ThemeScheme::Wallpaper   ? MF_CHECKED : 0), CMD_BG_WALLPAPER, L"Current Desktop Wallpaper");
    AppendMenuW(bgMenu, MF_STRING | (curTheme == ThemeScheme::Dark        ? MF_CHECKED : 0), CMD_BG_DARK,      L"Windows 8.1 Charcoal Dark");
    AppendMenuW(bgMenu, MF_STRING | (curTheme == ThemeScheme::Emerald     ? MF_CHECKED : 0), CMD_BG_EMERALD,   L"Windows 8 Emerald Green");
    AppendMenuW(bgMenu, MF_STRING | (curTheme == ThemeScheme::Indigo      ? MF_CHECKED : 0), CMD_BG_INDIGO,    L"Windows 8 Deep Indigo");
    AppendMenuW(bgMenu, MF_STRING | (curTheme == ThemeScheme::Teal        ? MF_CHECKED : 0), CMD_BG_TEAL,      L"Windows 8 Slate Teal");
    AppendMenuW(bgMenu, MF_STRING | (curTheme == ThemeScheme::Crimson     ? MF_CHECKED : 0), CMD_BG_CRIMSON,   L"Metro Burgundy Crimson");
    AppendMenuW(bgMenu, MF_STRING | (curTheme == ThemeScheme::Cobalt      ? MF_CHECKED : 0), CMD_BG_COBALT,    L"Metro Cobalt Night");
    AppendMenuW(bgMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(bgMenu, MF_STRING | (curTheme == ThemeScheme::CustomColor ? MF_CHECKED : 0), CMD_BG_CUSTOM_C,  L"Custom Solid Color (from Settings)");

    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(bgMenu), L"Background Theme");
    AppendMenuW(menu, MF_STRING | (BlurEnabled()         ? MF_CHECKED : 0), CMD_TOGGLE_BLUR,    L"Blur Wallpaper / Image Background");
    AppendMenuW(menu, MF_STRING | (MotifEnabled()        ? MF_CHECKED : 0), CMD_TOGGLE_MOTIF,   L"Show Geometric Background Motif");
    AppendMenuW(menu, MF_STRING | (AnimationsEnabled()   ? MF_CHECKED : 0), CMD_TOGGLE_ANIM,    L"Enable Smooth Animations");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING | (LabelsEnabled()       ? MF_CHECKED : 0), CMD_TOGGLE_LABELS,  L"Show App Names on Tiles");
    AppendMenuW(menu, MF_STRING | (GroupHeadersEnabled() ? MF_CHECKED : 0), CMD_TOGGLE_HEADERS, L"Show Group Header Titles");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, CMD_REFRESH_APPS, L"Refresh Installed Applications");
    AppendMenuW(menu, MF_STRING, CMD_RESTORE_ALL,  L"Restore All Unpinned Tiles");
    AppendMenuW(menu, MF_STRING, CMD_RESET_LAYOUT, L"Reset Layout to Default");

    UINT cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY,
                              screenPt.x, screenPt.y, 0, hwnd, nullptr);
    DestroyMenu(menu);

    if (cmd == CMD_NONE) return;
    if (cmd >= CMD_BG_EMERALD && cmd <= CMD_BG_WALLPAPER) {
        g_runtimeThemeOverride = static_cast<int>(cmd - CMD_BG_EMERALD);
        FreeBackgroundBitmap();
        SaveCustomizations();
        InvalidateRect(hwnd, nullptr, FALSE);
    } else if (cmd == CMD_BG_CUSTOM_C) {
        g_runtimeThemeOverride = static_cast<int>(ThemeScheme::CustomColor);
        FreeBackgroundBitmap();
        SaveCustomizations();
        InvalidateRect(hwnd, nullptr, FALSE);
    } else if (cmd == CMD_TOGGLE_BLUR) {
        g_runtimeBlurOverride = BlurEnabled() ? 0 : 1;
        FreeBackgroundBitmap();
        SaveCustomizations();
        InvalidateRect(hwnd, nullptr, FALSE);
    } else if (cmd == CMD_TOGGLE_MOTIF) {
        g_runtimeMotifOverride = MotifEnabled() ? 0 : 1;
        SaveCustomizations();
        InvalidateRect(hwnd, nullptr, FALSE);
    } else if (cmd == CMD_TOGGLE_ANIM) {
        g_runtimeAnimOverride = AnimationsEnabled() ? 0 : 1;
        if (!AnimationsEnabled()) StopAnimationLoop();
        UpdateWindowLayerAlpha();
        SaveCustomizations();
        InvalidateRect(hwnd, nullptr, FALSE);
    } else if (cmd == CMD_TOGGLE_LABELS) {
        g_runtimeLabelsOverride = LabelsEnabled() ? 0 : 1;
        SaveCustomizations();
        InvalidateRect(hwnd, nullptr, FALSE);
    } else if (cmd == CMD_TOGGLE_HEADERS) {
        g_runtimeHeadersOverride = GroupHeadersEnabled() ? 0 : 1;
        SaveCustomizations();
        RebuildLayout(false);
    } else if (cmd == CMD_REFRESH_APPS) {
        StartAsyncAppDiscovery();
    } else if (cmd == CMD_RESTORE_ALL) {
        for (auto& a : g_apps) {
            a.pinned = true;
            SyncAppToCustomization(a);
        }
        SaveCustomizations();
        RebuildLayout(false);
    } else if (cmd == CMD_RESET_LAYOUT) {
        if (MessageBoxW(hwnd, L"Reset all tile sizes, colors, groups, and positions back to default?",
                        L"Reset Layout", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) {
            g_customTiles.clear();
            g_groupRenames.clear();
            g_runtimeThemeOverride = -1;
            g_runtimeAnimOverride = -1;
            g_runtimeMotifOverride = -1;
            g_runtimeBlurOverride = -1;
            g_runtimeLabelsOverride = -1;
            g_runtimeHeadersOverride = -1;
            g_nextOrderCounter = 1;
            for (auto& a : g_apps) {
                a.group = a.defaultGroup;
                a.colorIndex = -1;
                a.pinned = true;
            }
            ApplyCustomizationsToApps(g_apps);
            SaveCustomizations();
            RebuildLayout(false);
        }
    }
}

void ShowTileContextMenu(HWND hwnd, int appIndex, POINT screenPt) {
    if (appIndex < 0 || static_cast<size_t>(appIndex) >= g_apps.size()) return;
    AppEntry& app = g_apps[static_cast<size_t>(appIndex)];

    HMENU menu = CreatePopupMenu();
    if (!menu) return;

    AppendMenuW(menu, MF_STRING, CMD_LAUNCH, L"Open / Launch");
    if (app.builtin == BuiltinKind::None) {
        AppendMenuW(menu, MF_STRING, CMD_RUN_ADMIN, L"Run as administrator");
        AppendMenuW(menu, MF_STRING, CMD_OPEN_LOCATION, L"Open file location");
    }
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);

    HMENU sizeMenu = CreatePopupMenu();
    AppendMenuW(sizeMenu, MF_STRING | (app.size == TileSize::Small  ? MF_CHECKED : 0), CMD_SIZE_SMALL,  L"Small (1x1)");
    AppendMenuW(sizeMenu, MF_STRING | (app.size == TileSize::Medium ? MF_CHECKED : 0), CMD_SIZE_MEDIUM, L"Medium (2x2)");
    AppendMenuW(sizeMenu, MF_STRING | (app.size == TileSize::Wide   ? MF_CHECKED : 0), CMD_SIZE_WIDE,   L"Wide (4x2)");
    AppendMenuW(sizeMenu, MF_STRING | (app.size == TileSize::Large  ? MF_CHECKED : 0), CMD_SIZE_LARGE,  L"Large (4x4)");
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(sizeMenu), L"Resize tile");

    HMENU colorMenu = CreatePopupMenu();
    AppendMenuW(colorMenu, MF_STRING | (app.colorIndex < 0 ? MF_CHECKED : 0), CMD_COLOR_AUTO, L"Default (Automatic)");
    AppendMenuW(colorMenu, MF_SEPARATOR, 0, nullptr);
    for (int c = 0; c < kPaletteSize; ++c) {
        AppendMenuW(colorMenu, MF_STRING | (app.colorIndex == c ? MF_CHECKED : 0), CMD_COLOR_BASE + c, kPaletteNames[c]);
    }
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(colorMenu), L"Tile color");

    std::vector<std::wstring> groups;
    for (const auto& h : g_headers) {
        if (std::find(groups.begin(), groups.end(), h.rawGroup) == groups.end()) {
            groups.push_back(h.rawGroup);
        }
    }
    for (const auto& a : g_apps) {
        if (!a.group.empty() && std::find(groups.begin(), groups.end(), a.group) == groups.end()) {
            groups.push_back(a.group);
        }
    }

    HMENU groupMenu = CreatePopupMenu();
    for (size_t g = 0; g < groups.size() && g < 90; ++g) {
        std::wstring label = GetGroupDisplayName(groups[g]);
        UINT flags = MF_STRING | (app.group == groups[g] ? MF_CHECKED : 0);
        AppendMenuW(groupMenu, flags, CMD_GROUP_BASE + static_cast<UINT>(g), label.c_str());
    }
    AppendMenuW(groupMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(groupMenu, MF_STRING, CMD_NEW_GROUP, L"Create new group...");
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(groupMenu), L"Move to group");

    AppendMenuW(menu, MF_STRING, CMD_RENAME_GROUP, L"Rename current group...");
    AppendMenuW(menu, MF_STRING, CMD_MOVE_EARLIER, L"Move tile earlier");
    AppendMenuW(menu, MF_STRING, CMD_MOVE_LATER, L"Move tile later");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, CMD_UNPIN, app.pinned ? L"Unpin from Start" : L"Pin to Start");
    AppendMenuW(menu, MF_STRING, CMD_RESTORE_ALL, L"Restore all unpinned tiles");
    AppendMenuW(menu, MF_STRING, CMD_RESET_LAYOUT, L"Reset entire layout to default");

    UINT cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON | TPM_NONOTIFY,
                              screenPt.x, screenPt.y, 0, hwnd, nullptr);
    DestroyMenu(menu);

    if (cmd == CMD_NONE) return;
    if (cmd == CMD_LAUNCH) { LaunchApp(appIndex, false); return; }
    if (cmd == CMD_RUN_ADMIN) { LaunchApp(appIndex, true); return; }
    if (cmd == CMD_OPEN_LOCATION) { OpenAppFileLocation(appIndex); return; }

    bool changed = false;
    if (cmd >= CMD_SIZE_SMALL && cmd <= CMD_SIZE_LARGE) {
        app.size = static_cast<TileSize>(cmd - CMD_SIZE_SMALL);
        SyncAppToCustomization(app);
        changed = true;
    } else if (cmd == CMD_COLOR_AUTO) {
        app.colorIndex = -1;
        app.color = kPalette[StableColorIndex(app.id)];
        SyncAppToCustomization(app);
        changed = true;
    } else if (cmd >= CMD_COLOR_BASE && cmd < CMD_COLOR_BASE + kPaletteSize) {
        app.colorIndex = static_cast<int>(cmd - CMD_COLOR_BASE);
        app.color = kPalette[app.colorIndex];
        SyncAppToCustomization(app);
        changed = true;
    } else if (cmd >= CMD_GROUP_BASE && cmd < CMD_GROUP_BASE + groups.size()) {
        app.group = groups[cmd - CMD_GROUP_BASE];
        app.searchGroup = Lower(app.group);
        app.order = g_nextOrderCounter++;
        SyncAppToCustomization(app);
        changed = true;
    } else if (cmd == CMD_NEW_GROUP) {
        int customNum = 1;
        std::wstring newGroup;
        do {
            newGroup = L"Group " + std::to_wstring(customNum++);
        } while (std::find(groups.begin(), groups.end(), newGroup) != groups.end());
        app.group = newGroup;
        app.searchGroup = Lower(app.group);
        app.order = g_nextOrderCounter++;
        SyncAppToCustomization(app);
        g_renamingGroup = true;
        g_renameGroupTarget = newGroup;
        g_renameBuffer = newGroup;
        changed = true;
    } else if (cmd == CMD_RENAME_GROUP) {
        g_renamingGroup = true;
        g_renameGroupTarget = app.group;
        g_renameBuffer = GetGroupDisplayName(app.group);
        InvalidateRect(hwnd, nullptr, FALSE);
        return;
    } else if (cmd == CMD_MOVE_EARLIER || cmd == CMD_MOVE_LATER) {
        auto it = std::find(g_visibleApps.begin(), g_visibleApps.end(), appIndex);
        if (it != g_visibleApps.end()) {
            if (cmd == CMD_MOVE_EARLIER && it != g_visibleApps.begin()) {
                int prevIdx = *(it - 1);
                if (g_apps[prevIdx].group == app.group) {
                    std::swap(app.order, g_apps[prevIdx].order);
                    if (app.order == g_apps[prevIdx].order) --app.order;
                    SyncAppToCustomization(app);
                    SyncAppToCustomization(g_apps[prevIdx]);
                    changed = true;
                }
            } else if (cmd == CMD_MOVE_LATER && (it + 1) != g_visibleApps.end()) {
                int nextIdx = *(it + 1);
                if (g_apps[nextIdx].group == app.group) {
                    std::swap(app.order, g_apps[nextIdx].order);
                    if (app.order == g_apps[nextIdx].order) ++app.order;
                    SyncAppToCustomization(app);
                    SyncAppToCustomization(g_apps[nextIdx]);
                    changed = true;
                }
            }
        }
    } else if (cmd == CMD_UNPIN) {
        app.pinned = !app.pinned;
        SyncAppToCustomization(app);
        changed = true;
    } else if (cmd == CMD_RESTORE_ALL) {
        for (auto& a : g_apps) {
            a.pinned = true;
            SyncAppToCustomization(a);
        }
        changed = true;
    } else if (cmd == CMD_RESET_LAYOUT) {
        if (MessageBoxW(hwnd, L"Reset all tile sizes, colors, groups, and positions back to default?",
                        L"Reset Layout", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) {
            g_customTiles.clear();
            g_groupRenames.clear();
            g_nextOrderCounter = 1;
            for (auto& a : g_apps) {
                a.group = a.defaultGroup;
                a.colorIndex = -1;
                a.pinned = true;
            }
            ApplyCustomizationsToApps(g_apps);
            changed = true;
        }
    }

    if (changed) {
        SaveCustomizations();
        RebuildLayout(false);
    }
}

void CompleteTileDragDrop(POINT clientPt) {
    if (g_pressedTileIndex < 0 || static_cast<size_t>(g_pressedTileIndex) >= g_apps.size()) return;
    POINT worldPt{clientPt.x + g_scrollX, clientPt.y};
    AppEntry& dragged = g_apps[static_cast<size_t>(g_pressedTileIndex)];

    for (int targetIdx : g_visibleApps) {
        if (targetIdx == g_pressedTileIndex) continue;
        if (PointInRect(g_apps[static_cast<size_t>(targetIdx)].rect, worldPt)) {
            AppEntry& target = g_apps[static_cast<size_t>(targetIdx)];
            dragged.group = target.group;
            dragged.searchGroup = Lower(dragged.group);
            int targetOrder = target.order;
            for (auto& a : g_apps) {
                if (a.group == target.group && a.order >= targetOrder && a.id != dragged.id) {
                    a.order += 2;
                    SyncAppToCustomization(a);
                }
            }
            dragged.order = targetOrder;
            SyncAppToCustomization(dragged);
            SaveCustomizations();
            RebuildLayout(false);
            return;
        }
    }

    for (const auto& h : g_headers) {
        if (worldPt.x >= h.x - S(20) && worldPt.x <= h.headerRect.right + S(30)) {
            dragged.group = h.rawGroup;
            dragged.searchGroup = Lower(dragged.group);
            dragged.order = g_nextOrderCounter++;
            SyncAppToCustomization(dragged);
            SaveCustomizations();
            RebuildLayout(false);
            return;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Keyboard Navigation & Search Input

void MoveSelection(int key) {
    HWND hwnd = g_window.load();
    if (!hwnd || g_visibleApps.empty()) return;
    if (g_selectedApp < 0) {
        g_selectedApp = g_visibleApps.front();
        EnsureSelectionVisible();
        InvalidateRect(hwnd, nullptr, FALSE);
        return;
    }
    auto currentIt = std::find(g_visibleApps.begin(), g_visibleApps.end(), g_selectedApp);
    if (currentIt == g_visibleApps.end()) {
        g_selectedApp = g_visibleApps.front();
        EnsureSelectionVisible();
        InvalidateRect(hwnd, nullptr, FALSE);
        return;
    }

    const RECT& current = g_apps[static_cast<size_t>(g_selectedApp)].rect;
    int cx = (current.left + current.right) / 2;
    int cy = (current.top + current.bottom) / 2;
    int best = -1;
    long long bestScore = LLONG_MAX;

    for (int candidate : g_visibleApps) {
        if (candidate == g_selectedApp) continue;
        const RECT& r = g_apps[static_cast<size_t>(candidate)].rect;
        if (r.right <= r.left || r.bottom <= r.top) continue;
        int x = (r.left + r.right) / 2;
        int y = (r.top + r.bottom) / 2;
        int dx = x - cx;
        int dy = y - cy;
        bool inDirection = false;
        if (key == VK_LEFT)  inDirection = dx < -S(8);
        if (key == VK_RIGHT) inDirection = dx > S(8);
        if (key == VK_UP)    inDirection = dy < -S(8);
        if (key == VK_DOWN)  inDirection = dy > S(8);
        if (!inDirection) continue;

        long long mainDist  = (key == VK_LEFT || key == VK_RIGHT) ? std::abs(dx) : std::abs(dy);
        long long crossDist = (key == VK_LEFT || key == VK_RIGHT) ? std::abs(dy) : std::abs(dx);
        long long score = mainDist + crossDist * 3;
        if (score < bestScore) {
            bestScore = score;
            best = candidate;
        }
    }

    if (best >= 0) {
        g_selectedApp = best;
        EnsureSelectionVisible();
        InvalidateRect(hwnd, nullptr, FALSE);
    }
}

void HandleCharInput(wchar_t ch) {
    HWND hwnd = g_window.load();
    if (!hwnd || g_alphaJumpView) return;

    if (g_renamingGroup) {
        if (ch == 8) {
            if (!g_renameBuffer.empty()) g_renameBuffer.pop_back();
        } else if (ch >= 32 && g_renameBuffer.size() < 48) {
            g_renameBuffer.push_back(ch);
        }
        InvalidateRect(hwnd, nullptr, FALSE);
        return;
    }

    if (ch == 8) {
        if (!g_searchText.empty()) {
            g_searchText.pop_back();
            g_searchFocused = true;
            RebuildLayout(false);
        }
        return;
    }
    if (ch == 0x7F) {
        size_t end = g_searchText.find_last_not_of(L' ');
        if (end == std::wstring::npos) g_searchText.clear();
        else {
            size_t sp = g_searchText.find_last_of(L' ', end);
            g_searchText.resize(sp == std::wstring::npos ? 0 : sp + 1);
        }
        g_searchFocused = true;
        RebuildLayout(false);
        return;
    }
    if (ch == 0x16) {
        if (OpenClipboard(hwnd)) {
            HANDLE data = GetClipboardData(CF_UNICODETEXT);
            if (data) {
                if (auto* txt = static_cast<const wchar_t*>(GlobalLock(data))) {
                    std::wstring clip(txt);
                    GlobalUnlock(data);
                    clip = clip.substr(0, clip.find_first_of(L"\r\n"));
                    if (g_searchText.size() + clip.size() < 100) {
                        g_searchText += clip;
                        g_searchFocused = true;
                        RebuildLayout(false);
                    }
                }
            }
            CloseClipboard();
        }
        return;
    }
    if (ch >= 32 && g_searchText.size() < 80) {
        g_searchText.push_back(ch);
        g_searchFocused = true;
        RebuildLayout(false);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Window Procedure

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        g_window = hwnd;
        CreateFonts(hwnd);
        g_fallbackIcon = LoadIconW(nullptr, IDI_APPLICATION);
        return 0;

    case WM_APP_TOGGLE:
        // Debounce rapid re-triggers right after closing via taskbar Start button click
        if (!IsWindowVisible(hwnd) && (GetTickCount64() - g_lastHideTick < 260)) {
            return 0;
        }
        ShowScreen();
        return 0;

    case WM_APP_RELOAD_CFG:
        LoadModSettings();
        CreateFonts(hwnd);
        FreeAppCachedBitmaps(g_apps);
        UnregisterHotKey(nullptr, kHotkeyId);
        RegisterHotKey(nullptr, kHotkeyId, g_settings.hotkeyModifiers, g_settings.hotkeyVk);
        StartAsyncAppDiscovery();
        if (IsWindowVisible(hwnd)) {
            RebuildLayout(false);
        }
        return 0;

    case WM_APP_APPS_UPDATED: {
        auto* discovered = reinterpret_cast<std::vector<AppEntry>*>(lParam);
        if (discovered) {
            ApplyCustomizationsToApps(*discovered);
            ClearApps();
            g_apps = std::move(*discovered);
            delete discovered;
            if (IsWindowVisible(hwnd)) {
                RebuildLayout(false);
            }
        }
        return 0;
    }

    case WM_TIMER:
        if (wParam == kTimerAnimation) {
            StepAnimationFrame();
        } else if (wParam == kTimerClock && IsWindowVisible(hwnd)) {
            InvalidateRect(hwnd, nullptr, FALSE);
        } else if (wParam == kTimerAutoRefresh) {
            StartAsyncAppDiscovery();
        }
        return 0;

    case WM_SIZE:
        FreeBackgroundBitmap();
        RebuildLayout(false);
        return 0;

    case WM_DISPLAYCHANGE:
    case WM_SETTINGCHANGE:
        FreeBackgroundBitmap();
        if (IsWindowVisible(hwnd)) {
            POINT pt{};
            GetCursorPos(&pt);
            HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
            MONITORINFO mi{};
            mi.cbSize = sizeof(mi);
            if (GetMonitorInfoW(mon, &mi)) {
                SetWindowPos(hwnd, HWND_TOPMOST, mi.rcMonitor.left, mi.rcMonitor.top,
                             mi.rcMonitor.right - mi.rcMonitor.left,
                             mi.rcMonitor.bottom - mi.rcMonitor.top,
                             SWP_NOACTIVATE);
            }
            RebuildLayout(false);
        }
        return 0;

    case WM_DPICHANGED: {
        g_dpi = HIWORD(wParam);
        const RECT* suggested = reinterpret_cast<const RECT*>(lParam);
        if (suggested) {
            SetWindowPos(hwnd, nullptr, suggested->left, suggested->top,
                         suggested->right - suggested->left, suggested->bottom - suggested->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
        }
        CreateFonts(hwnd);
        FreeAppCachedBitmaps(g_apps);
        RebuildLayout(false);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1;

    case WM_PAINT:
        PaintScreen(hwnd);
        return 0;

    case WM_MOUSEMOVE: {
        if (!g_trackingMouse) {
            TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
            g_trackingMouse = TrackMouseEvent(&tme) != FALSE;
        }
        POINT clientPt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
        RECT client{};
        GetClientRect(hwnd, &client);

        if (g_draggingScrollbar) {
            RECT track = ScrollbarTrackRect(client.right, client.bottom);
            RECT thumb = ScrollbarThumbRect(client.right, client.bottom);
            int thumbW = thumb.right - thumb.left;
            int trackW = std::max(1, static_cast<int>(track.right - track.left - thumbW));
            int newThumbX = std::clamp(static_cast<int>(clientPt.x) - g_scrollbarDragOffset,
                                       static_cast<int>(track.left),
                                       static_cast<int>(track.right) - thumbW);
            int maxScroll = std::max(0, g_contentWidth - static_cast<int>(client.right) + S(60));
            SetScrollTarget(MulDiv(newThumbX - track.left, maxScroll, trackW), true);
            return 0;
        }

        if (g_mouseDownOnTile && g_pressedTileIndex >= 0) {
            int dx = std::abs(clientPt.x - g_mouseDownPt.x);
            int dy = std::abs(clientPt.y - g_mouseDownPt.y);
            if (!g_isDraggingTile && (dx > S(8) || dy > S(8))) {
                g_isDraggingTile = true;
            }
            if (g_isDraggingTile) {
                g_dragCurrentPt = clientPt;
                if (clientPt.x < S(48)) {
                    SetScrollTarget(g_scrollX - S(18), true);
                } else if (clientPt.x > client.right - S(48)) {
                    SetScrollTarget(g_scrollX + S(18), true);
                }
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
        }

        bool needsRepaint = false;
        bool hAllApps = PointInRect(AllAppsButtonRect(client.right, client.bottom), clientPt);
        bool hPower   = PointInRect(TopPowerButtonRect(client.right), clientPt);
        bool hProfile = PointInRect(ProfilePillRect(client.right), clientPt);
        bool hSearch  = PointInRect(SearchBoxRect(client.right), clientPt);
        if (hAllApps != g_hoverAllAppsBtn || hPower != g_hoverPowerBtn ||
            hProfile != g_hoverProfileBtn || hSearch != g_hoverSearchBtn) {
            g_hoverAllAppsBtn = hAllApps;
            g_hoverPowerBtn   = hPower;
            g_hoverProfileBtn = hProfile;
            g_hoverSearchBtn  = hSearch;
            needsRepaint = true;
        }

        if (g_powerMenu) {
            RECT menu = PowerMenuRect(client.right, client.bottom);
            int item = PointInRect(menu, clientPt) ? (clientPt.y - menu.top - S(5)) / S(40) : -1;
            if (item < 0 || item >= 5) item = -1;
            if (item != g_hoverPowerItem) { g_hoverPowerItem = item; needsRepaint = true; }
        }
        if (g_profileMenu) {
            RECT menu = ProfileMenuRect(client.right);
            int item = PointInRect(menu, clientPt) ? (clientPt.y - menu.top - S(5)) / S(40) : -1;
            if (item < 0 || item >= 4) item = -1;
            if (item != g_hoverProfileItem) { g_hoverProfileItem = item; needsRepaint = true; }
        }

        POINT worldPt{clientPt.x + g_scrollX, clientPt.y};
        int hoverHeader = -1;
        if (!g_allAppsView && g_searchText.empty() && GroupHeadersEnabled()) {
            for (size_t i = 0; i < g_headers.size(); ++i) {
                if (PointInRect(g_headers[i].headerRect, worldPt)) {
                    hoverHeader = static_cast<int>(i);
                    break;
                }
            }
        }
        if (hoverHeader != g_hoverHeader) {
            g_hoverHeader = hoverHeader;
            needsRepaint = true;
        }

        int hover = -1;
        // When a dropdown menu is open under the cursor, don't highlight tiles underneath it!
        bool overPopup = (g_profileMenu && PointInRect(ProfileMenuRect(client.right), clientPt)) ||
                         (g_powerMenu && PointInRect(PowerMenuRect(client.right, client.bottom), clientPt));
        if (!overPopup) {
            for (int index : g_visibleApps) {
                if (PointInRect(g_apps[static_cast<size_t>(index)].rect, worldPt)) {
                    hover = index;
                    break;
                }
            }
        }
        if (hover != g_hoverApp) {
            g_hoverApp = hover;
            needsRepaint = true;
        }
        if (needsRepaint) {
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    }

    case WM_MOUSELEAVE:
        g_trackingMouse = false;
        if (g_hoverApp >= 0 || g_hoverHeader >= 0 || g_hoverPowerItem >= 0 ||
            g_hoverProfileItem >= 0 || g_hoverAllAppsBtn || g_hoverPowerBtn ||
            g_hoverProfileBtn || g_hoverSearchBtn) {
            g_hoverApp = -1;
            g_hoverHeader = -1;
            g_hoverPowerItem = -1;
            g_hoverProfileItem = -1;
            g_hoverAllAppsBtn = false;
            g_hoverPowerBtn = false;
            g_hoverProfileBtn = false;
            g_hoverSearchBtn = false;
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;

    case WM_LBUTTONDOWN: {
        POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
        RECT client{};
        GetClientRect(hwnd, &client);

        if (g_alphaJumpView) {
            for (int i = 0; i < 28; ++i) {
                if (PointInRect(g_alphaJumpRects[i], pt)) {
                    wchar_t target = (i == 0) ? L'#' : (i <= 26 ? static_cast<wchar_t>(L'A' + i - 1) : L'@');
                    for (const auto& ah : g_alphaHeaders) {
                        if (ah.letter == target) {
                            SetScrollTarget(std::max(0, static_cast<int>(ah.rect.left) - S(56)), false);
                            if (ah.firstVisibleIdx >= 0 && static_cast<size_t>(ah.firstVisibleIdx) < g_visibleApps.size()) {
                                g_selectedApp = g_visibleApps[static_cast<size_t>(ah.firstVisibleIdx)];
                            }
                            break;
                        }
                    }
                    break;
                }
            }
            g_alphaJumpView = false;
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }

        if (g_powerMenu) {
            RECT menu = PowerMenuRect(client.right, client.bottom);
            if (PointInRect(menu, pt)) {
                int row = (pt.y - menu.top - S(5)) / S(40);
                if (row >= 0 && row < 5) {
                    RunPowerAction(row);
                    return 0;
                }
            }
            g_powerMenu = false;
            InvalidateRect(hwnd, nullptr, FALSE);
            if (PointInRect(TopPowerButtonRect(client.right), pt)) return 0;
        }

        if (g_profileMenu) {
            RECT menu = ProfileMenuRect(client.right);
            if (PointInRect(menu, pt)) {
                int row = (pt.y - menu.top - S(5)) / S(40);
                if (row >= 0 && row < 4) {
                    RunProfileAction(row);
                    return 0;
                }
            }
            g_profileMenu = false;
            InvalidateRect(hwnd, nullptr, FALSE);
            if (PointInRect(ProfilePillRect(client.right), pt)) return 0;
        }

        if (g_contentWidth > client.right - S(56)) {
            RECT thumb = ScrollbarThumbRect(client.right, client.bottom);
            RECT track = ScrollbarTrackRect(client.right, client.bottom);
            if (PointInRect(thumb, pt)) {
                g_draggingScrollbar = true;
                g_scrollbarDragOffset = pt.x - thumb.left;
                SetCapture(hwnd);
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            } else if (PointInRect(track, pt)) {
                int thumbW = thumb.right - thumb.left;
                int trackW = std::max(1, static_cast<int>(track.right - track.left - thumbW));
                int maxScroll = std::max(0, g_contentWidth - static_cast<int>(client.right) + S(60));
                SetScrollTarget(MulDiv(pt.x - track.left - thumbW / 2, maxScroll, trackW), false);
                return 0;
            }
        }

        if (!g_searchText.empty() && PointInRect(SearchClearRect(client.right), pt)) {
            g_searchText.clear();
            g_searchFocused = true;
            RebuildLayout(false);
            return 0;
        }
        if (PointInRect(SearchBoxRect(client.right), pt)) {
            g_searchFocused = true;
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        g_searchFocused = false;

        if (PointInRect(ProfilePillRect(client.right), pt)) {
            g_profileMenu = !g_profileMenu;
            g_powerMenu = false;
            g_hoverProfileItem = -1;
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        if (PointInRect(TopPowerButtonRect(client.right), pt)) {
            g_powerMenu = !g_powerMenu;
            g_profileMenu = false;
            g_hoverPowerItem = -1;
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
        if (PointInRect(AllAppsButtonRect(client.right, client.bottom), pt)) {
            g_allAppsView = !g_allAppsView;
            RebuildLayout(true);
            return 0;
        }

        POINT worldPt{pt.x + g_scrollX, pt.y};
        if (g_allAppsView && g_searchText.empty()) {
            for (const auto& ah : g_alphaHeaders) {
                if (PointInRect(ah.rect, worldPt)) {
                    g_alphaJumpView = true;
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                }
            }
        } else if (!g_allAppsView && g_searchText.empty() && GroupHeadersEnabled()) {
            for (const auto& sh : g_headers) {
                if (PointInRect(sh.headerRect, worldPt)) {
                    g_renamingGroup = true;
                    g_renameGroupTarget = sh.rawGroup;
                    g_renameBuffer = sh.displayTitle;
                    InvalidateRect(hwnd, nullptr, FALSE);
                    return 0;
                }
            }
        }

        for (int index : g_visibleApps) {
            if (PointInRect(g_apps[static_cast<size_t>(index)].rect, worldPt)) {
                g_selectedApp = index;
                g_pressedTileIndex = index;
                g_mouseDownOnTile = true;
                g_isDraggingTile = false;
                g_mouseDownPt = pt;
                SetCapture(hwnd);
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
        }

        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    }

    case WM_LBUTTONUP: {
        POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
        if (GetCapture() == hwnd) ReleaseCapture();

        if (g_draggingScrollbar) {
            g_draggingScrollbar = false;
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }

        if (g_mouseDownOnTile) {
            int pressedIdx = g_pressedTileIndex;
            bool wasDragging = g_isDraggingTile;
            g_mouseDownOnTile = false;
            g_isDraggingTile = false;
            g_pressedTileIndex = -1;

            if (wasDragging && !g_allAppsView && g_searchText.empty()) {
                CompleteTileDragDrop(pt);
            } else if (pressedIdx >= 0) {
                POINT worldPt{pt.x + g_scrollX, pt.y};
                if (PointInRect(g_apps[static_cast<size_t>(pressedIdx)].rect, worldPt)) {
                    LaunchApp(pressedIdx, false);
                }
            }
            InvalidateRect(hwnd, nullptr, FALSE);
        }
        return 0;
    }

    case WM_RBUTTONUP: {
        POINT clientPt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
        POINT worldPt{clientPt.x + g_scrollX, clientPt.y};
        POINT screenPt = clientPt;
        ClientToScreen(hwnd, &screenPt);

        for (int index : g_visibleApps) {
            if (PointInRect(g_apps[static_cast<size_t>(index)].rect, worldPt)) {
                g_selectedApp = index;
                InvalidateRect(hwnd, nullptr, FALSE);
                ShowTileContextMenu(hwnd, index, screenPt);
                return 0;
            }
        }
        ShowCanvasContextMenu(hwnd, screenPt);
        return 0;
    }

    case WM_MOUSEWHEEL: {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        int direction = (delta > 0) ? -1 : 1;
        SetScrollTarget(g_scrollTargetX + direction * S(180), false);
        return 0;
    }

    case WM_KEYDOWN: {
        if (g_renamingGroup) {
            if (wParam == VK_ESCAPE) {
                g_renamingGroup = false;
                InvalidateRect(hwnd, nullptr, FALSE);
            } else if (wParam == VK_RETURN) {
                std::wstring trimmed = Trim(g_renameBuffer);
                if (!trimmed.empty()) {
                    g_groupRenames[g_renameGroupTarget] = trimmed;
                    SaveCustomizations();
                }
                g_renamingGroup = false;
                RebuildLayout(false);
            }
            return 0;
        }

        if (wParam == VK_ESCAPE) {
            if (g_alphaJumpView || g_powerMenu || g_profileMenu) {
                g_alphaJumpView = false;
                g_powerMenu = false;
                g_profileMenu = false;
                InvalidateRect(hwnd, nullptr, FALSE);
            } else if (!g_searchText.empty()) {
                g_searchText.clear();
                g_searchFocused = false;
                RebuildLayout(false);
            } else {
                HideScreen(true);
            }
            return 0;
        }

        if (wParam == VK_F2) {
            g_allAppsView = !g_allAppsView;
            RebuildLayout(true);
            return 0;
        }

        if (wParam == VK_RETURN) {
            if (g_searchFocused && !g_settings.enterInSearchLaunchesFirst) {
                g_searchFocused = false;
                if (!g_visibleApps.empty() && g_selectedApp < 0) {
                    g_selectedApp = g_visibleApps.front();
                }
                EnsureSelectionVisible();
                InvalidateRect(hwnd, nullptr, FALSE);
                return 0;
            }
            LaunchApp(g_selectedApp, (GetKeyState(VK_CONTROL) & 0x8000) != 0 && (GetKeyState(VK_SHIFT) & 0x8000) != 0);
            return 0;
        }

        if (wParam == VK_LEFT || wParam == VK_RIGHT || wParam == VK_UP || wParam == VK_DOWN) {
            g_searchFocused = false;
            MoveSelection(static_cast<int>(wParam));
            return 0;
        }

        if (wParam == VK_PRIOR) {
            SetScrollTarget(g_scrollTargetX - S(480), false);
            return 0;
        }
        if (wParam == VK_NEXT) {
            SetScrollTarget(g_scrollTargetX + S(480), false);
            return 0;
        }
        if (wParam == VK_HOME) {
            if (!g_visibleApps.empty()) g_selectedApp = g_visibleApps.front();
            SetScrollTarget(0, false);
            return 0;
        }
        if (wParam == VK_END) {
            if (!g_visibleApps.empty()) g_selectedApp = g_visibleApps.back();
            SetScrollTarget(g_contentWidth, false);
            return 0;
        }
        if (wParam == VK_APPS || (wParam == VK_F10 && (GetKeyState(VK_SHIFT) & 0x8000))) {
            if (g_selectedApp >= 0 && static_cast<size_t>(g_selectedApp) < g_apps.size()) {
                RECT r = g_apps[static_cast<size_t>(g_selectedApp)].rect;
                POINT pt{(r.left + r.right) / 2 - g_scrollX, (r.top + r.bottom) / 2};
                ClientToScreen(hwnd, &pt);
                ShowTileContextMenu(hwnd, g_selectedApp, pt);
            }
            return 0;
        }
        return 0;
    }

    case WM_CHAR:
        HandleCharInput(static_cast<wchar_t>(wParam));
        return 0;

    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE && IsWindowVisible(hwnd)) {
            HideScreen(false);
        }
        return 0;

    case WM_CLOSE:
        HideScreen(true);
        return 0;

    case WM_DESTROY:
        StopAnimationLoop();
        KillTimer(hwnd, kTimerClock);
        KillTimer(hwnd, kTimerAutoRefresh);
        FreeBackgroundBitmap();
        if (g_fontTitle) { DeleteObject(g_fontTitle); g_fontTitle = nullptr; }
        if (g_fontSubtitle) { DeleteObject(g_fontSubtitle); g_fontSubtitle = nullptr; }
        if (g_fontGroup) { DeleteObject(g_fontGroup); g_fontGroup = nullptr; }
        if (g_fontTile) { DeleteObject(g_fontTile); g_fontTile = nullptr; }
        if (g_fontTileLarge) { DeleteObject(g_fontTileLarge); g_fontTileLarge = nullptr; }
        if (g_fontClockBig) { DeleteObject(g_fontClockBig); g_fontClockBig = nullptr; }
        if (g_fontList) { DeleteObject(g_fontList); g_fontList = nullptr; }
        if (g_fontButton) { DeleteObject(g_fontButton); g_fontButton = nullptr; }
        g_window = nullptr;
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Taskbar Start Button & Windows Key Hook (twinui.pcshell.dll)

constexpr int kShowMethodKeyboard = 1;
constexpr int kShowFlagKeyboard   = 0x8;

bool IsKeyboardStartTrigger(int showMethod, int showFlags) {
    return showMethod == kShowMethodKeyboard && (showFlags & kShowFlagKeyboard);
}

using XamlLauncher_ShowStartView_t = HRESULT(WINAPI*)(void* pThis, int showMethod, int showFlags);
XamlLauncher_ShowStartView_t XamlLauncher_ShowStartView_Original = nullptr;

HRESULT WINAPI XamlLauncher_ShowStartView_Hook(void* pThis, int showMethod, int showFlags) {
    bool isKeyboard = IsKeyboardStartTrigger(showMethod, showFlags);
    bool intercept = isKeyboard ? g_openWithWinKeyAtomic.load() : g_openWithStartBtnAtomic.load();
    HWND hwnd = g_window.load();
    if (!intercept || !hwnd) {
        return XamlLauncher_ShowStartView_Original
                   ? XamlLauncher_ShowStartView_Original(pThis, showMethod, showFlags)
                   : S_OK;
    }
    PostMessageW(hwnd, WM_APP_TOGGLE, 0, 0);
    return S_OK;
}

void InstallStartButtonHook() {
#if defined(HAS_WINDHAWK_UTILS)
    HMODULE module = LoadLibraryExW(L"twinui.pcshell.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"twinui.pcshell.dll not loaded; using Ctrl+Alt+S hotkey.");
        return;
    }
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: virtual long __cdecl XamlLauncher::ShowStartView(enum IMMERSIVELAUNCHERSHOWMETHOD,enum IMMERSIVELAUNCHERSHOWFLAGS))"},
            &XamlLauncher_ShowStartView_Original,
            XamlLauncher_ShowStartView_Hook,
        },
    };
    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"Could not hook XamlLauncher::ShowStartView; backup hotkey remains active.");
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////
// UI Thread & Windhawk Lifecycle

DWORD WINAPI UiThreadProc(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    MSG message{};
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    LoadModSettings();
    LoadCustomizations();

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszClassName = L"MostafaWin8MetroStartScreen";
    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
                                wc.lpszClassName,
                                L"Windows 8 Metro Start Screen",
                                WS_POPUP | WS_CLIPCHILDREN, 0, 0, 1024, 768,
                                nullptr, nullptr, wc.hInstance, nullptr);
    if (!hwnd) {
        Wh_Log(L"Failed to create the Metro Start Screen window (error %lu).", GetLastError());
        if (g_readyEvent) SetEvent(g_readyEvent);
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        CoUninitialize();
        return 1;
    }
    g_window = hwnd;
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    ShowWindow(hwnd, SW_HIDE);

    if (!RegisterHotKey(nullptr, kHotkeyId, g_settings.hotkeyModifiers, g_settings.hotkeyVk)) {
        Wh_Log(L"Could not register toggle hotkey (error %lu).", GetLastError());
    }

    if (g_settings.autoRefreshMinutes > 0) {
        SetTimer(hwnd, kTimerAutoRefresh, static_cast<UINT>(g_settings.autoRefreshMinutes) * 60000u, nullptr);
    }

    if (g_readyEvent) SetEvent(g_readyEvent);
    StartAsyncAppDiscovery();

    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        if (message.message == WM_HOTKEY && message.wParam == kHotkeyId) {
            ShowScreen();
            continue;
        }
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    g_stopDiscovery = true;
    if (g_discoveryThread) {
        WaitForSingleObject(g_discoveryThread, INFINITE);
        CloseHandle(g_discoveryThread);
        g_discoveryThread = nullptr;
    }

    while (PeekMessageW(&message, nullptr, WM_APP_APPS_UPDATED, WM_APP_APPS_UPDATED, PM_REMOVE)) {
        auto* list = reinterpret_cast<std::vector<AppEntry>*>(message.lParam);
        if (list) {
            FreeAppListIcons(*list);
            delete list;
        }
    }

    UnregisterHotKey(nullptr, kHotkeyId);
    if (hwnd && IsWindow(hwnd)) DestroyWindow(hwnd);
    g_window = nullptr;
    ClearApps();
    FreeBackgroundBitmap();
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    CoUninitialize();
    return 0;
}

} // namespace

BOOL Wh_ModInit() {
    if (IsFolderWindowsProcess()) {
        Wh_Log(L"Skipping initialization in secondary folder process.");
        return FALSE;
    }

    LoadModSettings();
    InstallStartButtonHook();

    g_stopDiscovery = false;
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        Wh_Log(L"Failed to create startup event (error %lu).", GetLastError());
        return FALSE;
    }
    g_thread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &g_threadId);
    if (!g_thread) {
        Wh_Log(L"Failed to start UI thread (error %lu).", GetLastError());
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }
    WaitForSingleObject(g_readyEvent, INFINITE);
    CloseHandle(g_readyEvent);
    g_readyEvent = nullptr;
    Wh_Log(L"Windows 8 Metro Start Screen v2.2 initialized.");
    return TRUE;
}

void Wh_ModUninit() {
    g_stopDiscovery = true;
    if (g_threadId) PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
    if (g_thread) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    g_threadId = 0;
    Wh_Log(L"Windows 8 Metro Start Screen v2.2 unloaded cleanly.");
}

void Wh_ModSettingsChanged() {
    HWND hwnd = g_window.load();
    if (hwnd && IsWindow(hwnd)) {
        PostMessageW(hwnd, WM_APP_RELOAD_CFG, 0, 0);
    }
}

// ==WindhawkMod==
// @id              taskbar-media-player
// @name            Taskbar Media Player
// @description     A modern media controller for the Windows taskbar with popup player, themes (Acrylic / Mica / Glass / Solid / Transparent), and two layouts (Rounded Apple-style / Windows-style).
// @version         1.8.0
// @author          Touseef
// @github          https://github.com/Touseeef
// @include         explorer.exe
// @compilerOptions -lole32 -ldwmapi -lgdi32 -luser32 -lshcore -lgdiplus -lshell32 -lwindowsapp -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Media Player

A sleek, native-style media controller that lives directly on your Windows taskbar.

## Layouts
- **Rounded (Apple-style)**:
  - Compact bar: Album art, recessed capsule title pill with track info & progress, and media controls on the right.
  - Popup: Large album art with a 3D layered card stack behind it, centered typography, progress timestamps, and full controls.
- **Windows (Windows-style)**:
  - Compact bar: Boxy styling with subtle corner rounding, taskbar-aligned controls.
  - Popup: Horizontal Windows 11 flyout layout (album art on the left, track details on the right, controls below).

## Themes
- **Acrylic**: Translucent frosted blur-behind.
- **Mica**: Soft, dark tinted surface.
- **Glass**: Frosted blur with a crystal top sheen.
- **Solid**: Fully opaque modern card.
- **Transparent**: Controls and text float directly on the taskbar with no background card.

## Features
- **Clean Controls**: Symmetrical media buttons with smooth hover glows (no clashing permanent circles).
- **Interactive Popup Player**: Click the empty area of the compact bar to open; click anywhere outside to dismiss.
- **Progress Bar with Seek**: Click anywhere on the progress bar in the bar or popup to seek instantly.
- **Hide in Fullscreen**: Enabled by default to keep games and videos distraction-free.
- **Volume Wheel**: Scroll mouse wheel over the bar or popup to adjust system volume.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- LayoutStyle: rounded
  $name: Layout Style
  $description: Rounded = Apple-style pill with stacked art popup. Windows = Windows 11-style flyout.
  $options:
  - rounded: Rounded (Apple-style)
  - windows: Windows (Windows-style)
- Theme: acrylic
  $name: Theme
  $description: Surface material of the bar and popup
  $options:
  - acrylic: Acrylic (translucent frosted blur)
  - mica: Mica (soft, dark tinted surface)
  - glass: Glass (crystal sheen with frosted blur)
  - solid: Solid (opaque modern card)
  - transparent: Transparent (controls float directly on taskbar)
- StackedArt: 1
  $name: Stacked Album Art (Rounded layout)
  $description: Show a layered stack of cards behind the album art in the popup (1 = on, 0 = off)
- HideFullscreen: 1
  $name: Hide in Fullscreen
  $description: Hide the panel when a fullscreen application is detected (1 = on, 0 = off)
- PanelWidth: 360
  $name: Panel Width
  $description: Width of the media player panel in pixels (minimum 200)
- PanelHeight: 46
  $name: Panel Height
  $description: Height of the media player panel in pixels (minimum 32, recommended 44-48)
- FontSize: 11
  $name: Font Size
  $description: Track text font size in pixels
- IconSize: 14
  $name: Icon Size
  $description: Size of media control icons in pixels
- OffsetX: 12
  $name: Horizontal Offset
  $description: Horizontal offset from the left edge of the taskbar (pixels)
- OffsetY: 0
  $name: Vertical Offset
  $description: Vertical offset from the center of the taskbar (pixels)
- AutoTheme: 1
  $name: Auto Light/Dark
  $description: Match system light/dark mode automatically (1 = on, 0 = off)
- TextColor: FFFFFF
  $name: Text Color (Hex)
  $description: Manual text color as hex RGB, e.g. FFFFFF for white (only used when Auto Light/Dark is off)
- BgOpacity: 45
  $name: Background Opacity
  $description: Acrylic tint opacity 0-255 (only used by the Acrylic theme when Auto Light/Dark is off)
- ShowProgressBar: 1
  $name: Show Progress Bar
  $description: Display a song progress bar in the compact bar (1 = on, 0 = off)
- ProgressBarHeight: 3
  $name: Progress Bar Height
  $description: Height of the progress bar in pixels (1-6)
- UseAccentColor: 1
  $name: Use System Accent Color
  $description: Use Windows accent color for the progress bar (1 = on, 0 = off)
- AccentColorHex: 1DB954
  $name: Accent Color (Hex)
  $description: Custom accent color as hex RGB (only used when Use System Accent Color is off)
- ShowShuffleRepeat: 0
  $name: Show Shuffle/Repeat in Bar
  $description: Show shuffle and repeat buttons in the compact bar (1 = on, 0 = off). Always available in the popup.
- IdleTimeout: 0
  $name: Idle Timeout (seconds)
  $description: Seconds of no playback before auto-hiding the panel (0 = never hide)
- ScrollSpeed: 1
  $name: Scroll Speed
  $description: Text scroll speed for long titles, 1 (slow) to 5 (fast)
*/
// ==/WindhawkModSettings==

// ═══════════════════════════════════════════════════════════════════════════════
// Includes
// ═══════════════════════════════════════════════════════════════════════════════

#include <windows.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shcore.h>
#include <string>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>
#include <memory>
#include <cstdio>
#include <cmath>
#include <algorithm>

// WinRT
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace Gdiplus;
using namespace std;
using namespace winrt;
using namespace Windows::Media;
using namespace Windows::Media::Control;
using namespace Windows::Storage::Streams;

// [FIX] shared_ptr wrapper so the album-art bitmap cannot be freed while a
//       paint is in flight. See DrawMediaPanel / DrawPopupPanel / UpdateMediaInfo.
struct GdiplusBitmapDeleter {
    void operator()(Bitmap* b) const noexcept { delete b; }
};
using BitmapPtr = std::shared_ptr<Bitmap>;

// ═══════════════════════════════════════════════════════════════════════════════
// DWM / Composition API Types
// ═══════════════════════════════════════════════════════════════════════════════

typedef enum _WINDOWCOMPOSITIONATTRIB {
    WCA_ACCENT_POLICY = 19
} WINDOWCOMPOSITIONATTRIB;

typedef enum _ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_INVALID_STATE = 5
} ACCENT_STATE;

typedef struct _ACCENT_POLICY {
    ACCENT_STATE AccentState;
    DWORD AccentFlags;
    DWORD GradientColor;
    DWORD AnimationId;
} ACCENT_POLICY;

typedef struct _WINDOWCOMPOSITIONATTRIBDATA {
    WINDOWCOMPOSITIONATTRIB Attribute;
    PVOID Data;
    SIZE_T SizeOfData;
} WINDOWCOMPOSITIONATTRIBDATA;

typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

// ═══════════════════════════════════════════════════════════════════════════════
// Z-Band API
// ═══════════════════════════════════════════════════════════════════════════════

enum ZBID {
    ZBID_DEFAULT = 0, ZBID_DESKTOP, ZBID_UIACCESS,
    ZBID_IMMERSIVE_IHM, ZBID_IMMERSIVE_NOTIFICATION,
    ZBID_IMMERSIVE_APPCHROME, ZBID_IMMERSIVE_MOGO,
    ZBID_IMMERSIVE_EDGY, ZBID_IMMERSIVE_INACTIVEMOBODY,
    ZBID_IMMERSIVE_INACTIVEDOCK, ZBID_IMMERSIVE_ACTIVEMOBODY,
    ZBID_IMMERSIVE_ACTIVEDOCK, ZBID_IMMERSIVE_BACKGROUND,
    ZBID_IMMERSIVE_SEARCH, ZBID_GENUINE_WINDOWS,
    ZBID_IMMERSIVE_RESTRICTED, ZBID_SYSTEM_TOOLS,
    ZBID_LOCK, ZBID_ABOVELOCK_UX,
};

typedef HWND(WINAPI* pCreateWindowInBand)(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int x, int y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
    LPVOID lpParam, DWORD dwBand);

// ═══════════════════════════════════════════════════════════════════════════════
// Icon Font Codepoints (Segoe MDL2 Assets / Segoe Fluent Icons)
// ═══════════════════════════════════════════════════════════════════════════════

static const wchar_t ICON_PLAY[]       = { 0xE768, 0 };
static const wchar_t ICON_PAUSE[]      = { 0xE769, 0 };
static const wchar_t ICON_PREVIOUS[]   = { 0xE892, 0 };
static const wchar_t ICON_NEXT[]       = { 0xE893, 0 };
static const wchar_t ICON_SHUFFLE[]    = { 0xE8B1, 0 };
static const wchar_t ICON_REPEAT_ALL[] = { 0xE8EE, 0 };
static const wchar_t ICON_REPEAT_ONE[] = { 0xE8ED, 0 };
static const wchar_t ICON_MUSIC_NOTE[] = { 0xE8D6, 0 };

// ═══════════════════════════════════════════════════════════════════════════════
// Hit Testing Enums
// ═══════════════════════════════════════════════════════════════════════════════

enum HitTarget {
    HIT_NONE = 0,
    HIT_PREV, HIT_PLAYPAUSE, HIT_NEXT,
    HIT_SHUFFLE, HIT_REPEAT, HIT_PROGRESS,
};

enum PopupHitTarget {
    PHIT_NONE = 0,
    PHIT_PREV, PHIT_PLAYPAUSE, PHIT_NEXT,
    PHIT_SHUFFLE, PHIT_REPEAT, PHIT_PROGRESS,
};

// ═══════════════════════════════════════════════════════════════════════════════
// Context Menu IDs
// ═══════════════════════════════════════════════════════════════════════════════

#define IDM_PLAYPAUSE   40001
#define IDM_PREV        40002
#define IDM_NEXT        40003
#define IDM_SHUFFLE     40004
#define IDM_REPEAT_OFF  40005
#define IDM_REPEAT_ALL  40006
#define IDM_REPEAT_ONE  40007

// ═══════════════════════════════════════════════════════════════════════════════
// Configuration
// ═══════════════════════════════════════════════════════════════════════════════

enum ThemeKind  { THEME_ACRYLIC = 0, THEME_MICA, THEME_GLASS, THEME_SOLID, THEME_TRANSPARENT };
enum LayoutKind { LAYOUT_ROUNDED = 0, LAYOUT_WINDOWS };

struct ModSettings {
    int  theme            = THEME_ACRYLIC;
    int  layoutStyle      = LAYOUT_ROUNDED;
    bool stackedArt       = true;
    bool hideFullscreen   = true;
    int width             = 360;
    int height            = 46;
    int fontSize          = 11;
    int iconSize          = 14;
    int offsetX           = 12;
    int offsetY           = 0;
    bool autoTheme        = true;
    DWORD manualTextColor = 0xFFFFFFFF;
    int bgOpacity         = 45;
    bool showProgressBar  = true;
    int progressBarHeight = 3;
    bool useAccentColor   = true;
    DWORD accentColorRGB  = 0xFF1DB954;
    bool showShuffleRepeat = false;
    int idleTimeout       = 0;
    int scrollSpeed       = 1;
} g_Settings;

static bool IsRounded() { return g_Settings.layoutStyle == LAYOUT_ROUNDED; }

// ═══════════════════════════════════════════════════════════════════════════════
// Layout Metrics
// ═══════════════════════════════════════════════════════════════════════════════

static const int POPUP_GAP = 8;

struct LayoutMetrics {
    int artX, artY, artSize, artRadius;
    float btnPrevX, btnPlayX, btnNextX;
    float btnShuffleX, btnRepeatX;
    float btnCenterY, btnRadius, playRadius;
    int textX, textMaxW;
    bool pillVisible;
    int pillX, pillY, pillW, pillH;
    int progX, progY, progW, progH;
    bool progVisible;
};

struct PopupLayoutMetrics {
    int artX, artY, artSize, artRadius;
    bool stacked;
    int stackStep;
    int textX, textW;
    bool centered;
    int titleY, artistY, albumY;
    int timeLabelY;
    int progX, progY, progW, progH;
    float btnPrevX, btnPlayX, btnNextX;
    float btnShuffleX, btnRepeatX;
    float btnCenterY, btnRadius, playRadius;
    float iconSize, playIconSize;
    float titleSize, artistSize, albumSize;
    int pad;
};

static LayoutMetrics g_Layout;
static PopupLayoutMetrics g_PopupLayout;
static int g_PopupW = 340;
static int g_PopupH = 440;

static int CompactCornerRadius() { return 8; }
static int PopupCornerRadius()   { return IsRounded() ? 16 : 8; }

// ═══════════════════════════════════════════════════════════════════════════════
// Global State
// ═══════════════════════════════════════════════════════════════════════════════

static HWND g_hMediaWindow          = NULL;
static HWND g_hPopupWindow          = NULL;
static HitTarget g_HoverState       = HIT_NONE;
static PopupHitTarget g_PopupHover  = PHIT_NONE;
static HWINEVENTHOOK g_TaskbarHook  = nullptr;
static UINT g_TaskbarCreatedMsg     = RegisterWindowMessageW(L"TaskbarCreated");

static bool g_PopupVisible             = false;
static ULONGLONG g_PopupHideTimestamp   = 0;

static int  g_IdleSecondsCounter = 0;
static bool g_IsHiddenByIdle     = false;

static int  g_ScrollOffset = 0;
static int  g_TextWidth    = 0;
static bool g_IsScrolling  = false;
static int  g_ScrollWait   = 94; // ~3s pause at 33ms/tick before scrolling

static const wchar_t* g_IconFontName = L"Segoe MDL2 Assets";
static const wchar_t* g_TextFontName = L"Segoe UI";

// [PERF] Cached fonts — GDI+ FontFamily construction enumerates installed
//        fonts, so building these every frame is very expensive.
struct CachedFonts {
    std::unique_ptr<FontFamily> iconFam;
    std::unique_ptr<FontFamily> textFam;
    std::unique_ptr<Font>       iconBar;      // compact bar icons
    std::unique_ptr<Font>       titleBar;     // compact bar title
    std::unique_ptr<Font>       iconPopup;    // popup small icons
    std::unique_ptr<Font>       playPopup;    // popup play/pause
    std::unique_ptr<Font>       titlePopup;
    std::unique_ptr<Font>       artistPopup;
    std::unique_ptr<Font>       albumPopup;
    std::unique_ptr<Font>       timePopup;

    void Rebuild() {
        iconFam = std::make_unique<FontFamily>(g_IconFontName, nullptr);
        textFam = std::make_unique<FontFamily>(g_TextFontName, nullptr);

        iconBar   = std::make_unique<Font>(iconFam.get(), (REAL)g_Settings.iconSize,    FontStyleRegular, UnitPixel);
        titleBar  = std::make_unique<Font>(textFam.get(), (REAL)g_Settings.fontSize,    FontStyleBold,    UnitPixel);

        iconPopup  = std::make_unique<Font>(iconFam.get(), g_PopupLayout.iconSize,     FontStyleRegular, UnitPixel);
        playPopup  = std::make_unique<Font>(iconFam.get(), g_PopupLayout.playIconSize, FontStyleRegular, UnitPixel);
        titlePopup = std::make_unique<Font>(textFam.get(), g_PopupLayout.titleSize,    FontStyleBold,    UnitPixel);
        artistPopup= std::make_unique<Font>(textFam.get(), g_PopupLayout.artistSize,   FontStyleRegular, UnitPixel);
        albumPopup = std::make_unique<Font>(textFam.get(), g_PopupLayout.albumSize,    FontStyleRegular, UnitPixel);
        timePopup  = std::make_unique<Font>(textFam.get(), 10.5f,                     FontStyleRegular, UnitPixel);
    }
};
static CachedFonts g_Fonts;

// ═══════════════════════════════════════════════════════════════════════════════
// Media State
// ═══════════════════════════════════════════════════════════════════════════════

struct MediaState {
    wstring title   = L"No Media";
    wstring artist  = L"";
    wstring album   = L"";
    bool isPlaying  = false;
    bool hasMedia   = false;
    BitmapPtr albumArt;   // [FIX] shared_ptr — safe to copy out of the lock

    double positionSeconds = 0.0;
    double durationSeconds = 0.0;
    ULONGLONG positionTick = 0;
    double lastPolledPos   = -1.0;   // [FIX] raw reported position from previous poll
    bool   lastPlayingState = false; // [FIX] to detect play→pause transition

    bool shuffleActive    = false;
    bool shuffleAvailable = false;
    int  repeatMode       = 0;
    bool repeatAvailable  = false;

    mutex lock;
} g_MediaState;

static GlobalSystemMediaTransportControlsSessionManager g_SessionManager = nullptr;
static GlobalSystemMediaTransportControlsSession        g_CurrentSession = nullptr;
static std::mutex                                       g_SessionLock;   // [FIX] protects g_CurrentSession

// [FIX] polling thread asks the UI thread to reset scroll state
static std::atomic<bool> g_ResetScrollRequested{false};

// [PERF] Forward declarations so CachedTheme::Refresh() can call the
//        theme/color helpers defined later in the file.
static DWORD GetCurrentTextColor();
static bool  IsDarkSurface();
static Color GetAccentGdiColor();

// [PERF] Cached theme colors. Without this, every paint calls RegGetValueW
//        (4x) and DwmGetColorizationColor (1x) — 300 registry reads/sec.
struct CachedTheme {
    Color  text{255, 255, 255, 255};
    Color  accent{255, 0, 120, 212};
    bool   dark = true;
    DWORD  textColorRaw = 0xFFFFFFFF;

    void Refresh() {
        dark = IsDarkSurface();
        textColorRaw = GetCurrentTextColor();
        DWORD c = textColorRaw;
        text = Color(255, (BYTE)((c>>16)&0xFF), (BYTE)((c>>8)&0xFF), (BYTE)(c&0xFF));
        accent = GetAccentGdiColor();
    }
};
static CachedTheme g_Theme;

// ═══════════════════════════════════════════════════════════════════════════════
// Timer IDs
// ═══════════════════════════════════════════════════════════════════════════════

#define IDT_POLL_MEDIA 1001
#define IDT_ANIMATION  1002
#define APP_WM_CLOSE   WM_APP

// ═══════════════════════════════════════════════════════════════════════════════
// Settings Loading
// ═══════════════════════════════════════════════════════════════════════════════

static void LoadSettings() {
    g_Settings.theme = THEME_ACRYLIC;
    if (PCWSTR th = Wh_GetStringSetting(L"Theme")) {
        if (wcscmp(th, L"mica") == 0)             g_Settings.theme = THEME_MICA;
        else if (wcscmp(th, L"glass") == 0)       g_Settings.theme = THEME_GLASS;
        else if (wcscmp(th, L"solid") == 0)       g_Settings.theme = THEME_SOLID;
        else if (wcscmp(th, L"transparent") == 0) g_Settings.theme = THEME_TRANSPARENT;
        Wh_FreeStringSetting(th);
    }

    g_Settings.layoutStyle = LAYOUT_ROUNDED;
    if (PCWSTR ls = Wh_GetStringSetting(L"LayoutStyle")) {
        if (wcscmp(ls, L"windows") == 0)      g_Settings.layoutStyle = LAYOUT_WINDOWS;
        else if (wcscmp(ls, L"rounded") == 0) g_Settings.layoutStyle = LAYOUT_ROUNDED;
        Wh_FreeStringSetting(ls);
    }

    g_Settings.stackedArt     = Wh_GetIntSetting(L"StackedArt") != 0;
    g_Settings.hideFullscreen = Wh_GetIntSetting(L"HideFullscreen") != 0;

    g_Settings.width     = Wh_GetIntSetting(L"PanelWidth");
    g_Settings.height    = Wh_GetIntSetting(L"PanelHeight");
    g_Settings.fontSize  = Wh_GetIntSetting(L"FontSize");
    g_Settings.iconSize  = Wh_GetIntSetting(L"IconSize");
    g_Settings.offsetX   = Wh_GetIntSetting(L"OffsetX");
    g_Settings.offsetY   = Wh_GetIntSetting(L"OffsetY");
    g_Settings.autoTheme = Wh_GetIntSetting(L"AutoTheme") != 0;

    PCWSTR textHex = Wh_GetStringSetting(L"TextColor");
    DWORD textRGB = 0xFFFFFF;
    if (textHex && wcslen(textHex) > 0) textRGB = wcstoul(textHex, nullptr, 16);
    if (textHex) Wh_FreeStringSetting(textHex);
    g_Settings.manualTextColor = 0xFF000000 | (textRGB & 0x00FFFFFF);

    g_Settings.bgOpacity = Wh_GetIntSetting(L"BgOpacity");
    if (g_Settings.bgOpacity < 0) g_Settings.bgOpacity = 0;
    if (g_Settings.bgOpacity > 255) g_Settings.bgOpacity = 255;

    g_Settings.showProgressBar  = Wh_GetIntSetting(L"ShowProgressBar") != 0;
    g_Settings.progressBarHeight = Wh_GetIntSetting(L"ProgressBarHeight");
    if (g_Settings.progressBarHeight < 1) g_Settings.progressBarHeight = 1;
    if (g_Settings.progressBarHeight > 6) g_Settings.progressBarHeight = 6;

    g_Settings.useAccentColor = Wh_GetIntSetting(L"UseAccentColor") != 0;

    PCWSTR accentHex = Wh_GetStringSetting(L"AccentColorHex");
    DWORD accentRGB = 0x1DB954;
    if (accentHex && wcslen(accentHex) > 0) accentRGB = wcstoul(accentHex, nullptr, 16);
    if (accentHex) Wh_FreeStringSetting(accentHex);
    g_Settings.accentColorRGB = 0xFF000000 | (accentRGB & 0x00FFFFFF);

    g_Settings.showShuffleRepeat = Wh_GetIntSetting(L"ShowShuffleRepeat") != 0;
    g_Settings.idleTimeout       = Wh_GetIntSetting(L"IdleTimeout");
    g_Settings.scrollSpeed       = Wh_GetIntSetting(L"ScrollSpeed");
    if (g_Settings.scrollSpeed < 1) g_Settings.scrollSpeed = 1;
    if (g_Settings.scrollSpeed > 5) g_Settings.scrollSpeed = 5;

    if (g_Settings.width  < 200) g_Settings.width  = 360;
    if (g_Settings.height < 32)  g_Settings.height = 46;
    if (g_Settings.fontSize < 8) g_Settings.fontSize = 11;
    if (g_Settings.iconSize < 8) g_Settings.iconSize = 14;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Font Detection
// ═══════════════════════════════════════════════════════════════════════════════

static void DetectFonts() {
    {
        FontFamily test(L"Segoe Fluent Icons", nullptr);
        g_IconFontName = (test.GetLastStatus() == Ok)
            ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets";
    }
    {
        FontFamily test(L"Segoe UI Variable Display", nullptr);
        g_TextFontName = (test.GetLastStatus() == Ok)
            ? L"Segoe UI Variable Display" : L"Segoe UI";
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Layout Computation
// ═══════════════════════════════════════════════════════════════════════════════

static void ComputeLayout() {
    auto& L = g_Layout;
    int W = g_Settings.width;
    int H = g_Settings.height;
    bool rounded = IsRounded();

    L.btnRadius  = 11.0f;
    L.playRadius = 14.0f;
    L.btnCenterY = (float)(H / 2.0f);

    float btnGap = 28.0f;

    if (rounded) {
        int artPad = 7;
        L.artSize = H - artPad * 2;
        L.artX = 10;
        L.artY = artPad;
        L.artRadius = 6;

        float rightMargin = 12.0f;
        float firstBtnX;
        if (g_Settings.showShuffleRepeat) {
            L.btnRepeatX  = (float)W - rightMargin - L.btnRadius;
            L.btnNextX    = L.btnRepeatX - 24.0f;
            L.btnPlayX    = L.btnNextX - btnGap;
            L.btnPrevX    = L.btnPlayX - btnGap;
            L.btnShuffleX = L.btnPrevX - 24.0f;
            firstBtnX     = L.btnShuffleX;
        } else {
            L.btnShuffleX = -100;
            L.btnRepeatX  = -100;
            L.btnNextX    = (float)W - rightMargin - L.btnRadius;
            L.btnPlayX    = L.btnNextX - btnGap;
            L.btnPrevX    = L.btnPlayX - btnGap;
            firstBtnX     = L.btnPrevX;
        }

        L.pillVisible = true;
        L.pillX = L.artX + L.artSize + 8;
        L.pillY = 5;
        L.pillH = H - 10;
        if (L.pillH < 28) L.pillH = 28;
        L.pillW = (int)(firstBtnX - L.btnRadius - 8.0f) - L.pillX;
        if (L.pillW < 40) L.pillW = 40;

        int capPad = (L.pillH / 2) - 3;
        if (capPad < 8) capPad = 8;
        L.textX = L.pillX + capPad;
        L.textMaxW = L.pillW - (capPad * 2);
        if (L.textMaxW < 20) L.textMaxW = 20;

        L.progH = g_Settings.progressBarHeight;
        L.progVisible = g_Settings.showProgressBar;
        L.progX = L.textX;
        L.progW = L.textMaxW;
        L.progY = L.pillY + L.pillH - L.progH - 5;
    } else {
        int artPad = 7;
        L.artSize = H - artPad * 2;
        L.artX = 10;
        L.artY = artPad;
        L.artRadius = 4;

        float rightMargin = 14.0f;
        float firstBtnX;
        if (g_Settings.showShuffleRepeat) {
            L.btnRepeatX  = (float)W - rightMargin - L.btnRadius;
            L.btnNextX    = L.btnRepeatX - 24.0f;
            L.btnPlayX    = L.btnNextX - btnGap;
            L.btnPrevX    = L.btnPlayX - btnGap;
            L.btnShuffleX = L.btnPrevX - 24.0f;
            firstBtnX     = L.btnShuffleX;
        } else {
            L.btnShuffleX = -100;
            L.btnRepeatX  = -100;
            L.btnNextX    = (float)W - rightMargin - L.btnRadius;
            L.btnPlayX    = L.btnNextX - btnGap;
            L.btnPrevX    = L.btnPlayX - btnGap;
            firstBtnX     = L.btnPrevX;
        }

        L.pillVisible = false;
        L.pillX = L.pillY = L.pillW = L.pillH = 0;

        L.textX = L.artX + L.artSize + 10;
        L.textMaxW = (int)(firstBtnX - L.btnRadius - 10.0f) - L.textX;
        if (L.textMaxW < 40) L.textMaxW = 40;

        L.progH = g_Settings.progressBarHeight;
        L.progVisible = g_Settings.showProgressBar;
        L.progX = L.textX;
        L.progW = L.textMaxW;
        L.progY = H - L.progH - 6;
    }
}

static void ComputePopupLayout() {
    auto& P = g_PopupLayout;

    if (IsRounded()) {
        g_PopupW = 340;
        P.pad = 20;
        P.stacked = g_Settings.stackedArt;
        P.stackStep = 8;
        P.centered = true;

        int top = P.pad + (P.stacked ? P.stackStep * 2 : 0);
        P.artSize = 210;
        P.artX = (g_PopupW - P.artSize) / 2;
        P.artY = top;
        P.artRadius = 16;

        int artBottom = P.artY + P.artSize;
        P.textX = P.pad;
        P.textW = g_PopupW - 2 * P.pad;
        P.titleY  = artBottom + 16;
        P.artistY = P.titleY + 24;
        P.albumY  = P.artistY + 18;
        P.titleSize = 16.0f; P.artistSize = 13.0f; P.albumSize = 11.0f;

        P.progX = P.pad + 36;
        P.progW = g_PopupW - 2 * (P.pad + 36);
        P.progH = 4;
        P.progY = P.albumY + 30;
        P.timeLabelY = P.progY + P.progH / 2 - 8;

        P.btnRadius = 16.0f; P.playRadius = 24.0f;
        P.iconSize = 17.0f;  P.playIconSize = 22.0f;
        float gap = 50.0f;
        P.btnCenterY = (float)(P.progY + P.progH + 12) + P.playRadius;

        float rowW = gap * 4;
        float startX = (g_PopupW - rowW) / 2.0f;
        P.btnShuffleX = startX;
        P.btnPrevX    = startX + gap;
        P.btnPlayX    = startX + gap * 2;
        P.btnNextX    = startX + gap * 3;
        P.btnRepeatX  = startX + gap * 4;

        g_PopupH = (int)(P.btnCenterY + P.playRadius) + P.pad;
    } else {
        g_PopupW = 360;
        P.pad = 16;
        P.stacked = false;
        P.stackStep = 0;
        P.centered = false;

        P.artSize = 92;
        P.artX = P.pad;
        P.artY = P.pad;
        P.artRadius = 6;

        P.textX = P.artX + P.artSize + 14;
        P.textW = g_PopupW - P.textX - P.pad;
        P.titleY  = P.artY + 4;
        P.artistY = P.titleY + 24;
        P.albumY  = P.artistY + 18;
        P.titleSize = 15.0f; P.artistSize = 12.5f; P.albumSize = 11.0f;

        P.progX = P.pad + 36;
        P.progW = g_PopupW - 2 * (P.pad + 36);
        P.progH = 4;
        P.progY = P.artY + P.artSize + 20;
        P.timeLabelY = P.progY + P.progH / 2 - 8;

        P.btnRadius = 15.0f; P.playRadius = 20.0f;
        P.iconSize = 16.0f;  P.playIconSize = 20.0f;
        float gap = 46.0f;
        P.btnCenterY = (float)(P.progY + P.progH + 12) + P.playRadius;

        float rowW = gap * 4;
        float startX = (g_PopupW - rowW) / 2.0f;
        P.btnShuffleX = startX;
        P.btnPrevX    = startX + gap;
        P.btnPlayX    = startX + gap * 2;
        P.btnNextX    = startX + gap * 3;
        P.btnRepeatX  = startX + gap * 4;

        g_PopupH = (int)(P.btnCenterY + P.playRadius) + 14;
    }

    g_Fonts.Rebuild();
}

// ═══════════════════════════════════════════════════════════════════════════════
// Theme & Color Utilities
// ═══════════════════════════════════════════════════════════════════════════════

static bool IsSystemLightMode() {
    DWORD value = 0, size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"SystemUsesLightTheme", RRF_RT_DWORD, nullptr, &value, &size) == ERROR_SUCCESS)
        return value != 0;
    return false;
}

static DWORD GetCurrentTextColor() {
    if (g_Settings.autoTheme)
        return IsSystemLightMode() ? 0xFF000000 : 0xFFFFFFFF;
    return g_Settings.manualTextColor;
}

static bool IsDarkSurface() {
    if (g_Settings.autoTheme) return !IsSystemLightMode();
    DWORD c = g_Settings.manualTextColor;
    int lum = (int)((((c >> 16) & 0xFF) * 299 + ((c >> 8) & 0xFF) * 587 + (c & 0xFF) * 114) / 1000);
    return lum > 128;
}

static Color GetAccentGdiColor() {
    if (g_Settings.useAccentColor) {
        DWORD c = 0; BOOL o = FALSE;
        if (SUCCEEDED(DwmGetColorizationColor(&c, &o)))
            return Color(255, (BYTE)((c>>16)&0xFF), (BYTE)((c>>8)&0xFF), (BYTE)(c&0xFF));
        return Color(255, 0, 120, 212);
    }
    DWORD c = g_Settings.accentColorRGB;
    return Color(255, (BYTE)((c>>16)&0xFF), (BYTE)((c>>8)&0xFF), (BYTE)(c&0xFF));
}

static wstring FormatTime(double seconds) {
    if (seconds < 0) seconds = 0;
    int t = (int)seconds;
    wchar_t buf[16];
    swprintf_s(buf, L"%d:%02d", t / 60, t % 60);
    return buf;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Rounded Rectangle Helper
// ═══════════════════════════════════════════════════════════════════════════════

static void AddRoundedRect(GraphicsPath& path, int x, int y, int w, int h, int r) {
    if (r < 1) r = 1;
    if (w < 2 * r) r = w / 2;
    if (h < 2 * r) r = h / 2;
    if (r < 1) r = 1;
    int d = r * 2;
    path.AddArc(x, y, d, d, 180, 90);
    path.AddArc(x + w - d, y, d, d, 270, 90);
    path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    path.AddArc(x, y + h - d, d, d, 90, 90);
    path.CloseFigure();
}

// ═══════════════════════════════════════════════════════════════════════════════
// Window Appearance
// ═══════════════════════════════════════════════════════════════════════════════

static DWORD MakeTint(BYTE a, BYTE r, BYTE g, BYTE b) {
    return ((DWORD)a << 24) | ((DWORD)b << 16) | ((DWORD)g << 8) | (DWORD)r;
}

static DWORD GetAcrylicTint() {
    bool dark = g_Theme.dark;
    switch (g_Settings.theme) {
    case THEME_MICA:
        return dark ? MakeTint(0xD8, 0x1E, 0x1E, 0x1E) : MakeTint(0xE0, 0xF2, 0xF2, 0xF2);
    case THEME_GLASS:
        return dark ? MakeTint(0x35, 0x15, 0x1C, 0x24) : MakeTint(0x40, 0xFF, 0xFF, 0xFF);
    default:
        if (g_Settings.autoTheme)
            return dark ? MakeTint(0x55, 0, 0, 0) : MakeTint(0x55, 0xFF, 0xFF, 0xFF);
        return MakeTint((BYTE)g_Settings.bgOpacity, 0xFF, 0xFF, 0xFF);
    }
}

static void ApplyWindowShape(HWND hwnd) {
    if (!hwnd) return;
    SetWindowRgn(hwnd, NULL, TRUE);
    DWM_WINDOW_CORNER_PREFERENCE pref = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref));
}

static void UpdateAppearance(HWND hwnd) {
    if (!hwnd) return;
    ApplyWindowShape(hwnd);

    HMODULE hUser = GetModuleHandle(L"user32.dll");
    if (!hUser) return;
    auto SetComp = (pSetWindowCompositionAttribute)GetProcAddress(hUser, "SetWindowCompositionAttribute");
    if (!SetComp) return;

    ACCENT_POLICY policy = {};
    if (g_Settings.theme == THEME_SOLID || g_Settings.theme == THEME_TRANSPARENT) {
        policy.AccentState = ACCENT_DISABLED;
    } else {
        policy.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
        policy.GradientColor = GetAcrylicTint();
    }
    WINDOWCOMPOSITIONATTRIBDATA data = { WCA_ACCENT_POLICY, &policy, sizeof(ACCENT_POLICY) };
    SetComp(hwnd, &data);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Surface Painting
// ═══════════════════════════════════════════════════════════════════════════════

static void SetupGfx(Graphics& gfx) {
    gfx.SetSmoothingMode(SmoothingModeAntiAlias);
    gfx.SetTextRenderingHint(TextRenderingHintAntiAlias);
    gfx.SetInterpolationMode(InterpolationModeHighQualityBicubic);
    gfx.SetPixelOffsetMode(PixelOffsetModeHighQuality);
}

static void DrawSurface(Graphics& gfx, int w, int h, int radius) {
    if (g_Settings.theme == THEME_TRANSPARENT) return;

    bool dark = g_Theme.dark;
    int theme = g_Settings.theme;
    GraphicsPath sp; AddRoundedRect(sp, 0, 0, w, h, radius);

    if (theme == THEME_SOLID) {
        Color bg = dark ? Color(255, 0x24, 0x24, 0x24) : Color(255, 0xF5, 0xF5, 0xF5);
        SolidBrush b(bg); gfx.FillPath(&b, &sp);
    } else if (theme == THEME_GLASS) {
        Color bg = dark ? Color(130, 18, 22, 28) : Color(160, 240, 240, 240);
        SolidBrush b(bg); gfx.FillPath(&b, &sp);
        LinearGradientBrush sheen(Point(0, 0), Point(0, h),
            Color(dark ? 38 : 90, 255, 255, 255), Color(0, 255, 255, 255));
        gfx.FillPath(&sheen, &sp);
    } else if (theme == THEME_MICA) {
        Color bg = dark ? Color(220, 0x1E, 0x1E, 0x1E) : Color(220, 0xF0, 0xF0, 0xF0);
        SolidBrush b(bg); gfx.FillPath(&b, &sp);
    } else {
        BYTE alpha = g_Settings.autoTheme ? (dark ? 150 : 170) : (BYTE)g_Settings.bgOpacity;
        Color bg = dark ? Color(alpha, 16, 20, 25) : Color(alpha, 245, 245, 245);
        SolidBrush b(bg); gfx.FillPath(&b, &sp);
    }

    BYTE ba = (theme == THEME_GLASS) ? (dark ? 70 : 90) : (dark ? 28 : 32);
    Color bc = dark ? Color(ba, 255, 255, 255) : Color(ba, 0, 0, 0);
    Pen pen(bc, 1.0f);
    GraphicsPath bp; AddRoundedRect(bp, 0, 0, w - 1, h - 1, radius);
    gfx.DrawPath(&pen, &bp);
}

static void DrawPillSurface(Graphics& gfx, int x, int y, int w, int h, int radius, Color mainColor) {
    bool dark = g_Theme.dark;
    int theme = g_Settings.theme;
    GraphicsPath sp; AddRoundedRect(sp, x, y, w, h, radius);

    if (theme == THEME_GLASS) {
        SolidBrush bg(Color(dark ? 120 : 160, dark ? 18 : 240, dark ? 22 : 240, dark ? 28 : 240));
        gfx.FillPath(&bg, &sp);
        LinearGradientBrush sheen(Point(x, y), Point(x, y + h),
            Color(dark ? 40 : 80, 255, 255, 255), Color(0, 255, 255, 255));
        gfx.FillPath(&sheen, &sp);
        Pen pen(Color(dark ? 50 : 65, 255, 255, 255), 1.0f);
        GraphicsPath bp; AddRoundedRect(bp, x, y, w - 1, h - 1, radius);
        gfx.DrawPath(&pen, &bp);
    } else if (theme == THEME_SOLID) {
        Color bg = dark ? Color(255, 0x22, 0x22, 0x22) : Color(255, 0xF5, 0xF5, 0xF5);
        SolidBrush b(bg); gfx.FillPath(&b, &sp);
        Pen pen(dark ? Color(35, 255, 255, 255) : Color(25, 0, 0, 0), 1.0f);
        GraphicsPath bp; AddRoundedRect(bp, x, y, w - 1, h - 1, radius);
        gfx.DrawPath(&pen, &bp);
    } else if (theme == THEME_TRANSPARENT) {
        Pen pen(dark ? Color(25, 255, 255, 255) : Color(20, 0, 0, 0), 1.0f);
        GraphicsPath bp; AddRoundedRect(bp, x, y, w - 1, h - 1, radius);
        gfx.DrawPath(&pen, &bp);
    } else {
        BYTE alpha = g_Settings.autoTheme ? (dark ? 150 : 170) : (BYTE)g_Settings.bgOpacity;
        Color bg = dark ? Color(alpha, 16, 20, 25) : Color(alpha, 245, 245, 245);
        SolidBrush b(bg); gfx.FillPath(&b, &sp);
        Pen pen(dark ? Color(35, 255, 255, 255) : Color(25, 0, 0, 0), 1.0f);
        GraphicsPath bp; AddRoundedRect(bp, x, y, w - 1, h - 1, radius);
        gfx.DrawPath(&pen, &bp);
    }
}

static void FillBtnShape(Graphics& gfx, Brush* br, float cx, float cy, float r) {
    if (IsRounded()) {
        gfx.FillEllipse(br, cx - r, cy - r, r * 2, r * 2);
    } else {
        GraphicsPath p;
        AddRoundedRect(p, (int)(cx - r), (int)(cy - r), (int)(r * 2), (int)(r * 2), 4);
        gfx.FillPath(br, &p);
    }
}

static void DrawArtCover(Graphics& gfx, Bitmap* bmp, int x, int y, int w, int h, ImageAttributes* ia) {
    if (!bmp || w <= 0 || h <= 0) return;
    int bw = (int)bmp->GetWidth(), bh = (int)bmp->GetHeight();
    if (bw <= 0 || bh <= 0) return;
    double srcAspect = (double)bw / bh, dstAspect = (double)w / h;
    int sx = 0, sy = 0, sw = bw, sh = bh;
    if (srcAspect > dstAspect) { sw = (int)(bh * dstAspect); sx = (bw - sw) / 2; }
    else                       { sh = (int)(bw / dstAspect); sy = (bh - sh) / 2; }
    gfx.DrawImage(bmp, Rect(x, y, w, h), sx, sy, sw, sh, UnitPixel, ia);
}

// ═══════════════════════════════════════════════════════════════════════════════
// WinRT Media Session
// ═══════════════════════════════════════════════════════════════════════════════

static Bitmap* StreamToBitmap(IRandomAccessStreamWithContentType const& stream) {
    if (!stream) return nullptr;
    IStream* ns = nullptr;
    if (SUCCEEDED(CreateStreamOverRandomAccessStream(
            reinterpret_cast<IUnknown*>(winrt::get_abi(stream)), IID_PPV_ARGS(&ns)))) {
        Bitmap* bmp = Bitmap::FromStream(ns);
        ns->Release();
        if (bmp && bmp->GetLastStatus() == Ok) return bmp;
        delete bmp;
    }
    return nullptr;
}

static void UpdateMediaInfo() {
    try {
        if (!g_SessionManager)
            g_SessionManager = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        if (!g_SessionManager) return;

        GlobalSystemMediaTransportControlsSession session = nullptr;
        bool foundActive = false;
        for (auto const& s : g_SessionManager.GetSessions()) {
            auto pb = s.GetPlaybackInfo();
            if (pb && pb.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing) {
                session = s; foundActive = true; break;
            }
        }
        if (!foundActive) session = g_SessionManager.GetCurrentSession();
        { std::lock_guard<std::mutex> lk(g_SessionLock); g_CurrentSession = session; }

        if (session) {
            auto props = session.TryGetMediaPropertiesAsync().get();
            auto info  = session.GetPlaybackInfo();
            lock_guard<mutex> guard(g_MediaState.lock);

            wstring newTitle = props.Title().c_str();
            if (newTitle != g_MediaState.title || !g_MediaState.albumArt) {
                g_MediaState.albumArt.reset();
                auto thumbRef = props.Thumbnail();
                if (thumbRef) {
                    try {
                        Bitmap* raw = StreamToBitmap(thumbRef.OpenReadAsync().get());
                        if (raw) g_MediaState.albumArt.reset(raw);
                    } catch (...) {}
                }
                g_ResetScrollRequested.store(true);
                // [FIX] New track → force a clean re-anchor on the next poll.
                g_MediaState.positionSeconds = 0.0;
                g_MediaState.positionTick    = 0;
                g_MediaState.lastPolledPos   = -1.0;
                g_MediaState.lastPlayingState = false;
            }
            g_MediaState.title     = newTitle;
            g_MediaState.artist    = props.Artist().c_str();
            g_MediaState.album     = props.AlbumTitle().c_str();
            g_MediaState.isPlaying = (info.PlaybackStatus() == GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
            g_MediaState.hasMedia  = true;

            try {
                auto tl = session.GetTimelineProperties();
                double s0 = (double)tl.StartTime().count() / 10000000.0;
                double s1 = (double)tl.EndTime().count()   / 10000000.0;
                double sp = (double)tl.Position().count()  / 10000000.0;
                double dur = s1 - s0;
                double pos = sp - s0;
                if (dur > 0.0) {
                    g_MediaState.durationSeconds = dur;

                    // [FIX] On play→pause transition, capture the currently
                    //       interpolated position so the bar doesn't snap back
                    //       to the last anchor when the player fails to publish.
                    bool nowPlaying = g_MediaState.isPlaying;
                    if (!nowPlaying && g_MediaState.lastPlayingState &&
                        g_MediaState.positionTick > 0) {
                        double cur = g_MediaState.positionSeconds +
                            (double)(GetTickCount64() - g_MediaState.positionTick) / 1000.0;
                        if (cur > g_MediaState.durationSeconds) cur = g_MediaState.durationSeconds;
                        g_MediaState.positionSeconds = cur;
                        g_MediaState.positionTick    = GetTickCount64();
                    }
                    g_MediaState.lastPlayingState = nowPlaying;

                    // [FIX] Only re-anchor when the player reports a NEW value.
                    //       If the value is identical to the previous poll, the
                    //       player is stale (many players publish only on seek /
                    //       pause / track change) → keep the old anchor and let
                    //       the UI clock interpolate forward.
                    bool posChanged =
                        (g_MediaState.lastPolledPos < 0.0) ||
                        (fabs(pos - g_MediaState.lastPolledPos) > 0.10);

                    if (posChanged) {
                        g_MediaState.positionSeconds = pos < 0 ? 0 : pos;
                        g_MediaState.positionTick    = GetTickCount64();
                    }
                    g_MediaState.lastPolledPos = pos;
                }
            } catch (...) { /* keep previous values */ }

            try {
                auto sr = info.IsShuffleActive();
                g_MediaState.shuffleAvailable = (sr != nullptr);
                g_MediaState.shuffleActive    = sr ? sr.Value() : false;
            } catch (...) { g_MediaState.shuffleAvailable = false; }

            try {
                auto rr = info.AutoRepeatMode();
                g_MediaState.repeatAvailable = (rr != nullptr);
                if (rr) {
                    auto m = rr.Value();
                    g_MediaState.repeatMode = (m == MediaPlaybackAutoRepeatMode::List) ? 1
                                            : (m == MediaPlaybackAutoRepeatMode::Track) ? 2 : 0;
                } else g_MediaState.repeatMode = 0;
            } catch (...) { g_MediaState.repeatAvailable = false; }
        } else {
            { std::lock_guard<std::mutex> lk(g_SessionLock); g_CurrentSession = nullptr; }
            lock_guard<mutex> guard(g_MediaState.lock);
            g_MediaState.hasMedia = false;
            g_MediaState.title = L"No Media"; g_MediaState.artist = L""; g_MediaState.album = L"";
            g_MediaState.durationSeconds = 0; g_MediaState.positionSeconds = 0;
            g_MediaState.albumArt.reset();
        }
    } catch (...) {
        { std::lock_guard<std::mutex> lk(g_SessionLock); g_CurrentSession = nullptr; }
        lock_guard<mutex> guard(g_MediaState.lock);
        g_MediaState.hasMedia = false;
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Media Commands
// ═══════════════════════════════════════════════════════════════════════════════

static void SendMediaCommand(int cmd) {
    try {
        GlobalSystemMediaTransportControlsSession s = nullptr;
        { std::lock_guard<std::mutex> lk(g_SessionLock); s = g_CurrentSession; }
        if (!s) return;
        if (cmd == 1) s.TrySkipPreviousAsync();
        else if (cmd == 2) s.TryTogglePlayPauseAsync();
        else if (cmd == 3) s.TrySkipNextAsync();
    } catch (...) {}
}

static void SeekToRatio(double ratio) {
    try {
        GlobalSystemMediaTransportControlsSession s = nullptr;
        { std::lock_guard<std::mutex> lk(g_SessionLock); s = g_CurrentSession; }
        if (!s) return;
        auto tl = s.GetTimelineProperties();
        int64_t start = tl.StartTime().count(), end = tl.EndTime().count();
        if (end <= start) return;
        int64_t pos = start + (int64_t)(ratio * (double)(end - start));
        if (pos < start) pos = start;
        if (pos > end) pos = end;
        s.TryChangePlaybackPositionAsync(pos);
    } catch (...) {}
}

static void ToggleShuffle() {
    try {
        GlobalSystemMediaTransportControlsSession s = nullptr;
        { std::lock_guard<std::mutex> lk(g_SessionLock); s = g_CurrentSession; }
        if (!s) return;
        bool cur;
        { lock_guard<mutex> g(g_MediaState.lock); cur = g_MediaState.shuffleActive; }
        s.TryChangeShuffleActiveAsync(!cur);
    } catch (...) {}
}

static void CycleRepeatMode() {
    try {
        GlobalSystemMediaTransportControlsSession s = nullptr;
        { std::lock_guard<std::mutex> lk(g_SessionLock); s = g_CurrentSession; }
        if (!s) return;
        int cur;
        { lock_guard<mutex> g(g_MediaState.lock); cur = g_MediaState.repeatMode; }
        MediaPlaybackAutoRepeatMode next = (cur == 0) ? MediaPlaybackAutoRepeatMode::List
                                         : (cur == 1) ? MediaPlaybackAutoRepeatMode::Track
                                                      : MediaPlaybackAutoRepeatMode::None;
        s.TryChangeAutoRepeatModeAsync(next);
    } catch (...) {}
}

static void SetRepeatMode(int mode) {
    try {
        GlobalSystemMediaTransportControlsSession s = nullptr;
        { std::lock_guard<std::mutex> lk(g_SessionLock); s = g_CurrentSession; }
        if (!s) return;
        MediaPlaybackAutoRepeatMode m = (mode == 1) ? MediaPlaybackAutoRepeatMode::List
                                      : (mode == 2) ? MediaPlaybackAutoRepeatMode::Track
                                                    : MediaPlaybackAutoRepeatMode::None;
        s.TryChangeAutoRepeatModeAsync(m);
    } catch (...) {}
}

// ═══════════════════════════════════════════════════════════════════════════════
// Progress / Snapshot Helpers
// ═══════════════════════════════════════════════════════════════════════════════

static double GetInterpolatedProgress(double posSec, double durSec,
                                      ULONGLONG tick, bool playing) {
    if (durSec <= 0) return 0;
    double elapsed = (playing && tick > 0)
        ? (double)(GetTickCount64() - tick) / 1000.0 : 0;
    double cur = posSec + elapsed;
    if (cur < 0) cur = 0;
    if (cur > durSec) cur = durSec;
    return cur / durSec;
}

struct MediaSnap {
    wstring title, artist, album;
    bool isPlaying, hasMedia;
    BitmapPtr albumArt;   // [FIX] shared_ptr keeps bitmap alive for the whole paint
    double posSec, durSec;
    ULONGLONG posTick;
    bool shuffleActive, shuffleAvail;
    int repeatMode; bool repeatAvail;
};

static MediaSnap TakeSnapshot() {
    lock_guard<mutex> guard(g_MediaState.lock);
    MediaSnap s;
    s.title   = g_MediaState.title;
    s.artist  = g_MediaState.artist;
    s.album   = g_MediaState.album;
    s.albumArt = g_MediaState.albumArt;
    s.hasMedia   = g_MediaState.hasMedia;
    s.isPlaying  = g_MediaState.isPlaying;
    s.posSec     = g_MediaState.positionSeconds;
    s.durSec     = g_MediaState.durationSeconds;
    s.posTick    = g_MediaState.positionTick;
    s.shuffleActive = g_MediaState.shuffleActive;
    s.shuffleAvail  = g_MediaState.shuffleAvailable;
    s.repeatMode    = g_MediaState.repeatMode;
    s.repeatAvail   = g_MediaState.repeatAvailable;
    return s;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Hit Testing
// ═══════════════════════════════════════════════════════════════════════════════

static bool InCircle(float px, float py, float cx, float cy, float r) {
    float dx = px - cx, dy = py - cy;
    return dx*dx + dy*dy <= r*r;
}

static HitTarget HitTestCompact(int x, int y) {
    auto& L = g_Layout;
    float fx = (float)x, fy = (float)y;
    if (InCircle(fx, fy, L.btnPlayX, L.btnCenterY, L.playRadius)) return HIT_PLAYPAUSE;
    if (InCircle(fx, fy, L.btnPrevX, L.btnCenterY, L.btnRadius + 3.0f)) return HIT_PREV;
    if (InCircle(fx, fy, L.btnNextX, L.btnCenterY, L.btnRadius + 3.0f)) return HIT_NEXT;
    if (g_Settings.showShuffleRepeat) {
        if (InCircle(fx, fy, L.btnShuffleX, L.btnCenterY, L.btnRadius + 2.0f)) return HIT_SHUFFLE;
        if (InCircle(fx, fy, L.btnRepeatX,  L.btnCenterY, L.btnRadius + 2.0f)) return HIT_REPEAT;
    }
    if (L.progVisible && x >= L.progX && x <= L.progX + L.progW
                      && y >= L.progY - 6 && y <= L.progY + L.progH + 6)
        return HIT_PROGRESS;
    return HIT_NONE;
}

static PopupHitTarget HitTestPopup(int x, int y) {
    auto& P = g_PopupLayout;
    float fx = (float)x, fy = (float)y;
    if (InCircle(fx, fy, P.btnPlayX,    P.btnCenterY, P.playRadius)) return PHIT_PLAYPAUSE;
    if (InCircle(fx, fy, P.btnPrevX,    P.btnCenterY, P.btnRadius)) return PHIT_PREV;
    if (InCircle(fx, fy, P.btnNextX,    P.btnCenterY, P.btnRadius)) return PHIT_NEXT;
    if (InCircle(fx, fy, P.btnShuffleX, P.btnCenterY, P.btnRadius)) return PHIT_SHUFFLE;
    if (InCircle(fx, fy, P.btnRepeatX,  P.btnCenterY, P.btnRadius)) return PHIT_REPEAT;
    if (x >= P.progX && x <= P.progX + P.progW
     && y >= P.progY - 6 && y <= P.progY + P.progH + 6)
        return PHIT_PROGRESS;
    return PHIT_NONE;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Reusable GDI Back Buffer (avoids churning a bitmap every frame)
// ═══════════════════════════════════════════════════════════════════════════════

struct BackBuffer {
    HDC      hdc    = nullptr;
    HBITMAP  bmp    = nullptr;
    HBITMAP  oldBmp = nullptr;
    int      w = 0, h = 0;

    void Ensure(HWND hwnd, int width, int height) {
        if (hdc && w == width && h == height) return;
        Release();
        w = width; h = height;
        HDC screen = GetDC(hwnd);
        hdc = CreateCompatibleDC(screen);
        bmp = CreateCompatibleBitmap(screen, width, height);
        oldBmp = (HBITMAP)SelectObject(hdc, bmp);
        ReleaseDC(hwnd, screen);
    }
    void Release() {
        if (hdc) {
            SelectObject(hdc, oldBmp);
            DeleteObject(bmp);
            DeleteDC(hdc);
            hdc = nullptr; bmp = nullptr; oldBmp = nullptr; w = h = 0;
        }
    }
    ~BackBuffer() { Release(); }
};

static BackBuffer g_CompactBB;
static BackBuffer g_PopupBB;

// ═══════════════════════════════════════════════════════════════════════════════
// Drawing — Compact Panel
// ═══════════════════════════════════════════════════════════════════════════════

static void DrawMediaPanel(HDC hdc, int width, int height) {
    Graphics gfx(hdc);
    SetupGfx(gfx);
    gfx.Clear(Color(0, 0, 0, 0));

    DrawSurface(gfx, width, height, CompactCornerRadius());

    auto& L = g_Layout;
    bool rounded = IsRounded();
    Color mainColor = g_Theme.text;
    Color accentColor = g_Theme.accent;
    Color dimColor(145, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue());
    MediaSnap snap = TakeSnapshot();

    // ── 1. Album Art ─────────────────────────────────────────────────────────
    {
        GraphicsPath artClip;
        AddRoundedRect(artClip, L.artX, L.artY, L.artSize, L.artSize, L.artRadius);

        if (snap.albumArt) {
            gfx.SetClip(&artClip);
            DrawArtCover(gfx, snap.albumArt.get(), L.artX, L.artY, L.artSize, L.artSize, nullptr);
            gfx.ResetClip();

            Pen artBorder(Color(25, 255, 255, 255), 1.0f);
            gfx.DrawPath(&artBorder, &artClip);
        } else {
            SolidBrush pb(Color(24, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue()));
            gfx.FillPath(&pb, &artClip);

            Font nf(g_Fonts.iconFam.get(), (REAL)(L.artSize * 0.40f), FontStyleRegular, UnitPixel);
            SolidBrush nb(Color(80, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue()));
            StringFormat sf; sf.SetAlignment(StringAlignmentCenter); sf.SetLineAlignment(StringAlignmentCenter);
            RectF ar((REAL)L.artX, (REAL)L.artY, (REAL)L.artSize, (REAL)L.artSize);
            gfx.SetClip(&artClip);
            gfx.DrawString(ICON_MUSIC_NOTE, 1, &nf, ar, &sf, &nb);
            gfx.ResetClip();

            Pen artBorder(Color(20, 255, 255, 255), 1.0f);
            gfx.DrawPath(&artBorder, &artClip);
        }
    }

    // ── 2. Pill Container ────────────────────────────────────────────────────
    if (L.pillVisible) {
        int innerPillRadius = L.pillH / 2;
        DrawPillSurface(gfx, L.pillX, L.pillY, L.pillW, L.pillH, innerPillRadius, mainColor);
    }

    // ── 3. Media Buttons ─────────────────────────────────────────────────────
    {
        Font* iconFont = g_Fonts.iconBar.get();

        SolidBrush normalBr(mainColor), accentBr(accentColor), dimBr(dimColor);
        SolidBrush hoverBg(Color(35, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue()));
        StringFormat sf; sf.SetAlignment(StringAlignmentCenter); sf.SetLineAlignment(StringAlignmentCenter);

        auto drawBtn = [&](float cx, float cy, float r, const wchar_t* icon, HitTarget tgt,
                           bool active = false, bool dim = false, bool isPlay = false) {
            bool hov = (g_HoverState == tgt);
            if (hov) FillBtnShape(gfx, &hoverBg, cx, cy, r + 2.0f);

            float xOffset = (isPlay && !snap.isPlaying) ? -0.8f : 0.0f;
            RectF rc(cx - r + xOffset, cy - r, r * 2, r * 2);
            Brush* br = active ? (Brush*)&accentBr : dim ? (Brush*)&dimBr : (Brush*)&normalBr;
            gfx.DrawString(icon, -1, iconFont, rc, &sf, br);
        };

        if (g_Settings.showShuffleRepeat)
            drawBtn(L.btnShuffleX, L.btnCenterY, L.btnRadius, ICON_SHUFFLE, HIT_SHUFFLE,
                    snap.shuffleActive, !snap.shuffleAvail);

        drawBtn(L.btnPrevX, L.btnCenterY, L.btnRadius, ICON_PREVIOUS, HIT_PREV);
        drawBtn(L.btnPlayX, L.btnCenterY, L.playRadius, snap.isPlaying ? ICON_PAUSE : ICON_PLAY,
                HIT_PLAYPAUSE, false, false, true);
        drawBtn(L.btnNextX, L.btnCenterY, L.btnRadius, ICON_NEXT, HIT_NEXT);

        if (g_Settings.showShuffleRepeat) {
            const wchar_t* ri = (snap.repeatMode == 2) ? ICON_REPEAT_ONE : ICON_REPEAT_ALL;
            drawBtn(L.btnRepeatX, L.btnCenterY, L.btnRadius, ri, HIT_REPEAT,
                    snap.repeatMode > 0, !snap.repeatAvail);
        }
    }

    // ── 4. Title (with scrolling) ────────────────────────────────────────────
    {
        Font* titleFont = g_Fonts.titleBar.get();
        SolidBrush titleBr(mainColor);

        static wstring lastMeasuredTitle;
        static int     lastMeasuredWidth = 0;
        static REAL    lastMeasuredHeight = 0.0f;
        if (snap.title != lastMeasuredTitle) {
            RectF mr(0, 0, 4000, 200); RectF br;
            gfx.MeasureString(snap.title.c_str(), -1, titleFont, mr, &br);
            lastMeasuredTitle  = snap.title;
            lastMeasuredWidth  = (int)br.Width;
            lastMeasuredHeight = br.Height;
        }
        g_TextWidth = lastMeasuredWidth;

        Region textClip(Rect(L.textX, 0, L.textMaxW, height));
        gfx.SetClip(&textClip);

        float ty = L.progVisible
            ? (rounded ? (float)L.pillY : 0.0f) + (((float)L.progY - (rounded ? (float)L.pillY : 0.0f)) - lastMeasuredHeight) / 2.0f
            : (rounded ? (float)L.pillY + ((float)L.pillH - lastMeasuredHeight) / 2.0f : ((float)height - lastMeasuredHeight) / 2.0f);

        if (g_TextWidth > L.textMaxW) {
            g_IsScrolling = true;
            float dx = (float)(L.textX - g_ScrollOffset);
            int spacer = 50;
            gfx.DrawString(snap.title.c_str(), -1, titleFont, PointF(dx, ty), &titleBr);
            if (dx + g_TextWidth < L.textX + L.textMaxW + spacer)
                gfx.DrawString(snap.title.c_str(), -1, titleFont, PointF(dx + g_TextWidth + spacer, ty), &titleBr);
        } else {
            g_IsScrolling = false; g_ScrollOffset = 0;
            gfx.DrawString(snap.title.c_str(), -1, titleFont, PointF((float)L.textX, ty), &titleBr);
        }

        gfx.ResetClip();
    }
    // ── 5. Progress Bar ──────────────────────────────────────────────────────
    if (L.progVisible && snap.durSec > 0.0 && snap.hasMedia) {
        bool darkSurface = g_Theme.dark;
        double ratio = GetInterpolatedProgress(snap.posSec, snap.durSec, snap.posTick, snap.isPlaying);
        int fillW = (int)(ratio * L.progW);
        int tr = L.progH;

        GraphicsPath tp; AddRoundedRect(tp, L.progX, L.progY, L.progW, L.progH, tr);
        // Slightly more visible track so the fill is legible on dark themes.
        SolidBrush tbr(Color(darkSurface ? 60 : 40,
                             mainColor.GetRed(),
                             mainColor.GetGreen(),
                             mainColor.GetBlue()));
        gfx.FillPath(&tbr, &tp);

        if (fillW > 2) {
            GraphicsPath fp; AddRoundedRect(fp, L.progX, L.progY, fillW, L.progH, tr);
            SolidBrush fbr(accentColor); gfx.FillPath(&fbr, &fp);
        }

        if (g_HoverState == HIT_PROGRESS) {
            float tx = (float)(L.progX + fillW);
            float ty = (float)(L.progY + L.progH / 2.0f);
            float thumbR = (float)(L.progH + 2);
            SolidBrush thb(accentColor);
            gfx.FillEllipse(&thb, tx - thumbR, ty - thumbR, thumbR * 2, thumbR * 2);
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Drawing — Popup Player
// ═══════════════════════════════════════════════════════════════════════════════

static void DrawStackedLayer(Graphics& gfx, Bitmap* art, int x, int y, int w, int h, int r, float alpha) {
    GraphicsPath path;
    AddRoundedRect(path, x, y, w, h, r);
    if (art) {
        ColorMatrix cm = {{
            { 0.70f, 0, 0, 0, 0 },
            { 0, 0.70f, 0, 0, 0 },
            { 0, 0, 0.70f, 0, 0 },
            { 0, 0, 0, alpha, 0 },
            { 0, 0, 0, 0, 1 }
        }};
        ImageAttributes ia;
        ia.SetColorMatrix(&cm, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);
        gfx.SetClip(&path);
        DrawArtCover(gfx, art, x, y, w, h, &ia);
        gfx.ResetClip();
    } else {
        SolidBrush b(Color((BYTE)(60 * alpha), 128, 128, 128));
        gfx.FillPath(&b, &path);
    }
    Pen pen(Color(35, 255, 255, 255), 0.5f);
    gfx.DrawPath(&pen, &path);
}

static void DrawPopupPanel(HDC hdc) {
    Graphics gfx(hdc);
    SetupGfx(gfx);
    gfx.Clear(Color(0, 0, 0, 0));
    DrawSurface(gfx, g_PopupW, g_PopupH, PopupCornerRadius());

    auto& P = g_PopupLayout;
    Color mainColor = g_Theme.text;
    Color accentColor = g_Theme.accent;
    Color dimColor(140, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue());
    MediaSnap snap = TakeSnapshot();

    // ── 1. Album Art ─────────────────────────────────────────────────────────
    {
        Bitmap* artPtr = snap.albumArt.get();

        if (P.stacked) {
            int inset = 12;
            for (int k = 2; k >= 1; --k) {
                DrawStackedLayer(gfx, artPtr,
                    P.artX + k * inset, P.artY - k * P.stackStep,
                    P.artSize - 2 * k * inset, P.artSize, P.artRadius,
                    (k == 1) ? 0.55f : 0.30f);
            }
        }

        GraphicsPath artClip;
        AddRoundedRect(artClip, P.artX, P.artY, P.artSize, P.artSize, P.artRadius);
        if (artPtr) {
            gfx.SetClip(&artClip);
            DrawArtCover(gfx, artPtr, P.artX, P.artY, P.artSize, P.artSize, nullptr);
            gfx.ResetClip();
            Pen ap(Color(30, 255, 255, 255), 0.5f);
            gfx.DrawPath(&ap, &artClip);
        } else {
            SolidBrush pb(Color(18, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue()));
            gfx.FillPath(&pb, &artClip);
            Font nf(g_Fonts.iconFam.get(), (REAL)(P.artSize * 0.28f), FontStyleRegular, UnitPixel);
            SolidBrush nb(Color(45, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue()));
            StringFormat sf; sf.SetAlignment(StringAlignmentCenter); sf.SetLineAlignment(StringAlignmentCenter);
            RectF ar((REAL)P.artX, (REAL)P.artY, (REAL)P.artSize, (REAL)P.artSize);
            gfx.SetClip(&artClip);
            gfx.DrawString(ICON_MUSIC_NOTE, 1, &nf, ar, &sf, &nb);
            gfx.ResetClip();
        }
    }

    // ── 2. Title / Artist / Album ────────────────────────────────────────────
    {
        StringFormat sf;
        sf.SetAlignment(P.centered ? StringAlignmentCenter : StringAlignmentNear);
        sf.SetTrimming(StringTrimmingEllipsisCharacter);
        sf.SetFormatFlags(StringFormatFlagsNoWrap);

        Font* titleFont = g_Fonts.titlePopup.get();
        SolidBrush tb(mainColor);
        RectF trc((REAL)P.textX, (REAL)P.titleY, (REAL)P.textW, P.titleSize + 8.0f);
        gfx.DrawString(snap.title.c_str(), -1, titleFont, trc, &sf, &tb);

        Font* artistFont = g_Fonts.artistPopup.get();
        SolidBrush ab(Color(210, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue()));
        RectF arc((REAL)P.textX, (REAL)P.artistY, (REAL)P.textW, P.artistSize + 6.0f);
        gfx.DrawString(snap.artist.c_str(), -1, artistFont, arc, &sf, &ab);

        if (!snap.album.empty()) {
            Font* albumFont = g_Fonts.albumPopup.get();
            SolidBrush alb(dimColor);
            RectF alrc((REAL)P.textX, (REAL)P.albumY, (REAL)P.textW, P.albumSize + 6.0f);
            gfx.DrawString(snap.album.c_str(), -1, albumFont, alrc, &sf, &alb);
        }
    }

    // ── 3. Progress Bar + Timestamps ─────────────────────────────────────────
    {
        double ratio = GetInterpolatedProgress(snap.posSec, snap.durSec, snap.posTick, snap.isPlaying);
        double curSec = snap.durSec * ratio;

        Font* tf = g_Fonts.timePopup.get();
        SolidBrush timeBr(dimColor);

        wstring lt = FormatTime(curSec);
        StringFormat lsf; lsf.SetAlignment(StringAlignmentNear);
        gfx.DrawString(lt.c_str(), -1, tf, PointF((REAL)P.pad, (REAL)P.timeLabelY), &lsf, &timeBr);

        wstring rt = FormatTime(snap.durSec);
        StringFormat rsf; rsf.SetAlignment(StringAlignmentFar);
        RectF rrc(0.0f, (REAL)P.timeLabelY, (REAL)(g_PopupW - P.pad), 16.0f);
        gfx.DrawString(rt.c_str(), -1, tf, rrc, &rsf, &timeBr);

        int tr = P.progH;
        GraphicsPath tp; AddRoundedRect(tp, P.progX, P.progY, P.progW, P.progH, tr);
        SolidBrush trackBr(Color(35, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue()));
        gfx.FillPath(&trackBr, &tp);

        int fillW = (int)(ratio * P.progW);
        if (fillW > 2) {
            GraphicsPath fp; AddRoundedRect(fp, P.progX, P.progY, fillW, P.progH, tr);
            SolidBrush fillBr(accentColor); gfx.FillPath(&fillBr, &fp);
        }

        float tx = (float)(P.progX + fillW);
        float ty = (float)(P.progY + P.progH / 2);
        float thumbR = (float)(P.progH + 2);
        SolidBrush thb(accentColor);
        gfx.FillEllipse(&thb, tx - thumbR, ty - thumbR, thumbR * 2, thumbR * 2);
    }

    // ── 4. Controls Row ──────────────────────────────────────────────────────
    {
        Font* iconFont = g_Fonts.iconPopup.get();
        Font* playFont = g_Fonts.playPopup.get();
        SolidBrush normalBr(mainColor), accentBr(accentColor), dimBr(dimColor);
        SolidBrush hoverBg(Color(35, mainColor.GetRed(), mainColor.GetGreen(), mainColor.GetBlue()));
        StringFormat sf; sf.SetAlignment(StringAlignmentCenter); sf.SetLineAlignment(StringAlignmentCenter);

        auto drawBtn = [&](float cx, float cy, float r, const wchar_t* icon, PopupHitTarget tgt,
                           Font* f, bool active = false, bool dim = false, bool isPlay = false) {
            bool hov = (g_PopupHover == tgt);
            if (hov) FillBtnShape(gfx, &hoverBg, cx, cy, r + 2.0f);

            float xOffset = (isPlay && !snap.isPlaying) ? -0.8f : 0.0f;
            RectF rc(cx - r + xOffset, cy - r, r * 2, r * 2);
            Brush* br = active ? (Brush*)&accentBr : dim ? (Brush*)&dimBr : (Brush*)&normalBr;
            gfx.DrawString(icon, -1, f, rc, &sf, br);
        };

        drawBtn(P.btnShuffleX, P.btnCenterY, P.btnRadius, ICON_SHUFFLE, PHIT_SHUFFLE, iconFont,
                snap.shuffleActive, !snap.shuffleAvail);
        drawBtn(P.btnPrevX, P.btnCenterY, P.btnRadius, ICON_PREVIOUS, PHIT_PREV, iconFont);
        drawBtn(P.btnPlayX, P.btnCenterY, P.playRadius, snap.isPlaying ? ICON_PAUSE : ICON_PLAY,
                PHIT_PLAYPAUSE, playFont, false, false, true);
        drawBtn(P.btnNextX, P.btnCenterY, P.btnRadius, ICON_NEXT, PHIT_NEXT, iconFont);
        const wchar_t* ri = (snap.repeatMode == 2) ? ICON_REPEAT_ONE : ICON_REPEAT_ALL;
        drawBtn(P.btnRepeatX, P.btnCenterY, P.btnRadius, ri, PHIT_REPEAT, iconFont,
                snap.repeatMode > 0, !snap.repeatAvail);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Popup Show / Hide
// ═══════════════════════════════════════════════════════════════════════════════

static void ShowPopup() {
    if (!g_hPopupWindow || !g_hMediaWindow) return;

    RECT rc;
    GetWindowRect(g_hMediaWindow, &rc);

    int barW = rc.right - rc.left;
    int x = rc.left + (barW - g_PopupW) / 2;
    int y = rc.top - g_PopupH - POPUP_GAP;

    HMONITOR hMon = MonitorFromWindow(g_hMediaWindow, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (GetMonitorInfo(hMon, &mi)) {
        if (y < mi.rcWork.top) y = mi.rcWork.top + 4;
        if (x + g_PopupW > mi.rcWork.right) x = mi.rcWork.right - g_PopupW - 4;
        if (x < mi.rcWork.left) x = mi.rcWork.left + 4;
    }

    SetWindowPos(g_hPopupWindow, HWND_TOPMOST, x, y, g_PopupW, g_PopupH, SWP_NOACTIVATE);
    UpdateAppearance(g_hPopupWindow);
    ShowWindow(g_hPopupWindow, SW_SHOWNOACTIVATE);
    SetForegroundWindow(g_hPopupWindow);
    g_PopupVisible = true;
    InvalidateRect(g_hPopupWindow, NULL, TRUE);
}

static void HidePopup() {
    if (!g_hPopupWindow) return;
    ShowWindow(g_hPopupWindow, SW_HIDE);
    g_PopupVisible = false;
    g_PopupHideTimestamp = GetTickCount64();
}

static void TogglePopup() {
    if (g_PopupVisible) {
        HidePopup();
    } else {
        if (GetTickCount64() - g_PopupHideTimestamp > 250)
            ShowPopup();
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Context Menu
// ═══════════════════════════════════════════════════════════════════════════════

static void ShowContextMenu(HWND hwnd, int sx, int sy) {
    bool playing = false, shuffle = false; int rep = 0;
    { lock_guard<mutex> g(g_MediaState.lock);
      playing = g_MediaState.isPlaying; shuffle = g_MediaState.shuffleActive; rep = g_MediaState.repeatMode; }

    HMENU hm = CreatePopupMenu();
    AppendMenuW(hm, MF_STRING, IDM_PLAYPAUSE, playing ? L"Pause" : L"Play");
    AppendMenuW(hm, MF_STRING, IDM_PREV, L"Previous Track");
    AppendMenuW(hm, MF_STRING, IDM_NEXT, L"Next Track");
    AppendMenuW(hm, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hm, MF_STRING | (shuffle ? MF_CHECKED : 0), IDM_SHUFFLE, L"Shuffle");
    AppendMenuW(hm, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hm, MF_STRING | (rep == 0 ? MF_CHECKED : 0), IDM_REPEAT_OFF, L"Repeat Off");
    AppendMenuW(hm, MF_STRING | (rep == 1 ? MF_CHECKED : 0), IDM_REPEAT_ALL, L"Repeat All");
    AppendMenuW(hm, MF_STRING | (rep == 2 ? MF_CHECKED : 0), IDM_REPEAT_ONE, L"Repeat One");
    SetForegroundWindow(hwnd);
    TrackPopupMenu(hm, TPM_RIGHTBUTTON, sx, sy, 0, hwnd, NULL);
    DestroyMenu(hm);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Taskbar Event Hook
// ═══════════════════════════════════════════════════════════════════════════════

static bool IsTaskbarWindow(HWND hwnd) {
    if (!hwnd) return false;
    WCHAR cls[64]; GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    return wcscmp(cls, L"Shell_TrayWnd") == 0;
}

static void CALLBACK TaskbarEventProc(HWINEVENTHOOK, DWORD, HWND hwnd, LONG, LONG, DWORD, DWORD) {
    if (!IsTaskbarWindow(hwnd) || !g_hMediaWindow) return;
    PostMessage(g_hMediaWindow, WM_APP + 10, 0, 0);
}

static void RegisterTaskbarHook(HWND hwnd) {
    HWND hTB = FindWindow(L"Shell_TrayWnd", nullptr);
    if (hTB) { DWORD pid; DWORD tid = GetWindowThreadProcessId(hTB, &pid);
        if (tid) g_TaskbarHook = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
                                                  nullptr, TaskbarEventProc, pid, tid,
                                                  WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    }
    PostMessage(hwnd, WM_APP + 10, 0, 0);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Window Procedure — Popup Player
// ═══════════════════════════════════════════════════════════════════════════════

static LRESULT CALLBACK PopupWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_ERASEBKGND: return 1;
    case WM_ACTIVATE:
        if (LOWORD(wParam) == WA_INACTIVE) HidePopup();
        return 0;
    case WM_SETTINGCHANGE:
        UpdateAppearance(hwnd);
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    case WM_DWMCOLORIZATIONCOLORCHANGED:
        g_Theme.Refresh();
        UpdateAppearance(hwnd);
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    case WM_MOUSEMOVE: {
        PopupHitTarget nh = HitTestPopup(LOWORD(lParam), HIWORD(lParam));
        if (nh != g_PopupHover) { g_PopupHover = nh; InvalidateRect(hwnd, NULL, FALSE); }
        TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 }; TrackMouseEvent(&tme);
        return 0;
    }
    case WM_MOUSELEAVE:
        g_PopupHover = PHIT_NONE; InvalidateRect(hwnd, NULL, FALSE); return 0;
    case WM_LBUTTONUP: {
        // Re-hit-test at click time to avoid stale hover state.
        PopupHitTarget hit = HitTestPopup(LOWORD(lParam), HIWORD(lParam));
        int x = LOWORD(lParam);
        switch (hit) {
        case PHIT_PREV:      SendMediaCommand(1); break;
        case PHIT_PLAYPAUSE: SendMediaCommand(2); break;
        case PHIT_NEXT:      SendMediaCommand(3); break;
        case PHIT_SHUFFLE:   ToggleShuffle();     break;
        case PHIT_REPEAT:    CycleRepeatMode();   break;
        case PHIT_PROGRESS:
            if (g_PopupLayout.progW > 0) {
                double r = (double)(x - g_PopupLayout.progX) / (double)g_PopupLayout.progW;
                if (r < 0) r = 0; if (r > 1) r = 1;
                SeekToRatio(r);
            }
            break;
        default: break;
        }
        return 0;
    }
    case WM_RBUTTONUP: {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        ClientToScreen(hwnd, &pt);
        ShowContextMenu(hwnd, pt.x, pt.y);
        return 0;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_PLAYPAUSE: SendMediaCommand(2); break;
        case IDM_PREV:      SendMediaCommand(1); break;
        case IDM_NEXT:      SendMediaCommand(3); break;
        case IDM_SHUFFLE:   ToggleShuffle();     break;
        case IDM_REPEAT_OFF: SetRepeatMode(0);   break;
        case IDM_REPEAT_ALL: SetRepeatMode(1);   break;
        case IDM_REPEAT_ONE: SetRepeatMode(2);   break;
        }
        return 0;
    case WM_MOUSEWHEEL: {
        BYTE vk = (GET_WHEEL_DELTA_WPARAM(wParam) > 0) ? VK_VOLUME_UP : VK_VOLUME_DOWN;
        keybd_event(vk, 0, 0, 0); keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);
        return 0;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc; GetClientRect(hwnd, &rc);
        g_PopupBB.Ensure(hwnd, rc.right, rc.bottom);
        DrawPopupPanel(g_PopupBB.hdc);
        BitBlt(hdc, 0, 0, rc.right, rc.bottom, g_PopupBB.hdc, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
        return 0;
    }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Window Procedure — Compact Panel
// ═══════════════════════════════════════════════════════════════════════════════

static LRESULT CALLBACK MediaWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        UpdateAppearance(hwnd);
        SetTimer(hwnd, IDT_POLL_MEDIA, 1000, NULL);
        SetTimer(hwnd, IDT_ANIMATION, 33, NULL);
        RegisterTaskbarHook(hwnd);
        return 0;
    case WM_ERASEBKGND: return 1;
    case WM_CLOSE: return 0;
    case APP_WM_CLOSE:
        HidePopup(); DestroyWindow(hwnd); return 0;
    case WM_DESTROY:
        if (g_TaskbarHook) { UnhookWinEvent(g_TaskbarHook); g_TaskbarHook = nullptr; }
        { std::lock_guard<std::mutex> lk(g_SessionLock); g_CurrentSession = nullptr; }
        g_SessionManager = nullptr;
        PostQuitMessage(0); return 0;
    case WM_SETTINGCHANGE:
        HidePopup();
        ComputeLayout();
        ComputePopupLayout();
        g_Theme.Refresh();
        UpdateAppearance(hwnd);
        if (g_hPopupWindow) { UpdateAppearance(g_hPopupWindow); InvalidateRect(g_hPopupWindow, NULL, TRUE); }
        g_ScrollOffset = 0; g_ScrollWait = 94;
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    case WM_DWMCOLORIZATIONCOLORCHANGED:
        g_Theme.Refresh();
        UpdateAppearance(hwnd);
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    case WM_TIMER:
        if (wParam == IDT_POLL_MEDIA) {
            // [FIX] UpdateMediaInfo() runs on PollingThreadProc, not here.
            bool shouldHide = false;
            if (g_Settings.hideFullscreen) {
                QUERY_USER_NOTIFICATION_STATE q;
                if (SUCCEEDED(SHQueryUserNotificationState(&q)))
                    if (q == QUNS_BUSY || q == QUNS_RUNNING_D3D_FULL_SCREEN || q == QUNS_PRESENTATION_MODE)
                        shouldHide = true;
            }
            bool playing; { lock_guard<mutex> g(g_MediaState.lock); playing = g_MediaState.isPlaying; }
            if (g_Settings.idleTimeout > 0) {
                if (playing) { g_IdleSecondsCounter = 0; g_IsHiddenByIdle = false; }
                else { g_IdleSecondsCounter++; if (g_IdleSecondsCounter >= g_Settings.idleTimeout) g_IsHiddenByIdle = true; }
            } else g_IsHiddenByIdle = false;
            if (g_IsHiddenByIdle) shouldHide = true;
            if (shouldHide && IsWindowVisible(hwnd)) { ShowWindow(hwnd, SW_HIDE); HidePopup(); }
            else if (!shouldHide && !IsWindowVisible(hwnd)) {
                HWND hTB = FindWindow(L"Shell_TrayWnd", nullptr);
                if (hTB && IsWindowVisible(hTB)) ShowWindow(hwnd, SW_SHOWNOACTIVATE);
            }
            InvalidateRect(hwnd, NULL, FALSE);
            if (g_PopupVisible && g_hPopupWindow) InvalidateRect(g_hPopupWindow, NULL, FALSE);
        }
        else if (wParam == IDT_ANIMATION) {
            if (!IsWindowVisible(hwnd) && !g_PopupVisible) return 0;
            bool needRedraw = false;
            if (g_IsScrolling) {
                if (g_ScrollWait > 0) {
                    g_ScrollWait--;
                } else {
                    g_ScrollOffset += g_Settings.scrollSpeed;
                    if (g_ScrollOffset > g_TextWidth + 50) {
                        g_ScrollOffset = 0;
                        g_ScrollWait = 94;
                    }
                    needRedraw = true;
                }
            }
            bool playing, haveDur;
            {
                lock_guard<mutex> g(g_MediaState.lock);
                playing  = g_MediaState.isPlaying;
                haveDur  = g_MediaState.durationSeconds > 0.0;
            }
            // Redraw when playing, OR when we have a valid duration (covers
            // pause: bar should still reflect the paused position).
            if (g_Settings.showProgressBar && (playing || haveDur)) needRedraw = true;
            // [PERF] Only redraw if the progress bar will visibly move or the
            //        title is scrolling. A 3-minute track advances ~1 pixel every
            //        500 ms on a ~300 px bar; redrawing every 33 ms is wasteful.
            static ULONGLONG lastProgressRedraw = 0;
            if (needRedraw && !g_IsScrolling) {
                ULONGLONG now = GetTickCount64();
                if (now - lastProgressRedraw < 250) needRedraw = false;
                else lastProgressRedraw = now;
            }
            if (needRedraw) InvalidateRect(hwnd, NULL, FALSE);
            if (g_PopupVisible && g_hPopupWindow && playing)
                InvalidateRect(g_hPopupWindow, NULL, FALSE);
        }
        return 0;
    case WM_APP + 11: {   // [FIX] media state updated by polling thread
        if (g_ResetScrollRequested.exchange(false)) {
            g_ScrollOffset = 0;
            g_ScrollWait = 94;
        }
        InvalidateRect(hwnd, NULL, FALSE);
        if (g_PopupVisible && g_hPopupWindow) InvalidateRect(g_hPopupWindow, NULL, FALSE);
        return 0;
    }
    case WM_APP + 10: {
        HWND hTB = FindWindow(TEXT("Shell_TrayWnd"), nullptr);
        if (!hTB) break;
        if (!IsWindowVisible(hTB)) {
            if (IsWindowVisible(hwnd)) ShowWindow(hwnd, SW_HIDE);
            HidePopup(); return 0;
        }
        if (!g_IsHiddenByIdle && !IsWindowVisible(hwnd)) {
            bool fsHide = false;
            if (g_Settings.hideFullscreen) {
                QUERY_USER_NOTIFICATION_STATE q;
                if (SUCCEEDED(SHQueryUserNotificationState(&q)))
                    if (q == QUNS_BUSY || q == QUNS_RUNNING_D3D_FULL_SCREEN || q == QUNS_PRESENTATION_MODE) fsHide = true;
            }
            if (!fsHide) ShowWindow(hwnd, SW_SHOWNOACTIVATE);
        }
        RECT rc; GetWindowRect(hTB, &rc);
        int x = rc.left + g_Settings.offsetX;
        int tbH = rc.bottom - rc.top;
        int y = rc.top + (tbH / 2) - (g_Settings.height / 2) + g_Settings.offsetY;
        RECT my; GetWindowRect(hwnd, &my);
        bool sizeChanged = (my.right - my.left) != g_Settings.width || (my.bottom - my.top) != g_Settings.height;
        if (my.left != x || my.top != y || sizeChanged) {
            SetWindowPos(hwnd, HWND_TOPMOST, x, y, g_Settings.width, g_Settings.height, SWP_NOACTIVATE);
            if (sizeChanged) UpdateAppearance(hwnd);
        }
        return 0;
    }
    case WM_MOUSEMOVE: {
        HitTarget nh = HitTestCompact(LOWORD(lParam), HIWORD(lParam));
        if (nh != g_HoverState) { g_HoverState = nh; InvalidateRect(hwnd, NULL, FALSE); }
        TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 }; TrackMouseEvent(&tme);
        return 0;
    }
    case WM_MOUSELEAVE:
        g_HoverState = HIT_NONE; InvalidateRect(hwnd, NULL, FALSE); return 0;
    case WM_LBUTTONUP: {
        // Re-hit-test at click time to avoid stale hover state.
        HitTarget hit = HitTestCompact(LOWORD(lParam), HIWORD(lParam));
        int x = LOWORD(lParam);
        switch (hit) {
        case HIT_PREV:      SendMediaCommand(1); break;
        case HIT_PLAYPAUSE: SendMediaCommand(2); break;
        case HIT_NEXT:      SendMediaCommand(3); break;
        case HIT_SHUFFLE:   ToggleShuffle();     break;
        case HIT_REPEAT:    CycleRepeatMode();   break;
        case HIT_PROGRESS:
            if (g_Layout.progW > 0) {
                double r = (double)(x - g_Layout.progX) / (double)g_Layout.progW;
                if (r < 0) r = 0; if (r > 1) r = 1;
                SeekToRatio(r);
            }
            break;
        case HIT_NONE: TogglePopup(); break;
        }
        return 0;
    }
    case WM_RBUTTONUP: {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        ClientToScreen(hwnd, &pt);
        ShowContextMenu(hwnd, pt.x, pt.y);
        return 0;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_PLAYPAUSE: SendMediaCommand(2); break;
        case IDM_PREV:      SendMediaCommand(1); break;
        case IDM_NEXT:      SendMediaCommand(3); break;
        case IDM_SHUFFLE:   ToggleShuffle();     break;
        case IDM_REPEAT_OFF: SetRepeatMode(0);   break;
        case IDM_REPEAT_ALL: SetRepeatMode(1);   break;
        case IDM_REPEAT_ONE: SetRepeatMode(2);   break;
        }
        return 0;
    case WM_MOUSEWHEEL: {
        BYTE vk = (GET_WHEEL_DELTA_WPARAM(wParam) > 0) ? VK_VOLUME_UP : VK_VOLUME_DOWN;
        keybd_event(vk, 0, 0, 0); keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);
        return 0;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc; GetClientRect(hwnd, &rc);
        g_CompactBB.Ensure(hwnd, rc.right, rc.bottom);
        DrawMediaPanel(g_CompactBB.hdc, rc.right, rc.bottom);
        BitBlt(hdc, 0, 0, rc.right, rc.bottom, g_CompactBB.hdc, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
        return 0;
    }
    default:
        if (msg == g_TaskbarCreatedMsg) {
            if (g_TaskbarHook) { UnhookWinEvent(g_TaskbarHook); g_TaskbarHook = nullptr; }
            RegisterTaskbarHook(hwnd);
            return 0;
        }
        break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Media Thread (UI)
// ═══════════════════════════════════════════════════════════════════════════════

static void MediaThread() {
    winrt::init_apartment();
    GdiplusStartupInput gdipIn; ULONG_PTR gdipTok;
    GdiplusStartup(&gdipTok, &gdipIn, NULL);
    DetectFonts(); ComputeLayout(); ComputePopupLayout();
    g_Theme.Refresh();

    HINSTANCE hInst = GetModuleHandle(NULL);
    WNDCLASS wc = {}; wc.lpfnWndProc = MediaWndProc; wc.hInstance = hInst;
    wc.lpszClassName = TEXT("WindhawkMediaPlayer_Compact"); wc.hCursor = LoadCursor(NULL, IDC_HAND);
    RegisterClass(&wc);
    WNDCLASS pc = {}; pc.lpfnWndProc = PopupWndProc; pc.hInstance = hInst;
    pc.lpszClassName = TEXT("WindhawkMediaPlayer_Popup"); pc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClass(&pc);

    HMODULE hU32 = GetModuleHandle(L"user32.dll");
    pCreateWindowInBand CreateWindowInBand = nullptr;
    if (hU32) CreateWindowInBand = (pCreateWindowInBand)GetProcAddress(hU32, "CreateWindowInBand");

    if (CreateWindowInBand) {
        g_hMediaWindow = CreateWindowInBand(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TOPMOST,
            wc.lpszClassName, TEXT("MediaPlayer"), WS_POPUP|WS_VISIBLE,
            0,0, g_Settings.width, g_Settings.height, NULL,NULL, hInst, NULL, ZBID_IMMERSIVE_NOTIFICATION);
        if (g_hMediaWindow) Wh_Log(L"Compact: ZBID_IMMERSIVE_NOTIFICATION");
    }
    if (!g_hMediaWindow) {
        Wh_Log(L"Compact: fallback CreateWindowEx");
        g_hMediaWindow = CreateWindowEx(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TOPMOST,
            wc.lpszClassName, TEXT("MediaPlayer"), WS_POPUP|WS_VISIBLE,
            0,0, g_Settings.width, g_Settings.height, NULL,NULL, hInst, NULL);
    }
    SetLayeredWindowAttributes(g_hMediaWindow, 0, 255, LWA_ALPHA);

    if (CreateWindowInBand) {
        g_hPopupWindow = CreateWindowInBand(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TOPMOST,
            pc.lpszClassName, TEXT("MediaPlayerPopup"), WS_POPUP,
            -9999,-9999, g_PopupW, g_PopupH, NULL,NULL, hInst, NULL, ZBID_IMMERSIVE_NOTIFICATION);
        if (g_hPopupWindow) Wh_Log(L"Popup: ZBID_IMMERSIVE_NOTIFICATION");
    }
    if (!g_hPopupWindow) {
        Wh_Log(L"Popup: fallback CreateWindowEx");
        g_hPopupWindow = CreateWindowEx(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TOPMOST,
            pc.lpszClassName, TEXT("MediaPlayerPopup"), WS_POPUP,
            -9999,-9999, g_PopupW, g_PopupH, NULL,NULL, hInst, NULL);
    }
    SetLayeredWindowAttributes(g_hPopupWindow, 0, 255, LWA_ALPHA);
    UpdateAppearance(g_hPopupWindow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }

    if (g_hPopupWindow) DestroyWindow(g_hPopupWindow);
    UnregisterClass(wc.lpszClassName, hInst);
    UnregisterClass(pc.lpszClassName, hInst);
    { lock_guard<mutex> guard(g_MediaState.lock); g_MediaState.albumArt.reset(); }
    GdiplusShutdown(gdipTok);
    winrt::uninit_apartment();
}

// ═══════════════════════════════════════════════════════════════════════════════
// Polling Thread (WinRT media — runs off the UI thread)
// ═══════════════════════════════════════════════════════════════════════════════

static std::thread*     g_pMediaThread   = nullptr;
static std::thread*     g_pPollingThread = nullptr;
static HANDLE           g_PollStopEvent  = nullptr;

static void PollingThreadProc() {
    winrt::init_apartment();
    // Poll every 1s until the stop event is signaled.
    while (WaitForSingleObject(g_PollStopEvent, 1000) == WAIT_TIMEOUT) {
        UpdateMediaInfo();
        if (g_hMediaWindow) PostMessage(g_hMediaWindow, WM_APP + 11, 0, 0);
    }
    winrt::uninit_apartment();
}

// ═══════════════════════════════════════════════════════════════════════════════
// Windhawk Tool-Mod Callbacks
// ═══════════════════════════════════════════════════════════════════════════════

BOOL WhTool_ModInit() {
    SetCurrentProcessExplicitAppUserModelID(L"taskbar-media-player");
    LoadSettings();
    g_PollStopEvent  = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    g_pMediaThread   = new std::thread(MediaThread);
    g_pPollingThread = new std::thread(PollingThreadProc);
    return TRUE;
}

void WhTool_ModUninit() {
    if (g_PollStopEvent) SetEvent(g_PollStopEvent);
    if (g_pPollingThread) {
        if (g_pPollingThread->joinable()) g_pPollingThread->join();
        delete g_pPollingThread; g_pPollingThread = nullptr;
    }
    if (g_hMediaWindow) SendMessage(g_hMediaWindow, APP_WM_CLOSE, 0, 0);
    if (g_pMediaThread) {
        if (g_pMediaThread->joinable()) g_pMediaThread->join();
        delete g_pMediaThread; g_pMediaThread = nullptr;
    }
    if (g_PollStopEvent) { CloseHandle(g_PollStopEvent); g_PollStopEvent = nullptr; }
}

void WhTool_ModSettingsChanged() {
    LoadSettings(); ComputeLayout(); ComputePopupLayout();
    g_Theme.Refresh();
    if (g_hMediaWindow) {
        PostMessage(g_hMediaWindow, WM_TIMER, IDT_POLL_MEDIA, 0);
        PostMessage(g_hMediaWindow, WM_SETTINGCHANGE, 0, 0);
        PostMessage(g_hMediaWindow, WM_APP + 10, 0, 0);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Windhawk Tool-Mod Boilerplate
// ═══════════════════════════════════════════════════════════════════════════════

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;
void WINAPI EntryPoint_Hook() { Wh_Log(L">"); ExitThread(0); }

BOOL Wh_ModInit() {
    bool isService = false, isToolModProcess = false, isCurrentToolModProcess = false;
    int argc; LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) { Wh_Log(L"CommandLineToArgvW failed"); return FALSE; }
    for (int i = 1; i < argc; i++) { if (wcscmp(argv[i], L"-service") == 0) { isService = true; break; } }
    for (int i = 1; i < argc - 1; i++) { if (wcscmp(argv[i], L"-tool-mod") == 0) {
        isToolModProcess = true; if (wcscmp(argv[i+1], WH_MOD_ID) == 0) isCurrentToolModProcess = true; break; } }
    LocalFree(argv);
    if (isService) return FALSE;
    if (isCurrentToolModProcess) {
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) { Wh_Log(L"CreateMutex failed"); ExitProcess(1); }
        if (GetLastError() == ERROR_ALREADY_EXISTS) { Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID); ExitProcess(1); }
        if (!WhTool_ModInit()) ExitProcess(1);
        IMAGE_DOS_HEADER* dh = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* nh = (IMAGE_NT_HEADERS*)((BYTE*)dh + dh->e_lfanew);
        void* ep = (BYTE*)dh + nh->OptionalHeader.AddressOfEntryPoint;
        Wh_SetFunctionHook(ep, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }
    if (isToolModProcess) return FALSE;
    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) return;
    WCHAR path[MAX_PATH];
    switch (GetModuleFileName(nullptr, path, ARRAYSIZE(path))) { case 0: case ARRAYSIZE(path): Wh_Log(L"GetModuleFileName failed"); return; }
    WCHAR cmd[MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(cmd, L"\"%s\" -tool-mod \"%s\"", path, WH_MOD_ID);
    HMODULE km = GetModuleHandle(L"kernelbase.dll");
    if (!km) km = GetModuleHandle(L"kernel32.dll");
    if (!km) { Wh_Log(L"No kernelbase/kernel32"); return; }
    using CPIW = BOOL(WINAPI*)(HANDLE,LPCWSTR,LPWSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,WINBOOL,DWORD,LPVOID,LPCWSTR,LPSTARTUPINFOW,LPPROCESS_INFORMATION,PHANDLE);
    CPIW pCPIW = (CPIW)GetProcAddress(km, "CreateProcessInternalW");
    if (!pCPIW) { Wh_Log(L"No CreateProcessInternalW"); return; }
    STARTUPINFO si{ .cb = sizeof(STARTUPINFO), .dwFlags = STARTF_FORCEOFFFEEDBACK };
    PROCESS_INFORMATION pi;
    if (!pCPIW(nullptr, path, cmd, nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi, nullptr))
    { Wh_Log(L"CreateProcess failed"); return; }
    CloseHandle(pi.hProcess); CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() { if (!g_isToolModProcessLauncher) WhTool_ModSettingsChanged(); }
void Wh_ModUninit() { if (!g_isToolModProcessLauncher) { WhTool_ModUninit(); ExitProcess(0); } }
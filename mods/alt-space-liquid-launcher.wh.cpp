// ==WindhawkMod==
// @id              alt-space-liquid-launcher
// @name            Liquid Quick Launcher
// @description     Ultra-smooth Liquid Glass productivity launcher with Adaptive Frecency, 120Hz animations, and customizable Outer Window Shape
// @version         6.2.0
// @author          RobsHs
// @github          https://github.com/RobsHs
// @include         windhawk.exe
// @compilerOptions -lgdi32 -ldwmapi -luxtheme -lcomctl32 -lshell32 -lole32 -lshcore -lpowrprof
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Liquid Quick Launcher

A high-performance, self-learning productivity launcher for Windows 11 with a **Liquid Glass** aesthetic, **120Hz Smooth Animations**, and an **Adaptive MRU & Frecency Learning Engine**, built natively in Win32 / C++ for Windhawk as an isolated, standalone tool mod.

![Liquid Quick Launcher Preview](https://raw.githubusercontent.com/RobsHs/liquid-quick-launcher/main/assets/preview.png)

## Key Features
- **Dedicated Tool Process Isolation**: Runs completely in its own process outside of `explorer.exe` using Windhawk's Tool Mod architecture. Never crashes or blocks the Windows taskbar.
- **Global Shortcut (Default: `Alt + Space`, Configurable)**: Summons the launcher instantly from anywhere on a dedicated UI thread. Press `Esc` or click outside to dismiss.
- **Customizable Main Window & Card Shape (Sharp Box up to Full Oval Pill)**:
  - **Sharp Box (`0px`)**: Crisp rectangular main window and cards.
  - **Soft Rounded (`8px`)**: Subtle rounded main window corners.
  - **Balanced Rounded (`14px`)**: Classic modern rounded window frame.
  - **Super Smooth (`26px`)**: Deeply sculpted outer main card curvature (`SetWindowRgn` + anti-aliased GDI border).
  - **Full Pill / Oval Capsule (`70px`)**: When idle as a search bar, the entire main launcher window becomes a 100% stadium oval/pill capsule, and smoothly morphs into a deeply rounded capsule card as search results expand.
- **Adaptive Smart Prediction (1-Letter Instant Recall)**: Automatically learns which applications or URLs you open most frequently and recently. Type a single letter (e.g., `c`, `n`, `v`) or acronym (e.g., `vsc` for Visual Studio Code) and your top pick jumps straight to #1.
- **120Hz Smooth Animation Engine**:
  - **Fluid Height Expansion**: Main window height and outer capsule contour smoothly expand and contract at 120Hz.
  - **Gliding Selection Capsule**: The active selection card and its accent indicator glide smoothly between rows.
  - **Fade & Slide Entrance**: Gently fades in and slides into position when summoned.
  - **Zero Idle CPU**: Animation timers automatically disarm when cards and selection settle.
- **Per-Monitor High-DPI Awareness**: All elements, fonts, borders, and margins dynamically adapt to any display scaling (100%, 125%, 150%, 200%).
- **Clean, Customizable Appearance**:
  - **4 Curated Glass Themes**: Obsidian Dark Glass, Soft Alabaster Light (eye-friendly warm stone), Midnight Pitch Black, and Nordic Slate Glass.
  - **4 Transparency Presets**: Choose between `55%`, `70%`, `85%`, and `100%` glass opacity.
  - **6 Accent Colors**: Electric Blue, Royal Indigo, Emerald Mint, Sunset Rose, Amber Gold, and Crimson Red.
  - **Minimalist Clutter Toggles**: Hide the shortcut badge, item subtitles/descriptions, category badges, or the bottom status bar.
- **Built-in Safety & Power Management**:
  - Two-step confirmation for destructive actions (Restart, Shut Down, Empty Recycle Bin).
  - Instant calculator, direct URL launch, and terminal execution (`> ...`).

> [!NOTE]
> If you also use Spotlight Launcher or PowerToys Run, make sure to change the global shortcut in settings to avoid hotkey conflicts.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- themeMode: obsidian_dark
  $name: Visual Theme
  $description: Choose the Liquid Glass surface theme (Dark, Soft Light, Pitch Black, or Slate).
  $options:
    - obsidian_dark: Liquid Glass Dark (Obsidian)
    - pearl_light: Liquid Glass Light (Soft Alabaster)
    - pitch_black: Midnight Pitch Black (AMOLED)
    - nordic_slate: Nordic Frost (Cool Slate)

- accentPreset: electric_blue
  $name: Accent Color
  $description: Highlight color for the active indicator, search icon, and badges.
  $options:
    - electric_blue: Electric Sky Blue (Default)
    - royal_indigo: Royal Indigo Purple
    - emerald_mint: Emerald Mint Green
    - sunset_rose: Sunset Rose Pink
    - amber_gold: Amber Cyber Gold
    - crimson_red: Crimson Neon Red

- glassOpacity: pct_85
  $name: Glass Transparency (Opacity)
  $description: Select the transparency level of the launcher window.
  $options:
    - pct_55: 55% (Ultra Translucent Glass)
    - pct_70: 70% (Clear Frosted Glass)
    - pct_85: 85% (Balanced Liquid Glass - Default)
    - pct_100: 100% (Solid Opaque Surface)

- shapeStyle: rounded_standard
  $name: Main Window & Card Shape
  $description: Adjust the outer main launcher window and inner card curvature from a sharp box to a full oval/pill capsule.
  $options:
    - sharp_box: Sharp Box (0px Rectangular Window)
    - soft_round: Soft Rounded (8px Subtle Window)
    - rounded_standard: Balanced Rounded (14px Standard Window)
    - super_smooth: Super Smooth (26px Deep Sculpted Window)
    - pill_capsule: Full Pill / Oval Capsule (35px Radius Stadium Pill)

- hotkeyChoice: alt_space
  $name: Global Shortcut
  $description: Key combination used to toggle the launcher.
  $options:
    - alt_space: Alt + Space (Default)
    - ctrl_space: Ctrl + Space
    - win_alt_space: Win + Alt + Space
    - alt_q: Alt + Q

- showShortcutHint: true
  $name: Show Shortcut Header Badge & Action Keys
  $description: Display the shortcut hint pill in the search bar and action keycaps on selected items.

- showSubtitles: true
  $name: Show Item Descriptions (Subtitles)
  $description: Display secondary description text below each item title. Turn off for a clean, single-line title view.

- showCategoryBadges: true
  $name: Show Category Badges ([App], [System], [Web], etc.)
  $description: Display category tags on the right side of each search result.

- showFooterBar: false
  $name: Show Bottom Navigation Footer
  $description: Display the bottom status bar with keyboard navigation hints.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <objbase.h>
#include <shellscalingapi.h>
#include <sddl.h>
#include <powrprof.h>

#include <string>
#include <vector>
#include <cmath>
#include <cwctype>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

#include <windhawk_utils.h>

// Safe guards for DWM attribute constants
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
#ifndef DWMWA_BORDER_COLOR
#define DWMWA_BORDER_COLOR 34
#endif
#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif
#ifndef DWMWA_COLOR_NONE
#define DWMWA_COLOR_NONE 0xFFFFFFFE
#endif

// ============================================================================
// Constants & Layout Metrics
// ============================================================================
static const wchar_t* kWindowClassName    = L"WindhawkLiquidQuickLauncherWnd";
static const int kHotKeyId                = 0x4C51; // 'LQ'
static const UINT_PTR kAnimTimerId        = 0x7701;
static const UINT kAnimFrameIntervalMs    = 8;      // ~120 FPS animation tick
static const UINT WM_APP_INDEX_DONE       = WM_APP + 0x102;
static const UINT WM_APP_RELOAD_SETTINGS  = WM_APP + 0x103;

static const int kBaseWindowWidth         = 720;
static const int kBaseWindowHeight        = 70;
static const int kBaseDividerSpace        = 12;
static const int kBaseItemHeight          = 58;
static const int kBaseBottomPadNoFooter   = 10;
static const int kBaseFooterHeight        = 36;
static const int kMaxVisibleResults       = 5;

// ============================================================================
// DPI Scaling Helper
// ============================================================================
static UINT g_currentDpi = 96;

static inline int ScaleDpi(int val, UINT dpi = 0) {
    if (dpi == 0) dpi = g_currentDpi;
    return MulDiv(val, (int)dpi, 96);
}

// ============================================================================
// Dynamic Visual & UI Configuration
// ============================================================================
enum ShapeMode {
    SHAPE_SHARP_BOX = 0,
    SHAPE_SOFT_ROUND,
    SHAPE_ROUNDED_STANDARD,
    SHAPE_SUPER_SMOOTH,
    SHAPE_PILL_CAPSULE
};

enum HotkeyChoice {
    HOTKEY_ALT_SPACE = 0,
    HOTKEY_CTRL_SPACE,
    HOTKEY_WIN_ALT_SPACE,
    HOTKEY_ALT_Q
};

struct LauncherVisualConfig {
    bool         isLightMode;
    BYTE         targetWindowAlpha;
    bool         showShortcutHint;
    bool         showSubtitles;
    bool         showCategoryBadges;
    bool         showFooterBar;

    DWM_WINDOW_CORNER_PREFERENCE dwmCornerPref;
    ShapeMode    shapeMode;
    HotkeyChoice hotkeyChoice;
    bool         useCustomOuterRegion;
    int          outerWindowEllipse;
    int          itemCardRadius;
    int          iconTileRadius;
    int          badgeRadius;
    int          keycapRadius;

    COLORREF     colBgSurface;
    COLORREF     colHeaderSurface;
    COLORREF     colFooterSurface;
    COLORREF     colGlassRimTop;
    COLORREF     colBorderGlass;

    COLORREF     colItemHover;
    COLORREF     colItemHoverBdr;
    COLORREF     colItemActive;
    COLORREF     colItemBorderAct;
    COLORREF     colItemRimAct;

    COLORREF     colIconTileBg;
    COLORREF     colIconTileBdr;
    COLORREF     colIconTileActBg;
    COLORREF     colIconTileActBdr;

    COLORREF     colAccentPrimary;
    COLORREF     colAccentGlow;
    COLORREF     colFreqGold;
    COLORREF     colDivider;

    COLORREF     colTextPrimary;
    COLORREF     colTextSub;
    COLORREF     colTextSubActive;
    COLORREF     colTextMuted;

    COLORREF     colBadgeBg;
    COLORREF     colBadgeActive;
    COLORREF     colBadgeBorder;
    COLORREF     colBadgeText;
    COLORREF     colBadgeTextActive;

    COLORREF     colKeycapBg;
    COLORREF     colKeycapBorder;
    COLORREF     colKeycapActBg;
    COLORREF     colKeycapActBdr;
};

static LauncherVisualConfig g_cfg;

static const COLORREF kColDotRed    = RGB(235, 90, 82);
static const COLORREF kColDotAmber  = RGB(240, 178, 44);
static const COLORREF kColDotGreen  = RGB(42, 192, 66);

// ============================================================================
// Data Structures
// ============================================================================
enum ItemCategory {
    CAT_APP = 0,
    CAT_CALC,
    CAT_COMMAND,
    CAT_SYSTEM,
    CAT_WEB
};

enum SystemActionType {
    SYS_NONE = 0,
    SYS_LOCK,
    SYS_SLEEP,
    SYS_RESTART,
    SYS_SHUTDOWN,
    SYS_EMPTY_BIN
};

struct IndexedApp {
    std::wstring name;
    std::wstring lowerName;
    std::wstring initials;
    std::wstring targetPath;
    std::wstring arguments;
    std::wstring description;
    HICON        hIcon;
    bool         isSystemTool;
};

struct FrecencyEntry {
    std::wstring itemKey;
    DWORD        useCount;
    DWORD        lastSequence;
    std::wstring lastPrefixes;
};

struct ResultItem {
    std::wstring itemKey;
    std::wstring title;
    std::wstring subtitle;
    std::wstring badge;
    std::wstring actionHint;
    std::wstring iconGlyph;
    HICON        hAppIcon;
    ItemCategory category;
    std::wstring execPath;
    std::wstring execArgs;
    std::wstring clipboardText;
    SystemActionType sysAction;
    DWORD        learnedUseCount;
    bool         isPrefixHabitMatch;
    int          score;
};

// ============================================================================
// Global State & 120Hz Animation State
// ============================================================================
static HINSTANCE g_hModInstance        = NULL;
static HANDLE g_hUiThread              = NULL;
static DWORD  g_dwUiThreadId           = 0;
static HANDLE g_hIndexThread           = NULL;
static volatile LONG g_bStopRequested  = 0;

static HWND   g_hMainWnd               = NULL;
static HWND   g_hEditWnd               = NULL;
static bool   g_bWindowVisible         = false;

static HFONT  g_hFontSearch            = NULL;
static HFONT  g_hFontTitle             = NULL;
static HFONT  g_hFontSub               = NULL;
static HFONT  g_hFontBadge             = NULL;
static HFONT  g_hFontKeycap            = NULL;
static HFONT  g_hFontIcon              = NULL;
static HBRUSH g_hEditBgBrush           = NULL;

static CRITICAL_SECTION g_csApps;
static bool   g_bCsInitialized         = false;
static std::vector<IndexedApp> g_indexedApps;

static std::vector<FrecencyEntry> g_frecencyTable;
static DWORD  g_globalUseSequence      = 1;
static std::wstring g_currentTypedQuery;

static std::vector<ResultItem> g_results;
static int    g_selectedIndex          = 0;
static int    g_hoveredIndex           = -1;

// Confirmation state for destructive actions
static SystemActionType g_pendingConfirmAction = SYS_NONE;
static DWORD  g_dwConfirmArmTime       = 0;

// Animation State
static int    g_anchorPosX             = 300;
static int    g_anchorPosY             = 200;
static float  g_animCurHeight          = 70.0f;
static float  g_animTargetHeight       = 70.0f;
static float  g_animSelectionY         = 0.0f;
static float  g_animTargetSelectionY   = 0.0f;
static float  g_animHoverBlend         = 0.0f;
static float  g_animWindowAlpha        = 217.0f;
static float  g_animSlideOffsetY       = 0.0f;
static float  g_animPulsePhase         = 0.0f;

// Forward Declarations
static void CreateLauncherGdiResources();
static void DestroyLauncherGdiResources();
static void RefreshEditControlColorsAndLayout();
static void UpdateOuterMainWindowShapeRegion(HWND hwnd, int width, int height);

// ============================================================================
// System32 / Sysnative Redirection Helper
// ============================================================================
static std::wstring GetSystem32OrSysnativePath() {
    BOOL isWow64 = FALSE;
    IsWow64Process(GetCurrentProcess(), &isWow64);
    wchar_t winDir[MAX_PATH];
    if (!GetWindowsDirectoryW(winDir, MAX_PATH)) {
        wcscpy_s(winDir, L"C:\\Windows");
    }
    if (isWow64) {
        return std::wstring(winDir) + L"\\Sysnative";
    }
    return std::wstring(winDir) + L"\\System32";
}

// ============================================================================
// Color, String & Math Helpers
// ============================================================================
static int ClampInt(int val, int low, int high) {
    if (val < low) return low;
    if (val > high) return high;
    return val;
}

static COLORREF BlendColorRgb(COLORREF c1, COLORREF c2, float t) {
    if (t <= 0.0f) return c1;
    if (t >= 1.0f) return c2;
    int r1 = GetRValue(c1), g1 = GetGValue(c1), b1 = GetBValue(c1);
    int r2 = GetRValue(c2), g2 = GetGValue(c2), b2 = GetBValue(c2);
    int r = r1 + (int)((r2 - r1) * t);
    int g = g1 + (int)((g2 - g1) * t);
    int b = b1 + (int)((b2 - b1) * t);
    return RGB(ClampInt(r, 0, 255), ClampInt(g, 0, 255), ClampInt(b, 0, 255));
}

static std::wstring ToLowerStr(const std::wstring& s) {
    std::wstring out;
    out.resize(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        out[i] = (wchar_t)towlower(s[i]);
    }
    return out;
}

static std::wstring TrimStr(const std::wstring& s) {
    size_t start = 0;
    while (start < s.size() && iswspace(s[start])) {
        ++start;
    }
    if (start == s.size()) {
        return L"";
    }
    size_t end = s.size() - 1;
    while (end > start && iswspace(s[end])) {
        --end;
    }
    return s.substr(start, end - start + 1);
}

static std::wstring ExtractInitialsStr(const std::wstring& name) {
    std::wstring initials;
    bool prevWasSpaceOrPunct = true;
    bool prevWasLower = false;

    for (size_t i = 0; i < name.size(); ++i) {
        wchar_t c = name[i];
        if (iswalnum(c)) {
            bool isUpper = (iswupper(c) != 0);
            if (prevWasSpaceOrPunct || (isUpper && prevWasLower)) {
                initials.push_back((wchar_t)towlower(c));
            }
            prevWasSpaceOrPunct = false;
            prevWasLower = (iswlower(c) != 0);
        } else {
            prevWasSpaceOrPunct = true;
            prevWasLower = false;
        }
    }
    return initials;
}

static bool MatchSubsequenceScore(const std::wstring& lowerQuery, const std::wstring& lowerTarget, int& outBonus) {
    outBonus = 0;
    if (lowerQuery.empty() || lowerQuery.size() > lowerTarget.size()) {
        return false;
    }

    size_t qIdx = 0;
    int consecutive = 0;
    int boundaryHits = 0;

    for (size_t tIdx = 0; tIdx < lowerTarget.size() && qIdx < lowerQuery.size(); ++tIdx) {
        if (lowerTarget[tIdx] == lowerQuery[qIdx]) {
            if (tIdx == 0 || !iswalnum(lowerTarget[tIdx - 1])) {
                boundaryHits += 1;
            }
            consecutive += 1;
            outBonus += (consecutive * 12);
            ++qIdx;
        } else {
            consecutive = 0;
        }
    }

    if (qIdx != lowerQuery.size()) {
        return false;
    }

    if (lowerQuery.size() <= 2 && boundaryHits == 0) {
        return false;
    }

    outBonus += (boundaryHits * 35);
    return true;
}

static std::wstring UrlEncodeUtf8(const std::wstring& input) {
    if (input.empty()) {
        return L"";
    }
    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, input.c_str(), (int)input.size(), NULL, 0, NULL, NULL);
    if (utf8Len <= 0) {
        return L"";
    }
    std::string utf8Str(utf8Len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, input.c_str(), (int)input.size(), &utf8Str[0], utf8Len, NULL, NULL);

    std::wstring encoded;
    const wchar_t* hexDigits = L"0123456789ABCDEF";
    for (size_t i = 0; i < utf8Str.size(); ++i) {
        unsigned char c = (unsigned char)utf8Str[i];
        if ((c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~') {
            encoded.push_back((wchar_t)c);
        } else if (c == ' ') {
            encoded.push_back(L'+');
        } else {
            encoded.push_back(L'%');
            encoded.push_back(hexDigits[(c >> 4) & 0x0F]);
            encoded.push_back(hexDigits[c & 0x0F]);
        }
    }
    return encoded;
}

static bool CopyTextToClipboard(HWND hOwner, const std::wstring& text) {
    if (!OpenClipboard(hOwner)) {
        return false;
    }
    EmptyClipboard();
    size_t byteCount = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, byteCount);
    if (!hMem) {
        CloseClipboard();
        return false;
    }
    void* pMem = GlobalLock(hMem);
    if (pMem) {
        memcpy(pMem, text.c_str(), byteCount);
        GlobalUnlock(hMem);
        SetClipboardData(CF_UNICODETEXT, hMem);
    } else {
        GlobalFree(hMem);
    }
    CloseClipboard();
    return true;
}

static HICON ExtractFileIcon32(const std::wstring& filePath) {
    if (filePath.empty()) {
        return NULL;
    }
    SHFILEINFOW sfi;
    ZeroMemory(&sfi, sizeof(sfi));
    DWORD_PTR res = SHGetFileInfoW(filePath.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON);
    if (res != 0 && sfi.hIcon != NULL) {
        return sfi.hIcon;
    }
    return NULL;
}

static void DestroyAppIconsInList(std::vector<IndexedApp>& list) {
    for (size_t i = 0; i < list.size(); ++i) {
        if (list[i].hIcon != NULL) {
            DestroyIcon(list[i].hIcon);
            list[i].hIcon = NULL;
        }
    }
}

// ============================================================================
// Windhawk Settings Loader
// ============================================================================
static void LoadUserVisualSettings() {
    auto themeModeSetting = WindhawkUtils::StringSetting::make(L"themeMode");
    std::wstring themeMode = themeModeSetting.get();
    if (themeMode.empty()) themeMode = L"obsidian_dark";

    bool isLight = (themeMode == L"pearl_light");

    auto accentPresetSetting = WindhawkUtils::StringSetting::make(L"accentPreset");
    std::wstring accentPreset = accentPresetSetting.get();
    if (accentPreset.empty()) accentPreset = L"electric_blue";

    COLORREF accent = isLight ? RGB(0, 108, 218) : RGB(10, 132, 255);
    if (accentPreset == L"royal_indigo") {
        accent = isLight ? RGB(104, 66, 214) : RGB(138, 92, 255);
    } else if (accentPreset == L"emerald_mint") {
        accent = isLight ? RGB(12, 152, 104) : RGB(16, 198, 136);
    } else if (accentPreset == L"sunset_rose") {
        accent = isLight ? RGB(214, 52, 108) : RGB(255, 78, 136);
    } else if (accentPreset == L"amber_gold") {
        accent = isLight ? RGB(204, 132, 8) : RGB(255, 184, 28);
    } else if (accentPreset == L"crimson_red") {
        accent = isLight ? RGB(210, 44, 58) : RGB(255, 64, 76);
    }
    g_cfg.colAccentPrimary = accent;
    g_cfg.colAccentGlow    = isLight
                             ? BlendColorRgb(accent, RGB(255, 255, 255), 0.22f)
                             : BlendColorRgb(accent, RGB(255, 255, 255), 0.38f);
    g_cfg.colFreqGold      = isLight ? RGB(224, 152, 12) : RGB(255, 204, 77);

    if (isLight) {
        g_cfg.isLightMode        = true;
        g_cfg.colBgSurface       = RGB(228, 232, 240);
        g_cfg.colHeaderSurface   = RGB(228, 232, 240);
        g_cfg.colFooterSurface   = RGB(218, 223, 233);
        g_cfg.colGlassRimTop     = RGB(244, 246, 252);
        g_cfg.colBorderGlass     = RGB(176, 184, 198);

        g_cfg.colItemHover       = RGB(218, 223, 234);
        g_cfg.colItemHoverBdr    = RGB(192, 200, 214);
        g_cfg.colItemActive      = BlendColorRgb(RGB(238, 241, 248), accent, 0.15f);
        g_cfg.colItemBorderAct   = BlendColorRgb(RGB(178, 188, 206), accent, 0.48f);
        g_cfg.colItemRimAct      = RGB(248, 250, 254);

        g_cfg.colIconTileBg      = RGB(236, 239, 246);
        g_cfg.colIconTileBdr     = RGB(198, 205, 218);
        g_cfg.colIconTileActBg   = RGB(244, 246, 252);
        g_cfg.colIconTileActBdr  = BlendColorRgb(RGB(180, 190, 208), accent, 0.42f);

        g_cfg.colDivider         = RGB(202, 208, 220);
        g_cfg.colTextPrimary     = RGB(24, 28, 38);
        g_cfg.colTextSub         = RGB(90, 98, 116);
        g_cfg.colTextSubActive   = RGB(48, 56, 74);
        g_cfg.colTextMuted       = RGB(114, 122, 138);

        g_cfg.colBadgeBg         = RGB(215, 221, 232);
        g_cfg.colBadgeActive     = accent;
        g_cfg.colBadgeBorder     = RGB(188, 196, 212);
        g_cfg.colBadgeText       = RGB(58, 66, 84);
        g_cfg.colBadgeTextActive = RGB(255, 255, 255);

        g_cfg.colKeycapBg        = RGB(218, 223, 234);
        g_cfg.colKeycapBorder    = RGB(186, 194, 210);
        g_cfg.colKeycapActBg     = BlendColorRgb(RGB(232, 236, 245), accent, 0.18f);
        g_cfg.colKeycapActBdr    = BlendColorRgb(RGB(174, 184, 202), accent, 0.50f);
    } else if (themeMode == L"pitch_black") {
        g_cfg.isLightMode        = false;
        g_cfg.colBgSurface       = RGB(8, 8, 12);
        g_cfg.colHeaderSurface   = RGB(8, 8, 12);
        g_cfg.colFooterSurface   = RGB(5, 5, 8);
        g_cfg.colGlassRimTop     = RGB(58, 62, 78);
        g_cfg.colBorderGlass     = RGB(55, 55, 65);

        g_cfg.colItemHover       = RGB(18, 20, 28);
        g_cfg.colItemHoverBdr    = RGB(38, 42, 54);
        g_cfg.colItemActive      = BlendColorRgb(RGB(18, 20, 28), accent, 0.22f);
        g_cfg.colItemBorderAct   = BlendColorRgb(RGB(48, 54, 70), accent, 0.55f);
        g_cfg.colItemRimAct      = BlendColorRgb(RGB(70, 78, 100), accent, 0.45f);

        g_cfg.colIconTileBg      = RGB(15, 16, 22);
        g_cfg.colIconTileBdr     = RGB(34, 36, 46);
        g_cfg.colIconTileActBg   = BlendColorRgb(RGB(20, 22, 30), accent, 0.20f);
        g_cfg.colIconTileActBdr  = BlendColorRgb(RGB(52, 58, 76), accent, 0.50f);

        g_cfg.colDivider         = RGB(30, 32, 42);
        g_cfg.colTextPrimary     = RGB(248, 250, 255);
        g_cfg.colTextSub         = RGB(142, 150, 170);
        g_cfg.colTextSubActive   = RGB(205, 215, 238);
        g_cfg.colTextMuted       = RGB(96, 102, 120);

        g_cfg.colBadgeBg         = RGB(20, 22, 30);
        g_cfg.colBadgeActive     = BlendColorRgb(RGB(16, 20, 30), accent, 0.48f);
        g_cfg.colBadgeBorder     = RGB(44, 48, 62);
        g_cfg.colBadgeText       = BlendColorRgb(RGB(185, 195, 215), accent, 0.35f);
        g_cfg.colBadgeTextActive = RGB(255, 255, 255);

        g_cfg.colKeycapBg        = RGB(18, 20, 26);
        g_cfg.colKeycapBorder    = RGB(46, 50, 64);
        g_cfg.colKeycapActBg     = BlendColorRgb(RGB(22, 26, 36), accent, 0.32f);
        g_cfg.colKeycapActBdr    = BlendColorRgb(RGB(60, 68, 88), accent, 0.55f);
    } else if (themeMode == L"nordic_slate") {
        g_cfg.isLightMode        = false;
        g_cfg.colBgSurface       = RGB(28, 33, 43);
        g_cfg.colHeaderSurface   = RGB(28, 33, 43);
        g_cfg.colFooterSurface   = RGB(23, 27, 36);
        g_cfg.colGlassRimTop     = RGB(88, 98, 122);
        g_cfg.colBorderGlass     = RGB(55, 55, 65);

        g_cfg.colItemHover       = RGB(38, 45, 58);
        g_cfg.colItemHoverBdr    = RGB(60, 70, 90);
        g_cfg.colItemActive      = BlendColorRgb(RGB(42, 50, 66), accent, 0.24f);
        g_cfg.colItemBorderAct   = BlendColorRgb(RGB(72, 84, 108), accent, 0.55f);
        g_cfg.colItemRimAct      = BlendColorRgb(RGB(96, 110, 140), accent, 0.45f);

        g_cfg.colIconTileBg      = RGB(34, 40, 52);
        g_cfg.colIconTileBdr     = RGB(54, 62, 80);
        g_cfg.colIconTileActBg   = BlendColorRgb(RGB(38, 46, 60), accent, 0.22f);
        g_cfg.colIconTileActBdr  = BlendColorRgb(RGB(68, 78, 100), accent, 0.50f);

        g_cfg.colDivider         = RGB(50, 58, 74);
        g_cfg.colTextPrimary     = RGB(236, 239, 244);
        g_cfg.colTextSub         = RGB(160, 170, 190);
        g_cfg.colTextSubActive   = RGB(216, 222, 233);
        g_cfg.colTextMuted       = RGB(118, 128, 148);

        g_cfg.colBadgeBg         = RGB(39, 46, 60);
        g_cfg.colBadgeActive     = BlendColorRgb(RGB(32, 40, 54), accent, 0.50f);
        g_cfg.colBadgeBorder     = RGB(64, 74, 94);
        g_cfg.colBadgeText       = BlendColorRgb(RGB(200, 210, 228), accent, 0.35f);
        g_cfg.colBadgeTextActive = RGB(255, 255, 255);

        g_cfg.colKeycapBg        = RGB(36, 42, 55);
        g_cfg.colKeycapBorder    = RGB(64, 74, 94);
        g_cfg.colKeycapActBg     = BlendColorRgb(RGB(40, 48, 64), accent, 0.32f);
        g_cfg.colKeycapActBdr    = BlendColorRgb(RGB(74, 86, 110), accent, 0.55f);
    } else {
        // Default: obsidian_dark
        g_cfg.isLightMode        = false;
        g_cfg.colBgSurface       = RGB(20, 21, 27);
        g_cfg.colHeaderSurface   = RGB(20, 21, 27);
        g_cfg.colFooterSurface   = RGB(16, 17, 22);
        g_cfg.colGlassRimTop     = RGB(74, 78, 96);
        g_cfg.colBorderGlass     = RGB(55, 55, 65);

        g_cfg.colItemHover       = RGB(30, 32, 42);
        g_cfg.colItemHoverBdr    = RGB(48, 52, 66);
        g_cfg.colItemActive      = BlendColorRgb(RGB(32, 35, 48), accent, 0.22f);
        g_cfg.colItemBorderAct   = BlendColorRgb(RGB(62, 68, 88), accent, 0.55f);
        g_cfg.colItemRimAct      = BlendColorRgb(RGB(85, 95, 120), accent, 0.48f);

        g_cfg.colIconTileBg      = RGB(27, 29, 38);
        g_cfg.colIconTileBdr     = RGB(44, 47, 60);
        g_cfg.colIconTileActBg   = BlendColorRgb(RGB(28, 32, 44), accent, 0.20f);
        g_cfg.colIconTileActBdr  = BlendColorRgb(RGB(60, 68, 88), accent, 0.50f);

        g_cfg.colDivider         = RGB(42, 44, 56);
        g_cfg.colTextPrimary     = RGB(246, 248, 255);
        g_cfg.colTextSub         = RGB(148, 155, 174);
        g_cfg.colTextSubActive   = RGB(204, 214, 236);
        g_cfg.colTextMuted       = RGB(104, 110, 128);

        g_cfg.colBadgeBg         = RGB(32, 35, 46);
        g_cfg.colBadgeActive     = BlendColorRgb(RGB(24, 30, 44), accent, 0.48f);
        g_cfg.colBadgeBorder     = RGB(54, 58, 74);
        g_cfg.colBadgeText       = BlendColorRgb(RGB(190, 204, 230), accent, 0.35f);
        g_cfg.colBadgeTextActive = RGB(255, 255, 255);

        g_cfg.colKeycapBg        = RGB(30, 32, 40);
        g_cfg.colKeycapBorder    = RGB(58, 62, 76);
        g_cfg.colKeycapActBg     = BlendColorRgb(RGB(30, 34, 46), accent, 0.32f);
        g_cfg.colKeycapActBdr    = BlendColorRgb(RGB(68, 76, 96), accent, 0.55f);
    }

    auto opacitySetting = WindhawkUtils::StringSetting::make(L"glassOpacity");
    std::wstring opacityMode = opacitySetting.get();
    if (opacityMode.empty()) opacityMode = L"pct_85";

    if (opacityMode == L"pct_55") {
        g_cfg.targetWindowAlpha = (BYTE)140;
    } else if (opacityMode == L"pct_70") {
        g_cfg.targetWindowAlpha = (BYTE)179;
    } else if (opacityMode == L"pct_100") {
        g_cfg.targetWindowAlpha = (BYTE)255;
    } else {
        g_cfg.targetWindowAlpha = (BYTE)217;
    }

    auto shapeSetting = WindhawkUtils::StringSetting::make(L"shapeStyle");
    std::wstring shapeStyle = shapeSetting.get();
    if (shapeStyle.empty()) shapeStyle = L"rounded_standard";

    if (shapeStyle == L"sharp_box") {
        g_cfg.shapeMode            = SHAPE_SHARP_BOX;
        g_cfg.dwmCornerPref        = DWMWCP_DONOTROUND;
        g_cfg.useCustomOuterRegion = false;
        g_cfg.outerWindowEllipse   = 0;
        g_cfg.itemCardRadius       = 0;
        g_cfg.iconTileRadius       = 0;
        g_cfg.badgeRadius          = 0;
        g_cfg.keycapRadius         = 0;
    } else if (shapeStyle == L"soft_round") {
        g_cfg.shapeMode            = SHAPE_SOFT_ROUND;
        g_cfg.dwmCornerPref        = DWMWCP_ROUNDSMALL;
        g_cfg.useCustomOuterRegion = false;
        g_cfg.outerWindowEllipse   = 8;
        g_cfg.itemCardRadius       = 6;
        g_cfg.iconTileRadius       = 6;
        g_cfg.badgeRadius          = 4;
        g_cfg.keycapRadius         = 4;
    } else if (shapeStyle == L"super_smooth") {
        g_cfg.shapeMode            = SHAPE_SUPER_SMOOTH;
        g_cfg.dwmCornerPref        = DWMWCP_ROUND;
        g_cfg.useCustomOuterRegion = true;
        g_cfg.outerWindowEllipse   = 48;
        g_cfg.itemCardRadius       = 18;
        g_cfg.iconTileRadius       = 14;
        g_cfg.badgeRadius          = 10;
        g_cfg.keycapRadius         = 10;
    } else if (shapeStyle == L"pill_capsule") {
        g_cfg.shapeMode            = SHAPE_PILL_CAPSULE;
        g_cfg.dwmCornerPref        = DWMWCP_ROUND;
        g_cfg.useCustomOuterRegion = true;
        g_cfg.outerWindowEllipse   = 70;
        g_cfg.itemCardRadius       = 27;
        g_cfg.iconTileRadius       = 21;
        g_cfg.badgeRadius          = 12;
        g_cfg.keycapRadius         = 12;
    } else {
        g_cfg.shapeMode            = SHAPE_ROUNDED_STANDARD;
        g_cfg.dwmCornerPref        = DWMWCP_ROUND;
        g_cfg.useCustomOuterRegion = false;
        g_cfg.outerWindowEllipse   = 16;
        g_cfg.itemCardRadius       = 12;
        g_cfg.iconTileRadius       = 10;
        g_cfg.badgeRadius          = 8;
        g_cfg.keycapRadius         = 8;
    }

    auto hotkeySetting = WindhawkUtils::StringSetting::make(L"hotkeyChoice");
    std::wstring hotkey = hotkeySetting.get();
    if (hotkey == L"ctrl_space") {
        g_cfg.hotkeyChoice = HOTKEY_CTRL_SPACE;
    } else if (hotkey == L"win_alt_space") {
        g_cfg.hotkeyChoice = HOTKEY_WIN_ALT_SPACE;
    } else if (hotkey == L"alt_q") {
        g_cfg.hotkeyChoice = HOTKEY_ALT_Q;
    } else {
        g_cfg.hotkeyChoice = HOTKEY_ALT_SPACE;
    }

    g_cfg.showShortcutHint   = (Wh_GetIntSetting(L"showShortcutHint") != 0);
    g_cfg.showSubtitles      = (Wh_GetIntSetting(L"showSubtitles") != 0);
    g_cfg.showCategoryBadges = (Wh_GetIntSetting(L"showCategoryBadges") != 0);
    g_cfg.showFooterBar      = (Wh_GetIntSetting(L"showFooterBar") != 0);
}

// ============================================================================
// Windhawk Mod Storage (Per-User SID Scoped)
// ============================================================================
static std::wstring CurrentUserSid() {
    static std::wstring sid;
    if (!sid.empty()) return sid;
    HANDLE token = nullptr;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token) && token) {
        DWORD size = 0;
        GetTokenInformation(token, TokenUser, nullptr, 0, &size);
        if (size > 0) {
            std::vector<BYTE> buf(size);
            TOKEN_USER* user = reinterpret_cast<TOKEN_USER*>(buf.data());
            if (GetTokenInformation(token, TokenUser, user, size, &size) && user->User.Sid) {
                LPWSTR str = nullptr;
                if (ConvertSidToStringSidW(user->User.Sid, &str) && str) {
                    sid = str;
                    LocalFree(str);
                }
            }
        }
        CloseHandle(token);
    }
    return sid;
}

static std::wstring FrecencyStorageKey() {
    return L"frecency.v1_" + CurrentUserSid();
}

static void LoadFrecencyHistory() {
    g_frecencyTable.clear();
    g_globalUseSequence = 1;

    PCWSTR rawData = Wh_GetStringValue(FrecencyStorageKey().c_str());
    if (!rawData || !rawData[0]) {
        if (rawData) Wh_FreeStringValue(rawData);
        return;
    }

    std::wstring data = rawData;
    Wh_FreeStringValue(rawData);

    size_t start = 0;
    while (start < data.size()) {
        size_t newline = data.find(L'\n', start);
        std::wstring line = (newline == std::wstring::npos) ? data.substr(start) : data.substr(start, newline - start);
        start = (newline == std::wstring::npos) ? data.size() : newline + 1;

        if (line.empty()) continue;
        size_t p1 = line.find(L'|');
        if (p1 == std::wstring::npos) continue;
        size_t p2 = line.find(L'|', p1 + 1);
        if (p2 == std::wstring::npos) continue;
        size_t p3 = line.find(L'|', p2 + 1);
        if (p3 == std::wstring::npos) continue;

        std::wstring key = line.substr(0, p1);
        DWORD count = (DWORD)wcstoul(line.substr(p1 + 1, p2 - p1 - 1).c_str(), nullptr, 10);
        DWORD seq = (DWORD)wcstoul(line.substr(p2 + 1, p3 - p2 - 1).c_str(), nullptr, 10);
        std::wstring prefixes = line.substr(p3 + 1);

        if (count > 0 && !key.empty()) {
            FrecencyEntry entry;
            entry.itemKey = key;
            entry.useCount = count;
            entry.lastSequence = seq;
            entry.lastPrefixes = prefixes;
            g_frecencyTable.push_back(entry);
            if (seq >= g_globalUseSequence) {
                g_globalUseSequence = seq + 1;
            }
        }
    }
}

static void SaveFrecencyHistory() {
    // Keep top 100 entries to prevent unbounded growth
    if (g_frecencyTable.size() > 100) {
        std::sort(g_frecencyTable.begin(), g_frecencyTable.end(), [](const FrecencyEntry& a, const FrecencyEntry& b) {
            return a.lastSequence > b.lastSequence;
        });
        g_frecencyTable.resize(100);
    }

    std::wstring serialized;
    serialized.reserve(g_frecencyTable.size() * 64);
    wchar_t buf[256];
    for (const auto& entry : g_frecencyTable) {
        swprintf(buf, 256, L"%lu|%lu", (unsigned long)entry.useCount, (unsigned long)entry.lastSequence);
        serialized += entry.itemKey + L"|" + buf + L"|" + entry.lastPrefixes + L"\n";
    }

    Wh_SetStringValue(FrecencyStorageKey().c_str(), serialized.c_str());
}

static void AddLearnedPrefixToken(std::wstring& prefixStore, const std::wstring& token) {
    if (token.empty() || token.size() > 6) {
        return;
    }
    if (prefixStore.empty()) {
        prefixStore = L";";
    }
    std::wstring needle = L";" + token + L";";
    if (prefixStore.find(needle) != std::wstring::npos) {
        size_t pos = prefixStore.find(needle);
        prefixStore.erase(pos, needle.size() - 1);
    }
    prefixStore += token + L";";

    if (prefixStore.size() > 160) {
        size_t firstSep = prefixStore.find(L';', 1);
        if (firstSep != std::wstring::npos) {
            prefixStore.erase(0, firstSep);
        }
    }
}

static void RecordItemExecutionFrecency(const std::wstring& itemKey, const std::wstring& typedQuery) {
    // Only record genuine apps and URLs, never search queries or commands
    if (itemKey.empty() || itemKey.find(L"app:") != 0 && itemKey.find(L"url:") != 0) {
        return;
    }

    std::wstring normKey = ToLowerStr(itemKey);
    std::wstring normQuery = ToLowerStr(TrimStr(typedQuery));

    std::vector<std::wstring> prefixesToLearn;
    if (!normQuery.empty() && normQuery[0] != L'>') {
        size_t maxP = normQuery.size() < 4 ? normQuery.size() : 4;
        for (size_t len = 1; len <= maxP; ++len) {
            prefixesToLearn.push_back(normQuery.substr(0, len));
        }
    }

    if (!prefixesToLearn.empty()) {
        for (size_t i = 0; i < g_frecencyTable.size(); ++i) {
            if (g_frecencyTable[i].itemKey == normKey) {
                continue;
            }
            for (size_t p = 0; p < prefixesToLearn.size(); ++p) {
                std::wstring needle = L";" + prefixesToLearn[p] + L";";
                size_t pos = g_frecencyTable[i].lastPrefixes.find(needle);
                if (pos != std::wstring::npos) {
                    g_frecencyTable[i].lastPrefixes.erase(pos, needle.size() - 1);
                }
            }
        }
    }

    bool found = false;
    for (size_t i = 0; i < g_frecencyTable.size(); ++i) {
        if (g_frecencyTable[i].itemKey == normKey) {
            g_frecencyTable[i].useCount += 1;
            g_frecencyTable[i].lastSequence = g_globalUseSequence++;
            for (size_t p = 0; p < prefixesToLearn.size(); ++p) {
                AddLearnedPrefixToken(g_frecencyTable[i].lastPrefixes, prefixesToLearn[p]);
            }
            found = true;
            break;
        }
    }

    if (!found) {
        FrecencyEntry newEntry;
        newEntry.itemKey = normKey;
        newEntry.useCount = 1;
        newEntry.lastSequence = g_globalUseSequence++;
        newEntry.lastPrefixes = L";";
        for (size_t p = 0; p < prefixesToLearn.size(); ++p) {
            AddLearnedPrefixToken(newEntry.lastPrefixes, prefixesToLearn[p]);
        }
        g_frecencyTable.push_back(newEntry);
    }

    SaveFrecencyHistory();
}

static int ComputeFrecencyBoost(const std::wstring& itemKey,
                                const std::wstring& lowerQuery,
                                DWORD& outUseCount,
                                bool& outIsHabitMatch) {
    outUseCount = 0;
    outIsHabitMatch = false;
    if (itemKey.empty()) {
        return 0;
    }

    std::wstring normKey = ToLowerStr(itemKey);
    for (size_t i = 0; i < g_frecencyTable.size(); ++i) {
        const FrecencyEntry& e = g_frecencyTable[i];
        if (e.itemKey == normKey) {
            outUseCount = e.useCount;

            DWORD cappedUses = e.useCount < 50 ? e.useCount : 50;
            int freqBoost = (int)(cappedUses * 45);

            int recencyBoost = 0;
            if (g_globalUseSequence > e.lastSequence) {
                DWORD age = g_globalUseSequence - e.lastSequence;
                if (age == 1) {
                    recencyBoost = 420;
                } else if (age <= 3) {
                    recencyBoost = 300;
                } else if (age <= 8) {
                    recencyBoost = 180;
                } else if (age <= 20) {
                    recencyBoost = 90;
                }
            }

            int habitBoost = 0;
            if (!lowerQuery.empty() && lowerQuery.size() <= 6) {
                std::wstring needle = L";" + lowerQuery + L";";
                if (e.lastPrefixes.find(needle) != std::wstring::npos) {
                    habitBoost = 1250;
                    outIsHabitMatch = true;
                }
            }

            return freqBoost + recencyBoost + habitBoost;
        }
    }
    return 0;
}

// ============================================================================
// Recursive-Descent Math Expression Parser
// ============================================================================
struct MathParser {
    const wchar_t* src;
    size_t pos;
    size_t len;
    bool hasError;
    bool usedOperatorOrFunc;

    MathParser(const std::wstring& input) {
        src = input.c_str();
        pos = 0;
        len = input.size();
        hasError = false;
        usedOperatorOrFunc = false;
    }

    void SkipWhitespace() {
        while (pos < len && iswspace(src[pos])) {
            ++pos;
        }
    }

    bool MatchChar(wchar_t expected) {
        SkipWhitespace();
        if (pos < len && src[pos] == expected) {
            ++pos;
            return true;
        }
        return false;
    }

    bool MatchIdentifier(const wchar_t* ident) {
        SkipWhitespace();
        size_t savePos = pos;
        size_t i = 0;
        while (ident[i] != L'\0') {
            if (pos >= len || towlower(src[pos]) != towlower(ident[i])) {
                pos = savePos;
                return false;
            }
            ++pos;
            ++i;
        }
        if (pos < len && iswalnum(src[pos])) {
            pos = savePos;
            return false;
        }
        return true;
    }

    double ParseExpression() {
        double left = ParseTerm();
        while (!hasError) {
            SkipWhitespace();
            if (MatchChar(L'+')) {
                usedOperatorOrFunc = true;
                double right = ParseTerm();
                left = left + right;
            } else if (MatchChar(L'-')) {
                usedOperatorOrFunc = true;
                double right = ParseTerm();
                left = left - right;
            } else {
                break;
            }
        }
        return left;
    }

    double ParseTerm() {
        double left = ParsePower();
        while (!hasError) {
            SkipWhitespace();
            if (MatchChar(L'*')) {
                usedOperatorOrFunc = true;
                double right = ParsePower();
                left = left * right;
            } else if (MatchChar(L'/')) {
                usedOperatorOrFunc = true;
                double right = ParsePower();
                if (std::fabs(right) < 1e-15) {
                    hasError = true;
                    return 0.0;
                }
                left = left / right;
            } else if (MatchChar(L'%')) {
                usedOperatorOrFunc = true;
                double right = ParsePower();
                if (std::fabs(right) < 1e-15) {
                    hasError = true;
                    return 0.0;
                }
                left = std::fmod(left, right);
            } else {
                break;
            }
        }
        return left;
    }

    double ParsePower() {
        double base = ParseUnary();
        if (!hasError) {
            SkipWhitespace();
            if (MatchChar(L'^')) {
                usedOperatorOrFunc = true;
                double exponent = ParsePower();
                base = std::pow(base, exponent);
            }
        }
        return base;
    }

    double ParseUnary() {
        SkipWhitespace();
        if (MatchChar(L'+')) {
            return ParseUnary();
        }
        if (MatchChar(L'-')) {
            usedOperatorOrFunc = true;
            return -ParseUnary();
        }
        return ParsePrimary();
    }

    double ParsePrimary() {
        SkipWhitespace();
        if (MatchIdentifier(L"sqrt")) {
            usedOperatorOrFunc = true;
            if (!MatchChar(L'(')) {
                hasError = true;
                return 0.0;
            }
            double val = ParseExpression();
            if (!MatchChar(L')') || val < 0.0) {
                hasError = true;
                return 0.0;
            }
            return std::sqrt(val);
        }

        if (MatchChar(L'(')) {
            usedOperatorOrFunc = true;
            double val = ParseExpression();
            if (!MatchChar(L')')) {
                hasError = true;
                return 0.0;
            }
            return val;
        }

        return ParseNumber();
    }

    double ParseNumber() {
        SkipWhitespace();
        size_t startPos = pos;
        bool hasDigits = false;

        while (pos < len && iswdigit(src[pos])) {
            hasDigits = true;
            ++pos;
        }
        if (pos < len && src[pos] == L'.') {
            ++pos;
            while (pos < len && iswdigit(src[pos])) {
                hasDigits = true;
                ++pos;
            }
        }

        if (!hasDigits) {
            hasError = true;
            return 0.0;
        }

        std::wstring numStr(src + startPos, pos - startPos);
        wchar_t* endPtr = NULL;
        double val = wcstod(numStr.c_str(), &endPtr);
        if (endPtr == numStr.c_str()) {
            hasError = true;
            return 0.0;
        }
        return val;
    }
};

static bool TryEvaluateMath(const std::wstring& query, std::wstring& outFormatted) {
    std::wstring trimmed = TrimStr(query);
    if (trimmed.empty() || trimmed[0] == L'>') {
        return false;
    }

    MathParser parser(trimmed);
    double result = parser.ParseExpression();
    parser.SkipWhitespace();

    if (parser.hasError || parser.pos != parser.len || !parser.usedOperatorOrFunc) {
        return false;
    }
    if (std::isnan(result) || std::isinf(result)) {
        return false;
    }

    wchar_t buf[128];
    double rounded = std::round(result);
    if (std::fabs(result - rounded) < 1e-9 && std::fabs(result) < 1e15) {
        swprintf(buf, 128, L"%.0f", rounded);
    } else {
        swprintf(buf, 128, L"%.8f", result);
        size_t len = wcslen(buf);
        while (len > 0 && buf[len - 1] == L'0') {
            buf[--len] = L'\0';
        }
        if (len > 0 && buf[len - 1] == L'.') {
            buf[--len] = L'\0';
        }
    }
    outFormatted = buf;
    return true;
}

// ============================================================================
// URL / Domain Detection
// ============================================================================
static bool IsLikelyUrlOrDomain(const std::wstring& query, std::wstring& outFullUrl) {
    std::wstring q = TrimStr(query);
    if (q.empty()) {
        return false;
    }
    for (size_t i = 0; i < q.size(); ++i) {
        if (iswspace(q[i])) {
            return false;
        }
    }

    std::wstring lower = ToLowerStr(q);
    if (lower.find(L"http://") == 0 || lower.find(L"https://") == 0) {
        outFullUrl = q;
        return true;
    }

    size_t dotPos = q.find(L'.');
    if (dotPos == std::wstring::npos || dotPos == 0 || dotPos == q.size() - 1) {
        return false;
    }

    if (!iswalnum(q[0])) {
        return false;
    }

    size_t slashPos = q.find(L'/');
    std::wstring hostPart = (slashPos == std::wstring::npos) ? q : q.substr(0, slashPos);
    size_t lastDot = hostPart.rfind(L'.');
    if (lastDot == std::wstring::npos || hostPart.size() - lastDot - 1 < 2) {
        return false;
    }

    std::wstring tld = ToLowerStr(hostPart.substr(lastDot + 1));
    // Filter out common code/file extensions from false-triggering as web domains
    if (tld == L"exe" || tld == L"dll" || tld == L"py" || tld == L"cpp" ||
        tld == L"h" || tld == L"json" || tld == L"xml" || tld == L"txt" ||
        tld == L"log" || tld == L"md" || tld == L"bat" || tld == L"cmd") {
        return false;
    }

    for (size_t i = 0; i < tld.size(); ++i) {
        if (!iswalpha(tld[i])) {
            return false;
        }
    }

    outFullUrl = L"https://" + q;
    return true;
}

// ============================================================================
// Fast Application Indexer + Real Icon Extractor (Background Thread)
// ============================================================================
static void AddAppToList(std::vector<IndexedApp>& list,
                         const std::wstring& name,
                         const std::wstring& targetPath,
                         const std::wstring& arguments,
                         const std::wstring& description,
                         const std::wstring& iconSourcePath,
                         bool isSystemTool) {
    if (name.empty() || targetPath.empty()) {
        return;
    }
    std::wstring lower = ToLowerStr(name);
    for (size_t i = 0; i < list.size(); ++i) {
        if (list[i].lowerName == lower) {
            return;
        }
    }
    IndexedApp app;
    app.name = name;
    app.lowerName = lower;
    app.initials = ExtractInitialsStr(name);
    app.targetPath = targetPath;
    app.arguments = arguments;
    app.description = description;
    app.hIcon = ExtractFileIcon32(iconSourcePath.empty() ? targetPath : iconSourcePath);
    app.isSystemTool = isSystemTool;
    list.push_back(app);
}

static void ScanDirectoryForShortcuts(const std::wstring& folderPath,
                                      std::vector<IndexedApp>& outApps,
                                      int depth) {
    if (depth > 5 || InterlockedCompareExchange(&g_bStopRequested, 0, 0) != 0) {
        return;
    }

    std::wstring searchPattern = folderPath + L"\\*";
    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(searchPattern.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (InterlockedCompareExchange(&g_bStopRequested, 0, 0) != 0) {
            break;
        }
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) {
            continue;
        }

        std::wstring fullPath = folderPath + L"\\" + fd.cFileName;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            ScanDirectoryForShortcuts(fullPath, outApps, depth + 1);
        } else {
            std::wstring fileName = fd.cFileName;
            std::wstring lowerFile = ToLowerStr(fileName);
            if (lowerFile.size() > 4 && lowerFile.substr(lowerFile.size() - 4) == L".lnk") {
                if (lowerFile.find(L"uninstall") != std::wstring::npos) {
                    continue;
                }
                std::wstring displayName = fileName.substr(0, fileName.size() - 4);
                AddAppToList(outApps, displayName, fullPath, L"", L"Desktop Application  \x2022  " + displayName, fullPath, false);
            }
        }
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);
}

static DWORD WINAPI AppIndexerThreadProc(LPVOID lpParam) {
    (void)lpParam;
    HRESULT hrCom = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    std::vector<IndexedApp> localApps;
    localApps.reserve(256);

    std::wstring sDir = GetSystem32OrSysnativePath();

    wchar_t winDir[MAX_PATH];
    if (!GetWindowsDirectoryW(winDir, MAX_PATH)) {
        wcscpy_s(winDir, L"C:\\Windows");
    }
    std::wstring wDir = winDir;

    // Core Windows System Tools
    AddAppToList(localApps, L"Notepad",            sDir + L"\\notepad.exe",      L"",            L"System Application  \x2022  Text Editor",           sDir + L"\\notepad.exe", true);
    AddAppToList(localApps, L"Calculator",         sDir + L"\\calc.exe",         L"",            L"System Application  \x2022  Calculator",            sDir + L"\\calc.exe", true);
    AddAppToList(localApps, L"Task Manager",       sDir + L"\\taskmgr.exe",      L"",            L"System Utility  \x2022  Process & Performance",     sDir + L"\\taskmgr.exe", true);
    AddAppToList(localApps, L"Command Prompt",     sDir + L"\\cmd.exe",          L"",            L"Terminal  \x2022  Windows Command Processor",       sDir + L"\\cmd.exe", true);
    AddAppToList(localApps, L"PowerShell",         sDir + L"\\WindowsPowerShell\\v1.0\\powershell.exe", L"", L"Terminal  \x2022  Windows PowerShell Console", sDir + L"\\WindowsPowerShell\\v1.0\\powershell.exe", true);
    AddAppToList(localApps, L"Paint",              sDir + L"\\mspaint.exe",      L"",            L"System Application  \x2022  Graphics Editor",       sDir + L"\\mspaint.exe", true);
    AddAppToList(localApps, L"Registry Editor",    wDir + L"\\regedit.exe",      L"",            L"System Utility  \x2022  Registry Configuration",    wDir + L"\\regedit.exe", true);
    AddAppToList(localApps, L"Control Panel",      sDir + L"\\control.exe",      L"",            L"System Utility  \x2022  Control Panel",             sDir + L"\\control.exe", true);
    AddAppToList(localApps, L"File Explorer",      wDir + L"\\explorer.exe",     L"",            L"System Application  \x2022  File Manager",          wDir + L"\\explorer.exe", true);
    AddAppToList(localApps, L"Snipping Tool",      sDir + L"\\SnippingTool.exe", L"",            L"System Utility  \x2022  Screen Capture",            sDir + L"\\SnippingTool.exe", true);
    AddAppToList(localApps, L"Windows Settings",   L"ms-settings:",              L"",            L"System Preferences  \x2022  Windows 11 Settings",   sDir + L"\\control.exe", true);
    AddAppToList(localApps, L"Device Manager",     sDir + L"\\mmc.exe",          L"devmgmt.msc", L"System Utility  \x2022  Hardware Devices",          sDir + L"\\mmc.exe", true);
    AddAppToList(localApps, L"Services",           sDir + L"\\mmc.exe",          L"services.msc",L"System Utility  \x2022  Background Services",       sDir + L"\\mmc.exe", true);
    AddAppToList(localApps, L"Resource Monitor",   sDir + L"\\resmon.exe",       L"",            L"System Utility  \x2022  Live Resource Telemetry",   sDir + L"\\resmon.exe", true);
    AddAppToList(localApps, L"System Information", sDir + L"\\msinfo32.exe",     L"",            L"System Utility  \x2022  Hardware & OS Summary",     sDir + L"\\msinfo32.exe", true);

    // Scan Start Menu folders
    wchar_t szPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_STARTMENU, NULL, 0, szPath))) {
        ScanDirectoryForShortcuts(szPath, localApps, 0);
    }
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_COMMON_STARTMENU, NULL, 0, szPath))) {
        ScanDirectoryForShortcuts(szPath, localApps, 0);
    }

    if (InterlockedCompareExchange(&g_bStopRequested, 0, 0) == 0 && g_bCsInitialized) {
        EnterCriticalSection(&g_csApps);
        DestroyAppIconsInList(g_indexedApps);
        g_indexedApps.swap(localApps);
        LeaveCriticalSection(&g_csApps);

        if (g_hMainWnd && IsWindow(g_hMainWnd)) {
            PostMessageW(g_hMainWnd, WM_APP_INDEX_DONE, 0, 0);
        }
    } else {
        DestroyAppIconsInList(localApps);
    }

    if (SUCCEEDED(hrCom)) {
        CoUninitialize();
    }
    return 0;
}

// ============================================================================
// Built-in System Commands
// ============================================================================
struct BuiltinSystemCmd {
    const wchar_t* itemKey;
    const wchar_t* keyword;
    const wchar_t* altKeyword;
    const wchar_t* title;
    const wchar_t* subtitle;
    const wchar_t* iconGlyph;
    SystemActionType action;
};

static const BuiltinSystemCmd kSystemCommands[] = {
    { L"sys:lock",      L"lock",      L"lock workstation", L"Lock Workstation",        L"Session Security  \x2022  Lock desktop immediately",       L"\x229E", SYS_LOCK },
    { L"sys:sleep",     L"sleep",     L"suspend",          L"Sleep System",            L"Power Management  \x2022  Enter low-power standby",        L"\x26B2", SYS_SLEEP },
    { L"sys:restart",   L"restart",   L"reboot",           L"Restart Computer",        L"Power Management  \x2022  Reboot Windows cleanly",         L"\x2672", SYS_RESTART },
    { L"sys:shutdown",  L"shutdown",  L"poweroff",         L"Shut Down Computer",      L"Power Management  \x2022  Power off Windows cleanly",      L"\x2699", SYS_SHUTDOWN },
    { L"sys:emptybin",  L"empty bin", L"recycle bin",      L"Empty Recycle Bin",       L"Storage  \x2022  Clear all Recycle Bin items",             L"\x2672", SYS_EMPTY_BIN }
};

static bool CompareResultPriority(const ResultItem& a, const ResultItem& b) {
    if (a.score != b.score) {
        return a.score > b.score;
    }
    return a.title < b.title;
}

static inline int ComputeListStartY() {
    return ScaleDpi(kBaseWindowHeight) + (ScaleDpi(kBaseDividerSpace) / 2);
}

static void SetSelectedIndexSmooth(int newIndex, bool snapImmediately) {
    int count = (int)g_results.size();
    if (count <= 0) {
        g_selectedIndex = 0;
        g_animSelectionY = (float)ComputeListStartY();
        g_animTargetSelectionY = g_animSelectionY;
        return;
    }
    if (newIndex < 0) {
        newIndex = 0;
    }
    if (newIndex >= count) {
        newIndex = count - 1;
    }
    g_selectedIndex = newIndex;
    g_animTargetSelectionY = (float)(ComputeListStartY() + (newIndex * ScaleDpi(kBaseItemHeight)));
    if (snapImmediately) {
        g_animSelectionY = g_animTargetSelectionY;
    }
}

static int ComputeQueryMatchBaseScore(const std::wstring& lowerQuery,
                                      const std::wstring& lowerName,
                                      const std::wstring& initials) {
    if (lowerQuery.empty() || lowerName.empty()) {
        return 0;
    }

    if (lowerName == lowerQuery) {
        return 1100;
    }

    if (lowerName.find(lowerQuery) == 0) {
        int lenPenalty = (int)(lowerName.size() < 45 ? lowerName.size() : 45);
        return 860 - lenPenalty;
    }

    if (!initials.empty()) {
        if (initials == lowerQuery) {
            return 845;
        }
        if (initials.find(lowerQuery) == 0) {
            return 790;
        }
    }

    size_t pos = lowerName.find(lowerQuery);
    while (pos != std::wstring::npos) {
        if (pos > 0 && !iswalnum(lowerName[pos - 1])) {
            int posPenalty = (int)(pos < 40 ? pos : 40);
            return 760 - posPenalty;
        }
        pos = lowerName.find(lowerQuery, pos + 1);
    }

    size_t subPos = lowerName.find(lowerQuery);
    if (subPos != std::wstring::npos) {
        int posPenalty = (int)(subPos < 50 ? subPos : 50);
        return 620 - posPenalty;
    }

    if (lowerQuery.size() >= 2) {
        int fuzzyBonus = 0;
        if (MatchSubsequenceScore(lowerQuery, lowerName, fuzzyBonus)) {
            int score = 480 + (fuzzyBonus < 120 ? fuzzyBonus : 120);
            return score;
        }
    }

    return 0;
}

static void RebuildResultsForQuery(const std::wstring& rawQuery) {
    bool wasEmpty = g_results.empty();
    g_results.clear();
    g_hoveredIndex = -1;

    std::wstring query = TrimStr(rawQuery);
    g_currentTypedQuery = query;

    if (query.empty()) {
        SetSelectedIndexSmooth(0, true);
        return;
    }

    std::wstring lowerQuery = ToLowerStr(query);

    // 1. Terminal Command Runner Prefix ('>')
    if (query[0] == L'>') {
        std::wstring cmdText = TrimStr(query.substr(1));
        ResultItem item;
        item.itemKey = L"cmd:" + ToLowerStr(cmdText);
        item.category = CAT_COMMAND;
        item.badge = L"[Command]";
        item.actionHint = L"\x21B5 Run";
        item.iconGlyph = L">_";
        item.hAppIcon = NULL;
        item.sysAction = SYS_NONE;
        item.learnedUseCount = 0;
        item.isPrefixHabitMatch = false;
        item.score = 3000;

        std::wstring sDir = GetSystem32OrSysnativePath();
        if (cmdText.empty()) {
            item.title = L"Run Terminal Command...";
            item.subtitle = L"Type command after '>' (e.g. > ipconfig /all or > ping 1.1.1.1)";
            item.execPath = sDir + L"\\cmd.exe";
            item.execArgs = L"";
        } else {
            item.title = cmdText;
            item.subtitle = L"Terminal Execution  \x2022  cmd.exe /K " + cmdText;
            item.execPath = sDir + L"\\cmd.exe";
            item.execArgs = L"/K " + cmdText;
        }
        g_results.push_back(item);
        SetSelectedIndexSmooth(0, wasEmpty);
        return;
    }

    // 2. Instant Calculator
    std::wstring calcOutput;
    if (TryEvaluateMath(query, calcOutput)) {
        ResultItem item;
        item.itemKey = L"calc:expr";
        item.title = L"= " + calcOutput;
        item.subtitle = L"Expression (" + query + L")  \x2022  Press Enter to copy result";
        item.badge = L"[Calculator]";
        item.actionHint = L"\x21B5 Copy";
        item.iconGlyph = L"\x00B1";
        item.hAppIcon = NULL;
        item.category = CAT_CALC;
        item.clipboardText = calcOutput;
        item.sysAction = SYS_NONE;
        item.learnedUseCount = 0;
        item.isPrefixHabitMatch = false;
        item.score = 2800;
        g_results.push_back(item);
    }

    // 3. Quick System Commands
    size_t sysCount = sizeof(kSystemCommands) / sizeof(kSystemCommands[0]);
    for (size_t i = 0; i < sysCount; ++i) {
        std::wstring kw1 = kSystemCommands[i].keyword;
        std::wstring kw2 = kSystemCommands[i].altKeyword;
        std::wstring init1 = ExtractInitialsStr(kSystemCommands[i].title);

        int m1 = ComputeQueryMatchBaseScore(lowerQuery, kw1, init1);
        int m2 = ComputeQueryMatchBaseScore(lowerQuery, kw2, init1);
        int baseScore = (m1 > m2) ? m1 : m2;

        if (baseScore > 0) {
            ResultItem item;
            item.itemKey = kSystemCommands[i].itemKey;
            item.title = kSystemCommands[i].title;
            item.subtitle = kSystemCommands[i].subtitle;
            item.badge = L"[System]";
            item.actionHint = L"\x21B5 Run";
            item.iconGlyph = kSystemCommands[i].iconGlyph;
            item.hAppIcon = NULL;
            item.category = CAT_SYSTEM;
            item.sysAction = kSystemCommands[i].action;
            item.learnedUseCount = 0;
            item.isPrefixHabitMatch = false;
            item.score = baseScore;
            g_results.push_back(item);
        }
    }

    // 4. Direct URL Detection
    std::wstring detectedUrl;
    bool isDirectUrl = IsLikelyUrlOrDomain(query, detectedUrl);
    if (isDirectUrl) {
        std::wstring urlKey = L"url:" + ToLowerStr(detectedUrl);
        DWORD useCount = 0;
        bool isHabit = false;
        int frecencyBoost = ComputeFrecencyBoost(urlKey, lowerQuery, useCount, isHabit);

        ResultItem item;
        item.itemKey = urlKey;
        item.title = detectedUrl;
        item.subtitle = L"Direct URL  \x2022  Open in default web browser";
        item.badge = L"[Web]";
        item.actionHint = L"\x21B5 Open";
        item.iconGlyph = L"\x21D2";
        item.hAppIcon = NULL;
        item.category = CAT_WEB;
        item.execPath = detectedUrl;
        item.sysAction = SYS_NONE;
        item.learnedUseCount = useCount;
        item.isPrefixHabitMatch = isHabit;
        item.score = 950 + frecencyBoost;
        g_results.push_back(item);
    }

    // 5. Application Search + Adaptive Frecency Ranking
    std::vector<ResultItem> appMatches;
    if (g_bCsInitialized) {
        EnterCriticalSection(&g_csApps);
        for (size_t i = 0; i < g_indexedApps.size(); ++i) {
            const IndexedApp& app = g_indexedApps[i];
            int baseScore = ComputeQueryMatchBaseScore(lowerQuery, app.lowerName, app.initials);

            std::wstring appKey = L"app:" + app.lowerName;
            DWORD useCount = 0;
            bool isHabit = false;
            int frecencyBoost = ComputeFrecencyBoost(appKey, lowerQuery, useCount, isHabit);

            if (baseScore > 0 || isHabit) {
                if (app.isSystemTool) {
                    baseScore += 15;
                }

                ResultItem item;
                item.itemKey = appKey;
                item.title = app.name;
                item.subtitle = app.description;
                if (useCount > 0) {
                    wchar_t freqTag[64];
                    if (isHabit) {
                        swprintf(freqTag, 64, L"  \x2022  \x2605 Top Pick (%lu\x00D7)", (unsigned long)useCount);
                    } else {
                        swprintf(freqTag, 64, L"  \x2022  Opened %lu\x00D7", (unsigned long)useCount);
                    }
                    item.subtitle += freqTag;
                }
                item.badge = L"[App]";
                item.actionHint = L"\x21B5 Open";
                item.iconGlyph = app.isSystemTool ? L"\x2699" : L"\x229E";
                item.hAppIcon = app.hIcon;
                item.category = CAT_APP;
                item.execPath = app.targetPath;
                item.execArgs = app.arguments;
                item.sysAction = SYS_NONE;
                item.learnedUseCount = useCount;
                item.isPrefixHabitMatch = isHabit;
                item.score = baseScore + frecencyBoost;
                appMatches.push_back(item);
            }
        }
        LeaveCriticalSection(&g_csApps);
    }

    for (size_t i = 0; i < appMatches.size(); ++i) {
        g_results.push_back(appMatches[i]);
    }

    std::sort(g_results.begin(), g_results.end(), CompareResultPriority);

    // 6. Google Web Search Fallback
    if (!isDirectUrl) {
        ResultItem webItem;
        webItem.itemKey = L"web:" + lowerQuery;
        webItem.title = L"Search Google for \"" + query + L"\"";
        webItem.subtitle = L"Web Search  \x2022  google.com/search?q=" + UrlEncodeUtf8(query);
        webItem.badge = L"[Web]";
        webItem.actionHint = L"\x21B5 Search";
        webItem.iconGlyph = L"\x21D2";
        webItem.hAppIcon = NULL;
        webItem.category = CAT_WEB;
        webItem.execPath = L"https://www.google.com/search?q=" + UrlEncodeUtf8(query);
        webItem.sysAction = SYS_NONE;
        webItem.learnedUseCount = 0;
        webItem.isPrefixHabitMatch = false;
        webItem.score = 100;
        g_results.push_back(webItem);
    }

    if ((int)g_results.size() > kMaxVisibleResults) {
        g_results.resize(kMaxVisibleResults);
    }

    SetSelectedIndexSmooth(0, wasEmpty);
}

// ============================================================================
// Outer Window Region & DWM Attributes
// ============================================================================
static void UpdateOuterMainWindowShapeRegion(HWND hwnd, int width, int height) {
    if (!hwnd || width <= 0 || height <= 0) {
        return;
    }

    if (g_cfg.useCustomOuterRegion && g_cfg.outerWindowEllipse > 0) {
        int scaledEllipse = ScaleDpi(g_cfg.outerWindowEllipse);
        int maxAllowed = (height < width) ? height : width;
        if (scaledEllipse > maxAllowed) {
            scaledEllipse = maxAllowed;
        }
        HRGN hRgn = CreateRoundRectRgn(0, 0, width + 1, height + 1, scaledEllipse, scaledEllipse);
        SetWindowRgn(hwnd, hRgn, TRUE);
    } else {
        SetWindowRgn(hwnd, NULL, TRUE);
    }
}

static void ApplyLiquidGlassDwmAttributes(HWND hwnd) {
    if (!hwnd) {
        return;
    }

    BOOL bDarkMode = g_cfg.isLightMode ? FALSE : TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &bDarkMode, sizeof(bDarkMode));

    DWM_WINDOW_CORNER_PREFERENCE cornerPref = g_cfg.dwmCornerPref;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPref, sizeof(cornerPref));

    DWM_SYSTEMBACKDROP_TYPE backdropType = DWMSBT_TRANSIENTWINDOW;
    DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType, sizeof(backdropType));

    COLORREF dwmBorder = g_cfg.useCustomOuterRegion ? DWMWA_COLOR_NONE : g_cfg.colBorderGlass;
    DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &dwmBorder, sizeof(dwmBorder));

    MARGINS margins = { -1, -1, -1, -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    RECT rc;
    GetClientRect(hwnd, &rc);
    UpdateOuterMainWindowShapeRegion(hwnd, rc.right - rc.left, rc.bottom - rc.top);
}

static void RefreshEditControlColorsAndLayout() {
    if (g_hEditBgBrush) {
        DeleteObject(g_hEditBgBrush);
        g_hEditBgBrush = NULL;
    }
    g_hEditBgBrush = CreateSolidBrush(g_cfg.colHeaderSurface);

    if (g_hEditWnd) {
        SetWindowTheme(g_hEditWnd, g_cfg.isLightMode ? L"Explorer" : L"DarkMode_Explorer", NULL);

        int editLeft = ScaleDpi((g_cfg.shapeMode == SHAPE_PILL_CAPSULE) ? 72 : 66);
        int rightReserve = ScaleDpi(g_cfg.showShortcutHint
                           ? ((g_cfg.shapeMode == SHAPE_PILL_CAPSULE) ? 140 : 132)
                           : ((g_cfg.shapeMode == SHAPE_PILL_CAPSULE) ? 38 : 28));
        int winW = ScaleDpi(kBaseWindowWidth);
        int editWidth = winW - editLeft - rightReserve;
        int editTop = ScaleDpi(22);
        int editHeight = ScaleDpi(28);

        SetWindowPos(g_hEditWnd, NULL, editLeft, editTop, editWidth, editHeight, SWP_NOZORDER | SWP_NOACTIVATE);
        InvalidateRect(g_hEditWnd, NULL, TRUE);
    }
}

// ============================================================================
// 120Hz Animation Engine & Window Sizing
// ============================================================================
static int ComputeTargetWindowHeight() {
    int count = (int)g_results.size();
    if (count > kMaxVisibleResults) {
        count = kMaxVisibleResults;
    }
    int targetHeight = ScaleDpi(kBaseWindowHeight);
    if (count > 0) {
        int bottomExtra = g_cfg.showFooterBar ? ScaleDpi(kBaseFooterHeight) : ScaleDpi(kBaseBottomPadNoFooter);
        if (!g_cfg.showFooterBar && g_cfg.useCustomOuterRegion) {
            bottomExtra += ScaleDpi(6);
        }
        targetHeight += ScaleDpi(kBaseDividerSpace) + (count * ScaleDpi(kBaseItemHeight)) + bottomExtra;
    }
    return targetHeight;
}

static void RequestSmoothHeightUpdate() {
    if (!g_hMainWnd) {
        return;
    }
    g_animTargetHeight = (float)ComputeTargetWindowHeight();
    SetTimer(g_hMainWnd, kAnimTimerId, kAnimFrameIntervalMs, NULL);
    InvalidateRect(g_hMainWnd, NULL, FALSE);
}

static void StepLauncherAnimationFrame(HWND hwnd) {
    if (!g_bWindowVisible) {
        KillTimer(hwnd, kAnimTimerId);
        return;
    }

    bool needsRepaint = false;
    bool needsWindowPos = false;

    // 1. Smooth Height
    float heightDiff = g_animTargetHeight - g_animCurHeight;
    if (std::fabs(heightDiff) > 0.45f) {
        g_animCurHeight += heightDiff * 0.35f;
        needsWindowPos = true;
        needsRepaint = true;
    } else if (g_animCurHeight != g_animTargetHeight) {
        g_animCurHeight = g_animTargetHeight;
        needsWindowPos = true;
        needsRepaint = true;
    }

    // 2. Smooth Gliding Selection
    float selDiff = g_animTargetSelectionY - g_animSelectionY;
    if (std::fabs(selDiff) > 0.35f) {
        g_animSelectionY += selDiff * 0.38f;
        needsRepaint = true;
    } else if (g_animSelectionY != g_animTargetSelectionY) {
        g_animSelectionY = g_animTargetSelectionY;
        needsRepaint = true;
    }

    // 3. Hover Blend
    float hoverTarget = (g_hoveredIndex >= 0 && g_hoveredIndex != g_selectedIndex) ? 1.0f : 0.0f;
    float hoverDiff = hoverTarget - g_animHoverBlend;
    if (std::fabs(hoverDiff) > 0.02f) {
        g_animHoverBlend += hoverDiff * 0.30f;
        needsRepaint = true;
    } else if (g_animHoverBlend != hoverTarget) {
        g_animHoverBlend = hoverTarget;
        needsRepaint = true;
    }

    // 4. Alpha Entrance
    float targetAlpha = (float)g_cfg.targetWindowAlpha;
    float alphaDiff = targetAlpha - g_animWindowAlpha;
    if (std::fabs(alphaDiff) > 1.2f) {
        g_animWindowAlpha += alphaDiff * 0.35f;
        SetLayeredWindowAttributes(hwnd, 0, (BYTE)ClampInt((int)g_animWindowAlpha, 40, 255), LWA_ALPHA);
        needsRepaint = true;
    } else if (g_animWindowAlpha != targetAlpha) {
        g_animWindowAlpha = targetAlpha;
        SetLayeredWindowAttributes(hwnd, 0, g_cfg.targetWindowAlpha, LWA_ALPHA);
        needsRepaint = true;
    }

    // Slide Offset
    if (std::fabs(g_animSlideOffsetY) > 0.3f) {
        g_animSlideOffsetY *= 0.68f;
        needsWindowPos = true;
    } else if (g_animSlideOffsetY != 0.0f) {
        g_animSlideOffsetY = 0.0f;
        needsWindowPos = true;
    }

    // Breathing pulse
    g_animPulsePhase += 0.075f;
    if (g_animPulsePhase > 6.2831853f) {
        g_animPulsePhase -= 6.2831853f;
    }

    if (needsWindowPos) {
        int winW = ScaleDpi(kBaseWindowWidth);
        int newH = (int)(g_animCurHeight + 0.5f);
        if (newH < ScaleDpi(kBaseWindowHeight)) {
            newH = ScaleDpi(kBaseWindowHeight);
        }
        int newY = g_anchorPosY + (int)(g_animSlideOffsetY);
        SetWindowPos(hwnd, HWND_TOPMOST, g_anchorPosX, newY, winW, newH,
                     SWP_NOACTIVATE | SWP_NOZORDER);
        if (g_cfg.useCustomOuterRegion) {
            UpdateOuterMainWindowShapeRegion(hwnd, winW, newH);
        }
    }

    if (needsRepaint) {
        InvalidateRect(hwnd, NULL, FALSE);
    } else if (!needsWindowPos && (std::fabs(heightDiff) < 0.1f) && (std::fabs(selDiff) < 0.1f)) {
        // Disarm timer when fully settled to save CPU
        KillTimer(hwnd, kAnimTimerId);
    }
}

static void HideLauncherWindow() {
    if (!g_hMainWnd || !g_bWindowVisible) {
        return;
    }
    g_bWindowVisible = false;
    g_pendingConfirmAction = SYS_NONE;
    KillTimer(g_hMainWnd, kAnimTimerId);
    ShowWindow(g_hMainWnd, SW_HIDE);
}

static void ShowLauncherWindow() {
    if (!g_hMainWnd) {
        return;
    }

    POINT pt;
    GetCursorPos(&pt);
    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi;
    ZeroMemory(&mi, sizeof(mi));
    mi.cbSize = sizeof(mi);

    // Query per-monitor DPI dynamically
    UINT dpiX = 96, dpiY = 96;
    HMODULE hShcore = GetModuleHandleW(L"shcore.dll");
    if (!hShcore) hShcore = LoadLibraryW(L"shcore.dll");
    if (hShcore) {
        typedef HRESULT (WINAPI *GetDpiForMonitor_t)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);
        GetDpiForMonitor_t pGetDpi = (GetDpiForMonitor_t)GetProcAddress(hShcore, "GetDpiForMonitor");
        if (pGetDpi) {
            pGetDpi(hMon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
        }
    }
    if (dpiX > 0 && dpiX != g_currentDpi) {
        g_currentDpi = dpiX;
        CreateLauncherGdiResources();
    }

    int winW = ScaleDpi(kBaseWindowWidth);
    int baseH = ScaleDpi(kBaseWindowHeight);

    g_anchorPosX = 300;
    g_anchorPosY = 200;
    if (GetMonitorInfoW(hMon, &mi)) {
        int workW = mi.rcWork.right - mi.rcWork.left;
        int workH = mi.rcWork.bottom - mi.rcWork.top;
        g_anchorPosX = mi.rcWork.left + (workW - winW) / 2;
        g_anchorPosY = mi.rcWork.top + (workH / 5);
    }

    if (g_hEditWnd) {
        SetWindowTextW(g_hEditWnd, L"");
    }
    g_results.clear();
    g_currentTypedQuery.clear();
    g_selectedIndex = 0;
    g_hoveredIndex = -1;
    g_pendingConfirmAction = SYS_NONE;

    g_animCurHeight = (float)baseH;
    g_animTargetHeight = (float)baseH;
    g_animSelectionY = (float)ComputeListStartY();
    g_animTargetSelectionY = g_animSelectionY;
    g_animHoverBlend = 0.0f;

    float startAlpha = (float)g_cfg.targetWindowAlpha * 0.58f;
    if (startAlpha < 70.0f) startAlpha = 70.0f;
    g_animWindowAlpha = startAlpha;
    g_animSlideOffsetY = (float)(-ScaleDpi(12));

    RefreshEditControlColorsAndLayout();
    SetLayeredWindowAttributes(g_hMainWnd, 0, (BYTE)((int)g_animWindowAlpha), LWA_ALPHA);
    UpdateOuterMainWindowShapeRegion(g_hMainWnd, winW, baseH);
    SetWindowPos(g_hMainWnd, HWND_TOPMOST,
                 g_anchorPosX, g_anchorPosY + (int)g_animSlideOffsetY,
                 winW, baseH,
                 SWP_SHOWWINDOW);
    g_bWindowVisible = true;

    // Safe foreground activation without dangerous AttachThreadInput
    INPUT emptyInput = { 0 };
    emptyInput.type = INPUT_MOUSE;
    SendInput(1, &emptyInput, sizeof(INPUT));
    SetForegroundWindow(g_hMainWnd);
    SetFocus(g_hEditWnd);

    SetTimer(g_hMainWnd, kAnimTimerId, kAnimFrameIntervalMs, NULL);
    InvalidateRect(g_hMainWnd, NULL, FALSE);
}

static void ExecuteSelectedResult() {
    if (g_results.empty()) {
        return;
    }
    int idx = g_selectedIndex;
    if (idx < 0) idx = 0;
    if (idx >= (int)g_results.size()) idx = (int)g_results.size() - 1;

    ResultItem item = g_results[idx];
    std::wstring querySnapshot = g_currentTypedQuery;

    // Two-step confirmation for destructive actions
    if (item.category == CAT_SYSTEM) {
        bool isDestructive = (item.sysAction == SYS_RESTART || item.sysAction == SYS_SHUTDOWN || item.sysAction == SYS_EMPTY_BIN);
        if (isDestructive) {
            DWORD now = GetTickCount();
            if (g_pendingConfirmAction != item.sysAction || (now - g_dwConfirmArmTime > 4000)) {
                g_pendingConfirmAction = item.sysAction;
                g_dwConfirmArmTime = now;
                g_results[idx].subtitle = L"\x26A0 Press Enter again within 4s to confirm!";
                InvalidateRect(g_hMainWnd, NULL, FALSE);
                return;
            }
        }
    }

    if (!item.itemKey.empty() && item.category != CAT_CALC && item.category != CAT_SYSTEM) {
        RecordItemExecutionFrecency(item.itemKey, querySnapshot);
    }

    HideLauncherWindow();

    if (item.category == CAT_CALC) {
        CopyTextToClipboard(g_hMainWnd, item.clipboardText);
        return;
    }

    if (item.category == CAT_SYSTEM) {
        switch (item.sysAction) {
            case SYS_LOCK:
                LockWorkStation();
                break;
            case SYS_SLEEP:
                SetSuspendState(FALSE, FALSE, FALSE);
                break;
            case SYS_RESTART:
                ShellExecuteW(NULL, L"open", L"shutdown.exe", L"/r /t 0", NULL, SW_HIDE);
                break;
            case SYS_SHUTDOWN:
                ShellExecuteW(NULL, L"open", L"shutdown.exe", L"/s /t 0", NULL, SW_HIDE);
                break;
            case SYS_EMPTY_BIN:
                // Shell shows confirmation natively
                SHEmptyRecycleBinW(NULL, NULL, 0);
                break;
            default:
                break;
        }
        return;
    }

    if (!item.execPath.empty()) {
        const wchar_t* args = item.execArgs.empty() ? NULL : item.execArgs.c_str();
        ShellExecuteW(NULL, L"open", item.execPath.c_str(), args, NULL, SW_SHOWNORMAL);
    }
}

// ============================================================================
// Double-Buffered GDI Rendering (Per-Monitor DPI Scaled)
// ============================================================================
static void FillSolidRectGdi(HDC hdc, const RECT* rc, COLORREF color) {
    HBRUSH hBrush = CreateSolidBrush(color);
    FillRect(hdc, rc, hBrush);
    DeleteObject(hBrush);
}

static void DrawShapeCardGdi(HDC hdc, int left, int top, int right, int bottom, int radius, COLORREF fillCol, COLORREF borderCol) {
    HBRUSH hBrush = CreateSolidBrush(fillCol);
    HPEN hPen = CreatePen(PS_SOLID, 1, borderCol);
    HGDIOBJ oldBrush = SelectObject(hdc, hBrush);
    HGDIOBJ oldPen = SelectObject(hdc, hPen);

    if (radius <= 0) {
        Rectangle(hdc, left, top, right, bottom);
    } else {
        int h = bottom - top;
        int w = right - left;
        int maxR = (h < w ? h : w);
        int ellipseDiameter = ScaleDpi(radius * 2);
        if (ellipseDiameter > maxR) {
            ellipseDiameter = maxR;
        }
        RoundRect(hdc, left, top, right, bottom, ellipseDiameter, ellipseDiameter);
    }

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}

static void DrawOuterWindowBorderGdi(HDC hdc, int width, int height) {
    if (!g_cfg.useCustomOuterRegion || g_cfg.outerWindowEllipse <= 0) {
        return;
    }
    int maxAllowed = (height < width) ? height : width;
    int ellipse = ScaleDpi(g_cfg.outerWindowEllipse);
    if (ellipse > maxAllowed) {
        ellipse = maxAllowed;
    }

    HPEN hPen = CreatePen(PS_SOLID, 2, g_cfg.colBorderGlass);
    HGDIOBJ oldPen = SelectObject(hdc, hPen);
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));

    RoundRect(hdc, 1, 1, width - 1, height - 1, ellipse, ellipse);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}

static void DrawCircleDotGdi(HDC hdc, int centerX, int centerY, int radius, COLORREF color) {
    HBRUSH hBrush = CreateSolidBrush(color);
    HPEN hPen = CreatePen(PS_SOLID, 1, color);
    HGDIOBJ oldBrush = SelectObject(hdc, hBrush);
    HGDIOBJ oldPen = SelectObject(hdc, hPen);
    int r = ScaleDpi(radius);
    Ellipse(hdc, centerX - r, centerY - r, centerX + r, centerY + r);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}

static void DrawSearchLensIcon(HDC hdc, int centerX, int centerY, float pulsePhase) {
    float glowFactor = 0.5f + 0.5f * std::sin(pulsePhase);
    COLORREF lensCol = BlendColorRgb(g_cfg.colAccentPrimary, g_cfg.colAccentGlow, glowFactor);

    float haloStrength = g_cfg.isLightMode ? (0.10f + 0.05f * glowFactor) : (0.14f + 0.08f * glowFactor);
    COLORREF haloCol = BlendColorRgb(g_cfg.colHeaderSurface, g_cfg.colAccentPrimary, haloStrength);
    DrawCircleDotGdi(hdc, centerX, centerY, 15, haloCol);

    HPEN hPen = CreatePen(PS_SOLID, 2, lensCol);
    HGDIOBJ oldPen = SelectObject(hdc, hPen);
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));

    int r = ScaleDpi(7);
    Ellipse(hdc, centerX - r - 1, centerY - r - 1, centerX + r - 1, centerY + r - 1);
    MoveToEx(hdc, centerX + ScaleDpi(4), centerY + ScaleDpi(4), NULL);
    LineTo(hdc, centerX + ScaleDpi(10), centerY + ScaleDpi(10));

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(hPen);
}

static int DrawKeycapPill(HDC hdc, int rightEdge, int centerY, const wchar_t* label, bool isHighlighted) {
    HGDIOBJ oldFont = SelectObject(hdc, g_hFontKeycap);
    RECT rcCalc = { 0, 0, 0, 0 };
    DrawTextW(hdc, label, -1, &rcCalc, DT_CALCRECT | DT_SINGLELINE);
    int w = (rcCalc.right - rcCalc.left) + ScaleDpi(18);
    int h = ScaleDpi(24);
    int left = rightEdge - w;
    int top = centerY - (h / 2);
    int bottom = top + h;

    COLORREF bg  = isHighlighted ? g_cfg.colKeycapActBg  : g_cfg.colKeycapBg;
    COLORREF bdr = isHighlighted ? g_cfg.colKeycapActBdr : g_cfg.colKeycapBorder;
    COLORREF txt = isHighlighted ? g_cfg.colTextPrimary  : g_cfg.colTextSub;

    DrawShapeCardGdi(hdc, left, top, rightEdge, bottom, g_cfg.keycapRadius, bg, bdr);
    SetTextColor(hdc, txt);
    RECT rcText = { left, top, rightEdge, bottom };
    DrawTextW(hdc, label, -1, &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, oldFont);
    return left;
}

static void PaintLauncherWindow(HWND hwnd, HDC hdcScreen) {
    RECT rcClient;
    GetClientRect(hwnd, &rcClient);
    int width = rcClient.right - rcClient.left;
    int height = rcClient.bottom - rcClient.top;
    if (width <= 0 || height <= 0) {
        return;
    }

    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmMem = CreateCompatibleBitmap(hdcScreen, width, height);
    HGDIOBJ hOldBmp = SelectObject(hdcMem, hbmMem);

    SetBkMode(hdcMem, TRANSPARENT);

    // Base Liquid Glass
    FillSolidRectGdi(hdcMem, &rcClient, g_cfg.colBgSurface);

    // Specular Top Rim Highlight
    int rimInset = (g_cfg.shapeMode == SHAPE_SHARP_BOX) ? ScaleDpi(2) : ScaleDpi(g_cfg.outerWindowEllipse / 2 + 8);
    if (width - rimInset > rimInset) {
        RECT rcTopRim = { rimInset, 1, width - rimInset, 2 };
        FillSolidRectGdi(hdcMem, &rcTopRim, g_cfg.colGlassRimTop);
    }

    int baseH = ScaleDpi(kBaseWindowHeight);
    int lensCenterX = ScaleDpi((g_cfg.shapeMode == SHAPE_PILL_CAPSULE) ? 42 : 38);
    DrawSearchLensIcon(hdcMem, lensCenterX, baseH / 2, g_animPulsePhase);

    if (g_cfg.showShortcutHint) {
        int badgeRightEdge = width - ScaleDpi((g_cfg.shapeMode == SHAPE_PILL_CAPSULE) ? 28 : 22);
        const wchar_t* hint = L"Alt+Space";
        if (g_cfg.hotkeyChoice == HOTKEY_CTRL_SPACE) hint = L"Ctrl+Space";
        else if (g_cfg.hotkeyChoice == HOTKEY_WIN_ALT_SPACE) hint = L"Win+Alt+Space";
        else if (g_cfg.hotkeyChoice == HOTKEY_ALT_Q) hint = L"Alt+Q";
        DrawKeycapPill(hdcMem, badgeRightEdge, baseH / 2, hint, false);
    }

    int count = (int)g_results.size();
    if (count > kMaxVisibleResults) {
        count = kMaxVisibleResults;
    }

    HGDIOBJ hOldFont = SelectObject(hdcMem, g_hFontTitle);

    if (count > 0 || height > baseH + 4) {
        int sepInset = ScaleDpi((g_cfg.shapeMode == SHAPE_PILL_CAPSULE) ? 26 : 18);
        RECT rcSep = { sepInset, baseH, width - sepInset, baseH + 1 };
        FillSolidRectGdi(hdcMem, &rcSep, g_cfg.colDivider);

        int listStartY = ComputeListStartY();
        int itemH = ScaleDpi(kBaseItemHeight);
        int bottomReserve = g_cfg.showFooterBar ? ScaleDpi(kBaseFooterHeight) : 0;
        int footerTop = height - bottomReserve;

        int clipBottom = (footerTop > listStartY) ? footerTop : height;
        HRGN hClipRgn = CreateRectRgn(0, baseH + 1, width, clipBottom);
        SelectClipRgn(hdcMem, hClipRgn);

        int cardSideMargin = ScaleDpi((g_cfg.shapeMode == SHAPE_PILL_CAPSULE) ? 16 : 12);

        // Hover Card
        if (g_hoveredIndex >= 0 && g_hoveredIndex < count && g_hoveredIndex != g_selectedIndex && g_animHoverBlend > 0.02f) {
            int hovTop = listStartY + (g_hoveredIndex * itemH);
            int hovBottom = hovTop + itemH;
            COLORREF hovFill = BlendColorRgb(g_cfg.colBgSurface, g_cfg.colItemHover, g_animHoverBlend);
            COLORREF hovBdr  = BlendColorRgb(g_cfg.colBgSurface, g_cfg.colItemHoverBdr, g_animHoverBlend);
            DrawShapeCardGdi(hdcMem, cardSideMargin, hovTop + 3, width - cardSideMargin, hovBottom - 3,
                             g_cfg.itemCardRadius, hovFill, hovBdr);
        }

        // Active Selection Capsule
        if (count > 0) {
            int selTop = (int)(g_animSelectionY + 0.5f);
            int selBottom = selTop + itemH;
            int cardLeft = cardSideMargin;
            int cardRight = width - cardSideMargin;
            int cardTop = selTop + 2;
            int cardBottom = selBottom - 2;

            DrawShapeCardGdi(hdcMem, cardLeft, cardTop, cardRight, cardBottom,
                             g_cfg.itemCardRadius, g_cfg.colItemActive, g_cfg.colItemBorderAct);

            int rimPad = ScaleDpi((g_cfg.itemCardRadius > 14) ? g_cfg.itemCardRadius : 12);
            if (cardRight - rimPad > cardLeft + rimPad) {
                RECT rcCapsuleRim = { cardLeft + rimPad, cardTop + 1, cardRight - rimPad, cardTop + 2 };
                FillSolidRectGdi(hdcMem, &rcCapsuleRim, g_cfg.colItemRimAct);
            }

            int pillLeft   = cardLeft + ScaleDpi((g_cfg.shapeMode == SHAPE_PILL_CAPSULE) ? 14 : 6);
            int pillVInset = ScaleDpi((g_cfg.shapeMode == SHAPE_PILL_CAPSULE) ? 16 : 13);
            int pillRadius = (g_cfg.shapeMode == SHAPE_SHARP_BOX) ? 0 : 4;
            DrawShapeCardGdi(hdcMem, pillLeft, cardTop + pillVInset, pillLeft + ScaleDpi(4), cardBottom - pillVInset,
                             pillRadius, g_cfg.colAccentPrimary, g_cfg.colAccentGlow);
        }

        // Result Rows
        int tileBoxSize = ScaleDpi(42);
        for (int i = 0; i < count; ++i) {
            const ResultItem& item = g_results[i];
            int itemTop = listStartY + (i * itemH);
            int itemBottom = itemTop + itemH;
            int itemCenterY = itemTop + (itemH / 2);

            bool isSelected = (i == g_selectedIndex);
            bool isHovered = (i == g_hoveredIndex);

            int iconBoxLeft = ScaleDpi((g_cfg.shapeMode == SHAPE_PILL_CAPSULE) ? 38 : 28);
            int iconBoxTop = itemTop + (itemH - tileBoxSize) / 2;
            int iconBoxRight = iconBoxLeft + tileBoxSize;
            int iconBoxBottom = iconBoxTop + tileBoxSize;

            COLORREF iconTileBg  = isSelected ? g_cfg.colIconTileActBg  : g_cfg.colIconTileBg;
            COLORREF iconTileBdr = isSelected ? g_cfg.colIconTileActBdr : g_cfg.colIconTileBdr;
            DrawShapeCardGdi(hdcMem, iconBoxLeft, iconBoxTop, iconBoxRight, iconBoxBottom,
                             g_cfg.iconTileRadius, iconTileBg, iconTileBdr);

            if (item.hAppIcon != NULL) {
                DrawIconEx(hdcMem, iconBoxLeft + ScaleDpi(5), iconBoxTop + ScaleDpi(5), item.hAppIcon,
                           ScaleDpi(32), ScaleDpi(32), 0, NULL, DI_NORMAL);
            } else {
                SelectObject(hdcMem, g_hFontIcon);
                SetTextColor(hdcMem, isSelected ? g_cfg.colAccentPrimary : g_cfg.colBadgeText);
                RECT rcGlyph = { iconBoxLeft, iconBoxTop, iconBoxRight, iconBoxBottom };
                DrawTextW(hdcMem, item.iconGlyph.c_str(), -1, &rcGlyph, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }

            if (item.isPrefixHabitMatch) {
                DrawCircleDotGdi(hdcMem, iconBoxRight - ScaleDpi(4), iconBoxTop + ScaleDpi(4), 4, g_cfg.colFreqGold);
            }

            int rightCursor = width - ScaleDpi((g_cfg.shapeMode == SHAPE_PILL_CAPSULE) ? 34 : 26);

            if (g_cfg.showShortcutHint && (isSelected || isHovered)) {
                rightCursor = DrawKeycapPill(hdcMem, rightCursor, itemCenterY, item.actionHint.c_str(), isSelected) - ScaleDpi(8);
            }

            if (g_cfg.showCategoryBadges) {
                SelectObject(hdcMem, g_hFontBadge);
                RECT rcBadgeCalc = { 0, 0, 0, 0 };
                DrawTextW(hdcMem, item.badge.c_str(), -1, &rcBadgeCalc, DT_CALCRECT | DT_SINGLELINE);
                int badgeW = (rcBadgeCalc.right - rcBadgeCalc.left) + ScaleDpi(20);
                int badgeH = ScaleDpi(24);
                int badgeRight = rightCursor;
                int badgeLeft = badgeRight - badgeW;
                int badgeTop = itemCenterY - (badgeH / 2);
                int badgeBottom = badgeTop + badgeH;

                COLORREF badgeFill   = isSelected ? g_cfg.colBadgeActive : g_cfg.colBadgeBg;
                COLORREF badgeBorder = isSelected ? g_cfg.colAccentPrimary : g_cfg.colBadgeBorder;
                DrawShapeCardGdi(hdcMem, badgeLeft, badgeTop, badgeRight, badgeBottom,
                                 g_cfg.badgeRadius, badgeFill, badgeBorder);

                SetTextColor(hdcMem, isSelected ? g_cfg.colBadgeTextActive : g_cfg.colBadgeText);
                RECT rcBadgeText = { badgeLeft, badgeTop, badgeRight, badgeBottom };
                DrawTextW(hdcMem, item.badge.c_str(), -1, &rcBadgeText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

                rightCursor = badgeLeft - ScaleDpi(14);
            } else {
                rightCursor -= ScaleDpi(6);
            }

            int textLeft = iconBoxRight + ScaleDpi(15);
            int textRightLimit = rightCursor;

            if (g_cfg.showSubtitles) {
                SelectObject(hdcMem, g_hFontTitle);
                SetTextColor(hdcMem, g_cfg.colTextPrimary);
                RECT rcTitle = { textLeft, itemTop + ScaleDpi(9), textRightLimit, itemTop + ScaleDpi(32) };
                DrawTextW(hdcMem, item.title.c_str(), -1, &rcTitle, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);

                SelectObject(hdcMem, g_hFontSub);
                SetTextColor(hdcMem, isSelected ? g_cfg.colTextSubActive : g_cfg.colTextSub);
                RECT rcSub = { textLeft, itemTop + ScaleDpi(32), textRightLimit, itemBottom - ScaleDpi(8) };
                DrawTextW(hdcMem, item.subtitle.c_str(), -1, &rcSub, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
            } else {
                SelectObject(hdcMem, g_hFontTitle);
                SetTextColor(hdcMem, g_cfg.colTextPrimary);
                RECT rcTitle = { textLeft, itemTop, textRightLimit, itemBottom };
                DrawTextW(hdcMem, item.title.c_str(), -1, &rcTitle, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
            }
        }

        SelectClipRgn(hdcMem, NULL);
        DeleteObject(hClipRgn);

        // Optional Bottom Footer
        int footH = ScaleDpi(kBaseFooterHeight);
        if (g_cfg.showFooterBar && height > baseH + footH) {
            RECT rcFooter = { 0, footerTop, width, height };
            FillSolidRectGdi(hdcMem, &rcFooter, g_cfg.colFooterSurface);

            RECT rcFooterLine = { 0, footerTop, width, footerTop + 1 };
            FillSolidRectGdi(hdcMem, &rcFooterLine, g_cfg.colDivider);

            int footerInset = ScaleDpi((g_cfg.shapeMode == SHAPE_PILL_CAPSULE) ? 34 : 24);
            int footerCenterY = footerTop + (footH / 2);
            DrawCircleDotGdi(hdcMem, footerInset, footerCenterY, 4, kColDotRed);
            DrawCircleDotGdi(hdcMem, footerInset + ScaleDpi(14), footerCenterY, 4, kColDotAmber);
            DrawCircleDotGdi(hdcMem, footerInset + ScaleDpi(28), footerCenterY, 4, kColDotGreen);

            SelectObject(hdcMem, g_hFontKeycap);
            SetTextColor(hdcMem, g_cfg.colTextMuted);
            RECT rcFooterLeft = { footerInset + ScaleDpi(44), footerTop + 1, width / 2 + ScaleDpi(60), height };
            DrawTextW(hdcMem, L"\x2191\x2193 Navigate    \x21B5 Execute    Esc Dismiss", -1, &rcFooterLeft, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

            RECT rcFooterRight = { width / 2 + ScaleDpi(40), footerTop + 1, width - footerInset, height };
            DrawTextW(hdcMem, L"Liquid Quick Launcher  \x2022  RobsHs", -1, &rcFooterRight, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
        }
    }

    DrawOuterWindowBorderGdi(hdcMem, width, height);

    SelectObject(hdcMem, hOldFont);

    BitBlt(hdcScreen, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

    SelectObject(hdcMem, hOldBmp);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
}

// ============================================================================
// Edit Control Subclass Procedure
// ============================================================================
static LRESULT CALLBACK SearchEditSubclassProc(HWND hWnd,
                                               UINT uMsg,
                                               WPARAM wParam,
                                               LPARAM lParam,
                                               UINT_PTR uIdSubclass,
                                               DWORD_PTR dwRefData) {
    (void)uIdSubclass;
    (void)dwRefData;

    if (uMsg == WM_KEYDOWN) {
        if (wParam == VK_ESCAPE) {
            HideLauncherWindow();
            return 0;
        }
        if (wParam == VK_DOWN) {
            int count = (int)g_results.size();
            if (count > 0) {
                int nextIdx = (g_selectedIndex + 1) % count;
                SetSelectedIndexSmooth(nextIdx, false);
                SetTimer(g_hMainWnd, kAnimTimerId, kAnimFrameIntervalMs, NULL);
                InvalidateRect(g_hMainWnd, NULL, FALSE);
            }
            return 0;
        }
        if (wParam == VK_UP) {
            int count = (int)g_results.size();
            if (count > 0) {
                int prevIdx = (g_selectedIndex - 1 + count) % count;
                SetSelectedIndexSmooth(prevIdx, false);
                SetTimer(g_hMainWnd, kAnimTimerId, kAnimFrameIntervalMs, NULL);
                InvalidateRect(g_hMainWnd, NULL, FALSE);
            }
            return 0;
        }
        if (wParam == VK_RETURN) {
            ExecuteSelectedResult();
            return 0;
        }
    } else if (uMsg == WM_CHAR) {
        if (wParam == VK_RETURN || wParam == VK_ESCAPE) {
            return 0;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ============================================================================
// Main Launcher Window Procedure
// ============================================================================
static LRESULT CALLBACK LauncherWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            ApplyLiquidGlassDwmAttributes(hwnd);

            g_hEditWnd = CreateWindowExW(
                0,
                L"EDIT",
                L"",
                WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
                0, 0, 0, 0,
                hwnd,
                (HMENU)1001,
                g_hModInstance,
                NULL
            );

            if (g_hEditWnd) {
                SendMessageW(g_hEditWnd, WM_SETFONT, (WPARAM)g_hFontSearch, TRUE);
                SendMessageW(g_hEditWnd, EM_SETCUEBANNER, TRUE, (LPARAM)L"Search apps, calculate, > command, or web...");
                SetWindowSubclass(g_hEditWnd, SearchEditSubclassProc, 1, 0);
            }
            RefreshEditControlColorsAndLayout();
            return 0;
        }

        case WM_TIMER: {
            if (wParam == kAnimTimerId) {
                StepLauncherAnimationFrame(hwnd);
                return 0;
            }
            break;
        }

        case WM_CTLCOLOREDIT: {
            HDC hdcEdit = (HDC)wParam;
            SetTextColor(hdcEdit, g_cfg.colTextPrimary);
            SetBkColor(hdcEdit, g_cfg.colHeaderSurface);
            return (LRESULT)g_hEditBgBrush;
        }

        case WM_COMMAND: {
            if (LOWORD(wParam) == 1001 && HIWORD(wParam) == EN_CHANGE) {
                wchar_t buffer[512];
                GetWindowTextW(g_hEditWnd, buffer, 512);
                RebuildResultsForQuery(buffer);
                RequestSmoothHeightUpdate();
                return 0;
            }
            break;
        }

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            PaintLauncherWindow(hwnd, hdc);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_MOUSEMOVE: {
            int mouseY = (short)HIWORD(lParam);
            int listStartY = ComputeListStartY();
            int itemH = ScaleDpi(kBaseItemHeight);
            int listEndY = listStartY + ((int)g_results.size() * itemH);
            int newHover = -1;
            if (mouseY >= listStartY && mouseY < listEndY) {
                int idx = (mouseY - listStartY) / itemH;
                if (idx >= 0 && idx < (int)g_results.size()) {
                    newHover = idx;
                }
            }
            if (newHover != g_hoveredIndex) {
                g_hoveredIndex = newHover;
                SetTimer(hwnd, kAnimTimerId, kAnimFrameIntervalMs, NULL);
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }

        case WM_LBUTTONDOWN: {
            int mouseY = (short)HIWORD(lParam);
            int listStartY = ComputeListStartY();
            int itemH = ScaleDpi(kBaseItemHeight);
            int listEndY = listStartY + ((int)g_results.size() * itemH);
            if (mouseY >= listStartY && mouseY < listEndY) {
                int idx = (mouseY - listStartY) / itemH;
                if (idx >= 0 && idx < (int)g_results.size()) {
                    SetSelectedIndexSmooth(idx, true);
                    ExecuteSelectedResult();
                }
            } else if (g_hEditWnd) {
                SetFocus(g_hEditWnd);
            }
            return 0;
        }

        case WM_ACTIVATE: {
            if (LOWORD(wParam) == WA_INACTIVE && g_bWindowVisible) {
                HideLauncherWindow();
            }
            return 0;
        }

        case WM_HOTKEY: {
            if ((int)wParam == kHotKeyId) {
                if (g_bWindowVisible && GetForegroundWindow() == hwnd) {
                    HideLauncherWindow();
                } else {
                    ShowLauncherWindow();
                }
                return 0;
            }
            break;
        }

        case WM_APP_INDEX_DONE: {
            if (g_bWindowVisible && g_hEditWnd) {
                wchar_t buffer[512];
                GetWindowTextW(g_hEditWnd, buffer, 512);
                if (buffer[0] != L'\0') {
                    RebuildResultsForQuery(buffer);
                    RequestSmoothHeightUpdate();
                }
            }
            return 0;
        }

        case WM_APP_RELOAD_SETTINGS: {
            LoadUserVisualSettings();
            ApplyLiquidGlassDwmAttributes(hwnd);
            RefreshEditControlColorsAndLayout();
            g_animWindowAlpha = (float)g_cfg.targetWindowAlpha;
            SetLayeredWindowAttributes(hwnd, 0, g_cfg.targetWindowAlpha, LWA_ALPHA);
            if (g_bWindowVisible) {
                int targetH = ComputeTargetWindowHeight();
                int winW = ScaleDpi(kBaseWindowWidth);
                g_animCurHeight = (float)targetH;
                g_animTargetHeight = (float)targetH;
                UpdateOuterMainWindowShapeRegion(hwnd, winW, targetH);
                SetWindowPos(hwnd, HWND_TOPMOST, g_anchorPosX, g_anchorPosY,
                             winW, targetH,
                             SWP_NOACTIVATE | SWP_NOZORDER);
                InvalidateRect(hwnd, NULL, TRUE);
            }
            return 0;
        }

        default:
            break;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

// ============================================================================
// Dedicated UI & Hotkey Thread (Tool Mod Process)
// ============================================================================
static void CreateLauncherGdiResources() {
    DestroyLauncherGdiResources();

    g_hFontSearch = CreateFontW(-ScaleDpi(21), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    g_hFontTitle  = CreateFontW(-ScaleDpi(16), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    g_hFontSub    = CreateFontW(-ScaleDpi(13), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    g_hFontBadge  = CreateFontW(-ScaleDpi(12), 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    g_hFontKeycap = CreateFontW(-ScaleDpi(12), 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    g_hFontIcon   = CreateFontW(-ScaleDpi(19), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI Symbol");

    g_hEditBgBrush = CreateSolidBrush(g_cfg.colHeaderSurface);

    if (g_hEditWnd && IsWindow(g_hEditWnd)) {
        SendMessageW(g_hEditWnd, WM_SETFONT, (WPARAM)g_hFontSearch, TRUE);
    }
}

static void DestroyLauncherGdiResources() {
    if (g_hFontSearch)  { DeleteObject(g_hFontSearch);  g_hFontSearch = NULL; }
    if (g_hFontTitle)   { DeleteObject(g_hFontTitle);   g_hFontTitle = NULL; }
    if (g_hFontSub)     { DeleteObject(g_hFontSub);     g_hFontSub = NULL; }
    if (g_hFontBadge)   { DeleteObject(g_hFontBadge);   g_hFontBadge = NULL; }
    if (g_hFontKeycap)  { DeleteObject(g_hFontKeycap);  g_hFontKeycap = NULL; }
    if (g_hFontIcon)    { DeleteObject(g_hFontIcon);    g_hFontIcon = NULL; }
    if (g_hEditBgBrush) { DeleteObject(g_hEditBgBrush); g_hEditBgBrush = NULL; }
}

static DWORD WINAPI LauncherUiThreadProc(LPVOID lpParam) {
    (void)lpParam;
    HRESULT hrCom = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    LoadUserVisualSettings();
    LoadFrecencyHistory();
    CreateLauncherGdiResources();

    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&LauncherWndProc, &g_hModInstance);

    WNDCLASSEXW wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize        = sizeof(WNDCLASSEXW);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = LauncherWndProc;
    wc.hInstance     = g_hModInstance;
    wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszClassName = kWindowClassName;

    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassExW failed for launcher window.");
        if (SUCCEEDED(hrCom)) CoUninitialize();
        return 1;
    }

    int winW = ScaleDpi(kBaseWindowWidth);
    int baseH = ScaleDpi(kBaseWindowHeight);

    g_hMainWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        kWindowClassName,
        L"Liquid Quick Launcher",
        WS_POPUP | WS_CLIPCHILDREN,
        0, 0, winW, baseH,
        NULL, NULL, g_hModInstance, NULL
    );

    if (g_hMainWnd) {
        SetLayeredWindowAttributes(g_hMainWnd, 0, g_cfg.targetWindowAlpha, LWA_ALPHA);

        UINT fsMod = MOD_ALT | MOD_NOREPEAT;
        UINT vk = VK_SPACE;
        if (g_cfg.hotkeyChoice == HOTKEY_CTRL_SPACE) {
            fsMod = MOD_CONTROL | MOD_NOREPEAT;
            vk = VK_SPACE;
        } else if (g_cfg.hotkeyChoice == HOTKEY_WIN_ALT_SPACE) {
            fsMod = MOD_WIN | MOD_ALT | MOD_NOREPEAT;
            vk = VK_SPACE;
        } else if (g_cfg.hotkeyChoice == HOTKEY_ALT_Q) {
            fsMod = MOD_ALT | MOD_NOREPEAT;
            vk = 'Q';
        }

        if (!RegisterHotKey(g_hMainWnd, kHotKeyId, fsMod, vk)) {
            Wh_Log(L"RegisterHotKey failed. Ensure shortcut is not occupied by another app.");
        }
    }

    g_hIndexThread = CreateThread(NULL, 0, AppIndexerThreadProc, NULL, 0, NULL);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // Clean teardown after message loop
    InterlockedExchange(&g_bStopRequested, 1);
    if (g_hMainWnd) {
        KillTimer(g_hMainWnd, kAnimTimerId);
        UnregisterHotKey(g_hMainWnd, kHotKeyId);
        if (g_hEditWnd) {
            RemoveWindowSubclass(g_hEditWnd, SearchEditSubclassProc, 1);
            DestroyWindow(g_hEditWnd);
            g_hEditWnd = NULL;
        }
        DestroyWindow(g_hMainWnd);
        g_hMainWnd = NULL;
    }

    if (g_hIndexThread) {
        WaitForSingleObject(g_hIndexThread, INFINITE);
        CloseHandle(g_hIndexThread);
        g_hIndexThread = NULL;
    }

    UnregisterClassW(kWindowClassName, g_hModInstance);
    DestroyLauncherGdiResources();

    if (SUCCEEDED(hrCom)) {
        CoUninitialize();
    }
    return 0;
}

// ============================================================================
// Windhawk Tool Mod Lifecycle Callbacks
// ============================================================================
BOOL WhTool_ModInit() {
    Wh_Log(L"Initializing Liquid Quick Launcher as tool mod...");

    InterlockedExchange(&g_bStopRequested, 0);
    InitializeCriticalSection(&g_csApps);
    g_bCsInitialized = true;

    g_hUiThread = CreateThread(NULL, 0, LauncherUiThreadProc, NULL, 0, &g_dwUiThreadId);
    if (!g_hUiThread) {
        Wh_Log(L"Failed to create UI thread for Liquid Quick Launcher.");
        DeleteCriticalSection(&g_csApps);
        g_bCsInitialized = false;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Reloading Liquid Quick Launcher settings live...");
    if (g_hMainWnd && IsWindow(g_hMainWnd)) {
        PostMessageW(g_hMainWnd, WM_APP_RELOAD_SETTINGS, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninitializing Liquid Quick Launcher tool mod...");

    InterlockedExchange(&g_bStopRequested, 1);

    while (!PostThreadMessageW(g_dwUiThreadId, WM_QUIT, 0, 0) &&
           WaitForSingleObject(g_hUiThread, 10) == WAIT_TIMEOUT) {
    }

    if (g_hUiThread) {
        WaitForSingleObject(g_hUiThread, INFINITE);
        CloseHandle(g_hUiThread);
        g_hUiThread = NULL;
        g_dwUiThreadId = 0;
    }

    if (g_bCsInitialized) {
        EnterCriticalSection(&g_csApps);
        DestroyAppIconsInList(g_indexedApps);
        g_indexedApps.clear();
        LeaveCriticalSection(&g_csApps);
        DeleteCriticalSection(&g_csApps);
        g_bCsInitialized = false;
    }
}

// ============================================================================
// Official Tool Mod Boilerplate (Runs mod in dedicated windhawk.exe process)
// ============================================================================
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}

// ==WindhawkMod==
// @id              vision
// @name            Vision
// @description     Per-window opacity, Z-order pinning, and brightness-aware dimming via title-bar mouse gestures and optional header control
// @author          Ashix
// @version         1.2026.1711
// @github          https://github.com/k-ashix
// @twitter         https://x.com/k_ashix
// @donateUrl       https://ko-fi.com/kashix
// @include         windhawk-mod.exe
// @include         windhawk.exe
// @compilerOptions -lgdi32 -lgdiplus -ldwmapi -lwtsapi32 -lpsapi -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Vison

Control any window's **opacity**, **always-on-top**, and **opacity & dimming filter** directly from its title bar.

| Action | Default Trigger | Notes |
|---|---|---|
| Adjust Opacity | `Alt + Shift + Wheel` over title bar | 25%–100%, 5% step |
| Toggle Pin (`TOPMOST`) | `Alt + Shift + Middle-Click` over title bar | Pins/unpins window |
| Adjust Dim | `Ctrl + Wheel` over title bar | Soft overlay, never 100% pitch black |
| Toggle Saved App Dim | `Ctrl + Double-Click` over title bar | Toggles ON/OFF without erasing per-app preset |
| Header `%` Button & Slider | Optional (disabled by default) | Borderless circle `%` button; hover reveals slider |

### Highlights
- **Flicker-Free Capture Guard & Auto-Restore:** Detects Snipping Tool (`ScreenClippingHost.exe`, `SnippingTool.exe`), Screen Sketch, Xbox Game Bar, ShareX, and Lightshot sessions; holds dim & opacity states cleanly during capture without reset, flash, or Z-order jumping, and restores them automatically when capture ends.
- **Unified Screenshot & Opacity Exclusion Control:** Supports `exclude_all`, `show_dim`, `show_controls`, and `show_all`, plus automatic window-opacity restoration during excluded captures so background windows never bleed through screenshots.
- **Zero-Strip Atomic Resize Tracking:** Uses `CS_HREDRAW | CS_VREDRAW` brush-backed overlays without `SWP_NOCOPYBITS` so dragging a dimmed window's edge never flashes a black or white strip.
- **Text Clarity & Crispness Block:** Optional `Text-Clarity` settings keep text crisp and readable when dimmed or translucent via hover/focus clarity boost, contrast-preserving dark backing, and smart alpha floor clamping.
- **Hardware Brightness SSOT (Never Fights Monitor OSD / BenQ Eye-CareU):** External hardware/software brightness is always the Single Source of Truth (e.g., if your BenQ monitor is at 30%, that 30% is Vison's 100% baseline). Vison never polls or touches DDC/CI, WMI, or gamma ramps.
- **Dual Compatible:** Runs as a dedicated Windhawk Tool Mod (`windhawk-mod.exe` & `windhawk.exe`) compatible with both Windhawk 1.7.3 and 2.0.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Triggers:
  - opacityModifier: alt_shift
    $name: Opacity & Pin Modifier Key
    $description: Key(s) held while scrolling or clicking a title bar to change opacity or pin
    $options:
      - alt_shift: Alt + Shift
      - ctrl_shift: Ctrl + Shift
      - ctrl_alt: Ctrl + Alt
      - win_shift: Win + Shift
      - alt: Alt
      - ctrl: Ctrl
      - shift: Shift
  - pinButton: middle
    $name: Pin Window Mouse Button
    $description: Mouse button pressed with the Opacity Modifier on a title bar to toggle Always-on-Top
    $options:
      - middle: Middle Click
      - right: Right Click
      - x1: Mouse Back (XButton1)
      - x2: Mouse Forward (XButton2)
      - disabled: Disabled
  - dimModifier: ctrl
    $name: Dim Modifier Key
    $description: Key(s) held while scrolling a title bar to adjust window dimming
    $options:
      - ctrl: Ctrl
      - alt: Alt
      - shift: Shift
      - ctrl_shift: Ctrl + Shift
      - alt_shift: Alt + Shift
      - ctrl_alt: Ctrl + Alt
  - dimToggleAction: double_left
    $name: Quick Toggle Saved App Dim
    $description: Mouse action with the Dim Modifier on a title bar to toggle saved per-app dim ON/OFF
    $options:
      - double_left: Double Left-Click
      - middle: Middle Click
      - disabled: Disabled
  - opacityStep: 5
    $name: Opacity Scroll Step (%)
    $description: Percentage change per mouse wheel notch (1-25)
  - opacityMin: 25
    $name: Minimum Opacity Floor (%)
    $description: Lowest allowed window opacity (10-95)
  - dimStep: 5
    $name: Dim Scroll Step (%)
    $description: Percentage change per mouse wheel notch (1-25)
  - dimMaxAlphaPercent: 90
    $name: Maximum Dim Darkness Cap (%)
    $description: Hard cap (10-95%) so dimmed windows always remain readable
- Dimming:
  - mode: relative
    $name: Dimming Response Curve (Hardware Brightness = 100% Baseline)
    $description: Current monitor hardware/OSD brightness is always treated as 100% SSOT; Vison never alters or fights hardware brightness
    $options:
      - relative: Linear Relative to Hardware Brightness (100% = current monitor)
      - auto_scaled: Perceptual Eye-Care Curve (Gentle roll-off on low-brightness panels)
  - colorPreset: soft_charcoal
    $name: Dim Filter Tint
    $description: Predefined soft dim tints or choose Custom Hex Color below
    $options:
      - soft_charcoal: Soft Charcoal (#141418)
      - warm_slate: Warm Night Slate (#1A1614)
      - cool_navy: Cool Midnight Navy (#0E141E)
      - neutral_gray: Neutral Dim Gray (#242428)
      - oled_black: Deep OLED Black (#000000)
      - custom: Custom Hex Color
  - customHexColor: "#141418"
    $name: Custom Dim Hex Color (#RRGGBB)
    $description: Applied only when Dim Filter Tint is set to Custom Hex Color
  - persistWhenUnfocused: true
    $name: Keep Dim Active When Out of Focus
    $description: Keeps a dimmed window dimmed even when you switch to another app; turn off to dim only the focused window
  - captureBehavior: exclude_all
    $name: Screenshot & Screen Recording Visibility
    $description: Controls whether dim overlays, window opacity, and header controls are excluded or included in screenshots and recordings
    $options:
      - exclude_all: Exclude Both Dim & Decreased Opacity in Screenshots (Default)
      - show_dim: Include Dim & Opacity in Screenshot (Hide Header Controls)
      - show_controls: Show Header Controls Only (Exclude Dim & Opacity)
      - show_all: Include Both Dim, Opacity & Controls in Screenshots
  - restoreOpacityOnCapture: true
    $name: Temporarily Restore 100% Window Opacity During Excluded Captures
    $description: Prevents background text/windows from bleeding through translucent windows when taking a screenshot in Exclude mode, then restores opacity automatically
- Text-Clarity:
  - keepTextCrisp: true
    $name: Keep Text Crisp When Dimmed or Translucent
    $description: Preserves sharp text contrast and readability instead of washing out glyphs when dimming or lowering window opacity
  - crispMode: contrast_anchor
    $name: Crisp Text Preservation Mode
    $description: How Vison keeps text crisp while dimming or lowering opacity
    $options:
      - contrast_anchor: Contrast-Anchored Backing (Prevents Background Bleed & Washed-Out Text)
      - hover_focus_crisp: Crisp on Hover / Active Focus (Restores Full Text Clarity While Reading)
      - smart_floor: Smart Readability Clamp (Caps Dim & Opacity at High-Contrast Floor)
  - crispStrength: 65
    $name: Text Crispness & Contrast Anchor Strength (%)
    $description: Strength of dark contrast backing and clarity preservation (20-95%) so text stays crisp
- Header-Control:
  - enabled: false
    $name: Show Header % Control
    $description: Displays a minimal % indicator in the title bar when space is free; hovering reveals a dim slider
  - position: after_title
    $name: Header % Control Placement
    $description: After App Title avoids all right-side window buttons; Before Caption Buttons places it on the right
    $options:
      - after_title: After App Title (Left Side, Default)
      - before_caption: Before Caption Buttons (Right Side)
  - showFill: false
    $name: Show Background Fill Under % Text
    $description: Keep off for bare adaptive % text; turn on for an OLED Black circle fill behind the percentage
- Reset:
  - onWindowClose: true
    $name: Reset When Window or App Closes
    $description: Clean up overlay and active state when a window closes or an app exits (saved per-app presets remain)
  - onSessionChange: true
    $name: Reset on Lock / Logoff / Unlock
    $description: Restore all windows to normal when your Windows session locks, logs off, or unlocks
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <shellapi.h>
#include <wtsapi32.h>
#include <psapi.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstring>
#include <cwctype>
#include <map>
#include <string>
#include <vector>

#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif

#ifndef DWMWA_EXTENDED_FRAME_BOUNDS
#define DWMWA_EXTENDED_FRAME_BOUNDS 9
#endif

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

#ifndef DWMWA_CLOAKED
#define DWMWA_CLOAKED 14
#endif

#ifndef EVENT_OBJECT_CLOAKED
#define EVENT_OBJECT_CLOAKED 0x8017
#endif

#ifndef EVENT_OBJECT_UNCLOAKED
#define EVENT_OBJECT_UNCLOAKED 0x8018
#endif

namespace {

constexpr UINT WM_APP_RELOAD_SETTINGS = WM_APP + 101;
constexpr UINT WM_APP_HOOK_WHEEL      = WM_APP + 102;
constexpr UINT WM_APP_HOOK_BUTTON     = WM_APP + 103;
constexpr UINT WM_APP_HOOK_DBLCLICK   = WM_APP + 104;
constexpr UINT_PTR TIMER_ANIMATION    = 2;
constexpr UINT_PTR TIMER_HOVER_CHECK  = 3;
constexpr UINT_PTR TIMER_SAVE_PRESETS = 4;

constexpr wchar_t kCtrlClass[]        = L"Vison_ControlWnd";
constexpr wchar_t kDimClass[]         = L"Vison_DimOverlayWnd";
constexpr wchar_t kBadgeClass[]       = L"Vison_HeaderBadgeWnd";
constexpr wchar_t kSliderClass[]      = L"Vison_HeaderSliderWnd";

enum class ModCombo {
    AltShift,
    CtrlShift,
    CtrlAlt,
    WinShift,
    Alt,
    Ctrl,
    Shift
};

enum class ButtonAction {
    Disabled,
    Middle,
    Right,
    X1,
    X2,
    DoubleLeft
};

enum class DimMode {
    Relative,
    AutoScaled
};

enum class CaptureMode {
    ExcludeAll,
    ShowDimOnly,
    ShowControlsOnly,
    ShowAll
};

enum class CrispTextMode {
    ContrastAnchor,
    HoverFocusCrisp,
    SmartFloor
};

struct Settings {
    ModCombo opacityMod = ModCombo::AltShift;
    ButtonAction pinButton = ButtonAction::Middle;
    ModCombo dimMod = ModCombo::Ctrl;
    ButtonAction dimToggle = ButtonAction::DoubleLeft;
    int opacityStep = 5;
    int opacityMin = 25;
    int dimStep = 5;
    int dimMaxAlphaPercent = 90;
    DimMode dimMode = DimMode::Relative;
    CaptureMode captureMode = CaptureMode::ExcludeAll;
    bool restoreOpacityOnCapture = true;
    bool keepTextCrisp = true;
    CrispTextMode crispMode = CrispTextMode::ContrastAnchor;
    int crispStrength = 65;
    COLORREF dimColor = RGB(0x14, 0x14, 0x18);
    bool persistWhenUnfocused = true;
    bool headerControlEnabled = false;
    bool headerAfterTitle = true;
    bool headerShowFill = false;
    bool resetOnWindowClose = true;
    bool resetOnSessionChange = true;
};

struct WindowState {
    HWND target = nullptr;
    std::wstring exeName;
    bool addedLayered = false;
    int opacityPct = 100;
    BYTE currentAlpha = 255;
    bool wasTopmost = false;
    bool pinnedByMod = false;

    bool dimActive = false;
    int dimBrightness = 100;
    HWND overlayHwnd = nullptr;
    RECT lastOverlayRect{};
    DWORD lastCornerPref = 0;
    bool isMoveSizing = false;
    bool captureSuppressedOpacity = false;
};

struct AnimState {
    bool active = false;
    float current = 0.0f;
    float start = 0.0f;
    float target = 0.0f;
    LARGE_INTEGER startTick{};
    float durationMs = 165.0f;
};

struct CachedDIB {
    HDC memDC = nullptr;
    HBITMAP bmp = nullptr;
    HGDIOBJ oldBmp = nullptr;
    int width = 0;
    int height = 0;

    void Release() {
        if (memDC && oldBmp) {
            SelectObject(memDC, oldBmp);
            oldBmp = nullptr;
        }
        if (bmp) {
            DeleteObject(bmp);
            bmp = nullptr;
        }
        if (memDC) {
            DeleteDC(memDC);
            memDC = nullptr;
        }
        width = 0;
        height = 0;
    }

    bool Ensure(HDC screenDC, int w, int h) {
        if (memDC && bmp && width == w && height == h) return true;
        Release();
        memDC = CreateCompatibleDC(screenDC);
        if (!memDC) return false;

        BITMAPINFO bmi{};
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = w;
        bmi.bmiHeader.biHeight = -h;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        void* bits = nullptr;
        bmp = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!bmp) {
            Release();
            return false;
        }
        oldBmp = SelectObject(memDC, bmp);
        width = w;
        height = h;
        return true;
    }
};

// State owned exclusively by the worker UI thread unless marked atomic.
Settings g_settings;
std::atomic<int> g_hookOpacityMod{static_cast<int>(ModCombo::AltShift)};
std::atomic<int> g_hookPinButton{static_cast<int>(ButtonAction::Middle)};
std::atomic<int> g_hookDimMod{static_cast<int>(ModCombo::Ctrl)};
std::atomic<int> g_hookDimToggle{static_cast<int>(ButtonAction::DoubleLeft)};
HBRUSH g_dimBrush = nullptr;
HANDLE g_workerThread = nullptr;
DWORD g_workerThreadId = 0;
HANDLE g_readyEvent = nullptr;
HHOOK g_mouseHook = nullptr;
HWINEVENTHOOK g_winEventHook = nullptr;
ULONG_PTR g_gdiplusToken = 0;

HWND g_ctrlWnd = nullptr;
HWND g_badgeWnd = nullptr;
HWND g_sliderWnd = nullptr;
HWND g_headerTarget = nullptr;
HWND g_lastNormalForeground = nullptr;
HWND g_cachedHeaderHwnd = nullptr;
bool g_captureSessionActive = false;
int g_cachedCaptionReserve = 0;
int g_cachedTitlePx = 0;
int g_sliderBaseX = 0;
int g_sliderBaseY = 0;
bool g_sliderDragging = false;
bool g_headerIsLight = false;
bool g_badgeHovered = false;
bool g_sliderHovered = false;
AnimState g_sliderAnim;
LARGE_INTEGER g_qpcFreq{};
CachedDIB g_badgeDib;
CachedDIB g_sliderDib;

std::map<HWND, WindowState> g_ledger;
std::map<std::wstring, int> g_appPresets;
std::map<std::wstring, int> g_appOpacityPresets;
std::atomic<bool> g_unloading{false};

HWND g_lastWheelTarget = nullptr;
float g_wheelAccumOpacity = 0.0f;
float g_wheelAccumDim = 0.0f;
float g_wheelAccumSlider = 0.0f;

ULONGLONG g_lastCtrlDownTick = 0;
POINT g_lastCtrlDownPt{};
bool g_swallowNextLeftUp = false;

// -----------------------------------------------------------------------------
// Settings & Parsing
// -----------------------------------------------------------------------------

ModCombo ParseModCombo(PCWSTR s, ModCombo fallback) {
    if (!s) return fallback;
    if (wcscmp(s, L"alt_shift") == 0) return ModCombo::AltShift;
    if (wcscmp(s, L"ctrl_shift") == 0) return ModCombo::CtrlShift;
    if (wcscmp(s, L"ctrl_alt") == 0) return ModCombo::CtrlAlt;
    if (wcscmp(s, L"win_shift") == 0) return ModCombo::WinShift;
    if (wcscmp(s, L"alt") == 0) return ModCombo::Alt;
    if (wcscmp(s, L"ctrl") == 0) return ModCombo::Ctrl;
    if (wcscmp(s, L"shift") == 0) return ModCombo::Shift;
    return fallback;
}

ButtonAction ParseButtonAction(PCWSTR s, ButtonAction fallback) {
    if (!s) return fallback;
    if (wcscmp(s, L"middle") == 0) return ButtonAction::Middle;
    if (wcscmp(s, L"right") == 0) return ButtonAction::Right;
    if (wcscmp(s, L"x1") == 0) return ButtonAction::X1;
    if (wcscmp(s, L"x2") == 0) return ButtonAction::X2;
    if (wcscmp(s, L"double_left") == 0) return ButtonAction::DoubleLeft;
    if (wcscmp(s, L"disabled") == 0) return ButtonAction::Disabled;
    return fallback;
}

COLORREF ParseHexColor(PCWSTR hex, COLORREF fallback) {
    if (!hex) return fallback;
    if (*hex == L'#') ++hex;
    if (wcslen(hex) != 6) return fallback;
    wchar_t* end = nullptr;
    unsigned long val = wcstoul(hex, &end, 16);
    if (!end || *end != L'\0') return fallback;
    return RGB(static_cast<BYTE>((val >> 16) & 0xFF),
               static_cast<BYTE>((val >> 8) & 0xFF),
               static_cast<BYTE>(val & 0xFF));
}

void RefreshDimBrush(COLORREF color) {
    if (g_dimBrush) {
        DeleteObject(g_dimBrush);
        g_dimBrush = nullptr;
    }
    g_dimBrush = CreateSolidBrush(color);
}

void LoadSettings() {
    Settings s;
    PCWSTR str = Wh_GetStringSetting(L"triggers.opacityModifier");
    s.opacityMod = ParseModCombo(str, ModCombo::AltShift);
    Wh_FreeStringSetting(str);

    str = Wh_GetStringSetting(L"triggers.pinButton");
    s.pinButton = ParseButtonAction(str, ButtonAction::Middle);
    Wh_FreeStringSetting(str);

    str = Wh_GetStringSetting(L"triggers.dimModifier");
    s.dimMod = ParseModCombo(str, ModCombo::Ctrl);
    Wh_FreeStringSetting(str);

    str = Wh_GetStringSetting(L"triggers.dimToggleAction");
    s.dimToggle = ParseButtonAction(str, ButtonAction::DoubleLeft);
    Wh_FreeStringSetting(str);

    s.opacityStep = std::clamp(Wh_GetIntSetting(L"triggers.opacityStep"), 1, 25);
    s.opacityMin = std::clamp(Wh_GetIntSetting(L"triggers.opacityMin"), 10, 95);
    s.dimStep = std::clamp(Wh_GetIntSetting(L"triggers.dimStep"), 1, 25);
    s.dimMaxAlphaPercent = std::clamp(Wh_GetIntSetting(L"triggers.dimMaxAlphaPercent"), 10, 95);

    str = Wh_GetStringSetting(L"dimming.mode");
    s.dimMode = (str && wcscmp(str, L"auto_scaled") == 0) ? DimMode::AutoScaled : DimMode::Relative;
    Wh_FreeStringSetting(str);

    str = Wh_GetStringSetting(L"dimming.colorPreset");
    if (str && wcscmp(str, L"warm_slate") == 0) {
        s.dimColor = RGB(0x1A, 0x16, 0x14);
    } else if (str && wcscmp(str, L"cool_navy") == 0) {
        s.dimColor = RGB(0x0E, 0x14, 0x1E);
    } else if (str && wcscmp(str, L"neutral_gray") == 0) {
        s.dimColor = RGB(0x24, 0x24, 0x28);
    } else if (str && wcscmp(str, L"oled_black") == 0) {
        s.dimColor = RGB(0x00, 0x00, 0x00);
    } else if (str && wcscmp(str, L"custom") == 0) {
        PCWSTR customHex = Wh_GetStringSetting(L"dimming.customHexColor");
        s.dimColor = ParseHexColor(customHex, RGB(0x14, 0x14, 0x18));
        Wh_FreeStringSetting(customHex);
    } else {
        s.dimColor = RGB(0x14, 0x14, 0x18);
    }
    Wh_FreeStringSetting(str);

    s.persistWhenUnfocused = Wh_GetIntSetting(L"dimming.persistWhenUnfocused") != 0;

    str = Wh_GetStringSetting(L"dimming.captureBehavior");
    if (str && wcscmp(str, L"show_dim") == 0) {
        s.captureMode = CaptureMode::ShowDimOnly;
    } else if (str && (wcscmp(str, L"show_controls") == 0 || wcscmp(str, L"ignore") == 0)) {
        s.captureMode = CaptureMode::ShowControlsOnly;
    } else if (str && wcscmp(str, L"show_all") == 0) {
        s.captureMode = CaptureMode::ShowAll;
    } else {
        s.captureMode = CaptureMode::ExcludeAll;
    }
    Wh_FreeStringSetting(str);

    s.restoreOpacityOnCapture = Wh_GetIntSetting(L"dimming.restoreOpacityOnCapture") != 0;

    s.keepTextCrisp = Wh_GetIntSetting(L"textClarity.keepTextCrisp") != 0;
    str = Wh_GetStringSetting(L"textClarity.crispMode");
    if (str && wcscmp(str, L"hover_focus_crisp") == 0) {
        s.crispMode = CrispTextMode::HoverFocusCrisp;
    } else if (str && wcscmp(str, L"smart_floor") == 0) {
        s.crispMode = CrispTextMode::SmartFloor;
    } else {
        s.crispMode = CrispTextMode::ContrastAnchor;
    }
    Wh_FreeStringSetting(str);
    s.crispStrength = std::clamp(Wh_GetIntSetting(L"textClarity.crispStrength"), 20, 95);

    s.headerControlEnabled = Wh_GetIntSetting(L"headerControl.enabled") != 0;
    str = Wh_GetStringSetting(L"headerControl.position");
    s.headerAfterTitle = !(str && wcscmp(str, L"before_caption") == 0);
    Wh_FreeStringSetting(str);
    s.headerShowFill = Wh_GetIntSetting(L"headerControl.showFill") != 0;
    s.resetOnWindowClose = Wh_GetIntSetting(L"reset.onWindowClose") != 0;
    s.resetOnSessionChange = Wh_GetIntSetting(L"reset.onSessionChange") != 0;

    g_settings = s;
    g_hookOpacityMod.store(static_cast<int>(s.opacityMod), std::memory_order_relaxed);
    g_hookPinButton.store(static_cast<int>(s.pinButton), std::memory_order_relaxed);
    g_hookDimMod.store(static_cast<int>(s.dimMod), std::memory_order_relaxed);
    g_hookDimToggle.store(static_cast<int>(s.dimToggle), std::memory_order_relaxed);
    RefreshDimBrush(g_settings.dimColor);
}

// -----------------------------------------------------------------------------
// Per-App Preset Persistence (Windhawk Local Storage)
// -----------------------------------------------------------------------------

void ParsePresetMap(PCWSTR key, std::map<std::wstring, int>& outMap, int minVal, int maxVal) {
    outMap.clear();
    std::vector<wchar_t> buf(32768, L'\0');
    size_t len = Wh_GetStringValue(key, buf.data(), buf.size());
    if (len == 0) return;

    std::wstring raw(buf.data());
    size_t pos = 0;
    while (pos < raw.size()) {
        size_t semi = raw.find(L';', pos);
        if (semi == std::wstring::npos) semi = raw.size();
        std::wstring entry = raw.substr(pos, semi - pos);
        size_t eq = entry.find(L'=');
        if (eq != std::wstring::npos && eq > 0) {
            std::wstring exe = entry.substr(0, eq);
            int val = _wtoi(entry.substr(eq + 1).c_str());
            outMap[exe] = std::clamp(val, minVal, maxVal);
        }
        pos = semi + 1;
    }
}

void SerializePresetMap(PCWSTR key, const std::map<std::wstring, int>& inMap) {
    std::wstring out;
    for (const auto& [exe, val] : inMap) {
        if (!out.empty()) out += L';';
        out += exe + L'=' + std::to_wstring(val);
    }
    Wh_SetStringValue(key, out.c_str());
}

void LoadPresetsFromStorage() {
    ParsePresetMap(L"perAppDimPresets", g_appPresets, 0, 100);
    ParsePresetMap(L"perAppOpacityPresets", g_appOpacityPresets, 10, 100);
}

void SavePresetsToStorage() {
    SerializePresetMap(L"perAppDimPresets", g_appPresets);
    SerializePresetMap(L"perAppOpacityPresets", g_appOpacityPresets);
}

// -----------------------------------------------------------------------------
// Window & Safety Utilities
// -----------------------------------------------------------------------------

bool IsOwnOverlayWindow(HWND hwnd) {
    if (!hwnd) return false;
    if (hwnd == g_ctrlWnd || hwnd == g_badgeWnd || hwnd == g_sliderWnd) return true;
    for (const auto& [_, st] : g_ledger) {
        if (st.overlayHwnd == hwnd) return true;
    }
    return false;
}

bool IsModifierComboDown(ModCombo combo) {
    bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    bool win = ((GetAsyncKeyState(VK_LWIN) & 0x8000) != 0) ||
               ((GetAsyncKeyState(VK_RWIN) & 0x8000) != 0);

    switch (combo) {
        case ModCombo::AltShift:  return alt && shift && !ctrl && !win;
        case ModCombo::CtrlShift: return ctrl && shift && !alt && !win;
        case ModCombo::CtrlAlt:   return ctrl && alt && !shift && !win;
        case ModCombo::WinShift:  return win && shift && !ctrl && !alt;
        case ModCombo::Alt:       return alt && !ctrl && !shift && !win;
        case ModCombo::Ctrl:      return ctrl && !alt && !shift && !win;
        case ModCombo::Shift:     return shift && !ctrl && !alt && !win;
    }
    return false;
}

bool IsFullscreenActive() {
    QUERY_USER_NOTIFICATION_STATE state = QUNS_ACCEPTS_NOTIFICATIONS;
    if (SUCCEEDED(SHQueryUserNotificationState(&state))) {
        if (state == QUNS_BUSY ||
            state == QUNS_RUNNING_D3D_FULL_SCREEN ||
            state == QUNS_PRESENTATION_MODE) {
            return true;
        }
    }
    return false;
}

bool IsMonitorControlAppExe( const std::wstring& exe ) {
    if (exe.empty()) return false;
    return exe == L"eye-careu.exe" ||
           exe == L"eyecareu.exe" ||
           exe == L"display pilot.exe" ||
           exe == L"displaypilot.exe" ||
           exe == L"display pilot 2.exe" ||
           exe == L"displaypilot2.exe" ||
           exe == L"benqdisplaypilot.exe" ||
           exe == L"palettemasterultimate.exe" ||
           exe == L"twinkle tray.exe" ||
           exe == L"twinkletray.exe" ||
           exe == L"monitorian.exe" ||
           exe == L"displaybuddy.exe" ||
           exe == L"clickmonitorddc.exe" ||
           exe == L"f.lux.exe";
}

bool IsProtectedWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd) || IsOwnOverlayWindow(hwnd)) return true;

    wchar_t cls[128]{};
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) > 0) {
        if (wcscmp(cls, kDimClass) == 0 ||
            wcscmp(cls, L"Shell_TrayWnd") == 0 ||
            wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
            wcscmp(cls, L"Progman") == 0 ||
            wcscmp(cls, L"WorkerW") == 0 ||
            wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0 ||
            wcscmp(cls, L"XamlExplorerHostIslandWindow") == 0) {
            return true;
        }
    }
    return false;
}

std::wstring GetWindowExeName(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) return L"";
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return L"";
    wchar_t path[MAX_PATH]{};
    DWORD size = ARRAYSIZE(path);
    std::wstring result;
    if (QueryFullProcessImageNameW(hProc, 0, path, &size)) {
        const wchar_t* slash = wcsrchr(path, L'\\');
        result = slash ? (slash + 1) : path;
        for (auto& ch : result) ch = static_cast<wchar_t>(towlower(ch));
    }
    CloseHandle(hProc);
    return result;
}

bool IsMonitorControlWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return false;
    return IsMonitorControlAppExe(GetWindowExeName(hwnd));
}

bool IsCaptureToolExe(const std::wstring& exe) {
    if (exe.empty()) return false;
    return exe == L"screenclippinghost.exe" ||
           exe == L"snippingtool.exe" ||
           exe == L"screensketch.exe" ||
           exe == L"gamebar.exe" ||
           exe == L"sharex.exe" ||
           exe == L"lightshot.exe" ||
           exe == L"picpick.exe" ||
           exe == L"greenshot.exe" ||
           exe == L"flameshot.exe";
}

bool IsCaptureToolWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return false;
    wchar_t cls[128]{};
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) > 0) {
        if (wcsstr(cls, L"ScreenClipping") != nullptr ||
            wcsstr(cls, L"Snipping") != nullptr ||
            wcscmp(cls, L"ScreenSketch_Window") == 0) {
            return true;
        }
    }
    return IsCaptureToolExe(GetWindowExeName(hwnd));
}

BOOL CALLBACK FindActiveCaptureEnumProc(HWND hwnd, LPARAM lParam) {
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) return TRUE;
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked != 0) {
        return TRUE;
    }
    wchar_t cls[128]{};
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) > 0) {
        if (wcsstr(cls, L"ScreenClipping") != nullptr) {
            *reinterpret_cast<bool*>(lParam) = true;
            return FALSE;
        }
        if (wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0 ||
            wcscmp(cls, L"ApplicationFrameWindow") == 0) {
            std::wstring exe = GetWindowExeName(hwnd);
            if (exe == L"screenclippinghost.exe" || exe == L"screensketch.exe") {
                *reinterpret_cast<bool*>(lParam) = true;
                return FALSE;
            }
        }
    }
    std::wstring exe = GetWindowExeName(hwnd);
    if (exe == L"screenclippinghost.exe") {
        *reinterpret_cast<bool*>(lParam) = true;
        return FALSE;
    }
    return TRUE;
}

bool IsAnyCaptureOverlayVisible() {
    HWND fg = GetForegroundWindow();
    if (fg && IsCaptureToolWindow(fg)) return true;
    bool found = false;
    EnumWindows(FindActiveCaptureEnumProc, reinterpret_cast<LPARAM>(&found));
    return found;
}

HWND GetEffectiveForegroundTarget() {
    HWND fg = GetForegroundWindow();
    if (fg && IsWindow(fg) && !IsProtectedWindow(fg) &&
        !IsMonitorControlWindow(fg) && !IsCaptureToolWindow(fg)) {
        g_lastNormalForeground = fg;
        return fg;
    }
    if (g_lastNormalForeground && IsWindow(g_lastNormalForeground) &&
        IsWindowVisible(g_lastNormalForeground) && !IsIconic(g_lastNormalForeground)) {
        DWORD cloaked = 0;
        if (FAILED(DwmGetWindowAttribute(g_lastNormalForeground, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) || cloaked == 0) {
            return g_lastNormalForeground;
        }
    }
    return fg;
}

RECT GetVisualWindowRect(HWND hwnd) {
    RECT rc{};
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rc, sizeof(rc)))) {
        GetWindowRect(hwnd, &rc);
    }
    return rc;
}

// =============================================================================
// ARCHITECTURAL INVARIANT — HARDWARE BRIGHTNESS IS ALWAYS THE SSOT:
// 1. Vison MUST NEVER fight, poll, read, or write hardware/driver brightness
//    (never call DDC/CI SetMonitorBrightness/GetMonitorBrightness, WMI
//    WmiSetBrightness, SetDeviceGammaRamp, or Night Light/ICC APIs).
// 2. Whatever brightness the physical monitor or external controller (BenQ
//    Eye-CareU, Display Pilot 2, B.I.+, Monitorian, Twinkle Tray, Windows OSD)
//    is currently set to (e.g., 30% on a BenQ monitor) is ALWAYS treated as
//    Vison's 100% baseline (0% overlay alpha = no overlay window).
// 3. Dimming a window to X% in Vison simply places a passive per-window alpha
//    filter over that single window relative to the monitor's current real
//    output, and NEVER places a global HWND_TOPMOST overlay over normal windows
//    or intercepts monitor-control software windows.
// =============================================================================

HWND FindInsertAfterAboveTarget(HWND target) {
    bool targetTop = (GetWindowLongPtrW(target, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
    HWND prev = GetWindow(target, GW_HWNDPREV);
    while (prev) {
        if (IsOwnOverlayWindow(prev)) {
            prev = GetWindow(prev, GW_HWNDPREV);
            continue;
        }
        bool prevTop = (GetWindowLongPtrW(prev, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
        if (!targetTop && prevTop) {
            // Reached the lowest window in the TOPMOST band directly above our top normal window.
            // Return HWND_TOP (not `prev`), because passing a TOPMOST handle to SetWindowPos
            // promotes a non-topmost overlay into the TOPMOST band above Snipping Tool!
            return HWND_TOP;
        }
        return prev;
    }
    return targetTop ? HWND_TOPMOST : HWND_TOP;
}

// -----------------------------------------------------------------------------
// Dim Overlay Management (Zero Hardware Polling — Always Respects Monitor OSD)
// -----------------------------------------------------------------------------

DWORD GetDimOverlayAffinity() {
    return (g_settings.captureMode == CaptureMode::ShowDimOnly ||
            g_settings.captureMode == CaptureMode::ShowAll)
        ? WDA_NONE
        : WDA_EXCLUDEFROMCAPTURE;
}

DWORD GetHeaderControlAffinity() {
    return (g_settings.captureMode == CaptureMode::ShowControlsOnly ||
            g_settings.captureMode == CaptureMode::ShowAll)
        ? WDA_NONE
        : WDA_EXCLUDEFROMCAPTURE;
}

bool ShouldExcludeOpacityDuringCapture() {
    if (!g_settings.restoreOpacityOnCapture) return false;
    return (g_settings.captureMode == CaptureMode::ExcludeAll ||
            g_settings.captureMode == CaptureMode::ShowControlsOnly);
}

bool IsWindowCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked)))) {
        return cloaked != 0;
    }
    return false;
}

bool ShouldShowDimOverlay(const WindowState& st) {
    if (st.dimActive && st.dimBrightness < 100) return true;
    // Contrast-Anchored Crisp Text: when window opacity < 100%, a subtle dark backing
    // anchors background luminance so ClearType glyphs stay high-contrast and never wash out.
    if (g_settings.keepTextCrisp &&
        g_settings.crispMode == CrispTextMode::ContrastAnchor &&
        st.opacityPct < 100) {
        return true;
    }
    return false;
}

BYTE ComputeOverlayAlpha(const WindowState& st) {
    double dimAlpha = 0.0;
    if (st.dimActive && st.dimBrightness < 100) {
        int effBrightness = std::clamp(st.dimBrightness, 0, 100);
        if (g_settings.keepTextCrisp) {
            if (g_settings.crispMode == CrispTextMode::SmartFloor) {
                int minBrightFloor = std::clamp(100 - g_settings.crispStrength, 25, 75);
                effBrightness = std::max(effBrightness, minBrightFloor);
            } else if (g_settings.crispMode == CrispTextMode::HoverFocusCrisp) {
                HWND effFg = GetEffectiveForegroundTarget();
                if (st.target == effFg) {
                    // Gently lift veil on the active reading window so text strokes stay crisp
                    int lift = (100 - effBrightness) * (g_settings.crispStrength / 2) / 100;
                    effBrightness = std::min(100, effBrightness + lift);
                }
            }
        }
        double t = (100.0 - effBrightness) / 100.0;
        double effectiveDimRatio = (g_settings.dimMode == DimMode::AutoScaled)
            ? std::pow(t, 1.35)
            : t;
        double maxAlpha = (g_settings.dimMaxAlphaPercent / 100.0) * 255.0;
        if (g_settings.keepTextCrisp && g_settings.crispMode == CrispTextMode::ContrastAnchor) {
            // Soften veil ceiling slightly when contrast-anchoring so dark text on light UI stays crisp
            double clarityScale = 1.0 - (g_settings.crispStrength / 320.0);
            maxAlpha *= std::clamp(clarityScale, 0.70, 0.95);
        }
        dimAlpha = effectiveDimRatio * maxAlpha;
    }

    if (g_settings.keepTextCrisp &&
        g_settings.crispMode == CrispTextMode::ContrastAnchor &&
        st.opacityPct < 100 && dimAlpha < 18.0) {
        // Provide a crisp contrast anchor proportional to translucency so background text doesn't bleed
        double transRatio = (100.0 - st.opacityPct) / 100.0;
        double anchorAlpha = transRatio * (g_settings.crispStrength / 100.0) * 64.0;
        dimAlpha = std::max(dimAlpha, anchorAlpha);
    }

    int alpha = static_cast<int>(std::lround(dimAlpha));
    return static_cast<BYTE>(std::clamp(alpha, 0, 242));
}

bool IsOverlayDirectlyAboveTarget(HWND overlay, HWND target) {
    HWND prev = GetWindow(target, GW_HWNDPREV);
    while (prev && IsOwnOverlayWindow(prev)) {
        if (prev == overlay) return true;
        prev = GetWindow(prev, GW_HWNDPREV);
    }
    return false;
}

void PositionOverlayAboveTarget(WindowState& st, bool forceZOrder) {
    HWND overlay = st.overlayHwnd;
    HWND target = st.target;
    if (!overlay || !IsWindow(overlay) || !target || !IsWindow(target)) return;

    HWND effFg = GetEffectiveForegroundTarget();
    if (IsIconic(target) || !IsWindowVisible(target) || IsWindowCloaked(target) ||
        (!g_settings.persistWhenUnfocused && target != effFg)) {
        if (IsWindowVisible(overlay)) ShowWindow(overlay, SW_HIDE);
        SetRectEmpty(&st.lastOverlayRect);
        return;
    }

    RECT rc = GetVisualWindowRect(target);
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0) return;

    bool visible = IsWindowVisible(overlay) != 0;
    bool prevValid = !IsRectEmpty(&st.lastOverlayRect);
    int oldW = prevValid ? (st.lastOverlayRect.right - st.lastOverlayRect.left) : 0;
    int oldH = prevValid ? (st.lastOverlayRect.bottom - st.lastOverlayRect.top) : 0;
    bool posChanged = !prevValid || rc.left != st.lastOverlayRect.left || rc.top != st.lastOverlayRect.top;
    bool sizeChanged = !prevValid || w != oldW || h != oldH;
    bool rectChanged = posChanged || sizeChanged;

    bool targetTop = (GetWindowLongPtrW(target, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
    bool overlayTop = (GetWindowLongPtrW(overlay, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
    bool bandMismatch = (targetTop != overlayTop);
    bool zOrderOk = !bandMismatch && IsOverlayDirectlyAboveTarget(overlay, target);

    // During active capture sessions, never fight Z-order if already visible and sized.
    if (g_captureSessionActive && visible && !rectChanged) {
        return;
    }

    if (!rectChanged && visible && (zOrderOk || !forceZOrder)) {
        return;
    }

    // Freeze corner-rounding preference to DWMWCP_DONOTROUND during interactive edge-resize
    // so DWM's 1px rounded border anti-aliasing never creates a white/black edge seam.
    DWORD cornerPref = (IsZoomed(target) || st.isMoveSizing) ? 1 /*DWMWCP_DONOTROUND*/ : 2 /*DWMWCP_ROUND*/;
    if (cornerPref != st.lastCornerPref) {
        st.lastCornerPref = cornerPref;
        DwmSetWindowAttribute(overlay, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPref, sizeof(cornerPref));
    }

    st.lastOverlayRect = rc;
    if (bandMismatch) {
        SetWindowPos(overlay, targetTop ? HWND_TOPMOST : HWND_NOTOPMOST,
                     0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
        zOrderOk = IsOverlayDirectlyAboveTarget(overlay, target);
    }

    // Do NOT pass SWP_NOCOPYBITS on resize: keeping existing client bits + CS_HREDRAW|CS_VREDRAW
    // class background brush prevents any transient black/white strip along the dragged edge.
    UINT flags = SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_DEFERERASE;
    if (!visible) flags |= SWP_SHOWWINDOW;
    if (!posChanged) flags |= SWP_NOMOVE;
    if (!sizeChanged) {
        flags |= SWP_NOSIZE | SWP_NOREDRAW;
    }

    if (zOrderOk || g_captureSessionActive) {
        flags |= SWP_NOZORDER;
        SetWindowPos(overlay, nullptr, rc.left, rc.top, w, h, flags);
    } else {
        HWND insertAfter = FindInsertAfterAboveTarget(target);
        SetWindowPos(overlay, insertAfter, rc.left, rc.top, w, h, flags);
        if (!IsOverlayDirectlyAboveTarget(overlay, target)) {
            // Only reorder overlay relative to target; never call SetWindowPos on target during capture or move-size
            SetWindowPos(overlay, target, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOREDRAW);
            if (!st.isMoveSizing && !g_captureSessionActive) {
                SetWindowPos(target, overlay, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOREDRAW);
            }
        }
    }

    if (sizeChanged) {
        RedrawWindow(overlay, nullptr, nullptr, RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOERASE);
    }
}

void SyncDimOverlay(WindowState& st) {
    if (!ShouldShowDimOverlay(st)) {
        if (st.overlayHwnd && IsWindow(st.overlayHwnd)) {
            DestroyWindow(st.overlayHwnd);
        }
        st.overlayHwnd = nullptr;
        SetRectEmpty(&st.lastOverlayRect);
        st.lastCornerPref = 0;
        return;
    }

    if (!st.overlayHwnd || !IsWindow(st.overlayHwnd)) {
        RECT rc = GetVisualWindowRect(st.target);
        // Create unowned (hWndParent = nullptr) to avoid cross-thread input queue attachment and hung-app deadlocks.
        st.overlayHwnd = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
            kDimClass, L"", WS_POPUP,
            rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
            nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
        if (!st.overlayHwnd) return;
        SetWindowDisplayAffinity(st.overlayHwnd, GetDimOverlayAffinity());
        DWORD cornerPref = (IsZoomed(st.target) || st.isMoveSizing) ? 1 /*DWMWCP_DONOTROUND*/ : 2 /*DWMWCP_ROUND*/;
        st.lastCornerPref = cornerPref;
        DwmSetWindowAttribute(st.overlayHwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPref, sizeof(cornerPref));
        SetRectEmpty(&st.lastOverlayRect);
    }

    BYTE alpha = ComputeOverlayAlpha(st);
    SetLayeredWindowAttributes(st.overlayHwnd, 0, alpha, LWA_ALPHA);
    PositionOverlayAboveTarget(st, true);
}

// -----------------------------------------------------------------------------
// Ledger & Cleanup
// -----------------------------------------------------------------------------

WindowState& GetOrCreateLedgerEntry(HWND target) {
    auto it = g_ledger.find(target);
    if (it != g_ledger.end()) return it->second;

    WindowState st;
    st.target = target;
    st.exeName = GetWindowExeName(target);
    LONG_PTR ex = GetWindowLongPtrW(target, GWL_EXSTYLE);
    st.wasTopmost = (ex & WS_EX_TOPMOST) != 0;
    if ((ex & WS_EX_LAYERED) != 0) {
        BYTE a = 255;
        DWORD flags = 0;
        if (GetLayeredWindowAttributes(target, nullptr, &a, &flags) && (flags & LWA_ALPHA)) {
            st.currentAlpha = a;
            st.opacityPct = std::clamp(static_cast<int>(std::lround((a / 255.0) * 100.0)), 10, 100);
        }
    }
    auto [inserted, _] = g_ledger.emplace(target, std::move(st));
    return inserted->second;
}

void RestoreWindowState(WindowState& st) {
    if (st.overlayHwnd && IsWindow(st.overlayHwnd)) {
        DestroyWindow(st.overlayHwnd);
    }
    st.overlayHwnd = nullptr;
    st.dimActive = false;

    if (st.target && IsWindow(st.target)) {
        if (st.addedLayered) {
            LONG_PTR ex = GetWindowLongPtrW(st.target, GWL_EXSTYLE);
            SetWindowLongPtrW(st.target, GWL_EXSTYLE, ex & ~WS_EX_LAYERED);
        } else if (st.currentAlpha != 255) {
            SetLayeredWindowAttributes(st.target, 0, 255, LWA_ALPHA);
        }
        if (st.pinnedByMod && !st.wasTopmost) {
            SetWindowPos(st.target, HWND_NOTOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }
}

void RestoreAllWindows() {
    for (auto& [hwnd, st] : g_ledger) {
        RestoreWindowState(st);
    }
    g_ledger.clear();
    if (g_badgeWnd) ShowWindow(g_badgeWnd, SW_HIDE);
    if (g_sliderWnd) ShowWindow(g_sliderWnd, SW_HIDE);
    g_headerTarget = nullptr;
}

// -----------------------------------------------------------------------------
// Optional Header % Circle Button & Hover-Reveal Slider (GDI+ ARGB Layered)
// -----------------------------------------------------------------------------

int GetTargetDisplayPercent(HWND target) {
    auto it = g_ledger.find(target);
    if (it == g_ledger.end() || !it->second.dimActive) return 100;
    return std::clamp(it->second.dimBrightness, 0, 100);
}

UINT GetTargetDpi(HWND target) {
    UINT dpi = (target && IsWindow(target)) ? GetDpiForWindow(target) : 96;
    return dpi ? dpi : 96;
}

bool DetectHeaderIsLight(HWND target, int sampleX, int sampleY) {
    HDC screenDC = GetDC(nullptr);
    COLORREF c = GetPixel(screenDC, sampleX, sampleY);
    ReleaseDC(nullptr, screenDC);
    if (c != CLR_INVALID) {
        int r = GetRValue(c), g = GetGValue(c), b = GetBValue(c);
        int lum = (299 * r + 587 * g + 114 * b) / 1000;
        return lum > 135;
    }
    BOOL darkMode = FALSE;
    DwmGetWindowAttribute(target, 20 /*DWMWA_USE_IMMERSIVE_DARK_MODE*/, &darkMode, sizeof(darkMode));
    return !darkMode;
}

void RenderBadgeLayered(HWND hwnd) {
    if (!g_headerTarget || !IsWindow(g_headerTarget)) return;
    UINT dpi = GetTargetDpi(g_headerTarget);
    int size = MulDiv(30, dpi, 96);

    HDC screenDC = GetDC(nullptr);
    if (g_badgeDib.Ensure(screenDC, size, size)) {
        Gdiplus::Graphics g(g_badgeDib.memDC);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
        g.Clear(Gdiplus::Color(0, 0, 0, 0));

        if (g_settings.headerShowFill) {
            Gdiplus::SolidBrush bg(g_badgeHovered ? Gdiplus::Color(245, 16, 16, 20)
                                                 : Gdiplus::Color(235, 0, 0, 0));
            g.FillEllipse(&bg, 0.5f, 0.5f, size - 1.0f, size - 1.0f);
        }

        int pct = GetTargetDisplayPercent(g_headerTarget);
        wchar_t label[16];
        swprintf_s(label, L"%d%%", pct);

        float fontSize = (pct >= 100) ? (size * 0.34f) : (size * 0.39f);
        Gdiplus::Font font(L"Segoe UI", fontSize, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::StringFormat fmt(Gdiplus::StringFormat::GenericTypographic());
        fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
        fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        Gdiplus::RectF box(0.0f, 0.0f, static_cast<float>(size), static_cast<float>(size));

        // Bare text adapts to Light vs Dark header; filled pill always uses white on OLED Black.
        bool useDarkText = (!g_settings.headerShowFill && g_headerIsLight);
        Gdiplus::SolidBrush textBrush(useDarkText ? Gdiplus::Color(245, 10, 10, 14)
                                                  : Gdiplus::Color(252, 255, 255, 255));
        g.DrawString(label, -1, &font, box, &fmt, &textBrush);

        POINT ptSrc{0, 0};
        SIZE wndSize{size, size};
        BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(hwnd, screenDC, nullptr, &wndSize, g_badgeDib.memDC, &ptSrc, 0, &bf, ULW_ALPHA);
    }
    ReleaseDC(nullptr, screenDC);
}

void RenderSliderLayered(HWND hwnd) {
    if (!g_headerTarget || !IsWindow(g_headerTarget)) return;
    UINT dpi = GetTargetDpi(g_headerTarget);
    int w = MulDiv(136, dpi, 96);
    int h = MulDiv(28, dpi, 96);
    BYTE masterAlpha = static_cast<BYTE>(std::clamp(static_cast<int>(g_sliderAnim.current * 255.0f), 0, 255));

    HDC screenDC = GetDC(nullptr);
    if (g_sliderDib.Ensure(screenDC, w, h)) {
        Gdiplus::Graphics g(g_sliderDib.memDC);
        g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
        g.Clear(Gdiplus::Color(0, 0, 0, 0));

        // Minimal borderless pill background.
        float d = h - 1.0f;
        Gdiplus::GraphicsPath pill;
        pill.AddArc(0.5f, 0.5f, d, d, 90.0f, 180.0f);
        pill.AddArc(w - d - 0.5f, 0.5f, d, d, 270.0f, 180.0f);
        pill.CloseFigure();
        Gdiplus::SolidBrush bg(Gdiplus::Color(245, 0, 0, 0));
        g.FillPath(&bg, &pill);

        // Track and circular thumb only (no +/- buttons).
        float cy = h / 2.0f;
        float pad = h * 0.45f;
        float trackLeft = pad;
        float trackRight = w - pad;
        float trackW = trackRight - trackLeft;
        float trackH = std::max(3.0f, h * 0.15f);

        Gdiplus::SolidBrush trackBg(Gdiplus::Color(90, 255, 255, 255));
        g.FillRectangle(&trackBg, trackLeft, cy - trackH / 2.0f, trackW, trackH);

        int pct = GetTargetDisplayPercent(g_headerTarget);
        float fillW = trackW * (pct / 100.0f);
        Gdiplus::SolidBrush trackFill(Gdiplus::Color(245, 247, 247, 252));
        g.FillRectangle(&trackFill, trackLeft, cy - trackH / 2.0f, fillW, trackH);

        float thumbR = h * 0.24f;
        float thumbX = trackLeft + fillW;
        Gdiplus::SolidBrush thumbHalo(Gdiplus::Color(60, 255, 255, 255));
        g.FillEllipse(&thumbHalo, thumbX - thumbR * 1.35f, cy - thumbR * 1.35f, thumbR * 2.7f, thumbR * 2.7f);
        g.FillEllipse(&trackFill, thumbX - thumbR, cy - thumbR, thumbR * 2.0f, thumbR * 2.0f);

        int dir = g_settings.headerAfterTitle ? -1 : 1;
        int slideOffset = dir * static_cast<int>(std::lround((1.0f - g_sliderAnim.current) * MulDiv(10, dpi, 96)));
        POINT ptDst{g_sliderBaseX + slideOffset, g_sliderBaseY};
        POINT ptSrc{0, 0};
        SIZE wndSize{w, h};
        BLENDFUNCTION bf{AC_SRC_OVER, 0, masterAlpha, AC_SRC_ALPHA};
        UpdateLayeredWindow(hwnd, screenDC, &ptDst, &wndSize, g_sliderDib.memDC, &ptSrc, 0, &bf, ULW_ALPHA);
    }
    ReleaseDC(nullptr, screenDC);
}

void StartSliderAnimation(float targetAlpha) {
    if (std::fabs(g_sliderAnim.target - targetAlpha) < 0.01f && g_sliderAnim.active) return;
    g_sliderAnim.start = g_sliderAnim.current;
    g_sliderAnim.target = targetAlpha;
    g_sliderAnim.active = true;
    QueryPerformanceCounter(&g_sliderAnim.startTick);
    if (g_sliderWnd && targetAlpha > 0.0f) {
        ShowWindow(g_sliderWnd, SW_SHOWNOACTIVATE);
    }
    if (g_ctrlWnd) SetTimer(g_ctrlWnd, TIMER_ANIMATION, 16, nullptr);
}

void PositionHeaderControls(HWND target, bool queryTargetIPC = true) {
    HWND effFg = GetEffectiveForegroundTarget();
    if (!g_settings.headerControlEnabled || !target || !IsWindow(target) ||
        target != effFg || g_captureSessionActive || IsIconic(target) ||
        !IsWindowVisible(target) || IsWindowCloaked(target)) {
        if (g_badgeWnd) ShowWindow(g_badgeWnd, SW_HIDE);
        if (g_sliderWnd) ShowWindow(g_sliderWnd, SW_HIDE);
        return;
    }

    RECT rc = GetVisualWindowRect(target);
    UINT dpi = GetTargetDpi(target);
    int winW = rc.right - rc.left;
    if (winW < MulDiv(360, dpi, 96)) {
        if (g_badgeWnd) ShowWindow(g_badgeWnd, SW_HIDE);
        if (g_sliderWnd) ShowWindow(g_sliderWnd, SW_HIDE);
        return;
    }

    int badgeSize = MulDiv(30, dpi, 96);
    int sliderW = MulDiv(136, dpi, 96);
    int sliderH = MulDiv(28, dpi, 96);

    bool needIPC = queryTargetIPC || (target != g_cachedHeaderHwnd) || (g_cachedCaptionReserve <= 0);
    int captionReserve = needIPC ? MulDiv(148, dpi, 96) : g_cachedCaptionReserve;

    if (needIPC) {
        TITLEBARINFOEX tbi{sizeof(TITLEBARINFOEX)};
        if (SendMessageTimeoutW(target, WM_GETTITLEBARINFOEX, 0, reinterpret_cast<LPARAM>(&tbi),
                                SMTO_ABORTIFHUNG | SMTO_BLOCK, 15, nullptr) &&
            tbi.rcTitleBar.right > tbi.rcTitleBar.left) {
            int minBtnLeft = rc.right;
            for (int i = 2; i <= 5; ++i) {
                if (tbi.rgrect[i].left > rc.left && tbi.rgrect[i].left < minBtnLeft) {
                    minBtnLeft = tbi.rgrect[i].left;
                }
            }
            if (minBtnLeft < rc.right) {
                captionReserve = (rc.right - minBtnLeft) + MulDiv(8, dpi, 96);
            }
        }
        g_cachedCaptionReserve = captionReserve;
    }

    // Skip borderless/non-caption popup windows that have no title bar.
    LONG_PTR style = GetWindowLongPtrW(target, GWL_STYLE);
    if ((style & WS_CAPTION) == 0 && needIPC) {
        DWORD_PTR capHit = HTNOWHERE;
        POINT midPt{rc.left + winW / 2, rc.top + MulDiv(16, dpi, 96)};
        if (!SendMessageTimeoutW(target, WM_NCHITTEST, 0, MAKELPARAM(midPt.x, midPt.y),
                                 SMTO_ABORTIFHUNG | SMTO_BLOCK, 15, &capHit) ||
            (capHit != HTCAPTION && capHit != HTCLIENT)) {
            if (g_badgeWnd) ShowWindow(g_badgeWnd, SW_HIDE);
            if (g_sliderWnd) ShowWindow(g_sliderWnd, SW_HIDE);
            return;
        }
    }

    int badgeY = rc.top + MulDiv(4, dpi, 96);
    int badgeX = 0;

    if (g_settings.headerAfterTitle) {
        int titlePx = needIPC ? MulDiv(64, dpi, 96) : g_cachedTitlePx;
        if (needIPC) {
            wchar_t title[128]{};
            int titleLen = static_cast<int>(DefWindowProcW(target, WM_GETTEXT, ARRAYSIZE(title), reinterpret_cast<LPARAM>(title)));
            if (titleLen <= 0) {
                SendMessageTimeoutW(target, WM_GETTEXT, ARRAYSIZE(title), reinterpret_cast<LPARAM>(title),
                                    SMTO_ABORTIFHUNG | SMTO_BLOCK, 15, nullptr);
            }
            if (title[0] != 0) {
                HDC hdc = GetDC(nullptr);
                Gdiplus::Graphics g(hdc);
                Gdiplus::Font titleFont(L"Segoe UI", static_cast<float>(MulDiv(12, dpi, 96)),
                                        Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
                Gdiplus::RectF measured{};
                g.MeasureString(title, -1, &titleFont, Gdiplus::PointF(0, 0), &measured);
                ReleaseDC(nullptr, hdc);
                titlePx = std::clamp(static_cast<int>(std::ceil(measured.Width)) + MulDiv(24, dpi, 96),
                                     MulDiv(52, dpi, 96), MulDiv(240, dpi, 96));
            }
            g_cachedTitlePx = titlePx;
        }
        badgeX = rc.left + titlePx + MulDiv(10, dpi, 96);
        int maxRight = rc.right - captionReserve - badgeSize - sliderW - MulDiv(16, dpi, 96);
        if (badgeX > maxRight) {
            if (g_badgeWnd) ShowWindow(g_badgeWnd, SW_HIDE);
            if (g_sliderWnd) ShowWindow(g_sliderWnd, SW_HIDE);
            return;
        }
    } else {
        badgeX = rc.right - std::max(captionReserve, MulDiv(172, dpi, 96)) - badgeSize;
        if (badgeX <= rc.left + MulDiv(80, dpi, 96)) {
            if (g_badgeWnd) ShowWindow(g_badgeWnd, SW_HIDE);
            if (g_sliderWnd) ShowWindow(g_sliderWnd, SW_HIDE);
            return;
        }
    }

    if (needIPC) {
        DWORD_PTR hit = HTNOWHERE;
        POINT centerPt{badgeX + badgeSize / 2, badgeY + badgeSize / 2};
        if (SendMessageTimeoutW(target, WM_NCHITTEST, 0, MAKELPARAM(centerPt.x, centerPt.y),
                                SMTO_ABORTIFHUNG | SMTO_BLOCK, 15, &hit)) {
            if (hit == HTMINBUTTON || hit == HTMAXBUTTON || hit == HTCLOSE ||
                hit == HTSYSMENU || hit == HTHELP) {
                if (g_badgeWnd) ShowWindow(g_badgeWnd, SW_HIDE);
                if (g_sliderWnd) ShowWindow(g_sliderWnd, SW_HIDE);
                return;
            }
        }
        g_cachedHeaderHwnd = target;
    }

    if (!IsWindowVisible(g_badgeWnd)) {
        g_headerIsLight = DetectHeaderIsLight(target, badgeX + badgeSize / 2, badgeY + badgeSize / 2);
    }

    RenderBadgeLayered(g_badgeWnd);
    bool targetTopmost = (GetWindowLongPtrW(target, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
    SetWindowPos(g_badgeWnd, targetTopmost ? HWND_TOPMOST : HWND_TOP,
                 badgeX, badgeY, badgeSize, badgeSize,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);

    g_sliderBaseX = g_settings.headerAfterTitle
        ? (badgeX + badgeSize + MulDiv(6, dpi, 96))
        : (badgeX - sliderW - MulDiv(6, dpi, 96));
    g_sliderBaseY = badgeY - (sliderH - badgeSize) / 2;
    SetWindowPos(g_sliderWnd, g_badgeWnd, g_sliderBaseX, g_sliderBaseY, sliderW, sliderH,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER | (g_sliderAnim.current > 0.01f ? SWP_SHOWWINDOW : 0));
    if (g_sliderAnim.current > 0.01f) {
        RenderSliderLayered(g_sliderWnd);
    }
}

void RefreshHeaderControlsFor(HWND target) {
    if (!g_settings.headerControlEnabled || g_headerTarget != target) return;
    if (g_badgeWnd && IsWindowVisible(g_badgeWnd)) RenderBadgeLayered(g_badgeWnd);
    if (g_sliderWnd && IsWindowVisible(g_sliderWnd)) RenderSliderLayered(g_sliderWnd);
}

// -----------------------------------------------------------------------------
// Core Actions (Opacity, Pin, Dim, Toggle)
// -----------------------------------------------------------------------------

int ComputeEffectiveOpacityPct(const WindowState& st) {
    int basePct = std::clamp(st.opacityPct, g_settings.opacityMin, 100);
    if (basePct >= 100) return 100;

    // Exclude decreased opacity during screenshots/captures when captureBehavior excludes Vison effects
    if (g_captureSessionActive && ShouldExcludeOpacityDuringCapture()) {
        return 100;
    }

    if (g_settings.keepTextCrisp) {
        if (g_settings.crispMode == CrispTextMode::HoverFocusCrisp) {
            HWND effFg = GetEffectiveForegroundTarget();
            if (st.target == effFg) {
                return 100;
            }
        } else if (g_settings.crispMode == CrispTextMode::SmartFloor) {
            int crispMinFloor = std::clamp(35 + (g_settings.crispStrength / 2), g_settings.opacityMin, 92);
            return std::max(basePct, crispMinFloor);
        } else if (g_settings.crispMode == CrispTextMode::ContrastAnchor) {
            // Boost effective glyph alpha slightly while the dark contrast-anchor overlay provides depth
            int boost = (100 - basePct) * (g_settings.crispStrength / 3) / 100;
            return std::clamp(basePct + boost, g_settings.opacityMin, 100);
        }
    }
    return basePct;
}

bool EnsureTargetOpacityApplied(WindowState& st) {
    if (!st.target || !IsWindow(st.target)) return false;
    st.opacityPct = std::clamp(st.opacityPct, g_settings.opacityMin, 100);
    int effPct = ComputeEffectiveOpacityPct(st);
    BYTE nextAlpha = static_cast<BYTE>(std::lround((effPct / 100.0) * 255.0));

    if (effPct >= 100) {
        // If user opacity is genuinely 100%, remove WS_EX_LAYERED if we added it.
        // If temporarily at 100% due to Capture Guard or HoverFocusCrisp, keep WS_EX_LAYERED
        // and set alpha = 255 so restoring back to <100% never flashes or reallocates redirection surfaces!
        if (st.opacityPct >= 100) {
            if (st.addedLayered) {
                LONG_PTR ex = GetWindowLongPtrW(st.target, GWL_EXSTYLE);
                if ((ex & WS_EX_LAYERED) != 0) {
                    SetLayeredWindowAttributes(st.target, 0, 255, LWA_ALPHA);
                    SetWindowLongPtrW(st.target, GWL_EXSTYLE, ex & ~WS_EX_LAYERED);
                }
                st.addedLayered = false;
            } else if (st.currentAlpha != 255) {
                SetLayeredWindowAttributes(st.target, 0, 255, LWA_ALPHA);
            }
        } else {
            LONG_PTR ex = GetWindowLongPtrW(st.target, GWL_EXSTYLE);
            if ((ex & WS_EX_LAYERED) != 0 && st.currentAlpha != 255) {
                SetLayeredWindowAttributes(st.target, 0, 255, LWA_ALPHA);
            }
        }
        st.currentAlpha = 255;
        return true;
    }

    LONG_PTR ex = GetWindowLongPtrW(st.target, GWL_EXSTYLE);
    bool needSetAttr = (st.currentAlpha != nextAlpha);
    if ((ex & WS_EX_LAYERED) == 0) {
        SetLastError(0);
        if (SetWindowLongPtrW(st.target, GWL_EXSTYLE, ex | WS_EX_LAYERED) == 0 && GetLastError() != 0) {
            return false;
        }
        st.addedLayered = true;
        needSetAttr = true;
    } else {
        BYTE actualAlpha = 255;
        DWORD flags = 0;
        if (!GetLayeredWindowAttributes(st.target, nullptr, &actualAlpha, &flags) ||
            (flags & LWA_ALPHA) == 0 || actualAlpha != nextAlpha) {
            needSetAttr = true;
        }
    }

    if (needSetAttr) {
        if (!SetLayeredWindowAttributes(st.target, 0, nextAlpha, LWA_ALPHA)) {
            return false;
        }
    }
    st.currentAlpha = nextAlpha;
    return true;
}

void ApplyOpacityStep(HWND target, int wheelDelta) {
    if (g_lastWheelTarget != target) {
        g_lastWheelTarget = target;
        g_wheelAccumOpacity = 0.0f;
        g_wheelAccumDim = 0.0f;
    }
    g_wheelAccumOpacity += static_cast<float>(wheelDelta) / WHEEL_DELTA;
    int steps = static_cast<int>(g_wheelAccumOpacity);
    if (steps == 0) return;
    g_wheelAccumOpacity -= static_cast<float>(steps);

    WindowState& st = GetOrCreateLedgerEntry(target);
    int nextPct = std::clamp(st.opacityPct + steps * g_settings.opacityStep, g_settings.opacityMin, 100);
    st.opacityPct = nextPct;
    if (EnsureTargetOpacityApplied(st)) {
        if (g_settings.keepTextCrisp && g_settings.crispMode == CrispTextMode::ContrastAnchor) {
            SyncDimOverlay(st);
        }
        if (!st.exeName.empty()) {
            g_appOpacityPresets[st.exeName] = st.opacityPct;
            if (g_ctrlWnd) SetTimer(g_ctrlWnd, TIMER_SAVE_PRESETS, 400, nullptr);
        }
    }
}

void TogglePinWindow(HWND target) {
    WindowState& st = GetOrCreateLedgerEntry(target);
    bool isTop = (GetWindowLongPtrW(target, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
    HWND zOrder = isTop ? HWND_NOTOPMOST : HWND_TOPMOST;
    if (SetWindowPos(target, zOrder, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE)) {
        st.pinnedByMod = !isTop;
        if (st.dimActive && st.overlayHwnd) {
            PositionOverlayAboveTarget(st, true);
        }
        if (g_settings.headerControlEnabled && g_headerTarget == target) {
            PositionHeaderControls(target);
        }
    }
}

void SetWindowDimBrightness(HWND target, int newBrightness) {
    WindowState& st = GetOrCreateLedgerEntry(target);
    st.dimBrightness = std::clamp(newBrightness, 0, 100);
    st.dimActive = (st.dimBrightness < 100);

    if (!st.exeName.empty()) {
        g_appPresets[st.exeName] = st.dimBrightness;
        if (g_ctrlWnd) SetTimer(g_ctrlWnd, TIMER_SAVE_PRESETS, 400, nullptr);
    }

    SyncDimOverlay(st);
    RefreshHeaderControlsFor(target);
}

void ApplyDimStep(HWND target, int wheelDelta) {
    if (g_lastWheelTarget != target) {
        g_lastWheelTarget = target;
        g_wheelAccumOpacity = 0.0f;
        g_wheelAccumDim = 0.0f;
    }
    g_wheelAccumDim += static_cast<float>(wheelDelta) / WHEEL_DELTA;
    int steps = static_cast<int>(g_wheelAccumDim);
    if (steps == 0) return;
    g_wheelAccumDim -= static_cast<float>(steps);

    WindowState& st = GetOrCreateLedgerEntry(target);
    SetWindowDimBrightness(target, st.dimBrightness + steps * g_settings.dimStep);
}

void ToggleSavedAppDim(HWND target) {
    WindowState& st = GetOrCreateLedgerEntry(target);
    if (st.dimActive) {
        st.dimActive = false;
        SyncDimOverlay(st);
        } else {
        int preset = 70;
        if (!st.exeName.empty()) {
            auto it = g_appPresets.find(st.exeName);
            if (it != g_appPresets.end() && it->second < 100) {
                preset = it->second;
            }
        }
        st.dimBrightness = preset;
        st.dimActive = true;
        SyncDimOverlay(st);
        }
    RefreshHeaderControlsFor(target);
}

void SetSliderValueFromX(HWND sliderWnd, int x) {
    if (!g_headerTarget || !IsWindow(g_headerTarget)) return;
    RECT rc{};
    GetClientRect(sliderWnd, &rc);
    float pad = rc.bottom * 0.45f;
    float trackLeft = pad;
    float trackRight = rc.right - pad;
    float ratio = std::clamp((x - trackLeft) / std::max(1.0f, trackRight - trackLeft), 0.0f, 1.0f);
    SetWindowDimBrightness(g_headerTarget, static_cast<int>(std::lround(ratio * 100.0f)));
}

// -----------------------------------------------------------------------------
// Window Procedures
// -----------------------------------------------------------------------------

LRESULT CALLBACK DimOverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_ERASEBKGND) {
        HDC hdc = reinterpret_cast<HDC>(wParam);
        if (hdc && g_dimBrush) {
            RECT rc{};
            GetClientRect(hwnd, &rc);
            FillRect(hdc, &rc, g_dimBrush);
        }
        return 1;
    }
    if (msg == WM_PAINT) {
        PAINTSTRUCT ps{};
        HDC hdc = BeginPaint(hwnd, &ps);
        if (g_dimBrush) {
            RECT rc{};
            GetClientRect(hwnd, &rc);
            FillRect(hdc, &rc, g_dimBrush);
        }
        EndPaint(hwnd, &ps);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK BadgeWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_MOUSEMOVE:
            if (!g_badgeHovered) {
                g_badgeHovered = true;
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
                RenderBadgeLayered(hwnd);
                StartSliderAnimation(1.0f);
                SetTimer(g_ctrlWnd, TIMER_HOVER_CHECK, 100, nullptr);
            }
            return 0;
        case WM_MOUSELEAVE:
            g_badgeHovered = false;
            RenderBadgeLayered(hwnd);
            return 0;
        case WM_LBUTTONDOWN:
            if (g_headerTarget) ToggleSavedAppDim(g_headerTarget);
            return 0;
        case WM_MOUSEWHEEL:
            if (g_headerTarget) {
                g_wheelAccumSlider += static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;
                int steps = static_cast<int>(g_wheelAccumSlider);
                if (steps != 0) {
                    g_wheelAccumSlider -= static_cast<float>(steps);
                    SetWindowDimBrightness(g_headerTarget,
                                           GetTargetDisplayPercent(g_headerTarget) + steps * g_settings.dimStep);
                }
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK SliderWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_MOUSEMOVE:
            if (!g_sliderHovered) {
                g_sliderHovered = true;
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
                StartSliderAnimation(1.0f);
            }
            if (g_sliderDragging) {
                if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0) {
                    g_sliderDragging = false;
                    ReleaseCapture();
                    return 0;
                }
                SetSliderValueFromX(hwnd, GET_X_LPARAM(lParam));
            }
            return 0;
        case WM_MOUSELEAVE:
            g_sliderHovered = false;
            return 0;
        case WM_LBUTTONDOWN: {
            if (!g_headerTarget) return 0;
            g_sliderDragging = true;
            SetCapture(hwnd);
            SetSliderValueFromX(hwnd, GET_X_LPARAM(lParam));
            return 0;
        }
        case WM_LBUTTONUP:
            if (g_sliderDragging) {
                g_sliderDragging = false;
                ReleaseCapture();
            }
            return 0;
        case WM_CAPTURECHANGED:
            g_sliderDragging = false;
            return 0;
        case WM_MOUSEWHEEL:
            if (g_headerTarget) {
                g_wheelAccumSlider += static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;
                int steps = static_cast<int>(g_wheelAccumSlider);
                if (steps != 0) {
                    g_wheelAccumSlider -= static_cast<float>(steps);
                    SetWindowDimBrightness(g_headerTarget,
                                           GetTargetDisplayPercent(g_headerTarget) + steps * g_settings.dimStep);
                }
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK ControlWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_APP_RELOAD_SETTINGS: {
            LoadSettings();
            DWORD ctrlAffinity = GetHeaderControlAffinity();
            DWORD dimAffinity = GetDimOverlayAffinity();
            if (g_badgeWnd) SetWindowDisplayAffinity(g_badgeWnd, ctrlAffinity);
            if (g_sliderWnd) SetWindowDisplayAffinity(g_sliderWnd, ctrlAffinity);
            for (auto& [tHwnd, st] : g_ledger) {
                EnsureTargetOpacityApplied(st);
                if (st.overlayHwnd && IsWindow(st.overlayHwnd)) {
                    SetWindowDisplayAffinity(st.overlayHwnd, dimAffinity);
                    InvalidateRect(st.overlayHwnd, nullptr, FALSE);
                    UpdateWindow(st.overlayHwnd);
                }
                SyncDimOverlay(st);
            }
            if (!g_settings.headerControlEnabled) {
                if (g_badgeWnd) ShowWindow(g_badgeWnd, SW_HIDE);
                if (g_sliderWnd) ShowWindow(g_sliderWnd, SW_HIDE);
            } else {
                if (!g_headerTarget || !IsWindow(g_headerTarget)) {
                    HWND fg = GetForegroundWindow();
                    if (fg && !IsProtectedWindow(fg)) g_headerTarget = fg;
                }
                if (g_headerTarget && IsWindow(g_headerTarget)) {
                    PositionHeaderControls(g_headerTarget, true);
                }
            }
            return 0;
        }
        case WM_APP_HOOK_WHEEL: {
            HWND target = reinterpret_cast<HWND>(lParam);
            if (!target || !IsWindow(target) || IsMonitorControlWindow(target)) return 0;
            int delta = static_cast<int>(static_cast<SHORT>(HIWORD(wParam)));
            bool isOpacity = (LOWORD(wParam) == 1);
            if (isOpacity) ApplyOpacityStep(target, delta);
            else ApplyDimStep(target, delta);
            return 0;
        }
        case WM_APP_HOOK_BUTTON: {
            HWND target = reinterpret_cast<HWND>(lParam);
            if (!target || !IsWindow(target) || IsMonitorControlWindow(target)) return 0;
            if (wParam == 1) ToggleSavedAppDim(target);
            else TogglePinWindow(target);
            return 0;
        }
        case WM_APP_HOOK_DBLCLICK: {
            HWND target = reinterpret_cast<HWND>(lParam);
            if (target && IsWindow(target) && !IsMonitorControlWindow(target)) ToggleSavedAppDim(target);
            return 0;
        }
        case WM_TIMER: {
            if (wParam == TIMER_ANIMATION) {
                LARGE_INTEGER now{};
                QueryPerformanceCounter(&now);
                float elapsed = static_cast<float>((now.QuadPart - g_sliderAnim.startTick.QuadPart) * 1000.0 / g_qpcFreq.QuadPart);
                float t = std::clamp(elapsed / g_sliderAnim.durationMs, 0.0f, 1.0f);
                float eased = (g_sliderAnim.target > g_sliderAnim.start)
                    ? (1.0f - std::pow(1.0f - t, 3.0f))
                    : (t * t * (3.0f - 2.0f * t));
                g_sliderAnim.current = g_sliderAnim.start + (g_sliderAnim.target - g_sliderAnim.start) * eased;
                if (g_sliderWnd) RenderSliderLayered(g_sliderWnd);
                if (t >= 1.0f) {
                    g_sliderAnim.current = g_sliderAnim.target;
                    g_sliderAnim.active = false;
                    KillTimer(hwnd, TIMER_ANIMATION);
                    if (g_sliderAnim.current <= 0.01f && g_sliderWnd) {
                        ShowWindow(g_sliderWnd, SW_HIDE);
                    }
                }
            } else if (wParam == TIMER_HOVER_CHECK) {
                POINT pt{};
                GetCursorPos(&pt);
                RECT rBadge{}, rSlider{}, rUnion{};
                if (g_badgeWnd) GetWindowRect(g_badgeWnd, &rBadge);
                if (g_sliderWnd) GetWindowRect(g_sliderWnd, &rSlider);
                UnionRect(&rUnion, &rBadge, &rSlider);
                InflateRect(&rUnion, 8, 8);
                if (!PtInRect(&rUnion, pt) && !g_sliderDragging) {
                    g_badgeHovered = false;
                    g_sliderHovered = false;
                    if (g_badgeWnd) RenderBadgeLayered(g_badgeWnd);
                    StartSliderAnimation(0.0f);
                    KillTimer(hwnd, TIMER_HOVER_CHECK);
                }
            } else if (wParam == TIMER_SAVE_PRESETS) {
                KillTimer(hwnd, TIMER_SAVE_PRESETS);
                SavePresetsToStorage();
            }
            return 0;
        }
        case WM_WTSSESSION_CHANGE: {
            if (g_settings.resetOnSessionChange &&
                (wParam == WTS_SESSION_LOCK || wParam == WTS_SESSION_UNLOCK || wParam == WTS_SESSION_LOGOFF)) {
                RestoreAllWindows();
            }
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// -----------------------------------------------------------------------------
// Hooks (Fast Pre-Filtered WH_MOUSE_LL + Out-of-Context WinEvent)
// -----------------------------------------------------------------------------

bool MatchesButtonMsg(ButtonAction act, UINT msg, WPARAM mouseData) {
    switch (act) {
        case ButtonAction::Middle: return msg == WM_MBUTTONDOWN;
        case ButtonAction::Right:  return msg == WM_RBUTTONDOWN;
        case ButtonAction::X1:     return msg == WM_XBUTTONDOWN && HIWORD(mouseData) == XBUTTON1;
        case ButtonAction::X2:     return msg == WM_XBUTTONDOWN && HIWORD(mouseData) == XBUTTON2;
        default:                   return false;
    }
}

// Thread-safe target resolution for WH_MOUSE_LL without reading std::map<HWND, WindowState>.
bool IsHookIgnoredWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return true;
    if (hwnd == g_ctrlWnd || hwnd == g_badgeWnd || hwnd == g_sliderWnd) return true;

    wchar_t cls[128]{};
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) > 0) {
        if (wcscmp(cls, kDimClass) == 0 ||
            wcscmp(cls, kBadgeClass) == 0 ||
            wcscmp(cls, kSliderClass) == 0 ||
            wcscmp(cls, L"Shell_TrayWnd") == 0 ||
            wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
            wcscmp(cls, L"Progman") == 0 ||
            wcscmp(cls, L"WorkerW") == 0 ||
            wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
            return true;
        }
    }
    return false;
}

// Purely non-blocking geometry check for WH_MOUSE_LL (zero SendMessageTimeout and zero std::map access in hook).
HWND FastTopBandTargetFromHook(POINT pt) {
    if (IsFullscreenActive()) return nullptr;
    HWND raw = WindowFromPoint(pt);
    if (!raw) return nullptr;
    HWND target = GetAncestor(raw, GA_ROOT);
    if (!target || IsHookIgnoredWindow(target)) return nullptr;

    // Never intercept wheel or clicks on popup widgets/OSDs (such as BenQ Eye-CareU,
    // Display Pilot, Twinkle Tray, or system flyouts) that lack a standard caption bar.
    LONG_PTR style = GetWindowLongPtrW(target, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(target, GWL_EXSTYLE);
    if ((style & WS_CAPTION) == 0 || (exStyle & WS_EX_NOACTIVATE) != 0) {
        return nullptr;
    }

    RECT rc = GetVisualWindowRect(target);
    UINT dpi = GetDpiForWindow(target);
    int captionH = GetSystemMetricsForDpi(SM_CYCAPTION, dpi ? dpi : 96) +
                   GetSystemMetricsForDpi(SM_CYFRAME, dpi ? dpi : 96) +
                   MulDiv(8, dpi ? dpi : 96, 96);
    if (pt.x < rc.left || pt.x > rc.right || pt.y < rc.top || pt.y > rc.top + captionH) {
        return nullptr;
    }
    // Exclude right-most native caption button area (Min/Max/Close) so clicks there are never swallowed.
    if (pt.x > rc.right - MulDiv(138, dpi ? dpi : 96, 96)) {
        return nullptr;
    }
    return target;
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0 || g_unloading.load(std::memory_order_relaxed) || !g_ctrlWnd) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    const auto* info = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
    UINT msg = static_cast<UINT>(wParam);

    ModCombo opacityMod = static_cast<ModCombo>(g_hookOpacityMod.load(std::memory_order_relaxed));
    ButtonAction pinBtn = static_cast<ButtonAction>(g_hookPinButton.load(std::memory_order_relaxed));
    ModCombo dimMod = static_cast<ModCombo>(g_hookDimMod.load(std::memory_order_relaxed));
    ButtonAction dimToggle = static_cast<ButtonAction>(g_hookDimToggle.load(std::memory_order_relaxed));

    if (msg == WM_MOUSEWHEEL) {
        bool opDown = IsModifierComboDown(opacityMod);
        bool dimDown = IsModifierComboDown(dimMod);
        if (opDown || dimDown) {
            if (HWND target = FastTopBandTargetFromHook(info->pt)) {
                SHORT delta = static_cast<SHORT>(HIWORD(info->mouseData));
                WPARAM wp = MAKEWPARAM(opDown ? 1 : 0, static_cast<WORD>(delta));
                PostMessageW(g_ctrlWnd, WM_APP_HOOK_WHEEL, wp, reinterpret_cast<LPARAM>(target));
                return 1;
            }
        }
    } else if (msg == WM_LBUTTONDOWN && dimToggle == ButtonAction::DoubleLeft &&
               IsModifierComboDown(dimMod)) {
        if (HWND target = FastTopBandTargetFromHook(info->pt)) {
            ULONGLONG now = GetTickCount64();
            int dx = std::abs(info->pt.x - g_lastCtrlDownPt.x);
            int dy = std::abs(info->pt.y - g_lastCtrlDownPt.y);
            if (now - g_lastCtrlDownTick <= GetDoubleClickTime() &&
                dx <= GetSystemMetrics(SM_CXDOUBLECLK) && dy <= GetSystemMetrics(SM_CYDOUBLECLK)) {
                g_lastCtrlDownTick = 0;
                g_swallowNextLeftUp = true;
                PostMessageW(g_ctrlWnd, WM_APP_HOOK_DBLCLICK, 0, reinterpret_cast<LPARAM>(target));
                return 1;
            }
            g_lastCtrlDownTick = now;
            g_lastCtrlDownPt = info->pt;
        }
    } else if (msg == WM_LBUTTONUP && g_swallowNextLeftUp) {
        g_swallowNextLeftUp = false;
        return 1;
    } else if (msg == WM_MBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_XBUTTONDOWN) {
        if (MatchesButtonMsg(pinBtn, msg, info->mouseData) &&
            IsModifierComboDown(opacityMod)) {
            if (HWND target = FastTopBandTargetFromHook(info->pt)) {
                PostMessageW(g_ctrlWnd, WM_APP_HOOK_BUTTON, 0, reinterpret_cast<LPARAM>(target));
                return 1;
            }
        }
        if (MatchesButtonMsg(dimToggle, msg, info->mouseData) &&
            IsModifierComboDown(dimMod)) {
            if (HWND target = FastTopBandTargetFromHook(info->pt)) {
                PostMessageW(g_ctrlWnd, WM_APP_HOOK_BUTTON, 1, reinterpret_cast<LPARAM>(target));
                return 1;
            }
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

bool IsTransientPopupOrMenu(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return true;
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if ((ex & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) != 0 && (style & WS_CAPTION) == 0) {
        return true;
    }
    wchar_t cls[128]{};
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) > 0) {
        if (wcscmp(cls, L"#32768") == 0 ||
            wcscmp(cls, L"tooltips_class32") == 0 ||
            wcscmp(cls, L"XamlExplorerHostIslandWindow") == 0 ||
            wcscmp(cls, L"TopLevelWindowForOverflowXamlIsland") == 0 ||
            wcscmp(cls, L"MultitaskingViewFrame") == 0) {
            return true;
        }
    }
    return false;
}

void SyncAllWindowsForCaptureAndFocus(HWND activeFg, bool forceZOrder) {
    bool captureNow = IsAnyCaptureOverlayVisible();
    bool captureChanged = (captureNow != g_captureSessionActive);
    g_captureSessionActive = captureNow;

    if (captureNow) {
        if (g_badgeWnd && IsWindowVisible(g_badgeWnd)) ShowWindow(g_badgeWnd, SW_HIDE);
        if (g_sliderWnd && IsWindowVisible(g_sliderWnd)) ShowWindow(g_sliderWnd, SW_HIDE);
    }

    for (auto& [tHwnd, st] : g_ledger) {
        if (!tHwnd || !IsWindow(tHwnd)) continue;
        if (st.opacityPct < 100 || captureChanged ||
            (g_settings.keepTextCrisp && g_settings.crispMode == CrispTextMode::HoverFocusCrisp)) {
            EnsureTargetOpacityApplied(st);
        }
        if (ShouldShowDimOverlay(st)) {
            if (!st.overlayHwnd || !IsWindow(st.overlayHwnd)) {
                SyncDimOverlay(st);
            } else {
                BYTE alpha = ComputeOverlayAlpha(st);
                SetLayeredWindowAttributes(st.overlayHwnd, 0, alpha, LWA_ALPHA);
                bool needForceZ = forceZOrder || captureChanged || (tHwnd == activeFg) || !g_settings.persistWhenUnfocused;
                PositionOverlayAboveTarget(st, needForceZ);
            }
        }
    }

    if (!captureNow && g_settings.headerControlEnabled) {
        HWND effFg = GetEffectiveForegroundTarget();
        if (effFg && !IsProtectedWindow(effFg) && !IsMonitorControlWindow(effFg)) {
            g_headerTarget = effFg;
            PositionHeaderControls(effFg, true);
        } else {
            g_headerTarget = nullptr;
            if (g_badgeWnd) ShowWindow(g_badgeWnd, SW_HIDE);
            if (g_sliderWnd) ShowWindow(g_sliderWnd, SW_HIDE);
        }
    }
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject,
                           LONG idChild, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !hwnd) return;
    if (IsOwnOverlayWindow(hwnd)) return;

    if (event == EVENT_OBJECT_DESTROY) {
        if (hwnd == g_lastNormalForeground) {
            g_lastNormalForeground = nullptr;
        }
        auto it = g_ledger.find(hwnd);
        if (it != g_ledger.end()) {
            if (it->second.overlayHwnd && IsWindow(it->second.overlayHwnd)) {
                DestroyWindow(it->second.overlayHwnd);
            }
            it->second.overlayHwnd = nullptr;
            if (g_settings.resetOnWindowClose) {
                g_ledger.erase(it);
            }
        }
        if (hwnd == g_headerTarget) {
            g_headerTarget = nullptr;
            g_cachedHeaderHwnd = nullptr;
            if (g_badgeWnd) ShowWindow(g_badgeWnd, SW_HIDE);
            if (g_sliderWnd) ShowWindow(g_sliderWnd, SW_HIDE);
        }
        // If a capture overlay (e.g. ScreenClippingHost) just closed without firing EVENT_SYSTEM_FOREGROUND,
        // immediately restore all dimmed/translucent windows without flicker or delay.
        if (g_captureSessionActive || IsCaptureToolWindow(hwnd)) {
            SyncAllWindowsForCaptureAndFocus(GetEffectiveForegroundTarget(), false);
        }
        return;
    }

    if (event == EVENT_SYSTEM_MOVESIZESTART) {
        auto it = g_ledger.find(hwnd);
        if (it != g_ledger.end()) {
            it->second.isMoveSizing = true;
            if (it->second.overlayHwnd && IsWindow(it->second.overlayHwnd)) {
                PositionOverlayAboveTarget(it->second, false);
            }
        }
        return;
    }

    if (event == EVENT_SYSTEM_MOVESIZEEND) {
        auto it = g_ledger.find(hwnd);
        if (it != g_ledger.end()) {
            it->second.isMoveSizing = false;
            if (it->second.overlayHwnd && IsWindow(it->second.overlayHwnd)) {
                PositionOverlayAboveTarget(it->second, true);
            }
        }
        if (g_settings.headerControlEnabled && hwnd == g_headerTarget) {
            PositionHeaderControls(hwnd, true);
        }
        return;
    }

    if (event == EVENT_SYSTEM_FOREGROUND || event == EVENT_SYSTEM_CAPTURESTART || event == EVENT_SYSTEM_CAPTUREEND) {
        if (event == EVENT_SYSTEM_FOREGROUND && IsTransientPopupOrMenu(hwnd) && !IsCaptureToolWindow(hwnd)) {
            return;
        }
        if (event == EVENT_SYSTEM_FOREGROUND && !IsProtectedWindow(hwnd) &&
            !IsMonitorControlWindow(hwnd) && !IsCaptureToolWindow(hwnd)) {
            g_lastNormalForeground = hwnd;
        }
        SyncAllWindowsForCaptureAndFocus(GetEffectiveForegroundTarget(), false);
        return;
    }

    if (event == EVENT_OBJECT_NAMECHANGE) {
        if (g_settings.headerControlEnabled && hwnd == g_headerTarget && g_settings.headerAfterTitle) {
            PositionHeaderControls(hwnd, true);
        }
        return;
    }

    if (event == EVENT_OBJECT_LOCATIONCHANGE ||
        event == EVENT_SYSTEM_MINIMIZESTART || event == EVENT_SYSTEM_MINIMIZEEND ||
        event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_HIDE ||
        event == EVENT_OBJECT_CLOAKED || event == EVENT_OBJECT_UNCLOAKED) {
        if ((event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_HIDE ||
             event == EVENT_OBJECT_CLOAKED || event == EVENT_OBJECT_UNCLOAKED) &&
            (g_captureSessionActive || IsCaptureToolWindow(hwnd))) {
            SyncAllWindowsForCaptureAndFocus(GetEffectiveForegroundTarget(), false);
            return;
        }

        auto it = g_ledger.find(hwnd);
        if (it != g_ledger.end()) {
            if (it->second.opacityPct < 100 &&
                (event == EVENT_SYSTEM_MINIMIZEEND || event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_UNCLOAKED)) {
                EnsureTargetOpacityApplied(it->second);
            }
            if (ShouldShowDimOverlay(it->second) && it->second.overlayHwnd) {
                bool forceZ = (event != EVENT_OBJECT_LOCATIONCHANGE);
                PositionOverlayAboveTarget(it->second, forceZ);
            }
        }
    }

    if (g_settings.headerControlEnabled && hwnd == g_headerTarget &&
        (event == EVENT_OBJECT_LOCATIONCHANGE ||
         event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_HIDE ||
         event == EVENT_OBJECT_CLOAKED || event == EVENT_OBJECT_UNCLOAKED)) {
        static RECT s_lastHeaderTargetRect{};
        static HWND s_lastHeaderHwnd = nullptr;
        RECT curRc = GetVisualWindowRect(hwnd);
        bool queryIPC = (event != EVENT_OBJECT_LOCATIONCHANGE) || (hwnd != s_lastHeaderHwnd);
        if (queryIPC || !EqualRect(&curRc, &s_lastHeaderTargetRect)) {
            s_lastHeaderHwnd = hwnd;
            s_lastHeaderTargetRect = curRc;
            PositionHeaderControls(hwnd, queryIPC);
        }
    }
}

// -----------------------------------------------------------------------------
// Worker Thread & Tool Mod Entry Points
// -----------------------------------------------------------------------------

DWORD WINAPI WorkerThreadProc(LPVOID) {
    QueryPerformanceFrequency(&g_qpcFreq);
    LoadSettings();
    LoadPresetsFromStorage();

    Gdiplus::GdiplusStartupInput gdiInput{};
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiInput, nullptr);

    HINSTANCE hInst = GetModuleHandleW(nullptr);
    WNDCLASSEXW wc{sizeof(wc)};
    wc.hInstance = hInst;
    wc.lpfnWndProc = ControlWndProc;
    wc.lpszClassName = kCtrlClass;
    RegisterClassExW(&wc);

    wc.lpfnWndProc = DimOverlayWndProc;
    wc.lpszClassName = kDimClass;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    RegisterClassExW(&wc);
    wc.style = 0;

    wc.lpfnWndProc = BadgeWndProc;
    wc.lpszClassName = kBadgeClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_HAND);
    RegisterClassExW(&wc);

    wc.lpfnWndProc = SliderWndProc;
    wc.lpszClassName = kSliderClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_HAND);
    RegisterClassExW(&wc);

    MSG msg{};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_ctrlWnd = CreateWindowExW(0, kCtrlClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, hInst, nullptr);
    g_badgeWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
        kBadgeClass, L"", WS_POPUP, 0, 0, 30, 30, nullptr, nullptr, hInst, nullptr);
    g_sliderWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
        kSliderClass, L"", WS_POPUP, 0, 0, 176, 30, nullptr, nullptr, hInst, nullptr);

    DWORD initAff = GetHeaderControlAffinity();
    if (g_badgeWnd) SetWindowDisplayAffinity(g_badgeWnd, initAff);
    if (g_sliderWnd) SetWindowDisplayAffinity(g_sliderWnd, initAff);
    if (g_ctrlWnd) WTSRegisterSessionNotification(g_ctrlWnd, NOTIFY_FOR_THIS_SESSION);

    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, nullptr, 0);
    g_winEventHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_OBJECT_UNCLOAKED,
        nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    if (g_settings.headerControlEnabled) {
        HWND fg = GetForegroundWindow();
        if (fg && !IsProtectedWindow(fg)) {
            g_headerTarget = fg;
            PositionHeaderControls(fg);
        }
    }

    if (g_readyEvent) SetEvent(g_readyEvent);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_mouseHook) { UnhookWindowsHookEx(g_mouseHook); g_mouseHook = nullptr; }
    if (g_winEventHook) { UnhookWinEvent(g_winEventHook); g_winEventHook = nullptr; }
    if (g_ctrlWnd) WTSUnRegisterSessionNotification(g_ctrlWnd);

    SavePresetsToStorage();
    RestoreAllWindows();
    g_badgeDib.Release();
    g_sliderDib.Release();
    if (g_dimBrush) { DeleteObject(g_dimBrush); g_dimBrush = nullptr; }

    if (g_sliderWnd) { DestroyWindow(g_sliderWnd); g_sliderWnd = nullptr; }
    if (g_badgeWnd) { DestroyWindow(g_badgeWnd); g_badgeWnd = nullptr; }
    if (g_ctrlWnd) { DestroyWindow(g_ctrlWnd); g_ctrlWnd = nullptr; }

    UnregisterClassW(kSliderClass, hInst);
    UnregisterClassW(kBadgeClass, hInst);
    UnregisterClassW(kDimClass, hInst);
    UnregisterClassW(kCtrlClass, hInst);

    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
    return 0;
}

} // namespace

BOOL WhTool_ModInit() {
    g_unloading.store(false);

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, &g_workerThreadId);
    if (!g_workerThread) {
        if (g_readyEvent) { CloseHandle(g_readyEvent); g_readyEvent = nullptr; }
        return FALSE;
    }
    if (g_readyEvent) {
        WaitForSingleObject(g_readyEvent, 3000);
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_ctrlWnd && g_workerThreadId) {
        PostMessageW(g_ctrlWnd, WM_APP_RELOAD_SETTINGS, 0, 0);
    }
}

void WhTool_ModUninit() {
    g_unloading.store(true);
    if (g_workerThreadId) {
        PostThreadMessageW(g_workerThreadId, WM_QUIT, 0, 0);
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 5000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        g_workerThreadId = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0) {
            isService = true;
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

    if (isService) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutexW(nullptr, FALSE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        DWORD waitRes = WaitForSingleObject(g_toolModProcessMutex, 5000);
        if (waitRes != WAIT_OBJECT_0 && waitRes != WAIT_ABANDONED) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            CloseHandle(g_toolModProcessMutex);
            g_toolModProcessMutex = nullptr;
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandleW(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        void* entryPoint =
            (BYTE*)dosHeader + ntHeaders->OptionalHeader.AddressOfEntryPoint;
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

    WCHAR modulePath[MAX_PATH];
    if (!GetModuleFileNameW(nullptr, modulePath, ARRAYSIZE(modulePath))) {
        Wh_Log(L"GetModuleFileName failed");
        return;
    }

    WCHAR commandLine[MAX_PATH + 256];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", modulePath, WH_MOD_ID);

    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi;
    if (!CreateProcessW(modulePath, commandLine, nullptr, nullptr, FALSE,
                        CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
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
    if (g_toolModProcessMutex) {
        ReleaseMutex(g_toolModProcessMutex);
        CloseHandle(g_toolModProcessMutex);
        g_toolModProcessMutex = nullptr;
    }
    WhTool_ModUninit();
}

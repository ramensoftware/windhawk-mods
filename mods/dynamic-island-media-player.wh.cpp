// ==WindhawkMod==
// @id             dynamic-island-media-player
// @name           Dynamic Island Media Player
// @description    A dedicated, standalone Dynamic Island media player for Windows. Forked and optimized from 'Dynamic Island for Windows' by devcode90. Features per-app positioning, custom color morphing, multilingual UI, and optimized rendering.
// @version        1.6.0
// @author         BOG090909
// @github         https://github.com/BOG090909
// @homepage       https://github.com/BOG090909/dynamic-island-media-player
// @include        windhawk.exe
// @compilerOptions -lole32 -loleaut32 -ld2d1 -ldwrite -lgdi32 -luser32 -lshell32 -lruntimeobject -lwindowscodecs -lwinmm -lcomdlg32 -ldwmapi -lshcore
// @license        MIT
// ==/WindhawkMod==

/*
MIT License

Copyright (c) 2026 BOG090909
Copyright (c) 2023-2026 devcode90 (Himanshu)

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
# Dynamic Island Media Player for Windows

![Dynamic Island Preview](https://raw.githubusercontent.com/BOG090909/dynamic-island-media-player/main/assets/preview.png)

A lightweight, high-performance, standalone Dynamic Island media player built specifically for audio and video management on Windows 10/11. 

This mod is a specialized, lightweight adaptation of the incredible **[Dynamic Island for Windows](https://github.com/devcode90/Dynamic-Island-for-Windows)**. It strips away the extra dashboard modules (weather, hardware monitor, etc.) to focus purely on delivering the ultimate media player experience with zero bloat.

## Key Features
- **Context Menu Priority:** The right-click context menu acts as the primary configuration source. Settings changed here persistently override default Windhawk configurations.
- **Smart Media Tracking:** Real-time track metadata, album art, app badges, and progress scrubbing via Windows System Media Transport.
- **Optimized Rendering:** Hybrid event-driven architecture with intelligent sleep throttling guarantees ultra-low CPU/GPU consumption.
- **Per-App Dynamic Screen Positioning:** Supports saving unique screen positions on a per-application basis.
- **Trilingual Localized UI (UA / RU / EN):** Full native support for Ukrainian, Russian, and English.

## Credits & Acknowledgements
- **Himanshu (devcode90)** - For the original Dynamic Island for Windows framework, rendering physics, and core UI design.
- **BOG090909** - Standalone media player adaptation, custom app-positioning logic, multilingual support, and optimization.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Appearance:
  - Position: top-center
    $name: Position
    $description: Default position (used until you drag the island somewhere).
    $options:
      - top-center: Top Center
      - top-left: Top Left
      - top-right: Top Right
      - bottom-center: Bottom Center
      - bottom-left: Bottom Left
      - bottom-right: Bottom Right
  - TargetMonitor: primary
    $name: Target Monitor
    $options:
      - 'primary': Primary Monitor
      - '1': Display 1
      - '2': Display 2
      - '3': Display 3
      - '4': Display 4
  - OffsetX: 0
    $name: Offset X
  - OffsetY: 0
    $name: Offset Y
  - ShapeStyle: default
    $name: Island Shape Style
    $options:
      - default: Default (Apple Pill)
      - w11: Windows 11 (Rounded Box)
      - notch: macOS Notch (Top Edge Flush)
  - SizeScale: '1.0'
    $name: Size scale
    $options:
      - '0.8': 0.8x
      - '1.0': 1.0x
      - '1.2': 1.2x
      - '1.5': 1.5x
      - '2.0': 2.0x
  - AutoDpiScale: true
    $name: Auto DPI scaling
  $name: Appearance & Position
- Behavior:
  - ExpandOnHover: true
    $name: Expand on hover
    $description: If disabled, click to expand.
  - HideInFullscreen: true
    $name: Hide in browser/video fullscreen
    $description: Context Menu overrides this setting.
  - IdleTimeout: '0'
    $name: Hide when paused/idle
    $description: Context Menu overrides this setting.
    $options:
      - '0': Never
      - '30': After 30 seconds
      - '60': After 1 minute
      - '300': After 5 minutes
      - '900': After 15 minutes
      - '1800': After 30 minutes
      - '3600': After 1 hour
      - '7200': After 2 hours
  $name: Behavior
- Animations:
  - TargetFPS: auto
    $name: Refresh rate / FPS
    $options:
      - auto: Auto
      - '60': 60 FPS
      - '120': 120 FPS
      - '144': 144 FPS
      - '240': 240 FPS
  - AnimationStyle: default
    $name: Animation style
    $options:
      - smooth: Smooth
      - default: Default
      - bouncy: Bouncy
      - snappy: Snappy
  - AnimationSpeed: normal
    $name: Animation speed
    $options:
      - slow: Slow (0.75x)
      - normal: Normal (1.0x)
      - fast: Fast (1.35x)
      - ultra-fast: Ultra Fast (2.0x)
  $name: Animations
- Themes:
  - ThemePreset: obsidian
    $name: Theme preset
    $description: "Note: Color settings changed via the right-click Context Menu take priority."
    $options:
      - obsidian: Obsidian
      - graphite: Graphite
      - slate: Slate
      - nord: Nord
      - evergreen: Evergreen
      - espresso: Espresso
      - plum: Plum
      - porcelain: Porcelain (light)
  - PillOpacity: 96
    $name: Pill transparency
    $description: 35 to 100. Context Menu overrides this.
  - AccentColorMode: auto
    $name: Accent color mode
    $description: Context Menu overrides this setting.
    $options:
      - auto: Auto, from album art
      - system: System accent
      - custom: Custom hex
  - SmoothAccent: true
    $name: Smooth accent color transition
  - CustomAccentHex: "#4cc9f0"
    $name: Custom accent hex
  - FontFamily: ""
    $name: Font family
  - TextScale: 100
    $name: Text size
    $description: 70 to 160.
  $name: Colors & Theming
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <unknwn.h>
#include <shellapi.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <objbase.h>
#include <commdlg.h>
#include <dwmapi.h>
#include <shellscalingapi.h>
#include <wrl/client.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cwchar>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

using Microsoft::WRL::ComPtr;

namespace {

constexpr wchar_t kWindowClass[] = L"Windhawk.DynamicIslandMediaPlayer";
constexpr UINT WM_APP_NEW_EVENT = WM_APP + 0x443;
constexpr UINT WM_APP_WAKE = WM_APP + 0x446;
constexpr UINT WM_APP_APPLY_HOTKEY = WM_APP + 0x447;

enum HotkeyId : int {
    kHotkeyHideShow = 1,
    kHotkeyNextTrack = 2,
    kHotkeyPrevTrack = 3,
    kHotkeyPlayPause = 4,
};

constexpr float kRenderPadX = 28.0f;
constexpr float kRenderPadY = 22.0f;

namespace MediaLayout {
    constexpr float kExpandedWidth = 380.0f;
    constexpr float kExpandedHeight = 184.0f;
    constexpr float kCollapsedWidth = 160.0f;
    constexpr float kCollapsedHeight = 44.0f;

    constexpr float kScrubberY = 114.0f;
    constexpr float kScrubMargin = 24.0f;
    constexpr float kScrubBarLeftInset = 48.0f;
    constexpr float kScrubBarRightInset = 48.0f;
    constexpr float kScrubHitHalfHeight = 14.0f;
    constexpr float kScrubHitPadX = 6.0f;
    constexpr float kScrubInsetLeft = kScrubMargin + kScrubBarLeftInset;
    constexpr float kScrubInsetRight = kScrubMargin + kScrubBarRightInset;

    constexpr float kControlsY = 148.0f;
    constexpr float kControlSpacing = 64.0f;
    constexpr float kNavButtonRadius = 16.0f;
    constexpr float kPlayButtonRadius = 22.0f;
    constexpr float kControlHitPad = 6.0f;

    constexpr float kArtInsetX = 24.0f;
    constexpr float kArtInsetY = 20.0f;
    constexpr float kArtSize = 64.0f;

    constexpr float kGearInsetRight = 22.0f;
    constexpr float kGearInsetTop = 20.0f;
    constexpr float kGearRadius = 11.0f;
    constexpr float kGearHitRadius = 16.0f;

    constexpr float kExpandedMinHeight = 60.0f;
}

std::atomic<unsigned> g_mediaHitSeq{0};
std::atomic<float> g_mediaHitLeft{0.0f};
std::atomic<float> g_mediaHitTop{0.0f};
std::atomic<float> g_mediaHitRight{0.0f};
std::atomic<float> g_mediaHitBottom{0.0f};
std::atomic<float> g_mediaHitScale{1.0f};
std::atomic<unsigned long long> g_mediaHitStamp{0};

struct MediaContentPoint {
    bool valid = false;
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
};

enum class Position {
    TopCenter, TopLeft, TopRight, BottomCenter, BottomLeft, BottomRight,
};

constexpr bool IsBottomPosition(Position p) {
    return p == Position::BottomCenter || p == Position::BottomLeft || p == Position::BottomRight;
}

enum class AccentMode { Auto, System, Custom };
enum class AnimationStyle { Smooth, Default, Bouncy, Snappy };
enum class ButtonStyle { Default, Cubic };

enum class LanguageSetting {
    System = 0,
    Ukrainian = 1,
    Russian = 2,
    English = 3
};

std::atomic<LanguageSetting> g_language{LanguageSetting::System};

const wchar_t* Tr(const wchar_t* uk, const wchar_t* ru, const wchar_t* en) {
    LanguageSetting lang = g_language.load();
    if (lang == LanguageSetting::System) {
        LANGID sysLang = PRIMARYLANGID(GetUserDefaultUILanguage());
        if (sysLang == LANG_UKRAINIAN) return uk;
        if (sysLang == LANG_RUSSIAN) return ru;
        return en;
    }
    switch (lang) {
        case LanguageSetting::Ukrainian: return uk;
        case LanguageSetting::Russian:   return ru;
        case LanguageSetting::English:   return en;
        default:                         return en;
    }
}

struct ThemePalette {
    const wchar_t* id;
    const wchar_t* bg;
    const wchar_t* fg;
    const wchar_t* sec;
    const wchar_t* border;
};

static constexpr ThemePalette kThemePalettes[] = {
    {L"obsidian",  L"#08080A", L"#FFFFFF", L"#9B9BA5", L"#1E1E22"},
    {L"graphite",  L"#1C1C1E", L"#FFFFFF", L"#A8A8AE", L"#323236"},
    {L"slate",     L"#111721", L"#E9EEF6", L"#92A2B8", L"#232C3A"},
    {L"nord",      L"#2E3440", L"#ECEFF4", L"#A7B0C0", L"#3B4252"},
    {L"evergreen", L"#0C1512", L"#E4F1EA", L"#8CAE9D", L"#1A2A23"},
    {L"espresso",  L"#1A1512", L"#F6EEE7", L"#B7A395", L"#2E2420"},
    {L"plum",      L"#16111C", L"#F1EAF6", L"#AC9BB9", L"#281F33"},
    {L"porcelain", L"#F5F6F8", L"#14161A", L"#5B6270", L"#D8DBE1"},
};

struct Settings {
    Position position = Position::TopCenter;
    int targetMonitor = 0;
    int offsetX = 0;
    int offsetY = 0;
    float sizeScale = 1.0f;
    float baseSizeScale = 1.0f;
    std::wstring fontFamily;
    AccentMode accentMode = AccentMode::Auto;
    bool smoothAccent = true;
    D2D1_COLOR_F customAccent = D2D1::ColorF(0x4cc9f0);
    int targetFps = 0;
    AnimationStyle animationStyle = AnimationStyle::Default;
    float animationSpeed = 1.0f;
    bool expandOnHover = true;
    bool hideInFullscreen = true;
    int idleTimeoutSec = 0;
    bool w11Style = false;
    bool notchStyle = false;
    D2D1_COLOR_F pillBgColor = D2D1::ColorF(0.031f, 0.031f, 0.039f, 1.0f);
    D2D1_COLOR_F textPrimaryColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
    D2D1_COLOR_F textSecondaryColor = D2D1::ColorF(0.608f, 0.608f, 0.647f, 1.0f);
    D2D1_COLOR_F contourBorderColor = D2D1::ColorF(0.118f, 0.118f, 0.133f, 1.0f);
    float pillOpacity = 0.96f;
    float textScale = 1.0f;
    ButtonStyle buttonStyle = ButtonStyle::Default;
};

struct BitmapPixels {
    std::vector<uint8_t> bgra;
    UINT width = 0;
    UINT height = 0;
    uint64_t generation = 0;
    D2D1_COLOR_F sampledAccent = D2D1::ColorF(0x4cc9f0);
};

struct MediaSnapshot {
    bool available = false;
    bool playing = false;
    std::wstring title;
    std::wstring artist;
    std::wstring albumTitle;
    std::wstring sourceAppUserModelId;
    std::wstring sourceName;
    std::wstring sourceBadge;
    std::wstring trackId; 
    
    winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession activeSession{nullptr};

    std::shared_ptr<const BitmapPixels> art;
    std::shared_ptr<const BitmapPixels> sourceIcon;
    double titleChangedAt = 0.0;
    double metaChangedAt = 0.0;
    double pausedAt = 0.0;
    int64_t positionTicks = 0;
    int64_t endTicks = 0;
    ULONGLONG lastUpdatedTicks = 0;
};

struct SharedState {
    MediaSnapshot media;
};

struct SpringValue {
    float value = 0.0f;
    float velocity = 0.0f;
    float target = 0.0f;

    void Reset(float v) { value = target = v; velocity = 0.0f; }

    void Step(float totalDt, float stiffness, float damping) {
        const float kFixedDt = 0.0005f;
        while (totalDt > 0.0f) {
            float dt = std::min(totalDt, kFixedDt);
            const float displacement = value - target;
            const float acceleration = -stiffness * displacement - damping * velocity;
            velocity += acceleration * dt;
            value += velocity * dt;
            totalDt -= dt;
        }
        if (std::fabs(value - target) < 0.01f && std::fabs(velocity) < 0.01f) {
            value = target;
            velocity = 0.0f;
        }
    }
};

struct AppPosition {
    int centerX = 0;
    int top = 0;
};

Settings g_settings;
std::mutex g_settingsMutex;

Settings GetSettingsCopy() {
    std::lock_guard lock(g_settingsMutex);
    return g_settings;
}

std::mutex g_stateMutex;
SharedState g_state;
std::atomic<uint64_t> g_artGenerationCounter{0};

std::atomic<HWND> g_hwnd{nullptr};
HANDLE g_stopEvent = nullptr;
HANDLE g_renderThread = nullptr;
HANDLE g_mediaThread = nullptr;

std::atomic<bool> g_layoutDirty{true};
std::atomic<bool> g_clickExpanded{false};
std::atomic<int> g_pressedMediaButton{-1};
std::atomic<int> g_hoveredMediaButton{-1};
std::atomic<bool> g_hoveredGear{false};
std::atomic<bool> g_scrubbing{false};
std::atomic<float> g_scrubDragFraction{0.0f};
std::atomic<double> g_lastLiveSeekTime{0.0};
std::atomic<double> g_lastNudgeTime{0.0};

std::atomic<bool> g_alwaysOnTop{true};
std::atomic<bool> g_overGames{true};
std::atomic<bool> g_hideInFullscreen{true};
std::atomic<bool> g_overTaskbar{true};
std::atomic<bool> g_autoExpand{false};
std::atomic<bool> g_lockExpanded{false};
std::atomic<int>  g_menuOpacityOverride{-1};
std::atomic<int>  g_idleTimeoutSec{0};
std::atomic<bool> g_moveMode{false};
std::atomic<bool> g_dragging{false};

std::atomic<bool> g_customPosSet{false};
std::atomic<int> g_customCenterX{0};
std::atomic<int> g_customTop{0};

std::unordered_map<std::wstring, AppPosition> g_appPositions;
std::mutex g_appPosMutex;
std::wstring g_lastTargetAppExe;

std::atomic<bool> g_confirmTrackChange{false};
std::atomic<double> g_lastPrevClickTime{0.0};
std::atomic<double> g_lastNextClickTime{0.0};

std::atomic<bool> g_colorRainbowEnabled{false};
std::atomic<float> g_colorRainbowSpeed{1.0f};
std::vector<D2D1_COLOR_F> g_rainbowPalette = {
    D2D1::ColorF(0x4cc9f0), D2D1::ColorF(0x7209b7), D2D1::ColorF(0xf72585)
};
std::mutex g_rainbowMutex;

std::atomic<bool> g_flushTop{false};
std::atomic<bool> g_clickThrough{false};
std::atomic<bool> g_hidden{false};

std::atomic<bool> g_disableAnimations{false};
std::atomic<bool> g_disableShadow{false};

std::atomic<bool> g_hotkeyEnabled{false};
std::atomic<unsigned> g_hotkeyMods{MOD_CONTROL};
std::atomic<unsigned> g_hotkeyVk{'H'};
std::atomic<unsigned> g_hkNextMods{MOD_CONTROL};
std::atomic<unsigned> g_hkNextVk{'N'};
std::atomic<unsigned> g_hkPrevMods{MOD_CONTROL};
std::atomic<unsigned> g_hkPrevVk{'B'};
std::atomic<unsigned> g_hkPlayPauseMods{MOD_CONTROL};
std::atomic<unsigned> g_hkPlayPauseVk{'M'};

std::atomic<int> g_captureHotkeyTarget{0};
std::atomic<bool> g_captureHotkey{false};
std::atomic<double> g_captureDeadline{0.0};
HHOOK g_captureHook = nullptr;

bool ProcessImageNameForWindow(HWND hwnd, std::wstring* imageName);
std::wstring BaseNameFromPath(std::wstring path);
std::wstring ToLowerCopy(std::wstring value);

double NowSeconds() {
    using clock = std::chrono::steady_clock;
    static const auto start = clock::now();
    return std::chrono::duration<double>(clock::now() - start).count();
}

float Clamp(float v, float lo, float hi) { return std::max(lo, std::min(hi, v)); }
int ClampInt(int v, int lo, int hi) { return std::max(lo, std::min(hi, v)); }

bool EqualsNoCase(std::wstring_view a, std::wstring_view b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (towlower(a[i]) != towlower(b[i])) return false;
    }
    return true;
}

std::wstring GetStringSettingCopy(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

std::wstring ToLowerCopy(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](wchar_t ch) { return static_cast<wchar_t>(towlower(ch)); });
    return value;
}

std::wstring BaseNameFromPath(std::wstring path) {
    const size_t slash = path.find_last_of(L"\\/");
    if (slash != std::wstring::npos) path.erase(0, slash + 1);
    return path;
}

std::wstring GetForegroundExeName() {
    HWND hSelf = g_hwnd.load();
    HWND fg = GetForegroundWindow();
    
    if (!fg || fg == hSelf) {
        POINT pt;
        GetCursorPos(&pt);
        HWND hwndUnder = WindowFromPoint(pt);
        if (hwndUnder && hwndUnder != hSelf) {
            fg = GetAncestor(hwndUnder, GA_ROOT);
        }
    }

    if (!fg || fg == hSelf) return g_lastTargetAppExe;

    wchar_t className[256] = {};
    GetClassNameW(fg, className, 256);
    
    if (wcscmp(className, L"WorkerW") == 0 || wcscmp(className, L"Progman") == 0 ||
        wcscmp(className, L"Shell_TrayWnd") == 0) {
        return L"";
    }

    std::wstring exePath;
    if (ProcessImageNameForWindow(fg, &exePath)) {
        std::wstring base = ToLowerCopy(BaseNameFromPath(exePath));
        if (base != L"windhawk.exe" && base != L"explorer.exe" && !base.empty()) {
            std::lock_guard lock(g_appPosMutex);
            g_lastTargetAppExe = base;
            return base;
        }
    }
    
    return L"";
}

void SendMediaTransportCommand(int cmd);

void TriggerMediaTransportCommandWithConfirmation(int cmd) {
    if (g_confirmTrackChange.load()) {
        const double now = NowSeconds();
        constexpr double kDoubleClickWindow = 2.0;

        if (cmd == 0) {
            const double lastTime = g_lastPrevClickTime.load();
            if (now - lastTime > kDoubleClickWindow) {
                g_lastPrevClickTime.store(now);
                return; 
            }
            g_lastPrevClickTime.store(0.0);
        }
        else if (cmd == 2) {
            const double lastTime = g_lastNextClickTime.load();
            if (now - lastTime > kDoubleClickWindow) {
                g_lastNextClickTime.store(now);
                return;
            }
            g_lastNextClickTime.store(0.0);
        }
    }
    SendMediaTransportCommand(cmd);
}

bool PickColorWithDialog(HWND owner, D2D1_COLOR_F& outColor) {
    static COLORREF customColors[16] = {};
    CHOOSECOLORW cc = { sizeof(cc) };
    cc.hwndOwner = owner;
    cc.lpCustColors = customColors;
    cc.rgbResult = RGB(static_cast<BYTE>(outColor.r * 255),
                       static_cast<BYTE>(outColor.g * 255),
                       static_cast<BYTE>(outColor.b * 255));
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;

    if (ChooseColorW(&cc)) {
        outColor = D2D1::ColorF(GetRValue(cc.rgbResult) / 255.0f,
                                GetGValue(cc.rgbResult) / 255.0f,
                                GetBValue(cc.rgbResult) / 255.0f, 1.0f);
        return true;
    }
    return false;
}

void LoadAppPositionsFromRegistry() {
    std::lock_guard lock(g_appPosMutex);
    g_appPositions.clear();
    
    const int bufSize = 32768; 
    auto buf = std::make_unique<wchar_t[]>(bufSize);
    
    if (Wh_GetStringValue(L"AppPositionsData", buf.get(), bufSize)) {
        std::wstring str(buf.get());
        size_t start = 0;
        while (start < str.size()) {
            size_t end = str.find(L';', start);
            if (end == std::wstring::npos) end = str.size();
            std::wstring item = str.substr(start, end - start);
            start = end + 1;

            size_t eq = item.find(L'=');
            size_t comma = item.find(L',', eq);
            if (eq != std::wstring::npos && comma != std::wstring::npos) {
                std::wstring exe = item.substr(0, eq);
                int cx = _wtoi(item.substr(eq + 1, comma - (eq + 1)).c_str());
                int cy = _wtoi(item.substr(comma + 1).c_str());
                g_appPositions[exe] = {cx, cy};
            }
        }
    }
}

void SaveAppPositionsToRegistry() {
    std::lock_guard lock(g_appPosMutex);
    std::wstring serialized;
    for (const auto& [exe, pos] : g_appPositions) {
        serialized += exe + L"=" + std::to_wstring(pos.centerX) + L"," + std::to_wstring(pos.top) + L";";
    }
    Wh_SetStringValue(L"AppPositionsData", serialized.c_str());
}

void SaveRainbowPalette() {
    std::lock_guard lock(g_rainbowMutex);
    std::wstring s;
    for (const auto& c : g_rainbowPalette) {
        wchar_t hex[32];
        swprintf_s(hex, L"#%02X%02X%02X,", static_cast<int>(c.r * 255), static_cast<int>(c.g * 255), static_cast<int>(c.b * 255));
        s += hex;
    }
    Wh_SetStringValue(L"RainbowPalette", s.c_str());
}

D2D1_COLOR_F ColorFromHex(std::wstring text, D2D1_COLOR_F fallback) {
    const size_t first = text.find_first_not_of(L" \t\r\n");
    if (first == std::wstring::npos) return fallback;
    text = text.substr(first, text.find_last_not_of(L" \t\r\n") - first + 1);
    if (!text.empty() && text[0] == L'#') text.erase(text.begin());

    const size_t digits = text.size();
    if (digits != 3 && digits != 4 && digits != 6 && digits != 8) return fallback;
    if (text.find_first_not_of(L"0123456789abcdefABCDEF") != std::wstring::npos) return fallback;

    auto nibble = [](wchar_t c) -> int {
        if (c >= L'0' && c <= L'9') return c - L'0';
        if (c >= L'a' && c <= L'f') return c - L'a' + 10;
        return c - L'A' + 10;
    };

    int r, g, b, a = 255;
    if (digits <= 4) {
        r = nibble(text[0]) * 17;
        g = nibble(text[1]) * 17;
        b = nibble(text[2]) * 17;
        if (digits == 4) a = nibble(text[3]) * 17;
    } else {
        r = nibble(text[0]) * 16 + nibble(text[1]);
        g = nibble(text[2]) * 16 + nibble(text[3]);
        b = nibble(text[4]) * 16 + nibble(text[5]);
        if (digits == 8) a = nibble(text[6]) * 16 + nibble(text[7]);
    }
    return D2D1::ColorF(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

static float HueToRgb(float p, float q, float t) {
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;
    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 0.5f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
}

D2D1_COLOR_F HslToRgb(float h, float s, float l, float a = 1.0f) {
    h = std::fmod(h, 360.0f);
    if (h < 0.0f) h += 360.0f;
    s = Clamp(s, 0.0f, 1.0f);
    l = Clamp(l, 0.0f, 1.0f);
    if (s <= 1e-5f) return D2D1::ColorF(l, l, l, a);
    const float q = (l < 0.5f) ? (l * (1.0f + s)) : (l + s - l * s);
    const float p = 2.0f * l - q;
    const float hn = h / 360.0f;
    return D2D1::ColorF(Clamp(HueToRgb(p, q, hn + 1.0f / 3.0f), 0.0f, 1.0f),
                        Clamp(HueToRgb(p, q, hn), 0.0f, 1.0f),
                        Clamp(HueToRgb(p, q, hn - 1.0f / 3.0f), 0.0f, 1.0f), a);
}

void RgbToHsl(float r, float g, float b, float& h, float& s, float& l) {
    r = Clamp(r, 0.0f, 1.0f);
    g = Clamp(g, 0.0f, 1.0f);
    b = Clamp(b, 0.0f, 1.0f);
    const float maxVal = std::max({r, g, b});
    const float minVal = std::min({r, g, b});
    const float delta = maxVal - minVal;
    l = (maxVal + minVal) * 0.5f;
    if (delta <= 1e-5f) { h = 0.0f; s = 0.0f; return; }
    s = (l > 0.5f) ? (delta / (2.0f - maxVal - minVal)) : (delta / (maxVal + minVal));
    if (maxVal == r) h = ((g - b) / delta) + (g < b ? 6.0f : 0.0f);
    else if (maxVal == g) h = ((b - r) / delta) + 2.0f;
    else h = ((r - g) / delta) + 4.0f;
    h *= 60.0f;
    if (h < 0.0f) h += 360.0f;
    if (h >= 360.0f) h -= 360.0f;
}

double RelativeLuminance(D2D1_COLOR_F c) {
    auto toLinear = [](float channel) -> double {
        const double v = Clamp(channel, 0.0f, 1.0f);
        return (v <= 0.04045) ? (v / 12.92) : std::pow((v + 0.055) / 1.055, 2.4);
    };
    return 0.2126 * toLinear(c.r) + 0.7152 * toLinear(c.g) + 0.0722 * toLinear(c.b);
}

D2D1_COLOR_F EnsureContrastAgainstBackground(D2D1_COLOR_F candidate, D2D1_COLOR_F bgColor) {
    float h = 0.0f, s = 0.0f, l = 0.0f;
    RgbToHsl(candidate.r, candidate.g, candidate.b, h, s, l);
    if (s > 0.01f) s = std::max(s, 0.35f);

    const double bgLum = RelativeLuminance(bgColor);
    const bool onDark = bgLum < 0.45;
    l = onDark ? std::min(l, 0.85f) : std::max(l, 0.15f);
    const float step = onDark ? 0.02f : -0.02f;
    const float limit = onDark ? 0.95f : 0.06f;

    candidate = HslToRgb(h, s, l, candidate.a);
    auto contrastOf = [&](const D2D1_COLOR_F& c) {
        const double lum = RelativeLuminance(c);
        return (std::max(bgLum, lum) + 0.05) / (std::min(bgLum, lum) + 0.05);
    };
    double contrast = contrastOf(candidate);
    while (contrast < 3.0 && (onDark ? (l < limit) : (l > limit))) {
        l += step;
        candidate = HslToRgb(h, s, l, candidate.a);
        contrast = contrastOf(candidate);
    }
    return candidate;
}

bool IsForegroundFullscreen(HWND self) {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == self || fg == GetDesktopWindow() || fg == GetShellWindow()) return false;
    if (!IsWindowVisible(fg)) return false;

    wchar_t className[256] = {};
    GetClassNameW(fg, className, 256);
    if (wcscmp(className, L"WorkerW") == 0 || wcscmp(className, L"Progman") == 0 ||
        wcscmp(className, L"Shell_TrayWnd") == 0) {
        return false;
    }

    RECT clientRect = {};
    if (!GetClientRect(fg, &clientRect)) return false;
    POINT pt = {0, 0};
    ClientToScreen(fg, &pt);
    OffsetRect(&clientRect, pt.x, pt.y);

    HMONITOR mon = MonitorFromWindow(fg, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(MONITORINFO)};
    if (GetMonitorInfoW(mon, &mi)) {
        return clientRect.left <= mi.rcMonitor.left && clientRect.top <= mi.rcMonitor.top &&
               clientRect.right >= mi.rcMonitor.right && clientRect.bottom >= mi.rcMonitor.bottom;
    }
    return false;
}

bool IsBrowserOrVideoFullscreen(HWND self) {
    if (!IsForegroundFullscreen(self)) return false;
    HWND fg = GetForegroundWindow();
    if (!fg) return false;

    std::wstring exePath;
    if (!ProcessImageNameForWindow(fg, &exePath)) return false;
    const std::wstring base = ToLowerCopy(BaseNameFromPath(exePath));

    static const wchar_t* kMediaApps[] = {
        L"chrome.exe", L"msedge.exe", L"firefox.exe", L"opera.exe", L"brave.exe",
        L"vivaldi.exe", L"browser.exe", L"yandex.exe", L"vlc.exe", L"mpc-hc.exe",
        L"mpc-hc64.exe", L"mpc-be.exe", L"mpc-be64.exe", L"potplayer64.exe",
        L"potplayermini64.exe", L"wmplayer.exe", L"kmplayer.exe", L"netflix.exe"
    };

    for (const wchar_t* app : kMediaApps) {
        if (base == app) return true;
    }
    return false;
}

struct MonitorEnumData { std::vector<HMONITOR> monitors; };

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC, LPRECT, LPARAM dwData) {
    reinterpret_cast<MonitorEnumData*>(dwData)->monitors.push_back(hMonitor);
    return TRUE;
}

HMONITOR ResolveTargetMonitor(int targetMonitorIdx) {
    HMONITOR selected = nullptr;
    if (targetMonitorIdx > 0) {
        MonitorEnumData data;
        EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, reinterpret_cast<LPARAM>(&data));
        const int index = targetMonitorIdx - 1;
        if (index >= 0 && index < static_cast<int>(data.monitors.size())) {
            selected = data.monitors[index];
        }
    }
    if (!selected) {
        selected = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    }
    return selected;
}

float GetMonitorDpiScale(HMONITOR monitor) {
    if (!monitor) {
        monitor = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    }
    UINT dpiX = 96, dpiY = 96;
    GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    return static_cast<float>(dpiX) / 96.0f;
}

int GetMonitorRefreshRate(HWND hwnd) {
    HMONITOR monitor = hwnd ? MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST)
                            : MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(monitor, &mi)) {
        DEVMODEW dm = {};
        dm.dmSize = sizeof(dm);
        if (EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm) &&
            dm.dmDisplayFrequency > 1) {
            return static_cast<int>(dm.dmDisplayFrequency);
        }
    }
    return 60;
}

D2D1_COLOR_F GetSystemAccentColor() {
    DWORD color = 0;
    BOOL opaque = FALSE;
    D2D1_COLOR_F result = D2D1::ColorF(0x4cc9f0);
    if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque))) {
        result = D2D1::ColorF(((color >> 16) & 0xff) / 255.0f, ((color >> 8) & 0xff) / 255.0f,
                              (color & 0xff) / 255.0f, 1.0f);
    }
    return result;
}

int ThemeIndexFromId(std::wstring_view id) {
    for (int i = 0; i < static_cast<int>(ARRAYSIZE(kThemePalettes)); ++i) {
        if (EqualsNoCase(id, kThemePalettes[i].id)) return i;
    }
    return -1;
}

void LoadSettings() {
    Settings next;

    const std::wstring position = GetStringSettingCopy(L"Appearance.Position");
    if (EqualsNoCase(position, L"top-left")) next.position = Position::TopLeft;
    else if (EqualsNoCase(position, L"top-right")) next.position = Position::TopRight;
    else if (EqualsNoCase(position, L"bottom-center")) next.position = Position::BottomCenter;
    else if (EqualsNoCase(position, L"bottom-left")) next.position = Position::BottomLeft;
    else if (EqualsNoCase(position, L"bottom-right")) next.position = Position::BottomRight;

    const std::wstring scale = GetStringSettingCopy(L"Appearance.SizeScale");
    if (!scale.empty()) {
        wchar_t* end;
        float parsed = wcstof(scale.c_str(), &end);
        if (end != scale.c_str() && parsed > 0.1f && parsed < 10.0f) next.baseSizeScale = parsed;
    }
    
    next.sizeScale = next.baseSizeScale; 

    const std::wstring mon = GetStringSettingCopy(L"Appearance.TargetMonitor");
    next.targetMonitor = EqualsNoCase(mon, L"primary") ? 0 : _wtoi(mon.c_str());

    if (Wh_GetIntValue(L"CustomPosSet", 0) != 0) {
        g_customCenterX = Wh_GetIntValue(L"CustomPosX", 0);
        g_customTop = Wh_GetIntValue(L"CustomPosY", 0);
        g_customPosSet = true;
    } else {
        g_customPosSet = false;
    }

    std::wstring font = GetStringSettingCopy(L"Themes.FontFamily");
    const size_t fc = font.find_first_not_of(L" \t\r\n\"'");
    if (fc != std::wstring::npos) {
        next.fontFamily = font.substr(fc, font.find_last_not_of(L" \t\r\n\"'") - fc + 1);
    }

    next.smoothAccent = Wh_GetIntSetting(L"Themes.SmoothAccent") != 0;

    const std::wstring fps = GetStringSettingCopy(L"Animations.TargetFPS");
    next.targetFps = (fps.empty() || EqualsNoCase(fps, L"auto")) ? 0 : std::max(0, _wtoi(fps.c_str()));

    const std::wstring style = GetStringSettingCopy(L"Animations.AnimationStyle");
    if (EqualsNoCase(style, L"smooth")) next.animationStyle = AnimationStyle::Smooth;
    else if (EqualsNoCase(style, L"bouncy")) next.animationStyle = AnimationStyle::Bouncy;
    else if (EqualsNoCase(style, L"snappy")) next.animationStyle = AnimationStyle::Snappy;

    const std::wstring speed = GetStringSettingCopy(L"Animations.AnimationSpeed");
    if (EqualsNoCase(speed, L"slow")) next.animationSpeed = 0.75f;
    else if (EqualsNoCase(speed, L"fast")) next.animationSpeed = 1.35f;
    else if (EqualsNoCase(speed, L"ultra-fast")) next.animationSpeed = 2.0f;

    next.expandOnHover = Wh_GetIntSetting(L"Behavior.ExpandOnHover") != 0;
    
    int savedHideFs = Wh_GetIntValue(L"HideInFullscreen", -1);
    if (savedHideFs != -1) {
        g_hideInFullscreen = savedHideFs != 0;
    } else {
        g_hideInFullscreen = Wh_GetIntSetting(L"Behavior.HideInFullscreen") != 0;
    }
    next.hideInFullscreen = g_hideInFullscreen.load();

    next.offsetX = Wh_GetIntSetting(L"Appearance.OffsetX");
    next.offsetY = Wh_GetIntSetting(L"Appearance.OffsetY");

    const std::wstring shape = GetStringSettingCopy(L"Appearance.ShapeStyle");
    next.w11Style = EqualsNoCase(shape, L"w11");
    next.notchStyle = EqualsNoCase(shape, L"notch") && !IsBottomPosition(next.position);

    int themeIdx = ThemeIndexFromId(GetStringSettingCopy(L"Themes.ThemePreset"));
    if (themeIdx < 0) themeIdx = 0;
    const ThemePalette& p = kThemePalettes[themeIdx];
    next.pillBgColor = ColorFromHex(p.bg, next.pillBgColor);
    next.textPrimaryColor = ColorFromHex(p.fg, next.textPrimaryColor);
    next.textSecondaryColor = ColorFromHex(p.sec, next.textSecondaryColor);
    next.contourBorderColor = ColorFromHex(p.border, next.contourBorderColor);

    const std::wstring accentModeStr = GetStringSettingCopy(L"Themes.AccentColorMode");
    if (EqualsNoCase(accentModeStr, L"system")) next.accentMode = AccentMode::System;
    else if (EqualsNoCase(accentModeStr, L"custom")) next.accentMode = AccentMode::Custom;
    
    next.customAccent = ColorFromHex(GetStringSettingCopy(L"Themes.CustomAccentHex"), next.customAccent);

    int overrideAccentMode = Wh_GetIntValue(L"OverrideAccentMode", -1);
    if (overrideAccentMode != -1) next.accentMode = static_cast<AccentMode>(overrideAccentMode);

    wchar_t colorBuf[32];
    if (Wh_GetStringValue(L"OverrideCustomAccent", colorBuf, ARRAYSIZE(colorBuf))) 
        next.customAccent = ColorFromHex(colorBuf, next.customAccent);
    if (Wh_GetStringValue(L"OverrideBgColor", colorBuf, ARRAYSIZE(colorBuf))) 
        next.pillBgColor = ColorFromHex(colorBuf, next.pillBgColor);
    if (Wh_GetStringValue(L"OverrideTextPrimary", colorBuf, ARRAYSIZE(colorBuf))) 
        next.textPrimaryColor = ColorFromHex(colorBuf, next.textPrimaryColor);
    if (Wh_GetStringValue(L"OverrideTextSecondary", colorBuf, ARRAYSIZE(colorBuf))) 
        next.textSecondaryColor = ColorFromHex(colorBuf, next.textSecondaryColor);

    next.pillOpacity = Clamp(Wh_GetIntSetting(L"Themes.PillOpacity") / 100.0f, 0.35f, 1.0f);
    next.textScale = Clamp(Wh_GetIntSetting(L"Themes.TextScale") / 100.0f, 0.7f, 1.6f);

    g_alwaysOnTop = Wh_GetIntValue(L"AlwaysOnTop", 1) != 0;
    g_overGames = Wh_GetIntValue(L"OverGames", 1) != 0;
    g_overTaskbar = Wh_GetIntValue(L"OverTaskbar", 1) != 0;
    g_autoExpand = Wh_GetIntValue(L"AutoExpand", 0) != 0;
    g_lockExpanded = Wh_GetIntValue(L"LockExpanded", 0) != 0;
    g_confirmTrackChange = Wh_GetIntValue(L"ConfirmTrackChange", 0) != 0;
    g_colorRainbowEnabled = Wh_GetIntValue(L"ColorRainbowEnabled", 0) != 0;

    next.buttonStyle = Wh_GetIntValue(L"ButtonStyle", 0) == 1 ? ButtonStyle::Cubic : ButtonStyle::Default;

    int savedLang = Wh_GetIntValue(L"LanguageSetting", 0);
    g_language = static_cast<LanguageSetting>(ClampInt(savedLang, 0, 3));

    g_menuOpacityOverride = Wh_GetIntValue(L"MenuOpacityOverride", -1);
    if (g_menuOpacityOverride.load() != -1) {
        next.pillOpacity = Clamp(g_menuOpacityOverride.load() / 100.0f, 0.35f, 1.0f);
    }
    
    int savedIdleSec = Wh_GetIntValue(L"IdleTimeoutSec", -1);
    if (savedIdleSec != -1) {
        g_idleTimeoutSec = savedIdleSec;
    } else {
        const std::wstring idleStr = GetStringSettingCopy(L"Behavior.IdleTimeout");
        g_idleTimeoutSec = idleStr.empty() ? 0 : _wtoi(idleStr.c_str());
    }
    next.idleTimeoutSec = g_idleTimeoutSec.load();

    wchar_t palBuf[1024];
    if (Wh_GetStringValue(L"RainbowPalette", palBuf, ARRAYSIZE(palBuf))) {
        std::wstring s(palBuf);
        std::vector<D2D1_COLOR_F> loaded;
        size_t pos = 0;
        while (pos < s.size()) {
            size_t comma = s.find(L',', pos);
            if (comma == std::wstring::npos) comma = s.size();
            std::wstring hex = s.substr(pos, comma - pos);
            if (!hex.empty()) {
                loaded.push_back(ColorFromHex(hex, D2D1::ColorF(0)));
            }
            pos = comma + 1;
        }
        if (!loaded.empty()) {
            std::lock_guard lock(g_rainbowMutex);
            g_rainbowPalette = std::move(loaded);
        }
    }
    int rSpeed = Wh_GetIntValue(L"RainbowSpeed", -1);
    if (rSpeed != -1) g_colorRainbowSpeed = rSpeed / 10.0f;

    g_clickThrough = Wh_GetIntValue(L"ClickThrough", 0) != 0;
    g_disableAnimations = Wh_GetIntValue(L"DisableAnimations", 0) != 0;
    g_disableShadow = Wh_GetIntValue(L"DisableShadow", 0) != 0;

    g_hotkeyEnabled = Wh_GetIntValue(L"HotkeyEnabled", 0) != 0;
    g_hotkeyMods = static_cast<unsigned>(Wh_GetIntValue(L"HotkeyMods", MOD_CONTROL));
    g_hotkeyVk = static_cast<unsigned>(Wh_GetIntValue(L"HotkeyVk", 'H'));
    g_hkNextMods = static_cast<unsigned>(Wh_GetIntValue(L"HkNextMods", MOD_CONTROL));
    g_hkNextVk = static_cast<unsigned>(Wh_GetIntValue(L"HkNextVk", 'N'));
    g_hkPrevMods = static_cast<unsigned>(Wh_GetIntValue(L"HkPrevMods", MOD_CONTROL));
    g_hkPrevVk = static_cast<unsigned>(Wh_GetIntValue(L"HkPrevVk", 'B'));
    g_hkPlayPauseMods = static_cast<unsigned>(Wh_GetIntValue(L"HkPlayPauseMods", MOD_CONTROL));
    g_hkPlayPauseVk = static_cast<unsigned>(Wh_GetIntValue(L"HkPlayPauseVk", 'M'));

    LoadAppPositionsFromRegistry();

    {
        std::lock_guard lock(g_settingsMutex);
        g_settings = std::move(next);
    }
    g_layoutDirty = true;
    HWND hSelf = g_hwnd.load();
    if (hSelf) {
        PostMessageW(hSelf, WM_APP_APPLY_HOTKEY, 0, 0);
        PostMessageW(hSelf, WM_APP_WAKE, 0, 0);
    }
}

RECT GetAnchorWorkRect(const Settings& s) {
    HMONITOR selected = ResolveTargetMonitor(s.targetMonitor);
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(selected, &mi);
    return mi.rcWork;
}

void PositionOverlayWindow(HWND hwnd, int width, int height, const Settings& s) {
    int x, y;
    bool customActive = false;
    int cX = 0, cY = 0;

    std::wstring fgExe = GetForegroundExeName();
    if (!fgExe.empty()) {
        std::lock_guard lock(g_appPosMutex);
        auto it = g_appPositions.find(fgExe);
        if (it != g_appPositions.end()) {
            cX = it->second.centerX;
            cY = it->second.top;
            customActive = true;
        }
    }

    if (!customActive && g_customPosSet.load()) {
        cX = g_customCenterX.load();
        cY = g_customTop.load();
        customActive = true;
    }

    if (customActive) {
        x = cX - width / 2;
        y = cY;

        const POINT center = {cX, y + height / 2};
        MONITORINFO mi = {sizeof(mi)};
        HMONITOR mon = MonitorFromPoint(center, MONITOR_DEFAULTTONEAREST);
        if (GetMonitorInfoW(mon, &mi)) {
            const RECT b = g_overTaskbar.load() ? mi.rcMonitor : mi.rcWork;
            const int padX = static_cast<int>(kRenderPadX);
            const int padY = static_cast<int>(kRenderPadY);
            const int minX = b.left - padX;
            const int maxX = std::max(minX, static_cast<int>(b.right) - width + padX);
            const int minY = b.top - padY;
            const int maxY = std::max(minY, static_cast<int>(b.bottom) - height + padY);
            x = ClampInt(x, minX, maxX);
            y = ClampInt(y, minY, maxY);
        }
    } else {
        const RECT work = GetAnchorWorkRect(s);
        const bool flushTop = s.notchStyle;
        const int topY = flushTop ? work.top : (work.top + 8);
        const int bottomY = work.bottom - height - 40;
        x = work.left + (work.right - work.left - width) / 2;
        y = topY;
        switch (s.position) {
            case Position::TopLeft:      x = work.left + 16; break;
            case Position::TopRight:     x = work.right - width - 16; break;
            case Position::BottomCenter: y = bottomY; break;
            case Position::BottomLeft:   x = work.left + 16; y = bottomY; break;
            case Position::BottomRight:  x = work.right - width - 16; y = bottomY; break;
            default: break;
        }
        x += s.offsetX;
        y += s.offsetY;
    }

    SetWindowPos(hwnd, g_alwaysOnTop.load() ? HWND_TOPMOST : HWND_NOTOPMOST, x, y, width, height,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
}

bool DecodeImageBytesToPixels(const std::vector<uint8_t>& bytes, BitmapPixels* outPixels) {
    if (!outPixels || bytes.empty()) return false;

    ComPtr<IWICImagingFactory> factory;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&factory)))) {
        return false;
    }

    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, bytes.size());
    if (!mem) return false;
    void* locked = GlobalLock(mem);
    memcpy(locked, bytes.data(), bytes.size());
    GlobalUnlock(mem);

    ComPtr<IStream> stream;
    if (FAILED(CreateStreamOnHGlobal(mem, TRUE, &stream))) {
        GlobalFree(mem);
        return false;
    }

    ComPtr<IWICBitmapDecoder> decoder;
    ComPtr<IWICBitmapFrameDecode> frame;
    ComPtr<IWICFormatConverter> converter;
    if (FAILED(factory->CreateDecoderFromStream(stream.Get(), nullptr, WICDecodeMetadataCacheOnLoad, &decoder)) ||
        FAILED(decoder->GetFrame(0, &frame)) ||
        FAILED(factory->CreateFormatConverter(&converter)) ||
        FAILED(converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
                                     nullptr, 0.0, WICBitmapPaletteTypeCustom))) {
        return false;
    }

    UINT width = 0, height = 0;
    converter->GetSize(&width, &height);
    if (!width || !height || width > 2048 || height > 2048) return false;

    BitmapPixels pixels;
    pixels.width = width;
    pixels.height = height;
    pixels.bgra.resize(static_cast<size_t>(width) * height * 4);
    if (FAILED(converter->CopyPixels(nullptr, width * 4, static_cast<UINT>(pixels.bgra.size()),
                                     pixels.bgra.data()))) {
        return false;
    }

    struct Bucket { uint32_t count = 0, r = 0, g = 0, b = 0; };
    std::vector<Bucket> buckets(16 * 16 * 16);
    for (size_t i = 0; i + 3 < pixels.bgra.size(); i += 4) {
        const uint8_t alpha = pixels.bgra[i + 3];
        const int blue = pixels.bgra[i + 0], green = pixels.bgra[i + 1], red = pixels.bgra[i + 2];
        if (alpha < 32) continue;
        const int maxc = std::max({red, green, blue});
        const int minc = std::min({red, green, blue});
        const int luminance = (54 * red + 183 * green + 19 * blue) / 256;
        const int saturation = maxc - minc;
        if (luminance < 36 || luminance > 220 || saturation < 28) continue;

        Bucket& bk = buckets[((red >> 4) << 8) | ((green >> 4) << 4) | (blue >> 4)];
        const uint32_t weight = 1 + static_cast<uint32_t>(saturation / 32);
        bk.count += weight;
        bk.r += red * weight;
        bk.g += green * weight;
        bk.b += blue * weight;
    }

    size_t bestIdx = 0;
    uint32_t bestCount = 0;
    for (size_t i = 0; i < buckets.size(); ++i) {
        if (buckets[i].count > bestCount) { bestCount = buckets[i].count; bestIdx = i; }
    }
    if (bestCount > 0) {
        const int ri = static_cast<int>((bestIdx >> 8) & 0xF);
        const int gi = static_cast<int>((bestIdx >> 4) & 0xF);
        const int bi = static_cast<int>(bestIdx & 0xF);
        uint64_t poolCount = 0;
        double pr = 0, pg = 0, pb = 0;
        for (int dr = -1; dr <= 1; ++dr)
            for (int dg = -1; dg <= 1; ++dg)
                for (int db = -1; db <= 1; ++db) {
                    const int a = ri + dr, b = gi + dg, c = bi + db;
                    if (a < 0 || a > 15 || b < 0 || b > 15 || c < 0 || c > 15) continue;
                    const Bucket& nb = buckets[(static_cast<size_t>(a) << 8) | (static_cast<size_t>(b) << 4) | static_cast<size_t>(c)];
                    poolCount += nb.count;
                    pr += nb.r; pg += nb.g; pb += nb.b;
                }
        if (poolCount > 0) {
            pixels.sampledAccent = D2D1::ColorF(static_cast<float>(pr / poolCount) / 255.0f,
                                                static_cast<float>(pg / poolCount) / 255.0f,
                                                static_cast<float>(pb / poolCount) / 255.0f, 1.0f);
        }
    }

    pixels.generation = ++g_artGenerationCounter;
    *outPixels = std::move(pixels);
    return true;
}

bool IconToPixels(HICON icon, UINT size, BitmapPixels* outPixels) {
    if (!icon || !outPixels || !size) return false;

    HDC screen = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screen);
    ReleaseDC(nullptr, screen);
    if (!dc) return false;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = static_cast<LONG>(size);
    bi.bmiHeader.biHeight = -static_cast<LONG>(size);
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap) { DeleteDC(dc); return false; }

    HGDIOBJ old = SelectObject(dc, bitmap);
    ZeroMemory(bits, static_cast<size_t>(size) * size * 4);
    DrawIconEx(dc, 0, 0, icon, size, size, 0, nullptr, DI_NORMAL);

    BitmapPixels pixels;
    pixels.width = size;
    pixels.height = size;
    pixels.bgra.resize(static_cast<size_t>(size) * size * 4);
    memcpy(pixels.bgra.data(), bits, pixels.bgra.size());

    bool hasAlpha = false;
    for (size_t i = 3; i < pixels.bgra.size(); i += 4) {
        if (pixels.bgra[i] != 0) { hasAlpha = true; break; }
    }
    for (size_t i = 0; i + 3 < pixels.bgra.size(); i += 4) {
        if (!hasAlpha) {
            const bool black = pixels.bgra[i] < 4 && pixels.bgra[i + 1] < 4 && pixels.bgra[i + 2] < 4;
            pixels.bgra[i + 3] = black ? 0 : 255;
        } else {
            const uint8_t a = pixels.bgra[i + 3];
            if (a < 255 && a > 0) {
                pixels.bgra[i + 0] = static_cast<uint8_t>(pixels.bgra[i + 0] * a / 255);
                pixels.bgra[i + 1] = static_cast<uint8_t>(pixels.bgra[i + 1] * a / 255);
                pixels.bgra[i + 2] = static_cast<uint8_t>(pixels.bgra[i + 2] * a / 255);
            }
        }
    }

    pixels.generation = ++g_artGenerationCounter;
    *outPixels = std::move(pixels);

    SelectObject(dc, old);
    DeleteObject(bitmap);
    DeleteDC(dc);
    return true;
}

bool ProcessImageNameForPid(DWORD pid, std::wstring* imageName) {
    if (!pid || !imageName) return false;
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return false;
    wchar_t path[MAX_PATH] = {};
    DWORD size = ARRAYSIZE(path);
    const bool ok = QueryFullProcessImageNameW(process, 0, path, &size) != FALSE;
    CloseHandle(process);
    if (ok) *imageName = path;
    return ok;
}

bool ProcessImageNameForWindow(HWND hwnd, std::wstring* imageName) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return pid && ProcessImageNameForPid(pid, imageName);
}

HICON CopyWindowIcon(HWND hwnd, WPARAM iconType) {
    DWORD_PTR result = 0;
    SendMessageTimeoutW(hwnd, WM_GETICON, iconType, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK, 80, &result);
    return result ? CopyIcon(reinterpret_cast<HICON>(result)) : nullptr;
}

HICON GetProcessIcon(DWORD pid) {
    std::wstring path;
    if (ProcessImageNameForPid(pid, &path) && !path.empty()) {
        HICON hIcon = nullptr;
        UINT iconId = 0;
        using Fn = UINT(WINAPI*)(LPCWSTR, int, int, int, HICON*, UINT*, UINT, UINT);
        static auto pExtract = reinterpret_cast<Fn>(
            GetProcAddress(GetModuleHandleW(L"user32.dll"), "PrivateExtractIconsW"));
        if (pExtract && pExtract(path.c_str(), 0, 64, 64, &hIcon, &iconId, 1, 0) == 1 && hIcon) {
            return hIcon;
        }
        SHFILEINFOW sfi = {};
        if (SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON)) {
            return sfi.hIcon;
        }
    }
    return CopyIcon(LoadIconW(nullptr, IDI_APPLICATION));
}

HICON GetWindowIconHandle(HWND hwnd) {
    if (HICON icon = CopyWindowIcon(hwnd, ICON_BIG)) return icon;
    if (HICON icon = CopyWindowIcon(hwnd, ICON_SMALL)) return icon;
    if (auto icon = reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, GCLP_HICON))) return CopyIcon(icon);
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return GetProcessIcon(pid);
}

std::wstring FriendlyMediaSourceName(std::wstring_view source) {
    const std::wstring lower = ToLowerCopy(std::wstring(source));
    if (lower.find(L"youtube") != std::wstring::npos) return L"YouTube";
    if (lower.find(L"spotify") != std::wstring::npos) return L"Spotify";
    if (lower.find(L"chrome") != std::wstring::npos) return L"Chrome";
    if (lower.find(L"msedge") != std::wstring::npos || lower.find(L"edge") != std::wstring::npos) return L"Edge";
    if (lower.find(L"firefox") != std::wstring::npos) return L"Firefox";
    if (lower.find(L"vlc") != std::wstring::npos) return L"VLC";
    if (lower.find(L"zune") != std::wstring::npos || lower.find(L"media") != std::wstring::npos) return L"Media Player";

    std::wstring text(source);
    const size_t bang = text.find(L'!');
    if (bang != std::wstring::npos && bang + 1 < text.size()) text.erase(0, bang + 1);
    const size_t dot = text.find(L'.');
    if (dot != std::wstring::npos) text.resize(dot);
    return text.empty() ? L"Media" : text;
}

std::wstring MediaSourceBadge(std::wstring_view sourceName) {
    if (sourceName.empty()) return L"\u25b6";
    std::wstring badge;
    badge.push_back(static_cast<wchar_t>(towupper(sourceName[0])));
    return badge;
}

BOOL CALLBACK FindMediaSourceWindowProc(HWND hwnd, LPARAM lParam) {
    auto* data = reinterpret_cast<std::pair<const std::wstring*, HWND*>*>(lParam);
    if (!IsWindowVisible(hwnd) || hwnd == g_hwnd.load()) return TRUE;

    std::wstring image;
    if (!ProcessImageNameForWindow(hwnd, &image)) return TRUE;
    const std::wstring base = ToLowerCopy(BaseNameFromPath(image));
    if (base.empty()) return TRUE;

    const std::wstring& src = *data->first;
    if (src.find(base) != std::wstring::npos ||
        (base.find(L"chrome") != std::wstring::npos && src.find(L"chrome") != std::wstring::npos) ||
        (base.find(L"msedge") != std::wstring::npos && src.find(L"edge") != std::wstring::npos) ||
        (base.find(L"vlc") != std::wstring::npos && src.find(L"vlc") != std::wstring::npos)) {
        *data->second = hwnd;
        return FALSE;
    }
    return TRUE;
}

std::shared_ptr<const BitmapPixels> FindMediaSourceIcon(const std::wstring& aumid) {
    const std::wstring lower = ToLowerCopy(aumid);
    HWND found = nullptr;
    std::pair<const std::wstring*, HWND*> data{&lower, &found};
    EnumWindows(FindMediaSourceWindowProc, reinterpret_cast<LPARAM>(&data));
    if (!found) return nullptr;

    BitmapPixels pixels;
    HICON icon = GetWindowIconHandle(found);
    if (icon) {
        IconToPixels(icon, 32, &pixels);
        DestroyIcon(icon);
    }
    if (pixels.bgra.empty()) return nullptr;
    return std::make_shared<const BitmapPixels>(std::move(pixels));
}

std::vector<uint8_t> ReadWinRtStreamBytes(
    const winrt::Windows::Storage::Streams::IRandomAccessStreamReference& reference) {
    std::vector<uint8_t> bytes;
    if (!reference) return bytes;
    auto stream = reference.OpenReadAsync().get();
    if (!stream) return bytes;
    const uint64_t size64 = stream.Size();
    if (size64 == 0 || size64 > 8 * 1024 * 1024) return bytes;
    const uint32_t size = static_cast<uint32_t>(size64);
    winrt::Windows::Storage::Streams::DataReader reader(stream.GetInputStreamAt(0));
    reader.LoadAsync(size).get();
    bytes.resize(size);
    reader.ReadBytes(winrt::array_view<uint8_t>(bytes.data(), bytes.data() + bytes.size()));
    return bytes;
}

DWORD WINAPI MediaThreadProc(void*) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

    using Manager = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager;
    using PlaybackStatus = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackStatus;

    Manager manager{nullptr};
    
    static std::wstring s_cachedVlcTitle;
    static double s_lastVlcScan = 0.0;
    static double s_lastIconSearch = 0.0;
    static std::wstring s_preferredAumid;
    static double s_lastPlayingTime = 0.0;

    while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        MediaSnapshot prev;
        {
            std::lock_guard lock(g_stateMutex);
            prev = g_state.media;
        }
        MediaSnapshot next;
        const double now = NowSeconds();

        try {
            if (!manager) manager = Manager::RequestAsync().get();

            winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession session = nullptr;
            if (manager) {
                bool foundPlaying = false;
                for (auto const& candidate : manager.GetSessions()) {
                    if (candidate) {
                        try {
                            if (candidate.GetPlaybackInfo().PlaybackStatus() == PlaybackStatus::Playing) {
                                session = candidate;
                                foundPlaying = true;
                                break;
                            }
                        } catch (...) {}
                    }
                }

                if (foundPlaying) {
                    s_preferredAumid = session.SourceAppUserModelId().c_str();
                    s_lastPlayingTime = now;
                } else {
                    auto currentSystemSession = manager.GetCurrentSession();
                    std::wstring currentSystemAumid;
                    if (currentSystemSession) currentSystemAumid = currentSystemSession.SourceAppUserModelId().c_str();

                    if (!s_preferredAumid.empty() && currentSystemAumid != s_preferredAumid && (now - s_lastPlayingTime < 10.0)) {
                        for (auto const& candidate : manager.GetSessions()) {
                            if (candidate.SourceAppUserModelId().c_str() == s_preferredAumid) {
                                session = candidate;
                                break;
                            }
                        }
                    }
                    if (!session) {
                        session = currentSystemSession;
                        if (session) s_preferredAumid = session.SourceAppUserModelId().c_str();
                    }
                }
            }

            if (session) {
                next.activeSession = session; 
                auto properties = session.TryGetMediaPropertiesAsync().get();
                auto playback = session.GetPlaybackInfo();
                auto timeline = session.GetTimelineProperties();

                next.available = true;
                next.playing = playback.PlaybackStatus() == PlaybackStatus::Playing;
                next.title = properties.Title().c_str();
                next.artist = properties.Artist().c_str();
                next.albumTitle = properties.AlbumTitle().c_str();

                if (!next.playing) {
                    next.pausedAt = prev.playing ? now : (prev.pausedAt > 0.0 ? prev.pausedAt : now);
                } else {
                    next.pausedAt = 0.0;
                }

                if (timeline) {
                    const int64_t np = timeline.Position().count();
                    const int64_t ne = timeline.EndTime().count();
                    if (np != prev.positionTicks || ne != prev.endTicks || next.playing != prev.playing) {
                        next.positionTicks = np;
                        next.endTicks = ne;
                        next.lastUpdatedTicks = GetTickCount64();
                    } else {
                        next.positionTicks = prev.positionTicks;
                        next.endTicks = prev.endTicks;
                        next.lastUpdatedTicks = prev.lastUpdatedTicks;
                    }
                }

                next.sourceAppUserModelId = session.SourceAppUserModelId().c_str();
                next.sourceName = FriendlyMediaSourceName(next.sourceAppUserModelId);
                next.trackId = next.sourceAppUserModelId + L"|" + next.title + L"|" + next.artist + L"|" + next.albumTitle;

                if (next.title.empty() && next.sourceName == L"VLC") {
                    if (now - s_lastVlcScan > 1.0) {
                        s_lastVlcScan = now;
                        s_cachedVlcTitle.clear();
                        const std::wstring marker = L"VLC media player";
                        HWND hwnd = nullptr;
                        while ((hwnd = FindWindowExW(nullptr, hwnd, nullptr, nullptr)) != nullptr) {
                            if (!IsWindowVisible(hwnd)) continue;
                            wchar_t windowTitle[512];
                            if (GetWindowTextW(hwnd, windowTitle, ARRAYSIZE(windowTitle)) <= 0) continue;
                            std::wstring caption(windowTitle);
                            while (!caption.empty() && caption.back() == L' ') caption.pop_back();
                            if (caption.size() <= marker.size() ||
                                caption.compare(caption.size() - marker.size(), marker.size(), marker) != 0) {
                                continue;
                            }
                            std::wstring image;
                            if (!ProcessImageNameForWindow(hwnd, &image) ||
                                ToLowerCopy(BaseNameFromPath(image)).find(L"vlc") == std::wstring::npos) {
                                continue;
                            }
                            std::wstring candidate = caption.substr(0, caption.size() - marker.size());
                            while (!candidate.empty() && (candidate.back() == L' ' || candidate.back() == L'-')) {
                                candidate.pop_back();
                            }
                            if (!candidate.empty()) { s_cachedVlcTitle = candidate; break; }
                        }
                    }
                    next.title = s_cachedVlcTitle;
                    next.trackId = next.sourceAppUserModelId + L"|VLC_HACK|" + next.title;
                }

                if (next.sourceAppUserModelId == prev.sourceAppUserModelId) {
                    next.sourceBadge = prev.sourceBadge;
                    next.sourceIcon = prev.sourceIcon;
                    if (!next.sourceIcon && (now - s_lastIconSearch > 5.0)) {
                        s_lastIconSearch = now;
                        next.sourceIcon = FindMediaSourceIcon(next.sourceAppUserModelId);
                    }
                } else {
                    next.sourceBadge = MediaSourceBadge(next.sourceName);
                    s_lastIconSearch = now;
                    next.sourceIcon = FindMediaSourceIcon(next.sourceAppUserModelId);
                }

                bool metadataChanged = (next.trackId != prev.trackId);
                next.metaChangedAt = metadataChanged ? now : prev.metaChangedAt;

                // Простой синхронный запрос обложки без фоновых потоков и кеша
                if (metadataChanged || !prev.art) {
                    try {
                        auto thumbnailRef = properties.Thumbnail();
                        if (thumbnailRef) {
                            std::vector<uint8_t> bytes = ReadWinRtStreamBytes(thumbnailRef);
                            BitmapPixels pixels;
                            if (!bytes.empty() && DecodeImageBytesToPixels(bytes, &pixels)) {
                                next.art = std::make_shared<const BitmapPixels>(std::move(pixels));
                            }
                        }
                    } catch (...) {}
                } else {
                    next.art = prev.art;
                }
            }
        } catch (...) {
            manager = nullptr;
        }

        bool trackChanged = false;
        bool anyChange = false;
        {
            std::lock_guard lock(g_stateMutex);
            const MediaSnapshot& cur = g_state.media;
            trackChanged = next.available && !next.title.empty() && next.playing &&
                           (next.trackId != cur.trackId || !cur.playing);
            next.titleChangedAt = trackChanged ? now : cur.titleChangedAt;
            anyChange = next.available != cur.available || next.playing != cur.playing ||
                        next.trackId != cur.trackId;
            
            if (!next.art && prev.art && next.trackId == prev.trackId) {
                next.art = prev.art;
            }
            
            g_state.media = std::move(next);
        }

        if (HWND h = g_hwnd.load()) {
            if (trackChanged && g_autoExpand.load()) {
                const double t = NowSeconds();
                if (t - g_lastNudgeTime.load() > 0.45) {
                    g_lastNudgeTime = t;
                    PostMessageW(h, WM_APP_NEW_EVENT, 0, 0);
                }
            } else if (anyChange) {
                PostMessageW(h, WM_APP_WAKE, 0, 0);
            }
        }

        WaitForSingleObject(g_stopEvent, 100);
    }

    manager = nullptr;
    winrt::uninit_apartment();
    return 0;
}

void SeekMediaToTicks(int64_t targetTicks) {
    std::thread([targetTicks]() {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        try {
            winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession session = nullptr;
            {
                std::lock_guard lock(g_stateMutex);
                session = g_state.media.activeSession;
            }
            if (session) {
                session.TryChangePlaybackPositionAsync(targetTicks).get();
            }
        } catch (...) {}
        winrt::uninit_apartment();
    }).detach();
}

void SendMediaTransportCommand(int cmd) {
    {
        std::lock_guard lock(g_stateMutex);
        if (cmd == 1) {
            g_state.media.playing = !g_state.media.playing;
            g_state.media.pausedAt = g_state.media.playing ? 0.0 : NowSeconds();
            g_state.media.lastUpdatedTicks = GetTickCount64();
        } else if (cmd == 0 || cmd == 2) {
            g_state.media.positionTicks = 0;
            g_state.media.lastUpdatedTicks = GetTickCount64();
        }
    }
    
    if (HWND h = g_hwnd.load()) {
        PostMessageW(h, WM_APP_WAKE, 0, 0);
    }

    std::thread([cmd]() {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        bool sessionAttempted = false;
        try {
            winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession session = nullptr;
            {
                std::lock_guard lock(g_stateMutex);
                session = g_state.media.activeSession;
            }
            if (session) {
                sessionAttempted = true;
                if (cmd == 0) session.TrySkipPreviousAsync().get();
                else if (cmd == 1) session.TryTogglePlayPauseAsync().get();
                else if (cmd == 2) session.TrySkipNextAsync().get();
            }
        } catch (...) {
            sessionAttempted = false;
        }

        if (!sessionAttempted) {
            HWND fg = GetForegroundWindow();
            if (!fg) fg = GetDesktopWindow();
            WORD appCmd = (cmd == 0) ? 12 : ((cmd == 1) ? 14 : 11);
            SendNotifyMessageW(fg, WM_APPCOMMAND, (WPARAM)fg, MAKELONG(0, appCmd));
        }
        winrt::uninit_apartment();
    }).detach();
}

struct WindowSearch {
    std::wstring targetTitle;
    std::wstring targetApp;
    HWND foundHwnd = nullptr;
    HWND fallbackHwnd = nullptr;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    if (!IsWindowVisible(hwnd)) return TRUE;
    auto* search = reinterpret_cast<WindowSearch*>(lParam);

    wchar_t title[512];
    if (!search->targetTitle.empty() && GetWindowTextW(hwnd, title, ARRAYSIZE(title)) > 0) {
        std::wstring wTitle(title);
        auto it = std::search(wTitle.begin(), wTitle.end(), search->targetTitle.begin(),
                              search->targetTitle.end(),
                              [](wchar_t a, wchar_t b) { return towlower(a) == towlower(b); });
        if (it != wTitle.end()) {
            search->foundHwnd = hwnd;
            return FALSE;
        }
    }

    if (!search->fallbackHwnd && !search->targetApp.empty()) {
        std::wstring exePath;
        if (ProcessImageNameForWindow(hwnd, &exePath)) {
            std::wstring target = ToLowerCopy(search->targetApp);
            if (target.size() >= 2 && target.front() == L'"' && target.back() == L'"') {
                target = target.substr(1, target.size() - 2);
            }
            const std::wstring exeLower = ToLowerCopy(exePath);
            const std::wstring exeName = ToLowerCopy(BaseNameFromPath(exePath));
            if (exeLower == target || exeName == target || exeName == target + L".exe") {
                search->fallbackHwnd = hwnd;
            }
        }
    }
    return TRUE;
}

void OpenRelevantApp() {
    std::wstring title, app;
    {
        std::lock_guard lock(g_stateMutex);
        title = g_state.media.title;
        app = g_state.media.sourceAppUserModelId;
    }

    if (!title.empty() || !app.empty()) {
        WindowSearch search;
        search.targetTitle = title;
        search.targetApp = app;
        EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&search));
        HWND target = search.foundHwnd ? search.foundHwnd : search.fallbackHwnd;
        if (target) {
            if (IsIconic(target)) ShowWindow(target, SW_RESTORE);
            SetForegroundWindow(target);
            return;
        }
    }

    if (!app.empty()) {
        std::wstring path = app;
        if (path.size() >= 2 && path.front() == L'"' && path.back() == L'"') {
            path = path.substr(1, path.size() - 2);
        }
        const bool isFilePath = path.find(L":\\") != std::wstring::npos ||
                                (path.size() >= 4 && path.substr(path.size() - 4) == L".exe");
        std::wstring toRun = path;
        if (isFilePath) {
            if (GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) return;
        } else {
            toRun = L"shell:AppsFolder\\" + path;
        }
        SHELLEXECUTEINFOW sei = {sizeof(sei)};
        sei.fMask = SEE_MASK_FLAG_NO_UI;
        sei.lpFile = toRun.c_str();
        sei.nShow = SW_SHOWNORMAL;
        ShellExecuteExW(&sei);
    }
}

MediaContentPoint MediaContentFromClient(int clientX, int clientY) {
    MediaContentPoint out;
    float left = 0, top = 0, right = 0, bottom = 0, scale = 1.0f;
    unsigned long long stamp = 0;
    for (int attempt = 0; attempt < 8; ++attempt) {
        const unsigned before = g_mediaHitSeq.load(std::memory_order_relaxed);
        if (before & 1u) continue;
        std::atomic_thread_fence(std::memory_order_acquire);
        left = g_mediaHitLeft.load(std::memory_order_relaxed);
        top = g_mediaHitTop.load(std::memory_order_relaxed);
        right = g_mediaHitRight.load(std::memory_order_relaxed);
        bottom = g_mediaHitBottom.load(std::memory_order_relaxed);
        scale = g_mediaHitScale.load(std::memory_order_relaxed);
        stamp = g_mediaHitStamp.load(std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_acquire);
        if (g_mediaHitSeq.load(std::memory_order_relaxed) == before) break;
        stamp = 0;
    }
    if (stamp == 0) return out;
    if (scale < 0.05f) scale = 1.0f;

    const float width = right - left;
    const float height = bottom - top;
    if (width <= 1.0f || height <= 1.0f) return out;

    const float cx = (left + right) * 0.5f;
    const float cy = (top + bottom) * 0.5f;
    out.valid = true;
    out.x = (static_cast<float>(clientX) - cx) / scale + cx - left;
    out.y = (static_cast<float>(clientY) - cy) / scale + cy - top;
    out.width = width;
    out.height = height;
    return out;
}

int MediaTransportHitTest(const MediaContentPoint& pt) {
    if (!pt.valid || pt.height <= MediaLayout::kExpandedMinHeight) return -1;
    const float centerX = pt.width * 0.5f;
    const float dy = pt.y - MediaLayout::kControlsY;

    struct Target { float offsetX; float radius; int command; };
    const Target targets[] = {
        {-MediaLayout::kControlSpacing, MediaLayout::kNavButtonRadius, 0},
        {0.0f, MediaLayout::kPlayButtonRadius, 1},
        {MediaLayout::kControlSpacing, MediaLayout::kNavButtonRadius, 2},
    };
    for (const Target& t : targets) {
        const float reach = t.radius + MediaLayout::kControlHitPad;
        if (std::fabs(dy) <= reach && std::fabs(pt.x - (centerX + t.offsetX)) <= reach) return t.command;
    }
    return -1;
}

float MediaScrubFractionFromContent(const MediaContentPoint& pt) {
    if (!pt.valid || pt.height <= MediaLayout::kExpandedMinHeight) return -1.0f;
    const float barLeft = MediaLayout::kScrubInsetLeft;
    const float barRight = pt.width - MediaLayout::kScrubInsetRight;
    if (barRight - barLeft <= 1.0f) return -1.0f;
    if (std::fabs(pt.y - MediaLayout::kScrubberY) > MediaLayout::kScrubHitHalfHeight ||
        pt.x < barLeft - MediaLayout::kScrubHitPadX || pt.x > barRight + MediaLayout::kScrubHitPadX) {
        return -1.0f;
    }
    return Clamp((Clamp(pt.x, barLeft, barRight) - barLeft) / (barRight - barLeft), 0.0f, 1.0f);
}

float MediaScrubFractionUnbounded(const MediaContentPoint& pt) {
    if (!pt.valid) return -1.0f;
    const float barLeft = MediaLayout::kScrubInsetLeft;
    const float barRight = pt.width - MediaLayout::kScrubInsetRight;
    if (barRight - barLeft <= 1.0f) return -1.0f;
    return Clamp((Clamp(pt.x, barLeft, barRight) - barLeft) / (barRight - barLeft), 0.0f, 1.0f);
}

bool MediaArtHitTest(const MediaContentPoint& pt) {
    if (!pt.valid || pt.height <= MediaLayout::kExpandedMinHeight) return false;
    return pt.x >= MediaLayout::kArtInsetX && pt.x <= MediaLayout::kArtInsetX + MediaLayout::kArtSize &&
           pt.y >= MediaLayout::kArtInsetY && pt.y <= MediaLayout::kArtInsetY + MediaLayout::kArtSize;
}

bool MediaGearHitTest(const MediaContentPoint& pt) {
    if (g_lockExpanded.load()) return false;
    if (!pt.valid || pt.height <= MediaLayout::kExpandedMinHeight) return false;
    const float dx = pt.x - (pt.width - MediaLayout::kGearInsetRight);
    const float dy = pt.y - MediaLayout::kGearInsetTop;
    return dx * dx + dy * dy <= MediaLayout::kGearHitRadius * MediaLayout::kGearHitRadius;
}

struct MarqueeLayoutCache {
    std::wstring text;
    IDWriteTextFormat* format = nullptr;
    float wrapHeight = 0.0f;
    ComPtr<IDWriteTextLayout> layout;
    DWRITE_TEXT_METRICS metrics{};
};

class Renderer {
   public:
    bool Initialize(HWND hwnd) {
        hwnd_ = hwnd;
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory),
                                     reinterpret_cast<void**>(d2dFactory_.GetAddressOf())))) {
            return false;
        }
        if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                       reinterpret_cast<IUnknown**>(dwriteFactory_.GetAddressOf())))) {
            return false;
        }
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), 0.0f, 0.0f,
            D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE);
        if (FAILED(d2dFactory_->CreateDCRenderTarget(&props, &target_))) return false;
        return CreateBackingBitmap(520, 140);
    }

    bool Render(const SharedState& state, const Settings& settings, float width, float height,
                float nudge, bool hover, bool pinned, double now, bool isInteracting) {
        cfg_ = &settings;
        EnsureTextFormats(settings.fontFamily, settings.textScale);

        const int pw = std::max(1, static_cast<int>(std::ceil(width + kRenderPadX * 2.0f)));
        const int ph = std::max(1, static_cast<int>(std::ceil(height + kRenderPadY * 2.0f)));
        if (pw != bitmapWidth_ || ph != bitmapHeight_) {
            if (!CreateBackingBitmap(pw, ph)) return false;
            g_layoutDirty = false;
            PositionOverlayWindow(hwnd_, pw, ph, settings);
        } else if (g_layoutDirty.exchange(false)) {
            PositionOverlayWindow(hwnd_, pw, ph, settings);
        }

        RECT rc = {0, 0, bitmapWidth_, bitmapHeight_};
        if (FAILED(target_->BindDC(memDc_, &rc))) return false;

        target_->BeginDraw();
        target_->Clear(D2D1::ColorF(0, 0.0f));
        EnsureBrushes(settings, state, now);

        if (width >= 2.0f && height >= 2.0f) {
            const float top = settings.notchStyle ? std::max(0.0f, nudge) : (kRenderPadY + nudge);
            const float scale = (settings.expandOnHover && (hover || pinned) && !g_disableAnimations.load()) ? 1.025f : 1.0f;
            DrawPill(state, settings, D2D1::RectF(kRenderPadX, top, kRenderPadX + width, top + height),
                     scale, now);
        }

        if (FAILED(target_->EndDraw())) return false;

        POINT src = {0, 0};
        SIZE size = {bitmapWidth_, bitmapHeight_};
        RECT winRect = {};
        GetWindowRect(hwnd_, &winRect);
        POINT dst = {winRect.left, winRect.top};

        BLENDFUNCTION blend = {};
        blend.BlendOp = AC_SRC_OVER;

        const float activeOpacity = isInteracting ? 1.0f : settings.pillOpacity;
        blend.SourceConstantAlpha = static_cast<BYTE>(Clamp(activeOpacity, 0.35f, 1.0f) * 255.0f);
        blend.AlphaFormat = AC_SRC_ALPHA;
        return UpdateLayeredWindow(hwnd_, nullptr, &dst, &size, memDc_, &src, 0, &blend, ULW_ALPHA) != FALSE;
    }

    bool IsAnimationActive(float currentUnscaledWidth) const {
        const float textLeft = MediaLayout::kArtInsetX + MediaLayout::kArtSize + 18.0f;
        const float textRight = currentUnscaledWidth - 24.0f - 50.0f - 16.0f;
        const float available = textRight - textLeft;
        if (available > 0.0f) {
            if (marqueeTitle_.layout && marqueeTitle_.metrics.widthIncludingTrailingWhitespace > available) return true;
            if (marqueeArtist_.layout && marqueeArtist_.metrics.widthIncludingTrailingWhitespace > available) return true;
            if (marqueeAlbum_.layout && marqueeAlbum_.metrics.widthIncludingTrailingWhitespace > available) return true;
        }
        return false;
    }

    void Shutdown() {
        marqueeTitle_.layout.Reset();
        marqueeArtist_.layout.Reset();
        marqueeAlbum_.layout.Reset();
        artBitmap_.Reset();
        sourceIconBitmap_.Reset();
        accentBrush_.Reset();
        textBrush_.Reset();
        mutedBrush_.Reset();
        target_.Reset();
        textFormat_.Reset();
        smallTextFormat_.Reset();
        clockFormat_.Reset();
        iconFormat_.Reset();
        playIconFormat_.Reset();
        navIconFormat_.Reset();
        dwriteFactory_.Reset();
        d2dFactory_.Reset();
        if (oldBitmap_) { SelectObject(memDc_, oldBitmap_); oldBitmap_ = nullptr; }
        if (dib_) { DeleteObject(dib_); dib_ = nullptr; }
        if (memDc_) { DeleteDC(memDc_); memDc_ = nullptr; }
    }

   private:
    struct Material {
        D2D1_COLOR_F raised{};
        D2D1_COLOR_F raisedStrong{};
        D2D1_COLOR_F hairline{};
        D2D1_COLOR_F shadow{};
        D2D1_COLOR_F accent{};
        bool onDark = true;
    };

    static D2D1_COLOR_F WithAlpha(D2D1_COLOR_F c, float a) { c.a = Clamp(a, 0.0f, 1.0f); return c; }

    static D2D1_COLOR_F MixColor(const D2D1_COLOR_F& a, const D2D1_COLOR_F& b, float t) {
        return D2D1::ColorF(a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t,
                            a.a + (b.a - a.a) * t);
    }

    ComPtr<ID2D1SolidColorBrush> MakeBrush(D2D1_COLOR_F color) {
        ComPtr<ID2D1SolidColorBrush> b;
        target_->CreateSolidColorBrush(color, &b);
        return b;
    }

    bool CreateBackingBitmap(int width, int height) {
        if (oldBitmap_) { SelectObject(memDc_, oldBitmap_); oldBitmap_ = nullptr; }
        if (dib_) { DeleteObject(dib_); dib_ = nullptr; }
        if (!memDc_) {
            HDC screen = GetDC(nullptr);
            memDc_ = CreateCompatibleDC(screen);
            ReleaseDC(nullptr, screen);
            if (!memDc_) return false;
        }
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = width;
        bi.bmiHeader.biHeight = -height;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        dib_ = CreateDIBSection(memDc_, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!dib_) return false;
        oldBitmap_ = static_cast<HBITMAP>(SelectObject(memDc_, dib_));
        bitmapWidth_ = width;
        bitmapHeight_ = height;
        return true;
    }

    void EnsureTextFormats(const std::wstring& fontFamily, float textScale) {
        if (textFormat_ && fontFamily == lastFontFamily_ && std::abs(textScale - lastTextScale_) < 0.001f) return;
        lastTextScale_ = textScale;
        const float ts = Clamp(textScale, 0.7f, 1.6f);

        textFormat_.Reset();
        smallTextFormat_.Reset();
        clockFormat_.Reset();
        iconFormat_.Reset();
        playIconFormat_.Reset();
        navIconFormat_.Reset();
        marqueeTitle_.layout.Reset();
        marqueeArtist_.layout.Reset();
        marqueeAlbum_.layout.Reset();

        const wchar_t* defDisplay = L"Segoe UI Variable Display";
        const wchar_t* defSmall = L"Segoe UI Variable Small";
        const wchar_t* mainFamily = fontFamily.empty() ? defDisplay : fontFamily.c_str();
        const wchar_t* smallFamily = fontFamily.empty() ? defSmall : fontFamily.c_str();

        auto create = [&](const wchar_t* family, const wchar_t* fallback, DWRITE_FONT_WEIGHT weight,
                          float size, ComPtr<IDWriteTextFormat>& out) {
            HRESULT hr = dwriteFactory_->CreateTextFormat(family, nullptr, weight, DWRITE_FONT_STYLE_NORMAL,
                                                          DWRITE_FONT_STRETCH_NORMAL, size, L"", &out);
            if (FAILED(hr) || !out) {
                hr = dwriteFactory_->CreateTextFormat(fallback, nullptr, weight, DWRITE_FONT_STYLE_NORMAL,
                                                      DWRITE_FONT_STRETCH_NORMAL, size, L"", &out);
            }
            if (FAILED(hr) || !out) {
                dwriteFactory_->CreateTextFormat(L"Segoe UI", nullptr, weight, DWRITE_FONT_STYLE_NORMAL,
                                                 DWRITE_FONT_STRETCH_NORMAL, size, L"", &out);
            }
        };
        create(mainFamily, defDisplay, DWRITE_FONT_WEIGHT_SEMI_BOLD, 13.5f * ts, textFormat_);
        create(smallFamily, defSmall, DWRITE_FONT_WEIGHT_NORMAL, 11.0f * ts, smallTextFormat_);
        create(mainFamily, defDisplay, DWRITE_FONT_WEIGHT_BOLD, 12.0f * ts, clockFormat_);

        usingFluentIcons_ = false;
        ComPtr<IDWriteFontCollection> sysFonts;
        if (SUCCEEDED(dwriteFactory_->GetSystemFontCollection(&sysFonts, FALSE)) && sysFonts) {
            UINT32 idx = 0;
            BOOL found = FALSE;
            if (SUCCEEDED(sysFonts->FindFamilyName(L"Segoe Fluent Icons", &idx, &found)) && found) {
                usingFluentIcons_ = true;
            }
        }
        const wchar_t* iconFamily = usingFluentIcons_ ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets";
        auto createIcon = [&](float size, ComPtr<IDWriteTextFormat>& out) {
            dwriteFactory_->CreateTextFormat(iconFamily, nullptr, DWRITE_FONT_WEIGHT_NORMAL,
                                             DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, L"", &out);
            if (out) {
                out->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
                out->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                out->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            }
        };
        createIcon(15.0f, iconFormat_);
        createIcon(18.0f, playIconFormat_);
        createIcon(14.0f, navIconFormat_);

        if (textFormat_) textFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        if (smallTextFormat_) smallTextFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        if (clockFormat_) {
            clockFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            clockFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            clockFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        lastFontFamily_ = fontFamily;
    }

    void EnsureBrushes(const Settings& settings, const SharedState& state, double now) {
        D2D1_COLOR_F target = settings.customAccent;

        if (g_colorRainbowEnabled.load() && state.media.playing) {
            std::lock_guard lock(g_rainbowMutex);
            if (!g_rainbowPalette.empty()) {
                const float speed = g_colorRainbowSpeed.load();
                const float totalTime = static_cast<float>(now) * speed * 0.4f;
                const size_t count = g_rainbowPalette.size();
                const float idxFloat = std::fmod(totalTime, static_cast<float>(count));
                const size_t idx1 = static_cast<size_t>(idxFloat) % count;
                const size_t idx2 = (idx1 + 1) % count;
                const float t = idxFloat - std::floor(idxFloat);

                target = MixColor(g_rainbowPalette[idx1], g_rainbowPalette[idx2], t);
            }
        } else if (settings.accentMode == AccentMode::System) {
            target = GetSystemAccentColor();
        } else if (settings.accentMode == AccentMode::Auto && state.media.art) {
            target = state.media.art->sampledAccent;
        }

        target = EnsureContrastAgainstBackground(target, settings.pillBgColor);

        if (lastAccentTime_ < 0.0 || !settings.smoothAccent) {
            currentAccent_ = target;
        } else {
            const double dt = std::max(0.0, std::min(now - lastAccentTime_, 0.1));
            const float tau = 0.45f;
            const float k = 1.0f - static_cast<float>(std::exp(-dt / tau));
            currentAccent_.r += (target.r - currentAccent_.r) * k;
            currentAccent_.g += (target.g - currentAccent_.g) * k;
            currentAccent_.b += (target.b - currentAccent_.b) * k;
            currentAccent_.a = 1.0f;
            if (std::abs(currentAccent_.r - target.r) > 0.001f ||
                std::abs(currentAccent_.g - target.g) > 0.001f ||
                std::abs(currentAccent_.b - target.b) > 0.001f) {
                g_layoutDirty = true;
            }
        }
        lastAccentTime_ = now;

        if (!accentBrush_) target_->CreateSolidColorBrush(currentAccent_, &accentBrush_);
        else accentBrush_->SetColor(currentAccent_);

        D2D1_COLOR_F primary = WithAlpha(settings.textPrimaryColor, 0.98f);
        if (!textBrush_) target_->CreateSolidColorBrush(primary, &textBrush_);
        else textBrush_->SetColor(primary);

        D2D1_COLOR_F secondary = WithAlpha(settings.textSecondaryColor, 0.90f);
        if (!mutedBrush_) target_->CreateSolidColorBrush(secondary, &mutedBrush_);
        else mutedBrush_->SetColor(secondary);

        pillBgColor_ = settings.pillBgColor;
        settingsOpacity_ = settings.pillOpacity;

        const bool onDark = RelativeLuminance(pillBgColor_) < 0.45;
        const D2D1_COLOR_F lift = onDark ? D2D1::ColorF(1, 1, 1, 1) : D2D1::ColorF(0, 0, 0, 1);
        material_.onDark = onDark;
        material_.raised = WithAlpha(lift, onDark ? 0.085f : 0.062f);
        material_.raisedStrong = WithAlpha(lift, onDark ? 0.150f : 0.105f);
        material_.hairline = WithAlpha(lift, onDark ? 0.100f : 0.085f);
        material_.shadow = D2D1::ColorF(0, 0, 0, 0.55f);
        material_.accent = currentAccent_;
    }

    ComPtr<ID2D1PathGeometry> CreateNotchGeometry(D2D1_RECT_F rect, float radius) {
        ComPtr<ID2D1PathGeometry> geom;
        if (FAILED(d2dFactory_->CreatePathGeometry(&geom))) return nullptr;
        ComPtr<ID2D1GeometrySink> sink;
        if (FAILED(geom->Open(&sink))) return nullptr;

        float r = std::min({radius, (rect.right - rect.left) * 0.5f, (rect.bottom - rect.top) * 0.5f});
        if (r < 0.0f) r = 0.0f;

        sink->BeginFigure(D2D1::Point2F(rect.left, rect.top), D2D1_FIGURE_BEGIN_FILLED);
        sink->AddLine(D2D1::Point2F(rect.right, rect.top));
        sink->AddLine(D2D1::Point2F(rect.right, rect.bottom - r));
        if (r > 0.0f) {
            sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(rect.right - r, rect.bottom), D2D1::SizeF(r, r), 0.0f,
                                          D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
            sink->AddLine(D2D1::Point2F(rect.left + r, rect.bottom));
            sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(rect.left, rect.bottom - r), D2D1::SizeF(r, r), 0.0f,
                                          D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
        } else {
            sink->AddLine(D2D1::Point2F(rect.left, rect.bottom));
        }
        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
        sink->Close();
        return geom;
    }

    ComPtr<ID2D1Geometry> CreateIslandMaskGeometry(D2D1_RECT_F rect, float radius) {
        ComPtr<ID2D1Geometry> base;
        if (cfg_->notchStyle) {
            ComPtr<ID2D1PathGeometry> g = CreateNotchGeometry(rect, radius);
            if (g) { g.As(&base); return base; }
        }
        ComPtr<ID2D1RoundedRectangleGeometry> rr;
        d2dFactory_->CreateRoundedRectangleGeometry(D2D1::RoundedRect(rect, radius, radius), &rr);
        if (rr) rr.As(&base);
        return base;
    }

    void FillIslandShape(D2D1_RECT_F rect, float radius, ID2D1Brush* brush) {
        if (!brush) return;
        if (cfg_->notchStyle) {
            auto g = CreateNotchGeometry(rect, radius);
            if (g) { target_->FillGeometry(g.Get(), brush); return; }
        }
        target_->FillRoundedRectangle(D2D1::RoundedRect(rect, radius, radius), brush);
    }

    void DrawIslandShape(D2D1_RECT_F rect, float radius, ID2D1Brush* brush, float stroke,
                         ID2D1StrokeStyle* style = nullptr) {
        if (!brush) return;
        if (cfg_->notchStyle) {
            auto g = CreateNotchGeometry(rect, radius);
            if (g) { target_->DrawGeometry(g.Get(), brush, stroke, style); return; }
        }
        target_->DrawRoundedRectangle(D2D1::RoundedRect(rect, radius, radius), brush, stroke, style);
    }

    float ContentIslandRadius(float contentHeight) const {
        if (cfg_->notchStyle) return 16.0f;
        if (cfg_->w11Style) return 8.0f;
        return std::min(contentHeight * 0.5f, 44.0f);
    }

    void DrawSoftShadow(D2D1_RECT_F rect, float radius) {
        if (g_disableShadow.load()) return;

        const float spread = Clamp(14.0f * cfg_->sizeScale, 6.0f, kRenderPadY - 4.0f);
        const float yOffset = spread * 0.35f;
        constexpr int kSteps = 7;
        auto brush = MakeBrush(material_.shadow);
        if (!brush) return;
        for (int i = kSteps; i >= 1; --i) {
            const float t = static_cast<float>(i) / kSteps;
            const float grow = spread * t;
            const float alpha = material_.shadow.a * (1.0f - t) * (1.0f - t) * 0.55f * settingsOpacity_;
            if (alpha <= 0.002f) continue;
            brush->SetOpacity(alpha);
            FillIslandShape(D2D1::RectF(rect.left - grow, rect.top - grow * 0.55f + yOffset,
                                        rect.right + grow, rect.bottom + grow + yOffset),
                            radius + grow, brush.Get());
        }
    }

    void FillSurfaceDepth(D2D1_RECT_F rect, float radius) {
        const float h = rect.bottom - rect.top;
        if (h <= 2.0f) return;
        const float botA = 0.085f * (material_.onDark ? 1.0f : 0.55f) * settingsOpacity_;

        D2D1_GRADIENT_STOP stops[3] = {};
        stops[0] = {0.0f, D2D1::ColorF(0, 0, 0, 0.0f)};
        stops[1] = {0.45f, D2D1::ColorF(0, 0, 0, botA * 0.25f)};
        stops[2] = {1.0f, D2D1::ColorF(0, 0, 0, botA)};

        ComPtr<ID2D1GradientStopCollection> coll;
        ComPtr<ID2D1LinearGradientBrush> brush;
        if (FAILED(target_->CreateGradientStopCollection(stops, 3, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &coll)) || !coll) return;
        if (FAILED(target_->CreateLinearGradientBrush(
                D2D1::LinearGradientBrushProperties(D2D1::Point2F(rect.left, rect.top),
                                                    D2D1::Point2F(rect.left, rect.bottom)),
                coll.Get(), &brush)) || !brush) return;
        FillIslandShape(rect, radius, brush.Get());
    }

    void FillAccentBloom(D2D1_RECT_F rect, float radius, float strength) {
        const float w = rect.right - rect.left;
        const float h = rect.bottom - rect.top;
        if (w <= 2.0f || h <= 2.0f) return;

        D2D1_GRADIENT_STOP stops[2] = {};
        stops[0] = {0.0f, WithAlpha(material_.accent, strength * settingsOpacity_)};
        stops[1] = {1.0f, WithAlpha(material_.accent, 0.0f)};

        ComPtr<ID2D1GradientStopCollection> coll;
        ComPtr<ID2D1RadialGradientBrush> brush;
        if (FAILED(target_->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &coll)) || !coll) return;
        const D2D1_POINT_2F center = D2D1::Point2F((rect.left + rect.right) * 0.5f, rect.top);
        if (FAILED(target_->CreateRadialGradientBrush(
                D2D1::RadialGradientBrushProperties(center, D2D1::Point2F(0, 0), w * 0.75f, h * 1.15f),
                coll.Get(), &brush)) || !brush) return;
        FillIslandShape(rect, radius, brush.Get());
    }

    void DrawPillSurface(D2D1_RECT_F rect, float radius, const Settings& settings) {
        D2D1_COLOR_F bg = pillBgColor_;
        bg.a = Clamp(bg.a * settingsOpacity_, 0.0f, 1.0f);
        auto fill = MakeBrush(bg);
        FillIslandShape(rect, radius, fill.Get());

        FillSurfaceDepth(rect, radius);
        FillAccentBloom(rect, radius, 0.115f);

        D2D1_COLOR_F border = settings.contourBorderColor;
        border.a = std::min(1.0f, border.a * settingsOpacity_);
        auto bb = MakeBrush(border);
        DrawIslandShape(D2D1::RectF(rect.left + 0.5f, rect.top + 0.5f, rect.right - 0.5f, rect.bottom - 0.5f),
                        radius, bb.Get(), settings.w11Style ? 1.0f : 0.8f);
    }

    void DrawPill(const SharedState& state, const Settings& settings, D2D1_RECT_F rect, float scale, double now) {
        const float cx = (rect.left + rect.right) * 0.5f;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float w = (rect.right - rect.left) * scale;
        const float h = (rect.bottom - rect.top) * scale;
        rect = D2D1::RectF(cx - w * 0.5f, cy - h * 0.5f, cx + w * 0.5f, cy + h * 0.5f);

        float radius = settings.w11Style ? 8.0f * settings.sizeScale : (rect.bottom - rect.top) * 0.5f;
        if (settings.notchStyle) radius = 16.0f * settings.sizeScale;
        else if (!settings.w11Style) radius = std::min(radius, 44.0f * settings.sizeScale);

        DrawSoftShadow(rect, radius);
        DrawPillSurface(rect, radius, settings);

        D2D1_MATRIX_3X2_F oldTransform;
        target_->GetTransform(&oldTransform);
        const D2D1_POINT_2F center = D2D1::Point2F(cx, cy);
        target_->SetTransform(D2D1::Matrix3x2F::Scale(settings.sizeScale, settings.sizeScale, center) * oldTransform);

        const float inv = 1.0f / settings.sizeScale;
        const float uw = (rect.right - rect.left) * inv;
        const float uh = (rect.bottom - rect.top) * inv;
        DrawMedia(state, D2D1::RectF(cx - uw * 0.5f, cy - uh * 0.5f, cx + uw * 0.5f, cy + uh * 0.5f), now);

        target_->SetTransform(oldTransform);
    }

    void PublishContentGeometry(D2D1_RECT_F rect) {
        const unsigned seq = g_mediaHitSeq.load(std::memory_order_relaxed);
        g_mediaHitSeq.store(seq + 1, std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_acquire);
        g_mediaHitLeft.store(rect.left, std::memory_order_relaxed);
        g_mediaHitTop.store(rect.top, std::memory_order_relaxed);
        g_mediaHitRight.store(rect.right, std::memory_order_relaxed);
        g_mediaHitBottom.store(rect.bottom, std::memory_order_relaxed);
        g_mediaHitScale.store(cfg_->sizeScale, std::memory_order_relaxed);
        g_mediaHitStamp.store(GetTickCount64(), std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_release);
        g_mediaHitSeq.store(seq + 2, std::memory_order_relaxed);
    }

    void DrawSystemClock(D2D1_RECT_F rect) {
        if (!clockFormat_) return;
        SYSTEMTIME st;
        GetLocalTime(&st);
        wchar_t buf[16];
        swprintf_s(buf, L"%02d:%02d", st.wHour, st.wMinute);

        textBrush_->SetOpacity(0.92f);
        target_->DrawTextW(buf, static_cast<UINT32>(wcslen(buf)), clockFormat_.Get(), rect, textBrush_.Get());
        textBrush_->SetOpacity(0.98f);
    }

    void DrawMedia(const SharedState& state, D2D1_RECT_F rect, double now) {
        const float height = rect.bottom - rect.top;
        PublishContentGeometry(rect);

        const float radius = ContentIslandRadius(height);
        ComPtr<ID2D1Geometry> mask = CreateIslandMaskGeometry(rect, radius);
        ComPtr<ID2D1Layer> layer;
        target_->CreateLayer(&layer);

        const float expandedAlpha = std::clamp((height - MediaLayout::kExpandedMinHeight) / 60.0f, 0.0f, 1.0f);
        const float collapsedAlpha = std::clamp((80.0f - height) / 30.0f, 0.0f, 1.0f);

        if (expandedAlpha > 0.01f && mask && layer) {
            target_->PushLayer(D2D1::LayerParameters(rect, mask.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                                      D2D1::IdentityMatrix(), expandedAlpha, nullptr,
                                                      D2D1_LAYER_OPTIONS_NONE),
                               layer.Get());
            DrawExpandedMedia(state, rect, now);
            
            if (!g_lockExpanded.load()) {
                DrawGear(rect);
            }

            if (g_moveMode.load() || g_captureHotkey.load()) DrawMoveOverlay(rect, radius);
            target_->PopLayer();
        }

        if (collapsedAlpha > 0.01f && mask && layer) {
            target_->PushLayer(D2D1::LayerParameters(rect, mask.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                                                      D2D1::IdentityMatrix(), collapsedAlpha, nullptr,
                                                      D2D1_LAYER_OPTIONS_NONE),
                               layer.Get());
            const float cy = (rect.top + rect.bottom) * 0.5f;
            const float artPadding = 6.0f;
            const float artSize = height - artPadding * 2.0f;
            DrawAlbumArt(state.media, D2D1::RectF(rect.left + artPadding, cy - artSize * 0.5f,
                                                  rect.left + artPadding + artSize, cy + artSize * 0.5f),
                         artSize * 0.5f, false);

            const D2D1_RECT_F clockRect = D2D1::RectF(rect.right - 56.0f, cy - 12.0f, rect.right - 10.0f, cy + 12.0f);
            DrawSystemClock(clockRect);
            target_->PopLayer();
        }
    }

    static std::wstring FormatTime(double seconds) {
        if (seconds <= 0.0 || std::isnan(seconds)) return L"0:00";
        wchar_t buf[16];
        swprintf_s(buf, L"%d:%02d", static_cast<int>(seconds) / 60, static_cast<int>(seconds) % 60);
        return buf;
    }

    void DrawExpandedMedia(const SharedState& state, D2D1_RECT_F rect, double now) {
        const MediaSnapshot& media = state.media;

        const float artSize = MediaLayout::kArtSize;
        const D2D1_RECT_F artRect = D2D1::RectF(rect.left + MediaLayout::kArtInsetX, rect.top + MediaLayout::kArtInsetY,
                                                rect.left + MediaLayout::kArtInsetX + artSize,
                                                rect.top + MediaLayout::kArtInsetY + artSize);
        DrawAlbumArt(media, artRect, 16.0f, true);

        const float clockW = 50.0f, clockH = 20.0f;
        const D2D1_RECT_F clockRect = D2D1::RectF(rect.right - 24.0f - clockW,
                                                  rect.top + 20.0f + (artSize - clockH) * 0.5f + 8.0f,
                                                  rect.right - 24.0f,
                                                  rect.top + 20.0f + (artSize + clockH) * 0.5f + 8.0f);

        const float textLeft = artRect.right + 18.0f;
        const float textRight = clockRect.left - 16.0f;

        DrawMarqueeText(media.title.empty() ? std::wstring(Tr(L"Невідомо", L"Неизвестно", L"Unknown")) : media.title,
                        D2D1::RectF(textLeft, rect.top + 34.0f, textRight, rect.top + 54.0f), textFormat_.Get(),
                        textBrush_.Get(), now, 42.0f, marqueeTitle_);

        mutedBrush_->SetOpacity(0.80f);
        DrawMarqueeText(media.artist, D2D1::RectF(textLeft, rect.top + 54.0f, textRight, rect.top + 74.0f),
                        smallTextFormat_.Get(), mutedBrush_.Get(), now, 30.0f, marqueeArtist_);
        if (!media.albumTitle.empty()) {
            mutedBrush_->SetOpacity(0.70f);
            DrawMarqueeText(media.albumTitle, D2D1::RectF(textLeft, rect.top + 68.0f, textRight, rect.top + 84.0f),
                            smallTextFormat_.Get(), mutedBrush_.Get(), now, 28.0f, marqueeAlbum_);
        }
        mutedBrush_->SetOpacity(0.90f);

        DrawSystemClock(clockRect);

        const float scrubberY = rect.top + MediaLayout::kScrubberY;
        double position = media.positionTicks / 10000000.0;
        const double duration = media.endTicks / 10000000.0;
        if (media.playing && media.lastUpdatedTicks > 0) {
            position += (GetTickCount64() - media.lastUpdatedTicks) / 1000.0;
        }
        position = std::max(0.0, std::min(position, duration));

        const bool dragging = g_scrubbing.load(std::memory_order_relaxed);
        float progress;
        if (dragging) {
            progress = Clamp(g_scrubDragFraction.load(std::memory_order_relaxed), 0.0f, 1.0f);
            position = duration * progress;
        } else {
            progress = duration > 0.0 ? static_cast<float>(position / duration) : 0.0f;
        }

        const std::wstring elapsed = FormatTime(position);
        const std::wstring remain = L"-" + FormatTime(duration - position);

        const float scrubLeft = rect.left + MediaLayout::kScrubMargin;
        const float scrubRight = rect.right - MediaLayout::kScrubMargin;
        const float barLeft = scrubLeft + MediaLayout::kScrubBarLeftInset;
        const float barRight = scrubRight - MediaLayout::kScrubBarRightInset;
        const float timeGap = 8.0f;

        mutedBrush_->SetOpacity(0.8f);
        if (smallTextFormat_) {
            smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
            target_->DrawTextW(elapsed.c_str(), static_cast<UINT32>(elapsed.size()), smallTextFormat_.Get(),
                               D2D1::RectF(scrubLeft, scrubberY - 10.0f, barLeft - timeGap, scrubberY + 10.0f),
                               mutedBrush_.Get());
            smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            target_->DrawTextW(remain.c_str(), static_cast<UINT32>(remain.size()), smallTextFormat_.Get(),
                               D2D1::RectF(barRight + timeGap, scrubberY - 10.0f, scrubRight, scrubberY + 10.0f),
                               mutedBrush_.Get());
            smallTextFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        }

        const float barHalf = dragging ? 3.5f : 2.5f;
        DrawAccentTrack(D2D1::RectF(barLeft, scrubberY - barHalf, barRight, scrubberY + barHalf), progress, barHalf);

        const float thumbX = barLeft + (barRight - barLeft) * progress;
        const float thumbR = dragging ? 6.5f : 4.5f;
        if (dragging) {
            auto halo = MakeBrush(WithAlpha(currentAccent_, 0.22f));
            target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(thumbX, scrubberY), thumbR * 2.2f, thumbR * 2.2f), halo.Get());
        }
        auto thumbFill = MakeBrush(D2D1::ColorF(1, 1, 1, 0.98f));
        target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(thumbX, scrubberY), thumbR, thumbR), thumbFill.Get());
        auto thumbRing = MakeBrush(WithAlpha(currentAccent_, 0.85f));
        target_->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(thumbX, scrubberY), thumbR, thumbR), thumbRing.Get(), 1.6f);

        const float cy = rect.top + MediaLayout::kControlsY;
        const float cx = (rect.left + rect.right) * 0.5f;
        UpdateMediaButtonAnimations(now);
        DrawMediaButton(D2D1::Point2F(cx - MediaLayout::kControlSpacing, cy), MediaLayout::kNavButtonRadius, 0, false);
        DrawMediaButton(D2D1::Point2F(cx, cy), MediaLayout::kPlayButtonRadius, media.playing ? 1 : 2, true);
        DrawMediaButton(D2D1::Point2F(cx + MediaLayout::kControlSpacing, cy), MediaLayout::kNavButtonRadius, 3, false);
    }

    void DrawAccentTrack(D2D1_RECT_F track, float progress, float radius) {
        auto trackBrush = MakeBrush(material_.raisedStrong);
        target_->FillRoundedRectangle(D2D1::RoundedRect(track, radius, radius), trackBrush.Get());

        const float span = (track.right - track.left) * Clamp(progress, 0.0f, 1.0f);
        if (span <= 0.5f) return;
        const D2D1_RECT_F fillRect = D2D1::RectF(track.left, track.top, track.left + span, track.bottom);

        D2D1_GRADIENT_STOP stops[2] = {};
        stops[0] = {0.0f, WithAlpha(MixColor(material_.accent, D2D1::ColorF(1, 1, 1, 1), 0.30f), 0.95f)};
        stops[1] = {1.0f, WithAlpha(material_.accent, 1.0f)};
        ComPtr<ID2D1GradientStopCollection> coll;
        ComPtr<ID2D1LinearGradientBrush> grad;
        if (SUCCEEDED(target_->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &coll)) &&
            coll &&
            SUCCEEDED(target_->CreateLinearGradientBrush(
                D2D1::LinearGradientBrushProperties(D2D1::Point2F(fillRect.left, fillRect.top),
                                                    D2D1::Point2F(fillRect.right, fillRect.top)),
                coll.Get(), &grad)) && grad) {
            target_->FillRoundedRectangle(D2D1::RoundedRect(fillRect, radius, radius), grad.Get());
        }
    }

    void DrawGear(D2D1_RECT_F rect) {
        const D2D1_POINT_2F c = D2D1::Point2F(rect.right - MediaLayout::kGearInsetRight,
                                              rect.top + MediaLayout::kGearInsetTop);
        const bool hovered = g_hoveredGear.load(std::memory_order_relaxed);
        const float r = MediaLayout::kGearRadius + 3.0f;

        auto bg = MakeBrush(hovered ? material_.raisedStrong : material_.raised);
        target_->FillEllipse(D2D1::Ellipse(c, r, r), bg.Get());

        if (iconFormat_) {
            textBrush_->SetOpacity(hovered ? 1.0f : 0.78f);
            target_->DrawTextW(L"\uE713", 1, iconFormat_.Get(),
                               D2D1::RectF(c.x - r, c.y - r, c.x + r, c.y + r), textBrush_.Get(),
                               D2D1_DRAW_TEXT_OPTIONS_NONE);
            textBrush_->SetOpacity(0.98f);
        }
    }

    void DrawMoveOverlay(D2D1_RECT_F rect, float radius) {
        D2D1_COLOR_F bg = pillBgColor_;
        bg.a = 0.82f;
        auto overlay = MakeBrush(bg);
        FillIslandShape(rect, radius, overlay.Get());

        ComPtr<ID2D1StrokeStyle> dash;
        D2D1_STROKE_STYLE_PROPERTIES props = D2D1::StrokeStyleProperties();
        props.dashStyle = D2D1_DASH_STYLE_DASH;
        props.dashCap = D2D1_CAP_STYLE_ROUND;
        d2dFactory_->CreateStrokeStyle(props, nullptr, 0, &dash);
        auto border = MakeBrush(WithAlpha(currentAccent_, 0.9f));
        DrawIslandShape(D2D1::RectF(rect.left + 5.0f, rect.top + 5.0f, rect.right - 5.0f, rect.bottom - 5.0f),
                        std::max(2.0f, radius - 5.0f), border.Get(), 1.6f, dash.Get());

        const float cy = (rect.top + rect.bottom) * 0.5f;
        if (textFormat_) {
            textFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            textBrush_->SetOpacity(0.98f);
            const bool capture = g_captureHotkey.load();
            const wchar_t* line1 = capture ? Tr(L"Натисніть нову комбінацію клавіш", L"Нажмите новую комбинацию клавиш", L"Press key combination")
                                           : Tr(L"Затисніть ЛКМ для перетягування", L"Зажмите ЛКМ для перетаскивания", L"Hold LMB to drag player");
            target_->DrawTextW(line1, static_cast<UINT32>(wcslen(line1)), textFormat_.Get(),
                               D2D1::RectF(rect.left + 12.0f, cy - 26.0f, rect.right - 12.0f, cy - 2.0f),
                               textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
            textFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        }
        if (smallTextFormat_) {
            smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            const wchar_t* line2 = g_captureHotkey.load()
                ? Tr(L"Потрібен Ctrl/Alt/Shift/Win • Esc — скасування", L"Нужен Ctrl/Alt/Shift/Win • Esc — отмена", L"Needs Ctrl/Alt/Shift/Win • Esc cancels")
                : Tr(L"ПКМ — готово", L"ПКМ — готово", L"Right-click to finish");
            target_->DrawTextW(line2, static_cast<UINT32>(wcslen(line2)), smallTextFormat_.Get(),
                               D2D1::RectF(rect.left + 12.0f, cy + 4.0f, rect.right - 12.0f, cy + 24.0f),
                               mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
            smallTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        }
    }

    void UpdateMediaButtonAnimations(double now) {
        float dt = 0.016f;
        if (lastMediaBtnTime_ > 0.0) dt = static_cast<float>(std::max(0.001, std::min(now - lastMediaBtnTime_, 0.05)));
        lastMediaBtnTime_ = now;

        const int pressed = g_pressedMediaButton.load();
        bool animating = false;
        for (int i = 0; i < 3; ++i) {
            const float t = (pressed == i) ? 1.0f : 0.0f;
            const float tau = (t > mediaBtnPress_[i]) ? 0.025f : 0.075f;
            mediaBtnPress_[i] += (t - mediaBtnPress_[i]) * (1.0f - std::exp(-dt / tau));
            if (std::abs(mediaBtnPress_[i] - t) < 0.002f) mediaBtnPress_[i] = t;
            else animating = true;
        }
        if (animating) g_layoutDirty = true;
    }

    void DrawMediaButton(D2D1_POINT_2F center, float radius, int kind, bool primary) {
        const int cmd = (kind == 0) ? 0 : ((kind == 1 || kind == 2) ? 1 : 2);
        const bool hovered = g_hoveredMediaButton.load() == cmd;
        const float press = g_disableAnimations.load() ? 0.0f : mediaBtnPress_[cmd];
        const float r = radius * (1.0f - 0.13f * press);

        const D2D1_COLOR_F resting = D2D1::ColorF(1, 1, 1, primary ? (hovered ? 0.16f : 0.080f) : (hovered ? 0.09f : 0.040f));
        const D2D1_COLOR_F pressedBg = D2D1::ColorF(currentAccent_.r, currentAccent_.g, currentAccent_.b, primary ? 0.28f : 0.18f);
        auto bg = MakeBrush(MixColor(resting, pressedBg, press));

        const bool isCubic = (cfg_->buttonStyle == ButtonStyle::Cubic);
        if (isCubic) {
            const float cornerRadius = 6.0f;
            D2D1_RECT_F btnRect = D2D1::RectF(center.x - r, center.y - r, center.x + r, center.y + r);
            target_->FillRoundedRectangle(D2D1::RoundedRect(btnRect, cornerRadius, cornerRadius), bg.Get());
        } else {
            target_->FillEllipse(D2D1::Ellipse(center, r, r), bg.Get());
        }

        const float base = primary ? (hovered ? 1.0f : 0.88f) : (hovered ? 0.92f : 0.62f);
        accentBrush_->SetOpacity(Clamp(base + (1.0f - base) * press, 0.0f, 1.0f));

        const wchar_t* glyph = nullptr;
        IDWriteTextFormat* fmt = nullptr;
        static const wchar_t* fluent[] = {L"\uF8AC", L"\uF8AE", L"\uF5B0", L"\uF8AD"};
        static const wchar_t* mdl2[] = {L"\uE100", L"\uE103", L"\uE102", L"\uE101"};
        glyph = usingFluentIcons_ ? fluent[kind] : mdl2[kind];
        fmt = (kind == 1 || kind == 2) ? playIconFormat_.Get() : navIconFormat_.Get();

        if (glyph && fmt) {
            const float offsetX = (kind == 2) ? 1.0f : 0.0f;
            const D2D1_RECT_F glyphRect = D2D1::RectF(center.x - radius + offsetX, center.y - radius,
                                                      center.x + radius + offsetX, center.y + radius);
            D2D1_MATRIX_3X2_F old;
            target_->GetTransform(&old);
            const float s = 1.0f - 0.13f * press;
            target_->SetTransform(D2D1::Matrix3x2F::Scale(s, s, center) * old);
            target_->DrawTextW(glyph, 1, fmt, glyphRect, accentBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            target_->SetTransform(old);
        }
        accentBrush_->SetOpacity(1.0f);
    }

    void DrawBitmapPixels(const BitmapPixels& pixels, D2D1_RECT_F rect, ComPtr<ID2D1Bitmap>& cache,
                          uint64_t& cachedGeneration, float opacity = 1.0f) {
        if (pixels.bgra.empty()) return;
        if (cachedGeneration != pixels.generation || !cache) {
            D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
            cache.Reset();
            target_->CreateBitmap(D2D1::SizeU(pixels.width, pixels.height), pixels.bgra.data(),
                                  pixels.width * 4, &props, &cache);
            cachedGeneration = pixels.generation;
        }
        if (cache) target_->DrawBitmap(cache.Get(), rect, opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
    }

    void DrawCircularBitmapPixels(const BitmapPixels& pixels, D2D1_POINT_2F center, float radius,
                                  ComPtr<ID2D1Bitmap>& cache, uint64_t& gen, float opacity) {
        const D2D1_RECT_F rect = D2D1::RectF(center.x - radius, center.y - radius, center.x + radius, center.y + radius);
        ComPtr<ID2D1EllipseGeometry> ellipse;
        d2dFactory_->CreateEllipseGeometry(D2D1::Ellipse(center, radius, radius), &ellipse);
        ComPtr<ID2D1Layer> layer;
        target_->CreateLayer(nullptr, &layer);
        if (ellipse && layer) {
            target_->PushLayer(D2D1::LayerParameters(rect, ellipse.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE), layer.Get());
            DrawBitmapPixels(pixels, rect, cache, gen, opacity);
            target_->PopLayer();
        } else {
            DrawBitmapPixels(pixels, rect, cache, gen, opacity);
        }
        auto border = MakeBrush(material_.hairline);
        target_->DrawEllipse(D2D1::Ellipse(center, radius, radius), border.Get(), 1.0f);
    }

    void DrawAlbumArt(const MediaSnapshot& media, D2D1_RECT_F rect, float radius, bool drawBadge) {
        ComPtr<ID2D1RoundedRectangleGeometry> mask;
        d2dFactory_->CreateRoundedRectangleGeometry(D2D1::RoundedRect(rect, radius, radius), &mask);
        ComPtr<ID2D1Layer> layer;
        target_->CreateLayer(nullptr, &layer);
        const bool rounded = mask && layer;
        if (rounded) target_->PushLayer(D2D1::LayerParameters(rect, mask.Get()), layer.Get());
        else target_->PushAxisAlignedClip(rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

        if (media.art && !media.art->bgra.empty()) {
            DrawBitmapPixels(*media.art, rect, artBitmap_, artGeneration_, 1.0f);
        } else {
            accentBrush_->SetOpacity(0.24f);
            target_->FillRoundedRectangle(D2D1::RoundedRect(rect, radius, radius), accentBrush_.Get());
            accentBrush_->SetOpacity(1.0f);
            if (media.sourceIcon && !media.sourceIcon->bgra.empty() && (rect.right - rect.left) > 30.0f) {
                DrawBitmapPixels(*media.sourceIcon,
                                  D2D1::RectF(rect.left + 11, rect.top + 11, rect.right - 11, rect.bottom - 11),
                                  sourceIconBitmap_, sourceIconGeneration_, 0.95f);
            } else if (textFormat_) {
                const std::wstring badge = media.sourceBadge.empty() ? L"\u25b6" : media.sourceBadge;
                textFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                textFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                target_->DrawTextW(badge.c_str(), static_cast<UINT32>(badge.size()), textFormat_.Get(), rect, textBrush_.Get());
                textFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                textFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            }
        }

        if (rounded) target_->PopLayer();
        else target_->PopAxisAlignedClip();

        if (drawBadge && media.sourceIcon && !media.sourceIcon->bgra.empty()) {
            const D2D1_RECT_F badge = D2D1::RectF(rect.right - 24, rect.bottom - 22, rect.right - 3, rect.bottom - 3);
            DrawCircularBitmapPixels(*media.sourceIcon,
                                     D2D1::Point2F((badge.left + badge.right) * 0.5f, (badge.top + badge.bottom) * 0.5f),
                                     9.5f, sourceIconBitmap_, sourceIconGeneration_, 0.98f);
        }
    }

    void DrawMarqueeText(const std::wstring& text, D2D1_RECT_F rect, IDWriteTextFormat* format,
                         ID2D1Brush* brush, double now, float speed, MarqueeLayoutCache& cache) {
        if (!format || !brush || text.empty()) return;

        const float wrapHeight = rect.bottom - rect.top;
        if (cache.text != text || cache.format != format || std::fabs(cache.wrapHeight - wrapHeight) > 0.01f ||
            !cache.layout) {
            cache.layout.Reset();
            dwriteFactory_->CreateTextLayout(text.c_str(), static_cast<UINT32>(text.size()), format, 2000.0f,
                                             wrapHeight, &cache.layout);
            cache.text = text;
            cache.format = format;
            cache.wrapHeight = wrapHeight;
            cache.metrics = {};
            if (cache.layout) cache.layout->GetMetrics(&cache.metrics);
        }
        if (!cache.layout) return;

        const float available = rect.right - rect.left;
        D2D1_RECT_F clip = rect;
        clip.top -= 10.0f;
        clip.bottom += 10.0f;
        target_->PushAxisAlignedClip(clip, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        if (cache.metrics.widthIncludingTrailingWhitespace <= available) {
            target_->DrawTextLayout(D2D1::Point2F(rect.left, rect.top), cache.layout.Get(), brush);
        } else {
            const float cycle = cache.metrics.widthIncludingTrailingWhitespace + 38.0f;
            const float offset = std::fmod(static_cast<float>(now) * speed, cycle);
            target_->DrawTextLayout(D2D1::Point2F(rect.left - offset, rect.top), cache.layout.Get(), brush);
            target_->DrawTextLayout(D2D1::Point2F(rect.left - offset + cycle, rect.top), cache.layout.Get(), brush);
        }
        target_->PopAxisAlignedClip();
    }

    HWND hwnd_ = nullptr;
    HDC memDc_ = nullptr;
    HBITMAP dib_ = nullptr;
    HBITMAP oldBitmap_ = nullptr;
    int bitmapWidth_ = 0;
    int bitmapHeight_ = 0;
    const Settings* cfg_ = nullptr;

    ComPtr<ID2D1Factory> d2dFactory_;
    ComPtr<ID2D1DCRenderTarget> target_;
    ComPtr<IDWriteFactory> dwriteFactory_;
    ComPtr<IDWriteTextFormat> textFormat_;
    ComPtr<IDWriteTextFormat> smallTextFormat_;
    ComPtr<IDWriteTextFormat> clockFormat_;
    ComPtr<IDWriteTextFormat> iconFormat_;
    ComPtr<IDWriteTextFormat> playIconFormat_;
    ComPtr<IDWriteTextFormat> navIconFormat_;
    bool usingFluentIcons_ = true;
    ComPtr<ID2D1SolidColorBrush> accentBrush_;
    ComPtr<ID2D1SolidColorBrush> textBrush_;
    ComPtr<ID2D1SolidColorBrush> mutedBrush_;
    MarqueeLayoutCache marqueeTitle_;
    MarqueeLayoutCache marqueeArtist_;
    MarqueeLayoutCache marqueeAlbum_;
    ComPtr<ID2D1Bitmap> artBitmap_;
    ComPtr<ID2D1Bitmap> sourceIconBitmap_;
    uint64_t artGeneration_ = 0;
    uint64_t sourceIconGeneration_ = 0;
    float settingsOpacity_ = 0.96f;
    D2D1_COLOR_F pillBgColor_ = D2D1::ColorF(0.031f, 0.031f, 0.039f, 1.0f);
    Material material_{};
    D2D1_COLOR_F currentAccent_ = D2D1::ColorF(0x4cc9f0);
    double lastAccentTime_ = -1.0;
    float mediaBtnPress_[3] = {0.0f, 0.0f, 0.0f};
    double lastMediaBtnTime_ = -1.0;
    float lastTextScale_ = 0.0f;
    std::wstring lastFontFamily_;
};

void SetClickThrough(HWND hwnd, bool clickThrough) {
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    const bool has = (ex & WS_EX_TRANSPARENT) != 0;
    if (clickThrough == has) return;
    if (clickThrough) ex |= WS_EX_TRANSPARENT;
    else ex &= ~static_cast<LONG_PTR>(WS_EX_TRANSPARENT);
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

std::wstring FormatHotkey(UINT mods, UINT vk) {
    std::wstring s;
    if (mods & MOD_CONTROL) s += L"Ctrl+";
    if (mods & MOD_ALT) s += L"Alt+";
    if (mods & MOD_SHIFT) s += L"Shift+";
    if (mods & MOD_WIN) s += L"Win+";
    LONG sc = static_cast<LONG>(MapVirtualKeyW(vk, MAPVK_VK_TO_VSC)) << 16;
    switch (vk) {
        case VK_LEFT: case VK_RIGHT: case VK_UP: case VK_DOWN: case VK_PRIOR: case VK_NEXT:
        case VK_END: case VK_HOME: case VK_INSERT: case VK_DELETE:
            sc |= (1 << 24);
            break;
        default: break;
    }
    wchar_t name[64] = {};
    if (GetKeyNameTextW(sc, name, ARRAYSIZE(name)) > 0) {
        s += name;
    } else {
        wchar_t buf[16];
        swprintf_s(buf, L"0x%02X", vk);
        s += buf;
    }
    return s;
}

bool g_hotkeyRegistered[5] = {false};

void PersistHotkeys() {
    Wh_SetIntValue(L"HotkeyEnabled", g_hotkeyEnabled.load() ? 1 : 0);
    Wh_SetIntValue(L"HotkeyMods", static_cast<int>(g_hotkeyMods.load()));
    Wh_SetIntValue(L"HotkeyVk", static_cast<int>(g_hotkeyVk.load()));
    Wh_SetIntValue(L"HkNextMods", static_cast<int>(g_hkNextMods.load()));
    Wh_SetIntValue(L"HkNextVk", static_cast<int>(g_hkNextVk.load()));
    Wh_SetIntValue(L"HkPrevMods", static_cast<int>(g_hkPrevMods.load()));
    Wh_SetIntValue(L"HkPrevVk", static_cast<int>(g_hkPrevVk.load()));
    Wh_SetIntValue(L"HkPlayPauseMods", static_cast<int>(g_hkPlayPauseMods.load()));
    Wh_SetIntValue(L"HkPlayPauseVk", static_cast<int>(g_hkPlayPauseVk.load()));
}

void RegisterSingleHotkey(HWND hwnd, int id, unsigned mods, unsigned vk) {
    if (g_hotkeyRegistered[id]) {
        UnregisterHotKey(hwnd, id);
        g_hotkeyRegistered[id] = false;
    }
    if (g_hotkeyEnabled.load() && vk != 0) {
        if (RegisterHotKey(hwnd, id, mods | MOD_NOREPEAT, vk)) {
            g_hotkeyRegistered[id] = true;
        }
    }
}

void ApplyHideShowHotkey(HWND hwnd) {
    if (!hwnd) return;
    RegisterSingleHotkey(hwnd, kHotkeyHideShow, g_hotkeyMods.load(), g_hotkeyVk.load());
    RegisterSingleHotkey(hwnd, kHotkeyNextTrack, g_hkNextMods.load(), g_hkNextVk.load());
    RegisterSingleHotkey(hwnd, kHotkeyPrevTrack, g_hkPrevMods.load(), g_hkPrevVk.load());
    RegisterSingleHotkey(hwnd, kHotkeyPlayPause, g_hkPlayPauseMods.load(), g_hkPlayPauseVk.load());
}

bool g_capCtrl = false, g_capAlt = false, g_capShift = false, g_capWin = false;

LRESULT CALLBACK CaptureKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && g_captureHotkey.load()) {
        const auto* k = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
        const bool down = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        const DWORD vk = k->vkCode;
        bool isModifier = true;
        switch (vk) {
            case VK_CONTROL: case VK_LCONTROL: case VK_RCONTROL: g_capCtrl = down; break;
            case VK_MENU: case VK_LMENU: case VK_RMENU: g_capAlt = down; break;
            case VK_SHIFT: case VK_LSHIFT: case VK_RSHIFT: g_capShift = down; break;
            case VK_LWIN: case VK_RWIN: g_capWin = down; break;
            default: isModifier = false; break;
        }
        if (!isModifier && down) {
            if (vk == VK_ESCAPE) {
                g_captureHotkey = false;
            } else {
                const UINT mods = (g_capCtrl ? MOD_CONTROL : 0) | (g_capAlt ? MOD_ALT : 0) |
                                  (g_capShift ? MOD_SHIFT : 0) | (g_capWin ? MOD_WIN : 0);
                if (mods != 0) {
                    const int target = g_captureHotkeyTarget.load();
                    if (target == 1) { g_hotkeyMods = mods; g_hotkeyVk = vk; }
                    else if (target == 2) { g_hkNextMods = mods; g_hkNextVk = vk; }
                    else if (target == 3) { g_hkPrevMods = mods; g_hkPrevVk = vk; }
                    else if (target == 4) { g_hkPlayPauseMods = mods; g_hkPlayPauseVk = vk; }
                    
                    g_hotkeyEnabled = true;
                    PersistHotkeys();
                    g_captureHotkey = false;
                    HWND hSelf = g_hwnd.load();
                    if (hSelf) PostMessageW(hSelf, WM_APP_APPLY_HOTKEY, 0, 0);
                }
            }
        }
        return 1;
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void StartHotkeyCapture(int target) {
    if (g_captureHook) return;
    g_captureHotkeyTarget = target;
    g_capCtrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    g_capAlt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    g_capShift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    g_capWin = (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0 || (GetAsyncKeyState(VK_RWIN) & 0x8000) != 0;
    g_captureHook = SetWindowsHookExW(WH_KEYBOARD_LL, CaptureKeyboardProc, nullptr, 0);
    if (g_captureHook) {
        g_captureHotkey = true;
        g_captureDeadline = NowSeconds() + 10.0;
    }
}

void StopHotkeyCaptureIfDone() {
    if (!g_captureHook) return;
    if (g_captureHotkey.load() && NowSeconds() > g_captureDeadline.load()) g_captureHotkey = false;
    if (!g_captureHotkey.load()) {
        UnhookWindowsHookEx(g_captureHook);
        g_captureHook = nullptr;
        g_captureHotkeyTarget = 0;
        g_layoutDirty = true;
    }
}

enum MenuId : UINT {
    ID_TOPMOST = 1,
    ID_GAMES,
    ID_HIDE_FS,
    ID_TASKBAR,
    ID_AUTOEXPAND,
    ID_LOCK_EXPANDED,
    ID_CONFIRM_TRACK_CHANGE,
    ID_OPACITY_100,
    ID_OPACITY_90,
    ID_OPACITY_80,
    ID_OPACITY_70,
    ID_OPACITY_60,
    ID_OPACITY_50,
    ID_OPACITY_40,
    ID_MOVE,
    ID_RESET,
    ID_SET_APP_POS,
    ID_RESET_APP_POS,
    ID_CLICKTHROUGH,
    ID_DISABLE_ANIMATIONS,
    ID_DISABLE_SHADOW,
    ID_HOTKEY_ENABLE_TOGGLE,
    ID_HOTKEY_HIDESHOW_SET,
    ID_HOTKEY_NEXT_SET,
    ID_HOTKEY_PREV_SET,
    ID_HOTKEY_PLAYPAUSE_SET,
    ID_IDLE_NEVER,
    ID_IDLE_30S,
    ID_IDLE_1M,
    ID_IDLE_5M,
    ID_IDLE_15M,
    ID_IDLE_30M,
    ID_IDLE_1H,
    ID_IDLE_2H,

    ID_CUSTOMIZATION_SUBMENU = 2000,
    ID_COLOR_SETTINGS_SUBMENU,
    ID_COLOR_RAINBOW_SUBMENU,
    ID_BUTTON_STYLE_SUBMENU,

    ID_SET_COLOR_ACCENT,
    ID_SET_COLOR_BACKGROUND,
    ID_SET_COLOR_TEXT_PRIMARY,
    ID_SET_COLOR_TEXT_SECONDARY,

    ID_TOGGLE_RAINBOW,
    ID_RAINBOW_SPEED_SLOW,
    ID_RAINBOW_SPEED_NORMAL,
    ID_RAINBOW_SPEED_FAST,
    ID_ADD_RAINBOW_COLOR,
    ID_RESET_RAINBOW_COLORS,

    ID_BUTTON_STYLE_DEFAULT,
    ID_BUTTON_STYLE_CUBIC,

    ID_LANGUAGE_SUBMENU = 3000,
    ID_LANG_SYSTEM,
    ID_LANG_UKRAINIAN,
    ID_LANG_RUSSIAN,
    ID_LANG_ENGLISH,
    
    ID_RESET_OVERRIDES,
};

void ShowSettingsMenu(HWND hwnd, POINT screenPoint) {
    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING | (g_alwaysOnTop.load() ? MF_CHECKED : 0), ID_TOPMOST,
                Tr(L"Поверх усіх вікон", L"Поверх всех окон", L"Always on top"));
    AppendMenuW(menu, MF_STRING | (g_overGames.load() ? MF_CHECKED : 0), ID_GAMES,
                Tr(L"Поверх ігор (повноекранних додатків)", L"Поверх игр (полноэкранных приложений)", L"Above games (fullscreen apps)"));
    AppendMenuW(menu, MF_STRING | (g_hideInFullscreen.load() ? MF_CHECKED : 0), ID_HIDE_FS,
                Tr(L"Ховати при повному екрані відео/браузера", L"Скрывать при полном экране видео/браузера", L"Hide in browser/video fullscreen"));
    AppendMenuW(menu, MF_STRING | (g_overTaskbar.load() ? MF_CHECKED : 0), ID_TASKBAR,
                Tr(L"Поверх панелі завдань та трею", L"Поверх панели задач и трея", L"Above taskbar and tray"));
    AppendMenuW(menu, MF_STRING | (g_lockExpanded.load() ? MF_CHECKED : 0), ID_LOCK_EXPANDED,
                Tr(L"Зафіксувати розгорнутим", L"Зафиксировать развёрнутым", L"Keep expanded"));
    AppendMenuW(menu, MF_STRING | (g_autoExpand.load() ? MF_CHECKED : 0), ID_AUTOEXPAND,
                Tr(L"Розгортати при зміні треку", L"Раскрывать при смене трека", L"Expand on track change"));

    HMENU idleMenu = CreatePopupMenu();
    const int curIdle = g_idleTimeoutSec.load();
    auto addIdleItem = [&](UINT id, int sec, const wchar_t* uk, const wchar_t* ru, const wchar_t* en) {
        AppendMenuW(idleMenu, MF_STRING | (curIdle == sec ? MF_CHECKED : 0), id, Tr(uk, ru, en));
    };
    addIdleItem(ID_IDLE_NEVER, 0,    L"Ніколи", L"Никогда", L"Never");
    addIdleItem(ID_IDLE_30S,   30,   L"Через 30 секунд", L"Через 30 секунд", L"After 30 seconds");
    addIdleItem(ID_IDLE_1M,    60,   L"Через 1 хвилину", L"Через 1 минуту", L"After 1 minute");
    addIdleItem(ID_IDLE_5M,    300,  L"Через 5 хвилин", L"Через 5 минут", L"After 5 minutes");
    addIdleItem(ID_IDLE_15M,   900,  L"Через 15 хвилин", L"Через 15 минут", L"After 15 minutes");
    addIdleItem(ID_IDLE_30M,   1800, L"Через 30 хвилин", L"Через 30 минут", L"After 30 minutes");
    addIdleItem(ID_IDLE_1H,    3600, L"Через 1 годину", L"Через 1 час", L"After 1 hour");
    addIdleItem(ID_IDLE_2H,    7200, L"Через 2 години", L"Через 2 часа", L"After 2 hours");
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(idleMenu), Tr(L"Ховати при паузі", L"Скрывать при паузе", L"Hide when paused"));

    AppendMenuW(menu, MF_STRING | (g_confirmTrackChange.load() ? MF_CHECKED : 0), ID_CONFIRM_TRACK_CHANGE,
                Tr(L"Підтверджувати перемикання треків (подвійним кліком)", L"Подтверждать переключение треков (двойным кликом)", L"Confirm track changes (double click)"));

    HMENU opacityMenu = CreatePopupMenu();
    const int curOpacity = (g_menuOpacityOverride.load() != -1) 
                           ? g_menuOpacityOverride.load() 
                           : static_cast<int>(std::round(GetSettingsCopy().pillOpacity * 100.0f));
    auto addOpacityItem = [&](UINT id, int pct, const wchar_t* label) {
        bool isChecked = (std::abs(curOpacity - pct) <= 4);
        AppendMenuW(opacityMenu, MF_STRING | (isChecked ? MF_CHECKED : 0), id, label);
    };
    addOpacityItem(ID_OPACITY_100, 100, L"100%");
    addOpacityItem(ID_OPACITY_90,  90,  L"90%");
    addOpacityItem(ID_OPACITY_80,  80,  L"80%");
    addOpacityItem(ID_OPACITY_70,  70,  L"70%");
    addOpacityItem(ID_OPACITY_60,  60,  L"60%");
    addOpacityItem(ID_OPACITY_50,  50,  L"50%");
    addOpacityItem(ID_OPACITY_40,  40,  L"40%");

    HMENU colorMenu = CreatePopupMenu();
    AppendMenuW(colorMenu, MF_STRING, ID_SET_COLOR_ACCENT, Tr(L"Колір акценту...", L"Цвет акцента...", L"Accent color..."));
    AppendMenuW(colorMenu, MF_STRING, ID_SET_COLOR_BACKGROUND, Tr(L"Колір фону...", L"Цвет фона...", L"Background color..."));
    AppendMenuW(colorMenu, MF_STRING, ID_SET_COLOR_TEXT_PRIMARY, Tr(L"Основний текст...", L"Основной текст...", L"Primary text..."));
    AppendMenuW(colorMenu, MF_STRING, ID_SET_COLOR_TEXT_SECONDARY, Tr(L"Вторинний текст...", L"Вторичный текст...", L"Secondary text..."));

    HMENU rainbowMenu = CreatePopupMenu();
    AppendMenuW(rainbowMenu, MF_STRING | (g_colorRainbowEnabled.load() ? MF_CHECKED : 0), ID_TOGGLE_RAINBOW, Tr(L"Увімкнути переливання", L"Включить переливание", L"Enable morphing"));
    AppendMenuW(rainbowMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(rainbowMenu, MF_STRING, ID_ADD_RAINBOW_COLOR, Tr(L"+ Додати колір у палітру...", L"+ Добавить цвет в палитру...", L"+ Add color to palette..."));
    AppendMenuW(rainbowMenu, MF_STRING, ID_RESET_RAINBOW_COLORS, Tr(L"Скинути палітуру переливання", L"Сбросить палитру перелива", L"Reset morphing palette"));
    AppendMenuW(rainbowMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(rainbowMenu, MF_STRING | (g_colorRainbowSpeed.load() == 0.5f ? MF_CHECKED : 0), ID_RAINBOW_SPEED_SLOW, Tr(L"Повільно", L"Медленно", L"Slow"));
    AppendMenuW(rainbowMenu, MF_STRING | (g_colorRainbowSpeed.load() == 1.0f ? MF_CHECKED : 0), ID_RAINBOW_SPEED_NORMAL, Tr(L"Звичайна", L"Обычная", L"Normal"));
    AppendMenuW(rainbowMenu, MF_STRING | (g_colorRainbowSpeed.load() == 2.0f ? MF_CHECKED : 0), ID_RAINBOW_SPEED_FAST, Tr(L"Швидко", L"Быстро", L"Fast"));

    HMENU buttonStyleMenu = CreatePopupMenu();
    const bool isCubic = (GetSettingsCopy().buttonStyle == ButtonStyle::Cubic);
    AppendMenuW(buttonStyleMenu, MF_STRING | (!isCubic ? MF_CHECKED : 0), ID_BUTTON_STYLE_DEFAULT, Tr(L"Стандартний", L"Стандартный", L"Default"));
    AppendMenuW(buttonStyleMenu, MF_STRING | (isCubic ? MF_CHECKED : 0), ID_BUTTON_STYLE_CUBIC, Tr(L"Кубічний", L"Кубический", L"Cubic"));

    HMENU langMenu = CreatePopupMenu();
    const LanguageSetting curLang = g_language.load();
    AppendMenuW(langMenu, MF_STRING | (curLang == LanguageSetting::System ? MF_CHECKED : 0), ID_LANG_SYSTEM, L"Авто (Системна / System)");
    AppendMenuW(langMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(langMenu, MF_STRING | (curLang == LanguageSetting::Ukrainian ? MF_CHECKED : 0), ID_LANG_UKRAINIAN, L"Українська");
    AppendMenuW(langMenu, MF_STRING | (curLang == LanguageSetting::Russian ? MF_CHECKED : 0), ID_LANG_RUSSIAN, L"Русский");
    AppendMenuW(langMenu, MF_STRING | (curLang == LanguageSetting::English ? MF_CHECKED : 0), ID_LANG_ENGLISH, L"English");

    HMENU customMenu = CreatePopupMenu();
    AppendMenuW(customMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(colorMenu), Tr(L"Налаштування кольорів", L"Настройка цветов", L"Color settings"));
    AppendMenuW(customMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(rainbowMenu), Tr(L"Переливання кольорів", L"Переливание цветов", L"Color morphing"));
    AppendMenuW(customMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(buttonStyleMenu), Tr(L"Стиль кнопок", L"Стиль кнопок", L"Button style"));
    AppendMenuW(customMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(langMenu), Tr(L"Мова", L"Язык", L"Language"));
    AppendMenuW(customMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(customMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(opacityMenu), Tr(L"Прозорість вікна", L"Прозрачность окна", L"Window opacity"));
    AppendMenuW(customMenu, MF_STRING | (g_disableAnimations.load() ? MF_CHECKED : 0), ID_DISABLE_ANIMATIONS, Tr(L"Вимкнути анімації", L"Отключить анимации", L"Disable animations"));
    AppendMenuW(customMenu, MF_STRING | (g_disableShadow.load() ? MF_CHECKED : 0), ID_DISABLE_SHADOW, Tr(L"Вимкнути тінь", L"Отключить тень", L"Disable shadow"));
    AppendMenuW(customMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(customMenu, MF_STRING, ID_RESET_OVERRIDES, Tr(L"Скинути налаштування меню (до Windhawk)", L"Сбросить настройки меню (к Windhawk)", L"Reset menu overrides (to Windhawk)"));

    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(customMenu), Tr(L"Кастомізація", L"Кастомизация", L"Customization"));

    AppendMenuW(menu, MF_STRING | (g_clickThrough.load() ? MF_CHECKED : 0), ID_CLICKTHROUGH,
                Tr(L"Прозорий для натискань (Ctrl — взаємодія)", L"Прозрачный для нажатий (Ctrl — управлять)", L"Click-through (hold Ctrl to interact)"));

    HMENU hotkeyMenu = CreatePopupMenu();
    const bool hkOn = g_hotkeyEnabled.load();
    AppendMenuW(hotkeyMenu, MF_STRING | (hkOn ? MF_CHECKED : 0), ID_HOTKEY_ENABLE_TOGGLE, 
                Tr(L"Увімкнути гарячі клавіші", L"Включить горячие клавиши", L"Enable hotkeys"));
    AppendMenuW(hotkeyMenu, MF_SEPARATOR, 0, nullptr);

    UINT hkFlags = hkOn ? MF_STRING : (MF_STRING | MF_GRAYED);
    std::wstring hsLabel = std::wstring(Tr(L"Сховати / показати: ", L"Скрыть / показать: ", L"Hide / show: ")) + FormatHotkey(g_hotkeyMods.load(), g_hotkeyVk.load());
    AppendMenuW(hotkeyMenu, hkFlags, ID_HOTKEY_HIDESHOW_SET, hsLabel.c_str());

    std::wstring nextLabel = std::wstring(Tr(L"Наступний трек: ", L"Следующий трек: ", L"Next track: ")) + FormatHotkey(g_hkNextMods.load(), g_hkNextVk.load());
    AppendMenuW(hotkeyMenu, hkFlags, ID_HOTKEY_NEXT_SET, nextLabel.c_str());

    std::wstring prevLabel = std::wstring(Tr(L"Попередній трек: ", L"Предыдущий трек: ", L"Previous track: ")) + FormatHotkey(g_hkPrevMods.load(), g_hkPrevVk.load());
    AppendMenuW(hotkeyMenu, hkFlags, ID_HOTKEY_PREV_SET, prevLabel.c_str());

    std::wstring ppLabel = std::wstring(Tr(L"Пауза / Відтворення: ", L"Пауза / Воспроизведение: ", L"Play / Pause: ")) + FormatHotkey(g_hkPlayPauseMods.load(), g_hkPlayPauseVk.load());
    AppendMenuW(hotkeyMenu, hkFlags, ID_HOTKEY_PLAYPAUSE_SET, ppLabel.c_str());

    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(hotkeyMenu), Tr(L"Гарячі клавіші", L"Бинды", L"Hotkeys"));

    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING | (g_moveMode.load() ? MF_CHECKED : 0), ID_MOVE,
                g_moveMode.load() ? Tr(L"Готово (завершити переміщення)", L"Готово (закончить перемещение)", L"Done moving")
                                  : Tr(L"Змінити позицію плеєра", L"Изменить положение плеера", L"Change player position"));

    std::wstring fgExe = GetForegroundExeName();
    bool hasAppPos = false;
    if (!fgExe.empty()) {
        std::lock_guard lock(g_appPosMutex);
        hasAppPos = (g_appPositions.find(fgExe) != g_appPositions.end());
    }

    if (!fgExe.empty()) {
        std::wstring appTitle = Tr(L"Прив'язати позицію до ", L"Привязать позицию к ", L"Set position for ") + fgExe;
        AppendMenuW(menu, MF_STRING, ID_SET_APP_POS, appTitle.c_str());

        if (hasAppPos) {
            std::wstring resetAppTitle = Tr(L"Скинути позицію для ", L"Сбросить позицию для ", L"Reset position for ") + fgExe;
            AppendMenuW(menu, MF_STRING, ID_RESET_APP_POS, resetAppTitle.c_str());
        }
    }

    AppendMenuW(menu, MF_STRING | (g_customPosSet.load() ? 0 : MF_GRAYED), ID_RESET,
                Tr(L"Скинути базову позицію", L"Сбросить базовое положение", L"Reset default position"));

    SetForegroundWindow(hwnd);
    const UINT cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, screenPoint.x, screenPoint.y, 0, hwnd, nullptr);
    PostMessageW(hwnd, WM_NULL, 0, 0);
    DestroyMenu(menu);

    auto setOpacityVal = [](int val) {
        g_menuOpacityOverride = val;
        Wh_SetIntValue(L"MenuOpacityOverride", val);
        LoadSettings();
    };

    auto setIdleVal = [](int sec) {
        g_idleTimeoutSec = sec;
        Wh_SetIntValue(L"IdleTimeoutSec", sec);
        LoadSettings();
    };
    
    auto saveColorOverride = [](PCWSTR key, D2D1_COLOR_F c) {
        wchar_t hex[32];
        swprintf_s(hex, L"#%02X%02X%02X", (int)(c.r * 255), (int)(c.g * 255), (int)(c.b * 255));
        Wh_SetStringValue(key, hex);
    };

    switch (cmd) {
        case ID_TOPMOST:
            g_alwaysOnTop = !g_alwaysOnTop.load();
            Wh_SetIntValue(L"AlwaysOnTop", g_alwaysOnTop.load() ? 1 : 0);
            break;
        case ID_GAMES:
            g_overGames = !g_overGames.load();
            Wh_SetIntValue(L"OverGames", g_overGames.load() ? 1 : 0);
            break;
        case ID_HIDE_FS:
            g_hideInFullscreen = !g_hideInFullscreen.load();
            Wh_SetIntValue(L"HideInFullscreen", g_hideInFullscreen.load() ? 1 : 0);
            LoadSettings();
            break;
        case ID_TASKBAR:
            g_overTaskbar = !g_overTaskbar.load();
            Wh_SetIntValue(L"OverTaskbar", g_overTaskbar.load() ? 1 : 0);
            break;
        case ID_LOCK_EXPANDED:
            g_lockExpanded = !g_lockExpanded.load();
            Wh_SetIntValue(L"LockExpanded", g_lockExpanded.load() ? 1 : 0);
            break;
        case ID_AUTOEXPAND:
            g_autoExpand = !g_autoExpand.load();
            Wh_SetIntValue(L"AutoExpand", g_autoExpand.load() ? 1 : 0);
            break;
        case ID_CONFIRM_TRACK_CHANGE:
            g_confirmTrackChange = !g_confirmTrackChange.load();
            Wh_SetIntValue(L"ConfirmTrackChange", g_confirmTrackChange.load() ? 1 : 0);
            break;
        case ID_IDLE_NEVER: setIdleVal(0); break;
        case ID_IDLE_30S:   setIdleVal(30); break;
        case ID_IDLE_1M:    setIdleVal(60); break;
        case ID_IDLE_5M:    setIdleVal(300); break;
        case ID_IDLE_15M:   setIdleVal(900); break;
        case ID_IDLE_30M:   setIdleVal(1800); break;
        case ID_IDLE_1H:    setIdleVal(3600); break;
        case ID_IDLE_2H:    setIdleVal(7200); break;
        case ID_DISABLE_ANIMATIONS:
            g_disableAnimations = !g_disableAnimations.load();
            Wh_SetIntValue(L"DisableAnimations", g_disableAnimations.load() ? 1 : 0);
            break;
        case ID_DISABLE_SHADOW:
            g_disableShadow = !g_disableShadow.load();
            Wh_SetIntValue(L"DisableShadow", g_disableShadow.load() ? 1 : 0);
            break;
        case ID_OPACITY_100: setOpacityVal(100); break;
        case ID_OPACITY_90:  setOpacityVal(90);  break;
        case ID_OPACITY_80:  setOpacityVal(80);  break;
        case ID_OPACITY_70:  setOpacityVal(70);  break;
        case ID_OPACITY_60:  setOpacityVal(60);  break;
        case ID_OPACITY_50:  setOpacityVal(50);  break;
        case ID_OPACITY_40:  setOpacityVal(40);  break;
        case ID_SET_COLOR_ACCENT: {
            Settings s = GetSettingsCopy();
            if (PickColorWithDialog(hwnd, s.customAccent)) {
                Wh_SetIntValue(L"OverrideAccentMode", static_cast<int>(AccentMode::Custom));
                saveColorOverride(L"OverrideCustomAccent", s.customAccent);
                LoadSettings();
            }
            break;
        }
        case ID_SET_COLOR_BACKGROUND: {
            Settings s = GetSettingsCopy();
            if (PickColorWithDialog(hwnd, s.pillBgColor)) {
                saveColorOverride(L"OverrideBgColor", s.pillBgColor);
                LoadSettings();
            }
            break;
        }
        case ID_SET_COLOR_TEXT_PRIMARY: {
            Settings s = GetSettingsCopy();
            if (PickColorWithDialog(hwnd, s.textPrimaryColor)) {
                saveColorOverride(L"OverrideTextPrimary", s.textPrimaryColor);
                LoadSettings();
            }
            break;
        }
        case ID_SET_COLOR_TEXT_SECONDARY: {
            Settings s = GetSettingsCopy();
            if (PickColorWithDialog(hwnd, s.textSecondaryColor)) {
                saveColorOverride(L"OverrideTextSecondary", s.textSecondaryColor);
                LoadSettings();
            }
            break;
        }
        case ID_TOGGLE_RAINBOW:
            g_colorRainbowEnabled = !g_colorRainbowEnabled.load();
            Wh_SetIntValue(L"ColorRainbowEnabled", g_colorRainbowEnabled.load() ? 1 : 0);
            g_layoutDirty = true;
            break;
        case ID_ADD_RAINBOW_COLOR: {
            D2D1_COLOR_F newCol = D2D1::ColorF(0xff0000);
            if (PickColorWithDialog(hwnd, newCol)) {
                {
                    std::lock_guard lock(g_rainbowMutex);
                    g_rainbowPalette.push_back(newCol);
                }
                SaveRainbowPalette();
                g_layoutDirty = true;
            }
            break;
        }
        case ID_RESET_RAINBOW_COLORS: {
            {
                std::lock_guard lock(g_rainbowMutex);
                g_rainbowPalette = { D2D1::ColorF(0x4cc9f0), D2D1::ColorF(0x7209b7), D2D1::ColorF(0xf72585) };
            }
            SaveRainbowPalette();
            g_layoutDirty = true;
            break;
        }
        case ID_RAINBOW_SPEED_SLOW:   g_colorRainbowSpeed = 0.5f; Wh_SetIntValue(L"RainbowSpeed", 5); break;
        case ID_RAINBOW_SPEED_NORMAL: g_colorRainbowSpeed = 1.0f; Wh_SetIntValue(L"RainbowSpeed", 10); break;
        case ID_RAINBOW_SPEED_FAST:   g_colorRainbowSpeed = 2.0f; Wh_SetIntValue(L"RainbowSpeed", 20); break;
        case ID_BUTTON_STYLE_DEFAULT:
            Wh_SetIntValue(L"ButtonStyle", 0);
            LoadSettings();
            break;
        case ID_BUTTON_STYLE_CUBIC:
            Wh_SetIntValue(L"ButtonStyle", 1);
            LoadSettings();
            break;
        case ID_LANG_SYSTEM:
            g_language = LanguageSetting::System;
            Wh_SetIntValue(L"LanguageSetting", 0);
            g_layoutDirty = true;
            break;
        case ID_LANG_UKRAINIAN:
            g_language = LanguageSetting::Ukrainian;
            Wh_SetIntValue(L"LanguageSetting", 1);
            g_layoutDirty = true;
            break;
        case ID_LANG_RUSSIAN:
            g_language = LanguageSetting::Russian;
            Wh_SetIntValue(L"LanguageSetting", 2);
            g_layoutDirty = true;
            break;
        case ID_LANG_ENGLISH:
            g_language = LanguageSetting::English;
            Wh_SetIntValue(L"LanguageSetting", 3);
            g_layoutDirty = true;
            break;
        case ID_RESET_OVERRIDES:
            Wh_DeleteValue(L"OverrideAccentMode");
            Wh_DeleteValue(L"OverrideCustomAccent");
            Wh_DeleteValue(L"OverrideBgColor");
            Wh_DeleteValue(L"OverrideTextPrimary");
            Wh_DeleteValue(L"OverrideTextSecondary");
            Wh_DeleteValue(L"MenuOpacityOverride");
            Wh_DeleteValue(L"HideInFullscreen");
            Wh_DeleteValue(L"IdleTimeoutSec");
            Wh_SetIntValue(L"MenuOpacityOverride", -1);
            Wh_SetIntValue(L"HideInFullscreen", -1);
            Wh_SetIntValue(L"IdleTimeoutSec", -1);
            LoadSettings();
            break;
        case ID_MOVE:
            g_moveMode = !g_moveMode.load();
            break;
        case ID_SET_APP_POS:
            if (!fgExe.empty()) {
                RECT r = {};
                GetWindowRect(hwnd, &r);
                int cx = (r.left + r.right) / 2;
                int cy = r.top;
                {
                    std::lock_guard lock(g_appPosMutex);
                    g_appPositions[fgExe] = {cx, cy};
                }
                SaveAppPositionsToRegistry();
                g_moveMode = true;
            }
            break;
        case ID_RESET_APP_POS:
            if (!fgExe.empty()) {
                {
                    std::lock_guard lock(g_appPosMutex);
                    g_appPositions.erase(fgExe);
                }
                SaveAppPositionsToRegistry();
            }
            break;
        case ID_RESET:
            g_customPosSet = false;
            Wh_SetIntValue(L"CustomPosSet", 0);
            break;
        case ID_CLICKTHROUGH:
            g_clickThrough = !g_clickThrough.load();
            Wh_SetIntValue(L"ClickThrough", g_clickThrough.load() ? 1 : 0);
            break;
        case ID_HOTKEY_ENABLE_TOGGLE:
            g_hotkeyEnabled = !g_hotkeyEnabled.load();
            PersistHotkeys();
            ApplyHideShowHotkey(hwnd);
            break;
        case ID_HOTKEY_HIDESHOW_SET:
            if (g_hotkeyEnabled.load()) { g_moveMode = false; StartHotkeyCapture(1); }
            break;
        case ID_HOTKEY_NEXT_SET:
            if (g_hotkeyEnabled.load()) { g_moveMode = false; StartHotkeyCapture(2); }
            break;
        case ID_HOTKEY_PREV_SET:
            if (g_hotkeyEnabled.load()) { g_moveMode = false; StartHotkeyCapture(3); }
            break;
        case ID_HOTKEY_PLAYPAUSE_SET:
            if (g_hotkeyEnabled.load()) { g_moveMode = false; StartHotkeyCapture(4); }
            break;
        default:
            return;
    }
    g_layoutDirty = true;
}

void SaveCustomPositionFromWindow(HWND hwnd) {
    RECT r = {};
    if (!GetWindowRect(hwnd, &r)) return;
    int cX = (r.left + r.right) / 2;
    int cY = r.top;

    std::wstring fgExe = GetForegroundExeName();
    bool savedForApp = false;

    if (!fgExe.empty()) {
        std::lock_guard lock(g_appPosMutex);
        auto it = g_appPositions.find(fgExe);
        if (it != g_appPositions.end()) {
            it->second = {cX, cY};
            savedForApp = true;
        }
    }

    if (savedForApp) {
        SaveAppPositionsToRegistry();
    } else {
        g_customCenterX = cX;
        g_customTop = cY;
        g_customPosSet = true;
        Wh_SetIntValue(L"CustomPosSet", 1);
        Wh_SetIntValue(L"CustomPosX", g_customCenterX.load());
        Wh_SetIntValue(L"CustomPosY", g_customTop.load());
    }
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static POINT s_dragCursor = {0, 0};
    static POINT s_dragWindow = {0, 0};

    switch (msg) {
        case WM_DPICHANGED: {
            RECT* prcNewWindow = reinterpret_cast<RECT*>(lParam);
            SetWindowPos(hwnd, nullptr, prcNewWindow->left, prcNewWindow->top,
                         prcNewWindow->right - prcNewWindow->left,
                         prcNewWindow->bottom - prcNewWindow->top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            g_layoutDirty = true;
            return 0;
        }
        
        case WM_DISPLAYCHANGE: {
            g_layoutDirty = true;
            return 0;
        }
        
        case WM_NCHITTEST: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ScreenToClient(hwnd, &pt);
            RECT cr = {};
            GetClientRect(hwnd, &cr);
            const int minX = static_cast<int>(kRenderPadX);
            const int maxX = cr.right - static_cast<int>(kRenderPadX);
            const int minY = g_flushTop.load() ? 0 : static_cast<int>(kRenderPadY);
            const int maxY = cr.bottom - static_cast<int>(kRenderPadY);
            if (maxX > minX && maxY > minY &&
                (pt.x < minX || pt.x > maxX || pt.y < minY || pt.y > maxY)) {
                return HTTRANSPARENT;
            }
            return HTCLIENT;
        }

        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT) {
                if (g_moveMode.load()) {
                    SetCursor(LoadCursorW(nullptr, IDC_SIZEALL));
                    return TRUE;
                }
                if (g_scrubbing.load()) {
                    SetCursor(LoadCursorW(nullptr, IDC_HAND));
                    return TRUE;
                }
                POINT pt;
                GetCursorPos(&pt);
                ScreenToClient(hwnd, &pt);
                const MediaContentPoint cp = MediaContentFromClient(pt.x, pt.y);
                if (MediaArtHitTest(cp) || MediaTransportHitTest(cp) != -1 ||
                    MediaScrubFractionFromContent(cp) >= 0.0f || MediaGearHitTest(cp)) {
                    SetCursor(LoadCursorW(nullptr, IDC_HAND));
                    return TRUE;
                }
            }
            break;

        case WM_LBUTTONDOWN: {
            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);

            if (g_moveMode.load()) {
                GetCursorPos(&s_dragCursor);
                RECT wr = {};
                GetWindowRect(hwnd, &wr);
                s_dragWindow = {wr.left, wr.top};
                g_dragging = true;
                SetCapture(hwnd);
                return 0;
            }

            const MediaContentPoint cp = MediaContentFromClient(x, y);
            const int cmd = MediaTransportHitTest(cp);
            if (cmd != -1) {
                g_pressedMediaButton = cmd;
                SetCapture(hwnd);
                g_layoutDirty = true;
                return 0;
            }
            const float fraction = MediaScrubFractionFromContent(cp);
            if (fraction >= 0.0f) {
                g_scrubDragFraction.store(fraction, std::memory_order_relaxed);
                g_scrubbing = true;
                g_lastLiveSeekTime = 0.0;
                SetCapture(hwnd);
                g_layoutDirty = true;
                return 0;
            }
            break;
        }

        case WM_MOUSEMOVE: {
            TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hwnd, 0};
            TrackMouseEvent(&tme);

            if (g_dragging.load()) {
                POINT cur;
                GetCursorPos(&cur);
                RECT wr = {};
                GetWindowRect(hwnd, &wr);
                const int w = wr.right - wr.left;
                
                int nCX = s_dragWindow.x + (cur.x - s_dragCursor.x) + w / 2;
                int nCY = s_dragWindow.y + (cur.y - s_dragCursor.y);

                std::wstring fgExe = GetForegroundExeName();
                bool appSpecific = false;
                if (!fgExe.empty()) {
                    std::lock_guard lock(g_appPosMutex);
                    auto it = g_appPositions.find(fgExe);
                    if (it != g_appPositions.end()) {
                        it->second = {nCX, nCY};
                        appSpecific = true;
                    }
                }

                if (!appSpecific) {
                    g_customCenterX = nCX;
                    g_customTop = nCY;
                    g_customPosSet = true;
                }

                const Settings s = GetSettingsCopy();
                PositionOverlayWindow(hwnd, w, wr.bottom - wr.top, s);
                return 0;
            }

            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);

            if (g_scrubbing.load()) {
                const float fraction = MediaScrubFractionUnbounded(MediaContentFromClient(x, y));
                if (fraction < 0.0f) return 0;
                g_scrubDragFraction.store(fraction, std::memory_order_relaxed);
                g_layoutDirty = true;

                const double now = NowSeconds();
                if (now - g_lastLiveSeekTime.load(std::memory_order_relaxed) >= 0.15) {
                    g_lastLiveSeekTime.store(now, std::memory_order_relaxed);
                    int64_t endTicks = 0;
                    {
                        std::lock_guard lock(g_stateMutex);
                        endTicks = g_state.media.endTicks;
                    }
                    if (endTicks > 0) SeekMediaToTicks(static_cast<int64_t>(fraction * endTicks));
                }
                return 0;
            }

            const MediaContentPoint cp = MediaContentFromClient(x, y);
            const int hovered = MediaTransportHitTest(cp);
            if (g_hoveredMediaButton.exchange(hovered) != hovered) g_layoutDirty = true;
            const bool gear = MediaGearHitTest(cp);
            if (g_hoveredGear.exchange(gear) != gear) g_layoutDirty = true;
            return 0;
        }

        case WM_MOUSELEAVE:
            if (g_hoveredMediaButton.exchange(-1) != -1) g_layoutDirty = true;
            if (g_hoveredGear.exchange(false)) g_layoutDirty = true;
            return 0;

        case WM_CAPTURECHANGED:
            if (reinterpret_cast<HWND>(lParam) != hwnd) {
                if (g_dragging.exchange(false)) SaveCustomPositionFromWindow(hwnd);
                if (g_scrubbing.exchange(false)) g_layoutDirty = true;
                if (g_pressedMediaButton.exchange(-1) != -1) g_layoutDirty = true;
                g_hoveredMediaButton = -1;
            }
            return 0;

        case WM_LBUTTONUP: {
            if (g_dragging.load()) {
                g_dragging = false;
                ReleaseCapture();
                SaveCustomPositionFromWindow(hwnd);
                g_layoutDirty = true;
                return 0;
            }

            if (g_scrubbing.load()) {
                g_scrubbing = false;
                ReleaseCapture();
                const float fraction = Clamp(g_scrubDragFraction.load(std::memory_order_relaxed), 0.0f, 1.0f);
                int64_t endTicks = 0;
                {
                    std::lock_guard lock(g_stateMutex);
                    endTicks = g_state.media.endTicks;
                }
                if (endTicks > 0) {
                    const int64_t target = static_cast<int64_t>(fraction * endTicks);
                    SeekMediaToTicks(target);
                    std::lock_guard lock(g_stateMutex);
                    g_state.media.positionTicks = target;
                    g_state.media.lastUpdatedTicks = GetTickCount64();
                }
                g_layoutDirty = true;
                return 0;
            }

            if (g_pressedMediaButton.load() != -1) {
                g_pressedMediaButton = -1;
                ReleaseCapture();
                g_layoutDirty = true;
            }

            const int x = GET_X_LPARAM(lParam);
            const int y = GET_Y_LPARAM(lParam);
            const bool expandOnHover = GetSettingsCopy().expandOnHover;

            if (!expandOnHover && !g_clickExpanded.load()) {
                g_clickExpanded = true;
                g_layoutDirty = true;
                return 0;
            }

            const MediaContentPoint cp = MediaContentFromClient(x, y);

            if (MediaGearHitTest(cp)) {
                POINT pt = {x, y};
                ClientToScreen(hwnd, &pt);
                ShowSettingsMenu(hwnd, pt);
                return 0;
            }

            const int cmd = MediaTransportHitTest(cp);
            if (cmd != -1) {
                TriggerMediaTransportCommandWithConfirmation(cmd);
                return 0;
            }
            if (MediaScrubFractionFromContent(cp) >= 0.0f) return 0;

            OpenRelevantApp();
            return 0;
        }

        case WM_HOTKEY: {
            const int id = static_cast<int>(wParam);
            if (id == kHotkeyHideShow) {
                const bool nowHidden = !g_hidden.load();
                if (nowHidden) {
                    g_moveMode = false;
                    g_captureHotkey = false;
                }
                g_hidden = nowHidden;
                g_layoutDirty = true;
            } else if (id == kHotkeyNextTrack) {
                TriggerMediaTransportCommandWithConfirmation(2);
            } else if (id == kHotkeyPrevTrack) {
                TriggerMediaTransportCommandWithConfirmation(0);
            } else if (id == kHotkeyPlayPause) {
                SendMediaTransportCommand(1);
            }
            return 0;
        }

        case WM_APP_APPLY_HOTKEY:
            ApplyHideShowHotkey(hwnd);
            return 0;

        case WM_RBUTTONUP: {
            if (g_moveMode.load()) {
                g_moveMode = false;
                g_layoutDirty = true;
                return 0;
            }
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ClientToScreen(hwnd, &pt);
            ShowSettingsMenu(hwnd, pt);
            return 0;
        }
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI RenderThreadProc(void*) {
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kWindowClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                                kWindowClass, L"Dynamic Island Media Player", WS_POPUP, 0, 0, 520, 140,
                                nullptr, nullptr, wc.hInstance, nullptr);
    if (!hwnd) {
        if (SUCCEEDED(hrCo)) CoUninitialize();
        return 0;
    }
    g_hwnd.store(hwnd);
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    ApplyHideShowHotkey(hwnd);

    Renderer renderer;
    if (!renderer.Initialize(hwnd)) {
        DestroyWindow(hwnd);
        g_hwnd.store(nullptr);
        if (SUCCEEDED(hrCo)) CoUninitialize();
        return 0;
    }

    SpringValue widthSpring, heightSpring, nudgeSpring;
    widthSpring.Reset(0.0f);
    heightSpring.Reset(0.0f);
    nudgeSpring.Reset(0.0f);

    auto previousFrame = std::chrono::steady_clock::now();
    auto nextFrameTarget = previousFrame;
    bool parked = false;
    bool isBrowserFullscreen = false;
    bool isAnyFullscreen = false;
    double lastFullscreenCheck = -1.0;
    double lastTopmostCheck = 0.0;
    bool prevHover = false, prevPinned = false;
    uint64_t prevArtPtr = 0;
    std::wstring prevTitle;
    bool prevPlaying = false;
    bool prevCtrlState = false;
    std::wstring lastFgApp;
    int lastMinute = -1;
    
    int cachedRefreshRate = 60;
    double lastRefreshCheck = 0.0;

    while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        MSG message = {};
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == WM_APP_NEW_EVENT) {
                if (!g_disableAnimations.load()) {
                    nudgeSpring.value = -6.0f;
                    nudgeSpring.velocity = 0.0f;
                    nudgeSpring.target = 0.0f;
                }
                continue;
            }
            if (message.message == WM_APP_WAKE) continue;
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        const double now = NowSeconds();
        StopHotkeyCaptureIfDone();

        std::wstring currentFgApp = GetForegroundExeName();
        if (currentFgApp != lastFgApp) {
            lastFgApp = currentFgApp;
            g_layoutDirty = true;
        }

        SYSTEMTIME st;
        GetLocalTime(&st);
        if (st.wMinute != lastMinute) {
            lastMinute = st.wMinute;
            g_layoutDirty = true;
        }

        Settings cfg = GetSettingsCopy();
        
        bool hasAppPos = false;
        int appX = 0, appY = 0;
        if (!currentFgApp.empty()) {
            std::lock_guard lock(g_appPosMutex);
            auto it = g_appPositions.find(currentFgApp);
            if (it != g_appPositions.end()) {
                hasAppPos = true;
                appX = it->second.centerX;
                appY = it->second.top;
            }
        }

        if (Wh_GetIntSetting(L"Appearance.AutoDpiScale") != 0) {
            HMONITOR targetMon = nullptr;
            if (hasAppPos) {
                POINT pt = {appX, appY};
                targetMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
            } else if (g_customPosSet.load()) {
                POINT pt = {g_customCenterX.load(), g_customTop.load()};
                targetMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
            } else {
                targetMon = ResolveTargetMonitor(cfg.targetMonitor);
            }
            cfg.sizeScale = cfg.baseSizeScale * GetMonitorDpiScale(targetMon);
        } else {
            cfg.sizeScale = cfg.baseSizeScale;
        }

        if (g_customPosSet.load()) cfg.notchStyle = false;
        g_flushTop = cfg.notchStyle;

        SharedState snapshot;
        {
            std::lock_guard lock(g_stateMutex);
            snapshot = g_state;
        }

        if (now - lastFullscreenCheck > 0.5) {
            isAnyFullscreen = IsForegroundFullscreen(hwnd);
            isBrowserFullscreen = isAnyFullscreen && IsBrowserOrVideoFullscreen(hwnd);
            lastFullscreenCheck = now;
        }
        const bool moveMode = g_moveMode.load() || g_captureHotkey.load();
        const bool mediaPresent = snapshot.media.available;
        
        const bool fullscreenSuppressed = (isBrowserFullscreen && cfg.hideInFullscreen) ||
                                           (isAnyFullscreen && !g_overGames.load());
        
        bool idleExpired = false;
        if (cfg.idleTimeoutSec > 0 && mediaPresent && !snapshot.media.playing) {
            if (snapshot.media.pausedAt > 0.0 && (now - snapshot.media.pausedAt >= static_cast<double>(cfg.idleTimeoutSec))) {
                idleExpired = true;
            }
        }

        const bool wantVisible = g_hidden.load() ? false : (moveMode ? true : (mediaPresent && !fullscreenSuppressed && !idleExpired));
        const bool pinned = moveMode;

        if (wantVisible && isAnyFullscreen && g_overGames.load() && !isBrowserFullscreen && g_alwaysOnTop.load() &&
            now - lastTopmostCheck > 1.0) {
            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            lastTopmostCheck = now;
        }

        if (wantVisible && parked) {
            parked = false;
            ShowWindow(hwnd, SW_SHOWNOACTIVATE);
            g_layoutDirty = true;
            previousFrame = std::chrono::steady_clock::now();
            nextFrameTarget = previousFrame;
        }

        RECT windowRect = {};
        GetWindowRect(hwnd, &windowRect);
        POINT cursor = {};
        GetCursorPos(&cursor);
        bool hover = false;
        if (!parked && widthSpring.value > 1.0f && heightSpring.value > 1.0f) {
            const float topPad = cfg.notchStyle ? 0.0f : kRenderPadY;
            RECT pill = {
                windowRect.left + static_cast<int>(std::round(kRenderPadX)),
                windowRect.top + static_cast<int>(std::round(topPad + nudgeSpring.value)),
                windowRect.left + static_cast<int>(std::round(kRenderPadX + widthSpring.value)),
                windowRect.top + static_cast<int>(std::round(topPad + nudgeSpring.value + heightSpring.value))};
            hover = PtInRect(&pill, cursor) != FALSE;
        }

        const bool ctrlHeld = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        if (ctrlHeld != prevCtrlState) {
            prevCtrlState = ctrlHeld;
            g_layoutDirty = true;
        }

        const bool clickThrough = g_clickThrough.load() && !ctrlHeld && !moveMode;
        if (clickThrough) hover = false;
        SetClickThrough(hwnd, clickThrough);

        const bool isInteracting = (g_clickThrough.load() && ctrlHeld && hover) ||
                                   g_dragging.load() || g_scrubbing.load() ||
                                   (g_pressedMediaButton.load() != -1);

        bool needsRender = false;
        
        const bool hoverExpanded = cfg.expandOnHover ? hover : (hover && g_clickExpanded.load());
        const bool recentTrackChange = g_autoExpand.load() && mediaPresent && snapshot.media.playing &&
                                       !snapshot.media.title.empty() &&
                                       (now - snapshot.media.titleChangedAt < 5.0);
        const bool expanded = hoverExpanded || pinned || recentTrackChange || g_dragging.load() || g_lockExpanded.load();

        if (wantVisible && mediaPresent && !parked) {
            if ((snapshot.media.playing && expanded) ||
                g_scrubbing.load() ||
                g_pressedMediaButton.load() != -1 ||
                g_dragging.load() ||
                (g_colorRainbowEnabled.load() && snapshot.media.playing) ||
                renderer.IsAnimationActive(widthSpring.value / cfg.sizeScale)) {
                needsRender = true;
            }
        }

        if (!hover && g_clickExpanded.load()) {
            g_clickExpanded = false;
            needsRender = true;
        }
        if (!hover) {
            if (g_hoveredMediaButton.load() != -1) { g_hoveredMediaButton = -1; needsRender = true; }
            if (g_hoveredGear.load()) { g_hoveredGear = false; needsRender = true; }
        }

        float targetWidth = 0.0f, targetHeight = 0.0f;
        if (wantVisible) {
            targetWidth = (expanded ? MediaLayout::kExpandedWidth : MediaLayout::kCollapsedWidth) * cfg.sizeScale;
            targetHeight = (expanded ? MediaLayout::kExpandedHeight : MediaLayout::kCollapsedHeight) * cfg.sizeScale;
        }
        widthSpring.target = targetWidth;
        heightSpring.target = targetHeight;

        const auto currentFrame = std::chrono::steady_clock::now();
        float dt = Clamp(std::chrono::duration<float>(currentFrame - previousFrame).count(), 0.001f, 0.050f);
        previousFrame = currentFrame;

        float stiffMult = 1.0f, dampMult = 1.0f;
        switch (cfg.animationStyle) {
            case AnimationStyle::Smooth: dampMult = 1.35f; break;
            case AnimationStyle::Bouncy: stiffMult = 1.1f; dampMult = 0.70f; break;
            case AnimationStyle::Snappy: stiffMult = 1.5f; dampMult = 1.25f; break;
            default: break;
        }
        auto springParams = [&](const SpringValue& s, float& stiff, float& damp) {
            stiff = 280.0f * stiffMult;
            damp = 24.0f * dampMult;
            if (s.target > s.value) { stiff = 380.0f * stiffMult; damp = 26.0f * dampMult; }
            else if (s.target < s.value) { stiff = 200.0f * stiffMult; damp = 28.0f * dampMult; }
        };
        float ws, wd, hs, hd;
        springParams(widthSpring, ws, wd);
        springParams(heightSpring, hs, hd);
        const float speed = cfg.animationSpeed;

        if (g_disableAnimations.load()) {
            widthSpring.Reset(widthSpring.target);
            heightSpring.Reset(heightSpring.target);
            nudgeSpring.Reset(nudgeSpring.target);
        } else {
            widthSpring.Step(dt * speed, ws, wd);
            heightSpring.Step(dt * speed, hs, hd);
            if (widthSpring.value < 0.0f) { widthSpring.value = 0.0f; widthSpring.velocity = 0.0f; }
            if (heightSpring.value < 0.0f) { heightSpring.value = 0.0f; heightSpring.velocity = 0.0f; }
            nudgeSpring.Step(dt * speed, 280.0f * stiffMult, 24.0f * dampMult);
        }

        const bool springsAnimating = !g_disableAnimations.load() && (
            std::fabs(widthSpring.value - widthSpring.target) > 0.5f ||
            std::fabs(heightSpring.value - heightSpring.target) > 0.5f ||
            std::fabs(widthSpring.velocity) > 0.5f || std::fabs(heightSpring.velocity) > 0.5f ||
            std::fabs(nudgeSpring.value) > 0.5f || std::fabs(nudgeSpring.velocity) > 0.5f);
        if (springsAnimating) needsRender = true;

        if (!wantVisible && !springsAnimating && widthSpring.value < 0.5f && heightSpring.value < 0.5f) {
            if (!parked) {
                ShowWindow(hwnd, SW_HIDE);
                parked = true;
            }
            MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, (mediaPresent && !g_hidden.load()) ? 1000 : INFINITE, QS_ALLINPUT);
            nextFrameTarget = std::chrono::steady_clock::now();
            continue;
        }

        if (g_layoutDirty.load()) needsRender = true;
        if (hover != prevHover || pinned != prevPinned) {
            needsRender = true;
            prevHover = hover;
            prevPinned = pinned;
        }

        const uint64_t artPtr = snapshot.media.art ? snapshot.media.art->generation : 0;
        if (artPtr != prevArtPtr || snapshot.media.title != prevTitle || snapshot.media.playing != prevPlaying) {
            needsRender = true;
            prevArtPtr = artPtr;
            prevTitle = snapshot.media.title;
            prevPlaying = snapshot.media.playing;
        }

        if (needsRender && !parked) {
            renderer.Render(snapshot, cfg, widthSpring.value, heightSpring.value, nudgeSpring.value, hover,
                            pinned, now, isInteracting);
        }

        if (!needsRender) {
            DWORD sleepTime = 250; 
            if (wantVisible && !parked) {
                SYSTEMTIME stTime;
                GetLocalTime(&stTime);
                int msToNextMinute = (60 - stTime.wSecond) * 1000 - stTime.wMilliseconds + 50;
                sleepTime = static_cast<DWORD>(std::min<int>(sleepTime, std::max<int>(10, msToNextMinute)));
            }
            if (g_captureHotkey.load() || g_moveMode.load()) sleepTime = 100;
            
            MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE, sleepTime, QS_ALLINPUT);
            nextFrameTarget = std::chrono::steady_clock::now();
        } else {
            int targetFps = cfg.targetFps;
            if (targetFps <= 0) {
                if (now - lastRefreshCheck > 5.0) {
                    cachedRefreshRate = GetMonitorRefreshRate(hwnd);
                    lastRefreshCheck = now;
                }
                targetFps = cachedRefreshRate;
            }
            targetFps = ClampInt(targetFps, 30, 1000);
            double targetFrameMs = 1000.0 / targetFps;
            
            nextFrameTarget += std::chrono::duration_cast<std::chrono::steady_clock::duration>(
                std::chrono::duration<double, std::milli>(targetFrameMs));
            
            while (true) {
                const auto nowTime = std::chrono::steady_clock::now();
                if (nowTime >= nextFrameTarget) break;
                if (WaitForSingleObject(g_stopEvent, 0) != WAIT_TIMEOUT) break;
                const double remainingMs = std::chrono::duration<double, std::milli>(nextFrameTarget - nowTime).count();
                if (remainingMs > 1.0) {
                    Sleep(1);
                } else {
                    Sleep(0);
                }
            }
        }
    }

    if (g_captureHook) {
        UnhookWindowsHookEx(g_captureHook);
        g_captureHook = nullptr;
        g_captureHotkey = false;
        g_captureHotkeyTarget = 0;
    }
    for (int i = 1; i <= 4; ++i) {
        if (g_hotkeyRegistered[i]) {
            UnregisterHotKey(hwnd, i);
            g_hotkeyRegistered[i] = false;
        }
    }
    renderer.Shutdown();
    DestroyWindow(hwnd);
    g_hwnd.store(nullptr);
    UnregisterClassW(kWindowClass, wc.hInstance);
    if (SUCCEEDED(hrCo)) CoUninitialize();
    return 0;
}

DWORD WINAPI RenderThreadProc(void*);
DWORD WINAPI MediaThreadProc(void*);
bool StartThreads() {
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) return false;

    g_renderThread = CreateThread(nullptr, 0, RenderThreadProc, nullptr, 0, nullptr);
    if (!g_renderThread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return false;
    }
    
    g_mediaThread = CreateThread(nullptr, 0, MediaThreadProc, nullptr, 0, nullptr);
    if (!g_mediaThread) {
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_renderThread, INFINITE);
        CloseHandle(g_renderThread);
        g_renderThread = nullptr;
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return false;
    }
    
    return true;
}

void StopThreads() {
    if (g_stopEvent) SetEvent(g_stopEvent);

    HANDLE handles[] = {g_renderThread, g_mediaThread};
    for (HANDLE h : handles) {
        if (h) {
            WaitForSingleObject(h, 2000); 
            CloseHandle(h);
        }
    }
    g_renderThread = g_mediaThread = nullptr;

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

}

BOOL WhTool_ModInit() {
    LoadSettings();
    if (!StartThreads()) {
        StopThreads();
        return FALSE;
    }
    g_layoutDirty = true;
    Wh_Log(L"Dynamic Island Media Player initialized.");
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

void WhTool_ModUninit() {
    StopThreads();
    Wh_Log(L"Dynamic Island Media Player unloaded.");
}

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
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
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
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

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

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
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

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
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
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

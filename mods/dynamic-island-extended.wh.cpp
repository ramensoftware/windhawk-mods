// ==WindhawkMod==
// @id              dynamic-island-extended
// @name            Dynamic Island for Windows (Extended)
// @description     A living, breathing pill overlay inspired by iPhone's Dynamic Island with a RAM cleaner, a standalone Game Overlay options panel, and a stable panel order that never loses your current view. Reacts to media, downloads, clipboard, battery, and more.
// @version         1.0.0
// @author          jenci03
// @github          https://github.com/jenci03
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lshcore -ld2d1 -ldwrite -ldwmapi -lgdi32 -luser32 -lshell32 -lruntimeobject -lwindowscodecs -lavrt -lsetupapi -lpsapi -lpdh -ladvapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Dynamic Island for Windows (Extended)

A fluid, living overlay inspired by Apple's Dynamic Island, bringing a beautiful, highly-responsive UI to your Windows desktop. Built natively with hardware-accelerated Direct2D rendering for a buttery-smooth 60 FPS experience.

![Dynamic Island Preview](https://raw.githubusercontent.com/devcode90/Dynamic-Island-for-Windows/main/previews/Full-preview.png)

---

## Modules & Dashboards

The Dynamic Island intelligently expands to display context-aware dashboards. You can easily navigate between different views using your mouse scroll wheel (or left/right-click the pill).

| Module | Description | Preview |
| :--- | :--- | :--- |
| **Media Player** | Shows live album art, track details, audio waveforms, and full playback controls. | ![Media](https://raw.githubusercontent.com/devcode90/Dynamic-Island-for-Windows/main/previews/media.png) |
| **Game Overlay** | Real-time FPS, GPU temperature, CPU, GPU, and RAM utilization overlays tailored for gamers. | ![Gamebar](https://raw.githubusercontent.com/devcode90/Dynamic-Island-for-Windows/main/previews/gamebar.png) |
| **Game Overlay Options** | A dedicated panel with toggles for every overlay metric (FPS counter, GPU temp, CPU, RAM, GPU, disk load), so you can turn chips back on after disabling them. | |
| **RAM Cleaner** | A one-click memory cleaner that trims working sets and frees physical RAM for you. | |
| **Calendar** | A sleek, perfectly aligned monthly calendar that highlights the current date. | ![Calendar](https://raw.githubusercontent.com/devcode90/Dynamic-Island-for-Windows/main/previews/calender.png) |
| **Idle View** | A minimal dashboard with your battery status, digital clock, and sleek pagination dots. | ![Idle](https://raw.githubusercontent.com/devcode90/Dynamic-Island-for-Windows/main/previews/idle.png) |
| **Camera Privacy** | Shows a green dot when an app is actively using your webcam. | ![Camera](https://raw.githubusercontent.com/devcode90/Dynamic-Island-for-Windows/main/previews/camera-detected.png) |
| **Mic Privacy** | Shows an orange dot when an app is actively using your microphone. | ![Mic](https://raw.githubusercontent.com/devcode90/Dynamic-Island-for-Windows/main/previews/mic-detected.png) |

---

## Core Features

- **Stable Panel Order:** Every panel keeps its own fixed slot, so starting or stopping audio playback never shifts the other panels and never hides the tab you were on.
- **Standalone Game Overlay Options:** Game overlay chips are controlled from their own panel (the "Game Overlay" tab), letting you re-enable any disabled metric at any time.
- **RAM Cleaner:** Free memory with a single click from its dedicated dashboard.
- **Hardware Privacy Indicators:** A pulsing orange dot appears when your microphone is active, and a green dot when your camera is in use. Rate-limited polling ensures absolutely no CPU drain.
- **High-Res Clipboard & Notifications:** Instantly see what you copied or your latest Windows notifications, featuring crisp, high-fidelity 64px app icons extracted directly from system executables.
- **Dynamic Fluid Animations:** Fully smooth resizing and splitting when multiple events happen at once (e.g., media playing while you copy text or receive a notification).
- **Customizable Aesthetics:** Switch between sleek OLED Black, Dark Gray, Midnight Blue, and Deep Purple themes from the right-click menu, or use the settings to dial in your exact hex colors.

---

## Usage & Settings

- **Hover & Scroll:** Hover over the island to seamlessly expand it. Use your mouse scroll wheel to swipe between the Game, Game Overlay Options, Media, RAM Cleaner, and Calendar tabs.
- **Right-Click Menu:** Right-click the island to access Theme presets, Transparency settings, and to pin the island open.
- **Windhawk Settings:** Visit the Mod Settings tab to change the island's Position, Size Scale, Animation Speed, and toggle specific modules. You can also perfectly align the island using the new `Offset X` and `Offset Y` settings, and even select exactly which monitor the island should appear on (including a brand new "Follow Mouse" mode!).
- **Notifications:** To use the notification module, you need to add `explorer.exe` to the process inclusion list in the Advanced tab of the mod settings and restart the mod.

---

## Feedback & Credits

### Feedback / Support / Bug Reports
- Please use [Windhawk Mods Issues](https://github.com/ramensoftware/windhawk-mods/issues) to report bugs, request features, or share feedback.  
- Clear descriptions, screenshots, or steps to reproduce help improve fixes and updates.  
- Suggestions for UI/UX or new integrations are always welcome.

### Credits
- **[ciizerr @GitHub](https://github.com/ciizerr)**: Improved the UI by refining layout alignment, fixing dashboard scaling, and enhancing calendar module integration.

This is a community variant of the original [Dynamic Island for Windows](https://windhawk.net/mods/dynamic-island-for-windows) mod.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Appearance:
  - Position: top-center
    $name: Position
    $description: Where the island should appear on your screen.
    $options:
      - top-center: Top Center
      - top-left: Top Left
      - top-right: Top Right
      - bottom-center: Bottom Center
  - TargetMonitor: primary
    $name: Target Monitor
    $description: Select the screen to display the island. If a display isn't found, it safely falls back to the Primary Monitor.(Extra Displays are given even if they don't exist because windhawk settings are static)
    $options:
      - 'primary': Primary Monitor
      - '1': Display 1
      - '2': Display 2
      - '3': Display 3
      - '4': Display 4
      - '5': Display 5
      - 'follow': Follow Mouse (Active Monitor)
  - OffsetX: 0
    $name: Offset X
    $description: Adjust the horizontal position (in pixels). Positive values move it right, negative values move it left.
  - OffsetY: 0
    $name: Offset Y
    $description: Adjust the vertical position (in pixels). Positive values move it down, negative values move it up.
  - SizeScale: '1.0'
    $name: Size scale
    $description: Makes the entire island and its contents larger or smaller.
    $options:
      - '0.8': 0.8x
      - '1.0': 1.0x
      - '1.2': 1.2x
      - '1.5': 1.5x
      - '1.8': 1.8x
      - '2.0': 2.0x
      - '2.5': 2.5x
  - AutoHideIdleSeconds: '0'
    $name: Auto-hide idle island
    $description: Hide the idle pill after this many seconds of inactivity. 0 to disable.
    $options:
      - '-1': Hide instantly
      - '0': Never hide (default)
      - '5': Hide after 5 seconds
      - '10': Hide after 10 seconds
      - '30': Hide after 30 seconds
      - '60': Hide after 60 seconds
  - UnhideOnHover: true
    $name: Unhide on hover
    $description: Allow the hidden island to reappear when you hover your mouse over it.
  - AutoDpiScale: true
    $name: Auto DPI scaling
    $description: Automatically scales the island to match your monitor's DPI. Recommended for 4K screens.
  - W11Style: false
    $name: Native Windows 11 style
    $description: Changes the shape from an Apple pill to a Windows 11 rounded box.
  - AlwaysOnTop: true
    $name: Always on top
    $description: Keeps the island above all other windows. Turn this off if it blocks other apps.
  - ExpandOnHover: true
    $name: Expand on hover
    $description: Expand the island automatically when hovered. If disabled, click to expand.
  - AnimationSpeed: normal
    $name: Animation speed
    $description: How fast the island expands and collapses.
    $options:
      - slow: Slow
      - normal: Normal
      - fast: Fast
  $name: Appearance & Behavior
- Themes:
  - AccentColorMode: auto
    $name: Accent color mode
    $description: How the glowing accent color is chosen. Auto extracts it from album art.
    $options:
      - auto: Auto, from album art
      - system: System
      - custom: Custom hex
  - CustomAccentHex: "#4cc9f0"
    $name: Custom accent hex
    $description: The hex color to use when the accent mode is set to Custom.
  - PillBgColor: "#0D0D0F"
    $name: Pill background color
    $description: Hex color for the background. Presets are available in the right-click menu.
  - TextPrimaryColor: "#F7F7F7"
    $name: Primary text color
    $description: Hex color for titles and main text.
  - TextSecondaryColor: "#888888"
    $name: Secondary text color
    $description: Hex color for artist names and muted labels.
  - TintIntensity: 72
    $name: Background tint intensity
    $description: 0 to 100. Controls how dark the background tint behind the island is.
  - PillOpacity: 96
    $name: Pill transparency
    $description: 35 to 100. Lower values make the island more see-through.
  $name: Colors & Theming
- Modules:
  - Media: true
    $name: Media module
    $description: Shows album art, song info, and playback controls when music is playing.
  - Clipboard: true
    $name: Clipboard module
    $description: Shows a quick preview of the text or images you just copied.
  - Battery: true
    $name: Battery module
    $description: Shows an alert when your laptop battery is running low.
  - Progress: true
    $name: Progress module
    $description: Shows a progress ring around the island for downloads or file copies.
  - GameOverlay: false
    $name: Enable game overlay mode
    $description: Replaces the clock with live stats like FPS, GPU temperature, CPU, and RAM usage.
  - ShowMetricText: false
    $name: Show labels in metric chips
    $description: Adds text labels (like "CPU") inside the game overlay bars.
  - GameTempUnit: celsius
    $name: GPU temperature unit
    $description: Unit used for the GPU temperature chip in Game Overlay mode.
    $options:
      - celsius: Celsius (ÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â°C)
      - fahrenheit: Fahrenheit (ÃƒÆ’Ã¢â‚¬Å¡Ãƒâ€šÃ‚Â°F)
  - GameTempUpdateMs: 1000
    $name: GPU temp update interval (ms)
    $description: How often the GPU temperature is refreshed while the game overlay is visible. Clamped between 200 and 10000 ms.
  - GameFpsViewer: true
    $name: Show FPS counter
    $description: Shows a live FPS counter next to the GPU temperature chip in Game Overlay mode. Disable it to hide the FPS counter.
  $name: Modules & Features
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

#ifndef WH_MOD_ID
#define WH_MOD_ID L"dynamic-island-for-windows"
#endif
#ifndef WH_MOD_VERSION
#define WH_MOD_VERSION L"1.1.1"
#endif

#include <windows.h>
#include <unknwn.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <setupapi.h>
#include <dbt.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <shcore.h>
#include <windowsx.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <mmreg.h>
#include <objbase.h>
#include <wrl/client.h>
#include <uiautomation.h>
#include <psapi.h>
#include <tlhelp32.h>
#pragma comment(lib, "psapi.lib")
#include <pdh.h>
#include <pdhmsg.h>
#pragma comment(lib, "pdh.lib")
#include <evntrace.h>
#include <evntcons.h>
#pragma comment(lib, "advapi32.lib")

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cwchar>
#include <cstring>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#if __has_include(<winrt/Windows.UI.Notifications.Management.h>) && \
    __has_include(<winrt/Windows.UI.Notifications.h>)
#define DYNAMIC_ISLAND_HAS_USER_NOTIFICATION_LISTENER 1
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.UI.Notifications.h>
#include <winrt/Windows.UI.Notifications.Management.h>
#else
#define DYNAMIC_ISLAND_HAS_USER_NOTIFICATION_LISTENER 0
#endif

using Microsoft::WRL::ComPtr;
using namespace std::chrono_literals;

void DiLog(const wchar_t* fmt, ...) {
    (void)fmt;
}

namespace {

constexpr wchar_t kWindowClass[] = L"Windhawk.DynamicIslandForWindows";
constexpr UINT WM_APP_LAYOUT_CHANGED = WM_APP + 0x442;
constexpr UINT WM_APP_NEW_EVENT = WM_APP + 0x443;
constexpr float kRenderPadX = 28.0f;
constexpr float kRenderPadY = 22.0f;

enum class IslandKind {
    Idle,
    Media,
    Progress,
    Clipboard,
    Notification,
    Volume,
    BatteryLow,
    CapsLock,
    Device,
    Split,
};

enum class Position {
    TopCenter,
    TopLeft,
    TopRight,
    BottomCenter,
};

enum class AccentMode {
    Auto,
    System,
    Custom,
};

struct Settings {
    Position position = Position::TopCenter;
    int targetMonitor = 0;
    int offsetX = 0;
    int offsetY = 0;
    float sizeScale = 1.0f;
    AccentMode accentMode = AccentMode::Auto;
    D2D1_COLOR_F customAccent = D2D1::ColorF(0x4cc9f0);
    float animationSpeed = 1.0f;
    bool media = true;
    bool clipboard = true;
    bool battery = true;
    bool progress = true;
    float tintOpacity = 0.72f;
    float pillOpacity = 0.96f;
    bool gameOverlay = false;
    bool showMetricText = true;
    bool gpuTempFahrenheit = false;
    int gpuTempUpdateMs = 1000;
    bool gameFpsViewer = true;
    bool gameOverlayTemp = true;
    bool gameOverlayCpu = true;
    bool gameOverlayRam = true;
    bool gameOverlayGpu = true;
    bool gameOverlayDisk = true;
    int autoHideIdleSeconds = 0;
    bool unhideOnHover = true;
    bool alwaysOnTop = true;
    bool expandOnHover = true;
    bool autoDpiScale = true;
    bool w11Style = false;
    bool hideIsland = false;
    // Color customization
    D2D1_COLOR_F pillBgColor = D2D1::ColorF(0.051f, 0.051f, 0.059f, 1.0f); // #0D0D0F
    D2D1_COLOR_F textPrimaryColor = D2D1::ColorF(0.969f, 0.969f, 0.969f, 1.0f); // #F7F7F7
    D2D1_COLOR_F textSecondaryColor = D2D1::ColorF(0.533f, 0.533f, 0.533f, 1.0f); // #888888
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
    std::wstring sourceAppUserModelId;
    std::wstring sourceName;
    std::wstring sourceBadge;
    BitmapPixels art;
    BitmapPixels sourceIcon;
    uint64_t artGeneration = 0;
    uint64_t sourceIconGeneration = 0;
    double artChangedAt = 0.0;
    int64_t positionTicks = 0;
    int64_t endTicks = 0;
    int64_t lastUpdatedTicks = 0;
};

struct ClipboardSnapshot {
    bool active = false;
    bool image = false;
    std::wstring text;
    std::wstring appName;
    BitmapPixels appIcon;
    double expiresAt = 0.0;
};

struct BatterySnapshot {
    bool active = false;
    bool low = false;
    bool charging = false;
    int percent = 100;
    DWORD secondsRemaining = BATTERY_LIFE_UNKNOWN;
    double expiresAt = 0.0;
};

struct ProgressSnapshot {
    bool active = false;
    int percent = 0;
};

struct NotificationSnapshot {
    bool active = false;
    std::wstring app;
    std::wstring title;
    std::wstring body;
    BitmapPixels icon;
    double expiresAt = 0.0;
};

struct VolumeSnapshot {
    bool active = false;
    int percent = 0;
    bool muted = false;
    std::wstring deviceName;
    double expiresAt = 0.0;
};

struct CapsLockSnapshot {
    bool active = false;
    bool capsOn = false;
    bool numOn = false;
    bool isNumEvent = false;
    double expiresAt = 0.0;
};

enum class DeviceEventType {
    Connected,
    Disconnected,
};

struct DeviceSnapshot {
    bool active = false;
    DeviceEventType eventType = DeviceEventType::Connected;
    std::wstring deviceName;  // e.g. "USB Drive" or "Bluetooth Device"
    bool isBluetoothLike = false;
    double expiresAt = 0.0;
};

struct SystemSnapshot {
    int volumePercent = 0;
    bool volumeMuted = false;
    int cpuPercent = 0;
    int memoryPercent = 0;
    int diskFreePercent = 0;
    int gameFps = 0;
    bool gameFpsValid = false;
    int gpuPercent = -1;
    bool charging = false;
    bool micActive = false;      // orange dot: microphone in use
    bool cameraActive = false;   // green dot: camera in use
    std::wstring foregroundTitle;
};

struct Activity {
    IslandKind kind = IslandKind::Idle;
    float width = 120.0f;
    float height = 36.0f;
};

struct SharedState {
    MediaSnapshot media;
    ClipboardSnapshot clipboard;
    NotificationSnapshot notification;
    VolumeSnapshot volume;
    CapsLockSnapshot capsLock;
    DeviceSnapshot device;
    BatterySnapshot battery;
    ProgressSnapshot progress;
    SystemSnapshot system;
    std::array<float, 48> waveform{};
    size_t waveformWrite = 0;
    bool muted = false;
};

struct SpringValue {
    float value = 0.0f;
    float velocity = 0.0f;
    float target = 0.0f;

    void Reset(float v) {
        value = target = v;
        velocity = 0.0f;
    }

    void Step(float dt, float stiffness, float damping) {
        const float displacement = value - target;
        const float acceleration = -stiffness * displacement - damping * velocity;
        velocity += acceleration * dt;
        value += velocity * dt;

        if (std::fabs(value - target) < 0.01f && std::fabs(velocity) < 0.01f) {
            value = target;
            velocity = 0.0f;
        }
    }
};

Settings g_settings;
std::mutex g_stateMutex;
SharedState g_state;
std::atomic<uint64_t> g_artGenerationCounter = 0;

HWND g_hwnd = nullptr;
HANDLE g_stopEvent = nullptr;
HANDLE g_settingsChangedEvent = nullptr;
HANDLE g_renderThread = nullptr;
HANDLE g_mediaThread = nullptr;
HANDLE g_audioThread = nullptr;
HANDLE g_notificationThread = nullptr;
std::atomic<bool> g_running = false;
std::atomic<int> g_idleTab = 0;
std::atomic<bool> g_layoutDirty = true;
std::atomic<bool> g_clickExpanded = false;
std::atomic<int> g_pressedMediaButton = -1;

// RAM cleaner state
std::atomic<bool> g_ramCleaning = false;
std::atomic<DWORDLONG> g_ramFreedBytes = 0;
std::atomic<double> g_ramCleanedAt = 0.0;

// Unified panel list: [Game?][Game overlay options?][RAM cleaner][Calendar][Config][Media?].
// Game and its options appear only if the game overlay is on. Media is always appended
// LAST so that starting/stopping audio playback never shifts the other panels (their
// indices are stable regardless of mediaOn), and the game overlay is never lost.
static inline int PanelCount(bool gameOn, bool mediaOn) {
    return (gameOn ? 2 : 0) + 3 + (mediaOn ? 1 : 0);
}

static inline int GamePanelIndex() { return 0; }
static inline int GameOptionsPanelIndex() { return 1; }
static inline int MediaPanelIndex(bool gameOn, bool mediaOn) {
    return PanelCount(gameOn, mediaOn) - 1;
}
static inline int RamPanelIndex(bool gameOn, bool mediaOn) {
    return (gameOn ? 2 : 0);
}
static inline int CalendarPanelIndex(bool gameOn, bool mediaOn) {
    return RamPanelIndex(gameOn, mediaOn) + 1;
}
static inline int ConfigPanelIndex(bool gameOn, bool mediaOn) {
    return RamPanelIndex(gameOn, mediaOn) + 2;
}

static inline int CurrentPanelIndex(bool gameOn, bool mediaOn) {
    const int count = PanelCount(gameOn, mediaOn);
    int tab = g_idleTab.load() % count;
    if (tab < 0) tab += count;
    return tab;
}

void LoadSettings();
void ToggleGameOverlayChip(int id);

enum ConfigAction : int {
    ConfigActionNone = 0,
    ConfigActionLeft,
    ConfigActionCenter,
    ConfigActionRight,
    ConfigActionToggleFps,
    ConfigActionToggleIsland,
};

int ConfigActionAtPoint(float x, float y, float w, float h) {
    const float scale = 1.0f;
    const float inset = 30.0f * scale, gap = 10.0f * scale;
    const float y0 = 54.0f * scale, y1 = 112.0f * scale, bh = 42.0f * scale;
    const float bw = (w - 2.0f * inset - 2.0f * gap) / 3.0f;
    if (y >= y0 && y <= y0 + bh) {
        if (x >= inset && x <= inset + bw) return ConfigActionLeft;
        if (x >= inset + bw + gap && x <= inset + bw + gap + bw) return ConfigActionCenter;
        if (x >= inset + 2.0f * (bw + gap) && x <= inset + 2.0f * (bw + gap) + bw) return ConfigActionRight;
        return ConfigActionNone;
    }
    const float bw2 = (w - 2.0f * inset - gap) / 2.0f;
    if (y >= y1 && y <= y1 + bh) {
        if (x >= inset && x <= inset + bw2) return ConfigActionToggleFps;
        if (x >= inset + bw2 + gap && x <= inset + bw2 + gap + bw2) return ConfigActionToggleIsland;
    }
    return ConfigActionNone;
}

void ApplyConfigAction(int action) {
    switch (action) {
        case ConfigActionLeft:
            Wh_SetIntValue(L"PositionOverride", 0);
            LoadSettings();
            break;
        case ConfigActionCenter:
            Wh_SetIntValue(L"PositionOverride", 1);
            LoadSettings();
            break;
        case ConfigActionRight:
            Wh_SetIntValue(L"PositionOverride", 2);
            LoadSettings();
            break;
        case ConfigActionToggleFps:
            Wh_SetIntValue(L"FpsVisible",
                           (Wh_GetIntValue(L"FpsVisible", -1) >= 0
                                ? (Wh_GetIntValue(L"FpsVisible", 0) != 0)
                                : g_settings.gameFpsViewer)
                               ? 0
                               : 1);
            LoadSettings();
            break;
        case ConfigActionToggleIsland:
            Wh_SetIntValue(L"IslandHidden", Wh_GetIntValue(L"IslandHidden", 0) ? 0 : 1);
            LoadSettings();
            break;
    }
}

enum GameOptionsAction : int {
    GameOptionNone = 0,
    GameOptionToggleFps,
    GameOptionToggleTemp,
    GameOptionToggleCpu,
    GameOptionToggleRam,
    GameOptionToggleGpu,
    GameOptionToggleDisk,
};

int GameOptionsActionAtPoint(float x, float y, float w, float h) {
    const float scale = 1.0f;
    const float inset = 30.0f * scale, gap = 10.0f * scale;
    const float bh = 42.0f * scale;
    const float bw3 = (w - 2.0f * inset - 2.0f * gap) / 3.0f;
    if (y < 54.0f * scale || y > 54.0f * scale + 2.0f * (bh + gap) + bh) {
        return GameOptionNone;
    }
    const float col0 = inset;
    const float col1 = inset + bw3 + gap;
    const float col2 = inset + 2.0f * (bw3 + gap);
    int row = static_cast<int>((y - 54.0f * scale) / (bh + gap));
    if (row < 0 || row > 1) return GameOptionNone;
    if (row == 1 && y > 54.0f * scale + (bh + gap) + bh) return GameOptionNone;
    if (x >= col0 && x <= col0 + bw3) {
        return static_cast<int>(GameOptionToggleFps) + row * 3;
    }
    if (x >= col1 && x <= col1 + bw3) {
        return static_cast<int>(GameOptionToggleFps) + row * 3 + 1;
    }
    if (x >= col2 && x <= col2 + bw3) {
        return static_cast<int>(GameOptionToggleFps) + row * 3 + 2;
    }
    return GameOptionNone;
}

void ApplyGameOptionsAction(int action) {
    switch (action) {
        case GameOptionToggleFps: ToggleGameOverlayChip(0); break;
        case GameOptionToggleTemp: ToggleGameOverlayChip(1); break;
        case GameOptionToggleCpu: ToggleGameOverlayChip(2); break;
        case GameOptionToggleRam: ToggleGameOverlayChip(3); break;
        case GameOptionToggleGpu: ToggleGameOverlayChip(4); break;
        case GameOptionToggleDisk: ToggleGameOverlayChip(5); break;
        default: break;
    }
}

FILETIME g_prevIdleTime = {};
FILETIME g_prevKernelTime = {};
FILETIME g_prevUserTime = {};
UINT g_shellHookMessage = 0;
bool g_volumeInitialized = false;
std::atomic<double> g_lastNudgeTime = 0.0;

constexpr GUID kSubTypeIeeeFloat = {
    0x00000003,
    0x0000,
    0x0010,
    {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71},
};



double NowSeconds() {
    using clock = std::chrono::steady_clock;
    static const auto start = clock::now();
    return std::chrono::duration<double>(clock::now() - start).count();
}

float Clamp(float v, float lo, float hi) {
    return std::max(lo, std::min(hi, v));
}

int ClampInt(int v, int lo, int hi) {
    return std::max(lo, std::min(hi, v));
}

bool EqualsNoCase(std::wstring_view a, std::wstring_view b) {
    if (a.size() != b.size()) {
        return false;
    }

    for (size_t i = 0; i < a.size(); ++i) {
        if (towlower(a[i]) != towlower(b[i])) {
            return false;
        }
    }

    return true;
}

std::wstring GetStringSettingCopy(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

D2D1_COLOR_F ColorFromHex(std::wstring text, D2D1_COLOR_F fallback) {
    if (!text.empty() && text[0] == L'#') {
        text.erase(text.begin());
    }

    if (text.size() != 6) {
        return fallback;
    }

    wchar_t* end = nullptr;
    const unsigned long rgb = wcstoul(text.c_str(), &end, 16);
    if (!end || *end != L'\0') {
        return fallback;
    }

    return D2D1::ColorF(
        ((rgb >> 16) & 0xff) / 255.0f,
        ((rgb >> 8) & 0xff) / 255.0f,
        (rgb & 0xff) / 255.0f,
        1.0f);
}

// Returns the DPI scale factor for the primary monitor (1.0 = 96 DPI = 100%)
float GetPrimaryMonitorDpiScale() {
    POINT pt = {0, 0};
    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    UINT dpiX = 96, dpiY = 96;
    using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
    static auto pGetDpiForMonitor = reinterpret_cast<GetDpiForMonitor_t>(
        GetProcAddress(GetModuleHandleW(L"shcore.dll"), "GetDpiForMonitor"));
    if (pGetDpiForMonitor) {
        pGetDpiForMonitor(monitor, 0 /* MDT_EFFECTIVE_DPI */, &dpiX, &dpiY);
    }
    return static_cast<float>(dpiX) / 96.0f;
}

D2D1_COLOR_F GetSystemAccentColor() {
    DWORD color = 0;
    BOOL opaque = FALSE;
    using DwmGetColorizationColor_t = HRESULT(WINAPI*)(DWORD*, BOOL*);
    auto proc = reinterpret_cast<DwmGetColorizationColor_t>(
        GetProcAddress(GetModuleHandleW(L"dwmapi.dll"), "DwmGetColorizationColor"));

    if (proc && SUCCEEDED(proc(&color, &opaque))) {
        return D2D1::ColorF(
            ((color >> 16) & 0xff) / 255.0f,
            ((color >> 8) & 0xff) / 255.0f,
            (color & 0xff) / 255.0f,
            1.0f);
    }

    return D2D1::ColorF(0x4cc9f0);
}

void LoadSettings() {
    Settings next;

    const std::wstring position = GetStringSettingCopy(L"Appearance.Position");
    if (EqualsNoCase(position, L"top-left")) {
        next.position = Position::TopLeft;
    } else if (EqualsNoCase(position, L"top-right")) {
        next.position = Position::TopRight;
    } else if (EqualsNoCase(position, L"bottom-center")) {
        next.position = Position::BottomCenter;
    }

    // Quick-panel overrides (right-click menu) take priority over the settings file.
    const int posOverride = Wh_GetIntValue(L"PositionOverride", -1);
    if (posOverride == 0) {
        next.position = Position::TopLeft;
    } else if (posOverride == 1) {
        next.position = Position::TopCenter;
    } else if (posOverride == 2) {
        next.position = Position::TopRight;
    }

    const std::wstring scale = GetStringSettingCopy(L"Appearance.SizeScale");
    if (!scale.empty()) {
        wchar_t* end;
        float parsedScale = wcstof(scale.c_str(), &end);
        if (end != scale.c_str() && parsedScale > 0.1f && parsedScale < 10.0f) {
            next.sizeScale = parsedScale;
        }
    }

    // Auto DPI scaling: multiply sizeScale by monitor DPI factor.
    // On a 4K 200% display this doubles the island to the right physical size.
    if (Wh_GetIntSetting(L"Appearance.AutoDpiScale") != 0) {
        next.sizeScale *= GetPrimaryMonitorDpiScale();
    }

    const std::wstring accentMode = GetStringSettingCopy(L"Themes.AccentColorMode");
    if (EqualsNoCase(accentMode, L"system")) {
        next.accentMode = AccentMode::System;
    } else if (EqualsNoCase(accentMode, L"custom")) {
        next.accentMode = AccentMode::Custom;
    }

    next.customAccent = ColorFromHex(GetStringSettingCopy(L"Themes.CustomAccentHex"), next.customAccent);

    const std::wstring speed = GetStringSettingCopy(L"Appearance.AnimationSpeed");
    if (EqualsNoCase(speed, L"slow")) {
        next.animationSpeed = 0.75f;
    } else if (EqualsNoCase(speed, L"fast")) {
        next.animationSpeed = 1.35f;
    }

    next.media = Wh_GetIntSetting(L"Modules.Media") != 0;
    next.clipboard = Wh_GetIntSetting(L"Modules.Clipboard") != 0;
    next.battery = Wh_GetIntSetting(L"Modules.Battery") != 0;
    next.progress = Wh_GetIntSetting(L"Modules.Progress") != 0;
    next.tintOpacity = Clamp(Wh_GetIntSetting(L"Themes.TintIntensity") / 100.0f, 0.0f, 1.0f);
    const int settingOpacity = Wh_GetIntSetting(L"Themes.PillOpacity");
    const int localOpacity = Wh_GetIntValue(L"PillOpacityOverride", -1);
    next.pillOpacity = Clamp((localOpacity >= 0 ? localOpacity : settingOpacity) / 100.0f,
                             0.35f, 1.0f);
    next.gameOverlay = Wh_GetIntSetting(L"Modules.GameOverlay") != 0;
    next.showMetricText = Wh_GetIntSetting(L"Modules.ShowMetricText") != 0;
    next.gpuTempFahrenheit = EqualsNoCase(GetStringSettingCopy(L"Modules.GameTempUnit"), L"fahrenheit");
    next.gpuTempUpdateMs = ClampInt(Wh_GetIntSetting(L"Modules.GameTempUpdateMs"), 200, 10000);
    next.gameFpsViewer = Wh_GetIntSetting(L"Modules.GameFpsViewer") != 0;
    const int fpsVisible = Wh_GetIntValue(L"FpsVisible", -1);
    if (fpsVisible == 0) {
        next.gameFpsViewer = false;
    } else if (fpsVisible == 1) {
        next.gameFpsViewer = true;
    }
    const int ovTemp = Wh_GetIntValue(L"OverlayTemp", -1);
    const int ovCpu = Wh_GetIntValue(L"OverlayCpu", -1);
    const int ovRam = Wh_GetIntValue(L"OverlayRam", -1);
    const int ovGpu = Wh_GetIntValue(L"OverlayGpu", -1);
    const int ovDisk = Wh_GetIntValue(L"OverlayDisk", -1);
    next.gameOverlayTemp = ovTemp >= 0 ? (ovTemp != 0) : true;
    next.gameOverlayCpu = ovCpu >= 0 ? (ovCpu != 0) : true;
    next.gameOverlayRam = ovRam >= 0 ? (ovRam != 0) : true;
    next.gameOverlayGpu = ovGpu >= 0 ? (ovGpu != 0) : true;
    next.gameOverlayDisk = ovDisk >= 0 ? (ovDisk != 0) : true;
    const std::wstring hideSec = GetStringSettingCopy(L"Appearance.AutoHideIdleSeconds");
    next.autoHideIdleSeconds = hideSec.empty() ? 0 : _wtoi(hideSec.c_str());
    next.unhideOnHover = Wh_GetIntSetting(L"Appearance.UnhideOnHover") != 0;
    next.hideIsland = Wh_GetIntValue(L"IslandHidden", 0) != 0;
    next.alwaysOnTop = Wh_GetIntSetting(L"Appearance.AlwaysOnTop") != 0;
    const int localExpandOnHover = Wh_GetIntValue(L"ExpandOnHoverOverride", -1);
    next.expandOnHover = localExpandOnHover >= 0 ? (localExpandOnHover != 0) : (Wh_GetIntSetting(L"Appearance.ExpandOnHover") != 0);
    next.autoDpiScale = Wh_GetIntSetting(L"Appearance.AutoDpiScale") != 0;
    next.offsetX = Wh_GetIntSetting(L"Appearance.OffsetX");
    next.offsetY = Wh_GetIntSetting(L"Appearance.OffsetY");

    std::wstring mon = GetStringSettingCopy(L"Appearance.TargetMonitor");
    if (mon == L"primary") next.targetMonitor = 0;
    else if (mon == L"follow") next.targetMonitor = -1;
    else next.targetMonitor = _wtoi(mon.c_str());

    const int localW11Style = Wh_GetIntValue(L"W11StyleOverride", -1);
    next.w11Style = localW11Style >= 0 ? (localW11Style != 0) : (Wh_GetIntSetting(L"Appearance.W11Style") != 0);

    // Color settings ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÂ¢Ã¢â€šÂ¬Ã‚Â check local theme override first, then settings YAML.
    struct ThemeColors { const wchar_t* bg; const wchar_t* fg; const wchar_t* sec; };
    static constexpr ThemeColors kThemes[] = {
        {L"#0D0D0F", L"#F7F7F7", L"#888888"},  // 0: OLED Black
        {L"#1C1C1E", L"#FFFFFF", L"#999999"},  // 1: Dark Gray
        {L"#050D1A", L"#E8F0FF", L"#7090BB"},  // 2: Midnight Blue
        {L"#0E0014", L"#F0E8FF", L"#9060BB"},  // 3: Deep Purple
        {L"#1F1F1F", L"#FFFFFF", L"#A0A0A0"},  // 4: Fluent Design
    };
    const int theme = Wh_GetIntValue(L"ColorTheme", -1);
    if (theme >= 0 && theme < static_cast<int>(ARRAYSIZE(kThemes))) {
        next.pillBgColor = ColorFromHex(kThemes[theme].bg,
                                        D2D1::ColorF(0.051f, 0.051f, 0.059f, 1.0f));
        next.textPrimaryColor = ColorFromHex(kThemes[theme].fg,
                                             D2D1::ColorF(0.969f, 0.969f, 0.969f, 1.0f));
        next.textSecondaryColor = ColorFromHex(kThemes[theme].sec,
                                               D2D1::ColorF(0.533f, 0.533f, 0.533f, 1.0f));
    } else {
        next.pillBgColor = ColorFromHex(GetStringSettingCopy(L"Themes.PillBgColor"),
                                        D2D1::ColorF(0.051f, 0.051f, 0.059f, 1.0f));
        next.textPrimaryColor = ColorFromHex(GetStringSettingCopy(L"Themes.TextPrimaryColor"),
                                             D2D1::ColorF(0.969f, 0.969f, 0.969f, 1.0f));
        next.textSecondaryColor = ColorFromHex(GetStringSettingCopy(L"Themes.TextSecondaryColor"),
                                               D2D1::ColorF(0.533f, 0.533f, 0.533f, 1.0f));
    }

    g_settings = next;
    g_layoutDirty = true;
    if (g_settingsChangedEvent) {
        SetEvent(g_settingsChangedEvent);
    }
}

void EnableBlurBehind(HWND hwnd) {
    DWM_BLURBEHIND blur = {};
    blur.dwFlags = DWM_BB_ENABLE;
    blur.fEnable = FALSE;
    DwmEnableBlurBehindWindow(hwnd, &blur);
}



struct MonitorEnumData {
    std::vector<HMONITOR> monitors;
};

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC, LPRECT, LPARAM dwData) {
    auto* data = reinterpret_cast<MonitorEnumData*>(dwData);
    data->monitors.push_back(hMonitor);
    return TRUE;
}

RECT GetAnchorWorkRect() {
    HMONITOR selectedMonitor = nullptr;

    if (g_settings.targetMonitor == -1) {
        POINT pt = {0, 0};
        GetCursorPos(&pt);
        selectedMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    } else if (g_settings.targetMonitor > 0) {
        MonitorEnumData data;
        EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, reinterpret_cast<LPARAM>(&data));
        
        int index = g_settings.targetMonitor - 1;
        if (index >= 0 && index < static_cast<int>(data.monitors.size())) {
            selectedMonitor = data.monitors[index];
        }
    }

    if (!selectedMonitor) {
        POINT pt = {0, 0};
        selectedMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    }

    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(selectedMonitor, &mi);
    return mi.rcWork;
}

void PositionOverlayWindow(HWND hwnd, int width, int height) {
    RECT work = GetAnchorWorkRect();
    int x = work.left + (work.right - work.left - width) / 2;
    int y = work.top + 8;

    switch (g_settings.position) {
        case Position::TopLeft:
            x = work.left + 16;
            y = work.top + 8;
            break;
        case Position::TopRight:
            x = work.right - width - 16;
            y = work.top + 8;
            break;
        case Position::BottomCenter:
            x = work.left + (work.right - work.left - width) / 2;
            y = work.bottom - height - 40;
            break;
        case Position::TopCenter:
        default:
            break;
    }

    HWND zOrder = g_settings.alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST;
    
    // Manage owner window to firmly anchor to desktop when alwaysOnTop is false
    if (g_settings.alwaysOnTop) {
        SetWindowLongPtr(hwnd, GWLP_HWNDPARENT, 0);
    } else {
        HWND hProgman = FindWindowW(L"Progman", nullptr);
        if (hProgman) {
            SetWindowLongPtr(hwnd, GWLP_HWNDPARENT, reinterpret_cast<LONG_PTR>(hProgman));
        }
    }

    x += g_settings.offsetX;
    y += g_settings.offsetY;
    SetWindowPos(hwnd, zOrder, x, y, width, height,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
}

bool DecodeImageBytesToPixels(const std::vector<uint8_t>& bytes, BitmapPixels* outPixels) {
    if (!outPixels || bytes.empty()) {
        return false;
    }

    ComPtr<IWICImagingFactory> factory;
    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&factory));
    if (FAILED(hr)) {
        return false;
    }

    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, bytes.size());
    if (!mem) {
        return false;
    }

    void* locked = GlobalLock(mem);
    memcpy(locked, bytes.data(), bytes.size());
    GlobalUnlock(mem);

    ComPtr<IStream> stream;
    hr = CreateStreamOnHGlobal(mem, TRUE, &stream);
    if (FAILED(hr)) {
        GlobalFree(mem);
        return false;
    }

    ComPtr<IWICBitmapDecoder> decoder;
    hr = factory->CreateDecoderFromStream(stream.Get(), nullptr, WICDecodeMetadataCacheOnLoad,
                                          &decoder);
    if (FAILED(hr)) {
        return false;
    }

    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr)) {
        return false;
    }

    ComPtr<IWICFormatConverter> converter;
    hr = factory->CreateFormatConverter(&converter);
    if (FAILED(hr)) {
        return false;
    }

    hr = converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA,
                               WICBitmapDitherTypeNone, nullptr, 0.0,
                               WICBitmapPaletteTypeCustom);
    if (FAILED(hr)) {
        return false;
    }

    UINT width = 0;
    UINT height = 0;
    converter->GetSize(&width, &height);
    if (!width || !height || width > 2048 || height > 2048) {
        return false;
    }

    BitmapPixels pixels;
    pixels.width = width;
    pixels.height = height;
    pixels.bgra.resize(static_cast<size_t>(width) * height * 4);

    hr = converter->CopyPixels(nullptr, width * 4,
                               static_cast<UINT>(pixels.bgra.size()),
                               pixels.bgra.data());
    if (FAILED(hr)) {
        return false;
    }

    struct Bucket {
        uint32_t count = 0;
        uint32_t r = 0;
        uint32_t g = 0;
        uint32_t b = 0;
    };

    std::array<Bucket, 16 * 16 * 16> buckets{};
    for (size_t i = 0; i + 3 < pixels.bgra.size(); i += 4) {
        const uint8_t alpha = pixels.bgra[i + 3];
        const uint8_t blue = pixels.bgra[i + 0];
        const uint8_t green = pixels.bgra[i + 1];
        const uint8_t red = pixels.bgra[i + 2];
        if (alpha < 32) {
            continue;
        }

        const int maxc = std::max({red, green, blue});
        const int minc = std::min({red, green, blue});
        const int luminance = (54 * red + 183 * green + 19 * blue) / 256;
        const int saturation = maxc - minc;
        if (luminance < 28 || luminance > 232 || saturation < 24) {
            continue;
        }

        const size_t bucketIndex = ((red >> 4) << 8) | ((green >> 4) << 4) | (blue >> 4);
        Bucket& bucket = buckets[bucketIndex];
        const uint32_t weight = 1 + static_cast<uint32_t>(saturation / 48);
        bucket.count += weight;
        bucket.r += red * weight;
        bucket.g += green * weight;
        bucket.b += blue * weight;
    }

    const Bucket* best = nullptr;
    for (const Bucket& bucket : buckets) {
        if (!best || bucket.count > best->count) {
            best = &bucket;
        }
    }

    if (best && best->count > 0) {
        pixels.sampledAccent = D2D1::ColorF(
            Clamp((best->r / static_cast<float>(best->count)) / 255.0f, 0.14f, 1.0f),
            Clamp((best->g / static_cast<float>(best->count)) / 255.0f, 0.14f, 1.0f),
            Clamp((best->b / static_cast<float>(best->count)) / 255.0f, 0.14f, 1.0f),
            1.0f);
    }

    pixels.generation = ++g_artGenerationCounter;
    *outPixels = std::move(pixels);
    return true;
}

bool IconToPixels(HICON icon, UINT size, BitmapPixels* outPixels) {
    if (!icon || !outPixels || !size) {
        return false;
    }

    HDC screen = GetDC(nullptr);
    HDC dc = CreateCompatibleDC(screen);
    ReleaseDC(nullptr, screen);
    if (!dc) {
        return false;
    }

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = static_cast<LONG>(size);
    bi.bmiHeader.biHeight = -static_cast<LONG>(size);
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(dc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap) {
        DeleteDC(dc);
        return false;
    }

    // Fill with transparent black (alpha=0) so icon edges don't bleed dark fringe.
    HGDIOBJ old = SelectObject(dc, bitmap);
    RECT fill = {0, 0, static_cast<LONG>(size), static_cast<LONG>(size)};
    // Use NULL_BRUSH (transparent) then manually zero-fill the BGRA buffer after copy.
    FillRect(dc, &fill, reinterpret_cast<HBRUSH>(GetStockObject(NULL_BRUSH)));
    // Zero out the bits so background is fully transparent before drawing icon.
    ZeroMemory(bits, static_cast<size_t>(size) * size * 4);
    DrawIconEx(dc, 0, 0, icon, size, size, 0, nullptr, DI_NORMAL);

    BitmapPixels pixels;
    pixels.width = size;
    pixels.height = size;
    pixels.bgra.resize(static_cast<size_t>(size) * size * 4);
    memcpy(pixels.bgra.data(), bits, pixels.bgra.size());

    // Older icons can have no alpha in the color bitmap. Treat black pixels as
    // transparent only when the icon did not write any alpha at all.
    bool hasAlpha = false;
    for (size_t i = 3; i < pixels.bgra.size(); i += 4) {
        if (pixels.bgra[i] != 0) {
            hasAlpha = true;
            break;
        }
    }
    if (!hasAlpha) {
        // No alpha channel: treat near-black as transparent, rest as opaque.
        for (size_t i = 0; i + 3 < pixels.bgra.size(); i += 4) {
            const bool black = pixels.bgra[i] < 4 && pixels.bgra[i + 1] < 4 && pixels.bgra[i + 2] < 4;
            pixels.bgra[i + 3] = black ? 0 : 255;
        }
    } else {
        // Convert to premultiplied alpha so D2D renders edges cleanly without dark fringing.
        for (size_t i = 0; i + 3 < pixels.bgra.size(); i += 4) {
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
    if (!pid || !imageName) {
        return false;
    }

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }

    wchar_t path[MAX_PATH] = {};
    DWORD size = ARRAYSIZE(path);
    const bool ok = QueryFullProcessImageNameW(process, 0, path, &size) != FALSE;
    CloseHandle(process);
    if (ok) {
        *imageName = path;
    }
    return ok;
}

HICON CopyWindowIcon(HWND hwnd, WPARAM iconType) {
    DWORD_PTR result = 0;
    SendMessageTimeoutW(hwnd, WM_GETICON, iconType, 0,
                        SMTO_ABORTIFHUNG | SMTO_BLOCK, 80, &result);
    return result ? CopyIcon(reinterpret_cast<HICON>(result)) : nullptr;
}

HICON getProcessIcon(DWORD pid) {
    std::wstring path;
    if (ProcessImageNameForPid(pid, &path) && !path.empty()) {
        HICON hIcon = nullptr;
        UINT iconId = 0;
        // Try to fetch a high-res 64x64 icon first to avoid pixelated icons
        using PrivateExtractIconsW_t = UINT(WINAPI*)(LPCWSTR, int, int, int, HICON*, UINT*, UINT, UINT);
        static auto pPrivateExtractIconsW = reinterpret_cast<PrivateExtractIconsW_t>(
            GetProcAddress(GetModuleHandleW(L"user32.dll"), "PrivateExtractIconsW"));
        if (pPrivateExtractIconsW && pPrivateExtractIconsW(path.c_str(), 0, 64, 64, &hIcon, &iconId, 1, 0) == 1 && hIcon) {
            return hIcon;
        }

        SHFILEINFOW sfi = {};
        if (SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi),
                           SHGFI_ICON | SHGFI_LARGEICON)) {
            return sfi.hIcon;
        }

        HICON large = nullptr;
        HICON small = nullptr;
        if (ExtractIconExW(path.c_str(), 0, &large, &small, 1) > 0) {
            if (small) {
                DestroyIcon(small);
            }
            if (large) {
                return large;
            }
        }
    }

    return CopyIcon(LoadIconW(nullptr, IDI_APPLICATION));
}

HICON getWindowIcon(HWND hwnd) {
    if (!hwnd) {
        return CopyIcon(LoadIconW(nullptr, IDI_APPLICATION));
    }

    if (HICON icon = CopyWindowIcon(hwnd, ICON_BIG)) {
        return icon;
    }
    if (HICON icon = CopyWindowIcon(hwnd, ICON_SMALL)) {
        return icon;
    }

    if (auto icon = reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, GCLP_HICON))) {
        return CopyIcon(icon);
    }
    if (auto icon = reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, GCLP_HICONSM))) {
        return CopyIcon(icon);
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return getProcessIcon(pid);
}

std::wstring ToLowerCopy(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](wchar_t ch) { return static_cast<wchar_t>(towlower(ch)); });
    return value;
}

std::wstring BaseNameFromPath(std::wstring path) {
    const size_t slash = path.find_last_of(L"\\/");
    if (slash != std::wstring::npos) {
        path.erase(0, slash + 1);
    }
    return path;
}

std::wstring StripExtension(std::wstring value) {
    const size_t dot = value.find_last_of(L'.');
    if (dot != std::wstring::npos) {
        value.resize(dot);
    }
    return value;
}

bool ProcessImageNameForWindow(HWND hwnd, std::wstring* imageName) {
    if (!imageName) {
        return false;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) {
        return false;
    }

    return ProcessImageNameForPid(pid, imageName);
}

std::wstring FriendlyMediaSourceName(std::wstring_view source) {
    const std::wstring lower = ToLowerCopy(std::wstring(source));
    if (lower.find(L"youtube") != std::wstring::npos) return L"YouTube";
    if (lower.find(L"spotify") != std::wstring::npos) return L"Spotify";
    if (lower.find(L"chrome") != std::wstring::npos) return L"Chrome";
    if (lower.find(L"msedge") != std::wstring::npos || lower.find(L"edge") != std::wstring::npos) return L"Edge";
    if (lower.find(L"firefox") != std::wstring::npos) return L"Firefox";
    if (lower.find(L"vlc") != std::wstring::npos) return L"VLC";
    if (lower.find(L"wmplayer") != std::wstring::npos) return L"Windows Media";
    if (lower.find(L"zune") != std::wstring::npos || lower.find(L"media") != std::wstring::npos) return L"Media Player";

    std::wstring text(source);
    const size_t bang = text.find(L'!');
    if (bang != std::wstring::npos && bang + 1 < text.size()) {
        text.erase(0, bang + 1);
    }
    const size_t dot = text.find(L'.');
    if (dot != std::wstring::npos) {
        text.resize(dot);
    }
    return text.empty() ? L"Media" : text;
}

std::wstring MediaSourceBadge(std::wstring_view sourceName) {
    const std::wstring lower = ToLowerCopy(std::wstring(sourceName));
    if (lower.find(L"youtube") != std::wstring::npos) return L"YT";
    if (lower.find(L"spotify") != std::wstring::npos) return L"SP";
    if (lower.find(L"chrome") != std::wstring::npos) return L"CH";
    if (lower.find(L"edge") != std::wstring::npos) return L"ED";
    if (lower.find(L"firefox") != std::wstring::npos) return L"FF";
    if (lower.find(L"vlc") != std::wstring::npos) return L"VLC";
    if (sourceName.empty()) return L"\u25b6";
    std::wstring badge;
    badge.push_back(static_cast<wchar_t>(towupper(sourceName[0])));
    return badge;
}

bool WindowLooksLikeMediaSource(HWND hwnd, const std::wstring& sourceLower) {
    if (!IsWindowVisible(hwnd) || hwnd == g_hwnd) {
        return false;
    }

    std::wstring image;
    if (!ProcessImageNameForWindow(hwnd, &image)) {
        return false;
    }

    const std::wstring base = ToLowerCopy(BaseNameFromPath(image));
    if (base.empty()) {
        return false;
    }

    return sourceLower.find(base) != std::wstring::npos ||
           (base.find(L"chrome") != std::wstring::npos && sourceLower.find(L"chrome") != std::wstring::npos) ||
           (base.find(L"msedge") != std::wstring::npos && sourceLower.find(L"edge") != std::wstring::npos) ||
           (base.find(L"vlc") != std::wstring::npos && sourceLower.find(L"vlc") != std::wstring::npos);
}

bool IsBrowserMediaSource(std::wstring_view source) {
    const std::wstring lower = ToLowerCopy(std::wstring(source));
    return lower.find(L"chrome") != std::wstring::npos ||
           lower.find(L"edge") != std::wstring::npos ||
           lower.find(L"msedge") != std::wstring::npos ||
           lower.find(L"firefox") != std::wstring::npos ||
           lower.find(L"youtube") != std::wstring::npos;
}

std::wstring SiteBadgeFromTitle(std::wstring_view title) {
    const std::wstring lower = ToLowerCopy(std::wstring(title));
    if (lower.find(L"youtube") != std::wstring::npos) return L"YT";
    if (lower.find(L"netflix") != std::wstring::npos) return L"NF";
    if (lower.find(L"prime video") != std::wstring::npos || lower.find(L"amazon") != std::wstring::npos) return L"PV";
    if (lower.find(L"disney") != std::wstring::npos) return L"D+";
    if (lower.find(L"hotstar") != std::wstring::npos) return L"HS";
    if (lower.find(L"spotify") != std::wstring::npos) return L"SP";
    if (lower.find(L"soundcloud") != std::wstring::npos) return L"SC";
    return L"WEB";
}

struct IconCacheEntry {
    DWORD pid = 0;
    std::wstring exePath;
    BitmapPixels pixels;
    uint64_t lastUsed = 0;
};

std::mutex g_iconCacheMutex;
std::unordered_map<DWORD, IconCacheEntry> g_iconCacheByPid;
uint64_t g_iconCacheClock = 0;

BitmapPixels GetCachedProcessIconPixels(DWORD pid, UINT size) {
    std::wstring exePath;
    ProcessImageNameForPid(pid, &exePath);

    {
        std::lock_guard lock(g_iconCacheMutex);
        auto it = g_iconCacheByPid.find(pid);
        if (it != g_iconCacheByPid.end() && it->second.exePath == exePath &&
            it->second.pixels.width == size) {
            it->second.lastUsed = ++g_iconCacheClock;
            return it->second.pixels;
        }
    }

    BitmapPixels pixels;
    HICON icon = getProcessIcon(pid);
    if (icon) {
        IconToPixels(icon, size, &pixels);
        DestroyIcon(icon);
    }

    if (!pixels.bgra.empty()) {
        std::lock_guard lock(g_iconCacheMutex);
        g_iconCacheByPid[pid] = IconCacheEntry{pid, exePath, pixels, ++g_iconCacheClock};
        if (g_iconCacheByPid.size() > 32) {
            auto oldest = g_iconCacheByPid.begin();
            for (auto it = g_iconCacheByPid.begin(); it != g_iconCacheByPid.end(); ++it) {
                if (it->second.lastUsed < oldest->second.lastUsed) {
                    oldest = it;
                }
            }
            g_iconCacheByPid.erase(oldest);
        }
    }

    return pixels;
}

BitmapPixels GetWindowIconPixels(HWND hwnd, UINT size) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    BitmapPixels pixels = GetCachedProcessIconPixels(pid, size);
    if (!pixels.bgra.empty()) {
        return pixels;
    }

    HICON icon = getWindowIcon(hwnd);
    if (icon) {
        IconToPixels(icon, size, &pixels);
        DestroyIcon(icon);
    }
    return pixels;
}

bool IsIgnorableForegroundWindow(HWND hwnd, const std::wstring& title) {
    wchar_t className[128] = {};
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    const std::wstring cls = ToLowerCopy(className);
    const std::wstring lowerTitle = ToLowerCopy(title);

    std::wstring image;
    ProcessImageNameForWindow(hwnd, &image);
    const std::wstring base = ToLowerCopy(BaseNameFromPath(image));

    return title.empty() ||
           hwnd == g_hwnd ||
           cls == L"shell_traywnd" ||
           cls == L"workerw" ||
           cls == L"progman" ||
           lowerTitle.find(L"windhawk") != std::wstring::npos ||
           base == L"explorer.exe" ||
           base == L"windhawk.exe";
}

BOOL CALLBACK FindMediaSourceWindowProc(HWND hwnd, LPARAM lParam) {
    auto* data = reinterpret_cast<std::pair<const std::wstring*, HWND*>*>(lParam);
    if (WindowLooksLikeMediaSource(hwnd, *data->first)) {
        *data->second = hwnd;
        return FALSE;
    }
    return TRUE;
}

std::wstring FindBrowserMediaSiteBadge(const std::wstring& sourceAppUserModelId) {
    const std::wstring sourceLower = ToLowerCopy(sourceAppUserModelId);
    HWND found = nullptr;
    std::pair<const std::wstring*, HWND*> data{&sourceLower, &found};
    EnumWindows(FindMediaSourceWindowProc, reinterpret_cast<LPARAM>(&data));
    if (found) {
        wchar_t title[192] = {};
        GetWindowTextW(found, title, ARRAYSIZE(title));
        return SiteBadgeFromTitle(title);
    }
    return L"WEB";
}

BitmapPixels FindMediaSourceIcon(const std::wstring& sourceAppUserModelId) {
    BitmapPixels pixels;
    if (IsBrowserMediaSource(sourceAppUserModelId)) {
        // Continue searching for the browser window anyway to retrieve
        // the browser's app icon or the PWA's app icon.
    }

    const std::wstring sourceLower = ToLowerCopy(sourceAppUserModelId);
    HWND found = nullptr;
    std::pair<const std::wstring*, HWND*> data{&sourceLower, &found};
    EnumWindows(FindMediaSourceWindowProc, reinterpret_cast<LPARAM>(&data));
    if (found) {
        pixels = GetWindowIconPixels(found, 32);
    }
    return pixels;
}

struct FindAppIconData {
    const std::wstring* targetName;
    HWND bestHwnd = nullptr;
    std::unordered_map<DWORD, std::wstring> pidToProcessName;
};

BOOL CALLBACK FindAppIconWindowProc(HWND hwnd, LPARAM lParam) {
    auto* d = reinterpret_cast<FindAppIconData*>(lParam);
    if (!IsWindowVisible(hwnd)) {
        return TRUE;
    }

    if (hwnd == g_hwnd) {
        return TRUE;
    }

    // 1. Fast path: Check window title first (lightweight check without opening process handles)
    wchar_t title[128] = {};
    GetWindowTextW(hwnd, title, ARRAYSIZE(title));
    std::wstring titleLower = ToLowerCopy(title);
    if (!titleLower.empty() && titleLower.find(*d->targetName) != std::wstring::npos) {
        d->bestHwnd = hwnd;
        return FALSE; // Found via title, stop enumeration
    }

    // 2. Slow path fallback: Check process executable name
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) {
        return TRUE;
    }

    std::wstring baseNoExe;
    auto it = d->pidToProcessName.find(pid);
    if (it != d->pidToProcessName.end()) {
        baseNoExe = it->second;
    } else {
        std::wstring image;
        if (ProcessImageNameForPid(pid, &image)) {
            std::wstring base = ToLowerCopy(BaseNameFromPath(image));
            baseNoExe = base;
            if (baseNoExe.size() > 4 && baseNoExe.substr(baseNoExe.size() - 4) == L".exe") {
                baseNoExe = baseNoExe.substr(0, baseNoExe.size() - 4);
            }
            d->pidToProcessName[pid] = baseNoExe;
        } else {
            d->pidToProcessName[pid] = L"";
        }
    }

    if (!baseNoExe.empty()) {
        if (baseNoExe.find(*d->targetName) != std::wstring::npos ||
            d->targetName->find(baseNoExe) != std::wstring::npos) {
            d->bestHwnd = hwnd;
            return FALSE; // Found via process name, stop enumeration
        }
    }

    return TRUE;
}

BitmapPixels FindAppIconByName(const std::wstring& appName, UINT size) {
    BitmapPixels pixels;
    if (appName.empty()) {
        return pixels;
    }

    const std::wstring appNameLower = ToLowerCopy(appName);
    FindAppIconData data;
    data.targetName = &appNameLower;

    EnumWindows(FindAppIconWindowProc, reinterpret_cast<LPARAM>(&data));

    if (data.bestHwnd) {
        pixels = GetWindowIconPixels(data.bestHwnd, size);
    }
    return pixels;
}

std::vector<uint8_t> ReadWinRtStreamBytes(
    const winrt::Windows::Storage::Streams::IRandomAccessStreamReference& reference) {
    std::vector<uint8_t> bytes;
    if (!reference) {
        return bytes;
    }

    auto stream = reference.OpenReadAsync().get();
    if (!stream) {
        return bytes;
    }

    const uint64_t size64 = stream.Size();
    if (size64 == 0 || size64 > 8 * 1024 * 1024) {
        return bytes;
    }

    const uint32_t size = static_cast<uint32_t>(size64);
    winrt::Windows::Storage::Streams::DataReader reader(stream.GetInputStreamAt(0));
    reader.LoadAsync(size).get();
    bytes.resize(size);
    reader.ReadBytes(winrt::array_view<uint8_t>(bytes.data(), bytes.data() + bytes.size()));
    return bytes;
}

void TriggerNudge() {
    const double now = NowSeconds();
    const double previous = g_lastNudgeTime.load();
    if (now - previous < 0.45) {
        return;
    }
    g_lastNudgeTime = now;
    HWND hwnd = g_hwnd;
    if (hwnd) {
        PostMessageW(hwnd, WM_APP_NEW_EVENT, 0, 0);
    }
}

DWORD WINAPI MediaThreadProc(void*) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

    using Manager = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager;
    using PlaybackStatus = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackStatus;

    Manager manager{nullptr};
    bool loggedUnavailable = false;

    while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        MediaSnapshot next;

        try {
            if (!manager) {
                manager = Manager::RequestAsync().get();
            }

            if (manager) {
                auto session = manager.GetCurrentSession();
                if (session) {
                    auto properties = session.TryGetMediaPropertiesAsync().get();
                    auto playback = session.GetPlaybackInfo();
                    auto timeline = session.GetTimelineProperties();

                    next.available = true;
                    next.playing = playback.PlaybackStatus() == PlaybackStatus::Playing;
                    next.title = properties.Title().c_str();
                    next.artist = properties.Artist().c_str();
                    
                    if (timeline) {
                        int64_t np = timeline.Position().count();
                        int64_t ne = timeline.EndTime().count();
                        bool npP = (playback.PlaybackStatus() == PlaybackStatus::Playing);
                        
                        std::lock_guard lock(g_stateMutex);
                        if (np != g_state.media.positionTicks ||
                            ne != g_state.media.endTicks ||
                            npP != g_state.media.playing) {
                            next.positionTicks = np;
                            next.endTicks = ne;
                            next.lastUpdatedTicks = GetTickCount64();
                        } else {
                            next.positionTicks = g_state.media.positionTicks;
                            next.endTicks = g_state.media.endTicks;
                            next.lastUpdatedTicks = g_state.media.lastUpdatedTicks;
                        }
                    }

                    next.sourceAppUserModelId = session.SourceAppUserModelId().c_str();
                    next.sourceName = FriendlyMediaSourceName(next.sourceAppUserModelId);
                    
                    std::wstring prevSourceAppUserModelId;
                    bool hasPrevIcon = false;
                    BitmapPixels prevIcon;
                    uint64_t prevIconGeneration = 0;
                    std::wstring prevBadge;
                    {
                        std::lock_guard lock(g_stateMutex);
                        prevSourceAppUserModelId = g_state.media.sourceAppUserModelId;
                        hasPrevIcon = !g_state.media.sourceIcon.bgra.empty();
                        prevIcon = g_state.media.sourceIcon;
                        prevIconGeneration = g_state.media.sourceIconGeneration;
                        prevBadge = g_state.media.sourceBadge;
                    }

                    if (next.sourceAppUserModelId == prevSourceAppUserModelId) {
                        next.sourceBadge = prevBadge;
                        next.sourceIcon = prevIcon;
                        next.sourceIconGeneration = prevIconGeneration;
                        
                        if (!hasPrevIcon) {
                            next.sourceIcon = FindMediaSourceIcon(next.sourceAppUserModelId);
                            next.sourceIconGeneration = next.sourceIcon.generation;
                        }
                    } else {
                        next.sourceBadge = MediaSourceBadge(next.sourceName);
                        if (IsBrowserMediaSource(next.sourceAppUserModelId)) {
                            next.sourceBadge = FindBrowserMediaSiteBadge(next.sourceAppUserModelId);
                        }
                        next.sourceIcon = FindMediaSourceIcon(next.sourceAppUserModelId);
                        next.sourceIconGeneration = next.sourceIcon.generation;
                    }

                    if (auto thumbnail = properties.Thumbnail()) {
                        std::vector<uint8_t> bytes = ReadWinRtStreamBytes(thumbnail);
                        if (!bytes.empty()) {
                            BitmapPixels decoded;
                            if (DecodeImageBytesToPixels(bytes, &decoded)) {
                                next.art = std::move(decoded);
                                next.artGeneration = next.art.generation;
                                next.artChangedAt = NowSeconds();
                            }
                        }
                    }
                }
            }
        } catch (...) {
            if (!loggedUnavailable) {
                Wh_Log(L"WinRT media session unavailable; media module will fall back to idle.");
                loggedUnavailable = true;
            }
        }

        {
            std::lock_guard lock(g_stateMutex);
            const bool wasDifferent =
                next.playing != g_state.media.playing ||
                next.title != g_state.media.title ||
                next.artist != g_state.media.artist;

            if (!g_state.media.art.bgra.empty() &&
                next.title == g_state.media.title && next.artist == g_state.media.artist) {
                next.art = g_state.media.art;
                next.artGeneration = g_state.media.artGeneration;
                next.artChangedAt = g_state.media.artChangedAt;
            }
            if (next.sourceIcon.bgra.empty() &&
                next.sourceAppUserModelId == g_state.media.sourceAppUserModelId) {
                next.sourceIcon = g_state.media.sourceIcon;
                next.sourceIconGeneration = g_state.media.sourceIconGeneration;
            }

            g_state.media = std::move(next);
            if (wasDifferent && g_state.media.available) {
                TriggerNudge();
            }
        }

        WaitForSingleObject(g_stopEvent, 1500);
    }

    winrt::uninit_apartment();
    return 0;
}

#if DYNAMIC_ISLAND_HAS_USER_NOTIFICATION_LISTENER
DWORD WINAPI NotificationThreadProc(void*) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

    using winrt::Windows::UI::Notifications::KnownNotificationBindings;
    using winrt::Windows::UI::Notifications::NotificationKinds;
    using winrt::Windows::UI::Notifications::Management::UserNotificationListener;
    using winrt::Windows::UI::Notifications::Management::UserNotificationListenerAccessStatus;

    uint32_t lastSeenId = 0;
    bool accessLogged = false;

    try {
        auto listener = UserNotificationListener::Current();
        auto access = listener.RequestAccessAsync().get();
        if (access != UserNotificationListenerAccessStatus::Allowed) {
            Wh_Log(L"Notification listener permission not granted; shell-hook notification fallback remains active.");
            winrt::uninit_apartment();
            return 0;
        }

        while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
            try {
                auto notifications = listener.GetNotificationsAsync(NotificationKinds::Toast).get();
                for (uint32_t i = 0; i < notifications.Size(); ++i) {
                    auto userNotification = notifications.GetAt(i);
                    const uint32_t id = userNotification.Id();
                    if (id <= lastSeenId) {
                        continue;
                    }

                    NotificationSnapshot snapshot;
                    snapshot.active = true;
                    snapshot.expiresAt = NowSeconds() + 4.0;
                    auto appInfo = userNotification.AppInfo();
                    auto displayInfo = appInfo.DisplayInfo();
                    snapshot.app = displayInfo.DisplayName().c_str();
                    try {
                        auto logo = displayInfo.GetLogo({32.0f, 32.0f});
                        std::vector<uint8_t> logoBytes = ReadWinRtStreamBytes(logo);
                        if (!logoBytes.empty()) {
                            DecodeImageBytesToPixels(logoBytes, &snapshot.icon);
                        }
                    } catch (...) {
                    }

                    if (snapshot.icon.bgra.empty() && !snapshot.app.empty()) {
                        snapshot.icon = FindAppIconByName(snapshot.app, 64);
                    }

                    auto notification = userNotification.Notification();
                    auto binding = notification.Visual().GetBinding(KnownNotificationBindings::ToastGeneric());
                    if (binding) {
                        auto textElements = binding.GetTextElements();
                        if (textElements.Size() > 0) {
                            snapshot.title = textElements.GetAt(0).Text().c_str();
                        }
                        if (textElements.Size() > 1) {
                            snapshot.body = textElements.GetAt(1).Text().c_str();
                        }
                    }

                    if (snapshot.title.empty()) {
                        snapshot.title = snapshot.app.empty() ? L"New notification" : snapshot.app;
                    }
                    if (!snapshot.body.empty()) {
                        snapshot.title += L" - " + snapshot.body;
                    }
                    if (snapshot.title.size() > 120) {
                        snapshot.title.resize(120);
                        snapshot.title += L"...";
                    }

                    {
                        std::lock_guard lock(g_stateMutex);
                        g_state.notification = std::move(snapshot);
                    }
                    TriggerNudge();
                    lastSeenId = id;
                }
            } catch (const winrt::hresult_error& ex) {
                const HRESULT hr = ex.to_abi();
                Wh_Log(L"NotificationThreadProc loop WinRT error: %s (0x%08X)", ex.message().c_str(), hr);
                if (hr == E_NOTIMPL || hr == E_ACCESSDENIED || hr == REGDB_E_CLASSNOTREG) {
                    Wh_Log(L"NotificationThreadProc: Fatal/Unsupported WinRT context. Exiting WinRT notification listener thread.");
                    break;
                }
            } catch (...) {
                Wh_Log(L"NotificationThreadProc loop unknown exception.");
            }

            WaitForSingleObject(g_stopEvent, 1000);
        }
    } catch (const winrt::hresult_error& ex) {
        Wh_Log(L"NotificationThreadProc initialization WinRT error: %s (0x%08X). Falling back to shell hook.", ex.message().c_str(), ex.to_abi());
    } catch (...) {
        Wh_Log(L"NotificationThreadProc initialization unknown exception. Falling back to shell hook.");
    }

    winrt::uninit_apartment();
    return 0;
}
#endif

float SampleAudioAmplitude(BYTE* data, UINT32 frames, WAVEFORMATEX* format) {
    if (!data || !frames || !format || !format->nChannels) {
        return 0.0f;
    }

    double sum = 0.0;
    size_t samples = static_cast<size_t>(frames) * format->nChannels;

    if (format->wFormatTag == WAVE_FORMAT_IEEE_FLOAT ||
        (format->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
         format->cbSize >= sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX) &&
         IsEqualGUID(reinterpret_cast<WAVEFORMATEXTENSIBLE*>(format)->SubFormat,
                     kSubTypeIeeeFloat))) {
        auto* f = reinterpret_cast<float*>(data);
        for (size_t i = 0; i < samples; ++i) {
            sum += f[i] * f[i];
        }
    } else if (format->wBitsPerSample == 16) {
        auto* s = reinterpret_cast<int16_t*>(data);
        for (size_t i = 0; i < samples; ++i) {
            const double v = s[i] / 32768.0;
            sum += v * v;
        }
    }

    const double rms = samples ? std::sqrt(sum / samples) : 0.0;
    return Clamp(static_cast<float>(rms * 4.0), 0.0f, 1.0f);
}

void PushWaveformSample(float amplitude) {
    std::lock_guard lock(g_stateMutex);
    
    float lastVal = 0.0f;
    if (g_state.waveformWrite > 0) {
        lastVal = g_state.waveform[(g_state.waveformWrite - 1) % g_state.waveform.size()];
    }

    // Apply an attack/release envelope (Exponential Moving Average)
    // Quick snappy attack (0.7) for beats, buttery smooth release (0.85) for decay.
    float smoothed;
    if (amplitude > lastVal) {
        smoothed = lastVal * 0.3f + amplitude * 0.7f;
    } else {
        smoothed = lastVal * 0.85f + amplitude * 0.15f;
    }

    g_state.waveform[g_state.waveformWrite % g_state.waveform.size()] = smoothed;
    ++g_state.waveformWrite;
}

void PushAudioChunks(BYTE* data, UINT32 frames, WAVEFORMATEX* format) {
    if (!data || !frames || !format || !format->nChannels) {
        PushWaveformSample(0.0f);
        return;
    }

    constexpr UINT32 chunkFrames = 64;
    const UINT32 channels = format->nChannels;
    const bool isFloat =
        format->wFormatTag == WAVE_FORMAT_IEEE_FLOAT ||
        (format->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
         format->cbSize >= sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX) &&
         IsEqualGUID(reinterpret_cast<WAVEFORMATEXTENSIBLE*>(format)->SubFormat,
                     kSubTypeIeeeFloat));
    const bool is16 = format->wBitsPerSample == 16;

    if (!isFloat && !is16) {
        PushWaveformSample(SampleAudioAmplitude(data, frames, format));
        return;
    }

    for (UINT32 frame = 0; frame < frames; frame += chunkFrames) {
        const UINT32 chunk = std::min(chunkFrames, frames - frame);
        double sum = 0.0;
        const size_t samples = static_cast<size_t>(chunk) * channels;
        const size_t start = static_cast<size_t>(frame) * channels;

        if (isFloat) {
            auto* f = reinterpret_cast<float*>(data);
            for (size_t i = 0; i < samples; ++i) {
                const double v = f[start + i];
                sum += v * v;
            }
        } else {
            auto* s = reinterpret_cast<int16_t*>(data);
            for (size_t i = 0; i < samples; ++i) {
                const double v = s[start + i] / 32768.0;
                sum += v * v;
            }
        }

        PushWaveformSample(Clamp(static_cast<float>(std::sqrt(sum / samples) * 4.0), 0.0f, 1.0f));
    }
}

// --- RAM Cleaner Helpers ---
DWORD WINAPI RamCleanThreadProc(void*) {
    MEMORYSTATUSEX before = {};
    before.dwLength = sizeof(before);
    GlobalMemoryStatusEx(&before);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        g_ramFreedBytes = 0;
        g_ramCleaning = false;
        return 0;
    }

    PROCESSENTRY32W entry = {};
    entry.dwSize = sizeof(entry);
    if (Process32FirstW(snapshot, &entry)) {
        do {
            HANDLE proc = OpenProcess(PROCESS_SET_QUOTA | PROCESS_QUERY_INFORMATION, FALSE, entry.th32ProcessID);
            if (proc) {
                EmptyWorkingSet(proc);
                CloseHandle(proc);
            }
        } while (Process32NextW(snapshot, &entry));
    }
    CloseHandle(snapshot);

    MEMORYSTATUSEX after = {};
    after.dwLength = sizeof(after);
    GlobalMemoryStatusEx(&after);

    const DWORDLONG freed = before.ullAvailPhys >= after.ullAvailPhys
                                ? (after.ullAvailPhys - before.ullAvailPhys)
                                : 0;
    g_ramFreedBytes = freed;
    g_ramCleanedAt = NowSeconds();
    g_ramCleaning = false;
    return 0;
}

void RamCleanNow() {
    bool expected = false;
    if (!g_ramCleaning.compare_exchange_strong(expected, true)) {
        return;
    }
    std::thread([]() {
        RamCleanThreadProc(nullptr);
    }).detach();
}

DWORD WINAPI AudioThreadProc(void*) {
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        ComPtr<IMMDeviceEnumerator> enumerator;
        ComPtr<IMMDevice> device;
        ComPtr<IAudioClient> client;
        ComPtr<IAudioCaptureClient> capture;
        WAVEFORMATEX* mixFormat = nullptr;

        HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                      CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
        if (SUCCEEDED(hr)) {
            hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
        }
        if (SUCCEEDED(hr)) {
            hr = device->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr,
                                  reinterpret_cast<void**>(client.GetAddressOf()));
        }
        if (SUCCEEDED(hr)) {
            hr = client->GetMixFormat(&mixFormat);
        }
        if (SUCCEEDED(hr)) {
            REFERENCE_TIME bufferDuration = 10000000 / 5;
            hr = client->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                    AUDCLNT_STREAMFLAGS_LOOPBACK,
                                    bufferDuration, 0, mixFormat, nullptr);
        }
        if (SUCCEEDED(hr)) {
            hr = client->GetService(IID_PPV_ARGS(&capture));
        }
        if (SUCCEEDED(hr)) {
            hr = client->Start();
        }

        if (FAILED(hr)) {
            if (mixFormat) {
                CoTaskMemFree(mixFormat);
            }
            WaitForSingleObject(g_stopEvent, 2500);
            continue;
        }

        while (WaitForSingleObject(g_stopEvent, 16) == WAIT_TIMEOUT) {
            UINT32 packetFrames = 0;
            if (FAILED(capture->GetNextPacketSize(&packetFrames))) {
                break;
            }

            float amplitude = 0.0f;
            int packets = 0;
            while (packetFrames > 0) {
                BYTE* data = nullptr;
                UINT32 frames = 0;
                DWORD flags = 0;
                if (FAILED(capture->GetBuffer(&data, &frames, &flags, nullptr, nullptr))) {
                    break;
                }

                if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT)) {
                    amplitude = std::max(amplitude, SampleAudioAmplitude(data, frames, mixFormat));
                    PushAudioChunks(data, frames, mixFormat);
                } else {
                    PushWaveformSample(0.0f);
                }
                capture->ReleaseBuffer(frames);
                ++packets;

                if (FAILED(capture->GetNextPacketSize(&packetFrames))) {
                    packetFrames = 0;
                }
            }

            if (packets > 0) {
                if (amplitude <= 0.001f) {
                    PushWaveformSample(0.0f);
                }
            } else {
                PushWaveformSample(0.0f);
            }
        }

        client->Stop();
        if (mixFormat) {
            CoTaskMemFree(mixFormat);
        }
    }

    if (SUCCEEDED(hrCo)) {
        CoUninitialize();
    }

    return 0;
}

void UpdateBatterySnapshot() {
    SYSTEM_POWER_STATUS status = {};
    if (!GetSystemPowerStatus(&status)) {
        return;
    }

    bool newCharging = (status.ACLineStatus == 1);
    int newPercent = status.BatteryLifePercent == 255 ? 100 : status.BatteryLifePercent;

    bool triggerAlert = false;

    {
        std::lock_guard lock(g_stateMutex);
        static bool s_batteryInit = false;
        if (!s_batteryInit) {
            g_state.battery.charging = newCharging;
            g_state.battery.percent = newPercent;
            s_batteryInit = true;
        }

        if (g_state.battery.charging != newCharging) {
            triggerAlert = true;
        }

        if (!newCharging && newPercent < g_state.battery.percent && (newPercent == 20 || newPercent == 10)) {
            triggerAlert = true;
        }

        g_state.battery.charging = newCharging;
        g_state.battery.percent = newPercent;
        g_state.battery.secondsRemaining = status.BatteryLifeTime;
        g_state.battery.low = (!newCharging && newPercent <= 20);

        if (triggerAlert) {
            g_state.battery.active = true;
            g_state.battery.expiresAt = NowSeconds() + 4.0;
        }
    }

    if (triggerAlert) {
        TriggerNudge();
    }
}

ULONGLONG FileTimeToUInt64(FILETIME ft) {
    ULARGE_INTEGER value = {};
    value.LowPart = ft.dwLowDateTime;
    value.HighPart = ft.dwHighDateTime;
    return value.QuadPart;
}

static PDH_HQUERY g_gpuQuery = NULL;
static PDH_HCOUNTER g_gpuCounter = NULL;

static std::atomic<int> g_gpuTempCelsius{-1};

static void InitGpuQuery() {
    if (g_gpuQuery == NULL) {
        if (PdhOpenQueryW(NULL, 0, &g_gpuQuery) == ERROR_SUCCESS) {
            PdhAddEnglishCounterW(g_gpuQuery, L"\\GPU Engine(*)\\Utilization Percentage", 0, &g_gpuCounter);
            PdhCollectQueryData(g_gpuQuery);
        }
    }
}

static int GetGpuUsage() {
    InitGpuQuery();
    if (!g_gpuQuery || !g_gpuCounter) return 0;
    
    PdhCollectQueryData(g_gpuQuery);
    
    DWORD bufferSize = 0;
    DWORD itemCount = 0;
    PdhGetFormattedCounterArrayW(g_gpuCounter, PDH_FMT_DOUBLE, &bufferSize, &itemCount, NULL);
    
    if (bufferSize > 0) {
        std::vector<BYTE> buffer(bufferSize);
        PDH_FMT_COUNTERVALUE_ITEM_W* items = reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(buffer.data());
        
        if (PdhGetFormattedCounterArrayW(g_gpuCounter, PDH_FMT_DOUBLE, &bufferSize, &itemCount, items) == ERROR_SUCCESS) {
            double total = 0;
            for (DWORD i = 0; i < itemCount; i++) {
                if (items[i].szName && wcsstr(items[i].szName, L"engtype_3D")) {
                    total += items[i].FmtValue.doubleValue;
                }
            }
            return ClampInt(static_cast<int>(total), 0, 100);
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// GPU temperature support for the Game Overlay ("GPU temp" chip).
// Source 1: NVIDIA driver CLI "nvidia-smi.exe" (ships with the NVIDIA driver).
// Read through a short-lived child process to avoid crashing the mod's tool
// process with in-process NvAPI initialization.
// Source 2: PDH "\Thermal Zone(*)\Temperature" counter (covers AMD/Intel and
// integrated GPUs that expose an ACPI thermal zone). Falls back gracefully.
// ---------------------------------------------------------------------------

static int GetGpuTemperatureFromNvidiaSmi() {
    wchar_t systemDir[MAX_PATH] = {};
    if (GetSystemDirectoryW(systemDir, MAX_PATH) == 0) {
        return -1;
    }

    const std::wstring exePath = std::wstring(systemDir) + L"\\nvidia-smi.exe";
    if (GetFileAttributesW(exePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        return -1;
    }

    const std::wstring arguments =
        L"\"" + exePath + L"\" --query-gpu=temperature.gpu --format=csv,noheader,nounits";

    SECURITY_ATTRIBUTES sa = {};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;
    HANDLE readPipe = nullptr;
    HANDLE writePipe = nullptr;
    if (!CreatePipe(&readPipe, &writePipe, &sa, 0)) {
        return -1;
    }
    SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = writePipe;
    si.hStdError = writePipe;

    PROCESS_INFORMATION pi = {};
    std::vector<wchar_t> cmdLine(arguments.begin(), arguments.end());
    cmdLine.push_back(0);
    const BOOL created = CreateProcessW(nullptr, cmdLine.data(), nullptr, nullptr,
                                        TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);
    CloseHandle(writePipe);
    if (!created) {
        CloseHandle(readPipe);
        return -1;
    }
    CloseHandle(pi.hThread);

    WaitForSingleObject(pi.hProcess, 3000);

    char buffer[128] = {};
    DWORD bytesRead = 0;
    ReadFile(readPipe, buffer, sizeof(buffer) - 1, &bytesRead, nullptr);
    CloseHandle(readPipe);

    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    CloseHandle(pi.hProcess);

    if (exitCode != 0) {
        return -1;
    }

    buffer[bytesRead] = '\0';
    int temperature = -1;
    bool digits = false;
    for (const char* p = buffer; *p; ++p) {
        if (*p >= '0' && *p <= '9') {
            const int digit = *p - '0';
            if (!digits) {
                temperature = digit;
                digits = true;
            } else if (temperature < 100) {
                temperature = temperature * 10 + digit;
            } else {
                return -1;
            }
        } else if (digits) {
            break;
        }
    }

    const int result = (digits && temperature >= 0 && temperature <= 200) ? temperature : -1;
    DiLog(L"SMI temp: raw='%hs' result=%d", buffer, result);
    return result;
}

static PDH_HQUERY g_gpuTempQuery = NULL;
static PDH_HCOUNTER g_gpuTempCounter = NULL;

static void InitGpuTempQuery() {
    if (g_gpuTempQuery != NULL) {
        return;
    }

    if (PdhOpenQueryW(NULL, 0, &g_gpuTempQuery) == ERROR_SUCCESS) {
        PdhAddEnglishCounterW(g_gpuTempQuery, L"\\Thermal Zone(*)\\Temperature", 0, &g_gpuTempCounter);
        PdhCollectQueryData(g_gpuTempQuery);
    }
}

static int GetGpuTemperatureFromPdh() {
    InitGpuTempQuery();
    DiLog(L"PDH temp: query=0x%p counter=0x%p", (void*)g_gpuTempQuery, (void*)g_gpuTempCounter);
    if (!g_gpuTempQuery || !g_gpuTempCounter) {
        return -1;
    }

    const PDH_STATUS collectStatus = PdhCollectQueryData(g_gpuTempQuery);
    DiLog(L"PDH temp: collect=%d", static_cast<int>(collectStatus));

    DWORD bufferSize = 0;
    DWORD itemCount = 0;
    const PDH_STATUS firstStatus =
        PdhGetFormattedCounterArrayW(g_gpuTempCounter, PDH_FMT_DOUBLE, &bufferSize, &itemCount, nullptr);
    DiLog(L"PDH temp: first status=%d buff=%u cnt=%u", static_cast<int>(firstStatus), bufferSize, itemCount);
    if (firstStatus != ERROR_SUCCESS || bufferSize == 0 || itemCount == 0) {
        return -1;
    }

    std::vector<BYTE> buffer(bufferSize);
    PDH_FMT_COUNTERVALUE_ITEM_W* items =
        reinterpret_cast<PDH_FMT_COUNTERVALUE_ITEM_W*>(buffer.data());

    const PDH_STATUS secondStatus = PdhGetFormattedCounterArrayW(
        g_gpuTempCounter, PDH_FMT_DOUBLE, &bufferSize, &itemCount, items);
    DiLog(L"PDH temp: second status=%d cnt=%u", static_cast<int>(secondStatus), itemCount);
    if (secondStatus != ERROR_SUCCESS) {
        return -1;
    }

    // Thermal zone temperatures are reported in tenths of a degree Kelvin.
    // Prefer a zone with "GPU" in its instance name; otherwise take the hottest zone.
    int hottest = -1;
    for (DWORD i = 0; i < itemCount; ++i) {
        if (items[i].FmtValue.CStatus != ERROR_SUCCESS ||
            items[i].FmtValue.doubleValue <= 0.0) {
            continue;
        }

        const int celsius = static_cast<int>((items[i].FmtValue.doubleValue / 10.0) - 273.15 + 0.5);
        if (items[i].szName && wcsstr(items[i].szName, L"GPU")) {
            DiLog(L"PDH temp: returning %d C (GPU zone)", celsius);
            return celsius;
        }
        if (celsius > hottest) {
            hottest = celsius;
        }
    }

    DiLog(L"PDH temp: returning hottest=%d", hottest);
    return hottest;
}

static int GetGpuTemperatureCelsius() {
    DiLog(L"GetGpuTemperatureCelsius enter");
    const int smiTemp = GetGpuTemperatureFromNvidiaSmi();
    if (smiTemp > 0) {
        return smiTemp;
    }
    return GetGpuTemperatureFromPdh();
}

// ---------------------------------------------------------------------------
// Real game FPS support via ETW (Event Tracing for Windows).
// We consume "Present" events for the foreground process from the DXGI, D3D9
// and DxgKrnl providers (the same passive technique used by PresentMon, so no
// hooks or injections into the game). The providers can only be enabled by an
// elevated session, so if Windhawk is not running as administrator the FPS
// chip gracefully falls back to "--".
// ---------------------------------------------------------------------------

#ifndef PROCESS_TRACE_MODE_EVENT_RECORD
#define PROCESS_TRACE_MODE_EVENT_RECORD 0x10000000
#endif
#ifndef PROCESS_TRACE_MODE_REAL_TIME
#define PROCESS_TRACE_MODE_REAL_TIME 0x00000100
#endif
#ifndef EVENT_CONTROL_CODE_ENABLE_PROVIDER
#define EVENT_CONTROL_CODE_ENABLE_PROVIDER 1
#endif

static const GUID kDxgiProvider = {0xCA11C036, 0x0102, 0x4A2D, {0xA6, 0xAD, 0xF0, 0x3C, 0xFE, 0xD5, 0xD3, 0xC9}};
static const GUID kD3D9Provider = {0x783ACA0A, 0x790E, 0x4D7F, {0x84, 0x51, 0xAA, 0x85, 0x05, 0x11, 0xC6, 0xB9}};
static const GUID kDxgKrnlProvider = {0x802EC45A, 0x1E99, 0x4B83, {0x99, 0x20, 0x87, 0xC9, 0x82, 0x77, 0xBA, 0x9D}};

static const USHORT kDxgPresentEvent = 184;  // Present::Info
static const USHORT kDxgFlipEvent = 168;     // Flip::Info
static const USHORT kDxgBlitEvent = 166;     // Blit::Info

static const wchar_t kEtwSessionName[] = L"DynamicIslandFps";

static TRACEHANDLE g_etwSession = 0;
static TRACEHANDLE g_etwTrace = 0;
static HANDLE g_etwThread = nullptr;
static std::atomic<bool> g_etwRunning{false};
static std::atomic<bool> g_etwReady{false};
static std::atomic<DWORD> g_fpsTargetPid{0};
static std::atomic<int> g_gameFps{-1};
static double g_etwQpcFreq = 1.0;

static void WINAPI EtwFpsCallback(PEVENT_RECORD pEvent) {
    if (!g_etwRunning.load(std::memory_order_relaxed)) {
        return;
    }

    const DWORD pid = pEvent->EventHeader.ProcessId;
    const DWORD target = g_fpsTargetPid.load(std::memory_order_relaxed);
    if (target == 0 || pid != target) {
        return;
    }

    bool isPresent = false;
    bool isDxgi = false;
    bool isD3D9 = false;
    bool isDxgKrnl = false;

    if (memcmp(&pEvent->EventHeader.ProviderId, &kDxgiProvider, sizeof(GUID)) == 0) {
        if (pEvent->EventHeader.EventDescriptor.Id == 42) {  // PresentStart
            isPresent = true;
            isDxgi = true;
        }
    } else if (memcmp(&pEvent->EventHeader.ProviderId, &kD3D9Provider, sizeof(GUID)) == 0) {
        if (pEvent->EventHeader.EventDescriptor.Id == 1) {  // PresentStart
            isPresent = true;
            isD3D9 = true;
        }
    } else if (memcmp(&pEvent->EventHeader.ProviderId, &kDxgKrnlProvider, sizeof(GUID)) == 0) {
        const USHORT id = pEvent->EventHeader.EventDescriptor.Id;
        if (id == kDxgPresentEvent || id == kDxgFlipEvent || id == kDxgBlitEvent) {
            isPresent = true;
            isDxgKrnl = true;
        }
    }

    if (!isPresent) {
        return;
    }

    const double ts = static_cast<double>(pEvent->EventHeader.TimeStamp.QuadPart) / g_etwQpcFreq;

    // The callback runs on a single ProcessTrace thread, so plain statics are safe.
    static DWORD s_lastPid = 0;
    static double s_startTs = 0;
    static int s_dxgiCount = 0;
    static int s_d3d9Count = 0;
    static int s_dxgKrnlCount = 0;

    if (pid != s_lastPid) {
        s_lastPid = pid;
        s_dxgiCount = 0;
        s_d3d9Count = 0;
        s_dxgKrnlCount = 0;
        s_startTs = ts;
        return;
    }

    if (isDxgi) s_dxgiCount++;
    if (isD3D9) s_d3d9Count++;
    if (isDxgKrnl) s_dxgKrnlCount++;

    const double elapsed = ts - s_startTs;
    if (elapsed >= 1.0) {
        // Prioritize explicit API presents (DX9/10/11/12) over kernel level
        // events, which also fire for ordinary desktop apps going through DWM.
        int frames = 0;
        if (s_d3d9Count > 0) {
            frames = s_d3d9Count;
        } else if (s_dxgiCount > 0) {
            frames = s_dxgiCount;
        } else if (s_dxgKrnlCount > 0) {
            // Only kernel events (Vulkan/OpenGL): require a sustained rate so
            // sedentary desktop windows don't read as a game.
            if (s_dxgKrnlCount / elapsed >= 20.0) {
                frames = s_dxgKrnlCount;
            }
        }

        g_gameFps.store(static_cast<int>(frames / elapsed + 0.5), std::memory_order_relaxed);

        s_dxgiCount = 0;
        s_d3d9Count = 0;
        s_dxgKrnlCount = 0;
        s_startTs = ts;
    }
}

struct EtwTracePropsBuf {
    EVENT_TRACE_PROPERTIES p;
    wchar_t name[64];
};

static ULONG EtwStopSession(TRACEHANDLE handle, const wchar_t* name) {
    EtwTracePropsBuf buf;
    ZeroMemory(&buf, sizeof(buf));
    buf.p.Wnode.BufferSize = static_cast<ULONG>(sizeof(buf));
    buf.p.LoggerNameOffset = offsetof(EtwTracePropsBuf, name);
    if (name) {
        wcscpy_s(buf.name, name);
    }
    return ControlTraceW(handle, const_cast<LPWSTR>(name), &buf.p, EVENT_TRACE_CONTROL_STOP);
}

static DWORD WINAPI EtwThreadProc(void* /*param*/) {
    TRACEHANDLE h = g_etwTrace;
    ProcessTrace(&h, 1, nullptr, nullptr);
    return 0;
}

static bool StartEtwFpsSession() {
    if (g_etwReady.load()) {
        return true;
    }

    LARGE_INTEGER freq = {};
    QueryPerformanceFrequency(&freq);
    g_etwQpcFreq = static_cast<double>(freq.QuadPart);

    // Clean up a leftover session from a previous run if there is one.
    EtwStopSession(0, kEtwSessionName);

    EtwTracePropsBuf buf;
    ZeroMemory(&buf, sizeof(buf));
    buf.p.Wnode.BufferSize = static_cast<ULONG>(sizeof(buf));
    buf.p.Wnode.Flags = WNODE_FLAG_TRACED_GUID;
    buf.p.Wnode.ClientContext = 1;                    // QPC timestamps
    buf.p.LogFileMode = EVENT_TRACE_REAL_TIME_MODE;
    buf.p.LoggerNameOffset = offsetof(EtwTracePropsBuf, name);
    wcscpy_s(buf.name, kEtwSessionName);

    ULONG status = StartTraceW(&g_etwSession, kEtwSessionName, &buf.p);
    DiLog(L"Etw: StartTrace=%lu", status);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"Dynamic Island: ETW StartTrace failed (%lu) - game FPS unavailable (admin required).", status);
        g_etwSession = 0;
        return false;
    }

    status = EnableTraceEx2(g_etwSession, &kDxgiProvider, EVENT_CONTROL_CODE_ENABLE_PROVIDER,
                            TRACE_LEVEL_INFORMATION, 0, 0, 0, nullptr);
    DiLog(L"Etw: EnableTraceEx2 DXGI=%lu", status);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"Dynamic Island: DXGI provider enable failed (%lu).", status);
    }

    // D3D9 keeps DirectX 9 games working; DxgKrnl covers Vulkan/OpenGL and is
    // best effort - the session still works for DX10/11/12 if either fails.
    EnableTraceEx2(g_etwSession, &kD3D9Provider, EVENT_CONTROL_CODE_ENABLE_PROVIDER,
                   TRACE_LEVEL_INFORMATION, 0, 0, 0, nullptr);
    status = EnableTraceEx2(g_etwSession, &kDxgKrnlProvider, EVENT_CONTROL_CODE_ENABLE_PROVIDER,
                            TRACE_LEVEL_INFORMATION, 0x1 | 0x8000000, 0, 0, nullptr);
    DiLog(L"Etw: EnableTraceEx2 DxgKrnl=%lu", status);
    if (status != ERROR_SUCCESS) {
        Wh_Log(L"Dynamic Island: DxgKrnl provider enable failed (%lu).", status);
    }

    EVENT_TRACE_LOGFILEW logFile = {};
    logFile.LoggerName = const_cast<LPWSTR>(kEtwSessionName);
    logFile.ProcessTraceMode = PROCESS_TRACE_MODE_REAL_TIME | PROCESS_TRACE_MODE_EVENT_RECORD;
    logFile.EventRecordCallback = EtwFpsCallback;

    g_etwTrace = OpenTraceW(&logFile);
    DiLog(L"Etw: OpenTrace=%I64d", (unsigned long long)g_etwTrace);
    if (g_etwTrace == (TRACEHANDLE)INVALID_HANDLE_VALUE) {
        Wh_Log(L"Dynamic Island: OpenTrace failed - game FPS unavailable.");
        EtwStopSession(g_etwSession, nullptr);
        g_etwSession = 0;
        g_etwTrace = 0;
        return false;
    }

    g_etwRunning.store(true);
    g_etwThread = CreateThread(nullptr, 0, EtwThreadProc, nullptr, 0, nullptr);
    if (!g_etwThread) {
        Wh_Log(L"Dynamic Island: ETW consumer thread failed.");
        g_etwRunning.store(false);
        CloseTrace(g_etwTrace);
        g_etwTrace = 0;
        EtwStopSession(g_etwSession, nullptr);
        g_etwSession = 0;
        return false;
    }

    g_etwReady.store(true);
    Wh_Log(L"Dynamic Island: ETW FPS session started.");
    return true;
}

static void StopEtwFpsSession() {
    if (!g_etwReady.load() && !g_etwRunning.load()) {
        return;
    }

    g_etwRunning.store(false);
    g_gameFps.store(-1, std::memory_order_relaxed);
    g_fpsTargetPid.store(0, std::memory_order_relaxed);

    if (g_etwTrace != 0 && g_etwTrace != (TRACEHANDLE)INVALID_HANDLE_VALUE) {
        CloseTrace(g_etwTrace);
        g_etwTrace = 0;
    }
    if (g_etwThread) {
        WaitForSingleObject(g_etwThread, 3000);  // ProcessTrace returns after CloseTrace
        CloseHandle(g_etwThread);
        g_etwThread = nullptr;
    }

    EtwStopSession(g_etwSession, nullptr);
    g_etwSession = 0;
    g_etwReady.store(false);
}


void UpdateSystemSnapshot() {
    DiLog(L"UpdateSystemSnapshot enter");
    SystemSnapshot next;
    {
        std::lock_guard lock(g_stateMutex);
        next = g_state.system;
        next.charging = g_state.system.charging;
    }

    next.gpuPercent = GetGpuUsage();

    MEMORYSTATUSEX memory = {};
    memory.dwLength = sizeof(memory);
    if (GlobalMemoryStatusEx(&memory)) {
        next.memoryPercent = static_cast<int>(memory.dwMemoryLoad);
    }

    ULARGE_INTEGER freeBytesAvailable = {};
    ULARGE_INTEGER totalBytes = {};
    ULARGE_INTEGER totalFreeBytes = {};
    if (GetDiskFreeSpaceExW(L"C:\\", &freeBytesAvailable, &totalBytes, &totalFreeBytes) &&
        totalBytes.QuadPart > 0) {
        next.diskFreePercent = ClampInt(
            static_cast<int>(totalFreeBytes.QuadPart * 100 / totalBytes.QuadPart), 0, 100);
    }

    HWND foreground = GetForegroundWindow();
    if (foreground && foreground != g_hwnd) {
        wchar_t title[96] = {};
        GetWindowTextW(foreground, title, ARRAYSIZE(title));
        if (!IsIgnorableForegroundWindow(foreground, title)) {
            next.foregroundTitle = title;
            if (next.foregroundTitle.size() > 42) {
                next.foregroundTitle.resize(42);
                next.foregroundTitle += L"...";
            }

        } else {
            next.foregroundTitle.clear();
        }
    }

    FILETIME idle = {};
    FILETIME kernel = {};
    FILETIME user = {};
    if (GetSystemTimes(&idle, &kernel, &user)) {
        const ULONGLONG idleNow = FileTimeToUInt64(idle);
        const ULONGLONG kernelNow = FileTimeToUInt64(kernel);
        const ULONGLONG userNow = FileTimeToUInt64(user);
        const ULONGLONG idlePrev = FileTimeToUInt64(g_prevIdleTime);
        const ULONGLONG kernelPrev = FileTimeToUInt64(g_prevKernelTime);
        const ULONGLONG userPrev = FileTimeToUInt64(g_prevUserTime);

        const ULONGLONG total = (kernelNow - kernelPrev) + (userNow - userPrev);
        const ULONGLONG idleDelta = idleNow - idlePrev;
        if (total > 0 && kernelPrev != 0) {
            next.cpuPercent = ClampInt(static_cast<int>((total - idleDelta) * 100 / total), 0, 100);
        }

        g_prevIdleTime = idle;
        g_prevKernelTime = kernel;
        g_prevUserTime = user;
    }

    static ComPtr<IAudioEndpointVolume> s_volume;
    if (!s_volume) {
        ComPtr<IMMDeviceEnumerator> enumerator;
        ComPtr<IMMDevice> device;
        HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
        if (SUCCEEDED(hr)) {
            hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
        }
        if (SUCCEEDED(hr)) {
            hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(s_volume.GetAddressOf()));
        }
    }

    if (s_volume) {
        float level = 0.0f;
        BOOL muted = FALSE;
        if (SUCCEEDED(s_volume->GetMasterVolumeLevelScalar(&level)) && SUCCEEDED(s_volume->GetMute(&muted))) {
            next.volumePercent = ClampInt(static_cast<int>(level * 100.0f + 0.5f), 0, 100);
            next.volumeMuted = muted != FALSE;
        } else {
            s_volume.Reset(); // Retry next time
        }
    }

    std::lock_guard lock(g_stateMutex);
    const bool volumeChanged =
        g_volumeInitialized &&
        (std::abs(next.volumePercent - g_state.system.volumePercent) >= 2 ||
         next.volumeMuted != g_state.system.volumeMuted);
    g_state.system = next;
    g_state.muted = next.volumeMuted;
    if (volumeChanged) {
        g_state.volume.active = true;
        g_state.volume.percent = next.volumePercent;
        g_state.volume.muted = next.volumeMuted;
        g_state.volume.deviceName = L"System audio";
        g_state.volume.expiresAt = NowSeconds() + 1.8;
        TriggerNudge();
    }
    g_volumeInitialized = true;
}

// ---- Privacy indicator helpers ----
bool IsDeviceActiveViaRegistry(const wchar_t* capability) {
    bool isActive = false;
    std::wstring basePath = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\CapabilityAccessManager\\ConsentStore\\";
    basePath += capability;

    auto CheckSubkeys = [](HKEY hKeyParent) -> bool {
        DWORD index = 0;
        wchar_t subKeyName[256];
        DWORD nameLen = ARRAYSIZE(subKeyName);
        while (RegEnumKeyExW(hKeyParent, index, subKeyName, &nameLen, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
            HKEY hSub;
            if (RegOpenKeyExW(hKeyParent, subKeyName, 0, KEY_READ, &hSub) == ERROR_SUCCESS) {
                if (_wcsicmp(subKeyName, L"NonPackaged") == 0) {
                    DWORD npIndex = 0;
                    wchar_t npSubKeyName[256];
                    DWORD npNameLen = ARRAYSIZE(npSubKeyName);
                    while (RegEnumKeyExW(hSub, npIndex, npSubKeyName, &npNameLen, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS) {
                        HKEY hNpSub;
                        if (RegOpenKeyExW(hSub, npSubKeyName, 0, KEY_READ, &hNpSub) == ERROR_SUCCESS) {
                            uint64_t stopTime = 1;
                            DWORD dataSize = sizeof(stopTime);
                            if (RegQueryValueExW(hNpSub, L"LastUsedTimeStop", nullptr, nullptr, reinterpret_cast<LPBYTE>(&stopTime), &dataSize) == ERROR_SUCCESS) {
                                if (stopTime == 0) {
                                    RegCloseKey(hNpSub);
                                    RegCloseKey(hSub);
                                    return true;
                                }
                            }
                            RegCloseKey(hNpSub);
                        }
                        npIndex++;
                        npNameLen = ARRAYSIZE(npSubKeyName);
                    }
                } else {
                    uint64_t stopTime = 1;
                    DWORD dataSize = sizeof(stopTime);
                    if (RegQueryValueExW(hSub, L"LastUsedTimeStop", nullptr, nullptr, reinterpret_cast<LPBYTE>(&stopTime), &dataSize) == ERROR_SUCCESS) {
                        if (stopTime == 0) {
                            RegCloseKey(hSub);
                            return true;
                        }
                    }
                }
                RegCloseKey(hSub);
            }
            index++;
            nameLen = ARRAYSIZE(subKeyName);
        }
        return false;
    };

    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, basePath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        isActive = CheckSubkeys(hKey);
        RegCloseKey(hKey);
    }
    
    if (!isActive) {
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, basePath.c_str(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            isActive = CheckSubkeys(hKey);
            RegCloseKey(hKey);
        }
    }
    
    return isActive;
}

bool IsMicrophoneActive() {
    return IsDeviceActiveViaRegistry(L"microphone");
}

bool IsCameraActive() {
    return IsDeviceActiveViaRegistry(L"webcam");
}

void UpdateProgressSnapshot() {
    const int progress = Wh_GetIntValue(L"ProgressPercent", -1);
    std::lock_guard lock(g_stateMutex);
    g_state.progress.active = progress >= 0 && progress <= 100;
    g_state.progress.percent = ClampInt(progress, 0, 100);
}

void UpdatePrivacyIndicators() {
    const bool mic = IsMicrophoneActive();
    const bool cam = IsCameraActive();
    std::lock_guard lock(g_stateMutex);
    g_state.system.micActive = mic;
    g_state.system.cameraActive = cam;
}

std::wstring ReadClipboardText(HWND hwnd) {
    std::wstring text;
    if (!OpenClipboard(hwnd)) {
        return text;
    }

    HANDLE data = GetClipboardData(CF_UNICODETEXT);
    if (data) {
        auto* locked = static_cast<const wchar_t*>(GlobalLock(data));
        if (locked) {
            text = locked;
            GlobalUnlock(data);
        }
    }

    CloseClipboard();
    return text;
}

bool ClipboardHasBitmap(HWND hwnd) {
    bool result = false;
    if (OpenClipboard(hwnd)) {
        result = IsClipboardFormatAvailable(CF_BITMAP) ||
                 IsClipboardFormatAvailable(CF_DIB) ||
                 IsClipboardFormatAvailable(CF_DIBV5);
        CloseClipboard();
    }
    return result;
}

bool IsLikelyToastWindow(HWND hwnd, const wchar_t* className, const wchar_t* title) {
    if (hwnd == g_hwnd || !hwnd) {
        return false;
    }

    if (GetWindow(hwnd, GW_OWNER)) {
        return false;
    }

    const std::wstring cls = ToLowerCopy(className ? className : L"");
    const std::wstring text = ToLowerCopy(title ? title : L"");

    // Classic Windows 10 toasts have clear class names
    if (cls.find(L"notification") != std::wstring::npos ||
        cls.find(L"toast") != std::wstring::npos ||
        cls.find(L"windows.ui.notifications") != std::wstring::npos) {
        return true;
    }

    // Windows 11 toasts use generic XAML or CoreWindow classes, usually hosted by
    // explorer.exe, sihost.exe, or ShellExperienceHost.exe.
    // Importantly, their title is often empty at the exact moment of creation!
    if (cls.find(L"xaml_windowedpopupclass") != std::wstring::npos ||
        cls.find(L"windows.ui.core.corewindow") != std::wstring::npos) {
        
        std::wstring image;
        if (ProcessImageNameForWindow(hwnd, &image)) {
            const std::wstring base = ToLowerCopy(BaseNameFromPath(image));
            if (base == L"explorer.exe" || base == L"sihost.exe" || base == L"shellexperiencehost.exe") {
                // Ensure it's not the start menu, search, or action center main panel
                if (text != L"start" && text != L"action center" && text != L"search" && text != L"task view") {
                    return true;
                }
            }
        }
    }

    return false;
}

void CaptureShellNotification(HWND hwnd) {
    // Grace period: ignore notifications that fire in the first 3 seconds after
    // the mod starts. sihost.exe gets injected while system windows are still
    // settling, which causes false positives (e.g. Snipping Tool windows).
    if (NowSeconds() < 3.0) {
        return;
    }

    wchar_t className[128] = {};
    wchar_t title[192] = {};
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    GetWindowTextW(hwnd, title, ARRAYSIZE(title));

    if (!IsLikelyToastWindow(hwnd, className, title)) {
        return;
    }

    NotificationSnapshot notification;
    notification.active = true;
    notification.app = L"Notification";
    notification.title = title;
    notification.expiresAt = NowSeconds() + 4.0;
    // Fetch a 64px icon to ensure crisp rendering inside the pill
    notification.icon = GetWindowIconPixels(hwnd, 64);

    if (notification.title.size() > 96) {
        notification.body = notification.title.substr(64);
        notification.title.resize(64);
        notification.title += L"...";
    }

    {
        std::lock_guard lock(g_stateMutex);
        g_state.notification = std::move(notification);
    }
    TriggerNudge();

    // Spawn a background thread to extract the full rich text body of the toast using UI Automation.
    // Modern Windows Toasts often only provide the App Name via GetWindowTextW, leaving the body hidden in the XAML tree.
    std::thread([hwnd]() {
        Sleep(400); // Give the heavy UWP XAML tree enough time to fully construct the text nodes
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (SUCCEEDED(hr)) {
            IUIAutomation* uia = nullptr;
            hr = CoCreateInstance(__uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void**)&uia);
            if (SUCCEEDED(hr) && uia) {
                IUIAutomation2* uia2 = nullptr;
                if (SUCCEEDED(uia->QueryInterface(__uuidof(IUIAutomation2), (void**)&uia2)) && uia2) {
                    uia2->put_TransactionTimeout(500);
                    uia2->put_ConnectionTimeout(500);
                    uia2->Release();
                }
                IUIAutomationElement* windowEl = nullptr;
                if (SUCCEEDED(uia->ElementFromHandle(hwnd, &windowEl)) && windowEl) {
                    IUIAutomationCondition* cond = nullptr;
                    uia->CreateTrueCondition(&cond);
                    IUIAutomationElementArray* elements = nullptr;
                    if (SUCCEEDED(windowEl->FindAll(TreeScope_Descendants, cond, &elements)) && elements) {
                        int count = 0;
                        elements->get_Length(&count);
                        std::wstring appName;
                        std::wstring fullText;
                        for (int i = 0; i < count; ++i) {
                            IUIAutomationElement* el = nullptr;
                            if (SUCCEEDED(elements->GetElement(i, &el)) && el) {
                                BSTR name = nullptr;
                                el->get_CurrentName(&name);
                                if (name && wcslen(name) > 0) {
                                    std::wstring chunk = name;
                                    // Skip generic screen-reader labels often found in toasts
                                    if (chunk != L"Notification" && chunk != L"New notification") {
                                        if (appName.empty()) {
                                            appName = chunk;
                                        } else {
                                            if (!fullText.empty()) fullText += L"  -  ";
                                            fullText += chunk;
                                        }
                                    }
                                }
                                if (name) SysFreeString(name);
                                el->Release();
                            }
                        }
                        elements->Release();
                        
                        if (fullText.empty() && !appName.empty()) {
                            fullText = appName;
                            appName = L"Notification";
                        }
                        
                        if (!fullText.empty()) {
                            std::lock_guard lock(g_stateMutex);
                            if (g_state.notification.active) {
                                if (!appName.empty()) {
                                    g_state.notification.app = appName;
                                    if (appName != L"Notification") {
                                        BitmapPixels resolvedIcon = FindAppIconByName(appName, 64);
                                        if (!resolvedIcon.bgra.empty()) {
                                            g_state.notification.icon = std::move(resolvedIcon);
                                        }
                                    }
                                }
                                g_state.notification.title = fullText;
                            }
                        }
                    }
                    if (cond) cond->Release();
                    windowEl->Release();
                }
                uia->Release();
            }
            CoUninitialize();
        }
    }).detach();
}

void CaptureClipboard(HWND hwnd) {
    ClipboardSnapshot clip;
    clip.expiresAt = NowSeconds() + 2.5;
    HWND owner = GetClipboardOwner();
    if (!owner) {
        owner = GetForegroundWindow();
    }
    wchar_t ownerTitle[80] = {};
    if (owner) {
        GetWindowTextW(owner, ownerTitle, ARRAYSIZE(ownerTitle));
    }
    if (owner && !IsIgnorableForegroundWindow(owner, ownerTitle)) {
        DWORD pid = 0;
        GetWindowThreadProcessId(owner, &pid);
        // Fetch at 64px for crisp rendering ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÂ¢Ã¢â€šÂ¬Ã‚Â 18px/32px is often too small for icon APIs and returns empty.
        clip.appIcon = GetWindowIconPixels(owner, 64);

        clip.appName = ownerTitle;
        if (clip.appName.empty()) {
            std::wstring path;
            if (ProcessImageNameForPid(pid, &path)) {
                clip.appName = StripExtension(BaseNameFromPath(path));
            }
        }
        if (clip.appName.size() > 24) {
            clip.appName.resize(24);
            clip.appName += L"...";
        }
    }

    std::wstring text = ReadClipboardText(hwnd);
    if (!text.empty()) {
        constexpr size_t maxChars = 96;
        std::replace(text.begin(), text.end(), L'\r', L' ');
        std::replace(text.begin(), text.end(), L'\n', L' ');
        if (text.size() > maxChars) {
            text.resize(maxChars);
            text += L"...";
        }
        clip.text = text;
        clip.image = false;
        clip.active = true;
    } else if (ClipboardHasBitmap(hwnd)) {
        clip.text = L"Image copied";
        clip.image = true;
        clip.active = true;
    }

    if (clip.active) {
        {
            std::lock_guard lock(g_stateMutex);
            g_state.clipboard = std::move(clip);
        }
        TriggerNudge();
    }
}

void SetClickThrough(HWND hwnd, bool clickThrough) {
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    const bool has = (exStyle & WS_EX_TRANSPARENT) != 0;
    if (clickThrough == has) {
        return;
    }

    if (clickThrough) {
        exStyle |= WS_EX_TRANSPARENT;
    } else {
        exStyle &= ~WS_EX_TRANSPARENT;
    }

    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);
}

void SendVirtualKey(WORD vk) {
    INPUT inputs[2] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = vk;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = vk;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, inputs, sizeof(INPUT));
}

void OpenRelevantApp();

void SendMediaCommandAtPoint(HWND hwnd, LPARAM lParam) {
    RECT rc = {};
    GetClientRect(hwnd, &rc);
    const int x = GET_X_LPARAM(lParam);
    const int y = GET_Y_LPARAM(lParam);

    // Media controls are drawn in the right side of the media pill. Hit-test
    // using client coordinates so this stays independent of monitor position.
    // Hit-test positions must match DrawMedia control layout:
    // cx = rect.right - 42, prev = cx-26, play = cx, next = cx+26
    // rect.right = rc.right - kRenderPadX, so offset from rc.right:
    const int centerY = (rc.bottom + rc.top) / 2;
    const int prevX = rc.right - static_cast<int>(kRenderPadX) - 68;
    const int playX = rc.right - static_cast<int>(kRenderPadX) - 42;
    const int nextX = rc.right - static_cast<int>(kRenderPadX) - 16;
    const int radius = 15;

    auto hit = [&](int cx) {
        const int dx = x - cx;
        const int dy = y - centerY;
        return dx * dx + dy * dy <= radius * radius;
    };

    if (hit(prevX)) {
        SendVirtualKey(VK_MEDIA_PREV_TRACK);
    } else if (hit(playX)) {
        SendVirtualKey(VK_MEDIA_PLAY_PAUSE);
    } else if (hit(nextX)) {
        SendVirtualKey(VK_MEDIA_NEXT_TRACK);
    } else {
        OpenRelevantApp();
    }
}

void ToggleEndpointMute();

void HandleStatusClickAtPoint(HWND hwnd, LPARAM lParam) {
    // Disabled click handlers for status chips as requested by the user
    return;
}

void ToggleEndpointMute() {
    ComPtr<IMMDeviceEnumerator> enumerator;
    ComPtr<IMMDevice> device;
    ComPtr<IAudioEndpointVolume> volume;

    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                  CLSCTX_ALL, IID_PPV_ARGS(&enumerator));
    if (SUCCEEDED(hr)) {
        hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
    }
    if (SUCCEEDED(hr)) {
        hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr,
                              reinterpret_cast<void**>(volume.GetAddressOf()));
    }
    if (SUCCEEDED(hr)) {
        BOOL muted = FALSE;
        volume->GetMute(&muted);
        volume->SetMute(!muted, nullptr);
        std::lock_guard lock(g_stateMutex);
        g_state.muted = !muted;
    }
}

struct WindowSearch {
    std::wstring targetTitle;
    HWND foundHwnd = nullptr;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    if (!IsWindowVisible(hwnd)) {
        return TRUE;
    }

    wchar_t title[512];
    if (GetWindowTextW(hwnd, title, ARRAYSIZE(title)) > 0) {
        auto* search = reinterpret_cast<WindowSearch*>(lParam);
        std::wstring wTitle(title);
        // Case-insensitive check if window title contains currently playing media title
        auto it = std::search(
            wTitle.begin(), wTitle.end(),
            search->targetTitle.begin(), search->targetTitle.end(),
            [](wchar_t ch1, wchar_t ch2) { return towlower(ch1) == towlower(ch2); }
        );

        if (it != wTitle.end()) {
            search->foundHwnd = hwnd;
            return FALSE; // found, stop enumerating
        }
    }
    return TRUE;
}

void OpenRelevantApp() {
    std::wstring title;
    std::wstring app;
    {
        std::lock_guard lock(g_stateMutex);
        title = g_state.media.title;
        app = g_state.media.sourceAppUserModelId;
    }

    // Try to find and focus window containing track title (ideal for browser playing YouTube/Spotify)
    if (!title.empty()) {
        WindowSearch search;
        search.targetTitle = title;
        EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&search));

        if (search.foundHwnd) {
            if (IsIconic(search.foundHwnd)) {
                ShowWindow(search.foundHwnd, SW_RESTORE);
            }
            SetForegroundWindow(search.foundHwnd);
            return;
        }
    }

    // Fallback: Launch or focus via AppUserModelId or Path
    if (!app.empty()) {
        std::wstring executePath = app;
        
        // Remove surrounding quotes if any
        if (executePath.size() >= 2 && executePath.front() == L'"' && executePath.back() == L'"') {
            executePath = executePath.substr(1, executePath.size() - 2);
        }

        bool isFilePath = (executePath.find(L":\\") != std::wstring::npos || 
                           (executePath.size() >= 4 && executePath.substr(executePath.size() - 4) == L".exe"));

        if (isFilePath) {
            if (GetFileAttributesW(executePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
                // Path doesn't exist. Try 64-bit Program Files if it was in x86
                size_t x86Pos = executePath.find(L" (x86)");
                if (x86Pos != std::wstring::npos) {
                    std::wstring altPath = executePath;
                    altPath.erase(x86Pos, 6);
                    if (GetFileAttributesW(altPath.c_str()) != INVALID_FILE_ATTRIBUTES) {
                        executePath = altPath;
                    }
                }
                
                // If it STILL doesn't exist after trying alternatives, just gracefully abort!
                // Trying to guess 'brave.exe' triggers broken Windows Registry App Paths.
                if (GetFileAttributesW(executePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
                    return;
                }
            }
            
            SHELLEXECUTEINFOW sei = { sizeof(sei) };
            sei.fMask = SEE_MASK_FLAG_NO_UI;
            sei.lpFile = executePath.c_str();
            sei.nShow = SW_SHOWNORMAL;
            ShellExecuteExW(&sei);
        } else {
            // It's a UWP/Desktop AppUserModelId, launch via AppsFolder
            std::wstring shellPath = L"shell:AppsFolder\\" + executePath;
            SHELLEXECUTEINFOW sei = { sizeof(sei) };
            sei.fMask = SEE_MASK_FLAG_NO_UI;
            sei.lpFile = shellPath.c_str();
            sei.nShow = SW_SHOWNORMAL;
            ShellExecuteExW(&sei);
        }
        return;
    }

    ShellExecuteW(nullptr, L"open", L"ms-settings:", nullptr, nullptr, SW_SHOWNORMAL);
}

void DismissTransientState() {
    std::lock_guard lock(g_stateMutex);
    g_state.clipboard.active = false;
    g_state.notification.active = false;
    g_state.volume.active = false;
    g_state.progress.active = false;
    Wh_SetIntValue(L"ProgressPercent", -1);
}

int GameOverlayEnabledItemCount() {
    int count = 0;
    if (g_settings.gameFpsViewer) count++;
    if (g_settings.gameOverlayTemp) count++;
    if (g_settings.gameOverlayCpu) count++;
    if (g_settings.gameOverlayRam) count++;
    if (g_settings.gameOverlayGpu) count++;
    if (g_settings.gameOverlayDisk) count++;
    return count;
}

// Hit-testing rects for the game overlay chips, recorded in DrawGameOverlay in
// coordinates relative to the overlay rect center (matches the media click math).
// Chip ids: 0=FPS, 1=Temp, 2=CPU, 3=RAM, 4=GPU, 5=Disk.
struct GameOverlayChipHit {
    int id;
    float left, top, right, bottom;
};
GameOverlayChipHit g_gameOverlayChips[6];
int g_gameOverlayChipCount = 0;

float GameOverlayContentWidth() {
    int panels = (g_settings.gameFpsViewer ? 1 : 0) + (g_settings.gameOverlayTemp ? 1 : 0);
    int cards = (g_settings.gameOverlayCpu ? 1 : 0) + (g_settings.gameOverlayRam ? 1 : 0) +
                (g_settings.gameOverlayGpu ? 1 : 0) + (g_settings.gameOverlayDisk ? 1 : 0);
    if (panels == 0 && cards == 0) {
        return 100.0f;
    }
    float width = 20.0f;
    if (panels > 0) {
        width += panels * 74.0f;
        width += (cards > 0 ? panels : panels - 1) * 8.0f;
    }
    if (cards > 0) {
        width += cards * 62.0f + (cards - 1) * 6.0f;
    }
    return width;
}

void ToggleGameOverlayChip(int id) {
    bool currentlyOn = false;
    switch (id) {
        case 0: currentlyOn = g_settings.gameFpsViewer; break;
        case 1: currentlyOn = g_settings.gameOverlayTemp; break;
        case 2: currentlyOn = g_settings.gameOverlayCpu; break;
        case 3: currentlyOn = g_settings.gameOverlayRam; break;
        case 4: currentlyOn = g_settings.gameOverlayGpu; break;
        case 5: currentlyOn = g_settings.gameOverlayDisk; break;
        default: return;
    }
    if (currentlyOn && GameOverlayEnabledItemCount() <= 1) {
        return;  // keep at least one chip enabled
    }
    switch (id) {
        case 0:
            Wh_SetIntValue(L"FpsVisible", g_settings.gameFpsViewer ? 0 : 1);
            break;
        case 1:
            Wh_SetIntValue(L"OverlayTemp", g_settings.gameOverlayTemp ? 0 : 1);
            break;
        case 2:
            Wh_SetIntValue(L"OverlayCpu", g_settings.gameOverlayCpu ? 0 : 1);
            break;
        case 3:
            Wh_SetIntValue(L"OverlayRam", g_settings.gameOverlayRam ? 0 : 1);
            break;
        case 4:
            Wh_SetIntValue(L"OverlayGpu", g_settings.gameOverlayGpu ? 0 : 1);
            break;
        case 5:
            Wh_SetIntValue(L"OverlayDisk", g_settings.gameOverlayDisk ? 0 : 1);
            break;
        default:
            return;
    }
    LoadSettings();
    g_layoutDirty = true;
    if (g_settingsChangedEvent) {
        SetEvent(g_settingsChangedEvent);
    }
}

void ShowContextMenu(HWND hwnd, POINT screenPoint) {
    HMENU menu = CreatePopupMenu();
    AppendMenuW(menu, MF_STRING, 1, L"Dismiss");
    AppendMenuW(menu, MF_STRING, 2, L"Pin expanded");
    AppendMenuW(menu, MF_STRING, 3, Wh_GetIntValue(L"GameOverlayPinned", 0) ? L"Hide game overlay" : L"Show game overlay");
    const int activeW11 = Wh_GetIntValue(L"W11StyleOverride", -1) >= 0
                          ? Wh_GetIntValue(L"W11StyleOverride", 0)
                          : Wh_GetIntSetting(L"Appearance.W11Style");
    AppendMenuW(menu, MF_STRING, 10, activeW11 ? L"Use iPhone Pill Style" : L"Use Windows 11 Flyout Style");
    const int activeExpandOnHover = Wh_GetIntValue(L"ExpandOnHoverOverride", -1) >= 0
                          ? Wh_GetIntValue(L"ExpandOnHoverOverride", 0)
                          : Wh_GetIntSetting(L"Appearance.ExpandOnHover");
    AppendMenuW(menu, MF_STRING, 11, activeExpandOnHover ? L"Expand on Click" : L"Expand on Hover");
    // Quick settings panel: position, hide/show island, FPS counter.
    HMENU posMenu = CreatePopupMenu();
    AppendMenuW(posMenu, MF_STRING, 40, L"Left");
    AppendMenuW(posMenu, MF_STRING, 41, L"Center");
    AppendMenuW(posMenu, MF_STRING, 42, L"Right");
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(posMenu), L"Island position");
    const bool islandHidden = Wh_GetIntValue(L"IslandHidden", 0) != 0;
    AppendMenuW(menu, MF_STRING, 13, islandHidden ? L"Show island" : L"Hide island");
    const int fpsVisible = Wh_GetIntValue(L"FpsVisible", -1);
    const bool fpsOn = fpsVisible >= 0 ? (fpsVisible != 0) : g_settings.gameFpsViewer;
    AppendMenuW(menu, MF_STRING, 14, fpsOn ? L"Hide FPS counter" : L"Show FPS counter");
    HMENU overlayMenu = CreatePopupMenu();
    AppendMenuW(overlayMenu, MF_STRING | (fpsOn ? MF_CHECKED : 0), 50, L"FPS counter");
    AppendMenuW(overlayMenu, MF_STRING | (g_settings.gameOverlayTemp ? MF_CHECKED : 0), 51, L"GPU temperature");
    AppendMenuW(overlayMenu, MF_STRING | (g_settings.gameOverlayCpu ? MF_CHECKED : 0), 52, L"CPU load");
    AppendMenuW(overlayMenu, MF_STRING | (g_settings.gameOverlayRam ? MF_CHECKED : 0), 53, L"RAM load");
    AppendMenuW(overlayMenu, MF_STRING | (g_settings.gameOverlayGpu ? MF_CHECKED : 0), 54, L"GPU load");
    AppendMenuW(overlayMenu, MF_STRING | (g_settings.gameOverlayDisk ? MF_CHECKED : 0), 55, L"Disk load");
    AppendMenuW(overlayMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(overlayMenu, MF_STRING, 56, L"Restore all");
    AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(overlayMenu), L"Game overlay content");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, 4, L"Transparency 100%");
    AppendMenuW(menu, MF_STRING, 5, L"Transparency 85%");
    AppendMenuW(menu, MF_STRING, 6, L"Transparency 70%");
    AppendMenuW(menu, MF_STRING, 7, L"Transparency 55%");
    AppendMenuW(menu, MF_STRING, 8, L"Reset transparency");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    // Color theme presets
    AppendMenuW(menu, MF_STRING, 20, L"Theme: OLED Black (default)");
    AppendMenuW(menu, MF_STRING, 21, L"Theme: Dark Gray");
    AppendMenuW(menu, MF_STRING, 22, L"Theme: Midnight Blue");
    AppendMenuW(menu, MF_STRING, 23, L"Theme: Deep Purple");
    AppendMenuW(menu, MF_STRING, 24, L"Theme: Fluent Design");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, 9, L"Open Windhawk settings");

    SetForegroundWindow(hwnd);
    const UINT cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
                                   screenPoint.x, screenPoint.y, 0, hwnd, nullptr);
    DestroyMenu(menu);

    switch (cmd) {
        case 1:
            DismissTransientState();
            break;
        case 2:
            Wh_SetIntValue(L"PinnedExpanded", Wh_GetIntValue(L"PinnedExpanded", 0) ? 0 : 1);
            break;
        case 3:
            Wh_SetIntValue(L"GameOverlayPinned", Wh_GetIntValue(L"GameOverlayPinned", 0) ? 0 : 1);
            break;
        case 4:
            Wh_SetIntValue(L"PillOpacityOverride", 100);
            LoadSettings();
            break;
        case 5:
            Wh_SetIntValue(L"PillOpacityOverride", 85);
            LoadSettings();
            break;
        case 6:
            Wh_SetIntValue(L"PillOpacityOverride", 70);
            LoadSettings();
            break;
        case 7:
            Wh_SetIntValue(L"PillOpacityOverride", 55);
            LoadSettings();
            break;
        case 8:
            Wh_SetIntValue(L"PillOpacityOverride", -1);
            LoadSettings();
            break;
        case 9: {
            wchar_t currentProcessPath[MAX_PATH] = {};
            GetModuleFileNameW(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath));

            HINSTANCE result = ShellExecuteW(nullptr, L"open",
                                             currentProcessPath,
                                             nullptr,
                                             nullptr, SW_SHOWNORMAL);
            if (reinterpret_cast<INT_PTR>(result) <= 32) {
                Wh_Log(L"Failed to open Windhawk settings.");
            }
            break;
        }
        case 10: {
            const int activeW11Val = Wh_GetIntValue(L"W11StyleOverride", -1) >= 0
                                  ? Wh_GetIntValue(L"W11StyleOverride", 0)
                                  : Wh_GetIntSetting(L"Appearance.W11Style");
            Wh_SetIntValue(L"W11StyleOverride", activeW11Val ? 0 : 1);
            LoadSettings();
            g_layoutDirty = true;
            break;
        }
        case 11: {
            const int activeExpandOnHover = Wh_GetIntValue(L"ExpandOnHoverOverride", -1) >= 0
                                  ? Wh_GetIntValue(L"ExpandOnHoverOverride", 0)
                                  : Wh_GetIntSetting(L"Appearance.ExpandOnHover");
            Wh_SetIntValue(L"ExpandOnHoverOverride", activeExpandOnHover ? 0 : 1);
            LoadSettings();
            g_layoutDirty = true;
            break;
        }
        case 13:
            Wh_SetIntValue(L"IslandHidden", Wh_GetIntValue(L"IslandHidden", 0) ? 0 : 1);
            LoadSettings();
            break;
        case 14:
            Wh_SetIntValue(L"FpsVisible",
                           (Wh_GetIntValue(L"FpsVisible", -1) >= 0
                                ? (Wh_GetIntValue(L"FpsVisible", 0) != 0)
                                : g_settings.gameFpsViewer)
                               ? 0
                               : 1);
            LoadSettings();
            break;
        case 50:
            if (GameOverlayEnabledItemCount() <= 1) break;
            Wh_SetIntValue(L"FpsVisible", g_settings.gameFpsViewer ? 0 : 1);
            LoadSettings();
            g_layoutDirty = true;
            break;
        case 51:
            if (GameOverlayEnabledItemCount() <= 1) break;
            Wh_SetIntValue(L"OverlayTemp", g_settings.gameOverlayTemp ? 0 : 1);
            LoadSettings();
            g_layoutDirty = true;
            break;
        case 52:
            if (GameOverlayEnabledItemCount() <= 1) break;
            Wh_SetIntValue(L"OverlayCpu", g_settings.gameOverlayCpu ? 0 : 1);
            LoadSettings();
            g_layoutDirty = true;
            break;
        case 53:
            if (GameOverlayEnabledItemCount() <= 1) break;
            Wh_SetIntValue(L"OverlayRam", g_settings.gameOverlayRam ? 0 : 1);
            LoadSettings();
            g_layoutDirty = true;
            break;
        case 54:
            if (GameOverlayEnabledItemCount() <= 1) break;
            Wh_SetIntValue(L"OverlayGpu", g_settings.gameOverlayGpu ? 0 : 1);
            LoadSettings();
            g_layoutDirty = true;
            break;
        case 55:
            if (GameOverlayEnabledItemCount() <= 1) break;
            Wh_SetIntValue(L"OverlayDisk", g_settings.gameOverlayDisk ? 0 : 1);
            LoadSettings();
            g_layoutDirty = true;
            break;
        case 56:
            Wh_SetIntValue(L"FpsVisible", -1);
            Wh_SetIntValue(L"OverlayTemp", -1);
            Wh_SetIntValue(L"OverlayCpu", -1);
            Wh_SetIntValue(L"OverlayRam", -1);
            Wh_SetIntValue(L"OverlayGpu", -1);
            Wh_SetIntValue(L"OverlayDisk", -1);
            LoadSettings();
            g_layoutDirty = true;
            break;
        case 40:
            Wh_SetIntValue(L"PositionOverride", 0);
            LoadSettings();
            break;
        case 41:
            Wh_SetIntValue(L"PositionOverride", 1);
            LoadSettings();
            break;
        case 42:
            Wh_SetIntValue(L"PositionOverride", 2);
            LoadSettings();
            break;
        // Color theme presets ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÂ¢Ã¢â€šÂ¬Ã‚Â stored as integer index in local mod storage.
        case 20:  // OLED Black
            Wh_SetIntValue(L"ColorTheme", 0);
            LoadSettings();
            break;
        case 21:  // Dark Gray
            Wh_SetIntValue(L"ColorTheme", 1);
            LoadSettings();
            break;
        case 22:  // Midnight Blue
            Wh_SetIntValue(L"ColorTheme", 2);
            LoadSettings();
            break;
        case 23:  // Deep Purple
            Wh_SetIntValue(L"ColorTheme", 3);
            LoadSettings();
            break;
        case 24:  // Fluent Design
            Wh_SetIntValue(L"ColorTheme", 4);
            LoadSettings();
            break;
    }
}

class Renderer {
   public:
    bool Initialize(HWND hwnd) {
        hwnd_ = hwnd;

        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                       __uuidof(ID2D1Factory),
                                       reinterpret_cast<void**>(d2dFactory_.GetAddressOf()));
        if (FAILED(hr)) {
            return false;
        }

        hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                 reinterpret_cast<IUnknown**>(dwriteFactory_.GetAddressOf()));
        if (FAILED(hr)) {
            return false;
        }

        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            0.0f, 0.0f,
            D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE);

        hr = d2dFactory_->CreateDCRenderTarget(&props, &target_);
        if (FAILED(hr)) {
            return false;
        }

        // Apple SF Pro-like: use Segoe UI Variable Display for clean modern look.
        dwriteFactory_->CreateTextFormat(L"Segoe UI Variable Display", nullptr,
                                         DWRITE_FONT_WEIGHT_SEMI_BOLD,
                                         DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL,
                                         13.5f, L"", &textFormat_);
        dwriteFactory_->CreateTextFormat(L"Segoe UI Variable Small", nullptr,
                                         DWRITE_FONT_WEIGHT_NORMAL,
                                         DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL,
                                         11.0f, L"", &smallTextFormat_);
        dwriteFactory_->CreateTextFormat(L"Segoe UI Variable Display", nullptr,
                                         DWRITE_FONT_WEIGHT_BOLD,
                                         DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL,
                                         18.0f, L"", &clockFormat_);
        dwriteFactory_->CreateTextFormat(L"Segoe Fluent Icons", nullptr,
                                         DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL,
                                         16.0f, L"", &iconFormat_);

        if (textFormat_) {
            textFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }
        if (smallTextFormat_) {
            smallTextFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }
        if (clockFormat_) {
            clockFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            clockFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            clockFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (iconFormat_) {
            iconFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }

        return CreateBackingBitmap(520, 140);
    }

    bool Render(const SharedState& state, const Settings& settings, const Activity& primary,
                const std::optional<Activity>& secondary, float width, float height,
                float nudge, bool hover, bool pinned, double now) {
        DiLog(L"Render enter kind=%d w=%.0f h=%.0f", static_cast<int>(primary.kind), width, height);
        EnsureTextFormats(settings.sizeScale);
        const int pixelWidth = std::max(1, static_cast<int>(std::ceil(width + kRenderPadX * 2.0f)));
        const int pixelHeight = std::max(1, static_cast<int>(std::ceil(height + kRenderPadY * 2.0f)));

        if (pixelWidth != bitmapWidth_ || pixelHeight != bitmapHeight_) {
            if (!CreateBackingBitmap(pixelWidth, pixelHeight)) {
                return false;
            }
            PositionOverlayWindow(hwnd_, pixelWidth, pixelHeight);
        } else if (g_layoutDirty.exchange(false)) {
            PositionOverlayWindow(hwnd_, pixelWidth, pixelHeight);
        }

        RECT rc = {0, 0, bitmapWidth_, bitmapHeight_};
        HRESULT hr = target_->BindDC(memDc_, &rc);
        if (FAILED(hr)) {
            return false;
        }

        target_->BeginDraw();
        target_->Clear(D2D1::ColorF(0, 0.0f));

        EnsureBrushes(settings, state);
        settingsOpacity_ = settings.pillOpacity;

        const float hoverScale = hover || pinned ? 1.025f : 1.0f;
        const float scale = hoverScale;

        const float top = kRenderPadY + nudge;
        const float left = kRenderPadX;
        
        if (width >= 2.0f && height >= 2.0f) {
            if (secondary) {
                const float gap = 12.0f * settings.sizeScale;
                const float maxH = std::max(primary.height, secondary->height);
                const float pTop = top + (maxH - primary.height) * 0.5f;
                const float sTop = top + (maxH - secondary->height) * 0.5f;

                DrawPill(state, settings, primary,
                         D2D1::RectF(left, pTop, left + primary.width, pTop + primary.height),
                         scale, now);
                DrawPill(state, settings, *secondary,
                         D2D1::RectF(left + primary.width + gap, sTop,
                                      left + primary.width + gap + secondary->width,
                                      sTop + secondary->height),
                         scale, now);
            } else {
                DrawPill(state, settings, primary,
                         D2D1::RectF(left, top, left + width, top + height), scale, now);
            }
        }

        hr = target_->EndDraw();
        if (FAILED(hr)) {
            return false;
        }

        POINT src = {0, 0};
        SIZE size = {bitmapWidth_, bitmapHeight_};
        POINT dst = {};
        RECT winRect = {};
        GetWindowRect(hwnd_, &winRect);
        dst.x = winRect.left;
        dst.y = winRect.top;

        BLENDFUNCTION blend = {};
        blend.BlendOp = AC_SRC_OVER;
        blend.SourceConstantAlpha = static_cast<BYTE>(Clamp(settings.pillOpacity, 0.35f, 1.0f) * 255.0f);
        blend.AlphaFormat = AC_SRC_ALPHA;

        return UpdateLayeredWindow(hwnd_, nullptr, &dst, &size, memDc_, &src, 0, &blend,
                                   ULW_ALPHA) != FALSE;
    }

    void Shutdown() {
        artBitmap_.Reset();
        notificationIconBitmap_.Reset();
        mediaSourceIconBitmap_.Reset();
        clipboardIconBitmap_.Reset();
        accentBrush_.Reset();
        redBrush_.Reset();
        textBrush_.Reset();
        mutedBrush_.Reset();
        tintBrush_.Reset();
        shadowBrush_.Reset();
        target_.Reset();
        textFormat_.Reset();
        smallTextFormat_.Reset();
        boldTextFormat_.Reset();
        hugeTextFormat_.Reset();
        clockFormat_.Reset();
        iconFormat_.Reset();
        dwriteFactory_.Reset();
        d2dFactory_.Reset();

        if (oldBitmap_) {
            SelectObject(memDc_, oldBitmap_);
            oldBitmap_ = nullptr;
        }
        if (dib_) {
            DeleteObject(dib_);
            dib_ = nullptr;
        }
        if (memDc_) {
            DeleteDC(memDc_);
            memDc_ = nullptr;
        }
    }

   private:
    bool CreateBackingBitmap(int width, int height) {
        if (oldBitmap_) {
            SelectObject(memDc_, oldBitmap_);
            oldBitmap_ = nullptr;
        }
        if (dib_) {
            DeleteObject(dib_);
            dib_ = nullptr;
        }
        if (!memDc_) {
            HDC screen = GetDC(nullptr);
            memDc_ = CreateCompatibleDC(screen);
            ReleaseDC(nullptr, screen);
            if (!memDc_) {
                return false;
            }
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
        if (!dib_) {
            return false;
        }

        oldBitmap_ = static_cast<HBITMAP>(SelectObject(memDc_, dib_));
        bitmapWidth_ = width;
        bitmapHeight_ = height;
        return true;
    }

    float lastFontScale_ = 0.0f;
    void EnsureTextFormats(float scale) {
        if (std::abs(scale - lastFontScale_) < 0.001f) {
            return;
        }

        textFormat_ = nullptr;
        smallTextFormat_ = nullptr;
        clockFormat_ = nullptr;
        iconFormat_ = nullptr;

        dwriteFactory_->CreateTextFormat(L"Segoe UI Variable Display", nullptr,
                                         DWRITE_FONT_WEIGHT_SEMI_BOLD,
                                         DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL,
                                         13.5f, L"", &textFormat_);
        dwriteFactory_->CreateTextFormat(L"Segoe UI Variable Small", nullptr,
                                         DWRITE_FONT_WEIGHT_NORMAL,
                                         DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL,
                                         11.0f, L"", &smallTextFormat_);
        dwriteFactory_->CreateTextFormat(L"Segoe UI Variable Display", nullptr,
                                         DWRITE_FONT_WEIGHT_BOLD,
                                         DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL,
                                         18.0f, L"", &clockFormat_);
        dwriteFactory_->CreateTextFormat(L"Segoe UI Variable Display", nullptr,
                                         DWRITE_FONT_WEIGHT_BOLD,
                                         DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL,
                                         12.0f, L"", &boldTextFormat_);
        dwriteFactory_->CreateTextFormat(L"Segoe UI Variable Display", nullptr,
                                         DWRITE_FONT_WEIGHT_BOLD,
                                         DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL,
                                         42.0f, L"", &hugeTextFormat_);
        dwriteFactory_->CreateTextFormat(L"Segoe Fluent Icons", nullptr,
                                         DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                                         DWRITE_FONT_STRETCH_NORMAL,
                                         16.0f, L"", &iconFormat_);

        if (textFormat_) {
            textFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }
        if (smallTextFormat_) {
            smallTextFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }
        if (boldTextFormat_) {
            boldTextFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            boldTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        }
        if (hugeTextFormat_) {
            hugeTextFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            hugeTextFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        }
        if (clockFormat_) {
            clockFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            clockFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            clockFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (iconFormat_) {
            iconFormat_->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        }

        lastFontScale_ = scale;
    }

    void EnsureBrushes(const Settings& settings, const SharedState& state) {
        D2D1_COLOR_F accent = settings.customAccent;
        if (settings.accentMode == AccentMode::System) {
            accent = GetSystemAccentColor();
        } else if (settings.accentMode == AccentMode::Auto && !state.media.art.bgra.empty()) {
            accent = state.media.art.sampledAccent;
        }

        if (!accentBrush_) target_->CreateSolidColorBrush(accent, &accentBrush_);
        else accentBrush_->SetColor(accent);

        if (!redBrush_) target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.27f, 0.27f, 1.0f), &redBrush_);

        // Use user-configured text colors.
        D2D1_COLOR_F primary = settings.textPrimaryColor;
        primary.a = 0.96f;
        if (!textBrush_) target_->CreateSolidColorBrush(primary, &textBrush_);
        else textBrush_->SetColor(primary);

        D2D1_COLOR_F secondary = settings.textSecondaryColor;
        secondary.a = 0.72f;
        if (!mutedBrush_) target_->CreateSolidColorBrush(secondary, &mutedBrush_);
        else mutedBrush_->SetColor(secondary);

        // Cache pill bg color with opacity applied.
        pillBgColor_ = settings.pillBgColor;
        pillBgColor_.a = 1.0f;
        
        D2D1_COLOR_F tintColor = D2D1::ColorF(0.010f, 0.010f, 0.012f, settings.tintOpacity);
        if (!tintBrush_) target_->CreateSolidColorBrush(tintColor, &tintBrush_);
        else tintBrush_->SetColor(tintColor);

        if (!shadowBrush_) target_->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.70f), &shadowBrush_);
    }

    void DrawPill(const SharedState& state, const Settings& settings, const Activity& activity,
                  D2D1_RECT_F rect, float scale, double now) {
        const float cx = (rect.left + rect.right) * 0.5f;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float w = (rect.right - rect.left) * scale;
        const float h = (rect.bottom - rect.top) * scale;
        rect = D2D1::RectF(cx - w * 0.5f, cy - h * 0.5f, cx + w * 0.5f, cy + h * 0.5f);

        float radius = settings.w11Style ? 8.0f * settings.sizeScale : (rect.bottom - rect.top) * 0.5f;
        if (!settings.w11Style) {
            radius = std::min(radius, 44.0f * settings.sizeScale);
        }
        DrawSoftShadow(rect, radius);

        D2D1_ROUNDED_RECT pill = D2D1::RoundedRect(rect, radius, radius);
        DrawPillSurface(rect, radius, activity.kind, settings.w11Style);

        if (activity.kind == IslandKind::Progress) {
            DrawProgressRing(rect, state.progress.percent);
        }

        if (activity.kind == IslandKind::BatteryLow) {
            const float pulse = 0.5f + 0.5f * std::sin(static_cast<float>(now * 2.0 * 3.14159265 * 2.1));
            redBrush_->SetOpacity(0.45f + 0.45f * pulse);
            target_->DrawRoundedRectangle(pill, redBrush_.Get(), 2.0f);
            redBrush_->SetOpacity(1.0f);
        } else {
            accentBrush_->SetOpacity(activity.kind == IslandKind::Idle ? 0.18f : 0.34f);
            target_->DrawRoundedRectangle(pill, accentBrush_.Get(), 1.0f);
            accentBrush_->SetOpacity(1.0f);
        }

        if (!settings.w11Style) {
            ComPtr<ID2D1SolidColorBrush> highlight;
            target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.10f * settingsOpacity_), &highlight);
            target_->DrawRoundedRectangle(
                D2D1::RoundedRect(D2D1::RectF(rect.left + 1, rect.top + 1, rect.right - 1,
                                              rect.bottom - 1),
                                  radius - 1, radius - 1),
                highlight.Get(), 1.0f);
        }

        D2D1_MATRIX_3X2_F oldTransform;
        target_->GetTransform(&oldTransform);
        D2D1_POINT_2F pillCenter = D2D1::Point2F((rect.left + rect.right) * 0.5f, (rect.top + rect.bottom) * 0.5f);
        target_->SetTransform(D2D1::Matrix3x2F::Scale(settings.sizeScale, settings.sizeScale, pillCenter) * oldTransform);

        float invScale = 1.0f / settings.sizeScale;
        float unW = (rect.right - rect.left) * invScale;
        float unH = (rect.bottom - rect.top) * invScale;
        D2D1_RECT_F unscaledRect = D2D1::RectF(pillCenter.x - unW * 0.5f, pillCenter.y - unH * 0.5f, pillCenter.x + unW * 0.5f, pillCenter.y + unH * 0.5f);

        switch (activity.kind) {
            case IslandKind::Media:
                DrawMedia(state, unscaledRect, now);
                break;
            case IslandKind::Clipboard:
                DrawClipboard(state, unscaledRect);
                break;
            case IslandKind::Notification:
                DrawNotification(state, unscaledRect);
                break;
            case IslandKind::Volume:
                DrawVolume(state, unscaledRect);
                break;
            case IslandKind::CapsLock:
                DrawCapsLock(state, unscaledRect);
                break;
            case IslandKind::Device:
                DrawDevice(state, unscaledRect);
                break;
            case IslandKind::BatteryLow:
                DrawBattery(state, unscaledRect);
                break;
            case IslandKind::Progress:
                DrawProgress(state, unscaledRect);
                break;
            case IslandKind::Idle:
            default:
                DrawIdleDashboard(state, unscaledRect, settings, now);
                break;
        }

        // ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ Apple-style privacy indicator dots ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â€šÂ¬Ã‚ÂÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬
        // Green dot = camera in use, Orange dot = mic in use.
        // Drawn in top-right corner of pill, outside content area.
        DrawPrivacyDots(state, unscaledRect, now);

        target_->SetTransform(oldTransform);
    }

    void DrawSoftShadow(D2D1_RECT_F rect, float radius) {
        UNREFERENCED_PARAMETER(rect);
        UNREFERENCED_PARAMETER(radius);
    }

    // Apple Dynamic Island privacy dots.
    // Placed inside the pill near the top-right edge ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÂ¢Ã¢â€šÂ¬Ã‚Â pulsing glow like iPhone.
    void DrawPrivacyDots(const SharedState& state, D2D1_RECT_F rect, double now) {
        const bool mic = state.system.micActive;
        const bool cam = state.system.cameraActive;
        if (!mic && !cam) return;

        // Soft breathing pulse (0.75 Hz like iPhone).
        const float pulse = 0.72f + 0.28f * std::sin(static_cast<float>(now * 2.0 * 3.14159265 * 0.75));

        const float dotR   = 4.5f;   // dot radius
        const float gap    = 5.5f;   // gap between dots
        const float margin = 10.0f;  // from right edge
        const float dotY   = rect.top + (rect.bottom - rect.top) * 0.5f;

        float x = rect.right - margin - dotR;

        // Green = camera (rightmost when both active).
        if (cam) {
            ComPtr<ID2D1SolidColorBrush> camBrush;
            // Bright iOS camera green.
            target_->CreateSolidColorBrush(
                D2D1::ColorF(0.133f, 0.776f, 0.239f, pulse * settingsOpacity_), &camBrush);
            if (camBrush) {
                target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, dotY), dotR, dotR),
                                     camBrush.Get());
                // Glow halo.
                ComPtr<ID2D1SolidColorBrush> glow;
                target_->CreateSolidColorBrush(
                    D2D1::ColorF(0.133f, 0.776f, 0.239f, 0.18f * pulse * settingsOpacity_), &glow);
                if (glow) {
                    target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, dotY),
                                                        dotR * 2.2f, dotR * 2.2f), glow.Get());
                }
            }
            x -= (dotR * 2.0f + gap);
        }

        // Orange = microphone.
        if (mic) {
            ComPtr<ID2D1SolidColorBrush> micBrush;
            // iOS orange.
            target_->CreateSolidColorBrush(
                D2D1::ColorF(1.0f, 0.584f, 0.0f, pulse * settingsOpacity_), &micBrush);
            if (micBrush) {
                target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, dotY), dotR, dotR),
                                     micBrush.Get());
                ComPtr<ID2D1SolidColorBrush> glow;
                target_->CreateSolidColorBrush(
                    D2D1::ColorF(1.0f, 0.584f, 0.0f, 0.18f * pulse * settingsOpacity_), &glow);
                if (glow) {
                    target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, dotY),
                                                        dotR * 2.2f, dotR * 2.2f), glow.Get());
                }
            }
        }
    }

    void DrawPillSurface(D2D1_RECT_F rect, float radius, IslandKind kind, bool w11Style) {
        UNREFERENCED_PARAMETER(kind);

        if (tintBrush_) {
            target_->FillRoundedRectangle(D2D1::RoundedRect(rect, radius, radius),
                                          tintBrush_.Get());
        }

        // User-defined pill background color.
        ComPtr<ID2D1SolidColorBrush> blackBrush;
        D2D1_COLOR_F bg = pillBgColor_;
        bg.a = settingsOpacity_;
        target_->CreateSolidColorBrush(bg, &blackBrush);
        if (blackBrush) {
            target_->FillRoundedRectangle(D2D1::RoundedRect(rect, radius, radius),
                                          blackBrush.Get());
        }

        if (w11Style) {
            // In Windows 11 style, we draw a double-layer modern border.
            // Windows 11 uses a very subtle 1px border. In dark mode, it's white with 15% opacity.
            ComPtr<ID2D1SolidColorBrush> border;
            target_->CreateSolidColorBrush(
                D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.15f * settingsOpacity_), &border);
            if (border) {
                target_->DrawRoundedRectangle(
                    D2D1::RoundedRect(D2D1::RectF(rect.left + 0.5f, rect.top + 0.5f,
                                                  rect.right - 0.5f, rect.bottom - 0.5f),
                                      radius, radius),
                    border.Get(), 1.0f);
            }
        } else {
            // Thin top-edge gloss: simulates iPhone notch glass shine.
            ComPtr<ID2D1SolidColorBrush> gloss;
            target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.055f * settingsOpacity_),
                                           &gloss);
            if (gloss) {
                D2D1_RECT_F glossLine = D2D1::RectF(rect.left + radius, rect.top + 0.5f,
                                                    rect.right - radius, rect.top + 1.2f);
                target_->FillRectangle(glossLine, gloss.Get());
            }

            // Outer border: extremely subtle white rim like iPhone Dynamic Island edge.
            ComPtr<ID2D1SolidColorBrush> border;
            target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.08f * settingsOpacity_),
                                           &border);
            if (border) {
                target_->DrawRoundedRectangle(D2D1::RoundedRect(
                    D2D1::RectF(rect.left + 0.5f, rect.top + 0.5f,
                                rect.right - 0.5f, rect.bottom - 0.5f),
                    radius, radius), border.Get(), 0.8f);
            }
        }
    }

    void DrawAccentGlow(D2D1_RECT_F rect, const Activity& activity, double now) {
        float opacity = activity.kind == IslandKind::Media ? 0.23f : 0.12f;
        if (activity.kind == IslandKind::BatteryLow) {
            redBrush_->SetOpacity(0.18f);
            target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(rect.right - 38, rect.top + 20), 56, 36),
                                 redBrush_.Get());
            redBrush_->SetOpacity(1.0f);
            return;
        }

        opacity += 0.05f * (0.5f + 0.5f * std::sin(static_cast<float>(now * 1.7)));
        accentBrush_->SetOpacity(opacity);
        target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(rect.left + 48, rect.top + 10), 70, 42),
                             accentBrush_.Get());
        target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(rect.right - 58, rect.bottom - 8), 76, 42),
                             accentBrush_.Get());
        accentBrush_->SetOpacity(1.0f);
    }

    static int GetDaysInMonth(int year, int month) {
        if (month == 2) {
            bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            return leap ? 29 : 28;
        }
        if (month == 4 || month == 6 || month == 9 || month == 11) return 30;
        return 31;
    }

    static int GetDayOfWeek(int year, int month, int day) {
        if (month < 3) { month += 12; year -= 1; }
        int k = year % 100;
        int j = year / 100;
        int h = (day + 13 * (month + 1) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;
        return (h + 6) % 7;
    }



    void DrawCalendarDashboard(const SharedState& state, D2D1_RECT_F rect, const Settings& settings, double now, float scale, SYSTEMTIME& local) {
        ComPtr<ID2D1SolidColorBrush> calBg;
        target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.04f * settingsOpacity_), &calBg);
        D2D1_RECT_F leftBlock = D2D1::RectF(rect.left + 22.0f * scale, rect.top + 16.0f * scale,
                                            rect.left + 115.0f * scale, rect.bottom - 22.0f * scale);
        target_->FillRoundedRectangle(D2D1::RoundedRect(leftBlock, 12.0f * scale, 12.0f * scale), calBg.Get());
        
        ComPtr<ID2D1SolidColorBrush> calHeader;
        target_->CreateSolidColorBrush(D2D1::ColorF(0.85f, 0.25f, 0.20f, 0.9f * settingsOpacity_), &calHeader);
        
        wchar_t monthName[32] = {};
        GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &local, L"MMMM", monthName, ARRAYSIZE(monthName), nullptr);
        for (int i = 0; monthName[i]; ++i) monthName[i] = towupper(monthName[i]);
        
        target_->DrawTextW(monthName, static_cast<UINT32>(wcslen(monthName)), boldTextFormat_.Get(),
                           D2D1::RectF(leftBlock.left, leftBlock.top + 6.0f * scale, leftBlock.right, leftBlock.top + 24.0f * scale),
                           calHeader.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);

        wchar_t yearStr[16] = {};
        swprintf_s(yearStr, L"%d", local.wYear);
        mutedBrush_->SetOpacity(0.45f);
        target_->DrawTextW(yearStr, static_cast<UINT32>(wcslen(yearStr)), boldTextFormat_.Get(),
                           D2D1::RectF(leftBlock.left, leftBlock.top + 20.0f * scale, leftBlock.right, leftBlock.top + 38.0f * scale),
                           mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);

        wchar_t dayStr[16] = {};
        swprintf_s(dayStr, L"%d", local.wDay);
        textBrush_->SetOpacity(0.96f);
        target_->DrawTextW(dayStr, static_cast<UINT32>(wcslen(dayStr)), hugeTextFormat_.Get(),
                           D2D1::RectF(leftBlock.left, leftBlock.top + 30.0f * scale, leftBlock.right, leftBlock.top + 80.0f * scale),
                           textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);

        wchar_t weekdayName[32] = {};
        GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &local, L"dddd", weekdayName, ARRAYSIZE(weekdayName), nullptr);
        mutedBrush_->SetOpacity(0.75f);
        target_->DrawTextW(weekdayName, static_cast<UINT32>(wcslen(weekdayName)), boldTextFormat_.Get(),
                           D2D1::RectF(leftBlock.left, leftBlock.bottom - 22.0f * scale, leftBlock.right, leftBlock.bottom),
                           mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);

        // Right Grid
        const float gridStart = rect.left + 144.0f * scale;
        const float gridTop = rect.top + 30.0f * scale;
        const float colW = 31.0f * scale;
        const float rowH = 18.0f * scale;
        const wchar_t* days[] = {L"S", L"M", L"T", L"W", L"T", L"F", L"S"};
        
        for (int i = 0; i < 7; ++i) {
            D2D1_RECT_F cell = D2D1::RectF(gridStart + i * colW, gridTop, gridStart + (i+1)*colW, gridTop + rowH);
            ComPtr<ID2D1SolidColorBrush> brush = (i == 0 || i == 6) ? calHeader : mutedBrush_;
            target_->DrawTextW(days[i], 1, boldTextFormat_.Get(), cell, brush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
        }

        int startDay = GetDayOfWeek(local.wYear, local.wMonth, 1);
        int totalDays = GetDaysInMonth(local.wYear, local.wMonth);
        
        int row = 1;
        int col = startDay;
        textBrush_->SetOpacity(0.85f);
        for (int d = 1; d <= totalDays; ++d) {
            D2D1_RECT_F cell = D2D1::RectF(gridStart + col * colW, gridTop + row * rowH + 4.0f * scale, 
                                           gridStart + (col+1)*colW, gridTop + (row+1)*rowH + 4.0f * scale);
            
            if (d == local.wDay) {
                target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cell.left + colW*0.5f, cell.top + rowH*0.5f), 10.0f*scale, 10.0f*scale), calHeader.Get());
                target_->DrawTextW(std::to_wstring(d).c_str(), static_cast<UINT32>(std::to_wstring(d).length()), boldTextFormat_.Get(), cell, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            } else {
                ComPtr<ID2D1SolidColorBrush> dBrush = (col == 0 || col == 6) ? calHeader : textBrush_;
                if (col == 0 || col == 6) dBrush->SetOpacity(0.6f);
                target_->DrawTextW(std::to_wstring(d).c_str(), static_cast<UINT32>(std::to_wstring(d).length()), boldTextFormat_.Get(), cell, dBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            }
            
            col++;
            if (col > 6) { col = 0; row++; }
        }
    }

void DrawRAMCleanerDashboard(const SharedState& state, D2D1_RECT_F rect, float scale) {
        MEMORYSTATUSEX mem = {};
        mem.dwLength = sizeof(mem);
        GlobalMemoryStatusEx(&mem);

        const UINT64 totalMb = mem.ullTotalPhys / (1024 * 1024);
        const UINT64 usedMb = (mem.ullTotalPhys - mem.ullAvailPhys) / (1024 * 1024);
        const float usagePct = totalMb > 0 ? static_cast<float>(usedMb) / static_cast<float>(totalMb) * 100.0f : 0.0f;

        // Title
        textBrush_->SetOpacity(0.96f);
        target_->DrawTextW(L"RAM Cleaner", static_cast<UINT32>(wcslen(L"RAM Cleaner")), boldTextFormat_.Get(),
                           D2D1::RectF(rect.left + 35.0f * scale, rect.top + 30.0f * scale, rect.left + 185.0f * scale, rect.bottom),
                           textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);

        wchar_t wUsed[64] = {};
        swprintf_s(wUsed, L"%llu / %llu MB", usedMb, totalMb);
        mutedBrush_->SetOpacity(0.85f);
        target_->DrawTextW(wUsed, static_cast<UINT32>(wcslen(wUsed)), textFormat_.Get(),
                           D2D1::RectF(rect.left + 35.0f * scale, rect.top + 55.0f * scale, rect.left + 185.0f * scale, rect.bottom),
                           mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);

        // Usage bar
        const float barW = 185.0f * scale;
        const float barH = 8.0f * scale;
        D2D1_RECT_F barBg = D2D1::RectF(rect.left + 35.0f * scale, rect.top + 78.0f * scale,
                                        rect.left + 35.0f * scale + barW, rect.top + 78.0f * scale + barH);
        ComPtr<ID2D1SolidColorBrush> barBrush;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.10f * settingsOpacity_), &barBrush);
        target_->FillRoundedRectangle(D2D1::RoundedRect(barBg, barH / 2, barH / 2), barBrush.Get());

        float fillW = barW * std::clamp(usagePct / 100.0f, 0.0f, 1.0f);
        if (fillW > 1.0f) {
            D2D1_RECT_F barFill = D2D1::RectF(barBg.left, barBg.top, barBg.left + fillW, barBg.bottom);
            accentBrush_->SetOpacity(0.9f);
            target_->FillRoundedRectangle(D2D1::RoundedRect(barFill, barH / 2, barH / 2), accentBrush_.Get());
            accentBrush_->SetOpacity(1.0f);
        }

        // Freed memory feedback
        DWORDLONG freedMb = g_ramFreedBytes.load() / (1024 * 1024);
        wchar_t wFreed[64] = {};
        if (g_ramCleaning.load()) {
            wcscpy_s(wFreed, L"Cleaning RAM...");
        } else if (freedMb > 0 && (NowSeconds() - g_ramCleanedAt.load()) < 12.0) {
            swprintf_s(wFreed, L"Freed %llu MB", freedMb);
        } else if (freedMb > 0) {
            swprintf_s(wFreed, L"Last cleaned: %llu MB", freedMb);
        } else {
            wcscpy_s(wFreed, L"Press Clean to free memory");
        }
        mutedBrush_->SetOpacity(0.70f);
        target_->DrawTextW(wFreed, static_cast<UINT32>(wcslen(wFreed)), textFormat_.Get(),
                           D2D1::RectF(rect.left + 35.0f * scale, rect.top + 100.0f * scale, rect.left + 185.0f * scale, rect.bottom),
                           mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);

        // Clean button
        D2D1_RECT_F btn = D2D1::RectF(rect.right - 118.0f * scale, rect.top + 40.0f * scale, rect.right - 30.0f * scale, rect.bottom - 40.0f * scale);
        ComPtr<ID2D1SolidColorBrush> btnBrush;
        if (g_ramCleaning.load()) {
            target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.10f * settingsOpacity_), &btnBrush);
        } else {
            target_->CreateSolidColorBrush(D2D1::ColorF(0.16f, 0.63f, 0.96f, 0.85f * settingsOpacity_), &btnBrush);
        }
        target_->FillRoundedRectangle(D2D1::RoundedRect(btn, 8.0f * scale, 8.0f * scale), btnBrush.Get());

        textBrush_->SetOpacity(0.96f);
        const wchar_t* btnLabel = g_ramCleaning.load() ? L"..." : L"Clean";
        target_->DrawTextW(btnLabel, static_cast<UINT32>(wcslen(btnLabel)), boldTextFormat_.Get(), btn, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
    }

    void DrawConfigDashboard(const SharedState& state, D2D1_RECT_F rect, const Settings& settings) {
        DiLog(L"DrawConfigDashboard enter");
        const float scale = 1.0f;
        const float w = rect.right - rect.left;
        const float h = rect.bottom - rect.top;
        DiLog(L"DrawConfig rect dims: %f x %f (left=%f top=%f)", w, h, rect.left, rect.top);
        const float inset = 30.0f * scale, gap = 10.0f * scale;
        const float y0 = 54.0f * scale, y1 = 112.0f * scale, bh = 42.0f * scale;
        const float bw = (w - 2.0f * inset - 2.0f * gap) / 3.0f;
        const float bw2 = (w - 2.0f * inset - gap) / 2.0f;

        D2D1_RECT_F titleRect = D2D1::RectF(rect.left + 26.0f * scale, rect.top + 12.0f * scale,
                                            rect.right - 26.0f * scale, rect.top + 36.0f * scale);
        target_->DrawTextW(L"Configuration", 13, textFormat_.Get(), titleRect, textBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_NONE);

        ComPtr<ID2D1SolidColorBrush> baseBrush, activeBrush;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.10f * settingsOpacity_), &baseBrush);
        target_->CreateSolidColorBrush(D2D1::ColorF(0.0f, 1.0f, 0.65f, 0.45f), &activeBrush);

        int curPos = 1;
        if (settings.position == Position::TopLeft) curPos = 0;
        else if (settings.position == Position::TopRight) curPos = 2;
        const wchar_t* posLabels[3] = { L"\u2190 Left", L"Center", L"Right \u2192" };
        for (int i = 0; i < 3; i++) {
            float l = rect.left + inset + i * (bw + gap);
            D2D1_RECT_F b = D2D1::RectF(l, rect.top + y0, l + bw, rect.top + y0 + bh);
            target_->FillRoundedRectangle(D2D1::RoundedRect(b, 12.0f * scale, 12.0f * scale),
                                          (i == curPos ? activeBrush.Get() : baseBrush.Get()));
            D2D1_RECT_F tr = D2D1::RectF(l, rect.top + y0, l + bw, rect.top + y0 + bh);
            target_->DrawTextW(posLabels[i], static_cast<UINT32>(wcslen(posLabels[i])),
                               smallTextFormat_.Get(), tr, textBrush_.Get(),
                               D2D1_DRAW_TEXT_OPTIONS_NONE);
        }

        const wchar_t* toggleLabels[2];
        int toggleActive[2] = { 0, 0 };
        if (settings.gameFpsViewer) {
            toggleLabels[0] = L"FPS: On";
            toggleActive[0] = 1;
        } else {
            toggleLabels[0] = L"FPS: Off";
        }
        if (Wh_GetIntValue(L"IslandHidden", 0) != 0) {
            toggleLabels[1] = L"Island: Hidden";
        } else {
            toggleLabels[1] = L"Island: Visible";
            toggleActive[1] = 1;
        }
        for (int i = 0; i < 2; i++) {
            float l = rect.left + inset + i * (bw2 + gap);
            D2D1_RECT_F b = D2D1::RectF(l, rect.top + y1, l + bw2, rect.top + y1 + bh);
            target_->FillRoundedRectangle(D2D1::RoundedRect(b, 12.0f * scale, 12.0f * scale),
                                          (toggleActive[i] ? activeBrush.Get() : baseBrush.Get()));
            D2D1_RECT_F tr = D2D1::RectF(l, rect.top + y1, l + bw2, rect.top + y1 + bh);
            target_->DrawTextW(toggleLabels[i], static_cast<UINT32>(wcslen(toggleLabels[i])),
                               smallTextFormat_.Get(), tr, textBrush_.Get(),
                               D2D1_DRAW_TEXT_OPTIONS_NONE);
        }
    }

    void DrawIdleDashboard(const SharedState& state, D2D1_RECT_F rect, const Settings& settings,
                           double now) {
        target_->PushAxisAlignedClip(rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        
        bool privacyActive = state.system.micActive || state.system.cameraActive;
        if (!clockFormat_) return;

        SYSTEMTIME local = {};
        GetLocalTime(&local);
        wchar_t timeBuf[32] = {};
        GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &local, nullptr, timeBuf, ARRAYSIZE(timeBuf));

        const float scale = 1.0f;
        const float width = rect.right - rect.left;
        const bool gameOn = settings.gameOverlay || Wh_GetIntValue(L"GameOverlayPinned", 0) != 0;
        const bool mediaOn = settings.media && state.media.available;
        const int panel = CurrentPanelIndex(gameOn, mediaOn);
        const bool gameTabActive = gameOn && panel == GamePanelIndex();

        if (width / scale < 220.0f && !gameTabActive) {
            // Collapsed Mode
            D2D1_RECT_F timeRect = D2D1::RectF(rect.left + 20.0f * scale, rect.top + 7.0f * scale,
                                               rect.left + 80.0f * scale, rect.bottom - 7.0f * scale);
            textBrush_->SetOpacity(0.96f);
            target_->DrawTextW(timeBuf, static_cast<UINT32>(wcslen(timeBuf)), smallTextFormat_.Get(),
                               timeRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            textBrush_->SetOpacity(1.0f);
            target_->PopAxisAlignedClip();
            return;
        }

        // Expanded Mode - unified panel routing
        if (gameOn && panel == GamePanelIndex()) {
            target_->PopAxisAlignedClip();
            DrawGameOverlay(state, rect, 1.0f);
            return;
        }
        if (gameOn && panel == GameOptionsPanelIndex()) {
            target_->PopAxisAlignedClip();
            DrawGameOverlayOptions(state, rect);
            return;
        }
        if (mediaOn && panel == MediaPanelIndex(gameOn, mediaOn)) {
            target_->PopAxisAlignedClip();
            DrawMedia(state, rect, now);
            return;
        }
        if (panel == RamPanelIndex(gameOn, mediaOn)) {
            target_->PopAxisAlignedClip();
            DrawRAMCleanerDashboard(state, rect, scale);
            return;
        }
        if (panel == CalendarPanelIndex(gameOn, mediaOn)) {
            target_->PopAxisAlignedClip();
            DrawCalendarDashboard(state, rect, settings, now, scale, local);
            return;
        }
        // Config panel
        target_->PopAxisAlignedClip();
        DrawConfigDashboard(state, rect, settings);
        return;
    }

    void DrawGameOverlayOptions(const SharedState& state, D2D1_RECT_F rect) {
        DiLog(L"DrawGameOverlayOptions enter");
        const float scale = 1.0f;
        const float w = rect.right - rect.left;
        const float inset = 30.0f * scale, gap = 10.0f * scale;
        const float bh = 42.0f * scale;
        // 3 columns x 2 rows so the panel fits within the 184px expanded island.
        const float bw3 = (w - 2.0f * inset - 2.0f * gap) / 3.0f;

        D2D1_RECT_F titleRect = D2D1::RectF(rect.left + 26.0f * scale, rect.top + 12.0f * scale,
                                            rect.right - 26.0f * scale, rect.top + 36.0f * scale);
        target_->DrawTextW(L"Game Overlay", 12, textFormat_.Get(), titleRect, textBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_NONE);

        ComPtr<ID2D1SolidColorBrush> baseBrush, activeBrush;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.10f * settingsOpacity_), &baseBrush);
        target_->CreateSolidColorBrush(D2D1::ColorF(0.0f, 1.0f, 0.65f, 0.45f), &activeBrush);

        const bool items[6] = {
            g_settings.gameFpsViewer,
            g_settings.gameOverlayTemp,
            g_settings.gameOverlayCpu,
            g_settings.gameOverlayRam,
            g_settings.gameOverlayGpu,
            g_settings.gameOverlayDisk,
        };
        const wchar_t* labels[6] = {
            L"FPS counter",
            L"GPU temp",
            L"CPU load",
            L"RAM load",
            L"GPU load",
            L"Disk load",
        };
        // 3 columns x 2 rows so the panel fits within the 184px expanded island.
        for (int i = 0; i < 6; i++) {
            int row = i / 3;
            int col = i % 3;
            float l = rect.left + inset + col * (bw3 + gap);
            float t = rect.top + 54.0f * scale + row * (bh + gap);
            D2D1_RECT_F b = D2D1::RectF(l, t, l + bw3, t + bh);
            target_->FillRoundedRectangle(D2D1::RoundedRect(b, 12.0f * scale, 12.0f * scale),
                                          (items[i] ? activeBrush.Get() : baseBrush.Get()));
            D2D1_RECT_F tr = D2D1::RectF(l, t, l + bw3, t + bh);
            target_->DrawTextW(labels[i], static_cast<UINT32>(wcslen(labels[i])),
                               smallTextFormat_.Get(), tr, textBrush_.Get(),
                               D2D1_DRAW_TEXT_OPTIONS_NONE);
        }
    }

    void DrawGameOverlay(const SharedState& state, D2D1_RECT_F rect, float unused_scale) {
        DiLog(L"DrawGameOverlay enter fps=%d valid=%d", state.system.gameFps, state.system.gameFpsValid ? 1 : 0);
        const float scale = 1.0f;
        ComPtr<ID2D1SolidColorBrush> panelBrush;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.080f * settingsOpacity_), &panelBrush);

        const float ox = (rect.left + rect.right) * 0.5f;
        const float oy = (rect.top + rect.bottom) * 0.5f;
        g_gameOverlayChipCount = 0;
        auto AddChip = [&](int id, D2D1_RECT_F r) {
            if (g_gameOverlayChipCount < 6) {
                g_gameOverlayChips[g_gameOverlayChipCount++] = {id, r.left - ox, r.top - oy, r.right - ox, r.bottom - oy};
            }
        };

        float cursor = rect.left + 10.0f * scale;

        if (g_settings.gameFpsViewer) {
            D2D1_RECT_F fpsPanel = D2D1::RectF(cursor, rect.top + 10.0f * scale,
                                               cursor + 74.0f * scale, rect.bottom - 10.0f * scale);
            target_->FillRoundedRectangle(D2D1::RoundedRect(fpsPanel, 15.0f * scale, 15.0f * scale), panelBrush.Get());
            AddChip(0, fpsPanel);

            ComPtr<ID2D1SolidColorBrush> fpsIconBrush;
            target_->CreateSolidColorBrush(D2D1::ColorF(0.0f, 1.0f, 0.65f, 1.0f), &fpsIconBrush);
            D2D1_POINT_2F fpsCenter = D2D1::Point2F(fpsPanel.left + 18.0f * scale, fpsPanel.top + 17.0f * scale);
            DrawGameIcon(fpsCenter, 7.0f * scale, 0, fpsIconBrush.Get(), scale);

            if (g_settings.showMetricText) {
                mutedBrush_->SetOpacity(0.44f);
                target_->DrawTextW(L"FPS", 3, smallTextFormat_.Get(),
                                   D2D1::RectF(fpsPanel.left + 31.0f * scale, fpsPanel.top + 6.0f * scale,
                                               fpsPanel.right - 10.0f * scale, fpsPanel.top + 24.0f * scale),
                                   mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            }

            wchar_t fpsValue[16] = {};
            if (state.system.gameFpsValid && state.system.gameFps > 0) {
                swprintf_s(fpsValue, L"%d", state.system.gameFps);
            } else {
                wcscpy_s(fpsValue, ARRAYSIZE(fpsValue), L"--");
            }
            textBrush_->SetOpacity(0.96f);
            D2D1_RECT_F fpsValueRect = g_settings.showMetricText
                ? D2D1::RectF(fpsPanel.left + 16.0f * scale, fpsPanel.top + 23.0f * scale, fpsPanel.right - 10.0f * scale, fpsPanel.bottom - 4.0f * scale)
                : D2D1::RectF(fpsPanel.left + 31.0f * scale, fpsPanel.top + 10.0f * scale, fpsPanel.right - 10.0f * scale, fpsPanel.bottom - 4.0f * scale);
            target_->DrawTextW(fpsValue, static_cast<UINT32>(wcslen(fpsValue)), textFormat_.Get(),
                               fpsValueRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);

            cursor += 82.0f * scale;
        }

        D2D1_RECT_F tempPanel = D2D1::RectF(cursor, rect.top + 10.0f * scale,
                                            cursor + 74.0f * scale, rect.bottom - 10.0f * scale);
        if (g_settings.gameOverlayTemp) {
            target_->FillRoundedRectangle(D2D1::RoundedRect(tempPanel, 15.0f * scale, 15.0f * scale), panelBrush.Get());
            AddChip(1, tempPanel);

        ComPtr<ID2D1SolidColorBrush> tempIconBrush;
        target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.55f, 0.15f, 1.0f), &tempIconBrush);
        D2D1_POINT_2F tempCenter = D2D1::Point2F(tempPanel.left + 18.0f * scale, tempPanel.top + 17.0f * scale);
        DrawGameIcon(tempCenter, 7.0f * scale, 0, tempIconBrush.Get(), scale);

        if (g_settings.showMetricText) {
            mutedBrush_->SetOpacity(0.44f);
            target_->DrawTextW(L"GPU", 3, smallTextFormat_.Get(),
                               D2D1::RectF(tempPanel.left + 31.0f * scale, tempPanel.top + 6.0f * scale,
                                           tempPanel.right - 10.0f * scale, tempPanel.top + 24.0f * scale),
                               mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
        }

        wchar_t tempValue[32] = {};
        const int gpuTemp = g_gpuTempCelsius.load();
        if (gpuTemp < 0) {
            wcscpy_s(tempValue, ARRAYSIZE(tempValue), L"--");
        } else if (g_settings.gpuTempFahrenheit) {
            swprintf_s(tempValue, L"%d%cF", gpuTemp * 9 / 5 + 32, 0x00B0);
        } else {
            swprintf_s(tempValue, L"%d%cC", gpuTemp, 0x00B0);
        }
        textBrush_->SetOpacity(0.96f);
        D2D1_RECT_F tempValueRect = g_settings.showMetricText
            ? D2D1::RectF(tempPanel.left + 16.0f * scale, tempPanel.top + 23.0f * scale, tempPanel.right - 10.0f * scale, tempPanel.bottom - 4.0f * scale)
            : D2D1::RectF(tempPanel.left + 31.0f * scale, tempPanel.top + 10.0f * scale, tempPanel.right - 10.0f * scale, tempPanel.bottom - 4.0f * scale);
        target_->DrawTextW(tempValue, static_cast<UINT32>(wcslen(tempValue)), textFormat_.Get(),
                           tempValueRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            cursor = tempPanel.right + 8.0f * scale;
        }

        const float cardTop = rect.top + 10.0f * scale;
        const float cardW = 62.0f * scale;
        const float gap = 6.0f * scale;
        float start = cursor;
        if (g_settings.gameOverlayCpu) {
            D2D1_RECT_F cpuCard = D2D1::RectF(start, cardTop, start + cardW, rect.bottom - 10.0f * scale);
            DrawGameMetricCard(cpuCard, L"CPU", state.system.cpuPercent, 1, scale);
            AddChip(2, cpuCard);
            start += cardW + gap;
        }
        if (g_settings.gameOverlayRam) {
            D2D1_RECT_F ramCard = D2D1::RectF(start, cardTop, start + cardW, rect.bottom - 10.0f * scale);
            DrawGameMetricCard(ramCard, L"RAM", state.system.memoryPercent, 2, scale);
            AddChip(3, ramCard);
            start += cardW + gap;
        }
        if (g_settings.gameOverlayGpu) {
            D2D1_RECT_F gpuCard = D2D1::RectF(start, cardTop, start + cardW, rect.bottom - 10.0f * scale);
            DrawGameMetricCard(gpuCard, L"GPU", state.system.gpuPercent, 3, scale);
            AddChip(4, gpuCard);
            start += cardW + gap;
        }
        if (g_settings.gameOverlayDisk) {
            D2D1_RECT_F diskCard = D2D1::RectF(start, cardTop, start + cardW, rect.bottom - 10.0f * scale);
            DrawGameMetricCard(diskCard, L"DSK", 100 - state.system.diskFreePercent, 4, scale);
            AddChip(5, diskCard);
            start += cardW + gap;
        }

        textBrush_->SetOpacity(0.90f);
        mutedBrush_->SetOpacity(0.58f);
    }

    void DrawGameMetricCard(D2D1_RECT_F rect, const wchar_t* label, int percent, int iconKind, float unused_scale) {
        const float scale = 1.0f;
        D2D1_COLOR_F metricColor = D2D1::ColorF(0.0f, 0.82f, 1.0f, 1.0f);
        switch (iconKind) {
            case 1:
                metricColor = D2D1::ColorF(0.0f, 0.82f, 1.0f, 1.0f);
                break;
            case 2:
                metricColor = D2D1::ColorF(0.83f, 0.0f, 1.0f, 1.0f);
                break;
            case 3:
                metricColor = D2D1::ColorF(0.0f, 1.0f, 0.60f, 1.0f);
                break;
            case 4:
                metricColor = D2D1::ColorF(1.0f, 0.48f, 0.0f, 1.0f);
                break;
            default:
                metricColor = D2D1::ColorF(0.0f, 1.0f, 0.65f, 1.0f);
                break;
        }

        ComPtr<ID2D1SolidColorBrush> metricBrush;
        target_->CreateSolidColorBrush(metricColor, &metricBrush);

        ComPtr<ID2D1SolidColorBrush> cardBrush;
        ComPtr<ID2D1SolidColorBrush> borderBrush;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.080f * settingsOpacity_), &cardBrush);
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.060f * settingsOpacity_), &borderBrush);
        target_->FillRoundedRectangle(D2D1::RoundedRect(rect, 16.0f * scale, 16.0f * scale), cardBrush.Get());
        target_->DrawRoundedRectangle(D2D1::RoundedRect(rect, 16.0f * scale, 16.0f * scale), borderBrush.Get(), 1.0f * scale);

        metricBrush->SetOpacity(0.24f * settingsOpacity_);
        target_->DrawRoundedRectangle(D2D1::RoundedRect(rect, 16.0f * scale, 16.0f * scale), metricBrush.Get(), 1.2f * scale);
        metricBrush->SetOpacity(1.0f);

        DrawGameIcon(D2D1::Point2F(rect.left + 18.0f * scale, rect.top + 18.0f * scale), 8.0f * scale, iconKind, metricBrush.Get(), scale);

        if (g_settings.showMetricText) {
            mutedBrush_->SetOpacity(0.56f);
            target_->DrawTextW(label, static_cast<UINT32>(wcslen(label)), smallTextFormat_.Get(),
                               D2D1::RectF(rect.left + 31.0f * scale, rect.top + 6.0f * scale,
                                           rect.right - 5.0f * scale, rect.top + 24.0f * scale),
                               mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
        }

        wchar_t value[16] = {};
        if (percent < 0) {
            wcscpy_s(value, ARRAYSIZE(value), L"--");
        } else {
            swprintf_s(value, L"%d%%", percent);
        }
        textBrush_->SetOpacity(0.90f);
        D2D1_RECT_F valueRect = g_settings.showMetricText 
            ? D2D1::RectF(rect.left + 10.0f * scale, rect.top + 22.0f * scale, rect.right - 8.0f * scale, rect.bottom - 8.0f * scale)
            : D2D1::RectF(rect.left + 31.0f * scale, rect.top + 10.0f * scale, rect.right - 5.0f * scale, rect.bottom - 8.0f * scale);
        target_->DrawTextW(value, static_cast<UINT32>(wcslen(value)), textFormat_.Get(),
                           valueRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);

        const float pct = percent < 0 ? 0.0f : Clamp(percent / 100.0f, 0.0f, 1.0f);
        D2D1_RECT_F track = D2D1::RectF(rect.left + 10.0f * scale, rect.bottom - 6.0f * scale,
                                       rect.right - 10.0f * scale, rect.bottom - 3.0f * scale);
        borderBrush->SetOpacity(0.12f * settingsOpacity_);
        target_->FillRoundedRectangle(D2D1::RoundedRect(track, 1.5f * scale, 1.5f * scale), borderBrush.Get());
        D2D1_RECT_F fillRect = D2D1::RectF(track.left, track.top,
                                          track.left + (track.right - track.left) * pct,
                                          track.bottom);
        metricBrush->SetOpacity(0.85f);
        target_->FillRoundedRectangle(D2D1::RoundedRect(fillRect, 1.5f * scale, 1.5f * scale), metricBrush.Get());
        metricBrush->SetOpacity(1.0f);
    }

    void DrawGameIcon(D2D1_POINT_2F center, float radius, int kind, ID2D1SolidColorBrush* customBrush = nullptr, float unused_scale = 1.0f) {
        const float scale = 1.0f;
        ID2D1SolidColorBrush* brush = customBrush ? customBrush : accentBrush_.Get();
        brush->SetOpacity(0.88f);
        switch (kind) {
            case 1: {
                const float size = radius * 0.82f;
                D2D1_RECT_F outerRect = D2D1::RectF(center.x - size, center.y - size, center.x + size, center.y + size);
                target_->DrawRoundedRectangle(D2D1::RoundedRect(outerRect, 2.0f * scale, 2.0f * scale), brush, 1.4f * scale);
                const float dieSize = size * 0.45f;
                D2D1_RECT_F dieRect = D2D1::RectF(center.x - dieSize, center.y - dieSize, center.x + dieSize, center.y + dieSize);
                target_->FillRoundedRectangle(D2D1::RoundedRect(dieRect, 1.0f * scale, 1.0f * scale), brush);
                const float pinLength = 2.0f * scale;
                const float pinSpacing = size * 0.45f;
                for (float offset = -pinSpacing; offset <= pinSpacing + 0.1f * scale; offset += pinSpacing) {
                    target_->DrawLine(D2D1::Point2F(center.x + offset, center.y - size),
                                      D2D1::Point2F(center.x + offset, center.y - size - pinLength),
                                      brush, 1.0f * scale);
                    target_->DrawLine(D2D1::Point2F(center.x + offset, center.y + size),
                                      D2D1::Point2F(center.x + offset, center.y + size + pinLength),
                                      brush, 1.0f * scale);
                    target_->DrawLine(D2D1::Point2F(center.x - size, center.y + offset),
                                      D2D1::Point2F(center.x - size - pinLength, center.y + offset),
                                      brush, 1.0f * scale);
                    target_->DrawLine(D2D1::Point2F(center.x + size, center.y + offset),
                                      D2D1::Point2F(center.x + size + pinLength, center.y + offset),
                                      brush, 1.0f * scale);
                }
                break;
            }
            case 2: {
                const float w = radius * 1.15f;
                const float h = radius * 0.45f;
                D2D1_RECT_F pcb = D2D1::RectF(center.x - w, center.y - h, center.x + w, center.y + h);
                target_->DrawRoundedRectangle(D2D1::RoundedRect(pcb, 1.0f * scale, 1.0f * scale), brush, 1.3f * scale);
                const float chipW = (w * 2.0f - 6.0f * scale) / 3.0f;
                const float chipH = h * 0.65f;
                for (int i = 0; i < 3; ++i) {
                    float cx = center.x - w + 2.0f * scale + i * (chipW + 1.0f * scale);
                    D2D1_RECT_F chip = D2D1::RectF(cx, center.y - chipH, cx + chipW, center.y + chipH);
                    target_->FillRectangle(chip, brush);
                }
                const float pinY = center.y + h;
                for (float px = center.x - w + 2.0f * scale; px <= center.x + w - 2.0f * scale; px += 2.2f * scale) {
                    target_->DrawLine(D2D1::Point2F(px, pinY),
                                      D2D1::Point2F(px, pinY + 1.2f * scale),
                                      brush, 0.9f * scale);
                }
                break;
            }
            case 3: {
                const float w = radius * 1.1f;
                const float h = radius * 0.7f;
                D2D1_RECT_F shroud = D2D1::RectF(center.x - w, center.y - h, center.x + w, center.y + h);
                target_->DrawRoundedRectangle(D2D1::RoundedRect(shroud, 1.5f * scale, 1.5f * scale), brush, 1.3f * scale);
                target_->DrawLine(D2D1::Point2F(center.x - w - 1.5f * scale, center.y - h - 1.0f * scale),
                                  D2D1::Point2F(center.x - w - 1.5f * scale, center.y + h + 1.0f * scale),
                                  brush, 1.4f * scale);
                const float fanR = h * 0.75f;
                target_->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(center.x, center.y), fanR, fanR), brush, 1.2f * scale);
                target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(center.x, center.y), 1.5f * scale, 1.5f * scale), brush);
                for (int i = 0; i < 4; ++i) {
                    float angle = i * 3.14159f / 4.0f;
                    float dx = std::cos(angle) * fanR;
                    float dy = std::sin(angle) * fanR;
                    target_->DrawLine(D2D1::Point2F(center.x - dx, center.y - dy),
                                      D2D1::Point2F(center.x + dx, center.y + dy),
                                      brush, 0.8f * scale);
                }
                target_->DrawLine(D2D1::Point2F(center.x - w + 3.0f * scale, center.y + h + 1.2f * scale),
                                  D2D1::Point2F(center.x + w - 2.0f * scale, center.y + h + 1.2f * scale),
                                  brush, 1.0f * scale);
                break;
            }
            case 4: {
                const float w = radius * 0.85f;
                const float h = radius * 1.05f;
                D2D1_RECT_F enc = D2D1::RectF(center.x - w, center.y - h, center.x + w, center.y + h);
                target_->DrawRoundedRectangle(D2D1::RoundedRect(enc, 2.0f * scale, 2.0f * scale), brush, 1.3f * scale);
                const float platR = w * 0.82f;
                const float platY = center.y + h - platR - 2.0f * scale;
                target_->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(center.x, platY), platR, platR), brush, 1.2f * scale);
                target_->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(center.x, platY), 2.0f * scale, 2.0f * scale), brush, 1.0f * scale);
                const float pivotX = center.x - w + 3.0f * scale;
                const float pivotY = center.y - h + 3.5f * scale;
                target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(pivotX, pivotY), 1.5f * scale, 1.5f * scale), brush);
                target_->DrawLine(D2D1::Point2F(pivotX, pivotY),
                                  D2D1::Point2F(center.x + 1.0f * scale, platY - 1.0f * scale),
                                  brush, 1.1f * scale);
                break;
            }
            case 0:
            default: {
                const float dialR = radius * 1.05f;
                target_->DrawEllipse(D2D1::Ellipse(center, dialR, dialR), brush, 1.3f * scale);
                for (int i = 0; i < 5; ++i) {
                    float angle = -3.14159f * 0.8f + i * 3.14159f * 0.4f;
                    float dx1 = std::cos(angle) * dialR;
                    float dy1 = std::sin(angle) * dialR;
                    float dx2 = std::cos(angle) * (dialR - 2.0f * scale);
                    float dy2 = std::sin(angle) * (dialR - 2.0f * scale);
                    target_->DrawLine(D2D1::Point2F(center.x + dx1, center.y + dy1),
                                      D2D1::Point2F(center.x + dx2, center.y + dy2),
                                      brush, 0.9f * scale);
                }
                target_->FillEllipse(D2D1::Ellipse(center, 1.6f * scale, 1.6f * scale), brush);
                const float needleAngle = -3.14159f * 0.25f;
                const float needleLen = dialR * 0.85f;
                target_->DrawLine(center,
                                  D2D1::Point2F(center.x + std::cos(needleAngle) * needleLen,
                                                center.y + std::sin(needleAngle) * needleLen),
                                  brush, 1.4f * scale);
                break;
            }
        }
        brush->SetOpacity(1.0f);
    }

    void DrawPageDots(D2D1_RECT_F rect, int active, int count, float unused_scale) {
        const float scale = 1.0f;
        const float gap = 6.0f * scale;
        const float total = count * 3.0f * scale + (count - 1) * gap;
        const float start = (rect.left + rect.right - total) * 0.5f;
        const float y = rect.bottom - 7.0f * scale;
        for (int i = 0; i < count; ++i) {
            mutedBrush_->SetOpacity(i == active ? 0.45f : 0.16f);
            target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(start + i * (3.0f * scale + gap), y),
                                               (i == active ? 2.0f : 1.5f) * scale,
                                               (i == active ? 2.0f : 1.5f) * scale),
                                 mutedBrush_.Get());
        }
        mutedBrush_->SetOpacity(0.58f);
    }

    void DrawMetricChip(const SharedState& state, D2D1_RECT_F rect, const wchar_t* label, int percent, int iconKind) {
        const float scale = 1.0f;
        D2D1_COLOR_F metricColor = D2D1::ColorF(0.0f, 0.82f, 1.0f, 1.0f);
        switch (iconKind) {
            case 1: {
                if (state.battery.low) {
                    metricColor = D2D1::ColorF(1.0f, 0.23f, 0.18f, 1.0f);
                } else if (state.system.charging) {
                    metricColor = D2D1::ColorF(1.0f, 0.80f, 0.0f, 1.0f);
                } else {
                    metricColor = D2D1::ColorF(0.0f, 0.90f, 0.50f, 1.0f);
                }
                break;
            }
            case 2:
                metricColor = D2D1::ColorF(0.0f, 0.65f, 1.0f, 1.0f);
                break;
            case 3:
            default:
                metricColor = D2D1::ColorF(0.83f, 0.0f, 1.0f, 1.0f);
                break;
        }

        ComPtr<ID2D1SolidColorBrush> metricBrush;
        target_->CreateSolidColorBrush(metricColor, &metricBrush);

        ComPtr<ID2D1SolidColorBrush> chipBg;
        ComPtr<ID2D1SolidColorBrush> chipBorder;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.070f * settingsOpacity_), &chipBg);
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.060f * settingsOpacity_), &chipBorder);
        target_->FillRoundedRectangle(D2D1::RoundedRect(rect, 11.0f * scale, 11.0f * scale), chipBg.Get());
        target_->DrawRoundedRectangle(D2D1::RoundedRect(rect, 11.0f * scale, 11.0f * scale), chipBorder.Get(), 1.0f * scale);

        metricBrush->SetOpacity(0.24f * settingsOpacity_);
        target_->DrawRoundedRectangle(D2D1::RoundedRect(rect, 11.0f * scale, 11.0f * scale), metricBrush.Get(), 1.2f * scale);
        metricBrush->SetOpacity(1.0f);

        D2D1_POINT_2F iconCenter = D2D1::Point2F(rect.left + 14.5f * scale, rect.top + 13.0f * scale);
        const float radius = 5.5f * scale;
        
        switch (iconKind) {
            case 1: {
                const float w = 6.2f * scale;
                const float h = 3.4f * scale;
                D2D1_RECT_F batBody = D2D1::RectF(iconCenter.x - w, iconCenter.y - h, iconCenter.x + w - 1.2f * scale, iconCenter.y + h);
                metricBrush->SetOpacity(0.85f);
                target_->DrawRoundedRectangle(D2D1::RoundedRect(batBody, 0.8f * scale, 0.8f * scale), metricBrush.Get(), 1.1f * scale);
                
                D2D1_RECT_F batTip = D2D1::RectF(iconCenter.x + w - 1.2f * scale, iconCenter.y - h * 0.45f, iconCenter.x + w, iconCenter.y + h * 0.45f);
                target_->FillRoundedRectangle(D2D1::RoundedRect(batTip, 0.4f * scale, 0.4f * scale), metricBrush.Get());
                
                const float fillPercent = percent < 0 ? 0.0f : Clamp(percent / 100.0f, 0.0f, 1.0f);
                const float fillW = (batBody.right - batBody.left - 2.0f * scale) * fillPercent;
                if (fillW > 0.5f * scale) {
                    D2D1_RECT_F batFill = D2D1::RectF(batBody.left + 1.0f * scale, batBody.top + 1.0f * scale, batBody.left + 1.0f * scale + fillW, batBody.bottom - 1.0f * scale);
                    target_->FillRoundedRectangle(D2D1::RoundedRect(batFill, 0.4f * scale, 0.4f * scale), metricBrush.Get());
                }
                
                if (state.system.charging) {
                    ComPtr<ID2D1SolidColorBrush> boltBrush;
                    target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.85f, 0.0f, 1.0f), &boltBrush);
                    D2D1_POINT_2F p1 = D2D1::Point2F(iconCenter.x - 0.8f * scale, iconCenter.y - 3.2f * scale);
                    D2D1_POINT_2F p2 = D2D1::Point2F(iconCenter.x - 1.6f * scale, iconCenter.y + 0.3f * scale);
                    D2D1_POINT_2F p3 = D2D1::Point2F(iconCenter.x - 0.4f * scale, iconCenter.y + 0.3f * scale);
                    D2D1_POINT_2F p4 = D2D1::Point2F(iconCenter.x - 1.2f * scale, iconCenter.y + 3.6f * scale);
                    D2D1_POINT_2F p5 = D2D1::Point2F(iconCenter.x + 0.6f * scale, iconCenter.y - 0.1f * scale);
                    D2D1_POINT_2F p6 = D2D1::Point2F(iconCenter.x - 0.4f * scale, iconCenter.y - 0.1f * scale);
                    ComPtr<ID2D1PathGeometry> boltGeom;
                    d2dFactory_->CreatePathGeometry(&boltGeom);
                    ComPtr<ID2D1GeometrySink> sink;
                    if (SUCCEEDED(boltGeom->Open(&sink))) {
                        sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
                        sink->AddLine(p2);
                        sink->AddLine(p3);
                        sink->AddLine(p4);
                        sink->AddLine(p5);
                        sink->AddLine(p6);
                        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                        sink->Close();
                        target_->FillGeometry(boltGeom.Get(), boltBrush.Get());
                        ComPtr<ID2D1SolidColorBrush> outline;
                        target_->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.8f), &outline);
                        target_->DrawGeometry(boltGeom.Get(), outline.Get(), 0.5f * scale);
                    }
                }
                break;
            }
            case 2: {
                D2D1_RECT_F spkBox = D2D1::RectF(iconCenter.x - radius * 0.95f, iconCenter.y - radius * 0.45f, iconCenter.x - radius * 0.35f, iconCenter.y + radius * 0.45f);
                target_->FillRoundedRectangle(D2D1::RoundedRect(spkBox, 0.5f * scale, 0.5f * scale), metricBrush.Get());
                
                ComPtr<ID2D1PathGeometry> coneGeom;
                d2dFactory_->CreatePathGeometry(&coneGeom);
                ComPtr<ID2D1GeometrySink> coneSink;
                if (SUCCEEDED(coneGeom->Open(&coneSink))) {
                    coneSink->BeginFigure(D2D1::Point2F(iconCenter.x - radius * 0.35f, iconCenter.y - radius * 0.45f), D2D1_FIGURE_BEGIN_FILLED);
                    coneSink->AddLine(D2D1::Point2F(iconCenter.x + radius * 0.15f, iconCenter.y - radius * 0.85f));
                    coneSink->AddLine(D2D1::Point2F(iconCenter.x + radius * 0.15f, iconCenter.y + radius * 0.85f));
                    coneSink->AddLine(D2D1::Point2F(iconCenter.x - radius * 0.35f, iconCenter.y + radius * 0.45f));
                    coneSink->EndFigure(D2D1_FIGURE_END_CLOSED);
                    coneSink->Close();
                    target_->FillGeometry(coneGeom.Get(), metricBrush.Get());
                }
                
                if (state.system.volumeMuted) {
                    const float off = radius * 0.35f;
                    const float xCenter = iconCenter.x + radius * 0.60f;
                    target_->DrawLine(D2D1::Point2F(xCenter - off, iconCenter.y - off),
                                      D2D1::Point2F(xCenter + off, iconCenter.y + off),
                                      metricBrush.Get(), 1.2f * scale);
                    target_->DrawLine(D2D1::Point2F(xCenter - off, iconCenter.y + off),
                                      D2D1::Point2F(xCenter + off, iconCenter.y - off),
                                      metricBrush.Get(), 1.2f * scale);
                } else {
                    const float xCenter = iconCenter.x + radius * 0.15f;
                    if (percent > 0) {
                        ComPtr<ID2D1PathGeometry> wave1;
                        d2dFactory_->CreatePathGeometry(&wave1);
                        ComPtr<ID2D1GeometrySink> sink1;
                        if (SUCCEEDED(wave1->Open(&sink1))) {
                            sink1->BeginFigure(D2D1::Point2F(xCenter + radius * 0.28f, iconCenter.y - radius * 0.38f), D2D1_FIGURE_BEGIN_HOLLOW);
                            sink1->AddBezier(D2D1::BezierSegment(
                                D2D1::Point2F(xCenter + radius * 0.5f, iconCenter.y - radius * 0.18f),
                                D2D1::Point2F(xCenter + radius * 0.5f, iconCenter.y + radius * 0.18f),
                                D2D1::Point2F(xCenter + radius * 0.28f, iconCenter.y + radius * 0.38f)
                            ));
                            sink1->EndFigure(D2D1_FIGURE_END_OPEN);
                            sink1->Close();
                            target_->DrawGeometry(wave1.Get(), metricBrush.Get(), 1.2f * scale);
                        }
                    }
                    if (percent > 50) {
                        ComPtr<ID2D1PathGeometry> wave2;
                        d2dFactory_->CreatePathGeometry(&wave2);
                        ComPtr<ID2D1GeometrySink> sink2;
                        if (SUCCEEDED(wave2->Open(&sink2))) {
                            sink2->BeginFigure(D2D1::Point2F(xCenter + radius * 0.55f, iconCenter.y - radius * 0.72f), D2D1_FIGURE_BEGIN_HOLLOW);
                            sink2->AddBezier(D2D1::BezierSegment(
                                D2D1::Point2F(xCenter + radius * 0.88f, iconCenter.y - radius * 0.35f),
                                D2D1::Point2F(xCenter + radius * 0.88f, iconCenter.y + radius * 0.35f),
                                D2D1::Point2F(xCenter + radius * 0.55f, iconCenter.y + radius * 0.72f)
                            ));
                            sink2->EndFigure(D2D1_FIGURE_END_OPEN);
                            sink2->Close();
                            target_->DrawGeometry(wave2.Get(), metricBrush.Get(), 1.2f * scale);
                        }
                    }
                }
                break;
            }
            case 3:
            default: {
                const float size = radius * 0.78f;
                D2D1_RECT_F outerRect = D2D1::RectF(iconCenter.x - size, iconCenter.y - size, iconCenter.x + size, iconCenter.y + size);
                target_->DrawRoundedRectangle(D2D1::RoundedRect(outerRect, 1.5f * scale, 1.5f * scale), metricBrush.Get(), 1.1f * scale);
                
                const float dieSize = size * 0.42f;
                D2D1_RECT_F dieRect = D2D1::RectF(iconCenter.x - dieSize, iconCenter.y - dieSize, iconCenter.x + dieSize, iconCenter.y + dieSize);
                target_->FillRoundedRectangle(D2D1::RoundedRect(dieRect, 0.5f * scale, 0.5f * scale), metricBrush.Get());
                
                const float pinLength = 1.6f * scale;
                const float pinSpacing = size * 0.5f;
                for (float offset = -pinSpacing; offset <= pinSpacing + 0.1f; offset += pinSpacing * 2.0f) {
                    target_->DrawLine(D2D1::Point2F(iconCenter.x + offset, iconCenter.y - size),
                                      D2D1::Point2F(iconCenter.x + offset, iconCenter.y - size - pinLength),
                                      metricBrush.Get(), 0.9f * scale);
                    target_->DrawLine(D2D1::Point2F(iconCenter.x + offset, iconCenter.y + size),
                                      D2D1::Point2F(iconCenter.x + offset, iconCenter.y + size + pinLength),
                                      metricBrush.Get(), 0.9f * scale);
                    target_->DrawLine(D2D1::Point2F(iconCenter.x - size, iconCenter.y + offset),
                                      D2D1::Point2F(iconCenter.x - size - pinLength, iconCenter.y + offset),
                                      metricBrush.Get(), 0.9f * scale);
                    target_->DrawLine(D2D1::Point2F(iconCenter.x + size, iconCenter.y + offset),
                                      D2D1::Point2F(iconCenter.x + size + pinLength, iconCenter.y + offset),
                                      metricBrush.Get(), 0.9f * scale);
                }
                break;
            }
        }

        if (g_settings.showMetricText) {
            D2D1_RECT_F labelRect = D2D1::RectF(rect.left + 23.0f * scale, rect.top + 3.0f * scale, rect.right - 2.0f * scale, rect.top + 20.0f * scale);
            mutedBrush_->SetOpacity(0.48f);
            target_->DrawTextW(label, static_cast<UINT32>(wcslen(label)), smallTextFormat_.Get(), labelRect,
                               mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
        }

        wchar_t value[16] = {};
        if (percent < 0) {
            wcscpy_s(value, ARRAYSIZE(value), L"--");
        } else {
            swprintf_s(value, L"%d%%", percent);
        }
        textBrush_->SetOpacity(0.90f);
        D2D1_RECT_F valueRect = g_settings.showMetricText
            ? D2D1::RectF(rect.left + 7.5f * scale, rect.top + 14.5f * scale, rect.right - 2.0f * scale, rect.bottom - 8.0f * scale)
            : D2D1::RectF(rect.left + 23.0f * scale, rect.top + 8.0f * scale, rect.right - 2.0f * scale, rect.bottom - 8.0f * scale);
        target_->DrawTextW(value, static_cast<UINT32>(wcslen(value)), textFormat_.Get(),
                           valueRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);

        const float clamped = percent < 0 ? 0.0f : Clamp(percent / 100.0f, 0.0f, 1.0f);
        D2D1_RECT_F meterTrack = D2D1::RectF(rect.left + 8.0f * scale, rect.bottom - 6.0f * scale, rect.right - 8.0f * scale, rect.bottom - 4.0f * scale);
        chipBorder->SetOpacity(0.12f * settingsOpacity_);
        target_->FillRoundedRectangle(D2D1::RoundedRect(meterTrack, 1.0f * scale, 1.0f * scale), chipBorder.Get());
        
        D2D1_RECT_F meterFill = D2D1::RectF(meterTrack.left, meterTrack.top,
                                           meterTrack.left + (meterTrack.right - meterTrack.left) * clamped,
                                           meterTrack.bottom);
        metricBrush->SetOpacity(0.85f);
        target_->FillRoundedRectangle(D2D1::RoundedRect(meterFill, 1.0f * scale, 1.0f * scale), metricBrush.Get());
        
        textBrush_->SetOpacity(0.90f);
        mutedBrush_->SetOpacity(0.58f);
    }

    void DrawMiniUsageBars(D2D1_RECT_F rect, int memory, int cpu) {
        const int bars = 18;
        const float gap = 3.0f;
        const float barWidth = ((rect.right - rect.left) - gap * (bars - 1)) / bars;
        for (int i = 0; i < bars; ++i) {
            const float mix = i / static_cast<float>(bars - 1);
            const float value = (memory * (1.0f - mix) + cpu * mix) / 100.0f;
            const float x = rect.left + i * (barWidth + gap);
            const float h = 2.0f + value * (rect.bottom - rect.top);
            accentBrush_->SetOpacity(0.18f + 0.38f * value);
            target_->FillRoundedRectangle(
                D2D1::RoundedRect(D2D1::RectF(x, rect.bottom - h, x + barWidth, rect.bottom),
                                  barWidth * 0.5f, barWidth * 0.5f),
                accentBrush_.Get());
        }
        accentBrush_->SetOpacity(1.0f);
    }

    void DrawMedia(const SharedState& state, D2D1_RECT_F rect, double now) {
        const float height = rect.bottom - rect.top;

        float radius = g_settings.w11Style ? 8.0f * g_settings.sizeScale : height * 0.5f;
        if (!g_settings.w11Style) radius = std::min(radius, 44.0f * g_settings.sizeScale);
        ComPtr<ID2D1RoundedRectangleGeometry> mask;
        d2dFactory_->CreateRoundedRectangleGeometry(D2D1::RoundedRect(rect, radius, radius), &mask);
        ComPtr<ID2D1Layer> layer;
        target_->CreateLayer(&layer);

        float expandedAlpha = std::clamp((height - 60.0f) / 60.0f, 0.0f, 1.0f);
        float collapsedAlpha = std::clamp((80.0f - height) / 30.0f, 0.0f, 1.0f);

        // Expanded UI
        if (expandedAlpha > 0.01f && mask && layer) {
            target_->PushLayer(D2D1::LayerParameters(rect, mask.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::IdentityMatrix(), expandedAlpha, nullptr, D2D1_LAYER_OPTIONS_NONE), layer.Get());
            
            // Expanded Apple DI media: large square art on left, text center.
            const float artSize = 64.0f;
            D2D1_RECT_F artRect = D2D1::RectF(rect.left + 24.0f, rect.top + 20.0f,
                                              rect.left + 24.0f + artSize, rect.top + 20.0f + artSize);
            DrawAlbumArt(state.media, artRect, now, 16.0f, true);

            float shiftX = 0.0f;
            if (state.system.micActive && state.system.cameraActive) {
                shiftX = 30.0f;
            } else if (state.system.micActive || state.system.cameraActive) {
                shiftX = 16.0f;
            }

            const float waveW = 32.0f;
            const float waveH = 20.0f;
            D2D1_RECT_F waveRect = D2D1::RectF(rect.right - 24.0f - shiftX - waveW,
                                               rect.top + 20.0f + (artSize - waveH) * 0.5f,
                                               rect.right - 24.0f - shiftX,
                                               rect.top + 20.0f + (artSize + waveH) * 0.5f);

            const float textLeft = artRect.right + 18.0f;
            const float textRight = waveRect.left - 16.0f;
            
            // Title — bold, prominent.
            D2D1_RECT_F titleRect = D2D1::RectF(textLeft, rect.top + 34.0f, textRight, rect.top + 54.0f);
            DrawMarqueeText(state.media.title.empty() ? L"Unknown" : state.media.title,
                            titleRect, textFormat_.Get(), textBrush_.Get(), now, 42.0f);

            // Artist — muted below title.
            D2D1_RECT_F artistRect = D2D1::RectF(textLeft, rect.top + 54.0f, textRight, rect.top + 74.0f);
            mutedBrush_->SetOpacity(0.55f);
            DrawMarqueeText(state.media.artist.empty() ? L"" : state.media.artist,
                            artistRect, smallTextFormat_.Get(), mutedBrush_.Get(), now, 30.0f);
            mutedBrush_->SetOpacity(0.50f);

            if (state.media.playing) {
                DrawWaveform(state, waveRect);
            } else {
                mutedBrush_->SetOpacity(0.5f);
                for (int i = 0; i < 4; ++i) {
                    target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(waveRect.left + i * 4.0f + 2.0f, (waveRect.top + waveRect.bottom) * 0.5f), 1.2f, 1.2f), mutedBrush_.Get());
                }
            }

            // Timeline (Scrubber)
            const float scrubberY = rect.top + 114.0f;
            double currentPosition = state.media.positionTicks / 10000000.0;
            double duration = state.media.endTicks / 10000000.0;
            if (state.media.playing && state.media.lastUpdatedTicks > 0) {
                currentPosition += (GetTickCount64() - state.media.lastUpdatedTicks) / 1000.0;
            }
            currentPosition = std::max(0.0, std::min(currentPosition, duration));

            auto FormatTime = [](double seconds) -> std::wstring {
                if (seconds <= 0.0 || _isnan(seconds)) return L"0:00";
                int m = static_cast<int>(seconds) / 60;
                int s = static_cast<int>(seconds) % 60;
                wchar_t buf[16];
                swprintf_s(buf, L"%d:%02d", m, s);
                return buf;
            };

            std::wstring elapsedStr = FormatTime(currentPosition);
            std::wstring remainStr = L"-" + FormatTime(duration - currentPosition);

            const float scrubLeft = rect.left + 24.0f;
            const float scrubRight = rect.right - 24.0f;
            
            mutedBrush_->SetOpacity(0.8f);
            D2D1_RECT_F elRect = D2D1::RectF(scrubLeft, scrubberY - 8.0f, scrubLeft + 40.0f, scrubberY + 8.0f);
            target_->DrawTextW(elapsedStr.c_str(), static_cast<UINT32>(elapsedStr.size()), smallTextFormat_.Get(), elRect, mutedBrush_.Get());
            
            D2D1_RECT_F remRect = D2D1::RectF(scrubRight - 36.0f, scrubberY - 8.0f, scrubRight, scrubberY + 8.0f);
            target_->DrawTextW(remainStr.c_str(), static_cast<UINT32>(remainStr.size()), smallTextFormat_.Get(), remRect, mutedBrush_.Get());

            const float barLeft = scrubLeft + 36.0f;
            const float barRight = scrubRight - 38.0f;
            const float progress = duration > 0.0 ? static_cast<float>(currentPosition / duration) : 0.0f;

            ComPtr<ID2D1SolidColorBrush> scrubBg;
            target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.15f), &scrubBg);
            target_->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(barLeft, scrubberY - 2.5f, barRight, scrubberY + 2.5f), 2.5f, 2.5f), scrubBg.Get());

            ComPtr<ID2D1SolidColorBrush> scrubFg;
            target_->CreateSolidColorBrush(state.media.art.bgra.empty() ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.9f) : state.media.art.sampledAccent, &scrubFg);
            const float scrubW = (barRight - barLeft) * progress;
            target_->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(barLeft, scrubberY - 2.5f, barLeft + scrubW, scrubberY + 2.5f), 2.5f, 2.5f), scrubFg.Get());

            // Controls
            const float cy = rect.top + 148.0f;
            const float cx = (rect.left + rect.right) * 0.5f;
            DrawMediaControls(state.media.playing, 
                              D2D1::Point2F(cx - 64.0f, cy),
                              D2D1::Point2F(cx, cy),
                              D2D1::Point2F(cx + 64.0f, cy));

            target_->PopLayer();
        }

        // Collapsed UI
        if (collapsedAlpha > 0.01f && mask && layer) {
            target_->PushLayer(D2D1::LayerParameters(rect, mask.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::IdentityMatrix(), collapsedAlpha, nullptr, D2D1_LAYER_OPTIONS_NONE), layer.Get());
            
            const float cy = (rect.top + rect.bottom) * 0.5f;
            const float artPadding = 6.0f;
            const float artSize = height - artPadding * 2.0f;
            
            D2D1_RECT_F artRect = D2D1::RectF(rect.left + 8.0f, cy - artSize * 0.5f,
                                              rect.left + 8.0f + artSize, cy + artSize * 0.5f);
            DrawAlbumArt(state.media, artRect, now, artSize * 0.5f, false);

            float shiftX = 0.0f;
            if (state.system.micActive && state.system.cameraActive) {
                shiftX = 30.0f;
            } else if (state.system.micActive || state.system.cameraActive) {
                shiftX = 16.0f;
            }

            D2D1_RECT_F waveRect = D2D1::RectF(rect.right - 42.0f - shiftX, cy - 10.0f,
                                               rect.right - 14.0f - shiftX, cy + 10.0f);
            if (state.media.playing) {
                DrawWaveform(state, waveRect);
            } else {
                mutedBrush_->SetOpacity(0.5f);
                for (int i = 0; i < 4; ++i) {
                    target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(waveRect.left + i * 6.0f + 3.0f, cy), 1.5f, 1.5f), mutedBrush_.Get());
                }
            }

            target_->PopLayer();
        }
    }

    void DrawMediaControls(bool playing, D2D1_POINT_2F prev, D2D1_POINT_2F play, D2D1_POINT_2F next) {
        DrawMediaButton(prev, 16.0f, 0, false);
        DrawMediaButton(play, 22.0f, playing ? 1 : 2, true);
        DrawMediaButton(next, 16.0f, 3, false);
    }

    void DrawMediaButton(D2D1_POINT_2F center, float radius, int kind, bool primary) {
        int buttonCmd = (kind == 0) ? 0 : ((kind == 1 || kind == 2) ? 1 : 2);
        bool isPressed = (g_pressedMediaButton.load() == buttonCmd);

        if (isPressed) {
            radius *= 0.88f; // Shrink by 12% on click
        }

        ComPtr<ID2D1SolidColorBrush> bg;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, primary ? (isPressed ? 0.16f : 0.080f) : (isPressed ? 0.10f : 0.040f)), &bg);
        target_->FillEllipse(D2D1::Ellipse(center, radius, radius), bg.Get());
        accentBrush_->SetOpacity(primary ? (isPressed ? 1.0f : 0.88f) : (isPressed ? 0.80f : 0.62f));

        if (kind == 1) {  // pause
            const float h = radius * 0.72f;
            target_->FillRoundedRectangle(D2D1::RoundedRect(
                                             D2D1::RectF(center.x - 4.0f, center.y - h * 0.5f,
                                                         center.x - 1.5f, center.y + h * 0.5f),
                                             1.0f, 1.0f),
                                         accentBrush_.Get());
            target_->FillRoundedRectangle(D2D1::RoundedRect(
                                             D2D1::RectF(center.x + 1.5f, center.y - h * 0.5f,
                                                         center.x + 4.0f, center.y + h * 0.5f),
                                             1.0f, 1.0f),
                                         accentBrush_.Get());
        } else {
            const float dir = kind == 0 ? -1.0f : 1.0f;
            const float tri = radius * (primary ? 0.70f : 0.62f);
            
            // The combined bounding box of the triangle and the line is not centered.
            // We apply a slight horizontal shift to perfectly center the next/prev icons inside the circle.
            const float cx = (kind == 0 || kind == 3) ? center.x - dir * radius * 0.16f : center.x;

            D2D1_POINT_2F p1 = D2D1::Point2F(cx - dir * tri * 0.35f, center.y - tri * 0.58f);
            D2D1_POINT_2F p2 = D2D1::Point2F(cx - dir * tri * 0.35f, center.y + tri * 0.58f);
            D2D1_POINT_2F p3 = D2D1::Point2F(cx + dir * tri * 0.55f, center.y);
            ComPtr<ID2D1PathGeometry> geom;
            d2dFactory_->CreatePathGeometry(&geom);
            ComPtr<ID2D1GeometrySink> sink;
            geom->Open(&sink);
            sink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
            sink->AddLine(p2);
            sink->AddLine(p3);
            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
            sink->Close();
            target_->FillGeometry(geom.Get(), accentBrush_.Get());

            if (kind == 0 || kind == 3) {
                const float x = cx + dir * radius * 0.55f;
                target_->DrawLine(D2D1::Point2F(x, center.y - radius * 0.45f),
                                  D2D1::Point2F(x, center.y + radius * 0.45f),
                                  accentBrush_.Get(), 1.5f);
            }
        }

        accentBrush_->SetOpacity(1.0f);
    }

    void DrawAlbumArt(const MediaSnapshot& media, D2D1_RECT_F rect, double now, float radius = 9.0f, bool drawBadge = true) {
        ComPtr<ID2D1RoundedRectangleGeometry> mask;
        HRESULT hrMask = d2dFactory_->CreateRoundedRectangleGeometry(
            D2D1::RoundedRect(rect, radius, radius), &mask);
        ComPtr<ID2D1Layer> layer;
        HRESULT hrLayer = target_->CreateLayer(nullptr, &layer);
        const bool roundedClip = SUCCEEDED(hrMask) && SUCCEEDED(hrLayer) && mask && layer;
        if (roundedClip) {
            target_->PushLayer(D2D1::LayerParameters(rect, mask.Get()), layer.Get());
        } else {
            target_->PushAxisAlignedClip(rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        }

        if (!media.art.bgra.empty()) {
            if (artGeneration_ != media.art.generation || !artBitmap_) {
                D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
                    D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
                target_->CreateBitmap(D2D1::SizeU(media.art.width, media.art.height),
                                      media.art.bgra.data(), media.art.width * 4,
                                      &props, &artBitmap_);
                artGeneration_ = media.art.generation;
            }

            D2D1_RECT_F dst = D2D1::RectF(rect.left, rect.top, rect.right, rect.bottom);
            target_->DrawBitmap(artBitmap_.Get(), dst, 1.0f,
                                D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        } else {
            accentBrush_->SetOpacity(0.24f);
            target_->FillRoundedRectangle(D2D1::RoundedRect(rect, radius, radius), accentBrush_.Get());
            accentBrush_->SetOpacity(1.0f);
            if (!media.sourceIcon.bgra.empty()) {
                D2D1_RECT_F iconRect = D2D1::RectF(rect.left + 11, rect.top + 11,
                                                  rect.right - 11, rect.bottom - 11);
                DrawBitmapPixels(media.sourceIcon, iconRect, mediaSourceIconBitmap_,
                                 mediaSourceIconGeneration_, 0.95f);
            } else {
                target_->DrawTextW(media.sourceBadge.empty() ? L"\u25b6" : media.sourceBadge.c_str(),
                                   static_cast<UINT32>(media.sourceBadge.empty() ? 1 : media.sourceBadge.size()),
                                   textFormat_.Get(), rect, textBrush_.Get());
            }
        }

        if (drawBadge && !media.sourceIcon.bgra.empty()) {
            D2D1_RECT_F badge = D2D1::RectF(rect.right - 24, rect.bottom - 22,
                                           rect.right - 3, rect.bottom - 3);
            DrawCircularBitmapPixels(media.sourceIcon,
                                     D2D1::Point2F((badge.left + badge.right) * 0.5f,
                                                   (badge.top + badge.bottom) * 0.5f),
                                     9.5f, mediaSourceIconBitmap_,
                                     mediaSourceIconGeneration_, 0.98f);
        }

        if (roundedClip) {
            target_->PopLayer();
        } else {
            target_->PopAxisAlignedClip();
        }
    }

    void DrawBitmapPixels(const BitmapPixels& pixels, D2D1_RECT_F rect,
                          ComPtr<ID2D1Bitmap>& cache, uint64_t& cachedGeneration,
                          float opacity = 1.0f) {
        if (pixels.bgra.empty()) {
            return;
        }

        if (cachedGeneration != pixels.generation || !cache) {
            D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
            target_->CreateBitmap(D2D1::SizeU(pixels.width, pixels.height),
                                  pixels.bgra.data(), pixels.width * 4,
                                  &props, &cache);
            cachedGeneration = pixels.generation;
        }

        if (cache) {
            target_->DrawBitmap(cache.Get(), rect, opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        }
    }

    // Draws a bitmap filling a rounded-rect badge with a small inset for polish.
    void DrawRoundedBitmapPixels(const BitmapPixels& pixels, D2D1_RECT_F badge,
                                 float cornerRadius,
                                 ComPtr<ID2D1Bitmap>& cache, uint64_t& cachedGeneration,
                                 float opacity = 1.0f) {
        if (pixels.bgra.empty()) return;

        // 2px inset so the icon has clean edges inside the badge.
        const float pad = 2.0f;
        D2D1_RECT_F iconRect = D2D1::RectF(badge.left + pad, badge.top + pad,
                                           badge.right - pad, badge.bottom - pad);
        const float innerR = std::max(0.0f, cornerRadius - pad);

        ComPtr<ID2D1RoundedRectangleGeometry> mask;
        d2dFactory_->CreateRoundedRectangleGeometry(
            D2D1::RoundedRect(iconRect, innerR, innerR), &mask);
        ComPtr<ID2D1Layer> layer;
        target_->CreateLayer(nullptr, &layer);

        if (mask && layer) {
            target_->PushLayer(
                D2D1::LayerParameters(iconRect, mask.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE),
                layer.Get());
            DrawBitmapPixels(pixels, iconRect, cache, cachedGeneration, opacity);
            target_->PopLayer();
        } else {
            DrawBitmapPixels(pixels, iconRect, cache, cachedGeneration, opacity);
        }
    }

    void DrawCircularBitmapPixels(const BitmapPixels& pixels, D2D1_POINT_2F center, float radius,
                                  ComPtr<ID2D1Bitmap>& cache, uint64_t& cachedGeneration,
                                  float opacity = 1.0f) {
        if (pixels.bgra.empty()) {
            return;
        }

        D2D1_RECT_F rect = D2D1::RectF(center.x - radius, center.y - radius,
                                      center.x + radius, center.y + radius);
        ComPtr<ID2D1EllipseGeometry> ellipse;
        d2dFactory_->CreateEllipseGeometry(D2D1::Ellipse(center, radius, radius), &ellipse);
        ComPtr<ID2D1Layer> layer;
        target_->CreateLayer(nullptr, &layer);

        if (ellipse && layer) {
            target_->PushLayer(D2D1::LayerParameters(
                                  rect, ellipse.Get(), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE),
                              layer.Get());
            DrawBitmapPixels(pixels, rect, cache, cachedGeneration, opacity);
            target_->PopLayer();
        } else {
            DrawBitmapPixels(pixels, rect, cache, cachedGeneration, opacity);
        }

        ComPtr<ID2D1SolidColorBrush> border;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.12f), &border);
        if (border) {
            target_->DrawEllipse(D2D1::Ellipse(center, radius, radius), border.Get(), 1.0f);
        }
    }

    void DrawMarqueeText(const std::wstring& text, D2D1_RECT_F rect, IDWriteTextFormat* format,
                         ID2D1Brush* brush, double now, float speed) {
        if (!format || !brush || text.empty()) {
            return;
        }

        ComPtr<IDWriteTextLayout> layout;
        dwriteFactory_->CreateTextLayout(text.c_str(), static_cast<UINT32>(text.size()),
                                         format, 2000.0f, rect.bottom - rect.top, &layout);
        DWRITE_TEXT_METRICS metrics = {};
        if (layout) {
            layout->GetMetrics(&metrics);
        }

        const float available = rect.right - rect.left;
        
        // We only want to clip horizontally so we don't bleed out of the pill.
        // We expand the vertical bounds so descenders/ascenders aren't cut off.
        D2D1_RECT_F clipRect = rect;
        clipRect.top -= 10.0f;
        clipRect.bottom += 10.0f;
        target_->PushAxisAlignedClip(clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

        if (metrics.widthIncludingTrailingWhitespace <= available) {
            target_->DrawTextLayout(D2D1::Point2F(rect.left, rect.top), layout.Get(), brush,
                                    D2D1_DRAW_TEXT_OPTIONS_NONE);
        } else {
            const float cycle = metrics.widthIncludingTrailingWhitespace + 38.0f;
            const float offset = std::fmod(static_cast<float>(now) * speed, cycle);
            target_->DrawTextLayout(D2D1::Point2F(rect.left - offset, rect.top), layout.Get(),
                                    brush, D2D1_DRAW_TEXT_OPTIONS_NONE);
            target_->DrawTextLayout(D2D1::Point2F(rect.left - offset + cycle, rect.top),
                                    layout.Get(), brush, D2D1_DRAW_TEXT_OPTIONS_NONE);
        }
        target_->PopAxisAlignedClip();
    }

    void DrawWaveform(const SharedState& state, D2D1_RECT_F rect) {
        const float gap = 2.5f;
        const float minBarWidth = 2.0f;
        const float availableW = rect.right - rect.left;
        size_t count = std::max<size_t>(1, static_cast<size_t>((availableW + gap) / (minBarWidth + gap)));
        count = std::min<size_t>(count, 32);

        const float barWidth = (availableW - gap * (count - 1)) / count;
        const float centerY = (rect.top + rect.bottom) * 0.5f;
        const float maxH = (rect.bottom - rect.top) * 0.86f;

        // Use a step size of 4 samples (approx 40ms) so bars aren't identical
        const size_t step = 4;

        for (size_t i = 0; i < count; ++i) {
            const size_t offset = (count - i) * step;
            const size_t source = (state.waveformWrite + state.waveform.size() - offset) %
                                  state.waveform.size();
            const float amp = Clamp(state.waveform[source], 0.03f, 1.0f);
            const float h = std::max(3.0f, amp * maxH);
            const float x = rect.left + i * (barWidth + gap);
            D2D1_RECT_F bar = D2D1::RectF(x, centerY - h * 0.5f, x + barWidth, centerY + h * 0.5f);
            accentBrush_->SetOpacity(0.45f + 0.5f * amp);
            target_->FillRoundedRectangle(D2D1::RoundedRect(bar, barWidth * 0.5f, barWidth * 0.5f),
                                         accentBrush_.Get());
        }
        accentBrush_->SetOpacity(1.0f);
    }

    void DrawClipboard(const SharedState& state, D2D1_RECT_F rect) {
        if (rect.bottom - rect.top < 40.0f || rect.right - rect.left < 100.0f) return;
        const double now = NowSeconds();
        const float ttl = 2.5f;
        const float remaining = Clamp(static_cast<float>(state.clipboard.expiresAt - now), 0.0f, ttl);
        const float progress = remaining / ttl;

        D2D1_RECT_F badge = D2D1::RectF(rect.left + 12, rect.top + 10,
                                       rect.left + 50, rect.bottom - 10);
        ComPtr<ID2D1SolidColorBrush> badgeBg;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.070f), &badgeBg);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, 13, 13), badgeBg.Get());
        target_->DrawRoundedRectangle(D2D1::RoundedRect(badge, 13, 13), accentBrush_.Get(), 1.0f);

        if (!state.clipboard.appIcon.bgra.empty()) {
            DrawRoundedBitmapPixels(state.clipboard.appIcon,
                                    badge, 10.0f,
                                    clipboardIconBitmap_,
                                    clipboardIconGeneration_, 0.96f);
        } else {
            // Fallback icon when no app icon is available.
            // Use Segoe Fluent Icons for a native Windows 11 look.
            const wchar_t* glyph = state.clipboard.image ? L"\uE114" : L"\uE8C8";
            // For image: show a bright gradient-style photo icon placeholder.
            ComPtr<ID2D1SolidColorBrush> iconBg;
            const D2D1_COLOR_F iconColor = state.clipboard.image
                ? D2D1::ColorF(0.3f, 0.3f, 0.35f, 0.90f)
                : D2D1::ColorF(1, 1, 1, 0.055f * settingsOpacity_);
            target_->CreateSolidColorBrush(iconColor, &iconBg);
            target_->FillRoundedRectangle(D2D1::RoundedRect(badge, 13, 13), iconBg.Get());
            textBrush_->SetOpacity(0.95f);
            
            // Draw glyph perfectly centered in the badge rectangle.
            if (iconFormat_) {
                iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                target_->DrawTextW(glyph,
                                   static_cast<UINT32>(wcslen(glyph)), iconFormat_.Get(), badge,
                                   textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
                iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
                iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
            }
            
            textBrush_->SetOpacity(0.90f);
        }

        D2D1_RECT_F titleRect = D2D1::RectF(badge.right + 11, rect.top + 9,
                                           rect.right - 18, rect.top + 25);
        mutedBrush_->SetOpacity(0.48f);
        const std::wstring clipTitle =
            state.clipboard.appName.empty()
                ? (state.clipboard.image ? std::wstring(L"Image copied") : std::wstring(L"Clipboard"))
                : state.clipboard.appName + L"  \u00b7  Clipboard";
        target_->DrawTextW(clipTitle.c_str(), static_cast<UINT32>(clipTitle.size()),
                           smallTextFormat_.Get(), titleRect, mutedBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);

        D2D1_RECT_F textRect = D2D1::RectF(badge.right + 11, rect.top + 25,
                                           rect.right - 18, rect.bottom - 12);
        DrawMarqueeText(state.clipboard.text.empty() ? L"Copied" : state.clipboard.text,
                        textRect, textFormat_.Get(), textBrush_.Get(), now, 34.0f);

        D2D1_RECT_F track = D2D1::RectF(badge.right + 11, rect.bottom - 8,
                                       rect.right - 20, rect.bottom - 6);
        ComPtr<ID2D1SolidColorBrush> trackBrush;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.08f), &trackBrush);
        target_->FillRoundedRectangle(D2D1::RoundedRect(track, 1, 1), trackBrush.Get());
        D2D1_RECT_F fill = D2D1::RectF(track.left, track.top,
                                      track.left + (track.right - track.left) * progress,
                                      track.bottom);
        accentBrush_->SetOpacity(0.55f);
        target_->FillRoundedRectangle(D2D1::RoundedRect(fill, 1, 1), accentBrush_.Get());
        accentBrush_->SetOpacity(1.0f);
        mutedBrush_->SetOpacity(0.58f);
    }

    void DrawNotification(const SharedState& state, D2D1_RECT_F rect) {
        if (rect.bottom - rect.top < 48.0f || rect.right - rect.left < 120.0f) return;
        const double now = NowSeconds();
        const float ttl = 4.0f;
        const float remaining = Clamp(static_cast<float>(state.notification.expiresAt - now), 0.0f, ttl);
        const float progress = remaining / ttl;

        const float cy = (rect.top + rect.bottom) * 0.5f;
        // Apple DI: app icon is a large iOS-style rounded square.
        const float iconSz = (rect.bottom - rect.top) - 16.0f;
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 14, cy - iconSz * 0.5f,
                                        rect.left + 14 + iconSz, cy + iconSz * 0.5f);
        const float br = iconSz * 0.35f;  // Softer iOS superellipse-like squircle.

        // Icon background plate.
        ComPtr<ID2D1SolidColorBrush> plateBrush;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.12f), &plateBrush);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), plateBrush.Get());

        if (!state.notification.icon.bgra.empty()) {
            DrawRoundedBitmapPixels(state.notification.icon, badge, br,
                                    notificationIconBitmap_, notificationIconGeneration_, 1.0f);

            // Draw a red dot (badge) at the top-right of the app icon
            ComPtr<ID2D1SolidColorBrush> badgeColor;
            target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.23f, 0.18f, 1.0f), &badgeColor);
            
            const float dotR = iconSz * 0.13f;
            const float dotX = badge.right - dotR * 0.5f;
            const float dotY = badge.top + dotR * 0.5f;
            
            target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(dotX, dotY), dotR, dotR), badgeColor.Get());

            ComPtr<ID2D1SolidColorBrush> badgeBorder;
            target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.95f), &badgeBorder);
            target_->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(dotX, dotY), dotR, dotR), badgeBorder.Get(), 0.9f);
        } else {
            DrawNotificationFallbackIcon(
                D2D1::Point2F((badge.left + badge.right) * 0.5f, cy), iconSz * 0.38f);
        }

        const float tx = badge.right + 14;
        // App name ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÂ¢Ã¢â€šÂ¬Ã‚Â small dim label.
        D2D1_RECT_F appRect = D2D1::RectF(tx, cy - 22, rect.right - 14, cy - 6);
        mutedBrush_->SetOpacity(0.50f);
        target_->DrawTextW(state.notification.app.c_str(),
                           static_cast<UINT32>(state.notification.app.size()),
                           smallTextFormat_.Get(), appRect, mutedBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);

        // Title ÃƒÆ’Ã‚Â¢ÃƒÂ¢Ã¢â‚¬Å¡Ã‚Â¬ÃƒÂ¢Ã¢â€šÂ¬Ã‚Â bold white.
        D2D1_RECT_F titleRect = D2D1::RectF(tx, cy - 4, rect.right - 14, cy + 17);
        textBrush_->SetOpacity(0.95f);
        DrawMarqueeText(state.notification.title.empty() ? L"Notification" : state.notification.title,
                        titleRect, textFormat_.Get(), textBrush_.Get(), now, 28.0f);
        textBrush_->SetOpacity(0.90f);

        // Thicker, softer progress bar at bottom.
        D2D1_RECT_F track = D2D1::RectF(tx, rect.bottom - 7, rect.right - 14, rect.bottom - 3);
        ComPtr<ID2D1SolidColorBrush> trackBrush;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.08f), &trackBrush);
        target_->FillRoundedRectangle(D2D1::RoundedRect(track, 2, 2), trackBrush.Get());
        D2D1_RECT_F fill = D2D1::RectF(track.left, track.top,
                                       track.left + (track.right - track.left) * progress, track.bottom);
        accentBrush_->SetOpacity(0.75f);
        target_->FillRoundedRectangle(D2D1::RoundedRect(fill, 2, 2), accentBrush_.Get());
        accentBrush_->SetOpacity(1.0f);
        mutedBrush_->SetOpacity(0.50f);
    }

    void DrawVolume(const SharedState& state, D2D1_RECT_F rect) {
        if (rect.bottom - rect.top < 24.0f || rect.right - rect.left < 140.0f) return;
        const bool muted = state.volume.muted || state.volume.percent == 0;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float badgeSz = (rect.bottom - rect.top) - 16.0f;
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 14, cy - badgeSz * 0.5f,
                                        rect.left + 14 + badgeSz, cy + badgeSz * 0.5f);
        const float br = badgeSz * 0.35f; // Softer squircle corners

        ComPtr<ID2D1SolidColorBrush> badgeBg;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.12f), &badgeBg);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), badgeBg.Get());

        const wchar_t* glyph = muted ? L"\uE74F" : L"\uE993"; // Mute and Volume2 from Segoe Fluent Icons
        textBrush_->SetOpacity(0.95f);
        
        if (iconFormat_) {
            iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            
            target_->DrawTextW(glyph, static_cast<UINT32>(wcslen(glyph)), iconFormat_.Get(), badge,
                               textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
                               
            iconFormat_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            iconFormat_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        }

        const float tx = badge.right + 14;
        D2D1_RECT_F labelRect = D2D1::RectF(tx, cy - 22, rect.right - 58, cy - 6);
        mutedBrush_->SetOpacity(0.50f);
        const std::wstring deviceLabel =
            state.volume.deviceName.empty() ? std::wstring(L"Volume") : state.volume.deviceName;
        target_->DrawTextW(deviceLabel.c_str(), static_cast<UINT32>(deviceLabel.size()),
                           smallTextFormat_.Get(), labelRect, mutedBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);

        wchar_t value[16] = {};
        if (muted) {
            wcscpy_s(value, ARRAYSIZE(value), L"Muted");
        } else {
            swprintf_s(value, L"%d%%", state.volume.percent);
        }
        D2D1_RECT_F valueRect = D2D1::RectF(rect.right - 58, cy - 22, rect.right - 14, cy - 6);
        target_->DrawTextW(value, static_cast<UINT32>(wcslen(value)), smallTextFormat_.Get(),
                           valueRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        textBrush_->SetOpacity(0.90f);

        D2D1_RECT_F track = D2D1::RectF(tx, cy + 2, rect.right - 14, cy + 6);
        ComPtr<ID2D1SolidColorBrush> trackBrush;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.08f), &trackBrush);
        target_->FillRoundedRectangle(D2D1::RoundedRect(track, 2, 2), trackBrush.Get());
        const float pct = Clamp(state.volume.percent / 100.0f, 0.0f, 1.0f);
        D2D1_RECT_F fill = D2D1::RectF(track.left, track.top,
                                       track.left + (track.right - track.left) * pct,
                                       track.bottom);
        accentBrush_->SetOpacity(muted ? 0.24f : 0.85f);
        target_->FillRoundedRectangle(D2D1::RoundedRect(fill, 2, 2), accentBrush_.Get());
        accentBrush_->SetOpacity(1.0f);
        mutedBrush_->SetOpacity(0.58f);
    }

    void DrawCapsLock(const SharedState& state, D2D1_RECT_F rect) {
        if (rect.bottom - rect.top < 24.0f || rect.right - rect.left < 110.0f) return;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float badgeSz = (rect.bottom - rect.top) - 16.0f;
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 14, cy - badgeSz * 0.5f,
                                        rect.left + 14 + badgeSz, cy + badgeSz * 0.5f);
        const float br = badgeSz * 0.35f;

        // Badge Background (translucent physical keycap base)
        ComPtr<ID2D1SolidColorBrush> badgeBg;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.08f), &badgeBg);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), badgeBg.Get());

        // Keycap Border for 3D visual depth
        ComPtr<ID2D1SolidColorBrush> badgeBorder;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.16f), &badgeBorder);
        target_->DrawRoundedRectangle(D2D1::RoundedRect(badge, br, br), badgeBorder.Get(), 1.0f);

        const wchar_t* glyph = nullptr;
        std::wstring label;
        bool isOn = false;

        if (state.capsLock.isNumEvent) {
            glyph = L"1";
            label = L"Num Lock";
            isOn = state.capsLock.numOn;
        } else {
            glyph = L"A";
            label = L"Caps Lock";
            isOn = state.capsLock.capsOn;
        }

        // Draw central bold keycap glyph, vertically and horizontally centered
        textBrush_->SetOpacity(0.95f);
        target_->DrawTextW(glyph, static_cast<UINT32>(wcslen(glyph)), clockFormat_.Get(), badge,
                           textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);

        // Draw physical glowing status LED inside the keycap
        ComPtr<ID2D1SolidColorBrush> ledBrush;
        D2D1_COLOR_F ledColor = isOn
            ? D2D1::ColorF(0.19f, 0.83f, 0.38f, 1.0f)   // Green glowing LED for ON
            : D2D1::ColorF(1.0f,  1.0f,  1.0f,  0.22f);  // Dim white for OFF
        target_->CreateSolidColorBrush(ledColor, &ledBrush);

        D2D1_POINT_2F ledCenter = D2D1::Point2F(badge.right - 5.0f, badge.bottom - 5.0f);
        target_->FillEllipse(D2D1::Ellipse(ledCenter, 3.0f, 3.0f), ledBrush.Get());

        // Draw label text
        const float tx = badge.right + 14;
        D2D1_RECT_F labelRect = D2D1::RectF(tx, cy - 10, rect.right - 40, cy + 10);
        target_->DrawTextW(label.c_str(), static_cast<UINT32>(label.size()),
                           smallTextFormat_.Get(), labelRect, textBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);

        // Draw status string (ON/OFF)
        std::wstring status = isOn ? L"ON" : L"OFF";
        D2D1_RECT_F statusRect = D2D1::RectF(rect.right - 40, cy - 10, rect.right - 14, cy + 10);
        mutedBrush_->SetOpacity(0.80f);
        target_->DrawTextW(status.c_str(), static_cast<UINT32>(status.size()), smallTextFormat_.Get(),
                           statusRect, mutedBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
    }

    void DrawDevice(const SharedState& state, D2D1_RECT_F rect) {
        if (rect.bottom - rect.top < 24.0f || rect.right - rect.left < 100.0f) return;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const bool connected = (state.device.eventType == DeviceEventType::Connected);

        // Badge circle with colored dot
        const float badgeSz = (rect.bottom - rect.top) - 16.0f;
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 14, cy - badgeSz * 0.5f,
                                        rect.left + 14 + badgeSz, cy + badgeSz * 0.5f);
        const float br = badgeSz * 0.35f;

        ComPtr<ID2D1SolidColorBrush> badgeBg;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.10f), &badgeBg);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), badgeBg.Get());

        // Small colored status dot inside badge
        ComPtr<ID2D1SolidColorBrush> dotBrush;
        D2D1_COLOR_F dotColor = connected
            ? D2D1::ColorF(0.19f, 0.83f, 0.38f, 1.0f)   // green
            : D2D1::ColorF(1.0f,  0.27f, 0.22f, 1.0f);  // red
        target_->CreateSolidColorBrush(dotColor, &dotBrush);

        // Draw USB plug icon using simple rects
        const float px = (badge.left + badge.right) * 0.5f;
        const float py = (badge.top + badge.bottom) * 0.5f;
        const float ps = badgeSz * 0.28f;

        ComPtr<ID2D1SolidColorBrush> iconBrush;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.90f), &iconBrush);

        // Plug body
        D2D1_RECT_F plug = D2D1::RectF(px - ps * 0.4f, py - ps * 0.8f, px + ps * 0.4f, py + ps * 0.6f);
        target_->FillRoundedRectangle(D2D1::RoundedRect(plug, 1.5f, 1.5f), iconBrush.Get());
        // Plug prong left
        D2D1_RECT_F pl = D2D1::RectF(px - ps * 0.35f, py - ps * 1.2f, px - ps * 0.12f, py - ps * 0.8f);
        target_->FillRectangle(pl, iconBrush.Get());
        // Plug prong right
        D2D1_RECT_F pr = D2D1::RectF(px + ps * 0.12f, py - ps * 1.2f, px + ps * 0.35f, py - ps * 0.8f);
        target_->FillRectangle(pr, iconBrush.Get());
        // Plug cord
        D2D1_RECT_F cord = D2D1::RectF(px - ps * 0.1f, py + ps * 0.6f, px + ps * 0.1f, py + ps * 1.0f);
        target_->FillRoundedRectangle(D2D1::RoundedRect(cord, 1.0f, 1.0f), iconBrush.Get());

        // Status dot (bottom-right of badge)
        D2D1_POINT_2F dotCenter = D2D1::Point2F(badge.right - 4.5f, badge.bottom - 4.5f);
        target_->FillEllipse(D2D1::Ellipse(dotCenter, 4.5f, 4.5f), dotBrush.Get());

        // Text block
        const float tx = badge.right + 14;
        mutedBrush_->SetOpacity(0.50f);
        std::wstring label = connected ? L"Device Connected" : L"Device Removed";
        D2D1_RECT_F labelRect = D2D1::RectF(tx, cy - 22, rect.right - 14, cy - 5);
        target_->DrawTextW(label.c_str(), static_cast<UINT32>(label.size()),
                           smallTextFormat_.Get(), labelRect, mutedBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);

        textBrush_->SetOpacity(0.95f);
        const std::wstring& name = state.device.deviceName.empty()
            ? (state.device.isBluetoothLike ? std::wstring(L"Bluetooth") : std::wstring(L"USB Device"))
            : state.device.deviceName;
        D2D1_RECT_F nameRect = D2D1::RectF(tx, cy - 3, rect.right - 14, cy + 17);
        target_->DrawTextW(name.c_str(), static_cast<UINT32>(name.size()),
                           textFormat_.Get(), nameRect, textBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);
        textBrush_->SetOpacity(0.90f);
        mutedBrush_->SetOpacity(0.58f);
    }

    void DrawNotificationFallbackIcon(D2D1_POINT_2F center, float radius) {
        ComPtr<ID2D1SolidColorBrush> bg;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.055f * settingsOpacity_), &bg);
        target_->FillEllipse(D2D1::Ellipse(center, radius, radius), bg.Get());

        accentBrush_->SetOpacity(0.92f);
        
        ComPtr<ID2D1PathGeometry> bellGeom;
        d2dFactory_->CreatePathGeometry(&bellGeom);
        ComPtr<ID2D1GeometrySink> sink;
        if (SUCCEEDED(bellGeom->Open(&sink))) {
            const float r = radius;
            sink->BeginFigure(D2D1::Point2F(center.x - r * 0.15f, center.y - r * 0.7f), D2D1_FIGURE_BEGIN_FILLED);
            sink->AddBezier(D2D1::BezierSegment(
                D2D1::Point2F(center.x - r * 0.4f, center.y - r * 0.7f),
                D2D1::Point2F(center.x - r * 0.5f, center.y - r * 0.2f),
                D2D1::Point2F(center.x - r * 0.55f, center.y + r * 0.2f)
            ));
            sink->AddBezier(D2D1::BezierSegment(
                D2D1::Point2F(center.x - r * 0.6f, center.y + r * 0.45f),
                D2D1::Point2F(center.x - r * 0.85f, center.y + r * 0.55f),
                D2D1::Point2F(center.x - r * 0.85f, center.y + r * 0.6f)
            ));
            sink->AddLine(D2D1::Point2F(center.x + r * 0.85f, center.y + r * 0.6f));
            sink->AddBezier(D2D1::BezierSegment(
                D2D1::Point2F(center.x + r * 0.85f, center.y + r * 0.55f),
                D2D1::Point2F(center.x + r * 0.6f, center.y + r * 0.45f),
                D2D1::Point2F(center.x + r * 0.55f, center.y + r * 0.2f)
            ));
            sink->AddBezier(D2D1::BezierSegment(
                D2D1::Point2F(center.x + r * 0.5f, center.y - r * 0.2f),
                D2D1::Point2F(center.x + r * 0.4f, center.y - r * 0.7f),
                D2D1::Point2F(center.x + r * 0.15f, center.y - r * 0.7f)
            ));
            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
            sink->Close();

            target_->FillGeometry(bellGeom.Get(), accentBrush_.Get());
            target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(center.x, center.y + r * 0.7f), r * 0.22f, r * 0.22f), accentBrush_.Get());
            target_->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(center.x, center.y - r * 0.75f), r * 0.18f, r * 0.18f), accentBrush_.Get(), 1.4f);
        }

        ComPtr<ID2D1SolidColorBrush> badgeColor;
        target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.23f, 0.18f, 1.0f), &badgeColor);
        const float badgeR = radius * 0.28f;
        const float badgeX = center.x + radius * 0.65f;
        const float badgeY = center.y - radius * 0.5f;
        target_->FillEllipse(D2D1::Ellipse(D2D1::Point2F(badgeX, badgeY), badgeR, badgeR), badgeColor.Get());

        ComPtr<ID2D1SolidColorBrush> badgeBorder;
        target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.95f), &badgeBorder);
        target_->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(badgeX, badgeY), badgeR, badgeR), badgeBorder.Get(), 0.9f);

        accentBrush_->SetOpacity(1.0f);
    }

    void DrawBattery(const SharedState& state, D2D1_RECT_F rect) {
        if (rect.bottom - rect.top < 24.0f || rect.right - rect.left < 140.0f) return;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float badgeSz = (rect.bottom - rect.top) - 16.0f;
        D2D1_RECT_F badge = D2D1::RectF(rect.left + 14, cy - badgeSz * 0.5f,
                                        rect.left + 14 + badgeSz, cy + badgeSz * 0.5f);
        const float br = badgeSz * 0.35f;

        ComPtr<ID2D1SolidColorBrush> badgeBg;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.12f), &badgeBg);
        target_->FillRoundedRectangle(D2D1::RoundedRect(badge, br, br), badgeBg.Get());

        // Draw battery vector icon
        const float bx = badge.left + badgeSz * 0.25f;
        const float by = cy - badgeSz * 0.22f;
        const float bw = badgeSz * 0.45f;
        const float bh = badgeSz * 0.44f;
        D2D1_RECT_F batRect = D2D1::RectF(bx, by, bx + bw, by + bh);
        
        ComPtr<ID2D1SolidColorBrush> batBorder;
        target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.85f), &batBorder);
        target_->DrawRoundedRectangle(D2D1::RoundedRect(batRect, 2, 2), batBorder.Get(), 1.5f);
        
        // Battery Terminal (Nub)
        D2D1_RECT_F nubRect = D2D1::RectF(batRect.right, cy - 3, batRect.right + 2.5f, cy + 3);
        target_->FillRectangle(nubRect, batBorder.Get());

        // Battery Fill
        const float pct = Clamp(state.battery.percent / 100.0f, 0.0f, 1.0f);
        D2D1_RECT_F fillRect = D2D1::RectF(batRect.left + 2, batRect.top + 2,
                                           batRect.left + 2 + (bw - 4) * pct, batRect.bottom - 2);
        
        ComPtr<ID2D1SolidColorBrush> batFill;
        if (state.battery.low) {
            target_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.23f, 0.18f, 1.0f), &batFill); // Red
        } else if (state.battery.charging) {
            target_->CreateSolidColorBrush(D2D1::ColorF(0.19f, 0.83f, 0.38f, 1.0f), &batFill); // Green
        } else {
            target_->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.95f), &batFill); // White
        }
        target_->FillRoundedRectangle(D2D1::RoundedRect(fillRect, 1, 1), batFill.Get());

        // Text Labels
        const float tx = badge.right + 14;
        D2D1_RECT_F labelRect = D2D1::RectF(tx, cy - 22, rect.right - 14, cy - 6);
        mutedBrush_->SetOpacity(0.50f);
        std::wstring label = state.battery.charging ? L"Power Connected" : L"Battery Alert";
        target_->DrawTextW(label.c_str(), static_cast<UINT32>(label.size()),
                           smallTextFormat_.Get(), labelRect, mutedBrush_.Get(),
                           D2D1_DRAW_TEXT_OPTIONS_CLIP);

        wchar_t value[128] = {};
        if (state.battery.secondsRemaining != BATTERY_LIFE_UNKNOWN && !state.battery.charging) {
            const DWORD minutes = state.battery.secondsRemaining / 60;
            swprintf_s(value, ARRAYSIZE(value), L"%d%% \u2022 %luh %02lum left", 
                       state.battery.percent, minutes / 60, minutes % 60);
        } else {
            swprintf_s(value, ARRAYSIZE(value), L"%d%%", state.battery.percent);
        }

        D2D1_RECT_F valueRect = D2D1::RectF(tx, cy - 4, rect.right - 14, cy + 17);
        textBrush_->SetOpacity(0.95f);
        target_->DrawTextW(value, static_cast<UINT32>(wcslen(value)), textFormat_.Get(),
                           valueRect, textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
        textBrush_->SetOpacity(0.90f);
    }

    void DrawProgress(const SharedState& state, D2D1_RECT_F rect) {
        wchar_t buffer[64] = {};
        swprintf_s(buffer, L"Progress %d%%", state.progress.percent);
        target_->DrawTextW(buffer, static_cast<UINT32>(wcslen(buffer)), textFormat_.Get(),
                           D2D1::RectF(rect.left + 18, rect.top + 14, rect.right - 18,
                                       rect.bottom - 10),
                           textBrush_.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
    }

    void DrawProgressRing(D2D1_RECT_F rect, int percent) {
        ComPtr<ID2D1PathGeometry> geometry;
        d2dFactory_->CreatePathGeometry(&geometry);
        ComPtr<ID2D1GeometrySink> sink;
        geometry->Open(&sink);

        const float cx = (rect.left + rect.right) * 0.5f;
        const float cy = (rect.top + rect.bottom) * 0.5f;
        const float rx = (rect.right - rect.left) * 0.5f + 6.0f;
        const float ry = (rect.bottom - rect.top) * 0.5f + 6.0f;
        const float start = -3.14159265f * 0.5f;
        const float sweep = 2.0f * 3.14159265f * Clamp(percent / 100.0f, 0.0f, 1.0f);
        const int segments = std::max(2, static_cast<int>(48 * percent / 100.0f));

        auto pointAt = [&](float a) {
            return D2D1::Point2F(cx + std::cos(a) * rx, cy + std::sin(a) * ry);
        };

        sink->BeginFigure(pointAt(start), D2D1_FIGURE_BEGIN_HOLLOW);
        for (int i = 1; i <= segments; ++i) {
            const float a = start + sweep * i / segments;
            sink->AddLine(pointAt(a));
        }
        sink->EndFigure(D2D1_FIGURE_END_OPEN);
        sink->Close();

        accentBrush_->SetOpacity(0.92f);
        target_->DrawGeometry(geometry.Get(), accentBrush_.Get(), 3.0f);
        accentBrush_->SetOpacity(1.0f);
    }

    HWND hwnd_ = nullptr;
    HDC memDc_ = nullptr;
    HBITMAP dib_ = nullptr;
    HBITMAP oldBitmap_ = nullptr;
    int bitmapWidth_ = 0;
    int bitmapHeight_ = 0;

    ComPtr<ID2D1Factory> d2dFactory_;
    ComPtr<ID2D1DCRenderTarget> target_;
    ComPtr<IDWriteFactory> dwriteFactory_;
    ComPtr<IDWriteTextFormat> textFormat_;
    ComPtr<IDWriteTextFormat> smallTextFormat_;
    ComPtr<IDWriteTextFormat> boldTextFormat_;
    ComPtr<IDWriteTextFormat> hugeTextFormat_;
    ComPtr<IDWriteTextFormat> clockFormat_;
    ComPtr<IDWriteTextFormat> iconFormat_;
    ComPtr<ID2D1SolidColorBrush> accentBrush_;
    ComPtr<ID2D1SolidColorBrush> redBrush_;
    ComPtr<ID2D1SolidColorBrush> textBrush_;
    ComPtr<ID2D1SolidColorBrush> mutedBrush_;
    ComPtr<ID2D1SolidColorBrush> tintBrush_;
    ComPtr<ID2D1SolidColorBrush> shadowBrush_;
    ComPtr<ID2D1Bitmap> artBitmap_;
    ComPtr<ID2D1Bitmap> notificationIconBitmap_;
    ComPtr<ID2D1Bitmap> mediaSourceIconBitmap_;
    ComPtr<ID2D1Bitmap> clipboardIconBitmap_;
    uint64_t artGeneration_ = 0;
    uint64_t notificationIconGeneration_ = 0;
    uint64_t mediaSourceIconGeneration_ = 0;
    uint64_t clipboardIconGeneration_ = 0;
    float settingsOpacity_ = 0.96f;
    D2D1_COLOR_F pillBgColor_ = D2D1::ColorF(0.051f, 0.051f, 0.059f, 1.0f);
};

Activity ActivityForKind(IslandKind kind, const Settings& settings, const SharedState& state) {
    Activity activity;
    activity.kind = kind;

    switch (kind) {
        case IslandKind::Media:
            activity.width = 150.0f;
            activity.height = 44.0f;
            break;
        case IslandKind::Progress:
            activity.width = 230.0f;
            activity.height = 48.0f;
            break;
        case IslandKind::Clipboard:
            activity.width = 340.0f;
            activity.height = 56.0f;
            break;
        case IslandKind::Notification:
            activity.width = 360.0f;
            activity.height = 58.0f;
            break;
        case IslandKind::Volume:
            activity.width = 300.0f;
            activity.height = 54.0f;
            break;
        case IslandKind::BatteryLow:
            activity.width = 290.0f;
            activity.height = 52.0f;
            break;
        case IslandKind::CapsLock:
            activity.width = 180.0f;
            activity.height = 42.0f;
            break;
        case IslandKind::Device:
            activity.width = 240.0f;
            activity.height = 50.0f;
            break;
        case IslandKind::Idle:
        default:
            if (settings.autoHideIdleSeconds == -1 && !state.system.micActive && !state.system.cameraActive) {
                activity.width = 0.0f;
                activity.height = 0.0f;
            } else {
                activity.width = 170.0f;
                activity.height = 36.0f;
            }
            break;
    }

    activity.width *= settings.sizeScale;
    activity.height *= settings.sizeScale;
    return activity;
}

std::vector<IslandKind> ChooseActivities(const SharedState& state, const Settings& settings, double now) {
    std::vector<IslandKind> activities;

    if (state.clipboard.active && now < state.clipboard.expiresAt) {
        activities.push_back(IslandKind::Clipboard);
    }
    if (state.capsLock.active && now < state.capsLock.expiresAt) {
        activities.push_back(IslandKind::CapsLock);
    }
    if (state.device.active && now < state.device.expiresAt) {
        activities.push_back(IslandKind::Device);
    }
    if (state.volume.active && now < state.volume.expiresAt) {
        activities.push_back(IslandKind::Volume);
    }
    if (state.notification.active && now < state.notification.expiresAt) {
        activities.push_back(IslandKind::Notification);
    }
    if (settings.battery && state.battery.active && now < state.battery.expiresAt) {
        activities.push_back(IslandKind::BatteryLow);
    }
    if (settings.progress && state.progress.active) {
        activities.push_back(IslandKind::Progress);
    }

    if (activities.empty()) {
        activities.push_back(IslandKind::Idle);
    }

    return activities;
}

constexpr UINT WM_APP_CAPSLOCK = WM_APP + 0x444;
HHOOK g_keyboardHook = nullptr;
HANDLE g_keyboardThread = nullptr;
DWORD g_keyboardThreadId = 0;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
            auto* kbd = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
            if (kbd->vkCode == VK_CAPITAL || kbd->vkCode == VK_NUMLOCK) {
                PostMessageW(g_hwnd, WM_APP_CAPSLOCK, kbd->vkCode, 0);
            }
        }
    }
    return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
}

DWORD WINAPI KeyboardThreadProc(void*) {
    g_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0);
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }
    return 0;
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            AddClipboardFormatListener(hwnd);
            RegisterShellHookWindow(hwnd);
            return 0;

        case WM_DESTROY:
            RemoveClipboardFormatListener(hwnd);
            DeregisterShellHookWindow(hwnd);
            return 0;

        case WM_APP_CAPSLOCK: {
            bool isNum = (wParam == VK_NUMLOCK);
            bool capsOn = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
            bool numOn = (GetKeyState(VK_NUMLOCK) & 0x0001) != 0;
            {
                std::lock_guard lock(g_stateMutex);
                g_state.capsLock.active = true;
                g_state.capsLock.capsOn = capsOn;
                g_state.capsLock.numOn = numOn;
                g_state.capsLock.isNumEvent = isNum;
                g_state.capsLock.expiresAt = NowSeconds() + 2.5;
            }
            TriggerNudge();
            return 0;
        }

        case WM_DEVICECHANGE: {
            // DBT_DEVICEARRIVAL = 0x8000, DBT_DEVICEREMOVECOMPLETE = 0x8004
            if (wParam == 0x8000 || wParam == 0x8004) {
                bool arrived = (wParam == 0x8000);
                std::wstring devName;
                bool isBt = false;

                if (lParam) {
                    auto* hdr = reinterpret_cast<DEV_BROADCAST_HDR*>(lParam);
                    if (hdr->dbch_devicetype == DBT_DEVTYP_VOLUME) {
                        devName = L"USB Drive";
                    } else if (hdr->dbch_devicetype == DBT_DEVTYP_PORT) {
                        devName = L"COM Device";
                    } else {
                        // Generic/Bluetooth OEM
                        isBt = true;
                        devName = L"Bluetooth Device";
                    }
                }

                {
                    std::lock_guard lock(g_stateMutex);
                    g_state.device.active = true;
                    g_state.device.eventType = arrived ? DeviceEventType::Connected
                                                       : DeviceEventType::Disconnected;
                    g_state.device.deviceName = devName;
                    g_state.device.isBluetoothLike = isBt;
                    g_state.device.expiresAt = NowSeconds() + 3.0;
                }
                TriggerNudge();
            }
            return 0;
        }

        case WM_CLIPBOARDUPDATE:
            if (g_settings.clipboard) {
                CaptureClipboard(hwnd);
            }
            return 0;



        case WM_APP_LAYOUT_CHANGED:
            g_layoutDirty = true;
            return 0;

        case WM_LBUTTONDOWN:
            {
                int xPos = GET_X_LPARAM(lParam);
                int yPos = GET_Y_LPARAM(lParam);
                
                bool mediaActive = false;
                {
                    std::lock_guard lock(g_stateMutex);
                    mediaActive = g_settings.media && g_state.media.available;
                }

                RECT clientRect;
                GetClientRect(hwnd, &clientRect);
                const float height = static_cast<float>(clientRect.bottom - clientRect.top);
                const float width = static_cast<float>(clientRect.right - clientRect.left);

                const bool gameOn = g_settings.gameOverlay || Wh_GetIntValue(L"GameOverlayPinned", 0) != 0;
                const bool mediaOn = g_settings.media && g_state.media.available;
                const int panelIdx = CurrentPanelIndex(gameOn, mediaOn);
                if (gameOn && height > 45.0f && panelIdx == GamePanelIndex()) {
                    // Game overlay chip click-to-toggle.
                    float totalScale = (GetDpiForWindow(hwnd) / 96.0f) * g_settings.sizeScale;
                    float cx = width / 2.0f;
                    float cy = height / 2.0f;
                    float unX = (xPos - cx) / totalScale;
                    float unY = (yPos - cy) / totalScale;
                    for (int i = 0; i < g_gameOverlayChipCount; ++i) {
                        const auto& chip = g_gameOverlayChips[i];
                        if (unX >= chip.left && unX <= chip.right && unY >= chip.top && unY <= chip.bottom) {
                            ToggleGameOverlayChip(chip.id);
                            return 0;
                        }
                    }
                }
                const int mediaPanelIdx = MediaPanelIndex(gameOn, mediaActive);
                if (mediaActive && height > 60.0f && (g_idleTab % PanelCount(gameOn, mediaActive)) == mediaPanelIdx) {
                    float totalScale = (GetDpiForWindow(hwnd) / 96.0f) * g_settings.sizeScale;
                    float cx = width / 2.0f;
                    float cy = height / 2.0f;
                    
                    float unX = (xPos - cx) / totalScale;
                    float unY = (yPos - cy) / totalScale;

                    if (unY > 56.0f - 30.0f && unY < 56.0f + 30.0f) {
                        int cmd = -1;
                        if (unX > -84.0f && unX < -44.0f) cmd = 0; // Prev
                        else if (unX > -24.0f && unX < 24.0f) cmd = 1; // Play/Pause
                        else if (unX > 44.0f && unX < 84.0f) cmd = 2; // Next

                        if (cmd != -1) {
                            g_pressedMediaButton = cmd;
                            SetCapture(hwnd);
                            g_layoutDirty = true;
                            return 0;
                        }
                    }
                }
            }
            break;

        case WM_LBUTTONUP:
            {
                if (g_pressedMediaButton.load() != -1) {
                    g_pressedMediaButton = -1;
                    ReleaseCapture();
                    g_layoutDirty = true;
                }

                int xPos = GET_X_LPARAM(lParam);
                int yPos = GET_Y_LPARAM(lParam);
                
                bool expanded = Wh_GetIntValue(L"PinnedExpanded", 0) != 0 || g_clickExpanded.load();
                if (!g_settings.expandOnHover && !expanded) {
                    g_clickExpanded = true;
                    g_layoutDirty = true;
                    return 0; // consumed click to expand
                }
                
                bool mediaActive = false;
                std::vector<IslandKind> kinds;
                {
                    std::lock_guard lock(g_stateMutex);
                    mediaActive = g_settings.media && g_state.media.available;
                    kinds = ChooseActivities(g_state, g_settings, NowSeconds());
                }

                RECT clientRect;
                GetClientRect(hwnd, &clientRect);
                const float height = static_cast<float>(clientRect.bottom - clientRect.top);
                const float width = static_cast<float>(clientRect.right - clientRect.left);

                const bool gameOn = g_settings.gameOverlay || Wh_GetIntValue(L"GameOverlayPinned", 0) != 0;
                const int mediaPanelIdx = MediaPanelIndex(gameOn, mediaActive);
                if (mediaActive && height > 60.0f && (g_idleTab % PanelCount(gameOn, mediaActive)) == mediaPanelIdx) {
                    float totalScale = (GetDpiForWindow(hwnd) / 96.0f) * g_settings.sizeScale;
                    float cx = width / 2.0f;
                    float cy = height / 2.0f;
                    
                    float unX = (xPos - cx) / totalScale;
                    float unY = (yPos - cy) / totalScale;

                    if (unY > 56.0f - 30.0f && unY < 56.0f + 30.0f) {
                        // Check button clicks in expanded media view
                        int cmd = -1;
                        if (unX > -84.0f && unX < -44.0f) cmd = 0; // Prev
                        else if (unX > -24.0f && unX < 24.0f) cmd = 1; // Play/Pause
                        else if (unX > 44.0f && unX < 84.0f) cmd = 2; // Next

                        if (cmd != -1) {
                            std::thread([cmd]() {
                                winrt::init_apartment(winrt::apartment_type::multi_threaded);
                                try {
                                    using Manager = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager;
                                    auto manager = Manager::RequestAsync().get();
                                    if (manager) {
                                        auto sessions = manager.GetSessions();
                                        std::wstring currentAumid;
                                        {
                                            std::lock_guard lock(g_stateMutex);
                                            currentAumid = g_state.media.sourceAppUserModelId;
                                        }
                                        winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession session = nullptr;
                                        for (auto const& s : sessions) {
                                            if (s.SourceAppUserModelId().c_str() == currentAumid) {
                                                session = s;
                                                break;
                                            }
                                        }
                                        if (!session) session = manager.GetCurrentSession();

                                        if (session) {
                                            if (cmd == 0) session.TrySkipPreviousAsync().get();
                                            else if (cmd == 1) session.TryTogglePlayPauseAsync().get();
                                            else if (cmd == 2) session.TrySkipNextAsync().get();
                                        }
                                    }
                                } catch (...) {}
                            }).detach();
                            return 0;
                        }
                    }
                }

                const int panel = CurrentPanelIndex(gameOn, mediaActive);
                if (mediaActive && panel == mediaPanelIdx) {
                    if (height > 45.0f && xPos > width - 30.0f) {
                        // Clicked on the right edge scroll area in Media
                        g_idleTab = (g_idleTab + 1) % PanelCount(gameOn, true);
                        g_layoutDirty = true;
                    } else {
                        OpenRelevantApp();
                    }
                } else if (!kinds.empty() && kinds[0] == IslandKind::Idle && height > 45.0f) {
                    if (panel == RamPanelIndex(gameOn, mediaActive)) {
                        // RAM Cleaner panel: interactive Clean button (divide client px by DPI to match the D2D surface).
                        const float dpiCfg = GetDpiForWindow(hwnd) / 96.0f;
                        const float xd = xPos / dpiCfg;
                        const float yd = yPos / dpiCfg;
                        const float wd = width / dpiCfg;
                        const float hd = height / dpiCfg;
                        D2D1_RECT_F btn = D2D1::RectF(wd - 118.0f, 40.0f, wd - 30.0f, hd - 40.0f);
                        if (xd >= btn.left && xd <= btn.right && yd >= btn.top && yd <= btn.bottom) {
                            RamCleanNow();
                            return 0;
                        }
                    }
                    const int idleTabCount = PanelCount(gameOn, mediaActive);
                    if (panel == GameOptionsPanelIndex()) {
                        // Game overlay options panel: interactive toggles (divide client px by DPI to match D2D surface).
                        const float dpiCfg = GetDpiForWindow(hwnd) / 96.0f;
                        const int opt = GameOptionsActionAtPoint(xPos / dpiCfg, yPos / dpiCfg,
                                                                 width / dpiCfg, height / dpiCfg);
                        if (opt != GameOptionNone) {
                            ApplyGameOptionsAction(opt);
                            return 0;
                        }
                    }
                    if (panel == ConfigPanelIndex(gameOn, mediaActive)) {
                        // Config panel: interactive buttons (divide client px by DPI to match the D2D surface).
                        const float dpiCfg = GetDpiForWindow(hwnd) / 96.0f;
                        DiLog(L"ConfigClick debug: x=%d y=%d w=%d h=%d dpi=%.3f", xPos, yPos,
                              (int)width, (int)height, dpiCfg);
                        const int cfg = ConfigActionAtPoint(xPos / dpiCfg, yPos / dpiCfg,
                                                           width / dpiCfg, height / dpiCfg);
                        DiLog(L"ConfigClick debug: xd=%.2f yd=%.2f wd=%.2f hd=%.2f cfg=%d",
                              xPos / dpiCfg, yPos / dpiCfg, width / dpiCfg, height / dpiCfg, cfg);
                        if (cfg != ConfigActionNone) {
                            ApplyConfigAction(cfg);
                            return 0;
                        }
                    }
                    if (xPos < width / 2.0f) g_idleTab = (g_idleTab - 1 + idleTabCount) % idleTabCount;
                    else g_idleTab = (g_idleTab + 1) % idleTabCount;
                    g_layoutDirty = true;
                } else {
                    HandleStatusClickAtPoint(hwnd, lParam);
                }
            }
            return 0;

        case WM_MBUTTONUP:
            ToggleEndpointMute();
            return 0;

        case WM_LBUTTONDBLCLK:
            Wh_SetIntValue(L"PinnedExpanded", Wh_GetIntValue(L"PinnedExpanded", 0) ? 0 : 1);
            return 0;

        case WM_MOUSEWHEEL: {
            static ULONGLONG lastScrollTime = 0;
            ULONGLONG now = GetTickCount64();
            if (now - lastScrollTime < 150) return 0; // 150ms debounce
            lastScrollTime = now;

            bool mediaActive = false;
            {
                std::lock_guard lock(g_stateMutex);
                mediaActive = g_settings.media && g_state.media.available;
            }
            const bool gameOn = g_settings.gameOverlay || Wh_GetIntValue(L"GameOverlayPinned", 0) != 0;
            int tabCount = PanelCount(gameOn, mediaActive);
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (delta > 0) {
                if (g_idleTab > 0) g_idleTab--;
            } else if (delta < 0) {
                if (g_idleTab < tabCount - 1) g_idleTab++;
            }
            
            g_layoutDirty = true;
            return 0;
        }

        case WM_RBUTTONUP: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ClientToScreen(hwnd, &pt);
            ShowContextMenu(hwnd, pt);
            return 0;
        }
    }

    if (msg == g_shellHookMessage) {
        if (wParam == HSHELL_WINDOWCREATED) {
            CaptureShellNotification(reinterpret_cast<HWND>(lParam));
        }
        return 0;
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

    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
        kWindowClass, L"Dynamic Island for Windows", WS_POPUP, 0, 0, 520, 140,
        nullptr, nullptr, wc.hInstance, nullptr);

    if (!hwnd) {
        DiLog(L"RenderThread: CreateWindowEx failed %lu", GetLastError());
        Wh_Log(L"Failed to create Dynamic Island overlay window.");
        if (SUCCEEDED(hrCo)) {
            CoUninitialize();
        }
        return 0;
    }
    DiLog(L"RenderThread: hwnd=0x%p", hwnd);

    g_hwnd = hwnd;
    g_shellHookMessage = RegisterWindowMessageW(L"SHELLHOOK");
    EnableBlurBehind(hwnd);
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);

    Renderer renderer;
    if (!renderer.Initialize(hwnd)) {
        DestroyWindow(hwnd);
        g_hwnd = nullptr;
        if (SUCCEEDED(hrCo)) {
            CoUninitialize();
        }
        return 0;
    }

    StartEtwFpsSession();

    SpringValue widthSpring;
    SpringValue heightSpring;
    SpringValue nudgeSpring;
    widthSpring.Reset((g_settings.autoHideIdleSeconds == -1 ? 0.0f : 120.0f) * g_settings.sizeScale);
    heightSpring.Reset((g_settings.autoHideIdleSeconds == -1 ? 0.0f : 36.0f) * g_settings.sizeScale);
    nudgeSpring.Reset(0.0f);

    IslandKind previousPrimary = IslandKind::Idle;
    auto previousFrame = std::chrono::steady_clock::now();
    double nextBatteryPoll = 0.0;
    double nextProgressPoll = 0.0;
    double nextSystemPoll = 0.0;
    double nextPrivacyPoll = 0.0;
    static double nextGpuTempPoll = 0.0;

    while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        MSG message = {};
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            if (message.message == WM_APP_NEW_EVENT) {
                nudgeSpring.value = -6.0f;
                nudgeSpring.velocity = 0.0f;
                nudgeSpring.target = 0.0f;
                continue;
            }
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        const double now = NowSeconds();
        static int s_dbgTick = 0;
        if (s_dbgTick < 120) {
            DiLog(L"tick s=%d b=%d pr=%d sy=%d gt=%d pv=%d", s_dbgTick++,
                  now >= nextBatteryPoll, now >= nextProgressPoll,
                  now >= nextSystemPoll, now >= nextGpuTempPoll,
                  now >= nextPrivacyPoll);
        }
        if (now >= nextBatteryPoll) {
            UpdateBatterySnapshot();
            nextBatteryPoll = now + 15.0;
        }
        if (now >= nextProgressPoll) {
            UpdateProgressSnapshot();
            nextProgressPoll = now + 0.25;
        }
        if (now >= nextSystemPoll) {
            UpdateSystemSnapshot();
            nextSystemPoll = now + 1.0;
        }
        if (now >= nextGpuTempPoll) {
            const int gpuTemp = GetGpuTemperatureCelsius();
            g_gpuTempCelsius = gpuTemp;
            nextGpuTempPoll = now + std::max(0.2, g_settings.gpuTempUpdateMs / 1000.0);
        }
        if (now >= nextPrivacyPoll) {
            UpdatePrivacyIndicators();
            nextPrivacyPoll = now + 2.0;  // poll every 2 s
        }

        SharedState snapshot;
        {
            std::lock_guard lock(g_stateMutex);
            snapshot = g_state;
            if (g_state.clipboard.active && now >= g_state.clipboard.expiresAt) {
                g_state.clipboard.active = false;
                snapshot.clipboard.active = false;
            }
            if (g_state.notification.active && now >= g_state.notification.expiresAt) {
                g_state.notification.active = false;
                snapshot.notification.active = false;
            }
            if (g_state.volume.active && now >= g_state.volume.expiresAt) {
                g_state.volume.active = false;
                snapshot.volume.active = false;
            }
            if (g_state.capsLock.active && now >= g_state.capsLock.expiresAt) {
                g_state.capsLock.active = false;
                snapshot.capsLock.active = false;
            }
            if (g_state.battery.active && now >= g_state.battery.expiresAt) {
                g_state.battery.active = false;
                snapshot.battery.active = false;
            }
            if (g_state.device.active && now >= g_state.device.expiresAt) {
                g_state.device.active = false;
                snapshot.device.active = false;
            }
        }

        const std::vector<IslandKind> kinds = ChooseActivities(snapshot, g_settings, now);
        Activity primary = ActivityForKind(kinds[0], g_settings, snapshot);
        std::optional<Activity> secondary;
        if (kinds.size() >= 2) {
            secondary = ActivityForKind(kinds[1], g_settings, snapshot);
        }

        const bool pinned = Wh_GetIntValue(L"PinnedExpanded", 0) != 0;

        if (primary.kind != previousPrimary && primary.kind != IslandKind::Idle) {
            nudgeSpring.value = -6.0f;
            nudgeSpring.velocity = 0.0f;
            nudgeSpring.target = 0.0f;
        }
        previousPrimary = primary.kind;

        RECT windowRect = {};
        GetWindowRect(hwnd, &windowRect);
        POINT cursor = {};
        GetCursorPos(&cursor);
        const bool hover = PtInRect(&windowRect, cursor) != FALSE;
        
        bool needsRender = false;
        
        if (!hover && g_clickExpanded.load()) {
            g_clickExpanded = false;
            needsRender = true;
        }
        static double lastInteractionTime = NowSeconds();
        bool currentlyHidden = false;
        if (g_settings.autoHideIdleSeconds == -1) {
            currentlyHidden = true;
        } else if (g_settings.autoHideIdleSeconds > 0) {
            currentlyHidden = (now - lastInteractionTime > g_settings.autoHideIdleSeconds);
        }

        bool isHoverExpanded = g_settings.expandOnHover ? hover : (hover && g_clickExpanded.load());
        
        if (currentlyHidden && !g_settings.unhideOnHover && primary.kind == IslandKind::Idle) {
            isHoverExpanded = false;
        } else if (isHoverExpanded || pinned || primary.kind != IslandKind::Idle) {
            lastInteractionTime = now;
        }
        
        bool isHidden = false;
        if (g_settings.autoHideIdleSeconds == -1) {
            isHidden = true;
        } else if (g_settings.autoHideIdleSeconds > 0) {
            isHidden = (now - lastInteractionTime > g_settings.autoHideIdleSeconds);
        }

        bool privacyActive = snapshot.system.micActive || snapshot.system.cameraActive;
        if (primary.kind == IslandKind::Idle) {
            const bool gameOn = g_settings.gameOverlay || Wh_GetIntValue(L"GameOverlayPinned", 0) != 0;
            const bool mediaOn = g_settings.media && snapshot.media.available;
            const int tabIdx = CurrentPanelIndex(gameOn, mediaOn);
            const bool activeGameTab = gameOn && tabIdx == GamePanelIndex();
            if (activeGameTab) {
                primary.width = GameOverlayContentWidth() * g_settings.sizeScale;
                primary.height = 64.0f * g_settings.sizeScale;
            } else if (pinned || isHoverExpanded) {
                primary.width = 380.0f * g_settings.sizeScale;
                primary.height = 184.0f * g_settings.sizeScale;
            } else if (isHidden && !privacyActive) {
                primary.width = 0.0f;
                primary.height = 0.0f;
            }
        }
        if (g_settings.hideIsland && primary.kind == IslandKind::Idle && !pinned) {
            primary.width = 6.0f * g_settings.sizeScale;
            primary.height = 6.0f * g_settings.sizeScale;
        }
        if (primary.kind == IslandKind::Media) {
            bool recentArtChange = (NowSeconds() - g_state.media.artChangedAt) < 4.0;
            if (isHoverExpanded || pinned || recentArtChange) {
                primary.width = 380.0f * g_settings.sizeScale;
                primary.height = 184.0f * g_settings.sizeScale;
            }
        }

        float targetWidth = primary.width;
        float targetHeight = primary.height;
        if (secondary) {
            targetWidth = primary.width + secondary->width + 12.0f * g_settings.sizeScale;
            targetHeight = std::max(primary.height, secondary->height);
        }
        if (pinned) {
            targetWidth = std::max(targetWidth, 380.0f * g_settings.sizeScale);
            targetHeight = std::max(targetHeight, 64.0f * g_settings.sizeScale);
        }

        widthSpring.target = targetWidth;
        heightSpring.target = targetHeight;

        const auto currentFrame = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float>(currentFrame - previousFrame).count();
        previousFrame = currentFrame;
        dt = Clamp(dt, 0.001f, 0.050f);

        const float speed = g_settings.animationSpeed;
        float widthStiffness = 280.0f;
        float widthDamping = 24.0f;
        if (targetWidth > widthSpring.value) {
            widthStiffness = 380.0f;
            widthDamping = 26.0f;
        } else if (targetWidth < widthSpring.value) {
            widthStiffness = 200.0f;
            widthDamping = 28.0f;
        }

        float heightStiffness = 280.0f;
        float heightDamping = 24.0f;
        if (targetHeight > heightSpring.value) {
            heightStiffness = 380.0f;
            heightDamping = 26.0f;
        } else if (targetHeight < heightSpring.value) {
            heightStiffness = 200.0f;
            heightDamping = 28.0f;
        }

        widthSpring.Step(dt * speed, widthStiffness, widthDamping);
        if (widthSpring.value < 0.0f) {
            widthSpring.value = 0.0f;
            widthSpring.velocity = 0.0f;
        }

        heightSpring.Step(dt * speed, heightStiffness, heightDamping);
        if (heightSpring.value < 0.0f) {
            heightSpring.value = 0.0f;
            heightSpring.velocity = 0.0f;
        }

        nudgeSpring.Step(dt * speed, 280.0f, 24.0f);

        // Track the foreground process the FPS counter should follow
        // (exclude our own island window).
        {
            static DWORD s_trackedPid = 0;
            DWORD fgPid = 0;
            HWND fg = GetForegroundWindow();
            if (fg && fg != hwnd) {
                GetWindowThreadProcessId(fg, &fgPid);
            }
            if (fgPid != s_trackedPid) {
                s_trackedPid = fgPid;
                g_fpsTargetPid.store(fgPid, std::memory_order_relaxed);
                g_gameFps.store(-1, std::memory_order_relaxed);
            }
        }

        {
            std::lock_guard lock(g_stateMutex);
            const int fps = g_gameFps.load(std::memory_order_relaxed);
            g_state.system.gameFps = std::max(0, fps);
            g_state.system.gameFpsValid =
                g_etwReady.load() && g_fpsTargetPid.load() != 0 && fps >= 0;
        }

        SetClickThrough(hwnd, primary.kind == IslandKind::Idle && !hover && !pinned);

        // Check if animating structurally
        if (std::abs(widthSpring.velocity) > 0.01f || std::abs(widthSpring.target - widthSpring.value) > 0.01f ||
            std::abs(heightSpring.velocity) > 0.01f || std::abs(heightSpring.target - heightSpring.value) > 0.01f ||
            std::abs(nudgeSpring.velocity) > 0.01f || std::abs(nudgeSpring.target - nudgeSpring.value) > 0.01f) {
            needsRender = true;
        }

        // Active Monitor Tracking (Follow Mouse)
        if (g_settings.targetMonitor == -1) {
            static HMONITOR s_lastMonitor = nullptr;
            POINT pt;
            GetCursorPos(&pt);
            HMONITOR currentMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
            if (currentMonitor != s_lastMonitor) {
                s_lastMonitor = currentMonitor;
                g_layoutDirty = true;
            }
        }

        // Check if layout was explicitly invalidated
        if (g_layoutDirty.load()) {
            needsRender = true;
        }

        // Hover or pinned state changes visual elements slightly
        static bool prevHover = false;
        static bool prevPinned = false;
        if (hover != prevHover || pinned != prevPinned) {
            needsRender = true;
            prevHover = hover;
            prevPinned = pinned;
        }

        // Animated activities that require continuous rendering
        if (primary.kind == IslandKind::Media || primary.kind == IslandKind::BatteryLow ||
            primary.kind == IslandKind::Clipboard || primary.kind == IslandKind::Notification) {
            needsRender = true;
        }

        // Privacy dots
        if (snapshot.system.micActive || snapshot.system.cameraActive) {
            needsRender = true;
        }
        
        // Idle dashboard clock changes once a minute
        static SYSTEMTIME prevTime = {};
        if (primary.kind == IslandKind::Idle && !isHidden) {
            SYSTEMTIME local = {};
            GetLocalTime(&local);
            if (local.wMinute != prevTime.wMinute) {
                needsRender = true;
                prevTime = local;
            }
        }

        // Compare data snapshot to detect changes
        static uint64_t prevArtGen = 0;
        static uint64_t prevSrcIconGen = 0;
        static uint64_t prevNotifIconGen = 0;
        static uint64_t prevClipIconGen = 0;
        static int prevCpu = -1;
        static int prevRam = -1;
        static int prevDisk = -1;
        static int prevVol = -1;
        static bool prevMuted = false;
        static int prevBat = -1;
        static bool prevCharging = false;
        static int prevProg = -1;
        static int prevGpuTemp = -1;
        static int prevGameFps = -1;
        static std::wstring prevMediaTitle;
        
        if (snapshot.media.artGeneration != prevArtGen ||
            snapshot.media.sourceIconGeneration != prevSrcIconGen ||
            snapshot.media.title != prevMediaTitle ||
            snapshot.notification.icon.generation != prevNotifIconGen ||
            snapshot.clipboard.appIcon.generation != prevClipIconGen ||
            g_gpuTempCelsius.load() != prevGpuTemp ||
            g_gameFps.load() != prevGameFps ||
            snapshot.system.cpuPercent != prevCpu ||
            snapshot.system.memoryPercent != prevRam ||
            snapshot.system.diskFreePercent != prevDisk ||
            snapshot.system.volumePercent != prevVol ||
            snapshot.system.volumeMuted != prevMuted ||
            snapshot.battery.percent != prevBat ||
            snapshot.battery.charging != prevCharging ||
            snapshot.progress.percent != prevProg) {
            needsRender = true;
            prevArtGen = snapshot.media.artGeneration;
            prevSrcIconGen = snapshot.media.sourceIconGeneration;
            prevMediaTitle = snapshot.media.title;
            prevNotifIconGen = snapshot.notification.icon.generation;
            prevClipIconGen = snapshot.clipboard.appIcon.generation;
            prevGpuTemp = g_gpuTempCelsius.load();
            prevGameFps = g_gameFps.load();
            prevCpu = snapshot.system.cpuPercent;
            prevRam = snapshot.system.memoryPercent;
            prevDisk = snapshot.system.diskFreePercent;
            prevVol = snapshot.system.volumePercent;
            prevMuted = snapshot.system.volumeMuted;
            prevBat = snapshot.battery.percent;
            prevCharging = snapshot.battery.charging;
            prevProg = snapshot.progress.percent;
        }

        if (needsRender) {
            renderer.Render(snapshot, g_settings, primary, secondary,
                            widthSpring.value, heightSpring.value, nudgeSpring.value,
                            hover, pinned, now);
        }

        WaitForSingleObject(g_stopEvent, 16);
    }

    StopEtwFpsSession();
    renderer.Shutdown();
    DestroyWindow(hwnd);
    g_hwnd = nullptr;
    UnregisterClassW(kWindowClass, wc.hInstance);

    if (SUCCEEDED(hrCo)) {
        CoUninitialize();
    }

    return 0;
}



bool StartThreads() {
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_settingsChangedEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEvent || !g_settingsChangedEvent) {
        return false;
    }

    g_running = true;
    g_renderThread = CreateThread(nullptr, 0, RenderThreadProc, nullptr, 0, nullptr);
    if (!g_renderThread) {
        return false;
    }

    g_mediaThread = CreateThread(nullptr, 0, MediaThreadProc, nullptr, 0, nullptr);
    g_audioThread = CreateThread(nullptr, 0, AudioThreadProc, nullptr, 0, nullptr);
    g_keyboardThread = CreateThread(nullptr, 0, KeyboardThreadProc, nullptr, 0, &g_keyboardThreadId);
#if DYNAMIC_ISLAND_HAS_USER_NOTIFICATION_LISTENER
    g_notificationThread = CreateThread(nullptr, 0, NotificationThreadProc, nullptr, 0, nullptr);
#endif

    return true;
}

void StopThreads() {
    if (g_keyboardThreadId != 0) {
        PostThreadMessageW(g_keyboardThreadId, WM_QUIT, 0, 0);
    }
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    HANDLE handles[] = {g_renderThread, g_mediaThread, g_audioThread, g_notificationThread, g_keyboardThread};
    for (HANDLE handle : handles) {
        if (handle) {
            WaitForSingleObject(handle, 3000);
            CloseHandle(handle);
        }
    }

    g_renderThread = nullptr;
    g_mediaThread = nullptr;
    g_audioThread = nullptr;
    g_notificationThread = nullptr;
    g_keyboardThread = nullptr;
    g_keyboardThreadId = 0;

    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    if (g_settingsChangedEvent) {
        CloseHandle(g_settingsChangedEvent);
        g_settingsChangedEvent = nullptr;
    }

    g_running = false;
}



}  // namespace

BOOL WhTool_ModInit() {
    DiLog(L"WhTool_ModInit");
    LoadSettings();

    if (!StartThreads()) {
        DiLog(L"StartThreads failed");
        StopThreads();
        return FALSE;
    }

    g_layoutDirty = true;
    Wh_Log(L"Dynamic Island for Windows initialized.");
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

void WhTool_ModUninit() {
    if (g_hwnd) {
        PostMessageW(g_hwnd, WM_CLOSE, 0, 0);
    }
    StopThreads();
    Wh_Log(L"Dynamic Island for Windows unloaded.");
}

//////////////////////////////////////////////////////////////////////////////////
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
    DWORD sessionId;
    ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);
    DiLog(L"Wh_ModInit pid=%u sessionId=%u cmdline=%ls", GetCurrentProcessId(),
          sessionId, GetCommandLineW());
    if (sessionId == 0) {
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
    DiLog(L"Wh_ModInit -> launcher");
    return TRUE;
}

void Wh_ModAfterInit() {
    DiLog(L"Wh_ModAfterInit launcher=%d", g_isToolModProcessLauncher ? 1 : 0);
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            DiLog(L"GetModuleFileName failed %lu", GetLastError());
            Wh_Log(L"GetModuleFileName failed");
            return;
    }
    DiLog(L"Wh_ModAfterInit path=%ls", currentProcessPath);

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
        DiLog(L"No CreateProcessInternalW");
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
        DiLog(L"CreateProcess failed %lu cmd=%ls", GetLastError(), commandLine);
        Wh_Log(L"CreateProcess failed");
        return;
    }
    DiLog(L"spawned tool proc pid=%u", pi.dwProcessId);

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
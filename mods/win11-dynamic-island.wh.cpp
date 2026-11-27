// ==WindhawkMod==
// @id              win11-dynamic-island
// @name            Dynamic Island for Windows 11
// @description     An interactive Dynamic Island overlay and taskbar companion for Windows 11
// @version         0.1.30
// @author          Tederby and Qhaton
// @github          https://github.com/qhaton
// @include         explorer.exe
// @compilerOptions -ld2d1 -ldwrite -lwindowscodecs -luxtheme -lole32 -loleaut32 -lruntimeobject -lwindowsapp -lshcore -lversion -lgdi32 -ldwmapi -luser32 -lshell32 -ladvapi32 -lsetupapi -lwinhttp
// @license         MIT
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Dynamic Island for Windows 11
An interactive, fluid Dynamic Island overlay and taskbar widget for Windows 11,
bringing smooth Apple-inspired micro-interactions and live status pills to the desktop.
## Key Features
- **Adaptive Taskbar Placement**:
- **Top Taskbar**: Seamlessly embeds into the taskbar and expands downward.
- **Bottom / Left / Right Taskbar**: Floats gracefully as an independent pill with fluid spring animations.
- **Live Activities (Persistent & Expandable)**:
- **Now Playing**: Album artwork, marquee track/artist info, animated equalizer wave, expanded playback controls, and **interactive progress bar for seeking**.
- **Focus Timer**: Circular progress indicator, MM:SS countdown, and interactive pause/resume/stop controls.
- **Microphone & Privacy Dots**: Active indicator showing when mic or recording streams are in use.
- **Transient HUD Alerts (Auto-dismissing)**:
- Volume slider and percentage indicator.
- Caps Lock / Num Lock ON/OFF notifications.
- Power & Charging status (plugged in, charging %, low battery warning).
- Bluetooth device connection and peripheral battery level.
- **Fluid Spring Physics**:
- Smooth expansion and morphing animations modeled after modern UI curves (`cubic-bezier(0.34, 1.3, 0.5, 1)`).
- **Lightweight & High Performance**:
- Hardware-accelerated Direct2D and DirectWrite rendering.
## Inspiration
Inspired by [devcode90/Dynamic-Island-for-Windows](https://github.com/devcode90/Dynamic-Island-for-Windows)
and expanded to cover full multi-edge taskbars, rich interactive states, and an extensible service architecture.
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- placementMode: auto
  $name: Placement Mode
  $description: Choose how the Dynamic Island positions itself.
  $options:
    - auto: Automatically detect taskbar position (embedded on top, floating on bottom/sides)
    - top_embed: Always embed directly into top edge
    - floating: Always float as an overlay window
- idleVisibilityMode: always_visible
  $name: Idle Visibility Mode
  $description: Choose island appearance when no transient HUD or live activity is active.
  $options:
    - always_visible: Compact pill remains visible on taskbar during idle state
    - events_only: Island remains hidden when idle, appearing only during activities or alerts
- mediaVisibilityPolicy: always_visible
  $name: Media Playback Visibility Policy
  $description: Determine how media playback displays on the island.
  $options:
    - always_visible: Island stays visible while media is actively playing
    - track_change_only: Island pops up transiently for 3-4 seconds on track change, then collapses back to hidden
- enableMedia: true
  $name: Media Controls & Now Playing
  $description: Show active media playback, track marquee, equalizer, and playback controls.
- enableOnlineArtworkLookup: false
  $name: Online Album Artwork Lookup
  $description: Allow sending track title and artist to Apple iTunes Search when local artwork is unavailable.
- enableTimer: true
  $name: Focus Timer
  $description: Enable focus session countdown timer with progress ring.
- enableMicStatus: true
  $name: Microphone Privacy Dot
  $description: Show indicator when an application is accessing the microphone.
- enableVolumeHUD: true
  $name: Volume Changes HUD
  $description: Show compact island badge when master volume changes.
- enableCapsLockHUD: true
  $name: Caps Lock / Num Lock HUD
  $description: Show badge when Caps Lock or Num Lock is toggled ON or OFF.
- enablePowerHUD: true
  $name: Power & Battery Alerts
  $description: Show alerts when charger is plugged/unplugged or battery drops below 20%.
- enableBluetoothHUD: true
  $name: Bluetooth HUD
  $description: Show badge when Bluetooth headphones/devices connect with battery status.
- autoCollapseDelayMs: 1500
  $name: Auto-collapse Delay (ms)
  $description: Milliseconds of inactivity after cursor leaves before an expanded island collapses back to compact view (1500ms = 1.5s).
- enableDebugHotkeys: true
  $name: Enable Debug Hotkeys
  $description: Enable global hotkeys (Ctrl+Win+1..9, 0) to simulate volume, battery, media, and other HUD events.
- fontFamily: Segoe UI Variable Text
  $name: Font Family
  $description: Custom font family name (e.g. "Inter", "Segoe UI Variable Text", "Segoe UI"). "Segoe UI Variable Text" is the Windows 11 default with proportional digits; if the named font is not installed, the mod falls back to Segoe UI Variable / Segoe UI automatically.
- fontWeightRegular: medium
  $name: Body Font Weight
  $description: Font weight used by normal body text and secondary labels.
  $options:
    - thin: Thin (100) — very light, only for large display text
    - light: Light (300) — airy and elegant
    - regular: Regular (400) — standard book weight
    - medium: Medium (500) — recommended modern UI weight, balances readability and elegance
    - semibold: Semi Bold (600) — stronger emphasis
    - bold: Bold (700) — heaviest, maximum emphasis
- fontWeightBold: bold
  $name: Emphasis Font Weight
  $description: Font weight used by emphasized text, including media track titles and highlighted labels.
  $options:
    - thin: Thin (100)
    - light: Light (300)
    - regular: Regular (400)
    - medium: Medium (500)
    - semibold: Semi Bold (600)
    - bold: Bold (700)
- fontSizeRegular: 12
  $name: Body Font Size (pt)
  $description: Font size for regular text in the island (compact pill text, status labels).
- fontSizeBig: 32
  $name: Big Font Size (pt)
  $description: Font size for the large timer digits shown in the expanded timer view.
- fontSizeSmall: 11
  $name: Small Font Size (pt)
  $description: Font size for secondary text (artist names, subtitles, session info).
*/
// ==/WindhawkModSettings==
// ============================================================================
// System Includes
// ============================================================================
#include <windows.h>
#include <algorithm>
#include <condition_variable>
#include <chrono>
#include <cstring>
#include <exception>
#include <string>
#include <memory>
#include <utility>
#include <vector>
#include <cwchar>
#include <cmath>
#include <d2d1.h>
#include <wrl/client.h>
#include <dwrite.h>
#include <dwrite_2.h>
#include <deque>
#include <mutex>
#include <atomic>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <windowsx.h>
#include <wincodec.h>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <initguid.h>
#include <setupapi.h>
#include <devpkey.h>
#include <winhttp.h>

// Move-only RAII wrapper for Win32 kernel handles.
class UniqueWinHandle {
public:
    UniqueWinHandle() noexcept = default;
    explicit UniqueWinHandle(HANDLE value) noexcept : m_value(value) {}
    ~UniqueWinHandle() { reset(); }
    UniqueWinHandle(const UniqueWinHandle&) = delete;
    UniqueWinHandle& operator=(const UniqueWinHandle&) = delete;
    UniqueWinHandle(UniqueWinHandle&& other) noexcept : m_value(other.release()) {}
    UniqueWinHandle& operator=(UniqueWinHandle&& other) noexcept {
        if (this != &other) reset(other.release());
        return *this;
    }
    UniqueWinHandle& operator=(HANDLE value) noexcept { reset(value); return *this; }
    operator HANDLE() const noexcept { return m_value; }
    explicit operator bool() const noexcept { return m_value && m_value != INVALID_HANDLE_VALUE; }
    HANDLE get() const noexcept { return m_value; }
    HANDLE release() noexcept { HANDLE value = m_value; m_value = nullptr; return value; }
    void reset(HANDLE value = nullptr) noexcept {
        if (m_value && m_value != INVALID_HANDLE_VALUE) CloseHandle(m_value);
        m_value = value;
    }
private:
    HANDLE m_value = nullptr;
};

// Windows DEVPKEY_Device_BatteryLevel property key.
DEFINE_DEVPROPKEY(DEVPKEY_Device_BatteryLevel,
    0x104EA319, 0x6EE2, 0x4701, 0xBD, 0x47, 0x8D, 0xDB, 0xF4, 0x25, 0xBB, 0xE5, 2);

// ============================================================================
// [Module] src\common\defs.h
// ============================================================================
#ifndef WH_MOD
inline void Wh_Log(const wchar_t* format, ...) { (void)format; }
inline const wchar_t* Wh_GetStringSetting(const wchar_t* name, ...) { (void)name; return nullptr; }
inline void Wh_FreeStringSetting(const wchar_t* string) { (void)string; }
inline int Wh_GetIntSetting(const wchar_t* name, ...) { (void)name; return 0; }
#endif

namespace DynamicIsland {

enum class TaskbarPosition {
    Top,
    Bottom,
    Left,
    Right,
    Unknown
};

enum class PlacementMode {
    Auto,
    TopEmbed,
    Floating
};

enum class IdleVisibilityMode {
    AlwaysVisible,
    EventsOnly
};

enum class MediaVisibilityPolicy {
    AlwaysVisible,
    TrackChangeOnly
};

enum class FontWeightPreset {
    Thin,
    Light,
    Regular,
    Medium,
    SemiBold,
    Bold
};

enum class IslandState {
    Hidden,
    Compact,
    Expanded
};

enum class EventType {
    None,
    Media,
    Timer,
    MicStatus,
    Volume,
    CapsLock,
    Power,
    Bluetooth,
    LowBattery,
    TimerDone
};

constexpr DWORD DURATION_VOLUME_MS     = 1800;
constexpr DWORD DURATION_CAPS_LOCK_MS  = 1500;
constexpr DWORD DURATION_POWER_MS      = 2400;
constexpr DWORD DURATION_BLUETOOTH_MS  = 2600;
constexpr DWORD DURATION_LOW_BATT_MS   = 2800;
constexpr DWORD DURATION_TIMER_DONE_MS = 3200;

struct MediaThumbnail {
    uint32_t width = 0;
    uint32_t height = 0;
    std::vector<uint8_t> pixels;
    uint64_t version = 0;
};

struct MediaState {
    std::wstring title;
    std::wstring artist;
    float progress = 0.0f;
    int elapsedSeconds = 0;
    int durationSeconds = 0;
    bool isPlaying = false;
};

struct TimerState {
    int remainingSeconds = 0;
    int totalSeconds = 0;
    bool isPaused = false;
    std::wstring label = L"Focus";
    std::wstring session = L"Session 1 of 4";
};

struct MicState {
    std::wstring title = L"Microphone in use";
    std::wstring appName;
    bool isActive = false;
};

struct TransientState {
    EventType type = EventType::None;
    std::wstring title;
    std::wstring subtitle;
    int value = 0;
    float progressFraction = 0.0f;
};

struct ModSettings {
    PlacementMode placement = PlacementMode::Auto;
    IdleVisibilityMode idleVisibilityMode = IdleVisibilityMode::AlwaysVisible;
    MediaVisibilityPolicy mediaVisibilityPolicy = MediaVisibilityPolicy::AlwaysVisible;
    bool enableMedia = true;
    bool enableOnlineArtworkLookup = false;
    bool enableTimer = true;
    bool enableMicStatus = true;
    bool enableVolumeHUD = true;
    bool enableCapsLockHUD = true;
    bool enablePowerHUD = true;
    bool enableBluetoothHUD = true;
    bool enableDebugHotkeys = true;
    int autoCollapseDelayMs = 1500;
    std::wstring fontFamily = L"Segoe UI Variable Text";
    FontWeightPreset fontWeightRegular = FontWeightPreset::Medium;
    FontWeightPreset fontWeightBold = FontWeightPreset::Bold;
    int fontSizeRegular = 12;
    int fontSizeBig = 32;
    int fontSizeSmall = 11;
};

extern ModSettings g_settings;
extern std::mutex g_settingsMutex;

inline ModSettings GetSettingsSnapshot() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

namespace Colors {
    inline constexpr COLORREF BACKGROUND_DARK       = RGB(11, 11, 13);
    inline constexpr COLORREF EMBEDDED_DARK         = RGB(30, 31, 36);
    inline constexpr COLORREF ACCENT_BLUE           = RGB(47, 107, 255);
    inline constexpr COLORREF ACCENT_GREEN          = RGB(48, 209, 88);
    inline constexpr COLORREF ACCENT_ORANGE         = RGB(255, 159, 10);
    inline constexpr COLORREF ACCENT_RED            = RGB(255, 69, 58);
    inline constexpr COLORREF TEXT_PRIMARY          = RGB(255, 255, 255);
    inline constexpr COLORREF TEXT_MUTED            = RGB(168, 171, 181);
    inline constexpr COLORREF BORDER_COLOR          = RGB(44, 47, 55);
}
} // namespace DynamicIsland

// ============================================================================
// [Module] src\common\log.h
// ============================================================================
namespace DynamicIsland {

inline void LogInfo(const wchar_t* format, ...) {
    wchar_t buffer[1024];
    va_list args;
    va_start(args, format);
    _vsnwprintf_s(buffer, sizeof(buffer) / sizeof(wchar_t), _TRUNCATE, format, args);
    va_end(args);
    Wh_Log(L"[DynamicIsland] %s", buffer);
}

inline void LogError(const wchar_t* format, ...) {
    wchar_t buffer[1024];
    va_list args;
    va_start(args, format);
    _vsnwprintf_s(buffer, sizeof(buffer) / sizeof(wchar_t), _TRUNCATE, format, args);
    va_end(args);
    Wh_Log(L"[DynamicIsland:ERROR] %s", buffer);
}
} // namespace DynamicIsland

// ============================================================================
// [Module] src\common\utils.h
// ============================================================================
namespace DynamicIsland {
namespace Utils {

int ScaleDpi(int value, UINT dpi);
float ScaleDpiF(float value, UINT dpi);
std::wstring FormatDuration(int totalSeconds);

template <typename T>
constexpr const T& Clamp(const T& v, const T& lo, const T& hi) {
    return (v < lo) ? lo : (hi < v) ? hi : v;
}

inline float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

std::wstring GetSystemUiFontFamily();
} // namespace Utils
} // namespace DynamicIsland

// ============================================================================
// [Module] src\platform\taskbar.h
// ============================================================================
namespace DynamicIsland {
namespace Platform {

struct TaskbarInfo {
    TaskbarPosition position = TaskbarPosition::Bottom;
    RECT taskbarRect = {0, 0, 0, 0};
    RECT monitorRect = {0, 0, 0, 0};
    RECT workAreaRect = {0, 0, 0, 0};
    bool isAutoHide = false;
    UINT dpi = 96;
};

TaskbarInfo QueryPrimaryTaskbar();
HWND GetTaskbarHwnd();
} // namespace Platform
} // namespace DynamicIsland

// ============================================================================
// [Module] src\graphics\animation.h
// ============================================================================
namespace DynamicIsland {
namespace Graphics {

float EvaluateCubicBezier(float x1, float y1, float x2, float y2, float t);
double GetHighPrecisionTimeMs();

class AnimatedValue {
public:
    AnimatedValue(float initialValue = 0.0f)
        : m_current(initialValue), m_start(initialValue), m_target(initialValue),
          m_startTime(0.0), m_durationMs(450.0), m_isAnimating(false) {}
    
    void SetTarget(float target, double durationMs = 450.0);
    void SnapTo(float value);
    void Update(double currentTimeMs);
    
    float GetValue() const { return m_current; }
    bool IsAnimating() const { return m_isAnimating; }

private:
    float m_current;
    float m_start;
    float m_target;
    double m_startTime;
    double m_durationMs;
    bool m_isAnimating;
};
} // namespace Graphics
} // namespace DynamicIsland

// ============================================================================
// [Module] src\graphics\icons.h
// ============================================================================
namespace DynamicIsland {
namespace Graphics {

enum class IconType {
    Play,
    Pause,
    Previous,
    Next,
    Volume,
    CapsLock,
    Bolt,
    Bluetooth,
    Battery,
    Checkmark,
    BatteryHorizontal
};

void DrawIcon(
    ID2D1RenderTarget* rt,
    ID2D1Factory* factory,
    ID2D1SolidColorBrush* brush,
    IconType type,
    const D2D1_RECT_F& bounds,
    D2D1_COLOR_F color
);
} // namespace Graphics
} // namespace DynamicIsland
// ============================================================================
// [Module] src\graphics\d2d_renderer.h
// ============================================================================
namespace DynamicIsland {
namespace Graphics {

using Microsoft::WRL::ComPtr;

class D2DRenderer {
public:
    D2DRenderer();
    ~D2DRenderer();
    
    bool Initialize(HWND hwnd);
    void Cleanup();
    void Resize(UINT width, UINT height);
    bool BeginDraw();
    HRESULT EndDraw();
    bool PresentLayeredWindow(HWND hwnd, int winX, int winY);
    
    void Clear(D2D1_COLOR_F color);
    void DrawRoundedPill(const D2D1_ROUNDED_RECT& pill, D2D1_COLOR_F fillColor, D2D1_COLOR_F borderColor, float strokeWidth = 1.0f);
    void DrawTextString(const std::wstring& text, const D2D1_RECT_F& rect, D2D1_COLOR_F color, float fontSize = 12.0f, bool bold = false);
    void DrawTextCentered(const std::wstring& text, const D2D1_RECT_F& rect, D2D1_COLOR_F color, float fontSize = 12.0f, bool bold = false);
    void DrawMarqueeText(const std::wstring& text, const D2D1_RECT_F& rect, D2D1_COLOR_F color, float offset, bool bold = false);
    void DrawBigText(const std::wstring& text, const D2D1_RECT_F& rect, D2D1_COLOR_F color);
    void DrawProgressBar(const D2D1_RECT_F& barRect, float progressFraction, D2D1_COLOR_F bgColor, D2D1_COLOR_F fgColor);
    void DrawProgressRing(D2D1_POINT_2F center, float radius, float progressFraction, D2D1_COLOR_F ringColor, float strokeWidth = 2.0f);
    void DrawStatusDot(D2D1_POINT_2F center, float radius, D2D1_COLOR_F color);
    void DrawEqualizerWaves(D2D1_POINT_2F origin, float height, const float* bands, bool playing);
    void DrawAlbumArt(const D2D1_RECT_F& bounds, float cornerRadius, float rotationDegrees = 0.0f);
    void SetAlbumArtBitmap(const uint8_t* pixels, UINT width, UINT height);
    void ClearAlbumArtBitmap();
    bool HasAlbumArtBitmap() const { return m_albumArtBitmap != nullptr; }
    void DrawIconImage(IconType type, const D2D1_RECT_F& bounds, D2D1_COLOR_F color);
    void DrawButtonPill(const D2D1_RECT_F& bounds, const std::wstring& text, bool isHovered);
    void DrawCircularButton(const D2D1_RECT_F& bounds, IconType icon, float iconScale = 1.0f, float hoverProgress = 0.0f);
    void DrawCircularButtonTransition(
        const D2D1_RECT_F& bounds,
        IconType fromIcon,
        IconType toIcon,
        float transitionProgress,
        float iconScale = 1.0f,
        float hoverProgress = 0.0f
    );
    
    ID2D1RenderTarget* GetRenderTarget() const { return m_dcRenderTarget.Get(); }

private:
    HWND m_hwnd = nullptr;
    ComPtr<ID2D1Factory> m_d2dFactory;
    ComPtr<IDWriteFactory> m_dwriteFactory;
    bool m_isDrawing = false;
    ComPtr<ID2D1DCRenderTarget> m_dcRenderTarget;
    ComPtr<ID2D1SolidColorBrush> m_solidBrush;
    ComPtr<ID2D1Bitmap> m_albumArtBitmap;
    ComPtr<ID2D1BitmapBrush> m_albumArtBrush;
    ComPtr<IDWriteTextFormat> m_textFormatRegular;
    ComPtr<IDWriteTextFormat> m_textFormatBold;
    ComPtr<IDWriteTextFormat> m_textFormatBig;
    ComPtr<IDWriteTextFormat> m_textFormatSmall;
    float m_fontSizeRegular = 12.0f;
    float m_fontSizeSmall = 11.0f;
    float m_fontSizeBig = 32.0f;
    
    HDC m_memDC = nullptr;
    HBITMAP m_hBitmap = nullptr;
    HBITMAP m_hOldBitmap = nullptr;
    void* m_pixelBits = nullptr;
    UINT m_width = 0;
    UINT m_height = 0;
    
    struct CachedMarquee {
        std::wstring text;
        bool bold = false;
        float targetHeight = 0.0f;
        ComPtr<IDWriteTextLayout> layout;
        ComPtr<IDWriteTextLayout> loopLayout;
        float textWidth = 0.0f;
        float loopUnitWidth = 0.0f;
    };
    struct CachedCenteredText {
        std::wstring text;
        IDWriteTextFormat* format = nullptr;
        float width = 0.0f;
        float height = 0.0f;
        float textWidth = 0.0f;
        ComPtr<IDWriteTextLayout> layout;
    };
    std::vector<CachedMarquee> m_marqueeCache;
    std::vector<CachedCenteredText> m_centeredTextCache;
    
    bool CreateDeviceResources();
    void DiscardDeviceResources();
};
} // namespace Graphics
} // namespace DynamicIsland

// ============================================================================
// [Module] src\overlay\layout.h
// ============================================================================
namespace DynamicIsland {
namespace Overlay {

struct IslandMetrics {
    float width = 0.0f;
    float height = 32.0f;
    float cornerRadius = 16.0f;
    float posX = 0.0f;
    float posY = 0.0f;
    bool isEmbedded = false;
};

class LayoutEngine {
public:
    static IslandMetrics CalculateMetrics(
        EventType eventType,
        IslandState state,
        const Platform::TaskbarInfo& tbInfo,
        UINT customWidth = 0
    );
};
} // namespace Overlay
} // namespace DynamicIsland

// ============================================================================
// [Module] src\overlay\island_window.h
// ============================================================================
namespace DynamicIsland {
namespace Services {
class ServiceManager;
}
namespace Overlay {

class IslandWindow {
public:
    IslandWindow();
    ~IslandWindow();
    
    bool Create();
    void Destroy();
    void Show(bool show);
    void SetState(IslandState state);
    void SetCurrentEvent(EventType eventType);
    void SetServiceManager(Services::ServiceManager* sm) { m_serviceManager = sm; }
    
    void UpdateMediaState(const MediaState& state) {
        // Animate only real play/pause changes after a track is already active.
        // The outgoing glyph shrinks into the center before the incoming glyph grows out.
        if (!m_mediaState.title.empty() && m_mediaState.isPlaying != state.isPlaying) {
            m_playbackIconFrom = m_mediaState.isPlaying ? Graphics::IconType::Pause : Graphics::IconType::Play;
            m_playbackIconTo = state.isPlaying ? Graphics::IconType::Pause : Graphics::IconType::Play;
            m_playbackIconTransitionStartMs = Graphics::GetHighPrecisionTimeMs();
            m_playbackIconTransitionProgress = 0.0f;
            m_playbackIconTransitionActive = true;
            if (m_hwnd && m_state != IslandState::Hidden) {
                SetTimer(m_hwnd, m_animTimerId, 8, nullptr);
            }
        } else if (!m_playbackIconTransitionActive) {
            // Keep the icon correct on first load and whenever no transition is running.
            const Graphics::IconType currentIcon = state.isPlaying
                ? Graphics::IconType::Pause
                : Graphics::IconType::Play;
            m_playbackIconFrom = currentIcon;
            m_playbackIconTo = currentIcon;
            m_playbackIconTransitionProgress = 1.0f;
        }

        // Ignore external position updates while the user is dragging the progress bar.
        if (m_isSeeking) {
            // Still allow title/artist/play-state changes (e.g. track switch mid-seek).
            if (m_mediaState.title != state.title || m_mediaState.artist != state.artist) {
                m_marqueeOffset = 0.0f;
                m_albumArtRotation = 0.0f;
                m_currentThumbVersion = 0;
                m_mediaState.title = state.title;
                m_mediaState.artist = state.artist;
            }
            m_mediaState.isPlaying = state.isPlaying;
            m_mediaState.durationSeconds = state.durationSeconds;
            return;
        }
        
        if (m_mediaState.title != state.title || m_mediaState.artist != state.artist) {
            m_marqueeOffset = 0.0f;
            m_albumArtRotation = 0.0f;
            m_currentThumbVersion = 0;
        }
        const bool playbackClockNeedsReset =
            m_mediaState.title != state.title ||
            m_mediaState.isPlaying != state.isPlaying ||
            m_mediaState.elapsedSeconds != state.elapsedSeconds;
        m_mediaState = state;
        // Use the Windows monotonic tick counter for the display clock.
        // The render timer advances the visible elapsed time between GSMTC polls.
        if (playbackClockNeedsReset || m_mediaPositionAnchorMs <= 0.0) {
            m_mediaPositionAnchorSeconds = (state.durationSeconds > 0)
                ? static_cast<double>(state.progress) * static_cast<double>(state.durationSeconds)
                : static_cast<double>(state.elapsedSeconds);
            m_mediaPositionAnchorMs = static_cast<double>(GetTickCount64());
        }
    }
    
    void UpdateTimerState(const TimerState& state) { m_timerState = state; }
    void UpdateMicState(const MicState& state) { m_micState = state; }
    void UpdateTransientState(const TransientState& state) { m_transientState = state; }
    
    MediaState& GetMediaState() { return m_mediaState; }
    TimerState& GetTimerState() { return m_timerState; }
    MicState& GetMicState() { return m_micState; }
    TransientState& GetTransientState() { return m_transientState; }
    
    void TriggerAnimationUpdate();
    void UpdateMediaButtonHover(float localX, float localY);
    void Render();
    void UpdateDimensions();
    void OnTaskbarOrDisplayChanged();
    void RegisterHotkeys();
    void UnregisterHotkeys();
    void ReinitializeRenderer();
    
    HWND GetHwnd() const { return m_hwnd; }
    IslandState GetState() const { return m_state; }
    EventType GetCurrentEvent() const { return m_currentEvent; }
    
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
    static LRESULT CALLBACK WndProcImpl(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    HWND m_hwnd = nullptr;
    IslandState m_state = IslandState::Hidden;
    EventType m_currentEvent = EventType::None;
    Services::ServiceManager* m_serviceManager = nullptr;
    
    MediaState m_mediaState;
    TimerState m_timerState;
    MicState m_micState;
    TransientState m_transientState;
    
    std::unique_ptr<Graphics::D2DRenderer> m_renderer;
    
    Graphics::AnimatedValue m_animWidth{0.0f};
    Graphics::AnimatedValue m_animHeight{32.0f};
    Graphics::AnimatedValue m_animRadius{16.0f};
    Graphics::AnimatedValue m_animOpacity{0.0f};
    Graphics::AnimatedValue m_crossfadeAlpha{1.0f};
    Graphics::AnimatedValue m_animScale{1.0f};
    // Independent hover animations for Previous, Play/Pause, and Next.
    Graphics::AnimatedValue m_prevButtonHover{0.0f};
    Graphics::AnimatedValue m_playButtonHover{0.0f};
    Graphics::AnimatedValue m_nextButtonHover{0.0f};
    // Separate play/pause glyph transition: outgoing icon contracts, then incoming expands.
    Graphics::IconType m_playbackIconFrom = Graphics::IconType::Play;
    Graphics::IconType m_playbackIconTo = Graphics::IconType::Play;
    float m_playbackIconTransitionProgress = 1.0f;
    double m_playbackIconTransitionStartMs = 0.0;
    bool m_playbackIconTransitionActive = false;
    int m_hoveredMediaButton = -1; // -1 none, 0 previous, 1 play/pause, 2 next
    
    EventType m_prevEvent = EventType::None;
    TransientState m_prevTransientState;
    MediaState m_prevMediaState;
    TimerState m_prevTimerState;
    
    bool m_isHovered = false;
    float m_marqueeOffset = 0.0f;
    float m_albumArtRotation = 0.0f;
    
    // Per-frame smoothed equalizer bands so the compact bars glide smoothly
    // between the ~200 ms audio service polls instead of jumping.
    float m_eqBandSmooth[6]{};
    double m_lastEqUpdateTimeMs = 0.0;
    
    double m_lastMarqueeTimeMs = 0.0;
    double m_mediaPositionAnchorMs = 0.0;
    double m_mediaPositionAnchorSeconds = 0.0;
    uint64_t m_currentThumbVersion = 0;
    
    UINT_PTR m_autoCollapseTimerId = 1001;
    UINT_PTR m_animTimerId = 1002;
    UINT_PTR m_waveTimerId = 1003;
    UINT_PTR m_geometryCheckTimerId = 1004;
    
    Platform::TaskbarInfo m_cachedTaskbar;
    UINT m_taskbarCreatedMsg = 0;
    bool m_hotkeysRegistered = false;
    
    int m_windowX = 0;
    int m_windowY = 0;
    int m_windowW = 0;
    int m_windowH = 0;
    
    float m_currentPillX = 0.0f;
    float m_currentPillY = 0.0f;
    float m_currentPillW = 0.0f;
    float m_currentPillH = 0.0f;
    float m_currentPillR = 0.0f;
    
    bool m_isMouseDown = false;
    int m_mouseDownX = 0;
    int m_mouseDownY = 0;
    
    // Interactive progress bar seeking (expanded media view)
    bool m_isSeeking = false;
    float m_seekProgress = 0.0f;
    
    // Interactive volume scrubbing (compact volume pill)
    bool m_isVolumeDragging = false;
    float m_volumeDragFraction = 0.0f;
    double m_volumeDragEndTimeMs = 0.0;
    
    void ArmAutoCollapse();
    void DisarmAutoCollapse();
    void OnClick(int clientX, int clientY);
    bool TryBeginSeek(float localX, float localY, float w, float h);
    void UpdateSeekFromLocalX(float localX, float w);
    void EndSeek(bool commit);
    bool TryBeginVolumeDrag(float localX, float localY, float w, float h);
    void UpdateVolumeDragFromLocalX(float localX, float w);
    void EndVolumeDrag();
    void CheckTaskbarGeometry();
    bool IsPointInSquircle(float clientX, float clientY) const;
    void UpdateEqualizerSmoothing();
    void RenderCompactContent(EventType eventType, float w, float h, float alpha);
};
} // namespace Overlay
} // namespace DynamicIsland

// ============================================================================
// [Module] src\services\service_manager.h
// ============================================================================
namespace DynamicIsland {
namespace Services {

class MediaService;
class TimerService;
class AudioService;

struct QueuedEvent {
    EventType type = EventType::None;
    DWORD durationMs = 2000;
    std::wstring title;
    std::wstring subtitle;
    int progressPercent = 0;
};

struct EqBands {
    // Six independent frequency ranges, ordered from bass to treble.
    float values[6]{};
};

class ServiceManager {
public:
    explicit ServiceManager(Overlay::IslandWindow* window);
    ~ServiceManager();
    
    void Initialize();
    void Shutdown();
    
    void RegisterMediaService(MediaService* ms) { m_mediaService = ms; }
    void RegisterTimerService(TimerService* ts) { m_timerService = ts; }
    void RegisterAudioService(AudioService* as) { m_audioService = as; }
    
    void PostTransientEvent(const QueuedEvent& event);
    void SetLiveActivity(EventType type, bool active);
    void Update();
    void UpdateMediaState(const MediaState& state);
    void UpdateTimerState(const TimerState& state);
    void UpdateMicState(const MicState& state);
    
    bool GetMediaThumbnail(std::vector<uint8_t>& pixels, uint32_t& width, uint32_t& height, uint64_t& version);
    EqBands GetAudioBands() const;
    
    void OnMediaPlayPause();
    void OnMediaPrev();
    void OnMediaNext();
    void OnMediaSeek(float progressFraction);
    void OnVolumeScrub(float volumeFraction);
    void OnTimerTogglePause();
    void OnTimerStop();
    
    void TriggerTestScenario(int index);
    void FireTransient(EventType type);
    void RefreshVisibility();
    EventType ResolveCurrentLiveActivity() const;

private:
    Overlay::IslandWindow* m_window = nullptr;
    MediaService* m_mediaService = nullptr;
    TimerService* m_timerService = nullptr;
    AudioService* m_audioService = nullptr;
    
    std::deque<QueuedEvent> m_transientQueue;
    bool m_hasActiveTransient = false;
    QueuedEvent m_currentTransient;
    ULONGLONG m_transientExpiryTick = 0;
    
    bool m_mediaActive = false;
    bool m_timerActive = false;
    bool m_micActive = false;

    // Return the island to idle five seconds after media playback is paused.
    ULONGLONG m_mediaPausedSinceTick = 0;
    bool m_mediaPauseExpired = false;
    
    int m_demoIndex = 0;
    std::wstring m_lastTrackTitle;
    
    void PumpNextEvent();
};
} // namespace Services
} // namespace DynamicIsland

// ============================================================================
// [Module] src\services\media_service.h
// ============================================================================
#if defined(__has_include)
#if __has_include(<winrt/Windows.Foundation.h>) && __has_include(<winrt/Windows.Media.Control.h>) && __has_include(<winrt/Windows.Storage.Streams.h>)
#define DYNAMIC_ISLAND_HAS_WINRT_GSMTC 1
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Media.Control.h>
#if __has_include(<winrt/Windows.Devices.Enumeration.h>) && __has_include(<winrt/Windows.Devices.Bluetooth.h>)
#define DYNAMIC_ISLAND_HAS_WINRT_DEVICE_WATCHER 1
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#endif
#endif
#endif

namespace DynamicIsland {
namespace Services {

class ServiceManager;

class MediaService {
public:
    explicit MediaService(ServiceManager* manager);
    ~MediaService();
    
    void Start();
    void Stop();
    void Poll();
    
    MediaState SnapshotState() const {
        std::lock_guard<std::mutex> lock(m_thumbMutex);
        return m_state;
    }
    
    bool IsPlaying() const {
        std::lock_guard<std::mutex> lock(m_thumbMutex);
        return m_state.isPlaying;
    }
    
    bool HasRealSession() const { return m_hasRealSession; }
    
    void Play();
    void Pause();
    bool TogglePlayPause();
    void Next();
    void Previous();
    void Seek(float progressFraction);
    
    bool CopyThumbnail(std::vector<uint8_t>& outPixels, uint32_t& outW, uint32_t& outH, uint64_t& outVersion) const;
    void StoreDecodedThumbnail(std::vector<uint8_t> pixels, uint32_t width, uint32_t height, const std::wstring& trackId);
    void FinishThumbnailFetch();
    void ThumbnailWorkerFinished();
    
    // Called by the GSMTC thumbnail worker (a free function) when the session
    // provided no usable artwork; dispatches an iTunes Search API fallback.
    void RequestiTunesArtworkFallback(const std::wstring& trackId);

private:
    ServiceManager* m_manager = nullptr;
    MediaState m_state;
    std::atomic<bool> m_isRunning{false};
    std::atomic<bool> m_hasRealSession{false};
    
    UniqueWinHandle m_pollThread;
    DWORD  m_pollThreadId = 0;
    std::atomic<bool> m_pollStop{false};
    
    mutable std::mutex m_thumbMutex;
    // Keep the UI responsive while the asynchronous GSMTC command reaches the player.
    // These fields are protected by m_thumbMutex and prevent stale polling results
    // from briefly reverting an optimistic play/pause toggle.
    bool m_hasPendingPlaybackOverride = false;
    bool m_pendingPlaybackOverride = false;
    ULONGLONG m_pendingPlaybackOverrideUntilTick = 0;
    mutable std::mutex m_sessionMutex;
    std::mutex m_thumbWorkerMutex;
    std::condition_variable m_thumbWorkerCv;
    std::atomic<uint32_t> m_activeThumbnailWorkers{0};
    std::vector<uint8_t> m_thumbPixels;
    uint32_t m_thumbWidth = 0;
    uint32_t m_thumbHeight = 0;
    uint64_t m_thumbVersion = 0;
    std::wstring m_lastFetchedTrackId;
    std::atomic<bool> m_isFetchingThumbnail{false};
    
    // iTunes Search API fallback for album artwork when the media session
    // provides no thumbnail (or the fetch fails). Reuses the same active-worker
    // accounting as the GSMTC thumbnail workers for clean shutdown.
    std::atomic<bool> m_isFetchingItunes{false};
    
    struct iTunesFetchContext {
        MediaService* self = nullptr;
        std::wstring trackId;
        std::wstring title;
        std::wstring artist;
    };
    
    static DWORD WINAPI iTunesArtworkWorkerThreadStatic(LPVOID param);
    void TriggeriTunesArtworkFetch(const std::wstring& trackId, const std::wstring& title, const std::wstring& artist);
    
    struct CachedThumb {
        std::vector<uint8_t> pixels;
        uint32_t w = 0;
        uint32_t h = 0;
    };
    std::unordered_map<std::wstring, CachedThumb> m_thumbCache;
    static constexpr size_t kThumbCacheMax = 16;
    
    std::atomic<bool> m_stateDirty{false};
    MediaState m_lastSentState{};
    bool m_hasLastSentState = false;

    static DWORD WINAPI PollThreadProcStatic(LPVOID param);
    void PollThreadProcInternal();
    
#if defined(DYNAMIC_ISLAND_HAS_WINRT_GSMTC)
    winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager m_sessionManager{nullptr};
    winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession m_currentSession{nullptr};
    winrt::Windows::Storage::Streams::IRandomAccessStreamReference m_pendingThumbRef{nullptr};
    std::wstring m_pendingTrackId;
    double m_lastSessionsRefreshMs = 0.0;
    std::atomic<bool> m_sessionsChanged{true};
    std::atomic<bool> m_sessionDataChanged{true};
    winrt::event_token m_sessionsChangedToken{};
    winrt::event_token m_playbackInfoChangedToken{};
    winrt::event_token m_mediaPropertiesChangedToken{};
    winrt::event_token m_timelinePropertiesChangedToken{};
    void DetachSessionEvents() noexcept;
    void AttachSessionEvents(const winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession& session);
    
    void TriggerAsyncThumbnailFetch(winrt::Windows::Storage::Streams::IRandomAccessStreamReference thumbRef, const std::wstring& trackId);
#endif
};
} // namespace Services
} // namespace DynamicIsland

// ============================================================================
// [Module] src\services\audio_service.h
// ============================================================================
namespace DynamicIsland {
namespace Services {

using Microsoft::WRL::ComPtr;

class ServiceManager;

class AudioService {
public:
    explicit AudioService(ServiceManager* manager);
    ~AudioService();
    
    void Start();
    void Stop();
    void CheckMicUsage();
    void PollVolume();
    void SetVolume(float volume);
    void SetLoopbackCaptureEnabled(bool enabled);
    void StopLoopbackCapture();
    
    float GetCurrentVolume() const { return m_currentVolume; }
    bool IsMuted() const { return m_isMuted; }
    bool IsMicInUse() const { return m_isMicInUse; }
    
    float GetBand(size_t index) const {
        return index < 6 ? m_bandSmooth[index].load(std::memory_order_relaxed) : 0.0f;
    }

private:
    [[maybe_unused]] ServiceManager* m_manager = nullptr;
    float m_currentVolume = 0.5f;
    bool m_isMuted = false;
    bool m_isMicInUse = false;
    std::atomic<bool> m_isRunning{false};
    std::atomic<bool> m_loopbackRequested{false};
    UniqueWinHandle m_loopbackThread;
    UniqueWinHandle m_loopbackWakeEvent;
    UniqueWinHandle m_audioSamplesReadyEvent;
    std::wstring m_loopbackDeviceId;
    ULONGLONG m_lastLoopbackDeviceCheckTick = 0;

    static DWORD WINAPI LoopbackThreadProcStatic(LPVOID param);
    void LoopbackThreadProc();
    void UpdateLoopback();

    ComPtr<IMMDeviceEnumerator> m_deviceEnumerator;
    ComPtr<IMMDeviceEnumerator> m_volumeDeviceEnumerator;
    ComPtr<IAudioEndpointVolume> m_endpointVolume;
    std::wstring m_volumeDeviceId;
    ULONGLONG m_lastVolumeDeviceCheckTick = 0;
    ComPtr<IAudioClient> m_audioClient;
    ComPtr<IAudioCaptureClient> m_captureClient;
    WAVEFORMATEX* m_pwfx = nullptr;
    bool m_isLoopbackActive = false;
    
    
    struct BiquadBand {
        float b0 = 0.0f, b1 = 0.0f, b2 = 0.0f;
        float a1 = 0.0f, a2 = 0.0f;
        float x1 = 0.0f, x2 = 0.0f;
        float y1 = 0.0f, y2 = 0.0f;
    };
    BiquadBand m_eqFilters[6]{};
    float m_eqFilterSampleRate = 0.0f;
    std::atomic<float> m_bandSmooth[6]{};
    float m_lastReportedVolume = -1.0f;
    bool m_lastReportedMute = false;
    bool m_hasReportedVolume = false;
    ULONGLONG m_lastVolumePollTick = 0;
    ULONGLONG m_lastMicPollTick = 0;
    std::wstring m_lastMicApp;

    void ResetLoopback();
};
} // namespace Services
} // namespace DynamicIsland

// ============================================================================
// [Module] src\services\power_service.h
// ============================================================================
namespace DynamicIsland {
namespace Services {

class ServiceManager;

class PowerService {
public:
    explicit PowerService(ServiceManager* manager);
    ~PowerService();
    
    void Start();
    void Stop();
    void Poll();
    
    BYTE GetBatteryPercent() const { return m_batteryPercent; }
    bool IsACConnected() const { return m_isACConnected; }

private:
    ServiceManager* m_manager = nullptr;
    BYTE m_batteryPercent = 100;
    bool m_isACConnected = false;
    bool m_wasACConnected = false;
    bool m_hasKnownACStatus = false;
    bool m_lowBatteryAlerted = false;
    bool m_wasBatterySaver = false;
    bool m_isRunning = false;
    ULONGLONG m_lastPollTick = 0;
};
} // namespace Services
} // namespace DynamicIsland

// ============================================================================
// [Module] src\services\keyboard_service.h
// ============================================================================
namespace DynamicIsland {
namespace Services {

class ServiceManager;

class KeyboardService {
public:
    explicit KeyboardService(ServiceManager* manager);
    ~KeyboardService();
    
    void Start();
    void Stop();
    void Poll();
    
    bool IsCapsLockOn() const { return m_capsLockState; }

private:
    ServiceManager* m_manager = nullptr;
    bool m_capsLockState = false;
    bool m_numLockState = false;
    bool m_isRunning = false;
};
} // namespace Services
} // namespace DynamicIsland

// ============================================================================
// [Module] src\services\bluetooth_service.h
// ============================================================================
namespace DynamicIsland {
namespace Services {

class ServiceManager;

struct BluetoothDeviceInfo {
    std::wstring name;
    int batteryLevel = -1;
    bool isConnected = false;
};

class BluetoothService {
public:
    explicit BluetoothService(ServiceManager* manager);
    ~BluetoothService();
    
    void Start();
    void Stop();
    void Poll();

private:
    ServiceManager* m_manager = nullptr;
    std::atomic<bool> m_isRunning{false};
    bool m_baselineDone = false;
    std::unordered_set<std::wstring> m_knownConnected;
    ULONGLONG m_lastPollTick = 0;
#if defined(DYNAMIC_ISLAND_HAS_WINRT_DEVICE_WATCHER)
    winrt::Windows::Devices::Enumeration::DeviceWatcher m_deviceWatcher{nullptr};
    winrt::event_token m_watcherAddedToken{};
    winrt::event_token m_watcherUpdatedToken{};
    winrt::event_token m_watcherRemovedToken{};
    winrt::event_token m_watcherEnumerationToken{};
    std::mutex m_watcherMutex;
    std::unordered_map<std::wstring, std::wstring> m_watcherConnected;
    bool m_watcherBaselineDone = false;
    bool m_watcherActive = false;
    void StartDeviceWatcher();
    void StopDeviceWatcher();
    void OnWatcherDevice(const winrt::Windows::Devices::Enumeration::DeviceInformation& info, bool fromUpdate);
#endif
};
} // namespace Services
} // namespace DynamicIsland

// ============================================================================
// [Module] src\services\timer_service.h
// ============================================================================
namespace DynamicIsland {
namespace Services {

class ServiceManager;

class TimerService {
public:
    explicit TimerService(ServiceManager* manager);
    ~TimerService();
    
    void Start(int durationSeconds);
    void Pause();
    void Resume();
    void Stop();
    void Tick();
    
    bool IsActive() const { return m_isActive; }
    bool IsPaused() const { return m_isPaused; }

private:
    ServiceManager* m_manager = nullptr;
    int m_totalSeconds = 0;
    int m_remainingSeconds = 0;
    bool m_isActive = false;
    bool m_isPaused = false;
    ULONGLONG m_deadlineTick = 0;

    void SyncState();
};
} // namespace Services
} // namespace DynamicIsland
// ============================================================================
// [Module] src\common\utils.cpp
// ============================================================================
namespace DynamicIsland {
namespace Utils {

int ScaleDpi(int value, UINT dpi) {
    if (dpi == 0) dpi = 96;
    return MulDiv(value, dpi, 96);
}

float ScaleDpiF(float value, UINT dpi) {
    if (dpi == 0) dpi = 96;
    return (value * static_cast<float>(dpi)) / 96.0f;
}

std::wstring FormatDuration(int totalSeconds) {
    if (totalSeconds < 0) totalSeconds = 0;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    wchar_t buf[32];
    swprintf_s(buf, L"%d:%02d", minutes, seconds);
    return std::wstring(buf);
}

static bool IsFontFamilyInstalled(const wchar_t* familyName) {
    if (!familyName || !*familyName) return false;
    const HKEY roots[] = { HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER };
    const wchar_t* subPaths[] = {
        L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts",
        L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"
    };
    for (size_t i = 0; i < _countof(roots); ++i) {
        HKEY hKey = nullptr;
        if (RegOpenKeyExW(roots[i], subPaths[i], 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
            continue;
        }
        DWORD index = 0;
        wchar_t valueName[256];
        for (;;) {
            DWORD valueNameSize = _countof(valueName);
            LSTATUS rc = RegEnumValueW(hKey, index++, valueName, &valueNameSize,
                                       nullptr, nullptr, nullptr, nullptr);
            if (rc == ERROR_NO_MORE_ITEMS) break;
            if (rc != ERROR_SUCCESS) break;
            if (wcsstr(valueName, familyName) != nullptr) {
                RegCloseKey(hKey);
                return true;
            }
        }
        RegCloseKey(hKey);
    }
    return false;
}

std::wstring GetSystemUiFontFamily() {
    // Prefer the modern Windows 11 font stack: its proportional digits are the
    // closest visual match to the iOS look. Legacy "MS Shell Dlg 2" substitutes
    // such as Tahoma render boxy, tabular-looking digits, so they are only a
    // last-resort fallback.
    if (IsFontFamilyInstalled(L"Inter")) {
        return L"Inter";
    }
    if (IsFontFamilyInstalled(L"Segoe UI Variable")) {
        return L"Segoe UI Variable Text";
    }
    if (IsFontFamilyInstalled(L"Segoe UI")) {
        return L"Segoe UI";
    }
    HKEY hKey = nullptr;
    std::wstring font = L"Segoe UI";
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                      L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\FontSubstitutes",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        wchar_t buf[256];
        DWORD sz = sizeof(buf);
        DWORD type = 0;
        if (RegQueryValueExW(hKey, L"MS Shell Dlg 2", nullptr, &type,
                             reinterpret_cast<LPBYTE>(buf), &sz) == ERROR_SUCCESS
            && type == REG_SZ && sz > 0) {
            font = buf;
        }
        RegCloseKey(hKey);
    }
    return font;
}
} // namespace Utils
} // namespace DynamicIsland

// ============================================================================
// [Module] src\platform\taskbar.cpp
// ============================================================================
namespace DynamicIsland {
namespace Platform {

HWND GetTaskbarHwnd() {
    return FindWindowW(L"Shell_TrayWnd", nullptr);
}

TaskbarInfo QueryPrimaryTaskbar() {
    TaskbarInfo info{};
    
    HWND hDesktop = GetDesktopWindow();
    HMONITOR hMon = MonitorFromWindow(hDesktop, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi{};
    mi.cbSize = sizeof(MONITORINFO);
    if (GetMonitorInfoW(hMon, &mi)) {
        info.monitorRect = mi.rcMonitor;
        info.workAreaRect = mi.rcWork;
    } else {
        info.monitorRect = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
        info.workAreaRect = info.monitorRect;
    }
    
    HWND hTaskbar = GetTaskbarHwnd();
    RECT realRect{};
    bool hasHwndRect = (hTaskbar && GetWindowRect(hTaskbar, &realRect) && (realRect.right > realRect.left));
    
    APPBARDATA abd{};
    abd.cbSize = sizeof(APPBARDATA);
    abd.hWnd = hTaskbar;
    UINT_PTR res = SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
    if (res && (abd.rc.right > abd.rc.left)) {
        info.taskbarRect = abd.rc;
    } else if (hasHwndRect) {
        info.taskbarRect = realRect;
    } else {
        info.taskbarRect = { info.monitorRect.left, info.monitorRect.top, info.monitorRect.right, info.monitorRect.top + 34 };
    }
    
    int monHeight = info.monitorRect.bottom - info.monitorRect.top;
    if (info.taskbarRect.top <= info.monitorRect.top + 10 && info.taskbarRect.bottom < info.monitorRect.bottom - (monHeight / 2)) {
        info.position = TaskbarPosition::Top;
    } else if (info.taskbarRect.bottom >= info.monitorRect.bottom - 10 && info.taskbarRect.top > info.monitorRect.top + (monHeight / 2)) {
        info.position = TaskbarPosition::Bottom;
    } else if (info.taskbarRect.left <= info.monitorRect.left + 10) {
        info.position = TaskbarPosition::Left;
    } else if (info.taskbarRect.right >= info.monitorRect.right - 10) {
        info.position = TaskbarPosition::Right;
    } else {
        switch (abd.uEdge) {
            case ABE_TOP:    info.position = TaskbarPosition::Top; break;
            case ABE_BOTTOM: info.position = TaskbarPosition::Bottom; break;
            case ABE_LEFT:   info.position = TaskbarPosition::Left; break;
            case ABE_RIGHT:  info.position = TaskbarPosition::Right; break;
            default:         info.position = TaskbarPosition::Top; break;
        }
    }
    
    UINT_PTR state = SHAppBarMessage(ABM_GETSTATE, &abd);
    info.isAutoHide = (state & ABS_AUTOHIDE) != 0;
    
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
        auto pGetDpi = reinterpret_cast<GetDpiForWindow_t>(GetProcAddress(hUser32, "GetDpiForWindow"));
        if (pGetDpi && hTaskbar) {
            info.dpi = pGetDpi(hTaskbar);
        }
    }
    if (info.dpi == 0) {
        info.dpi = 96;
    }
    
    return info;
}
} // namespace Platform
} // namespace DynamicIsland

// ============================================================================
// [Module] src\graphics\animation.cpp
// ============================================================================
namespace DynamicIsland {
namespace Graphics {

float EvaluateCubicBezier(float x1, float y1, float x2, float y2, float x) {
    if (x <= 0.0f) return 0.0f;
    if (x >= 1.0f) return 1.0f;
    
    float t = x;
    for (int i = 0; i < 8; ++i) {
        float oneMinusT = 1.0f - t;
        float currentX = 3.0f * oneMinusT * oneMinusT * t * x1 +
                         3.0f * oneMinusT * t * t * x2 +
                         t * t * t;
        float derivativeX = 3.0f * oneMinusT * oneMinusT * x1 +
                            6.0f * oneMinusT * t * (x2 - x1) +
                            3.0f * t * t * (1.0f - x2);
        if (std::abs(derivativeX) < 1e-5f) break;
        t -= (currentX - x) / derivativeX;
        t = Utils::Clamp(t, 0.0f, 1.0f);
    }
    
    float oneMinusT = 1.0f - t;
    return 3.0f * oneMinusT * oneMinusT * t * y1 +
           3.0f * oneMinusT * t * t * y2 +
           t * t * t;
}

double GetHighPrecisionTimeMs() {
    static const double frequency = []() -> double {
        LARGE_INTEGER value{};
        if (!QueryPerformanceFrequency(&value) || value.QuadPart <= 0) return 0.0;
        return static_cast<double>(value.QuadPart);
    }();

    LARGE_INTEGER counter{};
    if (!QueryPerformanceCounter(&counter) || frequency <= 0.0) {
        return static_cast<double>(GetTickCount64());
    }
    return (static_cast<double>(counter.QuadPart) * 1000.0) / frequency;
}

void AnimatedValue::SetTarget(float target, double durationMs) {
    if (std::abs(m_target - target) < 0.001f && !m_isAnimating) {
        return;
    }
    m_start = m_current;
    m_target = target;
    m_durationMs = (durationMs > 0.0) ? durationMs : 1.0;
    m_startTime = GetHighPrecisionTimeMs();
    m_isAnimating = true;
}

void AnimatedValue::SnapTo(float value) {
    m_current = value;
    m_start = value;
    m_target = value;
    m_isAnimating = false;
}

void AnimatedValue::Update(double currentTimeMs) {
    if (!m_isAnimating) return;
    
    if (currentTimeMs >= m_startTime + m_durationMs) {
        m_current = m_target;
        m_isAnimating = false;
        return;
    }
    
    float linearProgress = static_cast<float>((currentTimeMs - m_startTime) / m_durationMs);
    float curvedProgress = EvaluateCubicBezier(0.34f, 1.3f, 0.5f, 1.0f, linearProgress);
    m_current = m_start + (m_target - m_start) * curvedProgress;
}
} // namespace Graphics
} // namespace DynamicIsland

// ============================================================================
// [Module] src\graphics\icons.cpp
// ============================================================================
// Icon glyphs are rendered from vector path data derived from Google's
// Material Symbols (Rounded style, weight 400), licensed under the Apache
// License, Version 2.0. The original SVG path data (viewBox "0 -960 960 960")
// was pre-converted into a 24-unit grid with absolute line/cubic segments and
// knock-out subpaths removed, so each glyph renders as a solid, rounded shape
// similar to the filled iOS (SF Symbols) look. Being pure geometry, the icons
// stay crisp at any DPI and are tinted by the caller's brush color.
// License note: https://www.apache.org/licenses/LICENSE-2.0
namespace DynamicIsland {
namespace Graphics {

namespace {

inline float ScaleX(float x, const D2D1_RECT_F& bounds) {
    return bounds.left + (x / 24.0f) * (bounds.right - bounds.left);
}

inline float ScaleY(float y, const D2D1_RECT_F& bounds) {
    return bounds.top + (y / 24.0f) * (bounds.bottom - bounds.top);
}

inline D2D1_POINT_2F Pt(float x, float y, const D2D1_RECT_F& bounds) {
    return D2D1::Point2F(ScaleX(x, bounds), ScaleY(y, bounds));
}

enum IconSegKind { SEG_LINE, SEG_CUBIC, SEG_ARC };

struct IconSeg {
    IconSegKind kind;
    float ex, ey;                            // segment end (24-unit grid)
    float c1x = 0.0f, c1y = 0.0f;            // cubic control point 1
    float c2x = 0.0f, c2y = 0.0f;            // cubic control point 2
    float rx = 0.0f, ry = 0.0f, rot = 0.0f;  // arc radii / rotation
    uint8_t large = 0;                       // arc large-angle flag
    uint8_t sweep = 0;                       // arc sweep flag (1 = clockwise)
};

struct IconFigure {
    const IconSeg* segs;
    size_t count;
    D2D1_POINT_2F start;  // figure start point (24-unit grid)
};

struct IconGlyph {
    const IconFigure* figures;
    size_t count;
};

// --- play ---
constexpr IconSeg kPLAYSegs[] = {
    // Rounded rear/top corner of the play triangle.
    { SEG_CUBIC, 9.15f, 4.05f, 8.55f, 3.55f, 8.90f, 3.70f },
    { SEG_LINE,  18.25f, 10.15f },
    // Broad cubic nose gives the arrow tip a visible, smooth radius.
    { SEG_CUBIC, 18.25f, 13.85f, 19.75f, 11.10f, 19.75f, 12.90f },
    { SEG_LINE,  9.15f, 19.95f },
    // Rounded lower/rear corners.
    { SEG_CUBIC, 7.80f, 20.30f, 8.90f, 20.30f, 8.55f, 20.45f },
    { SEG_CUBIC, 6.20f, 18.90f, 6.20f, 19.80f, 6.20f, 19.25f },
    { SEG_LINE,  6.20f, 5.10f },
    { SEG_CUBIC, 7.80f, 3.70f, 6.20f, 4.00f, 6.80f, 3.70f },
};
constexpr IconFigure kPLAYFigs[] = {
    { kPLAYSegs, _countof(kPLAYSegs), { 7.80f, 3.70f } },
};

// --- pause ---
constexpr IconSeg kPAUSESegs[] = {
    { SEG_CUBIC, 7.2f, 5.0f, 6.2f, 5.333f, 6.533f, 5.0f },
    { SEG_LINE,  9.2f, 5.0f },
    { SEG_CUBIC, 10.2f, 6.0f, 9.867f, 5.0f, 10.2f, 5.333f },
    { SEG_LINE,  10.2f, 18.0f },
    { SEG_CUBIC, 9.2f, 19.0f, 10.2f, 18.667f, 9.867f, 19.0f },
    { SEG_LINE,  7.2f, 19.0f },
    { SEG_CUBIC, 6.2f, 18.0f, 6.533f, 19.0f, 6.2f, 18.667f },
};
constexpr IconSeg kPAUSESegs1[] = {
    { SEG_CUBIC, 14.8f, 5.0f, 13.8f, 5.333f, 14.133f, 5.0f },
    { SEG_LINE,  16.8f, 5.0f },
    { SEG_CUBIC, 17.8f, 6.0f, 17.467f, 5.0f, 17.8f, 5.333f },
    { SEG_LINE,  17.8f, 18.0f },
    { SEG_CUBIC, 16.8f, 19.0f, 17.8f, 18.667f, 17.467f, 19.0f },
    { SEG_LINE,  14.8f, 19.0f },
    { SEG_CUBIC, 13.8f, 18.0f, 14.133f, 19.0f, 13.8f, 18.667f },
};
constexpr IconFigure kPAUSEFigs[] = {
    { kPAUSESegs, _countof(kPAUSESegs), { 6.2f, 6.0f } },
    { kPAUSESegs1, _countof(kPAUSESegs1), { 13.8f, 6.0f } },
};

// --- next ---
constexpr IconSeg kNEXTSegs[] = {
    { SEG_CUBIC, 5.056f, 5.881f, 4.1f, 5.433f, 4.419f, 5.294f },
    { SEG_LINE,  10.744f, 11.119f },
    { SEG_CUBIC, 10.744f, 12.881f, 11.381f, 11.706f, 11.381f, 12.294f },
    { SEG_LINE,  5.056f, 18.119f },
    { SEG_CUBIC, 4.1f, 17.7f, 4.419f, 18.706f, 4.1f, 18.567f },
};
constexpr IconSeg kNEXTSegs1[] = {
    { SEG_CUBIC, 13.256f, 5.881f, 12.3f, 5.433f, 12.619f, 5.294f },
    { SEG_LINE,  18.944f, 11.119f },
    { SEG_CUBIC, 18.944f, 12.881f, 19.581f, 11.706f, 19.581f, 12.294f },
    { SEG_LINE,  13.256f, 18.119f },
    { SEG_CUBIC, 12.3f, 17.7f, 12.619f, 18.706f, 12.3f, 18.567f },
};
constexpr IconFigure kNEXTFigs[] = {
    { kNEXTSegs, _countof(kNEXTSegs), { 4.1f, 6.3f } },
    { kNEXTSegs1, _countof(kNEXTSegs1), { 12.3f, 6.3f } },
};

// --- prev ---
constexpr IconSeg kPREVSegs[] = {
    { SEG_CUBIC, 10.744f, 5.881f, 11.7f, 5.433f, 11.381f, 5.294f },
    { SEG_LINE,  5.056f, 11.119f },
    { SEG_CUBIC, 5.056f, 12.881f, 4.419f, 11.706f, 4.419f, 12.294f },
    { SEG_LINE,  10.744f, 18.119f },
    { SEG_CUBIC, 11.7f, 17.7f, 11.381f, 18.706f, 11.7f, 18.567f },
};
constexpr IconSeg kPREVSegs1[] = {
    { SEG_CUBIC, 18.944f, 5.881f, 19.9f, 5.433f, 19.581f, 5.294f },
    { SEG_LINE,  13.256f, 11.119f },
    { SEG_CUBIC, 13.256f, 12.881f, 12.619f, 11.706f, 12.619f, 12.294f },
    { SEG_LINE,  18.944f, 18.119f },
    { SEG_CUBIC, 19.9f, 17.7f, 19.581f, 18.706f, 19.9f, 18.567f },
};
constexpr IconFigure kPREVFigs[] = {
    { kPREVSegs, _countof(kPREVSegs), { 11.7f, 6.3f } },
    { kPREVSegs1, _countof(kPREVSegs1), { 19.9f, 6.3f } },
};

// --- volume ---
constexpr IconSeg kVOLUMESegs[] = {
    { SEG_CUBIC, 17.9f, 8.188f, 19.0f, 10.592f, 18.633f, 9.329f },
    { SEG_CUBIC, 14.95f, 5.625f, 17.167f, 7.046f, 16.183f, 6.192f },
    { SEG_CUBIC, 14.4f, 5.088f, 14.7f, 5.508f, 14.517f, 5.329f },
    { SEG_CUBIC, 14.35f, 4.35f, 14.283f, 4.846f, 14.267f, 4.6f },
    { SEG_CUBIC, 14.888f, 3.775f, 14.45f, 4.083f, 14.629f, 3.892f },
    { SEG_CUBIC, 15.675f, 3.775f, 15.146f, 3.658f, 15.408f, 3.658f },
    { SEG_CUBIC, 19.55f, 7.063f, 17.292f, 4.492f, 18.583f, 5.588f },
    { SEG_CUBIC, 21.0f, 11.975f, 20.517f, 8.538f, 21.0f, 10.175f },
    { SEG_CUBIC, 19.55f, 16.888f, 21.0f, 13.775f, 20.517f, 15.413f },
    { SEG_CUBIC, 15.675f, 20.175f, 18.583f, 18.363f, 17.292f, 19.458f },
    { SEG_CUBIC, 14.888f, 20.175f, 15.408f, 20.292f, 15.146f, 20.292f },
    { SEG_CUBIC, 14.35f, 19.6f, 14.629f, 20.058f, 14.45f, 19.867f },
    { SEG_CUBIC, 14.4f, 18.863f, 14.267f, 19.35f, 14.283f, 19.104f },
    { SEG_CUBIC, 14.95f, 18.325f, 14.517f, 18.621f, 14.7f, 18.442f },
    { SEG_CUBIC, 17.9f, 15.763f, 16.183f, 17.758f, 17.167f, 16.904f },
    { SEG_CUBIC, 19.0f, 11.975f, 18.633f, 14.621f, 19.0f, 13.358f },
};
constexpr IconSeg kVOLUMESegs1[] = {
    { SEG_LINE,  4.0f, 15.0f },
    { SEG_CUBIC, 3.288f, 14.713f, 3.717f, 15.0f, 3.479f, 14.904f },
    { SEG_CUBIC, 3.0f, 14.0f, 3.096f, 14.521f, 3.0f, 14.283f },
    { SEG_LINE,  3.0f, 10.0f },
    { SEG_CUBIC, 3.288f, 9.288f, 3.0f, 9.717f, 3.096f, 9.479f },
    { SEG_CUBIC, 4.0f, 9.0f, 3.479f, 9.096f, 3.717f, 9.0f },
    { SEG_LINE,  7.0f, 9.0f },
    { SEG_LINE,  10.3f, 5.7f },
    { SEG_CUBIC, 11.388f, 5.488f, 10.617f, 5.383f, 10.979f, 5.313f },
    { SEG_CUBIC, 12.0f, 6.425f, 11.796f, 5.663f, 12.0f, 5.975f },
    { SEG_LINE,  12.0f, 17.575f },
    { SEG_CUBIC, 11.388f, 18.513f, 12.0f, 18.025f, 11.796f, 18.338f },
    { SEG_CUBIC, 10.3f, 18.3f, 10.979f, 18.688f, 10.617f, 18.617f },
    { SEG_LINE,  7.0f, 15.0f },
};
constexpr IconSeg kVOLUMESegs2[] = {
    { SEG_CUBIC, 16.025f, 13.988f, 16.5f, 12.7f, 16.342f, 13.363f },
    { SEG_CUBIC, 14.775f, 15.525f, 15.708f, 14.613f, 15.292f, 15.125f },
    { SEG_CUBIC, 14.263f, 15.538f, 14.608f, 15.625f, 14.438f, 15.629f },
    { SEG_CUBIC, 14.0f, 15.1f, 14.088f, 15.446f, 14.0f, 15.3f },
    { SEG_LINE,  14.0f, 8.85f },
    { SEG_CUBIC, 14.263f, 8.413f, 14.0f, 8.65f, 14.088f, 8.504f },
    { SEG_CUBIC, 14.775f, 8.425f, 14.438f, 8.321f, 14.608f, 8.325f },
    { SEG_CUBIC, 16.025f, 10.0f, 15.292f, 8.842f, 15.708f, 9.367f },
    { SEG_CUBIC, 16.5f, 12.0f, 16.342f, 10.633f, 16.5f, 11.3f },
};
constexpr IconFigure kVOLUMEFigs[] = {
    { kVOLUMESegs, _countof(kVOLUMESegs), { 19.0f, 11.975f } },
    { kVOLUMESegs1, _countof(kVOLUMESegs1), { 7.0f, 15.0f } },
    { kVOLUMESegs2, _countof(kVOLUMESegs2), { 16.5f, 12.0f } },
};

// --- capslock ---
constexpr IconSeg kCAPSLOCKSegs[] = {
    { SEG_CUBIC, 6.288f, 17.713f, 6.717f, 18.0f, 6.479f, 17.904f },
    { SEG_CUBIC, 6.0f, 17.0f, 6.096f, 17.521f, 6.0f, 17.283f },
    { SEG_CUBIC, 6.288f, 16.288f, 6.0f, 16.717f, 6.096f, 16.479f },
    { SEG_CUBIC, 7.0f, 16.0f, 6.479f, 16.096f, 6.717f, 16.0f },
    { SEG_LINE,  17.0f, 16.0f },
    { SEG_CUBIC, 17.713f, 16.288f, 17.283f, 16.0f, 17.521f, 16.096f },
    { SEG_CUBIC, 18.0f, 17.0f, 17.904f, 16.479f, 18.0f, 16.717f },
    { SEG_CUBIC, 17.713f, 17.713f, 18.0f, 17.283f, 17.904f, 17.521f },
    { SEG_CUBIC, 17.0f, 18.0f, 17.521f, 17.904f, 17.283f, 18.0f },
    { SEG_LINE,  7.0f, 18.0f },
};
constexpr IconSeg kCAPSLOCKSegs1[] = {
    { SEG_LINE,  8.1f, 12.3f },
    { SEG_CUBIC, 7.4f, 12.575f, 7.917f, 12.483f, 7.683f, 12.575f },
    { SEG_CUBIC, 6.7f, 12.3f, 7.117f, 12.575f, 6.883f, 12.483f },
    { SEG_CUBIC, 6.425f, 11.6f, 6.517f, 12.117f, 6.425f, 11.883f },
    { SEG_CUBIC, 6.7f, 10.9f, 6.425f, 11.317f, 6.517f, 11.083f },
    { SEG_LINE,  11.3f, 6.3f },
    { SEG_CUBIC, 12.0f, 6.0f, 11.5f, 6.1f, 11.733f, 6.0f },
    { SEG_CUBIC, 12.7f, 6.3f, 12.267f, 6.0f, 12.5f, 6.1f },
    { SEG_LINE,  17.3f, 10.9f },
    { SEG_CUBIC, 17.575f, 11.6f, 17.483f, 11.083f, 17.575f, 11.317f },
    { SEG_CUBIC, 17.3f, 12.3f, 17.575f, 11.883f, 17.483f, 12.117f },
    { SEG_CUBIC, 16.6f, 12.575f, 17.117f, 12.483f, 16.883f, 12.575f },
    { SEG_CUBIC, 15.9f, 12.3f, 16.317f, 12.575f, 16.083f, 12.483f },
    { SEG_LINE,  12.0f, 8.4f },
};
constexpr IconFigure kCAPSLOCKFigs[] = {
    { kCAPSLOCKSegs, _countof(kCAPSLOCKSegs), { 7.0f, 18.0f } },
    { kCAPSLOCKSegs1, _countof(kCAPSLOCKSegs1), { 12.0f, 8.4f } },
};

// --- bolt ---
constexpr IconSeg kBOLTSegs[] = {
    { SEG_LINE,  5.9f, 15.0f },
    { SEG_CUBIC, 5.013f, 14.463f, 5.5f, 15.0f, 5.204f, 14.821f },
    { SEG_CUBIC, 5.075f, 13.425f, 4.821f, 14.104f, 4.842f, 13.758f },
    { SEG_LINE,  12.55f, 2.675f },
    { SEG_CUBIC, 13.2f, 2.188f, 12.717f, 2.442f, 12.933f, 2.279f },
    { SEG_CUBIC, 14.025f, 2.2f, 13.467f, 2.096f, 13.742f, 2.1f },
    { SEG_CUBIC, 14.65f, 2.725f, 14.308f, 2.3f, 14.517f, 2.475f },
    { SEG_CUBIC, 14.8f, 3.525f, 14.783f, 2.975f, 14.833f, 3.242f },
    { SEG_LINE,  14.0f, 10.0f },
    { SEG_LINE,  17.875f, 10.0f },
    { SEG_CUBIC, 18.788f, 10.575f, 18.308f, 10.0f, 18.613f, 10.192f },
    { SEG_CUBIC, 18.625f, 11.65f, 18.963f, 10.958f, 18.908f, 11.317f },
    { SEG_LINE,  10.4f, 21.5f },
    { SEG_CUBIC, 9.725f, 21.925f, 10.217f, 21.717f, 9.992f, 21.858f },
    { SEG_CUBIC, 8.95f, 21.85f, 9.458f, 21.992f, 9.2f, 21.967f },
    { SEG_CUBIC, 8.363f, 21.313f, 8.7f, 21.733f, 8.504f, 21.554f },
    { SEG_CUBIC, 8.2f, 20.525f, 8.221f, 21.071f, 8.167f, 20.808f },
    { SEG_LINE,  9.0f, 15.0f },
};
constexpr IconFigure kBOLTFigs[] = {
    { kBOLTSegs, _countof(kBOLTSegs), { 9.0f, 15.0f } },
};

// --- bluetooth ---
constexpr IconSeg kBLUETOOTHSegs[] = {
    { SEG_LINE,  11.0f, 14.4f },
    { SEG_LINE,  7.1f, 18.3f },
    { SEG_CUBIC, 6.4f, 18.575f, 6.917f, 18.483f, 6.683f, 18.575f },
    { SEG_CUBIC, 5.7f, 18.3f, 6.117f, 18.575f, 5.883f, 18.483f },
    { SEG_CUBIC, 5.425f, 17.6f, 5.517f, 18.117f, 5.425f, 17.883f },
    { SEG_CUBIC, 5.7f, 16.9f, 5.425f, 17.317f, 5.517f, 17.083f },
    { SEG_LINE,  10.6f, 12.0f },
    { SEG_LINE,  5.7f, 7.1f },
    { SEG_CUBIC, 5.425f, 6.4f, 5.517f, 6.917f, 5.425f, 6.683f },
    { SEG_CUBIC, 5.7f, 5.7f, 5.425f, 6.117f, 5.517f, 5.883f },
    { SEG_CUBIC, 6.4f, 5.425f, 5.883f, 5.517f, 6.117f, 5.425f },
    { SEG_CUBIC, 7.1f, 5.7f, 6.683f, 5.425f, 6.917f, 5.517f },
    { SEG_LINE,  11.0f, 9.6f },
    { SEG_LINE,  11.0f, 3.425f },
    { SEG_CUBIC, 11.3f, 2.688f, 11.0f, 3.125f, 11.1f, 2.879f },
    { SEG_CUBIC, 12.0f, 2.4f, 11.5f, 2.496f, 11.733f, 2.4f },
    { SEG_CUBIC, 12.375f, 2.475f, 12.133f, 2.4f, 12.258f, 2.425f },
    { SEG_CUBIC, 12.7f, 2.7f, 12.492f, 2.525f, 12.6f, 2.6f },
    { SEG_LINE,  17.0f, 7.0f },
    { SEG_CUBIC, 17.213f, 7.325f, 17.1f, 7.1f, 17.171f, 7.208f },
    { SEG_CUBIC, 17.275f, 7.7f, 17.254f, 7.442f, 17.275f, 7.567f },
    { SEG_CUBIC, 17.213f, 8.075f, 17.275f, 7.833f, 17.254f, 7.958f },
    { SEG_CUBIC, 17.0f, 8.4f, 17.171f, 8.192f, 17.1f, 8.3f },
    { SEG_LINE,  13.4f, 12.0f },
    { SEG_LINE,  17.0f, 15.6f },
    { SEG_CUBIC, 17.213f, 15.925f, 17.1f, 15.7f, 17.171f, 15.808f },
    { SEG_CUBIC, 17.275f, 16.3f, 17.254f, 16.042f, 17.275f, 16.167f },
    { SEG_CUBIC, 17.213f, 16.675f, 17.275f, 16.433f, 17.254f, 16.558f },
    { SEG_CUBIC, 17.0f, 17.0f, 17.171f, 16.792f, 17.1f, 16.9f },
    { SEG_LINE,  12.7f, 21.3f },
    { SEG_CUBIC, 12.375f, 21.525f, 12.6f, 21.4f, 12.492f, 21.475f },
    { SEG_CUBIC, 12.0f, 21.6f, 12.258f, 21.575f, 12.133f, 21.6f },
    { SEG_CUBIC, 11.3f, 21.313f, 11.733f, 21.6f, 11.5f, 21.504f },
    { SEG_CUBIC, 11.0f, 20.575f, 11.1f, 21.121f, 11.0f, 20.875f },
};
constexpr IconFigure kBLUETOOTHFigs[] = {
    { kBLUETOOTHSegs, _countof(kBLUETOOTHSegs), { 11.0f, 20.575f } },
};

// --- battery ---
constexpr IconSeg kBATTERYSegs[] = {
    { SEG_CUBIC, 7.288f, 21.713f, 7.717f, 22.0f, 7.479f, 21.904f },
    { SEG_CUBIC, 7.0f, 21.0f, 7.096f, 21.521f, 7.0f, 21.283f },
    { SEG_LINE,  7.0f, 5.0f },
    { SEG_CUBIC, 7.288f, 4.288f, 7.0f, 4.717f, 7.096f, 4.479f },
    { SEG_CUBIC, 8.0f, 4.0f, 7.479f, 4.096f, 7.717f, 4.0f },
    { SEG_LINE,  10.0f, 4.0f },
    { SEG_LINE,  10.0f, 3.0f },
    { SEG_CUBIC, 10.288f, 2.288f, 10.0f, 2.717f, 10.096f, 2.479f },
    { SEG_CUBIC, 11.0f, 2.0f, 10.479f, 2.096f, 10.717f, 2.0f },
    { SEG_LINE,  13.0f, 2.0f },
    { SEG_CUBIC, 13.713f, 2.288f, 13.283f, 2.0f, 13.521f, 2.096f },
    { SEG_CUBIC, 14.0f, 3.0f, 13.904f, 2.479f, 14.0f, 2.717f },
    { SEG_LINE,  14.0f, 4.0f },
    { SEG_LINE,  16.0f, 4.0f },
    { SEG_CUBIC, 16.713f, 4.288f, 16.283f, 4.0f, 16.521f, 4.096f },
    { SEG_CUBIC, 17.0f, 5.0f, 16.904f, 4.479f, 17.0f, 4.717f },
    { SEG_LINE,  17.0f, 21.0f },
    { SEG_CUBIC, 16.713f, 21.713f, 17.0f, 21.283f, 16.904f, 21.521f },
    { SEG_CUBIC, 16.0f, 22.0f, 16.521f, 21.904f, 16.283f, 22.0f },
    { SEG_LINE,  8.0f, 22.0f },
};
constexpr IconFigure kBATTERYFigs[] = {
    { kBATTERYSegs, _countof(kBATTERYSegs), { 8.0f, 22.0f } },
};

// --- battery (horizontal) ---
// Same solid battery glyph rotated to horizontal (cap on the right) by the
// coordinate transform (x, y) -> (24 - y, x); matches the iOS charging pill.
constexpr IconSeg kBATTERYHSegs[] = {
    { SEG_CUBIC, 2.287f, 7.288f, 2.0f, 7.717f, 2.096f, 7.479f },
    { SEG_CUBIC, 3.0f, 7.0f, 2.479f, 7.096f, 2.717f, 7.0f },
    { SEG_LINE,  19.0f, 7.0f },
    { SEG_CUBIC, 19.712f, 7.288f, 19.283f, 7.0f, 19.521f, 7.096f },
    { SEG_CUBIC, 20.0f, 8.0f, 19.904f, 7.479f, 20.0f, 7.717f },
    { SEG_LINE,  20.0f, 10.0f },
    { SEG_LINE,  21.0f, 10.0f },
    { SEG_CUBIC, 21.712f, 10.288f, 21.283f, 10.0f, 21.521f, 10.096f },
    { SEG_CUBIC, 22.0f, 11.0f, 21.904f, 10.479f, 22.0f, 10.717f },
    { SEG_LINE,  22.0f, 13.0f },
    { SEG_CUBIC, 21.712f, 13.713f, 22.0f, 13.283f, 21.904f, 13.521f },
    { SEG_CUBIC, 21.0f, 14.0f, 21.521f, 13.904f, 21.283f, 14.0f },
    { SEG_LINE,  20.0f, 14.0f },
    { SEG_LINE,  20.0f, 16.0f },
    { SEG_CUBIC, 19.712f, 16.713f, 20.0f, 16.283f, 19.904f, 16.521f },
    { SEG_CUBIC, 19.0f, 17.0f, 19.521f, 16.904f, 19.283f, 17.0f },
    { SEG_LINE,  3.0f, 17.0f },
    { SEG_CUBIC, 2.287f, 16.713f, 2.717f, 17.0f, 2.479f, 16.904f },
    { SEG_CUBIC, 2.0f, 16.0f, 2.096f, 16.521f, 2.0f, 16.283f },
    { SEG_LINE,  2.0f, 8.0f },
};
constexpr IconFigure kBATTERYHFigs[] = {
    { kBATTERYHSegs, _countof(kBATTERYHSegs), { 2.0f, 8.0f } },
};

// --- check ---
constexpr IconSeg kCHECKSegs[] = {
    { SEG_LINE,  18.025f, 6.675f },
    { SEG_CUBIC, 18.725f, 6.375f, 18.225f, 6.475f, 18.458f, 6.375f },
    { SEG_CUBIC, 19.425f, 6.675f, 18.992f, 6.375f, 19.225f, 6.475f },
    { SEG_CUBIC, 19.725f, 7.388f, 19.625f, 6.875f, 19.725f, 7.113f },
    { SEG_CUBIC, 19.425f, 8.1f, 19.725f, 7.663f, 19.625f, 7.9f },
    { SEG_LINE,  10.25f, 17.3f },
    { SEG_CUBIC, 9.55f, 17.6f, 10.05f, 17.5f, 9.817f, 17.6f },
    { SEG_CUBIC, 8.85f, 17.3f, 9.283f, 17.6f, 9.05f, 17.5f },
    { SEG_LINE,  4.55f, 13.0f },
    { SEG_CUBIC, 4.263f, 12.288f, 4.35f, 12.8f, 4.254f, 12.563f },
    { SEG_CUBIC, 4.575f, 11.575f, 4.271f, 12.013f, 4.375f, 11.775f },
    { SEG_CUBIC, 5.288f, 11.275f, 4.775f, 11.375f, 5.013f, 11.275f },
    { SEG_CUBIC, 6.0f, 11.575f, 5.563f, 11.275f, 5.8f, 11.375f },
    { SEG_LINE,  9.55f, 15.15f },
};
constexpr IconFigure kCHECKFigs[] = {
    { kCHECKSegs, _countof(kCHECKSegs), { 9.55f, 15.15f } },
};

// Order matches the IconType enum declaration.
const IconGlyph kGlyphs[] = {
    { kPLAYFigs, _countof(kPLAYFigs) },        // Play
    { kPAUSEFigs, _countof(kPAUSEFigs) },      // Pause
    { kPREVFigs, _countof(kPREVFigs) },        // Previous
    { kNEXTFigs, _countof(kNEXTFigs) },        // Next
    { kVOLUMEFigs, _countof(kVOLUMEFigs) },    // Volume
    { kCAPSLOCKFigs, _countof(kCAPSLOCKFigs) },// CapsLock
    { kBOLTFigs, _countof(kBOLTFigs) },        // Bolt
    { kBLUETOOTHFigs, _countof(kBLUETOOTHFigs) }, // Bluetooth
    { kBATTERYFigs, _countof(kBATTERYFigs) },  // Battery
    { kCHECKFigs, _countof(kCHECKFigs) },      // Checkmark
    { kBATTERYHFigs, _countof(kBATTERYHFigs) },// BatteryHorizontal
};

void DrawIconGlyph(
    ID2D1RenderTarget* rt,
    ID2D1Factory* factory,
    ID2D1SolidColorBrush* brush,
    const IconGlyph& glyph,
    const D2D1_RECT_F& bounds
) {
    if (!rt || !factory || !brush || glyph.count == 0) return;

    const float scaleX = (bounds.right - bounds.left) / 24.0f;
    const float scaleY = (bounds.bottom - bounds.top) / 24.0f;

    Microsoft::WRL::ComPtr<ID2D1PathGeometry> path;
    if (FAILED(factory->CreatePathGeometry(path.GetAddressOf()))) return;

    Microsoft::WRL::ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(path->Open(sink.GetAddressOf()))) return;

    for (size_t f = 0; f < glyph.count; ++f) {
        const IconFigure& fig = glyph.figures[f];
        if (!fig.segs || fig.count == 0) continue;

        sink->BeginFigure(
            D2D1::Point2F(ScaleX(fig.start.x, bounds), ScaleY(fig.start.y, bounds)),
            D2D1_FIGURE_BEGIN_FILLED);

        for (size_t i = 0; i < fig.count; ++i) {
            const IconSeg& s = fig.segs[i];
            const D2D1_POINT_2F end = Pt(s.ex, s.ey, bounds);

            switch (s.kind) {
                case SEG_LINE:
                    sink->AddLine(end);
                    break;
                case SEG_CUBIC:
                    sink->AddBezier(D2D1::BezierSegment(
                        Pt(s.c1x, s.c1y, bounds),
                        Pt(s.c2x, s.c2y, bounds),
                        end));
                    break;
                case SEG_ARC: {
                    D2D1_ARC_SEGMENT arc{};
                    arc.point = end;
                    arc.size = D2D1::SizeF(
                        (std::max)(0.01f, s.rx * scaleX),
                        (std::max)(0.01f, s.ry * scaleY));
                    arc.rotationAngle = s.rot;
                    arc.sweepDirection = s.sweep
                        ? D2D1_SWEEP_DIRECTION_CLOCKWISE
                        : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE;
                    arc.arcSize = s.large ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL;
                    sink->AddArc(arc);
                    break;
                }
            }
        }

        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    }

    if (SUCCEEDED(sink->Close())) {
        rt->FillGeometry(path.Get(), brush);
    }
}

} // anonymous namespace

void DrawIcon(
    ID2D1RenderTarget* rt,
    ID2D1Factory* factory,
    ID2D1SolidColorBrush* brush,
    IconType type,
    const D2D1_RECT_F& bounds,
    D2D1_COLOR_F color
) {
    if (!rt || !factory || !brush) return;
    const size_t index = static_cast<size_t>(type);
    if (index >= _countof(kGlyphs)) return;
    brush->SetColor(color);
    DrawIconGlyph(rt, factory, brush, kGlyphs[index], bounds);
}
} // namespace Graphics
} // namespace DynamicIsland
// ============================================================================
// [Module] src\graphics\d2d_renderer.cpp
// ============================================================================
namespace DynamicIsland {
namespace Graphics {

D2DRenderer::D2DRenderer() = default;

D2DRenderer::~D2DRenderer() {
    Cleanup();
}

bool D2DRenderer::Initialize(HWND hwnd) {
    m_hwnd = hwnd;
    
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_d2dFactory.GetAddressOf());
    if (FAILED(hr)) {
        LogError(L"Failed to create Direct2D Factory, hr=0x%08X", hr);
        return false;
    }
    
    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(m_dwriteFactory.GetAddressOf())
    );
    if (FAILED(hr)) {
        LogError(L"Failed to create DirectWrite Factory, hr=0x%08X", hr);
        return false;
    }
    
    std::wstring fontFamily;
    // Keep regular and emphasized text formats distinct so calls with
    // bold=true (including the media track title) are actually bold.
    DWRITE_FONT_WEIGHT weightRegular = DWRITE_FONT_WEIGHT_MEDIUM;
    DWRITE_FONT_WEIGHT weightBold = DWRITE_FONT_WEIGHT_BOLD;
    float sizeRegular = 12.0f;
    float sizeBig     = 32.0f;
    float sizeSmall   = 11.0f;
    
    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        fontFamily = g_settings.fontFamily;
        
        auto toDwriteWeight = [](FontWeightPreset p) -> DWRITE_FONT_WEIGHT {
            switch (p) {
                case FontWeightPreset::Thin:      return DWRITE_FONT_WEIGHT_THIN;
                case FontWeightPreset::Light:     return DWRITE_FONT_WEIGHT_LIGHT;
                case FontWeightPreset::Regular:   return DWRITE_FONT_WEIGHT_NORMAL;
                case FontWeightPreset::Medium:    return DWRITE_FONT_WEIGHT_MEDIUM;
                case FontWeightPreset::SemiBold:  return DWRITE_FONT_WEIGHT_SEMI_BOLD;
                case FontWeightPreset::Bold:      return DWRITE_FONT_WEIGHT_BOLD;
                default:                          return DWRITE_FONT_WEIGHT_MEDIUM;
            }
        };
        
        weightRegular = toDwriteWeight(g_settings.fontWeightRegular);
        weightBold = toDwriteWeight(g_settings.fontWeightBold);
        sizeRegular = static_cast<float>(g_settings.fontSizeRegular);
        sizeBig     = static_cast<float>(g_settings.fontSizeBig);
        sizeSmall   = static_cast<float>(g_settings.fontSizeSmall);
    }
    m_fontSizeRegular = sizeRegular;
    m_fontSizeSmall = sizeSmall;
    m_fontSizeBig = sizeBig;

    if (fontFamily.empty()) {
        fontFamily = Utils::GetSystemUiFontFamily();
    } else {
        bool installed = false;
        ComPtr<IDWriteFontCollection> collection;
        if (SUCCEEDED(m_dwriteFactory->GetSystemFontCollection(collection.GetAddressOf()))) {
            UINT32 idx = 0;
            BOOL exists = FALSE;
            if (SUCCEEDED(collection->FindFamilyName(fontFamily.c_str(), &idx, &exists)) && exists) {
                installed = true;
            }
        }
        if (!installed) {
            fontFamily = Utils::GetSystemUiFontFamily();
        }
    }
    
    // Final validation: the resolved family (including the auto-detected one)
    // must be loadable by DirectWrite, otherwise D2D would silently draw
    // nothing for those runs.
    if (!fontFamily.empty()) {
        ComPtr<IDWriteFontCollection> collection;
        if (SUCCEEDED(m_dwriteFactory->GetSystemFontCollection(collection.GetAddressOf()))) {
            UINT32 idx = 0;
            BOOL exists = FALSE;
            if (SUCCEEDED(collection->FindFamilyName(fontFamily.c_str(), &idx, &exists)) && !exists) {
                fontFamily = L"Segoe UI";
            }
        }
    }
    
    hr = m_dwriteFactory->CreateTextFormat(
        fontFamily.c_str(),
        nullptr,
        weightRegular,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        sizeRegular,
        L"en-US",
        m_textFormatRegular.GetAddressOf()
    );
    
    if (SUCCEEDED(hr)) {
        m_dwriteFactory->CreateTextFormat(
            fontFamily.c_str(),
            nullptr,
            weightBold,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            sizeRegular,
            L"en-US",
            m_textFormatBold.GetAddressOf()
        );
    }
    
    if (m_dwriteFactory) {
        m_dwriteFactory->CreateTextFormat(
            fontFamily.c_str(),
            nullptr,
            weightRegular,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            sizeBig,
            L"en-US",
            m_textFormatBig.GetAddressOf()
        );
        m_dwriteFactory->CreateTextFormat(
            fontFamily.c_str(),
            nullptr,
            weightRegular,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            sizeSmall,
            L"en-US",
            m_textFormatSmall.GetAddressOf()
        );
    }
    
    auto configureNoWrap = [](IDWriteTextFormat* format) {
        if (!format) return;
        // No wrapping, no ellipsis: overflowing text is simply clipped at the
        // pill edge (the iOS island does the same, no trailing "...").
        format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    };
    
    configureNoWrap(m_textFormatRegular.Get());
    configureNoWrap(m_textFormatBold.Get());
    configureNoWrap(m_textFormatSmall.Get());
    configureNoWrap(m_textFormatBig.Get());
    
    m_memDC = CreateCompatibleDC(nullptr);
    if (!m_memDC) {
        LogError(L"Failed to create compatible memory DC for layered window");
        return false;
    }
    
    return CreateDeviceResources();
}

bool D2DRenderer::CreateDeviceResources() {
    if (m_dcRenderTarget) return true;
    
    D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
    );
    
    HRESULT hr = m_d2dFactory->CreateDCRenderTarget(
        &rtProps,
        m_dcRenderTarget.GetAddressOf()
    );
    if (FAILED(hr)) {
        LogError(L"Failed to create DCRenderTarget, hr=0x%08X", hr);
        return false;
    }
    
    m_dcRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    m_dcRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    
    {
        ComPtr<IDWriteFactory2> factory2;
        if (SUCCEEDED(m_dwriteFactory.As(&factory2)) && factory2) {
            ComPtr<IDWriteRenderingParams2> rp2;
            if (SUCCEEDED(factory2->CreateCustomRenderingParams(
                1.0f, 0.5f, 0.5f, 0.0f,
                DWRITE_PIXEL_GEOMETRY_FLAT,
                DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC,
                DWRITE_GRID_FIT_MODE_DISABLED,
                rp2.GetAddressOf())) && rp2) {
                m_dcRenderTarget->SetTextRenderingParams(rp2.Get());
            }
        } else {
            ComPtr<IDWriteRenderingParams> rp;
            if (SUCCEEDED(m_dwriteFactory->CreateCustomRenderingParams(
                1.0f, 0.5f, 0.0f,
                DWRITE_PIXEL_GEOMETRY_FLAT,
                DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC,
                rp.GetAddressOf())) && rp) {
                m_dcRenderTarget->SetTextRenderingParams(rp.Get());
            }
        }
    }
    
    hr = m_dcRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White),
        m_solidBrush.GetAddressOf()
    );
    if (FAILED(hr) || !m_solidBrush) {
        LogError(L"Failed to create Direct2D solid brush (HRESULT=0x%08X)",
                 static_cast<unsigned>(hr));
        DiscardDeviceResources();
        return false;
    }

    return true;
}

void D2DRenderer::DiscardDeviceResources() {
    // Device resources are discarded only after EndDraw reports a recreate
    // target condition, but reset defensively for all cleanup paths.
    m_isDrawing = false;
    m_albumArtBrush.Reset();
    m_albumArtBitmap.Reset();
    m_solidBrush.Reset();
    m_dcRenderTarget.Reset();
}

void D2DRenderer::Cleanup() {
    DiscardDeviceResources();
    m_marqueeCache.clear();
    m_centeredTextCache.clear();
    
    if (m_hOldBitmap && m_memDC) {
        SelectObject(m_memDC, m_hOldBitmap);
        m_hOldBitmap = nullptr;
    }
    if (m_hBitmap) {
        DeleteObject(m_hBitmap);
        m_hBitmap = nullptr;
    }
    if (m_memDC) {
        DeleteDC(m_memDC);
        m_memDC = nullptr;
    }
    
    m_pixelBits = nullptr;
    m_width = 0;
    m_height = 0;
    
    m_textFormatSmall.Reset();
    m_textFormatBig.Reset();
    m_textFormatBold.Reset();
    m_textFormatRegular.Reset();
    m_dwriteFactory.Reset();
    m_d2dFactory.Reset();
}

void D2DRenderer::Resize(UINT width, UINT height) {
    if (width == 0) width = 1;
    if (height == 0) height = 1;
    
    if (m_width == width && m_height == height && m_hBitmap) {
        return;
    }
    
    m_width = width;
    m_height = height;
    
    if (m_hOldBitmap && m_memDC) {
        SelectObject(m_memDC, m_hOldBitmap);
        m_hOldBitmap = nullptr;
    }
    if (m_hBitmap) {
        DeleteObject(m_hBitmap);
        m_hBitmap = nullptr;
    }
    m_pixelBits = nullptr;
    
    if (!m_memDC) return;
    
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = static_cast<LONG>(width);
    bmi.bmiHeader.biHeight = -static_cast<LONG>(height);
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    
    m_hBitmap = CreateDIBSection(m_memDC, &bmi, DIB_RGB_COLORS, &m_pixelBits, nullptr, 0);
    if (m_hBitmap) {
        m_hOldBitmap = static_cast<HBITMAP>(SelectObject(m_memDC, m_hBitmap));
    }
}

bool D2DRenderer::BeginDraw() {
    if (m_isDrawing) {
        LogError(L"D2D BeginDraw called while a frame is already active");
        return false;
    }

    if (!CreateDeviceResources() || !m_dcRenderTarget || !m_memDC ||
        !m_hBitmap || m_width == 0 || m_height == 0) {
        return false;
    }

    RECT rc = { 0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height) };
    const HRESULT bindHr = m_dcRenderTarget->BindDC(m_memDC, &rc);
    if (FAILED(bindHr)) {
        LogError(L"ID2D1DCRenderTarget::BindDC failed (HRESULT=0x%08X)",
                 static_cast<unsigned>(bindHr));
        return false;
    }

    m_dcRenderTarget->BeginDraw();
    m_isDrawing = true;
    return true;
}

HRESULT D2DRenderer::EndDraw() {
    if (!m_isDrawing || !m_dcRenderTarget) return E_UNEXPECTED;
    m_isDrawing = false;

    HRESULT hr = m_dcRenderTarget->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET) {
        DiscardDeviceResources();
    }
    return hr;
}

bool D2DRenderer::PresentLayeredWindow(HWND hwnd, int winX, int winY) {
    if (!hwnd || !m_memDC || !m_hBitmap || m_width == 0 || m_height == 0) {
        return false;
    }
    
    POINT ptSrc = { 0, 0 };
    SIZE sizeWnd = { static_cast<LONG>(m_width), static_cast<LONG>(m_height) };
    POINT ptDst = { winX, winY };
    
    BLENDFUNCTION blend{};
    blend.BlendOp = AC_SRC_OVER;
    blend.BlendFlags = 0;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;
    
    HDC screenDC = GetDC(nullptr);
    BOOL ok = UpdateLayeredWindow(
        hwnd,
        screenDC,
        &ptDst,
        &sizeWnd,
        m_memDC,
        &ptSrc,
        0,
        &blend,
        ULW_ALPHA
    );
    ReleaseDC(nullptr, screenDC);
    
    return (ok != FALSE);
}

void D2DRenderer::Clear(D2D1_COLOR_F color) {
    if (m_dcRenderTarget) {
        m_dcRenderTarget->Clear(color);
    }
}

void D2DRenderer::DrawRoundedPill(const D2D1_ROUNDED_RECT& pill, D2D1_COLOR_F fillColor, D2D1_COLOR_F borderColor, float strokeWidth) {
    if (!m_dcRenderTarget || !m_solidBrush) return;
    
    m_solidBrush->SetColor(fillColor);
    m_dcRenderTarget->FillRoundedRectangle(pill, m_solidBrush.Get());
    
    if (strokeWidth > 0.0f && borderColor.a > 0.0f) {
        m_solidBrush->SetColor(borderColor);
        m_dcRenderTarget->DrawRoundedRectangle(pill, m_solidBrush.Get(), strokeWidth);
    }
}

void D2DRenderer::DrawTextString(const std::wstring& text, const D2D1_RECT_F& rect, D2D1_COLOR_F color, float fontSize, bool bold) {
    if (!m_dcRenderTarget || !m_solidBrush) return;
    
    IDWriteTextFormat* format = bold ? m_textFormatBold.Get() : m_textFormatRegular.Get();
    const float dSmall = std::abs(fontSize - m_fontSizeSmall);
    const float dRegular = std::abs(fontSize - m_fontSizeRegular);
    const float dBig = std::abs(fontSize - m_fontSizeBig);
    if (dSmall < dRegular && dSmall <= dBig) format = m_textFormatSmall.Get();
    else if (dBig < dRegular && dBig < dSmall) format = m_textFormatBig.Get();
    if (!format) return;

    m_solidBrush->SetColor(color);
    m_dcRenderTarget->DrawText(
        text.c_str(),
        static_cast<UINT32>(text.length()),
        format,
        rect,
        m_solidBrush.Get()
    );
}

void D2DRenderer::DrawTextCentered(const std::wstring& text, const D2D1_RECT_F& rect, D2D1_COLOR_F color, float fontSize, bool bold) {
    if (!m_dcRenderTarget || !m_solidBrush || !m_dwriteFactory || text.empty()) return;

    IDWriteTextFormat* format = bold ? m_textFormatBold.Get() : m_textFormatRegular.Get();
    const float dSmall = std::abs(fontSize - m_fontSizeSmall);
    const float dRegular = std::abs(fontSize - m_fontSizeRegular);
    const float dBig = std::abs(fontSize - m_fontSizeBig);
    if (dSmall < dRegular && dSmall <= dBig) format = m_textFormatSmall.Get();
    else if (dBig < dRegular && dBig < dSmall) format = m_textFormatBig.Get();
    if (!format) return;

    const float layoutWidth = (std::max)(1.0f, rect.right - rect.left);
    const float layoutHeight = (std::max)(1.0f, rect.bottom - rect.top);
    CachedCenteredText* cached = nullptr;
    for (auto& item : m_centeredTextCache) {
        if (item.text == text && item.format == format &&
            std::abs(item.width - layoutWidth) < 0.25f &&
            std::abs(item.height - layoutHeight) < 0.25f) {
            cached = &item;
            break;
        }
    }

    if (!cached) {
        CachedCenteredText item;
        item.text = text;
        item.format = format;
        item.width = layoutWidth;
        item.height = layoutHeight;
        HRESULT hr = m_dwriteFactory->CreateTextLayout(
            text.c_str(), static_cast<UINT32>(text.length()), format,
            layoutWidth, layoutHeight, item.layout.GetAddressOf());
        if (FAILED(hr) || !item.layout) return;

        DWRITE_TEXT_METRICS metrics{};
        hr = item.layout->GetMetrics(&metrics);
        if (FAILED(hr)) return;
        item.textWidth = metrics.width;

        // Bound the cache: time labels and transient messages continually change.
        constexpr size_t kMaxCenteredTextLayouts = 64;
        if (m_centeredTextCache.size() >= kMaxCenteredTextLayouts) {
            m_centeredTextCache.erase(m_centeredTextCache.begin());
        }
        m_centeredTextCache.push_back(std::move(item));
        cached = &m_centeredTextCache.back();
    }

    const float centerX = (rect.left + rect.right) * 0.5f;
    float originX = centerX - cached->textWidth * 0.5f;
    // When the text is wider than the rect, centering would push the draw
    // origin (and its clip window) to the left of rect.left, spilling the
    // glyphs over neighbours like the album art. Left-align overflowing text
    // instead so it simply clips at the right edge.
    if (originX < rect.left) originX = rect.left;
    m_solidBrush->SetColor(color);
    m_dcRenderTarget->DrawTextLayout(
        D2D1::Point2F(originX, rect.top),
        cached->layout.Get(), m_solidBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_CLIP);
}


void D2DRenderer::DrawMarqueeText(const std::wstring& text, const D2D1_RECT_F& rect, D2D1_COLOR_F color, float offset, bool bold) {
    if (!m_dcRenderTarget || !m_solidBrush || !m_dwriteFactory || text.empty()) return;
    
    IDWriteTextFormat* format = bold ? m_textFormatBold.Get() : m_textFormatRegular.Get();
    if (!format) return;
    
    float availW = rect.right - rect.left;
    float availH = rect.bottom - rect.top;
    
    if (availW <= 0.0f || availH <= 0.0f) return;
    
    CachedMarquee* found = nullptr;
    for (auto& item : m_marqueeCache) {
        if (item.text == text && item.bold == bold && std::abs(item.targetHeight - availH) < 0.5f) {
            found = &item;
            break;
        }
    }
    
    if (!found) {
        CachedMarquee newItem;
        newItem.text = text;
        newItem.bold = bold;
        newItem.targetHeight = availH;
        
        HRESULT hr = m_dwriteFactory->CreateTextLayout(
            text.c_str(),
            static_cast<UINT32>(text.length()),
            format,
            10000.0f,
            availH,
            newItem.layout.GetAddressOf()
        );
        if (SUCCEEDED(hr) && newItem.layout) {
            // Guarantee a true bold face for the expanded track title.
            if (bold) newItem.layout->SetFontWeight(DWRITE_FONT_WEIGHT_BOLD, DWRITE_TEXT_RANGE{ 0, static_cast<UINT32>(text.length()) });
            DWRITE_TEXT_METRICS tm;
            newItem.layout->GetMetrics(&tm);
            newItem.textWidth = tm.width;
        }
        
        std::wstring loopText = text + L"    \x2022    ";
        hr = m_dwriteFactory->CreateTextLayout(
            loopText.c_str(),
            static_cast<UINT32>(loopText.length()),
            format,
            10000.0f,
            availH,
            newItem.loopLayout.GetAddressOf()
        );
        if (SUCCEEDED(hr) && newItem.loopLayout) {
            if (bold) newItem.loopLayout->SetFontWeight(DWRITE_FONT_WEIGHT_BOLD, DWRITE_TEXT_RANGE{ 0, static_cast<UINT32>(loopText.length()) });
            DWRITE_TEXT_METRICS loopMetrics;
            newItem.loopLayout->GetMetrics(&loopMetrics);
            newItem.loopUnitWidth = loopMetrics.width;
        }
        
        if (m_marqueeCache.size() >= 8) {
            m_marqueeCache.erase(m_marqueeCache.begin());
        }
        m_marqueeCache.push_back(std::move(newItem));
        found = &m_marqueeCache.back();
    }
    
    if (!found || !found->layout) {
        DrawTextString(text, rect, color, 12.0f, bold);
        return;
    }
    
    m_dcRenderTarget->PushAxisAlignedClip(rect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
    m_solidBrush->SetColor(color);
    
    if (found->textWidth <= availW) {
        m_dcRenderTarget->DrawTextLayout(
            D2D1::Point2F(rect.left, rect.top),
            found->layout.Get(),
            m_solidBrush.Get()
        );
    } else if (found->loopLayout && found->loopUnitWidth > 0.0f) {
        float unitWidth = found->loopUnitWidth;
        float shift = std::fmod(offset, unitWidth);
        if (shift < 0.0f) shift += unitWidth;
        
        m_dcRenderTarget->DrawTextLayout(
            D2D1::Point2F(rect.left - shift, rect.top),
            found->loopLayout.Get(),
            m_solidBrush.Get()
        );
        m_dcRenderTarget->DrawTextLayout(
            D2D1::Point2F(rect.left - shift + unitWidth, rect.top),
            found->loopLayout.Get(),
            m_solidBrush.Get()
        );
    } else {
        m_dcRenderTarget->DrawTextLayout(
            D2D1::Point2F(rect.left, rect.top),
            found->layout.Get(),
            m_solidBrush.Get()
        );
    }
    
    m_dcRenderTarget->PopAxisAlignedClip();
}

void D2DRenderer::DrawProgressBar(const D2D1_RECT_F& barRect, float progressFraction, D2D1_COLOR_F bgColor, D2D1_COLOR_F fgColor) {
    if (!m_dcRenderTarget || !m_solidBrush) return;
    
    float radius = (barRect.bottom - barRect.top) / 2.0f;
    
    D2D1_ROUNDED_RECT bgRounded = D2D1::RoundedRect(barRect, radius, radius);
    m_solidBrush->SetColor(bgColor);
    m_dcRenderTarget->FillRoundedRectangle(bgRounded, m_solidBrush.Get());
    
    if (progressFraction > 0.0f) {
        float fillWidth = (barRect.right - barRect.left) * (progressFraction > 1.0f ? 1.0f : progressFraction);
        D2D1_RECT_F fillRect = D2D1::RectF(barRect.left, barRect.top, barRect.left + fillWidth, barRect.bottom);
        D2D1_ROUNDED_RECT fillRounded = D2D1::RoundedRect(fillRect, radius, radius);
        
        m_solidBrush->SetColor(fgColor);
        m_dcRenderTarget->FillRoundedRectangle(fillRounded, m_solidBrush.Get());
    }
}

void D2DRenderer::DrawProgressRing(D2D1_POINT_2F center, float radius, float progressFraction, D2D1_COLOR_F ringColor, float strokeWidth) {
    if (!m_dcRenderTarget || !m_solidBrush || !m_d2dFactory) return;
    
    m_solidBrush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.2f));
    m_dcRenderTarget->DrawEllipse(D2D1::Ellipse(center, radius, radius), m_solidBrush.Get(), strokeWidth);
    
    if (progressFraction <= 0.0f) return;
    if (progressFraction > 1.0f) progressFraction = 1.0f;
    
    if (progressFraction >= 0.999f) {
        m_solidBrush->SetColor(ringColor);
        m_dcRenderTarget->DrawEllipse(D2D1::Ellipse(center, radius, radius), m_solidBrush.Get(), strokeWidth);
        return;
    }
    
    const float startAngleDeg = -90.0f;
    const float sweepAngleDeg = progressFraction * 360.0f;
    
    auto degToPoint = [&](float deg) -> D2D1_POINT_2F {
        float rad = (startAngleDeg + deg) * (3.14159265358979f / 180.0f);
        return D2D1::Point2F(
            center.x + radius * std::cos(rad),
            center.y + radius * std::sin(rad)
        );
    };
    
    Microsoft::WRL::ComPtr<ID2D1PathGeometry> path;
    if (FAILED(m_d2dFactory->CreatePathGeometry(path.GetAddressOf())) || !path) return;
    
    Microsoft::WRL::ComPtr<ID2D1GeometrySink> sink;
    if (FAILED(path->Open(sink.GetAddressOf())) || !sink) return;
    
    D2D1_POINT_2F startPt = degToPoint(0.0f);
    D2D1_POINT_2F endPt   = degToPoint(sweepAngleDeg);
    
    sink->BeginFigure(startPt, D2D1_FIGURE_BEGIN_HOLLOW);
    
    D2D1_ARC_SEGMENT arcSeg{};
    arcSeg.point        = endPt;
    arcSeg.size         = D2D1::SizeF(radius, radius);
    arcSeg.rotationAngle = 0.0f;
    arcSeg.sweepDirection = D2D1_SWEEP_DIRECTION_CLOCKWISE;
    arcSeg.arcSize      = (sweepAngleDeg > 180.0f) ? D2D1_ARC_SIZE_LARGE : D2D1_ARC_SIZE_SMALL;
    
    sink->AddArc(arcSeg);
    sink->EndFigure(D2D1_FIGURE_END_OPEN);
    sink->Close();
    
    m_solidBrush->SetColor(ringColor);
    m_dcRenderTarget->DrawGeometry(path.Get(), m_solidBrush.Get(), strokeWidth);
}

void D2DRenderer::DrawStatusDot(D2D1_POINT_2F center, float radius, D2D1_COLOR_F color) {
    if (!m_dcRenderTarget || !m_solidBrush) return;
    
    m_solidBrush->SetColor(color);
    m_dcRenderTarget->FillEllipse(D2D1::Ellipse(center, radius, radius), m_solidBrush.Get());
}

void D2DRenderer::DrawEqualizerWaves(D2D1_POINT_2F origin, float height, const float* inputBands, bool playing) {
    if (!m_dcRenderTarget || !m_solidBrush) return;

    m_solidBrush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f));
    const float centerY = origin.y + height / 2.0f;
    constexpr int kBarCount = 6;
    constexpr float kBarWidth = 2.0f;
    constexpr float kBarPitch = 3.2f;
    constexpr float kBarRadius = 1.0f;

    if (!playing || !inputBands) {
        for (int i = 0; i < kBarCount; ++i) {
            const float cx = origin.x + i * kBarPitch + kBarWidth / 2.0f;
            m_dcRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, centerY), kBarRadius, kBarRadius), m_solidBrush.Get());
        }
        return;
    }

    for (int i = 0; i < kBarCount; ++i) {
        float v = Utils::Clamp(inputBands[i], 0.0f, 1.0f);
        // Keep a small idle baseline, with headroom for loud transients.
        const float barH = height * (0.14f + 0.82f * v);
        const float barLeft = origin.x + i * kBarPitch;
        const D2D1_ROUNDED_RECT r = D2D1::RoundedRect(
            D2D1::RectF(barLeft, centerY - barH / 2.0f, barLeft + kBarWidth, centerY + barH / 2.0f),
            kBarRadius, kBarRadius);
        m_dcRenderTarget->FillRoundedRectangle(r, m_solidBrush.Get());
    }
}

void D2DRenderer::DrawBigText(const std::wstring& text, const D2D1_RECT_F& rect, D2D1_COLOR_F color) {
    if (!m_dcRenderTarget || !m_solidBrush) return;
    
    IDWriteTextFormat* format = m_textFormatBig.Get() ? m_textFormatBig.Get() : m_textFormatBold.Get();
    if (!format) return;
    
    m_solidBrush->SetColor(color);
    m_dcRenderTarget->DrawText(
        text.c_str(),
        static_cast<UINT32>(text.length()),
        format,
        rect,
        m_solidBrush.Get()
    );
}

void D2DRenderer::SetAlbumArtBitmap(const uint8_t* pixels, UINT width, UINT height) {
    if (!pixels || width == 0 || height == 0) {
        ClearAlbumArtBitmap();
        return;
    }
    
    CreateDeviceResources();
    if (!m_dcRenderTarget) return;
    
    D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
    );
    
    D2D1_SIZE_U size = D2D1::SizeU(width, height);
    
    m_albumArtBrush.Reset();
    m_albumArtBitmap.Reset();
    HRESULT hr = m_dcRenderTarget->CreateBitmap(size, pixels, width * 4, props, m_albumArtBitmap.GetAddressOf());
    if (SUCCEEDED(hr) && m_albumArtBitmap) {
        D2D1_BITMAP_BRUSH_PROPERTIES brushProps = D2D1::BitmapBrushProperties(
            D2D1_EXTEND_MODE_CLAMP,
            D2D1_EXTEND_MODE_CLAMP,
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
        );
        hr = m_dcRenderTarget->CreateBitmapBrush(
            m_albumArtBitmap.Get(), brushProps, m_albumArtBrush.GetAddressOf());
    }
    if (FAILED(hr)) {
        m_albumArtBrush.Reset();
        m_albumArtBitmap.Reset();
    }
}

void D2DRenderer::ClearAlbumArtBitmap() {
    m_albumArtBrush.Reset();
    m_albumArtBitmap.Reset();
}

void D2DRenderer::DrawAlbumArt(const D2D1_RECT_F& bounds, float cornerRadius, float rotationDegrees) {
    if (!m_dcRenderTarget) return;
    
    float bw = bounds.right - bounds.left;
    float bh = bounds.bottom - bounds.top;
    float side = (bw < bh) ? bw : bh;
    
    float offsetX = bounds.left + (bw - side) / 2.0f;
    float offsetY = bounds.top + (bh - side) / 2.0f;
    
    D2D1_RECT_F squareBounds = D2D1::RectF(offsetX, offsetY, offsetX + side, offsetY + side);
    
    float centerX = (squareBounds.left + squareBounds.right) / 2.0f;
    float centerY = (squareBounds.top + squareBounds.bottom) / 2.0f;
    
    bool hasRotation = std::abs(rotationDegrees) > 0.001f;
    
    if (m_albumArtBitmap) {
        D2D1_SIZE_F bmpSize = m_albumArtBitmap->GetSize();
        if (bmpSize.width > 0.0f && bmpSize.height > 0.0f) {
            float scaleW = side / bmpSize.width;
            float scaleH = side / bmpSize.height;
            float scale = (scaleW > scaleH) ? scaleW : scaleH;
            
            float scaledW = bmpSize.width * scale;
            float scaledH = bmpSize.height * scale;
            
            float tx = offsetX + (side - scaledW) / 2.0f;
            float ty = offsetY + (side - scaledH) / 2.0f;
            
            if (m_albumArtBrush) {
                D2D1::Matrix3x2F brushTransform =
                    D2D1::Matrix3x2F::Scale(scale, scale) *
                    D2D1::Matrix3x2F::Translation(tx, ty);
                
                if (hasRotation) {
                    // Rotate the mapped artwork around its own center. Applying the
                    // rotation before the bitmap mapping rotates the texture in the
                    // wrong coordinate space and makes the cover appear to orbit.
                    brushTransform = brushTransform *
                        D2D1::Matrix3x2F::Rotation(rotationDegrees, D2D1::Point2F(centerX, centerY));
                }
                
                m_albumArtBrush->SetTransform(brushTransform);

                D2D1_ROUNDED_RECT r = D2D1::RoundedRect(squareBounds, cornerRadius, cornerRadius);
                m_dcRenderTarget->FillRoundedRectangle(r, m_albumArtBrush.Get());
                return;
            }
        }
    }
    
    D2D1_GRADIENT_STOP stops[2];
    stops[0].position = 0.0f;
    stops[0].color = D2D1::ColorF(1.0f, 0.478f, 0.722f, 1.0f);
    stops[1].position = 1.0f;
    stops[1].color = D2D1::ColorF(0.416f, 0.361f, 1.0f, 1.0f);
    
    ComPtr<ID2D1GradientStopCollection> stopCollection;
    if (SUCCEEDED(m_dcRenderTarget->CreateGradientStopCollection(stops, 2, stopCollection.GetAddressOf()))) {
        ComPtr<ID2D1LinearGradientBrush> gradBrush;
        D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gradProps = D2D1::LinearGradientBrushProperties(
            D2D1::Point2F(squareBounds.left, squareBounds.top),
            D2D1::Point2F(squareBounds.right, squareBounds.bottom)
        );
        
        if (SUCCEEDED(m_dcRenderTarget->CreateLinearGradientBrush(gradProps, stopCollection.Get(), gradBrush.GetAddressOf()))) {
            if (hasRotation) {
                gradBrush->SetTransform(
                    D2D1::Matrix3x2F::Rotation(rotationDegrees, D2D1::Point2F(centerX, centerY))
                );
            }
            
            D2D1_ROUNDED_RECT r = D2D1::RoundedRect(squareBounds, cornerRadius, cornerRadius);
            m_dcRenderTarget->FillRoundedRectangle(r, gradBrush.Get());
        }
    }
}

void D2DRenderer::DrawIconImage(IconType type, const D2D1_RECT_F& bounds, D2D1_COLOR_F color) {
    if (!m_dcRenderTarget || !m_d2dFactory || !m_solidBrush) return;
    DrawIcon(m_dcRenderTarget.Get(), m_d2dFactory.Get(), m_solidBrush.Get(), type, bounds, color);
}

void D2DRenderer::DrawButtonPill(const D2D1_RECT_F& bounds, const std::wstring& text, bool isHovered) {
    if (!m_dcRenderTarget || !m_solidBrush) return;
    
    float radius = (bounds.bottom - bounds.top) / 2.0f;
    D2D1_ROUNDED_RECT pill = D2D1::RoundedRect(bounds, radius, radius);
    
    if (isHovered) {
        m_solidBrush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.15f));
        m_dcRenderTarget->FillRoundedRectangle(pill, m_solidBrush.Get());
    }
    
    m_solidBrush->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.25f));
    m_dcRenderTarget->DrawRoundedRectangle(pill, m_solidBrush.Get(), 1.0f);
    
    m_solidBrush->SetColor(D2D1::ColorF(D2D1::ColorF::White));
    
    IDWriteTextFormat* format = m_textFormatRegular.Get();
    if (format) {
        format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        m_dcRenderTarget->DrawText(text.c_str(), static_cast<UINT32>(text.length()), format, bounds, m_solidBrush.Get());
        format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    }
}

void D2DRenderer::DrawCircularButton(const D2D1_RECT_F& bounds, IconType icon, float iconScale, float hoverProgress) {
    if (!m_dcRenderTarget || !m_solidBrush) return;
    
    float w = bounds.right - bounds.left;
    float h = bounds.bottom - bounds.top;
    
    D2D1_POINT_2F center = D2D1::Point2F(bounds.left + w / 2.0f, bounds.top + h / 2.0f);
    hoverProgress = Utils::Clamp(hoverProgress, 0.0f, 1.0f);

    // Hover feedback is icon-only: no circular background is drawn.
    float iconSize = 22.0f * iconScale * (1.0f + 0.045f * hoverProgress);
    D2D1_RECT_F iconBounds = D2D1::RectF(
        center.x - iconSize / 2.0f,
        center.y - iconSize / 2.0f,
        center.x + iconSize / 2.0f,
        center.y + iconSize / 2.0f
    );
    
    DrawIconImage(icon, iconBounds, D2D1::ColorF(D2D1::ColorF::White));
}

void D2DRenderer::DrawCircularButtonTransition(
    const D2D1_RECT_F& bounds,
    IconType fromIcon,
    IconType toIcon,
    float transitionProgress,
    float iconScale,
    float hoverProgress
) {
    if (!m_dcRenderTarget || !m_solidBrush) return;

    const float w = bounds.right - bounds.left;
    const float h = bounds.bottom - bounds.top;
    const D2D1_POINT_2F center = D2D1::Point2F(bounds.left + w / 2.0f, bounds.top + h / 2.0f);
    hoverProgress = Utils::Clamp(hoverProgress, 0.0f, 1.0f);

    // Keep the hover animation on the glyph only; do not draw a circular backdrop.
    const float baseIconSize = 22.0f * iconScale * (1.0f + 0.045f * hoverProgress);
    const float progress = Utils::Clamp(transitionProgress, 0.0f, 1.0f);
    auto drawScaledGlyph = [&](IconType icon, float scale, float alpha) {
        if (alpha <= 0.001f || scale <= 0.001f) return;
        const float iconSize = baseIconSize * scale;
        const D2D1_RECT_F iconBounds = D2D1::RectF(
            center.x - iconSize / 2.0f,
            center.y - iconSize / 2.0f,
            center.x + iconSize / 2.0f,
            center.y + iconSize / 2.0f
        );
        DrawIconImage(icon, iconBounds, D2D1::ColorF(1.0f, 1.0f, 1.0f, alpha));
    };
    auto smoothStep = [](float value) {
        value = Utils::Clamp(value, 0.0f, 1.0f);
        return value * value * (3.0f - 2.0f * value);
    };

    // Keep the two phases distinct: the current glyph contracts into the
    // center first; the replacement then emerges from that same center.
    constexpr float kShrinkEnd = 0.46f;
    constexpr float kGrowStart = 0.54f;
    if (progress >= 1.0f) {
        drawScaledGlyph(toIcon, 1.0f, 1.0f);
    } else if (progress < kShrinkEnd) {
        const float phase = smoothStep(progress / kShrinkEnd);
        drawScaledGlyph(fromIcon, 1.0f - 0.84f * phase, 1.0f - phase);
    } else if (progress >= kGrowStart) {
        const float phase = smoothStep((progress - kGrowStart) / (1.0f - kGrowStart));
        drawScaledGlyph(toIcon, 0.16f + 0.84f * phase, phase);
    }
}
} // namespace Graphics
} // namespace DynamicIsland
// ============================================================================
// [Module] src\overlay\layout.cpp
// ============================================================================
namespace DynamicIsland {
namespace Overlay {

constexpr int FLOATING_WIN_TOP_GAP  = 4;
constexpr int FLOATING_PILL_TOP_GAP = 4;

IslandMetrics LayoutEngine::CalculateMetrics(
    EventType eventType,
    IslandState state,
    const Platform::TaskbarInfo& tbInfo,
    UINT customWidth
) {
    IslandMetrics m{};
    
    UINT dpi = tbInfo.dpi;
    int tbHeight = tbInfo.taskbarRect.bottom - tbInfo.taskbarRect.top;
    if (tbHeight <= 0) tbHeight = 34;
    
    int compactH = tbHeight - 6;
    if (compactH < 26) compactH = 26;
    
    const ModSettings layoutSettings = GetSettingsSnapshot();
    switch (layoutSettings.placement) {
        case PlacementMode::TopEmbed:
            m.isEmbedded = true;
            break;
        case PlacementMode::Floating:
            m.isEmbedded = false;
            break;
        case PlacementMode::Auto:
        default:
            m.isEmbedded = (tbInfo.position == TaskbarPosition::Top);
            break;
    }
    
    if (state == IslandState::Hidden) {
        m.width = 0.0f;
        m.height = static_cast<float>(Utils::ScaleDpi(compactH, dpi));
        m.cornerRadius = m.height / 2.0f;
    } else if (state == IslandState::Compact) {
        float defaultWidth = 196.0f;
        int idleH = 16;
        if (idleH > compactH) idleH = compactH;
        
        int heightForEvent = (eventType == EventType::None) ? idleH : compactH;
        
        switch (eventType) {
            case EventType::Media:      defaultWidth = 191.0f; break; // Compact media player: 5 px narrower
            case EventType::Timer:      defaultWidth = 176.0f; break;
            case EventType::MicStatus:  defaultWidth = 48.0f; break;
            case EventType::Volume:     defaultWidth = 206.0f; break;
            case EventType::CapsLock:   defaultWidth = 160.0f; break;
            case EventType::Power:      defaultWidth = 196.0f; break;
            case EventType::Bluetooth:  defaultWidth = 244.0f; break;
            case EventType::LowBattery: defaultWidth = 196.0f; break;
            case EventType::TimerDone:  defaultWidth = 204.0f; break;
            default:                    defaultWidth = 80.0f; break;
        }
        
        if (customWidth > 0) defaultWidth = static_cast<float>(customWidth);
        
        m.width = Utils::ScaleDpiF(defaultWidth, dpi);
        m.height = Utils::ScaleDpiF(static_cast<float>(heightForEvent), dpi);
        m.cornerRadius = m.height / 2.0f;
    } else if (state == IslandState::Expanded) {
        float expWidth = 340.0f;
        float expHeight = 150.0f;
        
        switch (eventType) {
            case EventType::Media:
                expWidth = 340.0f;
                expHeight = 150.0f;
                break;
            case EventType::Timer:
                expWidth = 300.0f;
                expHeight = 124.0f;
                break;
            case EventType::MicStatus:
                // Dot-only privacy indicator; width accounts for the 5 px global trim.
                expWidth = 69.0f;
                expHeight = 64.0f;
                break;
            default:
                expWidth = 280.0f;
                expHeight = 90.0f;
                break;
        }
        
        m.width = Utils::ScaleDpiF(expWidth, dpi);
        m.height = Utils::ScaleDpiF(expHeight, dpi);
        m.cornerRadius = Utils::ScaleDpiF(eventType == EventType::MicStatus ? 32.0f : 26.0f, dpi);
    }
    
    // Slightly shorten the island by 5 DPI-scaled pixels while preserving its height.
    // Position is recalculated below, so the island remains horizontally centered.
    if (state != IslandState::Hidden) {
        const float widthReduction = Utils::ScaleDpiF(5.0f, dpi);
        m.width = (m.width > widthReduction) ? (m.width - widthReduction) : m.width;
    }

    int screenWidth = tbInfo.monitorRect.right - tbInfo.monitorRect.left;
    int centerX = tbInfo.monitorRect.left + screenWidth / 2;
    
    if (tbInfo.position == TaskbarPosition::Left || tbInfo.position == TaskbarPosition::Right) {
        centerX = (tbInfo.workAreaRect.left + tbInfo.workAreaRect.right) / 2;
    }
    
    m.posX = static_cast<float>(centerX) - (m.width / 2.0f);
    
    if (m.isEmbedded) {
        // Forced Top Embed always anchors to the monitor's top edge. In Auto
        // mode with a top taskbar, use the taskbar's actual top coordinate.
        int topAnchor = tbInfo.monitorRect.top;
        if (layoutSettings.placement == PlacementMode::Auto &&
            tbInfo.position == TaskbarPosition::Top &&
            tbInfo.taskbarRect.top >= tbInfo.monitorRect.top) {
            topAnchor = tbInfo.taskbarRect.top;
        }
        
        if (state == IslandState::Expanded) {
            m.posY = static_cast<float>(topAnchor + 3);
        } else {
            m.posY = static_cast<float>(topAnchor) + static_cast<float>(tbHeight - compactH) / 2.0f;
        }
    } else {
        m.posY = static_cast<float>(tbInfo.monitorRect.top)
                 + static_cast<float>(FLOATING_WIN_TOP_GAP + FLOATING_PILL_TOP_GAP);
    }
    
    return m;
}
} // namespace Overlay
} // namespace DynamicIsland

// ============================================================================
// [Module] src\overlay\island_window.cpp
// ============================================================================
namespace DynamicIsland {
namespace Overlay {

static const wchar_t* WINDOW_CLASS_NAME = L"Win11DynamicIslandWndClass";

IslandWindow::IslandWindow() {
    m_renderer = std::make_unique<Graphics::D2DRenderer>();
}

IslandWindow::~IslandWindow() {
    Destroy();
}

bool IslandWindow::Create() {
    HINSTANCE hInstance = GetModuleHandleW(nullptr);
    
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = IslandWindow::WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    
    RegisterClassExW(&wc);
    
    m_hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        WINDOW_CLASS_NAME,
        L"Win11 Dynamic Island",
        WS_POPUP,
        0, 0, 400, 200,
        nullptr, nullptr, hInstance, this
    );
    
    if (!m_hwnd) {
        LogError(L"Failed to create Dynamic Island overlay window");
        return false;
    }
    
    m_cachedTaskbar = Platform::QueryPrimaryTaskbar();
    m_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
    
    if (!m_renderer->Initialize(m_hwnd)) {
        LogError(L"Failed to initialize D2D renderer for island window");
        return false;
    }
    
    SetTimer(m_hwnd, m_geometryCheckTimerId, 1000, nullptr);
    
    if (GetSettingsSnapshot().enableDebugHotkeys) {
        RegisterHotkeys();
    }
    
    LogInfo(L"Dynamic Island overlay window created successfully");
    return true;
}

void IslandWindow::Destroy() {
    UnregisterHotkeys();
    
    if (m_hwnd) {
        KillTimer(m_hwnd, m_autoCollapseTimerId);
        KillTimer(m_hwnd, m_animTimerId);
        KillTimer(m_hwnd, m_waveTimerId);
        KillTimer(m_hwnd, m_geometryCheckTimerId);
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
    
    UnregisterClassW(WINDOW_CLASS_NAME, GetModuleHandleW(nullptr));
}

void IslandWindow::ReinitializeRenderer() {
    if (!m_renderer || !m_hwnd) return;
    
    m_renderer->Cleanup();
    if (!m_renderer->Initialize(m_hwnd)) {
        LogError(L"Failed to re-initialize D2D renderer after settings change");
    } else {
        LogInfo(L"D2D renderer re-initialized with new font settings");
    }
    
    UpdateDimensions();
    TriggerAnimationUpdate();
}

static constexpr int HOTKEY_ID_BASE = 5100;

void IslandWindow::RegisterHotkeys() {
    if (!m_hwnd || m_hotkeysRegistered) return;

    int registeredCount = 0;
    for (int i = 0; i <= 9; ++i) {
        const UINT vk = (i == 9) ? '0' : static_cast<UINT>('1' + i);
        if (RegisterHotKey(m_hwnd, HOTKEY_ID_BASE + i, MOD_CONTROL | MOD_WIN, vk)) {
            ++registeredCount;
        } else {
            LogError(L"RegisterHotKey failed for Ctrl+Win+%c (error=%lu)",
                     static_cast<wchar_t>(vk), GetLastError());
        }
    }

    m_hotkeysRegistered = (registeredCount == 10);
    if (m_hotkeysRegistered) {
        LogInfo(L"Global test hotkeys registered (Ctrl+Win+1..9, 0)");
    } else if (registeredCount > 0) {
        LogError(L"Only %d of 10 debug hotkeys could be registered; rolling back", registeredCount);
        for (int i = 0; i <= 9; ++i) {
            UnregisterHotKey(m_hwnd, HOTKEY_ID_BASE + i);
        }
    }
}

void IslandWindow::UnregisterHotkeys() {
    if (!m_hwnd) return;

    for (int i = 0; i <= 9; ++i) {
        UnregisterHotKey(m_hwnd, HOTKEY_ID_BASE + i);
    }

    m_hotkeysRegistered = false;
}

void IslandWindow::OnTaskbarOrDisplayChanged() {
    Platform::TaskbarInfo tb = Platform::QueryPrimaryTaskbar();
    m_cachedTaskbar = tb;
    
    if (m_hotkeysRegistered) {
        UnregisterHotkeys();
    }
    
    if (GetSettingsSnapshot().enableDebugHotkeys) {
        RegisterHotkeys();
    }
    
    UpdateDimensions();
    TriggerAnimationUpdate();
}

void IslandWindow::CheckTaskbarGeometry() {
    Platform::TaskbarInfo tb = Platform::QueryPrimaryTaskbar();
    
    bool changed = (tb.position != m_cachedTaskbar.position) ||
                   (tb.isAutoHide != m_cachedTaskbar.isAutoHide) ||
                   (tb.taskbarRect.left != m_cachedTaskbar.taskbarRect.left) ||
                   (tb.taskbarRect.top != m_cachedTaskbar.taskbarRect.top) ||
                   (tb.taskbarRect.right != m_cachedTaskbar.taskbarRect.right) ||
                   (tb.taskbarRect.bottom != m_cachedTaskbar.taskbarRect.bottom) ||
                   (tb.workAreaRect.left != m_cachedTaskbar.workAreaRect.left) ||
                   (tb.workAreaRect.top != m_cachedTaskbar.workAreaRect.top) ||
                   (tb.workAreaRect.right != m_cachedTaskbar.workAreaRect.right) ||
                   (tb.workAreaRect.bottom != m_cachedTaskbar.workAreaRect.bottom);
    
    if (changed) {
        LogInfo(L"Detected real-time taskbar geometry change, adapting island position");
        m_cachedTaskbar = tb;
        UpdateDimensions();
    }
    
    if (m_hwnd && m_state != IslandState::Hidden) {
        SetWindowPos(m_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
}

void IslandWindow::Show(bool show) {
    if (!m_hwnd) return;
    if (show) {
        SetWindowPos(m_hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
        if (m_currentEvent == EventType::Media && m_mediaState.isPlaying) {
            SetTimer(m_hwnd, m_waveTimerId, 33, nullptr);
        }
    } else {
        KillTimer(m_hwnd, m_waveTimerId);
        ShowWindow(m_hwnd, SW_HIDE);
    }
}

void IslandWindow::SetState(IslandState state) {
    if (m_state == state) return;
    
    IslandState oldState = m_state;
    m_state = state;
    
    if (m_state != IslandState::Hidden && oldState == IslandState::Hidden) {
        Show(true);
    }
    
    UpdateDimensions();
    
    if (m_state != IslandState::Expanded) {
        DisarmAutoCollapse();
    }
}

void IslandWindow::SetCurrentEvent(EventType eventType) {
    if (m_currentEvent != eventType) {
        m_prevEvent = m_currentEvent;
        m_prevTransientState = m_transientState;
        m_prevMediaState = m_mediaState;
        m_prevTimerState = m_timerState;
        
        m_currentEvent = eventType;
        
        m_crossfadeAlpha.SnapTo(0.0f);
        m_crossfadeAlpha.SetTarget(1.0f, 220);
        
        Platform::TaskbarInfo tb = Platform::QueryPrimaryTaskbar();
        IslandMetrics oldM = LayoutEngine::CalculateMetrics(m_prevEvent, m_state, tb);
        IslandMetrics newM = LayoutEngine::CalculateMetrics(m_currentEvent, m_state, tb);
        
        if (std::abs(oldM.width - newM.width) < 24.0f) {
            m_animScale.SnapTo(1.035f);
            m_animScale.SetTarget(1.0f, 260);
        }
    }
    
    UpdateDimensions();
    if (m_hwnd) {
        if (m_currentEvent == EventType::Media && m_mediaState.isPlaying && m_state != IslandState::Hidden) {
            SetTimer(m_hwnd, m_waveTimerId, 33, nullptr);
        } else {
            KillTimer(m_hwnd, m_waveTimerId);
        }
    }
}

void IslandWindow::ArmAutoCollapse() {
    if (!m_hwnd) return;
    
    DisarmAutoCollapse();
    
    const int collapseDelay = GetSettingsSnapshot().autoCollapseDelayMs;
    UINT delayMs = (collapseDelay > 0) ? static_cast<UINT>(collapseDelay) : 1500;
    SetTimer(m_hwnd, m_autoCollapseTimerId, delayMs, nullptr);
}

void IslandWindow::DisarmAutoCollapse() {
    if (m_hwnd) {
        KillTimer(m_hwnd, m_autoCollapseTimerId);
    }
}

void IslandWindow::UpdateDimensions() {
    Platform::TaskbarInfo tb = Platform::QueryPrimaryTaskbar();
    m_cachedTaskbar = tb;
    
    IslandMetrics target = LayoutEngine::CalculateMetrics(m_currentEvent, m_state, tb);
    
    float animW = m_animWidth.GetValue();
    float animH = m_animHeight.GetValue();
    
    float maxW = (target.width > animW) ? target.width : animW;
    float maxH = (target.height > animH) ? target.height : animH;
    
    float minCanvasW = Utils::ScaleDpiF(360.0f, tb.dpi);
    float minCanvasH = Utils::ScaleDpiF(160.0f, tb.dpi);
    
    if (maxW < minCanvasW) maxW = minCanvasW;
    if (maxH < minCanvasH) maxH = minCanvasH;
    
    int screenWidth = tb.monitorRect.right - tb.monitorRect.left;
    int centerX = tb.monitorRect.left + screenWidth / 2;
    
    if (tb.position == TaskbarPosition::Left || tb.position == TaskbarPosition::Right) {
        centerX = (tb.workAreaRect.left + tb.workAreaRect.right) / 2;
    }
    
    int winW = static_cast<int>(maxW + 8.0f);
    int winH = static_cast<int>(maxH + 8.0f);
    int winX = centerX - (winW / 2);
    int winY = 0;
    
    if (tb.position == TaskbarPosition::Top) {
        int topAnchor = (tb.taskbarRect.top >= tb.monitorRect.top) ? tb.taskbarRect.top : tb.monitorRect.top;
        winY = topAnchor;
    } else {
        winY = tb.monitorRect.top + FLOATING_WIN_TOP_GAP;
    }
    
    if (m_hwnd && (m_windowX != winX || m_windowY != winY || m_windowW != winW || m_windowH != winH)) {
        m_windowX = winX;
        m_windowY = winY;
        m_windowW = winW;
        m_windowH = winH;
        
        SetWindowPos(m_hwnd, HWND_TOPMOST, winX, winY, winW, winH, SWP_NOACTIVATE);
        m_renderer->Resize(static_cast<UINT>(winW), static_cast<UINT>(winH));
    }
    
    if (m_animWidth.GetValue() <= 0.0f && target.width > 0.0f) {
        m_animWidth.SnapTo(target.width * 0.35f);
        m_animHeight.SnapTo(target.height * 0.35f);
        m_animRadius.SnapTo(target.cornerRadius * 0.35f);
        m_animOpacity.SnapTo(0.0f);
        
        m_animWidth.SetTarget(target.width, 450);
        m_animHeight.SetTarget(target.height, 450);
        m_animRadius.SetTarget(target.cornerRadius, 450);
        m_animOpacity.SetTarget(1.0f, 250);
        
        if (m_hwnd) {
            SetTimer(m_hwnd, m_animTimerId, 8, nullptr);
        }
        
        TriggerAnimationUpdate();
    } else {
        m_animWidth.SetTarget(target.width, 450);
        m_animHeight.SetTarget(target.height, 450);
        m_animRadius.SetTarget(target.cornerRadius, 450);
        m_animOpacity.SetTarget(m_state == IslandState::Hidden ? 0.0f : 1.0f, 200);
        
        if (m_hwnd) {
            SetTimer(m_hwnd, m_animTimerId, 8, nullptr);
        }
    }
}

void IslandWindow::UpdateMediaButtonHover(float localX, float localY) {
    int hoveredButton = -1;

    if (m_state == IslandState::Expanded && m_currentEvent == EventType::Media) {
        const float midX = m_animWidth.GetValue() / 2.0f;
        if (localY >= 98.0f && localY <= 136.0f) {
            if (localX >= midX - 60.0f && localX <= midX - 26.0f) {
                hoveredButton = 0;
            } else if (localX >= midX - 22.0f && localX <= midX + 22.0f) {
                hoveredButton = 1;
            } else if (localX >= midX + 26.0f && localX <= midX + 60.0f) {
                hoveredButton = 2;
            }
        }
    }

    if (hoveredButton == m_hoveredMediaButton) {
        return;
    }

    m_hoveredMediaButton = hoveredButton;
    m_prevButtonHover.SetTarget(hoveredButton == 0 ? 1.0f : 0.0f, 150.0);
    m_playButtonHover.SetTarget(hoveredButton == 1 ? 1.0f : 0.0f, 150.0);
    m_nextButtonHover.SetTarget(hoveredButton == 2 ? 1.0f : 0.0f, 150.0);

    if (m_hwnd && m_state != IslandState::Hidden) {
        SetTimer(m_hwnd, m_animTimerId, 8, nullptr);
    }
}

void IslandWindow::TriggerAnimationUpdate() {
    double now = Graphics::GetHighPrecisionTimeMs();
    
    m_animWidth.Update(now);
    m_animHeight.Update(now);
    m_animRadius.Update(now);
    m_animOpacity.Update(now);
    m_crossfadeAlpha.Update(now);
    m_animScale.Update(now);
    m_prevButtonHover.Update(now);
    m_playButtonHover.Update(now);
    m_nextButtonHover.Update(now);

    if (m_playbackIconTransitionActive) {
        constexpr double kPlaybackIconTransitionDurationMs = 280.0;
        const double elapsed = now - m_playbackIconTransitionStartMs;
        const float rawProgress = static_cast<float>(elapsed / kPlaybackIconTransitionDurationMs);
        m_playbackIconTransitionProgress = Utils::Clamp(rawProgress, 0.0f, 1.0f);
        if (rawProgress >= 1.0f) {
            m_playbackIconTransitionProgress = 1.0f;
            m_playbackIconTransitionActive = false;
        }
    }
    
    float curW = m_animWidth.GetValue();
    float curH = m_animHeight.GetValue();
    float curR = m_animRadius.GetValue();
    
    if (curW > 0.0f && curH > 0.0f && m_hwnd) {
        Platform::TaskbarInfo tb = m_cachedTaskbar;
        
        float pillX = (static_cast<float>(m_windowW) - curW) / 2.0f;
        float pillY = 0.0f;
        
        if (tb.position == TaskbarPosition::Top) {
            int tbHeight = tb.taskbarRect.bottom - tb.taskbarRect.top;
            if (tbHeight <= 0) tbHeight = 34;
            
            if (m_state == IslandState::Expanded) {
                pillY = 3.0f;
            } else {
                pillY = (static_cast<float>(tbHeight) - curH) / 2.0f;
                if (pillY < 0.0f) pillY = 0.0f;
            }
        } else {
            pillY = static_cast<float>(FLOATING_PILL_TOP_GAP);
        }
        
        m_currentPillX = pillX;
        m_currentPillY = pillY;
        m_currentPillW = curW;
        m_currentPillH = curH;
        m_currentPillR = curR;
    }
    
    Render();
    
    if (!m_animWidth.IsAnimating() && !m_animHeight.IsAnimating() &&
        !m_animRadius.IsAnimating() && !m_animOpacity.IsAnimating() &&
        !m_crossfadeAlpha.IsAnimating() && !m_animScale.IsAnimating() &&
        !m_prevButtonHover.IsAnimating() && !m_playButtonHover.IsAnimating() &&
        !m_nextButtonHover.IsAnimating() && !m_playbackIconTransitionActive) {
        KillTimer(m_hwnd, m_animTimerId);
        if (m_state == IslandState::Hidden) {
            KillTimer(m_hwnd, m_waveTimerId);
            ShowWindow(m_hwnd, SW_HIDE);
        }
    }
}

bool IslandWindow::IsPointInSquircle(float px, float py) const {
    if (m_animOpacity.GetValue() <= 0.05f) return false;
    
    float left = m_currentPillX;
    float top = m_currentPillY;
    float right = m_currentPillX + m_currentPillW;
    float bottom = m_currentPillY + m_currentPillH;
    float r = m_currentPillR;
    
    if (px < left || px > right || py < top || py > bottom) {
        return false;
    }
    
    if (r <= 0.0f) return true;
    
    if ((px >= left + r && px <= right - r) || (py >= top + r && py <= bottom - r)) {
        return true;
    }
    
    float cx = (px < left + r) ? (left + r) : (right - r);
    float cy = (py < top + r) ? (top + r) : (bottom - r);
    
    float dx = px - cx;
    float dy = py - cy;
    
    return (dx * dx + dy * dy) <= (r * r);
}

bool IslandWindow::TryBeginSeek(float localX, float localY, float w, float h) {
    if (m_state != IslandState::Expanded || m_currentEvent != EventType::Media) {
        return false;
    }
    if (m_mediaState.durationSeconds <= 0) {
        return false;
    }
    
    // Generous hit area around the thin progress bar (Y approximately 80-86 in layout coordinates).
    const float barLeft = 54.0f;
    const float barRight = w - 54.0f;
    const float barTop = 70.0f;
    const float barBottom = 96.0f;
    
    if (localX < barLeft || localX > barRight || localY < barTop || localY > barBottom) {
        return false;
    }
    
    m_isSeeking = true;
    UpdateSeekFromLocalX(localX, w);
    DisarmAutoCollapse();
    return true;
}

void IslandWindow::UpdateSeekFromLocalX(float localX, float w) {
    const float barLeft = 54.0f;
    const float barRight = w - 54.0f;
    const float barWidth = (std::max)(1.0f, barRight - barLeft);
    float t = (localX - barLeft) / barWidth;
    m_seekProgress = Utils::Clamp(t, 0.0f, 1.0f);
    
    // Optimistic local update so the bar and time labels follow the finger/mouse immediately.
    const int newElapsed = static_cast<int>(m_seekProgress * static_cast<float>(m_mediaState.durationSeconds) + 0.5f);
    m_mediaState.progress = m_seekProgress;
    m_mediaState.elapsedSeconds = newElapsed;
    m_mediaPositionAnchorSeconds = static_cast<double>(newElapsed);
    m_mediaPositionAnchorMs = static_cast<double>(GetTickCount64());
    
    Render();
}

void IslandWindow::EndSeek(bool commit) {
    if (!m_isSeeking) return;
    
    const float progress = m_seekProgress;
    m_isSeeking = false;
    
    if (commit && m_serviceManager) {
        m_serviceManager->OnMediaSeek(progress);
    }
    
    if (m_state == IslandState::Expanded) {
        ArmAutoCollapse();
    }
}

bool IslandWindow::TryBeginVolumeDrag(float localX, float localY, float w, float h) {
    if (m_state != IslandState::Compact || m_currentEvent != EventType::Volume) {
        return false;
    }
    
    // Same geometry the compact volume pill draws (with a generous hit area).
    const float barLeft = 38.0f;
    const float barRight = w - 42.0f;
    const float cy = h / 2.0f;
    
    if (localX < barLeft - 8.0f || localX > barRight + 8.0f) return false;
    if (localY < cy - 12.0f || localY > cy + 12.0f) return false;
    
    m_isVolumeDragging = true;
    UpdateVolumeDragFromLocalX(localX, w);
    return true;
}

void IslandWindow::UpdateVolumeDragFromLocalX(float localX, float w) {
    const float barLeft = 38.0f;
    const float barRight = w - 42.0f;
    const float barWidth = (std::max)(1.0f, barRight - barLeft);
    float t = (localX - barLeft) / barWidth;
    t = Utils::Clamp(t, 0.0f, 1.0f);
    
    m_volumeDragFraction = t;
    if (m_serviceManager) {
        m_serviceManager->OnVolumeScrub(t);
    }
    
    Render();
}

void IslandWindow::EndVolumeDrag() {
    if (!m_isVolumeDragging) return;
    
    m_isVolumeDragging = false;
    m_volumeDragEndTimeMs = Graphics::GetHighPrecisionTimeMs();
}

void IslandWindow::OnClick(int clientX, int clientY) {
    float localX = static_cast<float>(clientX) - m_currentPillX;
    float localY = static_cast<float>(clientY) - m_currentPillY;
    
    float w = m_animWidth.GetValue();
    float h = m_animHeight.GetValue();
    
    if (localX < 0.0f || localX > w || localY < 0.0f || localY > h) {
        return;
    }
    
    if (m_state == IslandState::Compact) {
        if (m_currentEvent == EventType::Media ||
            m_currentEvent == EventType::Timer ||
            m_currentEvent == EventType::MicStatus) {
            SetState(IslandState::Expanded);
            m_isHovered = true;
            TRACKMOUSEEVENT tme{sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hwnd, 0};
            TrackMouseEvent(&tme);
        }
        return;
    }
    
    if (m_state == IslandState::Expanded) {
        // Hit regions use the same 96-DPI Direct2D coordinates as the drawn controls.
        if (m_currentEvent == EventType::Media) {
            // Click-to-seek on the progress bar (also works if the user didn't drag).
            if (TryBeginSeek(localX, localY, w, h)) {
                EndSeek(true);
                return;
            }
            
            float midX = w / 2.0f;
            float btnY1 = 98.0f;
            float btnY2 = 136.0f;
            
            if (localY >= btnY1 && localY <= btnY2) {
                float prevX1 = midX - 60.0f;
                float prevX2 = midX - 26.0f;
                if (localX >= prevX1 && localX <= prevX2) {
                    if (m_serviceManager) m_serviceManager->OnMediaPrev();
                    return;
                }
                
                float playX1 = midX - 22.0f;
                float playX2 = midX + 22.0f;
                if (localX >= playX1 && localX <= playX2) {
                    if (m_serviceManager) m_serviceManager->OnMediaPlayPause();
                    return;
                }
                
                float nextX1 = midX + 26.0f;
                float nextX2 = midX + 60.0f;
                if (localX >= nextX1 && localX <= nextX2) {
                    if (m_serviceManager) m_serviceManager->OnMediaNext();
                    return;
                }
            }
        } else if (m_currentEvent == EventType::Timer) {
            float btnW = 90.0f;
            float btnGap = 16.0f;
            float b1Left = (w - btnW * 2.0f - btnGap) / 2.0f;
            
            float btnY1 = 70.0f;
            float btnY2 = 108.0f;
            
            if (localY >= btnY1 && localY <= btnY2) {
                if (localX >= b1Left && localX <= b1Left + btnW) {
                    if (m_serviceManager) m_serviceManager->OnTimerTogglePause();
                    return;
                }
                if (localX >= b1Left + btnW + btnGap && localX <= b1Left + btnW * 2.0f + btnGap) {
                    if (m_serviceManager) m_serviceManager->OnTimerStop();
                    SetState(IslandState::Compact);
                    return;
                }
            }
        }
    }
}

void IslandWindow::UpdateEqualizerSmoothing() {
    Services::EqBands target = m_serviceManager ? m_serviceManager->GetAudioBands() : Services::EqBands{};

    const double now = Graphics::GetHighPrecisionTimeMs();
    double dt = (m_lastEqUpdateTimeMs > 0.0) ? (now - m_lastEqUpdateTimeMs) / 1000.0 : 0.033;
    m_lastEqUpdateTimeMs = now;
    dt = (std::max)(0.0, (std::min)(dt, 0.1));
    const float dtF = static_cast<float>(dt);

    // Independent envelopes per frequency band: quick attack, gentle release.
    for (int i = 0; i < 6; ++i) {
        const float goal = Utils::Clamp(target.values[i], 0.0f, 1.0f);
        const float rate = (goal > m_eqBandSmooth[i]) ? 12.0f : 3.6f;
        const float alpha = 1.0f - std::exp(-rate * dtF);
        m_eqBandSmooth[i] += (goal - m_eqBandSmooth[i]) * alpha;
    }
}

void IslandWindow::RenderCompactContent(EventType eventType, float w, float h, float alpha) {
    if (alpha <= 0.005f || !m_renderer) return;
    
    const TransientState& trState = (eventType == m_prevEvent) ? m_prevTransientState : m_transientState;
    const MediaState& medState = (eventType == m_prevEvent) ? m_prevMediaState : m_mediaState;
    const TimerState& timState = (eventType == m_prevEvent) ? m_prevTimerState : m_timerState;
    
    switch (eventType) {
        case EventType::Media: {
            D2D1_RECT_F artRect = D2D1::RectF(10.0f, (h - 28.0f) / 2.0f, 38.0f, (h + 28.0f) / 2.0f);
            float compactArtRotation = !medState.title.empty() ? m_albumArtRotation : 0.0f;
            m_renderer->DrawAlbumArt(artRect, 14.0f, compactArtRotation);
            
            bool eqActive = !medState.title.empty();
            // Per-frame smoothed bands (updated in UpdateEqualizerSmoothing)
            // keep the compact bars moving fluidly instead of stepping.
            m_renderer->DrawEqualizerWaves(
                D2D1::Point2F(w - 28.0f, (h - 18.0f) / 2.0f),
                18.0f,
                m_eqBandSmooth,
                eqActive
            );
            
            std::wstring track = medState.title;
            // The compact pill is too small for long titles: if the title is
            // longer than 6 characters, don't render any text at all (art and
            // equalizer only). Short titles (<= 6 chars) show as before.
            if (track.size() > 6) {
                track.clear();
            }
            if (!track.empty()) {
                m_renderer->DrawTextCentered(
                    track,
                    D2D1::RectF(44.0f, (h - 16.0f) / 2.0f, w - 32.0f, (h + 16.0f) / 2.0f),
                    D2D1::ColorF(1.0f, 1.0f, 1.0f, alpha),
                    12.0f,
                    true
                );
            }
            break;
        }
        case EventType::Timer: {
            float fraction = timState.totalSeconds > 0
                             ? (1.0f - static_cast<float>(timState.remainingSeconds) / static_cast<float>(timState.totalSeconds))
                             : 0.0f;
            m_renderer->DrawProgressRing(
                D2D1::Point2F(19.0f, h / 2.0f),
                6.0f,
                fraction,
                D2D1::ColorF(1.0f, 0.624f, 0.039f, alpha),
                2.0f
            );
            m_renderer->DrawTextString(
                Utils::FormatDuration(timState.remainingSeconds),
                D2D1::RectF(32.0f, (h - 16.0f) / 2.0f, w - 54.0f, (h + 16.0f) / 2.0f),
                D2D1::ColorF(1.0f, 1.0f, 1.0f, alpha),
                12.0f,
                false
            );
            m_renderer->DrawTextString(
                timState.label,
                D2D1::RectF(w - 52.0f, (h - 16.0f) / 2.0f, w - 10.0f, (h + 16.0f) / 2.0f),
                D2D1::ColorF(0.66f, 0.67f, 0.71f, alpha),
                12.0f,
                false
            );
            break;
        }
        case EventType::MicStatus: {
            // The active microphone indicator is intentionally dot-only.
            m_renderer->DrawStatusDot(
                D2D1::Point2F(w / 2.0f, h / 2.0f),
                4.0f,
                D2D1::ColorF(1.0f, 0.624f, 0.039f, alpha)
            );
            break;
        }
        case EventType::Volume: {
            D2D1_RECT_F volRect = D2D1::RectF(12.0f, (h - 22.0f) / 2.0f, 34.0f, (h + 22.0f) / 2.0f);
            m_renderer->DrawIconImage(Graphics::IconType::Volume, volRect, D2D1::ColorF(1.0f, 1.0f, 1.0f, alpha));
            
            float barLeft = 38.0f;
            float barRight = w - 42.0f;
            float frac = trState.progressFraction > 0.0f ? trState.progressFraction : 0.68f;
            int val = trState.value > 0 ? trState.value : 68;
            
            // While (or just after) scrubbing, show the local drag value instead
            // of the last polled value so the bar tracks the cursor exactly.
            const double dragNow = Graphics::GetHighPrecisionTimeMs();
            if (m_isVolumeDragging ||
                (m_volumeDragEndTimeMs > 0.0 && dragNow - m_volumeDragEndTimeMs < 400.0)) {
                frac = m_volumeDragFraction;
                val = static_cast<int>(std::lround(frac * 100.0f));
            }
            
            m_renderer->DrawProgressBar(
                D2D1::RectF(barLeft, (h - 4.0f) / 2.0f, barRight, (h + 4.0f) / 2.0f),
                frac,
                D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.20f * alpha),
                D2D1::ColorF(1.0f, 1.0f, 1.0f, alpha)
            );
            
            m_renderer->DrawTextString(
                std::to_wstring(val) + L"%",
                D2D1::RectF(w - 36.0f, (h - 16.0f) / 2.0f, w - 8.0f, (h + 16.0f) / 2.0f),
                D2D1::ColorF(1.0f, 1.0f, 1.0f, alpha),
                12.0f,
                true
            );
            break;
        }
        case EventType::CapsLock: {
            D2D1_RECT_F capsRect = D2D1::RectF(12.0f, (h - 22.0f) / 2.0f, 34.0f, (h + 22.0f) / 2.0f);
            m_renderer->DrawIconImage(Graphics::IconType::CapsLock, capsRect, D2D1::ColorF(1.0f, 1.0f, 1.0f, alpha));
            
            std::wstring capsLabel = trState.title.empty()
                ? std::wstring(L"Caps Lock")
                : trState.title;
            m_renderer->DrawTextString(
                capsLabel,
                D2D1::RectF(38.0f, (h - 16.0f) / 2.0f, w - 45.0f, (h + 16.0f) / 2.0f),
                D2D1::ColorF(1.0f, 1.0f, 1.0f, alpha),
                12.0f,
                false
            );
            
            std::wstring status = trState.subtitle.empty() ? L"ON" : trState.subtitle;
            m_renderer->DrawTextString(
                status,
                D2D1::RectF(w - 42.0f, (h - 16.0f) / 2.0f, w - 10.0f, (h + 16.0f) / 2.0f),
                D2D1::ColorF(1.0f, 1.0f, 1.0f, alpha),
                12.0f,
                true
            );
            break;
        }
        case EventType::Power: {
            const bool charging = trState.title == L"Charging";
            const bool batterySaver = trState.title == L"Battery saver";
            D2D1_RECT_F batRect = D2D1::RectF(12.0f, (h - 22.0f) / 2.0f, 34.0f, (h + 22.0f) / 2.0f);
            const D2D1_COLOR_F batteryColor = charging
                ? D2D1::ColorF(0.188f, 0.820f, 0.345f, alpha)
                : (batterySaver
                    ? D2D1::ColorF(1.0f, 0.58f, 0.12f, alpha)
                    : D2D1::ColorF(1.0f, 1.0f, 1.0f, alpha));
            m_renderer->DrawIconImage(Graphics::IconType::BatteryHorizontal, batRect, batteryColor);

            const std::wstring powerLabel = charging
                ? L"Charging"
                : (batterySaver ? L"Battery saver" : L"On battery");
            m_renderer->DrawTextString(
                powerLabel,
                D2D1::RectF(40.0f, (h - 16.0f) / 2.0f, w - 10.0f, (h + 16.0f) / 2.0f),
                D2D1::ColorF(1.0f, 1.0f, 1.0f, alpha),
                12.0f,
                true
            );
            break;
        }
        case EventType::Bluetooth: {
            D2D1_RECT_F btRect = D2D1::RectF(12.0f, (h - 22.0f) / 2.0f, 34.0f, (h + 22.0f) / 2.0f);
            m_renderer->DrawIconImage(Graphics::IconType::Bluetooth, btRect, D2D1::ColorF(1.0f, 1.0f, 1.0f, alpha));
            
            std::wstring dev = trState.title.empty() ? L"WH-1000XM5" : trState.title;
            m_renderer->DrawTextString(
                dev,
                D2D1::RectF(38.0f, (h - 16.0f) / 2.0f, w - 50.0f, (h + 16.0f) / 2.0f),
                D2D1::ColorF(1.0f, 1.0f, 1.0f, alpha),
                12.0f,
                false
            );
            
            // Battery level is optional — many devices never report it.
            if (trState.value > 0) {
                m_renderer->DrawTextString(
                    std::to_wstring(trState.value) + L"%",
                    D2D1::RectF(w - 46.0f, (h - 16.0f) / 2.0f, w - 10.0f, (h + 16.0f) / 2.0f),
                    D2D1::ColorF(1.0f, 1.0f, 1.0f, alpha),
                    12.0f,
                    true
                );
            }
            break;
        }
        case EventType::LowBattery: {
            D2D1_RECT_F batRect = D2D1::RectF(12.0f, (h - 22.0f) / 2.0f, 34.0f, (h + 22.0f) / 2.0f);
            m_renderer->DrawIconImage(Graphics::IconType::BatteryHorizontal, batRect, D2D1::ColorF(1.0f, 0.271f, 0.227f, alpha));
            
            m_renderer->DrawTextString(
                L"Low battery",
                D2D1::RectF(38.0f, (h - 16.0f) / 2.0f, w - 50.0f, (h + 16.0f) / 2.0f),
                D2D1::ColorF(1.0f, 1.0f, 1.0f, alpha),
                12.0f,
                false
            );
            
            int lbVal = trState.value > 0 ? trState.value : 15;
            m_renderer->DrawTextString(
                std::to_wstring(lbVal) + L"%",
                D2D1::RectF(w - 46.0f, (h - 16.0f) / 2.0f, w - 10.0f, (h + 16.0f) / 2.0f),
                D2D1::ColorF(1.0f, 0.271f, 0.227f, alpha),
                12.0f,
                true
            );
            break;
        }
        case EventType::TimerDone: {
            D2D1_RECT_F okRect = D2D1::RectF(12.0f, (h - 22.0f) / 2.0f, 34.0f, (h + 22.0f) / 2.0f);
            m_renderer->DrawIconImage(Graphics::IconType::Checkmark, okRect, D2D1::ColorF(0.188f, 0.820f, 0.345f, alpha));
            
            m_renderer->DrawTextString(
                L"Timer done",
                D2D1::RectF(38.0f, (h - 16.0f) / 2.0f, w - 10.0f, (h + 16.0f) / 2.0f),
                D2D1::ColorF(1.0f, 1.0f, 1.0f, alpha),
                12.0f,
                false
            );
            break;
        }
        default: {
            break;
        }
    }
}

void IslandWindow::Render() {
    if (!m_renderer) return;
    
    if (m_serviceManager) {
        std::vector<uint8_t> thumbPixels;
        uint32_t thumbW = 0, thumbH = 0;
        if (m_serviceManager->GetMediaThumbnail(thumbPixels, thumbW, thumbH, m_currentThumbVersion)) {
            if (!thumbPixels.empty() && thumbW > 0 && thumbH > 0) {
                m_renderer->SetAlbumArtBitmap(thumbPixels.data(), thumbW, thumbH);
            } else {
                m_renderer->ClearAlbumArtBitmap();
            }
        }
    }
    
    UpdateEqualizerSmoothing();
    
    if (!m_renderer->BeginDraw()) {
        return;
    }
    m_renderer->Clear(D2D1::ColorF(0, 0, 0, 0.0f));
    
    float w = m_animWidth.GetValue();
    float h = m_animHeight.GetValue();
    float r = m_animRadius.GetValue();
    float o = m_animOpacity.GetValue();
    
    if (w > 0.0f && h > 0.0f && o > 0.01f) {
        ID2D1RenderTarget* rt = m_renderer->GetRenderTarget();
        if (rt) {
            float scale = m_animScale.GetValue();
            if (std::abs(scale - 1.0f) > 0.001f) {
                D2D1_POINT_2F center = D2D1::Point2F(m_currentPillX + w / 2.0f, m_currentPillY + h / 2.0f);
                rt->SetTransform(D2D1::Matrix3x2F::Scale(scale, scale, center) * D2D1::Matrix3x2F::Translation(m_currentPillX, m_currentPillY));
            } else {
                rt->SetTransform(D2D1::Matrix3x2F::Translation(m_currentPillX, m_currentPillY));
            }
        }
        
        D2D1_ROUNDED_RECT pill = D2D1::RoundedRect(D2D1::RectF(0.0f, 0.0f, w, h), r, r);
        
        D2D1_COLOR_F bgColor = D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f * o);
        // No border stroke: the island reads as one solid iOS-style pill.
        m_renderer->DrawRoundedPill(pill, bgColor, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f), 0.0f);
        
        IslandState visualState = m_state;
        float compactThreshold = Utils::ScaleDpiF(50.0f, m_cachedTaskbar.dpi);
        
        if (m_state == IslandState::Compact && m_animHeight.IsAnimating() && h > compactThreshold) {
            visualState = IslandState::Expanded;
        } else if (m_state == IslandState::Expanded && m_animHeight.IsAnimating() && h < compactThreshold * 0.55f) {
            visualState = IslandState::Compact;
        }
        
        if (visualState == IslandState::Compact) {
            if (m_crossfadeAlpha.IsAnimating() && m_prevEvent != EventType::None && m_prevEvent != m_currentEvent) {
                float aPrev = (1.0f - m_crossfadeAlpha.GetValue()) * o;
                float aCur = m_crossfadeAlpha.GetValue() * o;
                RenderCompactContent(m_prevEvent, w, h, aPrev);
                RenderCompactContent(m_currentEvent, w, h, aCur);
            } else {
                RenderCompactContent(m_currentEvent, w, h, o);
            }
        } else if (visualState == IslandState::Expanded) {
            // macOS-like collapse: the expanded player contents compress smoothly
            // into the island instead of being clipped away while the pill shrinks.
            bool collapsingToCompact = (m_state == IslandState::Compact && m_animHeight.IsAnimating());
            D2D1_MATRIX_3X2_F originalContentTransform = D2D1::Matrix3x2F::Identity();
            if (rt) rt->GetTransform(&originalContentTransform);
            if (collapsingToCompact && rt) {
                float expandedHeight = Utils::ScaleDpiF(150.0f, m_cachedTaskbar.dpi);
                float compactHeight = compactThreshold;
                float contentScaleY = (expandedHeight > compactHeight)
                    ? Utils::Clamp((h - compactHeight) / (expandedHeight - compactHeight), 0.0f, 1.0f)
                    : 1.0f;
                rt->SetTransform(D2D1::Matrix3x2F::Scale(1.0f, contentScaleY, D2D1::Point2F(0.0f, 0.0f)) * originalContentTransform);
            }
            switch (m_currentEvent) {
                case EventType::Media: {
                    D2D1_RECT_F artRect = D2D1::RectF(16.0f, 16.0f, 68.0f, 68.0f);
                    m_renderer->DrawAlbumArt(artRect, 16.0f, 0.0f);
                    
                    m_renderer->DrawMarqueeText(
                        m_mediaState.title,
                        D2D1::RectF(80.0f, 22.0f, w - 16.0f, 42.0f),
                        D2D1::ColorF(D2D1::ColorF::White),
                        m_mediaState.isPlaying ? m_marqueeOffset : 0.0f,
                        true
                    );
                    
                    m_renderer->DrawMarqueeText(
                        m_mediaState.artist,
                        D2D1::RectF(80.0f, 44.0f, w - 16.0f, 64.0f),
                        D2D1::ColorF(0.66f, 0.67f, 0.71f, 1.0f),
                        m_mediaState.isPlaying ? m_marqueeOffset * 0.7f : 0.0f,
                        false
                    );
                    
                    // Extrapolate playback position from the Windows monotonic clock.
                    // GSMTC Position is sampled periodically, so animate between polls.
                    // While the user is dragging the progress bar, prefer the seek position.
                    double displayElapsedExact = static_cast<double>(m_mediaState.elapsedSeconds);
                    float displayProgress = m_mediaState.progress;
                    
                    if (m_isSeeking) {
                        displayProgress = m_seekProgress;
                        displayElapsedExact = static_cast<double>(m_seekProgress) *
                            static_cast<double>((std::max)(m_mediaState.durationSeconds, 1));
                    } else if (m_mediaState.isPlaying && m_mediaPositionAnchorMs > 0.0) {
                        const double nowMs = static_cast<double>(GetTickCount64());
                        displayElapsedExact = static_cast<double>(m_mediaPositionAnchorSeconds) +
                            (std::max)(0.0, (nowMs - m_mediaPositionAnchorMs) / 1000.0);
                    }
                    displayElapsedExact = (std::max)(0.0, (std::min)(
                        displayElapsedExact, static_cast<double>(m_mediaState.durationSeconds)));
                    const int displayElapsedSeconds = static_cast<int>(displayElapsedExact);

                    if (!m_isSeeking && m_mediaState.durationSeconds > 0) {
                        displayProgress = static_cast<float>(displayElapsedExact /
                                          static_cast<double>(m_mediaState.durationSeconds));
                        displayProgress = (std::max)(0.0f, (std::min)(displayProgress, 1.0f));
                    }

                    // iOS-style row: elapsed time, bar, negative remaining time.
                    // Slightly taller bar while seeking for clearer feedback.
                    const float barY1 = m_isSeeking ? 78.0f : 80.0f;
                    const float barY2 = m_isSeeking ? 88.0f : 86.0f;
                    m_renderer->DrawProgressBar(
                        D2D1::RectF(56.0f, barY1, w - 56.0f, barY2),
                        displayProgress,
                        D2D1::ColorF(1.0f, 1.0f, 1.0f, m_isSeeking ? 0.28f : 0.18f),
                        D2D1::ColorF(D2D1::ColorF::White)
                    );

                    const std::wstring elapsedText = Utils::FormatDuration(displayElapsedSeconds);
                    std::wstring rightText = Utils::FormatDuration(m_mediaState.durationSeconds);
                    if (m_mediaState.durationSeconds > 0) {
                        // "-m:ss" like the iOS island (time left, not total).
                        rightText = L"-" + Utils::FormatDuration(
                            (std::max)(0, m_mediaState.durationSeconds - displayElapsedSeconds));
                    }
                    const D2D1_COLOR_F timeColor = D2D1::ColorF(0.68f, 0.69f, 0.72f, 0.95f);
                    // Long durations (h:mm:ss, "-h:mm:ss") shrink a step so the
                    // labels never get clipped at the pill edges.
                    const float leftFontSize = elapsedText.size() >= 7 ? 7.5f : 9.0f;
                    const float rightFontSize = rightText.size() >= 7 ? 7.5f : 9.0f;
                    // Labels are drawn top-aligned, so the rect top is chosen
                    // such that the digits' optical center sits on the bar's
                    // center line (bar 80..86, center y = 83): top + 6.5.
                    m_renderer->DrawTextCentered(
                        elapsedText,
                        D2D1::RectF(10.0f, 76.5f, 52.0f, 98.5f),
                        timeColor,
                        leftFontSize,
                        false
                    );
                    m_renderer->DrawTextCentered(
                        rightText,
                        D2D1::RectF(w - 52.0f, 76.5f, w - 10.0f, 98.5f),
                        timeColor,
                        rightFontSize,
                        false
                    );
                    
                    // Playback controls: each icon gets its own smooth hover animation.
                    float midX = w / 2.0f;
                    m_renderer->DrawCircularButton(
                        D2D1::RectF(midX - 58.0f, 100.0f, midX - 30.0f, 132.0f),
                        Graphics::IconType::Previous,
                        1.12f,
                        m_prevButtonHover.GetValue()
                    );
                    m_renderer->DrawCircularButtonTransition(
                        D2D1::RectF(midX - 16.0f, 98.0f, midX + 16.0f, 134.0f),
                        m_playbackIconFrom,
                        m_playbackIconTo,
                        m_playbackIconTransitionProgress,
                        1.40f,
                        m_playButtonHover.GetValue()
                    );
                    m_renderer->DrawCircularButton(
                        D2D1::RectF(midX + 30.0f, 100.0f, midX + 58.0f, 132.0f),
                        Graphics::IconType::Next,
                        1.12f,
                        m_nextButtonHover.GetValue()
                    );
                    break;
                }
                case EventType::Timer: {
                    m_renderer->DrawBigText(
                        Utils::FormatDuration(m_timerState.remainingSeconds),
                        D2D1::RectF(20.0f, 16.0f, 120.0f, 60.0f),
                        D2D1::ColorF(D2D1::ColorF::White)
                    );
                    
                    m_renderer->DrawTextString(
                        m_timerState.label,
                        D2D1::RectF(126.0f, 18.0f, w - 16.0f, 38.0f),
                        D2D1::ColorF(D2D1::ColorF::White),
                        14.0f,
                        true
                    );
                    
                    m_renderer->DrawTextString(
                        m_timerState.session,
                        D2D1::RectF(126.0f, 38.0f, w - 16.0f, 58.0f),
                        D2D1::ColorF(0.66f, 0.67f, 0.71f, 1.0f),
                        12.0f,
                        false
                    );
                    
                    float btnW = 90.0f;
                    float btnH = 28.0f;
                    float b1Left = (w - btnW * 2.0f - 16.0f) / 2.0f;
                    
                    std::wstring pauseText = m_timerState.isPaused ? L"Resume" : L"Pause";
                    m_renderer->DrawButtonPill(
                        D2D1::RectF(b1Left, 74.0f, b1Left + btnW, 74.0f + btnH),
                        pauseText,
                        false
                    );
                    m_renderer->DrawButtonPill(
                        D2D1::RectF(b1Left + btnW + 16.0f, 74.0f, b1Left + btnW * 2.0f + 16.0f, 74.0f + btnH),
                        L"Stop",
                        false
                    );
                    break;
                }
                case EventType::MicStatus: {
                    // Keep the expanded microphone indicator dot-only as well.
                    m_renderer->DrawStatusDot(
                        D2D1::Point2F(w / 2.0f, h / 2.0f),
                        5.0f,
                        D2D1::ColorF(1.0f, 0.624f, 0.039f, 1.0f)
                    );
                    break;
                }
                default:
                    break;
            }
            if (collapsingToCompact && rt) {
                rt->SetTransform(originalContentTransform);
                // Crossfade the compact player in as the expanded controls compress.
                float expandedHeight = Utils::ScaleDpiF(150.0f, m_cachedTaskbar.dpi);
                float compactHeight = compactThreshold;
                float contentScaleY = (expandedHeight > compactHeight)
                    ? Utils::Clamp((h - compactHeight) / (expandedHeight - compactHeight), 0.0f, 1.0f)
                    : 1.0f;
                RenderCompactContent(m_currentEvent, w, h, o * (1.0f - contentScaleY));
            }
        }
        
        if (rt) {
            rt->SetTransform(D2D1::Matrix3x2F::Identity());
        }
    }
    
    const HRESULT drawHr = m_renderer->EndDraw();
    if (SUCCEEDED(drawHr)) {
        m_renderer->PresentLayeredWindow(m_hwnd, m_windowX, m_windowY);
    }
}

LRESULT CALLBACK IslandWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    try {
        return WndProcImpl(hwnd, msg, wParam, lParam);
    } catch (const std::exception& e) {
        LogError(L"Unhandled C++ exception in window procedure: %S", e.what());
    } catch (...) {
        LogError(L"Unhandled unknown exception in window procedure");
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK IslandWindow::WndProcImpl(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    IslandWindow* self = nullptr;
    
    if (msg == WM_NCCREATE) {
        auto cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        self = reinterpret_cast<IslandWindow*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
    } else {
        self = reinterpret_cast<IslandWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }
    
    if (!self) {
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    
    if (self->m_taskbarCreatedMsg != 0 && msg == self->m_taskbarCreatedMsg) {
        LogInfo(L"TaskbarCreated window message received, adapting island window");
        self->OnTaskbarOrDisplayChanged();
        return 0;
    }
    
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;
        
        case WM_NCHITTEST: {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ScreenToClient(hwnd, &pt);
            if (self->IsPointInSquircle(static_cast<float>(pt.x), static_cast<float>(pt.y))) {
                return HTCLIENT;
            }
            return HTTRANSPARENT;
        }
        
        case WM_HOTKEY: {
            int hotkeyIdx = static_cast<int>(wParam) - HOTKEY_ID_BASE;
            if (self->m_serviceManager && hotkeyIdx >= 0 && hotkeyIdx <= 9) {
                self->m_serviceManager->TriggerTestScenario(hotkeyIdx);
            }
            return 0;
        }
        
        case WM_SETTINGCHANGE:
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
            self->OnTaskbarOrDisplayChanged();
            return 0;
        
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        
        case WM_TIMER:
            if (wParam == self->m_animTimerId) {
                self->TriggerAnimationUpdate();
            } else if (wParam == self->m_autoCollapseTimerId) {
                if (!self->m_isHovered && self->m_state == IslandState::Expanded) {
                    self->SetState(IslandState::Compact);
                }
            } else if (wParam == self->m_waveTimerId) {
                double now = Graphics::GetHighPrecisionTimeMs();
                if (self->m_lastMarqueeTimeMs > 0.0) {
                    double dt = (now - self->m_lastMarqueeTimeMs) / 1000.0;
                    if (dt > 0.1) dt = 0.1;
                    
                    self->m_marqueeOffset += static_cast<float>(dt * 16.0);
                    
                    if (self->m_currentEvent == EventType::Media &&
                        self->m_mediaState.isPlaying &&
                        self->m_state == IslandState::Compact &&
                        !self->m_animHeight.IsAnimating() &&
                        !self->m_mediaState.title.empty()) {
                        self->m_albumArtRotation = std::fmod(
                            self->m_albumArtRotation + static_cast<float>(dt * 18.0f), 360.0f
                        );
                    }
                }
                self->m_lastMarqueeTimeMs = now;
                
                if (self->m_state != IslandState::Hidden &&
                    self->m_currentEvent == EventType::Media && self->m_mediaState.isPlaying) {
                    self->Render();
                }
            } else if (wParam == self->m_geometryCheckTimerId) {
                self->CheckTaskbarGeometry();
            }
            return 0;
        
        case WM_MOUSEMOVE: {
            if (!self->m_isHovered) {
                self->m_isHovered = true;
                TRACKMOUSEEVENT tme{sizeof(TRACKMOUSEEVENT), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
                self->DisarmAutoCollapse();
            }
            
            const float pointerLocalX = static_cast<float>(GET_X_LPARAM(lParam)) - self->m_currentPillX;
            const float pointerLocalY = static_cast<float>(GET_Y_LPARAM(lParam)) - self->m_currentPillY;
            self->UpdateMediaButtonHover(pointerLocalX, pointerLocalY);

            if (self->m_isSeeking) {
                const float localX = pointerLocalX;
                const float w = self->m_animWidth.GetValue();
                self->UpdateSeekFromLocalX(localX, w);
            }
            
            if (self->m_isVolumeDragging) {
                const float localX = static_cast<float>(GET_X_LPARAM(lParam)) - self->m_currentPillX;
                const float w = self->m_animWidth.GetValue();
                self->UpdateVolumeDragFromLocalX(localX, w);
            }
            return 0;
        }
        
        case WM_MOUSELEAVE:
            self->m_isHovered = false;
            self->UpdateMediaButtonHover(-1.0f, -1.0f);
            if (self->m_isSeeking) {
                self->EndSeek(true);
            } else if (self->m_state == IslandState::Expanded) {
                self->ArmAutoCollapse();
            }
            return 0;
        
        case WM_LBUTTONDOWN: {
            self->m_isMouseDown = true;
            self->m_mouseDownX = GET_X_LPARAM(lParam);
            self->m_mouseDownY = GET_Y_LPARAM(lParam);
            SetCapture(hwnd);
            
            const float localX = static_cast<float>(self->m_mouseDownX) - self->m_currentPillX;
            const float localY = static_cast<float>(self->m_mouseDownY) - self->m_currentPillY;
            const float w = self->m_animWidth.GetValue();
            const float h = self->m_animHeight.GetValue();
            
            if (self->TryBeginVolumeDrag(localX, localY, w, h)) {
                return 0;
            }
            self->TryBeginSeek(localX, localY, w, h);
            return 0;
        }
        
        case WM_LBUTTONUP: {
            if (self->m_isMouseDown) {
                self->m_isMouseDown = false;
                ReleaseCapture();
                
                int upX = GET_X_LPARAM(lParam);
                int upY = GET_Y_LPARAM(lParam);
                
                if (self->m_isVolumeDragging) {
                    self->EndVolumeDrag();
                } else if (self->m_isSeeking) {
                    self->EndSeek(true);
                } else if (std::abs(upX - self->m_mouseDownX) < 10 && std::abs(upY - self->m_mouseDownY) < 10) {
                    self->OnClick(upX, upY);
                }
            }
            return 0;
        }
        
        case WM_CAPTURECHANGED:
            if (self->m_isSeeking) {
                self->EndSeek(true);
            }
            if (self->m_isVolumeDragging) {
                self->EndVolumeDrag();
            }
            self->m_isMouseDown = false;
            return 0;
        
        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            self->Render();
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        
        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}
} // namespace Overlay
} // namespace DynamicIsland

// ============================================================================
// [Module] src\services\service_manager.cpp
// ============================================================================
namespace DynamicIsland {
namespace Services {

ServiceManager::ServiceManager(Overlay::IslandWindow* window)
    : m_window(window) {
    if (m_window) {
        m_window->SetServiceManager(this);
    }
}

ServiceManager::~ServiceManager() {
    Shutdown();
}

void ServiceManager::Initialize() {
    RefreshVisibility();
    LogInfo(L"ServiceManager initialized");
}

void ServiceManager::Shutdown() {
    m_transientQueue.clear();
    m_hasActiveTransient = false;
}

EqBands ServiceManager::GetAudioBands() const {
    EqBands b{};
    if (m_audioService) {
        for (size_t i = 0; i < 6; ++i) b.values[i] = m_audioService->GetBand(i);
    }
    return b;
}

void ServiceManager::PostTransientEvent(const QueuedEvent& event) {
    if (m_hasActiveTransient && m_currentTransient.type == event.type) {
        m_currentTransient = event;
        m_transientExpiryTick = GetTickCount64() + static_cast<ULONGLONG>(event.durationMs);
        
        auto it = m_transientQueue.begin();
        while (it != m_transientQueue.end()) {
            if (it->type == event.type) {
                it = m_transientQueue.erase(it);
            } else {
                ++it;
            }
        }
        
        if (m_window) {
            TransientState ts;
            ts.type = event.type;
            ts.title = event.title;
            ts.subtitle = event.subtitle;
            ts.value = event.progressPercent;
            ts.progressFraction = static_cast<float>(event.progressPercent) / 100.0f;
            
            m_window->UpdateTransientState(ts);
            m_window->SetCurrentEvent(event.type);
            
            if (m_window->GetState() != IslandState::Expanded) {
                m_window->SetState(IslandState::Compact);
            }
        }
        return;
    }
    
    // Coalesce repeated queued notifications of the same type to keep the
    // queue bounded under high-frequency event sources.
    for (auto& queued : m_transientQueue) {
        if (queued.type == event.type) {
            queued = event;
            return;
        }
    }

    constexpr size_t kMaxTransientQueue = 32;
    if (m_transientQueue.size() >= kMaxTransientQueue) {
        m_transientQueue.pop_front();
    }
    m_transientQueue.push_back(event);

    if (!m_hasActiveTransient) {
        PumpNextEvent();
    }
}

void ServiceManager::SetLiveActivity(EventType type, bool active) {
    bool stateChanged = false;
    
    switch (type) {
        case EventType::Media:     if (m_mediaActive != active) { m_mediaActive = active; stateChanged = true; } break;
        case EventType::Timer:     if (m_timerActive != active) { m_timerActive = active; stateChanged = true; } break;
        case EventType::MicStatus: if (m_micActive != active)   { m_micActive = active;   stateChanged = true; } break;
        default: break;
    }

    // Media state is polled frequently. Avoid reapplying an already-correct
    // window state on every poll, but repair it if the window became out of sync.
    if (!stateChanged) {
        if (!m_window || m_hasActiveTransient) return;

        const EventType current = ResolveCurrentLiveActivity();
        const bool alwaysVisible =
            GetSettingsSnapshot().idleVisibilityMode == IdleVisibilityMode::AlwaysVisible;
        const IslandState windowState = m_window->GetState();
        const bool visibilityMatches = (current != EventType::None || alwaysVisible)
            ? windowState != IslandState::Hidden
            : (windowState == IslandState::Hidden || windowState == IslandState::Expanded);

        if (m_window->GetCurrentEvent() == current && visibilityMatches) return;
    }
    
    if (!m_hasActiveTransient && m_window) {
        EventType current = ResolveCurrentLiveActivity();
        if (current != EventType::None) {
            m_window->SetCurrentEvent(current);
            if (m_window->GetState() != IslandState::Expanded) {
                m_window->SetState(IslandState::Compact);
            }
        } else {
            if (GetSettingsSnapshot().idleVisibilityMode == IdleVisibilityMode::AlwaysVisible) {
                m_window->SetCurrentEvent(EventType::None);
                if (m_window->GetState() != IslandState::Expanded) {
                    m_window->SetState(IslandState::Compact);
                }
            } else {
                if (m_window->GetState() != IslandState::Expanded) {
                    m_window->SetState(IslandState::Hidden);
                }
            }
        }
    }
    
}

void ServiceManager::PumpNextEvent() {
    if (m_transientQueue.empty()) {
        m_hasActiveTransient = false;
        
        if (m_window) {
            EventType live = ResolveCurrentLiveActivity();
            if (live != EventType::None) {
                m_window->SetCurrentEvent(live);
                if (m_window->GetState() != IslandState::Expanded) {
                    m_window->SetState(IslandState::Compact);
                }
            } else {
                if (GetSettingsSnapshot().idleVisibilityMode == IdleVisibilityMode::AlwaysVisible) {
                    m_window->SetCurrentEvent(EventType::None);
                    if (m_window->GetState() != IslandState::Expanded) {
                        m_window->SetState(IslandState::Compact);
                    }
                } else {
                    m_window->SetState(IslandState::Hidden);
                }
            }
        }
        return;
    }
    
    m_currentTransient = m_transientQueue.front();
    m_transientQueue.pop_front();
    m_hasActiveTransient = true;
    m_transientExpiryTick = GetTickCount64() + static_cast<ULONGLONG>(m_currentTransient.durationMs);
    
    if (m_window) {
        TransientState ts;
        ts.type = m_currentTransient.type;
        ts.title = m_currentTransient.title;
        ts.subtitle = m_currentTransient.subtitle;
        ts.value = m_currentTransient.progressPercent;
        ts.progressFraction = static_cast<float>(m_currentTransient.progressPercent) / 100.0f;
        
        m_window->UpdateTransientState(ts);
        m_window->SetCurrentEvent(m_currentTransient.type);
        
        if (m_window->GetState() != IslandState::Expanded) {
            m_window->SetState(IslandState::Compact);
        }
    }
}

bool ServiceManager::GetMediaThumbnail(std::vector<uint8_t>& pixels, uint32_t& width, uint32_t& height, uint64_t& version) {
    if (m_mediaService) {
        return m_mediaService->CopyThumbnail(pixels, width, height, version);
    }
    return false;
}

void ServiceManager::Update() {
    if (m_hasActiveTransient) {
        if (GetTickCount64() >= m_transientExpiryTick) {
            PumpNextEvent();
        }
    }

    if (m_mediaPausedSinceTick != 0 && !m_mediaPauseExpired &&
        GetTickCount64() - m_mediaPausedSinceTick >= 5000) {
        m_mediaPauseExpired = true;
        SetLiveActivity(EventType::Media, false);
    }
}

void ServiceManager::UpdateMediaState(const MediaState& state) {
    bool trackChanged = (m_lastTrackTitle != state.title);
    m_lastTrackTitle = state.title;

    if (state.isPlaying || state.title.empty()) {
        m_mediaPausedSinceTick = 0;
        m_mediaPauseExpired = false;
    } else if (trackChanged || (m_mediaPausedSinceTick == 0 && !m_mediaPauseExpired)) {
        m_mediaPausedSinceTick = GetTickCount64();
        m_mediaPauseExpired = false;
    }
    
    if (m_window) {
        m_window->UpdateMediaState(state);
        // Media state is polled independently of the window animation timer.
        // Render immediately so the elapsed-time label and progress bar visibly advance.
        if (m_window->GetHwnd() && IsWindowVisible(m_window->GetHwnd())) {
            m_window->Render();
            if (state.isPlaying) SetTimer(m_window->GetHwnd(), 1003, 33, nullptr);
            else KillTimer(m_window->GetHwnd(), 1003);
        }
    }
    
    const ModSettings settings = GetSettingsSnapshot();
    if (settings.mediaVisibilityPolicy == MediaVisibilityPolicy::TrackChangeOnly) {
        if (trackChanged && state.isPlaying && !state.title.empty()) {
            QueuedEvent qe;
            qe.type = EventType::Media;
            qe.durationMs = 3500;
            qe.title = state.title;
            qe.subtitle = state.artist;
            PostTransientEvent(qe);
        }
    } else {
        // Keep a paused track visible for five seconds, then let the island
        // return to its normal idle state until playback resumes or the track changes.
        const bool pausedExpired = !state.isPlaying && !state.title.empty() && m_mediaPauseExpired;
        SetLiveActivity(EventType::Media, state.isPlaying || (!state.title.empty() && !pausedExpired));
    }
}

void ServiceManager::UpdateTimerState(const TimerState& state) {
    if (m_window) {
        m_window->UpdateTimerState(state);
        if (m_window->GetCurrentEvent() == EventType::Timer) m_window->Render();
    }
}

void ServiceManager::UpdateMicState(const MicState& state) {
    if (m_window) m_window->UpdateMicState(state);
    SetLiveActivity(EventType::MicStatus, state.isActive);
    if (m_window && m_window->GetCurrentEvent() == EventType::MicStatus) m_window->Render();
}

void ServiceManager::OnMediaPlayPause() {
    if (m_mediaService && m_mediaService->TogglePlayPause()) {
        return;
    }

    // Demo/fallback state when no real system media session is available.
    if (m_window) {
        MediaState state = m_window->GetMediaState();
        state.isPlaying = !state.isPlaying;
        UpdateMediaState(state);
    }
}

void ServiceManager::OnMediaPrev() {
    if (m_mediaService && m_mediaService->HasRealSession()) {
        m_mediaService->Previous();
    }
    
    if (m_window) {
        m_window->GetMediaState().progress = 0.0f;
        if (m_window->GetState() == IslandState::Expanded) {
            m_window->SetState(IslandState::Compact);
        }
        m_window->TriggerAnimationUpdate();
    }
}

void ServiceManager::OnMediaNext() {
    if (m_mediaService && m_mediaService->HasRealSession()) {
        m_mediaService->Next();
    }
    
    if (m_window) {
        m_window->GetMediaState().progress = 0.0f;
        if (m_window->GetState() == IslandState::Expanded) {
            m_window->SetState(IslandState::Compact);
        }
        m_window->TriggerAnimationUpdate();
    }
}

void ServiceManager::OnMediaSeek(float progressFraction) {
    progressFraction = Utils::Clamp(progressFraction, 0.0f, 1.0f);
    
    if (m_mediaService && m_mediaService->HasRealSession()) {
        m_mediaService->Seek(progressFraction);
        return;
    }
    
    // Demo / fallback path when no real GSMTC session is available.
    if (m_window) {
        MediaState state = m_window->GetMediaState();
        if (state.durationSeconds > 0) {
            state.progress = progressFraction;
            state.elapsedSeconds = static_cast<int>(progressFraction * static_cast<float>(state.durationSeconds) + 0.5f);
            UpdateMediaState(state);
        }
    }
}

void ServiceManager::OnVolumeScrub(float volumeFraction) {
    volumeFraction = Utils::Clamp(volumeFraction, 0.0f, 1.0f);
    if (m_audioService) m_audioService->SetVolume(volumeFraction);
}

void ServiceManager::OnTimerTogglePause() {
    if (!GetSettingsSnapshot().enableTimer || !m_timerService || !m_timerService->IsActive()) return;
    if (m_timerService->IsPaused()) m_timerService->Resume();
    else m_timerService->Pause();
}

void ServiceManager::OnTimerStop() {
    if (GetSettingsSnapshot().enableTimer && m_timerService) m_timerService->Stop();
}

void ServiceManager::FireTransient(EventType type) {
    QueuedEvent qe;
    qe.type = type;
    
    switch (type) {
        case EventType::Volume:
            qe.durationMs = DURATION_VOLUME_MS;
            qe.title = L"Volume";
            qe.progressPercent = 68;
            break;
        case EventType::CapsLock:
            qe.durationMs = DURATION_CAPS_LOCK_MS;
            qe.title = L"Caps Lock";
            qe.subtitle = L"ON";
            break;
        case EventType::Power:
            qe.durationMs = DURATION_POWER_MS;
            qe.title = L"Charging";
            qe.progressPercent = 62;
            break;
        case EventType::Bluetooth:
            qe.durationMs = DURATION_BLUETOOTH_MS;
            qe.title = L"WH-1000XM5";
            qe.progressPercent = 80;
            break;
        case EventType::LowBattery:
            qe.durationMs = DURATION_LOW_BATT_MS;
            qe.title = L"Low battery";
            qe.progressPercent = 15;
            break;
        case EventType::TimerDone:
            qe.durationMs = DURATION_TIMER_DONE_MS;
            qe.title = L"Timer done";
            break;
        default:
            return;
    }
    
    PostTransientEvent(qe);
}

void ServiceManager::TriggerTestScenario(int index) {
    if (index >= 0 && index < 9) {
        m_demoIndex = index;
    } else {
        m_demoIndex = (m_demoIndex + 1) % 9;
    }
    
    m_transientQueue.clear();
    m_hasActiveTransient = false;
    
    switch (m_demoIndex) {
        case 0: {
            m_timerActive = false;
            m_micActive = false;
            
            MediaState demoMedia;
            demoMedia.title = L"Senbonzakura";
            demoMedia.artist = L"Kurousa-P feat. Hatsune Miku";
            demoMedia.isPlaying = true;
            demoMedia.progress = 0.3f;
            
            if (m_window) {
                m_window->UpdateMediaState(demoMedia);
            }
            SetLiveActivity(EventType::Media, true);
            break;
        }
        case 1:
            if (!GetSettingsSnapshot().enableTimer) break;
            m_mediaActive = false;
            m_micActive = false;
            if (m_timerService && !m_timerService->IsActive()) {
                m_timerService->Start(30);
            }
            SetLiveActivity(EventType::Timer, true);
            break;
        case 2: {
            m_mediaActive = false;
            m_timerActive = false;
            
            MicState demoMic;
            demoMic.title = L"Microphone in use";
            demoMic.appName = L"Discord";
            demoMic.isActive = true;
            
            if (m_window) {
                m_window->UpdateMicState(demoMic);
            }
            SetLiveActivity(EventType::MicStatus, true);
            break;
        }
        case 3:
            FireTransient(EventType::Volume);
            break;
        case 4:
            FireTransient(EventType::CapsLock);
            break;
        case 5:
            FireTransient(EventType::Power);
            break;
        case 6:
            FireTransient(EventType::Bluetooth);
            break;
        case 7:
            FireTransient(EventType::LowBattery);
            break;
        case 8:
            FireTransient(EventType::TimerDone);
            break;
    }
}


void ServiceManager::RefreshVisibility() {
    if (!m_hasActiveTransient && m_window) {
        EventType current = ResolveCurrentLiveActivity();
        if (current != EventType::None) {
            m_window->SetCurrentEvent(current);
            if (m_window->GetState() != IslandState::Expanded) {
                m_window->SetState(IslandState::Compact);
            }
        } else {
            if (GetSettingsSnapshot().idleVisibilityMode == IdleVisibilityMode::AlwaysVisible) {
                m_window->SetCurrentEvent(EventType::None);
                if (m_window->GetState() != IslandState::Expanded) {
                    m_window->SetState(IslandState::Compact);
                }
            } else {
                if (m_window->GetState() != IslandState::Expanded) {
                    m_window->SetState(IslandState::Hidden);
                }
            }
        }
    }
}

EventType ServiceManager::ResolveCurrentLiveActivity() const {
    if (m_micActive)   return EventType::MicStatus;
    if (m_timerActive) return EventType::Timer;
    if (m_mediaActive) return EventType::Media;
    return EventType::None;
}
} // namespace Services
} // namespace DynamicIsland

// ============================================================================
// [Module] src\services\media_service.cpp
// ============================================================================
namespace DynamicIsland {
namespace Services {

namespace {

using Microsoft::WRL::ComPtr;

bool DecodeBytesToPBGRA(
    const uint8_t* data,
    size_t size,
    uint32_t& outWidth,
    uint32_t& outHeight,
    std::vector<uint8_t>& outPixels
) {
    if (!data || size == 0) return false;
    
    ComPtr<IWICImagingFactory> factory;
    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&factory)
    );
    if (FAILED(hr) || !factory) return false;
    
    ComPtr<IWICStream> stream;
    hr = factory->CreateStream(&stream);
    if (FAILED(hr) || !stream) return false;
    
    hr = stream->InitializeFromMemory(const_cast<BYTE*>(data), static_cast<DWORD>(size));
    if (FAILED(hr)) return false;
    
    ComPtr<IWICBitmapDecoder> decoder;
    hr = factory->CreateDecoderFromStream(
        stream.Get(),
        nullptr,
        WICDecodeMetadataCacheOnDemand,
        &decoder
    );
    if (FAILED(hr) || !decoder) return false;
    
    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr) || !frame) return false;
    
    ComPtr<IWICFormatConverter> converter;
    hr = factory->CreateFormatConverter(&converter);
    if (FAILED(hr) || !converter) return false;
    
    hr = converter->Initialize(
        frame.Get(),
        GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone,
        nullptr,
        0.0f,
        WICBitmapPaletteTypeCustom
    );
    if (FAILED(hr)) return false;
    
    UINT w = 0, h = 0;
    converter->GetSize(&w, &h);
    if (w == 0 || h == 0) return false;

    // Album art is displayed at small sizes; cap decoded dimensions to avoid
    // unexpectedly large allocations from malformed or oversized images.
    constexpr UINT kMaxThumbnailSide = 512;
    if (w > kMaxThumbnailSide || h > kMaxThumbnailSide) {
        const double scale = (std::min)(
            static_cast<double>(kMaxThumbnailSide) / static_cast<double>(w),
            static_cast<double>(kMaxThumbnailSide) / static_cast<double>(h));
        const UINT scaledW = (std::max)(1u, static_cast<UINT>(w * scale));
        const UINT scaledH = (std::max)(1u, static_cast<UINT>(h * scale));
        ComPtr<IWICBitmapScaler> scaler;
        hr = factory->CreateBitmapScaler(&scaler);
        if (FAILED(hr) || !scaler) return false;
        hr = scaler->Initialize(converter.Get(), scaledW, scaledH, WICBitmapInterpolationModeFant);
        if (FAILED(hr)) return false;
        ComPtr<IWICFormatConverter> scaledConverter;
        hr = factory->CreateFormatConverter(&scaledConverter);
        if (FAILED(hr) || !scaledConverter) return false;
        hr = scaledConverter->Initialize(
            scaler.Get(), GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);
        if (FAILED(hr)) return false;
        converter = scaledConverter;
        w = scaledW;
        h = scaledH;
    }

    // Small sources (e.g. 64x64 GSMTC thumbnails) look heavily pixelated when
    // the pill stretches them to ~44-52px with a plain linear brush. Upscale
    // them once to a comfortable resolution with the high-quality Fant filter,
    // so the brush only ever downscales at render time.
    constexpr UINT kMinThumbnailSide = 192;
    if (w < kMinThumbnailSide || h < kMinThumbnailSide) {
        const double scale = (std::max)(
            static_cast<double>(kMinThumbnailSide) / static_cast<double>(w),
            static_cast<double>(kMinThumbnailSide) / static_cast<double>(h));
        const UINT scaledW = (std::max)(1u, static_cast<UINT>(w * scale + 0.5));
        const UINT scaledH = (std::max)(1u, static_cast<UINT>(h * scale + 0.5));
        // Never let the upscale push the buffer past the size limit below,
        // otherwise unusually shaped art would get rejected entirely.
        const bool fitsLimit = static_cast<size_t>(scaledW) * static_cast<size_t>(scaledH) * 4u
            <= 512u * 512u * 4u;
        if (fitsLimit) {
        ComPtr<IWICBitmapScaler> upscaler;
        hr = factory->CreateBitmapScaler(&upscaler);
        if (SUCCEEDED(hr) && upscaler) {
            hr = upscaler->Initialize(converter.Get(), scaledW, scaledH, WICBitmapInterpolationModeFant);
            if (SUCCEEDED(hr)) {
                ComPtr<IWICFormatConverter> scaledConverter;
                hr = factory->CreateFormatConverter(&scaledConverter);
                if (SUCCEEDED(hr) && scaledConverter) {
                    hr = scaledConverter->Initialize(
                        upscaler.Get(), GUID_WICPixelFormat32bppPBGRA,
                        WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeCustom);
                    if (SUCCEEDED(hr)) {
                        converter = scaledConverter;
                        w = scaledW;
                        h = scaledH;
                    }
                }
            }
        }
        }
        // If the upscale failed for any reason, keep using the original
        // bitmap rather than rejecting the artwork outright.
    }

    const size_t bufferSize64 = static_cast<size_t>(w) * static_cast<size_t>(h) * 4u;
    if (bufferSize64 == 0 || bufferSize64 > 512u * 512u * 4u) return false;
    const UINT stride = w * 4u;
    const UINT bufferSize = static_cast<UINT>(bufferSize64);
    outPixels.resize(bufferSize);

    hr = converter->CopyPixels(nullptr, stride, bufferSize, outPixels.data());
    if (FAILED(hr)) return false;
    
    outWidth = w;
    outHeight = h;
    return true;
}

#if defined(DYNAMIC_ISLAND_HAS_WINRT_GSMTC)

struct ThumbnailFetchContext {
    MediaService* self = nullptr;
    winrt::Windows::Storage::Streams::IRandomAccessStreamReference thumbRef{nullptr};
    std::wstring trackId;
};

DWORD WINAPI ThumbnailWorkerThreadProc(LPVOID param) {
    auto* ctx = reinterpret_cast<ThumbnailFetchContext*>(param);
    if (!ctx || !ctx->self) {
        delete ctx;
        return 0;
    }
    
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
    } catch (const winrt::hresult_error& e) {
        MediaService* self = ctx->self;
        LogError(L"Thumbnail worker WinRT initialization failed (HRESULT=0x%08X): %s",
                 static_cast<unsigned>(e.code()), e.message().c_str());
        self->FinishThumbnailFetch();
        self->RequestiTunesArtworkFallback(ctx->trackId);
        delete ctx;
        // Decrement only after the fallback has had a chance to register itself.
        self->ThumbnailWorkerFinished();
        return 0;
    } catch (...) {
        MediaService* self = ctx->self;
        LogError(L"Thumbnail worker WinRT initialization failed (unknown error)");
        self->FinishThumbnailFetch();
        self->RequestiTunesArtworkFallback(ctx->trackId);
        delete ctx;
        self->ThumbnailWorkerFinished();
        return 0;
    }

    bool storedThumbnail = false;

    try {
        if (ctx->thumbRef) {
            using winrt::Windows::Foundation::AsyncStatus;
            auto openOperation = ctx->thumbRef.OpenReadAsync();
            if (openOperation.wait_for(std::chrono::seconds(5)) == AsyncStatus::Completed) {
                auto stream = openOperation.GetResults();
                if (stream) {
                    uint32_t sz = static_cast<uint32_t>(stream.Size());
                    if (sz > 0 && sz < 20 * 1024 * 1024) {
                        winrt::Windows::Storage::Streams::DataReader reader(stream);
                        auto loadOperation = reader.LoadAsync(sz);
                        if (loadOperation.wait_for(std::chrono::seconds(5)) == AsyncStatus::Completed) {
                            loadOperation.GetResults();

                            std::vector<uint8_t> streamBytes(reader.UnconsumedBufferLength());
                            reader.ReadBytes(winrt::array_view<uint8_t>(streamBytes));

                            uint32_t width = 0, height = 0;
                            std::vector<uint8_t> pbgraPixels;

                            if (DecodeBytesToPBGRA(streamBytes.data(), streamBytes.size(), width, height, pbgraPixels)) {
                                ctx->self->StoreDecodedThumbnail(std::move(pbgraPixels), width, height, ctx->trackId);
                                storedThumbnail = true;
                            }
                        } else {
                            // Do not let a stalled GSMTC stream hold Stop() open.
                            loadOperation.Cancel();
                        }
                    }
                }
            } else {
                // Abandon a stream-open operation that did not finish in time.
                openOperation.Cancel();
            }
        }
    } catch (...) {}
    
    MediaService* self = ctx->self;
    self->FinishThumbnailFetch();
    
    if (!storedThumbnail) {
        // GSMTC provided no usable artwork — try the iTunes Search API.
        self->RequestiTunesArtworkFallback(ctx->trackId);
    }
    
    delete ctx;

    winrt::uninit_apartment();
    // Keep this worker counted until any replacement/fallback worker is registered.
    self->ThumbnailWorkerFinished();
    return 0;
}

#endif

// ---- iTunes Search API artwork fallback helpers ----------------------------

std::wstring WideFromUtf8(const std::string& text) {
    if (text.empty()) return std::wstring();
    const int len = MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0);
    std::wstring out(static_cast<size_t>(len), L'\0');
    if (len > 0) {
        MultiByteToWideChar(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), &out[0], len);
    }
    return out;
}

std::wstring PercentEncodeUtf8(const std::wstring& text) {
    if (text.empty()) return text;
    const int len = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
    std::string utf8(static_cast<size_t>(len), '\0');
    if (len > 0) {
        WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), &utf8[0], len, nullptr, nullptr);
    }
    
    static const wchar_t* kHex = L"0123456789ABCDEF";
    auto isUnreserved = [](unsigned char c) {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
               (c >= '0' && c <= '9') || c == '-' || c == '.' || c == '_' || c == '~';
    };
    
    std::wstring out;
    out.reserve(utf8.size() * 3);
    for (unsigned char c : utf8) {
        if (isUnreserved(c)) {
            out += static_cast<wchar_t>(c);
        } else {
            out += L'%';
            out += kHex[c >> 4];
            out += kHex[c & 0x0F];
        }
    }
    return out;
}

namespace {
bool ParseJsonStringToken(const std::string& json, size_t& pos, std::string& utf8) {
    if (pos >= json.size() || json[pos] != '"') return false;
    ++pos;
    utf8.clear();

    auto appendCodePoint = [&utf8](uint32_t cp) {
        if (cp <= 0x7F) {
            utf8.push_back(static_cast<char>(cp));
        } else if (cp <= 0x7FF) {
            utf8.push_back(static_cast<char>(0xC0 | (cp >> 6)));
            utf8.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        } else if (cp <= 0xFFFF) {
            utf8.push_back(static_cast<char>(0xE0 | (cp >> 12)));
            utf8.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            utf8.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        } else if (cp <= 0x10FFFF) {
            utf8.push_back(static_cast<char>(0xF0 | (cp >> 18)));
            utf8.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
            utf8.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            utf8.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
    };

    auto readHex4 = [&json, &pos](uint32_t& value) -> bool {
        if (pos + 4 > json.size()) return false;
        value = 0;
        for (int i = 0; i < 4; ++i) {
            const char c = json[pos++];
            value <<= 4;
            if (c >= '0' && c <= '9') value |= static_cast<uint32_t>(c - '0');
            else if (c >= 'a' && c <= 'f') value |= static_cast<uint32_t>(c - 'a' + 10);
            else if (c >= 'A' && c <= 'F') value |= static_cast<uint32_t>(c - 'A' + 10);
            else return false;
        }
        return true;
    };

    while (pos < json.size()) {
        const unsigned char c = static_cast<unsigned char>(json[pos++]);
        if (c == '"') return true;
        if (c < 0x20) return false;
        if (c != '\\') {
            utf8.push_back(static_cast<char>(c));
            continue;
        }
        if (pos >= json.size()) return false;
        const char escaped = json[pos++];
        switch (escaped) {
            case '"': utf8.push_back('"'); break;
            case '\\': utf8.push_back('\\'); break;
            case '/': utf8.push_back('/'); break;
            case 'b': utf8.push_back('\b'); break;
            case 'f': utf8.push_back('\f'); break;
            case 'n': utf8.push_back('\n'); break;
            case 'r': utf8.push_back('\r'); break;
            case 't': utf8.push_back('\t'); break;
            case 'u': {
                uint32_t cp = 0;
                if (!readHex4(cp)) return false;
                if (cp >= 0xD800 && cp <= 0xDBFF) {
                    if (pos + 2 > json.size() || json[pos] != '\\' || json[pos + 1] != 'u') return false;
                    pos += 2;
                    uint32_t low = 0;
                    if (!readHex4(low) || low < 0xDC00 || low > 0xDFFF) return false;
                    cp = 0x10000 + ((cp - 0xD800) << 10) + (low - 0xDC00);
                } else if (cp >= 0xDC00 && cp <= 0xDFFF) {
                    return false;
                }
                appendCodePoint(cp);
                break;
            }
            default: return false;
        }
    }
    return false;
}
}

std::wstring ExtractFirstJsonStringValue(const std::string& json, const char* key) {
    if (!key || !*key) return std::wstring();
    size_t pos = 0;
    while (pos < json.size()) {
        if (json[pos] != '"') {
            ++pos;
            continue;
        }

        std::string token;
        if (!ParseJsonStringToken(json, pos, token)) return std::wstring();

        size_t lookahead = pos;
        while (lookahead < json.size() &&
               (json[lookahead] == ' ' || json[lookahead] == '\t' ||
                json[lookahead] == '\r' || json[lookahead] == '\n')) {
            ++lookahead;
        }
        if (lookahead >= json.size() || json[lookahead] != ':') continue;
        if (token != key) continue;

        pos = lookahead + 1;
        while (pos < json.size() &&
               (json[pos] == ' ' || json[pos] == '\t' ||
                json[pos] == '\r' || json[pos] == '\n')) {
            ++pos;
        }
        std::string value;
        if (!ParseJsonStringToken(json, pos, value)) return std::wstring();
        return WideFromUtf8(value);
    }
    return std::wstring();
}

bool HttpGetBinary(const std::wstring& url, std::vector<uint8_t>& out, size_t maxBytes) {
    out.clear();
    if (url.empty() || maxBytes == 0) return false;

    URL_COMPONENTS uc{};
    wchar_t hostName[256] = {};
    wchar_t urlPath[2048] = {};
    wchar_t extraInfo[2048] = {};
    uc.dwStructSize = sizeof(uc);
    uc.lpszHostName = hostName;
    uc.dwHostNameLength = _countof(hostName) - 1;
    uc.lpszUrlPath = urlPath;
    uc.dwUrlPathLength = _countof(urlPath) - 1;
    uc.lpszExtraInfo = extraInfo;
    uc.dwExtraInfoLength = _countof(extraInfo) - 1;

    if (!WinHttpCrackUrl(url.c_str(), static_cast<DWORD>(url.size()), 0, &uc)) {
        return false;
    }
    if ((uc.nScheme != INTERNET_SCHEME_HTTP && uc.nScheme != INTERNET_SCHEME_HTTPS) ||
        uc.dwHostNameLength == 0) {
        return false;
    }

    std::wstring requestPath(urlPath, uc.dwUrlPathLength);
    if (requestPath.empty()) requestPath = L"/";
    if (uc.dwExtraInfoLength > 0) {
        requestPath.append(extraInfo, uc.dwExtraInfoLength);
    }

    HINTERNET hSession = WinHttpOpen(
        L"DynamicIsland-Windhawk-Mod",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0);
    if (!hSession) return false;

    // Bound DNS, connection, send and receive waits so Stop() cannot be held
    // indefinitely by a stalled artwork request.
    if (!WinHttpSetTimeouts(hSession, 5000, 5000, 5000, 5000)) {
        WinHttpCloseHandle(hSession);
        return false;
    }

    bool ok = false;
    HINTERNET hConnect = WinHttpConnect(hSession, hostName, uc.nPort, 0);
    if (hConnect) {
        const DWORD secureFlag = (uc.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0;
        HINTERNET hRequest = WinHttpOpenRequest(
            hConnect, L"GET", requestPath.c_str(), nullptr,
            WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, secureFlag);
        if (hRequest) {
            if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                   WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
                WinHttpReceiveResponse(hRequest, nullptr)) {
                DWORD statusCode = 0;
                DWORD statusSize = sizeof(statusCode);
                if (WinHttpQueryHeaders(hRequest,
                                        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                                        WINHTTP_HEADER_NAME_BY_INDEX,
                                        &statusCode, &statusSize, WINHTTP_NO_HEADER_INDEX) &&
                    statusCode == 200) {
                    ok = true;
                    for (;;) {
                        DWORD available = 0;
                        if (!WinHttpQueryDataAvailable(hRequest, &available)) {
                            ok = false;
                            break;
                        }
                        if (available == 0) break;
                        if (available > maxBytes || out.size() > maxBytes - available) {
                            ok = false;
                            break;
                        }
                        std::vector<uint8_t> chunk(available);
                        DWORD readBytes = 0;
                        if (!WinHttpReadData(hRequest, chunk.data(), available, &readBytes)) {
                            ok = false;
                            break;
                        }
                        if (readBytes == 0) break;
                        out.insert(out.end(), chunk.begin(), chunk.begin() + readBytes);
                    }
                    if (out.empty()) ok = false;
                }
            }
            WinHttpCloseHandle(hRequest);
        }
        WinHttpCloseHandle(hConnect);
    }
    WinHttpCloseHandle(hSession);
    if (!ok) out.clear();
    return ok;
}
} // namespace

void MediaService::TriggeriTunesArtworkFetch(const std::wstring& trackId, const std::wstring& title, const std::wstring& artist) {
    if (!m_isRunning || !GetSettingsSnapshot().enableOnlineArtworkLookup) return;
    
    {
        std::lock_guard<std::mutex> lock(m_thumbMutex);
        // Only fetch while this track is still the one being displayed.
        if (m_lastFetchedTrackId != trackId) return;
    }
    
    if (m_isFetchingItunes.exchange(true)) return;
    
    auto* ctx = new iTunesFetchContext();
    ctx->self = this;
    ctx->trackId = trackId;
    ctx->title = title;
    ctx->artist = artist;
    
    // Shares the active-worker accounting with GSMTC thumbnail workers so
    // Stop() waits for this fetch too before tearing the service down.
    m_activeThumbnailWorkers.fetch_add(1);
    UniqueWinHandle hThread(CreateThread(nullptr, 0, iTunesArtworkWorkerThreadStatic, ctx, 0, nullptr));
    if (hThread) {
        // Detached worker handle is closed automatically; active-worker accounting
        // keeps the owning service alive until the worker exits.
    } else {
        m_isFetchingItunes = false;
        delete ctx;
        ThumbnailWorkerFinished();
    }
}

void MediaService::RequestiTunesArtworkFallback(const std::wstring& trackId) {
    std::wstring title, artist;
    {
        std::lock_guard<std::mutex> lock(m_thumbMutex);
        title = m_state.title;
        artist = m_state.artist;
    }
    TriggeriTunesArtworkFetch(trackId, title, artist);
}

DWORD WINAPI MediaService::iTunesArtworkWorkerThreadStatic(LPVOID param) {
    auto* ctx = static_cast<iTunesFetchContext*>(param);
    if (!ctx || !ctx->self) {
        delete ctx;
        return 0;
    }
    
    const HRESULT comInitHr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    MediaService* self = ctx->self;
    if (FAILED(comInitHr)) {
        LogError(L"Online artwork worker COM initialization failed (HRESULT=0x%08X)",
                 static_cast<unsigned>(comInitHr));
        self->m_isFetchingItunes = false;
        self->ThumbnailWorkerFinished();
        delete ctx;
        return 0;
    }
    
    // Look the track up on the iTunes Search API and download its artwork.
    // On any failure the bitmap stays cleared and the island shows its
    // placeholder. Keep exceptions inside this detached worker thread.
    try {
        std::wstring term = ctx->artist.empty() ? ctx->title : (ctx->artist + L" " + ctx->title);
        if (!term.empty()) {
            const std::wstring searchUrl =
                L"https://itunes.apple.com/search?term=" + PercentEncodeUtf8(term) +
                L"&entity=song&limit=1&media=music";
            
            std::vector<uint8_t> jsonBytes;
            if (HttpGetBinary(searchUrl, jsonBytes, 512 * 1024)) {
                const std::string json(jsonBytes.begin(), jsonBytes.end());
                std::wstring artworkUrl = ExtractFirstJsonStringValue(json, "artworkUrl100");
                if (artworkUrl.empty()) {
                    artworkUrl = ExtractFirstJsonStringValue(json, "artworkUrl60");
                }
                
                if (!artworkUrl.empty()) {
                    std::vector<uint8_t> imageBytes;
                    if (HttpGetBinary(artworkUrl, imageBytes, 10 * 1024 * 1024)) {
                        uint32_t width = 0, height = 0;
                        std::vector<uint8_t> pbgraPixels;
                        if (DecodeBytesToPBGRA(imageBytes.data(), imageBytes.size(), width, height, pbgraPixels)) {
                            self->StoreDecodedThumbnail(std::move(pbgraPixels), width, height, ctx->trackId);
                        }
                    }
                }
            }
        }
    } catch (const std::exception&) {
        LogError(L"Online artwork lookup failed (C++ exception)");
    } catch (...) {
        LogError(L"Online artwork lookup failed (unknown exception)");
    }
    
    self->m_isFetchingItunes = false;
    self->ThumbnailWorkerFinished();
    delete ctx;
    
    if (SUCCEEDED(comInitHr)) CoUninitialize();
    return 0;
}

MediaService::MediaService(ServiceManager* manager)
    : m_manager(manager) {
    m_state.title.clear();
    m_state.artist.clear();
    m_state.isPlaying = false;
    m_state.progress = 0.0f;
    m_state.elapsedSeconds = 0;
    m_state.durationSeconds = 0;
}

MediaService::~MediaService() {
    Stop();
}

void MediaService::Start() {
    if (m_isRunning.exchange(true)) return;

    m_pollStop = false;
    m_pollThread = CreateThread(nullptr, 0, PollThreadProcStatic, this, 0, &m_pollThreadId);
    if (!m_pollThread) {
        m_isRunning = false;
        m_pollThreadId = 0;
        LogError(L"MediaService failed to create polling thread (error=%lu)", GetLastError());
        return;
    }

    LogInfo(L"MediaService started");
}

void MediaService::Stop() {
    m_isRunning = false;
    m_pollStop = true;

#if defined(DYNAMIC_ISLAND_HAS_WINRT_GSMTC)
    {
        std::lock_guard<std::mutex> lock(m_thumbMutex);
        m_pendingThumbRef = nullptr;
        m_pendingTrackId.clear();
    }
#endif

    if (m_pollThread) {
        // The service object must remain alive until the polling thread exits.
        WaitForSingleObject(m_pollThread, INFINITE);
        m_pollThread.reset();
        m_pollThreadId = 0;
    }

    {
        std::unique_lock<std::mutex> lock(m_thumbWorkerMutex);
        m_thumbWorkerCv.wait(lock, [this] {
            return m_activeThumbnailWorkers.load() == 0;
        });
    }

    {
        std::lock_guard<std::mutex> lock(m_sessionMutex);
#if defined(DYNAMIC_ISLAND_HAS_WINRT_GSMTC)
        m_currentSession = nullptr;
#endif
    }

    m_hasRealSession = false;
    { std::lock_guard<std::mutex> lock(m_thumbMutex); m_lastFetchedTrackId.clear(); }
    
    {
        std::lock_guard<std::mutex> lock(m_thumbMutex);
        m_thumbPixels.clear();
        m_thumbWidth = 0;
        m_thumbHeight = 0;
        m_thumbVersion++;
        m_thumbCache.clear();
        
        m_state.isPlaying = false;
        m_state.title.clear();
        m_state.artist.clear();
        m_state.progress = 0.0f;
        m_state.elapsedSeconds = 0;
        m_state.durationSeconds = 0;
    }
    
    if (m_manager) {
        m_manager->UpdateMediaState(SnapshotState());
        m_manager->SetLiveActivity(EventType::Media, false);
    }
}

bool MediaService::CopyThumbnail(std::vector<uint8_t>& outPixels, uint32_t& outW, uint32_t& outH, uint64_t& outVersion) const {
    std::lock_guard<std::mutex> lock(m_thumbMutex);
    
    if (m_thumbVersion == outVersion) {
        return false;
    }
    
    outVersion = m_thumbVersion;
    outPixels = m_thumbPixels;
    outW = m_thumbWidth;
    outH = m_thumbHeight;
    return true;
}

void MediaService::StoreDecodedThumbnail(std::vector<uint8_t> pixels, uint32_t width, uint32_t height, const std::wstring& trackId) {
    std::lock_guard<std::mutex> lock(m_thumbMutex);
    if (!m_isRunning) return;

    if (m_thumbCache.size() >= kThumbCacheMax) {
        m_thumbCache.clear();
    }
    
    CachedThumb cached;
    cached.pixels = pixels;
    cached.w = width;
    cached.h = height;
    m_thumbCache[trackId] = std::move(cached);
    
    if (m_lastFetchedTrackId == trackId) {
        m_thumbPixels = std::move(pixels);
        m_thumbWidth = width;
        m_thumbHeight = height;
        m_thumbVersion++;
    }
    
    m_stateDirty = true;
}

void MediaService::FinishThumbnailFetch() {
    m_isFetchingThumbnail = false;

#if defined(DYNAMIC_ISLAND_HAS_WINRT_GSMTC)
    winrt::Windows::Storage::Streams::IRandomAccessStreamReference pending;
    std::wstring pendingId;

    {
        std::lock_guard<std::mutex> lock(m_thumbMutex);
        if (m_isRunning && m_pendingThumbRef && !m_pendingTrackId.empty()) {
            pending = m_pendingThumbRef;
            pendingId = m_pendingTrackId;
        }
        m_pendingThumbRef = nullptr;
        m_pendingTrackId.clear();
    }
    
    if (pending && m_isRunning) {
        {
            std::lock_guard<std::mutex> lock(m_thumbMutex);
            m_lastFetchedTrackId = pendingId;
        }
        TriggerAsyncThumbnailFetch(pending, pendingId);
    }
#endif
}

void MediaService::ThumbnailWorkerFinished() {
    if (m_activeThumbnailWorkers.fetch_sub(1) == 1) {
        std::lock_guard<std::mutex> lock(m_thumbWorkerMutex);
        m_thumbWorkerCv.notify_all();
    }
}

#if defined(DYNAMIC_ISLAND_HAS_WINRT_GSMTC)

void MediaService::TriggerAsyncThumbnailFetch(winrt::Windows::Storage::Streams::IRandomAccessStreamReference thumbRef, const std::wstring& trackId) {
    if (!m_isRunning) return;
    {
        std::lock_guard<std::mutex> lock(m_thumbMutex);
        auto it = m_thumbCache.find(trackId);
        if (it != m_thumbCache.end()) {
            m_thumbPixels = it->second.pixels;
            m_thumbWidth = it->second.w;
            m_thumbHeight = it->second.h;
            m_thumbVersion++;
            m_stateDirty = true;
            return;
        }
        
        m_thumbPixels.clear();
        m_thumbWidth = 0;
        m_thumbHeight = 0;
        m_thumbVersion++;
        m_stateDirty = true;
    }
    
    if (!thumbRef) {
        // No artwork from the media session — try the iTunes Search API.
        std::wstring title, artist;
        {
            std::lock_guard<std::mutex> lock(m_thumbMutex);
            title = m_state.title;
            artist = m_state.artist;
        }
        TriggeriTunesArtworkFetch(trackId, title, artist);
        return;
    }
    
    if (m_isFetchingThumbnail.exchange(true)) {
        std::lock_guard<std::mutex> lock(m_thumbMutex);
        m_pendingThumbRef = thumbRef;
        m_pendingTrackId = trackId;
        return;
    }
    
    auto* ctx = new ThumbnailFetchContext();
    ctx->self = this;
    ctx->thumbRef = thumbRef;
    ctx->trackId = trackId;

    m_activeThumbnailWorkers.fetch_add(1);
    UniqueWinHandle hThread(CreateThread(nullptr, 0, ThumbnailWorkerThreadProc, ctx, 0, nullptr));
    if (hThread) {
        // UniqueWinHandle closes the detached worker handle automatically;
        // active-worker accounting keeps the service alive until completion.
    } else {
        m_isFetchingThumbnail = false;
        delete ctx;
        ThumbnailWorkerFinished();
    }
}

#endif

DWORD WINAPI MediaService::PollThreadProcStatic(LPVOID param) {
    auto* self = static_cast<MediaService*>(param);
    if (self) {
        self->PollThreadProcInternal();
    }
    return 0;
}

#if defined(DYNAMIC_ISLAND_HAS_WINRT_GSMTC)

void MediaService::DetachSessionEvents() noexcept {
    try {
        if (m_currentSession) {
            if (m_playbackInfoChangedToken.value) m_currentSession.PlaybackInfoChanged(m_playbackInfoChangedToken);
            if (m_mediaPropertiesChangedToken.value) m_currentSession.MediaPropertiesChanged(m_mediaPropertiesChangedToken);
            if (m_timelinePropertiesChangedToken.value) m_currentSession.TimelinePropertiesChanged(m_timelinePropertiesChangedToken);
        }
    } catch (...) { LogError(L"Failed to detach GSMTC session event handlers"); }
    m_playbackInfoChangedToken = {};
    m_mediaPropertiesChangedToken = {};
    m_timelinePropertiesChangedToken = {};
}

void MediaService::AttachSessionEvents(const winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession& session) {
    if (!session) return;
    try {
        m_playbackInfoChangedToken = session.PlaybackInfoChanged([this](const auto&, const auto&) noexcept {
            m_sessionDataChanged.store(true, std::memory_order_release);
        });
        m_mediaPropertiesChangedToken = session.MediaPropertiesChanged([this](const auto&, const auto&) noexcept {
            m_sessionDataChanged.store(true, std::memory_order_release);
        });
        m_timelinePropertiesChangedToken = session.TimelinePropertiesChanged([this](const auto&, const auto&) noexcept {
            m_sessionDataChanged.store(true, std::memory_order_release);
        });
    } catch (const winrt::hresult_error& e) {
        LogError(L"Failed to subscribe to GSMTC session events (HRESULT=0x%08X): %s",
                 static_cast<unsigned>(e.code()), e.message().c_str());
    } catch (...) { LogError(L"Failed to subscribe to GSMTC session events"); }
}

void MediaService::PollThreadProcInternal() {
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
    } catch (const winrt::hresult_error& e) {
        LogError(L"MediaService WinRT initialization failed (HRESULT=0x%08X): %s",
                 static_cast<unsigned>(e.code()), e.message().c_str());
        m_isRunning = false;
        return;
    } catch (...) {
        LogError(L"MediaService WinRT initialization failed (unknown error)");
        m_isRunning = false;
        return;
    }

    ULONGLONG lastErrorLogTick = 0;
    while (!m_pollStop && m_isRunning) {
        try {
            if (!m_sessionManager) {
                auto requestOperation = winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager::RequestAsync();
                if (requestOperation.wait_for(std::chrono::seconds(5)) ==
                    winrt::Windows::Foundation::AsyncStatus::Completed) {
                    m_sessionManager = requestOperation.GetResults();
                    m_sessionsChangedToken = m_sessionManager.SessionsChanged([this](const auto&, const auto&) noexcept {
                        m_sessionsChanged.store(true, std::memory_order_release);
                        m_sessionDataChanged.store(true, std::memory_order_release);
                    });
                    LogInfo(L"GSMTC session manager acquired; event notifications enabled");
                } else {
                    requestOperation.Cancel();
                    const ULONGLONG nowTick = GetTickCount64();
                    if (nowTick - lastErrorLogTick >= 5000) {
                        LogError(L"GSMTC RequestAsync did not complete within 5 seconds");
                        lastErrorLogTick = nowTick;
                    }
                }
            }
            
            if (m_sessionManager) {
                winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession session = nullptr;
                
                double now = Graphics::GetHighPrecisionTimeMs();
                bool hasCachedSession = false;
                {
                    std::lock_guard<std::mutex> lock(m_sessionMutex);
                    hasCachedSession = static_cast<bool>(m_currentSession);
                }
                bool refreshSessions = m_sessionsChanged.exchange(false, std::memory_order_acq_rel) ||
                                       (now - m_lastSessionsRefreshMs) >= 15000.0 || !hasCachedSession;
                
                if (refreshSessions) {
                    m_lastSessionsRefreshMs = now;
                    auto sessions = m_sessionManager.GetSessions();
                    uint32_t sessionCount = sessions ? sessions.Size() : 0;
                    
                    for (uint32_t i = 0; i < sessionCount; ++i) {
                        auto s = sessions.GetAt(i);
                        if (s) {
                            auto pb = s.GetPlaybackInfo();
                            if (pb && pb.PlaybackStatus() == winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing) {
                                session = s;
                                break;
                            }
                        }
                    }
                }
                
                if (!session) {
                    // Prefer the session Windows currently considers the
                    // primary media session. Only retain the previous session
                    // if the manager temporarily reports no current session.
                    session = m_sessionManager.GetCurrentSession();
                    if (!session) {
                        std::lock_guard<std::mutex> lock(m_sessionMutex);
                        session = m_currentSession;
                    }
                }

                bool sessionChanged = false;
                {
                    std::lock_guard<std::mutex> lock(m_sessionMutex);
                    sessionChanged = (m_currentSession != session);
                    if (sessionChanged) {
                        DetachSessionEvents();
                        m_currentSession = session;
                        AttachSessionEvents(session);
                        m_sessionDataChanged.store(true, std::memory_order_release);
                    } else {
                        m_currentSession = session;
                    }
                }
                
                if (session) {
                    winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionMediaProperties props{nullptr};
                    bool refreshTrackProperties = m_sessionDataChanged.exchange(false, std::memory_order_acq_rel) || sessionChanged;
                    if (refreshTrackProperties) {
                        auto propertiesOperation = session.TryGetMediaPropertiesAsync();
                        if (propertiesOperation.wait_for(std::chrono::seconds(5)) !=
                            winrt::Windows::Foundation::AsyncStatus::Completed) {
                            propertiesOperation.Cancel();
                            throw winrt::hresult_error(HRESULT_FROM_WIN32(WAIT_TIMEOUT));
                        }
                        props = propertiesOperation.GetResults();
                    }
                    auto info = session.GetPlaybackInfo();
                    auto timeline = session.GetTimelineProperties();
                    
                    std::wstring title, artist;
                    {
                        std::lock_guard<std::mutex> lock(m_thumbMutex);
                        title = m_state.title;
                        artist = m_state.artist;
                    }
                    if (refreshTrackProperties) {
                        title = props ? props.Title().c_str() : L"";
                        artist = props ? props.Artist().c_str() : L"";
                    }
                    bool isPlaying = (info && info.PlaybackStatus() == winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing);
                    
                    float prog = 0.0f;
                    int elapsedSeconds = 0;
                    int durationSeconds = 0;
                    if (timeline) {
                        // GSMTC timeline values are absolute positions in 100-ns ticks.
                        // Normalize against the actual [StartTime, EndTime] range.
                        const int64_t start = timeline.StartTime().count();
                        const int64_t end = timeline.EndTime().count();
                        const int64_t position = timeline.Position().count();
                        const int64_t duration = end - start;

                        if (duration > 0) {
                            const int64_t elapsed = (std::max)(int64_t{0}, (std::min)(position - start, duration));
                            constexpr int64_t TICKS_PER_SECOND = 10000000;
                            elapsedSeconds = static_cast<int>(elapsed / TICKS_PER_SECOND);
                            durationSeconds = static_cast<int>(duration / TICKS_PER_SECOND);
                            prog = static_cast<float>(static_cast<double>(elapsed) /
                                                      static_cast<double>(duration));
                        }
                    }
                    prog = (std::max)(0.0f, (std::min)(prog, 1.0f));
                    
                    if (!title.empty()) {
                        bool mediaStateChanged = false;
                        {
                            std::lock_guard<std::mutex> lock(m_thumbMutex);
                            if (m_hasPendingPlaybackOverride) {
                                const ULONGLONG nowTick = GetTickCount64();
                                if (nowTick <= m_pendingPlaybackOverrideUntilTick) {
                                    if (isPlaying == m_pendingPlaybackOverride) {
                                        // The media session has acknowledged our requested state.
                                        m_hasPendingPlaybackOverride = false;
                                    } else {
                                        // Ignore a stale poll result while the command is in flight.
                                        isPlaying = m_pendingPlaybackOverride;
                                    }
                                } else {
                                    m_hasPendingPlaybackOverride = false;
                                }
                            }
                            mediaStateChanged =
                                !m_hasRealSession ||
                                m_state.title != title ||
                                m_state.artist != artist ||
                                m_state.isPlaying != isPlaying ||
                                m_state.elapsedSeconds != elapsedSeconds ||
                                m_state.durationSeconds != durationSeconds ||
                                std::abs(m_state.progress - prog) >= 0.0005f;
                            m_hasRealSession = true;
                            m_state.title = title;
                            m_state.artist = artist;
                            m_state.isPlaying = isPlaying;
                            // Always assign: zero is a valid position (new track / restart).
                            m_state.progress = prog;
                            m_state.elapsedSeconds = elapsedSeconds;
                            m_state.durationSeconds = durationSeconds;
                        }
                        if (mediaStateChanged) m_stateDirty = true;
                        
                        std::wstring currentTrackId = title + L" - " + artist;
                        bool needsThumbnail = false;
                        {
                            std::lock_guard<std::mutex> lock(m_thumbMutex);
                            if (currentTrackId != m_lastFetchedTrackId) {
                                m_lastFetchedTrackId = currentTrackId;
                                needsThumbnail = true;
                            }
                        }
                        if (needsThumbnail && m_isRunning && refreshTrackProperties && props) {
                            TriggerAsyncThumbnailFetch(props.Thumbnail(), currentTrackId);
                        }
                    } else {
                        std::lock_guard<std::mutex> lock(m_thumbMutex);
                        m_hasPendingPlaybackOverride = false;
                        m_pendingPlaybackOverrideUntilTick = 0;
                        const bool hadMedia = m_hasRealSession || m_state.isPlaying ||
                                              !m_state.title.empty() || !m_state.artist.empty();
                        m_hasRealSession = false;
                        m_state.isPlaying = false;
                        m_state.title.clear();
                        m_state.artist.clear();
                        m_state.progress = 0.0f;
                        m_state.elapsedSeconds = 0;
                        m_state.durationSeconds = 0;
                        if (hadMedia) m_stateDirty = true;
                    }
                } else {
                    bool wasActive;
                    {
                        std::lock_guard<std::mutex> lock(m_thumbMutex);
                        wasActive = m_hasRealSession || m_state.isPlaying || !m_state.title.empty();
                    }
                    
                    if (wasActive) {
                        {
                            std::lock_guard<std::mutex> lock(m_thumbMutex);
                            m_hasRealSession = false;
                            m_state.isPlaying = false;
                            m_state.title.clear();
                            m_state.artist.clear();
                            m_state.progress = 0.0f;
                            m_state.elapsedSeconds = 0;
                            m_state.durationSeconds = 0;
                            m_stateDirty = true;
                            m_lastFetchedTrackId.clear();
                            m_thumbPixels.clear();
                            m_thumbWidth = 0;
                            m_thumbHeight = 0;
                            m_thumbVersion++;
                        }
                    }
                }
            }
        } catch (const winrt::hresult_error& e) {
            const ULONGLONG nowTick = GetTickCount64();
            if (nowTick - lastErrorLogTick >= 5000) {
                LogError(L"GSMTC polling failed (HRESULT=0x%08X): %s",
                         static_cast<unsigned>(e.code()), e.message().c_str());
                lastErrorLogTick = nowTick;
            }
            {
                std::lock_guard<std::mutex> lock(m_thumbMutex);
                m_hasRealSession = false;
                m_state.isPlaying = false;
                m_state.title.clear();
                m_state.artist.clear();
                m_state.progress = 0.0f;
                m_state.elapsedSeconds = 0;
                m_state.durationSeconds = 0;
                m_lastFetchedTrackId.clear();
                m_thumbPixels.clear();
                m_thumbWidth = 0;
                m_thumbHeight = 0;
                ++m_thumbVersion;
                m_stateDirty = true;
            }
            {
                std::lock_guard<std::mutex> lock(m_sessionMutex);
                DetachSessionEvents();
                m_currentSession = nullptr;
            }
            if (m_sessionManager && m_sessionsChangedToken.value) {
                try { m_sessionManager.SessionsChanged(m_sessionsChangedToken); } catch (...) {}
                m_sessionsChangedToken = {};
            }
            m_sessionManager = nullptr;
        } catch (...) {
            const ULONGLONG nowTick = GetTickCount64();
            if (nowTick - lastErrorLogTick >= 5000) {
                LogError(L"GSMTC polling failed (unknown exception)");
                lastErrorLogTick = nowTick;
            }
            {
                std::lock_guard<std::mutex> lock(m_thumbMutex);
                m_hasRealSession = false;
                m_state.isPlaying = false;
                m_state.title.clear();
                m_state.artist.clear();
                m_state.progress = 0.0f;
                m_state.elapsedSeconds = 0;
                m_state.durationSeconds = 0;
                m_lastFetchedTrackId.clear();
                m_thumbPixels.clear();
                m_thumbWidth = 0;
                m_thumbHeight = 0;
                ++m_thumbVersion;
                m_stateDirty = true;
            }
            {
                std::lock_guard<std::mutex> lock(m_sessionMutex);
                DetachSessionEvents();
                m_currentSession = nullptr;
            }
            if (m_sessionManager && m_sessionsChangedToken.value) {
                try { m_sessionManager.SessionsChanged(m_sessionsChangedToken); } catch (...) {}
                m_sessionsChangedToken = {};
            }
            m_sessionManager = nullptr;
        }
        
        Sleep(500);
    }
    
    {
        std::lock_guard<std::mutex> lock(m_sessionMutex);
        DetachSessionEvents();
        m_currentSession = nullptr;
    }
    if (m_sessionManager && m_sessionsChangedToken.value) {
        try { m_sessionManager.SessionsChanged(m_sessionsChangedToken); } catch (...) {}
        m_sessionsChangedToken = {};
    }
    m_sessionManager = nullptr;
    winrt::uninit_apartment();
}

#else

void MediaService::PollThreadProcInternal() {
    LogError(L"Media detection is unavailable: required C++/WinRT GSMTC headers were not found at build time");
    m_isRunning = false;
}

#endif

void MediaService::Poll() {
    if (!m_isRunning) return;
    
    MediaState snapshot;
    bool hasSession = false;
    bool dirty = m_stateDirty.exchange(false);
    
    {
        std::lock_guard<std::mutex> lock(m_thumbMutex);
        hasSession = m_hasRealSession;
        snapshot = m_state;
    }
    
    if (!dirty && !hasSession && snapshot.title.empty()) return;
    
    const bool unchanged = m_hasLastSentState &&
        snapshot.title == m_lastSentState.title &&
        snapshot.artist == m_lastSentState.artist &&
        snapshot.isPlaying == m_lastSentState.isPlaying &&
        snapshot.elapsedSeconds == m_lastSentState.elapsedSeconds &&
        snapshot.durationSeconds == m_lastSentState.durationSeconds &&
        std::abs(snapshot.progress - m_lastSentState.progress) < 0.001f;
    if (unchanged && !dirty) return;

    m_lastSentState = snapshot;
    m_hasLastSentState = true;
    
    if (m_manager) {
        m_manager->UpdateMediaState(snapshot);
    }
}

#if defined(DYNAMIC_ISLAND_HAS_WINRT_GSMTC)
template <typename AsyncOperation>
void ObserveMediaCommand(AsyncOperation operation, const wchar_t* actionName) {
    operation.Completed([actionName](const auto& completedOperation,
                                      winrt::Windows::Foundation::AsyncStatus status) noexcept {
        try {
            if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
                if (!completedOperation.GetResults()) {
                    LogError(L"GSMTC media command '%s' was rejected by the player", actionName);
                }
            } else {
                LogError(L"GSMTC media command '%s' did not complete (status=%u)",
                         actionName, static_cast<unsigned>(status));
            }
        } catch (const winrt::hresult_error& e) {
            LogError(L"GSMTC media command '%s' failed (HRESULT=0x%08X): %s",
                     actionName, static_cast<unsigned>(e.code()), e.message().c_str());
        } catch (...) {
            LogError(L"GSMTC media command '%s' failed with an unknown error", actionName);
        }
    });
}
#endif

void MediaService::Play() {
#if defined(DYNAMIC_ISLAND_HAS_WINRT_GSMTC)
    winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession session{nullptr};
    {
        std::lock_guard<std::mutex> lock(m_sessionMutex);
        session = m_currentSession;
    }
    if (session) {
        try { ObserveMediaCommand(session.TryPlayAsync(), L"Play"); }
        catch (const winrt::hresult_error& e) {
            LogError(L"Unable to issue GSMTC Play command (HRESULT=0x%08X): %s",
                     static_cast<unsigned>(e.code()), e.message().c_str());
        } catch (...) { LogError(L"Unable to issue GSMTC Play command"); }
    }
#endif
}

void MediaService::Pause() {
#if defined(DYNAMIC_ISLAND_HAS_WINRT_GSMTC)
    winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession session{nullptr};
    {
        std::lock_guard<std::mutex> lock(m_sessionMutex);
        session = m_currentSession;
    }
    if (session) {
        try { ObserveMediaCommand(session.TryPauseAsync(), L"Pause"); }
        catch (const winrt::hresult_error& e) {
            LogError(L"Unable to issue GSMTC Pause command (HRESULT=0x%08X): %s",
                     static_cast<unsigned>(e.code()), e.message().c_str());
        } catch (...) { LogError(L"Unable to issue GSMTC Pause command"); }
    }
#endif
}

bool MediaService::TogglePlayPause() {
    MediaState optimisticState;
    bool targetPlaying = false;
    {
        std::lock_guard<std::mutex> lock(m_thumbMutex);
        if (!m_hasRealSession) return false;

        targetPlaying = !m_state.isPlaying;
        m_state.isPlaying = targetPlaying;
        m_hasPendingPlaybackOverride = true;
        m_pendingPlaybackOverride = targetPlaying;
        m_pendingPlaybackOverrideUntilTick = GetTickCount64() + 1500;
        optimisticState = m_state;
        m_stateDirty = true;
    }

    // Update the icon and playback UI immediately; do not wait for the next poll.
    if (m_manager) m_manager->UpdateMediaState(optimisticState);

    // The system command remains asynchronous, but the visual state is already updated.
    if (targetPlaying) Play();
    else Pause();
    return true;
}

void MediaService::Next() {
#if defined(DYNAMIC_ISLAND_HAS_WINRT_GSMTC)
    winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession session{nullptr};
    {
        std::lock_guard<std::mutex> lock(m_sessionMutex);
        session = m_currentSession;
    }
    if (session) {
        try { ObserveMediaCommand(session.TrySkipNextAsync(), L"Skip next"); }
        catch (const winrt::hresult_error& e) {
            LogError(L"Unable to issue GSMTC Skip next command (HRESULT=0x%08X): %s",
                     static_cast<unsigned>(e.code()), e.message().c_str());
        } catch (...) { LogError(L"Unable to issue GSMTC Skip next command"); }
    }
#endif
}

void MediaService::Previous() {
#if defined(DYNAMIC_ISLAND_HAS_WINRT_GSMTC)
    winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession session{nullptr};
    {
        std::lock_guard<std::mutex> lock(m_sessionMutex);
        session = m_currentSession;
    }
    if (session) {
        try { ObserveMediaCommand(session.TrySkipPreviousAsync(), L"Skip previous"); }
        catch (const winrt::hresult_error& e) {
            LogError(L"Unable to issue GSMTC Skip previous command (HRESULT=0x%08X): %s",
                     static_cast<unsigned>(e.code()), e.message().c_str());
        } catch (...) { LogError(L"Unable to issue GSMTC Skip previous command"); }
    }
#endif
}

void MediaService::Seek(float progressFraction) {
    progressFraction = Utils::Clamp(progressFraction, 0.0f, 1.0f);

#if defined(DYNAMIC_ISLAND_HAS_WINRT_GSMTC)
    winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession session{nullptr};
    {
        std::lock_guard<std::mutex> lock(m_sessionMutex);
        session = m_currentSession;
    }
    if (session) {
        try {
            const auto timeline = session.GetTimelineProperties();
            if (timeline) {
                const int64_t start = timeline.StartTime().count();
                const int64_t end = timeline.EndTime().count();
                const int64_t duration = end - start;
                if (duration > 0) {
                    const int64_t targetTicks = start + static_cast<int64_t>(
                        progressFraction * static_cast<double>(duration) + 0.5);
                    ObserveMediaCommand(
                        session.TryChangePlaybackPositionAsync(targetTicks), L"Seek");
                }
            }
        } catch (const winrt::hresult_error& e) {
            LogError(L"Unable to issue GSMTC Seek command (HRESULT=0x%08X): %s",
                     static_cast<unsigned>(e.code()), e.message().c_str());
        } catch (...) { LogError(L"Unable to issue GSMTC Seek command"); }
    }
#endif

    // Keep a responsive drag preview. The next authoritative GSMTC snapshot
    // replaces this value; failed commands are logged by ObserveMediaCommand.
    MediaState snap;
    {
        std::lock_guard<std::mutex> lock(m_thumbMutex);
        m_state.progress = progressFraction;
        if (m_state.durationSeconds > 0) {
            m_state.elapsedSeconds = static_cast<int>(
                progressFraction * static_cast<float>(m_state.durationSeconds) + 0.5f);
        }
        snap = m_state;
        m_stateDirty = true;
    }
    if (m_manager) m_manager->UpdateMediaState(snap);
}
} // namespace Services
} // namespace DynamicIsland

// ============================================================================
// [Module] src\services\audio_service.cpp
// ============================================================================
namespace DynamicIsland {
namespace Services {

AudioService::AudioService(ServiceManager* manager)
    : m_manager(manager) {
    for (auto& band : m_bandSmooth) {
        band.store(0.0f, std::memory_order_relaxed);
    }
}

AudioService::~AudioService() {
    Stop();
}

void AudioService::Start() {
    if (m_isRunning.exchange(true)) return;
    m_lastVolumePollTick = 0;
    m_lastVolumeDeviceCheckTick = 0;
    m_lastMicPollTick = 0;
    m_hasReportedVolume = false;

    m_loopbackWakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!m_loopbackWakeEvent) {
        LogError(L"Failed to create audio loopback wake event (error=%lu)", GetLastError());
    } else {
        m_loopbackThread = CreateThread(nullptr, 0, LoopbackThreadProcStatic, this, 0, nullptr);
        if (!m_loopbackThread) {
            LogError(L"Failed to create audio loopback worker (error=%lu)", GetLastError());
            m_loopbackWakeEvent.reset();
            m_loopbackWakeEvent = nullptr;
        }
    }
    LogInfo(L"AudioService started");
}

void AudioService::ResetLoopback() {
    if (m_audioClient) m_audioClient->Stop();
    m_captureClient.Reset();
    m_audioClient.Reset();
    if (m_audioSamplesReadyEvent) {
        m_audioSamplesReadyEvent.reset();
    }
    m_deviceEnumerator.Reset();
    m_loopbackDeviceId.clear();
    m_lastLoopbackDeviceCheckTick = 0;
    if (m_pwfx) {
        CoTaskMemFree(m_pwfx);
        m_pwfx = nullptr;
    }
    m_isLoopbackActive = false;
    for (auto& filter : m_eqFilters) filter = BiquadBand{};
    m_eqFilterSampleRate = 0.0f;
    for (auto& value : m_bandSmooth) value.store(0.0f, std::memory_order_relaxed);
}

DWORD WINAPI AudioService::LoopbackThreadProcStatic(LPVOID param) {
    auto* self = static_cast<AudioService*>(param);
    if (self) self->LoopbackThreadProc();
    return 0;
}

void AudioService::LoopbackThreadProc() {
    const HRESULT initHr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(initHr)) {
        LogError(L"Audio loopback worker COM initialization failed (HRESULT=0x%08X)",
                 static_cast<unsigned>(initHr));
        return;
    }

    while (m_isRunning.load(std::memory_order_acquire)) {
        try {
            if (m_loopbackRequested.load(std::memory_order_acquire)) {
                UpdateLoopback();
                if (m_audioSamplesReadyEvent && m_loopbackWakeEvent) {
                    HANDLE waits[] = { m_audioSamplesReadyEvent, m_loopbackWakeEvent };
                    WaitForMultipleObjects(_countof(waits), waits, FALSE, 100);
                } else if (m_loopbackWakeEvent) {
                    WaitForSingleObject(m_loopbackWakeEvent, 20);
                } else {
                    Sleep(20);
                }
            } else {
                ResetLoopback();
                if (m_loopbackWakeEvent) WaitForSingleObject(m_loopbackWakeEvent, INFINITE);
                else Sleep(50);
            }
        } catch (const std::exception& e) {
            LogError(L"Audio loopback worker exception: %S", e.what());
            ResetLoopback();
            Sleep(100);
        } catch (...) {
            LogError(L"Audio loopback worker unknown exception");
            ResetLoopback();
            Sleep(100);
        }
    }

    ResetLoopback();
    CoUninitialize();
}

void AudioService::Stop() {
    if (!m_isRunning.exchange(false)) return;
    m_loopbackRequested.store(false, std::memory_order_release);
    if (m_loopbackWakeEvent) SetEvent(m_loopbackWakeEvent);
    if (m_loopbackThread) {
        WaitForSingleObject(m_loopbackThread, INFINITE);
        m_loopbackThread.reset();
    }
    if (m_loopbackWakeEvent) {
        m_loopbackWakeEvent.reset();
    }
    m_endpointVolume.Reset();
    m_volumeDeviceEnumerator.Reset();
    m_volumeDeviceId.clear();
    m_lastVolumeDeviceCheckTick = 0;
    m_hasReportedVolume = false;
    if (m_isMicInUse && m_manager) {
        MicState state;
        state.isActive = false;
        state.title = L"Microphone inactive";
        m_manager->UpdateMicState(state);
    }
    m_isMicInUse = false;
    m_lastMicApp.clear();
    m_lastMicPollTick = 0;
}

void AudioService::PollVolume() {
    if (!m_isRunning || !GetSettingsSnapshot().enableVolumeHUD) return;
    const ULONGLONG now = GetTickCount64();
    if (m_lastVolumePollTick != 0 && now - m_lastVolumePollTick < 400) return;
    m_lastVolumePollTick = now;

    HRESULT hr = S_OK;
    // Re-check the default output device periodically. An endpoint-volume
    // interface can remain valid after Windows changes the default device.
    if (m_lastVolumeDeviceCheckTick == 0 || now - m_lastVolumeDeviceCheckTick >= 5000) {
        ComPtr<IMMDeviceEnumerator> freshEnumerator;
        hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                              IID_PPV_ARGS(&freshEnumerator));
        if (SUCCEEDED(hr) && freshEnumerator) {
            ComPtr<IMMDevice> defaultDevice;
            hr = freshEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &defaultDevice);
            if (SUCCEEDED(hr) && defaultDevice) {
                LPWSTR rawId = nullptr;
                hr = defaultDevice->GetId(&rawId);
                if (SUCCEEDED(hr) && rawId) {
                    const std::wstring newId(rawId);
                    CoTaskMemFree(rawId);
                    if (newId != m_volumeDeviceId) {
                        m_endpointVolume.Reset();
                        m_volumeDeviceId = newId;
                    }
                    m_volumeDeviceEnumerator = freshEnumerator;
                }
            }
        }
        m_lastVolumeDeviceCheckTick = now;
    }

    if (!m_endpointVolume) {
        if (!m_volumeDeviceEnumerator) {
            hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                  IID_PPV_ARGS(&m_volumeDeviceEnumerator));
            if (FAILED(hr) || !m_volumeDeviceEnumerator) return;
        }
        ComPtr<IMMDevice> device;
        hr = m_volumeDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &device);
        if (FAILED(hr) || !device) return;
        hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr,
                              reinterpret_cast<void**>(m_endpointVolume.GetAddressOf()));
        if (FAILED(hr) || !m_endpointVolume) {
            m_volumeDeviceEnumerator.Reset();
            m_volumeDeviceId.clear();
            return;
        }
    }

    float volume = 0.0f;
    BOOL muted = FALSE;
    hr = m_endpointVolume->GetMasterVolumeLevelScalar(&volume);
    if (SUCCEEDED(hr)) hr = m_endpointVolume->GetMute(&muted);
    if (FAILED(hr)) {
        m_endpointVolume.Reset();
        m_volumeDeviceEnumerator.Reset();
        m_volumeDeviceId.clear();
        m_lastVolumeDeviceCheckTick = 0;
        return;
    }

    volume = (std::max)(0.0f, (std::min)(volume, 1.0f));
    m_currentVolume = volume;
    m_isMuted = (muted != FALSE);
    const bool changed = !m_hasReportedVolume ||
        std::abs(volume - m_lastReportedVolume) >= 0.01f ||
        m_isMuted != m_lastReportedMute;
    if (changed && m_hasReportedVolume && m_manager) {
        QueuedEvent event;
        event.type = EventType::Volume;
        event.durationMs = DURATION_VOLUME_MS;
        event.title = m_isMuted ? L"Volume muted" : L"Volume";
        event.subtitle = std::to_wstring(static_cast<int>(std::lround(volume * 100.0f))) + L"%";
        event.progressPercent = static_cast<int>(std::lround(volume * 100.0f));
        m_manager->PostTransientEvent(event);
    }
    m_lastReportedVolume = volume;
    m_lastReportedMute = m_isMuted;
    m_hasReportedVolume = true;
}

void AudioService::SetVolume(float volume) {
    if (!m_isRunning) return;
    volume = (std::max)(0.0f, (std::min)(volume, 1.0f));
    
    // Activate the endpoint on demand (same path PollVolume uses) so scrubbing
    // works even before the first poll or after a device switch reset.
    if (!m_endpointVolume) {
        if (!m_volumeDeviceEnumerator) {
            HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                          IID_PPV_ARGS(&m_volumeDeviceEnumerator));
            if (FAILED(hr) || !m_volumeDeviceEnumerator) return;
        }
        ComPtr<IMMDevice> device;
        HRESULT hr = m_volumeDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &device);
        if (FAILED(hr) || !device) return;
        hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr,
                              reinterpret_cast<void**>(m_endpointVolume.GetAddressOf()));
        if (FAILED(hr) || !m_endpointVolume) {
            m_volumeDeviceEnumerator.Reset();
            m_volumeDeviceId.clear();
            return;
        }
    }
    
    m_endpointVolume->SetMasterVolumeLevelScalar(volume, nullptr);
    
    // Scrubbing the slider also unmutes, matching the Windows volume flyout.
    BOOL muted = FALSE;
    if (SUCCEEDED(m_endpointVolume->GetMute(&muted)) && muted != FALSE) {
        m_endpointVolume->SetMute(FALSE, nullptr);
        m_isMuted = false;
    }
    
    m_currentVolume = volume;
    // m_lastReportedVolume is intentionally NOT updated here: the next
    // PollVolume sees the change and re-posts the Volume transient event,
    // which keeps the pill alive for the whole drag session.
}

void AudioService::UpdateLoopback() {
    if (!m_isRunning.load(std::memory_order_acquire) ||
        !m_loopbackRequested.load(std::memory_order_acquire)) return;
    
    if (!m_isLoopbackActive) {
        HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&m_deviceEnumerator));
        if (FAILED(hr)) { ResetLoopback(); return; }
        
        ComPtr<IMMDevice> defaultDevice;
        hr = m_deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
        if (FAILED(hr)) { ResetLoopback(); return; }

        LPWSTR rawLoopbackId = nullptr;
        hr = defaultDevice->GetId(&rawLoopbackId);
        if (FAILED(hr) || !rawLoopbackId) { ResetLoopback(); return; }
        m_loopbackDeviceId = rawLoopbackId;
        CoTaskMemFree(rawLoopbackId);
        m_lastLoopbackDeviceCheckTick = GetTickCount64();

        m_audioSamplesReadyEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!m_audioSamplesReadyEvent) { ResetLoopback(); return; }

        hr = defaultDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, reinterpret_cast<void**>(m_audioClient.GetAddressOf()));
        if (FAILED(hr)) { ResetLoopback(); return; }

        hr = m_audioClient->GetMixFormat(&m_pwfx);
        if (FAILED(hr) || !m_pwfx) { ResetLoopback(); return; }

        hr = m_audioClient->Initialize(
            AUDCLNT_SHAREMODE_SHARED,
            AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
            1000000, 0, m_pwfx, nullptr);
        if (FAILED(hr)) { ResetLoopback(); return; }

        hr = m_audioClient->SetEventHandle(m_audioSamplesReadyEvent);
        if (FAILED(hr)) { ResetLoopback(); return; }

        hr = m_audioClient->GetService(IID_PPV_ARGS(&m_captureClient));
        if (FAILED(hr)) { ResetLoopback(); return; }

        hr = m_audioClient->Start();
        if (FAILED(hr)) { ResetLoopback(); return; }
        
        m_isLoopbackActive = true;
    }
    
    if (m_isLoopbackActive && m_captureClient) {
        const ULONGLONG now = GetTickCount64();
        if (m_lastLoopbackDeviceCheckTick == 0 ||
            now - m_lastLoopbackDeviceCheckTick >= 2000) {
            m_lastLoopbackDeviceCheckTick = now;
            ComPtr<IMMDevice> currentDefaultDevice;
            HRESULT deviceHr = m_deviceEnumerator->GetDefaultAudioEndpoint(
                eRender, eConsole, &currentDefaultDevice);
            if (FAILED(deviceHr) || !currentDefaultDevice) {
                ResetLoopback();
                return;
            }
            LPWSTR currentIdRaw = nullptr;
            deviceHr = currentDefaultDevice->GetId(&currentIdRaw);
            if (FAILED(deviceHr) || !currentIdRaw) {
                ResetLoopback();
                return;
            }
            const std::wstring currentId(currentIdRaw);
            CoTaskMemFree(currentIdRaw);
            if (currentId != m_loopbackDeviceId) {
                LogInfo(L"Default audio output changed; restarting loopback capture");
                ResetLoopback();
                return;
            }
        }

        UINT32 packetLength = 0;
        float maxBands[6]{};
        
        HRESULT packetHr = S_OK;
        while (true) {
            packetHr = m_captureClient->GetNextPacketSize(&packetLength);
            if (FAILED(packetHr) || packetLength == 0) break;
            BYTE* pData = nullptr;
            UINT32 numFrames = 0;
            DWORD flags = 0;
            
            const HRESULT bufferHr = m_captureClient->GetBuffer(&pData, &numFrames, &flags, nullptr, nullptr);
            if (FAILED(bufferHr)) {
                // Do not retry the same packet indefinitely after a capture failure.
                ResetLoopback();
                return;
            }
            {
                if (!(flags & AUDCLNT_BUFFERFLAGS_SILENT) && pData && numFrames > 0 && m_pwfx) {
                    bool isFloat = (m_pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT);
                    bool isPcm = (m_pwfx->wFormatTag == WAVE_FORMAT_PCM);
                    if (m_pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
                        m_pwfx->cbSize >= sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX)) {
                        const auto* extensible = reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(m_pwfx);
                        static const GUID kFloatSubtype = {0x00000003, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};
                        static const GUID kPcmSubtype = {0x00000001, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};
                        isFloat = IsEqualGUID(extensible->SubFormat, kFloatSubtype);
                        isPcm = IsEqualGUID(extensible->SubFormat, kPcmSubtype);
                    }
                    const UINT channels = (std::max)(1u, static_cast<UINT>(m_pwfx->nChannels));
                    const UINT bits = m_pwfx->wBitsPerSample;
                    const UINT bytesPerSample = (bits + 7u) / 8u;
                    if ((isFloat && bits == 32) || (isPcm && (bits == 8 || bits == 16 || bits == 24 || bits == 32))) {
                        auto readSample = [&](UINT frame, UINT channel) -> float {
                            const size_t offset = static_cast<size_t>(frame) * m_pwfx->nBlockAlign +
                                                  static_cast<size_t>(channel) * bytesPerSample;
                            if (offset + bytesPerSample > static_cast<size_t>(numFrames) * m_pwfx->nBlockAlign) return 0.0f;
                            if (isFloat) {
                                float value = 0.0f;
                                memcpy(&value, pData + offset, sizeof(value));
                                return std::isfinite(value) ? Utils::Clamp(value, -1.0f, 1.0f) : 0.0f;
                            }
                            if (bits == 8) return (static_cast<float>(pData[offset]) - 128.0f) / 128.0f;
                            if (bits == 16) {
                                int16_t value = 0; memcpy(&value, pData + offset, sizeof(value));
                                return static_cast<float>(value) / 32768.0f;
                            }
                            if (bits == 24) {
                                int32_t value = static_cast<int32_t>(pData[offset]) |
                                    (static_cast<int32_t>(pData[offset + 1]) << 8) |
                                    (static_cast<int32_t>(pData[offset + 2]) << 16);
                                if (value & 0x00800000) value |= static_cast<int32_t>(0xff000000);
                                return static_cast<float>(value) / 8388608.0f;
                            }
                            int32_t value = 0; memcpy(&value, pData + offset, sizeof(value));
                            return static_cast<float>(static_cast<double>(value) / 2147483648.0);
                        };

                        // Six logarithmically spaced band-pass filters. Each bar
                        // measures its own range rather than interpolating three broad bands.
                        const float sampleRate = static_cast<float>(m_pwfx->nSamplesPerSec);
                        if (sampleRate > 0.0f && std::abs(sampleRate - m_eqFilterSampleRate) > 1.0f) {
                            constexpr float centers[6] = { 70.0f, 150.0f, 320.0f, 680.0f, 1450.0f, 3100.0f };
                            constexpr float q = 0.90f;
                            for (int band = 0; band < 6; ++band) {
                                BiquadBand& f = m_eqFilters[band];
                                const float w0 = 6.28318530718f * centers[band] / sampleRate;
                                const float cosine = std::cos(w0);
                                const float alpha = std::sin(w0) / (2.0f * q);
                                const float a0 = 1.0f + alpha;
                                f.b0 = alpha / a0;
                                f.b1 = 0.0f;
                                f.b2 = -alpha / a0;
                                f.a1 = (-2.0f * cosine) / a0;
                                f.a2 = (1.0f - alpha) / a0;
                                f.x1 = f.x2 = f.y1 = f.y2 = 0.0f;
                            }
                            m_eqFilterSampleRate = sampleRate;
                        }

                        float sumSquares[6]{};
                        UINT n = 0;
                        for (UINT i = 0; i < numFrames; ++i) {
                            float sample = 0.0f;
                            for (UINT c = 0; c < channels; ++c) sample += readSample(i, c);
                            sample /= static_cast<float>(channels);

                            for (int band = 0; band < 6; ++band) {
                                BiquadBand& f = m_eqFilters[band];
                                const float y = f.b0 * sample + f.b1 * f.x1 + f.b2 * f.x2
                                              - f.a1 * f.y1 - f.a2 * f.y2;
                                f.x2 = f.x1; f.x1 = sample;
                                f.y2 = f.y1; f.y1 = std::isfinite(y) ? y : 0.0f;
                                sumSquares[band] += f.y1 * f.y1;
                            }
                            ++n;
                        }

                        if (n > 0) {
                            // Perceptual expansion: band-pass RMS values are naturally small,
                            // so linear scaling made the compact equalizer barely move.
                            // The square-root curve lifts quiet details while preserving peaks.
                            constexpr float kEqualizerGain = 14.0f;
                            for (int band = 0; band < 6; ++band) {
                                const float rms = std::sqrt(sumSquares[band] / static_cast<float>(n));
                                const float level = std::sqrt(Utils::Clamp(rms * kEqualizerGain, 0.0f, 1.0f));
                                if (level > maxBands[band]) maxBands[band] = level;
                            }
                        }
                    }
                }

                const HRESULT releaseHr = m_captureClient->ReleaseBuffer(numFrames);
                if (FAILED(releaseHr)) {
                    ResetLoopback();
                    return;
                }
            }
        }

        if (FAILED(packetHr)) {
            ResetLoopback();
            return;
        }
        for (int band = 0; band < 6; ++band) {
            const float value = Utils::Clamp(maxBands[band], 0.0f, 1.0f);
            // Audio-thread envelope: fast attack and smooth release per band.
            const float previous = m_bandSmooth[band].load(std::memory_order_relaxed);
            const float smoothed = (value > previous)
                ? previous + 0.70f * (value - previous)
                : previous * 0.82f;
            m_bandSmooth[band].store(smoothed, std::memory_order_relaxed);
        }
    }
}

void AudioService::SetLoopbackCaptureEnabled(bool enabled) {
    const bool previous = m_loopbackRequested.exchange(enabled, std::memory_order_acq_rel);
    if (previous != enabled && m_loopbackWakeEvent) {
        SetEvent(m_loopbackWakeEvent);
    }
}

void AudioService::StopLoopbackCapture() {
    SetLoopbackCaptureEnabled(false);
}

void AudioService::CheckMicUsage() {
    if (!m_isRunning) return;
    if (!GetSettingsSnapshot().enableMicStatus) {
        if (m_isMicInUse && m_manager) {
            MicState state;
            state.isActive = false;
            state.title = L"Microphone inactive";
            m_manager->UpdateMicState(state);
        }
        m_isMicInUse = false;
        m_lastMicApp.clear();
        m_lastMicPollTick = 0;
        return;
    }

    const ULONGLONG micNow = GetTickCount64();
    if (m_lastMicPollTick != 0 && micNow - m_lastMicPollTick < 1000) return;
    m_lastMicPollTick = micNow;
    
    bool currentlyInUse = false;
    std::wstring activeAppName;

    // Enumerate active sessions on the default capture endpoint instead of
    // relying on undocumented-ish registry bookkeeping timestamps.
    ComPtr<IMMDeviceEnumerator> enumerator;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                  IID_PPV_ARGS(&enumerator));
    if (SUCCEEDED(hr) && enumerator) {
        ComPtr<IMMDeviceCollection> captureDevices;
        hr = enumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &captureDevices);
        UINT endpointCount = 0;
        if (SUCCEEDED(hr) && captureDevices && SUCCEEDED(captureDevices->GetCount(&endpointCount))) {
            for (UINT endpointIndex = 0; endpointIndex < endpointCount && !currentlyInUse; ++endpointIndex) {
                ComPtr<IMMDevice> captureDevice;
                if (FAILED(captureDevices->Item(endpointIndex, &captureDevice)) || !captureDevice) continue;

                ComPtr<IAudioSessionManager2> sessionManager;
                hr = captureDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL,
                                             nullptr, reinterpret_cast<void**>(sessionManager.GetAddressOf()));
                if (FAILED(hr) || !sessionManager) continue;

                ComPtr<IAudioSessionEnumerator> sessions;
                hr = sessionManager->GetSessionEnumerator(&sessions);
                if (FAILED(hr) || !sessions) continue;

                int sessionCount = 0;
                if (FAILED(sessions->GetCount(&sessionCount))) continue;
                for (int i = 0; i < sessionCount; ++i) {
                    ComPtr<IAudioSessionControl> control;
                    if (FAILED(sessions->GetSession(i, &control)) || !control) continue;

                    AudioSessionState state = AudioSessionStateInactive;
                    if (FAILED(control->GetState(&state)) || state != AudioSessionStateActive) continue;

                    ComPtr<IAudioSessionControl2> control2;
                    DWORD processId = 0;
                    if (SUCCEEDED(control.As(&control2)) && control2) {
                        control2->GetProcessId(&processId);
                    }

                    LPWSTR displayName = nullptr;
                    if (SUCCEEDED(control->GetDisplayName(&displayName)) && displayName) {
                        activeAppName = displayName;
                        CoTaskMemFree(displayName);
                    }
                    if (activeAppName.empty()) {
                        if (processId) {
                            activeAppName = L"Application (PID " +
                                std::to_wstring(processId) + L")";
                        } else {
                            activeAppName = L"An application";
                        }
                    }
                    currentlyInUse = true;
                    break;
                }
            }
        }
    }

    const bool changed = (currentlyInUse != m_isMicInUse) ||
                         (currentlyInUse && activeAppName != m_lastMicApp);
    m_isMicInUse = currentlyInUse;
    m_lastMicApp = currentlyInUse ? activeAppName : L"";

    if (changed && m_manager) {
        MicState state;
        state.title = currentlyInUse ? L"Microphone in use" : L"Microphone inactive";
        state.appName = activeAppName;
        state.isActive = currentlyInUse;
        m_manager->UpdateMicState(state);
    }
}
} // namespace Services
} // namespace DynamicIsland

// ============================================================================
// [Module] src\services\power_service.cpp
// ============================================================================
namespace DynamicIsland {
namespace Services {

PowerService::PowerService(ServiceManager* manager)
    : m_manager(manager) {}

PowerService::~PowerService() {
    Stop();
}

void PowerService::Start() {
    if (m_isRunning) return;
    m_isRunning = true;
    m_lastPollTick = 0;
    m_hasKnownACStatus = false;
    m_lowBatteryAlerted = false;
    m_wasBatterySaver = false;
    
    SYSTEM_POWER_STATUS sps{};
    if (GetSystemPowerStatus(&sps)) {
        m_wasBatterySaver = (sps.SystemStatusFlag == 1 && sps.ACLineStatus != 1);
        if (sps.BatteryLifePercent <= 100) {
            m_batteryPercent = sps.BatteryLifePercent;
        }
        if (sps.ACLineStatus != 255) {
            m_isACConnected = (sps.ACLineStatus == 1);
            m_wasACConnected = m_isACConnected;
            m_hasKnownACStatus = true;
        }
    }
    
    const std::wstring batteryLog = m_batteryPercent <= 100
        ? std::to_wstring(m_batteryPercent) + L"%"
        : L"unknown";
    LogInfo(L"PowerService started (AC=%s, Battery=%s)",
            m_hasKnownACStatus ? (m_isACConnected ? L"connected" : L"disconnected") : L"unknown",
            batteryLog.c_str());
}

void PowerService::Stop() {
    m_isRunning = false;
}

void PowerService::Poll() {
    if (!m_isRunning || !m_manager) return;
    const ULONGLONG now = GetTickCount64();
    if (m_lastPollTick != 0 && now - m_lastPollTick < 1000) return;
    m_lastPollTick = now;

    SYSTEM_POWER_STATUS sps{};
    if (!GetSystemPowerStatus(&sps)) return;

    const bool hasACStatus = (sps.ACLineStatus != 255);
    const bool currentAC = (sps.ACLineStatus == 1);
    const bool hasBatteryPercent = (sps.BatteryLifePercent <= 100);
    const BYTE currentPercent = sps.BatteryLifePercent;
    const bool currentBatterySaver = (sps.SystemStatusFlag == 1 && !currentAC);
    const bool powerModeChanged = hasACStatus && m_hasKnownACStatus && currentAC != m_wasACConnected;
    const bool saverModeChanged = currentBatterySaver != m_wasBatterySaver;

    if ((powerModeChanged || saverModeChanged) && GetSettingsSnapshot().enablePowerHUD) {
        QueuedEvent qe;
        qe.type = EventType::Power;
        qe.durationMs = DURATION_POWER_MS;
        qe.title = currentAC ? L"Charging" : (currentBatterySaver ? L"Battery saver" : L"On battery");
        qe.progressPercent = -1; // Battery percentage is intentionally not displayed.
        m_manager->PostTransientEvent(qe);
    }

    m_wasBatterySaver = currentBatterySaver;
    if (hasACStatus) {
        m_wasACConnected = currentAC;
        m_isACConnected = currentAC;
        m_hasKnownACStatus = true;
    }

    if (hasBatteryPercent && m_hasKnownACStatus && !m_isACConnected &&
        currentPercent <= 20 && !m_lowBatteryAlerted) {
        m_lowBatteryAlerted = true;
        if (GetSettingsSnapshot().enablePowerHUD) {
            QueuedEvent qe;
            qe.type = EventType::LowBattery;
            qe.durationMs = DURATION_LOW_BATT_MS;
            qe.title = L"Low Battery";
            qe.progressPercent = static_cast<int>(currentPercent);
            m_manager->PostTransientEvent(qe);
        }
    } else if (hasBatteryPercent && currentPercent > 25) {
        m_lowBatteryAlerted = false;
    }

    if (hasBatteryPercent) {
        m_batteryPercent = currentPercent;
    }
}
} // namespace Services
} // namespace DynamicIsland

// ============================================================================
// [Module] src\services\keyboard_service.cpp
// ============================================================================
namespace DynamicIsland {
namespace Services {

KeyboardService::KeyboardService(ServiceManager* manager)
    : m_manager(manager) {}

KeyboardService::~KeyboardService() {
    Stop();
}

void KeyboardService::Start() {
    if (m_isRunning) return;
    m_isRunning = true;
    m_capsLockState = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
    m_numLockState  = (GetKeyState(VK_NUMLOCK)  & 0x0001) != 0;
    LogInfo(L"KeyboardService started (CapsLock=%s, NumLock=%s)",
            m_capsLockState ? L"ON" : L"OFF",
            m_numLockState ? L"ON" : L"OFF");
}

void KeyboardService::Stop() {
    m_isRunning = false;
}

void KeyboardService::Poll() {
    if (!m_isRunning || !m_manager) return;

    const bool hudEnabled = GetSettingsSnapshot().enableCapsLockHUD;

    const bool capsState = (GetKeyState(VK_CAPITAL) & 0x0001) != 0;
    if (capsState != m_capsLockState) {
        m_capsLockState = capsState;
        if (hudEnabled) {
            QueuedEvent qe;
            qe.type = EventType::CapsLock;
            qe.durationMs = DURATION_CAPS_LOCK_MS;
            qe.title = L"Caps Lock";
            qe.subtitle = m_capsLockState ? L"ON" : L"OFF";
            m_manager->PostTransientEvent(qe);
        }
    }

    const bool numState = (GetKeyState(VK_NUMLOCK) & 0x0001) != 0;
    if (numState != m_numLockState) {
        m_numLockState = numState;
        if (hudEnabled) {
            QueuedEvent qe;
            qe.type = EventType::CapsLock;
            qe.durationMs = DURATION_CAPS_LOCK_MS;
            qe.title = L"Num Lock";
            qe.subtitle = m_numLockState ? L"ON" : L"OFF";
            m_manager->PostTransientEvent(qe);
        }
    }
}
} // namespace Services
} // namespace DynamicIsland

// ============================================================================
// [Module] src\services\bluetooth_service.cpp
// ============================================================================
namespace DynamicIsland {
namespace Services {

BluetoothService::BluetoothService(ServiceManager* manager)
    : m_manager(manager) {}

BluetoothService::~BluetoothService() {
    Stop();
}

void BluetoothService::Start() {
    if (m_isRunning) return;
    m_isRunning = true;
    m_lastPollTick = 0;
    m_baselineDone = false;
#if defined(DYNAMIC_ISLAND_HAS_WINRT_DEVICE_WATCHER)
    StartDeviceWatcher();
#endif
    LogInfo(L"BluetoothService started");
}

void BluetoothService::Stop() {
    m_isRunning = false;
#if defined(DYNAMIC_ISLAND_HAS_WINRT_DEVICE_WATCHER)
    StopDeviceWatcher();
#endif
    m_knownConnected.clear();
    m_baselineDone = false;
}

#if defined(DYNAMIC_ISLAND_HAS_WINRT_DEVICE_WATCHER)
void BluetoothService::StartDeviceWatcher() {
    try {
        using namespace winrt::Windows::Devices::Enumeration;
        // A paired-device selector combined with the AEP connection property
        // distinguishes an actually connected peripheral from a merely present
        // Bluetooth PnP node. SetupAPI remains the compatibility fallback.
        const winrt::hstring selector =
            L"System.Devices.Aep.ProtocolId:=\"{e0cbf06c-cd8b-4647-bb8a-263b43f0f974}\"";
        std::vector<winrt::hstring> requestedProperties{ L"System.Devices.Aep.IsConnected" };
        m_deviceWatcher = DeviceInformation::CreateWatcher(
            selector, requestedProperties, DeviceInformationKind::AssociationEndpoint);
        m_watcherConnected.clear();
        m_watcherBaselineDone = false;

        m_watcherAddedToken = m_deviceWatcher.Added([this](const DeviceWatcher&, const DeviceInformation& info) noexcept {
            try { OnWatcherDevice(info, false); }
            catch (const winrt::hresult_error& e) {
                LogError(L"Bluetooth DeviceWatcher Added callback failed (HRESULT=0x%08X)", static_cast<unsigned>(e.code()));
            } catch (...) { LogError(L"Bluetooth DeviceWatcher Added callback failed"); }
        });
        m_watcherUpdatedToken = m_deviceWatcher.Updated([this](const DeviceWatcher&, const DeviceInformationUpdate& update) noexcept {
            try {
                auto info = DeviceInformation::CreateFromIdAsync(update.Id()).get();
                OnWatcherDevice(info, true);
            } catch (const winrt::hresult_error& e) {
                LogError(L"Bluetooth DeviceWatcher Updated callback failed (HRESULT=0x%08X)", static_cast<unsigned>(e.code()));
            } catch (...) { LogError(L"Bluetooth DeviceWatcher Updated callback failed"); }
        });
        m_watcherRemovedToken = m_deviceWatcher.Removed([this](const DeviceWatcher&, const DeviceInformationUpdate& update) noexcept {
            std::lock_guard<std::mutex> lock(m_watcherMutex);
            m_watcherConnected.erase(update.Id().c_str());
        });
        m_watcherEnumerationToken = m_deviceWatcher.EnumerationCompleted([this](const DeviceWatcher&, const winrt::Windows::Foundation::IInspectable&) noexcept {
            std::lock_guard<std::mutex> lock(m_watcherMutex);
            m_watcherBaselineDone = true;
        });
        m_deviceWatcher.Start();
        m_watcherActive = true;
        LogInfo(L"Bluetooth DeviceWatcher started");
    } catch (const winrt::hresult_error& e) {
        m_watcherActive = false;
        m_deviceWatcher = nullptr;
        LogError(L"Bluetooth DeviceWatcher unavailable; using SetupAPI fallback (HRESULT=0x%08X): %s",
                 static_cast<unsigned>(e.code()), e.message().c_str());
    } catch (...) {
        m_watcherActive = false;
        m_deviceWatcher = nullptr;
        LogError(L"Bluetooth DeviceWatcher unavailable; using SetupAPI fallback");
    }
}

void BluetoothService::StopDeviceWatcher() {
    if (!m_deviceWatcher) { m_watcherActive = false; return; }
    try {
        if (m_deviceWatcher.Status() == winrt::Windows::Devices::Enumeration::DeviceWatcherStatus::Started ||
            m_deviceWatcher.Status() == winrt::Windows::Devices::Enumeration::DeviceWatcherStatus::EnumerationCompleted) {
            m_deviceWatcher.Stop();
        }
        if (m_watcherAddedToken.value) m_deviceWatcher.Added(m_watcherAddedToken);
        if (m_watcherUpdatedToken.value) m_deviceWatcher.Updated(m_watcherUpdatedToken);
        if (m_watcherRemovedToken.value) m_deviceWatcher.Removed(m_watcherRemovedToken);
        if (m_watcherEnumerationToken.value) m_deviceWatcher.EnumerationCompleted(m_watcherEnumerationToken);
    } catch (...) { LogError(L"Bluetooth DeviceWatcher shutdown encountered an error"); }
    m_watcherAddedToken = {};
    m_watcherUpdatedToken = {};
    m_watcherRemovedToken = {};
    m_watcherEnumerationToken = {};
    m_deviceWatcher = nullptr;
    m_watcherActive = false;
    std::lock_guard<std::mutex> lock(m_watcherMutex);
    m_watcherConnected.clear();
    m_watcherBaselineDone = false;
}

void BluetoothService::OnWatcherDevice(const winrt::Windows::Devices::Enumeration::DeviceInformation& info, bool) {
    if (!m_isRunning || !m_manager || !info) return;
    bool connected = false;
    try {
        auto properties = info.Properties();
        auto value = properties.Lookup(L"System.Devices.Aep.IsConnected");
        if (value) connected = winrt::unbox_value<bool>(value);
    } catch (...) { return; }

    const std::wstring id = info.Id().c_str();
    const std::wstring name = info.Name().c_str();
    if (id.empty() || name.empty()) return;

    bool shouldNotify = false;
    {
        std::lock_guard<std::mutex> lock(m_watcherMutex);
        const bool wasConnected = m_watcherConnected.find(id) != m_watcherConnected.end();
        if (connected) {
            if (!wasConnected && m_watcherBaselineDone) shouldNotify = true;
            m_watcherConnected[id] = name;
        } else {
            m_watcherConnected.erase(id);
        }
    }

    if (shouldNotify && GetSettingsSnapshot().enableBluetoothHUD) {
        QueuedEvent event;
        event.type = EventType::Bluetooth;
        event.durationMs = DURATION_BLUETOOTH_MS;
        event.title = name;
        event.progressPercent = -1; // AEP watcher does not guarantee battery data.
        m_manager->PostTransientEvent(event);
    }
}
#endif

void BluetoothService::Poll() {
    if (!m_isRunning || !m_manager || !GetSettingsSnapshot().enableBluetoothHUD) return;
#if defined(DYNAMIC_ISLAND_HAS_WINRT_DEVICE_WATCHER)
    if (m_watcherActive) return;
#endif
    const ULONGLONG now = GetTickCount64();
    if (m_lastPollTick != 0 && now - m_lastPollTick < 3000) return;
    m_lastPollTick = now;

    static const GUID GUID_DEVCLASS_BLUETOOTH = {
        0xe0cbf06c, 0xcd8b, 0x4647,
        { 0xbb, 0x8a, 0x26, 0x3b, 0x43, 0xf0, 0xf9, 0x74 }
    };

    HDEVINFO hDevInfo = SetupDiGetClassDevsW(
        &GUID_DEVCLASS_BLUETOOTH, nullptr, nullptr,
        DIGCF_PRESENT
    );

    if (hDevInfo == INVALID_HANDLE_VALUE) return;

    std::unordered_set<std::wstring> nowConnected;
    nowConnected.reserve(8);

    SP_DEVINFO_DATA did{};
    did.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &did); ++i) {
        wchar_t nameBuf[256] = {};
        DWORD propType = 0;

        if (!SetupDiGetDeviceRegistryPropertyW(
            hDevInfo, &did, SPDRP_FRIENDLYNAME, &propType,
            reinterpret_cast<BYTE*>(nameBuf), sizeof(nameBuf), nullptr)) {
            if (!SetupDiGetDeviceRegistryPropertyW(
                hDevInfo, &did, SPDRP_DEVICEDESC, &propType,
                reinterpret_cast<BYTE*>(nameBuf), sizeof(nameBuf), nullptr)) {
                continue;
            }
        }

        std::wstring deviceName(nameBuf);
        if (deviceName.empty()) continue;

        // The Bluetooth class also contains the local radio and generic
        // protocol/service nodes ("...Bluetooth", "...Service", PAN/GATT
        // helpers). Only real peripherals should trigger connection alerts.
        if (deviceName.find(L"Bluetooth") != std::wstring::npos) continue;
        if (deviceName.find(L"Service") != std::wstring::npos) continue;

        // Battery level is optional: many devices never expose
        // DEVPKEY_Device_BatteryLevel. Previously such devices were skipped
        // entirely, so connection alerts never fired — that is the fix.
        DEVPROPTYPE devPropType = DEVPROP_TYPE_EMPTY;
        BYTE batteryByte = 0;
        DWORD requiredSize = 0;
        int batteryLevel = -1;

        if (SetupDiGetDevicePropertyW(
            hDevInfo, &did, &DEVPKEY_Device_BatteryLevel,
            &devPropType, &batteryByte, sizeof(batteryByte),
            &requiredSize, 0)
            && devPropType == DEVPROP_TYPE_BYTE) {
            batteryLevel = static_cast<int>(batteryByte);
        }

        // Device display names are not unique. Track the PnP instance ID so
        // two peripherals with identical friendly names remain independent.
        std::wstring deviceKey;
        DWORD requiredChars = 0;
        SetupDiGetDeviceInstanceIdW(hDevInfo, &did, nullptr, 0, &requiredChars);
        if (requiredChars > 1 && requiredChars < 4096) {
            std::vector<wchar_t> instanceId(requiredChars + 1, L'\0');
            if (SetupDiGetDeviceInstanceIdW(hDevInfo, &did, instanceId.data(),
                                            static_cast<DWORD>(instanceId.size()), nullptr)) {
                deviceKey.assign(instanceId.data());
            }
        }
        if (deviceKey.empty()) deviceKey = deviceName;

        nowConnected.insert(deviceKey);

        if (m_knownConnected.find(deviceKey) == m_knownConnected.end() && m_baselineDone) {
            QueuedEvent qe;
            qe.type = EventType::Bluetooth;
            qe.durationMs = DURATION_BLUETOOTH_MS;
            qe.title = deviceName;
            qe.progressPercent = batteryLevel; // -1 = no battery data reported
            m_manager->PostTransientEvent(qe);
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);

    // Baseline snapshot: record what is already connected at startup so the
    // island does not flood alerts for pre-existing devices on first poll.
    if (!m_baselineDone) {
        m_baselineDone = true;
        m_knownConnected = std::move(nowConnected);
        return;
    }

    m_knownConnected = std::move(nowConnected);
}
} // namespace Services
} // namespace DynamicIsland

// ============================================================================
// [Module] src\services\timer_service.cpp
// ============================================================================
namespace DynamicIsland {
namespace Services {

TimerService::TimerService(ServiceManager* manager)
    : m_manager(manager) {}

TimerService::~TimerService() {
    Stop();
}

void TimerService::Start(int durationSeconds) {
    durationSeconds = (std::max)(0, durationSeconds);
    if (durationSeconds == 0) {
        Stop();
        return;
    }
    m_totalSeconds = durationSeconds;
    m_remainingSeconds = durationSeconds;
    m_isActive = true;
    m_isPaused = false;
    m_deadlineTick = GetTickCount64() + static_cast<ULONGLONG>(durationSeconds) * 1000ULL;
    
    if (m_manager) {
        m_manager->SetLiveActivity(EventType::Timer, true);
    }
    
    SyncState();
    LogInfo(L"Timer started for %d seconds", durationSeconds);
}

void TimerService::Pause() {
    if (!m_isActive || m_isPaused) return;
    const ULONGLONG now = GetTickCount64();
    const ULONGLONG remainingMs = (m_deadlineTick > now) ? (m_deadlineTick - now) : 0;
    m_remainingSeconds = static_cast<int>((remainingMs + 999ULL) / 1000ULL);
    m_isPaused = true;
    SyncState();
}

void TimerService::Resume() {
    if (!m_isActive || !m_isPaused) return;
    m_isPaused = false;
    m_deadlineTick = GetTickCount64() + static_cast<ULONGLONG>(m_remainingSeconds) * 1000ULL;
    SyncState();
}

void TimerService::Stop() {
    m_isActive = false;
    m_isPaused = false;
    m_remainingSeconds = 0;
    m_deadlineTick = 0;
    
    if (m_manager) {
        m_manager->SetLiveActivity(EventType::Timer, false);
    }
    
    SyncState();
}

void TimerService::Tick() {
    if (!m_isActive || m_isPaused) return;

    const ULONGLONG now = GetTickCount64();
    const ULONGLONG remainingMs = (m_deadlineTick > now) ? (m_deadlineTick - now) : 0;
    const int newRemaining = static_cast<int>((remainingMs + 999ULL) / 1000ULL);
    if (newRemaining != m_remainingSeconds) {
        m_remainingSeconds = newRemaining;
        SyncState();
    }

    if (m_remainingSeconds <= 0) {
        Stop();
        if (m_manager) {
            QueuedEvent qe;
            qe.type = EventType::TimerDone;
            qe.durationMs = DURATION_TIMER_DONE_MS;
            qe.title = L"Timer done";
            m_manager->PostTransientEvent(qe);
        }
    }
}


void TimerService::SyncState() {
    if (!m_manager) return;
    
    TimerState ts;
    ts.remainingSeconds = m_remainingSeconds;
    ts.totalSeconds = m_totalSeconds;
    ts.isPaused = m_isPaused;
    ts.label = L"Focus";
    ts.session = L"Session 1 of 4";
    
    m_manager->UpdateTimerState(ts);
}
} // namespace Services
} // namespace DynamicIsland

namespace DynamicIsland {

ModSettings g_settings;
std::mutex g_settingsMutex;

static std::unique_ptr<Overlay::IslandWindow> g_islandWindow;
static std::unique_ptr<Services::ServiceManager> g_serviceManager;
static std::unique_ptr<Services::MediaService> g_mediaService;
static std::unique_ptr<Services::AudioService> g_audioService;
static std::unique_ptr<Services::PowerService> g_powerService;
static std::unique_ptr<Services::KeyboardService> g_keyboardService;
static std::unique_ptr<Services::BluetoothService> g_bluetoothService;
static std::unique_ptr<Services::TimerService> g_timerService;

static UINT_PTR g_pollTimerId = 2001;
static ULONGLONG g_lastTimerTick = 0;

static UniqueWinHandle g_uiThread;
static DWORD g_uiThreadId = 0;
static UniqueWinHandle g_uiReadyEvent;
static std::atomic<bool> g_uiStartupSucceeded{false};

void LoadSettings() {
    std::lock_guard<std::mutex> settingsLock(g_settingsMutex);
    
    PCWSTR placementStr = Wh_GetStringSetting(L"placementMode");
    if (placementStr) {
        if (wcscmp(placementStr, L"top_embed") == 0) {
            g_settings.placement = PlacementMode::TopEmbed;
        } else if (wcscmp(placementStr, L"floating") == 0) {
            g_settings.placement = PlacementMode::Floating;
        } else {
            g_settings.placement = PlacementMode::Auto;
        }
        Wh_FreeStringSetting(placementStr);
    }
    
    PCWSTR idleVisStr = Wh_GetStringSetting(L"idleVisibilityMode");
    if (idleVisStr) {
        if (wcscmp(idleVisStr, L"events_only") == 0) {
            g_settings.idleVisibilityMode = IdleVisibilityMode::EventsOnly;
        } else {
            g_settings.idleVisibilityMode = IdleVisibilityMode::AlwaysVisible;
        }
        Wh_FreeStringSetting(idleVisStr);
    }
    
    PCWSTR mediaVisStr = Wh_GetStringSetting(L"mediaVisibilityPolicy");
    if (mediaVisStr) {
        if (wcscmp(mediaVisStr, L"track_change_only") == 0) {
            g_settings.mediaVisibilityPolicy = MediaVisibilityPolicy::TrackChangeOnly;
        } else {
            g_settings.mediaVisibilityPolicy = MediaVisibilityPolicy::AlwaysVisible;
        }
        Wh_FreeStringSetting(mediaVisStr);
    }
    
    g_settings.enableMedia = Wh_GetIntSetting(L"enableMedia") != 0;
    g_settings.enableOnlineArtworkLookup = Wh_GetIntSetting(L"enableOnlineArtworkLookup") != 0;
    g_settings.enableTimer = Wh_GetIntSetting(L"enableTimer") != 0;
    g_settings.enableMicStatus = Wh_GetIntSetting(L"enableMicStatus") != 0;
    g_settings.enableVolumeHUD = Wh_GetIntSetting(L"enableVolumeHUD") != 0;
    g_settings.enableCapsLockHUD = Wh_GetIntSetting(L"enableCapsLockHUD") != 0;
    g_settings.enablePowerHUD = Wh_GetIntSetting(L"enablePowerHUD") != 0;
    g_settings.enableBluetoothHUD = Wh_GetIntSetting(L"enableBluetoothHUD") != 0;
    g_settings.enableDebugHotkeys = Wh_GetIntSetting(L"enableDebugHotkeys") != 0;
    
    g_settings.autoCollapseDelayMs = Wh_GetIntSetting(L"autoCollapseDelayMs");
    if (g_settings.autoCollapseDelayMs <= 0) {
        int legacySec = Wh_GetIntSetting(L"autoCollapseSeconds");
        if (legacySec > 0) {
            g_settings.autoCollapseDelayMs = legacySec * 1000;
        } else {
            g_settings.autoCollapseDelayMs = 1500;
        }
    }
    
    PCWSTR fontFamStr = Wh_GetStringSetting(L"fontFamily");
    if (fontFamStr) {
        g_settings.fontFamily = fontFamStr;
        Wh_FreeStringSetting(fontFamStr);
    }
    
    if (g_settings.fontFamily.empty()) {
        g_settings.fontFamily = L"Segoe UI Variable Text";
    }
    
    auto parseWeight = [](PCWSTR name, FontWeightPreset def) -> FontWeightPreset {
        PCWSTR s = Wh_GetStringSetting(name);
        FontWeightPreset result = def;
        if (s) {
            if (wcscmp(s, L"thin") == 0) result = FontWeightPreset::Thin;
            else if (wcscmp(s, L"light") == 0) result = FontWeightPreset::Light;
            else if (wcscmp(s, L"regular") == 0) result = FontWeightPreset::Regular;
            else if (wcscmp(s, L"medium") == 0) result = FontWeightPreset::Medium;
            else if (wcscmp(s, L"semibold") == 0) result = FontWeightPreset::SemiBold;
            else if (wcscmp(s, L"bold") == 0) result = FontWeightPreset::Bold;
            Wh_FreeStringSetting(s);
        }
        return result;
    };
    
    g_settings.fontWeightRegular = parseWeight(L"fontWeightRegular", FontWeightPreset::Medium);
    g_settings.fontWeightBold = parseWeight(L"fontWeightBold", FontWeightPreset::Bold);
    
    int fsReg = Wh_GetIntSetting(L"fontSizeRegular");
    if (fsReg >= 8 && fsReg <= 24) g_settings.fontSizeRegular = fsReg;
    
    int fsBig = Wh_GetIntSetting(L"fontSizeBig");
    if (fsBig >= 16 && fsBig <= 72) g_settings.fontSizeBig = fsBig;
    
    int fsSmall = Wh_GetIntSetting(L"fontSizeSmall");
    if (fsSmall >= 8 && fsSmall <= 20) g_settings.fontSizeSmall = fsSmall;
    
    LogInfo(L"Settings loaded successfully");
}

static DWORD WINAPI IslandUIThreadProc(LPVOID) {
#if defined(DYNAMIC_ISLAND_HAS_WINRT_GSMTC)
    try {
        winrt::init_apartment(winrt::apartment_type::single_threaded);
    } catch (const winrt::hresult_error& e) {
        LogError(L"UI thread WinRT initialization failed (HRESULT=0x%08X): %s",
                 static_cast<unsigned>(e.code()), e.message().c_str());
        if (g_uiReadyEvent) SetEvent(g_uiReadyEvent);
        return 0;
    } catch (...) {
        LogError(L"UI thread WinRT initialization failed (unknown error)");
        if (g_uiReadyEvent) SetEvent(g_uiReadyEvent);
        return 0;
    }
#else
    const HRESULT uiComInitHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(uiComInitHr)) {
        LogError(L"UI thread COM initialization failed (HRESULT=0x%08X)", static_cast<unsigned>(uiComInitHr));
        if (g_uiReadyEvent) SetEvent(g_uiReadyEvent);
        return 0;
    }
#endif

#if defined(DYNAMIC_ISLAND_HAS_WINRT_GSMTC)
    struct UiApartmentScope {
        ~UiApartmentScope() { winrt::uninit_apartment(); }
    } uiApartmentScope;
#else
    struct UiApartmentScope {
        ~UiApartmentScope() { CoUninitialize(); }
    } uiApartmentScope;
#endif

    auto cleanupUiThread = []() {
        if (g_islandWindow && g_islandWindow->GetHwnd()) {
            KillTimer(g_islandWindow->GetHwnd(), g_pollTimerId);
        }
        g_timerService.reset();
        g_bluetoothService.reset();
        g_keyboardService.reset();
        g_powerService.reset();
        g_audioService.reset();
        g_mediaService.reset();
        g_serviceManager.reset();
        if (g_islandWindow) {
            g_islandWindow->Destroy();
            g_islandWindow.reset();
        }
    };

    try {
    g_islandWindow = std::make_unique<Overlay::IslandWindow>();
    if (!g_islandWindow->Create()) {
        LogError(L"Failed to create Dynamic Island overlay window");
        g_islandWindow->Destroy();
        g_islandWindow.reset();
        if (g_uiReadyEvent) SetEvent(g_uiReadyEvent);
        return 0;
    }
    
    g_serviceManager = std::make_unique<Services::ServiceManager>(g_islandWindow.get());
    g_serviceManager->Initialize();
    
    g_mediaService = std::make_unique<Services::MediaService>(g_serviceManager.get());
    g_audioService = std::make_unique<Services::AudioService>(g_serviceManager.get());
    g_powerService = std::make_unique<Services::PowerService>(g_serviceManager.get());
    g_keyboardService = std::make_unique<Services::KeyboardService>(g_serviceManager.get());
    g_bluetoothService = std::make_unique<Services::BluetoothService>(g_serviceManager.get());
    g_timerService = std::make_unique<Services::TimerService>(g_serviceManager.get());
    
    g_serviceManager->RegisterMediaService(g_mediaService.get());
    g_serviceManager->RegisterTimerService(g_timerService.get());
    g_serviceManager->RegisterAudioService(g_audioService.get());
    
    if (GetSettingsSnapshot().enableMedia) g_mediaService->Start();
    if (GetSettingsSnapshot().enableMedia || GetSettingsSnapshot().enableMicStatus || GetSettingsSnapshot().enableVolumeHUD) g_audioService->Start();
    if (GetSettingsSnapshot().enablePowerHUD) g_powerService->Start();
    if (GetSettingsSnapshot().enableCapsLockHUD) g_keyboardService->Start();
    if (GetSettingsSnapshot().enableBluetoothHUD) g_bluetoothService->Start();
    
    g_lastTimerTick = GetTickCount64();
    SetTimer(g_islandWindow->GetHwnd(), g_pollTimerId, 200, nullptr);
    
    if (GetSettingsSnapshot().idleVisibilityMode == IdleVisibilityMode::AlwaysVisible ||
        g_serviceManager->ResolveCurrentLiveActivity() != EventType::None) {
        g_islandWindow->Show(true);
    } else {
        g_islandWindow->Show(false);
    }
    
    g_uiStartupSucceeded.store(true);
    if (g_uiReadyEvent) SetEvent(g_uiReadyEvent);
    
    MSG msg{};
    BOOL messageResult = 0;
    while ((messageResult = GetMessageW(&msg, nullptr, 0, 0)) > 0) {
        if (msg.message == WM_TIMER && msg.wParam == g_pollTimerId) {
            if (g_powerService) g_powerService->Poll();
            if (g_keyboardService) g_keyboardService->Poll();
            if (g_bluetoothService) g_bluetoothService->Poll();
            if (g_mediaService) g_mediaService->Poll();

            if (g_audioService) {
                g_audioService->PollVolume();
                g_audioService->CheckMicUsage();
                if (g_mediaService) {
                    const MediaState mediaSnapshot = g_mediaService->SnapshotState();
                    g_audioService->SetLoopbackCaptureEnabled(
                        !mediaSnapshot.title.empty() && mediaSnapshot.isPlaying);

                } else {
                    g_audioService->StopLoopbackCapture();
                }
            }
            
            ULONGLONG now = GetTickCount64();
            if (now - g_lastTimerTick >= 200) {
                g_lastTimerTick = now;
                if (GetSettingsSnapshot().enableTimer && g_timerService) g_timerService->Tick();
            }
            
            if (g_serviceManager) g_serviceManager->Update();
            
            continue;
        }
        
        if (msg.message == WM_USER + 101) {
            const ModSettings settings = GetSettingsSnapshot();

            if (g_mediaService) {
                if (settings.enableMedia) g_mediaService->Start();
                else g_mediaService->Stop();
            }
            if (g_audioService) {
                if (settings.enableMedia || settings.enableMicStatus || settings.enableVolumeHUD) g_audioService->Start();
                else g_audioService->Stop();
            }
            if (g_powerService) {
                if (settings.enablePowerHUD) g_powerService->Start();
                else g_powerService->Stop();
            }
            if (g_keyboardService) {
                if (settings.enableCapsLockHUD) g_keyboardService->Start();
                else g_keyboardService->Stop();
            }
            if (g_bluetoothService) {
                if (settings.enableBluetoothHUD) g_bluetoothService->Start();
                else g_bluetoothService->Stop();
            }
            if (!settings.enableTimer && g_timerService && g_timerService->IsActive()) {
                g_timerService->Stop();
            }

            if (g_islandWindow) {
                if (settings.enableDebugHotkeys) g_islandWindow->RegisterHotkeys();
                else g_islandWindow->UnregisterHotkeys();
                g_islandWindow->ReinitializeRenderer();
                g_islandWindow->UpdateDimensions();
            }
            if (g_serviceManager) g_serviceManager->RefreshVisibility();
            continue;
        }
        
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    
    if (messageResult == -1) {
        LogError(L"GetMessageW failed (error=%lu)", GetLastError());
    }
    cleanupUiThread();
    return 0;
    } catch (const std::exception& e) {
        LogError(L"Unhandled C++ exception in UI thread: %S", e.what());
    } catch (...) {
        LogError(L"Unhandled unknown exception in UI thread");
    }

    g_uiStartupSucceeded.store(false);
    if (g_uiReadyEvent) SetEvent(g_uiReadyEvent);
    try {
        cleanupUiThread();
    } catch (...) {
        LogError(L"Exception while cleaning up UI thread state");
    }
    return 0;
}
} // namespace DynamicIsland

namespace DynamicIsland {

static HMODULE g_hWinmmTimer = nullptr;
static bool g_winmmLoadedByUs = false;
static bool g_timerPeriodActive = false;

static void InitHighPrecisionTimer() {
    g_hWinmmTimer = GetModuleHandleW(L"winmm.dll");
    if (!g_hWinmmTimer) {
        g_hWinmmTimer = LoadLibraryW(L"winmm.dll");
        g_winmmLoadedByUs = (g_hWinmmTimer != nullptr);
    }

    if (g_hWinmmTimer) {
        using PFN_timeBeginPeriod = UINT(WINAPI*)(UINT);
        auto pfn = reinterpret_cast<PFN_timeBeginPeriod>(GetProcAddress(g_hWinmmTimer, "timeBeginPeriod"));
        if (pfn) g_timerPeriodActive = (pfn(1) == 0);
    }
}

static void CleanupHighPrecisionTimer() {
    if (g_hWinmmTimer && g_timerPeriodActive) {
        using PFN_timeEndPeriod = UINT(WINAPI*)(UINT);
        auto pfn = reinterpret_cast<PFN_timeEndPeriod>(GetProcAddress(g_hWinmmTimer, "timeEndPeriod"));
        if (pfn) pfn(1);
    }
    g_timerPeriodActive = false;

    if (g_hWinmmTimer && g_winmmLoadedByUs) FreeLibrary(g_hWinmmTimer);
    g_hWinmmTimer = nullptr;
    g_winmmLoadedByUs = false;
}
} // namespace DynamicIsland

// ============================================================================
// Windhawk Mod Lifecycle Hooks
// ============================================================================

BOOL Wh_ModInit() {
    DynamicIsland::LogInfo(L"Initializing Win11 Dynamic Island Mod...");
    
    DynamicIsland::InitHighPrecisionTimer();
    DynamicIsland::LoadSettings();
    
    DynamicIsland::g_uiReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!DynamicIsland::g_uiReadyEvent) {
        DynamicIsland::LogError(L"Failed to create UI-ready event (error=%lu)", GetLastError());
        DynamicIsland::CleanupHighPrecisionTimer();
        return FALSE;
    }

    DynamicIsland::g_uiStartupSucceeded.store(false);
    DynamicIsland::g_uiThread = CreateThread(
        nullptr,
        0,
        DynamicIsland::IslandUIThreadProc,
        nullptr,
        0,
        &DynamicIsland::g_uiThreadId
    );
    
    if (!DynamicIsland::g_uiThread) {
        DynamicIsland::LogError(L"Failed to create Dynamic Island UI thread");
        if (DynamicIsland::g_uiReadyEvent) {
            DynamicIsland::g_uiReadyEvent.reset();
        }
        DynamicIsland::CleanupHighPrecisionTimer();
        return FALSE;
    }
    
    const DWORD readyWait = WaitForSingleObject(DynamicIsland::g_uiReadyEvent, INFINITE);
    DynamicIsland::g_uiReadyEvent.reset();
    if (readyWait != WAIT_OBJECT_0 || !DynamicIsland::g_uiStartupSucceeded.load()) {
        DynamicIsland::LogError(L"UI thread failed to initialize");
        if (DynamicIsland::g_uiThreadId != 0) {
            PostThreadMessageW(DynamicIsland::g_uiThreadId, WM_QUIT, 0, 0);
        }
        WaitForSingleObject(DynamicIsland::g_uiThread, INFINITE);
        DynamicIsland::g_uiThread.reset();
        DynamicIsland::g_uiThreadId = 0;
        DynamicIsland::CleanupHighPrecisionTimer();
        return FALSE;
    }

    DynamicIsland::LogInfo(L"Win11 Dynamic Island Mod initialized successfully");
    return TRUE;
}

void Wh_ModUninit() {
    DynamicIsland::LogInfo(L"Unloading Win11 Dynamic Island Mod...");
    
    if (DynamicIsland::g_uiThread) {
        if (!PostThreadMessageW(DynamicIsland::g_uiThreadId, WM_QUIT, 0, 0) &&
            DynamicIsland::g_islandWindow && DynamicIsland::g_islandWindow->GetHwnd()) {
            PostMessageW(DynamicIsland::g_islandWindow->GetHwnd(), WM_CLOSE, 0, 0);
        }
        // Do not release module-owned state while its UI thread is still running.
        WaitForSingleObject(DynamicIsland::g_uiThread, INFINITE);
        DynamicIsland::g_uiThread.reset();
    }
    
    DynamicIsland::CleanupHighPrecisionTimer();
    
    DynamicIsland::LogInfo(L"Win11 Dynamic Island Mod unloaded");
}

void Wh_ModSettingsChanged() {
    DynamicIsland::LogInfo(L"Settings updated");
    
    DynamicIsland::LoadSettings();
    
    if (DynamicIsland::g_uiThreadId != 0) {
        PostThreadMessageW(DynamicIsland::g_uiThreadId, WM_USER + 101, 0, 0);
    }
}

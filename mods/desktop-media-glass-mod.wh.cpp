// ==WindhawkMod==
// @id              desktop-media-glass-mod
// @name            Desktop Media Glass mod
// @description     Interactive glass media player for Windows media sessions.
// @version         1.0.0
// @author          satt-glitch
// @github          https://github.com/satt-glitch
// @include         windhawk.exe
// @compilerOptions -lgdiplus -luser32 -lgdi32 -lole32 -luuid -lwindowsapp -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Media Glass mod

Interactive glass media player for Windows media sessions.

![Desktop Media Glass preview](https://imgur.com/a/NZcP1wy)

## Why a separate mod?
While mods such as *SMTC Now Playing Desktop Widgets* or *Glass Widget Bar* provide desktop media widgets or blur panels, **Desktop Media Glass** combines:
- Live multi-session SMTC playback controls and seek bar with automatic session tracking.
- An interactive system master volume slider with click-and-drag scrubbing.
- A locale-aware digital clock.
- One-click session cycling between active media apps.
- Native Windows 10/11 acrylic backdrop blur with 9 selectable color themes and custom tinting.
- Low-overhead event-driven updates for both SMTC and audio endpoint changes without periodic polling.

## Controls & Usage
The widget renders on the primary monitor:
- **Source line (`[1/2]` indicator, top left area)**: Click to cycle between active media sessions.
- **Upper progress bar**: Click to seek to any position in the currently playing track.
- **Transport controls**: Previous track, Play/Pause, and Next track buttons.
- **Lower volume bar**: Click or click-and-drag to adjust the system master volume.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- PositionPreset: topRight
  $name: Position preset
  $options:
    - center: Center
    - topLeft: Top Left
    - topRight: Top Right
    - bottomLeft: Bottom Left
    - bottomRight: Bottom Right
    - custom: Custom
- PositionX: 100
  $name: Custom X Position
- PositionY: 100
  $name: Custom Y Position
- PositionMargin: 24
  $name: Margin
- ScalePercent: 100
  $name: Scale (%)
  $description: Scale widget from 50% to 150%
- GlassOpacity: 190
  $name: Glass Opacity (0-255)
- ThemePreset: glass
  $name: Theme preset
  $options:
    - glass: Glass
    - cyberpunk: Cyberpunk
    - pastelNote: Pastel Note
    - cartoon: Cartoon
    - synthwave: Synthwave
    - ocean: Ocean
    - forest: Forest
    - sunset: Sunset
    - frost: Frost Glass
- UseCustomAccent: false
  $name: Use custom accent color
- AccentColor: '#B8FF5E'
  $name: Accent Color (HEX)
- UseCustomGlassColor: false
  $name: Use custom glass color
- GlassColor: '#2B3740'
  $name: Glass Color (HEX)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <shellscalingapi.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cwctype>
#include <mutex>
#include <string>
#include <vector>

using namespace Gdiplus;
using namespace winrt;
using namespace Windows::Media::Control;

extern "C" IMAGE_DOS_HEADER __ImageBase;

struct Settings {
    std::wstring positionPreset = L"topRight";
    std::wstring themePreset = L"glass";
    int positionX = 100;
    int positionY = 100;
    int positionMargin = 24;
    int scalePercent = 100;
    int glassOpacity = 190;
    bool useCustomAccent = false;
    bool useCustomGlassColor = false;
    std::wstring accentColor = L"#B8FF5E";
    std::wstring glassColor = L"#2B3740";
};

struct ThemePalette {
    BYTE topR, topG, topB;
    BYTE bottomR, bottomG, bottomB;
    BYTE accentR, accentG, accentB;
    bool light;
};

struct TrackInfo {
    std::wstring appId;
    std::wstring title;
    std::wstring artist;
    std::wstring source;
    int64_t startMs = 0;
    int64_t positionMs = 0;
    int64_t durationMs = 0;
    ULONGLONG sampledAtTick = 0;
    bool playing = false;
    bool canPlayPause = false;
    bool canNext = false;
    bool canPrevious = false;
    bool canSeek = false;
};

struct PendingCommand {
    std::wstring targetAppId;
    int action = 0; // 1: Play/Pause, 2: Prev, 3: Next
    int64_t seekMs = -1;
};

static Settings g_settings;
static HINSTANCE g_instance = nullptr;
static std::atomic<HWND> g_window{nullptr};
static HWND g_backdropWindow = nullptr;
static HANDLE g_uiThread = nullptr;
static HANDLE g_mediaThread = nullptr;
static HANDLE g_mediaWake = nullptr;
static HANDLE g_uiReady = nullptr;
static ULONG_PTR g_gdiplusToken = 0;
static FontFamily* g_fontFamily = nullptr;
static std::atomic<bool> g_running{false};
static std::atomic<int> g_selectedIndex{0};

static std::mutex g_cmdMutex;
static std::vector<PendingCommand> g_pendingCommands;

static std::mutex g_tracksMutex;
static std::vector<TrackInfo> g_tracks;

static constexpr UINT WM_APP_TRACKS = WM_APP + 31;
static constexpr UINT WM_APP_SETTINGS_UPDATE = WM_APP + 32;
static constexpr UINT WM_APP_VOLUME_UPDATE = WM_APP + 33;
static constexpr UINT_PTR kRefreshTimer = 1;

// Blur / Acrylic backdrop structures and helper
struct AccentPolicy {
    int accentState;
    int accentFlags;
    DWORD gradientColor;
    DWORD animationId;
};

struct CompositionAttribData {
    int attrib;
    PVOID data;
    SIZE_T size;
};

using SetWindowCompositionAttribute_t = BOOL(WINAPI*)(HWND, CompositionAttribData*);
static SetWindowCompositionAttribute_t g_pfnSetWindowCompositionAttribute = nullptr;
static DWORD g_lastAppliedTint = 0;
static int g_lastPlacedX = -9999;
static int g_lastPlacedY = -9999;
static int g_lastPlacedW = -1;
static int g_lastPlacedH = -1;
static int g_lastPlacedRadius = -1;
static int g_lastWidgetX = -9999;
static int g_lastWidgetY = -9999;
static int g_lastWidgetW = -1;
static int g_lastWidgetH = -1;

static void DisableTransitions(HWND hwnd) {
    if (!g_pfnSetWindowCompositionAttribute) {
        HMODULE hUser = GetModuleHandleW(L"user32.dll");
        if (hUser) {
            g_pfnSetWindowCompositionAttribute =
                (SetWindowCompositionAttribute_t)GetProcAddress(hUser, "SetWindowCompositionAttribute");
        }
    }
    if (!g_pfnSetWindowCompositionAttribute || !hwnd) return;
    BOOL disable = TRUE;
    CompositionAttribData data = {3, &disable, sizeof(disable)}; // WCA_TRANSITIONS_FORCEDISABLED
    g_pfnSetWindowCompositionAttribute(hwnd, &data);
}

static void ApplyWindowBlur(HWND hwnd, DWORD tintColor) {
    if (!g_pfnSetWindowCompositionAttribute) {
        HMODULE hUser = GetModuleHandleW(L"user32.dll");
        if (hUser) {
            g_pfnSetWindowCompositionAttribute =
                (SetWindowCompositionAttribute_t)GetProcAddress(hUser, "SetWindowCompositionAttribute");
        }
    }
    if (!g_pfnSetWindowCompositionAttribute || !hwnd) return;
    if (g_lastAppliedTint == tintColor) return;

    AccentPolicy policy{};
    policy.accentState = 4; // ACCENT_ENABLE_ACRYLICBLURBEHIND (Windows 10/11 native blur)
    policy.accentFlags = 0x10; // Clip accent to window region
    policy.gradientColor = tintColor;

    CompositionAttribData data{};
    data.attrib = 19; // WCA_ACCENT_POLICY
    data.data = &policy;
    data.size = sizeof(policy);

    g_pfnSetWindowCompositionAttribute(hwnd, &data);
    g_lastAppliedTint = tintColor;
}

static void InvalidateWindowCache() {
    g_lastAppliedTint = 0;
    g_lastPlacedX = -9999;
    g_lastPlacedY = -9999;
    g_lastPlacedW = -1;
    g_lastPlacedH = -1;
    g_lastPlacedRadius = -1;
    g_lastWidgetX = -9999;
    g_lastWidgetY = -9999;
    g_lastWidgetW = -1;
    g_lastWidgetH = -1;
}

static void LoadSettings() {
    InvalidateWindowCache();
    PCWSTR preset = Wh_GetStringSetting(L"PositionPreset");
    g_settings.positionPreset = (preset && *preset) ? preset : L"topRight";
    Wh_FreeStringSetting(preset);

    PCWSTR theme = Wh_GetStringSetting(L"ThemePreset");
    g_settings.themePreset = (theme && *theme) ? theme : L"glass";
    Wh_FreeStringSetting(theme);

    g_settings.positionX = Wh_GetIntSetting(L"PositionX");
    g_settings.positionY = Wh_GetIntSetting(L"PositionY");
    g_settings.positionMargin = (std::max)(0, Wh_GetIntSetting(L"PositionMargin"));
    g_settings.scalePercent = (std::clamp)(Wh_GetIntSetting(L"ScalePercent"), 50, 150);
    g_settings.glassOpacity = (std::clamp)(Wh_GetIntSetting(L"GlassOpacity"), 0, 255);
    g_settings.useCustomAccent = Wh_GetIntSetting(L"UseCustomAccent") != 0;
    g_settings.useCustomGlassColor = Wh_GetIntSetting(L"UseCustomGlassColor") != 0;

    PCWSTR color = Wh_GetStringSetting(L"AccentColor");
    g_settings.accentColor = (color && *color) ? color : L"#B8FF5E";
    Wh_FreeStringSetting(color);

    PCWSTR glassColor = Wh_GetStringSetting(L"GlassColor");
    g_settings.glassColor = (glassColor && *glassColor) ? glassColor : L"#2B3740";
    Wh_FreeStringSetting(glassColor);
}

static bool ParseHexRgb(const std::wstring& text, BYTE& red, BYTE& green, BYTE& blue) {
    const wchar_t* digits = text.c_str();
    if (*digits == L'#') digits++;
    if (wcslen(digits) != 6) return false;
    for (int i = 0; i < 6; ++i) {
        if (!iswxdigit(digits[i])) return false;
    }

    unsigned int value = 0;
    if (swscanf_s(digits, L"%06x", &value) != 1) return false;
    red = static_cast<BYTE>(value >> 16);
    green = static_cast<BYTE>(value >> 8);
    blue = static_cast<BYTE>(value);
    return true;
}

static ThemePalette GetThemePalette() {
    const auto& theme = g_settings.themePreset;
    if (_wcsicmp(theme.c_str(), L"cyberpunk") == 0)
        return {34, 13, 43, 9, 21, 37, 255, 54, 180, false};
    if (_wcsicmp(theme.c_str(), L"pastelNote") == 0)
        return {239, 218, 234, 215, 235, 243, 190, 86, 145, true};
    if (_wcsicmp(theme.c_str(), L"cartoon") == 0)
        return {48, 150, 219, 27, 96, 182, 255, 205, 55, false};
    if (_wcsicmp(theme.c_str(), L"synthwave") == 0)
        return {76, 35, 120, 27, 19, 65, 255, 87, 181, false};
    if (_wcsicmp(theme.c_str(), L"ocean") == 0)
        return {21, 103, 125, 12, 48, 77, 91, 231, 220, false};
    if (_wcsicmp(theme.c_str(), L"forest") == 0)
        return {37, 99, 71, 18, 50, 43, 183, 236, 108, false};
    if (_wcsicmp(theme.c_str(), L"sunset") == 0)
        return {137, 64, 51, 61, 31, 57, 255, 190, 88, false};
    if (_wcsicmp(theme.c_str(), L"frost-glass") == 0 || _wcsicmp(theme.c_str(), L"frostGlass") == 0 || _wcsicmp(theme.c_str(), L"frost") == 0)
        return {50, 50, 50, 37, 37, 37, 255, 255, 255, false};
    return {43, 55, 64, 18, 24, 31, 184, 255, 94, false};
}

static ThemePalette GetActivePalette() {
    ThemePalette palette = GetThemePalette();
    if (!g_settings.useCustomGlassColor) return palette;

    BYTE red = 0, green = 0, blue = 0;
    if (ParseHexRgb(g_settings.glassColor, red, green, blue)) {
        palette.topR = red;
        palette.topG = green;
        palette.topB = blue;
        palette.bottomR = static_cast<BYTE>(red * 82 / 100);
        palette.bottomG = static_cast<BYTE>(green * 82 / 100);
        palette.bottomB = static_cast<BYTE>(blue * 82 / 100);
        palette.light = (299 * red + 587 * green + 114 * blue) / 1000 > 155;
    }
    return palette;
}

static Color Accent() {
    const ThemePalette palette = GetThemePalette();
    if (!g_settings.useCustomAccent) {
        return Color(255, palette.accentR, palette.accentG, palette.accentB);
    }

    BYTE red = 0, green = 0, blue = 0;
    if (ParseHexRgb(g_settings.accentColor, red, green, blue)) {
        return Color(255, red, green, blue);
    }
    return Color(255, palette.accentR, palette.accentG, palette.accentB);
}

static Color AccentForeground(Color accent) {
    const int luminance = (299 * accent.GetRed() + 587 * accent.GetGreen() +
                           114 * accent.GetBlue()) / 1000;
    return luminance > 160 ? Color(255, 24, 29, 34) : Color(255, 255, 255, 255);
}

static float GetPrimaryMonitorDpiScale() {
    HMONITOR hMon = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY);
    HMODULE hShcore = GetModuleHandleW(L"shcore.dll");
    if (!hShcore) {
        hShcore = LoadLibraryW(L"shcore.dll");
    }
    if (hShcore) {
        using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);
        auto pfnGetDpiForMonitor = (GetDpiForMonitor_t)GetProcAddress(hShcore, "GetDpiForMonitor");
        if (pfnGetDpiForMonitor) {
            UINT dpiX = 96, dpiY = 96;
            if (SUCCEEDED(pfnGetDpiForMonitor(hMon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) && dpiX > 0) {
                return static_cast<float>(dpiX) / 96.0f;
            }
        }
    }
    HDC hdc = GetDC(nullptr);
    if (hdc) {
        int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
        ReleaseDC(nullptr, hdc);
        if (dpi > 0) return static_cast<float>(dpi) / 96.0f;
    }
    return 1.0f;
}

static float GetEffectiveScale() {
    return GetPrimaryMonitorDpiScale() * (g_settings.scalePercent / 100.0f);
}

static std::wstring SourceName(const std::wstring& appId) {
    const size_t separator = appId.find_last_of(L'!');
    std::wstring name = separator == std::wstring::npos ? appId : appId.substr(separator + 1);
    const size_t dot = name.find_last_of(L'.');
    if (dot != std::wstring::npos && dot > 0) name = name.substr(0, dot);
    return name.empty() ? L"Media session" : name;
}

// Interruptible helper for WinRT async operations
template <typename TAsync>
static bool WaitOp(TAsync& op) {
    while (g_running) {
        auto status = op.wait_for(std::chrono::milliseconds(150));
        if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
            return true;
        }
        if (status != winrt::Windows::Foundation::AsyncStatus::Started) {
            return false;
        }
    }
    try { op.Cancel(); } catch (...) {}
    return false;
}

static DWORD WINAPI MediaThreadProc(void*) {
    bool apartmentInitialized = false;
    try {
        init_apartment(apartment_type::multi_threaded);
        apartmentInitialized = true;

        GlobalSystemMediaTransportControlsSessionManager manager{nullptr};
        while (g_running && !manager) {
            try {
                auto op = GlobalSystemMediaTransportControlsSessionManager::RequestAsync();
                if (WaitOp(op)) {
                    manager = op.GetResults();
                }
            } catch (...) {
                Wh_Log(L"Desktop Media Glass: media session manager unavailable; retrying");
                WaitForSingleObject(g_mediaWake, 1000);
            }
        }

        if (!g_running || !manager) {
            if (apartmentInitialized) uninit_apartment();
            return 0;
        }

        event_token managerSessionsToken{};
        try {
            managerSessionsToken = manager.SessionsChanged([&](auto&&, auto&&) {
                SetEvent(g_mediaWake);
            });
        } catch (...) {}

        struct SessionTokens {
            GlobalSystemMediaTransportControlsSession session{nullptr};
            event_token propToken{};
            event_token playbackToken{};
            event_token timelineToken{};
        };
        std::vector<SessionTokens> activeTokens;

        auto cleanupTokens = [&]() {
            for (auto& item : activeTokens) {
                try {
                    if (item.session) {
                        item.session.MediaPropertiesChanged(item.propToken);
                        item.session.PlaybackInfoChanged(item.playbackToken);
                        item.session.TimelinePropertiesChanged(item.timelineToken);
                    }
                } catch (...) {}
            }
            activeTokens.clear();
        };

        while (g_running) {
            try {
                auto sessions = manager.GetSessions();
                cleanupTokens();

                std::vector<TrackInfo> updated;
                updated.reserve(sessions.Size());

                for (const auto& session : sessions) {
                    try {
                        SessionTokens tokens;
                        tokens.session = session;
                        tokens.propToken = session.MediaPropertiesChanged([&](auto&&, auto&&) { SetEvent(g_mediaWake); });
                        tokens.playbackToken = session.PlaybackInfoChanged([&](auto&&, auto&&) { SetEvent(g_mediaWake); });
                        tokens.timelineToken = session.TimelinePropertiesChanged([&](auto&&, auto&&) { SetEvent(g_mediaWake); });
                        activeTokens.push_back(tokens);

                        TrackInfo track;
                        track.appId = session.SourceAppUserModelId().c_str();
                        track.source = SourceName(track.appId);

                        auto propOp = session.TryGetMediaPropertiesAsync();
                        if (WaitOp(propOp)) {
                            auto properties = propOp.GetResults();
                            if (properties) {
                                track.title = properties.Title().c_str();
                                track.artist = properties.Artist().c_str();
                            }
                        }

                        auto playback = session.GetPlaybackInfo();
                        if (playback) {
                            auto controls = playback.Controls();
                            track.playing = playback.PlaybackStatus() ==
                                GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
                            if (controls) {
                                track.canPlayPause = controls.IsPlayEnabled() || controls.IsPauseEnabled();
                                track.canNext = controls.IsNextEnabled();
                                track.canPrevious = controls.IsPreviousEnabled();
                                track.canSeek = controls.IsPlaybackPositionEnabled();
                            }
                        }

                        auto timeline = session.GetTimelineProperties();
                        if (timeline) {
                            track.startMs = timeline.StartTime().count() / 10000;
                            track.positionMs = (std::max)(int64_t{0}, timeline.Position().count() / 10000 - track.startMs);
                            track.durationMs = (std::max)(int64_t{0}, timeline.EndTime().count() / 10000 - track.startMs);
                            const auto timelineAge = std::chrono::duration_cast<std::chrono::milliseconds>(
                                winrt::clock::now() - timeline.LastUpdatedTime()).count();
                            if (track.playing && timelineAge > 0 && timelineAge < 24LL * 60 * 60 * 1000) {
                                track.positionMs += timelineAge;
                            }
                            track.positionMs = (std::min)(track.positionMs, track.durationMs);
                        }
                        track.sampledAtTick = GetTickCount64();
                        updated.push_back(std::move(track));
                    } catch (...) {
                        Wh_Log(L"Desktop Media Glass: failed to query a media session");
                    }
                }

                // Process pending commands matched by appId
                std::vector<PendingCommand> commandsToRun;
                {
                    std::lock_guard<std::mutex> lock(g_cmdMutex);
                    commandsToRun = std::move(g_pendingCommands);
                }

                for (const auto& cmd : commandsToRun) {
                    for (const auto& session : sessions) {
                        if (cmd.targetAppId.empty() || session.SourceAppUserModelId() == cmd.targetAppId) {
                            try {
                                if (cmd.action == 1) {
                                    auto op = session.TryTogglePlayPauseAsync();
                                    WaitOp(op);
                                } else if (cmd.action == 2) {
                                    auto op = session.TrySkipPreviousAsync();
                                    WaitOp(op);
                                } else if (cmd.action == 3) {
                                    auto op = session.TrySkipNextAsync();
                                    WaitOp(op);
                                }
                                if (cmd.seekMs >= 0) {
                                    auto op = session.TryChangePlaybackPositionAsync(cmd.seekMs * 10000);
                                    WaitOp(op);
                                }
                            } catch (...) {}
                            break;
                        }
                    }
                }

                {
                    std::lock_guard<std::mutex> lock(g_tracksMutex);
                    g_tracks = std::move(updated);
                }

                HWND hwnd = g_window.load();
                if (hwnd) PostMessageW(hwnd, WM_APP_TRACKS, 0, 0);

            } catch (...) {
                Wh_Log(L"Desktop Media Glass: media session refresh failed");
            }

            WaitForSingleObject(g_mediaWake, INFINITE);
        }

        cleanupTokens();
        try {
            if (manager && managerSessionsToken.value != 0) {
                manager.SessionsChanged(managerSessionsToken);
            }
        } catch (...) {}

    } catch (...) {
        Wh_Log(L"Desktop Media Glass: media session thread exception");
    }

    if (apartmentInitialized) uninit_apartment();
    return 0;
}

static std::vector<TrackInfo> GetTracks() {
    std::lock_guard<std::mutex> lock(g_tracksMutex);
    return g_tracks;
}

static void QueueCommand(const std::wstring& appId, int action, int64_t seekMs = -1) {
    std::lock_guard<std::mutex> lock(g_cmdMutex);
    g_pendingCommands.push_back({appId, action, seekMs});
    SetEvent(g_mediaWake);
}

// Audio Endpoint Notification & Callback
class AudioNotificationClient;

static IMMDeviceEnumerator* g_pEnumerator = nullptr;
static IMMDevice* g_pDevice = nullptr;
static IAudioEndpointVolume* g_pVolume = nullptr;
static AudioNotificationClient* g_pAudioClient = nullptr;
static bool g_volumeDragging = false;

class AudioNotificationClient : public IMMNotificationClient, public IAudioEndpointVolumeCallback {
    LONG m_cRef;
public:
    AudioNotificationClient() : m_cRef(1) {}
    virtual ~AudioNotificationClient() {}

    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&m_cRef); }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG ulRef = InterlockedDecrement(&m_cRef);
        if (ulRef == 0) delete this;
        return ulRef;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID** ppvInterface) override {
        if (riid == __uuidof(IUnknown)) {
            *ppvInterface = static_cast<IUnknown*>(static_cast<IMMNotificationClient*>(this));
            AddRef();
            return S_OK;
        }
        if (riid == __uuidof(IMMNotificationClient)) {
            *ppvInterface = static_cast<IMMNotificationClient*>(this);
            AddRef();
            return S_OK;
        }
        if (riid == __uuidof(IAudioEndpointVolumeCallback)) {
            *ppvInterface = static_cast<IAudioEndpointVolumeCallback*>(this);
            AddRef();
            return S_OK;
        }
        *ppvInterface = nullptr;
        return E_NOINTERFACE;
    }

    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR) override {
        if (flow == eRender && role == eMultimedia) {
            HWND hwnd = g_window.load();
            if (hwnd) PostMessageW(hwnd, WM_APP_VOLUME_UPDATE, 1, 0); // 1 = device changed
        }
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override { return S_OK; }

    HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA) override {
        HWND hwnd = g_window.load();
        if (hwnd) PostMessageW(hwnd, WM_APP_VOLUME_UPDATE, 0, 0); // 0 = level changed
        return S_OK;
    }
};

static void UnregisterAudioEndpoint() {
    if (g_pVolume && g_pAudioClient) {
        g_pVolume->UnregisterControlChangeNotify(g_pAudioClient);
    }
    if (g_pVolume) {
        g_pVolume->Release();
        g_pVolume = nullptr;
    }
    if (g_pDevice) {
        g_pDevice->Release();
        g_pDevice = nullptr;
    }
}

static void AcquireAudioEndpoint() {
    UnregisterAudioEndpoint();
    if (!g_pEnumerator) return;
    if (SUCCEEDED(g_pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &g_pDevice))) {
        if (SUCCEEDED(g_pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr,
                                          reinterpret_cast<void**>(&g_pVolume)))) {
            if (g_pAudioClient) {
                g_pVolume->RegisterControlChangeNotify(g_pAudioClient);
            }
        }
    }
}

static void InitAudioEndpoint() {
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER,
                                   __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&g_pEnumerator)))) {
        g_pAudioClient = new AudioNotificationClient();
        g_pEnumerator->RegisterEndpointNotificationCallback(g_pAudioClient);
        AcquireAudioEndpoint();
    }
}

static void TeardownAudioEndpoint() {
    UnregisterAudioEndpoint();
    if (g_pEnumerator && g_pAudioClient) {
        g_pEnumerator->UnregisterEndpointNotificationCallback(g_pAudioClient);
    }
    if (g_pEnumerator) {
        g_pEnumerator->Release();
        g_pEnumerator = nullptr;
    }
    if (g_pAudioClient) {
        g_pAudioClient->Release();
        g_pAudioClient = nullptr;
    }
}

static float GetMasterVolume() {
    float level = 0.5f;
    if (g_pVolume) {
        g_pVolume->GetMasterVolumeLevelScalar(&level);
    }
    return level;
}

static void SetMasterVolume(float level) {
    if (g_pVolume) {
        g_pVolume->SetMasterVolumeLevelScalar((std::clamp)(level, 0.0f, 1.0f), nullptr);
    }
}

static void DrawLabel(Graphics& graphics, const wchar_t* text, REAL x, REAL y,
                      REAL width, REAL height, REAL size, Color color, bool bold = false,
                      bool alignRight = false) {
    Font font(g_fontFamily, size, bold ? FontStyleBold : FontStyleRegular, UnitPixel);
    SolidBrush brush(color);
    StringFormat format;
    format.SetAlignment(alignRight ? StringAlignmentFar : StringAlignmentNear);
    format.SetTrimming(StringTrimmingEllipsisCharacter);
    graphics.DrawString(text, -1, &font, RectF(x, y, width, height), &format, &brush);
}

static void FillRoundRect(Graphics& graphics, REAL x, REAL y, REAL width, REAL height,
                          REAL radius, Color color) {
    GraphicsPath path;
    const REAL diameter = radius * 2;
    path.AddArc(x, y, diameter, diameter, 180, 90);
    path.AddArc(x + width - diameter, y, diameter, diameter, 270, 90);
    path.AddArc(x + width - diameter, y + height - diameter, diameter, diameter, 0, 90);
    path.AddArc(x, y + height - diameter, diameter, diameter, 90, 90);
    path.CloseFigure();
    SolidBrush brush(color);
    graphics.FillPath(&brush, &path);
}

static void DrawTransportIcon(Graphics& graphics, REAL x, REAL y, REAL size,
                              int icon, Color color) {
    SolidBrush brush(color);
    Pen pen(color, size * 0.12f);
    pen.SetStartCap(LineCapRound);
    pen.SetEndCap(LineCapRound);

    if (icon == 0 || icon == 3) {
        const bool previous = icon == 0;
        const REAL barX = previous ? x : x + size * 0.82f;
        graphics.DrawLine(&pen, barX, y + size * 0.12f, barX, y + size * 0.88f);
        PointF points[3];
        if (previous) {
            points[0] = PointF(x + size * 0.72f, y);
            points[1] = PointF(x + size * 0.22f, y + size * 0.5f);
            points[2] = PointF(x + size * 0.72f, y + size);
        } else {
            points[0] = PointF(x + size * 0.28f, y);
            points[1] = PointF(x + size * 0.78f, y + size * 0.5f);
            points[2] = PointF(x + size * 0.28f, y + size);
        }
        GraphicsPath triangle;
        triangle.AddPolygon(points, 3);
        graphics.FillPath(&brush, &triangle);
    } else if (icon == 1) {
        PointF points[3] = {
            PointF(x + size * 0.12f, y),
            PointF(x + size * 0.92f, y + size * 0.5f),
            PointF(x + size * 0.12f, y + size)
        };
        GraphicsPath triangle;
        triangle.AddPolygon(points, 3);
        graphics.FillPath(&brush, &triangle);
    } else {
        FillRoundRect(graphics, x + size * 0.12f, y, size * 0.25f, size, size * 0.08f, color);
        FillRoundRect(graphics, x + size * 0.62f, y, size * 0.25f, size, size * 0.08f, color);
    }
}

static void DrawVolumeIcon(Graphics& graphics, REAL x, REAL y, REAL width,
                           REAL height, Color color) {
    PointF speakerPoints[6] = {
        PointF(x, y + height * 0.34f),
        PointF(x + width * 0.3f, y + height * 0.34f),
        PointF(x + width * 0.65f, y + height * 0.08f),
        PointF(x + width * 0.65f, y + height * 0.92f),
        PointF(x + width * 0.3f, y + height * 0.66f),
        PointF(x, y + height * 0.66f)
    };
    GraphicsPath speaker;
    speaker.AddPolygon(speakerPoints, 6);
    SolidBrush brush(color);
    graphics.FillPath(&brush, &speaker);

    Pen waves(color, 1.6f);
    waves.SetStartCap(LineCapRound);
    waves.SetEndCap(LineCapRound);
    GraphicsPath innerWave;
    innerWave.AddArc(x + width * 0.42f, y + height * 0.25f,
                     width * 0.38f, height * 0.5f, 300, 120);
    graphics.DrawPath(&waves, &innerWave);
    GraphicsPath outerWave;
    outerWave.AddArc(x + width * 0.36f, y + height * 0.08f,
                     width * 0.62f, height * 0.84f, 300, 120);
    graphics.DrawPath(&waves, &outerWave);
}

static void DrawClockIcon(Graphics& graphics, REAL x, REAL y, REAL size, Color color) {
    Pen pen(color, size * 0.1f);
    pen.SetStartCap(LineCapRound);
    pen.SetEndCap(LineCapRound);
    graphics.DrawEllipse(&pen, x, y, size, size);
    graphics.DrawLine(&pen, x + size * 0.5f, y + size * 0.2f,
                      x + size * 0.5f, y + size * 0.52f);
    graphics.DrawLine(&pen, x + size * 0.5f, y + size * 0.52f,
                      x + size * 0.75f, y + size * 0.68f);
}

static void FillGlassPanel(Graphics& graphics, REAL scale, BYTE opacity) {
    const REAL x = 5 * scale, y = 5 * scale, width = 470 * scale, height = 266 * scale;
    const REAL radius = 25 * scale, diameter = radius * 2;
    for (int layer = 8; layer >= 1; --layer) {
        const REAL spread = layer * 0.6f * scale;
        const REAL shadowX = x - 2 * scale - spread;
        const REAL shadowY = y + 4 * scale - spread;
        const REAL shadowWidth = width + spread * 2;
        const REAL shadowHeight = height + spread * 2;
        const BYTE shadowAlpha = static_cast<BYTE>(layer * 2);
        FillRoundRect(graphics, shadowX, shadowY, shadowWidth, shadowHeight,
                      radius + spread, Color(shadowAlpha, 0, 0, 0));
    }

    GraphicsPath path;
    path.AddArc(x, y, diameter, diameter, 180, 90);
    path.AddArc(x + width - diameter, y, diameter, diameter, 270, 90);
    path.AddArc(x + width - diameter, y + height - diameter, diameter, diameter, 0, 90);
    path.AddArc(x, y + height - diameter, diameter, diameter, 90, 90);
    path.CloseFigure();
    const ThemePalette palette = GetActivePalette();
    BYTE topAlpha = (std::min)(static_cast<BYTE>(60), opacity);
    BYTE bottomAlpha = (std::max)(0, static_cast<int>(topAlpha) - 20);
    LinearGradientBrush fill(PointF(x, y), PointF(x, y + height),
        Color(topAlpha, palette.topR, palette.topG, palette.topB),
        Color(bottomAlpha, palette.bottomR, palette.bottomG, palette.bottomB));
    graphics.FillPath(&fill, &path);
}

static void SetWidgetPosition(int width, int height) {
    HWND hwnd = g_window.load();
    if (!hwnd) return;

    RECT bounds{};
    MONITORINFO info{sizeof(info)};
    HMONITOR monitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY);
    if (GetMonitorInfoW(monitor, &info)) {
        bounds = info.rcWork;
    } else {
        bounds.right = GetSystemMetrics(SM_CXSCREEN);
        bounds.bottom = GetSystemMetrics(SM_CYSCREEN);
    }

    int x = bounds.right - width - g_settings.positionMargin;
    int y = bounds.top + g_settings.positionMargin;
    const auto& preset = g_settings.positionPreset;
    if (preset == L"center") {
        x = bounds.left + ((bounds.right - bounds.left) - width) / 2;
        y = bounds.top + ((bounds.bottom - bounds.top) - height) / 2;
    } else if (preset == L"topLeft") {
        x = bounds.left + g_settings.positionMargin;
    } else if (preset == L"bottomLeft") {
        x = bounds.left + g_settings.positionMargin;
        y = bounds.bottom - height - g_settings.positionMargin;
    } else if (preset == L"bottomRight") {
        y = bounds.bottom - height - g_settings.positionMargin;
    } else if (preset == L"custom") {
        x = bounds.left + g_settings.positionX;
        y = bounds.top + g_settings.positionY;
    }

    if (x != g_lastWidgetX || y != g_lastWidgetY ||
        width != g_lastWidgetW || height != g_lastWidgetH) {
        const UINT flags = SWP_NOACTIVATE | SWP_SHOWWINDOW;
        if (SetWindowPos(hwnd, HWND_BOTTOM, x, y, width, height, flags)) {
            g_lastWidgetX = x;
            g_lastWidgetY = y;
            g_lastWidgetW = width;
            g_lastWidgetH = height;
        }
    }

    if (g_backdropWindow) {
        const float scale = GetEffectiveScale();
        const int bx = static_cast<int>(5 * scale);
        const int by = static_cast<int>(5 * scale);
        const int bw = static_cast<int>(470 * scale);
        const int bh = static_cast<int>(266 * scale);
        const int bradius = static_cast<int>(25 * scale);
        const int targetX = x + bx;
        const int targetY = y + by;

        const bool backdropSizeChanged =
            bw != g_lastPlacedW || bh != g_lastPlacedH || bradius != g_lastPlacedRadius;
        bool backdropRegionUpdated = !backdropSizeChanged;
        if (backdropSizeChanged) {
            HRGN rgn = CreateRoundRectRgn(0, 0, bw + 1, bh + 1, bradius * 2, bradius * 2);
            if (rgn) {
                if (SetWindowRgn(g_backdropWindow, rgn, TRUE)) {
                    backdropRegionUpdated = true;
                    g_lastPlacedW = bw;
                    g_lastPlacedH = bh;
                    g_lastPlacedRadius = bradius;
                } else {
                    DeleteObject(rgn);
                }
            }
        }

        const bool backdropMoved = targetX != g_lastPlacedX || targetY != g_lastPlacedY;
        if ((backdropMoved || backdropSizeChanged) && backdropRegionUpdated) {
            SetWindowPos(g_backdropWindow, hwnd, targetX, targetY, bw, bh,
                         SWP_NOACTIVATE | SWP_SHOWWINDOW);
            g_lastPlacedX = targetX;
            g_lastPlacedY = targetY;
        }

        const ThemePalette palette = GetActivePalette();
        DWORD tint = ((DWORD)g_settings.glassOpacity << 24) |
                     ((DWORD)palette.topB << 16) |
                     ((DWORD)palette.topG << 8) |
                     (DWORD)palette.topR;
        ApplyWindowBlur(g_backdropWindow, tint);
    }
}

static void RenderWidget() {
    HWND hwnd = g_window.load();
    if (!hwnd) return;

    const float s = GetEffectiveScale();
    const int width = static_cast<int>(480 * s);
    const int height = static_cast<int>(276 * s);
    if (width <= 0 || height <= 0) return;

    HDC screen = GetDC(nullptr);
    HDC memory = CreateCompatibleDC(screen);
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(screen, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HGDIOBJ oldBitmap = SelectObject(memory, bitmap);

    {
        Graphics graphics(memory);
        graphics.SetSmoothingMode(SmoothingModeAntiAlias);
        graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
        graphics.Clear(Color(0, 0, 0, 0));

        FillGlassPanel(graphics, s, static_cast<BYTE>(g_settings.glassOpacity));

        const ThemePalette palette = GetActivePalette();
        const Color primaryText = palette.light ? Color(245, 24, 29, 34) : Color(245, 246, 248, 250);
        const Color secondaryText = palette.light ? Color(205, 75, 83, 95) : Color(205, 175, 185, 196);
        auto tracks = GetTracks();
        int selected = g_selectedIndex.load();
        if (tracks.empty()) {
            selected = 0;
        } else {
            selected = (std::clamp)(selected, 0, static_cast<int>(tracks.size()) - 1);
        }
        const TrackInfo track = tracks.empty() ? TrackInfo{} : tracks[selected];
        int64_t displayedPositionMs = track.positionMs;
        if (track.playing && track.sampledAtTick > 0) {
            const ULONGLONG elapsed = GetTickCount64() - track.sampledAtTick;
            displayedPositionMs = (std::min)(track.positionMs + static_cast<int64_t>(elapsed), track.durationMs);
        }

        DrawLabel(graphics, L"NOW PLAYING", 30 * s, 25 * s, 180 * s, 18 * s, 10 * s, secondaryText, true);
        DrawClockIcon(graphics, 396 * s, 24 * s, 14 * s, secondaryText);

        wchar_t clockText[32]{};
        if (GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, nullptr, nullptr, clockText, ARRAYSIZE(clockText)) == 0) {
            SYSTEMTIME localTime{};
            GetLocalTime(&localTime);
            swprintf_s(clockText, L"%02u:%02u", localTime.wHour, localTime.wMinute);
        }
        DrawLabel(graphics, clockText, 416 * s, 22 * s, 50 * s, 20 * s, 13 * s,
                  primaryText, true);

        std::wstring source = tracks.empty() ? L"No active media session" : track.source;
        if (tracks.size() > 1) {
            source += L"  [" + std::to_wstring(selected + 1) + L"/" + std::to_wstring(tracks.size()) + L"]";
        }
        DrawLabel(graphics, source.c_str(), 30 * s, 45 * s, 420 * s, 17 * s, 10 * s, Accent(), true);

        std::wstring title = track.title.empty() ? L"Start playback in a supported app" : track.title;
        std::wstring artist = track.artist.empty() ? L"" : track.artist;
        DrawLabel(graphics, title.c_str(), 30 * s, 67 * s, 420 * s, 34 * s, 21 * s,
                  primaryText, true);
        DrawLabel(graphics, artist.c_str(), 30 * s, 101 * s, 420 * s, 24 * s, 14 * s,
                  secondaryText);

        const REAL seekX = 30 * s, seekY = 149 * s, seekW = 420 * s, seekH = 4 * s;
        FillRoundRect(graphics, seekX, seekY, seekW, seekH, seekH / 2, Color(65, 255, 255, 255));
        const REAL progress = track.durationMs > 0 ? static_cast<REAL>(displayedPositionMs) / track.durationMs : 0;
        FillRoundRect(graphics, seekX, seekY, seekW * (std::clamp)(progress, 0.0f, 1.0f), seekH,
                      seekH / 2, Accent());

        const auto formatTime = [](int64_t ms) {
            wchar_t text[24]{};
            swprintf_s(text, L"%lld:%02lld", ms / 60000, (ms / 1000) % 60);
            return std::wstring(text);
        };
        DrawLabel(graphics, formatTime(displayedPositionMs).c_str(), 30 * s, 158 * s, 65 * s, 18 * s,
                  10 * s, Color(175, 221, 229, 234));
        DrawLabel(graphics, formatTime(track.durationMs).c_str(), 385 * s, 158 * s, 65 * s, 18 * s,
                  10 * s, Color(175, 221, 229, 234), false, true);

        const Color accent = Accent();
        const Color iconColor = palette.light ? Color(230, 34, 39, 49) : Color(230, 255, 255, 255);
        FillRoundRect(graphics, 142 * s, 188 * s, 54 * s, 38 * s, 18 * s, Color(34, 255, 255, 255));
        FillRoundRect(graphics, 213 * s, 182 * s, 54 * s, 48 * s, 22 * s, Color(55, accent.GetRed(), accent.GetGreen(), accent.GetBlue()));
        FillRoundRect(graphics, 284 * s, 188 * s, 54 * s, 38 * s, 18 * s, Color(34, 255, 255, 255));
        DrawTransportIcon(graphics, 162 * s, 200 * s, 15 * s, 0, iconColor);
        DrawTransportIcon(graphics, 231 * s, 197 * s, 18 * s, track.playing ? 2 : 1,
                          AccentForeground(accent));
        DrawTransportIcon(graphics, 303 * s, 200 * s, 15 * s, 3, iconColor);

        DrawVolumeIcon(graphics, 32 * s, 238 * s, 20 * s, 18 * s, secondaryText);
        FillRoundRect(graphics, 150 * s, 246 * s, 300 * s, 4 * s, 2 * s, Color(65, 255, 255, 255));
        float volumeLevel = GetMasterVolume();
        FillRoundRect(graphics, 150 * s, 246 * s, 300 * s * volumeLevel, 4 * s, 2 * s, Accent());
    }

    // Directly pre-multiply bits from memory
    auto* outputPixels = static_cast<BYTE*>(bits);
    for (int y = 0; y < height; ++y) {
        auto* outputRow = outputPixels + static_cast<size_t>(y) * width * 4;
        for (int x = 0; x < width; ++x) {
            const BYTE alpha = outputRow[x * 4 + 3];
            outputRow[x * 4] = static_cast<BYTE>(outputRow[x * 4] * alpha / 255);
            outputRow[x * 4 + 1] = static_cast<BYTE>(outputRow[x * 4 + 1] * alpha / 255);
            outputRow[x * 4 + 2] = static_cast<BYTE>(outputRow[x * 4 + 2] * alpha / 255);
        }
    }

    SetWidgetPosition(width, height);
    POINT destination{};
    RECT rect{};
    GetWindowRect(hwnd, &rect);
    destination.x = rect.left;
    destination.y = rect.top;
    SIZE size{width, height};
    POINT source{0, 0};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(hwnd, screen, &destination, &size, memory, &source, 0, &blend, ULW_ALPHA);
    SelectObject(memory, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(memory);
    ReleaseDC(nullptr, screen);
}

static LRESULT CALLBACK WidgetWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_TIMER:
            RenderWidget();
            return 0;
        case WM_APP_TRACKS:
            RenderWidget();
            return 0;
        case WM_APP_SETTINGS_UPDATE:
            LoadSettings();
            RenderWidget();
            return 0;
        case WM_APP_VOLUME_UPDATE:
            if (wParam == 1) { // Device changed
                AcquireAudioEndpoint();
            }
            RenderWidget();
            return 0;
        case WM_LBUTTONDOWN: {
            const float s = GetEffectiveScale();
            const int x = static_cast<int>(GET_X_LPARAM(lParam) / s);
            const int y = static_cast<int>(GET_Y_LPARAM(lParam) / s);
            if (y >= 235 && y <= 260 && x >= 145 && x <= 455) {
                g_volumeDragging = true;
                SetCapture(hwnd);
                SetMasterVolume((std::clamp)((x - 150) / 300.0f, 0.0f, 1.0f));
                RenderWidget();
                return 0;
            }
            break;
        }
        case WM_MOUSEMOVE: {
            if (g_volumeDragging) {
                const float s = GetEffectiveScale();
                const int x = static_cast<int>(GET_X_LPARAM(lParam) / s);
                SetMasterVolume((std::clamp)((x - 150) / 300.0f, 0.0f, 1.0f));
                RenderWidget();
                return 0;
            }
            break;
        }
        case WM_LBUTTONUP: {
            if (g_volumeDragging) {
                g_volumeDragging = false;
                ReleaseCapture();
                return 0;
            }
            const float s = GetEffectiveScale();
            const int x = static_cast<int>(GET_X_LPARAM(lParam) / s);
            const int y = static_cast<int>(GET_Y_LPARAM(lParam) / s);
            auto tracks = GetTracks();

            // Source line / session switcher hit-zone: x: 30..450, y: 42..64
            if (x >= 30 && x <= 450 && y >= 42 && y <= 64 && tracks.size() > 1) {
                g_selectedIndex = (g_selectedIndex.load() + 1) % static_cast<int>(tracks.size());
                RenderWidget();
            } else if (y >= 138 && y <= 156 && !tracks.empty()) {
                const int idx = (std::clamp)(g_selectedIndex.load(), 0, static_cast<int>(tracks.size()) - 1);
                const auto& track = tracks[idx];
                if (track.canSeek && track.durationMs > 0) {
                    const int xPosition = (std::clamp)(x - 30, 0, 420);
                    const int64_t seekMs = track.startMs + track.durationMs * xPosition / 420;
                    QueueCommand(track.appId, 0, seekMs);
                }
            } else if (y >= 180 && y <= 235 && !tracks.empty()) {
                const int idx = (std::clamp)(g_selectedIndex.load(), 0, static_cast<int>(tracks.size()) - 1);
                const auto& track = tracks[idx];
                if (x >= 135 && x <= 202 && track.canPrevious) {
                    QueueCommand(track.appId, 2);
                } else if (x >= 208 && x <= 272 && track.canPlayPause) {
                    QueueCommand(track.appId, 1);
                } else if (x >= 279 && x <= 344 && track.canNext) {
                    QueueCommand(track.appId, 3);
                }
            } else if (y >= 235 && y <= 260 && x >= 145 && x <= 455) {
                SetMasterVolume((std::clamp)((x - 150) / 300.0f, 0.0f, 1.0f));
            }
            RenderWidget();
            return 0;
        }
        case WM_CAPTURECHANGED: {
            g_volumeDragging = false;
            return 0;
        }
        case WM_DPICHANGED:
        case WM_DISPLAYCHANGE: {
            InvalidateWindowCache();
            RenderWidget();
            return 0;
        }
        case WM_SETTINGCHANGE: {
            RenderWidget();
            return 0;
        }
        case WM_DESTROY: {
            KillTimer(hwnd, kRefreshTimer);
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

static LRESULT CALLBACK BackdropWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_ERASEBKGND:
            return 1;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

static DWORD WINAPI UiThreadProc(void*) {
    GdiplusStartupInput startup;
    if (GdiplusStartup(&g_gdiplusToken, &startup, nullptr) != Ok) {
        SetEvent(g_uiReady);
        return 0;
    }
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    FontFamily preferred(L"Segoe UI Variable Text");
    if (preferred.IsAvailable()) {
        g_fontFamily = preferred.Clone();
    } else {
        FontFamily fallback(L"Segoe UI");
        g_fontFamily = fallback.Clone();
    }

    LoadSettings();
    InitAudioEndpoint();

    const wchar_t backdropClassName[] = L"WindhawkDesktopMediaBackdrop";
    WNDCLASSEXW bwc{};
    bwc.cbSize = sizeof(bwc);
    bwc.hInstance = g_instance;
    bwc.lpfnWndProc = BackdropWndProc;
    bwc.lpszClassName = backdropClassName;
    bwc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    ATOM bwcAtom = RegisterClassExW(&bwc);

    g_backdropWindow = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
        backdropClassName, L"", WS_POPUP, 0, 0, 1, 1,
        nullptr, nullptr, g_instance, nullptr);
    if (g_backdropWindow) {
        DisableTransitions(g_backdropWindow);
    }

    const wchar_t className[] = L"WindhawkDesktopMediaGlass";
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.hInstance = g_instance;
    wc.lpfnWndProc = WidgetWndProc;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursor(nullptr, IDC_HAND);
    ATOM wcAtom = RegisterClassExW(&wc);

    HWND createdWnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        className, L"Desktop Media Glass", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, g_instance, nullptr);
    g_window.store(createdWnd);

    // Signal UI thread readiness
    SetEvent(g_uiReady);

    if (!createdWnd) {
        if (g_backdropWindow) {
            DestroyWindow(g_backdropWindow);
            g_backdropWindow = nullptr;
        }
        if (bwcAtom) UnregisterClassW(backdropClassName, g_instance);
        if (wcAtom) UnregisterClassW(className, g_instance);
        delete g_fontFamily;
        g_fontFamily = nullptr;
        TeardownAudioEndpoint();
        CoUninitialize();
        GdiplusShutdown(g_gdiplusToken);
        return 0;
    }

    RenderWidget();
    SetTimer(createdWnd, kRefreshTimer, 500, nullptr);

    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    g_window.store(nullptr);
    if (g_backdropWindow) {
        DestroyWindow(g_backdropWindow);
        g_backdropWindow = nullptr;
    }

    if (wcAtom) UnregisterClassW(className, g_instance);
    if (bwcAtom) UnregisterClassW(backdropClassName, g_instance);

    delete g_fontFamily;
    g_fontFamily = nullptr;

    TeardownAudioEndpoint();
    CoUninitialize();
    GdiplusShutdown(g_gdiplusToken);
    return 0;
}

BOOL WhTool_ModInit() {
    g_instance = reinterpret_cast<HINSTANCE>(&__ImageBase);
    g_running = true;

    // Auto-reset event for wake signals
    g_mediaWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_uiReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_mediaWake || !g_uiReady) {
        if (g_mediaWake) { CloseHandle(g_mediaWake); g_mediaWake = nullptr; }
        if (g_uiReady) { CloseHandle(g_uiReady); g_uiReady = nullptr; }
        return FALSE;
    }

    g_mediaThread = CreateThread(nullptr, 0, MediaThreadProc, nullptr, 0, nullptr);
    if (!g_mediaThread) {
        CloseHandle(g_mediaWake); g_mediaWake = nullptr;
        CloseHandle(g_uiReady); g_uiReady = nullptr;
        return FALSE;
    }

    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, nullptr);
    if (!g_uiThread) {
        g_running = false;
        SetEvent(g_mediaWake);
        WaitForSingleObject(g_mediaThread, INFINITE);
        CloseHandle(g_mediaThread); g_mediaThread = nullptr;
        CloseHandle(g_mediaWake); g_mediaWake = nullptr;
        CloseHandle(g_uiReady); g_uiReady = nullptr;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    HWND hwnd = g_window.load();
    if (hwnd) PostMessageW(hwnd, WM_APP_SETTINGS_UPDATE, 0, 0);
}

void WhTool_ModUninit() {
    g_running = false;
    if (g_mediaWake) SetEvent(g_mediaWake);

    if (g_uiReady && g_uiThread) {
        HANDLE waitHandles[2] = {g_uiReady, g_uiThread};
        WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);
    }

    HWND hwnd = g_window.load();
    if (hwnd) PostMessageW(hwnd, WM_CLOSE, 0, 0);

    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, INFINITE);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
    if (g_mediaThread) {
        WaitForSingleObject(g_mediaThread, INFINITE);
        CloseHandle(g_mediaThread);
        g_mediaThread = nullptr;
    }
    if (g_mediaWake) {
        CloseHandle(g_mediaWake);
        g_mediaWake = nullptr;
    }
    if (g_uiReady) {
        CloseHandle(g_uiReady);
        g_uiReady = nullptr;
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



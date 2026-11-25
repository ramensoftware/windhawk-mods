// ==WindhawkMod==
// @id              desktop-media-glass-mod
// @name            Desktop Media Glass mod
// @description     Interactive glass media player for Windows media sessions.
// @version         1.0.0
// @author          satt-glitch
// @github          https://github.com/satt-glitch
// @include         explorer.exe
// @compilerOptions -lgdiplus -luser32 -lgdi32 -lole32 -luuid -lwindowsapp
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*...*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*...*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <gdiplus.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <string>
#include <vector>
#include <cwchar>

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

static Settings g_settings;
static HINSTANCE g_instance = nullptr;
static HWND g_window = nullptr;
static HWND g_backdropWindow = nullptr;
static HANDLE g_uiThread = nullptr;
static HANDLE g_mediaThread = nullptr;
static HANDLE g_mediaWake = nullptr;
static HANDLE g_mediaReady = nullptr;
static ULONG_PTR g_gdiplusToken = 0;
static std::atomic<bool> g_running{false};
static std::atomic<int> g_selectedIndex{0};
static std::atomic<int> g_pendingAction{0};
static std::atomic<int64_t> g_pendingSeekMs{-1};
static std::mutex g_tracksMutex;
static std::vector<TrackInfo> g_tracks;
static int g_widgetZOrderMode = -1;
static constexpr UINT WM_APP_TRACKS = WM_APP + 31;
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

static void LoadSettings() {
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
    PCWSTR preset = Wh_GetStringSetting(L"PositionPreset");
    g_settings.positionPreset = preset && *preset ? preset : L"topRight";
    if (preset) Wh_FreeStringSetting(preset);
    PCWSTR theme = Wh_GetStringSetting(L"ThemePreset");
    g_settings.themePreset = theme && *theme ? theme : L"glass";
    if (theme) Wh_FreeStringSetting(theme);
    g_settings.positionX = Wh_GetIntSetting(L"PositionX");
    g_settings.positionY = Wh_GetIntSetting(L"PositionY");
    g_settings.positionMargin = (std::max)(0, Wh_GetIntSetting(L"PositionMargin"));
    g_settings.scalePercent = (std::clamp)(Wh_GetIntSetting(L"ScalePercent"), 50, 150);
    g_settings.glassOpacity = (std::clamp)(Wh_GetIntSetting(L"GlassOpacity"), 0, 255);
    g_settings.useCustomAccent = Wh_GetIntSetting(L"UseCustomAccent") != 0;
    g_settings.useCustomGlassColor = Wh_GetIntSetting(L"UseCustomGlassColor") != 0;
    PCWSTR color = Wh_GetStringSetting(L"AccentColor");
    g_settings.accentColor = color && *color ? color : L"#B8FF5E";
    if (color) Wh_FreeStringSetting(color);
    PCWSTR glassColor = Wh_GetStringSetting(L"GlassColor");
    g_settings.glassColor = glassColor && *glassColor ? glassColor : L"#2B3740";
    if (glassColor) Wh_FreeStringSetting(glassColor);
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

static bool ParseHexRgb(const std::wstring& text, BYTE& red, BYTE& green, BYTE& blue);

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

    unsigned int value = (static_cast<unsigned int>(palette.accentR) << 16) |
                         (static_cast<unsigned int>(palette.accentG) << 8) |
                         palette.accentB;
    const wchar_t* text = g_settings.accentColor.c_str();
    if (*text == L'#') text++;
    if (wcslen(text) == 6 && swscanf_s(text, L"%06x", &value) != 1) {
        value = (static_cast<unsigned int>(palette.accentR) << 16) |
                (static_cast<unsigned int>(palette.accentG) << 8) | palette.accentB;
    }
    return Color(255, static_cast<BYTE>(value >> 16),
                 static_cast<BYTE>(value >> 8), static_cast<BYTE>(value));
}

static Color AccentForeground(Color accent) {
    const int luminance = (299 * accent.GetRed() + 587 * accent.GetGreen() +
                           114 * accent.GetBlue()) / 1000;
    return luminance > 160 ? Color(255, 24, 29, 34) : Color(255, 255, 255, 255);
}

static bool ParseHexRgb(const std::wstring& text, BYTE& red, BYTE& green, BYTE& blue) {
    const wchar_t* digits = text.c_str();
    if (*digits == L'#') digits++;
    if (wcslen(digits) != 6) return false;

    unsigned int value = 0;
    if (swscanf_s(digits, L"%06x", &value) != 1) return false;
    red = static_cast<BYTE>(value >> 16);
    green = static_cast<BYTE>(value >> 8);
    blue = static_cast<BYTE>(value);
    return true;
}

static std::wstring SourceName(const std::wstring& appId) {
    const size_t separator = appId.find_last_of(L'!');
    std::wstring name = separator == std::wstring::npos ? appId : appId.substr(separator + 1);
    const size_t dot = name.find_last_of(L'.');
    if (dot != std::wstring::npos && dot > 0) name = name.substr(0, dot);
    return name.empty() ? L"Media session" : name;
}

static DWORD WINAPI MediaThreadProc(void*) {
    bool apartmentInitialized = false;
    try {
        init_apartment(apartment_type::multi_threaded);
        apartmentInitialized = true;
        GlobalSystemMediaTransportControlsSessionManager manager{nullptr};
        bool managerReady = false;
        while (g_running && !managerReady) {
            try {
                manager = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
                managerReady = true;
            } catch (...) {
                SetEvent(g_mediaReady);
                Wh_Log(L"Desktop Media Glass: media session manager unavailable; retrying");
                WaitForSingleObject(g_mediaWake, 1000);
                ResetEvent(g_mediaWake);
            }
        }
        MSG message{};
        PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
        SetEvent(g_mediaReady);

        while (g_running && managerReady) {
            WaitForSingleObject(g_mediaWake, 1000);
            ResetEvent(g_mediaWake);
            if (!g_running) break;

            try {
                auto sessions = manager.GetSessions();
                std::vector<TrackInfo> updated;
                updated.reserve(sessions.Size());
                for (const auto& session : sessions) {
                    try {
                        TrackInfo track;
                        track.source = SourceName(session.SourceAppUserModelId().c_str());
                        auto properties = session.TryGetMediaPropertiesAsync().get();
                        track.title = properties.Title().c_str();
                        track.artist = properties.Artist().c_str();
                        auto playback = session.GetPlaybackInfo();
                        auto controls = playback.Controls();
                        track.playing = playback.PlaybackStatus() ==
                            GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
                        track.canPlayPause = controls.IsPlayEnabled() || controls.IsPauseEnabled();
                        track.canNext = controls.IsNextEnabled();
                        track.canPrevious = controls.IsPreviousEnabled();
                        track.canSeek = controls.IsPlaybackPositionEnabled();
                        auto timeline = session.GetTimelineProperties();
                        track.startMs = timeline.StartTime().count() / 10000;
                        track.positionMs = (std::max)(int64_t{0}, timeline.Position().count() / 10000 - track.startMs);
                        track.durationMs = (std::max)(int64_t{0}, timeline.EndTime().count() / 10000 - track.startMs);
                        const auto timelineAge = std::chrono::duration_cast<std::chrono::milliseconds>(
                            winrt::clock::now() - timeline.LastUpdatedTime()).count();
                        if (track.playing && timelineAge > 0 && timelineAge < 24LL * 60 * 60 * 1000) {
                            track.positionMs += timelineAge;
                        }
                        track.positionMs = (std::min)(track.positionMs, track.durationMs);
                        track.sampledAtTick = GetTickCount64();
                        updated.push_back(std::move(track));
                    } catch (...) {
                        Wh_Log(L"Desktop Media Glass: failed to query a media session");
                        updated.emplace_back();
                    }
                }

                int selected = g_selectedIndex.load();
                if (!updated.empty()) {
                    selected = (std::clamp)(selected, 0, static_cast<int>(updated.size()) - 1);
                    g_selectedIndex = selected;
                    auto session = sessions.GetAt(selected);
                    const TrackInfo& track = updated[selected];
                    const int action = g_pendingAction.exchange(0);
                    if (action == 1 && track.canPlayPause) session.TryTogglePlayPauseAsync().get();
                    if (action == 2 && track.canPrevious) session.TrySkipPreviousAsync().get();
                    if (action == 3 && track.canNext) session.TrySkipNextAsync().get();
                    const int64_t seekMs = g_pendingSeekMs.exchange(-1);
                    if (seekMs >= 0 && track.canSeek) {
                        session.TryChangePlaybackPositionAsync(seekMs * 10000).get();
                    }
                } else {
                    g_pendingAction = 0;
                    g_pendingSeekMs = -1;
                }

                {
                    std::lock_guard<std::mutex> lock(g_tracksMutex);
                    g_tracks = std::move(updated);
                }
                if (g_window) PostMessageW(g_window, WM_APP_TRACKS, 0, 0);
            } catch (...) {
                Wh_Log(L"Desktop Media Glass: media session refresh failed");
            }
        }
    } catch (...) {
        SetEvent(g_mediaReady);
        Wh_Log(L"Desktop Media Glass: media session initialization or query failed");
    }
    if (apartmentInitialized) uninit_apartment();
    return 0;
}

static std::vector<TrackInfo> GetTracks() {
    std::lock_guard<std::mutex> lock(g_tracksMutex);
    return g_tracks;
}

static void DrawText(Graphics& graphics, const wchar_t* text, REAL x, REAL y,
                     REAL width, REAL height, REAL size, Color color, bool bold = false,
                     bool alignRight = false) {
    FontFamily preferred(L"Segoe UI Variable Text");
    FontFamily fallback(L"Segoe UI");
    const FontFamily* family = preferred.IsAvailable() ? &preferred : &fallback;
    Font font(family, size, bold ? FontStyleBold : FontStyleRegular, UnitPixel);
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

static void DrawRoundRect(Graphics& graphics, REAL x, REAL y, REAL width, REAL height,
                          REAL radius, Pen& pen) {
    GraphicsPath path;
    const REAL diameter = radius * 2;
    path.AddArc(x, y, diameter, diameter, 180, 90);
    path.AddArc(x + width - diameter, y, diameter, diameter, 270, 90);
    path.AddArc(x + width - diameter, y + height - diameter, diameter, diameter, 0, 90);
    path.AddArc(x, y + height - diameter, diameter, diameter, 90, 90);
    path.CloseFigure();
    graphics.DrawPath(&pen, &path);
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
    // In backdrop blur mode, we use a gentle translucent gradient tint on top of the blurred background
    BYTE topAlpha = (std::min)(static_cast<BYTE>(60), opacity);
    BYTE bottomAlpha = (std::max)(0, static_cast<int>(topAlpha) - 20);
    LinearGradientBrush fill(PointF(x, y), PointF(x, y + height),
        Color(topAlpha, palette.topR, palette.topG, palette.topB),
        Color(bottomAlpha, palette.bottomR, palette.bottomG, palette.bottomB));
    graphics.FillPath(&fill, &path);
}

static void SetWidgetPosition(int width, int height) {
    RECT bounds{};
    MONITORINFO info{sizeof(info)};
    HMONITOR monitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY);
    if (GetMonitorInfoW(monitor, &info)) bounds = info.rcMonitor;
    else { bounds.right = GetSystemMetrics(SM_CXSCREEN); bounds.bottom = GetSystemMetrics(SM_CYSCREEN); }

    int x = bounds.right - width - g_settings.positionMargin;
    int y = bounds.top + g_settings.positionMargin;
    const auto& preset = g_settings.positionPreset;
    if (preset == L"center") {
        x = bounds.left + ((bounds.right - bounds.left) - width) / 2;
        y = bounds.top + ((bounds.bottom - bounds.top) - height) / 2;
    } else if (preset == L"topLeft") x = bounds.left + g_settings.positionMargin;
    else if (preset == L"bottomLeft") { x = bounds.left + g_settings.positionMargin; y = bounds.bottom - height - g_settings.positionMargin; }
    else if (preset == L"bottomRight") y = bounds.bottom - height - g_settings.positionMargin;
    else if (preset == L"custom") { x = bounds.left + g_settings.positionX; y = bounds.top + g_settings.positionY; }

    if (x != g_lastWidgetX || y != g_lastWidgetY ||
        width != g_lastWidgetW || height != g_lastWidgetH) {
        const UINT flags = SWP_NOACTIVATE | SWP_SHOWWINDOW;
        if (SetWindowPos(g_window, HWND_BOTTOM, x, y, width, height, flags)) {
            g_lastWidgetX = x;
            g_lastWidgetY = y;
            g_lastWidgetW = width;
            g_lastWidgetH = height;
        }
    }

    if (g_backdropWindow) {
        const float scale = g_settings.scalePercent / 100.0f;
        const int bx = static_cast<int>(5 * scale);
        const int by = static_cast<int>(5 * scale);
        const int bw = static_cast<int>(470 * scale);
        const int bh = static_cast<int>(266 * scale);
        const int bradius = static_cast<int>(25 * scale);
        const int targetX = x + bx;
        const int targetY = y + by;

        // Only update region if dimensions or scale changed (prevents flicker)
        const bool backdropSizeChanged =
            bw != g_lastPlacedW || bh != g_lastPlacedH || bradius != g_lastPlacedRadius;
        bool backdropRegionUpdated = !backdropSizeChanged;
        if (backdropSizeChanged) {
            HRGN hrgn = CreateRoundRectRgn(0, 0, bw + 1, bh + 1, bradius * 2, bradius * 2);
            if (hrgn) {
                if (SetWindowRgn(g_backdropWindow, hrgn, TRUE)) {
                    backdropRegionUpdated = true;
                } else {
                    DeleteObject(hrgn);
                }
            }
        }

        // Only update position/z-order if coordinates changed or initial show
        if (targetX != g_lastPlacedX || targetY != g_lastPlacedY ||
            backdropSizeChanged) {
            if (SetWindowPos(g_backdropWindow, g_window, targetX, targetY, bw, bh,
                             SWP_NOACTIVATE | SWP_SHOWWINDOW)) {
                g_lastPlacedX = targetX;
                g_lastPlacedY = targetY;
                if (backdropRegionUpdated) {
                    g_lastPlacedW = bw;
                    g_lastPlacedH = bh;
                    g_lastPlacedRadius = bradius;
                }
            }
        }

        const ThemePalette palette = GetActivePalette();
        BYTE alpha = static_cast<BYTE>((std::clamp)(g_settings.glassOpacity, 10, 250));
        // ABGR tint color for Acrylic Blur (matches menu-styler's translucent blur)
        DWORD tint = (static_cast<DWORD>(alpha) << 24) |
                     (static_cast<DWORD>(palette.topB) << 16) |
                     (static_cast<DWORD>(palette.topG) << 8) |
                     static_cast<DWORD>(palette.topR);
        ApplyWindowBlur(g_backdropWindow, tint);
    }
}

static void RenderWidget() {
    if (!g_window) return;
    const float scale = g_settings.scalePercent / 100.0f;
    const int width = static_cast<int>(480 * scale);
    const int height = static_cast<int>(276 * scale);
    HDC screen = GetDC(nullptr);
    if (!screen) return;
    HDC memory = CreateCompatibleDC(screen);
    if (!memory) {
        ReleaseDC(nullptr, screen);
        return;
    }
    BITMAPINFO bitmapInfo{};
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = -height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(screen, &bitmapInfo, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap || !bits) {
        if (bitmap) DeleteObject(bitmap);
        DeleteDC(memory);
        ReleaseDC(nullptr, screen);
        return;
    }
    HGDIOBJ oldBitmap = SelectObject(memory, bitmap);
    if (!oldBitmap || oldBitmap == HGDI_ERROR) {
        DeleteObject(bitmap);
        DeleteDC(memory);
        ReleaseDC(nullptr, screen);
        return;
    }
    memset(bits, 0, static_cast<size_t>(width) * height * 4);

    Bitmap canvas(width, height, PixelFormat32bppARGB);
    if (canvas.GetLastStatus() != Ok) {
        SelectObject(memory, oldBitmap);
        DeleteObject(bitmap);
        DeleteDC(memory);
        ReleaseDC(nullptr, screen);
        return;
    }
    {
        Graphics graphics(&canvas);
        if (graphics.GetLastStatus() != Ok) {
            SelectObject(memory, oldBitmap);
            DeleteObject(bitmap);
            DeleteDC(memory);
            ReleaseDC(nullptr, screen);
            return;
        }
        graphics.Clear(Color(0, 0, 0, 0));
        graphics.SetSmoothingMode(SmoothingModeAntiAlias);
        graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
        const REAL s = scale;
        FillGlassPanel(graphics, s, static_cast<BYTE>(g_settings.glassOpacity));

        auto tracks = GetTracks();
        int selected = tracks.empty() ? 0 : (std::clamp)(g_selectedIndex.load(), 0, static_cast<int>(tracks.size()) - 1);
        TrackInfo track;
        if (!tracks.empty()) track = tracks[selected];
        const ThemePalette palette = GetActivePalette();
        const Color primaryText = palette.light ? Color(255, 34, 39, 49) : Color(255, 247, 249, 250);
        const Color secondaryText = palette.light ? Color(220, 59, 67, 79) : Color(205, 217, 226, 232);
        const Color mutedText = palette.light ? Color(195, 59, 67, 79) : Color(170, 225, 235, 240);
        int64_t displayedPositionMs = track.positionMs;
        if (track.playing && track.sampledAtTick != 0) {
            displayedPositionMs += static_cast<int64_t>(GetTickCount64() - track.sampledAtTick);
        }
        if (track.durationMs > 0) displayedPositionMs = (std::min)(displayedPositionMs, track.durationMs);
        DrawText(graphics, L"NOW PLAYING", 30 * s, 24 * s, 180 * s, 20 * s, 11 * s,
                 mutedText, true);
        SYSTEMTIME localTime{};
        GetLocalTime(&localTime);
        wchar_t clockText[8]{};
        swprintf_s(clockText, L"%02u:%02u", localTime.wHour, localTime.wMinute);
        DrawClockIcon(graphics, 398 * s, 25 * s, 14 * s, mutedText);
        DrawText(graphics, clockText, 420 * s, 22 * s, 42 * s, 20 * s, 13 * s,
             primaryText, true);
        std::wstring source = tracks.empty() ? L"No active media session" : track.source;
        if (tracks.size() > 1) source += L"  [" + std::to_wstring(selected + 1) + L"/" + std::to_wstring(tracks.size()) + L"]";
        DrawText(graphics, source.c_str(), 30 * s, 45 * s, 420 * s, 17 * s, 10 * s, Accent(), true);
        std::wstring title = track.title.empty() ? L"Start playback in a supported app" : track.title;
        std::wstring artist = track.artist.empty() ? L"" : track.artist;
        DrawText(graphics, title.c_str(), 30 * s, 67 * s, 420 * s, 34 * s, 21 * s,
                 primaryText, true);
        DrawText(graphics, artist.c_str(), 30 * s, 101 * s, 420 * s, 24 * s, 14 * s,
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
        DrawText(graphics, formatTime(displayedPositionMs).c_str(), 30 * s, 158 * s, 65 * s, 18 * s,
                 10 * s, Color(175, 221, 229, 234));
        DrawText(graphics, formatTime(track.durationMs).c_str(), 385 * s, 158 * s, 65 * s, 18 * s,
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
        IMMDeviceEnumerator* enumerator = nullptr;
        IMMDevice* device = nullptr;
        IAudioEndpointVolume* volume = nullptr;
        float volumeLevel = 0.5f;
        if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER,
                                       __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&enumerator)))) {
            if (SUCCEEDED(enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &device)) &&
                SUCCEEDED(device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr,
                                           reinterpret_cast<void**>(&volume)))) volume->GetMasterVolumeLevelScalar(&volumeLevel);
        }
        FillRoundRect(graphics, 150 * s, 246 * s, 300 * s * volumeLevel, 4 * s, 2 * s, Accent());
        if (volume) volume->Release();
        if (device) device->Release();
        if (enumerator) enumerator->Release();
    }

    Rect lockRect(0, 0, width, height);
    BitmapData canvasData{};
    if (canvas.LockBits(&lockRect, ImageLockModeRead, PixelFormat32bppARGB, &canvasData) != Ok) {
        SelectObject(memory, oldBitmap);
        DeleteObject(bitmap);
        DeleteDC(memory);
        ReleaseDC(nullptr, screen);
        return;
    }
    auto* outputPixels = static_cast<BYTE*>(bits);
    for (int y = 0; y < height; ++y) {
        const auto* sourceRow = static_cast<const BYTE*>(canvasData.Scan0) + y * canvasData.Stride;
        auto* outputRow = outputPixels + static_cast<size_t>(y) * width * 4;
        for (int x = 0; x < width; ++x) {
            const BYTE alpha = sourceRow[x * 4 + 3];
            outputRow[x * 4] = static_cast<BYTE>(sourceRow[x * 4] * alpha / 255);
            outputRow[x * 4 + 1] = static_cast<BYTE>(sourceRow[x * 4 + 1] * alpha / 255);
            outputRow[x * 4 + 2] = static_cast<BYTE>(sourceRow[x * 4 + 2] * alpha / 255);
            outputRow[x * 4 + 3] = alpha;
        }
    }
    canvas.UnlockBits(&canvasData);

    SetWidgetPosition(width, height);
    POINT destination{};
    RECT rect{};
    GetWindowRect(g_window, &rect);
    destination.x = rect.left;
    destination.y = rect.top;
    SIZE size{width, height};
    POINT source{0, 0};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_window, screen, &destination, &size, memory, &source, 0, &blend, ULW_ALPHA);
    SelectObject(memory, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(memory);
    ReleaseDC(nullptr, screen);
}

static void SetMasterVolume(float level) {
    IMMDeviceEnumerator* enumerator = nullptr;
    IMMDevice* device = nullptr;
    IAudioEndpointVolume* volume = nullptr;
    if (SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER,
                                   __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&enumerator)))) {
        if (SUCCEEDED(enumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &device)) &&
            SUCCEEDED(device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr,
                                       reinterpret_cast<void**>(&volume)))) volume->SetMasterVolumeLevelScalar(level, nullptr);
    }
    if (volume) volume->Release();
    if (device) device->Release();
    if (enumerator) enumerator->Release();
}

static LRESULT CALLBACK WidgetWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_TIMER:
            RenderWidget();
            return 0;
        case WM_APP_TRACKS:
            RenderWidget();
            return 0;
        case WM_APP + 32:
            LoadSettings();
            RenderWidget();
            return 0;
        case WM_LBUTTONUP: {
            const float scale = g_settings.scalePercent / 100.0f;
            const int x = static_cast<int>(GET_X_LPARAM(lParam) / scale);
            const int y = static_cast<int>(GET_Y_LPARAM(lParam) / scale);
            auto tracks = GetTracks();
            if (x >= 245 && y >= 20 && y <= 48 && tracks.size() > 1) {
                g_selectedIndex = (g_selectedIndex.load() + 1) % static_cast<int>(tracks.size());
                RenderWidget();
            } else if (y >= 138 && y <= 156 && !tracks.empty()) {
                const auto& track = tracks[(std::clamp)(g_selectedIndex.load(), 0, static_cast<int>(tracks.size()) - 1)];
                if (track.canSeek && track.durationMs > 0) {
                    const int xPosition = (std::clamp)(x - 30, 0, 420);
                    g_pendingSeekMs = track.startMs + track.durationMs * xPosition / 420;
                    SetEvent(g_mediaWake);
                }
            } else if (y >= 180 && y <= 235) {
                if (!tracks.empty()) {
                    const auto& track = tracks[(std::clamp)(g_selectedIndex.load(), 0, static_cast<int>(tracks.size()) - 1)];
                    if (x >= 135 && x <= 202 && track.canPrevious) g_pendingAction = 2;
                    else if (x >= 208 && x <= 272 && track.canPlayPause) g_pendingAction = 1;
                    else if (x >= 279 && x <= 344 && track.canNext) g_pendingAction = 3;
                    if (g_pendingAction.load() != 0) SetEvent(g_mediaWake);
                }
            } else if (y >= 235 && y <= 260 && x >= 145 && x <= 455) {
                SetMasterVolume((std::clamp)((x - 150) / 300.0f, 0.0f, 1.0f));
            }
            RenderWidget();
            return 0;
        }
        case WM_DISPLAYCHANGE:
        case WM_SETTINGCHANGE:
            RenderWidget();
            return 0;
        case WM_DESTROY:
            KillTimer(hwnd, kRefreshTimer);
            PostQuitMessage(0);
            return 0;
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
    if (GdiplusStartup(&g_gdiplusToken, &startup, nullptr) != Ok) return 0;
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    LoadSettings();

    // Register backdrop window class for GPU acrylic/blur
    const wchar_t backdropClassName[] = L"WindhawkDesktopMediaBackdrop";
    WNDCLASSEXW bwc{};
    bwc.cbSize = sizeof(bwc);
    bwc.hInstance = g_instance;
    bwc.lpfnWndProc = BackdropWndProc;
    bwc.lpszClassName = backdropClassName;
    bwc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    RegisterClassExW(&bwc);

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
    RegisterClassExW(&wc);
    g_window = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        className, L"Desktop Media Glass", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, g_instance, nullptr);
    if (!g_window) {
        if (g_backdropWindow) {
            DestroyWindow(g_backdropWindow);
            g_backdropWindow = nullptr;
        }
        UnregisterClassW(backdropClassName, g_instance);
        CoUninitialize();
        GdiplusShutdown(g_gdiplusToken);
        return 0;
    }
    RenderWidget();
    SetTimer(g_window, kRefreshTimer, 500, nullptr);
    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    g_window = nullptr;
    if (g_backdropWindow) {
        DestroyWindow(g_backdropWindow);
        g_backdropWindow = nullptr;
    }
    UnregisterClassW(className, g_instance);
    UnregisterClassW(backdropClassName, g_instance);
    CoUninitialize();
    GdiplusShutdown(g_gdiplusToken);
    return 0;
}

BOOL Wh_ModInit() {
    g_instance = reinterpret_cast<HINSTANCE>(&__ImageBase);
    g_running = true;
    g_mediaWake = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_mediaReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_mediaWake || !g_mediaReady) return FALSE;
    g_mediaThread = CreateThread(nullptr, 0, MediaThreadProc, nullptr, 0, nullptr);
    if (!g_mediaThread) return FALSE;
    WaitForSingleObject(g_mediaReady, 5000);
    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, nullptr);
    return g_uiThread != nullptr;
}

void Wh_ModSettingsChanged() {
    if (g_window) PostMessageW(g_window, WM_APP + 32, 0, 0);
}

void Wh_ModUninit() {
    g_running = false;
    if (g_mediaWake) SetEvent(g_mediaWake);
    if (g_window) PostMessageW(g_window, WM_CLOSE, 0, 0);
    if (g_uiThread) { WaitForSingleObject(g_uiThread, 3000); CloseHandle(g_uiThread); g_uiThread = nullptr; }
    if (g_mediaThread) { WaitForSingleObject(g_mediaThread, 5000); CloseHandle(g_mediaThread); g_mediaThread = nullptr; }
    if (g_mediaWake) { CloseHandle(g_mediaWake); g_mediaWake = nullptr; }
    if (g_mediaReady) { CloseHandle(g_mediaReady); g_mediaReady = nullptr; }
}
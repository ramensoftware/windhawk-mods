// ==WindhawkMod==
// @id              parallax-wallpaper
// @name            Parallax Wallpaper
// @description     Smooth mouse-reactive wallpaper with four motion presets, a floating dead zone, independent multi-monitor support, and HDR color management
// @version         0.6.0
// @author          HaVeN80
// @github          https://github.com/haven80
// @license         MIT
// @include         explorer.exe
// @compilerOptions -lgdi32 -lgdiplus -lole32 -luuid -ldwmapi -ld2d1 -ld3d11 -ldxgi
// ==/WindhawkMod==

// clang-format off
// ==WindhawkModReadme==
/*
# Parallax Wallpaper

Adds subtle depth to your desktop by moving the wallpaper in response to the
mouse. Desktop icons stay in place. Each monitor uses its own Windows wallpaper
or an optional local image.

## Motion presets

| Preset | Character | Horizontal / vertical travel | Approx. settling time | Dead zone |
|---|---|---|---|---|
| Elegance | Subtle and restrained | 14 / 8 px | 750 ms | 3% |
| Silk | Soft and floating | 22 / 12 px | 1050 ms | 4% |
| Depth | More noticeable, with a quicker response | 32 / 18 px | 850 ms | 2% |
| Cinema | Broad and slow | 42 / 24 px | 1350 ms | 5% |

Travel is the maximum distance in each direction. Settling time is approximately
how long the critically damped motion takes to reach 95% of a suddenly changed
target from rest. Actual motion depends on the cursor trajectory.

Start with **Elegance**, **Floating dead zone = -1**, manual customization off,
**Subpixel motion** on, **HDR color management** on, and **Frame rate limit = 0**
(automatic).

### Floating dead zone

Small pointer adjustments do not change the animation target. The tolerance
follows the last accepted target instead of applying only at the screen center.
Once the pointer crosses the boundary, the target moves continuously, without a
position jump. Motion that has already started still settles gently.

- `-1` uses each preset's dead zone.
- `0` disables the dead zone while retaining smooth acceleration and deceleration.
- `3` gives a radius equivalent to 3% of the screen width horizontally and 3% of
  its height vertically. Larger values provide more separation from the cursor.

Manual travel and settling-time values apply only when manual customization is
on. Saved values from earlier versions do not override the preset otherwise.

## Multiple monitors

With **Independent monitors** enabled, only the monitor under the pointer receives
new targets. Other monitors finish their existing movement and then stop. When
disabled, all monitors follow the pointer's relative position on the active
monitor. Images remain separate; this is not a continuous panoramic wallpaper.

Display connections, disconnections, resolution changes, and wallpaper changes
are detected automatically. Rebuilding the layout resets the motion to the center.

### Per-monitor rules

Add entries under **Per-monitor rules** using `DISPLAY1`, `DISPLAY2`, etc., or the
full hardware path from the mod log. These names do not necessarily match the
numbers shown by Windows' Identify button. DISPLAY names can change when devices
are reconnected; a full hardware path provides a more stable match.

To find the identifiers, enable mod logging in Windhawk's Advanced tab, open the
log output, and save the settings. Each monitor is logged with its DISPLAY name,
rectangle, and `id`. Copy the complete `id` for an exact, case-insensitive match.
Logging can then be disabled.

The first matching rule wins. Fill entries consecutively: an empty monitor field
ends the list. Up to 32 rules are supported. A rule can inherit the global motion
settings, choose another preset, use manual motion values, disable the effect, or
choose a local image. A blank image path uses that monitor's Windows wallpaper.
Image overrides do not change Windows' saved wallpaper settings.

## HDR color management

On a monitor where Windows HDR ("Use HDR", also called Advanced color) is turned
on, the wallpaper is normally treated by the desktop compositor as an ordinary
8-bit sRGB image and can look washed out or clipped next to genuine HDR content.
With **HDR color management** enabled, such monitors render through a separate
Direct3D 11 / Direct2D pipeline: a wide-gamut (scRGB) swap chain paired with
Direct2D's built-in color-management effect, which explicitly tags the decoded
image as sRGB and converts it for the HDR swap chain. The visual result is the
same picture, correctly reproduced under Windows' HDR color management, rather
than an attempt to display real high-dynamic-range highlights — source images
are still ordinary SDR files.

Monitors without HDR enabled are unaffected and keep using the standard
rendering path. HDR rendering also requires **Subpixel motion** to be enabled,
since it reuses the same Direct2D pipeline; if HDR color management, the
required GPU pipeline, or the swap chain for a specific monitor is unavailable,
that monitor falls back to standard rendering and, eventually, plain GDI, the
same way Direct2D failures are already handled. Failures are written to the mod
log together with the HRESULT.

## Performance and compatibility

Images are resized once and reused. Direct2D enables fractional-pixel movement;
GDI rendering is used if Direct2D fails, with a retry after two seconds. Subpixel
motion can also be disabled manually if graphics problems occur; this also turns
off HDR color management, since it depends on the same rendering path.

Automatic frame rate follows the highest detected display refresh rate, capped
at 240 Hz. This is a requested rate, not a guarantee of FPS or hardware frame
synchronization. Windows' compositor controls presentation. A high-resolution
waitable timer is used without changing the global timer resolution. When motion
settles, identical frames are not redrawn and pointer polling drops to at most
60 Hz. Memory usage grows with the total display resolution.

The fullscreen pause checks the foreground application every 250 ms and pauses
only the monitors it completely covers. It does not inspect every background
window. Desktop attachment supports the classic WorkerW layout and the layered
Windows 11 layout. This undocumented Explorer mechanism can change in Windows
updates; unrecognized layouts are left untouched.

## Tested on

- Windows 11, build 26200.995.
- Dual-monitor setup, two 3440x1440 ultrawide displays.
- HDR color management: toggling Windows HDR ("Use HDR") on and off while the
  mod was already running was picked up live on the affected monitor, with no
  need to restart Explorer or reload the mod.
- The mod kept working correctly across resolution changes while running.
- GPU/driver were not recorded for this test. HDR color management only needs
  a Direct3D 11-capable GPU, which covers effectively all hardware able to run
  Windows 10/11, but it has not been verified across GPU vendors.

## Limitations

- Intended for Windows 10 and Windows 11; runtime testing is described in the PR.
- Images use aspect-preserving **Fill** cropping with extra space for movement.
  Fit, Center, Tile, and continuous Span are not implemented.
- Rotation and transitions between images are not included.
- HDR color management corrects color reproduction on HDR-enabled monitors; it
  does not add real high-dynamic-range content, and requires a Direct3D
  11-capable GPU and driver.
- Solid-color desktops without an image remain unchanged.
- Loading very large images can briefly interrupt animation.
- Do not combine this mod with another animated-wallpaper application.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- motionPreset: elegance
  $name: Motion preset
  $options:
  - elegance: Elegance - subtle and restrained
  - silk: Silk - soft and floating
  - depth: Depth - noticeable and composed
  - cinema: Cinema - broad and slow
- thresholdPercent: -1
  $name: Floating dead zone (%)
  $description: -1 uses the preset. Range 0 to 20. Zero follows every pointer movement; 3 ignores approximately 3% of the screen.
- manualMotion: false
  $name: Customize travel and settling time
  $description: Enable to use the next three fields instead of the preset values.
- strength: 25
  $name: Horizontal travel (pixels)
  $description: Range 0 to 150. Used only when manual customization is enabled.
- strengthY: 25
  $name: Vertical travel (pixels)
  $description: Range 0 to 150. Used only when manual customization is enabled.
- responseMs: 750
  $name: Approximate settling time (ms)
  $description: Range 200 to 2500. Time to reach approximately 95% of a new target, used only in manual mode.
- opposite: true
  $name: Move opposite to the pointer
- independent: true
  $name: Independent monitors
  $description: Each monitor follows its own pointer and finishes its motion gently. Disable to synchronize monitors.
- smoothFps: 0
  $name: Frame rate limit
  $description: 0 follows the highest detected refresh rate, up to 240. Otherwise choose 30 to 240; 60 reduces resource usage.
- smoothRendering: true
  $name: Subpixel motion
  $description: Recommended for the smoothest movement. Disable if graphics problems occur. HDR color management also requires this to be enabled.
- hdrColorManagement: true
  $name: HDR color management
  $description: On monitors with Windows HDR (Advanced color) turned on, renders through a wide-gamut Direct2D pipeline so wallpaper colors match the desktop's HDR color management instead of being treated as a plain 8-bit image. No effect on monitors without HDR enabled, or if Subpixel motion is off.
- pauseFullscreen: true
  $name: Pause for fullscreen applications
  $description: Pauses monitors completely covered by the foreground application.
- monitors:
  - - monitor: ""
      $name: Monitor
      $description: DISPLAY1, DISPLAY2, or the full hardware path from the log. An empty entry ends the list.
    - enabled: true
      $name: Enable the effect on this monitor
    - preset: inherit
      $name: Motion preset
      $options:
      - inherit: Use global settings
      - elegance: Elegance
      - silk: Silk
      - depth: Depth
      - cinema: Cinema
    - manualMotion: false
      $name: Customize travel and settling time for this monitor
      $description: Enable to use the next three fields instead of the preset.
    - strengthX: 25
      $name: Horizontal travel (pixels)
    - strengthY: 25
      $name: Vertical travel (pixels)
    - responseMs: 750
      $name: Approximate settling time (ms)
    - opposite: true
      $name: Move opposite to the pointer
    - image: ""
      $name: Custom image
      $description: Full path to a local image, without surrounding quotes. Leave empty to use the Windows wallpaper.
  $name: Per-monitor rules
*/
// ==/WindhawkModSettings==

// clang-format on
#define NOMINMAX
#define INITGUID
#ifndef WINVER
#define WINVER 0x0A00
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1effects.h>
#include <d3d11.h>
#include <dwmapi.h>
#include <dxgi1_6.h>
#include <gdiplus.h>
#include <objbase.h>
#include <objidl.h>
#include <shobjidl.h>
#include <windows.h>
#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

#ifndef WS_EX_NOREDIRECTIONBITMAP
#define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#endif

constexpr wchar_t kClass[] = L"WindhawkParallaxWallpaper060";
HANDLE g_thread = nullptr, g_stop = nullptr, g_settingsEvent = nullptr;
// Only the worker thread touches the state below.
HWND g_controller = nullptr, g_parent = nullptr, g_icons = nullptr;
IDesktopWallpaper* g_wallpaper = nullptr;
ID2D1Factory1* g_d2d = nullptr;
// Lazily created the first time a monitor needs the HDR color-management
// pipeline; shared across all panes.
ID2D1Device* g_d2d1Device = nullptr;
ID3D11Device* g_d3dDevice = nullptr;
IDXGIDevice* g_dxgiDevice = nullptr;
IDXGIFactory2* g_dxgiFactory = nullptr;
bool g_hdrDeviceFailed = false;
bool g_rebuild = true, g_rebuilding = false, g_retryWindows = false;
ULONGLONG g_nextMaintenance = 0;
double g_lastTick = 0;

double ClockMs() {
    static const double frequency = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return double(f.QuadPart);
    }();
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return double(counter.QuadPart) * 1000.0 / frequency;
}

struct Options {
    int x = 14, y = 8, response = 750;
    double threshold = 0.06;  // 3% screen displacement in [-1, 1] coordinates.
    bool enabled = true, opposite = true;
    std::wstring image;
};
struct Rule {
    std::wstring selector;
    Options options;
};
Options g_defaults;
std::vector<Rule> g_rules;
bool g_independent = true, g_pauseFullscreen = true;
bool g_smoothRendering = true;
bool g_hdrColorManagement = true;
int g_fps = 60, g_requestedFps = 0;

struct Display {
    RECT rect{};
    std::wstring name, id;
    bool hdr = false;  // Windows "Advanced color" (HDR) currently enabled.
};
struct Pane {
    Display display;
    Options options;
    HWND window = nullptr;
    HDC cache = nullptr;
    HBITMAP bitmap = nullptr, oldBitmap = nullptr;
    void* pixels = nullptr;
    ID2D1HwndRenderTarget* target = nullptr;
    ID2D1Bitmap* texture = nullptr;
    // HDR color-management pipeline (used instead of the pair above when the
    // monitor has HDR enabled and the setting allows it).
    IDXGISwapChain1* hdrSwap = nullptr;
    ID2D1DeviceContext* hdrContext = nullptr;
    ID2D1Bitmap1* hdrTarget = nullptr;
    ID2D1Bitmap1* hdrSource = nullptr;
    ID2D1Effect* hdrColorEffect = nullptr;
    double retryRenderer = 0;
    int width = 0, height = 0, cropX = 0, cropY = 0;
    double x = 0, y = 0;
    double vx = 0, vy = 0, acceptedX = 0, acceptedY = 0;
    double submittedX = 0, submittedY = 0;
    bool paused = false;
    std::wstring path;
    FILETIME stamp{};
    Pane() = default;
    Pane(const Pane&) = delete;
    Pane& operator=(const Pane&) = delete;
    void FreeRenderer() {
        if (texture)
            texture->Release();
        if (target)
            target->Release();
        texture = nullptr;
        target = nullptr;
        if (hdrColorEffect)
            hdrColorEffect->Release();
        if (hdrSource)
            hdrSource->Release();
        if (hdrTarget)
            hdrTarget->Release();
        if (hdrContext)
            hdrContext->Release();
        if (hdrSwap)
            hdrSwap->Release();
        hdrColorEffect = nullptr;
        hdrSource = nullptr;
        hdrTarget = nullptr;
        hdrContext = nullptr;
        hdrSwap = nullptr;
    }
    void FreeCache() {
        FreeRenderer();
        if (cache) {
            SelectObject(cache, oldBitmap);
            DeleteObject(bitmap);
            DeleteDC(cache);
        }
        cache = nullptr;
        bitmap = oldBitmap = nullptr;
        pixels = nullptr;
    }
    ~Pane() {
        if (window && IsWindow(window))
            DestroyWindow(window);
        FreeCache();
    }
};
std::vector<std::unique_ptr<Pane>> g_panes;
std::vector<Display> g_layout;

std::wstring StringSetting(const wchar_t* key) {
    const wchar_t* value = Wh_GetStringSetting(key);
    std::wstring result = value ? value : L"";
    if (value)
        Wh_FreeStringSetting(value);
    return result;
}
Options Preset(const std::wstring& name) {
    Options options;
    if (name == L"silk") {
        options.x = 22;
        options.y = 12;
        options.response = 1050;
        options.threshold = 0.08;
    } else if (name == L"depth") {
        options.x = 32;
        options.y = 18;
        options.response = 850;
        options.threshold = 0.04;
    } else if (name == L"cinema") {
        options.x = 42;
        options.y = 24;
        options.response = 1350;
        options.threshold = 0.10;
    }
    return options;  // Elegance also handles missing/unknown settings on
                     // upgrade.
}
void ReadSettings() {
    g_defaults = Preset(StringSetting(L"motionPreset"));
    if (Wh_GetIntSetting(L"manualMotion")) {
        g_defaults.x = std::clamp(Wh_GetIntSetting(L"strength"), 0, 150);
        g_defaults.y = std::clamp(Wh_GetIntSetting(L"strengthY"), 0, 150);
        g_defaults.response =
            std::clamp(Wh_GetIntSetting(L"responseMs"), 200, 2500);
    }
    int threshold = std::clamp(Wh_GetIntSetting(L"thresholdPercent"), -1, 20);
    if (threshold >= 0)
        g_defaults.threshold = threshold / 50.0;
    g_defaults.opposite = Wh_GetIntSetting(L"opposite") != 0;
    g_independent = Wh_GetIntSetting(L"independent") != 0;
    g_pauseFullscreen = Wh_GetIntSetting(L"pauseFullscreen") != 0;
    g_requestedFps = Wh_GetIntSetting(L"smoothFps");
    if (g_requestedFps)
        g_requestedFps = std::clamp(g_requestedFps, 30, 240);
    g_smoothRendering = Wh_GetIntSetting(L"smoothRendering") != 0;
    g_hdrColorManagement = Wh_GetIntSetting(L"hdrColorManagement") != 0;
    g_rules.clear();
    for (int i = 0; i < 32; ++i) {
        std::wstring prefix = L"monitors[" + std::to_wstring(i) + L"].";
        Rule rule;
        rule.selector = StringSetting((prefix + L"monitor").c_str());
        if (rule.selector.empty())
            break;
        std::wstring preset = StringSetting((prefix + L"preset").c_str());
        rule.options = preset.empty() || preset == L"inherit" ? g_defaults
                                                              : Preset(preset);
        if (threshold >= 0)
            rule.options.threshold = threshold / 50.0;
        rule.options.enabled =
            Wh_GetIntSetting((prefix + L"enabled").c_str()) != 0;
        if (Wh_GetIntSetting((prefix + L"manualMotion").c_str())) {
            rule.options.x = std::clamp(
                Wh_GetIntSetting((prefix + L"strengthX").c_str()), 0, 150);
            rule.options.y = std::clamp(
                Wh_GetIntSetting((prefix + L"strengthY").c_str()), 0, 150);
            rule.options.response = std::clamp(
                Wh_GetIntSetting((prefix + L"responseMs").c_str()), 200, 2500);
        }
        rule.options.opposite =
            Wh_GetIntSetting((prefix + L"opposite").c_str()) != 0;
        rule.options.image = StringSetting((prefix + L"image").c_str());
        g_rules.push_back(std::move(rule));
    }
}

bool Matches(const std::wstring& selector, const Display& d) {
    std::wstring shortName = d.name;
    if (shortName.rfind(L"\\\\.\\", 0) == 0)
        shortName.erase(0, 4);
    return !selector.empty() &&
           (!_wcsicmp(selector.c_str(), d.name.c_str()) ||
            !_wcsicmp(selector.c_str(), shortName.c_str()) ||
            (!d.id.empty() && !_wcsicmp(selector.c_str(), d.id.c_str())));
}
Options OptionsFor(const Display& d) {
    for (const auto& rule : g_rules)
        if (Matches(rule.selector, d))
            return rule.options;
    return g_defaults;
}

// Whether Windows "Advanced color" (HDR) is currently turned on for the
// monitor identified by its GDI device name (e.g. "\\.\DISPLAY1"). Absent or
// failing APIs (older Windows, no eligible display) simply report false.
bool DisplayIsHdr(const std::wstring& deviceName) {
    UINT32 pathCount = 0, modeCount = 0;
    if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount,
                                    &modeCount) != ERROR_SUCCESS ||
        !pathCount)
        return false;
    std::vector<DISPLAYCONFIG_PATH_INFO> paths(pathCount);
    std::vector<DISPLAYCONFIG_MODE_INFO> modes(modeCount);
    if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths.data(),
                           &modeCount, modes.data(),
                           nullptr) != ERROR_SUCCESS)
        return false;
    paths.resize(pathCount);
    for (const auto& path : paths) {
        DISPLAYCONFIG_SOURCE_DEVICE_NAME source{};
        source.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
        source.header.size = sizeof(source);
        source.header.adapterId = path.sourceInfo.adapterId;
        source.header.id = path.sourceInfo.id;
        if (DisplayConfigGetDeviceInfo(&source.header) != ERROR_SUCCESS ||
            _wcsicmp(source.viewGdiDeviceName, deviceName.c_str()))
            continue;
        DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO color{};
        color.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_ADVANCED_COLOR_INFO;
        color.header.size = sizeof(color);
        color.header.adapterId = path.targetInfo.adapterId;
        color.header.id = path.targetInfo.id;
        return DisplayConfigGetDeviceInfo(&color.header) == ERROR_SUCCESS &&
               color.advancedColorEnabled != 0;
    }
    return false;
}

BOOL CALLBACK EnumerateDisplay(HMONITOR monitor, HDC, LPRECT, LPARAM data) {
    MONITORINFOEXW info{};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, &info))
        return TRUE;
    Display d;
    d.rect = info.rcMonitor;
    d.name = info.szDevice;
    DISPLAY_DEVICEW device{};
    device.cb = sizeof(device);
    if (EnumDisplayDevicesW(info.szDevice, 0, &device,
                            EDD_GET_DEVICE_INTERFACE_NAME))
        d.id = device.DeviceID;
    d.hdr = DisplayIsHdr(d.name);
    reinterpret_cast<std::vector<Display>*>(data)->push_back(std::move(d));
    return TRUE;
}
std::vector<Display> Displays() {
    std::vector<Display> displays;
    EnumDisplayMonitors(nullptr, nullptr, EnumerateDisplay,
                        reinterpret_cast<LPARAM>(&displays));
    std::sort(
        displays.begin(), displays.end(),
        [](const Display& a, const Display& b) { return a.name < b.name; });
    return displays;
}
bool SameLayout(const std::vector<Display>& a, const std::vector<Display>& b) {
    if (a.size() != b.size())
        return false;
    for (size_t i = 0; i < a.size(); ++i)
        if (a[i].name != b[i].name || a[i].id != b[i].id ||
            a[i].hdr != b[i].hdr || !EqualRect(&a[i].rect, &b[i].rect))
            return false;
    return true;
}

BOOL CALLBACK FindIconHost(HWND window, LPARAM result) {
    DWORD process = 0;
    GetWindowThreadProcessId(window, &process);
    if (process == GetCurrentProcessId() && IsWindowVisible(window) &&
        FindWindowExW(window, nullptr, L"SHELLDLL_DefView", nullptr)) {
        *reinterpret_cast<HWND*>(result) = window;
        return FALSE;
    }
    return TRUE;
}
HWND FindWallpaperHost(HWND& iconView) {
    iconView = nullptr;
    HWND progman = FindWindowW(L"Progman", nullptr);
    DWORD owner = 0;
    if (progman) {
        GetWindowThreadProcessId(progman, &owner);
        HWND view =
            FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);
        HWND wallpaper = FindWindowExW(progman, nullptr, L"WorkerW", nullptr);
        if (owner == GetCurrentProcessId() && view && wallpaper &&
            (GetWindowLongPtrW(progman, GWL_EXSTYLE) &
             WS_EX_NOREDIRECTIONBITMAP) &&
            (GetWindowLongPtrW(view, GWL_EXSTYLE) & WS_EX_LAYERED)) {
            iconView = view;
            return progman;
        }
    }
    HWND icons = nullptr;
    EnumWindows(FindIconHost, reinterpret_cast<LPARAM>(&icons));
    if (!icons)
        return nullptr;
    HWND host = FindWindowExW(nullptr, icons, L"WorkerW", nullptr);
    GetWindowThreadProcessId(host, &owner);
    if (!host || !IsWindowVisible(host) || owner != GetCurrentProcessId() ||
        FindWindowExW(host, nullptr, L"SHELLDLL_DefView", nullptr))
        return nullptr;
    return host;
}
bool Position(Pane& pane, UINT flags) {
    if (!pane.window || !IsWindow(pane.window))
        return false;
    POINT origin{pane.display.rect.left, pane.display.rect.top};
    MapWindowPoints(nullptr, g_parent, &origin, 1);
    if (!IsWindow(g_parent) || (g_icons && !IsWindow(g_icons)))
        return false;
    return SetWindowPos(pane.window, g_icons ? g_icons : HWND_BOTTOM, origin.x,
                        origin.y, pane.width, pane.height,
                        SWP_NOACTIVATE | flags) != FALSE;
}
bool LayerOrderValid(const Pane& pane) {
    if (!g_icons)
        return true;
    for (HWND previous = GetWindow(pane.window, GW_HWNDPREV); previous;
         previous = GetWindow(previous, GW_HWNDPREV)) {
        if (previous == g_icons)
            return true;
        wchar_t name[64]{};
        GetClassNameW(previous, name, ARRAYSIZE(name));
        if (!wcscmp(name, L"WorkerW"))
            return false;
    }
    return false;
}

bool WallpaperPath(const Pane& pane, std::wstring& path) {
    if (!pane.options.image.empty()) {
        path = pane.options.image;
        return true;
    }
    if (g_wallpaper) {
        LPWSTR value = nullptr;
        // The hardware path normally matches IDesktopWallpaper's identifier.
        HRESULT hr =
            pane.display.id.empty()
                ? E_FAIL
                : g_wallpaper->GetWallpaper(pane.display.id.c_str(), &value);
        if (SUCCEEDED(hr)) {
            path = value ? value : L"";
            CoTaskMemFree(value);
            return true;
        }
        CoTaskMemFree(value);
        // Match by screen rectangle if a driver uses a different identifier.
        UINT count = 0;
        if (SUCCEEDED(g_wallpaper->GetMonitorDevicePathCount(&count))) {
            for (UINT i = 0; i < count; ++i) {
                LPWSTR id = nullptr;
                RECT rect{};
                if (g_wallpaper->GetMonitorDevicePathAt(i, &id) == S_OK && id &&
                    g_wallpaper->GetMonitorRECT(id, &rect) == S_OK &&
                    EqualRect(&rect, &pane.display.rect)) {
                    value = nullptr;
                    hr = g_wallpaper->GetWallpaper(id, &value);
                    if (SUCCEEDED(hr))
                        path = value ? value : L"";
                    CoTaskMemFree(value);
                    CoTaskMemFree(id);
                    return SUCCEEDED(hr);
                }
                CoTaskMemFree(id);
            }
        }
        return false;  // Don't substitute another monitor's wallpaper on a
                       // transient failure.
    }
    wchar_t fallback[32768]{};
    if (!SystemParametersInfoW(SPI_GETDESKWALLPAPER, ARRAYSIZE(fallback),
                               fallback, 0))
        return false;
    path = fallback;
    return true;
}
FILETIME FileStamp(const std::wstring& path) {
    WIN32_FILE_ATTRIBUTE_DATA data{};
    if (!GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &data))
        return {};
    return data.ftLastWriteTime;
}

bool BuildCache(Pane& pane, const std::wstring& path) {
    Gdiplus::Bitmap source(path.c_str());
    if (source.GetLastStatus() != Gdiplus::Ok || !source.GetWidth() ||
        !source.GetHeight())
        return false;
    int width = pane.width + 2 * pane.options.x,
        height = pane.height + 2 * pane.options.y;
    if (width <= 0 || height <= 0)
        return false;
    HDC screen = GetDC(nullptr);
    HDC cache = CreateCompatibleDC(screen);
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* pixels = nullptr;
    HBITMAP bitmap =
        CreateDIBSection(screen, &info, DIB_RGB_COLORS, &pixels, nullptr, 0);
    ReleaseDC(nullptr, screen);
    if (!cache || !bitmap) {
        if (cache)
            DeleteDC(cache);
        if (bitmap)
            DeleteObject(bitmap);
        return false;
    }
    HBITMAP old = static_cast<HBITMAP>(SelectObject(cache, bitmap));
    if (!old || old == HGDI_ERROR) {
        DeleteObject(bitmap);
        DeleteDC(cache);
        return false;
    }
    bool ok;
    {
        Gdiplus::Graphics graphics(cache);
        graphics.Clear(Gdiplus::Color(255, 0, 0, 0));
        graphics.SetInterpolationMode(
            Gdiplus::InterpolationModeHighQualityBicubic);
        double scale = std::max(double(width) / source.GetWidth(),
                                double(height) / source.GetHeight());
        float w = static_cast<float>(source.GetWidth() * scale),
              h = static_cast<float>(source.GetHeight() * scale);
        Gdiplus::ImageAttributes attributes;
        attributes.SetWrapMode(Gdiplus::WrapModeTileFlipXY);
        ok = graphics.DrawImage(
                 &source,
                 Gdiplus::RectF((width - w) / 2, (height - h) / 2, w, h), 0, 0,
                 static_cast<float>(source.GetWidth()),
                 static_cast<float>(source.GetHeight()), Gdiplus::UnitPixel,
                 &attributes) == Gdiplus::Ok;
    }
    if (!ok) {
        SelectObject(cache, old);
        DeleteObject(bitmap);
        DeleteDC(cache);
        return false;
    }
    pane.FreeCache();
    GdiFlush();  // Flush drawing before Direct2D reads the DIB memory.
    pane.cache = cache;
    pane.bitmap = bitmap;
    pane.oldBitmap = old;
    pane.pixels = pixels;
    pane.path = path;
    pane.stamp = FileStamp(path);
    pane.x = pane.y = 0;
    pane.vx = pane.vy = pane.acceptedX = pane.acceptedY = 0;
    pane.submittedX = pane.submittedY = 0;
    pane.retryRenderer = 0;
    pane.cropX = pane.options.x;
    pane.cropY = pane.options.y;
    return true;
}

bool DrawSmooth(Pane& pane) {
    if (!g_smoothRendering || !g_d2d || !pane.pixels ||
        ClockMs() < pane.retryRenderer)
        return false;
    HRESULT hr = S_OK;
    if (!pane.target) {
        auto properties = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_IGNORE),
            96, 96);
        auto window = D2D1::HwndRenderTargetProperties(
            pane.window, D2D1::SizeU(pane.width, pane.height),
            D2D1_PRESENT_OPTIONS_IMMEDIATELY);
        hr = g_d2d->CreateHwndRenderTarget(properties, window, &pane.target);
        if (SUCCEEDED(hr)) {
            UINT width = pane.width + 2 * pane.options.x,
                 height = pane.height + 2 * pane.options.y;
            auto bitmap = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_IGNORE),
                96, 96);
            hr = pane.target->CreateBitmap(D2D1::SizeU(width, height),
                                           pane.pixels, width * 4, bitmap,
                                           &pane.texture);
        }
    }
    if (SUCCEEDED(hr) && pane.target && pane.texture) {
        float x = static_cast<float>(pane.options.x + pane.x);
        float y = static_cast<float>(pane.options.y + pane.y);
        pane.target->BeginDraw();
        pane.target->DrawBitmap(
            pane.texture,
            D2D1::RectF(0, 0, static_cast<float>(pane.width),
                        static_cast<float>(pane.height)),
            1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            D2D1::RectF(x, y, x + pane.width, y + pane.height));
        hr = pane.target->EndDraw();
        if (SUCCEEDED(hr))
            return true;
    }
    Wh_Log(
        L"Parallax 0.6.0: smooth renderer unavailable for %s (0x%08lX); GDI "
        L"fallback, retry in 2s",
        pane.display.name.c_str(), static_cast<unsigned long>(hr));
    pane.FreeRenderer();
    pane.retryRenderer = ClockMs() + 2000;
    return false;
}

// Lazily creates the shared Direct3D 11 / Direct2D device used to render
// through a wide-gamut (scRGB) swap chain on HDR-enabled monitors. Failure is
// remembered so it is only attempted once per mod run.
bool EnsureHdrDevice() {
    if (g_d2d1Device)
        return true;
    if (g_hdrDeviceFailed || !g_d2d)
        return false;
    D3D_FEATURE_LEVEL level{};
    HRESULT hr = D3D11CreateDevice(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0, D3D11_SDK_VERSION,
        &g_d3dDevice, &level, nullptr);
    if (SUCCEEDED(hr))
        hr = g_d3dDevice->QueryInterface(IID_PPV_ARGS(&g_dxgiDevice));
    if (SUCCEEDED(hr))
        hr = g_d2d->CreateDevice(g_dxgiDevice, &g_d2d1Device);
    if (SUCCEEDED(hr)) {
        IDXGIAdapter* adapter = nullptr;
        hr = g_dxgiDevice->GetAdapter(&adapter);
        if (SUCCEEDED(hr)) {
            hr = adapter->GetParent(IID_PPV_ARGS(&g_dxgiFactory));
            adapter->Release();
        }
    }
    if (SUCCEEDED(hr) && g_d2d1Device && g_dxgiFactory)
        return true;
    Wh_Log(
        L"Parallax 0.6.0: HDR pipeline unavailable (0x%08lX); HDR-enabled "
        L"monitors use standard rendering",
        static_cast<unsigned long>(hr));
    if (g_dxgiFactory)
        g_dxgiFactory->Release();
    if (g_d2d1Device)
        g_d2d1Device->Release();
    if (g_dxgiDevice)
        g_dxgiDevice->Release();
    if (g_d3dDevice)
        g_d3dDevice->Release();
    g_dxgiFactory = nullptr;
    g_d2d1Device = nullptr;
    g_dxgiDevice = nullptr;
    g_d3dDevice = nullptr;
    g_hdrDeviceFailed = true;
    return false;
}
bool DrawHdr(Pane& pane) {
    if (!g_smoothRendering || !g_hdrColorManagement || !pane.display.hdr ||
        !pane.pixels || ClockMs() < pane.retryRenderer || !EnsureHdrDevice())
        return false;
    HRESULT hr = S_OK;
    if (!pane.hdrContext) {
        hr = g_d2d1Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
                                               &pane.hdrContext);
        if (SUCCEEDED(hr)) {
            DXGI_SWAP_CHAIN_DESC1 desc{};
            desc.Width = static_cast<UINT>(pane.width);
            desc.Height = static_cast<UINT>(pane.height);
            desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
            desc.SampleDesc.Count = 1;
            desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            desc.BufferCount = 2;
            desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
            desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
            hr = g_dxgiFactory->CreateSwapChainForHwnd(
                g_d3dDevice, pane.window, &desc, nullptr, nullptr,
                &pane.hdrSwap);
        }
        if (SUCCEEDED(hr)) {
            IDXGISwapChain3* swap3 = nullptr;
            if (SUCCEEDED(pane.hdrSwap->QueryInterface(IID_PPV_ARGS(&swap3)))) {
                // scRGB: linear light, primaries/white point of sRGB/Rec.709.
                swap3->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709);
                swap3->Release();
            }
        }
        ID2D1ColorContext *srgb = nullptr, *scrgb = nullptr;
        if (SUCCEEDED(hr)) {
            IDXGISurface* surface = nullptr;
            hr = pane.hdrSwap->GetBuffer(0, IID_PPV_ARGS(&surface));
            if (SUCCEEDED(hr)) {
                auto props = D2D1::BitmapProperties1(
                    D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                    D2D1::PixelFormat(DXGI_FORMAT_R16G16B16A16_FLOAT,
                                      D2D1_ALPHA_MODE_IGNORE));
                hr = pane.hdrContext->CreateBitmapFromDxgiSurface(
                    surface, props, &pane.hdrTarget);
                surface->Release();
            }
        }
        if (SUCCEEDED(hr))
            hr = pane.hdrContext->CreateEffect(CLSID_D2D1ColorManagement,
                                               &pane.hdrColorEffect);
        if (SUCCEEDED(hr))
            hr = pane.hdrContext->CreateColorContext(D2D1_COLOR_SPACE_SRGB,
                                                     nullptr, 0, &srgb);
        if (SUCCEEDED(hr))
            hr = pane.hdrContext->CreateColorContext(D2D1_COLOR_SPACE_SCRGB,
                                                     nullptr, 0, &scrgb);
        if (SUCCEEDED(hr)) {
            pane.hdrColorEffect->SetValue(D2D1_COLORMANAGEMENT_PROP_QUALITY,
                                          D2D1_COLORMANAGEMENT_QUALITY_BEST);
            pane.hdrColorEffect->SetValue(
                D2D1_COLORMANAGEMENT_PROP_SOURCE_COLOR_CONTEXT, srgb);
            pane.hdrColorEffect->SetValue(
                D2D1_COLORMANAGEMENT_PROP_DESTINATION_COLOR_CONTEXT, scrgb);
        }
        if (srgb)
            srgb->Release();
        if (scrgb)
            scrgb->Release();
        if (SUCCEEDED(hr)) {
            UINT width = pane.width + 2 * pane.options.x,
                 height = pane.height + 2 * pane.options.y;
            auto sourceProps = D2D1::BitmapProperties1(
                D2D1_BITMAP_OPTIONS_NONE,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_IGNORE));
            hr = pane.hdrContext->CreateBitmap(D2D1::SizeU(width, height),
                                               pane.pixels, width * 4,
                                               sourceProps, &pane.hdrSource);
        }
        if (SUCCEEDED(hr) && pane.hdrColorEffect && pane.hdrSource)
            pane.hdrColorEffect->SetInput(0, pane.hdrSource);
    }
    if (FAILED(hr) || !pane.hdrContext || !pane.hdrTarget || !pane.hdrSwap ||
        !pane.hdrColorEffect) {
        Wh_Log(
            L"Parallax 0.6.0: HDR renderer unavailable for %s (0x%08lX); "
            L"standard rendering, retry in 2s",
            pane.display.name.c_str(), static_cast<unsigned long>(hr));
        pane.FreeRenderer();
        pane.retryRenderer = ClockMs() + 2000;
        return false;
    }
    float x = static_cast<float>(pane.options.x + pane.x);
    float y = static_cast<float>(pane.options.y + pane.y);
    D2D1_POINT_2F offset = D2D1::Point2F(0, 0);
    D2D1_RECT_F crop = D2D1::RectF(x, y, x + pane.width, y + pane.height);
    pane.hdrContext->SetTarget(pane.hdrTarget);
    pane.hdrContext->BeginDraw();
    pane.hdrContext->DrawImage(pane.hdrColorEffect, &offset, &crop,
                               D2D1_INTERPOLATION_MODE_LINEAR,
                               D2D1_COMPOSITE_MODE_SOURCE_COPY);
    hr = pane.hdrContext->EndDraw();
    if (SUCCEEDED(hr)) {
        DXGI_PRESENT_PARAMETERS present{};
        hr = pane.hdrSwap->Present1(0, 0, &present);
    }
    if (SUCCEEDED(hr))
        return true;
    Wh_Log(
        L"Parallax 0.6.0: HDR present failed for %s (0x%08lX); standard "
        L"rendering, retry in 2s",
        pane.display.name.c_str(), static_cast<unsigned long>(hr));
    pane.FreeRenderer();
    pane.retryRenderer = ClockMs() + 2000;
    return false;
}

void RefreshImage(Pane& pane) {
    if (!pane.window || !IsWindow(pane.window))
        return;
    std::wstring path;
    if (!WallpaperPath(pane, path))
        return;
    if (!pane.window || !IsWindow(pane.window))
        return;  // COM can dispatch window messages.
    if (path.empty()) {
        ShowWindow(pane.window, SW_HIDE);
        pane.FreeCache();
        pane.path.clear();
        pane.stamp = {};
        return;
    }
    FILETIME stamp = FileStamp(path);
    if (pane.cache && path == pane.path &&
        CompareFileTime(&stamp, &pane.stamp) == 0) {
        if (!IsWindowVisible(pane.window) && Position(pane, SWP_SHOWWINDOW))
            InvalidateRect(pane.window, nullptr, FALSE);
        return;
    }
    if (BuildCache(pane, path)) {
        Position(pane, SWP_SHOWWINDOW);
        InvalidateRect(pane.window, nullptr, FALSE);
    } else {
        // Keep the last valid cache, if any; retry on the next maintenance
        // pass.
        Wh_Log(L"Parallax 0.6.0: unable to load image for %s: %s",
               pane.display.name.c_str(), path.c_str());
    }
}

bool Covers(const RECT& window, const RECT& monitor) {
    return window.left <= monitor.left && window.top <= monitor.top &&
           window.right >= monitor.right && window.bottom >= monitor.bottom;
}
void UpdatePause() {
    RECT rect{};
    bool full = false;
    HWND foreground = GetForegroundWindow();
    wchar_t cls[128]{};
    GetClassNameW(foreground, cls, ARRAYSIZE(cls));
    if (g_pauseFullscreen && foreground && IsWindowVisible(foreground) &&
        !IsIconic(foreground) && wcscmp(cls, L"Progman") &&
        wcscmp(cls, L"WorkerW") && wcscmp(cls, kClass) &&
        wcscmp(cls, L"Shell_TrayWnd") &&
        wcscmp(cls, L"Shell_SecondaryTrayWnd")) {
        DWORD cloaked = 0;
        DwmGetWindowAttribute(foreground, DWMWA_CLOAKED, &cloaked,
                              sizeof(cloaked));
        if (!cloaked) {
            full = SUCCEEDED(DwmGetWindowAttribute(
                foreground, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(rect)));
            if (!full)
                full = GetWindowRect(foreground, &rect) != FALSE;
        }
    }
    for (auto& p : g_panes) {
        bool pause = full && Covers(rect, p->display.rect);
        if (pause && !p->paused)
            p->vx = p->vy = 0;
        p->paused = pause;
    }
}

void Normalized(const RECT& rect, POINT cursor, double& x, double& y) {
    x = std::clamp(
        2.0 * (cursor.x - rect.left) / (rect.right - rect.left) - 1.0, -1.0,
        1.0);
    y = std::clamp(2.0 * (cursor.y - rect.top) / (rect.bottom - rect.top) - 1.0,
                   -1.0, 1.0);
}
void AcceptInput(Pane& pane, double nx, double ny) {
    double dx = nx - pane.acceptedX, dy = ny - pane.acceptedY;
    double distance = std::hypot(dx, dy);
    if (distance > pane.options.threshold) {
        double fraction = 1.0 - pane.options.threshold / distance;
        pane.acceptedX += dx * fraction;
        pane.acceptedY += dy * fraction;
    }
}
void SpringAxis(double& position,
                double& velocity,
                double target,
                double omega,
                double dt,
                double limit) {
    // Exact solution of the critically damped spring for a constant target.
    double offset = position - target;
    double c = velocity + omega * offset;
    double decay = std::exp(-omega * dt);
    position = target + (offset + c * dt) * decay;
    velocity = (velocity - omega * c * dt) * decay;
    if (position < -limit || position > limit) {
        position = std::clamp(position, -limit, limit);
        velocity = 0;
    }
    if (std::abs(position - target) < 0.001 && std::abs(velocity) < 0.005) {
        position = target;
        velocity = 0;
    }
}
bool Advance(Pane& pane, double elapsed) {
    double sign = pane.options.opposite ? 1.0 : -1.0;
    double omega = 4.744 / (pane.options.response / 1000.0);
    // Avoid a jump after a long stall/suspend; normal frame-rate changes remain
    // time-based.
    double dt = std::clamp(elapsed, 0.0, 50.0) / 1000.0;
    SpringAxis(pane.x, pane.vx, sign * pane.acceptedX * pane.options.x, omega,
               dt, pane.options.x);
    SpringAxis(pane.y, pane.vy, sign * pane.acceptedY * pane.options.y, omega,
               dt, pane.options.y);
    int x = std::clamp(pane.options.x + int(std::lround(pane.x)), 0,
                       2 * pane.options.x);
    int y = std::clamp(pane.options.y + int(std::lround(pane.y)), 0,
                       2 * pane.options.y);
    bool changed = g_smoothRendering
                       ? std::abs(pane.x - pane.submittedX) >= 0.005 ||
                             std::abs(pane.y - pane.submittedY) >= 0.005
                       : x != pane.cropX || y != pane.cropY;
    pane.cropX = x;
    pane.cropY = y;
    if (changed) {
        pane.submittedX = pane.x;
        pane.submittedY = pane.y;
    }
    return changed;
}
bool ShouldAnimate(const Pane& pane, const Display& active) {
    return !pane.paused && (!g_independent || pane.display.name == active.name);
}
bool Tick() {
    double now = ClockMs();
    double elapsed = now - g_lastTick;
    g_lastTick = now;
    POINT cursor{};
    if (!GetCursorPos(&cursor))
        return false;
    // Use all displays, including ones where the animation is disabled.
    const Display* active = nullptr;
    for (const auto& d : g_layout)
        if (PtInRect(&d.rect, cursor)) {
            active = &d;
            break;
        }
    double nx = 0, ny = 0;
    if (active)
        Normalized(active->rect, cursor, nx, ny);
    bool moving = false;
    for (auto& p : g_panes) {
        if (!p->cache || p->paused || !IsWindowVisible(p->window))
            continue;
        if (active && ShouldAnimate(*p, *active))
            AcceptInput(*p, nx, ny);
        if (Advance(*p, elapsed)) {
            moving = true;
            InvalidateRect(p->window, nullptr, FALSE);
            UpdateWindow(p->window);  // Paint once per scheduled frame, without
                                      // WM_PAINT starvation.
        }
        moving = moving || std::abs(p->vx) > 0.005 || std::abs(p->vy) > 0.005;
    }
    return moving;
}

LRESULT CALLBACK WindowProc(HWND window, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_NCCREATE) {
        auto create = reinterpret_cast<CREATESTRUCTW*>(lp);
        SetWindowLongPtrW(window, GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR>(create->lpCreateParams));
    }
    auto pane =
        reinterpret_cast<Pane*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_PAINT: {
            PAINTSTRUCT paint{};
            HDC dc = BeginPaint(window, &paint);
            if (pane && pane->cache && !DrawHdr(*pane) && !DrawSmooth(*pane)) {
                const RECT& r = paint.rcPaint;
                BitBlt(dc, r.left, r.top, r.right - r.left, r.bottom - r.top,
                       pane->cache, pane->cropX + r.left, pane->cropY + r.top,
                       SRCCOPY);
            }
            EndPaint(window, &paint);
            return 0;
        }
        case WM_TIMER:
            // Timers belong only to the hidden controller, which survives
            // monitor rebuilds.
            if (window == g_controller && !g_rebuilding) {
                if (wp == 2)
                    UpdatePause();
            }
            return 0;
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
            g_rebuild = true;
            return 0;
        case WM_NCDESTROY:
            if (pane) {
                pane->window = nullptr;
                if (!g_rebuilding)
                    g_rebuild = true;
            }
            SetWindowLongPtrW(window, GWLP_USERDATA, 0);
            break;
    }
    return DefWindowProcW(window, msg, wp, lp);
}

void Rebuild(const std::vector<Display>& layout, HWND parent, HWND icons) {
    g_rebuilding = true;
    g_rebuild = false;
    g_retryWindows = false;
    g_panes.clear();
    g_layout = layout;
    g_parent = parent;
    g_icons = icons;
    g_fps = g_requestedFps ? g_requestedFps : 60;
    if (!g_requestedFps) {
        for (const auto& d : layout) {
            DEVMODEW mode{};
            mode.dmSize = sizeof(mode);
            if (EnumDisplaySettingsW(d.name.c_str(), ENUM_CURRENT_SETTINGS,
                                     &mode))
                g_fps = std::max(
                    g_fps, std::clamp(static_cast<int>(mode.dmDisplayFrequency),
                                      30, 240));
        }
    }
    if (g_parent) {
        for (const auto& d : layout) {
            if (WaitForSingleObject(g_stop, 0) != WAIT_TIMEOUT)
                break;
            Wh_Log(L"Parallax 0.6.0: monitor %s, rect=(%ld,%ld,%ld,%ld), "
                   L"id=%s, hdr=%d",
                   d.name.c_str(), d.rect.left, d.rect.top, d.rect.right,
                   d.rect.bottom, d.id.c_str(), d.hdr ? 1 : 0);
            Options options = OptionsFor(d);
            if (!options.enabled)
                continue;
            auto p = std::make_unique<Pane>();
            p->display = d;
            p->options = options;
            p->width = d.rect.right - d.rect.left;
            p->height = d.rect.bottom - d.rect.top;
            if (p->width <= 0 || p->height <= 0)
                continue;
            p->window = CreateWindowExW(
                WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW |
                    (g_icons ? WS_EX_LAYERED | WS_EX_TRANSPARENT : 0),
                kClass, L"", WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 0, 0, g_parent,
                nullptr, GetModuleHandleW(nullptr), p.get());
            if (!p->window || (g_icons && !SetLayeredWindowAttributes(
                                              p->window, 0, 255, LWA_ALPHA))) {
                Wh_Log(L"Parallax 0.6.0: window setup failed for %s, error=%lu",
                       d.name.c_str(), GetLastError());
                g_retryWindows = true;
                continue;
            }
            RefreshImage(*p);
            g_panes.push_back(std::move(p));
        }
    } else {
        Wh_Log(L"Parallax 0.6.0: waiting for a supported desktop layout");
    }
    g_lastTick = ClockMs();
    g_rebuilding = false;
    UpdatePause();
}
void Maintenance() {
    // Explorer can have separate folder processes. Stop if this is not the
    // process that owns the desktop, including when the desktop appeared later.
    HWND shell = GetShellWindow();
    DWORD shellProcess = 0;
    if (shell)
        GetWindowThreadProcessId(shell, &shellProcess);
    if (shellProcess && shellProcess != GetCurrentProcessId()) {
        SetEvent(g_stop);
        return;
    }
    auto layout = Displays();
    HWND icons = nullptr;
    HWND parent = FindWallpaperHost(icons);
    if (!parent) {
        HWND progman = FindWindowW(L"Progman", nullptr);
        DWORD owner = 0;
        GetWindowThreadProcessId(progman, &owner);
        if (progman && owner == GetCurrentProcessId()) {
            DWORD_PTR result = 0;
            SendMessageTimeoutW(progman, 0x052C, 0xD, 1, SMTO_ABORTIFHUNG, 1000,
                                &result);
            parent = FindWallpaperHost(icons);
        }
    }
    if (g_rebuild || g_retryWindows || parent != g_parent || icons != g_icons ||
        !SameLayout(layout, g_layout)) {
        Rebuild(layout, parent, icons);
    } else {
        for (auto& p : g_panes) {
            RefreshImage(*p);
            if (g_icons && p->window && IsWindowVisible(p->window) &&
                !LayerOrderValid(*p))
                Position(*p, 0);
            bool wantHdr =
                g_smoothRendering && g_hdrColorManagement && p->display.hdr;
            bool rendererMissing = wantHdr ? !p->hdrTarget : !p->target;
            if (g_smoothRendering && p->cache && rendererMissing && p->window &&
                ClockMs() >= p->retryRenderer)
                InvalidateRect(p->window, nullptr, FALSE);
        }
    }
}

bool ArmFrameTimer(HANDLE timer, double deadline) {
    LARGE_INTEGER due;
    due.QuadPart =
        -static_cast<LONGLONG>(std::max(1.0, (deadline - ClockMs()) * 10000.0));
    return SetWaitableTimer(timer, &due, 0, nullptr, nullptr, FALSE) != FALSE;
}

DWORD WINAPI RenderThread(void*) {
    HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    using SetDpiProc = HANDLE(WINAPI*)(HANDLE);
    auto setDpi = reinterpret_cast<SetDpiProc>(GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "SetThreadDpiAwarenessContext"));
    HANDLE dpi = setDpi ? setDpi(reinterpret_cast<HANDLE>(-4)) : nullptr;
    ULONG_PTR token = 0;
    Gdiplus::GdiplusStartupInput input;
    HINSTANCE instance = GetModuleHandleW(nullptr);
    bool registered = false;
    HANDLE frameTimer = nullptr;
    g_parent = g_icons = nullptr;
    g_rebuild = true;
    g_hdrDeviceFailed = false;
    g_nextMaintenance = 0;
    if (Gdiplus::GdiplusStartup(&token, &input, nullptr) == Gdiplus::Ok) {
        if (FAILED(
                D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2d)))
            Wh_Log(
                L"Parallax 0.6.0: Direct2D initialization failed; using GDI");
        if (SUCCEEDED(com))
            CoCreateInstance(CLSID_DesktopWallpaper, nullptr, CLSCTX_ALL,
                             IID_PPV_ARGS(&g_wallpaper));
        if (!g_wallpaper)
            Wh_Log(
                L"Parallax 0.6.0: IDesktopWallpaper unavailable; using the "
                L"common wallpaper");
        ReadSettings();
        WNDCLASSW wc{};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = instance;
        wc.lpszClassName = kClass;
        registered = RegisterClassW(&wc) != 0;
        // Hidden top-level window receives display broadcasts; never shown.
        if (registered)
            g_controller = CreateWindowExW(WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
                                           kClass, L"", WS_POPUP, 0, 0, 0, 0,
                                           nullptr, nullptr, instance, nullptr);
        // High-resolution one-shot timer; no process/global timeBeginPeriod
        // changes.
        frameTimer = CreateWaitableTimerExW(nullptr, nullptr, 0x00000002,
                                            TIMER_ALL_ACCESS);
        if (!frameTimer)
            frameTimer = CreateWaitableTimerW(nullptr, FALSE, nullptr);
        double nextFrame = ClockMs();
        if (g_controller && frameTimer &&
            ArmFrameTimer(frameTimer, nextFrame) &&
            SetTimer(g_controller, 2, 250, nullptr)) {
            HANDLE events[]{g_stop, g_settingsEvent, frameTimer};
            bool running = true;
            while (running && WaitForSingleObject(g_stop, 0) == WAIT_TIMEOUT) {
                ULONGLONG now = GetTickCount64();
                if (g_rebuild || now >= g_nextMaintenance) {
                    Maintenance();
                    g_nextMaintenance = GetTickCount64() + 2000;
                }
                DWORD wait = MsgWaitForMultipleObjectsEx(
                    3, events, 250, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
                if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED)
                    break;
                if (wait == WAIT_OBJECT_0 + 1) {
                    ReadSettings();
                    g_rebuild = true;
                }
                if (wait == WAIT_OBJECT_0 + 2) {
                    bool moving = Tick();
                    int fps = moving ? g_fps : std::min(g_fps, 60);
                    nextFrame =
                        std::max(nextFrame + 1000.0 / fps, ClockMs() + 0.1);
                    if (!ArmFrameTimer(frameTimer, nextFrame))
                        break;
                }
                MSG msg{};
                // A bounded batch leaves room for shutdown and maintenance
                // under load.
                for (int i = 0;
                     i < 256 && PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE);
                     ++i) {
                    if (msg.message == WM_QUIT) {
                        running = false;
                        break;
                    }
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
            }
        } else
            Wh_Log(
                L"Parallax 0.6.0: controller/timer creation failed, error=%lu",
                GetLastError());
        g_rebuilding = true;
        if (frameTimer) {
            CancelWaitableTimer(frameTimer);
            CloseHandle(frameTimer);
        }
        g_panes.clear();
        g_layout.clear();
        g_rules.clear();
        if (g_controller)
            DestroyWindow(g_controller);
        g_controller = nullptr;
        if (registered)
            UnregisterClassW(kClass, instance);
        if (g_wallpaper) {
            g_wallpaper->Release();
            g_wallpaper = nullptr;
        }
        if (g_dxgiFactory) {
            g_dxgiFactory->Release();
            g_dxgiFactory = nullptr;
        }
        if (g_d2d1Device) {
            g_d2d1Device->Release();
            g_d2d1Device = nullptr;
        }
        if (g_dxgiDevice) {
            g_dxgiDevice->Release();
            g_dxgiDevice = nullptr;
        }
        if (g_d3dDevice) {
            g_d3dDevice->Release();
            g_d3dDevice = nullptr;
        }
        if (g_d2d) {
            g_d2d->Release();
            g_d2d = nullptr;
        }
        Gdiplus::GdiplusShutdown(token);
        g_rebuilding = false;
    }
    if (dpi && setDpi)
        setDpi(dpi);
    if (SUCCEEDED(com))
        CoUninitialize();
    return 0;
}

BOOL Wh_ModInit() {
    HWND shell = GetShellWindow();
    DWORD shellProcess = 0;
    if (shell)
        GetWindowThreadProcessId(shell, &shellProcess);
    if (shellProcess && shellProcess != GetCurrentProcessId())
        return FALSE;
    g_stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_settingsEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (g_stop && g_settingsEvent)
        g_thread = CreateThread(nullptr, 0, RenderThread, nullptr, 0, nullptr);
    if (g_thread)
        return TRUE;
    if (g_stop)
        CloseHandle(g_stop);
    if (g_settingsEvent)
        CloseHandle(g_settingsEvent);
    g_stop = g_settingsEvent = nullptr;
    return FALSE;
}
void Wh_ModBeforeUninit() {
    if (g_stop)
        SetEvent(g_stop);
}
void Wh_ModUninit() {
    if (g_thread) {
        SetEvent(g_stop);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stop)
        CloseHandle(g_stop);
    if (g_settingsEvent)
        CloseHandle(g_settingsEvent);
    g_stop = g_settingsEvent = nullptr;
}
void Wh_ModSettingsChanged() {
    if (g_settingsEvent)
        SetEvent(g_settingsEvent);
}

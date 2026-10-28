// ==WindhawkMod==
// @id              parallax-wallpaper
// @name            Parallax Wallpaper
// @description     Smooth mouse-reactive wallpaper with optional perspective, four motion presets, independent multi-monitor support, and HDR-aware rendering
// @version         0.9.0
// @author          HaVeN80
// @github          https://github.com/haven80
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lgdi32 -lgdiplus -lole32 -luuid -ldwmapi -ld2d1 -ld3d11 -ldxgi -ldcomp -lshell32
// ==/WindhawkMod==

// clang-format off
// ==WindhawkModReadme==
/*
# Parallax Wallpaper

Adds subtle depth to your desktop by moving the wallpaper in response to the
mouse. Desktop icons stay in place. Each monitor uses its own Windows wallpaper
or an optional local image.

The mod runs in its own background process rather than inside Explorer, so a
graphics driver problem cannot take the shell down with it.
![Parallax Wallpaper demo](https://i.imgur.com/jW1oE6J.gif)
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
on.

## Optional perspective

Enable **Enable perspective** with **Subpixel motion** on to tilt the wallpaper
as a flat panel following the pointer. Start with 2 degrees on both axes,
intensity 100%, and automatic edge compensation on. Perspective is off by
default.

Horizontal and vertical tilt range from 0 to 8 degrees. Intensity (0 to 200%)
controls perspective foreshortening; zero keeps an orthographic tilt.
Automatic compensation applies a constant zoom, computed once for the largest
tilt, so the screen stays covered without the image visibly zooming in and out
during motion. It crops a little more of the image. Disabling it may reveal
black edges.

Tilt shares the current motion preset's settling time, dead zone, direction,
fullscreen pause and independent-monitor behavior. It also works when travel
is set to zero. Per-monitor perspective can inherit global values, be disabled,
or use custom values independently of the monitor's motion preset.

Perspective is rendered with a Direct3D 11 / Direct2D effect pipeline, using
high-quality cubic sampling to avoid moving line patterns; on low-end GPUs,
lowering the frame rate limit reduces its cost. If the
pipeline is unavailable, the monitor falls back to the standard movement
without perspective. Disabling Subpixel motion also disables perspective. This
is a flat-image illusion, not a depth map or separate moving image layers.

## Multiple monitors

With **Independent monitors** enabled, only the monitor under the pointer receives
new targets. Other monitors finish their existing movement and then stop. When
disabled, all monitors follow the pointer's relative position on the active
monitor. Images remain separate; this is not a continuous panoramic wallpaper.

Display connections, disconnections, resolution changes, HDR changes,
wallpaper changes and Explorer restarts are detected automatically. Rebuilding the layout resets the
motion to the center.

### Per-monitor rules

Add entries under **Per-monitor rules** using `DISPLAY1`, `DISPLAY2`, etc., or the
full hardware path from the mod log. These names do not necessarily match the
numbers shown by Windows' Identify button. DISPLAY names can change when devices
are reconnected; a full hardware path provides a more stable match.

To find the identifiers, enable mod logging in Windhawk's Advanced tab, open the
log output, and save the settings. Each monitor is logged with its DISPLAY name,
rectangle, HDR state and `id`. Copy the complete `id` for an exact,
case-insensitive match. Logging can then be disabled.

The first matching rule wins. Fill entries consecutively: an empty monitor field
ends the list. Up to 32 rules are supported. A rule can inherit the global motion
settings and direction, choose another preset, use manual motion values, disable
the effect, or choose a local image. A blank image path uses that monitor's
Windows wallpaper. Image paths may contain environment variables such as
`%USERPROFILE%`. Image overrides do not change Windows' saved wallpaper settings.

## HDR color management

When Windows HDR ("Use HDR") is turned on for a monitor, this option renders the
wallpaper through a native wide-gamut (scRGB, FP16) swap chain: the image is
explicitly tagged as sRGB, converted with Direct2D's color-management effect,
and scaled to the **SDR content brightness** chosen in Windows' HDR settings.
The result matches how Windows presents other SDR content on that monitor, and
follows the brightness slider when it changes. Source images remain ordinary
SDR files; this does not add high-dynamic-range highlights.

Monitors without HDR, including SDR monitors that only use Windows' automatic
color management, keep the standard rendering path. The option requires
**Subpixel motion**. If the HDR or perspective pipeline fails on a monitor, that
monitor continues with standard rendering. Failures are retried with increasing
intervals and written to the mod log with the HRESULT. GPU resets (driver
updates, remote desktop sessions) are recovered automatically.

## Performance and compatibility

Images are decoded and resized once on a background thread and reused, so
loading a large image does not interrupt the animation. EXIF orientation is
honored. Supported formats are those GDI+ decodes (JPEG, PNG, BMP, GIF, TIFF);
other formats are reported once in the log and the monitor keeps Windows' own
wallpaper.

Rendering uses Direct3D 11, Direct2D and DirectComposition, which works on
both desktop layouts. Without a usable GPU driver, Direct3D's software renderer
(WARP) is used. If rendering fails on a monitor, that monitor shows the regular
Windows wallpaper and rendering is retried with increasing intervals. With
**Subpixel motion** off, the image moves in whole-pixel steps; this also turns
off perspective and HDR color management.

Automatic frame rate follows the highest detected display refresh rate, capped
at 240 Hz. This is a requested rate, not a guarantee of FPS or hardware frame
synchronization; Windows' compositor controls presentation. A high-resolution
waitable timer is used without changing the global timer resolution. When motion
settles, identical frames are not redrawn and pointer polling drops to at most
30 Hz, or 10 Hz while every monitor is paused. Memory usage grows with the total
display resolution.

The fullscreen pause checks the foreground application every 250 ms and pauses
only the monitors it completely covers. It does not inspect every background
window. Desktop attachment supports the classic WorkerW layout and the layered
Windows 11 layout. This undocumented Explorer mechanism can change in Windows
updates; unrecognized layouts are left untouched.

## Limitations

- Intended for Windows 10 and Windows 11.
- Images use aspect-preserving **Fill** cropping with extra space for movement.
  Fit, Center, Tile, and continuous Span are not implemented.
- Transitions between images are not included.
- Perspective and HDR color management are designed for a Direct3D 11-capable
  GPU; on the software renderer they work but cost noticeably more CPU.
- Solid-color desktops without an image remain unchanged.
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
- perspective: false
  $name: Enable perspective
  $description: Tilt the wallpaper with the pointer. Requires Subpixel motion and a working Direct3D 11 pipeline.
- perspectiveX: 2
  $name: Horizontal perspective tilt (degrees)
  $description: Range 0 to 8. Maximum tilt when moving the pointer horizontally.
- perspectiveY: 2
  $name: Vertical perspective tilt (degrees)
  $description: Range 0 to 8. Maximum tilt when moving the pointer vertically.
- perspectiveDepth: 100
  $name: Perspective intensity (%)
  $description: Range 0 to 200. Controls foreshortening; 0 uses an orthographic tilt. 100 is a gentle starting point.
- perspectiveAutoZoom: true
  $name: Automatically compensate perspective edges
  $description: Applies the minimum constant zoom needed to keep the screen covered at maximum tilt. Disabling this can reveal black edges.
- independent: true
  $name: Independent monitors
  $description: Each monitor follows its own pointer and finishes its motion gently. Disable to synchronize monitors.
- smoothFps: 0
  $name: Frame rate limit
  $description: 0 follows the highest detected refresh rate, up to 240. Otherwise choose 30 to 240; 60 reduces resource usage.
- smoothRendering: true
  $name: Subpixel motion
  $description: Recommended. When off, the image moves in whole-pixel steps, and perspective and HDR color management are disabled.
- hdrColorManagement: true
  $name: HDR color management
  $description: On monitors with Windows HDR turned on, renders through a native scRGB pipeline that follows Windows' SDR content brightness. No effect on other monitors or if Subpixel motion is off.
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
    - direction: inherit
      $name: Direction
      $options:
      - inherit: Use the global setting
      - opposite: Move opposite to the pointer
      - follow: Move with the pointer
    - perspectiveMode: inherit
      $name: Perspective
      $options:
      - inherit: Use global perspective settings
      - off: Disable perspective
      - custom: Use the following perspective settings
    - perspectiveX: 2
      $name: Horizontal perspective tilt (degrees)
      $description: Range 0 to 8. Used in custom perspective mode.
    - perspectiveY: 2
      $name: Vertical perspective tilt (degrees)
      $description: Range 0 to 8. Used in custom perspective mode.
    - perspectiveDepth: 100
      $name: Perspective intensity (%)
      $description: Range 0 to 200. Used in custom perspective mode.
    - perspectiveAutoZoom: true
      $name: Automatically compensate perspective edges
      $description: Used in custom perspective mode. Turning this off can reveal black edges.
    - image: ""
      $name: Custom image
      $description: Full path to a local image. Environment variables are expanded. Leave empty to use the Windows wallpaper.
  $name: Per-monitor rules
*/
// ==/WindhawkModSettings==
// clang-format on

#ifndef NOMINMAX
#define NOMINMAX
#endif
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
#include <dcomp.h>
#include <dwmapi.h>
#include <dxgi1_6.h>
#include <gdiplus.h>
#include <objbase.h>
#include <objidl.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <windows.h>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#ifndef WS_EX_NOREDIRECTIONBITMAP
#define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#endif

// ABI property indices and values from the Windows SDK. Some MinGW headers
// expose these effect CLSIDs but omit their property enumerations.
constexpr UINT32 kColorSourceContext = 0;
constexpr UINT32 kColorDestinationContext = 2;
constexpr UINT32 kColorQuality = 5;
constexpr UINT32 kColorQualityBest = 2;
constexpr UINT32 kTransform3DInterpolation = 0;
constexpr UINT32 kTransform3DBorderMode = 1;
constexpr UINT32 kTransform3DMatrix = 2;
constexpr UINT32 kBorderModeHard = 1;
constexpr UINT32 kInterpolationHighQualityCubic = 5;
constexpr UINT32 kColorMatrixValue = 0;
constexpr DISPLAYCONFIG_DEVICE_INFO_TYPE kGetSdrWhiteLevel =
    static_cast<DISPLAYCONFIG_DEVICE_INFO_TYPE>(11);
// Layout of DISPLAYCONFIG_SDR_WHITE_LEVEL; declared locally for older headers.
struct SdrWhiteLevelInfo {
    DISPLAYCONFIG_DEVICE_INFO_HEADER header;
    ULONG sdrWhiteLevel;  // 1000 corresponds to 80 nits.
};

constexpr wchar_t kClass[] = L"WindhawkParallaxWallpaper_Pane";
constexpr int kMaxConsecutiveFailures = 5;
constexpr double kNever = std::numeric_limits<double>::infinity();

HANDLE g_thread = nullptr, g_stop = nullptr, g_settingsEvent = nullptr;

// Only the render thread touches the state below, unless noted otherwise.
HWND g_controller = nullptr, g_parent = nullptr, g_icons = nullptr;
IDesktopWallpaper* g_wallpaper = nullptr;
ID2D1Factory1* g_d2d = nullptr;
// Shared Direct3D 11 / Direct2D / DirectComposition devices. Created lazily
// and recreated after a device loss.
ID3D11Device* g_d3dDevice = nullptr;
IDXGIDevice* g_dxgiDevice = nullptr;
ID2D1Device* g_d2dDevice = nullptr;
IDXGIFactory2* g_dxgiFactory = nullptr;
IDCompositionDevice* g_dcomp = nullptr;
double g_deviceRetryAt = 0;
int g_deviceFailures = 0, g_deviceLosses = 0;
bool g_rebuild = true, g_rebuilding = false, g_retryWindows = false;
ULONGLONG g_nextMaintenance = 0;
double g_lastTick = 0;
uint64_t g_nextPaneSerial = 1;

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
// Retry delay after the n-th consecutive failure: 2 s, 4 s, 8 s ... 60 s.
double Backoff(int failures) {
    if (failures <= 0)
        return 0;
    return std::min(60000.0, 2000.0 * double(1 << std::min(failures - 1, 5)));
}
bool SameStamp(const FILETIME& a, const FILETIME& b) {
    return CompareFileTime(&a, &b) == 0;
}

struct Options {
    int x = 14, y = 8, response = 750;
    double threshold = 0.06;  // 3% screen displacement in [-1, 1] coordinates.
    bool enabled = true, opposite = true;
    bool perspective = false, perspectiveAutoZoom = true;
    int perspectiveX = 2, perspectiveY = 2, perspectiveDepth = 100;
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
    bool hdr = false;            // Windows HDR is currently turned on.
    ULONG sdrWhiteLevel = 1000;  // SDR content brightness; 1000 = 80 nits.
};

// ---------------------------------------------------------------------------
// Background image loader. Requests and results cross threads under
// g_loadLock; everything else stays on the render thread.

struct LoadRequest {
    uint64_t serial = 0;
    std::wstring path;
    FILETIME stamp{};
    int width = 0, height = 0;
};
struct LoadResult {
    uint64_t serial = 0;
    std::wstring path;
    FILETIME stamp{};
    HBITMAP bitmap = nullptr;
    void* pixels = nullptr;
};
CRITICAL_SECTION g_loadLock;
bool g_loadLockReady = false;
std::vector<LoadRequest> g_loadQueue;
std::vector<LoadResult> g_loadResults;
HANDLE g_loaderThread = nullptr, g_loaderStop = nullptr, g_loadWake = nullptr,
       g_loadDone = nullptr;

void ApplyExifOrientation(Gdiplus::Bitmap& image) {
    UINT size = image.GetPropertyItemSize(PropertyTagOrientation);
    if (size < sizeof(Gdiplus::PropertyItem))
        return;
    std::vector<BYTE> buffer(size);
    auto item = reinterpret_cast<Gdiplus::PropertyItem*>(buffer.data());
    if (image.GetPropertyItem(PropertyTagOrientation, size, item) !=
            Gdiplus::Ok ||
        item->type != PropertyTagTypeShort || !item->value ||
        item->length < sizeof(USHORT))
        return;
    Gdiplus::RotateFlipType rotation;
    switch (*static_cast<const USHORT*>(item->value)) {
        case 2:
            rotation = Gdiplus::RotateNoneFlipX;
            break;
        case 3:
            rotation = Gdiplus::Rotate180FlipNone;
            break;
        case 4:
            rotation = Gdiplus::RotateNoneFlipY;
            break;
        case 5:
            rotation = Gdiplus::Rotate90FlipX;
            break;
        case 6:
            rotation = Gdiplus::Rotate90FlipNone;
            break;
        case 7:
            rotation = Gdiplus::Rotate270FlipX;
            break;
        case 8:
            rotation = Gdiplus::Rotate270FlipNone;
            break;
        default:
            return;
    }
    image.RotateFlip(rotation);
}

// Decodes and resizes an image into a top-down 32-bit DIB section. Runs on the
// loader thread; only GDI+ and GDI objects that are not thread-affine are used.
LoadResult DecodeImage(const LoadRequest& request) {
    LoadResult result;
    result.serial = request.serial;
    result.path = request.path;
    result.stamp = request.stamp;
    if (request.width <= 0 || request.height <= 0)
        return result;
    Gdiplus::Bitmap source(request.path.c_str());
    if (source.GetLastStatus() != Gdiplus::Ok || !source.GetWidth() ||
        !source.GetHeight())
        return result;
    ApplyExifOrientation(source);
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = request.width;
    info.bmiHeader.biHeight = -request.height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* pixels = nullptr;
    HBITMAP bitmap =
        CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &pixels, nullptr, 0);
    if (!bitmap || !pixels) {
        if (bitmap)
            DeleteObject(bitmap);
        return result;
    }
    bool ok = false;
    {
        Gdiplus::Bitmap canvas(request.width, request.height, request.width * 4,
                               PixelFormat32bppRGB, static_cast<BYTE*>(pixels));
        Gdiplus::Graphics graphics(&canvas);
        if (canvas.GetLastStatus() == Gdiplus::Ok &&
            graphics.GetLastStatus() == Gdiplus::Ok) {
            graphics.Clear(Gdiplus::Color(255, 0, 0, 0));
            graphics.SetInterpolationMode(
                Gdiplus::InterpolationModeHighQualityBicubic);
            graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
            double scale =
                std::max(double(request.width) / source.GetWidth(),
                         double(request.height) / source.GetHeight());
            float w = static_cast<float>(source.GetWidth() * scale),
                  h = static_cast<float>(source.GetHeight() * scale);
            Gdiplus::ImageAttributes attributes;
            attributes.SetWrapMode(Gdiplus::WrapModeTileFlipXY);
            ok = graphics.DrawImage(
                     &source,
                     Gdiplus::RectF((request.width - w) / 2,
                                    (request.height - h) / 2, w, h),
                     0, 0, static_cast<float>(source.GetWidth()),
                     static_cast<float>(source.GetHeight()), Gdiplus::UnitPixel,
                     &attributes) == Gdiplus::Ok;
        }
    }
    if (!ok) {
        DeleteObject(bitmap);
        return result;
    }
    GdiFlush();
    result.bitmap = bitmap;
    result.pixels = pixels;
    return result;
}

void PushLoadResult(LoadResult&& result) {
    EnterCriticalSection(&g_loadLock);
    g_loadResults.push_back(std::move(result));
    LeaveCriticalSection(&g_loadLock);
    SetEvent(g_loadDone);
}
bool PopLoadRequest(LoadRequest& request) {
    EnterCriticalSection(&g_loadLock);
    bool available = !g_loadQueue.empty();
    if (available) {
        request = std::move(g_loadQueue.front());
        g_loadQueue.erase(g_loadQueue.begin());
    }
    LeaveCriticalSection(&g_loadLock);
    return available;
}
DWORD WINAPI LoaderThread(void*) {
    HANDLE events[]{g_loaderStop, g_loadWake};
    while (WaitForMultipleObjects(2, events, FALSE, INFINITE) ==
           WAIT_OBJECT_0 + 1) {
        LoadRequest request;
        while (WaitForSingleObject(g_loaderStop, 0) == WAIT_TIMEOUT &&
               PopLoadRequest(request))
            PushLoadResult(DecodeImage(request));
    }
    return 0;
}
void RequestLoad(LoadRequest&& request) {
    if (!g_loaderThread) {
        // Synchronous fallback if the loader thread could not be created.
        PushLoadResult(DecodeImage(request));
        return;
    }
    EnterCriticalSection(&g_loadLock);
    // Only the newest request per pane matters.
    g_loadQueue.erase(std::remove_if(g_loadQueue.begin(), g_loadQueue.end(),
                                     [&](const LoadRequest& queued) {
                                         return queued.serial == request.serial;
                                     }),
                      g_loadQueue.end());
    g_loadQueue.push_back(std::move(request));
    LeaveCriticalSection(&g_loadLock);
    SetEvent(g_loadWake);
}
void StopLoader() {
    if (g_loaderThread) {
        SetEvent(g_loaderStop);
        WaitForSingleObject(g_loaderThread, INFINITE);
        CloseHandle(g_loaderThread);
        g_loaderThread = nullptr;
    }
    if (!g_loadLockReady)
        return;
    EnterCriticalSection(&g_loadLock);
    g_loadQueue.clear();
    for (auto& result : g_loadResults)
        if (result.bitmap)
            DeleteObject(result.bitmap);
    g_loadResults.clear();
    LeaveCriticalSection(&g_loadLock);
}

// ---------------------------------------------------------------------------

struct Pane {
    uint64_t serial = g_nextPaneSerial++;
    Display display;
    Options options;
    HWND window = nullptr;
    bool shown = false;
    // Decoded image, larger than the monitor by the travel on each side.
    HBITMAP bitmap = nullptr;
    void* pixels = nullptr;
    int width = 0, height = 0, cacheWidth = 0, cacheHeight = 0;
    int cropX = 0, cropY = 0;  // Whole-pixel offsets when subpixel is off.
    // GPU renderer: Direct2D into a composition swap chain, attached to the
    // window through DirectComposition.
    IDXGISwapChain1* swap = nullptr;
    IDCompositionTarget* compositionTarget = nullptr;
    IDCompositionVisual* visual = nullptr;
    ID2D1DeviceContext* context = nullptr;
    ID2D1Bitmap1* swapTarget = nullptr;
    ID2D1Bitmap1* source = nullptr;
    ID2D1Effect* perspectiveEffect = nullptr;
    ID2D1Effect* colorEffect = nullptr;
    ID2D1Effect* whiteEffect = nullptr;
    ID2D1Image* output = nullptr;  // Image drawn each frame.
    bool effectsDisabled = false;
    bool dirty = true;
    double retryRender = 0;
    int failures = 0;
    double perspectiveZoom = 1;
    // Motion state.
    double x = 0, y = 0, vx = 0, vy = 0, acceptedX = 0, acceptedY = 0;
    double submittedX = 0, submittedY = 0;
    double tiltX = 0, tiltY = 0, tiltVx = 0, tiltVy = 0;
    double submittedTiltX = 0, submittedTiltY = 0;
    bool paused = false;
    // Image bookkeeping.
    std::wstring path, pendingPath, failedPath;
    FILETIME stamp{}, pendingStamp{}, failedStamp{};
    bool loading = false;

    Pane() = default;
    Pane(const Pane&) = delete;
    Pane& operator=(const Pane&) = delete;
    void FreeRenderer() {
        bool composed = compositionTarget || visual;
        if (output)
            output->Release();
        if (whiteEffect)
            whiteEffect->Release();
        if (colorEffect)
            colorEffect->Release();
        if (perspectiveEffect)
            perspectiveEffect->Release();
        if (source)
            source->Release();
        if (context)
            context->SetTarget(nullptr);
        if (swapTarget)
            swapTarget->Release();
        if (context)
            context->Release();
        if (visual)
            visual->Release();
        if (compositionTarget)
            compositionTarget->Release();
        if (swap)
            swap->Release();
        output = nullptr;
        whiteEffect = colorEffect = perspectiveEffect = nullptr;
        source = swapTarget = nullptr;
        context = nullptr;
        visual = nullptr;
        compositionTarget = nullptr;
        swap = nullptr;
        if (composed && g_dcomp)
            g_dcomp->Commit();
    }
    void FreeImage() {
        FreeRenderer();
        if (bitmap)
            DeleteObject(bitmap);
        bitmap = nullptr;
        pixels = nullptr;
    }
    ~Pane() {
        // Release the swap chain and composition target before the window.
        FreeImage();
        if (window && IsWindow(window))
            DestroyWindow(window);
    }
};
// Panes own GPU resources and windows created on the render thread. They are
// released explicitly in RenderThread's teardown, never by the automatic
// destructor at process shutdown.
[[clang::no_destroy]] std::optional<std::vector<std::unique_ptr<Pane>>> g_panes;
std::vector<Display> g_layout;

// Hides a pane so the regular Windows wallpaper shows through until the pane
// renders again.
void HidePane(Pane& pane) {
    if (pane.shown && pane.window && IsWindow(pane.window))
        ShowWindow(pane.window, SW_HIDE);
    pane.shown = false;
    pane.dirty = true;
}
Pane* FindPane(uint64_t serial) {
    for (auto& pane : *g_panes)
        if (pane->serial == serial)
            return pane.get();
    return nullptr;
}

// ---------------------------------------------------------------------------
// Settings.

std::wstring StringSetting(const wchar_t* key) {
    const wchar_t* value = Wh_GetStringSetting(key);
    std::wstring result = value ? value : L"";
    if (value)
        Wh_FreeStringSetting(value);
    return result;
}
std::wstring NormalizePath(std::wstring path) {
    const wchar_t* blanks = L" \t\r\n";
    size_t first = path.find_first_not_of(blanks);
    if (first == std::wstring::npos)
        return L"";
    path = path.substr(first, path.find_last_not_of(blanks) - first + 1);
    if (path.size() >= 2 && path.front() == L'"' && path.back() == L'"')
        path = path.substr(1, path.size() - 2);
    if (path.find(L'%') != std::wstring::npos) {
        DWORD needed = ExpandEnvironmentStringsW(path.c_str(), nullptr, 0);
        if (needed) {
            std::wstring expanded(needed, L'\0');
            DWORD written =
                ExpandEnvironmentStringsW(path.c_str(), &expanded[0], needed);
            if (written && written <= needed) {
                expanded.resize(written - 1);
                path = std::move(expanded);
            }
        }
    }
    return path;
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
    return options;  // Elegance also covers missing or unknown values.
}
void ReadPerspective(Options& options, const std::wstring& prefix) {
    options.perspectiveX =
        std::clamp(Wh_GetIntSetting((prefix + L"perspectiveX").c_str()), 0, 8);
    options.perspectiveY =
        std::clamp(Wh_GetIntSetting((prefix + L"perspectiveY").c_str()), 0, 8);
    options.perspectiveDepth = std::clamp(
        Wh_GetIntSetting((prefix + L"perspectiveDepth").c_str()), 0, 200);
    options.perspectiveAutoZoom =
        Wh_GetIntSetting((prefix + L"perspectiveAutoZoom").c_str()) != 0;
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
    g_defaults.perspective = Wh_GetIntSetting(L"perspective") != 0;
    ReadPerspective(g_defaults, L"");
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
        // Direction and perspective inherit the global settings independently
        // of the motion preset.
        std::wstring direction = StringSetting((prefix + L"direction").c_str());
        rule.options.opposite = direction == L"opposite" ? true
                                : direction == L"follow" ? false
                                                         : g_defaults.opposite;
        rule.options.perspective = g_defaults.perspective;
        rule.options.perspectiveX = g_defaults.perspectiveX;
        rule.options.perspectiveY = g_defaults.perspectiveY;
        rule.options.perspectiveDepth = g_defaults.perspectiveDepth;
        rule.options.perspectiveAutoZoom = g_defaults.perspectiveAutoZoom;
        std::wstring perspectiveMode =
            StringSetting((prefix + L"perspectiveMode").c_str());
        if (perspectiveMode == L"off") {
            rule.options.perspective = false;
        } else if (perspectiveMode == L"custom") {
            rule.options.perspective = true;
            ReadPerspective(rule.options, prefix);
        }
        rule.options.image =
            NormalizePath(StringSetting((prefix + L"image").c_str()));
        g_rules.push_back(std::move(rule));
    }
}

bool UsesPerspective(const Pane& pane) {
    return g_smoothRendering && pane.options.perspective &&
           (pane.options.perspectiveX || pane.options.perspectiveY);
}
bool UsesHdr(const Pane& pane) {
    return g_smoothRendering && g_hdrColorManagement && pane.display.hdr;
}
bool WantsEffects(const Pane& pane) {
    return UsesHdr(pane) || UsesPerspective(pane);
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

// ---------------------------------------------------------------------------
// Display enumeration.

// Reads the HDR state and SDR content brightness of the monitor identified by
// its GDI device name (e.g. "\\.\DISPLAY1"). Windows' automatic color
// management on SDR monitors also reports advanced color as enabled; those
// report wideColorEnforced and are treated as SDR.
void ReadDisplayColor(Display& display) {
    UINT32 pathCount = 0, modeCount = 0;
    if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount,
                                    &modeCount) != ERROR_SUCCESS ||
        !pathCount)
        return;
    std::vector<DISPLAYCONFIG_PATH_INFO> paths(pathCount);
    std::vector<DISPLAYCONFIG_MODE_INFO> modes(modeCount);
    if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths.data(),
                           &modeCount, modes.data(), nullptr) != ERROR_SUCCESS)
        return;
    paths.resize(pathCount);
    for (const auto& path : paths) {
        DISPLAYCONFIG_SOURCE_DEVICE_NAME source{};
        source.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
        source.header.size = sizeof(source);
        source.header.adapterId = path.sourceInfo.adapterId;
        source.header.id = path.sourceInfo.id;
        if (DisplayConfigGetDeviceInfo(&source.header) != ERROR_SUCCESS ||
            _wcsicmp(source.viewGdiDeviceName, display.name.c_str()))
            continue;
        DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO color{};
        color.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_ADVANCED_COLOR_INFO;
        color.header.size = sizeof(color);
        color.header.adapterId = path.targetInfo.adapterId;
        color.header.id = path.targetInfo.id;
        display.hdr =
            DisplayConfigGetDeviceInfo(&color.header) == ERROR_SUCCESS &&
            color.advancedColorEnabled && !color.wideColorEnforced;
        if (display.hdr) {
            SdrWhiteLevelInfo white{};
            white.header.type = kGetSdrWhiteLevel;
            white.header.size = sizeof(white);
            white.header.adapterId = path.targetInfo.adapterId;
            white.header.id = path.targetInfo.id;
            if (DisplayConfigGetDeviceInfo(&white.header) == ERROR_SUCCESS &&
                white.sdrWhiteLevel >= 1000)
                display.sdrWhiteLevel = std::min<ULONG>(
                    white.sdrWhiteLevel, 1000 * 10000 / 80);  // 10000 nits.
        }
        return;
    }
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
    ReadDisplayColor(d);
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
// The SDR brightness is deliberately excluded: it is applied in place without
// rebuilding the panes.
bool SameLayout(const std::vector<Display>& a, const std::vector<Display>& b) {
    if (a.size() != b.size())
        return false;
    for (size_t i = 0; i < a.size(); ++i)
        if (a[i].name != b[i].name || a[i].id != b[i].id ||
            a[i].hdr != b[i].hdr || !EqualRect(&a[i].rect, &b[i].rect))
            return false;
    return true;
}

// ---------------------------------------------------------------------------
// Desktop attachment.

// The process that owns the desktop (Progman). The mod runs in its own
// process, so windows are matched against the shell's process instead.
DWORD ShellProcessId() {
    DWORD process = 0;
    if (HWND shell = GetShellWindow())
        GetWindowThreadProcessId(shell, &process);
    return process;
}
struct IconHostSearch {
    DWORD shellProcess;
    HWND result;
};
BOOL CALLBACK FindIconHost(HWND window, LPARAM data) {
    auto search = reinterpret_cast<IconHostSearch*>(data);
    DWORD process = 0;
    GetWindowThreadProcessId(window, &process);
    if (process == search->shellProcess && IsWindowVisible(window) &&
        FindWindowExW(window, nullptr, L"SHELLDLL_DefView", nullptr)) {
        search->result = window;
        return FALSE;
    }
    return TRUE;
}
HWND FindWallpaperHost(HWND& iconView) {
    iconView = nullptr;
    DWORD shellProcess = ShellProcessId();
    if (!shellProcess)
        return nullptr;
    HWND progman = FindWindowW(L"Progman", nullptr);
    DWORD owner = 0;
    if (progman) {
        GetWindowThreadProcessId(progman, &owner);
        HWND view =
            FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);
        HWND wallpaper = FindWindowExW(progman, nullptr, L"WorkerW", nullptr);
        if (owner == shellProcess && view && wallpaper &&
            (GetWindowLongPtrW(progman, GWL_EXSTYLE) &
             WS_EX_NOREDIRECTIONBITMAP) &&
            (GetWindowLongPtrW(view, GWL_EXSTYLE) & WS_EX_LAYERED)) {
            iconView = view;
            return progman;
        }
    }
    IconHostSearch search{shellProcess, nullptr};
    EnumWindows(FindIconHost, reinterpret_cast<LPARAM>(&search));
    if (!search.result)
        return nullptr;
    HWND host = FindWindowExW(nullptr, search.result, L"WorkerW", nullptr);
    owner = 0;
    if (host)
        GetWindowThreadProcessId(host, &owner);
    if (!host || !IsWindowVisible(host) || owner != shellProcess ||
        FindWindowExW(host, nullptr, L"SHELLDLL_DefView", nullptr))
        return nullptr;
    return host;
}
bool Position(Pane& pane, UINT flags) {
    if (!pane.window || !IsWindow(pane.window))
        return false;
    if (!IsWindow(g_parent) || (g_icons && !IsWindow(g_icons)))
        return false;
    POINT origin{pane.display.rect.left, pane.display.rect.top};
    MapWindowPoints(nullptr, g_parent, &origin, 1);
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

// ---------------------------------------------------------------------------
// Images.

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
    wchar_t fallback[MAX_PATH * 4]{};
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

void RefreshImage(Pane& pane) {
    if (!pane.window || !IsWindow(pane.window))
        return;
    std::wstring path;
    if (!WallpaperPath(pane, path))
        return;
    if (!pane.window || !IsWindow(pane.window))
        return;  // COM can dispatch window messages.
    if (path.empty()) {
        HidePane(pane);
        pane.FreeImage();
        pane.path.clear();
        pane.stamp = {};
        pane.loading = false;
        pane.failedPath.clear();
        return;
    }
    FILETIME stamp = FileStamp(path);
    if (pane.pixels && path == pane.path && SameStamp(stamp, pane.stamp))
        return;
    if (pane.loading && path == pane.pendingPath &&
        SameStamp(stamp, pane.pendingStamp))
        return;
    // A file that failed to load is retried only after it changes.
    if (path == pane.failedPath && SameStamp(stamp, pane.failedStamp))
        return;
    pane.loading = true;
    pane.pendingPath = path;
    pane.pendingStamp = stamp;
    LoadRequest request;
    request.serial = pane.serial;
    request.path = path;
    request.stamp = stamp;
    request.width = pane.cacheWidth;
    request.height = pane.cacheHeight;
    RequestLoad(std::move(request));
}
bool InstallImage(Pane& pane, LoadResult& result) {
    pane.FreeImage();
    pane.bitmap = result.bitmap;
    pane.pixels = result.pixels;
    result.bitmap = nullptr;
    pane.path = result.path;
    pane.stamp = result.stamp;
    pane.failedPath.clear();
    pane.x = pane.y = pane.vx = pane.vy = 0;
    pane.acceptedX = pane.acceptedY = 0;
    pane.submittedX = pane.submittedY = 0;
    pane.tiltX = pane.tiltY = pane.tiltVx = pane.tiltVy = 0;
    pane.submittedTiltX = pane.submittedTiltY = 0;
    pane.cropX = pane.options.x;
    pane.cropY = pane.options.y;
    pane.retryRender = 0;
    pane.failures = 0;
    pane.dirty = true;  // Rendered and shown on the next frame.
    return true;
}
void ProcessLoadResults() {
    std::vector<LoadResult> results;
    EnterCriticalSection(&g_loadLock);
    results.swap(g_loadResults);
    LeaveCriticalSection(&g_loadLock);
    for (auto& result : results) {
        Pane* pane = FindPane(result.serial);
        // Discard results for destroyed panes and superseded requests.
        if (pane && pane->loading && result.path == pane->pendingPath &&
            SameStamp(result.stamp, pane->pendingStamp) && pane->window &&
            IsWindow(pane->window)) {
            pane->loading = false;
            if (!result.bitmap || !InstallImage(*pane, result)) {
                pane->failedPath = result.path;
                pane->failedStamp = result.stamp;
                Wh_Log(
                    L"Unable to load the image for %s (unreadable or "
                    L"unsupported format): %s",
                    pane->display.name.c_str(), result.path.c_str());
            }
        }
        if (result.bitmap)
            DeleteObject(result.bitmap);
    }
}

// ---------------------------------------------------------------------------
// Perspective geometry.

struct TiltCoefficients {
    double a, b, c, d, p, q;
};
TiltCoefficients Tilt(const Pane& pane, double tiltX, double tiltY) {
    constexpr double radians = 3.14159265358979323846 / 180.0;
    double ry = tiltX * pane.options.perspectiveX * radians;
    double rx = -tiltY * pane.options.perspectiveY * radians;
    double inverseDistance = (pane.options.perspectiveDepth / 100.0) /
                             (2.0 * std::max(pane.width, pane.height));
    return {std::cos(ry),
            std::sin(rx) * std::sin(ry),
            0,
            std::cos(rx),
            -std::sin(ry) * inverseDistance,
            std::sin(rx) * std::cos(ry) * inverseDistance};
}
// Inverse-projects the viewport corners into the source crop. Positive
// homogeneous W and a convex quadrilateral make corner containment sufficient
// for the whole viewport. The travel margin is ignored, which keeps the test
// valid for any translation. A one-pixel guard avoids sampling the border.
bool CoversViewport(const Pane& pane, const TiltCoefficients& t, double zoom) {
    double cx = pane.width * 0.5, cy = pane.height * 0.5;
    double limitX = cx - std::min(1.0, cx * 0.01);
    double limitY = cy - std::min(1.0, cy * 0.01);
    for (int ix = -1; ix <= 1; ix += 2) {
        for (int iy = -1; iy <= 1; iy += 2) {
            double u = ix * cx / zoom, v = iy * cy / zoom;
            double aa = t.a - u * t.p, bb = t.b - u * t.q;
            double cc = t.c - v * t.p, dd = t.d - v * t.q;
            double determinant = aa * dd - bb * cc;
            if (std::abs(determinant) < 1e-9)
                return false;
            double sx = (u * dd - bb * v) / determinant;
            double sy = (aa * v - u * cc) / determinant;
            if (1 + t.p * sx + t.q * sy <= 0 || std::abs(sx) > limitX ||
                std::abs(sy) > limitY)
                return false;
        }
    }
    return true;
}
// Constant zoom that covers the viewport over the whole tilt range, so the
// image does not visibly zoom in and out while the tilt changes.
double PerspectiveZoom(const Pane& pane) {
    if (!UsesPerspective(pane) || !pane.options.perspectiveAutoZoom ||
        pane.width <= 0 || pane.height <= 0)
        return 1;
    double zoom = 1;
    for (int i = -4; i <= 4; ++i) {
        for (int j = -4; j <= 4; ++j) {
            TiltCoefficients t = Tilt(pane, i / 4.0, j / 4.0);
            if (CoversViewport(pane, t, zoom))
                continue;
            double low = zoom, high = 8;
            for (int k = 0; k < 24; ++k) {
                double mid = (low + high) * 0.5;
                if (CoversViewport(pane, t, mid))
                    high = mid;
                else
                    low = mid;
            }
            zoom = high;
        }
    }
    return zoom;
}
D2D1_MATRIX_4X4_F PerspectiveMatrix(const Pane& pane) {
    TiltCoefficients t = Tilt(pane, pane.tiltX, pane.tiltY);
    double zoom = pane.perspectiveZoom;
    double cx = pane.width * 0.5, cy = pane.height * 0.5;
    double sourceX = cx + pane.options.x + pane.x;
    double sourceY = cy + pane.options.y + pane.y;
    D2D1_MATRIX_4X4_F m{};
    m._11 = float(zoom * t.a + cx * t.p);
    m._12 = float(zoom * t.c + cy * t.p);
    m._14 = float(t.p);
    m._21 = float(zoom * t.b + cx * t.q);
    m._22 = float(zoom * t.d + cy * t.q);
    m._24 = float(t.q);
    m._33 = 1;
    m._41 = float(cx - sourceX * (zoom * t.a + cx * t.p) -
                  sourceY * (zoom * t.b + cx * t.q));
    m._42 = float(cy - sourceX * (zoom * t.c + cy * t.p) -
                  sourceY * (zoom * t.d + cy * t.q));
    m._44 = float(1 - sourceX * t.p - sourceY * t.q);
    return m;
}

// ---------------------------------------------------------------------------
// Rendering: Direct3D 11 + Direct2D into composition swap chains, attached to
// the pane windows with DirectComposition. This works on the classic WorkerW
// layout and on the layered Windows 11 layout, across processes.

template <typename T>
HRESULT SetProperty(ID2D1Effect* effect, UINT32 index, const T& value) {
    return effect->SetValue(index, D2D1_PROPERTY_TYPE_UNKNOWN,
                            reinterpret_cast<const BYTE*>(&value),
                            sizeof(value));
}
bool IsDeviceLost(HRESULT hr) {
    return hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET ||
           hr == DXGI_ERROR_DEVICE_HUNG ||
           hr == static_cast<HRESULT>(D2DERR_RECREATE_TARGET);
}
bool UsesEffects(const Pane& pane) {
    return WantsEffects(pane) && !pane.effectsDisabled;
}

void ReleaseDevice() {
    if (g_panes)
        for (auto& pane : *g_panes)
            pane->FreeRenderer();
    if (g_dcomp)
        g_dcomp->Release();
    if (g_dxgiFactory)
        g_dxgiFactory->Release();
    if (g_d2dDevice)
        g_d2dDevice->Release();
    if (g_dxgiDevice)
        g_dxgiDevice->Release();
    if (g_d3dDevice)
        g_d3dDevice->Release();
    g_dcomp = nullptr;
    g_dxgiFactory = nullptr;
    g_d2dDevice = nullptr;
    g_dxgiDevice = nullptr;
    g_d3dDevice = nullptr;
}
// Driver updates, TDRs and remote desktop sessions remove the device. Every
// GPU resource belongs to it, so all of them are recreated.
void HandleDeviceLoss(HRESULT hr) {
    ++g_deviceLosses;
    double delay = Backoff(g_deviceLosses) / 4;  // 0.5 s, 1 s, 2 s ... 15 s.
    if (g_deviceLosses <= kMaxConsecutiveFailures)
        Wh_Log(L"GPU device lost (0x%08lX); recreating the renderer in %d ms",
               static_cast<unsigned long>(hr), static_cast<int>(delay));
    if (g_panes)
        for (auto& pane : *g_panes)
            HidePane(*pane);
    ReleaseDevice();
    g_deviceRetryAt = ClockMs() + delay;
}
bool EnsureDevice() {
    if (g_dcomp) {
        HRESULT reason = g_d3dDevice->GetDeviceRemovedReason();
        if (SUCCEEDED(reason))
            return true;
        HandleDeviceLoss(reason);
    }
    if (!g_d2d || ClockMs() < g_deviceRetryAt)
        return false;
    HRESULT hr = E_FAIL;
    // WARP keeps the mod working without a usable GPU driver.
    for (D3D_DRIVER_TYPE type :
         {D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP}) {
        D3D_FEATURE_LEVEL level{};
        hr = D3D11CreateDevice(
            nullptr, type, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr,
            0, D3D11_SDK_VERSION, &g_d3dDevice, &level, nullptr);
        if (SUCCEEDED(hr)) {
            if (type == D3D_DRIVER_TYPE_WARP)
                Wh_Log(L"Hardware Direct3D unavailable; using WARP");
            break;
        }
    }
    if (SUCCEEDED(hr))
        hr = g_d3dDevice->QueryInterface(IID_PPV_ARGS(&g_dxgiDevice));
    if (SUCCEEDED(hr))
        hr = g_d2d->CreateDevice(g_dxgiDevice, &g_d2dDevice);
    if (SUCCEEDED(hr)) {
        IDXGIAdapter* adapter = nullptr;
        hr = g_dxgiDevice->GetAdapter(&adapter);
        if (SUCCEEDED(hr)) {
            hr = adapter->GetParent(IID_PPV_ARGS(&g_dxgiFactory));
            adapter->Release();
        }
    }
    if (SUCCEEDED(hr))
        hr = DCompositionCreateDevice(g_dxgiDevice, IID_PPV_ARGS(&g_dcomp));
    if (SUCCEEDED(hr)) {
        g_deviceFailures = 0;
        return true;
    }
    ReleaseDevice();
    ++g_deviceFailures;
    double delay = Backoff(g_deviceFailures);
    g_deviceRetryAt = ClockMs() + delay;
    if (g_deviceFailures <= kMaxConsecutiveFailures)
        Wh_Log(
            L"Renderer unavailable (0x%08lX); Windows wallpaper shown, "
            L"retry in %d ms%s",
            static_cast<unsigned long>(hr), static_cast<int>(delay),
            g_deviceFailures == kMaxConsecutiveFailures
                ? L" (further failures are not logged)"
                : L"");
    return false;
}

HRESULT ApplyWhiteLevel(Pane& pane) {
    if (!pane.whiteEffect)
        return S_OK;
    // Linear scRGB: 1.0 is 80 nits, so scale to Windows' SDR content
    // brightness. The matrix is 5x4, row-major: R, G, B, A, offset.
    float scale = pane.display.sdrWhiteLevel / 1000.0f;
    float matrix[20]{};
    matrix[0] = matrix[5] = matrix[10] = scale;
    matrix[15] = 1;
    return SetProperty(pane.whiteEffect, kColorMatrixValue, matrix);
}
HRESULT SetSwapChainColorSpace(IDXGISwapChain1* swap, bool hdr) {
    IDXGISwapChain3* swap3 = nullptr;
    HRESULT hr = swap->QueryInterface(IID_PPV_ARGS(&swap3));
    if (FAILED(hr))
        return hdr ? hr : S_OK;  // SDR already uses the default color space.
    const DXGI_COLOR_SPACE_TYPE space =
        hdr ? DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709
            : DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709;
    UINT support = 0;
    hr = swap3->CheckColorSpaceSupport(space, &support);
    if (SUCCEEDED(hr) &&
        !(support & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT))
        hr = DXGI_ERROR_UNSUPPORTED;
    if (SUCCEEDED(hr))
        hr = swap3->SetColorSpace1(space);
    swap3->Release();
    return hr;
}
// Builds source -> [3D transform] -> [color management -> white level] ->
// swap chain. The geometric transform works on 8-bit sRGB data; the
// conversion to linear scRGB happens last, straight into the FP16 target.
// Without effects, the source bitmap is drawn directly.
HRESULT CreateRenderer(Pane& pane) {
    const bool effects = UsesEffects(pane);
    const bool hdr = effects && UsesHdr(pane);
    const bool perspective = effects && UsesPerspective(pane);
    const DXGI_FORMAT format =
        hdr ? DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_B8G8R8A8_UNORM;
    HRESULT hr = g_d2dDevice->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &pane.context);
    if (SUCCEEDED(hr)) {
        D2D1_RENDERING_CONTROLS controls{};
        pane.context->GetRenderingControls(&controls);
        if (hdr && pane.context->IsBufferPrecisionSupported(
                       D2D1_BUFFER_PRECISION_16BPC_FLOAT))
            controls.bufferPrecision = D2D1_BUFFER_PRECISION_16BPC_FLOAT;
        // Render the effect graph in a single tile where the GPU allows it.
        // With the default tiles, the resampling 3D transform shows faint
        // seams along tile edges that crawl across the image during motion.
        UINT32 limit = pane.context->GetMaximumBitmapSize();
        controls.tileSize.width = std::min<UINT32>(
            limit, static_cast<UINT32>(std::max(pane.cacheWidth, pane.width)));
        controls.tileSize.height = std::min<UINT32>(
            limit,
            static_cast<UINT32>(std::max(pane.cacheHeight, pane.height)));
        pane.context->SetRenderingControls(&controls);
    }
    if (SUCCEEDED(hr)) {
        DXGI_SWAP_CHAIN_DESC1 desc{};
        desc.Width = static_cast<UINT>(pane.width);
        desc.Height = static_cast<UINT>(pane.height);
        desc.Format = format;
        desc.SampleDesc.Count = 1;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferCount = 2;
        desc.Scaling = DXGI_SCALING_STRETCH;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
        hr = g_dxgiFactory->CreateSwapChainForComposition(g_d3dDevice, &desc,
                                                          nullptr, &pane.swap);
    }
    if (SUCCEEDED(hr))
        hr = SetSwapChainColorSpace(pane.swap, hdr);
    if (SUCCEEDED(hr)) {
        IDXGISurface* surface = nullptr;
        hr = pane.swap->GetBuffer(0, IID_PPV_ARGS(&surface));
        if (SUCCEEDED(hr)) {
            auto properties = D2D1::BitmapProperties1(
                D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                D2D1::PixelFormat(format, D2D1_ALPHA_MODE_IGNORE));
            hr = pane.context->CreateBitmapFromDxgiSurface(surface, properties,
                                                           &pane.swapTarget);
            surface->Release();
        }
    }
    if (SUCCEEDED(hr)) {
        pane.context->SetTarget(pane.swapTarget);
        auto properties = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_NONE,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_IGNORE));
        hr = pane.context->CreateBitmap(
            D2D1::SizeU(pane.cacheWidth, pane.cacheHeight), pane.pixels,
            pane.cacheWidth * 4, properties, &pane.source);
    }
    ID2D1Effect* last = nullptr;  // Not owned.
    if (SUCCEEDED(hr) && perspective) {
        hr = pane.context->CreateEffect(CLSID_D2D13DTransform,
                                        &pane.perspectiveEffect);
        if (SUCCEEDED(hr)) {
            pane.perspectiveEffect->SetInput(0, pane.source);
            hr = SetProperty(pane.perspectiveEffect, kTransform3DBorderMode,
                             kBorderModeHard);
            // Bilinear sampling at a scale close to 1 produces a periodic
            // sharp/soft pattern (visible as moving vertical and horizontal
            // lines). High-quality cubic sampling suppresses it.
            if (SUCCEEDED(hr))
                hr = SetProperty(pane.perspectiveEffect,
                                 kTransform3DInterpolation,
                                 kInterpolationHighQualityCubic);
            last = pane.perspectiveEffect;
        }
    }
    if (SUCCEEDED(hr) && hdr) {
        ID2D1ColorContext *srgb = nullptr, *scrgb = nullptr;
        hr = pane.context->CreateEffect(CLSID_D2D1ColorManagement,
                                        &pane.colorEffect);
        if (SUCCEEDED(hr))
            hr = pane.context->CreateColorContext(D2D1_COLOR_SPACE_SRGB,
                                                  nullptr, 0, &srgb);
        if (SUCCEEDED(hr))
            hr = pane.context->CreateColorContext(D2D1_COLOR_SPACE_SCRGB,
                                                  nullptr, 0, &scrgb);
        if (SUCCEEDED(hr))
            hr = SetProperty(pane.colorEffect, kColorSourceContext, srgb);
        if (SUCCEEDED(hr))
            hr = SetProperty(pane.colorEffect, kColorDestinationContext, scrgb);
        if (SUCCEEDED(hr))
            hr =
                SetProperty(pane.colorEffect, kColorQuality, kColorQualityBest);
        if (srgb)
            srgb->Release();
        if (scrgb)
            scrgb->Release();
        if (SUCCEEDED(hr)) {
            if (last)
                pane.colorEffect->SetInputEffect(0, last);
            else
                pane.colorEffect->SetInput(0, pane.source);
            hr = pane.context->CreateEffect(CLSID_D2D1ColorMatrix,
                                            &pane.whiteEffect);
        }
        if (SUCCEEDED(hr)) {
            pane.whiteEffect->SetInputEffect(0, pane.colorEffect);
            hr = ApplyWhiteLevel(pane);
            last = pane.whiteEffect;
        }
    }
    if (SUCCEEDED(hr)) {
        if (last) {
            last->GetOutput(&pane.output);
        } else {
            pane.output = pane.source;
            pane.output->AddRef();
        }
    }
    if (SUCCEEDED(hr))
        hr = g_dcomp->CreateTargetForHwnd(pane.window, TRUE,
                                          &pane.compositionTarget);
    if (SUCCEEDED(hr))
        hr = g_dcomp->CreateVisual(&pane.visual);
    if (SUCCEEDED(hr))
        hr = pane.visual->SetContent(pane.swap);
    if (SUCCEEDED(hr))
        hr = pane.compositionTarget->SetRoot(pane.visual);
    if (SUCCEEDED(hr))
        hr = g_dcomp->Commit();
    return hr;
}
void RenderFailed(Pane& pane, HRESULT hr, const wchar_t* stage) {
    pane.FreeRenderer();
    HidePane(pane);
    if (IsDeviceLost(hr)) {
        HandleDeviceLoss(hr);
        return;
    }
    ++pane.failures;
    if (UsesEffects(pane) && pane.failures >= 3) {
        pane.effectsDisabled = true;
        pane.failures = 0;
        pane.retryRender = 0;
        Wh_Log(
            L"Perspective/HDR %s failed for %s (0x%08lX); using standard "
            L"rendering on this monitor until the layout or settings change",
            stage, pane.display.name.c_str(), static_cast<unsigned long>(hr));
        return;
    }
    if (pane.failures >= kMaxConsecutiveFailures) {
        pane.retryRender = kNever;
        Wh_Log(
            L"Rendering %s failed for %s (0x%08lX); the Windows wallpaper "
            L"stays visible on this monitor until the layout or settings "
            L"change",
            stage, pane.display.name.c_str(), static_cast<unsigned long>(hr));
        return;
    }
    double delay = Backoff(pane.failures);
    pane.retryRender = ClockMs() + delay;
    Wh_Log(L"Rendering %s failed for %s (0x%08lX); retry in %d ms", stage,
           pane.display.name.c_str(), static_cast<unsigned long>(hr),
           static_cast<int>(delay));
}
bool Render(Pane& pane) {
    if (!pane.pixels || !pane.window || ClockMs() < pane.retryRender ||
        !EnsureDevice())
        return false;
    HRESULT hr = S_OK;
    if (!pane.context) {
        hr = CreateRenderer(pane);
        if (FAILED(hr)) {
            RenderFailed(pane, hr, L"setup");
            return false;
        }
    }
    pane.context->BeginDraw();
    pane.context->Clear(D2D1::ColorF(0, 0, 0, 1));
    if (pane.perspectiveEffect) {
        pane.context->SetTransform(D2D1::Matrix3x2F::Identity());
        hr = SetProperty(pane.perspectiveEffect, kTransform3DMatrix,
                         PerspectiveMatrix(pane));
    } else {
        double x = g_smoothRendering ? pane.options.x + pane.x : pane.cropX;
        double y = g_smoothRendering ? pane.options.y + pane.y : pane.cropY;
        pane.context->SetTransform(D2D1::Matrix3x2F::Translation(
            static_cast<float>(-x), static_cast<float>(-y)));
    }
    if (SUCCEEDED(hr))
        pane.context->DrawImage(pane.output, nullptr, nullptr,
                                D2D1_INTERPOLATION_MODE_LINEAR,
                                D2D1_COMPOSITE_MODE_SOURCE_OVER);
    HRESULT endHr = pane.context->EndDraw();
    if (SUCCEEDED(hr))
        hr = endHr;
    if (SUCCEEDED(hr)) {
        DXGI_PRESENT_PARAMETERS present{};
        hr = pane.swap->Present1(0, 0, &present);
    }
    if (FAILED(hr)) {
        RenderFailed(pane, hr, L"drawing");
        return false;
    }
    pane.failures = 0;
    g_deviceLosses = 0;
    pane.dirty = false;
    if (!pane.shown)
        pane.shown = Position(pane, SWP_SHOWWINDOW);
    return true;
}

// ---------------------------------------------------------------------------
// Fullscreen pause and motion.

bool Covers(const RECT& window, const RECT& monitor) {
    return window.left <= monitor.left && window.top <= monitor.top &&
           window.right >= monitor.right && window.bottom >= monitor.bottom;
}
void UpdatePause() {
    RECT rect{};
    bool full = false;
    HWND foreground = GetForegroundWindow();
    wchar_t cls[128]{};
    if (foreground)
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
    for (auto& p : *g_panes) {
        bool pause = full && Covers(rect, p->display.rect);
        if (pause && !p->paused) {
            p->vx = p->vy = 0;
            p->tiltVx = p->tiltVy = 0;
        }
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
    // Avoid a jump after a long stall or suspend; normal frame-rate changes
    // remain time-based.
    double dt = std::clamp(elapsed, 0.0, 50.0) / 1000.0;
    SpringAxis(pane.x, pane.vx, sign * pane.acceptedX * pane.options.x, omega,
               dt, pane.options.x);
    SpringAxis(pane.y, pane.vy, sign * pane.acceptedY * pane.options.y, omega,
               dt, pane.options.y);
    bool perspective = UsesPerspective(pane);
    if (perspective) {
        // Normalized axes also allow perspective with zero travel.
        SpringAxis(pane.tiltX, pane.tiltVx, sign * pane.acceptedX, omega, dt,
                   1);
        SpringAxis(pane.tiltY, pane.tiltVy, sign * pane.acceptedY, omega, dt,
                   1);
    }
    int x = std::clamp(pane.options.x + int(std::lround(pane.x)), 0,
                       2 * pane.options.x);
    int y = std::clamp(pane.options.y + int(std::lround(pane.y)), 0,
                       2 * pane.options.y);
    bool changed = g_smoothRendering
                       ? std::abs(pane.x - pane.submittedX) >= 0.005 ||
                             std::abs(pane.y - pane.submittedY) >= 0.005
                       : x != pane.cropX || y != pane.cropY;
    changed =
        changed || (perspective &&
                    (std::abs(pane.tiltX - pane.submittedTiltX) >= 0.00001 ||
                     std::abs(pane.tiltY - pane.submittedTiltY) >= 0.00001));
    pane.cropX = x;
    pane.cropY = y;
    if (changed) {
        pane.submittedX = pane.x;
        pane.submittedY = pane.y;
        pane.submittedTiltX = pane.tiltX;
        pane.submittedTiltY = pane.tiltY;
    }
    return changed;
}
bool ShouldAnimate(const Pane& pane, const Display& active) {
    return !pane.paused && (!g_independent || pane.display.name == active.name);
}

enum class Activity { Moving, Idle, Dormant };
Activity Tick() {
    double now = ClockMs();
    double elapsed = now - g_lastTick;
    g_lastTick = now;
    POINT cursor{};
    bool haveCursor = GetCursorPos(&cursor) != FALSE;  // Fails on the secure
                                                       // desktop.
    // Use all displays, including ones where the animation is disabled.
    const Display* active = nullptr;
    if (haveCursor)
        for (const auto& d : g_layout)
            if (PtInRect(&d.rect, cursor)) {
                active = &d;
                break;
            }
    double nx = 0, ny = 0;
    if (active)
        Normalized(active->rect, cursor, nx, ny);
    bool moving = false, animatable = false;
    for (auto& p : *g_panes) {
        if (!p->pixels || !p->window)
            continue;
        if (!p->paused && haveCursor) {
            animatable = true;
            if (active && ShouldAnimate(*p, *active))
                AcceptInput(*p, nx, ny);
            if (Advance(*p, elapsed)) {
                moving = true;
                p->dirty = true;
            }
            moving =
                moving || std::abs(p->vx) > 0.005 || std::abs(p->vy) > 0.005 ||
                (UsesPerspective(*p) &&
                 (std::abs(p->tiltVx) > 0.005 || std::abs(p->tiltVy) > 0.005));
        }
        // New images, retries and device recovery also go through here.
        if (p->dirty && now >= p->retryRender)
            Render(*p);
    }
    return moving       ? Activity::Moving
           : animatable ? Activity::Idle
                        : Activity::Dormant;
}

// ---------------------------------------------------------------------------
// Windows and layout.

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
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_PAINT: {
            // Content is presented through DirectComposition.
            PAINTSTRUCT paint{};
            BeginPaint(window, &paint);
            EndPaint(window, &paint);
            return 0;
        }
        case WM_TIMER:
            // Timers belong only to the hidden controller, which survives
            // monitor rebuilds.
            if (window == g_controller && !g_rebuilding && wp == 2)
                UpdatePause();
            return 0;
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
            g_rebuild = true;
            return 0;
        case WM_NCDESTROY:
            if (pane) {
                pane->window = nullptr;
                pane->shown = false;
                if (!g_rebuilding)
                    g_rebuild = true;
            }
            SetWindowLongPtrW(window, GWLP_USERDATA, 0);
            break;
    }
    return DefWindowProcW(window, msg, wp, lp);
}

void CreateWallpaperInterface() {
    if (g_wallpaper) {
        g_wallpaper->Release();
        g_wallpaper = nullptr;
    }
    if (FAILED(CoCreateInstance(CLSID_DesktopWallpaper, nullptr, CLSCTX_ALL,
                                IID_PPV_ARGS(&g_wallpaper))))
        g_wallpaper = nullptr;
    if (!g_wallpaper)
        Wh_Log(L"IDesktopWallpaper unavailable; using the common wallpaper");
}

void Rebuild(const std::vector<Display>& layout, HWND parent, HWND icons) {
    g_rebuilding = true;
    g_rebuild = false;
    g_retryWindows = false;
    g_panes->clear();
    // After an Explorer restart, reconnect to the new shell.
    if (parent != g_parent && parent)
        CreateWallpaperInterface();
    g_layout = layout;
    g_parent = parent;
    g_icons = icons;
    // A new layout or new settings deserve a fresh attempt at the GPU path.
    g_deviceRetryAt = 0;
    g_deviceFailures = 0;
    g_deviceLosses = 0;
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
        Wh_Log(L"Desktop layout: %s",
               g_icons ? L"layered (Progman)" : L"classic (WorkerW)");
        for (const auto& d : layout) {
            if (WaitForSingleObject(g_stop, 0) != WAIT_TIMEOUT)
                break;
            Wh_Log(
                L"Monitor %s, rect=(%ld,%ld,%ld,%ld), id=%s, hdr=%d, "
                L"sdrWhite=%lu nits",
                d.name.c_str(), d.rect.left, d.rect.top, d.rect.right,
                d.rect.bottom, d.id.c_str(), d.hdr ? 1 : 0,
                static_cast<unsigned long>(d.sdrWhiteLevel * 80 / 1000));
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
            p->cacheWidth = p->width + 2 * options.x;
            p->cacheHeight = p->height + 2 * options.y;
            p->cropX = options.x;
            p->cropY = options.y;
            p->perspectiveZoom = PerspectiveZoom(*p);
            // The pane is a child of Explorer's wallpaper host, created by
            // this process. It has no redirection surface: content comes only
            // from DirectComposition. WS_EX_NOPARENTNOTIFY avoids synchronous
            // notifications to Explorer when panes are created or destroyed.
            p->window = CreateWindowExW(
                WS_EX_NOREDIRECTIONBITMAP | WS_EX_NOACTIVATE |
                    WS_EX_TOOLWINDOW | WS_EX_NOPARENTNOTIFY | WS_EX_TRANSPARENT,
                kClass, L"", WS_CHILD | WS_CLIPSIBLINGS, 0, 0, 0, 0, g_parent,
                nullptr, GetModuleHandleW(nullptr), p.get());
            if (!p->window) {
                Wh_Log(L"Window setup failed for %s, error=%lu", d.name.c_str(),
                       GetLastError());
                g_retryWindows = true;
                continue;
            }
            Position(*p, 0);  // Shown after the first successful frame.
            Pane* pane = p.get();
            g_panes->push_back(std::move(p));
            RefreshImage(*pane);
        }
    } else {
        Wh_Log(L"Waiting for a supported desktop layout");
    }
    g_lastTick = ClockMs();
    g_rebuilding = false;
    UpdatePause();
}
void Maintenance() {
    auto layout = Displays();
    HWND icons = nullptr;
    HWND parent = FindWallpaperHost(icons);
    if (!parent) {
        HWND progman = FindWindowW(L"Progman", nullptr);
        DWORD owner = 0;
        if (progman)
            GetWindowThreadProcessId(progman, &owner);
        if (progman && owner && owner == ShellProcessId()) {
            // Ask Explorer to create the WorkerW wallpaper layer.
            DWORD_PTR result = 0;
            SendMessageTimeoutW(progman, 0x052C, 0xD, 1, SMTO_ABORTIFHUNG, 1000,
                                &result);
            parent = FindWallpaperHost(icons);
        }
    }
    if (g_rebuild || g_retryWindows || parent != g_parent || icons != g_icons ||
        !SameLayout(layout, g_layout)) {
        Rebuild(layout, parent, icons);
        return;
    }
    g_layout = layout;
    for (auto& p : *g_panes) {
        // Follow Windows' SDR brightness slider without a rebuild.
        for (const auto& d : layout) {
            if (d.name != p->display.name ||
                d.sdrWhiteLevel == p->display.sdrWhiteLevel)
                continue;
            p->display.sdrWhiteLevel = d.sdrWhiteLevel;
            if (FAILED(ApplyWhiteLevel(*p)))
                p->FreeRenderer();
            p->dirty = true;
        }
        RefreshImage(*p);
        if (p->window && p->shown && g_icons && !LayerOrderValid(*p))
            Position(*p, 0);
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
    auto setDpi =
        reinterpret_cast<SetDpiProc>(reinterpret_cast<void*>(GetProcAddress(
            GetModuleHandleW(L"user32.dll"), "SetThreadDpiAwarenessContext")));
    // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2: physical coordinates.
    HANDLE dpi = setDpi ? setDpi(reinterpret_cast<HANDLE>(-4)) : nullptr;
    ULONG_PTR token = 0;
    Gdiplus::GdiplusStartupInput input;
    HINSTANCE instance = GetModuleHandleW(nullptr);
    bool registered = false;
    HANDLE frameTimer = nullptr;
    g_panes.emplace();
    g_parent = g_icons = nullptr;
    g_rebuild = true;
    g_nextMaintenance = 0;
    g_deviceRetryAt = 0;
    g_deviceFailures = g_deviceLosses = 0;
    InitializeCriticalSection(&g_loadLock);
    g_loadLockReady = true;
    g_loaderStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_loadWake = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_loadDone = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (g_loaderStop && g_loadWake && g_loadDone &&
        Gdiplus::GdiplusStartup(&token, &input, nullptr) == Gdiplus::Ok) {
        g_loaderThread =
            CreateThread(nullptr, 0, LoaderThread, nullptr, 0, nullptr);
        if (!g_loaderThread)
            Wh_Log(L"Loader thread unavailable; images load synchronously");
        if (FAILED(
                D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2d)))
            Wh_Log(L"Direct2D initialization failed; the mod stays idle");
        if (SUCCEEDED(com))
            CreateWallpaperInterface();
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
        // High-resolution one-shot timer; no process-wide timer resolution
        // changes. 0x2 is CREATE_WAITABLE_TIMER_HIGH_RESOLUTION.
        frameTimer = CreateWaitableTimerExW(nullptr, nullptr, 0x00000002,
                                            TIMER_ALL_ACCESS);
        if (!frameTimer)
            frameTimer = CreateWaitableTimerW(nullptr, FALSE, nullptr);
        double nextFrame = ClockMs();
        if (g_controller && frameTimer &&
            ArmFrameTimer(frameTimer, nextFrame) &&
            SetTimer(g_controller, 2, 250, nullptr)) {
            HANDLE events[]{g_stop, g_settingsEvent, frameTimer, g_loadDone};
            constexpr DWORD kEventCount = ARRAYSIZE(events);
            bool running = true;
            while (running && WaitForSingleObject(g_stop, 0) == WAIT_TIMEOUT) {
                if (g_rebuild || GetTickCount64() >= g_nextMaintenance) {
                    Maintenance();
                    g_nextMaintenance = GetTickCount64() + 2000;
                }
                DWORD wait = MsgWaitForMultipleObjectsEx(
                    kEventCount, events, 250, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
                if (wait == WAIT_OBJECT_0 || wait == WAIT_FAILED)
                    break;
                if (wait == WAIT_OBJECT_0 + 1) {
                    ReadSettings();
                    g_rebuild = true;
                }
                if (wait == WAIT_OBJECT_0 + 2) {
                    Activity activity = Tick();
                    int fps = activity == Activity::Moving ? g_fps
                              : activity == Activity::Idle ? std::min(g_fps, 30)
                                                           : 10;
                    nextFrame =
                        std::max(nextFrame + 1000.0 / fps, ClockMs() + 0.1);
                    if (!ArmFrameTimer(frameTimer, nextFrame))
                        break;
                }
                if (wait == WAIT_OBJECT_0 + 3)
                    ProcessLoadResults();
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
        } else {
            Wh_Log(L"Controller/timer creation failed, error=%lu",
                   GetLastError());
        }
        g_rebuilding = true;
        StopLoader();
        if (frameTimer) {
            CancelWaitableTimer(frameTimer);
            CloseHandle(frameTimer);
        }
        // Destroys the pane windows and their GPU resources on this thread.
        g_panes.reset();
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
        ReleaseDevice();
        if (g_d2d) {
            g_d2d->Release();
            g_d2d = nullptr;
        }
        Gdiplus::GdiplusShutdown(token);
        g_rebuilding = false;
    } else {
        Wh_Log(L"Initialization failed, error=%lu", GetLastError());
    }
    StopLoader();
    g_panes.reset();
    for (HANDLE* event : {&g_loaderStop, &g_loadWake, &g_loadDone}) {
        if (*event)
            CloseHandle(*event);
        *event = nullptr;
    }
    g_loadLockReady = false;
    DeleteCriticalSection(&g_loadLock);
    if (dpi && setDpi)
        setDpi(dpi);
    if (SUCCEEDED(com))
        CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// Tool mod callbacks. The mod runs in a dedicated windhawk.exe process, so a
// failure in the renderer cannot affect Explorer.

BOOL WhTool_ModInit() {
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
void WhTool_ModSettingsChanged() {
    if (g_settingsEvent)
        SetEvent(g_settingsEvent);
}
void WhTool_ModUninit() {
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

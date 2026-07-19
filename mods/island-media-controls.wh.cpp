// ==WindhawkMod==
// @id              island-media-controls
// @name            Island Media Controls
// @description     Dynamic island-like media controls for the Windows 11 taskbar.
// @version         0.9.211
// @author          usho
// @github          https://github.com/usho-lear
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lwindowsapp -luuid -luser32 -lshell32 -lgdi32 -lmsimg32 -lshlwapi -lwindowscodecs -ldwmapi -luiautomationcore -ld3d11 -ldxgi -ld2d1 -ldcomp -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Island Media Controls

Bring the current media session into the Windows 11 taskbar as a compact,
clickable island. It shows the active track, artist, artwork, playback state,
and progress, then expands into a larger player with seek, previous,
play/pause, and next controls.

## Preview

| Dark mode | Light mode |
| :---: | :---: |
| ![Island Media Controls in dark mode](https://raw.githubusercontent.com/usho-lear/island-media-controls/main/previews/dark-mode.gif) | ![Island Media Controls in light mode](https://raw.githubusercontent.com/usho-lear/island-media-controls/main/previews/light-mode.gif) |

## What's new

- **Liquid Glass refinements:** The material now uses a smoother edge-following
  refraction band and stronger light-mode backdrop/control tint.
- **Safer capture startup:** Borderless capture authorization is now bounded
  and cleanly stopped before the mod unloads.
- **Thread-safe live blur:** Capture rendering now uses a taskbar-thread
  snapshot and serializes GPU presentation without blocking popup animation.
- **Better light-mode visibility:** Liquid Glass now keeps the album-art
  background wash enabled in light mode so the expanded surface remains legible.
- **Current tuned defaults:** New installs now default to the tuned Liquid
  Glass setup, Fluent bold controls, updated popup spacing, and the current
  shadow depth while keeping browser thumbnails as the default artwork mode.
- **Taskbar-aware expansion:** The expanded player adapts its opening direction,
  layout, and artwork wash to the taskbar position.
- **More cohesive morphing:** The compact island, artwork, progress bar, and
  playback controls animate together during expand and collapse.

## Features

- **Taskbar placement:** Put the island near the tray, clock, or taskbar edge,
  with automatic sizing for different taskbar heights.
- **Compact media glance:** See artwork, title, artist, playback state, and
  progress without opening the full player.
- **Expanded player:** Open a larger player with artwork, seekable progress,
  transport buttons, album-color wash, and optional compact expanded layout.
- **Taskbar-position awareness:** When the taskbar moves, the popup direction,
  layout, and background wash update to match.
- **Multiple materials:** Choose Mica-like, Solid, Acrylic, or Liquid Glass
  styling for the compact and expanded surfaces.
- **Artwork options:** Keep browser thumbnails as-is, or replace low-resolution
  video thumbnails with generated mesh-gradient or flame artwork.
- **Animation controls:** Tune hover scale, smoothing, and popup animation speed
  for normal use or slow-motion preview.

## Material previews

| Material | Dark mode | Light mode |
| :--- | :---: | :---: |
| **Mica-like** | ![Mica-like material in dark mode](https://raw.githubusercontent.com/usho-lear/island-media-controls/main/previews/materials/mica-dark.png) | ![Mica-like material in light mode](https://raw.githubusercontent.com/usho-lear/island-media-controls/main/previews/materials/mica-light.png) |
| **Acrylic** | ![Acrylic material in dark mode](https://raw.githubusercontent.com/usho-lear/island-media-controls/main/previews/materials/acrylic-dark.png) | ![Acrylic material in light mode](https://raw.githubusercontent.com/usho-lear/island-media-controls/main/previews/materials/acrylic-light.png) |
| **Liquid Glass** | ![Liquid Glass material in dark mode](https://raw.githubusercontent.com/usho-lear/island-media-controls/main/previews/materials/liquid-glass-dark.png) | ![Liquid Glass material in light mode](https://raw.githubusercontent.com/usho-lear/island-media-controls/main/previews/materials/liquid-glass-light.png) |
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Main:
  - Position: "tray_left"
    $name: Taskbar position
    $options:
    - "tray_left": "Tray - far left"
    - "tray_right": "Tray - far right"
    - "tray_before_clock": "Tray - left of clock"
    - "tray_after_clock": "Tray - right of clock"
    - "taskbar_left_edge": "Taskbar - left overlay"
    - "taskbar_center_edge": "Taskbar - center overlay"
    - "taskbar_right_edge": "Taskbar - right overlay"
  - CompactWidth: 169
    $name: Compact width
  - AutoSizeToTaskbar: true
    $name: Auto size to taskbar
  - ExpandedWidth: 360
    $name: Expanded overlay width
  - ExpandedHeight: 500
    $name: Expanded overlay height
  - Compact: false
    $name: Compact expanded layout
    $description: Use a single compact card with the cover beside the song and artist information.
  - PopupSpacing: 20
    $name: Expanded outer spacing
  - PopupCardGap: 8
    $name: Expanded cover-to-controls gap
  - PopupShadowDepth: 58
    $name: Expanded shadow depth
  - PopupShadowOpacity: 70
    $name: Expanded shadow opacity (%)
  - PopupButtonStyle: "fluent_bold"
    $name: Expanded button style
    $options:
    - "minimal_transport": "Minimal transport"
    - "fluent_bold": "Fluent bold"
  - PopupBackdropCoverEffect: "dark_only"
    $name: Expanded background album wash
    $description: Liquid Glass keeps this enabled in light mode for readability.
    $options:
    - "off": "Off"
    - "dark_only": "Dark mode only"
    - "on": "Always on"
  - ArtworkAbstractMode: "browser_original"
    $name: Low-res video artwork mode
    $options:
    - "browser_original": "Browser thumbnail"
    - "mesh_gradient": "Mesh gradient"
    - "energy_flame": "Flame"
  - Height: 40
    $name: Component height
  - MarginLeft: 4
    $name: Left margin
  - MarginRight: 4
    $name: Right margin
  - HideWhenNoMedia: false
    $name: Hide when no media
  - HoverScale: 106
    $name: Hover scale (%)
  - HoverLerpSpeed: 28
    $name: Hover smoothing
  - AnimationSpeed: 100
    $name: Expanded animation speed (%)
    $description: 100 is normal speed. Use lower values such as 25 for slow-motion animation preview.
  - Material: "liquid_glass"
    $name: Background material
    $options:
    - "mica_like": "Mica-like content layer"
    - "solid": "Solid"
    - "acrylic": "Acrylic / glass"
    - "liquid_glass": "Liquid glass"
  - BackdropInitialFrameSkip: 2
    $name: Backdrop initial frame skip
    $description: Number of WGC frames to skip before replacing the fallback frame.
  - BackdropFallbackBlurPasses: 5
    $name: Backdrop fallback blur passes
    $description: Blur strength for the animation-period fallback frame.
  - BackdropFallbackCaptureScale: 2
    $name: Backdrop fallback capture scale
    $description: Lower values sample more pixels; higher values are faster but softer.
  - BackdropWgcBlurStdDev: 18
    $name: Backdrop WGC blur strength
    $description: Gaussian blur standard deviation for live WGC blur.
  - AllowScreenCapture: false
    $name: Allow screen capture of expanded popup
    $description: Uses a capturable static blurred backdrop for Acrylic and Liquid glass, avoiding self-capture feedback.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <tlhelp32.h>
#include <uiautomation.h>
#include <shlwapi.h>
#include <wincodec.h>
#include <commctrl.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>


#include <inspectable.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d2d1effects.h>
#include <dcomp.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Metadata.h>
#include <winrt/Windows.Security.Authorization.AppCapabilityAccess.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>

// Windhawk's bundled SDK may miss the two small WinRT interop headers even
// though the WinRT projection and D3D/D2D headers are present. Define the
// minimal COM interfaces locally so the WGC path is actually compiled instead
// of silently falling back to the stub branch.
//
// Do not use __declspec(uuid) here: Windhawk's Clang/MinGW configuration ignores
// that attribute, which breaks winrt::guid_of<T>. Use explicit GUID constants
// and raw QueryInterface instead.
static constexpr GUID kIID_IGraphicsCaptureItemInterop{
    0x3628E81B, 0x3CAC, 0x4C60,
    {0xB7, 0xF4, 0x23, 0xCE, 0x0E, 0x0C, 0x33, 0x56}};
static constexpr GUID kIID_IDirect3DDxgiInterfaceAccess{
    0xA9B3D012, 0x3DF2, 0x4EE3,
    {0xB8, 0xD1, 0x86, 0x95, 0xF4, 0x57, 0xD3, 0xC1}};
static constexpr GUID kIID_IGraphicsCaptureItem{
    0x79C3F95B, 0x31F7, 0x4EC2,
    {0xA4, 0x64, 0x63, 0x2E, 0xF5, 0xD3, 0x07, 0x60}};

struct IGraphicsCaptureItemInterop : ::IUnknown {
    virtual HRESULT __stdcall CreateForWindow(HWND window,
                                              REFIID riid,
                                              void** result) = 0;
    virtual HRESULT __stdcall CreateForMonitor(HMONITOR monitor,
                                               REFIID riid,
                                               void** result) = 0;
};

struct IDirect3DDxgiInterfaceAccess : ::IUnknown {
    virtual HRESULT __stdcall GetInterface(REFIID iid,
                                           void** p) = 0;
};

extern "C" HRESULT __stdcall CreateDirect3D11DeviceFromDXGIDevice(
    IDXGIDevice* dxgiDevice,
    IInspectable** graphicsDevice);

#include <windhawk_utils.h>

#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>

#include <algorithm>
#include <array>
#include <functional>
#include <atomic>
#include <chrono>
#include <cwctype>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <climits>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#ifndef WS_EX_NOREDIRECTIONBITMAP
#define WS_EX_NOREDIRECTIONBITMAP 0x00200000L
#endif

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

namespace xaml = winrt::Windows::UI::Xaml;
namespace composition = winrt::Windows::UI::Composition;
namespace controls = winrt::Windows::UI::Xaml::Controls;
namespace input = winrt::Windows::UI::Xaml::Input;
namespace mediax = winrt::Windows::UI::Xaml::Media;
namespace imaging = winrt::Windows::UI::Xaml::Media::Imaging;
namespace hosting = winrt::Windows::UI::Xaml::Hosting;
namespace streams = winrt::Windows::Storage::Streams;
namespace gsm = winrt::Windows::Media::Control;

namespace capture = winrt::Windows::Graphics::Capture;
namespace directx = winrt::Windows::Graphics::DirectX;
namespace direct3d11 = winrt::Windows::Graphics::DirectX::Direct3D11;


using xaml::FrameworkElement;
using xaml::GridLength;
using xaml::GridUnitType;
using xaml::HorizontalAlignment;
using xaml::Thickness;
using xaml::UIElement;
using xaml::VerticalAlignment;
using xaml::Visibility;
using controls::Border;
using controls::Button;
using controls::ColumnDefinition;
using controls::Grid;
using controls::Image;
using controls::ProgressBar;
using controls::StackPanel;
using controls::TextBlock;
using mediax::SolidColorBrush;
using mediax::ScaleTransform;
using mediax::TransformGroup;
using mediax::TranslateTransform;
using mediax::VisualTreeHelper;

namespace {

struct Settings {
    std::wstring position = L"tray_left";
    int compactWidth = 169;
    bool autoSizeToTaskbar = true;
    int expandedWidth = 360;
    int expandedHeight = 430;
    bool compact = false;
    int popupSpacing = 20;
    int popupCardGap = 8;
    int popupShadowDepth = 58;
    int popupShadowOpacity = 70;
    std::wstring popupButtonStyle = L"fluent_bold";
    std::wstring popupBackdropCoverEffect = L"dark_only";
    std::wstring artworkAbstractMode = L"browser_original";
    int height = 40;
    int marginLeft = 4;
    int marginRight = 4;
    bool hideWhenNoMedia = false;
    double hoverScale = 1.06;
    double hoverLerpSpeed = 28.0;
    double animationSpeed = 1.0;
    std::wstring material = L"liquid_glass";
    int backdropInitialFrameSkip = 2;
    int backdropFallbackBlurPasses = 5;
    int backdropFallbackCaptureScale = 2;
    int backdropWgcBlurStdDev = 18;
    bool allowScreenCapture = false;
};

struct RuntimeLayout {
    double taskbarHeightDip = 40.0;
    double compactHeight = 40.0;
    double compactWidth = 168.0;
    double artSize = 28.0;
    double artImageSize = 30.0;
    double artCornerRadius = 8.0;
    double cornerRadius = 20.0;
    double contentMarginX = 10.0;
    double contentMarginY = 4.0;
    double artColumnWidth = 30.0;
    double textMarginX = 8.0;
    double titleFontSize = 12.0;
    double artistFontSize = 10.0;
    double progressWidth = 74.0;
    double progressHeight = 4.0;
    double progressMarginLeft = 8.0;
};

struct MediaState {
    std::wstring title = L"Not Playing";
    std::wstring artist;
    bool hasSession = false;
    bool isPlaying = false;
    bool canSeek = false;
    int64_t timelineStartTicks = 0;
    int64_t positionTicks = 0;
    int64_t durationTicks = 0;
    std::wstring sourceAppUserModelId;
    std::vector<uint8_t> thumbnailBytes;
};

using CTaskBand_GetTaskbarHost_t = void(WINAPI*)(void* pThis, void* result);
using TaskbarHost_FrameHeight_t = int(WINAPI*)(void* pThis);
using Std_Ref_Decref_t = void(WINAPI*)(void* pThis);
using TrayUI_StartTaskbar_t = void(WINAPI*)(void* pThis);
using WindowThreadProc = void (*)(void*);
using MediaCommand =
    std::function<void(gsm::GlobalSystemMediaTransportControlsSession const&)>;

Settings g_settings;
RuntimeLayout g_layout;
std::mutex g_pendingSettingsMutex;
std::optional<Settings> g_pendingSettings;
std::mutex g_mediaMutex;
std::mutex g_seekMutex;
MediaState g_media;
std::chrono::steady_clock::time_point g_mediaStateTimestamp;
double g_popupLiveProgressValue = 0.0;
bool g_popupLiveProgressValid = false;
std::wstring g_popupLiveProgressKey;
std::chrono::steady_clock::time_point g_popupLiveProgressFrameTime;
bool g_popupSeekDragging = false;
double g_popupSeekPreviewRatio = 0.0;
std::chrono::steady_clock::time_point g_popupSeekPreviewUntil;
bool g_popupSeekCommitPending = false;
double g_popupSeekCommitRatio = 0.0;
int64_t g_popupSeekCommitTargetTicks = 0;
std::chrono::steady_clock::time_point g_popupSeekCommitUntil;

std::atomic_bool g_unloading = false;
std::atomic_bool g_modActive = false;
std::atomic_bool g_mediaThreadRunning = false;
std::thread* g_mediaThread = nullptr;
std::atomic_bool g_mediaThreadExited = true;
std::mutex g_mediaCommandMutex;
std::condition_variable g_mediaCommandCv;
std::deque<MediaCommand> g_mediaCommands;
std::atomic_bool g_mediaRefreshRequested = true;

bool IsModActive() {
    return g_modActive.load() && !g_unloading.load();
}

constexpr auto kMediaPropertiesAsyncTimeout = std::chrono::milliseconds(1500);
constexpr auto kThumbnailAsyncTimeout = std::chrono::milliseconds(900);
constexpr auto kMediaCommandAsyncTimeout = std::chrono::milliseconds(1500);
constexpr auto kUiLocalAsyncTimeout = std::chrono::milliseconds(500);
constexpr auto kMediaThreadStopTimeout = std::chrono::milliseconds(5000);
constexpr auto kPopupOverlayWgcBorderlessAccessTimeout =
    std::chrono::milliseconds(5000);

template <typename AsyncOperation>
bool WaitForAsyncWithTimeout(AsyncOperation const& operation,
                             std::chrono::milliseconds timeout,
                             const wchar_t* label) {
    if (!operation) {
        return false;
    }

    auto status = operation.wait_for(timeout);
    if (status == winrt::Windows::Foundation::AsyncStatus::Completed) {
        return true;
    }

    try {
        operation.Cancel();
    } catch (...) {
    }

    Wh_Log(L"Island: async operation timed out label=%s status=%d timeoutMs=%lld",
           label ? label : L"(unknown)",
           static_cast<int>(status),
           static_cast<long long>(timeout.count()));
    return false;
}

template <typename AsyncOperation>
auto GetAsyncResultWithTimeout(AsyncOperation const& operation,
                               std::chrono::milliseconds timeout,
                               const wchar_t* label)
    -> decltype(operation.GetResults()) {
    if (!WaitForAsyncWithTimeout(operation, timeout, label)) {
        throw winrt::hresult_error(HRESULT_FROM_WIN32(ERROR_TIMEOUT));
    }

    return operation.GetResults();
}

HWND g_taskbarWnd = nullptr;
Grid g_playerGrid = nullptr;
FrameworkElement g_injectionParent = nullptr;
int g_playerColumn = -1;
bool g_expanded = false;
ScaleTransform g_islandScale = nullptr;
uint64_t g_lastThumbnailHash = 0;
winrt::event_token g_hoverRenderingToken{};
bool g_hoverRenderingHooked = false;
double g_currentHoverScale = 1.0;
double g_targetHoverScale = 1.0;
std::chrono::steady_clock::time_point g_lastHoverFrameTime{};
bool g_lastDarkMode = false;
bool g_themeVisualsValid = false;
bool g_popupXamlThemeValid = false;
bool g_popupXamlThemeDark = false;
std::wstring g_popupXamlThemeMaterial;
std::wstring g_popupXamlThemeButtonStyle;
int g_popupXamlThemeShadowDepth = -1;
int g_popupXamlThemeShadowOpacity = -1;
bool g_popupTextForegroundValid = false;
bool g_popupTextForegroundEdgeFade = false;
bool g_popupTextForegroundDark = false;
double g_popupTextForegroundEdgeFadeAmount = -1.0;
HWND g_expandedPopup = nullptr;
bool g_popupClassRegistered = false;
HWND g_popupBackdropOverlay = nullptr;
bool g_popupBackdropOverlayClassRegistered = false;
std::chrono::steady_clock::time_point g_popupBackdropOverlayLastPaintTime{};
int g_popupBackdropOverlayLastWidth = 0;
int g_popupBackdropOverlayLastHeight = 0;
int g_popupBackdropOverlayLastRadius = 0;
std::atomic<int> g_popupBackdropOverlayFrameAlpha{255};
bool g_popupBackdropOverlayNativeBlurActive = false;
bool g_expandedPopupCaptureExcluded = false;
bool g_popupXamlChildCaptureExcluded = false;
bool g_popupBackdropOverlayCaptureExcluded = false;
RECT g_popupLiquidGlassPanelRectPx{};
int g_popupLiquidGlassPanelRadiusPx = 0;
bool g_popupLiquidGlassPanelRectValid = false;
bool g_popupOverlayWgcReadbackHadVisibleFrame = false;
enum class PopupOverlayWgcDiagnosticState {
    NotStarted,
    StartSkipped,
    StartedNoFrame,
    FrameArrived,
    RenderFailed,
    MapFailed,
    UpdateFailed,
    VisibleFrame,
};
PopupOverlayWgcDiagnosticState g_popupOverlayWgcDiagnosticState =
    PopupOverlayWgcDiagnosticState::NotStarted;
HRESULT g_popupOverlayWgcDiagnosticHr = S_OK;
uint64_t g_popupOverlayWgcFrameCount = 0;
uint64_t g_popupOverlayWgcRenderFailCount = 0;
std::chrono::steady_clock::time_point g_popupOverlayWgcLastStartAttemptTime{};
HRESULT g_popupOverlayWgcCreateItemHr = S_OK;
bool g_popupOverlayWgcCreateItemFailed = false;
bool g_popupOverlayWgcFallbackPainted = false;
int g_popupOverlayWgcFramesToSkip = 0;
std::mutex g_popupBackdropOverlayHandoffMutex;
std::vector<BYTE> g_popupBackdropOverlayFallbackPixels;
RECT g_popupBackdropOverlayFallbackRect{};
int g_popupBackdropOverlayFallbackWidth = 0;
int g_popupBackdropOverlayFallbackHeight = 0;
int g_popupBackdropOverlayFallbackRadius = 0;
bool g_popupBackdropOverlayFallbackPixelsValid = false;
std::vector<BYTE> g_popupBackdropOverlayCleanPixels;
RECT g_popupBackdropOverlayCleanRect{};
int g_popupBackdropOverlayCleanWidth = 0;
int g_popupBackdropOverlayCleanHeight = 0;
bool g_popupBackdropOverlayCleanPixelsValid = false;

void ClearPopupBackdropOverlayHandoffCache() {
    std::lock_guard cacheLock(g_popupBackdropOverlayHandoffMutex);
    g_popupBackdropOverlayFallbackPixels.clear();
    g_popupBackdropOverlayFallbackRect = {};
    g_popupBackdropOverlayFallbackWidth = 0;
    g_popupBackdropOverlayFallbackHeight = 0;
    g_popupBackdropOverlayFallbackRadius = 0;
    g_popupBackdropOverlayFallbackPixelsValid = false;
    g_popupBackdropOverlayCleanPixels.clear();
    g_popupBackdropOverlayCleanRect = {};
    g_popupBackdropOverlayCleanWidth = 0;
    g_popupBackdropOverlayCleanHeight = 0;
    g_popupBackdropOverlayCleanPixelsValid = false;
}



std::mutex g_popupOverlayWgcMutex;
std::atomic<bool> g_popupOverlayWgcRendering{false};
struct PopupOverlayWgcRenderParameters {
    int targetRadiusPx = 1;
    bool morphing = false;
    bool allowScreenCapture = false;
    bool useLiquidLensWarp = false;
    float blurStdDev = 18.0f;
    float refractionBlurStdDev = 2.0f;
    float liquidRimAlpha = 0.38f;
};
PopupOverlayWgcRenderParameters g_popupOverlayWgcRenderParameters;
std::atomic_bool g_popupOverlayWgcRunning = false;
bool g_popupOverlayWgcFrameCallbackHooked = false;
std::atomic<bool> g_popupOverlayWgcDirtyRegionsEnabled{false};
std::atomic<int> g_popupOverlayWgcBorderlessAccessState{0};
std::mutex g_popupOverlayWgcBorderlessAccessThreadMutex;
std::thread* g_popupOverlayWgcBorderlessAccessThread = nullptr;
RECT g_popupOverlayWgcCaptureRectPx{LONG_MIN, LONG_MIN, LONG_MIN, LONG_MIN};
RECT g_popupOverlayWgcMonitorRectPx{};
int g_popupOverlayWgcTargetWidthPx = 0;
int g_popupOverlayWgcTargetHeightPx = 0;
int g_popupOverlayWgcTargetRadiusPx = 0;
HMONITOR g_popupOverlayWgcMonitor = nullptr;
HRESULT g_popupOverlayWgcLastHr = S_OK;

[[clang::no_destroy]] winrt::com_ptr<ID3D11Device> g_popupOverlayWgcD3dDevice;
[[clang::no_destroy]] winrt::com_ptr<ID3D11DeviceContext> g_popupOverlayWgcD3dContext;
[[clang::no_destroy]] winrt::com_ptr<IDXGIDevice> g_popupOverlayWgcDxgiDevice;
[[clang::no_destroy]] winrt::com_ptr<IDXGISwapChain1> g_popupOverlayWgcSwapChain;
[[clang::no_destroy]] winrt::com_ptr<IDCompositionDevice> g_popupOverlayDcompDevice;
[[clang::no_destroy]] winrt::com_ptr<IDCompositionTarget> g_popupOverlayDcompTarget;
[[clang::no_destroy]] winrt::com_ptr<IDCompositionVisual> g_popupOverlayDcompVisual;
bool g_popupOverlayDcompContentAttached = false;
[[clang::no_destroy]] winrt::com_ptr<ID2D1Bitmap1> g_popupOverlayWgcSwapChainTargetBitmap;
[[clang::no_destroy]] winrt::com_ptr<ID2D1Factory1> g_popupOverlayWgcD2dFactory;
[[clang::no_destroy]] winrt::com_ptr<ID2D1Device> g_popupOverlayWgcD2dDevice;
[[clang::no_destroy]] winrt::com_ptr<ID2D1DeviceContext> g_popupOverlayWgcD2dContext;
[[clang::no_destroy]] winrt::com_ptr<ID2D1Bitmap1> g_popupOverlayWgcLensDisplacementMap;
[[clang::no_destroy]] winrt::com_ptr<ID2D1Bitmap1> g_popupOverlayWgcIntermediateBitmap;
[[clang::no_destroy]] winrt::com_ptr<ID2D1Effect> g_popupOverlayWgcBlurEffect;
[[clang::no_destroy]] winrt::com_ptr<ID2D1Effect> g_popupOverlayWgcRefractionBlurEffect;
[[clang::no_destroy]] winrt::com_ptr<ID2D1Effect> g_popupOverlayWgcDisplacementEffect;
[[clang::no_destroy]] winrt::com_ptr<ID2D1SolidColorBrush> g_popupOverlayWgcRimBrush;
[[clang::no_destroy]] winrt::com_ptr<ID2D1RoundedRectangleGeometry> g_popupOverlayWgcRoundedGeometry;
int g_popupOverlayWgcLensMapWidth = 0;
int g_popupOverlayWgcLensMapHeight = 0;
int g_popupOverlayWgcLensMapRadius = 0;
int g_popupOverlayWgcRenderCacheWidth = 0;
int g_popupOverlayWgcRenderCacheHeight = 0;
int g_popupOverlayWgcRoundedGeometryWidth = 0;
int g_popupOverlayWgcRoundedGeometryHeight = 0;
int g_popupOverlayWgcRoundedGeometryRadius = 0;
bool g_popupOverlayDcompTransformValid = false;
float g_popupOverlayDcompLastScaleX = 0.0f;
float g_popupOverlayDcompLastScaleY = 0.0f;

[[clang::no_destroy]] direct3d11::IDirect3DDevice g_popupOverlayWgcGraphicsDevice{nullptr};
[[clang::no_destroy]] capture::GraphicsCaptureItem g_popupOverlayWgcItem{nullptr};
[[clang::no_destroy]] capture::Direct3D11CaptureFramePool g_popupOverlayWgcFramePool{nullptr};
[[clang::no_destroy]] capture::GraphicsCaptureSession g_popupOverlayWgcSession{nullptr};
winrt::event_token g_popupOverlayWgcFrameArrivedToken{};
bool g_popupOverlayWgcHadFrame = false;
std::chrono::steady_clock::time_point g_popupOverlayWgcDiagnosticStartTime{};
std::chrono::steady_clock::time_point g_popupOverlayWgcLastRenderTime{};
std::chrono::steady_clock::time_point g_popupOverlayWgcLastResizeTime{};
double g_popupOverlayWgcFrameIntervalMs = 1000.0 / 60.0;


hosting::DesktopWindowXamlSource g_popupXamlSource = nullptr;
HWND g_popupXamlChild = nullptr;
bool g_popupXamlChildSubclassed = false;
Grid g_popupXamlRoot = nullptr;
controls::Canvas g_popupXamlCanvas = nullptr;
Border g_popupXamlShadow = nullptr;
composition::SpriteVisual g_popupXamlShadowVisual = nullptr;
composition::DropShadow g_popupXamlDropShadow = nullptr;
Border g_popupXamlBackdrop = nullptr;
ScaleTransform g_popupXamlBackdropScale = nullptr;
TranslateTransform g_popupXamlBackdropTranslate = nullptr;
Image g_popupXamlBackdropCoverFade = nullptr;
Image g_popupXamlBackdropCover = nullptr;
Border g_popupXamlBackdropTint = nullptr;
Border g_popupXamlBackdropSurfaceHighlight = nullptr;
Border g_popupXamlBackdropRimHighlight = nullptr;
Border g_popupXamlPanelCoverFrame = nullptr;
ScaleTransform g_popupXamlPanelCoverScale = nullptr;
TranslateTransform g_popupXamlPanelCoverTranslate = nullptr;
Image g_popupXamlPanelCoverFade = nullptr;
Image g_popupXamlPanelCover = nullptr;
Border g_popupXamlPanel = nullptr;
ScaleTransform g_popupXamlPanelScale = nullptr;
TranslateTransform g_popupXamlPanelTranslate = nullptr;
Border g_popupXamlArtFrame = nullptr;
ScaleTransform g_popupXamlArtScale = nullptr;
TranslateTransform g_popupXamlArtTranslate = nullptr;
Image g_popupXamlArtFade = nullptr;
Image g_popupXamlArt = nullptr;
FrameworkElement g_popupXamlTitleHost = nullptr;
FrameworkElement g_popupXamlArtistHost = nullptr;
FrameworkElement g_popupXamlOutgoingTitleHost = nullptr;
FrameworkElement g_popupXamlOutgoingArtistHost = nullptr;
Border g_popupXamlTitleLeftFade = nullptr;
Border g_popupXamlTitleRightFade = nullptr;
Border g_popupXamlArtistLeftFade = nullptr;
Border g_popupXamlArtistRightFade = nullptr;
Border g_popupXamlOutgoingTitleLeftFade = nullptr;
Border g_popupXamlOutgoingTitleRightFade = nullptr;
Border g_popupXamlOutgoingArtistLeftFade = nullptr;
Border g_popupXamlOutgoingArtistRightFade = nullptr;
TextBlock g_popupXamlTitle = nullptr;
TextBlock g_popupXamlArtist = nullptr;
TextBlock g_popupXamlOutgoingTitle = nullptr;
TextBlock g_popupXamlOutgoingArtist = nullptr;
TranslateTransform g_popupXamlTitleTranslate = nullptr;
TranslateTransform g_popupXamlArtistTranslate = nullptr;
TranslateTransform g_popupXamlOutgoingTitleTranslate = nullptr;
TranslateTransform g_popupXamlOutgoingArtistTranslate = nullptr;
double g_popupTextBaseOpacity = 1.0;
bool g_popupTextTransitionActive = false;
int g_popupTextTransitionDirection = 1;
int g_pendingMediaNavigationDirection = 0;
std::chrono::steady_clock::time_point g_pendingMediaNavigationTime{};
TextBlock g_popupXamlElapsed = nullptr;
TextBlock g_popupXamlDuration = nullptr;
ProgressBar g_popupXamlProgress = nullptr;
ScaleTransform g_popupXamlProgressScale = nullptr;
Border g_popupXamlProgressTrack = nullptr;
Border g_popupXamlProgressFill = nullptr;
Border g_popupXamlProgressHitTarget = nullptr;
controls::Canvas g_popupXamlProgressGlowMask = nullptr;
mediax::RectangleGeometry g_popupXamlProgressGlowClip = nullptr;
std::vector<Border> g_popupXamlProgressGlowLayers;
std::vector<Border> g_popupXamlProgressCoreBlurLayers;
Border g_popupXamlProgressGlowCore = nullptr;
controls::Canvas g_popupXamlControls = nullptr;
uint64_t g_popupXamlThumbnailHash = UINT64_MAX;
std::wstring g_popupXamlThumbnailMaterial;
bool g_popupXamlThumbnailCompact = false;
bool g_popupXamlBackdropCoverEnabled = false;
uint64_t g_popupAccentThumbnailHash = UINT64_MAX;
bool g_popupAccentColorValid = false;
winrt::Windows::UI::Color g_popupAccentColor{0xFF, 0x4F, 0x7D, 0xE8};
bool g_popupDisplayedAccentColorValid = false;
winrt::Windows::UI::Color g_popupDisplayedAccentColor{0xFF, 0x4F, 0x7D, 0xE8};
winrt::Windows::UI::Color g_popupAccentTransitionFrom{0xFF, 0x4F, 0x7D, 0xE8};
winrt::Windows::UI::Color g_popupAccentTransitionTo{0xFF, 0x4F, 0x7D, 0xE8};
double g_popupMediaTransitionProgress = 1.0;
bool g_popupMediaTransitionActive = false;
// Artwork/album-wash crossfade is intentionally separated from text/accent
// transitions. Track metadata can update before artwork arrives; sharing the
// same media transition would fade the current cover to the placeholder/blank
// even when the cover source itself did not change.
bool g_popupCoverTransitionActive = false;
double g_popupCurrentWidth = 0.0;
double g_popupTargetWidth = 0.0;
double g_popupExpandedWidth = 0.0;
double g_popupCurrentHeight = 0.0;
double g_popupTargetHeight = 0.0;
double g_popupExpandedHeight = 0.0;
bool g_popupClosing = false;
bool g_popupOutsideClickArmed = false;
HBITMAP g_popupAlbumBitmap = nullptr;
uint64_t g_popupThumbnailHash = 0;
RECT g_popupSourceRect{};
RECT g_popupSourceArtRect{};
RECT g_popupSourceTitleRect{};
RECT g_popupSourceArtistRect{};
RECT g_popupSourceProgressRect{};
RECT g_popupSourceControlsRect{};
RECT g_popupFinalRect{};
RECT g_popupHostRect{};
bool g_popupExpandsRight = true;
bool g_popupOpensDown = false;
bool g_taskbarAtTop = false;
double g_popupAnimationProgress = 0.0;
double g_popupAnimationTarget = 0.0;
winrt::event_token g_popupRenderingToken{};
bool g_popupRenderingHooked = false;
std::chrono::steady_clock::time_point g_lastPopupFrameTime{};
bool g_popupPendingContentRefresh = false;
double g_lastPopupVisualProgress = -1.0;
RECT g_lastPopupVisualRect{LONG_MIN, LONG_MIN, LONG_MIN, LONG_MIN};
RECT g_lastPopupWindowRect{LONG_MIN, LONG_MIN, LONG_MIN, LONG_MIN};

TextBlock g_compactTitleText = nullptr;
TextBlock g_compactArtistText = nullptr;
FrameworkElement g_compactTextHost = nullptr;
Border g_compactTextLeftFade = nullptr;
Border g_compactTextRightFade = nullptr;
TextBlock g_compactOutgoingTitleText = nullptr;
TextBlock g_compactOutgoingArtistText = nullptr;
TranslateTransform g_compactTitleTranslate = nullptr;
TranslateTransform g_compactArtistTranslate = nullptr;
TranslateTransform g_compactOutgoingTitleTranslate = nullptr;
TranslateTransform g_compactOutgoingArtistTranslate = nullptr;
Image g_compactAlbumArtImage = nullptr;
Image g_compactAlbumArtFade = nullptr;
ProgressBar g_compactProgress = nullptr;
winrt::event_token g_compactProgressRenderingToken{};
bool g_compactProgressRenderingHooked = false;
std::chrono::steady_clock::time_point g_lastCompactProgressFrameTime;
winrt::event_token g_compactTextRenderingToken{};
bool g_compactTextRenderingHooked = false;
double g_compactTextProgress = 1.0;
std::chrono::steady_clock::time_point g_lastCompactTextFrameTime{};
bool g_compactTextEdgeFadeActive = false;
HWND g_taskbarLayoutTimerWindow = nullptr;
FrameworkElement g_taskbarLayoutWatchRoot = nullptr;
FrameworkElement g_taskbarLayoutWatchTarget = nullptr;
bool g_compactTextInitialized = false;
std::wstring g_compactLastTitle;
std::wstring g_compactLastArtist;
uint64_t g_compactLastTextMediaHash = UINT64_MAX;

void* CTaskBand_ITaskListWndSite_vftable = nullptr;
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;
TaskbarHost_FrameHeight_t TaskbarHost_FrameHeight_Original = nullptr;
Std_Ref_Decref_t Std_Ref_Decref_Original = nullptr;
TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;

template <typename T>
T Clamp(T value, T lo, T hi) {
    return std::max(lo, std::min(value, hi));
}

winrt::Windows::UI::Color Color(BYTE a, BYTE r, BYTE g, BYTE b) {
    return winrt::Windows::UI::Color{a, r, g, b};
}

winrt::Windows::UI::Color ColorFromPbgra(BYTE const* pixel) {
    BYTE alpha = pixel[3];
    if (alpha == 0) {
        return Color(0, 0, 0, 0);
    }

    double inverseAlpha = 255.0 / alpha;
    return Color(
        alpha,
        static_cast<BYTE>(Clamp(std::lround(pixel[2] * inverseAlpha), 0L, 255L)),
        static_cast<BYTE>(Clamp(std::lround(pixel[1] * inverseAlpha), 0L, 255L)),
        static_cast<BYTE>(Clamp(std::lround(pixel[0] * inverseAlpha), 0L, 255L)));
}

DWORD FindAppleMusicProcessId() {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }
    PROCESSENTRY32W entry{sizeof(entry)};
    DWORD processId = 0;
    if (Process32FirstW(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, L"AppleMusic.exe") == 0) {
                processId = entry.th32ProcessID;
                break;
            }
        } while (Process32NextW(snapshot, &entry));
    }
    CloseHandle(snapshot);
    return processId;
}

bool TrySeekAppleMusicWithUiAutomation(double ratio) {
    DWORD processId = FindAppleMusicProcessId();
    if (!processId) {
        return false;
    }

    IUIAutomation* automation = nullptr;
    IUIAutomationElement* root = nullptr;
    IUIAutomationCondition* processCondition = nullptr;
    IUIAutomationCondition* idCondition = nullptr;
    IUIAutomationCondition* combinedCondition = nullptr;
    IUIAutomationElement* scrubber = nullptr;
    IUnknown* unknown = nullptr;
    IUIAutomationRangeValuePattern* range = nullptr;
    bool succeeded = false;

    HRESULT result = CoCreateInstance(CLSID_CUIAutomation, nullptr,
                                      CLSCTX_INPROC_SERVER,
                                      IID_PPV_ARGS(&automation));
    VARIANT processVariant{};
    processVariant.vt = VT_I4;
    processVariant.lVal = static_cast<LONG>(processId);
    VARIANT idVariant{};
    idVariant.vt = VT_BSTR;
    idVariant.bstrVal = SysAllocString(L"LCDScrubber");

    if (SUCCEEDED(result)) {
        result = automation->GetRootElement(&root);
    }
    if (SUCCEEDED(result)) {
        result = automation->CreatePropertyCondition(
            UIA_ProcessIdPropertyId, processVariant, &processCondition);
    }
    if (SUCCEEDED(result) && idVariant.bstrVal) {
        result = automation->CreatePropertyCondition(
            UIA_AutomationIdPropertyId, idVariant, &idCondition);
    }
    if (SUCCEEDED(result)) {
        result = automation->CreateAndCondition(
            processCondition, idCondition, &combinedCondition);
    }
    if (SUCCEEDED(result)) {
        result = root->FindFirst(TreeScope_Subtree, combinedCondition,
                                 &scrubber);
        if (SUCCEEDED(result) && !scrubber) {
            result = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }
    }
    if (SUCCEEDED(result)) {
        result = scrubber->GetCurrentPattern(UIA_RangeValuePatternId,
                                             &unknown);
    }
    if (SUCCEEDED(result)) {
        result = unknown->QueryInterface(IID_PPV_ARGS(&range));
    }
    if (SUCCEEDED(result)) {
        BOOL readOnly = TRUE;
        double minimum = 0.0;
        double maximum = 0.0;
        result = range->get_CurrentIsReadOnly(&readOnly);
        if (SUCCEEDED(result)) result = range->get_CurrentMinimum(&minimum);
        if (SUCCEEDED(result)) result = range->get_CurrentMaximum(&maximum);
        if (SUCCEEDED(result) && !readOnly && maximum > minimum) {
            double value = minimum +
                           (maximum - minimum) * Clamp(ratio, 0.0, 1.0);
            result = range->SetValue(value);
            succeeded = SUCCEEDED(result);
        }
    }

    VariantClear(&idVariant);
    if (range) range->Release();
    if (unknown) unknown->Release();
    if (scrubber) scrubber->Release();
    if (combinedCondition) combinedCondition->Release();
    if (idCondition) idCondition->Release();
    if (processCondition) processCondition->Release();
    if (root) root->Release();
    if (automation) automation->Release();

    Wh_Log(L"Island: Apple Music UI Automation seek result=%d hr=0x%08X",
           succeeded, static_cast<unsigned>(result));
    return succeeded;
}

SolidColorBrush Brush(winrt::Windows::UI::Color color) {
    return SolidColorBrush(color);
}

void AttachGpuFriendlyTransform(FrameworkElement const& element,
                                ScaleTransform& scale,
                                TranslateTransform& translate) {
    if (!element) {
        return;
    }

    scale = ScaleTransform();
    translate = TranslateTransform();
    TransformGroup group;
    group.Children().Append(scale);
    group.Children().Append(translate);
    element.RenderTransformOrigin({0.0, 0.0});
    element.RenderTransform(group);
}

void SetElementSizeIfChanged(FrameworkElement const& element,
                             double width,
                             double height) {
    if (!element) {
        return;
    }

    width = std::max(1.0, width);
    height = std::max(1.0, height);
    double currentWidth = element.Width();
    double currentHeight = element.Height();
    if (std::isnan(currentWidth) || std::abs(currentWidth - width) > 0.5) {
        element.Width(width);
    }
    if (std::isnan(currentHeight) || std::abs(currentHeight - height) > 0.5) {
        element.Height(height);
    }
}

void SetCanvasPositionIfChanged(FrameworkElement const& element,
                                double left,
                                double top) {
    if (!element) {
        return;
    }

    double currentLeft = controls::Canvas::GetLeft(element);
    double currentTop = controls::Canvas::GetTop(element);
    if (std::isnan(currentLeft) || std::abs(currentLeft - left) > 0.25) {
        controls::Canvas::SetLeft(element, left);
    }
    if (std::isnan(currentTop) || std::abs(currentTop - top) > 0.25) {
        controls::Canvas::SetTop(element, top);
    }
}

void ApplyLayoutRect(FrameworkElement const& element,
                     ScaleTransform const& scale,
                     TranslateTransform const& translate,
                     RECT const& currentScreen,
                     RECT const& popupScreen) {
    if (!element) {
        return;
    }

    double currentWidth = std::max(1.0, static_cast<double>(currentScreen.right - currentScreen.left));
    double currentHeight = std::max(1.0, static_cast<double>(currentScreen.bottom - currentScreen.top));
    SetElementSizeIfChanged(element, currentWidth, currentHeight);
    SetCanvasPositionIfChanged(element,
                               currentScreen.left - popupScreen.left,
                               currentScreen.top - popupScreen.top);
    if (scale) {
        scale.ScaleX(1.0);
        scale.ScaleY(1.0);
    }
    if (translate) {
        translate.X(0.0);
        translate.Y(0.0);
    }
}

bool IsDarkModeApprox() {
    HKEY key = nullptr;
    DWORD value = 1;
    DWORD size = sizeof(value);
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                      0,
                      KEY_READ,
                      &key) == ERROR_SUCCESS) {
        RegQueryValueExW(key, L"AppsUseLightTheme", nullptr, nullptr,
                         reinterpret_cast<LPBYTE>(&value), &size);
        RegCloseKey(key);
    }

    return value == 0;
}

bool IsLiquidGlassMaterial() {
    return g_settings.material == L"liquid_glass";
}

bool IsBlurredGlassMaterial() {
    return g_settings.material == L"acrylic" || IsLiquidGlassMaterial();
}

winrt::Windows::UI::Color IslandBackgroundColor() {
    bool dark = IsDarkModeApprox();
    if (g_settings.material == L"mica_like") {
        return dark ? Color(0xC8, 0x2A, 0x2A, 0x2F)
                    : Color(0xD4, 0xF3, 0xF3, 0xF6);
    }

    if (IsLiquidGlassMaterial()) {
        return dark ? Color(0xC8, 0x18, 0x1A, 0x20)
                    : Color(0xDE, 0xFB, 0xFC, 0xFF);
    }

    if (g_settings.material == L"acrylic") {
        // XAML AcrylicBrush can switch to its fallback when the taskbar/XAML
        // island loses focus, which makes the compact island suddenly look gray
        // and dirty. The expanded acrylic path uses a native overlay instead;
        // keep the compact surface visually stable with a tinted brush.
        return dark ? Color(0xD8, 0x20, 0x20, 0x24)
                    : Color(0xEA, 0xF8, 0xF8, 0xFA);
    }

    return dark ? Color(0xE8, 0x20, 0x20, 0x24)
                : Color(0xE8, 0xF4, 0xF4, 0xF6);
}

mediax::Brush IslandBackgroundBrush() {
    return Brush(IslandBackgroundColor());
}

mediax::Brush LiquidGlassRimHighlightBrush() {
    bool dark = IsDarkModeApprox();
    mediax::LinearGradientBrush brush;
    brush.StartPoint({0.0f, 0.0f});
    brush.EndPoint({0.0f, 1.0f});
    mediax::GradientStopCollection stops;

    auto addStop = [&](double offset, winrt::Windows::UI::Color const& color) {
        mediax::GradientStop stop;
        stop.Offset(offset);
        stop.Color(color);
        stops.Append(stop);
    };

    addStop(0.00, dark ? Color(0xB8, 0xFF, 0xFF, 0xFF)
                       : Color(0xD0, 0xFF, 0xFF, 0xFF));
    addStop(0.12, dark ? Color(0x70, 0xFF, 0xFF, 0xFF)
                       : Color(0x90, 0xFF, 0xFF, 0xFF));
    addStop(0.42, dark ? Color(0x1C, 0xFF, 0xFF, 0xFF)
                       : Color(0x28, 0xFF, 0xFF, 0xFF));
    addStop(0.64, dark ? Color(0x24, 0xFF, 0xFF, 0xFF)
                       : Color(0x36, 0xFF, 0xFF, 0xFF));
    addStop(0.88, dark ? Color(0x7A, 0xFF, 0xFF, 0xFF)
                       : Color(0x98, 0xFF, 0xFF, 0xFF));
    addStop(1.00, dark ? Color(0xAE, 0xFF, 0xFF, 0xFF)
                       : Color(0xC6, 0xFF, 0xFF, 0xFF));
    brush.GradientStops(stops);
    return brush;
}

mediax::Brush LiquidGlassSurfaceHighlightBrush() {
    bool dark = IsDarkModeApprox();
    mediax::LinearGradientBrush brush;
    brush.StartPoint({0.08f, 0.0f});
    brush.EndPoint({0.72f, 0.92f});
    mediax::GradientStopCollection stops;

    auto addStop = [&](double offset, winrt::Windows::UI::Color const& color) {
        mediax::GradientStop stop;
        stop.Offset(offset);
        stop.Color(color);
        stops.Append(stop);
    };

    addStop(0.00, dark ? Color(0x26, 0xFF, 0xFF, 0xFF)
                       : Color(0x70, 0xFF, 0xFF, 0xFF));
    addStop(0.16, dark ? Color(0x14, 0xFF, 0xFF, 0xFF)
                       : Color(0x44, 0xFF, 0xFF, 0xFF));
    addStop(0.48, Color(0x00, 0xFF, 0xFF, 0xFF));
    addStop(0.76, Color(0x00, 0xFF, 0xFF, 0xFF));
    addStop(1.00, dark ? Color(0x10, 0xFF, 0xFF, 0xFF)
                       : Color(0x32, 0xFF, 0xFF, 0xFF));
    brush.GradientStops(stops);
    return brush;
}

mediax::Brush IslandBorderBrush() {
    bool dark = IsDarkModeApprox();
    if (IsLiquidGlassMaterial()) {
        return LiquidGlassRimHighlightBrush();
    }

    return Brush(dark ? Color(0x36, 0xFF, 0xFF, 0xFF)
                      : Color(0x26, 0x00, 0x00, 0x00));
}

mediax::Brush CompactPlaybackControlStrokeBrush() {
    if (IsLiquidGlassMaterial()) {
        return LiquidGlassRimHighlightBrush();
    }

    bool dark = IsDarkModeApprox();
    return Brush(dark ? Color(0x20, 0xFF, 0xFF, 0xFF)
                      : Color(0x16, 0x00, 0x00, 0x00));
}

mediax::Brush PopupAlbumArtStrokeBrush() {
    if (IsLiquidGlassMaterial()) {
        return LiquidGlassRimHighlightBrush();
    }

    // Match the bright directional edge used by native Windows 11 surfaces.
    // This stroke always brightens instead of adapting to a dark color.
    bool dark = IsDarkModeApprox();
    mediax::LinearGradientBrush brush;
    brush.StartPoint({0.0f, 0.0f});
    brush.EndPoint({1.0f, 1.0f});
    mediax::GradientStopCollection stops;
    auto addStop = [&](double offset, BYTE alpha) {
        mediax::GradientStop stop;
        stop.Offset(offset);
        stop.Color(Color(alpha, 0xFF, 0xFF, 0xFF));
        stops.Append(stop);
    };
    addStop(0.00, dark ? 0x78 : 0xA0);
    addStop(0.46, dark ? 0x30 : 0x48);
    addStop(1.00, dark ? 0x58 : 0x70);
    brush.GradientStops(stops);
    return brush;
}

void ConfigureCompactAlbumArtStroke(Border const& stroke) {
    if (!stroke) {
        return;
    }

    // The stroke sits in Island_ArtShell, outside the clipped image host.
    // Keep it exactly aligned with the artwork bounds; otherwise the border
    // looks like it has drifted inward relative to the cover.
    double size = std::max(1.0, g_layout.artSize);
    double radius = std::max(1.0, g_layout.artCornerRadius);

    stroke.Width(size);
    stroke.Height(size);
    stroke.Margin({0, 0, 0, 0});
    stroke.HorizontalAlignment(HorizontalAlignment::Center);
    stroke.VerticalAlignment(VerticalAlignment::Center);
    stroke.CornerRadius({radius, radius, radius, radius});
    stroke.Background(Brush(Color(0x00, 0x00, 0x00, 0x00)));
    stroke.BorderThickness(IsLiquidGlassMaterial()
                               ? Thickness{1.35, 1.35, 1.35, 1.35}
                               : Thickness{1.2, 1.2, 1.2, 1.2});
    stroke.BorderBrush(PopupAlbumArtStrokeBrush());
    stroke.IsHitTestVisible(false);
}

std::vector<uint8_t> ReadThumbnailBytes(streams::IRandomAccessStreamReference const& thumbnail) {
    std::vector<uint8_t> bytes;
    if (!thumbnail) {
        return bytes;
    }

    try {
        auto stream = GetAsyncResultWithTimeout(
            thumbnail.OpenReadAsync(),
            kThumbnailAsyncTimeout,
            L"thumbnail OpenReadAsync");
        uint64_t size64 = stream.Size();
        if (size64 == 0 || size64 > 8 * 1024 * 1024) {
            return bytes;
        }

        uint32_t size = static_cast<uint32_t>(size64);
        auto buffer = GetAsyncResultWithTimeout(
            stream.ReadAsync(streams::Buffer(size), size,
                             streams::InputStreamOptions::None),
            kThumbnailAsyncTimeout,
            L"thumbnail ReadAsync");
        streams::DataReader reader = streams::DataReader::FromBuffer(buffer);
        bytes.resize(buffer.Length());
        if (!bytes.empty()) {
            reader.ReadBytes(winrt::array_view<uint8_t>(bytes));
        }
    } catch (...) {
        bytes.clear();
    }

    return bytes;
}

std::wstring GetStringSetting(const wchar_t* key, const wchar_t* fallback) {
    auto setting = WindhawkUtils::StringSetting::make(key);
    PCWSTR value = setting.get();
    return value && *value ? value : fallback;
}

constexpr int kSettingsMigrationVersion = 1;
constexpr wchar_t kMigrateMicaLikeMaterialValue[] =
    L"MigrateMicaLikeMaterialToAcrylic";

void ApplySettingsMigrations(Settings* settings) {
    int migratedVersion = Wh_GetIntValue(L"SettingsMigrationVersion", 0);
    if (migratedVersion < kSettingsMigrationVersion) {
        if (settings->material == L"mica_like") {
            Wh_SetIntValue(kMigrateMicaLikeMaterialValue, 1);
        }
        Wh_SetIntValue(L"SettingsMigrationVersion", kSettingsMigrationVersion);
    }

    if (Wh_GetIntValue(kMigrateMicaLikeMaterialValue, 0)) {
        if (settings->material == L"mica_like") {
            settings->material = L"acrylic";
        } else {
            Wh_SetIntValue(kMigrateMicaLikeMaterialValue, 0);
        }
    }
}

Settings ReadSettings() {
    Settings settings;
    settings.position = GetStringSetting(L"Main.Position", L"tray_left");
    settings.compactWidth = Clamp(Wh_GetIntSetting(L"Main.CompactWidth"), 96, 320);
    settings.autoSizeToTaskbar = Wh_GetIntSetting(L"Main.AutoSizeToTaskbar") != 0;
    settings.expandedWidth = Clamp(Wh_GetIntSetting(L"Main.ExpandedWidth"), 240, 640);
    settings.expandedHeight = Clamp(Wh_GetIntSetting(L"Main.ExpandedHeight"), 430, 760);
    settings.compact = Wh_GetIntSetting(L"Main.Compact") != 0;
    settings.popupSpacing = Clamp(Wh_GetIntSetting(L"Main.PopupSpacing"), 2, 40);
    settings.popupCardGap = Clamp(Wh_GetIntSetting(L"Main.PopupCardGap"), 0, 40);
    settings.popupShadowDepth = Clamp(Wh_GetIntSetting(L"Main.PopupShadowDepth"), 0, 128);
    settings.popupShadowOpacity = Clamp(Wh_GetIntSetting(L"Main.PopupShadowOpacity"), 0, 100);
    settings.popupButtonStyle = GetStringSetting(L"Main.PopupButtonStyle", L"fluent_bold");
    if (settings.popupButtonStyle != L"minimal_transport" &&
        settings.popupButtonStyle != L"fluent_bold") {
        settings.popupButtonStyle = L"minimal_transport";
    }
    settings.popupBackdropCoverEffect =
        GetStringSetting(L"Main.PopupBackdropCoverEffect", L"dark_only");
    if (settings.popupBackdropCoverEffect != L"off" &&
        settings.popupBackdropCoverEffect != L"dark_only" &&
        settings.popupBackdropCoverEffect != L"on") {
        settings.popupBackdropCoverEffect = L"dark_only";
    }
    settings.artworkAbstractMode =
        GetStringSetting(L"Main.ArtworkAbstractMode", L"browser_original");
    if (settings.artworkAbstractMode == L"off") {
        settings.artworkAbstractMode = L"browser_original";
    }
    if (settings.artworkAbstractMode != L"browser_original" &&
        settings.artworkAbstractMode != L"mesh_gradient" &&
        settings.artworkAbstractMode != L"energy_flame") {
        settings.artworkAbstractMode = L"browser_original";
    }
    settings.height = Clamp(Wh_GetIntSetting(L"Main.Height"), 32, 56);
    settings.marginLeft = Clamp(Wh_GetIntSetting(L"Main.MarginLeft"), 0, 48);
    settings.marginRight = Clamp(Wh_GetIntSetting(L"Main.MarginRight"), 0, 48);
    settings.hideWhenNoMedia = Wh_GetIntSetting(L"Main.HideWhenNoMedia") != 0;
    settings.hoverScale =
        static_cast<double>(Clamp(Wh_GetIntSetting(L"Main.HoverScale"), 100, 125)) / 100.0;
    settings.hoverLerpSpeed =
        static_cast<double>(Clamp(Wh_GetIntSetting(L"Main.HoverLerpSpeed"), 1, 80));
    settings.animationSpeed =
        static_cast<double>(Clamp(Wh_GetIntSetting(L"Main.AnimationSpeed"), 10, 400)) / 100.0;

    settings.material = GetStringSetting(L"Main.Material", L"liquid_glass");
    if (settings.material != L"mica_like" &&
        settings.material != L"solid" &&
        settings.material != L"acrylic" &&
        settings.material != L"liquid_glass") {
        settings.material = L"liquid_glass";
    }
    ApplySettingsMigrations(&settings);
    settings.backdropInitialFrameSkip =
        Clamp(Wh_GetIntSetting(L"Main.BackdropInitialFrameSkip"), 1, 3);
    settings.backdropFallbackBlurPasses =
        Clamp(Wh_GetIntSetting(L"Main.BackdropFallbackBlurPasses"), 4, 6);
    settings.backdropFallbackCaptureScale =
        Clamp(Wh_GetIntSetting(L"Main.BackdropFallbackCaptureScale"), 2, 3);
    settings.backdropWgcBlurStdDev =
        Clamp(Wh_GetIntSetting(L"Main.BackdropWgcBlurStdDev"), 14, 22);
    settings.allowScreenCapture =
        Wh_GetIntSetting(L"Main.AllowScreenCapture") != 0;
    return settings;
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND result = nullptr;
    EnumWindows([](HWND hwnd, LPARAM param) -> BOOL {
        DWORD pid = 0;
        wchar_t className[64]{};
        if (GetWindowThreadProcessId(hwnd, &pid) &&
            pid == GetCurrentProcessId() &&
            GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
            _wcsicmp(className, L"Shell_TrayWnd") == 0) {
            *reinterpret_cast<HWND*>(param) = hwnd;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&result));
    return result;
}

bool RunFromWindowThread(HWND hwnd, WindowThreadProc proc, void* param) {
    static const UINT msg = RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Payload {
        WindowThreadProc proc;
        void* param;
    };

    DWORD tid = GetWindowThreadProcessId(hwnd, nullptr);
    if (!tid) {
        return false;
    }

    if (tid == GetCurrentThreadId()) {
        try {
            proc(param);
        } catch (...) {
            Wh_Log(L"Island: exception in taskbar-thread callback");
            return false;
        }
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                auto cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                static const UINT innerMsg =
                    RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
                if (cwp->message == innerMsg) {
                    auto payload = reinterpret_cast<Payload*>(cwp->lParam);
                    try {
                        payload->proc(payload->param);
                    } catch (...) {
                        Wh_Log(L"Island: exception in taskbar-thread hook callback");
                    }
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr,
        tid);

    if (!hook) {
        return false;
    }

    Payload payload{proc, param};
    SendMessageW(hwnd, msg, 0, reinterpret_cast<LPARAM>(&payload));
    UnhookWindowsHookEx(hook);
    return true;
}

xaml::XamlRoot GetTaskbarXamlRoot(HWND taskbarWnd) {
    HWND taskSwWnd = reinterpret_cast<HWND>(GetPropW(taskbarWnd, L"TaskbandHWND"));
    if (!taskSwWnd) {
        return nullptr;
    }

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(taskSwWnd, 0));
    void* taskBandForSite = taskBand;
    for (int i = 0; i < 20; ++i) {
        if (*reinterpret_cast<void**>(taskBandForSite) == CTaskBand_ITaskListWndSite_vftable) {
            break;
        }
        taskBandForSite = reinterpret_cast<void**>(taskBandForSite) + 1;
        if (i == 19) {
            return nullptr;
        }
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForSite, taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t elementOffset = 0x10;
#if defined(_M_X64) || defined(__x86_64__)
    const BYTE* bytes = reinterpret_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
    if (bytes[0] == 0x48 && bytes[1] == 0x83 && bytes[2] == 0xEC &&
        bytes[4] == 0x48 && bytes[5] == 0x83 && bytes[6] == 0xC1 &&
        bytes[7] <= 0x7F) {
        elementOffset = bytes[7];
    }
#elif defined(_M_ARM64) || defined(__aarch64__)
    const DWORD* words = reinterpret_cast<const DWORD*>(TaskbarHost_FrameHeight_Original);
    if (words[0] == 0xD503237F && (words[1] & 0xFFC07FFF) == 0xA9807BFD &&
        words[2] == 0x910003FD && (words[3] & 0xFFF00FE0) == 0xF8400C00) {
        elementOffset = (words[3] >> 12) & 0xFF;
    }
#endif

    auto elementUnknown =
        *reinterpret_cast<IUnknown**>(reinterpret_cast<BYTE*>(taskbarHostSharedPtr[0]) + elementOffset);

    FrameworkElement taskbarElement{nullptr};
    if (elementUnknown) {
        elementUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                       winrt::put_abi(taskbarElement));
    }

    auto root = taskbarElement ? taskbarElement.XamlRoot() : nullptr;
    if (taskbarHostSharedPtr[1] && Std_Ref_Decref_Original) {
        Std_Ref_Decref_Original(taskbarHostSharedPtr[1]);
    }
    return root;
}

FrameworkElement FindChildByName(FrameworkElement const& root, std::wstring_view name, int depth = 32) {
    if (!root || depth <= 0) {
        return nullptr;
    }

    int count = VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < count; ++i) {
        auto child = VisualTreeHelper::GetChild(root, i).try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (child.Name() == name) {
            return child;
        }
        if (auto found = FindChildByName(child, name, depth - 1)) {
            return found;
        }
    }
    return nullptr;
}

Grid FindTaskbarRootGrid(FrameworkElement const& root) {
    auto frame = FindChildByName(root, L"TaskbarFrame");
    if (!frame) {
        frame = FindChildByName(root, L"TaskbarFrameRepeater");
    }

    auto rootGrid = frame ? FindChildByName(frame, L"RootGrid") : nullptr;
    if (!rootGrid) {
        rootGrid = FindChildByName(root, L"RootGrid");
    }
    return rootGrid ? rootGrid.try_as<Grid>() : nullptr;
}

struct InjectionTarget {
    Grid grid = nullptr;
    int column = -1;
    bool overlay = false;
};

InjectionTarget ResolveInjectionTarget(FrameworkElement const& root) {
    auto trayFrame = FindChildByName(root, L"SystemTrayFrameGrid");
    if (auto trayGrid = trayFrame ? trayFrame.try_as<Grid>() : nullptr) {
        int col = -1;
        if (g_settings.position == L"tray_left") {
            col = 0;
        } else if (g_settings.position == L"tray_right") {
            col = static_cast<int>(trayGrid.ColumnDefinitions().Size());
        } else if (g_settings.position == L"tray_before_clock") {
            auto clock = FindChildByName(trayGrid, L"NotificationCenterButton");
            if (!clock) {
                clock = FindChildByName(root, L"NotificationCenterButton");
            }
            col = clock ? controls::Grid::GetColumn(clock) : -1;
        } else if (g_settings.position == L"tray_after_clock") {
            auto showDesktop = FindChildByName(trayGrid, L"ShowDesktopStack");
            if (!showDesktop) {
                showDesktop = FindChildByName(root, L"ShowDesktopStack");
            }
            col = showDesktop ? controls::Grid::GetColumn(showDesktop) : -1;
        }

        if (col >= 0) {
            return {trayGrid, col, false};
        }
    }

    auto rootGrid = FindTaskbarRootGrid(root);
    if (rootGrid) {
        return {rootGrid, 0, true};
    }

    return {};
}

uint64_t MediaThumbFingerprint(std::vector<uint8_t> const& bytes) {
    if (bytes.empty()) {
        return 0;
    }

    uint64_t hash = 1469598103934665603ull;
    size_t step = std::max<size_t>(1, bytes.size() / 128);
    for (size_t i = 0; i < bytes.size(); i += step) {
        hash ^= bytes[i];
        hash *= 1099511628211ull;
    }
    hash ^= static_cast<uint64_t>(bytes.size());
    hash *= 1099511628211ull;
    return hash;
}

std::wstring MediaIdentityKey(MediaState const& state) {
    if (!state.hasSession) {
        return L"";
    }

    // Deliberately excludes artwork bytes. Browsers can emit metadata before a
    // live-room/video thumbnail is available, and some providers temporarily
    // repeat the previous thumbnail. This key represents the actual media item
    // for deciding whether old artwork can be reused safely.
    return state.sourceAppUserModelId + L"\n" +
           state.title + L"\n" +
           state.artist + L"\n" +
           std::to_wstring(state.durationTicks);
}

bool LooksLikeNativeMusicMediaSource(std::wstring const& source) {
    std::wstring lower;
    lower.reserve(source.size());
    for (wchar_t ch : source) {
        lower.push_back(static_cast<wchar_t>(std::towlower(ch)));
    }

    // Explicitly keep native music players out of the browser/live-thumbnail
    // path. Some providers use AppUserModelIds that contain generic substrings
    // which can otherwise be over-matched by the browser heuristics below.
    return lower.find(L"qqmusic") != std::wstring::npos ||
           lower.find(L"qq音乐") != std::wstring::npos ||
           lower.find(L"tencent.qqmusic") != std::wstring::npos ||
           lower.find(L"tencentmusic") != std::wstring::npos ||
           lower.find(L"spotify") != std::wstring::npos ||
           lower.find(L"applemusic") != std::wstring::npos ||
           lower.find(L"music.ui") != std::wstring::npos ||
           lower.find(L"zunemusic") != std::wstring::npos;
}

bool LooksLikeBrowserMediaSource(std::wstring const& source) {
    std::wstring lower;
    lower.reserve(source.size());
    for (wchar_t ch : source) {
        lower.push_back(static_cast<wchar_t>(std::towlower(ch)));
    }

    if (LooksLikeNativeMusicMediaSource(source)) {
        return false;
    }

    return lower.find(L"chrome") != std::wstring::npos ||
           lower.find(L"chromium") != std::wstring::npos ||
           lower.find(L"msedge") != std::wstring::npos ||
           lower.find(L"microsoftedge") != std::wstring::npos ||
           lower.find(L"firefox") != std::wstring::npos ||
           lower.find(L"brave") != std::wstring::npos ||
           lower.find(L"opera") != std::wstring::npos ||
           lower.find(L"vivaldi") != std::wstring::npos ||
           lower.find(L"arc") != std::wstring::npos ||
           lower.find(L"browser") != std::wstring::npos ||
           lower.find(L"zen") != std::wstring::npos ||
           lower.find(L"floorp") != std::wstring::npos ||
           lower.find(L"librewolf") != std::wstring::npos ||
           lower.find(L"yandex") != std::wstring::npos ||
           lower.find(L"safari") != std::wstring::npos;
}

std::wstring MediaContentKey(MediaState const& state) {
    if (!state.hasSession) {
        return L"";
    }
    return MediaIdentityKey(state) + L"\n" +
           std::to_wstring(MediaThumbFingerprint(state.thumbnailBytes));
}

MediaState SnapshotMedia() {
    std::lock_guard lock(g_mediaMutex);
    return g_media;
}

MediaState SnapshotMediaWithTimestamp(std::chrono::steady_clock::time_point& timestamp) {
    std::lock_guard lock(g_mediaMutex);
    timestamp = g_mediaStateTimestamp;
    return g_media;
}

void SetMedia(MediaState&& state) {
    std::lock_guard lock(g_mediaMutex);
    auto now = std::chrono::steady_clock::now();

    std::wstring currentKey = MediaContentKey(g_media);
    std::wstring nextKey = MediaContentKey(state);

    bool sameMedia = g_media.hasSession && state.hasSession &&
                     !currentKey.empty() && currentKey == nextKey &&
                     state.durationTicks > 0 && g_media.durationTicks == state.durationTicks;

    {
        std::lock_guard seekLock(g_seekMutex);
        if (g_popupSeekCommitPending) {
            if (now >= g_popupSeekCommitUntil) {
                g_popupSeekCommitPending = false;
                g_popupSeekCommitTargetTicks = 0;
            } else if (sameMedia) {
                int64_t targetTicks = std::clamp<int64_t>(
                    g_popupSeekCommitTargetTicks, 0, state.durationTicks);
                int64_t delta = state.positionTicks > targetTicks
                                    ? state.positionTicks - targetTicks
                                    : targetTicks - state.positionTicks;
                int64_t tolerance =
                    std::max<int64_t>(10000000LL, state.durationTicks / 240);
                if (delta <= tolerance) {
                    g_popupSeekCommitPending = false;
                    g_popupSeekCommitTargetTicks = 0;
                } else {
                    // Some providers briefly report the pre-seek position. Keep the
                    // committed local position until the provider catches up.
                    state.positionTicks = targetTicks;
                }
            }
        }
    }

    if (sameMedia && state.isPlaying) {
        int64_t estimatedTicks = g_media.positionTicks;
        if (g_media.isPlaying && g_mediaStateTimestamp.time_since_epoch().count() != 0) {
            double ageSec = std::chrono::duration_cast<std::chrono::milliseconds>(
                                now - g_mediaStateTimestamp)
                                .count() /
                            1000.0;
            ageSec = Clamp(ageSec, 0.0, 3.0);
            estimatedTicks += static_cast<int64_t>(ageSec * 10000000.0);
            estimatedTicks = std::clamp<int64_t>(estimatedTicks, 0, state.durationTicks);
        }

        // Some GSMTC providers re-emit the old position when metadata is polled.
        // If we accepted those samples, the meteor tail would jump left and then
        // right again on the next local extrapolation.
        bool looksLikeStaleBackwardSample =
            state.positionTicks < estimatedTicks &&
            (estimatedTicks - state.positionTicks) < 120000000LL; // not a real big seek
        if (looksLikeStaleBackwardSample) {
            state.positionTicks = estimatedTicks;
        }
    }

    g_media = std::move(state);
    g_mediaStateTimestamp = now;
}

void RequestMediaRefresh() {
    if (!IsModActive() || !g_mediaThreadRunning.load()) {
        return;
    }
    g_mediaRefreshRequested = true;
    g_mediaCommandCv.notify_one();
}

gsm::GlobalSystemMediaTransportControlsSessionManager
RequestMediaManagerWithTimeout(std::chrono::milliseconds timeout) {
    if (g_unloading) {
        return nullptr;
    }

    try {
        auto operation =
            gsm::GlobalSystemMediaTransportControlsSessionManager::RequestAsync();
        if (operation.wait_for(timeout) !=
            winrt::Windows::Foundation::AsyncStatus::Completed) {
            try {
                operation.Cancel();
            } catch (...) {
            }
            return nullptr;
        }
        return operation.GetResults();
    } catch (...) {
        return nullptr;
    }
}

gsm::GlobalSystemMediaTransportControlsSession SelectBestSession(
    gsm::GlobalSystemMediaTransportControlsSessionManager const& manager) {
    try {
        if (!manager) {
            return nullptr;
        }

        auto current = manager.GetCurrentSession();
        gsm::GlobalSystemMediaTransportControlsSession first{nullptr};
        gsm::GlobalSystemMediaTransportControlsSession firstPlaying{nullptr};
        for (auto const& session : manager.GetSessions()) {
            if (!first) {
                first = session;
            }

            bool isPlaying = false;
            try {
                isPlaying =
                    session.GetPlaybackInfo().PlaybackStatus() ==
                    gsm::GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
            } catch (...) {
            }
            if (!isPlaying) {
                continue;
            }

            if (current &&
                winrt::get_abi(session) == winrt::get_abi(current)) {
                return session;
            }
            if (!firstPlaying) {
                firstPlaying = session;
            }
        }

        if (firstPlaying) {
            return firstPlaying;
        }
        return current ? current : first;
    } catch (...) {
        return nullptr;
    }
}

gsm::GlobalSystemMediaTransportControlsSession CurrentSession() {
    try {
        auto manager = RequestMediaManagerWithTimeout(std::chrono::seconds(2));
        return SelectBestSession(manager);
    } catch (...) {
        return nullptr;
    }
}

gsm::GlobalSystemMediaTransportControlsSession CurrentSessionFromManager(
    gsm::GlobalSystemMediaTransportControlsSessionManager const& manager) {
    try {
        if (manager) {
            return SelectBestSession(manager);
        }
    } catch (...) {
    }
    return CurrentSession();
}

void RefreshMediaState(
    gsm::GlobalSystemMediaTransportControlsSessionManager const& manager = nullptr) {
    MediaState state;
    try {
        auto session = CurrentSessionFromManager(manager);
        if (session) {
            state.hasSession = true;
            try {
                state.sourceAppUserModelId = std::wstring(session.SourceAppUserModelId());
            } catch (...) {
                state.sourceAppUserModelId.clear();
            }
            auto playback = session.GetPlaybackInfo();
            state.isPlaying =
                playback.PlaybackStatus() ==
                gsm::GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
            try {
                state.canSeek =
                    playback.Controls().IsPlaybackPositionEnabled();
            } catch (...) {
                state.canSeek = false;
            }

            auto props = GetAsyncResultWithTimeout(
                session.TryGetMediaPropertiesAsync(),
                kMediaPropertiesAsyncTimeout,
                L"TryGetMediaPropertiesAsync");
            state.title = props.Title().empty() ? L"Unknown title" : std::wstring(props.Title());
            state.artist = props.Artist().empty() ? std::wstring(props.AlbumArtist())
                                                  : std::wstring(props.Artist());
            state.thumbnailBytes = ReadThumbnailBytes(props.Thumbnail());

            auto timeline = session.GetTimelineProperties();
            int64_t start = timeline.StartTime().count();
            int64_t end = timeline.EndTime().count();
            int64_t position = timeline.Position().count();
            state.timelineStartTicks = start;
            state.durationTicks = end > start ? end - start : 0;
            state.positionTicks = state.durationTicks > 0
                                      ? std::clamp<int64_t>(
                                            position - start, 0,
                                            state.durationTicks)
                                      : 0;

        }
    } catch (...) {
        state = {};
    }
    SetMedia(std::move(state));
}

void RunMediaCommand(MediaCommand command) {
    if (!command || !IsModActive() || !g_mediaThreadRunning.load()) {
        return;
    }

    {
        std::lock_guard lock(g_mediaCommandMutex);
        g_mediaCommands.push_back(std::move(command));
    }
    g_mediaCommandCv.notify_one();
}

void SeekToMediaPosition(double ratio) {
    MediaState state = SnapshotMedia();
    if (!state.hasSession || state.durationTicks <= 0) {
        return;
    }

    ratio = Clamp(ratio, 0.0, 1.0);
    int64_t relativePositionTicks = static_cast<int64_t>(
        std::llround(static_cast<double>(state.durationTicks) * ratio));
    int64_t absolutePositionTicks =
        state.timelineStartTicks + relativePositionTicks;
    bool providerReportsSeekSupport = state.canSeek;
    RunMediaCommand([absolutePositionTicks, relativePositionTicks,
                     providerReportsSeekSupport, ratio](
                        gsm::GlobalSystemMediaTransportControlsSession const& session) {
        try {
            auto source = session.SourceAppUserModelId();
            bool isAppleMusic =
                std::wstring_view(source).find(L"AppleMusic") !=
                std::wstring_view::npos;
            if (isAppleMusic && !providerReportsSeekSupport &&
                TrySeekAppleMusicWithUiAutomation(ratio)) {
                return;
            }

            bool changed = GetAsyncResultWithTimeout(
                session.TryChangePlaybackPositionAsync(relativePositionTicks),
                kMediaCommandAsyncTimeout,
                L"TryChangePlaybackPositionAsync relative");
            if (!changed && absolutePositionTicks != relativePositionTicks) {
                changed = GetAsyncResultWithTimeout(
                    session.TryChangePlaybackPositionAsync(absolutePositionTicks),
                    kMediaCommandAsyncTimeout,
                    L"TryChangePlaybackPositionAsync absolute");
            }
            Wh_Log(L"Island: seek source=%s result=%d advertised=%d relative=%lld absolute=%lld",
                   source.c_str(), changed,
                   providerReportsSeekSupport,
                   static_cast<long long>(relativePositionTicks),
                   static_cast<long long>(absolutePositionTicks));
        } catch (winrt::hresult_error const& error) {
            Wh_Log(L"Island: seek source=%s failed=0x%08X",
                   session.SourceAppUserModelId().c_str(),
                   static_cast<unsigned>(error.code().value));
        }
    });
}

uint64_t ThumbnailHash(std::vector<uint8_t> const& bytes) {
    if (bytes.empty()) {
        return 0;
    }

    uint64_t hash = 1469598103934665603ull;
    for (uint8_t byte : bytes) {
        hash ^= byte;
        hash *= 1099511628211ull;
    }
    return hash;
}

HBITMAP DecodeAlbumBitmap(std::vector<uint8_t> const& bytes, UINT size) {
    if (bytes.empty() || size == 0) {
        return nullptr;
    }

    IStream* stream = SHCreateMemStream(bytes.data(), static_cast<UINT>(bytes.size()));
    IWICImagingFactory* factory = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICBitmapScaler* scaler = nullptr;
    IWICFormatConverter* converter = nullptr;
    HBITMAP bitmap = nullptr;

    HRESULT hr = stream ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(&factory));
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateDecoderFromStream(stream, nullptr,
                                              WICDecodeMetadataCacheOnLoad, &decoder);
    }
    if (SUCCEEDED(hr)) {
        hr = decoder->GetFrame(0, &frame);
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateBitmapScaler(&scaler);
    }
    if (SUCCEEDED(hr)) {
        hr = scaler->Initialize(frame, size, size, WICBitmapInterpolationModeFant);
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateFormatConverter(&converter);
    }
    if (SUCCEEDED(hr)) {
        hr = converter->Initialize(scaler, GUID_WICPixelFormat32bppPBGRA,
                                   WICBitmapDitherTypeNone, nullptr, 0,
                                   WICBitmapPaletteTypeCustom);
    }

    void* pixels = nullptr;
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = static_cast<LONG>(size);
    info.bmiHeader.biHeight = -static_cast<LONG>(size);
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    if (SUCCEEDED(hr)) {
        bitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &pixels, nullptr, 0);
        if (!bitmap || !pixels) {
            hr = E_OUTOFMEMORY;
        }
    }
    if (SUCCEEDED(hr)) {
        hr = converter->CopyPixels(nullptr, size * 4, size * size * 4,
                                   static_cast<BYTE*>(pixels));
    }
    if (FAILED(hr) && bitmap) {
        DeleteObject(bitmap);
        bitmap = nullptr;
    }

    if (converter) converter->Release();
    if (scaler) scaler->Release();
    if (frame) frame->Release();
    if (decoder) decoder->Release();
    if (factory) factory->Release();
    if (stream) stream->Release();
    return bitmap;
}

std::vector<uint8_t> CreateLowDetailAlbumCoverBytes(std::vector<uint8_t> const& bytes,
                                                     bool edgeFadeToMiddle = false,
                                                     bool fadeFromTop = false,
                                                     bool fadeFromRight = false) {
    std::vector<uint8_t> output;
    if (bytes.empty()) {
        return output;
    }

    // Keep this layer as a soft color wash. Slightly stronger than the
    // previous C-polish pass, but not so smeared that it loses all album
    // character.
    constexpr UINT kLowDetailSize = 20;
    constexpr UINT kBlurRadius = 1;
    constexpr int kBlurPasses = 3;
    const UINT stride = kLowDetailSize * 4;
    const UINT bufferSize = stride * kLowDetailSize;

    IStream* stream = SHCreateMemStream(bytes.data(), static_cast<UINT>(bytes.size()));
    IWICImagingFactory* factory = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICBitmapScaler* scaler = nullptr;
    IWICFormatConverter* converter = nullptr;
    IStream* outStream = nullptr;
    IWICBitmapEncoder* encoder = nullptr;
    IWICBitmapFrameEncode* outFrame = nullptr;
    IPropertyBag2* propertyBag = nullptr;

    HRESULT hr = stream ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(&factory));
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateDecoderFromStream(stream, nullptr,
                                              WICDecodeMetadataCacheOnLoad, &decoder);
    }
    if (SUCCEEDED(hr)) {
        hr = decoder->GetFrame(0, &frame);
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateBitmapScaler(&scaler);
    }
    if (SUCCEEDED(hr)) {
        hr = scaler->Initialize(frame, kLowDetailSize, kLowDetailSize,
                                WICBitmapInterpolationModeFant);
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateFormatConverter(&converter);
    }
    if (SUCCEEDED(hr)) {
        hr = converter->Initialize(scaler, GUID_WICPixelFormat32bppPBGRA,
                                   WICBitmapDitherTypeNone, nullptr, 0,
                                   WICBitmapPaletteTypeCustom);
    }

    std::vector<BYTE> pixels(bufferSize);
    if (SUCCEEDED(hr)) {
        hr = converter->CopyPixels(nullptr, stride, bufferSize, pixels.data());
    }

    auto boxBlurPass = [&] {
        std::vector<BYTE> source = pixels;
        for (UINT y = 0; y < kLowDetailSize; ++y) {
            for (UINT x = 0; x < kLowDetailSize; ++x) {
                int sumB = 0;
                int sumG = 0;
                int sumR = 0;
                int sumA = 0;
                int count = 0;
                int left = std::max(0, static_cast<int>(x) - static_cast<int>(kBlurRadius));
                int right = std::min(static_cast<int>(kLowDetailSize) - 1,
                                     static_cast<int>(x) + static_cast<int>(kBlurRadius));
                int top = std::max(0, static_cast<int>(y) - static_cast<int>(kBlurRadius));
                int bottom = std::min(static_cast<int>(kLowDetailSize) - 1,
                                      static_cast<int>(y) + static_cast<int>(kBlurRadius));
                for (int yy = top; yy <= bottom; ++yy) {
                    for (int xx = left; xx <= right; ++xx) {
                        BYTE* pixel = source.data() +
                            (static_cast<size_t>(yy) * kLowDetailSize + xx) * 4;
                        sumB += pixel[0];
                        sumG += pixel[1];
                        sumR += pixel[2];
                        sumA += pixel[3];
                        ++count;
                    }
                }
                BYTE* target = pixels.data() +
                    (static_cast<size_t>(y) * kLowDetailSize + x) * 4;
                target[0] = static_cast<BYTE>(sumB / count);
                target[1] = static_cast<BYTE>(sumG / count);
                target[2] = static_cast<BYTE>(sumR / count);
                target[3] = static_cast<BYTE>(sumA / count);
            }
        }
    };
    if (SUCCEEDED(hr)) {
        for (int pass = 0; pass < kBlurPasses; ++pass) {
            boxBlurPass();
        }
    }

    if (SUCCEEDED(hr) && edgeFadeToMiddle) {
        // Expanded-surface color wash: keep only a low-frequency album glow
        // near the selected edge and ease it to transparent at the middle.
        const double kEdgeAlpha = IsDarkModeApprox() ? 0.72 : 0.88;
        const double middle = (static_cast<double>(kLowDetailSize) - 1.0) * 0.50;
        const double edge = static_cast<double>(kLowDetailSize) - 1.0;
        for (UINT y = 0; y < kLowDetailSize; ++y) {
            for (UINT x = 0; x < kLowDetailSize; ++x) {
                double edgePosition =
                    fadeFromRight
                        ? static_cast<double>(x)
                        : (fadeFromTop ? edge - static_cast<double>(y)
                                       : static_cast<double>(y));
                double amount = 0.0;
                if (edgePosition > middle && edge > middle) {
                    amount = (edgePosition - middle) / (edge - middle);
                    amount = Clamp(amount, 0.0, 1.0);
                    amount = amount * amount * (3.0 - 2.0 * amount);
                }
                double alphaScale = kEdgeAlpha * amount;
                BYTE* pixel = pixels.data() +
                    (static_cast<size_t>(y) * kLowDetailSize + x) * 4;
                pixel[3] = static_cast<BYTE>(std::lround(pixel[3] * alphaScale));
                pixel[0] = static_cast<BYTE>(std::lround(pixel[0] * alphaScale));
                pixel[1] = static_cast<BYTE>(std::lround(pixel[1] * alphaScale));
                pixel[2] = static_cast<BYTE>(std::lround(pixel[2] * alphaScale));
            }
        }
    }

    if (SUCCEEDED(hr)) {
        hr = CreateStreamOnHGlobal(nullptr, TRUE, &outStream);
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &encoder);
    }
    if (SUCCEEDED(hr)) {
        hr = encoder->Initialize(outStream, WICBitmapEncoderNoCache);
    }
    if (SUCCEEDED(hr)) {
        hr = encoder->CreateNewFrame(&outFrame, &propertyBag);
    }
    if (SUCCEEDED(hr)) {
        hr = outFrame->Initialize(propertyBag);
    }
    if (SUCCEEDED(hr)) {
        hr = outFrame->SetSize(kLowDetailSize, kLowDetailSize);
    }
    WICPixelFormatGUID pixelFormat = GUID_WICPixelFormat32bppPBGRA;
    if (SUCCEEDED(hr)) {
        hr = outFrame->SetPixelFormat(&pixelFormat);
    }
    if (SUCCEEDED(hr)) {
        hr = outFrame->WritePixels(kLowDetailSize, stride, bufferSize, pixels.data());
    }
    if (SUCCEEDED(hr)) {
        hr = outFrame->Commit();
    }
    if (SUCCEEDED(hr)) {
        hr = encoder->Commit();
    }
    if (SUCCEEDED(hr)) {
        HGLOBAL memory = nullptr;
        hr = GetHGlobalFromStream(outStream, &memory);
        if (SUCCEEDED(hr) && memory) {
            SIZE_T size = GlobalSize(memory);
            void* data = GlobalLock(memory);
            if (data && size > 0) {
                auto first = static_cast<uint8_t*>(data);
                output.assign(first, first + size);
            }
            if (data) {
                GlobalUnlock(memory);
            }
        }
    }

    if (propertyBag) propertyBag->Release();
    if (outFrame) outFrame->Release();
    if (encoder) encoder->Release();
    if (outStream) outStream->Release();
    if (converter) converter->Release();
    if (scaler) scaler->Release();
    if (frame) frame->Release();
    if (decoder) decoder->Release();
    if (factory) factory->Release();
    if (stream) stream->Release();
    return output;
}

winrt::Windows::UI::Color DefaultPopupAccentColor() {
    return Color(0xFF, 0x4F, 0x7D, 0xE8);
}

std::vector<uint8_t> EncodePbgraPngBytes(UINT width, UINT height, std::vector<BYTE> const& pixels) {
    std::vector<uint8_t> output;
    if (width == 0 || height == 0 || pixels.size() < static_cast<size_t>(width) * height * 4) {
        return output;
    }

    IWICImagingFactory* factory = nullptr;
    IStream* outStream = nullptr;
    IWICBitmapEncoder* encoder = nullptr;
    IWICBitmapFrameEncode* outFrame = nullptr;
    IPropertyBag2* propertyBag = nullptr;

    HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&factory));
    if (SUCCEEDED(hr)) {
        hr = CreateStreamOnHGlobal(nullptr, TRUE, &outStream);
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &encoder);
    }
    if (SUCCEEDED(hr)) {
        hr = encoder->Initialize(outStream, WICBitmapEncoderNoCache);
    }
    if (SUCCEEDED(hr)) {
        hr = encoder->CreateNewFrame(&outFrame, &propertyBag);
    }
    if (SUCCEEDED(hr)) {
        hr = outFrame->Initialize(propertyBag);
    }
    if (SUCCEEDED(hr)) {
        hr = outFrame->SetSize(width, height);
    }
    WICPixelFormatGUID pixelFormat = GUID_WICPixelFormat32bppPBGRA;
    if (SUCCEEDED(hr)) {
        hr = outFrame->SetPixelFormat(&pixelFormat);
    }
    if (SUCCEEDED(hr)) {
        hr = outFrame->WritePixels(height, width * 4,
                                   width * height * 4,
                                   const_cast<BYTE*>(pixels.data()));
    }
    if (SUCCEEDED(hr)) {
        hr = outFrame->Commit();
    }
    if (SUCCEEDED(hr)) {
        hr = encoder->Commit();
    }
    if (SUCCEEDED(hr)) {
        HGLOBAL memory = nullptr;
        hr = GetHGlobalFromStream(outStream, &memory);
        if (SUCCEEDED(hr) && memory) {
            SIZE_T size = GlobalSize(memory);
            void* data = GlobalLock(memory);
            if (data && size > 0) {
                auto first = static_cast<uint8_t*>(data);
                output.assign(first, first + size);
            }
            if (data) {
                GlobalUnlock(memory);
            }
        }
    }

    if (propertyBag) propertyBag->Release();
    if (outFrame) outFrame->Release();
    if (encoder) encoder->Release();
    if (outStream) outStream->Release();
    if (factory) factory->Release();
    return output;
}

bool PopupCoverG2MaskContains(double px, double py, UINT size, double radius) {
    if (size == 0 || radius <= 0.0) {
        return true;
    }

    const double maxCoord = static_cast<double>(size);
    double dx = 0.0;
    double dy = 0.0;

    if (px < radius) {
        dx = radius - px;
    } else if (px > maxCoord - radius) {
        dx = px - (maxCoord - radius);
    }

    if (py < radius) {
        dy = radius - py;
    } else if (py > maxCoord - radius) {
        dy = py - (maxCoord - radius);
    }

    if (dx <= 0.0 || dy <= 0.0) {
        return true;
    }

    const double nx = dx / radius;
    const double ny = dy / radius;
    const double superellipse = nx * nx * nx * nx + ny * ny * ny * ny;
    return superellipse <= 1.0;
}

double PopupCoverG2MaskScale(UINT x, UINT y, UINT size, double radius) {
    constexpr int kSamplesPerAxis = 4;
    int coveredSamples = 0;
    for (int sampleY = 0; sampleY < kSamplesPerAxis; ++sampleY) {
        double py = static_cast<double>(y) +
                    (static_cast<double>(sampleY) + 0.5) /
                        static_cast<double>(kSamplesPerAxis);
        for (int sampleX = 0; sampleX < kSamplesPerAxis; ++sampleX) {
            double px = static_cast<double>(x) +
                        (static_cast<double>(sampleX) + 0.5) /
                            static_cast<double>(kSamplesPerAxis);
            if (PopupCoverG2MaskContains(px, py, size, radius)) {
                ++coveredSamples;
            }
        }
    }

    return static_cast<double>(coveredSamples) /
           static_cast<double>(kSamplesPerAxis * kSamplesPerAxis);
}

void ApplyPopupCoverG2Mask(std::vector<BYTE>& pixels, UINT size) {
    if (size == 0 || pixels.size() < static_cast<size_t>(size) * size * 4) {
        return;
    }

    constexpr double kPopupCoverG2RadiusRatio = 0.118;
    const double radius = std::max(1.0, static_cast<double>(size) * kPopupCoverG2RadiusRatio);
    for (UINT y = 0; y < size; ++y) {
        for (UINT x = 0; x < size; ++x) {
            double alphaScale = PopupCoverG2MaskScale(x, y, size, radius);
            if (alphaScale >= 1.0) {
                continue;
            }

            BYTE* pixel = pixels.data() + (static_cast<size_t>(y) * size + x) * 4;
            pixel[0] = static_cast<BYTE>(std::lround(pixel[0] * alphaScale));
            pixel[1] = static_cast<BYTE>(std::lround(pixel[1] * alphaScale));
            pixel[2] = static_cast<BYTE>(std::lround(pixel[2] * alphaScale));
            pixel[3] = static_cast<BYTE>(std::lround(pixel[3] * alphaScale));
        }
    }
}

std::vector<uint8_t> CreatePopupG2AlbumCoverBytes(std::vector<uint8_t> const& bytes, UINT size = 512) {
    std::vector<uint8_t> output;
    if (bytes.empty() || size == 0) {
        return output;
    }

    IStream* stream = SHCreateMemStream(bytes.data(), static_cast<UINT>(bytes.size()));
    IWICImagingFactory* factory = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICBitmapScaler* scaler = nullptr;
    IWICFormatConverter* converter = nullptr;

    UINT sourceWidth = 0;
    UINT sourceHeight = 0;
    HRESULT hr = stream ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(&factory));
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateDecoderFromStream(stream, nullptr,
                                              WICDecodeMetadataCacheOnLoad, &decoder);
    }
    if (SUCCEEDED(hr)) {
        hr = decoder->GetFrame(0, &frame);
    }
    if (SUCCEEDED(hr)) {
        hr = frame->GetSize(&sourceWidth, &sourceHeight);
    }

    UINT scaledWidth = size;
    UINT scaledHeight = size;
    if (SUCCEEDED(hr) && sourceWidth > 0 && sourceHeight > 0) {
        double scale = static_cast<double>(size) /
                       static_cast<double>(std::min(sourceWidth, sourceHeight));
        scaledWidth = std::max(size, static_cast<UINT>(std::ceil(sourceWidth * scale)));
        scaledHeight = std::max(size, static_cast<UINT>(std::ceil(sourceHeight * scale)));
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateBitmapScaler(&scaler);
    }
    if (SUCCEEDED(hr)) {
        hr = scaler->Initialize(frame, scaledWidth, scaledHeight,
                                WICBitmapInterpolationModeFant);
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateFormatConverter(&converter);
    }
    if (SUCCEEDED(hr)) {
        hr = converter->Initialize(scaler, GUID_WICPixelFormat32bppPBGRA,
                                   WICBitmapDitherTypeNone, nullptr, 0,
                                   WICBitmapPaletteTypeCustom);
    }

    std::vector<BYTE> scaledPixels(static_cast<size_t>(scaledWidth) * scaledHeight * 4);
    if (SUCCEEDED(hr)) {
        hr = converter->CopyPixels(nullptr, scaledWidth * 4,
                                   static_cast<UINT>(scaledPixels.size()),
                                   scaledPixels.data());
    }
    if (SUCCEEDED(hr)) {
        std::vector<BYTE> pixels(static_cast<size_t>(size) * size * 4);
        UINT offsetX = (scaledWidth > size) ? (scaledWidth - size) / 2 : 0;
        UINT offsetY = (scaledHeight > size) ? (scaledHeight - size) / 2 : 0;
        for (UINT y = 0; y < size; ++y) {
            BYTE* source = scaledPixels.data() +
                (static_cast<size_t>(y + offsetY) * scaledWidth + offsetX) * 4;
            BYTE* target = pixels.data() + static_cast<size_t>(y) * size * 4;
            std::memcpy(target, source, static_cast<size_t>(size) * 4);
        }

        ApplyPopupCoverG2Mask(pixels, size);
        output = EncodePbgraPngBytes(size, size, pixels);
    }

    if (converter) converter->Release();
    if (scaler) scaler->Release();
    if (frame) frame->Release();
    if (decoder) decoder->Release();
    if (factory) factory->Release();
    if (stream) stream->Release();
    return output;
}

std::vector<uint8_t> CreatePlaceholderAlbumCoverBytes(UINT size,
                                                       bool edgeFadeToMiddle = false,
                                                       bool fadeFromTop = false,
                                                       bool fadeFromRight = false) {
    if (size == 0) {
        return {};
    }

    auto accent = DefaultPopupAccentColor();
    std::vector<BYTE> pixels(static_cast<size_t>(size) * size * 4);
    double denom = size > 1 ? static_cast<double>(size - 1) : 1.0;
    for (UINT y = 0; y < size; ++y) {
        double v = static_cast<double>(y) / denom;
        for (UINT x = 0; x < size; ++x) {
            double u = static_cast<double>(x) / denom;
            double dx = u - 0.34;
            double dy = v - 0.28;
            double glow = std::exp(-(dx * dx + dy * dy) / 0.16);
            double diagonal = 1.0 - 0.34 * v + 0.12 * u;
            double r = accent.R * (0.74 + 0.24 * glow) * diagonal + 36.0 * glow;
            double g = accent.G * (0.78 + 0.20 * glow) * diagonal + 28.0 * (1.0 - v);
            double b = accent.B * (0.86 + 0.14 * glow) * diagonal + 20.0 * u;

            double alphaScale = 1.0;
            if (edgeFadeToMiddle) {
                const double middle = 0.50;
                double edgeV = fadeFromRight ? u : (fadeFromTop ? 1.0 - v : v);
                alphaScale = 0.0;
                if (edgeV > middle) {
                    double amount = (edgeV - middle) / (1.0 - middle);
                    amount = Clamp(amount, 0.0, 1.0);
                    amount = amount * amount * (3.0 - 2.0 * amount);
                    alphaScale = 0.72 * amount;
                }
            }

            BYTE alpha = static_cast<BYTE>(std::lround(255.0 * Clamp(alphaScale, 0.0, 1.0)));
            BYTE rr = static_cast<BYTE>(Clamp(static_cast<int>(std::lround(r)), 0, 255));
            BYTE gg = static_cast<BYTE>(Clamp(static_cast<int>(std::lround(g)), 0, 255));
            BYTE bb = static_cast<BYTE>(Clamp(static_cast<int>(std::lround(b)), 0, 255));
            BYTE* pixel = pixels.data() + (static_cast<size_t>(y) * size + x) * 4;
            pixel[0] = static_cast<BYTE>(std::lround(bb * alpha / 255.0));
            pixel[1] = static_cast<BYTE>(std::lround(gg * alpha / 255.0));
            pixel[2] = static_cast<BYTE>(std::lround(rr * alpha / 255.0));
            pixel[3] = alpha;
        }
    }
    return EncodePbgraPngBytes(size, size, pixels);
}

bool GetArtworkDimensions(std::vector<uint8_t> const& bytes, UINT& width, UINT& height) {
    width = 0;
    height = 0;
    if (bytes.empty()) {
        return false;
    }

    IStream* stream = SHCreateMemStream(bytes.data(), static_cast<UINT>(bytes.size()));
    IWICImagingFactory* factory = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;

    HRESULT hr = stream ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(&factory));
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateDecoderFromStream(stream, nullptr,
                                              WICDecodeMetadataCacheOnLoad, &decoder);
    }
    if (SUCCEEDED(hr)) {
        hr = decoder->GetFrame(0, &frame);
    }
    if (SUCCEEDED(hr)) {
        hr = frame->GetSize(&width, &height);
    }

    if (frame) frame->Release();
    if (decoder) decoder->Release();
    if (factory) factory->Release();
    if (stream) stream->Release();
    return SUCCEEDED(hr) && width > 0 && height > 0;
}

bool ShouldUseAbstractArtworkForDisplay(std::vector<uint8_t> const& bytes) {
    UINT width = 0;
    UINT height = 0;
    if (!GetArtworkDimensions(bytes, width, height)) {
        return false;
    }

    UINT longer = std::max(width, height);
    UINT shorter = std::min(width, height);
    if (shorter == 0) {
        return false;
    }

    double aspect = static_cast<double>(longer) / static_cast<double>(shorter);
    bool videoLikeAspect = aspect >= 1.25;
    bool lowResolution = longer < 320 || shorter < 220;

    // Only abstract low-resolution / video-like thumbnails. Typical square album
    // covers stay untouched.
    return videoLikeAspect || lowResolution;
}

winrt::Windows::UI::Color BoostArtworkColor(winrt::Windows::UI::Color color,
                                            double saturationScale,
                                            double brightnessScale) {
    double r = color.R;
    double g = color.G;
    double b = color.B;
    double luma = 0.299 * r + 0.587 * g + 0.114 * b;
    r = (luma + (r - luma) * saturationScale) * brightnessScale;
    g = (luma + (g - luma) * saturationScale) * brightnessScale;
    b = (luma + (b - luma) * saturationScale) * brightnessScale;

    auto cleanColor = [&](double minChroma, double minLuma, double maxLuma,
                          double extraSaturation, double whiteMix) {
        double gray = 0.299 * r + 0.587 * g + 0.114 * b;
        r = gray + (r - gray) * extraSaturation;
        g = gray + (g - gray) * extraSaturation;
        b = gray + (b - gray) * extraSaturation;

        double maxC = std::max({r, g, b});
        double minC = std::min({r, g, b});
        double chroma = maxC - minC;
        if (chroma < minChroma) {
            double factor = chroma < 1.0 ? 2.35 : std::min(2.35, minChroma / chroma);
            double g2 = 0.299 * r + 0.587 * g + 0.114 * b;
            r = g2 + (r - g2) * factor;
            g = g2 + (g - g2) * factor;
            b = g2 + (b - g2) * factor;
        }

        if (whiteMix > 0.0) {
            r = r * (1.0 - whiteMix) + 255.0 * whiteMix;
            g = g * (1.0 - whiteMix) + 255.0 * whiteMix;
            b = b * (1.0 - whiteMix) + 255.0 * whiteMix;
        }

        double lum = 0.2126 * r + 0.7152 * g + 0.0722 * b;
        if (lum < minLuma) {
            double lift = minLuma - lum;
            r += lift * 0.88;
            g += lift * 0.88;
            b += lift * 0.88;
        } else if (lum > maxLuma) {
            double scale = maxLuma / std::max(1.0, lum);
            r *= scale;
            g *= scale;
            b *= scale;
        }
    };

    double maxBeforeClean = std::max({r, g, b});
    double minBeforeClean = std::min({r, g, b});
    double satBeforeClean = maxBeforeClean > 1.0
                                ? (maxBeforeClean - minBeforeClean) / maxBeforeClean
                                : 0.0;
    double muted = Clamp((0.34 - satBeforeClean) / 0.28, 0.0, 1.0);
    if (IsDarkModeApprox()) {
        // Dark theme: use the same adaptive idea as light mode. Muted covers get
        // a restrained clean-up rather than a fixed high-saturation boost.
        double minChroma = 34.0 + (1.0 - muted) * 26.0;
        double extraSat = 1.08 + (1.0 - muted) * 0.26;
        cleanColor(minChroma, 82.0, muted > 0.55 ? 202.0 : 226.0, extraSat, 0.00);
    } else {
        // Light theme: adapt to the source colorfulness. Muted covers should not
        // be forced into neon accents, but still need enough chroma to avoid a
        // dirty gray/brown look on the progress bar and generated artwork.
        double minChroma = 36.0 + (1.0 - muted) * 24.0;
        double extraSat = 1.16 + (1.0 - muted) * 0.28;
        cleanColor(minChroma, 122.0, muted > 0.55 ? 206.0 : 218.0, extraSat, 0.00);
    }

    return Color(color.A,
                 static_cast<BYTE>(std::clamp(std::lround(r), 0l, 255l)),
                 static_cast<BYTE>(std::clamp(std::lround(g), 0l, 255l)),
                 static_cast<BYTE>(std::clamp(std::lround(b), 0l, 255l)));
}

std::vector<uint8_t> CreateMeshGradientAlbumCoverBytes(std::vector<uint8_t> const& bytes) {
    if (bytes.empty()) {
        return {};
    }

    constexpr UINT kSampleSize = 14;
    constexpr UINT kOutputSize = 128;
    const UINT stride = kSampleSize * 4;
    const UINT bufferSize = stride * kSampleSize;

    IStream* stream = SHCreateMemStream(bytes.data(), static_cast<UINT>(bytes.size()));
    IWICImagingFactory* factory = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICBitmapScaler* scaler = nullptr;
    IWICFormatConverter* converter = nullptr;

    HRESULT hr = stream ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(&factory));
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateDecoderFromStream(stream, nullptr,
                                              WICDecodeMetadataCacheOnLoad, &decoder);
    }
    if (SUCCEEDED(hr)) {
        hr = decoder->GetFrame(0, &frame);
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateBitmapScaler(&scaler);
    }
    if (SUCCEEDED(hr)) {
        hr = scaler->Initialize(frame, kSampleSize, kSampleSize,
                                WICBitmapInterpolationModeFant);
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateFormatConverter(&converter);
    }
    if (SUCCEEDED(hr)) {
        hr = converter->Initialize(scaler, GUID_WICPixelFormat32bppPBGRA,
                                   WICBitmapDitherTypeNone, nullptr, 0,
                                   WICBitmapPaletteTypeCustom);
    }

    std::vector<BYTE> sample(bufferSize);
    if (SUCCEEDED(hr)) {
        hr = converter->CopyPixels(nullptr, stride, bufferSize, sample.data());
    }
    if (FAILED(hr)) {
        if (converter) converter->Release();
        if (scaler) scaler->Release();
        if (frame) frame->Release();
        if (decoder) decoder->Release();
        if (factory) factory->Release();
        if (stream) stream->Release();
        return {};
    }

    auto averageRectColor = [&](int x0, int y0, int x1, int y1) -> winrt::Windows::UI::Color {
        long sumB = 0, sumG = 0, sumR = 0, sumA = 0, count = 0;
        x0 = std::clamp(x0, 0, static_cast<int>(kSampleSize) - 1);
        y0 = std::clamp(y0, 0, static_cast<int>(kSampleSize) - 1);
        x1 = std::clamp(x1, x0 + 1, static_cast<int>(kSampleSize));
        y1 = std::clamp(y1, y0 + 1, static_cast<int>(kSampleSize));
        for (int y = y0; y < y1; ++y) {
            for (int x = x0; x < x1; ++x) {
                BYTE* pixel = sample.data() + (static_cast<size_t>(y) * kSampleSize + x) * 4;
                auto color = ColorFromPbgra(pixel);
                if (color.A == 0) {
                    continue;
                }
                sumB += color.B;
                sumG += color.G;
                sumR += color.R;
                sumA += color.A;
                ++count;
            }
        }
        if (count <= 0) {
            return DefaultPopupAccentColor();
        }
        winrt::Windows::UI::Color c = Color(static_cast<BYTE>(sumA / count),
                                            static_cast<BYTE>(sumR / count),
                                            static_cast<BYTE>(sumG / count),
                                            static_cast<BYTE>(sumB / count));
        return BoostArtworkColor(c, 1.18, 1.04);
    };

    struct SalientSample {
        winrt::Windows::UI::Color color = DefaultPopupAccentColor();
        float x = 0.72f;
        float y = 0.30f;
        double score = -1.0;
    };

    SalientSample salient;
    for (UINT y = 0; y < kSampleSize; ++y) {
        for (UINT x = 0; x < kSampleSize; ++x) {
            BYTE* pixel = sample.data() + (static_cast<size_t>(y) * kSampleSize + x) * 4;
            auto color = ColorFromPbgra(pixel);
            if (color.A == 0) {
                continue;
            }
            double b = color.B;
            double g = color.G;
            double r = color.R;
            double maxChannel = std::max({r, g, b});
            double minChannel = std::min({r, g, b});
            double chroma = maxChannel - minChannel;
            double saturation = maxChannel > 1.0 ? chroma / maxChannel : 0.0;
            double luma = 0.299 * r + 0.587 * g + 0.114 * b;

            // Pick a visible accent, not merely the largest-area color. Favor
            // saturated mid/high-luma colors and de-emphasize black/white UI
            // chrome that can appear in video thumbnails.
            double lumaWindow = 1.0 - std::abs(luma - 145.0) / 145.0;
            lumaWindow = Clamp(lumaWindow, 0.0, 1.0);
            double score = saturation * saturation * (0.45 + 0.55 * lumaWindow) *
                           (0.40 + 0.60 * (chroma / 255.0));
            if (score > salient.score) {
                salient.score = score;
                salient.color = BoostArtworkColor(
                    color, 1.48, 1.08);
                salient.x = kSampleSize > 1 ? static_cast<float>(x) / (kSampleSize - 1) : 0.72f;
                salient.y = kSampleSize > 1 ? static_cast<float>(y) / (kSampleSize - 1) : 0.30f;
            }
        }
    }

    // Keep a stable highlight position even if the salient sample is near an edge.
    salient.x = std::clamp(salient.x, 0.18f, 0.82f);
    salient.y = std::clamp(salient.y, 0.16f, 0.78f);

    std::array<winrt::Windows::UI::Color, 6> palette{
        averageRectColor(0, 0, 5, 5),
        averageRectColor(9, 0, 14, 5),
        averageRectColor(0, 9, 5, 14),
        averageRectColor(9, 9, 14, 14),
        averageRectColor(3, 3, 11, 11),
        salient.color,
    };

    std::array<winrt::Windows::Foundation::Numerics::float2, 6> points{{
        {0.16f, 0.18f},
        {0.84f, 0.16f},
        {0.18f, 0.82f},
        {0.82f, 0.84f},
        {0.50f, 0.50f},
        {salient.x, salient.y},
    }};
    std::array<float, 6> sigma{{0.34f, 0.34f, 0.34f, 0.34f, 0.42f, 0.16f}};
    std::array<float, 6> weightBoost{{1.0f, 1.0f, 1.0f, 1.0f, 0.92f, 2.25f}};

    std::vector<BYTE> outputPixels(static_cast<size_t>(kOutputSize) * kOutputSize * 4);
    for (UINT y = 0; y < kOutputSize; ++y) {
        float fy = kOutputSize > 1 ? static_cast<float>(y) / (kOutputSize - 1) : 0.0f;
        for (UINT x = 0; x < kOutputSize; ++x) {
            float fx = kOutputSize > 1 ? static_cast<float>(x) / (kOutputSize - 1) : 0.0f;
            double sumW = 0.0;
            double sumR = 0.0, sumG = 0.0, sumB = 0.0;
            for (size_t i = 0; i < points.size(); ++i) {
                float dx = fx - points[i].x;
                float dy = fy - points[i].y;
                float s = sigma[i];
                double w = std::exp(-static_cast<double>(dx * dx + dy * dy) /
                                    static_cast<double>(2.0f * s * s));
                w *= weightBoost[i];
                sumW += w;
                sumR += palette[i].R * w;
                sumG += palette[i].G * w;
                sumB += palette[i].B * w;
            }
            if (sumW <= 0.0) sumW = 1.0;
            double r = sumR / sumW;
            double g = sumG / sumW;
            double b = sumB / sumW;

            // Add an asymmetric accent orb: one side is denser / more solid,
            // while the opposite side diffuses into a softer bloom. This keeps
            // the salient accent color visible without turning the artwork into
            // a single flat gradient ball.
            double dirX = salient.x - 0.5;
            double dirY = salient.y - 0.5;
            double dirLen = std::sqrt(dirX * dirX + dirY * dirY);
            if (dirLen < 1e-4) {
                dirX = 0.78;
                dirY = -0.22;
                dirLen = std::sqrt(dirX * dirX + dirY * dirY);
            }
            dirX /= dirLen;
            dirY /= dirLen;
            double perpX = -dirY;
            double perpY = dirX;

            auto gaussianAniso = [&](double px, double py,
                                     double cx, double cy,
                                     double sigmaAlong, double sigmaAcross) -> double {
                double dx = px - cx;
                double dy = py - cy;
                double along = dx * dirX + dy * dirY;
                double across = dx * perpX + dy * perpY;
                double value = (along * along) / (2.0 * sigmaAlong * sigmaAlong) +
                               (across * across) / (2.0 * sigmaAcross * sigmaAcross);
                return std::exp(-value);
            };

            double coreCx = salient.x + dirX * 0.020;
            double coreCy = salient.y + dirY * 0.020;
            double tailCx = salient.x - dirX * 0.060;
            double tailCy = salient.y - dirY * 0.060;
            double core = gaussianAniso(fx, fy, coreCx, coreCy, 0.060, 0.070);
            double tail = gaussianAniso(fx, fy, tailCx, tailCy, 0.170, 0.115);
            double rim = gaussianAniso(fx, fy,
                                       salient.x + dirX * 0.006,
                                       salient.y + dirY * 0.006,
                                       0.038, 0.050);

            // Solid side: stronger local color injection plus a subtle specular lift.
            double solidMix = 0.40 * core + 0.16 * rim;
            // Soft side: broader low-contrast bloom using a lighter version.
            winrt::Windows::UI::Color softColor = BoostArtworkColor(salient.color, 1.08, 1.18);
            double softMix = 0.20 * tail;

            r = r * (1.0 - solidMix) + salient.color.R * solidMix;
            g = g * (1.0 - solidMix) + salient.color.G * solidMix;
            b = b * (1.0 - solidMix) + salient.color.B * solidMix;

            r = r * (1.0 - softMix) + softColor.R * softMix;
            g = g * (1.0 - softMix) + softColor.G * softMix;
            b = b * (1.0 - softMix) + softColor.B * softMix;

            // A small highlight sheen on the solid side adds contrast / texture.
            double sheen = 0.10 * core + 0.05 * rim;
            r = r * (1.0 - sheen) + 255.0 * sheen;
            g = g * (1.0 - sheen) + 255.0 * sheen;
            b = b * (1.0 - sheen) + 255.0 * sheen;

            // subtle brighten toward the center for a soft artwork-card glow
            double cx = (fx - 0.5) * 2.0;
            double cy = (fy - 0.5) * 2.0;
            double radial = std::clamp(1.0 - std::sqrt(cx * cx + cy * cy), 0.0, 1.0);
            double lift = 1.0 + radial * 0.07;
            size_t idx = (static_cast<size_t>(y) * kOutputSize + x) * 4;
            outputPixels[idx + 0] = static_cast<BYTE>(std::clamp(std::lround(b * lift), 0l, 255l));
            outputPixels[idx + 1] = static_cast<BYTE>(std::clamp(std::lround(g * lift), 0l, 255l));
            outputPixels[idx + 2] = static_cast<BYTE>(std::clamp(std::lround(r * lift), 0l, 255l));
            outputPixels[idx + 3] = 255;
        }
    }

    if (converter) converter->Release();
    if (scaler) scaler->Release();
    if (frame) frame->Release();
    if (decoder) decoder->Release();
    if (factory) factory->Release();
    if (stream) stream->Release();

    return EncodePbgraPngBytes(kOutputSize, kOutputSize, outputPixels);
}

struct AdaptiveArtworkPalette {
    winrt::Windows::UI::Color tl = DefaultPopupAccentColor();
    winrt::Windows::UI::Color tr = DefaultPopupAccentColor();
    winrt::Windows::UI::Color bl = DefaultPopupAccentColor();
    winrt::Windows::UI::Color br = DefaultPopupAccentColor();
    winrt::Windows::UI::Color center = DefaultPopupAccentColor();
    winrt::Windows::UI::Color bg = DefaultPopupAccentColor();
    winrt::Windows::UI::Color dark = Color(255, 32, 32, 40);
    winrt::Windows::UI::Color accent = DefaultPopupAccentColor();
    winrt::Windows::UI::Color accentSoft = DefaultPopupAccentColor();
    winrt::Windows::UI::Color bright = Color(255, 255, 240, 190);
    float accentX = 0.68f;
    float accentY = 0.32f;
};

struct RenderColorD {
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;
};

RenderColorD ToRenderColor(winrt::Windows::UI::Color const& c) {
    return {static_cast<double>(c.R), static_cast<double>(c.G), static_cast<double>(c.B)};
}

void BlendRenderColor(RenderColorD& base,
                      winrt::Windows::UI::Color const& color,
                      double alpha) {
    alpha = Clamp(alpha, 0.0, 1.0);
    base.r = base.r * (1.0 - alpha) + color.R * alpha;
    base.g = base.g * (1.0 - alpha) + color.G * alpha;
    base.b = base.b * (1.0 - alpha) + color.B * alpha;
}

winrt::Windows::UI::Color LerpArtworkColor(winrt::Windows::UI::Color const& a,
                                           winrt::Windows::UI::Color const& b,
                                           double t) {
    t = Clamp(t, 0.0, 1.0);
    auto L = [&](BYTE av, BYTE bv) -> BYTE {
        return static_cast<BYTE>(std::clamp(std::lround(av * (1.0 - t) + bv * t), 0l, 255l));
    };
    return Color(255, L(a.R, b.R), L(a.G, b.G), L(a.B, b.B));
}

bool ExtractAdaptiveArtworkPalette(std::vector<uint8_t> const& bytes,
                                   AdaptiveArtworkPalette& out) {
    constexpr UINT kSampleSize = 14;
    const UINT stride = kSampleSize * 4;
    const UINT bufferSize = stride * kSampleSize;

    IStream* stream = SHCreateMemStream(bytes.data(), static_cast<UINT>(bytes.size()));
    IWICImagingFactory* factory = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICBitmapScaler* scaler = nullptr;
    IWICFormatConverter* converter = nullptr;

    HRESULT hr = stream ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(&factory));
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateDecoderFromStream(stream, nullptr,
                                              WICDecodeMetadataCacheOnLoad, &decoder);
    }
    if (SUCCEEDED(hr)) {
        hr = decoder->GetFrame(0, &frame);
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateBitmapScaler(&scaler);
    }
    if (SUCCEEDED(hr)) {
        hr = scaler->Initialize(frame, kSampleSize, kSampleSize,
                                WICBitmapInterpolationModeFant);
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateFormatConverter(&converter);
    }
    if (SUCCEEDED(hr)) {
        hr = converter->Initialize(scaler, GUID_WICPixelFormat32bppPBGRA,
                                   WICBitmapDitherTypeNone, nullptr, 0,
                                   WICBitmapPaletteTypeCustom);
    }

    std::vector<BYTE> sample(bufferSize);
    if (SUCCEEDED(hr)) {
        hr = converter->CopyPixels(nullptr, stride, bufferSize, sample.data());
    }
    if (FAILED(hr)) {
        if (converter) converter->Release();
        if (scaler) scaler->Release();
        if (frame) frame->Release();
        if (decoder) decoder->Release();
        if (factory) factory->Release();
        if (stream) stream->Release();
        return false;
    }

    auto averageRectColor = [&](int x0, int y0, int x1, int y1,
                                double sat = 1.14, double bri = 1.03)
        -> winrt::Windows::UI::Color {
        long sumB = 0, sumG = 0, sumR = 0, sumA = 0, count = 0;
        x0 = std::clamp(x0, 0, static_cast<int>(kSampleSize) - 1);
        y0 = std::clamp(y0, 0, static_cast<int>(kSampleSize) - 1);
        x1 = std::clamp(x1, x0 + 1, static_cast<int>(kSampleSize));
        y1 = std::clamp(y1, y0 + 1, static_cast<int>(kSampleSize));
        for (int y = y0; y < y1; ++y) {
            for (int x = x0; x < x1; ++x) {
                BYTE* pixel = sample.data() + (static_cast<size_t>(y) * kSampleSize + x) * 4;
                auto color = ColorFromPbgra(pixel);
                if (color.A == 0) {
                    continue;
                }
                sumB += color.B;
                sumG += color.G;
                sumR += color.R;
                sumA += color.A;
                ++count;
            }
        }
        if (count <= 0) {
            return DefaultPopupAccentColor();
        }
        return BoostArtworkColor(Color(static_cast<BYTE>(sumA / count),
                                       static_cast<BYTE>(sumR / count),
                                       static_cast<BYTE>(sumG / count),
                                       static_cast<BYTE>(sumB / count)), sat, bri);
    };

    out.tl = averageRectColor(0, 0, 5, 5, 1.08, 1.01);
    out.tr = averageRectColor(9, 0, 14, 5, 1.08, 1.01);
    out.bl = averageRectColor(0, 9, 5, 14, 1.08, 1.00);
    out.br = averageRectColor(9, 9, 14, 14, 1.08, 1.00);
    out.center = averageRectColor(3, 3, 11, 11, 1.10, 1.03);
    out.bg = averageRectColor(0, 0, 14, 14, 1.05, 0.98);
    out.dark = averageRectColor(2, 2, 12, 12, 0.96, 0.72);

    double bestScore = -1.0;
    for (UINT y = 0; y < kSampleSize; ++y) {
        for (UINT x = 0; x < kSampleSize; ++x) {
            BYTE* pixel = sample.data() + (static_cast<size_t>(y) * kSampleSize + x) * 4;
            auto color = ColorFromPbgra(pixel);
            if (color.A == 0) {
                continue;
            }
            double b = color.B;
            double g = color.G;
            double r = color.R;
            double maxChannel = std::max({r, g, b});
            double minChannel = std::min({r, g, b});
            double chroma = maxChannel - minChannel;
            double saturation = maxChannel > 1.0 ? chroma / maxChannel : 0.0;
            double luma = 0.299 * r + 0.587 * g + 0.114 * b;
            double lumaWindow = 1.0 - std::abs(luma - 150.0) / 150.0;
            lumaWindow = Clamp(lumaWindow, 0.0, 1.0);
            double score = saturation * saturation * (0.42 + 0.58 * lumaWindow) *
                           (0.40 + 0.60 * (chroma / 255.0));
            if (score > bestScore) {
                bestScore = score;
                out.accent = BoostArtworkColor(
                    color, 1.46, 1.08);
                out.accentX = kSampleSize > 1 ? static_cast<float>(x) / (kSampleSize - 1) : 0.68f;
                out.accentY = kSampleSize > 1 ? static_cast<float>(y) / (kSampleSize - 1) : 0.32f;
            }
        }
    }
    out.accentX = std::clamp(out.accentX, 0.18f, 0.84f);
    out.accentY = std::clamp(out.accentY, 0.16f, 0.82f);
    out.accentSoft = BoostArtworkColor(out.accent, 1.08, 1.16);
    out.bright = BoostArtworkColor(LerpArtworkColor(out.accent, Color(255, 255, 245, 200), 0.42),
                                   1.02, 1.14);

    if (converter) converter->Release();
    if (scaler) scaler->Release();
    if (frame) frame->Release();
    if (decoder) decoder->Release();
    if (factory) factory->Release();
    if (stream) stream->Release();
    return true;
}

double GaussianBlobWeight(double fx, double fy,
                          double cx, double cy,
                          double sx, double sy,
                          double rot = 0.0) {
    double dx = fx - cx;
    double dy = fy - cy;
    double cr = std::cos(rot);
    double sr = std::sin(rot);
    double rx = dx * cr + dy * sr;
    double ry = -dx * sr + dy * cr;
    return std::exp(-((rx * rx) / (2.0 * sx * sx) + (ry * ry) / (2.0 * sy * sy)));
}

double DiagonalBeamWeight(double fx, double fy,
                          double cx, double cy,
                          double rot, double sx, double sy) {
    return GaussianBlobWeight(fx, fy, cx, cy, sx, sy, rot);
}

double RibbonWeight(double fx, double fy,
                    double yBase, double amp, double freq,
                    double phase, double tilt, double sigma) {
    double y = yBase + amp * std::sin((fx * freq + phase) * 6.28318530718) +
               tilt * (fx - 0.5);
    double d = fy - y;
    return std::exp(-(d * d) / (2.0 * sigma * sigma));
}

std::vector<uint8_t> RenderAdaptiveArtworkTemplate(
    AdaptiveArtworkPalette const& pal,
    std::function<void(double, double, RenderColorD&)> const& shader,
    UINT outputSize = 128) {
    std::vector<BYTE> out(static_cast<size_t>(outputSize) * outputSize * 4);
    for (UINT y = 0; y < outputSize; ++y) {
        double fy = outputSize > 1 ? static_cast<double>(y) / (outputSize - 1) : 0.0;
        for (UINT x = 0; x < outputSize; ++x) {
            double fx = outputSize > 1 ? static_cast<double>(x) / (outputSize - 1) : 0.0;
            RenderColorD color = ToRenderColor(pal.bg);
            shader(fx, fy, color);
            // subtle top light and bottom vignette, similar to the HTML preview
            double topLight = (1.0 - fy) * 0.05;
            double bottomShade = fy * 0.14;
            color.r = std::clamp(color.r * (1.0 - bottomShade) + 255.0 * topLight, 0.0, 255.0);
            color.g = std::clamp(color.g * (1.0 - bottomShade) + 255.0 * topLight, 0.0, 255.0);
            color.b = std::clamp(color.b * (1.0 - bottomShade) + 255.0 * topLight, 0.0, 255.0);
            size_t idx = (static_cast<size_t>(y) * outputSize + x) * 4;
            out[idx + 0] = static_cast<BYTE>(std::clamp(std::lround(color.b), 0l, 255l));
            out[idx + 1] = static_cast<BYTE>(std::clamp(std::lround(color.g), 0l, 255l));
            out[idx + 2] = static_cast<BYTE>(std::clamp(std::lround(color.r), 0l, 255l));
            out[idx + 3] = 255;
        }
    }
    return EncodePbgraPngBytes(outputSize, outputSize, out);
}

std::vector<uint8_t> CreateEnergyFlameAlbumCoverBytes(std::vector<uint8_t> const& bytes) {
    AdaptiveArtworkPalette pal;
    if (!ExtractAdaptiveArtworkPalette(bytes, pal)) {
        return {};
    }
    auto warm = LerpArtworkColor(pal.accent, pal.center, 0.20);
    auto golden = BoostArtworkColor(LerpArtworkColor(pal.accent, Color(255, 255, 215, 70), 0.56), 1.10, 1.10);
    return RenderAdaptiveArtworkTemplate(pal, [&](double fx, double fy, RenderColorD& c) {
        auto top = LerpArtworkColor(pal.tl, warm, 0.48);
        auto bottom = LerpArtworkColor(pal.bl, pal.dark, 0.42);
        BlendRenderColor(c, LerpArtworkColor(bottom, top, 1.0 - fy), 1.0);
        BlendRenderColor(c, warm, 0.28 * GaussianBlobWeight(fx, fy, 0.38, 0.34, 0.38, 0.26, -0.20));
        BlendRenderColor(c, pal.dark, 0.26 * GaussianBlobWeight(fx, fy, 0.18, 0.78, 0.58, 0.28, 0.18));
        BlendRenderColor(c, golden, 0.82 * GaussianBlobWeight(fx, fy, 0.55, 0.16, 0.14, 0.18, 0.46));
        BlendRenderColor(c, golden, 0.38 * DiagonalBeamWeight(fx, fy, 0.58, 0.20, -0.72, 0.22, 0.08));
        BlendRenderColor(c, pal.accent, 0.22 * RibbonWeight(fx, fy, 0.18, 0.03, 1.02, 0.22, 0.12, 0.08));
    });
}

std::vector<uint8_t> CreateDisplayAlbumCoverBytes(
    std::vector<uint8_t> const& bytes,
    std::wstring const& sourceAppUserModelId) {
    if (bytes.empty()) {
        return {};
    }

    bool browserSource = LooksLikeBrowserMediaSource(sourceAppUserModelId);
    if (!browserSource ||
        !ShouldUseAbstractArtworkForDisplay(bytes) ||
        g_settings.artworkAbstractMode == L"browser_original" ||
        g_settings.artworkAbstractMode == L"off") {
        return bytes;
    }

    std::vector<uint8_t> abstractBytes;
    if (g_settings.artworkAbstractMode == L"energy_flame") {
        abstractBytes = CreateEnergyFlameAlbumCoverBytes(bytes);
    } else if (g_settings.artworkAbstractMode == L"mesh_gradient") {
        abstractBytes = CreateMeshGradientAlbumCoverBytes(bytes);
    }
    return abstractBytes.empty() ? bytes : abstractBytes;
}

winrt::Windows::UI::Color ExtractAlbumAccentColor(std::vector<uint8_t> const& bytes) {
    if (bytes.empty()) {
        return DefaultPopupAccentColor();
    }

    constexpr UINT kAccentSampleSize = 10;
    const UINT stride = kAccentSampleSize * 4;
    const UINT bufferSize = stride * kAccentSampleSize;

    IStream* stream = SHCreateMemStream(bytes.data(), static_cast<UINT>(bytes.size()));
    IWICImagingFactory* factory = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICBitmapScaler* scaler = nullptr;
    IWICFormatConverter* converter = nullptr;

    HRESULT hr = stream ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(&factory));
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateDecoderFromStream(stream, nullptr,
                                              WICDecodeMetadataCacheOnLoad, &decoder);
    }
    if (SUCCEEDED(hr)) {
        hr = decoder->GetFrame(0, &frame);
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateBitmapScaler(&scaler);
    }
    if (SUCCEEDED(hr)) {
        hr = scaler->Initialize(frame, kAccentSampleSize, kAccentSampleSize,
                                WICBitmapInterpolationModeFant);
    }
    if (SUCCEEDED(hr)) {
        hr = factory->CreateFormatConverter(&converter);
    }
    if (SUCCEEDED(hr)) {
        hr = converter->Initialize(scaler, GUID_WICPixelFormat32bppPBGRA,
                                   WICBitmapDitherTypeNone, nullptr, 0,
                                   WICBitmapPaletteTypeCustom);
    }

    std::vector<BYTE> pixels(bufferSize);
    if (SUCCEEDED(hr)) {
        hr = converter->CopyPixels(nullptr, stride, bufferSize, pixels.data());
    }

    double sumR = 0.0;
    double sumG = 0.0;
    double sumB = 0.0;
    double weightSum = 0.0;
    double sourceSaturationSum = 0.0;
    double sourceChromaSum = 0.0;
    if (SUCCEEDED(hr)) {
        for (UINT y = 0; y < kAccentSampleSize; ++y) {
            for (UINT x = 0; x < kAccentSampleSize; ++x) {
                BYTE* pixel = pixels.data() +
                    (static_cast<size_t>(y) * kAccentSampleSize + x) * 4;
                double a = pixel[3] / 255.0;
                if (a < 0.05) {
                    continue;
                }
                double b = pixel[0] / std::max(a, 0.01);
                double g = pixel[1] / std::max(a, 0.01);
                double r = pixel[2] / std::max(a, 0.01);
                r = Clamp(r, 0.0, 255.0);
                g = Clamp(g, 0.0, 255.0);
                b = Clamp(b, 0.0, 255.0);
                double maxChannel = std::max({r, g, b});
                double minChannel = std::min({r, g, b});
                double chroma = maxChannel - minChannel;
                double saturation = maxChannel > 1.0 ? chroma / maxChannel : 0.0;
                double saturationWeight = 0.65 + chroma / 255.0;
                double weight = a * saturationWeight;
                sumR += r * weight;
                sumG += g * weight;
                sumB += b * weight;
                sourceSaturationSum += saturation * weight;
                sourceChromaSum += chroma * weight;
                weightSum += weight;
            }
        }
    }

    if (converter) converter->Release();
    if (scaler) scaler->Release();
    if (frame) frame->Release();
    if (decoder) decoder->Release();
    if (factory) factory->Release();
    if (stream) stream->Release();

    if (FAILED(hr) || weightSum <= 0.0) {
        return DefaultPopupAccentColor();
    }

    double sourceAvgSaturation = Clamp(sourceSaturationSum / weightSum, 0.0, 1.0);
    double sourceAvgChroma = Clamp(sourceChromaSum / weightSum, 0.0, 255.0);
    double r = sumR / weightSum;
    double g = sumG / weightSum;
    double b = sumB / weightSum;
    double gray = 0.299 * r + 0.587 * g + 0.114 * b;
    double mutedSource = Clamp((0.38 - sourceAvgSaturation) / 0.32, 0.0, 1.0);
    double kSaturationBoost = IsDarkModeApprox()
                                  ? 1.08 + (1.0 - mutedSource) * 0.24
                                  : 1.10 + (1.0 - mutedSource) * 0.20;
    r = gray + (r - gray) * kSaturationBoost;
    g = gray + (g - gray) * kSaturationBoost;
    b = gray + (b - gray) * kSaturationBoost;

    double lum = 0.2126 * r + 0.7152 * g + 0.0722 * b;
    if (lum < 82.0) {
        double lift = 82.0 - lum;
        r += lift * 0.70;
        g += lift * 0.70;
        b += lift * 0.70;
    } else if (lum > 214.0) {
        double scale = 214.0 / lum;
        r *= scale;
        g *= scale;
        b *= scale;
    }

    if (IsDarkModeApprox()) {
        // Dark theme now follows the same source-aware logic as light mode:
        // clean enough to be visible on a dark surface, but avoid neon accents
        // when the cover itself is globally muted.
        double gray2 = 0.299 * r + 0.587 * g + 0.114 * b;
        double sourceColorfulness = Clamp((sourceAvgSaturation - 0.16) / 0.44, 0.0, 1.0);
        double extraSat = 1.08 + sourceColorfulness * 0.30;
        r = gray2 + (r - gray2) * extraSat;
        g = gray2 + (g - gray2) * extraSat;
        b = gray2 + (b - gray2) * extraSat;

        double maxC = std::max({r, g, b});
        double minC = std::min({r, g, b});
        double chroma = maxC - minC;
        double minChroma = 34.0 + sourceColorfulness * 28.0;
        if (sourceAvgChroma < 26.0) {
            minChroma = std::min(minChroma, 38.0);
        }
        if (chroma < minChroma) {
            double maxFactor = 1.50 + sourceColorfulness * 0.80;
            double factor = chroma < 1.0 ? maxFactor : std::min(maxFactor, minChroma / chroma);
            double g2 = 0.299 * r + 0.587 * g + 0.114 * b;
            r = g2 + (r - g2) * factor;
            g = g2 + (g - g2) * factor;
            b = g2 + (b - g2) * factor;
        }

        double lum2 = 0.2126 * r + 0.7152 * g + 0.0722 * b;
        double minLuma = 88.0 + sourceColorfulness * 8.0;
        double maxLuma = 202.0 + sourceColorfulness * 22.0;
        if (lum2 < minLuma) {
            double lift = minLuma - lum2;
            r += lift * 0.68;
            g += lift * 0.68;
            b += lift * 0.68;
        } else if (lum2 > maxLuma) {
            double scale = maxLuma / std::max(1.0, lum2);
            r *= scale;
            g *= scale;
            b *= scale;
        }
    } else {
        // Light theme accent: avoid the dirty gray left edge, but keep muted
        // covers muted. Use only a gentle chroma floor for low-saturation
        // artwork, and reserve stronger saturation for already-colorful covers.
        double gray2 = 0.299 * r + 0.587 * g + 0.114 * b;
        double sourceColorfulness = Clamp((sourceAvgSaturation - 0.18) / 0.42, 0.0, 1.0);
        double extraSat = 1.10 + sourceColorfulness * 0.30;
        r = gray2 + (r - gray2) * extraSat;
        g = gray2 + (g - gray2) * extraSat;
        b = gray2 + (b - gray2) * extraSat;

        double maxC = std::max({r, g, b});
        double minC = std::min({r, g, b});
        double chroma = maxC - minC;
        double minChroma = 30.0 + sourceColorfulness * 30.0;
        if (sourceAvgChroma < 26.0) {
            minChroma = std::min(minChroma, 34.0);
        }
        if (chroma < minChroma) {
            double maxFactor = 1.45 + sourceColorfulness * 0.75;
            double factor = chroma < 1.0 ? maxFactor : std::min(maxFactor, minChroma / chroma);
            double g2 = 0.299 * r + 0.587 * g + 0.114 * b;
            r = g2 + (r - g2) * factor;
            g = g2 + (g - g2) * factor;
            b = g2 + (b - g2) * factor;
        }

        double lum2 = 0.2126 * r + 0.7152 * g + 0.0722 * b;
        double minLuma = 118.0 - sourceColorfulness * 8.0;
        double maxLuma = 170.0 + sourceColorfulness * 8.0;
        if (lum2 < minLuma) {
            double lift = minLuma - lum2;
            r += lift * 0.58;
            g += lift * 0.58;
            b += lift * 0.58;
        } else if (lum2 > maxLuma) {
            double scale = maxLuma / std::max(1.0, lum2);
            r *= scale;
            g *= scale;
            b *= scale;
        }
    }

    return Color(0xFF,
                 static_cast<BYTE>(Clamp(static_cast<int>(std::lround(r)), 0, 255)),
                 static_cast<BYTE>(Clamp(static_cast<int>(std::lround(g)), 0, 255)),
                 static_cast<BYTE>(Clamp(static_cast<int>(std::lround(b)), 0, 255)));
}

winrt::Windows::UI::Color LerpColor(winrt::Windows::UI::Color const& from,
                                  winrt::Windows::UI::Color const& to,
                                  double progress) {
    progress = Clamp(progress, 0.0, 1.0);
    auto lerpByte = [progress](BYTE a, BYTE b) -> BYTE {
        return static_cast<BYTE>(Clamp(static_cast<int>(std::lround(
            static_cast<double>(a) + (static_cast<double>(b) - static_cast<double>(a)) * progress)),
            0, 255));
    };
    return Color(lerpByte(from.A, to.A), lerpByte(from.R, to.R),
                 lerpByte(from.G, to.G), lerpByte(from.B, to.B));
}

winrt::Windows::UI::Color PopupTargetAccentColor() {
    return g_popupAccentColorValid ? g_popupAccentColor : DefaultPopupAccentColor();
}

winrt::Windows::UI::Color PopupAccentColor() {
    return g_popupDisplayedAccentColorValid ? g_popupDisplayedAccentColor
                                            : PopupTargetAccentColor();
}

double Clamp01(double v) {
    return Clamp(v, 0.0, 1.0);
}

winrt::Windows::UI::Color ColorFromHsv(double h, double s, double v, BYTE a = 0xFF) {
    h = std::fmod(h, 360.0);
    if (h < 0.0) h += 360.0;
    s = Clamp01(s);
    v = Clamp01(v);
    double c = v * s;
    double x = c * (1.0 - std::abs(std::fmod(h / 60.0, 2.0) - 1.0));
    double m = v - c;
    double r = 0.0, g = 0.0, b = 0.0;
    if (h < 60.0) { r = c; g = x; }
    else if (h < 120.0) { r = x; g = c; }
    else if (h < 180.0) { g = c; b = x; }
    else if (h < 240.0) { g = x; b = c; }
    else if (h < 300.0) { r = x; b = c; }
    else { r = c; b = x; }
    return Color(a,
                 static_cast<BYTE>(std::clamp(std::lround((r + m) * 255.0), 0l, 255l)),
                 static_cast<BYTE>(std::clamp(std::lround((g + m) * 255.0), 0l, 255l)),
                 static_cast<BYTE>(std::clamp(std::lround((b + m) * 255.0), 0l, 255l)));
}

void ColorToHsv(winrt::Windows::UI::Color const& color, double& h, double& s, double& v) {
    double r = color.R / 255.0;
    double g = color.G / 255.0;
    double b = color.B / 255.0;
    double maxv = std::max({r, g, b});
    double minv = std::min({r, g, b});
    double delta = maxv - minv;
    h = 0.0;
    if (delta > 1e-6) {
        if (maxv == r) h = 60.0 * std::fmod(((g - b) / delta), 6.0);
        else if (maxv == g) h = 60.0 * (((b - r) / delta) + 2.0);
        else h = 60.0 * (((r - g) / delta) + 4.0);
    }
    if (h < 0.0) h += 360.0;
    s = maxv <= 1e-6 ? 0.0 : delta / maxv;
    v = maxv;
}

winrt::Windows::UI::Color PopupProgressCleanStartColor() {
    double h, s, v;
    ColorToHsv(PopupAccentColor(), h, s, v);
    bool dark = IsDarkModeApprox();

    // The left edge uses the raw accent hue, but is cleaned in HSV space so it
    // doesn't become a gray/brown strip. If the cover is muted, keep saturation
    // moderate instead of forcing a neon color.
    double muted = Clamp((0.38 - s) / 0.32, 0.0, 1.0);
    double targetS;
    double targetV;
    if (dark) {
        targetS = muted > 0.0
                      ? Clamp(s * 1.10 + 0.08, 0.30, 0.52)
                      : Clamp(s * 1.04, 0.48, 0.76);
        targetV = muted > 0.0
                      ? Clamp(v + 0.08, 0.58, 0.76)
                      : Clamp(v + 0.04, 0.62, 0.88);
    } else {
        targetS = muted > 0.0
                      ? Clamp(s * 1.10 + 0.08, 0.28, 0.46)
                      : Clamp(s * 1.02, 0.46, 0.72);
        targetV = muted > 0.0
                      ? Clamp(v + 0.04, 0.64, 0.76)
                      : Clamp(v, 0.58, 0.82);
    }
    return ColorFromHsv(h, targetS, targetV);
}

winrt::Windows::UI::Color PopupProgressVividColor() {
    double h, s, v;
    ColorToHsv(PopupAccentColor(), h, s, v);
    bool dark = IsDarkModeApprox();

    double muted = Clamp((0.40 - s) / 0.34, 0.0, 1.0);
    double targetS;
    double targetV;
    if (dark) {
        targetS = muted > 0.0
                      ? Clamp(s * 1.10 + 0.10, 0.34, 0.54)
                      : Clamp(s * 1.08, 0.56, 0.82);
        targetV = muted > 0.0
                      ? Clamp(v + 0.10, 0.68, 0.86)
                      : Clamp(v + 0.08, 0.76, 0.94);
    } else {
        targetS = muted > 0.0
                      ? Clamp(s * 1.08 + 0.10, 0.32, 0.50)
                      : Clamp(s * 1.08, 0.54, 0.78);
        targetV = muted > 0.0
                      ? Clamp(v + 0.06, 0.68, 0.82)
                      : Clamp(v + 0.07, 0.64, 0.86);
    }
    return ColorFromHsv(h, targetS, targetV);
}

winrt::Windows::UI::Color PopupProgressBrightSoftColor() {
    double h, s, v;
    ColorToHsv(PopupAccentColor(), h, s, v);
    bool dark = IsDarkModeApprox();

    double targetS;
    double targetV;
    double muted = Clamp((0.40 - s) / 0.34, 0.0, 1.0);
    if (dark) {
        targetS = muted > 0.0
                      ? Clamp(s * 0.78 + 0.08, 0.25, 0.42)
                      : Clamp(s * 0.52, 0.32, 0.56);
        targetV = muted > 0.0 ? 0.82 : 0.92;
    } else {
        targetS = muted > 0.0
                      ? Clamp(s * 0.78 + 0.08, 0.24, 0.40)
                      : Clamp(s * 0.64, 0.36, 0.58);
        targetV = muted > 0.0 ? 0.80 : 0.86;
    }
    auto color = ColorFromHsv(h, targetS, targetV);

    // Same idea as the component stroke: on dark surfaces the highlight leans
    // closer to white; on light surfaces it keeps more accent chroma for contrast.
    if (dark) {
        double muted = Clamp((0.40 - s) / 0.34, 0.0, 1.0);
        color = LerpArtworkColor(color, Color(0xFF, 0xFF, 0xFF, 0xFF),
                                 muted > 0.0 ? 0.08 : 0.14);
    }
    return color;
}

mediax::Brush MakePopupProgressGradientBrush() {
    auto accent = PopupProgressCleanStartColor();
    auto vivid = PopupProgressVividColor();
    auto soft = PopupProgressBrightSoftColor();
    bool dark = IsDarkModeApprox();

    if (dark) {
        double h, s, v;
        ColorToHsv(PopupAccentColor(), h, s, v);
        double muted = Clamp((0.40 - s) / 0.34, 0.0, 1.0);
        vivid = LerpArtworkColor(vivid, accent, muted > 0.0 ? 0.18 : 0.08);
        soft = LerpArtworkColor(soft, vivid, muted > 0.0 ? 0.16 : 0.08);
    } else {
        // Light theme: keep the gradient clean and related to the artwork
        // without introducing a gray first stop or an over-saturated tail.
        vivid = LerpArtworkColor(vivid, accent, 0.10);
        soft = LerpArtworkColor(soft, vivid, 0.22);
    }

    mediax::LinearGradientBrush brush;
    brush.StartPoint({0.0, 0.5});
    brush.EndPoint({1.0, 0.5});
    mediax::GradientStopCollection stops;

    auto addStop = [&](double offset, winrt::Windows::UI::Color const& color) {
        mediax::GradientStop stop;
        stop.Offset(offset);
        stop.Color(color);
        stops.Append(stop);
    };

    addStop(0.00, accent);
    addStop(dark ? 0.66 : 0.58, vivid);
    addStop(1.00, soft);
    brush.GradientStops(stops);
    return brush;
}

std::wstring FormatMediaTime(int64_t ticks) {
    if (ticks <= 0) {
        return L"0:00";
    }
    int64_t totalSeconds = ticks / 10000000LL;
    int64_t hours = totalSeconds / 3600;
    int64_t minutes = (totalSeconds / 60) % 60;
    int64_t seconds = totalSeconds % 60;
    wchar_t buffer[32]{};
    if (hours > 0) {
        swprintf_s(buffer, L"%lld:%02lld:%02lld", hours, minutes, seconds);
    } else {
        swprintf_s(buffer, L"%lld:%02lld", minutes, seconds);
    }
    return buffer;
}

void UpdatePopupAlbumBitmap(MediaState const& state) {
    uint64_t hash = ThumbnailHash(state.thumbnailBytes);
    if (hash == g_popupThumbnailHash) {
        return;
    }

    g_popupThumbnailHash = hash;
    if (g_popupAlbumBitmap) {
        DeleteObject(g_popupAlbumBitmap);
        g_popupAlbumBitmap = nullptr;
    }
    if (hash) {
        g_popupAlbumBitmap = DecodeAlbumBitmap(state.thumbnailBytes, 256);
    }
}

constexpr wchar_t kPopupClassName[] = L"WindhawkIslandMediaPopup";
constexpr wchar_t kPopupBackdropOverlayClassName[] = L"WindhawkIslandMediaBackdropOverlay";
constexpr UINT_PTR kPopupTimerId = 0x494D;
constexpr UINT_PTR kPopupBackdropOverlayTimerId = 0x494E;
constexpr UINT_PTR kTaskbarLayoutMonitorTimerId = 0x494D4C54;
constexpr UINT kTaskbarLayoutMonitorIntervalMs = 600;
constexpr int kPopupBackdropOverlayRefreshMs = 90;
constexpr int kPopupBackdropOverlayMaxCaptureSize = 320;

double SmoothStep(double value) {
    value = Clamp(value, 0.0, 1.0);
    return value * value * (3.0 - 2.0 * value);
}

double SmootherStep(double value) {
    value = Clamp(value, 0.0, 1.0);
    return value * value * value * (value * (value * 6.0 - 15.0) + 10.0);
}

double PopupTextMotionEase(double value) {
    value = Clamp(value, 0.0, 1.0);
    return 0.5 - std::cos(value * 3.14159265358979323846) * 0.5;
}

// Clip animated text to its own text lane. Without this, render transforms can
// let the incoming/outgoing title briefly draw outside the compact/popup card.
void ApplyElementClip(FrameworkElement const& element, double width, double height) {
    if (!element || width <= 0.0 || height <= 0.0) {
        return;
    }
    try {
        mediax::RectangleGeometry clip;
        clip.Rect({0.0f, 0.0f,
                   static_cast<float>(std::max(1.0, width)),
                   static_cast<float>(std::max(1.0, height))});
        element.Clip(clip);
    } catch (...) {
    }
}

void ApplyElementClipWithPadding(FrameworkElement const& element,
                                 double width,
                                 double height,
                                 double horizontalPadding) {
    if (!element || width <= 0.0 || height <= 0.0) {
        return;
    }
    try {
        horizontalPadding = std::max(0.0, horizontalPadding);
        mediax::RectangleGeometry clip;
        clip.Rect({static_cast<float>(-horizontalPadding), 0.0f,
                   static_cast<float>(std::max(1.0, width + horizontalPadding * 2.0)),
                   static_cast<float>(std::max(1.0, height))});
        element.Clip(clip);
    } catch (...) {
    }
}

mediax::Brush CompactTextEdgeFadeBrush(bool leftEdge) {
    auto bg = IslandBackgroundColor();
    auto clear = bg;
    clear.A = 0x00;

    mediax::LinearGradientBrush brush;
    brush.StartPoint({0.0, 0.5});
    brush.EndPoint({1.0, 0.5});
    mediax::GradientStopCollection stops;

    auto addStop = [&](double offset, winrt::Windows::UI::Color const& color) {
        mediax::GradientStop stop;
        stop.Offset(offset);
        stop.Color(color);
        stops.Append(stop);
    };

    if (leftEdge) {
        addStop(0.00, bg);
        addStop(0.42, bg);
        addStop(1.00, clear);
    } else {
        addStop(0.00, clear);
        addStop(0.58, bg);
        addStop(1.00, bg);
    }
    brush.GradientStops(stops);
    return brush;
}

double PopupProgress() {
    return SmoothStep(g_popupAnimationProgress);
}

double PopupClosingShellOpacity(double progress) {
    progress = Clamp(progress, 0.0, 1.0);
    return g_popupClosing ? SmootherStep(Clamp(progress / 0.12, 0.0, 1.0))
                          : progress;
}

double PopupClosingCompactRevealOpacity(double progress) {
    if (!g_popupClosing) {
        return 0.0;
    }
    return 1.0 - SmootherStep(Clamp(progress / 0.18, 0.0, 1.0));
}

double PopupClosingTextOpacity(double progress) {
    progress = Clamp(progress, 0.0, 1.0);
    return g_popupClosing
               ? SmootherStep(Clamp((progress - 0.035) / 0.18, 0.0, 1.0))
               : progress;
}

double PopupShadowMorphOpacity(double progress) {
    progress = Clamp(progress, 0.0, 1.0);
    return g_popupClosing
               ? SmootherStep(Clamp(progress / 0.28, 0.0, 1.0))
               : SmootherStep(Clamp((progress - 0.12) / 0.58, 0.0, 1.0));
}

double PopupControlMorphProgress(double progress) {
    progress = Clamp(progress, 0.0, 1.0);
    return g_popupClosing ? std::pow(progress, 1.45) : progress;
}

double PopupControlOpacityProgress(double progress) {
    progress = Clamp(progress, 0.0, 1.0);
    return g_popupClosing
               ? SmootherStep(Clamp((progress - 0.10) / 0.22, 0.0, 1.0))
               : progress;
}

double LerpDouble(double from, double to, double progress) {
    return from + (to - from) * progress;
}

static double SourceScaledRadius(double baseRadius,
                                 double baseSize,
                                 RECT const& sourceRect) {
    double sourceWidth = std::max(1.0,
        static_cast<double>(sourceRect.right - sourceRect.left));
    double sourceHeight = std::max(1.0,
        static_cast<double>(sourceRect.bottom - sourceRect.top));
    double scale = std::min(sourceWidth, sourceHeight) /
                   std::max(1.0, baseSize);
    return std::max(1.0, baseRadius * scale);
}

int LerpInt(int from, int to, double progress) {
    return static_cast<int>(std::lround(from + (to - from) * progress));
}

// C variant: compact-material-expanded layout.
constexpr int kPopupMinimumArtSize = 220;
constexpr int kPopupMaximumArtSize = 420;
// Main.PopupSpacing controls only the outer shell inset: cover->shell top,
// cover->shell left/right, and card->shell bottom. Main.PopupCardGap controls
// only the distance between the cover and the playback card.
int PopupSurfaceGap() {
    return Clamp(g_settings.popupSpacing, 2, 40);
}
int PopupArtCardGap() {
    return Clamp(g_settings.popupCardGap, 0, 40);
}
int PopupShadowDepth() {
    return Clamp(g_settings.popupShadowDepth, 0, 128);
}
int PopupShadowOpacityPercent() {
    return Clamp(g_settings.popupShadowOpacity, 0, 100);
}
int PopupBackdropInitialFrameSkip() {
    return Clamp(g_settings.backdropInitialFrameSkip, 1, 3);
}
int PopupBackdropFallbackBlurPasses() {
    int passes = Clamp(g_settings.backdropFallbackBlurPasses, 4, 6);
    if (IsLiquidGlassMaterial()) {
        return Clamp(passes - 4, 1, 2);
    }

    return passes;
}
int PopupBackdropFallbackCaptureScale() {
    return Clamp(g_settings.backdropFallbackCaptureScale, 2, 3);
}
float PopupBackdropWgcBlurStdDev() {
    return static_cast<float>(Clamp(g_settings.backdropWgcBlurStdDev, 14, 22));
}
float PopupBackdropWgcEffectiveBlurStdDev() {
    float stdDev = PopupBackdropWgcBlurStdDev();
    if (IsLiquidGlassMaterial()) {
        return Clamp(stdDev * 0.28f, 3.5f, 5.2f);
    }

    return stdDev;
}
float PopupBackdropWgcLiquidRefractionBlurStdDev() {
    float stdDev = PopupBackdropWgcBlurStdDev();
    return Clamp(stdDev * 0.10f, 1.2f, 2.2f);
}
int PopupBackdropPadding() {
    return PopupSurfaceGap();
}
constexpr int kPopupCardHeight = 138;
constexpr int kPopupCompactCardHeight = 154;
constexpr int kPopupCompactMinimumCardWidth = 320;
constexpr int kPopupCardInnerPadding = 18;
constexpr int kPopupTextTopPadding = 13;
constexpr int kPopupTextLineGap = 24;
constexpr int kPopupTimeTextHeight = 16;
// G2-style popup radius family. Windhawk's UWP XAML surface does not expose
// a true squircle clip here, so keep the visual language consistent by routing
// every expanded popup corner through these shared radii.
constexpr double kPopupUnifiedCornerRadius = 24.0;
constexpr double kPopupG2ButtonCornerRadius = 14.0;
constexpr double kPopupG2ProgressCornerRadius = 3.0;

double PopupG2CornerRadius(double width, double height) {
    return std::max(1.0, std::min(width, height) * 0.118);
}

// Transparent host margin used only so the rounded XAML shadow can fit without
// falling back to the rectangular HWND shadow.
constexpr int kPopupHostShadowMargin = 18;
// The backdrop shell is the only outer background shape. In C mode its
// material is exactly the compact island material, so compact/expanded states
// share one color, one stroke model, and one predictable XAML clipping path.
constexpr double kPopupBackdropOpacity = 1.0;
constexpr double kPopupOverlayRadiusAdjustDip = 0.0;

bool UseOverlayPopupBackdropMaterial() {
    //          0.9.50 diagnostic: Acrylic uses a separate native overlay window behind
    // the XAML popup, clipped to the animated rounded popup shell instead of
    // the full transparent host rectangle. This keeps the visibility test from
    // leaking outside the actual visual surface.
    return IsBlurredGlassMaterial();
}

double PopupBackdropOverlayOpacity() {
    return 1.0;
}

BYTE PopupBackdropOverlayMaxAlpha() {
    return static_cast<BYTE>(
        Clamp(static_cast<int>(std::lround(255.0 * PopupBackdropOverlayOpacity())),
              0, 255));
}

bool PopupBackdropCoverEffectEnabled() {
    if (g_settings.popupBackdropCoverEffect == L"on") {
        return true;
    }
    if (IsLiquidGlassMaterial() && !IsDarkModeApprox()) {
        return true;
    }
    if (g_settings.popupBackdropCoverEffect == L"off") {
        return false;
    }
    // Default: keep the album color wash in dark mode. Liquid Glass also keeps
    // it in light mode so the glass surface remains readable.
    return IsDarkModeApprox();
}

bool PopupPanelCoverEffectEnabled() {
    if (IsLiquidGlassMaterial()) {
        return PopupBackdropCoverEffectEnabled();
    }

    return true;
}

int PopupTargetArtSize(int width, int height) {
    // finalRect is the host window. The visible compact-material surface sits
    // inside a small transparent margin so the shadow can be visible.
    int surfaceWidth = std::max(96, width - kPopupHostShadowMargin * 2);
    int surfaceHeight = std::max(96, height - kPopupHostShadowMargin * 2);
    int availableWidth = std::max(96, surfaceWidth - PopupSurfaceGap() * 2);
    int availableHeight = std::max(
        96,
        surfaceHeight - PopupSurfaceGap() * 2 - PopupArtCardGap() - kPopupCardHeight);
    return std::max(96, std::min(availableWidth, availableHeight));
}

RECT PopupBackdropRectFromParts(RECT const& artRect, RECT const& cardRect) {
    if (g_settings.compact) {
        return cardRect;
    }
    // The background shell is the tight bounding box around cover + control card.
    int left = std::min(artRect.left, cardRect.left);
    int top = std::min(artRect.top, cardRect.top);
    int right = std::max(artRect.right, cardRect.right);
    int bottom = std::max(artRect.bottom, cardRect.bottom);
    return {left - PopupBackdropPadding(),
            top - PopupBackdropPadding(),
            right + PopupBackdropPadding(),
            bottom + PopupBackdropPadding()};
}

int PopupFinalWidthFromArtSize(int artSize) {
    return artSize + PopupSurfaceGap() * 2 + kPopupHostShadowMargin * 2;
}

int PopupFinalHeightFromArtSize(int artSize) {
    return artSize + kPopupCardHeight + PopupArtCardGap() +
           PopupSurfaceGap() * 2 + kPopupHostShadowMargin * 2;
}

int PopupCompactFinalWidth(int cardWidth) {
    return cardWidth + kPopupHostShadowMargin * 2;
}

int PopupCompactFinalHeight() {
    return kPopupCompactCardHeight + kPopupHostShadowMargin * 2;
}

winrt::Windows::UI::Color PopupControlCardColor() {
    bool dark = IsDarkModeApprox();
    if (IsLiquidGlassMaterial()) {
        return dark ? Color(0x3A, 0x12, 0x14, 0x1A)
                    : Color(0x68, 0xFB, 0xFC, 0xFF);
    }

    // Keep light and dark tint strengths independent. Light mode keeps the
    // stronger white-tint experiment, while dark mode is rolled back to the
    // earlier compact-material-d-backdrop-card-tint values.
    if (dark) {
        return Color(0x70, 0x20, 0x20, 0x26);
    }
    return Color(0xD8, 0xFF, 0xFF, 0xFF);
}

mediax::Brush PopupControlCardBrush() {
    return Brush(PopupControlCardColor());
}

winrt::Windows::UI::Color PopupPrimaryTextColor() {
    bool dark = IsDarkModeApprox();
    return dark ? Color(0xFF, 0xFF, 0xFF, 0xFF)
                : Color(0xFF, 0x00, 0x00, 0x00);
}

winrt::Windows::UI::Color PopupSecondaryTextColor() {
    bool dark = IsDarkModeApprox();
    return dark ? Color(0xC8, 0xFF, 0xFF, 0xFF)
                : Color(0xC8, 0x00, 0x00, 0x00);
}

void NoteMediaNavigationDirection(int direction) {
    g_pendingMediaNavigationDirection = direction < 0 ? -1 : (direction > 0 ? 1 : 0);
    g_pendingMediaNavigationTime = std::chrono::steady_clock::now();
}

int ConsumeRecentMediaNavigationDirection() {
    int direction = g_pendingMediaNavigationDirection;
    if (direction == 0) {
        return 0;
    }

    auto now = std::chrono::steady_clock::now();
    auto ageMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                     now - g_pendingMediaNavigationTime)
                     .count();
    g_pendingMediaNavigationDirection = 0;
    return ageMs <= 4000 ? direction : 0;
}

mediax::Brush PopupTextForegroundBrush(winrt::Windows::UI::Color color,
                                       double edgeFadeAmount) {
    edgeFadeAmount = Clamp(edgeFadeAmount, 0.0, 1.0);
    auto clear = color;
    clear.A = static_cast<BYTE>(std::lround(color.A * (1.0 - edgeFadeAmount)));

    mediax::LinearGradientBrush brush;
    brush.StartPoint({0.0f, 0.5f});
    brush.EndPoint({1.0f, 0.5f});
    mediax::GradientStopCollection stops;

    auto addStop = [&](double offset, winrt::Windows::UI::Color const& stopColor) {
        mediax::GradientStop stop;
        stop.Offset(offset);
        stop.Color(stopColor);
        stops.Append(stop);
    };

    addStop(0.00, clear);
    addStop(0.16, color);
    addStop(0.84, color);
    addStop(1.00, clear);
    brush.GradientStops(stops);
    return brush;
}

void ApplyPopupTextForegroundFade(double edgeFadeAmount, bool force = false) {
    edgeFadeAmount = Clamp(edgeFadeAmount, 0.0, 1.0);
    bool dark = IsDarkModeApprox();
    if (!force && g_popupTextForegroundValid &&
        std::abs(g_popupTextForegroundEdgeFadeAmount - edgeFadeAmount) < 0.003 &&
        g_popupTextForegroundDark == dark) {
        return;
    }

    g_popupTextForegroundValid = true;
    g_popupTextForegroundEdgeFade = edgeFadeAmount > 0.001;
    g_popupTextForegroundEdgeFadeAmount = edgeFadeAmount;
    g_popupTextForegroundDark = dark;

    auto primary = PopupTextForegroundBrush(PopupPrimaryTextColor(), edgeFadeAmount);
    auto secondary = PopupTextForegroundBrush(PopupSecondaryTextColor(), edgeFadeAmount);
    auto secondarySolid = Brush(PopupSecondaryTextColor());

    if (g_popupXamlTitle) g_popupXamlTitle.Foreground(primary);
    if (g_popupXamlOutgoingTitle) g_popupXamlOutgoingTitle.Foreground(primary);
    if (g_popupXamlArtist) g_popupXamlArtist.Foreground(secondary);
    if (g_popupXamlOutgoingArtist) g_popupXamlOutgoingArtist.Foreground(secondary);
    if (g_popupXamlElapsed) g_popupXamlElapsed.Foreground(secondarySolid);
    if (g_popupXamlDuration) g_popupXamlDuration.Foreground(secondarySolid);
}

void ApplyPopupTextForegrounds(bool edgeFade, bool force = false) {
    ApplyPopupTextForegroundFade(edgeFade ? 1.0 : 0.0, force);
}

mediax::Brush PopupTextEdgeFadeBrush(bool leftEdge) {
    auto bg = PopupControlCardColor();
    if (IsLiquidGlassMaterial()) {
        bool dark = IsDarkModeApprox();
        bg = dark ? Color(0x28, 0x12, 0x14, 0x1A)
                  : Color(0x50, 0xFB, 0xFC, 0xFF);
    }
    auto clear = bg;
    clear.A = 0x00;

    mediax::LinearGradientBrush brush;
    brush.StartPoint({0.0, 0.5});
    brush.EndPoint({1.0, 0.5});
    mediax::GradientStopCollection stops;

    auto addStop = [&](double offset, winrt::Windows::UI::Color const& color) {
        mediax::GradientStop stop;
        stop.Offset(offset);
        stop.Color(color);
        stops.Append(stop);
    };

    if (leftEdge) {
        addStop(0.00, bg);
        addStop(0.42, bg);
        addStop(1.00, clear);
    } else {
        addStop(0.00, clear);
        addStop(0.58, bg);
        addStop(1.00, bg);
    }
    brush.GradientStops(stops);
    return brush;
}

double PopupTextClipPadding() {
    return 2.0;
}

double PopupTextEdgeFadeWidth() {
    return IsLiquidGlassMaterial() ? 24.0 : 18.0;
}

void ConfigurePopupTextEdgeFade(Border const& fade,
                                bool leftEdge,
                                double height) {
    if (!fade) {
        return;
    }

    double clipPadding = PopupTextClipPadding();
    fade.Width(PopupTextEdgeFadeWidth() + clipPadding);
    fade.Height(std::max(1.0, height));
    fade.Margin(leftEdge ? xaml::Thickness{-clipPadding, 0, 0, 0}
                         : xaml::Thickness{0, 0, -clipPadding, 0});
    fade.Background(PopupTextEdgeFadeBrush(leftEdge));
}

void SetPopupTextEdgeFadeOpacity(double opacity) {
    opacity = Clamp(opacity, 0.0, 1.0);
    try {
        Border fades[] = {
            g_popupXamlTitleLeftFade,
            g_popupXamlTitleRightFade,
            g_popupXamlArtistLeftFade,
            g_popupXamlArtistRightFade,
            g_popupXamlOutgoingTitleLeftFade,
            g_popupXamlOutgoingTitleRightFade,
            g_popupXamlOutgoingArtistLeftFade,
            g_popupXamlOutgoingArtistRightFade,
        };
        for (Border const& fade : fades) {
            if (fade) {
                fade.Opacity(opacity);
            }
        }
    } catch (...) {
    }
}

mediax::Brush PopupBackdropCardTintBrush() {
    bool dark = IsDarkModeApprox();
    bool preserveCompactWash =
        g_settings.compact && PopupBackdropCoverEffectEnabled();
    if (g_settings.material == L"mica_like") {
        // Mica-like already carries its theme tint in IslandBackgroundColor().
        // A second light-mode white veil makes the expanded surface nearly
        // opaque white and breaks compact-to-expanded material continuity.
        return Brush(Color(0x00, 0x00, 0x00, 0x00));
    }

    if (UseOverlayPopupBackdropMaterial()) {
        if (IsLiquidGlassMaterial()) {
            if (!dark) {
                mediax::LinearGradientBrush brush;
                brush.StartPoint({0.0f, 1.0f});
                brush.EndPoint({0.0f, 0.0f});
                mediax::GradientStopCollection stops;
                auto addStop = [&](double offset, BYTE alpha) {
                    mediax::GradientStop stop;
                    stop.Offset(offset);
                    stop.Color(Color(alpha, 0xF6, 0xF8, 0xFC));
                    stops.Append(stop);
                };

                addStop(0.00, preserveCompactWash ? 0x42 : 0x5C);
                addStop(0.24, preserveCompactWash ? 0x30 : 0x42);
                addStop(0.58, preserveCompactWash ? 0x16 : 0x22);
                addStop(1.00, 0x00);
                brush.GradientStops(stops);
                return brush;
            }
            return Brush(Color(preserveCompactWash ? 0x00 : 0x04,
                               0xFF, 0xFF, 0xFF));
        }

        // Native blur comes from the overlay window itself. Add only a clean
        // light-mode white veil here so acrylic keeps enough tint without the
        // compact island inheriting AcrylicBrush's inactive fallback behavior.
        return Brush(dark ? Color(0x00, 0x00, 0x00, 0x00)
                          : Color(preserveCompactWash ? 0x68 : 0x8C,
                                  0xFF, 0xFF, 0xFF));
    }
    // Backdrop and control tints are tuned separately so light mode can stay
    // brighter without making dark mode look washed out. In dark mode this
    // matches the previous version, where the backdrop card tint reused the
    // control-card tint.
    if (dark) {
        return Brush(Color(preserveCompactWash ? 0x58 : 0x70,
                           0x20, 0x20, 0x26));
    }
    return Brush(Color(preserveCompactWash ? 0xB8 : 0xE8,
                       0xFF, 0xFF, 0xFF));
}

double PopupPanelCoverOpacityFactor() {
    if (IsLiquidGlassMaterial()) {
        return IsDarkModeApprox() ? 0.30 : 0.36;
    }

    // The stronger light-mode white tint can hide the album color wash too much.
    // Boost only the light-mode cover-wash opacity; keep dark mode unchanged.
    return IsDarkModeApprox() ? 0.44 : 0.58;
}

mediax::Brush PopupSurfaceBrush() {
    if (UseOverlayPopupBackdropMaterial()) {
        // Let the native overlay window provide the real-time blurred material.
        // The XAML shell itself stays transparent so the clipped overlay can
        // show through cleanly beneath the controls and artwork.
        return Brush(Color(0x00, 0x00, 0x00, 0x00));
    }
    // C version: extend the compact island material to the expanded surface.
    // This keeps compact->expanded transitions visually consistent and avoids
    // mixing DWM backdrop, XAML Acrylic, and extra tint layers.
    return IslandBackgroundBrush();
}

mediax::Brush PopupSurfaceStrokeBrush() {
    if (UseOverlayPopupBackdropMaterial()) {
        if (IsLiquidGlassMaterial()) {
            return LiquidGlassRimHighlightBrush();
        }

        bool dark = IsDarkModeApprox();
        return Brush(dark ? Color(0x10, 0xFF, 0xFF, 0xFF)
                          : Color(0x00, 0x00, 0x00, 0x00));
    }
    // Match the compact island outline exactly: the expanded popup surface
    // should use the same subtle theme-aware stroke and no extra DWM border.
    return IslandBorderBrush();
}

mediax::Brush PopupShadowBrush() {
    bool dark = IsDarkModeApprox();
    // Rounded XAML-only shadow. Do not use CS_DROPSHADOW/DWM shadow here:
    // those shadow the transparent rectangular host instead of this surface.
    return Brush(dark ? Color(0x5C, 0x00, 0x00, 0x00)
                      : Color(0x46, 0x00, 0x00, 0x00));
}

void CalculatePopupFinalLayout(RECT const& finalRect,
                               RECT& artRect,
                               RECT& cardRect,
                               RECT& titleRect,
                               RECT& artistRect,
                               RECT& progressRect,
                               RECT& elapsedRect,
                               RECT& durationRect,
                               RECT& controlsRect) {
    RECT surfaceRect{finalRect.left + kPopupHostShadowMargin,
                     finalRect.top + kPopupHostShadowMargin,
                     finalRect.right - kPopupHostShadowMargin,
                     finalRect.bottom - kPopupHostShadowMargin};
    int finalWidth = surfaceRect.right - surfaceRect.left;

    if (g_settings.compact) {
        cardRect = surfaceRect;
        constexpr int inset = 14;
        int cardHeight = cardRect.bottom - cardRect.top;
        int artSize = std::max(72, cardHeight - inset * 2);
        int artTop = cardRect.top + inset;
        artRect = {cardRect.left + inset, artTop,
                   cardRect.left + inset + artSize, artTop + artSize};

        int contentLeft = artRect.right + inset;
        int contentRight = cardRect.right - inset;
        int textTop = artRect.top + 3;
        titleRect = {contentLeft, textTop,
                     contentRight, textTop + 24};
        artistRect = {contentLeft, textTop + 24,
                      contentRight, textTop + 45};
        int progressSideInset = artRect.left - cardRect.left;
        progressRect = {artRect.right + progressSideInset,
                        cardRect.top + 66,
                        cardRect.right - progressSideInset,
                        cardRect.top + 70};
        elapsedRect = {contentLeft, cardRect.top + 72,
                       contentLeft + 68, cardRect.top + 88};
        durationRect = {contentRight - 68, cardRect.top + 72,
                        contentRight, cardRect.top + 88};
        constexpr int controlsVisualOffset = 6;
        controlsRect = {contentLeft - controlsVisualOffset, cardRect.top + 96,
                        contentRight - controlsVisualOffset,
                        cardRect.bottom - inset};
        return;
    }

    int targetArt = PopupTargetArtSize(finalRect.right - finalRect.left,
                                       finalRect.bottom - finalRect.top);
    int artLeft = surfaceRect.left + (finalWidth - targetArt) / 2;
    if (g_popupOpensDown) {
        int cardTop = surfaceRect.top + PopupSurfaceGap();
        cardRect = {artLeft, cardTop, artLeft + targetArt,
                    cardTop + kPopupCardHeight};

        int artTop = cardRect.bottom + PopupArtCardGap();
        artRect = {artLeft, artTop, artLeft + targetArt, artTop + targetArt};
    } else {
        int artTop = surfaceRect.top + PopupSurfaceGap();
        artRect = {artLeft, artTop, artLeft + targetArt, artTop + targetArt};

        int cardTop = artRect.bottom + PopupArtCardGap();
        cardRect = {artRect.left, cardTop, artRect.right,
                    cardTop + kPopupCardHeight};
    }

    int contentLeft = cardRect.left + kPopupCardInnerPadding;
    int contentRight = cardRect.right - kPopupCardInnerPadding;
    titleRect = {contentLeft,
                 cardRect.top + kPopupTextTopPadding,
                 contentRight,
                 cardRect.top + kPopupTextTopPadding + 24};
    artistRect = {contentLeft,
                  cardRect.top + kPopupTextTopPadding + kPopupTextLineGap,
                  contentRight,
                  cardRect.top + kPopupTextTopPadding + kPopupTextLineGap + 21};
    progressRect = {contentLeft,
                    cardRect.top + 67,
                    contentRight,
                    cardRect.top + 71};
    elapsedRect = {contentLeft,
                   cardRect.top + 73,
                   contentLeft + 78,
                   cardRect.top + 73 + kPopupTimeTextHeight};
    durationRect = {contentRight - 78,
                    cardRect.top + 73,
                    contentRight,
                    cardRect.top + 73 + kPopupTimeTextHeight};
    controlsRect = {contentLeft,
                    cardRect.top + 94,
                    contentRight,
                    cardRect.bottom - 10};
}

RECT UnionPopupRects(RECT const& a, RECT const& b) {
    RECT result{std::min(a.left, b.left),
                std::min(a.top, b.top),
                std::max(a.right, b.right),
                std::max(a.bottom, b.bottom)};
    // A tiny pad prevents the animated XAML content from touching the host edge
    // during integer-rounded morph frames. The host remains visually transparent.
    InflateRect(&result, 2, 2);
    return result;
}

RECT CurrentPopupSurfaceScreenRect() {
    // Used only for outside-click hit testing. Keep this in physical screen
    // pixels and use the full animated host bounds; the precise XAML surface is
    // calculated in DIP coordinates in UpdatePopupXamlVisuals().
    double progress = PopupProgress();
    return {LerpInt(g_popupSourceRect.left, g_popupFinalRect.left, progress),
            LerpInt(g_popupSourceRect.top, g_popupFinalRect.top, progress),
            LerpInt(g_popupSourceRect.right, g_popupFinalRect.right, progress),
            LerpInt(g_popupSourceRect.bottom, g_popupFinalRect.bottom, progress)};
}

bool PopupScreenPointIsInsideCurrentSurface(POINT screenPoint) {
    if (!g_expandedPopup || !IsWindowVisible(g_expandedPopup) ||
        !g_expanded || g_popupClosing) {
        return false;
    }

    RECT hitRect{};
    if (g_popupXamlRoot) {
        hitRect = CurrentPopupSurfaceScreenRect();
    } else {
        GetWindowRect(g_expandedPopup, &hitRect);
    }
    if (hitRect.right <= hitRect.left || hitRect.bottom <= hitRect.top) {
        GetWindowRect(g_expandedPopup, &hitRect);
    }

    InflateRect(&hitRect, 8, 8);
    return PtInRect(&hitRect, screenPoint) != FALSE;
}

LRESULT PopupExpandedHitTest(LPARAM lParam) {
    POINT screenPoint{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    return PopupScreenPointIsInsideCurrentSurface(screenPoint)
               ? HTCLIENT
               : HTTRANSPARENT;
}

constexpr UINT_PTR kPopupXamlChildSubclassId = 1;

LRESULT CALLBACK PopupXamlChildSubclassProc(HWND hwnd,
                                            UINT message,
                                            WPARAM wParam,
                                            LPARAM lParam,
                                            UINT_PTR,
                                            DWORD_PTR) {
    if (message == WM_NCHITTEST) {
        LRESULT hit = PopupExpandedHitTest(lParam);
        if (hit == HTTRANSPARENT) {
            return HTTRANSPARENT;
        }
    } else if (message == WM_MOUSEACTIVATE) {
        return MA_NOACTIVATE;
    } else if (message == WM_NCDESTROY) {
        RemoveWindowSubclass(hwnd,
                             PopupXamlChildSubclassProc,
                             kPopupXamlChildSubclassId);
        if (g_popupXamlChild == hwnd) {
            g_popupXamlChild = nullptr;
            g_popupXamlChildSubclassed = false;
        }
    }

    return DefSubclassProc(hwnd, message, wParam, lParam);
}

void SubclassPopupXamlChildWindow() {
    if (!g_popupXamlChild || g_popupXamlChildSubclassed) {
        return;
    }
    if (SetWindowSubclass(g_popupXamlChild,
                          PopupXamlChildSubclassProc,
                          kPopupXamlChildSubclassId,
                          0)) {
        g_popupXamlChildSubclassed = true;
    }
}

void UnsubclassPopupXamlChildWindow() {
    if (!g_popupXamlChild || !g_popupXamlChildSubclassed) {
        return;
    }
    RemoveWindowSubclass(g_popupXamlChild,
                         PopupXamlChildSubclassProc,
                         kPopupXamlChildSubclassId);
    g_popupXamlChildSubclassed = false;
}

constexpr int kPopupHiddenParkingX = -32000;
constexpr int kPopupHiddenParkingY = -32000;

void SetPopupWindowClickThrough(HWND hwnd, bool clickThrough) {
    if (!hwnd || !IsWindow(hwnd)) {
        return;
    }

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    LONG_PTR targetStyle =
        clickThrough ? (style | WS_EX_TRANSPARENT)
                     : (style & ~static_cast<LONG_PTR>(WS_EX_TRANSPARENT));
    if (targetStyle != style) {
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, targetStyle);
    }
}

void ParkPopupWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return;
    }

    SetWindowPos(hwnd,
                 nullptr,
                 kPopupHiddenParkingX,
                 kPopupHiddenParkingY,
                 1,
                 1,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER |
                     SWP_NOCOPYBITS | SWP_NOSENDCHANGING | SWP_HIDEWINDOW);
}

void ParkExpandedPopupWindow(HWND hwnd) {
    SetPopupWindowClickThrough(hwnd, true);
    SetPopupWindowClickThrough(g_popupXamlChild, true);
    if (g_popupXamlChild && IsWindow(g_popupXamlChild)) {
        MoveWindow(g_popupXamlChild, 0, 0, 1, 1, FALSE);
    }
    ParkPopupWindow(hwnd);
}

bool PopupShouldExpandRight(MONITORINFO const& monitorInfo) {
    // Prefer the explicit taskbar-position setting over screen-center guessing.
    // In tray placements, the old behavior felt better: tray_left grows from
    // left to right, while the right-side tray placements grow right to left.
    if (g_settings.position == L"tray_left" ||
        g_settings.position == L"taskbar_left_edge") {
        return true;
    }
    if (g_settings.position == L"tray_right" ||
        g_settings.position == L"tray_before_clock" ||
        g_settings.position == L"tray_after_clock" ||
        g_settings.position == L"taskbar_right_edge") {
        return false;
    }

    int sourceCenter = (g_popupSourceRect.left + g_popupSourceRect.right) / 2;
    int monitorCenter = (monitorInfo.rcMonitor.left + monitorInfo.rcMonitor.right) / 2;
    return sourceCenter <= monitorCenter;
}

constexpr int kDwmCornerDoNotRound = 1;
constexpr int kDwmBackdropNone = 1;
constexpr DWORD kDwmColorNone = 0xFFFFFFFE;  // DWMWA_COLOR_NONE.

DWORD PopupNativeBorderColorRef() {
    bool dark = IsDarkModeApprox();
    // DWMWA_BORDER_COLOR takes a COLORREF-style value, not an ARGB color.
    // These values mirror the subtle Windows 11 window outline: a soft gray in
    // dark mode and a bright white edge in light mode.
    return dark ? RGB(74, 74, 82) : RGB(255, 255, 255);
}

void SetPopupDwmAttributes(HWND hwnd, int cornerPreference, int backdropType, bool nativeBorder = false) {
    DWORD borderColor = nativeBorder ? PopupNativeBorderColorRef() : kDwmColorNone;
    BOOL dark = IsDarkModeApprox();
    DwmSetWindowAttribute(hwnd, 34, &borderColor, sizeof(borderColor));
    DwmSetWindowAttribute(hwnd, 20, &dark, sizeof(dark));
    DwmSetWindowAttribute(hwnd, 33, &cornerPreference, sizeof(cornerPreference));
    DwmSetWindowAttribute(hwnd, 38, &backdropType, sizeof(backdropType));
}

void ApplyPopupBackdrop(HWND hwnd) {
    if (g_popupXamlRoot) {
        // C version: the host HWND only carries XAML. All visible material is
        // the same rounded compact-material Border used by the taskbar island.
        // Keep the host HWND visually empty. The only visible outline is the
        // same XAML IslandBorderBrush() stroke used by the compact island.
        SetPopupDwmAttributes(hwnd, kDwmCornerDoNotRound, kDwmBackdropNone, false);
        return;
    }

    struct AccentPolicy {
        int state;
        int flags;
        DWORD gradientColor;
        int animationId;
    };
    struct CompositionAttributeData {
        int attribute;
        void* data;
        SIZE_T size;
    };
    using SetWindowCompositionAttribute_t = BOOL(WINAPI*)(
        HWND, CompositionAttributeData*);

    static auto setWindowCompositionAttribute =
        reinterpret_cast<SetWindowCompositionAttribute_t>(
            GetProcAddress(GetModuleHandleW(L"user32.dll"),
                           "SetWindowCompositionAttribute"));
    if (setWindowCompositionAttribute) {
        bool dark = IsDarkModeApprox();
        AccentPolicy policy{
            4,  // ACCENT_ENABLE_ACRYLICBLURBEHIND.
            2,
            dark ? static_cast<DWORD>(0x702F2A2A)
                 : static_cast<DWORD>(0x78F6F3F3),
            0,
        };
        CompositionAttributeData data{19, &policy, sizeof(policy)};
        setWindowCompositionAttribute(hwnd, &data);
    }
}

RECT PopupButtonRect(int index, int width, int height) {
    RECT localFinal{0, 0, width, height};
    RECT art{}, card{}, title{}, artist{}, progress{}, elapsed{}, duration{}, controls{};
    CalculatePopupFinalLayout(localFinal, art, card, title, artist, progress, elapsed, duration, controls);

    int buttonSize = Clamp(static_cast<int>((controls.right - controls.left - 16) / 3), 30, 38);
    int gap = 8;
    int total = buttonSize * 3 + gap * 2;
    int left = (controls.left + controls.right - total) / 2 + index * (buttonSize + gap);
    int top = controls.top + ((controls.bottom - controls.top) - buttonSize) / 2;
    return {left, top, left + buttonSize, top + buttonSize};
}

void PaintExpandedPopup(HWND hwnd, HDC dc) {
    RECT client{};
    GetClientRect(hwnd, &client);
    int width = client.right;
    int height = client.bottom;
    if (width <= 0 || height <= 0) {
        return;
    }

    bool dark = IsDarkModeApprox();
    COLORREF primary = dark ? RGB(255, 255, 255) : RGB(0, 0, 0);
    COLORREF secondary = dark ? RGB(190, 190, 196) : RGB(82, 82, 88);
    COLORREF subtle = dark ? RGB(72, 72, 78) : RGB(218, 218, 224);
    double progress = PopupProgress();

    RECT popupScreen{};
    GetWindowRect(hwnd, &popupScreen);
    RECT targetArtScreen{};
    RECT targetCardScreen{};
    RECT targetTitleScreen{};
    RECT targetArtistScreen{};
    RECT targetProgressScreen{};
    RECT targetElapsedScreen{};
    RECT targetDurationScreen{};
    RECT targetControlsScreen{};
    CalculatePopupFinalLayout(g_popupFinalRect, targetArtScreen, targetCardScreen,
                              targetTitleScreen, targetArtistScreen,
                              targetProgressScreen, targetElapsedScreen, targetDurationScreen,
                              targetControlsScreen);
    double artProgress = progress;
    RECT artScreen{
        LerpInt(g_popupSourceArtRect.left, targetArtScreen.left, artProgress),
        LerpInt(g_popupSourceArtRect.top, targetArtScreen.top, artProgress),
        LerpInt(g_popupSourceArtRect.right, targetArtScreen.right, artProgress),
        LerpInt(g_popupSourceArtRect.bottom, targetArtScreen.bottom, artProgress),
    };
    RECT artRect{
        artScreen.left - popupScreen.left,
        artScreen.top - popupScreen.top,
        artScreen.right - popupScreen.left,
        artScreen.bottom - popupScreen.top,
    };
    int artSize = std::max(1, static_cast<int>(artRect.right - artRect.left));

    int artRadius = static_cast<int>(kPopupUnifiedCornerRadius);
    HRGN artClip = CreateRoundRectRgn(artRect.left, artRect.top,
                                      artRect.right + 1, artRect.bottom + 1,
                                      artRadius * 2, artRadius * 2);
    int saved = SaveDC(dc);
    SelectClipRgn(dc, artClip);
    if (g_popupAlbumBitmap) {
        HDC source = CreateCompatibleDC(dc);
        HGDIOBJ oldBitmap = SelectObject(source, g_popupAlbumBitmap);
        BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        AlphaBlend(dc, artRect.left, artRect.top, artSize, artSize,
                   source, 0, 0, 256, 256, blend);
        SelectObject(source, oldBitmap);
        DeleteDC(source);
    } else {
        HBRUSH fallback = CreateSolidBrush(RGB(79, 125, 232));
        FillRect(dc, &artRect, fallback);
        DeleteObject(fallback);
    }
    RestoreDC(dc, saved);
    DeleteObject(artClip);

    MediaState state = SnapshotMedia();
    SetBkMode(dc, TRANSPARENT);
    int titleSize = LerpInt(12, 16, progress);
    int artistSize = LerpInt(10, 13, progress);
    HFONT titleFont = CreateFontW(-titleSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                  CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                  DEFAULT_PITCH, L"Segoe UI Variable Text");
    HFONT artistFont = CreateFontW(-artistSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                   DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                   CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                   DEFAULT_PITCH, L"Segoe UI Variable Text");

    RECT titleScreen{
        LerpInt(g_popupSourceTitleRect.left, targetTitleScreen.left, progress),
        LerpInt(g_popupSourceTitleRect.top, targetTitleScreen.top, progress),
        LerpInt(g_popupSourceTitleRect.right, targetTitleScreen.right, progress),
        LerpInt(g_popupSourceTitleRect.bottom, targetTitleScreen.bottom, progress),
    };
    RECT artistScreen{
        LerpInt(g_popupSourceArtistRect.left, targetArtistScreen.left, progress),
        LerpInt(g_popupSourceArtistRect.top, targetArtistScreen.top, progress),
        LerpInt(g_popupSourceArtistRect.right, targetArtistScreen.right, progress),
        LerpInt(g_popupSourceArtistRect.bottom, targetArtistScreen.bottom, progress),
    };
    RECT titleRect{titleScreen.left - popupScreen.left,
                   titleScreen.top - popupScreen.top,
                   titleScreen.right - popupScreen.left,
                   titleScreen.bottom - popupScreen.top};
    RECT artistRect{artistScreen.left - popupScreen.left,
                    artistScreen.top - popupScreen.top,
                    artistScreen.right - popupScreen.left,
                    artistScreen.bottom - popupScreen.top};
    HGDIOBJ oldFont = SelectObject(dc, titleFont);
    SetTextColor(dc, primary);
    std::wstring title = state.hasSession ? state.title : L"No media";
    DrawTextW(dc, title.c_str(), -1, &titleRect,
              DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

    SelectObject(dc, artistFont);
    SetTextColor(dc, secondary);
    std::wstring artist = state.artist.empty()
                              ? (state.isPlaying ? L"Playing" : L"Paused")
                              : state.artist;
    DrawTextW(dc, artist.c_str(), -1, &artistRect,
              DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

    if (progress >= 0.45) {
        RECT progressTrack{targetProgressScreen.left - popupScreen.left,
                           targetProgressScreen.top - popupScreen.top,
                           targetProgressScreen.right - popupScreen.left,
                           targetProgressScreen.bottom - popupScreen.top};
        HBRUSH trackBrush = CreateSolidBrush(subtle);
        FillRect(dc, &progressTrack, trackBrush);
        DeleteObject(trackBrush);
        double ratio = state.durationTicks > 0
                           ? Clamp(static_cast<double>(state.positionTicks) /
                                       static_cast<double>(state.durationTicks),
                                   0.0, 1.0)
                           : 0.0;
        RECT progressValue = progressTrack;
        progressValue.right = progressValue.left +
                              static_cast<int>((progressTrack.right - progressTrack.left) * ratio);
        HBRUSH valueBrush = CreateSolidBrush(dark ? RGB(242, 242, 245) : RGB(42, 42, 46));
        FillRect(dc, &progressValue, valueBrush);
        DeleteObject(valueBrush);

        HFONT iconFont = CreateFontW(-16, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                     CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                                     DEFAULT_PITCH, L"Segoe Fluent Icons");
        SelectObject(dc, iconFont);
        const wchar_t* glyphs[] = {L"\uE892", state.isPlaying ? L"\uE769" : L"\uE768", L"\uE893"};
        for (int i = 0; i < 3; ++i) {
            RECT button = PopupButtonRect(i, width, height);
            SetTextColor(dc, primary);
            DrawTextW(dc, glyphs[i], -1, &button,
                      DT_CENTER | DT_SINGLELINE | DT_VCENTER);
        }
        SelectObject(dc, oldFont);
        DeleteObject(iconFont);
    }

    SelectObject(dc, oldFont);
    DeleteObject(artistFont);
    DeleteObject(titleFont);
}

bool PopupButtonStyleIs(std::wstring_view style) {
    return std::wstring_view(g_settings.popupButtonStyle) == style;
}

bool IsPopupPrimaryButton(Border const& surface) {
    try {
        return winrt::unbox_value_or<winrt::hstring>(
                   surface.Tag(), winrt::hstring{}) == L"primary";
    } catch (...) {
        return false;
    }
}

winrt::hstring PopupTransportGlyph(Border const& surface, bool playing) {
    std::wstring name(surface.Name());
    if (name == L"Popup_Prev") {
        return winrt::hstring(L"\uE892");
    }
    if (name == L"Popup_Next") {
        return winrt::hstring(L"\uE893");
    }
    return playing ? winrt::hstring(L"\uE769") : winrt::hstring(L"\uE768");
}

void UpdatePopupTransportButtonGlyph(Border const& surface, bool playing) {
    if (auto icon = surface.Child().try_as<controls::FontIcon>()) {
        icon.Glyph(PopupTransportGlyph(surface, playing));
    } else if (auto text = surface.Child().try_as<TextBlock>()) {
        text.Text(PopupTransportGlyph(surface, playing));
    }
}

double PopupControlsWidth() {
    return 126.0;
}

double PopupControlsHeight() {
    return 38.0;
}

double PopupButtonWidth() {
    return 38.0;
}

double PopupButtonHeight() {
    return 38.0;
}

double PopupButtonTop() {
    return (PopupControlsHeight() - PopupButtonHeight()) * 0.5;
}

double PopupButtonLeft(int index) {
    return 2.0 + 42.0 * static_cast<double>(index);
}

mediax::Brush PopupButtonHoverBrush(bool dark) {
    return Brush(dark ? Color(0x26, 0xFF, 0xFF, 0xFF)
                      : Color(0x18, 0x00, 0x00, 0x00));
}

void ApplyPopupButtonVisual(Border const& surface, bool hovered = false) {
    bool dark = IsDarkModeApprox();
    bool primary = IsPopupPrimaryButton(surface);
    auto primaryText = dark ? Color(0xFF, 0xFF, 0xFF, 0xFF)
                            : Color(0xFF, 0x00, 0x00, 0x00);

    double width = PopupButtonWidth();
    double height = PopupButtonHeight();
    double corner = std::min(height * 0.5, kPopupG2ButtonCornerRadius);
    double fontSize = primary ? 21.0 : 20.0;
    mediax::Brush background = hovered ? PopupButtonHoverBrush(dark)
                                       : Brush(Color(0x00, 0x00, 0x00, 0x00));
    mediax::Brush foreground = Brush(primaryText);

    if (PopupButtonStyleIs(L"fluent_bold")) {
        fontSize = primary ? 23.0 : 22.0;
    }

    surface.Width(width);
    surface.Height(height);
    surface.Visibility(Visibility::Visible);
    surface.CornerRadius({corner, corner, corner, corner});
    surface.Background(background);
    surface.BorderThickness({0, 0, 0, 0});
    surface.BorderBrush(Brush(Color(0x00, 0x00, 0x00, 0x00)));

    bool playing = SnapshotMedia().isPlaying;
    if (auto icon = surface.Child().try_as<controls::FontIcon>()) {
        icon.Visibility(Visibility::Visible);
        icon.Opacity(1.0);
        icon.Width(width);
        icon.Height(height);
        icon.Margin({0, 0, 0, 0});
        icon.HorizontalAlignment(HorizontalAlignment::Center);
        icon.VerticalAlignment(VerticalAlignment::Center);
        icon.Glyph(PopupTransportGlyph(surface, playing));
        icon.Foreground(foreground);
        icon.FontFamily(mediax::FontFamily(L"Segoe Fluent Icons"));
        icon.FontSize(fontSize);
        icon.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    } else if (auto text = surface.Child().try_as<TextBlock>()) {
        text.Visibility(Visibility::Visible);
        text.Opacity(1.0);
        text.Width(width);
        text.Height(height);
        text.Margin({0, 0, 0, 0});
        text.HorizontalAlignment(HorizontalAlignment::Stretch);
        text.VerticalAlignment(VerticalAlignment::Center);
        text.TextAlignment(xaml::TextAlignment::Center);
        text.Text(PopupTransportGlyph(surface, playing));
        text.Foreground(foreground);
        text.FontFamily(mediax::FontFamily(L"Segoe Fluent Icons"));
        text.FontSize(fontSize);
        text.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
    }
}

Border MakePopupXamlButton(const wchar_t* name, void (*onClick)(), bool primary = false) {
    Border surface;
    surface.Name(name);
    surface.Margin({0, 0, 0, 0});
    surface.Padding({0, 0, 0, 0});
    surface.Tag(winrt::box_value(winrt::hstring(primary ? L"primary" : L"secondary")));
    surface.IsHitTestVisible(true);

    controls::FontIcon icon;
    icon.FontFamily(mediax::FontFamily(L"Segoe Fluent Icons"));
    icon.Glyph(L"\uE768");
    icon.HorizontalAlignment(HorizontalAlignment::Center);
    icon.VerticalAlignment(VerticalAlignment::Center);
    surface.Child(icon);
    ApplyPopupButtonVisual(surface, false);

    surface.PointerEntered([](auto const& sender, input::PointerRoutedEventArgs const&) {
        if (auto border = sender.template try_as<Border>()) {
            ApplyPopupButtonVisual(border, true);
        }
    });
    surface.PointerExited([](auto const& sender, input::PointerRoutedEventArgs const&) {
        if (auto border = sender.template try_as<Border>()) {
            ApplyPopupButtonVisual(border, false);
        }
    });
    surface.PointerPressed([](auto const& sender, input::PointerRoutedEventArgs const& args) {
        if (auto border = sender.template try_as<Border>()) {
            border.Opacity(0.82);
        }
        args.Handled(true);
    });
    surface.PointerReleased([onClick](auto const& sender, input::PointerRoutedEventArgs const& args) {
        if (auto border = sender.template try_as<Border>()) {
            border.Opacity(1.0);
            ApplyPopupButtonVisual(border, false);
        }
        onClick();
        args.Handled(true);
    });
    surface.PointerCanceled([](auto const& sender, input::PointerRoutedEventArgs const&) {
        if (auto border = sender.template try_as<Border>()) {
            border.Opacity(1.0);
            ApplyPopupButtonVisual(border, false);
        }
    });
    return surface;
}

void InitializePopupCompositionShadow();
void ApplyPopupDynamicAccentVisuals();
void UpdatePopupSeekPreview(double ratio);
void EndPopupSeek(bool commit);

void ApplyPopupXamlTheme(bool force = false) {
    if (!g_popupXamlPanel) {
        return;
    }

    bool dark = IsDarkModeApprox();
    if (!force && g_popupXamlThemeValid &&
        g_popupXamlThemeDark == dark &&
        g_popupXamlThemeMaterial == g_settings.material &&
        g_popupXamlThemeButtonStyle == g_settings.popupButtonStyle &&
        g_popupXamlThemeShadowDepth == PopupShadowDepth() &&
        g_popupXamlThemeShadowOpacity == PopupShadowOpacityPercent()) {
        return;
    }
    g_popupXamlThemeValid = true;
    g_popupXamlThemeDark = dark;
    g_popupXamlThemeMaterial = g_settings.material;
    g_popupXamlThemeButtonStyle = g_settings.popupButtonStyle;
    g_popupXamlThemeShadowDepth = PopupShadowDepth();
    g_popupXamlThemeShadowOpacity = PopupShadowOpacityPercent();
    if (g_popupXamlShadow) {
        g_popupXamlShadow.Background(PopupShadowBrush());
        g_popupXamlShadow.BorderThickness({0, 0, 0, 0});
    }
    if (g_popupXamlBackdrop) {
        g_popupXamlBackdrop.Background(PopupSurfaceBrush());
        bool useNativeHighlightStroke = !IsLiquidGlassMaterial();
        g_popupXamlBackdrop.BorderThickness(
            useNativeHighlightStroke
                ? (g_settings.compact
                       ? Thickness{1.15, 1.15, 1.15, 1.15}
                       : Thickness{1.0, 1.0, 1.0, 1.0})
                : Thickness{0.0, 0.0, 0.0, 0.0});
        g_popupXamlBackdrop.BorderBrush(
            useNativeHighlightStroke ? PopupAlbumArtStrokeBrush()
                                     : PopupSurfaceStrokeBrush());
        InitializePopupCompositionShadow();
    }
    if (g_popupXamlBackdropTint) {
        g_popupXamlBackdropTint.Background(PopupBackdropCardTintBrush());
        g_popupXamlBackdropTint.BorderThickness({0, 0, 0, 0});
    }
    if (g_popupXamlBackdropSurfaceHighlight) {
        g_popupXamlBackdropSurfaceHighlight.Background(LiquidGlassSurfaceHighlightBrush());
        g_popupXamlBackdropSurfaceHighlight.BorderThickness({0, 0, 0, 0});
    }
    if (g_popupXamlBackdropRimHighlight) {
        g_popupXamlBackdropRimHighlight.Background(Brush(Color(0x00, 0x00, 0x00, 0x00)));
        g_popupXamlBackdropRimHighlight.BorderBrush(LiquidGlassRimHighlightBrush());
        g_popupXamlBackdropRimHighlight.BorderThickness(IsLiquidGlassMaterial()
                                                            ? Thickness{1.0, 1.0, 1.0, 1.0}
                                                            : Thickness{0.0, 0.0, 0.0, 0.0});
    }
    if (g_popupXamlDropShadow) {
        float strength = static_cast<float>(PopupShadowOpacityPercent()) / 100.0f;
        g_popupXamlDropShadow.BlurRadius(28.0f + static_cast<float>(PopupShadowDepth()) * 0.35f);
        g_popupXamlDropShadow.Opacity((dark ? 0.70f : 0.56f) * strength);
        g_popupXamlDropShadow.Offset({0.0f, 6.0f + static_cast<float>(PopupShadowDepth()) * 0.08f, 0.0f});
        g_popupXamlDropShadow.Color(Color(0xFF, 0x00, 0x00, 0x00));
    }
    if (g_popupXamlPanelCoverFrame) {
        g_popupXamlPanelCoverFrame.Background(Brush(Color(0x00, 0x00, 0x00, 0x00)));
        g_popupXamlPanelCoverFrame.BorderThickness({0, 0, 0, 0});
        g_popupXamlPanelCoverFrame.BorderBrush(Brush(Color(0x00, 0x00, 0x00, 0x00)));
    }
    if (g_popupXamlArtFrame) {
        g_popupXamlArtFrame.BorderThickness(IsLiquidGlassMaterial()
                                                ? Thickness{1.35, 1.35, 1.35, 1.35}
                                                : Thickness{1.2, 1.2, 1.2, 1.2});
        g_popupXamlArtFrame.BorderBrush(PopupAlbumArtStrokeBrush());
    }
    g_popupXamlPanel.Background(PopupControlCardBrush());
    // Playback control card uses the same subtle outline as the compact island
    // and the expanded outer surface.
    g_popupXamlPanel.BorderThickness({1, 1, 1, 1});
    g_popupXamlPanel.BorderBrush(PopupSurfaceStrokeBrush());
    ConfigurePopupTextEdgeFade(g_popupXamlTitleLeftFade, true, 1.0);
    ConfigurePopupTextEdgeFade(g_popupXamlTitleRightFade, false, 1.0);
    ConfigurePopupTextEdgeFade(g_popupXamlArtistLeftFade, true, 1.0);
    ConfigurePopupTextEdgeFade(g_popupXamlArtistRightFade, false, 1.0);
    ConfigurePopupTextEdgeFade(g_popupXamlOutgoingTitleLeftFade, true, 1.0);
    ConfigurePopupTextEdgeFade(g_popupXamlOutgoingTitleRightFade, false, 1.0);
    ConfigurePopupTextEdgeFade(g_popupXamlOutgoingArtistLeftFade, true, 1.0);
    ConfigurePopupTextEdgeFade(g_popupXamlOutgoingArtistRightFade, false, 1.0);

    ApplyPopupTextForegrounds(false, true);

    auto accent = PopupAccentColor();
    auto accentBrush = Brush(accent);
    auto vividGlow = PopupProgressVividColor();
    auto softCore = PopupProgressBrightSoftColor();
    if (g_popupXamlProgress) {
        g_popupXamlProgress.Foreground(accentBrush);
        g_popupXamlProgress.Background(Brush(dark ? Color(0x00, 0xFF, 0xFF, 0xFF)
                                             : Color(0x00, 0x00, 0x00, 0x00)));
    }
    if (g_popupXamlProgressTrack) {
        g_popupXamlProgressTrack.Background(Brush(dark ? Color(0x34, 0xFF, 0xFF, 0xFF)
                                                  : Color(0x24, 0x00, 0x00, 0x00)));
    }
    if (g_popupXamlProgressFill) {
        g_popupXamlProgressFill.Background(MakePopupProgressGradientBrush());
    }
    for (size_t i = 0; i < g_popupXamlProgressGlowLayers.size(); ++i) {
        static constexpr BYTE kAlpha[] = {0x14, 0x20, 0x32, 0x4A, 0x68, 0x86};
        BYTE a = kAlpha[std::min(i, (sizeof(kAlpha) / sizeof(kAlpha[0])) - 1)];
        g_popupXamlProgressGlowLayers[i].Background(Brush(Color(a, vividGlow.R, vividGlow.G, vividGlow.B)));
        g_popupXamlProgressGlowLayers[i].BorderThickness({0, 0, 0, 0});
    }
    for (size_t i = 0; i < g_popupXamlProgressCoreBlurLayers.size(); ++i) {
        static constexpr BYTE kAlpha[] = {0x08, 0x0E, 0x16, 0x24, 0x38, 0x58};
        BYTE a = kAlpha[std::min(i, (sizeof(kAlpha) / sizeof(kAlpha[0])) - 1)];
        g_popupXamlProgressCoreBlurLayers[i].Background(Brush(Color(a, softCore.R, softCore.G, softCore.B)));
        g_popupXamlProgressCoreBlurLayers[i].BorderThickness({0, 0, 0, 0});
    }
    if (g_popupXamlProgressGlowCore) {
        g_popupXamlProgressGlowCore.Background(Brush(Color(0xFF, softCore.R, softCore.G, softCore.B)));
        g_popupXamlProgressGlowCore.BorderThickness({0, 0, 0, 0});
    }
    if (g_popupXamlControls) {
        for (auto const& child : g_popupXamlControls.Children()) {
            if (auto buttonSurface = child.try_as<Border>()) {
                buttonSurface.Opacity(1.0);
                ApplyPopupButtonVisual(buttonSurface, false);
            }
        }
    }
    ApplyPopupDynamicAccentVisuals();
}

void ApplyPopupDynamicAccentVisuals() {
    auto accent = PopupAccentColor();
    auto accentBrush = Brush(accent);
    bool dark = IsDarkModeApprox();
    if (g_popupXamlArtFrame) {
        g_popupXamlArtFrame.BorderBrush(PopupAlbumArtStrokeBrush());
    }
    if (g_popupXamlProgress) {
        g_popupXamlProgress.Foreground(accentBrush);
        g_popupXamlProgress.Background(Brush(dark ? Color(0x30, 0xFF, 0xFF, 0xFF)
                                             : Color(0x22, 0x00, 0x00, 0x00)));
    }
    if (g_popupXamlProgressFill) {
        g_popupXamlProgressFill.Background(MakePopupProgressGradientBrush());
    }
    auto vividGlow = PopupProgressVividColor();
    auto softCore = PopupProgressBrightSoftColor();
    for (size_t i = 0; i < g_popupXamlProgressGlowLayers.size(); ++i) {
        static constexpr BYTE kAlpha[] = {0x14, 0x20, 0x32, 0x4A, 0x68, 0x86};
        BYTE a = kAlpha[std::min(i, (sizeof(kAlpha) / sizeof(kAlpha[0])) - 1)];
        g_popupXamlProgressGlowLayers[i].Background(Brush(Color(a, vividGlow.R, vividGlow.G, vividGlow.B)));
        g_popupXamlProgressGlowLayers[i].BorderThickness({0, 0, 0, 0});
    }
    for (size_t i = 0; i < g_popupXamlProgressCoreBlurLayers.size(); ++i) {
        static constexpr BYTE kAlpha[] = {0x08, 0x0E, 0x16, 0x24, 0x38, 0x58};
        BYTE a = kAlpha[std::min(i, (sizeof(kAlpha) / sizeof(kAlpha[0])) - 1)];
        g_popupXamlProgressCoreBlurLayers[i].Background(Brush(Color(a, softCore.R, softCore.G, softCore.B)));
        g_popupXamlProgressCoreBlurLayers[i].BorderThickness({0, 0, 0, 0});
    }
    if (g_popupXamlProgressGlowCore) {
        g_popupXamlProgressGlowCore.Background(Brush(Color(0xFF, softCore.R, softCore.G, softCore.B)));
        g_popupXamlProgressGlowCore.BorderThickness({0, 0, 0, 0});
    }
    if (g_popupXamlControls) {
        for (auto const& child : g_popupXamlControls.Children()) {
            if (auto buttonSurface = child.try_as<Border>()) {
                ApplyPopupButtonVisual(buttonSurface, false);
            }
        }
    }
}

void ApplyPopupMediaTransitionVisuals() {
    // Use a higher-order easing and a longer transition window so artwork,
    // album-wash, accent and text changes feel less abrupt during track switches.
    double fade = g_popupMediaTransitionActive
                      ? SmootherStep(Clamp(g_popupMediaTransitionProgress, 0.0, 1.0))
                      : 1.0;

    // Only fade artwork and album-wash layers when the artwork source really
    // changed. Text-only or accent-only transitions should not dim the cover;
    // otherwise a brief empty thumbnail state during track switches exposes the
    // placeholder/background as a visible flash.
    bool coverTransitionActive = g_popupCoverTransitionActive && g_popupMediaTransitionActive;
    double coverFade = coverTransitionActive ? fade : 1.0;
    double backdropWashProgress = g_popupClosing
                                      ? SmootherStep(Clamp((PopupProgress() - 0.001) / 0.065, 0.0, 1.0))
                                      : 1.0;
    if (g_popupXamlArt) {
        g_popupXamlArt.Opacity(coverFade);
    }
    if (g_popupXamlArtFade) {
        g_popupXamlArtFade.Opacity(coverTransitionActive ? (1.0 - fade) : 0.0);
    }
    bool backdropCoverEnabled = PopupBackdropCoverEffectEnabled();
    if (g_popupXamlBackdropCover) {
        g_popupXamlBackdropCover.Opacity(backdropCoverEnabled
                                             ? coverFade * backdropWashProgress
                                             : 0.0);
    }
    if (g_popupXamlBackdropCoverFade) {
        g_popupXamlBackdropCoverFade.Opacity(backdropCoverEnabled && coverTransitionActive
                                            ? (1.0 - fade) * backdropWashProgress
                                            : 0.0);
    }
    if (g_popupXamlPanelCover) {
        g_popupXamlPanelCover.Opacity(PopupPanelCoverEffectEnabled() ? coverFade : 0.0);
    }
    if (g_popupXamlPanelCoverFade) {
        g_popupXamlPanelCoverFade.Opacity(PopupPanelCoverEffectEnabled() && coverTransitionActive
                                              ? (1.0 - fade)
                                              : 0.0);
    }

    // Expanded-view text uses the same reversed direction as the compact island:
    // old title/artist slide left and fade out, while new title/artist slide in
    // from the right. The artist starts slightly later than the title.
    constexpr double kPopupTextSlideOffset = 28.0;
    if (g_popupTextTransitionActive) {
        double p = Clamp(g_popupMediaTransitionProgress, 0.0, 1.0);
        double titleIn = PopupTextMotionEase(Clamp((p - 0.02) / 0.92, 0.0, 1.0));
        double artistIn = PopupTextMotionEase(Clamp((p - 0.28) / 0.70, 0.0, 1.0));
        double titleOut = PopupTextMotionEase(Clamp((p + 0.02) / 0.84, 0.0, 1.0));
        double artistOut = PopupTextMotionEase(Clamp((p - 0.12) / 0.80, 0.0, 1.0));

        if (g_popupXamlTitleTranslate) {
            g_popupXamlTitleTranslate.X(kPopupTextSlideOffset * g_popupTextTransitionDirection * (1.0 - titleIn));
        }
        if (g_popupXamlArtistTranslate) {
            g_popupXamlArtistTranslate.X(kPopupTextSlideOffset * g_popupTextTransitionDirection * (1.0 - artistIn));
        }
        if (g_popupXamlTitle) {
            g_popupXamlTitle.Opacity(g_popupTextBaseOpacity * titleIn);
        }
        if (g_popupXamlArtist) {
            g_popupXamlArtist.Opacity(g_popupTextBaseOpacity * artistIn);
        }

        if (g_popupXamlOutgoingTitleTranslate) {
            g_popupXamlOutgoingTitleTranslate.X(-kPopupTextSlideOffset * g_popupTextTransitionDirection * titleOut);
        }
        if (g_popupXamlOutgoingArtistTranslate) {
            g_popupXamlOutgoingArtistTranslate.X(-kPopupTextSlideOffset * g_popupTextTransitionDirection * artistOut);
        }
        if (g_popupXamlOutgoingTitle) {
            g_popupXamlOutgoingTitle.Opacity(g_popupTextBaseOpacity * (1.0 - titleOut));
        }
        if (g_popupXamlOutgoingArtist) {
            g_popupXamlOutgoingArtist.Opacity(g_popupTextBaseOpacity * (1.0 - artistOut));
        }
        double popupEdgeFadeIn = PopupTextMotionEase(Clamp((p - 0.08) / 0.20, 0.0, 1.0));
        double popupEdgeFadeOut = 1.0 - PopupTextMotionEase(Clamp((p - 0.72) / 0.22, 0.0, 1.0));
        double popupEdgeFade = popupEdgeFadeIn * popupEdgeFadeOut;
        ApplyPopupTextForegroundFade(popupEdgeFade < 0.01 ? 0.0 : popupEdgeFade);
        SetPopupTextEdgeFadeOpacity(0.0);
    } else {
        if (g_popupXamlTitleTranslate) g_popupXamlTitleTranslate.X(0.0);
        if (g_popupXamlArtistTranslate) g_popupXamlArtistTranslate.X(0.0);
        if (g_popupXamlOutgoingTitleTranslate) g_popupXamlOutgoingTitleTranslate.X(0.0);
        if (g_popupXamlOutgoingArtistTranslate) g_popupXamlOutgoingArtistTranslate.X(0.0);
        if (g_popupXamlOutgoingTitle) g_popupXamlOutgoingTitle.Opacity(0.0);
        if (g_popupXamlOutgoingArtist) g_popupXamlOutgoingArtist.Opacity(0.0);
        ApplyPopupTextForegrounds(false);
        SetPopupTextEdgeFadeOpacity(0.0);
    }
}

void TickPopupMediaTransition(double dt) {
    if (!g_popupMediaTransitionActive) {
        ApplyPopupMediaTransitionVisuals();
        return;
    }

    g_popupMediaTransitionProgress = Clamp(g_popupMediaTransitionProgress + dt * 2.6, 0.0, 1.0);
    double eased = SmootherStep(g_popupMediaTransitionProgress);
    g_popupDisplayedAccentColor = LerpColor(g_popupAccentTransitionFrom,
                                            g_popupAccentTransitionTo,
                                            eased);
    g_popupDisplayedAccentColorValid = true;
    ApplyPopupDynamicAccentVisuals();
    ApplyPopupMediaTransitionVisuals();

    if (g_popupMediaTransitionProgress >= 1.0) {
        g_popupMediaTransitionActive = false;
        g_popupCoverTransitionActive = false;
        g_popupDisplayedAccentColor = g_popupAccentTransitionTo;
        g_popupDisplayedAccentColorValid = true;
        if (g_popupXamlArtFade) {
            g_popupXamlArtFade.Source(nullptr);
            g_popupXamlArtFade.Opacity(0.0);
        }
        if (g_popupXamlBackdropCoverFade) {
            g_popupXamlBackdropCoverFade.Source(nullptr);
            g_popupXamlBackdropCoverFade.Opacity(0.0);
        }
        if (g_popupXamlPanelCoverFade) {
            g_popupXamlPanelCoverFade.Source(nullptr);
            g_popupXamlPanelCoverFade.Opacity(0.0);
        }
        g_popupTextTransitionActive = false;
        if (g_popupXamlOutgoingTitle) {
            g_popupXamlOutgoingTitle.Opacity(0.0);
        }
        if (g_popupXamlOutgoingArtist) {
            g_popupXamlOutgoingArtist.Opacity(0.0);
        }
        if (g_popupXamlTitleTranslate) g_popupXamlTitleTranslate.X(0.0);
        if (g_popupXamlArtistTranslate) g_popupXamlArtistTranslate.X(0.0);
        if (g_popupXamlOutgoingTitleTranslate) g_popupXamlOutgoingTitleTranslate.X(0.0);
        if (g_popupXamlOutgoingArtistTranslate) g_popupXamlOutgoingArtistTranslate.X(0.0);
        ApplyPopupDynamicAccentVisuals();
        ApplyPopupMediaTransitionVisuals();
    }
}

void InitializePopupCompositionShadow() {
    // UWP XAML Border in Windhawk does not expose GetAlphaMask(), so an
    // unmasked Composition DropShadow would cast a rectangular shadow for the
    // transparent host bounds. Use XAML ThemeShadow on the rounded surface
    // instead. Main.PopupShadowDepth controls ThemeShadow depth/strength;
    // set it to 0 to disable the native shadow entirely.
    g_popupXamlShadowVisual = nullptr;
    g_popupXamlDropShadow = nullptr;
    if (!g_popupXamlBackdrop) {
        return;
    }

    try {
        int depth = static_cast<int>(std::lround(
            PopupShadowDepth() * (0.25 + 1.75 * PopupShadowOpacityPercent() / 100.0)));
        if (PopupShadowDepth() <= 0 || PopupShadowOpacityPercent() <= 0 || depth <= 0) {
            g_popupXamlBackdrop.Shadow(mediax::Shadow{nullptr});
            g_popupXamlBackdrop.Translation(
                winrt::Windows::Foundation::Numerics::float3{0.0f, 0.0f, 0.0f});
            return;
        }

        mediax::ThemeShadow nativeShadow;
        try {
            if (g_popupXamlRoot) {
                nativeShadow.Receivers().Append(g_popupXamlRoot);
            }
        } catch (...) {
        }
        g_popupXamlBackdrop.Shadow(nativeShadow);
        g_popupXamlBackdrop.Translation(
            winrt::Windows::Foundation::Numerics::float3{0.0f, 0.0f, 0.0f});
    } catch (...) {
    }
}

void ResetPopupXamlElementState() {
    UnsubclassPopupXamlChildWindow();
    g_popupXamlSource = nullptr;
    g_popupXamlChild = nullptr;
    g_popupXamlChildSubclassed = false;
    g_popupXamlChildCaptureExcluded = false;
    g_popupXamlRoot = nullptr;
    g_popupXamlCanvas = nullptr;
    g_popupXamlShadow = nullptr;
    g_popupXamlShadowVisual = nullptr;
    g_popupXamlDropShadow = nullptr;
    g_popupXamlBackdrop = nullptr;
    g_popupXamlBackdropScale = nullptr;
    g_popupXamlBackdropTranslate = nullptr;
    g_popupXamlBackdropCoverFade = nullptr;
    g_popupXamlBackdropCover = nullptr;
    g_popupXamlBackdropTint = nullptr;
    g_popupXamlBackdropSurfaceHighlight = nullptr;
    g_popupXamlBackdropRimHighlight = nullptr;
    g_popupXamlPanelCoverFrame = nullptr;
    g_popupXamlPanelCoverScale = nullptr;
    g_popupXamlPanelCoverTranslate = nullptr;
    g_popupXamlPanelCoverFade = nullptr;
    g_popupXamlPanelCover = nullptr;
    g_popupXamlPanel = nullptr;
    g_popupXamlPanelScale = nullptr;
    g_popupXamlPanelTranslate = nullptr;
    g_popupXamlArtFrame = nullptr;
    g_popupXamlArtScale = nullptr;
    g_popupXamlArtTranslate = nullptr;
    g_popupXamlArtFade = nullptr;
    g_popupXamlArt = nullptr;
    g_popupXamlTitleHost = nullptr;
    g_popupXamlArtistHost = nullptr;
    g_popupXamlOutgoingTitleHost = nullptr;
    g_popupXamlOutgoingArtistHost = nullptr;
    g_popupXamlTitleLeftFade = nullptr;
    g_popupXamlTitleRightFade = nullptr;
    g_popupXamlArtistLeftFade = nullptr;
    g_popupXamlArtistRightFade = nullptr;
    g_popupXamlOutgoingTitleLeftFade = nullptr;
    g_popupXamlOutgoingTitleRightFade = nullptr;
    g_popupXamlOutgoingArtistLeftFade = nullptr;
    g_popupXamlOutgoingArtistRightFade = nullptr;
    g_popupTextForegroundValid = false;
    g_popupTextForegroundEdgeFade = false;
    g_popupTextForegroundDark = false;
    g_popupTextForegroundEdgeFadeAmount = -1.0;

    g_popupXamlTitle = nullptr;
    g_popupXamlArtist = nullptr;
    g_popupXamlOutgoingTitle = nullptr;
    g_popupXamlOutgoingArtist = nullptr;
    g_popupXamlTitleTranslate = nullptr;
    g_popupXamlArtistTranslate = nullptr;
    g_popupXamlOutgoingTitleTranslate = nullptr;
    g_popupXamlOutgoingArtistTranslate = nullptr;
    g_popupTextBaseOpacity = 1.0;
    g_popupTextTransitionActive = false;
    g_popupXamlElapsed = nullptr;
    g_popupXamlDuration = nullptr;
    g_popupXamlProgress = nullptr;
    g_popupXamlProgressScale = nullptr;
    g_popupXamlProgressTrack = nullptr;
    g_popupXamlProgressFill = nullptr;
    g_popupXamlProgressHitTarget = nullptr;
    g_popupXamlProgressGlowMask = nullptr;
    g_popupXamlProgressGlowClip = nullptr;
    g_popupXamlProgressGlowLayers.clear();
    g_popupXamlProgressCoreBlurLayers.clear();
    g_popupXamlProgressGlowCore = nullptr;
    g_popupXamlControls = nullptr;
    g_popupXamlThumbnailHash = UINT64_MAX;
    g_popupXamlBackdropCoverEnabled = false;
    g_popupAccentThumbnailHash = UINT64_MAX;
    g_popupAccentColorValid = false;
    g_popupAccentColor = DefaultPopupAccentColor();
    g_popupDisplayedAccentColorValid = false;
    g_popupDisplayedAccentColor = DefaultPopupAccentColor();
    g_popupAccentTransitionFrom = DefaultPopupAccentColor();
    g_popupAccentTransitionTo = DefaultPopupAccentColor();
    g_popupMediaTransitionProgress = 1.0;
    g_popupMediaTransitionActive = false;
    g_popupCoverTransitionActive = false;
    g_popupXamlThemeValid = false;
    g_popupXamlThemeMaterial.clear();
    g_popupXamlThemeButtonStyle.clear();
    g_popupXamlThemeShadowDepth = -1;
    g_popupXamlThemeShadowOpacity = -1;
}

bool InitializePopupXamlHost(HWND hwnd) {
    try {
        g_popupXamlSource = hosting::DesktopWindowXamlSource();
        auto native = g_popupXamlSource.as<IDesktopWindowXamlSourceNative>();
        winrt::check_hresult(native->AttachToWindow(hwnd));
        winrt::check_hresult(native->get_WindowHandle(&g_popupXamlChild));
        SubclassPopupXamlChildWindow();

        Grid root;
        root.Background(Brush(Color(0, 0, 0, 0)));

        controls::Canvas canvas;

        // The host HWND stays fully transparent. Do not use a fake rounded black
        // rectangle or CS_DROPSHADOW; both look like a separate shape. A real
        // Composition DropShadow is attached to the rounded XAML surface below.
        Border shadow{nullptr};

        Border backdrop;
        backdrop.CornerRadius({kPopupUnifiedCornerRadius, kPopupUnifiedCornerRadius,
                               kPopupUnifiedCornerRadius, kPopupUnifiedCornerRadius});
        backdrop.BorderThickness({0, 0, 0, 0});
        backdrop.Opacity(kPopupBackdropOpacity);
        Grid backdropCoverHost;
        backdropCoverHost.Background(Brush(Color(0x00, 0x00, 0x00, 0x00)));
        Image backdropCoverFade;
        backdropCoverFade.Stretch(mediax::Stretch::UniformToFill);
        backdropCoverFade.Opacity(0.0);
        Image backdropCover;
        backdropCover.Stretch(mediax::Stretch::UniformToFill);
        backdropCover.Opacity(1.0);
        backdropCoverHost.Children().Append(backdropCoverFade);
        backdropCoverHost.Children().Append(backdropCover);
        Border backdropTint;
        backdropTint.Background(PopupBackdropCardTintBrush());
        backdropTint.BorderThickness({0, 0, 0, 0});
        backdropTint.CornerRadius({kPopupUnifiedCornerRadius,
                                   kPopupUnifiedCornerRadius,
                                   kPopupUnifiedCornerRadius,
                                   kPopupUnifiedCornerRadius});
        backdropTint.IsHitTestVisible(false);
        backdropCoverHost.Children().Append(backdropTint);
        Border backdropSurfaceHighlight;
        backdropSurfaceHighlight.Background(LiquidGlassSurfaceHighlightBrush());
        backdropSurfaceHighlight.BorderThickness({0, 0, 0, 0});
        backdropSurfaceHighlight.CornerRadius({kPopupUnifiedCornerRadius,
                                               kPopupUnifiedCornerRadius,
                                               kPopupUnifiedCornerRadius,
                                               kPopupUnifiedCornerRadius});
        backdropSurfaceHighlight.IsHitTestVisible(false);
        backdropSurfaceHighlight.Opacity(0.0);
        backdropCoverHost.Children().Append(backdropSurfaceHighlight);
        Border backdropRimHighlight;
        backdropRimHighlight.Background(Brush(Color(0x00, 0x00, 0x00, 0x00)));
        backdropRimHighlight.BorderBrush(LiquidGlassRimHighlightBrush());
        backdropRimHighlight.BorderThickness({1.0, 1.0, 1.0, 1.0});
        backdropRimHighlight.CornerRadius({kPopupUnifiedCornerRadius,
                                           kPopupUnifiedCornerRadius,
                                           kPopupUnifiedCornerRadius,
                                           kPopupUnifiedCornerRadius});
        backdropRimHighlight.IsHitTestVisible(false);
        backdropRimHighlight.Opacity(0.0);
        backdropCoverHost.Children().Append(backdropRimHighlight);
        backdrop.Child(backdropCoverHost);
        AttachGpuFriendlyTransform(backdrop, g_popupXamlBackdropScale, g_popupXamlBackdropTranslate);
        canvas.Children().Append(backdrop);

        Border panelCoverFrame;
        panelCoverFrame.CornerRadius({kPopupUnifiedCornerRadius, kPopupUnifiedCornerRadius,
                                      kPopupUnifiedCornerRadius, kPopupUnifiedCornerRadius});
        panelCoverFrame.BorderThickness({0, 0, 0, 0});
        panelCoverFrame.Opacity(0.0);
        AttachGpuFriendlyTransform(panelCoverFrame, g_popupXamlPanelCoverScale, g_popupXamlPanelCoverTranslate);
        Grid panelCoverHost;
        Image panelCoverFade;
        panelCoverFade.Stretch(mediax::Stretch::UniformToFill);
        panelCoverFade.Opacity(0.0);
        Image panelCover;
        panelCover.Stretch(mediax::Stretch::UniformToFill);
        panelCover.Opacity(1.0);
        panelCoverHost.Children().Append(panelCoverFade);
        panelCoverHost.Children().Append(panelCover);
        panelCoverFrame.Child(panelCoverHost);
        canvas.Children().Append(panelCoverFrame);

        Border panel;
        panel.CornerRadius({kPopupUnifiedCornerRadius, kPopupUnifiedCornerRadius,
                            kPopupUnifiedCornerRadius, kPopupUnifiedCornerRadius});
        panel.BorderThickness({1, 1, 1, 1});
        panel.BorderBrush(PopupSurfaceStrokeBrush());
        panel.Opacity(0.0);
        AttachGpuFriendlyTransform(panel, g_popupXamlPanelScale, g_popupXamlPanelTranslate);
        canvas.Children().Append(panel);

        Border artFrame;
        artFrame.CornerRadius({kPopupUnifiedCornerRadius, kPopupUnifiedCornerRadius,
                               kPopupUnifiedCornerRadius, kPopupUnifiedCornerRadius});
        artFrame.Background(Brush(Color(0x00, 0x00, 0x00, 0x00)));
        artFrame.BorderThickness(IsLiquidGlassMaterial()
                                     ? Thickness{1.35, 1.35, 1.35, 1.35}
                                     : Thickness{1.2, 1.2, 1.2, 1.2});
        artFrame.BorderBrush(PopupAlbumArtStrokeBrush());
        AttachGpuFriendlyTransform(artFrame, g_popupXamlArtScale, g_popupXamlArtTranslate);
        Grid artHost;
        Image artFade;
        artFade.Stretch(mediax::Stretch::UniformToFill);
        artFade.IsHitTestVisible(false);
        artFade.Opacity(0.0);
        Image art;
        art.Stretch(mediax::Stretch::UniformToFill);
        art.Opacity(1.0);
        artHost.Children().Append(artFade);
        artHost.Children().Append(art);
        artFrame.Child(artHost);
        canvas.Children().Append(artFrame);

        auto makePopupTextFade = [](bool leftEdge) {
            Border fade;
            fade.HorizontalAlignment(leftEdge ? HorizontalAlignment::Left
                                              : HorizontalAlignment::Right);
            fade.VerticalAlignment(VerticalAlignment::Stretch);
            fade.IsHitTestVisible(false);
            fade.Opacity(0.0);
            controls::Canvas::SetZIndex(fade, 10);
            return fade;
        };

        Grid outgoingTitleHost;
        outgoingTitleHost.IsHitTestVisible(false);
        TextBlock outgoingTitle;
        outgoingTitle.FontFamily(mediax::FontFamily(L"Segoe UI Variable Text"));
        outgoingTitle.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        outgoingTitle.TextTrimming(xaml::TextTrimming::CharacterEllipsis);
        outgoingTitle.VerticalAlignment(VerticalAlignment::Center);
        outgoingTitle.Opacity(0.0);
        TranslateTransform outgoingTitleTranslate;
        outgoingTitle.RenderTransform(outgoingTitleTranslate);
        outgoingTitleHost.Children().Append(outgoingTitle);
        Border outgoingTitleLeftFade = makePopupTextFade(true);
        Border outgoingTitleRightFade = makePopupTextFade(false);
        outgoingTitleHost.Children().Append(outgoingTitleLeftFade);
        outgoingTitleHost.Children().Append(outgoingTitleRightFade);
        canvas.Children().Append(outgoingTitleHost);

        Grid outgoingArtistHost;
        outgoingArtistHost.IsHitTestVisible(false);
        TextBlock outgoingArtist;
        outgoingArtist.FontFamily(mediax::FontFamily(L"Segoe UI Variable Text"));
        outgoingArtist.TextTrimming(xaml::TextTrimming::CharacterEllipsis);
        outgoingArtist.VerticalAlignment(VerticalAlignment::Center);
        outgoingArtist.Opacity(0.0);
        TranslateTransform outgoingArtistTranslate;
        outgoingArtist.RenderTransform(outgoingArtistTranslate);
        outgoingArtistHost.Children().Append(outgoingArtist);
        Border outgoingArtistLeftFade = makePopupTextFade(true);
        Border outgoingArtistRightFade = makePopupTextFade(false);
        outgoingArtistHost.Children().Append(outgoingArtistLeftFade);
        outgoingArtistHost.Children().Append(outgoingArtistRightFade);
        canvas.Children().Append(outgoingArtistHost);

        Grid titleHost;
        titleHost.IsHitTestVisible(false);
        TextBlock title;
        title.FontFamily(mediax::FontFamily(L"Segoe UI Variable Text"));
        title.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        title.TextTrimming(xaml::TextTrimming::CharacterEllipsis);
        title.VerticalAlignment(VerticalAlignment::Center);
        TranslateTransform titleTranslate;
        title.RenderTransform(titleTranslate);
        titleHost.Children().Append(title);
        Border titleLeftFade = makePopupTextFade(true);
        Border titleRightFade = makePopupTextFade(false);
        titleHost.Children().Append(titleLeftFade);
        titleHost.Children().Append(titleRightFade);
        canvas.Children().Append(titleHost);

        Grid artistHost;
        artistHost.IsHitTestVisible(false);
        TextBlock artist;
        artist.FontFamily(mediax::FontFamily(L"Segoe UI Variable Text"));
        artist.TextTrimming(xaml::TextTrimming::CharacterEllipsis);
        artist.VerticalAlignment(VerticalAlignment::Center);
        TranslateTransform artistTranslate;
        artist.RenderTransform(artistTranslate);
        artistHost.Children().Append(artist);
        Border artistLeftFade = makePopupTextFade(true);
        Border artistRightFade = makePopupTextFade(false);
        artistHost.Children().Append(artistLeftFade);
        artistHost.Children().Append(artistRightFade);
        canvas.Children().Append(artistHost);

        TextBlock elapsed;
        elapsed.FontFamily(mediax::FontFamily(L"Segoe UI Variable Text"));
        elapsed.FontSize(11);
        elapsed.Text(L"0:00");
        elapsed.VerticalAlignment(VerticalAlignment::Center);
        canvas.Children().Append(elapsed);

        TextBlock duration;
        duration.FontFamily(mediax::FontFamily(L"Segoe UI Variable Text"));
        duration.FontSize(11);
        duration.Text(L"0:00");
        duration.TextAlignment(xaml::TextAlignment::Right);
        duration.VerticalAlignment(VerticalAlignment::Center);
        canvas.Children().Append(duration);

        Border progressTrack;
        progressTrack.Height(6);
        progressTrack.CornerRadius({kPopupG2ProgressCornerRadius,
                                    kPopupG2ProgressCornerRadius,
                                    kPopupG2ProgressCornerRadius,
                                    kPopupG2ProgressCornerRadius});
        progressTrack.IsHitTestVisible(false);
        canvas.Children().Append(progressTrack);

        Border progressFill;
        progressFill.Height(6);
        progressFill.CornerRadius({kPopupG2ProgressCornerRadius,
                                   kPopupG2ProgressCornerRadius,
                                   kPopupG2ProgressCornerRadius,
                                   kPopupG2ProgressCornerRadius});
        progressFill.IsHitTestVisible(false);
        canvas.Children().Append(progressFill);

        ProgressBar progress;
        progress.Height(6);
        progress.Minimum(0);
        progress.Maximum(1000);
        progress.RenderTransformOrigin({0.5, 0.5});
        ScaleTransform progressScale;
        progressScale.ScaleX(0.82);
        progressScale.ScaleY(0.82);
        progress.RenderTransform(progressScale);
        progress.IsHitTestVisible(false);
        progress.Opacity(0.0);
        canvas.Children().Append(progress);

        controls::Canvas progressGlowMask;
        progressGlowMask.IsHitTestVisible(false);
        progressGlowMask.Opacity(0.0);
        mediax::RectangleGeometry progressGlowClip;
        progressGlowMask.Clip(progressGlowClip);
        std::vector<Border> progressGlowLayers;
        for (int i = 0; i < 6; ++i) {
            Border layer;
            layer.IsHitTestVisible(false);
            layer.Opacity(0.0);
            progressGlowMask.Children().Append(layer);
            progressGlowLayers.push_back(layer);
        }
        canvas.Children().Append(progressGlowMask);

        std::vector<Border> progressCoreBlurLayers;
        for (int i = 0; i < 6; ++i) {
            Border layer;
            layer.IsHitTestVisible(false);
            layer.Opacity(0.0);
            canvas.Children().Append(layer);
            progressCoreBlurLayers.push_back(layer);
        }

        Border progressGlowCore;
        progressGlowCore.Width(6);
        progressGlowCore.Height(6);
        progressGlowCore.CornerRadius({kPopupG2ProgressCornerRadius,
                                       kPopupG2ProgressCornerRadius,
                                       kPopupG2ProgressCornerRadius,
                                       kPopupG2ProgressCornerRadius});
        progressGlowCore.IsHitTestVisible(false);
        progressGlowCore.Opacity(0.0);
        canvas.Children().Append(progressGlowCore);

        Border progressHitTarget;
        progressHitTarget.Height(22);
        progressHitTarget.Background(Brush(Color(0x01, 0x00, 0x00, 0x00)));
        canvas.Children().Append(progressHitTarget);

        controls::Canvas controlsPanel;
        controlsPanel.Width(PopupControlsWidth());
        controlsPanel.Height(PopupControlsHeight());

        Border prevButton = MakePopupXamlButton(
            L"Popup_Prev", [] {
                RunMediaCommand([](gsm::GlobalSystemMediaTransportControlsSession const& s) {
                    NoteMediaNavigationDirection(-1);
                    GetAsyncResultWithTimeout(s.TrySkipPreviousAsync(),
                                           kMediaCommandAsyncTimeout,
                                           L"TrySkipPreviousAsync");
                });
            });
        Border playButton = MakePopupXamlButton(
            L"Popup_Play", [] {
                RunMediaCommand([](gsm::GlobalSystemMediaTransportControlsSession const& s) {
                    GetAsyncResultWithTimeout(s.TryTogglePlayPauseAsync(),
                                           kMediaCommandAsyncTimeout,
                                           L"TryTogglePlayPauseAsync");
                });
            }, true);
        Border nextButton = MakePopupXamlButton(
            L"Popup_Next", [] {
                RunMediaCommand([](gsm::GlobalSystemMediaTransportControlsSession const& s) {
                    NoteMediaNavigationDirection(1);
                    GetAsyncResultWithTimeout(s.TrySkipNextAsync(),
                                           kMediaCommandAsyncTimeout,
                                           L"TrySkipNextAsync");
                });
            });

        controls::Canvas::SetLeft(prevButton, PopupButtonLeft(0));
        controls::Canvas::SetTop(prevButton, PopupButtonTop());
        controls::Canvas::SetLeft(playButton, PopupButtonLeft(1));
        controls::Canvas::SetTop(playButton, PopupButtonTop());
        controls::Canvas::SetLeft(nextButton, PopupButtonLeft(2));
        controls::Canvas::SetTop(nextButton, PopupButtonTop());

        controlsPanel.Children().Append(prevButton);
        controlsPanel.Children().Append(playButton);
        controlsPanel.Children().Append(nextButton);
        canvas.Children().Append(controlsPanel);

        progressHitTarget.PointerPressed([](auto const& sender,
                                            input::PointerRoutedEventArgs const& e) {
            try {
                if (!g_popupXamlProgressTrack || PopupProgress() < 0.75) {
                    return;
                }

                auto point = e.GetCurrentPoint(g_popupXamlProgressTrack).Position();
                double width = g_popupXamlProgressTrack.ActualWidth();
                double height = g_popupXamlProgressTrack.ActualHeight();
                if (width <= 0.0) {
                    return;
                }

                constexpr double hitPadding = 8.0;
                if (point.X >= 0.0 && point.X <= width &&
                    point.Y >= -hitPadding && point.Y <= height + hitPadding) {
                    UpdatePopupSeekPreview(point.X / width);
                    if (auto element = sender.template try_as<UIElement>()) {
                        if (!element.CapturePointer(e.Pointer())) {
                            EndPopupSeek(false);
                            return;
                        }
                    }
                    e.Handled(true);
                }
            } catch (...) {
                try {
                    EndPopupSeek(false);
                } catch (...) {
                }
            }
        });
        progressHitTarget.PointerMoved([](auto const&,
                                          input::PointerRoutedEventArgs const& e) {
            try {
                if (!g_popupSeekDragging || !g_popupXamlProgressTrack) {
                    return;
                }

                double width = g_popupXamlProgressTrack.ActualWidth();
                if (width > 0.0) {
                    auto point = e.GetCurrentPoint(g_popupXamlProgressTrack).Position();
                    UpdatePopupSeekPreview(point.X / width);
                    e.Handled(true);
                }
            } catch (...) {
            }
        });
        progressHitTarget.PointerReleased([](auto const& sender,
                                             input::PointerRoutedEventArgs const& e) {
            if (!g_popupSeekDragging) {
                return;
            }
            try {
                if (g_popupXamlProgressTrack) {
                    double width = g_popupXamlProgressTrack.ActualWidth();
                    if (width > 0.0) {
                        auto point = e.GetCurrentPoint(g_popupXamlProgressTrack).Position();
                        UpdatePopupSeekPreview(point.X / width);
                    }
                }
            } catch (...) {
                // Commit the last valid preview even if the final pointer sample
                // can't be resolved while the popup is being relaid out.
            }
            try {
                EndPopupSeek(true);
            } catch (...) {
            }
            try {
                if (auto element = sender.template try_as<UIElement>()) {
                    element.ReleasePointerCapture(e.Pointer());
                }
                e.Handled(true);
            } catch (...) {
            }
        });
        progressHitTarget.PointerCanceled([](auto const& sender,
                                             input::PointerRoutedEventArgs const& e) {
            try {
                if (!g_popupSeekDragging) {
                    return;
                }
                EndPopupSeek(false);
                if (auto element = sender.template try_as<UIElement>()) {
                    element.ReleasePointerCapture(e.Pointer());
                }
                e.Handled(true);
            } catch (...) {
            }
        });
        progressHitTarget.PointerCaptureLost([](auto const&,
                                                input::PointerRoutedEventArgs const&) {
            try {
                if (g_popupSeekDragging) {
                    EndPopupSeek(false);
                }
            } catch (...) {
            }
        });

        root.Children().Append(canvas);
        g_popupXamlSource.Content(root);

        g_popupXamlRoot = root;
        g_popupXamlCanvas = canvas;
        g_popupXamlShadow = shadow;
        g_popupXamlBackdrop = backdrop;
        g_popupXamlBackdropCoverFade = backdropCoverFade;
        g_popupXamlBackdropCover = backdropCover;
        g_popupXamlBackdropTint = backdropTint;
        g_popupXamlBackdropSurfaceHighlight = backdropSurfaceHighlight;
        g_popupXamlBackdropRimHighlight = backdropRimHighlight;
        g_popupXamlPanelCoverFrame = panelCoverFrame;
        g_popupXamlPanelCoverFade = panelCoverFade;
        g_popupXamlPanelCover = panelCover;
        g_popupXamlPanel = panel;
        g_popupXamlArtFrame = artFrame;
        g_popupXamlArtFade = artFade;
        g_popupXamlArt = art;
        g_popupXamlTitleHost = titleHost;
        g_popupXamlArtistHost = artistHost;
        g_popupXamlOutgoingTitleHost = outgoingTitleHost;
        g_popupXamlOutgoingArtistHost = outgoingArtistHost;
        g_popupXamlTitleLeftFade = titleLeftFade;
        g_popupXamlTitleRightFade = titleRightFade;
        g_popupXamlArtistLeftFade = artistLeftFade;
        g_popupXamlArtistRightFade = artistRightFade;
        g_popupXamlOutgoingTitleLeftFade = outgoingTitleLeftFade;
        g_popupXamlOutgoingTitleRightFade = outgoingTitleRightFade;
        g_popupXamlOutgoingArtistLeftFade = outgoingArtistLeftFade;
        g_popupXamlOutgoingArtistRightFade = outgoingArtistRightFade;
        g_popupXamlTitle = title;
        g_popupXamlArtist = artist;
        g_popupXamlOutgoingTitle = outgoingTitle;
        g_popupXamlOutgoingArtist = outgoingArtist;
        g_popupXamlTitleTranslate = titleTranslate;
        g_popupXamlArtistTranslate = artistTranslate;
        g_popupXamlOutgoingTitleTranslate = outgoingTitleTranslate;
        g_popupXamlOutgoingArtistTranslate = outgoingArtistTranslate;
        g_popupXamlElapsed = elapsed;
        g_popupXamlDuration = duration;
        g_popupXamlProgress = progress;
        g_popupXamlProgressScale = progressScale;
        g_popupXamlProgressTrack = progressTrack;
        g_popupXamlProgressFill = progressFill;
        g_popupXamlProgressHitTarget = progressHitTarget;
        g_popupXamlProgressGlowMask = progressGlowMask;
        g_popupXamlProgressGlowClip = progressGlowClip;
        g_popupXamlProgressGlowLayers = progressGlowLayers;
        g_popupXamlProgressCoreBlurLayers = progressCoreBlurLayers;
        g_popupXamlProgressGlowCore = progressGlowCore;
        g_popupXamlControls = controlsPanel;
        g_popupXamlThumbnailHash = UINT64_MAX;
        InitializePopupCompositionShadow();
        ApplyPopupXamlTheme(true);
        ShowWindow(g_popupXamlChild, SW_SHOWNA);
        return true;
    } catch (...) {
        if (g_popupXamlSource) {
            try {
                g_popupXamlSource.Content(nullptr);
                g_popupXamlSource.Close();
            } catch (...) {
            }
        }
        ResetPopupXamlElementState();
        return false;
    }
}

double PopupDpiScale(HWND hwnd);
int PopupDipToPx(double value, HWND hwnd);
RECT PopupScreenRectToLocalDip(RECT const& rect,
                               RECT const& popupScreenPx,
                               double scale);

void UpdatePopupXamlVisuals() {
    if (!g_popupXamlRoot || !g_expandedPopup) {
        return;
    }

    RECT popupScreenPx{};
    GetWindowRect(g_expandedPopup, &popupScreenPx);
    int widthPx = popupScreenPx.right - popupScreenPx.left;
    int heightPx = popupScreenPx.bottom - popupScreenPx.top;
    if (widthPx <= 0 || heightPx <= 0) {
        return;
    }

    double dpiScale = PopupDpiScale(g_expandedPopup);
    int width = std::max(1, static_cast<int>(std::lround(widthPx / dpiScale)));
    int height = std::max(1, static_cast<int>(std::lround(heightPx / dpiScale)));
    RECT popupScreen{0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
    RECT sourceRect = PopupScreenRectToLocalDip(g_popupSourceRect, popupScreenPx, dpiScale);
    RECT sourceArtRect = PopupScreenRectToLocalDip(g_popupSourceArtRect, popupScreenPx, dpiScale);
    RECT sourceTitleRect = PopupScreenRectToLocalDip(g_popupSourceTitleRect, popupScreenPx, dpiScale);
    RECT sourceArtistRect = PopupScreenRectToLocalDip(g_popupSourceArtistRect, popupScreenPx, dpiScale);
    RECT sourceProgressRect = PopupScreenRectToLocalDip(g_popupSourceProgressRect, popupScreenPx, dpiScale);
    RECT sourceControlsRect = PopupScreenRectToLocalDip(g_popupSourceControlsRect, popupScreenPx, dpiScale);
    RECT finalRect = PopupScreenRectToLocalDip(g_popupFinalRect, popupScreenPx, dpiScale);

    double progress = PopupProgress();
    double morphProgress = Clamp(progress, 0.0, 1.0);
    double visibilityProgress = PopupClosingShellOpacity(morphProgress);
    double compactRevealOpacity = PopupClosingCompactRevealOpacity(morphProgress);
    double closingTailFade = g_popupClosing
                                 ? SmootherStep(Clamp((morphProgress - 0.001) / 0.065, 0.0, 1.0))
                                 : morphProgress;
    if (g_popupClosing && g_playerGrid) {
        try {
            g_playerGrid.Opacity(compactRevealOpacity);
            g_playerGrid.IsHitTestVisible(false);
        } catch (...) {
        }
    }
    RECT currentRect{0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
    // Always update the animated XAML geometry while the popup is moving.
    // The previous small-delta skip optimization made the album-art morph
    // freeze for one or more frames when the HWND rect rounded to the same
    // integer coordinates, then jump on the next update. That looked like a
    // flash/offset during the cover scaling animation.
    g_lastPopupVisualRect = currentRect;
    g_lastPopupVisualProgress = progress;

    double rootWidth = g_popupXamlRoot.Width();
    double rootHeight = g_popupXamlRoot.Height();
    if (std::isnan(rootWidth) || std::abs(rootWidth - width) > 0.5) {
        g_popupXamlRoot.Width(width);
    }
    if (std::isnan(rootHeight) || std::abs(rootHeight - height) > 0.5) {
        g_popupXamlRoot.Height(height);
    }
    if (g_popupXamlCanvas) {
        double canvasWidth = g_popupXamlCanvas.Width();
        double canvasHeight = g_popupXamlCanvas.Height();
        if (std::isnan(canvasWidth) || std::abs(canvasWidth - width) > 0.5) {
            g_popupXamlCanvas.Width(width);
        }
        if (std::isnan(canvasHeight) || std::abs(canvasHeight - height) > 0.5) {
            g_popupXamlCanvas.Height(height);
        }
    }

    RECT targetArtScreen{};
    RECT targetCard{};
    RECT targetTitle{};
    RECT targetArtist{};
    RECT targetProgress{};
    RECT targetElapsed{};
    RECT targetDuration{};
    RECT targetControls{};
    CalculatePopupFinalLayout(finalRect, targetArtScreen, targetCard,
                              targetTitle, targetArtist,
                              targetProgress, targetElapsed, targetDuration,
                              targetControls);
    RECT targetBackdrop = PopupBackdropRectFromParts(targetArtScreen, targetCard);
    RECT backdropScreen{
        LerpInt(sourceRect.left, targetBackdrop.left, progress),
        LerpInt(sourceRect.top, targetBackdrop.top, progress),
        LerpInt(sourceRect.right, targetBackdrop.right, progress),
        LerpInt(sourceRect.bottom, targetBackdrop.bottom, progress),
    };
    double backdropWidth = std::max(1L, targetBackdrop.right - targetBackdrop.left);
    double backdropHeight = std::max(1L, targetBackdrop.bottom - targetBackdrop.top);
    double sourceShellRadius = SourceScaledRadius(g_layout.cornerRadius,
                                                  g_layout.compactHeight,
                                                  sourceRect);
    if (g_popupXamlShadowVisual) {
        float shadowOpacity = static_cast<float>(visibilityProgress);
        g_popupXamlShadowVisual.Size({static_cast<float>(backdropWidth),
                                      static_cast<float>(backdropHeight)});
        g_popupXamlShadowVisual.Offset({static_cast<float>(targetBackdrop.left - popupScreen.left +
                                                           backdropScreen.left - targetBackdrop.left),
                                        static_cast<float>(targetBackdrop.top - popupScreen.top +
                                                           backdropScreen.top - targetBackdrop.top),
                                        0.0f});
        g_popupXamlShadowVisual.Opacity(shadowOpacity);
    }
    if (g_popupXamlBackdrop) {
        try {
            int shadowDepth = static_cast<int>(std::lround(
                PopupShadowDepth() * (0.25 + 1.75 * PopupShadowOpacityPercent() / 100.0)));
            float shadowZ = PopupShadowDepth() <= 0 || PopupShadowOpacityPercent() <= 0
                                ? 0.0f
                                : static_cast<float>(shadowDepth * PopupShadowMorphOpacity(morphProgress));
            g_popupXamlBackdrop.Translation(
                winrt::Windows::Foundation::Numerics::float3{0.0f, 0.0f, shadowZ});
        } catch (...) {
        }
        double targetShellRadius =
            g_settings.compact
                ? PopupG2CornerRadius(backdropWidth, backdropHeight)
                : kPopupUnifiedCornerRadius;
        double shellVisualRadius = LerpDouble(sourceShellRadius,
                                              targetShellRadius,
                                              morphProgress);
        double shellRadius = Clamp(shellVisualRadius, 1.0,
                                   std::min(backdropScreen.right - backdropScreen.left,
                                            backdropScreen.bottom - backdropScreen.top) * 0.5);
        g_popupXamlBackdrop.CornerRadius({shellRadius, shellRadius, shellRadius, shellRadius});
        double backdropOpacity = kPopupBackdropOpacity * visibilityProgress;
        double washOpacity = PopupBackdropCoverEffectEnabled()
                                 ? (g_popupClosing ? closingTailFade
                                                   : 1.0)
                                 : 0.0;
        g_popupXamlBackdrop.Opacity(backdropOpacity);
        // Optional album color wash. Liquid Glass keeps it visible in light mode;
        // otherwise Main.PopupBackdropCoverEffect controls the behavior.
        bool backdropCoverEnabled = PopupBackdropCoverEffectEnabled();
        if (g_popupXamlBackdropCover) {
            g_popupXamlBackdropCover.Stretch(mediax::Stretch::UniformToFill);
            g_popupXamlBackdropCover.Opacity(backdropCoverEnabled ? washOpacity : 0.0);
        }
        if (g_popupXamlBackdropCoverFade) {
            g_popupXamlBackdropCoverFade.Stretch(mediax::Stretch::UniformToFill);
            g_popupXamlBackdropCoverFade.Opacity(0.0);
        }
        if (g_popupXamlBackdropTint) {
            g_popupXamlBackdropTint.CornerRadius(
                {shellRadius, shellRadius, shellRadius, shellRadius});
        }
        double liquidHighlightOpacity = IsLiquidGlassMaterial() ? backdropOpacity : 0.0;
        double liquidSurfaceHighlightOpacity =
            IsLiquidGlassMaterial()
                ? backdropOpacity * (IsDarkModeApprox() ? 0.45 : 0.78)
                : 0.0;
        if (g_popupXamlBackdropSurfaceHighlight) {
            g_popupXamlBackdropSurfaceHighlight.CornerRadius(
                {shellRadius, shellRadius, shellRadius, shellRadius});
            g_popupXamlBackdropSurfaceHighlight.Opacity(liquidSurfaceHighlightOpacity);
        }
        if (g_popupXamlBackdropRimHighlight) {
            g_popupXamlBackdropRimHighlight.CornerRadius(
                {shellRadius, shellRadius, shellRadius, shellRadius});
            g_popupXamlBackdropRimHighlight.BorderThickness(
                IsLiquidGlassMaterial() ? Thickness{1.0, 1.0, 1.0, 1.0}
                                        : Thickness{0.0, 0.0, 0.0, 0.0});
            g_popupXamlBackdropRimHighlight.Opacity(liquidHighlightOpacity);
        }
        ApplyLayoutRect(g_popupXamlBackdrop,
                        g_popupXamlBackdropScale,
                        g_popupXamlBackdropTranslate,
                        backdropScreen,
                        popupScreen);
    }
    RECT artScreen{
        LerpInt(sourceArtRect.left, targetArtScreen.left, progress),
        LerpInt(sourceArtRect.top, targetArtScreen.top, progress),
        LerpInt(sourceArtRect.right, targetArtScreen.right, progress),
        LerpInt(sourceArtRect.bottom, targetArtScreen.bottom, progress),
    };
    double popupArtTargetSize =
        static_cast<double>(std::max(1L, targetArtScreen.right - targetArtScreen.left));
    double popupArtTargetRadius =
        PopupG2CornerRadius(popupArtTargetSize, popupArtTargetSize);
    double sourceArtRadius = SourceScaledRadius(g_layout.artCornerRadius,
                                                g_layout.artSize,
                                                sourceArtRect);
    double popupArtVisualRadius = LerpDouble(sourceArtRadius,
                                             popupArtTargetRadius,
                                             morphProgress);
    double popupArtRadius = Clamp(popupArtVisualRadius, 1.0,
                                  std::min(artScreen.right - artScreen.left,
                                           artScreen.bottom - artScreen.top) * 0.5);
    g_popupXamlArtFrame.CornerRadius(
        {popupArtRadius, popupArtRadius, popupArtRadius, popupArtRadius});
    g_popupXamlArtFrame.BorderThickness(IsLiquidGlassMaterial()
                                            ? Thickness{1.35, 1.35, 1.35, 1.35}
                                            : Thickness{1.2, 1.2, 1.2, 1.2});
    g_popupXamlArtFrame.BorderBrush(PopupAlbumArtStrokeBrush());
    g_popupXamlArtFrame.Clip(nullptr);
    double artWidth = std::max(1.0, static_cast<double>(artScreen.right - artScreen.left));
    double artHeight = std::max(1.0, static_cast<double>(artScreen.bottom - artScreen.top));
    double sourceArtOverscan = std::min(
        SourceScaledRadius(1.0, g_layout.artSize, sourceArtRect),
        std::min(artWidth, artHeight) * 0.12);
    double artOverscan = LerpDouble(sourceArtOverscan, 0.0, morphProgress);
    if (auto artHost = g_popupXamlArtFrame.Child().try_as<FrameworkElement>()) {
        artHost.Width(artWidth);
        artHost.Height(artHeight);
        ApplyElementClip(artHost, artWidth, artHeight);
    }
    auto alignPopupArtImage = [&](Image const& image) {
        if (!image) {
            return;
        }
        image.Width(artWidth + artOverscan * 2.0);
        image.Height(artHeight + artOverscan * 2.0);
        image.Margin({-artOverscan, -artOverscan,
                      -artOverscan, -artOverscan});
        image.Stretch(mediax::Stretch::UniformToFill);
    };
    alignPopupArtImage(g_popupXamlArtFade);
    alignPopupArtImage(g_popupXamlArt);
    g_popupXamlArtFrame.Opacity(g_popupClosing
                                    ? SmootherStep(Clamp((morphProgress - 0.025) / 0.16, 0.0, 1.0))
                                    : 1.0);
    ApplyLayoutRect(g_popupXamlArtFrame,
                    g_popupXamlArtScale,
                    g_popupXamlArtTranslate,
                    artScreen,
                    popupScreen);

    RECT cardScreen{
        LerpInt(sourceRect.left, targetCard.left, progress),
        LerpInt(sourceRect.top, targetCard.top, progress),
        LerpInt(sourceRect.right, targetCard.right, progress),
        LerpInt(sourceRect.bottom, targetCard.bottom, progress),
    };
    double panelTargetRadius =
        g_settings.compact
            ? PopupG2CornerRadius(targetCard.right - targetCard.left,
                                  targetCard.bottom - targetCard.top)
            : kPopupUnifiedCornerRadius;
    double panelVisualRadius = LerpDouble(sourceShellRadius,
                                          panelTargetRadius,
                                          morphProgress);
    double panelRadius = Clamp(panelVisualRadius, 1.0,
                               std::min(cardScreen.right - cardScreen.left,
                                        cardScreen.bottom - cardScreen.top) * 0.5);
    double expandedLayerOpacity = g_popupClosing ? closingTailFade : morphProgress;
    double cardOpacity = expandedLayerOpacity;
    if (g_popupXamlPanelCoverFrame) {
        g_popupXamlPanelCoverFrame.CornerRadius({panelRadius, panelRadius, panelRadius, panelRadius});
        g_popupXamlPanelCoverFrame.Opacity(
            !g_settings.compact && PopupPanelCoverEffectEnabled()
                ? cardOpacity * PopupPanelCoverOpacityFactor()
                : 0.0);
        ApplyLayoutRect(g_popupXamlPanelCoverFrame,
                        g_popupXamlPanelCoverScale,
                        g_popupXamlPanelCoverTranslate,
                        cardScreen,
                        popupScreen);
    }
    g_popupXamlPanel.CornerRadius({panelRadius, panelRadius, panelRadius, panelRadius});
    g_popupXamlPanel.Opacity(g_settings.compact ? 0.0 : cardOpacity);
    ApplyLayoutRect(g_popupXamlPanel,
                    g_popupXamlPanelScale,
                    g_popupXamlPanelTranslate,
                    cardScreen,
                    popupScreen);

    auto placeText = [&](FrameworkElement const& host,
                         TextBlock const& text,
                         Border const& leftFade,
                         Border const& rightFade,
                         RECT const& source,
                         RECT const& target,
                         double compactSize,
                         double expandedSize) {
        if (!host || !text) {
            return;
        }

        RECT screen{LerpInt(source.left, target.left, progress),
                    LerpInt(source.top, target.top, progress),
                    LerpInt(source.right, target.right, progress),
                    LerpInt(source.bottom, target.bottom, progress)};
        double textWidth = std::max(1L, screen.right - screen.left);
        double textHeight = std::max(1L, screen.bottom - screen.top);
        host.Width(textWidth);
        host.Height(textHeight);
        double clipPadding = PopupTextClipPadding();
        ApplyElementClipWithPadding(host, textWidth, textHeight, clipPadding);
        ConfigurePopupTextEdgeFade(leftFade, true, textHeight);
        ConfigurePopupTextEdgeFade(rightFade, false, textHeight);
        text.Width(textWidth);
        text.Height(textHeight);
        text.TextTrimming(g_popupClosing ? xaml::TextTrimming::None
                                         : xaml::TextTrimming::CharacterEllipsis);
        text.FontSize(LerpDouble(compactSize, expandedSize, progress));
        controls::Canvas::SetLeft(host, screen.left - popupScreen.left);
        controls::Canvas::SetTop(host, screen.top - popupScreen.top);
    };
    placeText(g_popupXamlOutgoingTitleHost, g_popupXamlOutgoingTitle,
              g_popupXamlOutgoingTitleLeftFade, g_popupXamlOutgoingTitleRightFade,
              sourceTitleRect, targetTitle, 12, 16);
    placeText(g_popupXamlOutgoingArtistHost, g_popupXamlOutgoingArtist,
              g_popupXamlOutgoingArtistLeftFade, g_popupXamlOutgoingArtistRightFade,
              sourceArtistRect, targetArtist, 10, 13);
    placeText(g_popupXamlTitleHost, g_popupXamlTitle,
              g_popupXamlTitleLeftFade, g_popupXamlTitleRightFade,
              sourceTitleRect, targetTitle, 12, 16);
    placeText(g_popupXamlArtistHost, g_popupXamlArtist,
              g_popupXamlArtistLeftFade, g_popupXamlArtistRightFade,
              sourceArtistRect, targetArtist, 10, 13);

    double textOpacity = PopupClosingTextOpacity(morphProgress);
    g_popupTextBaseOpacity = textOpacity;
    if (!g_popupTextTransitionActive) {
        if (g_popupXamlTitle) g_popupXamlTitle.Opacity(textOpacity);
        if (g_popupXamlArtist) g_popupXamlArtist.Opacity(textOpacity);
        if (g_popupXamlTitleTranslate) g_popupXamlTitleTranslate.X(0.0);
        if (g_popupXamlArtistTranslate) g_popupXamlArtistTranslate.X(0.0);
        if (g_popupXamlOutgoingTitle) g_popupXamlOutgoingTitle.Opacity(0.0);
        if (g_popupXamlOutgoingArtist) g_popupXamlOutgoingArtist.Opacity(0.0);
        ApplyPopupTextForegrounds(false);
        SetPopupTextEdgeFadeOpacity(0.0);
    }

    // Morph the transport controls from the compact island area to their final
    // positions, instead of starting from the already-expanded control card.
    // The previous version used the card center as the source and delayed the
    // animation to progress 0.28~0.62. In practice that looked like a simple
    // fade-in because the source was already close to the target and the active
    // time window was too short.
    double controlsProgress = PopupControlMorphProgress(morphProgress);
    double controlsOpacity = PopupControlOpacityProgress(morphProgress);
    double controlsScale = 1.0;
    double buttonProgress = controlsProgress;
    bool showControls = controlsOpacity > 0.002;

    auto lerpRect = [&](RECT const& source, RECT const& target, double amount) {
        return RECT{LerpInt(source.left, target.left, amount),
                    LerpInt(source.top, target.top, amount),
                    LerpInt(source.right, target.right, amount),
                    LerpInt(source.bottom, target.bottom, amount)};
    };

    int sourceCenterX = (sourceRect.left + sourceRect.right) / 2;
    int sourceCenterY = (sourceRect.top + sourceRect.bottom) / 2;

    RECT progressSource = sourceProgressRect;
    if (progressSource.right <= progressSource.left ||
        progressSource.bottom <= progressSource.top) {
        progressSource = {sourceCenterX - 10,
                          sourceCenterY - 2,
                          sourceCenterX + 10,
                          sourceCenterY + 2};
    }
    RECT progressScreen = lerpRect(progressSource, targetProgress, controlsProgress);
    double progressRatio = 0.0;
    if (g_popupXamlProgress && g_popupXamlProgress.Maximum() > 0.0) {
        progressRatio = Clamp(g_popupXamlProgress.Value() / g_popupXamlProgress.Maximum(), 0.0, 1.0);
        g_popupXamlProgress.Visibility(showControls ? Visibility::Visible : Visibility::Collapsed);
        g_popupXamlProgress.Opacity(0.0);
        g_popupXamlProgress.Width(std::max(1L, progressScreen.right - progressScreen.left));
        g_popupXamlProgress.Height(std::max(1L, progressScreen.bottom - progressScreen.top));
        if (g_popupXamlProgressScale) {
            g_popupXamlProgressScale.ScaleX(controlsScale);
            g_popupXamlProgressScale.ScaleY(controlsScale);
        }
        controls::Canvas::SetLeft(g_popupXamlProgress, progressScreen.left - popupScreen.left);
        controls::Canvas::SetTop(g_popupXamlProgress, progressScreen.top - popupScreen.top);
    }
    double progressWidth = std::max(1L, progressScreen.right - progressScreen.left) * controlsScale;
    double progressHeight = std::max(1L, progressScreen.bottom - progressScreen.top) * controlsScale;
    double progressLeft = (progressScreen.left - popupScreen.left) +
                          (std::max(1L, progressScreen.right - progressScreen.left) - progressWidth) * 0.5;
    double progressTop = (progressScreen.top - popupScreen.top) +
                         (std::max(1L, progressScreen.bottom - progressScreen.top) - progressHeight) * 0.5;
    if (g_popupXamlProgressTrack) {
        g_popupXamlProgressTrack.Visibility(showControls ? Visibility::Visible : Visibility::Collapsed);
        g_popupXamlProgressTrack.Opacity(controlsOpacity);
        g_popupXamlProgressTrack.Width(progressWidth);
        g_popupXamlProgressTrack.Height(std::max(4.0, progressHeight));
        double radius = kPopupG2ProgressCornerRadius;
        g_popupXamlProgressTrack.CornerRadius({radius, radius, radius, radius});
        controls::Canvas::SetLeft(g_popupXamlProgressTrack, progressLeft);
        controls::Canvas::SetTop(g_popupXamlProgressTrack, progressTop);
    }
    if (g_popupXamlProgressHitTarget) {
        double hitHeight = std::max(22.0, progressHeight + 16.0);
        g_popupXamlProgressHitTarget.Visibility(
            showControls ? Visibility::Visible : Visibility::Collapsed);
        g_popupXamlProgressHitTarget.Width(progressWidth);
        g_popupXamlProgressHitTarget.Height(hitHeight);
        controls::Canvas::SetLeft(g_popupXamlProgressHitTarget, progressLeft);
        controls::Canvas::SetTop(
            g_popupXamlProgressHitTarget,
            progressTop - (hitHeight - std::max(4.0, progressHeight)) * 0.5);
    }
    double progressThickness = std::max(4.0, progressHeight);
    double playedWidth = std::max(0.0, progressWidth * progressRatio);
    bool hasAnyProgress = playedWidth > 0.5;
    if (g_popupXamlProgressFill) {
        double fillWidth = hasAnyProgress ? playedWidth : 0.0;
        g_popupXamlProgressFill.Visibility(showControls && hasAnyProgress ? Visibility::Visible : Visibility::Collapsed);
        g_popupXamlProgressFill.Opacity(controlsOpacity);
        g_popupXamlProgressFill.Width(fillWidth);
        g_popupXamlProgressFill.Height(progressThickness);
        double fillRadius = kPopupG2ProgressCornerRadius;
        g_popupXamlProgressFill.CornerRadius({fillRadius, fillRadius, fillRadius, fillRadius});
        controls::Canvas::SetLeft(g_popupXamlProgressFill, progressLeft);
        controls::Canvas::SetTop(g_popupXamlProgressFill, progressTop);
    }
    if (g_popupXamlProgressGlowMask) {
        g_popupXamlProgressGlowMask.Visibility(Visibility::Collapsed);
        g_popupXamlProgressGlowMask.Opacity(0.0);
        if (g_popupXamlProgressGlowClip) {
            g_popupXamlProgressGlowClip.Rect({0.0f, 0.0f, 0.0f, 0.0f});
        }
        for (auto const& layer : g_popupXamlProgressGlowLayers) {
            layer.Visibility(Visibility::Collapsed);
            layer.Opacity(0.0);
        }
    }

    for (auto const& layer : g_popupXamlProgressCoreBlurLayers) {
        layer.Visibility(Visibility::Collapsed);
        layer.Opacity(0.0);
    }

    if (g_popupXamlProgressGlowCore) {
        g_popupXamlProgressGlowCore.Visibility(Visibility::Collapsed);
        g_popupXamlProgressGlowCore.Opacity(0.0);
    }

    auto placeTimeText = [&](TextBlock const& text,
                             RECT const& source,
                             RECT const& target,
                             bool alignRight) {
        if (!text) {
            return;
        }
        RECT screen = lerpRect(source, target, controlsProgress);
        text.Visibility(showControls ? Visibility::Visible : Visibility::Collapsed);
        text.Opacity(controlsOpacity);
        text.Width(std::max(1L, screen.right - screen.left));
        text.Height(std::max(1L, screen.bottom - screen.top));
        text.TextAlignment(alignRight ? xaml::TextAlignment::Right
                                      : xaml::TextAlignment::Left);
        controls::Canvas::SetLeft(text, screen.left - popupScreen.left);
        controls::Canvas::SetTop(text, screen.top - popupScreen.top);
    };

    RECT elapsedSource{progressSource.left,
                       progressSource.bottom + 2,
                       progressSource.left + 12,
                       progressSource.bottom + 16};
    RECT durationSource{progressSource.right - 12,
                        progressSource.bottom + 2,
                        progressSource.right,
                        progressSource.bottom + 16};
    placeTimeText(g_popupXamlElapsed, elapsedSource, targetElapsed, false);
    placeTimeText(g_popupXamlDuration, durationSource, targetDuration, true);

    double controlsWidth = PopupControlsWidth();
    double controlsHeight = PopupControlsHeight();
    RECT controlsSource = sourceControlsRect;
    if (controlsSource.right <= controlsSource.left ||
        controlsSource.bottom <= controlsSource.top) {
        controlsSource = {sourceCenterX - 14,
                          sourceCenterY - 10,
                          sourceCenterX + 14,
                          sourceCenterY + 10};
    }
    RECT controlsScreen = lerpRect(controlsSource, targetControls, controlsProgress);
    if (g_popupXamlControls) {
        g_popupXamlControls.Visibility(showControls ? Visibility::Visible : Visibility::Collapsed);
        g_popupXamlControls.Opacity(controlsOpacity);
        g_popupXamlControls.Width(controlsWidth);
        g_popupXamlControls.Height(controlsHeight);

        double screenWidth = std::max(1L, controlsScreen.right - controlsScreen.left);
        double screenHeight = std::max(1L, controlsScreen.bottom - controlsScreen.top);
        double left = controlsScreen.left - popupScreen.left +
                      (screenWidth - controlsWidth) * 0.5;
        double top = controlsScreen.top - popupScreen.top +
                     (screenHeight - controlsHeight) * 0.5;

        controls::Canvas::SetLeft(g_popupXamlControls, left);
        controls::Canvas::SetTop(g_popupXamlControls, top);

        // Keep child button slots fixed in the local Canvas coordinates instead
        // of scaling the whole StackPanel. The previous group RenderTransform
        // could diverge from hit testing under high DPI, making the hover
        // highlight and glyph appear offset from each other.
        int index = 0;
        for (auto const& child : g_popupXamlControls.Children()) {
            if (auto buttonSurface = child.try_as<Border>()) {
                double buttonWidth = LerpDouble(32.0,
                                                PopupButtonWidth(),
                                                buttonProgress);
                double buttonHeight = LerpDouble(28.0,
                                                 PopupButtonHeight(),
                                                 buttonProgress);
                double finalButtonLeft = PopupButtonLeft(index) +
                    (PopupButtonWidth() - buttonWidth) * 0.5;
                double finalButtonTop = PopupButtonTop() +
                    (PopupButtonHeight() - buttonHeight) * 0.5;
                double sourceGroupWidth = std::max(1L,
                    controlsSource.right - controlsSource.left);
                double sourceSlotWidth = sourceGroupWidth / 3.0;
                double sourceButtonLeft =
                    (controlsWidth - sourceGroupWidth) * 0.5 +
                    sourceSlotWidth * static_cast<double>(index) +
                    (sourceSlotWidth - buttonWidth) * 0.5;
                double sourceButtonTop =
                    controlsHeight * 0.5 - buttonHeight * 0.5;
                double buttonLeft = sourceButtonLeft +
                    (finalButtonLeft - sourceButtonLeft) * buttonProgress;
                double buttonTop = sourceButtonTop +
                    (finalButtonTop - sourceButtonTop) * buttonProgress;
                controls::Canvas::SetLeft(buttonSurface, buttonLeft);
                controls::Canvas::SetTop(buttonSurface, buttonTop);
                buttonSurface.Width(buttonWidth);
                buttonSurface.Height(buttonHeight);
                buttonSurface.Opacity(1.0);
                buttonSurface.Visibility(Visibility::Visible);
                if (auto icon = buttonSurface.Child().try_as<controls::FontIcon>()) {
                    icon.Visibility(Visibility::Visible);
                    icon.Opacity(1.0);
                    icon.Width(buttonWidth);
                    icon.Height(buttonHeight);
                    icon.Margin({0, 0, 0, 0});
                    icon.HorizontalAlignment(HorizontalAlignment::Center);
                    icon.VerticalAlignment(VerticalAlignment::Center);
                } else if (auto text = buttonSurface.Child().try_as<TextBlock>()) {
                    text.Visibility(Visibility::Visible);
                    text.Opacity(1.0);
                    text.Width(buttonWidth);
                    text.Height(buttonHeight);
                    text.Margin({0, 0, 0, 0});
                }
                ++index;
            }
        }
    }

    ApplyPopupMediaTransitionVisuals();
}

bool IsInsideRoundedPopupSample(double x, double y, int width, int height,
                                double radius, double inset) {
    double left = inset;
    double top = inset;
    double right = width - inset;
    double bottom = height - inset;
    if (x < left || x >= right || y < top || y >= bottom) {
        return false;
    }
    radius = std::max(0.0, radius - inset);
    double nearestX = Clamp(x, left + radius, right - radius);
    double nearestY = Clamp(y, top + radius, bottom - radius);
    double dx = x - nearestX;
    double dy = y - nearestY;
    return dx * dx + dy * dy <= radius * radius;
}

double RoundedPopupCoverage(int x, int y, int width, int height,
                            double radius, double inset) {
    constexpr double offsets[] = {0.25, 0.75};
    int samples = 0;
    for (double offsetY : offsets) {
        for (double offsetX : offsets) {
            if (IsInsideRoundedPopupSample(x + offsetX, y + offsetY,
                                           width, height, radius, inset)) {
                ++samples;
            }
        }
    }
    return samples / 4.0;
}

void RenderExpandedPopupLayer() {
    if (!g_expandedPopup) {
        return;
    }
    if (g_popupXamlRoot) {
        UpdatePopupXamlVisuals();
        return;
    }

    RECT windowRect{};
    GetWindowRect(g_expandedPopup, &windowRect);
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    if (width <= 0 || height <= 0) {
        return;
    }

    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;

    void* pixelMemory = nullptr;
    HDC screenDc = GetDC(nullptr);
    HDC memoryDc = CreateCompatibleDC(screenDc);
    HBITMAP surface = CreateDIBSection(screenDc, &info, DIB_RGB_COLORS,
                                       &pixelMemory, nullptr, 0);
    if (!memoryDc || !surface || !pixelMemory) {
        if (surface) DeleteObject(surface);
        if (memoryDc) DeleteDC(memoryDc);
        if (screenDc) ReleaseDC(nullptr, screenDc);
        return;
    }

    HGDIOBJ oldBitmap = SelectObject(memoryDc, surface);
    memset(pixelMemory, 0, static_cast<size_t>(width) * height * 4);
    bool dark = IsDarkModeApprox();
    RECT fullSurface{0, 0, width, height};
    HBRUSH baseBrush = CreateSolidBrush(
        dark ? RGB(42, 42, 47) : RGB(243, 243, 246));
    FillRect(memoryDc, &fullSurface, baseBrush);
    DeleteObject(baseBrush);
    PaintExpandedPopup(g_expandedPopup, memoryDc);

    BYTE backgroundAlpha = dark ? 0xC8 : 0xD4;
    BYTE backgroundR = dark ? 42 : 243;
    BYTE backgroundG = dark ? 42 : 243;
    BYTE backgroundB = dark ? 47 : 246;
    auto pixels = static_cast<BYTE*>(pixelMemory);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            BYTE* pixel = pixels + (static_cast<size_t>(y) * width + x) * 4;
            double outerCoverage = RoundedPopupCoverage(x, y, width, height,
                                                        20.0, 0.0);
            if (outerCoverage <= 0.0) {
                pixel[0] = 0;
                pixel[1] = 0;
                pixel[2] = 0;
                pixel[3] = 0;
                continue;
            }

            bool isBackground =
                std::abs(static_cast<int>(pixel[2]) - backgroundR) <= 2 &&
                std::abs(static_cast<int>(pixel[1]) - backgroundG) <= 2 &&
                std::abs(static_cast<int>(pixel[0]) - backgroundB) <= 2;
            if (isBackground) {
                BYTE alpha = static_cast<BYTE>(std::lround(backgroundAlpha * outerCoverage));
                pixel[0] = static_cast<BYTE>(std::lround(backgroundB * alpha / 255.0));
                pixel[1] = static_cast<BYTE>(std::lround(backgroundG * alpha / 255.0));
                pixel[2] = static_cast<BYTE>(std::lround(backgroundR * alpha / 255.0));
                pixel[3] = alpha;
            } else {
                BYTE alpha = static_cast<BYTE>(std::lround(255.0 * outerCoverage));
                pixel[0] = static_cast<BYTE>(std::lround(pixel[0] * alpha / 255.0));
                pixel[1] = static_cast<BYTE>(std::lround(pixel[1] * alpha / 255.0));
                pixel[2] = static_cast<BYTE>(std::lround(pixel[2] * alpha / 255.0));
                pixel[3] = alpha;
            }
        }
    }

    POINT destination{windowRect.left, windowRect.top};
    POINT source{0, 0};
    SIZE size{width, height};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_expandedPopup, screenDc, &destination, &size,
                        memoryDc, &source, 0, &blend, ULW_ALPHA);

    SelectObject(memoryDc, oldBitmap);
    DeleteObject(surface);
    DeleteDC(memoryDc);
    ReleaseDC(nullptr, screenDc);
}

bool GetElementScreenRect(FrameworkElement const& element, RECT& result) {
    if (!element || !g_taskbarWnd) {
        return false;
    }

    try {
        RECT taskbar{};
        GetWindowRect(g_taskbarWnd, &taskbar);
        auto transform = element.TransformToVisual(nullptr);
        auto point = transform.TransformPoint({0, 0});
        double scale = static_cast<double>(GetDpiForWindow(g_taskbarWnd)) / 96.0;
        result.left = taskbar.left + static_cast<int>(std::lround(point.X * scale));
        result.top = taskbar.top + static_cast<int>(std::lround(point.Y * scale));
        result.right = result.left +
                       static_cast<int>(std::lround(element.ActualWidth() * scale));
        result.bottom = result.top +
                        static_cast<int>(std::lround(element.ActualHeight() * scale));
        return result.right > result.left && result.bottom > result.top;
    } catch (...) {
        return false;
    }
}

double PopupDpiScale(HWND hwnd) {
    HWND source = hwnd ? hwnd : (g_expandedPopup ? g_expandedPopup : g_taskbarWnd);
    UINT dpi = source ? GetDpiForWindow(source) : 96;
    if (dpi == 0) {
        dpi = 96;
    }
    return std::max(0.01, static_cast<double>(dpi) / 96.0);
}

int PopupDipToPx(double value, HWND hwnd) {
    return static_cast<int>(std::lround(value * PopupDpiScale(hwnd)));
}

RECT PopupScreenRectToLocalDip(RECT const& rect,
                               RECT const& popupScreenPx,
                               double scale) {
    return {
        static_cast<LONG>(std::lround((rect.left - popupScreenPx.left) / scale)),
        static_cast<LONG>(std::lround((rect.top - popupScreenPx.top) / scale)),
        static_cast<LONG>(std::lround((rect.right - popupScreenPx.left) / scale)),
        static_cast<LONG>(std::lround((rect.bottom - popupScreenPx.top) / scale))};
}

bool TaskbarPopupOpensDown(RECT const& taskbarRect,
                           MONITORINFO const& monitorInfo) {
    if (taskbarRect.right <= taskbarRect.left ||
        taskbarRect.bottom <= taskbarRect.top) {
        return false;
    }

    int distanceToTop =
        std::abs(taskbarRect.top - monitorInfo.rcMonitor.top);
    int distanceToBottom =
        std::abs(monitorInfo.rcMonitor.bottom - taskbarRect.bottom);
    if (distanceToTop != distanceToBottom) {
        return distanceToTop < distanceToBottom;
    }

    int taskbarCenter =
        (taskbarRect.top + taskbarRect.bottom) / 2;
    int monitorCenter =
        (monitorInfo.rcMonitor.top + monitorInfo.rcMonitor.bottom) / 2;
    return taskbarCenter <= monitorCenter;
}

void CapturePopupSourceGeometry() {
    if (!GetElementScreenRect(g_playerGrid, g_popupSourceRect)) {
        RECT taskbar{};
        GetWindowRect(g_taskbarWnd, &taskbar);
        g_popupSourceRect = {
            taskbar.left,
            taskbar.top,
            taskbar.left + static_cast<LONG>(PopupDipToPx(g_layout.compactWidth, g_taskbarWnd)),
            taskbar.top + static_cast<LONG>(PopupDipToPx(g_layout.compactHeight, g_taskbarWnd))};
    }
    auto art = FindChildByName(g_playerGrid, L"Island_ArtFallback");
    if (!GetElementScreenRect(art, g_popupSourceArtRect)) {
        g_popupSourceArtRect = {
            g_popupSourceRect.left + PopupDipToPx(10.0, g_taskbarWnd),
            g_popupSourceRect.top + PopupDipToPx(6.0, g_taskbarWnd),
            g_popupSourceRect.left + PopupDipToPx(38.0, g_taskbarWnd),
            g_popupSourceRect.top + PopupDipToPx(34.0, g_taskbarWnd)};
    }
    auto title = FindChildByName(g_playerGrid, L"Island_Title");
    if (!GetElementScreenRect(title, g_popupSourceTitleRect)) {
        g_popupSourceTitleRect = {
            g_popupSourceRect.left + PopupDipToPx(48.0, g_taskbarWnd),
            g_popupSourceRect.top + PopupDipToPx(5.0, g_taskbarWnd),
            g_popupSourceRect.right - PopupDipToPx(10.0, g_taskbarWnd),
            g_popupSourceRect.top + PopupDipToPx(22.0, g_taskbarWnd)};
    }
    auto artist = FindChildByName(g_playerGrid, L"Island_CompactArtist");
    if (!GetElementScreenRect(artist, g_popupSourceArtistRect)) {
        g_popupSourceArtistRect = {
            g_popupSourceRect.left + PopupDipToPx(48.0, g_taskbarWnd),
            g_popupSourceRect.top + PopupDipToPx(21.0, g_taskbarWnd),
            g_popupSourceRect.right - PopupDipToPx(10.0, g_taskbarWnd),
            g_popupSourceRect.bottom - PopupDipToPx(4.0, g_taskbarWnd)};
    }

    auto progress = FindChildByName(g_playerGrid, L"Island_Progress");
    if (!GetElementScreenRect(progress, g_popupSourceProgressRect)) {
        int progressWidth = PopupDipToPx(g_layout.progressWidth, g_taskbarWnd);
        int progressHeight = std::max(1, PopupDipToPx(g_layout.progressHeight, g_taskbarWnd));
        int contentRight = g_popupSourceRect.right -
            PopupDipToPx(g_layout.contentMarginX, g_taskbarWnd);
        int centerY = (g_popupSourceRect.top + g_popupSourceRect.bottom) / 2;
        g_popupSourceProgressRect = {
            contentRight - progressWidth,
            centerY - progressHeight / 2,
            contentRight,
            centerY - progressHeight / 2 + progressHeight};
    }

    auto details = FindChildByName(g_playerGrid, L"Island_Details");
    if (!GetElementScreenRect(details, g_popupSourceControlsRect)) {
        int compactButtonWidth = PopupDipToPx(32.0, g_taskbarWnd);
        int compactButtonHeight = PopupDipToPx(28.0, g_taskbarWnd);
        int compactButtonGap = PopupDipToPx(4.0, g_taskbarWnd);
        int controlsWidth = compactButtonWidth * 3 + compactButtonGap * 2;
        int centerY = (g_popupSourceRect.top + g_popupSourceRect.bottom) / 2;
        int right = g_popupSourceProgressRect.left -
            PopupDipToPx(g_layout.progressMarginLeft, g_taskbarWnd);
        if (right <= g_popupSourceRect.left) {
            right = g_popupSourceRect.right -
                PopupDipToPx(g_layout.contentMarginX, g_taskbarWnd);
        }
        g_popupSourceControlsRect = {
            right - controlsWidth,
            centerY - compactButtonHeight / 2,
            right,
            centerY - compactButtonHeight / 2 + compactButtonHeight};
        if (g_popupSourceControlsRect.left < g_popupSourceRect.left) {
            int centerX = (g_popupSourceRect.left + g_popupSourceRect.right) / 2;
            g_popupSourceControlsRect = {
                centerX - controlsWidth / 2,
                centerY - compactButtonHeight / 2,
                centerX - controlsWidth / 2 + controlsWidth,
                centerY - compactButtonHeight / 2 + compactButtonHeight};
        }
    }

    HMONITOR monitor = MonitorFromRect(&g_popupSourceRect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{sizeof(monitorInfo)};
    if (GetMonitorInfoW(monitor, &monitorInfo)) {
        double dpiScale = PopupDpiScale(g_taskbarWnd);
        int monitorWidthPx = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
        int monitorWidthDip = std::max(1, static_cast<int>(std::floor(monitorWidthPx / dpiScale)));
        RECT taskbar{};
        GetWindowRect(g_taskbarWnd, &taskbar);
        bool opensDown = TaskbarPopupOpensDown(taskbar, monitorInfo);
        g_popupOpensDown = opensDown;
        g_taskbarAtTop = opensDown;
        int popupGap = PopupDipToPx(opensDown ? 2.0 : 8.0, g_taskbarWnd);
        int edgePad = PopupDipToPx(12.0, g_taskbarWnd);
        int finalTop = taskbar.bottom + popupGap;
        int finalBottom = taskbar.top - popupGap;
        int availableHeightPx =
            opensDown
                ? monitorInfo.rcMonitor.bottom - finalTop - edgePad
                : finalBottom - monitorInfo.rcMonitor.top - edgePad;
        int availableHeightDip = std::max(1, static_cast<int>(std::floor(availableHeightPx / dpiScale)));

        int finalWidthDip = 0;
        int finalHeightDip = 0;
        if (g_settings.compact) {
            int maxCardWidth = std::max(
                kPopupCompactMinimumCardWidth,
                monitorWidthDip - 24 - kPopupHostShadowMargin * 2);
            int cardWidth = Clamp(
                std::min(g_settings.expandedWidth, maxCardWidth),
                kPopupCompactMinimumCardWidth,
                640);
            finalWidthDip = PopupCompactFinalWidth(cardWidth);
            finalHeightDip = PopupCompactFinalHeight();
        } else {
            int maxArtBySetting = std::max(96, g_settings.expandedWidth -
                (PopupSurfaceGap() * 2 + kPopupHostShadowMargin * 2));
            int maxArtByMonitorWidth = std::max(96, monitorWidthDip - 24 -
                (PopupSurfaceGap() * 2 + kPopupHostShadowMargin * 2));
            int maxArtByHeight = std::max(96, availableHeightDip -
                (kPopupCardHeight + PopupArtCardGap() +
                 PopupSurfaceGap() * 2 + kPopupHostShadowMargin * 2));
            int targetArt = Clamp(std::min({maxArtBySetting,
                                            maxArtByMonitorWidth,
                                            maxArtByHeight,
                                            kPopupMaximumArtSize}),
                                  96, kPopupMaximumArtSize);
            if (targetArt < kPopupMinimumArtSize &&
                maxArtByHeight >= kPopupMinimumArtSize) {
                targetArt = kPopupMinimumArtSize;
            }
            finalWidthDip = PopupFinalWidthFromArtSize(targetArt);
            finalHeightDip = PopupFinalHeightFromArtSize(targetArt);
        }
        int finalWidth = PopupDipToPx(finalWidthDip, g_taskbarWnd);
        int finalHeight = PopupDipToPx(finalHeightDip, g_taskbarWnd);
        g_popupExpandedWidth = static_cast<double>(finalWidth);
        g_popupExpandedHeight = static_cast<double>(finalHeight);

        int rightSpace = monitorInfo.rcMonitor.right - g_popupSourceRect.left;
        int leftSpace = g_popupSourceRect.right - monitorInfo.rcMonitor.left;
        if (g_settings.position.rfind(L"tray_", 0) == 0 ||
            g_settings.position.rfind(L"taskbar_", 0) == 0) {
            g_popupExpandsRight = PopupShouldExpandRight(monitorInfo);
        } else {
            g_popupExpandsRight = rightSpace >= static_cast<int>(g_popupExpandedWidth) ||
                                  rightSpace >= leftSpace;
        }
        int finalLeft = g_popupExpandsRight
                            ? g_popupSourceRect.left
                            : g_popupSourceRect.right - finalWidth;
        finalLeft = Clamp(finalLeft,
                          static_cast<int>(monitorInfo.rcMonitor.left) + edgePad,
                          static_cast<int>(monitorInfo.rcMonitor.right) - finalWidth - edgePad);
        if (opensDown) {
            g_popupFinalRect = {finalLeft, finalTop,
                                finalLeft + finalWidth,
                                finalTop + finalHeight};
        } else {
            g_popupFinalRect = {finalLeft, finalBottom - finalHeight,
                                finalLeft + finalWidth, finalBottom};
        }
    } else {
        int finalWidthDip = 0;
        int finalHeightDip = 0;
        if (g_settings.compact) {
            int cardWidth = Clamp(g_settings.expandedWidth,
                                  kPopupCompactMinimumCardWidth, 640);
            finalWidthDip = PopupCompactFinalWidth(cardWidth);
            finalHeightDip = PopupCompactFinalHeight();
        } else {
            int targetArt = Clamp(g_settings.expandedWidth -
                                      (PopupSurfaceGap() * 2 + kPopupHostShadowMargin * 2),
                                  96, kPopupMaximumArtSize);
            finalWidthDip = PopupFinalWidthFromArtSize(targetArt);
            finalHeightDip = PopupFinalHeightFromArtSize(targetArt);
        }
        int finalWidth = PopupDipToPx(finalWidthDip, g_taskbarWnd);
        int finalHeight = PopupDipToPx(finalHeightDip, g_taskbarWnd);
        RECT taskbar{};
        bool opensDown = GetWindowRect(g_taskbarWnd, &taskbar) &&
                         g_popupSourceRect.top <=
                             (GetSystemMetrics(SM_YVIRTUALSCREEN) +
                              GetSystemMetrics(SM_CYVIRTUALSCREEN) / 2);
        g_popupOpensDown = opensDown;
        g_taskbarAtTop = opensDown;
        int popupGap = PopupDipToPx(opensDown ? 2.0 : 8.0, g_taskbarWnd);
        g_popupExpandedWidth = static_cast<double>(finalWidth);
        g_popupExpandedHeight = static_cast<double>(finalHeight);
        g_popupExpandsRight = true;
        if (opensDown) {
            int finalTop = taskbar.bottom + popupGap;
            g_popupFinalRect = {g_popupSourceRect.left,
                                finalTop,
                                g_popupSourceRect.left + finalWidth,
                                finalTop + finalHeight};
        } else {
            g_popupFinalRect = {g_popupSourceRect.left,
                                g_popupSourceRect.top - finalHeight - popupGap,
                                g_popupSourceRect.left + finalWidth,
                                g_popupSourceRect.top - popupGap};
        }
    }
}

void UpdatePopupBackdropOverlayWindow();
bool CalculatePopupBackdropOverlayRect(RECT& overlayRect, int& cornerRadiusPx);
void HidePopupBackdropOverlayWindowVisualOnly();
void HidePopupBackdropOverlayWindow();
void StopPopupOverlayWgcBackdrop();
void ReleasePopupOverlayWgcDeviceResources();
bool StartPopupOverlayWgcBackdrop(
    RECT const& captureRect,
    int widthPx,
    int heightPx,
    PopupOverlayWgcRenderParameters const& renderParameters);
bool SetPopupWindowCaptureExclusion(HWND hwnd, bool exclude);
bool SetExpandedPopupCaptureExclusion(bool exclude);
void ClearPopupBackdropOverlayHandoffCache();
bool ClearPopupBackdropOverlayLayeredSurface(HWND hwnd);

void PositionExpandedPopup() {
    if (!g_expandedPopup || !g_taskbarWnd) {
        return;
    }

    if (g_popupXamlRoot) {
        // Smooth XAML path: keep the DesktopWindowXamlSource host fixed for the
        // whole open/close cycle. The old implementation resized/moved this
        // HWND every frame, which is expensive in Explorer and looked like
        // dropped frames. The visible morph now happens entirely inside XAML.
        RECT hostRect = g_popupHostRect;
        if (hostRect.right <= hostRect.left || hostRect.bottom <= hostRect.top) {
            hostRect = UnionPopupRects(g_popupSourceRect, g_popupFinalRect);
        }
        int width = std::max(1L, hostRect.right - hostRect.left);
        int height = std::max(1L, hostRect.bottom - hostRect.top);
        g_popupCurrentWidth = width;
        g_popupCurrentHeight = height;

        if (!EqualRect(&hostRect, &g_lastPopupWindowRect)) {
            g_lastPopupWindowRect = hostRect;
            SetWindowPos(g_expandedPopup, HWND_TOPMOST,
                         hostRect.left, hostRect.top, width, height,
                         SWP_NOACTIVATE | SWP_NOCOPYBITS |
                             SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);
            if (g_popupXamlChild) {
                MoveWindow(g_popupXamlChild, 0, 0, width, height, FALSE);
            }
        }
        UpdatePopupBackdropOverlayWindow();
        return;
    }

    // GDI fallback path keeps the old window-level morph.
    double progress = PopupProgress();
    int left = LerpInt(g_popupSourceRect.left, g_popupFinalRect.left, progress);
    int top = LerpInt(g_popupSourceRect.top, g_popupFinalRect.top, progress);
    int right = LerpInt(g_popupSourceRect.right, g_popupFinalRect.right, progress);
    int bottom = LerpInt(g_popupSourceRect.bottom, g_popupFinalRect.bottom, progress);
    int width = std::max(1, right - left);
    int height = std::max(1, bottom - top);
    g_popupCurrentWidth = width;
    g_popupCurrentHeight = height;

    RECT newRect{left, top, left + width, top + height};
    if (EqualRect(&newRect, &g_lastPopupWindowRect)) {
        return;
    }
    g_lastPopupWindowRect = newRect;
    SetWindowPos(g_expandedPopup, HWND_TOPMOST, left, top, width, height,
                 SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);
    if (g_popupXamlChild) {
        MoveWindow(g_popupXamlChild, 0, 0, width, height, FALSE);
    }
    UpdatePopupBackdropOverlayWindow();
}
void SetCompactIslandSuppressed(bool suppressed);
void StartPopupXamlRenderLoop();
void StopHoverRenderLoop();
void StopCompactTextRenderLoop();
void StopCompactProgressRenderLoop();

bool UpdateCompactProgressFromSnapshot() {
    if (!g_compactProgress) {
        return false;
    }

    std::chrono::steady_clock::time_point timestamp;
    MediaState state = SnapshotMediaWithTimestamp(timestamp);
    int64_t positionTicks = state.positionTicks;
    if (state.hasSession && state.isPlaying && state.durationTicks > 0 &&
        timestamp.time_since_epoch().count() != 0) {
        double ageSeconds =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - timestamp)
                .count() /
            1000.0;
        // The event-driven media worker has a 30-second safety refresh, so local
        // interpolation can remain smooth between provider timeline events.
        ageSeconds = Clamp(ageSeconds, 0.0, 30.0);
        positionTicks +=
            static_cast<int64_t>(ageSeconds * 10000000.0);
    }

    double value = 0.0;
    if (state.hasSession && state.durationTicks > 0) {
        positionTicks = std::clamp<int64_t>(
            positionTicks, 0, state.durationTicks);
        value = Clamp(static_cast<double>(positionTicks) /
                          static_cast<double>(state.durationTicks) * 1000.0,
                      0.0, 1000.0);
    }
    g_compactProgress.Value(value);
    return state.hasSession && state.isPlaying && state.durationTicks > 0;
}

void StopCompactProgressRenderLoop() {
    if (!g_compactProgressRenderingHooked) {
        return;
    }
    try {
        mediax::CompositionTarget::Rendering(
            g_compactProgressRenderingToken);
    } catch (...) {
    }
    g_compactProgressRenderingHooked = false;
    g_lastCompactProgressFrameTime = {};
}

void OnCompactProgressRendering(
    winrt::Windows::Foundation::IInspectable const&,
    winrt::Windows::Foundation::IInspectable const&) {
    try {
        if (!g_playerGrid || !g_compactProgress || g_expanded || g_unloading) {
            StopCompactProgressRenderLoop();
            return;
        }

        auto now = std::chrono::steady_clock::now();
        if (g_lastCompactProgressFrameTime.time_since_epoch().count() != 0 &&
            now - g_lastCompactProgressFrameTime <
                std::chrono::milliseconds(33)) {
            return;
        }
        g_lastCompactProgressFrameTime = now;

        if (!UpdateCompactProgressFromSnapshot()) {
            StopCompactProgressRenderLoop();
        }
    } catch (...) {
        StopCompactProgressRenderLoop();
    }
}

void StartCompactProgressRenderLoop() {
    if (!g_compactProgress || g_compactProgressRenderingHooked ||
        g_expanded || g_unloading) {
        return;
    }
    g_lastCompactProgressFrameTime = {};
    g_compactProgressRenderingToken =
    mediax::CompositionTarget::Rendering(OnCompactProgressRendering);
    g_compactProgressRenderingHooked = true;
}

void SetCompactTextEdgeFadeOpacity(double opacity) {
    opacity = Clamp(opacity, 0.0, 1.0);
    try {
        if (g_compactTextLeftFade) {
            g_compactTextLeftFade.Opacity(opacity);
        }
        if (g_compactTextRightFade) {
            g_compactTextRightFade.Opacity(opacity);
        }
    } catch (...) {
    }
}

void ResetCompactTextAnimationVisuals() {
    try {
        if (g_compactTitleTranslate) {
            g_compactTitleTranslate.X(0.0);
        }
        if (g_compactArtistTranslate) {
            g_compactArtistTranslate.X(0.0);
        }
        if (g_compactOutgoingTitleTranslate) {
            g_compactOutgoingTitleTranslate.X(0.0);
        }
        if (g_compactOutgoingArtistTranslate) {
            g_compactOutgoingArtistTranslate.X(0.0);
        }
        if (g_compactTitleText) {
            g_compactTitleText.Opacity(1.0);
        }
        if (g_compactArtistText) {
            g_compactArtistText.Opacity(1.0);
        }
        if (g_compactOutgoingTitleText) {
            g_compactOutgoingTitleText.Opacity(0.0);
        }
        if (g_compactOutgoingArtistText) {
            g_compactOutgoingArtistText.Opacity(0.0);
        }
        if (g_compactAlbumArtImage) {
            g_compactAlbumArtImage.Opacity(1.0);
        }
        if (g_compactAlbumArtFade) {
            g_compactAlbumArtFade.Opacity(0.0);
            g_compactAlbumArtFade.Source(nullptr);
        }
        g_compactTextEdgeFadeActive = false;
        SetCompactTextEdgeFadeOpacity(0.0);
    } catch (...) {
    }
}

double CompactTextClipWidthFallback() {
    double width = g_layout.compactWidth -
                   (g_layout.contentMarginX * 2.0 +
                    g_layout.artColumnWidth +
                    g_layout.textMarginX * 2.0 + 4.0);
    return std::max(1.0, width);
}

double CompactTextClipHeightFallback() {
    double height = g_layout.compactHeight - g_layout.contentMarginY * 2.0;
    return std::max(1.0, height);
}

double CompactTextClipPadding() {
    return Clamp(g_layout.textMarginX * 0.75 + 2.0, 5.0, 9.0);
}

double CompactTextEdgeFadeWidth() {
    return Clamp(g_layout.textMarginX + 9.0, 12.0, 18.0);
}

void RefreshCompactTextHostClip(bool forceLayout = false) {
    if (!g_compactTextHost) {
        return;
    }

    try {
        if (forceLayout && g_playerGrid) {
            g_playerGrid.UpdateLayout();
        }

        double fallbackWidth = CompactTextClipWidthFallback();
        double fallbackHeight = CompactTextClipHeightFallback();
        double clipWidth = g_compactTextHost.ActualWidth();
        double clipHeight = g_compactTextHost.ActualHeight();

        // Right after a settings/theme change, XAML may report the previous
        // measured width for one frame. A too-small clip causes the compact
        // title/artist to look cropped until the next expand/collapse. Prefer a
        // geometry-derived fallback whenever ActualWidth is clearly stale.
        if (clipWidth <= 1.0 || clipWidth < fallbackWidth * 0.65) {
            clipWidth = fallbackWidth;
        }
        if (clipHeight <= 1.0 || clipHeight < fallbackHeight * 0.55) {
            clipHeight = fallbackHeight;
        }

        double clipPadding = CompactTextClipPadding();
        ApplyElementClipWithPadding(g_compactTextHost, clipWidth, clipHeight,
                                    clipPadding);

        double fadeWidth = CompactTextEdgeFadeWidth() + clipPadding;
        auto configureFade = [&](Border const& fade, bool leftEdge) {
            if (!fade) {
                return;
            }
            double leftCoverOffset = leftEdge ? Clamp(g_layout.textMarginX + 4.0, 8.0, 16.0) : 0.0;
            fade.Width(fadeWidth + leftCoverOffset);
            fade.Height(clipHeight);
            fade.Margin(leftEdge ? xaml::Thickness{-(clipPadding + leftCoverOffset), 0, 0, 0}
                                 : xaml::Thickness{0, 0, -clipPadding, 0});
            fade.Background(CompactTextEdgeFadeBrush(leftEdge));
        };
        configureFade(g_compactTextLeftFade, true);
        configureFade(g_compactTextRightFade, false);
    } catch (...) {
    }
}

void StopCompactTextRenderLoop() {
    if (!g_compactTextRenderingHooked) {
        return;
    }
    try {
        mediax::CompositionTarget::Rendering(g_compactTextRenderingToken);
    } catch (...) {
    }
    g_compactTextRenderingHooked = false;
    g_lastCompactTextFrameTime = {};
}

void OnCompactTextRendering(winrt::Windows::Foundation::IInspectable const&,
                            winrt::Windows::Foundation::IInspectable const&) {
    try {
        if (!g_playerGrid || !g_compactTitleTranslate || !g_compactArtistTranslate ||
            !g_compactTitleText || !g_compactArtistText || g_unloading) {
            StopCompactTextRenderLoop();
            ResetCompactTextAnimationVisuals();
            return;
        }

        auto now = std::chrono::steady_clock::now();
        double dtSec = 1.0 / 60.0;
        if (g_lastCompactTextFrameTime.time_since_epoch().count() != 0) {
            dtSec = std::chrono::duration_cast<std::chrono::microseconds>(
                        now - g_lastCompactTextFrameTime)
                        .count() /
                    1000000.0;
        }
        g_lastCompactTextFrameTime = now;
        dtSec = Clamp(dtSec, 0.0, 0.05);

        // Slightly slower than the previous pass so the crossfade and slide are
        // more visible. PopupTextMotionEase keeps velocity changes even.
        g_compactTextProgress = Clamp(g_compactTextProgress + dtSec * 2.35, 0.0, 1.0);
        constexpr double kTextSlideOffset = 21.0;
        RefreshCompactTextHostClip(false);

        double titleIn = PopupTextMotionEase(Clamp((g_compactTextProgress - 0.02) / 0.92, 0.0, 1.0));
        double artistIn = PopupTextMotionEase(Clamp((g_compactTextProgress - 0.28) / 0.70, 0.0, 1.0));
        double titleOut = PopupTextMotionEase(Clamp((g_compactTextProgress + 0.02) / 0.84, 0.0, 1.0));
        double artistOut = PopupTextMotionEase(Clamp((g_compactTextProgress - 0.12) / 0.80, 0.0, 1.0));
        double artIn = PopupTextMotionEase(Clamp(g_compactTextProgress / 0.96, 0.0, 1.0));
        if (g_compactTextEdgeFadeActive) {
            double edgeFadeIn = PopupTextMotionEase(Clamp((g_compactTextProgress - 0.08) / 0.20,
                                                          0.0, 1.0));
            double edgeFadeOut = 1.0 - PopupTextMotionEase(Clamp((g_compactTextProgress - 0.78) / 0.18,
                                                                 0.0, 1.0));
            SetCompactTextEdgeFadeOpacity(edgeFadeIn * edgeFadeOut);
        }

        // Reversed direction: new compact text now enters from the right.
        g_compactTitleTranslate.X(kTextSlideOffset * (1.0 - titleIn));
        g_compactArtistTranslate.X(kTextSlideOffset * (1.0 - artistIn));
        g_compactTitleText.Opacity(titleIn);
        g_compactArtistText.Opacity(artistIn);

        if (g_compactOutgoingTitleText && g_compactOutgoingTitleTranslate) {
            // Old compact text exits to the left.
            g_compactOutgoingTitleTranslate.X(-kTextSlideOffset * titleOut);
            g_compactOutgoingTitleText.Opacity(1.0 - titleOut);
        }
        if (g_compactOutgoingArtistText && g_compactOutgoingArtistTranslate) {
            g_compactOutgoingArtistTranslate.X(-kTextSlideOffset * artistOut);
            g_compactOutgoingArtistText.Opacity(1.0 - artistOut);
        }
        if (g_compactAlbumArtImage) {
            g_compactAlbumArtImage.Opacity(artIn);
        }
        if (g_compactAlbumArtFade) {
            g_compactAlbumArtFade.Opacity(1.0 - artIn);
        }

        if (g_compactTextProgress >= 1.0) {
            ResetCompactTextAnimationVisuals();
            StopCompactTextRenderLoop();
        }
    } catch (...) {
        ResetCompactTextAnimationVisuals();
        StopCompactTextRenderLoop();
    }
}

void StartCompactTrackTransition(std::wstring const& oldTitle,
                                 std::wstring const& oldArtist,
                                 bool animateText,
                                 bool animateArt) {
    if (!g_playerGrid || g_expanded || g_unloading) {
        ResetCompactTextAnimationVisuals();
        return;
    }
    if (!g_compactTitleTranslate || !g_compactArtistTranslate ||
        !g_compactTitleText || !g_compactArtistText) {
        return;
    }

    constexpr double kTextSlideOffset = 22.0;
    g_compactTextProgress = 0.0;
    g_lastCompactTextFrameTime = std::chrono::steady_clock::now();

    if (animateText) {
        if (g_compactOutgoingTitleText) {
            g_compactOutgoingTitleText.Text(winrt::hstring(oldTitle));
            g_compactOutgoingTitleText.Opacity(1.0);
        }
        if (g_compactOutgoingArtistText) {
            g_compactOutgoingArtistText.Text(winrt::hstring(oldArtist));
            g_compactOutgoingArtistText.Opacity(1.0);
        }
        if (g_compactOutgoingTitleTranslate) {
            g_compactOutgoingTitleTranslate.X(0.0);
        }
        if (g_compactOutgoingArtistTranslate) {
            g_compactOutgoingArtistTranslate.X(0.0);
        }
        // Reversed direction: new compact text starts on the right.
        g_compactTitleTranslate.X(kTextSlideOffset);
        g_compactArtistTranslate.X(kTextSlideOffset);
        g_compactTitleText.Opacity(0.0);
        g_compactArtistText.Opacity(0.0);
        g_compactTextEdgeFadeActive = true;
        SetCompactTextEdgeFadeOpacity(0.0);
    } else {
        if (g_compactTitleTranslate) g_compactTitleTranslate.X(0.0);
        if (g_compactArtistTranslate) g_compactArtistTranslate.X(0.0);
        if (g_compactTitleText) g_compactTitleText.Opacity(1.0);
        if (g_compactArtistText) g_compactArtistText.Opacity(1.0);
        if (g_compactOutgoingTitleText) g_compactOutgoingTitleText.Opacity(0.0);
        if (g_compactOutgoingArtistText) g_compactOutgoingArtistText.Opacity(0.0);
        g_compactTextEdgeFadeActive = false;
        SetCompactTextEdgeFadeOpacity(0.0);
    }

    if (animateArt) {
        if (g_compactAlbumArtImage) {
            g_compactAlbumArtImage.Opacity(0.0);
        }
        if (g_compactAlbumArtFade) {
            g_compactAlbumArtFade.Opacity(g_compactAlbumArtFade.Source() != nullptr ? 1.0 : 0.0);
        }
    } else {
        if (g_compactAlbumArtImage) g_compactAlbumArtImage.Opacity(1.0);
        if (g_compactAlbumArtFade) g_compactAlbumArtFade.Opacity(0.0);
    }

    if (!g_compactTextRenderingHooked) {
        g_compactTextRenderingToken =
            mediax::CompositionTarget::Rendering(OnCompactTextRendering);
        g_compactTextRenderingHooked = true;
    }
}

void SetCompactIslandSuppressed(bool suppressed);
void StartPopupXamlRenderLoop();
void StopHoverRenderLoop();
void ApplyExpandedState();
void UpdatePlayerContents();

void UpdatePopupSeekPreview(double ratio) {
    MediaState state = SnapshotMedia();
    if (!state.hasSession || state.durationTicks <= 0 || !g_popupXamlProgress) {
        g_popupSeekDragging = false;
        return;
    }

    ratio = Clamp(ratio, 0.0, 1.0);
    g_popupSeekDragging = true;
    g_popupSeekPreviewRatio = ratio;
    g_popupLiveProgressValue = ratio * g_popupXamlProgress.Maximum();
    g_popupXamlProgress.Value(g_popupLiveProgressValue);

    if (g_popupXamlProgressTrack && g_popupXamlProgressFill) {
        double width = g_popupXamlProgressTrack.ActualWidth();
        if (width <= 0.0) {
            width = g_popupXamlProgressTrack.Width();
        }
        double fillWidth = std::max(0.0, width * ratio);
        g_popupXamlProgressFill.Width(fillWidth);
        g_popupXamlProgressFill.Visibility(
            fillWidth > 0.5 ? Visibility::Visible : Visibility::Collapsed);
    }

    int64_t previewTicks = static_cast<int64_t>(
        std::llround(static_cast<double>(state.durationTicks) * ratio));
    if (g_popupXamlElapsed) {
        g_popupXamlElapsed.Text(FormatMediaTime(previewTicks));
    }
    if (g_popupXamlDuration) {
        g_popupXamlDuration.Text(FormatMediaTime(state.durationTicks));
    }
}

void UpdatePopupLiveProgressFromSnapshot() {
    if (!g_popupXamlProgress) {
        return;
    }

    auto now = std::chrono::steady_clock::now();
    g_popupLiveProgressFrameTime = now;

    std::chrono::steady_clock::time_point timestamp;
    MediaState state = SnapshotMediaWithTimestamp(timestamp);
    if (!state.hasSession || state.durationTicks <= 0) {
        g_popupSeekDragging = false;
        g_popupSeekPreviewUntil = {};
        {
            std::lock_guard seekLock(g_seekMutex);
            g_popupSeekCommitPending = false;
            g_popupSeekCommitTargetTicks = 0;
            g_popupSeekCommitUntil = {};
        }
        g_popupLiveProgressValue = 0.0;
        g_popupLiveProgressValid = false;
        g_popupLiveProgressKey.clear();
        g_popupXamlProgress.Value(0.0);
        if (g_popupXamlElapsed) {
            g_popupXamlElapsed.Text(L"0:00");
        }
        return;
    }

    std::wstring key = state.title + L"\n" + state.artist + L"\n" +
                       std::to_wstring(state.durationTicks);
    bool trackChanged = !g_popupLiveProgressValid || key != g_popupLiveProgressKey;
    if (trackChanged) {
        g_popupSeekDragging = false;
        g_popupSeekPreviewUntil = {};
        {
            std::lock_guard seekLock(g_seekMutex);
            g_popupSeekCommitPending = false;
            g_popupSeekCommitTargetTicks = 0;
            g_popupSeekCommitUntil = {};
        }
        g_popupLiveProgressKey = key;
        g_popupLiveProgressValid = true;
    }

    {
        std::lock_guard seekLock(g_seekMutex);
        if (g_popupSeekCommitPending) {
            if (now < g_popupSeekCommitUntil) {
                int64_t targetTicks = std::clamp<int64_t>(
                    g_popupSeekCommitTargetTicks, 0, state.durationTicks);
                g_popupLiveProgressValue = Clamp(
                    static_cast<double>(targetTicks) /
                        static_cast<double>(state.durationTicks) * 1000.0,
                    0.0, 1000.0);
                g_popupXamlProgress.Value(g_popupLiveProgressValue);
                if (g_popupXamlElapsed) {
                    g_popupXamlElapsed.Text(FormatMediaTime(targetTicks));
                }
                if (g_popupXamlDuration) {
                    g_popupXamlDuration.Text(FormatMediaTime(state.durationTicks));
                }
                return;
            }
            g_popupSeekCommitPending = false;
            g_popupSeekCommitTargetTicks = 0;
        }
    }

    if (g_popupSeekDragging || now < g_popupSeekPreviewUntil) {
        return;
    }

    int64_t livePositionTicks = state.positionTicks;
    if (state.isPlaying && timestamp.time_since_epoch().count() != 0) {
        double ageSec = std::chrono::duration_cast<std::chrono::milliseconds>(
                            now - timestamp)
                            .count() /
                        1000.0;
        // Keep progress moving between event-driven timeline updates. The media
        // worker performs a 30-second fallback refresh if a provider goes quiet.
        ageSec = Clamp(ageSec, 0.0, 30.0);
        livePositionTicks += static_cast<int64_t>(ageSec * 10000000.0);
    }

    livePositionTicks = std::clamp<int64_t>(livePositionTicks, 0, state.durationTicks);
    g_popupLiveProgressValue = Clamp(static_cast<double>(livePositionTicks) /
                                         static_cast<double>(state.durationTicks) * 1000.0,
                                     0.0, 1000.0);

    g_popupXamlProgress.Value(g_popupLiveProgressValue);
    if (g_popupXamlElapsed) {
        g_popupXamlElapsed.Text(FormatMediaTime(livePositionTicks));
    }
    if (g_popupXamlDuration) {
        g_popupXamlDuration.Text(FormatMediaTime(state.durationTicks));
    }
}

void EndPopupSeek(bool commit) {
    if (!g_popupSeekDragging) {
        return;
    }

    double ratio = g_popupSeekPreviewRatio;
    g_popupSeekDragging = false;
    if (commit) {
        auto now = std::chrono::steady_clock::now();
        MediaState state = SnapshotMedia();
        int64_t targetTicks = state.durationTicks > 0
                                  ? static_cast<int64_t>(std::llround(
                                        static_cast<double>(state.durationTicks) *
                                        Clamp(ratio, 0.0, 1.0)))
                                  : 0;
        g_popupSeekPreviewUntil = now + std::chrono::milliseconds(2600);
        double commitRatio = Clamp(ratio, 0.0, 1.0);
        {
            std::lock_guard seekLock(g_seekMutex);
            g_popupSeekCommitPending = true;
            g_popupSeekCommitRatio = commitRatio;
            g_popupSeekCommitTargetTicks = targetTicks;
            g_popupSeekCommitUntil = g_popupSeekPreviewUntil;
        }
        {
            std::lock_guard lock(g_mediaMutex);
            if (g_media.hasSession && g_media.durationTicks > 0) {
                g_media.positionTicks = std::clamp<int64_t>(
                    targetTicks, 0, g_media.durationTicks);
                g_mediaStateTimestamp = now;
            }
        }

        // Queue the provider seek before touching XAML. UI elements can become
        // temporarily invalid during popup relayout; that must never suppress
        // the actual playback-position change.
        SeekToMediaPosition(ratio);
        try {
            if (g_popupXamlProgress) {
                g_popupLiveProgressValue =
                    commitRatio * g_popupXamlProgress.Maximum();
                g_popupXamlProgress.Value(g_popupLiveProgressValue);
            }
            if (g_popupXamlElapsed && state.durationTicks > 0) {
                g_popupXamlElapsed.Text(FormatMediaTime(targetTicks));
            }
            StartPopupXamlRenderLoop();
        } catch (...) {
        }
    } else {
        g_popupSeekPreviewUntil = {};
        {
            std::lock_guard seekLock(g_seekMutex);
            g_popupSeekCommitPending = false;
            g_popupSeekCommitTargetTicks = 0;
            g_popupSeekCommitUntil = {};
        }
        UpdatePopupLiveProgressFromSnapshot();
    }
}

void BeginCloseExpandedPopup() {
    if (!g_expandedPopup || !IsWindowVisible(g_expandedPopup)) {
        if (g_expandedPopup) {
            ParkExpandedPopupWindow(g_expandedPopup);
        }
        g_expanded = false;
        SetCompactIslandSuppressed(false);
        return;
    }
    try {
        if (g_playerGrid) {
            ApplyExpandedState();
            UpdatePlayerContents();
            g_playerGrid.UpdateLayout();
            g_playerGrid.Opacity(0.0);
            g_playerGrid.IsHitTestVisible(false);
        }
        g_popupTextTransitionActive = false;
        if (g_popupXamlOutgoingTitle) g_popupXamlOutgoingTitle.Opacity(0.0);
        if (g_popupXamlOutgoingArtist) g_popupXamlOutgoingArtist.Opacity(0.0);
    } catch (...) {
    }
    g_popupClosing = true;
    SetPopupWindowClickThrough(g_expandedPopup, true);
    SetPopupWindowClickThrough(g_popupXamlChild, true);
    g_popupOutsideClickArmed = false;
    g_popupAnimationTarget = 0.0;
    if (g_popupXamlRoot) {
        StartPopupXamlRenderLoop();
    }
}

void SetCompactIslandSuppressed(bool suppressed) {
    if (!g_playerGrid) {
        return;
    }

    if (suppressed) {
        StopCompactTextRenderLoop();
        StopCompactProgressRenderLoop();
        ResetCompactTextAnimationVisuals();
    }

    if (!suppressed) {
        StopHoverRenderLoop();
        g_currentHoverScale = 1.0;
        g_targetHoverScale = 1.0;
        if (g_islandScale) {
            g_islandScale.ScaleX(1.0);
            g_islandScale.ScaleY(1.0);
        }
        ApplyExpandedState();
        UpdatePlayerContents();
        try {
            g_playerGrid.UpdateLayout();
        } catch (...) {
        }
    }

    g_playerGrid.Opacity(suppressed ? 0.0 : 1.0);
    g_playerGrid.IsHitTestVisible(!suppressed);
}

void StopPopupXamlRenderLoop() {
    if (!g_popupRenderingHooked) {
        return;
    }
    try {
        mediax::CompositionTarget::Rendering(g_popupRenderingToken);
    } catch (...) {
    }
    g_popupRenderingHooked = false;
    g_lastPopupFrameTime = {};
}

void FinishCloseExpandedPopup(HWND hwnd) {
    StopPopupXamlRenderLoop();
    g_expanded = false;
    try {
        StopHoverRenderLoop();
        g_currentHoverScale = 1.0;
        g_targetHoverScale = 1.0;
        if (g_islandScale) {
            g_islandScale.ScaleX(1.0);
            g_islandScale.ScaleY(1.0);
        }
        ApplyExpandedState();
        if (g_playerGrid) {
            g_playerGrid.Opacity(1.0);
            g_playerGrid.IsHitTestVisible(true);
        }
    } catch (...) {
        Wh_Log(L"Island: failed to restore compact island after closing popup");
        try {
            if (g_playerGrid) {
                g_playerGrid.Opacity(1.0);
                g_playerGrid.IsHitTestVisible(true);
            }
        } catch (...) {
        }
    }
    if (hwnd) {
        ParkExpandedPopupWindow(hwnd);
        KillTimer(hwnd, kPopupTimerId);
    }
    HidePopupBackdropOverlayWindowVisualOnly();
    g_popupAnimationProgress = 0.0;
    g_popupAnimationTarget = 0.0;
    g_lastPopupVisualProgress = -1.0;
    g_lastPopupVisualRect = {LONG_MIN, LONG_MIN, LONG_MIN, LONG_MIN};
    g_lastPopupWindowRect = {LONG_MIN, LONG_MIN, LONG_MIN, LONG_MIN};
    g_popupHostRect = {};
    g_popupClosing = false;
    g_popupOutsideClickArmed = false;
    g_popupLiveProgressValue = 0.0;
    g_popupLiveProgressValid = false;
    g_popupLiveProgressKey.clear();
    g_popupLiveProgressFrameTime = {};
    g_popupSeekDragging = false;
    g_popupSeekPreviewRatio = 0.0;
    g_popupSeekPreviewUntil = {};
    {
        std::lock_guard seekLock(g_seekMutex);
        g_popupSeekCommitPending = false;
        g_popupSeekCommitRatio = 0.0;
        g_popupSeekCommitTargetTicks = 0;
        g_popupSeekCommitUntil = {};
    }
}

void OnPopupXamlRendering(winrt::Windows::Foundation::IInspectable const&,
                          winrt::Windows::Foundation::IInspectable const&) {
    try {
    if (!g_popupXamlRoot || !g_expandedPopup || g_unloading) {
        StopPopupXamlRenderLoop();
        return;
    }

    auto now = std::chrono::steady_clock::now();
    double dt = 1.0 / 60.0;
    if (g_lastPopupFrameTime.time_since_epoch().count() != 0) {
        dt = std::chrono::duration_cast<std::chrono::microseconds>(
                 now - g_lastPopupFrameTime).count() / 1000000.0;
    }
    g_lastPopupFrameTime = now;
    dt = Clamp(dt, 0.0, 0.05);
    TickPopupMediaTransition(dt);
    double animationDt = dt * g_settings.animationSpeed;
    double alpha = 1.0 - std::exp(-14.0 * animationDt);
    g_popupAnimationProgress +=
        (g_popupAnimationTarget - g_popupAnimationProgress) * alpha;

    if (g_popupClosing && g_popupAnimationTarget == 0.0 &&
        g_popupAnimationProgress < 0.006) {
        FinishCloseExpandedPopup(g_expandedPopup);
        return;
    } else if (std::abs(g_popupAnimationTarget - g_popupAnimationProgress) < 0.006) {
        g_popupAnimationProgress = g_popupAnimationTarget;
    }
    PositionExpandedPopup();
    UpdatePopupLiveProgressFromSnapshot();
    RenderExpandedPopupLayer();

    if (g_popupAnimationProgress == g_popupAnimationTarget) {
        if (g_popupClosing && g_popupAnimationProgress == 0.0) {
            FinishCloseExpandedPopup(g_expandedPopup);
            return;
        }
        if (g_popupMediaTransitionActive) {
            return;
        }
        StopPopupXamlRenderLoop();
        if (g_popupPendingContentRefresh) {
            g_popupPendingContentRefresh = false;
            UpdatePlayerContents();
        }
    }
    } catch (...) {
        Wh_Log(L"Island: exception in popup rendering callback");
        FinishCloseExpandedPopup(g_expandedPopup);
    }
}

void StartPopupXamlRenderLoop() {
    if (!g_popupXamlRoot || g_popupRenderingHooked) {
        return;
    }
    g_lastPopupFrameTime = std::chrono::steady_clock::now();
    g_popupRenderingToken = mediax::CompositionTarget::Rendering(OnPopupXamlRendering);
    g_popupRenderingHooked = true;
}

LRESULT CALLBACK ExpandedPopupWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    try {
    switch (message) {
        case WM_ERASEBKGND:
            return 1;
        case WM_NCHITTEST:
            return PopupExpandedHitTest(lParam);
        case WM_PAINT: {
            PAINTSTRUCT paint{};
            BeginPaint(hwnd, &paint);
            EndPaint(hwnd, &paint);
            return 0;
        }
        case WM_LBUTTONUP: {
            if (PopupProgress() < 0.75) {
                return 0;
            }
            RECT client{};
            GetClientRect(hwnd, &client);
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            RECT art{};
            RECT card{};
            RECT title{};
            RECT artist{};
            RECT progress{};
            RECT elapsed{};
            RECT duration{};
            RECT controls{};
            CalculatePopupFinalLayout(client, art, card, title, artist, progress,
                                      elapsed, duration, controls);
            RECT progressHitTarget = progress;
            InflateRect(&progressHitTarget, 0, 8);
            if (PtInRect(&progressHitTarget, point) && progress.right > progress.left) {
                SeekToMediaPosition(
                    static_cast<double>(point.x - progress.left) /
                    static_cast<double>(progress.right - progress.left));
                return 0;
            }
            for (int i = 0; i < 3; ++i) {
                RECT button = PopupButtonRect(i, client.right, client.bottom);
                if (!PtInRect(&button, point)) {
                    continue;
                }
                if (i == 0) {
                    RunMediaCommand([](gsm::GlobalSystemMediaTransportControlsSession const& session) {
                        NoteMediaNavigationDirection(-1);
                        GetAsyncResultWithTimeout(session.TrySkipPreviousAsync(),
                                                   kMediaCommandAsyncTimeout,
                                                   L"TrySkipPreviousAsync");
                    });
                } else if (i == 1) {
                    RunMediaCommand([](gsm::GlobalSystemMediaTransportControlsSession const& session) {
                        GetAsyncResultWithTimeout(session.TryTogglePlayPauseAsync(),
                                                   kMediaCommandAsyncTimeout,
                                                   L"TryTogglePlayPauseAsync");
                    });
                } else {
                    RunMediaCommand([](gsm::GlobalSystemMediaTransportControlsSession const& session) {
                        NoteMediaNavigationDirection(1);
                        GetAsyncResultWithTimeout(session.TrySkipNextAsync(),
                                                   kMediaCommandAsyncTimeout,
                                                   L"TrySkipNextAsync");
                    });
                }
                return 0;
            }
            return 0;
        }
        case WM_TIMER: {
            if (!g_popupSeekDragging) {
                bool leftDown = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
                if (!leftDown) {
                    g_popupOutsideClickArmed = true;
                } else if (g_popupOutsideClickArmed) {
                    POINT cursor{};
                    RECT popupRect{};
                    GetCursorPos(&cursor);
                    if (g_popupXamlRoot) {
                        popupRect = CurrentPopupSurfaceScreenRect();
                        InflateRect(&popupRect, 12, 12);
                    } else {
                        GetWindowRect(hwnd, &popupRect);
                    }
                    if (!PtInRect(&popupRect, cursor)) {
                        BeginCloseExpandedPopup();
                    }
                }
            }



            if (g_popupXamlRoot) {
                UpdatePopupLiveProgressFromSnapshot();
                RenderExpandedPopupLayer();
                return 0;
            }

            double fallbackAlpha = 1.0 - std::pow(0.80, g_settings.animationSpeed);
            fallbackAlpha = Clamp(fallbackAlpha, 0.01, 1.0);
            g_popupAnimationProgress +=
                (g_popupAnimationTarget - g_popupAnimationProgress) * fallbackAlpha;
            if (g_popupClosing && g_popupAnimationTarget == 0.0 &&
                g_popupAnimationProgress < 0.006) {
                FinishCloseExpandedPopup(hwnd);
                return 0;
            } else if (std::abs(g_popupAnimationTarget - g_popupAnimationProgress) < 0.006) {
                g_popupAnimationProgress = g_popupAnimationTarget;
            }
            if (g_popupAnimationProgress == g_popupAnimationTarget) {
                if (g_popupClosing && g_popupAnimationProgress == 0.0) {
                    FinishCloseExpandedPopup(hwnd);
                    return 0;
                }
            }
            PositionExpandedPopup();
            RenderExpandedPopupLayer();
            return 0;
        }
        case WM_DPICHANGED: {
            auto suggestedRect = reinterpret_cast<RECT const*>(lParam);
            if (suggestedRect) {
                SetWindowPos(hwnd,
                             nullptr,
                             suggestedRect->left,
                             suggestedRect->top,
                             suggestedRect->right - suggestedRect->left,
                             suggestedRect->bottom - suggestedRect->top,
                             SWP_NOACTIVATE | SWP_NOOWNERZORDER |
                                 SWP_NOZORDER);
            }
            if (g_expandedPopup == hwnd) {
                PositionExpandedPopup();
                RenderExpandedPopupLayer();
            }
            return 0;
        }
        case WM_SETTINGCHANGE:
        case WM_THEMECHANGED:
            g_popupXamlThemeValid = false;
            ApplyPopupBackdrop(hwnd);
            ApplyPopupXamlTheme(true);
            RenderExpandedPopupLayer();
            return 0;
        case WM_DESTROY:
            StopPopupXamlRenderLoop();
            KillTimer(hwnd, kPopupTimerId);
            if (g_expandedPopup == hwnd) {
                g_expandedPopup = nullptr;
            }
            return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
    } catch (...) {
        Wh_Log(L"Island: exception in expanded popup window callback");
        FinishCloseExpandedPopup(hwnd);
        return 0;
    }
}

HINSTANCE ModInstance() {
    static HINSTANCE instance = [] {
        HMODULE module = nullptr;
        static int moduleAnchor = 0;
        if (!GetModuleHandleExW(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                reinterpret_cast<LPCWSTR>(&moduleAnchor), &module)) {
            return static_cast<HINSTANCE>(nullptr);
        }
        return reinterpret_cast<HINSTANCE>(module);
    }();
    return instance;
}

BYTE PopupRoundedRectAlpha(int x, int y, int width, int height, int radius);


bool CreatePopupOverlayWgcCaptureItemForMonitor(HMONITOR monitor,
                                                capture::GraphicsCaptureItem& item) {
    item = nullptr;
    g_popupOverlayWgcCreateItemHr = S_OK;
    if (!monitor) {
        g_popupOverlayWgcCreateItemHr = E_INVALIDARG;
        Wh_Log(L"Island: overlay WGC CreateForMonitor skipped: null monitor");
        return false;
    }

    try {
        auto activationFactory =
            winrt::get_activation_factory<capture::GraphicsCaptureItem>();

        winrt::com_ptr<IGraphicsCaptureItemInterop> interopFactory;
        HRESULT hr = reinterpret_cast<IUnknown*>(
                         winrt::get_abi(activationFactory))
                         ->QueryInterface(
                             kIID_IGraphicsCaptureItemInterop,
                             interopFactory.put_void());
        if (FAILED(hr) || !interopFactory) {
            g_popupOverlayWgcCreateItemHr = hr;
            Wh_Log(L"Island: overlay WGC QI IGraphicsCaptureItemInterop failed hr=0x%08X",
                   static_cast<unsigned>(hr));
            return false;
        }

        void* rawItem = nullptr;
        hr = interopFactory->CreateForMonitor(
            monitor,
            kIID_IGraphicsCaptureItem,
            &rawItem);
        g_popupOverlayWgcCreateItemHr = hr;
        if (FAILED(hr)) {
            Wh_Log(L"Island: overlay WGC CreateForMonitor failed hr=0x%08X monitor=%p",
                   static_cast<unsigned>(hr),
                   monitor);
            return false;
        }
        if (!rawItem) {
            g_popupOverlayWgcCreateItemHr = E_POINTER;
            Wh_Log(L"Island: overlay WGC CreateForMonitor returned null item hr=0x%08X monitor=%p",
                   static_cast<unsigned>(hr),
                   monitor);
            return false;
        }

        item = capture::GraphicsCaptureItem{
            rawItem,
            winrt::take_ownership_from_abi};
        Wh_Log(L"Island: overlay WGC CreateForMonitor succeeded monitor=%p", monitor);
        g_popupOverlayWgcCreateItemFailed = false;
        return true;
    } catch (winrt::hresult_error const& error) {
        g_popupOverlayWgcCreateItemHr = error.code().value;
        Wh_Log(L"Island: overlay WGC CreateForMonitor exception hr=0x%08X",
               static_cast<unsigned>(g_popupOverlayWgcCreateItemHr));
        return false;
    } catch (...) {
        g_popupOverlayWgcCreateItemHr = E_FAIL;
        Wh_Log(L"Island: overlay WGC CreateForMonitor unknown exception");
        return false;
    }
}

void ClearPopupOverlayWgcLensDisplacementMap() {
    g_popupOverlayWgcLensDisplacementMap = nullptr;
    g_popupOverlayWgcLensMapWidth = 0;
    g_popupOverlayWgcLensMapHeight = 0;
    g_popupOverlayWgcLensMapRadius = 0;
}

void ClearPopupOverlayWgcRenderCache() {
    g_popupOverlayWgcIntermediateBitmap = nullptr;
    g_popupOverlayWgcBlurEffect = nullptr;
    g_popupOverlayWgcRefractionBlurEffect = nullptr;
    g_popupOverlayWgcDisplacementEffect = nullptr;
    g_popupOverlayWgcRimBrush = nullptr;
    g_popupOverlayWgcRoundedGeometry = nullptr;
    g_popupOverlayWgcRenderCacheWidth = 0;
    g_popupOverlayWgcRenderCacheHeight = 0;
    g_popupOverlayWgcRoundedGeometryWidth = 0;
    g_popupOverlayWgcRoundedGeometryHeight = 0;
    g_popupOverlayWgcRoundedGeometryRadius = 0;
}

ID2D1RoundedRectangleGeometry* EnsurePopupOverlayWgcRoundedGeometry(
    D2D1_RECT_F const& rect,
    int width,
    int height,
    int radius) {
    if (!g_popupOverlayWgcD2dFactory || width <= 0 || height <= 0 ||
        radius <= 0) {
        return nullptr;
    }
    if (g_popupOverlayWgcRoundedGeometry &&
        g_popupOverlayWgcRoundedGeometryWidth == width &&
        g_popupOverlayWgcRoundedGeometryHeight == height &&
        g_popupOverlayWgcRoundedGeometryRadius == radius) {
        return g_popupOverlayWgcRoundedGeometry.get();
    }

    D2D1_ROUNDED_RECT roundedRect = D2D1::RoundedRect(
        rect,
        static_cast<float>(radius),
        static_cast<float>(radius));
    winrt::com_ptr<ID2D1RoundedRectangleGeometry> geometry;
    HRESULT hr = g_popupOverlayWgcD2dFactory->CreateRoundedRectangleGeometry(
        &roundedRect,
        geometry.put());
    if (FAILED(hr) || !geometry) {
        g_popupOverlayWgcLastHr = FAILED(hr) ? hr : E_FAIL;
        return nullptr;
    }

    g_popupOverlayWgcRoundedGeometry = std::move(geometry);
    g_popupOverlayWgcRoundedGeometryWidth = width;
    g_popupOverlayWgcRoundedGeometryHeight = height;
    g_popupOverlayWgcRoundedGeometryRadius = radius;
    return g_popupOverlayWgcRoundedGeometry.get();
}

bool EnsurePopupOverlayWgcRenderCache(int width,
                                      int height,
                                      D2D1_BITMAP_PROPERTIES1 const& props,
                                      bool useLiquidLensWarp) {
    if (!g_popupOverlayWgcD2dContext || width <= 0 || height <= 0) {
        return false;
    }

    if (g_popupOverlayWgcRenderCacheWidth != width ||
        g_popupOverlayWgcRenderCacheHeight != height) {
        g_popupOverlayWgcIntermediateBitmap = nullptr;
        g_popupOverlayWgcRenderCacheWidth = 0;
        g_popupOverlayWgcRenderCacheHeight = 0;
    }

    auto createTargetBitmap = [&](winrt::com_ptr<ID2D1Bitmap1>& bitmap) {
        if (bitmap) {
            return true;
        }
        HRESULT hr = g_popupOverlayWgcD2dContext->CreateBitmap(
            D2D1::SizeU(static_cast<UINT32>(width),
                        static_cast<UINT32>(height)),
            nullptr,
            0,
            &props,
            bitmap.put());
        if (FAILED(hr)) {
            g_popupOverlayWgcLastHr = hr;
            return false;
        }
        return true;
    };

    if (!createTargetBitmap(g_popupOverlayWgcIntermediateBitmap)) {
        ClearPopupOverlayWgcRenderCache();
        return false;
    }

    if (!g_popupOverlayWgcBlurEffect) {
        HRESULT hr = g_popupOverlayWgcD2dContext->CreateEffect(
            CLSID_D2D1GaussianBlur,
            g_popupOverlayWgcBlurEffect.put());
        if (FAILED(hr) || !g_popupOverlayWgcBlurEffect) {
            g_popupOverlayWgcLastHr = FAILED(hr) ? hr : E_FAIL;
            ClearPopupOverlayWgcRenderCache();
            return false;
        }
        g_popupOverlayWgcBlurEffect->SetValue(
            D2D1_GAUSSIANBLUR_PROP_BORDER_MODE,
            D2D1_BORDER_MODE_HARD);
    }

    if (useLiquidLensWarp && !g_popupOverlayWgcRefractionBlurEffect) {
        HRESULT hr = g_popupOverlayWgcD2dContext->CreateEffect(
            CLSID_D2D1GaussianBlur,
            g_popupOverlayWgcRefractionBlurEffect.put());
        if (FAILED(hr) || !g_popupOverlayWgcRefractionBlurEffect) {
            g_popupOverlayWgcLastHr = FAILED(hr) ? hr : E_FAIL;
            ClearPopupOverlayWgcRenderCache();
            return false;
        }
        g_popupOverlayWgcRefractionBlurEffect->SetValue(
            D2D1_GAUSSIANBLUR_PROP_BORDER_MODE,
            D2D1_BORDER_MODE_HARD);
    }

    g_popupOverlayWgcRenderCacheWidth = width;
    g_popupOverlayWgcRenderCacheHeight = height;
    return true;
}

bool EnsurePopupOverlayWgcLensDisplacementMap(int width, int height, int radius) {
    if (!g_popupOverlayWgcD2dContext || width <= 0 || height <= 0) {
        return false;
    }

    radius = Clamp(radius, 1, std::max(1, std::min(width, height) / 2));
    if (g_popupOverlayWgcLensDisplacementMap &&
        g_popupOverlayWgcLensMapWidth == width &&
        g_popupOverlayWgcLensMapHeight == height &&
        g_popupOverlayWgcLensMapRadius == radius) {
        return true;
    }

    std::vector<BYTE> pixels(static_cast<size_t>(width) * height * 4);
    double widthD = static_cast<double>(width);
    double heightD = static_cast<double>(height);
    double minD = std::min(widthD, heightD);
    double radiusD = static_cast<double>(radius);
    double halfWidth = widthD * 0.5;
    double halfHeight = heightD * 0.5;
    double innerHalfWidth = std::max(0.0, halfWidth - radiusD);
    double innerHalfHeight = std::max(0.0, halfHeight - radiusD);
    double centerX = widthD * 0.5;
    double centerY = heightD * 0.5;

    // CodePen-style RG displacement texture. The texture is neutral gray in
    // the calm center (R=G=128), then the red/green channels follow a single
    // continuous tangent field around the rounded shell. A narrow high-index
    // rim is blended with a wider shoulder so the refraction is strongest on
    // the edge and fades inward toward neutral.
    double rimWidth = Clamp(minD * 0.020, 1.8, 4.0);
    double shoulderWidth = Clamp(minD * 0.072, 6.0, 20.0);
    double bodyWidth = Clamp(minD * 0.300, 28.0, 96.0);

    auto smootherStep = [](double value) {
        value = Clamp(value, 0.0, 1.0);
        return value * value * value *
               (value * (value * 6.0 - 15.0) + 10.0);
    };
    auto smoothMax = [](double a, double b, double smoothness) {
        double h = Clamp(0.5 + 0.5 * (a - b) / smoothness, 0.0, 1.0);
        return a * h + b * (1.0 - h) + smoothness * h * (1.0 - h);
    };
    auto fastThenSlowQuadraticFalloff = [](double distance,
                                      double widthValue,
                                      double holdRatio) {
        double t = Clamp(distance / std::max(1.0, widthValue), 0.0, 1.0);
        holdRatio = Clamp(holdRatio, 0.0, 0.92);
        double u = t <= holdRatio
                       ? t / std::max(0.001, holdRatio)
                       : 1.0;
        double fastDrop = 1.0 - (1.0 - holdRatio) *
                                    (1.0 - (1.0 - u) * (1.0 - u));
        if (t <= holdRatio) {
            return Clamp(fastDrop, 0.0, 1.0);
        }

        double tail = (t - holdRatio) / std::max(0.001, 1.0 - holdRatio);
        return Clamp(fastDrop * (1.0 - tail * tail * 0.42), 0.0, 1.0);
    };

    auto signedDistanceAt = [&](double px, double py) {
        double localX = px - centerX;
        double localY = py - centerY;
        double qx = std::abs(localX) - innerHalfWidth;
        double qy = std::abs(localY) - innerHalfHeight;
        double outsideX = std::max(qx, 0.0);
        double outsideY = std::max(qy, 0.0);
        double outsideLength = std::sqrt(outsideX * outsideX +
                                         outsideY * outsideY);
        double hardMax = std::max(qx, qy);
        double smoothness = Clamp(minD * 0.018, 2.8, 7.0);
        double innerTerm =
            hardMax < 0.0 ? smoothMax(qx, qy, smoothness) : hardMax;
        return outsideLength + std::min(innerTerm, 0.0) - radiusD;
    };

    auto profileAtDistance = [&](double distance) {
        if (distance < 0.0) {
            return 0.0;
        }

        double rim = 1.0 - smootherStep(distance / std::max(1.0, rimWidth));
        double shoulder = fastThenSlowQuadraticFalloff(distance,
                                                  shoulderWidth,
                                                  0.36);
        double body = fastThenSlowQuadraticFalloff(distance,
                                              bodyWidth,
                                              0.24);
        rim = std::pow(Clamp(rim, 0.0, 1.0), 0.58);
        shoulder = std::pow(Clamp(shoulder, 0.0, 1.0), 1.18);
        body = std::pow(Clamp(body, 0.0, 1.0), 3.35);
        return Clamp(rim * 0.72 + shoulder * 0.24 + body * 0.04,
                     0.0,
                     1.0);
    };

    for (int y = 0; y < height; ++y) {
        double fy = static_cast<double>(y) + 0.5;
        for (int x = 0; x < width; ++x) {
            double fx = static_cast<double>(x) + 0.5;
            double signedDistance = signedDistanceAt(fx, fy);
            double distance = -signedDistance;
            double offsetX = 0.0;
            double offsetY = 0.0;

            if (distance >= 0.0) {
                constexpr double kGradientStep = 0.75;
                double gradX = signedDistanceAt(fx + kGradientStep, fy) -
                               signedDistanceAt(fx - kGradientStep, fy);
                double gradY = signedDistanceAt(fx, fy + kGradientStep) -
                               signedDistanceAt(fx, fy - kGradientStep);
                double gradLength = std::sqrt(gradX * gradX + gradY * gradY);
                if (gradLength > 0.0001) {
                    double outwardX = gradX / gradLength;
                    double outwardY = gradY / gradLength;

                    // The displacement direction now follows the tangent of
                    // the rounded-rectangle edge curve instead of pointing
                    // inward. The SDF gradient gives a continuous outward
                    // normal; rotating it -90 degrees gives the reverse
                    // tangent vector field around the component.
                    double tangentX = outwardY;
                    double tangentY = -outwardX;
                    double tangentLength = std::sqrt(tangentX * tangentX +
                                                     tangentY * tangentY);
                    if (tangentLength > 0.0001) {
                        tangentX /= tangentLength;
                        tangentY /= tangentLength;
                    }

                    double amount = profileAtDistance(distance);
                    offsetX = tangentX * amount;
                    offsetY = tangentY * amount;
                }
            }

            size_t index = (static_cast<size_t>(y) * width + x) * 4;
            pixels[index + 0] = 0x80;
            pixels[index + 1] = static_cast<BYTE>(
                Clamp(static_cast<int>(std::lround(128.0 + offsetY * 127.0)),
                      0,
                      255));
            pixels[index + 2] = static_cast<BYTE>(
                Clamp(static_cast<int>(std::lround(128.0 + offsetX * 127.0)),
                      0,
                      255));
            pixels[index + 3] = 0xFF;
        }
    }


    D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_NONE,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_IGNORE));
    winrt::com_ptr<ID2D1Bitmap1> map;
    HRESULT hr = g_popupOverlayWgcD2dContext->CreateBitmap(
        D2D1::SizeU(static_cast<UINT32>(width), static_cast<UINT32>(height)),
        pixels.data(),
        static_cast<UINT32>(width * 4),
        &props,
        map.put());
    if (FAILED(hr)) {
        g_popupOverlayWgcLastHr = hr;
        Wh_Log(L"Island: overlay WGC lens displacement map failed hr=0x%08X",
               static_cast<unsigned>(hr));
        ClearPopupOverlayWgcLensDisplacementMap();
        return false;
    }


    g_popupOverlayWgcLensDisplacementMap = std::move(map);
    g_popupOverlayWgcLensMapWidth = width;
    g_popupOverlayWgcLensMapHeight = height;
    g_popupOverlayWgcLensMapRadius = radius;
    return true;
}

bool DrawLiquidGlassGpuLensWarp(ID2D1DeviceContext* context,
                                ID2D1Image* blurredImage,
                                ID2D1Image* refractImage,
                                int width,
                                int height,
                                int radius,
                                float rimAlpha) {
    if (!context || !blurredImage || width <= 2 || height <= 2) {
        return false;
    }
    if (!refractImage) {
        refractImage = blurredImage;
    }

    float widthF = static_cast<float>(width);
    float heightF = static_cast<float>(height);
    D2D1_POINT_2F imageTargetOffset{0.0f, 0.0f};

    // Keep both Gaussian blurs inside one Direct2D effect graph. Direct2D can
    // schedule the complex sampling passes without us writing and reading two
    // additional full-size render targets every captured frame.
    context->DrawImage(blurredImage,
                       &imageTargetOffset,
                       nullptr,
                       D2D1_INTERPOLATION_MODE_LINEAR,
                       D2D1_COMPOSITE_MODE_SOURCE_OVER);

    if (radius <= 0) {
        radius = static_cast<int>(std::lround(
            std::min(std::min(widthF, heightF) * 0.17f, 36.0f)));
    }
    radius = Clamp(radius, 1, std::max(1, std::min(width, height) / 2));
    if (!EnsurePopupOverlayWgcLensDisplacementMap(width, height, radius)) {
        return true;
    }


    if (!g_popupOverlayWgcDisplacementEffect) {
        HRESULT createHr = context->CreateEffect(
            CLSID_D2D1DisplacementMap,
            g_popupOverlayWgcDisplacementEffect.put());
        if (FAILED(createHr) || !g_popupOverlayWgcDisplacementEffect) {
            return true;
        }
        g_popupOverlayWgcDisplacementEffect->SetValue(
            D2D1_DISPLACEMENTMAP_PROP_X_CHANNEL_SELECT,
            D2D1_CHANNEL_SELECTOR_R);
        g_popupOverlayWgcDisplacementEffect->SetValue(
            D2D1_DISPLACEMENTMAP_PROP_Y_CHANNEL_SELECT,
            D2D1_CHANNEL_SELECTOR_G);
    }

    g_popupOverlayWgcDisplacementEffect->SetInput(0, refractImage);
    g_popupOverlayWgcDisplacementEffect->SetInput(1, g_popupOverlayWgcLensDisplacementMap.get());

    float scale =
        static_cast<float>(Clamp(std::min(width, height) * 0.88, 24.0, 78.0));
    HRESULT hr = g_popupOverlayWgcDisplacementEffect->SetValue(D2D1_DISPLACEMENTMAP_PROP_SCALE, scale);
    if (FAILED(hr)) {
        return true;
    }
    // Do not clip the displaced image into four rectangular edge bands. The
    // creates a visibly different "center rectangle" where only the base blur
    // is present. Instead, blend the full-field displacement over the base
    // glass. The displacement map is neutral in the interior, so the whole
    // surface shares one continuous lens field while the rim still pulls from
    // the lower-blur refractive source.
    D2D1_LAYER_PARAMETERS1 layerParams = D2D1::LayerParameters1(
        D2D1::InfiniteRect(),
        nullptr,
        D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
        D2D1::IdentityMatrix(),
        0.58f,
        nullptr,
        D2D1_LAYER_OPTIONS1_NONE);
    context->PushLayer(layerParams, nullptr);
    context->DrawImage(g_popupOverlayWgcDisplacementEffect.get(),
                       &imageTargetOffset,
                       nullptr,
                       D2D1_INTERPOLATION_MODE_LINEAR,
                       D2D1_COMPOSITE_MODE_SOURCE_OVER);
    context->PopLayer();

    float rimInset = 0.75f;
    float rimRadius = std::max(1.0f, static_cast<float>(radius) - rimInset);
    D2D1_ROUNDED_RECT rimRect = D2D1::RoundedRect(
        D2D1::RectF(rimInset, rimInset, widthF - rimInset, heightF - rimInset),
        rimRadius,
        rimRadius);
    rimAlpha = Clamp(rimAlpha, 0.0f, 1.0f);
    D2D1_COLOR_F rimColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, rimAlpha);
    if (!g_popupOverlayWgcRimBrush) {
        context->CreateSolidColorBrush(rimColor,
                                       g_popupOverlayWgcRimBrush.put());
    } else {
        g_popupOverlayWgcRimBrush->SetColor(rimColor);
    }
    if (g_popupOverlayWgcRimBrush) {
        context->DrawRoundedRectangle(rimRect,
                                      g_popupOverlayWgcRimBrush.get(),
                                      1.25f);
    }


    return true;
}

void UpdatePopupOverlayDcompVisualTransformLocked(int widthPx, int heightPx) {
    if (!g_popupOverlayDcompVisual) {
        return;
    }

    float scaleX = 1.0f;
    float scaleY = 1.0f;
    if (g_popupOverlayWgcTargetWidthPx > 0 &&
        g_popupOverlayWgcTargetHeightPx > 0 &&
        widthPx > 0 && heightPx > 0) {
        scaleX = static_cast<float>(Clamp(
            static_cast<double>(widthPx) /
                static_cast<double>(g_popupOverlayWgcTargetWidthPx),
            0.01,
            8.0));
        scaleY = static_cast<float>(Clamp(
            static_cast<double>(heightPx) /
                static_cast<double>(g_popupOverlayWgcTargetHeightPx),
            0.01,
            8.0));
    }

    if (g_popupOverlayDcompTransformValid &&
        std::abs(scaleX - g_popupOverlayDcompLastScaleX) < 0.0005f &&
        std::abs(scaleY - g_popupOverlayDcompLastScaleY) < 0.0005f) {
        return;
    }

    D2D_MATRIX_3X2_F transform = D2D1::Matrix3x2F::Scale(scaleX, scaleY);
    g_popupOverlayDcompVisual->SetTransform(transform);
    g_popupOverlayDcompTransformValid = true;
    g_popupOverlayDcompLastScaleX = scaleX;
    g_popupOverlayDcompLastScaleY = scaleY;
    if (g_popupOverlayDcompDevice) {
        g_popupOverlayDcompDevice->Commit();
    }
}

void UpdatePopupOverlayDcompVisualTransform(int widthPx, int heightPx) {
    std::unique_lock lock(g_popupOverlayWgcMutex, std::try_to_lock);
    if (!lock.owns_lock()) {
        return;
    }

    UpdatePopupOverlayDcompVisualTransformLocked(widthPx, heightPx);
}

bool RecreatePopupOverlayWgcReadbackTextures(int widthPx, int heightPx) {
    if (!g_popupOverlayWgcD3dDevice || !g_popupOverlayWgcDxgiDevice ||
        !g_popupOverlayWgcD2dContext || !g_popupBackdropOverlay ||
        widthPx <= 0 || heightPx <= 0) {
        return false;
    }

    if (g_popupOverlayWgcSwapChain &&
        g_popupOverlayWgcSwapChainTargetBitmap &&
        g_popupOverlayWgcTargetWidthPx == widthPx &&
        g_popupOverlayWgcTargetHeightPx == heightPx) {
        return true;
    }

    // WGC capture, blur, rounded masking, and DirectComposition presentation
    // stay on the GPU without CPU readback or UpdateLayeredWindow.
    g_popupOverlayWgcSwapChainTargetBitmap = nullptr;
    g_popupOverlayWgcSwapChain = nullptr;
    g_popupOverlayDcompContentAttached = false;
    ClearPopupOverlayWgcRenderCache();
    ClearPopupOverlayWgcLensDisplacementMap();

    if (!g_popupOverlayDcompDevice) {
        HRESULT hr = DCompositionCreateDevice(
            g_popupOverlayWgcDxgiDevice.get(),
            __uuidof(IDCompositionDevice),
            g_popupOverlayDcompDevice.put_void());
        if (FAILED(hr)) {
            Wh_Log(L"Island: overlay DComp device failed hr=0x%08X",
                   static_cast<unsigned>(hr));
            g_popupOverlayWgcLastHr = hr;
            return false;
        }
    }

    if (!g_popupOverlayDcompTarget) {
        HRESULT hr = g_popupOverlayDcompDevice->CreateTargetForHwnd(
            g_popupBackdropOverlay, TRUE, g_popupOverlayDcompTarget.put());
        if (FAILED(hr)) {
            Wh_Log(L"Island: overlay DComp target failed hr=0x%08X",
                   static_cast<unsigned>(hr));
            g_popupOverlayWgcLastHr = hr;
            return false;
        }
    }

    if (!g_popupOverlayDcompVisual) {
        HRESULT hr = g_popupOverlayDcompDevice->CreateVisual(
            g_popupOverlayDcompVisual.put());
        if (FAILED(hr)) {
            Wh_Log(L"Island: overlay DComp visual failed hr=0x%08X",
                   static_cast<unsigned>(hr));
            g_popupOverlayWgcLastHr = hr;
            return false;
        }
        g_popupOverlayDcompTarget->SetRoot(g_popupOverlayDcompVisual.get());
    }

    winrt::com_ptr<IDXGIAdapter> adapter;
    HRESULT hr = g_popupOverlayWgcDxgiDevice->GetAdapter(adapter.put());
    if (FAILED(hr)) {
        g_popupOverlayWgcLastHr = hr;
        return false;
    }
    winrt::com_ptr<IDXGIFactory2> factory;
    hr = adapter->GetParent(__uuidof(IDXGIFactory2), factory.put_void());
    if (FAILED(hr)) {
        g_popupOverlayWgcLastHr = hr;
        return false;
    }

    DXGI_SWAP_CHAIN_DESC1 desc{};
    desc.Width = static_cast<UINT>(widthPx);
    desc.Height = static_cast<UINT>(heightPx);
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.Stereo = FALSE;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.Scaling = DXGI_SCALING_STRETCH;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    desc.Flags = 0;

    winrt::com_ptr<IDXGISwapChain1> swapChain;
    hr = factory->CreateSwapChainForComposition(
        g_popupOverlayWgcD3dDevice.get(), &desc, nullptr, swapChain.put());
    if (FAILED(hr)) {
        Wh_Log(L"Island: overlay DComp swapchain failed hr=0x%08X",
               static_cast<unsigned>(hr));
        g_popupOverlayWgcLastHr = hr;
        return false;
    }

    winrt::com_ptr<IDXGISurface> backBufferSurface;
    hr = swapChain->GetBuffer(0, __uuidof(IDXGISurface),
                              backBufferSurface.put_void());
    if (FAILED(hr)) {
        g_popupOverlayWgcLastHr = hr;
        return false;
    }

    D2D1_BITMAP_PROPERTIES1 targetProps = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED));
    winrt::com_ptr<ID2D1Bitmap1> targetBitmap;
    hr = g_popupOverlayWgcD2dContext->CreateBitmapFromDxgiSurface(
        backBufferSurface.get(), &targetProps, targetBitmap.put());
    if (FAILED(hr)) {
        Wh_Log(L"Island: overlay DComp target bitmap failed hr=0x%08X",
               static_cast<unsigned>(hr));
        g_popupOverlayWgcLastHr = hr;
        return false;
    }
    g_popupOverlayDcompContentAttached = false;

    g_popupOverlayWgcSwapChain = std::move(swapChain);
    g_popupOverlayWgcSwapChainTargetBitmap = std::move(targetBitmap);
    g_popupOverlayWgcTargetWidthPx = widthPx;
    g_popupOverlayWgcTargetHeightPx = heightPx;
    g_popupOverlayWgcLastHr = S_OK;
    return true;
}
bool EnsurePopupOverlayWgcDeviceResourcesLocked(HWND hwnd,
                                                int widthPx,
                                                int heightPx) {
    if (!UseOverlayPopupBackdropMaterial() || !hwnd ||
        widthPx <= 0 || heightPx <= 0) {
        return false;
    }

    if (g_popupOverlayWgcD3dDevice &&
        g_popupOverlayWgcSwapChain &&
        g_popupOverlayWgcSwapChainTargetBitmap &&
        g_popupOverlayWgcTargetWidthPx == widthPx &&
        g_popupOverlayWgcTargetHeightPx == heightPx) {
        return true;
    }

    g_popupOverlayWgcSession = nullptr;
    if (g_popupOverlayWgcFramePool && g_popupOverlayWgcFrameCallbackHooked) {
        try {
            g_popupOverlayWgcFramePool.FrameArrived(g_popupOverlayWgcFrameArrivedToken);
        } catch (...) {
        }
        g_popupOverlayWgcFrameCallbackHooked = false;
    }
    g_popupOverlayWgcFramePool = nullptr;
    g_popupOverlayWgcItem = nullptr;
    g_popupOverlayWgcGraphicsDevice = nullptr;
    g_popupOverlayWgcSwapChainTargetBitmap = nullptr;
    g_popupOverlayWgcSwapChain = nullptr;
    g_popupOverlayDcompContentAttached = false;
    if (g_popupOverlayDcompVisual) {
        g_popupOverlayDcompVisual->SetContent(nullptr);
    }
    if (g_popupOverlayDcompTarget) {
        g_popupOverlayDcompTarget->SetRoot(nullptr);
    }
    if (g_popupOverlayDcompDevice) {
        g_popupOverlayDcompDevice->Commit();
    }
    g_popupOverlayDcompVisual = nullptr;
    g_popupOverlayDcompTransformValid = false;
    g_popupOverlayDcompLastScaleX = 0.0f;
    g_popupOverlayDcompLastScaleY = 0.0f;
    g_popupOverlayDcompTarget = nullptr;
    g_popupOverlayDcompDevice = nullptr;
    ClearPopupOverlayWgcRenderCache();
    ClearPopupOverlayWgcLensDisplacementMap();
    g_popupOverlayWgcD2dContext = nullptr;
    g_popupOverlayWgcD2dDevice = nullptr;
    g_popupOverlayWgcD2dFactory = nullptr;
    g_popupOverlayWgcDxgiDevice = nullptr;
    g_popupOverlayWgcD3dContext = nullptr;
    g_popupOverlayWgcD3dDevice = nullptr;

    UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    D3D_FEATURE_LEVEL selectedFeatureLevel{};
    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        deviceFlags,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        g_popupOverlayWgcD3dDevice.put(),
        &selectedFeatureLevel,
        g_popupOverlayWgcD3dContext.put());
    if (FAILED(hr)) {
        hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_WARP,
            nullptr,
            deviceFlags,
            featureLevels,
            ARRAYSIZE(featureLevels),
            D3D11_SDK_VERSION,
            g_popupOverlayWgcD3dDevice.put(),
            &selectedFeatureLevel,
            g_popupOverlayWgcD3dContext.put());
    }
    if (FAILED(hr)) {
        Wh_Log(L"Island: overlay WGC D3D11CreateDevice failed hr=0x%08X",
               static_cast<unsigned>(hr));
        return false;
    }

    g_popupOverlayWgcDxgiDevice = g_popupOverlayWgcD3dDevice.as<IDXGIDevice>();

    D2D1_FACTORY_OPTIONS factoryOptions{};
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED,
                           __uuidof(ID2D1Factory1),
                           &factoryOptions,
                           reinterpret_cast<void**>(g_popupOverlayWgcD2dFactory.put()));
    if (FAILED(hr)) {
        Wh_Log(L"Island: overlay WGC D2D factory failed hr=0x%08X",
               static_cast<unsigned>(hr));
        return false;
    }
    hr = g_popupOverlayWgcD2dFactory->CreateDevice(g_popupOverlayWgcDxgiDevice.get(),
                                                   g_popupOverlayWgcD2dDevice.put());
    if (FAILED(hr)) {
        Wh_Log(L"Island: overlay WGC D2D device failed hr=0x%08X",
               static_cast<unsigned>(hr));
        return false;
    }
    hr = g_popupOverlayWgcD2dDevice->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
        g_popupOverlayWgcD2dContext.put());
    if (FAILED(hr)) {
        Wh_Log(L"Island: overlay WGC D2D context failed hr=0x%08X",
               static_cast<unsigned>(hr));
        return false;
    }

    winrt::com_ptr<IInspectable> graphicsDeviceInspectable;
    hr = CreateDirect3D11DeviceFromDXGIDevice(
        g_popupOverlayWgcDxgiDevice.get(),
        graphicsDeviceInspectable.put());
    if (FAILED(hr)) {
        Wh_Log(L"Island: overlay WGC CreateDirect3D11DeviceFromDXGIDevice failed hr=0x%08X",
               static_cast<unsigned>(hr));
        return false;
    }
    g_popupOverlayWgcGraphicsDevice =
        graphicsDeviceInspectable.as<direct3d11::IDirect3DDevice>();

    winrt::com_ptr<IDXGIAdapter> adapter;
    hr = g_popupOverlayWgcDxgiDevice->GetAdapter(adapter.put());
    if (FAILED(hr)) {
        return false;
    }
    winrt::com_ptr<IDXGIFactory2> factory;
    hr = adapter->GetParent(__uuidof(IDXGIFactory2), factory.put_void());
    if (FAILED(hr)) {
        return false;
    }

    return RecreatePopupOverlayWgcReadbackTextures(widthPx, heightPx);
}

double PopupOverlayWgcFrameIntervalMsForMonitor(HMONITOR monitor) {
    constexpr double kDefaultRefreshHz = 60.0;
    double refreshHz = kDefaultRefreshHz;

    MONITORINFOEXW monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (monitor && GetMonitorInfoW(monitor, &monitorInfo)) {
        DEVMODEW devMode{};
        devMode.dmSize = sizeof(devMode);
        if (EnumDisplaySettingsW(monitorInfo.szDevice,
                                 ENUM_CURRENT_SETTINGS,
                                 &devMode) &&
            devMode.dmDisplayFrequency > 1 &&
            devMode.dmDisplayFrequency < 1000) {
            refreshHz = static_cast<double>(devMode.dmDisplayFrequency);
        }
    }

    // Match the monitor cadence while keeping a conservative floor for VRR or
    // drivers that report unusually high transient values.
    return Clamp(1000.0 / refreshHz, 1000.0 / 360.0, 1000.0 / 50.0);
}


bool PopupOverlayWgcBorderlessAccessAllowed() {
    return g_popupOverlayWgcBorderlessAccessState.load(
               std::memory_order_acquire) == 2;
}

bool PopupOverlayWgcBorderlessAccessPending() {
    int state = g_popupOverlayWgcBorderlessAccessState.load(
        std::memory_order_acquire);
    return state == 0 || state == 1;
}

void RequestPopupOverlayWgcBorderlessAccessAsync() {
    int expected = 0;
    if (!g_popupOverlayWgcBorderlessAccessState.compare_exchange_strong(
            expected, 1, std::memory_order_acq_rel)) {
        return;
    }

    std::lock_guard threadLock(
        g_popupOverlayWgcBorderlessAccessThreadMutex);
    if (g_unloading.load()) {
        g_popupOverlayWgcBorderlessAccessState.store(
            4, std::memory_order_release);
        return;
    }

    try {
        g_popupOverlayWgcBorderlessAccessThread = new std::thread([]() {
            try {
                auto operation =
                    capture::GraphicsCaptureAccess::RequestAccessAsync(
                        capture::GraphicsCaptureAccessKind::Borderless);
                auto status = GetAsyncResultWithTimeout(
                    operation,
                    kPopupOverlayWgcBorderlessAccessTimeout,
                    L"WGC borderless access");
                bool allowed =
                    status ==
                    winrt::Windows::Security::Authorization::
                        AppCapabilityAccess::AppCapabilityAccessStatus::Allowed;
                g_popupOverlayWgcBorderlessAccessState.store(
                    allowed ? 2 : 3, std::memory_order_release);
                if (allowed) {
                    std::lock_guard lock(g_popupOverlayWgcMutex);
                    if (g_popupOverlayWgcSession) {
                        try {
                            g_popupOverlayWgcSession.IsBorderRequired(false);
                        } catch (...) {
                        }
                    }
                }
                Wh_Log(L"Island: overlay WGC borderless access %s",
                       allowed ? L"allowed" : L"denied");
            } catch (winrt::hresult_error const& error) {
                g_popupOverlayWgcBorderlessAccessState.store(
                    4, std::memory_order_release);
                Wh_Log(
                    L"Island: overlay WGC borderless access failed hr=0x%08X",
                    static_cast<unsigned>(error.code().value));
            } catch (...) {
                g_popupOverlayWgcBorderlessAccessState.store(
                    4, std::memory_order_release);
                Wh_Log(
                    L"Island: overlay WGC borderless access failed unknown");
            }
        });
    } catch (...) {
        g_popupOverlayWgcBorderlessAccessState.store(
            4, std::memory_order_release);
        g_popupOverlayWgcBorderlessAccessThread = nullptr;
        Wh_Log(L"Island: failed to start overlay WGC access thread");
    }
}

void StopPopupOverlayWgcBorderlessAccessThread() {
    std::thread* thread = nullptr;
    {
        std::lock_guard threadLock(
            g_popupOverlayWgcBorderlessAccessThreadMutex);
        thread = g_popupOverlayWgcBorderlessAccessThread;
        g_popupOverlayWgcBorderlessAccessThread = nullptr;
    }

    if (!thread) {
        return;
    }

    if (thread->joinable()) {
        thread->join();
    }
    delete thread;
}

bool PopupOverlayWgcDirtyRegionsSupported() {
    try {
        return winrt::Windows::Foundation::Metadata::ApiInformation::
            IsPropertyPresent(
                L"Windows.Graphics.Capture.GraphicsCaptureSession",
                L"DirtyRegionMode");
    } catch (...) {
        return false;
    }
}

bool PopupOverlayWgcFrameTouchesCapture(
    capture::Direct3D11CaptureFrame const& frame,
    RECT const& captureRect,
    RECT const& monitorRect,
    bool morphing) {
    if (morphing ||
        !g_popupOverlayWgcReadbackHadVisibleFrame ||
        !g_popupOverlayWgcDirtyRegionsEnabled.load(
            std::memory_order_acquire)) {
        return true;
    }

    try {
        if (frame.DirtyRegionMode() !=
            capture::GraphicsCaptureDirtyRegionMode::ReportAndRender) {
            return true;
        }

        RECT localCaptureRect{
            captureRect.left - monitorRect.left,
            captureRect.top - monitorRect.top,
            captureRect.right - monitorRect.left,
            captureRect.bottom - monitorRect.top};
        for (auto const& dirtyRegion : frame.DirtyRegions()) {
            RECT dirtyRect{
                dirtyRegion.X,
                dirtyRegion.Y,
                dirtyRegion.X + dirtyRegion.Width,
                dirtyRegion.Y + dirtyRegion.Height};
            RECT intersection{};
            if (IntersectRect(&intersection,
                              &localCaptureRect,
                              &dirtyRect)) {
                return true;
            }
        }
        return false;
    } catch (...) {
        g_popupOverlayWgcDirtyRegionsEnabled.store(
            false, std::memory_order_release);
        return true;
    }
}
void RenderPopupOverlayWgcFrameLocked(
    capture::Direct3D11CaptureFrame const& frame) {
    g_popupOverlayWgcHadFrame = true;
    ++g_popupOverlayWgcFrameCount;
    g_popupOverlayWgcDiagnosticState =
        PopupOverlayWgcDiagnosticState::FrameArrived;
    if (!g_popupOverlayWgcRunning || !frame ||
        !g_popupOverlayWgcD2dContext ||
        !g_popupOverlayWgcSwapChain ||
        !g_popupOverlayWgcSwapChainTargetBitmap) {
        return;
    }

    PopupOverlayWgcRenderParameters renderParameters =
        g_popupOverlayWgcRenderParameters;
    RECT captureRect = g_popupOverlayWgcCaptureRectPx;
    RECT monitorRect = g_popupOverlayWgcMonitorRectPx;
    int targetWidth = g_popupOverlayWgcTargetWidthPx;
    int targetHeight = g_popupOverlayWgcTargetHeightPx;
    if (targetWidth <= 0 || targetHeight <= 0 ||
        captureRect.right <= captureRect.left ||
        captureRect.bottom <= captureRect.top) {
        return;
    }

    bool morphing = renderParameters.morphing;
    if (!PopupOverlayWgcFrameTouchesCapture(frame,
                                            captureRect,
                                            monitorRect,
                                            morphing)) {
        return;
    }

    {
        auto now = std::chrono::steady_clock::now();
        // WGC normally follows the monitor compositor cadence. Keep only a
        // small jitter guard so 120/144/165/240 Hz displays stay synchronized
        // with the XAML morph instead of being capped to 60 Hz.
        double targetIntervalMs = Clamp(g_popupOverlayWgcFrameIntervalMs,
                                        1000.0 / 360.0,
                                        1000.0 / 50.0);
        double guardIntervalMs = targetIntervalMs * 0.72;
        if (g_popupOverlayWgcLastRenderTime.time_since_epoch().count() != 0) {
            double elapsedMs =
                std::chrono::duration<double, std::milli>(
                    now - g_popupOverlayWgcLastRenderTime)
                    .count();
            if (elapsedMs < guardIntervalMs) {
                return;
            }
        }
        g_popupOverlayWgcLastRenderTime = now;
    }

    if (g_popupOverlayWgcFramesToSkip > 0) {
        --g_popupOverlayWgcFramesToSkip;
        return;
    }

    std::vector<BYTE> recordingCleanPixels;
    bool useRecordingCleanBackdrop = false;
    if (renderParameters.allowScreenCapture &&
        renderParameters.useLiquidLensWarp) {
        std::lock_guard cacheLock(g_popupBackdropOverlayHandoffMutex);
        if (g_popupBackdropOverlayCleanPixelsValid &&
            g_popupBackdropOverlayCleanWidth == targetWidth &&
            g_popupBackdropOverlayCleanHeight == targetHeight &&
            g_popupBackdropOverlayCleanPixels.size() ==
                static_cast<size_t>(targetWidth) * targetHeight * 4) {
            recordingCleanPixels = g_popupBackdropOverlayCleanPixels;
            useRecordingCleanBackdrop = true;
        }
    }

    try {
        auto surface = frame.Surface();
        winrt::com_ptr<IDirect3DDxgiInterfaceAccess> access;
        winrt::check_hresult(reinterpret_cast<IUnknown*>(winrt::get_abi(surface))
                                 ->QueryInterface(
                                     kIID_IDirect3DDxgiInterfaceAccess,
                                     access.put_void()));
        winrt::com_ptr<ID3D11Texture2D> frameTexture;
        winrt::check_hresult(access->GetInterface(
            __uuidof(ID3D11Texture2D),
            frameTexture.put_void()));
        winrt::com_ptr<IDXGISurface> frameSurface;
        frameTexture.as(frameSurface);

        ID2D1Bitmap1* targetBitmap =
            g_popupOverlayWgcSwapChainTargetBitmap.get();

        D2D1_BITMAP_PROPERTIES1 sourceProps = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_NONE,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_IGNORE));
        winrt::com_ptr<ID2D1Bitmap1> sourceBitmap;
        winrt::check_hresult(g_popupOverlayWgcD2dContext->CreateBitmapFromDxgiSurface(
            frameSurface.get(),
            &sourceProps,
            sourceBitmap.put()));

        float srcLeft = static_cast<float>(captureRect.left - monitorRect.left);
        float srcTop = static_cast<float>(captureRect.top - monitorRect.top);
        float srcRight = static_cast<float>(captureRect.right - monitorRect.left);
        float srcBottom = static_cast<float>(captureRect.bottom - monitorRect.top);
        D2D1_RECT_F sourceRect = D2D1::RectF(srcLeft, srcTop, srcRight, srcBottom);
        D2D1_RECT_F targetRect = D2D1::RectF(
            0.0f, 0.0f,
            static_cast<float>(targetWidth),
            static_cast<float>(targetHeight));

        int cornerRadiusPx = renderParameters.targetRadiusPx;
        cornerRadiusPx = Clamp(
            cornerRadiusPx, 1,
            std::max(1, std::min(targetWidth, targetHeight) / 2));

        winrt::com_ptr<ID2D1Bitmap1> recordingCleanBitmap;
        winrt::com_ptr<ID2D1RoundedRectangleGeometry> recordingCleanMaskGeometry;
        if (useRecordingCleanBackdrop) {
            winrt::check_hresult(g_popupOverlayWgcD2dContext->CreateBitmap(
                D2D1::SizeU(static_cast<UINT32>(targetWidth),
                            static_cast<UINT32>(targetHeight)),
                recordingCleanPixels.data(),
                static_cast<UINT32>(targetWidth * 4),
                &sourceProps,
                recordingCleanBitmap.put()));

            D2D1_ROUNDED_RECT cleanMaskRect = D2D1::RoundedRect(
                targetRect,
                static_cast<float>(cornerRadiusPx),
                static_cast<float>(cornerRadiusPx));
            winrt::check_hresult(g_popupOverlayWgcD2dFactory->CreateRoundedRectangleGeometry(
                &cleanMaskRect,
                recordingCleanMaskGeometry.put()));
        }
        bool useLiquidLensWarp = renderParameters.useLiquidLensWarp;
        D2D1_BITMAP_PROPERTIES1 intermediateProps = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_IGNORE));
        if (!EnsurePopupOverlayWgcRenderCache(targetWidth,
                                              targetHeight,
                                              intermediateProps,
                                              useLiquidLensWarp)) {
            g_popupOverlayWgcDiagnosticHr = g_popupOverlayWgcLastHr;
            g_popupOverlayWgcDiagnosticState = PopupOverlayWgcDiagnosticState::RenderFailed;
            ++g_popupOverlayWgcRenderFailCount;
            return;
        }
        auto& intermediateBitmap = g_popupOverlayWgcIntermediateBitmap;

        // Step 1: crop/scale the monitor capture into an offscreen target-sized
        // bitmap. This keeps the later Gaussian blur independent of monitor
        // resolution and avoids trying to draw the full monitor-sized effect.
        g_popupOverlayWgcD2dContext->SetTarget(intermediateBitmap.get());
        g_popupOverlayWgcD2dContext->BeginDraw();
        g_popupOverlayWgcD2dContext->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
        g_popupOverlayWgcD2dContext->DrawBitmap(
            sourceBitmap.get(),
            targetRect,
            1.0f,
            D2D1_INTERPOLATION_MODE_LINEAR,
            sourceRect);
        if (recordingCleanBitmap && recordingCleanMaskGeometry) {
            D2D1_LAYER_PARAMETERS1 cleanLayerParams = D2D1::LayerParameters1(
                targetRect,
                recordingCleanMaskGeometry.get(),
                D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
                D2D1::IdentityMatrix(),
                1.0f,
                nullptr,
                D2D1_LAYER_OPTIONS1_NONE);
            g_popupOverlayWgcD2dContext->PushLayer(cleanLayerParams, nullptr);
            g_popupOverlayWgcD2dContext->DrawBitmap(
                recordingCleanBitmap.get(),
                targetRect,
                1.0f,
                D2D1_INTERPOLATION_MODE_LINEAR);
            g_popupOverlayWgcD2dContext->PopLayer();
        }
        HRESULT hr = g_popupOverlayWgcD2dContext->EndDraw();
        if (FAILED(hr)) {
            g_popupOverlayWgcLastHr = hr;
            g_popupOverlayWgcDiagnosticHr = hr;
            g_popupOverlayWgcDiagnosticState = PopupOverlayWgcDiagnosticState::RenderFailed;
            ++g_popupOverlayWgcRenderFailCount;
            Wh_Log(L"Island: overlay WGC state intermediate EndDraw failed hr=0x%08X frames=%llu fails=%llu",
                   static_cast<unsigned>(hr),
                   static_cast<unsigned long long>(g_popupOverlayWgcFrameCount),
                   static_cast<unsigned long long>(g_popupOverlayWgcRenderFailCount));
            return;
        }

        // Keep the blur outputs as a Direct2D effect graph and evaluate them
        // directly into the final swapchain target. This preserves the exact
        // blur and refraction parameters while removing two full-size
        // render-target clears, writes, reads, and EndDraw submissions.
        g_popupOverlayWgcBlurEffect->SetInput(0, intermediateBitmap.get());
        g_popupOverlayWgcBlurEffect->SetValue(
            D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION,
            renderParameters.blurStdDev);
        winrt::com_ptr<ID2D1Image> blurredImage;
        g_popupOverlayWgcBlurEffect->GetOutput(blurredImage.put());
        if (!blurredImage) {
            g_popupOverlayWgcLastHr = E_FAIL;
            g_popupOverlayWgcDiagnosticHr = E_FAIL;
            g_popupOverlayWgcDiagnosticState =
                PopupOverlayWgcDiagnosticState::RenderFailed;
            ++g_popupOverlayWgcRenderFailCount;
            return;
        }

        winrt::com_ptr<ID2D1Image> refractionEffectOutput;
        ID2D1Image* refractionImage = intermediateBitmap.get();
        if (useLiquidLensWarp) {
            g_popupOverlayWgcRefractionBlurEffect->SetInput(
                0, intermediateBitmap.get());
            g_popupOverlayWgcRefractionBlurEffect->SetValue(
                D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION,
                renderParameters.refractionBlurStdDev);
            g_popupOverlayWgcRefractionBlurEffect->GetOutput(
                refractionEffectOutput.put());
            if (refractionEffectOutput) {
                refractionImage = refractionEffectOutput.get();
            }
        }

        D2D1_POINT_2F imageTargetOffset = D2D1::Point2F(0.0f, 0.0f);
        ID2D1Image* renderImage = blurredImage.get();
        if (!useLiquidLensWarp) {
            ClearPopupOverlayWgcLensDisplacementMap();
        }
        // Step 2: render the final rounded premultiplied surface directly into
        // the DirectComposition swapchain backbuffer. Capture, blur, clipping,
        // alpha and present now stay on the GPU.
        BYTE maxAlpha = static_cast<BYTE>(Clamp(
            g_popupBackdropOverlayFrameAlpha.load(std::memory_order_relaxed),
            0, 255));
        double layerOpacity = static_cast<double>(maxAlpha) / 255.0;
        ID2D1RoundedRectangleGeometry* roundedGeometry =
            EnsurePopupOverlayWgcRoundedGeometry(targetRect,
                                                targetWidth,
                                                targetHeight,
                                                cornerRadiusPx);
        if (!roundedGeometry) {
            g_popupOverlayWgcDiagnosticHr = g_popupOverlayWgcLastHr;
            g_popupOverlayWgcDiagnosticState = PopupOverlayWgcDiagnosticState::RenderFailed;
            ++g_popupOverlayWgcRenderFailCount;
            return;
        }

        g_popupOverlayWgcD2dContext->SetTarget(targetBitmap);
        g_popupOverlayWgcD2dContext->BeginDraw();
        g_popupOverlayWgcD2dContext->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
        D2D1_LAYER_PARAMETERS1 layerParams = D2D1::LayerParameters1(
            targetRect,
            roundedGeometry,
            D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
            D2D1::IdentityMatrix(),
            static_cast<float>(layerOpacity),
            nullptr,
            D2D1_LAYER_OPTIONS1_NONE);
        g_popupOverlayWgcD2dContext->PushLayer(layerParams, nullptr);
        bool drewGpuLens = useLiquidLensWarp &&
            DrawLiquidGlassGpuLensWarp(g_popupOverlayWgcD2dContext.get(),
                                       blurredImage.get(),
                                       refractionImage,
                                       targetWidth,
                                       targetHeight,
                                       cornerRadiusPx,
                                       renderParameters.liquidRimAlpha);
        if (!drewGpuLens) {
            g_popupOverlayWgcD2dContext->DrawImage(
                renderImage,
                &imageTargetOffset,
                nullptr,
                D2D1_INTERPOLATION_MODE_LINEAR,
                D2D1_COMPOSITE_MODE_SOURCE_COPY);
        }
        g_popupOverlayWgcD2dContext->PopLayer();
        hr = g_popupOverlayWgcD2dContext->EndDraw();
        if (FAILED(hr)) {
            g_popupOverlayWgcLastHr = hr;
            g_popupOverlayWgcDiagnosticHr = hr;
            g_popupOverlayWgcDiagnosticState = PopupOverlayWgcDiagnosticState::RenderFailed;
            ++g_popupOverlayWgcRenderFailCount;
            Wh_Log(L"Island: overlay WGC state EndDraw failed hr=0x%08X frames=%llu fails=%llu",
                   static_cast<unsigned>(hr),
                   static_cast<unsigned long long>(g_popupOverlayWgcFrameCount),
                   static_cast<unsigned long long>(g_popupOverlayWgcRenderFailCount));
            return;
        }

        hr = g_popupOverlayWgcSwapChain->Present(0, 0);
        if (FAILED(hr)) {
            g_popupOverlayWgcLastHr = hr;
            g_popupOverlayWgcDiagnosticHr = hr;
            g_popupOverlayWgcDiagnosticState = PopupOverlayWgcDiagnosticState::UpdateFailed;
            ++g_popupOverlayWgcRenderFailCount;
            Wh_Log(L"Island: overlay WGC swapchain Present failed hr=0x%08X frames=%llu fails=%llu",
                   static_cast<unsigned>(hr),
                   static_cast<unsigned long long>(g_popupOverlayWgcFrameCount),
                   static_cast<unsigned long long>(g_popupOverlayWgcRenderFailCount));
            return;
        }
        bool attachedDcompContent = false;
        if (g_popupOverlayDcompVisual && g_popupOverlayWgcSwapChain &&
            !g_popupOverlayDcompContentAttached) {
            g_popupOverlayDcompVisual->SetContent(g_popupOverlayWgcSwapChain.get());
            g_popupOverlayDcompContentAttached = true;
            attachedDcompContent = true;
        }
        if (attachedDcompContent && g_popupOverlayDcompDevice) {
            g_popupOverlayDcompDevice->Commit();
        }
        g_popupOverlayWgcDiagnosticState = PopupOverlayWgcDiagnosticState::VisibleFrame;
        g_popupOverlayWgcReadbackHadVisibleFrame = true;
        g_popupOverlayWgcFallbackPainted = false;
    } catch (winrt::hresult_error const& error) {
        g_popupOverlayWgcLastHr = error.code().value;
        g_popupOverlayWgcDiagnosticHr = error.code().value;
        g_popupOverlayWgcDiagnosticState = PopupOverlayWgcDiagnosticState::RenderFailed;
        ++g_popupOverlayWgcRenderFailCount;
        Wh_Log(L"Island: overlay WGC state hresult_error=0x%08X frames=%llu fails=%llu",
               static_cast<unsigned>(g_popupOverlayWgcLastHr),
               static_cast<unsigned long long>(g_popupOverlayWgcFrameCount),
               static_cast<unsigned long long>(g_popupOverlayWgcRenderFailCount));
    } catch (...) {
        g_popupOverlayWgcLastHr = E_FAIL;
        g_popupOverlayWgcDiagnosticHr = E_FAIL;
        g_popupOverlayWgcDiagnosticState = PopupOverlayWgcDiagnosticState::RenderFailed;
        ++g_popupOverlayWgcRenderFailCount;
        Wh_Log(L"Island: overlay WGC state unknown failure frames=%llu fails=%llu",
               static_cast<unsigned long long>(g_popupOverlayWgcFrameCount),
               static_cast<unsigned long long>(g_popupOverlayWgcRenderFailCount));
    }
}

void ResetPopupOverlayWgcDeviceResourcesLocked() {
    g_popupOverlayWgcSession = nullptr;
    g_popupOverlayWgcFramePool = nullptr;
    g_popupOverlayWgcFrameArrivedToken = {};
    g_popupOverlayWgcFrameCallbackHooked = false;
    g_popupOverlayWgcDirtyRegionsEnabled.store(
        false, std::memory_order_release);
    g_popupOverlayWgcItem = nullptr;
    g_popupOverlayWgcGraphicsDevice = nullptr;
    g_popupOverlayWgcSwapChainTargetBitmap = nullptr;
    g_popupOverlayWgcSwapChain = nullptr;
    g_popupOverlayDcompContentAttached = false;
    if (g_popupOverlayDcompVisual) {
        g_popupOverlayDcompVisual->SetContent(nullptr);
    }
    if (g_popupOverlayDcompTarget) {
        g_popupOverlayDcompTarget->SetRoot(nullptr);
    }
    if (g_popupOverlayDcompDevice) {
        g_popupOverlayDcompDevice->Commit();
    }
    g_popupOverlayDcompVisual = nullptr;
    g_popupOverlayDcompTransformValid = false;
    g_popupOverlayDcompLastScaleX = 0.0f;
    g_popupOverlayDcompLastScaleY = 0.0f;
    g_popupOverlayDcompTarget = nullptr;
    g_popupOverlayDcompDevice = nullptr;
    ClearPopupOverlayWgcRenderCache();
    ClearPopupOverlayWgcLensDisplacementMap();
    g_popupOverlayWgcD2dContext = nullptr;
    g_popupOverlayWgcD2dDevice = nullptr;
    g_popupOverlayWgcD2dFactory = nullptr;
    g_popupOverlayWgcDxgiDevice = nullptr;
    g_popupOverlayWgcD3dContext = nullptr;
    g_popupOverlayWgcD3dDevice = nullptr;
    g_popupOverlayWgcTargetWidthPx = 0;
    g_popupOverlayWgcTargetHeightPx = 0;
    g_popupOverlayWgcTargetRadiusPx = 0;
    g_popupOverlayWgcRenderParameters = {};
    g_popupOverlayWgcFrameIntervalMs = 1000.0 / 60.0;
    g_popupOverlayWgcMonitor = nullptr;
    g_popupOverlayWgcCaptureRectPx = {LONG_MIN, LONG_MIN, LONG_MIN, LONG_MIN};
    g_popupOverlayWgcMonitorRectPx = {};
}

void StopPopupOverlayWgcBackdrop() {
    capture::GraphicsCaptureSession sessionToClose{nullptr};
    capture::Direct3D11CaptureFramePool framePoolToClose{nullptr};
    winrt::event_token frameArrivedToken{};
    bool frameCallbackHooked = false;

    {
        std::lock_guard lock(g_popupOverlayWgcMutex);
        g_popupOverlayWgcRunning = false;
        sessionToClose = g_popupOverlayWgcSession;
        framePoolToClose = g_popupOverlayWgcFramePool;
        frameArrivedToken = g_popupOverlayWgcFrameArrivedToken;
        frameCallbackHooked = g_popupOverlayWgcFrameCallbackHooked;
        g_popupOverlayWgcFrameCallbackHooked = false;
    }

    try {
        if (framePoolToClose && frameCallbackHooked) {
            framePoolToClose.FrameArrived(frameArrivedToken);
        }
    } catch (...) {
    }

    for (int i = 0; i < 80 &&
                    g_popupOverlayWgcRendering.load(std::memory_order_acquire);
         ++i) {
        Sleep(1);
    }

    {
        std::lock_guard lock(g_popupOverlayWgcMutex);
        ResetPopupOverlayWgcDeviceResourcesLocked();
        g_popupOverlayWgcHadFrame = false;
        g_popupOverlayWgcDiagnosticStartTime = {};
        g_popupOverlayWgcLastRenderTime = {};
        g_popupOverlayWgcLastResizeTime = {};
        g_popupOverlayWgcReadbackHadVisibleFrame = false;
        g_popupOverlayWgcDiagnosticState = PopupOverlayWgcDiagnosticState::NotStarted;
        g_popupOverlayWgcDiagnosticHr = S_OK;
        g_popupOverlayWgcFrameCount = 0;
        g_popupOverlayWgcRenderFailCount = 0;
        // Keep retry throttle fields intact here. This function is called before
        // rebuilding WGC resources, so clearing them here would re-enable rapid
        // CreateForMonitor retries after a failure.
        g_popupOverlayWgcFramesToSkip = 0;
    }

    if (g_expandedPopup) {
        SetWindowDisplayAffinity(g_expandedPopup, WDA_NONE);
    }
    if (g_popupXamlChild) {
        SetWindowDisplayAffinity(g_popupXamlChild, WDA_NONE);
    }
    if (g_popupBackdropOverlay) {
        SetWindowDisplayAffinity(g_popupBackdropOverlay, WDA_NONE);
    }
    g_expandedPopupCaptureExcluded = false;
    g_popupXamlChildCaptureExcluded = false;
    g_popupBackdropOverlayCaptureExcluded = false;
    try {
        if (sessionToClose) {
            sessionToClose.Close();
        }
        if (framePoolToClose) {
            framePoolToClose.Close();
        }
    } catch (...) {
    }
}
void ReleasePopupOverlayWgcDeviceResources() {
    StopPopupOverlayWgcBackdrop();

    std::lock_guard lock(g_popupOverlayWgcMutex);
    ResetPopupOverlayWgcDeviceResourcesLocked();
}

bool StartPopupOverlayWgcBackdrop(
    RECT const& captureRect,
    int widthPx,
    int heightPx,
    PopupOverlayWgcRenderParameters const& renderParameters) {
    auto now = std::chrono::steady_clock::now();
    if (g_popupOverlayWgcCreateItemFailed &&
        g_popupOverlayWgcLastStartAttemptTime.time_since_epoch().count() != 0 &&
        now - g_popupOverlayWgcLastStartAttemptTime <
            std::chrono::seconds(3)) {
        g_popupOverlayWgcDiagnosticState = PopupOverlayWgcDiagnosticState::StartSkipped;
        return false;
    }
    g_popupOverlayWgcLastStartAttemptTime = now;

    if (g_unloading || widthPx <= 0 || heightPx <= 0) {
        g_popupOverlayWgcDiagnosticState = PopupOverlayWgcDiagnosticState::StartSkipped;
        return false;
    }

    if (!UseOverlayPopupBackdropMaterial() || !g_popupBackdropOverlay ||
        !g_expandedPopup) {
        g_popupOverlayWgcDiagnosticState = PopupOverlayWgcDiagnosticState::StartSkipped;
        Wh_Log(L"Island: overlay WGC state start skipped material=%d overlay=%p popup=%p unloading=%d size=%dx%d",
               UseOverlayPopupBackdropMaterial(),
               g_popupBackdropOverlay,
               g_expandedPopup,
               g_unloading.load(),
               widthPx,
               heightPx);
        return false;
    }

    HMONITOR monitor = MonitorFromRect(&captureRect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{sizeof(monitorInfo)};
    if (!monitor || !GetMonitorInfoW(monitor, &monitorInfo)) {
        g_popupOverlayWgcDiagnosticState = PopupOverlayWgcDiagnosticState::StartSkipped;
        Wh_Log(L"Island: overlay WGC state no monitor");
        return false;
    }
    double frameIntervalMs =
        PopupOverlayWgcFrameIntervalMsForMonitor(monitor);

    {
        std::unique_lock lock(g_popupOverlayWgcMutex, std::try_to_lock);
        if (!lock.owns_lock()) {
            // The GPU callback is presenting the current frame. Keep the
            // previous snapshot for this tick instead of blocking the taskbar
            // animation thread.
            return true;
        }

        if (g_popupOverlayWgcRunning &&
            g_popupOverlayWgcSwapChain &&
            g_popupOverlayWgcSwapChainTargetBitmap &&
            g_popupOverlayWgcMonitor == monitor) {
            // Keep the monitor capture session alive during the morph. Only
            // update its immutable-per-frame input snapshot.
            g_popupOverlayWgcCreateItemFailed = false;
            bool sizeChanged =
                g_popupOverlayWgcTargetWidthPx != widthPx ||
                g_popupOverlayWgcTargetHeightPx != heightPx;
            bool resizeDue =
                g_popupOverlayWgcLastResizeTime.time_since_epoch().count() == 0 ||
                now - g_popupOverlayWgcLastResizeTime >=
                    std::chrono::milliseconds(40);
            if (sizeChanged && !renderParameters.morphing && resizeDue) {
                if (!RecreatePopupOverlayWgcReadbackTextures(widthPx,
                                                             heightPx)) {
                    g_popupOverlayWgcDiagnosticState =
                        PopupOverlayWgcDiagnosticState::RenderFailed;
                    ++g_popupOverlayWgcRenderFailCount;
                    return false;
                }
                g_popupOverlayWgcLastResizeTime = now;
            }
            if (sizeChanged && renderParameters.morphing) {
                UpdatePopupOverlayDcompVisualTransformLocked(widthPx,
                                                             heightPx);
            }
            g_popupOverlayWgcCaptureRectPx = captureRect;
            g_popupOverlayWgcMonitorRectPx = monitorInfo.rcMonitor;
            g_popupOverlayWgcTargetRadiusPx =
                Clamp(renderParameters.targetRadiusPx,
                      1,
                      std::max(1, std::min(widthPx, heightPx) / 2));
            g_popupOverlayWgcRenderParameters = renderParameters;
            g_popupOverlayWgcRenderParameters.targetRadiusPx =
                g_popupOverlayWgcTargetRadiusPx;
            g_popupOverlayWgcFrameIntervalMs = frameIntervalMs;
            return true;
        }
    }

    StopPopupOverlayWgcBackdrop();

    if (!capture::GraphicsCaptureSession::IsSupported()) {
        g_popupOverlayWgcDiagnosticState = PopupOverlayWgcDiagnosticState::StartSkipped;
        Wh_Log(L"Island: overlay WGC state not supported");
        return false;
    }

    RequestPopupOverlayWgcBorderlessAccessAsync();
    if (!PopupOverlayWgcBorderlessAccessAllowed()) {
        g_popupOverlayWgcDiagnosticState = PopupOverlayWgcDiagnosticState::StartSkipped;
        if (!PopupOverlayWgcBorderlessAccessPending()) {
            g_popupOverlayWgcDiagnosticHr = E_ACCESSDENIED;
        }
        return false;
    }

    {
        std::lock_guard lock(g_popupOverlayWgcMutex);
        if (!EnsurePopupOverlayWgcDeviceResourcesLocked(g_popupBackdropOverlay,
                                                        widthPx,
                                                        heightPx)) {
            g_popupOverlayWgcDiagnosticState =
                PopupOverlayWgcDiagnosticState::StartSkipped;
            Wh_Log(L"Island: overlay WGC state Ensure resources failed hr=0x%08X",
                   static_cast<unsigned>(g_popupOverlayWgcLastHr));
            return false;
        }
    }

    if (!CreatePopupOverlayWgcCaptureItemForMonitor(monitor,
                                                    g_popupOverlayWgcItem)) {
        g_popupOverlayWgcDiagnosticState = PopupOverlayWgcDiagnosticState::StartSkipped;
        g_popupOverlayWgcDiagnosticHr = g_popupOverlayWgcCreateItemHr;
        g_popupOverlayWgcCreateItemFailed = true;
        Wh_Log(L"Island: overlay WGC state Create capture item failed hr=0x%08X; throttling retries",
               static_cast<unsigned>(g_popupOverlayWgcCreateItemHr));
        return false;
    }
    g_popupOverlayWgcCreateItemFailed = false;

    // Keep the existing blurred fallback visible until the first WGC frame is
    // ready. Exclude the overlay from capture first; optionally keep the popup
    // capturable so users can record the expanded material effect.
    bool captureAffinityChanged =
        SetExpandedPopupCaptureExclusion(
            !renderParameters.allowScreenCapture);
    captureAffinityChanged =
        SetPopupWindowCaptureExclusion(g_popupBackdropOverlay, true) ||
        captureAffinityChanged;
    if (captureAffinityChanged) {
        DwmFlush();
    }

    std::lock_guard lock(g_popupOverlayWgcMutex);
    g_popupOverlayWgcCaptureRectPx = captureRect;
    g_popupOverlayWgcMonitorRectPx = monitorInfo.rcMonitor;
    g_popupOverlayWgcMonitor = monitor;
    g_popupOverlayWgcTargetRadiusPx =
        Clamp(renderParameters.targetRadiusPx,
              1,
              std::max(1, std::min(widthPx, heightPx) / 2));
    g_popupOverlayWgcRenderParameters = renderParameters;
    g_popupOverlayWgcRenderParameters.targetRadiusPx =
        g_popupOverlayWgcTargetRadiusPx;
    g_popupOverlayWgcFrameIntervalMs = frameIntervalMs;
    g_popupOverlayWgcRunning = true;
    g_popupOverlayWgcLastHr = S_OK;
    g_popupOverlayWgcDiagnosticHr = S_OK;
    g_popupOverlayWgcHadFrame = false;
    g_popupOverlayWgcFrameCount = 0;
    g_popupOverlayWgcRenderFailCount = 0;
    g_popupOverlayWgcDiagnosticState = PopupOverlayWgcDiagnosticState::StartedNoFrame;
    g_popupOverlayWgcDiagnosticStartTime = std::chrono::steady_clock::now();
    g_popupOverlayWgcLastResizeTime = g_popupOverlayWgcDiagnosticStartTime;
    g_popupOverlayWgcFramesToSkip =
        renderParameters.morphing ? 0 : PopupBackdropInitialFrameSkip();

    try {
        g_popupOverlayWgcFramePool = capture::Direct3D11CaptureFramePool::CreateFreeThreaded(
            g_popupOverlayWgcGraphicsDevice,
            directx::DirectXPixelFormat::B8G8R8A8UIntNormalized,
            2,
            g_popupOverlayWgcItem.Size());
        g_popupOverlayWgcFrameArrivedToken = g_popupOverlayWgcFramePool.FrameArrived(
            [](capture::Direct3D11CaptureFramePool const& sender,
               winrt::Windows::Foundation::IInspectable const&) {
                auto frame = sender.TryGetNextFrame();
                if (!frame) {
                    return;
                }

                bool expected = false;
                if (!g_popupOverlayWgcRendering.compare_exchange_strong(
                        expected, true, std::memory_order_acq_rel)) {
                    frame.Close();
                    return;
                }

                try {
                    std::lock_guard callbackLock(g_popupOverlayWgcMutex);
                    if (g_popupOverlayWgcRunning) {
                        RenderPopupOverlayWgcFrameLocked(frame);
                    }
                } catch (...) {
                    Wh_Log(
                        L"Island: overlay WGC callback render escaped exception");
                }
                g_popupOverlayWgcRendering.store(false,
                                                 std::memory_order_release);
                frame.Close();
            });
        g_popupOverlayWgcFrameCallbackHooked = true;
        g_popupOverlayWgcSession =
            g_popupOverlayWgcFramePool.CreateCaptureSession(g_popupOverlayWgcItem);
        g_popupOverlayWgcDirtyRegionsEnabled.store(
            false, std::memory_order_release);
        if (PopupOverlayWgcDirtyRegionsSupported()) {
            try {
                // Windows 11 24H2+ can preserve unchanged pixels in the frame
                // pool and render only compositor dirty regions. The callback
                // also uses those regions to skip blur work when a change is
                // completely outside the popup's sampled background.
                g_popupOverlayWgcSession.DirtyRegionMode(
                    capture::GraphicsCaptureDirtyRegionMode::ReportAndRender);
                g_popupOverlayWgcDirtyRegionsEnabled.store(
                    true, std::memory_order_release);
            } catch (...) {
            }
        }
        try {
            // Windows Graphics Capture shows a yellow border around monitor
            // captures by default. Request borderless access separately and
            // always set the per-session flag here; Windows ignores it until
            // access is granted, then the async callback applies it again to
            // the live session.
            g_popupOverlayWgcSession.IsBorderRequired(false);
        } catch (...) {
        }
        try {
            g_popupOverlayWgcSession.IsCursorCaptureEnabled(false);
        } catch (...) {
        }
        g_popupOverlayWgcSession.StartCapture();
        Wh_Log(L"Island: overlay WGC state backdrop started %dx%d monitor=(%ld,%ld,%ld,%ld)",
               widthPx,
               heightPx,
               monitorInfo.rcMonitor.left,
               monitorInfo.rcMonitor.top,
               monitorInfo.rcMonitor.right,
               monitorInfo.rcMonitor.bottom);
        return true;
    } catch (winrt::hresult_error const& error) {
        g_popupOverlayWgcLastHr = error.code().value;
        g_popupOverlayWgcDiagnosticState = PopupOverlayWgcDiagnosticState::StartSkipped;
        g_popupOverlayWgcDiagnosticHr = g_popupOverlayWgcLastHr;
        Wh_Log(L"Island: overlay WGC state start failed hr=0x%08X",
               static_cast<unsigned>(g_popupOverlayWgcLastHr));
        g_popupOverlayWgcRunning = false;
        g_popupOverlayWgcFrameCallbackHooked = false;
        ResetPopupOverlayWgcDeviceResourcesLocked();
        return false;
    } catch (...) {
        g_popupOverlayWgcLastHr = E_FAIL;
        g_popupOverlayWgcDiagnosticState = PopupOverlayWgcDiagnosticState::StartSkipped;
        g_popupOverlayWgcDiagnosticHr = E_FAIL;
        Wh_Log(L"Island: overlay WGC state start failed unknown");
        g_popupOverlayWgcRunning = false;
        g_popupOverlayWgcFrameCallbackHooked = false;
        ResetPopupOverlayWgcDeviceResourcesLocked();
        return false;
    }
}

BYTE PopupRoundedRectAlpha(int x, int y, int width, int height, int radius) {
    if (width <= 0 || height <= 0) {
        return 0;
    }

    radius = Clamp(radius, 1, std::max(1, std::min(width, height) / 2));

    auto contains = [&](double px, double py) {
        double left = 0.0;
        double top = 0.0;
        double right = static_cast<double>(width);
        double bottom = static_cast<double>(height);
        double r = static_cast<double>(radius);

        double cx = Clamp(px, left + r, right - r);
        double cy = Clamp(py, top + r, bottom - r);
        double dx = px - cx;
        double dy = py - cy;
        return dx * dx + dy * dy <= r * r;
    };

    double px = static_cast<double>(x) + 0.5;
    double py = static_cast<double>(y) + 0.5;
    if (contains(px, py)) {
        double innerMargin = 0.85;
        double left = static_cast<double>(radius) + innerMargin;
        double right = static_cast<double>(width - radius) - innerMargin;
        double top = static_cast<double>(radius) + innerMargin;
        double bottom = static_cast<double>(height - radius) - innerMargin;
        if ((px >= left && px <= right) || (py >= top && py <= bottom)) {
            return 255;
        }
    }

    constexpr int kSamplesPerAxis = 4;
    int coveredSamples = 0;
    for (int sy = 0; sy < kSamplesPerAxis; ++sy) {
        double sampleY = static_cast<double>(y) +
                         (static_cast<double>(sy) + 0.5) /
                             static_cast<double>(kSamplesPerAxis);
        for (int sx = 0; sx < kSamplesPerAxis; ++sx) {
            double sampleX = static_cast<double>(x) +
                             (static_cast<double>(sx) + 0.5) /
                                 static_cast<double>(kSamplesPerAxis);
            if (contains(sampleX, sampleY)) {
                ++coveredSamples;
            }
        }
    }

    return static_cast<BYTE>(std::lround(
        255.0 * static_cast<double>(coveredSamples) /
        static_cast<double>(kSamplesPerAxis * kSamplesPerAxis)));
}

void BoxBlurPbgraPixels(std::vector<BYTE>& pixels, int width, int height, int passes) {
    if (width <= 1 || height <= 1 || pixels.size() < static_cast<size_t>(width) * height * 4) {
        return;
    }

    std::vector<BYTE> temp(pixels.size());
    for (int pass = 0; pass < passes; ++pass) {
        temp = pixels;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int sumB = 0, sumG = 0, sumR = 0, sumA = 0, count = 0;
                for (int yy = std::max(0, y - 1); yy <= std::min(height - 1, y + 1); ++yy) {
                    for (int xx = std::max(0, x - 1); xx <= std::min(width - 1, x + 1); ++xx) {
                        BYTE* p = temp.data() + (static_cast<size_t>(yy) * width + xx) * 4;
                        sumB += p[0];
                        sumG += p[1];
                        sumR += p[2];
                        sumA += p[3];
                        ++count;
                    }
                }
                BYTE* out = pixels.data() + (static_cast<size_t>(y) * width + x) * 4;
                out[0] = static_cast<BYTE>(sumB / count);
                out[1] = static_cast<BYTE>(sumG / count);
                out[2] = static_cast<BYTE>(sumR / count);
                out[3] = static_cast<BYTE>(sumA / count);
            }
        }
    }
}

bool* PopupCaptureExclusionCacheForHwnd(HWND hwnd) {
    if (hwnd && hwnd == g_expandedPopup) {
        return &g_expandedPopupCaptureExcluded;
    }
    if (hwnd && hwnd == g_popupXamlChild) {
        return &g_popupXamlChildCaptureExcluded;
    }
    if (hwnd && hwnd == g_popupBackdropOverlay) {
        return &g_popupBackdropOverlayCaptureExcluded;
    }
    return nullptr;
}

bool SetPopupWindowCaptureExclusion(HWND hwnd, bool exclude) {
    if (!hwnd) {
        return false;
    }

#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif
#ifndef WDA_NONE
#define WDA_NONE 0x00000000
#endif

    bool* cached = PopupCaptureExclusionCacheForHwnd(hwnd);
    if (cached && *cached == exclude) {
        return false;
    }

    BOOL affinitySet = SetWindowDisplayAffinity(
        hwnd,
        exclude ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
    if (cached) {
        // Avoid hammering DWM every animation tick. Repeating the same display
        // affinity call can invalidate composition surfaces and shows up as
        // intermittent input loss or blur flicker on Explorer's thread. Child
        // HWNDs may reject display affinity, but retrying them every frame is
        // still wasted work.
        *cached = exclude;
    }
    return affinitySet != FALSE;
}

bool SetExpandedPopupCaptureExclusion(bool exclude) {
    bool changed = SetPopupWindowCaptureExclusion(g_expandedPopup, exclude);
    changed = SetPopupWindowCaptureExclusion(g_popupXamlChild, exclude) || changed;
    return changed;
}


bool ClearPopupBackdropOverlayLayeredSurface(HWND hwnd) {
    if (!hwnd) {
        return false;
    }

    RECT rect{};
    GetWindowRect(hwnd, &rect);
    int width = std::max(1L, rect.right - rect.left);
    int height = std::max(1L, rect.bottom - rect.top);

    // Guard against invalid stale geometry during Explorer unload/reload.
    width = Clamp(width, 1, 2048);
    height = Clamp(height, 1, 2048);

    HDC screenDc = GetDC(nullptr);
    if (!screenDc) {
        return false;
    }
    HDC memDc = CreateCompatibleDC(screenDc);
    if (!memDc) {
        ReleaseDC(nullptr, screenDc);
        return false;
    }

    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(screenDc, &info, DIB_RGB_COLORS,
                                      &bits, nullptr, 0);
    if (!bitmap || !bits) {
        if (bitmap) {
            DeleteObject(bitmap);
        }
        DeleteDC(memDc);
        ReleaseDC(nullptr, screenDc);
        return false;
    }

    std::memset(bits, 0, static_cast<size_t>(width) * height * 4);

    HGDIOBJ oldBitmap = SelectObject(memDc, bitmap);
    POINT srcPoint{0, 0};
    SIZE size{width, height};
    POINT dstPoint{rect.left, rect.top};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

    BOOL ok = UpdateLayeredWindow(hwnd, screenDc, &dstPoint, &size,
                                  memDc, &srcPoint, 0, &blend, ULW_ALPHA);

    SelectObject(memDc, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(memDc);
    ReleaseDC(nullptr, screenDc);
    return !!ok;
}

bool CaptureLowResScreenPixels(RECT const& screenRect,
                               int lowWidth,
                               int lowHeight,
                               std::vector<BYTE>& pixels,
                               HWND excludeOverlay = nullptr) {
    if (lowWidth <= 0 || lowHeight <= 0 ||
        screenRect.right <= screenRect.left ||
        screenRect.bottom <= screenRect.top) {
        return false;
    }

    pixels.assign(static_cast<size_t>(lowWidth) * lowHeight * 4, 0);

    // Keep the overlay visible while sampling. Hiding/restoring the layered
    // window here caused an obvious flash during the popup morph. Capture
    // exclusion is still requested; if a build ignores it for GetDC(nullptr),
    // WGC will take over after the final geometry is ready.
    if (excludeOverlay) {
        SetPopupWindowCaptureExclusion(excludeOverlay, true);
    }
    auto restoreExcludedOverlay = [&] {
        if (excludeOverlay) {
            // Keep the overlay excluded while a WGC session is active; otherwise
            // restore normal visibility for plain GDI fallback mode.
            SetPopupWindowCaptureExclusion(excludeOverlay,
                                           g_popupOverlayWgcRunning);
        }
    };

    HDC screenDc = GetDC(nullptr);
    if (!screenDc) {
        restoreExcludedOverlay();
        return false;
    }

    HDC memDc = CreateCompatibleDC(screenDc);
    if (!memDc) {
        ReleaseDC(nullptr, screenDc);
        restoreExcludedOverlay();
        return false;
    }

    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = lowWidth;
    info.bmiHeader.biHeight = -lowHeight;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(screenDc, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap || !bits) {
        if (bitmap) DeleteObject(bitmap);
        DeleteDC(memDc);
        ReleaseDC(nullptr, screenDc);
        restoreExcludedOverlay();
        return false;
    }

    HGDIOBJ oldBitmap = SelectObject(memDc, bitmap);
    SetStretchBltMode(memDc, HALFTONE);
    SetBrushOrgEx(memDc, 0, 0, nullptr);

    int sourceWidth = screenRect.right - screenRect.left;
    int sourceHeight = screenRect.bottom - screenRect.top;
    BOOL ok = StretchBlt(memDc, 0, 0, lowWidth, lowHeight,
                         screenDc, screenRect.left, screenRect.top,
                         sourceWidth, sourceHeight, SRCCOPY);

    if (ok) {
        std::memcpy(pixels.data(), bits, pixels.size());
    }

    SelectObject(memDc, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(memDc);
    ReleaseDC(nullptr, screenDc);
    restoreExcludedOverlay();
    return !!ok;
}

void SampleBilinearPbgra(std::vector<BYTE> const& pixels,
                       int width,
                       int height,
                       double x,
                       double y,
                       int& b,
                       int& g,
                       int& r) {
    if (width <= 0 || height <= 0 || pixels.empty()) {
        b = g = r = 0;
        return;
    }

    x = Clamp(x, 0.0, static_cast<double>(width - 1));
    y = Clamp(y, 0.0, static_cast<double>(height - 1));
    int x0 = Clamp(static_cast<int>(std::floor(x)), 0, width - 1);
    int y0 = Clamp(static_cast<int>(std::floor(y)), 0, height - 1);
    int x1 = Clamp(x0 + 1, 0, width - 1);
    int y1 = Clamp(y0 + 1, 0, height - 1);
    double fx = x - x0;
    double fy = y - y0;

    auto pixel = [&](int px, int py) -> BYTE const* {
        return pixels.data() + (static_cast<size_t>(py) * width + px) * 4;
    };

    BYTE const* p00 = pixel(x0, y0);
    BYTE const* p10 = pixel(x1, y0);
    BYTE const* p01 = pixel(x0, y1);
    BYTE const* p11 = pixel(x1, y1);

    auto blend = [&](int channel) -> int {
        double top = p00[channel] * (1.0 - fx) + p10[channel] * fx;
        double bottom = p01[channel] * (1.0 - fx) + p11[channel] * fx;
        return Clamp(static_cast<int>(std::lround(top * (1.0 - fy) + bottom * fy)), 0, 255);
    };

    b = blend(0);
    g = blend(1);
    r = blend(2);
}

bool UpdatePopupBackdropOverlayLayeredBlur(HWND hwnd,
                                           RECT const& screenRect,
                                           int width,
                                           int height,
                                           int cornerRadiusPx,
                                           bool force) {
    if (!hwnd || width <= 2 || height <= 2) {
        return false;
    }

    auto now = std::chrono::steady_clock::now();
    bool sizeChanged =
        width != g_popupBackdropOverlayLastWidth ||
        height != g_popupBackdropOverlayLastHeight ||
        cornerRadiusPx != g_popupBackdropOverlayLastRadius;
    if (!force && !sizeChanged &&
        g_popupBackdropOverlayLastPaintTime.time_since_epoch().count() != 0) {
        auto age = std::chrono::duration_cast<std::chrono::milliseconds>(
                       now - g_popupBackdropOverlayLastPaintTime)
                       .count();
        if (age < kPopupBackdropOverlayRefreshMs) {
            return true;
        }
    }

    int captureScale = PopupBackdropFallbackCaptureScale();
    int lowWidth = Clamp(width / captureScale,
                         40, kPopupBackdropOverlayMaxCaptureSize);
    int lowHeight = Clamp(height / captureScale,
                          40, kPopupBackdropOverlayMaxCaptureSize);
    std::vector<BYTE> lowPixels;
    if (!CaptureLowResScreenPixels(screenRect, lowWidth, lowHeight, lowPixels, hwnd)) {
        return false;
    }

    std::vector<BYTE> cleanPixels(static_cast<size_t>(width) * height * 4);
    for (int y = 0; y < height; ++y) {
        double sampleY = ((static_cast<double>(y) + 0.5) * lowHeight / height) - 0.5;
        for (int x = 0; x < width; ++x) {
            double sampleX = ((static_cast<double>(x) + 0.5) * lowWidth / width) - 0.5;
            int sb = 0;
            int sg = 0;
            int sr = 0;
            SampleBilinearPbgra(lowPixels, lowWidth, lowHeight, sampleX, sampleY, sb, sg, sr);

            BYTE* dst = cleanPixels.data() +
                         (static_cast<size_t>(y) * width + x) * 4;
            dst[0] = static_cast<BYTE>(sb);
            dst[1] = static_cast<BYTE>(sg);
            dst[2] = static_cast<BYTE>(sr);
            dst[3] = 255;
        }
    }

    int blurPasses = PopupBackdropFallbackBlurPasses();
    if (g_settings.allowScreenCapture && IsLiquidGlassMaterial()) {
        blurPasses = std::max(blurPasses, 3);
    }
    BoxBlurPbgraPixels(lowPixels, lowWidth, lowHeight, blurPasses);

    std::vector<BYTE> out(static_cast<size_t>(width) * height * 4);
    bool dark = IsDarkModeApprox();
    constexpr double kDarkDim = 0.94;
    constexpr double kLightDim = 1.00;
    double dim = dark ? kDarkDim : kLightDim;
    BYTE maxAlpha = PopupBackdropOverlayMaxAlpha();

    for (int y = 0; y < height; ++y) {
        double sampleY = ((static_cast<double>(y) + 0.5) * lowHeight / height) - 0.5;
        for (int x = 0; x < width; ++x) {
            double sampleX = ((static_cast<double>(x) + 0.5) * lowWidth / width) - 0.5;
            BYTE edgeAlpha = PopupRoundedRectAlpha(x, y, width, height, cornerRadiusPx);
            BYTE a = static_cast<BYTE>((static_cast<int>(edgeAlpha) * maxAlpha) / 255);

            int sb = 0;
            int sg = 0;
            int sr = 0;
            SampleBilinearPbgra(lowPixels, lowWidth, lowHeight, sampleX, sampleY, sb, sg, sr);

            int b = Clamp(static_cast<int>(std::lround(sb * dim)), 0, 255);
            int g = Clamp(static_cast<int>(std::lround(sg * dim)), 0, 255);
            int r = Clamp(static_cast<int>(std::lround(sr * dim)), 0, 255);

            BYTE* dst = out.data() + (static_cast<size_t>(y) * width + x) * 4;
            // UpdateLayeredWindow expects premultiplied BGRA for AC_SRC_ALPHA.
            dst[0] = static_cast<BYTE>((b * a) / 255);
            dst[1] = static_cast<BYTE>((g * a) / 255);
            dst[2] = static_cast<BYTE>((r * a) / 255);
            dst[3] = a;
        }
    }
    {
        std::lock_guard cacheLock(g_popupBackdropOverlayHandoffMutex);
        g_popupBackdropOverlayFallbackPixels = out;
        g_popupBackdropOverlayFallbackRect = screenRect;
        g_popupBackdropOverlayFallbackWidth = width;
        g_popupBackdropOverlayFallbackHeight = height;
        g_popupBackdropOverlayFallbackRadius = cornerRadiusPx;
        g_popupBackdropOverlayFallbackPixelsValid = true;
        g_popupBackdropOverlayCleanPixels = cleanPixels;
        g_popupBackdropOverlayCleanRect = screenRect;
        g_popupBackdropOverlayCleanWidth = width;
        g_popupBackdropOverlayCleanHeight = height;
        g_popupBackdropOverlayCleanPixelsValid = true;
    }

    HDC screenDc = GetDC(nullptr);
    if (!screenDc) {
        return false;
    }
    HDC memDc = CreateCompatibleDC(screenDc);
    if (!memDc) {
        ReleaseDC(nullptr, screenDc);
        return false;
    }

    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(screenDc, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap || !bits) {
        if (bitmap) DeleteObject(bitmap);
        DeleteDC(memDc);
        ReleaseDC(nullptr, screenDc);
        return false;
    }
    std::memcpy(bits, out.data(), out.size());

    HGDIOBJ oldBitmap = SelectObject(memDc, bitmap);
    POINT srcPoint{0, 0};
    SIZE size{width, height};
    POINT dstPoint{screenRect.left, screenRect.top};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

    BOOL ok = UpdateLayeredWindow(hwnd, screenDc, &dstPoint, &size,
                                  memDc, &srcPoint, 0, &blend, ULW_ALPHA);

    SelectObject(memDc, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(memDc);
    ReleaseDC(nullptr, screenDc);

    if (ok) {
        g_popupBackdropOverlayLastPaintTime = now;
        g_popupBackdropOverlayLastWidth = width;
        g_popupBackdropOverlayLastHeight = height;
        g_popupBackdropOverlayLastRadius = cornerRadiusPx;
    }
    return !!ok;
}

void PaintPopupBackdropOverlay(HWND hwnd) {
    PAINTSTRUCT paint{};
    HDC dc = BeginPaint(hwnd, &paint);
    if (dc) {
        EndPaint(hwnd, &paint);
    }
}

LRESULT CALLBACK PopupBackdropOverlayWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    try {
        switch (message) {
            case WM_ERASEBKGND:
                return 1;
            case WM_NCHITTEST:
                return HTTRANSPARENT;
            case WM_TIMER:
                if (wParam == kPopupBackdropOverlayTimerId) {
                    UpdatePopupBackdropOverlayWindow();
                    return 0;
                }
                break;
            case WM_PAINT:
                PaintPopupBackdropOverlay(hwnd);
                return 0;
            case WM_DESTROY:
                KillTimer(hwnd, kPopupBackdropOverlayTimerId);
                SetPopupWindowCaptureExclusion(hwnd, false);
                if (g_popupBackdropOverlay == hwnd) {
                    g_popupBackdropOverlay = nullptr;
                }
                return 0;
        }
    } catch (...) {
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

bool RegisterPopupBackdropOverlayClass() {
    if (g_popupBackdropOverlayClassRegistered) {
        return true;
    }
    HINSTANCE moduleInstance = ModInstance();
    if (!moduleInstance) {
        return false;
    }
    WNDCLASSEXW windowClass{sizeof(windowClass)};
    windowClass.lpfnWndProc = PopupBackdropOverlayWndProc;
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.hInstance = moduleInstance;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.lpszClassName = kPopupBackdropOverlayClassName;
    if (!RegisterClassExW(&windowClass)) {
        Wh_Log(L"Island: failed to register popup backdrop overlay window class");
        return false;
    }
    g_popupBackdropOverlayClassRegistered = true;
    return true;
}

void UnregisterPopupBackdropOverlayClass() {
    if (!g_popupBackdropOverlayClassRegistered) {
        return;
    }
    HINSTANCE moduleInstance = ModInstance();
    if (moduleInstance && UnregisterClassW(kPopupBackdropOverlayClassName, moduleInstance)) {
        g_popupBackdropOverlayClassRegistered = false;
    }
}

bool EnsurePopupBackdropOverlayWindow() {
    if (!UseOverlayPopupBackdropMaterial()) {
        return false;
    }
    if (g_popupBackdropOverlay) {
        return true;
    }
    if (!RegisterPopupBackdropOverlayClass()) {
        return false;
    }
    g_popupBackdropOverlay = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT |
            WS_EX_NOREDIRECTIONBITMAP,
        kPopupBackdropOverlayClassName, L"",
        WS_POPUP, 0, 0, 1, 1,
        g_taskbarWnd, nullptr, ModInstance(), nullptr);
    if (!g_popupBackdropOverlay) {
        Wh_Log(L"Island: failed to create popup backdrop overlay window");
        return false;
    }
    return true;
}


void HidePopupBackdropOverlayWindowVisualOnly() {
    StopPopupOverlayWgcBackdrop();
    if (g_popupBackdropOverlay) {
        SetPopupWindowCaptureExclusion(g_popupBackdropOverlay, false);
        SetWindowRgn(g_popupBackdropOverlay, nullptr, FALSE);
        ParkPopupWindow(g_popupBackdropOverlay);
        g_popupBackdropOverlayLastPaintTime = {};
        g_popupBackdropOverlayLastWidth = 0;
        g_popupBackdropOverlayLastHeight = 0;
        g_popupBackdropOverlayLastRadius = 0;
        g_popupLiquidGlassPanelRectPx = {};
        g_popupLiquidGlassPanelRadiusPx = 0;
        g_popupLiquidGlassPanelRectValid = false;
        g_popupBackdropOverlayNativeBlurActive = false;
        ClearPopupBackdropOverlayHandoffCache();
        if (g_expandedPopup) {
            SetExpandedPopupCaptureExclusion(false);
        }
    }
}

void HidePopupBackdropOverlayWindow() {
    if (g_popupBackdropOverlay) {
        ReleasePopupOverlayWgcDeviceResources();
        SetPopupWindowCaptureExclusion(g_popupBackdropOverlay, false);
        ClearPopupBackdropOverlayLayeredSurface(g_popupBackdropOverlay);
        SetWindowRgn(g_popupBackdropOverlay, nullptr, FALSE);
        ParkPopupWindow(g_popupBackdropOverlay);
        g_popupBackdropOverlayLastPaintTime = {};
        g_popupBackdropOverlayLastWidth = 0;
        g_popupBackdropOverlayLastHeight = 0;
        g_popupBackdropOverlayLastRadius = 0;
        g_popupLiquidGlassPanelRectPx = {};
        g_popupLiquidGlassPanelRadiusPx = 0;
        g_popupLiquidGlassPanelRectValid = false;
        g_popupOverlayWgcFallbackPainted = false;
        ClearPopupBackdropOverlayHandoffCache();
        if (g_expandedPopup) {
            SetExpandedPopupCaptureExclusion(false);
        }
    }
}

void DestroyPopupBackdropOverlayWindow() {
    if (g_popupBackdropOverlay) {
        StopPopupOverlayWgcBackdrop();
        SetPopupWindowCaptureExclusion(g_popupBackdropOverlay, false);
        ClearPopupBackdropOverlayLayeredSurface(g_popupBackdropOverlay);
        ClearPopupBackdropOverlayHandoffCache();
        SetWindowRgn(g_popupBackdropOverlay, nullptr, FALSE);
        DestroyWindow(g_popupBackdropOverlay);
        g_popupBackdropOverlay = nullptr;
    }
}


bool CalculatePopupBackdropOverlayRect(RECT& overlayRect, int& cornerRadiusPx) {
    if (!g_expandedPopup) {
        return false;
    }

    RECT popupScreenPx{};
    GetWindowRect(g_expandedPopup, &popupScreenPx);
    if (popupScreenPx.right <= popupScreenPx.left ||
        popupScreenPx.bottom <= popupScreenPx.top) {
        return false;
    }

    double dpiScale = PopupDpiScale(g_expandedPopup);
    RECT sourceRect = PopupScreenRectToLocalDip(g_popupSourceRect,
                                                popupScreenPx,
                                                dpiScale);
    RECT finalRect = PopupScreenRectToLocalDip(g_popupFinalRect,
                                               popupScreenPx,
                                               dpiScale);

    RECT targetArt{};
    RECT targetCard{};
    RECT targetTitle{};
    RECT targetArtist{};
    RECT targetProgress{};
    RECT targetElapsed{};
    RECT targetDuration{};
    RECT targetControls{};
    CalculatePopupFinalLayout(finalRect, targetArt, targetCard,
                              targetTitle, targetArtist,
                              targetProgress, targetElapsed,
                              targetDuration, targetControls);
    RECT targetBackdrop = PopupBackdropRectFromParts(targetArt, targetCard);

    double progress = PopupProgress();
    // Once the popup is visually at the end of the morph, snap the native
    // backdrop to the exact final XAML shell. This avoids a late 1-3 px WGC
    // handoff offset caused by starting capture at progress≈0.985.
    if (!g_popupClosing && progress >= 0.995) {
        progress = 1.0;
    }
    RECT currentDip{
        LerpInt(sourceRect.left, targetBackdrop.left, progress),
        LerpInt(sourceRect.top, targetBackdrop.top, progress),
        LerpInt(sourceRect.right, targetBackdrop.right, progress),
        LerpInt(sourceRect.bottom, targetBackdrop.bottom, progress),
    };

    // Keep the overlay geometry exactly aligned to the XAML backdrop shell.
    // The previous inset experiment helped hide hard native edges in some
    // cases, but it made the blur layer visibly disagree with the component
    // shape. For this test, prefer exact shape matching and remove all extra
    // XAML tint/gray layers instead.
    overlayRect = {
        popupScreenPx.left + static_cast<LONG>(std::lround(currentDip.left * dpiScale)),
        popupScreenPx.top + static_cast<LONG>(std::lround(currentDip.top * dpiScale)),
        popupScreenPx.left + static_cast<LONG>(std::lround(currentDip.right * dpiScale)),
        popupScreenPx.top + static_cast<LONG>(std::lround(currentDip.bottom * dpiScale)),
    };

    int width = overlayRect.right - overlayRect.left;
    int height = overlayRect.bottom - overlayRect.top;
    if (width <= 2 || height <= 2) {
        return false;
    }

    double targetRadiusDip =
        g_settings.compact
            ? PopupG2CornerRadius(targetBackdrop.right - targetBackdrop.left,
                                  targetBackdrop.bottom - targetBackdrop.top)
            : std::max(1.0,
                       kPopupUnifiedCornerRadius + kPopupOverlayRadiusAdjustDip);
    double sourceRadiusDip = SourceScaledRadius(g_layout.cornerRadius,
                                                g_layout.compactHeight,
                                                sourceRect);
    double radiusDip = LerpDouble(sourceRadiusDip,
                                  targetRadiusDip,
                                  progress);
    int desiredRadius = static_cast<int>(std::lround(radiusDip * dpiScale));
    cornerRadiusPx = Clamp(desiredRadius, 1, std::max(1, std::min(width, height) / 2));

    RECT currentCardDip{
        LerpInt(sourceRect.left, targetCard.left, progress),
        LerpInt(sourceRect.top, targetCard.top, progress),
        LerpInt(sourceRect.right, targetCard.right, progress),
        LerpInt(sourceRect.bottom, targetCard.bottom, progress),
    };
    RECT panelRect{
        static_cast<LONG>(std::lround(currentCardDip.left * dpiScale)) -
            static_cast<LONG>(std::lround(currentDip.left * dpiScale)),
        static_cast<LONG>(std::lround(currentCardDip.top * dpiScale)) -
            static_cast<LONG>(std::lround(currentDip.top * dpiScale)),
        static_cast<LONG>(std::lround(currentCardDip.right * dpiScale)) -
            static_cast<LONG>(std::lround(currentDip.left * dpiScale)),
        static_cast<LONG>(std::lround(currentCardDip.bottom * dpiScale)) -
            static_cast<LONG>(std::lround(currentDip.top * dpiScale)),
    };
    panelRect.left = Clamp<LONG>(panelRect.left, 0L, static_cast<LONG>(width));
    panelRect.top = Clamp<LONG>(panelRect.top, 0L, static_cast<LONG>(height));
    panelRect.right = Clamp<LONG>(panelRect.right, 0L, static_cast<LONG>(width));
    panelRect.bottom = Clamp<LONG>(panelRect.bottom, 0L, static_cast<LONG>(height));
    int panelWidth = static_cast<int>(panelRect.right - panelRect.left);
    int panelHeight = static_cast<int>(panelRect.bottom - panelRect.top);
    g_popupLiquidGlassPanelRectPx = panelRect;
    g_popupLiquidGlassPanelRadiusPx = Clamp(
        desiredRadius,
        1,
        std::max(1, std::min(panelWidth, panelHeight) / 2));
    g_popupLiquidGlassPanelRectValid = panelWidth > 4 && panelHeight > 4;
    return true;
}

bool CalculatePopupBackdropOverlayTargetSize(int& targetWidthPx,
                                             int& targetHeightPx,
                                             int& targetRadiusPx) {
    if (!g_expandedPopup) {
        return false;
    }

    RECT popupScreenPx{};
    GetWindowRect(g_expandedPopup, &popupScreenPx);
    if (popupScreenPx.right <= popupScreenPx.left ||
        popupScreenPx.bottom <= popupScreenPx.top) {
        return false;
    }

    double dpiScale = PopupDpiScale(g_expandedPopup);
    RECT finalRect = PopupScreenRectToLocalDip(g_popupFinalRect,
                                               popupScreenPx,
                                               dpiScale);

    RECT targetArt{};
    RECT targetCard{};
    RECT targetTitle{};
    RECT targetArtist{};
    RECT targetProgress{};
    RECT targetElapsed{};
    RECT targetDuration{};
    RECT targetControls{};
    CalculatePopupFinalLayout(finalRect, targetArt, targetCard,
                              targetTitle, targetArtist,
                              targetProgress, targetElapsed,
                              targetDuration, targetControls);
    RECT targetBackdrop = PopupBackdropRectFromParts(targetArt, targetCard);
    int targetWidthDip = std::max(1L, targetBackdrop.right - targetBackdrop.left);
    int targetHeightDip = std::max(1L, targetBackdrop.bottom - targetBackdrop.top);
    targetWidthPx = std::max(1, static_cast<int>(std::lround(targetWidthDip * dpiScale)));
    targetHeightPx = std::max(1, static_cast<int>(std::lround(targetHeightDip * dpiScale)));

    double targetRadiusDip =
        g_settings.compact
            ? PopupG2CornerRadius(targetBackdrop.right - targetBackdrop.left,
                                  targetBackdrop.bottom - targetBackdrop.top)
            : std::max(1.0,
                       kPopupUnifiedCornerRadius + kPopupOverlayRadiusAdjustDip);
    targetRadiusPx = Clamp(
        static_cast<int>(std::lround(targetRadiusDip * dpiScale)),
        1,
        std::max(1, std::min(targetWidthPx, targetHeightPx) / 2));
    return true;
}

void ApplyPopupBackdropOverlayRegion(int width, int height, int cornerRadiusPx) {
    if (!g_popupBackdropOverlay || width <= 0 || height <= 0) {
        return;
    }

    if (g_popupClosing) {
        HRGN region = CreateRoundRectRgn(0, 0, width + 1, height + 1,
                                         cornerRadiusPx * 2,
                                         cornerRadiusPx * 2);
        if (region) {
            if (!SetWindowRgn(g_popupBackdropOverlay, region, FALSE)) {
                DeleteObject(region);
            }
        }
        return;
    }

    // The layered overlay already carries a per-pixel rounded-rect alpha mask.
    // Keeping an additional Win32 HRGN here clips with a hard, non-AA boundary
    // and can create visible corner seams in the refractive material.
    SetWindowRgn(g_popupBackdropOverlay, nullptr, FALSE);
}

void UpdatePopupBackdropOverlayWindow() {
    if (!UseOverlayPopupBackdropMaterial()) {
        HidePopupBackdropOverlayWindow();
        return;
    }
    if (!g_expandedPopup || !g_expanded) {
        HidePopupBackdropOverlayWindowVisualOnly();
        return;
    }
    if (!IsWindowVisible(g_expandedPopup) && !g_popupClosing) {
        return;
    }
    if (!EnsurePopupBackdropOverlayWindow()) {
        return;
    }

    g_popupBackdropOverlayFrameAlpha.store(
        static_cast<int>(std::lround(PopupBackdropOverlayMaxAlpha() *
                                     PopupClosingShellOpacity(PopupProgress()))),
        std::memory_order_relaxed);

    RECT overlayRect{};
    int cornerRadiusPx = 1;
    if (!CalculatePopupBackdropOverlayRect(overlayRect, cornerRadiusPx)) {
        HidePopupBackdropOverlayWindowVisualOnly();
        return;
    }

    int width = std::max(1L, overlayRect.right - overlayRect.left);
    int height = std::max(1L, overlayRect.bottom - overlayRect.top);
    int renderWidth = width;
    int renderHeight = height;
    int renderRadiusPx = cornerRadiusPx;
    bool morphing =
        std::abs(g_popupAnimationTarget - g_popupAnimationProgress) >= 0.006;
    if (morphing) {
        int targetWidth = width;
        int targetHeight = height;
        int targetRadius = cornerRadiusPx;
        if (CalculatePopupBackdropOverlayTargetSize(targetWidth,
                                                    targetHeight,
                                                    targetRadius)) {
            renderWidth = targetWidth;
            renderHeight = targetHeight;
            renderRadiusPx = targetRadius;
        }
    }

    PopupOverlayWgcRenderParameters renderParameters;
    renderParameters.targetRadiusPx = renderRadiusPx;
    renderParameters.morphing = morphing;
    renderParameters.allowScreenCapture = g_settings.allowScreenCapture;
    renderParameters.useLiquidLensWarp = IsLiquidGlassMaterial();
    renderParameters.blurStdDev = PopupBackdropWgcEffectiveBlurStdDev();
    renderParameters.refractionBlurStdDev =
        PopupBackdropWgcLiquidRefractionBlurStdDev();
    renderParameters.liquidRimAlpha =
        IsDarkModeApprox() ? 0.38f : 0.52f;

    if (!g_settings.allowScreenCapture) {
        SetExpandedPopupCaptureExclusion(true);
        SetPopupWindowCaptureExclusion(g_popupBackdropOverlay, true);
    }

    bool overlayWasVisible = IsWindowVisible(g_popupBackdropOverlay) != FALSE;
    UINT overlayPosFlags = SWP_NOACTIVATE | SWP_NOCOPYBITS |
                           SWP_NOOWNERZORDER | SWP_NOSENDCHANGING;
    if (!overlayWasVisible) {
        overlayPosFlags |= SWP_SHOWWINDOW;
    }
    SetWindowPos(g_popupBackdropOverlay, g_expandedPopup,
                 overlayRect.left, overlayRect.top, width, height,
                 overlayPosFlags);
    ApplyPopupBackdropOverlayRegion(width, height, cornerRadiusPx);
    UpdatePopupOverlayDcompVisualTransform(width, height);
    g_popupBackdropOverlayLastWidth = width;
    g_popupBackdropOverlayLastHeight = height;
    g_popupBackdropOverlayLastRadius = cornerRadiusPx;
    if (!overlayWasVisible) {
        ShowWindow(g_popupBackdropOverlay, SW_SHOWNOACTIVATE);
    }

    if (g_settings.allowScreenCapture) {
        if (g_popupOverlayWgcRunning) {
            StopPopupOverlayWgcBackdrop();
        }
        LONG_PTR fallbackStyle = GetWindowLongPtrW(g_popupBackdropOverlay, GWL_EXSTYLE);
        LONG_PTR fallbackDesiredStyle =
            (fallbackStyle & ~WS_EX_NOREDIRECTIONBITMAP) |
            WS_EX_LAYERED | WS_EX_TRANSPARENT |
            WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW;
        if (fallbackDesiredStyle != fallbackStyle) {
            SetWindowLongPtrW(g_popupBackdropOverlay,
                              GWL_EXSTYLE,
                              fallbackDesiredStyle);
        }
        bool captureAffinityChanged = SetExpandedPopupCaptureExclusion(true);
        captureAffinityChanged =
            SetPopupWindowCaptureExclusion(g_popupBackdropOverlay, true) ||
            captureAffinityChanged;
        if (captureAffinityChanged) {
            DwmFlush();
        }
        bool forceSnapshot =
            !g_popupBackdropOverlayFallbackPixelsValid ||
            g_popupBackdropOverlayFallbackWidth != width ||
            g_popupBackdropOverlayFallbackHeight != height ||
            !EqualRect(&g_popupBackdropOverlayFallbackRect, &overlayRect);
        UpdatePopupBackdropOverlayLayeredBlur(g_popupBackdropOverlay,
                                              overlayRect,
                                              width,
                                              height,
                                              cornerRadiusPx,
                                              forceSnapshot);
        g_popupOverlayWgcFallbackPainted = true;
        SetPopupWindowCaptureExclusion(g_popupBackdropOverlay, false);
        SetExpandedPopupCaptureExclusion(false);
        return;
    }

    if (!g_popupOverlayWgcRunning && g_popupOverlayWgcFallbackPainted) {
        ClearPopupBackdropOverlayLayeredSurface(g_popupBackdropOverlay);
        g_popupOverlayWgcFallbackPainted = false;
    }
    LONG_PTR dcompStyle = GetWindowLongPtrW(g_popupBackdropOverlay, GWL_EXSTYLE);
    LONG_PTR dcompDesiredStyle =
        (dcompStyle & ~WS_EX_LAYERED) | WS_EX_TRANSPARENT |
        WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW |
        WS_EX_NOREDIRECTIONBITMAP;
    if (dcompDesiredStyle != dcompStyle) {
        SetWindowLongPtrW(g_popupBackdropOverlay,
                          GWL_EXSTYLE,
                          dcompDesiredStyle);
    }
    StartPopupOverlayWgcBackdrop(overlayRect,
                                 renderWidth,
                                 renderHeight,
                                 renderParameters);
    UpdatePopupOverlayDcompVisualTransform(width, height);
}

bool RegisterPopupWindowClass() {
    if (g_popupClassRegistered) {
        return true;
    }

    HINSTANCE moduleInstance = ModInstance();
    if (!moduleInstance) {
        Wh_Log(L"Island: failed to resolve the mod module handle");
        return false;
    }

    WNDCLASSEXW windowClass{sizeof(windowClass)};
    windowClass.lpfnWndProc = ExpandedPopupWndProc;
    windowClass.style = 0;
    windowClass.hInstance = moduleInstance;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.lpszClassName = kPopupClassName;
    if (!RegisterClassExW(&windowClass)) {
        Wh_Log(L"Island: failed to register expanded popup window class");
        return false;
    }

    g_popupClassRegistered = true;
    return true;
}

void UnregisterPopupWindowClass() {
    UnregisterPopupBackdropOverlayClass();
    if (!g_popupClassRegistered) {
        return;
    }

    HINSTANCE moduleInstance = ModInstance();
    if (moduleInstance && UnregisterClassW(kPopupClassName, moduleInstance)) {
        g_popupClassRegistered = false;
    }
}

bool EnsureExpandedPopup() {
    if (g_expandedPopup) {
        return true;
    }

    if (!RegisterPopupWindowClass()) {
        return false;
    }

    HWND popupOwner = g_taskbarWnd && IsWindow(g_taskbarWnd)
                          ? g_taskbarWnd
                          : FindCurrentProcessTaskbarWnd();
    RECT ownerRect{};
    int initialX = 0;
    int initialY = 0;
    if (popupOwner && GetWindowRect(popupOwner, &ownerRect)) {
        g_taskbarWnd = popupOwner;
        initialX = ownerRect.left;
        initialY = ownerRect.top;
    }
    int initialWidth =
        std::max(1, PopupDipToPx(g_layout.compactWidth, popupOwner));
    int initialHeight =
        std::max(1, PopupDipToPx(g_layout.compactHeight, popupOwner));

    g_expandedPopup = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_NOREDIRECTIONBITMAP,
        kPopupClassName, L"",
        WS_POPUP, initialX, initialY,
        initialWidth,
        initialHeight,
        popupOwner, nullptr, ModInstance(), nullptr);
    if (!g_expandedPopup) {
        return false;
    }
    if (!InitializePopupXamlHost(g_expandedPopup)) {
        auto style = GetWindowLongPtrW(g_expandedPopup, GWL_EXSTYLE);
        style &= ~static_cast<LONG_PTR>(WS_EX_NOREDIRECTIONBITMAP);
        SetWindowLongPtrW(g_expandedPopup, GWL_EXSTYLE, style | WS_EX_LAYERED);
    }
    ApplyPopupBackdrop(g_expandedPopup);
    return true;
}

void UpdatePlayerContents();

void ShowExpandedPopup() {
    g_expandedPopupCaptureExcluded = false;
    g_popupXamlChildCaptureExcluded = false;
    g_popupBackdropOverlayCaptureExcluded = false;
    if (!EnsureExpandedPopup()) {
        g_expanded = false;
        return;
    }

    g_expanded = true;
    g_popupClosing = false;
    SetPopupWindowClickThrough(g_expandedPopup, false);
    SetPopupWindowClickThrough(g_popupXamlChild, false);
    g_popupOutsideClickArmed = false;
    CapturePopupSourceGeometry();
    g_popupHostRect = UnionPopupRects(g_popupSourceRect, g_popupFinalRect);
    g_popupAnimationProgress = 0.0;
    g_popupAnimationTarget = 1.0;
    g_popupCurrentWidth = static_cast<double>(
        g_popupSourceRect.right - g_popupSourceRect.left);
    g_popupCurrentHeight = static_cast<double>(
        g_popupSourceRect.bottom - g_popupSourceRect.top);
    g_popupTargetWidth = g_popupExpandedWidth;
    g_popupTargetHeight = g_popupExpandedHeight;
    g_lastPopupVisualProgress = -1.0;
    g_lastPopupVisualRect = {LONG_MIN, LONG_MIN, LONG_MIN, LONG_MIN};
    g_lastPopupWindowRect = {LONG_MIN, LONG_MIN, LONG_MIN, LONG_MIN};
    g_popupPendingContentRefresh = false;
    if (g_popupXamlRoot) {
        UpdatePlayerContents();
    } else {
        UpdatePopupAlbumBitmap(SnapshotMedia());
    }
    ApplyPopupBackdrop(g_expandedPopup);
    PositionExpandedPopup();
    RenderExpandedPopupLayer();
    SetCompactIslandSuppressed(true);
    ShowWindow(g_expandedPopup, SW_SHOW);
    if (!g_settings.allowScreenCapture) {
        if (SetExpandedPopupCaptureExclusion(true)) {
            DwmFlush();
        }
    }
    // Re-apply after the popup becomes visible so the overlay is placed directly
    // under the popup in the active top-level z-order.
    UpdatePopupBackdropOverlayWindow();
    SetTimer(g_expandedPopup, kPopupTimerId, g_popupXamlRoot ? 30 : 15, nullptr);
    if (g_popupXamlRoot) {
        StartPopupXamlRenderLoop();
    }
}

void DestroyExpandedPopup() {
    StopPopupXamlRenderLoop();
    UnsubclassPopupXamlChildWindow();
    DestroyPopupBackdropOverlayWindow();
    SetCompactIslandSuppressed(false);
    if (g_popupXamlSource) {
        try {
            g_popupXamlSource.Content(nullptr);
            g_popupXamlSource.Close();
        } catch (...) {
        }
    }
    g_popupXamlSource = nullptr;
    g_popupXamlChild = nullptr;
    g_popupXamlChildSubclassed = false;
    g_popupXamlChildCaptureExcluded = false;
    g_popupXamlRoot = nullptr;
    g_popupXamlCanvas = nullptr;
    g_popupXamlShadow = nullptr;
    g_popupXamlShadowVisual = nullptr;
    g_popupXamlDropShadow = nullptr;
    g_popupXamlBackdrop = nullptr;
    g_popupXamlBackdropScale = nullptr;
    g_popupXamlBackdropTranslate = nullptr;
    g_popupXamlBackdropCoverFade = nullptr;
    g_popupXamlBackdropCover = nullptr;
    g_popupXamlBackdropTint = nullptr;
    g_popupXamlBackdropSurfaceHighlight = nullptr;
    g_popupXamlBackdropRimHighlight = nullptr;
    g_popupXamlPanelCoverFrame = nullptr;
    g_popupXamlPanelCoverScale = nullptr;
    g_popupXamlPanelCoverTranslate = nullptr;
    g_popupXamlPanelCoverFade = nullptr;
    g_popupXamlPanelCover = nullptr;
    g_popupXamlPanel = nullptr;
    g_popupXamlPanelScale = nullptr;
    g_popupXamlPanelTranslate = nullptr;
    g_popupXamlArtFrame = nullptr;
    g_popupXamlArtScale = nullptr;
    g_popupXamlArtTranslate = nullptr;
    g_popupXamlArtFade = nullptr;
    g_popupXamlArt = nullptr;
    g_popupXamlTitleHost = nullptr;
    g_popupXamlArtistHost = nullptr;
    g_popupXamlOutgoingTitleHost = nullptr;
    g_popupXamlOutgoingArtistHost = nullptr;
    g_popupXamlTitleLeftFade = nullptr;
    g_popupXamlTitleRightFade = nullptr;
    g_popupXamlArtistLeftFade = nullptr;
    g_popupXamlArtistRightFade = nullptr;
    g_popupXamlOutgoingTitleLeftFade = nullptr;
    g_popupXamlOutgoingTitleRightFade = nullptr;
    g_popupXamlOutgoingArtistLeftFade = nullptr;
    g_popupXamlOutgoingArtistRightFade = nullptr;
    g_popupXamlTitle = nullptr;
    g_popupXamlArtist = nullptr;
    g_popupXamlOutgoingTitle = nullptr;
    g_popupXamlOutgoingArtist = nullptr;
    g_popupXamlTitleTranslate = nullptr;
    g_popupXamlArtistTranslate = nullptr;
    g_popupXamlOutgoingTitleTranslate = nullptr;
    g_popupXamlOutgoingArtistTranslate = nullptr;
    g_popupTextBaseOpacity = 1.0;
    g_popupTextTransitionActive = false;
    g_popupXamlElapsed = nullptr;
    g_popupXamlDuration = nullptr;
    g_popupXamlProgress = nullptr;
    g_popupXamlProgressScale = nullptr;
    g_popupXamlProgressTrack = nullptr;
    g_popupXamlProgressFill = nullptr;
    g_popupXamlProgressHitTarget = nullptr;
    g_popupXamlProgressGlowMask = nullptr;
    g_popupXamlProgressGlowClip = nullptr;
    g_popupXamlProgressGlowLayers.clear();
    g_popupXamlProgressCoreBlurLayers.clear();
    g_popupXamlProgressGlowCore = nullptr;
    g_popupXamlControls = nullptr;
    g_popupXamlThumbnailHash = UINT64_MAX;
    g_popupXamlBackdropCoverEnabled = false;
    g_popupAccentThumbnailHash = UINT64_MAX;
    g_popupAccentColorValid = false;
    g_popupAccentColor = DefaultPopupAccentColor();
    g_popupDisplayedAccentColorValid = false;
    g_popupDisplayedAccentColor = DefaultPopupAccentColor();
    g_popupAccentTransitionFrom = DefaultPopupAccentColor();
    g_popupAccentTransitionTo = DefaultPopupAccentColor();
    g_popupMediaTransitionProgress = 1.0;
    g_popupMediaTransitionActive = false;
    g_popupCoverTransitionActive = false;
    g_popupTextTransitionActive = false;
    g_popupXamlThemeValid = false;
    g_popupXamlThemeMaterial.clear();
    g_popupXamlThemeShadowDepth = -1;
    g_popupXamlThemeShadowOpacity = -1;
    if (g_expandedPopup) {
        DestroyWindow(g_expandedPopup);
        g_expandedPopup = nullptr;
    }
    if (g_popupAlbumBitmap) {
        DeleteObject(g_popupAlbumBitmap);
        g_popupAlbumBitmap = nullptr;
    }
    g_popupThumbnailHash = 0;
    g_popupPendingContentRefresh = false;
    g_lastPopupVisualProgress = -1.0;
    g_lastPopupVisualRect = {LONG_MIN, LONG_MIN, LONG_MIN, LONG_MIN};
    g_lastPopupWindowRect = {LONG_MIN, LONG_MIN, LONG_MIN, LONG_MIN};
    g_expanded = false;
    g_popupXamlThemeValid = false;
    g_popupXamlThemeMaterial.clear();
    g_popupXamlThemeShadowDepth = -1;
    g_popupXamlThemeShadowOpacity = -1;
    UnregisterPopupWindowClass();
}

void UpdatePlayerContents();

void StopHoverRenderLoop() {
    if (g_hoverRenderingHooked) {
        try {
            mediax::CompositionTarget::Rendering(g_hoverRenderingToken);
        } catch (...) {
        }
        g_hoverRenderingHooked = false;
    }
}

void OnHoverRendering(winrt::Windows::Foundation::IInspectable const&,
                      winrt::Windows::Foundation::IInspectable const&) {
    try {
        if (!g_islandScale || g_unloading) {
            StopHoverRenderLoop();
            return;
        }

        auto now = std::chrono::steady_clock::now();
        double dtSec = 1.0 / 60.0;
        if (g_lastHoverFrameTime.time_since_epoch().count() != 0) {
            dtSec = std::chrono::duration_cast<std::chrono::microseconds>(
                        now - g_lastHoverFrameTime)
                        .count() /
                    1000000.0;
        }
        g_lastHoverFrameTime = now;
        dtSec = Clamp(dtSec, 0.0, 0.05);

        double alpha = 1.0 - std::exp(-g_settings.hoverLerpSpeed * dtSec);
        alpha = Clamp(alpha, 0.0, 1.0);
        g_currentHoverScale += (g_targetHoverScale - g_currentHoverScale) * alpha;

        if (std::abs(g_targetHoverScale - g_currentHoverScale) < 0.001) {
            g_currentHoverScale = g_targetHoverScale;
        }

        g_islandScale.ScaleX(g_currentHoverScale);
        g_islandScale.ScaleY(g_currentHoverScale);

        if (std::abs(g_targetHoverScale - g_currentHoverScale) < 0.001) {
            StopHoverRenderLoop();
            g_lastHoverFrameTime = {};
        }
    } catch (...) {
        StopHoverRenderLoop();
    }
}

void StartHoverRenderLoop(double targetScale) {
    g_targetHoverScale = targetScale;
    if (!g_islandScale) {
        return;
    }

    if (!g_hoverRenderingHooked) {
        g_lastHoverFrameTime = std::chrono::steady_clock::now();
        g_hoverRenderingToken = mediax::CompositionTarget::Rendering(OnHoverRendering);
        g_hoverRenderingHooked = true;
    }
}

void ApplyExpandedState() {
    if (!g_playerGrid) {
        return;
    }

    g_playerGrid.Width(g_layout.compactWidth);
    g_playerGrid.Height(g_layout.compactHeight);
    g_playerGrid.MinHeight(g_layout.compactHeight);

    if (auto backgroundFe = FindChildByName(g_playerGrid, L"Island_Background")) {
        if (auto background = backgroundFe.try_as<Border>()) {
            background.CornerRadius({g_layout.cornerRadius, g_layout.cornerRadius,
                                     g_layout.cornerRadius, g_layout.cornerRadius});
            background.BorderBrush(IslandBorderBrush());
        }
    }

    if (auto contentFe = FindChildByName(g_playerGrid, L"Island_Content")) {
        contentFe.Margin({g_layout.contentMarginX, g_layout.contentMarginY,
                          g_layout.contentMarginX, g_layout.contentMarginY});
        if (auto content = contentFe.try_as<Grid>()) {
            if (content.ColumnDefinitions().Size() > 0) {
                content.ColumnDefinitions().GetAt(0).Width(
                    {g_layout.artColumnWidth, GridUnitType::Pixel});
            }
        }
    }

    if (auto details = FindChildByName(g_playerGrid, L"Island_Details")) {
        details.Visibility(Visibility::Collapsed);
        details.IsHitTestVisible(false);
        details.Opacity(0.0);
    }
    if (auto compactArtist = FindChildByName(g_playerGrid, L"Island_CompactArtist")) {
        compactArtist.Visibility(Visibility::Visible);
        compactArtist.Opacity(1.0);
    }

    if (auto textHost = FindChildByName(g_playerGrid, L"Island_TextHost")) {
        textHost.Margin({g_layout.textMarginX, 0, g_layout.textMarginX, 0});
    }
    RefreshCompactTextHostClip(false);

    auto updateTextBlockSize = [](wchar_t const* name, double fontSize) {
        if (auto fe = FindChildByName(g_playerGrid, name)) {
            if (auto text = fe.try_as<TextBlock>()) {
                text.FontSize(fontSize);
            }
        }
    };
    updateTextBlockSize(L"Island_Title", g_layout.titleFontSize);
    updateTextBlockSize(L"Island_OutgoingTitle", g_layout.titleFontSize);
    updateTextBlockSize(L"Island_CompactArtist", g_layout.artistFontSize);
    updateTextBlockSize(L"Island_OutgoingCompactArtist", g_layout.artistFontSize);

    if (auto artFe = FindChildByName(g_playerGrid, L"Island_ArtFallback")) {
        artFe.Width(g_layout.artSize);
        artFe.Height(g_layout.artSize);
        if (auto art = artFe.try_as<Border>()) {
            art.CornerRadius({g_layout.artCornerRadius, g_layout.artCornerRadius,
                              g_layout.artCornerRadius, g_layout.artCornerRadius});
        }
    }
    if (auto artShellFe = FindChildByName(g_playerGrid, L"Island_ArtShell")) {
        artShellFe.Width(g_layout.artSize);
        artShellFe.Height(g_layout.artSize);
    }
    if (auto artHostFe = FindChildByName(g_playerGrid, L"Island_ArtHost")) {
        artHostFe.Width(g_layout.artSize);
        artHostFe.Height(g_layout.artSize);
        ApplyElementClip(artHostFe, g_layout.artSize, g_layout.artSize);
    }
    if (auto placeholderFe = FindChildByName(g_playerGrid, L"Island_ArtPlaceholder")) {
        placeholderFe.Width(g_layout.artSize);
        placeholderFe.Height(g_layout.artSize);
        if (auto placeholder = placeholderFe.try_as<Border>()) {
            placeholder.CornerRadius({g_layout.artCornerRadius, g_layout.artCornerRadius,
                                      g_layout.artCornerRadius, g_layout.artCornerRadius});
        }
    }
    if (auto imageFe = FindChildByName(g_playerGrid, L"Island_AlbumArt")) {
        imageFe.Width(g_layout.artImageSize);
        imageFe.Height(g_layout.artImageSize);
        imageFe.Margin({-1, -1, -1, -1});
    }
    if (auto fadeFe = FindChildByName(g_playerGrid, L"Island_AlbumArtFade")) {
        fadeFe.Width(g_layout.artImageSize);
        fadeFe.Height(g_layout.artImageSize);
        fadeFe.Margin({-1, -1, -1, -1});
    }
    if (auto strokeFe = FindChildByName(g_playerGrid, L"Island_ArtStroke")) {
        if (auto stroke = strokeFe.try_as<Border>()) {
            ConfigureCompactAlbumArtStroke(stroke);
        }
    }

    if (g_compactProgress) {
        g_compactProgress.Width(g_layout.progressWidth);
        g_compactProgress.Height(g_layout.progressHeight);
        g_compactProgress.Margin({g_layout.progressMarginLeft, 0, 0, 0});
    }
}

void EnsureCompactIslandInteractive() {
    if (!g_playerGrid || g_unloading) {
        return;
    }
    if (g_expanded || g_popupClosing) {
        return;
    }

    try {
        if (g_popupBackdropOverlay && IsWindowVisible(g_popupBackdropOverlay)) {
            HidePopupBackdropOverlayWindowVisualOnly();
        }
        if (g_expandedPopup && IsWindow(g_expandedPopup)) {
            ParkExpandedPopupWindow(g_expandedPopup);
        }
        g_playerGrid.Opacity(1.0);
        g_playerGrid.IsHitTestVisible(true);
    } catch (...) {
    }
}
void RepairExpandedPopupStateIfHidden() {
    if (!g_expanded || g_popupClosing) {
        return;
    }
    if (g_expandedPopup && IsWindowVisible(g_expandedPopup)) {
        return;
    }

    StopPopupXamlRenderLoop();
    g_expanded = false;
    g_popupClosing = false;
    g_popupAnimationProgress = 0.0;
    g_popupAnimationTarget = 0.0;
    g_popupOutsideClickArmed = false;
    HidePopupBackdropOverlayWindowVisualOnly();
    SetCompactIslandSuppressed(false);
    try {
        if (g_playerGrid) {
            g_playerGrid.Opacity(1.0);
            g_playerGrid.IsHitTestVisible(true);
            ApplyExpandedState();
            UpdatePlayerContents();
        }
    } catch (...) {
    }
}
void StartExpandRenderLoop(bool expanded) {
    RepairExpandedPopupStateIfHidden();
    ApplyExpandedState();
    if (expanded) {
        StopHoverRenderLoop();
        g_currentHoverScale = 1.0;
        g_targetHoverScale = 1.0;
        if (g_islandScale) {
            g_islandScale.ScaleX(1.0);
            g_islandScale.ScaleY(1.0);
        }
        if (g_playerGrid) {
            g_playerGrid.UpdateLayout();
            RefreshCompactTextHostClip(false);
        }
        ShowExpandedPopup();
    } else {
        BeginCloseExpandedPopup();
    }
}

bool InjectIslandGrid();

void MediaThreadProc() {
    struct MediaThreadExitGuard {
        ~MediaThreadExitGuard() {
            g_mediaThreadExited = true;
        }
    } mediaThreadExitGuard;

    bool apartmentInitialized = false;
    try {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        apartmentInitialized = true;
    } catch (...) {
        Wh_Log(L"Island: failed to initialize media thread apartment");
        g_mediaThreadRunning = false;
        return;
    }

    struct SessionEventSubscription {
        gsm::GlobalSystemMediaTransportControlsSession session{nullptr};
        winrt::event_token mediaPropertiesChangedToken{};
        winrt::event_token playbackInfoChangedToken{};
        winrt::event_token timelinePropertiesChangedToken{};
    };

    gsm::GlobalSystemMediaTransportControlsSessionManager manager{nullptr};
    std::vector<SessionEventSubscription> sessionEventSubscriptions;
    std::atomic_bool sessionListChanged = true;
    winrt::event_token sessionsChangedToken{};
    winrt::event_token currentSessionChangedToken{};

    auto clearSessionEvents = [&] {
        for (auto& subscription : sessionEventSubscriptions) {
            if (subscription.session) {
                try {
                    subscription.session.MediaPropertiesChanged(
                        subscription.mediaPropertiesChangedToken);
                } catch (...) {
                }
                try {
                    subscription.session.PlaybackInfoChanged(
                        subscription.playbackInfoChangedToken);
                } catch (...) {
                }
                try {
                    subscription.session.TimelinePropertiesChanged(
                        subscription.timelinePropertiesChangedToken);
                } catch (...) {
                }
            }
        }
        sessionEventSubscriptions.clear();
    };

    auto subscribeAllSessionEvents = [&] {
        clearSessionEvents();
        try {
            if (!manager) {
                return;
            }

            for (auto const& session : manager.GetSessions()) {
                SessionEventSubscription subscription;
                subscription.session = session;
                try {
                    subscription.mediaPropertiesChangedToken =
                        session.MediaPropertiesChanged(
                            [](auto const&, auto const&) {
                                RequestMediaRefresh();
                            });
                    subscription.playbackInfoChangedToken =
                        session.PlaybackInfoChanged(
                            [](auto const&, auto const&) {
                                RequestMediaRefresh();
                            });
                    subscription.timelinePropertiesChangedToken =
                        session.TimelinePropertiesChanged(
                            [](auto const&, auto const&) {
                                RequestMediaRefresh();
                            });
                    sessionEventSubscriptions.push_back(
                        std::move(subscription));
                } catch (...) {
                    try {
                        session.MediaPropertiesChanged(
                            subscription.mediaPropertiesChangedToken);
                    } catch (...) {
                    }
                    try {
                        session.PlaybackInfoChanged(
                            subscription.playbackInfoChangedToken);
                    } catch (...) {
                    }
                    try {
                        session.TimelinePropertiesChanged(
                            subscription.timelinePropertiesChangedToken);
                    } catch (...) {
                    }
                }
            }
        } catch (...) {
            clearSessionEvents();
        }
    };

    try {
        manager = RequestMediaManagerWithTimeout(std::chrono::seconds(3));
        if (manager) {
            sessionsChangedToken = manager.SessionsChanged(
                [&](auto const&, auto const&) {
                    sessionListChanged = true;
                    RequestMediaRefresh();
                });
            currentSessionChangedToken = manager.CurrentSessionChanged(
                [](auto const&, auto const&) { RequestMediaRefresh(); });
            subscribeAllSessionEvents();
            sessionListChanged = false;
        }
    } catch (...) {
        manager = nullptr;
    }

    auto lastFallbackPoll = std::chrono::steady_clock::time_point{};
    g_mediaRefreshRequested = true;

    while (g_mediaThreadRunning) {
        std::deque<MediaCommand> commands;
        {
            std::lock_guard lock(g_mediaCommandMutex);
            commands.swap(g_mediaCommands);
        }

        if (!commands.empty()) {
            // Resolve a fresh manager/session for commands. Apple Music can keep
            // reporting through a long-lived GSMTC session while rejecting
            // control requests issued through that stale session object.
            auto session = CurrentSession();
            if (session) {
                for (auto& command : commands) {
                    try {
                        command(session);
                    } catch (...) {
                    }
                }
            }
            g_mediaRefreshRequested = true;
        }

        auto now = std::chrono::steady_clock::now();
        bool fallbackPollDue =
            lastFallbackPoll.time_since_epoch().count() == 0 ||
            now - lastFallbackPoll >= std::chrono::seconds(30);
        bool shouldRefresh = g_mediaRefreshRequested.exchange(false) ||
                             fallbackPollDue;

        if (shouldRefresh) {
            if (sessionListChanged.exchange(false)) {
                subscribeAllSessionEvents();
            }
            RefreshMediaState(manager);
            lastFallbackPoll = now;

            HWND hwnd = g_taskbarWnd;
            if (!hwnd || !IsWindow(hwnd)) {
                hwnd = FindCurrentProcessTaskbarWnd();
            }
            if (IsModActive() && hwnd) {
                RunFromWindowThread(
                    hwnd,
                    [](void* param) {
                        if (!IsModActive()) {
                            return;
                        }
                        HWND taskbarWnd = reinterpret_cast<HWND>(param);
                        if (!g_playerGrid) {
                            g_taskbarWnd = taskbarWnd;
                            InjectIslandGrid();
                        } else {
                            UpdatePlayerContents();
                        }
                    },
                    reinterpret_cast<void*>(hwnd));
            }
        }

        std::unique_lock lock(g_mediaCommandMutex);
        g_mediaCommandCv.wait_for(
            lock, std::chrono::seconds(30), [] {
                return !g_mediaThreadRunning || !g_mediaCommands.empty() ||
                       g_mediaRefreshRequested.load();
            });
    }

    clearSessionEvents();
    try {
        if (manager) {
            manager.SessionsChanged(sessionsChangedToken);
            manager.CurrentSessionChanged(currentSessionChangedToken);
        }
    } catch (...) {
    }

    // Release the GSMTC manager before tearing down the WinRT apartment. Keeping
    // the WinRT/COM object alive across winrt::uninit_apartment() can leave its
    // final Release() running after the COM apartment has already been torn down,
    // which is risky when the mod is unloaded inside explorer.exe.
    manager = nullptr;

    if (apartmentInitialized) {
        winrt::uninit_apartment();
    }
}

void StartMediaThread() {
    if (g_mediaThreadRunning.exchange(true)) {
        return;
    }
    g_mediaThreadExited = false;
    try {
        g_mediaThread = new std::thread(MediaThreadProc);
    } catch (...) {
        g_mediaThreadRunning = false;
        g_mediaThreadExited = true;
        g_mediaThread = nullptr;
        Wh_Log(L"Island: failed to start media thread");
    }
}

void StopMediaThread() {
    g_mediaThreadRunning = false;
    g_mediaRefreshRequested = false;
    g_mediaCommandCv.notify_all();
    if (g_mediaThread) {
        if (g_mediaThread->joinable()) {
            if (g_mediaThread->get_id() != std::this_thread::get_id()) {
                auto deadline =
                    std::chrono::steady_clock::now() +
                    kMediaThreadStopTimeout;
                while (!g_mediaThreadExited.load() &&
                       std::chrono::steady_clock::now() < deadline) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(20));
                }

                if (g_mediaThreadExited.load()) {
                    g_mediaThread->join();
                } else {
                    Wh_Log(L"Island: media thread stop timed out; detaching");
                    g_mediaThread->detach();
                }
            } else {
                g_mediaThread->detach();
            }
        }
        delete g_mediaThread;
        g_mediaThread = nullptr;
    }
    std::lock_guard lock(g_mediaCommandMutex);
    g_mediaCommands.clear();
}

TextBlock MakeTextBlock(const wchar_t* name, double fontSize, bool semibold) {
    TextBlock text;
    text.Name(name);
    text.FontFamily(mediax::FontFamily(L"Segoe UI Variable Text"));
    text.FontSize(fontSize);
    text.FontWeight(semibold ? winrt::Windows::UI::Text::FontWeights::SemiBold()
                             : winrt::Windows::UI::Text::FontWeights::Normal());
    text.Foreground(Brush(IsDarkModeApprox() ? Color(0xF2, 0xFF, 0xFF, 0xFF)
                                             : Color(0xF2, 0x18, 0x18, 0x1B)));
    text.TextTrimming(xaml::TextTrimming::CharacterEllipsis);
    text.VerticalAlignment(VerticalAlignment::Center);
    return text;
}

Button MakeMediaButton(const wchar_t* name, const wchar_t* glyph, void (*onClick)()) {
    Button button;
    button.Name(name);
    button.Width(32);
    button.Height(28);
    button.Padding({0, 0, 0, 0});
    button.Margin({2, 0, 2, 0});
    button.Background(Brush(Color(0x18, 0xFF, 0xFF, 0xFF)));
    button.BorderBrush(CompactPlaybackControlStrokeBrush());
    button.BorderThickness({1, 1, 1, 1});
    button.CornerRadius({kPopupG2ButtonCornerRadius,
                         kPopupG2ButtonCornerRadius,
                         kPopupG2ButtonCornerRadius,
                         kPopupG2ButtonCornerRadius});

    TextBlock icon = MakeTextBlock(L"", 13, true);
    icon.FontFamily(mediax::FontFamily(L"Segoe Fluent Icons"));
    icon.Text(glyph);
    icon.HorizontalAlignment(HorizontalAlignment::Center);
    button.Content(icon);
    button.Click([onClick](auto const&, auto const&) { onClick(); });
    return button;
}

void UpdateButtonTheme(const wchar_t* name) {
    if (!g_playerGrid) {
        return;
    }

    bool dark = IsDarkModeApprox();
    if (auto buttonFe = FindChildByName(g_playerGrid, name)) {
        if (auto button = buttonFe.try_as<Button>()) {
            button.Background(Brush(dark ? Color(0x18, 0xFF, 0xFF, 0xFF)
                                         : Color(0x14, 0x00, 0x00, 0x00)));
            button.BorderBrush(CompactPlaybackControlStrokeBrush());
            if (auto icon = button.Content().try_as<TextBlock>()) {
                icon.Foreground(Brush(dark ? Color(0xF2, 0xFF, 0xFF, 0xFF)
                                           : Color(0xF2, 0x18, 0x18, 0x1B)));
            }
        }
    }
}

void UpdateThemeVisuals() {
    if (!g_playerGrid) {
        return;
    }

    bool dark = IsDarkModeApprox();
    if (g_themeVisualsValid && g_lastDarkMode == dark) {
        return;
    }
    g_themeVisualsValid = true;
    g_lastDarkMode = dark;

    if (auto bgFe = FindChildByName(g_playerGrid, L"Island_Background")) {
        if (auto bg = bgFe.try_as<Border>()) {
            bg.Background(IslandBackgroundBrush());
            bg.BorderBrush(IslandBorderBrush());
        }
    }
    auto primaryTextBrush = Brush(dark ? Color(0xF2, 0xFF, 0xFF, 0xFF)
                                       : Color(0xF2, 0x18, 0x18, 0x1B));
    auto secondaryTextBrush = Brush(dark ? Color(0xB8, 0xFF, 0xFF, 0xFF)
                                         : Color(0xB8, 0x1C, 0x1C, 0x20));
    if (auto titleFe = FindChildByName(g_playerGrid, L"Island_Title")) {
        if (auto title = titleFe.try_as<TextBlock>()) {
            title.Foreground(primaryTextBrush);
        }
    }
    if (auto outgoingTitleFe = FindChildByName(g_playerGrid, L"Island_OutgoingTitle")) {
        if (auto title = outgoingTitleFe.try_as<TextBlock>()) {
            title.Foreground(primaryTextBrush);
        }
    }
    if (auto artistFe = FindChildByName(g_playerGrid, L"Island_CompactArtist")) {
        if (auto artist = artistFe.try_as<TextBlock>()) {
            artist.Foreground(secondaryTextBrush);
        }
    }
    if (auto outgoingArtistFe = FindChildByName(g_playerGrid, L"Island_OutgoingCompactArtist")) {
        if (auto artist = outgoingArtistFe.try_as<TextBlock>()) {
            artist.Foreground(secondaryTextBrush);
        }
    }
    if (g_compactTextLeftFade) {
        g_compactTextLeftFade.Background(CompactTextEdgeFadeBrush(true));
    }
    if (g_compactTextRightFade) {
        g_compactTextRightFade.Background(CompactTextEdgeFadeBrush(false));
    }

    if (auto strokeFe = FindChildByName(g_playerGrid, L"Island_ArtStroke")) {
        if (auto stroke = strokeFe.try_as<Border>()) {
            ConfigureCompactAlbumArtStroke(stroke);
        }
    }

    UpdateButtonTheme(L"Island_Prev");
    UpdateButtonTheme(L"Island_Play");
    UpdateButtonTheme(L"Island_Next");
}

double EffectiveElementVisualHeightDip(FrameworkElement const& element,
                                       double referenceHeight) {
    if (!element) {
        return 0.0;
    }

    try {
        double height = element.ActualHeight();
        if (height <= 1.0) {
            return 0.0;
        }

        // Taskbar Styler themes often compress the *visual* taskbar by adding
        // positive top/bottom margins to high-level taskbar grids while the
        // outer layout slot keeps the normal taskbar height. If ActualHeight
        // still looks like the reference/root height, subtract those positive
        // margins to estimate the real visual content height. If XAML already
        // reduced ActualHeight because of the margin, do not subtract again.
        Thickness margin = element.Margin();
        double positiveVerticalMargin =
            std::max(0.0, margin.Top) + std::max(0.0, margin.Bottom);
        if (referenceHeight > 1.0 && positiveVerticalMargin > 0.0 &&
            height > referenceHeight - positiveVerticalMargin * 0.5) {
            height = std::max(1.0, height - positiveVerticalMargin);
        }

        return height;
    } catch (...) {
        return 0.0;
    }
}

double DetectTaskbarHeightDip(HWND hwnd,
                              FrameworkElement const& root,
                              FrameworkElement const& targetElement) {
    double rootHeight = 0.0;
    try {
        if (root) {
            rootHeight = root.ActualHeight();
        }
    } catch (...) {
        rootHeight = 0.0;
    }

    double bestVisualHeight = 0.0;
    auto consider = [&](FrameworkElement const& element) {
        double height = EffectiveElementVisualHeightDip(
            element, rootHeight > 1.0 ? rootHeight : 0.0);
        if (height >= 24.0) {
            bestVisualHeight =
                bestVisualHeight > 1.0 ? std::min(bestVisualHeight, height)
                                        : height;
        }
    };

    // The injection target can be a full-height layout grid. Use it, but also
    // sample the tray grid because themes such as Matter/BottomDensy often put
    // their visual compression there.
    consider(targetElement);
    if (root) {
        consider(FindChildByName(root, L"SystemTrayFrameGrid"));
        consider(FindChildByName(root, L"TaskbarFrame"));
    }

    double height = bestVisualHeight;
    if (height <= 1.0 && rootHeight > 1.0) {
        height = rootHeight;
    }

    if (height <= 1.0 && hwnd) {
        RECT rc{};
        if (GetWindowRect(hwnd, &rc)) {
            UINT dpi = GetDpiForWindow(hwnd);
            double scale = dpi > 0 ? static_cast<double>(dpi) / 96.0 : 1.0;
            height = static_cast<double>(rc.bottom - rc.top) /
                     std::max(0.01, scale);
        }
    }

    return Clamp(height, 28.0, 96.0);
}

void UpdateRuntimeLayout(HWND hwnd,
                         FrameworkElement const& root,
                         FrameworkElement const& targetElement) {
    double taskbarHeight = DetectTaskbarHeightDip(hwnd, root, targetElement);
    g_layout.taskbarHeightDip = taskbarHeight;

    double compactHeight = static_cast<double>(g_settings.height);
    double compactWidth = static_cast<double>(g_settings.compactWidth);
    if (g_settings.autoSizeToTaskbar) {
        // Use the detected *visual* height, not the outer Shell_TrayWnd height.
        // For smaller themed taskbars, never enlarge beyond the user's configured
        // Height/CompactWidth; for genuinely tall taskbars, allow moderate growth.
        double autoHeight = Clamp(taskbarHeight - 8.0, 28.0, 46.0);
        double autoScale = autoHeight / 40.0;
        double autoWidth = Clamp(168.0 * autoScale, 112.0, 220.0);

        if (taskbarHeight <= 50.0) {
            compactHeight = std::min(static_cast<double>(g_settings.height),
                                     autoHeight);
            compactWidth = std::min(static_cast<double>(g_settings.compactWidth),
                                    autoWidth);
        } else {
            compactHeight = autoHeight;
            compactWidth = autoWidth;
        }
    }

    double scale = Clamp(compactHeight / 40.0, 0.68, 1.20);
    g_layout.compactHeight = compactHeight;
    g_layout.compactWidth = compactWidth;
    g_layout.artSize = Clamp(compactHeight - 12.0, 18.0, 34.0);
    g_layout.artImageSize = g_layout.artSize + 2.0;
    g_layout.artCornerRadius = Clamp(g_layout.artSize * 0.29, 5.0, 10.0);
    g_layout.cornerRadius = Clamp(compactHeight * 0.50, 12.0, 23.0);
    g_layout.contentMarginX = Clamp(10.0 * scale, 6.0, 12.0);
    g_layout.contentMarginY = Clamp(4.0 * scale, 2.0, 5.0);
    g_layout.artColumnWidth = g_layout.artImageSize;
    g_layout.textMarginX = Clamp(8.0 * scale, 4.0, 10.0);
    g_layout.titleFontSize = Clamp(12.0 * scale, 10.0, 13.5);
    g_layout.artistFontSize = Clamp(10.0 * scale, 8.5, 11.5);
    g_layout.progressWidth = Clamp(74.0 * scale, 42.0, 92.0);
    g_layout.progressHeight = Clamp(4.0 * scale, 2.5, 5.0);
    g_layout.progressMarginLeft = Clamp(8.0 * scale, 4.0, 10.0);
}

Grid BuildIslandGrid() {
    Grid wrapper;
    wrapper.Name(L"IslandMedia_Wrapper");
    wrapper.Tag(winrt::box_value(winrt::hstring(L"IslandMediaControls")));
    wrapper.Width(g_layout.compactWidth);
    wrapper.Height(g_layout.compactHeight);
    wrapper.MinHeight(g_layout.compactHeight);
    wrapper.VerticalAlignment(VerticalAlignment::Center);
    wrapper.HorizontalAlignment(HorizontalAlignment::Left);
    wrapper.Margin({static_cast<double>(g_settings.marginLeft), 0,
                    static_cast<double>(g_settings.marginRight), 0});
    wrapper.Background(Brush(Color(0x01, 0, 0, 0)));
    wrapper.RenderTransformOrigin({0.5, 1.0});

    ScaleTransform hoverScale;
    hoverScale.ScaleX(1.0);
    hoverScale.ScaleY(1.0);
    wrapper.RenderTransform(hoverScale);
    g_islandScale = hoverScale;
    g_currentHoverScale = 1.0;
    g_targetHoverScale = 1.0;

    Border background;
    background.Name(L"Island_Background");
    background.CornerRadius({g_layout.cornerRadius, g_layout.cornerRadius, g_layout.cornerRadius, g_layout.cornerRadius});
    background.Background(IslandBackgroundBrush());
    background.BorderBrush(IslandBorderBrush());
    background.BorderThickness({1, 1, 1, 1});
    background.IsHitTestVisible(false);
    controls::Canvas::SetZIndex(background, 0);

    Grid content;
    content.Name(L"Island_Content");
    content.IsHitTestVisible(false);
    content.Margin({g_layout.contentMarginX, g_layout.contentMarginY, g_layout.contentMarginX, g_layout.contentMarginY});
    controls::Canvas::SetZIndex(content, 1);

    ColumnDefinition artCol;
    artCol.Width({g_layout.artColumnWidth, GridUnitType::Pixel});
    ColumnDefinition textCol;
    textCol.Width({1, GridUnitType::Star});
    ColumnDefinition controlsCol;
    controlsCol.Width({1, GridUnitType::Auto});
    content.ColumnDefinitions().Append(artCol);
    content.ColumnDefinitions().Append(textCol);
    content.ColumnDefinitions().Append(controlsCol);

    Border art;
    art.Name(L"Island_ArtFallback");
    art.Width(g_layout.artSize);
    art.Height(g_layout.artSize);
    art.CornerRadius({g_layout.artCornerRadius, g_layout.artCornerRadius, g_layout.artCornerRadius, g_layout.artCornerRadius});
    // Keep the parent transparent. The old blue fallback could show through as a
    // 1px strip on the right/bottom when the generated cover was rounded/scaled.
    art.Background(Brush(Color(0x00, 0x00, 0x00, 0x00)));
    art.BorderThickness({0, 0, 0, 0});
    art.BorderBrush(Brush(Color(0x00, 0x00, 0x00, 0x00)));
    art.VerticalAlignment(VerticalAlignment::Center);

    Grid artShell;
    artShell.Name(L"Island_ArtShell");
    artShell.Width(g_layout.artSize);
    artShell.Height(g_layout.artSize);

    Grid artHost;
    artHost.Name(L"Island_ArtHost");
    artHost.Width(g_layout.artSize);
    artHost.Height(g_layout.artSize);
    ApplyElementClip(artHost, g_layout.artSize, g_layout.artSize);
    artShell.Children().Append(artHost);

    Border artPlaceholder;
    artPlaceholder.Name(L"Island_ArtPlaceholder");
    artPlaceholder.Width(g_layout.artSize);
    artPlaceholder.Height(g_layout.artSize);
    artPlaceholder.CornerRadius({g_layout.artCornerRadius, g_layout.artCornerRadius, g_layout.artCornerRadius, g_layout.artCornerRadius});
    artPlaceholder.Background(Brush(Color(0xFF, 0x4F, 0x7D, 0xE8)));
    artPlaceholder.BorderThickness({0, 0, 0, 0});
    artPlaceholder.IsHitTestVisible(false);
    artHost.Children().Append(artPlaceholder);

    Image artFade;
    artFade.Name(L"Island_AlbumArtFade");
    artFade.Width(g_layout.artImageSize);
    artFade.Height(g_layout.artImageSize);
    artFade.Margin({-1, -1, -1, -1});
    artFade.Stretch(mediax::Stretch::UniformToFill);
    artFade.IsHitTestVisible(false);
    artFade.Opacity(0.0);
    artHost.Children().Append(artFade);

    Image artImage;
    artImage.Name(L"Island_AlbumArt");
    artImage.Width(g_layout.artImageSize);
    artImage.Height(g_layout.artImageSize);
    artImage.Margin({-1, -1, -1, -1});
    artImage.Stretch(mediax::Stretch::UniformToFill);
    artImage.IsHitTestVisible(false);
    artImage.Opacity(1.0);
    artHost.Children().Append(artImage);

    Border artStroke;
    artStroke.Name(L"Island_ArtStroke");
    ConfigureCompactAlbumArtStroke(artStroke);
    artShell.Children().Append(artStroke);

    art.Child(artShell);
    g_compactAlbumArtImage = artImage;
    g_compactAlbumArtFade = artFade;

    controls::Grid::SetColumn(art, 0);
    content.Children().Append(art);

    Grid textHost;
    textHost.Name(L"Island_TextHost");
    textHost.VerticalAlignment(VerticalAlignment::Center);
    textHost.Margin({g_layout.textMarginX, 0, g_layout.textMarginX, 0});
    controls::Grid::SetColumn(textHost, 1);

    StackPanel outgoingTextStack;
    outgoingTextStack.Orientation(controls::Orientation::Vertical);
    outgoingTextStack.IsHitTestVisible(false);
    outgoingTextStack.VerticalAlignment(VerticalAlignment::Center);
    outgoingTextStack.Opacity(1.0);

    auto outgoingTitle = MakeTextBlock(L"Island_OutgoingTitle", g_layout.titleFontSize, true);
    outgoingTitle.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    outgoingTitle.Opacity(0.0);
    TranslateTransform outgoingTitleTranslate;
    outgoingTitleTranslate.X(0.0);
    outgoingTitle.RenderTransform(outgoingTitleTranslate);

    auto outgoingArtist = MakeTextBlock(L"Island_OutgoingCompactArtist", g_layout.artistFontSize, false);
    outgoingArtist.Foreground(Brush(IsDarkModeApprox() ? Color(0xB8, 0xFF, 0xFF, 0xFF)
                                                       : Color(0xB8, 0x1C, 0x1C, 0x20)));
    outgoingArtist.Opacity(0.0);
    TranslateTransform outgoingArtistTranslate;
    outgoingArtistTranslate.X(0.0);
    outgoingArtist.RenderTransform(outgoingArtistTranslate);

    outgoingTextStack.Children().Append(outgoingTitle);
    outgoingTextStack.Children().Append(outgoingArtist);
    textHost.Children().Append(outgoingTextStack);

    StackPanel textStack;
    textStack.Orientation(controls::Orientation::Vertical);
    textStack.IsHitTestVisible(false);
    textStack.VerticalAlignment(VerticalAlignment::Center);

    auto title = MakeTextBlock(L"Island_Title", g_layout.titleFontSize, true);
    title.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    TranslateTransform titleTranslate;
    titleTranslate.X(0.0);
    title.RenderTransform(titleTranslate);
    auto artist = MakeTextBlock(L"Island_CompactArtist", g_layout.artistFontSize, false);
    artist.Foreground(Brush(IsDarkModeApprox() ? Color(0xB8, 0xFF, 0xFF, 0xFF)
                                               : Color(0xB8, 0x1C, 0x1C, 0x20)));
    TranslateTransform artistTranslate;
    artistTranslate.X(0.0);
    artist.RenderTransform(artistTranslate);
    g_compactTitleText = title;
    g_compactArtistText = artist;
    g_compactTextHost = textHost;
    g_compactOutgoingTitleText = outgoingTitle;
    g_compactOutgoingArtistText = outgoingArtist;
    g_compactTitleTranslate = titleTranslate;
    g_compactArtistTranslate = artistTranslate;
    g_compactOutgoingTitleTranslate = outgoingTitleTranslate;
    g_compactOutgoingArtistTranslate = outgoingArtistTranslate;
    g_compactTextInitialized = false;
    g_compactLastTitle.clear();
    g_compactLastArtist.clear();
    g_compactLastTextMediaHash = UINT64_MAX;
    textStack.Children().Append(title);
    textStack.Children().Append(artist);
    textHost.Children().Append(textStack);

    Border leftTextFade;
    leftTextFade.Name(L"Island_TextLeftFade");
    leftTextFade.HorizontalAlignment(HorizontalAlignment::Left);
    leftTextFade.VerticalAlignment(VerticalAlignment::Stretch);
    leftTextFade.IsHitTestVisible(false);
    leftTextFade.Opacity(0.0);
    controls::Canvas::SetZIndex(leftTextFade, 10);
    Border rightTextFade;
    rightTextFade.Name(L"Island_TextRightFade");
    rightTextFade.HorizontalAlignment(HorizontalAlignment::Right);
    rightTextFade.VerticalAlignment(VerticalAlignment::Stretch);
    rightTextFade.IsHitTestVisible(false);
    rightTextFade.Opacity(0.0);
    controls::Canvas::SetZIndex(rightTextFade, 10);
    g_compactTextLeftFade = leftTextFade;
    g_compactTextRightFade = rightTextFade;
    textHost.Children().Append(leftTextFade);
    textHost.Children().Append(rightTextFade);

    content.Children().Append(textHost);

    StackPanel details;
    details.Name(L"Island_Details");
    details.Orientation(controls::Orientation::Horizontal);
    details.VerticalAlignment(VerticalAlignment::Center);
    details.Visibility(Visibility::Collapsed);
    details.IsHitTestVisible(false);
    details.Opacity(0.0);
    controls::Grid::SetColumn(details, 2);

    details.Children().Append(MakeMediaButton(
        L"Island_Prev", L"\uE892",
        [] {
            RunMediaCommand([](gsm::GlobalSystemMediaTransportControlsSession const& s) {
                NoteMediaNavigationDirection(-1);
                GetAsyncResultWithTimeout(s.TrySkipPreviousAsync(),
                                           kMediaCommandAsyncTimeout,
                                           L"TrySkipPreviousAsync");
            });
        }));
    details.Children().Append(MakeMediaButton(
        L"Island_Play", L"\uE768",
        [] {
            RunMediaCommand([](gsm::GlobalSystemMediaTransportControlsSession const& s) {
                GetAsyncResultWithTimeout(s.TryTogglePlayPauseAsync(),
                                           kMediaCommandAsyncTimeout,
                                           L"TryTogglePlayPauseAsync");
            });
        }));
    details.Children().Append(MakeMediaButton(
        L"Island_Next", L"\uE893",
        [] {
            RunMediaCommand([](gsm::GlobalSystemMediaTransportControlsSession const& s) {
                NoteMediaNavigationDirection(1);
                GetAsyncResultWithTimeout(s.TrySkipNextAsync(),
                                           kMediaCommandAsyncTimeout,
                                           L"TrySkipNextAsync");
            });
        }));

    ProgressBar progress;
    progress.Name(L"Island_Progress");
    progress.Width(g_layout.progressWidth);
    progress.Height(g_layout.progressHeight);
    progress.Margin({g_layout.progressMarginLeft, 0, 0, 0});
    progress.Minimum(0);
    progress.Maximum(1000);
    progress.Value(0);
    progress.IsHitTestVisible(false);
    g_compactProgress = progress;
    details.Children().Append(progress);
    content.Children().Append(details);

    wrapper.Children().Append(background);
    wrapper.Children().Append(content);

    wrapper.PointerEntered([hoverScale](auto const&, auto const&) {
        g_islandScale = hoverScale;
        StartHoverRenderLoop(g_settings.hoverScale);
    });
    wrapper.PointerExited([hoverScale](auto const&, auto const&) {
        g_islandScale = hoverScale;
        StartHoverRenderLoop(1.0);
    });
    wrapper.PointerPressed([](auto const& sender, input::PointerRoutedEventArgs const& e) {
        auto source = sender.template try_as<UIElement>();
        auto point = e.GetCurrentPoint(source);
        if (point.Properties().IsLeftButtonPressed()) {
            RepairExpandedPopupStateIfHidden();
            bool popupActuallyExpanded =
                g_expanded && g_expandedPopup &&
                IsWindowVisible(g_expandedPopup) && !g_popupClosing;
            StartExpandRenderLoop(!popupActuallyExpanded);
            e.Handled(true);
        }
    });

    return wrapper;
}

void StopTaskbarLayoutMonitor() {
    if (g_taskbarLayoutTimerWindow) {
        KillTimer(g_taskbarLayoutTimerWindow, kTaskbarLayoutMonitorTimerId);
        g_taskbarLayoutTimerWindow = nullptr;
    }
    g_taskbarLayoutWatchRoot = nullptr;
    g_taskbarLayoutWatchTarget = nullptr;
}

void CALLBACK OnTaskbarLayoutTimer(HWND, UINT, UINT_PTR timerId, DWORD) {
    try {
        if (timerId != kTaskbarLayoutMonitorTimerId) {
            return;
        }
        if (g_unloading || !g_playerGrid || !g_taskbarLayoutWatchRoot ||
            !g_taskbarLayoutWatchTarget) {
            StopTaskbarLayoutMonitor();
            return;
        }

        RepairExpandedPopupStateIfHidden();

        double oldHeight = g_layout.compactHeight;
        double oldWidth = g_layout.compactWidth;
        double oldTaskbarHeight = g_layout.taskbarHeightDip;

        HWND hwnd = g_taskbarWnd && IsWindow(g_taskbarWnd)
                        ? g_taskbarWnd
                        : FindCurrentProcessTaskbarWnd();
        if (!hwnd) {
            return;
        }
        g_taskbarWnd = hwnd;

        bool taskbarDirectionChanged = false;
        RECT taskbarRect{};
        if (GetWindowRect(hwnd, &taskbarRect)) {
            HMONITOR monitor = MonitorFromRect(
                &taskbarRect, MONITOR_DEFAULTTONEAREST);
            MONITORINFO monitorInfo{sizeof(monitorInfo)};
            if (monitor && GetMonitorInfoW(monitor, &monitorInfo)) {
                bool taskbarAtTop =
                    TaskbarPopupOpensDown(taskbarRect, monitorInfo);
                taskbarDirectionChanged = taskbarAtTop != g_taskbarAtTop;
                if (taskbarDirectionChanged) {
                    g_taskbarAtTop = taskbarAtTop;
                    g_popupXamlThumbnailHash = UINT64_MAX;
                    RequestMediaRefresh();
                }
            }
        }

        UpdateRuntimeLayout(hwnd, g_taskbarLayoutWatchRoot,
                            g_taskbarLayoutWatchTarget);

        bool layoutChanged =
            std::abs(g_layout.compactHeight - oldHeight) > 0.5 ||
            std::abs(g_layout.compactWidth - oldWidth) > 0.5 ||
            std::abs(g_layout.taskbarHeightDip - oldTaskbarHeight) > 0.75;

        if (layoutChanged || taskbarDirectionChanged) {
            ApplyExpandedState();
            try {
                g_playerGrid.UpdateLayout();
            } catch (...) {
            }
            RefreshCompactTextHostClip(false);
            if (!g_expanded) {
                CapturePopupSourceGeometry();
            }
        }
    } catch (...) {
        StopTaskbarLayoutMonitor();
    }
}

void StartTaskbarLayoutMonitor(FrameworkElement const& root,
                               FrameworkElement const& targetElement) {
    if (g_unloading || !root || !targetElement) {
        return;
    }

    StopTaskbarLayoutMonitor();
    g_taskbarLayoutWatchRoot = root;
    g_taskbarLayoutWatchTarget = targetElement;

    HWND hwnd = g_taskbarWnd && IsWindow(g_taskbarWnd)
                    ? g_taskbarWnd
                    : FindCurrentProcessTaskbarWnd();
    if (!hwnd ||
        !SetTimer(hwnd,
                  kTaskbarLayoutMonitorTimerId,
                  kTaskbarLayoutMonitorIntervalMs,
                  OnTaskbarLayoutTimer)) {
        g_taskbarLayoutWatchRoot = nullptr;
        g_taskbarLayoutWatchTarget = nullptr;
        return;
    }
    g_taskbarLayoutTimerWindow = hwnd;
}

void UpdatePlayerContents() {
    if (!g_playerGrid || g_unloading) {
        return;
    }

    try {
    MediaState state = SnapshotMedia();

    // Some providers briefly report metadata before the matching thumbnail is
    // available. Reuse old artwork only when the *same media item* temporarily
    // has an empty thumbnail. Do not carry a previous song/video cover into a
    // new browser live room, where GSMTC often reports no thumbnail or repeats
    // the previous thumbnail while the page is still updating.
    auto artworkNow = std::chrono::steady_clock::now();
    std::wstring artworkIdentityKey = MediaIdentityKey(state);
    uint64_t rawThumbnailHash = ThumbnailHash(state.thumbnailBytes);

    static std::vector<uint8_t> s_lastNonEmptyThumbnailBytes;
    static std::wstring s_lastNonEmptyThumbnailIdentityKey;
    static uint64_t s_lastNonEmptyThumbnailHash = 0;
    static std::chrono::steady_clock::time_point s_lastNonEmptyThumbnailTime{};
    static std::wstring s_suspectRepeatedThumbnailIdentityKey;
    static uint64_t s_suspectRepeatedThumbnailHash = 0;
    static std::chrono::steady_clock::time_point s_suspectRepeatedThumbnailSince{};

    std::vector<uint8_t> visualThumbnailBytes = state.thumbnailBytes;
    if (!state.hasSession) {
        visualThumbnailBytes.clear();
        s_suspectRepeatedThumbnailIdentityKey.clear();
        s_suspectRepeatedThumbnailHash = 0;
    } else if (!state.thumbnailBytes.empty()) {
        bool differentMedia =
            !s_lastNonEmptyThumbnailIdentityKey.empty() &&
            artworkIdentityKey != s_lastNonEmptyThumbnailIdentityKey;
        bool repeatedPreviousArtwork =
            differentMedia &&
            rawThumbnailHash != 0 &&
            rawThumbnailHash == s_lastNonEmptyThumbnailHash;
        bool browserLiveLike =
            LooksLikeBrowserMediaSource(state.sourceAppUserModelId) &&
            state.durationTicks <= 0;

        if (repeatedPreviousArtwork && browserLiveLike) {
            if (s_suspectRepeatedThumbnailIdentityKey != artworkIdentityKey ||
                s_suspectRepeatedThumbnailHash != rawThumbnailHash) {
                s_suspectRepeatedThumbnailIdentityKey = artworkIdentityKey;
                s_suspectRepeatedThumbnailHash = rawThumbnailHash;
                s_suspectRepeatedThumbnailSince = artworkNow;
            }

            // Give browser/GSMTC a short chance to replace a stale thumbnail.
            // During that window show the placeholder instead of the previous
            // video's cover. If it never changes, accept it later so the UI
            // doesn't stay placeholder forever.
            auto suspectAge =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    artworkNow - s_suspectRepeatedThumbnailSince)
                    .count();
            if (suspectAge < 3500) {
                visualThumbnailBytes.clear();
            }
        } else {
            s_suspectRepeatedThumbnailIdentityKey.clear();
            s_suspectRepeatedThumbnailHash = 0;
        }

        if (!visualThumbnailBytes.empty()) {
            s_lastNonEmptyThumbnailBytes = state.thumbnailBytes;
            s_lastNonEmptyThumbnailIdentityKey = artworkIdentityKey;
            s_lastNonEmptyThumbnailHash = rawThumbnailHash;
            s_lastNonEmptyThumbnailTime = artworkNow;
        }
    } else if (!s_lastNonEmptyThumbnailBytes.empty() &&
               artworkIdentityKey == s_lastNonEmptyThumbnailIdentityKey) {
        auto age =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                artworkNow - s_lastNonEmptyThumbnailTime)
                .count();
        if (age <= 2500) {
            visualThumbnailBytes = s_lastNonEmptyThumbnailBytes;
        } else {
            visualThumbnailBytes.clear();
        }
    } else {
        visualThumbnailBytes.clear();
    }

    std::vector<uint8_t> displayThumbnailBytes =
        CreateDisplayAlbumCoverBytes(visualThumbnailBytes,
                                     state.sourceAppUserModelId);
    bool useGeneratedBlurCover =
        LooksLikeBrowserMediaSource(state.sourceAppUserModelId) &&
        ShouldUseAbstractArtworkForDisplay(visualThumbnailBytes) &&
        !displayThumbnailBytes.empty();
    std::vector<uint8_t> popupBlurSourceBytes =
        useGeneratedBlurCover ? displayThumbnailBytes : visualThumbnailBytes;

    UpdateThemeVisuals();

    auto makeBitmap = [](std::vector<uint8_t> const& bytes) -> imaging::BitmapImage {
        imaging::BitmapImage bitmap;
        if (bytes.empty()) {
            return imaging::BitmapImage{nullptr};
        }
        streams::InMemoryRandomAccessStream stream;
        streams::DataWriter writer(stream);
        writer.WriteBytes(winrt::array_view<const uint8_t>(bytes));
        try {
            GetAsyncResultWithTimeout(writer.StoreAsync(),
                                      kUiLocalAsyncTimeout,
                                      L"DataWriter.StoreAsync");
        } catch (...) {
            return imaging::BitmapImage{nullptr};
        }
        writer.DetachStream();
        stream.Seek(0);
        bitmap.SetSourceAsync(stream);
        return bitmap;
    };

    std::wstring compactTitle = state.hasSession ? state.title : std::wstring(L"No media");
    std::wstring compactArtist = state.artist.empty()
                                     ? std::wstring(state.isPlaying ? L"Playing" : L"Paused")
                                     : state.artist;
    uint64_t compactTextMediaHash = ThumbnailHash(visualThumbnailBytes);
    bool compactWasInitialized = g_compactTextInitialized;
    std::wstring compactOldTitle = g_compactLastTitle.empty() ? compactTitle : g_compactLastTitle;
    std::wstring compactOldArtist = g_compactLastArtist.empty() ? compactArtist : g_compactLastArtist;
    bool compactTextChanged =
        compactWasInitialized &&
        (compactTitle != g_compactLastTitle ||
         compactArtist != g_compactLastArtist ||
         (compactTextMediaHash != 0 && compactTextMediaHash != g_compactLastTextMediaHash));
    bool compactPlaybackOnlyTextChange =
        compactWasInitialized &&
        compactTitle == g_compactLastTitle &&
        state.artist.empty() &&
        (compactArtist == L"Playing" || compactArtist == L"Paused") &&
        (g_compactLastArtist == L"Playing" || g_compactLastArtist == L"Paused") &&
        compactTextMediaHash == g_compactLastTextMediaHash;
    if (compactPlaybackOnlyTextChange) {
        compactTextChanged = false;
    }

    if (auto titleFe = FindChildByName(g_playerGrid, L"Island_Title")) {
        if (auto title = titleFe.try_as<TextBlock>()) {
            title.Text(winrt::hstring(compactTitle));
        }
    }
    if (auto artistFe = FindChildByName(g_playerGrid, L"Island_CompactArtist")) {
        if (auto artist = artistFe.try_as<TextBlock>()) {
            artist.Text(winrt::hstring(compactArtist));
        }
    }

    if (!g_compactTextInitialized) {
        g_compactTextInitialized = true;
        ResetCompactTextAnimationVisuals();
    }
    g_compactLastTitle = compactTitle;
    g_compactLastArtist = compactArtist;
    g_compactLastTextMediaHash = compactTextMediaHash;
    if (auto playFe = FindChildByName(g_playerGrid, L"Island_Play")) {
        if (auto button = playFe.try_as<Button>()) {
            if (auto icon = button.Content().try_as<TextBlock>()) {
                icon.Text(state.isPlaying ? L"\uE769" : L"\uE768");
            }
        }
    }
    if (g_compactProgress) {
        bool shouldInterpolate = UpdateCompactProgressFromSnapshot();
        if (shouldInterpolate && !g_expanded) {
            StartCompactProgressRenderLoop();
        } else {
            StopCompactProgressRenderLoop();
        }
    }
    bool compactArtChanged = false;
    uint64_t compactArtHash = ThumbnailHash(displayThumbnailBytes);
    if (auto imageFe = FindChildByName(g_playerGrid, L"Island_AlbumArt")) {
        if (auto image = imageFe.try_as<Image>()) {
            if (!g_compactAlbumArtImage) {
                g_compactAlbumArtImage = image;
            }
            compactArtChanged = compactArtHash != g_lastThumbnailHash;
            if (compactArtChanged) {
                bool canAnimateArt = compactWasInitialized && !g_expanded && !g_unloading;
                try {
                    if (canAnimateArt && g_compactAlbumArtFade) {
                        auto oldSource = image.Source();
                        g_compactAlbumArtFade.Source(oldSource);
                        g_compactAlbumArtFade.Opacity(oldSource != nullptr ? 1.0 : 0.0);
                    }
                    if (compactArtHash) {
                        image.Source(makeBitmap(displayThumbnailBytes));
                    } else {
                        image.Source(nullptr);
                    }
                    image.Opacity(canAnimateArt ? 0.0 : 1.0);
                } catch (...) {
                    image.Source(nullptr);
                    image.Opacity(1.0);
                    if (g_compactAlbumArtFade) {
                        g_compactAlbumArtFade.Source(nullptr);
                        g_compactAlbumArtFade.Opacity(0.0);
                    }
                }
                g_lastThumbnailHash = compactArtHash;
            }
        }
    }

    if (compactWasInitialized && !g_expanded && (compactTextChanged || compactArtChanged)) {
        StartCompactTrackTransition(compactOldTitle, compactOldArtist,
                                    compactTextChanged, compactArtChanged);
    }

    if (g_popupXamlRoot) {
        if (g_popupRenderingHooked) {
            g_popupPendingContentRefresh = true;
        } else {
            std::vector<uint8_t> const& accentSourceBytes =
                displayThumbnailBytes.empty() ? visualThumbnailBytes
                                              : displayThumbnailBytes;
            uint64_t accentHash = ThumbnailHash(accentSourceBytes);
            bool accentChanged = accentHash != g_popupAccentThumbnailHash;
            winrt::Windows::UI::Color nextAccent = accentSourceBytes.empty()
                                                     ? DefaultPopupAccentColor()
                                                     : ExtractAlbumAccentColor(accentSourceBytes);
            if (accentChanged) {
                winrt::Windows::UI::Color currentAccent = PopupAccentColor();
                bool canAnimateAccent = g_popupAccentThumbnailHash != UINT64_MAX &&
                                        g_popupXamlRoot &&
                                        g_expandedPopup &&
                                        IsWindowVisible(g_expandedPopup);
                g_popupAccentThumbnailHash = accentHash;
                g_popupAccentColor = nextAccent;
                g_popupAccentColorValid = !accentSourceBytes.empty();
                g_popupAccentTransitionFrom = canAnimateAccent ? currentAccent : nextAccent;
                g_popupAccentTransitionTo = nextAccent;
                g_popupDisplayedAccentColor = canAnimateAccent ? currentAccent : nextAccent;
                g_popupDisplayedAccentColorValid = true;
                if (canAnimateAccent) {
                    g_popupMediaTransitionProgress = 0.0;
                    g_popupMediaTransitionActive = true;
                }
                g_popupXamlThemeValid = false;
            }
            ApplyPopupXamlTheme();
            winrt::hstring nextPopupTitle = state.hasSession ? winrt::hstring(state.title) : winrt::hstring(L"No media");
            winrt::hstring nextPopupArtist = state.artist.empty()
                                                 ? (state.isPlaying ? winrt::hstring(L"Playing")
                                                                    : winrt::hstring(L"Paused"))
                                                 : winrt::hstring(state.artist);
            bool popupTextChanged = g_popupXamlTitle && g_popupXamlArtist &&
                                    (g_popupXamlTitle.Text() != nextPopupTitle ||
                                     g_popupXamlArtist.Text() != nextPopupArtist);
            bool popupPlaybackOnlyTextChange =
                popupTextChanged &&
                g_popupXamlTitle &&
                g_popupXamlArtist &&
                g_popupXamlTitle.Text() == nextPopupTitle &&
                state.artist.empty() &&
                (nextPopupArtist == winrt::hstring(L"Playing") ||
                 nextPopupArtist == winrt::hstring(L"Paused")) &&
                (g_popupXamlArtist.Text() == winrt::hstring(L"Playing") ||
                 g_popupXamlArtist.Text() == winrt::hstring(L"Paused"));
            bool canAnimatePopupText = popupTextChanged &&
                                       !popupPlaybackOnlyTextChange &&
                                       g_popupXamlOutgoingTitle &&
                                       g_popupXamlOutgoingArtist &&
                                       g_expandedPopup &&
                                       IsWindowVisible(g_expandedPopup);
            if (canAnimatePopupText) {
                g_popupXamlOutgoingTitle.Text(g_popupXamlTitle.Text());
                g_popupXamlOutgoingArtist.Text(g_popupXamlArtist.Text());
                int navigationDirection = ConsumeRecentMediaNavigationDirection();
                g_popupTextTransitionDirection = navigationDirection < 0 ? -1 : 1;
                double popupTextStartOffset = 30.0 * static_cast<double>(g_popupTextTransitionDirection);
                if (g_popupXamlOutgoingTitleTranslate) g_popupXamlOutgoingTitleTranslate.X(0.0);
                if (g_popupXamlOutgoingArtistTranslate) g_popupXamlOutgoingArtistTranslate.X(0.0);
                g_popupXamlOutgoingTitle.Opacity(g_popupTextBaseOpacity);
                g_popupXamlOutgoingArtist.Opacity(g_popupTextBaseOpacity);
                if (g_popupXamlTitleTranslate) g_popupXamlTitleTranslate.X(popupTextStartOffset);
                if (g_popupXamlArtistTranslate) g_popupXamlArtistTranslate.X(popupTextStartOffset);
                g_popupXamlTitle.Opacity(0.0);
                g_popupXamlArtist.Opacity(0.0);
                g_popupTextTransitionActive = true;
                g_popupMediaTransitionProgress = 0.0;
                g_popupMediaTransitionActive = true;
            } else if (!popupTextChanged) {
                // Leave the current transition state alone if only artwork/accent is changing.
            } else {
                g_popupTextTransitionActive = false;
            }
            if (g_popupXamlTitle) g_popupXamlTitle.Text(nextPopupTitle);
            if (g_popupXamlArtist) g_popupXamlArtist.Text(nextPopupArtist);
            if (!canAnimatePopupText && !g_popupTextTransitionActive) {
                if (g_popupXamlTitleTranslate) g_popupXamlTitleTranslate.X(0.0);
                if (g_popupXamlArtistTranslate) g_popupXamlArtistTranslate.X(0.0);
                if (g_popupXamlOutgoingTitle) g_popupXamlOutgoingTitle.Opacity(0.0);
                if (g_popupXamlOutgoingArtist) g_popupXamlOutgoingArtist.Opacity(0.0);
            }
            UpdatePopupLiveProgressFromSnapshot();
            if (g_popupXamlControls) {
                for (auto const& child : g_popupXamlControls.Children()) {
                    if (auto buttonSurface = child.try_as<Border>()) {
                        UpdatePopupTransportButtonGlyph(buttonSurface, state.isPlaying);
                    }
                }
            }

            uint64_t popupHash = ThumbnailHash(
                displayThumbnailBytes.empty() ? visualThumbnailBytes
                                              : displayThumbnailBytes);
            bool useBackdropCover = PopupBackdropCoverEffectEnabled();
            if (popupHash != g_popupXamlThumbnailHash ||
                g_popupXamlThumbnailMaterial != g_settings.material ||
                g_popupXamlThumbnailCompact != g_settings.compact ||
                useBackdropCover != g_popupXamlBackdropCoverEnabled) {
                bool canAnimateCover = g_popupXamlThumbnailHash != UINT64_MAX &&
                                       g_popupXamlRoot &&
                                       g_expandedPopup &&
                                       IsWindowVisible(g_expandedPopup);
                try {
                    if (canAnimateCover) {
                        if (g_popupXamlArtFade && g_popupXamlArt) {
                            auto oldArtSource = g_popupXamlArt.Source();
                            g_popupXamlArtFade.Source(oldArtSource);
                            g_popupXamlArtFade.Opacity(oldArtSource != nullptr ? 1.0 : 0.0);
                        }
                        if (g_popupXamlBackdropCoverFade && g_popupXamlBackdropCover) {
                            auto oldBackdropCoverSource = useBackdropCover
                                                              ? g_popupXamlBackdropCover.Source()
                                                              : nullptr;
                            g_popupXamlBackdropCoverFade.Source(oldBackdropCoverSource);
                            g_popupXamlBackdropCoverFade.Opacity(oldBackdropCoverSource != nullptr ? 1.0 : 0.0);
                        }
                        if (g_popupXamlPanelCoverFade && g_popupXamlPanelCover) {
                            auto oldCoverSource = g_popupXamlPanelCover.Source();
                            g_popupXamlPanelCoverFade.Source(oldCoverSource);
                            g_popupXamlPanelCoverFade.Opacity(oldCoverSource != nullptr ? 1.0 : 0.0);
                        }
                    }

                    if (visualThumbnailBytes.empty()) {
                        auto placeholderArtBytes = CreatePlaceholderAlbumCoverBytes(128);
                        auto placeholderG2ArtBytes = CreatePopupG2AlbumCoverBytes(placeholderArtBytes);
                        auto placeholderPanelBytes = CreatePlaceholderAlbumCoverBytes(20);
                        auto placeholderBackdropBytes =
                            CreatePlaceholderAlbumCoverBytes(
                                20,
                                true,
                                g_settings.compact || g_taskbarAtTop,
                                false);
                        auto const& popupArtBytes = placeholderG2ArtBytes.empty()
                                                        ? placeholderArtBytes
                                                        : placeholderG2ArtBytes;
                        if (!popupArtBytes.empty()) {
                            g_popupXamlArt.Source(makeBitmap(popupArtBytes));
                        } else {
                            g_popupXamlArt.Source(nullptr);
                        }
                        if (g_popupXamlBackdropCover) {
                            if (useBackdropCover && !placeholderBackdropBytes.empty()) {
                                g_popupXamlBackdropCover.Source(makeBitmap(placeholderBackdropBytes));
                            } else {
                                g_popupXamlBackdropCover.Source(nullptr);
                            }
                        }
                        if (g_popupXamlPanelCover) {
                            if (!placeholderPanelBytes.empty()) {
                                g_popupXamlPanelCover.Source(makeBitmap(placeholderPanelBytes));
                            } else {
                                g_popupXamlPanelCover.Source(nullptr);
                            }
                        }
                    } else {
                        auto popupArtBytes = CreatePopupG2AlbumCoverBytes(displayThumbnailBytes);
                        g_popupXamlArt.Source(makeBitmap(
                            popupArtBytes.empty() ? displayThumbnailBytes : popupArtBytes));
                        if (g_popupXamlBackdropCover) {
                            if (useBackdropCover) {
                                auto lowDetailBackdropBytes =
                                    CreateLowDetailAlbumCoverBytes(
                                        popupBlurSourceBytes,
                                        true,
                                        g_settings.compact || g_taskbarAtTop,
                                        false);
                                auto const& backdropBytes = lowDetailBackdropBytes.empty()
                                                                ? popupBlurSourceBytes
                                                                : lowDetailBackdropBytes;
                                g_popupXamlBackdropCover.Source(makeBitmap(backdropBytes));
                            } else {
                                g_popupXamlBackdropCover.Source(nullptr);
                            }
                        }
                        if (g_popupXamlPanelCover) {
                            auto lowDetailCoverBytes = CreateLowDetailAlbumCoverBytes(popupBlurSourceBytes);
                            auto const& coverBytes = lowDetailCoverBytes.empty()
                                                         ? popupBlurSourceBytes
                                                         : lowDetailCoverBytes;
                            g_popupXamlPanelCover.Source(makeBitmap(coverBytes));
                        }
                    }

                    if (canAnimateCover) {
                        g_popupCoverTransitionActive = true;
                        g_popupMediaTransitionProgress = 0.0;
                        g_popupMediaTransitionActive = true;
                        ApplyPopupMediaTransitionVisuals();
                    } else {
                        g_popupCoverTransitionActive = false;
                        if (g_popupXamlArt) g_popupXamlArt.Opacity(1.0);
                        if (g_popupXamlArtFade) g_popupXamlArtFade.Opacity(0.0);
                        if (g_popupXamlBackdropCover) {
                            g_popupXamlBackdropCover.Opacity(useBackdropCover ? 1.0 : 0.0);
                        }
                        if (g_popupXamlBackdropCoverFade) g_popupXamlBackdropCoverFade.Opacity(0.0);
                        if (g_popupXamlPanelCover) g_popupXamlPanelCover.Opacity(1.0);
                        if (g_popupXamlPanelCoverFade) g_popupXamlPanelCoverFade.Opacity(0.0);
                    }
                    g_popupXamlThumbnailHash = popupHash;
                    g_popupXamlThumbnailMaterial = g_settings.material;
                    g_popupXamlThumbnailCompact = g_settings.compact;
                    g_popupXamlBackdropCoverEnabled = useBackdropCover;
                } catch (...) {
                    g_popupXamlArt.Source(nullptr);
                    if (g_popupXamlArtFade) {
                        g_popupXamlArtFade.Source(nullptr);
                        g_popupXamlArtFade.Opacity(0.0);
                    }
                    if (g_popupXamlBackdropCover) {
                        g_popupXamlBackdropCover.Source(nullptr);
                    }
                    if (g_popupXamlBackdropCoverFade) {
                        g_popupXamlBackdropCoverFade.Source(nullptr);
                        g_popupXamlBackdropCoverFade.Opacity(0.0);
                    }
                    if (g_popupXamlPanelCover) {
                        g_popupXamlPanelCover.Source(nullptr);
                    }
                    if (g_popupXamlPanelCoverFade) {
                        g_popupXamlPanelCoverFade.Source(nullptr);
                        g_popupXamlPanelCoverFade.Opacity(0.0);
                    }
                    g_popupXamlThumbnailHash = popupHash;
                    g_popupXamlThumbnailMaterial = g_settings.material;
                    g_popupXamlThumbnailCompact = g_settings.compact;
                    g_popupXamlBackdropCoverEnabled = useBackdropCover;
                }
            }
            if (g_popupMediaTransitionActive && !g_popupRenderingHooked) {
                StartPopupXamlRenderLoop();
            }
        }
    } else {
        UpdatePopupAlbumBitmap(state);
    }
    if (g_expandedPopup && IsWindowVisible(g_expandedPopup) && !g_popupXamlRoot) {
        ApplyPopupBackdrop(g_expandedPopup);
        RenderExpandedPopupLayer();
    }

    g_playerGrid.Visibility((g_settings.hideWhenNoMedia && !state.hasSession)
                               ? Visibility::Collapsed
                               : Visibility::Visible);
    EnsureCompactIslandInteractive();
    } catch (...) {
        Wh_Log(L"Island: exception while updating player contents");
        if (g_expandedPopup && IsWindowVisible(g_expandedPopup)) {
            try {
                BeginCloseExpandedPopup();
            } catch (...) {
                FinishCloseExpandedPopup(g_expandedPopup);
            }
        }
    }
}

void RemoveIslandGrid() {
    StopTaskbarLayoutMonitor();
    StopHoverRenderLoop();
    StopCompactTextRenderLoop();
    StopCompactProgressRenderLoop();
    DestroyExpandedPopup();

    if (!g_injectionParent || !g_playerGrid) {
        g_playerGrid = nullptr;
        g_injectionParent = nullptr;
        g_playerColumn = -1;
        g_islandScale = nullptr;
        g_compactTitleText = nullptr;
        g_compactArtistText = nullptr;
        g_compactTextHost = nullptr;
        g_compactTextLeftFade = nullptr;
        g_compactTextRightFade = nullptr;
        g_compactOutgoingTitleText = nullptr;
        g_compactOutgoingArtistText = nullptr;
        g_compactTitleTranslate = nullptr;
        g_compactArtistTranslate = nullptr;
        g_compactOutgoingTitleTranslate = nullptr;
        g_compactOutgoingArtistTranslate = nullptr;
        g_compactAlbumArtImage = nullptr;
        g_compactAlbumArtFade = nullptr;
        g_compactProgress = nullptr;
        g_compactTextEdgeFadeActive = false;
        g_compactTextInitialized = false;
        g_compactLastTitle.clear();
        g_compactLastArtist.clear();
        g_compactLastTextMediaHash = UINT64_MAX;
        return;
    }

    try {
        if (auto targetGrid = g_injectionParent.try_as<Grid>()) {
            auto children = targetGrid.Children();
            for (uint32_t i = 0; i < children.Size(); ++i) {
                auto child = children.GetAt(i).try_as<FrameworkElement>();
                bool isIsland = false;
                if (child) {
                    isIsland = child.Name() == L"IslandMedia_Wrapper";
                    if (!isIsland) {
                        try {
                            auto tag = winrt::unbox_value_or<winrt::hstring>(
                                child.Tag(), winrt::hstring{});
                            isIsland = tag == L"IslandMediaControls";
                        } catch (...) {
                        }
                    }
                }
                if (isIsland) {
                    children.RemoveAt(i);
                    break;
                }
            }

            if (g_playerColumn >= 0 &&
                g_playerColumn < static_cast<int>(targetGrid.ColumnDefinitions().Size())) {
                for (uint32_t i = 0; i < children.Size(); ++i) {
                    auto child = children.GetAt(i).try_as<FrameworkElement>();
                    if (!child) {
                        continue;
                    }
                    int col = controls::Grid::GetColumn(child);
                    if (col > g_playerColumn) {
                        controls::Grid::SetColumn(child, col - 1);
                    }
                }
                targetGrid.ColumnDefinitions().RemoveAt(g_playerColumn);
            }
        }
    } catch (...) {
    }

    g_playerGrid = nullptr;
    g_injectionParent = nullptr;
    g_playerColumn = -1;
    g_expanded = false;
    g_islandScale = nullptr;
    g_compactTitleText = nullptr;
    g_compactArtistText = nullptr;
    g_compactTextHost = nullptr;
    g_compactTextLeftFade = nullptr;
    g_compactTextRightFade = nullptr;
    g_compactOutgoingTitleText = nullptr;
    g_compactOutgoingArtistText = nullptr;
    g_compactTitleTranslate = nullptr;
    g_compactArtistTranslate = nullptr;
    g_compactOutgoingTitleTranslate = nullptr;
    g_compactOutgoingArtistTranslate = nullptr;
    g_compactAlbumArtImage = nullptr;
    g_compactAlbumArtFade = nullptr;
    g_compactProgress = nullptr;
    g_compactTextEdgeFadeActive = false;
    g_compactTextInitialized = false;
    g_compactLastTitle.clear();
    g_compactLastArtist.clear();
    g_compactLastTextMediaHash = UINT64_MAX;
    g_lastThumbnailHash = 0;
    g_themeVisualsValid = false;
    g_popupXamlThemeValid = false;
    g_popupXamlThemeMaterial.clear();
    g_popupXamlThemeShadowDepth = -1;
    g_popupXamlThemeShadowOpacity = -1;
    g_currentHoverScale = 1.0;
    g_targetHoverScale = 1.0;
}

bool InjectIslandGrid() {
    HWND hwnd = g_taskbarWnd ? g_taskbarWnd : FindCurrentProcessTaskbarWnd();
    if (!hwnd) {
        Wh_Log(L"Island: taskbar window not found");
        return false;
    }
    g_taskbarWnd = hwnd;

    try {
        auto xamlRoot = GetTaskbarXamlRoot(hwnd);
        if (!xamlRoot) {
            Wh_Log(L"Island: failed to get taskbar XAML root");
            return false;
        }

        auto root = xamlRoot.Content().try_as<FrameworkElement>();
        if (!root) {
            Wh_Log(L"Island: XAML root content isn't a FrameworkElement");
            return false;
        }

        RemoveIslandGrid();

        auto target = ResolveInjectionTarget(root);
        if (!target.grid) {
            Wh_Log(L"Island: injection target not found");
            return false;
        }

        UpdateRuntimeLayout(hwnd, root, target.grid);
        Grid island = BuildIslandGrid();

        if (target.overlay) {
            if (g_settings.position == L"taskbar_center_edge") {
                island.HorizontalAlignment(HorizontalAlignment::Center);
            } else if (g_settings.position == L"taskbar_right_edge") {
                island.HorizontalAlignment(HorizontalAlignment::Right);
            } else {
                island.HorizontalAlignment(HorizontalAlignment::Left);
            }
            controls::Grid::SetColumn(island, 0);
            controls::Canvas::SetZIndex(island, 1000);
            target.grid.Children().Append(island);
            g_playerColumn = -1;
        } else {
            ColumnDefinition colDef;
            colDef.Width({1.0, GridUnitType::Auto});
            int col = Clamp(target.column, 0,
                            static_cast<int>(target.grid.ColumnDefinitions().Size()));
            if (col >= static_cast<int>(target.grid.ColumnDefinitions().Size())) {
                target.grid.ColumnDefinitions().Append(colDef);
            } else {
                target.grid.ColumnDefinitions().InsertAt(col, colDef);
                for (uint32_t i = 0; i < target.grid.Children().Size(); ++i) {
                    auto child = target.grid.Children().GetAt(i).try_as<FrameworkElement>();
                    if (child) {
                        int childCol = controls::Grid::GetColumn(child);
                        if (childCol >= col) {
                            controls::Grid::SetColumn(child, childCol + 1);
                        }
                    }
                }
            }
            controls::Grid::SetColumn(island, col);
            target.grid.Children().Append(island);
            g_playerColumn = col;
        }

        g_playerGrid = island;
        g_injectionParent = target.grid;
        StartTaskbarLayoutMonitor(root, target.grid);
        ApplyExpandedState();
        UpdatePlayerContents();
        target.grid.UpdateLayout();
        RefreshCompactTextHostClip(false);
        Wh_Log(L"Island: injected successfully");
        return true;
    } catch (...) {
        Wh_Log(L"Island: exception while injecting");
        return false;
    }
}

void ApplyPendingSettings() {
    std::optional<Settings> settings;
    {
        std::lock_guard lock(g_pendingSettingsMutex);
        settings.swap(g_pendingSettings);
    }
    if (!settings) {
        return;
    }

    g_settings = std::move(*settings);
    g_themeVisualsValid = false;
    g_popupXamlThemeValid = false;
    g_popupXamlThemeMaterial.clear();
    g_popupXamlThemeShadowDepth = -1;
    g_popupXamlThemeShadowOpacity = -1;
    g_popupXamlThemeButtonStyle.clear();
}

void ApplyPendingSettingsAndInject() {
    if (!IsModActive()) {
        return;
    }
    ApplyPendingSettings();
    InjectIslandGrid();
}

void ApplySettingsOnTaskbarThread() {
    if (!IsModActive()) {
        return;
    }
    if (!g_taskbarWnd || !IsWindow(g_taskbarWnd)) {
        g_taskbarWnd = FindCurrentProcessTaskbarWnd();
    }
    if (g_taskbarWnd) {
        RunFromWindowThread(
            g_taskbarWnd,
            [](void*) { ApplyPendingSettingsAndInject(); },
            nullptr);
    }
}

void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    TrayUI_StartTaskbar_Original(pThis);
    if (!IsModActive()) {
        return;
    }

    g_taskbarWnd = FindCurrentProcessTaskbarWnd();
    if (g_taskbarWnd) {
        RunFromWindowThread(
            g_taskbarWnd,
            [](void*) { ApplyPendingSettingsAndInject(); },
            nullptr);
    }
}

bool HookTaskbarSymbols() {
    HMODULE taskbarDll = LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!taskbarDll) {
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {{L"const CTaskBand::`vftable'{for `ITaskListWndSite'}"},
         &CTaskBand_ITaskListWndSite_vftable},
        {{L"public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const "},
         &CTaskBand_GetTaskbarHost_Original},
        {{L"public: int __cdecl TaskbarHost::FrameHeight(void)const "},
         &TaskbarHost_FrameHeight_Original},
        {{L"public: void __cdecl std::_Ref_count_base::_Decref(void)"},
         &Std_Ref_Decref_Original},
        {{L"public: virtual void __cdecl TrayUI::StartTaskbar(void)"},
         &TrayUI_StartTaskbar_Original,
         TrayUI_StartTaskbar_Hook},
    };

    return WindhawkUtils::HookSymbols(taskbarDll, taskbarDllHooks,
                                      ARRAYSIZE(taskbarDllHooks));
}

} // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Island: init");
    g_unloading = false;
    g_modActive = false;
    g_mediaRefreshRequested = false;
    g_settings = ReadSettings();

    if (!HookTaskbarSymbols()) {
        Wh_Log(L"Island: failed to hook taskbar symbols");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_unloading) {
        return;
    }
    g_modActive = true;
    g_taskbarWnd = FindCurrentProcessTaskbarWnd();
    ApplySettingsOnTaskbarThread();
    StartMediaThread();
    RequestMediaRefresh();
}

void Wh_ModUninit() {
    if (g_unloading.exchange(true)) {
        return;
    }
    g_modActive = false;
    StopPopupOverlayWgcBorderlessAccessThread();
    ReleasePopupOverlayWgcDeviceResources();
    StopMediaThread();

    HWND hwnd = g_taskbarWnd && IsWindow(g_taskbarWnd)
                    ? g_taskbarWnd
                    : FindCurrentProcessTaskbarWnd();
    if (!hwnd && g_expandedPopup && IsWindow(g_expandedPopup)) {
        hwnd = g_expandedPopup;
    }
    if (hwnd) {
        RunFromWindowThread(hwnd, [](void*) { RemoveIslandGrid(); }, nullptr);
    }
    UnregisterPopupWindowClass();
}

void Wh_ModSettingsChanged() {
    if (!IsModActive()) {
        return;
    }
    if (Wh_GetIntValue(kMigrateMicaLikeMaterialValue, 0) &&
        GetStringSetting(L"Main.Material", L"acrylic") == L"mica_like") {
        // A settings-page change makes the current material an explicit user
        // choice. Stop applying the one-time old-default migration so
        // Mica-like can be selected normally after the update.
        Wh_SetIntValue(kMigrateMicaLikeMaterialValue, 0);
    }

    Settings settings = ReadSettings();
    {
        std::lock_guard lock(g_pendingSettingsMutex);
        g_pendingSettings = std::move(settings);
    }
    ApplySettingsOnTaskbarThread();
    RequestMediaRefresh();
}

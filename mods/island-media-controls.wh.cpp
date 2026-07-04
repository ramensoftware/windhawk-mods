// ==WindhawkMod==
// @id              island-media-controls
// @name            Island Media Controls
// @description     Dynamic island-like media controls for the Windows 11 taskbar.
// @version         0.9.27
// @author          usho
// @github          https://github.com/usho-lear
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lwindowsapp -luuid -luser32 -lshell32 -lgdi32 -lmsimg32 -lshlwapi -lwindowscodecs -ldwmapi -luiautomationcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Island Media Controls

Bring the current media session directly into the Windows 11 taskbar with a
compact, theme-aware island built from native XAML. Unlike a floating Win32
overlay, the island is inserted into the taskbar layout as a real XAML Grid, so
it fits naturally alongside your taskbar and system tray items.

## Preview

| Dark mode | Light mode |
| :---: | :---: |
| ![Island Media Controls in dark mode](https://raw.githubusercontent.com/usho-lear/island-media-controls/main/previews/dark-mode.gif) | ![Island Media Controls in light mode](https://raw.githubusercontent.com/usho-lear/island-media-controls/main/previews/light-mode.gif) |

## Features

- **Native taskbar integration:** Choose from several system tray and taskbar
  positions without placing a separate always-on-top window over the taskbar.
- **Live media information:** See album artwork, track title, and artist at a
  glance, with smooth transitions when the active media session changes.
- **Expanded player:** Click the compact island to open a native XAML player
  with artwork, seekable playback progress, and previous, play/pause, and next
  controls.
- **Light and dark themes:** Colors, text, surfaces, and controls automatically
  follow the Windows app theme.
- **Fluent visuals:** Native Acrylic, rounded XAML geometry, album-art color
  accents, hover feedback, and display-synced animations keep interactions
  fluid and consistent with Windows 11.
- **Flexible customization:** Adjust position, size, spacing, shadows, hover
  behavior, background material, expanded button style, and idle visibility.
- **Artwork effects:** Add an optional album-art background wash and choose
  whether low-resolution video thumbnails use the original browser artwork,
  a mesh gradient, or an energy-flame visual.
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
  - CompactWidth: 168
    $name: Compact width
  - ExpandedWidth: 360
    $name: Expanded overlay width
  - ExpandedHeight: 500
    $name: Expanded overlay height
  - PopupSpacing: 8
    $name: Expanded outer spacing
  - PopupCardGap: 8
    $name: Expanded cover-to-controls gap
  - PopupShadowDepth: 48
    $name: Expanded shadow depth
  - PopupShadowOpacity: 70
    $name: Expanded shadow opacity (%)
  - PopupButtonStyle: "minimal_transport"
    $name: Expanded button style
    $options:
    - "minimal_transport": "Minimal transport"
    - "fluent_bold": "Fluent bold"
  - PopupBackdropCoverEffect: "dark_only"
    $name: Expanded background album wash
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
  - Material: "mica_like"
    $name: Background material
    $options:
    - "mica_like": "Mica-like content layer"
    - "solid": "Solid"
    - "acrylic": "Acrylic / glass"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <tlhelp32.h>
#include <uiautomation.h>
#include <shlwapi.h>
#include <wincodec.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>
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
#include <cstdint>
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

namespace xaml = winrt::Windows::UI::Xaml;
namespace composition = winrt::Windows::UI::Composition;
namespace controls = winrt::Windows::UI::Xaml::Controls;
namespace input = winrt::Windows::UI::Xaml::Input;
namespace mediax = winrt::Windows::UI::Xaml::Media;
namespace imaging = winrt::Windows::UI::Xaml::Media::Imaging;
namespace hosting = winrt::Windows::UI::Xaml::Hosting;
namespace streams = winrt::Windows::Storage::Streams;
namespace gsm = winrt::Windows::Media::Control;

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
    int compactWidth = 168;
    int expandedWidth = 360;
    int expandedHeight = 430;
    int popupSpacing = 8;
    int popupCardGap = 8;
    int popupShadowDepth = 48;
    int popupShadowOpacity = 70;
    std::wstring popupButtonStyle = L"minimal_transport";
    std::wstring popupBackdropCoverEffect = L"dark_only";
    std::wstring artworkAbstractMode = L"browser_original";
    int height = 40;
    int marginLeft = 4;
    int marginRight = 4;
    bool hideWhenNoMedia = false;
    double hoverScale = 1.06;
    double hoverLerpSpeed = 28.0;
    std::wstring material = L"mica_like";
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
std::atomic_bool g_mediaThreadRunning = false;
std::thread* g_mediaThread = nullptr;
std::mutex g_mediaCommandMutex;
std::condition_variable g_mediaCommandCv;
std::deque<MediaCommand> g_mediaCommands;
std::atomic_bool g_mediaRefreshRequested = true;

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
HWND g_expandedPopup = nullptr;
bool g_popupClassRegistered = false;
hosting::DesktopWindowXamlSource g_popupXamlSource = nullptr;
HWND g_popupXamlChild = nullptr;
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
StackPanel g_popupXamlControls = nullptr;
ScaleTransform g_popupXamlControlsScale = nullptr;
uint64_t g_popupXamlThumbnailHash = UINT64_MAX;
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
RECT g_popupFinalRect{};
RECT g_popupHostRect{};
bool g_popupExpandsRight = true;
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

void ApplyCompositorRect(FrameworkElement const& element,
                         ScaleTransform const& scale,
                         TranslateTransform const& translate,
                         RECT const& currentScreen,
                         RECT const& targetScreen,
                         RECT const& popupScreen) {
    if (!element || !scale || !translate) {
        return;
    }

    double targetWidth = std::max(1.0, static_cast<double>(targetScreen.right - targetScreen.left));
    double targetHeight = std::max(1.0, static_cast<double>(targetScreen.bottom - targetScreen.top));
    double currentWidth = std::max(1.0, static_cast<double>(currentScreen.right - currentScreen.left));
    double currentHeight = std::max(1.0, static_cast<double>(currentScreen.bottom - currentScreen.top));

    // Keep the XAML layout at the final size/position and only animate the
    // compositor transform. This avoids forcing XAML measure/arrange for the
    // large cover/background surfaces on every frame; DWM can animate the
    // scale/offset transform more smoothly than repeated Width/Height changes.
    SetElementSizeIfChanged(element, targetWidth, targetHeight);
    SetCanvasPositionIfChanged(element,
                               targetScreen.left - popupScreen.left,
                               targetScreen.top - popupScreen.top);
    scale.ScaleX(currentWidth / targetWidth);
    scale.ScaleY(currentHeight / targetHeight);
    translate.X(currentScreen.left - targetScreen.left);
    translate.Y(currentScreen.top - targetScreen.top);
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

mediax::Brush IslandBackgroundBrush() {
    bool dark = IsDarkModeApprox();
    if (g_settings.material == L"mica_like") {
        return Brush(dark ? Color(0xC8, 0x2A, 0x2A, 0x2F)
                          : Color(0xD4, 0xF3, 0xF3, 0xF6));
    }

    if (g_settings.material == L"acrylic") {
        try {
            mediax::AcrylicBrush brush;
            brush.BackgroundSource(mediax::AcrylicBackgroundSource::HostBackdrop);
            brush.TintColor(dark ? Color(0xFF, 0x20, 0x20, 0x24)
                                 : Color(0xFF, 0xF3, 0xF3, 0xF6));
            brush.TintOpacity(0.62);
            brush.FallbackColor(dark ? Color(0xE8, 0x20, 0x20, 0x24)
                                     : Color(0xE8, 0xF3, 0xF3, 0xF6));
            return brush;
        } catch (...) {
        }
    }

    return Brush(dark ? Color(0xE8, 0x20, 0x20, 0x24)
                      : Color(0xE8, 0xF4, 0xF4, 0xF6));
}

mediax::Brush IslandBorderBrush() {
    bool dark = IsDarkModeApprox();
    return Brush(dark ? Color(0x36, 0xFF, 0xFF, 0xFF)
                      : Color(0x26, 0x00, 0x00, 0x00));
}

std::vector<uint8_t> ReadThumbnailBytes(streams::IRandomAccessStreamReference const& thumbnail) {
    std::vector<uint8_t> bytes;
    if (!thumbnail) {
        return bytes;
    }

    try {
        auto stream = thumbnail.OpenReadAsync().get();
        uint64_t size64 = stream.Size();
        if (size64 == 0 || size64 > 8 * 1024 * 1024) {
            return bytes;
        }

        uint32_t size = static_cast<uint32_t>(size64);
        auto buffer = stream.ReadAsync(streams::Buffer(size), size,
                                       streams::InputStreamOptions::None)
                          .get();
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

Settings ReadSettings() {
    Settings settings;
    settings.position = GetStringSetting(L"Main.Position", L"tray_left");
    settings.compactWidth = Clamp(Wh_GetIntSetting(L"Main.CompactWidth"), 96, 320);
    settings.expandedWidth = Clamp(Wh_GetIntSetting(L"Main.ExpandedWidth"), 240, 640);
    settings.expandedHeight = Clamp(Wh_GetIntSetting(L"Main.ExpandedHeight"), 430, 760);
    settings.popupSpacing = Clamp(Wh_GetIntSetting(L"Main.PopupSpacing"), 2, 24);
    settings.popupCardGap = Clamp(Wh_GetIntSetting(L"Main.PopupCardGap"), 0, 40);
    settings.popupShadowDepth = Clamp(Wh_GetIntSetting(L"Main.PopupShadowDepth"), 0, 128);
    settings.popupShadowOpacity = Clamp(Wh_GetIntSetting(L"Main.PopupShadowOpacity"), 0, 100);
    settings.popupButtonStyle = GetStringSetting(L"Main.PopupButtonStyle", L"minimal_transport");
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
    settings.material = GetStringSetting(L"Main.Material", L"mica_like");
    if (settings.material != L"mica_like" &&
        settings.material != L"solid" &&
        settings.material != L"acrylic") {
        settings.material = L"mica_like";
    }
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

std::wstring MediaContentKey(MediaState const& state) {
    if (!state.hasSession) {
        return L"";
    }
    return state.title + L"\n" + state.artist + L"\n" +
           std::to_wstring(state.durationTicks) + L"\n" +
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
    g_mediaRefreshRequested = true;
    g_mediaCommandCv.notify_one();
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
        auto manager =
            gsm::GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
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

            auto props = session.TryGetMediaPropertiesAsync().get();
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
    if (!command || g_unloading || !g_mediaThreadRunning) {
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

            bool changed = session
                               .TryChangePlaybackPositionAsync(
                                   relativePositionTicks)
                               .get();
            if (!changed && absolutePositionTicks != relativePositionTicks) {
                changed = session
                              .TryChangePlaybackPositionAsync(
                                  absolutePositionTicks)
                              .get();
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


std::vector<uint8_t> CreateLowDetailAlbumCoverBytes(std::vector<uint8_t> const& bytes, bool bottomFadeToMiddle = false) {
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

    if (SUCCEEDED(hr) && bottomFadeToMiddle) {
        // Expanded-surface color wash: keep only a low-frequency album glow
        // near the bottom of the rounded background shell. The image itself
        // carries a vertical alpha gradient: fully transparent from the top to
        // the middle, then easing to a stronger translucent value at the bottom.
        const double kBottomAlpha = IsDarkModeApprox() ? 0.72 : 0.88;
        const double middle = (static_cast<double>(kLowDetailSize) - 1.0) * 0.50;
        const double bottom = static_cast<double>(kLowDetailSize) - 1.0;
        for (UINT y = 0; y < kLowDetailSize; ++y) {
            double amount = 0.0;
            if (static_cast<double>(y) > middle && bottom > middle) {
                amount = (static_cast<double>(y) - middle) / (bottom - middle);
                amount = Clamp(amount, 0.0, 1.0);
                amount = amount * amount * (3.0 - 2.0 * amount);
            }
            double alphaScale = kBottomAlpha * amount;
            for (UINT x = 0; x < kLowDetailSize; ++x) {
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

std::vector<uint8_t> CreatePlaceholderAlbumCoverBytes(UINT size, bool bottomFadeToMiddle = false) {
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
            if (bottomFadeToMiddle) {
                const double middle = 0.50;
                alphaScale = 0.0;
                if (v > middle) {
                    double amount = (v - middle) / (1.0 - middle);
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

std::vector<uint8_t> CreateDisplayAlbumCoverBytes(std::vector<uint8_t> const& bytes) {
    if (bytes.empty()) {
        return {};
    }
    if (!ShouldUseAbstractArtworkForDisplay(bytes) ||
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
constexpr UINT_PTR kPopupTimerId = 0x494D;

double SmoothStep(double value) {
    value = Clamp(value, 0.0, 1.0);
    return value * value * (3.0 - 2.0 * value);
}

double SmootherStep(double value) {
    value = Clamp(value, 0.0, 1.0);
    return value * value * value * (value * (value * 6.0 - 15.0) + 10.0);
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

double PopupProgress() {
    return SmoothStep(g_popupAnimationProgress);
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
    return Clamp(g_settings.popupSpacing, 2, 24);
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
int PopupBackdropPadding() {
    return PopupSurfaceGap();
}
constexpr int kPopupCardHeight = 138;
constexpr int kPopupCardInnerPadding = 18;
constexpr int kPopupTextTopPadding = 13;
constexpr int kPopupTextLineGap = 24;
constexpr int kPopupTimeTextHeight = 16;
constexpr double kPopupUnifiedCornerRadius = 24.0;
// Transparent host margin used only so the rounded XAML shadow can fit without
// falling back to the rectangular HWND shadow.
constexpr int kPopupHostShadowMargin = 18;
// The backdrop shell is the only outer background shape. In C mode its
// material is exactly the compact island material, so compact/expanded states
// share one color, one stroke model, and one predictable XAML clipping path.
constexpr double kPopupBackdropOpacity = 1.0;

bool PopupBackdropCoverEffectEnabled() {
    if (g_settings.popupBackdropCoverEffect == L"on") {
        return true;
    }
    if (g_settings.popupBackdropCoverEffect == L"off") {
        return false;
    }
    // Default: keep the album color wash only in dark mode. In light mode the
    // compact-material surface stays clean and bright unless explicitly enabled.
    return IsDarkModeApprox();
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
    // The background shell is the tight bounding box around cover + control card.
    // These four distances are intentionally identical:
    // cover->shell top, cover->shell left, cover->shell right, card->shell bottom.
    return {artRect.left - PopupBackdropPadding(),
            artRect.top - PopupBackdropPadding(),
            artRect.right + PopupBackdropPadding(),
            cardRect.bottom + PopupBackdropPadding()};
}

int PopupFinalWidthFromArtSize(int artSize) {
    return artSize + PopupSurfaceGap() * 2 + kPopupHostShadowMargin * 2;
}

int PopupFinalHeightFromArtSize(int artSize) {
    return artSize + kPopupCardHeight + PopupArtCardGap() +
           PopupSurfaceGap() * 2 + kPopupHostShadowMargin * 2;
}


mediax::Brush PopupControlCardBrush() {
    bool dark = IsDarkModeApprox();
    // Keep light and dark tint strengths independent. Light mode keeps the
    // stronger white-tint experiment, while dark mode is rolled back to the
    // earlier compact-material-d-backdrop-card-tint values.
    if (dark) {
        return Brush(Color(0x70, 0x20, 0x20, 0x26));
    }
    return Brush(Color(0xB8, 0xFF, 0xFF, 0xFF));
}

mediax::Brush PopupBackdropCardTintBrush() {
    bool dark = IsDarkModeApprox();
    // Backdrop and control tints are tuned separately so light mode can stay
    // brighter without making dark mode look washed out. In dark mode this
    // matches the previous version, where the backdrop card tint reused the
    // control-card tint.
    if (dark) {
        return Brush(Color(0x70, 0x20, 0x20, 0x26));
    }
    return Brush(Color(0xD0, 0xFF, 0xFF, 0xFF));
}

double PopupPanelCoverOpacityFactor() {
    // The stronger light-mode white tint can hide the album color wash too much.
    // Boost only the light-mode cover-wash opacity; keep dark mode unchanged.
    return IsDarkModeApprox() ? 0.44 : 0.58;
}

mediax::Brush PopupSurfaceBrush() {
    // C version: extend the compact island material to the expanded surface.
    // This keeps compact->expanded transitions visually consistent and avoids
    // mixing DWM backdrop, XAML Acrylic, and extra tint layers.
    return IslandBackgroundBrush();
}

mediax::Brush PopupSurfaceStrokeBrush() {
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
    int targetArt = PopupTargetArtSize(finalRect.right - finalRect.left,
                                       finalRect.bottom - finalRect.top);

    int artLeft = surfaceRect.left + (finalWidth - targetArt) / 2;
    int artTop = surfaceRect.top + PopupSurfaceGap();
    artRect = {artLeft, artTop, artLeft + targetArt, artTop + targetArt};

    int cardTop = artRect.bottom + PopupArtCardGap();
    cardRect = {artRect.left, cardTop, artRect.right, cardTop + kPopupCardHeight};

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
    RECT targetArt{};
    RECT targetCard{};
    RECT targetTitle{};
    RECT targetArtist{};
    RECT targetProgress{};
    RECT targetElapsed{};
    RECT targetDuration{};
    RECT targetControls{};
    CalculatePopupFinalLayout(g_popupFinalRect, targetArt, targetCard,
                              targetTitle, targetArtist, targetProgress,
                              targetElapsed, targetDuration, targetControls);
    RECT targetBackdrop = PopupBackdropRectFromParts(targetArt, targetCard);
    double progress = PopupProgress();
    return {LerpInt(g_popupSourceRect.left, targetBackdrop.left, progress),
            LerpInt(g_popupSourceRect.top, targetBackdrop.top, progress),
            LerpInt(g_popupSourceRect.right, targetBackdrop.right, progress),
            LerpInt(g_popupSourceRect.bottom, targetBackdrop.bottom, progress)};
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
    if (auto icon = surface.Child().try_as<TextBlock>()) {
        icon.Text(PopupTransportGlyph(surface, playing));
    }
}

double PopupControlsWidth() {
    return 126.0;
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

    double width = 36.0;
    double height = 34.0;
    double corner = 17.0;
    double fontSize = primary ? 22.0 : 21.0;
    mediax::Brush background = hovered ? PopupButtonHoverBrush(dark)
                                       : Brush(Color(0x00, 0x00, 0x00, 0x00));
    mediax::Brush foreground = Brush(primaryText);

    if (PopupButtonStyleIs(L"fluent_bold")) {
        fontSize = primary ? 26.0 : 24.0;
    }

    surface.Width(width);
    surface.Height(height);
    surface.CornerRadius({corner, corner, corner, corner});
    surface.Background(background);
    surface.BorderThickness({0, 0, 0, 0});
    surface.BorderBrush(Brush(Color(0x00, 0x00, 0x00, 0x00)));

    if (auto icon = surface.Child().try_as<TextBlock>()) {
        bool playing = SnapshotMedia().isPlaying;
        icon.Text(PopupTransportGlyph(surface, playing));
        icon.Foreground(foreground);
        icon.FontFamily(mediax::FontFamily(L"Segoe Fluent Icons"));
        icon.FontSize(fontSize);
        icon.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    }
}

Border MakePopupXamlButton(const wchar_t* name, void (*onClick)(), bool primary = false) {
    Border surface;
    surface.Name(name);
    surface.Margin({3, 0, 3, 0});
    surface.Padding({0, 0, 0, 0});
    surface.Tag(winrt::box_value(winrt::hstring(primary ? L"primary" : L"secondary")));
    surface.IsHitTestVisible(true);

    TextBlock icon;
    icon.FontFamily(mediax::FontFamily(L"Segoe Fluent Icons"));
    icon.Text(L"\uE768");
    icon.HorizontalAlignment(HorizontalAlignment::Center);
    icon.VerticalAlignment(VerticalAlignment::Center);
    icon.TextAlignment(xaml::TextAlignment::Center);
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
        g_popupXamlBackdrop.BorderThickness({1, 1, 1, 1});
        g_popupXamlBackdrop.BorderBrush(PopupSurfaceStrokeBrush());
        InitializePopupCompositionShadow();
    }
    if (g_popupXamlBackdropTint) {
        g_popupXamlBackdropTint.Background(PopupBackdropCardTintBrush());
        g_popupXamlBackdropTint.BorderThickness({0, 0, 0, 0});
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
        g_popupXamlArtFrame.BorderThickness({1, 1, 1, 1});
        g_popupXamlArtFrame.BorderBrush(PopupSurfaceStrokeBrush());
    }
    g_popupXamlPanel.Background(PopupControlCardBrush());
    // Playback control card uses the same subtle outline as the compact island
    // and the expanded outer surface.
    g_popupXamlPanel.BorderThickness({1, 1, 1, 1});
    g_popupXamlPanel.BorderBrush(PopupSurfaceStrokeBrush());

    auto primary = Brush(dark ? Color(0xFF, 0xFF, 0xFF, 0xFF)
                              : Color(0xFF, 0x00, 0x00, 0x00));
    auto secondary = Brush(dark ? Color(0xC8, 0xFF, 0xFF, 0xFF)
                                : Color(0xC8, 0x00, 0x00, 0x00));
    if (g_popupXamlTitle) g_popupXamlTitle.Foreground(primary);
    if (g_popupXamlArtist) g_popupXamlArtist.Foreground(secondary);
    if (g_popupXamlElapsed) g_popupXamlElapsed.Foreground(secondary);
    if (g_popupXamlDuration) g_popupXamlDuration.Foreground(secondary);

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
    if (g_popupXamlArt) {
        g_popupXamlArt.Opacity(coverFade);
    }
    if (g_popupXamlArtFade) {
        g_popupXamlArtFade.Opacity(coverTransitionActive ? (1.0 - fade) : 0.0);
    }
    bool backdropCoverEnabled = PopupBackdropCoverEffectEnabled();
    if (g_popupXamlBackdropCover) {
        g_popupXamlBackdropCover.Opacity(backdropCoverEnabled ? coverFade : 0.0);
    }
    if (g_popupXamlBackdropCoverFade) {
        g_popupXamlBackdropCoverFade.Opacity(backdropCoverEnabled && coverTransitionActive
                                            ? (1.0 - fade)
                                            : 0.0);
    }
    if (g_popupXamlPanelCover) {
        g_popupXamlPanelCover.Opacity(coverFade);
    }
    if (g_popupXamlPanelCoverFade) {
        g_popupXamlPanelCoverFade.Opacity(coverTransitionActive ? (1.0 - fade) : 0.0);
    }

    // Expanded-view text uses the same reversed direction as the compact island:
    // old title/artist slide left and fade out, while new title/artist slide in
    // from the right. The artist starts slightly later than the title.
    constexpr double kPopupTextSlideOffset = 30.0;
    if (g_popupTextTransitionActive) {
        double p = Clamp(g_popupMediaTransitionProgress, 0.0, 1.0);
        double titleIn = SmootherStep(Clamp((p - 0.04) / 0.86, 0.0, 1.0));
        double artistIn = SmootherStep(Clamp((p - 0.34) / 0.62, 0.0, 1.0));
        double titleOut = SmootherStep(Clamp((p - 0.00) / 0.78, 0.0, 1.0));
        double artistOut = SmootherStep(Clamp((p - 0.18) / 0.72, 0.0, 1.0));

        if (g_popupXamlTitleTranslate) {
            g_popupXamlTitleTranslate.X(kPopupTextSlideOffset * (1.0 - titleIn));
        }
        if (g_popupXamlArtistTranslate) {
            g_popupXamlArtistTranslate.X(kPopupTextSlideOffset * (1.0 - artistIn));
        }
        if (g_popupXamlTitle) {
            g_popupXamlTitle.Opacity(g_popupTextBaseOpacity * titleIn);
        }
        if (g_popupXamlArtist) {
            g_popupXamlArtist.Opacity(g_popupTextBaseOpacity * artistIn);
        }

        if (g_popupXamlOutgoingTitleTranslate) {
            g_popupXamlOutgoingTitleTranslate.X(-kPopupTextSlideOffset * titleOut);
        }
        if (g_popupXamlOutgoingArtistTranslate) {
            g_popupXamlOutgoingArtistTranslate.X(-kPopupTextSlideOffset * artistOut);
        }
        if (g_popupXamlOutgoingTitle) {
            g_popupXamlOutgoingTitle.Opacity(g_popupTextBaseOpacity * (1.0 - titleOut));
        }
        if (g_popupXamlOutgoingArtist) {
            g_popupXamlOutgoingArtist.Opacity(g_popupTextBaseOpacity * (1.0 - artistOut));
        }
    } else {
        if (g_popupXamlTitleTranslate) g_popupXamlTitleTranslate.X(0.0);
        if (g_popupXamlArtistTranslate) g_popupXamlArtistTranslate.X(0.0);
        if (g_popupXamlOutgoingTitleTranslate) g_popupXamlOutgoingTitleTranslate.X(0.0);
        if (g_popupXamlOutgoingArtistTranslate) g_popupXamlOutgoingArtistTranslate.X(0.0);
        if (g_popupXamlOutgoingTitle) g_popupXamlOutgoingTitle.Opacity(0.0);
        if (g_popupXamlOutgoingArtist) g_popupXamlOutgoingArtist.Opacity(0.0);
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
            winrt::Windows::Foundation::Numerics::float3{0.0f, 0.0f, static_cast<float>(depth)});
    } catch (...) {
    }
}

void ResetPopupXamlElementState() {
    g_popupXamlSource = nullptr;
    g_popupXamlChild = nullptr;
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
    g_popupXamlControlsScale = nullptr;
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
        backdropCoverHost.Children().Append(backdropTint);
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
        // Match the compact no-media placeholder: when there is no thumbnail,
        // the expanded cover still shows a proper placeholder instead of a blank frame.
        artFrame.Background(Brush(DefaultPopupAccentColor()));
        artFrame.BorderThickness({1, 1, 1, 1});
        artFrame.BorderBrush(PopupSurfaceStrokeBrush());
        AttachGpuFriendlyTransform(artFrame, g_popupXamlArtScale, g_popupXamlArtTranslate);
        Grid artHost;
        Image artFade;
        artFade.Stretch(mediax::Stretch::UniformToFill);
        artFade.Opacity(0.0);
        Image art;
        art.Stretch(mediax::Stretch::UniformToFill);
        art.Opacity(1.0);
        artHost.Children().Append(artFade);
        artHost.Children().Append(art);
        artFrame.Child(artHost);
        canvas.Children().Append(artFrame);

        TextBlock outgoingTitle;
        outgoingTitle.FontFamily(mediax::FontFamily(L"Segoe UI Variable Text"));
        outgoingTitle.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        outgoingTitle.TextTrimming(xaml::TextTrimming::CharacterEllipsis);
        outgoingTitle.VerticalAlignment(VerticalAlignment::Center);
        outgoingTitle.Opacity(0.0);
        TranslateTransform outgoingTitleTranslate;
        outgoingTitle.RenderTransform(outgoingTitleTranslate);
        canvas.Children().Append(outgoingTitle);

        TextBlock outgoingArtist;
        outgoingArtist.FontFamily(mediax::FontFamily(L"Segoe UI Variable Text"));
        outgoingArtist.TextTrimming(xaml::TextTrimming::CharacterEllipsis);
        outgoingArtist.VerticalAlignment(VerticalAlignment::Center);
        outgoingArtist.Opacity(0.0);
        TranslateTransform outgoingArtistTranslate;
        outgoingArtist.RenderTransform(outgoingArtistTranslate);
        canvas.Children().Append(outgoingArtist);

        TextBlock title;
        title.FontFamily(mediax::FontFamily(L"Segoe UI Variable Text"));
        title.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
        title.TextTrimming(xaml::TextTrimming::CharacterEllipsis);
        title.VerticalAlignment(VerticalAlignment::Center);
        TranslateTransform titleTranslate;
        title.RenderTransform(titleTranslate);
        canvas.Children().Append(title);

        TextBlock artist;
        artist.FontFamily(mediax::FontFamily(L"Segoe UI Variable Text"));
        artist.TextTrimming(xaml::TextTrimming::CharacterEllipsis);
        artist.VerticalAlignment(VerticalAlignment::Center);
        TranslateTransform artistTranslate;
        artist.RenderTransform(artistTranslate);
        canvas.Children().Append(artist);

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
        progressTrack.CornerRadius({3, 3, 3, 3});
        progressTrack.IsHitTestVisible(false);
        canvas.Children().Append(progressTrack);

        Border progressFill;
        progressFill.Height(6);
        progressFill.CornerRadius({3, 3, 3, 3});
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
        progressGlowCore.CornerRadius({3, 3, 3, 3});
        progressGlowCore.IsHitTestVisible(false);
        progressGlowCore.Opacity(0.0);
        canvas.Children().Append(progressGlowCore);

        Border progressHitTarget;
        progressHitTarget.Height(22);
        progressHitTarget.Background(Brush(Color(0x01, 0x00, 0x00, 0x00)));
        canvas.Children().Append(progressHitTarget);

        StackPanel controlsPanel;
        controlsPanel.Orientation(controls::Orientation::Horizontal);
        controlsPanel.RenderTransformOrigin({0.5, 0.5});
        ScaleTransform controlsScale;
        controlsScale.ScaleX(0.82);
        controlsScale.ScaleY(0.82);
        controlsPanel.RenderTransform(controlsScale);
        controlsPanel.Children().Append(MakePopupXamlButton(
            L"Popup_Prev", [] {
                RunMediaCommand([](gsm::GlobalSystemMediaTransportControlsSession const& s) {
                    s.TrySkipPreviousAsync().get();
                });
            }));
        controlsPanel.Children().Append(MakePopupXamlButton(
            L"Popup_Play", [] {
                RunMediaCommand([](gsm::GlobalSystemMediaTransportControlsSession const& s) {
                    s.TryTogglePlayPauseAsync().get();
                });
            }, true));
        controlsPanel.Children().Append(MakePopupXamlButton(
            L"Popup_Next", [] {
                RunMediaCommand([](gsm::GlobalSystemMediaTransportControlsSession const& s) {
                    s.TrySkipNextAsync().get();
                });
            }));
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
        g_popupXamlPanelCoverFrame = panelCoverFrame;
        g_popupXamlPanelCoverFade = panelCoverFade;
        g_popupXamlPanelCover = panelCover;
        g_popupXamlPanel = panel;
        g_popupXamlArtFrame = artFrame;
        g_popupXamlArtFade = artFade;
        g_popupXamlArt = art;
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
        g_popupXamlControlsScale = controlsScale;
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

void UpdatePopupXamlVisuals() {
    if (!g_popupXamlRoot || !g_expandedPopup) {
        return;
    }

    RECT popupScreen{};
    GetWindowRect(g_expandedPopup, &popupScreen);
    int width = popupScreen.right - popupScreen.left;
    int height = popupScreen.bottom - popupScreen.top;
    if (width <= 0 || height <= 0) {
        return;
    }
    double progress = PopupProgress();
    RECT currentRect{popupScreen.left, popupScreen.top, popupScreen.right, popupScreen.bottom};
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
    CalculatePopupFinalLayout(g_popupFinalRect, targetArtScreen, targetCard,
                              targetTitle, targetArtist,
                              targetProgress, targetElapsed, targetDuration,
                              targetControls);
    RECT targetBackdrop = PopupBackdropRectFromParts(targetArtScreen, targetCard);
    RECT backdropScreen{
        LerpInt(g_popupSourceRect.left, targetBackdrop.left, progress),
        LerpInt(g_popupSourceRect.top, targetBackdrop.top, progress),
        LerpInt(g_popupSourceRect.right, targetBackdrop.right, progress),
        LerpInt(g_popupSourceRect.bottom, targetBackdrop.bottom, progress),
    };
    double backdropWidth = std::max(1L, targetBackdrop.right - targetBackdrop.left);
    double backdropHeight = std::max(1L, targetBackdrop.bottom - targetBackdrop.top);
    if (g_popupXamlShadowVisual) {
        float shadowOpacity = static_cast<float>(Clamp((progress - 0.08) / 0.26, 0.0, 1.0));
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
        double shellRadius = kPopupUnifiedCornerRadius;
        g_popupXamlBackdrop.CornerRadius({shellRadius, shellRadius, shellRadius, shellRadius});
        double backdropOpacity = kPopupBackdropOpacity * Clamp((progress - 0.04) / 0.22, 0.0, 1.0);
        g_popupXamlBackdrop.Opacity(backdropOpacity);
        // Optional album color wash. By default it is disabled in light mode
        // and enabled in dark mode; Main.PopupBackdropCoverEffect can override it.
        bool backdropCoverEnabled = PopupBackdropCoverEffectEnabled();
        if (g_popupXamlBackdropCover) {
            g_popupXamlBackdropCover.Stretch(mediax::Stretch::UniformToFill);
            if (!backdropCoverEnabled) {
                g_popupXamlBackdropCover.Opacity(0.0);
            }
        }
        if (g_popupXamlBackdropCoverFade) {
            g_popupXamlBackdropCoverFade.Stretch(mediax::Stretch::UniformToFill);
            if (!backdropCoverEnabled) {
                g_popupXamlBackdropCoverFade.Opacity(0.0);
            }
        }
        ApplyCompositorRect(g_popupXamlBackdrop,
                            g_popupXamlBackdropScale,
                            g_popupXamlBackdropTranslate,
                            backdropScreen,
                            targetBackdrop,
                            popupScreen);
    }
    RECT artScreen{
        LerpInt(g_popupSourceArtRect.left, targetArtScreen.left, progress),
        LerpInt(g_popupSourceArtRect.top, targetArtScreen.top, progress),
        LerpInt(g_popupSourceArtRect.right, targetArtScreen.right, progress),
        LerpInt(g_popupSourceArtRect.bottom, targetArtScreen.bottom, progress),
    };
    double artRadius = kPopupUnifiedCornerRadius;
    g_popupXamlArtFrame.CornerRadius({artRadius, artRadius, artRadius, artRadius});
    ApplyCompositorRect(g_popupXamlArtFrame,
                        g_popupXamlArtScale,
                        g_popupXamlArtTranslate,
                        artScreen,
                        targetArtScreen,
                        popupScreen);

    RECT cardScreen{
        LerpInt(g_popupSourceRect.left, targetCard.left, progress),
        LerpInt(g_popupSourceRect.top, targetCard.top, progress),
        LerpInt(g_popupSourceRect.right, targetCard.right, progress),
        LerpInt(g_popupSourceRect.bottom, targetCard.bottom, progress),
    };
    double panelRadius = kPopupUnifiedCornerRadius;
    double cardOpacity = Clamp((progress - 0.12) / 0.28, 0.0, 1.0);
    if (g_popupXamlPanelCoverFrame) {
        // Keep the card's layout at its final rect and animate only transform.
        // This preserves the color-wash clipping while avoiding per-frame
        // remeasure/arrange of the large blurred cover layer.
        g_popupXamlPanelCoverFrame.CornerRadius({panelRadius, panelRadius, panelRadius, panelRadius});
        g_popupXamlPanelCoverFrame.Opacity(cardOpacity * PopupPanelCoverOpacityFactor());
        ApplyCompositorRect(g_popupXamlPanelCoverFrame,
                            g_popupXamlPanelCoverScale,
                            g_popupXamlPanelCoverTranslate,
                            cardScreen,
                            targetCard,
                            popupScreen);
    }
    g_popupXamlPanel.CornerRadius({panelRadius, panelRadius, panelRadius, panelRadius});
    g_popupXamlPanel.Opacity(cardOpacity);
    ApplyCompositorRect(g_popupXamlPanel,
                        g_popupXamlPanelScale,
                        g_popupXamlPanelTranslate,
                        cardScreen,
                        targetCard,
                        popupScreen);

    auto placeText = [&](TextBlock const& text, RECT const& source, RECT const& target,
                         double compactSize, double expandedSize) {
        RECT screen{LerpInt(source.left, target.left, progress),
                    LerpInt(source.top, target.top, progress),
                    LerpInt(source.right, target.right, progress),
                    LerpInt(source.bottom, target.bottom, progress)};
        double textWidth = std::max(1L, screen.right - screen.left);
        double textHeight = std::max(1L, screen.bottom - screen.top);
        text.Width(textWidth);
        text.Height(textHeight);
        ApplyElementClip(text, textWidth, textHeight);
        text.FontSize(expandedSize);
        controls::Canvas::SetLeft(text, screen.left - popupScreen.left);
        controls::Canvas::SetTop(text, screen.top - popupScreen.top);
    };
    placeText(g_popupXamlOutgoingTitle, g_popupSourceTitleRect, targetTitle, 12, 16);
    placeText(g_popupXamlOutgoingArtist, g_popupSourceArtistRect, targetArtist, 10, 13);
    placeText(g_popupXamlTitle, g_popupSourceTitleRect, targetTitle, 12, 16);
    placeText(g_popupXamlArtist, g_popupSourceArtistRect, targetArtist, 10, 13);

    double textOpacity = SmoothStep(Clamp(progress, 0.0, 1.0));
    g_popupTextBaseOpacity = textOpacity;
    if (!g_popupTextTransitionActive) {
        if (g_popupXamlTitle) g_popupXamlTitle.Opacity(textOpacity);
        if (g_popupXamlArtist) g_popupXamlArtist.Opacity(textOpacity);
        if (g_popupXamlTitleTranslate) g_popupXamlTitleTranslate.X(0.0);
        if (g_popupXamlArtistTranslate) g_popupXamlArtistTranslate.X(0.0);
        if (g_popupXamlOutgoingTitle) g_popupXamlOutgoingTitle.Opacity(0.0);
        if (g_popupXamlOutgoingArtist) g_popupXamlOutgoingArtist.Opacity(0.0);
    }

    // Morph the transport controls from the compact island area to their final
    // positions, instead of starting from the already-expanded control card.
    // The previous version used the card center as the source and delayed the
    // animation to progress 0.28~0.62. In practice that looked like a simple
    // fade-in because the source was already close to the target and the active
    // time window was too short.
    double controlsProgress = SmoothStep(Clamp(progress, 0.0, 1.0));
    double controlsOpacity = SmoothStep(Clamp(progress, 0.0, 1.0));
    double controlsScale = 0.52 + 0.48 * controlsProgress;
    bool showControls = progress > 0.025 || controlsOpacity > 0.001;

    auto lerpRect = [&](RECT const& source, RECT const& target, double amount) {
        return RECT{LerpInt(source.left, target.left, amount),
                    LerpInt(source.top, target.top, amount),
                    LerpInt(source.right, target.right, amount),
                    LerpInt(source.bottom, target.bottom, amount)};
    };

    int sourceWidth = std::max(1, static_cast<int>(g_popupSourceRect.right - g_popupSourceRect.left));
    int sourceCenterX = (g_popupSourceRect.left + g_popupSourceRect.right) / 2;
    int sourceCenterY = (g_popupSourceRect.top + g_popupSourceRect.bottom) / 2;
    int sourceInset = Clamp(sourceWidth / 8, 8, 18);

    RECT progressSource{g_popupSourceRect.left + sourceInset,
                        g_popupSourceRect.bottom - 8,
                        g_popupSourceRect.right - sourceInset,
                        g_popupSourceRect.bottom - 4};
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
        double radius = std::max(2.0, g_popupXamlProgressTrack.Height() * 0.5);
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
        double fillRadius = std::max(2.0, progressThickness * 0.5);
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

    RECT elapsedSource{sourceCenterX - 44,
                       g_popupSourceRect.bottom - 7,
                       sourceCenterX - 6,
                       g_popupSourceRect.bottom + 9};
    RECT durationSource{sourceCenterX + 6,
                        g_popupSourceRect.bottom - 7,
                        sourceCenterX + 44,
                        g_popupSourceRect.bottom + 9};
    placeTimeText(g_popupXamlElapsed, elapsedSource, targetElapsed, false);
    placeTimeText(g_popupXamlDuration, durationSource, targetDuration, true);

    double controlsWidth = PopupControlsWidth();
    int controlsSourceHalfWidth = static_cast<int>(std::lround(controlsWidth * 0.38));
    RECT controlsSource{sourceCenterX - controlsSourceHalfWidth,
                        sourceCenterY - 14,
                        sourceCenterX + controlsSourceHalfWidth,
                        sourceCenterY + 14};
    RECT controlsScreen = lerpRect(controlsSource, targetControls, controlsProgress);
    if (g_popupXamlControls) {
        g_popupXamlControls.Visibility(showControls ? Visibility::Visible : Visibility::Collapsed);
        g_popupXamlControls.Opacity(controlsOpacity);
        g_popupXamlControls.Width(controlsWidth);
        if (g_popupXamlControlsScale) {
            g_popupXamlControlsScale.ScaleX(controlsScale);
            g_popupXamlControlsScale.ScaleY(controlsScale);
        }
        controls::Canvas::SetLeft(g_popupXamlControls,
                                  controlsScreen.left - popupScreen.left +
                                      ((controlsScreen.right - controlsScreen.left) - controlsWidth) / 2.0);
        controls::Canvas::SetTop(g_popupXamlControls,
                                 controlsScreen.top - popupScreen.top +
                                     ((controlsScreen.bottom - controlsScreen.top) - 34.0) / 2.0);
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

void CapturePopupSourceGeometry() {
    if (!GetElementScreenRect(g_playerGrid, g_popupSourceRect)) {
        RECT taskbar{};
        GetWindowRect(g_taskbarWnd, &taskbar);
        g_popupSourceRect = {taskbar.left, taskbar.top,
                             taskbar.left + g_settings.compactWidth,
                             taskbar.top + g_settings.height};
    }
    auto art = FindChildByName(g_playerGrid, L"Island_ArtFallback");
    if (!GetElementScreenRect(art, g_popupSourceArtRect)) {
        g_popupSourceArtRect = {g_popupSourceRect.left + 10,
                                g_popupSourceRect.top + 6,
                                g_popupSourceRect.left + 38,
                                g_popupSourceRect.top + 34};
    }
    auto title = FindChildByName(g_playerGrid, L"Island_Title");
    if (!GetElementScreenRect(title, g_popupSourceTitleRect)) {
        g_popupSourceTitleRect = {g_popupSourceRect.left + 48,
                                  g_popupSourceRect.top + 5,
                                  g_popupSourceRect.right - 10,
                                  g_popupSourceRect.top + 22};
    }
    auto artist = FindChildByName(g_playerGrid, L"Island_CompactArtist");
    if (!GetElementScreenRect(artist, g_popupSourceArtistRect)) {
        g_popupSourceArtistRect = {g_popupSourceRect.left + 48,
                                   g_popupSourceRect.top + 21,
                                   g_popupSourceRect.right - 10,
                                   g_popupSourceRect.bottom - 4};
    }

    HMONITOR monitor = MonitorFromRect(&g_popupSourceRect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO monitorInfo{sizeof(monitorInfo)};
    if (GetMonitorInfoW(monitor, &monitorInfo)) {
        int monitorWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
        RECT taskbar{};
        GetWindowRect(g_taskbarWnd, &taskbar);
        int finalBottom = taskbar.top - 8;
        int availableHeight = finalBottom - monitorInfo.rcMonitor.top - 12;

        int maxArtBySetting = std::max(96, g_settings.expandedWidth -
            (PopupSurfaceGap() * 2 + kPopupHostShadowMargin * 2));
        int maxArtByMonitorWidth = std::max(96, monitorWidth - 24 -
            (PopupSurfaceGap() * 2 + kPopupHostShadowMargin * 2));
        int maxArtByHeight = std::max(96, availableHeight -
            (kPopupCardHeight + PopupArtCardGap() +
             PopupSurfaceGap() * 2 + kPopupHostShadowMargin * 2));
        int targetArt = Clamp(std::min({maxArtBySetting,
                                        maxArtByMonitorWidth,
                                        maxArtByHeight,
                                        kPopupMaximumArtSize}),
                              96, kPopupMaximumArtSize);
        if (targetArt < kPopupMinimumArtSize && maxArtByHeight >= kPopupMinimumArtSize) {
            targetArt = kPopupMinimumArtSize;
        }

        int finalWidth = PopupFinalWidthFromArtSize(targetArt);
        int finalHeight = PopupFinalHeightFromArtSize(targetArt);
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
                          static_cast<int>(monitorInfo.rcMonitor.left) + 12,
                          static_cast<int>(monitorInfo.rcMonitor.right) - finalWidth - 12);
        g_popupFinalRect = {finalLeft, finalBottom - finalHeight,
                            finalLeft + finalWidth, finalBottom};
    } else {
        int targetArt = Clamp(g_settings.expandedWidth -
                                  (PopupSurfaceGap() * 2 + kPopupHostShadowMargin * 2),
                              96, kPopupMaximumArtSize);
        g_popupExpandedWidth = static_cast<double>(PopupFinalWidthFromArtSize(targetArt));
        g_popupExpandedHeight = static_cast<double>(PopupFinalHeightFromArtSize(targetArt));
        g_popupExpandsRight = true;
        g_popupFinalRect = {g_popupSourceRect.left,
                            g_popupSourceRect.top - static_cast<int>(g_popupExpandedHeight) - 8,
                            g_popupSourceRect.left + static_cast<int>(g_popupExpandedWidth),
                            g_popupSourceRect.top - 8};
    }
}

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
        // more visible. SmootherStep keeps both ends of the motion soft.
        g_compactTextProgress = Clamp(g_compactTextProgress + dtSec * 2.45, 0.0, 1.0);
        constexpr double kTextSlideOffset = 22.0;
        if (g_compactTextHost) {
            double clipWidth = g_compactTextHost.ActualWidth();
            double clipHeight = g_compactTextHost.ActualHeight();
            if (clipWidth <= 0.0) {
                clipWidth = std::max(1, g_settings.compactWidth - 122);
            }
            if (clipHeight <= 0.0) {
                clipHeight = std::max(1, g_settings.height - 8);
            }
            ApplyElementClip(g_compactTextHost, clipWidth, clipHeight);
        }

        double titleIn = SmootherStep(Clamp((g_compactTextProgress - 0.04) / 0.86, 0.0, 1.0));
        double artistIn = SmootherStep(Clamp((g_compactTextProgress - 0.34) / 0.62, 0.0, 1.0));
        double titleOut = SmootherStep(Clamp(g_compactTextProgress / 0.78, 0.0, 1.0));
        double artistOut = SmootherStep(Clamp((g_compactTextProgress - 0.18) / 0.72, 0.0, 1.0));
        double artIn = SmootherStep(Clamp(g_compactTextProgress / 0.96, 0.0, 1.0));

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
    } else {
        if (g_compactTitleTranslate) g_compactTitleTranslate.X(0.0);
        if (g_compactArtistTranslate) g_compactArtistTranslate.X(0.0);
        if (g_compactTitleText) g_compactTitleText.Opacity(1.0);
        if (g_compactArtistText) g_compactArtistText.Opacity(1.0);
        if (g_compactOutgoingTitleText) g_compactOutgoingTitleText.Opacity(0.0);
        if (g_compactOutgoingArtistText) g_compactOutgoingArtistText.Opacity(0.0);
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
        g_expanded = false;
        SetCompactIslandSuppressed(false);
        return;
    }
    g_popupClosing = true;
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
    if (hwnd) {
        ShowWindow(hwnd, SW_HIDE);
        KillTimer(hwnd, kPopupTimerId);
    }
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
    g_expanded = false;
    try {
        SetCompactIslandSuppressed(false);
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
    double alpha = 1.0 - std::exp(-14.0 * dt);
    g_popupAnimationProgress +=
        (g_popupAnimationTarget - g_popupAnimationProgress) * alpha;

    if (g_popupClosing && g_popupAnimationTarget == 0.0 &&
        g_popupAnimationProgress < 0.085) {
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
                        session.TrySkipPreviousAsync().get();
                    });
                } else if (i == 1) {
                    RunMediaCommand([](gsm::GlobalSystemMediaTransportControlsSession const& session) {
                        session.TryTogglePlayPauseAsync().get();
                    });
                } else {
                    RunMediaCommand([](gsm::GlobalSystemMediaTransportControlsSession const& session) {
                        session.TrySkipNextAsync().get();
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

            g_popupAnimationProgress +=
                (g_popupAnimationTarget - g_popupAnimationProgress) * 0.20;
            if (g_popupClosing && g_popupAnimationTarget == 0.0 &&
                g_popupAnimationProgress < 0.085) {
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

    g_expandedPopup = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kPopupClassName, L"",
        WS_POPUP, 0, 0, g_settings.compactWidth, g_settings.height,
        g_taskbarWnd, nullptr, ModInstance(), nullptr);
    if (!g_expandedPopup) {
        return false;
    }
    if (!InitializePopupXamlHost(g_expandedPopup)) {
        auto style = GetWindowLongPtrW(g_expandedPopup, GWL_EXSTYLE);
        SetWindowLongPtrW(g_expandedPopup, GWL_EXSTYLE, style | WS_EX_LAYERED);
    }
    ApplyPopupBackdrop(g_expandedPopup);
    return true;
}

void UpdatePlayerContents();

void ShowExpandedPopup() {
    if (!EnsureExpandedPopup()) {
        g_expanded = false;
        return;
    }

    g_expanded = true;
    g_popupClosing = false;
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
    SetTimer(g_expandedPopup, kPopupTimerId, g_popupXamlRoot ? 80 : 15, nullptr);
    if (g_popupXamlRoot) {
        StartPopupXamlRenderLoop();
    }
}

void DestroyExpandedPopup() {
    StopPopupXamlRenderLoop();
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
    g_popupXamlControlsScale = nullptr;
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

    g_playerGrid.Width(g_settings.compactWidth);

    if (auto backgroundFe = FindChildByName(g_playerGrid, L"Island_Background")) {
        if (auto background = backgroundFe.try_as<Border>()) {
            background.CornerRadius({20, 20, 20, 20});
        }
    }

    if (auto details = FindChildByName(g_playerGrid, L"Island_Details")) {
        details.Visibility(Visibility::Collapsed);
        details.Opacity(0.0);
    }
    if (auto compactArtist = FindChildByName(g_playerGrid, L"Island_CompactArtist")) {
        compactArtist.Visibility(Visibility::Visible);
        compactArtist.Opacity(1.0);
    }
    if (auto artFe = FindChildByName(g_playerGrid, L"Island_ArtFallback")) {
        artFe.Width(28.0);
        artFe.Height(28.0);
        if (auto art = artFe.try_as<Border>()) {
            art.CornerRadius({8, 8, 8, 8});
        }
    }
    if (auto imageFe = FindChildByName(g_playerGrid, L"Island_AlbumArt")) {
        imageFe.Width(30.0);
        imageFe.Height(30.0);
        imageFe.Margin({-1, -1, -1, -1});
    }
    if (auto fadeFe = FindChildByName(g_playerGrid, L"Island_AlbumArtFade")) {
        fadeFe.Width(30.0);
        fadeFe.Height(30.0);
        fadeFe.Margin({-1, -1, -1, -1});
    }
    if (auto strokeFe = FindChildByName(g_playerGrid, L"Island_ArtStroke")) {
        if (auto stroke = strokeFe.try_as<Border>()) {
            stroke.BorderBrush(IslandBorderBrush());
        }
    }
    if (auto contentFe = FindChildByName(g_playerGrid, L"Island_Content")) {
        if (auto content = contentFe.try_as<Grid>()) {
            if (content.ColumnDefinitions().Size() > 0) {
                content.ColumnDefinitions().GetAt(0).Width({30.0, GridUnitType::Pixel});
            }
        }
    }
}

void StartExpandRenderLoop(bool expanded) {
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
        }
        ShowExpandedPopup();
    } else {
        BeginCloseExpandedPopup();
    }
}

bool InjectIslandGrid();

void MediaThreadProc() {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

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
        manager =
            gsm::GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
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
            if (hwnd) {
                RunFromWindowThread(
                    hwnd,
                    [](void* param) {
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

    winrt::uninit_apartment();
}

void StartMediaThread() {
    if (g_mediaThreadRunning) {
        return;
    }
    g_mediaThreadRunning = true;
    g_mediaThread = new std::thread(MediaThreadProc);
}

void StopMediaThread() {
    g_mediaThreadRunning = false;
    g_mediaCommandCv.notify_all();
    if (g_mediaThread) {
        if (g_mediaThread->joinable()) {
            g_mediaThread->join();
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
    button.BorderBrush(Brush(Color(0x20, 0xFF, 0xFF, 0xFF)));
    button.BorderThickness({1, 1, 1, 1});
    button.CornerRadius({14, 14, 14, 14});

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
            button.BorderBrush(Brush(dark ? Color(0x20, 0xFF, 0xFF, 0xFF)
                                          : Color(0x16, 0x00, 0x00, 0x00)));
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

    UpdateButtonTheme(L"Island_Prev");
    UpdateButtonTheme(L"Island_Play");
    UpdateButtonTheme(L"Island_Next");
}

Grid BuildIslandGrid() {
    Grid wrapper;
    wrapper.Name(L"IslandMedia_Wrapper");
    wrapper.Tag(winrt::box_value(winrt::hstring(L"IslandMediaControls")));
    wrapper.Width(g_settings.compactWidth);
    wrapper.Height(g_settings.height);
    wrapper.MinHeight(g_settings.height);
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
    background.CornerRadius({20, 20, 20, 20});
    background.Background(IslandBackgroundBrush());
    background.BorderBrush(IslandBorderBrush());
    background.BorderThickness({1, 1, 1, 1});
    controls::Canvas::SetZIndex(background, 0);

    Grid content;
    content.Name(L"Island_Content");
    content.Margin({10, 4, 10, 4});
    controls::Canvas::SetZIndex(content, 1);

    ColumnDefinition artCol;
    artCol.Width({30, GridUnitType::Pixel});
    ColumnDefinition textCol;
    textCol.Width({1, GridUnitType::Star});
    ColumnDefinition controlsCol;
    controlsCol.Width({1, GridUnitType::Auto});
    content.ColumnDefinitions().Append(artCol);
    content.ColumnDefinitions().Append(textCol);
    content.ColumnDefinitions().Append(controlsCol);

    Border art;
    art.Name(L"Island_ArtFallback");
    art.Width(28);
    art.Height(28);
    art.CornerRadius({8, 8, 8, 8});
    // Keep the parent transparent. The old blue fallback could show through as a
    // 1px strip on the right/bottom when the generated cover was rounded/scaled.
    art.Background(Brush(Color(0x00, 0x00, 0x00, 0x00)));
    art.BorderThickness({0, 0, 0, 0});
    art.BorderBrush(Brush(Color(0x00, 0x00, 0x00, 0x00)));
    art.VerticalAlignment(VerticalAlignment::Center);

    Grid artHost;
    artHost.Width(28);
    artHost.Height(28);
    ApplyElementClip(artHost, 28.0, 28.0);

    Border artPlaceholder;
    artPlaceholder.Name(L"Island_ArtPlaceholder");
    artPlaceholder.Width(28);
    artPlaceholder.Height(28);
    artPlaceholder.CornerRadius({8, 8, 8, 8});
    artPlaceholder.Background(Brush(Color(0xFF, 0x4F, 0x7D, 0xE8)));
    artPlaceholder.BorderThickness({0, 0, 0, 0});
    artPlaceholder.IsHitTestVisible(false);
    artHost.Children().Append(artPlaceholder);

    Image artFade;
    artFade.Name(L"Island_AlbumArtFade");
    artFade.Width(30);
    artFade.Height(30);
    artFade.Margin({-1, -1, -1, -1});
    artFade.Stretch(mediax::Stretch::UniformToFill);
    artFade.Opacity(0.0);
    artHost.Children().Append(artFade);

    Image artImage;
    artImage.Name(L"Island_AlbumArt");
    artImage.Width(30);
    artImage.Height(30);
    artImage.Margin({-1, -1, -1, -1});
    artImage.Stretch(mediax::Stretch::UniformToFill);
    artImage.Opacity(1.0);
    artHost.Children().Append(artImage);

    Border artStroke;
    artStroke.Name(L"Island_ArtStroke");
    artStroke.Width(28);
    artStroke.Height(28);
    artStroke.CornerRadius({8, 8, 8, 8});
    artStroke.Background(Brush(Color(0x00, 0x00, 0x00, 0x00)));
    artStroke.BorderThickness({1, 1, 1, 1});
    artStroke.BorderBrush(IslandBorderBrush());
    artStroke.IsHitTestVisible(false);
    artHost.Children().Append(artStroke);

    art.Child(artHost);
    g_compactAlbumArtImage = artImage;
    g_compactAlbumArtFade = artFade;

    controls::Grid::SetColumn(art, 0);
    content.Children().Append(art);

    Grid textHost;
    textHost.Name(L"Island_TextHost");
    textHost.VerticalAlignment(VerticalAlignment::Center);
    textHost.Margin({8, 0, 8, 0});
    controls::Grid::SetColumn(textHost, 1);

    StackPanel outgoingTextStack;
    outgoingTextStack.Orientation(controls::Orientation::Vertical);
    outgoingTextStack.VerticalAlignment(VerticalAlignment::Center);
    outgoingTextStack.Opacity(1.0);

    auto outgoingTitle = MakeTextBlock(L"Island_OutgoingTitle", 12, true);
    outgoingTitle.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    outgoingTitle.Opacity(0.0);
    TranslateTransform outgoingTitleTranslate;
    outgoingTitleTranslate.X(0.0);
    outgoingTitle.RenderTransform(outgoingTitleTranslate);

    auto outgoingArtist = MakeTextBlock(L"Island_OutgoingCompactArtist", 10, false);
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
    textStack.VerticalAlignment(VerticalAlignment::Center);

    auto title = MakeTextBlock(L"Island_Title", 12, true);
    title.FontWeight(winrt::Windows::UI::Text::FontWeights::Bold());
    TranslateTransform titleTranslate;
    titleTranslate.X(0.0);
    title.RenderTransform(titleTranslate);
    auto artist = MakeTextBlock(L"Island_CompactArtist", 10, false);
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
    content.Children().Append(textHost);

    StackPanel details;
    details.Name(L"Island_Details");
    details.Orientation(controls::Orientation::Horizontal);
    details.VerticalAlignment(VerticalAlignment::Center);
    details.Visibility(Visibility::Collapsed);
    details.Opacity(0.0);
    controls::Grid::SetColumn(details, 2);

    details.Children().Append(MakeMediaButton(
        L"Island_Prev", L"\uE892",
        [] {
            RunMediaCommand([](gsm::GlobalSystemMediaTransportControlsSession const& s) {
                s.TrySkipPreviousAsync().get();
            });
        }));
    details.Children().Append(MakeMediaButton(
        L"Island_Play", L"\uE768",
        [] {
            RunMediaCommand([](gsm::GlobalSystemMediaTransportControlsSession const& s) {
                s.TryTogglePlayPauseAsync().get();
            });
        }));
    details.Children().Append(MakeMediaButton(
        L"Island_Next", L"\uE893",
        [] {
            RunMediaCommand([](gsm::GlobalSystemMediaTransportControlsSession const& s) {
                s.TrySkipNextAsync().get();
            });
        }));

    ProgressBar progress;
    progress.Name(L"Island_Progress");
    progress.Width(74);
    progress.Height(4);
    progress.Margin({8, 0, 0, 0});
    progress.Minimum(0);
    progress.Maximum(1000);
    progress.Value(0);
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
            StartExpandRenderLoop(!g_expanded);
            e.Handled(true);
        }
    });

    return wrapper;
}

void UpdatePlayerContents() {
    if (!g_playerGrid || g_unloading) {
        return;
    }

    try {
    MediaState state = SnapshotMedia();

    // Some media sessions briefly report a new title before the new thumbnail is
    // available. Keep the previous real artwork during that transient empty
    // thumbnail state to avoid a placeholder-cover flash on every track change.
    static std::vector<uint8_t> s_lastNonEmptyThumbnailBytes;
    std::vector<uint8_t> visualThumbnailBytes = state.thumbnailBytes;
    if (!state.thumbnailBytes.empty()) {
        s_lastNonEmptyThumbnailBytes = state.thumbnailBytes;
    } else if (state.hasSession && !s_lastNonEmptyThumbnailBytes.empty()) {
        visualThumbnailBytes = s_lastNonEmptyThumbnailBytes;
    } else if (!state.hasSession) {
        visualThumbnailBytes.clear();
    }

    std::vector<uint8_t> displayThumbnailBytes = CreateDisplayAlbumCoverBytes(visualThumbnailBytes);
    bool useGeneratedBlurCover = ShouldUseAbstractArtworkForDisplay(visualThumbnailBytes) &&
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
        writer.StoreAsync().get();
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
                if (g_popupXamlOutgoingTitleTranslate) g_popupXamlOutgoingTitleTranslate.X(0.0);
                if (g_popupXamlOutgoingArtistTranslate) g_popupXamlOutgoingArtistTranslate.X(0.0);
                g_popupXamlOutgoingTitle.Opacity(g_popupTextBaseOpacity);
                g_popupXamlOutgoingArtist.Opacity(g_popupTextBaseOpacity);
                if (g_popupXamlTitleTranslate) g_popupXamlTitleTranslate.X(30.0);
                if (g_popupXamlArtistTranslate) g_popupXamlArtistTranslate.X(30.0);
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
                        auto placeholderPanelBytes = CreatePlaceholderAlbumCoverBytes(20);
                        auto placeholderBackdropBytes = CreatePlaceholderAlbumCoverBytes(20, true);
                        if (!placeholderArtBytes.empty()) {
                            g_popupXamlArt.Source(makeBitmap(placeholderArtBytes));
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
                        g_popupXamlArt.Source(makeBitmap(displayThumbnailBytes));
                        if (g_popupXamlBackdropCover) {
                            if (useBackdropCover) {
                                auto lowDetailBackdropBytes =
                                    CreateLowDetailAlbumCoverBytes(popupBlurSourceBytes, true);
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
        g_compactOutgoingTitleText = nullptr;
        g_compactOutgoingArtistText = nullptr;
        g_compactTitleTranslate = nullptr;
        g_compactArtistTranslate = nullptr;
        g_compactOutgoingTitleTranslate = nullptr;
        g_compactOutgoingArtistTranslate = nullptr;
        g_compactAlbumArtImage = nullptr;
        g_compactAlbumArtFade = nullptr;
        g_compactProgress = nullptr;
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
    g_compactOutgoingTitleText = nullptr;
    g_compactOutgoingArtistText = nullptr;
    g_compactTitleTranslate = nullptr;
    g_compactArtistTranslate = nullptr;
    g_compactProgress = nullptr;
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
        ApplyExpandedState();
        UpdatePlayerContents();
        target.grid.UpdateLayout();
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

void ApplySettingsOnTaskbarThread() {
    if (!g_taskbarWnd || !IsWindow(g_taskbarWnd)) {
        g_taskbarWnd = FindCurrentProcessTaskbarWnd();
    }
    if (g_taskbarWnd) {
        RunFromWindowThread(
            g_taskbarWnd,
            [](void*) {
                ApplyPendingSettings();
                InjectIslandGrid();
            },
            nullptr);
    }
}

void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    TrayUI_StartTaskbar_Original(pThis);
    if (g_unloading) {
        return;
    }

    g_taskbarWnd = FindCurrentProcessTaskbarWnd();
    if (g_taskbarWnd) {
        RunFromWindowThread(
            g_taskbarWnd,
            [](void*) {
                ApplyPendingSettings();
                InjectIslandGrid();
            },
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
    g_settings = ReadSettings();

    if (!HookTaskbarSymbols()) {
        Wh_Log(L"Island: failed to hook taskbar symbols");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    g_taskbarWnd = FindCurrentProcessTaskbarWnd();
    StartMediaThread();
    ApplySettingsOnTaskbarThread();
    RequestMediaRefresh();
}

void Wh_ModUninit() {
    g_unloading = true;
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
    Settings settings = ReadSettings();
    {
        std::lock_guard lock(g_pendingSettingsMutex);
        g_pendingSettings = std::move(settings);
    }
    ApplySettingsOnTaskbarThread();
    RequestMediaRefresh();
}

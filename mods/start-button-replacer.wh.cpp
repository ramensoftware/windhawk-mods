// ==WindhawkMod==
// @id              start-button-replacer
// @name            Start Button Replacer
// @description     Replace the Windows 11 Start button icon with a custom SVG, PNG, JPG or GIF image
// @version         1.0.0
// @author          Ender
// @github          https://github.com/EnderDragonEP
// @twitter         https://twitter.com/NoobieNoodle89
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lcomctl32
// @license         GPL-3.0-only
// ==/WindhawkMod==

// Source code is published under the GNU General Public License v3.0 only.
//
// The TaskbarHost/XamlRoot discovery technique is based on the technique used
// by Windhawk's official Windows 11 taskbar mods by m417z.
//
// This mod:
//   * Finds the Windows 11 Start button.
//   * Keeps all original Start button behavior intact.
//   * Makes the stock Start icon transparent.
//   * Inserts two layered Windows.UI.Xaml.Controls.Image elements.
//   * Supports SVG/PNG/JPG/GIF.
//   * Supports local absolute paths, environment variables and HTTP/HTTPS URLs.
//   * Supports animated GIF playback.
//   * Supports crossfading normal, hover and pressed images.
//   * Supports hover/press scale, rotation and opacity effects.
//   * Supports separate images for Windows dark mode.

// ==WindhawkModReadme==
/*
# Start Button Replacer

Replaces the icon inside the Windows 11 Start button with a custom SVG, PNG, JPG or GIF image. Animated GIF playback and hover/pressed effects are optional features.

![preview](https://raw.githubusercontent.com/EnderDragonEP/asset/7ae3c6659e0237bf98cc67f52576ebaff8d9fd44/windhawk-mods/start-button-replacer/preview-1.gif)


## Image sources

Local files:

    C:\Users\YourName\Pictures\start.png

Environment variables are supported:

    %USERPROFILE%\Pictures\start.jpg

HTTP/HTTPS URLs are also supported:

    https://example.com/start.gif

## SVG images

Use a local `.svg` file or an HTTP/HTTPS URL whose path ends in `.svg` (query strings and fragments are supported). A source without a known image extension, such as a URL with no file name, is tried as SVG if it can't be loaded as a bitmap. Every image source supports SVG. Windows renders static SVG content only; SVG scripts, interactions and animations are not supported. Use explicit colors in SVG artwork. Scale, rotation, opacity and crossfade effects still apply.

## Animated GIF playback

- Always
- Hover
- Pressed
- Stopped

## Effects

Separate scale, rotation and opacity settings are available for hover and pressed states.

## Separate state images

The main **Image source** is always the normal image. Add a hover, pressed or activated image source to enable that state automatically. The activated image is shown while the Start menu is open. If it is blank or unavailable, activated falls back to the pressed image, then the hover image, then the main image. Other blank or unavailable state images fall back to the main image.

While the Start menu is shown, pressing the button temporarily uses the pressed state. The activated state resumes on release and otherwise takes priority over the hover state.

State images crossfade whenever their applicable duration is greater than zero. Hover transitions use the hover duration; pressed and activated transitions use the pressed duration. Set a duration to `0` to switch instantly.

## Dark mode images

The main **Images** are used in light mode. To use different images while Windows is in dark mode, fill in **Dark mode images**. Blank fields use the corresponding image from **Images**, and the fallback rules above still apply. If the dark mode **Image source** can't be loaded, the main images are used in dark mode too. The main **Image source** is always required: if it's blank or can't be loaded, the stock Start icon is shown in both modes, even when a dark mode image is set.

Both image sets are loaded up front, so images switch immediately when you change **Choose your mode** in Settings > Personalization > Colors. The mod follows the Windows mode (which the taskbar uses), not the app mode. Icon size, effects and animation settings are shared between modes.

## Notes

For crisp results, use a square image with transparency. Keep animated GIFs reasonably small, such as 64x64 or 128x128. The displayed size is independent from the source image resolution. XAML loads and decodes image sources asynchronously. XAML also caches images by source URI, so after replacing a file at the same path, reload the mod if the previous image remains visible.

## Credits

- [Asteski](https://github.com/Asteski) - SVG support and dark mode images
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- images:
  - imageSource: "https://raw.githubusercontent.com/EnderDragonEP/asset/be76faf00be2a45cb166a4971e67eb32afd4b1fb/windhawk-mods/start-button-replacer/gecko/normal-hover.gif"
    $name: Image source (required)
    $description: >-
      Absolute file path, path containing environment variables, or HTTP/HTTPS URL. Supports SVG, PNG, JPG and animated GIF. This is the normal image and the final fallback for every other state.
  - hoverImageSource: ""
    $name: Hover image source
    $description: >-
      Image shown while the pointer is over the Start button. Leave blank to use the normal image.
  - pressedImageSource: "https://raw.githubusercontent.com/EnderDragonEP/asset/be76faf00be2a45cb166a4971e67eb32afd4b1fb/windhawk-mods/start-button-replacer/gecko/pressed.png"
    $name: Pressed image source
    $description: >-
      Image shown while the Start button is pressed. Leave blank to use the normal image.
  - activatedImageSource: "https://raw.githubusercontent.com/EnderDragonEP/asset/be76faf00be2a45cb166a4971e67eb32afd4b1fb/windhawk-mods/start-button-replacer/gecko/activated.png"
    $name: Activated image source
    $description: >-
      Image shown while the Start menu is open. If blank or unavailable, the pressed image is used, followed by the hover image and normal image.
  - iconSize: 34
    $name: Icon size
    $description: >-
      Displayed size in device-independent pixels, from 8 to 128. The default is 34.
  $name: Images
  $description: Configure the replacement images and their displayed size. These images are also used in dark mode unless Dark mode images are set.

- darkModeImages:
  - imageSource: ""
    $name: Image source
    $description: >-
      Normal image used while Windows is in dark mode. Leave blank to use the main image source.
  - hoverImageSource: ""
    $name: Hover image source
    $description: >-
      Hover image used while Windows is in dark mode. Leave blank to use the main hover image source.
  - pressedImageSource: ""
    $name: Pressed image source
    $description: >-
      Pressed image used while Windows is in dark mode. Leave blank to use the main pressed image source.
  - activatedImageSource: ""
    $name: Activated image source
    $description: >-
      Activated image used while Windows is in dark mode. Leave blank to use the main activated image source.
  $name: Dark mode images
  $description: Optional images used instead of the main images while Windows is in dark mode.

- imageAnimation:
  - gifPlayback: hover
    $name: GIF playback
    $description: Choose when the currently displayed animated GIF plays.
    $options:
    - always: Always
    - hover: Only while hovering
    - pressed: Only while pressed
    - stopped: Never / first frame
  - hoverFadeDuration: 120
    $name: Hover crossfade duration
    $description: >-
      Crossfade duration in milliseconds when entering or leaving the hover state. Set to 0 to switch images instantly. The default is 120.
  - pressedFadeDuration: 80
    $name: Pressed and activated crossfade duration
    $description: >-
      Crossfade duration in milliseconds when entering or leaving the pressed or activated state. Set to 0 to switch images instantly. The default is 80.
  $name: Image animation
  $description: Configure animated GIF playback and crossfades between images.

- hoverEffects:
  - hoverScale: 115
    $name: Scale
    $description: >-
      Icon scale as a percentage while hovering. 100 keeps its size. The default is 115.
  - hoverRotation: 4
    $name: Rotation
    $description: >-
      Rotation in degrees while hovering. 0 disables rotation. The default is 4.
  - hoverOpacity: 100
    $name: Opacity
    $description: >-
      Icon opacity as a percentage while hovering. The default is 100.
  - hoverDuration: 120
    $name: Transition duration
    $description: >-
      Effect transition duration in milliseconds. The default is 120.
  $name: Hover effects
  $description: Configure the visual effect applied while hovering.

- pressedEffects:
  - pressedScale: 95
    $name: Scale
    $description: >-
      Icon scale as a percentage while pressed. 100 keeps its size. The default is 95.
  - pressedRotation: -4
    $name: Rotation
    $description: >-
      Rotation in degrees while pressed. 0 disables rotation. The default is -4.
  - pressedOpacity: 100
    $name: Opacity
    $description: >-
      Icon opacity as a percentage while pressed. The default is 100.
  - pressedDuration: 80
    $name: Transition duration
    $description: >-
      Effect transition duration in milliseconds. The default is 80.
  $name: Pressed effects
  $description: Configure the visual effect applied while pressing the button.

- animationBehavior:
  - releaseDuration: 140
    $name: Release duration
    $description: >-
      Effect transition duration in milliseconds when the pointer leaves or the button is released. The default is 140.
  - respectSystemAnimations: true
    $name: Respect Windows animation setting
    $description: >-
      Stop GIF playback and make all transitions instant when animation effects are disabled in Windows.
  $name: Animation behavior
  $description: Configure shared animation and accessibility behavior.

- showImageLoadFailureWarnings: true
  $name: Show image load failure warnings
  $description: >-
    Show a popup when one or more configured images cannot be loaded. Image-loading errors are always recorded in the Windhawk log.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <commctrl.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cwctype>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

namespace wf = winrt::Windows::Foundation;

namespace wuc = winrt::Windows::UI::Composition;

namespace wux = winrt::Windows::UI::Xaml;

namespace wuxa = winrt::Windows::UI::Xaml::Automation;

namespace wuxap = winrt::Windows::UI::Xaml::Automation::Peers;

namespace wuxc = winrt::Windows::UI::Xaml::Controls;

namespace wuxcp = winrt::Windows::UI::Xaml::Controls::Primitives;

namespace wuxh = winrt::Windows::UI::Xaml::Hosting;

namespace wuxi = winrt::Windows::UI::Xaml::Input;

namespace wuxm = winrt::Windows::UI::Xaml::Media;

namespace wuxmi = winrt::Windows::UI::Xaml::Media::Imaging;

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

enum class GifPlaybackMode {
    Always,
    Hover,
    Pressed,
    Stopped,
};

enum class GifAnimationStatus {
    Unknown,
    NotAnimated,
    Playing,
    Stopped,
    SystemAnimationsDisabled,
};

enum class IconState : size_t {
    Normal,
    Hover,
    Pressed,
    Activated,
    Count,
};

constexpr size_t kIconStateCount = static_cast<size_t>(IconState::Count);

// Every state has one image slot per image set: the main images, followed by
// the dark mode images.
constexpr size_t kImageSetCount = 2;
constexpr size_t kImageSlotCount = kIconStateCount * kImageSetCount;

struct ModSettings {
    // Main image sources, indexed by IconState. The normal one is required.
    std::array<std::wstring, kIconStateCount> imageSources;

    // Optional dark mode overrides, indexed by IconState. Blank entries
    // inherit the corresponding main image source.
    std::array<std::wstring, kIconStateCount> darkModeImages;

    int hoverFadeDuration = 120;
    int pressedFadeDuration = 80;

    int iconSize = 34;

    GifPlaybackMode gifPlayback = GifPlaybackMode::Hover;

    double hoverScale = 1.15;
    double hoverRotation = 4.0;
    double hoverOpacity = 1.0;
    int hoverDuration = 120;

    double pressedScale = 0.95;
    double pressedRotation = -4.0;
    double pressedOpacity = 1.0;
    int pressedDuration = 80;

    int releaseDuration = 140;

    bool respectSystemAnimations = true;
    bool showImageLoadFailureWarnings = true;
};

ModSettings g_settings;
std::mutex g_settingsMutex;
std::atomic<uint64_t> g_settingsGeneration{0};

std::atomic<bool> g_unloading = false;

std::mutex g_imageFailureMessagesMutex;
uint64_t g_imageFailureMessageGeneration = 0;
bool g_imageFailureMessageQueued = false;
HANDLE g_imageFailureMessageThread = nullptr;
std::atomic<HWND> g_imageFailureMessageWindow = nullptr;
std::atomic<bool> g_imageFailureMessageCancelRequested = false;

// -----------------------------------------------------------------------------
// Start icon instance
// -----------------------------------------------------------------------------

struct ImageResource {
    // The source assigned to Image layers. It's either the bitmap (PNG, JPG,
    // GIF) or the SVG source below; the other one stays null.
    wuxm::ImageSource source{nullptr};
    wuxmi::BitmapImage bitmap{nullptr};
    wuxmi::SvgImageSource svg{nullptr};

    // Assigning the source to an Image connected to the XAML tree starts URI
    // retrieval. The visible layers aren't assigned until ImageOpened so the
    // stock icon remains visible while loading.
    wuxc::Image loadTarget{nullptr};

    wuxmi::BitmapImage::ImageOpened_revoker bitmapOpenedRevoker;
    wuxmi::BitmapImage::ImageFailed_revoker bitmapFailedRevoker;
    wuxmi::SvgImageSource::Opened_revoker svgOpenedRevoker;
    wuxmi::SvgImageSource::OpenFailed_revoker svgOpenFailedRevoker;

    // A bitmap which failed to load before the source was retried as SVG. It's
    // kept until detach so its handlers aren't removed while one of them runs.
    wuxmi::BitmapImage retiredBitmap{nullptr};
    bool svgFallbackTried = false;

    // The source as configured, and as the URI passed to XAML.
    std::wstring configuredSource;
    std::wstring resolvedUri;

    bool opened = false;
    bool failed = false;
    bool reportableFailure = false;
};

struct StartIconInstance {
    DWORD threadId = 0;
    ModSettings settings;
    uint64_t settingsGeneration = 0;

    winrt::weak_ref<wux::FrameworkElement> startButton;
    winrt::weak_ref<wux::FrameworkElement> stockIcon;
    winrt::weak_ref<wuxc::Panel> hostPanel;
    winrt::weak_ref<wuxc::Grid> customImageHost;
    winrt::weak_ref<wuxc::Image> customImage;
    winrt::weak_ref<wuxc::Image> transitionImage;

    double originalStockOpacity = 1.0;

    // Indexed by image slot; see ImageSlotIndex.
    std::array<ImageResource, kImageSlotCount> imageResources;

    // Multiple image slots can share one decoded image resource when their
    // sources are identical. A dark mode slot without an override shares the
    // resource of the corresponding main image.
    std::array<size_t, kImageSlotCount> imageResourceIndexBySlot{};
    bool imageLoadingSetupComplete = false;

    // Whether dark mode images are configured (and therefore loaded), and
    // whether Windows currently uses dark mode. Both image sets stay loaded so
    // a theme change only switches the displayed image.
    bool darkImageSetLoaded = false;
    bool darkMode = false;

    size_t activeImageIndex = 0;
    bool activeImageSet = false;
    size_t activeLayerIndex = 0;
    IconState requestedState = IconState::Normal;
    bool requestedStateSet = false;
    IconState displayedState = IconState::Normal;
    bool displayedStateSet = false;

    GifAnimationStatus loggedGifStatus = GifAnimationStatus::Unknown;
    GifPlaybackMode loggedGifMode = GifPlaybackMode::Always;
    IconState loggedGifState = IconState::Normal;
    size_t loggedGifResourceIndex = 0;
    bool gifStatusLogged = false;

    bool hovering = false;
    bool pressed = false;
    bool activated = false;

    int64_t pressedStateCallbackToken = 0;
    bool pressedStateCallbackRegistered = false;

    int64_t activatedStateCallbackToken = 0;
    bool activatedStateCallbackRegistered = false;

    wf::IInspectable pointerEnteredHandler{nullptr};
    wf::IInspectable pointerExitedHandler{nullptr};
    wf::IInspectable pointerPressedHandler{nullptr};
    wf::IInspectable pointerReleasedHandler{nullptr};
    wf::IInspectable pointerCanceledHandler{nullptr};
    wf::IInspectable pointerCaptureLostHandler{nullptr};

    bool pointerEventsAttached = false;

    // SVG images are rasterized for the display scale, so they're rasterized
    // again when the taskbar moves to a monitor with another scale or the
    // scale setting changes.
    wux::XamlRoot::Changed_revoker xamlRootChangedRevoker;
    double rasterizationScale = 0.0;

    bool detached = false;
};

std::mutex g_instancesMutex;

// StartIconInstance owns XAML images and callback delegates. Suppress automatic
// destruction during Explorer process shutdown; controlled unload removes the
// callbacks and releases every instance synchronously on its taskbar UI thread.
[[clang::no_destroy]] std::optional<
    std::vector<std::shared_ptr<StartIconInstance>>>
    g_instances{std::in_place};

// -----------------------------------------------------------------------------
// Utility
// -----------------------------------------------------------------------------

std::wstring GetStringSetting(const wchar_t* name) {
    auto value = WindhawkUtils::StringSetting::make(name);

    return value.get();
}

bool EqualsIgnoreCase(const std::wstring& a, const wchar_t* b) {
    return _wcsicmp(a.c_str(), b) == 0;
}

bool IsBlank(const std::wstring& value) {
    return value.find_first_not_of(L" \t\r\n") == std::wstring::npos;
}

bool StartsWithIgnoreCase(const std::wstring& value, const wchar_t* prefix) {
    size_t prefixLength = wcslen(prefix);

    return value.length() >= prefixLength &&
           _wcsnicmp(value.c_str(), prefix, prefixLength) == 0;
}

bool IsHttpUrl(const std::wstring& source) {
    return StartsWithIgnoreCase(source, L"http://") ||
           StartsWithIgnoreCase(source, L"https://");
}

std::wstring ExpandPath(const std::wstring& source) {
    if (source.empty()) {
        return source;
    }

    DWORD required = ExpandEnvironmentStringsW(source.c_str(), nullptr, 0);

    if (!required) {
        return source;
    }

    std::wstring result(required, L'\0');

    DWORD written =
        ExpandEnvironmentStringsW(source.c_str(), result.data(), required);

    if (!written || written > required) {
        return source;
    }

    result.resize(written - 1);

    return result;
}

std::wstring MakeFileUri(const std::wstring& path) {
    std::wstring result;
    result.reserve(path.size() + 8);
    result = L"file:///";

    for (wchar_t c : path) {
        switch (c) {
            case L'\\':
                result += L'/';
                break;

            case L' ':
                result += L"%20";
                break;

            case L'#':
                result += L"%23";
                break;

            case L'%':
                result += L"%25";
                break;

            default:
                result += c;
                break;
        }
    }

    return result;
}

ModSettings LoadSettings() {
    ModSettings settings;

    // Order matches IconState.
    constexpr const wchar_t* kImageKeys[] = {
        L"imageSource",
        L"hoverImageSource",
        L"pressedImageSource",
        L"activatedImageSource",
    };

    static_assert(ARRAYSIZE(kImageKeys) == kIconStateCount);

    for (size_t i = 0; i < kIconStateCount; i++) {
        settings.imageSources[i] = GetStringSetting(
            (std::wstring(L"images.") + kImageKeys[i]).c_str());

        settings.darkModeImages[i] = GetStringSetting(
            (std::wstring(L"darkModeImages.") + kImageKeys[i]).c_str());
    }

    settings.hoverFadeDuration =
        std::clamp(Wh_GetIntSetting(L"imageAnimation.hoverFadeDuration"), 0,
                   5000);

    settings.pressedFadeDuration =
        std::clamp(Wh_GetIntSetting(L"imageAnimation.pressedFadeDuration"), 0,
                   5000);

    settings.iconSize =
        std::clamp(Wh_GetIntSetting(L"images.iconSize"), 8, 128);

    std::wstring playback =
        GetStringSetting(L"imageAnimation.gifPlayback");

    if (EqualsIgnoreCase(playback, L"hover")) {
        settings.gifPlayback = GifPlaybackMode::Hover;
    } else if (EqualsIgnoreCase(playback, L"pressed")) {
        settings.gifPlayback = GifPlaybackMode::Pressed;
    } else if (EqualsIgnoreCase(playback, L"stopped")) {
        settings.gifPlayback = GifPlaybackMode::Stopped;
    } else {
        settings.gifPlayback = GifPlaybackMode::Always;
    }

    settings.hoverScale =
        std::clamp(Wh_GetIntSetting(L"hoverEffects.hoverScale"), 10, 500) /
        100.0;

    settings.hoverRotation =
        std::clamp(Wh_GetIntSetting(L"hoverEffects.hoverRotation"), -360,
                   360);

    settings.hoverOpacity =
        std::clamp(Wh_GetIntSetting(L"hoverEffects.hoverOpacity"), 0, 100) /
        100.0;

    settings.hoverDuration =
        std::clamp(Wh_GetIntSetting(L"hoverEffects.hoverDuration"), 0, 5000);

    settings.pressedScale =
        std::clamp(Wh_GetIntSetting(L"pressedEffects.pressedScale"), 10, 500) /
        100.0;

    settings.pressedRotation =
        std::clamp(Wh_GetIntSetting(L"pressedEffects.pressedRotation"), -360,
                   360);

    settings.pressedOpacity =
        std::clamp(Wh_GetIntSetting(L"pressedEffects.pressedOpacity"), 0,
                   100) /
        100.0;

    settings.pressedDuration =
        std::clamp(Wh_GetIntSetting(L"pressedEffects.pressedDuration"), 0,
                   5000);

    settings.releaseDuration =
        std::clamp(Wh_GetIntSetting(L"animationBehavior.releaseDuration"), 0,
                   5000);

    settings.respectSystemAnimations =
        Wh_GetIntSetting(L"animationBehavior.respectSystemAnimations") != 0;

    settings.showImageLoadFailureWarnings =
        Wh_GetIntSetting(L"showImageLoadFailureWarnings") != 0;

    return settings;
}

uint64_t StoreSettings(ModSettings settings) {
    std::lock_guard<std::mutex> lock(g_settingsMutex);

    g_settings = std::move(settings);

    return g_settingsGeneration.fetch_add(1, std::memory_order_relaxed) + 1;
}

ModSettings GetSettingsSnapshot(uint64_t* generation = nullptr) {
    std::lock_guard<std::mutex> lock(g_settingsMutex);

    if (generation) {
        *generation =
            g_settingsGeneration.load(std::memory_order_relaxed);
    }

    return g_settings;
}

// -----------------------------------------------------------------------------
// Dark mode images
// -----------------------------------------------------------------------------

bool HasDarkModeImages(const ModSettings& settings) {
    for (size_t i = 0; i < kIconStateCount; i++) {
        if (!IsBlank(settings.darkModeImages[i])) {
            return true;
        }
    }

    return false;
}

// Reads the Windows mode, which the taskbar follows. Returns nullopt if the
// value can't be read.
std::optional<bool> IsSystemLightTheme() {
    DWORD lightTheme = 0;
    DWORD size = sizeof(lightTheme);

    LSTATUS status = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &lightTheme,
        &size);

    if (status != ERROR_SUCCESS) {
        return std::nullopt;
    }

    return lightTheme != 0;
}

size_t ImageSlotIndex(bool darkSet, IconState state) {
    return (darkSet ? kIconStateCount : 0) + static_cast<size_t>(state);
}

bool IsDarkImageSlot(size_t slotIndex) {
    return slotIndex >= kIconStateCount;
}

IconState ImageSlotState(size_t slotIndex) {
    return static_cast<IconState>(slotIndex % kIconStateCount);
}

const wchar_t* ImageSlotName(size_t slotIndex) {
    constexpr const wchar_t* kNames[] = {
        L"normal",           L"hover",
        L"pressed",          L"activated",
        L"dark mode normal", L"dark mode hover",
        L"dark mode pressed", L"dark mode activated",
    };

    static_assert(ARRAYSIZE(kNames) == kImageSlotCount);

    return slotIndex < kImageSlotCount ? kNames[slotIndex] : L"unknown";
}

// Whether the slot has a setting of its own, as opposed to a dark mode slot
// which inherits the main image.
bool ImageSlotConfigured(const ModSettings& settings, size_t slotIndex) {
    size_t stateIndex = slotIndex % kIconStateCount;

    return IsDarkImageSlot(slotIndex)
               ? !IsBlank(settings.darkModeImages[stateIndex])
               : !settings.imageSources[stateIndex].empty();
}

// Returns the image source which a slot displays.
const std::wstring& GetImageSourceForSlot(const ModSettings& settings,
                                          size_t slotIndex) {
    size_t stateIndex = slotIndex % kIconStateCount;

    if (IsDarkImageSlot(slotIndex) &&
        !IsBlank(settings.darkModeImages[stateIndex])) {
        return settings.darkModeImages[stateIndex];
    }

    return settings.imageSources[stateIndex];
}

// -----------------------------------------------------------------------------
// Windows animation accessibility setting
// -----------------------------------------------------------------------------

bool MotionAllowed(const ModSettings& settings) {
    if (!settings.respectSystemAnimations) {
        return true;
    }

    BOOL animationsEnabled = TRUE;

    if (!SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0,
                               &animationsEnabled, 0)) {
        return true;
    }

    return animationsEnabled != FALSE;
}

// -----------------------------------------------------------------------------
// Composition effects
// -----------------------------------------------------------------------------

void SetVisualImmediately(const wuc::Visual& visual,
                          float scale,
                          float rotation,
                          float opacity) {
    visual.StopAnimation(L"Scale");
    visual.StopAnimation(L"RotationAngleInDegrees");
    visual.StopAnimation(L"Opacity");

    visual.Scale({scale, scale, 1.0f});

    visual.RotationAngleInDegrees(rotation);

    visual.Opacity(opacity);
}

void AnimateVisual(const wux::FrameworkElement& element,
                   const ModSettings& settings,
                   float scale,
                   float rotation,
                   float opacity,
                   int durationMs) {
    if (!element) {
        return;
    }

    try {
        auto visual =
            wuxh::ElementCompositionPreview::GetElementVisual(element);

        if (!visual) {
            return;
        }

        float width = static_cast<float>(element.ActualWidth());

        float height = static_cast<float>(element.ActualHeight());

        if (width <= 0) {
            width = static_cast<float>(settings.iconSize);
        }

        if (height <= 0) {
            height = static_cast<float>(settings.iconSize);
        }

        visual.CenterPoint({width / 2.0f, height / 2.0f, 0.0f});

        if (!MotionAllowed(settings) || durationMs <= 0) {
            SetVisualImmediately(visual, scale, rotation, opacity);

            return;
        }

        auto compositor = visual.Compositor();

        auto easing = compositor.CreateCubicBezierEasingFunction(
            {0.20f, 0.00f}, {0.00f, 1.00f});

        // Scale.
        {
            auto animation = compositor.CreateVector3KeyFrameAnimation();

            animation.InsertKeyFrame(1.0f, {scale, scale, 1.0f}, easing);

            animation.Duration(std::chrono::milliseconds(durationMs));

            visual.StartAnimation(L"Scale", animation);
        }

        // Rotation.
        {
            auto animation = compositor.CreateScalarKeyFrameAnimation();

            animation.InsertKeyFrame(1.0f, rotation, easing);

            animation.Duration(std::chrono::milliseconds(durationMs));

            visual.StartAnimation(L"RotationAngleInDegrees", animation);
        }

        // Opacity.
        {
            auto animation = compositor.CreateScalarKeyFrameAnimation();

            animation.InsertKeyFrame(1.0f, opacity, easing);

            animation.Duration(std::chrono::milliseconds(durationMs));

            visual.StartAnimation(L"Opacity", animation);
        }
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"AnimateVisual failed: 0x%08X %s", e.code().value,
               e.message().c_str());
    }
}

// -----------------------------------------------------------------------------
// GIF playback
// -----------------------------------------------------------------------------

size_t IconStateIndex(IconState state) {
    return static_cast<size_t>(state);
}

const wchar_t* IconStateName(IconState state) {
    switch (state) {
        case IconState::Normal:
            return L"normal";

        case IconState::Hover:
            return L"hover";

        case IconState::Pressed:
            return L"pressed";

        case IconState::Activated:
            return L"activated";

        default:
            return L"unknown";
    }
}

constexpr wchar_t kImageFailureMessageTitle[] =
    L"Start Button Replacer - Windhawk";

const wchar_t* IconStateSettingName(IconState state) {
    switch (state) {
        case IconState::Normal:
            return L"Image source";

        case IconState::Hover:
            return L"Hover image source";

        case IconState::Pressed:
            return L"Pressed image source";

        case IconState::Activated:
            return L"Activated image source";

        default:
            return L"Image source";
    }
}

struct ImageFailureDetail {
    IconState state = IconState::Normal;
    bool darkModeSetting = false;
};

std::wstring BuildImageFailureMessage(
    const std::vector<ImageFailureDetail>& failures) {
    if (failures.empty()) {
        return L"";
    }

    bool mainImageFailed = failures.front().state == IconState::Normal &&
                           !failures.front().darkModeSetting;
    bool multipleFailures = failures.size() > 1;

    std::wstring message = multipleFailures
                               ? L"The following images could not be loaded:\n\n"
                               : L"The following image could not be loaded:\n\n";

    for (size_t i = 0; i < failures.size(); i++) {
        message += L"- ";
        message += failures[i].darkModeSetting ? L"Dark mode images"
                                               : L"Images";
        message += L" > ";
        message += IconStateSettingName(failures[i].state);
        message += L"\n";
    }

    if (mainImageFailed) {
        message +=
            L"\nNo custom Start icon is applied because Image source is "
            L"required.";
    } else if (multipleFailures) {
        message +=
            L"\nThe custom Start icon remains active, using available "
            L"fallback images for the affected states.";
    } else {
        message +=
            L"\nThe custom Start icon remains active, using an available "
            L"fallback image for the affected state.";
    }

    message += L"\n\nCheck the image settings and Windhawk log for details.";

    return message;
}

struct ImageFailureMessageParams {
    std::wstring message;
    uint64_t settingsGeneration = 0;
};

DWORD WINAPI ImageFailureMessageThreadProc(void* parameter) {
    std::unique_ptr<ImageFailureMessageParams> params(
        static_cast<ImageFailureMessageParams*>(parameter));

    if (g_unloading || g_imageFailureMessageCancelRequested ||
        params->settingsGeneration !=
            g_settingsGeneration.load(std::memory_order_relaxed)) {
        return 0;
    }

    HMODULE comctl32 = GetModuleHandleW(L"comctl32.dll");
    auto taskDialogIndirect = comctl32
                                  ? reinterpret_cast<decltype(
                                        &TaskDialogIndirect)>(GetProcAddress(
                                        comctl32, "TaskDialogIndirect"))
                                  : nullptr;

    if (!taskDialogIndirect) {
        Wh_Log(L"TaskDialogIndirect is unavailable; image failures are in the "
               L"Windhawk log");
        return 0;
    }

    TASKDIALOGCONFIG config{
        .cbSize = sizeof(config),
        .dwFlags = TDF_ALLOW_DIALOG_CANCELLATION,
        .dwCommonButtons = TDCBF_OK_BUTTON,
        .pszWindowTitle = kImageFailureMessageTitle,
        .pszMainIcon = TD_WARNING_ICON,
        .pszContent = params->message.c_str(),
        .pfCallback = [](HWND hwnd, UINT notification, WPARAM, LPARAM,
                         LONG_PTR callbackData) WINAPI -> HRESULT {
            auto params = reinterpret_cast<ImageFailureMessageParams*>(
                callbackData);

            switch (notification) {
                case TDN_CREATED:
                    g_imageFailureMessageWindow = hwnd;
                    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                                 SWP_NOMOVE | SWP_NOSIZE);

                    if (g_unloading ||
                        g_imageFailureMessageCancelRequested || !params ||
                        params->settingsGeneration != g_settingsGeneration.load(
                                                          std::memory_order_relaxed)) {
                        PostMessageW(hwnd, WM_CLOSE, 0, 0);
                    }
                    break;

                case TDN_DESTROYED: {
                    HWND expected = hwnd;
                    g_imageFailureMessageWindow.compare_exchange_strong(
                        expected, nullptr);
                    break;
                }
            }

            return S_OK;
        },
        .lpCallbackData = reinterpret_cast<LONG_PTR>(params.get()),
    };

    int button = 0;
    HRESULT result = taskDialogIndirect(&config, &button, nullptr, nullptr);

    if (FAILED(result)) {
        Wh_Log(L"Showing the image failure dialog failed: 0x%08X",
               result);
    }

    return 0;
}

void StopImageFailureMessageThread() {
    HANDLE thread = nullptr;

    {
        std::lock_guard<std::mutex> lock(g_imageFailureMessagesMutex);
        g_imageFailureMessageCancelRequested = true;
        thread = g_imageFailureMessageThread;
    }

    if (HWND window = g_imageFailureMessageWindow.load()) {
        PostMessageW(window, WM_CLOSE, 0, 0);
    }

    if (!thread) {
        return;
    }

    DWORD waitResult = WaitForSingleObject(thread, INFINITE);

    if (waitResult == WAIT_FAILED) {
        Wh_Log(L"Waiting for the image failure dialog thread failed: %u",
               GetLastError());
    }

    g_imageFailureMessageWindow = nullptr;
    CloseHandle(thread);

    std::lock_guard<std::mutex> lock(g_imageFailureMessagesMutex);

    if (g_imageFailureMessageThread == thread) {
        g_imageFailureMessageThread = nullptr;
    }
}

void ResetReportedImageFailures(uint64_t settingsGeneration) {
    StopImageFailureMessageThread();

    std::lock_guard<std::mutex> lock(g_imageFailureMessagesMutex);

    g_imageFailureMessageGeneration = settingsGeneration;
    g_imageFailureMessageQueued = false;
    g_imageFailureMessageCancelRequested = false;
}

void ForgetReportedImageFailure(uint64_t settingsGeneration) {
    std::lock_guard<std::mutex> lock(g_imageFailureMessagesMutex);

    if (g_imageFailureMessageGeneration == settingsGeneration) {
        g_imageFailureMessageQueued = false;
    }
}

void ShowImageFailureMessage(uint64_t settingsGeneration,
                             const std::vector<ImageFailureDetail>& failures)
    noexcept {
    if (!settingsGeneration || failures.empty() || g_unloading ||
        settingsGeneration !=
            g_settingsGeneration.load(std::memory_order_relaxed)) {
        return;
    }

    bool messageMarkedAsQueued = false;

    try {
        {
            std::lock_guard<std::mutex> lock(g_imageFailureMessagesMutex);

            if (g_unloading ||
                settingsGeneration !=
                    g_settingsGeneration.load(std::memory_order_relaxed) ||
                g_imageFailureMessageGeneration != settingsGeneration ||
                g_imageFailureMessageQueued) {
                return;
            }

            g_imageFailureMessageQueued = true;
            messageMarkedAsQueued = true;
        }

        auto params = std::make_unique<ImageFailureMessageParams>();
        params->message = BuildImageFailureMessage(failures);
        params->settingsGeneration = settingsGeneration;

        if (g_unloading ||
            settingsGeneration !=
                g_settingsGeneration.load(std::memory_order_relaxed)) {
            ForgetReportedImageFailure(settingsGeneration);
            return;
        }

        {
            std::lock_guard<std::mutex> lock(g_imageFailureMessagesMutex);

            if (g_unloading || g_imageFailureMessageCancelRequested ||
                settingsGeneration !=
                    g_settingsGeneration.load(std::memory_order_relaxed) ||
                g_imageFailureMessageGeneration != settingsGeneration ||
                g_imageFailureMessageThread) {
                if (g_imageFailureMessageGeneration == settingsGeneration) {
                    g_imageFailureMessageQueued = false;
                }

                return;
            }

            auto rawParams = params.release();
            HANDLE thread =
                CreateThread(nullptr, 0, ImageFailureMessageThreadProc,
                             rawParams, 0, nullptr);

            if (!thread) {
                DWORD error = GetLastError();
                params.reset(rawParams);
                g_imageFailureMessageQueued = false;
                Wh_Log(L"Creating the image failure message thread failed: %u",
                       error);
                return;
            }

            g_imageFailureMessageThread = thread;
        }
    } catch (...) {
        if (messageMarkedAsQueued) {
            try {
                ForgetReportedImageFailure(settingsGeneration);
            } catch (...) {
            }
        }

        Wh_Log(L"Preparing the image failure message failed");
    }
}

IconState GetCurrentIconState(
    const std::shared_ptr<StartIconInstance>& instance) {
    if (instance && instance->pressed) {
        return IconState::Pressed;
    }

    if (instance && instance->activated) {
        return IconState::Activated;
    }

    if (instance && instance->hovering) {
        return IconState::Hover;
    }

    return IconState::Normal;
}

bool ImageResourceAvailable(const ImageResource& resource) {
    return resource.source && resource.opened && !resource.failed;
}

wuxc::Image GetImageLayer(const std::shared_ptr<StartIconInstance>& instance,
                          size_t layerIndex) {
    if (!instance) {
        return nullptr;
    }

    return layerIndex == 0 ? instance->customImage.get()
                           : instance->transitionImage.get();
}

bool ImageSourcesEqual(const std::wstring& first, const std::wstring& second) {
    if (first.empty() || second.empty()) {
        return false;
    }

    if (first == second) {
        return true;
    }

    // URL paths can be case-sensitive. Only reuse URL resources when the
    // configured strings match exactly.
    if (IsHttpUrl(first) || IsHttpUrl(second)) {
        return false;
    }

    std::wstring expandedFirst = ExpandPath(first);
    std::wstring expandedSecond = ExpandPath(second);

    return _wcsicmp(expandedFirst.c_str(), expandedSecond.c_str()) == 0;
}

void SetImageLayerOpacity(const wuxc::Image& image, float opacity) {
    if (!image) {
        return;
    }

    try {
        auto visual = wuxh::ElementCompositionPreview::GetElementVisual(image);

        if (!visual) {
            return;
        }

        visual.StopAnimation(L"Opacity");
        visual.Opacity(opacity);
    } catch (...) {
    }
}

const wchar_t* GifPlaybackModeName(GifPlaybackMode mode) {
    switch (mode) {
        case GifPlaybackMode::Always:
            return L"always";

        case GifPlaybackMode::Hover:
            return L"hover";

        case GifPlaybackMode::Pressed:
            return L"pressed";

        case GifPlaybackMode::Stopped:
            return L"never/first-frame";

        default:
            return L"unknown";
    }
}

const wchar_t* GifAnimationStatusName(GifAnimationStatus status) {
    switch (status) {
        case GifAnimationStatus::NotAnimated:
            return L"not-animated";

        case GifAnimationStatus::Playing:
            return L"playing";

        case GifAnimationStatus::Stopped:
            return L"stopped";

        case GifAnimationStatus::SystemAnimationsDisabled:
            return L"stopped-system-animations-disabled";

        default:
            return L"unknown";
    }
}

void AnimateImageLayerOpacity(const wuxc::Image& image,
                              const ModSettings& settings,
                              float fromOpacity,
                              float toOpacity,
                              int durationMs) {
    if (!image) {
        return;
    }

    if (!MotionAllowed(settings) || durationMs <= 0) {
        SetImageLayerOpacity(image, toOpacity);
        return;
    }

    try {
        auto visual = wuxh::ElementCompositionPreview::GetElementVisual(image);

        if (!visual) {
            return;
        }

        visual.StopAnimation(L"Opacity");
        visual.Opacity(fromOpacity);

        auto compositor = visual.Compositor();

        auto easing = compositor.CreateCubicBezierEasingFunction(
            {0.20f, 0.00f}, {0.00f, 1.00f});

        auto animation = compositor.CreateScalarKeyFrameAnimation();

        animation.InsertKeyFrame(0.0f, fromOpacity);

        animation.InsertKeyFrame(1.0f, toOpacity, easing);

        animation.Duration(std::chrono::milliseconds(durationMs));

        visual.StartAnimation(L"Opacity", animation);
    } catch (...) {
        SetImageLayerOpacity(image, toOpacity);
    }
}

int GetStateCrossfadeDuration(
    const std::shared_ptr<StartIconInstance>& instance,
    IconState nextState) {
    if (!instance) {
        return 0;
    }

    if (nextState == IconState::Pressed || nextState == IconState::Activated ||
        (instance->displayedStateSet &&
         (instance->displayedState == IconState::Pressed ||
          instance->displayedState == IconState::Activated))) {
        return instance->settings.pressedFadeDuration;
    }

    return instance->settings.hoverFadeDuration;
}

bool UpdateDisplayedImage(const std::shared_ptr<StartIconInstance>& instance) {
    if (!instance) {
        return false;
    }

    auto firstLayer = GetImageLayer(instance, 0);
    auto secondLayer = GetImageLayer(instance, 1);

    if (!firstLayer || !secondLayer) {
        return false;
    }

    auto setStockOpacity = [&](double opacity) {
        auto stockIcon = instance->stockIcon.get();

        if (stockIcon) {
            try {
                stockIcon.Opacity(opacity);
            } catch (...) {
            }
        }
    };

    auto useStockIcon = [&]() {
        for (size_t layerIndex = 0; layerIndex < 2; layerIndex++) {
            auto layer = GetImageLayer(instance, layerIndex);

            try {
                layer.Source(wuxm::ImageSource{nullptr});
            } catch (...) {
            }

            SetImageLayerOpacity(layer, 0.0f);
        }

        instance->activeImageSet = false;
        instance->displayedState = IconState::Normal;
        instance->displayedStateSet = true;

        setStockOpacity(instance->originalStockOpacity);

        return false;
    };

    IconState desiredState = GetCurrentIconState(instance);

    auto resourceIndexFor = [&](bool darkSet, IconState state) {
        return instance->imageResourceIndexBySlot[ImageSlotIndex(darkSet,
                                                                 state)];
    };

    size_t normalResourceIndex = resourceIndexFor(false, IconState::Normal);

    // The main image is required. Keep the stock icon visible until it has
    // opened successfully, and never display optional state images if it fails.
    if (!ImageResourceAvailable(
            instance->imageResources[normalResourceIndex])) {
        return useStockIcon();
    }

    bool darkSet = instance->darkImageSetLoaded && instance->darkMode;

    if (darkSet) {
        const auto& darkNormalResource =
            instance->imageResources[resourceIndexFor(true,
                                                      IconState::Normal)];

        if (darkNormalResource.failed) {
            // The dark mode normal image is unavailable. Use the main images.
            darkSet = false;
        } else if (!ImageResourceAvailable(darkNormalResource)) {
            // Still loading. Keep the stock icon at startup instead of showing
            // the main image first; after a theme change, keep displaying the
            // main images until the dark mode image opens.
            if (!instance->activeImageSet) {
                return useStockIcon();
            }

            darkSet = false;
        }
    }

    if (darkSet) {
        normalResourceIndex = resourceIndexFor(true, IconState::Normal);
    }

    size_t selectedIndex = normalResourceIndex;
    IconState selectedState = IconState::Normal;

    auto selectIfAvailable = [&](IconState state) {
        size_t stateIndex = resourceIndexFor(darkSet, state);

        if (!ImageResourceAvailable(instance->imageResources[stateIndex])) {
            return false;
        }

        selectedIndex = stateIndex;
        selectedState = state;
        return true;
    };

    if (desiredState == IconState::Activated) {
        if (!selectIfAvailable(IconState::Activated) &&
            !selectIfAvailable(IconState::Pressed) &&
            !selectIfAvailable(IconState::Hover)) {
            selectIfAvailable(IconState::Normal);
        }
    } else if (!selectIfAvailable(desiredState)) {
        selectIfAvailable(IconState::Normal);
    }

    auto& selectedResource = instance->imageResources[selectedIndex];

    bool selectionChanged = !instance->requestedStateSet ||
                            instance->requestedState != desiredState ||
                            !instance->displayedStateSet ||
                            instance->displayedState != selectedState ||
                            (instance->activeImageSet &&
                             instance->activeImageIndex != selectedIndex);

    instance->requestedState = desiredState;
    instance->requestedStateSet = true;

    if (selectionChanged) {
        Wh_Log(L"Start icon state: requested=%s selected=%s images=%s",
               IconStateName(desiredState), IconStateName(selectedState),
               darkSet ? L"dark mode" : L"main");
    }

    if (!ImageResourceAvailable(selectedResource)) {
        return useStockIcon();
    }

    if (instance->activeImageSet &&
        instance->activeImageIndex == selectedIndex) {
        instance->displayedState = selectedState;
        instance->displayedStateSet = true;
        setStockOpacity(0.0);
        return true;
    }

    try {
        if (!instance->activeImageSet) {
            firstLayer.Source(selectedResource.source);

            SetImageLayerOpacity(firstLayer, 1.0f);
            SetImageLayerOpacity(secondLayer, 0.0f);

            instance->activeLayerIndex = 0;
        } else {
            size_t outgoingLayerIndex = instance->activeLayerIndex;
            size_t incomingLayerIndex = 1 - outgoingLayerIndex;

            auto outgoingLayer = GetImageLayer(instance, outgoingLayerIndex);
            auto incomingLayer = GetImageLayer(instance, incomingLayerIndex);

            incomingLayer.Source(selectedResource.source);

            int fadeDuration =
                GetStateCrossfadeDuration(instance, selectedState);

            AnimateImageLayerOpacity(outgoingLayer, instance->settings, 1.0f,
                                     0.0f, fadeDuration);

            AnimateImageLayerOpacity(incomingLayer, instance->settings, 0.0f,
                                     1.0f, fadeDuration);

            instance->activeLayerIndex = incomingLayerIndex;
        }

        instance->activeImageIndex = selectedIndex;
        instance->activeImageSet = true;
        instance->displayedState = selectedState;
        instance->displayedStateSet = true;

        setStockOpacity(0.0);

        return true;
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Switching Start image failed: 0x%08X %s", e.code().value,
               e.message().c_str());
    } catch (...) {
        Wh_Log(L"Switching Start image failed");
    }

    instance->activeImageSet = false;
    setStockOpacity(instance->originalStockOpacity);

    return false;
}

void LogGifAnimationStatus(const std::shared_ptr<StartIconInstance>& instance,
                           GifAnimationStatus status) {
    if (!instance || !instance->activeImageSet) {
        return;
    }

    IconState state = instance->displayedStateSet
                          ? instance->displayedState
                          : GetCurrentIconState(instance);

    if (instance->gifStatusLogged && instance->loggedGifStatus == status &&
        instance->loggedGifMode == instance->settings.gifPlayback &&
        instance->loggedGifState == state &&
        instance->loggedGifResourceIndex == instance->activeImageIndex) {
        return;
    }

    instance->loggedGifStatus = status;
    instance->loggedGifMode = instance->settings.gifPlayback;
    instance->loggedGifState = state;
    instance->loggedGifResourceIndex = instance->activeImageIndex;
    instance->gifStatusLogged = true;

    Wh_Log(L"Icon Status: state=%s resource=%s mode=%s status=%s",
           IconStateName(state), ImageSlotName(instance->activeImageIndex),
           GifPlaybackModeName(instance->settings.gifPlayback),
           GifAnimationStatusName(status));
}

void UpdateGifPlayback(const std::shared_ptr<StartIconInstance>& instance) {
    if (!instance) {
        return;
    }

    for (size_t i = 0; i < kImageSlotCount; i++) {
        auto& resource = instance->imageResources[i];

        if (!resource.bitmap ||
            (instance->activeImageSet && instance->activeImageIndex == i)) {
            continue;
        }

        try {
            resource.bitmap.Stop();
        } catch (...) {
        }
    }

    if (!instance->activeImageSet) {
        return;
    }

    auto& activeResource = instance->imageResources[instance->activeImageIndex];

    if (!ImageResourceAvailable(activeResource)) {
        return;
    }

    try {
        // SVG sources are never animated.
        if (!activeResource.bitmap ||
            !activeResource.bitmap.IsAnimatedBitmap()) {
            LogGifAnimationStatus(instance, GifAnimationStatus::NotAnimated);
            return;
        }

        if (!MotionAllowed(instance->settings)) {
            activeResource.bitmap.Stop();

            LogGifAnimationStatus(instance,
                                  GifAnimationStatus::SystemAnimationsDisabled);
            return;
        }

        bool play = false;

        switch (instance->settings.gifPlayback) {
            case GifPlaybackMode::Always:
                play = true;
                break;

            case GifPlaybackMode::Hover:
                play = instance->hovering;
                break;

            case GifPlaybackMode::Pressed:
                play = instance->pressed;
                break;

            case GifPlaybackMode::Stopped:
                play = false;
                break;
        }

        if (play) {
            activeResource.bitmap.Play();

            LogGifAnimationStatus(instance, GifAnimationStatus::Playing);
        } else {
            activeResource.bitmap.Stop();

            LogGifAnimationStatus(instance, GifAnimationStatus::Stopped);
        }
    } catch (...) {
        // Non-animated images or images that
        // haven't finished loading yet can
        // simply ignore playback control.
    }
}

// -----------------------------------------------------------------------------
// Visual states
// -----------------------------------------------------------------------------

void ApplyNormalState(const std::shared_ptr<StartIconInstance>& instance,
                      int duration) {
    auto imageHost = instance->customImageHost.get();

    if (!imageHost) {
        return;
    }

    AnimateVisual(imageHost, instance->settings, 1.0f, 0.0f, 1.0f, duration);
}

void ApplyHoverState(const std::shared_ptr<StartIconInstance>& instance,
                     int duration) {
    auto imageHost = instance->customImageHost.get();

    if (!imageHost) {
        return;
    }

    AnimateVisual(imageHost, instance->settings,
                  static_cast<float>(instance->settings.hoverScale),
                  static_cast<float>(instance->settings.hoverRotation),
                  static_cast<float>(instance->settings.hoverOpacity),
                  duration);
}

void ApplyPressedState(const std::shared_ptr<StartIconInstance>& instance,
                       int duration) {
    auto imageHost = instance->customImageHost.get();

    if (!imageHost) {
        return;
    }

    AnimateVisual(imageHost, instance->settings,
                  static_cast<float>(instance->settings.pressedScale),
                  static_cast<float>(instance->settings.pressedRotation),
                  static_cast<float>(instance->settings.pressedOpacity),
                  duration);
}

void ApplyCurrentState(const std::shared_ptr<StartIconInstance>& instance,
                       int duration) {
    if (!instance) {
        return;
    }

    if (!UpdateDisplayedImage(instance)) {
        return;
    }

    if (instance->pressed) {
        ApplyPressedState(instance, duration);
    } else if (instance->hovering) {
        ApplyHoverState(instance, duration);
    } else {
        ApplyNormalState(instance, duration);
    }

    UpdateGifPlayback(instance);
}

void SetPressedState(const std::shared_ptr<StartIconInstance>& instance,
                     bool pressed) {
    if (!instance || g_unloading || instance->pressed == pressed) {
        return;
    }

    instance->pressed = pressed;

    if (pressed) {
        Wh_Log(L"Start button: PRESSED");
    } else {
        Wh_Log(L"Start button: RELEASED");
    }

    ApplyCurrentState(instance, pressed ? instance->settings.pressedDuration
                                        : instance->settings.releaseDuration);
}

bool IsToggleButtonChecked(const wuxcp::ToggleButton& toggleButton) {
    if (!toggleButton) {
        return false;
    }

    auto isChecked = toggleButton.IsChecked();

    return isChecked && isChecked.Value();
}

void SetActivatedState(const std::shared_ptr<StartIconInstance>& instance,
                       bool activated) {
    if (!instance || g_unloading || instance->activated == activated) {
        return;
    }

    instance->activated = activated;

    if (activated) {
        Wh_Log(L"Start menu: SHOWN");
    } else {
        Wh_Log(L"Start menu: HIDDEN");
    }

    UpdateDisplayedImage(instance);
    UpdateGifPlayback(instance);
}

// -----------------------------------------------------------------------------
// XAML tree search
// -----------------------------------------------------------------------------

wux::FrameworkElement FindElementRecursive(
    const wux::DependencyObject& root,
    const std::function<bool(const wux::FrameworkElement&)>& callback) {
    if (!root) {
        return nullptr;
    }

    int childrenCount = 0;

    try {
        childrenCount = wuxm::VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return nullptr;
    }

    for (int i = 0; i < childrenCount; i++) {
        wux::DependencyObject child{nullptr};

        try {
            child = wuxm::VisualTreeHelper::GetChild(root, i);
        } catch (...) {
            continue;
        }

        if (!child) {
            continue;
        }

        auto frameworkElement = child.try_as<wux::FrameworkElement>();

        if (frameworkElement) {
            try {
                if (callback(frameworkElement)) {
                    return frameworkElement;
                }
            } catch (...) {
            }
        }

        auto recursiveResult = FindElementRecursive(child, callback);

        if (recursiveResult) {
            return recursiveResult;
        }
    }

    return nullptr;
}

wux::FrameworkElement FindStartButton(const wux::FrameworkElement& root) {
    return FindElementRecursive(root, [](const wux::FrameworkElement& element) {
        try {
            auto automationId =
                wuxa::AutomationProperties::GetAutomationId(element);

            if (automationId == L"StartButton") {
                return true;
            }
        } catch (...) {
        }

        return false;
    });
}

wux::FrameworkElement FindStockStartIcon(
    const wux::FrameworkElement& startButton) {
    // Preferred match:
    //
    // Microsoft.UI.Xaml.Controls
    //     .AnimatedVisualPlayer#Icon

    auto exact = FindElementRecursive(
        startButton, [](const wux::FrameworkElement& element) {
            try {
                if (element.Name() != L"Icon") {
                    return false;
                }

                std::wstring className = winrt::get_class_name(element).c_str();

                return className.find(L"AnimatedVisualPlayer") !=
                       std::wstring::npos;
            } catch (...) {
                return false;
            }
        });

    if (exact) {
        return exact;
    }

    // Fail closed on unknown Windows layouts. Hiding an arbitrary descendant
    // named "Icon" can make the Start button unusable after a Windows update.
    return nullptr;
}

wuxc::Panel FindHostPanel(const wux::FrameworkElement& stockIcon,
                          const wux::FrameworkElement& startButton) {
    wux::DependencyObject current = stockIcon;

    while (current) {
        try {
            current = wuxm::VisualTreeHelper::GetParent(current);
        } catch (...) {
            return nullptr;
        }

        if (!current) {
            return nullptr;
        }

        auto panel = current.try_as<wuxc::Panel>();

        if (panel) {
            return panel;
        }

        auto frameworkElement = current.try_as<wux::FrameworkElement>();

        if (frameworkElement && frameworkElement == startButton) {
            break;
        }
    }

    return nullptr;
}

// -----------------------------------------------------------------------------
// Instance management
// -----------------------------------------------------------------------------

void DetachInstance(const std::shared_ptr<StartIconInstance>& instance);

bool IsInstanceAttachedToCurrentVisualTree(
    const std::shared_ptr<StartIconInstance>& instance) {
    if (!instance || instance->detached) {
        return false;
    }

    auto startButton = instance->startButton.get();
    auto stockIcon = instance->stockIcon.get();
    auto hostPanel = instance->hostPanel.get();
    auto imageHost = instance->customImageHost.get();

    if (!startButton || !stockIcon || !hostPanel || !imageHost) {
        return false;
    }

    try {
        // A control can keep the same object while ApplyTemplate replaces its
        // template-generated visual subtree. Verify both our injected host and
        // the stock icon still belong to this Start button before treating the
        // instance as attached.
        auto imageHostParent = wuxm::VisualTreeHelper::GetParent(imageHost);

        if (!imageHostParent || imageHostParent != hostPanel) {
            return false;
        }

        bool foundStartButton = false;
        wux::DependencyObject current = hostPanel;

        while (current) {
            if (current == startButton) {
                foundStartButton = true;
                break;
            }

            current = wuxm::VisualTreeHelper::GetParent(current);
        }

        if (!foundStartButton) {
            return false;
        }

        // The stock icon is normally below the host panel, not above it. Check
        // its ancestry separately to ensure it belongs to the same live
        // template subtree.
        bool foundHostPanel = false;
        current = stockIcon;

        while (current) {
            if (current == hostPanel) {
                foundHostPanel = true;
            }

            if (current == startButton) {
                return foundHostPanel;
            }

            current = wuxm::VisualTreeHelper::GetParent(current);
        }
    } catch (...) {
        // XAML access can fail while a template is being torn down. In that
        // case, retire the old instance and let the caller attach a fresh one.
    }

    return false;
}

void PruneDeadInstancesForCurrentThread() {
    DWORD threadId = GetCurrentThreadId();
    std::vector<std::shared_ptr<StartIconInstance>> threadInstances;
    std::vector<std::shared_ptr<StartIconInstance>> deadInstances;

    {
        std::lock_guard<std::mutex> lock(g_instancesMutex);

        if (!g_instances) {
            return;
        }

        std::erase(*g_instances, nullptr);

        for (const auto& instance : *g_instances) {
            if (instance->threadId == threadId) {
                threadInstances.push_back(instance);
            }
        }
    }

    // Inspect XAML only on the instance's owning UI thread and without holding
    // the global instance mutex.
    for (const auto& instance : threadInstances) {
        if (!IsInstanceAttachedToCurrentVisualTree(instance)) {
            deadInstances.push_back(instance);
        }
    }

    if (!deadInstances.empty()) {
        std::lock_guard<std::mutex> lock(g_instancesMutex);

        if (g_instances) {
            std::erase_if(*g_instances, [&](const auto& instance) {
                return std::find(deadInstances.begin(), deadInstances.end(),
                                 instance) != deadInstances.end();
            });
        }
    }

    for (const auto& instance : deadInstances) {
        Wh_Log(L"Start button template changed; replacing the stale custom "
               L"image host");
        DetachInstance(instance);
    }
}

bool AlreadyAttached(const wux::FrameworkElement& startButton) {
    PruneDeadInstancesForCurrentThread();

    std::lock_guard<std::mutex> lock(g_instancesMutex);

    if (!g_instances) {
        return false;
    }

    for (const auto& instance : *g_instances) {
        if (!instance || instance->threadId != GetCurrentThreadId()) {
            continue;
        }

        auto existingButton = instance->startButton.get();

        if (existingButton && existingButton == startButton) {
            return true;
        }
    }

    return false;
}

bool RegisterInstance(const std::shared_ptr<StartIconInstance>& instance) {
    std::lock_guard<std::mutex> lock(g_instancesMutex);

    if (g_unloading || !g_instances) {
        return false;
    }

    g_instances->push_back(instance);
    return true;
}

void UnregisterInstance(const std::shared_ptr<StartIconInstance>& instance) {
    std::lock_guard<std::mutex> lock(g_instancesMutex);

    if (!g_instances) {
        return;
    }

    g_instances->erase(
        std::remove(g_instances->begin(), g_instances->end(), instance),
        g_instances->end());
}

void DetachInstanceForStartButton(
    const wux::FrameworkElement& startButton) {
    std::vector<std::shared_ptr<StartIconInstance>> instances;

    {
        std::lock_guard<std::mutex> lock(g_instancesMutex);

        if (!g_instances) {
            return;
        }

        for (auto it = g_instances->begin(); it != g_instances->end();) {
            auto instance = *it;

            if (!instance || instance->threadId != GetCurrentThreadId()) {
                ++it;
                continue;
            }

            auto existingButton = instance->startButton.get();

            if (existingButton && existingButton == startButton) {
                instances.push_back(instance);
                it = g_instances->erase(it);
                continue;
            }

            ++it;
        }
    }

    for (const auto& instance : instances) {
        DetachInstance(instance);
    }
}

void RemoveCustomImage(const std::shared_ptr<StartIconInstance>& instance) {
    auto panel = instance->hostPanel.get();

    auto imageHost = instance->customImageHost.get();

    if (!panel || !imageHost) {
        return;
    }

    try {
        auto children = panel.Children();

        for (uint32_t i = 0; i < children.Size(); i++) {
            auto child = children.GetAt(i);

            if (child == imageHost) {
                children.RemoveAt(i);
                break;
            }
        }
    } catch (...) {
    }
}

void DetachInstance(const std::shared_ptr<StartIconInstance>& instance) {
    if (!instance || instance->detached) {
        return;
    }

    instance->detached = true;

    auto startButton = instance->startButton.get();

    if (startButton && instance->pressedStateCallbackRegistered) {
        try {
            auto buttonBase = startButton.try_as<wuxcp::ButtonBase>();

            if (buttonBase) {
                buttonBase.UnregisterPropertyChangedCallback(
                    wuxcp::ButtonBase::IsPressedProperty(),
                    instance->pressedStateCallbackToken);
            }
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"Removing pressed-state callback failed: 0x%08X %s",
                   e.code().value, e.message().c_str());
        } catch (...) {
        }

        instance->pressedStateCallbackToken = 0;
        instance->pressedStateCallbackRegistered = false;
    } else if (!startButton) {
        instance->pressedStateCallbackToken = 0;
        instance->pressedStateCallbackRegistered = false;
    }

    if (startButton && instance->activatedStateCallbackRegistered) {
        try {
            auto toggleButton = startButton.try_as<wuxcp::ToggleButton>();

            if (toggleButton) {
                toggleButton.UnregisterPropertyChangedCallback(
                    wuxcp::ToggleButton::IsCheckedProperty(),
                    instance->activatedStateCallbackToken);
            }
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"Removing activated-state callback failed: 0x%08X %s",
                   e.code().value, e.message().c_str());
        } catch (...) {
        }

        instance->activatedStateCallbackToken = 0;
        instance->activatedStateCallbackRegistered = false;
    } else if (!startButton) {
        instance->activatedStateCallbackToken = 0;
        instance->activatedStateCallbackRegistered = false;
    }

    if (startButton && instance->pointerEventsAttached) {
        auto removeHandler = [&](const wux::RoutedEvent& routedEvent,
                                 wf::IInspectable& handler,
                                 const wchar_t* eventName) {
            if (!handler) {
                return;
            }

            try {
                startButton.RemoveHandler(routedEvent, handler);
            } catch (const winrt::hresult_error& e) {
                Wh_Log(L"Removing %s handler failed: 0x%08X %s", eventName,
                       e.code().value, e.message().c_str());
            } catch (...) {
                Wh_Log(L"Removing %s handler failed", eventName);
            }

            handler = nullptr;
        };

        removeHandler(wux::UIElement::PointerEnteredEvent(),
                      instance->pointerEnteredHandler, L"PointerEntered");

        removeHandler(wux::UIElement::PointerExitedEvent(),
                      instance->pointerExitedHandler, L"PointerExited");

        removeHandler(wux::UIElement::PointerPressedEvent(),
                      instance->pointerPressedHandler, L"PointerPressed");

        removeHandler(wux::UIElement::PointerReleasedEvent(),
                      instance->pointerReleasedHandler, L"PointerReleased");

        removeHandler(wux::UIElement::PointerCanceledEvent(),
                      instance->pointerCanceledHandler, L"PointerCanceled");

        removeHandler(wux::UIElement::PointerCaptureLostEvent(),
                      instance->pointerCaptureLostHandler,
                      L"PointerCaptureLost");

        instance->pointerEventsAttached = false;
    } else if (!startButton) {
        instance->pointerEnteredHandler = nullptr;
        instance->pointerExitedHandler = nullptr;
        instance->pointerPressedHandler = nullptr;
        instance->pointerReleasedHandler = nullptr;
        instance->pointerCanceledHandler = nullptr;
        instance->pointerCaptureLostHandler = nullptr;
        instance->pointerEventsAttached = false;
    }

    try {
        instance->xamlRootChangedRevoker.revoke();
    } catch (...) {
    }

    for (size_t i = 0; i < kImageSlotCount; i++) {
        auto& resource = instance->imageResources[i];

        try {
            resource.bitmapOpenedRevoker.revoke();
            resource.bitmapFailedRevoker.revoke();
            resource.svgOpenedRevoker.revoke();
            resource.svgOpenFailedRevoker.revoke();
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"Removing %s image handlers failed: 0x%08X %s",
                   ImageSlotName(i), e.code().value, e.message().c_str());
        } catch (...) {
            Wh_Log(L"Removing %s image handlers failed", ImageSlotName(i));
        }

        try {
            if (resource.bitmap) {
                resource.bitmap.Stop();
            }
        } catch (...) {
        }

        try {
            if (resource.loadTarget) {
                resource.loadTarget.Source(wuxm::ImageSource{nullptr});
            }
        } catch (...) {
        }

        resource.loadTarget = nullptr;
        resource.source = nullptr;
        resource.bitmap = nullptr;
        resource.retiredBitmap = nullptr;
        resource.svg = nullptr;
        resource.opened = false;
        resource.failed = false;
    }

    instance->activeImageSet = false;
    instance->requestedStateSet = false;
    instance->displayedStateSet = false;
    instance->gifStatusLogged = false;

    auto imageHost = instance->customImageHost.get();

    if (imageHost) {
        try {
            auto visual =
                wuxh::ElementCompositionPreview::GetElementVisual(imageHost);

            if (visual) {
                visual.StopAnimation(L"Scale");

                visual.StopAnimation(L"RotationAngleInDegrees");

                visual.StopAnimation(L"Opacity");
            }
        } catch (...) {
        }
    }

    for (size_t layerIndex = 0; layerIndex < 2; layerIndex++) {
        auto layer = GetImageLayer(instance, layerIndex);

        if (!layer) {
            continue;
        }

        try {
            auto visual =
                wuxh::ElementCompositionPreview::GetElementVisual(layer);

            if (visual) {
                visual.StopAnimation(L"Opacity");
            }
        } catch (...) {
        }
    }

    RemoveCustomImage(instance);

    auto stockIcon = instance->stockIcon.get();

    if (stockIcon) {
        try {
            stockIcon.Opacity(instance->originalStockOpacity);
        } catch (...) {
        }
    }

    instance->startButton = {};
    instance->stockIcon = {};
    instance->hostPanel = {};
    instance->customImageHost = {};
    instance->customImage = {};
    instance->transitionImage = {};
}

// -----------------------------------------------------------------------------
// Theme change notifications
//
// Windows broadcasts WM_SETTINGCHANGE with "ImmersiveColorSet" to top-level
// windows when the Windows mode changes. While dark mode images are
// configured, the taskbar windows are subclassed to receive it on the taskbar
// UI thread, which also owns the Start button XAML.
// -----------------------------------------------------------------------------

LRESULT CALLBACK TaskbarThemeSubclassProc(HWND hWnd,
                                          UINT uMsg,
                                          WPARAM wParam,
                                          LPARAM lParam,
                                          DWORD_PTR refData);

std::mutex g_themeWindowsMutex;
std::vector<HWND> g_themeWindows;

bool IsTaskbarWindowClass(HWND window) {
    wchar_t className[64]{};

    if (!GetClassNameW(window, className, ARRAYSIZE(className))) {
        return false;
    }

    return _wcsicmp(className, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0;
}

// Must be called on a taskbar UI thread.
void SubclassTaskbarWindowsForThemeChanges() {
    std::vector<HWND> windows;

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND window, LPARAM param) -> BOOL {
            if (IsTaskbarWindowClass(window)) {
                reinterpret_cast<std::vector<HWND>*>(param)->push_back(window);
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&windows));

    for (HWND window : windows) {
        {
            std::lock_guard<std::mutex> lock(g_themeWindowsMutex);

            if (std::find(g_themeWindows.begin(), g_themeWindows.end(),
                          window) != g_themeWindows.end()) {
                continue;
            }
        }

        if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
                window, TaskbarThemeSubclassProc, 0)) {
            Wh_Log(L"Subclassing a taskbar window for theme changes failed");
            continue;
        }

        std::lock_guard<std::mutex> lock(g_themeWindowsMutex);
        g_themeWindows.push_back(window);
    }
}

// Removes the subclass from this thread's taskbar windows, or from every
// subclassed window if allThreads is true (used as a final unload pass).
void RemoveThemeSubclasses(bool allThreads) {
    DWORD threadId = GetCurrentThreadId();
    std::vector<HWND> windows;

    {
        std::lock_guard<std::mutex> lock(g_themeWindowsMutex);

        std::erase_if(g_themeWindows, [&](HWND window) {
            if (!allThreads &&
                GetWindowThreadProcessId(window, nullptr) != threadId) {
                return false;
            }

            windows.push_back(window);
            return true;
        });
    }

    for (HWND window : windows) {
        if (IsWindow(window)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                window, TaskbarThemeSubclassProc);
        }
    }
}

void DetachAllForCurrentThread() {
    RemoveThemeSubclasses(false);

    DWORD threadId = GetCurrentThreadId();
    std::vector<std::shared_ptr<StartIconInstance>> instances;

    {
        std::lock_guard<std::mutex> lock(g_instancesMutex);

        if (!g_instances) {
            return;
        }

        for (auto it = g_instances->begin(); it != g_instances->end();) {
            auto instance = *it;

            if (!instance) {
                it = g_instances->erase(it);
                continue;
            }

            if (instance->threadId == threadId) {
                instances.push_back(instance);
                it = g_instances->erase(it);
                continue;
            }

            ++it;
        }

        // During controlled unload, the last taskbar UI thread to remove its
        // instances also destroys the now-empty container. Instance objects
        // themselves remain in the local vector below until DetachInstance has
        // released all XAML references and callbacks on this owning thread.
        if (g_unloading && g_instances->empty()) {
            g_instances.reset();
        }
    }

    for (const auto& instance : instances) {
        DetachInstance(instance);
    }
}

void BeginInstanceShutdown() {
    std::lock_guard<std::mutex> lock(g_instancesMutex);

    g_unloading = true;

    if (g_instances && g_instances->empty()) {
        g_instances.reset();
    }
}

size_t GetTrackedInstanceCount() {
    std::lock_guard<std::mutex> lock(g_instancesMutex);

    return g_instances ? g_instances->size() : 0;
}

std::vector<DWORD> GetTrackedInstanceThreadIds() {
    std::lock_guard<std::mutex> lock(g_instancesMutex);
    std::vector<DWORD> threadIds;

    if (!g_instances) {
        return threadIds;
    }

    for (const auto& instance : *g_instances) {
        if (!instance || !instance->threadId ||
            std::find(threadIds.begin(), threadIds.end(),
                      instance->threadId) != threadIds.end()) {
            continue;
        }

        threadIds.push_back(instance->threadId);
    }

    return threadIds;
}

// -----------------------------------------------------------------------------
// Bitmap/image loading
// -----------------------------------------------------------------------------

void MaybeShowImageFailureMessageImpl(
    const std::shared_ptr<StartIconInstance>& instance) {
    if (!instance || instance->detached || g_unloading ||
        !instance->imageLoadingSetupComplete ||
        !instance->settings.showImageLoadFailureWarnings) {
        return;
    }

    const auto& normalResource = instance->imageResources
        [instance->imageResourceIndexBySlot[ImageSlotIndex(false,
                                                           IconState::Normal)]];

    // A failed main image decides the result for every state, so report only
    // that required setting without waiting for optional images.
    if (normalResource.reportableFailure) {
        ShowImageFailureMessage(instance->settingsGeneration,
                                {{IconState::Normal, false}});
        return;
    }

    // Optional failures only matter after the required main image succeeds.
    if (!ImageResourceAvailable(normalResource)) {
        return;
    }

    std::vector<ImageFailureDetail> failures;

    for (size_t slotIndex = 1; slotIndex < kImageSlotCount; slotIndex++) {
        bool darkSlot = IsDarkImageSlot(slotIndex);

        // Dark mode slots without an override share a main image, which is
        // reported under its own setting.
        if ((darkSlot && !instance->darkImageSetLoaded) ||
            !ImageSlotConfigured(instance->settings, slotIndex)) {
            continue;
        }

        const auto& resource = instance->imageResources
            [instance->imageResourceIndexBySlot[slotIndex]];

        // Wait for every configured optional resource to reach a terminal
        // state so one dialog can list all unavailable settings.
        if (!resource.opened && !resource.failed) {
            return;
        }

        if (resource.reportableFailure) {
            failures.push_back({ImageSlotState(slotIndex), darkSlot});
        }
    }

    if (!failures.empty()) {
        ShowImageFailureMessage(instance->settingsGeneration, failures);
    }
}

void MaybeShowImageFailureMessage(
    const std::shared_ptr<StartIconInstance>& instance) noexcept {
    try {
        MaybeShowImageFailureMessageImpl(instance);
    } catch (...) {
        // Never allow allocation or string-formatting failures to escape a
        // XAML ImageOpened/ImageFailed callback into Explorer.
        Wh_Log(L"Preparing the image failure summary failed");
    }
}

// Returns the extension of a path or URL in lower case, without the dot.
std::wstring GetSourceExtension(const std::wstring& source) {
    // Only URLs have query/fragment delimiters; '#' is valid in file names.
    size_t end = source.size();

    if (IsHttpUrl(source)) {
        end = source.find_first_of(L"?#");

        if (end == std::wstring::npos) {
            end = source.size();
        }
    }

    // Allow trailing whitespace in configured local paths.
    while (end > 0 && iswspace(source[end - 1])) {
        end--;
    }

    size_t nameStart = source.find_last_of(L"/\\", end ? end - 1 : 0);
    nameStart = nameStart == std::wstring::npos ? 0 : nameStart + 1;

    size_t dot = source.rfind(L'.', end ? end - 1 : 0);

    if (dot == std::wstring::npos || dot < nameStart || dot + 1 >= end) {
        return L"";
    }

    std::wstring extension = source.substr(dot + 1, end - dot - 1);

    for (wchar_t& c : extension) {
        c = towlower(c);
    }

    return extension;
}

bool IsSvgSource(const std::wstring& source) {
    return GetSourceExtension(source) == L"svg";
}

// Sources with one of these extensions aren't retried as SVG when they fail
// to load as a bitmap.
bool IsKnownBitmapSource(const std::wstring& source) {
    constexpr const wchar_t* kExtensions[] = {
        L"png", L"jpg", L"jpeg", L"jpe", L"jfif", L"gif",
        L"bmp", L"ico", L"tif",  L"tiff", L"webp", L"jxr",
    };

    std::wstring extension = GetSourceExtension(source);

    for (const wchar_t* known : kExtensions) {
        if (extension == known) {
            return true;
        }
    }

    return false;
}

const wchar_t* SvgLoadStatusName(wuxmi::SvgImageSourceLoadStatus status) {
    switch (status) {
        case wuxmi::SvgImageSourceLoadStatus::Success:
            return L"success";

        case wuxmi::SvgImageSourceLoadStatus::NetworkError:
            return L"network error";

        case wuxmi::SvgImageSourceLoadStatus::InvalidFormat:
            return L"invalid SVG format";

        default:
            return L"unknown error";
    }
}

// XAML rasterizes an SVG for the size of the Image which displays it. The
// hover and pressed effects then scale that bitmap, which would look soft, so
// rasterize for the largest size the icon is displayed at.
void SetSvgRasterizeSize(const std::shared_ptr<StartIconInstance>& instance,
                         const wuxmi::SvgImageSource& svg) {
    double scale = std::max({1.0, instance->settings.hoverScale,
                             instance->settings.pressedScale});

    try {
        // Account for the display scale of the monitor showing this taskbar.
        if (auto startButton = instance->startButton.get()) {
            if (auto xamlRoot = startButton.XamlRoot()) {
                scale *= std::max(1.0, xamlRoot.RasterizationScale());
            }
        }
    } catch (...) {
    }

    double pixels = std::ceil(instance->settings.iconSize * scale);

    svg.RasterizePixelWidth(pixels);
    svg.RasterizePixelHeight(pixels);
}

void OnXamlRootChanged(const std::weak_ptr<StartIconInstance>& weakInstance,
                       const wux::XamlRoot& xamlRoot) {
    auto instance = weakInstance.lock();

    if (!instance || instance->detached || g_unloading) {
        return;
    }

    double scale = 0.0;

    try {
        scale = xamlRoot.RasterizationScale();
    } catch (...) {
        return;
    }

    // XamlRoot.Changed also fires for size and visibility changes.
    if (scale == instance->rasterizationScale) {
        return;
    }

    instance->rasterizationScale = scale;

    for (size_t i = 0; i < kImageSlotCount; i++) {
        auto& resource = instance->imageResources[i];

        if (!resource.svg) {
            continue;
        }

        try {
            // Setting the raster size again makes XAML rasterize the already
            // loaded SVG for the new scale.
            SetSvgRasterizeSize(instance, resource.svg);
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"Rasterizing the %s SVG for scale %.2f failed: 0x%08X %s",
                   ImageSlotName(i), scale, e.code().value,
                   e.message().c_str());
        } catch (...) {
        }
    }
}

void TrackRasterizationScale(const std::shared_ptr<StartIconInstance>& instance,
                             const wux::FrameworkElement& startButton) {
    try {
        auto xamlRoot = startButton.XamlRoot();

        if (!xamlRoot) {
            return;
        }

        instance->rasterizationScale = xamlRoot.RasterizationScale();

        std::weak_ptr<StartIconInstance> weakInstance = instance;

        instance->xamlRootChangedRevoker = xamlRoot.Changed(
            winrt::auto_revoke,
            [weakInstance](const wux::XamlRoot& sender,
                           const wux::XamlRootChangedEventArgs&) {
                OnXamlRootChanged(weakInstance, sender);
            });
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Tracking the display scale failed: 0x%08X %s",
               e.code().value, e.message().c_str());
    } catch (...) {
    }
}

void OnImageOpened(const std::weak_ptr<StartIconInstance>& weakInstance,
                   size_t slotIndex) {
    auto instance = weakInstance.lock();

    if (!instance || instance->detached || g_unloading) {
        return;
    }

    auto& resource = instance->imageResources[slotIndex];

    resource.opened = true;
    resource.failed = false;
    resource.reportableFailure = false;

    Wh_Log(L"Custom Start %s image loaded", ImageSlotName(slotIndex));

    // Apply both the image and the current transform. The pointer might
    // already be hovering or pressed when decoding finishes.
    ApplyCurrentState(instance, 0);
    MaybeShowImageFailureMessage(instance);
}

void OnImageFailed(const std::weak_ptr<StartIconInstance>& weakInstance,
                   size_t slotIndex,
                   const wchar_t* error) {
    auto instance = weakInstance.lock();

    if (!instance || instance->detached || g_unloading) {
        return;
    }

    auto& resource = instance->imageResources[slotIndex];

    resource.opened = false;
    resource.failed = true;
    resource.reportableFailure = true;

    Wh_Log(L"Custom Start %s image failed: %s; source: %s",
           ImageSlotName(slotIndex), error, resource.configuredSource.c_str());

    if (slotIndex == ImageSlotIndex(false, IconState::Normal)) {
        Wh_Log(L"The main image is unavailable; keeping the stock Start icon "
               L"and ignoring optional state images");
    }

    UpdateDisplayedImage(instance);
    UpdateGifPlayback(instance);
    MaybeShowImageFailureMessage(instance);
}

bool RetryImageAsSvg(const std::weak_ptr<StartIconInstance>& weakInstance,
                     size_t slotIndex,
                     const wchar_t* bitmapError);

// Creates the XAML image source of a resource and starts loading
// resource.resolvedUri. Completion is reported by OnImageOpened or
// OnImageFailed. Can throw.
void BeginImageLoad(const std::shared_ptr<StartIconInstance>& instance,
                    size_t slotIndex,
                    bool asSvg) {
    auto& resource = instance->imageResources[slotIndex];

    std::weak_ptr<StartIconInstance> weakInstance = instance;

    resource.opened = false;
    resource.failed = false;
    resource.reportableFailure = false;

    if (asSvg) {
        wuxmi::SvgImageSource svg;

        SetSvgRasterizeSize(instance, svg);

        resource.svg = svg;
        resource.source = svg;

        resource.svgOpenedRevoker = svg.Opened(
            winrt::auto_revoke,
            [weakInstance, slotIndex](
                const wuxmi::SvgImageSource&,
                const wuxmi::SvgImageSourceOpenedEventArgs&) {
                OnImageOpened(weakInstance, slotIndex);
            });

        resource.svgOpenFailedRevoker = svg.OpenFailed(
            winrt::auto_revoke,
            [weakInstance, slotIndex](
                const wuxmi::SvgImageSource&,
                const wuxmi::SvgImageSourceFailedEventArgs& args) {
                OnImageFailed(weakInstance, slotIndex,
                              SvgLoadStatusName(args.Status()));
            });

        // UriSource lets XAML retrieve and decode the image asynchronously.
        svg.UriSource(wf::Uri(resource.resolvedUri));
        return;
    }

    wuxmi::BitmapImage bitmap;

    // Only the currently displayed GIF should run. Playback is updated after
    // every state change and when decoding completes.
    bitmap.AutoPlay(false);

    resource.bitmap = bitmap;
    resource.source = bitmap;

    resource.bitmapOpenedRevoker = bitmap.ImageOpened(
        winrt::auto_revoke,
        [weakInstance, slotIndex](const wf::IInspectable&,
                                  const wux::RoutedEventArgs&) {
            OnImageOpened(weakInstance, slotIndex);
        });

    resource.bitmapFailedRevoker = bitmap.ImageFailed(
        winrt::auto_revoke,
        [weakInstance, slotIndex](const wf::IInspectable&,
                                  const wux::ExceptionRoutedEventArgs& args) {
            auto error = args.ErrorMessage();

            if (RetryImageAsSvg(weakInstance, slotIndex, error.c_str())) {
                return;
            }

            OnImageFailed(weakInstance, slotIndex, error.c_str());
        });

    bitmap.UriSource(wf::Uri(resource.resolvedUri));
}

// An SVG which isn't recognizable by its extension, such as one served from a
// URL without a file name, fails to decode as a bitmap. Try it once as SVG.
// Returns false if the bitmap failure is final.
bool RetryImageAsSvg(const std::weak_ptr<StartIconInstance>& weakInstance,
                     size_t slotIndex,
                     const wchar_t* bitmapError) {
    auto instance = weakInstance.lock();

    if (!instance || instance->detached || g_unloading) {
        return false;
    }

    auto& resource = instance->imageResources[slotIndex];

    if (resource.svgFallbackTried || !resource.loadTarget ||
        IsKnownBitmapSource(resource.configuredSource)) {
        return false;
    }

    resource.svgFallbackTried = true;

    Wh_Log(L"Custom Start %s image failed as a bitmap: %s; retrying as SVG",
           ImageSlotName(slotIndex), bitmapError);

    try {
        // This runs inside the bitmap's ImageFailed handler, so keep the
        // bitmap and its handlers alive until detach.
        resource.retiredBitmap = resource.bitmap;
        resource.bitmap = nullptr;

        BeginImageLoad(instance, slotIndex, true);
        resource.loadTarget.Source(resource.source);

        return true;
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Retrying the %s image as SVG failed: 0x%08X %s",
               ImageSlotName(slotIndex), e.code().value, e.message().c_str());
    } catch (...) {
        Wh_Log(L"Retrying the %s image as SVG failed",
               ImageSlotName(slotIndex));
    }

    return false;
}

bool LoadImageResource(const std::shared_ptr<StartIconInstance>& instance,
                       size_t slotIndex,
                       const std::wstring& source) {
    if (!instance || source.empty()) {
        return false;
    }

    auto& resource = instance->imageResources[slotIndex];
    bool sourceUriConstructionInProgress = false;

    resource.configuredSource = source;
    resource.opened = false;
    resource.failed = false;
    resource.reportableFailure = false;

    try {
        // Preserve URLs exactly, including percent-encoded content. Local paths
        // need environment expansion followed by conversion to an escaped file
        // URI so characters such as '#' and '%' remain part of the filename.
        std::wstring expandedSource;

        if (IsHttpUrl(source)) {
            expandedSource = source;
            resource.resolvedUri = source;
        } else {
            expandedSource = ExpandPath(source);

            if (expandedSource.empty()) {
                resource.failed = true;
                resource.reportableFailure = true;
                Wh_Log(L"Custom Start %s image path couldn't be expanded: %s",
                       ImageSlotName(slotIndex), source.c_str());
                return false;
            }

            resource.resolvedUri = MakeFileUri(expandedSource);
        }

        if (resource.resolvedUri.empty()) {
            resource.failed = true;
            resource.reportableFailure = true;
            Wh_Log(L"Custom Start %s image URI couldn't be created: %s",
                   ImageSlotName(slotIndex), source.c_str());
            return false;
        }

        // Creating the URI fails for a malformed source, which is worth
        // reporting to the user.
        sourceUriConstructionInProgress = true;
        BeginImageLoad(instance, slotIndex, IsSvgSource(expandedSource));
        sourceUriConstructionInProgress = false;

        auto imageHost = instance->customImageHost.get();

        if (!imageHost) {
            resource.failed = true;
            return false;
        }

        wuxc::Image loadTarget;

        loadTarget.Opacity(0.0);
        loadTarget.IsHitTestVisible(false);

        wuxa::AutomationProperties::SetAccessibilityView(
            loadTarget, wuxap::AccessibilityView::Raw);

        resource.loadTarget = loadTarget;

        // Keep the target in the live tree for the lifetime of the resource.
        // It stays fully transparent; only customImage and transitionImage are
        // used to render the selected state. Assigning the source starts the
        // retrieval.
        imageHost.Children().Append(loadTarget);
        loadTarget.Source(resource.source);

        return true;
    } catch (const winrt::hresult_error& e) {
        resource.failed = true;

        Wh_Log(L"Loading %s image failed: 0x%08X %s; source: %s",
               ImageSlotName(slotIndex), e.code().value, e.message().c_str(),
               source.c_str());

        if (sourceUriConstructionInProgress) {
            resource.reportableFailure = true;
        }
    } catch (...) {
        resource.failed = true;

        Wh_Log(L"Loading %s image failed with unknown exception; source: %s",
               ImageSlotName(slotIndex), source.c_str());

        if (sourceUriConstructionInProgress) {
            resource.reportableFailure = true;
        }
    }

    return false;
}

bool LoadImages(const std::shared_ptr<StartIconInstance>& instance) {
    if (!instance) {
        return false;
    }

    instance->imageLoadingSetupComplete = false;

    std::array<std::wstring, kImageSlotCount> sources;

    for (size_t i = 0; i < kImageSlotCount; i++) {
        instance->imageResourceIndexBySlot[i] = i;

        // Without dark mode images, the dark mode slots stay empty.
        if (!IsDarkImageSlot(i) || instance->darkImageSetLoaded) {
            sources[i] = GetImageSourceForSlot(instance->settings, i);
        }
    }

    size_t normalSlot = ImageSlotIndex(false, IconState::Normal);

    if (!LoadImageResource(instance, normalSlot, sources[normalSlot])) {
        instance->imageLoadingSetupComplete = true;
        MaybeShowImageFailureMessage(instance);
        return false;
    }

    for (size_t slotIndex = 1; slotIndex < kImageSlotCount; slotIndex++) {
        if (sources[slotIndex].empty()) {
            continue;
        }

        bool reusedResource = false;

        for (size_t previousIndex = 0; previousIndex < slotIndex;
             previousIndex++) {
            if (!ImageSourcesEqual(sources[slotIndex],
                                   sources[previousIndex])) {
                continue;
            }

            instance->imageResourceIndexBySlot[slotIndex] =
                instance->imageResourceIndexBySlot[previousIndex];

            Wh_Log(L"The %s image reuses the %s image resource",
                   ImageSlotName(slotIndex), ImageSlotName(previousIndex));

            reusedResource = true;
            break;
        }

        if (reusedResource) {
            continue;
        }

        if (!LoadImageResource(instance, slotIndex, sources[slotIndex])) {
            Wh_Log(L"The %s image is unavailable; using its fallback",
                   ImageSlotName(slotIndex));
        }
    }

    instance->imageLoadingSetupComplete = true;
    MaybeShowImageFailureMessage(instance);

    return true;
}

// -----------------------------------------------------------------------------
// Interaction state setup
// -----------------------------------------------------------------------------

void AttachPointerEvents(const std::shared_ptr<StartIconInstance>& instance,
                         const wux::FrameworkElement& startButton) {
    std::weak_ptr<StartIconInstance> weakInstance = instance;

    // Mark cleanup as necessary before the first registration so a failure in
    // the middle of this function can still remove every handler added so far.
    instance->pointerEventsAttached = true;

    //
    // POINTER ENTERED
    //
    instance->pointerEnteredHandler = winrt::box_value(
        wuxi::PointerEventHandler([weakInstance](
                                      const wf::IInspectable&,
                                      const wuxi::PointerRoutedEventArgs&) {
            auto instance = weakInstance.lock();

            if (!instance) {
                return;
            }

            instance->hovering = true;

            ApplyCurrentState(instance, instance->pressed
                                            ? instance->settings.pressedDuration
                                            : instance->settings.hoverDuration);
        }));

    startButton.AddHandler(wux::UIElement::PointerEnteredEvent(),
                           instance->pointerEnteredHandler, true);

    //
    // POINTER EXITED
    //
    instance->pointerExitedHandler = winrt::box_value(wuxi::PointerEventHandler(
        [weakInstance](const wf::IInspectable&,
                       const wuxi::PointerRoutedEventArgs&) {
            auto instance = weakInstance.lock();

            if (!instance) {
                return;
            }

            instance->hovering = false;

            // PointerPressed and PointerReleased aren't guaranteed to
            // occur in pairs. ButtonBase tracks this itself; the raw
            // fallback must treat PointerExited as an end condition.
            if (!instance->pressedStateCallbackRegistered) {
                instance->pressed = false;
            }

            ApplyCurrentState(instance, instance->settings.releaseDuration);
        }));

    startButton.AddHandler(wux::UIElement::PointerExitedEvent(),
                           instance->pointerExitedHandler, true);

    // The Start control is a ButtonBase on supported Windows 11 builds.
    // Observe its own pressed state instead of reconstructing that state from
    // routed pointer events which the control class handles internally.
    try {
        auto buttonBase = startButton.try_as<wuxcp::ButtonBase>();

        if (buttonBase) {
            instance->pressedStateCallbackToken =
                buttonBase.RegisterPropertyChangedCallback(
                    wuxcp::ButtonBase::IsPressedProperty(),
                    wux::DependencyPropertyChangedCallback(
                        [weakInstance](const wux::DependencyObject& sender,
                                       const wux::DependencyProperty&) {
                            auto instance = weakInstance.lock();

                            auto buttonBase =
                                sender.try_as<wuxcp::ButtonBase>();

                            if (!instance || !buttonBase) {
                                return;
                            }

                            SetPressedState(instance, buttonBase.IsPressed());
                        }));

            instance->pressedStateCallbackRegistered = true;

            instance->pressed = buttonBase.IsPressed();

            Wh_Log(
                L"Start button pressed state is tracked "
                L"through ButtonBase::IsPressed");
        }
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Registering ButtonBase::IsPressed callback failed: 0x%08X %s",
               e.code().value, e.message().c_str());
    } catch (...) {
    }

    // Taskbar.ExperienceToggleButton derives from ToggleButton. Its checked
    // state follows whether the Start menu is currently shown.
    try {
        auto toggleButton = startButton.try_as<wuxcp::ToggleButton>();

        if (toggleButton) {
            instance->activatedStateCallbackToken =
                toggleButton.RegisterPropertyChangedCallback(
                    wuxcp::ToggleButton::IsCheckedProperty(),
                    wux::DependencyPropertyChangedCallback(
                        [weakInstance](const wux::DependencyObject& sender,
                                       const wux::DependencyProperty&) {
                            auto instance = weakInstance.lock();

                            auto toggleButton =
                                sender.try_as<wuxcp::ToggleButton>();

                            if (!instance || !toggleButton) {
                                return;
                            }

                            SetActivatedState(
                                instance, IsToggleButtonChecked(toggleButton));
                        }));

            instance->activatedStateCallbackRegistered = true;

            instance->activated = IsToggleButtonChecked(toggleButton);

            Wh_Log(
                L"Start menu visibility is tracked through "
                L"ToggleButton::IsChecked");
        }
    } catch (const winrt::hresult_error& e) {
        Wh_Log(
            L"Registering ToggleButton::IsChecked callback failed: 0x%08X %s",
            e.code().value, e.message().c_str());
    } catch (...) {
    }

    if (!instance->pressedStateCallbackRegistered) {
        Wh_Log(
            L"ButtonBase pressed-state callback is unavailable; "
            L"using routed-pointer fallback");

        //
        // POINTER PRESSED
        //
        instance->pointerPressedHandler =
            winrt::box_value(wuxi::PointerEventHandler(
                [weakInstance](const wf::IInspectable&,
                               const wuxi::PointerRoutedEventArgs&) {
                    auto instance = weakInstance.lock();

                    if (!instance) {
                        return;
                    }

                    SetPressedState(instance, true);

                    //
                    // IMPORTANT:
                    // Don't call args.Handled(true).
                    //
                    // Explorer still needs to receive
                    // the event normally.
                    //
                }));

        //
        // The third argument is critical.
        //
        // ButtonBase normally marks PointerPressed
        // as handled. true means our handler still
        // receives the routed event.
        //
        startButton.AddHandler(wux::UIElement::PointerPressedEvent(),
                               instance->pointerPressedHandler, true);

        //
        // POINTER RELEASED
        //
        instance->pointerReleasedHandler =
            winrt::box_value(wuxi::PointerEventHandler(
                [weakInstance](const wf::IInspectable&,
                               const wuxi::PointerRoutedEventArgs&) {
                    auto instance = weakInstance.lock();

                    if (!instance) {
                        return;
                    }

                    SetPressedState(instance, false);
                }));

        startButton.AddHandler(wux::UIElement::PointerReleasedEvent(),
                               instance->pointerReleasedHandler, true);

        //
        // POINTER CANCELED
        //
        instance->pointerCanceledHandler =
            winrt::box_value(wuxi::PointerEventHandler(
                [weakInstance](const wf::IInspectable&,
                               const wuxi::PointerRoutedEventArgs&) {
                    auto instance = weakInstance.lock();

                    if (!instance) {
                        return;
                    }

                    Wh_Log(L"Start button: CANCELED");

                    SetPressedState(instance, false);
                }));

        startButton.AddHandler(wux::UIElement::PointerCanceledEvent(),
                               instance->pointerCanceledHandler, true);

        //
        // POINTER CAPTURE LOST
        //
        instance->pointerCaptureLostHandler =
            winrt::box_value(wuxi::PointerEventHandler(
                [weakInstance](const wf::IInspectable&,
                               const wuxi::PointerRoutedEventArgs&) {
                    auto instance = weakInstance.lock();

                    if (!instance) {
                        return;
                    }

                    Wh_Log(L"Start button: CAPTURE LOST");

                    SetPressedState(instance, false);
                }));

        startButton.AddHandler(wux::UIElement::PointerCaptureLostEvent(),
                               instance->pointerCaptureLostHandler, true);
    }

    Wh_Log(L"Start button pointer handlers attached");
}

// -----------------------------------------------------------------------------
// Install replacement into a Start button
// -----------------------------------------------------------------------------

void ApplyThemeChange(const std::shared_ptr<StartIconInstance>& instance) {
    if (!instance || instance->detached || g_unloading ||
        !instance->darkImageSetLoaded) {
        return;
    }

    auto lightTheme = IsSystemLightTheme();

    // Keep the current images after a transient read failure.
    if (!lightTheme || *lightTheme == !instance->darkMode) {
        return;
    }

    instance->darkMode = !*lightTheme;

    Wh_Log(L"Windows mode changed to %s", *lightTheme ? L"light" : L"dark");

    // Both image sets are already loaded, so this only switches (and
    // crossfades) the displayed image.
    ApplyCurrentState(instance, 0);
}

void ApplyThemeChangeForCurrentThread() {
    DWORD threadId = GetCurrentThreadId();
    std::vector<std::shared_ptr<StartIconInstance>> instances;

    {
        std::lock_guard<std::mutex> lock(g_instancesMutex);

        if (!g_instances) {
            return;
        }

        for (const auto& instance : *g_instances) {
            if (instance && instance->threadId == threadId) {
                instances.push_back(instance);
            }
        }
    }

    for (const auto& instance : instances) {
        try {
            ApplyThemeChange(instance);
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"Applying dark mode images failed: 0x%08X %s",
                   e.code().value, e.message().c_str());
        } catch (...) {
            Wh_Log(L"Applying dark mode images failed");
        }
    }
}

LRESULT CALLBACK TaskbarThemeSubclassProc(HWND hWnd,
                                          UINT uMsg,
                                          WPARAM wParam,
                                          LPARAM lParam,
                                          DWORD_PTR refData) {
    if (uMsg == WM_SETTINGCHANGE && lParam && !g_unloading &&
        wcscmp(reinterpret_cast<PCWSTR>(lParam), L"ImmersiveColorSet") == 0) {
        // Every taskbar window on this thread receives the broadcast. The
        // first one applies the change; the rest find nothing to do.
        ApplyThemeChangeForCurrentThread();
    } else if (uMsg == WM_NCDESTROY) {
        // The subclass is removed automatically when the window is destroyed.
        std::lock_guard<std::mutex> lock(g_themeWindowsMutex);
        std::erase(g_themeWindows, hWnd);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

bool AttachToStartButton(const wux::FrameworkElement& startButton) {
    if (!startButton || g_unloading) {
        return false;
    }

    uint64_t settingsGeneration = 0;
    ModSettings settings = GetSettingsSnapshot(&settingsGeneration);

    if (IsBlank(settings.imageSources[IconStateIndex(IconState::Normal)])) {
        // An earlier build or an interrupted hot reload might have left the
        // stock element transparent. Remove any tracked replacement first,
        // then explicitly make the vanilla icon visible.
        DetachInstanceForStartButton(startButton);

        auto stockIcon = FindStockStartIcon(startButton);

        if (!stockIcon) {
            Wh_Log(L"Image source is empty, but the stock Start icon wasn't "
                   L"found");
            return false;
        }

        try {
            stockIcon.Opacity(1.0);
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"Restoring the stock Start icon failed: 0x%08X %s",
                   e.code().value, e.message().c_str());
            return false;
        }

        Wh_Log(L"Image source is empty; using the stock Start icon");

        return true;
    }

    if (AlreadyAttached(startButton)) {
        return true;
    }

    auto stockIcon = FindStockStartIcon(startButton);

    if (!stockIcon) {
        Wh_Log(L"Couldn't find Start AnimatedVisualPlayer#Icon");

        return false;
    }

    auto hostPanel = FindHostPanel(stockIcon, startButton);

    if (!hostPanel) {
        Wh_Log(L"Couldn't find a panel which can host the custom Start image");

        return false;
    }

    std::shared_ptr<StartIconInstance> instance;

    try {
        double originalStockOpacity = stockIcon.Opacity();

        instance = std::make_shared<StartIconInstance>();

        instance->threadId = GetCurrentThreadId();

        instance->settings = std::move(settings);
        instance->settingsGeneration = settingsGeneration;

        instance->startButton = winrt::make_weak(startButton);

        instance->stockIcon = winrt::make_weak(stockIcon);

        instance->hostPanel = winrt::make_weak(hostPanel);

        instance->originalStockOpacity = originalStockOpacity;

        // Register before adding XAML elements or callbacks. If unload starts
        // concurrently, its synchronous taskbar-thread callback runs after
        // this UI-thread work and sees everything that needs to be detached.
        if (!RegisterInstance(instance)) {
            return false;
        }

        wuxc::Grid imageHost;

        imageHost.Width(instance->settings.iconSize);

        imageHost.Height(instance->settings.iconSize);

        imageHost.HorizontalAlignment(wux::HorizontalAlignment::Center);

        imageHost.VerticalAlignment(wux::VerticalAlignment::Center);

        imageHost.IsHitTestVisible(false);

        wuxa::AutomationProperties::SetAccessibilityView(
            imageHost, wuxap::AccessibilityView::Raw);

        wuxc::Image customImage;
        wuxc::Image transitionImage;

        auto configureImageLayer = [](const wuxc::Image& image) {
            image.HorizontalAlignment(wux::HorizontalAlignment::Stretch);

            image.VerticalAlignment(wux::VerticalAlignment::Stretch);

            image.Stretch(wuxm::Stretch::Uniform);

            image.IsHitTestVisible(false);

            wuxa::AutomationProperties::SetAccessibilityView(
                image, wuxap::AccessibilityView::Raw);
        };

        configureImageLayer(customImage);
        configureImageLayer(transitionImage);

        imageHost.Children().Append(customImage);

        imageHost.Children().Append(transitionImage);

        // Put the image above the stock
        // AnimatedVisualPlayer.
        wuxc::Canvas::SetZIndex(imageHost, 10000);

        instance->customImageHost = winrt::make_weak(imageHost);

        instance->customImage = winrt::make_weak(customImage);

        instance->transitionImage = winrt::make_weak(transitionImage);

        hostPanel.Children().Append(imageHost);

        AttachPointerEvents(instance, startButton);
        TrackRasterizationScale(instance, startButton);

        instance->darkImageSetLoaded = HasDarkModeImages(instance->settings);

        if (instance->darkImageSetLoaded) {
            auto lightTheme = IsSystemLightTheme();

            if (!lightTheme) {
                Wh_Log(L"Windows mode unavailable; using the XAML theme");

                lightTheme =
                    startButton.ActualTheme() == wux::ElementTheme::Light;
            }

            instance->darkMode = !*lightTheme;

            SubclassTaskbarWindowsForThemeChanges();
        }

        if (!LoadImages(instance)) {
            UnregisterInstance(instance);
            DetachInstance(instance);

            Wh_Log(L"Failed to load custom image; keeping stock icon");

            return false;
        }

        // UpdateDisplayedImage keeps the stock icon visible until ImageOpened
        // confirms that the normal replacement decoded successfully.
        ApplyCurrentState(instance, 0);

        Wh_Log(L"Custom Start icon attached");

        return true;
    } catch (const winrt::hresult_error& e) {
        UnregisterInstance(instance);
        DetachInstance(instance);

        Wh_Log(L"AttachToStartButton failed: 0x%08X %s", e.code().value,
               e.message().c_str());

        return false;
    } catch (...) {
        UnregisterInstance(instance);
        DetachInstance(instance);

        Wh_Log(L"AttachToStartButton failed with an unknown exception");

        return false;
    }
}

// -----------------------------------------------------------------------------
// Apply to XamlRoot
// -----------------------------------------------------------------------------

bool ApplyToXamlRoot(const wux::XamlRoot& xamlRoot) {
    if (!xamlRoot) {
        return false;
    }

    try {
        auto content = xamlRoot.Content().try_as<wux::FrameworkElement>();

        if (!content) {
            Wh_Log(L"XamlRoot content isn't a FrameworkElement");

            return false;
        }

        auto startButton = FindStartButton(content);

        if (!startButton) {
            Wh_Log(L"Couldn't find AutomationId=StartButton");

            return false;
        }

        return AttachToStartButton(startButton);
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"ApplyToXamlRoot failed: 0x%08X %s", e.code().value,
               e.message().c_str());

        return false;
    } catch (...) {
        Wh_Log(L"ApplyToXamlRoot failed with an unknown exception");

        return false;
    }
}

// -----------------------------------------------------------------------------
// taskbar.dll internals
//
// This is the same general TaskbarHost/XamlRoot approach used by current
// Windhawk Windows 11 taskbar mods.
// -----------------------------------------------------------------------------

void* CTaskBand_ITaskListWndSite_vftable;

void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);

CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void** result);

CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using RefCountDecref_t = void(WINAPI*)(void* pThis);

RefCountDecref_t RefCountDecref_Original;

// -----------------------------------------------------------------------------
// Extract XamlRoot from TaskbarHost
// -----------------------------------------------------------------------------

wux::XamlRoot XamlRootFromTaskbarHostSharedPtr(void* sharedPtr[2]) {
    struct SharedPtrReleaseGuard {
        void** sharedPtr;

        ~SharedPtrReleaseGuard() noexcept {
            if (sharedPtr[1] && RefCountDecref_Original) {
                RefCountDecref_Original(sharedPtr[1]);
                sharedPtr[1] = nullptr;
            }
        }
    } releaseGuard{sharedPtr};

    if (!sharedPtr[0]) {
        return nullptr;
    }

    size_t taskbarElementOffset = 0x10;

#if defined(_M_X64)

    // Current taskbar.dll builds generally make
    // TaskbarHost::FrameHeight access a member
    // at the same offset as the taskbar XAML
    // element reference. Derive that offset from
    // the function rather than hard-coding it
    // whenever possible.
    const BYTE* code =
        reinterpret_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);

    if (code && code[0] == 0x48 && code[1] == 0x83 && code[2] == 0xEC &&
        code[4] == 0x48 && code[5] == 0x83 && code[6] == 0xC1 &&
        code[7] <= 0x7F) {
        taskbarElementOffset = code[7];
    } else {
        Wh_Log(
            L"Unrecognized TaskbarHost::FrameHeight; refusing an unsafe "
            L"TaskbarHost layout fallback");

        return nullptr;
    }

#elif defined(_M_ARM64)

    // 7f2303d5 pacibsp
    // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
    // fd030091 mov     fp, sp
    // 080c41f8 ldr     x8, [x0, #0x10]!
    const DWORD* code =
        reinterpret_cast<const DWORD*>(TaskbarHost_FrameHeight_Original);

    if (code && code[0] == 0xD503237F &&
        (code[1] & 0xFFC07FFF) == 0xA9807BFD &&
        code[2] == 0x910003FD &&
        (code[3] & 0xFFF00FE0) == 0xF8400C00) {
        taskbarElementOffset = (code[3] >> 12) & 0xFF;
    } else {
        Wh_Log(
            L"Unrecognized TaskbarHost::FrameHeight; refusing an unsafe "
            L"TaskbarHost layout fallback");

        return nullptr;
    }

#else
#error Unsupported architecture.
#endif

    auto* taskbarElementIUnknown = *reinterpret_cast<IUnknown**>(
        reinterpret_cast<BYTE*>(sharedPtr[0]) + taskbarElementOffset);

    wux::FrameworkElement taskbarElement{nullptr};

    if (taskbarElementIUnknown) {
        taskbarElementIUnknown->QueryInterface(
            winrt::guid_of<wux::FrameworkElement>(),
            winrt::put_abi(taskbarElement));
    }

    wux::XamlRoot result{nullptr};

    if (taskbarElement) {
        result = taskbarElement.XamlRoot();
    }

    return result;
}

// -----------------------------------------------------------------------------
// Primary taskbar XamlRoot
// -----------------------------------------------------------------------------

wux::XamlRoot GetTaskbarXamlRoot(HWND taskbarWnd) {
    HWND taskSwitchWnd =
        reinterpret_cast<HWND>(GetProp(taskbarWnd, L"TaskbandHWND"));

    if (!taskSwitchWnd) {
        return nullptr;
    }

    void* taskBand =
        reinterpret_cast<void*>(GetWindowLongPtr(taskSwitchWnd, 0));

    if (!taskBand) {
        return nullptr;
    }

    void* interfacePointer = taskBand;

    for (int i = 0; i < 20; i++) {
        if (*reinterpret_cast<void**>(interfacePointer) ==
            CTaskBand_ITaskListWndSite_vftable) {
            break;
        }

        interfacePointer = reinterpret_cast<void**>(interfacePointer) + 1;

        if (i == 19) {
            return nullptr;
        }
    }

    void* sharedPtr[2]{};

    CTaskBand_GetTaskbarHost_Original(interfacePointer, sharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(sharedPtr);
}

// -----------------------------------------------------------------------------
// Secondary taskbar XamlRoot
// -----------------------------------------------------------------------------

wux::XamlRoot GetSecondaryTaskbarXamlRoot(HWND secondaryTaskbarWnd) {
    HWND taskSwitchWnd =
        FindWindowEx(secondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);

    if (!taskSwitchWnd) {
        return nullptr;
    }

    void* taskBand =
        reinterpret_cast<void*>(GetWindowLongPtr(taskSwitchWnd, 0));

    if (!taskBand) {
        return nullptr;
    }

    void* interfacePointer = taskBand;

    for (int i = 0; i < 20; i++) {
        if (*reinterpret_cast<void**>(interfacePointer) ==
            CSecondaryTaskBand_ITaskListWndSite_vftable) {
            break;
        }

        interfacePointer = reinterpret_cast<void**>(interfacePointer) + 1;

        if (i == 19) {
            return nullptr;
        }
    }

    void* sharedPtr[2]{};

    CSecondaryTaskBand_GetTaskbarHost_Original(interfacePointer, sharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(sharedPtr);
}

// -----------------------------------------------------------------------------
// Run code on taskbar UI thread
// -----------------------------------------------------------------------------

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND window,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT registeredMessage =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RunParam {
        RunFromWindowThreadProc_t proc;
        void* param;
        std::atomic<bool> invoked{false};
        std::atomic<bool> succeeded{false};
    };

    DWORD threadId = GetWindowThreadProcessId(window, nullptr);

    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        try {
            proc(procParam);
            return true;
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"Taskbar-thread callback failed: 0x%08X %s",
                   e.code().value, e.message().c_str());
        } catch (...) {
            Wh_Log(L"Taskbar-thread callback failed with an unknown exception");
        }

        return false;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                const CWPSTRUCT* cwp =
                    reinterpret_cast<const CWPSTRUCT*>(lParam);

                if (cwp->message == registeredMessage) {
                    auto* parameter = reinterpret_cast<RunParam*>(cwp->lParam);

                    try {
                        parameter->proc(parameter->param);
                        parameter->succeeded.store(true);
                    } catch (const winrt::hresult_error& e) {
                        Wh_Log(L"Taskbar-thread callback failed: 0x%08X %s",
                               e.code().value, e.message().c_str());
                    } catch (...) {
                        Wh_Log(L"Taskbar-thread callback failed with an "
                               L"unknown exception");
                    }

                    parameter->invoked.store(true);
                }
            }

            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);

    if (!hook) {
        return false;
    }

    RunParam parameter{proc, procParam};

    SendMessage(window, registeredMessage, 0,
                reinterpret_cast<LPARAM>(&parameter));

    UnhookWindowsHookEx(hook);

    return parameter.invoked.load() && parameter.succeeded.load();
}

// -----------------------------------------------------------------------------
// Taskbar discovery
// -----------------------------------------------------------------------------

std::vector<HWND> FindCurrentProcessTaskbarWindows() {
    std::vector<HWND> result;

    EnumWindows(
        [](HWND window, LPARAM param) -> BOOL {
            DWORD processId = 0;

            GetWindowThreadProcessId(window, &processId);

            if (processId != GetCurrentProcessId()) {
                return TRUE;
            }

            wchar_t className[64]{};

            if (!GetClassNameW(window, className, ARRAYSIZE(className))) {
                return TRUE;
            }

            if (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
                _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
                reinterpret_cast<std::vector<HWND>*>(param)->push_back(window);
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));

    return result;
}

bool RunOnAllTaskbarThreads(RunFromWindowThreadProc_t proc, void* procParam) {
    auto taskbarWindows = FindCurrentProcessTaskbarWindows();

    std::vector<DWORD> processedThreadIds;
    bool allSucceeded = true;

    for (HWND taskbarWindow : taskbarWindows) {
        DWORD threadId = GetWindowThreadProcessId(taskbarWindow, nullptr);

        if (!threadId ||
            std::find(processedThreadIds.begin(), processedThreadIds.end(),
                      threadId) != processedThreadIds.end()) {
            continue;
        }

        processedThreadIds.push_back(threadId);

        if (!RunFromWindowThread(taskbarWindow, proc, procParam)) {
            Wh_Log(L"Running code on taskbar thread %lu failed", threadId);
            allSucceeded = false;
        }
    }

    if (processedThreadIds.empty()) {
        Wh_Log(L"No taskbar windows were found in explorer.exe");
        return false;
    }

    return allSucceeded;
}

std::vector<HWND> FindWindowsOnThread(DWORD threadId) {
    std::vector<HWND> result;

    EnumThreadWindows(
        threadId,
        [](HWND window, LPARAM param) -> BOOL {
            try {
                reinterpret_cast<std::vector<HWND>*>(param)->push_back(window);
                return TRUE;
            } catch (...) {
                return FALSE;
            }
        },
        reinterpret_cast<LPARAM>(&result));

    return result;
}

bool RunOnTrackedInstanceThreads(RunFromWindowThreadProc_t proc,
                                 void* procParam) {
    auto threadIds = GetTrackedInstanceThreadIds();
    bool allSucceeded = true;

    for (DWORD threadId : threadIds) {
        auto windows = FindWindowsOnThread(threadId);

        if (windows.empty()) {
            Wh_Log(L"No windows were found on tracked taskbar thread %lu",
                   threadId);
            allSucceeded = false;
            continue;
        }

        bool succeeded = false;

        for (HWND window : windows) {
            if (RunFromWindowThread(window, proc, procParam)) {
                succeeded = true;
                break;
            }
        }

        if (!succeeded) {
            Wh_Log(L"Running code on tracked taskbar thread %lu failed",
                   threadId);
            allSucceeded = false;
        }
    }

    return allSucceeded;
}

// -----------------------------------------------------------------------------
// Apply/remove on taskbar thread
// -----------------------------------------------------------------------------

void ApplyFromTaskbarThread(bool mainTaskbarOnly = false) {
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND window, LPARAM param) -> BOOL {
            try {
                bool mainOnly = param != 0;

                wchar_t className[64]{};

                if (!GetClassNameW(window, className, ARRAYSIZE(className))) {
                    return TRUE;
                }

                wux::XamlRoot xamlRoot{nullptr};

                if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
                    xamlRoot = GetTaskbarXamlRoot(window);
                } else if (!mainOnly &&
                           _wcsicmp(className,
                                    L"Shell_SecondaryTrayWnd") == 0) {
                    xamlRoot = GetSecondaryTaskbarXamlRoot(window);
                } else {
                    return TRUE;
                }

                if (!xamlRoot) {
                    Wh_Log(L"Couldn't obtain taskbar XamlRoot");

                    return TRUE;
                }

                ApplyToXamlRoot(xamlRoot);
            } catch (const winrt::hresult_error& e) {
                Wh_Log(L"Applying a taskbar window failed: 0x%08X %s",
                       e.code().value, e.message().c_str());
            } catch (...) {
                Wh_Log(L"Applying a taskbar window failed with an unknown "
                       L"exception");
            }

            return TRUE;
        },
        static_cast<LPARAM>(mainTaskbarOnly));
}

void RebuildFromTaskbarThread() {
    DetachAllForCurrentThread();

    if (!g_unloading) {
        ApplyFromTaskbarThread();
    }
}

void WINAPI ApplyAllTaskbarsProc(void*) {
    ApplyFromTaskbarThread();
}

void WINAPI RebuildAllTaskbarsProc(void*) {
    RebuildFromTaskbarThread();
}

void WINAPI DetachAllTaskbarsProc(void*) {
    DetachAllForCurrentThread();
}

// -----------------------------------------------------------------------------
// Taskbar creation hooks
// -----------------------------------------------------------------------------

using TrayUI_StartTaskbar_t = void(WINAPI*)(void* pThis);

TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original;

void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    TrayUI_StartTaskbar_Original(pThis);

    if (!g_unloading) {
        ApplyFromTaskbarThread(true);
    }
}

using CSecondaryTray_GetTrayWindow_t = HWND(WINAPI*)(void* pThis);

CSecondaryTray_GetTrayWindow_t CSecondaryTray_GetTrayWindow_Original;

using CSecondaryTray_InitModelAndHost_t = void(WINAPI*)(void* pThis,
                                                        void* taskbarModel);

CSecondaryTray_InitModelAndHost_t CSecondaryTray_InitModelAndHost_Original;

void WINAPI CSecondaryTray_InitModelAndHost_Hook(void* pThis,
                                                 void* taskbarModel) {
    CSecondaryTray_InitModelAndHost_Original(pThis, taskbarModel);

    if (g_unloading) {
        return;
    }

    try {
        HWND taskbarWnd = CSecondaryTray_GetTrayWindow_Original(pThis);

        if (!taskbarWnd) {
            return;
        }

        auto xamlRoot = GetSecondaryTaskbarXamlRoot(taskbarWnd);

        if (xamlRoot) {
            ApplyToXamlRoot(xamlRoot);
        }
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Handling a secondary taskbar failed: 0x%08X %s",
               e.code().value, e.message().c_str());
    } catch (...) {
        Wh_Log(L"Handling a secondary taskbar failed with an unknown "
               L"exception");
    }
}

// -----------------------------------------------------------------------------
// Start button recreation hook
// -----------------------------------------------------------------------------

using ExperienceToggleButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);

ExperienceToggleButton_UpdateButtonPadding_t
    ExperienceToggleButton_UpdateButtonPadding_Original;

void WINAPI ExperienceToggleButton_UpdateButtonPadding_Hook(void* pThis) {
    ExperienceToggleButton_UpdateButtonPadding_Original(pThis);

    if (g_unloading || !pThis) {
        return;
    }

    try {
        IUnknown* inspectable = reinterpret_cast<IUnknown**>(pThis)[1];

        if (!inspectable) {
            return;
        }

        wux::FrameworkElement element{nullptr};

        inspectable->QueryInterface(winrt::guid_of<wux::FrameworkElement>(),
                                    winrt::put_abi(element));

        if (!element ||
            winrt::get_class_name(element) !=
                L"Taskbar.ExperienceToggleButton" ||
            wuxa::AutomationProperties::GetAutomationId(element) !=
                L"StartButton") {
            return;
        }

        AttachToStartButton(element);
    } catch (const winrt::hresult_error& e) {
        Wh_Log(L"Handling a recreated Start button failed: 0x%08X %s",
               e.code().value, e.message().c_str());
    } catch (...) {
        Wh_Log(L"Handling a recreated Start button failed");
    }
}

std::atomic<bool> g_taskbarViewDllLoaded = false;

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateButtonPadding(void))"},
            &ExperienceToggleButton_UpdateButtonPadding_Original,
            ExperienceToggleButton_UpdateButtonPadding_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"Failed to resolve taskbar view module symbols");
        return false;
    }

    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");

    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedTaskbarViewModule(HMODULE module) {
    if (g_unloading || g_taskbarViewDllLoaded.load() || !module ||
        GetTaskbarViewModuleHandle() != module) {
        return;
    }

    bool expected = false;

    if (!g_taskbarViewDllLoaded.compare_exchange_strong(expected, true)) {
        return;
    }

    Wh_Log(L"Taskbar view module loaded");

    if (!HookTaskbarViewDllSymbols(module)) {
        return;
    }

    Wh_ApplyHookOperations();
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);

LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);

    HandleLoadedTaskbarViewModule(module);

    return module;
}

// -----------------------------------------------------------------------------
// Symbol hooks
// -----------------------------------------------------------------------------

bool HookTaskbarDllSymbols() {
    HMODULE taskbarDll =
        LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (!taskbarDll) {
        Wh_Log(L"Failed to load taskbar.dll");

        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {

        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },

        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
        },

        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },

        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },

        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
        },

        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &RefCountDecref_Original,
        },

        {
            {LR"(public: virtual void __cdecl TrayUI::StartTaskbar(void))"},
            &TrayUI_StartTaskbar_Original,
            TrayUI_StartTaskbar_Hook,
        },

        {
            {LR"(public: virtual struct HWND__ * __cdecl CSecondaryTray::GetTrayWindow(void))"},
            &CSecondaryTray_GetTrayWindow_Original,
        },

        {
            {LR"(public: virtual void __cdecl CSecondaryTray::InitModelAndHost(struct winrt::WindowsUdk::UI::Shell::TaskbarModel))"},
            &CSecondaryTray_InitModelAndHost_Original,
            CSecondaryTray_InitModelAndHost_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(taskbarDll, taskbarDllHooks,
                                    ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"Failed to resolve taskbar.dll symbols");

        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------
// Windhawk lifecycle
// -----------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Start Button Replacer initializing");

    g_unloading = false;

    uint64_t settingsGeneration = StoreSettings(LoadSettings());
    ResetReportedImageFailures(settingsGeneration);

    if (IsBlank(GetSettingsSnapshot()
                    .imageSources[IconStateIndex(IconState::Normal)])) {
        Wh_Log(L"No image source is configured; leaving the stock Start icon "
               L"unchanged");

        return FALSE;
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;

        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");

        if (!kernelBaseModule) {
            Wh_Log(L"Failed to get kernelbase.dll");
            return FALSE;
        }

        auto loadLibraryExW = reinterpret_cast<LoadLibraryExW_t>(
            GetProcAddress(kernelBaseModule, "LoadLibraryExW"));

        if (!loadLibraryExW) {
            Wh_Log(L"Failed to find LoadLibraryExW");
            return FALSE;
        }

        if (!WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                            &LoadLibraryExW_Original)) {
            Wh_Log(L"Hooking LoadLibraryExW failed");
            return FALSE;
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"Start Button Replacer applying");

    if (!g_taskbarViewDllLoaded) {
        HandleLoadedTaskbarViewModule(GetTaskbarViewModuleHandle());
    }

    RunOnAllTaskbarThreads(ApplyAllTaskbarsProc, nullptr);
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"Start Button Replacer restoring stock icon");

    BeginInstanceShutdown();

    // The dialog owns a return address in this module. Close it and join its
    // worker before Windhawk can unload the module.
    StopImageFailureMessageThread();

    // Drive cleanup from the instances themselves instead of rediscovering
    // taskbar window classes. RunFromWindowThread is synchronous, so every
    // callback and XAML reference is gone before controlled unload continues.
    RunOnTrackedInstanceThreads(DetachAllTaskbarsProc, nullptr);

    size_t remainingInstances = GetTrackedInstanceCount();

    if (remainingInstances != 0) {
        Wh_Log(L"%zu Start icon instance(s) remained after owner-thread "
               L"cleanup; trying current taskbar windows",
               remainingInstances);

        RunOnAllTaskbarThreads(DetachAllTaskbarsProc, nullptr);
        remainingInstances = GetTrackedInstanceCount();
    }

    // A taskbar window can be recreated while either pass is enumerating.
    // Retry the recorded owner threads once before allowing unload to continue.
    if (remainingInstances != 0) {
        Wh_Log(L"%zu Start icon instance(s) remained after taskbar-window "
               L"cleanup; retrying owner threads",
               remainingInstances);

        RunOnTrackedInstanceThreads(DetachAllTaskbarsProc, nullptr);
        remainingInstances = GetTrackedInstanceCount();
    }

    if (remainingInstances != 0) {
        Wh_Log(L"Critical: %zu Start icon instance(s) couldn't be detached",
               remainingInstances);
    }

    // Normally removed by the owner-thread passes above. Remove any left over,
    // for example on a thread whose instances were already gone.
    RemoveThemeSubclasses(true);
}

void Wh_ModUninit() {
    Wh_Log(L"Start Button Replacer unloaded");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Start Button Replacer settings changed");

    uint64_t settingsGeneration = StoreSettings(LoadSettings());
    ResetReportedImageFailures(settingsGeneration);

    RunOnAllTaskbarThreads(RebuildAllTaskbarsProc, nullptr);
}

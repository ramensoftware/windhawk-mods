// ==WindhawkMod==
// @id              start-button-replacer
// @name            Start Button Replacer
// @description     Replace the Windows 11 Start button icon with a custom PNG, JPG or GIF image
// @version         0.12.1
// @author          Ender
// @github          https://github.com/EnderDragonEP
// @twitter         https://twitter.com/NoobieNoodle89
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore
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
//   * Supports PNG/JPG/GIF.
//   * Supports local absolute paths and paths with environment variables.
//   * Supports animated GIF playback.
//   * Supports crossfading normal, hover and pressed images.
//   * Supports hover/press scale, rotation and opacity effects.

// ==WindhawkModReadme==
/*
# Start Button Replacer

Replaces the icon inside the Windows 11 Start button with a custom PNG, JPG or
GIF image. Animated GIF playback and hover/pressed effects are optional
features.

![preview](https://raw.githubusercontent.com/EnderDragonEP/asset/main/windhawk-mods/start-button-replacer/preview-1.gif)

Only the icon is replaced. Windows continues to handle clicking, keyboard
navigation, accessibility and Start menu activation.

## Image sources

Local files:

    C:\Users\YourName\Pictures\start.png

Environment variables are supported:

    %USERPROFILE%\Pictures\start.jpg

## Animated GIF playback

- Always
- Hover
- Pressed
- Stopped

## Effects

Separate scale, rotation and opacity settings are available for hover and
pressed states.

## Separate state images

The main **Image source** is always the normal image. Add a hover, pressed or
activated image source to enable that state automatically. The activated image
is shown while the Start menu is open. If it is blank or unavailable, activated
falls back to the pressed image, then the hover image, then the main image.
Other blank or unavailable state images fall back to the main image.

While the Start menu is shown, pressing the button temporarily uses the pressed
state. The activated state resumes on release and otherwise takes priority over
the hover state.

State images crossfade whenever their applicable duration is greater than zero.
Hover transitions use the hover duration; pressed and activated transitions use
the pressed duration. Set a duration to `0` to switch instantly.

## Notes

For crisp results, use a square image with transparency. Keep animated GIFs
reasonably small, such as 64x64 or 128x128. The displayed size is independent
from the source image resolution. Image files must be no larger than 8 MiB and
must be stored on a fixed local drive. Network, mapped, removable and optical
drives aren't read from Explorer's taskbar UI thread.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- images:
  - imageSource: ""
    $name: Image source (necessary)
    $description: >-
      Absolute path on a fixed local drive, or a path containing environment
      variables. Supports PNG, JPG and animated GIF files up to 8 MiB. This is
      the normal image and the final fallback for every other state.
  - hoverImageSource: ""
    $name: Hover image source
    $description: >-
      Image shown while the pointer is over the Start button. Leave blank to use
      the normal image.
  - pressedImageSource: ""
    $name: Pressed image source
    $description: >-
      Image shown while the Start button is pressed. Leave blank to use the
      normal image.
  - activatedImageSource: ""
    $name: Activated image source
    $description: >-
      Image shown while the Start menu is open. If blank or unavailable, the
      pressed image is used, followed by the hover image and normal image.
  - iconSize: 34
    $name: Icon size
    $description: >-
      Displayed size in device-independent pixels, from 8 to 128. The default is
      34.
  $name: Images
  $description: >-
    Configure the replacement images and their displayed size. Image files on
    network, mapped, removable and optical drives aren't supported.

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
      Crossfade duration in milliseconds when entering or leaving the hover
      state. Set to 0 to switch images instantly. The default is 120.
  - pressedFadeDuration: 80
    $name: Pressed and activated crossfade duration
    $description: >-
      Crossfade duration in milliseconds when entering or leaving the pressed
      or activated state. Set to 0 to switch images instantly. The default is
      80.
  $name: Image animation
  $description: Configure animated GIF playback and crossfades between images.

- hoverEffects:
  - hoverScale: 115
    $name: Scale
    $description: >-
      Icon scale as a percentage while hovering. 100 keeps its size. The default
      is 115.
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
      Icon scale as a percentage while pressed. 100 keeps its size. The default
      is 95.
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
      Effect transition duration in milliseconds when the pointer leaves or the
      button is released. The default is 140.
  - respectSystemAnimations: true
    $name: Respect Windows animation setting
    $description: >-
      Stop GIF playback and make all transitions instant when animation effects
      are disabled in Windows.
  $name: Animation behavior
  $description: Configure shared animation and accessibility behavior.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include <shcore.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
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

namespace wss = winrt::Windows::Storage::Streams;

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
constexpr uint64_t kMaximumImageFileSize = 8ULL * 1024 * 1024;

struct ModSettings {
    std::wstring imageSource;

    std::wstring hoverImageSource;
    std::wstring pressedImageSource;
    std::wstring activatedImageSource;

    int hoverFadeDuration = 120;
    int pressedFadeDuration = 80;

    int iconSize = 24;

    GifPlaybackMode gifPlayback = GifPlaybackMode::Always;

    double hoverScale = 1.0;
    double hoverRotation = 0.0;
    double hoverOpacity = 1.0;
    int hoverDuration = 120;

    double pressedScale = 1.0;
    double pressedRotation = 0.0;
    double pressedOpacity = 1.0;
    int pressedDuration = 80;

    int releaseDuration = 140;

    bool respectSystemAnimations = true;
};

ModSettings g_settings;
std::mutex g_settingsMutex;

std::atomic<bool> g_unloading = false;

// -----------------------------------------------------------------------------
// Start icon instance
// -----------------------------------------------------------------------------

struct ImageResource {
    wuxmi::BitmapImage bitmap{nullptr};

    // The source file is copied to memory so the file itself isn't kept open.
    wss::IRandomAccessStream memoryStream{nullptr};
    wf::IAsyncAction loadAction{nullptr};

    winrt::event_token imageOpenedToken{};
    winrt::event_token imageFailedToken{};

    bool imageOpenedAttached = false;
    bool imageFailedAttached = false;
    bool opened = false;
    bool failed = false;
};

struct StartIconInstance {
    DWORD threadId = 0;
    ModSettings settings;

    winrt::weak_ref<wux::FrameworkElement> startButton;
    winrt::weak_ref<wux::FrameworkElement> stockIcon;
    winrt::weak_ref<wuxc::Panel> hostPanel;
    winrt::weak_ref<wuxc::Grid> customImageHost;
    winrt::weak_ref<wuxc::Image> customImage;
    winrt::weak_ref<wuxc::Image> transitionImage;

    double originalStockOpacity = 1.0;

    std::array<ImageResource, kIconStateCount> imageResources;

    // Multiple logical states can share one decoded image resource when their
    // configured sources are identical.
    std::array<size_t, kIconStateCount> imageResourceIndexByState{0, 1, 2, 3};

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

    return value.get() ? value.get() : L"";
}

bool EqualsIgnoreCase(const std::wstring& a, const wchar_t* b) {
    return _wcsicmp(a.c_str(), b) == 0;
}

bool IsBlank(const std::wstring& value) {
    return value.find_first_not_of(L" \t\r\n") == std::wstring::npos;
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

    if (!result.empty() && result.back() == L'\0') {
        result.pop_back();
    }

    return result;
}

ModSettings LoadSettings() {
    ModSettings settings;

    settings.imageSource = GetStringSetting(L"images.imageSource");

    settings.hoverImageSource =
        GetStringSetting(L"images.hoverImageSource");

    settings.pressedImageSource =
        GetStringSetting(L"images.pressedImageSource");

    settings.activatedImageSource =
        GetStringSetting(L"images.activatedImageSource");

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

    return settings;
}

void StoreSettings(ModSettings settings) {
    std::lock_guard<std::mutex> lock(g_settingsMutex);

    g_settings = std::move(settings);
}

ModSettings GetSettingsSnapshot() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);

    return g_settings;
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

std::wstring GetImageSourceForState(const ModSettings& settings,
                                    IconState state) {
    if (state == IconState::Normal) {
        return settings.imageSource;
    }

    switch (state) {
        case IconState::Hover:
            return settings.hoverImageSource;

        case IconState::Pressed:
            return settings.pressedImageSource;

        case IconState::Activated:
            return settings.activatedImageSource;

        default:
            return L"";
    }
}

bool ImageResourceAvailable(const ImageResource& resource) {
    return resource.bitmap && resource.opened && !resource.failed;
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

    IconState desiredState = GetCurrentIconState(instance);

    size_t normalIndex = IconStateIndex(IconState::Normal);

    size_t selectedIndex = normalIndex;
    IconState selectedState = IconState::Normal;

    auto selectIfAvailable = [&](IconState state) {
        size_t stateIndex =
            instance->imageResourceIndexByState[IconStateIndex(state)];

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
                            instance->displayedState != selectedState;

    instance->requestedState = desiredState;
    instance->requestedStateSet = true;

    if (selectionChanged) {
        Wh_Log(L"Start icon state: requested=%s selected=%s",
               IconStateName(desiredState), IconStateName(selectedState));
    }

    if (!ImageResourceAvailable(selectedResource)) {
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
            firstLayer.Source(selectedResource.bitmap);

            SetImageLayerOpacity(firstLayer, 1.0f);
            SetImageLayerOpacity(secondLayer, 0.0f);

            instance->activeLayerIndex = 0;
        } else {
            size_t outgoingLayerIndex = instance->activeLayerIndex;
            size_t incomingLayerIndex = 1 - outgoingLayerIndex;

            auto outgoingLayer = GetImageLayer(instance, outgoingLayerIndex);
            auto incomingLayer = GetImageLayer(instance, incomingLayerIndex);

            incomingLayer.Source(selectedResource.bitmap);

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

    IconState resourceState =
        instance->activeImageIndex < kIconStateCount
            ? static_cast<IconState>(instance->activeImageIndex)
            : IconState::Normal;

    Wh_Log(L"Icon Status: state=%s resource=%s mode=%s status=%s",
           IconStateName(state), IconStateName(resourceState),
           GifPlaybackModeName(instance->settings.gifPlayback),
           GifAnimationStatusName(status));
}

void UpdateGifPlayback(const std::shared_ptr<StartIconInstance>& instance) {
    if (!instance) {
        return;
    }

    for (size_t i = 0; i < kIconStateCount; i++) {
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
        if (!activeResource.bitmap.IsAnimatedBitmap()) {
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

void PruneDeadInstancesForCurrentThread() {
    DWORD threadId = GetCurrentThreadId();
    std::vector<std::shared_ptr<StartIconInstance>> deadInstances;

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

            if (instance->threadId == threadId &&
                !instance->startButton.get()) {
                deadInstances.push_back(instance);
                it = g_instances->erase(it);
                continue;
            }

            ++it;
        }
    }

    for (const auto& instance : deadInstances) {
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

    for (size_t i = 0; i < kIconStateCount; i++) {
        auto& resource = instance->imageResources[i];

        if (resource.bitmap && resource.imageOpenedAttached) {
            try {
                resource.bitmap.ImageOpened(resource.imageOpenedToken);
            } catch (const winrt::hresult_error& e) {
                Wh_Log(L"Removing %s ImageOpened handler failed: 0x%08X %s",
                       IconStateName(static_cast<IconState>(i)), e.code().value,
                       e.message().c_str());
            } catch (...) {
                Wh_Log(L"Removing %s ImageOpened handler failed",
                       IconStateName(static_cast<IconState>(i)));
            }

            resource.imageOpenedAttached = false;
        }

        if (resource.bitmap && resource.imageFailedAttached) {
            try {
                resource.bitmap.ImageFailed(resource.imageFailedToken);
            } catch (const winrt::hresult_error& e) {
                Wh_Log(L"Removing %s ImageFailed handler failed: 0x%08X %s",
                       IconStateName(static_cast<IconState>(i)), e.code().value,
                       e.message().c_str());
            } catch (...) {
                Wh_Log(L"Removing %s ImageFailed handler failed",
                       IconStateName(static_cast<IconState>(i)));
            }

            resource.imageFailedAttached = false;
        }

        try {
            if (resource.bitmap) {
                resource.bitmap.Stop();
            }
        } catch (...) {
        }

        resource.loadAction = nullptr;
        resource.memoryStream = nullptr;
        resource.bitmap = nullptr;
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

void DetachAllForCurrentThread() {
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

// -----------------------------------------------------------------------------
// Bitmap/image loading
// -----------------------------------------------------------------------------

bool IsFixedLocalImagePath(const std::wstring& path) {
    // Accept ordinary drive-letter paths and their extended-length form. UNC,
    // device, relative and volume-GUID paths are intentionally rejected before
    // CreateFileW can block Explorer's taskbar UI thread on external storage.
    size_t driveOffset = 0;

    if (path.compare(0, 4, L"\\\\?\\") == 0) {
        driveOffset = 4;
    }

    if (path.size() < driveOffset + 3) {
        return false;
    }

    wchar_t driveLetter = path[driveOffset];
    bool isDriveLetter =
        (driveLetter >= L'A' && driveLetter <= L'Z') ||
        (driveLetter >= L'a' && driveLetter <= L'z');

    if (!isDriveLetter || path[driveOffset + 1] != L':' ||
        (path[driveOffset + 2] != L'\\' &&
         path[driveOffset + 2] != L'/')) {
        return false;
    }

    wchar_t driveRoot[] = {driveLetter, L':', L'\\', L'\0'};

    return GetDriveTypeW(driveRoot) == DRIVE_FIXED;
}

HRESULT CreateMemoryBackedStreamFromFile(
    const std::wstring& path,
    wss::IRandomAccessStream& randomAccessStream) {
    if (!IsFixedLocalImagePath(path)) {
        return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);
    }

    HANDLE file =
        CreateFileW(path.c_str(), GENERIC_READ,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    nullptr, OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);

    if (file == INVALID_HANDLE_VALUE) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    std::unique_ptr<void, decltype(&CloseHandle)> fileOwner(file, CloseHandle);

    LARGE_INTEGER fileSize{};

    if (!GetFileSizeEx(file, &fileSize)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    if (fileSize.QuadPart < 0 ||
        static_cast<uint64_t>(fileSize.QuadPart) > kMaximumImageFileSize) {
        return HRESULT_FROM_WIN32(ERROR_FILE_TOO_LARGE);
    }

    winrt::com_ptr<IStream> memoryStream;
    HRESULT hr = CreateStreamOnHGlobal(nullptr, TRUE, memoryStream.put());

    if (FAILED(hr)) {
        return hr;
    }

    std::array<BYTE, 64 * 1024> buffer;
    uint64_t totalBytesRead = 0;

    while (true) {
        DWORD bytesRead = 0;

        if (!ReadFile(file, buffer.data(), static_cast<DWORD>(buffer.size()),
                      &bytesRead, nullptr)) {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        if (bytesRead == 0) {
            break;
        }

        if (totalBytesRead > kMaximumImageFileSize - bytesRead) {
            return HRESULT_FROM_WIN32(ERROR_FILE_TOO_LARGE);
        }

        totalBytesRead += bytesRead;

        ULONG bytesWritten = 0;
        hr = memoryStream->Write(buffer.data(), bytesRead, &bytesWritten);

        if (FAILED(hr)) {
            return hr;
        }

        if (bytesWritten != bytesRead) {
            return STG_E_MEDIUMFULL;
        }
    }

    // Close the source file before XAML starts decoding. The user can replace
    // or delete it while the mod remains enabled.
    fileOwner.reset();

    LARGE_INTEGER beginning{};
    hr = memoryStream->Seek(beginning, STREAM_SEEK_SET, nullptr);

    if (FAILED(hr)) {
        return hr;
    }

    winrt::guid streamGuid = winrt::guid_of<wss::IRandomAccessStream>();

    return CreateRandomAccessStreamOverStream(
        memoryStream.get(), BSOS_DEFAULT,
        reinterpret_cast<const IID&>(streamGuid),
        reinterpret_cast<void**>(winrt::put_abi(randomAccessStream)));
}

bool LoadImageResource(const std::shared_ptr<StartIconInstance>& instance,
                       IconState state,
                       const std::wstring& source) {
    if (!instance || source.empty()) {
        return false;
    }

    size_t stateIndex = IconStateIndex(state);

    auto& resource = instance->imageResources[stateIndex];

    try {
        wuxmi::BitmapImage bitmap;

        // Only the currently displayed GIF should run. Playback is updated
        // after every state change and when decoding completes.
        bitmap.AutoPlay(false);

        resource.bitmap = bitmap;
        resource.opened = false;
        resource.failed = false;

        std::weak_ptr<StartIconInstance> weakInstance = instance;

        resource.imageOpenedToken = bitmap.ImageOpened(
            [weakInstance, stateIndex](const wf::IInspectable&,
                                       const wux::RoutedEventArgs&) {
                auto instance = weakInstance.lock();

                if (!instance) {
                    return;
                }

                auto& resource = instance->imageResources[stateIndex];

                resource.opened = true;
                resource.failed = false;
                resource.loadAction = nullptr;
                resource.memoryStream = nullptr;

                auto state = static_cast<IconState>(stateIndex);

                Wh_Log(L"Custom Start %s image loaded", IconStateName(state));

                UpdateDisplayedImage(instance);
                UpdateGifPlayback(instance);
            });

        resource.imageOpenedAttached = true;

        resource.imageFailedToken =
            bitmap.ImageFailed([weakInstance, stateIndex](
                                   const wf::IInspectable&,
                                   const wux::ExceptionRoutedEventArgs& args) {
                auto instance = weakInstance.lock();

                if (!instance) {
                    return;
                }

                auto& resource = instance->imageResources[stateIndex];

                resource.opened = false;
                resource.failed = true;
                resource.loadAction = nullptr;
                resource.memoryStream = nullptr;

                auto state = static_cast<IconState>(stateIndex);

                Wh_Log(L"Custom Start %s image failed: %s",
                       IconStateName(state), args.ErrorMessage().c_str());

                UpdateDisplayedImage(instance);
                UpdateGifPlayback(instance);
            });

        resource.imageFailedAttached = true;

        std::wstring path = ExpandPath(source);

        if (path.empty()) {
            resource.failed = true;
            return false;
        }

        if (!IsFixedLocalImagePath(path)) {
            resource.failed = true;

            Wh_Log(L"Reading %s image \"%s\" was refused: the source must "
                   L"be on a fixed local drive",
                   IconStateName(state), path.c_str());

            return false;
        }

        wss::IRandomAccessStream stream{nullptr};

        HRESULT hr = CreateMemoryBackedStreamFromFile(path, stream);

        if (FAILED(hr) || !stream) {
            resource.failed = true;

            if (hr == HRESULT_FROM_WIN32(ERROR_FILE_TOO_LARGE)) {
                Wh_Log(L"Reading %s image \"%s\" was refused: the file "
                       L"exceeds the 8 MiB limit",
                       IconStateName(state), path.c_str());
            } else {
                Wh_Log(L"Reading %s image \"%s\" failed: 0x%08X",
                       IconStateName(state), path.c_str(), hr);
            }

            return false;
        }

        resource.memoryStream = stream;

        // SetSourceAsync decodes the bounded in-memory copy asynchronously. The
        // source file itself was read synchronously above.
        resource.loadAction = bitmap.SetSourceAsync(stream);

        return true;
    } catch (const winrt::hresult_error& e) {
        resource.failed = true;

        Wh_Log(L"Loading %s image failed: 0x%08X %s", IconStateName(state),
               e.code().value, e.message().c_str());
    } catch (...) {
        resource.failed = true;

        Wh_Log(L"Loading %s image failed with unknown exception",
               IconStateName(state));
    }

    return false;
}

bool LoadImages(const std::shared_ptr<StartIconInstance>& instance) {
    if (!instance) {
        return false;
    }

    std::array<std::wstring, kIconStateCount> sources;

    for (size_t i = 0; i < kIconStateCount; i++) {
        instance->imageResourceIndexByState[i] = i;

        sources[i] = GetImageSourceForState(instance->settings,
                                            static_cast<IconState>(i));
    }

    if (!LoadImageResource(instance, IconState::Normal,
                           sources[IconStateIndex(IconState::Normal)])) {
        return false;
    }

    for (size_t stateIndex = 1; stateIndex < kIconStateCount; stateIndex++) {
        IconState state = static_cast<IconState>(stateIndex);

        if (sources[stateIndex].empty()) {
            continue;
        }

        bool reusedResource = false;

        for (size_t previousIndex = 0; previousIndex < stateIndex;
             previousIndex++) {
            if (!ImageSourcesEqual(sources[stateIndex],
                                   sources[previousIndex])) {
                continue;
            }

            instance->imageResourceIndexByState[stateIndex] =
                instance->imageResourceIndexByState[previousIndex];

            Wh_Log(L"The %s image reuses the %s image resource",
                   IconStateName(state),
                   IconStateName(static_cast<IconState>(previousIndex)));

            reusedResource = true;
            break;
        }

        if (reusedResource) {
            continue;
        }

        if (!LoadImageResource(instance, state, sources[stateIndex])) {
            Wh_Log(L"The %s image is unavailable; using its fallback",
                   IconStateName(state));
        }
    }

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

bool AttachToStartButton(const wux::FrameworkElement& startButton) {
    if (!startButton || g_unloading) {
        return false;
    }

    ModSettings settings = GetSettingsSnapshot();

    if (IsBlank(GetImageSourceForState(settings, IconState::Normal))) {
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
    auto releaseSharedPtr = [sharedPtr]() {
        if (sharedPtr[1] && RefCountDecref_Original) {
            RefCountDecref_Original(sharedPtr[1]);

            sharedPtr[1] = nullptr;
        }
    };

    if (!sharedPtr[0]) {
        releaseSharedPtr();
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

        releaseSharedPtr();
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

        releaseSharedPtr();
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

    releaseSharedPtr();

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
    };

    DWORD threadId = GetWindowThreadProcessId(window, nullptr);

    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                const CWPSTRUCT* cwp =
                    reinterpret_cast<const CWPSTRUCT*>(lParam);

                if (cwp->message == registeredMessage) {
                    auto* parameter = reinterpret_cast<RunParam*>(cwp->lParam);

                    parameter->proc(parameter->param);
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

    return parameter.invoked.load();
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

// -----------------------------------------------------------------------------
// Apply/remove on taskbar thread
// -----------------------------------------------------------------------------

void ApplyFromTaskbarThread(bool mainTaskbarOnly = false) {
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND window, LPARAM param) -> BOOL {
            bool mainOnly = param != 0;

            wchar_t className[64]{};

            if (!GetClassNameW(window, className, ARRAYSIZE(className))) {
                return TRUE;
            }

            wux::XamlRoot xamlRoot{nullptr};

            if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
                xamlRoot = GetTaskbarXamlRoot(window);
            } else if (!mainOnly &&
                       _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
                xamlRoot = GetSecondaryTaskbarXamlRoot(window);
            } else {
                return TRUE;
            }

            if (!xamlRoot) {
                Wh_Log(L"Couldn't obtain taskbar XamlRoot");

                return TRUE;
            }

            ApplyToXamlRoot(xamlRoot);

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

    HWND taskbarWnd = CSecondaryTray_GetTrayWindow_Original(pThis);

    if (!taskbarWnd) {
        return;
    }

    auto xamlRoot = GetSecondaryTaskbarXamlRoot(taskbarWnd);

    if (xamlRoot) {
        ApplyToXamlRoot(xamlRoot);
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
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateButtonPadding(void))"},
            &ExperienceToggleButton_UpdateButtonPadding_Original,
            ExperienceToggleButton_UpdateButtonPadding_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"Failed to resolve Taskbar.View.dll symbols");
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
    if (g_unloading || !module || GetTaskbarViewModuleHandle() != module) {
        return;
    }

    bool expected = false;

    if (!g_taskbarViewDllLoaded.compare_exchange_strong(expected, true)) {
        return;
    }

    Wh_Log(L"Taskbar view module loaded");

    if (!HookTaskbarViewDllSymbols(module)) {
        g_taskbarViewDllLoaded = false;
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

    StoreSettings(LoadSettings());

    if (IsBlank(GetSettingsSnapshot().imageSource)) {
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

    // Run synchronously on each owning taskbar UI thread. Unlike a queued
    // CoreDispatcher callback, this has completed before RunFromWindowThread
    // returns, so no handler implemented by this DLL remains queued or
    // registered when controlled unload continues.
    RunOnAllTaskbarThreads(DetachAllTaskbarsProc, nullptr);

    // A taskbar window can be recreated while windows are being enumerated.
    // Retry once if the first pass didn't visit every tracked UI thread.
    size_t remainingInstances = GetTrackedInstanceCount();

    if (remainingInstances != 0) {
        Wh_Log(L"%zu Start icon instance(s) remained after cleanup; retrying",
               remainingInstances);

        RunOnAllTaskbarThreads(DetachAllTaskbarsProc, nullptr);
        remainingInstances = GetTrackedInstanceCount();
    }

    if (remainingInstances != 0) {
        Wh_Log(L"Critical: %zu Start icon instance(s) couldn't be detached",
               remainingInstances);
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Start Button Replacer unloaded");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Start Button Replacer settings changed");

    StoreSettings(LoadSettings());

    RunOnAllTaskbarThreads(RebuildAllTaskbarsProc, nullptr);
}

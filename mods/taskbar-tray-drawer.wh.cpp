// ==WindhawkMod==
// @id              taskbar-tray-drawer
// @name            Taskbar Tray Drawer
// @description     Hides the Windows 11 system tray icons until you hover the tray, then slides them smoothly back into place
// @version         1.1.0
// @author          sempier
// @github          https://github.com/Sempier
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// The taskbar XAML plumbing (GetTaskbarXamlRoot, RunFromWindowThread, the
// SystemTray::IconView hook and the visual tree helpers) is adapted from
// m417z's Windhawk mods, in particular "Taskbar tray system icon tweaks".

// ==WindhawkModReadme==
/*
# Taskbar Tray Drawer

Turns the right hand side of the Windows 11 taskbar into a drawer. The icons you
pick are hidden, and slide back into place when you hover the tray.

![The tray icons sliding into place as the pointer enters, and away again as it leaves](https://i.imgur.com/4SVabnu.gif)

Everything is a toggle: pick exactly which groups go into the drawer, whether the
space they used is kept or reclaimed, what opens the drawer, and how the
animation feels.

## What can go in the drawer

Each group is a separate switch:

* **App tray icons** - the notification area icons and the "show hidden icons"
  chevron (`NotifyIconStack`).
* **Status icons** - microphone, location, Studio Effects, Recall (`MainStack`).
* **Language / IME indicator** (`NonActivatableStack`).
* **Wi-Fi, volume and battery** - the Quick Settings button
  (`ControlCenterButton`).
* **Clock, date and notification bell** - the Notification Center button
  (`NotificationCenterButton`).
* **"Show desktop" strip** (`ShowDesktopStack`).

## Closed state

* **Keep the space** - the slot stays the same width but empty, so nothing else
  on the taskbar moves. Pure transform and opacity, no layout work, the smoothest
  option. The empty slot is what you hover.
* **Collapse completely** - the width animates to zero and whatever is left of
  the tray slides over to the taskbar edge. Looks properly gone, but animates
  layout on every frame.
* **Collapse to a thin handle** - collapses, but leaves a narrow always visible
  strip that acts as the drawer handle.

If you hide *everything* and choose "Collapse completely", there is nothing left
to hover. Set **Hover target** to "Anywhere on the taskbar", or use the thin
handle instead.

## Motion

The slide is the base movement. **Shrink to** and **Rotate by** layer on top of
it, and both pivot around the anchor set by **Shrink and rotate around**, which
by default is the edge the icons slide towards, so they appear to be pulled out
of the taskbar rather than shrinking in place. Set the slide to "Do not move,
only fade" to use either effect on its own.

Ten easing curves are available, from **Gentle** through **Very strong**, plus
**Overshoot**, **Bounce** and **Elastic**. **Easing strength** controls how
pronounced those last three are; the rest ignore it.

Every duration and delay is in milliseconds and can be set individually. **Overall
speed** then scales all of them at once, so the whole animation can be retimed
without touching each field.

## Known limitations

* Clicking a tray icon opens its flyout and moves the pointer off the taskbar,
  so the drawer closes underneath the flyout. The flyout itself keeps working.
* Touch and keyboard focus do not open the drawer; it is pointer hover driven.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hideNotifyIcons: true
  $name: Hide app tray icons
  $description: The notification area icons and the "show hidden icons" chevron.
- hideStatusIcons: true
  $name: Hide status icons
  $description: Microphone, location, Studio Effects and Recall indicators.
- hideLanguage: true
  $name: Hide language / IME indicator
  $description: The keyboard layout indicator.
- hideSystemIcons: false
  $name: Hide Wi-Fi, volume and battery
  $description: The Quick Settings button.
- hideClock: false
  $name: Hide clock, date and notification bell
  $description: >-
    The Notification Center button. If you hide this as well, make sure "Hover
    target" is set to "Anywhere on the taskbar", or that the closed state leaves
    a handle, otherwise there is nothing left to hover over.
- hideShowDesktop: false
  $name: Hide the "Show desktop" strip
  $description: The thin strip at the very end of the taskbar.
- closedMode: reserved
  $name: When closed
  $options:
  - reserved: Keep the space empty (nothing else moves)
  - collapse: Collapse completely (the rest of the tray slides over)
  - sliver: Collapse to a thin handle
- sliverWidth: 12
  $name: Handle width (px)
  $description: Width of the strip left behind in "Collapse to a thin handle" mode.
- trigger: tray
  $name: Hover target
  $options:
  - tray: The tray area
  - taskbar: Anywhere on the taskbar
- openDelay: 60
  $name: Open delay (ms)
  $description: >-
    How long the pointer has to rest on the target before the drawer opens.
    Stops it flying open when you just sweep across the taskbar.
- openDuration: 220
  $name: Open duration (ms)
- closeDelay: 350
  $name: Close delay (ms)
  $description: Grace period after the pointer leaves, before the drawer closes.
- closeDuration: 160
  $name: Close duration (ms)
- stagger: 25
  $name: Stagger (ms)
  $description: >-
    Delay added between consecutive groups so they cascade instead of moving as
    one block. Set to 0 to move everything together.
- slide: right
  $name: Slide direction
  $description: Which way the icons travel as they hide.
  $options:
  - right: Out to the right
  - left: Out to the left
  - down: Down, into the taskbar
  - up: Up, out of the taskbar
  - none: Do not move, only fade
- slideDistance: 0
  $name: Slide distance (px)
  $description: 0 means each group travels exactly its own width (or height).
- fade: true
  $name: Fade while sliding
- scaleAmount: 100
  $name: Shrink to (%)
  $description: >-
    Scale the icons down to this percentage as they hide, on top of the slide.
    100 disables scaling. Try 70 for a drawer that pulls away from you.
- rotateAmount: 0
  $name: Rotate by (degrees)
  $description: >-
    Rotate the icons as they hide, on top of the slide. 0 disables rotation.
    Small values read best, roughly 10 to 20.
- transformOrigin: auto
  $name: Shrink and rotate around
  $description: The anchor point that scaling and rotation pivot around.
  $options:
  - auto: The edge they slide towards
  - center: Their centre
  - leading: Their left / top edge
  - trailing: Their right / bottom edge
- easing: standard
  $name: Easing
  $options:
  - standard: Smooth
  - decelerate: Snappy
  - sine: Gentle
  - circle: Strong
  - exponential: Very strong
  - power: Firm
  - overshoot: Overshoot
  - bounce: Bounce
  - elastic: Elastic
  - linear: Linear
- easingStrength: 100
  $name: Easing strength (%)
  $description: >-
    How pronounced the overshoot, bounce and elastic curves are. The other
    easings ignore it. 100 is the natural amount.
- durationScale: 100
  $name: Overall speed (%)
  $description: >-
    Scales every duration and delay above at once, so you can retime the whole
    animation without editing each field. 100 leaves them as typed, 200 makes
    everything twice as slow, 50 twice as fast.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <functional>
#include <initializer_list>
#include <list>
#include <memory>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

// windows.h defines GetCurrentTime as a macro, which collides with
// Storyboard::GetCurrentTime in the XAML animation headers.
#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>

using namespace winrt::Windows::UI::Xaml;

namespace CO = winrt::Windows::UI::Composition;
namespace WF = winrt::Windows::Foundation;
namespace NUM = winrt::Windows::Foundation::Numerics;
namespace ANIM = winrt::Windows::UI::Xaml::Media::Animation;

using WF::TimeSpan;
using winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview;

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

enum class ClosedMode { reserved, collapse, sliver };
enum class SlideDir { right, left, down, up, none };
enum class TransformOrigin { autoEdge, center, leading, trailing };
enum class Easing {
    standard,
    decelerate,
    sine,
    circle,
    exponential,
    power,
    overshoot,
    bounce,
    elastic,
    linear,
};

struct Settings {
    bool hideNotifyIcons;
    bool hideStatusIcons;
    bool hideLanguage;
    bool hideSystemIcons;
    bool hideClock;
    bool hideShowDesktop;
    ClosedMode closedMode;
    int sliverWidth;
    bool triggerWholeTaskbar;
    int openDelay;
    int openDuration;
    int closeDelay;
    int closeDuration;
    int stagger;
    SlideDir slide;
    int slideDistance;
    bool fade;
    int scaleAmount;
    int rotateAmount;
    TransformOrigin transformOrigin;
    Easing easing;
    int easingStrength;
};

Settings g_settings;
std::atomic<bool> g_unloading;

// -----------------------------------------------------------------------------
// Visual tree helpers
// -----------------------------------------------------------------------------

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }

        if (enumCallback(child)) {
            return child;
        }
    }

    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

FrameworkElement GetParentElementByName(FrameworkElement element,
                                        PCWSTR name) {
    auto parent = element;
    while (parent && parent.Name() != name) {
        parent = Media::VisualTreeHelper::GetParent(parent)
                     .try_as<FrameworkElement>();
    }

    return parent;
}

FrameworkElement GetParentElementByClassName(FrameworkElement element,
                                             PCWSTR className) {
    auto parent = element;
    while (parent && winrt::get_class_name(parent) != className) {
        parent = Media::VisualTreeHelper::GetParent(parent)
                     .try_as<FrameworkElement>();
    }

    return parent;
}

bool IsCursorOnElement(FrameworkElement element, HWND hWnd) {
    POINT pt;
    if (!GetCursorPos(&pt) || !ScreenToClient(hWnd, &pt)) {
        return false;
    }

    UINT dpi = GetDpiForWindow(hWnd);
    if (!dpi) {
        return false;
    }

    int logicalX = MulDiv(pt.x, 96, dpi);
    int logicalY = MulDiv(pt.y, 96, dpi);
    auto topLeft = element.TransformToVisual(nullptr).TransformPoint({0, 0});
    float width = (float)element.ActualWidth();
    float height = (float)element.ActualHeight();

    return logicalX >= topLeft.X && logicalX < topLeft.X + width &&
           logicalY >= topLeft.Y && logicalY < topLeft.Y + height;
}

// Defined further down, alongside the rest of the taskbar plumbing.
struct TaskbarWnd {
    HWND hWnd;
    bool secondary;
};

std::vector<TaskbarWnd> FindCurrentProcessTaskbarWnds();
XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd);
XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd);

// The pointer handlers need a window to hit test the cursor against, and the
// IconView path only has an element to start from.
HWND FindTaskbarWndForXamlRoot(XamlRoot xamlRoot) {
    if (!xamlRoot) {
        return nullptr;
    }

    for (const auto& taskbarWnd : FindCurrentProcessTaskbarWnds()) {
        try {
            auto candidate = taskbarWnd.secondary
                                 ? GetSecondaryTaskbarXamlRoot(taskbarWnd.hWnd)
                                 : GetTaskbarXamlRoot(taskbarWnd.hWnd);
            if (candidate && candidate == xamlRoot) {
                return taskbarWnd.hWnd;
            }
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"XamlRoot lookup failed: %s", ex.message().c_str());
        }
    }

    return nullptr;
}

// -----------------------------------------------------------------------------
// Drawer state
// -----------------------------------------------------------------------------

struct DrawerItem {
    winrt::weak_ref<FrameworkElement> element;
    // Last known laid out width, sampled every time the drawer closes, while
    // the element is still at its natural size. Used as the target of the width
    // animation when re-opening in collapse/sliver mode.
    double naturalWidth = 0;
    ANIM::Storyboard widthStoryboard{nullptr};
};

struct Drawer {
    winrt::weak_ref<FrameworkElement> grid;     // SystemTrayFrameGrid
    winrt::weak_ref<FrameworkElement> trigger;  // element carrying the handlers
    HWND hTaskbarWnd = nullptr;                 // for cursor hit testing
    std::vector<DrawerItem> items;
    bool open = true;
    bool pendingOpen = false;
    bool gridBackgroundSet = false;
    winrt::event_token enteredToken{};
    winrt::event_token exitedToken{};
    winrt::event_token captureLostToken{};
    winrt::event_token canceledToken{};
    winrt::event_token timerToken{};
    DispatcherTimer timer{nullptr};
};

// Only ever touched from the taskbar UI thread.
//
// Wh_ModUninit does not run when explorer.exe itself terminates, so a plain
// global would release its DispatcherTimers and Storyboards from the CRT
// shutdown thread, after the XAML core may already be gone. The destructor is
// suppressed and the real release happens on the UI thread in ApplySettings.
[[clang::no_destroy]] std::optional<std::vector<std::shared_ptr<Drawer>>>
    g_drawers{std::in_place};

// -----------------------------------------------------------------------------
// Animation
// -----------------------------------------------------------------------------

// Scales the "how much of it" parameter of the springy curves. Never returns
// less than one repetition, a bounce with zero bounces does not animate.
int EasingRepetitions(int natural) {
    int scaled = (int)lround(natural * (g_settings.easingStrength / 100.0));
    return std::max(1, scaled);
}

CO::CompositionEasingFunction CreateEasing(CO::Compositor const& compositor) {
    using Mode = CO::CompositionEasingFunctionMode;

    float strength = g_settings.easingStrength / 100.0f;

    // The named easing factories need ICompositor6 (Windows 10 2004+). Fall
    // back to the hand-rolled bezier if this build does not have them.
    try {
        using Factory = CO::CompositionEasingFunction;

        switch (g_settings.easing) {
            case Easing::sine:
                return Factory::CreateSineEasingFunction(compositor, Mode::Out);
            case Easing::circle:
                return Factory::CreateCircleEasingFunction(compositor,
                                                           Mode::Out);
            case Easing::exponential:
                return Factory::CreateExponentialEasingFunction(compositor,
                                                                Mode::Out, 5.0f);
            case Easing::power:
                return Factory::CreatePowerEasingFunction(compositor, Mode::Out,
                                                          3.0f);
            case Easing::overshoot:
                return Factory::CreateBackEasingFunction(
                    compositor, Mode::Out, std::max(0.0f, 0.4f * strength));
            case Easing::bounce:
                return Factory::CreateBounceEasingFunction(
                    compositor, Mode::Out, EasingRepetitions(3), 2.0f);
            case Easing::elastic:
                return Factory::CreateElasticEasingFunction(
                    compositor, Mode::Out, EasingRepetitions(3), 3.0f);
            default:
                break;
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Easing unavailable, falling back: %s", ex.message().c_str());
    }

    switch (g_settings.easing) {
        case Easing::decelerate:
            return compositor.CreateCubicBezierEasingFunction({0.1f, 0.9f},
                                                              {0.2f, 1.0f});
        case Easing::overshoot:
            return compositor.CreateCubicBezierEasingFunction({0.34f, 1.56f},
                                                              {0.64f, 1.0f});
        case Easing::linear:
            return compositor.CreateLinearEasingFunction();
        case Easing::standard:
        default:
            return compositor.CreateCubicBezierEasingFunction({0.2f, 0.0f},
                                                              {0.0f, 1.0f});
    }
}

// The two bezier curves have no XAML easing class. Returns their control
// points so they can be expressed as a KeySpline, or nullopt for the named
// easings, which map onto XAML easing classes directly.
std::optional<std::pair<WF::Point, WF::Point>> XamlEasingSpline() {
    switch (g_settings.easing) {
        case Easing::standard:
            return std::pair{WF::Point{0.2f, 0.0f}, WF::Point{0.0f, 1.0f}};
        case Easing::decelerate:
            return std::pair{WF::Point{0.1f, 0.9f}, WF::Point{0.2f, 1.0f}};
        default:
            return std::nullopt;
    }
}

// The XAML counterpart, used for the MaxWidth animation in the collapse and
// sliver modes. Returns null for linear, which means "no easing".
ANIM::EasingFunctionBase CreateXamlEasing() {
    auto withMode = [](auto ease) {
        ease.EasingMode(ANIM::EasingMode::EaseOut);
        return ease;
    };

    switch (g_settings.easing) {
        case Easing::sine:
            return withMode(ANIM::SineEase());
        case Easing::circle:
            return withMode(ANIM::CircleEase());
        case Easing::exponential: {
            auto ease = withMode(ANIM::ExponentialEase());
            ease.Exponent(5.0);
            return ease;
        }
        case Easing::power: {
            auto ease = withMode(ANIM::PowerEase());
            ease.Power(3.0);
            return ease;
        }
        case Easing::overshoot: {
            auto ease = withMode(ANIM::BackEase());
            ease.Amplitude(std::max(0.0, 0.4 * g_settings.easingStrength / 100.0));
            return ease;
        }
        case Easing::bounce: {
            auto ease = withMode(ANIM::BounceEase());
            ease.Bounces(EasingRepetitions(3));
            ease.Bounciness(2.0);
            return ease;
        }
        case Easing::elastic: {
            auto ease = withMode(ANIM::ElasticEase());
            ease.Oscillations(EasingRepetitions(3));
            ease.Springiness(3.0);
            return ease;
        }
        case Easing::linear:
            return nullptr;
        case Easing::standard:
        case Easing::decelerate:
        default:
            return withMode(ANIM::CubicEase());
    }
}

bool SlidesHorizontally() {
    return g_settings.slide == SlideDir::right ||
           g_settings.slide == SlideDir::left;
}

NUM::float3 ClosedTranslation(FrameworkElement const& element) {
    if (g_settings.slide == SlideDir::none) {
        return {0.0f, 0.0f, 0.0f};
    }

    float distance = (float)g_settings.slideDistance;
    if (distance <= 0.0f) {
        distance = SlidesHorizontally() ? (float)element.ActualWidth()
                                        : (float)element.ActualHeight();
    }
    if (distance <= 0.0f) {
        // Not laid out yet. Any non-zero value works, the item is faded out
        // anyway, and the real size is picked up on the next close.
        distance = 48.0f;
    }

    switch (g_settings.slide) {
        case SlideDir::right:
            return {distance, 0.0f, 0.0f};
        case SlideDir::left:
            return {-distance, 0.0f, 0.0f};
        case SlideDir::down:
            return {0.0f, distance, 0.0f};
        case SlideDir::up:
            return {0.0f, -distance, 0.0f};
        default:
            return {0.0f, 0.0f, 0.0f};
    }
}

// Scaling and rotation pivot around this point, in element local pixels.
NUM::float3 ComputeCenterPoint(FrameworkElement const& element) {
    float width = (float)element.ActualWidth();
    float height = (float)element.ActualHeight();

    switch (g_settings.transformOrigin) {
        case TransformOrigin::center:
            return {width / 2, height / 2, 0.0f};

        case TransformOrigin::leading:
            return SlidesHorizontally()
                       ? NUM::float3{0.0f, height / 2, 0.0f}
                       : NUM::float3{width / 2, 0.0f, 0.0f};

        case TransformOrigin::trailing:
            return SlidesHorizontally()
                       ? NUM::float3{width, height / 2, 0.0f}
                       : NUM::float3{width / 2, height, 0.0f};

        case TransformOrigin::autoEdge:
        default:
            switch (g_settings.slide) {
                case SlideDir::right:
                    return {width, height / 2, 0.0f};
                case SlideDir::left:
                    return {0.0f, height / 2, 0.0f};
                case SlideDir::down:
                    return {width / 2, height, 0.0f};
                case SlideDir::up:
                    return {width / 2, 0.0f, 0.0f};
                default:
                    return {width / 2, height / 2, 0.0f};
            }
    }
}

void AnimateVisual(FrameworkElement element,
                   bool open,
                   bool animate,
                   int delayMs) {
    ElementCompositionPreview::SetIsTranslationEnabled(element, true);

    auto visual = ElementCompositionPreview::GetElementVisual(element);
    if (!visual) {
        return;
    }

    bool usesScale = g_settings.scaleAmount != 100;
    bool usesRotation = g_settings.rotateAmount != 0;

    if (usesScale || usesRotation) {
        // Sizes change as icons come and go, so re-anchor on every transition.
        visual.CenterPoint(ComputeCenterPoint(element));
    }

    auto translation =
        open ? NUM::float3{0.0f, 0.0f, 0.0f} : ClosedTranslation(element);
    float opacity = (open || !g_settings.fade) ? 1.0f : 0.0f;
    float scale = open ? 1.0f : g_settings.scaleAmount / 100.0f;
    float rotation = open ? 0.0f : (float)g_settings.rotateAmount;

    if (!animate) {
        visual.StopAnimation(L"Translation");
        visual.StopAnimation(L"Opacity");
        visual.Properties().InsertVector3(L"Translation", translation);
        visual.Opacity(opacity);

        if (usesScale) {
            visual.StopAnimation(L"Scale");
            visual.Scale({scale, scale, 1.0f});
        }
        if (usesRotation) {
            visual.StopAnimation(L"RotationAngleInDegrees");
            visual.RotationAngleInDegrees(rotation);
        }
        return;
    }

    auto compositor = visual.Compositor();
    auto easing = CreateEasing(compositor);
    TimeSpan duration = std::chrono::milliseconds(
        open ? g_settings.openDuration : g_settings.closeDuration);
    TimeSpan delay = std::chrono::milliseconds(delayMs);

    auto translationAnim = compositor.CreateVector3KeyFrameAnimation();
    translationAnim.InsertKeyFrame(1.0f, translation, easing);
    translationAnim.Duration(duration);
    if (delayMs > 0) {
        translationAnim.DelayTime(delay);
    }
    visual.StartAnimation(L"Translation", translationAnim);

    auto opacityAnim = compositor.CreateScalarKeyFrameAnimation();
    opacityAnim.InsertKeyFrame(1.0f, opacity, easing);
    opacityAnim.Duration(duration);
    if (delayMs > 0) {
        opacityAnim.DelayTime(delay);
    }
    visual.StartAnimation(L"Opacity", opacityAnim);

    if (usesScale) {
        auto scaleAnim = compositor.CreateVector3KeyFrameAnimation();
        scaleAnim.InsertKeyFrame(1.0f, {scale, scale, 1.0f}, easing);
        scaleAnim.Duration(duration);
        if (delayMs > 0) {
            scaleAnim.DelayTime(delay);
        }
        visual.StartAnimation(L"Scale", scaleAnim);
    }

    if (usesRotation) {
        auto rotationAnim = compositor.CreateScalarKeyFrameAnimation();
        rotationAnim.InsertKeyFrame(1.0f, rotation, easing);
        rotationAnim.Duration(duration);
        if (delayMs > 0) {
            rotationAnim.DelayTime(delay);
        }
        visual.StartAnimation(L"RotationAngleInDegrees", rotationAnim);
    }
}

ANIM::Storyboard MakeWidthStoryboard(FrameworkElement element,
                                     double from,
                                     double to,
                                     int durationMs,
                                     int delayMs) {
    ANIM::Storyboard storyboard;
    ANIM::DoubleAnimation animation;

    animation.EnableDependentAnimation(true);
    animation.From(from);
    animation.To(to);

    TimeSpan duration = std::chrono::milliseconds(durationMs);
    animation.Duration(Duration{duration});
    if (delayMs > 0) {
        TimeSpan delay = std::chrono::milliseconds(delayMs);
        animation.BeginTime(delay);
    }

    // The two bezier curves have no XAML easing equivalent, so they are
    // expressed as a KeySpline instead. Without this the width would collapse
    // on a different curve than the slide it runs alongside.
    if (auto spline = XamlEasingSpline()) {
        ANIM::KeySpline keySpline;
        keySpline.ControlPoint1(spline->first);
        keySpline.ControlPoint2(spline->second);

        ANIM::SplineDoubleKeyFrame endFrame;
        endFrame.KeyTime(ANIM::KeyTime{duration});
        endFrame.Value(to);
        endFrame.KeySpline(keySpline);

        ANIM::DoubleAnimationUsingKeyFrames keyFrameAnimation;
        keyFrameAnimation.EnableDependentAnimation(true);
        if (delayMs > 0) {
            keyFrameAnimation.BeginTime(
                TimeSpan{std::chrono::milliseconds(delayMs)});
        }

        ANIM::DiscreteDoubleKeyFrame startFrame;
        startFrame.KeyTime(ANIM::KeyTime{TimeSpan{}});
        startFrame.Value(from);

        keyFrameAnimation.KeyFrames().Append(startFrame);
        keyFrameAnimation.KeyFrames().Append(endFrame);

        ANIM::Storyboard::SetTarget(keyFrameAnimation, element);
        ANIM::Storyboard::SetTargetProperty(keyFrameAnimation, L"MaxWidth");
        storyboard.Children().Append(keyFrameAnimation);

        return storyboard;
    }

    if (auto ease = CreateXamlEasing()) {
        animation.EasingFunction(ease);
    }

    ANIM::Storyboard::SetTarget(animation, element);
    ANIM::Storyboard::SetTargetProperty(animation, L"MaxWidth");
    storyboard.Children().Append(animation);

    return storyboard;
}

void AnimateWidth(DrawerItem& item,
                  FrameworkElement element,
                  bool open,
                  bool animate,
                  int delayMs) {
    // Sample before stopping. Storyboard::Stop() restores the property's
    // pre-animation base value, which is the natural width written by the
    // previous close, so reading ActualWidth afterwards would report the
    // element as already full size.
    double current = element.ActualWidth();

    // The handle outlives the animation - the open storyboard's Completed
    // handler clears MaxWidth but not the handle itself - so ask the clock
    // whether it is actually still running. Treating a finished storyboard as
    // in flight would stop naturalWidth ever being re-measured.
    bool wasAnimating =
        item.widthStoryboard &&
        item.widthStoryboard.GetCurrentState() == ANIM::ClockState::Active;

    if (item.widthStoryboard) {
        item.widthStoryboard.Stop();
        item.widthStoryboard = nullptr;
    }

    if (open) {
        if (item.naturalWidth <= 0 || !animate) {
            // Nothing sensible to animate towards, let layout take over.
            element.ClearValue(FrameworkElement::MaxWidthProperty());
            return;
        }

        // Pin the starting point, both for the animation itself and for the
        // BeginTime delay that precedes it.
        element.MaxWidth(current);

        auto storyboard =
            MakeWidthStoryboard(element, current, item.naturalWidth,
                                g_settings.openDuration, delayMs);
        auto weakElement = item.element;
        storyboard.Completed(
            [weakElement](WF::IInspectable const&, WF::IInspectable const&) {
                if (auto element = weakElement.get()) {
                    // Drop the constraint entirely so a later size change, an
                    // icon appearing say, is not clipped.
                    element.ClearValue(FrameworkElement::MaxWidthProperty());
                }
            });
        item.widthStoryboard = storyboard;
        storyboard.Begin();
        return;
    }

    // Closing. A fully open element is at its natural size, so this is the one
    // moment where that width can be measured, but only trust the reading if
    // no width animation was in flight, otherwise it is a partial value.
    if (!wasAnimating && current > 0) {
        item.naturalWidth = current;
    }

    if (current > 0) {
        element.MaxWidth(current);
    }

    if (!animate) {
        element.MaxWidth(0);
        return;
    }

    auto storyboard = MakeWidthStoryboard(element, current, 0,
                                          g_settings.closeDuration, delayMs);
    item.widthStoryboard = storyboard;
    storyboard.Begin();
}

void SetDrawerOpen(Drawer& drawer, bool open, bool animate) {
    drawer.open = open;

    int count = (int)drawer.items.size();
    for (int i = 0; i < count; i++) {
        auto& item = drawer.items[i];
        auto element = item.element.get();
        if (!element) {
            continue;
        }

        // Composition opacity does not affect XAML hit testing, so a faded out
        // icon would still swallow clicks in the empty slot.
        element.IsHitTestVisible(open);

        // Opening cascades left to right, closing right to left.
        int delayMs = g_settings.stagger * (open ? i : (count - 1 - i));

        try {
            AnimateVisual(element, open, animate, delayMs);

            if (g_settings.closedMode != ClosedMode::reserved) {
                AnimateWidth(item, element, open, animate, delayMs);
            }
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Animation failed: %s", ex.message().c_str());
        }
    }

    if (auto grid = drawer.grid.get()) {
        try {
            if (g_settings.closedMode == ClosedMode::sliver && !open) {
                grid.MinWidth(g_settings.sliverWidth);
            } else {
                grid.ClearValue(FrameworkElement::MinWidthProperty());
            }
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Grid update failed: %s", ex.message().c_str());
        }
    }
}

// -----------------------------------------------------------------------------
// Hover handling
// -----------------------------------------------------------------------------

void ScheduleTransition(std::shared_ptr<Drawer> drawer, bool open) {
    if (drawer->timer) {
        drawer->timer.Stop();
    }

    if (drawer->open == open) {
        return;
    }

    int delay = open ? g_settings.openDelay : g_settings.closeDelay;
    if (delay <= 0) {
        SetDrawerOpen(*drawer, open, true);
        return;
    }

    drawer->pendingOpen = open;

    if (!drawer->timer) {
        DispatcherTimer timer;
        std::weak_ptr<Drawer> weakDrawer = drawer;
        drawer->timerToken = timer.Tick(
            [weakDrawer](WF::IInspectable const&, WF::IInspectable const&) {
                auto drawer = weakDrawer.lock();
                if (!drawer) {
                    return;
                }
                drawer->timer.Stop();

                // PointerExited and PointerCaptureLost are bubbling routed
                // events, so a descendant reports an exit when the pointer
                // merely moves off an icon onto the tray behind it, or when a
                // button releases its capture on click. A real exit still
                // raises the event on the trigger itself, so re-checking where
                // the cursor actually is drops only the spurious closes.
                if (!drawer->pendingOpen && drawer->hTaskbarWnd) {
                    auto trigger = drawer->trigger.get();
                    if (trigger &&
                        IsCursorOnElement(trigger, drawer->hTaskbarWnd)) {
                        return;
                    }
                }

                SetDrawerOpen(*drawer, drawer->pendingOpen, true);
            });
        drawer->timer = timer;
    }

    drawer->timer.Interval(TimeSpan{std::chrono::milliseconds(delay)});
    drawer->timer.Start();
}

void AttachTrigger(std::shared_ptr<Drawer> drawer, FrameworkElement trigger) {
    auto element = trigger.try_as<UIElement>();
    if (!element) {
        return;
    }

    std::weak_ptr<Drawer> weakDrawer = drawer;

    drawer->enteredToken = element.PointerEntered(
        [weakDrawer](WF::IInspectable const&,
                     Input::PointerRoutedEventArgs const&) {
            if (auto drawer = weakDrawer.lock()) {
                ScheduleTransition(drawer, true);
            }
        });

    drawer->exitedToken = element.PointerExited(
        [weakDrawer](WF::IInspectable const&,
                     Input::PointerRoutedEventArgs const&) {
            if (auto drawer = weakDrawer.lock()) {
                ScheduleTransition(drawer, false);
            }
        });

    // If something takes the pointer capture, PointerExited may never arrive
    // and the drawer would stay open until the next enter/exit pair.
    drawer->captureLostToken = element.PointerCaptureLost(
        [weakDrawer](WF::IInspectable const&,
                     Input::PointerRoutedEventArgs const&) {
            if (auto drawer = weakDrawer.lock()) {
                ScheduleTransition(drawer, false);
            }
        });

    drawer->canceledToken = element.PointerCanceled(
        [weakDrawer](WF::IInspectable const&,
                     Input::PointerRoutedEventArgs const&) {
            if (auto drawer = weakDrawer.lock()) {
                ScheduleTransition(drawer, false);
            }
        });

    drawer->trigger = trigger;
}

// -----------------------------------------------------------------------------
// Building and tearing down drawers
// -----------------------------------------------------------------------------

bool IsMemberEnabled(std::wstring_view name) {
    if (name == L"NotifyIconStack") {
        return g_settings.hideNotifyIcons;
    }
    if (name == L"MainStack") {
        return g_settings.hideStatusIcons;
    }
    if (name == L"NonActivatableStack") {
        return g_settings.hideLanguage;
    }
    if (name == L"ControlCenterButton") {
        return g_settings.hideSystemIcons;
    }
    if (name == L"NotificationCenterButton") {
        return g_settings.hideClock;
    }
    if (name == L"ShowDesktopStack") {
        return g_settings.hideShowDesktop;
    }
    return false;
}

void TearDownDrawer(Drawer& drawer) {
    // Each step is guarded, so a failure early on cannot skip the handler
    // revocation or the element restore below it.
    try {
        if (drawer.timer) {
            drawer.timer.Stop();
            if (drawer.timerToken.value) {
                drawer.timer.Tick(drawer.timerToken);
            }
            drawer.timer = nullptr;
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Timer teardown failed: %s", ex.message().c_str());
    }
    drawer.timerToken = {};

    try {
        if (auto trigger = drawer.trigger.get()) {
            if (auto element = trigger.try_as<UIElement>()) {
                if (drawer.enteredToken.value) {
                    element.PointerEntered(drawer.enteredToken);
                }
                if (drawer.exitedToken.value) {
                    element.PointerExited(drawer.exitedToken);
                }
                if (drawer.captureLostToken.value) {
                    element.PointerCaptureLost(drawer.captureLostToken);
                }
                if (drawer.canceledToken.value) {
                    element.PointerCanceled(drawer.canceledToken);
                }
            }
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Handler revocation failed: %s", ex.message().c_str());
    }
    drawer.enteredToken = {};
    drawer.exitedToken = {};
    drawer.captureLostToken = {};
    drawer.canceledToken = {};

    for (auto& item : drawer.items) {
        if (item.widthStoryboard) {
            item.widthStoryboard.Stop();
            item.widthStoryboard = nullptr;
        }

        auto element = item.element.get();
        if (!element) {
            continue;
        }

        try {
            element.ClearValue(FrameworkElement::MaxWidthProperty());
            element.ClearValue(UIElement::IsHitTestVisibleProperty());

            auto visual = ElementCompositionPreview::GetElementVisual(element);
            if (visual) {
                visual.StopAnimation(L"Translation");
                visual.StopAnimation(L"Opacity");
                visual.StopAnimation(L"Scale");
                visual.StopAnimation(L"RotationAngleInDegrees");
                visual.Properties().InsertVector3(L"Translation",
                                                  {0.0f, 0.0f, 0.0f});
                visual.Opacity(1.0f);
                visual.Scale({1.0f, 1.0f, 1.0f});
                visual.RotationAngleInDegrees(0.0f);
                visual.CenterPoint({0.0f, 0.0f, 0.0f});
            }
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Restore failed: %s", ex.message().c_str());
        }
    }
    drawer.items.clear();

    if (auto grid = drawer.grid.get()) {
        try {
            grid.ClearValue(FrameworkElement::MinWidthProperty());
            if (drawer.gridBackgroundSet) {
                if (auto panel = grid.try_as<Controls::Panel>()) {
                    panel.ClearValue(Controls::Panel::BackgroundProperty());
                }
            }
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Grid restore failed: %s", ex.message().c_str());
        }
    }
    drawer.gridBackgroundSet = false;
}

void TearDownAllDrawers() {
    // Per drawer, so one failure cannot leave the others hidden with no mod
    // left to restore them, and so the list is always emptied - the drawers
    // would otherwise be destroyed later with their pointer handlers still
    // registered on live XAML elements.
    for (auto& drawer : *g_drawers) {
        try {
            TearDownDrawer(*drawer);
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Teardown failed: %s", ex.message().c_str());
        }
    }
    g_drawers->clear();
}

// Collects the enabled groups in tree order, so the stagger cascades in the
// same direction they are laid out. Existing entries keep their measured
// widths. Returns true if a group was picked up that was not tracked before,
// which happens when one appears later - NonActivatableStack, for instance,
// only exists once a second keyboard layout is added.
bool RefreshDrawerMembers(Drawer& drawer) {
    auto grid = drawer.grid.get();
    if (!grid) {
        return false;
    }

    std::vector<DrawerItem> refreshed;
    bool added = false;

    EnumChildElements(grid, [&drawer, &refreshed,
                             &added](FrameworkElement child) {
        if (!IsMemberEnabled(child.Name())) {
            return false;
        }

        for (const auto& item : drawer.items) {
            auto existing = item.element.get();
            if (existing && existing == child) {
                refreshed.push_back(item);
                return false;
            }
        }

        Wh_Log(L"Drawer member: %s", child.Name().c_str());
        refreshed.push_back(DrawerItem{.element = child});
        added = true;
        return false;
    });

    drawer.items = std::move(refreshed);
    return added;
}

void InitDrawerForGrid(FrameworkElement grid, HWND hTaskbarWnd) {
    for (auto& existing : *g_drawers) {
        if (auto existingGrid = existing->grid.get()) {
            if (existingGrid == grid) {
                if (!existing->hTaskbarWnd) {
                    existing->hTaskbarWnd = hTaskbarWnd;
                }

                // A group may have appeared since the drawer was built.
                if (RefreshDrawerMembers(*existing) && !existing->open) {
                    SetDrawerOpen(*existing, false, false);
                }
                return;
            }
        }
    }

    auto drawer = std::make_shared<Drawer>();
    drawer->grid = grid;

    RefreshDrawerMembers(*drawer);

    if (drawer->items.empty()) {
        Wh_Log(L"No drawer members found");
        return;
    }

    // A Panel with no background does not hit test, so the empty slot would not
    // raise pointer events once the icons are hidden.
    if (auto panel = grid.try_as<Controls::Panel>()) {
        if (!panel.Background()) {
            panel.Background(Media::SolidColorBrush(
                winrt::Windows::UI::Colors::Transparent()));
            drawer->gridBackgroundSet = true;
        }
    }

    FrameworkElement trigger = grid;
    if (g_settings.triggerWholeTaskbar) {
        if (auto taskbarFrame =
                GetParentElementByClassName(grid, L"Taskbar.TaskbarFrame")) {
            trigger = taskbarFrame;
        } else {
            Wh_Log(L"Taskbar frame not found, falling back to the tray");
        }
    }

    AttachTrigger(drawer, trigger);
    drawer->hTaskbarWnd = hTaskbarWnd;
    g_drawers->push_back(drawer);

    // Starting closed under the pointer would snap the tray shut and leave it
    // shut until the pointer left and came back, which is what happens when the
    // mod is enabled or a setting changed while hovering the tray.
    bool startOpen =
        hTaskbarWnd && IsCursorOnElement(trigger, hTaskbarWnd);

    SetDrawerOpen(*drawer, startOpen, false);
}

void InitDrawerFromDescendant(FrameworkElement element) {
    auto grid = GetParentElementByName(element, L"SystemTrayFrameGrid");
    if (!grid) {
        // Icons in the overflow flyout live in their own island.
        return;
    }

    InitDrawerForGrid(grid, FindTaskbarWndForXamlRoot(element.XamlRoot()));
}

void ApplyToXamlRoot(XamlRoot xamlRoot, HWND hTaskbarWnd) {
    auto content = xamlRoot.Content().try_as<FrameworkElement>();
    if (!content) {
        return;
    }

    auto systemTrayFrame =
        FindChildByClassName(content, L"SystemTray.SystemTrayFrame");
    if (!systemTrayFrame) {
        Wh_Log(L"SystemTray.SystemTrayFrame not found");
        return;
    }

    auto grid = FindChildByName(systemTrayFrame, L"SystemTrayFrameGrid");
    if (!grid) {
        Wh_Log(L"SystemTrayFrameGrid not found");
        return;
    }

    InitDrawerForGrid(grid, hTaskbarWnd);
}

// -----------------------------------------------------------------------------
// Hooks
// -----------------------------------------------------------------------------

// Same shutdown concern as g_drawers: each revoker's destructor calls back
// into a FrameworkElement, which is thread affine.
[[clang::no_destroy]] std::optional<std::list<FrameworkElement::Loaded_revoker>>
    g_autoRevokerList{std::in_place};

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original;

void* WINAPI IconView_IconView_Hook(void* pThis) {
    Wh_Log(L">");

    void* ret = IconView_IconView_Original(pThis);

    FrameworkElement iconView = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(iconView));
    if (!iconView) {
        return ret;
    }

    // The hooks stay installed until Wh_ModBeforeUninit returns, but the
    // globals are released before that. Writing into a disengaged optional
    // would corrupt the heap, not just fault.
    if (g_unloading || !g_autoRevokerList) {
        return ret;
    }

    g_autoRevokerList->emplace_back();
    auto autoRevokerIt = g_autoRevokerList->end();
    --autoRevokerIt;

    *autoRevokerIt = iconView.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](WF::IInspectable const& sender, RoutedEventArgs const&) {
            g_autoRevokerList->erase(autoRevokerIt);

            if (g_unloading) {
                return;
            }

            auto iconView = sender.try_as<FrameworkElement>();
            if (!iconView) {
                return;
            }

            try {
                InitDrawerFromDescendant(iconView);
            } catch (winrt::hresult_error const& ex) {
                Wh_Log(L"Init failed: %s", ex.message().c_str());
            }
        });

    return ret;
}

void* CTaskBand_ITaskListWndSite_vftable;
void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot XamlRootFromTaskbarHostSharedPtr(void** taskbarHostSharedPtr) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x48;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_ARM64)
    // Just use the default offset which will hopefully work in most cases.
#else
#error "Unsupported architecture"
#endif

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

// Secondary taskbars keep their task band in a child WorkerW rather than the
// TaskbandHWND property, and the band is a CSecondaryTaskBand, so neither the
// vtable comparison nor the accessor above applies to them.
XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    HWND hTaskSwWnd =
        (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                               taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

// Collects the taskbars of the current process. The primary taskbar comes
// first, followed by the secondary (per monitor) ones, which have their own
// system tray and so need their own drawer.
std::vector<TaskbarWnd> FindCurrentProcessTaskbarWnds() {
    std::vector<TaskbarWnd> taskbarWnds;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& taskbarWnds =
                *reinterpret_cast<std::vector<TaskbarWnd>*>(lParam);

            DWORD dwProcessId;
            WCHAR className[32];
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId() ||
                !GetClassName(hWnd, className, ARRAYSIZE(className))) {
                return TRUE;
            }

            if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
                taskbarWnds.insert(taskbarWnds.begin(),
                                   TaskbarWnd{hWnd, false});
            } else if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
                taskbarWnds.push_back(TaskbarWnd{hWnd, true});
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&taskbarWnds));

    return taskbarWnds;
}

// -----------------------------------------------------------------------------
// Settings plumbing
// -----------------------------------------------------------------------------

template <typename T>
T ReadEnumSetting(PCWSTR name,
                  std::initializer_list<std::pair<PCWSTR, T>> values,
                  T fallback) {
    WindhawkUtils::StringSetting value = WindhawkUtils::StringSetting::make(name);
    for (const auto& [text, enumValue] : values) {
        if (wcscmp(value.get(), text) == 0) {
            return enumValue;
        }
    }
    return fallback;
}

void LoadSettings() {
    g_settings.hideNotifyIcons = Wh_GetIntSetting(L"hideNotifyIcons");
    g_settings.hideStatusIcons = Wh_GetIntSetting(L"hideStatusIcons");
    g_settings.hideLanguage = Wh_GetIntSetting(L"hideLanguage");
    g_settings.hideSystemIcons = Wh_GetIntSetting(L"hideSystemIcons");
    g_settings.hideClock = Wh_GetIntSetting(L"hideClock");
    g_settings.hideShowDesktop = Wh_GetIntSetting(L"hideShowDesktop");

    g_settings.closedMode = ReadEnumSetting<ClosedMode>(
        L"closedMode",
        {{L"reserved", ClosedMode::reserved},
         {L"collapse", ClosedMode::collapse},
         {L"sliver", ClosedMode::sliver}},
        ClosedMode::reserved);

    g_settings.sliverWidth = Wh_GetIntSetting(L"sliverWidth");
    if (g_settings.sliverWidth < 0) {
        g_settings.sliverWidth = 0;
    }

    g_settings.triggerWholeTaskbar = ReadEnumSetting<bool>(
        L"trigger", {{L"tray", false}, {L"taskbar", true}}, false);

    // One knob that retimes everything, applied on top of the per-phase values.
    int durationScale = Wh_GetIntSetting(L"durationScale");
    if (durationScale < 1) {
        durationScale = 1;
    }
    auto scaled = [durationScale](int milliseconds) {
        return (int)((long long)milliseconds * durationScale / 100);
    };

    g_settings.openDelay = scaled(Wh_GetIntSetting(L"openDelay"));
    g_settings.openDuration = scaled(Wh_GetIntSetting(L"openDuration"));
    g_settings.closeDelay = scaled(Wh_GetIntSetting(L"closeDelay"));
    g_settings.closeDuration = scaled(Wh_GetIntSetting(L"closeDuration"));
    g_settings.stagger = scaled(Wh_GetIntSetting(L"stagger"));

    // Composition rejects a zero duration, and a zero duration animation is
    // pointless anyway.
    if (g_settings.openDuration < 1) {
        g_settings.openDuration = 1;
    }
    if (g_settings.closeDuration < 1) {
        g_settings.closeDuration = 1;
    }
    if (g_settings.openDelay < 0) {
        g_settings.openDelay = 0;
    }
    if (g_settings.closeDelay < 0) {
        g_settings.closeDelay = 0;
    }
    if (g_settings.stagger < 0) {
        g_settings.stagger = 0;
    }

    g_settings.slide = ReadEnumSetting<SlideDir>(L"slide",
                                                 {{L"right", SlideDir::right},
                                                  {L"left", SlideDir::left},
                                                  {L"down", SlideDir::down},
                                                  {L"up", SlideDir::up},
                                                  {L"none", SlideDir::none}},
                                                 SlideDir::right);

    g_settings.slideDistance = Wh_GetIntSetting(L"slideDistance");
    g_settings.fade = Wh_GetIntSetting(L"fade");

    g_settings.scaleAmount = Wh_GetIntSetting(L"scaleAmount");
    if (g_settings.scaleAmount < 0) {
        g_settings.scaleAmount = 0;
    }
    g_settings.rotateAmount = Wh_GetIntSetting(L"rotateAmount");

    g_settings.transformOrigin = ReadEnumSetting<TransformOrigin>(
        L"transformOrigin",
        {{L"auto", TransformOrigin::autoEdge},
         {L"center", TransformOrigin::center},
         {L"leading", TransformOrigin::leading},
         {L"trailing", TransformOrigin::trailing}},
        TransformOrigin::autoEdge);

    g_settings.easing =
        ReadEnumSetting<Easing>(L"easing",
                                {{L"standard", Easing::standard},
                                 {L"decelerate", Easing::decelerate},
                                 {L"sine", Easing::sine},
                                 {L"circle", Easing::circle},
                                 {L"exponential", Easing::exponential},
                                 {L"power", Easing::power},
                                 {L"overshoot", Easing::overshoot},
                                 {L"bounce", Easing::bounce},
                                 {L"elastic", Easing::elastic},
                                 {L"linear", Easing::linear}},
                                Easing::standard);

    g_settings.easingStrength = Wh_GetIntSetting(L"easingStrength");
    if (g_settings.easingStrength < 0) {
        g_settings.easingStrength = 0;
    }
}

void ApplySettings() {
    Wh_Log(L"Applying settings");

    auto taskbarWnds = FindCurrentProcessTaskbarWnds();
    if (taskbarWnds.empty()) {
        Wh_Log(L"No taskbar found");
        return;
    }

    // All taskbars of a process share one UI thread, so the primary taskbar's
    // thread can service the secondary ones too.
    RunFromWindowThread(
        taskbarWnds.front().hWnd,
        [](void* pParam) {
            auto& taskbarWnds = *(std::vector<TaskbarWnd>*)pParam;

            // Any IconView that was constructed but never loaded still holds a
            // Loaded subscription into this image. Drop them here, on the UI
            // thread, while that is still safe to do.
            g_autoRevokerList->clear();

            try {
                TearDownAllDrawers();
            } catch (winrt::hresult_error const& ex) {
                Wh_Log(L"Teardown failed: %s", ex.message().c_str());
            }

            if (g_unloading) {
                // reset() rather than clear(), so the buffers go too.
                g_autoRevokerList.reset();
                g_drawers.reset();
                return;
            }

            for (const auto& taskbarWnd : taskbarWnds) {
                try {
                    auto xamlRoot =
                        taskbarWnd.secondary
                            ? GetSecondaryTaskbarXamlRoot(taskbarWnd.hWnd)
                            : GetTaskbarXamlRoot(taskbarWnd.hWnd);
                    if (!xamlRoot) {
                        Wh_Log(L"Getting XamlRoot failed");
                        continue;
                    }

                    ApplyToXamlRoot(xamlRoot, taskbarWnd.hWnd);
                } catch (winrt::hresult_error const& ex) {
                    Wh_Log(L"Apply failed: %s", ex.message().c_str());
                }
            }
        },
        &taskbarWnds);
}

// -----------------------------------------------------------------------------
// Module plumbing
// -----------------------------------------------------------------------------

std::atomic<bool> g_systemTrayModuleHooked;

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
            &IconView_IconView_Original,
            IconView_IconView_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

// Returns the module that hosts winrt::SystemTray::* in the current build.
// SystemTray.dll is the new home (Win11 26200+); Taskbar.View.dll is kept as a
// fallback for older builds.
HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // First known module version without SystemTray is Taskbar.View.dll
            // 2604.8002.200.6000.
            VS_FIXEDFILEINFO* fixedFileInfo =
                GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor =
                fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                Wh_Log(L"Skipping Taskbar.View.dll version %d", moduleMajor);
                module = nullptr;
            }
        }
    }

    return module;
}

void HandleLoadedModuleIfSystemTray(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == module &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfSystemTray(module, lpLibFileName);
    }

    return module;
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
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
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        if (!HookSystemTraySymbols(systemTrayModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"System tray module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        if (!kernelBaseModule) {
            Wh_Log(L"Failed to get kernelbase.dll");
            return FALSE;
        }

        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        if (!pKernelBaseLoadLibraryExW) {
            Wh_Log(L"Failed to get LoadLibraryExW");
            return FALSE;
        }

        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"Got system tray module");

                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    ApplySettings();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    ApplySettings();
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    ApplySettings();
}

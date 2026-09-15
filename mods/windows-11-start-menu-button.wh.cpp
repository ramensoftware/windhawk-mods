// ==WindhawkMod==
// @id              windows-11-start-menu-button
// @name            Windows 11 Start Button Tinter
// @description     Recolor (animation-preserving, with depth gradient and press-sweep) for the Windows 11 taskbar Start button
// @version         1.4
// @author          AristideBH
// @github          https://github.com/AristideBH
// @homepage        https://aristide-bh.com/
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 Start Button Tinter

> **Note:** This mod is vibe-coded - built largely with AI assistance and
> tested manually by the author, without a full independent code audit. Use
> at your own judgment, and please report anything odd via GitHub Issues.

![Silver recolor demo](https://raw.githubusercontent.com/AristideBH/windhawk-lib/main/windows-11-start-menu-button/demo/demo-recolor-silver.gif)

![Flat color + click shimmer demo](https://raw.githubusercontent.com/AristideBH/windhawk-lib/main/windows-11-start-menu-button/demo/demo-recolor-flat-shimmer.gif)

Customize the Windows 11 taskbar Start button's icon, without losing its
native hover/press animation:

- **System default** — leave the icon untouched.
- **Recolor** — tint the stock icon to any color, or follow the system
  accent color live, with two independent add-ons:
  - **Gradient shading** — a diagonal light-to-dark tint across the icon's
    four tiles (echoing the original flag icon's look), plus a brightness
    boost while hovered, pressed, or the Start menu is open.
  - **Click shimmer** — a one-shot highlight sweep that plays across the
    icon each time the Start button is clicked, either in a custom color
    or auto-lightened from the icon color.

Gradient shading and click shimmer can be toggled independently - shimmer
on a flat-colored icon, gradient shading with no shimmer, both, or neither.

Only Windows 11 25H2 is supported. Applies to the Start button on every
taskbar instance (multi-monitor).

## Requirements

- Windows 11 25H2, build 26200.9445 or later, 64-bit
- Windhawk v1.4 or later
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mode: default
  $name: Icon mode
  $description: >-
    What the taskbar's Start icon should look like. "System default" leaves
    it untouched. "Recolor" tints the built-in icon while keeping its
    native hover/press animation.
  $options:
  - default: System default
  - recolor: Recolor
- recolor:
  - useAccentColor: false
    $name: Use system accent color
    $description: >-
      Follow Windows' current accent color instead of Icon color below.
      Updates live when the accent color changes in Settings.
  - color: "#F2F2F2"
    $name: Icon color
    $description: >-
      Hex color (#RRGGBB or #AARRGGBB) for the icon at rest. Ignored when
      "Use system accent color" is on.
  - gradient:
    - enabled: true
      $name: Add gradient shading
      $description: >-
        Shade the icon with a diagonal light-to-dark gradient derived from
        Icon color, similar to the original icon's look, instead of one
        flat tone. Also enables the hover/press glow below. Independent of
        Click shimmer, which has its own toggle.
    - lightenPercent: 18
      $name: Light side strength
      $description: How much lighter the icon's light side is than Icon color.
    - darkenPercent: 15
      $name: Dark side strength
      $description: How much darker the icon's dark side is than Icon color.
    $name: Gradient shading
    $description: The diagonal light/dark shading applied to the icon.
  - shimmer:
    - enabled: true
      $name: Show click shimmer
      $description: >-
        Play a one-shot highlight sweep across the icon each time the Start
        button is clicked.
    - color: ""
      $name: Shimmer color
      $description: >-
        Hex color for the sweep highlight. Leave empty to auto-lighten Icon
        color instead (see "Auto shimmer lighten" below).
    - autoLightenPercent: 65
      $name: Auto shimmer lighten
      $description: >-
        Only used when Shimmer color is empty: how strongly to lighten Icon
        color for the sweep, like laying a white overlay over it at this
        opacity.
    - durationMs: 350
      $name: Sweep duration (ms)
      $description: How long the shimmer takes to sweep across the icon.
    - bandWidthPercent: 350
      $name: Sweep width
      $description: >-
        Width of the moving highlight band, as a percent of the icon's
        diagonal. Usable range is roughly 100 (a thin line) to 1000 (a
        broad glow); 500 is a good starting point.
    $name: Click shimmer
    $description: >-
      The one-shot highlight sweep played on click. Works whether or not
      Gradient shading is on.
  - elevate:
    - lightenBoostPercent: 50
      $name: Extra brightness
      $description: >-
        How much brighter the light side gets, as a percentage of the
        normal Light side strength above, while hovered, pressed, or the
        Start menu is open. 100% doubles it; it never overshoots white.
    - darkenReliefPercent: 100
      $name: Shadow reduction
      $description: >-
        How much the dark side's shading eases off, as a percentage of the
        normal Dark side strength above, while hovered, pressed, or the
        Start menu is open. 100% removes the darkening entirely.
    - transitionMs: 180
      $name: Fade speed (ms)
      $description: >-
        How long the brighten/dim transition takes when hover, press, or
        Start menu state changes.
    $name: Hover & press glow
    $description: >-
      The icon's brightness boost while hovered, pressed, or the Start menu
      is open. Only used when Gradient shading is on.
  $name: Recolor
  $description: Settings used when Icon mode is "Recolor".
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/base.h>

// The Start button's icon is a Lottie-based Microsoft.UI.Xaml.Controls.
// AnimatedVisualPlayer (WinUI 2/MUX), not a Windows.UI.Xaml IconElement.
#define WH_WINRT_WINUI2
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>

using namespace winrt::Windows::UI::Xaml;

// -----------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------

enum class IconMode {
    Default,
    Recolor,
};

struct {
    IconMode mode;
    bool recolorUseAccentColor;
    std::wstring recolorColor;
    std::wstring recolorShimmerColor;
    bool recolorGradient;
    // Gradient shading / hover glow / click shimmer tuning
    // (recolor.gradient.*, recolor.elevate.*, recolor.shimmer.*),
    // live-editable via settings so tuning "feel" doesn't need a recompile.
    float gradientLightenAmount;
    float gradientDarkenAmount;
    float elevatedLightenBoost;
    float elevatedDarkenRelief;
    float hoverTransitionMs;
    bool shimmerEnabled;
    float sweepDurationMs;
    float sweepBandWidth;
    float shimmerAutoAmount;
} g_settings;

// Bumped every time LoadSettings() runs, so the per-frame render loop can
// tell "settings just changed, must reapply at least once" apart from
// "still idle since the last applied frame, safe to skip" without diffing
// every individual field.
int g_settingsGeneration = 0;

// Parsed once in LoadSettings instead of re-parsing the hex string every
// render frame.
std::optional<winrt::Windows::UI::Color> g_cachedShimmerColor;

enum ButtonState {
    kStateDefault = 0,
    kStateHover = 1,
    kStatePressed = 2,
};

std::atomic<bool> g_unloading;
std::atomic<bool> g_taskbarViewDllLoaded;

// -----------------------------------------------------------------------
// Per-button tracked state (declared early: StartPersistentIconColorMaintenance,
// defined further down, looks up TrackedButton state every frame)
// -----------------------------------------------------------------------

struct TrackedButton {
    winrt::weak_ref<FrameworkElement> buttonRef;
    winrt::weak_ref<FrameworkElement> panelRef;
    ButtonState lastState = kStateDefault;
    bool menuOpen = false;  // spec state D: Start menu held open
    // Icon depth-gradient animation state (read/written from the
    // per-frame loop in StartPersistentIconColorMaintenance).
    float iconElevatedAmount = 0.0f;  // 0..1, ramped over g_settings.hoverTransitionMs
    ULONGLONG iconLastFrameTimeMs = 0;
    bool iconPressSweepActive = false;
    ULONGLONG iconPressSweepStartMs = 0;
    // Whether pointer/toggle handlers and the persistent render loop are
    // currently attached - only true while mode is Recolor. Lets settings
    // changes attach/detach them on an already-tracked button without
    // rediscovering it via the padding hook.
    bool recolorActive = false;
    // Last frame actually pushed to the Composition tree, so the render
    // loop (StartPersistentIconColorMaintenance) can skip the tree walk
    // and brush recreation entirely once the icon is sitting idle at rest
    // with nothing left to change - see the dirty-check at its call site.
    bool hasLastApplied = false;
    int lastAppliedSettingsGeneration = -1;
    float lastAppliedElevatedAmount = 0.0f;
    bool lastAppliedSweepActive = false;
    winrt::Windows::UI::Color lastAppliedBaseColor{};
    winrt::Windows::UI::Color lastAppliedBandColor{};
    winrt::event_token pointerEnteredToken;
    winrt::event_token pointerExitedToken;
    winrt::event_token pointerPressedToken;
    winrt::event_token pointerReleasedToken;
    winrt::event_token pointerCaptureLostToken;
    winrt::event_token checkedToken;
    winrt::event_token uncheckedToken;
    // Revoked, and the entry pruned from g_trackedButtons, when the button
    // itself unloads (taskbar recreated on a monitor/DPI change) - see the
    // Unloaded handler in EnsureTrackedButton.
    winrt::event_token unloadedToken;
    // Held so teardown can revoke the CompositionTarget.Rendering
    // subscription synchronously and unconditionally - see
    // StartPersistentIconColorMaintenance / StopIconColorMaintenance for
    // why relying on the render callback to notice g_unloading and
    // self-revoke on its own next invocation isn't safe.
    std::shared_ptr<winrt::event_token> renderingToken;
};

std::vector<TrackedButton> g_trackedButtons;

TrackedButton* FindTrackedButton(const FrameworkElement& button) {
    for (auto& tracked : g_trackedButtons) {
        auto elem = tracked.buttonRef.get();
        if (elem && elem == button) {
            return &tracked;
        }
    }
    return nullptr;
}

// -----------------------------------------------------------------------
// XAML tree helpers (pattern verified against public Windhawk taskbar mods)
// -----------------------------------------------------------------------

FrameworkElement EnumChildElements(
    FrameworkElement element,
    const std::function<bool(FrameworkElement)>& enumCallback) {
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

// Recursive descendant search (icon/border elements may be nested a few
// levels deep inside the button's root panel).
FrameworkElement FindDescendant(
    FrameworkElement element,
    const std::function<bool(FrameworkElement)>& predicate,
    int maxDepth = 6) {
    if (maxDepth <= 0) {
        return nullptr;
    }

    FrameworkElement result = nullptr;
    EnumChildElements(element, [&](FrameworkElement child) {
        if (predicate(child)) {
            result = child;
            return true;
        }
        auto nested = FindDescendant(child, predicate, maxDepth - 1);
        if (nested) {
            result = nested;
            return true;
        }
        return false;
    });
    return result;
}

FrameworkElement FindIconElement(FrameworkElement panel) {
    return FindDescendant(panel, [](FrameworkElement child) {
        auto className = winrt::get_class_name(child);
        return className == L"Windows.UI.Xaml.Controls.FontIcon" ||
               className == L"Windows.UI.Xaml.Controls.PathIcon" ||
               className == L"Windows.UI.Xaml.Controls.BitmapIcon" ||
               className == L"Windows.UI.Xaml.Controls.Image" ||
               className == L"Windows.UI.Xaml.Controls.AnimatedIcon" ||
               className == L"Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer";
    });
}

// The Start button icon ships as a Lottie-based AnimatedVisualPlayer
// rendering Windows' classic four-color flag logo (confirmed live: 28
// CompositionSpriteShape fill brushes with genuine distinct RGB values, not
// a single-tone icon). IAnimatedVisualSource2::SetColorProperty("Foreground",
// ...) - the theme-color mechanism used by Microsoft's own published
// LottieGen single-tone icon sources - was confirmed to have zero effect on
// this specific asset even after reasserting every frame for 2 seconds; the
// "Foreground" string found in Taskbar.View.dll evidently belongs to a
// different icon class in the same binary. Since there is no working theme
// knob for this asset, recolor is done by directly overwriting every
// CompositionSpriteShape's FillBrush/StrokeBrush color in its Lottie shape
// tree. Original colors are captured (keyed by brush COM identity) before
// the first override so Default mode can restore the real multi-color flag
// look instead of guessing a fallback color.
//
// Lottie/Lottie-Windows content is a vector shape tree, not sprite-visual
// rectangles: ShapeVisual.Shapes() -> CompositionContainerShape.Shapes()
// (recursively) -> CompositionSpriteShape.FillBrush()/StrokeBrush(). Also,
// AnimatedVisualPlayer hosts this tree as an "element child visual"
// (ElementCompositionPreview.SetElementChildVisual), a separate composition
// hook from the element's own XAML-owned visual, so both GetElementVisual
// and GetElementChildVisual must be checked.
// Mutating an existing brush's .Color() only works for a plain
// CompositionColorBrush - it silently no-ops (try_as fails) on a
// CompositionLinearGradientBrush/CompositionRadialGradientBrush, which is
// what the Start icon's flag squares actually use (confirmed: the resting
// squares are a blue *gradient*, unaffected by our .Color() writes, while
// a separate solid-brush shape - the hover/press highlight overlay - did
// visibly change, which is what read as "shimmer only while animating").
// Fix: replace the whole FillBrush/StrokeBrush/Visual.Brush with a freshly
// created solid CompositionColorBrush, which works regardless of the
// original brush's type. The original brush object (gradient or
// otherwise) is captured by shape/visual identity before the first
// replacement, so Default mode can restore the exact original - including
// the gradient - rather than guessing a fallback.
//
// Holds strong Composition refs, so it must never be destroyed off the
// taskbar UI thread - wrapped so the automatic static destructor at
// arbitrary-thread DLL unload / process shutdown is skipped; controlled
// teardown (TeardownAllTrackedButtonsProc) clears it explicitly instead.
[[clang::no_destroy]] std::optional<
    std::unordered_map<void*, winrt::Windows::UI::Composition::CompositionBrush>>
    g_originalIconBrushes{std::in_place};

template <typename T>
void* Identity(T const& obj) {
    return winrt::get_abi(obj);
}

// -----------------------------------------------------------------------
// Depth gradient: per-tile Relative-mode gradient (0..1 of that shape's own
// render box), oriented so its direction matches the icon's global
// top-left-to-bottom-right diagonal regardless of how that specific tile is
// rotated/mirrored.
//
// A live full-tree dump proved two things Absolute-mode
// absolute-position math (the previous approach) couldn't work around:
// 1. Two of the four tile shapes use a genuine 90d/-90d rotation matrix
//    (zero on the diagonal, e.g. tm=[0,-1,1,0]) - mathematically impossible
//    to represent as independent X/Y scale factors, which is what the
//    previous per-axis-scale-extraction approach required. Only the
//    identity and 180d-rotation tiles (diagonal-only matrices) were ever
//    representable that way.
// 2. The tile shapes' translations cluster within ~1 unit of the icon's
//    center - the real per-tile position/size lives in path geometry data
//    that Composition doesn't expose a bounds query for, so absolute
//    positioning can't be computed reliably at all.
//
// Relative mode sidesteps both: no position/size data needed, only each
// shape's own rotation (which TransformMatrix does give reliably). Trade-
// off: each tile shows its own full light->dark range rather than one
// gradient spanning the whole icon - not literally continuous across
// tiles, but consistently oriented.
// -----------------------------------------------------------------------

using Float2 = winrt::Windows::Foundation::Numerics::float2;

// Plain 2x2 matrix (row-vector convention matching Numerics.Matrix3x2:
// x' = x*m11 + y*m21, y' = x*m12 + y*m22) - only the rotation/scale part,
// no translation, since Relative mode never needs shape position.
struct Mat2 {
    float m11 = 1.0f, m12 = 0.0f, m21 = 0.0f, m22 = 1.0f;
};

// Composed such that applying the result to a point equals applying
// `parent` to the result of applying `child` to that point - i.e. `child`
// is this shape's own local transform, `parent` is everything above it.
Mat2 Mat2Compose(const Mat2& child, const Mat2& parent) {
    return {
        child.m11 * parent.m11 + child.m12 * parent.m21,
        child.m11 * parent.m12 + child.m12 * parent.m22,
        child.m21 * parent.m11 + child.m22 * parent.m21,
        child.m21 * parent.m12 + child.m22 * parent.m22,
    };
}

Mat2 Mat2Invert(const Mat2& m) {
    float det = m.m11 * m.m22 - m.m12 * m.m21;
    if (std::abs(det) < 1e-6f) {
        return Mat2{};  // degenerate (e.g. a collapsed/inactive Lottie
                         // animation-state layer with scale 0) - identity
                         // fallback, harmless since it's not rendered.
    }
    float invDet = 1.0f / det;
    return {m.m22 * invDet, -m.m12 * invDet, -m.m21 * invDet, m.m11 * invDet};
}

Float2 Mat2Apply(const Mat2& m, Float2 v) {
    return {v.x * m.m11 + v.y * m.m21, v.x * m.m12 + v.y * m.m22};
}

struct GradientFrameParams {
    winrt::Windows::UI::Color baseColor{};
    winrt::Windows::UI::Color bandColor{};
    float elevatedAmount = 0.0f;  // 0..1, ramped
    float sweepProgress = -1.0f;  // -1 == inactive, else raw progress 0..1
};

winrt::Windows::UI::Color LerpColor(winrt::Windows::UI::Color a,
                                     winrt::Windows::UI::Color b,
                                     float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    auto lerpByte = [t](BYTE x, BYTE y) {
        return (BYTE)std::round(x + (y - x) * t);
    };
    return winrt::Windows::UI::Color{lerpByte(a.A, b.A), lerpByte(a.R, b.R),
                                      lerpByte(a.G, b.G), lerpByte(a.B, b.B)};
}

// Tint/shade toward white/black (a "screen"/"multiply"-style RGB blend)
// instead of an HSL lightness add: raw HSL-L manipulation pushes saturated
// colors toward white/black fast once anything else (e.g. the elevate
// boost below) stacks another delta on top, since it's recomputing hue/
// saturation geometry rather than proportionally blending. Blending in RGB
// keeps the shift feeling like a subtle sheen rather than a wash-out.
winrt::Windows::UI::Color Tint(winrt::Windows::UI::Color color, float amount) {
    return LerpColor(color, winrt::Windows::UI::Color{255, 255, 255, 255},
                      amount);
}

winrt::Windows::UI::Color Shade(winrt::Windows::UI::Color color, float amount) {
    return LerpColor(color, winrt::Windows::UI::Color{255, 0, 0, 0}, amount);
}

bool ColorsEqual(winrt::Windows::UI::Color a, winrt::Windows::UI::Color b) {
    return a.A == b.A && a.R == b.R && a.G == b.G && a.B == b.B;
}

float EaseOut(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return 1.0f - (1.0f - t) * (1.0f - t);
}

winrt::Windows::UI::Composition::CompositionBrush CreateDepthGradientBrush(
    winrt::Windows::UI::Composition::Compositor compositor,
    const GradientFrameParams& params,
    const Mat2& shapeMatrix) {
    // Classify this tile's role from its rotation trace (m11+m22), so its
    // stop colors approximate its true position along the icon's overall
    // diagonal instead of every tile showing the same full light->dark
    // swing: the identity-like tile (trace ~+2) is top-left, the
    // 180d-rotated tile (trace ~-2) is bottom-right - confirmed live,
    // darken only ever visibly showed up on the tile whose matrix was
    // [-1,0,0,-1] - and the two 90d/-90d tiles (trace ~0, off-diagonal)
    // are the anti-diagonal pair, sitting near the middle of the overall
    // gradient with a narrower swing. Also used below to stagger the
    // click-sweep timing per tile, independent of whether gradient shading
    // itself is on.
    float trace = shapeMatrix.m11 + shapeMatrix.m22;

    // Gradient shading and click shimmer are independent settings (an icon
    // can shimmer on click while staying flat-shaded at rest, and vice
    // versa) - this function is reused for both since the shimmer band
    // needs the same per-tile role/stagger logic either way. When gradient
    // shading is off, collapse to a flat color and let the band sweep (if
    // any) be the only thing that varies the fill.
    winrt::Windows::UI::Color colorStart, colorEnd;
    if (!g_settings.recolorGradient) {
        colorStart = colorEnd = params.baseColor;
    } else {
        // Elevate boost/relief scale the base amount rather than adding an
        // independent absolute delta - "50% brighter" means the lighten
        // amount grows by up to 50% of itself at full elevation, whatever
        // that base amount is, instead of always adding the same fixed
        // chunk of the whole white-black range regardless of how subtle
        // the base gradient was set to. Tint/Shade below also clamp their
        // blend amount to [0,1], so this can't overshoot even at extreme
        // settings.
        float lightenDelta = g_settings.gradientLightenAmount *
                              (1.0f + g_settings.elevatedLightenBoost *
                                          params.elevatedAmount);
        float darkenDelta = g_settings.gradientDarkenAmount *
                             (1.0f - g_settings.elevatedDarkenRelief *
                                         params.elevatedAmount);
        darkenDelta = std::max(0.0f, darkenDelta);
        if (trace > 1.0f) {
            colorStart = Tint(params.baseColor, lightenDelta);
            colorEnd = params.baseColor;
        } else if (trace < -1.0f) {
            colorStart = params.baseColor;
            colorEnd = Shade(params.baseColor, darkenDelta);
        } else {
            colorStart = Tint(params.baseColor, lightenDelta * 0.25f);
            colorEnd = Shade(params.baseColor, darkenDelta * 0.25f);
        }
    }

    // Color at an arbitrary offset along this tile's own (non-sweep)
    // gradient - used to sandwich the sweep band so it doesn't disturb
    // the rest of the gradient.
    auto colorAtOffset = [&](float t) {
        return LerpColor(colorStart, colorEnd, t);
    };

    struct Stop {
        float offset;
        winrt::Windows::UI::Color color;
    };
    std::vector<Stop> stops = {{0.0f, colorStart}, {1.0f, colorEnd}};

    // Stagger *when* each tile's own sweep plays, using the same role
    // classification as the base gradient colors above, so the 4
    // independent per-tile sweeps read as one diagonal wave passing
    // top-left -> anti-diagonal -> bottom-right instead of all 4 tiles
    // flashing in perfect unison. Windows overlap (0.6/0.8/1.0 span vs.
    // 0.6-wide windows) so the wave reads as continuous rather than 3
    // disjoint pops. No real position data needed - just the same 3-way
    // role split already used above.
    float roleWindowStart, roleWindowEnd;
    if (trace > 1.0f) {
        roleWindowStart = 0.0f;
        roleWindowEnd = 0.6f;
    } else if (trace < -1.0f) {
        roleWindowStart = 0.4f;
        roleWindowEnd = 1.0f;
    } else {
        roleWindowStart = 0.2f;
        roleWindowEnd = 0.8f;
    }
    float localSweepProgress = -1.0f;
    if (params.sweepProgress >= roleWindowStart &&
        params.sweepProgress <= roleWindowEnd) {
        localSweepProgress = (params.sweepProgress - roleWindowStart) /
                              (roleWindowEnd - roleWindowStart);
    }

    if (localSweepProgress >= 0.0f) {
        float eased = EaseOut(localSweepProgress);
        float bandWidth = g_settings.sweepBandWidth;
        float bandCenter = -bandWidth / 2.0f + eased * (1.0f + bandWidth);
        float bandStart = bandCenter - bandWidth / 2.0f;
        float bandEnd = bandCenter + bandWidth / 2.0f;

        if (bandEnd > 0.0f && bandStart < 1.0f) {
            float clampedStart = std::clamp(bandStart, 0.0f, 1.0f);
            float clampedCenter = std::clamp(bandCenter, 0.0f, 1.0f);
            float clampedEnd = std::clamp(bandEnd, 0.0f, 1.0f);
            stops.push_back({clampedStart, colorAtOffset(clampedStart)});
            stops.push_back({clampedCenter, params.bandColor});
            stops.push_back({clampedEnd, colorAtOffset(clampedEnd)});
        }
    }

    std::sort(stops.begin(), stops.end(),
              [](const Stop& a, const Stop& b) { return a.offset < b.offset; });

    auto gradientBrush = compositor.CreateLinearGradientBrush();
    gradientBrush.MappingMode(
        winrt::Windows::UI::Composition::CompositionMappingMode::Relative);

    // Map the global diagonal direction (1,1) through the INVERSE of this
    // shape's accumulated rotation, so the gradient - drawn in this
    // shape's own local/pre-rotation space - ends up pointing along the
    // true global diagonal once the shape's own transform is applied to
    // render it. For an identity shape this is just (1,1) -> corner to
    // corner, same as before.
    Float2 localDir = Mat2Apply(Mat2Invert(shapeMatrix), Float2{1.0f, 1.0f});
    gradientBrush.StartPoint({0.5f - localDir.x * 0.5f, 0.5f - localDir.y * 0.5f});
    gradientBrush.EndPoint({0.5f + localDir.x * 0.5f, 0.5f + localDir.y * 0.5f});

    auto colorStops = gradientBrush.ColorStops();
    for (auto& stop : stops) {
        colorStops.Append(
            compositor.CreateColorGradientStop(stop.offset, stop.color));
    }

    return gradientBrush;
}

winrt::Windows::UI::Composition::CompositionBrush CreateRecolorBrush(
    winrt::Windows::UI::Composition::Compositor compositor,
    const GradientFrameParams& params,
    const Mat2& shapeMatrix) {
    // A sweep is only actually in progress if shimmer is enabled (the
    // per-frame loop forces params.sweepProgress to -1 otherwise - see
    // StartPersistentIconColorMaintenance), so this alone decides whether
    // the (pricier) gradient-brush path is needed for its sake.
    if (g_settings.recolorGradient || params.sweepProgress >= 0.0f) {
        return CreateDepthGradientBrush(compositor, params, shapeMatrix);
    }
    return compositor.CreateColorBrush(params.baseColor);
}

int ReplaceBrush(
    winrt::Windows::UI::Composition::Compositor compositor,
    void* identity,
    winrt::Windows::UI::Composition::CompositionBrush currentBrush,
    const std::optional<GradientFrameParams>& frameParams,
    const Mat2& shapeMatrix,
    std::function<void(winrt::Windows::UI::Composition::CompositionBrush)> setter) {
    if (!compositor) {
        return 0;
    }

    if (frameParams) {
        if (currentBrush) {
            g_originalIconBrushes->try_emplace(identity, currentBrush);
        }
        setter(CreateRecolorBrush(compositor, *frameParams, shapeMatrix));
    } else {
        auto it = g_originalIconBrushes->find(identity);
        if (it != g_originalIconBrushes->end()) {
            setter(it->second);
        }
    }

    return 1;
}

int RecolorShapeBrushes(winrt::Windows::UI::Composition::CompositionShape shape,
                         const std::optional<GradientFrameParams>& frameParams,
                         const Mat2& accMatrix,
                         int depth = 0) {
    if (!shape || depth > 12) {
        return 0;
    }

    int count = 0;

    // Only TransformMatrix's own 2x2 linear part is used - NOT
    // shape.Scale(), which a live full-tree dump showed reads back as
    // (0,0) on many shapes that clearly aren't meant to be zero-scaled
    // (e.g. plain container shapes whose TransformMatrix is genuine
    // identity). TransformMatrix reliably reads real identity {1,0,0,1}
    // when unused and the real rotation/reflection when used, so it alone
    // is the trustworthy source here.
    auto tm = shape.TransformMatrix();
    Mat2 shapeMatrix{tm.m11, tm.m12, tm.m21, tm.m22};
    Mat2 totalMatrix = Mat2Compose(shapeMatrix, accMatrix);

    if (auto spriteShape =
            shape.try_as<winrt::Windows::UI::Composition::CompositionSpriteShape>()) {
        auto compositor = spriteShape.Compositor();

        if (spriteShape.FillBrush()) {
            count += ReplaceBrush(
                compositor, Identity(spriteShape), spriteShape.FillBrush(),
                frameParams, totalMatrix, [spriteShape](auto brush) {
                    spriteShape.FillBrush(brush);
                });
        }
        if (spriteShape.StrokeBrush()) {
            // Distinct identity key from the fill (same shape, different
            // brush slot) - offset the raw shape pointer by 1 byte.
            void* strokeIdentity =
                reinterpret_cast<char*>(Identity(spriteShape)) + 1;
            count += ReplaceBrush(
                compositor, strokeIdentity, spriteShape.StrokeBrush(),
                frameParams, totalMatrix, [spriteShape](auto brush) {
                    spriteShape.StrokeBrush(brush);
                });
        }
    }

    if (auto containerShape =
            shape.try_as<winrt::Windows::UI::Composition::CompositionContainerShape>()) {
        for (auto child : containerShape.Shapes()) {
            count += RecolorShapeBrushes(child, frameParams, totalMatrix,
                                          depth + 1);
        }
    }

    return count;
}

int RecolorVisualBrushes(winrt::Windows::UI::Composition::Visual visual,
                          const std::optional<GradientFrameParams>& frameParams,
                          const Mat2& accMatrix,
                          int depth = 0) {
    if (!visual || depth > 12) {
        return 0;
    }

    int count = 0;

    // Visual-level rotation isn't in evidence in this asset (a full dump
    // showed every ShapeVisual/ContainerVisual at identity offset/scale) -
    // fold in just the XY scale diagonal for completeness, ignore Z/
    // rotation/TransformMatrix at this level.
    auto visualScale = visual.Scale();
    Mat2 visualMatrix{visualScale.x, 0.0f, 0.0f, visualScale.y};
    Mat2 totalMatrix = Mat2Compose(visualMatrix, accMatrix);

    if (auto spriteVisual =
            visual.try_as<winrt::Windows::UI::Composition::SpriteVisual>()) {
        if (spriteVisual.Brush()) {
            count += ReplaceBrush(
                spriteVisual.Compositor(), Identity(spriteVisual),
                spriteVisual.Brush(), frameParams, totalMatrix,
                [spriteVisual](auto brush) { spriteVisual.Brush(brush); });
        }
    }

    if (auto shapeVisual =
            visual.try_as<winrt::Windows::UI::Composition::ShapeVisual>()) {
        for (auto shape : shapeVisual.Shapes()) {
            count += RecolorShapeBrushes(shape, frameParams, totalMatrix,
                                          depth + 1);
        }
    }

    if (auto container =
            visual.try_as<winrt::Windows::UI::Composition::ContainerVisual>()) {
        for (auto child : container.Children()) {
            count += RecolorVisualBrushes(child, frameParams, totalMatrix,
                                           depth + 1);
        }
    }

    return count;
}

int RecolorAnimatedVisualPlayer(
    winrt::Microsoft::UI::Xaml::Controls::AnimatedVisualPlayer player,
    const std::optional<GradientFrameParams>& frameParams) {
    int found = 0;
    Mat2 rootMatrix{};
    if (auto ownVisual =
            Hosting::ElementCompositionPreview::GetElementVisual(player)) {
        found += RecolorVisualBrushes(ownVisual, frameParams, rootMatrix);
    }
    if (auto childVisual =
            Hosting::ElementCompositionPreview::GetElementChildVisual(player)) {
        found += RecolorVisualBrushes(childVisual, frameParams, rootMatrix);
    }
    return found;
}

// -----------------------------------------------------------------------
// Color / settings parsing
// -----------------------------------------------------------------------

std::optional<winrt::Windows::UI::Color> ParseHexColor(std::wstring hex) {
    if (hex.empty()) {
        return std::nullopt;
    }
    if (hex[0] == L'#') {
        hex = hex.substr(1);
    }

    for (wchar_t c : hex) {
        if (!iswxdigit(c)) {
            return std::nullopt;
        }
    }

    BYTE a = 0xFF, r, g, b;
    if (hex.size() == 6) {
        r = (BYTE)wcstoul(hex.substr(0, 2).c_str(), nullptr, 16);
        g = (BYTE)wcstoul(hex.substr(2, 2).c_str(), nullptr, 16);
        b = (BYTE)wcstoul(hex.substr(4, 2).c_str(), nullptr, 16);
    } else if (hex.size() == 8) {
        a = (BYTE)wcstoul(hex.substr(0, 2).c_str(), nullptr, 16);
        r = (BYTE)wcstoul(hex.substr(2, 2).c_str(), nullptr, 16);
        g = (BYTE)wcstoul(hex.substr(4, 2).c_str(), nullptr, 16);
        b = (BYTE)wcstoul(hex.substr(6, 2).c_str(), nullptr, 16);
    } else {
        return std::nullopt;
    }

    return winrt::Windows::UI::Color{a, r, g, b};
}

// UISettings is activated once (not per-frame - COM activation is not
// cheap) and its accent color read once, then kept fresh via
// ColorValuesChanged instead of being polled every render tick.
// ColorValuesChanged can fire off the UI thread; g_cachedAccentColor is a
// plain 4-byte Color, so the resulting race is the same low-risk class
// already accepted elsewhere for plain settings fields, not a new one.
winrt::Windows::UI::ViewManagement::UISettings g_uiSettings{nullptr};
winrt::event_token g_uiSettingsColorChangedToken{};
bool g_uiSettingsSubscribed = false;
winrt::Windows::UI::Color g_cachedAccentColor{};

void EnsureAccentColorTracking() {
    if (g_uiSettingsSubscribed) {
        return;
    }
    try {
        g_uiSettings = winrt::Windows::UI::ViewManagement::UISettings();
        g_cachedAccentColor = g_uiSettings.GetColorValue(
            winrt::Windows::UI::ViewManagement::UIColorType::Accent);
        g_uiSettingsColorChangedToken = g_uiSettings.ColorValuesChanged(
            [](auto&&, auto&&) {
                try {
                    g_cachedAccentColor = g_uiSettings.GetColorValue(
                        winrt::Windows::UI::ViewManagement::UIColorType::Accent);
                } catch (...) {
                }
            });
        g_uiSettingsSubscribed = true;
    } catch (...) {
        g_uiSettings = nullptr;
        g_uiSettingsSubscribed = false;
    }
}

// Teardown counterpart to EnsureAccentColorTracking - must run on the
// taskbar UI thread, same as everything else ColorValuesChanged touches.
void StopAccentColorTracking() {
    if (!g_uiSettingsSubscribed) {
        return;
    }
    try {
        g_uiSettings.ColorValuesChanged(g_uiSettingsColorChangedToken);
    } catch (...) {
    }
    g_uiSettings = nullptr;
    g_uiSettingsSubscribed = false;
}

// Resolves the recolor mode's resting icon color: the system accent color
// when "Use system accent color" is on, otherwise the manual hex field.
std::optional<winrt::Windows::UI::Color> GetRecolorBaseColor() {
    if (g_settings.recolorUseAccentColor) {
        EnsureAccentColorTracking();
        return g_uiSettingsSubscribed ? std::optional(g_cachedAccentColor)
                                       : std::nullopt;
    }
    return ParseHexColor(g_settings.recolorColor);
}

// Empirically (live on 25H2): a one-shot or briefly-retried brush override
// gets fought/reset by the icon's own native hover/press Lottie animation
// playback - confirmed by the override only visibly appearing WHILE that
// animation is actively playing, then reverting once it settles at rest.
// So this still reasserts every frame for the button's entire lifetime
// (only stopped on unload) - but *which* color/gradient it reasserts is
// now driven by our own tracked interaction state (idle/hover/pressed,
// see TrackedButton::lastState, plus TrackedButton::menuOpen), not
// player.IsPlaying(). IsPlaying() was only ever a proxy for "Explorer's
// animation is fighting us right now" - it can't express hover-brighten
// vs. a one-shot press sweep vs. a sustained menu-open hold, which are
// genuinely different states, not different phases of the same animation.
// Stops a button's persistent recolor loop and restores its Composition
// tree's original brushes - called explicitly and synchronously from
// Wh_ModBeforeUninit for every tracked button, not left to chance. Relying
// on the render callback itself to notice g_unloading and self-revoke on
// its own next invocation is NOT safe: CompositionTarget.Rendering stops
// firing once nothing needs to redraw (confirmed live - the icon sitting
// idle at rest is exactly the state that preceded a real Explorer crash,
// event ID 1000, exception 0x20474343 in KERNELBASE.dll, a Control Flow
// Guard fail-fast). If no further frame fires before Windhawk unloads this
// mod's DLL, the still-registered callback's code lives in now-unmapped
// memory - the next time the compositor invokes it, indirect-calling into
// freed memory is exactly what CFG fail-fasts on. Revoking synchronously
// here, before Wh_ModUninit/DLL unload can happen, closes that window
// entirely rather than hoping another frame ticks in time.
void StopIconColorMaintenance(TrackedButton& tracked) {
    if (!tracked.renderingToken) {
        return;
    }

    if (auto panel = tracked.panelRef.get()) {
        if (auto iconElement = FindIconElement(panel)) {
            if (auto player =
                    iconElement.try_as<
                        winrt::Microsoft::UI::Xaml::Controls::AnimatedVisualPlayer>()) {
                // RestoreIconMode only handles the IconElement's own
                // Visibility/Foreground - it doesn't touch the
                // AnimatedVisualPlayer's Composition shape-tree brushes,
                // which is what this loop has been overriding every frame.
                // One last restore call here, matching what the removed
                // per-frame g_unloading branch used to attempt on a "next
                // frame" that wasn't guaranteed to come.
                RecolorAnimatedVisualPlayer(player, std::nullopt);
            }
        }
    }

    try {
        Media::CompositionTarget::Rendering(*tracked.renderingToken);
    } catch (winrt::hresult_error const&) {
        // Already revoked, or otherwise invalid - nothing left to do.
    }
    tracked.renderingToken.reset();
}

void StartPersistentIconColorMaintenance(
    winrt::Microsoft::UI::Xaml::Controls::AnimatedVisualPlayer player,
    FrameworkElement button) {
    auto token = std::make_shared<winrt::event_token>();
    *token = Media::CompositionTarget::Rendering(
        [player, button, token](
            winrt::Windows::Foundation::IInspectable const&,
            winrt::Windows::Foundation::IInspectable const&) {
          try {
            // Unloading is handled explicitly and synchronously by
            // StopIconColorMaintenance (called from Wh_ModBeforeUninit),
            // which revokes this callback outright - so this is just a
            // defensive early-out for the vanishingly unlikely case of a
            // frame landing in the same tick g_unloading flips, before
            // that revoke runs. No brush restoration or self-revoke here
            // (both are StopIconColorMaintenance's job now).
            if (g_unloading) {
                return;
            }

            auto tracked = FindTrackedButton(button);

            if (g_settings.mode != IconMode::Recolor) {
                // Once the original brushes are restored for the current
                // settings generation, every further idle frame would just
                // re-set the same brushes it already restored - walking the
                // whole Composition shape tree for nothing. Only re-run
                // when something (mode, or any other setting) actually
                // changed since the last restore.
                if (tracked && tracked->hasLastApplied &&
                    tracked->lastAppliedSettingsGeneration ==
                        g_settingsGeneration) {
                    return;
                }
                RecolorAnimatedVisualPlayer(player, std::nullopt);
                if (tracked) {
                    tracked->hasLastApplied = true;
                    tracked->lastAppliedSettingsGeneration = g_settingsGeneration;
                }
                return;
            }

            auto baseColor = GetRecolorBaseColor();
            if (!tracked || !baseColor) {
                RecolorAnimatedVisualPlayer(player, std::nullopt);
                return;
            }

            ULONGLONG now = GetTickCount64();
            if (tracked->iconLastFrameTimeMs == 0) {
                tracked->iconLastFrameTimeMs = now;
            }
            float deltaMs = (float)(now - tracked->iconLastFrameTimeMs);
            tracked->iconLastFrameTimeMs = now;

            // "Elevated" = hover, pressed, or menu held open - all three
            // share the same brighten treatment per spec (state D reuses
            // hover's "elevated brightness tone").
            bool elevated =
                tracked->lastState != kStateDefault || tracked->menuOpen;
            float target = elevated ? 1.0f : 0.0f;
            float rampStep = deltaMs / g_settings.hoverTransitionMs;
            if (tracked->iconElevatedAmount < target) {
                tracked->iconElevatedAmount =
                    std::min(target, tracked->iconElevatedAmount + rampStep);
            } else if (tracked->iconElevatedAmount > target) {
                tracked->iconElevatedAmount =
                    std::max(target, tracked->iconElevatedAmount - rampStep);
            }

            // One-shot sweep: triggered directly from the PointerPressed
            // handler (see AttachRecolorHandlers), not inferred here by
            // polling tracked->lastState - a fast click's Pressed->Hover
            // round trip can complete entirely between two Rendering ticks
            // (both are dispatched synchronously on the UI thread as part
            // of input processing, well under one 16ms composition frame),
            // so polling would miss it outright rather than just catching
            // it late. This loop's job is just to run the countdown once
            // triggered, then clear itself.
            float sweepProgress = -1.0f;
            if (!g_settings.shimmerEnabled) {
                // Disabled: whatever armed it (click handlers arm
                // unconditionally, cheaper than checking the setting in
                // every one of them) just gets cleared here instead of
                // ever counting down.
                tracked->iconPressSweepActive = false;
            } else if (tracked->iconPressSweepActive) {
                float elapsed = (float)(now - tracked->iconPressSweepStartMs);
                if (elapsed >= g_settings.sweepDurationMs) {
                    tracked->iconPressSweepActive = false;
                } else {
                    sweepProgress = elapsed / g_settings.sweepDurationMs;
                }
            }

            // Band tint: explicit override if configured, otherwise a
            // white overlay blended over the resting color - equivalent to
            // compositing a translucent white (e.g. #4Cffffff) on top of
            // it - so it always reads as a highlight regardless of how
            // light or dark the resting color already is.
            auto& shimmerColor = g_cachedShimmerColor;
            winrt::Windows::UI::Color bandColor =
                shimmerColor
                    ? *shimmerColor
                    : LerpColor(*baseColor,
                                winrt::Windows::UI::Color{255, 255, 255, 255},
                                g_settings.shimmerAutoAmount);

            GradientFrameParams params;
            params.baseColor = *baseColor;
            params.bandColor = bandColor;
            params.elevatedAmount = tracked->iconElevatedAmount;
            params.sweepProgress = sweepProgress;

            // Idle dirty-check: once elevatedAmount has finished ramping to
            // its target and no sweep is running, this frame would produce
            // pixel-identical brushes to the last one actually applied -
            // walking the shape tree and allocating a fresh gradient brush
            // per shape again is pure waste while the icon just sits at
            // rest (the overwhelming majority of frames). Anything that
            // could change the outcome (a settings edit, ramp still in
            // progress, a sweep starting/running, or the accent color
            // changing live) invalidates one of these checks and forces a
            // real reapply.
            bool sweepActiveNow = sweepProgress >= 0.0f;
            if (tracked->hasLastApplied &&
                tracked->lastAppliedSettingsGeneration == g_settingsGeneration &&
                !sweepActiveNow && !tracked->lastAppliedSweepActive &&
                tracked->lastAppliedElevatedAmount == tracked->iconElevatedAmount &&
                ColorsEqual(tracked->lastAppliedBaseColor, *baseColor) &&
                ColorsEqual(tracked->lastAppliedBandColor, bandColor)) {
                return;
            }

            RecolorAnimatedVisualPlayer(player, params);
            tracked->hasLastApplied = true;
            tracked->lastAppliedSettingsGeneration = g_settingsGeneration;
            tracked->lastAppliedElevatedAmount = tracked->iconElevatedAmount;
            tracked->lastAppliedSweepActive = sweepActiveNow;
            tracked->lastAppliedBaseColor = *baseColor;
            tracked->lastAppliedBandColor = bandColor;
          } catch (winrt::hresult_error const& ex) {
            // Never let an exception escape this native callback - one bad
            // WinRT call here must not be able to take Explorer down with
            // it. Log and skip the frame; the next tick tries again.
            Wh_Log(L"Icon color maintenance: caught hresult_error 0x%08x: %s",
                   (unsigned int)ex.code().value, ex.message().c_str());
          } catch (...) {
            Wh_Log(L"Icon color maintenance: caught unknown exception");
          }
        });

    // Stashed so Wh_ModBeforeUninit can revoke this subscription
    // synchronously via StopIconColorMaintenance - see its comment for why
    // that's necessary rather than relying on this callback noticing
    // g_unloading on its own next invocation.
    if (auto tracked = FindTrackedButton(button)) {
        tracked->renderingToken = token;
    }
}

// -----------------------------------------------------------------------
// Icon mode application (recolor foreground fallback for the plain
// IconElement path). Not per-state - mode/recolorColor don't vary with
// hover/press, so this only needs to run on setup and on settings change.
// -----------------------------------------------------------------------

void ApplyIconMode(FrameworkElement panel) {
    auto iconElement = FindIconElement(panel);
    if (!iconElement) {
        return;
    }

    iconElement.Visibility(Visibility::Visible);

    // AnimatedVisualPlayer (the Start icon's actual type) is NOT handled
    // here: its Lottie shape-brush colors get fought/reset by native
    // hover/press animation playback, so it needs continuous per-frame
    // enforcement rather than a one-shot property set - see
    // StartPersistentIconColorMaintenance, run once per button for its
    // whole lifetime.
    if (auto iconElementAsIcon = iconElement.try_as<Controls::IconElement>()) {
        if (g_settings.mode == IconMode::Recolor) {
            if (auto color = GetRecolorBaseColor()) {
                iconElementAsIcon.Foreground(Media::SolidColorBrush{*color});
            }
        } else {
            iconElementAsIcon.ClearValue(
                Controls::IconElement::ForegroundProperty());
        }
    }
}

// Undo whatever ApplyIconMode did, so the button is left showing the
// stock native icon.
void RestoreIconMode(FrameworkElement panel) {
    auto iconElement = FindIconElement(panel);
    if (iconElement) {
        iconElement.Visibility(Visibility::Visible);
        if (auto icon = iconElement.try_as<Controls::IconElement>()) {
            icon.ClearValue(Controls::IconElement::ForegroundProperty());
        }
    }
}

// -----------------------------------------------------------------------
// Recolor-mode handler attach/detach. Split from tracking discovery so a
// mode toggle in settings can add/remove these on an already-discovered
// button, instead of leaving them (and the per-frame render loop) running
// unconditionally, including in "System default" mode - see
// AttachRecolorHandlers's TrackedButton::recolorActive guard.
// -----------------------------------------------------------------------

void AttachRecolorHandlers(TrackedButton& tracked,
                            FrameworkElement button,
                            FrameworkElement panel) {
    if (tracked.recolorActive) {
        return;
    }
    tracked.recolorActive = true;

    auto setState = [](FrameworkElement button, ButtonState state) {
        if (auto t = FindTrackedButton(button)) {
            t->lastState = state;
        }
    };

    tracked.pointerEnteredToken = button.PointerEntered(
        [setState](winrt::Windows::Foundation::IInspectable const& sender,
                    auto const&) {
            setState(sender.try_as<FrameworkElement>(), kStateHover);
        });
    tracked.pointerExitedToken = button.PointerExited(
        [setState](winrt::Windows::Foundation::IInspectable const& sender,
                    auto const&) {
            setState(sender.try_as<FrameworkElement>(), kStateDefault);
        });
    tracked.pointerPressedToken = button.PointerPressed(
        [setState](winrt::Windows::Foundation::IInspectable const& sender,
                    auto const&) {
            auto pressedButton = sender.try_as<FrameworkElement>();
            setState(pressedButton, kStatePressed);
            // Trigger the sweep here, not by polling lastState in the
            // render loop - see the comment above sweepProgress in
            // StartPersistentIconColorMaintenance for why polling misses
            // fast clicks entirely.
            if (auto t = FindTrackedButton(pressedButton)) {
                t->iconPressSweepActive = true;
                t->iconPressSweepStartMs = GetTickCount64();
            }
        });
    tracked.pointerReleasedToken = button.PointerReleased(
        [setState](winrt::Windows::Foundation::IInspectable const& sender,
                    auto const&) {
            setState(sender.try_as<FrameworkElement>(), kStateHover);
        });
    tracked.pointerCaptureLostToken = button.PointerCaptureLost(
        [setState](winrt::Windows::Foundation::IInspectable const& sender,
                    auto const&) {
            setState(sender.try_as<FrameworkElement>(), kStateDefault);
        });

    // Spec state D (menu held open) - unverified guess that
    // Taskbar.ExperienceToggleButton derives from Primitives.ToggleButton
    // (consistent with the "ToggleButton" in its class name), same
    // try-and-log-if-it-fails pattern as the rest of this mod. If the cast
    // fails, state D simply never triggers; everything else still works.
    //
    // Confirmed live: PointerPressed NEVER fires on this element for a
    // real Start-button click (logged Entered -> Checked -> CaptureLost,
    // no Pressed in between) - the click is handled some other way before
    // it reaches this FrameworkElement's routed pointer events. Checked/
    // Unchecked are the only reliably-firing "a click just happened"
    // signal available here, so the press sweep is armed from both of
    // them (every click, whether it opens or closes the menu, counts as
    // a "press" for sweep purposes) - the PointerPressed arming above is
    // kept as a harmless no-op fallback in case a future build does route
    // it through normally.
    if (auto toggleButton = button.try_as<Controls::Primitives::ToggleButton>()) {
        tracked.checkedToken = toggleButton.Checked(
            [](winrt::Windows::Foundation::IInspectable const& sender,
               auto const&) {
                if (auto t = FindTrackedButton(sender.try_as<FrameworkElement>())) {
                    t->menuOpen = true;
                    t->iconPressSweepActive = true;
                    t->iconPressSweepStartMs = GetTickCount64();
                }
            });
        tracked.uncheckedToken = toggleButton.Unchecked(
            [](winrt::Windows::Foundation::IInspectable const& sender,
               auto const&) {
                if (auto t = FindTrackedButton(sender.try_as<FrameworkElement>())) {
                    t->menuOpen = false;
                    t->iconPressSweepActive = true;
                    t->iconPressSweepStartMs = GetTickCount64();
                }
            });
    }

    if (auto iconElement = FindIconElement(panel)) {
        if (auto player =
                iconElement
                    .try_as<winrt::Microsoft::UI::Xaml::Controls::AnimatedVisualPlayer>()) {
            StartPersistentIconColorMaintenance(player, button);
        }
    }
}

void DetachRecolorHandlers(TrackedButton& tracked, FrameworkElement button) {
    if (!tracked.recolorActive) {
        return;
    }
    tracked.recolorActive = false;

    // Revokes the render subscription and restores the original brushes.
    StopIconColorMaintenance(tracked);

    if (!button) {
        return;
    }
    auto revoke = [](auto&& fn) {
        try {
            fn();
        } catch (winrt::hresult_error const&) {
        }
    };
    revoke([&] { button.PointerEntered(tracked.pointerEnteredToken); });
    revoke([&] { button.PointerExited(tracked.pointerExitedToken); });
    revoke([&] { button.PointerPressed(tracked.pointerPressedToken); });
    revoke([&] { button.PointerReleased(tracked.pointerReleasedToken); });
    revoke([&] { button.PointerCaptureLost(tracked.pointerCaptureLostToken); });
    if (auto toggleButton = button.try_as<Controls::Primitives::ToggleButton>()) {
        revoke([&] { toggleButton.Checked(tracked.checkedToken); });
        revoke([&] { toggleButton.Unchecked(tracked.uncheckedToken); });
    }
}

// -----------------------------------------------------------------------
// Tracking discovery - called from the UpdateButtonPadding hook whenever a
// Start button panel is found. Creates the TrackedButton entry (once) and
// prunes it via Unloaded when the button goes away (taskbar recreated on
// a monitor/DPI change) - independent of AttachRecolorHandlers, which
// only fires the pointer/toggle/render subscriptions while mode is
// Recolor.
// -----------------------------------------------------------------------

void EnsureTrackedButton(FrameworkElement button, FrameworkElement panel) {
    TrackedButton* tracked = FindTrackedButton(button);
    if (!tracked) {
        Wh_Log(L"Setting up tracking for new button instance");
        g_trackedButtons.push_back({});
        tracked = &g_trackedButtons.back();
        tracked->buttonRef = button;
        tracked->panelRef = panel;
        tracked->unloadedToken = button.Unloaded(
            [](winrt::Windows::Foundation::IInspectable const& sender,
               auto const&) {
                auto elem = sender.try_as<FrameworkElement>();
                if (!elem) {
                    return;
                }
                for (auto it = g_trackedButtons.begin();
                     it != g_trackedButtons.end(); ++it) {
                    auto btn = it->buttonRef.get();
                    if (btn && btn == elem) {
                        DetachRecolorHandlers(*it, btn);
                        g_trackedButtons.erase(it);
                        break;
                    }
                }
            });
    }

    ApplyIconMode(panel);
    if (g_settings.mode == IconMode::Recolor) {
        AttachRecolorHandlers(*tracked, button, panel);
    }
}

// -----------------------------------------------------------------------
// Hook: ExperienceToggleButton::UpdateButtonPadding
// (symbol verified against public "Start button always on the left" mod,
//  ramensoftware/windhawk-mods, which uses the same hook point to reach
//  the Start button's "ExperienceToggleButtonRootPanel".)
// -----------------------------------------------------------------------

using ExperienceToggleButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
ExperienceToggleButton_UpdateButtonPadding_t
    ExperienceToggleButton_UpdateButtonPadding_Original;
void WINAPI ExperienceToggleButton_UpdateButtonPadding_Hook(void* pThis) {
    Wh_Log(L"UpdateButtonPadding hook fired, pThis=%p", pThis);

    ExperienceToggleButton_UpdateButtonPadding_Original(pThis);

    if (g_unloading) {
        Wh_Log(L"Unloading, skipping");
        return;
    }

    FrameworkElement toggleButtonElement = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                            winrt::put_abi(toggleButtonElement));
    if (!toggleButtonElement) {
        Wh_Log(L"QueryInterface for FrameworkElement failed");
        return;
    }

    auto className = winrt::get_class_name(toggleButtonElement);
    Wh_Log(L"Button class name: %s", className.c_str());
    if (className != L"Taskbar.ExperienceToggleButton") {
        return;
    }

    auto automationId =
        Automation::AutomationProperties::GetAutomationId(toggleButtonElement);
    Wh_Log(L"AutomationId: %s", automationId.c_str());
    if (automationId != L"StartButton") {
        return;
    }

    Wh_Log(L"Found StartButton element, name=%s", toggleButtonElement.Name().c_str());

    auto panelElement =
        FindChildByName(toggleButtonElement, L"ExperienceToggleButtonRootPanel");
    if (!panelElement) {
        Wh_Log(L"ExperienceToggleButtonRootPanel not found among direct children");
        int childCount =
            Media::VisualTreeHelper::GetChildrenCount(toggleButtonElement);
        for (int i = 0; i < childCount; i++) {
            auto child = Media::VisualTreeHelper::GetChild(toggleButtonElement, i)
                             .try_as<FrameworkElement>();
            if (child) {
                Wh_Log(L"  child %d: class=%s name=%s", i,
                       winrt::get_class_name(child).c_str(),
                       child.Name().c_str());
            }
        }
        return;
    }

    Wh_Log(L"Found panel, setting up tracking");
    EnsureTrackedButton(toggleButtonElement, panelElement);
}

// -----------------------------------------------------------------------
// Module load / symbol hooking plumbing
// -----------------------------------------------------------------------

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateButtonPadding(void))"},
            &ExperienceToggleButton_UpdateButtonPadding_Original,
            ExperienceToggleButton_UpdateButtonPadding_Hook,
        },
    };

    return HookSymbols(module, hooks, ARRAYSIZE(hooks));
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);
        if (HookTaskbarViewDllSymbols(module)) {
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
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }
    return module;
}

// -----------------------------------------------------------------------
// UI-thread marshaling. Wh_ModBeforeUninit/Wh_ModUninit/Wh_ModSettingsChanged
// run on the Windhawk engine thread, never the taskbar UI thread that owns
// every XAML/Composition object this mod touches (see the mod-lifetime
// wiki page) - calling into them directly throws RPC_E_WRONG_THREAD, or
// (for event revocation) silently fails to remove anything, leaving a
// callback registered against this module's memory after it's unloaded.
// RunFromWindowThread hands work to the owning thread's message loop and
// blocks until it's done, so by the time a caller returns, the work
// genuinely happened there.
// -----------------------------------------------------------------------

using RunFromWindowThreadProc_t = void(WINAPI*)(void*);

bool RunFromWindowThread(HWND window, RunFromWindowThreadProc_t proc, void* param) {
    static const UINT registeredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RunParam {
        RunFromWindowThreadProc_t proc;
        void* param;
    };

    DWORD threadId = GetWindowThreadProcessId(window, nullptr);
    if (!threadId) {
        return false;
    }
    if (threadId == GetCurrentThreadId()) {
        proc(param);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int code, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (code == HC_ACTION) {
                auto* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == registeredMsg) {
                    auto* p = (RunParam*)cwp->lParam;
                    p->proc(p->param);
                }
            }
            return CallNextHookEx(nullptr, code, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        return false;
    }

    RunParam runParam{proc, param};
    SendMessage(window, registeredMsg, 0, (LPARAM)&runParam);
    UnhookWindowsHookEx(hook);
    return true;
}

std::vector<HWND> FindCurrentProcessTaskbarWindows() {
    std::vector<HWND> result;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD pid = 0;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &pid) &&
                pid == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
                 _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0)) {
                ((std::vector<HWND>*)lParam)->push_back(hWnd);
            }
            return TRUE;
        },
        (LPARAM)&result);
    return result;
}

// Runs proc once per distinct taskbar UI thread (primary/secondary
// monitors may share or differ), synchronously.
void RunOnAllTaskbarThreads(RunFromWindowThreadProc_t proc, void* param) {
    std::vector<DWORD> doneThreads;
    for (HWND wnd : FindCurrentProcessTaskbarWindows()) {
        DWORD threadId = GetWindowThreadProcessId(wnd, nullptr);
        if (!threadId ||
            std::find(doneThreads.begin(), doneThreads.end(), threadId) !=
                doneThreads.end()) {
            continue;
        }
        doneThreads.push_back(threadId);
        RunFromWindowThread(wnd, proc, param);
    }
}

// -----------------------------------------------------------------------
// Settings load / lifecycle
// -----------------------------------------------------------------------

IconMode ParseMode(PCWSTR value) {
    if (wcscmp(value, L"recolor") == 0) {
        return IconMode::Recolor;
    }
    return IconMode::Default;
}

std::wstring GetStringSetting(PCWSTR name) {
    return WindhawkUtils::StringSetting::make(name).get();
}

void LoadSettings() {
    g_settingsGeneration++;

    auto modeStr = GetStringSetting(L"mode");
    g_settings.mode = ParseMode(modeStr.c_str());
    g_settings.recolorUseAccentColor =
        Wh_GetIntSetting(L"recolor.useAccentColor") != 0;
    g_settings.recolorColor = GetStringSetting(L"recolor.color");
    g_settings.recolorGradient = Wh_GetIntSetting(L"recolor.gradient.enabled") != 0;

    g_settings.gradientLightenAmount =
        Wh_GetIntSetting(L"recolor.gradient.lightenPercent") / 100.0f;
    g_settings.gradientDarkenAmount =
        Wh_GetIntSetting(L"recolor.gradient.darkenPercent") / 100.0f;

    g_settings.elevatedLightenBoost =
        Wh_GetIntSetting(L"recolor.elevate.lightenBoostPercent") / 100.0f;
    g_settings.elevatedDarkenRelief =
        Wh_GetIntSetting(L"recolor.elevate.darkenReliefPercent") / 100.0f;
    // Clamped to 1: used as a ramp-step divisor in the per-frame loop, and
    // 0 is a reachable user input ("Fade speed" has no built-in minimum).
    g_settings.hoverTransitionMs =
        std::max(1.0f, (float)Wh_GetIntSetting(L"recolor.elevate.transitionMs"));

    g_settings.shimmerEnabled =
        Wh_GetIntSetting(L"recolor.shimmer.enabled") != 0;
    g_settings.recolorShimmerColor = GetStringSetting(L"recolor.shimmer.color");
    g_cachedShimmerColor = ParseHexColor(g_settings.recolorShimmerColor);
    g_settings.shimmerAutoAmount =
        Wh_GetIntSetting(L"recolor.shimmer.autoLightenPercent") / 100.0f;
    g_settings.sweepDurationMs =
        (float)Wh_GetIntSetting(L"recolor.shimmer.durationMs");
    g_settings.sweepBandWidth =
        Wh_GetIntSetting(L"recolor.shimmer.bandWidthPercent") / 100.0f;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing...");

    LoadSettings();

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
        if (HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll")) {
            auto pKernelBaseLoadLibraryExW =
                (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                            "LoadLibraryExW");
            WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                            LoadLibraryExW_Hook,
                                            &LoadLibraryExW_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");
                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
}

// Runs on each taskbar UI thread via RunOnAllTaskbarThreads. Detaches every
// tracked button's handlers/render loop, restores its native icon, and
// releases the globals that hold strong Composition/UISettings refs -
// synchronously, so nothing is left registered against this module's
// memory once Wh_ModBeforeUninit returns and the DLL can be unloaded.
void WINAPI TeardownAllTrackedButtonsProc(void*) {
    for (auto& tracked : g_trackedButtons) {
        if (auto button = tracked.buttonRef.get()) {
            DetachRecolorHandlers(tracked, button);
            try {
                button.Unloaded(tracked.unloadedToken);
            } catch (winrt::hresult_error const&) {
            }
        }
        if (auto panel = tracked.panelRef.get()) {
            RestoreIconMode(panel);
        }
    }
    g_trackedButtons.clear();
    g_originalIconBrushes->clear();
    StopAccentColorTracking();
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    RunOnAllTaskbarThreads(TeardownAllTrackedButtonsProc, nullptr);
}

void Wh_ModUninit() {
}

// Runs on each taskbar UI thread via RunOnAllTaskbarThreads. Re-applies the
// resting foreground for every tracked button and attaches/detaches the
// recolor handlers as the new mode requires; prunes any entry whose button
// or panel no longer resolves.
void WINAPI ApplySettingsChangedProc(void*) {
    for (auto it = g_trackedButtons.begin(); it != g_trackedButtons.end();) {
        auto button = it->buttonRef.get();
        auto panel = it->panelRef.get();
        if (!button || !panel) {
            it = g_trackedButtons.erase(it);
            continue;
        }

        ApplyIconMode(panel);
        if (g_settings.mode == IconMode::Recolor) {
            AttachRecolorHandlers(*it, button, panel);
        } else {
            DetachRecolorHandlers(*it, button);
        }
        ++it;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    RunOnAllTaskbarThreads(ApplySettingsChangedProc, nullptr);
}

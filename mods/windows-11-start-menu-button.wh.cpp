// ==WindhawkMod==
// @id              windows-11-start-menu-button
// @name            Windows 11 Start Button Customizer
// @description     Custom icon and recolor (animation-preserving, with depth gradient and press-sweep) for the Windows 11 taskbar Start button
// @version         1.2
// @author          AristideBH
// @github          https://github.com/AristideBH
// @homepage        https://aristide-bh.com/
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 Start Button Customizer

> **Note:** This mod is vibe-coded - built largely with AI assistance and
> tested manually by the author, without a full independent code audit. Use
> at your own judgment, and please report anything odd via GitHub Issues.

![Silver recolor demo](https://raw.githubusercontent.com/AristideBH/windhawk-lib/main/windows-11-start-menu-button/demo/demo-recolor-silver.gif)

![Flat color + click shimmer demo](https://raw.githubusercontent.com/AristideBH/windhawk-lib/main/windows-11-start-menu-button/demo/demo-recolor-flat-shimmer.gif)

Customize the Windows 11 taskbar Start button's icon, without losing its
native hover/press animation:

- **System default** — leave the icon untouched.
- **Custom icon** — replace the stock icon with your own PNG/ICO file.
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
    it untouched. "Custom icon" replaces it with your own image. "Recolor"
    tints the built-in icon while keeping its native hover/press animation.
  $options:
  - default: System default
  - customIcon: Custom icon
  - recolor: Recolor
- customIcon:
  - path: ""
    $name: Image file
    $description: PNG or ICO file to show instead of the stock icon.
  $name: Custom icon
  $description: Settings used when Icon mode is "Custom icon".
- recolor:
  - useAccentColor: false
    $name: Use system accent color
    $description: >-
      Follow Windows' current accent color instead of Icon color below.
      Updates live when the accent color changes in Settings.
  - color: "#BABABA"
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
    - darkenPercent: 18
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
    - autoLightenPercent: 30
      $name: Auto shimmer lighten
      $description: >-
        Only used when Shimmer color is empty: how strongly to lighten Icon
        color for the sweep, like laying a white overlay over it at this
        opacity.
    - durationMs: 250
      $name: Sweep duration (ms)
      $description: How long the shimmer takes to sweep across the icon.
    - bandWidthPercent: 500
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
    - lightenBoostPercent: 100
      $name: Extra brightness
      $description: >-
        How much brighter the icon gets, on top of the normal shading,
        while hovered, pressed, or the Start menu is open.
    - darkenReliefPercent: 100
      $name: Shadow reduction
      $description: >-
        How much the dark side lightens up while hovered, pressed, or the
        Start menu is open.
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
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
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
    CustomIcon,
    Recolor,
};

struct {
    IconMode mode;
    std::wstring customIconPath;
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
    winrt::Windows::UI::Xaml::Controls::Image customIconOverlay{nullptr};
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
    // Held so Wh_ModBeforeUninit can revoke the CompositionTarget.Rendering
    // subscription synchronously and unconditionally on unload - see
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
// what the Start icon's flag squares actually use (confirmed: user reports
// the resting squares are a blue *gradient*, unaffected by our .Color()
// writes, while a separate solid-brush shape - the hover/press highlight
// overlay - did visibly change, which is what read as "shimmer only while
// animating"). Fix: replace the whole FillBrush/StrokeBrush/Visual.Brush
// with a freshly created solid CompositionColorBrush, which works
// regardless of the original brush's type. The original brush object
// (gradient or otherwise) is captured by shape/visual identity before the
// first replacement, so Default mode can restore the exact original -
// including the gradient - rather than guessing a fallback.
std::unordered_map<void*, winrt::Windows::UI::Composition::CompositionBrush>
    g_originalIconBrushes;

template <typename T>
void* Identity(T const& obj) {
    return winrt::get_abi(obj);
}

// Lightness-only adjustment (HSL), so the derived shade keeps the chosen
// color's hue/saturation instead of just blending toward white/black.
winrt::Windows::UI::Color AdjustLightness(winrt::Windows::UI::Color color,
                                           float delta) {
    float r = color.R / 255.0f;
    float g = color.G / 255.0f;
    float b = color.B / 255.0f;

    float maxC = std::max({r, g, b});
    float minC = std::min({r, g, b});
    float l = (maxC + minC) / 2.0f;
    float s = 0.0f;
    float h = 0.0f;

    if (maxC != minC) {
        float d = maxC - minC;
        s = l > 0.5f ? d / (2.0f - maxC - minC) : d / (maxC + minC);
        if (maxC == r) {
            h = (g - b) / d + (g < b ? 6.0f : 0.0f);
        } else if (maxC == g) {
            h = (b - r) / d + 2.0f;
        } else {
            h = (r - g) / d + 4.0f;
        }
        h /= 6.0f;
    }

    l = std::clamp(l + delta, 0.0f, 1.0f);

    auto hueToRgb = [](float p, float q, float t) {
        if (t < 0) {
            t += 1;
        }
        if (t > 1) {
            t -= 1;
        }
        if (t < 1.0f / 6) {
            return p + (q - p) * 6 * t;
        }
        if (t < 1.0f / 2) {
            return q;
        }
        if (t < 2.0f / 3) {
            return p + (q - p) * (2.0f / 3 - t) * 6;
        }
        return p;
    };

    float outR, outG, outB;
    if (s == 0.0f) {
        outR = outG = outB = l;
    } else {
        float q = l < 0.5f ? l * (1 + s) : l + s - l * s;
        float p = 2 * l - q;
        outR = hueToRgb(p, q, h + 1.0f / 3);
        outG = hueToRgb(p, q, h);
        outB = hueToRgb(p, q, h - 1.0f / 3);
    }

    auto toByte = [](float v) {
        return (BYTE)std::round(std::clamp(v, 0.0f, 1.0f) * 255.0f);
    };
    return winrt::Windows::UI::Color{color.A, toByte(outR), toByte(outG),
                                      toByte(outB)};
}

// -----------------------------------------------------------------------
// Depth gradient: per-tile Relative-mode gradient (0..1 of that shape's own
// render box), oriented so its direction matches the icon's global
// top-left-to-bottom-right diagonal regardless of how that specific tile is
// rotated/mirrored.
//
// A live full-tree dump (see conversation) proved two things Absolute-mode
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
        float lightenDelta =
            g_settings.gradientLightenAmount +
            g_settings.elevatedLightenBoost * params.elevatedAmount;
        float darkenDelta =
            std::max(0.0f, g_settings.gradientDarkenAmount -
                               g_settings.elevatedDarkenRelief *
                                   params.elevatedAmount);
        if (trace > 1.0f) {
            colorStart = AdjustLightness(params.baseColor, lightenDelta);
            colorEnd = params.baseColor;
        } else if (trace < -1.0f) {
            colorStart = params.baseColor;
            colorEnd = AdjustLightness(params.baseColor, -darkenDelta);
        } else {
            colorStart = AdjustLightness(params.baseColor, lightenDelta * 0.25f);
            colorEnd = AdjustLightness(params.baseColor, -darkenDelta * 0.25f);
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
            g_originalIconBrushes.try_emplace(identity, currentBrush);
        }
        setter(CreateRecolorBrush(compositor, *frameParams, shapeMatrix));
    } else {
        auto it = g_originalIconBrushes.find(identity);
        if (it != g_originalIconBrushes.end()) {
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

// Resolves the recolor mode's resting icon color: the system accent color
// when "Use system accent color" is on, otherwise the manual hex field.
std::optional<winrt::Windows::UI::Color> GetRecolorBaseColor() {
    if (g_settings.recolorUseAccentColor) {
        try {
            winrt::Windows::UI::ViewManagement::UISettings uiSettings;
            return uiSettings.GetColorValue(
                winrt::Windows::UI::ViewManagement::UIColorType::Accent);
        } catch (...) {
            return std::nullopt;
        }
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
    auto frameCounter = std::make_shared<int>(0);
    *token = Media::CompositionTarget::Rendering(
        [player, button, token, frameCounter](
            winrt::Windows::Foundation::IInspectable const&,
            winrt::Windows::Foundation::IInspectable const&) {
          try {
            (*frameCounter)++;
            bool logThisFrame = (*frameCounter % 60) == 0;

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
                int touched = RecolorAnimatedVisualPlayer(player, std::nullopt);
                if (tracked) {
                    tracked->hasLastApplied = true;
                    tracked->lastAppliedSettingsGeneration = g_settingsGeneration;
                }
                if (logThisFrame) {
                    Wh_Log(L"Icon color maintenance: mode!=Recolor, "
                           L"restoring original, touched=%d",
                           touched);
                }
                return;
            }

            auto baseColor = GetRecolorBaseColor();
            if (!tracked || !baseColor) {
                int touched = RecolorAnimatedVisualPlayer(player, std::nullopt);
                if (logThisFrame) {
                    Wh_Log(L"Icon color maintenance: no tracked button or "
                           L"unparsable resting color, restoring original, "
                           L"touched=%d",
                           touched);
                }
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
            // handler (see SetupButtonTracking), not inferred here by
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
            auto shimmerColor = ParseHexColor(g_settings.recolorShimmerColor);
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
                if (logThisFrame) {
                    Wh_Log(L"Icon color maintenance: idle, skipping reapply");
                }
                return;
            }

            int touched = RecolorAnimatedVisualPlayer(player, params);
            tracked->hasLastApplied = true;
            tracked->lastAppliedSettingsGeneration = g_settingsGeneration;
            tracked->lastAppliedElevatedAmount = tracked->iconElevatedAmount;
            tracked->lastAppliedSweepActive = sweepActiveNow;
            tracked->lastAppliedBaseColor = *baseColor;
            tracked->lastAppliedBandColor = bandColor;
            // Unthrottled while the sweep is running (~a few dozen frames
            // at most) so a capture can't miss it the way the 60-frame
            // throttle below did - this is the direct evidence for whether
            // the PointerPressed-triggered fix above actually fires and
            // for how the band renders frame-by-frame.
            if (sweepProgress >= 0.0f) {
                Wh_Log(L"Icon color maintenance: SWEEP progress=%.3f "
                       L"bandColor=(A%d,R%d,G%d,B%d) touched=%d",
                       sweepProgress, bandColor.A, bandColor.R, bandColor.G,
                       bandColor.B, touched);
            }
            if (logThisFrame) {
                Wh_Log(
                    L"Icon color maintenance: state=%d menuOpen=%d "
                    L"elevated=%.2f sweep=%.2f touched=%d",
                    (int)tracked->lastState, (int)tracked->menuOpen,
                    tracked->iconElevatedAmount, sweepProgress, touched);
            }
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

std::wstring FilePathToFileUri(const std::wstring& path) {
    std::wstring uri = L"file:///";
    for (wchar_t c : path) {
        uri += (c == L'\\') ? L'/' : c;
    }
    return uri;
}

// -----------------------------------------------------------------------
// Icon mode application (custom icon overlay / recolor foreground
// fallback). Not per-state - mode/customIconPath/recolorColor don't vary
// with hover/press, so this only needs to run on setup and on settings
// change, not on every pointer transition.
// -----------------------------------------------------------------------

void ApplyIconMode(FrameworkElement panel, TrackedButton& tracked) {
    auto iconElement = FindIconElement(panel);
    if (iconElement) {
        Wh_Log(L"Icon element found: class=%s name=%s",
               winrt::get_class_name(iconElement).c_str(),
               iconElement.Name().c_str());
    } else {
        Wh_Log(L"Icon element NOT found under panel (class=%s)",
               winrt::get_class_name(panel).c_str());
        int childCount = Media::VisualTreeHelper::GetChildrenCount(panel);
        for (int i = 0; i < childCount; i++) {
            auto child = Media::VisualTreeHelper::GetChild(panel, i)
                             .try_as<FrameworkElement>();
            if (child) {
                Wh_Log(L"  panel child %d: class=%s name=%s", i,
                       winrt::get_class_name(child).c_str(),
                       child.Name().c_str());
            }
        }
    }

    const std::wstring& iconPath = g_settings.customIconPath;

    if (g_settings.mode == IconMode::CustomIcon && !iconPath.empty()) {
        // Hide the native icon and overlay our own Image element. The panel
        // (Taskbar.TaskListButtonPanel) is a Panel, not necessarily a Grid,
        // so insert into its generic Children collection rather than
        // requiring Grid row/column semantics.
        if (iconElement) {
            iconElement.Visibility(Visibility::Collapsed);
        }

        auto panelAsPanel = panel.try_as<Controls::Panel>();
        if (panelAsPanel) {
            if (!tracked.customIconOverlay) {
                Controls::Image image;
                image.Stretch(Media::Stretch::Uniform);
                image.HorizontalAlignment(HorizontalAlignment::Center);
                image.VerticalAlignment(VerticalAlignment::Center);
                if (iconElement) {
                    image.Width(iconElement.ActualWidth() > 0
                                    ? iconElement.ActualWidth()
                                    : iconElement.Width());
                    image.Height(iconElement.ActualHeight() > 0
                                     ? iconElement.ActualHeight()
                                     : iconElement.Height());
                    if (auto grid = panel.try_as<Controls::Grid>()) {
                        Controls::Grid::SetRow(image,
                                                Controls::Grid::GetRow(iconElement));
                        Controls::Grid::SetColumn(
                            image, Controls::Grid::GetColumn(iconElement));
                    }
                }
                panelAsPanel.Children().Append(image);
                tracked.customIconOverlay = image;
            }

            Media::Imaging::BitmapImage bitmap;
            bitmap.UriSource(
                winrt::Windows::Foundation::Uri{FilePathToFileUri(iconPath)});
            tracked.customIconOverlay.Source(bitmap);
            tracked.customIconOverlay.Visibility(Visibility::Visible);
        } else {
            Wh_Log(L"Panel is not a Panel-derived element, cannot overlay icon");
        }
    } else {
        // Not custom-icon mode (or no path set): remove any overlay and
        // restore the native icon.
        if (tracked.customIconOverlay) {
            tracked.customIconOverlay.Visibility(Visibility::Collapsed);
        }
        if (iconElement) {
            iconElement.Visibility(Visibility::Visible);

            // AnimatedVisualPlayer (the Start icon's actual type) is NOT
            // handled here: its Lottie shape-brush colors get fought/reset
            // by native hover/press animation playback, so it needs
            // continuous per-frame enforcement rather than a one-shot
            // property set. See StartPersistentIconColorMaintenance, run
            // once per button for its whole lifetime.
            if (auto iconElementAsIcon =
                    iconElement.try_as<Controls::IconElement>()) {
                if (g_settings.mode == IconMode::Recolor) {
                    auto color = GetRecolorBaseColor();
                    if (color) {
                        iconElementAsIcon.Foreground(
                            Media::SolidColorBrush{*color});
                    }
                } else {
                    iconElementAsIcon.ClearValue(
                        Controls::IconElement::ForegroundProperty());
                }
            }
        }
    }
}

// Unload/mode-switch restore: undo whatever ApplyIconMode did, so the
// button is left showing the stock native icon.
void RestoreIconMode(FrameworkElement panel, TrackedButton& tracked) {
    if (tracked.customIconOverlay) {
        tracked.customIconOverlay.Visibility(Visibility::Collapsed);
    }
    auto iconElement = FindIconElement(panel);
    if (iconElement) {
        iconElement.Visibility(Visibility::Visible);
        if (auto icon = iconElement.try_as<Controls::IconElement>()) {
            icon.ClearValue(Controls::IconElement::ForegroundProperty());
        }
    }
}

// -----------------------------------------------------------------------
// Pointer state tracking - only feeds TrackedButton::lastState, which the
// icon depth-gradient animation (elevated brighten, press sweep) reads.
// No box styling is applied here; native hover/press animations are
// untouched regardless.
// -----------------------------------------------------------------------

void SetupButtonTracking(FrameworkElement button, FrameworkElement panel) {
    if (FindTrackedButton(button)) {
        Wh_Log(L"Button already tracked, skipping setup");
        return;
    }

    Wh_Log(L"Setting up tracking for new button instance");

    g_trackedButtons.push_back({});
    TrackedButton& tracked = g_trackedButtons.back();
    tracked.buttonRef = button;
    tracked.panelRef = panel;

    auto setState = [](FrameworkElement button, ButtonState state) {
        if (auto t = FindTrackedButton(button)) {
            t->lastState = state;
        }
    };

    tracked.pointerEnteredToken = button.PointerEntered(
        [setState](winrt::Windows::Foundation::IInspectable const& sender,
                    auto const&) {
            Wh_Log(L"PointerEntered fired");
            setState(sender.try_as<FrameworkElement>(), kStateHover);
        });
    tracked.pointerExitedToken = button.PointerExited(
        [setState](winrt::Windows::Foundation::IInspectable const& sender,
                    auto const&) {
            Wh_Log(L"PointerExited fired");
            setState(sender.try_as<FrameworkElement>(), kStateDefault);
        });
    tracked.pointerPressedToken = button.PointerPressed(
        [setState](winrt::Windows::Foundation::IInspectable const& sender,
                    auto const&) {
            Wh_Log(L"PointerPressed fired");
            auto pressedButton = sender.try_as<FrameworkElement>();
            setState(pressedButton, kStatePressed);
            // Trigger the sweep here, not by polling lastState in the
            // render loop - see the comment above sweepProgress in
            // StartPersistentIconColorMaintenance for why polling misses
            // fast clicks entirely.
            if (auto t = FindTrackedButton(pressedButton)) {
                t->iconPressSweepActive = true;
                t->iconPressSweepStartMs = GetTickCount64();
                Wh_Log(L"Sweep armed: active=%d startMs=%llu",
                       (int)t->iconPressSweepActive,
                       (unsigned long long)t->iconPressSweepStartMs);
            } else {
                Wh_Log(L"PointerPressed: FindTrackedButton failed, sweep NOT armed");
            }
        });
    tracked.pointerReleasedToken = button.PointerReleased(
        [setState](winrt::Windows::Foundation::IInspectable const& sender,
                    auto const&) {
            Wh_Log(L"PointerReleased fired");
            setState(sender.try_as<FrameworkElement>(), kStateHover);
        });
    tracked.pointerCaptureLostToken = button.PointerCaptureLost(
        [setState](winrt::Windows::Foundation::IInspectable const& sender,
                    auto const&) {
            Wh_Log(L"PointerCaptureLost fired");
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
    // a "press" for sweep purposes) - the PointerPressed arming below is
    // kept as a harmless no-op fallback in case a future build does
    // route it through normally.
    if (auto toggleButton = button.try_as<Controls::Primitives::ToggleButton>()) {
        tracked.checkedToken = toggleButton.Checked(
            [](winrt::Windows::Foundation::IInspectable const& sender,
               auto const&) {
                Wh_Log(L"Checked fired");
                if (auto t = FindTrackedButton(sender.try_as<FrameworkElement>())) {
                    t->menuOpen = true;
                    t->iconPressSweepActive = true;
                    t->iconPressSweepStartMs = GetTickCount64();
                }
            });
        tracked.uncheckedToken = toggleButton.Unchecked(
            [](winrt::Windows::Foundation::IInspectable const& sender,
               auto const&) {
                Wh_Log(L"Unchecked fired");
                if (auto t = FindTrackedButton(sender.try_as<FrameworkElement>())) {
                    t->menuOpen = false;
                    t->iconPressSweepActive = true;
                    t->iconPressSweepStartMs = GetTickCount64();
                }
            });
    } else {
        Wh_Log(L"Button is not a ToggleButton, menu-open icon state will "
               L"not be detected");
    }

    ApplyIconMode(panel, tracked);

    if (auto iconElement = FindIconElement(panel)) {
        if (auto player =
                iconElement
                    .try_as<winrt::Microsoft::UI::Xaml::Controls::AnimatedVisualPlayer>()) {
            StartPersistentIconColorMaintenance(player, button);
        }
    }
}

void ReapplyAllTrackedButtons() {
    for (auto& tracked : g_trackedButtons) {
        auto panel = tracked.panelRef.get();
        if (!panel) {
            continue;
        }
        if (g_unloading) {
            RestoreIconMode(panel, tracked);
        } else {
            ApplyIconMode(panel, tracked);
        }
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
    SetupButtonTracking(toggleButtonElement, panelElement);
}

// -----------------------------------------------------------------------
// Module load / symbol hooking plumbing
// -----------------------------------------------------------------------

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, or ExplorerExtensions.dll on some builds - see
    // GetTaskbarViewModuleHandle.
    WindhawkUtils::SYMBOL_HOOK taskbarViewDllHooks[] = {
        {
            {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateButtonPadding(void))"},
            &ExperienceToggleButton_UpdateButtonPadding_Original,
            ExperienceToggleButton_UpdateButtonPadding_Hook,
        },
    };

    return HookSymbols(module, taskbarViewDllHooks, ARRAYSIZE(taskbarViewDllHooks));
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
// Settings load / lifecycle
// -----------------------------------------------------------------------

IconMode ParseMode(PCWSTR value) {
    if (wcscmp(value, L"customIcon") == 0) {
        return IconMode::CustomIcon;
    }
    if (wcscmp(value, L"recolor") == 0) {
        return IconMode::Recolor;
    }
    return IconMode::Default;
}

std::wstring GetStringSetting(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

void LoadSettings() {
    g_settingsGeneration++;

    auto modeStr = GetStringSetting(L"mode");
    g_settings.mode = ParseMode(modeStr.c_str());
    g_settings.customIconPath = GetStringSetting(L"customIcon.path");
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
    g_settings.hoverTransitionMs =
        (float)Wh_GetIntSetting(L"recolor.elevate.transitionMs");

    g_settings.shimmerEnabled =
        Wh_GetIntSetting(L"recolor.shimmer.enabled") != 0;
    g_settings.recolorShimmerColor = GetStringSetting(L"recolor.shimmer.color");
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
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                        "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                        LoadLibraryExW_Hook,
                                        &LoadLibraryExW_Original);
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

void Wh_ModBeforeUninit() {
    g_unloading = true;
    // Stop every button's persistent render-loop subscription synchronously
    // before anything else - see StopIconColorMaintenance for why this
    // can't be left to the render callback noticing g_unloading on its own.
    for (auto& tracked : g_trackedButtons) {
        StopIconColorMaintenance(tracked);
    }
    ReapplyAllTrackedButtons();  // restores native icon (RestoreIconMode, via g_unloading)
}

void Wh_ModUninit() {
    g_trackedButtons.clear();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    ReapplyAllTrackedButtons();
}

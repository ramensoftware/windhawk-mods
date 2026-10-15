// ==WindhawkMod==
// @id              macos-style-dock
// @name            MacOS Style Dock
// @description     Turns the Windows 11 taskbar into a macOS-style dock with magnifying icons
// @version         1.0
// @author          Zach Vaughn
// @github          https://github.com/zachvdev
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# MacOS Style Dock

A dock as close to macOS as possible, while still working the way Windows
does. Icons sit on a frosted-glass dock and grow as the pointer passes over
them.

Later I plan on splitting the Start Menu and Status Bar into a "Menu Bar" and
sticking it to the top of the display.

## Compatibility

- Windows 11 only, with the taskbar at the bottom of the screen.
- x64 PCs only for now; ARM64 isn't supported yet.
- The "Small taskbar buttons" setting isn't supported.
- Not designed to be used with other taskbar mods. In particular:
  - It sets the taskbar's height and icon size itself, so don't combine it
    with "Taskbar height and icon size", "Mac Island & Dock" or "Taskbar Dock
    Animation".
  - It uses XAML diagnostics to reach the taskbar's buttons, and only one
    program can do that at a time, so it can't run together with "Windows 11
    Taskbar Styler", TranslucentTB or UWPSpy.
- If Windows' Smart App Control is on, it blocks this mod (as it does all
  Windhawk mods), because the mod is compiled on your PC and isn't signed.

The taskbar height code is based on m417z's "Taskbar height and icon size"
mod (GPL-3.0).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- dockSize: 85
  $name: Dock Size (0-100)
  $description: Size of the icons and the dock. (Default = 85)
- magnification: 60
  $name: Magnification (0-100)
  $description: How much the icons grow when hovered. (Default = 60)
- dockOpacity: 25
  $name: Dock Tint (0-100)
  $description: How opaque the dock panel is. (Default = 25)
- dockBlur: 15
  $name: Dock Blur (0-100)
  $description: How blurred the dock panel is. (Default = 15)
- animationMs: 125
  $name: Smoothing (ms)
  $description: Duration of animation transitions. (Default = 125)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <ocidl.h>
#include <xamlOM.h>

#include <algorithm>
#include <atomic>
#include <charconv>
#include <chrono>
#include <cmath>
#include <iterator>
#include <limits>
#include <list>
#include <regex>
#include <vector>
#include <string_view>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxh = winrt::Windows::UI::Xaml::Hosting;
namespace wuxi = winrt::Windows::UI::Xaml::Input;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuc = winrt::Windows::UI::Composition;
namespace wge = winrt::Windows::Graphics::Effects;

// ---- Settings, as the user set them ----
int g_dockSize = 85;           // 0-100
float g_scale = 1.6f;          // from Magnification: 0-100 -> 1.0-2.0
int g_dockTint = 25;           // 0-100
int g_dockBlur = 15;           // 0-100
int g_animationMs = 125;

// ---- Sizes derived from the settings (see ComputeSizes) ----
float g_iconSize = 39.0f;
float g_radius = 130.0f;       // effect radius
float g_buttonMargin = 4.0f;   // per side; negative pulls buttons together
float g_dockHeight = 65.0f;
float g_dockSidePadding = 15.0f;
float g_dockRadius = 20.0f;
float g_dockBottomGap = 7.0f;
int g_taskbarHeight = 85;      // 0 = leave the taskbar at its normal height
float g_iconLoadSize = 63;     // resolution icons are loaded at; 0 = Windows' own

constexpr float kStockIconSize = 24;      // Windows 11 taskbar icon size
constexpr float kStockButtonWidth = 44;   // Windows 11 taskbar button width
constexpr int kMinTaskbarHeight = 40;     // keeps the clock from being squashed

// Fixed tuning, found by testing. Both are given for the stock 24px icon and
// scale with the dock.
constexpr float kEffectRadius = 80;       // how far from the pointer icons grow
constexpr float kIconSpacing = 4;         // on the old 0-10 scale: gap = icon / 3

void ComputeSizes() {
    // Exponential, so every step of the slider feels the same:
    // 0 -> 12px, 50 -> 24px (Windows' own size), 100 -> 48px.
    float icon = std::round(kStockIconSize * std::exp2((g_dockSize - 50) / 50.0f));
    g_iconSize = icon;

    // Ratios that give the original look (40px dock, 12px corners, 4px gap)
    // at the stock 24px icon.
    g_dockHeight = std::round(icon * 5 / 3);
    // Space between the outermost icons and the dock's ends: 15px at the
    // default 39px icon (Dock Size 85).
    g_dockSidePadding = std::round(icon * 0.385f);
    g_dockRadius = std::round(g_dockHeight * 0.3f);
    g_dockBottomGap = std::round(icon / 6);
    g_radius = kEffectRadius * icon / kStockIconSize;

    // Visible gap between icons, scaled with the icons. The margins make
    // every button take up exactly icon + gap.
    float gap = kIconSpacing * icon / 12;
    g_buttonMargin = (icon + gap - kStockButtonWidth) / 2;

    // Icons are loaded at the largest size they're ever shown at: fully
    // magnified. Shown smaller, they're scaled down, which stays sharp.
    g_iconLoadSize = std::ceil(icon * g_scale);

    // The highest point the dock reaches: the top of the dock itself, or of a
    // fully magnified icon. The icon sits centered in the dock and grows
    // upward from its bottom edge, so its top ends up
    // bottomGap + (dock - icon) / 2 + scale * icon above the screen's bottom.
    // With little magnification, that's lower than the top of the dock.
    float dockTop = g_dockBottomGap + g_dockHeight;
    float iconTop = g_dockBottomGap + (g_dockHeight - icon) / 2 + g_scale * icon;
    float highest = std::max(dockTop, iconTop);

    // The taskbar is just tall enough for that (+2px so a magnified icon never
    // touches the edge).
    g_taskbarHeight = std::max((int)std::ceil(highest + 2), kMinTaskbarHeight);
}

// ---------------------------------------------------------------------------
// The actual feature
// ---------------------------------------------------------------------------

struct Button {
    InstanceHandle handle = 0;
    winrt::weak_ref<wux::FrameworkElement> element;
    wf::IInspectable originalMargin{nullptr};  // local value, may be UnsetValue
    int64_t marginToken = 0;
    wux::FrameworkElement::SizeChanged_revoker sizeChanged;
};

// Side margins set the space between buttons: negative pulls neighbors
// closer together, positive pushes them apart. Only writes when different,
// since it also runs from the Margin change callback (see OnButtonAdded).
void ApplySpacing(wux::FrameworkElement const& el) {
    wux::Thickness want{g_buttonMargin, 0, g_buttonMargin, 0};
    auto m = el.Margin();
    if (m.Left != want.Left || m.Top != want.Top || m.Right != want.Right ||
        m.Bottom != want.Bottom) {
        el.Margin(want);
    }
}

void RestoreSpacing(Button const& b) {
    auto el = b.element.get();
    if (!el) return;
    if (!b.originalMargin || b.originalMargin == wux::DependencyProperty::UnsetValue()) {
        el.ClearValue(wux::FrameworkElement::MarginProperty());
    } else {
        el.SetValue(wux::FrameworkElement::MarginProperty(), b.originalMargin);
    }
}

// Globals holding XAML objects are marked no_destroy: when Explorer exits,
// XAML shuts down before globals are destroyed, and destroying them then
// could crash it. ResetAll empties them when the mod is unloaded.
[[clang::no_destroy]] std::list<Button> g_buttons;

float GetRowOffsetY(wux::DependencyObject const& row);
void PlaceIndicator(wux::FrameworkElement const& button, float s);

// The point a button is magnified around: the bottom edge of its icon. The
// icon is centered in its button, so that's half an icon below the center.
float ButtonPivotY(wux::FrameworkElement const& button) {
    return (float)button.ActualHeight() / 2 + g_iconSize / 2;
}

// Scale the icon (growing upward from the icon's bottom edge), shift it
// sideways by `tx` pixels, and move it down onto the dock by its row's
// vertical offset (see MeasurePlate). None of this affects the taskbar's
// layout.
void SetScale(wux::FrameworkElement const& el, float s, float tx = 0) {
    auto visual = wuxh::ElementCompositionPreview::GetElementVisual(el);
    visual.CenterPoint({(float)el.ActualWidth() / 2, ButtonPivotY(el), 0});
    visual.Scale({s, s, 1});
    float ty = GetRowOffsetY(wuxm::VisualTreeHelper::GetParent(el));
    wuxh::ElementCompositionPreview::SetIsTranslationEnabled(el, true);
    visual.Properties().InsertVector3(L"Translation", {tx, ty, 0});
    // Everything inside the button moves with it; the running indicator gets
    // pulled back down to the bottom of the dock.
    PlaceIndicator(el, s);
}

// The row of icons the pointer is currently over. Its positions are re-read
// every frame (see OnFrame), so it follows apps opening, closing and being
// dragged to a new spot.
struct RowItem {
    winrt::weak_ref<wux::FrameworkElement> element;
    float center = 0;  // horizontal center, in the row parent's coordinates
    float width = 0;
    float scale = 1.0f;
    float shift = 0;
};

[[clang::no_destroy]] std::vector<RowItem> g_row;
[[clang::no_destroy]] winrt::weak_ref<wux::UIElement> g_rowParent;

// Latest pointer state. Pointer events only write these; the per-frame
// update below reads them.
bool g_pointerOver = false;
float g_pointerX = 0;

// ---- Hidden taskbar background ----

struct HiddenBackground {
    InstanceHandle handle = 0;
    winrt::weak_ref<wux::FrameworkElement> element;
    double originalOpacity = 1.0;
};

[[clang::no_destroy]] std::list<HiddenBackground> g_backgrounds;

// Makes the taskbar's background invisible (opacity 0), remembering how to
// put it back. Opacity rather than Collapsed, so it still catches clicks on
// the taskbar's empty space.
void HideBackground(InstanceHandle handle, wux::FrameworkElement const& el) {
    // The thumbnail-preview flyout uses the same control type; only hide the
    // taskbar's own one.
    if (el.Name() != L"BackgroundControl") return;
    for (auto& b : g_backgrounds) {
        if (b.handle == handle) return;
    }
    g_backgrounds.push_back({handle, winrt::make_weak(el), el.Opacity()});
    el.Opacity(0);
    Wh_Log(L"Taskbar background hidden");
}

void RestoreHidden(HiddenBackground const& b) {
    if (auto el = b.element.get()) {
        try { el.Opacity(b.originalOpacity); } catch (...) {}
    }
}

void RestoreBackgrounds() {
    for (auto& b : g_backgrounds) RestoreHidden(b);
    g_backgrounds.clear();
}

// The taskbar app/system button `el` sits inside, at most `maxDepth` levels
// up, or null.
wux::FrameworkElement FindTaskbarButton(wux::FrameworkElement const& el, int maxDepth = 4) {
    wux::DependencyObject current = wuxm::VisualTreeHelper::GetParent(el);
    for (int depth = 0; current && depth < maxDepth; depth++) {
        auto name = winrt::get_class_name(current);
        if (name == L"Taskbar.TaskListButton" || name == L"Taskbar.ExperienceToggleButton") {
            return current.try_as<wux::FrameworkElement>();
        }
        current = wuxm::VisualTreeHelper::GetParent(current);
    }
    return nullptr;
}

bool IsInsideTaskbarButton(wux::FrameworkElement const& el, int maxDepth = 4) {
    return (bool)FindTaskbarButton(el, maxDepth);
}

// ---- Button highlight ----
// The rounded box behind each icon when it's hovered or its window is active
// ("BackgroundElement"), and its stacked copy for apps with several windows
// ("MultiWindowElement"). Each is hidden two ways at once: Opacity 0, and
// Collapsed, which isn't drawn at all whatever its opacity. The taskbar fades
// a new app's highlight in with an animation on Opacity, and an animation
// always beats a value we set, so opacity alone would leave it visible. If
// the taskbar later sets either property itself, it's hidden again.

struct HiddenHighlight {
    winrt::weak_ref<wux::FrameworkElement> element;
    double originalOpacity = 1.0;
    wux::Visibility originalVisibility = wux::Visibility::Visible;
    int64_t opacityToken = 0, visibilityToken = 0;
};

[[clang::no_destroy]] std::vector<HiddenHighlight> g_highlights;

void ApplyHighlightHidden(wux::UIElement const& el) {
    if (el.Opacity() != 0) el.Opacity(0);
    if (el.Visibility() != wux::Visibility::Collapsed) el.Visibility(wux::Visibility::Collapsed);
}

void HideHighlight(wux::FrameworkElement const& el) {
    // Only the dock buttons' own; other controls use the same names.
    if (!IsInsideTaskbarButton(el)) return;
    // Forget the highlights of buttons that are gone.
    g_highlights.erase(std::remove_if(g_highlights.begin(), g_highlights.end(),
                                      [](HiddenHighlight& h) { return !h.element.get(); }),
                       g_highlights.end());
    for (auto& h : g_highlights) {
        if (h.element.get() == el) return;
    }
    auto& h = g_highlights.emplace_back();
    h.element = winrt::make_weak(el);
    h.originalOpacity = el.Opacity();
    h.originalVisibility = el.Visibility();
    auto rehide = [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
        try {
            if (auto ui = sender.try_as<wux::UIElement>()) ApplyHighlightHidden(ui);
        } catch (...) {}
    };
    h.opacityToken = el.RegisterPropertyChangedCallback(wux::UIElement::OpacityProperty(), rehide);
    h.visibilityToken =
        el.RegisterPropertyChangedCallback(wux::UIElement::VisibilityProperty(), rehide);
    ApplyHighlightHidden(el);
}

void RestoreHighlights() {
    for (auto& h : g_highlights) {
        auto el = h.element.get();
        if (!el) continue;
        try {
            el.UnregisterPropertyChangedCallback(wux::UIElement::OpacityProperty(),
                                                 h.opacityToken);
            el.UnregisterPropertyChangedCallback(wux::UIElement::VisibilityProperty(),
                                                 h.visibilityToken);
            el.Opacity(h.originalOpacity);
            el.Visibility(h.originalVisibility);
        } catch (...) {}
    }
    g_highlights.clear();
}

// ---- Icon size ----
// The image named "Icon" inside each button. Windows sizes it at 24px; we
// give it the dock's icon size, and set it again whenever the taskbar puts
// its own size back. Resizing the image (rather than scaling the button)
// lets Windows draw the icon from its full-resolution source, so it stays
// sharp as long as that source is big enough.

struct IconImage {
    InstanceHandle handle = 0;
    winrt::weak_ref<wux::FrameworkElement> element;
    wf::IInspectable originalWidth{nullptr};   // local values, may be UnsetValue
    wf::IInspectable originalHeight{nullptr};
    int64_t widthToken = 0, heightToken = 0;
};

[[clang::no_destroy]] std::list<IconImage> g_iconImages;

void ApplyIconSize(wux::FrameworkElement const& el) {
    // Width() is NaN when unset, and NaN != anything, so this also covers it.
    if (el.Width() != g_iconSize) el.Width(g_iconSize);
    if (el.Height() != g_iconSize) el.Height(g_iconSize);
}

void TrackIconImage(InstanceHandle handle, wux::FrameworkElement const& el) {
    if (!IsInsideTaskbarButton(el, 6)) return;
    for (auto& i : g_iconImages) {
        if (i.handle == handle) return;
    }
    auto& i = g_iconImages.emplace_back();
    i.handle = handle;
    i.element = winrt::make_weak(el);
    i.originalWidth = el.ReadLocalValue(wux::FrameworkElement::WidthProperty());
    i.originalHeight = el.ReadLocalValue(wux::FrameworkElement::HeightProperty());
    auto reapply = [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
        try {
            if (auto fe = sender.try_as<wux::FrameworkElement>()) ApplyIconSize(fe);
        } catch (...) {}
    };
    i.widthToken =
        el.RegisterPropertyChangedCallback(wux::FrameworkElement::WidthProperty(), reapply);
    i.heightToken =
        el.RegisterPropertyChangedCallback(wux::FrameworkElement::HeightProperty(), reapply);
    ApplyIconSize(el);
}

void RestoreLocalValue(wux::FrameworkElement const& el, wux::DependencyProperty const& dp,
                       wf::IInspectable const& value) {
    if (!value || value == wux::DependencyProperty::UnsetValue()) {
        el.ClearValue(dp);
    } else {
        el.SetValue(dp, value);
    }
}

// Undoes TrackIconImage for one image.
void ReleaseIconImage(IconImage const& i) {
    auto el = i.element.get();
    if (!el) return;
    try {
        el.UnregisterPropertyChangedCallback(wux::FrameworkElement::WidthProperty(),
                                             i.widthToken);
        el.UnregisterPropertyChangedCallback(wux::FrameworkElement::HeightProperty(),
                                             i.heightToken);
        RestoreLocalValue(el, wux::FrameworkElement::WidthProperty(), i.originalWidth);
        RestoreLocalValue(el, wux::FrameworkElement::HeightProperty(), i.originalHeight);
    } catch (...) {}
}

void RestoreIconImages() {
    for (auto& i : g_iconImages) ReleaseIconImage(i);
    g_iconImages.clear();
}

// A visible taskbar button and where it sits inside its row.
struct Icon {
    wux::FrameworkElement element{nullptr};
    float left = 0;  // in the row parent's coordinates
    float top = 0;
    float width = 0;
    float height = 0;
};

// All visible tracked buttons whose parent is `parent`.
std::vector<Icon> GetIcons(wux::UIElement const& parent) {
    std::vector<Icon> icons;
    for (auto& b : g_buttons) {
        auto other = b.element.get();
        if (!other || other.Visibility() != wux::Visibility::Visible ||
            other.ActualWidth() <= 0) {
            continue;
        }
        if (wuxm::VisualTreeHelper::GetParent(other) != parent) continue;
        // Where the taskbar laid the button out. Unlike TransformToVisual,
        // this ignores the scaling/shifting we apply, so measurements taken
        // while icons are magnified are still correct.
        auto slot = wuxc::Primitives::LayoutInformation::GetLayoutSlot(other);
        auto margin = other.Margin();
        float x = slot.X + (float)margin.Left;
        float y = slot.Y + (float)margin.Top;
        // Skip recycled buttons parked out of view by the taskbar.
        if (y < -other.ActualHeight() || y > other.ActualHeight()) continue;
        icons.push_back({other, x, y, (float)other.ActualWidth(), (float)other.ActualHeight()});
    }
    return icons;
}

// ---- Dock plate: a rounded box drawn behind each taskbar's icons ----

// Dock Blur 100 = this much Gaussian blur. 50 gives 30, the same as Windows'
// own acrylic material.
constexpr float kMaxBlur = 60;

// A Gaussian blur for Windows.UI.Composition. Composition takes effects as
// descriptions it hands to Direct2D, normally built with the Win2D library;
// Explorer doesn't ship Win2D, so this describes the one effect we need
// ourselves, the same way Win2D does.
enum DOCK_EFFECT_PROPERTY_MAPPING : int {
    DOCK_EFFECT_PROPERTY_MAPPING_UNKNOWN = 0,
    DOCK_EFFECT_PROPERTY_MAPPING_DIRECT = 1,
};

MIDL_INTERFACE("2FC57384-A068-44D7-A331-30982FCF7177")
IDockGraphicsEffectD2D1Interop : public IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index, DOCK_EFFECT_PROPERTY_MAPPING* mapping) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperty(UINT index, void** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSource(UINT index, void** source) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) = 0;
};
__CRT_UUID_DECL(IDockGraphicsEffectD2D1Interop, 0x2fc57384, 0xa068, 0x44d7, 0xa3, 0x31, 0x30,
                0x98, 0x2f, 0xcf, 0x71, 0x77)

// CLSID_D2D1GaussianBlur
constexpr GUID kGaussianBlurId = {
    0x1feb6d69, 0x2fe6, 0x4ac9, {0x8c, 0x58, 0x1d, 0x7f, 0x93, 0xe7, 0xa6, 0xa5}};

class GaussianBlurEffect
    : public winrt::implements<GaussianBlurEffect, wge::IGraphicsEffect,
                               wge::IGraphicsEffectSource, IDockGraphicsEffectD2D1Interop> {
public:
    GaussianBlurEffect(float amount, wge::IGraphicsEffectSource const& source)
        : m_amount(amount), m_source(source) {}

    winrt::hstring Name() { return m_name; }
    void Name(winrt::hstring const& name) { m_name = name; }

    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) override {
        *id = kGaussianBlurId;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index, DOCK_EFFECT_PROPERTY_MAPPING* mapping) override {
        if (_wcsicmp(name, L"BlurAmount") != 0) return E_INVALIDARG;
        *index = 0;
        *mapping = DOCK_EFFECT_PROPERTY_MAPPING_DIRECT;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) override {
        *count = 3;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, void** value) override {
        try {
            wf::IInspectable v;
            switch (index) {
                case 0: v = wf::PropertyValue::CreateSingle(m_amount); break;  // std. deviation
                case 1: v = wf::PropertyValue::CreateUInt32(1); break;  // optimization: balanced
                case 2: v = wf::PropertyValue::CreateUInt32(1); break;  // border mode: hard
                default: return E_INVALIDARG;
            }
            *value = winrt::detach_abi(v.as<wf::IPropertyValue>());
            return S_OK;
        } catch (...) {
            return winrt::to_hresult();
        }
    }
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, void** source) override {
        if (index != 0) return E_INVALIDARG;
        winrt::copy_to_abi(m_source, *source);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) override {
        *count = 1;
        return S_OK;
    }

private:
    float m_amount;
    wge::IGraphicsEffectSource m_source;
    winrt::hstring m_name;
};

// Puts a blurred copy of what's behind `host` inside it, with rounded
// corners. Returns the corner shape so its radius can be changed later, or
// nothing when the blur is off.
wuc::CompositionRoundedRectangleGeometry SetBlurVisual(wux::UIElement const& host, float amount) {
    if (amount <= 0) {
        wuxh::ElementCompositionPreview::SetElementChildVisual(host, nullptr);
        return nullptr;
    }
    auto hostVisual = wuxh::ElementCompositionPreview::GetElementVisual(host);
    auto compositor = hostVisual.Compositor();

    auto effect = winrt::make<GaussianBlurEffect>(
        amount, wuc::CompositionEffectSourceParameter(L"Backdrop"));
    auto brush = compositor.CreateEffectFactory(effect).CreateBrush();
    brush.SetSourceParameter(L"Backdrop", compositor.CreateBackdropBrush());

    auto sprite = compositor.CreateSpriteVisual();
    sprite.Brush(brush);

    // Follow the host element's size, and clip to the dock's rounded shape.
    auto size = compositor.CreateExpressionAnimation(L"host.Size");
    size.SetReferenceParameter(L"host", hostVisual);
    sprite.StartAnimation(L"Size", size);
    auto shape = compositor.CreateRoundedRectangleGeometry();
    shape.CornerRadius({g_dockRadius, g_dockRadius});
    shape.StartAnimation(L"Size", size);
    sprite.Clip(compositor.CreateGeometricClip(shape));

    wuxh::ElementCompositionPreview::SetElementChildVisual(host, sprite);
    return shape;
}

// The plate is two layers in a grid: the blur at the bottom, and the tint and
// outline on top of it.
struct Plate {
    winrt::weak_ref<wux::UIElement> row;     // the icons' parent
    winrt::weak_ref<wuxc::Panel> panel;      // the row's parent; the plate lives here
    wuxc::Grid root{nullptr};                // gets positioned and sized
    wuxc::Border blurHost{nullptr};
    wuxc::Border tint{nullptr};
    // Invisible strip behind the icons, the dock's width and the taskbar's
    // full height. The buttons don't touch each other, so without it the
    // pointer "leaves" the dock in the gaps between them.
    wuxc::Border hitArea{nullptr};
    wuc::CompositionRoundedRectangleGeometry blurShape{nullptr};
    float appliedBlur = -1;
    float baseLeft = 0, baseWidth = 0;       // size when nothing is magnified
    float top = 0;
    float left = -1, width = -1;             // last applied
    float offsetY = 0;                       // how far the row's icons are moved down
    float dockBottom = 0;                    // bottom edge of the dock, in the row's coordinates
    wux::FrameworkElement::SizeChanged_revoker rowSizeChanged;
    wux::FrameworkElement::SizeChanged_revoker panelSizeChanged;
};

[[clang::no_destroy]] std::list<Plate> g_plates;

Plate* GetRowPlate(wux::DependencyObject const& row) {
    if (!row) return nullptr;
    for (auto& p : g_plates) {
        if (p.row.get() == row) return &p;
    }
    return nullptr;
}

// How far the icons in `row` are moved down to sit on its dock plate.
float GetRowOffsetY(wux::DependencyObject const& row) {
    Plate* p = GetRowPlate(row);
    return p ? p->offsetY : 0;
}

// Frames left in which to re-measure the plates. Set when buttons come or go,
// and counted down over a few frames so the taskbar has laid them out first.
int g_platesDirtyFrames = 0;

void StartFrames();
void SubscribeHitArea(wux::FrameworkElement const& el);

void MarkPlatesDirty() {
    g_platesDirtyFrames = 3;
    StartFrames();
}

// Frosted glass: the desktop behind the dock, blurred (Dock Blur), under a
// translucent color (Dock Tint).
void ApplyPlateLook(Plate& p) {
    uint8_t alpha = (uint8_t)(g_dockTint * 255 / 100);
    p.tint.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Color{alpha, 60, 60, 65}));
    p.tint.BorderBrush(wuxm::SolidColorBrush(winrt::Windows::UI::Color{40, 255, 255, 255}));
    p.tint.BorderThickness({1, 1, 1, 1});
    p.tint.CornerRadius({g_dockRadius, g_dockRadius, g_dockRadius, g_dockRadius});

    float blur = g_dockBlur * kMaxBlur / 100;
    if (blur != p.appliedBlur) {
        p.appliedBlur = blur;
        try {
            p.blurShape = SetBlurVisual(p.blurHost, blur);
        } catch (...) {
            p.blurShape = nullptr;
            Wh_Log(L"Dock blur failed: 0x%08X %s", winrt::to_hresult(),
                   winrt::to_message().c_str());
        }
    } else if (p.blurShape) {
        p.blurShape.CornerRadius({g_dockRadius, g_dockRadius});
    }
}

void EnsurePlate(wux::FrameworkElement const& button) {
    auto row = wuxm::VisualTreeHelper::GetParent(button).try_as<wux::FrameworkElement>();
    if (!row) return;
    for (auto& p : g_plates) {
        if (p.row.get() == row) return;
    }
    auto rowParent = wuxm::VisualTreeHelper::GetParent(row);
    Wh_Log(L"[plate] row: %s #%s, row parent: %s", winrt::get_class_name(row).c_str(),
           row.Name().c_str(), rowParent ? winrt::get_class_name(rowParent).c_str() : L"(none)");
    auto panel = rowParent ? rowParent.try_as<wuxc::Panel>() : nullptr;
    if (!panel) {
        Wh_Log(L"[plate] Icon row's parent isn't a panel; no dock plate");
        return;
    }

    wuxc::Grid root;
    root.IsHitTestVisible(false);
    root.HorizontalAlignment(wux::HorizontalAlignment::Left);
    root.VerticalAlignment(wux::VerticalAlignment::Top);
    root.Visibility(wux::Visibility::Collapsed);
    if (panel.try_as<wuxc::Grid>()) {
        // Span the whole grid so the margin is measured from its top-left.
        wuxc::Grid::SetRowSpan(root, 100);
        wuxc::Grid::SetColumnSpan(root, 100);
    }
    wuxc::Border blurHost;
    wuxc::Border tint;
    root.Children().Append(blurHost);
    root.Children().Append(tint);  // later children draw on top

    // A transparent background makes it catch the pointer; invisible otherwise.
    wuxc::Border hitArea;
    hitArea.Background(wuxm::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0}));
    hitArea.HorizontalAlignment(wux::HorizontalAlignment::Left);
    hitArea.VerticalAlignment(wux::VerticalAlignment::Stretch);
    hitArea.Visibility(wux::Visibility::Collapsed);
    if (panel.try_as<wuxc::Grid>()) {
        wuxc::Grid::SetRowSpan(hitArea, 100);
        wuxc::Grid::SetColumnSpan(hitArea, 100);
    }

    // Insert both right before the icon row: drawn above whatever comes
    // earlier (the taskbar background), below the icons, which get the
    // pointer first wherever they are.
    uint32_t index = 0;
    if (!panel.Children().IndexOf(row, index)) index = 0;
    panel.Children().InsertAt(index, root);
    panel.Children().InsertAt(index, hitArea);
    Wh_Log(L"[plate] inserted at index %u of %u children", index, panel.Children().Size());

    auto& p = g_plates.emplace_back();
    p.row = winrt::make_weak(row.as<wux::UIElement>());
    p.panel = winrt::make_weak(panel);
    p.root = root;
    p.blurHost = blurHost;
    p.tint = tint;
    p.hitArea = hitArea;
    ApplyPlateLook(p);
    SubscribeHitArea(hitArea);

    auto onSize = [](wf::IInspectable const&, wux::SizeChangedEventArgs const&) {
        MarkPlatesDirty();
    };
    p.rowSizeChanged = row.SizeChanged(winrt::auto_revoke, onSize);
    p.panelSizeChanged = panel.SizeChanged(winrt::auto_revoke, onSize);
    Wh_Log(L"Dock plate added");
}

// Re-applies a button's transform with its current magnification.
void ReapplyButton(wux::FrameworkElement const& button) {
    float s = 1.0f, tx = 0;
    for (auto& item : g_row) {
        if (item.element.get() == button) {
            s = item.scale;
            tx = item.shift;
            break;
        }
    }
    SetScale(button, s, tx);
}

// Re-measure the plate around the icons, as if nothing were magnified.
void MeasurePlate(Plate& p) {
    auto row = p.row.get();
    auto panel = p.panel.get();
    if (!row || !panel || !p.root) return;

    auto icons = GetIcons(row);
    if (icons.empty()) {
        Wh_Log(L"[plate] measure: no visible icons");
        p.root.Visibility(wux::Visibility::Collapsed);
        if (p.hitArea) p.hitArea.Visibility(wux::Visibility::Collapsed);
        return;
    }

    float minLeft = icons[0].left, maxRight = icons[0].left + icons[0].width;
    float minTop = icons[0].top, maxBottom = icons[0].top + icons[0].height;
    for (auto& icon : icons) {
        minLeft = std::min(minLeft, icon.left);
        maxRight = std::max(maxRight, icon.left + icon.width);
        minTop = std::min(minTop, icon.top);
        maxBottom = std::max(maxBottom, icon.top + icon.height);
    }

    // Horizontally: the buttons are wider than their icons (empty space on
    // each side), so pull the edges in by that much, then add the padding
    // measured from the icons themselves.
    float avgWidth = 0;
    for (auto& icon : icons) avgWidth += icon.width;
    avgWidth /= icons.size();
    float emptyPerSide = std::max(0.0f, (avgWidth - g_iconSize) / 2);
    float padX = g_dockSidePadding - emptyPerSide;
    auto origin = row.TransformToVisual(panel).TransformPoint({0, 0});
    p.baseLeft = origin.X + minLeft - padX;
    p.baseWidth = (maxRight - minLeft) + 2 * padX;

    // Vertically: the dock sits at the bottom of the taskbar, and the icons
    // are moved down onto its center. Layout can't do this: the taskbar
    // stretches its buttons to its full height and centers each icon in its
    // button, so the icons always end up in the middle of the taskbar.
    float height = g_dockHeight;
    p.top = (float)panel.ActualHeight() - g_dockBottomGap - height;
    float iconCenterY = origin.Y + (minTop + maxBottom) / 2;
    float offsetY = p.top + height / 2 - iconCenterY;

    // A new element's Height is NaN, and any comparison with NaN is false, so
    // check for it explicitly.
    double current = p.root.Height();
    if (std::isnan(current) || std::fabs(current - height) > 0.5) p.root.Height(height);
    p.left = p.width = -1;  // force ApplyPlate to write
    p.root.Visibility(wux::Visibility::Visible);
    if (p.hitArea) p.hitArea.Visibility(wux::Visibility::Visible);

    // Re-apply every icon's transform so the new offset and dock bottom take
    // effect, keeping any magnification currently in progress.
    p.offsetY = offsetY;
    p.dockBottom = p.top + height - origin.Y;
    for (auto& icon : icons) ReapplyButton(icon.element);

    Wh_Log(L"[plate] measure: %zu icons, left=%.1f top=%.1f width=%.1f height=%.1f, "
           L"icons moved %.1f, panel %.1fx%.1f",
           icons.size(), p.baseLeft, p.top, p.baseWidth, height, p.offsetY,
           panel.ActualWidth(), panel.ActualHeight());
}

// Position the plate, widened by `extra` px split evenly on both sides.
void ApplyPlate(Plate& p, float extra) {
    if (!p.root) return;
    float left = p.baseLeft - extra / 2;
    float width = p.baseWidth + extra;
    if (std::fabs(left - p.left) > 0.25f || std::fabs(width - p.width) > 0.25f) {
        p.root.Margin({left, p.top, 0, 0});
        p.root.Width(width);
        if (p.hitArea) {
            p.hitArea.Margin({left, 0, 0, 0});
            p.hitArea.Width(width);
        }
        p.left = left;
        p.width = width;
    }
}

void RemovePlates() {
    for (auto& p : g_plates) {
        p.rowSizeChanged.revoke();
        p.panelSizeChanged.revoke();
        if (p.blurHost) {
            try { wuxh::ElementCompositionPreview::SetElementChildVisual(p.blurHost, nullptr); }
            catch (...) {}
        }
        auto panel = p.panel.get();
        if (panel && p.root) {
            uint32_t index;
            if (panel.Children().IndexOf(p.root, index)) panel.Children().RemoveAt(index);
            if (p.hitArea && panel.Children().IndexOf(p.hitArea, index)) {
                panel.Children().RemoveAt(index);
            }
        }
    }
    g_plates.clear();
}

// ---- Running indicators ----
// The little pill under each running app. It's part of its button, so it
// moves and grows with it; PlaceIndicator undoes that, keeping it at its
// normal size, centered under the icon, just above the bottom of the dock.

struct Indicator {
    InstanceHandle handle = 0;
    winrt::weak_ref<wux::FrameworkElement> element;
    winrt::weak_ref<wux::FrameworkElement> button;
    wux::FrameworkElement::SizeChanged_revoker sizeChanged;
};

[[clang::no_destroy]] std::list<Indicator> g_indicators;

void PlaceIndicator(wux::FrameworkElement const& button, float s) {
    wux::FrameworkElement el{nullptr};
    for (auto& i : g_indicators) {
        if (i.button.get() == button) {
            el = i.element.get();
            break;
        }
    }
    if (!el) return;
    auto visual = wuxh::ElementCompositionPreview::GetElementVisual(el);
    wuxh::ElementCompositionPreview::SetIsTranslationEnabled(el, true);

    auto reset = [&] {
        visual.Scale({1, 1, 1});
        visual.Properties().InsertVector3(L"Translation", {0, 0, 0});
    };
    Plate* p = GetRowPlate(wuxm::VisualTreeHelper::GetParent(button));
    if (!p || s <= 0) return reset();

    // The indicator's vertical center within the button, as laid out.
    float w = (float)el.ActualWidth(), h = (float)el.ActualHeight();
    float centerY = h / 2;
    wux::DependencyObject current = el;
    while (current && current != button) {
        if (auto ui = current.try_as<wux::UIElement>()) centerY += ui.ActualOffset().y;
        current = wuxm::VisualTreeHelper::GetParent(current);
    }
    if (!current) return reset();  // not inside this button after all

    // Where it should be: just above the dock's bottom edge.
    auto slot = wuxc::Primitives::LayoutInformation::GetLayoutSlot(button);
    float buttonTop = slot.Y + (float)button.Margin().Top;
    float inset = std::max(2.0f, std::round(g_iconSize / 10));
    float targetY = p->dockBottom - buttonTop - inset - h / 2;

    // The button scales by s around its pivot, then moves down by offsetY.
    // Scale the indicator by 1/s around its own center to cancel the growth,
    // and move it so that after the button's transform it lands on targetY.
    float pivotY = ButtonPivotY(button);
    float ty = (targetY - p->offsetY - pivotY) / s + pivotY - centerY;
    visual.CenterPoint({w / 2, h / 2, 0});
    visual.Scale({1 / s, 1 / s, 1});
    visual.Properties().InsertVector3(L"Translation", {0, ty, 0});
}

void TrackIndicator(InstanceHandle handle, wux::FrameworkElement const& el) {
    auto button = FindTaskbarButton(el, 6);
    if (!button) return;
    for (auto& i : g_indicators) {
        if (i.handle == handle) return;
    }
    auto& i = g_indicators.emplace_back();
    i.handle = handle;
    i.element = winrt::make_weak(el);
    i.button = winrt::make_weak(button);
    // Laid out for the first time, or wider for the active app: re-place it.
    i.sizeChanged = el.SizeChanged(
        winrt::auto_revoke, [weakButton = i.button](wf::IInspectable const&,
                                                    wux::SizeChangedEventArgs const&) {
            try {
                if (auto b = weakButton.get()) ReapplyButton(b);
            } catch (...) {}
        });
}

void RestoreIndicators() {
    for (auto& i : g_indicators) {
        i.sizeChanged.revoke();
        if (auto el = i.element.get()) {
            try {
                auto visual = wuxh::ElementCompositionPreview::GetElementVisual(el);
                visual.Scale({1, 1, 1});
                visual.Properties().InsertVector3(L"Translation", {0, 0, 0});
                wuxh::ElementCompositionPreview::SetIsTranslationEnabled(el, false);
            } catch (...) {}
        }
    }
    g_indicators.clear();
}

// (Re)builds the list of icons in `parent`. Icons that were already in the
// list keep their current size, so rebuilding mid-hover doesn't jump; icons
// that dropped out (e.g. the pointer moved to another monitor's taskbar) go
// back to normal size right away.
void BuildRow(wux::UIElement const& parent) {
    std::vector<RowItem> old = std::move(g_row);
    g_row.clear();
    g_rowParent = nullptr;

    if (parent) {
        for (auto& icon : GetIcons(parent)) {
            // The margins change how far apart buttons sit, so spread by the
            // space each one actually takes up rather than its own width.
            float pitch = std::max(1.0f, icon.width + 2 * g_buttonMargin);
            RowItem item{winrt::make_weak(icon.element), icon.left + icon.width / 2, pitch};
            for (auto& o : old) {
                if (o.element.get() == icon.element) {
                    item.scale = o.scale;
                    item.shift = o.shift;
                    o.element = nullptr;  // still in the row; don't reset it below
                    break;
                }
            }
            g_row.push_back(item);
        }
        // Left to right, so the spreading below shifts icons in the right order.
        std::sort(g_row.begin(), g_row.end(),
                  [](RowItem const& a, RowItem const& b) { return a.center < b.center; });
        g_rowParent = winrt::make_weak(parent);
    }

    for (auto& o : old) {
        if (auto icon = o.element.get()) SetScale(icon, 1.0f);
    }
}

// ---- Once-per-frame update ----
// CompositionTarget::Rendering fires once per screen refresh. We only
// subscribe while icons are moving toward their target size.

winrt::event_token g_frameToken{};
bool g_frameRunning = false;
std::chrono::steady_clock::time_point g_lastFrame;

void StopFrames() {
    if (!g_frameRunning) return;
    wuxm::CompositionTarget::Rendering(g_frameToken);
    g_frameRunning = false;
}

void OnFrame() {
    auto now = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(now - g_lastFrame).count();
    g_lastFrame = now;
    if (dt > 0.1f) dt = 0.1f;

    // How far to move toward the target this frame. animationMs is roughly
    // the time to get (almost) all the way there.
    float alpha = 1.0f;
    if (g_animationMs > 0) {
        float tau = g_animationMs / 1000.0f / 3.0f;
        alpha = 1.0f - std::exp(-dt / tau);
    }

    // 0. Create missing plates and re-measure them if buttons recently came,
    //    went or were laid out. Done here rather than when buttons are
    //    reported, because the taskbar doesn't like its tree being changed
    //    mid-report.
    if (g_platesDirtyFrames > 0) {
        g_platesDirtyFrames--;
        for (auto& b : g_buttons) {
            if (auto el = b.element.get()) {
                try { EnsurePlate(el); } catch (...) {
                    Wh_Log(L"EnsurePlate failed: 0x%08X", winrt::to_hresult());
                }
            }
        }
        for (auto& p : g_plates) MeasurePlate(p);
    }

    // 0b. Re-read where the row's icons are, in their current order, so the
    //     magnification follows apps that opened, closed or were dragged to
    //     a new spot. Sizes in progress are kept.
    if (auto rowParent = g_rowParent.get()) BuildRow(rowParent);

    // 1. Move each icon's scale toward its target.
    bool settled = g_platesDirtyFrames == 0;
    std::vector<float> scales(g_row.size(), 1.0f);
    for (size_t i = 0; i < g_row.size(); i++) {
        auto& item = g_row[i];

        float target = 1.0f;
        if (g_pointerOver) {
            float d = std::fabs(g_pointerX - item.center);
            if (d < g_radius) {
                // Cosine falloff: full size under the pointer, normal at the radius.
                float k = (std::cos(d / g_radius * 3.14159265f) + 1.0f) / 2.0f;
                target = 1.0f + (g_scale - 1.0f) * k;
            }
        }

        float s = item.scale + (target - item.scale) * alpha;
        if (std::fabs(target - s) < 0.001f) {
            s = target;
        } else {
            settled = false;
        }
        scales[i] = s;
    }

    // 2. Spread: each icon is pushed right by the extra width of the icons
    //    before it, then the whole row is pulled back left by half the total,
    //    so it grows outward evenly on both sides.
    float total = 0;
    for (size_t i = 0; i < g_row.size(); i++) {
        total += (scales[i] - 1.0f) * g_row[i].width;
    }

    float before = 0;
    for (size_t i = 0; i < g_row.size(); i++) {
        auto& item = g_row[i];
        float extra = (scales[i] - 1.0f) * item.width;
        float shift = before + extra / 2 - total / 2;
        before += extra;

        auto icon = item.element.get();
        if (!icon) continue;
        if (scales[i] != item.scale || std::fabs(shift - item.shift) > 0.01f) {
            SetScale(icon, scales[i], shift);
            item.scale = scales[i];
            item.shift = shift;
        }
    }

    // 3. Widen the plate under the current row by the same total.
    auto rowParent = g_rowParent.get();
    for (auto& p : g_plates) {
        ApplyPlate(p, rowParent && p.row.get() == rowParent ? total : 0);
    }

    if (settled) StopFrames();
}

void StartFrames() {
    if (g_frameRunning) return;
    g_lastFrame = std::chrono::steady_clock::now();
    g_frameToken = wuxm::CompositionTarget::Rendering(
        [](wf::IInspectable const&, wf::IInspectable const&) {
            try {
                OnFrame();
            } catch (...) {
                Wh_Log(L"OnFrame error: 0x%08X %s", winrt::to_hresult(),
                       winrt::to_message().c_str());
                StopFrames();
            }
        });
    g_frameRunning = true;
}

// ---- Pointer events: just record where the pointer is ----

// The pointer is over the row of icons `row` (on a button, or in the dock
// between them).
void OnPointerOverRow(wux::UIElement const& row, wuxi::PointerRoutedEventArgs const& e) {
    if (g_rowParent.get() != row) {
        BuildRow(row);
        if (!g_rowParent.get()) return;
    }
    g_pointerX = e.GetCurrentPoint(row).Position().X;
    g_pointerOver = true;
    StartFrames();
}

// The pointer is over a button.
void OnPointer(wux::FrameworkElement const& el, wuxi::PointerRoutedEventArgs const& e) {
    auto row = wuxm::VisualTreeHelper::GetParent(el).try_as<wux::UIElement>();
    if (!row) return;
    OnPointerOverRow(row, e);
}

void OnLeave(wux::FrameworkElement const&) {
    // If the pointer moved onto the next icon, or into the dock between
    // icons, its enter event right after this sets g_pointerOver back to true
    // before the next frame.
    g_pointerOver = false;
    StartFrames();
}

// Shared handlers. They must be the same objects for AddHandler/RemoveHandler.
[[clang::no_destroy]] wf::IInspectable g_enterHandler{nullptr};
[[clang::no_destroy]] wf::IInspectable g_leaveHandler{nullptr};
[[clang::no_destroy]] wf::IInspectable g_hitAreaHandler{nullptr};

void EnsureHandlers() {
    if (g_enterHandler) return;
    // Used for both PointerEntered and PointerMoved.
    g_enterHandler = winrt::box_value(wuxi::PointerEventHandler(
        [](wf::IInspectable const& sender, wuxi::PointerRoutedEventArgs const& e) {
            try {
                if (auto el = sender.try_as<wux::FrameworkElement>()) OnPointer(el, e);
            } catch (...) {}
        }));
    // Pointer over a plate's hit area: treat it as being over that row.
    g_hitAreaHandler = winrt::box_value(wuxi::PointerEventHandler(
        [](wf::IInspectable const& sender, wuxi::PointerRoutedEventArgs const& e) {
            try {
                for (auto& p : g_plates) {
                    if (p.hitArea && p.hitArea == sender) {
                        if (auto row = p.row.get()) OnPointerOverRow(row, e);
                        return;
                    }
                }
            } catch (...) {}
        }));
    g_leaveHandler = winrt::box_value(wuxi::PointerEventHandler(
        [](wf::IInspectable const& sender, wuxi::PointerRoutedEventArgs const&) {
            try {
                if (auto el = sender.try_as<wux::FrameworkElement>()) OnLeave(el);
            } catch (...) {}
        }));
}

void Subscribe(wux::FrameworkElement const& el, bool add) {
    // handledEventsToo = true: the button marks its own pointer events as
    // handled, which would otherwise hide them from us.
    if (add) {
        el.AddHandler(wux::UIElement::PointerEnteredEvent(), g_enterHandler, true);
        el.AddHandler(wux::UIElement::PointerMovedEvent(), g_enterHandler, true);
        el.AddHandler(wux::UIElement::PointerExitedEvent(), g_leaveHandler, true);
        el.AddHandler(wux::UIElement::PointerCanceledEvent(), g_leaveHandler, true);
        el.AddHandler(wux::UIElement::PointerCaptureLostEvent(), g_leaveHandler, true);
    } else {
        el.RemoveHandler(wux::UIElement::PointerEnteredEvent(), g_enterHandler);
        el.RemoveHandler(wux::UIElement::PointerMovedEvent(), g_enterHandler);
        el.RemoveHandler(wux::UIElement::PointerExitedEvent(), g_leaveHandler);
        el.RemoveHandler(wux::UIElement::PointerCanceledEvent(), g_leaveHandler);
        el.RemoveHandler(wux::UIElement::PointerCaptureLostEvent(), g_leaveHandler);
    }
}

// A plate's hit area reports the pointer for its row (see EnsurePlate).
void SubscribeHitArea(wux::FrameworkElement const& el) {
    EnsureHandlers();
    el.AddHandler(wux::UIElement::PointerEnteredEvent(), g_hitAreaHandler, true);
    el.AddHandler(wux::UIElement::PointerMovedEvent(), g_hitAreaHandler, true);
    el.AddHandler(wux::UIElement::PointerExitedEvent(), g_leaveHandler, true);
    el.AddHandler(wux::UIElement::PointerCanceledEvent(), g_leaveHandler, true);
}

void OnButtonAdded(InstanceHandle handle, wux::FrameworkElement const& el) {
    for (auto& b : g_buttons) {
        if (b.handle == handle) return;
    }
    EnsureHandlers();
    Subscribe(el, true);
    auto& b = g_buttons.emplace_back();
    b.handle = handle;
    b.element = winrt::make_weak(el);
    b.originalMargin = el.ReadLocalValue(wux::FrameworkElement::MarginProperty());
    // A button for a newly opened app is still being set up when we see it;
    // if the taskbar sets its own margin afterwards, put the dock's back.
    b.marginToken = el.RegisterPropertyChangedCallback(
        wux::FrameworkElement::MarginProperty(),
        [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
            try {
                if (auto fe = sender.try_as<wux::FrameworkElement>()) ApplySpacing(fe);
            } catch (...) {}
        });
    // It's also laid out (or resized) later than the plates' few-frame
    // re-measure; re-measure again when it is, so it gets moved onto the dock.
    b.sizeChanged = el.SizeChanged(
        winrt::auto_revoke,
        [](wf::IInspectable const&, wux::SizeChangedEventArgs const&) { MarkPlatesDirty(); });
    ApplySpacing(el);
    MarkPlatesDirty();  // the plate is created on the next frame
}

// Undoes OnButtonAdded for one button. Also matters when the taskbar later
// reuses the same button for another app: it starts from a clean state.
void ReleaseButton(Button& b) {
    b.sizeChanged.revoke();
    auto el = b.element.get();
    if (!el) return;
    Subscribe(el, false);
    if (b.marginToken) {
        el.UnregisterPropertyChangedCallback(wux::FrameworkElement::MarginProperty(),
                                             b.marginToken);
        b.marginToken = 0;
    }
    RestoreSpacing(b);
}

void OnButtonRemoved(InstanceHandle handle) {
    bool removed = false;
    g_buttons.remove_if([&](Button& b) {
        if (b.handle != handle) return false;
        try { ReleaseButton(b); } catch (...) {}
        removed = true;
        return true;
    });
    if (removed) MarkPlatesDirty();
}

void ResetAll() {
    StopFrames();
    try { RemovePlates(); } catch (...) {}
    RestoreBackgrounds();
    RestoreHighlights();
    RestoreIconImages();
    for (auto& b : g_buttons) {
        if (auto el = b.element.get()) {
            try {
                ReleaseButton(b);
                // Clears animations left over from earlier versions of the mod.
                wuxh::ElementCompositionPreview::GetElementVisual(el).ImplicitAnimations(nullptr);
                SetScale(el, 1.0f, 0);
                wuxh::ElementCompositionPreview::SetIsTranslationEnabled(el, false);
            } catch (...) {}
        }
    }
    RestoreIndicators();
    g_buttons.clear();
    g_row.clear();
    g_rowParent = nullptr;
    g_enterHandler = nullptr;
    g_leaveHandler = nullptr;
    g_hitAreaHandler = nullptr;
}

// ---------------------------------------------------------------------------
// XAML diagnostics plumbing (attaches to the taskbar's visual tree)
// ---------------------------------------------------------------------------

thread_local bool g_reportCompositionDiagAsDisabled = false;

// The thread subscribing to the taskbar's visual tree (see AdviseAsync). The
// mod waits for it before unloading, so it never runs code that's gone.
std::atomic<HANDLE> g_adviseThread = nullptr;

class VisualTreeWatcher
    : public winrt::implements<VisualTreeWatcher, IVisualTreeServiceCallback2, winrt::non_agile> {
public:
    explicit VisualTreeWatcher(winrt::com_ptr<::IUnknown> const& site)
        : m_diag(site.as<IXamlDiagnostics>()) {}

    void AdviseAsync() {
        auto* holder = new winrt::com_ptr<VisualTreeWatcher>();
        holder->copy_from(this);
        HANDLE t = CreateThread(nullptr, 0, [](LPVOID p) -> DWORD {
            auto* holder = static_cast<winrt::com_ptr<VisualTreeWatcher>*>(p);
            try {
                g_reportCompositionDiagAsDisabled = true;
                HRESULT hr = (*holder)->m_diag.as<IVisualTreeService3>()
                                 ->AdviseVisualTreeChange(holder->get());
                g_reportCompositionDiagAsDisabled = false;
                Wh_Log(L"AdviseVisualTreeChange: 0x%08X", hr);
            } catch (...) {
                g_reportCompositionDiagAsDisabled = false;
            }
            delete holder;
            return 0;
        }, holder, 0, nullptr);
        if (!t) {
            delete holder;
            return;
        }
        // Kept so Wh_ModUninit can wait for it (an earlier one, if any, is
        // long finished).
        if (HANDLE old = g_adviseThread.exchange(t)) CloseHandle(old);
    }

    void Unadvise() {
        try { m_diag.as<IVisualTreeService3>()->UnadviseVisualTreeChange(this); } catch (...) {}
    }

    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation, VisualElement element,
                                                 VisualMutationType mutation) override {
        try {
            std::wstring_view type = element.Type ? element.Type : L"";
            if (mutation == Add) {
                bool isButton = type == L"Taskbar.TaskListButton" ||
                                type == L"Taskbar.ExperienceToggleButton";
                bool isBackground = type == L"Taskbar.TaskbarBackground";
                std::wstring_view name = element.Name ? element.Name : L"";
                bool isHighlight = type == L"Windows.UI.Xaml.Controls.Border" &&
                                   (name == L"BackgroundElement" || name == L"MultiWindowElement");
                bool isIcon = name == L"Icon";
                bool isIndicator = name == L"RunningIndicator";
                if (!isButton && !isBackground && !isHighlight && !isIcon && !isIndicator) {
                    return S_OK;
                }

                wf::IInspectable insp;
                winrt::check_hresult(m_diag->GetIInspectableFromHandle(
                    element.Handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(insp))));
                if (auto fe = insp.try_as<wux::FrameworkElement>()) {
                    if (isButton) OnButtonAdded(element.Handle, fe);
                    else if (isBackground) HideBackground(element.Handle, fe);
                    else if (isHighlight) HideHighlight(fe);
                    else if (isIcon) TrackIconImage(element.Handle, fe);
                    else TrackIndicator(element.Handle, fe);
                }
            } else if (mutation == Remove) {
                OnButtonRemoved(element.Handle);
                // Put removed elements back the way they were: the taskbar
                // may reuse them for another app, and they're tracked afresh
                // when they come back.
                g_backgrounds.remove_if([&](HiddenBackground const& b) {
                    if (b.handle != element.Handle) return false;
                    RestoreHidden(b);
                    return true;
                });
                g_iconImages.remove_if([&](IconImage const& i) {
                    if (i.handle != element.Handle) return false;
                    ReleaseIconImage(i);
                    return true;
                });
                g_indicators.remove_if(
                    [&](Indicator const& i) { return i.handle == element.Handle; });
            }
        } catch (...) {
            Wh_Log(L"OnVisualTreeChange error: 0x%08X", winrt::to_hresult());
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle, VisualElementState,
                                                    LPCWSTR) override {
        return S_OK;
    }

private:
    winrt::com_ptr<IXamlDiagnostics> m_diag;
};

[[clang::no_destroy]] winrt::com_ptr<VisualTreeWatcher> g_watcher;

// {C1B6E4A2-58D3-4F7E-9A21-6E0B3D7C9F45}
constexpr CLSID CLSID_DockTAP = {
    0xc1b6e4a2, 0x58d3, 0x4f7e, {0x9a, 0x21, 0x6e, 0x0b, 0x3d, 0x7c, 0x9f, 0x45}};

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCWSTR)&GetCurrentModuleHandle, &module)) {
        return nullptr;
    }
    return module;
}

class DockTAP : public winrt::implements<DockTAP, IObjectWithSite, winrt::non_agile> {
public:
    HRESULT STDMETHODCALLTYPE SetSite(::IUnknown* site) override {
        try {
            if (g_watcher) { g_watcher->Unadvise(); g_watcher = nullptr; }
            m_site.copy_from(site);
            if (m_site) {
                FreeLibrary(GetCurrentModuleHandle());  // balance IXDE's LoadLibrary
                g_watcher = winrt::make_self<VisualTreeWatcher>(m_site);
                g_watcher->AdviseAsync();
            }
            return S_OK;
        } catch (...) {
            return winrt::to_hresult();
        }
    }
    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void** ppv) override {
        if (!m_site) { *ppv = nullptr; return E_FAIL; }
        return m_site->QueryInterface(riid, ppv);
    }
private:
    winrt::com_ptr<::IUnknown> m_site;
};

class DockTAPFactory : public winrt::implements<DockTAPFactory, IClassFactory, winrt::non_agile> {
public:
    HRESULT STDMETHODCALLTYPE CreateInstance(::IUnknown* outer, REFIID riid, void** ppv) override {
        try {
            *ppv = nullptr;
            if (outer) return CLASS_E_NOAGGREGATION;
            return winrt::make_self<DockTAP>().as<IObjectWithSite>()->QueryInterface(riid, ppv);
        } catch (...) {
            return winrt::to_hresult();
        }
    }
    HRESULT STDMETHODCALLTYPE LockServer(BOOL) override { return S_OK; }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"
extern "C" __declspec(dllexport) HRESULT STDAPICALLTYPE
DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) {
    try {
        if (rclsid == CLSID_DockTAP) {
            *ppv = nullptr;
            return winrt::make_self<DockTAPFactory>().as<IClassFactory>()->QueryInterface(riid, ppv);
        }
        return CLASS_E_CLASSNOTAVAILABLE;
    } catch (...) {
        return winrt::to_hresult();
    }
}
#pragma clang diagnostic pop

bool g_tapInjected = false;

void InjectTAP() {
    if (g_tapInjected) return;

    WCHAR path[MAX_PATH];
    if (!GetModuleFileName(GetCurrentModuleHandle(), path, ARRAYSIZE(path))) return;

    HMODULE hXaml = LoadLibraryEx(L"Windows.UI.Xaml.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hXaml) return;

    using IXDE_t = HRESULT(WINAPI*)(LPCWSTR, DWORD, LPCWSTR, LPCWSTR, CLSID, LPCWSTR);
    auto ixde = (IXDE_t)GetProcAddress(hXaml, "InitializeXamlDiagnosticsEx");
    if (!ixde) return;

    HRESULT hr = E_FAIL;
    for (int i = 1; i <= 10000; i++) {
        WCHAR name[64];
        swprintf(name, ARRAYSIZE(name), L"VisualDiagConnection%d", i);
        hr = ixde(name, GetCurrentProcessId(), L"", path, CLSID_DockTAP, nullptr);
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) break;
    }
    Wh_Log(L"InitializeXamlDiagnosticsEx: 0x%08X", hr);
    if (SUCCEEDED(hr)) g_tapInjected = true;
}

// ---------------------------------------------------------------------------
// Helpers / hooks
// ---------------------------------------------------------------------------

using RunProc_t = void (*)(void*);

bool RunFromWindowThread(HWND hWnd, RunProc_t proc, void* param) {
    static const UINT msg = RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    struct Param { RunProc_t proc; void* param; };

    DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
    if (!tid) return false;
    if (tid == GetCurrentThreadId()) { proc(param); return true; }

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, [](int code, WPARAM w, LPARAM l) -> LRESULT {
        if (code == HC_ACTION) {
            auto* cwp = (const CWPSTRUCT*)l;
            if (cwp->message == msg) {
                auto* p = (Param*)cwp->lParam;
                p->proc(p->param);
            }
        }
        return CallNextHookEx(nullptr, code, w, l);
    }, nullptr, tid);
    if (!hook) return false;

    Param p{proc, param};
    SendMessage(hWnd, msg, 0, (LPARAM)&p);
    UnhookWindowsHookEx(hook);
    return true;
}

HWND FindTaskbar() {
    HWND result = nullptr;
    EnumWindows([](HWND hWnd, LPARAM l) -> BOOL {
        DWORD pid; WCHAR cls[32];
        if (GetWindowThreadProcessId(hWnd, &pid) && pid == GetCurrentProcessId() &&
            GetClassName(hWnd, cls, ARRAYSIZE(cls)) && _wcsicmp(cls, L"Shell_TrayWnd") == 0) {
            *(HWND*)l = hWnd;
            return FALSE;
        }
        return TRUE;
    }, (LPARAM)&result);
    return result;
}

// Catch the taskbar's XAML island being created (e.g. after Explorer restarts).
using CreateWindowInBand_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int,
                                           HWND, HMENU, HINSTANCE, PVOID, DWORD);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD ex, LPCWSTR cls, LPCWSTR name, DWORD style, int x,
                                    int y, int w, int h, HWND parent, HMENU menu,
                                    HINSTANCE inst, PVOID param, DWORD band) {
    HWND hWnd = CreateWindowInBand_Original(ex, cls, name, style, x, y, w, h, parent, menu,
                                            inst, param, band);
    if (hWnd && parent && !g_tapInjected && ((ULONG_PTR)cls & ~(ULONG_PTR)0xFFFF) &&
        _wcsicmp(cls, L"Windows.UI.Composition.DesktopWindowContentBridge") == 0) {
        WCHAR parentCls[32];
        if (GetClassName(parent, parentCls, ARRAYSIZE(parentCls)) &&
            _wcsicmp(parentCls, L"Shell_TrayWnd") == 0) {
            InjectTAP();
        }
    }
    return hWnd;
}

// Report XAML composition diagnostics as disabled while advising; they aren't
// thread-safe and can corrupt Explorer's heap (same as Taskbar Styler).
using RegOpenKeyExW_t = decltype(&RegOpenKeyExW);
RegOpenKeyExW_t RegOpenKeyExW_Original;
LSTATUS WINAPI RegOpenKeyExW_Hook(HKEY key, LPCWSTR sub, DWORD opt, REGSAM sam, PHKEY out) {
    LSTATUS r = RegOpenKeyExW_Original(key, sub, opt, sam, out);
    if (r == ERROR_SUCCESS || !g_reportCompositionDiagAsDisabled || key != HKEY_LOCAL_MACHINE ||
        !sub || _wcsicmp(sub, L"Software\\Microsoft\\XAML\\Debug") != 0) {
        return r;
    }
    return RegOpenKeyExW_Original(HKEY_LOCAL_MACHINE, L"Software\\Microsoft", opt, sam, out);
}

using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExW_Original;
LSTATUS WINAPI RegQueryValueExW_Hook(HKEY key, LPCWSTR name, LPDWORD res, LPDWORD type,
                                     LPBYTE data, LPDWORD size) {
    if (!g_reportCompositionDiagAsDisabled || !name ||
        _wcsicmp(name, L"DisableCompositionDiag") != 0) {
        return RegQueryValueExW_Original(key, name, res, type, data, size);
    }
    if (type) *type = REG_DWORD;
    if (data && (!size || *size < sizeof(DWORD))) {
        if (size) *size = sizeof(DWORD);
        return ERROR_MORE_DATA;
    }
    if (data) *(DWORD*)data = 1;
    if (size) *size = sizeof(DWORD);
    return ERROR_SUCCESS;
}

// ---------------------------------------------------------------------------
// Taskbar height
// ---------------------------------------------------------------------------
// The taskbar window's height is decided by Explorer's own code, not by its
// XAML, so these hook the functions that report and apply it. Same strategy
// as m417z's "Taskbar height and icon size" mod, trimmed to the height only.
// Every hook passes straight through when g_taskbarHeight is 0.

std::atomic<bool> g_applyingHeight = false;

// enumTaskbarSize: 0 = small, 1 = regular, 2 = large.
bool OverrideHeight(int enumTaskbarSize) {
    return g_taskbarHeight > 0 && (enumTaskbarSize == 1 || enumTaskbarSize == 2);
}

using VoidMethod_t = void(WINAPI*)(void* pThis);
using SetDouble_t = void(WINAPI*)(void* pThis, double value);

// ---- Height of the taskbar window ----

using TaskbarConfiguration_GetFrameSize_t = double(WINAPI*)(int enumTaskbarSize);
TaskbarConfiguration_GetFrameSize_t TaskbarConfiguration_GetFrameSize_Original;
double WINAPI TaskbarConfiguration_GetFrameSize_Hook(int enumTaskbarSize) {
    if (OverrideHeight(enumTaskbarSize)) return g_taskbarHeight;
    return TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
}

// ---- Height of the system tray (clock, notification area) ----

using SystemTray_GetFrameSize_t = double(WINAPI*)(void* pThis, int enumTaskbarSize);
SystemTray_GetFrameSize_t SystemTrayController_GetFrameSize_Original;
double WINAPI SystemTrayController_GetFrameSize_Hook(void* pThis, int enumTaskbarSize) {
    if (OverrideHeight(enumTaskbarSize)) return g_taskbarHeight;
    return SystemTrayController_GetFrameSize_Original(pThis, enumTaskbarSize);
}

// Same, for the taskbars on other monitors.
SystemTray_GetFrameSize_t SystemTraySecondaryController_GetFrameSize_Original;
double WINAPI SystemTraySecondaryController_GetFrameSize_Hook(void* pThis, int enumTaskbarSize) {
    if (OverrideHeight(enumTaskbarSize)) return g_taskbarHeight;
    return SystemTraySecondaryController_GetFrameSize_Original(pThis, enumTaskbarSize);
}

// While the tray resizes itself, make its frame exactly the taskbar's height.
thread_local bool g_inTrayUpdateFrameSize = false;

VoidMethod_t SystemTrayController_UpdateFrameSize_Original;
void WINAPI SystemTrayController_UpdateFrameSize_Hook(void* pThis) {
    g_inTrayUpdateFrameSize = true;
    SystemTrayController_UpdateFrameSize_Original(pThis);
    g_inTrayUpdateFrameSize = false;
}

VoidMethod_t SystemTraySecondaryController_UpdateFrameSize_Original;
void WINAPI SystemTraySecondaryController_UpdateFrameSize_Hook(void* pThis) {
    g_inTrayUpdateFrameSize = true;
    SystemTraySecondaryController_UpdateFrameSize_Original(pThis);
    g_inTrayUpdateFrameSize = false;
}

SetDouble_t SystemTrayFrame_Height_Original;
void WINAPI SystemTrayFrame_Height_Hook(void* pThis, double value) {
    if (g_inTrayUpdateFrameSize && g_taskbarHeight > 0) value = g_taskbarHeight;
    SystemTrayFrame_Height_Original(pThis, value);
}

// ---- The XAML frame inside the taskbar window ----

// Without lifting MaxHeight, the frame stays at the stock height.
SetDouble_t TaskbarFrame_MaxHeight_Original;

// Windows 11 22H2/23H2 set the frame's height through this.
SetDouble_t TaskbarFrame_Height_Original;
void WINAPI TaskbarFrame_Height_Hook(void* pThis, double value) {
    if (TaskbarFrame_MaxHeight_Original) {
        TaskbarFrame_MaxHeight_Original(pThis, std::numeric_limits<double>::infinity());
    }
    TaskbarFrame_Height_Original(pThis, value);
}

// Windows 11 24H2 and newer set it in TaskbarController::UpdateFrameHeight.
// The controller keeps its TaskbarFrame at an offset that's read from the
// start of OnGroupingModeChanged:  sub rsp, 28h / mov rax|r8, [rcx+offset]
void* TaskbarController_OnGroupingModeChanged_Address;

LONG GetTaskbarFrameOffset() {
    static LONG offset = []() -> LONG {
        const BYTE* p = (const BYTE*)TaskbarController_OnGroupingModeChanged_Address;
        if (p && p[0] == 0x48 && p[1] == 0x83 && p[2] == 0xEC &&
            (p[4] == 0x48 || p[4] == 0x4C) && p[5] == 0x8B && (p[6] & 0xC0) == 0x80) {
            LONG o = *(LONG*)(p + 7);
            if (o > 0 && o <= 0xFFFF) return o;
        }
        Wh_Log(L"Taskbar frame offset not found");
        return 0;
    }();
    return offset;
}

VoidMethod_t TaskbarController_UpdateFrameHeight_Original;
void WINAPI TaskbarController_UpdateFrameHeight_Hook(void* pThis) {
    wux::FrameworkElement frame{nullptr};
    try {
        LONG offset = GetTaskbarFrameOffset();
        void* taskbarFrame = offset ? *(void**)((BYTE*)pThis + offset) : nullptr;
        if (taskbarFrame) {
            ((::IUnknown**)taskbarFrame)[1]->QueryInterface(
                winrt::guid_of<wux::FrameworkElement>(), winrt::put_abi(frame));
        }
        if (frame) frame.MaxHeight(std::numeric_limits<double>::infinity());
    } catch (...) {}

    TaskbarController_UpdateFrameHeight_Original(pThis);

    // The grid holding the frame has a height of its own; keep it matching.
    if (!frame) return;
    try {
        auto grid = wuxm::VisualTreeHelper::GetParent(frame).try_as<wux::FrameworkElement>();
        if (grid) {
            double height = frame.Height();
            double gridHeight = grid.Height();
            if (gridHeight > 0 && gridHeight != height) grid.Height(height);
        }
    } catch (...) {}
}

// ---------------------------------------------------------------------------
// Icon resolution
// ---------------------------------------------------------------------------
// The taskbar loads each app's icon at the size it thinks icons are shown,
// normally 24px (times the display scaling), and anything bigger is that
// small image stretched. TaskbarConfiguration::GetIconHeightInViewPixels is
// where that size comes from, so it now reports g_iconLoadSize, and the icons
// load at full magnification. The icon images are still sized by
// ApplyIconSize, so they show at the dock's icon size.
//
// The taskbar also uses the icon height to lay out its buttons (padding,
// badges, the multi-window look), and some of that code tells the taskbar
// size settings apart by it, so all of that still gets the stock 24px, the
// same as before. Start, Task View and other system buttons get the dock's
// icon size. Adapted from m417z's "Taskbar height and icon size".

// TaskbarFrame::GetMetrics picks the button width from the icon height.
thread_local bool g_inTaskbarFrameGetMetrics = false;

using GetIconHeightInViewPixels_t = double(WINAPI*)(void* pThis);
GetIconHeightInViewPixels_t TaskbarConfiguration_GetIconHeightInViewPixels_Original;
double WINAPI TaskbarConfiguration_GetIconHeightInViewPixels_Hook(void* pThis) {
    double height = TaskbarConfiguration_GetIconHeightInViewPixels_Original(pThis);
    // 16 = the "small taskbar buttons" setting, which this mod doesn't support.
    if (g_iconLoadSize <= 0 || g_inTaskbarFrameGetMetrics || height <= 16) return height;
    return g_iconLoadSize;
}

using TaskbarFrame_GetMetrics_t = void*(WINAPI*)(void* pThis, void* metrics);
TaskbarFrame_GetMetrics_t TaskbarFrame_GetMetrics_Original;
void* WINAPI TaskbarFrame_GetMetrics_Hook(void* pThis, void* metrics) {
    g_inTaskbarFrameGetMetrics = true;
    void* ret = TaskbarFrame_GetMetrics_Original(pThis, metrics);
    g_inTaskbarFrameGetMetrics = false;
    return ret;
}

// ---- App buttons: layout keeps seeing 24px ----

// Where a TaskListButton keeps its icon height, read from its setter, which
// starts by loading the current value:  movsd xmmN, qword ptr [rcx+offset]
void* TaskListButton_IconHeight_Address;

size_t GetIconHeightOffset() {
    static size_t offset = []() -> size_t {
        BYTE* p = (BYTE*)TaskListButton_IconHeight_Address;
        std::regex regex(R"(movsd xmm\d+, qword ptr \[rcx\+0x([0-9a-f]+)\])",
                         std::regex_constants::icase);
        for (int i = 0; p && i < 30; i++) {
            WH_DISASM_RESULT result;
            if (!Wh_Disasm(p, &result)) break;
            p += result.length;
            std::string_view text = result.text;
            if (text == "ret") break;
            std::match_results<std::string_view::const_iterator> match;
            if (std::regex_match(text.begin(), text.end(), match, regex)) {
                size_t o = 0;
                std::from_chars(&*match[1].first, &*match[1].first + match[1].length(), o, 16);
                Wh_Log(L"Icon height offset: 0x%zX", o);
                return o <= 0xFFFF ? o : 0;
            }
        }
        Wh_Log(L"Icon height offset not found; icon resolution unchanged");
        return 0;
    }();
    return offset;
}

// Runs `original` with the button's icon height set to the stock 24px, then
// puts the real value back.
template <typename F>
void WithStockIconHeight(void* pThis, F original) {
    size_t offset = g_iconLoadSize > 0 ? GetIconHeightOffset() : 0;
    if (!offset) return original();
    double* iconHeight = (double*)((BYTE*)pThis + offset);
    double saved = *iconHeight;
    *iconHeight = kStockIconSize;
    original();
    *iconHeight = saved;
}

#define STOCK_ICON_HEIGHT_HOOK(name)                                              \
    VoidMethod_t TaskListButton_##name##_Original;                                \
    void WINAPI TaskListButton_##name##_Hook(void* pThis) {                       \
        WithStockIconHeight(pThis, [=] { TaskListButton_##name##_Original(pThis); }); \
    }

STOCK_ICON_HEIGHT_HOOK(UpdateButtonPadding)
STOCK_ICON_HEIGHT_HOOK(UpdateVisualStates)
STOCK_ICON_HEIGHT_HOOK(UpdateMultiWindowClip)
STOCK_ICON_HEIGHT_HOOK(CreateMultiWindowClip)
STOCK_ICON_HEIGHT_HOOK(UpdateBadge)

using TaskListButton_OverlayIcon_t = void(WINAPI*)(void* pThis, void* stream);
TaskListButton_OverlayIcon_t TaskListButton_OverlayIcon_Original;
void WINAPI TaskListButton_OverlayIcon_Hook(void* pThis, void* stream) {
    WithStockIconHeight(pThis, [=] { TaskListButton_OverlayIcon_Original(pThis, stream); });
}

// ---- Start, Task View and other system buttons ----

// Their icon is shown at the dock's icon size, not the loading size.
SetDouble_t ExperienceToggleButton_IconHeight_Original;
void WINAPI ExperienceToggleButton_IconHeight_Hook(void* pThis, double height) {
    if (g_iconLoadSize > 0 && height > 16) height = g_iconSize;
    ExperienceToggleButton_IconHeight_Original(pThis, height);
}

// Their padding is worked out for the stock 24px, as before. Setting the
// height calls UpdateButtonPadding again, which the flag skips.
using GetDouble_t = double(WINAPI*)(void* pThis);
GetDouble_t ExperienceToggleButton_IconHeight_get;
thread_local bool g_settingToggleIconHeight = false;

void SetToggleIconHeight(void* pThis, double height) {
    g_settingToggleIconHeight = true;
    ExperienceToggleButton_IconHeight_Original(pThis, height);
    g_settingToggleIconHeight = false;
}

VoidMethod_t ExperienceToggleButton_UpdateButtonPadding_Original;
void WINAPI ExperienceToggleButton_UpdateButtonPadding_Hook(void* pThis) {
    if (g_settingToggleIconHeight) return;
    double height = kStockIconSize;
    if (g_iconLoadSize > 0 && ExperienceToggleButton_IconHeight_get &&
        ExperienceToggleButton_IconHeight_Original) {
        height = ExperienceToggleButton_IconHeight_get(pThis);
    }
    bool swap = height > 16 && height != kStockIconSize;
    if (swap) SetToggleIconHeight(pThis, kStockIconSize);
    ExperienceToggleButton_UpdateButtonPadding_Original(pThis);
    if (swap) SetToggleIconHeight(pThis, height);
}

// Taskbar extensions such as widgets: shown at the dock's icon size too.
SetDouble_t TaskbarComponentHost_IconHeight_Original;
void WINAPI TaskbarComponentHost_IconHeight_Hook(void* pThis, double height) {
    if (g_iconLoadSize > 0 && height > 16) height = g_iconSize;
    TaskbarComponentHost_IconHeight_Original(pThis, height);
}

// ---- Making Explorer apply a new height ----

// Explorer re-reads the height when it handles a settings change; this makes
// it also resize the taskbar window right away.
VoidMethod_t TrayUI__StuckTrayChange_Original;

using TrayUI__HandleSettingChange_t = void(WINAPI*)(void*, void*, void*, void*, void*);
TrayUI__HandleSettingChange_t TrayUI__HandleSettingChange_Original;
void WINAPI TrayUI__HandleSettingChange_Hook(void* pThis, void* p1, void* p2, void* p3,
                                             void* p4) {
    TrayUI__HandleSettingChange_Original(pThis, p1, p2, p3, p4);
    if (g_applyingHeight) TrayUI__StuckTrayChange_Original(pThis);
}

void ApplyTaskbarHeight() {
    HWND tb = FindTaskbar();
    if (!tb) return;
    g_applyingHeight = true;
    SendMessage(tb, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);
    g_applyingHeight = false;

    // Make the taskbar reload its icons, at the new resolution.
    if (HWND rebar = FindWindowEx(tb, nullptr, L"ReBarWindow32", nullptr)) {
        if (HWND taskSw = FindWindowEx(rebar, nullptr, L"MSTaskSwWClass", nullptr)) {
            SendMessage(taskSw, 0x452, 3, 0);
        }
    }
}

// ---- Installing the hooks ----

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) return false;

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: void __cdecl TrayUI::_StuckTrayChange(void))"},
            &TrayUI__StuckTrayChange_Original,
        },
        {
            {LR"(public: void __cdecl TrayUI::_HandleSettingChange(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &TrayUI__HandleSettingChange_Original,
            TrayUI__HandleSettingChange_Hook,
        },
    };
    return WindhawkUtils::HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

// The name the symbol extraction script expects for a list of hooks that
// isn't one of the hook arrays itself.
using COMBINED_SH = WindhawkUtils::SYMBOL_HOOK;

// The system tray's hooks. Newer builds keep the tray in SystemTray.dll,
// older ones in Taskbar.View.dll (see TaskbarViewHasTray).
std::vector<COMBINED_SH> GetTrayHooks() {
    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK trayHooks[] = {
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTrayController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTrayController_GetFrameSize_Original,
            SystemTrayController_GetFrameSize_Hook,
            true,
        },
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTraySecondaryController_GetFrameSize_Original,
            SystemTraySecondaryController_GetFrameSize_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTrayController::UpdateFrameSize(void))"},
            &SystemTrayController_UpdateFrameSize_Original,
            SystemTrayController_UpdateFrameSize_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::UpdateFrameSize(void))"},
            &SystemTraySecondaryController_UpdateFrameSize_Original,
            SystemTraySecondaryController_UpdateFrameSize_Hook,
            true,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::SystemTrayFrame>::Height(double)const )"},
            &SystemTrayFrame_Height_Original,
            SystemTrayFrame_Height_Hook,
            true,
        },
    };
    return {std::make_move_iterator(std::begin(trayHooks)),
            std::make_move_iterator(std::end(trayHooks))};
}

bool HookTaskbarViewSymbols(HMODULE module, bool withTray) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &TaskbarConfiguration_GetFrameSize_Original,
            TaskbarConfiguration_GetFrameSize_Hook,
            true,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::MaxHeight(double)const )"},
            &TaskbarFrame_MaxHeight_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const )"},
            &TaskbarFrame_Height_Original,
            TaskbarFrame_Height_Hook,
            true,  // Gone in 24H2.
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::OnGroupingModeChanged(void))"},
            &TaskbarController_OnGroupingModeChanged_Address,
            nullptr,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::UpdateFrameHeight(void))"},
            &TaskbarController_UpdateFrameHeight_Original,
            TaskbarController_UpdateFrameHeight_Hook,
            true,
        },

        // Icon resolution
        {
            {LR"(public: double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(void))"},
            &TaskbarConfiguration_GetIconHeightInViewPixels_Original,
            TaskbarConfiguration_GetIconHeightInViewPixels_Hook,
            true,
        },
        {
            {LR"(public: struct winrt::Taskbar::implementation::TaskbarFrameMetrics __cdecl winrt::Taskbar::implementation::TaskbarFrame::GetMetrics(void)const )"},
            &TaskbarFrame_GetMetrics_Original,
            TaskbarFrame_GetMetrics_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::IconHeight(double))"},
            &TaskListButton_IconHeight_Address,
            nullptr,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void))"},
            &TaskListButton_UpdateButtonPadding_Original,
            TaskListButton_UpdateButtonPadding_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateMultiWindowClip(void))"},
            &TaskListButton_UpdateMultiWindowClip_Original,
            TaskListButton_UpdateMultiWindowClip_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::CreateMultiWindowClip(void))"},
            &TaskListButton_CreateMultiWindowClip_Original,
            TaskListButton_CreateMultiWindowClip_Hook,
            true,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateBadge(void))"},
            &TaskListButton_UpdateBadge_Original,
            TaskListButton_UpdateBadge_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::OverlayIcon(struct winrt::Windows::Storage::Streams::IRandomAccessStream const &))"},
            &TaskListButton_OverlayIcon_Original,
            TaskListButton_OverlayIcon_Hook,
            true,
        },
        {
            {LR"(public: double __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::IconHeight(void)const )"},
            &ExperienceToggleButton_IconHeight_get,
            nullptr,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::IconHeight(double))"},
            &ExperienceToggleButton_IconHeight_Original,
            ExperienceToggleButton_IconHeight_Hook,
            true,
        },
        {
            {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateButtonPadding(void))"},
            &ExperienceToggleButton_UpdateButtonPadding_Original,
            ExperienceToggleButton_UpdateButtonPadding_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Microsoft::Windows::Taskbar::implementation::TaskbarComponentHost::IconHeight(double))"},
            &TaskbarComponentHost_IconHeight_Original,
            TaskbarComponentHost_IconHeight_Hook,
            true,
        },
    };
    // On builds where the tray is in Taskbar.View.dll too, its hooks go in the
    // same call, so the module's symbols are only looked up once.
    std::vector<COMBINED_SH> allHooks(std::make_move_iterator(std::begin(taskbarViewHooks)),
                                      std::make_move_iterator(std::end(taskbarViewHooks)));
    if (withTray) {
        for (auto& hook : GetTrayHooks()) allHooks.push_back(std::move(hook));
    }
    if (!WindhawkUtils::HookSymbols(module, allHooks.data(), allHooks.size())) return false;
    if (TaskListButton_IconHeight_Address) GetIconHeightOffset();  // find it up front
    return true;
}

bool HookTraySymbols(HMODULE module) {
    auto systemTrayHooks = GetTrayHooks();
    return WindhawkUtils::HookSymbols(module, systemTrayHooks.data(), systemTrayHooks.size());
}

// Taskbar.View.dll before version 2604 contains the system tray too; newer
// builds moved it into SystemTray.dll.
bool TaskbarViewHasTray(HMODULE module) {
    HRSRC res = FindResource(module, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    HGLOBAL global = res ? LoadResource(module, res) : nullptr;
    void* data = global ? LockResource(global) : nullptr;
    VS_FIXEDFILEINFO* info = nullptr;
    UINT size = 0;
    if (!data || !VerQueryValue(data, L"\\", (void**)&info, &size) || !size) return false;
    return HIWORD(info->dwFileVersionMS) < 2604;
}

std::atomic<bool> g_taskbarViewHooked = false;
std::atomic<bool> g_trayHooked = false;

// Hooks whichever of the taskbar's DLLs are loaded and not hooked yet.
// Returns true if anything new was hooked.
bool HookTaskbarModules() {
    bool hooked = false;
    if (HMODULE view = GetModuleHandle(L"Taskbar.View.dll");
        view && !g_taskbarViewHooked.exchange(true)) {
        bool withTray = !GetModuleHandle(L"SystemTray.dll") && TaskbarViewHasTray(view) &&
                        !g_trayHooked.exchange(true);
        HookTaskbarViewSymbols(view, withTray);
        hooked = true;
    }
    if (HMODULE tray = GetModuleHandle(L"SystemTray.dll");
        tray && !g_trayHooked.exchange(true)) {
        HookTraySymbols(tray);
        hooked = true;
    }
    return hooked;
}

// The taskbar's DLLs load late when Explorer starts after the mod.
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(name, file, flags);
    if (module && (!g_taskbarViewHooked || !g_trayHooked) && HookTaskbarModules()) {
        Wh_ApplyHookOperations();
    }
    return module;
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

void LoadSettings() {
    g_dockSize = std::clamp(Wh_GetIntSetting(L"dockSize"), 0, 100);
    // Magnification 0-100 -> the icon grows to 1.0x-2.0x.
    g_scale = 1.0f + std::clamp(Wh_GetIntSetting(L"magnification"), 0, 100) / 100.0f;
    g_dockTint = std::clamp(Wh_GetIntSetting(L"dockOpacity"), 0, 100);
    g_dockBlur = std::clamp(Wh_GetIntSetting(L"dockBlur"), 0, 100);
    g_animationMs = std::clamp(Wh_GetIntSetting(L"animationMs"), 0, 1000);

    ComputeSizes();
    Wh_Log(L"Sizes: icon %.0f (loaded at %.0f), dock %.0f, taskbar %d, button margin %.1f",
           g_iconSize, g_iconLoadSize, g_dockHeight, g_taskbarHeight, g_buttonMargin);
}

BOOL Wh_ModInit() {
    LoadSettings();

    if (!HookTaskbarDllSymbols()) Wh_Log(L"taskbar.dll hooks failed; height won't change");
    HookTaskbarModules();

    if (HMODULE user32 = GetModuleHandle(L"user32.dll")) {
        if (auto p = GetProcAddress(user32, "CreateWindowInBand")) {
            Wh_SetFunctionHook((void*)p, (void*)CreateWindowInBand_Hook,
                               (void**)&CreateWindowInBand_Original);
        }
    }
    if (HMODULE kb = GetModuleHandle(L"kernelbase.dll")) {
        if (auto p = GetProcAddress(kb, "RegOpenKeyExW")) {
            Wh_SetFunctionHook((void*)p, (void*)RegOpenKeyExW_Hook, (void**)&RegOpenKeyExW_Original);
        }
        if (auto p = GetProcAddress(kb, "RegQueryValueExW")) {
            Wh_SetFunctionHook((void*)p, (void*)RegQueryValueExW_Hook,
                               (void**)&RegQueryValueExW_Original);
        }
        if (auto p = GetProcAddress(kb, "LoadLibraryExW")) {
            Wh_SetFunctionHook((void*)p, (void*)LoadLibraryExW_Hook,
                               (void**)&LoadLibraryExW_Original);
        }
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (HWND tb = FindTaskbar()) {
        RunFromWindowThread(tb, [](void*) { InjectTAP(); }, nullptr);
    }
    if (g_taskbarHeight > 0) ApplyTaskbarHeight();
}

// Runs before the hooks are removed, which the taskbar needs to go back to its
// normal height and icon size.
void Wh_ModBeforeUninit() {
    // If the visual tree subscription is still running, let it finish first.
    if (HANDLE t = g_adviseThread.exchange(nullptr)) {
        WaitForSingleObject(t, INFINITE);
        CloseHandle(t);
    }

    // Undo the changes to the taskbar's elements first, so that when it goes
    // back to its normal size below, it sizes its icons itself, with nothing
    // of ours left to override that.
    if (HWND tb = FindTaskbar()) {
        RunFromWindowThread(tb, [](void*) {
            ResetAll();
            if (g_watcher) { g_watcher->Unadvise(); g_watcher = nullptr; }
        }, nullptr);
    }

    if (g_taskbarHeight > 0) {
        g_taskbarHeight = 0;
        g_iconLoadSize = 0;
        ApplyTaskbarHeight();
    }
}

void Wh_ModUninit() {}

void Wh_ModSettingsChanged() {
    int oldTaskbarHeight = g_taskbarHeight;
    float oldIconLoadSize = g_iconLoadSize;
    LoadSettings();
    // Scale settings are read every frame; the plate's look needs a refresh.
    if (HWND tb = FindTaskbar()) {
        RunFromWindowThread(tb, [](void*) {
            for (auto& p : g_plates) {
                if (p.root) {
                    try { ApplyPlateLook(p); } catch (...) {}
                }
            }
            for (auto& b : g_buttons) {
                if (auto el = b.element.get()) {
                    try { ApplySpacing(el); } catch (...) {}
                }
            }
            for (auto& i : g_iconImages) {
                if (auto el = i.element.get()) {
                    try { ApplyIconSize(el); } catch (...) {}
                }
            }
            g_row.clear();  // rebuilt with the new spacing on next hover
            g_rowParent = nullptr;
            MarkPlatesDirty();
        }, nullptr);
    }
    if (g_taskbarHeight != oldTaskbarHeight || g_iconLoadSize != oldIconLoadSize) {
        ApplyTaskbarHeight();
    }
}

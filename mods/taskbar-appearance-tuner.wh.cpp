// ==WindhawkMod==
// @id              taskbar-brightness-and-opacity-tuner
// @name            Taskbar brightness and opacity tuner
// @name:zh-CN      任务栏亮度和透明度调节器
// @description     Adjust the opacity of the taskbar background and of the icons and text, and dim the taskbar background, for a clean, beautiful taskbar which is easier on the eyes and on OLED displays
// @description:zh-CN 分别调整任务栏背景与图标文字的不透明度，并可调暗任务栏背景，定制出简洁漂亮的任务栏，也更护眼、更适合 OLED 显示器
// @version         1.9.3
// @author          lzxujun
// @homepage        https://github.com/lzxujun
// @license         GPL-3.0
// @github          https://github.com/lzxujun
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# Taskbar brightness and opacity tuner

> **English:** Adjust the opacity of the taskbar background and of its icons
> and text, and dim the taskbar background, to build a clean, beautiful
> taskbar that fits your desktop — and a taskbar which blends into it is also
> easier on the eyes and lighter on OLED displays.
>
> **中文：** 调整任务栏背景与图标文字的不透明度，并可调暗任务栏背景，定制出
> 简洁漂亮的任务栏——与桌面融为一体的任务栏，也更护眼、更适合 OLED 显示器。

The taskbar is adjusted in two independent layers, each displayed on a simple
0-100 scale:

* **Taskbar background** - the material behind the icons. 0 means fully
  transparent, 100 means unchanged.
* **Icons and text** - buttons, labels and the system tray. 0 means fully
  transparent, 100 means unchanged.

The taskbar background can additionally be dimmed on a 0-100 scale: 0 means
unchanged, 100 means fully dimmed to black. Dimming renders the live backdrop
behind the taskbar darker - a low, dark taskbar is easier on the eyes at
night and reduces OLED burn-in. The mod only dims, never brightens.

Additionally:

* **Show the taskbar top line** - shows or hides the thin line at the top
  edge of the taskbar.
* The small gray rounded drag handle at the top center of the taskbar is
  always hidden while the mod is active. That handle is part of Windows itself
  (it shows while the taskbar is unlocked), it is not drawn by this mod. If
  Windows shows it again later, e.g. after the taskbar was locked and
  unlocked, the mod hides it again automatically.

## Screenshots

**Task buttons area, transparency unchanged:**

![Taskbar task button area when transparency has not been modified](https://i.imgur.com/L37dzXL.png)

**Task buttons area, transparency set to 35%:**

![Taskbar task button area when transparency is set to 35%](https://i.imgur.com/QHkfKZJ.png)

**Task buttons area, transparency state:**

![Taskbar task button area transparency state](https://i.imgur.com/DrMjBOY.png)

**System tray area, transparency state:**

![Taskbar system tray area transparency state](https://i.imgur.com/ACFLOFC.png)

## How it works

The two layers are XAML elements of the taskbar visual tree, and their
opacity is adjusted exactly, with no visual tricks: the background blends
into whatever is behind the taskbar, and the icons and text blend into the
background.

The background dimming is exact as well: the fill of the background rectangle
is replaced with a brush which renders the live backdrop filtered by a color
matrix effect, so what gets darker is the real content behind the taskbar -
not an overlay painted on top of it.

The icons and text layer is found by walking the taskbar visual tree: every
subtree which contains neither the background nor a flyout host is adjusted
as a unit. This covers all icon areas (the Start, search and Task View
buttons, the task buttons and the system tray) on every Windows build,
without depending on the class names of the individual containers.

## Compared with other mods

* **Windows 11 Taskbar Styler** can restyle the top line, the drag grip and
  the background (and much more), but it takes style rules. This mod is a
  zero-configuration dial instead: two 0-100 values cover the whole taskbar
  at once, no rules to write, and the drag grip is hidden automatically.
* **Taskbar Background Helper** and **Dynamic Taskbar Transparency** adjust
  the background only (blur/acrylic/color, or per-shell-state opacity). This
  mod adjusts the background opacity and dimming as well, but additionally
  the icons and text as a whole layer.

## Notes

* Windows 11 only (the mod relies on the XAML taskbar visual tree).
* If nothing seems to happen, make sure at least one value differs from 100.
* The background layer is the same XAML element which background mods such as
  **Windows 11 Taskbar Styler** or **Taskbar Background Helper** adjust. Two
  mods fighting over the same element can overwrite each other's result, so
  adjust the taskbar background with only one of them at a time.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- backgroundBrightness: 0
  $name: Background brightness (0 = unchanged, 100 = black)
  $name:zh-CN: 背景亮度（0 = 不变，100 = 全黑）
  $description: >-
    Dimming applied to the taskbar background material, from 0 (no change)
    to 100 (fully dimmed to black). The mod only dims, never brightens.
  $description:zh-CN: 任务栏背景材质的调暗量，0 表示不调整，100 表示调至全黑。本 mod 只调暗不调亮。
- backgroundOpacity: 100
  $name: Background opacity (0 = transparent, 100 = unchanged)
  $name:zh-CN: 背景不透明度（0 = 全透明，100 = 不变）
  $description: >-
    Opacity of the taskbar background material, from 0 (fully transparent)
    to 100 (unchanged).
  $description:zh-CN: 任务栏背景材质的不透明度，0 表示完全透明，100 表示不调整。
- iconsOpacity: 100
  $name: Icons and text opacity (0 = transparent, 100 = unchanged)
  $name:zh-CN: 图标和文字不透明度（0 = 全透明，100 = 不变）
  $description: >-
    Opacity of the task buttons, tray icons and labels, from 0 (fully
    transparent) to 100 (unchanged).
  $description:zh-CN: 任务按钮、托盘图标和文字标签的不透明度，0 表示完全透明，100 表示不调整。
- topLine: true
  $name: Show the taskbar top line
  $name:zh-CN: 显示任务栏顶部线
  $description: >-
    Whether to show the thin line at the top edge of the taskbar. The line is
    hidden by fading it out, so switching it back on restores it exactly.
  $description:zh-CN: 是否显示任务栏顶部的细线。关闭后该线以淡出方式隐藏，重新打开即精确恢复。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cwchar>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#undef GetCurrentTime

#include <d2d1_1.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>

namespace wuc = winrt::Windows::UI::Composition;
namespace wge = winrt::Windows::Graphics::Effects;

using namespace winrt::Windows::UI::Xaml;

////////////////////////////////////////////////////////////////////////////////
// windows.graphics.effects.interop.h (subset, from Windows 11 Taskbar Styler)
#ifndef BUILD_WINDOWS
namespace ABI {
#endif
namespace Windows {
namespace Graphics {
namespace Effects {

typedef interface IGraphicsEffectSource                         IGraphicsEffectSource;
typedef interface IGraphicsEffectD2D1Interop                    IGraphicsEffectD2D1Interop;

typedef enum GRAPHICS_EFFECT_PROPERTY_MAPPING
{
    GRAPHICS_EFFECT_PROPERTY_MAPPING_UNKNOWN,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORX,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORY,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORZ,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORW,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_RECT_TO_VECTOR4,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_COLORMATRIX_ALPHA_MODE,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR3,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR4
} GRAPHICS_EFFECT_PROPERTY_MAPPING;

#undef INTERFACE
#define INTERFACE IGraphicsEffectD2D1Interop
DECLARE_INTERFACE_IID_(IGraphicsEffectD2D1Interop, IUnknown, "2FC57384-A068-44D7-A331-30982FCF7177")
{
    STDMETHOD(GetEffectId)(
        _Out_ GUID * id
        ) PURE;

    STDMETHOD(GetNamedPropertyMapping)(
        LPCWSTR name,
        _Out_ UINT * index,
        _Out_ GRAPHICS_EFFECT_PROPERTY_MAPPING * mapping
        ) PURE;

    STDMETHOD(GetPropertyCount)(
        _Out_ UINT * count
        ) PURE;

    STDMETHOD(GetProperty)(
        UINT index,
        _Outptr_ winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue> ** value
        ) PURE;

    STDMETHOD(GetSource)(
        UINT index,
        _Outptr_ IGraphicsEffectSource ** source
        ) PURE;

    STDMETHOD(GetSourceCount)(
        _Out_ UINT * count
        ) PURE;
};

} // namespace Effects
} // namespace Graphics
} // namespace Windows
#ifndef BUILD_WINDOWS
} // namespace ABI
#endif

template <> inline constexpr winrt::guid winrt::impl::guid_v<ABI::Windows::Graphics::Effects::IGraphicsEffectD2D1Interop>{
    0x2FC57384, 0xA068, 0x44D7, { 0xA3, 0x31, 0x30, 0x98, 0x2F, 0xCF, 0x71, 0x77 }
};

// Required for the ABI IPropertyValue pointer used by GetProperty(): the
// MinGW based compiler doesn't provide a uuid for the ABI projection of
// IPropertyValue, so it is aliased to the projected interface's uuid here.
template <> inline constexpr winrt::guid winrt::impl::guid_v<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>{
    winrt::impl::guid_v<winrt::Windows::Foundation::IPropertyValue>
};

namespace awge = ABI::Windows::Graphics::Effects;

// {921F03D6-641C-47DF-852D-B4BB6153AE11} - the Direct2D color matrix effect
// CLSID. It is duplicated here as a local constant instead of using the
// CLSID_D2D1ColorMatrix macro from d2d1effects.h, because that macro expands
// to an extern declaration which the Windhawk linker can't resolve.
constexpr GUID kColorMatrixEffectId{
    0x921f03d6, 0x641c, 0x47df, {0x85, 0x2d, 0xb4, 0xbb, 0x61, 0x53, 0xae, 0x11}};

////////////////////////////////////////////////////////////////////////////////
// ColorMatrixEffect (from Windows 11 Taskbar Styler)
struct ColorMatrixEffect
    : winrt::implements<ColorMatrixEffect,
                        wge::IGraphicsEffect,
                        wge::IGraphicsEffectSource,
                        awge::IGraphicsEffectD2D1Interop> {
    wge::IGraphicsEffectSource Source{nullptr};

    // D2D1_MATRIX_5X4_F: 5 rows x 4 columns (20 floats), identity by default.
    // Rows are the output RGBA channels, columns are the input RGBA channels,
    // and the last row holds the per-channel offsets.
    float Matrix[20] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
        0, 0, 0, 0,
    };

    uint32_t AlphaMode = D2D1_COLORMATRIX_ALPHA_MODE_PREMULTIPLIED;
    bool ClampOutput = false;

    // IGraphicsEffectD2D1Interop
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) {
            return E_INVALIDARG;
        }

        *id = kColorMatrixEffectId;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name,
        UINT* index,
        awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!index || !mapping) {
            return E_INVALIDARG;
        }

        const std::wstring_view nameView(name);
        if (nameView == L"ColorMatrix") {
            *index = D2D1_COLORMATRIX_PROP_COLOR_MATRIX;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }

        if (nameView == L"AlphaMode") {
            *index = D2D1_COLORMATRIX_PROP_ALPHA_MODE;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }

        if (nameView == L"ClampOutput") {
            *index = D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }

        return E_INVALIDARG;
    }

    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) {
            return E_INVALIDARG;
        }

        *count = 3;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetProperty(
        UINT index,
        winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>**
            value) noexcept override try {
        if (!value) {
            return E_INVALIDARG;
        }

        switch (index) {
            case D2D1_COLORMATRIX_PROP_COLOR_MATRIX:
                *value = winrt::Windows::Foundation::PropertyValue::
                    CreateSingleArray(winrt::array_view<const float>(
                        Matrix, Matrix + 20))
                        .as<winrt::impl::abi_t<
                            winrt::Windows::Foundation::IPropertyValue>>()
                        .detach();
                break;

            case D2D1_COLORMATRIX_PROP_ALPHA_MODE:
                *value = winrt::Windows::Foundation::PropertyValue::
                    CreateUInt32(AlphaMode)
                        .as<winrt::impl::abi_t<
                            winrt::Windows::Foundation::IPropertyValue>>()
                        .detach();
                break;

            case D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT:
                *value = winrt::Windows::Foundation::PropertyValue::
                    CreateBoolean(ClampOutput)
                        .as<winrt::impl::abi_t<
                            winrt::Windows::Foundation::IPropertyValue>>()
                        .detach();
                break;

            default:
                return E_BOUNDS;
        }

        return S_OK;
    }
    catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE GetSource(
        UINT index,
        awge::IGraphicsEffectSource** source) noexcept override {
        if (!source) {
            return E_INVALIDARG;
        }

        if (index == 0 && Source) {
            winrt::copy_to_abi(Source, *reinterpret_cast<void**>(source));
            return S_OK;
        }

        return E_BOUNDS;
    }

    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) {
            return E_INVALIDARG;
        }

        *count = 1;
        return S_OK;
    }

    // IGraphicsEffect
    winrt::hstring Name() { return m_name; }

    void Name(winrt::hstring name) { m_name = std::move(name); }

private:
    winrt::hstring m_name = L"ColorMatrixEffect";
};

////////////////////////////////////////////////////////////////////////////////
// BackdropAdjustBrush: renders the live backdrop (what is behind the taskbar)
// with a brightness adjustment via a color matrix effect.
class BackdropAdjustBrush
    : public Media::XamlCompositionBrushBaseT<BackdropAdjustBrush> {
public:
    BackdropAdjustBrush(wuc::Compositor const& compositor, float brightness)
        : m_compositor(compositor),
          m_brightness(brightness) {}

    void OnConnected() {
        if (CompositionBrush()) {
            return;
        }

        CompositionBrush(CreateEffectBrush());
    }

    void OnDisconnected() {
        CompositionBrush(nullptr);
    }

private:
    wuc::CompositionBrush CreateEffectBrush() {
        auto backdropBrush = m_compositor.CreateBackdropBrush();

        // Dimming: the color matrix scales the color channels toward black.
        // Brightening is not supported (the mod exists to make the taskbar
        // dimmer), so the bias row of the matrix stays at zero.
        auto brightMatrix = winrt::make_self<ColorMatrixEffect>();
        brightMatrix->Source = wuc::CompositionEffectSourceParameter(L"source");

        float brightness = std::clamp(m_brightness, -1.0f, 0.0f);
        float scale = 1.0f + brightness;
        auto& bm = brightMatrix->Matrix;
        bm[0] = scale; bm[1] = 0.0f;  bm[2] = 0.0f;  bm[3] = 0.0f;
        bm[4] = 0.0f;  bm[5] = scale; bm[6] = 0.0f;  bm[7] = 0.0f;
        bm[8] = 0.0f;  bm[9] = 0.0f;  bm[10] = scale; bm[11] = 0.0f;
        bm[12] = 0.0f; bm[13] = 0.0f; bm[14] = 0.0f; bm[15] = 1.0f;
        bm[16] = 0.0f; bm[17] = 0.0f; bm[18] = 0.0f; bm[19] = 0.0f;
        brightMatrix->ClampOutput = true;
        brightMatrix->Name(L"BrightnessEffect");

        Wh_Log(L"Effect graph: dimming=%.2f (scale=%.2f)", brightness, scale);

        auto factory = m_compositor.CreateEffectFactory(*brightMatrix);
        auto brush = factory.CreateBrush();
        brush.SetSourceParameter(L"source", backdropBrush);

        return brush;
    }

    wuc::Compositor m_compositor;
    float m_brightness;
};

////////////////////////////////////////////////////////////////////////////////
// Mod logic

// All values are shown to the user on a 0-100 scale. Brightness values are
// dimming amounts: 0 means no change, 100 means fully dimmed to black.
struct {
    int backgroundBrightness;  // 0..100 dimming amount
    int backgroundOpacity;     // 0..100, 100 = no change
    int iconsOpacity;          // 0..100, 100 = no change
    bool topLine;              // false = hide the taskbar top line
} g_settings;

std::atomic<bool> g_unloading{false};

// The taskbar XAML tree is dumped to the log once per mod load, and only
// after it turned out to be populated.
std::atomic<bool> g_treeDumped{false};

// Finds a direct child by its XAML name.
FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (child) {
            try {
                if (child.Name() == name) {
                    return child;
                }
            } catch (...) {
            }
        }
    }
    return nullptr;
}

// Finds a direct child by its XAML class name.
FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (child) {
            try {
                if (winrt::get_class_name(child) == className) {
                    return child;
                }
            } catch (...) {
            }
        }
    }
    return nullptr;
}

struct AppearanceState {
    // The XamlRoot content element of the taskbar this state belongs to. It is
    // used to look the state up again when the settings change.
    winrt::weak_ref<FrameworkElement> root;
    winrt::weak_ref<FrameworkElement> backgroundFill;
    double originalFillOpacity = 1.0;
    // backgroundFillReplaced records whether the fill of the background
    // rectangle was replaced with a backdrop brush (background dimming). The
    // original brush is held and put back by assignment: elements of the
    // taskbar XAML template receive their values as local values, so
    // ClearValue would wipe the fill instead of reverting it. RestoreState
    // only ever runs on the taskbar UI thread, so the strong reference is
    // always released from the thread the object belongs to.
    bool backgroundFillReplaced = false;
    Media::Brush originalFillBrush = nullptr;
    // The taskbar top line rectangle. topLineHidden records whether the line
    // was actually hidden, so that restore only touches properties which were
    // modified. The line is hidden with Opacity only: that is a plain value
    // which can be handed back verbatim, and it is independent of how the line
    // happens to be drawn (fill or stroke). Visibility, Fill and
    // StrokeThickness are deliberately never written - the line belongs to the
    // taskbar XAML template and holds its values as local values, so
    // overwriting them destroys the original look for good (ClearValue does not
    // revert a template value, it wipes it).
    winrt::weak_ref<FrameworkElement> topLine;
    bool topLineHidden = false;
    double originalTopLineOpacity = 1.0;
    // The taskbar drag grip handle (Rectangle#Gripper). The handle is hidden
    // while the mod is active; the property-changed callback re-hides it when
    // Windows makes it visible again (locking and unlocking the taskbar does).
    winrt::weak_ref<FrameworkElement> grip;
    Visibility originalGripVisibility = Visibility::Visible;
    int64_t gripVisibilityToken = 0;
    // Foreground elements with their original opacity.
    std::vector<std::pair<winrt::weak_ref<FrameworkElement>, double>>
        foregroundOpacity;
};

// The state holds weak references and value types, plus the single strong
// brush saved for the background fill restore. RestoreAllStates() always runs
// on the taskbar UI thread (settings changes, neutral settings and unload all
// go through ApplyPassOnTaskbarThread), so the brush is released from the
// thread it belongs to. The only path which could release it elsewhere is
// process termination running global destructors, where nothing can be served
// anymore anyway.
std::vector<AppearanceState> g_states;

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

// Recursively enumerates all descendants of the given element. The walk is
// bounded: an unexpectedly deep tree stops the enumeration instead of
// overflowing the stack. The callback is a template parameter so that no
// std::function is allocated per call.
template <typename T>
void EnumDescendants(FrameworkElement element, T&& enumCallback, int depth = 0) {
    constexpr int kMaxDepth = 12;
    if (depth > kMaxDepth) {
        return;
    }

    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }

        if (enumCallback(child)) {
            return;
        }

        EnumDescendants(child, enumCallback, depth + 1);
    }
}

FrameworkElement FindDescendantByName(FrameworkElement element, PCWSTR name) {
    FrameworkElement result = nullptr;
    EnumDescendants(element, [&](FrameworkElement child) {
        try {
            if (child.Name() == name) {
                result = child;
                return true;
            }
        } catch (...) {
            // The element may be torn down mid-walk.
        }
        return false;
    });
    return result;
}

std::wstring DescribeElement(FrameworkElement element) {
    std::wstring result;
    try {
        result = winrt::get_class_name(element).c_str();
    } catch (...) {
        result = L"<unknown>";
    }

    try {
        auto name = element.Name();
        if (!name.empty()) {
            result += L"#";
            result += name.c_str();
        }
    } catch (...) {
    }

    return result;
}

// Describes the bounds of the element relative to the content element as
// "(x, y w x h)", for the log.
std::wstring DescribeRect(FrameworkElement element, FrameworkElement content) {
    try {
        auto origin = element.TransformToVisual(content)
                          .TransformPoint(
                              winrt::Windows::Foundation::Point{0, 0});
        wchar_t buffer[128];
        swprintf_s(buffer, L"(%.0f,%.0f %.0fx%.0f)", origin.X, origin.Y,
                   element.ActualWidth(), element.ActualHeight());
        return buffer;
    } catch (...) {
        return L"(?)";
    }
}

// Flyouts (the hover window previews, the tray overflow menu, Alt+Tab, ...) are
// hosted by elements inside the taskbar tree and render outside of the visible
// taskbar. Adjusting them leaks the adjustment outside of the taskbar (a gray
// strip above it), so they are skipped. Popups are recognized by their type,
// the rest by the class name of the host element.
bool IsFlyoutHost(FrameworkElement element) {
    try {
        if (element.try_as<Controls::Primitives::Popup>()) {
            return true;
        }
    } catch (...) {
    }

    try {
        std::wstring_view className(winrt::get_class_name(element));
        return className.find(L"Switcher") != std::wstring_view::npos ||
               className.find(L"TaskbarExperienceHost") !=
                   std::wstring_view::npos ||
               className.find(L"Flyout") != std::wstring_view::npos;
    } catch (...) {
        return false;
    }
}

// The background rectangles (the taskbar background and the background of the
// hover flyout) are not part of the icon and text layer: the taskbar background
// is adjusted separately, and the flyout background must not be touched at all.
bool IsBackgroundRectangle(FrameworkElement element) {
    try {
        auto name = element.Name();
        return name == L"BackgroundFill" || name == L"BackgroundStroke";
    } catch (...) {
        return false;
    }
}

// System chrome inside the taskbar which is not icon and text content: the
// drag grip handle (shown while the taskbar is unlocked) and the stroke which
// closes the taskbar at the bottom screen edge. Adjusting these has no useful
// effect and the grip would show up as a gray bar of its own.
bool IsSystemChromeElement(FrameworkElement element) {
    try {
        if (std::wstring_view(winrt::get_class_name(element).c_str())
                .starts_with(L"Taskbar.Gripper")) {
            return true;
        }
    } catch (...) {
    }
    try {
        if (element.Name() == L"ScreenEdgeStroke") {
            return true;
        }
    } catch (...) {
    }
    return false;
}

// Returns true when the subtree is pure icon and text content, i.e. it contains
// neither the background nor a flyout host, so that it can be adjusted as a
// single unit. The walk is bounded, an unexpectedly large subtree counts as
// impure, in which case the caller descends into it instead.
bool IsPureIconSubtree(FrameworkElement element) {
    constexpr int kMaxDepth = 12;
    constexpr int kMaxElements = 400;

    std::vector<std::pair<FrameworkElement, int>> stack;
    stack.push_back({element, 0});

    int visited = 0;
    while (!stack.empty()) {
        auto [current, depth] = stack.back();
        stack.pop_back();

        if (!current || IsBackgroundRectangle(current) ||
            IsFlyoutHost(current) || depth > kMaxDepth) {
            return false;
        }

        if (++visited > kMaxElements) {
            return false;
        }

        int childrenCount = 0;
        try {
            childrenCount = Media::VisualTreeHelper::GetChildrenCount(current);
        } catch (...) {
            return false;
        }

        for (int i = 0; i < childrenCount; i++) {
            FrameworkElement child = nullptr;
            try {
                child = Media::VisualTreeHelper::GetChild(current, i)
                            .try_as<FrameworkElement>();
            } catch (...) {
            }
            if (child) {
                stack.push_back({child, depth + 1});
            }
        }
    }

    return true;
}

bool ContainsElement(std::vector<FrameworkElement> const& elements,
                     FrameworkElement element) {
    return std::find(elements.begin(), elements.end(), element) !=
           elements.end();
}

bool HasCollectedAncestor(std::vector<FrameworkElement> const& elements,
                          FrameworkElement element) {
    try {
        auto parent = Media::VisualTreeHelper::GetParent(element)
                          .try_as<FrameworkElement>();
        while (parent) {
            if (ContainsElement(elements, parent)) {
                return true;
            }
            parent = Media::VisualTreeHelper::GetParent(parent)
                         .try_as<FrameworkElement>();
        }
    } catch (...) {
    }

    return false;
}

// The taskbar XAML tree looks roughly like this (Windows 11):
//
//   <content element of the taskbar XamlRoot>
//     Taskbar.TaskbarFrame
//       Grid#RootGrid
//         Taskbar.TaskbarBackground#BackgroundControl
//           Grid
//             Rectangle#BackgroundFill
//             Rectangle#BackgroundStroke
//         ... Start/Search/Task View buttons, task buttons, badges, gripper ...
//         Popup                    <- hover previews and other flyouts
//     SystemTray.SystemTrayFrame
//
// The icon and text layer is therefore everything below the content element
// except the background and except flyout hosts. The class names of the
// individual containers differ between Windows builds, so instead of matching a
// hard-coded list of class names the tree is walked and every subtree which
// contains neither the background nor a flyout host is adjusted as a unit.
void CollectIconLayers(FrameworkElement element,
                       FrameworkElement content,
                       double bandTop,
                       double bandBottom,
                       int depth,
                       std::vector<FrameworkElement>& elements) {
    constexpr int kMaxDepth = 12;

    if (!element || depth > kMaxDepth) {
        return;
    }

    if (IsFlyoutHost(element) || IsBackgroundRectangle(element) ||
        IsSystemChromeElement(element) ||
        ContainsElement(elements, element) ||
        HasCollectedAncestor(elements, element)) {
        return;
    }

    bool visible = true;
    try {
        visible = element.Visibility() == Visibility::Visible &&
                  element.ActualWidth() > 0 && element.ActualHeight() > 0;
    } catch (...) {
    }

    if (visible && IsPureIconSubtree(element)) {
        // Elements outside of the visible taskbar area leak the adjustment
        // outside of the taskbar (a gray strip above it), so only elements
        // fully inside the visible band qualify.
        bool withinVisibleTaskbar = true;
        try {
            auto origin = element.TransformToVisual(content)
                              .TransformPoint(
                                  winrt::Windows::Foundation::Point{0, 0});
            withinVisibleTaskbar =
                origin.X >= -1 && origin.Y >= -1 &&
                origin.X + element.ActualWidth() <= content.ActualWidth() + 1 &&
                origin.Y + element.ActualHeight() <= content.ActualHeight() + 1;

            // The island extends above the visible taskbar: the element must
            // also lie within the background area and not just fit into the
            // island.
            if (withinVisibleTaskbar && bandBottom > bandTop) {
                withinVisibleTaskbar =
                    origin.Y >= bandTop - 1 &&
                    origin.Y + element.ActualHeight() <= bandBottom + 1;
            }
        } catch (...) {
        }

        if (withinVisibleTaskbar) {
            elements.push_back(element);
            Wh_Log(L"Icon layer: %s %s", DescribeElement(element).c_str(),
                   DescribeRect(element, content).c_str());
            return;
        }
    }

    // Either a mixed container (it holds the background or a flyout), or an
    // element which is not fully inside the visible taskbar: descend into the
    // children.
    int childrenCount = 0;
    try {
        childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    } catch (...) {
        return;
    }

    for (int i = 0; i < childrenCount; i++) {
        FrameworkElement child = nullptr;
        try {
            child = Media::VisualTreeHelper::GetChild(element, i)
                        .try_as<FrameworkElement>();
        } catch (...) {
        }
        CollectIconLayers(child, content, bandTop, bandBottom, depth + 1,
                          elements);
    }
}

// Dumps a bounded part of the visual tree to the log, one line per element:
// depth, class name and (if set) the element name. Used for diagnostics when
// expected elements are missing on an unknown Windows build.
void DumpVisualTree(FrameworkElement root, int maxDepth, int maxElements) {
    int logged = 0;

    auto walk = [&](auto&& self, FrameworkElement element, int depth) -> void {
        if (!element || depth > maxDepth || logged >= maxElements) {
            return;
        }

        logged++;
        std::wstring line((size_t)depth * 2, L' ');
        line += DescribeElement(element);
        Wh_Log(L"%s", line.c_str());

        int childrenCount = 0;
        try {
            childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
        } catch (...) {
        }

        for (int i = 0; i < childrenCount && logged < maxElements; i++) {
            FrameworkElement child = nullptr;
            try {
                child = Media::VisualTreeHelper::GetChild(element, i)
                            .try_as<FrameworkElement>();
            } catch (...) {
            }
            self(self, child, depth + 1);
        }
    };

    walk(walk, root, 0);
}

// Collects the containers which form the icons and text layer. Elements which
// contain the background, or which are already covered by another collected
// element, are skipped, so that an adjustment is never applied twice and the
// background is never affected by a foreground adjustment.
//
// bandTop/bandBottom are the vertical bounds of the visible taskbar area in
// content coordinates, as given by the background rectangle. Elements outside
// of them are split up instead of being adjusted as a whole: the XAML island
// which hosts the taskbar is larger than the visible taskbar (the extra space
// above it hosts flyouts), and an adjustment which lands there shows up as a
// gray strip above the taskbar.
std::vector<FrameworkElement> CollectForegroundElements(
    FrameworkElement content, double bandTop, double bandBottom) {
    std::vector<FrameworkElement> elements;
    CollectIconLayers(content, content, bandTop, bandBottom, 0, elements);

    Wh_Log(L"Icon layers: %d, content %.0fx%.0f, visible band %.0f..%.0f",
           (int)elements.size(), content.ActualWidth(), content.ActualHeight(),
           bandTop, bandBottom);

    if (elements.empty()) {
        static bool dumped = false;
        if (!dumped) {
            dumped = true;
            Wh_Log(L"Icon and text layer not found");
        }
    }

    return elements;
}

void RestoreState(AppearanceState& state) {
    if (state.backgroundFillReplaced) {
        if (auto backgroundFill = state.backgroundFill.get()) {
            if (auto rect = backgroundFill.try_as<Shapes::Rectangle>()) {
                // The fill was replaced with a backdrop brush for the
                // brightness adjustment. Put the saved brush back by
                // assignment: template elements hold their values as local
                // values, so ClearValue would wipe the fill instead of
                // reverting it.
                rect.Fill(state.originalFillBrush);
            }
        }
        state.originalFillBrush = nullptr;
        state.backgroundFillReplaced = false;
    }

    if (auto backgroundFill = state.backgroundFill.get()) {
        backgroundFill.Opacity(state.originalFillOpacity);
    }

    if (state.topLineHidden) {
        if (auto topLine = state.topLine.get()) {
            // The line was hidden with Opacity only; assigning the saved value
            // back brings it back verbatim.
            topLine.Opacity(state.originalTopLineOpacity);
        }
    }

    if (auto grip = state.grip.get()) {
        // Unregister the re-hide callback before restoring the visibility,
        // otherwise the callback would immediately collapse it again.
        if (state.gripVisibilityToken) {
            grip.UnregisterPropertyChangedCallback(
                UIElement::VisibilityProperty(), state.gripVisibilityToken);
            state.gripVisibilityToken = 0;
        }
        grip.Visibility(state.originalGripVisibility);
    }

    for (auto& [element, originalOpacity] : state.foregroundOpacity) {
        if (auto elem = element.get()) {
            elem.Opacity(originalOpacity);
        }
    }
}

// Restores all tracked taskbars and forgets about them. Used when all settings
// are back to their defaults, or when the mod is unloading, so that nothing is
// left adjusted.
void RestoreAllStates() {
    for (auto& state : g_states) {
        RestoreState(state);
    }

    g_states.clear();
}

std::pair<AppearanceState*, size_t> FindStateForRoot(
    FrameworkElement root) {
    for (size_t i = 0; i < g_states.size(); i++) {
        if (auto stateRoot = g_states[i].root.get();
            stateRoot && stateRoot == root) {
            return {&g_states[i], i};
        }
    }

    return {nullptr, 0};
}

bool IsNeutral() {
    return g_settings.backgroundBrightness == 0 &&
           g_settings.backgroundOpacity == 100 &&
           g_settings.iconsOpacity == 100 && g_settings.topLine;
}

// The vertical bounds of the visible taskbar area in content coordinates.
// The island which hosts the taskbar is larger than the visible taskbar (the
// extra space above it hosts flyouts), and an adjustment which lands there
// shows up as a gray strip above the taskbar. The background rectangle is the
// visible taskbar, so it defines the band.
std::pair<double, double> GetVisibleBand(FrameworkElement content,
                                         FrameworkElement backgroundFill) {
    double bandTop = 0.0;
    double bandBottom = content.ActualHeight();
    try {
        auto origin = backgroundFill.TransformToVisual(content)
                          .TransformPoint(
                              winrt::Windows::Foundation::Point{0, 0});
        bandTop = origin.Y;
        bandBottom = origin.Y + backgroundFill.ActualHeight();
    } catch (...) {
    }
    return {bandTop, bandBottom};
}

// Looks the thin line at the top edge of the taskbar up and hides it on
// request. The line is drawn by the taskbar border rectangle
// (Rectangle#BackgroundStroke, a sibling of Rectangle#BackgroundFill). It is
// looked up among the siblings of BackgroundFill first, as the taskbar flyouts
// contain rectangles with the same names. Hiding only changes the opacity, so
// the original look can always be restored verbatim.
void ApplyTopLineStyle(FrameworkElement content,
                       FrameworkElement backgroundFillElem,
                       AppearanceState& state) {
    FrameworkElement topLineElem = nullptr;
    if (auto parent =
            Media::VisualTreeHelper::GetParent(backgroundFillElem)
                .try_as<FrameworkElement>()) {
        int childrenCount =
            Media::VisualTreeHelper::GetChildrenCount(parent);
        for (int i = 0; i < childrenCount; i++) {
            auto child = Media::VisualTreeHelper::GetChild(parent, i)
                             .try_as<FrameworkElement>();
            if (child && child.Name() == L"BackgroundStroke") {
                topLineElem = child;
                break;
            }
        }
    }
    if (!topLineElem) {
        topLineElem = FindDescendantByName(content, L"BackgroundStroke");
    }

    if (!topLineElem) {
        Wh_Log(L"BackgroundStroke not found");
        return;
    }

    auto topLineRect = topLineElem.try_as<Shapes::Rectangle>();
    if (!topLineRect) {
        Wh_Log(L"BackgroundStroke is not a Rectangle");
        return;
    }

    state.topLine = winrt::make_weak(topLineRect.as<FrameworkElement>());
    state.originalTopLineOpacity = topLineRect.Opacity();

    if (!g_settings.topLine) {
        state.topLineHidden = true;
        // Hide the line with Opacity only. Visibility, Fill and
        // StrokeThickness are never written: they belong to the taskbar
        // template, and overwriting them destroys the original look for good.
        topLineRect.Opacity(0.0);
        Wh_Log(L"Top line hidden");
    }
}

// Records and hides the drag grip handle: the small gray rounded handle at the
// top center of the taskbar (Rectangle#Gripper inside Taskbar.Gripper#
// GripperControl). It is part of Windows (it shows while the taskbar is
// unlocked) and is not affected by the opacity adjustments, so it is always
// hidden while the mod is active - a gray handle would otherwise stick out of
// an otherwise adjusted taskbar. The original state is recorded so that
// RestoreState can bring it back.
void ApplyGripStyle(FrameworkElement content, AppearanceState& state) {
    auto gripElem = FindDescendantByName(content, L"Gripper");
    if (!gripElem) {
        Wh_Log(L"Grip handle not found");
        return;
    }

    state.grip = winrt::make_weak(gripElem);
    state.originalGripVisibility = gripElem.Visibility();

    gripElem.Visibility(Visibility::Collapsed);

    // Windows keeps the grip visible after lock/unlock cycles and re-shows it
    // when the taskbar is unlocked again. Re-hide it whenever it shows up
    // again while the mod is active.
    state.gripVisibilityToken = gripElem.RegisterPropertyChangedCallback(
        UIElement::VisibilityProperty(),
        [](DependencyObject sender, DependencyProperty) {
            if (auto elem = sender.try_as<FrameworkElement>();
                elem && elem.Visibility() == Visibility::Visible) {
                elem.Visibility(Visibility::Collapsed);
            }
        });

    Wh_Log(L"Grip handle hidden");
}

// Adjusts the background rectangle: opacity directly, brightness by replacing
// the fill with a live backdrop brush filtered by a color matrix effect. The
// original fill is saved in the state so that RestoreState can put it back by
// assignment.
void ApplyBackgroundStyle(Shapes::Rectangle const& backgroundFill,
                          AppearanceState& state) {
    bool hasOpacity = g_settings.backgroundOpacity != 100;
    bool hasBrightness = g_settings.backgroundBrightness != 0;

    if (hasOpacity) {
        backgroundFill.Opacity(g_settings.backgroundOpacity / 100.0);
    }

    if (!hasBrightness) {
        return;
    }

    try {
        auto compositor =
            winrt::Windows::UI::Xaml::Hosting::ElementCompositionPreview::
                GetElementVisual(backgroundFill)
                    .Compositor();
        float brightness = -g_settings.backgroundBrightness / 100.0f;
        auto brush = winrt::make<BackdropAdjustBrush>(compositor, brightness);
        state.originalFillBrush = backgroundFill.Fill();
        backgroundFill.Fill(brush);
        state.backgroundFillReplaced = true;
        Wh_Log(L"Background fill replaced (brightness=%.2f)", brightness);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Failed to create backdrop brush: %08X (error)",
               (unsigned)e.code().value);
        // Keep going with the other adjustments.
    } catch (...) {
        Wh_Log(L"Failed to create backdrop brush (unknown error)");
        // Keep going with the other adjustments.
    }
}

// Adjusts the icon and text elements: opacity directly.
void ApplyForegroundStyle(std::vector<FrameworkElement> const& elements,
                          AppearanceState& state) {
    Wh_Log(L"Foreground elements found: %d", (int)elements.size());

    if (g_settings.iconsOpacity == 100) {
        return;
    }

    for (auto& element : elements) {
        Wh_Log(L"Foreground element: %s", DescribeElement(element).c_str());

        state.foregroundOpacity.emplace_back(winrt::make_weak(element),
                                             element.Opacity());
        element.Opacity(g_settings.iconsOpacity / 100.0);
    }
}

// Locates the taskbar background rectangle. The known tree path is walked
// explicitly first: flyouts contain rectangles with the same names, and a
// depth-first search just takes whatever it reaches first. The search falls
// back to a bounded tree walk for Windows builds with a different structure.
FrameworkElement FindBackgroundFill(FrameworkElement content) {
    FrameworkElement child = content;
    if ((child = FindChildByName(child, L"RootGrid")) &&
        (child = FindChildByName(child, L"BackgroundControl")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.Grid")) &&
        (child = FindChildByName(child, L"BackgroundFill"))) {
        return child;
    }

    return FindDescendantByName(content, L"BackgroundFill");
}

// The XAML tree can be mutated from several threads and elements can be torn
// down while being touched. A top level catch keeps a WinRT exception from
// propagating into the taskbar thread and crashing Explorer; the retry logic
// treats a failure the same way as a tree which is not ready yet.
bool ApplyStyleImpl(XamlRoot xamlRoot);

bool ApplyStyle(XamlRoot xamlRoot) {
    try {
        return ApplyStyleImpl(xamlRoot);
    } catch (...) {
        Wh_Log(L"ApplyStyle failed with an exception");
        return false;
    }
}

bool ApplyStyleImpl(XamlRoot xamlRoot) {
    auto content = xamlRoot.Content().as<FrameworkElement>();
    if (!content) {
        return false;
    }

    Wh_Log(L"Applying settings: background brightness=%d, opacity=%d, "
           L"icons opacity=%d",
           g_settings.backgroundBrightness, g_settings.backgroundOpacity,
           g_settings.iconsOpacity);

    // The visual tree is keyed by the root content element.
    // When all settings are back to their defaults, or when the mod is
    // unloading, everything must be restored to its original appearance.
    if (g_unloading || IsNeutral()) {
        Wh_Log(L"%s, restoring original appearance",
               g_unloading ? L"Mod is unloading" : L"All settings are neutral");
        RestoreAllStates();
        return true;
    }

    auto [state, stateIndex] = FindStateForRoot(content);

    // Locate the taskbar background rectangle.
    auto backgroundFillElem = FindBackgroundFill(content);
    if (!backgroundFillElem) {
        Wh_Log(L"BackgroundFill not found (unsupported taskbar?)");
        return false;
    }

    auto backgroundFill = backgroundFillElem.try_as<Shapes::Rectangle>();
    if (!backgroundFill) {
        Wh_Log(L"BackgroundFill is not a Rectangle");
        return false;
    }

    // The tree is populated (the background rectangle exists), so dump it once
    // per mod load: the log then always contains the real element names and
    // the real structure of the Windows build it was made on.
    if (!g_treeDumped.exchange(true)) {
        DumpVisualTree(content, 7, 260);
    }

    bool hasForegroundOpacity = g_settings.iconsOpacity != 100;

    // The icon and text layer is collected up front. If it is requested but
    // comes back empty, the taskbar content exists while not being laid out
    // yet, i.e. all of its elements are still zero-sized. In that case nothing
    // is modified at all, so that the retry which covers this state doesn't
    // make the background flicker by re-applying itself over and over.
    std::vector<FrameworkElement> foregroundElements;
    if (hasForegroundOpacity) {
        auto [bandTop, bandBottom] =
            GetVisibleBand(content, backgroundFillElem);
        foregroundElements =
            CollectForegroundElements(content, bandTop, bandBottom);
        if (foregroundElements.empty()) {
            Wh_Log(L"Icon and text layer is empty, taskbar not laid out yet");
            return false;
        }
    }

    // Restore any previous state for this root before re-applying.
    if (state) {
        RestoreState(*state);
        g_states.erase(g_states.begin() + stateIndex);
    }

    AppearanceState newState;
    newState.root = winrt::make_weak(content);
    newState.backgroundFill =
        winrt::make_weak(backgroundFill.as<FrameworkElement>());
    newState.originalFillOpacity = backgroundFill.Opacity();

    ApplyTopLineStyle(content, backgroundFillElem, newState);

    ApplyGripStyle(content, newState);

    ApplyBackgroundStyle(backgroundFill, newState);

    if (hasForegroundOpacity) {
        ApplyForegroundStyle(foregroundElements, newState);
    }

    g_states.push_back(std::move(newState));
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Taskbar plumbing (from Taskbar tray auto-hide (show on hover) by m417z)

void* CTaskBand_ITaskListWndSite_vftable;

void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
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

// Defined further down, next to the retry timer which uses them.
void WINAPI ApplyPassOnTaskbarThread(void* parameter);
void ScheduleApplyRetry(HWND hTaskbarWnd);
void StopApplyRetry(HWND hTaskbarWnd);

// The outcome of one pass over the taskbar windows of the current thread. The
// taskbar XAML tree is created asynchronously, so a pass which runs too early
// finds the taskbar but no elements, and reports allReady == false.
struct ApplyPassResult {
    bool sawTaskbar = false;
    bool allReady = true;
};

void WINAPI ApplyPassOnTaskbarThread(void* parameter) {
    ApplyPassResult* result = (ApplyPassResult*)parameter;

    Wh_Log(L"Applying settings");

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            ApplyPassResult* result = (ApplyPassResult*)lParam;

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            XamlRoot xamlRoot = nullptr;
            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                xamlRoot = GetTaskbarXamlRoot(hWnd);
            } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
            } else {
                return TRUE;
            }

            result->sawTaskbar = true;

            if (g_unloading.load()) {
                // Stop a retry which is still in flight, so that no timer
                // callback can outlive the mod, and restore unconditionally:
                // RestoreAllStates() works off the stored weak refs and does
                // not need a XamlRoot. A skipped restore would leave the
                // taskbar holding the background fill brush, which lives in
                // the mod image and crashes Explorer once the mod is unloaded.
                StopApplyRetry(hWnd);
                RestoreAllStates();
                return TRUE;
            }

            if (!xamlRoot) {
                Wh_Log(L"Getting XamlRoot failed");
                result->allReady = false;
            } else if (!ApplyStyle(xamlRoot)) {
                // The taskbar XAML tree is created asynchronously, so on a
                // fresh start-up the elements are still missing here. Retry
                // until they exist instead of silently doing nothing.
                Wh_Log(L"ApplyStyle failed (taskbar XAML tree not ready yet?)");
                result->allReady = false;
                ScheduleApplyRetry(hWnd);
            }

            return TRUE;
        },
        (LPARAM)result);
}

////////////////////////////////////////////////////////////////////////////////
// Deferred retry
//
// TrayUI::StartTaskbar returns while the taskbar XAML tree is still being
// created: at that point its root only holds an empty Taskbar.TaskbarFrame and
// an empty SystemTray.SystemTrayFrame, and neither the background rectangles
// nor the task buttons nor the tray icons exist yet. The same happens on the
// first pass after a recompile, because the taskbar is rebuilt from scratch.
//
// Such a pass is therefore repeated until the tree is populated. The retry runs
// as a timer of the taskbar window itself, so it fires on the taskbar window
// thread and the XAML work stays where it belongs. No other thread is involved,
// which also means that unloading can never deadlock against a retry in
// flight.

// The timer ID must not collide with the timers of the taskbar itself or with
// timers of other mods on the same window. An atom is unique process-wide;
// the unusual constant is only a fallback for the unlikely case that the atom
// could not be created.
UINT_PTR GetApplyRetryTimerId() {
    static UINT_PTR timerId = []() -> UINT_PTR {
        ATOM atom =
            GlobalAddAtom(L"Windhawk_TaskbarAppearanceTuner_RetryTimer");
        return atom ? (UINT_PTR)atom : 0x7A9C;
    }();
    return timerId;
}

constexpr UINT kApplyRetryIntervalMs = 150;
constexpr ULONGLONG kApplyRetryTotalMs = 8000;

// The retry deadline is stored per taskbar window (there can be several on
// multi-monitor systems, and a shared global would make whichever timer fires
// first drive the retry window of all of them). Only ever touched on the
// taskbar window thread.
constexpr const wchar_t* kApplyRetryDeadlineProp =
    L"Windhawk_TaskbarAppearanceTuner_RetryDeadline";

ULONGLONG GetApplyRetryDeadline(HWND hWnd) {
    auto value = (ULONGLONG)(ULONG_PTR)GetProp(hWnd, kApplyRetryDeadlineProp);
    return value;
}

void SetApplyRetryDeadline(HWND hWnd, ULONGLONG deadline) {
    SetProp(hWnd, kApplyRetryDeadlineProp,
            (HANDLE)(ULONG_PTR)deadline);
}

void RemoveApplyRetryDeadline(HWND hWnd) {
    RemoveProp(hWnd, kApplyRetryDeadlineProp);
}

// The retry timer is handled in a subclass procedure rather than in a
// TIMERPROC: a TIMERPROC lives in the mod image and would crash Explorer if a
// timer outlived the mod, while the subclass is installed and removed
// deterministically and WM_TIMER is only dispatched while it is installed.
LRESULT CALLBACK ApplyRetrySubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                        LPARAM lParam, DWORD_PTR) {
    if (uMsg == WM_TIMER && wParam == GetApplyRetryTimerId()) {
        ApplyPassResult result;
        ApplyPassOnTaskbarThread(&result);

        if ((result.sawTaskbar && result.allReady) ||
            GetTickCount64() >= GetApplyRetryDeadline(hWnd)) {
            Wh_Log(L"Apply retry finished (tree ready: %s)",
                   result.allReady ? L"yes" : L"no");
            StopApplyRetry(hWnd);
        }

        return 0;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Must be called on the taskbar window thread.
void ScheduleApplyRetry(HWND hTaskbarWnd) {
    // The deadline is only ever set once. The passes driven by the timer itself
    // report the same "not ready" state, and must not extend the retry window,
    // otherwise the timer would never stop.
    if (GetApplyRetryDeadline(hTaskbarWnd) == 0) {
        SetApplyRetryDeadline(hTaskbarWnd,
                              GetTickCount64() + kApplyRetryTotalMs);
    }

    // The subclass must be installed before the timer: WM_TIMER is only
    // handled while the subclass is in place.
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            hTaskbarWnd, ApplyRetrySubclassProc, 0)) {
        Wh_Log(L"Failed to subclass the taskbar window");
        return;
    }

    // Idempotent: re-arming a running timer just restarts it.
    if (SetTimer(hTaskbarWnd, GetApplyRetryTimerId(), kApplyRetryIntervalMs,
                 nullptr)) {
        Wh_Log(L"Taskbar XAML tree not ready, retrying every %u ms",
               kApplyRetryIntervalMs);
    } else {
        Wh_Log(L"Failed to start the apply retry timer");
    }
}

// Must be called on the taskbar window thread.
void StopApplyRetry(HWND hTaskbarWnd) {
    KillTimer(hTaskbarWnd, GetApplyRetryTimerId());
    RemoveApplyRetryDeadline(hTaskbarWnd);
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hTaskbarWnd,
                                                    ApplyRetrySubclassProc);
}

using TrayUI_StartTaskbar_t = void(WINAPI*)(void* pThis);
TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original;
void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    Wh_Log(L">");

    TrayUI_StartTaskbar_Original(pThis);

    // TrayUI::StartTaskbar runs on the taskbar window thread.
    ApplyPassResult result;
    ApplyPassOnTaskbarThread(&result);
}

using CSecondaryTray_GetTrayWindow_t = HWND(WINAPI*)(void* pThis);
CSecondaryTray_GetTrayWindow_t CSecondaryTray_GetTrayWindow_Original;

using CSecondaryTray_InitModelAndHost_t = void(WINAPI*)(void* pThis,
                                                        void* taskbarModel);
CSecondaryTray_InitModelAndHost_t CSecondaryTray_InitModelAndHost_Original;
void WINAPI CSecondaryTray_InitModelAndHost_Hook(void* pThis,
                                                 void* taskbarModel) {
    Wh_Log(L">");

    CSecondaryTray_InitModelAndHost_Original(pThis, taskbarModel);

    HWND taskbarWnd = CSecondaryTray_GetTrayWindow_Original(pThis);

    auto xamlRoot = GetSecondaryTaskbarXamlRoot(taskbarWnd);
    if (!xamlRoot) {
        Wh_Log(L"Getting XamlRoot failed");
        return;
    }

    if (!ApplyStyle(xamlRoot)) {
        // The secondary taskbar runs into the same start-up race as the main
        // one: its XAML tree is still being created.
        ScheduleApplyRetry(taskbarWnd);
    }
}

void ApplySettings(HWND hTaskbarWnd) {
    ApplyPassResult result;
    if (!RunFromWindowThread(hTaskbarWnd, ApplyPassOnTaskbarThread, &result)) {
        Wh_Log(L"RunFromWindowThread failed");
    }
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
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
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

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

void LoadSettings() {
    // Values are clamped so that an out-of-range configuration cannot make
    // the taskbar more than fully transparent or more than fully opaque.
    g_settings.backgroundBrightness =
        std::clamp(Wh_GetIntSetting(L"backgroundBrightness"), 0, 100);
    g_settings.backgroundOpacity =
        std::clamp(Wh_GetIntSetting(L"backgroundOpacity"), 0, 100);
    g_settings.iconsOpacity =
        std::clamp(Wh_GetIntSetting(L"iconsOpacity"), 0, 100);
    g_settings.topLine = Wh_GetIntSetting(L"topLine") != 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    // Wh_ModInit can run again without the DLL having been unloaded.
    g_treeDumped = false;
    g_states.clear();

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    // RestoreAllStates() already ran on the taskbar UI thread during unload
    // (see ApplyPassOnTaskbarThread), so the vector is empty here. The state
    // holds only weak references and value types, so even if something is
    // left over, the automatic destructor at process shutdown cannot touch a
    // live XAML object.
    g_states.clear();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}

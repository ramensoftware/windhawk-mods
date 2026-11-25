// ==WindhawkMod==
// @id              dynamic-taskbar
// @name            Dynamic Taskbar
// @description     TranslucentTB-style dynamic taskbar appearance per monitor (Desktop, Visible Window, Maximized Window, rules, ignored windows, Peek & line controls)
// @version         1.0.0
// @author          KnoxFury
// @github          https://github.com/KnoxFury
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -ldwmapi -lgdi32 -lole32 -loleaut32 -lpsapi -lruntimeobject -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Dynamic Taskbar

Dynamically customize the Windows 11 taskbar appearance per monitor based on window
state—similar to **TranslucentTB**, running directly inside `explorer.exe` with no
separate background process and smooth fade transitions between states.

## Features

### Per-monitor dynamic states
Each monitor's taskbar (`Shell_TrayWnd` and `Shell_SecondaryTrayWnd`) is evaluated
independently using event-driven window hooks (`SetWinEventHook`), in priority order:

1. **Maximized window** (`maximizedWindowAppearance`) — Active when a non-ignored window is maximized on that monitor.
2. **Visible window** (`visibleWindowAppearance`) — Active when a non-ignored window is visible on that monitor.
3. **Desktop** (`desktopAppearance`) — Active when no non-ignored windows are visible on that monitor (and optionally when **Task View** is open).

### Accent modes
- **`clear`** — Transparent or custom-tinted background (`#RRGGBBAA`, e.g. `#00000000` for fully transparent).
- **`blur`** — Gaussian blur backdrop with configurable **Blur radius** and tint color.
- **`acrylic`** — Fluent Acrylic backdrop with blur, tint, luminosity, and subtle noise texture.
- **`opaque`** — Solid color background using your chosen `#RRGGBBAA` color at full opacity.
- **`normal`** — Restores the default Windows 11 taskbar background.

### Taskbar details & animations
- **Show Aero Peek button** (`showPeek`) — Show or hide the far-corner "Show desktop" strip per state.
- **Show taskbar border line** (`showLine`) — Show or hide the 1 px taskbar border line (`BackgroundStroke`) per state.
- **Smooth transitions** (`transitionDurationMs`) — Configurable eased cross-fade / opacity animation (`180 ms` by default; set to `0` to switch instantly). Handles rapid mid-transition reversals seamlessly.
- **Task View follows Desktop** (`taskViewFollowsDesktop`) — Uses your Desktop appearance while Task View (`Win + Tab`) is open.

### Per-window rules & ignore lists
- **Window rules** (under both **Visible window appearance** and **Maximized window appearance**): Override accent mode, color, blur radius, Peek, and border line for specific apps matched by:
  - **Process name** (e.g. `notepad.exe` or `notepad`)
  - **Window class** (exact case-insensitive match)
  - **Window title** (case-insensitive substring match)
- **Ignored windows** (`ignoredWindows`): Exclude specific windows by class, title substring, or process name so background/widget windows don't trigger Visible or Maximized states. Hidden, minimized, tool, and DWM-cloaked windows (including windows on other virtual desktops) are automatically ignored.

## Compatibility notes
- Exit or uninstall **TranslucentTB** before enabling this mod so nothing competes over the taskbar background elements.
- Uses `CTaskBand::GetTaskbarHost` / `CSecondaryTaskBand::GetTaskbarHost` instead of `InitializeXamlDiagnosticsEx`, leaving XAML diagnostics free for other tools and cleanly restoring all original XAML properties if the mod is disabled or unloaded.
- Designed for **Windows 11 (x86-64)**.

### Credits & License
Licensed under the **GNU General Public License v3.0 (GPL-3.0)**.
- Blur/Acrylic Direct2D composition brush (`XamlBlurBrush`) adapted from [**TranslucentTB**](https://github.com/TranslucentTB/TranslucentTB) and [**Windows 11 Taskbar Styler**](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/windows-11-taskbar-styler.wh.cpp) by **m417z**.
- `TaskbarHost` XAML root lookup and taskbar thread dispatch patterns adapted from **m417z**'s Windows 11 taskbar mods and **EDM115**'s [**Taskbar multi-tray**](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-multi-tray.wh.cpp).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- desktopAppearance:
  - accent: clear
    $name: Accent mode
    $options:
    - clear: Clear
    - blur: Blur
    - acrylic: Acrylic
    - opaque: Opaque
    - normal: Normal (Windows default)
  - color: "#00000000"
    $name: Color (#RRGGBBAA)
  - showPeek: false
    $name: Show Aero Peek button
  - showLine: false
    $name: Show taskbar border line
  - blurRadius: 9
    $name: Blur radius
  $name: Desktop appearance
- visibleWindowAppearance:
  - enabled: true
    $name: Enabled
  - accent: clear
    $name: Accent mode
    $options:
    - clear: Clear
    - blur: Blur
    - acrylic: Acrylic
    - opaque: Opaque
    - normal: Normal (Windows default)
  - color: "#00000000"
    $name: Color (#RRGGBBAA)
  - showPeek: true
    $name: Show Aero Peek button
  - showLine: false
    $name: Show taskbar border line
  - blurRadius: 9
    $name: Blur radius
  - rules:
    - - matchType: process_name
        $name: Match by
        $options:
        - process_name: Process name (e.g. notepad.exe)
        - window_class: Window class
        - window_title: Window title (contains)
      - matchValue: ""
        $name: Match value
      - accent: clear
        $name: Accent mode
        $options:
        - clear: Clear
        - blur: Blur
        - acrylic: Acrylic
        - opaque: Opaque
        - normal: Normal (Windows default)
      - color: "#00000000"
        $name: Color (#RRGGBBAA)
      - showPeek: true
        $name: Show Aero Peek button
      - showLine: false
        $name: Show taskbar border line
      - blurRadius: 9
        $name: Blur radius
    $name: Window rules
  $name: Visible window appearance
- maximizedWindowAppearance:
  - enabled: true
    $name: Enabled
  - accent: normal
    $name: Accent mode
    $options:
    - clear: Clear
    - blur: Blur
    - acrylic: Acrylic
    - opaque: Opaque
    - normal: Normal (Windows default)
  - color: "#373535FF"
    $name: Color (#RRGGBBAA)
  - showPeek: true
    $name: Show Aero Peek button
  - showLine: true
    $name: Show taskbar border line
  - blurRadius: 9
    $name: Blur radius
  - rules:
    - - matchType: process_name
        $name: Match by
        $options:
        - process_name: Process name (e.g. code.exe)
        - window_class: Window class
        - window_title: Window title (contains)
      - matchValue: ""
        $name: Match value
      - accent: normal
        $name: Accent mode
        $options:
        - clear: Clear
        - blur: Blur
        - acrylic: Acrylic
        - opaque: Opaque
        - normal: Normal (Windows default)
      - color: "#373535FF"
        $name: Color (#RRGGBBAA)
      - showPeek: true
        $name: Show Aero Peek button
      - showLine: true
        $name: Show taskbar border line
      - blurRadius: 9
        $name: Blur radius
    $name: Window rules
  $name: Maximized window appearance
- ignoredWindows:
  - windowClass: [""]
    $name: Window classes to ignore
  - windowTitle: [""]
    $name: Window titles to ignore (substring)
  - processName: [""]
    $name: Process names to ignore (e.g. app.exe)
  $name: Ignored windows
- taskViewFollowsDesktop: true
  $name: Task View follows Desktop appearance
- transitionDurationMs: 180
  $name: Transition animation duration (ms)
  $description: Smooth fade duration when switching taskbar states (0 to disable, recommended 150-250)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cwctype>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <initguid.h>

#include <commctrl.h>
#include <d2d1_1.h>
#include <dwmapi.h>
#include <psapi.h>
#include <roapi.h>
#include <shlwapi.h>
#include <windows.graphics.effects.h>
#include <winstring.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.System.Power.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;
namespace wf = winrt::Windows::Foundation;
namespace wge = winrt::Windows::Graphics::Effects;
namespace wuc = winrt::Windows::UI::Composition;
namespace wuxh = winrt::Windows::UI::Xaml::Hosting;
namespace awge = ABI::Windows::Graphics::Effects;

template <>
inline constexpr winrt::guid winrt::impl::guid_v<
    winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>{
    winrt::impl::guid_v<winrt::Windows::Foundation::IPropertyValue>};

template <auto fn>
struct deleter_from_fn {
    template <typename T>
    constexpr void operator()(T* arg) const {
        fn(arg);
    }
};
using string_setting_unique_ptr =
    std::unique_ptr<const WCHAR[], deleter_from_fn<Wh_FreeStringSetting>>;

// ============================================================================
//  Direct2D Composition Effects & XamlBlurBrush (adapted from TranslucentTB / Taskbar Styler)
// ============================================================================

#ifndef BUILD_WINDOWS
namespace ABI {
#endif
namespace Windows {
namespace Graphics {
namespace Effects {

typedef interface IGraphicsEffectSource IGraphicsEffectSource;
typedef interface IGraphicsEffectD2D1Interop IGraphicsEffectD2D1Interop;

typedef enum GRAPHICS_EFFECT_PROPERTY_MAPPING {
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
DECLARE_INTERFACE_IID_(IGraphicsEffectD2D1Interop, IUnknown, "2FC57384-A068-44D7-A331-30982FCF7177") {
    STDMETHOD(GetEffectId)(_Out_ GUID* id) PURE;
    STDMETHOD(GetNamedPropertyMapping)(
        LPCWSTR name, _Out_ UINT* index,
        _Out_ GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) PURE;
    STDMETHOD(GetPropertyCount)(_Out_ UINT* count) PURE;
    STDMETHOD(GetProperty)(
        UINT index,
        _Outptr_ winrt::impl::abi_t<wf::IPropertyValue>** value) PURE;
    STDMETHOD(GetSource)(
        UINT index, _Outptr_ IGraphicsEffectSource** source) PURE;
    STDMETHOD(GetSourceCount)(_Out_ UINT* count) PURE;
};

}  // namespace Effects
}  // namespace Graphics
}  // namespace Windows
#ifndef BUILD_WINDOWS
}  // namespace ABI
#endif

template <>
inline constexpr winrt::guid winrt::impl::guid_v<
    ABI::Windows::Graphics::Effects::IGraphicsEffectD2D1Interop>{
    0x2FC57384,
    0xA068,
    0x44D7,
    {0xA3, 0x31, 0x30, 0x98, 0x2F, 0xCF, 0x71, 0x77}};

namespace {

typedef enum MY_D2D1_GAUSSIANBLUR_OPTIMIZATION {
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_SPEED = 0,
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED = 1,
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_QUALITY = 2,
} MY_D2D1_GAUSSIANBLUR_OPTIMIZATION;

struct CompositeEffect
    : winrt::implements<CompositeEffect, wge::IGraphicsEffect,
                        wge::IGraphicsEffectSource,
                        awge::IGraphicsEffectD2D1Interop> {
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) return E_INVALIDARG;
        *id = CLSID_D2D1Composite;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index,
        awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!index || !mapping) return E_INVALIDARG;
        if (std::wstring_view(name) == L"Mode") {
            *index = D2D1_COMPOSITE_PROP_MODE;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }
        return E_INVALIDARG;
    }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = 1;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(
        UINT index,
        winrt::impl::abi_t<wf::IPropertyValue>** value) noexcept override try {
        if (!value) return E_INVALIDARG;
        if (index == D2D1_COMPOSITE_PROP_MODE) {
            *value = wf::PropertyValue::CreateUInt32((UINT32)Mode)
                         .as<winrt::impl::abi_t<wf::IPropertyValue>>()
                         .detach();
            return S_OK;
        }
        return E_BOUNDS;
    } catch (...) {
        return winrt::to_hresult();
    }
    HRESULT STDMETHODCALLTYPE GetSource(
        UINT index, awge::IGraphicsEffectSource** source) noexcept override try {
        if (!source) return E_INVALIDARG;
        winrt::copy_to_abi(Sources.at(index), *reinterpret_cast<void**>(source));
        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = static_cast<UINT>(Sources.size());
        return S_OK;
    }
    winrt::hstring Name() { return m_name; }
    void Name(winrt::hstring name) { m_name = name; }

    std::vector<wge::IGraphicsEffectSource> Sources;
    D2D1_COMPOSITE_MODE Mode = D2D1_COMPOSITE_MODE_SOURCE_OVER;

private:
    winrt::hstring m_name = L"CompositeEffect";
};

struct FloodEffect
    : winrt::implements<FloodEffect, wge::IGraphicsEffect,
                        wge::IGraphicsEffectSource,
                        awge::IGraphicsEffectD2D1Interop> {
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) return E_INVALIDARG;
        *id = CLSID_D2D1Flood;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index,
        awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!index || !mapping) return E_INVALIDARG;
        if (std::wstring_view(name) == L"Color") {
            *index = D2D1_FLOOD_PROP_COLOR;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }
        return E_INVALIDARG;
    }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = 1;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(
        UINT index,
        winrt::impl::abi_t<wf::IPropertyValue>** value) noexcept override try {
        if (!value) return E_INVALIDARG;
        if (index == D2D1_FLOOD_PROP_COLOR) {
            *value = wf::PropertyValue::CreateSingleArray({
                         Color.R / 255.0f,
                         Color.G / 255.0f,
                         Color.B / 255.0f,
                         Color.A / 255.0f,
                     })
                         .as<winrt::impl::abi_t<wf::IPropertyValue>>()
                         .detach();
            return S_OK;
        }
        return E_BOUNDS;
    } catch (...) {
        return winrt::to_hresult();
    }
    HRESULT STDMETHODCALLTYPE GetSource(
        UINT, awge::IGraphicsEffectSource**) noexcept override {
        return E_BOUNDS;
    }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = 0;
        return S_OK;
    }
    winrt::hstring Name() { return m_name; }
    void Name(winrt::hstring name) { m_name = name; }

    winrt::Windows::UI::Color Color{};

private:
    winrt::hstring m_name = L"FloodEffect";
};

struct BorderEffect
    : winrt::implements<BorderEffect, wge::IGraphicsEffect,
                        wge::IGraphicsEffectSource,
                        awge::IGraphicsEffectD2D1Interop> {
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) return E_INVALIDARG;
        *id = CLSID_D2D1Border;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index,
        awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!index || !mapping) return E_INVALIDARG;
        std::wstring_view nameView(name);
        if (nameView == L"ExtendX") {
            *index = D2D1_BORDER_PROP_EDGE_MODE_X;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }
        if (nameView == L"ExtendY") {
            *index = D2D1_BORDER_PROP_EDGE_MODE_Y;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }
        return E_INVALIDARG;
    }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = 2;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(
        UINT index,
        winrt::impl::abi_t<wf::IPropertyValue>** value) noexcept override try {
        if (!value) return E_INVALIDARG;
        switch (index) {
            case D2D1_BORDER_PROP_EDGE_MODE_X:
                *value = wf::PropertyValue::CreateUInt32((UINT32)ExtendX)
                             .as<winrt::impl::abi_t<wf::IPropertyValue>>()
                             .detach();
                return S_OK;
            case D2D1_BORDER_PROP_EDGE_MODE_Y:
                *value = wf::PropertyValue::CreateUInt32((UINT32)ExtendY)
                             .as<winrt::impl::abi_t<wf::IPropertyValue>>()
                             .detach();
                return S_OK;
            default:
                return E_BOUNDS;
        }
    } catch (...) {
        return winrt::to_hresult();
    }
    HRESULT STDMETHODCALLTYPE GetSource(
        UINT index, awge::IGraphicsEffectSource** source) noexcept override {
        if (!source) return E_INVALIDARG;
        if (index == 0 && Source) {
            winrt::copy_to_abi(Source, *reinterpret_cast<void**>(source));
            return S_OK;
        }
        return E_BOUNDS;
    }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = 1;
        return S_OK;
    }
    winrt::hstring Name() { return m_name; }
    void Name(winrt::hstring name) { m_name = name; }

    wge::IGraphicsEffectSource Source{nullptr};
    D2D1_BORDER_EDGE_MODE ExtendX = D2D1_BORDER_EDGE_MODE_WRAP;
    D2D1_BORDER_EDGE_MODE ExtendY = D2D1_BORDER_EDGE_MODE_WRAP;

private:
    winrt::hstring m_name = L"BorderEffect";
};

struct GaussianBlurEffect
    : winrt::implements<GaussianBlurEffect, wge::IGraphicsEffect,
                        wge::IGraphicsEffectSource,
                        awge::IGraphicsEffectD2D1Interop> {
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) return E_INVALIDARG;
        *id = CLSID_D2D1GaussianBlur;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index,
        awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!index || !mapping) return E_INVALIDARG;
        std::wstring_view nameView(name);
        if (nameView == L"BlurAmount") {
            *index = D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }
        if (nameView == L"Optimization") {
            *index = D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }
        if (nameView == L"BorderMode") {
            *index = D2D1_GAUSSIANBLUR_PROP_BORDER_MODE;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }
        return E_INVALIDARG;
    }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = 3;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(
        UINT index,
        winrt::impl::abi_t<wf::IPropertyValue>** value) noexcept override try {
        if (!value) return E_INVALIDARG;
        switch (index) {
            case D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION:
                *value = wf::PropertyValue::CreateSingle(BlurAmount)
                             .as<winrt::impl::abi_t<wf::IPropertyValue>>()
                             .detach();
                return S_OK;
            case D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION:
                *value = wf::PropertyValue::CreateUInt32((UINT32)Optimization)
                             .as<winrt::impl::abi_t<wf::IPropertyValue>>()
                             .detach();
                return S_OK;
            case D2D1_GAUSSIANBLUR_PROP_BORDER_MODE:
                *value = wf::PropertyValue::CreateUInt32((UINT32)BorderMode)
                             .as<winrt::impl::abi_t<wf::IPropertyValue>>()
                             .detach();
                return S_OK;
            default:
                return E_BOUNDS;
        }
    } catch (...) {
        return winrt::to_hresult();
    }
    HRESULT STDMETHODCALLTYPE GetSource(
        UINT index, awge::IGraphicsEffectSource** source) noexcept override {
        if (!source) return E_INVALIDARG;
        if (index == 0) {
            winrt::copy_to_abi(Source, *reinterpret_cast<void**>(source));
            return S_OK;
        }
        return E_BOUNDS;
    }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = 1;
        return S_OK;
    }
    winrt::hstring Name() { return m_name; }
    void Name(winrt::hstring name) { m_name = name; }

    wge::IGraphicsEffectSource Source{nullptr};
    float BlurAmount = 9.0f;
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION Optimization =
        MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED;
    D2D1_BORDER_MODE BorderMode = D2D1_BORDER_MODE_SOFT;

private:
    winrt::hstring m_name = L"GaussianBlurEffect";
};

struct ColorMatrixEffect
    : winrt::implements<ColorMatrixEffect, wge::IGraphicsEffect,
                        wge::IGraphicsEffectSource,
                        awge::IGraphicsEffectD2D1Interop> {
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) return E_INVALIDARG;
        *id = CLSID_D2D1ColorMatrix;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index,
        awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!index || !mapping) return E_INVALIDARG;
        std::wstring_view nameView(name);
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
        if (!count) return E_INVALIDARG;
        *count = 3;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(
        UINT index,
        winrt::impl::abi_t<wf::IPropertyValue>** value) noexcept override try {
        if (!value) return E_INVALIDARG;
        switch (index) {
            case D2D1_COLORMATRIX_PROP_COLOR_MATRIX:
                *value = wf::PropertyValue::CreateSingleArray(
                             winrt::array_view<const float>(Matrix, Matrix + 20))
                             .as<winrt::impl::abi_t<wf::IPropertyValue>>()
                             .detach();
                return S_OK;
            case D2D1_COLORMATRIX_PROP_ALPHA_MODE:
                *value = wf::PropertyValue::CreateUInt32(AlphaMode)
                             .as<winrt::impl::abi_t<wf::IPropertyValue>>()
                             .detach();
                return S_OK;
            case D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT:
                *value = wf::PropertyValue::CreateBoolean(ClampOutput)
                             .as<winrt::impl::abi_t<wf::IPropertyValue>>()
                             .detach();
                return S_OK;
            default:
                return E_BOUNDS;
        }
    } catch (...) {
        return winrt::to_hresult();
    }
    HRESULT STDMETHODCALLTYPE GetSource(
        UINT index, awge::IGraphicsEffectSource** source) noexcept override {
        if (!source) return E_INVALIDARG;
        if (index == 0 && Source) {
            winrt::copy_to_abi(Source, *reinterpret_cast<void**>(source));
            return S_OK;
        }
        return E_BOUNDS;
    }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = 1;
        return S_OK;
    }
    winrt::hstring Name() { return m_name; }
    void Name(winrt::hstring name) { m_name = name; }

    wge::IGraphicsEffectSource Source{nullptr};
    float Matrix[20] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
        0, 0, 0, 0,
    };
    uint32_t AlphaMode = D2D1_COLORMATRIX_ALPHA_MODE_PREMULTIPLIED;
    bool ClampOutput = false;

private:
    winrt::hstring m_name = L"ColorMatrixEffect";
};

thread_local float g_noiseStreamDensity =
    std::numeric_limits<float>::quiet_NaN();
thread_local winrt::Windows::Storage::Streams::InMemoryRandomAccessStream
    g_noiseStream{nullptr};

winrt::Windows::Storage::Streams::IRandomAccessStream CreateNoiseStream(
    float density) {
    if (density == g_noiseStreamDensity && g_noiseStream) {
        return g_noiseStream.CloneStream();
    }

    constexpr int kSize = 256;
    constexpr DWORD kBpp = 32;
    constexpr DWORD rowSize = kSize * (kBpp / 8);
    constexpr DWORD dataSize = rowSize * kSize;

    BITMAPFILEHEADER fileHeader{
        .bfType = 0x4D42,
        .bfSize =
            sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dataSize,
        .bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER),
    };

    BITMAPINFOHEADER infoHeader{
        .biSize = sizeof(BITMAPINFOHEADER),
        .biWidth = kSize,
        .biHeight = kSize,
        .biPlanes = 1,
        .biBitCount = kBpp,
        .biSizeImage = dataSize,
    };

    std::vector<uint8_t> pixels(dataSize);
    float safeDensity = std::clamp(density, 0.001f, 1.0f);
    float exponent = 1.0f / safeDensity;

    uint8_t lut[256];
    for (int i = 0; i < 256; i++) {
        lut[i] = static_cast<uint8_t>(std::pow(i / 255.0f, exponent) * 255.0f);
    }

    std::mt19937 rng(0);
    std::uniform_int_distribution<int> dist(0, 255);
    for (size_t i = 0; i < pixels.size(); i += 4) {
        uint8_t gray = lut[dist(rng)];
        pixels[i] = gray;
        pixels[i + 1] = gray;
        pixels[i + 2] = gray;
        pixels[i + 3] = 255;
    }

    winrt::Windows::Storage::Streams::InMemoryRandomAccessStream stream;
    winrt::Windows::Storage::Streams::DataWriter writer(stream);
    writer.WriteBytes(winrt::array_view<const uint8_t>(
        reinterpret_cast<const uint8_t*>(&fileHeader), sizeof(fileHeader)));
    writer.WriteBytes(winrt::array_view<const uint8_t>(
        reinterpret_cast<const uint8_t*>(&infoHeader), sizeof(infoHeader)));
    writer.WriteBytes(pixels);
    writer.StoreAsync().get();
    writer.DetachStream();

    g_noiseStream = std::move(stream);
    g_noiseStreamDensity = density;
    return g_noiseStream.CloneStream();
}

class XamlBlurBrush : public Media::XamlCompositionBrushBaseT<XamlBlurBrush> {
public:
    XamlBlurBrush(UIElement element,
                  float blurAmount,
                  winrt::Windows::UI::Color tint,
                  std::optional<float> tintLuminosityOpacity,
                  std::optional<float> tintSaturation,
                  std::optional<float> noiseOpacity,
                  std::optional<float> noiseDensity,
                  std::optional<winrt::Windows::UI::Color> fallbackColor)
        : m_compositor(
              wuxh::ElementCompositionPreview::GetElementVisual(element)
                  .Compositor()),
          m_blurAmount(blurAmount),
          m_tint(tint),
          m_tintLuminosityOpacity(tintLuminosityOpacity),
          m_tintSaturation(tintSaturation),
          m_noiseOpacity(noiseOpacity),
          m_noiseDensity(noiseDensity),
          m_fallbackColor(fallbackColor) {}

    void OnConnected() {
        if (!CompositionBrush()) {
            CompositionBrush(ShouldUseFallback() ? CreateFallbackBrush()
                                                 : CreateEffectBrush());
        }
    }

    void OnDisconnected() {
        if (const auto brush = CompositionBrush()) {
            brush.Close();
            CompositionBrush(nullptr);
        }
    }

private:
    bool ShouldUseFallback() const {
        if (!m_fallbackColor) {
            return false;
        }
        try {
            winrt::Windows::UI::ViewManagement::UISettings uiSettings;
            return !uiSettings.AdvancedEffectsEnabled();
        } catch (...) {
            return false;
        }
    }

    wuc::CompositionBrush CreateFallbackBrush() {
        return m_compositor.CreateColorBrush(m_fallbackColor.value_or(m_tint));
    }

    wuc::CompositionBrush CreateEffectBrush() {
        auto backdropBrush = m_compositor.CreateBackdropBrush();

        constexpr float kLumaR = 0.2126f;
        constexpr float kLumaG = 0.7152f;
        constexpr float kLumaB = 0.0722f;

        auto blurEffect = winrt::make_self<GaussianBlurEffect>();
        blurEffect->Source = wuc::CompositionEffectSourceParameter(L"backdrop");
        blurEffect->BlurAmount = m_blurAmount;
        blurEffect->Name(L"BlurEffect");

        wge::IGraphicsEffectSource topOfStack = *blurEffect;

        if (m_tintSaturation && *m_tintSaturation != 1.0f) {
            float s = std::max(*m_tintSaturation, 0.0f);
            float invS = 1.0f - s;

            auto satMatrix = winrt::make_self<ColorMatrixEffect>();
            satMatrix->Source = topOfStack;
            auto& m = satMatrix->Matrix;
            m[0] = invS * kLumaR + s; m[1] = invS * kLumaR;     m[2] = invS * kLumaR;     m[3] = 0.0f;
            m[4] = invS * kLumaG;     m[5] = invS * kLumaG + s; m[6] = invS * kLumaG;     m[7] = 0.0f;
            m[8] = invS * kLumaB;     m[9] = invS * kLumaB;     m[10] = invS * kLumaB + s; m[11] = 0.0f;
            m[12] = 0.0f;             m[13] = 0.0f;             m[14] = 0.0f;              m[15] = 1.0f;
            satMatrix->Name(L"SaturationEffect");
            topOfStack = *satMatrix;
        }

        if (m_tintLuminosityOpacity && *m_tintLuminosityOpacity > 0.0f) {
            float op = std::clamp(*m_tintLuminosityOpacity, 0.0f, 1.0f);
            float tintLum = (m_tint.R / 255.0f) * kLumaR +
                            (m_tint.G / 255.0f) * kLumaG +
                            (m_tint.B / 255.0f) * kLumaB;

            auto lumMatrix = winrt::make_self<ColorMatrixEffect>();
            lumMatrix->Source = topOfStack;
            auto& m = lumMatrix->Matrix;
            m[0] = 1.0f - (kLumaR * op); m[1] = -(kLumaR * op);       m[2] = -(kLumaR * op);       m[3] = 0.0f;
            m[4] = -(kLumaG * op);       m[5] = 1.0f - (kLumaG * op); m[6] = -(kLumaG * op);       m[7] = 0.0f;
            m[8] = -(kLumaB * op);       m[9] = -(kLumaB * op);       m[10] = 1.0f - (kLumaB * op); m[11] = 0.0f;
            m[12] = 0.0f;                m[13] = 0.0f;                m[14] = 0.0f;                 m[15] = 1.0f;
            m[16] = tintLum * op;        m[17] = tintLum * op;        m[18] = tintLum * op;         m[19] = 0.0f;
            lumMatrix->Name(L"LuminosityBlend");
            topOfStack = *lumMatrix;
        }

        wuc::CompositionSurfaceBrush noiseBrush{nullptr};
        if (m_noiseOpacity && *m_noiseOpacity > 0.0f) {
            float density = m_noiseDensity.value_or(1.0f);
            auto stream = CreateNoiseStream(density);
            auto surface =
                Media::LoadedImageSurface::StartLoadFromStream(stream);
            noiseBrush = m_compositor.CreateSurfaceBrush(surface);
            noiseBrush.Stretch(wuc::CompositionStretch::None);

            auto borderEffect = winrt::make_self<BorderEffect>();
            borderEffect->Source =
                wuc::CompositionEffectSourceParameter(L"NoiseSource");

            float nOp = std::clamp(*m_noiseOpacity, 0.0f, 1.0f);
            auto opacityEffect = winrt::make_self<ColorMatrixEffect>();
            opacityEffect->Source = *borderEffect;
            opacityEffect->Matrix[0] = nOp;
            opacityEffect->Matrix[5] = nOp;
            opacityEffect->Matrix[10] = nOp;
            opacityEffect->Matrix[15] = nOp;
            opacityEffect->Name(L"NoiseOpacityEffect");

            auto noiseComposite = winrt::make_self<CompositeEffect>();
            noiseComposite->Mode = D2D1_COMPOSITE_MODE_SOURCE_OVER;
            noiseComposite->Sources.push_back(topOfStack);
            noiseComposite->Sources.push_back(*opacityEffect);
            noiseComposite->Name(L"NoiseComposite");
            topOfStack = *noiseComposite;
        }

        auto floodEffect = winrt::make_self<FloodEffect>();
        floodEffect->Color = m_tint;
        floodEffect->Name(L"FloodEffect");

        auto compositeEffect = winrt::make_self<CompositeEffect>();
        compositeEffect->Mode = D2D1_COMPOSITE_MODE_SOURCE_OVER;
        compositeEffect->Sources.push_back(topOfStack);
        compositeEffect->Sources.push_back(*floodEffect);

        auto factory = m_compositor.CreateEffectFactory(*compositeEffect);
        auto brush = factory.CreateBrush();
        brush.SetSourceParameter(L"backdrop", backdropBrush);
        if (noiseBrush) {
            brush.SetSourceParameter(L"NoiseSource", noiseBrush);
        }
        return brush;
    }

    wuc::Compositor m_compositor;
    float m_blurAmount;
    winrt::Windows::UI::Color m_tint;
    std::optional<float> m_tintLuminosityOpacity;
    std::optional<float> m_tintSaturation;
    std::optional<float> m_noiseOpacity;
    std::optional<float> m_noiseDensity;
    std::optional<winrt::Windows::UI::Color> m_fallbackColor;
};

// ============================================================================
//  Settings & Rules Data Model
// ============================================================================

enum class AccentMode {
    Clear,
    Blur,
    Acrylic,
    Opaque,
    Normal,
};

struct AppearanceConfig {
    bool enabled = true;
    AccentMode accent = AccentMode::Clear;
    winrt::Windows::UI::Color color{0, 0, 0, 0};  // ARGB
    bool showPeek = true;
    bool showLine = false;
    float blurRadius = 9.0f;

    bool operator==(const AppearanceConfig& other) const {
        return enabled == other.enabled &&
               accent == other.accent &&
               color.A == other.color.A &&
               color.R == other.color.R &&
               color.G == other.color.G &&
               color.B == other.color.B &&
               showPeek == other.showPeek &&
               showLine == other.showLine &&
               blurRadius == other.blurRadius;
    }
    bool operator!=(const AppearanceConfig& other) const {
        return !(*this == other);
    }
};

enum class RuleMatchType {
    ProcessName,
    WindowClass,
    WindowTitle,
};

struct WindowRule {
    RuleMatchType matchType = RuleMatchType::ProcessName;
    std::wstring matchValue;  // normalized lowercase
    AppearanceConfig appearance;
};

struct IgnoredWindowsConfig {
    std::vector<std::wstring> windowClasses;
    std::vector<std::wstring> windowTitles;
    std::vector<std::wstring> processNames;
};

struct ModSettings {
    AppearanceConfig desktop;
    AppearanceConfig visibleWindow;
    std::vector<WindowRule> visibleRules;
    AppearanceConfig maximizedWindow;
    std::vector<WindowRule> maximizedRules;
    IgnoredWindowsConfig ignored;
    bool taskViewFollowsDesktop = true;
    int transitionDurationMs = 180;
};

ModSettings g_settings;
SRWLOCK g_settingsLock = SRWLOCK_INIT;

std::wstring ToLowerTrimmed(std::wstring_view input) {
    size_t start = 0;
    while (start < input.size() && iswspace(input[start])) start++;
    size_t end = input.size();
    while (end > start && iswspace(input[end - 1])) end--;
    std::wstring out(input.substr(start, end - start));
    for (wchar_t& ch : out) {
        ch = static_cast<wchar_t>(towlower(ch));
    }
    return out;
}

AccentMode ParseAccentMode(PCWSTR value, AccentMode fallback) {
    if (!value) return fallback;
    if (_wcsicmp(value, L"clear") == 0) return AccentMode::Clear;
    if (_wcsicmp(value, L"blur") == 0) return AccentMode::Blur;
    if (_wcsicmp(value, L"acrylic") == 0) return AccentMode::Acrylic;
    if (_wcsicmp(value, L"opaque") == 0) return AccentMode::Opaque;
    if (_wcsicmp(value, L"normal") == 0) return AccentMode::Normal;
    return fallback;
}

// TranslucentTB stores colors as #RRGGBBAA (also accepts #RRGGBB).
winrt::Windows::UI::Color ParseRgbaHex(
    PCWSTR str, winrt::Windows::UI::Color fallback) {
    if (!str) return fallback;
    std::wstring_view sv(str);
    while (!sv.empty() && iswspace(sv.front())) sv.remove_prefix(1);
    while (!sv.empty() && iswspace(sv.back())) sv.remove_suffix(1);
    if (!sv.empty() && sv.front() == L'#') sv.remove_prefix(1);

    auto hexByte = [](std::wstring_view s, size_t off, uint8_t& out) -> bool {
        if (off + 2 > s.size()) return false;
        unsigned val = 0;
        for (size_t i = 0; i < 2; i++) {
            wchar_t c = s[off + i];
            val <<= 4;
            if (c >= L'0' && c <= L'9') val |= (c - L'0');
            else if (c >= L'a' && c <= L'f') val |= (c - L'a' + 10);
            else if (c >= L'A' && c <= L'F') val |= (c - L'A' + 10);
            else return false;
        }
        out = static_cast<uint8_t>(val);
        return true;
    };

    uint8_t r = 0, g = 0, b = 0, a = 255;
    if (sv.size() == 8) {
        if (hexByte(sv, 0, r) && hexByte(sv, 2, g) &&
            hexByte(sv, 4, b) && hexByte(sv, 6, a)) {
            return {a, r, g, b};
        }
    } else if (sv.size() == 6) {
        if (hexByte(sv, 0, r) && hexByte(sv, 2, g) && hexByte(sv, 4, b)) {
            return {255, r, g, b};
        }
    }
    return fallback;
}

AppearanceConfig LoadAppearanceGroup(PCWSTR prefix,
                                     bool hasEnabledToggle,
                                     AccentMode defAccent,
                                     winrt::Windows::UI::Color defColor) {
    AppearanceConfig cfg{};
    cfg.enabled = hasEnabledToggle
                      ? (Wh_GetIntSetting(L"%s.enabled", prefix) != 0)
                      : true;

    string_setting_unique_ptr accentStr(
        Wh_GetStringSetting(L"%s.accent", prefix));
    cfg.accent = ParseAccentMode(accentStr.get(), defAccent);

    string_setting_unique_ptr colorStr(
        Wh_GetStringSetting(L"%s.color", prefix));
    cfg.color = ParseRgbaHex(colorStr.get(), defColor);

    cfg.showPeek = Wh_GetIntSetting(L"%s.showPeek", prefix) != 0;
    cfg.showLine = Wh_GetIntSetting(L"%s.showLine", prefix) != 0;

    int radius = Wh_GetIntSetting(L"%s.blurRadius", prefix);
    cfg.blurRadius = static_cast<float>(std::clamp(radius > 0 ? radius : 9, 0, 100));
    return cfg;
}

std::vector<WindowRule> LoadRulesList(PCWSTR prefix) {
    std::vector<WindowRule> out;
    for (int i = 0;; i++) {
        string_setting_unique_ptr valStr(
            Wh_GetStringSetting(L"%s.rules[%d].matchValue", prefix, i));
        std::wstring matchVal = ToLowerTrimmed(valStr.get() ? valStr.get() : L"");

        string_setting_unique_ptr typeStr(
            Wh_GetStringSetting(L"%s.rules[%d].matchType", prefix, i));
        string_setting_unique_ptr accentStr(
            Wh_GetStringSetting(L"%s.rules[%d].accent", prefix, i));
        if (!typeStr.get()[0] && !accentStr.get()[0] && !valStr.get()[0]) {
            break;
        }
        if (matchVal.empty()) {
            continue;
        }

        WindowRule rule;
        if (_wcsicmp(typeStr.get(), L"window_class") == 0) {
            rule.matchType = RuleMatchType::WindowClass;
        } else if (_wcsicmp(typeStr.get(), L"window_title") == 0) {
            rule.matchType = RuleMatchType::WindowTitle;
        } else {
            rule.matchType = RuleMatchType::ProcessName;
        }
        rule.matchValue = std::move(matchVal);
        rule.appearance.enabled = true;
        rule.appearance.accent = ParseAccentMode(accentStr.get(), AccentMode::Clear);

        string_setting_unique_ptr colorStr(
            Wh_GetStringSetting(L"%s.rules[%d].color", prefix, i));
        rule.appearance.color = ParseRgbaHex(colorStr.get(), {0, 0, 0, 0});
        rule.appearance.showPeek =
            Wh_GetIntSetting(L"%s.rules[%d].showPeek", prefix, i) != 0;
        rule.appearance.showLine =
            Wh_GetIntSetting(L"%s.rules[%d].showLine", prefix, i) != 0;
        int radius = Wh_GetIntSetting(L"%s.rules[%d].blurRadius", prefix, i);
        rule.appearance.blurRadius =
            static_cast<float>(std::clamp(radius > 0 ? radius : 9, 0, 100));

        out.push_back(std::move(rule));
    }
    return out;
}

std::vector<std::wstring> LoadIgnoredArray(PCWSTR key) {
    std::vector<std::wstring> list;
    for (int i = 0;; i++) {
        string_setting_unique_ptr val(Wh_GetStringSetting(L"%s[%d]", key, i));
        if (!val.get()[0]) {
            // Stop only once we've probed beyond index 0 or empty array terminator:
            // Note Windhawk terminates arrays with an empty string unless items follow;
            // check next index once if index 0 is default placeholder empty string.
            if (i == 0) {
                string_setting_unique_ptr next(
                    Wh_GetStringSetting(L"%s[%d]", key, 1));
                if (!next.get()[0]) break;
                continue;
            }
            break;
        }
        std::wstring clean = ToLowerTrimmed(val.get());
        if (!clean.empty()) {
            list.push_back(std::move(clean));
        }
    }
    return list;
}

void LoadSettings() {
    ModSettings s;
    s.desktop = LoadAppearanceGroup(
        L"desktopAppearance", false, AccentMode::Clear, {0, 0, 0, 0});
    s.visibleWindow = LoadAppearanceGroup(
        L"visibleWindowAppearance", true, AccentMode::Clear, {0, 0, 0, 0});
    s.visibleRules = LoadRulesList(L"visibleWindowAppearance");
    s.maximizedWindow = LoadAppearanceGroup(
        L"maximizedWindowAppearance", true, AccentMode::Normal, {255, 0x37, 0x35, 0x35});
    s.maximizedRules = LoadRulesList(L"maximizedWindowAppearance");
    s.ignored.windowClasses = LoadIgnoredArray(L"ignoredWindows.windowClass");
    s.ignored.windowTitles = LoadIgnoredArray(L"ignoredWindows.windowTitle");
    s.ignored.processNames = LoadIgnoredArray(L"ignoredWindows.processName");
    s.taskViewFollowsDesktop =
        Wh_GetIntSetting(L"taskViewFollowsDesktop") != 0;
    s.transitionDurationMs =
        std::clamp(Wh_GetIntSetting(L"transitionDurationMs"), 0, 2000);

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = std::move(s);
    ReleaseSRWLockExclusive(&g_settingsLock);
}

ModSettings SnapshotSettings() {
    AcquireSRWLockShared(&g_settingsLock);
    ModSettings copy = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return copy;
}

// ============================================================================
//  Explorer taskbar.dll Symbols & XAML Tree Access
// ============================================================================

std::atomic<bool> g_modUnloading{false};

void* CTaskBand_ITaskListWndSite_vftable = nullptr;
void* CSecondaryTaskBand_ITaskListWndSite_vftable = nullptr;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original = nullptr;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original = nullptr;

void* TaskbarHost_FrameHeight_Original = nullptr;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original = nullptr;

using TrayUI_StartTaskbar_t = void(WINAPI*)(void* pThis);
TrayUI_StartTaskbar_t TrayUI_StartTaskbar_Original = nullptr;

using CSecondaryTray_GetTrayWindow_t = HWND(WINAPI*)(void* pThis);
CSecondaryTray_GetTrayWindow_t CSecondaryTray_GetTrayWindow_Original = nullptr;

using CSecondaryTray_InitModelAndHost_t = void(WINAPI*)(void* pThis, void* taskbarModel);
CSecondaryTray_InitModelAndHost_t CSecondaryTray_InitModelAndHost_Original = nullptr;

bool IsTaskbarWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return false;
    wchar_t className[64]{};
    return GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
           (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
            _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0);
}

bool IsSecondaryTaskbarWindow(HWND hWnd) {
    if (!hWnd) return false;
    wchar_t className[64]{};
    return GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
           _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0;
}

HWND FindPrimaryTaskbarWnd() {
    HWND result = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD pid = 0;
            wchar_t cls[32]{};
            if (GetWindowThreadProcessId(hWnd, &pid) &&
                pid == GetCurrentProcessId() &&
                GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
                _wcsicmp(cls, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&result));
    return result;
}

std::vector<HWND> FindAllTaskbarWindows() {
    std::vector<HWND> list;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD pid = 0;
            if (GetWindowThreadProcessId(hWnd, &pid) &&
                pid == GetCurrentProcessId() &&
                IsTaskbarWindow(hWnd)) {
                reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(hWnd);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&list));
    return list;
}

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t offset = 0x10;
#if defined(_M_X64)
    const BYTE* b = static_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
    if (b && b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
        b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
        offset = b[7];
    }
#elif defined(_M_ARM64)
    const DWORD* p = static_cast<const DWORD*>(TaskbarHost_FrameHeight_Original);
    if (p && p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
        p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
        offset = (p[3] >> 12) & 0xFF;
    }
#endif

    XamlRoot result{nullptr};
    if (taskbarHostSharedPtr[0]) {
        auto* elemIUnknown = *reinterpret_cast<IUnknown**>(
            static_cast<BYTE*>(taskbarHostSharedPtr[0]) + offset);
        if (elemIUnknown) {
            FrameworkElement elem{nullptr};
            elemIUnknown->QueryInterface(
                winrt::guid_of<FrameworkElement>(), winrt::put_abi(elem));
            if (elem) {
                result = elem.XamlRoot();
            }
        }
    }

    if (taskbarHostSharedPtr[1]) {
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    }
    return result;
}

XamlRoot GetPrimaryTaskbarXamlRoot(HWND taskbarWnd) {
    HWND taskSwWnd =
        reinterpret_cast<HWND>(GetPropW(taskbarWnd, L"TaskbandHWND"));
    if (!taskSwWnd) return nullptr;

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(taskSwWnd, 0));
    if (!taskBand) return nullptr;

    void* slot = taskBand;
    for (int i = 0; *reinterpret_cast<void**>(slot) !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) return nullptr;
        slot = reinterpret_cast<void**>(slot) + 1;
    }

    void* sharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(slot, sharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(sharedPtr);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND secondaryTaskbarWnd) {
    HWND workerWnd =
        FindWindowExW(secondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!workerWnd) return nullptr;

    void* taskBand = reinterpret_cast<void*>(GetWindowLongPtrW(workerWnd, 0));
    if (!taskBand) return nullptr;

    void* slot = taskBand;
    for (int i = 0; *reinterpret_cast<void**>(slot) !=
                    CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) return nullptr;
        slot = reinterpret_cast<void**>(slot) + 1;
    }

    void* sharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(slot, sharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(sharedPtr);
}

XamlRoot GetTaskbarXamlRoot(HWND taskbarWnd) {
    try {
        return IsSecondaryTaskbarWindow(taskbarWnd)
                   ? GetSecondaryTaskbarXamlRoot(taskbarWnd)
                   : GetPrimaryTaskbarXamlRoot(taskbarWnd);
    } catch (...) {
        return nullptr;
    }
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

UINT g_runFromWindowThreadMsg = 0;

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    if (!g_runFromWindowThreadMsg) {
        g_runFromWindowThreadMsg = RegisterWindowMessageW(
            L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    }

    struct Param {
        RunFromWindowThreadProc_t proc;
        void* procParam;
        bool started = false;
        bool completed = false;
    };

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId) return false;
    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == g_runFromWindowThreadMsg) {
                    auto* p = reinterpret_cast<Param*>(cwp->lParam);
                    if (!p->started) {
                        p->started = true;
                        p->proc(p->procParam);
                        p->completed = true;
                    }
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) return false;

    Param p{proc, procParam};
    SendMessageW(hWnd, g_runFromWindowThreadMsg, 0, reinterpret_cast<LPARAM>(&p));
    UnhookWindowsHookEx(hook);
    return p.completed;
}

// ============================================================================
//  Window State & Rules Evaluation (per monitor)
// ============================================================================

struct MonInfoData {
    HMONITOR hMon;
    RECT rcMonitor;
};

struct WindowMetadata {
    HWND hwnd = nullptr;
    RECT frameRect{};
    bool maximized = false;
    std::wstring classNameLower;
    std::wstring titleLower;
    std::wstring processNameLower;
};

bool IsBuiltInIgnoredClass(std::wstring_view cls) {
    return cls == L"progman" ||
           cls == L"workerw" ||
           cls == L"shell_traywnd" ||
           cls == L"shell_secondarytraywnd" ||
           cls == L"windows.ui.core.corewindow" ||
           cls == L"foregroundstaging" ||
           cls == L"edgeuiinputwndclass" ||
           cls == L"edgeuiinputtopwndclass" ||
           cls == L"shell_inputswitchtoplevelwindow" ||
           cls == L"toplevelwindowforoverflowxamlisland" ||
           cls == L"notifyiconoverflowwindow" ||
           cls == L"#32768" ||
           cls == L"tooltips_class32" ||
           cls == L"multitaskingviewframe" ||
           cls == L"xamlexplorerhostwindow" ||
           cls == L"applicationmanager_desktopshellwindow";
}

bool ProcessMatchesExactOrStem(const std::wstring& procLower,
                               const std::wstring& patternLower) {
    if (procLower == patternLower) return true;
    // Allow matching "notepad" against "notepad.exe" or vice versa
    if (patternLower.size() > 4 &&
        patternLower.compare(patternLower.size() - 4, 4, L".exe") == 0) {
        return procLower == patternLower.substr(0, patternLower.size() - 4);
    }
    if (procLower.size() > 4 &&
        procLower.compare(procLower.size() - 4, 4, L".exe") == 0) {
        return procLower.substr(0, procLower.size() - 4) == patternLower;
    }
    return false;
}

std::wstring GetWindowProcessNameLower(HWND hwnd) {
    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hwnd, &pid) || !pid) return {};
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return {};
    wchar_t path[MAX_PATH]{};
    DWORD len = ARRAYSIZE(path);
    BOOL ok = QueryFullProcessImageNameW(hProc, 0, path, &len);
    CloseHandle(hProc);
    if (!ok || len == 0) return {};
    PCWSTR fileName = PathFindFileNameW(path);
    return ToLowerTrimmed(fileName);
}

bool IsTaskViewOpen() {
    struct Ctx {
        bool open = false;
    } ctx;

    EnumWindows(
        [](HWND hwnd, LPARAM lp) -> BOOL {
            if (!IsWindowVisible(hwnd)) return TRUE;
            BOOL cloaked = FALSE;
            if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                                sizeof(cloaked))) &&
                cloaked) {
                return TRUE;
            }
            wchar_t cls[128]{};
            if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) return TRUE;
            if (_wcsicmp(cls, L"MultitaskingViewFrame") == 0) {
                reinterpret_cast<Ctx*>(lp)->open = true;
                return FALSE;
            }
            if (_wcsicmp(cls, L"XamlExplorerHostWindow") == 0 ||
                _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
                wchar_t title[128]{};
                GetWindowTextW(hwnd, title, ARRAYSIZE(title));
                if (_wcsicmp(title, L"Task View") == 0) {
                    reinterpret_cast<Ctx*>(lp)->open = true;
                    return FALSE;
                }
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&ctx));
    return ctx.open;
}

bool MatchesIgnoreList(const WindowMetadata& wm,
                       const IgnoredWindowsConfig& ignored) {
    for (const auto& c : ignored.windowClasses) {
        if (wm.classNameLower == c) return true;
    }
    for (const auto& p : ignored.processNames) {
        if (ProcessMatchesExactOrStem(wm.processNameLower, p)) return true;
    }
    for (const auto& t : ignored.windowTitles) {
        if (!t.empty() && wm.titleLower.find(t) != std::wstring::npos) {
            return true;
        }
    }
    return false;
}

bool MatchesRule(const WindowMetadata& wm, const WindowRule& rule) {
    switch (rule.matchType) {
        case RuleMatchType::ProcessName:
            return ProcessMatchesExactOrStem(wm.processNameLower,
                                             rule.matchValue);
        case RuleMatchType::WindowClass:
            return wm.classNameLower == rule.matchValue;
        case RuleMatchType::WindowTitle:
            return !rule.matchValue.empty() &&
                   wm.titleLower.find(rule.matchValue) != std::wstring::npos;
    }
    return false;
}

struct EnumWindowsCtx {
    const ModSettings* settings = nullptr;
    HWND foregroundWnd = nullptr;
    std::vector<WindowMetadata> windows;
};

std::vector<WindowMetadata> CollectActiveWindows(const ModSettings& settings) {
    EnumWindowsCtx ctx;
    ctx.settings = &settings;
    ctx.foregroundWnd = GetForegroundWindow();

    EnumWindows(
        [](HWND hwnd, LPARAM lp) -> BOOL {
            auto* ctx = reinterpret_cast<EnumWindowsCtx*>(lp);
            if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) return TRUE;

            LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
            if (style & WS_CHILD) return TRUE;

            LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
            if ((exStyle & WS_EX_TOOLWINDOW) && !(exStyle & WS_EX_APPWINDOW)) {
                return TRUE;
            }
            if ((exStyle & WS_EX_NOACTIVATE) &&
                (exStyle & WS_EX_TRANSPARENT)) {
                return TRUE;
            }

            BOOL cloaked = FALSE;
            if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                                sizeof(cloaked))) &&
                cloaked) {
                return TRUE;
            }

            wchar_t clsBuf[256]{};
            if (!GetClassNameW(hwnd, clsBuf, ARRAYSIZE(clsBuf))) return TRUE;
            std::wstring clsLower = ToLowerTrimmed(clsBuf);
            if (IsBuiltInIgnoredClass(clsLower)) return TRUE;

            RECT rc{};
            if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS,
                                             &rc, sizeof(rc)))) {
                if (!GetWindowRect(hwnd, &rc)) return TRUE;
            }
            if (rc.right <= rc.left || rc.bottom <= rc.top) return TRUE;

            WindowMetadata wm;
            wm.hwnd = hwnd;
            wm.frameRect = rc;
            wm.maximized = IsZoomed(hwnd) != FALSE;
            wm.classNameLower = std::move(clsLower);

            wchar_t titleBuf[512]{};
            GetWindowTextW(hwnd, titleBuf, ARRAYSIZE(titleBuf));
            wm.titleLower = ToLowerTrimmed(titleBuf);
            wm.processNameLower = GetWindowProcessNameLower(hwnd);

            if (MatchesIgnoreList(wm, ctx->settings->ignored)) {
                return TRUE;
            }

            ctx->windows.push_back(std::move(wm));
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&ctx));

    // Move foreground window to the front so its rules take priority.
    if (ctx.foregroundWnd) {
        auto it = std::find_if(
            ctx.windows.begin(), ctx.windows.end(),
            [&](const WindowMetadata& w) { return w.hwnd == ctx.foregroundWnd; });
        if (it != ctx.windows.end() && it != ctx.windows.begin()) {
            std::rotate(ctx.windows.begin(), it, it + 1);
        }
    }
    return ctx.windows;
}

bool WindowOnMonitor(const WindowMetadata& wm,
                     HMONITOR targetMon,
                     const RECT& monRect) {
    if (wm.maximized) {
        return MonitorFromWindow(wm.hwnd, MONITOR_DEFAULTTONEAREST) == targetMon;
    }
    RECT inter{};
    if (!IntersectRect(&inter, &wm.frameRect, &monRect)) return false;
    return (inter.right - inter.left) >= 4 && (inter.bottom - inter.top) >= 4;
}

AppearanceConfig EvaluateAppearanceForMonitor(
    HMONITOR targetMon,
    bool taskViewOpen,
    const std::vector<WindowMetadata>& windows,
    const ModSettings& settings) {
    if (taskViewOpen && settings.taskViewFollowsDesktop) {
        return settings.desktop;
    }

    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!targetMon || !GetMonitorInfoW(targetMon, &mi)) {
        return settings.desktop;
    }

    bool hasMaximized = false;
    bool hasVisible = false;
    const WindowRule* matchedMaxRule = nullptr;
    const WindowRule* matchedVisRule = nullptr;

    for (const auto& wm : windows) {
        if (!WindowOnMonitor(wm, targetMon, mi.rcMonitor)) continue;

        if (wm.maximized) {
            hasMaximized = true;
            if (!matchedMaxRule) {
                for (const auto& r : settings.maximizedRules) {
                    if (MatchesRule(wm, r)) {
                        matchedMaxRule = &r;
                        break;
                    }
                }
            }
        } else {
            hasVisible = true;
            if (!matchedVisRule) {
                for (const auto& r : settings.visibleRules) {
                    if (MatchesRule(wm, r)) {
                        matchedVisRule = &r;
                        break;
                    }
                }
            }
        }
    }

    if (matchedMaxRule) {
        return matchedMaxRule->appearance;
    }
    if (hasMaximized && settings.maximizedWindow.enabled) {
        return settings.maximizedWindow;
    }
    if (matchedVisRule) {
        return matchedVisRule->appearance;
    }
    if (hasVisible && settings.visibleWindow.enabled) {
        return settings.visibleWindow;
    }
    return settings.desktop;
}

// ============================================================================
//  XAML Element Styling & State Tracking per Taskbar (UI-thread affine)
// ============================================================================

template <typename Fn>
void WalkDescendants(FrameworkElement const& root, Fn&& callback) {
    if (!root) return;
    try {
        int count = Media::VisualTreeHelper::GetChildrenCount(root);
        for (int i = 0; i < count; i++) {
            auto child = Media::VisualTreeHelper::GetChild(root, i)
                             .try_as<FrameworkElement>();
            if (!child) continue;
            if (callback(child)) {
                WalkDescendants(child, callback);
            }
        }
    } catch (...) {
    }
}

struct TrackedFillElement {
    winrt::weak_ref<Shapes::Shape> shape;
    wf::IInspectable originalLocalFill{nullptr};
    wf::IInspectable originalLocalOpacity{nullptr};
    bool originalCaptured = false;
    int64_t propToken = 0;
};

struct TrackedVisibilityElement {
    winrt::weak_ref<UIElement> elem;
    wf::IInspectable originalLocalVisibility{nullptr};
    wf::IInspectable originalLocalOpacity{nullptr};
    bool originalCaptured = false;
    int64_t propToken = 0;
};

struct TaskbarVisualState {
    HWND taskbarWnd = nullptr;
    winrt::weak_ref<FrameworkElement> rootFrame;
    std::vector<TrackedFillElement> fills;
    std::vector<TrackedVisibilityElement> strokes;
    std::vector<TrackedVisibilityElement> peeks;

    // Optional sibling rectangle inside BackgroundFill's Grid for non-transparent cross-fades.
    Shapes::Rectangle fadeOverlay{nullptr};
    Controls::Panel fadeOverlayParent{nullptr};

    bool hasLastApplied = false;
    AppearanceConfig lastConfig{};
    bool applying = false;
    Media::Brush currentBrush{nullptr};
    bool activeFillIsNative = true;

    // Animation state (steady visual values live in cur*Opacity / curSolidColor)
    bool animating = false;
    LONGLONG animStartQpc = 0;
    int animDurationMs = 0;

    bool fadeClearAtEnd = false;
    bool useOverlayFade = false;
    bool colorLerp = false;
    winrt::Windows::UI::Color startColor{}, endColor{}, curColor{};
    Media::SolidColorBrush lerpBrush{nullptr};

    double curFillOp = 1.0, startFillOp = 1.0, endFillOp = 1.0;
    double curLineOp = 1.0, startLineOp = 1.0, endLineOp = 1.0;
    double curPeekOp = 1.0, startPeekOp = 1.0, endPeekOp = 1.0;
};

// Accessed strictly from Explorer's taskbar thread.
std::unordered_map<HWND, TaskbarVisualState> g_taskbarStates;
constexpr UINT_PTR kReevaluateTimerId = 0x44545401ULL;
constexpr UINT_PTR kRetryInitTimerId = 0x44545402ULL;
constexpr UINT_PTR kAnimTimerId = 0x44545403ULL;
int g_retryInitRemaining = 0;
HWINEVENTHOOK g_winEventHookSystem = nullptr;
HWINEVENTHOOK g_winEventHookObject = nullptr;
bool g_evalPending = false;
bool g_animTimerActive = false;
bool g_hooksInstalled = false;

double QueryPerfMsSince(LONGLONG startQpc) {
    static const LONGLONG freq = []() {
        LARGE_INTEGER f{};
        QueryPerformanceFrequency(&f);
        return f.QuadPart > 0 ? f.QuadPart : 1000LL;
    }();
    LARGE_INTEGER now{};
    QueryPerformanceCounter(&now);
    return static_cast<double>(now.QuadPart - startQpc) * 1000.0 /
           static_cast<double>(freq);
}

LONGLONG CurrentQpc() {
    LARGE_INTEGER now{};
    QueryPerformanceCounter(&now);
    return now.QuadPart;
}

double EaseOutCubic(double t) {
    double inv = 1.0 - std::clamp(t, 0.0, 1.0);
    return 1.0 - inv * inv * inv;
}

uint8_t LerpByte(uint8_t a, uint8_t b, double t) {
    return static_cast<uint8_t>(std::lround(a + (b - a) * t));
}

bool IsClearZero(const AppearanceConfig& cfg) {
    return cfg.accent == AccentMode::Clear && cfg.color.A == 0;
}

bool IsSolidAccent(const AppearanceConfig& cfg) {
    return cfg.accent == AccentMode::Clear || cfg.accent == AccentMode::Opaque;
}

winrt::Windows::UI::Color EffectiveSolidColor(const AppearanceConfig& cfg) {
    winrt::Windows::UI::Color c = cfg.color;
    if (cfg.accent == AccentMode::Opaque) c.A = 255;
    return c;
}

void ApplyAppearanceToState(TaskbarVisualState& state,
                            const AppearanceConfig& cfg,
                            bool forceBrushRebuild,
                            int transitionMs = 0);

Media::Brush CreateBrushForConfig(UIElement const& anchor,
                                  const AppearanceConfig& cfg) {
    switch (cfg.accent) {
        case AccentMode::Normal:
            return nullptr;  // signal to restore original fill
        case AccentMode::Clear: {
            Media::SolidColorBrush brush;
            brush.Color(cfg.color);
            return brush;
        }
        case AccentMode::Opaque: {
            winrt::Windows::UI::Color solid = cfg.color;
            solid.A = 255;
            Media::SolidColorBrush brush;
            brush.Color(solid);
            return brush;
        }
        case AccentMode::Blur:
            return winrt::make<XamlBlurBrush>(
                anchor, cfg.blurRadius, cfg.color,
                std::nullopt, std::nullopt, std::nullopt, std::nullopt,
                std::optional<winrt::Windows::UI::Color>{cfg.color});
        case AccentMode::Acrylic: {
            float blur = std::max(cfg.blurRadius, 24.0f);
            return winrt::make<XamlBlurBrush>(
                anchor, blur, cfg.color,
                std::optional<float>{0.85f},
                std::optional<float>{1.25f},
                std::optional<float>{0.02f},
                std::optional<float>{0.44f},
                std::optional<winrt::Windows::UI::Color>{cfg.color});
        }
    }
    return nullptr;
}

void RemoveFadeOverlay(TaskbarVisualState& state) {
    if (state.fadeOverlay) {
        try {
            state.fadeOverlay.Fill(nullptr);
            state.fadeOverlay.Visibility(Visibility::Collapsed);
            if (state.fadeOverlayParent) {
                auto children = state.fadeOverlayParent.Children();
                uint32_t idx = 0;
                if (children.IndexOf(state.fadeOverlay, idx)) {
                    children.RemoveAt(idx);
                }
            }
        } catch (...) {
        }
    }
    state.fadeOverlay = nullptr;
    state.fadeOverlayParent = nullptr;
}

bool EnsureFadeOverlay(TaskbarVisualState& state) {
    if (state.fills.empty()) return false;
    auto primary = state.fills[0].shape.get();
    if (!primary) return false;

    if (state.fadeOverlay && state.fadeOverlayParent) {
        try {
            if (auto r = primary.try_as<Shapes::Rectangle>()) {
                if (auto ov = state.fadeOverlay) {
                    ov.RadiusX(r.RadiusX());
                    ov.RadiusY(r.RadiusY());
                    ov.Margin(r.Margin());
                }
            }
            return true;
        } catch (...) {
        }
    }

    RemoveFadeOverlay(state);

    try {
        auto parent =
            Media::VisualTreeHelper::GetParent(primary).try_as<Controls::Panel>();
        if (!parent) return false;

        Shapes::Rectangle ov;
        ov.IsHitTestVisible(false);
        ov.HorizontalAlignment(HorizontalAlignment::Stretch);
        ov.VerticalAlignment(VerticalAlignment::Stretch);
        ov.Visibility(Visibility::Collapsed);
        if (auto r = primary.try_as<Shapes::Rectangle>()) {
            ov.RadiusX(r.RadiusX());
            ov.RadiusY(r.RadiusY());
            ov.Margin(r.Margin());
            Controls::Grid::SetRow(ov, Controls::Grid::GetRow(r));
            Controls::Grid::SetColumn(ov, Controls::Grid::GetColumn(r));
            Controls::Grid::SetRowSpan(ov, Controls::Grid::GetRowSpan(r));
            Controls::Grid::SetColumnSpan(ov, Controls::Grid::GetColumnSpan(r));
        }

        auto children = parent.Children();
        uint32_t fillIdx = 0;
        if (children.IndexOf(primary, fillIdx)) {
            children.InsertAt(fillIdx + 1, ov);
        } else {
            children.Append(ov);
        }
        state.fadeOverlay = std::move(ov);
        state.fadeOverlayParent = std::move(parent);
        return true;
    } catch (...) {
        return false;
    }
}

void DetachCallbacksFromState(TaskbarVisualState& state) {
    for (auto& f : state.fills) {
        if (auto sh = f.shape.get(); sh && f.propToken) {
            try {
                sh.UnregisterPropertyChangedCallback(
                    Shapes::Shape::FillProperty(), f.propToken);
            } catch (...) {
            }
        }
        f.propToken = 0;
    }
    for (auto& s : state.strokes) {
        if (auto el = s.elem.get(); el && s.propToken) {
            try {
                el.UnregisterPropertyChangedCallback(
                    UIElement::VisibilityProperty(), s.propToken);
            } catch (...) {
            }
        }
        s.propToken = 0;
    }
    for (auto& p : state.peeks) {
        if (auto el = p.elem.get(); el && p.propToken) {
            try {
                el.UnregisterPropertyChangedCallback(
                    UIElement::VisibilityProperty(), p.propToken);
            } catch (...) {
            }
        }
        p.propToken = 0;
    }
}

void RestoreStateToNative(TaskbarVisualState& state) {
    state.applying = true;
    state.animating = false;
    RemoveFadeOverlay(state);
    auto unsetSentinel = DependencyProperty::UnsetValue();

    for (auto& f : state.fills) {
        if (auto sh = f.shape.get(); sh && f.originalCaptured) {
            try {
                if (f.originalLocalFill == unsetSentinel) {
                    sh.ClearValue(Shapes::Shape::FillProperty());
                } else {
                    sh.SetValue(Shapes::Shape::FillProperty(),
                                f.originalLocalFill);
                }
                if (f.originalLocalOpacity == unsetSentinel) {
                    sh.ClearValue(UIElement::OpacityProperty());
                } else {
                    sh.SetValue(UIElement::OpacityProperty(),
                                f.originalLocalOpacity);
                }
            } catch (...) {
            }
        }
    }
    for (auto& s : state.strokes) {
        if (auto el = s.elem.get(); el && s.originalCaptured) {
            try {
                if (s.originalLocalVisibility == unsetSentinel) {
                    el.ClearValue(UIElement::VisibilityProperty());
                } else {
                    el.SetValue(UIElement::VisibilityProperty(),
                                s.originalLocalVisibility);
                }
                if (s.originalLocalOpacity == unsetSentinel) {
                    el.ClearValue(UIElement::OpacityProperty());
                } else {
                    el.SetValue(UIElement::OpacityProperty(),
                                s.originalLocalOpacity);
                }
            } catch (...) {
            }
        }
    }
    for (auto& p : state.peeks) {
        if (auto el = p.elem.get(); el && p.originalCaptured) {
            try {
                if (p.originalLocalVisibility == unsetSentinel) {
                    el.ClearValue(UIElement::VisibilityProperty());
                } else {
                    el.SetValue(UIElement::VisibilityProperty(),
                                p.originalLocalVisibility);
                }
                if (p.originalLocalOpacity == unsetSentinel) {
                    el.ClearValue(UIElement::OpacityProperty());
                } else {
                    el.SetValue(UIElement::OpacityProperty(),
                                p.originalLocalOpacity);
                }
            } catch (...) {
            }
        }
    }

    state.applying = false;
    DetachCallbacksFromState(state);
    state.currentBrush = nullptr;
    state.lerpBrush = nullptr;
    state.hasLastApplied = false;
}

void RegisterPropertyWatchers(HWND taskbarWnd, TaskbarVisualState& state) {
    for (auto& f : state.fills) {
        auto sh = f.shape.get();
        if (!sh || f.propToken) continue;
        f.propToken = sh.RegisterPropertyChangedCallback(
            Shapes::Shape::FillProperty(),
            [taskbarWnd](DependencyObject const&, DependencyProperty const&) {
                if (g_modUnloading.load(std::memory_order_acquire)) return;
                auto it = g_taskbarStates.find(taskbarWnd);
                if (it == g_taskbarStates.end() || it->second.applying) return;
                auto& st = it->second;
                if (st.hasLastApplied && !st.activeFillIsNative &&
                    !st.animating && st.currentBrush) {
                    st.applying = true;
                    for (auto& item : st.fills) {
                        if (auto s = item.shape.get()) {
                            try {
                                if (s.Fill() != st.currentBrush) {
                                    s.Fill(st.currentBrush);
                                }
                            } catch (...) {
                            }
                        }
                    }
                    st.applying = false;
                }
            });
    }

    for (auto& s : state.strokes) {
        auto el = s.elem.get();
        if (!el || s.propToken) continue;
        s.propToken = el.RegisterPropertyChangedCallback(
            UIElement::VisibilityProperty(),
            [taskbarWnd](DependencyObject const&, DependencyProperty const&) {
                if (g_modUnloading.load(std::memory_order_acquire)) return;
                auto it = g_taskbarStates.find(taskbarWnd);
                if (it == g_taskbarStates.end() || it->second.applying) return;
                auto& st = it->second;
                if (st.hasLastApplied && !st.animating &&
                    !st.lastConfig.showLine) {
                    st.applying = true;
                    for (auto& item : st.strokes) {
                        if (auto e = item.elem.get()) {
                            try {
                                if (e.Visibility() != Visibility::Collapsed) {
                                    e.Visibility(Visibility::Collapsed);
                                }
                            } catch (...) {
                            }
                        }
                    }
                    st.applying = false;
                }
            });
    }

    for (auto& p : state.peeks) {
        auto el = p.elem.get();
        if (!el || p.propToken) continue;
        p.propToken = el.RegisterPropertyChangedCallback(
            UIElement::VisibilityProperty(),
            [taskbarWnd](DependencyObject const&, DependencyProperty const&) {
                if (g_modUnloading.load(std::memory_order_acquire)) return;
                auto it = g_taskbarStates.find(taskbarWnd);
                if (it == g_taskbarStates.end() || it->second.applying) return;
                auto& st = it->second;
                if (st.hasLastApplied && !st.animating &&
                    !st.lastConfig.showPeek) {
                    st.applying = true;
                    for (auto& item : st.peeks) {
                        if (auto e = item.elem.get()) {
                            try {
                                if (e.Visibility() != Visibility::Collapsed) {
                                    e.Visibility(Visibility::Collapsed);
                                }
                            } catch (...) {
                            }
                        }
                    }
                    st.applying = false;
                }
            });
    }
}

bool EnsureTaskbarTargetsCollected(HWND taskbarWnd, TaskbarVisualState& state) {
    bool alive = !state.fills.empty();
    if (alive) {
        for (const auto& f : state.fills) {
            if (!f.shape.get()) {
                alive = false;
                break;
            }
        }
    }
    if (alive) return true;

    DetachCallbacksFromState(state);
    RemoveFadeOverlay(state);
    state.fills.clear();
    state.strokes.clear();
    state.peeks.clear();
    state.currentBrush = nullptr;
    state.lerpBrush = nullptr;
    state.hasLastApplied = false;
    state.animating = false;
    state.taskbarWnd = taskbarWnd;

    XamlRoot root = GetTaskbarXamlRoot(taskbarWnd);
    if (!root) return false;
    FrameworkElement content = root.Content().try_as<FrameworkElement>();
    if (!content) return false;

    state.rootFrame = winrt::make_weak(content);

    WalkDescendants(content, [&](FrameworkElement const& child) -> bool {
        try {
            auto name = child.Name();
            if (name == L"HoverFlyoutBackgroundControl") {
                return false;  // do not override hover flyout backdrop
            }
            if (name == L"BackgroundFill") {
                if (auto shape = child.try_as<Shapes::Shape>()) {
                    TrackedFillElement item;
                    item.shape = winrt::make_weak(shape);
                    item.originalLocalFill =
                        shape.ReadLocalValue(Shapes::Shape::FillProperty());
                    item.originalLocalOpacity =
                        shape.ReadLocalValue(UIElement::OpacityProperty());
                    item.originalCaptured = true;
                    state.fills.push_back(std::move(item));
                }
            } else if (name == L"BackgroundStroke") {
                TrackedVisibilityElement item;
                item.elem = winrt::make_weak(child.as<UIElement>());
                item.originalLocalVisibility =
                    child.ReadLocalValue(UIElement::VisibilityProperty());
                item.originalLocalOpacity =
                    child.ReadLocalValue(UIElement::OpacityProperty());
                item.originalCaptured = true;
                state.strokes.push_back(std::move(item));
            } else if (name == L"ShowDesktopStack") {
                TrackedVisibilityElement item;
                item.elem = winrt::make_weak(child.as<UIElement>());
                item.originalLocalVisibility =
                    child.ReadLocalValue(UIElement::VisibilityProperty());
                item.originalLocalOpacity =
                    child.ReadLocalValue(UIElement::OpacityProperty());
                item.originalCaptured = true;
                state.peeks.push_back(std::move(item));
            }
        } catch (...) {
        }
        return true;
    });

    if (state.fills.empty()) return false;
    RegisterPropertyWatchers(taskbarWnd, state);
    return true;
}

void ApplyFillElements(TaskbarVisualState& state,
                       bool useNative,
                       Media::Brush const& brush,
                       double opacity) {
    auto unsetSentinel = DependencyProperty::UnsetValue();
    state.activeFillIsNative = useNative;
    for (auto& f : state.fills) {
        auto sh = f.shape.get();
        if (!sh) continue;
        try {
            if (useNative) {
                if (f.originalCaptured) {
                    if (f.originalLocalFill == unsetSentinel) {
                        sh.ClearValue(Shapes::Shape::FillProperty());
                    } else {
                        sh.SetValue(Shapes::Shape::FillProperty(),
                                    f.originalLocalFill);
                    }
                }
            } else if (brush) {
                if (sh.Fill() != brush) {
                    sh.Fill(brush);
                }
            }
            sh.Opacity(opacity);
        } catch (...) {
        }
    }
}

void ApplyAnimatedVisibilityGroup(std::vector<TrackedVisibilityElement>& group,
                                  double opacity,
                                  bool targetVisible,
                                  bool finalFrame) {
    auto unsetSentinel = DependencyProperty::UnsetValue();
    for (auto& item : group) {
        auto el = item.elem.get();
        if (!el) continue;
        try {
            if (finalFrame) {
                if (!targetVisible) {
                    if (el.Visibility() != Visibility::Collapsed) {
                        el.Visibility(Visibility::Collapsed);
                    }
                    if (item.originalCaptured) {
                        if (item.originalLocalOpacity == unsetSentinel) {
                            el.ClearValue(UIElement::OpacityProperty());
                        } else {
                            el.SetValue(UIElement::OpacityProperty(),
                                        item.originalLocalOpacity);
                        }
                    }
                } else {
                    if (item.originalCaptured) {
                        if (item.originalLocalVisibility == unsetSentinel) {
                            el.ClearValue(UIElement::VisibilityProperty());
                        } else {
                            el.SetValue(UIElement::VisibilityProperty(),
                                        item.originalLocalVisibility);
                        }
                        if (item.originalLocalOpacity == unsetSentinel) {
                            el.ClearValue(UIElement::OpacityProperty());
                        } else {
                            el.SetValue(UIElement::OpacityProperty(),
                                        item.originalLocalOpacity);
                        }
                    }
                }
            } else {
                // Keep visible during transition unless natively hidden when targetVisible.
                if (el.Visibility() == Visibility::Collapsed &&
                    (targetVisible || opacity > 0.001)) {
                    el.Visibility(Visibility::Visible);
                }
                el.Opacity(std::clamp(opacity, 0.0, 1.0));
            }
        } catch (...) {
        }
    }
}

void StepTaskbarAnimation(TaskbarVisualState& state) {
    if (!state.animating || state.applying) return;

    double rawT = state.animDurationMs > 0
                      ? (QueryPerfMsSince(state.animStartQpc) /
                         static_cast<double>(state.animDurationMs))
                      : 1.0;
    bool done = rawT >= 1.0;
    double t = done ? 1.0 : EaseOutCubic(rawT);

    state.applying = true;

    state.curFillOp =
        state.startFillOp + (state.endFillOp - state.startFillOp) * t;
    state.curLineOp =
        state.startLineOp + (state.endLineOp - state.startLineOp) * t;
    state.curPeekOp =
        state.startPeekOp + (state.endPeekOp - state.startPeekOp) * t;

    if (state.colorLerp && state.lerpBrush) {
        state.curColor = {
            LerpByte(state.startColor.A, state.endColor.A, t),
            LerpByte(state.startColor.R, state.endColor.R, t),
            LerpByte(state.startColor.G, state.endColor.G, t),
            LerpByte(state.startColor.B, state.endColor.B, t),
        };
        try {
            state.lerpBrush.Color(state.curColor);
        } catch (...) {
        }
    }

    if (state.useOverlayFade && state.fadeOverlay) {
        try {
            if (done) {
                state.fadeOverlay.Visibility(Visibility::Collapsed);
                state.fadeOverlay.Fill(nullptr);
            } else {
                // Incoming fill ramps up smoothly while outgoing overlay fades out.
                double inv = 1.0 - t;
                double inOp = 1.0 - inv * inv;
                double outOp = inv;
                state.fadeOverlay.Opacity(outOp);
                for (auto& f : state.fills) {
                    if (auto sh = f.shape.get()) {
                        sh.Opacity(inOp);
                    }
                }
                state.curFillOp = inOp;
            }
        } catch (...) {
        }
    }

    if (!state.useOverlayFade) {
        if (done && state.fadeClearAtEnd) {
            ApplyFillElements(state, false, state.currentBrush, 1.0);
            state.curFillOp = 0.0;
            state.fadeClearAtEnd = false;
        } else {
            for (auto& f : state.fills) {
                if (auto sh = f.shape.get()) {
                    try {
                        sh.Opacity(done ? 1.0 : std::clamp(state.curFillOp, 0.0, 1.0));
                    } catch (...) {
                    }
                }
            }
        }
    } else if (done) {
        for (auto& f : state.fills) {
            if (auto sh = f.shape.get()) {
                try {
                    sh.Opacity(1.0);
                } catch (...) {
                }
            }
        }
        state.curFillOp = IsClearZero(state.lastConfig) ? 0.0 : 1.0;
        state.useOverlayFade = false;
    }

    ApplyAnimatedVisibilityGroup(
        state.strokes, state.curLineOp, state.lastConfig.showLine, done);
    ApplyAnimatedVisibilityGroup(
        state.peeks, state.curPeekOp, state.lastConfig.showPeek, done);

    if (done) {
        state.animating = false;
        state.colorLerp = false;
        state.lerpBrush = nullptr;
    }
    state.applying = false;
}

void EnsureAnimTimerRunning() {
    bool anyAnimating = false;
    for (const auto& [tb, st] : g_taskbarStates) {
        if (st.animating) {
            anyAnimating = true;
            break;
        }
    }
    HWND primary = FindPrimaryTaskbarWnd();
    if (!primary) return;

    if (anyAnimating && !g_animTimerActive) {
        g_animTimerActive = true;
        SetTimer(primary, kAnimTimerId, 15, nullptr);
    } else if (!anyAnimating && g_animTimerActive) {
        g_animTimerActive = false;
        KillTimer(primary, kAnimTimerId);
    }
}

void ApplyAppearanceToState(TaskbarVisualState& state,
                            const AppearanceConfig& cfg,
                            bool forceBrushRebuild,
                            int transitionMs) {
    if (state.applying) return;
    if (state.fills.empty()) return;

    auto anchor = state.fills[0].shape.get();
    if (!anchor) return;

    // First application or animations disabled -> apply immediately.
    if (!state.hasLastApplied || transitionMs <= 0) {
        state.applying = true;
        state.animating = false;
        state.colorLerp = false;
        state.fadeClearAtEnd = false;
        state.useOverlayFade = false;
        state.lerpBrush = nullptr;
        if (state.fadeOverlay) {
            try {
                state.fadeOverlay.Visibility(Visibility::Collapsed);
                state.fadeOverlay.Fill(nullptr);
            } catch (...) {
            }
        }

        if (forceBrushRebuild || !state.hasLastApplied ||
            state.lastConfig.accent != cfg.accent ||
            state.lastConfig.color.A != cfg.color.A ||
            state.lastConfig.color.R != cfg.color.R ||
            state.lastConfig.color.G != cfg.color.G ||
            state.lastConfig.color.B != cfg.color.B ||
            state.lastConfig.blurRadius != cfg.blurRadius) {
            state.currentBrush =
                (cfg.accent == AccentMode::Normal)
                    ? nullptr
                    : CreateBrushForConfig(anchor, cfg);
        }

        ApplyFillElements(state, cfg.accent == AccentMode::Normal,
                          state.currentBrush, 1.0);
        state.curFillOp = IsClearZero(cfg) ? 0.0 : 1.0;
        state.curLineOp = cfg.showLine ? 1.0 : 0.0;
        state.curPeekOp = cfg.showPeek ? 1.0 : 0.0;
        ApplyAnimatedVisibilityGroup(state.strokes, state.curLineOp, cfg.showLine, true);
        ApplyAnimatedVisibilityGroup(state.peeks, state.curPeekOp, cfg.showPeek, true);

        state.lastConfig = cfg;
        state.hasLastApplied = true;
        state.applying = false;
        EnsureAnimTimerRunning();
        return;
    }

    // Animated transition from current visual state -> cfg
    state.applying = true;

    const bool fillSame =
        state.lastConfig.accent == cfg.accent &&
        state.lastConfig.color.A == cfg.color.A &&
        state.lastConfig.color.R == cfg.color.R &&
        state.lastConfig.color.G == cfg.color.G &&
        state.lastConfig.color.B == cfg.color.B &&
        state.lastConfig.blurRadius == cfg.blurRadius &&
        !forceBrushRebuild;

    state.animStartQpc = CurrentQpc();
    state.animDurationMs = transitionMs;
    state.startLineOp = state.curLineOp;
    state.endLineOp = cfg.showLine ? 1.0 : 0.0;
    state.startPeekOp = state.curPeekOp;
    state.endPeekOp = cfg.showPeek ? 1.0 : 0.0;

    state.fadeClearAtEnd = false;
    state.useOverlayFade = false;
    state.colorLerp = false;
    state.lerpBrush = nullptr;

    if (!fillSame) {
        const bool toClearZero = IsClearZero(cfg);
        const bool fromClearZero =
            IsClearZero(state.lastConfig) && !state.animating && !state.fadeClearAtEnd;

        Media::Brush outgoingBrush{nullptr};
        if (state.activeFillIsNative) {
            try {
                outgoingBrush = anchor.Fill();
            } catch (...) {
            }
        } else if (state.colorLerp && state.lerpBrush) {
            outgoingBrush = state.lerpBrush;
        } else {
            outgoingBrush = state.currentBrush;
        }

        if (toClearZero) {
            // Keep the currently visible brush (e.g. Normal or Blur) on BackgroundFill
            // and fade its Opacity smoothly down to 0 before swapping to clear.
            state.currentBrush = CreateBrushForConfig(anchor, cfg);
            if (state.curFillOp > 0.01 && outgoingBrush) {
                state.fadeClearAtEnd = true;
                state.startFillOp = state.curFillOp;
                state.endFillOp = 0.0;
                state.animDurationMs = std::max(
                    30, static_cast<int>(std::lround(transitionMs * state.curFillOp)));
                if (state.fadeOverlay) {
                    try {
                        state.fadeOverlay.Visibility(Visibility::Collapsed);
                        state.fadeOverlay.Fill(nullptr);
                    } catch (...) {
                    }
                }
            } else {
                ApplyFillElements(state, false, state.currentBrush, 1.0);
                state.startFillOp = state.endFillOp = state.curFillOp = 0.0;
            }
        } else if (fromClearZero || state.fadeClearAtEnd) {
            // Reversing a fade-out to clear, or fading in from clear -> target:
            // If reversing clear->normal when BackgroundFill STILL holds native, keep it & ramp up!
            const bool toNative = (cfg.accent == AccentMode::Normal);
            if (!(state.fadeClearAtEnd && toNative && state.activeFillIsNative)) {
                state.currentBrush =
                    toNative ? nullptr : CreateBrushForConfig(anchor, cfg);
                ApplyFillElements(state, toNative, state.currentBrush, state.curFillOp);
            }
            state.startFillOp = state.curFillOp;
            state.endFillOp = 1.0;
            double remain = 1.0 - std::clamp(state.curFillOp, 0.0, 1.0);
            state.animDurationMs = std::max(
                30, static_cast<int>(std::lround(transitionMs * remain)));
        } else if (IsSolidAccent(state.lastConfig) && IsSolidAccent(cfg)) {
            // Direct smooth RGBA color interpolation between two solid brushes.
            state.startColor = state.colorLerp
                                   ? state.curColor
                                   : EffectiveSolidColor(state.lastConfig);
            state.endColor = EffectiveSolidColor(cfg);
            state.curColor = state.startColor;

            Media::SolidColorBrush lerp;
            lerp.Color(state.startColor);
            state.lerpBrush = lerp;
            state.currentBrush = CreateBrushForConfig(anchor, cfg);
            state.colorLerp = true;
            ApplyFillElements(state, false, state.lerpBrush, 1.0);
            state.startFillOp = state.endFillOp = state.curFillOp = 1.0;
        } else {
            // Non-transparent cross-fade (e.g. Blur <-> Normal, Acrylic <-> Opaque)
            const bool toNative = (cfg.accent == AccentMode::Normal);
            state.currentBrush =
                toNative ? nullptr : CreateBrushForConfig(anchor, cfg);
            if (outgoingBrush && EnsureFadeOverlay(state)) {
                try {
                    state.fadeOverlay.Fill(outgoingBrush);
                    state.fadeOverlay.Opacity(1.0);
                    state.fadeOverlay.Visibility(Visibility::Visible);
                    state.useOverlayFade = true;
                } catch (...) {
                    state.useOverlayFade = false;
                }
            }
            ApplyFillElements(state, toNative, state.currentBrush,
                              state.useOverlayFade ? 0.0 : 1.0);
            state.startFillOp = state.useOverlayFade ? 0.0 : 1.0;
            state.endFillOp = 1.0;
            state.curFillOp = state.startFillOp;
        }
    } else {
        state.startFillOp = state.endFillOp = state.curFillOp;
    }

    const bool anyAnimated =
        state.fadeClearAtEnd ||
        state.useOverlayFade ||
        state.colorLerp ||
        std::abs(state.endFillOp - state.startFillOp) > 0.001 ||
        std::abs(state.endLineOp - state.startLineOp) > 0.001 ||
        std::abs(state.endPeekOp - state.startPeekOp) > 0.001;

    state.lastConfig = cfg;
    state.hasLastApplied = true;
    state.animating = anyAnimated;
    state.applying = false;

    if (anyAnimated) {
        StepTaskbarAnimation(state);
    }
    EnsureAnimTimerRunning();
}

void EvaluateAllTaskbars(bool forceApply = false) {
    if (g_modUnloading.load(std::memory_order_acquire)) return;

    std::vector<HWND> taskbars = FindAllTaskbarWindows();
    if (taskbars.empty()) return;

    ModSettings settings = SnapshotSettings();
    bool taskViewOpen =
        settings.taskViewFollowsDesktop && IsTaskViewOpen();
    std::vector<WindowMetadata> windows =
        taskViewOpen ? std::vector<WindowMetadata>{}
                     : CollectActiveWindows(settings);

    std::unordered_set<HWND> liveSet(taskbars.begin(), taskbars.end());
    for (auto it = g_taskbarStates.begin(); it != g_taskbarStates.end();) {
        if (liveSet.find(it->first) == liveSet.end()) {
            RemoveFadeOverlay(it->second);
            DetachCallbacksFromState(it->second);
            it = g_taskbarStates.erase(it);
        } else {
            ++it;
        }
    }

    for (HWND tb : taskbars) {
        auto& state = g_taskbarStates[tb];
        if (!EnsureTaskbarTargetsCollected(tb, state)) {
            continue;
        }

        HMONITOR mon = MonitorFromWindow(tb, MONITOR_DEFAULTTONEAREST);
        AppearanceConfig targetCfg = EvaluateAppearanceForMonitor(
            mon, taskViewOpen, windows, settings);

        if (forceApply || !state.hasLastApplied ||
            state.lastConfig != targetCfg) {
            ApplyAppearanceToState(
                state, targetCfg, forceApply,
                forceApply ? 0 : settings.transitionDurationMs);
        }
    }
}

void RequestCoalescedEvaluate(HWND primaryWnd = nullptr) {
    if (g_modUnloading.load(std::memory_order_acquire) || g_evalPending) {
        return;
    }
    HWND anchor = primaryWnd ? primaryWnd : FindPrimaryTaskbarWnd();
    if (!anchor) return;
    g_evalPending = true;
    SetTimer(anchor, kReevaluateTimerId, 25, nullptr);
}

// ============================================================================
//  Subclass & Event Hooks (running on Explorer's taskbar thread)
// ============================================================================

LRESULT CALLBACK PrimaryTaskbarSubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR) {
    if (g_modUnloading.load(std::memory_order_acquire)) {
        if (uMsg == WM_TIMER &&
            (wParam == kReevaluateTimerId || wParam == kRetryInitTimerId ||
             wParam == kAnimTimerId)) {
            KillTimer(hWnd, wParam);
            return 0;
        }
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    if (uMsg == WM_TIMER) {
        if (wParam == kAnimTimerId) {
            for (auto& [tb, st] : g_taskbarStates) {
                if (st.animating) {
                    StepTaskbarAnimation(st);
                }
            }
            EnsureAnimTimerRunning();
            return 0;
        }
        if (wParam == kReevaluateTimerId) {
            KillTimer(hWnd, kReevaluateTimerId);
            g_evalPending = false;
            EvaluateAllTaskbars(false);
            return 0;
        }
        if (wParam == kRetryInitTimerId) {
            EvaluateAllTaskbars(true);
            bool allReady = !g_taskbarStates.empty();
            for (const auto& [tb, st] : g_taskbarStates) {
                if (st.fills.empty()) {
                    allReady = false;
                    break;
                }
            }
            if (allReady || --g_retryInitRemaining <= 0) {
                KillTimer(hWnd, kRetryInitTimerId);
                g_retryInitRemaining = 0;
            }
            return 0;
        }
    }

    if (uMsg == WM_DISPLAYCHANGE || uMsg == WM_SETTINGCHANGE ||
        uMsg == WM_THEMECHANGED || uMsg == WM_DWMCOLORIZATIONCOLORCHANGED) {
        RequestCoalescedEvaluate(hWnd);
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void CALLBACK WinEventProc(HWINEVENTHOOK,
                           DWORD event,
                           HWND hwnd,
                           LONG idObject,
                           LONG idChild,
                           DWORD,
                           DWORD) {
    if (g_modUnloading.load(std::memory_order_acquire)) return;

    if (event != EVENT_SYSTEM_FOREGROUND &&
        event != EVENT_SYSTEM_MINIMIZESTART &&
        event != EVENT_SYSTEM_MINIMIZEEND &&
        event != EVENT_SYSTEM_MOVESIZESTART &&
        event != EVENT_SYSTEM_MOVESIZEEND) {
        if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !hwnd) {
            return;
        }
    }

    RequestCoalescedEvaluate();
}

void SetupTaskbarThread(void* param) {
    if (g_modUnloading.load(std::memory_order_acquire)) return;
    HWND primaryWnd = static_cast<HWND>(param);
    if (!primaryWnd || !IsWindow(primaryWnd)) return;

    if (!g_hooksInstalled) {
        WindhawkUtils::SetWindowSubclassFromAnyThread(
            primaryWnd, PrimaryTaskbarSubclassProc, 0);

        g_winEventHookSystem = SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_MINIMIZEEND,
            nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
        g_winEventHookObject = SetWinEventHook(
            EVENT_OBJECT_CREATE, EVENT_OBJECT_UNCLOAKED,
            nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
        g_hooksInstalled = true;
    }

    EvaluateAllTaskbars(true);
    g_retryInitRemaining = 12;
    SetTimer(primaryWnd, kRetryInitTimerId, 250, nullptr);
}

void ReapplyFromTaskbarThread(void* param) {
    HWND primaryWnd = static_cast<HWND>(param);
    if (primaryWnd && IsWindow(primaryWnd) && !g_hooksInstalled) {
        SetupTaskbarThread(primaryWnd);
        return;
    }
    EvaluateAllTaskbars(true);
}

void UninitFromTaskbarThread(void* param) {
    HWND primaryWnd = static_cast<HWND>(param);

    if (g_winEventHookSystem) {
        UnhookWinEvent(g_winEventHookSystem);
        g_winEventHookSystem = nullptr;
    }
    if (g_winEventHookObject) {
        UnhookWinEvent(g_winEventHookObject);
        g_winEventHookObject = nullptr;
    }
    g_evalPending = false;
    g_animTimerActive = false;
    g_hooksInstalled = false;

    if (primaryWnd && IsWindow(primaryWnd)) {
        KillTimer(primaryWnd, kReevaluateTimerId);
        KillTimer(primaryWnd, kRetryInitTimerId);
        KillTimer(primaryWnd, kAnimTimerId);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            primaryWnd, PrimaryTaskbarSubclassProc);
    }

    for (auto& [tb, st] : g_taskbarStates) {
        RestoreStateToNative(st);
    }
    g_taskbarStates.clear();
}

// ============================================================================
//  taskbar.dll Hooks
// ============================================================================

void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    TrayUI_StartTaskbar_Original(pThis);
    if (g_modUnloading.load(std::memory_order_acquire)) return;

    if (HWND primary = FindPrimaryTaskbarWnd()) {
        SetupTaskbarThread(primary);
    }
}

void WINAPI CSecondaryTray_InitModelAndHost_Hook(void* pThis,
                                                 void* taskbarModel) {
    CSecondaryTray_InitModelAndHost_Original(pThis, taskbarModel);
    if (g_modUnloading.load(std::memory_order_acquire)) return;

    if (HWND primary = FindPrimaryTaskbarWnd()) {
        RequestCoalescedEvaluate(primary);
    }
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryExW(
        L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
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

    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"Failed to hook taskbar.dll symbols");
        return false;
    }
    return true;
}

}  // namespace

// ============================================================================
//  Windhawk Entry Points
// ============================================================================

BOOL Wh_ModInit() {
    g_modUnloading.store(false, std::memory_order_release);
    LoadSettings();
    return HookTaskbarDllSymbols() ? TRUE : FALSE;
}

void Wh_ModAfterInit() {
    if (HWND primary = FindPrimaryTaskbarWnd()) {
        RunFromWindowThread(primary, SetupTaskbarThread, primary);
    }
}

void Wh_ModBeforeUninit() {
    g_modUnloading.store(true, std::memory_order_release);
    if (HWND primary = FindPrimaryTaskbarWnd()) {
        if (!RunFromWindowThread(primary, UninitFromTaskbarThread, primary)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                primary, PrimaryTaskbarSubclassProc);
        }
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (HWND primary = FindPrimaryTaskbarWnd()) {
        RunFromWindowThread(primary, ReapplyFromTaskbarThread, primary);
    }
}

// ==WindhawkMod==
// @id              dynamic-taskbar
// @name            Dynamic Taskbar
// @description     TranslucentTB-style per-monitor dynamic taskbar appearance (Desktop, Visible/Maximized states, #RRGGBBAA colors, rules, ignore lists, Peek/line toggles, and smooth transitions)
// @version         1.0.0
// @author          KnoxFury
// @github          https://github.com/KnoxFury
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -ldwmapi -lole32 -loleaut32 -lruntimeobject -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Dynamic Taskbar

Dynamically customize the Windows 11 taskbar appearance per monitor based on window
state—implementing TranslucentTB's window-state, rule, and visual-chrome workflow
natively inside `explorer.exe` with no separate background process and smooth
animated transitions.

## Features

### Per-monitor dynamic states
Each monitor's taskbar (`Shell_TrayWnd` and `Shell_SecondaryTrayWnd`) is evaluated
independently using narrow, child-filtered `SetWinEventHook` watchers on a dedicated
background thread:

1. **Maximized window** (`maximizedWindowAppearance`) — Active when a non-ignored window is maximized on that monitor.
2. **Visible window** (`visibleWindowAppearance`) — Active when a non-ignored visible window exists on that monitor (also handles maximized windows if Maximized state is disabled).
3. **Desktop** (`desktopAppearance`) — Active when no non-ignored windows are visible on that monitor (and optionally while **Task View** is open).

### Accent modes
- **`clear`** — Transparent or custom-tinted background (`#RRGGBBAA`, e.g. `#00000000` for fully transparent).
- **`blur`** — Frosted Gaussian blur backdrop with configurable **Blur radius** (`0–100`) and `#RRGGBBAA` tint.
- **`acrylic`** — Stronger composition-backed blur with enhanced saturation and `#RRGGBBAA` tint (uses at least `24` blur radius for multi-monitor XAML Island reliability).
- **`opaque`** — Solid color background using your chosen `#RRGGBBAA` color at full opacity.
- **`normal`** — Restores the native Windows 11 taskbar background.

### Taskbar details & animations
- **Show Aero Peek button** (`showPeek`) — Show or hide the far-corner "Show desktop" strip (`ShowDesktopStack`) per state.
- **Show taskbar border line** (`showLine`) — Show or hide the 1 px taskbar border line (`BackgroundStroke`) per state.
- **Smooth transitions** (`transitionDurationMs`) — Configurable eased transition (`180 ms` by default; `0` disables animations):
  - Fades `Opacity` smoothly when entering or leaving `clear` (`#00000000`)
  - Interpolates RGBA colors smoothly between two solid tints
  - Cross-fades non-transparent fills (`Blur <-> Normal`, `Acrylic <-> Opaque`) via a transient overlay rectangle
  - Fades the border line and Aero Peek strip in sync and reverses cleanly mid-animation
- **Task View follows Desktop** (`taskViewFollowsDesktop`) — Uses your Desktop appearance while Task View (`Win + Tab`) is open.

### Per-window rules & ignore lists
- **Window rules** (under both **Visible window appearance** and **Maximized window appearance**, active when that section is enabled): Override accent mode, `#RRGGBBAA` color, blur radius, Peek, and border line for specific windows matched by:
  - **Process name** (e.g. `notepad.exe` or `notepad`)
  - **Window class** (exact case-insensitive match)
  - **Window title** (case-insensitive substring match)
- **Ignored windows** (`ignoredWindows`): Exclude specific windows by class, title substring, or process name. Hidden, minimized, tool, click-through overlay, shell host, and DWM-cloaked windows (including windows on other virtual desktops) are ignored automatically.

## Comparison with existing mods
- **Dynamic Taskbar Transparency** (`dynamic-taskbar-transparency`): Focuses on shell-interaction state transitions (Start, Search, Task View, tray/notification flyouts, and maximized windows) and specifically fades taskbar `Opacity` over captured or preset blur/acrylic styles without per-state `#RRGGBBAA` tinting, an `opaque` mode, a **Visible window** state, per-window rules (`process_name` / `window_class` / `window_title`), ignore lists, or per-state Aero Peek (`showPeek`) and border line (`showLine`) toggles. **Dynamic Taskbar** mirrors TranslucentTB's full window-centric desktop/visible/maximized rule model, custom `#RRGGBBAA` colorization, and per-state chrome controls via `CTaskBand::GetTaskbarHost` without using `InitializeXamlDiagnosticsEx`.
- **Taskbar Background Helper** (`taskbar-background-helper`): Applies a single Win32 `SetWindowCompositionAttribute` background (`Always` or `Only when maximized`, with an excluded programs list) behind transparent taskbars styled with Windows 11 Taskbar Styler; it does not directly style `BackgroundFill`/`BackgroundStroke`/`ShowDesktopStack` per window rule or animate transitions.

## Compatibility notes
- Exit or uninstall **TranslucentTB**, and do not use this mod simultaneously with **Dynamic Taskbar Transparency**, **Taskbar Background Helper**, or **Windows 11 Taskbar Styler** background themes (otherwise both will fight over `BackgroundFill` / `BackgroundStroke`).
- Cleanly restores all original XAML values and detaches its hooks on mod disable/unload.
- Designed for **Windows 11 (x86-64)**.
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
    $description: Blur intensity (0-100; Acrylic mode enforces at least 24)
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
    $description: Blur intensity (0-100; Acrylic mode enforces at least 24)
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
    $description: Blur intensity (0-100; Acrylic mode enforces at least 24)
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
#include <cmath>
#include <cstdlib>
#include <cwctype>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <initguid.h>

#include <commctrl.h>
#include <d2d1_1.h>
#include <dwmapi.h>
#include <shlwapi.h>
#include <windows.graphics.effects.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
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

// Standard Windows SDK IGraphicsEffectD2D1Interop declaration
// (omitted in MinGW's windows.graphics.effects.h).
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
DECLARE_INTERFACE_IID_(IGraphicsEffectD2D1Interop,
                       IUnknown,
                       "2FC57384-A068-44D7-A331-30982FCF7177") {
    STDMETHOD(GetEffectId)(_Out_ GUID* id) PURE;
    STDMETHOD(GetNamedPropertyMapping)(
        LPCWSTR name,
        _Out_ UINT* index,
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

// ============================================================================
//  Clean Composition Backdrop Brush (MIT, built on Direct2D Effect CLSIDs)
// ============================================================================

struct EffectPropEntry {
    winrt::hstring name;
    UINT d2dIndex = 0;
    wf::IInspectable value{nullptr};
};

struct D2DEffectNode
    : winrt::implements<D2DEffectNode,
                        wge::IGraphicsEffect,
                        wge::IGraphicsEffectSource,
                        awge::IGraphicsEffectD2D1Interop> {
    GUID effectId{};
    winrt::hstring effectName;
    std::vector<EffectPropEntry> properties;
    std::vector<wge::IGraphicsEffectSource> sources;

    winrt::hstring Name() { return effectName; }
    void Name(winrt::hstring value) { effectName = std::move(value); }

    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) return E_INVALIDARG;
        *id = effectId;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name,
        UINT* index,
        awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!name || !index || !mapping) return E_INVALIDARG;
        std::wstring_view target(name);
        for (const auto& prop : properties) {
            if (prop.name == target) {
                *index = prop.d2dIndex;
                *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
                return S_OK;
            }
        }
        return E_INVALIDARG;
    }

    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = static_cast<UINT>(properties.size());
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetProperty(
        UINT index,
        winrt::impl::abi_t<wf::IPropertyValue>** value) noexcept override try {
        if (!value) return E_INVALIDARG;
        for (const auto& prop : properties) {
            if (prop.d2dIndex == index) {
                *value = prop.value.as<winrt::impl::abi_t<wf::IPropertyValue>>()
                             .detach();
                return S_OK;
            }
        }
        return E_BOUNDS;
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE GetSource(
        UINT index, awge::IGraphicsEffectSource** source) noexcept override try {
        if (!source) return E_INVALIDARG;
        if (index >= sources.size()) return E_BOUNDS;
        winrt::copy_to_abi(sources[index], *reinterpret_cast<void**>(source));
        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = static_cast<UINT>(sources.size());
        return S_OK;
    }
};

class BackdropTintBrush : public Media::XamlCompositionBrushBaseT<BackdropTintBrush> {
public:
    BackdropTintBrush(UIElement const& anchor,
                      float blurAmount,
                      winrt::Windows::UI::Color tint,
                      bool acrylicMode)
        : m_compositor(
              wuxh::ElementCompositionPreview::GetElementVisual(anchor)
                  .Compositor()),
          m_blurAmount(blurAmount),
          m_tint(tint),
          m_acrylic(acrylicMode) {}

    void OnConnected() {
        if (CompositionBrush()) return;

        bool effectsEnabled = true;
        try {
            winrt::Windows::UI::ViewManagement::UISettings ui;
            effectsEnabled = ui.AdvancedEffectsEnabled();
        } catch (...) {
        }

        if (!effectsEnabled) {
            CompositionBrush(m_compositor.CreateColorBrush(m_tint));
            return;
        }

        auto blur = winrt::make_self<D2DEffectNode>();
        blur->effectId = CLSID_D2D1GaussianBlur;
        blur->effectName = L"GaussianBlur";
        blur->sources.push_back(
            wuc::CompositionEffectSourceParameter(L"backdrop"));
        blur->properties = {
            {L"BlurAmount", D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION,
             wf::PropertyValue::CreateSingle(m_blurAmount)},
            {L"BorderMode", D2D1_GAUSSIANBLUR_PROP_BORDER_MODE,
             wf::PropertyValue::CreateUInt32(D2D1_BORDER_MODE_SOFT)},
        };

        wge::IGraphicsEffectSource backdropChain = *blur;

        winrt::com_ptr<D2DEffectNode> saturationNode;
        if (m_acrylic) {
            constexpr UINT kD2dSaturationPropSaturation = 0;  // D2D1_SATURATION_PROP_SATURATION
            saturationNode = winrt::make_self<D2DEffectNode>();
            saturationNode->effectId = CLSID_D2D1Saturation;
            saturationNode->effectName = L"Saturation";
            saturationNode->sources.push_back(backdropChain);
            saturationNode->properties = {
                {L"Saturation", kD2dSaturationPropSaturation,
                 wf::PropertyValue::CreateSingle(1.25f)},
            };
            backdropChain = *saturationNode;
        }

        auto flood = winrt::make_self<D2DEffectNode>();
        flood->effectId = CLSID_D2D1Flood;
        flood->effectName = L"TintFlood";
        flood->properties = {
            {L"Color", D2D1_FLOOD_PROP_COLOR,
             wf::PropertyValue::CreateSingleArray({
                 m_tint.R / 255.0f,
                 m_tint.G / 255.0f,
                 m_tint.B / 255.0f,
                 m_tint.A / 255.0f,
             })},
        };

        auto composite = winrt::make_self<D2DEffectNode>();
        composite->effectId = CLSID_D2D1Composite;
        composite->effectName = L"TintComposite";
        composite->sources.push_back(backdropChain);
        composite->sources.push_back(*flood);
        composite->properties = {
            {L"Mode", D2D1_COMPOSITE_PROP_MODE,
             wf::PropertyValue::CreateUInt32(D2D1_COMPOSITE_MODE_SOURCE_OVER)},
        };

        auto factory = m_compositor.CreateEffectFactory(*composite);
        auto effectBrush = factory.CreateBrush();
        effectBrush.SetSourceParameter(
            L"backdrop", m_compositor.CreateBackdropBrush());
        CompositionBrush(effectBrush);
    }

    void OnDisconnected() {
        if (const auto brush = CompositionBrush()) {
            brush.Close();
            CompositionBrush(nullptr);
        }
    }

private:
    wuc::Compositor m_compositor;
    float m_blurAmount;
    winrt::Windows::UI::Color m_tint;
    bool m_acrylic;
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

    bool hasTitleFilters = false;
    bool hasProcessFilters = false;
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

    auto accentStr = WindhawkUtils::StringSetting::make(L"%s.accent", prefix);
    cfg.accent = ParseAccentMode(accentStr.get(), defAccent);

    auto colorStr = WindhawkUtils::StringSetting::make(L"%s.color", prefix);
    cfg.color = ParseRgbaHex(colorStr.get(), defColor);

    cfg.showPeek = Wh_GetIntSetting(L"%s.showPeek", prefix) != 0;
    cfg.showLine = Wh_GetIntSetting(L"%s.showLine", prefix) != 0;
    cfg.blurRadius = static_cast<float>(
        std::clamp(Wh_GetIntSetting(L"%s.blurRadius", prefix), 0, 100));
    return cfg;
}

std::vector<WindowRule> LoadRulesList(PCWSTR prefix) {
    std::vector<WindowRule> out;
    for (int i = 0;; i++) {
        auto valStr = WindhawkUtils::StringSetting::make(
            L"%s.rules[%d].matchValue", prefix, i);
        auto typeStr = WindhawkUtils::StringSetting::make(
            L"%s.rules[%d].matchType", prefix, i);
        auto accentStr = WindhawkUtils::StringSetting::make(
            L"%s.rules[%d].accent", prefix, i);

        if (!typeStr.get()[0] && !accentStr.get()[0] && !valStr.get()[0]) {
            break;
        }

        std::wstring matchVal = ToLowerTrimmed(valStr.get());
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
        rule.appearance.accent =
            ParseAccentMode(accentStr.get(), AccentMode::Clear);

        auto colorStr = WindhawkUtils::StringSetting::make(
            L"%s.rules[%d].color", prefix, i);
        rule.appearance.color = ParseRgbaHex(colorStr.get(), {0, 0, 0, 0});
        rule.appearance.showPeek =
            Wh_GetIntSetting(L"%s.rules[%d].showPeek", prefix, i) != 0;
        rule.appearance.showLine =
            Wh_GetIntSetting(L"%s.rules[%d].showLine", prefix, i) != 0;
        rule.appearance.blurRadius = static_cast<float>(std::clamp(
            Wh_GetIntSetting(L"%s.rules[%d].blurRadius", prefix, i), 0, 100));

        out.push_back(std::move(rule));
    }
    return out;
}

std::vector<std::wstring> LoadIgnoredArray(PCWSTR key) {
    std::vector<std::wstring> list;
    for (int i = 0;; i++) {
        auto val = WindhawkUtils::StringSetting::make(L"%s[%d]", key, i);
        if (!val.get()[0]) {
            if (i == 0) {
                auto next = WindhawkUtils::StringSetting::make(L"%s[%d]", key, 1);
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

    s.hasTitleFilters = !s.ignored.windowTitles.empty();
    s.hasProcessFilters = !s.ignored.processNames.empty();
    auto scanRules = [&](const std::vector<WindowRule>& rules) {
        for (const auto& r : rules) {
            if (r.matchType == RuleMatchType::WindowTitle) s.hasTitleFilters = true;
            if (r.matchType == RuleMatchType::ProcessName) s.hasProcessFilters = true;
        }
    };
    if (s.visibleWindow.enabled) scanRules(s.visibleRules);
    if (s.maximizedWindow.enabled) scanRules(s.maximizedRules);

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
//  Explorer taskbar.dll Symbols, Primary HWND Cache & XAML Lookup
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

using CSecondaryTray_InitModelAndHost_t = void(WINAPI*)(void* pThis, void* taskbarModel);
CSecondaryTray_InitModelAndHost_t CSecondaryTray_InitModelAndHost_Original = nullptr;

using InternalGetWindowText_t = int(WINAPI*)(HWND, LPWSTR, int);
InternalGetWindowText_t g_internalGetWindowText = nullptr;

void ReadWindowTitleSafe(HWND hwnd, wchar_t* buf, int cchMax) {
    if (!buf || cchMax <= 0) return;
    buf[0] = L'\0';
    if (g_internalGetWindowText) {
        g_internalGetWindowText(hwnd, buf, cchMax);
        return;
    }
    DWORD pid = 0;
    if (GetWindowThreadProcessId(hwnd, &pid) && pid != GetCurrentProcessId()) {
        GetWindowTextW(hwnd, buf, cchMax);
    }
}

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

std::atomic<HWND> g_cachedPrimaryTaskbarWnd{nullptr};

HWND FindPrimaryTaskbarWnd() {
    HWND cached = g_cachedPrimaryTaskbarWnd.load(std::memory_order_acquire);
    if (cached && IsWindow(cached)) {
        wchar_t cls[32]{};
        if (GetClassNameW(cached, cls, ARRAYSIZE(cls)) &&
            _wcsicmp(cls, L"Shell_TrayWnd") == 0) {
            return cached;
        }
    }

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
    if (result) {
        g_cachedPrimaryTaskbarWnd.store(result, std::memory_order_release);
    }
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

size_t ResolveTaskbarElementOffset() {
    static const size_t resolved = []() -> size_t {
        const BYTE* code =
            static_cast<const BYTE*>(TaskbarHost_FrameHeight_Original);
        for (int i = 0; code && i < 6; i++) {
            WH_DISASM_RESULT dis{};
            if (!Wh_Disasm(const_cast<BYTE*>(code), &dis)) break;
            code += dis.length;
            std::string_view text(dis.text);
            if (text == "ret") break;
            if (text.starts_with("add rcx, 0x")) {
                size_t val =
                    static_cast<size_t>(std::strtoull(dis.text + 11, nullptr, 16));
                if (val > 0 && val <= 0x200) return val;
            }
            if (text.starts_with("lea rcx, [rcx+0x")) {
                size_t val =
                    static_cast<size_t>(std::strtoull(dis.text + 16, nullptr, 16));
                if (val > 0 && val <= 0x200) return val;
            }
        }
        return 0x10;
    }();
    return resolved;
}

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t offset = ResolveTaskbarElementOffset();
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

// ============================================================================
//  Window State & Rules Evaluation (per monitor)
// ============================================================================

struct WindowMetadata {
    HWND hwnd = nullptr;
    RECT frameRect{};
    bool maximized = false;
    std::wstring classNameLower;
    std::wstring titleLower;
    std::wstring processNameLower;
};

// Accessed only on Explorer's taskbar thread.
[[clang::no_destroy]] std::optional<std::unordered_map<DWORD, std::wstring>>
    g_pidProcessCache{std::in_place};

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
           cls == L"xamlexplorerhostislandwindow" ||
           cls == L"applicationmanager_desktopshellwindow";
}

bool ProcessMatchesExactOrStem(const std::wstring& procLower,
                               const std::wstring& patternLower) {
    if (procLower == patternLower) return true;
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

std::wstring GetWindowProcessNameLowerCached(
    HWND hwnd, std::unordered_set<DWORD>& seenPids) {
    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hwnd, &pid) || !pid) return {};
    seenPids.insert(pid);

    if (g_pidProcessCache) {
        if (auto it = g_pidProcessCache->find(pid);
            it != g_pidProcessCache->end()) {
            return it->second;
        }
    }

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return {};
    wchar_t path[MAX_PATH]{};
    DWORD len = ARRAYSIZE(path);
    BOOL ok = QueryFullProcessImageNameW(hProc, 0, path, &len);
    CloseHandle(hProc);
    if (!ok || len == 0) return {};

    std::wstring name = ToLowerTrimmed(PathFindFileNameW(path));
    if (g_pidProcessCache) {
        (*g_pidProcessCache)[pid] = name;
    }
    return name;
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
            if (_wcsicmp(cls, L"XamlExplorerHostIslandWindow") == 0 ||
                _wcsicmp(cls, L"XamlExplorerHostWindow") == 0 ||
                _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
                wchar_t title[128]{};
                ReadWindowTitleSafe(hwnd, title, ARRAYSIZE(title));
                if (_wcsicmp(title, L"Task View") == 0) {
                    reinterpret_cast<Ctx*>(lp)->open = true;
                    return FALSE;
                }
                // Non-English fallback: Task View spans the full monitor while
                // Alt+Tab in XamlExplorerHostIslandWindow is smaller.
                if (_wcsicmp(cls, L"XamlExplorerHostIslandWindow") == 0) {
                    RECT rc{};
                    HMONITOR mon =
                        MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
                    MONITORINFO mi{};
                    mi.cbSize = sizeof(mi);
                    if (mon && GetMonitorInfoW(mon, &mi) &&
                        GetWindowRect(hwnd, &rc)) {
                        if ((rc.right - rc.left) >=
                                (mi.rcMonitor.right - mi.rcMonitor.left - 4) &&
                            (rc.bottom - rc.top) >=
                                (mi.rcMonitor.bottom - mi.rcMonitor.top - 4)) {
                            reinterpret_cast<Ctx*>(lp)->open = true;
                            return FALSE;
                        }
                    }
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
    std::unordered_set<DWORD> seenPids;
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

            if (ctx->settings->hasTitleFilters) {
                wchar_t titleBuf[512]{};
                ReadWindowTitleSafe(hwnd, titleBuf, ARRAYSIZE(titleBuf));
                wm.titleLower = ToLowerTrimmed(titleBuf);
            }
            if (ctx->settings->hasProcessFilters) {
                wm.processNameLower =
                    GetWindowProcessNameLowerCached(hwnd, ctx->seenPids);
            }

            if (MatchesIgnoreList(wm, ctx->settings->ignored)) {
                return TRUE;
            }

            ctx->windows.push_back(std::move(wm));
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&ctx));

    // Prune closed PIDs so recycled PIDs never return stale process names.
    if (g_pidProcessCache) {
        for (auto it = g_pidProcessCache->begin();
             it != g_pidProcessCache->end();) {
            if (ctx.seenPids.find(it->first) == ctx.seenPids.end()) {
                it = g_pidProcessCache->erase(it);
            } else {
                ++it;
            }
        }
    }

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

        if (wm.maximized && settings.maximizedWindow.enabled) {
            hasMaximized = true;
            if (!matchedMaxRule) {
                for (const auto& r : settings.maximizedRules) {
                    if (MatchesRule(wm, r)) {
                        matchedMaxRule = &r;
                        break;
                    }
                }
            }
        } else if (settings.visibleWindow.enabled) {
            // Also treat maximized windows as visible when Maximized state is off.
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
//  XAML Element Styling & Animated State Tracking (taskbar UI thread)
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
    std::vector<TrackedFillElement> fills;
    std::vector<TrackedVisibilityElement> strokes;
    std::vector<TrackedVisibilityElement> peeks;

    Shapes::Rectangle fadeOverlay{nullptr};
    Controls::Panel fadeOverlayParent{nullptr};

    bool hasLastApplied = false;
    AppearanceConfig lastConfig{};
    bool applying = false;
    Media::Brush currentBrush{nullptr};
    bool activeFillIsNative = true;

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

// Accessed only from Explorer's taskbar thread; reset on unload so process
// shutdown never releases XAML references on an arbitrary shutdown thread.
[[clang::no_destroy]] std::optional<std::unordered_map<HWND, TaskbarVisualState>>
    g_taskbarStates{std::in_place};

constexpr UINT_PTR kReevaluateTimerId = 0x44545401ULL;
constexpr UINT_PTR kRetryInitTimerId = 0x44545402ULL;
constexpr UINT_PTR kAnimTimerId = 0x44545403ULL;
int g_retryInitRemaining = 0;
bool g_evalPending = false;
bool g_animTimerActive = false;
bool g_subclassInstalled = false;

UINT g_msgSetup = 0;
UINT g_msgReapply = 0;
UINT g_msgUninit = 0;
UINT g_msgScheduleEval = 0;
std::atomic<bool> g_evalPosted{false};

// Dedicated WinEvent hook thread state.
HANDLE g_winEventThread = nullptr;
std::atomic<DWORD> g_winEventThreadId{0};
UINT g_msgUpdateEventHooks = 0;

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

Media::Brush CreateBrushForConfig(UIElement const& anchor,
                                  const AppearanceConfig& cfg) {
    switch (cfg.accent) {
        case AccentMode::Normal:
            return nullptr;
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
            return winrt::make<BackdropTintBrush>(
                anchor, cfg.blurRadius, cfg.color, false);
        case AccentMode::Acrylic:
            return winrt::make<BackdropTintBrush>(
                anchor, std::max(cfg.blurRadius, 24.0f), cfg.color, true);
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
                state.fadeOverlay.RadiusX(r.RadiusX());
                state.fadeOverlay.RadiusY(r.RadiusY());
                state.fadeOverlay.Margin(r.Margin());
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
                if (g_modUnloading.load(std::memory_order_acquire) ||
                    !g_taskbarStates) {
                    return;
                }
                auto it = g_taskbarStates->find(taskbarWnd);
                if (it == g_taskbarStates->end() || it->second.applying) return;
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
                if (g_modUnloading.load(std::memory_order_acquire) ||
                    !g_taskbarStates) {
                    return;
                }
                auto it = g_taskbarStates->find(taskbarWnd);
                if (it == g_taskbarStates->end() || it->second.applying) return;
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
                if (g_modUnloading.load(std::memory_order_acquire) ||
                    !g_taskbarStates) {
                    return;
                }
                auto it = g_taskbarStates->find(taskbarWnd);
                if (it == g_taskbarStates->end() || it->second.applying) return;
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

    XamlRoot root = GetTaskbarXamlRoot(taskbarWnd);
    if (!root) return false;
    FrameworkElement content = root.Content().try_as<FrameworkElement>();
    if (!content) return false;

    WalkDescendants(content, [&](FrameworkElement const& child) -> bool {
        try {
            auto name = child.Name();
            if (name == L"HoverFlyoutBackgroundControl") {
                return false;
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
                double inv = 1.0 - t;
                double inOp = 1.0 - inv * inv;
                state.fadeOverlay.Opacity(inv);
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
    if (!g_taskbarStates) return;
    bool anyAnimating = false;
    for (const auto& [tb, st] : *g_taskbarStates) {
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
    if (state.applying || state.fills.empty()) return;

    auto anchor = state.fills[0].shape.get();
    if (!anchor) return;

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
        ApplyAnimatedVisibilityGroup(
            state.strokes, state.curLineOp, cfg.showLine, true);
        ApplyAnimatedVisibilityGroup(
            state.peeks, state.curPeekOp, cfg.showPeek, true);

        state.lastConfig = cfg;
        state.hasLastApplied = true;
        state.applying = false;
        EnsureAnimTimerRunning();
        return;
    }

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
    if (g_modUnloading.load(std::memory_order_acquire) || !g_taskbarStates) {
        return;
    }

    std::vector<HWND> taskbars = FindAllTaskbarWindows();
    if (taskbars.empty()) return;

    ModSettings settings = SnapshotSettings();
    bool taskViewOpen =
        settings.taskViewFollowsDesktop && IsTaskViewOpen();
    std::vector<WindowMetadata> windows =
        taskViewOpen ? std::vector<WindowMetadata>{}
                     : CollectActiveWindows(settings);

    std::unordered_set<HWND> liveSet(taskbars.begin(), taskbars.end());
    for (auto it = g_taskbarStates->begin(); it != g_taskbarStates->end();) {
        if (liveSet.find(it->first) == liveSet.end()) {
            RemoveFadeOverlay(it->second);
            DetachCallbacksFromState(it->second);
            it = g_taskbarStates->erase(it);
        } else {
            ++it;
        }
    }

    for (HWND tb : taskbars) {
        auto& state = (*g_taskbarStates)[tb];
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

void StartRetryInitTimer(HWND primaryWnd, int attempts = 12) {
    if (!primaryWnd || !IsWindow(primaryWnd)) return;
    g_retryInitRemaining = std::max(g_retryInitRemaining, attempts);
    SetTimer(primaryWnd, kRetryInitTimerId, 250, nullptr);
}

// ============================================================================
//  Dedicated WinEvent Hook Thread (keeps Explorer's UI thread clean)
// ============================================================================

struct WinEventThreadHooks {
    HWINEVENTHOOK foreground = nullptr;
    HWINEVENTHOOK moveSizeEnd = nullptr;
    HWINEVENTHOOK minimize = nullptr;
    HWINEVENTHOOK objectVis = nullptr;
    HWINEVENTHOOK locationChange = nullptr;
    HWINEVENTHOOK cloaked = nullptr;
    HWINEVENTHOOK nameChange = nullptr;
};

void CALLBACK WinEventProc(HWINEVENTHOOK,
                           DWORD,
                           HWND hwnd,
                           LONG idObject,
                           LONG idChild,
                           DWORD,
                           DWORD) {
    if (g_modUnloading.load(std::memory_order_acquire)) return;
    if (!hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return;
    if (g_evalPosted.load(std::memory_order_relaxed)) return;
    if (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD) return;

    HWND primary = FindPrimaryTaskbarWnd();
    if (!primary || !g_msgScheduleEval) return;

    if (!g_evalPosted.exchange(true, std::memory_order_acq_rel)) {
        PostMessageW(primary, g_msgScheduleEval, 0, 0);
    }
}

void SyncTitleEventHook(WinEventThreadHooks& hooks, bool needTitleHook) {
    if (needTitleHook && !hooks.nameChange) {
        hooks.nameChange = SetWinEventHook(
            EVENT_OBJECT_NAMECHANGE, EVENT_OBJECT_NAMECHANGE,
            nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    } else if (!needTitleHook && hooks.nameChange) {
        UnhookWinEvent(hooks.nameChange);
        hooks.nameChange = nullptr;
    }
}

DWORD WINAPI WinEventThreadProc(LPVOID) {
    WinEventThreadHooks hooks;
    hooks.foreground = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
        nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    hooks.moveSizeEnd = SetWinEventHook(
        EVENT_SYSTEM_MOVESIZEEND, EVENT_SYSTEM_MOVESIZEEND,
        nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    hooks.minimize = SetWinEventHook(
        EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND,
        nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    hooks.objectVis = SetWinEventHook(
        EVENT_OBJECT_CREATE, EVENT_OBJECT_HIDE,
        nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    hooks.locationChange = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
        nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    hooks.cloaked = SetWinEventHook(
        EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED,
        nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    ModSettings s = SnapshotSettings();
    SyncTitleEventHook(hooks, s.hasTitleFilters);

    MSG msg;
    // Force message queue creation before signaling ready via PeekMessage.
    PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == g_msgUpdateEventHooks) {
            ModSettings cur = SnapshotSettings();
            SyncTitleEventHook(hooks, cur.hasTitleFilters);
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (hooks.foreground) UnhookWinEvent(hooks.foreground);
    if (hooks.moveSizeEnd) UnhookWinEvent(hooks.moveSizeEnd);
    if (hooks.minimize) UnhookWinEvent(hooks.minimize);
    if (hooks.objectVis) UnhookWinEvent(hooks.objectVis);
    if (hooks.locationChange) UnhookWinEvent(hooks.locationChange);
    if (hooks.cloaked) UnhookWinEvent(hooks.cloaked);
    if (hooks.nameChange) UnhookWinEvent(hooks.nameChange);
    return 0;
}

void StartWinEventThread() {
    if (g_winEventThread) return;
    DWORD tid = 0;
    g_winEventThread =
        CreateThread(nullptr, 0, WinEventThreadProc, nullptr, 0, &tid);
    if (g_winEventThread) {
        g_winEventThreadId.store(tid, std::memory_order_release);
    }
}

void StopWinEventThread() {
    DWORD tid = g_winEventThreadId.exchange(0, std::memory_order_acq_rel);
    if (tid) {
        // Retry briefly in case the worker queue wasn't created yet.
        for (int i = 0; i < 20 && !PostThreadMessageW(tid, WM_QUIT, 0, 0); i++) {
            Sleep(5);
        }
    }
    if (g_winEventThread) {
        WaitForSingleObject(g_winEventThread, 2000);
        CloseHandle(g_winEventThread);
        g_winEventThread = nullptr;
    }
}

// ============================================================================
//  Primary Taskbar Subclass (UI-thread lifecycle & animations)
// ============================================================================

LRESULT CALLBACK PrimaryTaskbarSubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, DWORD_PTR) {
    if (uMsg == g_msgUninit) {
        g_evalPending = false;
        g_animTimerActive = false;
        g_evalPosted.store(false, std::memory_order_release);
        KillTimer(hWnd, kReevaluateTimerId);
        KillTimer(hWnd, kRetryInitTimerId);
        KillTimer(hWnd, kAnimTimerId);

        if (g_taskbarStates) {
            for (auto& [tb, st] : *g_taskbarStates) {
                RestoreStateToNative(st);
            }
            g_taskbarStates.reset();
        }
        g_pidProcessCache.reset();
        return 0;
    }

    if (g_modUnloading.load(std::memory_order_acquire)) {
        if (uMsg == WM_TIMER &&
            (wParam == kReevaluateTimerId || wParam == kRetryInitTimerId ||
             wParam == kAnimTimerId)) {
            KillTimer(hWnd, wParam);
            return 0;
        }
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    if (uMsg == g_msgSetup) {
        g_cachedPrimaryTaskbarWnd.store(hWnd, std::memory_order_release);
        StartWinEventThread();
        EvaluateAllTaskbars(true);
        StartRetryInitTimer(hWnd, 12);
        return 0;
    }

    if (uMsg == g_msgReapply) {
        if (DWORD tid = g_winEventThreadId.load(std::memory_order_acquire)) {
            PostThreadMessageW(tid, g_msgUpdateEventHooks, 0, 0);
        }
        EvaluateAllTaskbars(true);
        return 0;
    }

    if (uMsg == g_msgScheduleEval) {
        g_evalPosted.store(false, std::memory_order_release);
        RequestCoalescedEvaluate(hWnd);
        return 0;
    }

    if (uMsg == WM_TIMER) {
        if (wParam == kAnimTimerId) {
            if (g_taskbarStates) {
                for (auto& [tb, st] : *g_taskbarStates) {
                    if (st.animating) {
                        StepTaskbarAnimation(st);
                    }
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
            bool allReady = g_taskbarStates && !g_taskbarStates->empty();
            if (g_taskbarStates) {
                for (const auto& [tb, st] : *g_taskbarStates) {
                    if (st.fills.empty()) {
                        allReady = false;
                        break;
                    }
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

void EnsurePrimaryTaskbarSubclassedAndSetup(HWND primary) {
    if (!primary || !IsWindow(primary)) return;
    g_cachedPrimaryTaskbarWnd.store(primary, std::memory_order_release);
    if (!g_subclassInstalled) {
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(
                primary, PrimaryTaskbarSubclassProc, 0)) {
            g_subclassInstalled = true;
        }
    }
    if (g_subclassInstalled) {
        SendMessageW(primary, g_msgSetup, 0, 0);
    }
}

// ============================================================================
//  taskbar.dll Hooks
// ============================================================================

void WINAPI TrayUI_StartTaskbar_Hook(void* pThis) {
    TrayUI_StartTaskbar_Original(pThis);
    if (g_modUnloading.load(std::memory_order_acquire)) return;

    if (HWND primary = FindPrimaryTaskbarWnd()) {
        EnsurePrimaryTaskbarSubclassedAndSetup(primary);
    }
}

void WINAPI CSecondaryTray_InitModelAndHost_Hook(void* pThis,
                                                 void* taskbarModel) {
    CSecondaryTray_InitModelAndHost_Original(pThis, taskbarModel);
    if (g_modUnloading.load(std::memory_order_acquire)) return;

    if (HWND primary = FindPrimaryTaskbarWnd()) {
        RequestCoalescedEvaluate(primary);
        StartRetryInitTimer(primary, 12);
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
            {LR"(public: virtual void __cdecl CSecondaryTray::InitModelAndHost(struct winrt::WindowsUdk::UI::Shell::TaskbarModel))"},
            &CSecondaryTray_InitModelAndHost_Original,
            CSecondaryTray_InitModelAndHost_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(
            module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
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
    if (HMODULE user32 = GetModuleHandleW(L"user32.dll")) {
        g_internalGetWindowText = reinterpret_cast<InternalGetWindowText_t>(
            GetProcAddress(user32, "InternalGetWindowText"));
    }

    g_msgSetup = RegisterWindowMessageW(L"Windhawk_DTT_Setup_" WH_MOD_ID);
    g_msgReapply = RegisterWindowMessageW(L"Windhawk_DTT_Reapply_" WH_MOD_ID);
    g_msgUninit = RegisterWindowMessageW(L"Windhawk_DTT_Uninit_" WH_MOD_ID);
    g_msgScheduleEval =
        RegisterWindowMessageW(L"Windhawk_DTT_ScheduleEval_" WH_MOD_ID);
    g_msgUpdateEventHooks =
        RegisterWindowMessageW(L"Windhawk_DTT_UpdateHooks_" WH_MOD_ID);

    LoadSettings();
    return HookTaskbarDllSymbols() ? TRUE : FALSE;
}

void Wh_ModAfterInit() {
    if (HWND primary = FindPrimaryTaskbarWnd()) {
        EnsurePrimaryTaskbarSubclassedAndSetup(primary);
    }
}

void Wh_ModBeforeUninit() {
    g_modUnloading.store(true, std::memory_order_release);
    StopWinEventThread();

    if (HWND primary = FindPrimaryTaskbarWnd()) {
        SendMessageW(primary, g_msgUninit, 0, 0);
        if (g_subclassInstalled) {
            g_subclassInstalled = false;
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                primary, PrimaryTaskbarSubclassProc);
        }
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (HWND primary = FindPrimaryTaskbarWnd()) {
        SendMessageW(primary, g_msgReapply, 0, 0);
    }
}

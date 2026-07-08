// ==WindhawkMod==
// @id              dynamic-taskbar-transparency
// @name            Dynamic Taskbar Transparency
// @description     Dynamically changes the Windows 11 taskbar XAML background transparency for desktop, Start, search, tray flyouts, task view, and maximized windows.
// @version         0.3.8
// @author          11581
// @github          https://github.com/r1file
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Dynamic Taskbar Transparency

![Dynamic Taskbar Transparency demo](https://i.imgur.com/Ab7hMYp.gif)

This mod applies a TranslucentTB-like state machine to the Windows 11 taskbar.
It keeps the taskbar clear on the desktop or when no maximized window is
present, then restores or changes the taskbar appearance while Start, search,
task view, notification/quick settings flyouts, tray flyouts, other shell
interactions, or maximized windows are active.

Version 0.3 edits the Windows 11 taskbar XAML background rectangles directly.
It fades the rectangles with `Opacity` instead of applying a colored alpha
overlay, and it can fall back to the captured existing style, clear the mod's
local overrides for the Windows native/default style, clear, blur, or
blur-backed acrylic.

State detection is driven by shell/window events and uses heuristic window
class/process-name matching for Start, search, task view, tray flyouts, and
other shell surfaces. These heuristics can change between Windows 11 builds.

Do not run this together with TranslucentTB or another tool that continuously
writes the same taskbar background.

Compared with taskbar-background-helper, this mod focuses on a TranslucentTB-
style state machine: desktop/no-maximized-window, maximized windows, Start,
search, task view, tray flyouts, notification/quick settings flyouts, and an
optional catch-all for other shell surfaces can each choose their own
appearance and fade duration. taskbar-background-helper is a lower-level
background helper for always/maximized behavior and pairs well with Taskbar
Styler; this mod adds per-shell-state switching and animation on top of the
same general taskbar background use case.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- desktop:
  - style: clear
    $name: Style
    $options:
    - captured: Existing style / other mods
    - native: Windows native/default
    - clear: Clear
    - blur: Blur
    - acrylic: Acrylic
  $name: Desktop / no maximized window
- fallback:
  - style: captured
    $name: Style
    $description: Used by states whose style is set to Fallback.
    $options:
    - captured: Existing style / other mods
    - native: Windows native/default
    - clear: Clear
    - blur: Blur
    - acrylic: Acrylic
  $name: Fallback appearance
- maximized:
  - enabled: true
    $name: Enabled
  - style: fallback
    $name: Style
    $options:
    - fallback: Fallback
    - captured: Existing style / other mods
    - native: Windows native/default
    - clear: Clear
    - blur: Blur
    - acrylic: Acrylic
  $name: Maximized window
- startOpened:
  - enabled: true
    $name: Enabled
  - style: fallback
    $name: Style
    $options:
    - fallback: Fallback
    - captured: Existing style / other mods
    - native: Windows native/default
    - clear: Clear
    - blur: Blur
    - acrylic: Acrylic
  $name: Start opened
- searchOpened:
  - enabled: true
    $name: Enabled
  - style: fallback
    $name: Style
    $options:
    - fallback: Fallback
    - captured: Existing style / other mods
    - native: Windows native/default
    - clear: Clear
    - blur: Blur
    - acrylic: Acrylic
  $name: Search opened
- taskViewOpened:
  - enabled: true
    $name: Enabled
  - style: fallback
    $name: Style
    $options:
    - fallback: Fallback
    - captured: Existing style / other mods
    - native: Windows native/default
    - clear: Clear
    - blur: Blur
    - acrylic: Acrylic
  $name: Task view opened
- trayFlyoutOpened:
  - enabled: true
    $name: Enabled
  - style: fallback
    $name: Style
    $options:
    - fallback: Fallback
    - captured: Existing style / other mods
    - native: Windows native/default
    - clear: Clear
    - blur: Blur
    - acrylic: Acrylic
  $name: Notification / quick settings / tray flyouts
- otherInteraction:
  - enabled: false
    $name: Enabled
  - style: fallback
    $name: Style
    $description: Optional catch-all for unclassified shell-host surfaces.
    $options:
    - fallback: Fallback
    - captured: Existing style / other mods
    - native: Windows native/default
    - clear: Clear
    - blur: Blur
    - acrylic: Acrylic
  $name: Other shell interaction
- animation:
  - durationMs: 220
    $name: Fade duration (ms)
    $description: Set to 0 to disable fading.
  $name: Animation
- detection:
  - fullscreenAsMaximized: true
    $name: Treat fullscreen windows as maximized
  $name: Detection
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <windows.h>
#include <dwmapi.h>
#include <initguid.h>
#include <d2d1_1.h>
#include <windows.graphics.effects.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/base.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cwctype>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#ifndef DWMWA_CLOAKED
#define DWMWA_CLOAKED 14
#endif

using namespace winrt::Windows::UI::Xaml;
namespace Controls = winrt::Windows::UI::Xaml::Controls;
namespace Core = winrt::Windows::UI::Core;
namespace Media = winrt::Windows::UI::Xaml::Media;
namespace Shapes = winrt::Windows::UI::Xaml::Shapes;
namespace ViewManagement = winrt::Windows::UI::ViewManagement;
namespace wf = winrt::Windows::Foundation;
namespace wge = winrt::Windows::Graphics::Effects;
namespace wuc = winrt::Windows::UI::Composition;
namespace wuxh = winrt::Windows::UI::Xaml::Hosting;
namespace awge = ABI::Windows::Graphics::Effects;

enum class AppearanceStyle {
    fallback,
    captured,
    native,
    clear,
    blur,
    acrylic,
};

enum class TaskbarDynamicState {
    desktop,
    maximized,
    startOpened,
    searchOpened,
    taskViewOpened,
    trayFlyoutOpened,
    otherInteraction,
};

struct Appearance {
    bool enabled = true;
    AppearanceStyle style = AppearanceStyle::clear;
    BYTE opacity = 0;
};

struct Settings {
    Appearance desktop;
    Appearance fallback;
    Appearance maximized;
    Appearance startOpened;
    Appearance searchOpened;
    Appearance taskViewOpened;
    Appearance trayFlyoutOpened;
    Appearance otherInteraction;
    int animationDurationMs = 220;
    bool fullscreenAsMaximized = true;
};

struct ShellActivity {
    bool startOpened = false;
    bool searchOpened = false;
    bool taskViewOpened = false;
    bool trayFlyoutOpened = false;
    bool otherInteraction = false;
};

struct TrackedBackgroundElement {
    winrt::weak_ref<Shapes::Rectangle> element;
    winrt::weak_ref<Controls::Grid> parentGrid;
    Media::Brush originalFill{nullptr};
    Media::Brush originalParentBackground{nullptr};
    double originalOpacity = 1.0;
    Media::Brush nativeFill{nullptr};
    Media::Brush nativeParentBackground{nullptr};
    double nativeOpacity = 1.0;
    bool hasAppliedAppearance = false;
    AppearanceStyle appliedStyle = AppearanceStyle::clear;
    void* abi = nullptr;
};

std::mutex g_settingsMutex;
Settings g_settings;

std::mutex g_elementsMutex;
std::vector<TrackedBackgroundElement>& BackgroundElements() {
    static auto* elements = new std::vector<TrackedBackgroundElement>();
    return *elements;
}

struct ActiveAppearance {
    bool hasValue = false;
    Appearance appearance;
    bool restoreOriginal = false;
};

std::mutex g_activeAppearanceMutex;
ActiveAppearance g_activeAppearance;

std::atomic<bool> g_stopWorker = false;
std::thread* g_workerThread = nullptr;
std::thread* g_winEventThread = nullptr;
std::atomic<DWORD> g_winEventThreadId = 0;
std::atomic<bool> g_taskbarViewDllLoaded = false;
std::atomic<bool> g_scanPending = false;
std::atomic<bool> g_detectionPending = true;
std::mutex g_detectionMutex;
std::condition_variable g_detectionCondition;

TaskbarDynamicState g_lastLoggedState = TaskbarDynamicState::desktop;
bool g_hasLoggedState = false;

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void*);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;

using TaskListButton_UpdateButtonPadding_t = void(WINAPI*)(void*);
TaskListButton_UpdateButtonPadding_t TaskListButton_UpdateButtonPadding_Original;

using ExperienceToggleButton_UpdateVisualStates_t = void(WINAPI*)(void*);
ExperienceToggleButton_UpdateVisualStates_t
    ExperienceToggleButton_UpdateVisualStates_Original;

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

int ClampInt(int value, int minValue, int maxValue) {
    return std::min(std::max(value, minValue), maxValue);
}

std::wstring GetStringSetting(PCWSTR name) {
    auto value = WindhawkUtils::StringSetting::make(name);
    return value.get();
}

AppearanceStyle ParseStyle(const std::wstring& value) {
    if (value == L"fallback") {
        return AppearanceStyle::fallback;
    }
    if (value == L"captured") {
        return AppearanceStyle::captured;
    }
    if (value == L"native") {
        return AppearanceStyle::native;
    }
    if (value == L"blur") {
        return AppearanceStyle::blur;
    }
    if (value == L"acrylic") {
        return AppearanceStyle::acrylic;
    }
    return AppearanceStyle::clear;
}

Appearance LoadAppearance(PCWSTR prefix,
                          bool hasEnabled,
                          bool defaultEnabled,
                          AppearanceStyle defaultStyle) {
    WCHAR name[96];

    Appearance appearance{};
    appearance.enabled = defaultEnabled;
    appearance.style = defaultStyle;
    if (hasEnabled) {
        swprintf_s(name, L"%s.enabled", prefix);
        appearance.enabled = Wh_GetIntSetting(name) != 0;
    }

    swprintf_s(name, L"%s.style", prefix);
    const std::wstring style = GetStringSetting(name);
    if (!style.empty()) {
        appearance.style = ParseStyle(style);
    }

    appearance.opacity =
        appearance.style == AppearanceStyle::clear ? 0 : 255;

    return appearance;
}

void LoadSettings() {
    Settings settings{};
    settings.desktop =
        LoadAppearance(L"desktop", false, true, AppearanceStyle::clear);
    settings.fallback =
        LoadAppearance(L"fallback", false, true, AppearanceStyle::captured);
    settings.maximized =
        LoadAppearance(L"maximized", true, true, AppearanceStyle::fallback);
    settings.startOpened =
        LoadAppearance(L"startOpened", true, true, AppearanceStyle::fallback);
    settings.searchOpened =
        LoadAppearance(L"searchOpened", true, true, AppearanceStyle::fallback);
    settings.taskViewOpened =
        LoadAppearance(L"taskViewOpened", true, true, AppearanceStyle::fallback);
    settings.trayFlyoutOpened =
        LoadAppearance(L"trayFlyoutOpened", true, true, AppearanceStyle::fallback);
    settings.otherInteraction =
        LoadAppearance(L"otherInteraction", true, false, AppearanceStyle::fallback);

    settings.animationDurationMs =
        ClampInt(Wh_GetIntSetting(L"animation.durationMs"), 0, 5000);
    settings.fullscreenAsMaximized =
        Wh_GetIntSetting(L"detection.fullscreenAsMaximized") != 0;

    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_settings = settings;
}

template <>
inline constexpr winrt::guid
    winrt::impl::guid_v<winrt::impl::abi_t<wf::IPropertyValue>>{
        winrt::impl::guid_v<wf::IPropertyValue>};

typedef enum MY_D2D1_GAUSSIANBLUR_OPTIMIZATION {
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_SPEED = 0,
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED = 1,
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_QUALITY = 2,
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_FORCE_DWORD = 0xffffffff
} MY_D2D1_GAUSSIANBLUR_OPTIMIZATION;

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
    STDMETHOD(GetNamedPropertyMapping)
    (LPCWSTR name,
     _Out_ UINT* index,
     _Out_ GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) PURE;
    STDMETHOD(GetPropertyCount)(_Out_ UINT* count) PURE;
    STDMETHOD(GetProperty)
    (UINT index, _Outptr_ winrt::impl::abi_t<wf::IPropertyValue>** value) PURE;
    STDMETHOD(GetSource)(UINT index,
                         _Outptr_ IGraphicsEffectSource** source) PURE;
    STDMETHOD(GetSourceCount)(_Out_ UINT* count) PURE;
};

}  // namespace Effects
}  // namespace Graphics
}  // namespace Windows
#ifndef BUILD_WINDOWS
}  // namespace ABI
#endif

template <>
inline constexpr winrt::guid
    winrt::impl::guid_v<awge::IGraphicsEffectD2D1Interop>{
        0x2FC57384,
        0xA068,
        0x44D7,
        {0xA3, 0x31, 0x30, 0x98, 0x2F, 0xCF, 0x71, 0x77}};

struct CompositeEffect
    : winrt::implements<CompositeEffect,
                        wge::IGraphicsEffect,
                        wge::IGraphicsEffectSource,
                        awge::IGraphicsEffectD2D1Interop> {
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) {
            return E_INVALIDARG;
        }
        *id = CLSID_D2D1Composite;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name,
        UINT* index,
        awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!name || !index || !mapping) {
            return E_INVALIDARG;
        }
        if (std::wstring_view(name) == L"Mode") {
            *index = D2D1_COMPOSITE_PROP_MODE;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }
        return E_INVALIDARG;
    }

    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) {
            return E_INVALIDARG;
        }
        *count = 1;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetProperty(
        UINT index,
        winrt::impl::abi_t<wf::IPropertyValue>** value) noexcept override try {
        if (!value) {
            return E_INVALIDARG;
        }
        if (index != D2D1_COMPOSITE_PROP_MODE) {
            return E_BOUNDS;
        }
        *value = wf::PropertyValue::CreateUInt32(static_cast<UINT32>(Mode))
                     .as<winrt::impl::abi_t<wf::IPropertyValue>>()
                     .detach();
        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE GetSource(UINT index,
                                        awge::IGraphicsEffectSource** source)
        noexcept override try {
        if (!source) {
            return E_INVALIDARG;
        }
        winrt::copy_to_abi(Sources.at(index), *reinterpret_cast<void**>(source));
        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) {
            return E_INVALIDARG;
        }
        *count = static_cast<UINT>(Sources.size());
        return S_OK;
    }

    winrt::hstring Name() {
        return name;
    }

    void Name(winrt::hstring value) {
        name = value;
    }

    std::vector<wge::IGraphicsEffectSource> Sources;
    D2D1_COMPOSITE_MODE Mode = D2D1_COMPOSITE_MODE_SOURCE_OVER;

private:
    winrt::hstring name = L"CompositeEffect";
};

struct FloodEffect
    : winrt::implements<FloodEffect,
                        wge::IGraphicsEffect,
                        wge::IGraphicsEffectSource,
                        awge::IGraphicsEffectD2D1Interop> {
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) {
            return E_INVALIDARG;
        }
        *id = CLSID_D2D1Flood;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name,
        UINT* index,
        awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!name || !index || !mapping) {
            return E_INVALIDARG;
        }
        if (std::wstring_view(name) == L"Color") {
            *index = D2D1_FLOOD_PROP_COLOR;
            *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }
        return E_INVALIDARG;
    }

    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) {
            return E_INVALIDARG;
        }
        *count = 1;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetProperty(
        UINT index,
        winrt::impl::abi_t<wf::IPropertyValue>** value) noexcept override try {
        if (!value) {
            return E_INVALIDARG;
        }
        if (index != D2D1_FLOOD_PROP_COLOR) {
            return E_BOUNDS;
        }
        *value = wf::PropertyValue::CreateSingleArray({
                     Color.R / 255.0f,
                     Color.G / 255.0f,
                     Color.B / 255.0f,
                     Color.A / 255.0f,
                 })
                     .as<winrt::impl::abi_t<wf::IPropertyValue>>()
                     .detach();
        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE GetSource(UINT,
                                        awge::IGraphicsEffectSource** source)
        noexcept override {
        if (!source) {
            return E_INVALIDARG;
        }
        return E_BOUNDS;
    }

    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) {
            return E_INVALIDARG;
        }
        *count = 0;
        return S_OK;
    }

    winrt::hstring Name() {
        return name;
    }

    void Name(winrt::hstring value) {
        name = value;
    }

    winrt::Windows::UI::Color Color{};

private:
    winrt::hstring name = L"FloodEffect";
};

struct GaussianBlurEffect
    : winrt::implements<GaussianBlurEffect,
                        wge::IGraphicsEffect,
                        wge::IGraphicsEffectSource,
                        awge::IGraphicsEffectD2D1Interop> {
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) {
            return E_INVALIDARG;
        }
        *id = CLSID_D2D1GaussianBlur;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name,
        UINT* index,
        awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!name || !index || !mapping) {
            return E_INVALIDARG;
        }

        const std::wstring_view nameView(name);
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
        if (!count) {
            return E_INVALIDARG;
        }
        *count = 3;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetProperty(
        UINT index,
        winrt::impl::abi_t<wf::IPropertyValue>** value) noexcept override try {
        if (!value) {
            return E_INVALIDARG;
        }

        switch (index) {
            case D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION:
                *value = wf::PropertyValue::CreateSingle(BlurAmount)
                             .as<winrt::impl::abi_t<wf::IPropertyValue>>()
                             .detach();
                break;
            case D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION:
                *value =
                    wf::PropertyValue::CreateUInt32(
                        static_cast<UINT32>(Optimization))
                        .as<winrt::impl::abi_t<wf::IPropertyValue>>()
                        .detach();
                break;
            case D2D1_GAUSSIANBLUR_PROP_BORDER_MODE:
                *value = wf::PropertyValue::CreateUInt32(
                             static_cast<UINT32>(BorderMode))
                             .as<winrt::impl::abi_t<wf::IPropertyValue>>()
                             .detach();
                break;
            default:
                return E_BOUNDS;
        }

        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE GetSource(UINT index,
                                        awge::IGraphicsEffectSource** source)
        noexcept override {
        if (!source) {
            return E_INVALIDARG;
        }
        if (index != 0) {
            return E_BOUNDS;
        }
        winrt::copy_to_abi(Source, *reinterpret_cast<void**>(source));
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) {
            return E_INVALIDARG;
        }
        *count = 1;
        return S_OK;
    }

    winrt::hstring Name() {
        return name;
    }

    void Name(winrt::hstring value) {
        name = value;
    }

    wge::IGraphicsEffectSource Source{nullptr};
    float BlurAmount = 3.0f;
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION Optimization =
        MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED;
    D2D1_BORDER_MODE BorderMode = D2D1_BORDER_MODE_SOFT;

private:
    winrt::hstring name = L"GaussianBlurEffect";
};

class DynamicXamlBlurBrush
    : public Media::XamlCompositionBrushBaseT<DynamicXamlBlurBrush> {
public:
    DynamicXamlBlurBrush(UIElement element,
                         float blurAmount,
                         winrt::Windows::UI::Color tint)
        : compositor(wuxh::ElementCompositionPreview::GetElementVisual(element)
                         .Compositor()),
          blurAmount(blurAmount),
          tint(tint) {}

    void OnConnected() {
        if (!CompositionBrush()) {
            CompositionBrush(CreateEffectBrush());
        }
    }

    void OnDisconnected() {
        if (const auto brush = CompositionBrush()) {
            brush.Close();
            CompositionBrush(nullptr);
        }
    }

private:
    wuc::CompositionBrush CreateEffectBrush() {
        auto backdropBrush = compositor.CreateBackdropBrush();

        auto blurEffect = winrt::make_self<GaussianBlurEffect>();
        blurEffect->Source = wuc::CompositionEffectSourceParameter(L"backdrop");
        blurEffect->BlurAmount = blurAmount;
        blurEffect->Name(L"BlurEffect");

        auto floodEffect = winrt::make_self<FloodEffect>();
        floodEffect->Color = tint;
        floodEffect->Name(L"FloodEffect");

        auto compositeEffect = winrt::make_self<CompositeEffect>();
        compositeEffect->Mode = D2D1_COMPOSITE_MODE_SOURCE_OVER;
        compositeEffect->Sources.push_back(*blurEffect);
        compositeEffect->Sources.push_back(*floodEffect);

        auto factory = compositor.CreateEffectFactory(*compositeEffect);
        auto brush = factory.CreateBrush();
        brush.SetSourceParameter(L"backdrop", backdropBrush);
        return brush;
    }

    wuc::Compositor compositor{nullptr};
    float blurAmount = 18.0f;
    winrt::Windows::UI::Color tint{};
};

std::wstring ToLower(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(), towlower);
    return value;
}

winrt::Windows::UI::Color ColorFromArgb(BYTE a, BYTE r, BYTE g, BYTE b) {
    return winrt::Windows::UI::Color{a, r, g, b};
}

winrt::Windows::UI::Color GetSystemBackgroundTint(BYTE alpha) {
    try {
        ViewManagement::UISettings uiSettings;
        auto background =
            uiSettings.GetColorValue(ViewManagement::UIColorType::Background);
        return ColorFromArgb(alpha, background.R, background.G, background.B);
    } catch (...) {
        return ColorFromArgb(alpha, 32, 32, 32);
    }
}

Media::Brush MakeNativeTaskbarBrush(UIElement element, BYTE opacity) {
    auto brush = winrt::make<DynamicXamlBlurBrush>(
                     element, 30.0f, GetSystemBackgroundTint(0x70))
                     .as<Media::Brush>();
    brush.Opacity(static_cast<double>(opacity) / 255.0);
    return brush;
}

Media::Brush MakeTranslucentTaskbarBlurBrush(UIElement element) {
    // Same preset used by Windows 11 Taskbar Styler's TranslucentTaskbar
    // theme: <WindhawkBlur BlurAmount="18" TintColor="#25323232"/>.
    return winrt::make<DynamicXamlBlurBrush>(
               element, 18.0f, ColorFromArgb(0x25, 0x32, 0x32, 0x32))
        .as<Media::Brush>();
}

Media::Brush MakeAcrylicLikeBlurBrush(UIElement element) {
    // AcrylicBrush is unreliable on multi-monitor taskbars; use the Styler
    // WindhawkBlur-style implementation with a stronger blur/tint instead.
    return winrt::make<DynamicXamlBlurBrush>(
               element, 30.0f, ColorFromArgb(0x55, 0x32, 0x32, 0x32))
        .as<Media::Brush>();
}

bool IsTransparentBrush(Media::Brush brush) {
    if (!brush) {
        return false;
    }

    try {
        if (brush.Opacity() <= 0.01) {
            return true;
        }

        if (auto solidBrush = brush.try_as<Media::SolidColorBrush>()) {
            return solidBrush.Color().A == 0 || solidBrush.Opacity() <= 0.01;
        }

        if (auto acrylicBrush = brush.try_as<Media::AcrylicBrush>()) {
            return acrylicBrush.TintOpacity() <= 0.01 &&
                   acrylicBrush.FallbackColor().A == 0;
        }
    } catch (...) {
    }

    return false;
}

bool IsBackgroundStroke(Shapes::Rectangle rectangle) {
    try {
        auto frameworkElement = rectangle.try_as<FrameworkElement>();
        return frameworkElement && frameworkElement.Name() == L"BackgroundStroke";
    } catch (...) {
        return false;
    }
}

Controls::Grid GetParentGrid(DependencyObject element) {
    try {
        auto parent = Media::VisualTreeHelper::GetParent(element);
        return parent ? parent.try_as<Controls::Grid>() : nullptr;
    } catch (...) {
        return nullptr;
    }
}

void ClearParentGridBackground(Controls::Grid parentGrid) {
    if (parentGrid) {
        parentGrid.ClearValue(Controls::Panel::BackgroundProperty());
    }
}

void RestoreParentGridBackground(Controls::Grid parentGrid,
                                 Media::Brush originalParentBackground) {
    if (!parentGrid) {
        return;
    }

    if (originalParentBackground) {
        parentGrid.Background(originalParentBackground);
    } else {
        ClearParentGridBackground(parentGrid);
    }
}

bool HasUsableNativeCapture(Media::Brush originalFill,
                            Media::Brush originalParentBackground) {
    return (originalFill && !IsTransparentBrush(originalFill)) ||
           (originalParentBackground &&
            !IsTransparentBrush(originalParentBackground));
}

struct CapturedBackgroundAppearance {
    Media::Brush fill{nullptr};
    Media::Brush parentBackground{nullptr};
    double opacity = 1.0;
};

CapturedBackgroundAppearance CaptureCurrentAppearance(
    Shapes::Rectangle rectangle,
    Controls::Grid parentGrid) {
    CapturedBackgroundAppearance appearance;
    appearance.fill = rectangle.Fill();
    appearance.opacity = rectangle.Opacity();
    if (parentGrid) {
        appearance.parentBackground = parentGrid.Background();
    }
    return appearance;
}

void ApplyCapturedAppearance(Shapes::Rectangle rectangle,
                             Controls::Grid parentGrid,
                             const CapturedBackgroundAppearance& appearance) {
    RestoreParentGridBackground(parentGrid, appearance.parentBackground);

    if (appearance.fill) {
        rectangle.Fill(appearance.fill);
    } else {
        rectangle.ClearValue(Shapes::Shape::FillProperty());
    }

    if (appearance.opacity > 0.01) {
        rectangle.Opacity(appearance.opacity);
    } else {
        rectangle.ClearValue(UIElement::OpacityProperty());
    }
}

CapturedBackgroundAppearance CaptureNativeAppearance(
    Shapes::Rectangle rectangle,
    Controls::Grid parentGrid) {
    CapturedBackgroundAppearance current =
        CaptureCurrentAppearance(rectangle, parentGrid);

    try {
        ClearParentGridBackground(parentGrid);
        rectangle.ClearValue(Shapes::Shape::FillProperty());
        rectangle.ClearValue(UIElement::OpacityProperty());

        CapturedBackgroundAppearance nativeAppearance =
            CaptureCurrentAppearance(rectangle, parentGrid);
        ApplyCapturedAppearance(rectangle, parentGrid, current);
        return nativeAppearance;
    } catch (...) {
        ApplyCapturedAppearance(rectangle, parentGrid, current);
        return {};
    }
}

void ApplyNativeDefaultAppearance(Shapes::Rectangle rectangle,
                                  Controls::Grid parentGrid,
                                  Media::Brush nativeFill,
                                  Media::Brush nativeParentBackground,
                                  double nativeOpacity,
                                  bool refreshStyle,
                                  BYTE animationOpacity) {
    if (HasUsableNativeCapture(nativeFill, nativeParentBackground)) {
        if (refreshStyle) {
            RestoreParentGridBackground(parentGrid, nativeParentBackground);

            if (nativeFill) {
                rectangle.Fill(nativeFill);
            } else {
                rectangle.ClearValue(Shapes::Shape::FillProperty());
            }
        }

        if (animationOpacity >= 255) {
            if (nativeOpacity > 0.01) {
                rectangle.Opacity(nativeOpacity);
            } else {
                rectangle.ClearValue(UIElement::OpacityProperty());
            }
        } else {
            rectangle.Opacity(static_cast<double>(animationOpacity) / 255.0);
        }
        return;
    }

    if (parentGrid) {
        if (refreshStyle || !parentGrid.Background()) {
            parentGrid.Background(MakeNativeTaskbarBrush(parentGrid,
                                                         animationOpacity));
        } else if (auto brush = parentGrid.Background()) {
            brush.Opacity(static_cast<double>(animationOpacity) / 255.0);
        }
    }

    if (refreshStyle) {
        rectangle.ClearValue(Shapes::Shape::FillProperty());
        rectangle.ClearValue(UIElement::OpacityProperty());
    }
}

void RestoreCapturedAppearance(Shapes::Rectangle rectangle,
                               Media::Brush originalFill,
                               Media::Brush originalParentBackground,
                               double originalOpacity,
                               Controls::Grid parentGrid,
                               bool refreshStyle,
                               BYTE animationOpacity) {
    if (refreshStyle) {
        RestoreParentGridBackground(parentGrid, originalParentBackground);

        if (originalFill) {
            rectangle.Fill(originalFill);
        } else {
            rectangle.ClearValue(Shapes::Shape::FillProperty());
        }
    }

    if (animationOpacity >= 255) {
        if (originalOpacity > 0.01) {
            rectangle.Opacity(originalOpacity);
        } else {
            rectangle.ClearValue(UIElement::OpacityProperty());
        }
    } else {
        rectangle.Opacity(static_cast<double>(animationOpacity) / 255.0);
    }
}

void ApplyAppearanceToElement(Shapes::Rectangle rectangle,
                              const Appearance& appearance,
                              Media::Brush originalFill,
                              Media::Brush originalParentBackground,
                              double originalOpacity,
                              Media::Brush nativeFill,
                              Media::Brush nativeParentBackground,
                              double nativeOpacity,
                              winrt::weak_ref<Controls::Grid> parentGridRef,
                              bool refreshStyle,
                              bool restoreOriginal) {
    try {
        if (!rectangle) {
            return;
        }

        const bool isBackgroundStroke = IsBackgroundStroke(rectangle);
        Controls::Grid parentGrid = parentGridRef.get();
        if (!parentGrid) {
            parentGrid = GetParentGrid(rectangle);
        }

        if (restoreOriginal) {
            if (HasUsableNativeCapture(originalFill,
                                       originalParentBackground)) {
                RestoreCapturedAppearance(rectangle, originalFill,
                                          originalParentBackground,
                                          originalOpacity, parentGrid,
                                          refreshStyle, 255);
            } else {
                ApplyNativeDefaultAppearance(rectangle, parentGrid,
                                             nativeFill,
                                             nativeParentBackground,
                                             nativeOpacity, refreshStyle, 255);
            }
            return;
        }

        if (appearance.style == AppearanceStyle::captured) {
            RestoreCapturedAppearance(rectangle, originalFill,
                                      originalParentBackground, originalOpacity,
                                      parentGrid, refreshStyle,
                                      appearance.opacity);
            return;
        } else if (appearance.style == AppearanceStyle::native) {
            ApplyNativeDefaultAppearance(rectangle, parentGrid, nativeFill,
                                         nativeParentBackground, nativeOpacity,
                                         refreshStyle,
                                         appearance.opacity);
            return;
        } else if (appearance.style == AppearanceStyle::clear) {
            if (refreshStyle) {
                ClearParentGridBackground(parentGrid);
            }
        } else if (appearance.style == AppearanceStyle::blur) {
            if (refreshStyle) {
                ClearParentGridBackground(parentGrid);
            }
            if (refreshStyle && !isBackgroundStroke) {
                rectangle.Fill(MakeTranslucentTaskbarBlurBrush(rectangle));
            }
        } else if (appearance.style == AppearanceStyle::acrylic) {
            if (refreshStyle) {
                ClearParentGridBackground(parentGrid);
            }
            if (refreshStyle && !isBackgroundStroke) {
                rectangle.Fill(MakeAcrylicLikeBlurBrush(rectangle));
            }
        }

        const bool hideStroke =
            isBackgroundStroke &&
            (appearance.style == AppearanceStyle::clear ||
             appearance.style == AppearanceStyle::blur ||
             appearance.style == AppearanceStyle::acrylic);
        rectangle.Opacity(hideStroke ? 0.0
                                     : static_cast<double>(appearance.opacity) /
                                           255.0);
    } catch (const winrt::hresult_error& ex) {
        Wh_Log(L"Failed to apply taskbar background: %08X %s", ex.code(),
               ex.message().c_str());
    } catch (...) {
        Wh_Log(L"Failed to apply taskbar background");
    }
}

TrackedBackgroundElement GetOriginalElementInfo(void* abi) {
    std::lock_guard<std::mutex> lock(g_elementsMutex);
    for (const auto& tracked : BackgroundElements()) {
        if (tracked.abi == abi) {
            return tracked;
        }
    }

    return {};
}

void StoreAppliedAppearance(void* abi,
                            AppearanceStyle style,
                            bool restoreOriginal) {
    std::lock_guard<std::mutex> lock(g_elementsMutex);
    for (auto& tracked : BackgroundElements()) {
        if (tracked.abi == abi) {
            if (restoreOriginal) {
                tracked.hasAppliedAppearance = false;
            } else {
                tracked.hasAppliedAppearance = true;
                tracked.appliedStyle = style;
            }
            return;
        }
    }
}

struct BackgroundElementSnapshot {
    winrt::weak_ref<Shapes::Rectangle> element;
    void* abi = nullptr;
};

void StoreActiveAppearance(const Appearance& appearance, bool restoreOriginal) {
    std::lock_guard<std::mutex> lock(g_activeAppearanceMutex);
    g_activeAppearance.hasValue = true;
    g_activeAppearance.appearance = appearance;
    g_activeAppearance.restoreOriginal = restoreOriginal;
}

ActiveAppearance GetActiveAppearance() {
    std::lock_guard<std::mutex> lock(g_activeAppearanceMutex);
    return g_activeAppearance;
}

void DispatchApplyAppearance(const Appearance& appearance,
                             bool restoreOriginal,
                             bool waitForCompletion = false) {
    std::vector<BackgroundElementSnapshot> snapshot;
    {
        std::lock_guard<std::mutex> lock(g_elementsMutex);
        auto& elements = BackgroundElements();
        std::erase_if(elements, [](const auto& tracked) {
            return !tracked.element.get();
        });
        snapshot.reserve(elements.size());
        for (const auto& tracked : elements) {
            snapshot.push_back({tracked.element, tracked.abi});
        }
    }

    for (auto& item : snapshot) {
        auto rectangle = item.element.get();
        if (!rectangle) {
            continue;
        }

        auto dispatcher = rectangle.Dispatcher();
        if (!dispatcher) {
            continue;
        }

        HANDLE completionEvent =
            waitForCompletion ? CreateEventW(nullptr, TRUE, FALSE, nullptr)
                              : nullptr;

        auto apply = [weakElement = item.element, abi = item.abi, appearance,
                      restoreOriginal, completionEvent]() {
            if (auto rectangle = weakElement.get()) {
                TrackedBackgroundElement originalInfo =
                    GetOriginalElementInfo(abi);
                const bool refreshStyle =
                    restoreOriginal ||
                    !originalInfo.hasAppliedAppearance ||
                    originalInfo.appliedStyle != appearance.style;
                ApplyAppearanceToElement(rectangle, appearance,
                                         originalInfo.originalFill,
                                         originalInfo.originalParentBackground,
                                         originalInfo.originalOpacity,
                                         originalInfo.nativeFill,
                                         originalInfo.nativeParentBackground,
                                         originalInfo.nativeOpacity,
                                         originalInfo.parentGrid,
                                         refreshStyle,
                                         restoreOriginal);
                StoreAppliedAppearance(abi, appearance.style, restoreOriginal);
            }

            if (completionEvent) {
                SetEvent(completionEvent);
            }
        };

        if (dispatcher.HasThreadAccess()) {
            apply();
        } else {
            try {
                dispatcher.RunAsync(Core::CoreDispatcherPriority::Normal,
                                    apply);
            } catch (...) {
                if (completionEvent) {
                    SetEvent(completionEvent);
                }
            }
        }

        if (completionEvent) {
            WaitForSingleObject(completionEvent, 500);
            CloseHandle(completionEvent);
        }
    }
}

void ReleaseTrackedBrushesForElement(void* abi) {
    std::lock_guard<std::mutex> lock(g_elementsMutex);
    for (auto& tracked : BackgroundElements()) {
        if (tracked.abi == abi) {
            tracked.originalFill = nullptr;
            tracked.originalParentBackground = nullptr;
            tracked.nativeFill = nullptr;
            tracked.nativeParentBackground = nullptr;
            tracked.hasAppliedAppearance = false;
            return;
        }
    }
}

void ReleaseTrackedBackgroundElements() {
    std::vector<BackgroundElementSnapshot> snapshot;
    {
        std::lock_guard<std::mutex> lock(g_elementsMutex);
        auto& elements = BackgroundElements();
        snapshot.reserve(elements.size());
        for (const auto& tracked : elements) {
            snapshot.push_back({tracked.element, tracked.abi});
        }
    }

    std::vector<void*> releasedAbis;
    releasedAbis.reserve(snapshot.size());

    for (const auto& item : snapshot) {
        auto rectangle = item.element.get();
        if (!rectangle) {
            releasedAbis.push_back(item.abi);
            continue;
        }

        auto dispatcher = rectangle.Dispatcher();
        if (!dispatcher) {
            continue;
        }

        if (dispatcher.HasThreadAccess()) {
            ReleaseTrackedBrushesForElement(item.abi);
            releasedAbis.push_back(item.abi);
            continue;
        }

        HANDLE completionEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!completionEvent) {
            continue;
        }

        auto release = [abi = item.abi, completionEvent]() {
            ReleaseTrackedBrushesForElement(abi);
            SetEvent(completionEvent);
        };

        try {
            dispatcher.RunAsync(Core::CoreDispatcherPriority::Normal, release);
        } catch (...) {
            SetEvent(completionEvent);
        }

        DWORD waitResult = WaitForSingleObject(completionEvent, 1000);
        if (waitResult == WAIT_OBJECT_0) {
            releasedAbis.push_back(item.abi);
        }
        CloseHandle(completionEvent);
    }

    std::lock_guard<std::mutex> lock(g_elementsMutex);
    auto& elements = BackgroundElements();
    std::erase_if(elements, [&releasedAbis](const auto& tracked) {
        return std::find(releasedAbis.begin(), releasedAbis.end(),
                         tracked.abi) != releasedAbis.end();
    });
}

bool HasAncestorClass(DependencyObject element, std::wstring_view className) {
    DependencyObject current = element;
    while (current) {
        if (winrt::get_class_name(current) == className) {
            return true;
        }
        current = Media::VisualTreeHelper::GetParent(current);
    }

    return false;
}

bool IsTaskbarBackgroundRectangle(FrameworkElement element) {
    if (!element) {
        return false;
    }

    if (winrt::get_class_name(element) !=
        L"Windows.UI.Xaml.Shapes.Rectangle") {
        return false;
    }

    auto name = element.Name();
    if (name != L"BackgroundFill" && name != L"BackgroundStroke") {
        return false;
    }

    return HasAncestorClass(element, L"Taskbar.TaskbarBackground") &&
           HasAncestorClass(element, L"Taskbar.TaskbarFrame");
}

void RegisterBackgroundElement(FrameworkElement element) {
    if (!IsTaskbarBackgroundRectangle(element)) {
        return;
    }

    auto rectangle = element.try_as<Shapes::Rectangle>();
    if (!rectangle) {
        return;
    }

    void* abi = winrt::get_abi(element);
    bool added = false;
    {
        std::lock_guard<std::mutex> lock(g_elementsMutex);
        auto& elements = BackgroundElements();
        std::erase_if(elements, [](const auto& tracked) {
            return !tracked.element.get();
        });

        auto it = std::find_if(
            elements.begin(), elements.end(),
            [abi](const auto& tracked) { return tracked.abi == abi; });

        if (it == elements.end()) {
            auto parentGrid = GetParentGrid(rectangle);
            TrackedBackgroundElement tracked;
            tracked.element = winrt::make_weak(rectangle);
            if (parentGrid) {
                tracked.parentGrid = winrt::make_weak(parentGrid);
                tracked.originalParentBackground = parentGrid.Background();
            }
            tracked.originalFill = rectangle.Fill();
            tracked.originalOpacity = rectangle.Opacity();
            CapturedBackgroundAppearance nativeAppearance =
                CaptureNativeAppearance(rectangle, parentGrid);
            tracked.nativeFill = nativeAppearance.fill;
            tracked.nativeParentBackground =
                nativeAppearance.parentBackground;
            tracked.nativeOpacity = nativeAppearance.opacity;
            tracked.abi = abi;
            elements.push_back(std::move(tracked));
            added = true;
        }
    }

    if (added) {
        Wh_Log(L"Found taskbar background element: %s#%s",
               winrt::get_class_name(element).c_str(),
               element.Name().c_str());

        const ActiveAppearance activeAppearance = GetActiveAppearance();
        if (activeAppearance.hasValue) {
            TrackedBackgroundElement originalInfo =
                GetOriginalElementInfo(abi);
            const bool refreshStyle =
                activeAppearance.restoreOriginal ||
                !originalInfo.hasAppliedAppearance ||
                originalInfo.appliedStyle !=
                    activeAppearance.appearance.style;
            ApplyAppearanceToElement(rectangle, activeAppearance.appearance,
                                     originalInfo.originalFill,
                                     originalInfo.originalParentBackground,
                                     originalInfo.originalOpacity,
                                     originalInfo.nativeFill,
                                     originalInfo.nativeParentBackground,
                                     originalInfo.nativeOpacity,
                                     originalInfo.parentGrid,
                                     refreshStyle,
                                     activeAppearance.restoreOriginal);
            StoreAppliedAppearance(abi, activeAppearance.appearance.style,
                                   activeAppearance.restoreOriginal);
        }
    }
}

void ScanTaskbarBackgroundsRecursive(FrameworkElement element, int depth = 32) {
    if (!element || depth <= 0) {
        return;
    }

    RegisterBackgroundElement(element);

    int childrenCount = 0;
    try {
        childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    } catch (...) {
        return;
    }

    for (int i = 0; i < childrenCount; i++) {
        auto child =
            Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>();
        if (child) {
            ScanTaskbarBackgroundsRecursive(child, depth - 1);
        }
    }
}

FrameworkElement GetFrameworkElementFromNative(void* pThis) {
    try {
        void* iUnknownPtr = (void**)pThis + 3;
        winrt::Windows::Foundation::IUnknown iUnknown;
        winrt::copy_from_abi(iUnknown, iUnknownPtr);
        return iUnknown.try_as<FrameworkElement>();
    } catch (...) {
        return nullptr;
    }
}

void QueueScanFromElement(void* pThis) {
    if (g_scanPending.exchange(true)) {
        return;
    }

    try {
        FrameworkElement element = GetFrameworkElementFromNative(pThis);
        if (!element) {
            g_scanPending = false;
            return;
        }

        auto dispatcher = element.Dispatcher();
        if (!dispatcher) {
            g_scanPending = false;
            return;
        }

        auto weakElement = winrt::make_weak(element);
        dispatcher.RunAsync(Core::CoreDispatcherPriority::Low,
                            [weakElement]() {
            try {
                auto element = weakElement.get();
                if (element) {
                    ScanTaskbarBackgroundsRecursive(element);

                    FrameworkElement current = element;
                    while (current) {
                        if (winrt::get_class_name(current) ==
                            L"Taskbar.TaskbarFrame") {
                            ScanTaskbarBackgroundsRecursive(current);
                            break;
                        }

                        auto parent = Media::VisualTreeHelper::GetParent(current);
                        current =
                            parent ? parent.try_as<FrameworkElement>() : nullptr;
                    }
                }
            } catch (...) {
            }
            g_scanPending = false;
        });
    } catch (...) {
        g_scanPending = false;
    }
}

bool IsWindowCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    HRESULT hr = DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                       sizeof(cloaked));
    return SUCCEEDED(hr) && cloaked != 0;
}

bool HasUsableRect(HWND hwnd, RECT* rect) {
    RECT localRect{};
    if (!GetWindowRect(hwnd, &localRect)) {
        return false;
    }

    if (localRect.right <= localRect.left || localRect.bottom <= localRect.top) {
        return false;
    }

    if (rect) {
        *rect = localRect;
    }

    return true;
}

bool IsTaskbarClass(PCWSTR className) {
    return _wcsicmp(className, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0;
}

bool IsDesktopClass(PCWSTR className) {
    return _wcsicmp(className, L"Progman") == 0 ||
           _wcsicmp(className, L"WorkerW") == 0;
}

bool IsFileExplorerClass(PCWSTR className) {
    return _wcsicmp(className, L"CabinetWClass") == 0 ||
           _wcsicmp(className, L"ExploreWClass") == 0;
}

bool IsIgnoredShellWindowClass(PCWSTR className) {
    return IsTaskbarClass(className) || IsDesktopClass(className) ||
           _wcsicmp(className, L"XamlExplorerHostIslandWindow") == 0 ||
           _wcsicmp(className, L"XamlExplorerHostIslandWindow_WASDK") == 0 ||
           _wcsicmp(className,
                    L"Windows.UI.Composition.DesktopWindowContentBridge") == 0 ||
           _wcsicmp(className, L"Windows.UI.Core.CoreWindow") == 0 ||
           _wcsicmp(className,
                    L"Microsoft.UI.Content.PopupWindowSiteBridge") == 0 ||
           _wcsicmp(className, L"Windows.UI.Input.InputSite.WindowClass") == 0 ||
           _wcsicmp(className, L"NotifyIconOverflowWindow") == 0 ||
           _wcsicmp(className, L"TaskListThumbnailWnd") == 0 ||
           _wcsicmp(className, L"MultitaskingViewFrame") == 0 ||
           _wcsicmp(className, L"SysShadow") == 0 ||
           _wcsicmp(className, L"#32768") == 0;
}

std::wstring FileNameFromPath(std::wstring path) {
    size_t slash = path.find_last_of(L"\\/");
    if (slash != std::wstring::npos) {
        path.erase(0, slash + 1);
    }

    return ToLower(path);
}

std::wstring GetProcessFileName(HWND hwnd) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    if (!processId) {
        return L"";
    }

    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!process) {
        return L"";
    }

    WCHAR path[MAX_PATH * 4]{};
    DWORD pathSize = ARRAYSIZE(path);
    std::wstring result;
    if (QueryFullProcessImageNameW(process, 0, path, &pathSize)) {
        result = FileNameFromPath(path);
    }

    CloseHandle(process);
    return result;
}

bool IsShellHostProcess(const std::wstring& processName) {
    return processName == L"startmenuexperiencehost.exe" ||
           processName == L"searchhost.exe" ||
           processName == L"shellexperiencehost.exe" ||
           processName == L"shellhost.exe" ||
           processName == L"textinputhost.exe";
}

std::wstring GetWindowClassName(HWND hwnd) {
    WCHAR className[128]{};
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    return className;
}

bool IsVisibleShellSurface(HWND hwnd, const RECT& rect) {
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd) || IsWindowCloaked(hwnd)) {
        return false;
    }

    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;
    return width >= 60 && height >= 60;
}

bool LooksLikeShellFlyout(HWND hwnd, const RECT& rect) {
    if (!IsVisibleShellSurface(hwnd, rect)) {
        return false;
    }

    WCHAR className[128]{};
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    if (IsTaskbarClass(className) || IsDesktopClass(className)) {
        return false;
    }

    const std::wstring processName = GetProcessFileName(hwnd);
    return IsShellHostProcess(processName);
}

struct EnumShellActivityContext {
    ShellActivity activity;
};

BOOL CALLBACK EnumShellActivityProc(HWND hwnd, LPARAM lParam) {
    RECT rect{};
    if (!HasUsableRect(hwnd, &rect) || !IsVisibleShellSurface(hwnd, rect)) {
        return TRUE;
    }

    auto* context = reinterpret_cast<EnumShellActivityContext*>(lParam);
    const std::wstring className = ToLower(GetWindowClassName(hwnd));
    const std::wstring processName = GetProcessFileName(hwnd);

    if (className.find(L"multitasking") != std::wstring::npos) {
        context->activity.taskViewOpened = true;
        return TRUE;
    }

    if (className.find(L"notifyiconoverflow") != std::wstring::npos ||
        className.find(L"clockflyout") != std::wstring::npos ||
        className.find(L"shellflyout") != std::wstring::npos) {
        context->activity.trayFlyoutOpened = true;
        return TRUE;
    }

    if (!LooksLikeShellFlyout(hwnd, rect)) {
        return TRUE;
    }

    if (processName == L"startmenuexperiencehost.exe") {
        context->activity.startOpened = true;
    } else if (processName == L"searchhost.exe") {
        context->activity.searchOpened = true;
    } else if (processName == L"shellexperiencehost.exe" ||
               processName == L"shellhost.exe") {
        context->activity.trayFlyoutOpened = true;
    } else {
        context->activity.otherInteraction = true;
    }

    return TRUE;
}

ShellActivity DetectShellActivity() {
    EnumShellActivityContext context{};
    EnumWindows(EnumShellActivityProc, reinterpret_cast<LPARAM>(&context));

    return context.activity;
}

struct TaskbarWindow {
    HWND hwnd;
    HMONITOR monitor;
    RECT rect;
};

struct EnumTaskbarsContext {
    std::vector<TaskbarWindow>* taskbars;
    DWORD processId;
};

BOOL CALLBACK EnumTaskbarsProc(HWND hwnd, LPARAM lParam) {
    WCHAR className[64]{};
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className)) ||
        !IsTaskbarClass(className)) {
        return TRUE;
    }

    auto* context = reinterpret_cast<EnumTaskbarsContext*>(lParam);
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId != context->processId) {
        return TRUE;
    }

    RECT rect{};
    if (!HasUsableRect(hwnd, &rect)) {
        return TRUE;
    }

    context->taskbars->push_back(
        {hwnd, MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), rect});
    return TRUE;
}

std::vector<TaskbarWindow> FindTaskbars() {
    std::vector<TaskbarWindow> taskbars;
    EnumTaskbarsContext context{&taskbars, GetCurrentProcessId()};
    EnumWindows(EnumTaskbarsProc, reinterpret_cast<LPARAM>(&context));
    return taskbars;
}

bool RectCoversMonitor(HWND hwnd, HMONITOR monitor) {
    RECT rect{};
    if (!HasUsableRect(hwnd, &rect)) {
        return false;
    }

    MONITORINFO monitorInfo{.cbSize = sizeof(monitorInfo)};
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        return false;
    }

    const RECT& m = monitorInfo.rcMonitor;
    return rect.left <= m.left + 2 && rect.top <= m.top + 2 &&
           rect.right >= m.right - 2 && rect.bottom >= m.bottom - 2;
}

bool IsUserCandidateWindow(HWND hwnd) {
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd) || IsWindowCloaked(hwnd)) {
        return false;
    }

    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (root != hwnd) {
        return false;
    }

    WCHAR className[128]{};
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    if (IsIgnoredShellWindowClass(className)) {
        return false;
    }

    const std::wstring processName = GetProcessFileName(hwnd);
    if (IsShellHostProcess(processName)) {
        return false;
    }

    if (processName == L"explorer.exe" && !IsFileExplorerClass(className)) {
        return false;
    }

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) {
        return false;
    }

    RECT rect{};
    if (!HasUsableRect(hwnd, &rect)) {
        return false;
    }

    return (rect.right - rect.left) >= 120 && (rect.bottom - rect.top) >= 80;
}

struct EnumMaximizedContext {
    bool fullscreenAsMaximized;
    const std::vector<TaskbarWindow>* taskbars;
    bool found;
};

bool MonitorMatchesTaskbar(HMONITOR monitor,
                           const std::vector<TaskbarWindow>& taskbars) {
    if (!monitor || taskbars.empty()) {
        return true;
    }

    for (const auto& taskbar : taskbars) {
        if (taskbar.monitor == monitor) {
            return true;
        }
    }

    return false;
}

BOOL CALLBACK EnumMaximizedProc(HWND hwnd, LPARAM lParam) {
    auto* context = reinterpret_cast<EnumMaximizedContext*>(lParam);
    if (context->found || !IsUserCandidateWindow(hwnd)) {
        return TRUE;
    }

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
    if (!MonitorMatchesTaskbar(monitor, *context->taskbars)) {
        return TRUE;
    }

    if (IsZoomed(hwnd) ||
        (context->fullscreenAsMaximized &&
         monitor && RectCoversMonitor(hwnd, monitor))) {
        context->found = true;
        return FALSE;
    }

    return TRUE;
}

bool HasMaximizedWindow(bool fullscreenAsMaximized,
                        const std::vector<TaskbarWindow>& taskbars) {
    EnumMaximizedContext context{fullscreenAsMaximized, &taskbars, false};
    EnumWindows(EnumMaximizedProc, reinterpret_cast<LPARAM>(&context));
    return context.found;
}

struct StateResolution {
    TaskbarDynamicState state;
    PCWSTR reason;
    bool hasMaximizedWindow;
};

Appearance GetAppearanceForState(const Settings& settings,
                                 TaskbarDynamicState state) {
    Appearance appearance = settings.desktop;

    switch (state) {
        case TaskbarDynamicState::maximized:
            if (settings.maximized.enabled) {
                appearance = settings.maximized;
            }
            break;

        case TaskbarDynamicState::startOpened:
            if (settings.startOpened.enabled) {
                appearance = settings.startOpened;
            }
            break;

        case TaskbarDynamicState::searchOpened:
            if (settings.searchOpened.enabled) {
                appearance = settings.searchOpened;
            }
            break;

        case TaskbarDynamicState::taskViewOpened:
            if (settings.taskViewOpened.enabled) {
                appearance = settings.taskViewOpened;
            }
            break;

        case TaskbarDynamicState::trayFlyoutOpened:
            if (settings.trayFlyoutOpened.enabled) {
                appearance = settings.trayFlyoutOpened;
            }
            break;

        case TaskbarDynamicState::otherInteraction:
            if (settings.otherInteraction.enabled) {
                appearance = settings.otherInteraction;
            }
            break;

        case TaskbarDynamicState::desktop:
            break;
    }

    if (appearance.style == AppearanceStyle::fallback) {
        appearance.style = settings.fallback.style;
    }

    if (appearance.style == AppearanceStyle::fallback) {
        appearance.style = AppearanceStyle::captured;
    }

    appearance.opacity =
        appearance.style == AppearanceStyle::clear ? 0 : 255;

    return appearance;
}

StateResolution ResolveState(const Settings& settings,
                             const ShellActivity& shellActivity,
                             const std::vector<TaskbarWindow>& taskbars) {
    const bool hasMaximizedWindow =
        HasMaximizedWindow(settings.fullscreenAsMaximized, taskbars);

    if (shellActivity.taskViewOpened) {
        return {TaskbarDynamicState::taskViewOpened, L"taskViewOpened",
                hasMaximizedWindow};
    }

    if (shellActivity.startOpened) {
        return {TaskbarDynamicState::startOpened, L"startOpened",
                hasMaximizedWindow};
    }

    if (shellActivity.searchOpened) {
        return {TaskbarDynamicState::searchOpened, L"searchOpened",
                hasMaximizedWindow};
    }

    if (shellActivity.trayFlyoutOpened) {
        return {TaskbarDynamicState::trayFlyoutOpened, L"trayFlyoutOpened",
                hasMaximizedWindow};
    }

    if (hasMaximizedWindow) {
        return {TaskbarDynamicState::maximized, L"maximizedWindow",
                hasMaximizedWindow};
    }

    if (shellActivity.otherInteraction) {
        return {TaskbarDynamicState::otherInteraction, L"otherShellInteraction",
                hasMaximizedWindow};
    }

    return {TaskbarDynamicState::desktop, L"desktop", hasMaximizedWindow};
}

PCWSTR StateName(TaskbarDynamicState state) {
    switch (state) {
        case TaskbarDynamicState::desktop:
            return L"desktop";
        case TaskbarDynamicState::maximized:
            return L"maximized";
        case TaskbarDynamicState::startOpened:
            return L"startOpened";
        case TaskbarDynamicState::searchOpened:
            return L"searchOpened";
        case TaskbarDynamicState::taskViewOpened:
            return L"taskViewOpened";
        case TaskbarDynamicState::trayFlyoutOpened:
            return L"trayFlyoutOpened";
        case TaskbarDynamicState::otherInteraction:
            return L"otherInteraction";
    }

    return L"unknown";
}

bool AppearanceSameTarget(const Appearance& left, const Appearance& right) {
    return left.style == right.style && left.opacity == right.opacity;
}

double EaseOutCubic(double progress) {
    progress = std::clamp(progress, 0.0, 1.0);
    return 1.0 - std::pow(1.0 - progress, 3.0);
}

BYTE InterpolateOpacity(BYTE from, BYTE to, double progress) {
    return static_cast<BYTE>(
        std::lround(from + (to - from) * EaseOutCubic(progress)));
}

void RequestStateDetection() {
    g_detectionPending = true;
    g_detectionCondition.notify_one();
}

void CALLBACK WinEventProc(HWINEVENTHOOK,
                           DWORD event,
                           HWND hwnd,
                           LONG idObject,
                           LONG,
                           DWORD,
                           DWORD) {
    if (event != EVENT_SYSTEM_FOREGROUND && idObject != OBJID_WINDOW) {
        return;
    }

    if (event != EVENT_SYSTEM_FOREGROUND && !hwnd) {
        return;
    }

    RequestStateDetection();
}

void WinEventThreadProc() {
    g_winEventThreadId = GetCurrentThreadId();

    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    HWINEVENTHOOK objectEventHook =
        SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_HIDE, nullptr,
                        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!objectEventHook) {
        Wh_Log(L"Failed to hook window object events");
    }

    HWINEVENTHOOK locationEventHook =
        SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE,
                        EVENT_OBJECT_LOCATIONCHANGE, nullptr, WinEventProc, 0,
                        0, WINEVENT_OUTOFCONTEXT);
    if (!locationEventHook) {
        Wh_Log(L"Failed to hook window location events");
    }

    HWINEVENTHOOK cloakEventHook =
        SetWinEventHook(EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED, nullptr,
                        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!cloakEventHook) {
        Wh_Log(L"Failed to hook window cloak events");
    }

    HWINEVENTHOOK foregroundEventHook =
        SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                        nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!foregroundEventHook) {
        Wh_Log(L"Failed to hook foreground events");
    }

    RequestStateDetection();

    BOOL result;
    while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0) {
        if (result == -1) {
            break;
        }

        if (msg.hwnd == nullptr && msg.message == WM_APP) {
            break;
        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (objectEventHook) {
        UnhookWinEvent(objectEventHook);
    }
    if (locationEventHook) {
        UnhookWinEvent(locationEventHook);
    }
    if (cloakEventHook) {
        UnhookWinEvent(cloakEventHook);
    }
    if (foregroundEventHook) {
        UnhookWinEvent(foregroundEventHook);
    }

    g_winEventThreadId = 0;
}

void StartWinEventThread() {
    if (g_winEventThread) {
        return;
    }

    g_winEventThread = new std::thread(WinEventThreadProc);
    for (int i = 0; i < 100 && g_winEventThreadId == 0; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void StopWinEventThread() {
    if (!g_winEventThread) {
        return;
    }

    DWORD threadId = g_winEventThreadId;
    if (threadId) {
        PostThreadMessageW(threadId, WM_APP, 0, 0);
    }

    if (g_winEventThread->joinable()) {
        g_winEventThread->join();
    }
    delete g_winEventThread;
    g_winEventThread = nullptr;
    g_winEventThreadId = 0;
}

void WorkerLoop() {
    Appearance currentAppearance{};
    Appearance animationStartAppearance{};
    Appearance targetAppearance{};
    Appearance lastAppliedAppearance{};
    bool hasCurrentAppearance = false;
    bool hasLastAppliedAppearance = false;
    bool loggedNoElements = false;
    const ULONGLONG workerStartTick = GetTickCount64();
    ULONGLONG animationStartTick = GetTickCount64();
    ShellActivity shellActivity{};
    StateResolution resolution{TaskbarDynamicState::desktop, L"desktop",
                               false};
    constexpr int frameTimeMs = 1000 / 60;

    while (!g_stopWorker) {
        Settings settings{};
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            settings = g_settings;
        }

        const ULONGLONG now = GetTickCount64();

        if (!hasCurrentAppearance || g_detectionPending.exchange(false)) {
            const auto taskbars = FindTaskbars();
            shellActivity = DetectShellActivity();
            resolution = ResolveState(settings, shellActivity, taskbars);

            const TaskbarDynamicState state = resolution.state;
            if (!g_hasLoggedState || g_lastLoggedState != state) {
                Wh_Log(L"Taskbar state: %s (%s; taskView=%d start=%d "
                       L"search=%d tray=%d max=%d other=%d)",
                       StateName(state), resolution.reason,
                       shellActivity.taskViewOpened, shellActivity.startOpened,
                       shellActivity.searchOpened,
                       shellActivity.trayFlyoutOpened,
                       resolution.hasMaximizedWindow,
                       shellActivity.otherInteraction);
                g_lastLoggedState = state;
                g_hasLoggedState = true;
            }

            Appearance desiredAppearance =
                GetAppearanceForState(settings, state);

            if (!hasCurrentAppearance) {
                currentAppearance = desiredAppearance;
                animationStartAppearance = desiredAppearance;
                targetAppearance = desiredAppearance;
                hasCurrentAppearance = true;
                animationStartTick = now;
            }

            if (!AppearanceSameTarget(targetAppearance, desiredAppearance)) {
                animationStartAppearance = currentAppearance;
                targetAppearance = desiredAppearance;
                animationStartTick = now;
            }
        }

        if (!loggedNoElements && now - workerStartTick > 5000) {
            size_t elementCount = 0;
            {
                std::lock_guard<std::mutex> lock(g_elementsMutex);
                elementCount = BackgroundElements().size();
            }

            if (elementCount == 0) {
                Wh_Log(L"No taskbar XAML background elements discovered yet");
            }

            loggedNoElements = true;
        }

        double progress = 1.0;
        if (settings.animationDurationMs > 0) {
            progress =
                static_cast<double>(now - animationStartTick) /
                static_cast<double>(settings.animationDurationMs);
            progress = std::clamp(progress, 0.0, 1.0);
        }

        const bool visibleStyleChange =
            settings.animationDurationMs > 0 &&
            animationStartAppearance.style != targetAppearance.style &&
            animationStartAppearance.opacity > 0 && targetAppearance.opacity > 0;

        if (visibleStyleChange && progress < 0.5) {
            currentAppearance = animationStartAppearance;
            const BYTE floorOpacity =
                std::min<BYTE>(animationStartAppearance.opacity, 245);
            currentAppearance.opacity =
                InterpolateOpacity(animationStartAppearance.opacity,
                                   floorOpacity,
                                   progress * 2.0);
        } else if (visibleStyleChange) {
            currentAppearance = targetAppearance;
            const BYTE floorOpacity =
                std::min<BYTE>(targetAppearance.opacity, 245);
            currentAppearance.opacity =
                InterpolateOpacity(floorOpacity, targetAppearance.opacity,
                                   (progress - 0.5) * 2.0);
        } else {
            currentAppearance = targetAppearance;
            currentAppearance.opacity =
                InterpolateOpacity(animationStartAppearance.opacity,
                                   targetAppearance.opacity, progress);
        }

        const bool animationDone = progress >= 1.0;
        if (!hasLastAppliedAppearance ||
            !AppearanceSameTarget(lastAppliedAppearance, currentAppearance)) {
            StoreActiveAppearance(currentAppearance, false);
            DispatchApplyAppearance(currentAppearance, false);
            lastAppliedAppearance = currentAppearance;
            hasLastAppliedAppearance = true;
        }

        if (animationDone) {
            std::unique_lock<std::mutex> lock(g_detectionMutex);
            g_detectionCondition.wait(lock, [] {
                return g_stopWorker || g_detectionPending.load();
            });
        } else {
            std::unique_lock<std::mutex> lock(g_detectionMutex);
            g_detectionCondition.wait_for(
                lock, std::chrono::milliseconds(frameTimeMs), [] {
                    return g_stopWorker || g_detectionPending.load();
                });
        }
    }

    DispatchApplyAppearance(currentAppearance, true, true);
    {
        std::lock_guard<std::mutex> lock(g_activeAppearanceMutex);
        g_activeAppearance = {};
    }
}

void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    TaskListButton_UpdateVisualStates_Original(pThis);
    QueueScanFromElement(pThis);
}

void WINAPI TaskListButton_UpdateButtonPadding_Hook(void* pThis) {
    TaskListButton_UpdateButtonPadding_Original(pThis);
    QueueScanFromElement(pThis);
}

void WINAPI ExperienceToggleButton_UpdateVisualStates_Hook(void* pThis) {
    ExperienceToggleButton_UpdateVisualStates_Original(pThis);
    QueueScanFromElement(pThis);
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    return module ? module : GetModuleHandleW(L"ExplorerExtensions.dll");
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void))"},
            &TaskListButton_UpdateButtonPadding_Original,
            TaskListButton_UpdateButtonPadding_Hook,
        },
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateVisualStates(void))"},
            &ExperienceToggleButton_UpdateVisualStates_Original,
            ExperienceToggleButton_UpdateVisualStates_Hook,
            true,
        },
    };

    if (!HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        Wh_Log(L"Failed to hook Taskbar.View.dll symbols");
        return false;
    }

    return true;
}

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);

    if (module && !g_taskbarViewDllLoaded &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Taskbar View DLL loaded: %s", lpLibFileName);
        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Dynamic Taskbar Transparency");
    LoadSettings();

    g_stopWorker = false;
    g_taskbarViewDllLoaded = false;
    g_scanPending = false;
    g_hasLoggedState = false;

    if (HMODULE module = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(module)) {
            return FALSE;
        }
    } else {
        HMODULE kernelbase = GetModuleHandleW(L"kernelbase.dll");
        if (!kernelbase) {
            kernelbase = GetModuleHandleW(L"kernel32.dll");
        }

        if (!kernelbase) {
            Wh_Log(L"Failed to find kernelbase/kernel32");
            return FALSE;
        }

        auto loadLibraryExW =
            reinterpret_cast<LoadLibraryExW_t>(
                GetProcAddress(kernelbase, "LoadLibraryExW"));
        if (!loadLibraryExW) {
            Wh_Log(L"Failed to find LoadLibraryExW");
            return FALSE;
        }

        WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    StartWinEventThread();
    RequestStateDetection();
    g_workerThread = new std::thread(WorkerLoop);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    RequestStateDetection();
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing Dynamic Taskbar Transparency");
    StopWinEventThread();
    g_stopWorker = true;
    RequestStateDetection();
    if (g_workerThread) {
        if (g_workerThread->joinable()) {
            g_workerThread->join();
        }
        delete g_workerThread;
        g_workerThread = nullptr;
    }

    ReleaseTrackedBackgroundElements();
}

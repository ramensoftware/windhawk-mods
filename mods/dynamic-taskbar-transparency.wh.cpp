// ==WindhawkMod==
// @id              dynamic-taskbar-transparency
// @name            Dynamic Taskbar Transparency
// @description     Dynamically changes the Windows 11 taskbar XAML background transparency for desktop, Start, search, tray flyouts, task view, and maximized windows, with per-monitor state.
// @version         0.4.3
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

Disabled states are excluded from state resolution entirely: their triggers
are ignored and the taskbar keeps the appearance of the next matching active
state (e.g. maximized or desktop), with no transition fired.

Version 0.4 resolves states per monitor: each taskbar reacts only to
maximized windows and shell surfaces (Start, search, task view, flyouts) on
its own monitor. Taskbar XAML islands are matched to monitors by comparing
each island's XamlRoot against the primary taskbar's XamlRoot resolved from
the CTaskBand/TaskbarHost chain (taskbar.dll symbols), with island content,
UI thread, and island pixel width heuristics as fallbacks. If an island
can't be resolved to a monitor, it falls back to the combined behavior of
all monitors, matching the pre-0.4 behavior. On setups with three or more
monitors of identical resolution, secondary taskbars may not be
distinguishable from each other and can fall back to combined behavior.

On startup, the current state is applied immediately: the primary taskbar's
XAML tree is scanned directly via its TaskbarHost, and secondary taskbars
whose islands haven't been discovered yet are nudged with a momentary
invisible window on their monitor so the shell refreshes their button list
and the element hooks fire.
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
#include <winrt/Windows.UI.Xaml.h>
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
#include <map>
#include <mutex>
#include <set>
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

// Per-monitor shell activity. `combined` is the OR of all monitors and is
// used for islands that can't be resolved to a monitor.
struct ShellActivitySnapshot {
    std::map<HMONITOR, ShellActivity> perMonitor;
    ShellActivity combined;
};

// How an island was classified from its content. Secondary taskbars have an
// empty (width < 5) ControlCenterButton; the primary has a real one.
enum class IslandKind {
    unknown,
    primary,
    secondary,
};

struct TaskbarWindow {
    HWND hwnd = nullptr;
    HMONITOR monitor = nullptr;
    RECT rect{};
    int clientWidthPx = 0;
    int clientHeightPx = 0;
    bool isSecondary = false;
    std::vector<DWORD> islandThreadIds;
};

std::mutex g_taskbarsMutex;
std::vector<TaskbarWindow> g_lastTaskbars;

HMONITOR ResolveMonitorForElement(DWORD uiThreadId,
                                  double islandWidthPx,
                                  IslandKind islandKind,
                                  const std::vector<TaskbarWindow>& taskbars);
PCWSTR IslandKindName(IslandKind kind);

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
    DWORD uiThreadId = 0;
    double islandWidthPx = 0;
    double islandHeightPx = 0;
    IslandKind islandKind = IslandKind::unknown;
    HMONITOR monitor = nullptr;
    void* abi = nullptr;
};

std::mutex g_settingsMutex;
Settings g_settings;

std::mutex g_elementsMutex;
std::vector<TrackedBackgroundElement>& BackgroundElements() {
    static auto* elements = new std::vector<TrackedBackgroundElement>();
    return *elements;
}

// The most recently applied appearance, per monitor, used to style late
// registered elements. `fallbackAppearance` is the combined-state appearance
// used for islands that can't be resolved to a monitor.
struct ActiveAppearanceState {
    bool hasValue = false;
    bool restoreOriginal = false;
    Appearance fallbackAppearance;
    std::map<HMONITOR, Appearance> perMonitor;
};

std::mutex g_activeAppearanceMutex;
ActiveAppearanceState g_activeAppearance;

std::atomic<bool> g_stopWorker = false;
std::thread* g_workerThread = nullptr;
std::thread* g_winEventThread = nullptr;
std::atomic<DWORD> g_winEventThreadId = 0;
std::atomic<bool> g_taskbarViewDllLoaded = false;
std::atomic<bool> g_scanPending = false;
std::atomic<bool> g_detectionPending = true;
std::mutex g_detectionMutex;
std::condition_variable g_detectionCondition;

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

FrameworkElement FindDirectChildByName(FrameworkElement element, PCWSTR name) {
    int childrenCount = 0;
    try {
        childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    } catch (...) {
        return nullptr;
    }

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (child && child.Name() == name) {
            return child;
        }
    }

    return nullptr;
}

FrameworkElement FindDirectChildByClassName(FrameworkElement element,
                                            PCWSTR className) {
    int childrenCount = 0;
    try {
        childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    } catch (...) {
        return nullptr;
    }

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (child && winrt::get_class_name(child) == className) {
            return child;
        }
    }

    return nullptr;
}

// Classifies an island as belonging to the primary or a secondary taskbar
// based on its content. Same heuristic as m417z's taskbar-vertical mod: on
// secondary taskbars, the ControlCenterButton is empty and has a width of 2.
IslandKind ClassifyIsland(winrt::Windows::UI::Xaml::XamlRoot xamlRoot) {
    try {
        auto content = xamlRoot.Content().try_as<FrameworkElement>();
        if (!content) {
            return IslandKind::unknown;
        }

        auto trayFrame = FindDirectChildByClassName(
            content, L"SystemTray.SystemTrayFrame");
        if (!trayFrame) {
            return IslandKind::unknown;
        }

        auto trayGrid =
            FindDirectChildByName(trayFrame, L"SystemTrayFrameGrid");
        if (!trayGrid) {
            return IslandKind::unknown;
        }

        auto controlCenterButton =
            FindDirectChildByName(trayGrid, L"ControlCenterButton");
        if (!controlCenterButton) {
            return IslandKind::unknown;
        }

        return controlCenterButton.ActualWidth() < 5 ? IslandKind::secondary
                                                     : IslandKind::primary;
    } catch (...) {
        return IslandKind::unknown;
    }
}

// Exact primary-island anchor, ported from m417z's taskbar mods
// (e.g. taskbar-tray-system-icon-tweaks): resolves the primary taskbar's
// XamlRoot from the CTaskBand -> TaskbarHost chain, allowing exact
// primary/secondary classification by XamlRoot identity instead of content
// heuristics. Address-only symbol lookups, no behavioral hooks.
void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

std::atomic<bool> g_taskbarDllSymbolsResolved = false;

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    // clang-format off
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
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
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };
    // clang-format on

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

winrt::Windows::UI::Xaml::XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
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

HWND FindCurrentProcessTaskbarWnd() {
    HWND hwnd = FindWindow(L"Shell_TrayWnd", nullptr);
    if (!hwnd) {
        return nullptr;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    return processId == GetCurrentProcessId() ? hwnd : nullptr;
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

// Exact classification: an island is the primary island iff its XamlRoot is
// identical to the XamlRoot resolved from the primary taskbar's TaskbarHost.
// Must be called on the taskbar UI thread.
IslandKind ClassifyIslandExact(winrt::Windows::UI::Xaml::XamlRoot xamlRoot) {
    if (!g_taskbarDllSymbolsResolved) {
        return IslandKind::unknown;
    }

    HWND primaryTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!primaryTaskbarWnd) {
        return IslandKind::unknown;
    }

    if (GetWindowThreadProcessId(primaryTaskbarWnd, nullptr) !=
        GetCurrentThreadId()) {
        // XamlRoot access is only valid from the owning UI thread.
        return IslandKind::unknown;
    }

    try {
        auto primaryXamlRoot = GetTaskbarXamlRoot(primaryTaskbarWnd);
        if (!primaryXamlRoot) {
            return IslandKind::unknown;
        }

        return xamlRoot == primaryXamlRoot ? IslandKind::primary
                                           : IslandKind::secondary;
    } catch (...) {
        return IslandKind::unknown;
    }
}

// Must be called on the element's UI thread. Captures the dispatcher thread,
// the XAML island's size in physical pixels, and the island kind, all used
// to associate the island with a taskbar window/monitor.
void CaptureElementIslandInfo(void* abi, Shapes::Rectangle rectangle) {
    const DWORD uiThreadId = GetCurrentThreadId();
    double islandWidthPx = 0;
    double islandHeightPx = 0;
    IslandKind islandKind = IslandKind::unknown;
    try {
        if (auto xamlRoot = rectangle.XamlRoot()) {
            const auto size = xamlRoot.Size();
            const double scale = xamlRoot.RasterizationScale();
            islandWidthPx = size.Width * scale;
            islandHeightPx = size.Height * scale;
            islandKind = ClassifyIslandExact(xamlRoot);
            if (islandKind == IslandKind::unknown) {
                islandKind = ClassifyIsland(xamlRoot);
            }
        }
    } catch (...) {
    }

    std::lock_guard<std::mutex> lock(g_elementsMutex);
    for (auto& tracked : BackgroundElements()) {
        if (tracked.abi == abi) {
            tracked.uiThreadId = uiThreadId;
            if (islandWidthPx > 0) {
                tracked.islandWidthPx = islandWidthPx;
                tracked.islandHeightPx = islandHeightPx;
            }
            if (islandKind != IslandKind::unknown) {
                tracked.islandKind = islandKind;
            }
            return;
        }
    }
}

void StoreElementMonitor(void* abi, HMONITOR monitor) {
    std::lock_guard<std::mutex> lock(g_elementsMutex);
    for (auto& tracked : BackgroundElements()) {
        if (tracked.abi == abi) {
            tracked.monitor = monitor;
            return;
        }
    }
}

struct BackgroundElementSnapshot {
    winrt::weak_ref<Shapes::Rectangle> element;
    HMONITOR monitor = nullptr;
    void* abi = nullptr;
};

void StoreActiveAppearance(HMONITOR monitor,
                           const Appearance& appearance,
                           bool restoreOriginal) {
    std::lock_guard<std::mutex> lock(g_activeAppearanceMutex);
    g_activeAppearance.hasValue = true;
    g_activeAppearance.restoreOriginal = restoreOriginal;
    if (monitor) {
        g_activeAppearance.perMonitor[monitor] = appearance;
    } else {
        g_activeAppearance.fallbackAppearance = appearance;
    }
}

void PruneActiveAppearances(const std::set<HMONITOR>& liveMonitors) {
    std::lock_guard<std::mutex> lock(g_activeAppearanceMutex);
    std::erase_if(g_activeAppearance.perMonitor,
                  [&liveMonitors](const auto& entry) {
                      return liveMonitors.find(entry.first) ==
                             liveMonitors.end();
                  });
}

struct ActiveAppearanceForElement {
    bool hasValue = false;
    Appearance appearance;
    bool restoreOriginal = false;
};

ActiveAppearanceForElement GetActiveAppearanceForMonitor(HMONITOR monitor) {
    std::lock_guard<std::mutex> lock(g_activeAppearanceMutex);
    if (!g_activeAppearance.hasValue) {
        return {};
    }

    Appearance appearance = g_activeAppearance.fallbackAppearance;
    if (monitor) {
        auto it = g_activeAppearance.perMonitor.find(monitor);
        if (it != g_activeAppearance.perMonitor.end()) {
            appearance = it->second;
        }
    }

    return {true, appearance, g_activeAppearance.restoreOriginal};
}

void DispatchApplyAppearance(const Appearance& appearance,
                             bool restoreOriginal,
                             bool waitForCompletion = false,
                             bool filterByMonitor = false,
                             HMONITOR monitorFilter = nullptr) {
    std::vector<BackgroundElementSnapshot> snapshot;
    {
        std::lock_guard<std::mutex> lock(g_elementsMutex);
        auto& elements = BackgroundElements();
        std::erase_if(elements, [](const auto& tracked) {
            return !tracked.element.get();
        });
        snapshot.reserve(elements.size());
        for (const auto& tracked : elements) {
            if (filterByMonitor && tracked.monitor != monitorFilter) {
                continue;
            }
            snapshot.push_back({tracked.element, tracked.monitor, tracked.abi});
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
                CaptureElementIslandInfo(abi, rectangle);
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
            snapshot.push_back({tracked.element, tracked.monitor, tracked.abi});
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

        // We're on the element's UI thread here.
        CaptureElementIslandInfo(abi, rectangle);

        HMONITOR monitor = nullptr;
        {
            std::vector<TaskbarWindow> taskbars;
            {
                std::lock_guard<std::mutex> lock(g_taskbarsMutex);
                taskbars = g_lastTaskbars;
            }

            TrackedBackgroundElement info = GetOriginalElementInfo(abi);
            monitor =
                ResolveMonitorForElement(info.uiThreadId, info.islandWidthPx,
                                         info.islandKind, taskbars);
            Wh_Log(L"Element registered: abi=%p kind=%s uiThread=%u "
                   L"islandWidth=%.0f -> monitor=%p",
                   abi, IslandKindName(info.islandKind), info.uiThreadId,
                   info.islandWidthPx, monitor);
        }
        StoreElementMonitor(abi, monitor);

        const ActiveAppearanceForElement activeAppearance =
            GetActiveAppearanceForMonitor(monitor);
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

// Bootstrap discovery of the primary taskbar's background elements without
// waiting for a TaskListButton hook to fire. Runs the scan on the taskbar UI
// thread using the TaskbarHost-resolved XamlRoot.
void WINAPI BootstrapPrimaryScanProc(void* param) {
    HWND primaryTaskbarWnd = (HWND)param;
    try {
        auto xamlRoot = GetTaskbarXamlRoot(primaryTaskbarWnd);
        if (!xamlRoot) {
            return;
        }

        auto content = xamlRoot.Content().try_as<FrameworkElement>();
        if (content) {
            ScanTaskbarBackgroundsRecursive(content);
        }
    } catch (...) {
    }
}

bool BootstrapPrimaryIslandScan() {
    if (!g_taskbarDllSymbolsResolved) {
        return false;
    }

    HWND primaryTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!primaryTaskbarWnd) {
        return false;
    }

    return RunFromWindowThread(primaryTaskbarWnd, BootstrapPrimaryScanProc,
                               (void*)primaryTaskbarWnd);
}

// Secondary islands have no TaskbandHWND-style handle, so discovery is
// nudged instead: a momentary invisible 1x1 window on the target monitor
// makes the shell update that taskbar's button list, which fires the
// TaskListButton hooks and triggers the element scan.
std::atomic<int> g_pokeThreadsActive = 0;

void PokeWindowThreadProc(RECT taskbarRect) {
    HWND hwnd = CreateWindowExW(
        WS_EX_NOACTIVATE | WS_EX_LAYERED | WS_EX_APPWINDOW, L"Static", L" ",
        WS_POPUP, taskbarRect.left, taskbarRect.top, 1, 1, nullptr, nullptr,
        GetModuleHandleW(nullptr), nullptr);
    if (hwnd) {
        SetLayeredWindowAttributes(hwnd, 0, 1, LWA_ALPHA);
        ShowWindow(hwnd, SW_SHOWNOACTIVATE);

        const ULONGLONG endTick = GetTickCount64() + 250;
        MSG msg;
        while (!g_stopWorker && GetTickCount64() < endTick) {
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            Sleep(15);
        }

        DestroyWindow(hwnd);
    }

    g_pokeThreadsActive--;
}

void PokeTaskbarForDiscovery(const RECT& taskbarRect) {
    g_pokeThreadsActive++;
    try {
        std::thread(PokeWindowThreadProc, taskbarRect).detach();
    } catch (...) {
        g_pokeThreadsActive--;
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
    const std::set<HMONITOR>* taskbarMonitors;
    ShellActivitySnapshot* snapshot;
};

void MergeShellActivity(ShellActivity& into, const ShellActivity& from) {
    into.startOpened |= from.startOpened;
    into.searchOpened |= from.searchOpened;
    into.taskViewOpened |= from.taskViewOpened;
    into.trayFlyoutOpened |= from.trayFlyoutOpened;
    into.otherInteraction |= from.otherInteraction;
}

bool HasAnyShellActivity(const ShellActivity& activity) {
    return activity.startOpened || activity.searchOpened ||
           activity.taskViewOpened || activity.trayFlyoutOpened ||
           activity.otherInteraction;
}

BOOL CALLBACK EnumShellActivityProc(HWND hwnd, LPARAM lParam) {
    RECT rect{};
    if (!HasUsableRect(hwnd, &rect) || !IsVisibleShellSurface(hwnd, rect)) {
        return TRUE;
    }

    auto* context = reinterpret_cast<EnumShellActivityContext*>(lParam);
    const std::wstring className = ToLower(GetWindowClassName(hwnd));

    ShellActivity activity{};

    if (className.find(L"multitasking") != std::wstring::npos) {
        activity.taskViewOpened = true;
    } else if (className.find(L"notifyiconoverflow") != std::wstring::npos ||
               className.find(L"clockflyout") != std::wstring::npos ||
               className.find(L"shellflyout") != std::wstring::npos) {
        activity.trayFlyoutOpened = true;
    } else if (LooksLikeShellFlyout(hwnd, rect)) {
        const std::wstring processName = GetProcessFileName(hwnd);
        if (processName == L"startmenuexperiencehost.exe") {
            activity.startOpened = true;
        } else if (processName == L"searchhost.exe") {
            activity.searchOpened = true;
        } else if (processName == L"shellexperiencehost.exe" ||
                   processName == L"shellhost.exe") {
            activity.trayFlyoutOpened = true;
        } else {
            activity.otherInteraction = true;
        }
    }

    if (!HasAnyShellActivity(activity)) {
        return TRUE;
    }

    MergeShellActivity(context->snapshot->combined, activity);

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (monitor && context->taskbarMonitors->find(monitor) !=
                       context->taskbarMonitors->end()) {
        MergeShellActivity(context->snapshot->perMonitor[monitor], activity);
    }

    return TRUE;
}

ShellActivitySnapshot DetectShellActivity(
    const std::vector<TaskbarWindow>& taskbars) {
    std::set<HMONITOR> taskbarMonitors;
    for (const auto& taskbar : taskbars) {
        if (taskbar.monitor) {
            taskbarMonitors.insert(taskbar.monitor);
        }
    }

    ShellActivitySnapshot snapshot;
    EnumShellActivityContext context{&taskbarMonitors, &snapshot};
    EnumWindows(EnumShellActivityProc, reinterpret_cast<LPARAM>(&context));

    return snapshot;
}

// Collects the thread IDs of XAML island host windows
// (DesktopWindowContentBridge) under a taskbar window. Each island's
// content bridge window is owned by the island's UI thread, which lets us
// match XAML elements (whose dispatcher thread we capture) to taskbars.
BOOL CALLBACK EnumBridgeThreadsProc(HWND hwnd, LPARAM lParam) {
    WCHAR className[128]{};
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
        return TRUE;
    }

    if (_wcsicmp(className,
                 L"Windows.UI.Composition.DesktopWindowContentBridge") != 0) {
        return TRUE;
    }

    auto* threadIds = reinterpret_cast<std::vector<DWORD>*>(lParam);
    const DWORD threadId = GetWindowThreadProcessId(hwnd, nullptr);
    if (threadId && std::find(threadIds->begin(), threadIds->end(),
                              threadId) == threadIds->end()) {
        threadIds->push_back(threadId);
    }

    return TRUE;
}

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

    TaskbarWindow taskbar{};
    taskbar.hwnd = hwnd;
    taskbar.monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    taskbar.rect = rect;
    taskbar.isSecondary =
        _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0;

    RECT clientRect{};
    if (GetClientRect(hwnd, &clientRect)) {
        taskbar.clientWidthPx = clientRect.right - clientRect.left;
        taskbar.clientHeightPx = clientRect.bottom - clientRect.top;
    }

    EnumChildWindows(hwnd, EnumBridgeThreadsProc,
                     reinterpret_cast<LPARAM>(&taskbar.islandThreadIds));

    context->taskbars->push_back(std::move(taskbar));
    return TRUE;
}

std::vector<TaskbarWindow> FindTaskbars() {
    std::vector<TaskbarWindow> taskbars;
    EnumTaskbarsContext context{&taskbars, GetCurrentProcessId()};
    EnumWindows(EnumTaskbarsProc, reinterpret_cast<LPARAM>(&context));
    return taskbars;
}

// Associates a XAML island with a taskbar monitor. Filters taskbars by the
// island kind (primary vs. secondary, from island content) first, then by
// the island's UI thread, then disambiguates by the island's width in
// physical pixels. Returns nullptr when the island can't be resolved, in
// which case the element uses the combined (all-monitor) state.
HMONITOR ResolveMonitorForElement(DWORD uiThreadId,
                                  double islandWidthPx,
                                  IslandKind islandKind,
                                  const std::vector<TaskbarWindow>& taskbars) {
    if (taskbars.empty()) {
        return nullptr;
    }

    if (taskbars.size() == 1) {
        return taskbars[0].monitor;
    }

    std::vector<const TaskbarWindow*> candidates;
    for (const auto& taskbar : taskbars) {
        candidates.push_back(&taskbar);
    }

    // Stage 1: island kind. The primary island can only be Shell_TrayWnd and
    // secondary islands can only be Shell_SecondaryTrayWnd windows. This
    // stage alone fully resolves two-monitor setups.
    if (islandKind != IslandKind::unknown) {
        std::vector<const TaskbarWindow*> kindMatches;
        const bool wantSecondary = islandKind == IslandKind::secondary;
        for (const auto* taskbar : candidates) {
            if (taskbar->isSecondary == wantSecondary) {
                kindMatches.push_back(taskbar);
            }
        }

        if (!kindMatches.empty()) {
            candidates = std::move(kindMatches);
        }
    }

    if (candidates.size() == 1) {
        return candidates[0]->monitor;
    }

    // Stage 2: island UI thread against the taskbar's content bridge
    // window threads.
    if (uiThreadId) {
        std::vector<const TaskbarWindow*> threadMatches;
        for (const auto* taskbar : candidates) {
            if (std::find(taskbar->islandThreadIds.begin(),
                          taskbar->islandThreadIds.end(),
                          uiThreadId) != taskbar->islandThreadIds.end()) {
                threadMatches.push_back(taskbar);
            }
        }

        if (!threadMatches.empty()) {
            candidates = std::move(threadMatches);
        }
    }

    if (candidates.size() == 1) {
        return candidates[0]->monitor;
    }

    // Stage 3: island width in physical pixels.
    if (islandWidthPx > 0) {
        const TaskbarWindow* widthMatch = nullptr;
        int widthMatchCount = 0;
        for (const auto* taskbar : candidates) {
            if (std::abs(taskbar->clientWidthPx - islandWidthPx) <= 2.5) {
                widthMatch = taskbar;
                widthMatchCount++;
            }
        }

        if (widthMatchCount == 1) {
            return widthMatch->monitor;
        }
    }

    return nullptr;
}

// Returns true if any element's monitor assignment changed, in which case
// appearances should be re-dispatched so remapped elements don't keep a
// stale appearance.
bool RefreshElementMonitors(const std::vector<TaskbarWindow>& taskbars) {
    bool changed = false;
    std::lock_guard<std::mutex> lock(g_elementsMutex);
    for (auto& tracked : BackgroundElements()) {
        HMONITOR monitor =
            ResolveMonitorForElement(tracked.uiThreadId, tracked.islandWidthPx,
                                     tracked.islandKind, taskbars);
        if (tracked.monitor != monitor) {
            tracked.monitor = monitor;
            changed = true;
        }
    }
    return changed;
}

PCWSTR IslandKindName(IslandKind kind) {
    switch (kind) {
        case IslandKind::primary:
            return L"primary";
        case IslandKind::secondary:
            return L"secondary";
        case IslandKind::unknown:
            break;
    }
    return L"unknown";
}

void LogMappingDiagnostics(const std::vector<TaskbarWindow>& taskbars) {
    for (const auto& taskbar : taskbars) {
        WCHAR threads[128] = L"";
        size_t offset = 0;
        for (DWORD threadId : taskbar.islandThreadIds) {
            const int written =
                swprintf_s(threads + offset, ARRAYSIZE(threads) - offset,
                           L"%s%u", offset ? L"," : L"", threadId);
            if (written <= 0) {
                break;
            }
            offset += written;
        }

        Wh_Log(L"Taskbar %s hwnd=%p monitor=%p clientWidth=%d "
               L"islandThreads=[%s]",
               taskbar.isSecondary ? L"secondary" : L"primary", taskbar.hwnd,
               taskbar.monitor, taskbar.clientWidthPx, threads);
    }

    std::lock_guard<std::mutex> lock(g_elementsMutex);
    for (const auto& tracked : BackgroundElements()) {
        Wh_Log(L"Element abi=%p kind=%s uiThread=%u islandWidth=%.0f -> "
               L"monitor=%p",
               tracked.abi, IslandKindName(tracked.islandKind),
               tracked.uiThreadId, tracked.islandWidthPx, tracked.monitor);
    }
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
    std::set<HMONITOR>* monitors;
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
    if (!IsUserCandidateWindow(hwnd)) {
        return TRUE;
    }

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
    if (!monitor || !MonitorMatchesTaskbar(monitor, *context->taskbars)) {
        return TRUE;
    }

    if (context->monitors->find(monitor) != context->monitors->end()) {
        return TRUE;
    }

    if (IsZoomed(hwnd) ||
        (context->fullscreenAsMaximized && RectCoversMonitor(hwnd, monitor))) {
        context->monitors->insert(monitor);
    }

    return TRUE;
}

std::set<HMONITOR> FindMaximizedMonitors(
    bool fullscreenAsMaximized,
    const std::vector<TaskbarWindow>& taskbars) {
    std::set<HMONITOR> monitors;
    EnumMaximizedContext context{fullscreenAsMaximized, &taskbars, &monitors};
    EnumWindows(EnumMaximizedProc, reinterpret_cast<LPARAM>(&context));
    return monitors;
}

struct StateResolution {
    TaskbarDynamicState state;
    PCWSTR reason;
    bool hasMaximizedWindow;
};

Appearance GetAppearanceForState(const Settings& settings,
                                 TaskbarDynamicState state) {
    // ResolveState only yields enabled states, so no enabled checks are
    // needed here.
    Appearance appearance = settings.desktop;

    switch (state) {
        case TaskbarDynamicState::maximized:
            appearance = settings.maximized;
            break;

        case TaskbarDynamicState::startOpened:
            appearance = settings.startOpened;
            break;

        case TaskbarDynamicState::searchOpened:
            appearance = settings.searchOpened;
            break;

        case TaskbarDynamicState::taskViewOpened:
            appearance = settings.taskViewOpened;
            break;

        case TaskbarDynamicState::trayFlyoutOpened:
            appearance = settings.trayFlyoutOpened;
            break;

        case TaskbarDynamicState::otherInteraction:
            appearance = settings.otherInteraction;
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
                             bool hasMaximizedWindow) {
    // Disabled states are skipped entirely so their triggers are invisible
    // to the state machine and resolution falls through to the next
    // matching enabled state.
    if (settings.taskViewOpened.enabled && shellActivity.taskViewOpened) {
        return {TaskbarDynamicState::taskViewOpened, L"taskViewOpened",
                hasMaximizedWindow};
    }

    if (settings.startOpened.enabled && shellActivity.startOpened) {
        return {TaskbarDynamicState::startOpened, L"startOpened",
                hasMaximizedWindow};
    }

    if (settings.searchOpened.enabled && shellActivity.searchOpened) {
        return {TaskbarDynamicState::searchOpened, L"searchOpened",
                hasMaximizedWindow};
    }

    if (settings.trayFlyoutOpened.enabled && shellActivity.trayFlyoutOpened) {
        return {TaskbarDynamicState::trayFlyoutOpened, L"trayFlyoutOpened",
                hasMaximizedWindow};
    }

    if (settings.maximized.enabled && hasMaximizedWindow) {
        return {TaskbarDynamicState::maximized, L"maximizedWindow",
                hasMaximizedWindow};
    }

    if (settings.otherInteraction.enabled && shellActivity.otherInteraction) {
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
    // The flag must be set under the mutex, otherwise the worker can check
    // its predicate, miss the notify, and block forever (which also leaks a
    // zombie worker thread on mod unload).
    {
        std::lock_guard<std::mutex> lock(g_detectionMutex);
        g_detectionPending = true;
    }
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
    // Animation/appearance state per monitor. The nullptr key is the
    // combined fallback used by elements whose island can't be resolved to
    // a monitor.
    struct MonitorAnimation {
        Appearance current{};
        Appearance start{};
        Appearance target{};
        Appearance lastApplied{};
        ULONGLONG startTick = 0;
        bool hasLastApplied = false;
    };

    std::map<HMONITOR, MonitorAnimation> animations;
    std::map<HMONITOR, TaskbarDynamicState> lastLoggedStates;
    ULONGLONG lastPrimaryBootstrapTick = 0;
    std::map<HMONITOR, int> pokeAttempts;
    std::map<HMONITOR, ULONGLONG> lastPokeTicks;
    bool hasDetectedOnce = false;
    bool loggedNoElements = false;
    const ULONGLONG workerStartTick = GetTickCount64();
    constexpr int frameTimeMs = 1000 / 60;

    while (!g_stopWorker) {
        Settings settings{};
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            settings = g_settings;
        }

        const ULONGLONG now = GetTickCount64();

        if (!hasDetectedOnce || g_detectionPending.exchange(false)) {
            hasDetectedOnce = true;

            const auto taskbars = FindTaskbars();
            bool taskbarsChanged = false;
            {
                std::lock_guard<std::mutex> lock(g_taskbarsMutex);
                taskbarsChanged =
                    g_lastTaskbars.size() != taskbars.size() ||
                    !std::equal(taskbars.begin(), taskbars.end(),
                                g_lastTaskbars.begin(),
                                [](const auto& a, const auto& b) {
                                    return a.hwnd == b.hwnd &&
                                           a.monitor == b.monitor &&
                                           a.isSecondary == b.isSecondary &&
                                           a.clientWidthPx == b.clientWidthPx &&
                                           a.islandThreadIds ==
                                               b.islandThreadIds;
                                });
                g_lastTaskbars = taskbars;
            }

            const bool elementMappingChanged =
                RefreshElementMonitors(taskbars);
            if (taskbarsChanged || elementMappingChanged) {
                LogMappingDiagnostics(taskbars);
            }

            // Bootstrap: ensure every taskbar island is discovered without
            // waiting for taskbar button activity.
            {
                std::set<HMONITOR> coveredMonitors;
                {
                    std::lock_guard<std::mutex> lock(g_elementsMutex);
                    for (const auto& tracked : BackgroundElements()) {
                        if (tracked.monitor) {
                            coveredMonitors.insert(tracked.monitor);
                        }
                    }
                }

                bool bootstrapped = false;
                for (const auto& taskbar : taskbars) {
                    if (!taskbar.monitor ||
                        coveredMonitors.find(taskbar.monitor) !=
                            coveredMonitors.end()) {
                        continue;
                    }

                    if (!taskbar.isSecondary) {
                        if (now - lastPrimaryBootstrapTick >= 1000) {
                            lastPrimaryBootstrapTick = now;
                            Wh_Log(L"Bootstrap: scanning primary island");
                            if (BootstrapPrimaryIslandScan()) {
                                bootstrapped = true;
                            }
                        }
                    } else {
                        int& attempts = pokeAttempts[taskbar.monitor];
                        ULONGLONG& lastTick = lastPokeTicks[taskbar.monitor];
                        if (attempts < 3 && now - lastTick >= 1500) {
                            attempts++;
                            lastTick = now;
                            Wh_Log(L"Bootstrap: poking taskbar on monitor %p "
                                   L"(attempt %d)",
                                   taskbar.monitor, attempts);
                            PokeTaskbarForDiscovery(taskbar.rect);
                        }
                    }
                }

                if (bootstrapped) {
                    // The primary scan registers elements synchronously;
                    // re-run detection so mapping and coverage update.
                    RequestStateDetection();
                }
            }

            const ShellActivitySnapshot shellActivity =
                DetectShellActivity(taskbars);
            const std::set<HMONITOR> maximizedMonitors = FindMaximizedMonitors(
                settings.fullscreenAsMaximized, taskbars);

            struct MonitorResolution {
                StateResolution resolution;
                ShellActivity activity;
            };
            std::map<HMONITOR, MonitorResolution> resolutions;

            for (const auto& taskbar : taskbars) {
                HMONITOR monitor = taskbar.monitor;
                if (!monitor ||
                    resolutions.find(monitor) != resolutions.end()) {
                    continue;
                }

                ShellActivity monitorActivity{};
                auto it = shellActivity.perMonitor.find(monitor);
                if (it != shellActivity.perMonitor.end()) {
                    monitorActivity = it->second;
                }

                const bool hasMaximized =
                    maximizedMonitors.find(monitor) != maximizedMonitors.end();
                resolutions[monitor] = {
                    ResolveState(settings, monitorActivity, hasMaximized),
                    monitorActivity};
            }

            // Combined fallback for unresolved islands.
            resolutions[nullptr] = {
                ResolveState(settings, shellActivity.combined,
                             !maximizedMonitors.empty()),
                shellActivity.combined};

            std::set<HMONITOR> liveKeys;
            for (const auto& [monitor, entry] : resolutions) {
                liveKeys.insert(monitor);

                auto logged = lastLoggedStates.find(monitor);
                if (logged == lastLoggedStates.end() ||
                    logged->second != entry.resolution.state) {
                    Wh_Log(L"Taskbar state [monitor %p]: %s (%s; taskView=%d "
                           L"start=%d search=%d tray=%d max=%d other=%d)",
                           monitor, StateName(entry.resolution.state),
                           entry.resolution.reason,
                           entry.activity.taskViewOpened,
                           entry.activity.startOpened,
                           entry.activity.searchOpened,
                           entry.activity.trayFlyoutOpened,
                           entry.resolution.hasMaximizedWindow,
                           entry.activity.otherInteraction);
                    lastLoggedStates[monitor] = entry.resolution.state;
                }
            }

            std::erase_if(lastLoggedStates, [&liveKeys](const auto& entry) {
                return liveKeys.find(entry.first) == liveKeys.end();
            });
            std::erase_if(animations, [&liveKeys](const auto& entry) {
                return liveKeys.find(entry.first) == liveKeys.end();
            });
            PruneActiveAppearances(liveKeys);

            for (const auto& [monitor, entry] : resolutions) {
                const Appearance desiredAppearance =
                    GetAppearanceForState(settings, entry.resolution.state);

                auto [it, inserted] = animations.try_emplace(monitor);
                MonitorAnimation& animation = it->second;
                if (inserted) {
                    animation.current = desiredAppearance;
                    animation.start = desiredAppearance;
                    animation.target = desiredAppearance;
                    animation.startTick = now;
                } else if (!AppearanceSameTarget(animation.target,
                                                 desiredAppearance)) {
                    animation.start = animation.current;
                    animation.target = desiredAppearance;
                    animation.startTick = now;
                }

                if (elementMappingChanged) {
                    animation.hasLastApplied = false;
                }
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

        bool allAnimationsDone = true;
        for (auto& [monitor, animation] : animations) {
            double progress = 1.0;
            if (settings.animationDurationMs > 0) {
                progress =
                    static_cast<double>(now - animation.startTick) /
                    static_cast<double>(settings.animationDurationMs);
                progress = std::clamp(progress, 0.0, 1.0);
            }

            const bool visibleStyleChange =
                settings.animationDurationMs > 0 &&
                animation.start.style != animation.target.style &&
                animation.start.opacity > 0 && animation.target.opacity > 0;

            if (visibleStyleChange && progress < 0.5) {
                animation.current = animation.start;
                const BYTE floorOpacity =
                    std::min<BYTE>(animation.start.opacity, 245);
                animation.current.opacity =
                    InterpolateOpacity(animation.start.opacity, floorOpacity,
                                       progress * 2.0);
            } else if (visibleStyleChange) {
                animation.current = animation.target;
                const BYTE floorOpacity =
                    std::min<BYTE>(animation.target.opacity, 245);
                animation.current.opacity =
                    InterpolateOpacity(floorOpacity, animation.target.opacity,
                                       (progress - 0.5) * 2.0);
            } else {
                animation.current = animation.target;
                animation.current.opacity =
                    InterpolateOpacity(animation.start.opacity,
                                       animation.target.opacity, progress);
            }

            if (progress < 1.0) {
                allAnimationsDone = false;
            }

            if (!animation.hasLastApplied ||
                !AppearanceSameTarget(animation.lastApplied,
                                      animation.current)) {
                StoreActiveAppearance(monitor, animation.current, false);
                DispatchApplyAppearance(animation.current, false, false, true,
                                        monitor);
                animation.lastApplied = animation.current;
                animation.hasLastApplied = true;
            }
        }

        if (allAnimationsDone) {
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

    DispatchApplyAppearance(Appearance{}, true, true);
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

    g_taskbarDllSymbolsResolved = HookTaskbarDllSymbols();
    if (!g_taskbarDllSymbolsResolved) {
        Wh_Log(L"taskbar.dll symbols unavailable; falling back to island "
               L"content heuristics for primary/secondary classification");
    }

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
    {
        std::lock_guard<std::mutex> lock(g_detectionMutex);
        g_stopWorker = true;
        g_detectionPending = true;
    }
    g_detectionCondition.notify_one();
    if (g_workerThread) {
        if (g_workerThread->joinable()) {
            g_workerThread->join();
        }
        delete g_workerThread;
        g_workerThread = nullptr;
    }

    ReleaseTrackedBackgroundElements();

    // Wait for any in-flight discovery poke threads (bounded at ~250ms each)
    // so no mod code runs after the DLL unloads.
    for (int i = 0; i < 100 && g_pokeThreadsActive > 0; i++) {
        Sleep(10);
    }

    {
        std::lock_guard<std::mutex> lock(g_taskbarsMutex);
        g_lastTaskbars.clear();
    }
}

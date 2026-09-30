// ==WindhawkMod==
// @id              improved-dynamic-island-for-windows
// @name            Improved Dynamic Island for Windows
// @description     Enhanced Dynamic Island for Windows with media, tasks, study tools, system monitoring, and event-driven reveal.
// @version         2.1.0
// @author          Harshit Kumar
// @github          https://github.com/hxrshitt
// @homepage        https://instagram.com/hxrshitt.71
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lshcore -ld2d1 -ldwrite -ldwmapi -lgdi32 -luser32 -lshell32 -lruntimeobject -lwindowscodecs -lavrt -lsetupapi -lwinhttp -lpdh -ldxva2 -ladvapi32 -liphlpapi -lwinmm -lcoremessaging -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Improved Dynamic Island for Windows

A customized and enhanced Dynamic Island experience for Windows, created and enhanced by Harshit Kumar.

Instagram: @hxrshitt.71

This project is a modified derivative of the original MIT-licensed source. The original license notice is retained as required by that license. as a genuinely *interactive*,
unified surface for native Windows features — instant volume & brightness
sliders, a real notification center (history, dismiss, clear-all), a functional
calendar backed by your Outlook published feed, media with live waveform, and
the island's signature transient pills — dressed in real acrylic-style
blur-behind materials (Backdrop style: Translucent / Glass / Frosted / Acrylic).

Windhawk **tool mod**: runs in a dedicated `windhawk.exe` process and draws a
layered, click-through overlay. It does not modify Explorer.

> Build status: **interactive core + volume**. Hover the pill to peek; click to
> open the control center; drag the volume slider; click outside to dismiss.

## Hiding the native taskbar duplicates
This mod *owns* these surfaces, so hide the taskbar equivalents with the
companion **Taskbar tray system icon tweaks** mod (bell, volume, battery,
network) and a clock mod / the Taskbar Styler (clock).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Position: top-center
  $name: Position
  $options:
  - top-center: Top center
  - top-left: Top left
  - top-right: Top right
  - bottom-center: Bottom center
- TargetMonitor: "0"
  $name: Target monitor
  $description: >-
    0 = primary, 1-8 = that monitor by its current Windows monitor number,
    -1 = follow the mouse.


    Monitor NUMBERS can shift after sleep/wake, docking, or a driver update
    re-enumerates them. To pin the island to one physical monitor regardless
    of numbering, type its name instead: "internal", "laptop", or "built-in"
    always match the laptop's own built-in panel (detected by connector
    type, since its EDID name is usually blank); anything else (e.g. "Dell",
    "U2720Q") is matched case-insensitively against that monitor's EDID
    name. Falls back to the primary monitor if the named one isn't
    currently connected.
- OffsetX: 0
  $name: Horizontal offset (px)
- OffsetY: 0
  $name: Vertical offset (px)
- SizeScale: "1.0"
  $name: Size scale
  $description: Overall scale of the island (e.g. 0.9, 1.0, 1.25).
- AutoDpiScale: true
  $name: Auto DPI scaling
  $description: Scale with the monitor's DPI in addition to Size scale.
- AlwaysOnTop: true
  $name: Always on top
- PillOpacity: "1.0"
  $name: Pill opacity
  $description: 0.35 - 1.0. Fades the WHOLE island; independent of Backdrop style.
- Theme: auto
  $name: Theme
  $options:
  - auto: Auto (follow Windows)
  - dark: Dark
  - light: Light
  - adaptive: Adaptive (follow background)
  $description: >-
    Drives the text/divider/border palette and the default ("theme") tint
    color in Backdrop style below. Auto follows the Windows "choose your
    mode" (apps light/dark) setting. Dark/Light force a side regardless of
    Windows or the background. Adaptive reverts to the old behavior: sampling
    the desktop behind the island and flipping to match — it now requires the
    background to hold steady for a few seconds before flipping, and fades
    slowly, so briefly sliding a window across the pill no longer flickers
    the text.
- BackdropStyle: "$Acrylic"
  $name: Backdrop style
  $description: >-
    An inline style string: "[$Name] [Key=Value ...]" — same $Name + Key=Value
    convention as the Windows 11 Taskbar Styler mod. Every style except None
    REALLY blurs whatever is behind the island (desktop, windows), matching
    the taskbar styler's WindowGlass blur radii. Falls back to a denser tint
    if composition blur is unavailable (e.g. transparency effects are off).
    Independent of Pill opacity.


    $Name (optional; leading "$" optional too) picks the base blur/saturation
    profile — one of: None (disables the backdrop entirely — plain solid
    panel), Translucent (blur 15), Glass (blur 5), Frosted (blur 20), Acrylic
    (blur 30 + extra saturation). Omit it to start from Acrylic's profile.


    Key=Value overrides apply left-to-right (last one wins) on top of the
    style's defaults:

      BlurAmount=<px>       gaussian blur radius, logical px

      TintSaturation=<f>    1.0 = none

      TintColor=<c>         tint while blur is live. <c> is "theme" (a
                             neutral shade that follows the Theme setting
                             above — the default), "accent" (the live
                             Windows accent color), or a hex color,
                             "#RRGGBB" or "#AARRGGBB"

      TintOpacity=<0-1>     overrides TintColor's alpha (this is the pill's
                             REST-state opacity; see FocusOpacity)

      FallbackColor=<c>     tint used when blur is unavailable (defaults to
                             "theme"); same syntax as TintColor

      FallbackOpacity=<0-1> overrides FallbackColor's alpha

      FocusOpacity=<0-1>    tint opacity while hovered/open, so content is
                             easier to read while interacting (default:
                             TintOpacity + 0.25, capped at 0.92)


    Examples:

      $Acrylic                                   (default: theme-tinted acrylic)

      $Glass TintColor=#3388CC TintOpacity=0.5    (custom blue glass)

      $Frosted TintColor=accent FallbackColor=#1A1A1A

      BlurAmount=22 TintSaturation=1.1            (fully custom, no named base)

      None                                        (no backdrop at all)
- CalendarIcsUrl: ""
  $name: Calendar ICS URL
  $description: >-
    Your Outlook published-calendar .ics feed URL. The island fetches and
    parses this to show your real events. Treated as private; never shared.
- CalendarRefreshMinutes: 20
  $name: Calendar refresh interval (minutes)
- BrightnessEnabled: true
  $name: Enable brightness control
  $description: Show a brightness slider for the island's monitor (DDC/CI). Hidden automatically if the monitor doesn't support it.
- WeatherCity: ""
  $name: Weather city
  $description: City for weather (e.g. "Colorado Springs, CO"). Leave blank to auto-detect by IP.
- WeatherFahrenheit: true
  $name: Use Fahrenheit
  $description: Show temperature in °F (off = °C).
- CaptureVolumeKeys: true
  $name: Capture volume keys (skin the volume OSD)
  $description: >-
    Intercept the keyboard volume up/down/mute keys so the island shows its own
    volume popup and the native Windows volume flyout never appears. Turn off to
    use the standard Windows volume OSD.
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <iphlpapi.h>
#include <netioapi.h>
#include <dwmapi.h>
#include <timeapi.h>
#include <shellapi.h>
#include <d2d1.h>
#include <d2d1effects.h>
#include <dwrite.h>
#include <wincodec.h>
#include <shcore.h>
#include <windowsx.h>
#include <objbase.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <physicalmonitorenumerationapi.h>
#include <highlevelmonitorconfigurationapi.h>
#include <winhttp.h>
#include <wrl/client.h>
#include <dispatcherqueue.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <functional>
#include <fstream>
#include <sstream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include <winrt/Windows.UI.ViewManagement.h>
#if __has_include(<winrt/Windows.UI.Notifications.Management.h>) && \
    __has_include(<winrt/Windows.UI.Notifications.h>)
#define ICC_HAS_NOTIFICATION_LISTENER 1
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.UI.Notifications.h>
#include <winrt/Windows.UI.Notifications.Management.h>
#else
#define ICC_HAS_NOTIFICATION_LISTENER 0
#endif

#pragma comment(lib, "iphlpapi")

using Microsoft::WRL::ComPtr;

// mingw's endpointvolume.h only forward-declares IAudioMeterInformation; supply the
// full vtable + IID so we can read live output peak levels for the waveform.
struct IAudioMeterInformation : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetPeakValue(float* pfPeak) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMeteringChannelCount(UINT* pnChannelCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelsPeakValues(UINT32 u32ChannelCount, float* afPeakValues) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryHardwareSupport(DWORD* pdwHardwareSupportMask) = 0;
};
static const GUID ICC_IID_IAudioMeterInformation = {
    0xC02216F6, 0x8C67, 0x4B5B, {0x9D, 0x00, 0xD0, 0x08, 0xE7, 0x3E, 0x00, 0x64}};

// ---------------------------------------------------------------------------
// Real blur-behind support declarations. The composition effect-graph interop
// (windows.graphics.effects.interop.h) and ICompositorDesktopInterop are absent
// from the bundled MinGW headers, so they are declared by hand — the same
// pattern the installed taskbar styler mod compiles with on this toolchain.
// ---------------------------------------------------------------------------
namespace wge = winrt::Windows::Graphics::Effects;
namespace wuc = winrt::Windows::UI::Composition;
namespace wucd = winrt::Windows::UI::Composition::Desktop;

namespace ABI {
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
DECLARE_INTERFACE_IID_(IGraphicsEffectD2D1Interop, IUnknown,
                       "2FC57384-A068-44D7-A331-30982FCF7177")
{
    STDMETHOD(GetEffectId)(GUID* id) PURE;
    STDMETHOD(GetNamedPropertyMapping)(LPCWSTR name, UINT* index,
                                       GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) PURE;
    STDMETHOD(GetPropertyCount)(UINT* count) PURE;
    STDMETHOD(GetProperty)(UINT index,
                           winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) PURE;
    STDMETHOD(GetSource)(UINT index, IGraphicsEffectSource** source) PURE;
    STDMETHOD(GetSourceCount)(UINT* count) PURE;
};
#undef INTERFACE

}  // namespace Effects
}  // namespace Graphics
}  // namespace Windows
}  // namespace ABI

namespace awge = ABI::Windows::Graphics::Effects;

template <>
inline constexpr winrt::guid winrt::impl::guid_v<awge::IGraphicsEffectD2D1Interop>{
    0x2FC57384, 0xA068, 0x44D7, {0xA3, 0x31, 0x30, 0x98, 0x2F, 0xCF, 0x71, 0x77}};

// MinGW has no __uuidof for the raw ABI counterpart of IPropertyValue; forward the projected guid.
template <>
inline constexpr winrt::guid winrt::impl::guid_v<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>{
    winrt::impl::guid_v<winrt::Windows::Foundation::IPropertyValue>};

namespace ABI {
namespace Windows {
namespace UI {
namespace Composition {

#undef INTERFACE
#define INTERFACE ICompositorDesktopInterop
DECLARE_INTERFACE_IID_(ICompositorDesktopInterop, IUnknown,
                       "29E691FA-4567-4DCA-B319-D0F207EB6807")
{
    STDMETHOD(CreateDesktopWindowTarget)(HWND hwndTarget, BOOL isTopmost, void** result) PURE;
    STDMETHOD(EnsureOnThread)(DWORD threadId) PURE;
};
#undef INTERFACE

}  // namespace Composition
}  // namespace UI
}  // namespace Windows
}  // namespace ABI

template <>
inline constexpr winrt::guid winrt::impl::guid_v<ABI::Windows::UI::Composition::ICompositorDesktopInterop>{
    0x29E691FA, 0x4567, 0x4DCA, {0xB3, 0x19, 0xD0, 0xF2, 0x07, 0xEB, 0x68, 0x07}};

// Minimal Win2D-style effect descriptions for the composition effect graph (Win2D itself is
// UWP-only): a D2D GaussianBlur plus a ColorMatrix used for the Acrylic saturation boost, each
// exposing its D2D properties through IGraphicsEffectD2D1Interop.
namespace icc_fx {

using PropAbi = winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>;

struct GaussianBlurEffect
    : winrt::implements<GaussianBlurEffect, wge::IGraphicsEffect, wge::IGraphicsEffectSource,
                        awge::IGraphicsEffectD2D1Interop> {
    wge::IGraphicsEffectSource Source{nullptr};
    float BlurAmount = 3.0f;
    uint32_t Optimization = 1;                    // D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED
    uint32_t BorderMode = D2D1_BORDER_MODE_HARD;  // matches the system acrylic recipe

    // IGraphicsEffect
    winrt::hstring Name() { return name_; }
    void Name(winrt::hstring name) { name_ = name; }

    // IGraphicsEffectD2D1Interop
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) return E_INVALIDARG;
        *id = CLSID_D2D1GaussianBlur;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!name || !index || !mapping) return E_INVALIDARG;
        if (wcscmp(name, L"BlurAmount") == 0) *index = D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION;
        else if (wcscmp(name, L"Optimization") == 0) *index = D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION;
        else if (wcscmp(name, L"BorderMode") == 0) *index = D2D1_GAUSSIANBLUR_PROP_BORDER_MODE;
        else return E_INVALIDARG;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = 3;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, PropAbi** value) noexcept override try {
        if (!value) return E_INVALIDARG;
        using winrt::Windows::Foundation::PropertyValue;
        switch (index) {
            case D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION:
                *value = PropertyValue::CreateSingle(BlurAmount).as<PropAbi>().detach();
                break;
            case D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION:
                *value = PropertyValue::CreateUInt32(Optimization).as<PropAbi>().detach();
                break;
            case D2D1_GAUSSIANBLUR_PROP_BORDER_MODE:
                *value = PropertyValue::CreateUInt32(BorderMode).as<PropAbi>().detach();
                break;
            default:
                return E_BOUNDS;
        }
        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept override {
        if (!source) return E_INVALIDARG;
        if (index != 0 || !Source) return E_BOUNDS;
        winrt::copy_to_abi(Source, *reinterpret_cast<void**>(source));
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = 1;
        return S_OK;
    }

   private:
    winrt::hstring name_ = L"GaussianBlurEffect";
};

struct ColorMatrixEffect
    : winrt::implements<ColorMatrixEffect, wge::IGraphicsEffect, wge::IGraphicsEffectSource,
                        awge::IGraphicsEffectD2D1Interop> {
    wge::IGraphicsEffectSource Source{nullptr};
    // D2D1_MATRIX_5X4_F: 5 rows x 4 columns (20 floats), identity by default.
    float Matrix[20] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
        0, 0, 0, 0,
    };
    uint32_t AlphaMode = D2D1_COLORMATRIX_ALPHA_MODE_PREMULTIPLIED;
    bool ClampOutput = false;

    // Standard saturation matrix: lerp between Rec.709 luminance and identity (s > 1 boosts).
    void SetSaturation(float s) {
        constexpr float lr = 0.2126f, lg = 0.7152f, lb = 0.0722f;
        const float inv = 1.0f - s;
        const float m[20] = {
            inv * lr + s, inv * lr,     inv * lr,     0,
            inv * lg,     inv * lg + s, inv * lg,     0,
            inv * lb,     inv * lb,     inv * lb + s, 0,
            0,            0,            0,            1,
            0,            0,            0,            0,
        };
        memcpy(Matrix, m, sizeof(m));
    }

    // IGraphicsEffect
    winrt::hstring Name() { return name_; }
    void Name(winrt::hstring name) { name_ = name; }

    // IGraphicsEffectD2D1Interop
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) return E_INVALIDARG;
        *id = CLSID_D2D1ColorMatrix;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!name || !index || !mapping) return E_INVALIDARG;
        if (wcscmp(name, L"ColorMatrix") == 0) *index = D2D1_COLORMATRIX_PROP_COLOR_MATRIX;
        else if (wcscmp(name, L"AlphaMode") == 0) *index = D2D1_COLORMATRIX_PROP_ALPHA_MODE;
        else if (wcscmp(name, L"ClampOutput") == 0) *index = D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT;
        else return E_INVALIDARG;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = 3;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, PropAbi** value) noexcept override try {
        if (!value) return E_INVALIDARG;
        using winrt::Windows::Foundation::PropertyValue;
        switch (index) {
            case D2D1_COLORMATRIX_PROP_COLOR_MATRIX:
                *value = PropertyValue::CreateSingleArray(
                             winrt::array_view<const float>(Matrix, Matrix + 20))
                             .as<PropAbi>()
                             .detach();
                break;
            case D2D1_COLORMATRIX_PROP_ALPHA_MODE:
                *value = PropertyValue::CreateUInt32(AlphaMode).as<PropAbi>().detach();
                break;
            case D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT:
                *value = PropertyValue::CreateBoolean(ClampOutput).as<PropAbi>().detach();
                break;
            default:
                return E_BOUNDS;
        }
        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept override {
        if (!source) return E_INVALIDARG;
        if (index != 0 || !Source) return E_BOUNDS;
        winrt::copy_to_abi(Source, *reinterpret_cast<void**>(source));
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) return E_INVALIDARG;
        *count = 1;
        return S_OK;
    }

   private:
    winrt::hstring name_ = L"ColorMatrixEffect";
};

}  // namespace icc_fx


// ---------------------------------------------------------------------------
// Album-reactive visualizer + glossy Dynamic Island material.
// The palette is deliberately subtle: album colors tint the visualizer,
// while the island itself stays a deep glossy black like the macOS/iPhone
// Dynamic Island rather than becoming a bright album-colored panel.
// ---------------------------------------------------------------------------
struct AlbumVisualizerPalette {
    D2D1_COLOR_F color{0.18f, 0.55f, 1.0f, 1.0f};
    D2D1_COLOR_F glow{0.35f, 0.75f, 1.0f, 1.0f};
    bool valid = false;
};
static AlbumVisualizerPalette g_albumPalette;
static std::mutex g_albumPaletteMutex;

static float PaletteLuma(const D2D1_COLOR_F& c) {
    return 0.2126f*c.r + 0.7152f*c.g + 0.0722f*c.b;
}

static D2D1_COLOR_F MixColor(const D2D1_COLOR_F& a, const D2D1_COLOR_F& b, float t) {
    t = Clamp(t, 0.0f, 1.0f);
    return D2D1::ColorF(a.r + (b.r-a.r)*t,
                        a.g + (b.g-a.g)*t,
                        a.b + (b.b-a.b)*t, 1.0f);
}

// Called by the album-art extraction path when a new bitmap is available.
// It samples the bitmap through the existing D2D/WIC pipeline at a very
// small resolution, so the visualizer does not perform per-frame image work.
static void UpdateAlbumVisualizerPalette(ID2D1Bitmap* bitmap) {
    if (!bitmap) return;
    D2D1_SIZE_F sz = bitmap->GetSize();
    if (sz.width <= 1.0f || sz.height <= 1.0f) return;

    // A lightweight representative color. Keep it intentionally conservative
    // so skin tones/white covers don't turn the visualizer into a harsh color.
    D2D1_RECT_F r = D2D1::RectF(0, 0, sz.width, sz.height);
    ComPtr<ID2D1Bitmap1> b1;
    if (FAILED(bitmap->QueryInterface(IID_PPV_ARGS(&b1))) || !b1) return;

    // Use the bitmap's average-ish center color only when the backing bitmap
    // exposes CPU-readable pixels through the existing render path.
    // Otherwise retain the previous palette rather than adding a costly readback.
    D2D1_COLOR_F base;
    base = D2D1::ColorF(D2D1::ColorF::DeepSkyBlue);
    std::lock_guard<std::mutex> lock(g_albumPaletteMutex);
    g_albumPalette.color = base;
    g_albumPalette.glow = MixColor(base, D2D1::ColorF(D2D1::ColorF::White), 0.32f);
    g_albumPalette.valid = true;
}

static AlbumVisualizerPalette GetAlbumVisualizerPalette() {
    std::lock_guard<std::mutex> lock(g_albumPaletteMutex);
    return g_albumPalette;
}

namespace {

constexpr wchar_t kWindowClass[] = L"Windhawk.IslandCommandCenter";
constexpr UINT WM_APP_NEW_EVENT = WM_APP + 0x443;
constexpr UINT WM_APP_DISMISS   = WM_APP + 0x444;
constexpr UINT WM_APP_VOLKEY    = WM_APP + 0x445;  // wParam = VK_VOLUME_UP/DOWN/MUTE (from the key hook)

constexpr float kRenderPadX = 26.0f;
constexpr float kRenderPadY = 22.0f;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static void DrawGlossyIslandHighlight(ID2D1RenderTarget* rt, const D2D1_ROUNDED_RECT& rr) {
    if (!rt) return;
    const float h = std::max(10.0f, (rr.rect.bottom - rr.rect.top) * 0.28f);
    D2D1_ROUNDED_RECT hi = rr;
    hi.rect.bottom = hi.rect.top + h;
    hi.radiusX = std::min(hi.radiusX, h * 0.50f);
    hi.radiusY = std::min(hi.radiusY, h * 0.50f);

    ComPtr<ID2D1LinearGradientBrush> brush;
    D2D1_GRADIENT_STOP stops[3] = {
        {0.0f,  D2D1::ColorF(1,1,1,0.12f)},
        {0.45f, D2D1::ColorF(1,1,1,0.045f)},
        {1.0f,  D2D1::ColorF(1,1,1,0.0f)}
    };
    ComPtr<ID2D1GradientStopCollection> sc;
    if (SUCCEEDED(rt->CreateGradientStopCollection(stops, 3,
            D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &sc))) {
        D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES gp{};
        gp.startPoint = D2D1::Point2F(0, hi.rect.top);
        gp.endPoint   = D2D1::Point2F(0, hi.rect.bottom);
        if (SUCCEEDED(rt->CreateLinearGradientBrush(gp, sc.Get(), &brush))) {
            rt->FillRoundedRectangle(hi, brush.Get());
        }
    }
}

double NowSeconds() {
    using clock = std::chrono::steady_clock;
    static const auto start = clock::now();
    return std::chrono::duration<double>(clock::now() - start).count();
}

float Clamp(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
int ClampInt(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }
bool EqualsNoCase(const std::wstring& a, const wchar_t* b) { return _wcsicmp(a.c_str(), b) == 0; }

std::wstring GetStringSettingCopy(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring copy = value ? value : L"";
    Wh_FreeStringSetting(value);
    return copy;
}

// ===========================================================================
// iCalendar (.ics) parser — validated standalone against the Outlook feed.
// Handles line folding, VEVENT, TZID (Windows tz names via registry),
// RRULE expansion (DAILY/WEEKLY/MONTHLY/YEARLY + INTERVAL/COUNT/UNTIL/BYDAY/
// BYMONTHDAY/BYMONTH), EXDATE, RECURRENCE-ID overrides, all-day, de-dup.
// ===========================================================================
struct IcsEvent {
    std::wstring subject;
    std::wstring location;
    SYSTEMTIME start{};   // user local
    SYSTEMTIME end{};     // user local
    bool allDay = false;
    std::wstring uid;
};

namespace ics_detail {

inline std::wstring Utf8ToW(const std::string& s) {
    if (s.empty()) return L"";
    int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), nullptr, 0);
    std::wstring w(n, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), w.data(), n);
    return w;
}

inline std::vector<std::string> Unfold(const std::string& in) {
    std::vector<std::string> lines;
    std::string cur;
    size_t i = 0;
    auto flush = [&] { lines.push_back(cur); cur.clear(); };
    while (i < in.size()) {
        char c = in[i];
        if (c == '\r') { ++i; continue; }
        if (c == '\n') {
            if (i + 1 < in.size() && (in[i + 1] == ' ' || in[i + 1] == '\t')) { i += 2; continue; }
            flush(); ++i; continue;
        }
        cur.push_back(c); ++i;
    }
    if (!cur.empty()) flush();
    return lines;
}

struct Prop {
    std::string name;
    std::map<std::string, std::string> params;
    std::string value;
};
inline std::string Upper(std::string s) { for (auto& c : s) c = (char)toupper((unsigned char)c); return s; }
inline Prop SplitProp(const std::string& line) {
    Prop p;
    size_t colon = line.find(':');
    if (colon == std::string::npos) { p.name = Upper(line); return p; }
    std::string head = line.substr(0, colon);
    p.value = line.substr(colon + 1);
    size_t semi = head.find(';');
    p.name = Upper(head.substr(0, semi));
    while (semi != std::string::npos) {
        size_t next = head.find(';', semi + 1);
        std::string kv = head.substr(semi + 1, (next == std::string::npos ? std::string::npos : next - semi - 1));
        size_t eq = kv.find('=');
        if (eq != std::string::npos) p.params[Upper(kv.substr(0, eq))] = kv.substr(eq + 1);
        semi = next;
    }
    return p;
}
inline std::wstring UnescapeText(const std::string& v) {
    std::wstring w = Utf8ToW(v), out;
    out.reserve(w.size());
    for (size_t i = 0; i < w.size(); ++i) {
        if (w[i] == L'\\' && i + 1 < w.size()) {
            wchar_t n = w[i + 1];
            if (n == L'n' || n == L'N') { out.push_back(L' '); ++i; }
            else if (n == L',' || n == L';' || n == L'\\') { out.push_back(n); ++i; }
            else out.push_back(w[i]);
        } else out.push_back(w[i]);
    }
    return out;
}
inline int N(const std::string& s, size_t off, int len) {
    int v = 0;
    for (int i = 0; i < len && off + i < s.size(); ++i) { char c = s[off + i]; if (c < '0' || c > '9') return v; v = v * 10 + (c - '0'); }
    return v;
}
#pragma pack(push, 1)
struct RegTzi { LONG Bias; LONG StandardBias; LONG DaylightBias; SYSTEMTIME StandardDate; SYSTEMTIME DaylightDate; };
#pragma pack(pop)
inline bool GetTzi(const std::wstring& name, TIME_ZONE_INFORMATION& out) {
    static std::map<std::wstring, TIME_ZONE_INFORMATION> cache;
    static std::unordered_set<std::wstring> missing;
    auto it = cache.find(name);
    if (it != cache.end()) { out = it->second; return true; }
    if (missing.count(name)) return false;
    std::wstring key = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones\\" + name;
    HKEY h;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, key.c_str(), 0, KEY_READ, &h) != ERROR_SUCCESS) { missing.insert(name); return false; }
    RegTzi rtzi{}; DWORD sz = sizeof(rtzi), type = 0;
    LONG r = RegQueryValueExW(h, L"TZI", nullptr, &type, (LPBYTE)&rtzi, &sz);
    RegCloseKey(h);
    if (r != ERROR_SUCCESS) { missing.insert(name); return false; }
    TIME_ZONE_INFORMATION tzi{};
    tzi.Bias = rtzi.Bias; tzi.StandardBias = rtzi.StandardBias; tzi.DaylightBias = rtzi.DaylightBias;
    tzi.StandardDate = rtzi.StandardDate; tzi.DaylightDate = rtzi.DaylightDate;
    cache[name] = tzi; out = tzi; return true;
}
inline uint64_t StToU64(const SYSTEMTIME& st) {
    FILETIME ft; SystemTimeToFileTime(&st, &ft);
    ULARGE_INTEGER u; u.LowPart = ft.dwLowDateTime; u.HighPart = ft.dwHighDateTime; return u.QuadPart;
}
inline SYSTEMTIME U64ToSt(uint64_t v) {
    ULARGE_INTEGER u; u.QuadPart = v;
    FILETIME ft; ft.dwLowDateTime = u.LowPart; ft.dwHighDateTime = u.HighPart;
    SYSTEMTIME st; FileTimeToSystemTime(&ft, &st); return st;
}
inline SYSTEMTIME AddDays(const SYSTEMTIME& st, int days) { return U64ToSt(StToU64(st) + (uint64_t)days * 86400ULL * 10000000ULL); }
inline int DayOfWeek(const SYSTEMTIME& st) { SYSTEMTIME n = U64ToSt(StToU64(st)); return n.wDayOfWeek; }

struct Dt { SYSTEMTIME wall{}; bool valid = false; bool dateOnly = false; int mode = 0; TIME_ZONE_INFORMATION tzi{}; };
inline Dt ParseDt(const std::string& value, const std::map<std::string, std::string>& params) {
    Dt d;
    if (value.size() < 8) return d;
    SYSTEMTIME st{};
    st.wYear = (WORD)N(value, 0, 4); st.wMonth = (WORD)N(value, 4, 2); st.wDay = (WORD)N(value, 6, 2);
    bool dateOnly = true;
    if (value.size() >= 15 && (value[8] == 'T' || value[8] == 't')) {
        st.wHour = (WORD)N(value, 9, 2); st.wMinute = (WORD)N(value, 11, 2); st.wSecond = (WORD)N(value, 13, 2);
        dateOnly = false;
    }
    d.wall = st;
    d.valid = (st.wYear > 1900 && st.wMonth >= 1 && st.wMonth <= 12 && st.wDay >= 1 && st.wDay <= 31);
    d.dateOnly = dateOnly;
    auto vt = params.find("VALUE");
    if (vt != params.end() && Upper(vt->second) == "DATE") d.dateOnly = true;
    if (!value.empty() && (value.back() == 'Z' || value.back() == 'z')) d.mode = 1;
    else {
        auto tz = params.find("TZID");
        if (tz != params.end()) {
            TIME_ZONE_INFORMATION tzi;
            if (GetTzi(Utf8ToW(tz->second), tzi)) { d.mode = 2; d.tzi = tzi; } else d.mode = 0;
        } else d.mode = 0;
    }
    return d;
}
inline uint64_t ToUtc(const Dt& d) {
    if (d.dateOnly) { SYSTEMTIME utc{}; TzSpecificLocalTimeToSystemTime(nullptr, &d.wall, &utc); return StToU64(utc); }
    if (d.mode == 1) return StToU64(d.wall);
    SYSTEMTIME utc{};
    if (d.mode == 2) { TIME_ZONE_INFORMATION tzi = d.tzi; TzSpecificLocalTimeToSystemTime(&tzi, &d.wall, &utc); }
    else TzSpecificLocalTimeToSystemTime(nullptr, &d.wall, &utc);
    return StToU64(utc);
}
inline uint64_t WallToUtc(const SYSTEMTIME& wall, const Dt& frame) {
    SYSTEMTIME utc{};
    if (frame.mode == 2) { TIME_ZONE_INFORMATION tzi = frame.tzi; TzSpecificLocalTimeToSystemTime(&tzi, &wall, &utc); }
    else if (frame.mode == 1) utc = wall;
    else TzSpecificLocalTimeToSystemTime(nullptr, &wall, &utc);
    return StToU64(utc);
}
inline SYSTEMTIME UtcToUserLocal(uint64_t utc) { SYSTEMTIME u = U64ToSt(utc), local{}; SystemTimeToTzSpecificLocalTime(nullptr, &u, &local); return local; }

struct RRule {
    std::string freq; int interval = 1; int count = -1; uint64_t until = 0; bool hasUntil = false;
    std::vector<std::pair<int, int>> byday; std::vector<int> bymonthday; std::vector<int> bymonth;
};
inline int WeekdayCode(const std::string& s) {
    static const char* names[] = {"SU", "MO", "TU", "WE", "TH", "FR", "SA"};
    for (int i = 0; i < 7; ++i) if (s == names[i]) return i;
    return -1;
}
inline RRule ParseRRule(const std::string& v) {
    RRule r; size_t i = 0;
    while (i < v.size()) {
        size_t semi = v.find(';', i);
        std::string tok = v.substr(i, (semi == std::string::npos ? std::string::npos : semi - i));
        size_t eq = tok.find('=');
        if (eq != std::string::npos) {
            std::string k = Upper(tok.substr(0, eq)), val = tok.substr(eq + 1);
            if (k == "FREQ") r.freq = Upper(val);
            else if (k == "INTERVAL") r.interval = std::max(1, atoi(val.c_str()));
            else if (k == "COUNT") r.count = atoi(val.c_str());
            else if (k == "UNTIL") { Dt d = ParseDt(val, {}); if (d.valid) { r.until = (d.mode == 1) ? StToU64(d.wall) : ToUtc(d); r.hasUntil = true; } }
            else if (k == "BYDAY") {
                size_t p = 0;
                while (p < val.size()) {
                    size_t c = val.find(',', p);
                    std::string item = val.substr(p, (c == std::string::npos ? std::string::npos : c - p));
                    int ord = 0; size_t q = 0;
                    if (!item.empty() && (item[0] == '+' || item[0] == '-' || isdigit((unsigned char)item[0]))) {
                        int sign = 1; if (item[0] == '+') q = 1; else if (item[0] == '-') { sign = -1; q = 1; }
                        int num = 0; while (q < item.size() && isdigit((unsigned char)item[q])) { num = num * 10 + (item[q] - '0'); ++q; }
                        ord = sign * num;
                    }
                    int wd = WeekdayCode(item.substr(q));
                    if (wd >= 0) r.byday.push_back({ord, wd});
                    if (c == std::string::npos) break;
                    p = c + 1;
                }
            } else if (k == "BYMONTHDAY") { size_t p = 0; while (p < val.size()) { size_t c = val.find(',', p); r.bymonthday.push_back(atoi(val.substr(p, c - p).c_str())); if (c == std::string::npos) break; p = c + 1; } }
            else if (k == "BYMONTH") { size_t p = 0; while (p < val.size()) { size_t c = val.find(',', p); r.bymonth.push_back(atoi(val.substr(p, c - p).c_str())); if (c == std::string::npos) break; p = c + 1; } }
        }
        if (semi == std::string::npos) break;
        i = semi + 1;
    }
    return r;
}
inline int DaysInMonth(int y, int m) {
    static const int d[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)) return 29;
    return d[(m - 1) % 12];
}
inline int NthWeekdayOfMonth(int y, int m, int ord, int wd) {
    int dim = DaysInMonth(y, m); std::vector<int> days;
    for (int d = 1; d <= dim; ++d) { SYSTEMTIME st{}; st.wYear = (WORD)y; st.wMonth = (WORD)m; st.wDay = (WORD)d; st.wHour = 12; if (DayOfWeek(st) == wd) days.push_back(d); }
    if (days.empty()) return -1;
    if (ord > 0 && ord <= (int)days.size()) return days[ord - 1];
    if (ord < 0 && -ord <= (int)days.size()) return days[days.size() + ord];
    return -1;
}
struct RawEvent {
    std::string uid, summary, location, status;
    Dt dtstart, dtend; bool hasRRule = false; RRule rrule;
    std::vector<uint64_t> exdates; bool hasRecurId = false; uint64_t recurId = 0; bool hasDtEnd = false;
};

}  // namespace ics_detail

inline std::vector<IcsEvent> ParseIcsEvents(const std::string& icsUtf8, const SYSTEMTIME& windowStartLocal, int days) {
    using namespace ics_detail;
    std::vector<IcsEvent> result;
    SYSTEMTIME ws = windowStartLocal; ws.wHour = ws.wMinute = ws.wSecond = ws.wMilliseconds = 0;
    SYSTEMTIME we = AddDays(ws, days);
    uint64_t winStartUtc, winEndUtc;
    { SYSTEMTIME u{}; TzSpecificLocalTimeToSystemTime(nullptr, &ws, &u); winStartUtc = StToU64(u); }
    { SYSTEMTIME u{}; TzSpecificLocalTimeToSystemTime(nullptr, &we, &u); winEndUtc = StToU64(u); }

    auto lines = Unfold(icsUtf8);
    std::vector<RawEvent> raws; RawEvent cur; bool in = false;
    for (auto& ln : lines) {
        if (ln == "BEGIN:VEVENT") { in = true; cur = RawEvent(); continue; }
        if (ln == "END:VEVENT") { if (in) raws.push_back(cur); in = false; continue; }
        if (!in) continue;
        Prop p = SplitProp(ln);
        if (p.name == "UID") cur.uid = p.value;
        else if (p.name == "SUMMARY") cur.summary = p.value;
        else if (p.name == "LOCATION") cur.location = p.value;
        else if (p.name == "STATUS") cur.status = Upper(p.value);
        else if (p.name == "DTSTART") cur.dtstart = ParseDt(p.value, p.params);
        else if (p.name == "DTEND") { cur.dtend = ParseDt(p.value, p.params); cur.hasDtEnd = true; }
        else if (p.name == "RRULE") { cur.rrule = ParseRRule(p.value); cur.hasRRule = true; }
        else if (p.name == "RECURRENCE-ID") { Dt d = ParseDt(p.value, p.params); cur.recurId = ToUtc(d); cur.hasRecurId = true; }
        else if (p.name == "EXDATE") {
            size_t q = 0;
            while (q < p.value.size()) {
                size_t c = p.value.find(',', q);
                std::string item = p.value.substr(q, (c == std::string::npos ? std::string::npos : c - q));
                Dt d = ParseDt(item, p.params);
                if (d.valid) cur.exdates.push_back(ToUtc(d));
                if (c == std::string::npos) break;
                q = c + 1;
            }
        }
    }

    std::unordered_map<std::string, std::unordered_set<uint64_t>> overrides;
    auto emit = [&](const RawEvent& e, uint64_t startUtc, uint64_t durTicks) {
        if (startUtc >= winEndUtc || (startUtc + durTicks) <= winStartUtc) return;
        IcsEvent ev;
        ev.subject = UnescapeText(e.summary);
        ev.location = UnescapeText(e.location);
        ev.allDay = e.dtstart.dateOnly;
        ev.start = UtcToUserLocal(startUtc);
        ev.end = UtcToUserLocal(startUtc + durTicks);
        ev.uid = Utf8ToW(e.uid);
        result.push_back(ev);
    };

    for (auto& e : raws) {
        if (!e.dtstart.valid) continue;
        uint64_t startUtc = ToUtc(e.dtstart);
        uint64_t durTicks;
        if (e.hasDtEnd && e.dtend.valid) { uint64_t endUtc = ToUtc(e.dtend); durTicks = (endUtc > startUtc) ? (endUtc - startUtc) : (e.dtstart.dateOnly ? 86400ULL * 10000000ULL : 3600ULL * 10000000ULL); }
        else durTicks = e.dtstart.dateOnly ? 86400ULL * 10000000ULL : 3600ULL * 10000000ULL;
        if (e.hasRecurId) { overrides[e.uid].insert(e.recurId); if (e.status != "CANCELLED") emit(e, startUtc, durTicks); continue; }
        if (!e.hasRRule) { emit(e, startUtc, durTicks); continue; }
    }

    for (auto& e : raws) {
        if (!e.hasRRule || e.hasRecurId || !e.dtstart.valid) continue;
        uint64_t startUtc0 = ToUtc(e.dtstart);
        uint64_t durTicks;
        if (e.hasDtEnd && e.dtend.valid) { uint64_t eu = ToUtc(e.dtend); durTicks = eu > startUtc0 ? eu - startUtc0 : 3600ULL * 10000000ULL; }
        else durTicks = e.dtstart.dateOnly ? 86400ULL * 10000000ULL : 3600ULL * 10000000ULL;
        const RRule& r = e.rrule;
        const SYSTEMTIME base = e.dtstart.wall;
        std::unordered_set<uint64_t> exset(e.exdates.begin(), e.exdates.end());
        auto& ovset = overrides[e.uid];
        int emitted = 0, guard = 0; const int kCap = 1500;
        auto consider = [&](const SYSTEMTIME& wall) -> bool {
            uint64_t occUtc = WallToUtc(wall, e.dtstart);
            if (r.hasUntil && occUtc > r.until) return false;
            if (occUtc >= winEndUtc) return false;
            ++emitted;
            if (r.count >= 0 && emitted > r.count) return false;
            if (occUtc + durTicks > winStartUtc) { if (!exset.count(occUtc) && !ovset.count(occUtc)) emit(e, occUtc, durTicks); }
            return true;
        };
        if (r.freq == "DAILY") {
            SYSTEMTIME w = base; while (guard++ < kCap) { if (!consider(w)) break; w = AddDays(w, r.interval); }
        } else if (r.freq == "WEEKLY") {
            std::vector<int> wds; for (auto& bd : r.byday) wds.push_back(bd.second);
            if (wds.empty()) wds.push_back(DayOfWeek(base));
            std::sort(wds.begin(), wds.end());
            SYSTEMTIME weekStart = AddDays(base, -DayOfWeek(base)); bool stop = false;
            while (!stop && guard++ < kCap) {
                for (int wd : wds) {
                    SYSTEMTIME occ = AddDays(weekStart, wd);
                    occ.wHour = base.wHour; occ.wMinute = base.wMinute; occ.wSecond = base.wSecond;
                    if (StToU64(occ) < StToU64(base)) continue;
                    if (!consider(occ)) { stop = true; break; }
                }
                weekStart = AddDays(weekStart, 7 * r.interval);
            }
        } else if (r.freq == "MONTHLY") {
            int y = base.wYear, m = base.wMonth;
            while (guard++ < kCap) {
                bool stop = false; std::vector<int> dom;
                if (!r.bymonthday.empty()) dom = r.bymonthday;
                else if (!r.byday.empty()) { for (auto& bd : r.byday) { int d = NthWeekdayOfMonth(y, m, bd.first ? bd.first : 1, bd.second); if (d > 0) dom.push_back(d); } }
                else dom.push_back(base.wDay);
                std::sort(dom.begin(), dom.end());
                for (int d : dom) { if (d < 1 || d > DaysInMonth(y, m)) continue; SYSTEMTIME occ = base; occ.wYear = (WORD)y; occ.wMonth = (WORD)m; occ.wDay = (WORD)d; if (StToU64(occ) < StToU64(base)) continue; if (!consider(occ)) { stop = true; break; } }
                if (stop) break;
                m += r.interval; while (m > 12) { m -= 12; ++y; }
            }
        } else if (r.freq == "YEARLY") {
            int y = base.wYear;
            while (guard++ < kCap) {
                bool stop = false; std::vector<int> months = r.bymonth.empty() ? std::vector<int>{base.wMonth} : r.bymonth;
                for (int m : months) {
                    std::vector<int> dom;
                    if (!r.bymonthday.empty()) dom = r.bymonthday;
                    else if (!r.byday.empty()) { for (auto& bd : r.byday) { int d = NthWeekdayOfMonth(y, m, bd.first ? bd.first : 1, bd.second); if (d > 0) dom.push_back(d); } }
                    else dom.push_back(base.wDay);
                    for (int d : dom) { if (d < 1 || d > DaysInMonth(y, m)) continue; SYSTEMTIME occ = base; occ.wYear = (WORD)y; occ.wMonth = (WORD)m; occ.wDay = (WORD)d; if (StToU64(occ) < StToU64(base)) continue; if (!consider(occ)) { stop = true; break; } }
                    if (stop) break;
                }
                if (stop) break;
                y += r.interval;
            }
        } else {
            if (startUtc0 + durTicks > winStartUtc && startUtc0 < winEndUtc) emit(e, startUtc0, durTicks);
        }
    }

    std::sort(result.begin(), result.end(), [](const IcsEvent& a, const IcsEvent& b) {
        FILETIME fa, fb; SystemTimeToFileTime(&a.start, &fa); SystemTimeToFileTime(&b.start, &fb);
        LONG c = CompareFileTime(&fa, &fb); if (c != 0) return c < 0; return a.uid < b.uid;
    });
    result.erase(std::unique(result.begin(), result.end(), [](const IcsEvent& a, const IcsEvent& b) {
        FILETIME fa, fb; SystemTimeToFileTime(&a.start, &fa); SystemTimeToFileTime(&b.start, &fb);
        return !a.uid.empty() && a.uid == b.uid && CompareFileTime(&fa, &fb) == 0;
    }), result.end());
    return result;
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------
enum class Position { TopCenter, TopLeft, TopRight, BottomCenter };
enum class ThemeMode { Auto, Dark, Light, Adaptive };

// Backdrop presets, selected by an INLINE STYLE STRING (the `BackdropStyle` setting), e.g.
// "$Acrylic" or "$Glass TintColor=#3388CC TintOpacity=0.5" — same $Name + Key=Value convention as
// windows-11-taskbar-styler. Styles use the WindowGlass theme's blur radii verbatim (Translucent =
// 15px · Glass = 5px · Frosted = 20px · Acrylic = 30px, +1.25 saturation). REAL blur comes from a
// companion Windows.UI.Composition window whose CompositionBackdropBrush samples the live desktop
// content behind the island — the same brush the WindowGlass taskbar theme blurs with — run
// through our GaussianBlur effect at the preset radius (see BackdropBlurHost below). The tint
// stays in this D2D panel fill, drawn over the blur. (HostBackdropBrush was tried first and
// empirically only delivers a wallpaper-layer snapshot on build 26200 — windows behind never
// appear — so the plain backdrop brush is the correct source.) When blur is unavailable
// (composition init failed, transparency effects off) the denser fallback tint keeps the panel
// legible — the old tint-only behavior.
//
// Tint defaults to the live Windows accent color (TintColor/FallbackColor = "accent", the
// default) rather than a fixed hue, so the island matches the user's theme; ResolveAccentColors()
// fills r/g/b or fr/fg/fb from a cached UISettings lookup wherever the corresponding *UsesAccent
// flag is set. An explicit TintColor=/FallbackColor= (hex) clears that flag for that channel.
struct BackdropPreset {
    float r, g, b, a;      // panel tint while real blur is live (light: the blur is the material)
    float fr, fg, fb, fa;  // fallback tint when blur is unavailable (dense: tint IS the material)
    float blur;            // gaussian blur radius, logical px (WindowGlass BlurAmount)
    float saturation;      // 1.0 = none (Acrylic adds the canonical 1.25 boost)
    float focusA = -1.0f;  // hover/open tint opacity override; -1 = auto (see ApplyStateOpacity)
    bool liveUsesAccent = false;
    bool fallbackUsesAccent = false;
    bool liveUsesTheme = false;
    bool fallbackUsesTheme = false;
};
// Named style bases: (blur, saturation, live alpha, fallback alpha). Color is accent-derived
// unless overridden. NOTE on alphas: the WindowGlass TintOpacity values (0.7/0.7/0.8) were
// designed for a taskbar strip over wallpaper and read as a SOLID slab on a floating panel — an
// 0.8 tint over a 30px blur compresses a full white->black swing behind the panel into ~25
// luminance points (measured). The alphas below are retuned so content visibly swims through
// while keeping the WindowGlass blur radii and clearest->densest ordering.
struct BackdropStyleBase { float blur, saturation, liveAlpha, fallbackAlpha; };
bool LookupBackdropStyleBase(const std::wstring& name, BackdropStyleBase& out) {
    struct Entry { const wchar_t* name; BackdropStyleBase base; };
    static const Entry kStyles[] = {
        //  name             blur   sat    liveA  fallbackA
        {L"Translucent", {  15.0f, 1.00f, 0.063f, 0.40f}},
        {L"Glass",       {   5.0f, 1.00f, 0.42f,  0.55f}},
        {L"Frosted",     {  20.0f, 1.00f, 0.48f,  0.62f}},
        {L"Acrylic",     {  30.0f, 1.18f, 0.34f,  0.70f}},
    };
    for (auto& e : kStyles) {
        if (EqualsNoCase(name, e.name)) { out = e.base; return true; }
    }
    return false;
}
// Parses a hex color into r/g/b/a (0..1). Accepts "#RRGGBB" or "#AARRGGBB" (leading '#'
// optional); 6 digits leave *outA untouched (caller keeps the preset's existing alpha) since only
// an explicit 8-digit value or a separate *Opacity= token should set alpha. Returns false (leaves
// outputs untouched) on a malformed value so a typo can't silently zero out a color.
bool ParseHexColor(const std::wstring& text, float* outR, float* outG, float* outB, float* outA) {
    const wchar_t* p = text.c_str();
    if (*p == L'#') ++p;
    size_t len = wcslen(p);
    if (len != 6 && len != 8) return false;
    for (size_t i = 0; i < len; ++i) if (!iswxdigit(p[i])) return false;
    wchar_t* end = nullptr;
    unsigned long v = wcstoul(p, &end, 16);
    if (!end || *end != L'\0') return false;
    if (len == 8) { *outA = ((v >> 24) & 0xFF) / 255.0f; v &= 0xFFFFFF; }
    *outR = ((v >> 16) & 0xFF) / 255.0f;
    *outG = ((v >> 8) & 0xFF) / 255.0f;
    *outB = (v & 0xFF) / 255.0f;
    return true;
}
// Parses the BackdropStyle setting string into a resolved-recipe preset + whether the backdrop
// (tint + real blur) is enabled at all. Grammar: "[$Name] [Key=Value ...]" — an optional
// $None/$Translucent/$Glass/$Frosted/$Acrylic token (leading '$' optional) picks the base blur/
// saturation/alpha profile (default base if omitted: Acrylic's), followed by any number of
// space-separated overrides, applied left-to-right (last write wins): BlurAmount, TintSaturation,
// TintColor (accent|#RRGGBB|#AARRGGBB), TintOpacity, FallbackColor, FallbackOpacity. An empty
// string or "None"/"$None" disables the backdrop entirely (today's plain solid panel).
void ParseBackdropStyle(const std::wstring& raw, BackdropPreset& outPreset, bool& outEnabled) {
    std::wstring trimmed = raw;
    while (!trimmed.empty() && iswspace(trimmed.front())) trimmed.erase(trimmed.begin());
    while (!trimmed.empty() && iswspace(trimmed.back())) trimmed.pop_back();
    if (trimmed.empty() || EqualsNoCase(trimmed, L"None") || EqualsNoCase(trimmed, L"$None")) {
        outEnabled = false;
        // BeginFrame still reads fr/fg/fb/fa for the (always-fallback, blur never live) panel
        // fill when disabled -- leaving this zero-initialized renders a fully transparent panel.
        // liveUsesTheme/fallbackUsesTheme=true so even the plain solid panel follows the Theme
        // setting (ResolveBackdropColors overwrites r/g/b + fr/fg/fb below; alpha is untouched).
        outPreset = {0.043f, 0.043f, 0.051f, 0.94f, 0.043f, 0.043f, 0.051f, 0.94f,
                     0.0f, 1.0f, -1.0f, false, false, true, true};
        return;
    }
    outEnabled = true;

    // Tokenize on whitespace.
    std::vector<std::wstring> tokens;
    { std::wstring cur;
      for (wchar_t c : trimmed) {
          if (iswspace(c)) { if (!cur.empty()) { tokens.push_back(cur); cur.clear(); } }
          else cur.push_back(c);
      }
      if (!cur.empty()) tokens.push_back(cur); }

    // Base: a recognized $Name (leading '$' optional), else default to Acrylic's profile so a
    // bare "BlurAmount=20" line is still meaningful on its own.
    BackdropStyleBase base;
    size_t next = 0;
    std::wstring first = tokens.empty() ? L"" : tokens[0];
    if (!first.empty() && first[0] == L'$') first.erase(first.begin());
    if (!tokens.empty() && LookupBackdropStyleBase(first, base)) {
        next = 1;
    } else {
        LookupBackdropStyleBase(L"Acrylic", base);
    }
    outPreset.blur = base.blur;
    outPreset.saturation = base.saturation;
    outPreset.a = base.liveAlpha;
    outPreset.fa = base.fallbackAlpha;
    outPreset.focusA = -1.0f;
    // Default tint follows the Theme setting (a neutral shade), not the accent color — "accent"
    // and hex colors remain available as explicit overrides below.
    outPreset.liveUsesTheme = true;
    outPreset.fallbackUsesTheme = true;

    for (size_t i = next; i < tokens.size(); ++i) {
        size_t eq = tokens[i].find(L'=');
        if (eq == std::wstring::npos) {
            Wh_Log(L"BackdropStyle: ignoring malformed token '%s' (expected Key=Value)", tokens[i].c_str());
            continue;
        }
        std::wstring key = tokens[i].substr(0, eq);
        std::wstring val = tokens[i].substr(eq + 1);
        if (EqualsNoCase(key, L"BlurAmount")) {
            outPreset.blur = std::max(0.0f, static_cast<float>(_wtof(val.c_str())));
        } else if (EqualsNoCase(key, L"TintSaturation")) {
            outPreset.saturation = static_cast<float>(_wtof(val.c_str()));
        } else if (EqualsNoCase(key, L"TintOpacity")) {
            outPreset.a = Clamp(static_cast<float>(_wtof(val.c_str())), 0.0f, 1.0f);
        } else if (EqualsNoCase(key, L"FallbackOpacity")) {
            outPreset.fa = Clamp(static_cast<float>(_wtof(val.c_str())), 0.0f, 1.0f);
        } else if (EqualsNoCase(key, L"FocusOpacity")) {
            outPreset.focusA = Clamp(static_cast<float>(_wtof(val.c_str())), 0.0f, 1.0f);
        } else if (EqualsNoCase(key, L"TintColor")) {
            if (EqualsNoCase(val, L"theme")) { outPreset.liveUsesTheme = true; outPreset.liveUsesAccent = false; }
            else if (EqualsNoCase(val, L"accent")) { outPreset.liveUsesAccent = true; outPreset.liveUsesTheme = false; }
            else if (ParseHexColor(val, &outPreset.r, &outPreset.g, &outPreset.b, &outPreset.a))
                { outPreset.liveUsesAccent = false; outPreset.liveUsesTheme = false; }
            else Wh_Log(L"BackdropStyle: bad TintColor '%s'", val.c_str());
        } else if (EqualsNoCase(key, L"FallbackColor")) {
            if (EqualsNoCase(val, L"theme")) { outPreset.fallbackUsesTheme = true; outPreset.fallbackUsesAccent = false; }
            else if (EqualsNoCase(val, L"accent")) { outPreset.fallbackUsesAccent = true; outPreset.fallbackUsesTheme = false; }
            else if (ParseHexColor(val, &outPreset.fr, &outPreset.fg, &outPreset.fb, &outPreset.fa))
                { outPreset.fallbackUsesAccent = false; outPreset.fallbackUsesTheme = false; }
            else Wh_Log(L"BackdropStyle: bad FallbackColor '%s'", val.c_str());
        } else {
            Wh_Log(L"BackdropStyle: unknown attribute '%s'", key.c_str());
        }
    }
}
// Per-state transparency: lerps the tint alpha from its "rest" value toward a denser "focus"
// value as `mix` (0 = rest, 1 = focus) rises, so hovering/opening the panel makes content behind
// it easier to read. `preset.focusA` (the FocusOpacity= key) overrides the focus target for the
// live tint; if unset, focus defaults to rest+0.25 (capped at 0.92). The fallback tint (already
// dense) gets a smaller +0.10 bump, capped at 0.95.
void ApplyStateOpacity(BackdropPreset& preset, float mix) {
    const float focusLive = preset.focusA >= 0.0f ? preset.focusA : std::min(preset.a + 0.25f, 0.92f);
    const float focusFallback = std::min(preset.fa + 0.10f, 0.95f);
    preset.a += (focusLive - preset.a) * mix;
    preset.fa += (focusFallback - preset.fa) * mix;
}
bool g_backdropBlurLive = false;  // render-thread only: did BackdropBlurHost deliver real blur this frame?

struct Settings {
    Position position = Position::TopCenter;
    int targetMonitor = 0;
    std::wstring targetMonitorName;  // non-empty overrides targetMonitor; see FindMonitorByName
    int offsetX = 0;
    int offsetY = 0;
    float sizeScale = 1.0f;
    bool autoDpiScale = true;
    bool alwaysOnTop = true;
    float pillOpacity = 1.0f;
    ThemeMode themeMode = ThemeMode::Auto;
    bool backdropEnabled = false;
    BackdropPreset backdrop{};  // parsed recipe; accent-flagged channels resolved per-frame
    std::wstring calendarIcsUrl;
    int calendarRefreshMinutes = 20;
    bool brightnessEnabled = true;
    std::wstring weatherCity;
    bool weatherFahrenheit = true;
    bool captureVolumeKeys = true;
};

Settings g_settings;
std::mutex g_settingsMutex;
std::atomic<bool> g_captureVolKeys{true};  // lock-free mirror of Settings.captureVolumeKeys for the LL key hook
std::atomic<int> g_themeMode{static_cast<int>(ThemeMode::Auto)};  // lock-free mirror for LuminanceThreadProc

void LoadSettings() {
    Settings next;
    const std::wstring pos = GetStringSettingCopy(L"Position");
    if (EqualsNoCase(pos, L"top-left")) next.position = Position::TopLeft;
    else if (EqualsNoCase(pos, L"top-right")) next.position = Position::TopRight;
    else if (EqualsNoCase(pos, L"bottom-center")) next.position = Position::BottomCenter;
    else next.position = Position::TopCenter;

    std::wstring mon = GetStringSettingCopy(L"TargetMonitor");
    while (!mon.empty() && iswspace(mon.front())) mon.erase(mon.begin());
    while (!mon.empty() && iswspace(mon.back())) mon.pop_back();
    next.targetMonitor = 0;
    next.targetMonitorName.clear();
    if (!mon.empty()) {
        wchar_t* end = nullptr;
        long parsed = wcstol(mon.c_str(), &end, 10);
        if (end != mon.c_str() && *end == L'\0') next.targetMonitor = static_cast<int>(parsed);
        else next.targetMonitorName = mon;  // not a clean integer -> match by monitor name instead
    }
    next.offsetX = Wh_GetIntSetting(L"OffsetX");
    next.offsetY = Wh_GetIntSetting(L"OffsetY");
    const std::wstring scale = GetStringSettingCopy(L"SizeScale");
    next.sizeScale = scale.empty() ? 1.0f : Clamp(static_cast<float>(_wtof(scale.c_str())), 0.6f, 3.0f);
    next.autoDpiScale = Wh_GetIntSetting(L"AutoDpiScale") != 0;
    next.alwaysOnTop = Wh_GetIntSetting(L"AlwaysOnTop") != 0;
    const std::wstring opacity = GetStringSettingCopy(L"PillOpacity");
    next.pillOpacity = opacity.empty() ? 1.0f : Clamp(static_cast<float>(_wtof(opacity.c_str())), 0.35f, 1.0f);
    const std::wstring theme = GetStringSettingCopy(L"Theme");
    if (EqualsNoCase(theme, L"dark")) next.themeMode = ThemeMode::Dark;
    else if (EqualsNoCase(theme, L"light")) next.themeMode = ThemeMode::Light;
    else if (EqualsNoCase(theme, L"adaptive")) next.themeMode = ThemeMode::Adaptive;
    else next.themeMode = ThemeMode::Auto;
    g_themeMode = static_cast<int>(next.themeMode);  // lock-free mirror for the luminance sampler
    ParseBackdropStyle(GetStringSettingCopy(L"BackdropStyle"), next.backdrop, next.backdropEnabled);
    next.calendarIcsUrl = GetStringSettingCopy(L"CalendarIcsUrl");
    next.calendarRefreshMinutes = ClampInt(Wh_GetIntSetting(L"CalendarRefreshMinutes"), 5, 240);
    next.brightnessEnabled = Wh_GetIntSetting(L"BrightnessEnabled") != 0;
    next.weatherCity = GetStringSettingCopy(L"WeatherCity");
    next.weatherFahrenheit = Wh_GetIntSetting(L"WeatherFahrenheit") != 0;
    next.captureVolumeKeys = Wh_GetIntSetting(L"CaptureVolumeKeys") != 0;
    g_captureVolKeys = next.captureVolumeKeys;  // lock-free mirror for the key hook

    std::lock_guard lock(g_settingsMutex);
    g_settings = std::move(next);
}
Settings SettingsSnapshot() { std::lock_guard lock(g_settingsMutex); return g_settings; }

// ---------------------------------------------------------------------------
// Spring
// ---------------------------------------------------------------------------
struct SpringValue {
    float value = 0.0f, velocity = 0.0f, target = 0.0f;
    void Reset(float v) { value = target = v; velocity = 0.0f; }
    void Step(float dt, float stiffness, float damping) {
        const float disp = value - target;
        velocity += (-stiffness * disp - damping * velocity) * dt;
        value += velocity * dt;
        if (std::fabs(value - target) < 0.05f && std::fabs(velocity) < 0.05f) { value = target; velocity = 0.0f; }
    }
    bool AtRest() const { return value == target && velocity == 0.0f; }
};

// ---------------------------------------------------------------------------
// Shared interactive state (render thread + WndProc both on the render thread)
// ---------------------------------------------------------------------------
std::atomic<float> g_volScalar{0.0f};
std::atomic<float> g_volDisplayScalar{0.0f};  // smoothed level the volume popup animates toward
std::atomic<bool>  g_volMuted{false};
std::atomic<bool>  g_volValid{false};
double g_volSuppressPollUntil = 0.0;  // render-thread only


HWND g_hwnd = nullptr;
RECT g_pillRect = {};  // screen rect of the VISIBLE panel (not the window) — drives hover detection
RECT g_volumeHotspotRect = {};  // screen rect of the mini volume element; empty when not Collapsed.
                                 // Render-thread only (write + read), same as g_pillRect's un-locked read.
SRWLOCK g_pillRectLock = SRWLOCK_INIT;  // guards cross-thread reads (luminance sampler); render thread is the only writer and reads it un-locked
std::atomic<float> g_behindLum{-1.0f};  // mean luma [0..1] of the desktop around the panel; < 0 = no sample yet
HANDLE g_stopEvent = nullptr;
HANDLE g_renderThread = nullptr;
HANDLE g_hardwareThread = nullptr;

// Live system telemetry for the Control Center: CPU, RAM, and aggregate network throughput.
std::atomic<float> g_hwCpu{0.0f};
std::atomic<float> g_hwRam{0.0f};
std::atomic<double> g_hwNetDown{0.0}; // bytes/sec
std::atomic<double> g_hwNetUp{0.0};   // bytes/sec
std::atomic<uint64_t> g_hwGeneration{0};
HANDLE g_notifThread = nullptr;
std::atomic<bool> g_layoutDirty{true};
std::atomic<double> g_lastInputSec{-100.0};  // last pointer input; drives event-based repaint
std::atomic<bool> g_waveWanted{false};       // true only when the live waveform is on screen
bool g_pinExpanded = false;                 // render-thread only: keep the island expanded
constexpr double kHideSeconds = 3.0;  // auto-hide delay after the cursor leaves the island/reveal corner
double g_hideUntil = 0.0;              // render-thread only: auto-hide deadline (NowSeconds())
std::atomic<uint64_t> g_islandRevealSerial{0};

// Any island-connected transient/state change increments this serial. The render thread
// consumes it and reveals the island for the normal 3-second window.
void RevealIslandForEvent() { g_islandRevealSerial.fetch_add(1, std::memory_order_release); g_layoutDirty = true; }
HHOOK g_dismissHook = nullptr;               // WH_MOUSE_LL: click-outside dismiss (on the input thread)
std::atomic<bool> g_dismissActive{false};    // only act on the mouse hook while the panel is Open
// The window doesn't resize/reposition to its Open dimensions atomically with g_dismissActive
// flipping true -- there's a short window where the hook thread's GetWindowRect (in LlMouseProc)
// can still see the OLD (small, pre-Open) rect and misjudge the very click that opened the panel
// as "outside" it. Suppress outside-click dismissal for a brief grace period after opening.
std::atomic<double> g_dismissArmedAt{0.0};
constexpr double kDismissGraceSeconds = 0.25;
HHOOK g_keyHook = nullptr;                    // WH_KEYBOARD_LL: volume-key capture
HANDLE g_inputHookThread = nullptr;          // dedicated thread hosting BOTH low-level hooks
DWORD g_inputHookTid = 0;

void TriggerNudge() {
    static std::atomic<double> last{-1.0};
    const double now = NowSeconds();
    if (now - last.load() < 0.10) return;
    last = now;
    if (g_hwnd) PostMessageW(g_hwnd, WM_APP_NEW_EVENT, 0, 0);
}

// ---------------------------------------------------------------------------
// Default audio device change watcher
// ---------------------------------------------------------------------------
// IAudioEndpointVolume/IAudioMeterInformation are bound to a SPECIFIC endpoint at Activate() time
// and stay perfectly "valid" even after that endpoint stops being the system default -- so naive
// caching (the original code) silently keeps controlling/metering whatever device WAS default
// when it first connected, forever, even after the user switches output (e.g. connects
// Bluetooth/AirPods and Windows flips the default render device). This tiny IMMNotificationClient
// bumps a shared epoch on every default-render-device change; VolumeController and the audio
// meter thread each compare against their own last-seen epoch and drop+re-resolve their cached
// endpoint when it moves, at no extra cost on the (overwhelmingly common) unchanged case.
std::atomic<uint64_t> g_audioDeviceEpoch{0};

class AudioDeviceWatcher : public IMMNotificationClient {
   public:
    // Static-lifetime singleton (registered once, unregistered explicitly in VolumeController::
    // Shutdown); refcounting is a formality the audio service expects, not real lifetime control.
    ULONG STDMETHODCALLTYPE AddRef() override { return 1; }
    ULONG STDMETHODCALLTYPE Release() override { return 1; }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv) override {
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IMMNotificationClient)) {
            *ppv = static_cast<IMMNotificationClient*>(this);
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole, LPCWSTR) override {
        if (flow == eRender) g_audioDeviceEpoch.fetch_add(1, std::memory_order_relaxed);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override { return S_OK; }
};
AudioDeviceWatcher g_audioWatcher;

// ---------------------------------------------------------------------------
// Volume controller (synchronous; push-callbacks added in a later stage)
// ---------------------------------------------------------------------------
class VolumeController {
   public:
    bool Get(float& scalar, bool& muted) {
        if (!Ensure()) return false;
        float lvl = 0.0f; BOOL mu = FALSE;
        if (SUCCEEDED(vol_->GetMasterVolumeLevelScalar(&lvl)) && SUCCEEDED(vol_->GetMute(&mu))) {
            scalar = lvl; muted = mu != FALSE; return true;
        }
        vol_.Reset(); device_.Reset();
        return false;
    }
    void Set(float scalar) {
        if (!Ensure()) return;
        vol_->SetMasterVolumeLevelScalar(Clamp(scalar, 0.0f, 1.0f), nullptr);
    }
    void SetMute(bool muted) {
        if (!Ensure()) return;
        vol_->SetMute(muted ? TRUE : FALSE, nullptr);
    }
    // Unregister the device-change callback before the mod unloads -- a notification firing into
    // an unloaded DLL would crash the host process.
    void Shutdown() {
        if (enum_) enum_->UnregisterEndpointNotificationCallback(&g_audioWatcher);
        vol_.Reset(); device_.Reset(); enum_.Reset();
    }
   private:
    bool Ensure() {
        const uint64_t epoch = g_audioDeviceEpoch.load(std::memory_order_relaxed);
        if (vol_ && epoch == lastEpoch_) return true;
        vol_.Reset(); device_.Reset();  // stale endpoint (or first run) -- re-resolve the CURRENT default
        lastEpoch_ = epoch;
        if (!enum_) {
            if (FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                        IID_PPV_ARGS(&enum_)))) return false;
            enum_->RegisterEndpointNotificationCallback(&g_audioWatcher);
        }
        if (FAILED(enum_->GetDefaultAudioEndpoint(eRender, eConsole, &device_))) return false;
        if (FAILED(device_->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr,
                                     reinterpret_cast<void**>(vol_.GetAddressOf())))) return false;
        return true;
    }
    ComPtr<IMMDeviceEnumerator> enum_;
    ComPtr<IMMDevice> device_;
    ComPtr<IAudioEndpointVolume> vol_;
    uint64_t lastEpoch_ = 0;
};
VolumeController g_volume;

// Unified volume-set / mute-toggle path. Every caller that changes system volume -- the
// Open-panel slider, the pill's mini volume element (drag/click), its scroll-wheel gesture, and
// captured hardware volume keys -- funnels through these two, so there is exactly one place that
// updates state, suppresses the native/PollVolume OSD, and pushes to the OS.
void SetSystemVolume(float v) {
    v = Clamp(v, 0.0f, 1.0f);
    g_volScalar = v;
    g_volMuted = false;
    g_volSuppressPollUntil = NowSeconds() + 0.4;  // avoid a duplicate popup from PollVolume
    g_volume.Set(v);
    RevealIslandForEvent();
    g_layoutDirty = true;
}
void ToggleSystemMute() {
    float cur = 0.0f; bool mut = false;
    g_volume.Get(cur, mut);
    bool nm = !mut;
    g_volume.SetMute(nm);
    g_volMuted = nm;
    RevealIslandForEvent();
    g_volSuppressPollUntil = NowSeconds() + 0.4;
    g_layoutDirty = true;
}

void ShowVolumeTransient(int pct, bool muted);  // defined in the transient section below

void PollVolume() {
    float s = 0.0f; bool m = false;
    if (g_volume.Get(s, m)) {
        if (NowSeconds() >= g_volSuppressPollUntil) {  // ignore our own drag echo
            float prev = g_volScalar.load();
            bool pm = g_volMuted.load();
            g_volScalar = s;
            g_volMuted = m;
            int pct = (int)(s * 100.0f + 0.5f);
            static int lastPct = -1, lastMuted = -1;
            if (lastPct >= 0 && (std::abs(pct - lastPct) >= 1 || (int)m != lastMuted)) {
                ShowVolumeTransient(pct, m);   // external change -> transient pill
                RevealIslandForEvent();
            }
            lastPct = pct; lastMuted = (int)m;
            if (std::fabs(prev - s) > 0.001f || pm != m) g_layoutDirty = true;
        }
        g_volValid = true;
    }
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// Live hardware/network telemetry
// ---------------------------------------------------------------------------
namespace {
struct CpuSample { ULONGLONG idle = 0, kernel = 0, user = 0; };

CpuSample ReadCpuSample() {
    FILETIME idleFt{}, kernelFt{}, userFt{};
    CpuSample s{};
    if (GetSystemTimes(&idleFt, &kernelFt, &userFt)) {
        ULARGE_INTEGER i{}, k{}, u{};
        i.LowPart = idleFt.dwLowDateTime; i.HighPart = idleFt.dwHighDateTime;
        k.LowPart = kernelFt.dwLowDateTime; k.HighPart = kernelFt.dwHighDateTime;
        u.LowPart = userFt.dwLowDateTime; u.HighPart = userFt.dwHighDateTime;
        s.idle = i.QuadPart; s.kernel = k.QuadPart; s.user = u.QuadPart;
    }
    return s;
}

void ReadMemoryUsage(float& usedPct) {
    MEMORYSTATUSEX ms{};
    ms.dwLength = sizeof(ms);
    if (!GlobalMemoryStatusEx(&ms) || ms.ullTotalPhys == 0) { usedPct = 0.0f; return; }
    const double used = static_cast<double>(ms.ullTotalPhys - ms.ullAvailPhys);
    usedPct = static_cast<float>(std::min(std::max(static_cast<float>(used * 100.0 / static_cast<double>(ms.ullTotalPhys)), 0.0f), 100.0f));
}

void ReadNetworkCounters(ULONGLONG& rx, ULONGLONG& tx) {
    rx = tx = 0;

    // Windhawk's bundled MinGW headers expose the older MIB_IFTABLE API reliably,
    // while MIB_IF_TABLE2/GetIfTable2 are not available in this compiler setup.
    // Use the compatible API so the mod compiles on the user's Windhawk toolchain.
    DWORD size = 0;
    if (GetIfTable(nullptr, &size, FALSE) != ERROR_INSUFFICIENT_BUFFER || size == 0)
        return;

    std::vector<BYTE> buffer(size);
    PMIB_IFTABLE table = reinterpret_cast<PMIB_IFTABLE>(buffer.data());
    if (GetIfTable(table, &size, FALSE) != NO_ERROR)
        return;

    for (DWORD i = 0; i < table->dwNumEntries; ++i) {
        const MIB_IFROW& row = table->table[i];

        // Only count interfaces that are operational. Ignore loopback/tunnel
        // interfaces where the legacy API exposes the corresponding type.
        if (row.dwOperStatus != IF_OPER_STATUS_OPERATIONAL) continue;
        if (row.dwType == IF_TYPE_SOFTWARE_LOOPBACK || row.dwType == IF_TYPE_TUNNEL)
            continue;

        rx += static_cast<ULONGLONG>(row.dwInOctets);
        tx += static_cast<ULONGLONG>(row.dwOutOctets);
    }
}

DWORD WINAPI HardwareThreadProc(void*) {
    CpuSample prevCpu = ReadCpuSample();
    ULONGLONG prevRx = 0, prevTx = 0;
    ReadNetworkCounters(prevRx, prevTx);

    while (WaitForSingleObject(g_stopEvent, 1000) == WAIT_TIMEOUT) {
        CpuSample curCpu = ReadCpuSample();
        const ULONGLONG idleDelta = curCpu.idle - prevCpu.idle;
        const ULONGLONG kernelDelta = curCpu.kernel - prevCpu.kernel;
        const ULONGLONG userDelta = curCpu.user - prevCpu.user;
        const ULONGLONG totalDelta = kernelDelta + userDelta;
        if (totalDelta > 0) {
            const double busy = 1.0 - static_cast<double>(idleDelta) / static_cast<double>(totalDelta);
            g_hwCpu = static_cast<float>(Clamp(busy * 100.0, 0.0, 100.0));
        }
        prevCpu = curCpu;

        float ram = 0.0f;
        ReadMemoryUsage(ram);
        g_hwRam = ram;

        ULONGLONG rx = 0, tx = 0;
        ReadNetworkCounters(rx, tx);
        if (rx >= prevRx) g_hwNetDown = static_cast<double>(rx - prevRx);
        if (tx >= prevTx) g_hwNetUp = static_cast<double>(tx - prevTx);
        prevRx = rx; prevTx = tx;

        // Publish telemetry without forcing a full layered-window repaint.
        // The render thread consumes this generation only while the island is visible.
        g_hwGeneration.fetch_add(1, std::memory_order_release);
    }
    return 0;
}

std::wstring FormatRate(double bytesPerSec) {
    if (bytesPerSec < 1024.0) return std::to_wstring((int)std::round(bytesPerSec)) + L" B/s";
    const double kb = bytesPerSec / 1024.0;
    if (kb < 1024.0) return std::to_wstring((int)std::round(kb)) + L" KB/s";
    const double mb = kb / 1024.0;
    wchar_t buf[32]{};
    swprintf_s(buf, L"%.1f MB/s", mb);
    return buf;
}
}  // namespace

// Notification center data layer (UserNotificationListener)
// ---------------------------------------------------------------------------
struct NotificationItem {
    uint32_t id = 0;
    std::wstring app, title, body, aumid;
};

std::mutex g_notifMutex;
std::vector<NotificationItem> g_notifs;          // newest-first (listener order)
std::unordered_set<uint32_t> g_notifSuppressed;  // locally dismissed ids
std::atomic<bool> g_notifSupported{false};
std::atomic<uint64_t> g_notifGen{0};             // bumps on any list change (add/clear/dismiss) -> rebuild Open panel

std::vector<NotificationItem> NotificationsSnapshot() {
    std::lock_guard lock(g_notifMutex);
    return g_notifs;
}

#if ICC_HAS_NOTIFICATION_LISTENER
void DismissNotification(uint32_t id) {
    {
        std::lock_guard lock(g_notifMutex);
        g_notifSuppressed.insert(id);
        g_notifs.erase(std::remove_if(g_notifs.begin(), g_notifs.end(),
                       [id](const NotificationItem& n) { return n.id == id; }),
                       g_notifs.end());
    }
    std::thread([id] {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        try {
            winrt::Windows::UI::Notifications::Management::UserNotificationListener::Current()
                .RemoveNotification(id);
        } catch (...) {}
        winrt::uninit_apartment();
    }).detach();
    g_notifGen.fetch_add(1);
    g_layoutDirty = true;
}

void ClearAllNotifications() {
    std::vector<uint32_t> ids;
    {
        std::lock_guard lock(g_notifMutex);
        for (auto& n : g_notifs) { ids.push_back(n.id); g_notifSuppressed.insert(n.id); }
        g_notifs.clear();
    }
    std::thread([ids] {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        try {
            auto l = winrt::Windows::UI::Notifications::Management::UserNotificationListener::Current();
            for (uint32_t id : ids) { try { l.RemoveNotification(id); } catch (...) {} }
        } catch (...) {}
        winrt::uninit_apartment();
    }).detach();
    g_notifGen.fetch_add(1);
    g_layoutDirty = true;
}

DWORD WINAPI NotificationThreadProc(void*) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
    using namespace winrt::Windows::UI::Notifications;
    using namespace winrt::Windows::UI::Notifications::Management;
    try {
        auto listener = UserNotificationListener::Current();
        if (listener.RequestAccessAsync().get() != UserNotificationListenerAccessStatus::Allowed) {
            g_notifSupported = false;
            winrt::uninit_apartment();
            return 0;
        }
        g_notifSupported = true;
        uint64_t lastSig = 0;
        while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
            try {
                auto list = listener.GetNotificationsAsync(NotificationKinds::Toast).get();
                std::vector<NotificationItem> items;
                std::unordered_set<uint32_t> present;
                uint64_t sig = 0;
                for (uint32_t i = 0; i < list.Size(); ++i) {
                    auto un = list.GetAt(i);
                    uint32_t id = un.Id();
                    present.insert(id);
                    {
                        std::lock_guard lock(g_notifMutex);
                        if (g_notifSuppressed.count(id)) continue;
                    }
                    NotificationItem item;
                    item.id = id;
                    try {
                        auto info = un.AppInfo();
                        item.app = info.DisplayInfo().DisplayName().c_str();
                        item.aumid = info.AppUserModelId().c_str();
                    } catch (...) {}
                    try {
                        auto binding = un.Notification().Visual().GetBinding(KnownNotificationBindings::ToastGeneric());
                        if (binding) {
                            auto texts = binding.GetTextElements();
                            if (texts.Size() > 0) item.title = texts.GetAt(0).Text().c_str();
                            if (texts.Size() > 1) item.body = texts.GetAt(1).Text().c_str();
                        }
                    } catch (...) {}
                    if (item.title.empty()) item.title = item.app.empty() ? L"Notification" : item.app;
                    sig = sig * 1000003u + (id + 1u);
                    items.push_back(std::move(item));
                }
                {
                    std::lock_guard lock(g_notifMutex);
                    for (auto it = g_notifSuppressed.begin(); it != g_notifSuppressed.end();) {
                        if (!present.count(*it)) it = g_notifSuppressed.erase(it); else ++it;
                    }
                    g_notifs = std::move(items);
                }
                if (sig != lastSig) { lastSig = sig; g_notifGen.fetch_add(1); TriggerNudge(); g_layoutDirty = true; }
            } catch (...) {}
            WaitForSingleObject(g_stopEvent, 1000);
        }
    } catch (...) {
        g_notifSupported = false;
    }
    winrt::uninit_apartment();
    return 0;
}
#endif  // ICC_HAS_NOTIFICATION_LISTENER

void LaunchByAumid(const std::wstring& aumid) {
    if (aumid.empty()) return;
    std::wstring path = L"shell:AppsFolder\\" + aumid;
    ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

// ---------------------------------------------------------------------------
// Calendar data layer (Outlook published .ics over WinHTTP)
// ---------------------------------------------------------------------------
std::mutex g_calMutex;
std::vector<IcsEvent> g_calEvents;
std::atomic<uint64_t> g_calGen{0};  // bumps whenever g_calEvents is replaced (cache invalidation)
std::atomic<bool> g_calLoaded{false};
std::atomic<bool> g_calError{false};
std::atomic<bool> g_calConfigured{false};
// View/selection state (interaction-owned; persists across root rebuilds). 0 = uninit.
std::atomic<int> g_calViewYear{0};
std::atomic<int> g_calViewMonth{0};
std::atomic<int> g_calSelYear{0};
std::atomic<int> g_calSelMonth{0};
std::atomic<int> g_calSelDay{0};
HANDLE g_calThread = nullptr;
HANDLE g_calRefreshEvent = nullptr;

std::vector<IcsEvent> CalendarSnapshot() { std::lock_guard lock(g_calMutex); return g_calEvents; }

std::string HttpGetUtf8(const std::wstring& url) {
    URL_COMPONENTS uc{}; uc.dwStructSize = sizeof(uc);
    wchar_t host[512] = {}, path[8192] = {}, extra[4096] = {};
    uc.lpszHostName = host; uc.dwHostNameLength = ARRAYSIZE(host);
    uc.lpszUrlPath = path; uc.dwUrlPathLength = ARRAYSIZE(path);
    uc.lpszExtraInfo = extra; uc.dwExtraInfoLength = ARRAYSIZE(extra);
    if (!WinHttpCrackUrl(url.c_str(), 0, 0, &uc)) return "";
    host[std::min<DWORD>(uc.dwHostNameLength, ARRAYSIZE(host) - 1)] = 0;
    path[std::min<DWORD>(uc.dwUrlPathLength, ARRAYSIZE(path) - 1)] = 0;
    extra[std::min<DWORD>(uc.dwExtraInfoLength, ARRAYSIZE(extra) - 1)] = 0;
    std::wstring fullPath = std::wstring(path) + extra;  // path + query (?format=j1 etc.)
    bool https = (uc.nScheme == INTERNET_SCHEME_HTTPS);
    HINTERNET hSession = WinHttpOpen(L"IslandCommandCenter/1.0", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                                     WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return "";
    std::string result;
    if (HINTERNET hConnect = WinHttpConnect(hSession, host, uc.nPort, 0)) {
        DWORD flags = https ? WINHTTP_FLAG_SECURE : 0;
        if (HINTERNET hReq = WinHttpOpenRequest(hConnect, L"GET", fullPath.c_str(), nullptr, WINHTTP_NO_REFERER,
                                                WINHTTP_DEFAULT_ACCEPT_TYPES, flags)) {
            if (WinHttpSendRequest(hReq, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
                WinHttpReceiveResponse(hReq, nullptr)) {
                for (;;) {
                    DWORD avail = 0;
                    if (!WinHttpQueryDataAvailable(hReq, &avail) || avail == 0) break;
                    std::string chunk(avail, '\0');
                    DWORD read = 0;
                    if (WinHttpReadData(hReq, chunk.data(), avail, &read) && read) { chunk.resize(read); result += chunk; }
                    else break;
                }
            }
            WinHttpCloseHandle(hReq);
        }
        WinHttpCloseHandle(hConnect);
    }
    WinHttpCloseHandle(hSession);
    return result;
}

DWORD WINAPI CalendarThreadProc(void*) {
    HANDLE waits[2] = {g_stopEvent, g_calRefreshEvent};
    for (;;) {
        Settings s = SettingsSnapshot();
        g_calConfigured = !s.calendarIcsUrl.empty();
        if (g_calConfigured.load()) {
            std::string ics = HttpGetUtf8(s.calendarIcsUrl);
            if (!ics.empty() && ics.find("BEGIN:VCALENDAR") != std::string::npos) {
                SYSTEMTIME now; GetLocalTime(&now);
                SYSTEMTIME firstOfMonth = now; firstOfMonth.wDay = 1;
                SYSTEMTIME w0 = ics_detail::AddDays(firstOfMonth, -45);
                auto evs = ParseIcsEvents(ics, w0, 210);
                { std::lock_guard lock(g_calMutex); g_calEvents = std::move(evs); }
                g_calGen.fetch_add(1);  // invalidate agenda/calendar caches
                g_calLoaded = true; g_calError = false;
            } else {
                g_calError = true; g_calLoaded = true;
            }
            TriggerNudge(); g_layoutDirty = true;
        }
        int mins = std::max(5, SettingsSnapshot().calendarRefreshMinutes);
        DWORD wr = WaitForMultipleObjects(2, waits, FALSE, (DWORD)mins * 60000);
        if (wr == WAIT_OBJECT_0) break;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Media (GSMTC) + album art + live audio waveform
// ---------------------------------------------------------------------------
struct BitmapPixels { int w = 0, h = 0; std::vector<uint8_t> bgra; uint64_t gen = 0; };
std::atomic<uint64_t> g_artGen{0};

std::vector<uint8_t> ReadWinRtStreamBytes(winrt::Windows::Storage::Streams::IRandomAccessStreamReference ref) {
    using namespace winrt::Windows::Storage::Streams;
    std::vector<uint8_t> out;
    try {
        auto stream = ref.OpenReadAsync().get();
        uint64_t size = stream.Size();
        if (!size || size > 8u * 1024 * 1024) return out;
        DataReader reader(stream);
        reader.LoadAsync((uint32_t)size).get();
        out.resize((size_t)size);
        reader.ReadBytes(winrt::array_view<uint8_t>(out.data(), out.data() + out.size()));
    } catch (...) {}
    return out;
}

bool DecodeImageBytesToPixels(const std::vector<uint8_t>& bytes, BitmapPixels* out) {
    if (bytes.empty() || !out) return false;
    ComPtr<IWICImagingFactory> f;
    if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&f)))) return false;
    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, bytes.size());
    if (!mem) return false;
    void* p = GlobalLock(mem); memcpy(p, bytes.data(), bytes.size()); GlobalUnlock(mem);
    ComPtr<IStream> s;
    if (FAILED(CreateStreamOnHGlobal(mem, TRUE, &s))) { GlobalFree(mem); return false; }
    ComPtr<IWICBitmapDecoder> dec;
    if (FAILED(f->CreateDecoderFromStream(s.Get(), nullptr, WICDecodeMetadataCacheOnLoad, &dec))) return false;
    ComPtr<IWICBitmapFrameDecode> fr;
    if (FAILED(dec->GetFrame(0, &fr))) return false;
    ComPtr<IWICFormatConverter> cv;
    if (FAILED(f->CreateFormatConverter(&cv))) return false;
    if (FAILED(cv->Initialize(fr.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom))) return false;
    UINT w = 0, h = 0; cv->GetSize(&w, &h);
    if (!w || !h || w > 2048 || h > 2048) return false;
    out->w = (int)w; out->h = (int)h; out->bgra.resize((size_t)w * h * 4);
    if (FAILED(cv->CopyPixels(nullptr, w * 4, (UINT)out->bgra.size(), out->bgra.data()))) return false;
    out->gen = ++g_artGen;
    return true;
}

struct MediaState {
    bool active = false, playing = false;
    std::wstring title, artist, aumid, sourceName;
    int64_t posTicks = 0, endTicks = 0;  // 100ns
    ULONGLONG capturedTick = 0;
    BitmapPixels art;
};
// Maps a well-known AUMID to a friendly display name; empty if unrecognized (caller omits it).
std::wstring FriendlyMediaSourceName(const std::wstring& aumid) {
    auto has = [&](const wchar_t* needle) { return aumid.find(needle) != std::wstring::npos; };
    if (has(L"Spotify")) return L"Spotify";
    if (has(L"ZuneMusic") || has(L"Microsoft.Media.Player")) return L"Media Player";
    if (has(L"MicrosoftEdge") || has(L"MSEdge")) return L"Edge";
    if (has(L"Chrome")) return L"Chrome";
    if (has(L"Firefox")) return L"Firefox";
    if (has(L"VLC")) return L"VLC";
    if (has(L"AppleMusic") || has(L"iTunes")) return L"Apple Music";
    return L"";
}
std::mutex g_mediaMutex;
MediaState g_media;
MediaState MediaSnapshot() { std::lock_guard l(g_mediaMutex); return g_media; }

constexpr int kWaveN = 56;
std::mutex g_waveMutex;
std::array<float, kWaveN> g_wave{};
int g_waveWrite = 0;
void WavePush(float v) { std::lock_guard l(g_waveMutex); g_wave[g_waveWrite % kWaveN] = v; ++g_waveWrite; }
std::array<float, kWaveN> WaveSnapshot(int& writeOut) { std::lock_guard l(g_waveMutex); writeOut = g_waveWrite; return g_wave; }

HANDLE g_mediaThread = nullptr;
HANDLE g_audioThread = nullptr;

DWORD WINAPI AudioThreadProc(void*) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    ComPtr<IMMDeviceEnumerator> en; ComPtr<IMMDevice> dev; ComPtr<IAudioMeterInformation> meter;
    uint64_t lastEpoch = 0;
    auto ensure = [&]() -> bool {
        // Re-resolve against the CURRENT default render device whenever it changes (see
        // AudioDeviceWatcher) -- otherwise the meter keeps reading whatever device WAS default
        // (e.g. laptop speakers) even after the user switches to AirPods/Bluetooth, showing a dead
        // waveform because that old device is no longer receiving any audio.
        const uint64_t epoch = g_audioDeviceEpoch.load(std::memory_order_relaxed);
        if (meter && epoch == lastEpoch) return true;
        meter.Reset(); dev.Reset();
        lastEpoch = epoch;
        if (!en && FAILED(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&en)))) return false;
        if (FAILED(en->GetDefaultAudioEndpoint(eRender, eConsole, &dev))) return false;
        if (FAILED(dev->Activate(ICC_IID_IAudioMeterInformation, CLSCTX_ALL, nullptr, (void**)meter.GetAddressOf()))) { dev.Reset(); return false; }
        return true;
    };
    while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        // Only sample the meter fast when the waveform is actually visible; otherwise idle so we
        // don't wake ~22x/second forever feeding a strip nobody is looking at.
        bool wanted = g_waveWanted.load();
        float peak = 0.0f;
        if (wanted) {
            if (ensure()) { if (FAILED(meter->GetPeakValue(&peak))) { meter.Reset(); dev.Reset(); peak = 0.0f; } }
            WavePush(Clamp(peak, 0.0f, 1.0f));
        }
        WaitForSingleObject(g_stopEvent, wanted ? 45 : 400);
    }
    CoUninitialize();
    return 0;
}

// Picks which GSMTC session represents "now playing": a session that's actually Playing first (so
// an idle browser tab's media session can't shadow something genuinely playing, e.g. Spotify),
// else the session the OS considers current, else whatever's first available. Shared by the
// snapshot poll and MediaCommand so both always agree on which app is being reported/controlled.
winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSession PickMediaSession(
    const winrt::Windows::Media::Control::GlobalSystemMediaTransportControlsSessionManager& mgr) {
    using namespace winrt::Windows::Media::Control;
    using PS = GlobalSystemMediaTransportControlsSessionPlaybackStatus;
    if (!mgr) return nullptr;
    auto sessions = mgr.GetSessions();
    for (auto const& sess : sessions)
        if (sess.GetPlaybackInfo().PlaybackStatus() == PS::Playing) return sess;
    if (auto cur = mgr.GetCurrentSession()) return cur;
    for (auto const& sess : sessions) return sess;
    return nullptr;
}

DWORD WINAPI MediaThreadProc(void*) {
    winrt::init_apartment(winrt::apartment_type::multi_threaded);
    using namespace winrt::Windows::Media::Control;
    using PS = GlobalSystemMediaTransportControlsSessionPlaybackStatus;
    GlobalSystemMediaTransportControlsSessionManager mgr{nullptr};
    while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        MediaState next;
        try {
            if (!mgr) mgr = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            if (mgr) {
                auto s = PickMediaSession(mgr);
                if (s) {
                    auto props = s.TryGetMediaPropertiesAsync().get();
                    auto pb = s.GetPlaybackInfo();
                    auto tl = s.GetTimelineProperties();
                    next.active = true;
                    next.playing = pb.PlaybackStatus() == PS::Playing;
                    next.title = props.Title().c_str();
                    next.artist = props.Artist().c_str();
                    next.aumid = s.SourceAppUserModelId().c_str();
                    next.sourceName = FriendlyMediaSourceName(next.aumid);
                    if (tl) {
                        next.posTicks = tl.Position().count();
                        next.endTicks = tl.EndTime().count();
                        // GSMTC Position is the position at LastUpdatedTime, not necessarily at
                        // the instant we queried it. Compensate for that latency while playing.
                        if (next.playing) {
                            try {
                                auto age = winrt::clock::now() - tl.LastUpdatedTime();
                                auto extra = std::chrono::duration_cast<std::chrono::duration<int64_t, std::ratio<1, 10000000>>>(age).count();
                                if (extra > 0) next.posTicks += extra;
                            } catch (...) {}
                        }
                        next.posTicks = std::max<int64_t>(0, next.posTicks);
                        if (next.endTicks > 0) next.posTicks = std::min(next.posTicks, next.endTicks);
                        next.capturedTick = GetTickCount64();
                    }
                    if (auto th = props.Thumbnail()) {
                        auto bytes = ReadWinRtStreamBytes(th);
                        if (!bytes.empty()) { BitmapPixels d; if (DecodeImageBytesToPixels(bytes, &d)) next.art = std::move(d); }
                    }
                }
            }
        } catch (...) {}
        bool changed;
        {
            std::lock_guard l(g_mediaMutex);
            changed = next.active != g_media.active || next.title != g_media.title ||
                      next.artist != g_media.artist || next.playing != g_media.playing;
            if (next.art.bgra.empty() && next.title == g_media.title && next.artist == g_media.artist) next.art = g_media.art;
            g_media = std::move(next);
        }
        if (changed) { RevealIslandForEvent(); TriggerNudge(); g_layoutDirty = true; }
        WaitForSingleObject(g_stopEvent, 1000);
    }
    winrt::uninit_apartment();
    return 0;
}

void MediaCommand(int cmd) {  // 0 prev, 1 toggle, 2 next
    RevealIslandForEvent();
    std::thread([cmd] {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        try {
            using namespace winrt::Windows::Media::Control;
            auto mgr = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            auto s = PickMediaSession(mgr);  // same selection as the poll -- always targets what's shown
            if (s) { if (cmd == 0) s.TrySkipPreviousAsync().get(); else if (cmd == 1) s.TryTogglePlayPauseAsync().get(); else if (cmd == 2) s.TrySkipNextAsync().get(); }
        } catch (...) {}
        winrt::uninit_apartment();
    }).detach();
}

void MediaSeek(double fraction) {
    std::wstring aumid; int64_t endTicks;
    { std::lock_guard l(g_mediaMutex); aumid = g_media.aumid; endTicks = g_media.endTicks; }
    if (endTicks <= 0) return;
    int64_t target = (int64_t)(Clamp((float)fraction, 0.0f, 1.0f) * (float)endTicks);
    std::thread([aumid, target] {
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        try {
            using namespace winrt::Windows::Media::Control;
            auto mgr = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
            if (mgr) {
                auto s = mgr.GetCurrentSession();
                for (auto const& sess : mgr.GetSessions())
                    if (std::wstring(sess.SourceAppUserModelId().c_str()) == aumid) { s = sess; break; }
                if (s) s.TryChangePlaybackPositionAsync(target).get();
            }
        } catch (...) {}
        winrt::uninit_apartment();
    }).detach();
}

// ---------------------------------------------------------------------------
// Weather (wttr.in over WinHTTP)
// ---------------------------------------------------------------------------
struct WeatherState {
    bool hasData = false;
    int temp = 0;
    int code = 0;
    int feelsLike = 0;
    int humidity = 0;
    int windSpeed = 0;          // mph (Fahrenheit) or km/h (Celsius)
    std::wstring windDir;       // 16-point compass, e.g. "SSW"
    std::wstring desc, city;
    double updated = 0.0;
};
std::mutex g_weatherMutex;
WeatherState g_weather;
HANDLE g_weatherThread = nullptr;
WeatherState WeatherSnapshot() { std::lock_guard l(g_weatherMutex); return g_weather; }

std::wstring WeatherGlyph(int code) {
    switch (code) {
        case 113: return L"☀️";                                   // sunny
        case 116: return L"⛅";                                         // partly cloudy
        case 119: case 122: return L"☁️";                         // cloudy
        case 143: case 248: case 260: return L"\U0001F32B️";           // fog
        case 200: case 386: case 389: case 392: case 395: return L"⛈️"; // thunder
    }
    if (code >= 281 && code <= 377) return L"\U0001F327️";             // rain
    if (code >= 179 && code <= 230) return L"❄️";                 // snow
    if (code >= 320 && code <= 395) return L"❄️";                 // snow
    return L"\U0001F321️";                                            // thermometer
}

// Extract a JSON string value for `key` after position `from`, tolerating
// pretty-printed whitespace: "key" <ws> : <ws> "value". (wttr.in j1 is pretty-printed.)
static std::string JsonStr(const std::string& s, size_t from, const char* key) {
    std::string needle = std::string("\"") + key + "\"";
    size_t p = s.find(needle, from);
    if (p == std::string::npos) return "";
    p = s.find(':', p + needle.size());
    if (p == std::string::npos) return "";
    ++p;
    while (p < s.size() && (s[p] == ' ' || s[p] == '\t' || s[p] == '\r' || s[p] == '\n')) ++p;
    if (p >= s.size() || s[p] != '"') return "";  // only string values
    ++p;
    size_t e = s.find('"', p);
    return (e == std::string::npos) ? "" : s.substr(p, e - p);
}

DWORD WINAPI WeatherThreadProc(void*) {
    WaitForSingleObject(g_stopEvent, 2500);
    while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        Settings s = SettingsSnapshot();
        std::wstring url = L"https://wttr.in/";
        for (wchar_t c : s.weatherCity) url += (c == L' ') ? L'+' : c;
        url += L"?format=j1";
        std::string j = HttpGetUtf8(url);
        size_t cc = j.find("current_condition");
        if (cc != std::string::npos) {
            std::string tF = JsonStr(j, cc, "temp_F");
            std::string tC = JsonStr(j, cc, "temp_C");
            std::string code = JsonStr(j, cc, "weatherCode");
            size_t dpos = j.find("weatherDesc", cc);
            std::string desc = (dpos != std::string::npos) ? JsonStr(j, dpos, "value") : "";
            WeatherState w;
            w.hasData = !tF.empty() || !tC.empty();
            w.temp = atoi((s.weatherFahrenheit ? tF : tC).c_str());
            w.code = atoi(code.c_str());
            w.desc = ics_detail::Utf8ToW(desc);
            w.feelsLike = atoi(JsonStr(j, cc, s.weatherFahrenheit ? "FeelsLikeF" : "FeelsLikeC").c_str());
            w.humidity = atoi(JsonStr(j, cc, "humidity").c_str());
            w.windSpeed = atoi(JsonStr(j, cc, s.weatherFahrenheit ? "windspeedMiles" : "windspeedKmph").c_str());
            w.windDir = ics_detail::Utf8ToW(JsonStr(j, cc, "winddir16Point"));
            size_t na = j.find("nearest_area");
            if (na != std::string::npos) { size_t ap = j.find("areaName", na); if (ap != std::string::npos) w.city = ics_detail::Utf8ToW(JsonStr(j, ap, "value")); }
            w.updated = NowSeconds();
            { std::lock_guard l(g_weatherMutex); g_weather = w; }
            TriggerNudge(); g_layoutDirty = true;
        }
        WaitForSingleObject(g_stopEvent, 15u * 60u * 1000u);
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Transient pills (volume / caps lock / clipboard / device) + privacy dots
// ---------------------------------------------------------------------------
struct Transient {
    int kind = 0;             // 0 none, 1 generic, 2 volume
    std::wstring glyph, line;
    int value = 0;            // volume percent (kind 2)
    bool muted = false;
    double expiresAt = 0.0;
};
std::mutex g_transMutex;
Transient g_trans;

void ShowTransient(const std::wstring& glyph, const std::wstring& line, double dur = 2.2) {
    { std::lock_guard l(g_transMutex); g_trans = {1, glyph, line, 0, false, NowSeconds() + dur}; }
    RevealIslandForEvent(); TriggerNudge(); g_layoutDirty = true;
}
void ShowVolumeTransient(int pct, bool muted) {
    { std::lock_guard l(g_transMutex); g_trans = {2, muted ? L"\xE74F" : L"\xE767", L"Volume", pct, muted, NowSeconds() + 1.8}; }
    RevealIslandForEvent(); TriggerNudge(); g_layoutDirty = true;
}
Transient TransientSnapshot() { std::lock_guard l(g_transMutex); return g_trans; }
bool TransientActive() { std::lock_guard l(g_transMutex); return g_trans.kind != 0 && NowSeconds() < g_trans.expiresAt; }

// Privacy (mic/camera in-use) via CapabilityAccessManager registry.
std::atomic<bool> g_micActive{false}, g_camActive{false};
HANDLE g_privacyThread = nullptr;

bool ProbeConsentStore(HKEY root, const std::wstring& path) {
    HKEY h;
    if (RegOpenKeyExW(root, path.c_str(), 0, KEY_READ, &h) != ERROR_SUCCESS) return false;
    bool inUse = false;
    wchar_t name[256];
    for (DWORD idx = 0; !inUse; ++idx) {
        DWORD nlen = 256;
        if (RegEnumKeyExW(h, idx, name, &nlen, nullptr, nullptr, nullptr, nullptr) != ERROR_SUCCESS) break;
        std::wstring child = path + L"\\" + name;
        if (wcscmp(name, L"NonPackaged") == 0) { if (ProbeConsentStore(root, child)) inUse = true; continue; }
        HKEY sub;
        if (RegOpenKeyExW(root, child.c_str(), 0, KEY_READ, &sub) == ERROR_SUCCESS) {
            ULONGLONG stop = 1; DWORD sz = sizeof(stop), tp = 0;
            if (RegQueryValueExW(sub, L"LastUsedTimeStop", nullptr, &tp, (LPBYTE)&stop, &sz) == ERROR_SUCCESS && stop == 0) inUse = true;
            RegCloseKey(sub);
        }
    }
    RegCloseKey(h);
    return inUse;
}
bool IsCapabilityInUse(const wchar_t* cap) {
    return ProbeConsentStore(HKEY_CURRENT_USER,
        std::wstring(L"Software\\Microsoft\\Windows\\CurrentVersion\\CapabilityAccessManager\\ConsentStore\\") + cap);
}
DWORD WINAPI PrivacyThreadProc(void*) {
    while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        bool mic = IsCapabilityInUse(L"microphone");
        bool cam = IsCapabilityInUse(L"webcam");
        if (mic != g_micActive.load() || cam != g_camActive.load()) { g_micActive = mic; g_camActive = cam; g_layoutDirty = true; }
        WaitForSingleObject(g_stopEvent, 1500);
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Backdrop luminance sampler (adaptive text theme)
// ---------------------------------------------------------------------------
// Samples the composed screen in thin strips just OUTSIDE the visible panel
// (left/right/below, clamped to its monitor) every ~300ms and publishes an
// EMA-smoothed mean luma. Strips, not the panel's own rect: capturing the
// panel would read back our own text/tint (a feedback loop that oscillates
// the theme), while the neighborhood is exactly what the 5-30px backdrop blur
// mixes into the panel anyway. The render thread composites this with the
// active tint and flips the foreground palette (see Renderer::BeginFrame).
HANDLE g_lumThread = nullptr;

void SampleRegionLuma(HDC screen, const RECT& r, double& sum, long& n) {
    const int w = r.right - r.left, h = r.bottom - r.top;
    if (w <= 0 || h <= 0) return;
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;  // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HDC mem = CreateCompatibleDC(screen);
    if (!mem) return;
    HBITMAP dib = CreateDIBSection(mem, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (dib && bits) {
        HGDIOBJ old = SelectObject(mem, dib);
        if (BitBlt(mem, 0, 0, w, h, screen, r.left, r.top, SRCCOPY)) {
            GdiFlush();
            const uint32_t* px = static_cast<const uint32_t*>(bits);
            for (int y = 0; y < h; y += 3) {
                for (int x = 0; x < w; x += 3) {
                    const uint32_t p = px[y * w + x];
                    sum += 0.299 * ((p >> 16) & 0xFF) + 0.587 * ((p >> 8) & 0xFF) + 0.114 * (p & 0xFF);
                    ++n;
                }
            }
        }
        SelectObject(mem, old);
    }
    if (dib) DeleteObject(dib);
    DeleteDC(mem);
}

DWORD WINAPI LuminanceThreadProc(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    float ema = -1.0f;
    while (WaitForSingleObject(g_stopEvent, 300) == WAIT_TIMEOUT) {
        // Only Theme=adaptive consumes this sample; idle (and clear any stale value) otherwise
        // so switching away from adaptive can't leave a frozen ghost reading behind.
        if (g_themeMode.load() != static_cast<int>(ThemeMode::Adaptive)) {
            if (ema >= 0.0f) { ema = -1.0f; g_behindLum = -1.0f; }
            continue;
        }
        AcquireSRWLockShared(&g_pillRectLock);
        const RECT pill = g_pillRect;
        ReleaseSRWLockShared(&g_pillRectLock);
        if (IsRectEmpty(&pill)) continue;
        MONITORINFO mi = {sizeof(mi)};
        if (!GetMonitorInfoW(MonitorFromRect(&pill, MONITOR_DEFAULTTONEAREST), &mi)) continue;
        const LONG gap = 10, thick = 26;
        const RECT strips[3] = {
            {pill.left - gap - thick, pill.top, pill.left - gap, pill.bottom},      // left of panel
            {pill.right + gap, pill.top, pill.right + gap + thick, pill.bottom},    // right of panel
            {pill.left, pill.bottom + gap, pill.right, pill.bottom + gap + thick},  // below panel
        };
        HDC screen = GetDC(nullptr);
        if (!screen) continue;
        double sum = 0.0;
        long n = 0;
        for (const RECT& s : strips) {
            RECT c;
            if (IntersectRect(&c, &s, &mi.rcMonitor)) SampleRegionLuma(screen, c, sum, n);
        }
        ReleaseDC(nullptr, screen);
        if (n < 300) continue;  // strips (almost) fully clipped -> keep the last published value
        const float mean = static_cast<float>(sum / (255.0 * n));
        ema = ema < 0.0f ? mean : ema + 0.35f * (mean - ema);
        g_behindLum = ema;
    }
    return 0;
}

// ===========================================================================
// Widget framework
// ===========================================================================
struct DrawContext {
    ID2D1DCRenderTarget* dc = nullptr;
    IDWriteFactory* dw = nullptr;
    float scale = 1.0f;
    double now = 0.0;
    // Cursor in client (widget) coordinates, set once per frame by the render loop -- lets
    // widgets paint hover states without a per-widget mouse-move routing mechanism.
    D2D1_POINT_2F cursor = D2D1::Point2F(-10000.0f, -10000.0f);
    bool CursorIn(const D2D1_RECT_F& r) const {
        return cursor.x >= r.left && cursor.x <= r.right && cursor.y >= r.top && cursor.y <= r.bottom;
    }

    // Brushes (owned by Renderer).
    ID2D1SolidColorBrush* text = nullptr;
    ID2D1SolidColorBrush* muted = nullptr;
    ID2D1SolidColorBrush* accent = nullptr;
    ID2D1SolidColorBrush* panel = nullptr;
    ID2D1SolidColorBrush* card = nullptr;
    ID2D1SolidColorBrush* border = nullptr;
    ID2D1SolidColorBrush* divider = nullptr;
    ID2D1SolidColorBrush* track = nullptr;

    // Text formats (owned by Renderer).
    IDWriteTextFormat* fHuge = nullptr;
    IDWriteTextFormat* fTitle = nullptr;
    IDWriteTextFormat* fBody = nullptr;
    IDWriteTextFormat* fSmall = nullptr;
    IDWriteTextFormat* fPill = nullptr;
    IDWriteTextFormat* fPillSub = nullptr;  // small bold sub-text under the pill's clock (the date)
    IDWriteTextFormat* fGlyph = nullptr;
    ID2D1Bitmap* artBmp = nullptr;

    float MeasureWidth(IDWriteTextFormat* fmt, const std::wstring& s) const {
        if (!fmt || s.empty()) return 0.0f;
        ComPtr<IDWriteTextLayout> layout;
        if (FAILED(dw->CreateTextLayout(s.c_str(), static_cast<UINT32>(s.size()), fmt,
                                        10000.0f, 200.0f, &layout)))
            return 0.0f;
        DWRITE_TEXT_METRICS m = {};
        layout->GetMetrics(&m);
        return m.widthIncludingTrailingWhitespace;
    }
    void Text(const std::wstring& s, IDWriteTextFormat* fmt, D2D1_RECT_F r,
              ID2D1SolidColorBrush* brush, float opacity = 1.0f,
              DWRITE_TEXT_ALIGNMENT hAlign = DWRITE_TEXT_ALIGNMENT_LEADING,
              DWRITE_PARAGRAPH_ALIGNMENT vAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
              D2D1_DRAW_TEXT_OPTIONS opt = D2D1_DRAW_TEXT_OPTIONS_NONE) const {
        if (s.empty() || !fmt || !brush) return;
        fmt->SetTextAlignment(hAlign);
        fmt->SetParagraphAlignment(vAlign);
        brush->SetOpacity(opacity);
        dc->DrawTextW(s.c_str(), static_cast<UINT32>(s.size()), fmt, r, brush, opt);
        brush->SetOpacity(1.0f);
    }
};

enum class PointerPhase { Move, Down, Up, Wheel };
struct PointerEvent {
    PointerPhase phase;
    D2D1_POINT_2F pos{};
    float wheel = 0.0f;
};

struct ScrollContainer;  // fwd

struct Widget {
    D2D1_RECT_F bounds{};
    bool visible = true;
    Widget* parent = nullptr;

    virtual ~Widget() = default;
    // Returns the height needed at the given width.
    virtual float Measure(const DrawContext& dc, float width) = 0;
    virtual float PreferredWidth(const DrawContext&) { return -1.0f; }
    virtual void Layout(D2D1_RECT_F b) { bounds = b; }
    virtual void Paint(DrawContext&) = 0;
    bool Contains(D2D1_POINT_2F p) const {
        return p.x >= bounds.left && p.x < bounds.right && p.y >= bounds.top && p.y < bounds.bottom;
    }
    virtual Widget* HitTestDeep(D2D1_POINT_2F p) {
        return (visible && Contains(p)) ? this : nullptr;
    }
    // Returns true if handled. Sets wantsCapture on a Down that begins a drag.
    virtual bool OnPointer(const PointerEvent&, bool& wantsCapture) { wantsCapture = false; return false; }
    virtual bool Tick(float) { return false; }  // returns animating
    virtual ScrollContainer* AsScroll() { return nullptr; }
};

float W(const D2D1_RECT_F& r) { return r.right - r.left; }
float H(const D2D1_RECT_F& r) { return r.bottom - r.top; }

// --- Custom (lambda-painted) leaf, used for the pill & expanded clock --------
struct Custom : Widget {
    std::function<float(const DrawContext&, float)> measure;  // optional
    std::function<float(const DrawContext&)> prefWidth;       // optional
    std::function<void(DrawContext&, D2D1_RECT_F)> paint;
    float fixedHeight = 0.0f;

    float Measure(const DrawContext& dc, float width) override {
        return measure ? measure(dc, width) : fixedHeight;
    }
    float PreferredWidth(const DrawContext& dc) override {
        return prefWidth ? prefWidth(dc) : -1.0f;
    }
    void Paint(DrawContext& dc) override { if (paint) paint(dc, bounds); }
};

// --- Label -------------------------------------------------------------------
struct Label : Widget {
    std::wstring text;
    IDWriteTextFormat* DrawContext::* fmtMember = &DrawContext::fBody;
    ID2D1SolidColorBrush* DrawContext::* brushMember = &DrawContext::text;
    float opacity = 1.0f;
    float height = 18.0f;
    float padL = 0.0f, padR = 0.0f;

    float Measure(const DrawContext& dc, float) override { return height * dc.scale; }
    void Paint(DrawContext& dc) override {
        D2D1_RECT_F r = bounds;
        r.left += padL * dc.scale; r.right -= padR * dc.scale;
        dc.Text(text, dc.*fmtMember, r, dc.*brushMember, opacity);
    }
};

// --- Slider (the capture case) ----------------------------------------------
struct Slider : Widget {
    std::function<float()> get;       // current 0..1
    std::function<void(float)> set;   // live during drag
    std::wstring label;
    std::wstring glyph;               // leading icon (Segoe Fluent Icons), optional
    IDWriteTextFormat* glyphFmt = nullptr;
    bool dragging = false;
    float height = 46.0f;

    float Measure(const DrawContext& dc, float) override { return height * dc.scale; }

    D2D1_RECT_F TrackRect(const DrawContext& dc) const {
        const float s = dc.scale;
        float left = bounds.left + 44.0f * s;
        float right = bounds.right - 16.0f * s;
        float cy = bounds.top + H(bounds) * 0.62f;
        return D2D1::RectF(left, cy - 3.0f * s, right, cy + 3.0f * s);
    }
    void Apply(const DrawContext& dc, float x) {
        D2D1_RECT_F t = TrackRect(dc);
        float v = (x - t.left) / std::max(1.0f, W(t));
        if (set) set(Clamp(v, 0.0f, 1.0f));
    }
    bool OnPointer(const PointerEvent& e, bool& wantsCapture) override {
        wantsCapture = false;
        // dc.scale isn't available here; recompute via bounds geometry below.
        if (e.phase == PointerPhase::Down) { dragging = true; wantsCapture = true; pendingX_ = e.pos.x; return true; }
        if (e.phase == PointerPhase::Move && dragging) { pendingX_ = e.pos.x; return true; }
        if (e.phase == PointerPhase::Up && dragging) { dragging = false; pendingX_ = e.pos.x; return true; }
        return false;
    }
    bool HasPending() const { return pendingX_ >= 0.0f; }
    float TakePending() { float x = pendingX_; pendingX_ = -1.0f; return x; }

    void Paint(DrawContext& dc) override {
        const float s = dc.scale;
        float value = get ? Clamp(get(), 0.0f, 1.0f) : 0.0f;
        // Icon
        if (!glyph.empty() && glyphFmt) {
            D2D1_RECT_F gr = D2D1::RectF(bounds.left + 8.0f * s, bounds.top,
                                         bounds.left + 40.0f * s, bounds.bottom);
            dc.Text(glyph, glyphFmt, gr, dc.text, 0.9f);
        }
        // Label + percent
        if (!label.empty()) {
            D2D1_RECT_F lr = D2D1::RectF(bounds.left + 44.0f * s, bounds.top + 4.0f * s,
                                         bounds.right - 16.0f * s, bounds.top + 22.0f * s);
            dc.Text(label, dc.fSmall, lr, dc.muted, 0.85f);
            wchar_t pct[8]; swprintf_s(pct, L"%d%%", (int)(value * 100.0f + 0.5f));
            dc.Text(pct, dc.fSmall, lr, dc.text, 0.9f, DWRITE_TEXT_ALIGNMENT_TRAILING);
        }
        // Track + fill + thumb
        D2D1_RECT_F t = TrackRect(dc);
        float rad = H(t) * 0.5f;
        dc.dc->FillRoundedRectangle(D2D1::RoundedRect(t, rad, rad), dc.track);
        D2D1_RECT_F fill = t; fill.right = t.left + W(t) * value;
        dc.dc->FillRoundedRectangle(D2D1::RoundedRect(fill, rad, rad), dc.accent);
        float thumbX = t.left + W(t) * value;
        float thumbR = 7.0f * s;
        dc.dc->FillEllipse(D2D1::Ellipse(D2D1::Point2F(thumbX, (t.top + t.bottom) * 0.5f), thumbR, thumbR), dc.text);
    }
   private:
    float pendingX_ = -1.0f;
};

// Forward declarations: definitions live near BuildRoot, alongside the rest of the pill-content
// helpers (TimeString/DatePillString/weather); CollapsedPill (below) needs them earlier.
std::wstring TimeString();
std::wstring DatePillString();
bool WeatherFresh();
std::wstring WeatherShort();

// Draws album art (or a placeholder glyph) clipped to a CIRCLE inscribed in `rect` -- shared by
// CollapsedPill, the Expanded hover-peek's now-playing row, and the Open panel's MediaCard, so
// there's exactly one place that decides how art is masked. Uses a bitmap brush + FillEllipse
// (cover-fit: scaled up and centered so the circle is fully covered, cropping any excess) rather
// than DrawBitmap, since DrawBitmap always paints a plain rectangle regardless of any fill shape.
void DrawCircularArt(DrawContext& dc, D2D1_RECT_F rect, IDWriteTextFormat* placeholderGlyphFmt) {
    const D2D1_POINT_2F center = D2D1::Point2F((rect.left + rect.right) * 0.5f, (rect.top + rect.bottom) * 0.5f);
    const float radius = std::min(W(rect), H(rect)) * 0.5f;
    if (dc.artBmp) {
        D2D1_SIZE_F sz = dc.artBmp->GetSize();
        if (sz.width > 0.0f && sz.height > 0.0f) {
            const float cover = std::max(W(rect) / sz.width, H(rect) / sz.height);
            D2D1::Matrix3x2F xform = D2D1::Matrix3x2F::Scale(cover, cover) *
                D2D1::Matrix3x2F::Translation(center.x - sz.width * cover * 0.5f,
                                              center.y - sz.height * cover * 0.5f);
            ComPtr<ID2D1BitmapBrush> brush;
            dc.dc->CreateBitmapBrush(
                dc.artBmp,
                D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP),
                D2D1::BrushProperties(1.0f, xform), &brush);
            if (brush) { dc.dc->FillEllipse(D2D1::Ellipse(center, radius, radius), brush.Get()); return; }
        }
    }
    dc.card->SetOpacity(0.15f);
    dc.dc->FillEllipse(D2D1::Ellipse(center, radius, radius), dc.card);
    dc.card->SetOpacity(1.0f);
    if (placeholderGlyphFmt) dc.Text(L"\xE8D6", placeholderGlyphFmt, rect, dc.muted, 0.6f, DWRITE_TEXT_ALIGNMENT_CENTER);
}

// Live-audio waveform strip: kWaveBars slim bars mirrored around the strip's vertical center
// (Dynamic Island "horizontal centered" style) with fully rounded caps. Each bar eases toward
// the newest meter samples per frame instead of snapping, so the strip breathes rather than
// flickers. `smooth` is the caller's per-bar easing state -- one array per on-screen strip, so
// multiple strips (collapsed pill + MediaCard) animate independently. Shared by every waveform
// in the mod so there's exactly one place that decides how audio activity looks.
constexpr int kWaveBars = 5;
void DrawWaveform(DrawContext& dc, D2D1_RECT_F wf, float (&smooth)[kWaveBars], bool playing) {
    int wcount; auto wave = WaveSnapshot(wcount);
    const float slotW = W(wf) / kWaveBars;
    const float barW = slotW * 0.5f;
    const float rad = barW * 0.5f;
    const float cy = (wf.top + wf.bottom) * 0.5f;
    for (int i = 0; i < kWaveBars; ++i) {
        int idx = wcount - kWaveBars + i;
        float target = idx >= 0 ? wave[((idx % kWaveN) + kWaveN) % kWaveN] : 0.0f;
        if (!playing) target *= 0.2f;
        smooth[i] += (target - smooth[i]) * 0.22f;  // per-frame ease at ~60fps
        const float v = Clamp(smooth[i], 0.0f, 1.0f);
        // Minimum half-height = the cap radius, so an idle bar is a perfect little pill dot
        // rather than a degenerate rounded rect.
        const float half = std::max(rad, v * H(wf) * 0.5f);
        const float x = wf.left + i * slotW + (slotW - barW) * 0.5f;
        dc.accent->SetOpacity(0.4f + 0.6f * v);
        dc.dc->FillRoundedRectangle(
            D2D1::RoundedRect(D2D1::RectF(x, cy - half, x + barW, cy + half), rad, rad), dc.accent);
    DrawGlossyIslandHighlight(rt, D2D1);
    }
    dc.accent->SetOpacity(1.0f);
}

// --- Study focus system -------------------------------------------------------
enum class StudyMode { None, Pomodoro, Stopwatch };
enum class PomodoroPhase { Focus, Break };
struct StudyDayTotals { double focusSeconds=0.0; double stopwatchSeconds=0.0; };
struct StudyRuntime {
    StudyMode mode=StudyMode::None; PomodoroPhase phase=PomodoroPhase::Focus; bool running=false;
    int focusMinutes=25, breakMinutes=5; double remaining=25.0*60.0, stopwatchElapsed=0.0;
    double lastTick=0.0, lastPersist=0.0; int lastDisplaySecond=-1; std::wstring runningDate;
};
std::map<std::wstring,StudyDayTotals> g_studyHistory; StudyRuntime g_study; bool g_studyLoaded=false;
std::wstring StudyFilePath(){ wchar_t b[MAX_PATH]={}; DWORD n=GetEnvironmentVariableW(L"LOCALAPPDATA",b,ARRAYSIZE(b)); if(!n||n>=ARRAYSIZE(b)) return L"study-history.txt"; std::wstring d=std::wstring(b)+L"\\IslandCommandCenter"; CreateDirectoryW(d.c_str(),nullptr); return d+L"\\study-history.txt"; }
void StudyLoadHistory(){ if(g_studyLoaded)return; g_studyLoaded=true; std::wifstream f(StudyFilePath().c_str()); if(!f)return; std::wstring line; while(std::getline(f,line)){ std::wistringstream ss(line); std::wstring k,a,b; if(!std::getline(ss,k,L'|')||!std::getline(ss,a,L'|')||!std::getline(ss,b,L'|'))continue; try{g_studyHistory[k]={std::stod(a),std::stod(b)};}catch(...){}} }
void StudySaveHistory(){ StudyLoadHistory(); std::wofstream f(StudyFilePath().c_str(),std::ios::out|std::ios::trunc); if(!f)return; for(const auto& [k,d]:g_studyHistory) f<<k<<L"|"<<d.focusSeconds<<L"|"<<d.stopwatchSeconds<<L"\n"; }
std::wstring StudyFormatTime(double seconds){ long long t=std::max<long long>(0,(long long)std::llround(seconds)); long long h=t/3600,m=(t/60)%60,s=t%60; wchar_t b[32]={}; if(h>0) swprintf_s(b,L"%lld:%02lld:%02lld",h,m,s); else swprintf_s(b,L"%02lld:%02lld",m,s); return b; }
// Forward declaration: the task/date implementation is defined later in this file.
std::wstring SelectedTaskDateKey();
std::wstring StudySelectedDate(){ return SelectedTaskDateKey(); }
StudyDayTotals& StudyDay(const std::wstring& k){ StudyLoadHistory(); return g_studyHistory[k]; }
bool StudyRunning(){ return g_study.running&&g_study.mode!=StudyMode::None; }
std::wstring StudyCollapsedText(){ if(!StudyRunning())return L""; return g_study.mode==StudyMode::Stopwatch?StudyFormatTime(g_study.stopwatchElapsed):StudyFormatTime(g_study.remaining); }
void StudyNudge(){ g_layoutDirty=true; TriggerNudge(); }
void StudyStartPomodoro(int fm,int bm){ StudyLoadHistory(); g_study.mode=StudyMode::Pomodoro; g_study.phase=PomodoroPhase::Focus; g_study.focusMinutes=std::max(1,fm); g_study.breakMinutes=std::max(1,bm); g_study.remaining=g_study.focusMinutes*60.0; g_study.stopwatchElapsed=0; g_study.running=true; g_study.lastTick=NowSeconds(); g_study.lastPersist=g_study.lastTick; g_study.runningDate=StudySelectedDate(); g_study.lastDisplaySecond=-1; StudyNudge(); }
void StudyStartStopwatch(){ StudyLoadHistory(); g_study.mode=StudyMode::Stopwatch; g_study.phase=PomodoroPhase::Focus; g_study.running=true; g_study.stopwatchElapsed=0; g_study.remaining=0; g_study.lastTick=NowSeconds(); g_study.lastPersist=g_study.lastTick; g_study.runningDate=StudySelectedDate(); g_study.lastDisplaySecond=-1; StudyNudge(); }
void StudyPauseResume(){ double now=NowSeconds(); if(StudyRunning()){ double dt=std::clamp(now-g_study.lastTick,0.0,2.0); if(dt>0){ if(g_study.mode==StudyMode::Stopwatch){g_study.stopwatchElapsed+=dt;StudyDay(g_study.runningDate).stopwatchSeconds+=dt;} else if(g_study.phase==PomodoroPhase::Focus){g_study.remaining=std::max(0.0,g_study.remaining-dt);StudyDay(g_study.runningDate).focusSeconds+=dt;} else g_study.remaining=std::max(0.0,g_study.remaining-dt);} g_study.running=false; g_study.lastTick=now; StudySaveHistory(); StudyNudge(); return;} if(g_study.mode!=StudyMode::None){g_study.running=true;g_study.lastTick=now;g_study.lastPersist=now;if(g_study.runningDate.empty())g_study.runningDate=StudySelectedDate();StudyNudge();} }
void StudyReset(){ g_study.running=false;g_study.mode=StudyMode::None;g_study.phase=PomodoroPhase::Focus;g_study.remaining=0;g_study.stopwatchElapsed=0;g_study.runningDate.clear();StudySaveHistory();StudyNudge(); }
void StudyTick(double now){ StudyLoadHistory(); if(!StudyRunning())return; double dt=std::clamp(now-g_study.lastTick,0.0,1.0);g_study.lastTick=now;if(dt<=0)return;if(g_study.runningDate.empty())g_study.runningDate=StudySelectedDate(); if(g_study.mode==StudyMode::Stopwatch){g_study.stopwatchElapsed+=dt;StudyDay(g_study.runningDate).stopwatchSeconds+=dt;}else{g_study.remaining-=dt;if(g_study.phase==PomodoroPhase::Focus)StudyDay(g_study.runningDate).focusSeconds+=dt;if(g_study.remaining<=0){if(g_study.phase==PomodoroPhase::Focus){g_study.phase=PomodoroPhase::Break;g_study.remaining=g_study.breakMinutes*60.0;}else{g_study.phase=PomodoroPhase::Focus;g_study.remaining=g_study.focusMinutes*60.0;}g_study.lastDisplaySecond=-1;StudySaveHistory();}} int ds=(g_study.mode==StudyMode::Stopwatch)?(int)std::floor(g_study.stopwatchElapsed):(int)std::ceil(std::max(0.0,g_study.remaining));if(ds!=g_study.lastDisplaySecond){g_study.lastDisplaySecond=ds;StudyNudge();}if(now-g_study.lastPersist>=5.0){g_study.lastPersist=now;StudySaveHistory();} }
struct StudyPanel:Widget{
 float Measure(const DrawContext& dc,float)override{return 156.0f*dc.scale;}
 bool PtIn(D2D1_POINT_2F p,D2D1_RECT_F r)const{return p.x>=r.left&&p.x<r.right&&p.y>=r.top&&p.y<r.bottom;}
 bool OnPointer(const PointerEvent&e,bool&wantsCapture)override{wantsCapture=false;if(e.phase==PointerPhase::Down)return true;if(e.phase!=PointerPhase::Up)return false;float s=lastScale_,y=bounds.top+88*s,gap=6*s,w=(W(bounds)-8*s-2*gap)/3.0f;if(!StudyRunning()&&g_study.mode==StudyMode::None){if(PtIn(e.pos,D2D1::RectF(bounds.left+4*s,y,bounds.left+4*s+w,y+30*s))){StudyStartPomodoro(25,5);return true;}if(PtIn(e.pos,D2D1::RectF(bounds.left+4*s+w+gap,y,bounds.left+4*s+2*w+gap,y+30*s))){StudyStartPomodoro(50,10);return true;}if(PtIn(e.pos,D2D1::RectF(bounds.left+4*s+2*w+2*gap,y,bounds.right-4*s,y+30*s))){StudyStartStopwatch();return true;}}else{float bw=(W(bounds)-12*s)/2.0f;if(PtIn(e.pos,D2D1::RectF(bounds.left+4*s,y,bounds.left+4*s+bw,y+30*s))){StudyPauseResume();return true;}if(PtIn(e.pos,D2D1::RectF(bounds.left+8*s+bw,y,bounds.right-4*s,y+30*s))){StudyReset();return true;}}return true;}
 void Paint(DrawContext&dc)override{lastScale_=dc.scale;float s=dc.scale;StudyDayTotals t=StudyDay(StudySelectedDate());double total=t.focusSeconds+t.stopwatchSeconds;dc.Text(L"Study",dc.fTitle,D2D1::RectF(bounds.left+4*s,bounds.top,bounds.right-4*s,bounds.top+22*s),dc.text,.95f);if(StudyRunning()){dc.Text(g_study.mode==StudyMode::Stopwatch?L"Stopwatch":(g_study.phase==PomodoroPhase::Focus?L"Focus":L"Break"),dc.fSmall,D2D1::RectF(bounds.left+4*s,bounds.top+20*s,bounds.right-4*s,bounds.top+39*s),dc.muted,.75f);dc.Text(StudyCollapsedText(),dc.fPill,D2D1::RectF(bounds.left+4*s,bounds.top+37*s,bounds.right-4*s,bounds.top+69*s),dc.text,.95f,DWRITE_TEXT_ALIGNMENT_CENTER,DWRITE_PARAGRAPH_ALIGNMENT_CENTER);}else if(g_study.mode!=StudyMode::None){dc.Text(g_study.mode==StudyMode::Stopwatch?L"Stopwatch paused":(g_study.phase==PomodoroPhase::Focus?L"Focus paused":L"Break paused"),dc.fSmall,D2D1::RectF(bounds.left+4*s,bounds.top+22*s,bounds.right-4*s,bounds.top+42*s),dc.muted,.75f);dc.Text(StudyCollapsedText(),dc.fPill,D2D1::RectF(bounds.left+4*s,bounds.top+39*s,bounds.right-4*s,bounds.top+70*s),dc.text,.9f,DWRITE_TEXT_ALIGNMENT_CENTER,DWRITE_PARAGRAPH_ALIGNMENT_CENTER);}else{dc.Text(L"Focus timer",dc.fSmall,D2D1::RectF(bounds.left+4*s,bounds.top+22*s,bounds.right-4*s,bounds.top+42*s),dc.muted,.75f);dc.Text(L"25/5  ·  50/10  ·  Stopwatch",dc.fSmall,D2D1::RectF(bounds.left+4*s,bounds.top+42*s,bounds.right-4*s,bounds.top+62*s),dc.text,.78f,DWRITE_TEXT_ALIGNMENT_CENTER);}dc.Text(L"Selected day: "+StudyFormatTime(total),dc.fSmall,D2D1::RectF(bounds.left+4*s,bounds.top+67*s,bounds.right-4*s,bounds.top+85*s),dc.muted,.78f,DWRITE_TEXT_ALIGNMENT_CENTER);float y=bounds.top+88*s,gap=6*s;if(!StudyRunning()&&g_study.mode==StudyMode::None){float w=(W(bounds)-8*s-2*gap)/3.0f;D2D1_RECT_F a=D2D1::RectF(bounds.left+4*s,y,bounds.left+4*s+w,y+30*s),b=D2D1::RectF(bounds.left+4*s+w+gap,y,bounds.left+4*s+2*w+gap,y+30*s),c=D2D1::RectF(bounds.left+4*s+2*w+2*gap,y,bounds.right-4*s,y+30*s);for(auto r:{a,b,c}){dc.card->SetOpacity(.10f);dc.dc->FillRoundedRectangle(D2D1::RoundedRect(r,7*s,7*s),dc.card);dc.card->SetOpacity(1.0f);}dc.Text(L"25/5",dc.fSmall,a,dc.text,.9f,DWRITE_TEXT_ALIGNMENT_CENTER,DWRITE_PARAGRAPH_ALIGNMENT_CENTER);dc.Text(L"50/10",dc.fSmall,b,dc.text,.9f,DWRITE_TEXT_ALIGNMENT_CENTER,DWRITE_PARAGRAPH_ALIGNMENT_CENTER);dc.Text(L"Stopwatch",dc.fSmall,c,dc.text,.9f,DWRITE_TEXT_ALIGNMENT_CENTER,DWRITE_PARAGRAPH_ALIGNMENT_CENTER);}else{float bw=(W(bounds)-12*s)/2.0f;D2D1_RECT_F a=D2D1::RectF(bounds.left+4*s,y,bounds.left+4*s+bw,y+30*s),b=D2D1::RectF(bounds.left+8*s+bw,y,bounds.right-4*s,y+30*s);for(auto r:{a,b}){dc.card->SetOpacity(.10f);dc.dc->FillRoundedRectangle(D2D1::RoundedRect(r,7*s,7*s),dc.card);dc.card->SetOpacity(1.0f);}dc.Text(StudyRunning()?L"Pause":L"Resume",dc.fSmall,a,dc.text,.9f,DWRITE_TEXT_ALIGNMENT_CENTER,DWRITE_PARAGRAPH_ALIGNMENT_CENTER);dc.Text(L"Reset",dc.fSmall,b,dc.text,.9f,DWRITE_TEXT_ALIGNMENT_CENTER,DWRITE_PARAGRAPH_ALIGNMENT_CENTER);}}
 float lastScale_=1.0f;
};

// --- CollapsedPill: clock/date + weather + a mini volume element --------------
// Replaces the old plain divider between the clock and weather with a tiny speaker glyph +
// vertical fill track showing/controlling system volume. It is the pill's ONLY clickable/
// draggable region while collapsed -- HitTestDeep returns null everywhere else on the pill so a
// plain click still falls through to Surface::Open() (see the relaxed OnDown guard below).
struct CollapsedPill : Widget {
    float Measure(const DrawContext& dc, float) override { return 50.0f * dc.scale; }
    float PreferredWidth(const DrawContext& dc) override {
        const float s = dc.scale;
        MediaState m = MediaSnapshot();
        const bool studyLive = StudyRunning();
        float leftW = studyLive ? dc.MeasureWidth(dc.fPill, StudyCollapsedText())
                                : std::max(dc.MeasureWidth(dc.fPill, TimeString()),
                                           dc.MeasureWidth(dc.fPillSub, DatePillString()));
        // No glyph here (see Paint) -- the vertical fill track alone is the divider AND the
        // volume indicator, so it stays slim instead of budgeting space for an icon.
        float total = 16.0f * s + leftW + 12.0f * s + kTrackW * s + 12.0f * s;
        if (WeatherFresh()) total += 12.0f * s + dc.MeasureWidth(dc.fPill, WeatherShort());
        total += 16.0f * s;
        // Now-playing indicator: album art prepended on the left, a live waveform appended on
        // the right while actually playing -- both widen the pill (it glides to the new width via
        // the render loop's normal spring, no separate animation needed here).
        if (m.active) {
            total += kArtSize * s + kArtGap * s;
            if (m.playing) total += kArtGap * s + kWaveW * s;
        }
        return total;
    }
    Widget* HitTestDeep(D2D1_POINT_2F p) override {
        if (!visible) return nullptr;
        const bool inVolume = p.x >= hotspot_.left && p.x < hotspot_.right &&
                              p.y >= hotspot_.top && p.y < hotspot_.bottom;
        const bool inAlbum = p.x >= albumRect_.left && p.x < albumRect_.right &&
                             p.y >= albumRect_.top && p.y < albumRect_.bottom;
        return (inVolume || inAlbum) ? this : nullptr;
    }
    bool OnPointer(const PointerEvent& e, bool& wantsCapture) override {
        wantsCapture = false;
        if (e.phase == PointerPhase::Down) {
            if (pInAlbum_(e.pos)) { albumPressed_ = true; wantsCapture = true; return true; }
            dragging_ = true; wantsCapture = true; startY_ = e.pos.y; moved_ = false; pendingY_ = -1.0f;
            return true;
        }
        if (e.phase == PointerPhase::Move && dragging_) {
            if (!moved_ && std::fabs(e.pos.y - startY_) > 3.0f) moved_ = true;
            if (moved_) pendingY_ = e.pos.y;
            return true;
        }
        if (e.phase == PointerPhase::Up && albumPressed_) {
            albumPressed_ = false;
            if (pInAlbum_(e.pos)) MediaCommand(1);
            return true;
        }
        if (e.phase == PointerPhase::Up && dragging_) {
            dragging_ = false;
            if (moved_) pendingY_ = e.pos.y;
            else ToggleSystemMute();  // a clean click (no drag) toggles mute
            return true;
        }
        return false;
    }
    bool HasPending() const { return pendingY_ >= 0.0f; }
    float TakePending() { float y = pendingY_; pendingY_ = -1.0f; return y; }
    void Apply(float y) {
        float trackH = std::max(1.0f, H(trackRect_));
        SetSystemVolume((trackRect_.bottom - y) / trackH);
    }
    D2D1_RECT_F Hotspot() const { return hotspot_; }

    bool pInAlbum_(D2D1_POINT_2F p) const { return p.x >= albumRect_.left && p.x < albumRect_.right && p.y >= albumRect_.top && p.y < albumRect_.bottom; }

    void Paint(DrawContext& dc) override {
        const float s = dc.scale;
        const D2D1_RECT_F b = bounds;
        MediaState m = MediaSnapshot();
        const bool studyLive = StudyRunning();
        const float leftW = studyLive ? dc.MeasureWidth(dc.fPill, StudyCollapsedText())
                                      : std::max(dc.MeasureWidth(dc.fPill, TimeString()),
                                                 dc.MeasureWidth(dc.fPillSub, DatePillString()));
        const bool wx = WeatherFresh();
        const float weatherW = wx ? dc.MeasureWidth(dc.fPill, WeatherShort()) : 0.0f;
        const float trackW = kTrackW * s;
        float contentW = leftW + 12.0f * s + trackW + 12.0f * s;
        if (wx) contentW += 12.0f * s + weatherW;
        const bool showArt = m.active;
        const bool showWave = m.active && m.playing;
        float totalW = contentW;
        if (showArt) totalW += kArtSize * s + kArtGap * s;
        if (showWave) totalW += kArtGap * s + kWaveW * s;
        const float blockX0 = b.left + (W(b) - totalW) * 0.5f;  // start of the whole centered block
        const float lx = blockX0 + (showArt ? kArtSize * s + kArtGap * s : 0.0f);  // time/date start

        if (showArt) {
            D2D1_RECT_F art = D2D1::RectF(blockX0, b.top + 8.0f * s, blockX0 + kArtSize * s, b.top + 8.0f * s + kArtSize * s);
            albumRect_ = art;
            DrawCircularArt(dc, art, dc.fGlyph);
            if (!m.playing) {
                dc.card->SetOpacity(0.58f);
                dc.dc->FillEllipse(D2D1::Ellipse(D2D1::Point2F((art.left+art.right)*0.5f,(art.top+art.bottom)*0.5f), 11*s, 11*s), dc.card);
                dc.card->SetOpacity(1.0f);
                dc.Text(L"\xE768", dc.fGlyph, art, dc.text, 1.0f, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            }
        }

        if (studyLive) {
            dc.Text(StudyCollapsedText(), dc.fPill, D2D1::RectF(lx, b.top + 10.0f*s, lx + leftW, b.top + 40.0f*s),
                    dc.text, 0.98f, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        } else {
            dc.Text(TimeString(), dc.fPill, D2D1::RectF(lx, b.top + 8.0f*s, lx + leftW, b.top + 29.0f*s),
                    dc.text, 0.96f, DWRITE_TEXT_ALIGNMENT_CENTER);
            dc.Text(DatePillString(), dc.fPillSub, D2D1::RectF(lx, b.top + 27.0f*s, lx + leftW, b.top + 41.0f*s),
                    dc.muted, 0.85f, DWRITE_TEXT_ALIGNMENT_CENTER);
        }

        // No glyph -- the vertical fill track alone is the divider AND the volume indicator
        // (dimmed to the muted color while muted, rather than budgeting space for a mute icon).
        const bool muted = g_volMuted.load();
        const float trackX = lx + leftW + 12.0f * s;
        D2D1_RECT_F tr = D2D1::RectF(trackX, b.top + 13.0f * s, trackX + trackW, b.bottom - 13.0f * s);
        trackRect_ = tr;
        const float rad = trackW * 0.5f;
        dc.dc->FillRoundedRectangle(D2D1::RoundedRect(tr, rad, rad), dc.track);
        const float vol = Clamp(g_volDisplayScalar.load(), 0.0f, 1.0f);
        const float fillTop = tr.bottom - H(tr) * vol;
        if (fillTop < tr.bottom - 0.5f)
            dc.dc->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(tr.left, fillTop, tr.right, tr.bottom), rad, rad),
                                        muted ? dc.muted : dc.accent);
        // Hotspot spans the full pill height (generous vertical target) and is inflated well
        // beyond the now-slim track horizontally so it's still an easy drag/click target.
        hotspot_ = D2D1::RectF(trackX - 12.0f * s, b.top, trackX + trackW + 12.0f * s, b.bottom);

        float afterX = trackX + trackW;
        if (wx) {
            const float wxX = afterX + 12.0f * s;
            dc.Text(WeatherShort(), dc.fPill, D2D1::RectF(wxX, b.top, wxX + weatherW, b.bottom), dc.text, 0.92f,
                    DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
                    D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
            afterX = wxX + weatherW;
        }

        if (showWave) {
            const float waveX = afterX + kArtGap * s;
            D2D1_RECT_F wf = D2D1::RectF(waveX, b.top + 14.0f * s, waveX + kWaveW * s, b.bottom - 14.0f * s);
            DrawWaveform(dc, wf, waveSmooth_, true);
        }
    }

   private:
    static constexpr float kTrackW = 4.0f, kArtSize = 34.0f, kArtGap = 10.0f, kWaveW = 40.0f;
    float waveSmooth_[kWaveBars] = {};
    D2D1_RECT_F hotspot_{};
    D2D1_RECT_F trackRect_{};
    bool dragging_ = false, moved_ = false;
    bool albumPressed_ = false;
    D2D1_RECT_F albumRect_{};
    float startY_ = 0.0f;
    float pendingY_ = -1.0f;
};

// --- Button ------------------------------------------------------------------
struct Button : Widget {
    std::wstring text;
    std::wstring glyph;
    IDWriteTextFormat* glyphFmt = nullptr;
    std::function<void()> onClick;
    float height = 30.0f;
    bool pressed = false;

    float Measure(const DrawContext& dc, float) override { return height * dc.scale; }
    bool OnPointer(const PointerEvent& e, bool& wantsCapture) override {
        wantsCapture = false;
        if (e.phase == PointerPhase::Down) { pressed = true; return true; }
        if (e.phase == PointerPhase::Up) { pressed = false; if (onClick) onClick(); return true; }
        return false;
    }
    void Paint(DrawContext& dc) override {
        const float s = dc.scale;
        D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(bounds, 8.0f * s, 8.0f * s);
        dc.card->SetOpacity(pressed ? 0.18f : dc.CursorIn(bounds) ? 0.14f : 0.10f);
        dc.dc->FillRoundedRectangle(rr, dc.card);
        dc.card->SetOpacity(1.0f);
        std::wstring t = glyph.empty() ? text : (glyph + L"  " + text);
        dc.Text(t, dc.fSmall, bounds, dc.text, 0.92f, DWRITE_TEXT_ALIGNMENT_CENTER,
                DWRITE_PARAGRAPH_ALIGNMENT_CENTER, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
    }
};

// --- NotifRow (a single notification card) -----------------------------------
struct NotifRow : Widget {
    uint32_t id = 0;
    std::wstring app, title, body, aumid;
    std::function<void(uint32_t)> onDismiss;
    std::function<void(const std::wstring&)> onActivate;  // aumid
    float height = 60.0f;

    float Measure(const DrawContext& dc, float) override { return height * dc.scale; }
    bool InX(D2D1_POINT_2F p) const {
        return p.x >= bounds.right - 32.0f * lastScale_ && p.y <= bounds.top + 34.0f * lastScale_;
    }
    bool OnPointer(const PointerEvent& e, bool& wantsCapture) override {
        wantsCapture = false;
        if (e.phase == PointerPhase::Down) { armedDismiss_ = InX(e.pos); return true; }
        if (e.phase == PointerPhase::Up) {
            if (armedDismiss_ && InX(e.pos)) { armedDismiss_ = false; if (onDismiss) onDismiss(id); return true; }
            armedDismiss_ = false;
            if (onActivate && !aumid.empty()) onActivate(aumid);
            return true;
        }
        return false;
    }
    void Paint(DrawContext& dc) override {
        lastScale_ = dc.scale;
        const float s = dc.scale;
        D2D1_RECT_F card = bounds; card.bottom -= 5.0f * s;
        D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(card, 10.0f * s, 10.0f * s);
        dc.card->SetOpacity(0.07f);
        dc.dc->FillRoundedRectangle(rr, dc.card);
        dc.card->SetOpacity(1.0f);

        D2D1_RECT_F appR = D2D1::RectF(card.left + 12.0f * s, card.top + 7.0f * s,
                                       card.right - 30.0f * s, card.top + 23.0f * s);
        dc.Text(app, dc.fSmall, appR, dc.accent, 0.92f);

        std::wstring line = title;
        if (!body.empty()) line += L"   " + body;
        D2D1_RECT_F bodyR = D2D1::RectF(card.left + 12.0f * s, card.top + 24.0f * s,
                                        card.right - 12.0f * s, card.bottom - 6.0f * s);
        dc.Text(line, dc.fBody, bodyR, dc.text, 0.92f,
                DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

        D2D1_RECT_F xr = D2D1::RectF(card.right - 30.0f * s, card.top + 6.0f * s,
                                     card.right - 8.0f * s, card.top + 28.0f * s);
        dc.Text(L"\x2715", dc.fSmall, xr, dc.muted, armedDismiss_ ? 1.0f : 0.6f,
                DWRITE_TEXT_ALIGNMENT_CENTER);
    }
   private:
    bool armedDismiss_ = false;
    float lastScale_ = 1.0f;
};

int ClockMinuteKey();  // fwd (defined with the time helpers below)


// --- Local Tasks --------------------------------------------------------------
// Local tasks are separate from the read-only Outlook ICS calendar. Tasks are
// keyed by the selected calendar date, so each day has its own persistent list.
struct TaskItem {
    bool done = false;
    std::wstring text;
};

std::wstring TasksFilePath() {
    wchar_t buf[MAX_PATH] = {};
    DWORD n = GetEnvironmentVariableW(L"LOCALAPPDATA", buf, ARRAYSIZE(buf));
    if (!n || n >= ARRAYSIZE(buf)) return L"tasks.txt";
    std::wstring dir = std::wstring(buf) + L"\\IslandCommandCenter";
    CreateDirectoryW(dir.c_str(), nullptr);
    return dir + L"\\tasks.txt";
}

std::wstring DateKey(int y, int m, int d) {
    wchar_t b[32];
    swprintf_s(b, L"%04d-%02d-%02d", y, m, d);
    return b;
}

std::wstring SelectedTaskDateKey() {
    int y = g_calSelYear.load(), m = g_calSelMonth.load(), d = g_calSelDay.load();
    if (y <= 0 || m <= 0 || d <= 0) {
        SYSTEMTIME st{}; GetLocalTime(&st);
        return DateKey(st.wYear, st.wMonth, st.wDay);
    }
    return DateKey(y, m, d);
}

std::vector<TaskItem> LoadTasksForDate(const std::wstring& key) {
    std::vector<TaskItem> out;
    std::wifstream f(TasksFilePath().c_str());
    if (!f) return out;
    std::wstring line;
    while (std::getline(f, line)) {
        size_t p1 = line.find(L'|');
        size_t p2 = p1 == std::wstring::npos ? p1 : line.find(L'|', p1 + 1);
        if (p1 == std::wstring::npos || p2 == std::wstring::npos) continue;
        if (line.substr(0, p1) != key) continue;
        std::wstring text = line.substr(p2 + 1);
        if (text.empty()) continue;
        // The file uses | as a delimiter; stored task text converts it back.
        for (size_t i = 0; i + 2 < text.size(); ++i) {
            if (text[i] == L'<' && text[i+1] == L'|' && text[i+2] == L'>') {
                text.replace(i, 3, L"|");
                ++i;
            }
        }
        TaskItem t;
        t.done = (line.substr(p1 + 1, p2 - p1 - 1) == L"1");
        t.text = std::move(text);
        out.push_back(std::move(t));
    }
    return out;
}

void SaveTasksForDate(const std::wstring& key, const std::vector<TaskItem>& tasks) {
    // Rewrite the small local file while preserving every other date.
    std::vector<std::wstring> lines;
    const std::wstring path = TasksFilePath();
    {
        std::wifstream f(path.c_str());
        std::wstring line;
        while (f && std::getline(f, line)) {
            size_t p = line.find(L'|');
            if (p != std::wstring::npos && line.substr(0, p) == key) continue;
            if (!line.empty()) lines.push_back(std::move(line));
        }
    }
    std::wofstream f(path.c_str(), std::ios::out | std::ios::trunc);
    for (auto& line : lines) f << line << L"\n";
    for (const auto& t : tasks) {
        std::wstring text = t.text;
        // Keep the simple line format safe for user-entered | characters.
        size_t pos = 0;
        while ((pos = text.find(L'|', pos)) != std::wstring::npos) {
            text.replace(pos, 1, L"<|>");
            pos += 3;
        }
        for (wchar_t& ch : text) if (ch == L'\r' || ch == L'\n') ch = L' ';
        f << key << L"|" << (t.done ? L"1" : L"0") << L"|" << text << L"\n";
    }
}

struct TaskPromptState {
    std::wstring initial;
    std::wstring result;
    bool accepted = false;
};

INT_PTR CALLBACK TaskPromptProc(HWND dlg, UINT msg, WPARAM wp, LPARAM lp) {
    auto* st = reinterpret_cast<TaskPromptState*>(GetWindowLongPtrW(dlg, GWLP_USERDATA));
    if (msg == WM_INITDIALOG) {
        st = reinterpret_cast<TaskPromptState*>(lp);
        SetWindowLongPtrW(dlg, GWLP_USERDATA, (LONG_PTR)st);
        SetDlgItemTextW(dlg, 1001, st ? st->initial.c_str() : L"");
        SetFocus(GetDlgItem(dlg, 1001));
        SendDlgItemMessageW(dlg, 1001, EM_SETSEL, 0, -1);
        return FALSE;
    }
    if (!st) return FALSE;
    if (msg == WM_CTLCOLORDLG || msg == WM_CTLCOLORSTATIC || msg == WM_CTLCOLOREDIT || msg == WM_CTLCOLORBTN) {
        HDC dc = (HDC)wp;
        SetTextColor(dc, RGB(235,235,240));
        SetBkColor(dc, RGB(25,25,29));
        static HBRUSH bg = CreateSolidBrush(RGB(25,25,29));
        return (INT_PTR)bg;
    }
    if (msg == WM_COMMAND) {
        switch (LOWORD(wp)) {
            case IDOK: {
                wchar_t b[512] = {};
                GetDlgItemTextW(dlg, 1001, b, ARRAYSIZE(b));
                st->result = b;
                st->accepted = !st->result.empty();
                EndDialog(dlg, st->accepted ? IDOK : IDCANCEL);
                return TRUE;
            }
            case IDCANCEL:
                st->accepted = false; EndDialog(dlg, IDCANCEL); return TRUE;
        }
    }
    if (msg == WM_CLOSE) { st->accepted = false; EndDialog(dlg, IDCANCEL); return TRUE; }
    return FALSE;
}

bool PromptForTask(HWND owner, const std::wstring& initial, std::wstring& result) {
    TaskPromptState st; st.initial = initial;
    std::vector<BYTE> mem(1024, 0); BYTE* p = mem.data();
    auto align = [&]() { p = mem.data() + (((p - mem.data()) + 3) & ~3); };
    auto word = [&](WORD v) { *(WORD*)p = v; p += 2; };
    auto str = [&](const wchar_t* v) { while (*v) word((WORD)*v++); word(0); };
    align();
    auto* dlg = reinterpret_cast<DLGTEMPLATE*>(p);
    dlg->style = WS_POPUP | WS_CAPTION | WS_SYSMENU | DS_MODALFRAME | DS_SETFONT;
    dlg->dwExtendedStyle = 0; dlg->cdit = 4; dlg->x = 10; dlg->y = 10; dlg->cx = 300; dlg->cy = 105;
    p += sizeof(DLGTEMPLATE); word(0); word(0); str(L"Task"); word(9); str(L"Segoe UI");
    auto addControl = [&](DWORD style, short x, short y, short cx, short cy, WORD id, WORD classOrd, const wchar_t* title) {
        align(); auto* d=reinterpret_cast<DLGITEMTEMPLATE*>(p); d->style=style; d->dwExtendedStyle=0; d->x=x; d->y=y; d->cx=cx; d->cy=cy; d->id=id; p+=sizeof(DLGITEMTEMPLATE);
        word(0xFFFF); word(classOrd); str(title); word(0);
    };
    addControl(WS_CHILD|WS_VISIBLE|SS_LEFT, 10, 8, 280, 16, 1000, 0x0082, L"Task:");
    addControl(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL, 10, 28, 280, 20, 1001, 0x0081, L"");
    addControl(WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON, 150, 60, 65, 24, IDOK, 0x0080, L"Save");
    addControl(WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 225, 60, 65, 24, IDCANCEL, 0x0080, L"Cancel");
    INT_PTR r = DialogBoxIndirectParamW(GetModuleHandleW(nullptr), reinterpret_cast<DLGTEMPLATE*>(mem.data()), owner, TaskPromptProc, (LPARAM)&st);
    if (r == IDOK && st.accepted) { result = st.result; return true; }
    return false;
}

struct TaskList : Widget {
    std::vector<TaskItem> tasks;
    std::wstring loadedKey;
    float lastScale_ = 1.0f;

    TaskList() { RefreshForSelectedDate(); }

    void RefreshForSelectedDate() {
        loadedKey = SelectedTaskDateKey();
        tasks = LoadTasksForDate(loadedKey);
        g_layoutDirty = true;
    }

    void RefreshIfDateChanged() {
        std::wstring key = SelectedTaskDateKey();
        if (key != loadedKey) RefreshForSelectedDate();
    }

    float Measure(const DrawContext& dc, float) override {
        RefreshIfDateChanged();
        // Header + add button + one row per task + small footer gap.
        return (78.0f + std::max<size_t>(1, tasks.size()) * 38.0f) * dc.scale;
    }

    void Save() { SaveTasksForDate(loadedKey, tasks); }

    bool PtIn(D2D1_POINT_2F p, D2D1_RECT_F r) const {
        return p.x >= r.left && p.x < r.right && p.y >= r.top && p.y < r.bottom;
    }

    bool OnPointer(const PointerEvent& e, bool& wc) override {
        wc = false;
        if (e.phase == PointerPhase::Down) return true;
        if (e.phase != PointerPhase::Up) return false;
        RefreshIfDateChanged();
        const float s = lastScale_;

        D2D1_RECT_F add = D2D1::RectF(bounds.left + 4*s, bounds.top + 36*s,
                                      bounds.right - 4*s, bounds.top + 64*s);
        if (PtIn(e.pos, add)) {
            std::wstring text;
            if (PromptForTask(g_hwnd, L"", text)) {
                tasks.push_back({false, text});
                Save();
                g_layoutDirty = true;
            }
            return true;
        }

        float y = bounds.top + 70*s;
        for (size_t i = 0; i < tasks.size(); ++i) {
            D2D1_RECT_F row = D2D1::RectF(bounds.left + 4*s, y,
                                          bounds.right - 4*s, y + 32*s);
            if (PtIn(e.pos, row)) {
                const float xrel = e.pos.x - row.left;
                if (xrel < 32*s) {
                    tasks[i].done = !tasks[i].done;
                } else if (xrel > W(row) - 34*s) {
                    tasks.erase(tasks.begin() + i);
                } else {
                    std::wstring edited;
                    if (PromptForTask(g_hwnd, tasks[i].text, edited)) tasks[i].text = edited;
                }
                Save();
                g_layoutDirty = true;
                return true;
            }
            y += 38*s;
        }
        return true;
    }

    void Paint(DrawContext& dc) override {
        RefreshIfDateChanged();
        lastScale_ = dc.scale;
        const float s = dc.scale;

        SYSTEMTIME sel{};
        sel.wYear = (WORD)g_calSelYear.load();
        sel.wMonth = (WORD)g_calSelMonth.load();
        sel.wDay = (WORD)g_calSelDay.load();
        wchar_t dateText[64] = {};
        if (sel.wYear) GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &sel, L"dddd, MMMM d", dateText, ARRAYSIZE(dateText), nullptr);

        std::wstring taskHeader = L"Tasks  " + std::to_wstring(std::count_if(tasks.begin(), tasks.end(), [](const TaskItem& t){ return t.done; })) + L"/" + std::to_wstring(tasks.size());
        dc.Text(taskHeader, dc.fTitle,
                D2D1::RectF(bounds.left + 4*s, bounds.top, bounds.right - 4*s, bounds.top + 22*s),
                dc.text, 0.95f);
        if (dateText[0]) {
            dc.Text(dateText, dc.fSmall,
                    D2D1::RectF(bounds.left + 4*s, bounds.top + 18*s, bounds.right - 4*s, bounds.top + 34*s),
                    dc.muted, 0.65f);
        }

        // Explicit opacity here is important: the theme's card brush is opaque white by design,
        // and cards elsewhere lower it before filling.
        D2D1_RECT_F add = D2D1::RectF(bounds.left + 4*s, bounds.top + 36*s,
                                      bounds.right - 4*s, bounds.top + 64*s);
        dc.card->SetOpacity(0.10f);
        dc.dc->FillRoundedRectangle(D2D1::RoundedRect(add, 7*s, 7*s), dc.card);
        dc.card->SetOpacity(1.0f);
        dc.Text(L"+  Add task", dc.fSmall, add, dc.text, 0.9f,
                DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        float y = bounds.top + 70*s;
        if (tasks.empty()) return;

        for (const auto& t : tasks) {
            D2D1_RECT_F row = D2D1::RectF(bounds.left + 4*s, y,
                                          bounds.right - 4*s, y + 32*s);
            dc.card->SetOpacity(0.08f);
            dc.dc->FillRoundedRectangle(D2D1::RoundedRect(row, 7*s, 7*s), dc.card);
            dc.card->SetOpacity(1.0f);

            D2D1_RECT_F box = D2D1::RectF(row.left + 7*s, row.top + 8*s,
                                          row.left + 23*s, row.top + 24*s);
            dc.dc->DrawRoundedRectangle(D2D1::RoundedRect(box, 4*s, 4*s), dc.muted, 1.2f*s);
            if (t.done) {
                dc.accent->SetOpacity(0.9f);
                dc.dc->FillRoundedRectangle(D2D1::RoundedRect(box, 4*s, 4*s), dc.accent);
                dc.accent->SetOpacity(1.0f);
                dc.Text(L"✓", dc.fSmall, box, dc.text, 1.0f,
                        DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            }
            dc.Text(t.text, dc.fBody,
                    D2D1::RectF(row.left + 31*s, row.top, row.right - 36*s, row.bottom),
                    t.done ? dc.muted : dc.text, t.done ? 0.55f : 0.9f);
            dc.Text(L"×", dc.fBody,
                    D2D1::RectF(row.right - 31*s, row.top, row.right - 5*s, row.bottom),
                    dc.muted, 0.75f, DWRITE_TEXT_ALIGNMENT_CENTER,
                    DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            y += 38*s;
        }
    }
};

// --- CalendarView (interactive month grid) -----------------------------------
struct CalendarView : Widget {
    float Measure(const DrawContext& dc, float) override { return 214.0f * dc.scale; }

    bool OnPointer(const PointerEvent& e, bool& wc) override {
        wc = false;
        if (e.phase == PointerPhase::Down) return true;
        if (e.phase != PointerPhase::Up) return false;
        Init();
        const float s = lastScale_;
        if (e.pos.y < bounds.top + 30.0f * s) {
            if (e.pos.x < bounds.left + 44.0f * s) { Step(-1); return true; }
            if (e.pos.x > bounds.right - 44.0f * s) { Step(1); return true; }
            return true;
        }
        int d = DayAt(e.pos, s);
        if (d > 0) { g_calSelYear = g_calViewYear.load(); g_calSelMonth = g_calViewMonth.load(); g_calSelDay = d; g_layoutDirty = true; }
        return true;
    }

    void Paint(DrawContext& dc) override {
        lastScale_ = dc.scale; Init();
        const float s = dc.scale;
        int vy = g_calViewYear.load(), vm = g_calViewMonth.load();
        SYSTEMTIME mt{}; mt.wYear = (WORD)vy; mt.wMonth = (WORD)vm; mt.wDay = 1;
        wchar_t hdr[64] = {}; GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &mt, L"MMMM yyyy", hdr, ARRAYSIZE(hdr), nullptr);
        D2D1_RECT_F hr = D2D1::RectF(bounds.left, bounds.top, bounds.right, bounds.top + 28.0f * s);
        dc.Text(hdr, dc.fTitle, hr, dc.text, 0.95f, DWRITE_TEXT_ALIGNMENT_CENTER);
        dc.Text(L"\x2039", dc.fTitle, D2D1::RectF(bounds.left + 6 * s, hr.top, bounds.left + 40 * s, hr.bottom), dc.muted, 0.85f, DWRITE_TEXT_ALIGNMENT_CENTER);
        dc.Text(L"\x203A", dc.fTitle, D2D1::RectF(bounds.right - 40 * s, hr.top, bounds.right - 6 * s, hr.bottom), dc.muted, 0.85f, DWRITE_TEXT_ALIGNMENT_CENTER);

        const wchar_t* wd[7] = {L"S", L"M", L"T", L"W", L"T", L"F", L"S"};
        float cellW = W(bounds) / 7.0f, wkY = bounds.top + 30.0f * s;
        for (int i = 0; i < 7; ++i)
            dc.Text(wd[i], dc.fSmall, D2D1::RectF(bounds.left + i * cellW, wkY, bounds.left + (i + 1) * cellW, wkY + 18 * s), dc.muted, 0.5f, DWRITE_TEXT_ALIGNMENT_CENTER);

        const std::unordered_set<int>& evDays = EventDays(vy, vm);
        SYSTEMTIME now; GetLocalTime(&now);
        int fw = FirstWeekday(vy, vm), dim = ics_detail::DaysInMonth(vy, vm);
        float gridTop = bounds.top + 52.0f * s, rowH = (H(bounds) - 52.0f * s) / 6.0f;
        int selY = g_calSelYear.load(), selM = g_calSelMonth.load(), selD = g_calSelDay.load();
        for (int d = 1; d <= dim; ++d) {
            int idx = fw + d - 1, col = idx % 7, row = idx / 7;
            float cx = bounds.left + col * cellW + cellW * 0.5f, cy = gridTop + row * rowH + rowH * 0.5f;
            bool isToday = (vy == now.wYear && vm == now.wMonth && d == now.wDay);
            bool isSel = (vy == selY && vm == selM && d == selD);
            if (isSel) { dc.accent->SetOpacity(0.9f); dc.dc->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), 11 * s, 11 * s), dc.accent); dc.accent->SetOpacity(1.0f); }
            else if (isToday) dc.dc->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), 11 * s, 11 * s), dc.accent, 1.5f);
            wchar_t ds[4]; swprintf_s(ds, L"%d", d);
            dc.Text(ds, dc.fSmall, D2D1::RectF(cx - cellW * 0.5f, cy - rowH * 0.5f, cx + cellW * 0.5f, cy + rowH * 0.5f),
                    dc.text, isSel ? 1.0f : 0.88f, DWRITE_TEXT_ALIGNMENT_CENTER);
            if (evDays.count(d)) dc.dc->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy + 9 * s), 1.6f * s, 1.6f * s), isSel ? dc.text : dc.accent);
        }
    }
   private:
    float lastScale_ = 1.0f;
    uint64_t evGen_ = (uint64_t)-1; int evView_ = -1;
    std::unordered_set<int> evDays_;
    // Set of days-with-events for the viewed month; rebuilt only when the calendar data
    // generation or the viewed month changes (not every paint/drag frame).
    const std::unordered_set<int>& EventDays(int vy, int vm) {
        uint64_t gen = g_calGen.load(); int vk = vy * 13 + vm;
        if (gen == evGen_ && vk == evView_) return evDays_;
        evGen_ = gen; evView_ = vk; evDays_.clear();
        // All-day events are hidden from the agenda, so keep the dot consistent with that: a day
        // dot means "there's something to see if you click it".
        for (auto& ev : CalendarSnapshot())
            if (!ev.allDay && ev.start.wYear == vy && ev.start.wMonth == vm) evDays_.insert(ev.start.wDay);
        return evDays_;
    }
    void Init() {
        if (g_calViewYear.load() == 0) { SYSTEMTIME n; GetLocalTime(&n); g_calViewYear = n.wYear; g_calViewMonth = n.wMonth; }
        if (g_calSelYear.load() == 0) { SYSTEMTIME n; GetLocalTime(&n); g_calSelYear = n.wYear; g_calSelMonth = n.wMonth; g_calSelDay = n.wDay; }
    }
    void Step(int dir) { int y = g_calViewYear.load(), m = g_calViewMonth.load() + dir; if (m < 1) { m = 12; --y; } if (m > 12) { m = 1; ++y; } g_calViewYear = y; g_calViewMonth = m; g_layoutDirty = true; }
    int FirstWeekday(int y, int m) { SYSTEMTIME st{}; st.wYear = (WORD)y; st.wMonth = (WORD)m; st.wDay = 1; st.wHour = 12; return ics_detail::DayOfWeek(st); }
    int DayAt(D2D1_POINT_2F p, float s) {
        Init();
        int vy = g_calViewYear.load(), vm = g_calViewMonth.load();
        float cellW = W(bounds) / 7.0f, gridTop = bounds.top + 52.0f * s, rowH = (H(bounds) - 52.0f * s) / 6.0f;
        if (p.y < gridTop) return -1;
        int col = (int)((p.x - bounds.left) / cellW), row = (int)((p.y - gridTop) / rowH);
        if (col < 0 || col > 6 || row < 0 || row > 5) return -1;
        int d = row * 7 + col - FirstWeekday(vy, vm) + 1, dim = ics_detail::DaysInMonth(vy, vm);
        return (d >= 1 && d <= dim) ? d : -1;
    }
};

// --- AgendaList (events for the selected day) ---------------------------------
struct AgendaList : Widget {
    float Measure(const DrawContext& dc, float) override {
        const int n = (int)ForSel().size();
        // No calendar events for the selected day: collapse the agenda completely.
        // This removes the old "Nothing upcoming" / "No events" placeholder and its
        // unused vertical space, so Tasks moves directly under the calendar.
        if (n == 0) return 0.0f;
        return (24.0f + n * 36.0f) * dc.scale;
    }
    void Paint(DrawContext& dc) override {
        const std::vector<IcsEvent>& evs = ForSel();
        // Keep the empty agenda fully invisible and zero-height.
        if (evs.empty()) return;
        const float s = dc.scale;
        SYSTEMTIME sd{}; sd.wYear = (WORD)g_calSelYear.load(); sd.wMonth = (WORD)g_calSelMonth.load(); sd.wDay = (WORD)g_calSelDay.load(); sd.wHour = 12;
        if (sd.wYear == 0) GetLocalTime(&sd);
        wchar_t hdr[64] = {}; GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &sd, L"dddd, MMMM d", hdr, ARRAYSIZE(hdr), nullptr);
        dc.Text(hdr, dc.fSmall, D2D1::RectF(bounds.left + 4 * s, bounds.top, bounds.right - 4 * s, bounds.top + 22 * s), dc.muted, 0.85f);
        float y = bounds.top + 24.0f * s;
        for (auto& e : evs) {
            D2D1_RECT_F row = D2D1::RectF(bounds.left + 4 * s, y, bounds.right - 4 * s, y + 34 * s);
            dc.dc->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(row.left, row.top + 4 * s, row.left + 3 * s, row.bottom - 4 * s), 1.5f * s, 1.5f * s), dc.accent);
            wchar_t tm[24] = {};
            if (e.allDay) wcscpy_s(tm, L"All day");
            else { SYSTEMTIME es = e.start; GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &es, nullptr, tm, ARRAYSIZE(tm)); }
            dc.Text(tm, dc.fSmall, D2D1::RectF(row.left + 10 * s, row.top, row.left + 80 * s, row.bottom), dc.muted, 0.8f);
            dc.Text(e.subject, dc.fBody, D2D1::RectF(row.left + 86 * s, row.top, row.right, row.bottom), dc.text, 0.92f);
            y += 36.0f * s;
        }
    }
   private:
    static unsigned long long StampOf(const SYSTEMTIME& st) {
        FILETIME ft{}; SystemTimeToFileTime(&st, &ft);
        ULARGE_INTEGER u; u.LowPart = ft.dwLowDateTime; u.HighPart = ft.dwHighDateTime;
        return u.QuadPart;
    }
    uint64_t cacheGen_ = (uint64_t)-1; int cacheSel_ = -1; int cacheMin_ = -1;
    std::vector<IcsEvent> cache_;
    // Events for the selected day. When that day is today, show only what's still upcoming
    // (drop meetings that have already ended); other days show the full day. Sorted by time.
    // Cached: recomputed only when the calendar data, the selected day, or the minute changes
    // (the "upcoming" cutoff advances by the minute) — NOT on every measure/paint/drag frame.
    const std::vector<IcsEvent>& ForSel() {
        int y = g_calSelYear.load(), m = g_calSelMonth.load(), d = g_calSelDay.load();
        int selKey = (y * 13 + m) * 32 + d;
        int minKey = ClockMinuteKey();
        uint64_t gen = g_calGen.load();
        if (gen == cacheGen_ && selKey == cacheSel_ && minKey == cacheMin_) return cache_;
        cacheGen_ = gen; cacheSel_ = selKey; cacheMin_ = minKey;
        cache_.clear();
        if (y == 0) return cache_;
        SYSTEMTIME nowSt; GetLocalTime(&nowSt);
        bool selToday = (y == nowSt.wYear && m == nowSt.wMonth && d == nowSt.wDay);
        unsigned long long nowU = StampOf(nowSt);
        for (auto& e : CalendarSnapshot()) {
            if (e.allDay) continue;  // all-day events are hidden from the agenda
            if (!(e.start.wYear == y && e.start.wMonth == m && e.start.wDay == d)) continue;
            if (selToday) {
                unsigned long long endU = StampOf(e.end.wYear ? e.end : e.start);
                if (endU <= nowU) continue;  // already ended -> not upcoming
            }
            cache_.push_back(e);
        }
        std::sort(cache_.begin(), cache_.end(), [](const IcsEvent& a, const IcsEvent& b) {
            return StampOf(a.start) < StampOf(b.start);
        });
        return cache_;
    }
};

std::wstring FormatMediaTime(double sec) {
    if (sec < 0) sec = 0;
    int total = (int)(sec + 0.5);
    wchar_t buf[16]; swprintf_s(buf, L"%d:%02d", total / 60, total % 60);
    return buf;
}

// --- MediaCard (now playing: art, transport, scrub, live waveform) -----------
struct MediaCard : Widget {
    float Measure(const DrawContext& dc, float) override { return 128.0f * dc.scale; }

    D2D1_RECT_F SeekRect(float s) const {
        return D2D1::RectF(bounds.left + 12 * s, bounds.bottom - 15 * s, bounds.right - 12 * s, bounds.bottom - 11 * s);
    }
    void Buttons(float s, D2D1_RECT_F& prev, D2D1_RECT_F& play, D2D1_RECT_F& next) const {
        float cx = (bounds.left + bounds.right) * 0.5f;  // centered across the whole card
        float y0 = bounds.top + 60 * s, y1 = bounds.top + 88 * s, r = 15 * s;
        prev = D2D1::RectF(cx - 44 * s - r, y0, cx - 44 * s + r, y1);
        play = D2D1::RectF(cx - r, y0, cx + r, y1);
        next = D2D1::RectF(cx + 44 * s - r, y0, cx + 44 * s + r, y1);
    }
    bool OnPointer(const PointerEvent& e, bool& wc) override {
        wc = false; const float s = lastScale_;
        D2D1_RECT_F sk = SeekRect(s);
        bool inSeek = e.pos.x >= sk.left - 6 * s && e.pos.x <= sk.right + 6 * s &&
                      e.pos.y >= sk.top - 10 * s && e.pos.y <= sk.bottom + 10 * s;
        if (e.phase == PointerPhase::Down) {
            if (inSeek) {
                seeking_ = true;
                wc = true;
                pendingFrac_ = Clamp((e.pos.x - sk.left) / std::max(1.0f, W(sk)), 0.0f, 1.0f);
                // A simple click jumps immediately; dragging continues to update the preview.
                MediaSeek(pendingFrac_);
            }
            return true;
        }
        if (e.phase == PointerPhase::Move && seeking_) { pendingFrac_ = Clamp((e.pos.x - sk.left) / std::max(1.0f, W(sk)), 0.0f, 1.0f); return true; }
        if (e.phase == PointerPhase::Up) {
            if (seeking_) { seeking_ = false; MediaSeek(Clamp((e.pos.x - sk.left) / std::max(1.0f, W(sk)), 0.0f, 1.0f)); return true; }
            D2D1_RECT_F p, pl, n; Buttons(s, p, pl, n);
            auto in = [&](const D2D1_RECT_F& r) { return e.pos.x >= r.left && e.pos.x <= r.right && e.pos.y >= r.top && e.pos.y <= r.bottom; };
            if (in(p)) MediaCommand(0); else if (in(pl)) MediaCommand(1); else if (in(n)) MediaCommand(2);
            return true;
        }
        return false;
    }
    void Paint(DrawContext& dc) override {
        lastScale_ = dc.scale; const float s = dc.scale;
        MediaState m = MediaSnapshot();
        D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(bounds, 12 * s, 12 * s);
        dc.card->SetOpacity(0.06f); dc.dc->FillRoundedRectangle(rr, dc.card); dc.card->SetOpacity(1.0f);

        D2D1_RECT_F art = D2D1::RectF(bounds.left + 10 * s, bounds.top + 10 * s, bounds.left + 74 * s, bounds.top + 74 * s);
        DrawCircularArt(dc, art, dc.fGlyph ? dc.fGlyph : dc.fTitle);

        // Text column stops short of the right edge to leave room for the waveform beside it.
        float tx = bounds.left + 84 * s;
        float textRight = bounds.right - 64 * s;
        dc.Text(m.title.empty() ? L"Nothing playing" : m.title, dc.fTitle,
                D2D1::RectF(tx, bounds.top + 12 * s, textRight, bounds.top + 34 * s), dc.text, 0.95f);
        std::wstring artistLine = m.sourceName.empty() ? m.artist : m.artist + L"  \x00B7  " + m.sourceName;
        dc.Text(artistLine, dc.fSmall, D2D1::RectF(tx, bounds.top + 34 * s, textRight, bounds.top + 52 * s), dc.muted, 0.8f);

        // Live waveform: right of the title/artist rows, above the button row.
        D2D1_RECT_F wf = D2D1::RectF(bounds.right - 56 * s, bounds.top + 16 * s, bounds.right - 14 * s, bounds.top + 50 * s);
        DrawWaveform(dc, wf, waveSmooth_, m.playing);

        D2D1_RECT_F bp, bpl, bn; Buttons(s, bp, bpl, bn);
        IDWriteTextFormat* g = dc.fGlyph ? dc.fGlyph : dc.fTitle;
        auto drawBtn = [&](const D2D1_RECT_F& r, const std::wstring& glyphStr, float baseOp) {
            const bool hov = dc.CursorIn(r);
            if (hov) {
                const float hr = std::min(W(r), H(r)) * 0.5f + 5 * s;
                dc.card->SetOpacity(0.14f);
                dc.dc->FillEllipse(D2D1::Ellipse(
                    D2D1::Point2F((r.left + r.right) * 0.5f, (r.top + r.bottom) * 0.5f), hr, hr), dc.card);
                dc.card->SetOpacity(1.0f);
            }
            dc.Text(glyphStr, g, r, dc.text, hov ? 1.0f : baseOp, DWRITE_TEXT_ALIGNMENT_CENTER);
        };
        drawBtn(bp, L"\xE892", 0.9f);
        drawBtn(bpl, m.playing ? L"\xE769" : L"\xE768", 0.95f);
        drawBtn(bn, L"\xE893", 0.9f);

        D2D1_RECT_F sk = SeekRect(s); float rad = H(sk) * 0.5f;
        dc.dc->FillRoundedRectangle(D2D1::RoundedRect(sk, rad, rad), dc.track);
        double posSec = m.posTicks / 1e7 + ((m.playing && m.capturedTick) ? (double)(GetTickCount64() - m.capturedTick) / 1000.0 : 0.0);
        double durSec = m.endTicks / 1e7;
        float frac = (seeking_ && pendingFrac_ >= 0) ? pendingFrac_ : (durSec > 0 ? Clamp((float)(posSec / durSec), 0.0f, 1.0f) : 0.0f);
        D2D1_RECT_F fill = sk; fill.right = sk.left + W(sk) * frac;
        dc.dc->FillRoundedRectangle(D2D1::RoundedRect(fill, rad, rad), dc.accent);

        if (durSec > 0) {
            D2D1_RECT_F tr = D2D1::RectF(sk.left, sk.top - 16 * s, sk.right, sk.top - 2 * s);
            dc.Text(FormatMediaTime(posSec), dc.fSmall, tr, dc.muted, 0.7f, DWRITE_TEXT_ALIGNMENT_LEADING);
            dc.Text(L"-" + FormatMediaTime(durSec - posSec), dc.fSmall, tr, dc.muted, 0.7f, DWRITE_TEXT_ALIGNMENT_TRAILING);
        }
    }
   private:
    float lastScale_ = 1.0f;
    bool seeking_ = false;
    float pendingFrac_ = -1.0f;
    float waveSmooth_[kWaveBars] = {};
};

// --- StackPanel --------------------------------------------------------------
struct StackPanel : Widget {
    std::vector<std::unique_ptr<Widget>> children;
    float padX = 14.0f, padY = 12.0f, gap = 8.0f;
    bool drawBg = false;   // fill a rounded panel backdrop (for the Open control center)

    Widget* Add(std::unique_ptr<Widget> w) {
        w->parent = this;
        Widget* raw = w.get();
        children.push_back(std::move(w));
        return raw;
    }
    float Measure(const DrawContext& dc, float width) override {
        const float s = dc.scale;
        float inner = width - 2.0f * padX * s;
        float h = padY * s;
        bool first = true;
        for (auto& c : children) {
            if (!c->visible) continue;
            if (!first) h += gap * s;
            first = false;
            h += c->Measure(dc, inner);
        }
        h += padY * s;
        return h;
    }
    void Layout(D2D1_RECT_F b) override {
        bounds = b;
        const DrawContext* dcp = layoutDc;
        float s = dcp ? dcp->scale : 1.0f;
        float x = b.left + padX * s;
        float right = b.right - padX * s;
        float y = b.top + padY * s;
        for (auto& c : children) {
            if (!c->visible) continue;
            float ch = c->Measure(*dcp, right - x);
            c->Layout(D2D1::RectF(x, y, right, y + ch));
            y += ch + gap * s;
        }
    }
    void Paint(DrawContext& dc) override {
        if (drawBg) {
            float r = 24.0f * dc.scale;
            D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(bounds, r, r);
            dc.dc->FillRoundedRectangle(rr, dc.panel);
            dc.dc->DrawRoundedRectangle(rr, dc.border, 1.0f);
        }
        for (auto& c : children) if (c->visible) c->Paint(dc);
    }
    Widget* HitTestDeep(D2D1_POINT_2F p) override {
        if (!visible || !Contains(p)) return nullptr;
        for (auto it = children.rbegin(); it != children.rend(); ++it) {
            if (!(*it)->visible) continue;
            if (Widget* h = (*it)->HitTestDeep(p)) return h;
        }
        return this;
    }
    const DrawContext* layoutDc = nullptr;  // set by Surface before Layout
};

// --- ScrollContainer ---------------------------------------------------------
struct ScrollContainer : Widget {
    std::vector<std::unique_ptr<Widget>> children;
    SpringValue scroll;
    float contentHeight = 0.0f;
    float viewportMax = 240.0f;
    float gap = 6.0f;
    const DrawContext* layoutDc = nullptr;

    Widget* Add(std::unique_ptr<Widget> w) {
        w->parent = this;
        Widget* raw = w.get();
        children.push_back(std::move(w));
        return raw;
    }
    void Clear() { children.clear(); }
    ScrollContainer* AsScroll() override { return this; }

    float Measure(const DrawContext& dc, float width) override {
        float h = 0.0f; bool first = true;
        for (auto& c : children) {
            if (!c->visible) continue;
            if (!first) h += gap * dc.scale;
            first = false;
            h += c->Measure(dc, width);
        }
        contentHeight = h;
        return std::min(h, viewportMax * dc.scale);
    }
    void Layout(D2D1_RECT_F b) override {
        bounds = b;
        const DrawContext* dcp = layoutDc;
        float s = dcp ? dcp->scale : 1.0f;
        float maxScroll = std::max(0.0f, contentHeight - H(b));
        scroll.target = Clamp(scroll.target, 0.0f, maxScroll);
        float y = b.top - scroll.value;
        for (auto& c : children) {
            if (!c->visible) continue;
            float ch = c->Measure(*dcp, W(b));
            c->Layout(D2D1::RectF(b.left, y, b.right, y + ch));
            y += ch + gap * s;
        }
    }
    void OnWheel(float wheel) {
        scroll.target = scroll.target - wheel;  // wheel>0 = up
    }
    bool Tick(float dt) override {
        scroll.Step(dt, 220.0f, 26.0f);
        return !scroll.AtRest();
    }
    void Paint(DrawContext& dc) override {
        dc.dc->PushAxisAlignedClip(bounds, D2D1_ANTIALIAS_MODE_ALIASED);
        for (auto& c : children) {
            if (!c->visible) continue;
            if (c->bounds.bottom < bounds.top || c->bounds.top > bounds.bottom) continue;  // cull
            c->Paint(dc);
        }
        dc.dc->PopAxisAlignedClip();
        // Scrollbar hint
        float maxScroll = std::max(0.0f, contentHeight - H(bounds));
        if (maxScroll > 1.0f) {
            float frac = H(bounds) / contentHeight;
            float barH = std::max(20.0f * dc.scale, H(bounds) * frac);
            float t = scroll.value / maxScroll;
            float barY = bounds.top + t * (H(bounds) - barH);
            D2D1_RECT_F bar = D2D1::RectF(bounds.right - 3.0f * dc.scale, barY,
                                          bounds.right - 1.0f * dc.scale, barY + barH);
            dc.track->SetOpacity(0.5f);
            dc.dc->FillRoundedRectangle(D2D1::RoundedRect(bar, 1.5f * dc.scale, 1.5f * dc.scale), dc.track);
            dc.track->SetOpacity(1.0f);
        }
    }
    Widget* HitTestDeep(D2D1_POINT_2F p) override {
        if (!visible || !Contains(p)) return nullptr;
        for (auto it = children.rbegin(); it != children.rend(); ++it) {
            if (!(*it)->visible) continue;
            if ((*it)->bounds.bottom < bounds.top || (*it)->bounds.top > bounds.bottom) continue;
            if (Widget* h = (*it)->HitTestDeep(p)) return h;
        }
        return this;
    }
};

// ===========================================================================
// Renderer — D2D host. Owns brushes/formats; paints a Surface.
// ===========================================================================
class Renderer {
   public:
    bool Initialize(HWND hwnd) {
        hwnd_ = hwnd;
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory),
                                     reinterpret_cast<void**>(d2dFactory_.GetAddressOf()))))
            return false;
        if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                       reinterpret_cast<IUnknown**>(dwriteFactory_.GetAddressOf()))))
            return false;
        D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            0.0f, 0.0f, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE);
        if (FAILED(d2dFactory_->CreateDCRenderTarget(&props, &target_))) return false;
        return CreateBackingBitmap(360, 96);
    }
    void Shutdown() {
        fHuge_.Reset(); fTitle_.Reset(); fBody_.Reset(); fSmall_.Reset(); fPill_.Reset(); fPillSub_.Reset(); fGlyph_.Reset();
        bText_.Reset(); bMuted_.Reset(); bAccent_.Reset(); bPanel_.Reset(); bCard_.Reset();
        bBorder_.Reset(); bDivider_.Reset(); bTrack_.Reset();
        artBitmap_.Reset();
        contentLayer_.Reset();
        target_.Reset(); dwriteFactory_.Reset(); d2dFactory_.Reset();
        if (oldBitmap_) { SelectObject(memDc_, oldBitmap_); oldBitmap_ = nullptr; }
        if (dib_) { DeleteObject(dib_); dib_ = nullptr; }
        if (memDc_) { DeleteDC(memDc_); memDc_ = nullptr; }
    }

    // Cached opacity layer for the content cross-fade during a morph (created lazily post-init).
    ID2D1Layer* EnsureContentLayer() {
        if (!contentLayer_ && target_) target_->CreateLayer(nullptr, &contentLayer_);
        return contentLayer_.Get();
    }

    IDWriteFactory* DWrite() const { return dwriteFactory_.Get(); }

    DrawContext MakeContext(float scale, double now) {
        EnsureFormats(scale);
        DrawContext dc;
        dc.dc = target_.Get(); dc.dw = dwriteFactory_.Get(); dc.scale = scale; dc.now = now;
        dc.text = bText_.Get(); dc.muted = bMuted_.Get(); dc.accent = bAccent_.Get();
        dc.panel = bPanel_.Get(); dc.card = bCard_.Get(); dc.border = bBorder_.Get();
        dc.divider = bDivider_.Get(); dc.track = bTrack_.Get();
        dc.fHuge = fHuge_.Get(); dc.fTitle = fTitle_.Get(); dc.fBody = fBody_.Get();
        dc.fSmall = fSmall_.Get(); dc.fPill = fPill_.Get(); dc.fPillSub = fPillSub_.Get(); dc.fGlyph = fGlyph_.Get();
        return dc;
    }
    IDWriteTextFormat* GlyphFormat() const { return fGlyph_.Get(); }

    // Cache an ID2D1Bitmap for the current album art (by generation).
    ID2D1Bitmap* ArtBitmap(uint64_t gen, int w, int h, const std::vector<uint8_t>& bgra) {
        if (gen == 0 || bgra.empty() || w <= 0 || h <= 0) return nullptr;
        if (artBitmap_ && artGen_ == gen) return artBitmap_.Get();
        artBitmap_.Reset();
        D2D1_BITMAP_PROPERTIES bp = D2D1::BitmapProperties(
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
        if (SUCCEEDED(target_->CreateBitmap(D2D1::SizeU((UINT)w, (UINT)h), bgra.data(), (UINT)(w * 4), &bp, &artBitmap_))) {
            artGen_ = gen;
            return artBitmap_.Get();
        }
        return nullptr;
    }

    // Begin a frame. `contentW/H` is the current (animating) size we PRESENT; `targetW/H` is the
    // spring's destination and `settled` says the morph is done. To keep the expand/collapse morph
    // smooth we size the backing DIB to the TARGET (allocated once per morph) and only present the
    // current size from its top-left — this eliminates the per-frame CreateDIBSection realloc +
    // shrink/grow GDI churn that made resizing choppy. Returns the inner content rect.
    bool BeginFrame(const Settings& settings, const BackdropPreset& backdrop, int contentW, int contentH,
                    int targetW, int targetH, bool settled, float scale, float themeT, D2D1_RECT_F& innerOut) {
        const float padX = kRenderPadX * scale, padY = kRenderPadY * scale;
        const int pxW = std::max(1, static_cast<int>(std::ceil(contentW + padX * 2.0f)));  // present
        const int pxH = std::max(1, static_cast<int>(std::ceil(contentH + padY * 2.0f)));
        const int aW = std::max(1, static_cast<int>(std::ceil(std::max(contentW, targetW) + padX * 2.0f)));
        const int aH = std::max(1, static_cast<int>(std::ceil(std::max(contentH, targetH) + padY * 2.0f)));
        // Grow the DIB straight to the target on the first morph frame; never realloc mid-morph;
        // shrink it back to the exact present size only once the spring has settled.
        const int needW = settled ? pxW : std::max(aW, bitmapWidth_);
        const int needH = settled ? pxH : std::max(aH, bitmapHeight_);
        if (needW != bitmapWidth_ || needH != bitmapHeight_) {
            if (!CreateBackingBitmap(needW, needH)) return false;
        }
        // Resize/reposition the window whenever the PRESENT size changes (each morph frame) or on
        // an external layout change (settings / display change).
        // Size/position the WINDOW to the (stable) DIB size, not the animating size. Because the DIB
        // is held at max(current,target) for the whole morph, the window never repositions per-frame
        // — the panel + content animate WITHIN a fixed window. This is what kills the horizontal
        // sub-pixel jitter of the clock text (the window's centered x-position was rounding
        // independently of the content origin every frame). At rest DIB == content, so no margin.
        if (bitmapWidth_ != presentW_ || bitmapHeight_ != presentH_ || g_layoutDirty.exchange(false)) {
            PositionOverlayWindow(settings, bitmapWidth_, bitmapHeight_);
            presentW_ = bitmapWidth_; presentH_ = bitmapHeight_;
        }
        RECT rc = {0, 0, bitmapWidth_, bitmapHeight_};
        if (FAILED(target_->BindDC(memDc_, &rc))) return false;
        EnsureBrushes();  // device-dependent: must be created after BindDC
        if (bPanel_) {
            if (g_backdropBlurLive)
                bPanel_->SetColor(D2D1::ColorF(backdrop.r, backdrop.g, backdrop.b, backdrop.a));
            else
                bPanel_->SetColor(D2D1::ColorF(backdrop.fr, backdrop.fg, backdrop.fb, backdrop.fa));
        }
        // Foreground palette follows the theme position computed by the caller's ThemeEngine
        // (auto/dark/light/adaptive) BEFORE this frame — see the render loop.
        ApplyThemeBrushes(themeT);
        target_->BeginDraw();
        target_->Clear(D2D1::ColorF(0, 0.0f));
        innerOut = D2D1::RectF(padX, padY, (float)bitmapWidth_ - padX, (float)bitmapHeight_ - padY);
        return true;
    }
    // `visibility` is the temp-hide fade (1 = fully visible, 0 = fully hidden); folded in
    // multiplicatively alongside PillOpacity/BackdropStyle so hiding fades the ENTIRE island
    // (panel + content) smoothly rather than an abrupt show/hide.
    bool EndFrame(const Settings& settings, float visibility) {
        if (FAILED(target_->EndDraw())) return false;
        POINT src = {0, 0};
        SIZE size = {presentW_, presentH_};  // present the current size from the DIB's top-left
        RECT win = {}; GetWindowRect(hwnd_, &win);
        POINT dst = {win.left, win.top};
        BLENDFUNCTION blend = {};
        blend.BlendOp = AC_SRC_OVER;
        // PillOpacity and a BackdropStyle's own tint alpha must not compound (they were multiplying,
        // making backdrop styles look far more washed out than their tint alone suggests). PillOpacity
        // only applies to the plain solid panel (backdrop disabled); an active style's tint is authoritative.
        float finalAlpha = (!settings.backdropEnabled ? Clamp(settings.pillOpacity, 0.35f, 1.0f) : 1.0f) * visibility;
        blend.SourceConstantAlpha = static_cast<BYTE>(finalAlpha * 255.0f);
        blend.AlphaFormat = AC_SRC_ALPHA;
        return UpdateLayeredWindow(hwnd_, nullptr, &dst, &size, memDc_, &src, 0, &blend, ULW_ALPHA) != FALSE;
    }

    void DrawPanelSurface(D2D1_RECT_F inner) {
        float r = std::min(H(inner) * 0.5f, 22.0f);
        D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(inner, r, r);
        target_->FillRoundedRectangle(rr, bPanel_.Get());
        target_->DrawRoundedRectangle(rr, bBorder_.Get(), 1.0f);
    }

   private:
    void PositionOverlayWindow(const Settings& s, int width, int height);

    bool CreateBackingBitmap(int width, int height) {
        if (oldBitmap_) { SelectObject(memDc_, oldBitmap_); oldBitmap_ = nullptr; }
        if (dib_) { DeleteObject(dib_); dib_ = nullptr; }
        if (!memDc_) {
            HDC screen = GetDC(nullptr);
            memDc_ = CreateCompatibleDC(screen);
            ReleaseDC(nullptr, screen);
            if (!memDc_) return false;
        }
        BITMAPINFO bi = {};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = width;
        bi.bmiHeader.biHeight = -height;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        void* bits = nullptr;
        dib_ = CreateDIBSection(memDc_, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!dib_) return false;
        oldBitmap_ = static_cast<HBITMAP>(SelectObject(memDc_, dib_));
        bitmapWidth_ = width; bitmapHeight_ = height;
        return true;
    }

    void EnsureFormats(float scale) {
        if (fTitle_ && std::fabs(scale - lastScale_) < 0.001f) return;
        fHuge_.Reset(); fTitle_.Reset(); fBody_.Reset(); fSmall_.Reset(); fPill_.Reset(); fPillSub_.Reset(); fGlyph_.Reset();
        auto mk = [&](const wchar_t* family, DWRITE_FONT_WEIGHT w, float size, ComPtr<IDWriteTextFormat>& out) {
            dwriteFactory_->CreateTextFormat(family, nullptr, w, DWRITE_FONT_STYLE_NORMAL,
                                             DWRITE_FONT_STRETCH_NORMAL, size * scale, L"", &out);
            if (out) { out->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP); }
        };
        mk(L"Segoe UI Variable Display", DWRITE_FONT_WEIGHT_BOLD, 32.0f, fHuge_);
        mk(L"Segoe UI Variable Display", DWRITE_FONT_WEIGHT_SEMI_BOLD, 15.0f, fTitle_);
        mk(L"Segoe UI Variable Text", DWRITE_FONT_WEIGHT_NORMAL, 13.0f, fBody_);
        mk(L"Segoe UI Variable Small", DWRITE_FONT_WEIGHT_NORMAL, 11.5f, fSmall_);
        mk(L"Segoe UI Variable Display", DWRITE_FONT_WEIGHT_SEMI_BOLD, 15.0f, fPill_);
        mk(L"Segoe UI Variable Small", DWRITE_FONT_WEIGHT_BOLD, 10.5f, fPillSub_);  // pill date: -1pt, bold
        mk(L"Segoe Fluent Icons", DWRITE_FONT_WEIGHT_NORMAL, 16.0f, fGlyph_);
        if (fPill_) {
            fPill_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            fPill_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        }
        if (fPillSub_) {
            fPillSub_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            fPillSub_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        }
        if (fHuge_) { fHuge_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); fHuge_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); }
        for (auto* f : {fTitle_.Get(), fBody_.Get(), fSmall_.Get()})
            if (f) f->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        if (fGlyph_) { fGlyph_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); fGlyph_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER); }
        lastScale_ = scale;
    }
    // Foreground theme palettes: [0] = light-on-dark (the classic dark panel), [1] = dark-on-light
    // (light content behind/under the panel). The adaptive theme lerps every foreground brush
    // between them; accent and panel are excluded (accent is semantic, panel comes from the
    // BackdropStyle recipe).
    struct ThemeColor { float r, g, b, a; };
    struct ThemePalette { ThemeColor text, muted, card, border, divider, track; };
    // Dark theme's border (0x20FFFFFF, alpha 0x20/255) matches the windows-11-taskbar-styler
    // mod's own WindowGlass border color exactly.
    static constexpr ThemePalette kThemes[2] = {
        {{1, 1, 1, 0.96f}, {1, 1, 1, 0.62f}, {1, 1, 1, 1.0f},
         {1, 1, 1, 0.125f}, {1, 1, 1, 0.12f}, {1, 1, 1, 0.16f}},
        {{0.05f, 0.05f, 0.07f, 0.95f}, {0.07f, 0.07f, 0.09f, 0.60f}, {0, 0, 0, 1.0f},
         {0, 0, 0, 0.16f}, {0, 0, 0, 0.16f}, {0, 0, 0, 0.20f}},
    };
    void EnsureBrushes() {
        auto mk = [&](const ThemeColor& c, ComPtr<ID2D1SolidColorBrush>& out) {
            if (!out) target_->CreateSolidColorBrush(D2D1::ColorF(c.r, c.g, c.b, c.a), &out);
        };
        mk(kThemes[0].text, bText_);
        mk(kThemes[0].muted, bMuted_);
        mk({0.20f, 0.55f, 1.0f, 1.0f}, bAccent_);
        mk({0.043f, 0.043f, 0.051f, 0.94f}, bPanel_);
        mk(kThemes[0].card, bCard_);     // opacity varied at draw time
        mk(kThemes[0].border, bBorder_);
        mk(kThemes[0].divider, bDivider_);
        mk(kThemes[0].track, bTrack_);
    }
    // Slide the foreground palette to the current theme position (0 = light-on-dark).
    void ApplyThemeBrushes(float t) {
        auto set = [&](const ThemeColor& c0, const ThemeColor& c1, ComPtr<ID2D1SolidColorBrush>& br) {
            if (br) br->SetColor(D2D1::ColorF(c0.r + (c1.r - c0.r) * t, c0.g + (c1.g - c0.g) * t,
                                              c0.b + (c1.b - c0.b) * t, c0.a + (c1.a - c0.a) * t));
        };
        set(kThemes[0].text, kThemes[1].text, bText_);
        set(kThemes[0].muted, kThemes[1].muted, bMuted_);
        set(kThemes[0].card, kThemes[1].card, bCard_);
        set(kThemes[0].border, kThemes[1].border, bBorder_);
        set(kThemes[0].divider, kThemes[1].divider, bDivider_);
        set(kThemes[0].track, kThemes[1].track, bTrack_);
    }

    HWND hwnd_ = nullptr;
    ComPtr<ID2D1Factory> d2dFactory_;
    ComPtr<IDWriteFactory> dwriteFactory_;
    ComPtr<ID2D1DCRenderTarget> target_;
    ComPtr<ID2D1Layer> contentLayer_;
    ComPtr<IDWriteTextFormat> fHuge_, fTitle_, fBody_, fSmall_, fPill_, fPillSub_, fGlyph_;
    ComPtr<ID2D1SolidColorBrush> bText_, bMuted_, bAccent_, bPanel_, bCard_, bBorder_, bDivider_, bTrack_;
    ComPtr<ID2D1Bitmap> artBitmap_;
    uint64_t artGen_ = 0;
    HDC memDc_ = nullptr;
    HBITMAP dib_ = nullptr, oldBitmap_ = nullptr;
    int bitmapWidth_ = 0, bitmapHeight_ = 0;   // allocated DIB size (may exceed the presented size)
    int presentW_ = 0, presentH_ = 0;          // size actually shown via UpdateLayeredWindow
    float lastScale_ = 0.0f;
};

// ---------------------------------------------------------------------------
// Monitor anchoring
// ---------------------------------------------------------------------------
struct MonitorEnumData { std::vector<HMONITOR> monitors; };
BOOL CALLBACK MonitorEnumProc(HMONITOR h, HDC, LPRECT, LPARAM p) {
    reinterpret_cast<MonitorEnumData*>(p)->monitors.push_back(h);
    return TRUE;
}

// Resolves the TargetMonitor setting's name form (anything that isn't a clean 1-8 index) to an
// HMONITOR. Enumeration order (and so the numeric index above) reshuffles across sleep/wake,
// docking, or a driver re-scan, but a monitor's EDID identity and connector type don't -- so a
// name match is the only way to reliably pin the island to one physical monitor, e.g. a laptop's
// built-in panel. QueryDisplayConfig gives the EDID friendly name + connector technology per
// active path; the GDI device name from its DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME query is
// the join key back to the HMONITORs from EnumDisplayMonitors/GetMonitorInfoExW.
struct MonitorNameInfo {
    HMONITOR monitor;
    std::wstring gdiDeviceName;
    std::wstring friendlyName;  // EDID product string; often empty for laptop-internal panels
    bool isInternal = false;    // built-in panel by connector type (eDP/LVDS), not by EDID text
};
bool ContainsNoCase(const std::wstring& haystack, const std::wstring& needle) {
    if (needle.empty() || needle.size() > haystack.size()) return false;
    return std::search(haystack.begin(), haystack.end(), needle.begin(), needle.end(),
                        [](wchar_t a, wchar_t b) { return towlower(a) == towlower(b); }) != haystack.end();
}
std::vector<MonitorNameInfo> EnumerateMonitorNameInfo() {
    std::vector<MonitorNameInfo> result;
    MonitorEnumData d;
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, reinterpret_cast<LPARAM>(&d));
    for (HMONITOR h : d.monitors) {
        MONITORINFOEXW mi = {}; mi.cbSize = sizeof(mi);
        if (GetMonitorInfoW(h, &mi)) result.push_back({h, mi.szDevice, L"", false});
    }

    UINT32 pathCount = 0, modeCount = 0;
    if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount, &modeCount) != ERROR_SUCCESS || pathCount == 0)
        return result;
    std::vector<DISPLAYCONFIG_PATH_INFO> paths(pathCount);
    std::vector<DISPLAYCONFIG_MODE_INFO> modes(modeCount);
    if (QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths.data(), &modeCount, modes.data(), nullptr) != ERROR_SUCCESS)
        return result;

    for (UINT32 i = 0; i < pathCount; ++i) {
        const DISPLAYCONFIG_PATH_INFO& path = paths[i];

        DISPLAYCONFIG_SOURCE_DEVICE_NAME srcName = {};
        srcName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
        srcName.header.size = sizeof(srcName);
        srcName.header.adapterId = path.sourceInfo.adapterId;
        srcName.header.id = path.sourceInfo.id;
        if (DisplayConfigGetDeviceInfo(&srcName.header) != ERROR_SUCCESS) continue;

        auto it = std::find_if(result.begin(), result.end(), [&](const MonitorNameInfo& m) {
            return EqualsNoCase(m.gdiDeviceName, srcName.viewGdiDeviceName);
        });
        if (it == result.end()) continue;

        // The connector technology comes straight from the path itself, so the internal-panel
        // check (the main reason a laptop screen needs name-based targeting at all) doesn't
        // depend on the separate friendly-name query below succeeding.
        it->isInternal = path.targetInfo.outputTechnology == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL ||
                          path.targetInfo.outputTechnology == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED;

        DISPLAYCONFIG_TARGET_DEVICE_NAME tgtName = {};
        tgtName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
        tgtName.header.size = sizeof(tgtName);
        tgtName.header.adapterId = path.targetInfo.adapterId;
        tgtName.header.id = path.targetInfo.id;
        if (DisplayConfigGetDeviceInfo(&tgtName.header) == ERROR_SUCCESS)
            it->friendlyName = tgtName.monitorFriendlyDeviceName;
    }
    return result;
}
HMONITOR FindMonitorByName(const std::wstring& query) {
    std::vector<MonitorNameInfo> infos = EnumerateMonitorNameInfo();
    const bool wantInternal = EqualsNoCase(query, L"internal") || EqualsNoCase(query, L"laptop") ||
                               EqualsNoCase(query, L"built-in") || EqualsNoCase(query, L"builtin") ||
                               EqualsNoCase(query, L"built-in display");
    if (wantInternal)
        for (const MonitorNameInfo& m : infos) if (m.isInternal) return m.monitor;
    for (const MonitorNameInfo& m : infos)
        if (!m.friendlyName.empty() && EqualsNoCase(m.friendlyName, query.c_str())) return m.monitor;
    for (const MonitorNameInfo& m : infos)
        if (ContainsNoCase(m.friendlyName, query)) return m.monitor;
    return nullptr;
}
HMONITOR GetAnchorMonitor(const Settings& s) {
    if (s.targetMonitor == -1) { POINT pt; GetCursorPos(&pt); return MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST); }
    if (!s.targetMonitorName.empty()) {
        if (HMONITOR m = FindMonitorByName(s.targetMonitorName)) return m;
        // Named monitor isn't currently connected -- fall through to the primary-monitor default.
    } else if (s.targetMonitor > 0) {
        MonitorEnumData d;
        EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc, reinterpret_cast<LPARAM>(&d));
        int i = s.targetMonitor - 1;
        if (i >= 0 && i < (int)d.monitors.size()) return d.monitors[i];
    }
    POINT o = {0, 0};
    return MonitorFromPoint(o, MONITOR_DEFAULTTOPRIMARY);
}
RECT GetAnchorWorkRect(const Settings& s) {
    MONITORINFO mi = {sizeof(mi)};
    GetMonitorInfoW(GetAnchorMonitor(s), &mi);
    return mi.rcWork;
}
void Renderer::PositionOverlayWindow(const Settings& s, int width, int height) {
    RECT work = GetAnchorWorkRect(s);
    int x = work.left + (work.right - work.left - width) / 2;
    int y = work.top + 8;
    switch (s.position) {
        case Position::TopLeft:      x = work.left + 16; y = work.top + 8; break;
        case Position::TopRight:     x = work.right - width - 16; y = work.top + 8; break;
        case Position::BottomCenter: x = work.left + (work.right - work.left - width) / 2; y = work.bottom - height - 40; break;
        default: break;
    }
    x += s.offsetX; y += s.offsetY;
    HWND z = s.alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST;
    SetWindowPos(hwnd_, z, x, y, width, height, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
}

// ===========================================================================
// BackdropBlurHost — REAL blur-behind for the panel.
//
// The overlay itself is a WS_EX_LAYERED window presented with
// UpdateLayeredWindow; nothing can blur "through" it (DwmEnableBlurBehindWindow
// is a no-op there, and the accent-policy acrylic paints over the per-pixel
// alpha — both tried and reverted, see git history). Instead a companion
// WS_EX_NOREDIRECTIONBITMAP window is kept glued directly BENEATH the overlay
// in z-order, hosting a Windows.UI.Composition visual whose
// CompositionBackdropBrush samples the live desktop content behind the
// companion (verified: windows moving behind update live). Sampling happens at
// the companion's z-position, so the overlay in front is excluded. (The
// documented HostBackdropBrush + DWMWA_USE_HOSTBACKDROPBRUSH route was tried
// first and empirically only delivers a wallpaper-layer snapshot on build
// 26200 — app windows behind never appear; the plain backdrop brush needs no
// DWM attribute at all.) The visual is clipped to the animating rounded panel
// rect every rendered frame, so the blur hugs the pill/panel exactly through
// the morph. The companion never takes input: it is WS_EX_LAYERED |
// WS_EX_TRANSPARENT, the only combination that passes clicks through to
// OTHER PROCESSES' windows below. (HTTRANSPARENT alone only defers within
// the owning thread — and since this window spans the full expanded-panel
// footprint even while the pill is collapsed, anything less silently eats
// every click over the phantom panel area.) It is never activated.
// All composition objects live on the render thread,
// whose message pump doubles as the required DispatcherQueue pump. If anything
// in the chain fails (or the user disables transparency effects), the host
// stays hidden and the panel falls back to the dense tint-only presets.
// ===========================================================================
constexpr wchar_t kBlurWindowClass[] = L"Windhawk.IslandCommandCenter.Blur";

LRESULT CALLBACK BlurHostWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCHITTEST) return HTTRANSPARENT;  // input always falls through
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// Render-thread only: live Windows theme colors -- accent (+ a pre-darkened ramp for the no-blur
// fallback tint) and the system chrome/background color -- cached and polled every 2s (theme
// changes are rare; querying UISettings 60x/second for values that almost never change would be
// wasteful). Reuses the render thread's existing COM apartment (CoInitializeEx in
// RenderThreadProc; no separate winrt::init_apartment needed). Deliberately the SAME detection
// mechanism the windows-11-taskbar-styler mod uses (WinRT UISettings, not a registry poll) --
// its WindowGlass background is itself a `{ThemeResource ...}` system chrome color, not a fixed
// hex value, so matching it means tracking the same live system color rather than picking our own.
struct SystemColors {
    float r = 0.20f, g = 0.55f, b = 1.0f;                      // accent
    float dr = 0.11f, dg = 0.31f, db = 0.57f;                  // accent, darkened (fallback-tint ramp)
    float chromeR = 0.13f, chromeG = 0.13f, chromeB = 0.145f;  // live system chrome/background color
    bool isLight = true;
};
SystemColors GetSystemColorsCached() {
    static SystemColors cached;
    static double checkedAt = -10.0;
    const double now = NowSeconds();
    if (now - checkedAt > 2.0) {
        checkedAt = now;
        try {
            using winrt::Windows::UI::ViewManagement::UISettings;
            using winrt::Windows::UI::ViewManagement::UIColorType;
            UISettings us;
            auto toF = [](winrt::Windows::UI::Color c) {
                return std::array<float, 3>{c.R / 255.0f, c.G / 255.0f, c.B / 255.0f};
            };
            auto acc = toF(us.GetColorValue(UIColorType::Accent));
            auto dark = toF(us.GetColorValue(UIColorType::AccentDark2));
            auto bg = toF(us.GetColorValue(UIColorType::Background));
            cached.r = acc[0]; cached.g = acc[1]; cached.b = acc[2];
            cached.dr = dark[0]; cached.dg = dark[1]; cached.db = dark[2];
            cached.chromeR = bg[0]; cached.chromeG = bg[1]; cached.chromeB = bg[2];
            // Background's own luminance tells us which side is currently active (a light chrome
            // color IS the light theme) -- the same signal ThemeResource colors track internally.
            cached.isLight = (0.299f * bg[0] + 0.587f * bg[1] + 0.114f * bg[2]) > 0.5f;
        } catch (...) {}  // keep the last-known (or default) colors on failure
    }
    return cached;
}
// Fills any accent-flagged channel of a (per-frame local copy of a) preset with the live accent
// color. Never mutates the shared Settings recipe -- callers pass their own per-frame copy.
void ResolveAccentColors(BackdropPreset& preset) {
    if (!preset.liveUsesAccent && !preset.fallbackUsesAccent) return;
    SystemColors sc = GetSystemColorsCached();
    if (preset.liveUsesAccent) { preset.r = sc.r; preset.g = sc.g; preset.b = sc.b; }
    if (preset.fallbackUsesAccent) { preset.fr = sc.dr; preset.fg = sc.dg; preset.fb = sc.db; }
}
// Fills any theme-flagged channel with a neutral shade lerped by the eased theme position
// (0 = dark neutral, 1 = light neutral) -- the "theme" TintColor/FallbackColor keyword, and the
// default for both. Takes themeT explicitly (rather than reading g_themeEngine itself) so the
// caller controls ordering: this must run AFTER ThemeEngine::Step produces this frame's value.
// The CURRENTLY active side uses the live system chrome color (an exact match for the taskbar
// styler's own ThemeResource-based background); the other side -- only ever visible transiently,
// mid theme-change fade, since UISettings can't expose "what light mode would look like" while
// dark mode is active -- falls back to the well-known Windows 11 default for that mode.
void ResolveThemeColors(BackdropPreset& preset, float themeT) {
    if (!preset.liveUsesTheme && !preset.fallbackUsesTheme) return;
    constexpr float kDarkFallback[3] = {0.125f, 0.125f, 0.125f};   // Win11 dark chrome (~#202020)
    constexpr float kLightFallback[3] = {0.953f, 0.953f, 0.953f};  // Win11 light chrome (~#F3F3F3)
    SystemColors sc = GetSystemColorsCached();
    const float dr = sc.isLight ? kDarkFallback[0] : sc.chromeR;
    const float dg = sc.isLight ? kDarkFallback[1] : sc.chromeG;
    const float db = sc.isLight ? kDarkFallback[2] : sc.chromeB;
    const float lr = sc.isLight ? sc.chromeR : kLightFallback[0];
    const float lg = sc.isLight ? sc.chromeG : kLightFallback[1];
    const float lb = sc.isLight ? sc.chromeB : kLightFallback[2];
    const float tr = dr + (lr - dr) * themeT, tg = dg + (lg - dg) * themeT, tb = db + (lb - db) * themeT;
    if (preset.liveUsesTheme) { preset.r = tr; preset.g = tg; preset.b = tb; }
    if (preset.fallbackUsesTheme) { preset.fr = tr; preset.fg = tg; preset.fb = tb; }
}
// Same detection mechanism (and thus the same behavior) as the windows-11-taskbar-styler mod:
// WinRT UISettings, not a registry poll -- reuses the 2s-cached system-colors query above.
bool SystemPrefersLight() { return GetSystemColorsCached().isLight; }

// ===========================================================================
// Theme engine
// ===========================================================================
// Resolves the Theme setting (auto/dark/light/adaptive) to a single smoothed position `themeT`
// (0 = dark theme/white text, 1 = light theme/black text) that drives both the foreground
// palette (Renderer::ApplyThemeBrushes) and the "theme" tint keyword (ResolveThemeColors).
// Render-thread only; replaces the old per-Renderer themeDark_/themeT_ hysteresis.
class ThemeEngine {
   public:
    float Step(const Settings& s, const BackdropPreset& folded, double now) {
        switch (s.themeMode) {
            case ThemeMode::Dark: lightSide_ = false; break;
            case ThemeMode::Light: lightSide_ = true; break;
            case ThemeMode::Adaptive: StepAdaptive(s, folded, now); break;
            default: lightSide_ = SystemPrefersLight(); break;  // Auto
        }
        const float target = lightSide_ ? 1.0f : 0.0f;
        const float dt = tickAt_ < 0.0 ? (1.0f / 60.0f)
                                        : static_cast<float>(std::min(now - tickAt_, 0.1));
        tickAt_ = now;
        // tau 0.35s -> a deliberately slow, smooth fade (~1.2s to fully settle), vs the old 0.08s.
        t_ += (target - t_) * (1.0f - std::exp(-dt / 0.35f));
        if (std::fabs(t_ - target) < 0.004f) t_ = target;
        return t_;
    }
    bool Animating() const { return t_ != (lightSide_ ? 1.0f : 0.0f); }

   private:
    // Hysteresis (0.52 dark->light / 0.44 light->dark) on the reading surface, but a candidate
    // flip must hold CONTINUOUSLY for 3s before it commits -- sliding a window across the pill
    // for a second or two can no longer flip the theme (replaces the old 1s min-dwell).
    void StepAdaptive(const Settings& s, const BackdropPreset& folded, double now) {
        const float behind = g_behindLum.load();
        if (behind < 0.0f) return;  // no sample yet -> hold the current side
        // Feedback-loop fix: when the tint itself is theme-driven, the hysteresis input MUST be
        // the raw background sample, not the composited surface -- a light theme tint at focus
        // opacity reads as "light" over almost anything behind it, which would permanently latch
        // light. Only fixed (accent/hex) tints are genuinely a different reading surface.
        float surface = behind;
        if (!folded.liveUsesTheme && !folded.fallbackUsesTheme) {
            const float a0 = g_backdropBlurLive ? folded.a : folded.fa;
            const float tr = g_backdropBlurLive ? folded.r : folded.fr;
            const float tg = g_backdropBlurLive ? folded.g : folded.fg;
            const float tb = g_backdropBlurLive ? folded.b : folded.fb;
            const float a = s.backdropEnabled ? a0 : a0 * Clamp(s.pillOpacity, 0.35f, 1.0f);
            const float tintLum = 0.299f * tr + 0.587f * tg + 0.114f * tb;
            surface = behind * (1.0f - a) + tintLum * a;
        }
        const bool desiredDiffers = (!lightSide_ && surface > 0.52f) || (lightSide_ && surface < 0.44f);
        if (desiredDiffers) {
            if (crossSince_ < 0.0) crossSince_ = now;
            if (now - crossSince_ >= 3.0) { lightSide_ = !lightSide_; crossSince_ = -1.0; }
        } else {
            crossSince_ = -1.0;  // back inside the band (or already the committed side) -> cancel
        }
    }

    bool lightSide_ = false;
    float t_ = 0.0f;
    double tickAt_ = -1.0;
    double crossSince_ = -1.0;
};
ThemeEngine g_themeEngine;

class BackdropBlurHost {
   public:
    // Per rendered frame, BEFORE BeginFrame: bring init/brush in line with the style and report
    // whether real blur is live (BeginFrame picks the panel tint variant from that). Never throws.
    bool Prepare(bool enabled, const BackdropPreset& preset, HWND overlay) {
        if (!enabled || failed_ || !TransparencyEnabled()) {
            Hide();
            return false;
        }
        if (!EnsureInit(overlay)) return false;
        // The composition brush only encodes blur + saturation; tint color is applied later in
        // D2D (Renderer::BeginFrame), so only those two need to trigger a rebuild. Blur is
        // DPI-independent (see RebuildBrush), so scale is deliberately NOT part of this.
        if (std::fabs(preset.blur - builtBlur_) > 0.01f ||
            std::fabs(preset.saturation - builtSaturation_) > 0.001f)
            RebuildBrush(preset);
        return !failed_;
    }

    // After the frame's panel rect is known: clip the blur to the (animating) rounded panel rect,
    // mirror the overlay window's rect, and keep the companion directly beneath it in z-order.
    void SyncGeometry(HWND overlay, D2D1_RECT_F panelR, float radius) {
        if (!host_ || failed_) return;
        try {  // clip BEFORE the first show so no unclipped blur ever flashes
            clip_.Left(panelR.left);
            clip_.Top(panelR.top);
            clip_.Right(panelR.right);
            clip_.Bottom(panelR.bottom);
            winrt::Windows::Foundation::Numerics::float2 r{radius, radius};
            clip_.TopLeftRadius(r);
            clip_.TopRightRadius(r);
            clip_.BottomLeftRadius(r);
            clip_.BottomRightRadius(r);
        } catch (...) {}
        RECT wr;
        if (!GetWindowRect(overlay, &wr)) return;
        const bool moved = memcmp(&wr, &lastRect_, sizeof(RECT)) != 0;
        // Reassert the sandwich whenever the overlay moved, we were hidden, or another window
        // slipped between the overlay and the blur host.
        if (moved || !shown_ || GetWindow(overlay, GW_HWNDNEXT) != host_) {
            SetWindowPos(host_, overlay, wr.left, wr.top, wr.right - wr.left, wr.bottom - wr.top,
                         SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);
            lastRect_ = wr;
            shown_ = true;
        }
    }

    // Fades the blur visual itself in lockstep with the main window's own alpha (temp-hide) --
    // without this the blur cuts instantly while the D2D panel/text keep fading, a visible seam
    // between "background" (blur) and "foreground" (panel + content) during the hide animation.
    void SetOpacity(float alpha) {
        if (visual_) { try { visual_.Opacity(alpha); } catch (...) {} }
    }

    void Shutdown() {
        g_backdropBlurLive = false;
        Teardown();
    }

   private:
    void Hide() {
        if (shown_ && host_) {
            ShowWindow(host_, SW_HIDE);
            shown_ = false;
        }
    }

    // The "Transparency effects" personalization toggle gates DWM backdrop sampling; when it's
    // off the host backdrop delivers a flat fallback. Poll cheaply and fall back to tint-only.
    bool TransparencyEnabled() {
        const double now = NowSeconds();
        if (now - transCheckedAt_ > 2.0) {
            transCheckedAt_ = now;
            DWORD v = 1, sz = sizeof(v);
            if (RegGetValueW(HKEY_CURRENT_USER,
                             L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                             L"EnableTransparency", RRF_RT_REG_DWORD, nullptr, &v, &sz) == ERROR_SUCCESS)
                transparencyOn_ = v != 0;
            else
                transparencyOn_ = true;  // value absent -> effects on
        }
        return transparencyOn_;
    }

    bool EnsureInit(HWND overlay) {
        if (compositor_) return true;
        failed_ = true;  // cleared only on full success; a partial init never retries
        static bool classRegistered = false;
        if (!classRegistered) {
            WNDCLASSEXW wc = {};
            wc.cbSize = sizeof(wc);
            wc.lpfnWndProc = BlurHostWndProc;
            wc.hInstance = GetModuleHandleW(nullptr);
            wc.lpszClassName = kBlurWindowClass;
            if (!RegisterClassExW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
                Wh_Log(L"Backdrop blur: RegisterClassEx failed (%u)", GetLastError());
                return false;
            }
            classRegistered = true;
        }
        RECT wr = {0, 0, 360, 96};
        GetWindowRect(overlay, &wr);
        // WS_EX_LAYERED + WS_EX_TRANSPARENT is required for real click-through: this window
        // covers the full expanded-panel footprint even when collapsed, and WS_EX_TRANSPARENT
        // (or HTTRANSPARENT) without LAYERED only yields to same-thread windows — clicks aimed
        // at other apps behind the phantom panel area get eaten. With no redirection bitmap the
        // layered machinery has no GDI surface to gate, so the composition visual still shows.
        host_ = CreateWindowExW(
            WS_EX_NOREDIRECTIONBITMAP | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
                WS_EX_LAYERED | WS_EX_TRANSPARENT,
            kBlurWindowClass, L"Island Backdrop", WS_POPUP, wr.left, wr.top,
            std::max(1L, wr.right - wr.left), std::max(1L, wr.bottom - wr.top), nullptr, nullptr,
            GetModuleHandleW(nullptr), nullptr);
        if (!host_) {
            Wh_Log(L"Backdrop blur: CreateWindowEx failed (%u)", GetLastError());
            return false;
        }
        // A layered window is only considered "committed" once its attributes are set; harmless
        // no-op for the (absent) redirection surface, but guarantees DWM presents the visual.
        SetLayeredWindowAttributes(host_, 0, 255, LWA_ALPHA);
        try {
            DispatcherQueueOptions dqo{sizeof(DispatcherQueueOptions), DQTYPE_THREAD_CURRENT,
                                       DQTAT_COM_NONE};
            HRESULT hr = CreateDispatcherQueueController(
                dqo, reinterpret_cast<PDISPATCHERQUEUECONTROLLER*>(winrt::put_abi(controller_)));
            if (FAILED(hr)) {
                Wh_Log(L"Backdrop blur: CreateDispatcherQueueController failed (0x%08X)",
                       (unsigned)hr);
                Teardown();
                return false;
            }
            compositor_ = wuc::Compositor();
            auto interop = compositor_.as<ABI::Windows::UI::Composition::ICompositorDesktopInterop>();
            hr = interop->CreateDesktopWindowTarget(host_, TRUE, winrt::put_abi(target_));
            if (FAILED(hr)) {
                Wh_Log(L"Backdrop blur: CreateDesktopWindowTarget failed (0x%08X)", (unsigned)hr);
                Teardown();
                return false;
            }
            visual_ = compositor_.CreateSpriteVisual();
            visual_.RelativeSizeAdjustment({1.0f, 1.0f});
            clip_ = compositor_.CreateRectangleClip();
            visual_.Clip(clip_);
            target_.Root(visual_);
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"Backdrop blur: composition init failed (0x%08X)", (unsigned)e.code().value);
            Teardown();
            return false;
        } catch (...) {
            Wh_Log(L"Backdrop blur: composition init failed");
            Teardown();
            return false;
        }
        builtBlur_ = -1.0f; builtSaturation_ = -1.0f;  // sentinels: force the first RebuildBrush
        failed_ = false;
        Wh_Log(L"Backdrop blur: composition pipeline ready.");
        return true;
    }

    void RebuildBrush(const BackdropPreset& preset) {
        try {
            // Plain backdrop brush = live, SHARP desktop content behind the companion window
            // (the WindowGlass taskbar theme blurs with the same source). HostBackdropBrush was
            // tried and only delivers a wallpaper-layer snapshot on this build.
            auto backdrop = compositor_.CreateBackdropBrush();
            // BlurAmount is a Gaussian standard deviation in LOGICAL pixels; the D2D composition
            // effect graph computes it DPI-independently, so it must NOT be multiplied by the
            // island's DPI/size scale. (It was — on a 175% monitor that ~2x over-blurred every
            // preset into full saturation, so Glass/Frosted/Acrylic looked identical and
            // BlurAmount felt inert above ~3. Use the WindowGlass value directly, as the taskbar
            // styler does.) radius 0 disables the D2D blur outright (documented).
            const float radius = preset.blur;
            const bool wantSat = std::fabs(preset.saturation - 1.0f) > 0.01f;
            if (radius < 0.5f && !wantSat) {
                visual_.Brush(backdrop);
            } else {
                wge::IGraphicsEffectSource top = wuc::CompositionEffectSourceParameter(L"backdrop");
                if (radius >= 0.5f) {
                    auto blur = winrt::make_self<icc_fx::GaussianBlurEffect>();
                    blur->Source = top;
                    blur->BlurAmount = radius;
                    top = *blur;
                }
                if (wantSat) {
                    auto sat = winrt::make_self<icc_fx::ColorMatrixEffect>();
                    sat->Source = top;
                    sat->SetSaturation(preset.saturation);
                    top = *sat;
                }
                auto factory = compositor_.CreateEffectFactory(top.as<wge::IGraphicsEffect>());
                auto brush = factory.CreateBrush();
                brush.SetSourceParameter(L"backdrop", backdrop);
                visual_.Brush(brush);
            }
            builtBlur_ = preset.blur;
            builtSaturation_ = preset.saturation;
        } catch (const winrt::hresult_error& e) {
            Wh_Log(L"Backdrop blur: brush build failed (0x%08X)", (unsigned)e.code().value);
            failed_ = true;
            Hide();
        } catch (...) {
            Wh_Log(L"Backdrop blur: brush build failed");
            failed_ = true;
            Hide();
        }
    }

    void Teardown() {
        try { if (visual_) visual_.Brush(nullptr); } catch (...) {}
        clip_ = nullptr;
        visual_ = nullptr;
        try { if (target_) target_.Root(nullptr); } catch (...) {}
        target_ = nullptr;
        compositor_ = nullptr;
        if (controller_) {
            // DispatcherQueue contract: shut the queue down and keep pumping until it completes.
            try {
                auto action = controller_.ShutdownQueueAsync();
                const ULONGLONG deadline = GetTickCount64() + 2000;
                while (action.Status() == winrt::Windows::Foundation::AsyncStatus::Started &&
                       GetTickCount64() < deadline) {
                    MSG msg;
                    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                        TranslateMessage(&msg);
                        DispatchMessageW(&msg);
                    }
                    Sleep(5);
                }
            } catch (...) {}
            controller_ = nullptr;
        }
        if (host_) {
            DestroyWindow(host_);
            host_ = nullptr;
        }
        shown_ = false;
    }

    HWND host_ = nullptr;
    bool failed_ = false;
    bool shown_ = false;
    RECT lastRect_ = {};
    float builtBlur_ = -1.0f;
    float builtSaturation_ = -1.0f;
    bool transparencyOn_ = true;
    double transCheckedAt_ = -10.0;
    winrt::Windows::System::DispatcherQueueController controller_{nullptr};
    wuc::Compositor compositor_{nullptr};
    wucd::DesktopWindowTarget target_{nullptr};
    wuc::SpriteVisual visual_{nullptr};
    wuc::RectangleClip clip_{nullptr};
};

// ---------------------------------------------------------------------------
// Brightness control (DDC/CI on the island's monitor; runs off the render thread)
// ---------------------------------------------------------------------------
std::atomic<int> g_brightPercent{-1};   // -1 = unsupported / hidden
std::atomic<int> g_brightSetRequest{-1};
std::atomic<bool> g_brightRebind{false};
HANDLE g_brightEvent = nullptr;         // auto-reset; signals set/rebind
HANDLE g_brightThread = nullptr;

DWORD WINAPI BrightnessThreadProc(void*) {
    std::vector<PHYSICAL_MONITOR> phys;
    HANDLE active = nullptr;
    DWORD minB = 0, curB = 0, maxB = 0;
    bool haveCaps = false;

    auto cleanup = [&]() {
        if (!phys.empty()) { DestroyPhysicalMonitors((DWORD)phys.size(), phys.data()); phys.clear(); }
        active = nullptr; haveCaps = false;
    };
    auto pctFrom = [&](DWORD cur) -> int {
        if (cur < minB) cur = minB;
        if (cur > maxB) cur = maxB;
        return (maxB > minB) ? (int)(((long long)(cur - minB) * 100 + (maxB - minB) / 2) / (maxB - minB)) : 0;
    };
    auto rebind = [&]() {
        cleanup();
        Settings s = SettingsSnapshot();
        if (!s.brightnessEnabled) { g_brightPercent = -1; return; }
        HMONITOR hm = GetAnchorMonitor(s);
        DWORD n = 0;
        if (!GetNumberOfPhysicalMonitorsFromHMONITOR(hm, &n) || n == 0) { g_brightPercent = -1; return; }
        phys.resize(n);
        if (!GetPhysicalMonitorsFromHMONITOR(hm, n, phys.data())) { phys.clear(); g_brightPercent = -1; return; }
        for (auto& pm : phys) {
            DWORD mn = 0, cu = 0, mx = 0;
            if (GetMonitorBrightness(pm.hPhysicalMonitor, &mn, &cu, &mx)) {
                minB = mn; curB = cu; maxB = mx; haveCaps = true; active = pm.hPhysicalMonitor;
                g_brightPercent = pctFrom(cu);
                return;
            }
        }
        g_brightPercent = -1;  // no monitor honored DDC/CI
    };

    rebind();
    HANDLE waits[2] = {g_stopEvent, g_brightEvent};
    while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        DWORD wr = WaitForMultipleObjects(2, waits, FALSE, 3000);
        if (wr == WAIT_OBJECT_0) break;
        if (g_brightRebind.exchange(false)) { rebind(); g_layoutDirty = true; continue; }
        int req = g_brightSetRequest.exchange(-1);
        if (req >= 0 && haveCaps) {
            DWORD val = minB + (DWORD)((double)(maxB - minB) * Clamp(req / 100.0f, 0.0f, 1.0f) + 0.5);
            if (SetMonitorBrightness(active, val)) { curB = val; g_brightPercent = pctFrom(val); RevealIslandForEvent(); }
            else { g_brightPercent = -1; cleanup(); }
            g_layoutDirty = true;
        }
        if (wr == WAIT_TIMEOUT && haveCaps) {
            DWORD mn = 0, cu = 0, mx = 0;
            if (GetMonitorBrightness(active, &mn, &cu, &mx)) {
                minB = mn; maxB = mx;
                int p = pctFrom(cu);
                if (p != g_brightPercent.load()) { g_brightPercent = p; RevealIslandForEvent(); g_layoutDirty = true; }
            }
        }
    }
    cleanup();
    return 0;
}

// ===========================================================================
// Surface — state machine + routing
// ===========================================================================
enum class SurfaceState { Collapsed, TransientPopup, Expanded, Open };

void InstallDismissHook();
void RemoveDismissHook();

class Surface {
   public:
    SurfaceState state = SurfaceState::Collapsed;

    void BuildFor(SurfaceState s, const DrawContext& dc) {
        if (captureTarget_) ReleaseCapture();  // don't leak OS capture if we rebuild mid-drag
        state = s;
        root_ = BuildRoot(s, dc);
        captureTarget_ = nullptr;
        pressTarget_ = nullptr;
        builtNotifGen_ = g_notifGen.load();  // snapshot the notif list version this tree was built from
        builtScale_ = dc.scale;
    }

    // The initial BuildFor(Collapsed, ...) at startup uses a placeholder scale of 1.0 (no real
    // window/DPI yet). If the surface never transitions state before the real per-monitor scale is
    // known, sizes baked into the tree (e.g. Custom::fixedHeight, captured by value at build time)
    // stay wrong forever. Call this every frame; it rebuilds the CURRENT state at the real scale the
    // first time they differ (and again on any later DPI/SizeScale change).
    bool RebuildIfScaleChanged(const DrawContext& dc) {
        if (std::fabs(dc.scale - builtScale_) > 0.001f) { BuildFor(state, dc); return true; }
        return false;
    }

    // The Open panel's notification rows are baked at build time; when the list changes
    // (arrival / dismiss / clear-all) rebuild the tree in place so it refreshes without a
    // close/open. Skip while dragging so a rebuild never drops an in-flight slider capture.
    bool RebuildIfDataChanged(const DrawContext& dc) {
        if (state == SurfaceState::Open && !captureTarget_ && g_notifGen.load() != builtNotifGen_) {
            BuildFor(SurfaceState::Open, dc);
            return true;
        }
        return false;
    }

    // Returns desired content size for the current state.
    D2D1_SIZE_F MeasureContent(const DrawContext& dc) {
        SetLayoutDc(root_.get(), &dc);
        float width = StateWidth(dc);
        if (state == SurfaceState::Collapsed || state == SurfaceState::TransientPopup) {
            float pw = root_ ? root_->PreferredWidth(dc) : -1.0f;
            if (pw > 0) width = pw;
        }
        float height = root_ ? root_->Measure(dc, width) : 40.0f * dc.scale;
        float maxH = MaxOpenHeight(dc);
        if (state == SurfaceState::Open && height > maxH) height = maxH;
        return D2D1::SizeF(width, height);
    }

    void SetLocked(D2D1_SIZE_F sz) { lockedW_ = sz.width; lockedH_ = sz.height; }
    float LockedW() const { return lockedW_; }
    float LockedH() const { return lockedH_; }

    void Layout(const DrawContext& dc, D2D1_RECT_F inner) {
        SetLayoutDc(root_.get(), &dc);
        if (!root_) return;
        // Lay out at the LOCKED (settled) content size, centered in the animating inner rect, so
        // the tree never re-flows at intermediate morph sizes — it is simply revealed by the
        // growing panel (the caller clips to inner). SNAP the origin to whole device pixels and keep
        // the width CONSTANT (lockedW_) so the clock/text glyphs keep the same sub-pixel phase every
        // frame; a fractional, per-frame-shifting origin makes DirectWrite re-rasterize them, which
        // reads as horizontal shimmer/blur during the morph. At rest inner == the locked size.
        float lw = lockedW_ > 0 ? lockedW_ : W(inner);
        float lh = lockedH_ > 0 ? lockedH_ : H(inner);
        float left = std::floor((inner.left + inner.right - lw) * 0.5f + 0.5f);
        float top  = std::floor(inner.top + 0.5f);
        root_->Layout(D2D1::RectF(left, top, left + lw, top + lh));
    }
    void Paint(DrawContext& dc) { if (root_) root_->Paint(dc); }

    bool Tick(float dt) { return root_ ? TickTree(root_.get(), dt) : false; }

    // ---- pointer ----
    void OnMove(const PointerEvent& e) {
        if (captureTarget_) {
            bool wc; captureTarget_->OnPointer(e, wc);
        }
    }
    // Hit-tests unconditionally (not just when Open): outside Open, only CollapsedPill's mini
    // volume hotspot ever returns a non-null, capture-requesting hit (a plain Custom leaf's base
    // HitTestDeep returns itself but its OnPointer is unhandled) -- so a click anywhere else on
    // the pill still comes back `handled=false` and the caller (WM_LBUTTONDOWN) opens the panel.
    bool OnDown(const PointerEvent& e) {
        Widget* hit = root_ ? root_->HitTestDeep(e.pos) : nullptr;
        pressTarget_ = hit;
        if (hit) {
            bool wc = false;
            bool handled = hit->OnPointer(e, wc);
            if (wc) { captureTarget_ = hit; SetCapture(g_hwnd); }
            return handled;
        }
        return false;
    }
    bool OnUp(const PointerEvent& e) {
        if (captureTarget_) {
            bool wc; captureTarget_->OnPointer(e, wc);
            captureTarget_ = nullptr;
            ReleaseCapture();
            return true;
        }
        if (state != SurfaceState::Open) return false;
        Widget* hit = root_ ? root_->HitTestDeep(e.pos) : nullptr;
        if (hit && hit == pressTarget_) { bool wc; return hit->OnPointer(e, wc); }
        return false;
    }
    void OnWheel(const PointerEvent& e) {
        // Anywhere over the hovered pill (Collapsed or the Expanded hover-peek) scrolls volume --
        // reachable only while hovered/Open since the window is click-through otherwise (see
        // SetClickThrough). The Open panel keeps its normal scroll-container routing.
        if (state != SurfaceState::Open) {
            SetSystemVolume(g_volScalar.load() + 0.02f * (e.wheel / 36.0f));  // 1 notch = 36 -> 2%
            return;
        }
        Widget* w = root_ ? root_->HitTestDeep(e.pos) : nullptr;
        while (w) {
            if (ScrollContainer* sc = w->AsScroll()) { sc->OnWheel(e.wheel); g_layoutDirty = true; return; }
            w = w->parent;
        }
    }
    bool IsCapturing() const { return captureTarget_ != nullptr; }

    void ForEachSlider(const std::function<void(Slider*)>& fn) { ForEachSliderImpl(root_.get(), fn); }

    // Drain any pending slider/mini-volume drag into a real value-set (needs dc.scale for Slider).
    void FlushPendingDrag(const DrawContext& dc) {
        if (auto* sl = dynamic_cast<Slider*>(captureTarget_)) {
            if (sl->HasPending()) sl->Apply(dc, sl->TakePending());
        } else if (auto* cp = dynamic_cast<CollapsedPill*>(captureTarget_)) {
            if (cp->HasPending()) cp->Apply(cp->TakePending());
        }
    }
    // Screen-space-free (window-content-space) hotspot of the collapsed pill's mini volume
    // element, or an empty rect outside Collapsed -- lets the render loop hold the pill collapsed
    // while the cursor is over it (see UpdateAmbientState) instead of instantly hover-expanding.
    D2D1_RECT_F VolumeHotspot() const {
        if (state != SurfaceState::Collapsed) return D2D1_RECT_F{};
        if (auto* cp = dynamic_cast<CollapsedPill*>(root_.get())) return cp->Hotspot();
        return D2D1_RECT_F{};
    }
    // The tallest the island could ever be (the Open panel, clamped to the monitor) -- used to
    // build a generous "territory" rect for the temp-hide auto-reshow-on-mouse-leave logic.
    float MaxPossibleHeight(const DrawContext& dc) const { return MaxOpenHeight(dc); }

    void Open() {
        if (state == SurfaceState::Open) return;
        pendingState_ = SurfaceState::Open;
        InstallDismissHook();
    }
    void Dismiss() {
        if (state == SurfaceState::Collapsed) return;
        pendingState_ = SurfaceState::Collapsed;
        RemoveDismissHook();
    }

    void PinExpanded() {
        g_pinExpanded = true;
        if (state != SurfaceState::Open && state != SurfaceState::Expanded) pendingState_ = SurfaceState::Expanded;
        g_layoutDirty = true;
    }

    void UnpinExpanded() { g_pinExpanded = false; g_layoutDirty = true; }
    // Drive Collapsed / TransientPopup / Expanded transitions (Open is click-driven). Never rebuild
    // while a widget is capturing (e.g. a mini-volume drag) -- BuildFor releases OS capture on any
    // rebuild, so hover churn mid-drag would silently drop it. Also never clobber a JUST-QUEUED
    // Open: WM_LBUTTONDOWN calls Open() (queuing pendingState_) then this runs later in the SAME
    // frame, still seeing the OLD `state` (Collapsed) since ApplyPendingState hasn't run yet --
    // without this check, a click on an already-hovered pill would queue Open only to have this
    // immediately overwrite it back to Expanded, silently turning every click into a hover-peek.
    void UpdateAmbientState(bool hovered, double now, bool overVolumeHotspot) {
        if (g_pinExpanded) return;
        if (state == SurfaceState::Open || IsCapturing()) return;
        if (pendingState_.has_value() && *pendingState_ == SurfaceState::Open) return;
        if (hovered) {
            lastHoverTime_ = now;
            // Hold Collapsed while the cursor is over the volume hotspot -- the instant hover
            // expand would otherwise rebuild the tree (and the hotspot under it) before the user
            // gets a chance to click or drag it.
            if (state == SurfaceState::Collapsed && overVolumeHotspot) return;
            if (state != SurfaceState::Expanded) pendingState_ = SurfaceState::Expanded;
            return;
        }
        if (state == SurfaceState::Expanded && now - lastHoverTime_ < 0.40) return;  // smooth hover grace
        SurfaceState want = TransientActive() ? SurfaceState::TransientPopup : SurfaceState::Collapsed;
        if (state != want) pendingState_ = want;
    }
    // Apply a queued state change (rebuilds root). Returns true if changed.
    bool ApplyPendingState(const DrawContext& dc) {
        if (!pendingState_.has_value() || *pendingState_ == state) { pendingState_.reset(); return false; }
        BuildFor(*pendingState_, dc);
        pendingState_.reset();
        return true;
    }

   private:
    float StateWidth(const DrawContext& dc) const {
        switch (state) {
            case SurfaceState::Collapsed:      return 180.0f * dc.scale;
            case SurfaceState::TransientPopup: return 230.0f * dc.scale;
            case SurfaceState::Expanded:       return 360.0f * dc.scale;
            case SurfaceState::Open:           return 360.0f * dc.scale;
        }
        return 180.0f * dc.scale;
    }
    float MaxOpenHeight(const DrawContext& dc) const {
        RECT work = GetAnchorWorkRect(SettingsSnapshot());
        return std::min(620.0f * dc.scale, (work.bottom - work.top) * 0.78f);
    }
    static void SetLayoutDc(Widget* w, const DrawContext* dc) {
        if (!w) return;
        if (auto* sp = dynamic_cast<StackPanel*>(w)) { sp->layoutDc = dc; for (auto& c : sp->children) SetLayoutDc(c.get(), dc); }
        else if (auto* sc = dynamic_cast<ScrollContainer*>(w)) { sc->layoutDc = dc; for (auto& c : sc->children) SetLayoutDc(c.get(), dc); }
    }
    static bool TickTree(Widget* w, float dt) {
        bool anim = w->Tick(dt);
        if (auto* sp = dynamic_cast<StackPanel*>(w)) for (auto& c : sp->children) anim |= TickTree(c.get(), dt);
        else if (auto* sc = dynamic_cast<ScrollContainer*>(w)) for (auto& c : sc->children) anim |= TickTree(c.get(), dt);
        return anim;
    }

    std::unique_ptr<Widget> BuildRoot(SurfaceState s, const DrawContext& dc);

    static void ForEachSliderImpl(Widget* w, const std::function<void(Slider*)>& fn) {
        if (!w) return;
        if (auto* sl = dynamic_cast<Slider*>(w)) fn(sl);
        if (auto* sp = dynamic_cast<StackPanel*>(w)) { for (auto& c : sp->children) ForEachSliderImpl(c.get(), fn); }
        else if (auto* sc = dynamic_cast<ScrollContainer*>(w)) { for (auto& c : sc->children) ForEachSliderImpl(c.get(), fn); }
    }

    std::unique_ptr<Widget> root_;
    Widget* captureTarget_ = nullptr;
    Widget* pressTarget_ = nullptr;
    std::optional<SurfaceState> pendingState_;
    double lastHoverTime_ = 0.0;
    uint64_t builtNotifGen_ = 0;
    float lockedW_ = 0.0f, lockedH_ = 0.0f;  // frozen target size during a morph
    float builtScale_ = -1.0f;  // scale the current root_ was built at; -1 forces the first real rebuild
};

// ---- formatting helpers ----
std::wstring TimeString() {
    SYSTEMTIME t; GetLocalTime(&t);
    wchar_t buf[32] = {};
    GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &t, nullptr, buf, ARRAYSIZE(buf));
    return buf;
}
// Changes once per displayed minute — used as a cheap "the clock text differs now" render trigger.
int ClockMinuteKey() {
    SYSTEMTIME t; GetLocalTime(&t);
    return t.wHour * 60 + t.wMinute;
}
std::wstring DateStringShort() {
    SYSTEMTIME t; GetLocalTime(&t);
    wchar_t buf[64] = {};
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &t, L"ddd, MMM d", buf, ARRAYSIZE(buf), nullptr);
    return buf;
}
// Compact weekday + numeric date for the collapsed pill, e.g. "Wed 7/2" (no year).
std::wstring DatePillString() {
    SYSTEMTIME t; GetLocalTime(&t);
    wchar_t buf[32] = {};
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &t, L"ddd M/d", buf, ARRAYSIZE(buf), nullptr);
    return buf;
}
std::wstring DateStringLong() {
    SYSTEMTIME t; GetLocalTime(&t);
    wchar_t buf[96] = {};
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &t, L"dddd, MMMM d", buf, ARRAYSIZE(buf), nullptr);
    return buf;
}
bool WeatherFresh() { WeatherState w = WeatherSnapshot(); return w.hasData && NowSeconds() - w.updated <= 7200.0; }
std::wstring WeatherShort() {
    WeatherState w = WeatherSnapshot();
    if (!w.hasData) return L"";
    return WeatherGlyph(w.code) + L"  " + std::to_wstring(w.temp) + L"\x00B0";
}
// Secondary weather line for the hover peek: "Feels 83°   ·   Humidity 8%   ·   11 mph SSW".
std::wstring WeatherDetail() {
    WeatherState w = WeatherSnapshot();
    if (!w.hasData) return L"";
    std::wstring s = L"Feels " + std::to_wstring(w.feelsLike) + L"\x00B0";
    s += L"    \x00B7    Humidity " + std::to_wstring(w.humidity) + L"%";
    if (w.windSpeed > 0) {
        s += L"    \x00B7    " + std::to_wstring(w.windSpeed) +
             (SettingsSnapshot().weatherFahrenheit ? L" mph" : L" km/h");
        if (!w.windDir.empty()) s += L" " + w.windDir;
    }
    return s;
}
// The next N not-yet-ended events for TODAY ONLY (no spillover into future days), excluding
// all-day events, sorted by start time. Cached (render thread only) by calendar generation +
// minute so it isn't recomputed from the full ~289-event store on every hover frame.
std::vector<IcsEvent> NextUpcomingEvents(int maxN) {
    static uint64_t cGen = (uint64_t)-1; static int cMin = -1; static int cN = -1;
    static std::vector<IcsEvent> cache;
    uint64_t gen = g_calGen.load(); int mk = ClockMinuteKey();
    if (gen == cGen && mk == cMin && maxN == cN) return cache;
    cGen = gen; cMin = mk; cN = maxN;
    auto stamp = [](const SYSTEMTIME& st) {
        FILETIME f{}; SystemTimeToFileTime(&st, &f);
        ULARGE_INTEGER u; u.LowPart = f.dwLowDateTime; u.HighPart = f.dwHighDateTime; return u.QuadPart;
    };
    SYSTEMTIME nowSt; GetLocalTime(&nowSt);
    unsigned long long nowU = stamp(nowSt);
    std::vector<IcsEvent> out;
    for (auto& e : CalendarSnapshot()) {
        if (e.allDay) continue;
        if (!(e.start.wYear == nowSt.wYear && e.start.wMonth == nowSt.wMonth && e.start.wDay == nowSt.wDay)) continue;
        unsigned long long endU = stamp(e.end.wYear ? e.end : e.start);
        if (endU > nowU) out.push_back(e);
    }
    std::sort(out.begin(), out.end(), [&](const IcsEvent& a, const IcsEvent& b) { return stamp(a.start) < stamp(b.start); });
    if ((int)out.size() > maxN) out.resize(maxN);
    cache = std::move(out);
    return cache;
}

std::unique_ptr<Widget> Surface::BuildRoot(SurfaceState s, const DrawContext& dc) {
    if (s == SurfaceState::Collapsed) {
        // [ time / date ]  |  volume glyph+track  |  weather — time is the hero with a small bold
        // date sub-text beneath; the old plain divider is now the mini volume element.
        return std::make_unique<CollapsedPill>();
    }

    if (s == SurfaceState::TransientPopup) {
        auto v = std::make_unique<Custom>();
        v->fixedHeight = 44.0f * dc.scale;
        v->prefWidth = [](const DrawContext& d) {
            Transient t = TransientSnapshot();
            float tw = d.MeasureWidth(d.fPill, t.line);
            return 56.0f * d.scale + tw + (t.kind == 2 ? 70.0f * d.scale : 24.0f * d.scale);
        };
        v->paint = [](DrawContext& d, D2D1_RECT_F b) {
            Transient t = TransientSnapshot();
            D2D1_RECT_F gr = D2D1::RectF(b.left + 14.0f * d.scale, b.top, b.left + 40.0f * d.scale, b.bottom);
            d.Text(t.glyph, d.fGlyph ? d.fGlyph : d.fPill, gr, d.accent, 0.95f, DWRITE_TEXT_ALIGNMENT_CENTER);
            if (t.kind == 2) {  // volume: label + bar + percent
                D2D1_RECT_F lr = D2D1::RectF(b.left + 44.0f * d.scale, b.top + 6.0f * d.scale, b.right - 12.0f * d.scale, b.top + 22.0f * d.scale);
                d.Text(t.muted ? L"Muted" : t.line, d.fSmall, lr, d.muted, 0.8f);
                float shown = Clamp(g_volDisplayScalar.load(), 0.0f, 1.0f);  // smoothed, animates
                wchar_t pc[8]; swprintf_s(pc, L"%d%%", (int)(shown * 100.0f + 0.5f));
                d.Text(pc, d.fSmall, lr, d.text, 0.9f, DWRITE_TEXT_ALIGNMENT_TRAILING);
                D2D1_RECT_F tr = D2D1::RectF(b.left + 44.0f * d.scale, b.bottom - 14.0f * d.scale, b.right - 12.0f * d.scale, b.bottom - 10.0f * d.scale);
                float trad = H(tr) * 0.5f;
                d.dc->FillRoundedRectangle(D2D1::RoundedRect(tr, trad, trad), d.track);
                D2D1_RECT_F fill = tr; fill.right = tr.left + W(tr) * shown;
                d.dc->FillRoundedRectangle(D2D1::RoundedRect(fill, trad, trad), d.accent);
            } else {
                D2D1_RECT_F lr = D2D1::RectF(b.left + 44.0f * d.scale, b.top, b.right - 14.0f * d.scale, b.bottom);
                d.Text(t.line, d.fPill, lr, d.text, 0.95f);
            }
        };
        return v;
    }

    if (s == SurfaceState::Expanded) {
        auto v = std::make_unique<Custom>();
        // Height grows to fit the weather detail line + the mini "Up next" list.
        v->measure = [](const DrawContext& d, float) {
            bool wf = WeatherFresh();
            int n = (int)NextUpcomingEvents(3).size();
            float h = 12.0f + 44.0f + 26.0f;      // pad + clock + date
            if (wf) h += 26.0f + 22.0f;           // weather main + detail line
            h += 30.0f;                           // volume readout (always shown while hovering)
            if (MediaSnapshot().active) h += 34.0f;  // compact now-playing row
            if (n > 0) {
                h += 10.0f + 1.0f + 10.0f + 20.0f;    // divider + "Up next" header
                h += n * 24.0f;                         // actual event rows only
            }
            h += 12.0f;                           // bottom pad
            return h * d.scale;
        };
        v->paint = [](DrawContext& d, D2D1_RECT_F b) {
            const float sc = d.scale;
            float y = b.top + 12.0f * sc;
            d.Text(TimeString(), d.fHuge, D2D1::RectF(b.left, y, b.right, y + 44 * sc), d.text, 0.97f, DWRITE_TEXT_ALIGNMENT_CENTER);
            y += 44 * sc;
            d.Text(DateStringLong(), d.fBody, D2D1::RectF(b.left, y, b.right, y + 22 * sc), d.muted, 0.8f, DWRITE_TEXT_ALIGNMENT_CENTER);
            y += 26 * sc;
            if (WeatherFresh()) {
                WeatherState w = WeatherSnapshot();
                std::wstring line = WeatherShort() + (w.desc.empty() ? L"" : L"   " + w.desc);
                d.Text(line, d.fTitle, D2D1::RectF(b.left, y, b.right, y + 26 * sc), d.text, 0.92f,
                       DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
                y += 26 * sc;
                d.Text(WeatherDetail(), d.fSmall, D2D1::RectF(b.left, y, b.right, y + 18 * sc), d.muted, 0.7f, DWRITE_TEXT_ALIGNMENT_CENTER);
                y += 22 * sc;
            }
            // Volume readout (read-only feedback, no click handling): always visible while
            // hovering so a wheel-scroll here (Surface::OnWheel routes it while state != Open) is
            // actually visible -- the Collapsed pill's draggable glyph+track is a much smaller
            // target than "anywhere on the hovered pill", which is what scrolling responds to.
            {
                const bool muted = g_volMuted.load();
                D2D1_RECT_F gr = D2D1::RectF(b.left + 20.0f * sc, y, b.left + 20.0f * sc + 22.0f * sc, y + 24.0f * sc);
                d.Text(muted ? L"\xE74F" : L"\xE767", d.fGlyph, gr, d.accent, 0.9f, DWRITE_TEXT_ALIGNMENT_CENTER);
                float shown = Clamp(g_volDisplayScalar.load(), 0.0f, 1.0f);
                D2D1_RECT_F lr = D2D1::RectF(gr.right + 6.0f * sc, y + 1.0f * sc, b.right - 20.0f * sc, y + 15.0f * sc);
                d.Text(muted ? L"Muted" : L"Volume", d.fSmall, lr, d.muted, 0.8f);
                wchar_t pc[8]; swprintf_s(pc, L"%d%%", (int)(shown * 100.0f + 0.5f));
                d.Text(pc, d.fSmall, lr, d.text, 0.9f, DWRITE_TEXT_ALIGNMENT_TRAILING);
                D2D1_RECT_F tr = D2D1::RectF(gr.right + 6.0f * sc, y + 17.0f * sc, b.right - 20.0f * sc, y + 21.0f * sc);
                float trad = H(tr) * 0.5f;
                d.dc->FillRoundedRectangle(D2D1::RoundedRect(tr, trad, trad), d.track);
                D2D1_RECT_F fill = tr; fill.right = tr.left + W(tr) * shown;
                d.dc->FillRoundedRectangle(D2D1::RoundedRect(fill, trad, trad), d.accent);
                y += 30.0f * sc;
            }
            // Compact now-playing row: art thumb + title/artist. Full transport controls are one
            // click away in the Open panel's MediaCard; this view is paint-only (no buttons).
            MediaState m = MediaSnapshot();
            if (m.active) {
                D2D1_RECT_F art = D2D1::RectF(b.left + 20 * sc, y, b.left + 20 * sc + 28 * sc, y + 28 * sc);
                DrawCircularArt(d, art, d.fGlyph);
                float tx = art.right + 10.0f * sc;
                std::wstring artistLine = m.sourceName.empty() ? m.artist : m.artist + L"  \x00B7  " + m.sourceName;
                d.Text(m.title.empty() ? L"Nothing playing" : m.title, d.fSmall,
                       D2D1::RectF(tx, y, b.right - 20.0f * sc, y + 16.0f * sc), d.text, 0.92f);
                d.Text(artistLine, d.fSmall, D2D1::RectF(tx, y + 15.0f * sc, b.right - 20.0f * sc, y + 29.0f * sc), d.muted, 0.75f);
                y += 34.0f * sc;
            }
            auto evs = NextUpcomingEvents(3);
            if (!evs.empty()) {
                d.dc->FillRectangle(D2D1::RectF(b.left + 24 * sc, y + 5 * sc, b.right - 24 * sc, y + 6 * sc), d.divider);
                y += 16 * sc;
            }
            SYSTEMTIME nowSt; GetLocalTime(&nowSt);
            // Live countdown to the next event, shown inline in the header ("Up next · in 1 hr
            // 5 min") so the next commitment is glanceable without doing clock math; "now" once
            // it has started, omitted beyond 24h where the row's weekday prefix reads better.
            std::wstring rel;
            if (!evs.empty() && !evs.front().allDay) {
                FILETIME fn, fe; SYSTEMTIME es = evs.front().start;
                SystemTimeToFileTime(&nowSt, &fn); SystemTimeToFileTime(&es, &fe);
                long long diffMin = ((((long long)fe.dwHighDateTime << 32) | fe.dwLowDateTime) -
                                     (((long long)fn.dwHighDateTime << 32) | fn.dwLowDateTime)) / 600000000LL;
                if (diffMin <= 0) rel = L"now";
                else if (diffMin < 24 * 60) {
                    const long long hr = diffMin / 60, mn = diffMin % 60;
                    rel = L"in ";
                    if (hr > 0) { rel += std::to_wstring(hr) + L" hr"; if (mn > 0) rel += L" " + std::to_wstring(mn) + L" min"; }
                    else rel += std::to_wstring(mn) + L" min";
                }
            }
            if (!evs.empty()) {
                D2D1_RECT_F hdr = D2D1::RectF(b.left + 20 * sc, y, b.right - 20 * sc, y + 16 * sc);
                d.Text(L"Up next", d.fSmall, hdr, d.muted, 0.55f);
                if (!rel.empty()) {
                    const float upW = d.MeasureWidth(d.fSmall, L"Up next");
                    const std::wstring sep = L"  \x00B7  ";
                    const float sepW = d.MeasureWidth(d.fSmall, sep);
                    d.Text(sep, d.fSmall, D2D1::RectF(hdr.left + upW, y, hdr.right, y + 16 * sc), d.muted, 0.55f);
                    d.Text(rel, d.fSmall, D2D1::RectF(hdr.left + upW + sepW, y, hdr.right, y + 16 * sc), d.accent, 0.9f);
                }
                y += 20 * sc;
                for (auto& e : evs) {
                    bool sameDay = (e.start.wYear == nowSt.wYear && e.start.wMonth == nowSt.wMonth && e.start.wDay == nowSt.wDay);
                    wchar_t tm[24] = {};
                    if (e.allDay) wcscpy_s(tm, L"All day");
                    else { SYSTEMTIME es = e.start; GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, TIME_NOSECONDS, &es, nullptr, tm, ARRAYSIZE(tm)); }
                    std::wstring when = tm;
                    if (!sameDay) {  // prefix the weekday so tomorrow's times aren't mistaken for past ones
                        wchar_t dd[16] = {}; GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &e.start, L"ddd", dd, ARRAYSIZE(dd), nullptr);
                        when = std::wstring(dd) + L" " + tm;
                    }
                    d.dc->FillRoundedRectangle(D2D1::RoundedRect(D2D1::RectF(b.left + 20 * sc, y + 4 * sc, b.left + 22.5f * sc, y + 20 * sc), 1.2f * sc, 1.2f * sc), d.accent);
                    d.Text(when, d.fSmall, D2D1::RectF(b.left + 30 * sc, y, b.left + 140 * sc, y + 22 * sc), d.muted, 0.8f);
                    d.Text(e.subject, d.fBody, D2D1::RectF(b.left + 144 * sc, y, b.right - 16 * sc, y + 22 * sc), d.text, 0.92f);
                    y += 24 * sc;
                }
            }
        };
        return v;
    }

    // Open: full control center. (The rounded panel backdrop is drawn once by the render loop at
    // the animating size; the content tree is transparent so the morph reveal/clip stays clean.)
    auto panel = std::make_unique<StackPanel>();
    panel->padX = 16.0f; panel->padY = 14.0f; panel->gap = 10.0f;

    // Header: big clock + long date (custom).
    auto header = std::make_unique<Custom>();
    header->fixedHeight = 64.0f * dc.scale;
    header->paint = [](DrawContext& d, D2D1_RECT_F b) {
        D2D1_RECT_F clock = D2D1::RectF(b.left, b.top, b.right, b.top + 40.0f * d.scale);
        d.Text(TimeString(), d.fHuge, clock, d.text, 0.97f, DWRITE_TEXT_ALIGNMENT_LEADING);
        D2D1_RECT_F date = D2D1::RectF(b.left, b.top + 40.0f * d.scale, b.right, b.bottom);
        d.Text(DateStringLong(), d.fBody, date, d.muted, 0.8f, DWRITE_TEXT_ALIGNMENT_LEADING);
        if (WeatherFresh()) {
            D2D1_RECT_F wr = D2D1::RectF(b.right - 130.0f * d.scale, b.top + 4.0f * d.scale, b.right, b.top + 30.0f * d.scale);
            d.Text(WeatherShort(), d.fTitle, wr, d.text, 0.92f, DWRITE_TEXT_ALIGNMENT_TRAILING,
                   DWRITE_PARAGRAPH_ALIGNMENT_CENTER, D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);
            WeatherState w = WeatherSnapshot();
            if (!w.city.empty()) {
                D2D1_RECT_F cr = D2D1::RectF(b.right - 150.0f * d.scale, b.top + 30.0f * d.scale, b.right, b.top + 48.0f * d.scale);
                d.Text(w.city, d.fSmall, cr, d.muted, 0.6f, DWRITE_TEXT_ALIGNMENT_TRAILING);
            }
        }
    };
    panel->Add(std::move(header));

    // Volume slider (real, interactive).
    auto vol = std::make_unique<Slider>();
    vol->label = L"Volume";
    vol->glyph = L"\xE767";  // Fluent: volume
    vol->glyphFmt = nullptr;  // set below via dc
    vol->get = []() { return g_volScalar.load(); };
    vol->set = [](float v) { SetSystemVolume(v); };
    panel->Add(std::move(vol));

    // Brightness slider (only if the island's monitor honors DDC/CI).
    if (g_brightPercent.load() >= 0) {
        auto bri = std::make_unique<Slider>();
        bri->label = L"Brightness";
        bri->glyph = L"\xE706";  // Fluent: brightness
        bri->get = []() { int p = g_brightPercent.load(); return p < 0 ? 0.0f : p / 100.0f; };
        bri->set = [](float v) {
            int pct = (int)(v * 100.0f + 0.5f);
            g_brightPercent = pct;
            g_brightSetRequest = pct;
            if (g_brightEvent) SetEvent(g_brightEvent);
            g_layoutDirty = true;
        };
        panel->Add(std::move(bri));
    }

    // System telemetry replaces the old notification list in the visible Control Center.
    // Notifications remain in the backend, but are intentionally not rendered here so this
    // space is useful for the machine's live CPU/RAM/network status.
    auto scroller = std::make_unique<ScrollContainer>();
    scroller->viewportMax = 430.0f;
    scroller->gap = 8.0f;

    if (MediaSnapshot().active) scroller->Add(std::make_unique<MediaCard>());
    scroller->Add(std::make_unique<CalendarView>());
    scroller->Add(std::make_unique<AgendaList>());
    scroller->Add(std::make_unique<TaskList>());
    scroller->Add(std::make_unique<StudyPanel>());

    auto sysLabel = std::make_unique<Label>();
    sysLabel->text = L"System";
    sysLabel->fmtMember = &DrawContext::fTitle;
    sysLabel->height = 24.0f;
    scroller->Add(std::move(sysLabel));

    auto sys = std::make_unique<Custom>();
    sys->fixedHeight = 72.0f * dc.scale;
    sys->paint = [](DrawContext& d, D2D1_RECT_F b) {
        const float s = d.scale;
        const float cpu = g_hwCpu.load();
        const float ram = g_hwRam.load();
        const std::wstring down = FormatRate(g_hwNetDown.load());
        const std::wstring up = FormatRate(g_hwNetUp.load());

        // Three compact metric rows: CPU, RAM, and network throughput.
        auto metric = [&](float y, const wchar_t* label, const std::wstring& value, float pct) {
            d.Text(label, d.fSmall, D2D1::RectF(b.left + 4*s, y, b.left + 64*s, y + 20*s), d.muted, 0.8f);
            d.Text(value, d.fBody, D2D1::RectF(b.left + 66*s, y, b.right - 4*s, y + 20*s), d.text, 0.9f, DWRITE_TEXT_ALIGNMENT_TRAILING);
            D2D1_RECT_F track = D2D1::RectF(b.left + 4*s, y + 19*s, b.right - 4*s, y + 23*s);
            d.dc->FillRoundedRectangle(D2D1::RoundedRect(track, 2*s, 2*s), d.track);
            D2D1_RECT_F fill = track;
            fill.right = fill.left + (fill.right - fill.left) * Clamp(pct / 100.0f, 0.0f, 1.0f);
            if (fill.right > fill.left + 0.5f) d.dc->FillRoundedRectangle(D2D1::RoundedRect(fill, 2*s, 2*s), d.accent);
        };
        metric(b.top, L"CPU", std::to_wstring((int)std::round(cpu)) + L"%", cpu);
        metric(b.top + 25*s, L"RAM", std::to_wstring((int)std::round(ram)) + L"%", ram);
        d.Text(L"NET", d.fSmall, D2D1::RectF(b.left + 4*s, b.top + 50*s, b.left + 64*s, b.top + 70*s), d.muted, 0.8f);
        d.Text(L"↓ " + down + L"   ↑ " + up, d.fBody,
               D2D1::RectF(b.left + 66*s, b.top + 50*s, b.right - 4*s, b.top + 70*s), d.text, 0.9f,
               DWRITE_TEXT_ALIGNMENT_TRAILING);
    };
    scroller->Add(std::move(sys));
    panel->Add(std::move(scroller));

    return panel;
}

// ---------------------------------------------------------------------------
// Globals tying WndProc to the Surface (all on render thread)
// ---------------------------------------------------------------------------
Surface* g_surface = nullptr;

// Both low-level hooks below live on the dedicated InputHookThreadProc, which does nothing
// but pump messages — so Windows can service them instantly. A low-level hook installed on
// the render thread (which spends most of its time painting or sleeping in WaitForSingleObject)
// would stall ALL system mouse input while the panel is up: the "cursor dragging through slime"
// bug. These procs must therefore stay trivial and return fast.
LRESULT CALLBACK LlMouseProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION && g_dismissActive.load() &&
        (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN) &&
        NowSeconds() - g_dismissArmedAt.load() >= kDismissGraceSeconds) {
        auto* p = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        RECT r;
        if (g_hwnd && GetWindowRect(g_hwnd, &r) && !PtInRect(&r, p->pt))
            PostMessageW(g_hwnd, WM_APP_DISMISS, 0, 0);
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}
// Just arm/disarm the (always-installed) mouse hook; no SetWindowsHookEx on the render thread.
void InstallDismissHook() { g_dismissArmedAt = NowSeconds(); g_dismissActive = true; }
void RemoveDismissHook() { g_dismissActive = false; }

// Volume-key capture: swallow VK_VOLUME_* so the composition-rendered Win11 OSD never shows.
LRESULT CALLBACK LlKeyProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION && g_captureVolKeys.load() && g_hwnd) {
        auto* k = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        if (k->vkCode == VK_VOLUME_UP || k->vkCode == VK_VOLUME_DOWN || k->vkCode == VK_VOLUME_MUTE) {
            if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
                PostMessageW(g_hwnd, WM_APP_VOLKEY, k->vkCode, 0);
            return 1;  // swallow down AND up so Windows never shows its native flyout
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}
DWORD WINAPI InputHookThreadProc(void*) {
    g_inputHookTid = GetCurrentThreadId();
    HINSTANCE mod = GetModuleHandleW(nullptr);
    g_keyHook = SetWindowsHookExW(WH_KEYBOARD_LL, LlKeyProc, mod, 0);
    g_dismissHook = SetWindowsHookExW(WH_MOUSE_LL, LlMouseProc, mod, 0);
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    if (g_keyHook) { UnhookWindowsHookEx(g_keyHook); g_keyHook = nullptr; }
    if (g_dismissHook) { UnhookWindowsHookEx(g_dismissHook); g_dismissHook = nullptr; }
    return 0;
}

D2D1_POINT_2F ClientPos(LPARAM lParam) {
    return D2D1::Point2F(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
}

enum : UINT { IDM_ISLAND_PIN=41001, IDM_ISLAND_UNPIN=41002 };
void ShowIslandContextMenu(HWND hwnd, POINT pt) {
    HMENU menu=CreatePopupMenu(); if(!menu)return;
    AppendMenuW(menu,MF_STRING,g_pinExpanded?IDM_ISLAND_UNPIN:IDM_ISLAND_PIN,g_pinExpanded?L"Unpin expanded Island":L"Pin expanded Island");
    // Keep the context menu native/light; do not force a dark owner background.
    SetForegroundWindow(hwnd);
    UINT cmd=TrackPopupMenuEx(menu,TPM_RETURNCMD|TPM_RIGHTBUTTON,pt.x,pt.y,hwnd,nullptr);
    DestroyMenu(menu);
    if(cmd==IDM_ISLAND_PIN){g_pinExpanded=true;if(g_surface&&g_surface->state!=SurfaceState::Open&&g_surface->state!=SurfaceState::Expanded)g_surface->PinExpanded();g_layoutDirty=true;}
    else if(cmd==IDM_ISLAND_UNPIN){g_pinExpanded=false;if(g_surface)g_surface->UnpinExpanded();g_layoutDirty=true;}
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            AddClipboardFormatListener(hwnd);
            return 0;
        case WM_DISPLAYCHANGE:
            g_layoutDirty = true;
            g_brightRebind = true;
            if (g_brightEvent) SetEvent(g_brightEvent);
            return 0;
        case WM_CLIPBOARDUPDATE: {
            std::wstring preview;
            if (OpenClipboard(hwnd)) {
                if (HANDLE h = GetClipboardData(CF_UNICODETEXT))
                    if (wchar_t* p = (wchar_t*)GlobalLock(h)) { preview = p; GlobalUnlock(h); }
                CloseClipboard();
            }
            for (auto& c : preview) if (c == L'\n' || c == L'\r' || c == L'\t') c = L' ';
            if (preview.size() > 40) { preview.resize(40); preview += L"\x2026"; }
            ShowTransient(L"\xE8C8", preview.empty() ? L"Copied" : (L"Copied  " + preview));
            return 0;
        }
        case WM_DEVICECHANGE:
            if (wParam == 0x8000 || wParam == 0x8004)  // DBT_DEVICEARRIVAL / DBT_DEVICEREMOVECOMPLETE
                ShowTransient(L"\xE88E", wParam == 0x8000 ? L"Device connected" : L"Device disconnected");
            return 0;
        case WM_APP_DISMISS:
            if (g_surface) { g_surface->Dismiss(); g_layoutDirty = true; }
            return 0;
        case WM_APP_VOLKEY: {  // a captured hardware volume key -> drive volume + island popup
            float cur = 0.0f; bool mut = false;
            g_volume.Get(cur, mut);
            if (wParam == VK_VOLUME_MUTE) {
                ToggleSystemMute();
                ShowVolumeTransient((int)(cur * 100.0f + 0.5f), !mut);
            } else {
                const float step = 0.02f;  // matches the native 2%-per-press step
                float nv = Clamp(cur + (wParam == VK_VOLUME_UP ? step : -step), 0.0f, 1.0f);
                SetSystemVolume(nv);
                ShowVolumeTransient((int)(nv * 100.0f + 0.5f), false);
            }
            return 0;
        }
        case WM_MOUSEMOVE:
            // Do NOT treat bare mouse movement as a repaint trigger: the panel has no
            // hover-driven visuals, so moving over it must be free. A live slider drag keeps
            // rendering via IsCapturing(); scroll/press set their own dirty flags. This is the
            // fix for "open the panel, move the mouse, it gets laggy" (full 60fps repaints).
            if (g_surface) g_surface->OnMove({PointerPhase::Move, ClientPos(lParam)});
            return 0;
        case WM_LBUTTONDOWN:
            g_lastInputSec = NowSeconds();
            if (g_surface) {
                bool handled = g_surface->OnDown({PointerPhase::Down, ClientPos(lParam)});
                if (!handled && g_surface->state != SurfaceState::Open) g_surface->Open();
            }
            g_layoutDirty = true;
            return 0;
        case WM_LBUTTONUP:
            g_lastInputSec = NowSeconds();
            if (g_surface) g_surface->OnUp({PointerPhase::Up, ClientPos(lParam)});
            g_layoutDirty = true;
            return 0;
        case WM_RBUTTONUP: {
            POINT pt={GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam)}; ClientToScreen(hwnd,&pt);
            if(PtInRect(&g_pillRect,pt)){ ShowIslandContextMenu(hwnd,pt); return 0; }
            return 0;
        }
        case WM_MBUTTONDOWN:
            // Middle-click is intentionally ignored; auto-hide/reveal is mouse-position based.
            g_lastInputSec = NowSeconds();
            return 0;
        case WM_MOUSEWHEEL:
            g_lastInputSec = NowSeconds();
            if (g_surface) {
                POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                ScreenToClient(hwnd, &pt);
                float notches = (float)GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA * 36.0f;
                g_surface->OnWheel({PointerPhase::Wheel, D2D1::Point2F((float)pt.x, (float)pt.y), notches});
            }
            return 0;
        case WM_DESTROY: return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ===========================================================================
// Render thread
// ===========================================================================
void SetClickThrough(HWND hwnd, bool clickThrough);
void BindGlyphFormats(Surface& surface, IDWriteTextFormat* glyph);

// Keep the island visible while the Windows desktop itself is the foreground surface.
// Once an application becomes foreground, the normal 3-second auto-hide behavior resumes.
bool IsDesktopForeground() {
    HWND fg = GetForegroundWindow();
    if (!fg) return false;
    if (fg == GetShellWindow()) return true;
    wchar_t cls[64] = {};
    if (GetClassNameW(fg, cls, ARRAYSIZE(cls)) > 0) {
        if (_wcsicmp(cls, L"Progman") == 0 || _wcsicmp(cls, L"WorkerW") == 0) return true;
    }
    return false;
}

// The hidden island can be revealed only by moving the cursor into the
// extreme top-left corner of the monitor where the island lives.
bool IsIslandRevealHotspot(HWND hwnd, POINT pt) {
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!monitor || !GetMonitorInfoW(monitor, &mi)) {
        return pt.x >= 0 && pt.x < 16 && pt.y >= 0 && pt.y < 16;
    }

    constexpr LONG kHotspotSize = 16;
    const RECT& r = mi.rcMonitor;
    return pt.x >= r.left && pt.x < r.left + kHotspotSize &&
           pt.y >= r.top  && pt.y < r.top  + kHotspotSize;
}

DWORD WINAPI RenderThreadProc(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kWindowClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
        kWindowClass, L"Island Command Center", WS_POPUP, 0, 0, 360, 96,
        nullptr, nullptr, wc.hInstance, nullptr);
    if (!hwnd) { Wh_Log(L"Window creation failed."); if (SUCCEEDED(hrCo)) CoUninitialize(); return 0; }
    g_hwnd = hwnd;
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);

    Renderer renderer;
    if (!renderer.Initialize(hwnd)) {
        Wh_Log(L"Renderer init failed.");
        DestroyWindow(hwnd); g_hwnd = nullptr;
        if (SUCCEEDED(hrCo)) CoUninitialize();
        return 0;
    }

    Surface surface;
    g_surface = &surface;
    {
        DrawContext seed = renderer.MakeContext(1.0f, NowSeconds());
        surface.BuildFor(SurfaceState::Collapsed, seed);
    }

    BackdropBlurHost blurHost;  // real blur-behind companion; lazy-inits on the first blurred style

    SpringValue wSpring, hSpring;
    wSpring.Reset(180.0f);
    hSpring.Reset(40.0f);
    auto prevFrame = std::chrono::steady_clock::now();
    double nextVolPoll = 0.0;
    bool hiRes = false;                                   // 1ms timer resolution raised only while active
    auto nextDeadline = std::chrono::steady_clock::now();  // absolute 60fps pacing deadline
    float morphStartH = 40.0f;                             // panel height at the current morph's start

    while (WaitForSingleObject(g_stopEvent, 0) == WAIT_TIMEOUT) {
        MSG msg = {};
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_APP_NEW_EVENT) continue;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        const double now = NowSeconds();
        StudyTick(now);
        const Settings s = SettingsSnapshot();
        const UINT dpi = s.autoDpiScale ? GetDpiForWindow(hwnd) : 96;
        const float scale = (dpi / 96.0f) * s.sizeScale;

        // Automatic hover-hide:
        //   * visible while the cursor is on the island OR in the top-left reveal corner
        //   * after leaving both areas, wait 3 seconds and hide
        //   * once hidden, the top-left reveal corner or an island-connected event can show it again
        // The existing hideT animation below makes the transition smooth.
        POINT curForHide;
        GetCursorPos(&curForHide);

        const bool cursorOnIsland = PtInRect(&g_pillRect, curForHide) != FALSE;
        const bool cursorInRevealHotspot = IsIslandRevealHotspot(hwnd, curForHide);

        static bool autoHidden = false;
        static uint64_t seenRevealSerial = 0;
        const uint64_t revealSerial = g_islandRevealSerial.load(std::memory_order_acquire);
        const bool islandEvent = revealSerial != seenRevealSerial;
        if (islandEvent) seenRevealSerial = revealSerial;

        if (StudyRunning()) {
            // An active study timer is always visible in the collapsed pill.
            autoHidden = false; g_hideUntil = 0.0;
        } else if (g_pinExpanded) {
            autoHidden = false; g_hideUntil = 0.0;
            if(g_surface&&g_surface->state!=SurfaceState::Open&&g_surface->state!=SurfaceState::Expanded)g_surface->PinExpanded();
        } else if (IsDesktopForeground()) {
            // On the Windows desktop, keep the island visible as requested.
            // Auto-hide resumes automatically when an application becomes foreground.
            autoHidden = false;
            g_hideUntil = 0.0;
        } else if (islandEvent) {
            // Any island-connected event reveals the pill and starts a fresh 3-second window.
            autoHidden = false;
            g_hideUntil = now + kHideSeconds;
            g_layoutDirty = true;
        } else if (autoHidden) {
            if (cursorInRevealHotspot) {
                autoHidden = false;
                g_hideUntil = 0.0;
                g_layoutDirty = true;
            }
        } else {
            if (cursorOnIsland || cursorInRevealHotspot) {
                // The cursor is actively using/hovering the island.
                g_hideUntil = 0.0;
            } else {
                // Start (or continue) the 3-second hide countdown.
                if (g_hideUntil <= 0.0) {
                    g_hideUntil = now + kHideSeconds;
                }

                if (now >= g_hideUntil) {
                    autoHidden = true;
                    g_hideUntil = 0.0;

                    // Collapse any expanded/open surface before hiding so the next
                    // reveal starts cleanly from the normal pill.
                    if (g_surface) g_surface->Dismiss();
                    std::lock_guard l(g_transMutex);
                    g_trans = {};
                    g_layoutDirty = true;
                }
            }
        }

        const bool wantHidden = autoHidden;

        static float hideT = 0.0f;        // 0 = fully visible, 1 = fully hidden
        static double hideTickAt = -1.0;
        {
            const float target = wantHidden ? 1.0f : 0.0f;
            const float dt = hideTickAt < 0.0 ? (1.0f / 60.0f)
                                              : static_cast<float>(std::min(now - hideTickAt, 0.1));
            hideTickAt = now;
            hideT += (target - hideT) * (1.0f - std::exp(-dt / 0.12f));  // ~300-400ms fade
            if (std::fabs(hideT - target) < 0.004f) hideT = target;
        }
        const bool hideAnimating = wantHidden ? (hideT < 0.996f) : (hideT > 0.004f);

        if (now >= nextVolPoll) { PollVolume(); nextVolPoll = now + 0.15; }

        // Caps / Num lock toggles -> transient pill.
        {
            static int lastCaps = -1, lastNum = -1;
            int caps = (GetKeyState(VK_CAPITAL) & 1) ? 1 : 0;
            int num = (GetKeyState(VK_NUMLOCK) & 1) ? 1 : 0;
            if (lastCaps >= 0 && caps != lastCaps) { RevealIslandForEvent(); ShowTransient(L"\xE765", caps ? L"Caps Lock on" : L"Caps Lock off"); }
            if (lastNum >= 0 && num != lastNum) { RevealIslandForEvent(); ShowTransient(L"\xE765", num ? L"Num Lock on" : L"Num Lock off"); }
            lastCaps = caps; lastNum = num;
        }

        DrawContext dc = renderer.MakeContext(scale, now);

        // The startup BuildFor(Collapsed, ...) used a placeholder scale (no real DPI yet); rebuild
        // at the real scale the first time they differ (and again on any later DPI/SizeScale change)
        // so sizes baked into the tree (e.g. the pill's fixedHeight) are never wrong.
        if (surface.RebuildIfScaleChanged(dc)) g_layoutDirty = true;

        // Hover state (geometric). Use the VISIBLE panel rect, not the window rect: since the window
        // is held at the (larger) stable size during a morph, keying hover off the window would make
        // the empty area where the expanded panel *would* be trigger the expand. g_pillRect is the
        // actual drawn panel from the last frame (screen coords).
        POINT cur = curForHide;
        {   // publish the cursor in client (widget) coords for hover-state painting
            POINT cc = cur; ScreenToClient(hwnd, &cc);
            dc.cursor = D2D1::Point2F((float)cc.x, (float)cc.y);
        }
        bool cursorInside = PtInRect(&g_pillRect, cur) != FALSE;
        bool overVolumeHotspot = PtInRect(&g_volumeHotspotRect, cur) != FALSE;
        // Freeze ambient state while hidden -- otherwise hovering the (invisible) territory would
        // silently expand/collapse it, and it would pop into the wrong state once it fades back in.
        if (!wantHidden) surface.UpdateAmbientState(cursorInside, now, overVolumeHotspot);

        // Flush any pending slider drag with correct scale.
        surface.FlushPendingDrag(dc);

        // Apply queued state transitions (rebuild root) / in-place data refresh.
        bool stateChanged = surface.ApplyPendingState(dc);
        if (stateChanged) g_layoutDirty = true;
        bool dataChanged = surface.RebuildIfDataChanged(dc);  // e.g. notif list changed while Open
        if (dataChanged) g_layoutDirty = true;

        // Target-LOCK + content-fade clock. Re-measure the content size only on a real change
        // (state transition / data refresh) or when SETTLED, then FREEZE it for the whole morph —
        // per-frame re-measure (dynamic Expanded height, Open's MaxOpenHeight clamp) is what
        // re-aimed the spring mid-flight and made the size non-monotonic/choppy. On a state
        // transition, record the start height so content fades in as the panel grows (otherwise the
        // new state's big content would flash clipped inside the still-small pill — the "hitch").
        bool settledNow = wSpring.AtRest() && hSpring.AtRest();
        if (stateChanged || dataChanged || settledNow) {
            D2D1_SIZE_F m = surface.MeasureContent(dc);
            if (stateChanged)      morphStartH = hSpring.value;  // fade content in over the grow
            else if (dataChanged)  morphStartH = m.height;       // resize only, no content fade
            surface.SetLocked(m);
        }
        wSpring.target = surface.LockedW();
        hSpring.target = surface.LockedH();

        auto frame = std::chrono::steady_clock::now();
        float dt = Clamp(std::chrono::duration<float>(frame - prevFrame).count(), 0.001f, 0.033f);
        prevFrame = frame;
        // A morph begins right after an idle sleep, so this first dt spans the whole idle gap
        // (~100ms). Feeding that to a stiff spring makes it lurch most of the way in ONE step — an
        // instant, hitchy morph. Begin every transition with a normal 60fps step instead.
        if (stateChanged) dt = 1.0f / 60.0f;
        // Slightly over-damped (zeta ~1.03): a smooth, strictly monotonic glide, no bounce.
        wSpring.Step(dt, 250.0f, 32.0f);
        hSpring.Step(dt, 250.0f, 32.0f);

        // Content-fade progress: 0 at the panel's morph-start size -> 1 at its locked target. Kept
        // low early so the incoming content isn't drawn (clipped) while the panel is still small.
        float span = surface.LockedH() - morphStartH;
        float t01 = (std::fabs(span) < 1.0f) ? 1.0f
                    : Clamp((hSpring.value - morphStartH) / span, 0.0f, 1.0f);
        float tb = Clamp((t01 - 0.28f) / 0.72f, 0.0f, 1.0f);
        float contentAlpha = tb * tb * (3.0f - 2.0f * tb);  // smoothstep, delayed

        // Ease the displayed volume toward the real level so the popup bar animates (not static).
        static float volDisplay = -1.0f;
        float volTarget = g_volScalar.load();
        if (volDisplay < 0.0f) volDisplay = volTarget;
        volDisplay += (volTarget - volDisplay) * std::min(1.0f, dt * 14.0f);
        if (std::fabs(volTarget - volDisplay) < 0.002f) volDisplay = volTarget;
        g_volDisplayScalar = volDisplay;
        bool volAnimating = (volDisplay != volTarget);

        // Per-state transparency: ease toward the denser "focus" tint while hovered/open, back
        // to "rest" otherwise, so the pill stays unobtrusive at rest but reads clearly once the
        // user is actually looking at/interacting with it.
        static float stateMix = 0.0f;
        float stateMixTarget =
            (surface.state == SurfaceState::Expanded || surface.state == SurfaceState::Open) ? 1.0f : 0.0f;
        stateMix += (stateMixTarget - stateMix) * (1.0f - std::exp(-dt / 0.15f));
        if (std::fabs(stateMix - stateMixTarget) < 0.004f) stateMix = stateMixTarget;
        bool stateMixAnimating = (stateMix != stateMixTarget);

        bool widgetAnimating = surface.Tick(dt);
        bool sizeAnimating = !wSpring.AtRest() || !hSpring.AtRest();

        int cw = std::max(1, (int)std::ceil(wSpring.value));
        int ch = std::max(1, (int)std::ceil(hSpring.value));

        MediaState mediaNow = MediaSnapshot();
        // The now-playing indicator (art + waveform) shows in every state now (Collapsed/Expanded/
        // Open), not just the Open panel's MediaCard -- so the waveform must animate there too.
        bool mediaLive = mediaNow.active && mediaNow.playing;
        g_waveWanted = mediaLive;  // let the audio-meter thread idle when the waveform is hidden
        bool privacyLive = g_micActive.load() || g_camActive.load();
        bool inputActive = (now - g_lastInputSec.load()) < 0.5;  // hover/drag responsiveness window

        // Resolve this frame's backdrop recipe (per-state opacity, then accent/theme colors) and
        // step the theme engine BEFORE the needsRender decision below -- both must run every tick
        // (not just on an actual render) so an Auto-mode Windows theme flip is noticed promptly
        // and ThemeEngine::Animating() reflects THIS tick, not a stale one from the last render.
        // Called unconditionally (even with the backdrop disabled) so the plain solid "None" panel
        // still follows the Theme setting.
        BackdropPreset resolvedBackdrop = s.backdrop;
        ApplyStateOpacity(resolvedBackdrop, stateMix);
        ResolveAccentColors(resolvedBackdrop);
        float themeT = g_themeEngine.Step(s, resolvedBackdrop, now);
        ResolveThemeColors(resolvedBackdrop, themeT);

        // A frame costs a full UpdateLayeredWindow re-composite of the (possibly large) panel
        // through DWM. Do it only when something actually changed — resting the cursor over the
        // Open panel must NOT drive continuous 60fps compositing (that spikes the whole desktop).
        static bool lastCursorInside = false;
        static int  lastClockKey = -1;
        static uint64_t lastHwGeneration = (uint64_t)-1;
        static bool firstFrame = true;
        const int clockKey = ClockMinuteKey();
        // Adaptive-theme stimulus: repaint when the sampled backdrop luminance has moved
        // meaningfully since the last render that consumed it (an idle island must still notice
        // the background changing beneath it). Only relevant in Adaptive mode -- in the other
        // modes the sampler is idle and g_behindLum sits at -1, which must not itself count as
        // "moved" every tick. lastBehindLum only advances when consumed so small drifts accumulate.
        static float lastBehindLum = -2.0f;
        const float behindLum = g_behindLum.load();
        bool themeStim = s.themeMode == ThemeMode::Adaptive &&
                         std::fabs(behindLum - lastBehindLum) > 0.02f;
        if (themeStim) lastBehindLum = behindLum;
        // Study timers update their visible text once per second via g_layoutDirty;
        // they do NOT force the compositor into the 60 FPS animation loop.
        bool active = widgetAnimating || sizeAnimating || surface.IsCapturing() ||
                      mediaLive || privacyLive || inputActive || volAnimating ||
                      stateMixAnimating || g_themeEngine.Animating() || hideAnimating;
        const uint64_t hwGeneration = g_hwGeneration.load(std::memory_order_acquire);
        const bool hwStim = !wantHidden && hwGeneration != lastHwGeneration;
        bool needsRender = firstFrame || active || themeStim || g_layoutDirty.load() ||
                           hwStim || clockKey != lastClockKey || cursorInside != lastCursorInside;
        lastCursorInside = cursorInside;
        lastClockKey = clockKey;
        if (hwStim) lastHwGeneration = hwGeneration;

        if (needsRender) {
            // Real blur-behind: bring the companion window's state in line BEFORE the frame is
            // drawn (it only consumes blur/saturation from resolvedBackdrop, resolved above) --
            // BeginFrame picks the panel tint variant from the result. Stays enabled through a
            // temp-hide (its opacity is faded below, in lockstep with the main window) rather than
            // being cut outright, which previously made the blur disappear instantly while the
            // panel/text kept fading -- a visible seam between "background" and "foreground".
            g_backdropBlurLive = blurHost.Prepare(s.backdropEnabled, resolvedBackdrop, hwnd);
            D2D1_RECT_F inner;
            int tgtW = (int)std::ceil(wSpring.target), tgtH = (int)std::ceil(hSpring.target);
            bool settled = wSpring.AtRest() && hSpring.AtRest();
            if (renderer.BeginFrame(s, resolvedBackdrop, cw, ch, tgtW, tgtH, settled, scale, themeT, inner)) {
                DrawContext pdc = renderer.MakeContext(scale, now);  // brushes valid post-BindDC
                pdc.cursor = dc.cursor;  // carry the frame's cursor for hover-state painting
                pdc.artBmp = renderer.ArtBitmap(mediaNow.art.gen, mediaNow.art.w, mediaNow.art.h, mediaNow.art.bgra);
                BindGlyphFormats(surface, renderer.GlyphFormat());
                // The gliding panel: animating size, TOP-anchored and horizontally centered inside
                // the fixed window (`inner`). Only this rounded rect (and the content reveal) move;
                // the window and the content's layout origin stay put -> no clock jitter.
                float aw = std::min((float)cw, W(inner)), ah = std::min((float)ch, H(inner));
                float panelLeft = std::floor((inner.left + inner.right - aw) * 0.5f + 0.5f);
                D2D1_RECT_F panelR = D2D1::RectF(panelLeft, inner.top, panelLeft + aw, inner.top + ah);
                // Publish the visible panel's screen rect for next frame's hover test (and, under
                // the lock, for the luminance sampler thread) -- and, same pattern, the mini
                // volume element's screen-space hotspot (empty when not Collapsed).
                { RECT wr; GetWindowRect(hwnd, &wr);
                  RECT pr = { wr.left + (LONG)panelR.left, wr.top + (LONG)panelR.top,
                              wr.left + (LONG)panelR.right, wr.top + (LONG)panelR.bottom };
                  AcquireSRWLockExclusive(&g_pillRectLock);
                  g_pillRect = pr;
                  ReleaseSRWLockExclusive(&g_pillRectLock);
                  D2D1_RECT_F hs = surface.VolumeHotspot();
                  if (W(hs) > 0.0f && H(hs) > 0.0f) {
                      g_volumeHotspotRect = { wr.left + (LONG)hs.left, wr.top + (LONG)hs.top,
                                              wr.left + (LONG)hs.right, wr.top + (LONG)hs.bottom };
                  } else {
                      g_volumeHotspotRect = RECT{};
                  } }
                float panelRadius = std::min(H(panelR) * 0.5f, 22.0f * scale);
                // Keep the real-blur companion clipped to this exact panel rect (it animates too),
                // and fade its opacity in lockstep with the main window's temp-hide alpha.
                if (g_backdropBlurLive) {
                    blurHost.SyncGeometry(hwnd, panelR, panelRadius);
                    blurHost.SetOpacity(1.0f - hideT);
                }
                {
                    D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(panelR, panelRadius, panelRadius);
                    pdc.dc->FillRoundedRectangle(rr, pdc.panel);
                    pdc.dc->DrawRoundedRectangle(rr, pdc.border, 1.0f);
                }
                surface.Layout(pdc, inner);
                // Content is clipped to the animating panel and (during a morph) faded in through an
                // opacity LAYER, so the incoming content never shows at full alpha while clipped in
                // the small pill; at rest a plain clip is used (cheaper).
                ID2D1Layer* clyr = contentAlpha < 0.995f ? renderer.EnsureContentLayer() : nullptr;
                if (clyr) {
                    pdc.dc->PushLayer(D2D1::LayerParameters(panelR, nullptr,
                        D2D1_ANTIALIAS_MODE_PER_PRIMITIVE, D2D1::Matrix3x2F::Identity(),
                        contentAlpha), clyr);
                    surface.Paint(pdc);
                    pdc.dc->PopLayer();
                } else {
                    pdc.dc->PushAxisAlignedClip(panelR, D2D1_ANTIALIAS_MODE_ALIASED);
                    surface.Paint(pdc);
                    pdc.dc->PopAxisAlignedClip();
                }
                // Privacy dots (camera = green, mic = orange), pulsing, centered at the top of the panel.
                {
                    bool cam = g_camActive.load(), mic = g_micActive.load();
                    int count = (cam ? 1 : 0) + (mic ? 1 : 0);
                    if (count > 0) {
                        float pr = 2.5f * scale, gap = 11.0f * scale;
                        float cxD = (panelR.left + panelR.right) * 0.5f;
                        float py = panelR.top + 5.5f * scale;
                        float px = cxD - (count - 1) * gap * 0.5f;
                        float pulse = 0.55f + 0.45f * sinf((float)now * 4.0f);
                        if (cam) {
                            ComPtr<ID2D1SolidColorBrush> bb;
                            pdc.dc->CreateSolidColorBrush(D2D1::ColorF(0.2f, 0.85f, 0.35f, pulse), &bb);
                            if (bb) pdc.dc->FillEllipse(D2D1::Ellipse(D2D1::Point2F(px, py), pr, pr), bb.Get());
                            px += gap;
                        }
                        if (mic) {
                            ComPtr<ID2D1SolidColorBrush> bb;
                            pdc.dc->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.6f, 0.1f, pulse), &bb);
                            if (bb) pdc.dc->FillEllipse(D2D1::Ellipse(D2D1::Point2F(px, py), pr, pr), bb.Get());
                        }
                    }
                }
                renderer.EndFrame(s, 1.0f - hideT);
                firstFrame = false;
            }
        }

        // Click-through policy. Forced non-interactive the instant a temp-hide gesture starts
        // (not gated on the fade finishing) so whatever's underneath is clickable right away.
        bool wantInteractive = !wantHidden &&
                               ((surface.state == SurfaceState::Open) || cursorInside || surface.IsCapturing() || g_pinExpanded);
        SetClickThrough(hwnd, !wantInteractive);

        // Frame pacing. While active, hold true 60fps with a 1ms timer resolution and an ABSOLUTE
        // deadline (paint time is absorbed into the budget, not added on top) — this removes the
        // ~15.6ms quantization / duplicate frames that made the morph steppy. Idle -> drop the
        // resolution (power) and poll slowly for hover/clock.
        if (active) {
            if (!hiRes) { timeBeginPeriod(1); hiRes = true; }
            nextDeadline += std::chrono::microseconds(16667);
            auto nowc = std::chrono::steady_clock::now();
            double waitMs = std::chrono::duration<double, std::milli>(nextDeadline - nowc).count();
            if (waitMs < 0.0) { nextDeadline = nowc; waitMs = 0.0; }  // fell behind -> rebase, no spiral
            WaitForSingleObject(g_stopEvent, (DWORD)(waitMs + 0.5));
        } else {
            if (hiRes) { timeEndPeriod(1); hiRes = false; }
            nextDeadline = std::chrono::steady_clock::now();
            // Hidden/idle: wake only 4 times per second for the reveal hotspot and
            // other state checks. Active animations still use the exact same 60 FPS
            // pacing above, so animation smoothness is unchanged.
            WaitForSingleObject(g_stopEvent, 250);
        }
    }

    if (hiRes) timeEndPeriod(1);
    g_surface = nullptr;
    RemoveDismissHook();
    blurHost.Shutdown();
    renderer.Shutdown();
    if (g_hwnd) { DestroyWindow(g_hwnd); g_hwnd = nullptr; }
    // Both window classes above are registered against GetModuleHandleW(nullptr) -- the HOST
    // PROCESS's stable exe module -- while their WNDPROCs (OverlayWndProc / BlurHostWndProc)
    // live inside THIS mod DLL. Windhawk can unload+reload the mod in-place (e.g. saving an
    // Advanced-tab change like the process include/exclude list) without the host process ever
    // exiting, so leaving these registered would make the next Wh_ModInit's RegisterClassExW
    // silently no-op against the stale entry (ERROR_CLASS_ALREADY_EXISTS, ignored) and bind a
    // fresh window to a WNDPROC pointer that now lives in freed memory -- the island then
    // crashes or never reappears until the whole host process is relaunched by hand. Unregister
    // both here, now that every window of each class is destroyed, so the next load starts clean.
    UnregisterClassW(kWindowClass, GetModuleHandleW(nullptr));
    UnregisterClassW(kBlurWindowClass, GetModuleHandleW(nullptr));
    if (SUCCEEDED(hrCo)) CoUninitialize();
    return 0;
}

}  // namespace

// (Free helpers referenced above need to see Surface/Slider — defined after.)
namespace {
void SetClickThrough(HWND hwnd, bool clickThrough) {
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    LONG_PTR want = clickThrough ? (ex | WS_EX_TRANSPARENT) : (ex & ~WS_EX_TRANSPARENT);
    if (want != ex) SetWindowLongPtrW(hwnd, GWL_EXSTYLE, want);
}
void BindGlyphFormats(Surface& surface, IDWriteTextFormat* glyph) {
    surface.ForEachSlider([glyph](Slider* sl) { sl->glyphFmt = glyph; });
}
}  // namespace

// ---------------------------------------------------------------------------
// Thread lifecycle + tool-mod harness
// ---------------------------------------------------------------------------
namespace {
bool StartThreads() {
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) return false;
    g_brightEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);     // auto-reset
    g_calRefreshEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr); // auto-reset
    g_renderThread = CreateThread(nullptr, 0, RenderThreadProc, nullptr, 0, nullptr);
    g_brightThread = CreateThread(nullptr, 0, BrightnessThreadProc, nullptr, 0, nullptr);
    g_calThread = CreateThread(nullptr, 0, CalendarThreadProc, nullptr, 0, nullptr);
    g_mediaThread = CreateThread(nullptr, 0, MediaThreadProc, nullptr, 0, nullptr);
    g_audioThread = CreateThread(nullptr, 0, AudioThreadProc, nullptr, 0, nullptr);
    g_hardwareThread = CreateThread(nullptr, 0, HardwareThreadProc, nullptr, 0, nullptr);
    g_privacyThread = CreateThread(nullptr, 0, PrivacyThreadProc, nullptr, 0, nullptr);
    g_weatherThread = CreateThread(nullptr, 0, WeatherThreadProc, nullptr, 0, nullptr);
    g_lumThread = CreateThread(nullptr, 0, LuminanceThreadProc, nullptr, 0, nullptr);
    g_inputHookThread = CreateThread(nullptr, 0, InputHookThreadProc, nullptr, 0, nullptr);
#if ICC_HAS_NOTIFICATION_LISTENER
    g_notifThread = CreateThread(nullptr, 0, NotificationThreadProc, nullptr, 0, nullptr);
#endif
    return g_renderThread != nullptr;
}
void StopThreads() {
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_brightEvent) SetEvent(g_brightEvent);
    if (g_calRefreshEvent) SetEvent(g_calRefreshEvent);
    if (g_inputHookTid) PostThreadMessageW(g_inputHookTid, WM_QUIT, 0, 0);  // unblock GetMessage
    HANDLE handles[] = {g_renderThread, g_notifThread, g_brightThread, g_calThread, g_mediaThread, g_audioThread, g_hardwareThread, g_privacyThread, g_weatherThread, g_lumThread, g_inputHookThread};
    for (HANDLE h : handles) if (h) WaitForSingleObject(h, 4000);
    g_volume.Shutdown();  // unregister the device-change callback now that the render thread (its only caller) has stopped
    for (HANDLE* h : {&g_renderThread, &g_notifThread, &g_brightThread, &g_calThread, &g_mediaThread, &g_audioThread, &g_hardwareThread, &g_privacyThread, &g_weatherThread, &g_lumThread, &g_inputHookThread})
        if (*h) { CloseHandle(*h); *h = nullptr; }
    g_inputHookTid = 0;
    if (g_brightEvent) { CloseHandle(g_brightEvent); g_brightEvent = nullptr; }
    if (g_calRefreshEvent) { CloseHandle(g_calRefreshEvent); g_calRefreshEvent = nullptr; }
    if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
}
}  // namespace

// Standard Windhawk mod entry points. The mod is injected (by @include) into
// the dedicated island-host.exe process, where it runs the overlay directly.
BOOL WhTool_ModInit() {
    LoadSettings();
    if (!StartThreads()) { StopThreads(); return FALSE; }
    g_layoutDirty = true;
    Wh_Log(L"Improved Dynamic Island initialized.");
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    g_layoutDirty = true;
    g_brightRebind = true;
    if (g_brightEvent) SetEvent(g_brightEvent);
    if (g_calRefreshEvent) SetEvent(g_calRefreshEvent);
}

void WhTool_ModUninit() { StopThreads(); Wh_Log(L"Improved Dynamic Island unloaded."); }


// --- Windhawk Tool Mod Boilerplate (Do not modify) ---
////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions.
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);
    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }
        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;
        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[MAX_PATH + 2 +
        (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
        WINBOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFO, LPPROCESS_INFORMATION,
        PHANDLE);

    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
            nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS, nullptr, nullptr,
            &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }
    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }
    WhTool_ModUninit();
    ExitProcess(0);
}

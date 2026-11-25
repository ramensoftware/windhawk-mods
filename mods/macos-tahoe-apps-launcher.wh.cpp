// ==WindhawkMod==
// @id              macos-tahoe-apps-launcher
// @name            macOS 26 Tahoe Apps Launcher
// @description     Spotlight-style categorized Apps Launcher overlay with macOS 26 Tahoe Liquid Glass, grid/list toggle, instant search, and optional Pin to Dock
// @version         1.0.0
// @author          Badalemo
// @github          https://github.com/Badalemo
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luuid -lshell32 -lshlwapi -lpropsys -ldwmapi -ld2d1 -ld3d11 -ldxgi -ldwrite -lwindowscodecs -lgdi32 -luser32 -lversion -lshcore -lcomctl32 -lcomdlg32 -ladvapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# macOS 26 Tahoe Apps Launcher

A standalone Spotlight-style categorized application launcher rendered with
**macOS 26 Tahoe Liquid Glass** (`Clear` & `Tinted` modes with live DXGI refraction),
grid & list views, category pills (*Productivity, Utilities, Creativity,
Entertainment, Social*), keyboard navigation, and instant search across both
Start Menu shortcuts and UWP / Microsoft Store apps (`shell:AppsFolder`).

- **Zero idle overhead**: real-time DXGI glass capture runs *only* while the
  launcher window is open.
- **Global Shortcut**: toggle with **Win+Shift+A** (configurable in Settings).
- **Optional Dock Integration**: if **macOS 26 Tahoe Dock** (`macos-tahoe-dock`) is
  active, clicking its Apps button opens this overlay and right-clicking any
  tile lets you **Pin to Dock**.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- appearance: auto
  $name: Appearance theme
  $description: Follow Windows light/dark theme or force dark or light Tahoe glass.
  $options:
  - auto: Auto (Follow Windows)
  - dark: Dark Mode
  - light: Light Mode
- glassStyle: clear
  $name: Liquid Glass style
  $description: Clear keeps a bright translucent glass plate; Tinted boosts contrast with your Windows accent color.
  $options:
  - clear: Clear
  - tinted: Tinted
- glassRefraction: lens
  $name: Optical refraction mode
  $description: Real-time DXGI refractive edge warp around the launcher surface.
  $options:
  - lens: Lens Refraction (macOS 26 Tahoe)
  - reflection: Mirror Reflection
  - stretch: Convex Stretch
  - off: Frosted Blur Only (No DXGI Capture)
- glassEdgeDepth: 45
  $name: Edge refraction depth (0-100)
  $description: Width of the optical glass bezel refraction.
- glassEdgeStrength: 65
  $name: Specular rim strength (0-100)
  $description: Crispness of top & bottom glass specular edges.
- glassTint: 28
  $name: Glass tint opacity (0-100)
  $description: Smoked backdrop fill intensity.
- glassBlur: 50
  $name: Backdrop blur (0-100)
  $description: Frost intensity behind the launcher.
- glassBrightnessLimit: 78
  $name: Dark-mode highlight limiter (0-100)
  $description: Caps bright wallpaper patches behind dark Liquid Glass so labels stay legible.
- hotkeyCombo: winShiftA
  $name: Global toggle shortcut
  $description: Keyboard shortcut to open or close the Apps Launcher from anywhere.
  $options:
  - winShiftA: Win + Shift + A (Default)
  - winAltA: Win + Alt + A
  - ctrlShiftA: Ctrl + Shift + A
  - altSpace: Alt + Space
  - none: Disabled (Open via Dock / IPC only)
- defaultView: grid
  $name: Default view mode
  $description: Initial layout when the launcher starts.
  $options:
  - grid: Grid View
  - list: List View
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windhawk_utils.h>

#include <windows.h>
#include <appmodel.h>
#include <commctrl.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1_3.h>
#include <d2d1effects.h>
#include <d3d11.h>
#include <dispatcherqueue.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <ole2.h>
#include <propkey.h>
#include <propsys.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <wincodec.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cwctype>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.Graphics.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.h>

namespace wuc = winrt::Windows::UI::Composition;
namespace wf = winrt::Windows::Foundation;
namespace wge = winrt::Windows::Graphics::Effects;

constexpr DWORD kDwmUseHostBackdropBrush = 17;
constexpr DWORD kWdaExcludeFromCapture = 0x00000011;
constexpr UINT kWmAppSettings = WM_APP + 101;
constexpr UINT kWmAppQuit = WM_APP + 102;
constexpr UINT kWmAppDeferredAction = WM_APP + 105;
constexpr UINT kWmAppGlassFrame = WM_APP + 106;

constexpr UINT_PTR kTimerGlassMaintenance = 5;
constexpr int kHotkeyToggleApps = 2001;
constexpr WCHAR kAppsHostClassName[] = L"WindhawkMacOSTahoeAppsHost";

// Local WinRT Composition interop interfaces to avoid ABI header clashes.
struct ICompositorDesktopInteropLocal : ::IUnknown {
    virtual HRESULT __stdcall CreateDesktopWindowTarget(HWND window,
                                                        BOOL isTopmost,
                                                        void** target) = 0;
    virtual HRESULT __stdcall EnsureOnThread(DWORD threadId) = 0;
};
template <>
inline constexpr winrt::guid
    winrt::impl::guid_v<ICompositorDesktopInteropLocal>{
        0x29E691FA,
        0x4567,
        0x4DCA,
        {0xB3, 0x19, 0xD0, 0xF2, 0x07, 0xEB, 0x68, 0x07}};

struct ICompositorInteropLocal : ::IUnknown {
    virtual HRESULT __stdcall CreateCompositionSurfaceForHandle(
        HANDLE swapChain,
        void** result) = 0;
    virtual HRESULT __stdcall CreateCompositionSurfaceForSwapChain(
        ::IUnknown* swapChain,
        void** result) = 0;
    virtual HRESULT __stdcall CreateGraphicsDevice(::IUnknown* renderingDevice,
                                                   void** result) = 0;
};
template <>
inline constexpr winrt::guid winrt::impl::guid_v<ICompositorInteropLocal>{
    0x25297D5C,
    0x3AD4,
    0x4C9C,
    {0xB5, 0xCF, 0xE3, 0x6A, 0x38, 0x51, 0x23, 0x30}};

struct ICompositionGraphicsDeviceInteropLocal : ::IUnknown {
    virtual HRESULT __stdcall GetRenderingDevice(::IUnknown** value) = 0;
    virtual HRESULT __stdcall SetRenderingDevice(::IUnknown* value) = 0;
};
template <>
inline constexpr winrt::guid
    winrt::impl::guid_v<ICompositionGraphicsDeviceInteropLocal>{
        0xA116FF71,
        0xF8BF,
        0x4C8A,
        {0x9C, 0x98, 0x70, 0x77, 0x9A, 0x32, 0xA9, 0xC8}};

struct ICompositionDrawingSurfaceInteropLocal : ::IUnknown {
    virtual HRESULT __stdcall BeginDraw(const RECT* updateRect,
                                        REFIID iid,
                                        void** updateObject,
                                        POINT* updateOffset) = 0;
    virtual HRESULT __stdcall EndDraw() = 0;
    virtual HRESULT __stdcall Resize(SIZE sizePixels) = 0;
    virtual HRESULT __stdcall Scroll(const RECT* scrollRect,
                                     const RECT* clipRect,
                                     int offsetX,
                                     int offsetY) = 0;
    virtual HRESULT __stdcall ResumeDraw() = 0;
    virtual HRESULT __stdcall SuspendDraw() = 0;
};
template <>
inline constexpr winrt::guid
    winrt::impl::guid_v<ICompositionDrawingSurfaceInteropLocal>{
        0xFD04E6E3,
        0xFE0C,
        0x4C3C,
        {0xAB, 0x19, 0xA0, 0x76, 0x01, 0xA5, 0x76, 0xEE}};

enum DOCK_EFFECT_PROPERTY_MAPPING : int {
    DOCK_EFFECT_PROPERTY_MAPPING_DIRECT = 1,
};

MIDL_INTERFACE("2FC57384-A068-44D7-A331-30982FCF7177")
IDockGraphicsEffectD2D1Interop : public ::IUnknown {
public:
    virtual HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name, UINT* index, DOCK_EFFECT_PROPERTY_MAPPING* mapping) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperty(UINT index, void** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSource(UINT index, void** source) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) = 0;
};
__CRT_UUID_DECL(IDockGraphicsEffectD2D1Interop,
                0x2fc57384,
                0xa068,
                0x44d7,
                0xa3,
                0x31,
                0x30,
                0x98,
                0x2f,
                0xcf,
                0x71,
                0x77)

struct IGeometrySource2DInteropLocal : ::IUnknown {
    virtual HRESULT __stdcall GetGeometry(ID2D1Geometry** value) = 0;
    virtual HRESULT __stdcall TryGetGeometryUsingFactory(
        ID2D1Factory* factory,
        ID2D1Geometry** value) = 0;
};
template <>
inline constexpr winrt::guid
    winrt::impl::guid_v<IGeometrySource2DInteropLocal>{
        0x0657AF73,
        0x53FD,
        0x47CF,
        {0x84, 0xFF, 0xC8, 0x49, 0x2D, 0x2A, 0x80, 0xA3}};

struct GlassPath
    : winrt::implements<GlassPath,
                        winrt::Windows::Graphics::IGeometrySource2D,
                        IGeometrySource2DInteropLocal> {
    explicit GlassPath(ID2D1Geometry* geometry) {
        m_geometry.copy_from(geometry);
    }
    HRESULT __stdcall GetGeometry(ID2D1Geometry** value) noexcept override {
        if (!value) {
            return E_POINTER;
        }
        m_geometry.copy_to(value);
        return S_OK;
    }
    HRESULT __stdcall TryGetGeometryUsingFactory(
        ID2D1Factory*,
        ID2D1Geometry** value) noexcept override {
        if (!value) {
            return E_POINTER;
        }
        *value = nullptr;
        return E_NOTIMPL;
    }

private:
    winrt::com_ptr<ID2D1Geometry> m_geometry;
};

// ============================================================================

// ============================================================================
// 3. Settings & Minimal Host State Bridge
// ============================================================================

enum class AppearanceMode { Auto = 0, Dark = 1, Light = 2 };
enum class GlassStyle { Clear = 0, Tinted = 1 };
enum class GlassRefraction { Lens = 0, Reflection = 1, Stretch = 2, Off = 3 };
enum class HotkeyCombo { WinShiftA = 0, WinAltA = 1, CtrlShiftA = 2, AltSpace = 3, None = 4 };

struct DockSettings {
    AppearanceMode appearance = AppearanceMode::Auto;
    GlassStyle glassStyle = GlassStyle::Clear;
    GlassRefraction glassRefraction = GlassRefraction::Lens;
    int glassEdgeDepth = 45;
    int glassEdgeStrength = 65;
    int glassTint = 28;
    int glassBlur = 50;
    int glassBrightnessLimit = 78;
    HotkeyCombo hotkeyCombo = HotkeyCombo::WinShiftA;
    bool defaultListView = false;
    std::wstring fontFamily;
};

struct DockWindowState {
    HWND hwnd = nullptr;
    wuc::CompositionBackdropBrush hostBackdrop{nullptr};
    HMONITOR monitor = nullptr;
    RECT monitorRect{};
    float scale = 1.0f;
    bool darkMode = true;
    bool modalActive = false;
    bool hasHostBackdrop = true;
};

DockSettings g_settings;
DockWindowState g_dock;
bool g_quitting = false;
UINT g_msgToggleApps = 0;

std::wstring ReadWhStringSetting(PCWSTR name) {
    PCWSTR raw = Wh_GetStringSetting(name);
    std::wstring out = raw ? raw : L"";
    if (raw) {
        Wh_FreeStringSetting(raw);
    }
    return out;
}

inline void RequestAnimationFrame() {}
inline void OnFinderBarMenuClosed() {}
inline bool PointInRectF(const D2D1_RECT_F& r, float x, float y) {
    return x >= r.left && x <= r.right && y >= r.top && y <= r.bottom;
}

void LoadSettings();
void UpdateGlassAppearance();
void CloseTahoeAppsLauncher();
void RenderTahoeAppsLauncher();
void RefreshOpticalGlassPlatesOnly();
void ToggleTahoeAppsLauncher(const D2D1_RECT_F& iconRectLocal);

// 4. Utilities
// ============================================================================

double NowSeconds() {
    static LARGE_INTEGER freq = [] {
        LARGE_INTEGER f{};
        QueryPerformanceFrequency(&f);
        return f;
    }();
    LARGE_INTEGER now{};
    QueryPerformanceCounter(&now);
    return static_cast<double>(now.QuadPart) / static_cast<double>(freq.QuadPart);
}

inline float Clamp01(float x) {
    return std::clamp(x, 0.0f, 1.0f);
}

inline float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

inline float EaseOutCubic(float t) {
    t = Clamp01(t);
    float u = 1.0f - t;
    return 1.0f - u * u * u;
}

inline float EaseInCubic(float t) {
    t = Clamp01(t);
    return t * t * t;
}

inline float EaseInOutCubic(float t) {
    t = Clamp01(t);
    return t < 0.5f ? 4.0f * t * t * t
                    : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}

inline float Smoothstep(float edge0, float edge1, float x) {
    if (edge1 <= edge0) {
        return x >= edge1 ? 1.0f : 0.0f;
    }
    float t = Clamp01((x - edge0) / (edge1 - edge0));
    return t * t * (3.0f - 2.0f * t);
}

bool StepCriticallyDampedSpring(float& value,
                                float& velocity,
                                float target,
                                float omega,
                                float dt) {
    if (dt <= 0.0f) {
        return std::fabs(value - target) > 0.0005f ||
               std::fabs(velocity) > 0.0005f;
    }
    dt = std::min(dt, 0.05f);
    float x = value - target;
    float e = std::exp(-omega * dt);
    float temp = (velocity + omega * x) * dt;
    velocity = (velocity - omega * temp) * e;
    value = target + (x + temp) * e;
    if (std::fabs(value - target) < 0.0005f && std::fabs(velocity) < 0.0005f) {
        value = target;
        velocity = 0.0f;
        return false;
    }
    return true;
}

std::wstring ToLower(std::wstring_view sv) {
    std::wstring out(sv);
    for (wchar_t& ch : out) {
        ch = static_cast<wchar_t>(towlower(ch));
    }
    return out;
}

bool EqualsIgnoreCase(std::wstring_view a, std::wstring_view b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); ++i) {
        if (towlower(a[i]) != towlower(b[i])) {
            return false;
        }
    }
    return true;
}

std::wstring NormalizePath(std::wstring_view p) {
    std::wstring s(p);
    while (!s.empty() && (s.back() == L'\\' || s.back() == L'/')) {
        if (s.size() <= 3 && s.size() >= 2 && s[1] == L':') {
            break;
        }
        s.pop_back();
    }
    for (wchar_t& ch : s) {
        if (ch == L'/') {
            ch = L'\\';
        } else {
            ch = static_cast<wchar_t>(towlower(ch));
        }
    }
    return s;
}

HMONITOR GetPrimaryMonitor() {
    POINT zero{0, 0};
    return MonitorFromPoint(zero, MONITOR_DEFAULTTOPRIMARY);
}

float GetMonitorDpiScale(HMONITOR mon) {
    if (!mon) {
        mon = GetPrimaryMonitor();
    }
    UINT dpiX = 96, dpiY = 96;
    if (SUCCEEDED(GetDpiForMonitor(mon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) &&
        dpiX > 0) {
        return static_cast<float>(dpiX) / 96.0f;
    }
    return 1.0f;
}

bool IsSystemDarkMode() {
    if (g_settings.appearance == AppearanceMode::Light) {
        return false;
    }
    if (g_settings.appearance == AppearanceMode::Dark) {
        return true;
    }
    DWORD val = 1;
    DWORD sz = sizeof(val);
    if (RegGetValueW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &val,
            &sz) == ERROR_SUCCESS) {
        return val == 0;
    }
    return false;
}

winrt::Windows::UI::Color GetAccentColor() {
    DWORD argb = 0xFF0078D4;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&argb, &opaque))) {
        return winrt::Windows::UI::Color{
            255, static_cast<uint8_t>((argb >> 16) & 0xFF),
            static_cast<uint8_t>((argb >> 8) & 0xFF),
            static_cast<uint8_t>(argb & 0xFF)};
    }
    return winrt::Windows::UI::Color{255, 0, 120, 212};
}

// ============================================================================
// 5. Composition Effects
// ============================================================================

constexpr GUID kGaussianBlurEffectId = {
    0x1feb6d69,
    0x2fe6,
    0x4ac9,
    {0x8c, 0x58, 0x1d, 0x7f, 0x93, 0xe7, 0xa6, 0xa5}};

class GaussianBlurEffect
    : public winrt::implements<GaussianBlurEffect,
                               wge::IGraphicsEffect,
                               wge::IGraphicsEffectSource,
                               IDockGraphicsEffectD2D1Interop> {
public:
    GaussianBlurEffect(float blurAmount,
                       wge::IGraphicsEffectSource const& source)
        : m_blurAmount(blurAmount), m_source(source) {}

    winrt::hstring Name() const { return m_name; }
    void Name(winrt::hstring const& name) { m_name = name; }

    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) {
            return E_POINTER;
        }
        *id = kGaussianBlurEffectId;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name,
        UINT* index,
        DOCK_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!name || !index || !mapping) {
            return E_POINTER;
        }
        if (_wcsicmp(name, L"BlurAmount") == 0) {
            *index = 0;
            *mapping = DOCK_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }
        return E_INVALIDARG;
    }

    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) {
            return E_POINTER;
        }
        *count = 3;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetProperty(UINT index,
                                          void** value) noexcept override {
        if (!value) {
            return E_POINTER;
        }
        try {
            wf::IInspectable v;
            switch (index) {
                case 0:
                    v = wf::PropertyValue::CreateSingle(m_blurAmount);
                    break;
                case 1:
                    v = wf::PropertyValue::CreateUInt32(1);  // balanced
                    break;
                case 2:
                    v = wf::PropertyValue::CreateUInt32(1);  // hard border
                    break;
                default:
                    return E_INVALIDARG;
            }
            *value = winrt::detach_abi(v.as<wf::IPropertyValue>());
            return S_OK;
        } catch (...) {
            return winrt::to_hresult();
        }
    }

    HRESULT STDMETHODCALLTYPE GetSource(UINT index,
                                        void** source) noexcept override {
        if (!source) {
            return E_POINTER;
        }
        if (index != 0) {
            return E_INVALIDARG;
        }
        winrt::copy_to_abi(m_source, *source);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) {
            return E_POINTER;
        }
        *count = 1;
        return S_OK;
    }

private:
    float m_blurAmount = 18.0f;
    wge::IGraphicsEffectSource m_source{nullptr};
    winrt::hstring m_name;
};

constexpr GUID kSaturationEffectId = {
    0x5cb2d9cf,
    0x327d,
    0x459f,
    {0xa0, 0xce, 0x40, 0xc0, 0xb2, 0x08, 0x6b, 0xf7}};

class SaturationEffect
    : public winrt::implements<SaturationEffect,
                               wge::IGraphicsEffect,
                               wge::IGraphicsEffectSource,
                               IDockGraphicsEffectD2D1Interop> {
public:
    SaturationEffect(float saturation, wge::IGraphicsEffectSource const& source)
        : m_saturation(saturation), m_source(source) {}

    winrt::hstring Name() const { return m_name; }
    void Name(winrt::hstring const& name) { m_name = name; }

    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) {
            return E_POINTER;
        }
        *id = kSaturationEffectId;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(
        LPCWSTR name,
        UINT* index,
        DOCK_EFFECT_PROPERTY_MAPPING* mapping) noexcept override {
        if (!name || !index || !mapping) {
            return E_POINTER;
        }
        if (_wcsicmp(name, L"Saturation") == 0) {
            *index = 0;
            *mapping = DOCK_EFFECT_PROPERTY_MAPPING_DIRECT;
            return S_OK;
        }
        return E_INVALIDARG;
    }

    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) {
            return E_POINTER;
        }
        *count = 1;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetProperty(UINT index,
                                          void** value) noexcept override {
        if (!value) {
            return E_POINTER;
        }
        try {
            if (index != 0) {
                return E_INVALIDARG;
            }
            auto pv = wf::PropertyValue::CreateSingle(m_saturation);
            *value = winrt::detach_abi(pv.as<wf::IPropertyValue>());
            return S_OK;
        } catch (...) {
            return winrt::to_hresult();
        }
    }

    HRESULT STDMETHODCALLTYPE GetSource(UINT index,
                                        void** source) noexcept override {
        if (!source) {
            return E_POINTER;
        }
        if (index != 0) {
            return E_INVALIDARG;
        }
        winrt::copy_to_abi(m_source, *source);
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) {
            return E_POINTER;
        }
        *count = 1;
        return S_OK;
    }

private:
    float m_saturation = 1.0f;
    wge::IGraphicsEffectSource m_source{nullptr};
    winrt::hstring m_name;
};

wuc::CompositionBrush CreateLiquidGlassBackdropBrush(
    wuc::Compositor const& c,
    wuc::CompositionBackdropBrush const& hostBackdrop,
    float blurStdDev,
    float saturation) {
    try {
        wge::IGraphicsEffectSource src =
            wuc::CompositionEffectSourceParameter(L"backdrop");
        wge::IGraphicsEffect top{nullptr};
        if (blurStdDev > 0.25f) {
            auto blur = winrt::make<GaussianBlurEffect>(blurStdDev, src);
            blur.Name(L"Blur");
            top = blur;
            src = blur.as<wge::IGraphicsEffectSource>();
        }
        auto sat = winrt::make<SaturationEffect>(saturation, src);
        sat.Name(L"Sat");
        top = sat;
        auto factory = c.CreateEffectFactory(top);
        auto brush = factory.CreateBrush();
        brush.SetSourceParameter(L"backdrop", hostBackdrop);
        return brush;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"LiquidGlass chained effect failed (0x%08X), trying blur only",
               (unsigned)e.code());
    }
    if (blurStdDev > 0.25f) {
        try {
            auto blur = winrt::make<GaussianBlurEffect>(
                blurStdDev, wuc::CompositionEffectSourceParameter(L"backdrop"));
            auto factory = c.CreateEffectFactory(blur);
            auto brush = factory.CreateBrush();
            brush.SetSourceParameter(L"backdrop", hostBackdrop);
            return brush;
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"LiquidGlass blur effect failed (0x%08X), using raw backdrop",
                   (unsigned)e.code());
        }
    }
    return hostBackdrop;
}

// ============================================================================
// 6. Graphics Device & Surfaces
// ============================================================================

struct GraphicsContext {
    winrt::com_ptr<ID3D11Device> d3dDevice;
    winrt::com_ptr<ID3D11DeviceContext> d3dContext;
    winrt::com_ptr<ID2D1Factory1> d2dFactory;
    winrt::com_ptr<ID2D1Multithread> d2dMultithread;
    winrt::com_ptr<ID2D1Device> d2dDevice;
    winrt::com_ptr<IDWriteFactory> dwriteFactory;
    winrt::com_ptr<IWICImagingFactory> wicFactory;
    wuc::Compositor compositor{nullptr};
    wuc::CompositionGraphicsDevice compGraphics{nullptr};
    winrt::event_token deviceReplacedToken{};

    void Shutdown() {
        if (compGraphics && deviceReplacedToken.value != 0) {
            try {
                compGraphics.RenderingDeviceReplaced(deviceReplacedToken);
            } catch (...) {
            }
            deviceReplacedToken = {};
        }
        compGraphics = nullptr;
        compositor = nullptr;
        wicFactory = nullptr;
        dwriteFactory = nullptr;
        d2dDevice = nullptr;
        d2dMultithread = nullptr;
        d2dFactory = nullptr;
        d3dContext = nullptr;
        d3dDevice = nullptr;
    }
};

GraphicsContext g_gfx;

winrt::com_ptr<IDXGIAdapter1> FindAdapterForMonitor(HMONITOR monitor) {
    winrt::com_ptr<IDXGIFactory1> factory;
    if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), factory.put_void())) ||
        !factory) {
        return nullptr;
    }
    winrt::com_ptr<IDXGIAdapter1> adapter;
    for (UINT i = 0; SUCCEEDED(factory->EnumAdapters1(i, adapter.put()));
         ++i, adapter = nullptr) {
        winrt::com_ptr<IDXGIOutput> output;
        for (UINT j = 0; SUCCEEDED(adapter->EnumOutputs(j, output.put()));
             ++j, output = nullptr) {
            DXGI_OUTPUT_DESC desc{};
            if (SUCCEEDED(output->GetDesc(&desc)) && desc.Monitor == monitor) {
                return adapter;
            }
        }
    }
    return nullptr;
}

bool InitD3DAndD2D() {
    g_gfx.d2dDevice = nullptr;
    g_gfx.d3dContext = nullptr;
    g_gfx.d3dDevice = nullptr;

    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    D3D_FEATURE_LEVEL obtained{};
    auto adapter = FindAdapterForMonitor(GetPrimaryMonitor());
    HRESULT hr = D3D11CreateDevice(
        adapter.get(),
        adapter ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE, nullptr,
        flags, levels, ARRAYSIZE(levels), D3D11_SDK_VERSION,
        g_gfx.d3dDevice.put(), &obtained, g_gfx.d3dContext.put());
    if (FAILED(hr) && adapter) {
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
                               flags, levels, ARRAYSIZE(levels),
                               D3D11_SDK_VERSION, g_gfx.d3dDevice.put(),
                               &obtained, g_gfx.d3dContext.put());
    }
    if (FAILED(hr)) {
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, flags,
                               levels, ARRAYSIZE(levels), D3D11_SDK_VERSION,
                               g_gfx.d3dDevice.put(), &obtained,
                               g_gfx.d3dContext.put());
    }
    if (FAILED(hr)) {
        Wh_Log(L"D3D11CreateDevice failed: 0x%08X", (unsigned)hr);
        return false;
    }

    winrt::com_ptr<IDXGIDevice> dxgiDevice;
    hr = g_gfx.d3dDevice->QueryInterface(IID_PPV_ARGS(dxgiDevice.put()));
    if (FAILED(hr)) {
        Wh_Log(L"QueryInterface IDXGIDevice failed: 0x%08X", (unsigned)hr);
        return false;
    }

    if (!g_gfx.d2dFactory) {
        D2D1_FACTORY_OPTIONS opts{};
        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED,
                               __uuidof(ID2D1Factory1), &opts,
                               g_gfx.d2dFactory.put_void());
        if (FAILED(hr)) {
            Wh_Log(L"D2D1CreateFactory failed: 0x%08X", (unsigned)hr);
            return false;
        }
        g_gfx.d2dMultithread = g_gfx.d2dFactory.try_as<ID2D1Multithread>();
    }

    hr = g_gfx.d2dFactory->CreateDevice(dxgiDevice.get(),
                                        g_gfx.d2dDevice.put());
    if (FAILED(hr)) {
        Wh_Log(L"CreateDevice ID2D1Device failed: 0x%08X", (unsigned)hr);
        return false;
    }
    return true;
}

void OnGraphicsDeviceLost();

bool InitGraphics(wuc::Compositor const& compositor) {
    g_gfx.compositor = compositor;

    if (!g_gfx.dwriteFactory) {
        HRESULT hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(g_gfx.dwriteFactory.put()));
        if (FAILED(hr)) {
            Wh_Log(L"DWriteCreateFactory failed: 0x%08X", (unsigned)hr);
            return false;
        }
    }

    if (!g_gfx.wicFactory) {
        HRESULT hr = CoCreateInstance(
            CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(g_gfx.wicFactory.put()));
        if (FAILED(hr)) {
            Wh_Log(L"WICImagingFactory failed: 0x%08X", (unsigned)hr);
            return false;
        }
    }

    if (!InitD3DAndD2D()) {
        return false;
    }

    auto interop = compositor.as<ICompositorInteropLocal>();
    HRESULT hr = interop->CreateGraphicsDevice(
        g_gfx.d2dDevice.get(), winrt::put_abi(g_gfx.compGraphics));
    if (FAILED(hr) || !g_gfx.compGraphics) {
        Wh_Log(L"CreateGraphicsDevice failed: 0x%08X", (unsigned)hr);
        return false;
    }

    g_gfx.deviceReplacedToken = g_gfx.compGraphics.RenderingDeviceReplaced(
        [](wuc::CompositionGraphicsDevice const& sender,
           wuc::RenderingDeviceReplacedEventArgs const&) {
            if (InitD3DAndD2D()) {
                auto gfxInterop =
                    sender.as<ICompositionGraphicsDeviceInteropLocal>();
                gfxInterop->SetRenderingDevice(g_gfx.d2dDevice.get());
            }
            OnGraphicsDeviceLost();
        });
    return true;
}

template <typename Fn>
bool DrawCompositionSurface(wuc::CompositionDrawingSurface const& surface,
                            Fn&& drawFn) {
    if (!surface) {
        return false;
    }
    auto interop = surface.as<ICompositionDrawingSurfaceInteropLocal>();
    winrt::com_ptr<ID2D1DeviceContext> dc;
    POINT offset{};
    HRESULT hr = interop->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext),
                                    dc.put_void(), &offset);
    if (FAILED(hr) || !dc) {
        return false;
    }

    bool ok = false;
    try {
        dc->SetDpi(96.0f, 96.0f);
        dc->SetTransform(D2D1::Matrix3x2F::Translation(
            static_cast<float>(offset.x), static_cast<float>(offset.y)));
        dc->Clear(D2D1::ColorF(0, 0, 0, 0));
        drawFn(dc.get());
        ok = true;
    } catch (...) {
        ok = false;
    }
    HRESULT endHr = interop->EndDraw();
    return ok && SUCCEEDED(endHr);
}

wuc::CompositionDrawingSurface CreateDrawingSurface(float widthPx,
                                                    float heightPx) {
    if (!g_gfx.compGraphics) {
        return nullptr;
    }
    float w = std::max(1.0f, std::ceil(widthPx));
    float h = std::max(1.0f, std::ceil(heightPx));
    return g_gfx.compGraphics.CreateDrawingSurface(
        wf::Size{w, h},
        winrt::Windows::Graphics::DirectX::DirectXPixelFormat::
            B8G8R8A8UIntNormalized,
        winrt::Windows::Graphics::DirectX::DirectXAlphaMode::Premultiplied);
}

struct ShadowSurfaceResult {
    wuc::CompositionDrawingSurface surface{nullptr};
    float padPx = 0.0f;
    float cornerPx = 0.0f;
    float widthPx = 0.0f;
    float heightPx = 0.0f;
};

ShadowSurfaceResult BuildShelfShadowSurface(float pillHeightPx,
                                            float cornerRadiusPx,
                                            float scale,
                                            bool darkMode) {
    ShadowSurfaceResult res;
    const float blurStdDev = 12.0f * scale;
    const float pad = std::ceil(blurStdDev * 2.5f + 6.0f * scale);
    const float innerW = std::ceil(cornerRadiusPx * 2.0f + 16.0f * scale);
    const float innerH = std::ceil(pillHeightPx);
    const float totalW = innerW + pad * 2.0f;
    const float totalH = innerH + pad * 2.0f;

    res.padPx = pad;
    res.cornerPx = cornerRadiusPx;
    res.widthPx = totalW;
    res.heightPx = totalH;
    res.surface = CreateDrawingSurface(totalW, totalH);
    if (!res.surface) {
        return res;
    }

    DrawCompositionSurface(res.surface, [&](ID2D1DeviceContext* dc) {
        winrt::com_ptr<ID2D1BitmapRenderTarget> rt;
        if (FAILED(dc->CreateCompatibleRenderTarget(
                D2D1::SizeF(totalW, totalH), rt.put()))) {
            return;
        }
        rt->BeginDraw();
        rt->Clear(D2D1::ColorF(0, 0, 0, 0));
        winrt::com_ptr<ID2D1SolidColorBrush> fillBrush;
        rt->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f), fillBrush.put());
        D2D1_ROUNDED_RECT rr{
            D2D1::RectF(pad, pad, pad + innerW, pad + innerH),
            cornerRadiusPx,
            cornerRadiusPx,
        };
        rt->FillRoundedRectangle(rr, fillBrush.get());
        rt->EndDraw();

        winrt::com_ptr<ID2D1Bitmap> shapeBmp;
        if (FAILED(rt->GetBitmap(shapeBmp.put()))) {
            return;
        }

        winrt::com_ptr<ID2D1Effect> shadowEffect;
        if (FAILED(dc->CreateEffect(CLSID_D2D1Shadow, shadowEffect.put()))) {
            return;
        }
        shadowEffect->SetInput(0, shapeBmp.get());
        shadowEffect->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION,
                               blurStdDev);
        const float shadowAlpha = darkMode ? 0.52f : 0.32f;
        shadowEffect->SetValue(
            D2D1_SHADOW_PROP_COLOR,
            D2D1::Vector4F(0.0f, 0.0f, 0.0f, shadowAlpha));
        shadowEffect->SetValue(D2D1_SHADOW_PROP_OPTIMIZATION,
                               static_cast<UINT32>(1));

        dc->DrawImage(shadowEffect.get(),
                      D2D1::Point2F(0.0f, 4.0f * scale));
    });
    return res;
}

// Local Shell GUIDs / property keys to keep linkage self-contained.
constexpr GUID kFolderIdAppsFolder = {
    0x1e87508d, 0x89c2, 0x42f0, {0x8a, 0x7e, 0x64, 0x5a, 0x0f, 0x50, 0xca, 0x58}};

// Apple continuous-curvature squircle corner Bézier control points (PaintCode).
constexpr float kAppleCornerReach = 1.52866f;
constexpr float kAppleCorner[16][2] = {
    {kAppleCornerReach, 0.0f},
    {1.08849f, 0.0f}, {0.86841f, 0.0f}, {0.66993f, 0.06550f},
    {0.66993f, 0.06550f}, {0.63149f, 0.07491f}, {0.63149f, 0.07491f},
    {0.37282f, 0.16906f}, {0.16906f, 0.37282f}, {0.07491f, 0.63149f},
    {0.07491f, 0.63149f}, {0.06550f, 0.66993f}, {0.06550f, 0.66993f},
    {0.0f, 0.86841f}, {0.0f, 1.08849f}, {0.0f, kAppleCornerReach},
};

winrt::com_ptr<ID2D1PathGeometry> BuildContinuousSquirclePath(float x,
                                                              float y,
                                                              float w,
                                                              float h,
                                                              float radius) {
    winrt::com_ptr<ID2D1PathGeometry> geom;
    if (!g_gfx.d2dFactory || w <= 0.0f || h <= 0.0f ||
        FAILED(g_gfx.d2dFactory->CreatePathGeometry(geom.put()))) {
        return nullptr;
    }
    winrt::com_ptr<ID2D1GeometrySink> sink;
    if (FAILED(geom->Open(sink.put()))) {
        return nullptr;
    }

    float r = std::clamp(radius, 0.0f,
                         std::min(w, h) * 0.5f / kAppleCornerReach);

    struct CornerSpec {
        float cx, cy, inX, inY, outX, outY;
    };
    const CornerSpec corners[4] = {
        {x + w, y, -1, 0, 0, 1},
        {x + w, y + h, 0, -1, -1, 0},
        {x, y + h, 1, 0, 0, -1},
        {x, y, 0, 1, 1, 0},
    };

    std::vector<D2D1_POINT_2F> pts;
    pts.reserve(76);
    for (const auto& c : corners) {
        for (int i = 0; i < 16; ++i) {
            float in = kAppleCorner[i][0];
            float out = kAppleCorner[i][1];
            D2D1_POINT_2F pt = D2D1::Point2F(
                c.cx + (in * c.inX + out * c.outX) * r,
                c.cy + (in * c.inY + out * c.outY) * r);
            if (i == 0 && !pts.empty()) {
                pts.push_back(pts.back());
                pts.push_back(pt);
            }
            pts.push_back(pt);
        }
    }

    sink->BeginFigure(pts[0], D2D1_FIGURE_BEGIN_FILLED);
    for (size_t i = 1; i + 2 < pts.size(); i += 3) {
        sink->AddBezier(D2D1::BezierSegment(pts[i], pts[i + 1], pts[i + 2]));
    }
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    return geom;
}

wuc::CompositionPath MakeCompositionSquirclePath(float x,
                                                 float y,
                                                 float w,
                                                 float h,
                                                 float radius) {
    auto d2dPath = BuildContinuousSquirclePath(x, y, w, h, radius);
    if (!d2dPath) {
        return nullptr;
    }
    auto source = winrt::make<GlassPath>(d2dPath.get());
    return wuc::CompositionPath(source);
}

// Apple continuous-curvature squircle for app icons (radius = 22.5% of side).
winrt::com_ptr<ID2D1PathGeometry> BuildSquircle(D2D1_RECT_F rect) {
    float w = rect.right - rect.left;
    float h = rect.bottom - rect.top;
    return BuildContinuousSquirclePath(rect.left, rect.top, w, h,
                                       std::min(w, h) * 0.225f);
}

// ============================================================================
// 6b. True macOS 27 Optical Liquid Glass Engine (DXGI Capture + Refraction)
// ============================================================================

constexpr GUID kClsidD2D1DisplacementMap = {
    0xedc48364,
    0x0417,
    0x4111,
    {0x94, 0x50, 0x43, 0x84, 0x5f, 0xa9, 0xf8, 0x90}};
constexpr GUID kClsidD2D1Crop = {
    0xe23f7110,
    0x0e9a,
    0x4324,
    {0xaf, 0x47, 0x6a, 0x2c, 0x0c, 0x46, 0xf3, 0x5b}};
constexpr GUID kClsidD2D1Border = {
    0x2a2d49c0,
    0x4acf,
    0x43c7,
    {0x8c, 0x6a, 0x7c, 0x4a, 0x27, 0x87, 0x4d, 0x27}};
constexpr GUID kClsidD2D1ColorMatrix = {
    0x921f03d6,
    0x641c,
    0x47df,
    {0x85, 0x2d, 0xb4, 0xbb, 0x61, 0x53, 0xae, 0x11}};
constexpr GUID kClsidD2D12DAffineTransform = {
    0x6aa97485,
    0x6354,
    0x4cfc,
    {0x90, 0x8c, 0xe4, 0xa7, 0x4f, 0x62, 0xc9, 0x6c}};

extern bool g_quitting;
HWND GetGlassNotifyHwnd();
void ApplyCaptureExclusionToAllModWindows(bool excluded);

struct D2DMultithreadLock {
    ID2D1Multithread* lock = nullptr;
    explicit D2DMultithreadLock(ID2D1Multithread* l) : lock(l) {
        if (lock) {
            lock->Enter();
        }
    }
    ~D2DMultithreadLock() {
        if (lock) {
            lock->Leave();
        }
    }
};

struct LiquidCaptureWorker {
    winrt::com_ptr<ID3D11Device> device;
    winrt::com_ptr<ID3D11DeviceContext> context;
    winrt::com_ptr<IDXGIOutputDuplication> duplication;
    winrt::com_ptr<ID3D11Texture2D> sharedTex;
    winrt::com_ptr<IDXGIKeyedMutex> sharedLock;
    RECT screenRect{};
    std::atomic<bool> stop{false};
    std::atomic<uint64_t> copied{0};
    enum State : int { Running = 0, Lost = 1, Failed = 2 };
    std::atomic<int> state{Running};
    std::atomic<HRESULT> error{S_OK};
};

struct LiquidCaptureState {
    HMONITOR monitor = nullptr;
    RECT screenRect{};
    std::shared_ptr<LiquidCaptureWorker> worker;
    HANDLE workerThread = nullptr;
    winrt::com_ptr<ID3D11Texture2D> openedSharedTex;
    winrt::com_ptr<IDXGIKeyedMutex> openedSharedLock;
    uint64_t takenCopies = 0;

    winrt::com_ptr<ID3D11Texture2D> desktopTex;
    uint64_t frameId = 0;

    bool excludedApplied = false;
    ULONGLONG excludedAtMs = 0;
    ULONGLONG retryAtMs = 0;
    bool unsupported = false;
    bool loggedFail = false;
    bool frozen = false;
    int consecutiveFailures = 0;

    std::atomic<bool> framePosted{false};
    std::atomic<bool> watching{false};

    std::mutex regionsMutex;
    std::vector<RECT> watchedRegions;
    std::vector<RECT> lastUiRegions;
};

LiquidCaptureState g_liquidCapture;

void UpdateLiquidCaptureWatchedRegions(const std::vector<RECT>& regions) {
    auto& prev = g_liquidCapture.lastUiRegions;
    if (prev.size() == regions.size()) {
        bool same = true;
        for (size_t i = 0; i < regions.size(); ++i) {
            if (!EqualRect(&prev[i], &regions[i])) {
                same = false;
                break;
            }
        }
        if (same) {
            return;
        }
    }
    prev = regions;
    std::lock_guard<std::mutex> lock(g_liquidCapture.regionsMutex);
    g_liquidCapture.watchedRegions = regions;
}

bool DuplicationChangedNearWatched(IDXGIOutputDuplication* dup,
                                   const RECT& screenRect,
                                   const DXGI_OUTDUPL_FRAME_INFO& info,
                                   std::vector<BYTE>& metaBuf) {
    if (!info.TotalMetadataBufferSize) {
        return true;
    }
    std::vector<RECT> regions;
    {
        std::lock_guard<std::mutex> lock(g_liquidCapture.regionsMutex);
        regions = g_liquidCapture.watchedRegions;
    }
    if (regions.empty()) {
        return true;
    }
    for (auto& r : regions) {
        OffsetRect(&r, -screenRect.left, -screenRect.top);
    }

    metaBuf.resize(info.TotalMetadataBufferSize);
    UINT sz = 0;
    RECT hit{};
    auto* moves = reinterpret_cast<DXGI_OUTDUPL_MOVE_RECT*>(metaBuf.data());
    if (FAILED(dup->GetFrameMoveRects(static_cast<UINT>(metaBuf.size()), moves,
                                      &sz))) {
        return true;
    }
    UINT moveCount = sz / sizeof(DXGI_OUTDUPL_MOVE_RECT);
    for (UINT i = 0; i < moveCount; ++i) {
        for (const auto& r : regions) {
            if (IntersectRect(&hit, &moves[i].DestinationRect, &r)) {
                return true;
            }
        }
    }
    auto* dirty = reinterpret_cast<RECT*>(metaBuf.data());
    if (FAILED(dup->GetFrameDirtyRects(static_cast<UINT>(metaBuf.size()), dirty,
                                       &sz))) {
        return true;
    }
    UINT dirtyCount = sz / sizeof(RECT);
    for (UINT i = 0; i < dirtyCount; ++i) {
        for (const auto& r : regions) {
            if (IntersectRect(&hit, &dirty[i], &r)) {
                return true;
            }
        }
    }
    return false;
}

void PostGlassFrameWake() {
    HWND hwnd = GetGlassNotifyHwnd();
    if (hwnd && !g_liquidCapture.framePosted.exchange(true)) {
        if (!PostMessageW(hwnd, kWmAppGlassFrame, 0, 0)) {
            g_liquidCapture.framePosted = false;
        }
    }
}

DWORD WINAPI LiquidCaptureThreadProc(LPVOID param) {
    auto* holder = static_cast<std::shared_ptr<LiquidCaptureWorker>*>(param);
    auto w = *holder;
    delete holder;

    bool first = true;
    bool missed = false;
    std::vector<BYTE> metaBuf;
    try {
        while (!w->stop.load(std::memory_order_relaxed)) {
            DXGI_OUTDUPL_FRAME_INFO info{};
            winrt::com_ptr<IDXGIResource> res;
            HRESULT hr = w->duplication->AcquireNextFrame(60, &info, res.put());
            if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
                continue;
            }
            if (FAILED(hr)) {
                w->state = LiquidCaptureWorker::Lost;
                break;
            }
            struct FrameGuard {
                IDXGIOutputDuplication* dup;
                ~FrameGuard() { dup->ReleaseFrame(); }
            } guard{w->duplication.get()};

            bool keep =
                std::exchange(first, false) || std::exchange(missed, false) ||
                (info.LastPresentTime.QuadPart != 0 &&
                 DuplicationChangedNearWatched(w->duplication.get(),
                                               w->screenRect, info, metaBuf));
            if (!keep || w->stop.load(std::memory_order_relaxed)) {
                continue;
            }
            auto tex = res.try_as<ID3D11Texture2D>();
            if (!tex) {
                continue;
            }
            D3D11_TEXTURE2D_DESC srcDesc{}, dstDesc{};
            tex->GetDesc(&srcDesc);
            w->sharedTex->GetDesc(&dstDesc);
            if (srcDesc.Format != dstDesc.Format ||
                srcDesc.Width != dstDesc.Width ||
                srcDesc.Height != dstDesc.Height) {
                w->state = LiquidCaptureWorker::Lost;
                break;
            }

            HRESULT syncHr = w->sharedLock->AcquireSync(0, 50);
            if (syncHr == static_cast<HRESULT>(WAIT_TIMEOUT)) {
                missed = true;
                continue;
            }
            if (syncHr != S_OK) {
                w->state = LiquidCaptureWorker::Lost;
                break;
            }
            w->context->CopyResource(w->sharedTex.get(), tex.get());
            w->sharedLock->ReleaseSync(0);
            w->copied.fetch_add(1, std::memory_order_release);
            if (g_liquidCapture.watching.load(std::memory_order_relaxed)) {
                PostGlassFrameWake();
            }
        }
    } catch (...) {
        w->error = winrt::to_hresult();
        w->state = LiquidCaptureWorker::Failed;
    }
    w->sharedLock = nullptr;
    w->sharedTex = nullptr;
    w->duplication = nullptr;
    w->context = nullptr;
    w->device = nullptr;
    return 0;
}

void StopLiquidCaptureWorker() {
    auto& c = g_liquidCapture;
    if (c.worker) {
        c.worker->stop.store(true, std::memory_order_release);
    }
    if (c.workerThread) {
        WaitForSingleObject(c.workerThread, 1500);
        CloseHandle(c.workerThread);
        c.workerThread = nullptr;
    }
    c.worker.reset();
    c.openedSharedLock = nullptr;
    c.openedSharedTex = nullptr;
    c.takenCopies = 0;
}

void ShutdownLiquidCapture() {
    g_liquidCapture.watching = false;
    StopLiquidCaptureWorker();
    g_liquidCapture.desktopTex = nullptr;
    g_liquidCapture.frameId = 0;
    g_liquidCapture.excludedApplied = false;
    g_liquidCapture.frozen = false;
    g_liquidCapture.unsupported = false;
    g_liquidCapture.loggedFail = false;
    g_liquidCapture.consecutiveFailures = 0;
}

bool WantOpticalLiquidGlass() {
    return g_settings.glassRefraction != GlassRefraction::Off &&
           !g_liquidCapture.unsupported &&
           g_liquidCapture.consecutiveFailures < 5 && !g_quitting;
}

void ApplyWindowGlassCaptureAffinity(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return;
    }
    DWORD want = (WantOpticalLiquidGlass() && !g_liquidCapture.frozen)
                     ? kWdaExcludeFromCapture
                     : WDA_NONE;
    DWORD cur = WDA_NONE;
    if (!GetWindowDisplayAffinity(hwnd, &cur) || cur != want) {
        SetWindowDisplayAffinity(hwnd, want);
    }
}

bool StartLiquidCaptureForPrimaryMonitor() {
    auto& c = g_liquidCapture;
    if (!g_gfx.d3dDevice || !g_gfx.d3dContext) {
        return false;
    }
    HMONITOR mon = GetPrimaryMonitor();
    if (!mon) {
        return false;
    }
    c.monitor = mon;

    winrt::com_ptr<IDXGIDevice> dxgiDev;
    if (FAILED(g_gfx.d3dDevice->QueryInterface(IID_PPV_ARGS(dxgiDev.put())))) {
        return false;
    }
    winrt::com_ptr<IDXGIAdapter> adapter;
    if (FAILED(dxgiDev->GetAdapter(adapter.put())) || !adapter) {
        return false;
    }

    winrt::com_ptr<IDXGIOutput> output;
    for (UINT i = 0; SUCCEEDED(adapter->EnumOutputs(i, output.put()));
         ++i, output = nullptr) {
        DXGI_OUTPUT_DESC desc{};
        if (FAILED(output->GetDesc(&desc)) || desc.Monitor != mon) {
            continue;
        }
        auto output1 = output.try_as<IDXGIOutput1>();
        if (!output1) {
            c.unsupported = true;
            return false;
        }

        auto w = std::make_shared<LiquidCaptureWorker>();
        D3D_FEATURE_LEVEL levels[] = {
            D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0};
        D3D_FEATURE_LEVEL obtained{};
        HRESULT hr = D3D11CreateDevice(
            adapter.get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, 0, levels,
            ARRAYSIZE(levels), D3D11_SDK_VERSION, w->device.put(), &obtained,
            w->context.put());
        if (FAILED(hr)) {
            return false;
        }

        hr = output1->DuplicateOutput(w->device.get(), w->duplication.put());
        if (FAILED(hr)) {
            if (!std::exchange(c.loggedFail, true)) {
                Wh_Log(L"Liquid Glass DXGI DuplicateOutput failed (0x%08X), "
                       L"using fallback blur",
                       (unsigned)hr);
            }
            return false;
        }

        DXGI_OUTDUPL_DESC dupDesc{};
        w->duplication->GetDesc(&dupDesc);
        if (dupDesc.Rotation != DXGI_MODE_ROTATION_IDENTITY &&
            dupDesc.Rotation != DXGI_MODE_ROTATION_UNSPECIFIED) {
            c.unsupported = true;
            Wh_Log(L"Liquid Glass: rotated display not supported, using blur");
            return false;
        }

        c.screenRect = w->screenRect = desc.DesktopCoordinates;
        LONG monW = c.screenRect.right - c.screenRect.left;
        LONG monH = c.screenRect.bottom - c.screenRect.top;
        if (monW <= 0 || monH <= 0) {
            return false;
        }

        D3D11_TEXTURE2D_DESC td{};
        td.Width = static_cast<UINT>(monW);
        td.Height = static_cast<UINT>(monH);
        td.MipLevels = 1;
        td.ArraySize = 1;
        td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        td.SampleDesc = {1, 0};
        td.Usage = D3D11_USAGE_DEFAULT;
        td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        td.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
        if (FAILED(w->device->CreateTexture2D(&td, nullptr,
                                              w->sharedTex.put()))) {
            return false;
        }
        w->sharedLock = w->sharedTex.try_as<IDXGIKeyedMutex>();
        auto sharedRes = w->sharedTex.try_as<IDXGIResource>();
        HANDLE sharedHandle = nullptr;
        if (!w->sharedLock || !sharedRes ||
            FAILED(sharedRes->GetSharedHandle(&sharedHandle)) ||
            !sharedHandle) {
            return false;
        }

        winrt::com_ptr<ID3D11Texture2D> opened;
        if (FAILED(g_gfx.d3dDevice->OpenSharedResource(
                sharedHandle, __uuidof(ID3D11Texture2D), opened.put_void())) ||
            !opened) {
            return false;
        }
        auto openedLock = opened.try_as<IDXGIKeyedMutex>();
        if (!openedLock) {
            return false;
        }

        StopLiquidCaptureWorker();
        auto* arg = new std::shared_ptr<LiquidCaptureWorker>(w);
        HANDLE th = CreateThread(nullptr, 0, LiquidCaptureThreadProc, arg, 0,
                                 nullptr);
        if (!th) {
            delete arg;
            return false;
        }
        c.worker = w;
        c.workerThread = th;
        c.openedSharedTex = opened;
        c.openedSharedLock = openedLock;
        c.takenCopies = 0;
        c.loggedFail = false;
        Wh_Log(L"Liquid Glass DXGI capture active (%ldx%ld)", monW, monH);
        return true;
    }

    c.unsupported = true;
    return false;
}

bool EnsureLiquidCaptureReady() {
    auto& c = g_liquidCapture;
    if (!WantOpticalLiquidGlass()) {
        if (c.worker) {
            StopLiquidCaptureWorker();
        }
        if (c.excludedApplied) {
            ApplyCaptureExclusionToAllModWindows(false);
            c.excludedApplied = false;
        }
        return false;
    }
    if (c.frozen) {
        return c.desktopTex != nullptr;
    }
    ULONGLONG now = GetTickCount64();
    if (!c.excludedApplied) {
        ApplyCaptureExclusionToAllModWindows(true);
        c.excludedApplied = true;
        c.excludedAtMs = now;
        c.retryAtMs = now + 90;
        return false;
    }
    if (c.worker) {
        int st = c.worker->state.load(std::memory_order_acquire);
        if (st == LiquidCaptureWorker::Lost) {
            StopLiquidCaptureWorker();
            c.retryAtMs = now + 500;
            return c.desktopTex != nullptr;
        }
        if (st == LiquidCaptureWorker::Failed) {
            StopLiquidCaptureWorker();
            c.consecutiveFailures++;
            c.retryAtMs = now + 2000;
            return false;
        }
        uint64_t copied = c.worker->copied.load(std::memory_order_acquire);
        if (copied != c.takenCopies && c.openedSharedTex &&
            c.openedSharedLock && g_gfx.d3dDevice && g_gfx.d3dContext) {
            D3D11_TEXTURE2D_DESC desc{}, have{};
            c.openedSharedTex->GetDesc(&desc);
            if (c.desktopTex) {
                c.desktopTex->GetDesc(&have);
            }
            winrt::com_ptr<ID3D11Texture2D> target = c.desktopTex;
            if (!target || have.Width != desc.Width ||
                have.Height != desc.Height || have.Format != desc.Format) {
                desc.BindFlags = 0;
                desc.CPUAccessFlags = 0;
                desc.MiscFlags = 0;
                target = nullptr;
                if (FAILED(g_gfx.d3dDevice->CreateTexture2D(&desc, nullptr,
                                                            target.put()))) {
                    return false;
                }
            }
            {
                D2DMultithreadLock lock(g_gfx.d2dMultithread.get());
                if (c.openedSharedLock->AcquireSync(0, 0) == S_OK) {
                    g_gfx.d3dContext->CopyResource(target.get(),
                                                   c.openedSharedTex.get());
                    c.openedSharedLock->ReleaseSync(0);
                    c.desktopTex = target;
                    c.frameId++;
                    c.takenCopies = copied;
                }
            }
        }
        return c.desktopTex != nullptr;
    }

    if (now >= c.retryAtMs && now >= c.excludedAtMs + 80) {
        if (!StartLiquidCaptureForPrimaryMonitor()) {
            c.retryAtMs = now + 2000;
        }
    }
    return c.desktopTex != nullptr;
}

// ---- Reusable Optical Liquid Glass Plate ----

struct IconReflectionSpot {
    float localCenterX = 0.0f;
    float localCenterY = 0.0f;
    float radiusPx = 0.0f;
    D2D1_COLOR_F color{0.5f, 0.7f, 1.0f, 1.0f};
};

struct LiquidGlassPlate {
    wuc::CompositionDrawingSurface surface{nullptr};
    wuc::CompositionSurfaceBrush brush{nullptr};
    wuc::SpriteVisual visual{nullptr};
    SIZE surfaceSize{};

    winrt::com_ptr<ID3D11Texture2D> behindTex;
    winrt::com_ptr<ID2D1Bitmap1> behindBmp;
    winrt::com_ptr<ID2D1Bitmap1> mapBmp;
    SIZE mapCap{};
    std::vector<float> mapPixels;
    float mapScale = 1.0f;
    std::array<float, 11> mapKey{};

    // Cached D2D1 effect graph (zero per-frame CreateEffect churn!)
    ID2D1DeviceContext* fxDc = nullptr;
    winrt::com_ptr<ID2D1Effect> fxCrop;
    winrt::com_ptr<ID2D1Effect> fxBorder;
    winrt::com_ptr<ID2D1Effect> fxMapScale;
    winrt::com_ptr<ID2D1Effect> fxDisp;
    winrt::com_ptr<ID2D1Effect> fxBlur;
    winrt::com_ptr<ID2D1Effect> fxSat;
    winrt::com_ptr<ID2D1Effect> fxCeil1;
    winrt::com_ptr<ID2D1Effect> fxCeil2;

    std::array<float, 5> lastRect{};
    uint64_t lastFrameId = 0;
    uint32_t lastStyleSig = 0;
    ULONGLONG lastDrawTick = 0;
    bool dirty = true;
    bool active = false;

    void ClearEffects() {
        fxDc = nullptr;
        fxCrop = nullptr;
        fxBorder = nullptr;
        fxMapScale = nullptr;
        fxDisp = nullptr;
        fxBlur = nullptr;
        fxSat = nullptr;
        fxCeil1 = nullptr;
        fxCeil2 = nullptr;
    }

    void ResetGpuResources() {
        ClearEffects();
        behindBmp = nullptr;
        behindTex = nullptr;
        mapBmp = nullptr;
        mapCap = {};
        mapKey = {};
        surface = nullptr;
        surfaceSize = {};
        if (brush) {
            brush.Surface(nullptr);
        }
        dirty = true;
        active = false;
    }

    void Shutdown() {
        ResetGpuResources();
        visual = nullptr;
        brush = nullptr;
    }
};

inline LONG RoundUpAlign(LONG val, LONG step) {
    return ((val + step - 1) / step) * step;
}

float ComputeLiquidBlurSigmaPx(float scale, float blurMultiplier = 1.0f) {
    float v = static_cast<float>(std::clamp(g_settings.glassBlur, 0, 100)) /
              100.0f;
    if (v <= 0.001f) {
        return 0.0f;
    }
    // Authentic macOS 27 Clear/Dark optical blur curve:
    // at default 36 -> ~4.4 * scale (silky crystal translucency);
    // at 60 -> ~8.1 * scale; at 100 -> ~15.0 * scale.
    return std::pow(v, 1.20f) * 15.0f * scale * blurMultiplier;
}

float ComputeRimShiftCurve(float u) {
    switch (g_settings.glassRefraction) {
        case GlassRefraction::Reflection: {
            constexpr float kTurn = 0.25f;
            return u < kTurn ? (u * u / kTurn) : (2.0f * u - kTurn);
        }
        case GlassRefraction::Stretch:
            return 0.5f * u * u;
        case GlassRefraction::Lens:
        default:
            // Smooth convex liquid-glass lens curve (macOS 27).
            return 2.0f * u * u;
    }
}

float BuildLiquidGlassMap(std::vector<float>& pixels,
                          int w,
                          int h,
                          float sx,
                          float sy,
                          float sw,
                          float sh,
                          float radius,
                          float scale,
                          float rimDepthMul = 1.0f,
                          float strengthMul = 1.0f) {
    float depthPx =
        static_cast<float>(g_settings.glassEdgeDepth) * scale * rimDepthMul;
    float strength =
        (static_cast<float>(g_settings.glassEdgeStrength) / 100.0f) *
        strengthMul;
    float dispScale = std::max(4.0f * depthPx * std::max(strength, 0.2f), 1.0f);

    pixels.assign(static_cast<size_t>(w) * static_cast<size_t>(h) * 4, 0.5f);
    for (size_t i = 3; i < pixels.size(); i += 4) {
        pixels[i] = 1.0f;
    }

    float halfW = sw * 0.5f;
    float halfH = sh * 0.5f;
    float cx = sx + halfW;
    float cy = sy + halfH;
    radius = std::clamp(radius, 0.0f, std::min(halfW, halfH));
    float rim = std::min(depthPx, std::min(halfW, halfH) * 0.48f);
    if (rim <= 0.5f || strength <= 0.001f) {
        return dispScale;
    }

    float roundCorner = std::max(radius, rim);

    auto bendPixel = [&](int x, int y) {
        float px = (static_cast<float>(x) + 0.5f) - cx;
        float py = (static_cast<float>(y) + 0.5f) - cy;
        float ax = std::fabs(px);
        float ay = std::fabs(py);

        float qx = ax - (halfW - radius);
        float qy = ay - (halfH - radius);
        float ox = std::max(qx, 0.0f);
        float oy = std::max(qy, 0.0f);
        float depth = radius - std::min(std::max(qx, qy), 0.0f) -
                      std::sqrt(ox * ox + oy * oy);
        if (depth < 0.0f || depth >= rim) {
            return;
        }
        float nx = std::max(ax - (halfW - roundCorner), 0.0f);
        float ny = std::max(ay - (halfH - roundCorner), 0.0f);
        float len = std::sqrt(nx * nx + ny * ny);
        if (len <= 1e-5f) {
            return;
        }
        float u = 1.0f - depth / rim;
        float off = rim * ComputeRimShiftCurve(u) * strength / dispScale / len;
        float* p = &pixels[(static_cast<size_t>(y) * static_cast<size_t>(w) +
                            static_cast<size_t>(x)) *
                           4];
        p[0] = std::clamp(0.5f - std::copysign(nx, px) * off, 0.0f, 1.0f);
        p[1] = std::clamp(0.5f - std::copysign(ny, py) * off, 0.0f, 1.0f);
    };

    int x0 = std::max(0, static_cast<int>(std::floor(sx)));
    int x1 = std::min(w, static_cast<int>(std::ceil(sx + sw)));
    int y0 = std::max(0, static_cast<int>(std::floor(sy)));
    int y1 = std::min(h, static_cast<int>(std::ceil(sy + sh)));

    float ends = halfW - radius - rim;
    int innerLeft =
        std::clamp(static_cast<int>(std::floor(cx - ends - 0.5f)) + 1, x0, x1);
    int innerRight =
        std::clamp(static_cast<int>(std::ceil(cx + ends - 0.5f)), innerLeft, x1);

    for (int y = y0; y < y1; ++y) {
        bool inBandY = std::fabs((static_cast<float>(y) + 0.5f) - cy) >=
                       (halfH - rim);
        for (int x = x0; x < x1; ++x) {
            if (!inBandY && x == innerLeft) {
                x = innerRight;
            }
            if (x >= x1) {
                break;
            }
            bendPixel(x, y);
        }
    }
    return dispScale;
}

template <typename T>
void SetD2DEffectVal(ID2D1Effect* fx, UINT32 prop, const T& val) {
    winrt::check_hresult(fx->SetValue(
        prop, D2D1_PROPERTY_TYPE_UNKNOWN,
        reinterpret_cast<const BYTE*>(&val), sizeof(val)));
}

winrt::com_ptr<ID2D1Effect> MakeD2DEffect(ID2D1DeviceContext* dc,
                                          const GUID& clsid,
                                          ID2D1Image* input) {
    winrt::com_ptr<ID2D1Effect> fx;
    winrt::check_hresult(dc->CreateEffect(clsid, fx.put()));
    fx->SetInput(0, input, TRUE);
    return fx;
}

winrt::com_ptr<ID2D1Image> GetD2DEffectOut(ID2D1Effect* fx) {
    winrt::com_ptr<ID2D1Image> img;
    fx->GetOutput(img.put());
    return img;
}

struct LiquidGlassStyleParams {
    float screenX = 0.0f;
    float screenY = 0.0f;
    float widthPx = 0.0f;
    float heightPx = 0.0f;
    float cornerRadiusPx = 0.0f;
    float scale = 1.0f;
    bool darkMode = true;
    bool useSquircle = true;
    float tintBoost = 0.0f;       // +0.15 for tooltips, +0.26 for menus
    float blurMul = 1.0f;
    float rimDepthMul = 1.0f;
    float strengthMul = 1.0f;
    bool fullWidthBar = false;    // top Finder Bar styling
    const std::vector<IconReflectionSpot>* reflections = nullptr;
};

void DrawMacOS27GlassLighting(ID2D1DeviceContext* dc,
                              const LiquidGlassStyleParams& p,
                              ID2D1Geometry* clipGeom) {
    const float w = p.widthPx;
    const float h = p.heightPx;
    const float s = p.scale;
    const bool dark = p.darkMode;
    const float tintFrac =
        static_cast<float>(std::clamp(g_settings.glassTint, 0, 100)) / 100.0f;

    auto makeGrad = [&](D2D1_POINT_2F a, D2D1_POINT_2F b,
                        const D2D1_GRADIENT_STOP* stops, UINT32 count) {
        winrt::com_ptr<ID2D1GradientStopCollection> coll;
        winrt::com_ptr<ID2D1LinearGradientBrush> br;
        if (SUCCEEDED(dc->CreateGradientStopCollection(stops, count,
                                                       coll.put()))) {
            dc->CreateLinearGradientBrush(
                D2D1::LinearGradientBrushProperties(a, b), coll.get(),
                br.put());
        }
        return br;
    };

    auto fillShape = [&](ID2D1Brush* br) {
        if (!br) {
            return;
        }
        if (clipGeom) {
            dc->FillGeometry(clipGeom, br);
        } else {
            dc->FillRectangle(D2D1::RectF(0.0f, 0.0f, w, h), br);
        }
    };

    // 1. Base macOS 27 Clear/Dark Crystal Glass vertical tint (ultra-clean, zero milky fog)
    float baseR = dark ? 0.038f : 0.985f;
    float baseG = dark ? 0.052f : 0.988f;
    float baseB = dark ? 0.082f : 1.000f;
    const bool tintedWash =
        (g_settings.glassStyle == GlassStyle::Tinted);
    if (tintedWash) {
        auto acc = GetAccentColor();
        float ar = acc.R / 255.0f;
        float ag = acc.G / 255.0f;
        float ab = acc.B / 255.0f;
        baseR = Lerp(baseR, ar, dark ? 0.30f : 0.20f);
        baseG = Lerp(baseG, ag, dark ? 0.30f : 0.20f);
        baseB = Lerp(baseB, ab, dark ? 0.30f : 0.20f);
    }

    float baseAlpha = dark ? (0.10f + 0.44f * tintFrac + p.tintBoost)
                           : (0.14f + 0.38f * tintFrac + p.tintBoost);
    baseAlpha = std::clamp(baseAlpha, 0.04f, 0.84f);

    if (dark) {
        D2D1_GRADIENT_STOP stops[3] = {
            {0.0f, D2D1::ColorF(baseR * 1.15f, baseG * 1.20f, baseB * 1.30f,
                                baseAlpha * 0.78f)},
            {0.55f, D2D1::ColorF(baseR * 0.75f, baseG * 0.82f, baseB * 0.95f,
                                 baseAlpha)},
            {1.0f, D2D1::ColorF(baseR * 0.95f, baseG * 1.05f, baseB * 1.20f,
                                baseAlpha * 0.94f)},
        };
        if (auto br = makeGrad(D2D1::Point2F(0, 0), D2D1::Point2F(0, h), stops,
                               3)) {
            fillShape(br.get());
        }
    } else {
        D2D1_GRADIENT_STOP stops[3] = {
            {0.0f, D2D1::ColorF(baseR, baseG, baseB,
                                std::min(0.92f, baseAlpha * 1.15f))},
            {0.55f, D2D1::ColorF(baseR * 0.98f, baseG * 0.98f, baseB,
                                 baseAlpha * 0.90f)},
            {1.0f, D2D1::ColorF(baseR * 0.95f, baseG * 0.96f, baseB,
                                baseAlpha * 0.82f)},
        };
        if (auto br = makeGrad(D2D1::Point2F(0, 0), D2D1::Point2F(0, h), stops,
                               3)) {
            fillShape(br.get());
        }
    }

    // 2. Optional soft icon reflection pools on Dock glass (off by default for Clear/Dark purity)
    if (p.reflections && !p.reflections->empty()) {
        float reflAlpha = dark ? 0.12f : 0.08f;
        for (const auto& sp : *p.reflections) {
            if (sp.radiusPx <= 2.0f) {
                continue;
            }
            D2D1_GRADIENT_STOP rStops[2] = {
                {0.0f, D2D1::ColorF(sp.color.r, sp.color.g, sp.color.b,
                                    reflAlpha)},
                {1.0f, D2D1::ColorF(sp.color.r, sp.color.g, sp.color.b, 0.0f)},
            };
            winrt::com_ptr<ID2D1GradientStopCollection> coll;
            winrt::com_ptr<ID2D1RadialGradientBrush> rbr;
            if (SUCCEEDED(dc->CreateGradientStopCollection(rStops, 2,
                                                           coll.put())) &&
                SUCCEEDED(dc->CreateRadialGradientBrush(
                    D2D1::RadialGradientBrushProperties(
                        D2D1::Point2F(sp.localCenterX, sp.localCenterY),
                        D2D1::Point2F(0.0f, 0.0f), sp.radiusPx * 1.15f,
                        sp.radiusPx * 0.55f),
                    coll.get(), rbr.put()))) {
                dc->FillEllipse(D2D1::Ellipse(
                                    D2D1::Point2F(sp.localCenterX,
                                                  sp.localCenterY),
                                    sp.radiusPx * 1.15f, sp.radiusPx * 0.55f),
                                rbr.get());
            }
        }
    }

    // 3. Whisper-thin upper specular catchlight (crystal clear, zero cloudiness)
    {
        float sheenH = std::max(2.0f, std::min(h * 0.20f, 15.0f * s));
        D2D1_GRADIENT_STOP sheen[2] = {
            {0.0f, D2D1::ColorF(0.92f, 0.96f, 1.0f, dark ? 0.065f : 0.24f)},
            {1.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f)},
        };
        if (auto br = makeGrad(D2D1::Point2F(0, 0), D2D1::Point2F(0, sheenH),
                               sheen, 2)) {
            fillShape(br.get());
        }
    }

    // 4. Delicate lower cool caustic reflection
    {
        float bounceH = std::max(2.0f, std::min(h * 0.16f, 12.0f * s));
        D2D1_GRADIENT_STOP bounce[2] = {
            {0.0f, D2D1::ColorF(0.76f, 0.88f, 1.0f, 0.0f)},
            {1.0f, D2D1::ColorF(0.76f, 0.88f, 1.0f, dark ? 0.055f : 0.12f)},
        };
        if (auto br = makeGrad(D2D1::Point2F(0, h - bounceH),
                               D2D1::Point2F(0, h), bounce, 2)) {
            fillShape(br.get());
        }
    }

    if (p.fullWidthBar) {
        winrt::com_ptr<ID2D1SolidColorBrush> lineBr;
        if (SUCCEEDED(dc->CreateSolidColorBrush(
                D2D1::ColorF(0.86f, 0.93f, 1.0f, dark ? 0.24f : 0.45f),
                lineBr.put()))) {
            dc->FillRectangle(
                D2D1::RectF(0.0f, h - 2.0f * s, w, h - 1.0f * s), lineBr.get());
        }
        if (SUCCEEDED(dc->CreateSolidColorBrush(
                D2D1::ColorF(0.0f, 0.0f, 0.0f, dark ? 0.38f : 0.12f),
                lineBr.put()))) {
            dc->FillRectangle(D2D1::RectF(0.0f, h - 1.0f * s, w, h),
                              lineBr.get());
        }
        return;
    }

    auto makeInsetGeom = [&](float inset) -> winrt::com_ptr<ID2D1PathGeometry> {
        float iw = std::max(1.0f, w - 2.0f * inset);
        float ih = std::max(1.0f, h - 2.0f * inset);
        float ir = std::max(0.5f, p.cornerRadiusPx - inset * 0.65f);
        return BuildContinuousSquirclePath(inset, inset, iw, ih, ir);
    };

    // 5. Crisp 1px outer dark contour separating glass cleanly from wallpaper
    if (auto outerGeom = makeInsetGeom(0.5f)) {
        winrt::com_ptr<ID2D1SolidColorBrush> darkEdge;
        if (SUCCEEDED(dc->CreateSolidColorBrush(
                D2D1::ColorF(0.0f, 0.0f, 0.0f, dark ? 0.45f : 0.14f),
                darkEdge.put()))) {
            dc->DrawGeometry(outerGeom.get(), darkEdge.get(), 1.0f);
        }
    }

    // 6. Signature macOS 27 Clear/Dark Razor-Sharp Continuous Silvery Crystal Rim
    float rimW = std::max(1.0f, 1.15f * s);
    float rimInset = 0.9f + rimW * 0.5f;
    if (auto rimGeom = makeInsetGeom(rimInset)) {
        float rimR = 0.86f, rimG = 0.92f, rimB = 1.0f;
        if (tintedWash) {
            auto acc = GetAccentColor();
            rimR = Lerp(rimR, acc.R / 255.0f, 0.45f);
            rimG = Lerp(rimG, acc.G / 255.0f, 0.45f);
            rimB = Lerp(rimB, acc.B / 255.0f, 0.45f);
        }
        // Unbroken delicate silvery-white (or Tahoe tinted) glass outline all the way around
        winrt::com_ptr<ID2D1SolidColorBrush> baseRim;
        if (SUCCEEDED(dc->CreateSolidColorBrush(
                D2D1::ColorF(rimR, rimG, rimB, dark ? 0.30f : 0.48f),
                baseRim.put()))) {
            dc->DrawGeometry(rimGeom.get(), baseRim.get(), rimW);
        }
        // Diagonal Fresnel corner specular boost (bright curved ends)
        D2D1_GRADIENT_STOP rStops[5] = {
            {0.00f, D2D1::ColorF(1.00f, 1.00f, 1.00f, dark ? 0.54f : 0.80f)},
            {0.16f, D2D1::ColorF(0.92f, 0.96f, 1.00f, dark ? 0.16f : 0.26f)},
            {0.50f, D2D1::ColorF(0.86f, 0.92f, 1.00f, 0.02f)},
            {0.84f, D2D1::ColorF(0.86f, 0.94f, 1.00f, dark ? 0.18f : 0.26f)},
            {1.00f, D2D1::ColorF(0.90f, 0.96f, 1.00f, dark ? 0.44f : 0.64f)},
        };
        if (auto rbr = makeGrad(D2D1::Point2F(0.0f, 0.0f), D2D1::Point2F(w, h),
                                rStops, 5)) {
            dc->DrawGeometry(rimGeom.get(), rbr.get(), rimW);
        }
        // Crisp top & bottom specular gleam
        D2D1_GRADIENT_STOP vStops[4] = {
            {0.00f, D2D1::ColorF(1.0f, 1.0f, 1.0f, dark ? 0.24f : 0.40f)},
            {0.18f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f)},
            {0.82f, D2D1::ColorF(0.84f, 0.92f, 1.0f, 0.0f)},
            {1.00f, D2D1::ColorF(0.84f, 0.92f, 1.0f, dark ? 0.20f : 0.28f)},
        };
        if (auto vbr = makeGrad(D2D1::Point2F(0.0f, 0.0f),
                                D2D1::Point2F(0.0f, h), vStops, 4)) {
            dc->DrawGeometry(rimGeom.get(), vbr.get(), rimW * 0.9f);
        }
    }
}

uint32_t ComputeGlassStyleSignature(const LiquidGlassStyleParams& p) {
    uint32_t h = 2166136261u;
    auto mix = [&](uint32_t v) {
        h ^= v;
        h *= 16777619u;
    };
    mix(static_cast<uint32_t>(g_settings.glassStyle));
    mix(static_cast<uint32_t>(g_settings.glassRefraction));
    mix(static_cast<uint32_t>(g_settings.glassEdgeDepth));
    mix(static_cast<uint32_t>(g_settings.glassEdgeStrength));
    mix(static_cast<uint32_t>(g_settings.glassTint));
    mix(static_cast<uint32_t>(g_settings.glassBlur));
    mix(static_cast<uint32_t>(g_settings.glassBrightnessLimit));
    mix(p.darkMode ? 1u : 0u);
    mix(static_cast<uint32_t>(std::lround(p.tintBoost * 100.0f)));
    mix(p.fullWidthBar ? 1u : 0u);
    if (p.reflections) {
        mix(static_cast<uint32_t>(p.reflections->size()));
        for (const auto& sp : *p.reflections) {
            mix(static_cast<uint32_t>(std::lround(sp.localCenterX)));
        }
    }
    return h;
}

bool RenderLiquidGlassPlate(LiquidGlassPlate& plate,
                            const LiquidGlassStyleParams& p) {
    if (!g_gfx.compositor || !g_gfx.compGraphics || !g_gfx.d3dDevice ||
        !g_gfx.d3dContext || !g_liquidCapture.desktopTex ||
        p.widthPx < 2.0f || p.heightPx < 2.0f) {
        return false;
    }

    uint32_t styleSig = ComputeGlassStyleSignature(p);
    std::array<float, 5> curRect{p.screenX, p.screenY, p.widthPx, p.heightPx,
                                 p.cornerRadiusPx};
    if (!plate.dirty && plate.active && curRect == plate.lastRect &&
        plate.lastFrameId == g_liquidCapture.frameId &&
        plate.lastStyleSig == styleSig) {
        return true;
    }

    float blurSigma = ComputeLiquidBlurSigmaPx(p.scale, p.blurMul);
    int pad = static_cast<int>(std::ceil(3.0f * blurSigma)) + 8;
    LONG left = static_cast<LONG>(std::floor(p.screenX));
    LONG top = static_cast<LONG>(std::floor(p.screenY));
    float fx = p.screenX - static_cast<float>(left);
    float fy = p.screenY - static_cast<float>(top);
    SIZE surfSz{std::max(1L, static_cast<LONG>(std::ceil(p.widthPx))),
                std::max(1L, static_cast<LONG>(std::ceil(p.heightPx)))};
    SIZE areaSz{surfSz.cx + 1 + 2 * pad, surfSz.cy + 1 + 2 * pad};
    RECT around{left - pad, top - pad, left - pad + areaSz.cx,
                top - pad + areaSz.cy};
    RECT valid{};
    if (!IntersectRect(&valid, &around, &g_liquidCapture.screenRect)) {
        return false;
    }

    D3D11_TEXTURE2D_DESC deskDesc{}, haveDesc{};
    g_liquidCapture.desktopTex->GetDesc(&deskDesc);
    if (plate.behindTex) {
        plate.behindTex->GetDesc(&haveDesc);
    }
    if (!plate.behindTex || haveDesc.Format != deskDesc.Format ||
        static_cast<LONG>(haveDesc.Width) < areaSz.cx ||
        static_cast<LONG>(haveDesc.Height) < areaSz.cy) {
        D3D11_TEXTURE2D_DESC td{};
        td.Width = static_cast<UINT>(RoundUpAlign(areaSz.cx, 256));
        td.Height = static_cast<UINT>(RoundUpAlign(areaSz.cy, 64));
        td.MipLevels = 1;
        td.ArraySize = 1;
        td.Format = deskDesc.Format;
        td.SampleDesc.Count = 1;
        td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        plate.behindTex = nullptr;
        plate.behindBmp = nullptr;
        if (FAILED(g_gfx.d3dDevice->CreateTexture2D(&td, nullptr,
                                                    plate.behindTex.put()))) {
            return false;
        }
    }

    {
        D2DMultithreadLock lock(g_gfx.d2dMultithread.get());
        D3D11_BOX box{
            static_cast<UINT>(valid.left - g_liquidCapture.screenRect.left),
            static_cast<UINT>(valid.top - g_liquidCapture.screenRect.top),
            0,
            static_cast<UINT>(valid.right - g_liquidCapture.screenRect.left),
            static_cast<UINT>(valid.bottom - g_liquidCapture.screenRect.top),
            1};
        g_gfx.d3dContext->CopySubresourceRegion(
            plate.behindTex.get(), 0,
            static_cast<UINT>(valid.left - around.left),
            static_cast<UINT>(valid.top - around.top), 0,
            g_liquidCapture.desktopTex.get(), 0, &box);
    }
    OffsetRect(&valid, -around.left, -around.top);

    // Ensure CompositionDrawingSurface & SpriteVisual
    if (!plate.surface) {
        plate.surface = CreateDrawingSurface(static_cast<float>(surfSz.cx),
                                             static_cast<float>(surfSz.cy));
        if (!plate.surface) {
            return false;
        }
        plate.surfaceSize = surfSz;
    } else if (plate.surfaceSize.cx != surfSz.cx ||
               plate.surfaceSize.cy != surfSz.cy) {
        auto interop = plate.surface.as<ICompositionDrawingSurfaceInteropLocal>();
        if (FAILED(interop->Resize(surfSz))) {
            plate.surface = CreateDrawingSurface(static_cast<float>(surfSz.cx),
                                                 static_cast<float>(surfSz.cy));
            if (!plate.surface) {
                return false;
            }
        }
        plate.surfaceSize = surfSz;
    }
    if (!plate.brush) {
        plate.brush = g_gfx.compositor.CreateSurfaceBrush(plate.surface);
        plate.brush.Stretch(wuc::CompositionStretch::Fill);
    } else {
        plate.brush.Surface(plate.surface);
    }
    if (!plate.visual) {
        plate.visual = g_gfx.compositor.CreateSpriteVisual();
        plate.visual.Brush(plate.brush);
    }
    plate.visual.Size({p.widthPx, p.heightPx});

    auto interop = plate.surface.as<ICompositionDrawingSurfaceInteropLocal>();
    winrt::com_ptr<ID2D1DeviceContext> dc;
    POINT offset{};
    if (FAILED(interop->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext),
                                  dc.put_void(), &offset)) ||
        !dc) {
        return false;
    }

    bool ok = false;
    try {
        dc->SetDpi(96.0f, 96.0f);
        if (!plate.behindBmp) {
            D2D1_BITMAP_PROPERTIES1 bp{
                {deskDesc.Format, D2D1_ALPHA_MODE_IGNORE},
                96.0f,
                96.0f,
                D2D1_BITMAP_OPTIONS_NONE,
                nullptr};
            winrt::check_hresult(dc->CreateBitmapFromDxgiSurface(
                plate.behindTex.as<IDXGISurface>().get(), &bp,
                plate.behindBmp.put()));
        }

        // Rebuild half-res 32-bit float refraction map only when geometry/settings change
        // (4x less CPU & GPU upload with identical smooth optical lens quality!)
        int mapW = std::max(2, static_cast<int>((areaSz.cx + 1) / 2));
        int mapH = std::max(2, static_cast<int>((areaSz.cy + 1) / 2));
        float invScaleX = static_cast<float>(mapW) / static_cast<float>(areaSz.cx);
        float invScaleY = static_cast<float>(mapH) / static_cast<float>(areaSz.cy);

        std::array<float, 11> mKey{
            static_cast<float>(mapW),
            static_cast<float>(mapH),
            (static_cast<float>(pad) + fx) * invScaleX,
            (static_cast<float>(pad) + fy) * invScaleY,
            p.widthPx * invScaleX,
            p.heightPx * invScaleY,
            p.cornerRadiusPx * invScaleX,
            p.scale * invScaleX,
            static_cast<float>(g_settings.glassRefraction),
            static_cast<float>(g_settings.glassEdgeDepth) * p.rimDepthMul,
            static_cast<float>(g_settings.glassEdgeStrength) * p.strengthMul};

        if (!plate.mapBmp || mKey != plate.mapKey) {
            plate.mapScale = BuildLiquidGlassMap(
                plate.mapPixels, mapW, mapH, mKey[2], mKey[3], mKey[4], mKey[5],
                mKey[6], mKey[7], p.rimDepthMul, p.strengthMul) /
                             std::max(invScaleX, 0.25f);
            if (!plate.mapBmp || plate.mapCap.cx < mapW ||
                plate.mapCap.cy < mapH) {
                plate.mapBmp = nullptr;
                plate.mapCap = {RoundUpAlign(mapW, 128),
                                RoundUpAlign(mapH, 64)};
                D2D1_BITMAP_PROPERTIES1 mp{
                    {DXGI_FORMAT_R32G32B32A32_FLOAT,
                     D2D1_ALPHA_MODE_PREMULTIPLIED},
                    96.0f,
                    96.0f,
                    D2D1_BITMAP_OPTIONS_NONE,
                    nullptr};
                winrt::check_hresult(dc->CreateBitmap(
                    D2D1::SizeU(static_cast<UINT32>(plate.mapCap.cx),
                                static_cast<UINT32>(plate.mapCap.cy)),
                    nullptr, 0, &mp, plate.mapBmp.put()));
            }
            D2D1_RECT_U dstU{0, 0, static_cast<UINT32>(mapW),
                             static_cast<UINT32>(mapH)};
            winrt::check_hresult(plate.mapBmp->CopyFromMemory(
                &dstU, plate.mapPixels.data(),
                static_cast<UINT32>(mapW) * 16u));
            plate.mapKey = mKey;
        }

        // Reuse cached D2D1 effects bound to this DeviceContext (zero per-frame CreateEffect churn!)
        if (plate.fxDc != dc.get() || !plate.fxCrop) {
            plate.ClearEffects();
            plate.fxDc = dc.get();
            winrt::check_hresult(
                dc->CreateEffect(kClsidD2D1Crop, plate.fxCrop.put()));
            winrt::check_hresult(
                dc->CreateEffect(kClsidD2D1Border, plate.fxBorder.put()));
            winrt::check_hresult(dc->CreateEffect(kClsidD2D12DAffineTransform,
                                                  plate.fxMapScale.put()));
            winrt::check_hresult(
                dc->CreateEffect(kClsidD2D1DisplacementMap, plate.fxDisp.put()));
            winrt::check_hresult(
                dc->CreateEffect(kGaussianBlurEffectId, plate.fxBlur.put()));
            winrt::check_hresult(
                dc->CreateEffect(kClsidD2D1ColorMatrix, plate.fxSat.put()));
            winrt::check_hresult(
                dc->CreateEffect(kClsidD2D1ColorMatrix, plate.fxCeil1.put()));
            winrt::check_hresult(
                dc->CreateEffect(kClsidD2D1ColorMatrix, plate.fxCeil2.put()));

            SetD2DEffectVal(plate.fxCrop.get(), 1, static_cast<UINT32>(1));
            SetD2DEffectVal(plate.fxMapScale.get(),
                            D2D1_2DAFFINETRANSFORM_PROP_INTERPOLATION_MODE,
                            D2D1_2DAFFINETRANSFORM_INTERPOLATION_MODE_LINEAR);
            constexpr UINT32 kPropPrecision = 0x80000007u;
            constexpr UINT32 kPrecision32Float = 5u;
            SetD2DEffectVal(plate.fxDisp.get(), kPropPrecision,
                            kPrecision32Float);
            SetD2DEffectVal(plate.fxDisp.get(), 1, static_cast<UINT32>(0));
            SetD2DEffectVal(plate.fxDisp.get(), 2, static_cast<UINT32>(1));
            SetD2DEffectVal(plate.fxBlur.get(), 1, static_cast<UINT32>(1));
            SetD2DEffectVal(plate.fxBlur.get(), 2, static_cast<UINT32>(1));
        }

        // 1. Crop valid on-screen pixels + Border clamp
        plate.fxCrop->SetInput(0, plate.behindBmp.get(), TRUE);
        D2D1_VECTOR_4F cropRect{static_cast<float>(valid.left),
                                static_cast<float>(valid.top),
                                static_cast<float>(valid.right),
                                static_cast<float>(valid.bottom)};
        SetD2DEffectVal(plate.fxCrop.get(), 0, cropRect);
        plate.fxBorder->SetInputEffect(0, plate.fxCrop.get(), TRUE);
        auto glassImg = GetD2DEffectOut(plate.fxBorder.get());

        // 2. Half-res 32-bit float optical refraction scaled smoothly 2x
        if (g_settings.glassEdgeStrength > 0) {
            plate.fxMapScale->SetInput(0, plate.mapBmp.get(), TRUE);
            D2D1::Matrix3x2F upMat = D2D1::Matrix3x2F::Scale(
                static_cast<float>(areaSz.cx) / static_cast<float>(mapW),
                static_cast<float>(areaSz.cy) / static_cast<float>(mapH));
            SetD2DEffectVal(plate.fxMapScale.get(),
                            D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, upMat);
            plate.fxDisp->SetInput(0, glassImg.get(), TRUE);
            plate.fxDisp->SetInputEffect(1, plate.fxMapScale.get(), TRUE);
            SetD2DEffectVal(plate.fxDisp.get(), 0, plate.mapScale);
            glassImg = GetD2DEffectOut(plate.fxDisp.get());
        }

        // 3. Silky optical blur
        if (blurSigma > 0.15f) {
            plate.fxBlur->SetInput(0, glassImg.get(), TRUE);
            SetD2DEffectVal(plate.fxBlur.get(), 0, blurSigma);
            glassImg = GetD2DEffectOut(plate.fxBlur.get());
        }

        // 4. Color vibrancy & light transmission through glass
        {
            plate.fxSat->SetInput(0, glassImg.get(), TRUE);
            float sat = p.darkMode ? 1.20f : 1.14f;
            float gain = p.darkMode ? 1.02f : 1.04f;
            const float lw[3] = {0.2126f, 0.7152f, 0.0722f};
            D2D1_MATRIX_5X4_F cm{};
            for (int r = 0; r < 3; ++r) {
                for (int cIdx = 0; cIdx < 3; ++cIdx) {
                    cm.m[r][cIdx] =
                        ((1.0f - sat) * lw[r] + (r == cIdx ? sat : 0.0f)) *
                        gain;
                }
            }
            cm.m[3][3] = 1.0f;
            cm.m[4][0] = cm.m[4][1] = cm.m[4][2] = p.darkMode ? 0.004f : 0.012f;
            SetD2DEffectVal(plate.fxSat.get(), 0, cm);
            SetD2DEffectVal(plate.fxSat.get(), 2, static_cast<BOOL>(TRUE));
            glassImg = GetD2DEffectOut(plate.fxSat.get());
        }

        // 5. Dark Mode per-pixel luminance ceiling
        if (p.darkMode && g_settings.glassBrightnessLimit < 100) {
            float ceiling = std::clamp(
                static_cast<float>(g_settings.glassBrightnessLimit) / 100.0f,
                0.30f, 0.99f);
            D2D1_MATRIX_5X4_F m1{}, m2{};
            m1.m[0][0] = m1.m[1][1] = m1.m[2][2] = 1.0f / ceiling;
            m1.m[3][3] = 1.0f;
            m2.m[0][0] = m2.m[1][1] = m2.m[2][2] = ceiling;
            m2.m[3][3] = 1.0f;
            plate.fxCeil1->SetInput(0, glassImg.get(), TRUE);
            SetD2DEffectVal(plate.fxCeil1.get(), 0, m1);
            SetD2DEffectVal(plate.fxCeil1.get(), 2, static_cast<BOOL>(TRUE));
            plate.fxCeil2->SetInputEffect(0, plate.fxCeil1.get(), TRUE);
            SetD2DEffectVal(plate.fxCeil2.get(), 0, m2);
            SetD2DEffectVal(plate.fxCeil2.get(), 2, static_cast<BOOL>(FALSE));
            glassImg = GetD2DEffectOut(plate.fxCeil2.get());
        }

        dc->SetTransform(D2D1::Matrix3x2F::Translation(
            static_cast<float>(offset.x), static_cast<float>(offset.y)));
        D2D1_RECT_F bounds =
            D2D1::RectF(0.0f, 0.0f, static_cast<float>(surfSz.cx),
                        static_cast<float>(surfSz.cy));
        dc->PushAxisAlignedClip(&bounds, D2D1_ANTIALIAS_MODE_ALIASED);
        dc->Clear(D2D1::ColorF(0, 0, 0, 0));

        winrt::com_ptr<ID2D1PathGeometry> squircleClip;
        bool pushedLayer = false;
        if (p.useSquircle && !p.fullWidthBar && p.cornerRadiusPx > 0.5f) {
            squircleClip = BuildContinuousSquirclePath(
                0.0f, 0.0f, p.widthPx, p.heightPx, p.cornerRadiusPx);
            if (squircleClip) {
                dc->PushLayer(D2D1::LayerParameters(
                                  bounds, squircleClip.get(),
                                  D2D1_ANTIALIAS_MODE_PER_PRIMITIVE),
                              nullptr);
                pushedLayer = true;
            }
        }

        D2D1_RECT_F srcRect{static_cast<float>(pad) + fx,
                            static_cast<float>(pad) + fy,
                            static_cast<float>(pad) + fx + p.widthPx,
                            static_cast<float>(pad) + fy + p.heightPx};
        dc->DrawImage(glassImg.get(), nullptr, &srcRect,
                      D2D1_INTERPOLATION_MODE_LINEAR,
                      D2D1_COMPOSITE_MODE_SOURCE_COPY);

        DrawMacOS27GlassLighting(dc.get(), p, squircleClip.get());

        if (pushedLayer) {
            dc->PopLayer();
        }
        dc->PopAxisAlignedClip();
        ok = true;
    } catch (...) {
        ok = false;
    }

    HRESULT endHr = interop->EndDraw();
    if (!ok || FAILED(endHr)) {
        return false;
    }
    plate.lastRect = curRect;
    plate.lastFrameId = g_liquidCapture.frameId;
    plate.lastStyleSig = styleSig;
    plate.lastDrawTick = GetTickCount64();
    plate.dirty = false;
    plate.active = true;
    return true;
}

std::wstring g_resolvedFontFamily;

const std::wstring& ResolveFontFamily() {
    if (!g_resolvedFontFamily.empty()) {
        return g_resolvedFontFamily;
    }
    g_resolvedFontFamily = L"Segoe UI Variable Text";
    if (!g_gfx.dwriteFactory) {
        return g_resolvedFontFamily;
    }
    winrt::com_ptr<IDWriteFontCollection> coll;
    if (SUCCEEDED(g_gfx.dwriteFactory->GetSystemFontCollection(coll.put()))) {
        UINT32 idx = 0;
        BOOL exists = FALSE;
        for (PCWSTR cand :
             {L"SF Pro Text", L"Segoe UI Variable Text", L"Segoe UI"}) {
            if (SUCCEEDED(coll->FindFamilyName(cand, &idx, &exists)) &&
                exists) {
                g_resolvedFontFamily = cand;
                break;
            }
        }
    }
    return g_resolvedFontFamily;
}

struct TextSurfaceResult {
    wuc::CompositionDrawingSurface surface{nullptr};
    float widthPx = 0.0f;
    float heightPx = 0.0f;
};

TextSurfaceResult RenderTextSurface(std::wstring_view text,
                                    float sizePx,
                                    DWRITE_FONT_WEIGHT weight,
                                    D2D1_COLOR_F color,
                                    float maxWidthPx = 600.0f) {
    TextSurfaceResult res;
    if (!g_gfx.dwriteFactory || !g_gfx.compGraphics || text.empty()) {
        return res;
    }

    winrt::com_ptr<IDWriteTextFormat> fmt;
    HRESULT hr = g_gfx.dwriteFactory->CreateTextFormat(
        ResolveFontFamily().c_str(), nullptr, weight, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, sizePx, L"", fmt.put());
    if (FAILED(hr) || !fmt) {
        return res;
    }
    fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    DWRITE_TRIMMING trim{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
    winrt::com_ptr<IDWriteInlineObject> ellipsis;
    if (SUCCEEDED(g_gfx.dwriteFactory->CreateEllipsisTrimmingSign(
            fmt.get(), ellipsis.put()))) {
        fmt->SetTrimming(&trim, ellipsis.get());
    }

    winrt::com_ptr<IDWriteTextLayout> layout;
    hr = g_gfx.dwriteFactory->CreateTextLayout(
        text.data(), static_cast<UINT32>(text.size()), fmt.get(), maxWidthPx,
        sizePx * 2.0f, layout.put());
    if (FAILED(hr) || !layout) {
        return res;
    }

    DWRITE_TEXT_METRICS metrics{};
    if (FAILED(layout->GetMetrics(&metrics))) {
        return res;
    }

    res.widthPx = std::ceil(std::min(metrics.widthIncludingTrailingWhitespace,
                                     maxWidthPx)) +
                  2.0f;
    res.heightPx = std::ceil(metrics.height) + 2.0f;
    res.surface = CreateDrawingSurface(res.widthPx, res.heightPx);
    if (!res.surface) {
        return res;
    }

    DrawCompositionSurface(res.surface, [&](ID2D1DeviceContext* dc) {
        dc->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
        winrt::com_ptr<ID2D1SolidColorBrush> brush;
        if (SUCCEEDED(dc->CreateSolidColorBrush(color, brush.put()))) {
            dc->DrawTextLayout(D2D1::Point2F(1.0f, 1.0f), layout.get(),
                               brush.get());
        }
    });
    return res;
}

std::wstring GetExplorerExePath() {
    WCHAR winDir[MAX_PATH]{};
    if (GetWindowsDirectoryW(winDir, ARRAYSIZE(winDir))) {
        return std::wstring(winDir) + L"\\explorer.exe";
    }
    return L"C:\\Windows\\explorer.exe";
}

winrt::com_ptr<IWICBitmapSource> ConvertWicToPbgra(IWICBitmapSource* src) {
    if (!src || !g_gfx.wicFactory) {
        return nullptr;
    }
    winrt::com_ptr<IWICFormatConverter> conv;
    if (FAILED(g_gfx.wicFactory->CreateFormatConverter(conv.put()))) {
        return nullptr;
    }
    if (FAILED(conv->Initialize(src, GUID_WICPixelFormat32bppPBGRA,
                                WICBitmapDitherTypeNone, nullptr, 0.0f,
                                WICBitmapPaletteTypeCustom))) {
        return nullptr;
    }
    winrt::com_ptr<IWICBitmapSource> out;
    conv->QueryInterface(IID_PPV_ARGS(out.put()));
    return out;
}

uint32_t ReadBigEndian32(const BYTE* p) {
    return (static_cast<uint32_t>(p[0]) << 24) |
           (static_cast<uint32_t>(p[1]) << 16) |
           (static_cast<uint32_t>(p[2]) << 8) | static_cast<uint32_t>(p[3]);
}

int64_t IconSideRank(uint32_t side) {
    return side <= 1024 ? static_cast<int64_t>(side)
                        : -static_cast<int64_t>(side);
}

// Reads a native Apple .icns file and returns its largest embedded PNG (up to 1024px).
std::vector<BYTE> ExtractBestPngFromIcns(const std::wstring& path) {
    HANDLE hFile = CreateFileW(
        path.c_str(), GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        return {};
    }
    LARGE_INTEGER sz{};
    char magic[4]{};
    DWORD read = 0;
    if (!GetFileSizeEx(hFile, &sz) || sz.QuadPart < 8 ||
        sz.QuadPart > (64LL << 20) ||
        !ReadFile(hFile, magic, 4, &read, nullptr) || read != 4 ||
        memcmp(magic, "icns", 4) != 0) {
        CloseHandle(hFile);
        return {};
    }
    std::vector<BYTE> raw(static_cast<size_t>(sz.QuadPart));
    memcpy(raw.data(), magic, 4);
    DWORD rem = static_cast<DWORD>(raw.size() - 4);
    if (!ReadFile(hFile, raw.data() + 4, rem, &read, nullptr) || read != rem) {
        CloseHandle(hFile);
        return {};
    }
    CloseHandle(hFile);

    static constexpr BYTE kPngSig[8] = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1A, '\n'};
    size_t end = std::min<size_t>(raw.size(), ReadBigEndian32(&raw[4]));
    size_t bestOff = 0, bestLen = 0;
    int64_t bestRank = std::numeric_limits<int64_t>::min();

    for (size_t at = 8; at + 8 <= end;) {
        size_t len = ReadBigEndian32(&raw[at + 4]);
        if (len < 8 || len > end - at) {
            break;
        }
        size_t dataOff = at + 8;
        size_t dataLen = len - 8;
        at += len;
        if (dataLen < 24 || memcmp(raw.data() + dataOff, kPngSig, 8) != 0) {
            continue;
        }
        uint32_t side = std::max(ReadBigEndian32(&raw[dataOff + 16]),
                                 ReadBigEndian32(&raw[dataOff + 20]));
        if (!side) {
            continue;
        }
        int64_t rank = IconSideRank(side);
        if (rank > bestRank) {
            bestRank = rank;
            bestOff = dataOff;
            bestLen = dataLen;
        }
    }
    if (!bestLen) {
        return {};
    }
    return std::vector<BYTE>(raw.begin() + bestOff,
                             raw.begin() + bestOff + bestLen);
}

winrt::com_ptr<IWICBitmapSource> LoadWicBitmapFromFile(
    const std::wstring& path) {
    if (path.empty() || !g_gfx.wicFactory) {
        return nullptr;
    }
    DWORD attr = GetFileAttributesW(path.c_str());
    if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY)) {
        return nullptr;
    }

    winrt::com_ptr<IWICBitmapDecoder> decoder;
    winrt::com_ptr<IWICStream> memStream;
    std::vector<BYTE> icnsPng = ExtractBestPngFromIcns(path);
    if (!icnsPng.empty()) {
        if (FAILED(g_gfx.wicFactory->CreateStream(memStream.put())) ||
            FAILED(memStream->InitializeFromMemory(
                icnsPng.data(), static_cast<DWORD>(icnsPng.size()))) ||
            FAILED(g_gfx.wicFactory->CreateDecoderFromStream(
                memStream.get(), nullptr, WICDecodeMetadataCacheOnLoad,
                decoder.put()))) {
            return nullptr;
        }
    } else {
        if (FAILED(g_gfx.wicFactory->CreateDecoderFromFilename(
                path.c_str(), nullptr, GENERIC_READ,
                WICDecodeMetadataCacheOnLoad, decoder.put()))) {
            return nullptr;
        }
    }

    // Pick best-resolution frame for multi-frame .ico files.
    UINT frameCount = 0;
    UINT chosen = 0;
    if (SUCCEEDED(decoder->GetFrameCount(&frameCount)) && frameCount > 1) {
        int64_t bestRank = std::numeric_limits<int64_t>::min();
        for (UINT i = 0; i < std::min(frameCount, 32u); ++i) {
            winrt::com_ptr<IWICBitmapFrameDecode> cand;
            UINT w = 0, h = 0;
            if (SUCCEEDED(decoder->GetFrame(i, cand.put())) &&
                SUCCEEDED(cand->GetSize(&w, &h)) && w > 0 && h > 0) {
                int64_t rank = IconSideRank(std::max(w, h));
                if (rank > bestRank) {
                    bestRank = rank;
                    chosen = i;
                }
            }
        }
    }

    winrt::com_ptr<IWICBitmapFrameDecode> frame;
    if (FAILED(decoder->GetFrame(chosen, frame.put()))) {
        return nullptr;
    }
    return ConvertWicToPbgra(frame.get());
}

winrt::com_ptr<IWICBitmapSource> LoadWicBitmapFromHBitmap(HBITMAP hbmp) {
    if (!hbmp || !g_gfx.wicFactory) {
        return nullptr;
    }
    winrt::com_ptr<IWICBitmap> wicBmp;
    if (FAILED(g_gfx.wicFactory->CreateBitmapFromHBITMAP(
            hbmp, nullptr, WICBitmapUseAlpha, wicBmp.put()))) {
        return nullptr;
    }
    // Check if legacy 32-bit DIB has all-zero alpha channel; if so reload ignoring alpha.
    UINT w = 0, h = 0;
    if (SUCCEEDED(wicBmp->GetSize(&w, &h)) && w > 0 && h > 0) {
        WICRect rc{0, 0, static_cast<INT>(w), static_cast<INT>(h)};
        winrt::com_ptr<IWICBitmapLock> lock;
        if (SUCCEEDED(wicBmp->Lock(&rc, WICBitmapLockRead, lock.put()))) {
            UINT bufSz = 0;
            BYTE* ptr = nullptr;
            if (SUCCEEDED(lock->GetDataPointer(&bufSz, &ptr)) && ptr) {
                bool anyAlpha = false;
                for (UINT i = 3; i < bufSz; i += 4) {
                    if (ptr[i] != 0) {
                        anyAlpha = true;
                        break;
                    }
                }
                lock = nullptr;
                if (!anyAlpha) {
                    wicBmp = nullptr;
                    if (FAILED(g_gfx.wicFactory->CreateBitmapFromHBITMAP(
                            hbmp, nullptr, WICBitmapIgnoreAlpha,
                            wicBmp.put()))) {
                        return nullptr;
                    }
                }
            }
        }
    }
    return ConvertWicToPbgra(wicBmp.get());
}

winrt::com_ptr<IWICBitmapSource> LoadWicBitmapFromHIcon(HICON hicon) {
    if (!hicon || !g_gfx.wicFactory) {
        return nullptr;
    }
    winrt::com_ptr<IWICBitmap> wicBmp;
    if (FAILED(g_gfx.wicFactory->CreateBitmapFromHICON(hicon, wicBmp.put()))) {
        return nullptr;
    }
    return ConvertWicToPbgra(wicBmp.get());
}

winrt::com_ptr<IWICBitmapSource> LoadShellItemIcon(IShellItem* item,
                                                   int sizePx = 256) {
    if (!item) {
        return nullptr;
    }
    winrt::com_ptr<IShellItemImageFactory> factory;
    if (FAILED(item->QueryInterface(IID_PPV_ARGS(factory.put())))) {
        return nullptr;
    }
    HBITMAP hbmp = nullptr;
    HRESULT hr = factory->GetImage(SIZE{sizePx, sizePx},
                                   SIIGBF_ICONONLY | SIIGBF_BIGGERSIZEOK,
                                   &hbmp);
    if (FAILED(hr) || !hbmp) {
        return nullptr;
    }
    auto res = LoadWicBitmapFromHBitmap(hbmp);
    DeleteObject(hbmp);
    return res;
}

winrt::com_ptr<IWICBitmapSource> LoadIconFromPath(const std::wstring& path,
                                                  int sizePx = 256) {
    if (path.empty()) {
        return nullptr;
    }
    winrt::com_ptr<IShellItem> item;
    if (SUCCEEDED(SHCreateItemFromParsingName(path.c_str(), nullptr,
                                              IID_PPV_ARGS(item.put())))) {
        if (auto bmp = LoadShellItemIcon(item.get(), sizePx)) {
            return bmp;
        }
    }
    SHFILEINFOW sfi{};
    if (SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi),
                       SHGFI_ICON | SHGFI_LARGEICON) &&
        sfi.hIcon) {
        auto bmp = LoadWicBitmapFromHIcon(sfi.hIcon);
        DestroyIcon(sfi.hIcon);
        return bmp;
    }
    return nullptr;
}

winrt::com_ptr<IWICBitmapSource> LoadIconFromAumid(const std::wstring& aumid,
                                                   int sizePx = 256) {
    if (aumid.empty()) {
        return nullptr;
    }
    winrt::com_ptr<IShellItem> item;
    if (SUCCEEDED(SHCreateItemInKnownFolder(kFolderIdAppsFolder, 0,
                                            aumid.c_str(),
                                            IID_PPV_ARGS(item.put())))) {
        return LoadShellItemIcon(item.get(), sizePx);
    }
    return nullptr;
}

winrt::com_ptr<IWICBitmapSource> LoadIconFromWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd) || IsHungAppWindow(hwnd)) {
        return nullptr;
    }
    HICON hicon = nullptr;
    DWORD_PTR result = 0;
    if (SendMessageTimeoutW(hwnd, WM_GETICON, ICON_BIG, 0,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 200, &result) &&
        result) {
        hicon = reinterpret_cast<HICON>(result);
    }
    if (!hicon &&
        SendMessageTimeoutW(hwnd, WM_GETICON, ICON_SMALL2, 0,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 200, &result) &&
        result) {
        hicon = reinterpret_cast<HICON>(result);
    }
    if (!hicon) {
        hicon = reinterpret_cast<HICON>(GetClassLongPtrW(hwnd, GCLP_HICON));
    }
    if (!hicon) {
        return nullptr;
    }
    return LoadWicBitmapFromHIcon(hicon);
}

// 8-point alpha test: 4 corners inset 12% + 4 edge midpoints inset 4%.
// >= 7 points with alpha > 200 => full-bleed artwork (clip directly to squircle).
void ShowInFileExplorer(const std::wstring& path) {
    if (path.empty()) {
        return;
    }
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, nullptr)) &&
        pidl) {
        SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
        CoTaskMemFree(pidl);
    }
}

// 13. Liquid Glass Right-Click Menus & 1-Click Icon Changer (M4)
// ============================================================================

struct MenuEntry {
    enum class Type { Action, Separator, Submenu };
    Type type = Type::Action;
    std::wstring label;
    bool checked = false;
    bool enabled = true;
    std::function<void()> onClick;
    std::vector<MenuEntry> submenu;
};

struct MenuRowVisual {
    float top = 0.0f;
    float height = 0.0f;
    wuc::SpriteVisual labelNormalVisual{nullptr};
    wuc::SpriteVisual labelHoverVisual{nullptr};
    wuc::SpriteVisual shortcutNormalVisual{nullptr};
    wuc::SpriteVisual shortcutHoverVisual{nullptr};
    wuc::SpriteVisual checkNormalVisual{nullptr};
    wuc::SpriteVisual checkHoverVisual{nullptr};
    wuc::SpriteVisual arrowNormalVisual{nullptr};
    wuc::SpriteVisual arrowHoverVisual{nullptr};
    wuc::SpriteVisual sepVisual{nullptr};
};

struct GlassMenuPopup {
    HWND hwnd = nullptr;
    bool open = false;
    bool isSubmenu = false;
    int hoveredRow = -1;
    int openSubForRow = -1;
    float widthPx = 0.0f;
    float heightPx = 0.0f;
    std::vector<MenuEntry> entries;
    std::vector<MenuRowVisual> rows;

    wuc::Desktop::DesktopWindowTarget target{nullptr};
    wuc::ContainerVisual root{nullptr};
    wuc::ContainerVisual glassContainer{nullptr};
    wuc::CompositionPathGeometry glassClipGeom{nullptr};
    wuc::SpriteVisual backdropVisual{nullptr};
    wuc::SpriteVisual tintVisual{nullptr};
    wuc::SpriteVisual sheenVisual{nullptr};
    wuc::ShapeVisual rimVisual{nullptr};
    wuc::SpriteVisual hoverVisual{nullptr};
    wuc::ContainerVisual itemsLayer{nullptr};
    LiquidGlassPlate liquidPlate;

    void DestroyVisuals() {
        liquidPlate.Shutdown();
        rows.clear();
        entries.clear();
        itemsLayer = nullptr;
        hoverVisual = nullptr;
        rimVisual = nullptr;
        sheenVisual = nullptr;
        tintVisual = nullptr;
        backdropVisual = nullptr;
        glassClipGeom = nullptr;
        glassContainer = nullptr;
        root = nullptr;
        target = nullptr;
        hoveredRow = -1;
        openSubForRow = -1;
        open = false;
    }

    void CloseWindow() {
        DestroyVisuals();
        if (hwnd) {
            ShowWindow(hwnd, SW_HIDE);
            DestroyWindow(hwnd);
            hwnd = nullptr;
        }
    }
};

struct MenuManagerState {
    GlassMenuPopup mainPopup;
    GlassMenuPopup subPopup;
    std::function<void()> pendingAction;
};

MenuManagerState g_menus;
constexpr WCHAR kMenuClassName[] = L"WindhawkMacOSTahoeAppsMenuWnd";


void OnFinderBarMenuClosed();

void CloseAllMenus() {
    g_menus.subPopup.CloseWindow();
    g_menus.mainPopup.CloseWindow();
    g_dock.modalActive = false;
    OnFinderBarMenuClosed();
    RequestAnimationFrame();
}

void PostDeferredDockAction(std::function<void()> fn) {
    g_menus.pendingAction = std::move(fn);
    if (g_dock.hwnd) {
        PostMessageW(g_dock.hwnd, kWmAppDeferredAction, 0, 0);
    }
}

void OpenWindhawkUi() {
    WCHAR selfPath[MAX_PATH]{};
    if (GetModuleFileNameW(nullptr, selfPath, ARRAYSIZE(selfPath)) > 0) {
        ShellExecuteW(nullptr, L"open", selfPath, nullptr, nullptr,
                      SW_SHOWNORMAL);
    }
}

bool OpenGlassMenuPopup(GlassMenuPopup& popup,
                        std::vector<MenuEntry> entries,
                        POINT anchorScreen,
                        bool alignAbove,
                        bool isSubmenu);

void UpdateMenuHoverState(GlassMenuPopup& popup, int newRow) {
    if (newRow == popup.hoveredRow) {
        return;
    }
    int oldRow = popup.hoveredRow;
    popup.hoveredRow = newRow;

    auto setRowHovered = [&](int idx, bool hov) {
        if (idx < 0 || idx >= static_cast<int>(popup.rows.size())) {
            return;
        }
        auto& r = popup.rows[idx];
        if (r.labelNormalVisual) {
            r.labelNormalVisual.IsVisible(!hov);
        }
        if (r.labelHoverVisual) {
            r.labelHoverVisual.IsVisible(hov);
        }
        if (r.shortcutNormalVisual) {
            r.shortcutNormalVisual.IsVisible(!hov);
        }
        if (r.shortcutHoverVisual) {
            r.shortcutHoverVisual.IsVisible(hov);
        }
        if (r.checkNormalVisual) {
            r.checkNormalVisual.IsVisible(!hov);
        }
        if (r.checkHoverVisual) {
            r.checkHoverVisual.IsVisible(hov);
        }
        if (r.arrowNormalVisual) {
            r.arrowNormalVisual.IsVisible(!hov);
        }
        if (r.arrowHoverVisual) {
            r.arrowHoverVisual.IsVisible(hov);
        }
    };

    setRowHovered(oldRow, false);

    bool validHover =
        newRow >= 0 && newRow < static_cast<int>(popup.entries.size()) &&
        popup.entries[newRow].type != MenuEntry::Type::Separator &&
        popup.entries[newRow].enabled;

    if (validHover) {
        setRowHovered(newRow, true);
        if (popup.hoverVisual) {
            const float s = g_dock.scale;
            const float padX = 5.0f * s;
            const auto& rv = popup.rows[newRow];
            popup.hoverVisual.Offset({padX, rv.top, 0.0f});
            popup.hoverVisual.Size(
                {std::max(1.0f, popup.widthPx - padX * 2.0f), rv.height});
            popup.hoverVisual.IsVisible(true);
        }
    } else if (popup.hoverVisual) {
        popup.hoverVisual.IsVisible(false);
    }

    // Open or close submenu when hovering rows on main menu
    if (!popup.isSubmenu) {
        if (validHover &&
            popup.entries[newRow].type == MenuEntry::Type::Submenu &&
            !popup.entries[newRow].submenu.empty()) {
            if (popup.openSubForRow != newRow) {
                RECT rc{};
                GetWindowRect(popup.hwnd, &rc);
                POINT anchor{
                    rc.right - static_cast<LONG>(4.0f * g_dock.scale),
                    rc.top + static_cast<LONG>(popup.rows[newRow].top) -
                        static_cast<LONG>(6.0f * g_dock.scale)};
                popup.openSubForRow = newRow;
                OpenGlassMenuPopup(g_menus.subPopup,
                                   popup.entries[newRow].submenu, anchor, false,
                                   true);
            }
        } else if (popup.openSubForRow != -1) {
            popup.openSubForRow = -1;
            g_menus.subPopup.CloseWindow();
        }
    }
}

void ActivateMenuEntry(GlassMenuPopup& popup, int idx) {
    if (idx < 0 || idx >= static_cast<int>(popup.entries.size())) {
        return;
    }
    const auto& e = popup.entries[idx];
    if (!e.enabled || e.type == MenuEntry::Type::Separator) {
        return;
    }
    if (e.type == MenuEntry::Type::Submenu) {
        UpdateMenuHoverState(popup, idx);
        return;
    }
    auto cb = e.onClick;
    CloseAllMenus();
    if (cb) {
        PostDeferredDockAction(std::move(cb));
    }
}

void StepMenuSelection(GlassMenuPopup& popup, int dir) {
    if (popup.entries.empty()) {
        return;
    }
    int n = static_cast<int>(popup.entries.size());
    int cur = popup.hoveredRow;
    for (int step = 0; step < n; ++step) {
        cur = (cur + dir + n) % n;
        if (popup.entries[cur].type != MenuEntry::Type::Separator &&
            popup.entries[cur].enabled) {
            UpdateMenuHoverState(popup, cur);
            return;
        }
    }
}

LRESULT CALLBACK GlassMenuWndProc(HWND hwnd,
                                  UINT msg,
                                  WPARAM wParam,
                                  LPARAM lParam) {
    try {
        GlassMenuPopup* popup =
            (hwnd == g_menus.subPopup.hwnd)
                ? &g_menus.subPopup
                : ((hwnd == g_menus.mainPopup.hwnd) ? &g_menus.mainPopup
                                                    : nullptr);

        switch (msg) {
            case WM_ACTIVATE:
                if (LOWORD(wParam) == WA_INACTIVE) {
                    HWND next = reinterpret_cast<HWND>(lParam);
                    if (next != g_menus.mainPopup.hwnd &&
                        next != g_menus.subPopup.hwnd) {
                        CloseAllMenus();
                    }
                }
                return 0;

            case WM_MOUSEMOVE:
                if (popup) {
                    float ly =
                        static_cast<float>(static_cast<short>(HIWORD(lParam)));
                    int hit = -1;
                    for (size_t i = 0; i < popup->rows.size(); ++i) {
                        if (ly >= popup->rows[i].top &&
                            ly < popup->rows[i].top + popup->rows[i].height) {
                            hit = static_cast<int>(i);
                            break;
                        }
                    }
                    UpdateMenuHoverState(*popup, hit);
                    TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                    TrackMouseEvent(&tme);
                }
                return 0;

            case WM_MOUSELEAVE:
                if (popup) {
                    // Keep parent row highlighted while submenu is open
                    if (popup->isSubmenu || popup->openSubForRow == -1) {
                        UpdateMenuHoverState(*popup, -1);
                    }
                }
                return 0;

            case WM_LBUTTONUP:
                if (popup) {
                    float ly =
                        static_cast<float>(static_cast<short>(HIWORD(lParam)));
                    for (size_t i = 0; i < popup->rows.size(); ++i) {
                        if (ly >= popup->rows[i].top &&
                            ly < popup->rows[i].top + popup->rows[i].height) {
                            ActivateMenuEntry(*popup, static_cast<int>(i));
                            break;
                        }
                    }
                }
                return 0;

            case WM_KEYDOWN:
                if (popup) {
                    GlassMenuPopup& active =
                        (g_menus.subPopup.open && !popup->isSubmenu &&
                         g_menus.subPopup.hoveredRow >= 0)
                            ? g_menus.subPopup
                            : *popup;
                    if (wParam == VK_ESCAPE) {
                        CloseAllMenus();
                    } else if (wParam == VK_DOWN) {
                        StepMenuSelection(active, +1);
                    } else if (wParam == VK_UP) {
                        StepMenuSelection(active, -1);
                    } else if (wParam == VK_RIGHT && !active.isSubmenu &&
                               g_menus.subPopup.open) {
                        StepMenuSelection(g_menus.subPopup, +1);
                    } else if (wParam == VK_LEFT && g_menus.subPopup.open) {
                        g_menus.mainPopup.openSubForRow = -1;
                        g_menus.subPopup.CloseWindow();
                    } else if (wParam == VK_RETURN) {
                        if (active.hoveredRow >= 0) {
                            ActivateMenuEntry(active, active.hoveredRow);
                        }
                    }
                }
                return 0;
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"GlassMenuWndProc WinRT error: 0x%08X", (unsigned)e.code());
    } catch (...) {
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool OpenGlassMenuPopup(GlassMenuPopup& popup,
                        std::vector<MenuEntry> entries,
                        POINT anchorScreen,
                        bool alignAbove,
                        bool isSubmenu) {
    popup.CloseWindow();
    if (entries.empty() || !g_gfx.compositor || !g_dock.hostBackdrop) {
        return false;
    }

    static bool s_classReg = false;
    HMODULE hSelf = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&OpenGlassMenuPopup), &hSelf);
    if (!s_classReg) {
        WNDCLASSEXW wc{
            .cbSize = sizeof(wc),
            .lpfnWndProc = GlassMenuWndProc,
            .hInstance = hSelf,
            .hCursor = LoadCursorW(nullptr, IDC_ARROW),
            .lpszClassName = kMenuClassName,
        };
        RegisterClassExW(&wc);
        s_classReg = true;
    }

    const float s = g_dock.scale;
    const bool dark = g_dock.darkMode;
    const float padV = 6.0f * s;
    const float rowH = 26.0f * s;
    const float sepH = 9.0f * s;
    const float checkCol = 18.0f * s;
    const float arrowCol = 18.0f * s;
    const float sidePad = 10.0f * s;
    const float fontSize = 13.0f * s;
    const float corner = 10.0f * s;

    auto normalCol = dark ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.94f)
                          : D2D1::ColorF(0.08f, 0.08f, 0.10f, 0.94f);
    auto disabledCol = dark ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.36f)
                            : D2D1::ColorF(0.08f, 0.08f, 0.10f, 0.36f);
    auto shortcutCol = dark ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.48f)
                            : D2D1::ColorF(0.08f, 0.08f, 0.10f, 0.48f);
    auto hoverCol = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.98f);
    auto hoverShortcutCol = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.80f);

    // Pre-render text surfaces and compute menu dimensions
    struct BuiltRow {
        TextSurfaceResult labelNorm;
        TextSurfaceResult labelHov;
        TextSurfaceResult shortcutNorm;
        TextSurfaceResult shortcutHov;
        TextSurfaceResult checkNorm;
        TextSurfaceResult checkHov;
        TextSurfaceResult arrowNorm;
        TextSurfaceResult arrowHov;
        float top = 0.0f;
        float height = 0.0f;
    };
    std::vector<BuiltRow> built(entries.size());
    float maxTextW = 120.0f * s;
    float maxShortcutW = 0.0f;
    float y = padV;

    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& e = entries[i];
        built[i].top = y;
        if (e.type == MenuEntry::Type::Separator) {
            built[i].height = sepH;
            y += sepH;
            continue;
        }
        built[i].height = rowH;
        y += rowH;

        std::wstring mainText = e.label;
        std::wstring shortcutText;
        size_t tabPos = e.label.find(L'\t');
        if (tabPos != std::wstring::npos) {
            mainText = e.label.substr(0, tabPos);
            shortcutText = e.label.substr(tabPos + 1);
        }

        D2D1_COLOR_F col = e.enabled ? normalCol : disabledCol;
        built[i].labelNorm = RenderTextSurface(
            mainText, fontSize, DWRITE_FONT_WEIGHT_NORMAL, col, 260.0f * s);
        maxTextW = std::max(maxTextW, built[i].labelNorm.widthPx);
        if (e.enabled) {
            built[i].labelHov = RenderTextSurface(
                mainText, fontSize, DWRITE_FONT_WEIGHT_NORMAL, hoverCol,
                260.0f * s);
        }
        if (!shortcutText.empty()) {
            D2D1_COLOR_F scol = e.enabled ? shortcutCol : disabledCol;
            built[i].shortcutNorm = RenderTextSurface(
                shortcutText, fontSize * 0.92f, DWRITE_FONT_WEIGHT_NORMAL, scol,
                140.0f * s);
            maxShortcutW =
                std::max(maxShortcutW, built[i].shortcutNorm.widthPx);
            if (e.enabled) {
                built[i].shortcutHov = RenderTextSurface(
                    shortcutText, fontSize * 0.92f, DWRITE_FONT_WEIGHT_NORMAL,
                    hoverShortcutCol, 140.0f * s);
            }
        }
        if (e.checked) {
            built[i].checkNorm = RenderTextSurface(
                L"\u2713", fontSize, DWRITE_FONT_WEIGHT_SEMI_BOLD, col,
                24.0f * s);
            if (e.enabled) {
                built[i].checkHov = RenderTextSurface(
                    L"\u2713", fontSize, DWRITE_FONT_WEIGHT_SEMI_BOLD, hoverCol,
                    24.0f * s);
            }
        }
        if (e.type == MenuEntry::Type::Submenu) {
            built[i].arrowNorm = RenderTextSurface(
                L"\u203A", fontSize * 1.15f, DWRITE_FONT_WEIGHT_MEDIUM, col,
                24.0f * s);
            if (e.enabled) {
                built[i].arrowHov = RenderTextSurface(
                    L"\u203A", fontSize * 1.15f, DWRITE_FONT_WEIGHT_MEDIUM,
                    hoverCol, 24.0f * s);
            }
        }
    }
    float totalH = y + padV;
    float shortcutGap = (maxShortcutW > 0.0f) ? (18.0f * s + maxShortcutW) : 0.0f;
    float totalW = std::ceil(sidePad + checkCol + maxTextW +
                             std::max(arrowCol, shortcutGap) + sidePad);
    totalH = std::ceil(totalH);

    // Clamp menu position inside monitor rect
    const RECT& mr = g_dock.monitorRect;
    int px = anchorScreen.x;
    int py = alignAbove ? (anchorScreen.y - static_cast<int>(totalH))
                        : anchorScreen.y;
    if (isSubmenu && px + static_cast<int>(totalW) > mr.right - 6) {
        // Flip submenu to left side of parent menu
        RECT pr{};
        if (g_menus.mainPopup.hwnd && GetWindowRect(g_menus.mainPopup.hwnd, &pr)) {
            px = pr.left - static_cast<int>(totalW) + static_cast<int>(4.0f * s);
        }
    }
    px = std::clamp(px, static_cast<int>(mr.left + 6),
                    std::max(static_cast<int>(mr.left + 6),
                             static_cast<int>(mr.right - totalW - 6)));
    py = std::clamp(py, static_cast<int>(mr.top + 6),
                    std::max(static_cast<int>(mr.top + 6),
                             static_cast<int>(mr.bottom - totalH - 6)));

    DWORD exStyle = WS_EX_NOREDIRECTIONBITMAP | WS_EX_LAYERED | WS_EX_TOPMOST |
                    WS_EX_TOOLWINDOW | (isSubmenu ? WS_EX_NOACTIVATE : 0);
    popup.hwnd = CreateWindowExW(exStyle, kMenuClassName, L"", WS_POPUP, px, py,
                                 static_cast<int>(totalW),
                                 static_cast<int>(totalH), nullptr, nullptr,
                                 hSelf, nullptr);
    if (!popup.hwnd) {
        return false;
    }
    SetLayeredWindowAttributes(popup.hwnd, 0, 255, LWA_ALPHA);
    ApplyWindowGlassCaptureAffinity(popup.hwnd);
    const BOOL hostBackdropAttr = TRUE;
    DwmSetWindowAttribute(popup.hwnd, kDwmUseHostBackdropBrush,
                          &hostBackdropAttr, sizeof(hostBackdropAttr));

    auto& c = g_gfx.compositor;
    auto desktopInterop = c.as<ICompositorDesktopInteropLocal>();
    if (FAILED(desktopInterop->CreateDesktopWindowTarget(
            popup.hwnd, TRUE, winrt::put_abi(popup.target))) ||
        !popup.target) {
        popup.CloseWindow();
        return false;
    }

    popup.isSubmenu = isSubmenu;
    popup.widthPx = totalW;
    popup.heightPx = totalH;
    popup.entries = std::move(entries);
    popup.open = true;

    popup.root = c.CreateContainerVisual();
    popup.target.Root(popup.root);

    // Liquid Glass menu container
    popup.glassContainer = c.CreateContainerVisual();
    popup.glassContainer.Size({totalW, totalH});
    popup.glassClipGeom = c.CreatePathGeometry();
    if (auto path = MakeCompositionSquirclePath(0, 0, totalW, totalH, corner)) {
        popup.glassClipGeom.Path(path);
    }
    popup.glassContainer.Clip(c.CreateGeometricClip(popup.glassClipGeom));
    popup.root.Children().InsertAtTop(popup.glassContainer);

    // Blurred backdrop
    float blurPx = (14.0f + 12.0f * (g_settings.glassBlur / 100.0f)) * s;
    auto backdropBrush = CreateLiquidGlassBackdropBrush(
        c, g_dock.hostBackdrop, blurPx, dark ? 1.50f : 1.35f);
    popup.backdropVisual = c.CreateSpriteVisual();
    popup.backdropVisual.Size({totalW, totalH});
    popup.backdropVisual.Brush(backdropBrush);
    popup.glassContainer.Children().InsertAtTop(popup.backdropVisual);

    // Smoked glass fill (high readability)
    popup.tintVisual = c.CreateSpriteVisual();
    popup.tintVisual.Size({totalW, totalH});
    popup.tintVisual.Brush(c.CreateColorBrush(
        dark ? winrt::Windows::UI::Color{184, 24, 24, 30}
             : winrt::Windows::UI::Color{198, 248, 248, 252}));
    popup.glassContainer.Children().InsertAtTop(popup.tintVisual);

    // Subtle top sheen
    auto sheenBrush = c.CreateLinearGradientBrush();
    sheenBrush.MappingMode(wuc::CompositionMappingMode::Absolute);
    sheenBrush.StartPoint({0.0f, 0.0f});
    sheenBrush.EndPoint({0.0f, totalH * 0.55f});
    sheenBrush.ColorStops().Append(c.CreateColorGradientStop(
        0.0f, winrt::Windows::UI::Color{static_cast<uint8_t>(dark ? 0x1E : 0x36),
                                        255, 255, 255}));
    sheenBrush.ColorStops().Append(
        c.CreateColorGradientStop(1.0f, winrt::Windows::UI::Color{0, 255, 255, 255}));
    popup.sheenVisual = c.CreateSpriteVisual();
    popup.sheenVisual.Size({totalW, totalH});
    popup.sheenVisual.Brush(sheenBrush);
    popup.glassContainer.Children().InsertAtTop(popup.sheenVisual);

    // Specular rim
    float sw = 1.0f * s;
    auto rimGeom = c.CreatePathGeometry();
    if (auto rp = MakeCompositionSquirclePath(
            sw * 0.5f, sw * 0.5f, std::max(1.0f, totalW - sw),
            std::max(1.0f, totalH - sw), std::max(1.0f, corner - sw * 0.4f))) {
        rimGeom.Path(rp);
    }
    auto rimShape = c.CreateSpriteShape(rimGeom);
    auto rimBrush = c.CreateLinearGradientBrush();
    rimBrush.MappingMode(wuc::CompositionMappingMode::Absolute);
    rimBrush.StartPoint({0.0f, 0.0f});
    rimBrush.EndPoint({totalW, totalH});
    float rs = dark ? 0.75f : 1.0f;
    auto rimStop = [&](float off, float a) {
        rimBrush.ColorStops().Append(c.CreateColorGradientStop(
            off, winrt::Windows::UI::Color{
                     static_cast<uint8_t>(std::lround(a * rs * 255.0f)), 255,
                     255, 255}));
    };
    rimStop(0.0f, 0.72f);
    rimStop(0.45f, 0.16f);
    rimStop(1.0f, 0.48f);
    rimShape.StrokeBrush(rimBrush);
    rimShape.StrokeThickness(sw);
    popup.rimVisual = c.CreateShapeVisual();
    popup.rimVisual.Size({totalW, totalH});
    popup.rimVisual.Shapes().Append(rimShape);
    popup.glassContainer.Children().InsertAtTop(popup.rimVisual);

    // Upgrade popup glass to real DXGI optical Liquid Glass if available
    if (EnsureLiquidCaptureReady()) {
        LiquidGlassStyleParams lp{};
        lp.screenX = static_cast<float>(px);
        lp.screenY = static_cast<float>(py);
        lp.widthPx = totalW;
        lp.heightPx = totalH;
        lp.cornerRadiusPx = corner;
        lp.scale = s;
        lp.darkMode = dark;
        lp.useSquircle = true;
        lp.tintBoost = dark ? 0.24f : 0.20f;
        lp.blurMul = 1.65f;
        lp.rimDepthMul = 0.85f;
        lp.strengthMul = 0.90f;
        if (RenderLiquidGlassPlate(popup.liquidPlate, lp) &&
            popup.liquidPlate.visual) {
            popup.glassContainer.Children().InsertAtTop(
                popup.liquidPlate.visual);
            popup.backdropVisual.IsVisible(false);
            popup.tintVisual.IsVisible(false);
            popup.sheenVisual.IsVisible(false);
            popup.rimVisual.IsVisible(false);
        }
    }

    // Hover accent pill
    auto acc = GetAccentColor();
    popup.hoverVisual = c.CreateSpriteVisual();
    popup.hoverVisual.Brush(
        c.CreateColorBrush(winrt::Windows::UI::Color{230, acc.R, acc.G, acc.B}));
    auto hoverClip = c.CreateRoundedRectangleGeometry();
    hoverClip.Size({std::max(1.0f, totalW - 10.0f * s), rowH});
    hoverClip.CornerRadius({6.0f * s, 6.0f * s});
    popup.hoverVisual.Clip(c.CreateGeometricClip(hoverClip));
    popup.hoverVisual.IsVisible(false);
    popup.glassContainer.Children().InsertAtTop(popup.hoverVisual);

    // Row items layer
    popup.itemsLayer = c.CreateContainerVisual();
    popup.itemsLayer.Size({totalW, totalH});
    popup.glassContainer.Children().InsertAtTop(popup.itemsLayer);

    auto makeTextSprite = [&](const TextSurfaceResult& ts, float x, float yPos,
                              bool visible) -> wuc::SpriteVisual {
        if (!ts.surface) {
            return nullptr;
        }
        auto br = c.CreateSurfaceBrush(ts.surface);
        br.Stretch(wuc::CompositionStretch::Fill);
        auto sp = c.CreateSpriteVisual();
        sp.Brush(br);
        sp.Offset({x, yPos, 0.0f});
        sp.Size({ts.widthPx, ts.heightPx});
        sp.IsVisible(visible);
        popup.itemsLayer.Children().InsertAtTop(sp);
        return sp;
    };

    popup.rows.resize(popup.entries.size());
    for (size_t i = 0; i < popup.entries.size(); ++i) {
        auto& rv = popup.rows[i];
        const auto& b = built[i];
        rv.top = b.top;
        rv.height = b.height;

        if (popup.entries[i].type == MenuEntry::Type::Separator) {
            rv.sepVisual = c.CreateSpriteVisual();
            rv.sepVisual.Brush(c.CreateColorBrush(
                dark ? winrt::Windows::UI::Color{44, 255, 255, 255}
                     : winrt::Windows::UI::Color{36, 0, 0, 0}));
            float lineH = std::max(1.0f, std::round(1.0f * s));
            rv.sepVisual.Offset({sidePad + 2.0f * s,
                                 b.top + (b.height - lineH) * 0.5f, 0.0f});
            rv.sepVisual.Size(
                {std::max(1.0f, totalW - (sidePad + 2.0f * s) * 2.0f), lineH});
            popup.itemsLayer.Children().InsertAtTop(rv.sepVisual);
            continue;
        }

        float textY = b.top + (b.height - b.labelNorm.heightPx) * 0.5f;
        rv.labelNormalVisual =
            makeTextSprite(b.labelNorm, sidePad + checkCol, textY, true);
        rv.labelHoverVisual =
            makeTextSprite(b.labelHov, sidePad + checkCol, textY, false);

        if (b.checkNorm.surface) {
            float cy = b.top + (b.height - b.checkNorm.heightPx) * 0.5f;
            rv.checkNormalVisual =
                makeTextSprite(b.checkNorm, sidePad + 2.0f * s, cy, true);
            rv.checkHoverVisual =
                makeTextSprite(b.checkHov, sidePad + 2.0f * s, cy, false);
        }
        if (b.shortcutNorm.surface) {
            float sy = b.top + (b.height - b.shortcutNorm.heightPx) * 0.5f;
            float sx = totalW - sidePad - b.shortcutNorm.widthPx - 2.0f * s;
            rv.shortcutNormalVisual = makeTextSprite(b.shortcutNorm, sx, sy, true);
            rv.shortcutHoverVisual = makeTextSprite(b.shortcutHov, sx, sy, false);
        }
        if (b.arrowNorm.surface) {
            float ay = b.top + (b.height - b.arrowNorm.heightPx) * 0.5f;
            float ax = totalW - sidePad - b.arrowNorm.widthPx - 2.0f * s;
            rv.arrowNormalVisual = makeTextSprite(b.arrowNorm, ax, ay, true);
            rv.arrowHoverVisual = makeTextSprite(b.arrowHov, ax, ay, false);
        }
    }

    ShowWindow(popup.hwnd, isSubmenu ? SW_SHOWNOACTIVATE : SW_SHOW);
    if (!isSubmenu) {
        g_dock.modalActive = true;
        SetForegroundWindow(popup.hwnd);
        RequestAnimationFrame();
    }
    return true;
}

void InitGlassPopupWindowShell(HWND hwnd,
                               float widthPx,
                               float heightPx,
                               float cornerPx,
                               int screenX,
                               int screenY,
                               float tintBoost,
                               wuc::Desktop::DesktopWindowTarget& target,
                               wuc::ContainerVisual& root,
                               wuc::ContainerVisual& glassContainer,
                               wuc::CompositionPathGeometry& clipGeom,
                               wuc::SpriteVisual& fallbackBackdrop,
                               wuc::SpriteVisual& fallbackTint,
                               LiquidGlassPlate& liquidPlate,
                               wuc::CompositionDrawingSurface& uiSurface,
                               wuc::CompositionSurfaceBrush& uiBrush,
                               wuc::SpriteVisual& uiVisual) {
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    ApplyWindowGlassCaptureAffinity(hwnd);
    const BOOL hb = TRUE;
    DwmSetWindowAttribute(hwnd, kDwmUseHostBackdropBrush, &hb, sizeof(hb));

    auto& c = g_gfx.compositor;
    auto desk = c.as<ICompositorDesktopInteropLocal>();
    if (FAILED(desk->CreateDesktopWindowTarget(hwnd, TRUE,
                                               winrt::put_abi(target))) ||
        !target) {
        return;
    }
    root = c.CreateContainerVisual();
    target.Root(root);

    glassContainer = c.CreateContainerVisual();
    glassContainer.Size({widthPx, heightPx});
    clipGeom = c.CreatePathGeometry();
    if (auto p = MakeCompositionSquirclePath(0, 0, widthPx, heightPx, cornerPx)) {
        clipGeom.Path(p);
    }
    glassContainer.Clip(c.CreateGeometricClip(clipGeom));
    root.Children().InsertAtTop(glassContainer);

    const bool dark = g_dock.darkMode;
    const float s = g_dock.scale;
    fallbackBackdrop = c.CreateSpriteVisual();
    fallbackBackdrop.Size({widthPx, heightPx});
    fallbackBackdrop.Brush(CreateLiquidGlassBackdropBrush(
        c, g_dock.hostBackdrop, 16.0f * s, dark ? 1.45f : 1.30f));
    glassContainer.Children().InsertAtTop(fallbackBackdrop);

    fallbackTint = c.CreateSpriteVisual();
    fallbackTint.Size({widthPx, heightPx});
    fallbackTint.Brush(c.CreateColorBrush(
        dark ? winrt::Windows::UI::Color{176, 20, 21, 27}
             : winrt::Windows::UI::Color{196, 248, 249, 253}));
    glassContainer.Children().InsertAtTop(fallbackTint);

    if (EnsureLiquidCaptureReady()) {
        LiquidGlassStyleParams lp{};
        lp.screenX = static_cast<float>(screenX);
        lp.screenY = static_cast<float>(screenY);
        lp.widthPx = widthPx;
        lp.heightPx = heightPx;
        lp.cornerRadiusPx = cornerPx;
        lp.scale = s;
        lp.darkMode = dark;
        lp.useSquircle = true;
        lp.tintBoost = tintBoost;
        lp.blurMul = 1.65f;
        lp.rimDepthMul = 0.85f;
        lp.strengthMul = 0.90f;
        if (RenderLiquidGlassPlate(liquidPlate, lp) && liquidPlate.visual) {
            glassContainer.Children().InsertAtTop(liquidPlate.visual);
            fallbackBackdrop.IsVisible(false);
            fallbackTint.IsVisible(false);
        }
    }

    uiSurface = CreateDrawingSurface(widthPx, heightPx);
    uiBrush = c.CreateSurfaceBrush(uiSurface);
    uiBrush.Stretch(wuc::CompositionStretch::Fill);
    uiVisual = c.CreateSpriteVisual();
    uiVisual.Size({widthPx, heightPx});
    uiVisual.Brush(uiBrush);
    glassContainer.Children().InsertAtTop(uiVisual);
}

// ---- M10. macOS 26 Tahoe Apps Launcher (Spotlight-Style Categorized Overlay) ----

struct LauncherAppEntry {
    std::wstring name;
    std::wstring lowerName;
    std::wstring path;     // .lnk or .exe path
    std::wstring aumid;    // UWP AUMID if applicable
    std::wstring targetExe;
    std::wstring args;
    std::wstring workdir;
    int category = 1;  // 1=Productivity, 2=Utilities, 3=Creativity, 4=Entertainment, 5=Social
    winrt::com_ptr<IWICBitmapSource> icon;
    bool iconLoaded = false;
    D2D1_RECT_F hitRect{};
};

struct TahoeAppsLauncherState {
    HWND hwnd = nullptr;
    bool open = false;
    bool catalogLoaded = false;
    bool listView = false;
    int activeCategory = 0;  // 0 = All, 1..5 = categories
    std::wstring query;
    float scrollY = 0.0f;
    float maxScrollY = 0.0f;
    int hoveredIdx = -1;
    int selectedIdx = 0;
    int hoveredCat = -1;
    bool hoveredViewToggle = false;
    bool hoveredClear = false;

    float widthPx = 0.0f;
    float heightPx = 0.0f;
    D2D1_RECT_F searchRect{};
    D2D1_RECT_F clearBtnRect{};
    D2D1_RECT_F viewToggleRect{};
    D2D1_RECT_F catRects[6]{};
    D2D1_RECT_F listViewport{};

    std::vector<LauncherAppEntry> allApps;
    std::vector<size_t> filteredIdx;

    wuc::Desktop::DesktopWindowTarget target{nullptr};
    wuc::ContainerVisual root{nullptr};
    wuc::ContainerVisual glassContainer{nullptr};
    wuc::CompositionPathGeometry clipGeom{nullptr};
    wuc::SpriteVisual fallbackBackdrop{nullptr};
    wuc::SpriteVisual fallbackTint{nullptr};
    LiquidGlassPlate liquidPlate;
    wuc::CompositionDrawingSurface uiSurface{nullptr};
    wuc::CompositionSurfaceBrush uiBrush{nullptr};
    wuc::SpriteVisual uiVisual{nullptr};
};

TahoeAppsLauncherState g_appsLauncher;
constexpr WCHAR kAppsLauncherClassName[] = L"WindhawkMacOS27AppsLauncherWnd";

int ClassifyTahoeAppCategory(const std::wstring& lowerName,
                             const std::wstring& lowerFolder) {
    auto hasAny = [&](const std::wstring& hay,
                      std::initializer_list<PCWSTR> keys) {
        for (PCWSTR k : keys) {
            if (hay.find(k) != std::wstring::npos) return true;
        }
        return false;
    };
    std::wstring combined = lowerName + L" " + lowerFolder;
    if (hasAny(combined, {L"discord", L"whatsapp", L"telegram", L"slack",
                          L"teams", L"zoom", L"skype", L"mail", L"outlook",
                          L"messages", L"signal", L"messenger", L"wechat",
                          L"twitter", L"reddit"})) {
        return 5;  // Social
    }
    if (hasAny(combined, {L"spotify", L"steam", L"epic", L"xbox", L"vlc",
                          L"netflix", L"music", L"media", L"video", L"youtube",
                          L"game", L"obs", L"twitch", L"itunes", L"apple tv",
                          L"plex", L"battle.net", L"riot", L"ea ", L"ubisoft",
                          L"gog", L"sound", L"tv"})) {
        return 4;  // Entertainment
    }
    if (hasAny(combined, {L"photoshop", L"illustrator", L"premiere", L"after effects",
                          L"blender", L"figma", L"canva", L"davinci", L"gimp",
                          L"inkscape", L"paint", L"photo", L"camera", L"clipchamp",
                          L"audacity", L"fl studio", L"ableton", L"capcut",
                          L"jianying", L"krita", L"unity", L"unreal", L"3d",
                          L"studio", L"draw", L"design"})) {
        return 3;  // Creativity
    }
    if (hasAny(combined, {L"settings", L"control panel", L"terminal", L"powershell",
                          L"cmd", L"command prompt", L"task manager", L"windhawk",
                          L"registry", L"calculator", L"snipping", L"defender",
                          L"security", L"backup", L"disk", L"device", L"system",
                          L"admin", L"tools", L"7-zip", L"winrar", L"everything",
                          L"powertoys", L"cpuz", L"hwinfo", L"nvidia", L"amd",
                          L"intel", L"driver", L"network", L"vpn", L"clock"})) {
        return 2;  // Utilities
    }
    return 1;  // Productivity (browsers, editors, Office, IDEs, notes)
}

void ScanStartMenuDirForLauncher(const std::wstring& dir,
                                 const std::wstring& relFolder,
                                 std::vector<LauncherAppEntry>& out,
                                 std::unordered_set<std::wstring>& seenNames) {
    WIN32_FIND_DATAW fd{};
    std::wstring pat = dir + L"\\*";
    HANDLE hf = FindFirstFileW(pat.c_str(), &fd);
    if (hf == INVALID_HANDLE_VALUE) {
        return;
    }
    do {
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) {
            continue;
        }
        std::wstring full = dir + L"\\" + fd.cFileName;
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            std::wstring sub =
                relFolder.empty() ? std::wstring(fd.cFileName)
                                  : (relFolder + L" " + fd.cFileName);
            ScanStartMenuDirForLauncher(full, sub, out, seenNames);
            continue;
        }
        std::wstring lowFile = ToLower(fd.cFileName);
        if (!lowFile.ends_with(L".lnk")) {
            continue;
        }
        std::wstring stem(fd.cFileName);
        if (stem.size() > 4) {
            stem.resize(stem.size() - 4);
        }
        std::wstring lowStem = ToLower(stem);
        if (lowStem.find(L"uninstall") != std::wstring::npos ||
            lowStem.find(L"readme") != std::wstring::npos ||
            lowStem.find(L"help") == 0 ||
            lowStem.find(L"manual") != std::wstring::npos ||
            lowStem.find(L"documentation") != std::wstring::npos) {
            continue;
        }
        if (seenNames.count(lowStem)) {
            continue;
        }
        seenNames.insert(lowStem);
        LauncherAppEntry e;
        e.name = stem;
        e.lowerName = lowStem;
        e.path = full;
        e.category = ClassifyTahoeAppCategory(lowStem, ToLower(relFolder));
        out.push_back(std::move(e));
    } while (FindNextFileW(hf, &fd));
    FindClose(hf);
}

void EnsureTahoeAppsCatalogLoaded(bool forceRefresh) {
    if (g_appsLauncher.catalogLoaded && !forceRefresh) {
        return;
    }
    g_appsLauncher.allApps.clear();
    std::unordered_set<std::wstring> seenNames;

    // 1. Scan Common + User Start Menu Programs (.lnk)
    const KNOWNFOLDERID dirs[2] = {FOLDERID_CommonPrograms, FOLDERID_Programs};
    for (const auto& fid : dirs) {
        PWSTR p = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(fid, 0, nullptr, &p)) && p) {
            ScanStartMenuDirForLauncher(p, L"", g_appsLauncher.allApps,
                                        seenNames);
            CoTaskMemFree(p);
        }
    }

    // 2. Enumerate UWP / packaged apps from shell:AppsFolder
    winrt::com_ptr<IShellItem> appsRoot;
    if (SUCCEEDED(SHCreateItemInKnownFolder(kFolderIdAppsFolder, 0, nullptr,
                                            IID_PPV_ARGS(appsRoot.put()))) &&
        appsRoot) {
        winrt::com_ptr<IEnumShellItems> en;
        if (SUCCEEDED(appsRoot->BindToHandler(nullptr, BHID_EnumItems,
                                              IID_PPV_ARGS(en.put()))) &&
            en) {
            winrt::com_ptr<IShellItem> child;
            ULONG got = 0;
            while (en->Next(1, child.put(), &got) == S_OK && got == 1) {
                PWSTR disp = nullptr;
                PWSTR parse = nullptr;
                if (SUCCEEDED(child->GetDisplayName(SIGDN_NORMALDISPLAY, &disp)) &&
                    disp && *disp) {
                    std::wstring name(disp);
                    std::wstring low = ToLower(name);
                    if (!seenNames.count(low) &&
                        low.find(L"uninstall") == std::wstring::npos &&
                        SUCCEEDED(child->GetDisplayName(SIGDN_PARENTRELATIVEPARSING,
                                                        &parse)) &&
                        parse && *parse) {
                        std::wstring aumid(parse);
                        if (aumid.find(L'!') != std::wstring::npos) {
                            seenNames.insert(low);
                            LauncherAppEntry e;
                            e.name = name;
                            e.lowerName = low;
                            e.aumid = aumid;
                            e.category = ClassifyTahoeAppCategory(low, L"");
                            g_appsLauncher.allApps.push_back(std::move(e));
                        }
                    }
                }
                if (disp) CoTaskMemFree(disp);
                if (parse) CoTaskMemFree(parse);
                child = nullptr;
            }
        }
    }

    std::sort(g_appsLauncher.allApps.begin(), g_appsLauncher.allApps.end(),
              [](const LauncherAppEntry& a, const LauncherAppEntry& b) {
                  return _wcsicmp(a.name.c_str(), b.name.c_str()) < 0;
              });
    g_appsLauncher.catalogLoaded = true;
}

void RebuildTahoeAppsFilter() {
    g_appsLauncher.filteredIdx.clear();
    std::wstring lowQ = ToLower(g_appsLauncher.query);
    for (size_t i = 0; i < g_appsLauncher.allApps.size(); ++i) {
        const auto& a = g_appsLauncher.allApps[i];
        if (g_appsLauncher.activeCategory != 0 &&
            a.category != g_appsLauncher.activeCategory) {
            continue;
        }
        if (!lowQ.empty() && a.lowerName.find(lowQ) == std::wstring::npos) {
            continue;
        }
        g_appsLauncher.filteredIdx.push_back(i);
    }
    g_appsLauncher.scrollY = 0.0f;
    g_appsLauncher.hoveredIdx = -1;
    g_appsLauncher.selectedIdx =
        g_appsLauncher.filteredIdx.empty() ? -1 : 0;
}

void CloseTahoeAppsLauncher() {
    if (!g_appsLauncher.open && !g_appsLauncher.hwnd) {
        return;
    }
    g_appsLauncher.open = false;
    if (g_dock.hwnd) {
        KillTimer(g_dock.hwnd, kTimerGlassMaintenance);
    }
    g_liquidCapture.watching.store(false, std::memory_order_relaxed);
    ShutdownLiquidCapture();
    g_appsLauncher.liquidPlate.Shutdown();
    g_appsLauncher.uiVisual = nullptr;
    g_appsLauncher.uiBrush = nullptr;
    g_appsLauncher.uiSurface = nullptr;
    g_appsLauncher.fallbackTint = nullptr;
    g_appsLauncher.fallbackBackdrop = nullptr;
    g_appsLauncher.clipGeom = nullptr;
    g_appsLauncher.glassContainer = nullptr;
    g_appsLauncher.root = nullptr;
    g_appsLauncher.target = nullptr;
    if (g_appsLauncher.hwnd) {
        ShowWindow(g_appsLauncher.hwnd, SW_HIDE);
        DestroyWindow(g_appsLauncher.hwnd);
        g_appsLauncher.hwnd = nullptr;
    }
    g_dock.modalActive = false;
    RequestAnimationFrame();
}

void LaunchTahoeEntry(const LauncherAppEntry& e) {
    std::wstring path = e.path;
    std::wstring aumid = e.aumid;
    CloseTahoeAppsLauncher();
    if (!path.empty()) {
        ShellExecuteW(nullptr, L"open", path.c_str(), nullptr, nullptr,
                      SW_SHOWNORMAL);
    } else if (!aumid.empty()) {
        winrt::com_ptr<IApplicationActivationManager> aam;
        if (SUCCEEDED(CoCreateInstance(CLSID_ApplicationActivationManager,
                                       nullptr, CLSCTX_LOCAL_SERVER,
                                       IID_PPV_ARGS(aam.put())))) {
            DWORD pid = 0;
            aam->ActivateApplication(aumid.c_str(), nullptr, AO_NONE, &pid);
        }
    }
}

void PinTahoeEntryToDock(const LauncherAppEntry& e) {
    HWND dockWnd = FindWindowW(L"WindhawkMacOS27DockWnd", nullptr);
    if (!dockWnd) {
        return;
    }
    std::wstring payload = e.path + L"\x1F" + e.aumid + L"\x1F" + e.name;
    COPYDATASTRUCT cds{};
    cds.dwData = 0x54414831u;
    cds.cbData = static_cast<DWORD>((payload.size() + 1) * sizeof(wchar_t));
    cds.lpData = const_cast<wchar_t*>(payload.c_str());
    SendMessageTimeoutW(dockWnd, WM_COPYDATA,
                        reinterpret_cast<WPARAM>(g_dock.hwnd),
                        reinterpret_cast<LPARAM>(&cds), SMTO_ABORTIFHUNG,
                        500, nullptr);
}

void RenderTahoeAppsLauncher() {
    if (!g_appsLauncher.open || !g_appsLauncher.uiSurface ||
        !g_gfx.dwriteFactory) {
        return;
    }
    const float s = g_dock.scale;
    const float W = g_appsLauncher.widthPx;
    const float H = g_appsLauncher.heightPx;
    const bool dark = g_dock.darkMode;
    auto acc = GetAccentColor();
    D2D1_COLOR_F accCol =
        D2D1::ColorF(acc.R / 255.0f, acc.G / 255.0f, acc.B / 255.0f, 0.90f);
    D2D1_COLOR_F fg = dark ? D2D1::ColorF(0.96f, 0.98f, 1.0f, 0.95f)
                           : D2D1::ColorF(0.08f, 0.09f, 0.12f, 0.94f);
    D2D1_COLOR_F fgSub = dark ? D2D1::ColorF(0.84f, 0.90f, 0.98f, 0.56f)
                              : D2D1::ColorF(0.10f, 0.12f, 0.16f, 0.54f);

    DrawCompositionSurface(g_appsLauncher.uiSurface, [&](ID2D1DeviceContext* dc) {
        winrt::com_ptr<ID2D1SolidColorBrush> br;
        dc->CreateSolidColorBrush(fg, br.put());
        if (!br) return;

        winrt::com_ptr<IDWriteTextFormat> fmtSearch, fmtPill, fmtLabel, fmtSub;
        const auto& fam = ResolveFontFamily();
        g_gfx.dwriteFactory->CreateTextFormat(
            fam.c_str(), nullptr, DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.5f * s,
            L"en-us", fmtSearch.put());
        g_gfx.dwriteFactory->CreateTextFormat(
            fam.c_str(), nullptr, DWRITE_FONT_WEIGHT_MEDIUM,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 11.5f * s,
            L"en-us", fmtPill.put());
        g_gfx.dwriteFactory->CreateTextFormat(
            fam.c_str(), nullptr, DWRITE_FONT_WEIGHT_MEDIUM,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 11.5f * s,
            L"en-us", fmtLabel.put());
        g_gfx.dwriteFactory->CreateTextFormat(
            fam.c_str(), nullptr, DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 10.5f * s,
            L"en-us", fmtSub.put());

        auto prep = [&](IDWriteTextFormat* f, DWRITE_TEXT_ALIGNMENT al) {
            if (!f) return;
            f->SetTextAlignment(al);
            f->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            f->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            winrt::com_ptr<IDWriteInlineObject> ell;
            if (SUCCEEDED(g_gfx.dwriteFactory->CreateEllipsisTrimmingSign(
                    f, ell.put())) &&
                ell) {
                DWRITE_TRIMMING tr{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
                f->SetTrimming(&tr, ell.get());
            }
        };
        prep(fmtSearch.get(), DWRITE_TEXT_ALIGNMENT_LEADING);
        prep(fmtPill.get(), DWRITE_TEXT_ALIGNMENT_CENTER);
        prep(fmtLabel.get(), g_appsLauncher.listView
                                 ? DWRITE_TEXT_ALIGNMENT_LEADING
                                 : DWRITE_TEXT_ALIGNMENT_CENTER);
        prep(fmtSub.get(), DWRITE_TEXT_ALIGNMENT_TRAILING);

        const float pad = 16.0f * s;
        float y = pad;

        // 1. Spotlight-style Search Pill + Grid/List Toggle Pill
        float toggleW = 66.0f * s;
        float searchH = 38.0f * s;
        g_appsLauncher.searchRect = {pad, y, W - pad - toggleW - 10 * s,
                                     y + searchH};
        g_appsLauncher.viewToggleRect = {W - pad - toggleW, y, W - pad,
                                         y + searchH};

        D2D1_ROUNDED_RECT srRR = D2D1::RoundedRect(
            g_appsLauncher.searchRect, searchH * 0.5f, searchH * 0.5f);
        br->SetColor(dark ? D2D1::ColorF(0.05f, 0.07f, 0.11f, 0.44f)
                          : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.54f));
        dc->FillRoundedRectangle(srRR, br.get());
        br->SetColor(dark ? D2D1::ColorF(0.92f, 0.96f, 1.0f, 0.24f)
                          : D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.12f));
        dc->DrawRoundedRectangle(srRR, br.get(), 1.0f * s);

        // Magnifying glass vector glyph
        float gx = g_appsLauncher.searchRect.left + 19.0f * s;
        float gy = g_appsLauncher.searchRect.top + searchH * 0.5f - 1.0f * s;
        br->SetColor(fgSub);
        dc->DrawEllipse(D2D1::Ellipse({gx, gy}, 5.5f * s, 5.5f * s), br.get(),
                        1.5f * s);
        dc->DrawLine({gx + 4.0f * s, gy + 4.0f * s},
                     {gx + 8.0f * s, gy + 8.0f * s}, br.get(), 1.6f * s);

        // Search text or placeholder
        std::wstring sTxt = g_appsLauncher.query.empty()
                                ? L"Search Applications\u2026"
                                : (g_appsLauncher.query + L"|");
        br->SetColor(g_appsLauncher.query.empty() ? fgSub : fg);
        D2D1_RECT_F sTextRc{g_appsLauncher.searchRect.left + 36.0f * s,
                            g_appsLauncher.searchRect.top,
                            g_appsLauncher.searchRect.right - 34.0f * s,
                            g_appsLauncher.searchRect.bottom};
        dc->DrawText(sTxt.c_str(), static_cast<UINT32>(sTxt.size()),
                     fmtSearch.get(), sTextRc, br.get());

        // Clear 'x' inside search bar when non-empty
        g_appsLauncher.clearBtnRect = {};
        if (!g_appsLauncher.query.empty()) {
            float cr = 9.0f * s;
            float cx = g_appsLauncher.searchRect.right - 18.0f * s;
            float cy = (g_appsLauncher.searchRect.top +
                        g_appsLauncher.searchRect.bottom) *
                       0.5f;
            g_appsLauncher.clearBtnRect = {cx - cr, cy - cr, cx + cr, cy + cr};
            br->SetColor(g_appsLauncher.hoveredClear
                             ? D2D1::ColorF(1, 1, 1, 0.30f)
                             : D2D1::ColorF(1, 1, 1, 0.14f));
            dc->FillEllipse(D2D1::Ellipse({cx, cy}, cr, cr), br.get());
            br->SetColor(fg);
            dc->DrawLine({cx - 3.5f * s, cy - 3.5f * s},
                         {cx + 3.5f * s, cy + 3.5f * s}, br.get(), 1.3f * s);
            dc->DrawLine({cx + 3.5f * s, cy - 3.5f * s},
                         {cx - 3.5f * s, cy + 3.5f * s}, br.get(), 1.3f * s);
        }

        // Grid / List view toggle pill
        D2D1_ROUNDED_RECT vtRR = D2D1::RoundedRect(
            g_appsLauncher.viewToggleRect, searchH * 0.5f, searchH * 0.5f);
        br->SetColor(g_appsLauncher.hoveredViewToggle
                         ? accCol
                         : (dark ? D2D1::ColorF(1, 1, 1, 0.10f)
                                 : D2D1::ColorF(0, 0, 0, 0.08f)));
        dc->FillRoundedRectangle(vtRR, br.get());
        br->SetColor(g_appsLauncher.hoveredViewToggle
                         ? D2D1::ColorF(1, 1, 1, 0.98f)
                         : fg);
        PCWSTR vtLbl = g_appsLauncher.listView ? L"Grid" : L"List";
        dc->DrawText(vtLbl, static_cast<UINT32>(wcslen(vtLbl)), fmtPill.get(),
                     g_appsLauncher.viewToggleRect, br.get());
        y += searchH + 12.0f * s;

        // 2. Tahoe Category Filter Pills
        PCWSTR catNames[6] = {L"All",        L"Productivity",  L"Utilities",
                              L"Creativity", L"Entertainment", L"Social"};
        float catH = 26.0f * s;
        float cx = pad;
        for (int c = 0; c < 6; ++c) {
            float cw = (c == 0) ? 44.0f * s
                                : (static_cast<float>(wcslen(catNames[c])) *
                                       6.8f * s +
                                   22.0f * s);
            D2D1_RECT_F rc{cx, y, cx + cw, y + catH};
            g_appsLauncher.catRects[c] = rc;
            bool act = (g_appsLauncher.activeCategory == c);
            bool hov = (g_appsLauncher.hoveredCat == c);
            if (act || hov) {
                br->SetColor(act ? accCol
                                 : (dark ? D2D1::ColorF(1, 1, 1, 0.13f)
                                         : D2D1::ColorF(0, 0, 0, 0.10f)));
            } else {
                br->SetColor(dark ? D2D1::ColorF(1, 1, 1, 0.06f)
                                  : D2D1::ColorF(0, 0, 0, 0.05f));
            }
            dc->FillRoundedRectangle(
                D2D1::RoundedRect(rc, catH * 0.5f, catH * 0.5f), br.get());
            br->SetColor(act ? D2D1::ColorF(1, 1, 1, 0.98f) : fg);
            dc->DrawText(catNames[c], static_cast<UINT32>(wcslen(catNames[c])),
                         fmtPill.get(), rc, br.get());
            cx += cw + 7.0f * s;
        }
        y += catH + 12.0f * s;

        // Divider hairline
        br->SetColor(dark ? D2D1::ColorF(0.90f, 0.95f, 1.0f, 0.12f)
                          : D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.08f));
        dc->DrawLine({pad, y}, {W - pad, y}, br.get(), 0.85f * s);
        y += 8.0f * s;

        // 3. Scrollable Apps Viewport
        const float footerH = 24.0f * s;
        D2D1_RECT_F vp{pad, y, W - pad, H - footerH - 6.0f * s};
        g_appsLauncher.listViewport = vp;
        const float vpW = vp.right - vp.left;
        const float vpH = std::max(40.0f, vp.bottom - vp.top);

        const size_t n = g_appsLauncher.filteredIdx.size();
        const int cols = g_appsLauncher.listView ? 1 : 5;
        const float cellGap = g_appsLauncher.listView ? (4.0f * s) : (10.0f * s);
        const float cellW =
            g_appsLauncher.listView
                ? (vpW - 10.0f * s)
                : ((vpW - 10.0f * s - cellGap * (cols - 1)) / cols);
        const float cellH = g_appsLauncher.listView ? (40.0f * s) : (92.0f * s);
        int rows = (static_cast<int>(n) + cols - 1) / cols;
        float contentH =
            (rows > 0) ? (rows * cellH + (rows - 1) * cellGap) : 0.0f;
        g_appsLauncher.maxScrollY = std::max(0.0f, contentH - vpH);
        g_appsLauncher.scrollY =
            std::clamp(g_appsLauncher.scrollY, 0.0f, g_appsLauncher.maxScrollY);

        dc->PushAxisAlignedClip(&vp, D2D1_ANTIALIAS_MODE_ALIASED);
        if (n == 0) {
            br->SetColor(fgSub);
            PCWSTR noneTxt = L"No matching applications";
            dc->DrawText(noneTxt, static_cast<UINT32>(wcslen(noneTxt)),
                         fmtPill.get(), vp, br.get());
        } else {
            PCWSTR catLabels[6] = {L"All",        L"Productivity",  L"Utilities",
                                   L"Creativity", L"Entertainment", L"Social"};
            for (size_t fi = 0; fi < n; ++fi) {
                auto& app = g_appsLauncher.allApps[g_appsLauncher.filteredIdx[fi]];
                int r = static_cast<int>(fi) / cols;
                int c = static_cast<int>(fi) % cols;
                float rx = vp.left + c * (cellW + cellGap);
                float ry = vp.top + r * (cellH + cellGap) - g_appsLauncher.scrollY;
                D2D1_RECT_F rc{rx, ry, rx + cellW, ry + cellH};
                app.hitRect = rc;

                // Cull off-screen rows cleanly (huge speedup + lazy icon load!)
                if (rc.bottom < vp.top - 4.0f || rc.top > vp.bottom + 4.0f) {
                    app.hitRect = {};
                    continue;
                }

                if (!app.iconLoaded) {
                    app.iconLoaded = true;
                    if (!app.path.empty()) {
                        app.icon = LoadIconFromPath(app.path, 96);
                    } else if (!app.aumid.empty()) {
                        app.icon = LoadIconFromAumid(app.aumid, 96);
                    }
                }

                bool hov = (static_cast<int>(fi) == g_appsLauncher.hoveredIdx);
                bool sel = (static_cast<int>(fi) == g_appsLauncher.selectedIdx &&
                            g_appsLauncher.hoveredIdx == -1);
                if (hov || sel) {
                    br->SetColor(hov ? accCol
                                     : (dark ? D2D1::ColorF(1, 1, 1, 0.14f)
                                             : D2D1::ColorF(0, 0, 0, 0.10f)));
                    dc->FillRoundedRectangle(
                        D2D1::RoundedRect(rc, 12 * s, 12 * s), br.get());
                }

                if (g_appsLauncher.listView) {
                    float icoSz = 26.0f * s;
                    float ix = rc.left + 10.0f * s;
                    float iy = rc.top + (cellH - icoSz) * 0.5f;
                    if (app.icon) {
                        winrt::com_ptr<ID2D1Bitmap> bmp;
                        if (SUCCEEDED(dc->CreateBitmapFromWicBitmap(
                                app.icon.get(), nullptr, bmp.put())) &&
                            bmp) {
                            dc->DrawBitmap(
                                bmp.get(),
                                D2D1::RectF(ix, iy, ix + icoSz, iy + icoSz),
                                1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                        }
                    }
                    br->SetColor(hov ? D2D1::ColorF(1, 1, 1, 0.98f) : fg);
                    dc->DrawText(app.name.c_str(),
                                 static_cast<UINT32>(app.name.size()),
                                 fmtLabel.get(),
                                 D2D1::RectF(ix + icoSz + 12 * s, rc.top,
                                             rc.right - 100 * s, rc.bottom),
                                 br.get());
                    br->SetColor(hov ? D2D1::ColorF(1, 1, 1, 0.80f) : fgSub);
                    PCWSTR cl = catLabels[std::clamp(app.category, 0, 5)];
                    dc->DrawText(cl, static_cast<UINT32>(wcslen(cl)),
                                 fmtSub.get(),
                                 D2D1::RectF(rc.right - 96 * s, rc.top,
                                             rc.right - 12 * s, rc.bottom),
                                 br.get());
                } else {
                    float icoSz = 48.0f * s;
                    float ix = rc.left + (cellW - icoSz) * 0.5f;
                    float iy = rc.top + 9.0f * s;
                    if (app.icon) {
                        winrt::com_ptr<ID2D1Bitmap> bmp;
                        if (SUCCEEDED(dc->CreateBitmapFromWicBitmap(
                                app.icon.get(), nullptr, bmp.put())) &&
                            bmp) {
                            dc->DrawBitmap(
                                bmp.get(),
                                D2D1::RectF(ix, iy, ix + icoSz, iy + icoSz),
                                1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                        }
                    }
                    br->SetColor(hov ? D2D1::ColorF(1, 1, 1, 0.98f) : fg);
                    dc->DrawText(app.name.c_str(),
                                 static_cast<UINT32>(app.name.size()),
                                 fmtLabel.get(),
                                 D2D1::RectF(rc.left + 4 * s, iy + icoSz + 4 * s,
                                             rc.right - 4 * s, rc.bottom - 4 * s),
                                 br.get());
                }
            }
        }
        dc->PopAxisAlignedClip();

        // Scroll thumb pill
        if (g_appsLauncher.maxScrollY > 1.0f) {
            float fracVis = vpH / std::max(vpH, contentH);
            float thumbH = std::max(26.0f * s, vpH * fracVis);
            float fracPos = g_appsLauncher.scrollY / g_appsLauncher.maxScrollY;
            float thumbY = vp.top + fracPos * (vpH - thumbH);
            br->SetColor(dark ? D2D1::ColorF(1, 1, 1, 0.28f)
                              : D2D1::ColorF(0, 0, 0, 0.22f));
            dc->FillRoundedRectangle(
                D2D1::RoundedRect(D2D1::RectF(vp.right - 5 * s, thumbY,
                                              vp.right - 1 * s, thumbY + thumbH),
                                  2 * s, 2 * s),
                br.get());
        }

        // Subtle footer hint
        br->SetColor(fgSub);
        std::wstring fHint =
            std::to_wstring(n) + L" apps  \u2022  Type to search  \u2022  Right-click any app to Pin to Dock";
        fmtSub->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        dc->DrawText(fHint.c_str(), static_cast<UINT32>(fHint.size()),
                     fmtSub.get(),
                     D2D1::RectF(pad, H - footerH - 2 * s, W - pad, H - 4 * s),
                     br.get());
    });
}

LRESULT CALLBACK TahoeAppsLauncherWndProc(HWND hwnd, UINT msg, WPARAM wp,
                                          LPARAM lp) {
    try {
        switch (msg) {
            case WM_ACTIVATE:
                if (LOWORD(wp) == WA_INACTIVE && !g_menus.mainPopup.hwnd) {
                    CloseTahoeAppsLauncher();
                }
                return 0;
            case WM_CHAR: {
                wchar_t ch = static_cast<wchar_t>(wp);
                if (ch == 8) {  // Backspace
                    if (!g_appsLauncher.query.empty()) {
                        g_appsLauncher.query.pop_back();
                        RebuildTahoeAppsFilter();
                        RenderTahoeAppsLauncher();
                    }
                } else if (ch >= 32) {
                    if (g_appsLauncher.query.size() < 48) {
                        g_appsLauncher.query.push_back(ch);
                        RebuildTahoeAppsFilter();
                        RenderTahoeAppsLauncher();
                    }
                }
                return 0;
            }
            case WM_KEYDOWN: {
                if (wp == VK_ESCAPE) {
                    if (!g_appsLauncher.query.empty()) {
                        g_appsLauncher.query.clear();
                        RebuildTahoeAppsFilter();
                        RenderTahoeAppsLauncher();
                    } else {
                        CloseTahoeAppsLauncher();
                    }
                    return 0;
                }
                if (wp == VK_RETURN && !g_appsLauncher.filteredIdx.empty()) {
                    int idx = (g_appsLauncher.hoveredIdx >= 0)
                                  ? g_appsLauncher.hoveredIdx
                                  : std::max(0, g_appsLauncher.selectedIdx);
                    if (idx < static_cast<int>(g_appsLauncher.filteredIdx.size())) {
                        LaunchTahoeEntry(
                            g_appsLauncher
                                .allApps[g_appsLauncher.filteredIdx[idx]]);
                    }
                    return 0;
                }
                int n = static_cast<int>(g_appsLauncher.filteredIdx.size());
                if (n > 0 && (wp == VK_LEFT || wp == VK_RIGHT || wp == VK_UP ||
                              wp == VK_DOWN)) {
                    int cols = g_appsLauncher.listView ? 1 : 5;
                    int cur = std::max(0, g_appsLauncher.selectedIdx);
                    if (wp == VK_RIGHT) cur = std::min(n - 1, cur + 1);
                    if (wp == VK_LEFT) cur = std::max(0, cur - 1);
                    if (wp == VK_DOWN) cur = std::min(n - 1, cur + cols);
                    if (wp == VK_UP) cur = std::max(0, cur - cols);
                    g_appsLauncher.selectedIdx = cur;
                    g_appsLauncher.hoveredIdx = -1;
                    RenderTahoeAppsLauncher();
                    return 0;
                }
                return 0;
            }
            case WM_MOUSEWHEEL: {
                int delta = GET_WHEEL_DELTA_WPARAM(wp);
                g_appsLauncher.scrollY = std::clamp(
                    g_appsLauncher.scrollY -
                        static_cast<float>(delta) * 0.45f * g_dock.scale,
                    0.0f, g_appsLauncher.maxScrollY);
                RenderTahoeAppsLauncher();
                return 0;
            }
            case WM_MOUSEMOVE: {
                float lx = static_cast<float>(static_cast<short>(LOWORD(lp)));
                float ly = static_cast<float>(static_cast<short>(HIWORD(lp)));
                int newCat = -1;
                for (int c = 0; c < 6; ++c) {
                    if (PointInRectF(g_appsLauncher.catRects[c], lx, ly)) {
                        newCat = c;
                        break;
                    }
                }
                bool newVt = PointInRectF(g_appsLauncher.viewToggleRect, lx, ly);
                bool newClr = PointInRectF(g_appsLauncher.clearBtnRect, lx, ly);
                int newHov = -1;
                if (PointInRectF(g_appsLauncher.listViewport, lx, ly)) {
                    for (size_t fi = 0; fi < g_appsLauncher.filteredIdx.size();
                         ++fi) {
                        const auto& a =
                            g_appsLauncher.allApps[g_appsLauncher.filteredIdx[fi]];
                        if (PointInRectF(a.hitRect, lx, ly)) {
                            newHov = static_cast<int>(fi);
                            break;
                        }
                    }
                }
                if (newCat != g_appsLauncher.hoveredCat ||
                    newVt != g_appsLauncher.hoveredViewToggle ||
                    newClr != g_appsLauncher.hoveredClear ||
                    newHov != g_appsLauncher.hoveredIdx) {
                    g_appsLauncher.hoveredCat = newCat;
                    g_appsLauncher.hoveredViewToggle = newVt;
                    g_appsLauncher.hoveredClear = newClr;
                    g_appsLauncher.hoveredIdx = newHov;
                    RenderTahoeAppsLauncher();
                }
                return 0;
            }
            case WM_LBUTTONUP: {
                float lx = static_cast<float>(static_cast<short>(LOWORD(lp)));
                float ly = static_cast<float>(static_cast<short>(HIWORD(lp)));
                if (PointInRectF(g_appsLauncher.clearBtnRect, lx, ly)) {
                    g_appsLauncher.query.clear();
                    RebuildTahoeAppsFilter();
                    RenderTahoeAppsLauncher();
                    return 0;
                }
                if (PointInRectF(g_appsLauncher.viewToggleRect, lx, ly)) {
                    g_appsLauncher.listView = !g_appsLauncher.listView;
                    g_appsLauncher.scrollY = 0.0f;
                    RenderTahoeAppsLauncher();
                    return 0;
                }
                for (int c = 0; c < 6; ++c) {
                    if (PointInRectF(g_appsLauncher.catRects[c], lx, ly)) {
                        g_appsLauncher.activeCategory = c;
                        RebuildTahoeAppsFilter();
                        RenderTahoeAppsLauncher();
                        return 0;
                    }
                }
                if (PointInRectF(g_appsLauncher.listViewport, lx, ly)) {
                    for (size_t fi = 0; fi < g_appsLauncher.filteredIdx.size();
                         ++fi) {
                        const auto& a =
                            g_appsLauncher.allApps[g_appsLauncher.filteredIdx[fi]];
                        if (PointInRectF(a.hitRect, lx, ly)) {
                            LaunchTahoeEntry(a);
                            return 0;
                        }
                    }
                }
                return 0;
            }
            case WM_RBUTTONUP: {
                float lx = static_cast<float>(static_cast<short>(LOWORD(lp)));
                float ly = static_cast<float>(static_cast<short>(HIWORD(lp)));
                if (PointInRectF(g_appsLauncher.listViewport, lx, ly)) {
                    for (size_t fi = 0; fi < g_appsLauncher.filteredIdx.size();
                         ++fi) {
                        auto a =
                            g_appsLauncher.allApps[g_appsLauncher.filteredIdx[fi]];
                        if (PointInRectF(a.hitRect, lx, ly)) {
                            POINT pt{static_cast<LONG>(lx),
                                     static_cast<LONG>(ly)};
                            ClientToScreen(hwnd, &pt);
                            std::vector<MenuEntry> m;
                            m.push_back({MenuEntry::Type::Action, L"Open",
                                         false, true,
                                         [a] { LaunchTahoeEntry(a); }, {}});
                            bool hasDock = FindWindowW(L"WindhawkMacOS27DockWnd", nullptr) != nullptr;
                            m.push_back({MenuEntry::Type::Action,
                                         hasDock ? L"Pin to Dock" : L"Pin to Dock (Dock Not Running)",
                                         false, hasDock,
                                         [a] { PinTahoeEntryToDock(a); }, {}});
                            if (!a.path.empty()) {
                                m.push_back({MenuEntry::Type::Action,
                                             L"Show in File Explorer", false,
                                             true,
                                             [p = a.path] {
                                                 CloseTahoeAppsLauncher();
                                                 ShowInFileExplorer(p);
                                             },
                                             {}});
                            }
                            OpenGlassMenuPopup(g_menus.mainPopup, std::move(m),
                                               pt, false, false);
                            return 0;
                        }
                    }
                }
                return 0;
            }
        }
    } catch (...) {
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void ToggleTahoeAppsLauncher(const D2D1_RECT_F& iconRectLocal) {
    if (g_appsLauncher.open) {
        CloseTahoeAppsLauncher();
        return;
    }
    CloseAllMenus();
    if (!g_dock.hwnd) {
        return;
    }
    POINT curPt{};
    HMONITOR mon = GetCursorPos(&curPt)
                       ? MonitorFromPoint(curPt, MONITOR_DEFAULTTONEAREST)
                       : GetPrimaryMonitor();
    if (!mon) mon = GetPrimaryMonitor();
    g_dock.monitor = mon;
    MONITORINFO mi{sizeof(mi)};
    if (GetMonitorInfoW(mon, &mi)) {
        g_dock.monitorRect = mi.rcMonitor;
    }
    g_dock.scale = GetMonitorDpiScale(mon);
    UpdateGlassAppearance();

    EnsureTahoeAppsCatalogLoaded(false);
    g_appsLauncher.query.clear();
    g_appsLauncher.activeCategory = 0;
    RebuildTahoeAppsFilter();

    const float s = g_dock.scale;
    const RECT& mr = g_dock.monitorRect;
    float w = std::min(std::ceil(652.0f * s),
                       static_cast<float>(mr.right - mr.left) - 32.0f * s);
    float h = std::min(std::ceil(510.0f * s),
                       static_cast<float>(mr.bottom - mr.top) - 110.0f * s);
    int px = mr.left + ((mr.right - mr.left) - static_cast<int>(w)) / 2;
    int py = mr.top + ((mr.bottom - mr.top) - static_cast<int>(h)) / 2 -
             static_cast<int>(20.0f * s);
    py = std::max(static_cast<int>(mr.top + 42.0f * s), py);
    (void)iconRectLocal;

    HMODULE hSelf = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&ToggleTahoeAppsLauncher),
                       &hSelf);
    static bool s_regApps = false;
    if (!s_regApps) {
        WNDCLASSEXW wc{.cbSize = sizeof(wc),
                       .lpfnWndProc = TahoeAppsLauncherWndProc,
                       .hInstance = hSelf,
                       .hCursor = LoadCursorW(nullptr, IDC_ARROW),
                       .lpszClassName = kAppsLauncherClassName};
        RegisterClassExW(&wc);
        s_regApps = true;
    }

    g_appsLauncher.hwnd = CreateWindowExW(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_LAYERED | WS_EX_TOPMOST |
            WS_EX_TOOLWINDOW,
        kAppsLauncherClassName, L"Apps", WS_POPUP, px, py, static_cast<int>(w),
        static_cast<int>(h), nullptr, nullptr, hSelf, nullptr);
    if (!g_appsLauncher.hwnd) {
        return;
    }
    g_appsLauncher.widthPx = w;
    g_appsLauncher.heightPx = h;
    g_appsLauncher.open = true;
    g_dock.modalActive = true;

    InitGlassPopupWindowShell(
        g_appsLauncher.hwnd, w, h, 22.0f * s, px, py,
        g_dock.darkMode ? 0.16f : 0.14f, g_appsLauncher.target,
        g_appsLauncher.root, g_appsLauncher.glassContainer,
        g_appsLauncher.clipGeom, g_appsLauncher.fallbackBackdrop,
        g_appsLauncher.fallbackTint, g_appsLauncher.liquidPlate,
        g_appsLauncher.uiSurface, g_appsLauncher.uiBrush,
        g_appsLauncher.uiVisual);
    if (g_settings.glassRefraction != GlassRefraction::Off &&
        !g_liquidCapture.frozen) {
        RECT wrc{px, py, px + static_cast<int>(w), py + static_cast<int>(h)};
        InflateRect(&wrc, 24, 24);
        UpdateLiquidCaptureWatchedRegions({wrc});
        g_liquidCapture.watching.store(true, std::memory_order_relaxed);
        EnsureLiquidCaptureReady();
        SetTimer(g_dock.hwnd, kTimerGlassMaintenance, 16, nullptr);
    }
    RenderTahoeAppsLauncher();
    ShowWindow(g_appsLauncher.hwnd, SW_SHOW);
    SetForegroundWindow(g_appsLauncher.hwnd);
}


void RefreshOpticalGlassPlatesOnly() {
    if (!g_appsLauncher.open || !g_appsLauncher.hwnd ||
        !WantOpticalLiquidGlass() || g_liquidCapture.frozen) {
        return;
    }
    if (!EnsureLiquidCaptureReady()) {
        return;
    }
    RECT rc{};
    if (!GetWindowRect(g_appsLauncher.hwnd, &rc)) {
        return;
    }
    RECT watchRc = rc;
    InflateRect(&watchRc, 24, 24);
    UpdateLiquidCaptureWatchedRegions({watchRc});

    LiquidGlassStyleParams lp{};
    lp.screenX = static_cast<float>(rc.left);
    lp.screenY = static_cast<float>(rc.top);
    lp.widthPx = g_appsLauncher.widthPx;
    lp.heightPx = g_appsLauncher.heightPx;
    lp.cornerRadiusPx = 22.0f * g_dock.scale;
    lp.scale = g_dock.scale;
    lp.darkMode = g_dock.darkMode;
    lp.useSquircle = true;
    lp.tintBoost = g_dock.darkMode ? 0.16f : 0.14f;
    lp.blurMul = 1.65f;
    lp.rimDepthMul = 0.85f;
    lp.strengthMul = 0.90f;

    bool attached = g_appsLauncher.liquidPlate.visual &&
                    g_appsLauncher.glassContainer;
    if (RenderLiquidGlassPlate(g_appsLauncher.liquidPlate, lp) &&
        g_appsLauncher.liquidPlate.visual) {
        if (!attached && g_appsLauncher.glassContainer) {
            g_appsLauncher.glassContainer.Children().InsertAbove(
                g_appsLauncher.liquidPlate.visual,
                g_appsLauncher.fallbackTint);
        }
        if (g_appsLauncher.fallbackBackdrop) {
            g_appsLauncher.fallbackBackdrop.IsVisible(false);
        }
        if (g_appsLauncher.fallbackTint) {
            g_appsLauncher.fallbackTint.IsVisible(false);
        }
    }
}

void UpdateGlassAppearance() {
    if (g_settings.appearance == AppearanceMode::Dark) {
        g_dock.darkMode = true;
    } else if (g_settings.appearance == AppearanceMode::Light) {
        g_dock.darkMode = false;
    } else {
        g_dock.darkMode = IsSystemDarkMode();
    }
}

void LoadSettings() {
    DockSettings s;
    std::wstring app = ReadWhStringSetting(L"appearance");
    if (app == L"dark") {
        s.appearance = AppearanceMode::Dark;
    } else if (app == L"light") {
        s.appearance = AppearanceMode::Light;
    } else {
        s.appearance = AppearanceMode::Auto;
    }

    std::wstring gs = ReadWhStringSetting(L"glassStyle");
    s.glassStyle = (gs == L"tinted") ? GlassStyle::Tinted : GlassStyle::Clear;

    std::wstring ref = ReadWhStringSetting(L"glassRefraction");
    if (ref == L"reflection") {
        s.glassRefraction = GlassRefraction::Reflection;
    } else if (ref == L"stretch") {
        s.glassRefraction = GlassRefraction::Stretch;
    } else if (ref == L"off") {
        s.glassRefraction = GlassRefraction::Off;
    } else {
        s.glassRefraction = GlassRefraction::Lens;
    }

    s.glassEdgeDepth = std::clamp(Wh_GetIntSetting(L"glassEdgeDepth"), 0, 100);
    s.glassEdgeStrength =
        std::clamp(Wh_GetIntSetting(L"glassEdgeStrength"), 0, 100);
    s.glassTint = std::clamp(Wh_GetIntSetting(L"glassTint"), 0, 100);
    s.glassBlur = std::clamp(Wh_GetIntSetting(L"glassBlur"), 0, 100);
    s.glassBrightnessLimit =
        std::clamp(Wh_GetIntSetting(L"glassBrightnessLimit"), 0, 100);

    std::wstring hk = ReadWhStringSetting(L"hotkeyCombo");
    if (hk == L"winAltA") {
        s.hotkeyCombo = HotkeyCombo::WinAltA;
    } else if (hk == L"ctrlShiftA") {
        s.hotkeyCombo = HotkeyCombo::CtrlShiftA;
    } else if (hk == L"altSpace") {
        s.hotkeyCombo = HotkeyCombo::AltSpace;
    } else if (hk == L"none") {
        s.hotkeyCombo = HotkeyCombo::None;
    } else {
        s.hotkeyCombo = HotkeyCombo::WinShiftA;
    }

    s.defaultListView = (ReadWhStringSetting(L"defaultView") == L"list");
    g_settings = std::move(s);
    UpdateGlassAppearance();
}

void ApplyAppsHotkey(HWND hostHwnd) {
    UnregisterHotKey(hostHwnd, kHotkeyToggleApps);
    UINT mods = 0;
    UINT vk = 0;
    switch (g_settings.hotkeyCombo) {
        case HotkeyCombo::WinShiftA:
            mods = MOD_WIN | MOD_SHIFT | MOD_NOREPEAT;
            vk = 'A';
            break;
        case HotkeyCombo::WinAltA:
            mods = MOD_WIN | MOD_ALT | MOD_NOREPEAT;
            vk = 'A';
            break;
        case HotkeyCombo::CtrlShiftA:
            mods = MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT;
            vk = 'A';
            break;
        case HotkeyCombo::AltSpace:
            mods = MOD_ALT | MOD_NOREPEAT;
            vk = VK_SPACE;
            break;
        case HotkeyCombo::None:
            return;
    }
    RegisterHotKey(hostHwnd, kHotkeyToggleApps, mods, vk);
}

HWND GetGlassNotifyHwnd() {
    return g_dock.hwnd;
}

void ApplyCaptureExclusionToAllModWindows(bool excluded) {
    DWORD affinity = excluded ? kWdaExcludeFromCapture : WDA_NONE;
    auto applyOne = [affinity](HWND h) {
        if (h && IsWindow(h)) {
            SetWindowDisplayAffinity(h, affinity);
        }
    };
    applyOne(g_appsLauncher.hwnd);
    applyOne(g_menus.mainPopup.hwnd);
    applyOne(g_menus.subPopup.hwnd);
}

void OnGraphicsDeviceLost() {
    ShutdownLiquidCapture();
    g_appsLauncher.liquidPlate.ResetGpuResources();
    g_menus.mainPopup.liquidPlate.ResetGpuResources();
    g_menus.subPopup.liquidPlate.ResetGpuResources();
    if (g_appsLauncher.open) {
        RefreshOpticalGlassPlatesOnly();
        RenderTahoeAppsLauncher();
    }
}

winrt::Windows::System::DispatcherQueueController g_dqController{nullptr};
HANDLE g_uiThread = nullptr;
DWORD g_uiThreadId = 0;

void ShutdownDispatcherQueue() {
    if (!g_dqController) {
        return;
    }
    try {
        auto controller = std::move(g_dqController);
        auto shutdown = controller.ShutdownQueueAsync();
        for (int i = 0;
             i < 100 && shutdown.Status() == wf::AsyncStatus::Started; ++i) {
            MsgWaitForMultipleObjects(0, nullptr, FALSE, 10, QS_ALLINPUT);
            MSG msg;
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    } catch (...) {
    }
}

LRESULT CALLBACK AppsHostWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    try {
        if (g_msgToggleApps && msg == g_msgToggleApps) {
            ToggleTahoeAppsLauncher(D2D1_RECT_F{});
            return 0;
        }
        switch (msg) {
            case WM_HOTKEY:
                if (wp == kHotkeyToggleApps) {
                    ToggleTahoeAppsLauncher(D2D1_RECT_F{});
                    return 0;
                }
                break;

            case WM_TIMER:
                if (wp == kTimerGlassMaintenance) {
                    if (g_appsLauncher.open && WantOpticalLiquidGlass() &&
                        !g_liquidCapture.frozen) {
                        uint64_t prevFrame = g_liquidCapture.frameId;
                        if (EnsureLiquidCaptureReady() &&
                            (g_liquidCapture.frameId != prevFrame ||
                             !g_appsLauncher.liquidPlate.active)) {
                            RefreshOpticalGlassPlatesOnly();
                        }
                    } else {
                        KillTimer(hwnd, kTimerGlassMaintenance);
                    }
                    return 0;
                }
                break;

            case kWmAppGlassFrame:
                g_liquidCapture.framePosted.store(false,
                                                  std::memory_order_release);
                if (g_appsLauncher.open && EnsureLiquidCaptureReady()) {
                    RefreshOpticalGlassPlatesOnly();
                }
                return 0;

            case kWmAppDeferredAction: {
                auto fn = std::move(g_menus.pendingAction);
                g_menus.pendingAction = nullptr;
                if (fn) {
                    fn();
                }
                return 0;
            }

            case kWmAppSettings:
                LoadSettings();
                ApplyAppsHotkey(hwnd);
                g_liquidCapture.consecutiveFailures = 0;
                g_liquidCapture.unsupported = false;
                if (g_appsLauncher.open) {
                    RefreshOpticalGlassPlatesOnly();
                    RenderTahoeAppsLauncher();
                }
                return 0;

            case WM_SETTINGCHANGE:
                if (lp && _wcsicmp(reinterpret_cast<PCWSTR>(lp),
                                   L"ImmersiveColorSet") == 0) {
                    UpdateGlassAppearance();
                    if (g_appsLauncher.open) {
                        RefreshOpticalGlassPlatesOnly();
                        RenderTahoeAppsLauncher();
                    }
                }
                return 0;

            case WM_DWMCOLORIZATIONCOLORCHANGED:
                UpdateGlassAppearance();
                if (g_appsLauncher.open) {
                    RefreshOpticalGlassPlatesOnly();
                    RenderTahoeAppsLauncher();
                }
                return 0;

            case WM_DISPLAYCHANGE:
            case WM_DPICHANGED:
                CloseAllMenus();
                if (g_appsLauncher.open) {
                    CloseTahoeAppsLauncher();
                } else {
                    ShutdownLiquidCapture();
                }
                g_appsLauncher.catalogLoaded = false;
                g_dock.scale = GetMonitorDpiScale(GetPrimaryMonitor());
                return 0;

            case kWmAppQuit:
                g_quitting = true;
                CloseAllMenus();
                CloseTahoeAppsLauncher();
                UnregisterHotKey(hwnd, kHotkeyToggleApps);
                DestroyWindow(hwnd);
                return 0;

            case WM_DESTROY:
                KillTimer(hwnd, kTimerGlassMaintenance);
                PostQuitMessage(0);
                return 0;
        }
    } catch (...) {
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

DWORD WINAPI UiThreadProc(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HRESULT oleHr = OleInitialize(nullptr);

    HMODULE hCoreMsg = LoadLibraryW(L"CoreMessaging.dll");
    using CreateDispatcherQueueController_t = HRESULT(WINAPI*)(
        DispatcherQueueOptions,
        ABI::Windows::System::IDispatcherQueueController**);
    auto createDqc = hCoreMsg
                         ? reinterpret_cast<CreateDispatcherQueueController_t>(
                               GetProcAddress(hCoreMsg,
                                              "CreateDispatcherQueueController"))
                         : nullptr;

    HMODULE hSelf = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&UiThreadProc), &hSelf);

    bool ok = false;
    try {
        DispatcherQueueOptions opts{
            .dwSize = sizeof(opts),
            .threadType = DQTYPE_THREAD_CURRENT,
            .apartmentType = DQTAT_COM_NONE,
        };
        if (createDqc &&
            SUCCEEDED(createDqc(
                opts,
                reinterpret_cast<
                    ABI::Windows::System::IDispatcherQueueController**>(
                    winrt::put_abi(g_dqController))))) {
            wuc::Compositor compositor;
            if (InitGraphics(compositor)) {
                g_dock.hostBackdrop = compositor.CreateHostBackdropBrush();
                g_dock.monitor = GetPrimaryMonitor();
                MONITORINFO mi{sizeof(mi)};
                if (g_dock.monitor && GetMonitorInfoW(g_dock.monitor, &mi)) {
                    g_dock.monitorRect = mi.rcMonitor;
                }
                g_dock.scale = GetMonitorDpiScale(g_dock.monitor);
                g_appsLauncher.listView = g_settings.defaultListView;

                WNDCLASSEXW wc{
                    .cbSize = sizeof(wc),
                    .lpfnWndProc = AppsHostWndProc,
                    .hInstance = hSelf,
                    .lpszClassName = kAppsHostClassName,
                };
                RegisterClassExW(&wc);

                // Top-level invisible WS_POPUP so FindWindowW locates us from companion mods.
                g_dock.hwnd = CreateWindowExW(
                    WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kAppsHostClassName,
                    L"macOS 26 Tahoe Apps Launcher Host", WS_POPUP, 0, 0, 0, 0,
                    nullptr, nullptr, hSelf, nullptr);
                if (g_dock.hwnd) {
                    g_msgToggleApps =
                        RegisterWindowMessageW(L"WindhawkMacOSTahoe_ToggleApps");
                    ApplyAppsHotkey(g_dock.hwnd);
                    ok = true;
                }
            }
        }
    } catch (...) {
    }

    if (ok) {
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    CloseAllMenus();
    CloseTahoeAppsLauncher();
    ShutdownLiquidCapture();
    g_gfx.Shutdown();
    if (g_dock.hwnd) {
        DestroyWindow(g_dock.hwnd);
        g_dock.hwnd = nullptr;
    }
    UnregisterClassW(kAppsHostClassName, hSelf);
    UnregisterClassW(kAppsLauncherClassName, hSelf);
    UnregisterClassW(kMenuClassName, hSelf);
    ShutdownDispatcherQueue();
    if (SUCCEEDED(oleHr)) {
        OleUninitialize();
    }
    return 0;
}

BOOL WhTool_ModInit() {
    LoadSettings();
    g_quitting = false;
    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &g_uiThreadId);
    return g_uiThread != nullptr;
}

void WhTool_ModSettingsChanged() {
    if (g_dock.hwnd) {
        PostMessageW(g_dock.hwnd, kWmAppSettings, 0, 0);
    }
}

void WhTool_ModUninit() {
    g_quitting = true;
    if (g_dock.hwnd) {
        PostMessageW(g_dock.hwnd, kWmAppQuit, 0, 0);
    } else if (g_uiThreadId) {
        PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
    }
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, 5000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions.
////////////////////////////////////////////////////////////////////////////////

bool g_isToolModProcessLauncher = false;
HANDLE g_toolModProcessMutex = nullptr;

void WINAPI EntryPoint_Hook() {
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId = 0;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
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
        g_toolModProcessMutex =
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            ExitProcess(1);
        }
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            CloseHandle(g_toolModProcessMutex);
            g_toolModProcessMutex = nullptr;
            ExitProcess(1);
        }
        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }
        IMAGE_DOS_HEADER* dosHeader =
            reinterpret_cast<IMAGE_DOS_HEADER*>(GetModuleHandleW(nullptr));
        IMAGE_NT_HEADERS* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(
            reinterpret_cast<BYTE*>(dosHeader) + dosHeader->e_lfanew);
        void* entryPoint = reinterpret_cast<BYTE*>(dosHeader) +
                           ntHeaders->OptionalHeader.AddressOfEntryPoint;
        Wh_SetFunctionHook(entryPoint, reinterpret_cast<void*>(EntryPoint_Hook),
                           nullptr);
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
    switch (GetModuleFileNameW(nullptr, currentProcessPath,
                               ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            return;
    }
    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) -
                      1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
        if (!kernelModule) {
            return;
        }
    }
    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    auto pCreateProcessInternalW =
        reinterpret_cast<CreateProcessInternalW_t>(
            GetProcAddress(kernelModule, "CreateProcessInternalW"));
    if (!pCreateProcessInternalW) {
        return;
    }
    STARTUPINFOW si{.cb = sizeof(STARTUPINFOW),
                    .dwFlags = STARTF_FORCEOFFFEEDBACK};
    PROCESS_INFORMATION pi{};
    if (pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                nullptr, nullptr, &si, &pi, nullptr)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
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

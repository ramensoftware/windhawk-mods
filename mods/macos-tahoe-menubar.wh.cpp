// ==WindhawkMod==
// @id              macos-tahoe-menubar
// @name            macOS 26 Tahoe Menu Bar (Finder Bar)
// @description     macOS 26/27 top menu bar with global active-app menus, Wi-Fi/Bluetooth/Audio/Battery Control Center, Calendar, mirrored system tray, and live Liquid Glass
// @version         1.0.0
// @author          Badalemo
// @github          https://github.com/Badalemo
// @include         windhawk.exe
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luuid -lshell32 -lshlwapi -lpropsys -ldwmapi -ld2d1 -ld3d11 -ldxgi -ldwrite -lwindowscodecs -lgdi32 -luser32 -lversion -lshcore -lcomctl32 -lcomdlg32 -lpowrprof -lwlanapi -ldxva2 -ladvapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# macOS 26 Tahoe Menu Bar (Finder Bar)

Turns the top edge of your screen into a standalone **macOS 26/27 Tahoe Menu Bar**:

![macOS 26 Tahoe Menu Bar](https://raw.githubusercontent.com/Badalemo/windhawk-mods-media/main/menubar.png)

![macOS 26 Tahoe Menu Bar demo](https://raw.githubusercontent.com/Badalemo/windhawk-mods-media/main/menubar-demo.gif)

- **Dynamic Active App Menus**: Logo menu, active app name, File, Edit, View, Go,
  and Window menus tailored to the focused window.
- **Control Center & Quick Toggles**: Wi-Fi, Bluetooth, Output Volume, Display
  Brightness, Dark Mode switch, Now Playing controls, Screenshot (`Win+Shift+S`),
  Lock Screen, and Task Manager.
- **Mirrored Windows System Tray**: Clean dual-process `explorer.exe` bridge puts
  legacy & UWP tray icons inside a Liquid Glass popup on the menu bar.
- **Calendar & Clock**: Click the top-right date/time to open a Liquid Glass
  monthly calendar popup.
- **Optional Dock Integration**: Clicking *Desktop & Dock Settings…* opens the
  companion **macOS 26 Tahoe Dock** (`macos-tahoe-dock`) settings panel when running.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- finderBarHeight: 28
  $name: Menu Bar height (22-40)
  $description: Height of the top Menu Bar in DIPs.
- finderBarStyle: liquidGlass
  $name: Menu Bar background style
  $description: Background appearance of the top Menu Bar.
  $options:
  - liquidGlass: Liquid Glass (macOS 26/27)
  - transparent: Transparent (Wallpaper through)
  - frosted: Frosted Glass
  - solid: Solid Dark / Light
- appearance: auto
  $name: Appearance theme
  $description: Follow Windows light/dark theme or force Dark / Light mode.
  $options:
  - auto: Auto (Follow Windows)
  - dark: Dark Mode
  - light: Light Mode
- glassStyle: clear
  $name: Liquid Glass style
  $description: Clear keeps a crisp translucent bar; Tinted subtly blends your Windows accent color.
  $options:
  - clear: Clear
  - tinted: Tinted
- glassRefraction: lens
  $name: Optical refraction mode
  $description: Real-time DXGI refractive edge rendering when Liquid Glass style is selected.
  $options:
  - lens: Lens Refraction (macOS 26 Tahoe)
  - reflection: Mirror Reflection
  - stretch: Convex Stretch
  - off: Frosted Blur Only (No DXGI Capture)
- glassEdgeDepth: 45
  $name: Edge refraction depth (0-100)
  $description: Optical bezel refraction depth along bar and popups.
- glassEdgeStrength: 65
  $name: Specular rim strength (0-100)
  $description: Specular glass catchlight intensity.
- glassTint: 28
  $name: Glass tint opacity (0-100)
  $description: Smoked fill density.
- glassBlur: 50
  $name: Backdrop blur (0-100)
  $description: Frost intensity behind menu popups and bar.
- glassBrightnessLimit: 78
  $name: Dark-mode highlight limiter (0-100)
  $description: Prevents white wallpaper highlights from blowing out white text in Dark Mode.
- finderBarDesktopName: "Finder"
  $name: Desktop / File Explorer app name
  $description: Name displayed in the Menu Bar when the Desktop or File Explorer is active.
- finderBarReserveSpace: true
  $name: Reserve top screen space (AppBar)
  $description: Keep maximized windows below the top Menu Bar.
- finderBarShowSeconds: false
  $name: Show seconds in clock
  $description: Display seconds in the top-right date & time clock.
- finderBarShow24h: false
  $name: Use 24-hour clock
  $description: Display 24-hour format instead of AM/PM.
- finderBarShowBatteryPct: true
  $name: Show battery percentage
  $description: Display numeric battery percentage on laptops.
- finderBarShowTray: true
  $name: Mirror Windows system tray icons
  $description: Host mirrored tray icons inside a Liquid Glass popup on the Menu Bar.
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windhawk_utils.h>

#include <windows.h>
#include <appmodel.h>
#include <commctrl.h>
#include <commdlg.h>
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
#include <endpointvolume.h>
#include <highlevelmonitorconfigurationapi.h>
#include <mmdeviceapi.h>
#include <netlistmgr.h>
#include <ole2.h>
#include <physicalmonitorenumerationapi.h>
#include <powrprof.h>
#include <propkey.h>
#include <propsys.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <wincodec.h>
#include <wlanapi.h>

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
constexpr UINT kWmAppAppBar = WM_APP + 103;
constexpr UINT kWmAppDeferredAction = WM_APP + 105;
constexpr UINT kWmAppGlassFrame = WM_APP + 106;

constexpr UINT_PTR kTimerFullscreenCheck = 4;
constexpr UINT_PTR kTimerGlassMaintenance = 5;
constexpr UINT_PTR kTimerFinderBarStatus = 6;

constexpr int kHotkeyFreezeGlass = 1;

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
enum class FinderBarStyle { LiquidGlass = 0, Transparent = 1, Frosted = 2, Solid = 3 };

struct DockSettings {
    AppearanceMode appearance = AppearanceMode::Auto;
    GlassStyle glassStyle = GlassStyle::Clear;
    GlassRefraction glassRefraction = GlassRefraction::Lens;
    int glassEdgeDepth = 45;
    int glassEdgeStrength = 65;
    int glassTint = 28;
    int glassBlur = 50;
    int glassBrightnessLimit = 78;
    bool enableFinderBar = true;
    int finderBarHeight = 28;
    FinderBarStyle finderBarStyle = FinderBarStyle::LiquidGlass;
    std::wstring finderBarDesktopName = L"Finder";
    bool finderBarReserveSpace = true;
    bool finderBarShowSeconds = false;
    bool finderBarShow24h = false;
    bool finderBarShowBatteryPct = true;
    bool finderBarShowTray = true;
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
    bool fullscreenHidden = false;
    UINT taskbarCreatedMsg = 0;
};

DockSettings g_settings;
DockWindowState g_dock;
bool g_quitting = false;
UINT g_msgOpenDockSettings = 0;

std::wstring ReadWhStringSetting(PCWSTR name) {
    PCWSTR raw = Wh_GetStringSetting(name);
    std::wstring out = raw ? raw : L"";
    if (raw) {
        Wh_FreeStringSetting(raw);
    }
    return out;
}

inline void RequestAnimationFrame() {}
inline void CloseWindowPreviewFlyout() {}
inline bool PointInRectF(const D2D1_RECT_F& r, float x, float y) {
    return x >= r.left && x <= r.right && y >= r.top && y <= r.bottom;
}
void LoadSettings();
void UpdateGlassAppearance();
void OnFinderBarMenuClosed();

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
constexpr PROPERTYKEY kPkeyAppUserModelId = {
    {0x9f4c2855, 0x9f79, 0x4b39, {0xa8, 0xd0, 0xe1, 0xd4, 0x2d, 0xe1, 0xd5, 0xf3}},
    5};
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
    std::mutex copyRectMutex;
    RECT pendingCopyRect{};
    bool pendingCopyValid = false;
    bool pendingCopyFull = true;
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
    std::atomic<bool> highRate{false};
    std::atomic<bool> forceFullCopy{true};

    std::mutex regionsMutex;
    std::vector<RECT> watchedRegions;
    std::vector<RECT> lastUiRegions;
    RECT lastCoveredRect{};
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
    RECT newBounds{};
    for (const auto& r : regions) {
        if (r.right > r.left && r.bottom > r.top) {
            if (IsRectEmpty(&newBounds)) {
                newBounds = r;
            } else {
                UnionRect(&newBounds, &newBounds, &r);
            }
        }
    }
    if (!IsRectEmpty(&newBounds)) {
        RECT inter{};
        if (IsRectEmpty(&g_liquidCapture.lastCoveredRect) ||
            !IntersectRect(&inter, &newBounds, &g_liquidCapture.lastCoveredRect) ||
            !EqualRect(&inter, &newBounds)) {
            g_liquidCapture.forceFullCopy.store(true, std::memory_order_release);
        }
        g_liquidCapture.lastCoveredRect = newBounds;
    }
    std::lock_guard<std::mutex> lock(g_liquidCapture.regionsMutex);
    g_liquidCapture.watchedRegions = regions;
}

bool DuplicationChangedNearWatched(IDXGIOutputDuplication* dup,
                                   const RECT& screenRect,
                                   const DXGI_OUTDUPL_FRAME_INFO& info,
                                   std::vector<BYTE>& metaBuf,
                                   std::vector<RECT>& regions) {
    if (!info.TotalMetadataBufferSize) {
        return true;
    }
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
    std::vector<RECT> regionsBuf;
    try {
        while (!w->stop.load(std::memory_order_relaxed)) {
            if (!first && !g_liquidCapture.watching.load(std::memory_order_relaxed)) {
                Sleep(150);
                continue;
            }
            DXGI_OUTDUPL_FRAME_INFO info{};
            winrt::com_ptr<IDXGIResource> res;
            HRESULT hr = w->duplication->AcquireNextFrame(250, &info, res.put());
            if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
                continue;
            }
            if (FAILED(hr)) {
                w->state = LiquidCaptureWorker::Lost;
                break;
            }
            bool doFull =
                std::exchange(first, false) ||
                g_liquidCapture.forceFullCopy.exchange(false, std::memory_order_acq_rel);
            bool copiedFrame = false;
            bool lost = false;
            {
                struct FrameGuard {
                    IDXGIOutputDuplication* dup;
                    ~FrameGuard() { dup->ReleaseFrame(); }
                } guard{w->duplication.get()};

                do {
                    bool keep =
                        doFull || std::exchange(missed, false) ||
                        (info.LastPresentTime.QuadPart != 0 &&
                         DuplicationChangedNearWatched(w->duplication.get(),
                                                       w->screenRect, info, metaBuf,
                                                       regionsBuf));
                    if (!keep || w->stop.load(std::memory_order_relaxed)) {
                        break;
                    }
                    auto tex = res.try_as<ID3D11Texture2D>();
                    if (!tex) {
                        break;
                    }
                    D3D11_TEXTURE2D_DESC srcDesc{}, dstDesc{};
                    tex->GetDesc(&srcDesc);
                    w->sharedTex->GetDesc(&dstDesc);
                    if (srcDesc.Format != dstDesc.Format ||
                        srcDesc.Width != dstDesc.Width ||
                        srcDesc.Height != dstDesc.Height) {
                        w->state = LiquidCaptureWorker::Lost;
                        lost = true;
                        break;
                    }

                    RECT copyRc = w->screenRect;
                    bool copyFull = doFull;
                    if (!copyFull) {
                        {
                            std::lock_guard<std::mutex> lock(
                                g_liquidCapture.regionsMutex);
                            regionsBuf = g_liquidCapture.watchedRegions;
                        }
                        RECT bounds{};
                        for (const auto& r : regionsBuf) {
                            if (r.right > r.left && r.bottom > r.top) {
                                if (IsRectEmpty(&bounds)) {
                                    bounds = r;
                                } else {
                                    UnionRect(&bounds, &bounds, &r);
                                }
                            }
                        }
                        if (!IsRectEmpty(&bounds)) {
                            InflateRect(&bounds, 64, 64);
                            RECT clamped{};
                            if (IntersectRect(&clamped, &bounds, &w->screenRect) &&
                                clamped.right > clamped.left &&
                                clamped.bottom > clamped.top) {
                                copyRc = clamped;
                            } else {
                                break;
                            }
                        } else {
                            copyFull = true;
                        }
                    }

                    HRESULT syncHr = w->sharedLock->AcquireSync(0, 50);
                    if (syncHr == static_cast<HRESULT>(WAIT_TIMEOUT)) {
                        missed = true;
                        if (doFull) {
                            g_liquidCapture.forceFullCopy.store(
                                true, std::memory_order_release);
                        }
                        break;
                    }
                    if (syncHr != S_OK) {
                        w->state = LiquidCaptureWorker::Lost;
                        lost = true;
                        break;
                    }
                    if (copyFull) {
                        w->context->CopyResource(w->sharedTex.get(), tex.get());
                    } else {
                        D3D11_BOX box{
                            static_cast<UINT>(std::max<LONG>(0, copyRc.left - w->screenRect.left)),
                            static_cast<UINT>(std::max<LONG>(0, copyRc.top - w->screenRect.top)),
                            0,
                            static_cast<UINT>(std::min<LONG>(dstDesc.Width, copyRc.right - w->screenRect.left)),
                            static_cast<UINT>(std::min<LONG>(dstDesc.Height, copyRc.bottom - w->screenRect.top)),
                            1};
                        if (box.right > box.left && box.bottom > box.top) {
                            w->context->CopySubresourceRegion(
                                w->sharedTex.get(), 0, box.left, box.top, 0,
                                tex.get(), 0, &box);
                        }
                    }
                    w->sharedLock->ReleaseSync(0);

                    {
                        std::lock_guard<std::mutex> lock(w->copyRectMutex);
                        if (copyFull) {
                            w->pendingCopyFull = true;
                            w->pendingCopyRect = w->screenRect;
                            w->pendingCopyValid = true;
                        } else if (!w->pendingCopyFull) {
                            if (!w->pendingCopyValid) {
                                w->pendingCopyRect = copyRc;
                                w->pendingCopyValid = true;
                            } else {
                                UnionRect(&w->pendingCopyRect, &w->pendingCopyRect,
                                          &copyRc);
                            }
                        }
                    }

                    w->copied.fetch_add(1, std::memory_order_release);
                    copiedFrame = true;
                    if (g_liquidCapture.watching.load(std::memory_order_relaxed)) {
                        PostGlassFrameWake();
                    }
                } while (false);
            }
            if (lost) {
                break;
            }
            if (!w->stop.load(std::memory_order_relaxed)) {
                bool hi = g_liquidCapture.highRate.load(std::memory_order_relaxed);
                Sleep(copiedFrame ? (hi ? 20u : 50u) : (hi ? 20u : 33u));
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
    g_liquidCapture.lastCoveredRect = {};
    g_liquidCapture.lastUiRegions.clear();
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
            bool newTarget =
                (!target || have.Width != desc.Width ||
                 have.Height != desc.Height || have.Format != desc.Format);
            if (newTarget) {
                desc.BindFlags = 0;
                desc.CPUAccessFlags = 0;
                desc.MiscFlags = 0;
                target = nullptr;
                if (FAILED(g_gfx.d3dDevice->CreateTexture2D(&desc, nullptr,
                                                            target.put()))) {
                    return false;
                }
                g_liquidCapture.forceFullCopy.store(true,
                                                    std::memory_order_release);
            }
            {
                D2DMultithreadLock lock(g_gfx.d2dMultithread.get());
                if (c.openedSharedLock->AcquireSync(0, 0) == S_OK) {
                    RECT copyRc{};
                    bool copyFull = true;
                    bool copyValid = false;
                    {
                        std::lock_guard<std::mutex> rlock(
                            c.worker->copyRectMutex);
                        copyFull = c.worker->pendingCopyFull || newTarget;
                        copyRc = c.worker->pendingCopyRect;
                        copyValid = c.worker->pendingCopyValid;
                        c.worker->pendingCopyFull = false;
                        c.worker->pendingCopyValid = false;
                    }
                    if (copyFull || !copyValid || newTarget) {
                        g_gfx.d3dContext->CopyResource(
                            target.get(), c.openedSharedTex.get());
                    } else {
                        D3D11_BOX box{
                            static_cast<UINT>(std::max<LONG>(0, copyRc.left - c.screenRect.left)),
                            static_cast<UINT>(std::max<LONG>(0, copyRc.top - c.screenRect.top)),
                            0,
                            static_cast<UINT>(std::min<LONG>(desc.Width, copyRc.right - c.screenRect.left)),
                            static_cast<UINT>(std::min<LONG>(desc.Height, copyRc.bottom - c.screenRect.top)),
                            1};
                        if (box.right > box.left && box.bottom > box.top) {
                            g_gfx.d3dContext->CopySubresourceRegion(
                                target.get(), 0, box.left, box.top, 0,
                                c.openedSharedTex.get(), 0, &box);
                        }
                    }
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

bool IsRecycleBinNonEmpty() {
    SHQUERYRBINFO info{.cbSize = sizeof(info)};
    if (SUCCEEDED(SHQueryRecycleBinW(nullptr, &info))) {
        return info.i64NumItems > 0;
    }
    return false;
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
// 8. Window Tracker
// ============================================================================

struct WindowIdentity {
    std::wstring aumid;
    std::wstring exePath;
    std::wstring normPath;
    std::wstring displayName;
    bool isExplorerCabinet = false;
};

struct TrackedWindow {
    HWND hwnd = nullptr;
    WindowIdentity id;
    bool minimized = false;
    bool active = false;
};

struct WindowTrackerState {
    winrt::com_ptr<IVirtualDesktopManager> vdm;
    std::unordered_map<HWND, WindowIdentity> identityCache;
    std::vector<HWINEVENTHOOK> hooks;
    UINT shellHookMsg = 0;
    bool shellHookRegistered = false;
    bool dirty = false;
    bool refreshScheduled = false;
    double lastRefreshTime = 0.0;
    HWND foregroundHwnd = nullptr;
};

WindowTrackerState g_tracker;
extern bool g_quitting;

void ScheduleWindowRefresh();
void ScheduleFullscreenCheck();
void OnWindowAttentionFlash(HWND hwnd);
void OnWindowForegroundChanged(HWND hwnd);

std::wstring GetFileDescriptionFromPath(const std::wstring& path) {
    if (path.empty()) {
        return {};
    }
    DWORD dummy = 0;
    DWORD sz = GetFileVersionInfoSizeW(path.c_str(), &dummy);
    if (sz > 0) {
        std::vector<BYTE> buf(sz);
        if (GetFileVersionInfoW(path.c_str(), 0, sz, buf.data())) {
            struct LangCp {
                WORD lang;
                WORD cp;
            };
            LangCp* trans = nullptr;
            UINT transLen = 0;
            WCHAR query[128]{};
            if (VerQueryValueW(buf.data(), L"\\VarFileInfo\\Translation",
                               reinterpret_cast<void**>(&trans), &transLen) &&
                trans && transLen >= sizeof(LangCp)) {
                swprintf_s(query, L"\\StringFileInfo\\%04x%04x\\FileDescription",
                           trans[0].lang, trans[0].cp);
                PWSTR desc = nullptr;
                UINT descLen = 0;
                if (VerQueryValueW(buf.data(), query,
                                   reinterpret_cast<void**>(&desc), &descLen) &&
                    desc && descLen > 1 && *desc) {
                    return desc;
                }
            }
            PWSTR desc = nullptr;
            UINT descLen = 0;
            if (VerQueryValueW(
                    buf.data(), L"\\StringFileInfo\\040904b0\\FileDescription",
                    reinterpret_cast<void**>(&desc), &descLen) &&
                desc && descLen > 1 && *desc) {
                return desc;
            }
        }
    }
    const wchar_t* fname = PathFindFileNameW(path.c_str());
    std::wstring stem = fname ? fname : path;
    size_t dot = stem.find_last_of(L'.');
    if (dot != std::wstring::npos) {
        stem.resize(dot);
    }
    return stem;
}

std::wstring GetDisplayNameForAumid(const std::wstring& aumid) {
    if (aumid.empty()) {
        return {};
    }
    winrt::com_ptr<IShellItem> item;
    if (SUCCEEDED(SHCreateItemInKnownFolder(kFolderIdAppsFolder, 0,
                                            aumid.c_str(),
                                            IID_PPV_ARGS(item.put())))) {
        PWSTR name = nullptr;
        if (SUCCEEDED(item->GetDisplayName(SIGDN_NORMALDISPLAY, &name)) &&
            name) {
            std::wstring out = name;
            CoTaskMemFree(name);
            return out;
        }
    }
    return {};
}

std::wstring GetWindowPropAumid(HWND hwnd) {
    winrt::com_ptr<IPropertyStore> ps;
    if (FAILED(SHGetPropertyStoreForWindow(hwnd, IID_PPV_ARGS(ps.put()))) ||
        !ps) {
        return {};
    }
    PROPVARIANT pv{};
    PropVariantInit(&pv);
    std::wstring res;
    if (SUCCEEDED(ps->GetValue(kPkeyAppUserModelId, &pv))) {
        if (pv.vt == VT_LPWSTR && pv.pwszVal) {
            res = pv.pwszVal;
        }
        PropVariantClear(&pv);
    }
    return res;
}

std::wstring GetProcessExePath(DWORD pid) {
    if (!pid) {
        return {};
    }
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) {
        return {};
    }
    WCHAR buf[MAX_PATH * 2]{};
    DWORD len = ARRAYSIZE(buf);
    std::wstring res;
    if (QueryFullProcessImageNameW(hProc, 0, buf, &len) && len > 0) {
        res.assign(buf, len);
    }
    CloseHandle(hProc);
    return res;
}

std::wstring GetProcessAumid(DWORD pid) {
    if (!pid) {
        return {};
    }
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) {
        return {};
    }
    UINT32 len = 0;
    std::wstring res;
    LONG rc = GetApplicationUserModelId(hProc, &len, nullptr);
    if (rc == ERROR_INSUFFICIENT_BUFFER && len > 1) {
        res.resize(len);
        if (GetApplicationUserModelId(hProc, &len, res.data()) ==
            ERROR_SUCCESS) {
            res.resize(wcslen(res.c_str()));
        } else {
            res.clear();
        }
    }
    CloseHandle(hProc);
    return res;
}

struct CoreWindowChildCtx {
    DWORD framePid;
    DWORD foundPid;
};

BOOL CALLBACK FindCoreWindowChildProc(HWND child, LPARAM lp) {
    auto* c = reinterpret_cast<CoreWindowChildCtx*>(lp);
    WCHAR cls[64]{};
    if (GetClassNameW(child, cls, ARRAYSIZE(cls)) &&
        _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
        DWORD pid = 0;
        GetWindowThreadProcessId(child, &pid);
        if (pid && pid != c->framePid) {
            c->foundPid = pid;
            return FALSE;
        }
    }
    return TRUE;
}

DWORD FindCoreWindowChildPid(HWND frameHwnd, DWORD framePid) {
    CoreWindowChildCtx ctx{framePid, 0};
    EnumChildWindows(frameHwnd, FindCoreWindowChildProc,
                     reinterpret_cast<LPARAM>(&ctx));
    return ctx.foundPid;
}

WindowIdentity ResolveWindowIdentity(HWND hwnd) {
    if (auto it = g_tracker.identityCache.find(hwnd);
        it != g_tracker.identityCache.end()) {
        return it->second;
    }

    WindowIdentity id;
    WCHAR cls[96]{};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));

    if (_wcsicmp(cls, L"CabinetWClass") == 0) {
        id.isExplorerCabinet = true;
        id.exePath = GetExplorerExePath();
        id.normPath = NormalizePath(id.exePath);
        id.displayName = L"Finder";
        g_tracker.identityCache[hwnd] = id;
        return id;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    DWORD targetPid = pid;

    id.aumid = GetWindowPropAumid(hwnd);

    if (_wcsicmp(cls, L"ApplicationFrameWindow") == 0) {
        DWORD childPid = FindCoreWindowChildPid(hwnd, pid);
        if (childPid) {
            targetPid = childPid;
        }
    }

    if (id.aumid.empty() && targetPid) {
        id.aumid = GetProcessAumid(targetPid);
    }
    if (targetPid) {
        id.exePath = GetProcessExePath(targetPid);
        id.normPath = NormalizePath(id.exePath);
    }

    if (!id.aumid.empty()) {
        id.displayName = GetDisplayNameForAumid(id.aumid);
    }
    if (id.displayName.empty() && !id.exePath.empty()) {
        id.displayName = GetFileDescriptionFromPath(id.exePath);
    }
    if (id.displayName.empty()) {
        WCHAR title[128]{};
        if (GetWindowTextW(hwnd, title, ARRAYSIZE(title)) > 0) {
            id.displayName = title;
        } else {
            id.displayName = L"Application";
        }
    }

    // Do not cache empty UWP ApplicationFrameWindow before its CoreWindow mounts.
    if (!(_wcsicmp(cls, L"ApplicationFrameWindow") == 0 && id.aumid.empty() &&
          targetPid == pid)) {
        g_tracker.identityCache[hwnd] = id;
    }
    return id;
}

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

void ActivateWindowForce(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return;
    }
    if (IsIconic(hwnd)) {
        ShowWindow(hwnd, SW_RESTORE);
    }
    if (!SetForegroundWindow(hwnd)) {
        SwitchToThisWindow(hwnd, TRUE);
    }
}

void ShowAllItemWindows(const std::vector<HWND>& hwnds, HWND lastActive) {
    HWND primary = (lastActive && IsWindow(lastActive))
                       ? lastActive
                       : (hwnds.empty() ? nullptr : hwnds.front());
    for (HWND h : hwnds) {
        if (h != primary && IsWindow(h)) {
            if (IsIconic(h)) {
                ShowWindow(h, SW_RESTORE);
            }
            SetWindowPos(h, HWND_TOP, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }
    if (primary) {
        ActivateWindowForce(primary);
    }
}

std::wstring GetWindowDisplayTitle(HWND hwnd) {
    WCHAR buf[128]{};
    if (IsWindow(hwnd) && GetWindowTextW(hwnd, buf, ARRAYSIZE(buf)) > 0) {
        return buf;
    }
    return L"Window";
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
constexpr WCHAR kMenuClassName[] = L"WindhawkMacOSTahoeBarMenuWnd";


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

// 16. macOS 27 Top Finder Bar, Global Menus, Control Center, Calendar & Tray
// ============================================================================

// ---- Shared Tray Mirroring IPC (explorer.exe x64 -> windhawk.exe x86) ----

struct TrayNID32 {
    DWORD cbSize;
    DWORD hWnd;
    UINT uID;
    UINT uFlags;
    UINT uCallbackMessage;
    DWORD hIcon;
    WCHAR szTip[128];
    DWORD dwState;
    DWORD dwStateMask;
    WCHAR szInfo[256];
    UINT uVersion;
    WCHAR szInfoTitle[64];
    DWORD dwInfoFlags;
    GUID guidItem;
    DWORD hBalloonIcon;
};

struct TrayIpcMsg {
    DWORD signature;  // 0x34753423
    DWORD message;    // NIM_ADD / NIM_MODIFY / NIM_DELETE / NIM_SETVERSION
    TrayNID32 nid;
};

constexpr WCHAR kTrayClientClassName[] = L"WindhawkMacOS27TrayClient";
constexpr ULONG_PTR kTrayIpcFwd = 0x4D443237;    // 'MD27'
constexpr ULONG_PTR kTrayIpcHello = 0x4D443238;  // 'MD28'

struct MirroredTrayIcon {
    HWND ownerHwnd = nullptr;
    UINT uID = 0;
    GUID guid{};
    bool hasGuid = false;
    UINT callbackMsg = 0;
    HICON hIcon = nullptr;
    std::wstring tip;
    bool hidden = false;
    UINT version = 0;
    DWORD pid = 0;
};

// Explorer-side lightweight tray forwarding hook (runs ONLY inside explorer.exe)
namespace ExplorerTrayHost {
HHOOK g_hook = nullptr;
HWND g_shellTray = nullptr;
HANDLE g_stopEv = nullptr;
HANDLE g_wakeEv = nullptr;
HANDLE g_thread = nullptr;
volatile LONG g_hookBusy = 0;
SRWLOCK g_queueLock = SRWLOCK_INIT;
std::vector<std::vector<BYTE>> g_queue;

LRESULT CALLBACK CallWndProcHook(int code, WPARAM wp, LPARAM lp) {
    InterlockedIncrement(&g_hookBusy);
    if (code == HC_ACTION) {
        auto* cwp = reinterpret_cast<CWPSTRUCT*>(lp);
        if (cwp && cwp->message == WM_COPYDATA && cwp->hwnd == g_shellTray) {
            auto* cds = reinterpret_cast<COPYDATASTRUCT*>(cwp->lParam);
            if (cds && cds->dwData == 1 && cds->lpData && cds->cbData >= 8 &&
                *reinterpret_cast<const DWORD*>(cds->lpData) == 0x34753423u) {
                const BYTE* p = reinterpret_cast<const BYTE*>(cds->lpData);
                DWORD n = std::min<DWORD>(cds->cbData,
                                          static_cast<DWORD>(sizeof(TrayIpcMsg)));
                std::vector<BYTE> pkt(p, p + n);
                AcquireSRWLockExclusive(&g_queueLock);
                if (g_queue.size() < 2048) {
                    g_queue.push_back(std::move(pkt));
                }
                ReleaseSRWLockExclusive(&g_queueLock);
                if (g_wakeEv) {
                    SetEvent(g_wakeEv);
                }
            }
        }
    }
    LRESULT r = CallNextHookEx(nullptr, code, wp, lp);
    InterlockedDecrement(&g_hookBusy);
    return r;
}

bool SendToClient(HWND client, ULONG_PTR kind, const void* data, DWORD sz) {
    COPYDATASTRUCT cds{kind, sz, const_cast<void*>(data)};
    DWORD_PTR res = 0;
    return SendMessageTimeoutW(client, WM_COPYDATA, 0,
                               reinterpret_cast<LPARAM>(&cds),
                               SMTO_ABORTIFHUNG | SMTO_BLOCK, 800, &res) != 0;
}

DWORD WINAPI HostThreadProc(LPVOID) {
    HMODULE hSelf = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&HostThreadProc), &hSelf);

    for (int i = 0; i < 240; ++i) {
        HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        DWORD pid = 0;
        DWORD tid = tray ? GetWindowThreadProcessId(tray, &pid) : 0;
        if (tray && tid && pid == GetCurrentProcessId()) {
            g_shellTray = tray;
            g_hook = SetWindowsHookExW(WH_CALLWNDPROC, CallWndProcHook, hSelf,
                                       tid);
            break;
        }
        if (WaitForSingleObject(g_stopEv, 500) == WAIT_OBJECT_0) {
            return 0;
        }
    }

    HWND lastClient = nullptr;
    HANDLE hs[2] = {g_stopEv, g_wakeEv};
    for (;;) {
        DWORD w = WaitForMultipleObjects(2, hs, FALSE, 2000);
        if (w == WAIT_OBJECT_0) {
            break;
        }
        HWND client =
            FindWindowExW(HWND_MESSAGE, nullptr, kTrayClientClassName, nullptr);
        if (client && client != lastClient) {
            SendToClient(client, kTrayIpcHello, nullptr, 0);
            lastClient = client;
        }
        std::vector<std::vector<BYTE>> batch;
        AcquireSRWLockExclusive(&g_queueLock);
        batch.swap(g_queue);
        ReleaseSRWLockExclusive(&g_queueLock);
        if (!client) {
            continue;
        }
        for (auto& pkt : batch) {
            if (WaitForSingleObject(g_stopEv, 0) == WAIT_OBJECT_0) {
                break;
            }
            SendToClient(client, kTrayIpcFwd, pkt.data(),
                         static_cast<DWORD>(pkt.size()));
        }
    }
    return 0;
}

void UnhookSafe() {
    if (g_hook) {
        UnhookWindowsHookEx(g_hook);
        g_hook = nullptr;
    }
    while (InterlockedCompareExchange(&g_hookBusy, 0, 0) > 0) {
        Sleep(10);
    }
}

BOOL Init() {
    g_stopEv = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_wakeEv = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_stopEv || !g_wakeEv) {
        return FALSE;
    }
    g_thread = CreateThread(nullptr, 0, HostThreadProc, nullptr, 0, nullptr);
    return g_thread != nullptr;
}

void Uninit() {
    UnhookSafe();
    if (g_stopEv) {
        SetEvent(g_stopEv);
    }
    if (g_thread) {
        WaitForSingleObject(g_thread, 3000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    UnhookSafe();
    if (g_stopEv) {
        CloseHandle(g_stopEv);
        g_stopEv = nullptr;
    }
    if (g_wakeEv) {
        CloseHandle(g_wakeEv);
        g_wakeEv = nullptr;
    }
    AcquireSRWLockExclusive(&g_queueLock);
    g_queue.clear();
    ReleaseSRWLockExclusive(&g_queueLock);
}
}  // namespace ExplorerTrayHost

// ---- System Status Helpers (Battery, Wi-Fi/Net, Audio, Brightness, Dark Mode) ----

struct SystemStatusSnapshot {
    std::wstring clockText;
    bool hasBattery = false;
    int batteryPct = 100;
    bool batteryCharging = false;
    bool batteryAc = true;
    int batteryRemainSec = -1;
    int netKind = 0;  // 0 = offline, 1 = Wi-Fi, 2 = Ethernet
    std::wstring wifiSsid;
    int wifiQuality = 100;
    bool hasAudio = false;
    int volumePct = 50;
    bool muted = false;
    bool hasBrightness = false;
    int brightnessPct = 80;
    int pollCounter = 0;
};

struct SystemComCache {
    HANDLE wlanHandle = nullptr;
    winrt::com_ptr<INetworkListManager> nlm;
    winrt::com_ptr<IMMDeviceEnumerator> mmEnum;

    void Shutdown() {
        mmEnum = nullptr;
        nlm = nullptr;
        if (wlanHandle) {
            WlanCloseHandle(wlanHandle, nullptr);
            wlanHandle = nullptr;
        }
    }
};

SystemComCache g_sysCom;

std::wstring Utf8BytesToWide(const char* s, int len) {
    if (!s || len <= 0) {
        return {};
    }
    int n = MultiByteToWideChar(CP_UTF8, 0, s, len, nullptr, 0);
    if (n <= 0) {
        return {};
    }
    std::wstring out(static_cast<size_t>(n), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s, len, out.data(), n);
    return out;
}

winrt::com_ptr<IAudioEndpointVolume> GetMasterAudioEndpoint() {
    if (!g_sysCom.mmEnum) {
        CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                         IID_PPV_ARGS(g_sysCom.mmEnum.put()));
    }
    if (!g_sysCom.mmEnum) {
        return nullptr;
    }
    winrt::com_ptr<IMMDevice> dev;
    if (FAILED(g_sysCom.mmEnum->GetDefaultAudioEndpoint(eRender, eConsole,
                                                        dev.put())) ||
        !dev) {
        return nullptr;
    }
    winrt::com_ptr<IAudioEndpointVolume> vol;
    dev->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr,
                  vol.put_void());
    return vol;
}

void PollAudioStatus(SystemStatusSnapshot& st) {
    auto vol = GetMasterAudioEndpoint();
    if (!vol) {
        return;
    }
    float scalar = 0.0f;
    BOOL mute = FALSE;
    if (SUCCEEDED(vol->GetMasterVolumeLevelScalar(&scalar))) {
        st.hasAudio = true;
        st.volumePct =
            std::clamp(static_cast<int>(std::lround(scalar * 100.0f)), 0, 100);
    }
    if (SUCCEEDED(vol->GetMute(&mute))) {
        st.muted = (mute != FALSE);
    }
}

void SetSystemVolumePct(int pct) {
    pct = std::clamp(pct, 0, 100);
    if (auto vol = GetMasterAudioEndpoint()) {
        vol->SetMasterVolumeLevelScalar(static_cast<float>(pct) / 100.0f,
                                        nullptr);
        if (pct > 0) {
            vol->SetMute(FALSE, nullptr);
        }
    }
}

void ToggleSystemMute() {
    if (auto vol = GetMasterAudioEndpoint()) {
        BOOL mute = FALSE;
        if (SUCCEEDED(vol->GetMute(&mute))) {
            vol->SetMute(!mute, nullptr);
        }
    }
}

void PollNetworkStatus(SystemStatusSnapshot& st) {
    st.netKind = 0;
    st.wifiSsid.clear();
    st.wifiQuality = 0;

    if (!g_sysCom.wlanHandle) {
        DWORD ver = 0;
        if (WlanOpenHandle(2, nullptr, &ver, &g_sysCom.wlanHandle) !=
            ERROR_SUCCESS) {
            g_sysCom.wlanHandle = nullptr;
        }
    }
    if (g_sysCom.wlanHandle) {
        PWLAN_INTERFACE_INFO_LIST list = nullptr;
        if (WlanEnumInterfaces(g_sysCom.wlanHandle, nullptr, &list) ==
                ERROR_SUCCESS &&
            list) {
            for (DWORD i = 0; i < list->dwNumberOfItems; ++i) {
                PWLAN_CONNECTION_ATTRIBUTES attr = nullptr;
                DWORD sz = 0;
                WLAN_OPCODE_VALUE_TYPE opType{};
                if (WlanQueryInterface(g_sysCom.wlanHandle,
                                       &list->InterfaceInfo[i].InterfaceGuid,
                                       wlan_intf_opcode_current_connection,
                                       nullptr, &sz,
                                       reinterpret_cast<PVOID*>(&attr),
                                       &opType) == ERROR_SUCCESS &&
                    attr) {
                    if (attr->isState == wlan_interface_state_connected) {
                        st.netKind = 1;
                        st.wifiSsid = Utf8BytesToWide(
                            reinterpret_cast<const char*>(
                                attr->wlanAssociationAttributes.dot11Ssid
                                    .ucSSID),
                            static_cast<int>(
                                attr->wlanAssociationAttributes.dot11Ssid
                                    .uSSIDLength));
                        st.wifiQuality = static_cast<int>(
                            attr->wlanAssociationAttributes.wlanSignalQuality);
                        WlanFreeMemory(attr);
                        break;
                    }
                    WlanFreeMemory(attr);
                }
            }
            WlanFreeMemory(list);
        }
    }

    if (st.netKind == 0) {
        if (!g_sysCom.nlm) {
            CoCreateInstance(__uuidof(NetworkListManager), nullptr, CLSCTX_ALL,
                             IID_PPV_ARGS(g_sysCom.nlm.put()));
        }
        if (g_sysCom.nlm) {
            NLM_CONNECTIVITY conn = NLM_CONNECTIVITY_DISCONNECTED;
            if (SUCCEEDED(g_sysCom.nlm->GetConnectivity(&conn)) &&
                conn != NLM_CONNECTIVITY_DISCONNECTED) {
                st.netKind = 2;
            }
        }
    }
}

bool GetPrimaryMonitorBrightness(int& outPct) {
    HMONITOR mon = GetPrimaryMonitor();
    DWORD count = 0;
    if (!mon || !GetNumberOfPhysicalMonitorsFromHMONITOR(mon, &count) ||
        count == 0) {
        return false;
    }
    std::vector<PHYSICAL_MONITOR> pms(count);
    if (!GetPhysicalMonitorsFromHMONITOR(mon, count, pms.data())) {
        return false;
    }
    DWORD mn = 0, cur = 0, mx = 100;
    bool ok = GetMonitorBrightness(pms[0].hPhysicalMonitor, &mn, &cur, &mx) &&
              mx > mn;
    if (ok) {
        outPct = std::clamp(
            static_cast<int>(std::lround(
                static_cast<double>(cur - mn) * 100.0 / (mx - mn))),
            0, 100);
    }
    DestroyPhysicalMonitors(count, pms.data());
    return ok;
}

bool SetPrimaryMonitorBrightness(int pct) {
    pct = std::clamp(pct, 0, 100);
    HMONITOR mon = GetPrimaryMonitor();
    DWORD count = 0;
    if (!mon || !GetNumberOfPhysicalMonitorsFromHMONITOR(mon, &count) ||
        count == 0) {
        return false;
    }
    std::vector<PHYSICAL_MONITOR> pms(count);
    if (!GetPhysicalMonitorsFromHMONITOR(mon, count, pms.data())) {
        return false;
    }
    bool ok = false;
    for (DWORD i = 0; i < count; ++i) {
        DWORD mn = 0, cur = 0, mx = 100;
        if (GetMonitorBrightness(pms[i].hPhysicalMonitor, &mn, &cur, &mx) &&
            mx > mn) {
            DWORD val = mn + static_cast<DWORD>(std::lround(
                                 (mx - mn) * (static_cast<double>(pct) / 100.0)));
            if (SetMonitorBrightness(pms[i].hPhysicalMonitor, val)) {
                ok = true;
            }
        }
    }
    DestroyPhysicalMonitors(count, pms.data());
    return ok;
}

void ToggleWindowsDarkModeLive() {
    bool newDark = !IsSystemDarkMode();
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        DWORD val = newDark ? 0u : 1u;
        RegSetValueExW(hKey, L"AppsUseLightTheme", 0, REG_DWORD,
                       reinterpret_cast<const BYTE*>(&val), sizeof(val));
        RegSetValueExW(hKey, L"SystemUsesLightTheme", 0, REG_DWORD,
                       reinterpret_cast<const BYTE*>(&val), sizeof(val));
        RegCloseKey(hKey);
    }
    DWORD_PTR res = 0;
    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                        reinterpret_cast<LPARAM>(L"ImmersiveColorSet"),
                        SMTO_ABORTIFHUNG, 150, &res);
}

std::wstring FormatMacClockString() {
    SYSTEMTIME st{};
    GetLocalTime(&st);
    WCHAR dateBuf[64]{};
    WCHAR timeBuf[64]{};
    GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, L"ddd MMM d", dateBuf,
                    ARRAYSIZE(dateBuf), nullptr);
    PCWSTR timeFmt =
        g_settings.finderBarShow24h
            ? (g_settings.finderBarShowSeconds ? L"HH:mm:ss" : L"HH:mm")
            : (g_settings.finderBarShowSeconds ? L"h:mm:ss tt" : L"h:mm tt");
    GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, timeFmt, timeBuf,
                    ARRAYSIZE(timeBuf));
    std::wstring out = dateBuf;
    if (!out.empty()) {
        out += L"  ";
    }
    out += timeBuf;
    return out;
}

// ---- Global Shortcut & Win32 Menu Helpers ----

constexpr BYTE kModCtrl = 1;
constexpr BYTE kModShift = 2;
constexpr BYTE kModAlt = 4;
constexpr BYTE kModWin = 8;

struct BarKeyCmd {
    PCWSTR label;
    BYTE mods;
    WORD vk;
    BYTE mods2 = 0;
    WORD vk2 = 0;
};

void SendKeyCombo(BYTE mods, WORD vk) {
    std::vector<INPUT> seq;
    auto pushKey = [&](WORD k, bool up) {
        INPUT in{};
        in.type = INPUT_KEYBOARD;
        in.ki.wVk = k;
        in.ki.dwFlags = up ? KEYEVENTF_KEYUP : 0;
        seq.push_back(in);
    };
    if (mods & kModCtrl) pushKey(VK_CONTROL, false);
    if (mods & kModShift) pushKey(VK_SHIFT, false);
    if (mods & kModAlt) pushKey(VK_MENU, false);
    if (mods & kModWin) pushKey(VK_LWIN, false);
    pushKey(vk, false);
    pushKey(vk, true);
    if (mods & kModWin) pushKey(VK_LWIN, true);
    if (mods & kModAlt) pushKey(VK_MENU, true);
    if (mods & kModShift) pushKey(VK_SHIFT, true);
    if (mods & kModCtrl) pushKey(VK_CONTROL, true);
    if (!seq.empty()) {
        SendInput(static_cast<UINT>(seq.size()), seq.data(), sizeof(INPUT));
    }
}

void SendShortcutToTarget(HWND target, BYTE mods, WORD vk, BYTE mods2 = 0,
                          WORD vk2 = 0) {
    if (target && IsWindow(target)) {
        if (IsIconic(target)) {
            ShowWindow(target, SW_RESTORE);
        }
        SetForegroundWindow(target);
        Sleep(30);
    }
    SendKeyCombo(mods, vk);
    if (vk2) {
        Sleep(35);
        SendKeyCombo(mods2, vk2);
    }
}

void OpenShellUri(PCWSTR uri) {
    if (uri && *uri) {
        ShellExecuteW(nullptr, L"open", uri, nullptr, nullptr, SW_SHOWNORMAL);
    }
}

void EnablePrivilege(PCWSTR privName) {
    HANDLE tok = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &tok)) {
        return;
    }
    TOKEN_PRIVILEGES tp{};
    if (LookupPrivilegeValueW(nullptr, privName, &tp.Privileges[0].Luid)) {
        tp.PrivilegeCount = 1;
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(tok, FALSE, &tp, sizeof(tp), nullptr, nullptr);
    }
    CloseHandle(tok);
}

struct EnumPidWinsCtx {
    DWORD pid = 0;
    std::vector<HWND> out;
};

BOOL CALLBACK EnumWindowsByPidProc(HWND hwnd, LPARAM lp) {
    auto* ctx = reinterpret_cast<EnumPidWinsCtx*>(lp);
    if (!IsWindowVisible(hwnd)) {
        return TRUE;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != ctx->pid) {
        return TRUE;
    }
    if (GetWindow(hwnd, GW_OWNER) != nullptr) {
        return TRUE;
    }
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (ex & WS_EX_TOOLWINDOW) {
        return TRUE;
    }
    WCHAR title[16]{};
    if (GetWindowTextW(hwnd, title, ARRAYSIZE(title)) <= 0) {
        return TRUE;
    }
    ctx->out.push_back(hwnd);
    return TRUE;
}

std::vector<HWND> CollectWindowsForPid(DWORD pid) {
    EnumPidWinsCtx ctx{pid, {}};
    if (pid) {
        EnumWindows(EnumWindowsByPidProc, reinterpret_cast<LPARAM>(&ctx));
    }
    return ctx.out;
}

struct HideOthersCtx {
    DWORD keepPid = 0;
    DWORD selfPid = 0;
};

BOOL CALLBACK HideOtherWindowsProc(HWND hwnd, LPARAM lp) {
    auto* ctx = reinterpret_cast<HideOthersCtx*>(lp);
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) {
        return TRUE;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid || pid == ctx->keepPid || pid == ctx->selfPid) {
        return TRUE;
    }
    if (GetWindow(hwnd, GW_OWNER) != nullptr) {
        return TRUE;
    }
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (ex & WS_EX_TOOLWINDOW) {
        return TRUE;
    }
    WCHAR cls[64]{};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (_wcsicmp(cls, L"Progman") == 0 || _wcsicmp(cls, L"WorkerW") == 0 ||
        _wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
        _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0) {
        return TRUE;
    }
    ShowWindowAsync(hwnd, SW_MINIMIZE);
    return TRUE;
}

std::wstring CleanWin32MenuLabel(const std::wstring& raw) {
    std::wstring out;
    out.reserve(raw.size());
    for (size_t i = 0; i < raw.size(); ++i) {
        if (raw[i] == L'&') {
            if (i + 1 < raw.size() && raw[i + 1] == L'&') {
                out.push_back(L'&');
                ++i;
            }
            continue;
        }
        out.push_back(raw[i]);
    }
    return out;
}

std::vector<MenuEntry> BuildEntriesFromHMenu(HMENU hMenu, HWND targetHwnd,
                                             int depth = 0) {
    std::vector<MenuEntry> out;
    if (!hMenu || depth > 1) {
        return out;
    }
    int count = GetMenuItemCount(hMenu);
    if (count <= 0) {
        return out;
    }
    count = std::min(count, 40);
    for (int i = 0; i < count; ++i) {
        MENUITEMINFOW mii{};
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU |
                    MIIM_STRING;
        WCHAR buf[256]{};
        mii.dwTypeData = buf;
        mii.cch = ARRAYSIZE(buf);
        if (!GetMenuItemInfoW(hMenu, static_cast<UINT>(i), TRUE, &mii)) {
            continue;
        }
        if (mii.fType & MFT_SEPARATOR) {
            if (!out.empty() && out.back().type != MenuEntry::Type::Separator) {
                out.push_back({MenuEntry::Type::Separator, L"", false, false,
                               nullptr, {}});
            }
            continue;
        }
        std::wstring label = CleanWin32MenuLabel(buf);
        if (label.empty()) {
            continue;
        }
        bool enabled = (mii.fState & (MFS_DISABLED | MFS_GRAYED)) == 0;
        bool checked = (mii.fState & MFS_CHECKED) != 0;
        if (mii.hSubMenu && depth < 1) {
            auto sub = BuildEntriesFromHMenu(mii.hSubMenu, targetHwnd, depth + 1);
            if (!sub.empty()) {
                out.push_back({MenuEntry::Type::Submenu, label, checked,
                               enabled, nullptr, std::move(sub)});
            }
        } else {
            UINT cmdId = mii.wID;
            out.push_back({MenuEntry::Type::Action, label, checked, enabled,
                           [targetHwnd, cmdId] {
                               if (IsWindow(targetHwnd)) {
                                   SetForegroundWindow(targetHwnd);
                                   PostMessageW(targetHwnd, WM_COMMAND, cmdId,
                                                0);
                               }
                           },
                           {}});
        }
    }
    while (!out.empty() && out.back().type == MenuEntry::Type::Separator) {
        out.pop_back();
    }
    return out;
}

// ---- Finder Bar State & Interactive Popovers ----

enum class BarItemId : int {
    Logo = 0,
    ActiveApp = 1,
    GlobalMenuBase = 10,  // 10..25
    Tray = 100,
    Battery = 101,
    Network = 102,
    Volume = 103,
    Search = 104,
    ControlCenter = 105,
    Clock = 106,
};

struct BarSlot {
    int id = 0;
    std::wstring label;
    D2D1_RECT_F rect{};
    bool bold = false;
    bool isRight = false;
    HMENU win32Sub = nullptr;
};

struct FinderBarState {
    HWND hwnd = nullptr;
    HWND topAppBarHwnd = nullptr;
    HWND trayClientHwnd = nullptr;
    bool topAppBarRegistered = false;
    bool hiddenForFullscreen = false;

    float widthPx = 0.0f;
    float heightPx = 0.0f;
    float scale = 1.0f;
    RECT windowRect{};

    wuc::Desktop::DesktopWindowTarget target{nullptr};
    wuc::ContainerVisual root{nullptr};
    wuc::SpriteVisual fallbackBackdrop{nullptr};
    wuc::SpriteVisual fallbackTint{nullptr};
    wuc::CompositionColorBrush fallbackTintBrush{nullptr};
    LiquidGlassPlate liquidPlate;
    LiquidGlassStyleParams lastGlassParams{};
    bool hasGlassParams = false;
    bool liquidAttached = false;
    bool glassMaintenanceTimerActive = false;

    wuc::CompositionDrawingSurface fgSurface{nullptr};
    wuc::CompositionSurfaceBrush fgBrush{nullptr};
    wuc::SpriteVisual fgVisual{nullptr};
    SIZE fgSurfSize{};
    winrt::com_ptr<IDWriteTextFormat> fmtReg;
    winrt::com_ptr<IDWriteTextFormat> fmtBold;
    winrt::com_ptr<IDWriteTextFormat> fmtSmall;
    float cachedFmtScale = 0.0f;

    // Active foreground app info
    HWND activeHwnd = nullptr;
    DWORD activePid = 0;
    std::wstring activeExeName;
    std::wstring activeExePath;
    std::wstring activeAppName = L"Finder";
    bool isFinderActive = true;

    // Bar layout & interaction
    std::vector<BarSlot> slots;
    int hoveredSlotId = -1;
    int openSlotId = -1;
    bool trackingMouse = false;

    // System status
    SystemStatusSnapshot status;

    // Mirrored tray icons from explorer.exe
    std::vector<MirroredTrayIcon> trayIcons;
    ULONGLONG ignoreTaskbarCreatedUntil = 0;
};

struct ControlCenterState {
    HWND hwnd = nullptr;
    bool open = false;
    float widthPx = 0.0f;
    float heightPx = 0.0f;
    int posX = 0;
    int posY = 0;

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

    // Hit rects inside Control Center
    D2D1_RECT_F rcWifi{}, rcNet{}, rcBt{};
    D2D1_RECT_F rcDarkMode{}, rcFreeze{}, rcFocus{};
    D2D1_RECT_F rcBrightTrack{}, rcVolTrack{}, rcVolMuteBtn{};
    D2D1_RECT_F rcPrev{}, rcPlay{}, rcNext{};
    D2D1_RECT_F rcShot{}, rcLock{}, rcTaskMgr{}, rcDesktop{}, rcSettings{};
    int draggingSlider = 0;  // 0 = none, 1 = brightness, 2 = volume
    int hoverZone = -1;
};

struct TrayPopupState {
    HWND hwnd = nullptr;
    bool open = false;
    float widthPx = 0.0f;
    float heightPx = 0.0f;
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
    std::vector<size_t> visibleIdx;
    std::vector<D2D1_RECT_F> cellRects;
    int hoveredCell = -1;
};

struct CalendarPopupState {
    HWND hwnd = nullptr;
    bool open = false;
    int viewYear = 2026;
    int viewMonth = 1;
    float widthPx = 0.0f;
    float heightPx = 0.0f;
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
    D2D1_RECT_F rcPrev{}, rcNext{}, rcFooter{};
    int hoverBtn = -1;
};

FinderBarState g_bar;
ControlCenterState g_cc;
TrayPopupState g_trayPop;
CalendarPopupState g_cal;

constexpr WCHAR kFinderBarClassName[] = L"WindhawkMacOS27FinderBarWnd";
constexpr WCHAR kTopAppBarClassName[] = L"WindhawkMacOS27TopAppBarWnd";
constexpr WCHAR kControlCenterClassName[] = L"WindhawkMacOS27ControlCenterWnd";
constexpr WCHAR kTrayPopupClassName[] = L"WindhawkMacOS27TrayPopupWnd";
constexpr WCHAR kCalendarPopupClassName[] = L"WindhawkMacOS27CalendarWnd";

void RenderFinderBar();
void CloseControlCenter();
void CloseTrayPopup();
void CloseCalendarPopup();
void CloseAllFinderBarPopups() {
    CloseControlCenter();
    CloseTrayPopup();
    CloseCalendarPopup();
}

void OnFinderBarMenuClosed() {
    if (g_bar.openSlotId != -1) {
        g_bar.openSlotId = -1;
        RenderFinderBar();
    }
}

void FinderBarUpdateActiveApp(HWND fg) {
    if (!fg || !IsWindow(fg)) {
        return;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(fg, &pid);
    if (pid == GetCurrentProcessId()) {
        return;  // Ignore our own Dock / Finder Bar / Glass Menu windows!
    }
    WCHAR cls[64]{};
    GetClassNameW(fg, cls, ARRAYSIZE(cls));
    bool isDesktopOrExplorer =
        _wcsicmp(cls, L"Progman") == 0 || _wcsicmp(cls, L"WorkerW") == 0 ||
        _wcsicmp(cls, L"CabinetWClass") == 0 ||
        _wcsicmp(cls, L"ExploreWClass") == 0;

    std::wstring exePath = GetProcessExePath(pid);
    std::wstring exeName;
    if (!exePath.empty()) {
        const wchar_t* fn = PathFindFileNameW(exePath.c_str());
        exeName = ToLower(fn ? fn : exePath);
    }
    if (exeName == L"explorer.exe" &&
        (_wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
         _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
         _wcsicmp(cls, L"NotifyIconOverflowWindow") == 0)) {
        return;
    }

    std::wstring appName;
    bool isFinder = isDesktopOrExplorer;
    if (isFinder) {
        appName = g_settings.finderBarDesktopName.empty()
                      ? L"Finder"
                      : g_settings.finderBarDesktopName;
    } else {
        auto id = ResolveWindowIdentity(fg);
        appName = id.displayName;
        if (appName.empty() && !exePath.empty()) {
            appName = GetFileDescriptionFromPath(exePath);
        }
        if (appName.empty()) {
            WCHAR title[128]{};
            if (GetWindowTextW(fg, title, ARRAYSIZE(title)) > 0) {
                appName = title;
            } else {
                appName = L"Application";
            }
        }
        if (appName.size() > 26) {
            appName = appName.substr(0, 25) + L"\u2026";
        }
    }

    bool changed = (g_bar.activeHwnd != fg || g_bar.activeAppName != appName ||
                    g_bar.isFinderActive != isFinder);
    g_bar.activeHwnd = fg;
    g_bar.activePid = pid;
    g_bar.activeExePath = exePath;
    g_bar.activeExeName = exeName;
    g_bar.activeAppName = appName;
    g_bar.isFinderActive = isFinder;
    if (changed && g_bar.hwnd) {
        RenderFinderBar();
    }
}

bool PollFinderBarStatus(bool forceFull) {
    auto& st = g_bar.status;
    std::wstring prevClock = st.clockText;
    bool prevHasBat = st.hasBattery;
    int prevBatPct = st.batteryPct;
    bool prevBatChg = st.batteryCharging;
    bool prevBatAc = st.batteryAc;
    int prevNetKind = st.netKind;
    int prevWifiQ = st.wifiQuality;
    bool prevHasAudio = st.hasAudio;
    int prevVol = st.volumePct;
    bool prevMuted = st.muted;

    st.clockText = FormatMacClockString();

    SYSTEM_POWER_STATUS ps{};
    if (GetSystemPowerStatus(&ps) && ps.BatteryFlag != 128 &&
        ps.BatteryFlag != 255 && ps.BatteryLifePercent <= 100) {
        st.hasBattery = true;
        st.batteryPct = ps.BatteryLifePercent;
        st.batteryAc = (ps.ACLineStatus == 1);
        st.batteryCharging = (ps.BatteryFlag & 8) != 0;
        st.batteryRemainSec =
            (ps.BatteryLifeTime == static_cast<DWORD>(-1))
                ? -1
                : static_cast<int>(ps.BatteryLifeTime);
    } else {
        st.hasBattery = false;
    }

    if (forceFull || (st.pollCounter % 3) == 0) {
        PollNetworkStatus(st);
    }
    PollAudioStatus(st);
    if (forceFull || (st.pollCounter % 10) == 0) {
        int b = st.brightnessPct;
        if (GetPrimaryMonitorBrightness(b)) {
            st.hasBrightness = true;
            st.brightnessPct = b;
        }
    }
    st.pollCounter++;

    return forceFull || prevClock != st.clockText ||
           prevHasBat != st.hasBattery || prevBatPct != st.batteryPct ||
           prevBatChg != st.batteryCharging || prevBatAc != st.batteryAc ||
           prevNetKind != st.netKind || prevWifiQ != st.wifiQuality ||
           prevHasAudio != st.hasAudio || prevVol != st.volumePct ||
           prevMuted != st.muted;
}

// ---- Finder Bar Global Menu Builders ----

void AppendKeyCmds(std::vector<MenuEntry>& out, const BarKeyCmd* cmds,
                   size_t count, HWND target) {
    for (size_t i = 0; i < count; ++i) {
        if (!cmds[i].label) {
            out.push_back({MenuEntry::Type::Separator, L"", false, false,
                           nullptr, {}});
            continue;
        }
        auto cmd = cmds[i];
        out.push_back({MenuEntry::Type::Action, cmd.label, false, true,
                       [target, cmd] {
                           SendShortcutToTarget(target, cmd.mods, cmd.vk,
                                                cmd.mods2, cmd.vk2);
                       },
                       {}});
    }
}

std::vector<MenuEntry> BuildLogoBarMenu() {
    std::vector<MenuEntry> m;
    auto sep = [&] {
        m.push_back(
            {MenuEntry::Type::Separator, L"", false, false, nullptr, {}});
    };
    m.push_back({MenuEntry::Type::Action, L"About This PC", false, true,
                 [] { OpenShellUri(L"ms-settings:about"); }, {}});
    sep();
    m.push_back({MenuEntry::Type::Action, L"System Settings\u2026", false, true,
                 [] { OpenShellUri(L"ms-settings:"); }, {}});
    m.push_back({MenuEntry::Type::Action, L"Microsoft Store\u2026", false, true,
                 [] { OpenShellUri(L"ms-windows-store:"); }, {}});
    m.push_back({MenuEntry::Type::Action, L"Desktop & Dock Settings\u2026",
                 false, true,
                 [] {
                     if (HWND dw = FindWindowW(L"WindhawkMacOS27DockWnd", nullptr)) {
                         PostMessageW(dw, g_msgOpenDockSettings, 0, 0);
                     } else {
                         OpenWindhawkUi();
                     }
                 },
                 {}});
    m.push_back({MenuEntry::Type::Action, L"Windhawk Mod Settings\u2026",
                 false, true, [] { OpenWindhawkUi(); }, {}});
    sep();
    m.push_back({MenuEntry::Type::Action, L"Force Quit\u2026\tCtrl+Shift+Esc",
                 false, true, [] { OpenShellUri(L"taskmgr.exe"); }, {}});
    m.push_back({MenuEntry::Type::Action, L"Show Desktop\tWin+D", false, true,
                 [] { SendKeyCombo(kModWin, 'D'); }, {}});
    sep();
    m.push_back({MenuEntry::Type::Action, L"Sleep", false, true,
                 [] {
                     EnablePrivilege(SE_SHUTDOWN_NAME);
                     SetSuspendState(FALSE, FALSE, FALSE);
                 },
                 {}});
    m.push_back({MenuEntry::Type::Action, L"Restart\u2026", false, true,
                 [] {
                     if (MessageBoxW(nullptr, L"Restart Windows now?",
                                     L"Restart",
                                     MB_OKCANCEL | MB_ICONQUESTION |
                                         MB_TOPMOST) == IDOK) {
                         EnablePrivilege(SE_SHUTDOWN_NAME);
                         ExitWindowsEx(EWX_REBOOT,
                                       SHTDN_REASON_MAJOR_OTHER |
                                           SHTDN_REASON_FLAG_PLANNED);
                     }
                 },
                 {}});
    m.push_back({MenuEntry::Type::Action, L"Shut Down\u2026", false, true,
                 [] {
                     if (MessageBoxW(nullptr, L"Shut down Windows now?",
                                     L"Shut Down",
                                     MB_OKCANCEL | MB_ICONQUESTION |
                                         MB_TOPMOST) == IDOK) {
                         EnablePrivilege(SE_SHUTDOWN_NAME);
                         ExitWindowsEx(EWX_SHUTDOWN | EWX_POWEROFF,
                                       SHTDN_REASON_MAJOR_OTHER |
                                           SHTDN_REASON_FLAG_PLANNED);
                     }
                 },
                 {}});
    sep();
    m.push_back({MenuEntry::Type::Action, L"Lock Screen\tWin+L", false, true,
                 [] { LockWorkStation(); }, {}});
    m.push_back({MenuEntry::Type::Action, L"Sign Out\u2026", false, true,
                 [] {
                     if (MessageBoxW(nullptr, L"Sign out of Windows now?",
                                     L"Sign Out",
                                     MB_OKCANCEL | MB_ICONQUESTION |
                                         MB_TOPMOST) == IDOK) {
                         ExitWindowsEx(EWX_LOGOFF, 0);
                     }
                 },
                 {}});
    return m;
}

std::vector<MenuEntry> BuildActiveAppBarMenu() {
    std::vector<MenuEntry> m;
    auto sep = [&] {
        m.push_back(
            {MenuEntry::Type::Separator, L"", false, false, nullptr, {}});
    };
    const std::wstring name = g_bar.activeAppName;
    HWND target = g_bar.activeHwnd;
    DWORD pid = g_bar.activePid;
    std::wstring path = g_bar.activeExePath;

    if (g_bar.isFinderActive) {
        m.push_back({MenuEntry::Type::Action, L"New Finder Window\tCtrl+N",
                     false, true, [] { OpenShellUri(L"explorer.exe"); }, {}});
        m.push_back({MenuEntry::Type::Action, L"Empty Trash\u2026", false,
                     IsRecycleBinNonEmpty(),
                     [] { SHEmptyRecycleBinW(nullptr, nullptr, 0); }, {}});
        sep();
        m.push_back({MenuEntry::Type::Action, L"Hide Others", false, true,
                     [pid] {
                         HideOthersCtx ctx{pid, GetCurrentProcessId()};
                         EnumWindows(HideOtherWindowsProc,
                                     reinterpret_cast<LPARAM>(&ctx));
                     },
                     {}});
        return m;
    }

    m.push_back({MenuEntry::Type::Action, L"Hide " + name, false, true,
                 [pid] {
                     for (HWND h : CollectWindowsForPid(pid)) {
                         ShowWindowAsync(h, SW_MINIMIZE);
                     }
                 },
                 {}});
    m.push_back({MenuEntry::Type::Action, L"Hide Others", false, true,
                 [pid] {
                     HideOthersCtx ctx{pid, GetCurrentProcessId()};
                     EnumWindows(HideOtherWindowsProc,
                                 reinterpret_cast<LPARAM>(&ctx));
                 },
                 {}});
    if (!path.empty()) {
        sep();
        m.push_back({MenuEntry::Type::Action, L"Show in File Explorer", false,
                     true, [path] { ShowInFileExplorer(path); }, {}});
    }
    sep();
    m.push_back({MenuEntry::Type::Action, L"Close Window\tAlt+F4", false,
                 target && IsWindow(target),
                 [target] {
                     if (IsWindow(target)) {
                         PostMessageW(target, WM_CLOSE, 0, 0);
                     }
                 },
                 {}});
    m.push_back({MenuEntry::Type::Action, L"Quit " + name, false, pid != 0,
                 [pid] {
                     for (HWND h : CollectWindowsForPid(pid)) {
                         PostMessageW(h, WM_CLOSE, 0, 0);
                     }
                 },
                 {}});
    m.push_back({MenuEntry::Type::Action, L"Force Quit " + name + L"\u2026",
                 false, pid != 0,
                 [pid] {
                     if (HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, pid)) {
                         TerminateProcess(h, 1);
                         CloseHandle(h);
                     }
                 },
                 {}});
    return m;
}

std::vector<MenuEntry> BuildFinderGoMenu(HWND target) {
    std::vector<MenuEntry> m;
    auto sep = [&] {
        m.push_back(
            {MenuEntry::Type::Separator, L"", false, false, nullptr, {}});
    };
    static const BarKeyCmd kNav[] = {
        {L"Back\tAlt+Left", kModAlt, VK_LEFT},
        {L"Forward\tAlt+Right", kModAlt, VK_RIGHT},
        {L"Enclosing Folder\tAlt+Up", kModAlt, VK_UP},
    };
    AppendKeyCmds(m, kNav, ARRAYSIZE(kNav), target);
    sep();

    auto openKnown = [](REFKNOWNFOLDERID id) {
        PWSTR p = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(id, 0, nullptr, &p)) && p) {
            OpenShellUri(p);
            CoTaskMemFree(p);
        }
    };
    m.push_back({MenuEntry::Type::Action, L"Home", false, true,
                 [openKnown] { openKnown(FOLDERID_Profile); }, {}});
    m.push_back({MenuEntry::Type::Action, L"Desktop", false, true,
                 [openKnown] { openKnown(FOLDERID_Desktop); }, {}});
    m.push_back({MenuEntry::Type::Action, L"Documents", false, true,
                 [openKnown] { openKnown(FOLDERID_Documents); }, {}});
    m.push_back({MenuEntry::Type::Action, L"Downloads", false, true,
                 [openKnown] { openKnown(FOLDERID_Downloads); }, {}});
    m.push_back({MenuEntry::Type::Action, L"Pictures", false, true,
                 [openKnown] { openKnown(FOLDERID_Pictures); }, {}});
    m.push_back({MenuEntry::Type::Action, L"Music", false, true,
                 [openKnown] { openKnown(FOLDERID_Music); }, {}});
    m.push_back({MenuEntry::Type::Action, L"Videos", false, true,
                 [openKnown] { openKnown(FOLDERID_Videos); }, {}});
    sep();
    m.push_back({MenuEntry::Type::Action, L"Computer (This PC)", false, true,
                 [] { OpenShellUri(L"shell:MyComputerFolder"); }, {}});
    m.push_back({MenuEntry::Type::Action, L"Network", false, true,
                 [] { OpenShellUri(L"shell:NetworkPlacesFolder"); }, {}});
    m.push_back({MenuEntry::Type::Action, L"Trash", false, true,
                 [] {
                     OpenShellUri(
                         L"shell:::{645FF040-5081-101B-9F08-00AA002F954E}");
                 },
                 {}});

    DWORD drives = GetLogicalDrives();
    std::vector<MenuEntry> driveSub;
    for (int i = 0; i < 26; ++i) {
        if (!(drives & (1u << i))) {
            continue;
        }
        WCHAR root[4] = {static_cast<WCHAR>(L'A' + i), L':', L'\\', 0};
        WCHAR vol[64]{};
        GetVolumeInformationW(root, vol, ARRAYSIZE(vol), nullptr, nullptr,
                              nullptr, nullptr, 0);
        std::wstring label = vol[0] ? (std::wstring(vol) + L" (" + root + L")")
                                    : std::wstring(root);
        std::wstring rootStr = root;
        driveSub.push_back({MenuEntry::Type::Action, label, false, true,
                            [rootStr] { OpenShellUri(rootStr.c_str()); }, {}});
    }
    if (!driveSub.empty()) {
        sep();
        m.push_back({MenuEntry::Type::Submenu, L"Drives", false, true, nullptr,
                     std::move(driveSub)});
    }
    return m;
}

std::vector<MenuEntry> BuildWindowBarMenu() {
    std::vector<MenuEntry> m;
    auto sep = [&] {
        m.push_back(
            {MenuEntry::Type::Separator, L"", false, false, nullptr, {}});
    };
    HWND target = g_bar.activeHwnd;
    DWORD pid = g_bar.activePid;
    bool valid = target && IsWindow(target) && !g_bar.isFinderActive;
    bool zoomed = valid && IsZoomed(target);

    m.push_back({MenuEntry::Type::Action, L"Minimize", false, valid,
                 [target] {
                     if (IsWindow(target)) ShowWindowAsync(target, SW_MINIMIZE);
                 },
                 {}});
    m.push_back({MenuEntry::Type::Action, zoomed ? L"Restore" : L"Zoom", false,
                 valid,
                 [target, zoomed] {
                     if (IsWindow(target)) {
                         ShowWindowAsync(target,
                                         zoomed ? SW_RESTORE : SW_MAXIMIZE);
                     }
                 },
                 {}});
    m.push_back({MenuEntry::Type::Action, L"Center Window", false, valid,
                 [target] {
                     if (IsWindow(target) && !IsZoomed(target)) {
                         RECT wr{};
                         MONITORINFO mi{.cbSize = sizeof(mi)};
                         if (GetWindowRect(target, &wr) &&
                             GetMonitorInfoW(GetPrimaryMonitor(), &mi)) {
                             int w = wr.right - wr.left;
                             int h = wr.bottom - wr.top;
                             int x = mi.rcWork.left +
                                     ((mi.rcWork.right - mi.rcWork.left) - w) /
                                         2;
                             int y = mi.rcWork.top +
                                     ((mi.rcWork.bottom - mi.rcWork.top) - h) /
                                         2;
                             SetWindowPos(target, nullptr, x, y, 0, 0,
                                          SWP_NOSIZE | SWP_NOZORDER);
                         }
                     }
                 },
                 {}});
    sep();
    m.push_back({MenuEntry::Type::Action, L"Tile Left\tWin+Left", false, valid,
                 [target] { SendShortcutToTarget(target, kModWin, VK_LEFT); },
                 {}});
    m.push_back({MenuEntry::Type::Action, L"Tile Right\tWin+Right", false,
                 valid,
                 [target] { SendShortcutToTarget(target, kModWin, VK_RIGHT); },
                 {}});
    sep();
    m.push_back({MenuEntry::Type::Action, L"Bring All to Front", false,
                 pid != 0,
                 [pid, target] {
                     auto wins = CollectWindowsForPid(pid);
                     ShowAllItemWindows(wins, target);
                 },
                 {}});
    auto wins = CollectWindowsForPid(pid);
    if (!wins.empty()) {
        sep();
        for (size_t i = 0; i < std::min<size_t>(wins.size(), 16); ++i) {
            HWND w = wins[i];
            m.push_back({MenuEntry::Type::Action, GetWindowDisplayTitle(w),
                         w == target, true, [w] { ActivateWindowForce(w); },
                         {}});
        }
    }
    return m;
}

bool IsBrowserExe(const std::wstring& exe) {
    return exe == L"chrome.exe" || exe == L"msedge.exe" ||
           exe == L"brave.exe" || exe == L"firefox.exe" ||
           exe == L"opera.exe" || exe == L"vivaldi.exe" ||
           exe == L"arc.exe" || exe == L"zen.exe";
}

bool IsCodeEditorExe(const std::wstring& exe) {
    return exe == L"code.exe" || exe == L"cursor.exe" ||
           exe == L"windsurf.exe" || exe == L"antigravity.exe";
}

std::vector<MenuEntry> BuildGlobalMenuByTitle(const std::wstring& title,
                                              HMENU win32Sub) {
    HWND target = g_bar.activeHwnd;
    if (win32Sub) {
        auto native = BuildEntriesFromHMenu(win32Sub, target);
        if (!native.empty()) {
            return native;
        }
    }
    if (title == L"Window") {
        return BuildWindowBarMenu();
    }
    if (title == L"Go" && g_bar.isFinderActive) {
        return BuildFinderGoMenu(target);
    }

    std::vector<MenuEntry> m;
    if (g_bar.isFinderActive) {
        if (title == L"File") {
            static const BarKeyCmd k[] = {
                {L"New Finder Window\tCtrl+N", kModCtrl, 'N'},
                {L"New Tab\tCtrl+T", kModCtrl, 'T'},
                {L"New Folder\tCtrl+Shift+N", kModCtrl | kModShift, 'N'},
                {nullptr, 0, 0},
                {L"Open\tEnter", 0, VK_RETURN},
                {L"Get Info\tAlt+Enter", kModAlt, VK_RETURN},
                {L"Rename\tF2", 0, VK_F2},
                {nullptr, 0, 0},
                {L"Move to Trash\tDelete", 0, VK_DELETE},
                {L"Close Window\tCtrl+W", kModCtrl, 'W'},
            };
            AppendKeyCmds(m, k, ARRAYSIZE(k), target);
            return m;
        }
        if (title == L"Edit") {
            static const BarKeyCmd k[] = {
                {L"Undo\tCtrl+Z", kModCtrl, 'Z'},
                {L"Redo\tCtrl+Y", kModCtrl, 'Y'},
                {nullptr, 0, 0},
                {L"Cut\tCtrl+X", kModCtrl, 'X'},
                {L"Copy\tCtrl+C", kModCtrl, 'C'},
                {L"Paste\tCtrl+V", kModCtrl, 'V'},
                {L"Copy as Path\tCtrl+Shift+C", kModCtrl | kModShift, 'C'},
                {L"Select All\tCtrl+A", kModCtrl, 'A'},
                {nullptr, 0, 0},
                {L"Search Folder\tCtrl+E", kModCtrl, 'E'},
            };
            AppendKeyCmds(m, k, ARRAYSIZE(k), target);
            return m;
        }
        if (title == L"View") {
            static const BarKeyCmd k[] = {
                {L"Extra Large Icons\tCtrl+Shift+1", kModCtrl | kModShift, '1'},
                {L"Large Icons\tCtrl+Shift+2", kModCtrl | kModShift, '2'},
                {L"Medium Icons\tCtrl+Shift+3", kModCtrl | kModShift, '3'},
                {L"Small Icons\tCtrl+Shift+4", kModCtrl | kModShift, '4'},
                {L"List\tCtrl+Shift+5", kModCtrl | kModShift, '5'},
                {L"Details\tCtrl+Shift+6", kModCtrl | kModShift, '6'},
                {nullptr, 0, 0},
                {L"Preview Pane\tAlt+P", kModAlt, 'P'},
                {L"Details Pane\tAlt+Shift+P", kModAlt | kModShift, 'P'},
                {nullptr, 0, 0},
                {L"Refresh\tF5", 0, VK_F5},
                {L"Full Screen\tF11", 0, VK_F11},
            };
            AppendKeyCmds(m, k, ARRAYSIZE(k), target);
            return m;
        }
    } else if (IsBrowserExe(g_bar.activeExeName)) {
        if (title == L"File") {
            static const BarKeyCmd k[] = {
                {L"New Tab\tCtrl+T", kModCtrl, 'T'},
                {L"New Window\tCtrl+N", kModCtrl, 'N'},
                {L"New Private Window\tCtrl+Shift+N", kModCtrl | kModShift,
                 'N'},
                {L"Reopen Closed Tab\tCtrl+Shift+T", kModCtrl | kModShift, 'T'},
                {nullptr, 0, 0},
                {L"Open File\u2026\tCtrl+O", kModCtrl, 'O'},
                {L"Open Location\u2026\tCtrl+L", kModCtrl, 'L'},
                {nullptr, 0, 0},
                {L"Close Tab\tCtrl+W", kModCtrl, 'W'},
                {L"Save Page As\u2026\tCtrl+S", kModCtrl, 'S'},
                {L"Print\u2026\tCtrl+P", kModCtrl, 'P'},
            };
            AppendKeyCmds(m, k, ARRAYSIZE(k), target);
            return m;
        }
        if (title == L"View") {
            static const BarKeyCmd k[] = {
                {L"Bookmarks Bar\tCtrl+Shift+B", kModCtrl | kModShift, 'B'},
                {nullptr, 0, 0},
                {L"Reload Page\tF5", 0, VK_F5},
                {L"Hard Reload\tCtrl+F5", kModCtrl, VK_F5},
                {nullptr, 0, 0},
                {L"Zoom In\tCtrl++", kModCtrl, VK_OEM_PLUS},
                {L"Zoom Out\tCtrl+-", kModCtrl, VK_OEM_MINUS},
                {L"Actual Size\tCtrl+0", kModCtrl, '0'},
                {L"Full Screen\tF11", 0, VK_F11},
                {nullptr, 0, 0},
                {L"Developer Tools\tCtrl+Shift+I", kModCtrl | kModShift, 'I'},
            };
            AppendKeyCmds(m, k, ARRAYSIZE(k), target);
            return m;
        }
        if (title == L"History") {
            static const BarKeyCmd k[] = {
                {L"Back\tAlt+Left", kModAlt, VK_LEFT},
                {L"Forward\tAlt+Right", kModAlt, VK_RIGHT},
                {nullptr, 0, 0},
                {L"Show Full History\tCtrl+H", kModCtrl, 'H'},
                {L"Downloads\tCtrl+J", kModCtrl, 'J'},
                {L"Clear Browsing Data\u2026\tCtrl+Shift+Del",
                 kModCtrl | kModShift, VK_DELETE},
            };
            AppendKeyCmds(m, k, ARRAYSIZE(k), target);
            return m;
        }
        if (title == L"Bookmarks") {
            static const BarKeyCmd k[] = {
                {L"Bookmark Manager\tCtrl+Shift+O", kModCtrl | kModShift, 'O'},
                {nullptr, 0, 0},
                {L"Bookmark This Tab\u2026\tCtrl+D", kModCtrl, 'D'},
            };
            AppendKeyCmds(m, k, ARRAYSIZE(k), target);
            return m;
        }
    } else if (IsCodeEditorExe(g_bar.activeExeName)) {
        if (title == L"File") {
            static const BarKeyCmd k[] = {
                {L"New Text File\tCtrl+N", kModCtrl, 'N'},
                {L"New Window\tCtrl+Shift+N", kModCtrl | kModShift, 'N'},
                {nullptr, 0, 0},
                {L"Open File\u2026\tCtrl+O", kModCtrl, 'O'},
                {L"Open Folder\u2026\tCtrl+K O", kModCtrl, 'K', kModCtrl, 'O'},
                {nullptr, 0, 0},
                {L"Save\tCtrl+S", kModCtrl, 'S'},
                {L"Save As\u2026\tCtrl+Shift+S", kModCtrl | kModShift, 'S'},
                {L"Preferences: Settings\tCtrl+,", kModCtrl, VK_OEM_COMMA},
                {nullptr, 0, 0},
                {L"Close Editor\tCtrl+W", kModCtrl, 'W'},
            };
            AppendKeyCmds(m, k, ARRAYSIZE(k), target);
            return m;
        }
        if (title == L"Terminal") {
            static const BarKeyCmd k[] = {
                {L"New Terminal\tCtrl+Shift+`", kModCtrl | kModShift,
                 VK_OEM_3},
                {L"Toggle Terminal\tCtrl+`", kModCtrl, VK_OEM_3},
                {L"Command Palette\tCtrl+Shift+P", kModCtrl | kModShift, 'P'},
            };
            AppendKeyCmds(m, k, ARRAYSIZE(k), target);
            return m;
        }
    }

    // Universal fallback menus (File / Edit / View / Help)
    if (title == L"File") {
        static const BarKeyCmd k[] = {
            {L"New\tCtrl+N", kModCtrl, 'N'},
            {L"Open\u2026\tCtrl+O", kModCtrl, 'O'},
            {nullptr, 0, 0},
            {L"Save\tCtrl+S", kModCtrl, 'S'},
            {L"Save As\u2026\tCtrl+Shift+S", kModCtrl | kModShift, 'S'},
            {L"Print\u2026\tCtrl+P", kModCtrl, 'P'},
            {nullptr, 0, 0},
            {L"Close Window\tAlt+F4", kModAlt, VK_F4},
        };
        AppendKeyCmds(m, k, ARRAYSIZE(k), target);
    } else if (title == L"Edit") {
        static const BarKeyCmd k[] = {
            {L"Undo\tCtrl+Z", kModCtrl, 'Z'},
            {L"Redo\tCtrl+Y", kModCtrl, 'Y'},
            {nullptr, 0, 0},
            {L"Cut\tCtrl+X", kModCtrl, 'X'},
            {L"Copy\tCtrl+C", kModCtrl, 'C'},
            {L"Paste\tCtrl+V", kModCtrl, 'V'},
            {L"Select All\tCtrl+A", kModCtrl, 'A'},
            {nullptr, 0, 0},
            {L"Find\u2026\tCtrl+F", kModCtrl, 'F'},
            {L"Emoji & Symbols\tWin+.", kModWin, VK_OEM_PERIOD},
        };
        AppendKeyCmds(m, k, ARRAYSIZE(k), target);
    } else if (title == L"View") {
        static const BarKeyCmd k[] = {
            {L"Zoom In\tCtrl++", kModCtrl, VK_OEM_PLUS},
            {L"Zoom Out\tCtrl+-", kModCtrl, VK_OEM_MINUS},
            {L"Actual Size\tCtrl+0", kModCtrl, '0'},
            {nullptr, 0, 0},
            {L"Full Screen\tF11", 0, VK_F11},
        };
        AppendKeyCmds(m, k, ARRAYSIZE(k), target);
    } else {
        static const BarKeyCmd k[] = {
            {L"Help\tF1", 0, VK_F1},
        };
        AppendKeyCmds(m, k, ARRAYSIZE(k), target);
        m.push_back(
            {MenuEntry::Type::Separator, L"", false, false, nullptr, {}});
        m.push_back({MenuEntry::Type::Action, L"Dock & Finder Settings\u2026",
                     false, true, [] { OpenWindhawkUi(); }, {}});
    }
    return m;
}

// ---- Mirrored System Tray Client (windhawk.exe side) ----

void ClearMirroredTrayIcons() {
    for (auto& ti : g_bar.trayIcons) {
        if (ti.hIcon) {
            DestroyIcon(ti.hIcon);
            ti.hIcon = nullptr;
        }
    }
    g_bar.trayIcons.clear();
}

void PruneDeadTrayIcons() {
    for (auto it = g_bar.trayIcons.begin(); it != g_bar.trayIcons.end();) {
        if (!it->ownerHwnd || !IsWindow(it->ownerHwnd)) {
            if (it->hIcon) {
                DestroyIcon(it->hIcon);
            }
            it = g_bar.trayIcons.erase(it);
        } else {
            ++it;
        }
    }
}

void ProcessTrayIpcPacket(const BYTE* data, DWORD sz) {
    if (!data || sz < offsetof(TrayIpcMsg, nid) + 24) {
        return;
    }
    TrayIpcMsg msg{};
    memcpy(&msg, data, std::min<size_t>(sz, sizeof(msg)));
    if (msg.signature != 0x34753423u) {
        return;
    }
    HWND owner =
        reinterpret_cast<HWND>(static_cast<LONG_PTR>(static_cast<LONG>(
            msg.nid.hWnd)));
    if (!owner || !IsWindow(owner)) {
        return;
    }
    bool hasGuid = (msg.nid.uFlags & NIF_GUID) != 0;
    auto findIt = std::find_if(
        g_bar.trayIcons.begin(), g_bar.trayIcons.end(),
        [&](const MirroredTrayIcon& t) {
            if (hasGuid && t.hasGuid &&
                memcmp(&t.guid, &msg.nid.guidItem, sizeof(GUID)) == 0) {
                return true;
            }
            return t.ownerHwnd == owner && t.uID == msg.nid.uID;
        });

    if (msg.message == NIM_DELETE) {
        if (findIt != g_bar.trayIcons.end()) {
            if (findIt->hIcon) {
                DestroyIcon(findIt->hIcon);
            }
            g_bar.trayIcons.erase(findIt);
            RenderFinderBar();
        }
        return;
    }

    if (msg.message == NIM_ADD || msg.message == NIM_MODIFY ||
        msg.message == NIM_SETVERSION) {
        MirroredTrayIcon* ti = nullptr;
        if (findIt != g_bar.trayIcons.end()) {
            ti = &(*findIt);
        } else if (msg.message == NIM_ADD || msg.message == NIM_MODIFY) {
            MirroredTrayIcon fresh;
            fresh.ownerHwnd = owner;
            fresh.uID = msg.nid.uID;
            GetWindowThreadProcessId(owner, &fresh.pid);
            g_bar.trayIcons.push_back(std::move(fresh));
            ti = &g_bar.trayIcons.back();
        }
        if (!ti) {
            return;
        }
        if (hasGuid) {
            ti->guid = msg.nid.guidItem;
            ti->hasGuid = true;
        }
        if (msg.nid.uFlags & NIF_MESSAGE) {
            ti->callbackMsg = msg.nid.uCallbackMessage;
        }
        if (msg.nid.uFlags & NIF_TIP) {
            WCHAR tipBuf[129]{};
            wcsncpy_s(tipBuf, msg.nid.szTip, 128);
            ti->tip = tipBuf;
        }
        if (msg.nid.uFlags & NIF_STATE) {
            if (msg.nid.dwStateMask & NIS_HIDDEN) {
                ti->hidden = (msg.nid.dwState & NIS_HIDDEN) != 0;
            }
        }
        if (msg.message == NIM_SETVERSION) {
            ti->version = msg.nid.uVersion;
        }
        if ((msg.nid.uFlags & NIF_ICON) && msg.nid.hIcon) {
            HICON raw = reinterpret_cast<HICON>(
                static_cast<LONG_PTR>(static_cast<LONG>(msg.nid.hIcon)));
            HICON copy = CopyIcon(raw);
            if (copy) {
                if (ti->hIcon) {
                    DestroyIcon(ti->hIcon);
                }
                ti->hIcon = copy;
            }
        }
        RenderFinderBar();
    }
}

void RequestShellTrayIcons() {
    g_bar.ignoreTaskbarCreatedUntil = GetTickCount64() + 4500;
    if (g_dock.taskbarCreatedMsg) {
        PostMessageW(HWND_BROADCAST, g_dock.taskbarCreatedMsg, 0, 0);
    }
}

LRESULT CALLBACK TrayClientWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_COPYDATA) {
        auto* cds = reinterpret_cast<COPYDATASTRUCT*>(lp);
        if (!cds) {
            return FALSE;
        }
        if (cds->dwData == kTrayIpcHello) {
            RequestShellTrayIcons();
            return TRUE;
        }
        if (cds->dwData == kTrayIpcFwd && cds->lpData && cds->cbData >= 8) {
            ProcessTrayIpcPacket(
                reinterpret_cast<const BYTE*>(cds->lpData), cds->cbData);
            return TRUE;
        }
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void DispatchTrayIconClick(const MirroredTrayIcon& ti, UINT mouseMsg,
                           POINT screenPt) {
    if (!ti.ownerHwnd || !IsWindow(ti.ownerHwnd) || !ti.callbackMsg) {
        return;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(ti.ownerHwnd, &pid);
    if (pid) {
        AllowSetForegroundWindow(pid);
    }
    if (ti.version >= NOTIFYICON_VERSION_4) {
        WPARAM w = MAKEWPARAM(screenPt.x, screenPt.y);
        LPARAM l = MAKELPARAM(mouseMsg, ti.uID);
        PostMessageW(ti.ownerHwnd, ti.callbackMsg, w, l);
        if (mouseMsg == WM_RBUTTONUP) {
            PostMessageW(ti.ownerHwnd, ti.callbackMsg, w,
                         MAKELPARAM(WM_CONTEXTMENU, ti.uID));
        } else if (mouseMsg == WM_LBUTTONUP) {
            PostMessageW(ti.ownerHwnd, ti.callbackMsg, w,
                         MAKELPARAM(NIN_SELECT, ti.uID));
        }
    } else {
        PostMessageW(ti.ownerHwnd, ti.callbackMsg, ti.uID, mouseMsg);
    }
}

// ---- Reusable Glass Popup Shell Helper (Control Center / Tray / Calendar) ----

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

// ---- Control Center Popup (macOS 27 Liquid Glass) ----

void RenderControlCenter() {
    if (!g_cc.open || !g_cc.uiSurface || !g_gfx.dwriteFactory) {
        return;
    }
    const float s = g_dock.scale;
    const float W = g_cc.widthPx;
    const bool dark = g_dock.darkMode;
    auto acc = GetAccentColor();
    D2D1_COLOR_F accCol =
        D2D1::ColorF(acc.R / 255.0f, acc.G / 255.0f, acc.B / 255.0f, 1.0f);
    D2D1_COLOR_F fg = dark ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.94f)
                           : D2D1::ColorF(0.08f, 0.08f, 0.11f, 0.94f);
    D2D1_COLOR_F fgSub = dark ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.58f)
                              : D2D1::ColorF(0.08f, 0.08f, 0.11f, 0.56f);
    D2D1_COLOR_F cardBg = dark ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.085f)
                               : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.44f);
    D2D1_COLOR_F cardHov = dark ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.145f)
                                : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.62f);
    D2D1_COLOR_F cardRim = dark ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.15f)
                                : D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.08f);

    DrawCompositionSurface(g_cc.uiSurface, [&](ID2D1DeviceContext* dc) {
        winrt::com_ptr<ID2D1SolidColorBrush> br;
        dc->CreateSolidColorBrush(fg, br.put());

        auto makeFmt = [&](float sz, DWRITE_FONT_WEIGHT wt,
                           DWRITE_TEXT_ALIGNMENT al =
                               DWRITE_TEXT_ALIGNMENT_LEADING) {
            winrt::com_ptr<IDWriteTextFormat> fmt;
            g_gfx.dwriteFactory->CreateTextFormat(
                ResolveFontFamily().c_str(), nullptr, wt,
                DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, sz,
                L"en-us", fmt.put());
            if (fmt) {
                fmt->SetTextAlignment(al);
                fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            }
            return fmt;
        };
        auto fTitle = makeFmt(12.5f * s, DWRITE_FONT_WEIGHT_SEMI_BOLD);
        auto fSub = makeFmt(10.5f * s, DWRITE_FONT_WEIGHT_NORMAL);
        auto fCenter = makeFmt(12.0f * s, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                               DWRITE_TEXT_ALIGNMENT_CENTER);
        auto fGlyph = makeFmt(15.0f * s, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                              DWRITE_TEXT_ALIGNMENT_CENTER);

        auto drawCard = [&](const D2D1_RECT_F& r, float rad, bool active,
                            bool hovered) {
            D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(r, rad, rad);
            if (active) {
                br->SetColor(D2D1::ColorF(accCol.r, accCol.g, accCol.b,
                                          hovered ? 0.90f : 0.80f));
            } else {
                br->SetColor(hovered ? cardHov : cardBg);
            }
            dc->FillRoundedRectangle(rr, br.get());
            br->SetColor(cardRim);
            dc->DrawRoundedRectangle(rr, br.get(), 1.0f * s);
        };

        const float pad = 12.0f * s;
        const float gap = 10.0f * s;
        const float innerW = W - 2.0f * pad;
        const float halfW = (innerW - gap) * 0.5f;
        const float cardR = 14.0f * s;
        float y = pad;

        // Top row: Left Connectivity card (136*s) | Right DarkMode + 2 mini tiles
        const float topH = 134.0f * s;
        D2D1_RECT_F connRc{pad, y, pad + halfW, y + topH};
        drawCard(connRc, cardR, false, false);

        float rowH = topH / 3.0f;
        g_cc.rcWifi = {connRc.left + 6.0f * s, connRc.top + 4.0f * s,
                       connRc.right - 6.0f * s, connRc.top + rowH - 2.0f * s};
        g_cc.rcNet = {connRc.left + 6.0f * s, connRc.top + rowH + 2.0f * s,
                      connRc.right - 6.0f * s,
                      connRc.top + rowH * 2.0f - 2.0f * s};
        g_cc.rcBt = {connRc.left + 6.0f * s,
                     connRc.top + rowH * 2.0f + 2.0f * s,
                     connRc.right - 6.0f * s, connRc.bottom - 4.0f * s};

        auto drawConnRow = [&](const D2D1_RECT_F& rr, PCWSTR iconTxt,
                               const std::wstring& title,
                               const std::wstring& sub, bool on, int zone) {
            if (g_cc.hoverZone == zone) {
                br->SetColor(cardHov);
                dc->FillRoundedRectangle(D2D1::RoundedRect(rr, 9 * s, 9 * s),
                                         br.get());
            }
            float cy = (rr.top + rr.bottom) * 0.5f;
            float cx = rr.left + 16.0f * s;
            br->SetColor(on ? accCol
                            : (dark ? D2D1::ColorF(1, 1, 1, 0.14f)
                                    : D2D1::ColorF(0, 0, 0, 0.10f)));
            dc->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), 13 * s, 13 * s),
                            br.get());
            br->SetColor(on ? D2D1::ColorF(1, 1, 1, 0.98f) : fg);
            dc->DrawText(iconTxt, static_cast<UINT32>(wcslen(iconTxt)),
                         fCenter.get(),
                         D2D1::RectF(cx - 13 * s, cy - 12 * s, cx + 13 * s,
                                     cy + 12 * s),
                         br.get());
            D2D1_RECT_F tr{cx + 19 * s, rr.top + 3 * s, rr.right - 4 * s,
                           cy + 1 * s};
            D2D1_RECT_F sr{cx + 19 * s, cy - 1 * s, rr.right - 4 * s,
                           rr.bottom - 3 * s};
            br->SetColor(fg);
            dc->DrawText(title.c_str(), static_cast<UINT32>(title.size()),
                         fTitle.get(), tr, br.get());
            br->SetColor(fgSub);
            dc->DrawText(sub.c_str(), static_cast<UINT32>(sub.size()),
                         fSub.get(), sr, br.get());
        };

        bool wifiOn = (g_bar.status.netKind == 1);
        std::wstring wifiSub =
            wifiOn ? (g_bar.status.wifiSsid.empty() ? L"Connected"
                                                    : g_bar.status.wifiSsid)
                   : (g_bar.status.netKind == 2 ? L"Ethernet" : L"Off");
        drawConnRow(g_cc.rcWifi, L"\u25CE", L"Wi-Fi", wifiSub, wifiOn, 1);
        drawConnRow(g_cc.rcNet, L"\u21C4", L"Network",
                    g_bar.status.netKind ? L"Online" : L"Offline",
                    g_bar.status.netKind != 0, 2);
        drawConnRow(g_cc.rcBt, L"B", L"Bluetooth", L"Settings", true, 3);

        // Top-right stack: Dark Mode toggle + Freeze Glass + Focus
        float rx = pad + halfW + gap;
        float topCardH = 62.0f * s;
        g_cc.rcDarkMode = {rx, y, rx + halfW, y + topCardH};
        drawCard(g_cc.rcDarkMode, cardR, dark, g_cc.hoverZone == 4);
        {
            br->SetColor(dark ? D2D1::ColorF(1, 1, 1, 0.98f) : fg);
            std::wstring lbl = dark ? L"\u263E  Dark Mode" : L"\u2600  Light Mode";
            dc->DrawText(lbl.c_str(), static_cast<UINT32>(lbl.size()),
                         fCenter.get(), g_cc.rcDarkMode, br.get());
        }

        float miniY = y + topCardH + gap;
        float miniH = topH - topCardH - gap;
        float miniW = (halfW - gap) * 0.5f;
        g_cc.rcFreeze = {rx, miniY, rx + miniW, miniY + miniH};
        g_cc.rcFocus = {rx + miniW + gap, miniY, rx + halfW, miniY + miniH};
        drawCard(g_cc.rcFreeze, cardR, g_liquidCapture.frozen,
                 g_cc.hoverZone == 5);
        drawCard(g_cc.rcFocus, cardR, false, g_cc.hoverZone == 6);
        {
            br->SetColor(g_liquidCapture.frozen ? D2D1::ColorF(1, 1, 1, 0.98f)
                                                : fg);
            PCWSTR fTxt = g_liquidCapture.frozen ? L"Frozen" : L"Freeze";
            dc->DrawText(fTxt, static_cast<UINT32>(wcslen(fTxt)), fCenter.get(),
                         g_cc.rcFreeze, br.get());
            br->SetColor(fg);
            PCWSTR dTxt = L"Focus";
            dc->DrawText(dTxt, static_cast<UINT32>(wcslen(dTxt)), fCenter.get(),
                         g_cc.rcFocus, br.get());
        }

        y += topH + gap;

        // Sliders helper
        auto drawSliderCard = [&](PCWSTR title, int pct, bool muted,
                                  D2D1_RECT_F& outTrack,
                                  D2D1_RECT_F* outLeftBtn, PCWSTR glyph) {
            float sh = 58.0f * s;
            D2D1_RECT_F card{pad, y, pad + innerW, y + sh};
            drawCard(card, cardR, false, false);
            br->SetColor(fg);
            dc->DrawText(title, static_cast<UINT32>(wcslen(title)),
                         fTitle.get(),
                         D2D1::RectF(card.left + 12 * s, card.top + 6 * s,
                                     card.right - 46 * s, card.top + 24 * s),
                         br.get());
            std::wstring pctTxt = muted ? L"Muted"
                                        : (std::to_wstring(pct) + L"%");
            br->SetColor(fgSub);
            auto fRight = makeFmt(11.0f * s, DWRITE_FONT_WEIGHT_NORMAL,
                                  DWRITE_TEXT_ALIGNMENT_TRAILING);
            dc->DrawText(pctTxt.c_str(), static_cast<UINT32>(pctTxt.size()),
                         fRight.get(),
                         D2D1::RectF(card.right - 56 * s, card.top + 6 * s,
                                     card.right - 12 * s, card.top + 24 * s),
                         br.get());

            float trH = 22.0f * s;
            float trY = card.bottom - trH - 9.0f * s;
            outTrack = {card.left + 12 * s, trY, card.right - 12 * s,
                        trY + trH};
            if (outLeftBtn) {
                *outLeftBtn = {outTrack.left, outTrack.top,
                               outTrack.left + trH, outTrack.bottom};
            }
            float trR = trH * 0.5f;
            br->SetColor(dark ? D2D1::ColorF(1, 1, 1, 0.13f)
                              : D2D1::ColorF(0, 0, 0, 0.10f));
            dc->FillRoundedRectangle(D2D1::RoundedRect(outTrack, trR, trR),
                                     br.get());

            float frac = muted ? 0.0f : (std::clamp(pct, 0, 100) / 100.0f);
            float fillW = std::max(trH, (outTrack.right - outTrack.left) * frac);
            D2D1_RECT_F fillRc{outTrack.left, outTrack.top,
                               outTrack.left + fillW, outTrack.bottom};
            br->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, dark ? 0.88f : 0.96f));
            dc->FillRoundedRectangle(D2D1::RoundedRect(fillRc, trR, trR),
                                     br.get());

            br->SetColor(D2D1::ColorF(0.12f, 0.13f, 0.16f, 0.88f));
            dc->DrawText(glyph, static_cast<UINT32>(wcslen(glyph)),
                         fCenter.get(),
                         D2D1::RectF(outTrack.left + 2 * s, outTrack.top,
                                     outTrack.left + trH, outTrack.bottom),
                         br.get());
            y += sh + gap;
        };

        drawSliderCard(L"Display", g_bar.status.brightnessPct, false,
                       g_cc.rcBrightTrack, nullptr, L"\u2600");
        drawSliderCard(L"Sound", g_bar.status.volumePct, g_bar.status.muted,
                       g_cc.rcVolTrack, &g_cc.rcVolMuteBtn,
                       g_bar.status.muted ? L"\u2715" : L"\u266A");

        // Media & Quick Actions strip
        float mediaH = 42.0f * s;
        float btnW = (innerW - gap * 2.0f) / 3.0f;
        g_cc.rcPrev = {pad, y, pad + btnW, y + mediaH};
        g_cc.rcPlay = {pad + btnW + gap, y, pad + btnW * 2.0f + gap,
                       y + mediaH};
        g_cc.rcNext = {pad + (btnW + gap) * 2.0f, y, pad + innerW, y + mediaH};
        drawCard(g_cc.rcPrev, 11 * s, false, g_cc.hoverZone == 10);
        drawCard(g_cc.rcPlay, 11 * s, false, g_cc.hoverZone == 11);
        drawCard(g_cc.rcNext, 11 * s, false, g_cc.hoverZone == 12);
        br->SetColor(fg);
        dc->DrawText(L"\u23EE", 1, fGlyph.get(), g_cc.rcPrev, br.get());
        dc->DrawText(L"\u23EF", 1, fGlyph.get(), g_cc.rcPlay, br.get());
        dc->DrawText(L"\u23ED", 1, fGlyph.get(), g_cc.rcNext, br.get());
        y += mediaH + gap;

        // Bottom quick actions row (4 pills)
        float qaH = 38.0f * s;
        float qaW = (innerW - gap * 3.0f) / 4.0f;
        g_cc.rcShot = {pad, y, pad + qaW, y + qaH};
        g_cc.rcLock = {pad + (qaW + gap), y, pad + qaW * 2 + gap, y + qaH};
        g_cc.rcTaskMgr = {pad + (qaW + gap) * 2, y, pad + qaW * 3 + gap * 2,
                          y + qaH};
        g_cc.rcSettings = {pad + (qaW + gap) * 3, y, pad + innerW, y + qaH};
        drawCard(g_cc.rcShot, 11 * s, false, g_cc.hoverZone == 20);
        drawCard(g_cc.rcLock, 11 * s, false, g_cc.hoverZone == 21);
        drawCard(g_cc.rcTaskMgr, 11 * s, false, g_cc.hoverZone == 22);
        drawCard(g_cc.rcSettings, 11 * s, false, g_cc.hoverZone == 23);
        br->SetColor(fg);
        dc->DrawText(L"Shot", 4, fCenter.get(), g_cc.rcShot, br.get());
        dc->DrawText(L"Lock", 4, fCenter.get(), g_cc.rcLock, br.get());
        dc->DrawText(L"Tasks", 5, fCenter.get(), g_cc.rcTaskMgr, br.get());
        dc->DrawText(L"Prefs", 5, fCenter.get(), g_cc.rcSettings, br.get());
    });
}

void CloseControlCenter() {
    if (!g_cc.open && !g_cc.hwnd) {
        return;
    }
    g_cc.open = false;
    g_cc.draggingSlider = 0;
    g_cc.liquidPlate.Shutdown();
    g_cc.uiVisual = nullptr;
    g_cc.uiBrush = nullptr;
    g_cc.uiSurface = nullptr;
    g_cc.fallbackTint = nullptr;
    g_cc.fallbackBackdrop = nullptr;
    g_cc.clipGeom = nullptr;
    g_cc.glassContainer = nullptr;
    g_cc.root = nullptr;
    g_cc.target = nullptr;
    if (g_cc.hwnd) {
        ShowWindow(g_cc.hwnd, SW_HIDE);
        DestroyWindow(g_cc.hwnd);
        g_cc.hwnd = nullptr;
    }
    if (g_bar.openSlotId == static_cast<int>(BarItemId::ControlCenter)) {
        g_bar.openSlotId = -1;
        RenderFinderBar();
    }
}

void ApplyControlCenterSliderAtX(int slider, float lx) {
    const D2D1_RECT_F& tr =
        (slider == 1) ? g_cc.rcBrightTrack : g_cc.rcVolTrack;
    float span = std::max(1.0f, tr.right - tr.left);
    int pct = std::clamp(
        static_cast<int>(std::lround(((lx - tr.left) / span) * 100.0f)), 0,
        100);
    if (slider == 1) {
        g_bar.status.brightnessPct = pct;
        SetPrimaryMonitorBrightness(pct);
    } else if (slider == 2) {
        g_bar.status.volumePct = pct;
        g_bar.status.muted = (pct == 0);
        SetSystemVolumePct(pct);
        RenderFinderBar();
    }
    RenderControlCenter();
}

LRESULT CALLBACK ControlCenterWndProc(HWND hwnd, UINT msg, WPARAM wp,
                                      LPARAM lp) {
    try {
        switch (msg) {
            case WM_ACTIVATE:
                if (LOWORD(wp) == WA_INACTIVE) {
                    CloseControlCenter();
                }
                return 0;
            case WM_KEYDOWN:
                if (wp == VK_ESCAPE) {
                    CloseControlCenter();
                }
                return 0;
            case WM_MOUSEMOVE: {
                float lx = static_cast<float>(static_cast<short>(LOWORD(lp)));
                float ly = static_cast<float>(static_cast<short>(HIWORD(lp)));
                if (g_cc.draggingSlider) {
                    ApplyControlCenterSliderAtX(g_cc.draggingSlider, lx);
                    return 0;
                }
                int hz = -1;
                if (PointInRectF(g_cc.rcWifi, lx, ly)) hz = 1;
                else if (PointInRectF(g_cc.rcNet, lx, ly)) hz = 2;
                else if (PointInRectF(g_cc.rcBt, lx, ly)) hz = 3;
                else if (PointInRectF(g_cc.rcDarkMode, lx, ly)) hz = 4;
                else if (PointInRectF(g_cc.rcFreeze, lx, ly)) hz = 5;
                else if (PointInRectF(g_cc.rcFocus, lx, ly)) hz = 6;
                else if (PointInRectF(g_cc.rcPrev, lx, ly)) hz = 10;
                else if (PointInRectF(g_cc.rcPlay, lx, ly)) hz = 11;
                else if (PointInRectF(g_cc.rcNext, lx, ly)) hz = 12;
                else if (PointInRectF(g_cc.rcShot, lx, ly)) hz = 20;
                else if (PointInRectF(g_cc.rcLock, lx, ly)) hz = 21;
                else if (PointInRectF(g_cc.rcTaskMgr, lx, ly)) hz = 22;
                else if (PointInRectF(g_cc.rcSettings, lx, ly)) hz = 23;
                if (hz != g_cc.hoverZone) {
                    g_cc.hoverZone = hz;
                    RenderControlCenter();
                }
                return 0;
            }
            case WM_LBUTTONDOWN: {
                float lx = static_cast<float>(static_cast<short>(LOWORD(lp)));
                float ly = static_cast<float>(static_cast<short>(HIWORD(lp)));
                if (PointInRectF(g_cc.rcBrightTrack, lx, ly)) {
                    g_cc.draggingSlider = 1;
                    SetCapture(hwnd);
                    ApplyControlCenterSliderAtX(1, lx);
                    return 0;
                }
                if (PointInRectF(g_cc.rcVolMuteBtn, lx, ly)) {
                    ToggleSystemMute();
                    PollAudioStatus(g_bar.status);
                    RenderControlCenter();
                    RenderFinderBar();
                    return 0;
                }
                if (PointInRectF(g_cc.rcVolTrack, lx, ly)) {
                    g_cc.draggingSlider = 2;
                    SetCapture(hwnd);
                    ApplyControlCenterSliderAtX(2, lx);
                    return 0;
                }
                if (PointInRectF(g_cc.rcWifi, lx, ly)) {
                    CloseControlCenter();
                    OpenShellUri(L"ms-availablenetworks:");
                } else if (PointInRectF(g_cc.rcNet, lx, ly)) {
                    CloseControlCenter();
                    OpenShellUri(L"ms-settings:network");
                } else if (PointInRectF(g_cc.rcBt, lx, ly)) {
                    CloseControlCenter();
                    OpenShellUri(L"ms-settings:bluetooth");
                } else if (PointInRectF(g_cc.rcDarkMode, lx, ly)) {
                    ToggleWindowsDarkModeLive();
                    UpdateGlassAppearance();
                    RenderControlCenter();
                    RenderFinderBar();
                } else if (PointInRectF(g_cc.rcFreeze, lx, ly)) {
                    g_liquidCapture.frozen = !g_liquidCapture.frozen;
                    if (g_liquidCapture.frozen) {
                        ApplyCaptureExclusionToAllModWindows(false);
                    } else {
                        g_liquidCapture.excludedApplied = false;
                    }
                    g_bar.liquidPlate.dirty = true;
                    RenderControlCenter();
                    RenderFinderBar();
                } else if (PointInRectF(g_cc.rcFocus, lx, ly)) {
                    CloseControlCenter();
                    OpenShellUri(L"ms-settings:quiethours");
                } else if (PointInRectF(g_cc.rcPrev, lx, ly)) {
                    SendKeyCombo(0, VK_MEDIA_PREV_TRACK);
                } else if (PointInRectF(g_cc.rcPlay, lx, ly)) {
                    SendKeyCombo(0, VK_MEDIA_PLAY_PAUSE);
                } else if (PointInRectF(g_cc.rcNext, lx, ly)) {
                    SendKeyCombo(0, VK_MEDIA_NEXT_TRACK);
                } else if (PointInRectF(g_cc.rcShot, lx, ly)) {
                    CloseControlCenter();
                    SendKeyCombo(kModWin | kModShift, 'S');
                } else if (PointInRectF(g_cc.rcLock, lx, ly)) {
                    CloseControlCenter();
                    LockWorkStation();
                } else if (PointInRectF(g_cc.rcTaskMgr, lx, ly)) {
                    CloseControlCenter();
                    OpenShellUri(L"taskmgr.exe");
                } else if (PointInRectF(g_cc.rcSettings, lx, ly)) {
                    CloseControlCenter();
                    OpenWindhawkUi();
                }
                return 0;
            }
            case WM_LBUTTONUP:
                if (g_cc.draggingSlider) {
                    g_cc.draggingSlider = 0;
                    ReleaseCapture();
                }
                return 0;
        }
    } catch (...) {
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void OpenControlCenterPopup(int rightEdgeScreenX) {
    CloseAllMenus();
    CloseAllFinderBarPopups();
    PollFinderBarStatus(true);

    const float s = g_dock.scale;
    float w = std::ceil(324.0f * s);
    float h = std::ceil(384.0f * s);
    const RECT& mr = g_dock.monitorRect;
    int px = std::clamp(rightEdgeScreenX - static_cast<int>(w),
                        static_cast<int>(mr.left + 8),
                        static_cast<int>(mr.right - w - 8));
    int py = static_cast<int>(mr.top + g_bar.heightPx + 6.0f * s);

    HMODULE hSelf = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&OpenControlCenterPopup),
                       &hSelf);
    static bool s_reg = false;
    if (!s_reg) {
        WNDCLASSEXW wc{.cbSize = sizeof(wc),
                       .lpfnWndProc = ControlCenterWndProc,
                       .hInstance = hSelf,
                       .hCursor = LoadCursorW(nullptr, IDC_ARROW),
                       .lpszClassName = kControlCenterClassName};
        RegisterClassExW(&wc);
        s_reg = true;
    }

    g_cc.hwnd = CreateWindowExW(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_LAYERED | WS_EX_TOPMOST |
            WS_EX_TOOLWINDOW,
        kControlCenterClassName, L"", WS_POPUP, px, py, static_cast<int>(w),
        static_cast<int>(h), nullptr, nullptr, hSelf, nullptr);
    if (!g_cc.hwnd) {
        return;
    }
    g_cc.widthPx = w;
    g_cc.heightPx = h;
    g_cc.posX = px;
    g_cc.posY = py;
    g_cc.hoverZone = -1;
    g_cc.open = true;

    InitGlassPopupWindowShell(g_cc.hwnd, w, h, 18.0f * s, px, py,
                              g_dock.darkMode ? 0.22f : 0.18f, g_cc.target,
                              g_cc.root, g_cc.glassContainer, g_cc.clipGeom,
                              g_cc.fallbackBackdrop, g_cc.fallbackTint,
                              g_cc.liquidPlate, g_cc.uiSurface, g_cc.uiBrush,
                              g_cc.uiVisual);
    RenderControlCenter();
    ShowWindow(g_cc.hwnd, SW_SHOW);
    SetForegroundWindow(g_cc.hwnd);
    g_bar.openSlotId = static_cast<int>(BarItemId::ControlCenter);
    RenderFinderBar();
}

// ---- Mirrored System Tray Popup (macOS 27 Liquid Glass) ----

void RenderTrayPopup() {
    if (!g_trayPop.open || !g_trayPop.uiSurface) {
        return;
    }
    const float s = g_dock.scale;
    const bool dark = g_dock.darkMode;
    DrawCompositionSurface(g_trayPop.uiSurface, [&](ID2D1DeviceContext* dc) {
        winrt::com_ptr<ID2D1SolidColorBrush> br;
        dc->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 0.14f), br.put());

        if (g_trayPop.visibleIdx.empty()) {
            winrt::com_ptr<IDWriteTextFormat> fmt;
            g_gfx.dwriteFactory->CreateTextFormat(
                ResolveFontFamily().c_str(), nullptr, DWRITE_FONT_WEIGHT_NORMAL,
                DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f * s,
                L"en-us", fmt.put());
            if (fmt) {
                fmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                br->SetColor(dark ? D2D1::ColorF(1, 1, 1, 0.65f)
                                  : D2D1::ColorF(0.1f, 0.1f, 0.12f, 0.65f));
                PCWSTR msg = L"No system tray icons yet";
                dc->DrawText(msg, static_cast<UINT32>(wcslen(msg)), fmt.get(),
                             D2D1::RectF(8 * s, 0, g_trayPop.widthPx - 8 * s,
                                         g_trayPop.heightPx),
                             br.get());
            }
            return;
        }

        for (size_t i = 0; i < g_trayPop.visibleIdx.size(); ++i) {
            const auto& r = g_trayPop.cellRects[i];
            if (static_cast<int>(i) == g_trayPop.hoveredCell) {
                br->SetColor(dark ? D2D1::ColorF(1, 1, 1, 0.16f)
                                  : D2D1::ColorF(0, 0, 0, 0.09f));
                dc->FillRoundedRectangle(D2D1::RoundedRect(r, 8 * s, 8 * s),
                                         br.get());
            }
            size_t idx = g_trayPop.visibleIdx[i];
            if (idx < g_bar.trayIcons.size() && g_bar.trayIcons[idx].hIcon) {
                if (auto wic = LoadWicBitmapFromHIcon(
                        g_bar.trayIcons[idx].hIcon)) {
                    winrt::com_ptr<ID2D1Bitmap> bmp;
                    if (SUCCEEDED(dc->CreateBitmapFromWicBitmap(
                            wic.get(), nullptr, bmp.put())) &&
                        bmp) {
                        float ico = 18.0f * s;
                        float cx = (r.left + r.right) * 0.5f;
                        float cy = (r.top + r.bottom) * 0.5f;
                        dc->DrawBitmap(bmp.get(),
                                       D2D1::RectF(cx - ico * 0.5f,
                                                   cy - ico * 0.5f,
                                                   cx + ico * 0.5f,
                                                   cy + ico * 0.5f),
                                       1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                }
            }
        }
    });
}

void CloseTrayPopup() {
    if (!g_trayPop.open && !g_trayPop.hwnd) {
        return;
    }
    g_trayPop.open = false;
    g_trayPop.liquidPlate.Shutdown();
    g_trayPop.uiVisual = nullptr;
    g_trayPop.uiBrush = nullptr;
    g_trayPop.uiSurface = nullptr;
    g_trayPop.fallbackTint = nullptr;
    g_trayPop.fallbackBackdrop = nullptr;
    g_trayPop.clipGeom = nullptr;
    g_trayPop.glassContainer = nullptr;
    g_trayPop.root = nullptr;
    g_trayPop.target = nullptr;
    if (g_trayPop.hwnd) {
        ShowWindow(g_trayPop.hwnd, SW_HIDE);
        DestroyWindow(g_trayPop.hwnd);
        g_trayPop.hwnd = nullptr;
    }
    if (g_bar.openSlotId == static_cast<int>(BarItemId::Tray)) {
        g_bar.openSlotId = -1;
        RenderFinderBar();
    }
}

LRESULT CALLBACK TrayPopupWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    try {
        switch (msg) {
            case WM_ACTIVATE:
                if (LOWORD(wp) == WA_INACTIVE) {
                    CloseTrayPopup();
                }
                return 0;
            case WM_KEYDOWN:
                if (wp == VK_ESCAPE) {
                    CloseTrayPopup();
                }
                return 0;
            case WM_MOUSEMOVE: {
                float lx = static_cast<float>(static_cast<short>(LOWORD(lp)));
                float ly = static_cast<float>(static_cast<short>(HIWORD(lp)));
                int hov = -1;
                for (size_t i = 0; i < g_trayPop.cellRects.size(); ++i) {
                    if (PointInRectF(g_trayPop.cellRects[i], lx, ly)) {
                        hov = static_cast<int>(i);
                        break;
                    }
                }
                if (hov != g_trayPop.hoveredCell) {
                    g_trayPop.hoveredCell = hov;
                    RenderTrayPopup();
                }
                return 0;
            }
            case WM_LBUTTONUP:
            case WM_RBUTTONUP:
            case WM_LBUTTONDBLCLK: {
                float lx = static_cast<float>(static_cast<short>(LOWORD(lp)));
                float ly = static_cast<float>(static_cast<short>(HIWORD(lp)));
                POINT sp{static_cast<LONG>(lx), static_cast<LONG>(ly)};
                ClientToScreen(hwnd, &sp);
                for (size_t i = 0; i < g_trayPop.cellRects.size(); ++i) {
                    if (PointInRectF(g_trayPop.cellRects[i], lx, ly)) {
                        size_t idx = g_trayPop.visibleIdx[i];
                        if (idx < g_bar.trayIcons.size()) {
                            auto ti = g_bar.trayIcons[idx];
                            CloseTrayPopup();
                            DispatchTrayIconClick(ti, msg, sp);
                        }
                        return 0;
                    }
                }
                return 0;
            }
        }
    } catch (...) {
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void OpenTrayPopup(int centerScreenX) {
    CloseAllMenus();
    CloseAllFinderBarPopups();
    PruneDeadTrayIcons();

    g_trayPop.visibleIdx.clear();
    g_trayPop.cellRects.clear();
    for (size_t i = 0; i < g_bar.trayIcons.size(); ++i) {
        if (!g_bar.trayIcons[i].hidden && g_bar.trayIcons[i].hIcon) {
            g_trayPop.visibleIdx.push_back(i);
        }
    }

    const float s = g_dock.scale;
    const float pad = 10.0f * s;
    const float cell = 34.0f * s;
    const float gap = 4.0f * s;
    int count = static_cast<int>(g_trayPop.visibleIdx.size());
    int cols = count > 0 ? std::min(6, std::max(1, count)) : 1;
    int rows = count > 0 ? ((count + cols - 1) / cols) : 1;
    float w = count > 0 ? std::ceil(pad * 2 + cols * cell + (cols - 1) * gap)
                        : std::ceil(190.0f * s);
    float h = count > 0 ? std::ceil(pad * 2 + rows * cell + (rows - 1) * gap)
                        : std::ceil(44.0f * s);

    for (int i = 0; i < count; ++i) {
        int col = i % cols;
        int row = i / cols;
        float x = pad + col * (cell + gap);
        float y = pad + row * (cell + gap);
        g_trayPop.cellRects.push_back({x, y, x + cell, y + cell});
    }

    const RECT& mr = g_dock.monitorRect;
    int px = std::clamp(centerScreenX - static_cast<int>(w * 0.5f),
                        static_cast<int>(mr.left + 8),
                        static_cast<int>(mr.right - w - 8));
    int py = static_cast<int>(mr.top + g_bar.heightPx + 6.0f * s);

    HMODULE hSelf = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&OpenTrayPopup), &hSelf);
    static bool s_reg = false;
    if (!s_reg) {
        WNDCLASSEXW wc{.cbSize = sizeof(wc),
                       .style = CS_DBLCLKS,
                       .lpfnWndProc = TrayPopupWndProc,
                       .hInstance = hSelf,
                       .hCursor = LoadCursorW(nullptr, IDC_ARROW),
                       .lpszClassName = kTrayPopupClassName};
        RegisterClassExW(&wc);
        s_reg = true;
    }

    g_trayPop.hwnd = CreateWindowExW(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_LAYERED | WS_EX_TOPMOST |
            WS_EX_TOOLWINDOW,
        kTrayPopupClassName, L"", WS_POPUP, px, py, static_cast<int>(w),
        static_cast<int>(h), nullptr, nullptr, hSelf, nullptr);
    if (!g_trayPop.hwnd) {
        return;
    }
    g_trayPop.widthPx = w;
    g_trayPop.heightPx = h;
    g_trayPop.hoveredCell = -1;
    g_trayPop.open = true;

    InitGlassPopupWindowShell(g_trayPop.hwnd, w, h, 14.0f * s, px, py,
                              g_dock.darkMode ? 0.22f : 0.18f, g_trayPop.target,
                              g_trayPop.root, g_trayPop.glassContainer,
                              g_trayPop.clipGeom, g_trayPop.fallbackBackdrop,
                              g_trayPop.fallbackTint, g_trayPop.liquidPlate,
                              g_trayPop.uiSurface, g_trayPop.uiBrush,
                              g_trayPop.uiVisual);
    RenderTrayPopup();
    ShowWindow(g_trayPop.hwnd, SW_SHOW);
    SetForegroundWindow(g_trayPop.hwnd);
    g_bar.openSlotId = static_cast<int>(BarItemId::Tray);
    RenderFinderBar();
}

// ---- Monthly Liquid Glass Calendar Popup ----

int DaysInMonth(int year, int month) {
    static const int kDays[12] = {31, 28, 31, 30, 31, 30,
                                  31, 31, 30, 31, 30, 31};
    if (month == 2 &&
        ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))) {
        return 29;
    }
    return kDays[std::clamp(month - 1, 0, 11)];
}

int FirstWeekdayOfMonth(int year, int month) {
    // Sakamoto's algorithm (0 = Sun .. 6 = Sat)
    static const int t[12] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    int y = year - (month < 3 ? 1 : 0);
    return (y + y / 4 - y / 100 + y / 400 + t[month - 1] + 1) % 7;
}

void RenderCalendarPopup() {
    if (!g_cal.open || !g_cal.uiSurface || !g_gfx.dwriteFactory) {
        return;
    }
    const float s = g_dock.scale;
    const float W = g_cal.widthPx;
    const float H = g_cal.heightPx;
    const bool dark = g_dock.darkMode;
    auto acc = GetAccentColor();
    D2D1_COLOR_F accCol =
        D2D1::ColorF(acc.R / 255.0f, acc.G / 255.0f, acc.B / 255.0f, 0.92f);
    D2D1_COLOR_F fg = dark ? D2D1::ColorF(1, 1, 1, 0.94f)
                           : D2D1::ColorF(0.08f, 0.08f, 0.11f, 0.94f);
    D2D1_COLOR_F fgSub = dark ? D2D1::ColorF(1, 1, 1, 0.52f)
                              : D2D1::ColorF(0.08f, 0.08f, 0.11f, 0.52f);

    SYSTEMTIME today{};
    GetLocalTime(&today);

    DrawCompositionSurface(g_cal.uiSurface, [&](ID2D1DeviceContext* dc) {
        winrt::com_ptr<ID2D1SolidColorBrush> br;
        dc->CreateSolidColorBrush(fg, br.put());
        auto makeFmt = [&](float sz, DWRITE_FONT_WEIGHT wt,
                           DWRITE_TEXT_ALIGNMENT al) {
            winrt::com_ptr<IDWriteTextFormat> f;
            g_gfx.dwriteFactory->CreateTextFormat(
                ResolveFontFamily().c_str(), nullptr, wt,
                DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, sz,
                L"en-us", f.put());
            if (f) {
                f->SetTextAlignment(al);
                f->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            }
            return f;
        };
        auto fHead = makeFmt(14.0f * s, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                             DWRITE_TEXT_ALIGNMENT_LEADING);
        auto fDay = makeFmt(12.0f * s, DWRITE_FONT_WEIGHT_NORMAL,
                            DWRITE_TEXT_ALIGNMENT_CENTER);
        auto fDow = makeFmt(10.5f * s, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                            DWRITE_TEXT_ALIGNMENT_CENTER);

        static const PCWSTR kMonths[12] = {
            L"January", L"February", L"March",     L"April",
            L"May",     L"June",     L"July",      L"August",
            L"September", L"October", L"November", L"December"};
        std::wstring title =
            std::wstring(kMonths[std::clamp(g_cal.viewMonth - 1, 0, 11)]) +
            L" " + std::to_wstring(g_cal.viewYear);

        const float pad = 14.0f * s;
        br->SetColor(fg);
        dc->DrawText(title.c_str(), static_cast<UINT32>(title.size()),
                     fHead.get(), D2D1::RectF(pad, pad, W - 70 * s, pad + 26 * s),
                     br.get());

        g_cal.rcPrev = {W - 62 * s, pad, W - 38 * s, pad + 24 * s};
        g_cal.rcNext = {W - 34 * s, pad, W - 10 * s, pad + 24 * s};
        if (g_cal.hoverBtn == 1 || g_cal.hoverBtn == 2) {
            br->SetColor(dark ? D2D1::ColorF(1, 1, 1, 0.14f)
                              : D2D1::ColorF(0, 0, 0, 0.08f));
            dc->FillRoundedRectangle(
                D2D1::RoundedRect(
                    g_cal.hoverBtn == 1 ? g_cal.rcPrev : g_cal.rcNext, 6 * s,
                    6 * s),
                br.get());
        }
        br->SetColor(fg);
        dc->DrawText(L"\u2039", 1, fDay.get(), g_cal.rcPrev, br.get());
        dc->DrawText(L"\u203A", 1, fDay.get(), g_cal.rcNext, br.get());

        static const PCWSTR kDow[7] = {L"Su", L"Mo", L"Tu", L"We",
                                       L"Th", L"Fr", L"Sa"};
        float colW = (W - 2.0f * pad) / 7.0f;
        float y = pad + 34.0f * s;
        br->SetColor(fgSub);
        for (int c = 0; c < 7; ++c) {
            dc->DrawText(kDow[c], 2, fDow.get(),
                         D2D1::RectF(pad + c * colW, y, pad + (c + 1) * colW,
                                     y + 18 * s),
                         br.get());
        }
        y += 22.0f * s;

        int firstDow = FirstWeekdayOfMonth(g_cal.viewYear, g_cal.viewMonth);
        int days = DaysInMonth(g_cal.viewYear, g_cal.viewMonth);
        float rowH = 29.0f * s;
        for (int d = 1; d <= days; ++d) {
            int slot = firstDow + (d - 1);
            int col = slot % 7;
            int row = slot / 7;
            D2D1_RECT_F rc{pad + col * colW, y + row * rowH,
                           pad + (col + 1) * colW, y + (row + 1) * rowH};
            bool isToday = (g_cal.viewYear == today.wYear &&
                            g_cal.viewMonth == today.wMonth &&
                            d == today.wDay);
            if (isToday) {
                float cx = (rc.left + rc.right) * 0.5f;
                float cy = (rc.top + rc.bottom) * 0.5f;
                float r = 12.0f * s;
                br->SetColor(accCol);
                dc->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r),
                                br.get());
                br->SetColor(D2D1::ColorF(1, 1, 1, 0.98f));
            } else {
                br->SetColor(fg);
            }
            std::wstring ds = std::to_wstring(d);
            dc->DrawText(ds.c_str(), static_cast<UINT32>(ds.size()), fDay.get(),
                         rc, br.get());
        }

        // Footer link
        g_cal.rcFooter = {pad, H - 36 * s, W - pad, H - 10 * s};
        if (g_cal.hoverBtn == 3) {
            br->SetColor(dark ? D2D1::ColorF(1, 1, 1, 0.12f)
                              : D2D1::ColorF(0, 0, 0, 0.07f));
            dc->FillRoundedRectangle(
                D2D1::RoundedRect(g_cal.rcFooter, 8 * s, 8 * s), br.get());
        }
        br->SetColor(fgSub);
        PCWSTR ft = L"Date & Time Settings\u2026";
        dc->DrawText(ft, static_cast<UINT32>(wcslen(ft)), fDay.get(),
                     g_cal.rcFooter, br.get());
    });
}

void CloseCalendarPopup() {
    if (!g_cal.open && !g_cal.hwnd) {
        return;
    }
    g_cal.open = false;
    g_cal.liquidPlate.Shutdown();
    g_cal.uiVisual = nullptr;
    g_cal.uiBrush = nullptr;
    g_cal.uiSurface = nullptr;
    g_cal.fallbackTint = nullptr;
    g_cal.fallbackBackdrop = nullptr;
    g_cal.clipGeom = nullptr;
    g_cal.glassContainer = nullptr;
    g_cal.root = nullptr;
    g_cal.target = nullptr;
    if (g_cal.hwnd) {
        ShowWindow(g_cal.hwnd, SW_HIDE);
        DestroyWindow(g_cal.hwnd);
        g_cal.hwnd = nullptr;
    }
    if (g_bar.openSlotId == static_cast<int>(BarItemId::Clock)) {
        g_bar.openSlotId = -1;
        RenderFinderBar();
    }
}

LRESULT CALLBACK CalendarPopupWndProc(HWND hwnd, UINT msg, WPARAM wp,
                                      LPARAM lp) {
    try {
        switch (msg) {
            case WM_ACTIVATE:
                if (LOWORD(wp) == WA_INACTIVE) {
                    CloseCalendarPopup();
                }
                return 0;
            case WM_KEYDOWN:
                if (wp == VK_ESCAPE) {
                    CloseCalendarPopup();
                }
                return 0;
            case WM_MOUSEMOVE: {
                float lx = static_cast<float>(static_cast<short>(LOWORD(lp)));
                float ly = static_cast<float>(static_cast<short>(HIWORD(lp)));
                int hb = -1;
                if (PointInRectF(g_cal.rcPrev, lx, ly)) hb = 1;
                else if (PointInRectF(g_cal.rcNext, lx, ly)) hb = 2;
                else if (PointInRectF(g_cal.rcFooter, lx, ly)) hb = 3;
                if (hb != g_cal.hoverBtn) {
                    g_cal.hoverBtn = hb;
                    RenderCalendarPopup();
                }
                return 0;
            }
            case WM_LBUTTONUP: {
                float lx = static_cast<float>(static_cast<short>(LOWORD(lp)));
                float ly = static_cast<float>(static_cast<short>(HIWORD(lp)));
                if (PointInRectF(g_cal.rcPrev, lx, ly)) {
                    if (--g_cal.viewMonth < 1) {
                        g_cal.viewMonth = 12;
                        --g_cal.viewYear;
                    }
                    RenderCalendarPopup();
                } else if (PointInRectF(g_cal.rcNext, lx, ly)) {
                    if (++g_cal.viewMonth > 12) {
                        g_cal.viewMonth = 1;
                        ++g_cal.viewYear;
                    }
                    RenderCalendarPopup();
                } else if (PointInRectF(g_cal.rcFooter, lx, ly)) {
                    CloseCalendarPopup();
                    OpenShellUri(L"ms-settings:dateandtime");
                }
                return 0;
            }
        }
    } catch (...) {
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void OpenCalendarPopup(int rightEdgeScreenX) {
    CloseAllMenus();
    CloseAllFinderBarPopups();
    SYSTEMTIME st{};
    GetLocalTime(&st);
    g_cal.viewYear = st.wYear;
    g_cal.viewMonth = st.wMonth;

    const float s = g_dock.scale;
    float w = std::ceil(268.0f * s);
    float h = std::ceil(292.0f * s);
    const RECT& mr = g_dock.monitorRect;
    int px = std::clamp(rightEdgeScreenX - static_cast<int>(w),
                        static_cast<int>(mr.left + 8),
                        static_cast<int>(mr.right - w - 8));
    int py = static_cast<int>(mr.top + g_bar.heightPx + 6.0f * s);

    HMODULE hSelf = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&OpenCalendarPopup), &hSelf);
    static bool s_reg = false;
    if (!s_reg) {
        WNDCLASSEXW wc{.cbSize = sizeof(wc),
                       .lpfnWndProc = CalendarPopupWndProc,
                       .hInstance = hSelf,
                       .hCursor = LoadCursorW(nullptr, IDC_ARROW),
                       .lpszClassName = kCalendarPopupClassName};
        RegisterClassExW(&wc);
        s_reg = true;
    }

    g_cal.hwnd = CreateWindowExW(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_LAYERED | WS_EX_TOPMOST |
            WS_EX_TOOLWINDOW,
        kCalendarPopupClassName, L"", WS_POPUP, px, py, static_cast<int>(w),
        static_cast<int>(h), nullptr, nullptr, hSelf, nullptr);
    if (!g_cal.hwnd) {
        return;
    }
    g_cal.widthPx = w;
    g_cal.heightPx = h;
    g_cal.hoverBtn = -1;
    g_cal.open = true;

    InitGlassPopupWindowShell(g_cal.hwnd, w, h, 16.0f * s, px, py,
                              g_dock.darkMode ? 0.22f : 0.18f, g_cal.target,
                              g_cal.root, g_cal.glassContainer, g_cal.clipGeom,
                              g_cal.fallbackBackdrop, g_cal.fallbackTint,
                              g_cal.liquidPlate, g_cal.uiSurface, g_cal.uiBrush,
                              g_cal.uiVisual);
    RenderCalendarPopup();
    ShowWindow(g_cal.hwnd, SW_SHOW);
    SetForegroundWindow(g_cal.hwnd);
    g_bar.openSlotId = static_cast<int>(BarItemId::Clock);
    RenderFinderBar();
}

// ---- Top Finder Bar Layout & Crisp Vector Rendering ----

float MeasureBarTextWidth(IDWriteTextFormat* fmt, const std::wstring& txt) {
    if (!fmt || txt.empty() || !g_gfx.dwriteFactory) {
        return 0.0f;
    }
    winrt::com_ptr<IDWriteTextLayout> layout;
    if (FAILED(g_gfx.dwriteFactory->CreateTextLayout(
            txt.c_str(), static_cast<UINT32>(txt.size()), fmt, 4096.0f, 200.0f,
            layout.put())) ||
        !layout) {
        return 0.0f;
    }
    DWRITE_TEXT_METRICS tm{};
    if (SUCCEEDED(layout->GetMetrics(&tm))) {
        return tm.widthIncludingTrailingWhitespace;
    }
    return 0.0f;
}

void DrawFinderBarVectorIcon(ID2D1DeviceContext* dc,
                             ID2D1SolidColorBrush* br,
                             BarItemId id,
                             const D2D1_RECT_F& r,
                             float s,
                             D2D1_COLOR_F fg,
                             bool dark) {
    float cx = (r.left + r.right) * 0.5f;
    float cy = (r.top + r.bottom) * 0.5f;
    br->SetColor(fg);

    switch (id) {
        case BarItemId::Logo: {
            // Crisp geometric Apple-inspired dual-facet emblem (pure vector, zero copyright assets)
            float rad = 5.4f * s;
            // Subtle diamond-circle emblem with top leaf accent
            dc->FillEllipse(
                D2D1::Ellipse(D2D1::Point2F(cx - 1.6f * s, cy + 1.2f * s),
                              rad * 0.72f, rad * 0.85f),
                br);
            dc->FillEllipse(
                D2D1::Ellipse(D2D1::Point2F(cx + 1.6f * s, cy + 1.2f * s),
                              rad * 0.72f, rad * 0.85f),
                br);
            // Leaf accent
            winrt::com_ptr<ID2D1PathGeometry> leaf;
            winrt::com_ptr<ID2D1GeometrySink> sink;
            if (SUCCEEDED(g_gfx.d2dFactory->CreatePathGeometry(leaf.put())) &&
                SUCCEEDED(leaf->Open(sink.put()))) {
                sink->BeginFigure(D2D1::Point2F(cx, cy - 3.0f * s),
                                  D2D1_FIGURE_BEGIN_FILLED);
                sink->AddQuadraticBezier(D2D1::QuadraticBezierSegment(
                    D2D1::Point2F(cx + 0.5f * s, cy - 6.8f * s),
                    D2D1::Point2F(cx + 3.4f * s, cy - 6.8f * s)));
                sink->AddQuadraticBezier(D2D1::QuadraticBezierSegment(
                    D2D1::Point2F(cx + 3.0f * s, cy - 3.3f * s),
                    D2D1::Point2F(cx, cy - 3.0f * s)));
                sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                sink->Close();
                dc->FillGeometry(leaf.get(), br);
            }
            break;
        }
        case BarItemId::Tray: {
            // Crisp chevron '^' / '<'
            float sz = 3.5f * s;
            dc->DrawLine(D2D1::Point2F(cx - sz, cy + sz * 0.4f),
                         D2D1::Point2F(cx, cy - sz * 0.6f), br, 1.5f * s);
            dc->DrawLine(D2D1::Point2F(cx, cy - sz * 0.6f),
                         D2D1::Point2F(cx + sz, cy + sz * 0.4f), br, 1.5f * s);
            break;
        }
        case BarItemId::Battery: {
            float bw = 20.0f * s;
            float bh = 10.5f * s;
            float bx = r.left + 6.0f * s;
            float by = cy - bh * 0.5f;
            D2D1_ROUNDED_RECT body =
                D2D1::RoundedRect(D2D1::RectF(bx, by, bx + bw, by + bh),
                                  2.8f * s, 2.8f * s);
            br->SetColor(D2D1::ColorF(fg.r, fg.g, fg.b, 0.55f));
            dc->DrawRoundedRectangle(body, br, 1.15f * s);
            // Terminal nub
            dc->FillRoundedRectangle(
                D2D1::RoundedRect(
                    D2D1::RectF(bx + bw + 1.0f * s, cy - 2.0f * s,
                                bx + bw + 2.6f * s, cy + 2.0f * s),
                    0.8f * s, 0.8f * s),
                br);
            // Fill bar
            float frac =
                std::clamp(g_bar.status.batteryPct / 100.0f, 0.05f, 1.0f);
            D2D1_COLOR_F fillCol = fg;
            if (g_bar.status.batteryCharging) {
                fillCol = D2D1::ColorF(0.20f, 0.84f, 0.42f, 0.96f);
            } else if (g_bar.status.batteryPct <= 20) {
                fillCol = D2D1::ColorF(1.0f, 0.28f, 0.25f, 0.96f);
            }
            br->SetColor(fillCol);
            float padIn = 2.0f * s;
            dc->FillRoundedRectangle(
                D2D1::RoundedRect(
                    D2D1::RectF(bx + padIn, by + padIn,
                                bx + padIn + (bw - 2.0f * padIn) * frac,
                                by + bh - padIn),
                    1.3f * s, 1.3f * s),
                br);
            break;
        }
        case BarItemId::Network: {
            if (g_bar.status.netKind == 2) {
                // Ethernet icon
                dc->DrawRoundedRectangle(
                    D2D1::RoundedRect(
                        D2D1::RectF(cx - 5.5f * s, cy - 4.5f * s, cx + 5.5f * s,
                                    cy + 4.5f * s),
                        1.5f * s, 1.5f * s),
                    br, 1.35f * s);
                dc->FillRectangle(
                    D2D1::RectF(cx - 2.2f * s, cy - 1.5f * s, cx + 2.2f * s,
                                cy + 1.5f * s),
                    br);
            } else {
                // Wi-Fi 3 concentric arcs + dot
                int bars =
                    (g_bar.status.netKind == 0)
                        ? 0
                        : (g_bar.status.wifiQuality >= 70
                               ? 3
                               : (g_bar.status.wifiQuality >= 40 ? 2 : 1));
                float oy = cy + 4.2f * s;
                br->SetColor(bars > 0 ? fg
                                      : D2D1::ColorF(fg.r, fg.g, fg.b, 0.35f));
                dc->FillEllipse(
                    D2D1::Ellipse(D2D1::Point2F(cx, oy), 1.4f * s, 1.4f * s),
                    br);
                for (int b = 1; b <= 3; ++b) {
                    float rad = (2.8f + b * 2.5f) * s;
                    br->SetColor(b <= bars
                                     ? fg
                                     : D2D1::ColorF(fg.r, fg.g, fg.b, 0.28f));
                    winrt::com_ptr<ID2D1PathGeometry> arc;
                    winrt::com_ptr<ID2D1GeometrySink> sink;
                    if (SUCCEEDED(g_gfx.d2dFactory->CreatePathGeometry(
                            arc.put())) &&
                        SUCCEEDED(arc->Open(sink.put()))) {
                        float dx = rad * 0.72f;
                        float dy = rad * 0.69f;
                        sink->BeginFigure(D2D1::Point2F(cx - dx, oy - dy),
                                          D2D1_FIGURE_BEGIN_HOLLOW);
                        sink->AddArc(D2D1::ArcSegment(
                            D2D1::Point2F(cx + dx, oy - dy),
                            D2D1::SizeF(rad, rad), 0.0f,
                            D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
                        sink->EndFigure(D2D1_FIGURE_END_OPEN);
                        sink->Close();
                        dc->DrawGeometry(arc.get(), br, 1.35f * s);
                    }
                }
            }
            break;
        }
        case BarItemId::Volume: {
            // Crisp Apple speaker polygon + wave arcs or mute slash
            winrt::com_ptr<ID2D1PathGeometry> spk;
            winrt::com_ptr<ID2D1GeometrySink> sink;
            if (SUCCEEDED(g_gfx.d2dFactory->CreatePathGeometry(spk.put())) &&
                SUCCEEDED(spk->Open(sink.put()))) {
                float sx = cx - 5.5f * s;
                sink->BeginFigure(D2D1::Point2F(sx, cy - 2.0f * s),
                                  D2D1_FIGURE_BEGIN_FILLED);
                sink->AddLine(D2D1::Point2F(sx + 2.8f * s, cy - 2.0f * s));
                sink->AddLine(D2D1::Point2F(sx + 6.0f * s, cy - 4.8f * s));
                sink->AddLine(D2D1::Point2F(sx + 6.0f * s, cy + 4.8f * s));
                sink->AddLine(D2D1::Point2F(sx + 2.8f * s, cy + 2.0f * s));
                sink->AddLine(D2D1::Point2F(sx, cy + 2.0f * s));
                sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                sink->Close();
                dc->FillGeometry(spk.get(), br);
            }
            if (g_bar.status.muted || g_bar.status.volumePct == 0) {
                br->SetColor(D2D1::ColorF(fg.r, fg.g, fg.b, 0.72f));
                dc->DrawLine(D2D1::Point2F(cx + 2.0f * s, cy - 2.6f * s),
                             D2D1::Point2F(cx + 6.6f * s, cy + 2.6f * s), br,
                             1.35f * s);
                dc->DrawLine(D2D1::Point2F(cx + 6.6f * s, cy - 2.6f * s),
                             D2D1::Point2F(cx + 2.0f * s, cy + 2.6f * s), br,
                             1.35f * s);
            } else {
                int wv = g_bar.status.volumePct >= 66
                             ? 2
                             : (g_bar.status.volumePct >= 25 ? 1 : 0);
                for (int i = 0; i <= wv; ++i) {
                    float rx = (2.8f + i * 2.2f) * s;
                    float ry = (2.6f + i * 1.8f) * s;
                    winrt::com_ptr<ID2D1PathGeometry> arc;
                    winrt::com_ptr<ID2D1GeometrySink> asink;
                    if (SUCCEEDED(g_gfx.d2dFactory->CreatePathGeometry(
                            arc.put())) &&
                        SUCCEEDED(arc->Open(asink.put()))) {
                        asink->BeginFigure(
                            D2D1::Point2F(cx + 1.2f * s + rx * 0.55f, cy - ry),
                            D2D1_FIGURE_BEGIN_HOLLOW);
                        asink->AddArc(D2D1::ArcSegment(
                            D2D1::Point2F(cx + 1.2f * s + rx * 0.55f, cy + ry),
                            D2D1::SizeF(rx, ry), 0.0f,
                            D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
                        asink->EndFigure(D2D1_FIGURE_END_OPEN);
                        asink->Close();
                        dc->DrawGeometry(arc.get(), br, 1.25f * s);
                    }
                }
            }
            break;
        }
        case BarItemId::Search: {
            float rad = 4.2f * s;
            dc->DrawEllipse(
                D2D1::Ellipse(D2D1::Point2F(cx - 1.2f * s, cy - 1.2f * s), rad,
                              rad),
                br, 1.45f * s);
            dc->DrawLine(D2D1::Point2F(cx + 2.0f * s, cy + 2.0f * s),
                         D2D1::Point2F(cx + 5.2f * s, cy + 5.2f * s), br,
                         1.55f * s);
            break;
        }
        case BarItemId::ControlCenter: {
            // Iconic macOS dual toggle pills
            float pw = 13.0f * s;
            float ph = 5.4f * s;
            float pr = ph * 0.5f;
            float x0 = cx - pw * 0.5f;
            // Top pill (outlined, knob on left)
            D2D1_RECT_F topP{x0, cy - ph - 1.0f * s, x0 + pw, cy - 1.0f * s};
            dc->DrawRoundedRectangle(D2D1::RoundedRect(topP, pr, pr), br,
                                     1.15f * s);
            dc->FillEllipse(
                D2D1::Ellipse(D2D1::Point2F(topP.left + pr,
                                            (topP.top + topP.bottom) * 0.5f),
                              pr * 0.72f, pr * 0.72f),
                br);
            // Bottom pill (filled, contrasting knob on right)
            D2D1_RECT_F botP{x0, cy + 1.0f * s, x0 + pw, cy + ph + 1.0f * s};
            dc->FillRoundedRectangle(D2D1::RoundedRect(botP, pr, pr), br);
            br->SetColor(dark ? D2D1::ColorF(0.08f, 0.08f, 0.12f, 0.90f)
                              : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.92f));
            dc->FillEllipse(
                D2D1::Ellipse(D2D1::Point2F(botP.right - pr,
                                            (botP.top + botP.bottom) * 0.5f),
                              pr * 0.62f, pr * 0.62f),
                br);
            break;
        }
        default:
            break;
    }
}

void RenderFinderBar() {
    if (!g_bar.hwnd || !g_settings.enableFinderBar || !g_gfx.compositor ||
        !g_gfx.dwriteFactory) {
        return;
    }
    const float s = g_dock.scale;
    const float W = g_bar.widthPx;
    const float H = g_bar.heightPx;
    if (W < 10.0f || H < 10.0f) {
        return;
    }
    const bool dark = g_dock.darkMode ||
                      g_settings.finderBarStyle == FinderBarStyle::Transparent;

    // 1. Update background layer according to finderBarStyle
    bool useOptical =
        (g_settings.finderBarStyle == FinderBarStyle::LiquidGlass) &&
        EnsureLiquidCaptureReady() && !g_bar.hiddenForFullscreen;
    if (useOptical) {
        const RECT& wr = g_bar.windowRect;
        if (wr.right > wr.left && wr.bottom > wr.top) {
            LiquidGlassStyleParams lp{};
            lp.screenX = static_cast<float>(wr.left);
            lp.screenY = static_cast<float>(wr.top);
            lp.widthPx = W;
            lp.heightPx = H;
            lp.cornerRadiusPx = 0.0f;
            lp.scale = s;
            lp.darkMode = g_dock.darkMode;
            lp.useSquircle = false;
            lp.fullWidthBar = true;
            lp.tintBoost = g_dock.darkMode ? 0.04f : 0.02f;
            lp.blurMul = 1.15f;
            lp.rimDepthMul = 0.45f;
            lp.strengthMul = 0.65f;
            g_bar.lastGlassParams = lp;
            g_bar.hasGlassParams = true;
            if (RenderLiquidGlassPlate(g_bar.liquidPlate, lp) &&
                g_bar.liquidPlate.visual) {
                if (!g_bar.liquidAttached) {
                    g_bar.root.Children().InsertAtBottom(
                        g_bar.liquidPlate.visual);
                    g_bar.liquidAttached = true;
                }
                g_bar.liquidPlate.visual.IsVisible(true);
                if (g_bar.fallbackBackdrop)
                    g_bar.fallbackBackdrop.IsVisible(false);
                if (g_bar.fallbackTint) g_bar.fallbackTint.IsVisible(false);
            } else {
                useOptical = false;
            }
        } else {
            useOptical = false;
        }
    }

    if (!useOptical) {
        if (g_bar.liquidPlate.visual) {
            g_bar.liquidPlate.visual.IsVisible(false);
        }
        bool showBlur =
            (g_settings.finderBarStyle == FinderBarStyle::Frosted ||
             g_settings.finderBarStyle == FinderBarStyle::LiquidGlass);
        if (g_bar.fallbackBackdrop) {
            g_bar.fallbackBackdrop.IsVisible(showBlur);
            g_bar.fallbackBackdrop.Size({W, H});
        }
        if (g_bar.fallbackTint) {
            g_bar.fallbackTint.IsVisible(
                g_settings.finderBarStyle != FinderBarStyle::Transparent);
            g_bar.fallbackTint.Size({W, H});
            uint8_t a =
                (g_settings.finderBarStyle == FinderBarStyle::Solid)
                    ? 245
                    : (g_dock.darkMode ? 115 : 135);
            auto col =
                g_dock.darkMode ? winrt::Windows::UI::Color{a, 18, 19, 24}
                                : winrt::Windows::UI::Color{a, 246, 247, 251};
            if (!g_bar.fallbackTintBrush) {
                g_bar.fallbackTintBrush = g_gfx.compositor.CreateColorBrush(col);
                g_bar.fallbackTint.Brush(g_bar.fallbackTintBrush);
            } else {
                g_bar.fallbackTintBrush.Color(col);
            }
        }
    }

    // 2. Ensure foreground DrawingSurface matches bar size
    SIZE wantSz{static_cast<LONG>(std::ceil(W)),
                static_cast<LONG>(std::ceil(H))};
    if (!g_bar.fgSurface || g_bar.fgSurfSize.cx != wantSz.cx ||
        g_bar.fgSurfSize.cy != wantSz.cy) {
        g_bar.fgSurface = CreateDrawingSurface(W, H);
        g_bar.fgSurfSize = wantSz;
        if (g_bar.fgBrush && g_bar.fgSurface) {
            g_bar.fgBrush.Surface(g_bar.fgSurface);
        }
    }
    if (g_bar.fgVisual) {
        g_bar.fgVisual.Size({W, H});
    }
    if (!g_bar.fgSurface) {
        return;
    }

    // 3. Build left & right BarSlots (reusing cached DirectWrite TextFormats)
    g_bar.slots.clear();
    if (!g_bar.fmtReg || std::fabs(g_bar.cachedFmtScale - s) > 0.001f) {
        g_bar.fmtReg = nullptr;
        g_bar.fmtBold = nullptr;
        g_bar.fmtSmall = nullptr;
        g_bar.cachedFmtScale = s;
        g_gfx.dwriteFactory->CreateTextFormat(
            ResolveFontFamily().c_str(), nullptr, DWRITE_FONT_WEIGHT_MEDIUM,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 13.0f * s,
            L"en-us", g_bar.fmtReg.put());
        g_gfx.dwriteFactory->CreateTextFormat(
            ResolveFontFamily().c_str(), nullptr, DWRITE_FONT_WEIGHT_BOLD,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 13.2f * s,
            L"en-us", g_bar.fmtBold.put());
        g_gfx.dwriteFactory->CreateTextFormat(
            ResolveFontFamily().c_str(), nullptr, DWRITE_FONT_WEIGHT_MEDIUM,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 11.5f * s,
            L"en-us", g_bar.fmtSmall.put());
        for (auto* f : {g_bar.fmtReg.get(), g_bar.fmtBold.get(),
                        g_bar.fmtSmall.get()}) {
            if (f) {
                f->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                f->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
            }
        }
    }
    auto& fmtReg = g_bar.fmtReg;
    auto& fmtBold = g_bar.fmtBold;
    auto& fmtSmall = g_bar.fmtSmall;

    const float itemPadX = 9.0f * s;
    float lx = 6.0f * s;

    // Logo
    g_bar.slots.push_back(
        {static_cast<int>(BarItemId::Logo), L"", {lx, 2 * s, lx + 30 * s, H - 2 * s},
         false, false, nullptr});
    lx += 30 * s + 2 * s;

    // Active app name (bold)
    float appW =
        MeasureBarTextWidth(fmtBold.get(), g_bar.activeAppName) + itemPadX * 2;
    g_bar.slots.push_back({static_cast<int>(BarItemId::ActiveApp),
                           g_bar.activeAppName,
                           {lx, 2 * s, lx + appW, H - 2 * s}, true, false,
                           nullptr});
    lx += appW;

    // Global menus: real Win32 HMENU if present, else per-app macOS menus
    bool addedWin32Menus = false;
    if (!g_bar.isFinderActive && g_bar.activeHwnd &&
        IsWindow(g_bar.activeHwnd)) {
        if (HMENU hMenu = GetMenu(g_bar.activeHwnd)) {
            int cnt = std::min(GetMenuItemCount(hMenu), 10);
            for (int i = 0; i < cnt; ++i) {
                MENUITEMINFOW mii{};
                mii.cbSize = sizeof(mii);
                mii.fMask = MIIM_STRING | MIIM_SUBMENU;
                WCHAR buf[64]{};
                mii.dwTypeData = buf;
                mii.cch = ARRAYSIZE(buf);
                if (GetMenuItemInfoW(hMenu, static_cast<UINT>(i), TRUE, &mii) &&
                    buf[0]) {
                    std::wstring lbl = CleanWin32MenuLabel(buf);
                    float mw = MeasureBarTextWidth(fmtReg.get(), lbl) +
                               itemPadX * 2;
                    if (lx + mw < W * 0.56f) {
                        g_bar.slots.push_back(
                            {static_cast<int>(BarItemId::GlobalMenuBase) + i,
                             lbl, {lx, 2 * s, lx + mw, H - 2 * s}, false, false,
                             mii.hSubMenu});
                        lx += mw;
                        addedWin32Menus = true;
                    }
                }
            }
        }
    }

    if (!addedWin32Menus) {
        std::vector<PCWSTR> names;
        if (g_bar.isFinderActive) {
            names = {L"File", L"Edit", L"View", L"Go", L"Window", L"Help"};
        } else if (IsBrowserExe(g_bar.activeExeName)) {
            names = {L"File", L"Edit", L"View", L"History", L"Bookmarks",
                     L"Window", L"Help"};
        } else if (IsCodeEditorExe(g_bar.activeExeName)) {
            names = {L"File", L"Edit", L"View", L"Terminal", L"Window", L"Help"};
        } else {
            names = {L"File", L"Edit", L"View", L"Window", L"Help"};
        }
        for (size_t i = 0; i < names.size(); ++i) {
            std::wstring lbl = names[i];
            float mw = MeasureBarTextWidth(fmtReg.get(), lbl) + itemPadX * 2;
            if (lx + mw < W * 0.58f) {
                g_bar.slots.push_back(
                    {static_cast<int>(BarItemId::GlobalMenuBase) +
                         static_cast<int>(i),
                     lbl, {lx, 2 * s, lx + mw, H - 2 * s}, false, false,
                     nullptr});
                lx += mw;
            }
        }
    }

    // Right side items (laid out right-to-left)
    float rx = W - 6.0f * s;
    auto pushRight = [&](BarItemId id, const std::wstring& lbl, float slotW) {
        rx -= slotW;
        g_bar.slots.push_back({static_cast<int>(id), lbl,
                               {rx, 2 * s, rx + slotW, H - 2 * s}, false, true,
                               nullptr});
        rx -= 1.0f * s;
    };

    float clockW =
        MeasureBarTextWidth(fmtReg.get(), g_bar.status.clockText) + 16.0f * s;
    pushRight(BarItemId::Clock, g_bar.status.clockText, clockW);
    pushRight(BarItemId::ControlCenter, L"", 28.0f * s);
    pushRight(BarItemId::Search, L"", 26.0f * s);
    pushRight(BarItemId::Volume, L"", 26.0f * s);
    pushRight(BarItemId::Network, L"", 26.0f * s);

    if (g_bar.status.hasBattery) {
        std::wstring pctLbl =
            g_settings.finderBarShowBatteryPct
                ? (std::to_wstring(g_bar.status.batteryPct) + L"%")
                : L"";
        float pctW =
            pctLbl.empty() ? 0.0f : (MeasureBarTextWidth(fmtSmall.get(), pctLbl) + 4 * s);
        pushRight(BarItemId::Battery, pctLbl, 32.0f * s + pctW);
    }

    if (g_settings.finderBarShowTray) {
        pushRight(BarItemId::Tray, L"", 24.0f * s);
    }

    // 4. Paint foreground surface
    D2D1_COLOR_F fg = dark ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.94f)
                           : D2D1::ColorF(0.06f, 0.06f, 0.09f, 0.94f);
    D2D1_COLOR_F pillBg = dark ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.16f)
                               : D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.10f);
    D2D1_COLOR_F shadowCol = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.42f);
    const bool drawShadow =
        (g_settings.finderBarStyle == FinderBarStyle::Transparent);

    DrawCompositionSurface(g_bar.fgSurface, [&](ID2D1DeviceContext* dc) {
        dc->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
        winrt::com_ptr<ID2D1SolidColorBrush> br;
        dc->CreateSolidColorBrush(fg, br.put());

        // Subtle top protective gradient on transparent mode for readability
        if (g_settings.finderBarStyle == FinderBarStyle::Transparent) {
            D2D1_GRADIENT_STOP st[2] = {
                {0.0f, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.24f)},
                {1.0f, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f)}};
            winrt::com_ptr<ID2D1GradientStopCollection> coll;
            winrt::com_ptr<ID2D1LinearGradientBrush> lg;
            if (SUCCEEDED(dc->CreateGradientStopCollection(st, 2, coll.put())) &&
                SUCCEEDED(dc->CreateLinearGradientBrush(
                    D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, 0),
                                                        D2D1::Point2F(0, H)),
                    coll.get(), lg.put()))) {
                dc->FillRectangle(D2D1::RectF(0, 0, W, H), lg.get());
            }
        }

        for (const auto& sl : g_bar.slots) {
            bool active =
                (sl.id == g_bar.openSlotId || sl.id == g_bar.hoveredSlotId);
            if (active) {
                br->SetColor(pillBg);
                dc->FillRoundedRectangle(
                    D2D1::RoundedRect(sl.rect, 5.5f * s, 5.5f * s), br.get());
            }

            if (sl.id == static_cast<int>(BarItemId::Logo) ||
                sl.id == static_cast<int>(BarItemId::Tray) ||
                sl.id == static_cast<int>(BarItemId::Network) ||
                sl.id == static_cast<int>(BarItemId::Volume) ||
                sl.id == static_cast<int>(BarItemId::Search) ||
                sl.id == static_cast<int>(BarItemId::ControlCenter)) {
                DrawFinderBarVectorIcon(dc, br.get(),
                                        static_cast<BarItemId>(sl.id), sl.rect,
                                        s, fg, dark);
            } else if (sl.id == static_cast<int>(BarItemId::Battery)) {
                DrawFinderBarVectorIcon(dc, br.get(), BarItemId::Battery,
                                        sl.rect, s, fg, dark);
                if (!sl.label.empty()) {
                    D2D1_RECT_F tr{sl.rect.left + 30.0f * s, sl.rect.top,
                                   sl.rect.right - 2.0f * s, sl.rect.bottom};
                    if (drawShadow) {
                        br->SetColor(shadowCol);
                        D2D1_RECT_F sh{tr.left, tr.top + s, tr.right,
                                       tr.bottom + s};
                        dc->DrawText(sl.label.c_str(),
                                     static_cast<UINT32>(sl.label.size()),
                                     fmtSmall.get(), sh, br.get());
                    }
                    br->SetColor(fg);
                    dc->DrawText(sl.label.c_str(),
                                 static_cast<UINT32>(sl.label.size()),
                                 fmtSmall.get(), tr, br.get());
                }
            } else if (!sl.label.empty()) {
                IDWriteTextFormat* fmt = sl.bold ? fmtBold.get() : fmtReg.get();
                D2D1_RECT_F tr{sl.rect.left + itemPadX, sl.rect.top,
                               sl.rect.right - itemPadX, sl.rect.bottom};
                if (drawShadow) {
                    br->SetColor(shadowCol);
                    D2D1_RECT_F sh{tr.left, tr.top + s, tr.right, tr.bottom + s};
                    dc->DrawText(sl.label.c_str(),
                                 static_cast<UINT32>(sl.label.size()), fmt, sh,
                                 br.get());
                }
                br->SetColor(fg);
                dc->DrawText(sl.label.c_str(),
                             static_cast<UINT32>(sl.label.size()), fmt, tr,
                             br.get());
            }
        }
    });
}

void OpenMenuForFinderBarSlot(const BarSlot& sl) {
    RECT barRc{};
    if (!g_bar.hwnd || !GetWindowRect(g_bar.hwnd, &barRc)) {
        return;
    }
    int anchorY = barRc.bottom + static_cast<int>(4.0f * g_dock.scale);
    int leftX = barRc.left + static_cast<int>(sl.rect.left);
    int rightX = barRc.left + static_cast<int>(sl.rect.right);
    int centerX = (leftX + rightX) / 2;

    if (sl.id == static_cast<int>(BarItemId::ControlCenter)) {
        if (g_cc.open) {
            CloseControlCenter();
        } else {
            OpenControlCenterPopup(rightX);
        }
        return;
    }
    if (sl.id == static_cast<int>(BarItemId::Tray)) {
        if (g_trayPop.open) {
            CloseTrayPopup();
        } else {
            OpenTrayPopup(centerX);
        }
        return;
    }
    if (sl.id == static_cast<int>(BarItemId::Clock)) {
        if (g_cal.open) {
            CloseCalendarPopup();
        } else {
            OpenCalendarPopup(rightX);
        }
        return;
    }
    if (sl.id == static_cast<int>(BarItemId::Search)) {
        CloseAllMenus();
        CloseAllFinderBarPopups();
        SendKeyCombo(kModWin, 'S');
        return;
    }

    CloseAllFinderBarPopups();
    std::vector<MenuEntry> entries;
    if (sl.id == static_cast<int>(BarItemId::Logo)) {
        entries = BuildLogoBarMenu();
    } else if (sl.id == static_cast<int>(BarItemId::ActiveApp)) {
        entries = BuildActiveAppBarMenu();
    } else if (sl.id >= static_cast<int>(BarItemId::GlobalMenuBase) &&
               sl.id < 90) {
        entries = BuildGlobalMenuByTitle(sl.label, sl.win32Sub);
    } else if (sl.id == static_cast<int>(BarItemId::Battery)) {
        std::wstring stLine =
            L"Battery: " + std::to_wstring(g_bar.status.batteryPct) + L"% (" +
            (g_bar.status.batteryCharging
                 ? L"Charging"
                 : (g_bar.status.batteryAc ? L"Plugged In" : L"On Battery")) +
            L")";
        entries.push_back(
            {MenuEntry::Type::Action, stLine, false, false, nullptr, {}});
        if (g_bar.status.batteryRemainSec > 60 && !g_bar.status.batteryAc) {
            int mins = g_bar.status.batteryRemainSec / 60;
            std::wstring rem = std::to_wstring(mins / 60) + L"h " +
                               std::to_wstring(mins % 60) + L"m remaining";
            entries.push_back(
                {MenuEntry::Type::Action, rem, false, false, nullptr, {}});
        }
        entries.push_back(
            {MenuEntry::Type::Separator, L"", false, false, nullptr, {}});
        entries.push_back({MenuEntry::Type::Action, L"Power & Battery Settings\u2026",
                           false, true,
                           [] { OpenShellUri(L"ms-settings:powersleep"); }, {}});
    } else if (sl.id == static_cast<int>(BarItemId::Network)) {
        std::wstring netLine =
            g_bar.status.netKind == 1
                ? (L"Wi-Fi: " + (g_bar.status.wifiSsid.empty()
                                     ? L"Connected"
                                     : g_bar.status.wifiSsid) +
                   L" (" + std::to_wstring(g_bar.status.wifiQuality) + L"%)")
                : (g_bar.status.netKind == 2 ? L"Ethernet: Connected"
                                             : L"Offline");
        entries.push_back(
            {MenuEntry::Type::Action, netLine, false, false, nullptr, {}});
        entries.push_back(
            {MenuEntry::Type::Separator, L"", false, false, nullptr, {}});
        entries.push_back({MenuEntry::Type::Action, L"Available Networks\u2026",
                           false, true,
                           [] { OpenShellUri(L"ms-availablenetworks:"); }, {}});
        entries.push_back({MenuEntry::Type::Action, L"Network Settings\u2026",
                           false, true,
                           [] { OpenShellUri(L"ms-settings:network"); }, {}});
    } else if (sl.id == static_cast<int>(BarItemId::Volume)) {
        entries.push_back({MenuEntry::Type::Action,
                           g_bar.status.muted ? L"Unmute Sound" : L"Mute Sound",
                           g_bar.status.muted, true,
                           [] {
                               ToggleSystemMute();
                               PollAudioStatus(g_bar.status);
                               RenderFinderBar();
                           },
                           {}});
        entries.push_back(
            {MenuEntry::Type::Separator, L"", false, false, nullptr, {}});
        for (int lv : {100, 75, 50, 25}) {
            entries.push_back(
                {MenuEntry::Type::Action,
                 L"Volume " + std::to_wstring(lv) + L"%",
                 !g_bar.status.muted && std::abs(g_bar.status.volumePct - lv) <= 5,
                 true,
                 [lv] {
                     SetSystemVolumePct(lv);
                     PollAudioStatus(g_bar.status);
                     RenderFinderBar();
                 },
                 {}});
        }
        entries.push_back(
            {MenuEntry::Type::Separator, L"", false, false, nullptr, {}});
        entries.push_back({MenuEntry::Type::Action, L"Sound Settings\u2026",
                           false, true,
                           [] { OpenShellUri(L"ms-settings:sound"); }, {}});
    }

    if (entries.empty()) {
        return;
    }
    CloseAllMenus();
    g_bar.openSlotId = sl.id;
    RenderFinderBar();
    OpenGlassMenuPopup(g_menus.mainPopup, std::move(entries),
                       POINT{leftX, anchorY}, false, false);
}

int HitTestFinderBarSlot(float lx, float ly) {
    for (const auto& sl : g_bar.slots) {
        if (PointInRectF(sl.rect, lx, ly)) {
            return sl.id;
        }
    }
    return -1;
}

void UpdateTopAppBarReservation();
void ResizeFinderBarForMonitor();
void ResetFinderBarGpuResources();
void SetBarGlassMaintenanceTimer(bool on);
void RefreshBarOpticalGlassPlates();
void CheckBarFullscreenState();
void StopBarWinEventHook();

LRESULT CALLBACK FinderBarWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    try {
        if (g_dock.taskbarCreatedMsg && msg == g_dock.taskbarCreatedMsg) {
            if (GetTickCount64() >= g_bar.ignoreTaskbarCreatedUntil) {
                UpdateTopAppBarReservation();
            }
            return 0;
        }
        switch (msg) {
            case WM_TIMER:
                if (wp == kTimerFinderBarStatus) {
                    if (g_bar.hwnd && PollFinderBarStatus(false)) {
                        RenderFinderBar();
                    }
                    return 0;
                }
                if (wp == kTimerGlassMaintenance) {
                    RefreshBarOpticalGlassPlates();
                    return 0;
                }
                if (wp == kTimerFullscreenCheck) {
                    KillTimer(hwnd, kTimerFullscreenCheck);
                    CheckBarFullscreenState();
                    return 0;
                }
                break;
            case kWmAppGlassFrame:
                g_liquidCapture.framePosted.store(false,
                                                  std::memory_order_release);
                if (EnsureLiquidCaptureReady()) {
                    RefreshBarOpticalGlassPlates();
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
                g_liquidCapture.consecutiveFailures = 0;
                g_liquidCapture.unsupported = false;
                g_bar.liquidPlate.dirty = true;
                ResizeFinderBarForMonitor();
                UpdateTopAppBarReservation();
                PollFinderBarStatus(true);
                RenderFinderBar();
                SetBarGlassMaintenanceTimer(WantOpticalLiquidGlass());
                return 0;
            case WM_HOTKEY:
                if (wp == kHotkeyFreezeGlass) {
                    g_liquidCapture.frozen = !g_liquidCapture.frozen;
                    ApplyCaptureExclusionToAllModWindows(!g_liquidCapture.frozen);
                    g_bar.liquidPlate.dirty = true;
                    RenderFinderBar();
                    return 0;
                }
                break;
            case WM_SETTINGCHANGE:
                if (lp && _wcsicmp(reinterpret_cast<PCWSTR>(lp),
                                   L"ImmersiveColorSet") == 0) {
                    UpdateGlassAppearance();
                    g_bar.liquidPlate.dirty = true;
                    RenderFinderBar();
                } else if (wp == SPI_SETWORKAREA) {
                    ResizeFinderBarForMonitor();
                }
                return 0;
            case WM_DWMCOLORIZATIONCOLORCHANGED:
                UpdateGlassAppearance();
                g_bar.liquidPlate.dirty = true;
                RenderFinderBar();
                return 0;
            case WM_DISPLAYCHANGE:
            case WM_DPICHANGED:
                CloseAllMenus();
                ShutdownLiquidCapture();
                ResetFinderBarGpuResources();
                g_dock.monitor = GetPrimaryMonitor();
                {
                    MONITORINFO mi{sizeof(mi)};
                    if (g_dock.monitor && GetMonitorInfoW(g_dock.monitor, &mi)) {
                        g_dock.monitorRect = mi.rcMonitor;
                    }
                }
                g_dock.scale = GetMonitorDpiScale(g_dock.monitor);
                ResizeFinderBarForMonitor();
                UpdateTopAppBarReservation();
                RenderFinderBar();
                SetBarGlassMaintenanceTimer(WantOpticalLiquidGlass());
                return 0;
            case kWmAppQuit:
                g_quitting = true;
                StopBarWinEventHook();
                CloseAllMenus();
                CloseAllFinderBarPopups();
                UnregisterHotKey(hwnd, kHotkeyFreezeGlass);
                KillTimer(hwnd, kTimerFinderBarStatus);
                SetBarGlassMaintenanceTimer(false);
                g_bar.glassMaintenanceTimerActive = false;
                KillTimer(hwnd, kTimerFullscreenCheck);
                DestroyWindow(hwnd);
                return 0;
            case WM_DESTROY:
                g_bar.hwnd = nullptr;
                g_dock.hwnd = nullptr;
                PostQuitMessage(0);
                return 0;
            case WM_MOUSEMOVE: {
                if (!g_bar.trackingMouse) {
                    TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                    TrackMouseEvent(&tme);
                    g_bar.trackingMouse = true;
                }
                float lx = static_cast<float>(static_cast<short>(LOWORD(lp)));
                float ly = static_cast<float>(static_cast<short>(HIWORD(lp)));
                int hit = HitTestFinderBarSlot(lx, ly);
                if (hit != g_bar.hoveredSlotId) {
                    g_bar.hoveredSlotId = hit;
                    RenderFinderBar();
                    // macOS menu hover-scrubbing while a bar menu is open
                    if (g_bar.openSlotId != -1 && hit != -1 &&
                        hit != g_bar.openSlotId && g_menus.mainPopup.open &&
                        hit < 90) {
                        for (const auto& sl : g_bar.slots) {
                            if (sl.id == hit) {
                                OpenMenuForFinderBarSlot(sl);
                                break;
                            }
                        }
                    }
                }
                return 0;
            }
            case WM_MOUSELEAVE:
                g_bar.trackingMouse = false;
                if (g_bar.hoveredSlotId != -1) {
                    g_bar.hoveredSlotId = -1;
                    RenderFinderBar();
                }
                return 0;
            case WM_LBUTTONDOWN: {
                float lx = static_cast<float>(static_cast<short>(LOWORD(lp)));
                float ly = static_cast<float>(static_cast<short>(HIWORD(lp)));
                int hit = HitTestFinderBarSlot(lx, ly);
                if (hit == -1) {
                    CloseAllMenus();
                    CloseAllFinderBarPopups();
                    return 0;
                }
                if (hit == g_bar.openSlotId) {
                    CloseAllMenus();
                    CloseAllFinderBarPopups();
                    return 0;
                }
                for (const auto& sl : g_bar.slots) {
                    if (sl.id == hit) {
                        OpenMenuForFinderBarSlot(sl);
                        break;
                    }
                }
                return 0;
            }
            case WM_RBUTTONUP: {
                POINT pt{static_cast<short>(LOWORD(lp)),
                         static_cast<short>(HIWORD(lp))};
                ClientToScreen(hwnd, &pt);
                CloseAllFinderBarPopups();
                CloseAllMenus();
                std::vector<MenuEntry> m;
                m.push_back({MenuEntry::Type::Action,
                             g_dock.darkMode ? L"Switch to Light Mode"
                                             : L"Switch to Dark Mode",
                             false, true,
                             [] {
                                 ToggleWindowsDarkModeLive();
                                 UpdateGlassAppearance();
                                 RenderFinderBar();
                             },
                             {}});
                m.push_back({MenuEntry::Type::Action, L"Refresh Tray Icons",
                             false, true, [] { RequestShellTrayIcons(); }, {}});
                m.push_back(
                    {MenuEntry::Type::Separator, L"", false, false, nullptr, {}});
                m.push_back({MenuEntry::Type::Action,
                             L"Finder Bar Settings\u2026", false, true,
                             [] { OpenWindhawkUi(); }, {}});
                OpenGlassMenuPopup(g_menus.mainPopup, std::move(m), pt, false,
                                   false);
                return 0;
            }
            case WM_MOUSEWHEEL: {
                POINT pt{static_cast<short>(LOWORD(lp)),
                         static_cast<short>(HIWORD(lp))};
                ScreenToClient(hwnd, &pt);
                int hit = HitTestFinderBarSlot(static_cast<float>(pt.x),
                                               static_cast<float>(pt.y));
                if (hit == static_cast<int>(BarItemId::Volume)) {
                    int delta = GET_WHEEL_DELTA_WPARAM(wp) > 0 ? 4 : -4;
                    PollAudioStatus(g_bar.status);
                    SetSystemVolumePct(g_bar.status.volumePct + delta);
                    PollAudioStatus(g_bar.status);
                    RenderFinderBar();
                    return 0;
                }
                break;
            }
        }
    } catch (...) {
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void RemoveTopAppBar() {
    if (g_bar.topAppBarRegistered && g_bar.topAppBarHwnd) {
        APPBARDATA abd{.cbSize = sizeof(abd), .hWnd = g_bar.topAppBarHwnd};
        SHAppBarMessage(ABM_REMOVE, &abd);
        g_bar.topAppBarRegistered = false;
    }
}

void UpdateTopAppBarReservation() {
    bool want = g_settings.enableFinderBar &&
                g_settings.finderBarReserveSpace && !g_quitting;
    if (!want || !g_bar.topAppBarHwnd) {
        RemoveTopAppBar();
        return;
    }
    HMONITOR mon = GetPrimaryMonitor();
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfoW(mon, &mi)) {
        return;
    }
    float s = GetMonitorDpiScale(mon);
    int thick = static_cast<int>(
        std::ceil(static_cast<float>(g_settings.finderBarHeight) * s));
    APPBARDATA abd{};
    abd.cbSize = sizeof(abd);
    abd.hWnd = g_bar.topAppBarHwnd;
    abd.uCallbackMessage = kWmAppAppBar;
    if (!g_bar.topAppBarRegistered) {
        if (!SHAppBarMessage(ABM_NEW, &abd)) {
            return;
        }
        g_bar.topAppBarRegistered = true;
    }
    abd.uEdge = ABE_TOP;
    abd.rc = mi.rcMonitor;
    abd.rc.bottom = abd.rc.top + thick;
    SHAppBarMessage(ABM_QUERYPOS, &abd);
    abd.rc.bottom = abd.rc.top + thick;
    SHAppBarMessage(ABM_SETPOS, &abd);
}

LRESULT CALLBACK TopAppBarWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == kWmAppAppBar && wp == ABN_POSCHANGED) {
        UpdateTopAppBarReservation();
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void ResizeFinderBarForMonitor() {
    if (!g_bar.hwnd) {
        return;
    }
    if (!g_settings.enableFinderBar) {
        ShowWindow(g_bar.hwnd, SW_HIDE);
        RemoveTopAppBar();
        return;
    }
    HMONITOR mon = GetPrimaryMonitor();
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfoW(mon, &mi)) {
        return;
    }
    g_bar.scale = GetMonitorDpiScale(mon);
    int w = mi.rcMonitor.right - mi.rcMonitor.left;
    int h = static_cast<int>(
        std::ceil(static_cast<float>(g_settings.finderBarHeight) * g_bar.scale));
    g_bar.widthPx = static_cast<float>(w);
    g_bar.heightPx = static_cast<float>(h);
    g_bar.windowRect = {mi.rcMonitor.left, mi.rcMonitor.top,
                        mi.rcMonitor.left + w, mi.rcMonitor.top + h};
    SetWindowPos(g_bar.hwnd, HWND_TOPMOST, mi.rcMonitor.left, mi.rcMonitor.top,
                 w, h,
                 SWP_NOACTIVATE |
                     (g_bar.hiddenForFullscreen ? SWP_HIDEWINDOW
                                                : SWP_SHOWWINDOW));
    UpdateTopAppBarReservation();
    g_bar.liquidPlate.dirty = true;
    RenderFinderBar();
}

void SyncFinderBarFullscreenVisibility(bool hidden) {
    if (g_bar.hiddenForFullscreen == hidden) {
        return;
    }
    g_bar.hiddenForFullscreen = hidden;
    if (g_bar.hwnd && g_settings.enableFinderBar) {
        if (hidden) {
            CloseAllFinderBarPopups();
            ShowWindow(g_bar.hwnd, SW_HIDE);
        } else {
            ShowWindow(g_bar.hwnd, SW_SHOWNOACTIVATE);
            g_bar.liquidPlate.dirty = true;
            RenderFinderBar();
        }
    }
}

void InitFinderBar(HINSTANCE hInst) {
    if (!g_gfx.compositor) {
        return;
    }
    WNDCLASSEXW wcBar{.cbSize = sizeof(wcBar),
                      .lpfnWndProc = FinderBarWndProc,
                      .hInstance = hInst,
                      .hCursor = LoadCursorW(nullptr, IDC_ARROW),
                      .lpszClassName = kFinderBarClassName};
    RegisterClassExW(&wcBar);

    WNDCLASSEXW wcTop{.cbSize = sizeof(wcTop),
                      .lpfnWndProc = TopAppBarWndProc,
                      .hInstance = hInst,
                      .lpszClassName = kTopAppBarClassName};
    RegisterClassExW(&wcTop);

    WNDCLASSEXW wcTray{.cbSize = sizeof(wcTray),
                       .lpfnWndProc = TrayClientWndProc,
                       .hInstance = hInst,
                       .lpszClassName = kTrayClientClassName};
    RegisterClassExW(&wcTray);

    g_bar.trayClientHwnd = CreateWindowExW(
        0, kTrayClientClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
        hInst, nullptr);

    g_bar.topAppBarHwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kTopAppBarClassName, L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, hInst,
        nullptr);
    if (g_bar.topAppBarHwnd) {
        SetLayeredWindowAttributes(g_bar.topAppBarHwnd, 0, 0, LWA_ALPHA);
    }

    g_bar.hwnd = CreateWindowExW(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_LAYERED | WS_EX_TOPMOST |
            WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kFinderBarClassName, L"macOS 27 Finder Bar", WS_POPUP, 0, 0, 100, 28,
        nullptr, nullptr, hInst, nullptr);
    if (!g_bar.hwnd) {
        return;
    }
    g_dock.hwnd = g_bar.hwnd;
    g_dock.taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
    g_msgOpenDockSettings =
        RegisterWindowMessageW(L"WindhawkMacOSTahoe_OpenDockSettings");
    RegisterHotKey(g_bar.hwnd, kHotkeyFreezeGlass,
                   MOD_WIN | MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT, 'M');
    SetBarGlassMaintenanceTimer(WantOpticalLiquidGlass());
    SetLayeredWindowAttributes(g_bar.hwnd, 0, 255, LWA_ALPHA);
    ApplyWindowGlassCaptureAffinity(g_bar.hwnd);
    const BOOL hb = TRUE;
    DwmSetWindowAttribute(g_bar.hwnd, kDwmUseHostBackdropBrush, &hb,
                          sizeof(hb));

    auto& c = g_gfx.compositor;
    auto desk = c.as<ICompositorDesktopInteropLocal>();
    if (SUCCEEDED(desk->CreateDesktopWindowTarget(
            g_bar.hwnd, TRUE, winrt::put_abi(g_bar.target))) &&
        g_bar.target) {
        g_bar.root = c.CreateContainerVisual();
        g_bar.target.Root(g_bar.root);

        g_bar.fallbackBackdrop = c.CreateSpriteVisual();
        g_bar.fallbackBackdrop.Brush(CreateLiquidGlassBackdropBrush(
            c, g_dock.hostBackdrop, 14.0f * g_dock.scale, 1.35f));
        g_bar.root.Children().InsertAtTop(g_bar.fallbackBackdrop);

        g_bar.fallbackTint = c.CreateSpriteVisual();
        g_bar.root.Children().InsertAtTop(g_bar.fallbackTint);

        g_bar.fgBrush = c.CreateSurfaceBrush();
        g_bar.fgBrush.Stretch(wuc::CompositionStretch::Fill);
        g_bar.fgVisual = c.CreateSpriteVisual();
        g_bar.fgVisual.Brush(g_bar.fgBrush);
        g_bar.root.Children().InsertAtTop(g_bar.fgVisual);
    }

    PollFinderBarStatus(true);
    if (HWND fg = GetForegroundWindow()) {
        FinderBarUpdateActiveApp(fg);
    }
    ResizeFinderBarForMonitor();
    if (g_dock.hwnd) {
        SetTimer(g_dock.hwnd, kTimerFinderBarStatus, 1000, nullptr);
    }
    RequestShellTrayIcons();
}

void ShutdownFinderBar(HINSTANCE hInst) {
    if (g_dock.hwnd) {
        KillTimer(g_dock.hwnd, kTimerFinderBarStatus);
    }
    CloseAllFinderBarPopups();
    RemoveTopAppBar();
    if (g_bar.topAppBarHwnd) {
        DestroyWindow(g_bar.topAppBarHwnd);
        g_bar.topAppBarHwnd = nullptr;
    }
    if (g_bar.trayClientHwnd) {
        DestroyWindow(g_bar.trayClientHwnd);
        g_bar.trayClientHwnd = nullptr;
    }
    g_bar.liquidPlate.Shutdown();
    g_bar.hasGlassParams = false;
    g_bar.liquidAttached = false;
    g_bar.fmtReg = nullptr;
    g_bar.fmtBold = nullptr;
    g_bar.fmtSmall = nullptr;
    g_bar.cachedFmtScale = 0.0f;
    g_bar.fgVisual = nullptr;
    g_bar.fgBrush = nullptr;
    g_bar.fgSurface = nullptr;
    g_bar.fgSurfSize = {};
    g_bar.fallbackTintBrush = nullptr;
    g_bar.fallbackTint = nullptr;
    g_bar.fallbackBackdrop = nullptr;
    g_bar.root = nullptr;
    g_bar.target = nullptr;
    if (g_bar.hwnd) {
        DestroyWindow(g_bar.hwnd);
        g_bar.hwnd = nullptr;
    }
    ClearMirroredTrayIcons();
    g_sysCom.Shutdown();
    UnregisterClassW(kFinderBarClassName, hInst);
    UnregisterClassW(kTopAppBarClassName, hInst);
    UnregisterClassW(kTrayClientClassName, hInst);
}

void AddFinderBarWatchedRegion(std::vector<RECT>& watchList) {
    if (g_bar.hwnd && g_settings.enableFinderBar &&
        !g_bar.hiddenForFullscreen &&
        g_settings.finderBarStyle == FinderBarStyle::LiquidGlass) {
        RECT br = g_bar.windowRect;
        if (br.right > br.left && br.bottom > br.top) {
            InflateRect(&br, 16, 16);
            watchList.push_back(br);
        }
    }
}

void ResetFinderBarGpuResources() {
    CloseAllFinderBarPopups();
    g_bar.liquidPlate.ResetGpuResources();
    g_bar.fgSurface = nullptr;
    g_bar.fgSurfSize = {};
    RenderFinderBar();
}

// ============================================================================

HWINEVENTHOOK g_barWinHook = nullptr;

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
    s.finderBarHeight =
        std::clamp(Wh_GetIntSetting(L"finderBarHeight"), 22, 40);
    std::wstring fbs = ReadWhStringSetting(L"finderBarStyle");
    if (fbs == L"transparent") {
        s.finderBarStyle = FinderBarStyle::Transparent;
    } else if (fbs == L"frosted") {
        s.finderBarStyle = FinderBarStyle::Frosted;
    } else if (fbs == L"solid") {
        s.finderBarStyle = FinderBarStyle::Solid;
    } else {
        s.finderBarStyle = FinderBarStyle::LiquidGlass;
    }

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

    s.finderBarDesktopName = ReadWhStringSetting(L"finderBarDesktopName");
    if (s.finderBarDesktopName.empty()) {
        s.finderBarDesktopName = L"Finder";
    }
    s.finderBarReserveSpace =
        Wh_GetIntSetting(L"finderBarReserveSpace") != 0;
    s.finderBarShowSeconds = Wh_GetIntSetting(L"finderBarShowSeconds") != 0;
    s.finderBarShow24h = Wh_GetIntSetting(L"finderBarShow24h") != 0;
    s.finderBarShowBatteryPct =
        Wh_GetIntSetting(L"finderBarShowBatteryPct") != 0;
    s.finderBarShowTray = Wh_GetIntSetting(L"finderBarShowTray") != 0;

    g_settings = std::move(s);
    UpdateGlassAppearance();
}

void CheckBarFullscreenState() {
    bool fs = false;
    QUERY_USER_NOTIFICATION_STATE quns = QUNS_ACCEPTS_NOTIFICATIONS;
    if (SUCCEEDED(SHQueryUserNotificationState(&quns))) {
        if (quns == QUNS_BUSY || quns == QUNS_RUNNING_D3D_FULL_SCREEN ||
            quns == QUNS_PRESENTATION_MODE) {
            fs = true;
        }
    }
    if (!fs) {
        HWND fg = GetForegroundWindow();
        if (fg && IsWindow(fg) && !IsIconic(fg)) {
            DWORD pid = 0;
            GetWindowThreadProcessId(fg, &pid);
            if (pid != GetCurrentProcessId()) {
                WCHAR cls[64]{};
                GetClassNameW(fg, cls, ARRAYSIZE(cls));
                bool isDesktop =
                    _wcsicmp(cls, L"Progman") == 0 ||
                    _wcsicmp(cls, L"WorkerW") == 0 ||
                    _wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
                    _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0;
                if (!isDesktop) {
                    RECT rc{};
                    if (GetWindowRect(fg, &rc)) {
                        const RECT& mr = g_dock.monitorRect;
                        if (rc.left <= mr.left && rc.top <= mr.top &&
                            rc.right >= mr.right && rc.bottom >= mr.bottom) {
                            fs = true;
                        }
                    }
                }
            }
        }
    }
    if (fs != g_dock.fullscreenHidden) {
        g_dock.fullscreenHidden = fs;
        SyncFinderBarFullscreenVisibility(fs);
    }
}

void CALLBACK BarWinEventProc(HWINEVENTHOOK,
                              DWORD event,
                              HWND hwnd,
                              LONG,
                              LONG,
                              DWORD,
                              DWORD) {
    if (g_quitting || !g_bar.hwnd) {
        return;
    }
    if (event == EVENT_SYSTEM_FOREGROUND) {
        FinderBarUpdateActiveApp(hwnd);
        SetTimer(g_bar.hwnd, kTimerFullscreenCheck, 120, nullptr);
    } else if (event == EVENT_SYSTEM_MOVESIZEEND ||
               event == EVENT_SYSTEM_MINIMIZESTART ||
               event == EVENT_SYSTEM_MINIMIZEEND) {
        SetTimer(g_bar.hwnd, kTimerFullscreenCheck, 120, nullptr);
    }
}

void StartBarWinEventHook() {
    if (!g_barWinHook) {
        g_barWinHook = SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_MINIMIZEEND, nullptr,
            BarWinEventProc, 0, 0,
            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    }
}

void StopBarWinEventHook() {
    if (g_barWinHook) {
        UnhookWinEvent(g_barWinHook);
        g_barWinHook = nullptr;
    }
}

void SetBarGlassMaintenanceTimer(bool on) {
    if (!g_bar.hwnd || on == g_bar.glassMaintenanceTimerActive) {
        return;
    }
    g_bar.glassMaintenanceTimerActive = on;
    if (on) {
        SetTimer(g_bar.hwnd, kTimerGlassMaintenance, 150, nullptr);
    } else {
        KillTimer(g_bar.hwnd, kTimerGlassMaintenance);
    }
}

void RefreshBarOpticalGlassPlates() {
    const bool want =
        !g_quitting && g_bar.hwnd && !g_bar.hiddenForFullscreen &&
        (g_settings.finderBarStyle == FinderBarStyle::LiquidGlass) &&
        WantOpticalLiquidGlass() && !g_liquidCapture.frozen;

    bool anyPopupOpen = (g_menus.mainPopup.open || g_menus.subPopup.open);
    std::vector<RECT> watchList;
    AddFinderBarWatchedRegion(watchList);
    auto addPopWatch = [&](HWND h) {
        RECT r{};
        if (h && IsWindow(h) && IsWindowVisible(h) && GetWindowRect(h, &r)) {
            InflateRect(&r, 16, 16);
            watchList.push_back(r);
            anyPopupOpen = true;
        }
    };
    addPopWatch(g_cc.hwnd);
    addPopWatch(g_trayPop.hwnd);
    addPopWatch(g_cal.hwnd);
    UpdateLiquidCaptureWatchedRegions(watchList);
    g_liquidCapture.highRate.store(anyPopupOpen, std::memory_order_relaxed);
    g_liquidCapture.watching.store(want, std::memory_order_relaxed);

    if (!want) {
        SetBarGlassMaintenanceTimer(false);
        return;
    }
    uint64_t prevFrame = g_liquidCapture.frameId;
    bool ready = EnsureLiquidCaptureReady();
    SetBarGlassMaintenanceTimer(want &&
                                (!g_liquidCapture.worker || !g_bar.liquidPlate.active));
    if (!ready) {
        return;
    }
    if (g_liquidCapture.frameId != prevFrame || !g_bar.liquidPlate.active) {
        if (g_bar.liquidPlate.active && g_bar.hasGlassParams &&
            g_bar.liquidAttached) {
            ULONGLONG minInterval = anyPopupOpen ? 25u : 50u;
            if (GetTickCount64() - g_bar.liquidPlate.lastDrawTick < minInterval) {
                return;
            }
            RenderLiquidGlassPlate(g_bar.liquidPlate, g_bar.lastGlassParams);
        } else {
            g_bar.liquidPlate.dirty = true;
            RenderFinderBar();
        }
    }
}

HWND GetGlassNotifyHwnd() {
    return g_bar.hwnd;
}

void ApplyFinderBarCaptureExclusion(DWORD affinity) {
    for (HWND h : {g_bar.hwnd, g_cc.hwnd, g_trayPop.hwnd, g_cal.hwnd,
                   g_menus.mainPopup.hwnd, g_menus.subPopup.hwnd}) {
        if (h && IsWindow(h)) {
            SetWindowDisplayAffinity(h, affinity);
        }
    }
}

void ApplyCaptureExclusionToAllModWindows(bool excluded) {
    ApplyFinderBarCaptureExclusion(excluded ? kWdaExcludeFromCapture : WDA_NONE);
}

void OnGraphicsDeviceLost() {
    ShutdownLiquidCapture();
    ResetFinderBarGpuResources();
    g_menus.mainPopup.liquidPlate.ResetGpuResources();
    g_menus.subPopup.liquidPlate.ResetGpuResources();
    RenderFinderBar();
    SetBarGlassMaintenanceTimer(WantOpticalLiquidGlass());
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
                InitFinderBar(hSelf);
                if (g_bar.hwnd) {
                    StartBarWinEventHook();
                    CheckBarFullscreenState();
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

    StopBarWinEventHook();
    CloseAllMenus();
    ShutdownFinderBar(hSelf);
    UnregisterClassW(kMenuClassName, hSelf);
    ShutdownLiquidCapture();
    g_gfx.Shutdown();
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
    if (g_bar.hwnd) {
        PostMessageW(g_bar.hwnd, kWmAppSettings, 0, 0);
    }
}

void WhTool_ModUninit() {
    g_quitting = true;
    if (g_bar.hwnd) {
        PostMessageW(g_bar.hwnd, kWmAppQuit, 0, 0);
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
// Dual-process boilerplate: explorer.exe hosts the lightweight tray-icon mirror
// hook, while windhawk.exe (-tool-mod) hosts the standalone Top Menu Bar.
////////////////////////////////////////////////////////////////////////////////

bool g_isExplorerProcess = false;
bool g_isToolModProcessLauncher = false;
HANDLE g_toolModProcessMutex = nullptr;
HANDLE g_toolModClosingEvent = nullptr;

bool IsCurrentProcessExplorer() {
    WCHAR path[MAX_PATH]{};
    DWORD n = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (n == 0 || n >= ARRAYSIZE(path)) {
        return false;
    }
    const wchar_t* fn = PathFindFileNameW(path);
    return fn && _wcsicmp(fn, L"explorer.exe") == 0;
}

void WINAPI EntryPoint_Hook() {
    ExitThread(0);
}

BOOL Wh_ModInit() {
    if (IsCurrentProcessExplorer()) {
        g_isExplorerProcess = true;
        return ExplorerTrayHost::Init();
    }

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
            ULONGLONG start = GetTickCount64();
            for (;;) {
                Sleep(100);
                HANDLE m = CreateMutexW(nullptr, TRUE,
                                        L"windhawk-tool-mod_" WH_MOD_ID);
                if (m && GetLastError() != ERROR_ALREADY_EXISTS) {
                    g_toolModProcessMutex = m;
                    break;
                }
                if (m) CloseHandle(m);
                bool closing = false;
                if (HANDLE ev = OpenEventW(
                        SYNCHRONIZE, FALSE,
                        L"windhawk-tool-mod-closing_" WH_MOD_ID)) {
                    closing = WaitForSingleObject(ev, 0) == WAIT_OBJECT_0;
                    CloseHandle(ev);
                }
                if (GetTickCount64() - start > (closing ? 12000u : 3000u)) {
                    break;
                }
            }
            if (!g_toolModProcessMutex) {
                ExitProcess(1);
            }
        }
        g_toolModClosingEvent = CreateEventW(
            nullptr, TRUE, FALSE, L"windhawk-tool-mod-closing_" WH_MOD_ID);
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
    if (g_isExplorerProcess || !g_isToolModProcessLauncher) {
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
    if (g_isExplorerProcess || g_isToolModProcessLauncher) {
        return;
    }
    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isExplorerProcess) {
        ExplorerTrayHost::Uninit();
        return;
    }
    if (g_isToolModProcessLauncher) {
        return;
    }
    if (g_toolModClosingEvent) {
        SetEvent(g_toolModClosingEvent);
    }
    WhTool_ModUninit();
    if (g_toolModClosingEvent) {
        CloseHandle(g_toolModClosingEvent);
        g_toolModClosingEvent = nullptr;
    }
    ExitProcess(0);
}

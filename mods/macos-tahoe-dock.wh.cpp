// ==WindhawkMod==
// @id              macos-tahoe-dock
// @name            macOS 26 Tahoe Dock
// @description     Standalone macOS 26 Tahoe Liquid Glass Dock for Windows 11 with magnification, stacks, live previews, and custom icons
// @version         1.3
// @author          Badalemo
// @github          https://github.com/Badalemo
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luuid -lshell32 -lshlwapi -lpropsys -ldwmapi -ld2d1 -ld3d11 -ldxgi -ldwrite -lwindowscodecs -lgdi32 -luser32 -lversion -lshcore -lcomctl32 -lcomdlg32 -ladvapi32
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# macOS 26 Tahoe Dock
A standalone **macOS 26 Tahoe Liquid Glass Dock** for Windows 11.

![macOS 26 Tahoe Dock](https://raw.githubusercontent.com/Badalemo/windhawk-mods-media/main/dock.png)

![macOS 26 Tahoe Dock magnification demo](https://raw.githubusercontent.com/Badalemo/windhawk-mods-media/main/dock-demo.gif)

## Features
- **True Optical Liquid Glass**: Real-time DXGI Desktop Duplication with 32-bit float Apple-squircle curved-lens refraction (`CLSID_D2D1DisplacementMap`), crisp blur, saturation boost, dark-mode highlight limiter, outer dark separation hairline, sculpted inner bevel halo, dual-corner Fresnel catchlight rim, and Clear/Dark/Default/Tinted icon styles. Press **Win+Ctrl+Shift+G** (or right-click Dock → *Liquid Glass → Freeze Glass for Screenshot*) to freeze glass for screenshots.
- **True macOS Dock + Windows Taskbar Sync**: Live two-way Windows Taskbar pin sync (`IPinnedList3`, including Microsoft Store/UWP & PWA apps), native **Pin to Taskbar / Unpin from Taskbar**, Windows Jump Lists (**Recent** items), **Shift+Click** new window, **Ctrl+Shift+Click / Run as administrator**, smooth cosine-bell magnification, live window preview flyouts, Liquid Glass menus & Control Panel, **1-click icon changer** (`.png/.icns/.ico/.jpg/.webp`), drag-to-reorder & drag-off-to-unpin, OLE file drag-and-drop, and multi-folder Stacks.

## Comparison with Existing Dock Mods
Compared to [`mac-island-dock`](https://windhawk.net/mods/mac-island-dock), [`macos-style-dock`](https://windhawk.net/mods/macos-style-dock), and [`desktop-icon-dock`](https://windhawk.net/mods/desktop-icon-dock), **macOS 26 Tahoe Dock** runs as a standalone taskbar replacement in its own dedicated process while providing live two-way Windows Taskbar pin sync (`IPinnedList3`), Jump Lists, folder/Downloads stacks, DXGI curved-lens optical refraction glass, and an integrated Liquid Glass settings panel.

## Important Notes
- **Hide Windows taskbar** (off by default): when enabled, changes Windows' taskbar auto-hide setting (`ABM_SETSTATE`) while the dock runs and restores it on unload and startup. If the process terminates unexpectedly, auto-hide may remain enabled until the dock restarts or you toggle it in Windows Settings.
- **Taskbar pins**: pinning/unpinning from the dock menu, dropping apps onto the dock, or dragging pinned apps off the dock updates your Windows taskbar pins via `IPinnedList3`.
- **Optical Liquid Glass**: captures the screen behind the dock locally in GPU memory while active (`Win+Ctrl+Shift+G` freezes the frame for screenshots).

## Credits & License
Licensed under **GPL-3.0**. The DXGI Desktop Duplication Liquid Glass refraction engine, PaintCode squircle geometry, blur interop, and `.icns` reader are adapted from **MacOS Style Dock** by **Zach Vaughn** (`GPL-3.0`).

## Custom Icons
Right-click any icon in the dock and choose **Options → Change Icon…** (or **Change Icon…** on Finder/Apps/Downloads/Trash) to pick any `.png`, `.icns` (native macOS icon), `.ico`, `.jpg`, or `.webp` file — changes apply instantly and persist automatically.

You can also set **Custom icons folder** for bulk overrides:
- `finder.png`, `apps.png`, `downloads.png`, `trash.png`, `trash-full.png` (also `.icns` / `.ico`)
- `<exe-name-without-extension>.png` (e.g., `chrome.png`, `code.png`)
- `<aumid-with-!-replaced-by-_>.png`
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- position: bottom
  $name: Dock position
  $description: Screen edge where the dock is anchored.
  $options:
  - bottom: Bottom
  - left: Left
  - right: Right
- iconSize: 48
  $name: Icon size
  $description: Base tile size in DIPs (32-128).
- magnification: true
  $name: Magnification
  $description: Magnify icons on mouse hover using the macOS curve.
- magnificationScale: 180
  $name: Magnification scale (%)
  $description: Hover magnification factor as a percentage of base icon size (100-300, e.g. 180 = 1.8x).
- autoHide: false
  $name: Automatically hide and show the dock
  $description: Slide the dock offscreen when not in use and reveal it at the screen edge.
- autoHideDelay: 200
  $name: Auto-hide delay (ms)
  $description: Delay after the cursor leaves the dock before hiding.
- autoHideAnimation: 350
  $name: Auto-hide animation duration (ms)
  $description: Duration of the dock slide-in and slide-out animation.
- reserveSpace: true
  $name: Reserve screen space
  $description: Keep maximized windows from covering the dock when auto-hide is disabled.
- edgeGap: 4
  $name: Screen edge gap
  $description: Gap in DIPs between the dock shelf and the screen edge.
- appearance: auto
  $name: Appearance
  $description: Color scheme for Liquid Glass, icon plates, menus, and tooltips.
  $options:
  - auto: Automatic (follow Windows theme)
  - light: Light
  - dark: Dark
- glassStyle: clear
  $name: Liquid Glass style
  $description: Choose clear neutral Liquid Glass or accent-tinted Liquid Glass.
  $options:
  - clear: Clear
  - tinted: Tinted
- glassRefraction: lens
  $name: Liquid Glass edge refraction
  $description: Real optical glass refraction bending what's behind the dock & bar rims (DXGI Desktop Duplication). Press Win+Ctrl+Shift+G to freeze glass for screenshots.
  $options:
  - lens: Lens (macOS 27)
  - reflection: Mirror Reflection
  - stretch: Convex Stretch
  - "off": Off (Frosted Blur Only)
- glassEdgeDepth: 16
  $name: Liquid Glass rim depth (4-48)
  $description: Depth of the curved refracting glass bevel in DIPs.
- glassEdgeStrength: 105
  $name: Liquid Glass refraction strength (0-150)
  $description: Optical bending strength around the curved glass rim.
- glassTint: 22
  $name: Glass tint opacity (0-100)
  $description: Controls smoked glass fill density (lower = clearer crystal glass).
- glassBlur: 36
  $name: Liquid Glass blur (0-100)
  $description: Controls optical glass softening behind the shelf and menus.
- glassBrightnessLimit: 78
  $name: Dark glass brightness ceiling (30-100)
  $description: Caps bright backgrounds shining through Dark Liquid Glass so white windows stay rich smoked glass.
- iconStyle: clear
  $name: Icon style (macOS 26 Tahoe)
  $description: Choose Clear (frosted monochrome glass), Dark (smoked squircle with full-color glyphs), Default (full color), or Tinted (accent-washed glass). Custom icon overrides are always kept as-is.
  $options:
  - clear: Clear (Frosted Glass)
  - dark: Dark (Smoked Glass)
  - default: Default (Full Color)
  - tinted: Tinted (Accent Glass)
- dockIconReflections: false
  $name: Subtle icon reflections on glass
  $description: Cast a soft colored reflection from each app icon onto the Liquid Glass dock shelf.
- showWindowPreviews: true
  $name: Show live window preview flyouts on hover
  $description: Hovering over an open app in the Dock displays live GPU window preview cards to switch or close windows.
- showIndicators: true
  $name: Show indicators for open applications
  $description: Display a dot beneath running applications.
- animateOpening: true
  $name: Animate opening applications
  $description: Bounce application icons while launching.
- bounceForAttention: true
  $name: Bounce for attention
  $description: Bounce application icons when a window flashes for attention.
- minimizeIntoAppIcon: true
  $name: Minimize windows into application icon
  $description: Minimize windows into their application icon instead of separate dock tiles.
- showRecents: true
  $name: Show suggested and recent applications in Dock
  $description: Keep recently closed unpinned apps in a separate section before Downloads.
- recentsCount: 3
  $name: Recent applications limit
  $description: Maximum number of recent unpinned apps to show.
- showDownloads: true
  $name: Show Downloads stack
  $description: Show the Downloads folder stack beside Trash.
- downloadsFolder: ""
  $name: Downloads folder override
  $description: Optional folder path to display in the Downloads stack (leave empty for default Downloads).
- showFinder: true
  $name: Show Finder (File Explorer) button
  $description: Show a Finder-style File Explorer button on the leading end of the dock.
- showAppsButton: true
  $name: Show Apps (Start) button
  $description: Show an Apps launcher button beside Finder.
- appsButtonAction: start
  $name: Apps button action
  $description: Action when clicking the Apps button beside Finder.
  $options:
  - start: Open Windows Start menu
  - allApps: Open Applications folder
- customIconsFolder: ""
  $name: Custom icons folder
  $description: Optional directory of PNG icon overrides (finder.png, apps.png, trash.png, trash-full.png, downloads.png, <exe>.png).
- hideTaskbar: false
  $name: Hide Windows taskbar
  $description: Hide the Windows taskbar while the dock is active and restore it on unload.
- clickActiveToMinimize: false
  $name: Click active app to minimize
  $description: Clicking the currently focused app minimizes its windows.
- bringAllWindows: false
  $name: Bring all windows to front on click
  $description: Clicking a running app brings all of its windows forward instead of only the last active one.
*/
// ==/WindhawkModSettings==

// ============================================================================
// 2. Includes & Aliases
// ============================================================================

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
constexpr UINT kWmAppAppBar = WM_APP + 103;
constexpr UINT kWmAppDeferredAction = WM_APP + 105;
constexpr UINT kWmAppGlassFrame = WM_APP + 106;
constexpr UINT kWmAppShellNotify = WM_APP + 107;
constexpr UINT kWmAppTaskbarPinsChanged = WM_APP + 108;
constexpr UINT kWmAppDeviceLost = WM_APP + 109;
constexpr UINT kWmAppDeviceRedraw = WM_APP + 110;

inline std::atomic<bool> g_deviceLostPending{false};
inline std::atomic<bool> g_deviceRedrawPending{false};
inline std::atomic<bool> g_settingRenderingDevice{false};
void PostDeviceLostMessage();
void PostDeviceRedrawMessage();

constexpr UINT_PTR kTimerCursorPoll = 1;
constexpr UINT_PTR kTimerTaskbarGuard = 2;
constexpr UINT_PTR kTimerWindowRefresh = 3;
constexpr UINT_PTR kTimerFullscreenCheck = 4;
constexpr UINT_PTR kTimerGlassMaintenance = 5;
constexpr UINT_PTR kTimerFlyoutHover = 8;
constexpr UINT_PTR kTimerFlyoutLeave = 9;
constexpr UINT_PTR kTimerTaskbarPinSync = 10;

constexpr int kHotkeyFreezeGlass = 1;

constexpr DWORD kEventObjectCloaked = 0x8017;
constexpr DWORD kEventObjectUncloaked = 0x8018;
constexpr WPARAM kShellHookFlash = 0x8006;

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
// 3. Settings & Runtime Storage
// ============================================================================

enum class DockPosition { Bottom = 0, Left = 1, Right = 2 };
enum class AppearanceMode { Auto = 0, Light = 1, Dark = 2 };
enum class GlassStyle { Clear = 0, Tinted = 1 };
enum class GlassRefraction { Lens = 0, Reflection = 1, Stretch = 2, Off = 3 };
enum class DockIconStyle { Clear = 0, Dark = 1, Default = 2, Tinted = 3 };
enum class AppsButtonAction { Start = 0, AllApps = 1 };
enum class StackViewStyle { Fan = 0, Grid = 1, List = 2 };

struct DockSettings {
    DockPosition position = DockPosition::Bottom;
    int iconSize = 48;
    bool magnification = true;
    int magnificationScale = 180;
    float EffectiveMagScale() const {
        return magnification
                   ? static_cast<float>(
                         std::clamp(magnificationScale, 100, 300)) /
                         100.0f
                   : 1.0f;
    }
    int EffectiveMagSize() const {
        return std::max(
            iconSize,
            static_cast<int>(std::lround(static_cast<float>(iconSize) *
                                         EffectiveMagScale())));
    }
    bool autoHide = false;
    int autoHideDelay = 200;
    int autoHideAnimation = 350;
    bool reserveSpace = true;
    int edgeGap = 4;
    AppearanceMode appearance = AppearanceMode::Auto;
    GlassStyle glassStyle = GlassStyle::Clear;
    GlassRefraction glassRefraction = GlassRefraction::Lens;
    DockIconStyle iconStyle = DockIconStyle::Clear;
    int glassEdgeDepth = 16;
    int glassEdgeStrength = 105;
    int glassTint = 22;
    int glassBlur = 36;
    int glassBrightnessLimit = 78;
    bool dockIconReflections = false;
    bool showWindowPreviews = true;
    bool showIndicators = true;
    bool animateOpening = true;
    bool bounceForAttention = true;
    bool minimizeIntoAppIcon = true;
    bool showRecents = true;
    int recentsCount = 3;
    bool showDownloads = true;
    std::wstring downloadsFolder;
    bool showFinder = true;
    bool showAppsButton = true;
    AppsButtonAction appsButtonAction = AppsButtonAction::Start;
    std::wstring customIconsFolder;
    bool hideTaskbar = false;
    bool clickActiveToMinimize = false;
    bool bringAllWindows = false;

};

DockSettings g_settings;

std::wstring NativeSystemPath(const std::wstring& path) {
    static const bool wow64 = [] {
        BOOL w = FALSE;
        return IsWow64Process(GetCurrentProcess(), &w) && w;
    }();
    if (!wow64 || path.empty()) {
        return path;
    }
    WCHAR winDir[MAX_PATH]{};
    UINT n = GetWindowsDirectoryW(winDir, ARRAYSIZE(winDir));
    if (n == 0 || n >= ARRAYSIZE(winDir)) {
        return path;
    }
    std::wstring prefix = std::wstring(winDir) + L"\\System32\\";
    if (path.size() > prefix.size() &&
        _wcsnicmp(path.c_str(), prefix.c_str(), prefix.size()) == 0) {
        return std::wstring(winDir) + L"\\Sysnative\\" +
               path.substr(prefix.size());
    }
    return path;
}

std::wstring ReadWhStringSetting(PCWSTR name) {
    PCWSTR val = Wh_GetStringSetting(name);
    std::wstring result = val ? val : L"";
    if (val) {
        Wh_FreeStringSetting(val);
    }
    return result;
}

std::wstring ReadWhStringValue(PCWSTR name, PCWSTR defVal = L"") {
    size_t cap = 4096;
    while (cap <= (1u << 20)) {
        std::wstring buf(cap, L'\0');
        size_t n = Wh_GetStringValue(name, buf.data(), buf.size());
        if (n == 0 && defVal && *defVal) {
            return defVal;
        }
        if (n < cap - 1) {
            buf.resize(wcslen(buf.c_str()));
            return buf;
        }
        cap *= 4;
    }
    std::wstring buf(1u << 20, L'\0');
    Wh_GetStringValue(name, buf.data(), buf.size());
    buf.resize(wcslen(buf.c_str()));
    return buf;
}

void SaveNumberedStringList(PCWSTR prefix,
                            const std::vector<std::wstring>& entries) {
    std::wstring countKey = std::wstring(prefix) + L"_count";
    int prevCount = Wh_GetIntValue(countKey.c_str(), 0);
    int newCount = static_cast<int>(entries.size());
    Wh_SetIntValue(countKey.c_str(), newCount);
    for (int i = 0; i < newCount; ++i) {
        std::wstring key = std::wstring(prefix) + L"_" + std::to_wstring(i);
        std::wstring val = entries[static_cast<size_t>(i)];
        for (wchar_t& ch : val) {
            if (ch == L'\r' || ch == L'\n') ch = L' ';
        }
        Wh_SetStringValue(key.c_str(), val.c_str());
    }
    for (int i = newCount; i < prevCount; ++i) {
        std::wstring key = std::wstring(prefix) + L"_" + std::to_wstring(i);
        Wh_DeleteValue(key.c_str());
    }
}

std::vector<std::wstring> LoadNumberedStringList(PCWSTR prefix,
                                                 PCWSTR legacyKey) {
    std::vector<std::wstring> out;
    std::wstring countKey = std::wstring(prefix) + L"_count";
    int count = Wh_GetIntValue(countKey.c_str(), -1);
    if (count >= 0) {
        out.reserve(static_cast<size_t>(std::min(count, 4096)));
        for (int i = 0; i < count; ++i) {
            std::wstring key =
                std::wstring(prefix) + L"_" + std::to_wstring(i);
            std::wstring val = ReadWhStringValue(key.c_str());
            if (!val.empty()) {
                out.push_back(std::move(val));
            }
        }
        return out;
    }
    if (legacyKey && *legacyKey) {
        std::wstring raw = ReadWhStringValue(legacyKey);
        size_t pos = 0;
        while (pos < raw.size()) {
            size_t nl = raw.find(L'\n', pos);
            if (nl == std::wstring::npos) nl = raw.size();
            std::wstring line = raw.substr(pos, nl - pos);
            pos = nl + 1;
            if (!line.empty()) {
                out.push_back(std::move(line));
            }
        }
        if (!out.empty()) {
            SaveNumberedStringList(prefix, out);
        }
    }
    return out;
}

void ClearRuntimeOverrides() {
    Wh_SetIntValue(L"ovr_autohide", -1);
    Wh_SetIntValue(L"ovr_magnify", -1);
    Wh_SetIntValue(L"ovr_position", -1);
    Wh_SetIntValue(L"ovr_glass_refract", -1);
    Wh_SetIntValue(L"ovr_glass_style", -1);
    Wh_SetIntValue(L"ovr_icon_refl", -1);
    Wh_SetIntValue(L"ovr_icon_style", -1);
    Wh_SetIntValue(L"ovr_previews", -1);
    Wh_SetIntValue(L"ovr_icon_size", -1);
    Wh_SetIntValue(L"ovr_mag_scale", -1);
    Wh_SetIntValue(L"ovr_mag_size", -1);
    Wh_SetIntValue(L"ovr_glass_blur", -1);
    Wh_SetIntValue(L"ovr_glass_tint", -1);
    Wh_SetIntValue(L"ovr_min_into_icon", -1);
    Wh_SetIntValue(L"ovr_show_recents", -1);
    Wh_SetIntValue(L"ovr_hide_taskbar", -1);
}

void ApplyRuntimeOverrides(DockSettings& s) {
    int ovrAutoHide = Wh_GetIntValue(L"ovr_autohide", -1);
    if (ovrAutoHide == 0 || ovrAutoHide == 1) {
        s.autoHide = (ovrAutoHide == 1);
    }
    int ovrMag = Wh_GetIntValue(L"ovr_magnify", -1);
    if (ovrMag == 0 || ovrMag == 1) {
        s.magnification = (ovrMag == 1);
    }
    int ovrPos = Wh_GetIntValue(L"ovr_position", -1);
    if (ovrPos >= 0 && ovrPos <= 2) {
        s.position = static_cast<DockPosition>(ovrPos);
    }
    int ovrRefract = Wh_GetIntValue(L"ovr_glass_refract", -1);
    if (ovrRefract >= 0 && ovrRefract <= 3) {
        s.glassRefraction = static_cast<GlassRefraction>(ovrRefract);
    }
    int ovrStyle = Wh_GetIntValue(L"ovr_glass_style", -1);
    if (ovrStyle == 0 || ovrStyle == 1) {
        s.glassStyle = static_cast<GlassStyle>(ovrStyle);
    }
    int ovrRefl = Wh_GetIntValue(L"ovr_icon_refl", -1);
    if (ovrRefl == 0 || ovrRefl == 1) {
        s.dockIconReflections = (ovrRefl == 1);
    }
    int ovrIcoStyle = Wh_GetIntValue(L"ovr_icon_style", -1);
    if (ovrIcoStyle >= 0 && ovrIcoStyle <= 3) {
        s.iconStyle = static_cast<DockIconStyle>(ovrIcoStyle);
    }
    int ovrPrev = Wh_GetIntValue(L"ovr_previews", -1);
    if (ovrPrev == 0 || ovrPrev == 1) {
        s.showWindowPreviews = (ovrPrev == 1);
    }
    int ovrIcoSz = Wh_GetIntValue(L"ovr_icon_size", -1);
    if (ovrIcoSz >= 28 && ovrIcoSz <= 128) {
        s.iconSize = ovrIcoSz;
    }
    int ovrMagScale = Wh_GetIntValue(L"ovr_mag_scale", -1);
    if (ovrMagScale >= 100 && ovrMagScale <= 300) {
        s.magnificationScale = ovrMagScale;
    }
    int ovrBlur = Wh_GetIntValue(L"ovr_glass_blur", -1);
    if (ovrBlur >= 0 && ovrBlur <= 100) {
        s.glassBlur = ovrBlur;
    }
    int ovrTint = Wh_GetIntValue(L"ovr_glass_tint", -1);
    if (ovrTint >= 0 && ovrTint <= 100) {
        s.glassTint = ovrTint;
    }
    int ovrMinInto = Wh_GetIntValue(L"ovr_min_into_icon", -1);
    if (ovrMinInto == 0 || ovrMinInto == 1) {
        s.minimizeIntoAppIcon = (ovrMinInto == 1);
    }
    int ovrRecents = Wh_GetIntValue(L"ovr_show_recents", -1);
    if (ovrRecents == 0 || ovrRecents == 1) {
        s.showRecents = (ovrRecents == 1);
    }
    int ovrHideTb = Wh_GetIntValue(L"ovr_hide_taskbar", -1);
    if (ovrHideTb == 0 || ovrHideTb == 1) {
        s.hideTaskbar = (ovrHideTb == 1);
    }
}

void LoadSettings(bool clearOverrides = false) {
    if (clearOverrides) {
        ClearRuntimeOverrides();
    }

    DockSettings s;
    std::wstring pos = ReadWhStringSetting(L"position");
    if (pos == L"left") {
        s.position = DockPosition::Left;
    } else if (pos == L"right") {
        s.position = DockPosition::Right;
    } else {
        s.position = DockPosition::Bottom;
    }

    s.iconSize = std::clamp(Wh_GetIntSetting(L"iconSize"), 32, 128);
    s.magnification = Wh_GetIntSetting(L"magnification") != 0;
    int magScale = Wh_GetIntSetting(L"magnificationScale");
    if (magScale <= 0) {
        int legacyMagPx = Wh_GetIntSetting(L"magnifiedSize");
        if (legacyMagPx >= s.iconSize && s.iconSize > 0) {
            magScale = static_cast<int>(std::lround(
                (static_cast<double>(legacyMagPx) * 100.0) /
                static_cast<double>(s.iconSize)));
        } else {
            magScale = 180;
        }
    }
    s.magnificationScale = std::clamp(magScale, 100, 300);
    s.autoHide = Wh_GetIntSetting(L"autoHide") != 0;
    s.autoHideDelay = std::clamp(Wh_GetIntSetting(L"autoHideDelay"), 0, 2000);
    s.autoHideAnimation =
        std::clamp(Wh_GetIntSetting(L"autoHideAnimation"), 50, 1500);
    s.reserveSpace = Wh_GetIntSetting(L"reserveSpace") != 0;
    s.edgeGap = std::clamp(Wh_GetIntSetting(L"edgeGap"), 0, 32);

    std::wstring app = ReadWhStringSetting(L"appearance");
    if (app == L"light") {
        s.appearance = AppearanceMode::Light;
    } else if (app == L"dark") {
        s.appearance = AppearanceMode::Dark;
    } else {
        s.appearance = AppearanceMode::Auto;
    }

    std::wstring gs = ReadWhStringSetting(L"glassStyle");
    s.glassStyle = (gs == L"tinted") ? GlassStyle::Tinted : GlassStyle::Clear;

    std::wstring gr = ReadWhStringSetting(L"glassRefraction");
    if (gr == L"reflection") {
        s.glassRefraction = GlassRefraction::Reflection;
    } else if (gr == L"stretch") {
        s.glassRefraction = GlassRefraction::Stretch;
    } else if (gr == L"off") {
        s.glassRefraction = GlassRefraction::Off;
    } else {
        s.glassRefraction = GlassRefraction::Lens;
    }
    s.glassEdgeDepth = std::clamp(Wh_GetIntSetting(L"glassEdgeDepth"), 4, 48);
    s.glassEdgeStrength =
        std::clamp(Wh_GetIntSetting(L"glassEdgeStrength"), 0, 150);
    s.glassTint = std::clamp(Wh_GetIntSetting(L"glassTint"), 0, 100);
    s.glassBlur = std::clamp(Wh_GetIntSetting(L"glassBlur"), 0, 100);
    s.glassBrightnessLimit =
        std::clamp(Wh_GetIntSetting(L"glassBrightnessLimit"), 30, 100);
    std::wstring icStyle = ReadWhStringSetting(L"iconStyle");
    if (icStyle == L"dark") {
        s.iconStyle = DockIconStyle::Dark;
    } else if (icStyle == L"default" || icStyle == L"standard") {
        s.iconStyle = DockIconStyle::Default;
    } else if (icStyle == L"tinted") {
        s.iconStyle = DockIconStyle::Tinted;
    } else {
        s.iconStyle = DockIconStyle::Clear;
    }
    s.dockIconReflections = Wh_GetIntSetting(L"dockIconReflections") != 0;
    s.showWindowPreviews = Wh_GetIntSetting(L"showWindowPreviews") != 0;

    s.showIndicators = Wh_GetIntSetting(L"showIndicators") != 0;
    s.animateOpening = Wh_GetIntSetting(L"animateOpening") != 0;
    s.bounceForAttention = Wh_GetIntSetting(L"bounceForAttention") != 0;

    s.minimizeIntoAppIcon = Wh_GetIntSetting(L"minimizeIntoAppIcon") != 0;
    s.showRecents = Wh_GetIntSetting(L"showRecents") != 0;
    s.recentsCount = std::clamp(Wh_GetIntSetting(L"recentsCount"), 0, 12);
    s.showDownloads = Wh_GetIntSetting(L"showDownloads") != 0;
    s.downloadsFolder = ReadWhStringSetting(L"downloadsFolder");
    s.showFinder = Wh_GetIntSetting(L"showFinder") != 0;
    s.showAppsButton = Wh_GetIntSetting(L"showAppsButton") != 0;

    std::wstring aba = ReadWhStringSetting(L"appsButtonAction");
    if (aba == L"allApps") {
        s.appsButtonAction = AppsButtonAction::AllApps;
    } else {
        s.appsButtonAction = AppsButtonAction::Start;
    }

    s.customIconsFolder = ReadWhStringSetting(L"customIconsFolder");
    s.hideTaskbar = Wh_GetIntSetting(L"hideTaskbar") != 0;
    s.clickActiveToMinimize = Wh_GetIntSetting(L"clickActiveToMinimize") != 0;
    s.bringAllWindows = Wh_GetIntSetting(L"bringAllWindows") != 0;


    ApplyRuntimeOverrides(s);
    g_settings = std::move(s);
}

// ============================================================================
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
        [](wuc::CompositionGraphicsDevice const&,
           wuc::RenderingDeviceReplacedEventArgs const&) {
            if (!g_settingRenderingDevice.load(std::memory_order_acquire)) {
                PostDeviceRedrawMessage();
            }
        });
    return true;
}

inline bool IsDeviceLostHresult(HRESULT hr) {
    return hr == DXGI_ERROR_DEVICE_REMOVED ||
           hr == DXGI_ERROR_DEVICE_RESET ||
           hr == D2DERR_RECREATE_TARGET;
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
        if (IsDeviceLostHresult(hr)) {
            PostDeviceLostMessage();
        }
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
    if (IsDeviceLostHresult(endHr)) {
        PostDeviceLostMessage();
    }
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
constexpr GUID kFolderIdDownloads = {
    0x374de290, 0x123f, 0x4565, {0x91, 0x64, 0x39, 0xc4, 0x92, 0x5e, 0x46, 0x7b}};

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

extern std::atomic<bool> g_quitting;
HWND GetGlassNotifyHwnd();
bool IsDockHoverAnimating();
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
            bool keep = false;
            bool copiedFrame = false;
            bool lost = false;
            {
                struct FrameGuard {
                    IDXGIOutputDuplication* dup;
                    ~FrameGuard() { dup->ReleaseFrame(); }
                } guard{w->duplication.get()};

                do {
                    keep =
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
                Sleep(copiedFrame ? (hi ? 16u : 50u) : (hi ? 16u : 33u));
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

    std::array<float, 4> geomKey{};
    winrt::com_ptr<ID2D1PathGeometry> cachedClipGeom;
    winrt::com_ptr<ID2D1PathGeometry> cachedOuterGeom;
    winrt::com_ptr<ID2D1PathGeometry> cachedRimGeom;

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
        geomKey = {};
        cachedClipGeom = nullptr;
        cachedOuterGeom = nullptr;
        cachedRimGeom = nullptr;
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
                              ID2D1Geometry* clipGeom,
                              ID2D1Geometry* cachedOuterGeom = nullptr,
                              ID2D1Geometry* cachedRimGeom = nullptr) {
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
        (g_settings.glassStyle == GlassStyle::Tinted) ||
        (g_settings.iconStyle == DockIconStyle::Tinted);
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
    if (p.reflections && !p.reflections->empty() &&
        g_settings.dockIconReflections) {
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
    {
        ID2D1Geometry* outerG = cachedOuterGeom;
        winrt::com_ptr<ID2D1PathGeometry> tmpOuter;
        if (!outerG) {
            tmpOuter = makeInsetGeom(0.5f);
            outerG = tmpOuter.get();
        }
        if (outerG) {
            winrt::com_ptr<ID2D1SolidColorBrush> darkEdge;
            if (SUCCEEDED(dc->CreateSolidColorBrush(
                    D2D1::ColorF(0.0f, 0.0f, 0.0f, dark ? 0.45f : 0.14f),
                    darkEdge.put()))) {
                dc->DrawGeometry(outerG, darkEdge.get(), 1.0f);
            }
        }
    }

    // 6. Signature macOS 27 Clear/Dark Razor-Sharp Continuous Silvery Crystal Rim
    float rimW = std::max(1.0f, 1.15f * s);
    float rimInset = 0.9f + rimW * 0.5f;
    {
        ID2D1Geometry* rimG = cachedRimGeom;
        winrt::com_ptr<ID2D1PathGeometry> tmpRim;
        if (!rimG) {
            tmpRim = makeInsetGeom(rimInset);
            rimG = tmpRim.get();
        }
        if (rimG) {
            float rimR = 0.86f, rimGCol = 0.92f, rimB = 1.0f;
            if (tintedWash) {
                auto acc = GetAccentColor();
                rimR = Lerp(rimR, acc.R / 255.0f, 0.45f);
                rimGCol = Lerp(rimGCol, acc.G / 255.0f, 0.45f);
                rimB = Lerp(rimB, acc.B / 255.0f, 0.45f);
            }
            // Unbroken delicate silvery-white (or Tahoe tinted) glass outline all the way around
            winrt::com_ptr<ID2D1SolidColorBrush> baseRim;
            if (SUCCEEDED(dc->CreateSolidColorBrush(
                    D2D1::ColorF(rimR, rimGCol, rimB, dark ? 0.30f : 0.48f),
                    baseRim.put()))) {
                dc->DrawGeometry(rimG, baseRim.get(), rimW);
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
                dc->DrawGeometry(rimG, rbr.get(), rimW);
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
                dc->DrawGeometry(rimG, vbr.get(), rimW * 0.9f);
            }
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
    mix(static_cast<uint32_t>(g_settings.iconStyle));
    mix(static_cast<uint32_t>(g_settings.glassRefraction));
    mix(static_cast<uint32_t>(g_settings.glassEdgeDepth));
    mix(static_cast<uint32_t>(g_settings.glassEdgeStrength));
    mix(static_cast<uint32_t>(g_settings.glassTint));
    mix(static_cast<uint32_t>(g_settings.glassBlur));
    mix(static_cast<uint32_t>(g_settings.glassBrightnessLimit));
    mix(g_settings.dockIconReflections ? 1u : 0u);
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

        float qW = p.widthPx * invScaleX;
        if (IsDockHoverAnimating() && !p.fullWidthBar) {
            qW = std::round(qW * 0.5f) * 2.0f;
        }
        std::array<float, 11> mKey{
            static_cast<float>(mapW),
            static_cast<float>(mapH),
            std::round((static_cast<float>(pad) + fx) * invScaleX),
            std::round((static_cast<float>(pad) + fy) * invScaleY),
            qW,
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

        ID2D1Geometry* clipG = nullptr;
        ID2D1Geometry* outerG = nullptr;
        ID2D1Geometry* rimG = nullptr;
        bool pushedLayer = false;
        if (p.useSquircle && !p.fullWidthBar && p.cornerRadiusPx > 0.5f) {
            std::array<float, 4> gKey{p.widthPx, p.heightPx, p.cornerRadiusPx,
                                      p.scale};
            if (!plate.cachedClipGeom || gKey != plate.geomKey) {
                auto makeInset = [&](float inset) {
                    float iw = std::max(1.0f, p.widthPx - 2.0f * inset);
                    float ih = std::max(1.0f, p.heightPx - 2.0f * inset);
                    float ir =
                        std::max(0.5f, p.cornerRadiusPx - inset * 0.65f);
                    return BuildContinuousSquirclePath(inset, inset, iw, ih, ir);
                };
                plate.cachedClipGeom = BuildContinuousSquirclePath(
                    0.0f, 0.0f, p.widthPx, p.heightPx, p.cornerRadiusPx);
                plate.cachedOuterGeom = makeInset(0.5f);
                float rimW = std::max(1.0f, 1.15f * p.scale);
                float rimInset = 0.9f + rimW * 0.5f;
                plate.cachedRimGeom = makeInset(rimInset);
                plate.geomKey = gKey;
            }
            clipG = plate.cachedClipGeom.get();
            outerG = plate.cachedOuterGeom.get();
            rimG = plate.cachedRimGeom.get();
            if (clipG) {
                dc->PushLayer(
                    D2D1::LayerParameters(bounds, clipG,
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

        DrawMacOS27GlassLighting(dc.get(), p, clipG, outerG, rimG);

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

std::wstring GetEffectiveDownloadsPath() {
    if (!g_settings.downloadsFolder.empty()) {
        return g_settings.downloadsFolder;
    }
    PWSTR path = nullptr;
    std::wstring result;
    if (SUCCEEDED(
            SHGetKnownFolderPath(kFolderIdDownloads, 0, nullptr, &path)) &&
        path) {
        result = path;
        CoTaskMemFree(path);
    }
    return result;
}

std::wstring GetExplorerExePath() {
    WCHAR winDir[MAX_PATH]{};
    if (GetWindowsDirectoryW(winDir, ARRAYSIZE(winDir))) {
        return std::wstring(winDir) + L"\\explorer.exe";
    }
    return L"C:\\Windows\\explorer.exe";
}

winrt::com_ptr<IWICBitmapSource> ConvertWicToPbgra(IWICBitmapSource* src,
                                                   UINT maxSide = 0) {
    if (!src || !g_gfx.wicFactory) {
        return nullptr;
    }
    UINT w = 0, h = 0;
    if (FAILED(src->GetSize(&w, &h)) || w == 0 || h == 0) {
        return nullptr;
    }

    winrt::com_ptr<IWICBitmapSource> scaled;
    IWICBitmapSource* stepSrc = src;
    if (maxSide > 0 && (w > maxSide || h > maxSide)) {
        UINT dstW = w;
        UINT dstH = h;
        if (w >= h) {
            dstW = maxSide;
            dstH = std::max(1u, static_cast<UINT>(std::lround(
                                    static_cast<double>(h) * maxSide / w)));
        } else {
            dstH = maxSide;
            dstW = std::max(1u, static_cast<UINT>(std::lround(
                                    static_cast<double>(w) * maxSide / h)));
        }
        winrt::com_ptr<IWICBitmapScaler> scaler;
        if (SUCCEEDED(g_gfx.wicFactory->CreateBitmapScaler(scaler.put())) &&
            SUCCEEDED(scaler->Initialize(src, dstW, dstH,
                                         WICBitmapInterpolationModeFant))) {
            scaled = scaler;
            stepSrc = scaled.get();
        }
    }

    winrt::com_ptr<IWICFormatConverter> conv;
    if (FAILED(g_gfx.wicFactory->CreateFormatConverter(conv.put()))) {
        return nullptr;
    }
    if (FAILED(conv->Initialize(stepSrc, GUID_WICPixelFormat32bppPBGRA,
                                WICBitmapDitherTypeNone, nullptr, 0.0f,
                                WICBitmapPaletteTypeCustom))) {
        return nullptr;
    }

    // Materialize in RAM so decoded pixels never depend on stack buffers/streams.
    winrt::com_ptr<IWICBitmap> cachedBmp;
    if (SUCCEEDED(g_gfx.wicFactory->CreateBitmapFromSource(
            conv.get(), WICBitmapCacheOnLoad, cachedBmp.put())) &&
        cachedBmp) {
        winrt::com_ptr<IWICBitmapSource> out;
        cachedBmp->QueryInterface(IID_PPV_ARGS(out.put()));
        return out;
    }
    return nullptr;
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
    return ConvertWicToPbgra(frame.get(), 512);
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
    std::wstring accessPath = NativeSystemPath(path);
    winrt::com_ptr<IShellItem> item;
    if (SUCCEEDED(SHCreateItemFromParsingName(accessPath.c_str(), nullptr,
                                              IID_PPV_ARGS(item.put())))) {
        if (auto bmp = LoadShellItemIcon(item.get(), sizePx)) {
            return bmp;
        }
    }
    SHFILEINFOW sfi{};
    if (SHGetFileInfoW(accessPath.c_str(), 0, &sfi, sizeof(sfi),
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
bool IsFullBleedWicBitmap(IWICBitmapSource* src) {
    if (!src) {
        return false;
    }
    UINT w = 0, h = 0;
    if (FAILED(src->GetSize(&w, &h)) || w < 8 || h < 8) {
        return false;
    }
    const UINT stride = w * 4;
    std::vector<BYTE> pixels(static_cast<size_t>(stride) * h);
    if (FAILED(src->CopyPixels(nullptr, stride,
                               static_cast<UINT>(pixels.size()),
                               pixels.data()))) {
        return false;
    }
    auto alphaAt = [&](float fx, float fy) -> BYTE {
        UINT x = std::min(w - 1, static_cast<UINT>(fx * (w - 1)));
        UINT y = std::min(h - 1, static_cast<UINT>(fy * (h - 1)));
        return pixels[static_cast<size_t>(y) * stride + x * 4 + 3];
    };
    const std::array<std::pair<float, float>, 8> pts = {{
        {0.12f, 0.12f},
        {0.88f, 0.12f},
        {0.12f, 0.88f},
        {0.88f, 0.88f},
        {0.50f, 0.04f},
        {0.50f, 0.96f},
        {0.04f, 0.50f},
        {0.96f, 0.50f},
    }};
    int opaqueCount = 0;
    for (auto [fx, fy] : pts) {
        if (alphaAt(fx, fy) > 200) {
            ++opaqueCount;
        }
    }
    return opaqueCount >= 7;
}

void DrawSquircleShadow(ID2D1DeviceContext* dc,
                        ID2D1PathGeometry* squircle,
                        float tilePx,
                        float scale) {
    winrt::com_ptr<ID2D1BitmapRenderTarget> rt;
    if (FAILED(dc->CreateCompatibleRenderTarget(
            D2D1::SizeF(tilePx, tilePx), rt.put()))) {
        return;
    }
    rt->BeginDraw();
    rt->Clear(D2D1::ColorF(0, 0, 0, 0));
    winrt::com_ptr<ID2D1SolidColorBrush> solid;
    rt->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f), solid.put());
    rt->FillGeometry(squircle, solid.get());
    rt->EndDraw();

    winrt::com_ptr<ID2D1Bitmap> maskBmp;
    if (FAILED(rt->GetBitmap(maskBmp.put()))) {
        return;
    }
    winrt::com_ptr<ID2D1Effect> shadow;
    if (FAILED(dc->CreateEffect(CLSID_D2D1Shadow, shadow.put()))) {
        return;
    }
    shadow->SetInput(0, maskBmp.get());
    shadow->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, 2.0f * scale);
    shadow->SetValue(D2D1_SHADOW_PROP_COLOR,
                     D2D1::Vector4F(0.0f, 0.0f, 0.0f, 0.18f));
    shadow->SetValue(D2D1_SHADOW_PROP_OPTIMIZATION, static_cast<UINT32>(1));
    dc->DrawImage(shadow.get(), D2D1::Point2F(0.0f, 1.0f * scale));
}

void DrawClearDarkIconGlassRim(ID2D1DeviceContext* dc,
                               const D2D1_RECT_F& bodyRect,
                               float scale,
                               bool strongRim) {
    float bw = bodyRect.right - bodyRect.left;
    float bh = bodyRect.bottom - bodyRect.top;
    float rimW = std::max(0.9f, (strongRim ? 1.15f : 0.85f) * scale);
    float inset = rimW * 0.5f;
    D2D1_RECT_F rr{bodyRect.left + inset, bodyRect.top + inset,
                   bodyRect.right - inset, bodyRect.bottom - inset};
    auto rimGeom = BuildSquircle(rr);
    if (!rimGeom) {
        return;
    }
    D2D1_GRADIENT_STOP stops[4] = {
        {0.00f, D2D1::ColorF(1.00f, 1.00f, 1.00f, strongRim ? 0.62f : 0.24f)},
        {0.35f, D2D1::ColorF(0.86f, 0.92f, 1.00f, strongRim ? 0.22f : 0.08f)},
        {0.70f, D2D1::ColorF(0.84f, 0.90f, 1.00f, strongRim ? 0.16f : 0.06f)},
        {1.00f, D2D1::ColorF(0.90f, 0.96f, 1.00f, strongRim ? 0.48f : 0.18f)},
    };
    winrt::com_ptr<ID2D1GradientStopCollection> coll;
    winrt::com_ptr<ID2D1LinearGradientBrush> grad;
    if (SUCCEEDED(dc->CreateGradientStopCollection(stops, 4, coll.put())) &&
        SUCCEEDED(dc->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(
                D2D1::Point2F(bodyRect.left, bodyRect.top),
                D2D1::Point2F(bodyRect.left + bw, bodyRect.top + bh)),
            coll.get(), grad.put()))) {
        dc->DrawGeometry(rimGeom.get(), grad.get(), rimW);
    }
}

void DrawClearDarkIconPlate(ID2D1DeviceContext* dc,
                            ID2D1PathGeometry* squircle,
                            const D2D1_RECT_F& bodyRect) {
    D2D1_GRADIENT_STOP stops[3] = {
        {0.00f, D2D1::ColorF(0.16f, 0.18f, 0.23f, 0.68f)},
        {0.52f, D2D1::ColorF(0.08f, 0.10f, 0.14f, 0.72f)},
        {1.00f, D2D1::ColorF(0.12f, 0.14f, 0.20f, 0.66f)},
    };
    winrt::com_ptr<ID2D1GradientStopCollection> stopColl;
    if (SUCCEEDED(dc->CreateGradientStopCollection(stops, 3, stopColl.put()))) {
        winrt::com_ptr<ID2D1LinearGradientBrush> grad;
        dc->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(
                D2D1::Point2F(bodyRect.left, bodyRect.top),
                D2D1::Point2F(bodyRect.left, bodyRect.bottom)),
            stopColl.get(), grad.put());
        dc->FillGeometry(squircle, grad.get());
    }
    // Delicate top glass catchlight inside icon squircle
    D2D1_GRADIENT_STOP hl[2] = {
        {0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.14f)},
        {1.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f)},
    };
    winrt::com_ptr<ID2D1GradientStopCollection> hlColl;
    winrt::com_ptr<ID2D1LinearGradientBrush> hlGrad;
    if (SUCCEEDED(dc->CreateGradientStopCollection(hl, 2, hlColl.put())) &&
        SUCCEEDED(dc->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(
                D2D1::Point2F(bodyRect.left, bodyRect.top),
                D2D1::Point2F(bodyRect.left,
                              bodyRect.top +
                                  (bodyRect.bottom - bodyRect.top) * 0.38f)),
            hlColl.get(), hlGrad.put()))) {
        dc->FillGeometry(squircle, hlGrad.get());
    }
}

void DrawTahoeIconPlate(ID2D1DeviceContext* dc,
                        ID2D1PathGeometry* squircle,
                        const D2D1_RECT_F& bodyRect,
                        DockIconStyle style,
                        bool darkMode) {
    if (style == DockIconStyle::Clear && darkMode) {
        DrawClearDarkIconPlate(dc, squircle, bodyRect);
        return;
    }

    D2D1_COLOR_F topCol{}, midCol{}, botCol{};
    if (style == DockIconStyle::Clear) {
        // Light Clear frosted crystal glass
        topCol = D2D1::ColorF(0.95f, 0.97f, 1.00f, 0.72f);
        midCol = D2D1::ColorF(0.86f, 0.90f, 0.96f, 0.62f);
        botCol = D2D1::ColorF(0.90f, 0.94f, 0.99f, 0.68f);
    } else if (style == DockIconStyle::Dark) {
        // Tahoe Dark mode: deep smoked charcoal-obsidian plate for vivid color glyphs
        topCol = D2D1::ColorF(0.19f, 0.21f, 0.25f, 0.96f);
        midCol = D2D1::ColorF(0.11f, 0.12f, 0.16f, 0.97f);
        botCol = D2D1::ColorF(0.08f, 0.09f, 0.12f, 0.98f);
    } else if (style == DockIconStyle::Tinted) {
        auto acc = GetAccentColor();
        float ar = acc.R / 255.0f;
        float ag = acc.G / 255.0f;
        float ab = acc.B / 255.0f;
        if (darkMode) {
            topCol = D2D1::ColorF(Lerp(0.16f, ar, 0.36f),
                                  Lerp(0.18f, ag, 0.36f),
                                  Lerp(0.22f, ab, 0.36f), 0.78f);
            midCol = D2D1::ColorF(Lerp(0.08f, ar, 0.26f),
                                  Lerp(0.10f, ag, 0.26f),
                                  Lerp(0.13f, ab, 0.26f), 0.84f);
            botCol = D2D1::ColorF(Lerp(0.12f, ar, 0.32f),
                                  Lerp(0.14f, ag, 0.32f),
                                  Lerp(0.18f, ab, 0.32f), 0.80f);
        } else {
            topCol = D2D1::ColorF(Lerp(0.94f, ar, 0.28f),
                                  Lerp(0.95f, ag, 0.28f),
                                  Lerp(0.98f, ab, 0.28f), 0.82f);
            midCol = D2D1::ColorF(Lerp(0.84f, ar, 0.32f),
                                  Lerp(0.86f, ag, 0.32f),
                                  Lerp(0.92f, ab, 0.32f), 0.78f);
            botCol = D2D1::ColorF(Lerp(0.88f, ar, 0.30f),
                                  Lerp(0.90f, ag, 0.30f),
                                  Lerp(0.95f, ab, 0.30f), 0.80f);
        }
    } else {
        // Default adaptive light/dark backing
        topCol = darkMode ? D2D1::ColorF(0x363840, 1.0f)
                          : D2D1::ColorF(0xFFFFFF, 1.0f);
        midCol = darkMode ? D2D1::ColorF(0x2B2D34, 1.0f)
                          : D2D1::ColorF(0xF5F6FA, 1.0f);
        botCol = darkMode ? D2D1::ColorF(0x22242A, 1.0f)
                          : D2D1::ColorF(0xECECF2, 1.0f);
    }

    D2D1_GRADIENT_STOP stops[3] = {
        {0.0f, topCol},
        {0.5f, midCol},
        {1.0f, botCol},
    };
    winrt::com_ptr<ID2D1GradientStopCollection> coll;
    winrt::com_ptr<ID2D1LinearGradientBrush> grad;
    if (SUCCEEDED(dc->CreateGradientStopCollection(stops, 3, coll.put())) &&
        SUCCEEDED(dc->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(
                D2D1::Point2F(bodyRect.left, bodyRect.top),
                D2D1::Point2F(bodyRect.left, bodyRect.bottom)),
            coll.get(), grad.put()))) {
        dc->FillGeometry(squircle, grad.get());
    }

    // Top specular glass catchlight
    D2D1_GRADIENT_STOP hl[2] = {
        {0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, darkMode ? 0.14f : 0.28f)},
        {1.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f)},
    };
    winrt::com_ptr<ID2D1GradientStopCollection> hlColl;
    winrt::com_ptr<ID2D1LinearGradientBrush> hlGrad;
    if (SUCCEEDED(dc->CreateGradientStopCollection(hl, 2, hlColl.put())) &&
        SUCCEEDED(dc->CreateLinearGradientBrush(
            D2D1::LinearGradientBrushProperties(
                D2D1::Point2F(bodyRect.left, bodyRect.top),
                D2D1::Point2F(bodyRect.left,
                              bodyRect.top +
                                  (bodyRect.bottom - bodyRect.top) * 0.36f)),
            hlColl.get(), hlGrad.put()))) {
        dc->FillGeometry(squircle, hlGrad.get());
    }
}

void RenderAppsButtonIcon(ID2D1DeviceContext* dc,
                          float tilePx,
                          float scale,
                          DockIconStyle style,
                          bool darkMode) {
    const float bodySize = tilePx * 0.84f;
    const float inset = (tilePx - bodySize) * 0.5f;
    D2D1_RECT_F bodyRect =
        D2D1::RectF(inset, inset, inset + bodySize, inset + bodySize);
    auto squircle = BuildSquircle(bodyRect);
    if (!squircle) {
        return;
    }

    DrawSquircleShadow(dc, squircle.get(), tilePx, scale);
    DrawTahoeIconPlate(dc, squircle.get(), bodyRect,
                       (style == DockIconStyle::Default) ? DockIconStyle::Dark
                                                         : style,
                       darkMode);

    const uint32_t colors[9] = {
        0xFF3B30, 0xFF9500, 0xFFCC00, 0x34C759, 0x00C7BE,
        0x32ADE6, 0x007AFF, 0x5856D6, 0xAF52DE,
    };
    const float gridSpan = bodySize * 0.58f;
    const float gap = gridSpan * 0.13f;
    const float cell = (gridSpan - 2.0f * gap) / 3.0f;
    const float startX = inset + (bodySize - gridSpan) * 0.5f;
    const float startY = inset + (bodySize - gridSpan) * 0.5f;
    const float cellRadius = cell * 0.28f;

    auto acc = GetAccentColor();
    float ar = acc.R / 255.0f;
    float ag = acc.G / 255.0f;
    float ab = acc.B / 255.0f;

    winrt::com_ptr<ID2D1SolidColorBrush> cellBrush;
    dc->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1, 1), cellBrush.put());
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            float x = startX + c * (cell + gap);
            float y = startY + r * (cell + gap);
            if (style == DockIconStyle::Clear) {
                float lum = darkMode ? (0.72f + 0.08f * static_cast<float>(2 - r))
                                     : (0.22f + 0.08f * static_cast<float>(r));
                cellBrush->SetColor(
                    D2D1::ColorF(lum * 0.95f, lum * 0.98f, lum, 0.88f));
            } else if (style == DockIconStyle::Tinted) {
                float boost = 0.70f + 0.10f * static_cast<float>(2 - r);
                cellBrush->SetColor(D2D1::ColorF(
                    std::min(1.0f, Lerp(ar, 1.0f, 0.35f) * boost),
                    std::min(1.0f, Lerp(ag, 1.0f, 0.35f) * boost),
                    std::min(1.0f, Lerp(ab, 1.0f, 0.35f) * boost), 0.92f));
            } else {
                cellBrush->SetColor(D2D1::ColorF(colors[r * 3 + c], 1.0f));
            }
            dc->FillRoundedRectangle(
                D2D1::RoundedRect(D2D1::RectF(x, y, x + cell, y + cell),
                                  cellRadius, cellRadius),
                cellBrush.get());
        }
    }
    DrawClearDarkIconGlassRim(dc, bodyRect, scale,
                              style == DockIconStyle::Clear ||
                                  style == DockIconStyle::Tinted);
}

// macOS 26 Tahoe Layered Paper-Sheet Folder / Stack Icon
void RenderTahoeFolderIcon(ID2D1DeviceContext* dc,
                           float tilePx,
                           float scale,
                           DockIconStyle style,
                           bool darkMode,
                           bool isDownloads) {
    const float w = tilePx * 0.86f;
    const float h = tilePx * 0.72f;
    const float x0 = (tilePx - w) * 0.5f;
    const float y0 = tilePx * 0.18f;
    const float r = w * 0.14f;

    auto acc = GetAccentColor();
    float ar = acc.R / 255.0f;
    float ag = acc.G / 255.0f;
    float ab = acc.B / 255.0f;

    D2D1_COLOR_F backTop{}, backBot{}, frontTop{}, frontBot{}, sheetCol{},
        rimCol{};
    if (style == DockIconStyle::Clear) {
        if (darkMode) {
            backTop = D2D1::ColorF(0.24f, 0.28f, 0.35f, 0.78f);
            backBot = D2D1::ColorF(0.14f, 0.17f, 0.23f, 0.82f);
            frontTop = D2D1::ColorF(0.32f, 0.37f, 0.45f, 0.84f);
            frontBot = D2D1::ColorF(0.16f, 0.20f, 0.27f, 0.90f);
            sheetCol = D2D1::ColorF(0.86f, 0.91f, 0.98f, 0.85f);
            rimCol = D2D1::ColorF(0.92f, 0.96f, 1.0f, 0.54f);
        } else {
            backTop = D2D1::ColorF(0.76f, 0.82f, 0.90f, 0.76f);
            backBot = D2D1::ColorF(0.66f, 0.72f, 0.82f, 0.80f);
            frontTop = D2D1::ColorF(0.88f, 0.92f, 0.98f, 0.84f);
            frontBot = D2D1::ColorF(0.76f, 0.82f, 0.91f, 0.88f);
            sheetCol = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.92f);
            rimCol = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.75f);
        }
    } else if (style == DockIconStyle::Tinted) {
        backTop = D2D1::ColorF(ar * 0.65f, ag * 0.65f, ab * 0.65f, 0.88f);
        backBot = D2D1::ColorF(ar * 0.42f, ag * 0.42f, ab * 0.42f, 0.92f);
        frontTop = D2D1::ColorF(Lerp(ar, 1.0f, 0.22f), Lerp(ag, 1.0f, 0.22f),
                                Lerp(ab, 1.0f, 0.22f), 0.90f);
        frontBot = D2D1::ColorF(ar * 0.72f, ag * 0.72f, ab * 0.72f, 0.94f);
        sheetCol = D2D1::ColorF(Lerp(ar, 1.0f, 0.72f), Lerp(ag, 1.0f, 0.72f),
                                Lerp(ab, 1.0f, 0.72f), 0.90f);
        rimCol = D2D1::ColorF(Lerp(ar, 1.0f, 0.55f), Lerp(ag, 1.0f, 0.55f),
                              Lerp(ab, 1.0f, 0.55f), 0.62f);
    } else if (style == DockIconStyle::Dark) {
        backTop = D2D1::ColorF(0.18f, 0.34f, 0.56f, 0.96f);
        backBot = D2D1::ColorF(0.11f, 0.22f, 0.39f, 0.98f);
        frontTop = D2D1::ColorF(0.24f, 0.48f, 0.78f, 0.95f);
        frontBot = D2D1::ColorF(0.14f, 0.32f, 0.58f, 0.98f);
        sheetCol = D2D1::ColorF(0.88f, 0.93f, 0.98f, 0.92f);
        rimCol = D2D1::ColorF(0.75f, 0.88f, 1.0f, 0.45f);
    } else {
        // Default macOS Tahoe azure glass folder
        backTop = D2D1::ColorF(0.28f, 0.65f, 0.97f, 0.96f);
        backBot = D2D1::ColorF(0.18f, 0.50f, 0.88f, 0.98f);
        frontTop = D2D1::ColorF(0.45f, 0.79f, 1.00f, 0.96f);
        frontBot = D2D1::ColorF(0.22f, 0.58f, 0.94f, 0.98f);
        sheetCol = D2D1::ColorF(0.98f, 0.99f, 1.0f, 0.95f);
        rimCol = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.55f);
    }

    winrt::com_ptr<ID2D1SolidColorBrush> br;
    dc->CreateSolidColorBrush(backTop, br.put());
    if (!br) return;

    // 1. Folder back tab + back body
    D2D1_RECT_F tabRc{x0, y0, x0 + w * 0.42f, y0 + h * 0.32f};
    dc->FillRoundedRectangle(D2D1::RoundedRect(tabRc, r * 0.65f, r * 0.65f),
                             br.get());
    D2D1_RECT_F backRc{x0, y0 + h * 0.11f, x0 + w, y0 + h};
    {
        D2D1_GRADIENT_STOP st[2] = {{0.0f, backTop}, {1.0f, backBot}};
        winrt::com_ptr<ID2D1GradientStopCollection> coll;
        winrt::com_ptr<ID2D1LinearGradientBrush> lg;
        if (SUCCEEDED(dc->CreateGradientStopCollection(st, 2, coll.put())) &&
            SUCCEEDED(dc->CreateLinearGradientBrush(
                D2D1::LinearGradientBrushProperties(
                    D2D1::Point2F(x0, backRc.top),
                    D2D1::Point2F(x0, backRc.bottom)),
                coll.get(), lg.put()))) {
            dc->FillRoundedRectangle(D2D1::RoundedRect(backRc, r, r), lg.get());
        }
    }

    // 2. Tahoe signature peeking layered paper sheets inside folder
    br->SetColor(D2D1::ColorF(sheetCol.r * 0.88f, sheetCol.g * 0.90f,
                              sheetCol.b * 0.94f, sheetCol.a * 0.76f));
    D2D1_RECT_F p1{x0 + w * 0.12f, y0 + h * 0.04f, x0 + w * 0.88f,
                   y0 + h * 0.58f};
    dc->FillRoundedRectangle(D2D1::RoundedRect(p1, r * 0.45f, r * 0.45f),
                             br.get());
    br->SetColor(sheetCol);
    D2D1_RECT_F p2{x0 + w * 0.08f, y0 + h * 0.14f, x0 + w * 0.92f,
                   y0 + h * 0.66f};
    dc->FillRoundedRectangle(D2D1::RoundedRect(p2, r * 0.45f, r * 0.45f),
                             br.get());

    // 3. Front frosted Liquid Glass folder flap
    D2D1_RECT_F frontRc{x0, y0 + h * 0.24f, x0 + w, y0 + h};
    {
        D2D1_GRADIENT_STOP st[2] = {{0.0f, frontTop}, {1.0f, frontBot}};
        winrt::com_ptr<ID2D1GradientStopCollection> coll;
        winrt::com_ptr<ID2D1LinearGradientBrush> lg;
        if (SUCCEEDED(dc->CreateGradientStopCollection(st, 2, coll.put())) &&
            SUCCEEDED(dc->CreateLinearGradientBrush(
                D2D1::LinearGradientBrushProperties(
                    D2D1::Point2F(x0, frontRc.top),
                    D2D1::Point2F(x0, frontRc.bottom)),
                coll.get(), lg.put()))) {
            dc->FillRoundedRectangle(D2D1::RoundedRect(frontRc, r, r), lg.get());
        }
    }

    // Subtle down-arrow badge for Downloads folder
    if (isDownloads) {
        float cx = (frontRc.left + frontRc.right) * 0.5f;
        float cy = (frontRc.top + frontRc.bottom) * 0.52f;
        float arSz = h * 0.22f;
        br->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f,
                                  (style == DockIconStyle::Clear && !darkMode)
                                      ? 0.72f
                                      : 0.78f));
        float sw = std::max(1.5f, tilePx * 0.042f);
        dc->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), arSz * 1.15f,
                                      arSz * 1.15f),
                        br.get(), sw * 0.85f);
        dc->DrawLine(D2D1::Point2F(cx, cy - arSz * 0.55f),
                     D2D1::Point2F(cx, cy + arSz * 0.48f), br.get(), sw);
        dc->DrawLine(D2D1::Point2F(cx - arSz * 0.44f, cy + arSz * 0.06f),
                     D2D1::Point2F(cx, cy + arSz * 0.52f), br.get(), sw);
        dc->DrawLine(D2D1::Point2F(cx + arSz * 0.44f, cy + arSz * 0.06f),
                     D2D1::Point2F(cx, cy + arSz * 0.52f), br.get(), sw);
    }

    // 4. Crisp specular glass rim around front flap
    br->SetColor(rimCol);
    dc->DrawRoundedRectangle(D2D1::RoundedRect(frontRc, r, r), br.get(),
                             std::max(1.0f, 1.1f * scale));
}

// Live Right-Zone Minimized Window Tile (macOS 26 Tahoe)
wuc::CompositionDrawingSurface RenderMinimizedWindowTile(
    IWICBitmapSource* windowSnap,
    IWICBitmapSource* appBadgeIcon,
    float tilePx,
    float scale,
    bool darkMode) {
    auto surface = CreateDrawingSurface(tilePx, tilePx);
    if (!surface) {
        return nullptr;
    }
    DrawCompositionSurface(surface, [&](ID2D1DeviceContext* dc) {
        const float w = tilePx * 0.88f;
        const float h = tilePx * 0.68f;
        const float x0 = (tilePx - w) * 0.5f;
        const float y0 = (tilePx - h) * 0.52f;
        const float r = std::max(4.0f * scale, tilePx * 0.11f);
        D2D1_RECT_F cardRc{x0, y0, x0 + w, y0 + h};
        D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(cardRc, r, r);

        winrt::com_ptr<ID2D1SolidColorBrush> br;
        dc->CreateSolidColorBrush(
            darkMode ? D2D1::ColorF(0.11f, 0.13f, 0.18f, 0.92f)
                     : D2D1::ColorF(0.94f, 0.96f, 0.99f, 0.94f),
            br.put());
        if (!br) return;

        // Soft drop shadow
        D2D1_RECT_F shRc{x0, y0 + 2.0f * scale, x0 + w, y0 + h + 2.0f * scale};
        br->SetColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.34f));
        dc->FillRoundedRectangle(D2D1::RoundedRect(shRc, r, r), br.get());

        // Glass miniature window card background
        br->SetColor(darkMode ? D2D1::ColorF(0.12f, 0.14f, 0.19f, 0.92f)
                              : D2D1::ColorF(0.95f, 0.96f, 0.99f, 0.94f));
        dc->FillRoundedRectangle(rr, br.get());

        if (windowSnap) {
            winrt::com_ptr<ID2D1Bitmap> bmp;
            if (SUCCEEDED(dc->CreateBitmapFromWicBitmap(windowSnap, nullptr,
                                                        bmp.put())) &&
                bmp) {
                winrt::com_ptr<ID2D1RoundedRectangleGeometry> clipG;
                if (SUCCEEDED(g_gfx.d2dFactory->CreateRoundedRectangleGeometry(
                        rr, clipG.put()))) {
                    winrt::com_ptr<ID2D1Layer> layer;
                    if (SUCCEEDED(dc->CreateLayer(layer.put()))) {
                        dc->PushLayer(D2D1::LayerParameters(
                                          cardRc, clipG.get(),
                                          D2D1_ANTIALIAS_MODE_PER_PRIMITIVE),
                                      layer.get());
                        dc->DrawBitmap(bmp.get(), cardRc, 1.0f,
                                       D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
                        dc->PopLayer();
                    }
                }
            }
        } else {
            // Clean macOS mini-window chrome with traffic lights + centered app icon
            float dotR = std::max(1.5f, w * 0.045f);
            float dotY = y0 + h * 0.16f;
            br->SetColor(D2D1::ColorF(0xFF5F57, 0.95f));
            dc->FillEllipse(D2D1::Ellipse({x0 + w * 0.12f, dotY}, dotR, dotR),
                            br.get());
            br->SetColor(D2D1::ColorF(0xFEBC2E, 0.95f));
            dc->FillEllipse(D2D1::Ellipse({x0 + w * 0.23f, dotY}, dotR, dotR),
                            br.get());
            br->SetColor(D2D1::ColorF(0x28C840, 0.95f));
            dc->FillEllipse(D2D1::Ellipse({x0 + w * 0.34f, dotY}, dotR, dotR),
                            br.get());
            if (appBadgeIcon) {
                winrt::com_ptr<ID2D1Bitmap> bmp;
                if (SUCCEEDED(dc->CreateBitmapFromWicBitmap(
                        appBadgeIcon, nullptr, bmp.put())) &&
                    bmp) {
                    float sz = h * 0.50f;
                    float ix = (cardRc.left + cardRc.right - sz) * 0.5f;
                    float iy = y0 + h * 0.34f;
                    dc->DrawBitmap(bmp.get(),
                                   D2D1::RectF(ix, iy, ix + sz, iy + sz), 0.94f,
                                   D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
                }
            }
        }

        // Specular frame border
        br->SetColor(darkMode ? D2D1::ColorF(0.92f, 0.96f, 1.0f, 0.45f)
                              : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.78f));
        dc->DrawRoundedRectangle(rr, br.get(), std::max(1.0f, 1.05f * scale));

        // Parent app badge anchored cleanly at bottom-right
        if (appBadgeIcon) {
            winrt::com_ptr<ID2D1Bitmap> bmp;
            if (SUCCEEDED(dc->CreateBitmapFromWicBitmap(appBadgeIcon, nullptr,
                                                        bmp.put())) &&
                bmp) {
                float bSz = tilePx * 0.34f;
                float bx = cardRc.right - bSz * 0.85f;
                float by = cardRc.bottom - bSz * 0.80f;
                dc->DrawBitmap(bmp.get(),
                               D2D1::RectF(bx, by, bx + bSz, by + bSz), 1.0f,
                               D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
            }
        }
    });
    return surface;
}

wuc::CompositionDrawingSurface RenderStyledIconSurface(
    IWICBitmapSource* sourceWic,
    bool isCustomOverride,
    bool applyMacPlate,
    bool isProceduralApps,
    bool isTahoeFolder,
    bool isDownloadsFolder,
    float tilePx,
    float scale,
    bool darkMode) {
    auto surface = CreateDrawingSurface(tilePx, tilePx);
    if (!surface) {
        return nullptr;
    }

    const DockIconStyle style =
        isCustomOverride ? DockIconStyle::Default : g_settings.iconStyle;
    const bool fullBleed =
        (!isCustomOverride && applyMacPlate && sourceWic)
            ? IsFullBleedWicBitmap(sourceWic)
            : false;

    bool drewContent = false;
    bool drawOk = DrawCompositionSurface(surface, [&](ID2D1DeviceContext* dc) {
        if (isProceduralApps && !sourceWic) {
            RenderAppsButtonIcon(dc, tilePx, scale, style, darkMode);
            drewContent = true;
            return;
        }
        if (isTahoeFolder && !isCustomOverride) {
            RenderTahoeFolderIcon(dc, tilePx, scale, style, darkMode,
                                  isDownloadsFolder);
            drewContent = true;
            return;
        }
        if (!sourceWic) {
            return;
        }

        winrt::com_ptr<ID2D1Bitmap> d2dBmp;
        if (FAILED(dc->CreateBitmapFromWicBitmap(sourceWic, nullptr,
                                                 d2dBmp.put())) ||
            !d2dBmp) {
            return;
        }

        auto drawFilteredGlyph = [&](const D2D1_RECT_F& dstRect,
                                     bool tintedMode) {
            winrt::com_ptr<ID2D1Effect> cmFx;
            if (SUCCEEDED(dc->CreateEffect(kClsidD2D1ColorMatrix,
                                           cmFx.put()))) {
                cmFx->SetInput(0, d2dBmp.get(), TRUE);
                D2D1_MATRIX_5X4_F m{};
                constexpr float rW = 0.299f, gW = 0.587f, bW = 0.114f;
                if (tintedMode) {
                    auto acc = GetAccentColor();
                    float ar = Lerp(acc.R / 255.0f, 1.0f, 0.34f);
                    float ag = Lerp(acc.G / 255.0f, 1.0f, 0.34f);
                    float ab = Lerp(acc.B / 255.0f, 1.0f, 0.34f);
                    constexpr float g = 0.92f;
                    m.m[0][0] = rW * g * ar;
                    m.m[0][1] = rW * g * ag;
                    m.m[0][2] = rW * g * ab;
                    m.m[1][0] = gW * g * ar;
                    m.m[1][1] = gW * g * ag;
                    m.m[1][2] = gW * g * ab;
                    m.m[2][0] = bW * g * ar;
                    m.m[2][1] = bW * g * ag;
                    m.m[2][2] = bW * g * ab;
                    m.m[3][3] = 0.96f;
                    m.m[4][0] = ar * (darkMode ? 0.22f : 0.08f);
                    m.m[4][1] = ag * (darkMode ? 0.22f : 0.08f);
                    m.m[4][2] = ab * (darkMode ? 0.22f : 0.08f);
                } else if (darkMode) {
                    // Clear/Dark: luminous cool-silver monochrome glass glyph
                    constexpr float g = 0.88f;
                    m.m[0][0] = rW * g * 0.96f;
                    m.m[0][1] = rW * g * 0.98f;
                    m.m[0][2] = rW * g * 1.04f;
                    m.m[1][0] = gW * g * 0.96f;
                    m.m[1][1] = gW * g * 0.98f;
                    m.m[1][2] = gW * g * 1.04f;
                    m.m[2][0] = bW * g * 0.96f;
                    m.m[2][1] = bW * g * 0.98f;
                    m.m[2][2] = bW * g * 1.04f;
                    m.m[3][3] = 0.96f;
                    m.m[4][0] = 0.24f;
                    m.m[4][1] = 0.26f;
                    m.m[4][2] = 0.30f;
                } else {
                    // Clear/Light: crisp frosted dark-graphite glass glyph
                    constexpr float g = 0.55f;
                    m.m[0][0] = rW * g * 0.94f;
                    m.m[0][1] = rW * g * 0.96f;
                    m.m[0][2] = rW * g * 1.02f;
                    m.m[1][0] = gW * g * 0.94f;
                    m.m[1][1] = gW * g * 0.96f;
                    m.m[1][2] = gW * g * 1.02f;
                    m.m[2][0] = bW * g * 0.94f;
                    m.m[2][1] = bW * g * 0.96f;
                    m.m[2][2] = bW * g * 1.02f;
                    m.m[3][3] = 0.92f;
                    m.m[4][0] = 0.06f;
                    m.m[4][1] = 0.08f;
                    m.m[4][2] = 0.12f;
                }
                SetD2DEffectVal(cmFx.get(), 0, m);
                SetD2DEffectVal(cmFx.get(), 2, static_cast<BOOL>(TRUE));

                auto bsz = d2dBmp->GetSize();
                if (bsz.width > 0.0f && bsz.height > 0.0f) {
                    D2D1_MATRIX_3X2_F prev{};
                    dc->GetTransform(&prev);
                    float sx = (dstRect.right - dstRect.left) / bsz.width;
                    float sy = (dstRect.bottom - dstRect.top) / bsz.height;
                    dc->SetTransform(
                        D2D1::Matrix3x2F::Scale(sx, sy) *
                        D2D1::Matrix3x2F::Translation(dstRect.left,
                                                      dstRect.top) *
                        prev);
                    auto cmOut = GetD2DEffectOut(cmFx.get());
                    dc->SetTransform(prev);
                    if (cmOut) {
                        D2D1_POINT_2F origin{0.0f, 0.0f};
                        dc->SetTransform(
                            D2D1::Matrix3x2F::Scale(sx, sy) *
                            D2D1::Matrix3x2F::Translation(dstRect.left,
                                                          dstRect.top) *
                            prev);
                        dc->DrawImage(cmOut.get(), &origin, nullptr,
                                      D2D1_INTERPOLATION_MODE_CUBIC,
                                      D2D1_COMPOSITE_MODE_SOURCE_OVER);
                        dc->SetTransform(prev);
                        return;
                    }
                }
            }
            dc->DrawBitmap(d2dBmp.get(), dstRect, 0.92f,
                           D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
        };

        if (isCustomOverride || !applyMacPlate) {
            const float pad = isCustomOverride ? tilePx * 0.04f : tilePx * 0.08f;
            D2D1_RECT_F dst =
                D2D1::RectF(pad, pad, tilePx - pad, tilePx - pad);
            dc->DrawBitmap(d2dBmp.get(), dst, 1.0f,
                           D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
            drewContent = true;
            return;
        }

        const float bodySize = tilePx * 0.84f;
        const float inset = (tilePx - bodySize) * 0.5f;
        D2D1_RECT_F bodyRect =
            D2D1::RectF(inset, inset, inset + bodySize, inset + bodySize);
        auto squircle = BuildSquircle(bodyRect);
        if (!squircle) {
            dc->DrawBitmap(d2dBmp.get(), bodyRect, 1.0f,
                           D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
            drewContent = true;
            return;
        }

        DrawSquircleShadow(dc, squircle.get(), tilePx, scale);

        if (style == DockIconStyle::Clear || style == DockIconStyle::Tinted) {
            DrawTahoeIconPlate(dc, squircle.get(), bodyRect, style, darkMode);
            const float span = fullBleed ? (bodySize * 0.86f)
                                         : (bodySize * 0.68f);
            const float off = (tilePx - span) * 0.5f;
            D2D1_RECT_F glyphDst =
                D2D1::RectF(off, off, off + span, off + span);
            winrt::com_ptr<ID2D1Layer> layer;
            if (SUCCEEDED(dc->CreateLayer(layer.put()))) {
                dc->PushLayer(
                    D2D1::LayerParameters(bodyRect, squircle.get(),
                                          D2D1_ANTIALIAS_MODE_PER_PRIMITIVE),
                    layer.get());
                drawFilteredGlyph(glyphDst, style == DockIconStyle::Tinted);
                dc->PopLayer();
            } else {
                drawFilteredGlyph(glyphDst, style == DockIconStyle::Tinted);
            }
            drewContent = true;
            DrawClearDarkIconGlassRim(dc, bodyRect, scale, true);
            return;
        }

        if (style == DockIconStyle::Dark) {
            // Tahoe Dark mode: smoked dark squircle + vivid full-color glyph
            DrawTahoeIconPlate(dc, squircle.get(), bodyRect,
                               DockIconStyle::Dark, true);
            const float span =
                fullBleed ? (bodySize * 0.90f) : (bodySize * 0.68f);
            const float off = (tilePx - span) * 0.5f;
            D2D1_RECT_F glyphDst =
                D2D1::RectF(off, off, off + span, off + span);
            winrt::com_ptr<ID2D1Layer> layer;
            if (SUCCEEDED(dc->CreateLayer(layer.put()))) {
                dc->PushLayer(
                    D2D1::LayerParameters(bodyRect, squircle.get(),
                                          D2D1_ANTIALIAS_MODE_PER_PRIMITIVE),
                    layer.get());
                dc->DrawBitmap(d2dBmp.get(), glyphDst, 0.96f,
                               D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
                dc->PopLayer();
            } else {
                dc->DrawBitmap(d2dBmp.get(), glyphDst, 0.96f,
                               D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
            }
            drewContent = true;
            DrawClearDarkIconGlassRim(dc, bodyRect, scale, true);
            return;
        }

        // Default full-color Tahoe icon style
        if (fullBleed) {
            winrt::com_ptr<ID2D1Layer> layer;
            if (SUCCEEDED(dc->CreateLayer(layer.put()))) {
                dc->PushLayer(
                    D2D1::LayerParameters(bodyRect, squircle.get(),
                                          D2D1_ANTIALIAS_MODE_PER_PRIMITIVE),
                    layer.get());
                dc->DrawBitmap(d2dBmp.get(), bodyRect, 1.0f,
                               D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
                dc->PopLayer();
            } else {
                dc->DrawBitmap(d2dBmp.get(), bodyRect, 1.0f,
                               D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
            }
            drewContent = true;
            DrawClearDarkIconGlassRim(dc, bodyRect, scale, false);
        } else {
            DrawTahoeIconPlate(dc, squircle.get(), bodyRect,
                               DockIconStyle::Default, darkMode);
            const float iconSpan = bodySize * 0.70f;
            const float iconOff = (tilePx - iconSpan) * 0.5f;
            D2D1_RECT_F iconDst = D2D1::RectF(
                iconOff, iconOff, iconOff + iconSpan, iconOff + iconSpan);
            dc->DrawBitmap(d2dBmp.get(), iconDst, 1.0f,
                           D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC);
            drewContent = true;
            DrawClearDarkIconGlassRim(dc, bodyRect, scale, false);
        }
    });
    if (!drawOk || !drewContent) {
        return nullptr;
    }
    return surface;
}

// ============================================================================
// 7. App Model & Icon Cache
// ============================================================================

enum class DockItemKind {
    Finder,
    Apps,
    App,
    Divider,
    Recent,
    Downloads,
    FolderStack,
    Trash,
    MinimizedWindow,
};

struct PinnedFolderStack {
    std::wstring path;
    std::wstring name;
    int viewMode = 0;  // 0 = Fan, 1 = Grid, 2 = List
    int sortMode = 0;  // 0 = Date Modified, 1 = Name, 2 = Kind
};

std::vector<PinnedFolderStack> g_pinnedFolders;
std::unordered_map<HWND, winrt::com_ptr<IWICBitmapSource>> g_minWindowThumbs;

struct DockItem {
    DockItemKind kind = DockItemKind::App;
    std::wstring aumid;
    std::wstring path;
    std::wstring shortcutPath;
    std::wstring taskbarAppId;
    std::vector<BYTE> taskbarPidl;
    std::wstring args;
    std::wstring workdir;
    std::wstring name;
    std::wstring identityKey;

    bool pinned = false;
    std::vector<HWND> windows;
    HWND lastActive = nullptr;
    HWND minimizedHwnd = nullptr;

    bool launching = false;
    double launchStart = 0.0;
    bool attention = false;
    double attentionStart = 0.0;
    bool pressed = false;
    bool focused = false;
    bool allMinimized = false;

    // Animation state
    float presence = 1.0f;
    float presenceVel = 0.0f;
    float targetPresence = 1.0f;
    float currentScale = 1.0f;
    float bounceOffset = 0.0f;
    float focusAnim = 0.0f;

    // Layout rects (window-local pixels)
    D2D1_RECT_F iconRect{};
    float slotStart = 0.0f;
    float slotEnd = 0.0f;

    // Composition visuals
    wuc::ContainerVisual container{nullptr};
    wuc::SpriteVisual iconVisual{nullptr};
    wuc::CompositionSurfaceBrush iconBrush{nullptr};
    wuc::SpriteVisual pressVisual{nullptr};
    wuc::CompositionMaskBrush pressBrush{nullptr};
    wuc::ShapeVisual dotVisual{nullptr};
    wuc::CompositionRoundedRectangleGeometry dotGeom{nullptr};
    wuc::CompositionColorBrush dotBrush{nullptr};
    wuc::SpriteVisual dividerVisual{nullptr};
    wuc::CompositionColorBrush dividerBrush{nullptr};
    wuc::CompositionDrawingSurface boundSurface{nullptr};
};

std::vector<DockItem> g_items;
std::unordered_map<std::wstring, wuc::CompositionDrawingSurface> g_iconCache;
std::unordered_map<std::wstring, std::wstring> g_customIcons;  // key -> file path

void LoadCustomIcons() {
    g_customIcons.clear();
    for (const auto& line :
         LoadNumberedStringList(L"custom_icons_v2", L"custom_icons_v1")) {
        size_t sep = line.find(L'\x1F');
        if (sep != std::wstring::npos && sep > 0 && sep + 1 < line.size()) {
            g_customIcons[line.substr(0, sep)] = line.substr(sep + 1);
        }
    }
}

void SaveCustomIcons() {
    std::vector<std::wstring> entries;
    entries.reserve(g_customIcons.size());
    for (const auto& [k, v] : g_customIcons) {
        if (k.empty() || v.empty()) {
            continue;
        }
        entries.push_back(k + L"\x1F" + v);
    }
    SaveNumberedStringList(L"custom_icons_v2", entries);
}

std::wstring CustomIconStorageKey(const DockItem& item, bool trashFullSlot = false) {
    if (item.kind == DockItemKind::Trash) {
        return trashFullSlot ? L"fixed:trash_full" : L"fixed:trash";
    }
    if (item.kind == DockItemKind::Recent &&
        item.identityKey.rfind(L"recent:", 0) == 0) {
        return item.identityKey.substr(7);
    }
    return item.identityKey;
}

uint64_t Fnv1a64(std::wstring_view s) {
    uint64_t h = 14695981039346656037ULL;
    for (wchar_t ch : s) {
        h ^= static_cast<uint64_t>(static_cast<uint16_t>(ch));
        h *= 1099511628211ULL;
    }
    return h;
}

std::wstring GetManagedCustomIconsDir() {
    WCHAR storageBuf[MAX_PATH]{};
    size_t len = Wh_GetModStoragePath(storageBuf, ARRAYSIZE(storageBuf));
    if (len == 0 || !storageBuf[0]) {
        return {};
    }
    std::wstring dir(storageBuf);
    while (!dir.empty() && (dir.back() == L'\\' || dir.back() == L'/')) {
        dir.pop_back();
    }
    if (dir.empty()) {
        return {};
    }
    dir += L"\\custom-icons";
    int rc = SHCreateDirectoryExW(nullptr, dir.c_str(), nullptr);
    if (rc != ERROR_SUCCESS && rc != ERROR_ALREADY_EXISTS &&
        rc != ERROR_FILE_EXISTS) {
        return {};
    }
    return dir;
}

bool IsPathInsideDir(std::wstring_view filePath, std::wstring_view dirPath) {
    if (filePath.empty() || dirPath.empty()) {
        return false;
    }
    std::wstring normFile = NormalizePath(filePath);
    std::wstring normDir = NormalizePath(dirPath);
    if (normFile.size() <= normDir.size() + 1) {
        return false;
    }
    return normFile.rfind(normDir, 0) == 0 && normFile[normDir.size()] == L'\\';
}

void MaybeDeleteUnreferencedManagedIcon(const std::wstring& removedPath) {
    if (removedPath.empty()) {
        return;
    }
    std::wstring dir = GetManagedCustomIconsDir();
    if (!IsPathInsideDir(removedPath, dir)) {
        return;
    }
    std::wstring targetNorm = NormalizePath(removedPath);
    for (const auto& [k, v] : g_customIcons) {
        if (!v.empty() && NormalizePath(v) == targetNorm) {
            return;
        }
    }
    DeleteFileW(removedPath.c_str());
}

std::wstring PersistManagedCustomIcon(const std::wstring& storageKey,
                                      const std::wstring& sourcePath) {
    if (sourcePath.empty() || !g_gfx.wicFactory) {
        return {};
    }
    auto wic = LoadWicBitmapFromFile(sourcePath);
    if (!wic) {
        return {};
    }
    std::wstring dir = GetManagedCustomIconsDir();
    if (dir.empty()) {
        return sourcePath;
    }

    WCHAR hexBuf[32]{};
    swprintf_s(hexBuf, L"%016llx",
               static_cast<unsigned long long>(Fnv1a64(storageKey)));
    std::wstring finalPath = dir + L"\\" + hexBuf + L".png";
    std::wstring tmpPath = finalPath + L".tmp";

    UINT w = 0, h = 0;
    if (FAILED(wic->GetSize(&w, &h)) || w == 0 || h == 0) {
        return sourcePath;
    }

    bool encoded = false;
    {
        winrt::com_ptr<IWICFormatConverter> straightBgra;
        winrt::com_ptr<IWICStream> outStream;
        winrt::com_ptr<IWICBitmapEncoder> encoder;
        winrt::com_ptr<IWICBitmapFrameEncode> frame;
        WICPixelFormatGUID pf = GUID_WICPixelFormat32bppBGRA;
        if (SUCCEEDED(g_gfx.wicFactory->CreateFormatConverter(
                straightBgra.put())) &&
            SUCCEEDED(straightBgra->Initialize(
                wic.get(), GUID_WICPixelFormat32bppBGRA,
                WICBitmapDitherTypeNone, nullptr, 0.0f,
                WICBitmapPaletteTypeCustom)) &&
            SUCCEEDED(g_gfx.wicFactory->CreateStream(outStream.put())) &&
            SUCCEEDED(outStream->InitializeFromFilename(tmpPath.c_str(),
                                                        GENERIC_WRITE)) &&
            SUCCEEDED(g_gfx.wicFactory->CreateEncoder(
                GUID_ContainerFormatPng, nullptr, encoder.put())) &&
            SUCCEEDED(encoder->Initialize(outStream.get(),
                                          WICBitmapEncoderNoCache)) &&
            SUCCEEDED(encoder->CreateNewFrame(frame.put(), nullptr)) &&
            SUCCEEDED(frame->Initialize(nullptr)) &&
            SUCCEEDED(frame->SetSize(w, h)) &&
            SUCCEEDED(frame->SetPixelFormat(&pf)) &&
            SUCCEEDED(frame->WriteSource(straightBgra.get(), nullptr)) &&
            SUCCEEDED(frame->Commit()) &&
            SUCCEEDED(encoder->Commit())) {
            encoded = true;
        }
    }

    if (encoded &&
        MoveFileExW(tmpPath.c_str(), finalPath.c_str(),
                    MOVEFILE_REPLACE_EXISTING)) {
        return finalPath;
    }
    DeleteFileW(tmpPath.c_str());
    return sourcePath;
}

std::vector<std::wstring> CollectCustomIconLookupKeys(const DockItem& item) {
    std::vector<std::wstring> keys;
    auto addUnique = [&](std::wstring k) {
        if (k.empty()) {
            return;
        }
        if (std::find(keys.begin(), keys.end(), k) == keys.end()) {
            keys.push_back(std::move(k));
        }
    };
    addUnique(CustomIconStorageKey(item, false));
    if (item.kind == DockItemKind::App || item.kind == DockItemKind::Recent) {
        if (!item.aumid.empty()) {
            addUnique(L"aumid:" + ToLower(item.aumid));
        }
        if (!item.path.empty()) {
            addUnique(L"path:" + NormalizePath(item.path));
        }
    }
    return keys;
}

bool HasUserCustomIcon(const std::wstring& key) {
    auto it = g_customIcons.find(key);
    return it != g_customIcons.end() && !it->second.empty();
}

bool HasUserCustomIconForItem(const DockItem& item, bool trashFullSlot = false) {
    if (item.kind == DockItemKind::Trash && trashFullSlot) {
        if (HasUserCustomIcon(L"fixed:trash_full")) {
            return true;
        }
    }
    for (const auto& k : CollectCustomIconLookupKeys(item)) {
        if (HasUserCustomIcon(k)) {
            return true;
        }
    }
    return false;
}

void ClearIconCache() {
    g_iconCache.clear();
    for (auto& item : g_items) {
        item.boundSurface = nullptr;
    }
}

void InvalidateIconCacheForPrefixes(
    const std::initializer_list<std::wstring_view>& prefixes) {
    auto matches = [&](const std::wstring& key) {
        for (auto p : prefixes) {
            if (!p.empty() && key.rfind(p, 0) == 0) {
                return true;
            }
        }
        return false;
    };
    for (auto it = g_iconCache.begin(); it != g_iconCache.end();) {
        if (matches(it->first)) {
            it = g_iconCache.erase(it);
        } else {
            ++it;
        }
    }
    for (auto& item : g_items) {
        if (matches(item.identityKey)) {
            item.boundSurface = nullptr;
        }
    }
}

std::wstring SanitizeCustomIconStem(std::wstring_view raw) {
    std::wstring out;
    out.reserve(raw.size());
    for (wchar_t ch : raw) {
        if (ch == L'!') {
            out.push_back(L'_');
        } else {
            out.push_back(static_cast<wchar_t>(towlower(ch)));
        }
    }
    return out;
}

std::wstring GetCustomIconPathForItem(const DockItem& item, bool trashFull) {
    // 1. Per-app user-picked icon via right-click "Change Icon..."
    const std::wstring managedDir = GetManagedCustomIconsDir();
    auto resolveStoredPath = [&](const std::wstring& key) -> std::wstring {
        auto it = g_customIcons.find(key);
        if (it == g_customIcons.end() || it->second.empty()) {
            return {};
        }
        DWORD attr = GetFileAttributesW(it->second.c_str());
        if (attr == INVALID_FILE_ATTRIBUTES ||
            (attr & FILE_ATTRIBUTE_DIRECTORY)) {
            return {};
        }
        if (!managedDir.empty() && !IsPathInsideDir(it->second, managedDir)) {
            std::wstring migrated = PersistManagedCustomIcon(key, it->second);
            if (!migrated.empty() && migrated != it->second) {
                it->second = migrated;
                SaveCustomIcons();
            }
        }
        return it->second;
    };

    if (item.kind == DockItemKind::Trash && trashFull) {
        if (auto p = resolveStoredPath(L"fixed:trash_full"); !p.empty()) {
            return p;
        }
    }
    for (const auto& k : CollectCustomIconLookupKeys(item)) {
        if (auto p = resolveStoredPath(k); !p.empty()) {
            return p;
        }
    }

    // 2. Bulk customIconsFolder overrides (.png, .icns, .ico)
    if (g_settings.customIconsFolder.empty()) {
        return {};
    }
    std::wstring dir = g_settings.customIconsFolder;
    while (!dir.empty() && (dir.back() == L'\\' || dir.back() == L'/')) {
        dir.pop_back();
    }
    if (dir.empty()) {
        return {};
    }

    auto tryCandidate = [&](std::wstring_view stem) -> std::wstring {
        if (stem.empty()) {
            return {};
        }
        for (PCWSTR ext : {L".png", L".icns", L".ico"}) {
            std::wstring full = dir + L"\\" + std::wstring(stem) + ext;
            DWORD attr = GetFileAttributesW(full.c_str());
            if (attr != INVALID_FILE_ATTRIBUTES &&
                !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
                return full;
            }
        }
        return {};
    };

    switch (item.kind) {
        case DockItemKind::Finder:
            return tryCandidate(L"finder");
        case DockItemKind::Apps:
            return tryCandidate(L"apps");
        case DockItemKind::Downloads:
            return tryCandidate(L"downloads");
        case DockItemKind::Trash:
            if (trashFull) {
                if (auto p = tryCandidate(L"trash-full"); !p.empty()) {
                    return p;
                }
            }
            return tryCandidate(L"trash");
        case DockItemKind::App:
        case DockItemKind::Recent: {
            if (!item.path.empty()) {
                const wchar_t* fname = PathFindFileNameW(item.path.c_str());
                std::wstring stem = fname ? fname : L"";
                size_t dot = stem.find_last_of(L'.');
                if (dot != std::wstring::npos) {
                    stem.resize(dot);
                }
                if (auto p = tryCandidate(SanitizeCustomIconStem(stem));
                    !p.empty()) {
                    return p;
                }
            }
            if (!item.aumid.empty()) {
                if (auto p = tryCandidate(SanitizeCustomIconStem(item.aumid));
                    !p.empty()) {
                    return p;
                }
            }
            return {};
        }
        default:
            return {};
    }
}

// Dominant vibrant color cache for subtle MyDockFinder-style icon reflections on glass
std::unordered_map<std::wstring, D2D1_COLOR_F> g_iconColorCache;

D2D1_COLOR_F SampleDominantColorFromWic(IWICBitmapSource* src) {
    D2D1_COLOR_F fallback{0.42f, 0.68f, 1.0f, 1.0f};
    if (!src) {
        return fallback;
    }
    UINT w = 0, h = 0;
    if (FAILED(src->GetSize(&w, &h)) || w < 4 || h < 4) {
        return fallback;
    }
    const UINT stride = w * 4;
    std::vector<BYTE> px(static_cast<size_t>(stride) * h);
    if (FAILED(src->CopyPixels(nullptr, stride, static_cast<UINT>(px.size()),
                               px.data()))) {
        return fallback;
    }
    double sumR = 0.0, sumG = 0.0, sumB = 0.0, sumW = 0.0;
    UINT stepX = std::max(1u, w / 24u);
    UINT stepY = std::max(1u, h / 24u);
    for (UINT y = 0; y < h; y += stepY) {
        for (UINT x = 0; x < w; x += stepX) {
            const BYTE* p = &px[static_cast<size_t>(y) * stride + x * 4];
            float a = p[3] / 255.0f;
            if (a < 0.35f) {
                continue;
            }
            float b = std::min(1.0f, (p[0] / 255.0f) / a);
            float g = std::min(1.0f, (p[1] / 255.0f) / a);
            float r = std::min(1.0f, (p[2] / 255.0f) / a);
            float mx = std::max({r, g, b});
            float mn = std::min({r, g, b});
            float sat = (mx > 0.01f) ? ((mx - mn) / mx) : 0.0f;
            // Weight vibrant pixels higher so reflections show the app's signature color
            double weight = static_cast<double>(a) * (0.15 + 2.5 * sat * sat);
            sumR += r * weight;
            sumG += g * weight;
            sumB += b * weight;
            sumW += weight;
        }
    }
    if (sumW < 0.1) {
        return fallback;
    }
    return D2D1::ColorF(
        std::clamp(static_cast<float>(sumR / sumW), 0.12f, 1.0f),
        std::clamp(static_cast<float>(sumG / sumW), 0.12f, 1.0f),
        std::clamp(static_cast<float>(sumB / sumW), 0.12f, 1.0f), 1.0f);
}

D2D1_COLOR_F GetIconDominantColor(const std::wstring& identityKey) {
    if (auto it = g_iconColorCache.find(identityKey);
        it != g_iconColorCache.end()) {
        return it->second;
    }
    return D2D1::ColorF(0.45f, 0.70f, 1.0f, 1.0f);
}

wuc::CompositionDrawingSurface GetOrRenderItemIcon(const DockItem& item,
                                                   float renderPx,
                                                   float scale,
                                                   bool darkMode) {
    if (item.kind == DockItemKind::Divider) {
        return nullptr;
    }
    const bool trashFull =
        (item.kind == DockItemKind::Trash) ? IsRecycleBinNonEmpty() : false;
    int sizeInt = static_cast<int>(std::ceil(renderPx));

    // Live Minimized Window tile on the Dock right zone
    if (item.kind == DockItemKind::MinimizedWindow) {
        std::wstring mkey = item.identityKey + L"|" + std::to_wstring(sizeInt) +
                            L"|" + (darkMode ? L"D" : L"L");
        if (auto it = g_iconCache.find(mkey); it != g_iconCache.end()) {
            return it->second;
        }
        winrt::com_ptr<IWICBitmapSource> snap;
        if (auto tit = g_minWindowThumbs.find(item.minimizedHwnd);
            tit != g_minWindowThumbs.end()) {
            snap = tit->second;
        }
        winrt::com_ptr<IWICBitmapSource> badge;
        if (!item.path.empty()) {
            badge = LoadIconFromPath(item.path, 64);
        }
        if (!badge && !item.aumid.empty()) {
            badge = LoadIconFromAumid(item.aumid, 64);
        }
        if (!badge && item.minimizedHwnd) {
            badge = LoadIconFromWindow(item.minimizedHwnd);
        }
        auto surf = RenderMinimizedWindowTile(snap.get(), badge.get(),
                                              static_cast<float>(sizeInt),
                                              scale, darkMode);
        if (surf) {
            g_iconCache.insert_or_assign(mkey, surf);
        }
        return surf;
    }

    const wchar_t* styleCode = L"C";
    switch (g_settings.iconStyle) {
        case DockIconStyle::Clear: styleCode = L"C"; break;
        case DockIconStyle::Dark: styleCode = L"K"; break;
        case DockIconStyle::Default: styleCode = L"S"; break;
        case DockIconStyle::Tinted: styleCode = L"T"; break;
    }

    std::wstring key = item.identityKey + L"|" + std::to_wstring(sizeInt) +
                       L"|" + (darkMode ? L"D" : L"L") + L"|" + styleCode +
                       (trashFull ? L"|F" : L"");
    if (auto it = g_iconCache.find(key); it != g_iconCache.end()) {
        return it->second;
    }

    winrt::com_ptr<IWICBitmapSource> src;
    bool isCustom = false;
    if (auto customPath = GetCustomIconPathForItem(item, trashFull);
        !customPath.empty()) {
        src = LoadWicBitmapFromFile(customPath);
        isCustom = (src != nullptr);
    }

    bool applyMacPlate = false;
    bool isProceduralApps = false;
    bool isTahoeFolder = false;
    bool isDownloadsFolder = false;

    auto loadDefaultSource = [&]() {
        applyMacPlate = false;
        isProceduralApps = false;
        isTahoeFolder = false;
        isDownloadsFolder = false;
        src = nullptr;
        switch (item.kind) {
            case DockItemKind::Finder:
                applyMacPlate = true;
                src = LoadIconFromPath(GetExplorerExePath(), 256);
                break;
            case DockItemKind::Apps:
                applyMacPlate = true;
                isProceduralApps = true;
                break;
            case DockItemKind::Downloads:
                isTahoeFolder = true;
                isDownloadsFolder = true;
                break;
            case DockItemKind::FolderStack:
                isTahoeFolder = true;
                isDownloadsFolder = false;
                break;
            case DockItemKind::Trash:
                applyMacPlate = false;
                src = LoadIconFromPath(
                    L"shell:::{645FF040-5081-101B-9F08-00AA002F954E}", 256);
                break;
            case DockItemKind::App:
            case DockItemKind::Recent:
                applyMacPlate = true;
                if (!item.shortcutPath.empty()) {
                    src = LoadIconFromPath(item.shortcutPath, 256);
                }
                if (!src && !item.path.empty()) {
                    src = LoadIconFromPath(item.path, 256);
                }
                if (!src && !item.aumid.empty()) {
                    src = LoadIconFromAumid(item.aumid, 256);
                }
                if (!src && item.lastActive) {
                    src = LoadIconFromWindow(item.lastActive);
                }
                if (!src && !item.windows.empty()) {
                    src = LoadIconFromWindow(item.windows.front());
                }
                break;
            default:
                break;
        }
    };

    if (!isCustom) {
        loadDefaultSource();
    }

    if (src) {
        g_iconColorCache[item.identityKey] =
            SampleDominantColorFromWic(src.get());
    } else if (isProceduralApps || isTahoeFolder) {
        g_iconColorCache[item.identityKey] =
            D2D1::ColorF(0.45f, 0.72f, 1.0f, 1.0f);
    }

    auto surface = RenderStyledIconSurface(
        src.get(), isCustom, applyMacPlate, isProceduralApps, isTahoeFolder,
        isDownloadsFolder, static_cast<float>(sizeInt), scale, darkMode);
    if (!surface && isCustom) {
        isCustom = false;
        loadDefaultSource();
        surface = RenderStyledIconSurface(
            src.get(), false, applyMacPlate, isProceduralApps, isTahoeFolder,
            isDownloadsFolder, static_cast<float>(sizeInt), scale, darkMode);
    }
    if (surface) {
        g_iconCache.insert_or_assign(key, surface);
    }
    return surface;
}

// ============================================================================
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
    std::unordered_map<DWORD, std::wstring> pidExeCache;
    std::vector<HWINEVENTHOOK> hooks;
    UINT shellHookMsg = 0;
    bool shellHookRegistered = false;
    bool dirty = false;
    bool refreshScheduled = false;
    double lastRefreshTime = 0.0;
    HWND foregroundHwnd = nullptr;
};

WindowTrackerState g_tracker;
extern std::atomic<bool> g_quitting;

void ScheduleWindowRefresh();
void ScheduleFullscreenCheck();
void OnWindowAttentionFlash(HWND hwnd);
void OnWindowForegroundChanged(HWND hwnd);

std::wstring GetFileDescriptionFromPath(const std::wstring& path) {
    if (path.empty()) {
        return {};
    }
    std::wstring accessPath = NativeSystemPath(path);
    DWORD dummy = 0;
    DWORD sz = GetFileVersionInfoSizeW(accessPath.c_str(), &dummy);
    if (sz > 0) {
        std::vector<BYTE> buf(sz);
        if (GetFileVersionInfoW(accessPath.c_str(), 0, sz, buf.data())) {
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
    if (auto it = g_tracker.pidExeCache.find(pid);
        it != g_tracker.pidExeCache.end()) {
        return it->second;
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
        if (g_tracker.pidExeCache.size() > 256) {
            g_tracker.pidExeCache.clear();
        }
        g_tracker.pidExeCache.emplace(pid, res);
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

bool IsTransientShellHostExe(std::wstring_view exePath) {
    if (exePath.empty()) {
        return false;
    }
    size_t sep = exePath.find_last_of(L"\\/");
    std::wstring base = ToLower(
        sep == std::wstring_view::npos ? exePath : exePath.substr(sep + 1));
    static constexpr std::wstring_view kTransientHosts[] = {
        L"pickerhost.exe",
        L"openwith.exe",
        L"credentialuibroker.exe",
        L"consent.exe",
        L"wwahost.exe",
        L"lockapp.exe",
        L"searchhost.exe",
        L"startmenuexperiencehost.exe",
        L"shellexperiencehost.exe",
        L"textinputhost.exe",
    };
    for (auto h : kTransientHosts) {
        if (base == h) {
            return true;
        }
    }
    return false;
}

WindowIdentity ResolveWindowIdentity(HWND hwnd) {
    if (auto it = g_tracker.identityCache.find(hwnd);
        it != g_tracker.identityCache.end()) {
        return it->second;
    }

    WindowIdentity id;
    WCHAR cls[96]{};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));

    if (_wcsicmp(cls, L"CabinetWClass") == 0 ||
        _wcsicmp(cls, L"ExploreWClass") == 0) {
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

bool IsEligibleAppWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd)) {
        return false;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId()) {
        return false;
    }

    WCHAR cls[96]{};
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        return false;
    }
    if (_wcsicmp(cls, L"Progman") == 0 || _wcsicmp(cls, L"WorkerW") == 0 ||
        _wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
        _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
        _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
        return false;
    }

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    bool isAppWnd = (exStyle & WS_EX_APPWINDOW) != 0;
    if (!isAppWnd) {
        if (GetWindow(hwnd, GW_OWNER) != nullptr) {
            return false;
        }
        if ((exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) != 0) {
            return false;
        }
    }

    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) &&
        cloaked != 0) {
        bool allowCloaked = false;
        if (cloaked == DWM_CLOAKED_SHELL && g_tracker.vdm) {
            BOOL onCurrent = TRUE;
            if (SUCCEEDED(g_tracker.vdm->IsWindowOnCurrentVirtualDesktop(
                    hwnd, &onCurrent)) &&
                !onCurrent) {
                allowCloaked = true;
            }
        } else if (cloaked == DWM_CLOAKED_APP) {
            if (g_tracker.identityCache.contains(hwnd)) {
                allowCloaked = true;
            } else {
                for (const auto& it : g_items) {
                    if (it.minimizedHwnd == hwnd ||
                        std::find(it.windows.begin(), it.windows.end(), hwnd) !=
                            it.windows.end()) {
                        allowCloaked = true;
                        break;
                    }
                }
            }
        }
        if (!allowCloaked) {
            return false;
        }
    }

    DWORD targetPid = pid;
    // Skip cloaked-equivalent empty ApplicationFrameHost shells with no CoreWindow yet.
    if (_wcsicmp(cls, L"ApplicationFrameWindow") == 0) {
        DWORD childPid = FindCoreWindowChildPid(hwnd, pid);
        if (childPid == 0 && GetWindowPropAumid(hwnd).empty()) {
            return false;
        }
        if (childPid != 0) {
            targetPid = childPid;
        }
    }

    if (targetPid) {
        std::wstring exe = GetProcessExePath(targetPid);
        if (!exe.empty() && IsTransientShellHostExe(exe)) {
            return false;
        }
    }

    return true;
}

struct EnumTrackedWindowsCtx {
    std::vector<TrackedWindow>* out;
    HWND fg;
};

BOOL CALLBACK EnumTrackedWindowsProc(HWND hwnd, LPARAM lp) {
    auto* c = reinterpret_cast<EnumTrackedWindowsCtx*>(lp);
    if (!IsEligibleAppWindow(hwnd)) {
        return TRUE;
    }
    TrackedWindow tw;
    tw.hwnd = hwnd;
    tw.id = ResolveWindowIdentity(hwnd);
    tw.minimized = IsIconic(hwnd) != FALSE;
    tw.active = (hwnd == c->fg);
    if (!tw.id.isExplorerCabinet && tw.id.aumid.empty() &&
        tw.id.normPath.empty()) {
        return TRUE;
    }
    c->out->push_back(std::move(tw));
    return TRUE;
}

HWND ResolveActiveForegroundWindow() {
    HWND fg = GetForegroundWindow();
    if (fg && IsWindow(fg) && !IsIconic(fg)) {
        DWORD pid = 0;
        GetWindowThreadProcessId(fg, &pid);
        if (pid != GetCurrentProcessId()) {
            return fg;
        }
    }
    if (g_tracker.foregroundHwnd && IsWindow(g_tracker.foregroundHwnd) &&
        !IsIconic(g_tracker.foregroundHwnd)) {
        DWORD pid = 0;
        GetWindowThreadProcessId(g_tracker.foregroundHwnd, &pid);
        if (pid != GetCurrentProcessId()) {
            return g_tracker.foregroundHwnd;
        }
    }
    return nullptr;
}

std::vector<TrackedWindow> EnumerateTrackedWindows() {
    g_tracker.pidExeCache.clear();
    std::vector<TrackedWindow> list;
    HWND fg = ResolveActiveForegroundWindow();
    EnumTrackedWindowsCtx ctx{&list, fg};
    EnumWindows(EnumTrackedWindowsProc, reinterpret_cast<LPARAM>(&ctx));
    return list;
}

winrt::com_ptr<IWICBitmapSource> CaptureWindowSnapshot(HWND hwnd,
                                                       const RECT& rc) {
    if (!hwnd || !IsWindow(hwnd) || IsHungAppWindow(hwnd)) {
        return nullptr;
    }
    int w = rc.right - rc.left;
    int h = rc.bottom - rc.top;
    if (w <= 8 || h <= 8 || w > 4096 || h > 4096) {
        return nullptr;
    }
    HDC scr = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(scr);
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bmp = CreateDIBSection(mem ? mem : scr, &bmi, DIB_RGB_COLORS, &bits,
                                   nullptr, 0);
    if (!mem || !bmp || !bits) {
        if (bmp) DeleteObject(bmp);
        if (mem) DeleteDC(mem);
        ReleaseDC(nullptr, scr);
        return nullptr;
    }
    HGDIOBJ old = SelectObject(mem, bmp);
    BOOL ok = PrintWindow(hwnd, mem, PW_RENDERFULLCONTENT);
    if (!ok && !IsIconic(hwnd)) {
        ok = BitBlt(mem, 0, 0, w, h, scr, rc.left, rc.top, SRCCOPY);
    }
    GdiFlush();
    SelectObject(mem, old);
    winrt::com_ptr<IWICBitmapSource> wic;
    if (ok) {
        wic = LoadWicBitmapFromHBitmap(bmp);
    }
    DeleteObject(bmp);
    DeleteDC(mem);
    ReleaseDC(nullptr, scr);
    return wic;
}

void CaptureMinimizedThumbnailIfNeeded(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd) || g_settings.minimizeIntoAppIcon) {
        return;
    }
    RECT rc{};
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rc,
                                     sizeof(rc)))) {
        if (!GetWindowRect(hwnd, &rc)) {
            return;
        }
    }
    if (rc.left <= -10000 || rc.top <= -10000 ||
        (rc.right - rc.left) < 40 || (rc.bottom - rc.top) < 40) {
        return;
    }
    if (auto snap = CaptureWindowSnapshot(hwnd, rc)) {
        g_minWindowThumbs[hwnd] = std::move(snap);
    }
}

bool IsWindowPreviewFlyoutOpen();
void RefreshWindowPreviewFlyoutIfOpen();

void CALLBACK WindowTrackerEventProc(HWINEVENTHOOK,
                                     DWORD event,
                                     HWND hwnd,
                                     LONG idObject,
                                     LONG idChild,
                                     DWORD,
                                     DWORD) {
    if (g_quitting) {
        return;
    }
    if (event == EVENT_SYSTEM_FOREGROUND) {
        if (hwnd && IsWindow(hwnd)) {
            DWORD pid = 0;
            GetWindowThreadProcessId(hwnd, &pid);
            if (pid != GetCurrentProcessId()) {
                g_tracker.foregroundHwnd = hwnd;
            }
        } else {
            g_tracker.foregroundHwnd = nullptr;
        }
        OnWindowForegroundChanged(hwnd);
        ScheduleFullscreenCheck();
        return;
    }
    if (event == EVENT_SYSTEM_MINIMIZESTART ||
        event == EVENT_SYSTEM_MINIMIZEEND) {
        if (event == EVENT_SYSTEM_MINIMIZESTART && hwnd) {
            CaptureMinimizedThumbnailIfNeeded(hwnd);
        }
        ScheduleWindowRefresh();
        ScheduleFullscreenCheck();
        return;
    }
    if (event == EVENT_SYSTEM_MOVESIZEEND) {
        ScheduleFullscreenCheck();
        return;
    }

    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !hwnd) {
        return;
    }
    if (event == EVENT_OBJECT_DESTROY) {
        g_tracker.identityCache.erase(hwnd);
        ScheduleWindowRefresh();
        return;
    }
    if (GetAncestor(hwnd, GA_ROOT) != hwnd) {
        return;
    }
    if (event == EVENT_OBJECT_NAMECHANGE) {
        if (IsWindowPreviewFlyoutOpen()) {
            RefreshWindowPreviewFlyoutIfOpen();
        }
        return;
    }
    if (event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_HIDE ||
        event == kEventObjectCloaked || event == kEventObjectUncloaked) {
        ScheduleWindowRefresh();
        if (event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_HIDE) {
            ScheduleFullscreenCheck();
        }
    }
}

void StartWindowTracker(HWND dockHwnd) {
    CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_INPROC_SERVER,
                     IID_PPV_ARGS(g_tracker.vdm.put()));

    const std::array<std::pair<DWORD, DWORD>, 6> ranges = {{
        {EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND},
        {EVENT_SYSTEM_MOVESIZEEND, EVENT_SYSTEM_MOVESIZEEND},
        {EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND},
        {EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE},
        {EVENT_OBJECT_NAMECHANGE, EVENT_OBJECT_NAMECHANGE},
        {kEventObjectCloaked, kEventObjectUncloaked},
    }};
    for (auto [lo, hi] : ranges) {
        if (HWINEVENTHOOK h = SetWinEventHook(
                lo, hi, nullptr, WindowTrackerEventProc, 0, 0,
                WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS)) {
            g_tracker.hooks.push_back(h);
        }
    }

    g_tracker.shellHookMsg = RegisterWindowMessageW(L"SHELLHOOK");
    if (dockHwnd && RegisterShellHookWindow(dockHwnd)) {
        g_tracker.shellHookRegistered = true;
    }
    g_tracker.foregroundHwnd = GetForegroundWindow();
}

void StopWindowTracker(HWND dockHwnd) {
    for (HWINEVENTHOOK h : g_tracker.hooks) {
        UnhookWinEvent(h);
    }
    g_tracker.hooks.clear();
    if (g_tracker.shellHookRegistered && dockHwnd) {
        DeregisterShellHookWindow(dockHwnd);
        g_tracker.shellHookRegistered = false;
    }
    g_tracker.identityCache.clear();
    g_tracker.pidExeCache.clear();
    g_tracker.vdm = nullptr;
}

// ============================================================================
// 9. Pinned Store, Recents & Item List Reconciliation
// ============================================================================

struct StoredApp {
    bool isAumid = false;
    std::wstring aumid;
    std::wstring path;
    std::wstring shortcutPath;
    std::wstring taskbarAppId;
    std::vector<BYTE> taskbarPidl;
    std::wstring args;
    std::wstring workdir;
    std::wstring name;

    std::wstring IdentityKey() const {
        if (!aumid.empty()) {
            return L"aumid:" + ToLower(aumid);
        }
        if (!path.empty()) {
            return L"path:" + NormalizePath(path);
        }
        return L"lnk:" + NormalizePath(shortcutPath);
    }
};

std::vector<StoredApp> g_pinnedApps;
std::vector<StoredApp> g_recentApps;
std::vector<std::wstring> g_runningOrder;  // Preserves append order of unpinned running apps

bool IsDistinctAppAumid(std::wstring_view a) {
    if (a.find(L'!') != std::wstring_view::npos) {
        return true;
    }
    std::wstring low = ToLower(a);
    return low.find(L"_crx_") != std::wstring::npos;
}

bool IsRealAumid(std::wstring_view a) {
    if (a.empty() || a.find(L'\\') != std::wstring_view::npos ||
        a.find(L'/') != std::wstring_view::npos || a[0] == L'{') {
        return false;
    }
    constexpr std::wstring_view kAutoGen = L"Microsoft.AutoGenerated.";
    if (a.size() >= kAutoGen.size() &&
        _wcsnicmp(a.data(), kAutoGen.data(), kAutoGen.size()) == 0) {
        return false;
    }
    return true;
}

bool AppsMatch(const std::wstring& aumidA,
               const std::wstring& pathA,
               const std::wstring& aumidB,
               const std::wstring& pathB) {
    if (!aumidA.empty() && !aumidB.empty()) {
        if (EqualsIgnoreCase(aumidA, aumidB)) {
            return true;
        }
        if (IsDistinctAppAumid(aumidA) || IsDistinctAppAumid(aumidB) ||
            IsRealAumid(aumidA) || IsRealAumid(aumidB)) {
            return false;
        }
    }
    if (IsDistinctAppAumid(aumidA) || IsDistinctAppAumid(aumidB)) {
        return false;
    }
    if (!pathA.empty() && !pathB.empty()) {
        return NormalizePath(pathA) == NormalizePath(pathB);
    }
    return false;
}

std::wstring SanitizeField(std::wstring_view sv) {
    std::wstring out;
    out.reserve(sv.size());
    for (wchar_t ch : sv) {
        if (ch != L'\n' && ch != L'\r' && ch != L'\x1F' && ch != L'\x1E') {
            out.push_back(ch);
        }
    }
    return out;
}

std::wstring SerializeStoredAppEntry(const StoredApp& a) {
    std::wstring out;
    out += (a.isAumid ? L"aumid" : L"path");
    out.push_back(L'\x1F');
    out += SanitizeField(a.aumid);
    out.push_back(L'\x1F');
    out += SanitizeField(a.path);
    out.push_back(L'\x1F');
    out += SanitizeField(a.args);
    out.push_back(L'\x1F');
    out += SanitizeField(a.workdir);
    out.push_back(L'\x1F');
    out += SanitizeField(a.name);
    return out;
}

std::optional<StoredApp> DeserializeStoredAppEntry(std::wstring_view line) {
    if (line.empty()) {
        return std::nullopt;
    }
    std::array<std::wstring, 6> fields;
    size_t fpos = 0;
    for (int fi = 0; fi < 6; ++fi) {
        size_t sep = line.find(L'\x1F', fpos);
        if (sep == std::wstring_view::npos) {
            fields[fi] = std::wstring(line.substr(fpos));
            break;
        }
        fields[fi] = std::wstring(line.substr(fpos, sep - fpos));
        fpos = sep + 1;
    }
    StoredApp app;
    app.isAumid = (fields[0] == L"aumid");
    app.aumid = std::move(fields[1]);
    app.path = std::move(fields[2]);
    app.args = std::move(fields[3]);
    app.workdir = std::move(fields[4]);
    app.name = std::move(fields[5]);
    if (app.aumid.empty() && app.path.empty()) {
        return std::nullopt;
    }
    return app;
}

std::vector<StoredApp> LoadStoredAppList(PCWSTR prefix, PCWSTR legacyKey) {
    std::vector<StoredApp> list;
    for (const auto& line : LoadNumberedStringList(prefix, legacyKey)) {
        if (auto app = DeserializeStoredAppEntry(line)) {
            list.push_back(std::move(*app));
        }
    }
    return list;
}

void SaveStoredAppList(PCWSTR prefix, const std::vector<StoredApp>& list) {
    std::vector<std::wstring> entries;
    entries.reserve(list.size());
    for (const auto& a : list) {
        entries.push_back(SerializeStoredAppEntry(a));
    }
    SaveNumberedStringList(prefix, entries);
}

void SavePinnedApps() {
    SaveStoredAppList(L"pinned_v2", g_pinnedApps);
}

void SaveRecentApps() {
    SaveStoredAppList(L"recents_v2", g_recentApps);
}

void SavePinnedFolders() {
    std::vector<std::wstring> entries;
    entries.reserve(g_pinnedFolders.size());
    for (const auto& f : g_pinnedFolders) {
        entries.push_back(SanitizeField(f.path) + L"\x1F" +
                          SanitizeField(f.name) + L"\x1F" +
                          std::to_wstring(f.viewMode) + L"\x1F" +
                          std::to_wstring(f.sortMode));
    }
    SaveNumberedStringList(L"pinned_folders_v2", entries);
}

void LoadPinnedFolders() {
    g_pinnedFolders.clear();
    for (const auto& line :
         LoadNumberedStringList(L"pinned_folders_v2", L"pinned_folders_v1")) {
        if (!line.empty()) {
            std::vector<std::wstring> cols;
            size_t cpos = 0;
            while (cpos <= line.size()) {
                size_t tb = line.find(L'\x1F', cpos);
                cols.push_back((tb == std::wstring::npos)
                                   ? line.substr(cpos)
                                   : line.substr(cpos, tb - cpos));
                if (tb == std::wstring::npos) break;
                cpos = tb + 1;
            }
            if (!cols.empty() && !cols[0].empty()) {
                PinnedFolderStack pf;
                pf.path = cols[0];
                pf.name = (cols.size() > 1 && !cols[1].empty())
                              ? cols[1]
                              : PathFindFileNameW(pf.path.c_str());
                pf.viewMode = (cols.size() > 2) ? std::clamp(_wtoi(cols[2].c_str()), 0, 2) : 0;
                pf.sortMode = (cols.size() > 3) ? std::clamp(_wtoi(cols[3].c_str()), 0, 2) : 0;
                g_pinnedFolders.push_back(std::move(pf));
            }
        }
    }
}

void GetDownloadsStackConfig(int& viewMode, int& sortMode) {
    viewMode = std::clamp(Wh_GetIntValue(L"dl_stack_view", 0), 0, 2);
    sortMode = std::clamp(Wh_GetIntValue(L"dl_stack_sort", 0), 0, 2);
}

void SetDownloadsStackConfig(int viewMode, int sortMode) {
    Wh_SetIntValue(L"dl_stack_view", std::clamp(viewMode, 0, 2));
    Wh_SetIntValue(L"dl_stack_sort", std::clamp(sortMode, 0, 2));
}

enum PINNEDLISTMODIFYCALLER {
    PLMC_EXPLORER = 4,
};

constexpr CLSID kClsidTaskbandPin = {
    0x90AA3A4E, 0x1CBA, 0x4233, {0xB8, 0xBB, 0x53, 0x57, 0x73, 0xD4, 0x84, 0x49}};
constexpr IID kIidIPinnedList3 = {
    0x0DD79AE2, 0xD156, 0x45D4, {0x9E, 0xEB, 0x3B, 0x54, 0x97, 0x69, 0xE9, 0x40}};
constexpr CLSID kClsidApplicationDocumentListsLocal = {
    0x86BEC222, 0x30F2, 0x47E0, {0x9F, 0x25, 0x60, 0xD1, 0x1C, 0xD7, 0x5C, 0x28}};

struct IPinnedList3Local : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE EnumObjects(IEnumFullIDList** ppv) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPinnableInfo(IDataObject* ido,
                                                      int pinnableFlag,
                                                      IShellItem** psi,
                                                      IShellItem** psi2,
                                                      PWSTR* ppsz,
                                                      int* pfPinnable) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsPinnable(IDataObject* ido,
                                                 int pinnableFlag) = 0;
    virtual HRESULT STDMETHODCALLTYPE Resolve(
        HWND hwnd,
        UINT flags,
        PCIDLIST_ABSOLUTE pidl,
        PIDLIST_ABSOLUTE* ppidlResolved) = 0;
    virtual HRESULT STDMETHODCALLTYPE LegacyModify(PCIDLIST_ABSOLUTE unpin,
                                                   PCIDLIST_ABSOLUTE pin) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChangeCount(ULONG* pulChangeCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsPinned(PCIDLIST_ABSOLUTE pidl) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPinnedItem(
        PCIDLIST_ABSOLUTE pidl,
        PIDLIST_ABSOLUTE* ppidlOut) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAppIDForPinnedItem(
        PCIDLIST_ABSOLUTE pidl,
        PWSTR* ppszAppID) = 0;
    virtual HRESULT STDMETHODCALLTYPE ItemChangeNotify(
        PCIDLIST_ABSOLUTE pidlOld,
        PCIDLIST_ABSOLUTE pidlNew) = 0;
    virtual HRESULT STDMETHODCALLTYPE UpdateForRemovedItemsAsNecessary() = 0;
    virtual HRESULT STDMETHODCALLTYPE PinShellLink(UINT flags,
                                                   IShellLinkW* psl) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPinnedItemForAppID(
        PCWSTR pszAppID,
        PIDLIST_ABSOLUTE* ppidlOut) = 0;
    virtual HRESULT STDMETHODCALLTYPE Modify(PCIDLIST_ABSOLUTE unpin,
                                             PCIDLIST_ABSOLUTE pin,
                                             int caller) = 0;
};

struct TaskbarPinSyncState {
    ULONG lastChangeCount = UINT_MAX;
    bool initialized = false;
    HKEY taskbandKey = nullptr;
    DWORD lastRegChangeCount = UINT_MAX;
    bool hasRegCount = false;
};

TaskbarPinSyncState g_taskbarSync;

void CloseTaskbarSyncKey() {
    if (g_taskbarSync.taskbandKey) {
        RegCloseKey(g_taskbarSync.taskbandKey);
        g_taskbarSync.taskbandKey = nullptr;
    }
    g_taskbarSync.hasRegCount = false;
}

bool ReadTaskbandFavoritesChangeCount(DWORD& outCount) {
    if (!g_taskbarSync.taskbandKey) {
        if (RegOpenKeyExW(
                HKEY_CURRENT_USER,
                L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Taskband",
                0, KEY_QUERY_VALUE, &g_taskbarSync.taskbandKey) !=
            ERROR_SUCCESS) {
            g_taskbarSync.taskbandKey = nullptr;
            return false;
        }
    }
    DWORD val = 0;
    DWORD type = 0;
    DWORD sz = sizeof(val);
    LSTATUS st = RegQueryValueExW(g_taskbarSync.taskbandKey,
                                  L"FavoritesChanges", nullptr, &type,
                                  reinterpret_cast<LPBYTE>(&val), &sz);
    if (st != ERROR_SUCCESS || type != REG_DWORD) {
        if (st == ERROR_KEY_DELETED) {
            CloseTaskbarSyncKey();
        }
        return false;
    }
    outCount = val;
    return true;
}

winrt::com_ptr<IPinnedList3Local> CreateTaskbarPinnedList() {
    winrt::com_ptr<IPinnedList3Local> pl;
    if (FAILED(CoCreateInstance(kClsidTaskbandPin, nullptr, CLSCTX_INPROC_SERVER,
                                kIidIPinnedList3, pl.put_void()))) {
        return nullptr;
    }
    return pl;
}

std::wstring GetUserPinnedTaskbarDir() {
    WCHAR appData[MAX_PATH * 2]{};
    if (ExpandEnvironmentStringsW(
            L"%APPDATA%\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar",
            appData, ARRAYSIZE(appData)) > 0) {
        return appData;
    }
    return {};
}

bool FileExistsNonDir(const std::wstring& path) {
    if (path.empty()) {
        return false;
    }
    std::wstring accessPath = NativeSystemPath(path);
    DWORD attr = GetFileAttributesW(accessPath.c_str());
    return attr != INVALID_FILE_ATTRIBUTES &&
           !(attr & FILE_ATTRIBUTE_DIRECTORY);
}

std::wstring ResolveWow64Path(const std::wstring& rawTarget) {
    if (rawTarget.empty()) {
        return {};
    }
    auto expandEnv = [](const std::wstring& s) -> std::wstring {
        WCHAR buf[MAX_PATH * 2]{};
        DWORD n = ExpandEnvironmentStringsW(s.c_str(), buf, ARRAYSIZE(buf));
        return (n > 0 && n <= ARRAYSIZE(buf)) ? std::wstring(buf) : s;
    };
    std::wstring expanded = expandEnv(rawTarget);
    if (FileExistsNonDir(expanded)) {
        return expanded;
    }

    WCHAR pf32[MAX_PATH]{}, pf64[MAX_PATH]{};
    GetEnvironmentVariableW(L"ProgramFiles", pf32, ARRAYSIZE(pf32));
    GetEnvironmentVariableW(L"ProgramW6432", pf64, ARRAYSIZE(pf64));
    if (pf32[0] && pf64[0] && _wcsicmp(pf32, pf64) != 0) {
        size_t pf32Len = wcslen(pf32);
        if (expanded.size() > pf32Len &&
            _wcsnicmp(expanded.c_str(), pf32, pf32Len) == 0 &&
            (expanded[pf32Len] == L'\\' || expanded[pf32Len] == L'/')) {
            std::wstring alt = std::wstring(pf64) + expanded.substr(pf32Len);
            if (FileExistsNonDir(alt)) {
                return alt;
            }
        }
        static constexpr std::wstring_view kPfVar = L"%ProgramFiles%";
        if (rawTarget.size() >= kPfVar.size() &&
            _wcsnicmp(rawTarget.c_str(), kPfVar.data(), kPfVar.size()) == 0) {
            std::wstring alt =
                std::wstring(pf64) + rawTarget.substr(kPfVar.size());
            alt = expandEnv(alt);
            if (FileExistsNonDir(alt)) {
                return alt;
            }
        }
    }

    WCHAR cpf32[MAX_PATH]{}, cpf64[MAX_PATH]{};
    GetEnvironmentVariableW(L"CommonProgramFiles", cpf32, ARRAYSIZE(cpf32));
    GetEnvironmentVariableW(L"CommonProgramW6432", cpf64, ARRAYSIZE(cpf64));
    if (cpf32[0] && cpf64[0] && _wcsicmp(cpf32, cpf64) != 0) {
        size_t cpf32Len = wcslen(cpf32);
        if (expanded.size() > cpf32Len &&
            _wcsnicmp(expanded.c_str(), cpf32, cpf32Len) == 0 &&
            (expanded[cpf32Len] == L'\\' || expanded[cpf32Len] == L'/')) {
            std::wstring alt =
                std::wstring(cpf64) + expanded.substr(cpf32Len);
            if (FileExistsNonDir(alt)) {
                return alt;
            }
        }
        static constexpr std::wstring_view kCpfVar = L"%CommonProgramFiles%";
        if (rawTarget.size() >= kCpfVar.size() &&
            _wcsnicmp(rawTarget.c_str(), kCpfVar.data(), kCpfVar.size()) == 0) {
            std::wstring alt =
                std::wstring(cpf64) + rawTarget.substr(kCpfVar.size());
            alt = expandEnv(alt);
            if (FileExistsNonDir(alt)) {
                return alt;
            }
        }
    }

    // 32-bit WOW64 check for 64-bit-only binaries inside %SystemRoot%\System32
    WCHAR winDir[MAX_PATH]{};
    if (GetWindowsDirectoryW(winDir, ARRAYSIZE(winDir)) > 0) {
        std::wstring sys32Prefix = std::wstring(winDir) + L"\\System32\\";
        if (expanded.size() > sys32Prefix.size() &&
            _wcsnicmp(expanded.c_str(), sys32Prefix.c_str(),
                      sys32Prefix.size()) == 0) {
            std::wstring sysnativePath =
                std::wstring(winDir) + L"\\Sysnative\\" +
                expanded.substr(sys32Prefix.size());
            if (FileExistsNonDir(sysnativePath)) {
                return expanded;
            }
        }
    }
    return {};
}

std::optional<StoredApp> ResolveShortcutFile(const std::wstring& lnkPath) {
    winrt::com_ptr<IShellLinkW> link;
    if (FAILED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(link.put())))) {
        return std::nullopt;
    }
    winrt::com_ptr<IPersistFile> pf;
    if (FAILED(link->QueryInterface(IID_PPV_ARGS(pf.put()))) ||
        FAILED(pf->Load(lnkPath.c_str(), STGM_READ))) {
        return std::nullopt;
    }

    StoredApp app;
    app.shortcutPath = lnkPath;
    WCHAR targetResolved[MAX_PATH * 2]{};
    WCHAR targetRaw[MAX_PATH * 2]{};
    WCHAR args[1024]{};
    WCHAR workdir[MAX_PATH * 2]{};
    link->GetPath(targetResolved, ARRAYSIZE(targetResolved), nullptr, 0);
    link->GetPath(targetRaw, ARRAYSIZE(targetRaw), nullptr, SLGP_RAWPATH);
    link->GetArguments(args, ARRAYSIZE(args));
    link->GetWorkingDirectory(workdir, ARRAYSIZE(workdir));

    app.path = ResolveWow64Path(targetResolved);
    if (app.path.empty()) {
        app.path = ResolveWow64Path(targetRaw);
    }
    auto expandEnvStr = [](PCWSTR raw) -> std::wstring {
        if (!raw || !*raw) {
            return {};
        }
        WCHAR buf[1024]{};
        DWORD n = ExpandEnvironmentStringsW(raw, buf, ARRAYSIZE(buf));
        return (n > 0 && n <= ARRAYSIZE(buf)) ? std::wstring(buf)
                                               : std::wstring(raw);
    };
    app.args = expandEnvStr(args);
    app.workdir = expandEnvStr(workdir);

    winrt::com_ptr<IPropertyStore> ps;
    if (SUCCEEDED(link->QueryInterface(IID_PPV_ARGS(ps.put()))) && ps) {
        PROPVARIANT pv{};
        PropVariantInit(&pv);
        if (SUCCEEDED(ps->GetValue(kPkeyAppUserModelId, &pv))) {
            if (pv.vt == VT_LPWSTR && pv.pwszVal) {
                app.aumid = pv.pwszVal;
            }
            PropVariantClear(&pv);
        }
    }

    if (app.path.empty() && app.aumid.empty() &&
        !FileExistsNonDir(app.shortcutPath)) {
        return std::nullopt;
    }
    app.isAumid = app.path.empty();

    // Name from shortcut filename
    const wchar_t* fname = PathFindFileNameW(lnkPath.c_str());
    std::wstring stem = fname ? fname : L"";
    size_t dot = stem.find_last_of(L'.');
    if (dot != std::wstring::npos) {
        stem.resize(dot);
    }
    app.name = stem;
    if (app.name.empty() && !app.aumid.empty()) {
        app.name = GetDisplayNameForAumid(app.aumid);
    }
    if (app.name.empty() && !app.path.empty()) {
        app.name = GetFileDescriptionFromPath(app.path);
    }
    return app;
}

std::vector<std::wstring> LoadPinnedOrderOverlay() {
    return LoadNumberedStringList(L"dock_pin_order_v2", L"dock_pin_order_v1");
}

void SavePinnedOrderOverlay(const std::vector<StoredApp>& apps) {
    std::vector<std::wstring> entries;
    entries.reserve(apps.size());
    for (const auto& a : apps) {
        std::wstring k = SanitizeField(a.IdentityKey());
        if (!k.empty()) {
            entries.push_back(std::move(k));
        }
    }
    SaveNumberedStringList(L"dock_pin_order_v2", entries);
}

void ApplyPinOrderOverlay(std::vector<StoredApp>& apps) {
    auto order = LoadPinnedOrderOverlay();
    if (order.empty() || apps.size() <= 1) {
        return;
    }
    std::unordered_map<std::wstring, size_t> rank;
    for (size_t i = 0; i < order.size(); ++i) {
        rank.try_emplace(order[i], i);
    }
    std::vector<std::pair<size_t, StoredApp>> ordered;
    std::vector<StoredApp> fresh;
    ordered.reserve(apps.size());
    for (auto& a : apps) {
        auto it = rank.find(a.IdentityKey());
        if (it != rank.end()) {
            ordered.emplace_back(it->second, std::move(a));
        } else {
            fresh.push_back(std::move(a));
        }
    }
    std::stable_sort(ordered.begin(), ordered.end(),
                     [](const auto& lhs, const auto& rhs) {
                         return lhs.first < rhs.first;
                     });
    apps.clear();
    for (auto& kv : ordered) {
        apps.push_back(std::move(kv.second));
    }
    for (auto& a : fresh) {
        apps.push_back(std::move(a));
    }
}

bool LoadPinnedAppsFromTaskbar(std::vector<StoredApp>& outApps,
                               ULONG* outChangeCount) {
    outApps.clear();
    auto pl = CreateTaskbarPinnedList();
    if (!pl) {
        return false;
    }
    ULONG cc = 0;
    pl->GetChangeCount(&cc);
    if (outChangeCount) {
        *outChangeCount = cc;
    }

    winrt::com_ptr<IEnumFullIDList> en;
    if (FAILED(pl->EnumObjects(en.put())) || !en) {
        return false;
    }

    std::wstring explorerNorm = NormalizePath(GetExplorerExePath());
    PIDLIST_ABSOLUTE pidl = nullptr;
    ULONG fetched = 0;
    while (en->Next(1, &pidl, &fetched) == S_OK && fetched > 0 && pidl) {
        UINT pidlSz = ILGetSize(pidl);
        std::vector<BYTE> pidlCopy;
        if (pidlSz > 0) {
            pidlCopy.resize(pidlSz);
            memcpy(pidlCopy.data(), pidl, pidlSz);
        }

        std::wstring tbAppId;
        PWSTR rawAppId = nullptr;
        if (SUCCEEDED(pl->GetAppIDForPinnedItem(pidl, &rawAppId)) && rawAppId) {
            tbAppId = rawAppId;
            CoTaskMemFree(rawAppId);
        }

        winrt::com_ptr<IShellItem> si;
        std::wstring normalName;
        std::wstring fsPath;
        std::wstring parseName;
        std::wstring relName;
        if (SUCCEEDED(SHCreateItemFromIDList(pidl, IID_PPV_ARGS(si.put()))) &&
            si) {
            auto grabName = [&](SIGDN sig) -> std::wstring {
                PWSTR p = nullptr;
                if (SUCCEEDED(si->GetDisplayName(sig, &p)) && p) {
                    std::wstring res = p;
                    CoTaskMemFree(p);
                    return res;
                }
                return {};
            };
            normalName = grabName(SIGDN_NORMALDISPLAY);
            fsPath = grabName(SIGDN_FILESYSPATH);
            parseName = grabName(SIGDN_DESKTOPABSOLUTEPARSING);
            relName = grabName(SIGDN_PARENTRELATIVEPARSING);
        }
        CoTaskMemFree(pidl);
        pidl = nullptr;

        std::optional<StoredApp> sa;
        if (!fsPath.empty()) {
            sa = ResolveShortcutFile(fsPath);
            if (sa) {
                if (sa->name.empty() && !normalName.empty()) {
                    sa->name = normalName;
                }
            } else {
                StoredApp fallback;
                fallback.shortcutPath = fsPath;
                fallback.name = normalName;
                sa = std::move(fallback);
            }
        } else {
            std::wstring aid =
                !tbAppId.empty() ? tbAppId
                                 : (!relName.empty() ? relName : parseName);
            if (aid.rfind(L"::{", 0) == 0) {
                size_t bs = aid.find(L'\\');
                if (bs != std::wstring::npos && bs + 1 < aid.size()) {
                    aid = aid.substr(bs + 1);
                }
            }
            if (!aid.empty()) {
                StoredApp uwp;
                uwp.isAumid = true;
                uwp.aumid = aid;
                uwp.name = !normalName.empty() ? normalName
                                               : GetDisplayNameForAumid(aid);
                if (uwp.name.empty()) {
                    uwp.name = aid;
                }
                sa = std::move(uwp);
            }
        }
        if (!sa) {
            continue;
        }
        sa->taskbarAppId = std::move(tbAppId);
        if (sa->aumid.empty() && IsRealAumid(sa->taskbarAppId)) {
            sa->aumid = sa->taskbarAppId;
        }
        sa->taskbarPidl = std::move(pidlCopy);

        // Avoid duplicating File Explorer when Finder is pinned at the left.
        if (g_settings.showFinder &&
            (EqualsIgnoreCase(sa->aumid, L"Microsoft.Windows.Explorer") ||
             EqualsIgnoreCase(sa->taskbarAppId, L"Microsoft.Windows.Explorer") ||
             (!sa->path.empty() && NormalizePath(sa->path) == explorerNorm))) {
            continue;
        }

        bool dup = false;
        for (const auto& existing : outApps) {
            if (AppsMatch(existing.aumid, existing.path, sa->aumid, sa->path) &&
                existing.IdentityKey() == sa->IdentityKey()) {
                dup = true;
                break;
            }
        }
        if (!dup) {
            outApps.push_back(std::move(*sa));
        }
    }

    ApplyPinOrderOverlay(outApps);
    return true;
}

bool SyncPinnedAppsWithTaskbar(bool forceReload = false) {
    DWORD regCc = 0;
    bool gotRegCc = ReadTaskbandFavoritesChangeCount(regCc);
    if (!forceReload && g_taskbarSync.initialized && gotRegCc &&
        g_taskbarSync.hasRegCount &&
        regCc == g_taskbarSync.lastRegChangeCount) {
        return false;
    }

    auto pl = CreateTaskbarPinnedList();
    if (!pl) {
        return false;
    }
    ULONG cc = 0;
    if (SUCCEEDED(pl->GetChangeCount(&cc)) && !forceReload &&
        g_taskbarSync.initialized && cc == g_taskbarSync.lastChangeCount) {
        if (gotRegCc) {
            g_taskbarSync.lastRegChangeCount = regCc;
            g_taskbarSync.hasRegCount = true;
        }
        return false;
    }

    std::vector<StoredApp> fresh;
    ULONG loadedCc = cc;
    if (!LoadPinnedAppsFromTaskbar(fresh, &loadedCc)) {
        return false;
    }
    bool changed = !g_taskbarSync.initialized ||
                   loadedCc != g_taskbarSync.lastChangeCount ||
                   fresh.size() != g_pinnedApps.size();
    if (!changed) {
        for (size_t i = 0; i < fresh.size(); ++i) {
            if (fresh[i].IdentityKey() != g_pinnedApps[i].IdentityKey() ||
                fresh[i].shortcutPath != g_pinnedApps[i].shortcutPath ||
                fresh[i].taskbarAppId != g_pinnedApps[i].taskbarAppId) {
                changed = true;
                break;
            }
        }
    }
    g_pinnedApps = std::move(fresh);
    g_taskbarSync.lastChangeCount = loadedCc;
    g_taskbarSync.initialized = true;
    if (ReadTaskbandFavoritesChangeCount(regCc)) {
        g_taskbarSync.lastRegChangeCount = regCc;
        g_taskbarSync.hasRegCount = true;
    }
    return changed;
}

void SeedPinnedAppsIfNeeded() {
    LoadPinnedFolders();
    g_recentApps = LoadStoredAppList(L"recents_v2", L"recents_v1");
    size_t beforeRecents = g_recentApps.size();
    g_recentApps.erase(
        std::remove_if(g_recentApps.begin(), g_recentApps.end(),
                       [](const StoredApp& r) {
                           return !r.path.empty() &&
                                  IsTransientShellHostExe(r.path);
                       }),
        g_recentApps.end());
    if (g_recentApps.size() != beforeRecents) {
        SaveRecentApps();
    }

    if (SyncPinnedAppsWithTaskbar(true)) {
        return;
    }

    // Fallback when IPinnedList3 COM server is unavailable
    if (Wh_GetIntValue(L"seeded", 0) != 0) {
        g_pinnedApps = LoadStoredAppList(L"pinned_v2", L"pinned_v1");
        return;
    }

    std::vector<StoredApp> seeded;
    std::wstring explorerNorm = NormalizePath(GetExplorerExePath());
    std::wstring tbDir = GetUserPinnedTaskbarDir();
    if (!tbDir.empty()) {
        std::wstring pattern = tbDir + L"\\*.lnk";
        WIN32_FIND_DATAW fd{};
        HANDLE hFind = FindFirstFileW(pattern.c_str(), &fd);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    continue;
                }
                std::wstring full = tbDir + L"\\" + fd.cFileName;
                if (auto app = ResolveShortcutFile(full)) {
                    if (!app->path.empty() &&
                        NormalizePath(app->path) == explorerNorm) {
                        continue;
                    }
                    bool dup = false;
                    for (const auto& existing : seeded) {
                        if (AppsMatch(existing.aumid, existing.path, app->aumid,
                                      app->path)) {
                            dup = true;
                            break;
                        }
                    }
                    if (!dup) {
                        seeded.push_back(std::move(*app));
                    }
                }
            } while (FindNextFileW(hFind, &fd));
            FindClose(hFind);
        }
    }

    if (seeded.empty()) {
        const std::array<PCWSTR, 4> fallbacks = {{
            L"MSEdge",
            L"Microsoft.WindowsStore_8wekyb3d8bbwe!App",
            L"windows.immersivecontrolpanel_cw5n1h2txyewy!microsoft.windows.immersivecontrolpanel",
            L"Microsoft.WindowsTerminal_8wekyb3d8bbwe!App",
        }};
        for (PCWSTR aumid : fallbacks) {
            winrt::com_ptr<IShellItem> item;
            if (SUCCEEDED(SHCreateItemInKnownFolder(kFolderIdAppsFolder, 0,
                                                    aumid,
                                                    IID_PPV_ARGS(item.put())))) {
                StoredApp app;
                app.isAumid = true;
                app.aumid = aumid;
                app.name = GetDisplayNameForAumid(aumid);
                if (app.name.empty()) {
                    app.name = aumid;
                }
                seeded.push_back(std::move(app));
            }
        }
    }

    g_pinnedApps = std::move(seeded);
    SavePinnedApps();
    Wh_SetIntValue(L"seeded", 1);
}

void PushRecentApp(const StoredApp& app) {
    if ((app.aumid.empty() && app.path.empty()) ||
        (!app.path.empty() && IsTransientShellHostExe(app.path))) {
        return;
    }
    // Remove duplicates
    g_recentApps.erase(
        std::remove_if(g_recentApps.begin(), g_recentApps.end(),
                       [&](const StoredApp& r) {
                           return AppsMatch(r.aumid, r.path, app.aumid,
                                            app.path);
                       }),
        g_recentApps.end());
    g_recentApps.insert(g_recentApps.begin(), app);
    size_t maxKeep = static_cast<size_t>(std::max(12, g_settings.recentsCount));
    if (g_recentApps.size() > maxKeep) {
        g_recentApps.resize(maxKeep);
    }
    SaveRecentApps();
}

// Forward declarations for M3 dock window, layout, tooltip, interactions
void EnsureItemVisuals(DockItem& item);
void DetachItemVisuals(DockItem& item);
void SyncItemIconsAndTheme();
void ReconcileDockItems(bool immediate);
void LayoutAndApplyDock(double now);
void RequestAnimationFrame();

// ============================================================================
// 11. Dock Window, Tooltip, AppBar, Auto-Hide & Fullscreen State
// ============================================================================

struct TooltipState {
    wuc::ContainerVisual container{nullptr};
    wuc::CompositionRoundedRectangleGeometry clipGeom{nullptr};
    wuc::SpriteVisual backdropVisual{nullptr};
    wuc::CompositionBrush backdropBrush{nullptr};
    wuc::SpriteVisual tintVisual{nullptr};
    wuc::CompositionColorBrush tintBrush{nullptr};
    wuc::ShapeVisual rimVisual{nullptr};
    wuc::CompositionRoundedRectangleGeometry rimGeom{nullptr};
    wuc::CompositionSpriteShape rimShape{nullptr};
    wuc::CompositionLinearGradientBrush rimBrush{nullptr};
    wuc::SpriteVisual textVisual{nullptr};
    wuc::CompositionSurfaceBrush textBrush{nullptr};
    TextSurfaceResult textSurface;
    LiquidGlassPlate liquidPlate;

    std::wstring currentText;
    float opacity = 0.0f;
    float targetOpacity = 0.0f;
    float widthPx = 0.0f;
    float heightPx = 0.0f;
    float posX = 0.0f;
    float posY = 0.0f;
};

struct DockWindowState {
    HWND hwnd = nullptr;
    HWND appBarHwnd = nullptr;
    bool appBarRegistered = false;
    bool appBarFullscreen = false;

    HMONITOR monitor = nullptr;
    RECT monitorRect{};
    RECT windowRect{};
    float scale = 1.0f;
    bool darkMode = false;
    bool clickThrough = true;
    bool hasHostBackdrop = false;

    wf::Size windowSize{0, 0};
    D2D1_RECT_F shelfRect{};
    D2D1_RECT_F activeRegion{};
    float shelfCornerRadius = 0.0f;
    float effectiveTilePx = 48.0f;
    float lastShadowShelfH = -1.0f;
    float lastGlassW = -1.0f;
    float lastGlassH = -1.0f;
    float lastGlassR = -1.0f;
    bool modalActive = false;

    // Magnification & hover state
    bool cursorInActive = false;
    float cursorMainCoord = 0.0f;
    float cursorMainSmooth = 0.0f;
    bool cursorSmoothValid = false;
    float magAmount = 0.0f;      // 0..1
    int hoveredIndex = -1;
    int pressedIndex = -1;
    POINT lastPollCursor{INT_MIN, INT_MIN};
    ULONGLONG lastCursorMoveTick = 0;

    // Auto-hide & fullscreen slide state (0 = visible, 1 = hidden offscreen)
    bool fullscreenHidden = false;
    double lastFullscreenCheck = 0.0;
    bool autoHideRevealed = false;
    double cursorLeftTime = 0.0;
    bool wasCursorInActive = false;
    float hideProgress = 0.0f;
    float slideExtentPx = 120.0f;

    // Frame timing
    double lastTickTime = 0.0;
    bool animating = false;
    bool layoutDirty = true;
    bool glassMaintenanceTimerActive = false;
    UINT cursorPollIntervalMs = 0;
    HWND cursorPollHwnd = nullptr;

    wuc::Desktop::DesktopWindowTarget target{nullptr};
    wuc::ContainerVisual root{nullptr};
    wuc::ContainerVisual contentRoot{nullptr};
    wuc::CompositionBackdropBrush hostBackdrop{nullptr};

    // Shadow layer
    wuc::SpriteVisual shadowVisual{nullptr};
    wuc::CompositionNineGridBrush shadowNineGrid{nullptr};
    wuc::CompositionSurfaceBrush shadowSurfaceBrush{nullptr};
    ShadowSurfaceResult shadowSurface;

    // 7-Layer macOS 27 Liquid Glass shelf
    wuc::ContainerVisual glassContainer{nullptr};
    wuc::CompositionPathGeometry glassClipGeom{nullptr};
    wuc::SpriteVisual backdropVisual{nullptr};
    wuc::CompositionBrush backdropEffectBrush{nullptr};
    wuc::SpriteVisual tintVisual{nullptr};
    wuc::CompositionColorBrush tintBrush{nullptr};
    wuc::SpriteVisual sheenVisual{nullptr};
    wuc::CompositionLinearGradientBrush sheenBrush{nullptr};
    wuc::SpriteVisual bounceVisual{nullptr};
    wuc::CompositionLinearGradientBrush bounceBrush{nullptr};
    wuc::ShapeVisual bandVisual{nullptr};
    wuc::CompositionPathGeometry bandGeom{nullptr};
    wuc::CompositionSpriteShape bandShape{nullptr};
    wuc::CompositionColorBrush bandBrush{nullptr};
    wuc::ShapeVisual rimVisual{nullptr};
    wuc::CompositionPathGeometry rimGeom{nullptr};
    wuc::CompositionSpriteShape rimShape{nullptr};
    wuc::CompositionLinearGradientBrush rimBrush{nullptr};

    // True optical Liquid Glass plate (DXGI refraction + macOS 27 dark lighting)
    LiquidGlassPlate liquidPlate;
    LiquidGlassStyleParams lastGlassParams{};
    bool hasGlassParams = false;
    bool liquidShelfAttached = false;

    // Items container + tooltip
    wuc::ContainerVisual itemsContainer{nullptr};
    TooltipState tooltip;
};

DockWindowState g_dock;
HANDLE g_uiThread = nullptr;
DWORD g_uiThreadId = 0;
winrt::Windows::System::DispatcherQueueController g_dqController{nullptr};
std::atomic<bool> g_quitting{false};

void PostDeviceLostMessage() {
    if (g_dock.hwnd &&
        !g_deviceLostPending.exchange(true, std::memory_order_acq_rel)) {
        PostMessageW(g_dock.hwnd, kWmAppDeviceLost, 0, 0);
    }
}

void PostDeviceRedrawMessage() {
    if (g_dock.hwnd &&
        !g_deviceRedrawPending.exchange(true, std::memory_order_acq_rel)) {
        PostMessageW(g_dock.hwnd, kWmAppDeviceRedraw, 0, 0);
    }
}

constexpr WCHAR kDockClassName[] = L"WindhawkMacOS27DockWnd";
constexpr WCHAR kAppBarClassName[] = L"WindhawkMacOS27AppBarWnd";

void RequestAnimationFrame() {
    g_dock.animating = true;
    g_dock.layoutDirty = true;
}

void ScheduleWindowRefresh() {
    g_tracker.dirty = true;
    if (!g_dock.hwnd || g_tracker.refreshScheduled) {
        return;
    }
    double now = NowSeconds();
    double elapsedMs = (now - g_tracker.lastRefreshTime) * 1000.0;
    UINT delay = elapsedMs >= 100.0 ? 15u : static_cast<UINT>(100.0 - elapsedMs);
    g_tracker.refreshScheduled = true;
    SetTimer(g_dock.hwnd, kTimerWindowRefresh, std::max(15u, delay), nullptr);
}

void ScheduleFullscreenCheck() {
    if (!g_dock.hwnd) {
        return;
    }
    SetTimer(g_dock.hwnd, kTimerFullscreenCheck, 150, nullptr);
}

void UpdateClickThrough(bool wantTransparent) {
    if (!g_dock.hwnd || wantTransparent == g_dock.clickThrough) {
        return;
    }
    LONG_PTR ex = GetWindowLongPtrW(g_dock.hwnd, GWL_EXSTYLE);
    if (wantTransparent) {
        ex |= WS_EX_TRANSPARENT;
    } else {
        ex &= ~WS_EX_TRANSPARENT;
    }
    SetWindowLongPtrW(g_dock.hwnd, GWL_EXSTYLE, ex);
    g_dock.clickThrough = wantTransparent;
}

bool PointInRectF(const D2D1_RECT_F& r, float x, float y) {
    return x >= r.left && x <= r.right && y >= r.top && y <= r.bottom;
}

bool IsCursorAtScreenEdge(POINT pt) {
    const RECT& mr = g_dock.monitorRect;
    constexpr int kStrip = 2;
    switch (g_settings.position) {
        case DockPosition::Bottom:
            return pt.x >= mr.left && pt.x < mr.right &&
                   pt.y >= mr.bottom - kStrip && pt.y <= mr.bottom;
        case DockPosition::Left:
            return pt.y >= mr.top && pt.y < mr.bottom &&
                   pt.x >= mr.left && pt.x < mr.left + kStrip;
        case DockPosition::Right:
            return pt.y >= mr.top && pt.y < mr.bottom &&
                   pt.x >= mr.right - kStrip && pt.x <= mr.right;
    }
    return false;
}

bool AnyItemHasAttentionBounce() {
    for (const auto& item : g_items) {
        if (item.attention) {
            return true;
        }
    }
    return false;
}

void SetGlassMaintenanceTimer(bool on) {
    if (!g_dock.hwnd || on == g_dock.glassMaintenanceTimerActive) {
        return;
    }
    g_dock.glassMaintenanceTimerActive = on;
    if (on) {
        SetTimer(g_dock.hwnd, kTimerGlassMaintenance, 150, nullptr);
    } else {
        KillTimer(g_dock.hwnd, kTimerGlassMaintenance);
    }
}

void UpdateCursorTrackingTimer(UINT intervalMs) {
    if (!g_dock.hwnd) {
        return;
    }
    if (intervalMs != g_dock.cursorPollIntervalMs ||
        g_dock.hwnd != g_dock.cursorPollHwnd) {
        g_dock.cursorPollIntervalMs = intervalMs;
        g_dock.cursorPollHwnd = g_dock.hwnd;
        SetTimer(g_dock.hwnd, kTimerCursorPoll, intervalMs, nullptr);
    }
}

bool IsWindowPreviewFlyoutOpen();

void PollCursorState() {
    if (!g_dock.hwnd) {
        return;
    }
    POINT pt{};
    if (!GetCursorPos(&pt)) {
        return;
    }

    ULONGLONG nowTick = GetTickCount64();
    bool moved =
        (pt.x != g_dock.lastPollCursor.x || pt.y != g_dock.lastPollCursor.y);
    if (moved) {
        g_dock.lastPollCursor = pt;
        g_dock.lastCursorMoveTick = nowTick;
    }

    const RECT& wndRect = g_dock.windowRect;
    bool insideWnd = PtInRect(&wndRect, pt) != FALSE;
    bool atEdge = IsCursorAtScreenEdge(pt);

    const RECT& mr = g_dock.monitorRect;
    float nearDist = 140.0f * g_dock.scale;
    bool nearEdge = false;
    if (PtInRect(&mr, pt)) {
        if (g_settings.position == DockPosition::Bottom) {
            nearEdge = pt.y >= static_cast<LONG>(mr.bottom - nearDist);
        } else if (g_settings.position == DockPosition::Left) {
            nearEdge = pt.x <= static_cast<LONG>(mr.left + nearDist);
        } else {
            nearEdge = pt.x >= static_cast<LONG>(mr.right - nearDist);
        }
    }

    bool activeZone =
        insideWnd || atEdge || nearEdge || IsWindowPreviewFlyoutOpen();
    ULONGLONG idleMs = nowTick - g_dock.lastCursorMoveTick;
    UINT wantMs = activeZone
                      ? 100u
                      : ((moved || idleMs < 500)
                             ? 100u
                             : (idleMs < 1500 ? 150u : 250u));
    UpdateCursorTrackingTimer(wantMs);

    if (!moved && !g_dock.cursorInActive && !atEdge &&
        !g_dock.autoHideRevealed && !insideWnd) {
        UpdateClickThrough(true);
        return;
    }

    float lx = static_cast<float>(pt.x - wndRect.left);
    float ly = static_cast<float>(pt.y - wndRect.top);

    bool visibleForHit = (g_dock.hideProgress < 0.5f) && !g_dock.fullscreenHidden;
    bool inActive =
        visibleForHit && insideWnd && PointInRectF(g_dock.activeRegion, lx, ly);

    if (g_settings.autoHide && !g_dock.fullscreenHidden) {
        if (atEdge) {
            if (!g_dock.autoHideRevealed) {
                g_dock.autoHideRevealed = true;
                RequestAnimationFrame();
            }
            g_dock.cursorLeftTime = 0.0;
        } else if (inActive) {
            g_dock.autoHideRevealed = true;
            g_dock.cursorLeftTime = 0.0;
        } else if (g_dock.autoHideRevealed) {
            if (g_dock.cursorLeftTime == 0.0) {
                g_dock.cursorLeftTime = NowSeconds();
                RequestAnimationFrame();
            }
        }
    }

    float mainCoord =
        (g_settings.position == DockPosition::Bottom) ? lx : ly;

    if (inActive != g_dock.cursorInActive ||
        (inActive && std::fabs(mainCoord - g_dock.cursorMainCoord) > 0.25f)) {
        g_dock.cursorInActive = inActive;
        g_dock.cursorMainCoord = mainCoord;
        if (!inActive) {
            g_dock.cursorSmoothValid = false;
        } else if (!g_dock.cursorSmoothValid) {
            g_dock.cursorMainSmooth = mainCoord;
            g_dock.cursorSmoothValid = true;
        }
        RequestAnimationFrame();
    } else {
        g_dock.cursorInActive = inActive;
        if (!inActive) {
            g_dock.cursorSmoothValid = false;
        }
    }

    UpdateClickThrough(!inActive);
}

bool IsWindowPreviewFlyoutOpen();
void CloseWindowPreviewFlyout();
void RefreshWindowPreviewFlyoutIfOpen();

void CheckFullscreenState() {
    double now = NowSeconds();
    g_dock.lastFullscreenCheck = now;

    bool fs = g_dock.appBarFullscreen;

    if (!fs) {
        QUERY_USER_NOTIFICATION_STATE quns = QUNS_ACCEPTS_NOTIFICATIONS;
        if (SUCCEEDED(SHQueryUserNotificationState(&quns))) {
            if (quns == QUNS_BUSY || quns == QUNS_RUNNING_D3D_FULL_SCREEN ||
                quns == QUNS_PRESENTATION_MODE) {
                fs = true;
            }
        }
    }

    if (!fs) {
        HWND fg = GetForegroundWindow();
        if (fg && IsWindow(fg) && !IsIconic(fg) && !IsZoomed(fg)) {
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
                    if (SUCCEEDED(DwmGetWindowAttribute(
                            fg, DWMWA_EXTENDED_FRAME_BOUNDS, &rc,
                            sizeof(rc))) ||
                        GetWindowRect(fg, &rc)) {
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
        if (fs) {
            CloseWindowPreviewFlyout();
        }
        RequestAnimationFrame();
    }
}

void RebuildBackdropBrushes() {
    if (!g_gfx.compositor || !g_dock.hostBackdrop) {
        return;
    }
    const float blurPx =
        (g_settings.glassBlur <= 0)
            ? 0.0f
            : (4.0f + 22.0f * (static_cast<float>(g_settings.glassBlur) / 100.0f)) *
                  g_dock.scale;
    const float sat = g_dock.darkMode ? 1.60f : 1.45f;

    g_dock.backdropEffectBrush = CreateLiquidGlassBackdropBrush(
        g_gfx.compositor, g_dock.hostBackdrop, blurPx, sat);
    if (g_dock.backdropVisual) {
        g_dock.backdropVisual.Brush(g_dock.backdropEffectBrush);
    }
    if (g_dock.tooltip.backdropVisual) {
        g_dock.tooltip.backdropBrush = CreateLiquidGlassBackdropBrush(
            g_gfx.compositor, g_dock.hostBackdrop,
            std::max(10.0f * g_dock.scale, blurPx * 0.85f), sat);
        g_dock.tooltip.backdropVisual.Brush(g_dock.tooltip.backdropBrush);
    }
}

void UpdateGlassAppearance() {
    if (!g_gfx.compositor || !g_dock.tintBrush || !g_dock.sheenBrush ||
        !g_dock.bounceBrush || !g_dock.bandBrush || !g_dock.rimBrush) {
        return;
    }
    g_dock.darkMode = IsSystemDarkMode();
    const bool dark = g_dock.darkMode;
    const float tintFrac = static_cast<float>(g_settings.glassTint) / 100.0f;

    // macOS 27 Dark / Light Liquid Glass smoked base fill (fallback when refraction is off)
    uint8_t baseR = dark ? 14 : 248;
    uint8_t baseG = dark ? 15 : 249;
    uint8_t baseB = dark ? 20 : 254;
    float alpha =
        dark ? (0.18f + 0.36f * tintFrac) : (0.16f + 0.28f * tintFrac);

    if (g_settings.glassStyle == GlassStyle::Tinted) {
        auto acc = GetAccentColor();
        baseR = static_cast<uint8_t>(std::lround(baseR * 0.76f + acc.R * 0.24f));
        baseG = static_cast<uint8_t>(std::lround(baseG * 0.76f + acc.G * 0.24f));
        baseB = static_cast<uint8_t>(std::lround(baseB * 0.76f + acc.B * 0.24f));
    }

    uint8_t aByte = static_cast<uint8_t>(std::lround(Clamp01(alpha) * 255.0f));
    g_dock.tintBrush.Color(
        winrt::Windows::UI::Color{aByte, baseR, baseG, baseB});

    // Upper convex dome specular sheen
    {
        auto stops = g_dock.sheenBrush.ColorStops();
        stops.Clear();
        stops.Append(g_gfx.compositor.CreateColorGradientStop(
            0.0f,
            winrt::Windows::UI::Color{static_cast<uint8_t>(dark ? 0x26 : 0x46),
                                      255, 255, 255}));
        stops.Append(g_gfx.compositor.CreateColorGradientStop(
            0.40f,
            winrt::Windows::UI::Color{static_cast<uint8_t>(dark ? 0x09 : 0x14),
                                      255, 255, 255}));
        stops.Append(g_gfx.compositor.CreateColorGradientStop(
            0.66f, winrt::Windows::UI::Color{0, 255, 255, 255}));
    }

    // Lower cool caustic bounce light along bottom curved rim
    {
        auto stops = g_dock.bounceBrush.ColorStops();
        stops.Clear();
        stops.Append(g_gfx.compositor.CreateColorGradientStop(
            0.0f, winrt::Windows::UI::Color{0, 215, 228, 255}));
        stops.Append(g_gfx.compositor.CreateColorGradientStop(
            1.0f,
            winrt::Windows::UI::Color{static_cast<uint8_t>(dark ? 0x1A : 0x26),
                                      215, 228, 255}));
    }

    // Inner glass thickness refraction band
    g_dock.bandBrush.Color(winrt::Windows::UI::Color{
        static_cast<uint8_t>(dark ? 0x14 : 0x24), 255, 255, 255});

    // Crisp multi-stop diagonal specular catchlight rim
    const float rimScale = dark ? 0.78f : 1.0f;
    auto populateRimStops = [&](wuc::CompositionLinearGradientBrush const& b) {
        auto stops = b.ColorStops();
        stops.Clear();
        auto stopColor = [rimScale](float a) {
            return winrt::Windows::UI::Color{
                static_cast<uint8_t>(std::lround(Clamp01(a * rimScale) * 255.0f)),
                255, 255, 255};
        };
        stops.Append(
            g_gfx.compositor.CreateColorGradientStop(0.00f, stopColor(0.82f)));
        stops.Append(
            g_gfx.compositor.CreateColorGradientStop(0.24f, stopColor(0.22f)));
        stops.Append(
            g_gfx.compositor.CreateColorGradientStop(0.52f, stopColor(0.10f)));
        stops.Append(
            g_gfx.compositor.CreateColorGradientStop(0.80f, stopColor(0.24f)));
        stops.Append(
            g_gfx.compositor.CreateColorGradientStop(1.00f, stopColor(0.58f)));
    };
    populateRimStops(g_dock.rimBrush);

    // Update tooltip glass theme as well
    if (g_dock.tooltip.tintBrush) {
        uint8_t tipAlpha = static_cast<uint8_t>(
            std::lround(Clamp01(alpha + 0.16f) * 255.0f));
        g_dock.tooltip.tintBrush.Color(
            winrt::Windows::UI::Color{tipAlpha, baseR, baseG, baseB});
    }
    if (g_dock.tooltip.rimBrush) {
        populateRimStops(g_dock.tooltip.rimBrush);
    }
    RebuildBackdropBrushes();
    g_dock.liquidPlate.dirty = true;
    g_dock.tooltip.liquidPlate.dirty = true;
    g_dock.tooltip.currentText.clear();
    SetGlassMaintenanceTimer(WantOpticalLiquidGlass() && !g_liquidCapture.frozen &&
                             (!g_liquidCapture.worker ||
                              !g_dock.liquidPlate.active));
}

struct ShelfMetrics {
    float topPad = 6.0f;
    float botPad = 11.0f;
    float shelfH = 65.0f;
    float corner = 21.0f;
    float endPad = 10.0f;
};

ShelfMetrics ComputeShelfMetrics(float T, float s) {
    ShelfMetrics m;
    // Asymmetric macOS 27 Tahoe vertical padding: generous dedicated bottom channel
    // so running indicator dots sit dead-center inside the dock without touching icons or rim.
    m.topPad = std::max(5.5f * s, 0.12f * T);
    m.botPad = std::max(10.5f * s, 0.21f * T);
    m.shelfH = T + m.topPad + m.botPad;
    m.corner = std::min(m.shelfH * 0.34f, m.shelfH * 0.5f / kAppleCornerReach);
    m.endPad = std::max(8.0f * s, 0.22f * T);
    return m;
}

void RebuildShadowSurface(float shelfHPx = -1.0f) {
    if (!g_dock.shadowSurfaceBrush || !g_dock.shadowNineGrid) {
        return;
    }
    const float s = g_dock.scale;
    const float T = (g_dock.effectiveTilePx > 4.0f)
                        ? g_dock.effectiveTilePx
                        : static_cast<float>(g_settings.iconSize) * s;
    const auto sm = ComputeShelfMetrics(T, s);
    const float shelfH = (shelfHPx > 0.0f) ? shelfHPx : sm.shelfH;
    const float corner =
        std::min(shelfH * 0.34f, shelfH * 0.5f / kAppleCornerReach) *
        kAppleCornerReach;

    g_dock.lastShadowShelfH = shelfH;
    g_dock.shadowSurface =
        BuildShelfShadowSurface(shelfH, corner, s, g_dock.darkMode);
    if (g_dock.shadowSurface.surface) {
        g_dock.shadowSurfaceBrush.Surface(g_dock.shadowSurface.surface);
        float inset = g_dock.shadowSurface.padPx + g_dock.shadowSurface.cornerPx;
        g_dock.shadowNineGrid.SetInsets(inset, inset, inset, inset);
    }
}

RECT GetDockAnchorWorkRect(const MONITORINFO& mi) {
    RECT r = mi.rcMonitor;
    if (g_settings.hideTaskbar) {
        return r;
    }
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tray || !IsWindowVisible(tray)) {
        return r;
    }
    APPBARDATA abd{.cbSize = sizeof(abd), .hWnd = tray};
    UINT tbState = static_cast<UINT>(SHAppBarMessage(ABM_GETSTATE, &abd));
    if ((tbState & ABS_AUTOHIDE) != 0) {
        return r;
    }
    RECT tbRc{};
    if (!GetWindowRect(tray, &tbRc)) {
        return r;
    }
    if (MonitorFromRect(&tbRc, MONITOR_DEFAULTTONEAREST) !=
        MonitorFromRect(&mi.rcMonitor, MONITOR_DEFAULTTONEAREST)) {
        return r;
    }
    LONG tbW = tbRc.right - tbRc.left;
    LONG tbH = tbRc.bottom - tbRc.top;
    if (tbW <= 0 || tbH <= 0) {
        return r;
    }
    if (tbW >= tbH) {
        if (tbRc.top >= (mi.rcMonitor.top + mi.rcMonitor.bottom) / 2) {
            r.bottom = std::min(r.bottom, tbRc.top);
        } else {
            r.top = std::max(r.top, tbRc.bottom);
        }
    } else {
        if (tbRc.left <= (mi.rcMonitor.left + mi.rcMonitor.right) / 2) {
            r.left = std::max(r.left, tbRc.right);
        } else {
            r.right = std::min(r.right, tbRc.left);
        }
    }
    if (r.right <= r.left + 64 || r.bottom <= r.top + 64) {
        return mi.rcMonitor;
    }
    return r;
}

void ResizeDockWindowForMonitor() {
    if (!g_dock.hwnd) {
        return;
    }
    HMONITOR mon = GetPrimaryMonitor();
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfoW(mon, &mi)) {
        return;
    }
    g_dock.monitor = mon;
    g_dock.monitorRect = mi.rcMonitor;
    g_dock.scale = GetMonitorDpiScale(mon);
    const float s = g_dock.scale;
    const RECT anchor = GetDockAnchorWorkRect(mi);

    const float monW = static_cast<float>(anchor.right - anchor.left);
    const float monH = static_cast<float>(anchor.bottom - anchor.top);

    const float T = static_cast<float>(g_settings.iconSize) * s;
    const auto sm = ComputeShelfMetrics(T, s);
    const float shelfH = sm.shelfH;
    const float edgeGap = static_cast<float>(g_settings.edgeGap) * s;
    const float magExtra =
        g_settings.magnification
            ? static_cast<float>(std::max(0, g_settings.EffectiveMagSize() -
                                                 g_settings.iconSize)) *
                  s
            : 0.0f;
    const float headroom = edgeGap + shelfH + magExtra + 1.0f * T + 48.0f * s;

    int winX = anchor.left;
    int winY = anchor.bottom - static_cast<int>(std::ceil(headroom));
    int winW = static_cast<int>(monW);
    int winH = static_cast<int>(std::ceil(headroom));

    if (g_settings.position == DockPosition::Left) {
        winX = anchor.left;
        winY = anchor.top;
        winW = static_cast<int>(std::ceil(headroom));
        winH = static_cast<int>(monH);
    } else if (g_settings.position == DockPosition::Right) {
        winX = anchor.right - static_cast<int>(std::ceil(headroom));
        winY = anchor.top;
        winW = static_cast<int>(std::ceil(headroom));
        winH = static_cast<int>(monH);
    }

    SetWindowPos(g_dock.hwnd, HWND_TOPMOST, winX, winY, winW, winH,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    g_dock.windowRect = {winX, winY, winX + winW, winY + winH};
    g_dock.windowSize = {static_cast<float>(winW), static_cast<float>(winH)};
    g_dock.slideExtentPx =
        edgeGap + shelfH + g_dock.shadowSurface.padPx + 12.0f * s;
}

// --- AppBar Space Reservation ---

void RemoveAppBar() {
    if (g_dock.appBarRegistered && g_dock.appBarHwnd) {
        APPBARDATA abd{.cbSize = sizeof(abd), .hWnd = g_dock.appBarHwnd};
        SHAppBarMessage(ABM_REMOVE, &abd);
        g_dock.appBarRegistered = false;
    }
    g_dock.appBarFullscreen = false;
}

void UpdateAppBarReservation() {
    bool wantReserve = g_settings.reserveSpace && !g_settings.autoHide && !g_quitting;
    if (!wantReserve || !g_dock.appBarHwnd) {
        RemoveAppBar();
        return;
    }

    HMONITOR mon = GetPrimaryMonitor();
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfoW(mon, &mi)) {
        return;
    }
    float s = GetMonitorDpiScale(mon);
    float tilePx = (g_dock.effectiveTilePx > 4.0f)
                       ? g_dock.effectiveTilePx
                       : static_cast<float>(g_settings.iconSize) * s;
    const auto sm = ComputeShelfMetrics(tilePx, s);
    int thickness = static_cast<int>(std::ceil(
        static_cast<float>(g_settings.edgeGap) * s + sm.shelfH));

    APPBARDATA abd{};
    abd.cbSize = sizeof(abd);
    abd.hWnd = g_dock.appBarHwnd;
    abd.uCallbackMessage = kWmAppAppBar;

    if (!g_dock.appBarRegistered) {
        // FALSE also means the appbar is already registered (e.g. non-restart TaskbarCreated).
        SHAppBarMessage(ABM_NEW, &abd);
        g_dock.appBarRegistered = true;
    }

    abd.rc = GetDockAnchorWorkRect(mi);
    if (g_settings.position == DockPosition::Bottom) {
        abd.uEdge = ABE_BOTTOM;
        abd.rc.top = abd.rc.bottom - thickness;
    } else if (g_settings.position == DockPosition::Left) {
        abd.uEdge = ABE_LEFT;
        abd.rc.right = abd.rc.left + thickness;
    } else {
        abd.uEdge = ABE_RIGHT;
        abd.rc.left = abd.rc.right - thickness;
    }

    SHAppBarMessage(ABM_QUERYPOS, &abd);
    if (abd.uEdge == ABE_BOTTOM) {
        abd.rc.top = abd.rc.bottom - thickness;
    } else if (abd.uEdge == ABE_LEFT) {
        abd.rc.right = abd.rc.left + thickness;
    } else {
        abd.rc.left = abd.rc.right - thickness;
    }
    SHAppBarMessage(ABM_SETPOS, &abd);
}

LRESULT CALLBACK AppBarWndProc(HWND hwnd,
                               UINT msg,
                               WPARAM wParam,
                               LPARAM lParam) {
    if (msg == kWmAppAppBar) {
        if (wParam == ABN_POSCHANGED) {
            UpdateAppBarReservation();
            ResizeDockWindowForMonitor();
            RequestAnimationFrame();
        } else if (wParam == ABN_FULLSCREENAPP) {
            g_dock.appBarFullscreen = (lParam != 0);
            CheckFullscreenState();
        }
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void CreateAppBarWindow(HINSTANCE hInst) {
    WNDCLASSEXW wc{
        .cbSize = sizeof(wc),
        .lpfnWndProc = AppBarWndProc,
        .hInstance = hInst,
        .lpszClassName = kAppBarClassName,
    };
    RegisterClassExW(&wc);

    g_dock.appBarHwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kAppBarClassName, L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, hInst,
        nullptr);
    if (g_dock.appBarHwnd) {
        SetLayeredWindowAttributes(g_dock.appBarHwnd, 0, 0, LWA_ALPHA);
    }
}

void EnsureItemVisuals(DockItem& item) {
    auto& c = g_gfx.compositor;
    if (!c || !g_dock.itemsContainer) {
        return;
    }
    if (!item.container) {
        item.container = c.CreateContainerVisual();
        g_dock.itemsContainer.Children().InsertAtTop(item.container);
    }

    if (item.kind == DockItemKind::Divider) {
        if (!item.dividerVisual) {
            item.dividerBrush = c.CreateColorBrush();
            item.dividerVisual = c.CreateSpriteVisual();
            item.dividerVisual.Brush(item.dividerBrush);
            item.container.Children().InsertAtTop(item.dividerVisual);
        }
        return;
    }

    if (!item.iconVisual) {
        item.iconBrush = c.CreateSurfaceBrush();
        item.iconBrush.Stretch(wuc::CompositionStretch::Fill);
        item.iconBrush.BitmapInterpolationMode(
            wuc::CompositionBitmapInterpolationMode::Linear);
        item.iconVisual = c.CreateSpriteVisual();
        item.iconVisual.Brush(item.iconBrush);
        item.container.Children().InsertAtTop(item.iconVisual);
    }

    if (!item.pressVisual) {
        item.pressBrush = c.CreateMaskBrush();
        item.pressBrush.Source(
            c.CreateColorBrush(winrt::Windows::UI::Color{90, 0, 0, 0}));
        item.pressBrush.Mask(item.iconBrush);
        item.pressVisual = c.CreateSpriteVisual();
        item.pressVisual.Brush(item.pressBrush);
        item.pressVisual.IsVisible(false);
        item.container.Children().InsertAtTop(item.pressVisual);
    }

    if (!item.dotVisual) {
        item.dotGeom = c.CreateRoundedRectangleGeometry();
        auto dotShape = c.CreateSpriteShape(item.dotGeom);
        item.dotBrush = c.CreateColorBrush();
        dotShape.FillBrush(item.dotBrush);
        item.dotVisual = c.CreateShapeVisual();
        item.dotVisual.Shapes().Append(dotShape);
        item.dotVisual.IsVisible(false);
        item.container.Children().InsertAtTop(item.dotVisual);
    }
}

void DetachItemVisuals(DockItem& item) {
    if (item.container && g_dock.itemsContainer) {
        try {
            g_dock.itemsContainer.Children().Remove(item.container);
        } catch (...) {
        }
    }
    item.container = nullptr;
    item.iconVisual = nullptr;
    item.iconBrush = nullptr;
    item.pressVisual = nullptr;
    item.pressBrush = nullptr;
    item.dotVisual = nullptr;
    item.dotGeom = nullptr;
    item.dotBrush = nullptr;
    item.dividerVisual = nullptr;
    item.dividerBrush = nullptr;
    item.boundSurface = nullptr;
}

float GetIconRenderResolutionPx() {
    float base = static_cast<float>(
        g_settings.magnification
            ? std::max(g_settings.iconSize, g_settings.EffectiveMagSize())
            : g_settings.iconSize);
    return std::min(512.0f, std::ceil(base * g_dock.scale));
}

void SyncItemIconsAndTheme() {
    const float renderPx = GetIconRenderResolutionPx();
    const bool dark = g_dock.darkMode;
    auto divColor = dark ? winrt::Windows::UI::Color{68, 224, 236, 255}
                         : winrt::Windows::UI::Color{64, 0, 0, 0};

    for (auto& item : g_items) {
        EnsureItemVisuals(item);
        if (item.kind == DockItemKind::Divider) {
            if (item.dividerBrush) {
                item.dividerBrush.Color(divColor);
            }
            continue;
        }
        auto surf = GetOrRenderItemIcon(item, renderPx, g_dock.scale, dark);
        if (surf && surf != item.boundSurface) {
            item.iconBrush.Surface(surf);
            item.boundSurface = surf;
        }
    }
}

bool DoesWindowMatchDockItem(HWND fg, const DockItem& item) {
    if (!fg || !IsWindow(fg) || IsIconic(fg) || item.windows.empty()) {
        return false;
    }
    HWND root = GetAncestor(fg, GA_ROOT);
    HWND rootOwner = GetAncestor(fg, GA_ROOTOWNER);
    for (HWND h : item.windows) {
        if (h == fg || h == root || h == rootOwner) {
            return true;
        }
    }
    if (item.kind == DockItemKind::Finder) {
        for (HWND cand : {fg, root, rootOwner}) {
            if (!cand || !IsWindow(cand)) continue;
            WCHAR cls[64]{};
            if (GetClassNameW(cand, cls, ARRAYSIZE(cls)) &&
                (_wcsicmp(cls, L"CabinetWClass") == 0 ||
                 _wcsicmp(cls, L"ExploreWClass") == 0)) {
                return true;
            }
        }
        return false;
    }
    HWND target = (root && IsWindow(root)) ? root : fg;
    WindowIdentity wid = ResolveWindowIdentity(target);
    if (!wid.isExplorerCabinet &&
        (!wid.aumid.empty() || !wid.exePath.empty()) &&
        AppsMatch(item.aumid, item.path, wid.aumid, wid.exePath)) {
        return true;
    }
    return false;
}

void ReconcileDockItems(bool immediate) {
    g_tracker.dirty = false;
    g_tracker.lastRefreshTime = NowSeconds();
    auto tracked = EnumerateTrackedWindows();

    // Build desired list of logical items
    std::vector<DockItem> desired;

    // 1. Finder
    if (g_settings.showFinder) {
        DockItem finder;
        finder.kind = DockItemKind::Finder;
        finder.identityKey = L"fixed:finder";
        finder.path = GetExplorerExePath();
        finder.name = L"Finder";
        finder.pinned = true;
        desired.push_back(std::move(finder));
    }

    // 2. Apps button
    if (g_settings.showAppsButton) {
        DockItem apps;
        apps.kind = DockItemKind::Apps;
        apps.identityKey = L"fixed:apps";
        apps.name = L"Apps";
        apps.pinned = true;
        desired.push_back(std::move(apps));
    }

    // 3. Pinned apps
    for (const auto& pa : g_pinnedApps) {
        DockItem item;
        item.kind = DockItemKind::App;
        item.aumid = pa.aumid;
        item.path = pa.path;
        item.shortcutPath = pa.shortcutPath;
        item.taskbarAppId = pa.taskbarAppId;
        item.taskbarPidl = pa.taskbarPidl;
        item.args = pa.args;
        item.workdir = pa.workdir;
        item.name = pa.name;
        item.identityKey = pa.IdentityKey();
        item.pinned = true;
        desired.push_back(std::move(item));
    }

    // Group tracked windows into existing desired items or new running unpinned apps.
    struct RunningGroup {
        std::wstring key;
        DockItem item;
    };
    std::vector<RunningGroup> unpinnedRunning;

    for (const auto& tw : tracked) {
        if (tw.id.isExplorerCabinet) {
            for (auto& d : desired) {
                if (d.kind == DockItemKind::Finder) {
                    d.windows.push_back(tw.hwnd);
                    if (tw.active || !d.lastActive) {
                        d.lastActive = tw.hwnd;
                    }
                    break;
                }
            }
            continue;
        }

        // Match against pinned items first
        DockItem* matched = nullptr;
        for (auto& d : desired) {
            if (d.kind == DockItemKind::App &&
                AppsMatch(d.aumid, d.path, tw.id.aumid, tw.id.exePath)) {
                matched = &d;
                break;
            }
        }
        if (matched) {
            matched->windows.push_back(tw.hwnd);
            if (tw.active || !matched->lastActive) {
                matched->lastActive = tw.hwnd;
            }
            continue;
        }

        // Match against already collected unpinned running items
        for (auto& rg : unpinnedRunning) {
            if (AppsMatch(rg.item.aumid, rg.item.path, tw.id.aumid,
                          tw.id.exePath)) {
                matched = &rg.item;
                break;
            }
        }
        if (matched) {
            matched->windows.push_back(tw.hwnd);
            if (tw.active || !matched->lastActive) {
                matched->lastActive = tw.hwnd;
            }
            continue;
        }

        DockItem runItem;
        runItem.kind = DockItemKind::App;
        runItem.aumid = tw.id.aumid;
        runItem.path = tw.id.exePath;
        runItem.name = tw.id.displayName;
        runItem.pinned = false;
        runItem.identityKey =
            !runItem.aumid.empty()
                ? (L"aumid:" + ToLower(runItem.aumid))
                : (L"path:" + NormalizePath(runItem.path));
        runItem.windows.push_back(tw.hwnd);
        runItem.lastActive = tw.hwnd;

        std::wstring key = runItem.identityKey;
        if (std::find(g_runningOrder.begin(), g_runningOrder.end(), key) ==
            g_runningOrder.end()) {
            g_runningOrder.push_back(key);
        }
        unpinnedRunning.push_back({std::move(key), std::move(runItem)});
    }

    // Detect unpinned running apps whose last window closed -> push to Recents.
    if (!immediate) {
        for (const auto& prev : g_items) {
            if (prev.kind == DockItemKind::App && !prev.pinned &&
                prev.targetPresence > 0.5f && !prev.windows.empty()) {
                bool stillRunning = false;
                for (const auto& rg : unpinnedRunning) {
                    if (rg.key == prev.identityKey) {
                        stillRunning = true;
                        break;
                    }
                }
                bool nowPinned = false;
                for (const auto& pa : g_pinnedApps) {
                    if (AppsMatch(pa.aumid, pa.path, prev.aumid, prev.path)) {
                        nowPinned = true;
                        break;
                    }
                }
                if (!stillRunning && !nowPinned) {
                    StoredApp rec;
                    rec.isAumid = prev.path.empty();
                    rec.aumid = prev.aumid;
                    rec.path = prev.path;
                    rec.args = prev.args;
                    rec.workdir = prev.workdir;
                    rec.name = prev.name;
                    PushRecentApp(rec);
                }
            }
        }
    }

    // Prune closed apps from running order
    g_runningOrder.erase(
        std::remove_if(g_runningOrder.begin(), g_runningOrder.end(),
                       [&](const std::wstring& k) {
                           return std::none_of(
                               unpinnedRunning.begin(), unpinnedRunning.end(),
                               [&](const RunningGroup& rg) {
                                   return rg.key == k;
                               });
                       }),
        g_runningOrder.end());

    // 4. Running unpinned apps in stable append order
    for (const auto& key : g_runningOrder) {
        for (auto& rg : unpinnedRunning) {
            if (rg.key == key) {
                desired.push_back(std::move(rg.item));
                break;
            }
        }
    }

    // 5. Optional recents section
    if (g_settings.showRecents && g_settings.recentsCount > 0) {
        std::vector<DockItem> recItems;
        for (const auto& ra : g_recentApps) {
            if (static_cast<int>(recItems.size()) >= g_settings.recentsCount) {
                break;
            }
            bool alreadyPresent = false;
            for (const auto& d : desired) {
                if (d.kind == DockItemKind::App &&
                    AppsMatch(d.aumid, d.path, ra.aumid, ra.path)) {
                    alreadyPresent = true;
                    break;
                }
            }
            if (alreadyPresent) {
                continue;
            }
            DockItem r;
            r.kind = DockItemKind::Recent;
            r.aumid = ra.aumid;
            r.path = ra.path;
            r.args = ra.args;
            r.workdir = ra.workdir;
            r.name = ra.name;
            r.identityKey = L"recent:" + ra.IdentityKey();
            r.pinned = false;
            recItems.push_back(std::move(r));
        }
        if (!recItems.empty()) {
            DockItem divRec;
            divRec.kind = DockItemKind::Divider;
            divRec.identityKey = L"fixed:div_recents";
            desired.push_back(std::move(divRec));
            for (auto& r : recItems) {
                desired.push_back(std::move(r));
            }
        }
    }

    // 6. Divider before Right Zone (Minimized Windows / Stacks / Trash)
    {
        DockItem divEnd;
        divEnd.kind = DockItemKind::Divider;
        divEnd.identityKey = L"fixed:div_end";
        desired.push_back(std::move(divEnd));
    }

    // 6b. Minimized window live preview tiles when "Minimize into App Icon" is off
    std::unordered_set<HWND> activeMinimizedSet;
    if (!g_settings.minimizeIntoAppIcon) {
        for (const auto& tw : tracked) {
            if (tw.hwnd && IsWindow(tw.hwnd) && IsIconic(tw.hwnd)) {
                activeMinimizedSet.insert(tw.hwnd);
                DockItem mw;
                mw.kind = DockItemKind::MinimizedWindow;
                mw.minimizedHwnd = tw.hwnd;
                mw.identityKey =
                    L"minwin:" +
                    std::to_wstring(reinterpret_cast<uintptr_t>(tw.hwnd));
                mw.aumid = tw.id.aumid;
                mw.path = tw.id.exePath;
                WCHAR tbuf[128]{};
                if (GetWindowTextW(tw.hwnd, tbuf, ARRAYSIZE(tbuf)) > 0 &&
                    tbuf[0]) {
                    mw.name = tbuf;
                } else {
                    mw.name = tw.id.displayName.empty() ? L"Minimized Window"
                                                        : tw.id.displayName;
                }
                mw.pinned = false;
                desired.push_back(std::move(mw));
            }
        }
    }
    // Prune stale minimized thumbnails for closed/restored windows
    for (auto it = g_minWindowThumbs.begin(); it != g_minWindowThumbs.end();) {
        if (!IsWindow(it->first) || !IsIconic(it->first)) {
            it = g_minWindowThumbs.erase(it);
        } else {
            ++it;
        }
    }

    // 7. Downloads Stack
    if (g_settings.showDownloads) {
        DockItem dl;
        dl.kind = DockItemKind::Downloads;
        dl.identityKey = L"fixed:downloads";
        dl.path = GetEffectiveDownloadsPath();
        dl.name = L"Downloads";
        dl.pinned = true;
        desired.push_back(std::move(dl));
    }

    // 7b. Custom pinned Folder Stacks (drag any folder onto Dock!)
    for (const auto& pf : g_pinnedFolders) {
        DWORD attr = GetFileAttributesW(pf.path.c_str());
        if (attr == INVALID_FILE_ATTRIBUTES || !(attr & FILE_ATTRIBUTE_DIRECTORY)) {
            continue;
        }
        DockItem fs;
        fs.kind = DockItemKind::FolderStack;
        fs.identityKey = L"folder:" + NormalizePath(pf.path);
        fs.path = pf.path;
        fs.name = pf.name.empty() ? PathFindFileNameW(pf.path.c_str()) : pf.name;
        fs.pinned = true;
        desired.push_back(std::move(fs));
    }

    // 8. Trash
    {
        DockItem tr;
        tr.kind = DockItemKind::Trash;
        tr.identityKey = L"fixed:trash";
        tr.name = L"Trash";
        tr.pinned = true;
        desired.push_back(std::move(tr));
    }

    HWND activeFg = ResolveActiveForegroundWindow();
    for (auto& d : desired) {
        if ((d.kind == DockItemKind::App || d.kind == DockItemKind::Finder) &&
            !d.windows.empty()) {
            d.focused = (activeFg && DoesWindowMatchDockItem(activeFg, d));
            bool anyVisible = false;
            for (HWND h : d.windows) {
                if (IsWindow(h) && !IsIconic(h)) {
                    anyVisible = true;
                    break;
                }
            }
            d.allMinimized = !anyVisible;
        }
    }

    // Reconcile g_items in place to preserve visuals, bounce state, and presence animation.
    std::unordered_map<std::wstring, size_t> oldIndexByKey;
    for (size_t i = 0; i < g_items.size(); ++i) {
        oldIndexByKey[g_items[i].identityKey] = i;
    }

    std::vector<bool> oldUsed(g_items.size(), false);
    std::vector<DockItem> merged;
    merged.reserve(desired.size() + 4);
    bool stateChanged = (desired.size() != g_items.size());

    for (auto& d : desired) {
        size_t idx = SIZE_MAX;
        if (auto it = oldIndexByKey.find(d.identityKey);
            it != oldIndexByKey.end() && !oldUsed[it->second]) {
            idx = it->second;
        } else if (d.kind == DockItemKind::App ||
                   d.kind == DockItemKind::Recent) {
            for (size_t i = 0; i < g_items.size(); ++i) {
                if (!oldUsed[i] &&
                    (g_items[i].kind == DockItemKind::App ||
                     g_items[i].kind == DockItemKind::Recent) &&
                    AppsMatch(g_items[i].aumid, g_items[i].path, d.aumid,
                              d.path)) {
                    idx = i;
                    break;
                }
            }
        }
        if (idx != SIZE_MAX) {
            oldUsed[idx] = true;
            DockItem existing = std::move(g_items[idx]);
            if (existing.kind != d.kind || existing.pinned != d.pinned ||
                existing.identityKey != d.identityKey ||
                existing.windows != d.windows ||
                existing.focused != d.focused ||
                existing.allMinimized != d.allMinimized ||
                existing.minimizedHwnd != d.minimizedHwnd ||
                existing.name != d.name) {
                stateChanged = true;
            }
            existing.kind = d.kind;
            existing.pinned = d.pinned;
            existing.identityKey = std::move(d.identityKey);
            existing.shortcutPath = std::move(d.shortcutPath);
            existing.taskbarAppId = std::move(d.taskbarAppId);
            existing.taskbarPidl = std::move(d.taskbarPidl);
            if (!d.name.empty()) {
                existing.name = d.name;
            }
            if (!d.path.empty()) {
                existing.path = d.path;
            }
            if (!d.aumid.empty()) {
                existing.aumid = d.aumid;
            }
            existing.args = d.args;
            existing.workdir = d.workdir;
            existing.minimizedHwnd = d.minimizedHwnd;
            existing.windows = std::move(d.windows);
            existing.focused = d.focused;
            existing.allMinimized = d.allMinimized;
            if (!existing.windows.empty()) {
                if (std::find(existing.windows.begin(), existing.windows.end(),
                              existing.lastActive) == existing.windows.end()) {
                    existing.lastActive = d.lastActive;
                }
                if (existing.launching) {
                    existing.launching = false;
                    stateChanged = true;
                }
            } else {
                existing.lastActive = nullptr;
                existing.attention = false;
                existing.focused = false;
                existing.allMinimized = false;
            }
            existing.targetPresence = 1.0f;
            if (immediate) {
                existing.presence = 1.0f;
                existing.presenceVel = 0.0f;
                existing.focusAnim = existing.focused ? 1.0f : 0.0f;
            }
            merged.push_back(std::move(existing));
        } else {
            stateChanged = true;
            d.targetPresence = 1.0f;
            d.presence = immediate ? 1.0f : 0.0f;
            d.presenceVel = 0.0f;
            d.focusAnim = d.focused ? 1.0f : 0.0f;
            merged.push_back(std::move(d));
        }
    }

    // Keep departing items animating out (presence -> 0) unless immediate.
    for (size_t i = 0; i < g_items.size(); ++i) {
        if (oldUsed[i]) {
            continue;
        }
        stateChanged = true;
        if (immediate || g_items[i].presence <= 0.02f) {
            DetachItemVisuals(g_items[i]);
        } else {
            g_items[i].targetPresence = 0.0f;
            g_items[i].windows.clear();
            g_items[i].lastActive = nullptr;
            g_items[i].launching = false;
            g_items[i].attention = false;
            g_items[i].focused = false;
            g_items[i].allMinimized = false;
            merged.push_back(std::move(g_items[i]));
        }
    }

    g_items = std::move(merged);
    SyncItemIconsAndTheme();
    RefreshWindowPreviewFlyoutIfOpen();
    if (stateChanged || immediate) {
        RequestAnimationFrame();
    }
}

void OnWindowAttentionFlash(HWND hwnd) {
    if (!g_settings.bounceForAttention || !hwnd) {
        return;
    }
    auto findAndBounce = [&](HWND target) -> bool {
        for (auto& item : g_items) {
            if (std::find(item.windows.begin(), item.windows.end(), target) !=
                item.windows.end()) {
                item.attention = true;
                item.attentionStart = NowSeconds();
                RequestAnimationFrame();
                return true;
            }
        }
        return false;
    };
    if (!findAndBounce(hwnd)) {
        ReconcileDockItems(false);
        findAndBounce(hwnd);
    }
}

void ApplyOverlayCaptureExclusion(DWORD affinity);
bool HasOpenOverlayUi();

void OnDockHoveredIndexChanged(int newHovered);
void OnFlyoutHoverTimer();
void OnFlyoutLeaveTimer();
void RefreshWindowPreviewFlyoutIfOpen();
void AddFlyoutWatchedRegion(std::vector<RECT>& watchList);
void AddAllOverlayWatchedRegions(std::vector<RECT>& watchList);
void ResetFlyoutGpuResources();
void RefreshOpticalGlassPlatesOnly();

void OnWindowForegroundChanged(HWND hwnd) {
    bool valid = (hwnd && IsWindow(hwnd));
    if (valid) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == GetCurrentProcessId()) {
            return;
        }
    }
    bool clearAll = !valid || IsIconic(hwnd);
    bool changed = false;
    bool matchedAny = false;
    for (auto& item : g_items) {
        if (item.kind != DockItemKind::App &&
            item.kind != DockItemKind::Finder) {
            continue;
        }
        bool nowFocused = !clearAll && !item.windows.empty() &&
                          DoesWindowMatchDockItem(hwnd, item);
        if (nowFocused) {
            matchedAny = true;
            if (std::find(item.windows.begin(), item.windows.end(), hwnd) !=
                item.windows.end()) {
                item.lastActive = hwnd;
            }
            if (item.attention) {
                item.attention = false;
                changed = true;
            }
        }
        if (nowFocused != item.focused) {
            item.focused = nowFocused;
            changed = true;
        }
    }
    if (!clearAll && !matchedAny && IsEligibleAppWindow(hwnd)) {
        ScheduleWindowRefresh();
    }
    if (changed) {
        RequestAnimationFrame();
    }
}

// ============================================================================
// 10. Layout, True macOS Magnification, Bounces, Dots & Tooltip (14)
// ============================================================================

inline float MagIntegralF(float x) {
    constexpr float kPi = 3.14159265358979323846f;
    if (x <= -1.0f) {
        return -0.5f;
    }
    if (x >= 1.0f) {
        return 0.5f;
    }
    return 0.5f * x + std::sin(kPi * x) / (2.0f * kPi);
}

void UpdateTooltipVisual(int hoveredIdx, float s) {
    auto& tip = g_dock.tooltip;
    if (!tip.container || !g_gfx.compositor) {
        return;
    }

    bool show =
        hoveredIdx >= 0 && hoveredIdx < static_cast<int>(g_items.size()) &&
        g_items[hoveredIdx].kind != DockItemKind::Divider &&
        !g_items[hoveredIdx].name.empty() && g_dock.hideProgress < 0.25f &&
        !g_dock.fullscreenHidden && !g_dock.modalActive &&
        !IsWindowPreviewFlyoutOpen();

    tip.targetOpacity = show ? 1.0f : 0.0f;

    if (show) {
        const auto& item = g_items[hoveredIdx];
        if (tip.opacity > 0.5f && tip.currentText != item.name) {
            tip.opacity = 1.0f;
        }
        if (tip.currentText != item.name || !tip.textSurface.surface) {
            tip.currentText = item.name;
            auto textColor = g_dock.darkMode
                                 ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.95f)
                                 : D2D1::ColorF(0.08f, 0.08f, 0.10f, 0.92f);
            tip.textSurface = RenderTextSurface(
                item.name, 13.0f * s, DWRITE_FONT_WEIGHT_MEDIUM, textColor,
                260.0f * s);
            if (tip.textSurface.surface && tip.textBrush) {
                tip.textBrush.Surface(tip.textSurface.surface);
            }
            const float padX = 12.0f * s;
            tip.heightPx = 26.0f * s;
            tip.widthPx =
                std::max(tip.heightPx, tip.textSurface.widthPx + padX * 2.0f);
            float r = tip.heightPx * 0.5f;

            tip.container.Size({tip.widthPx, tip.heightPx});
            tip.clipGeom.Size({tip.widthPx, tip.heightPx});
            tip.clipGeom.CornerRadius({r, r});
            tip.backdropVisual.Size({tip.widthPx, tip.heightPx});
            tip.tintVisual.Size({tip.widthPx, tip.heightPx});

            float sw = 1.0f * s;
            tip.rimVisual.Size({tip.widthPx, tip.heightPx});
            tip.rimShape.StrokeThickness(sw);
            tip.rimBrush.StartPoint({0.0f, 0.0f});
            tip.rimBrush.EndPoint({tip.widthPx, tip.heightPx});
            tip.rimGeom.Offset({sw * 0.5f, sw * 0.5f});
            tip.rimGeom.Size(
                {std::max(1.0f, tip.widthPx - sw),
                 std::max(1.0f, tip.heightPx - sw)});
            tip.rimGeom.CornerRadius({std::max(1.0f, r - sw * 0.5f),
                                      std::max(1.0f, r - sw * 0.5f)});

            float tx = (tip.widthPx - tip.textSurface.widthPx) * 0.5f;
            float ty = (tip.heightPx - tip.textSurface.heightPx) * 0.5f;
            tip.textVisual.Offset({tx, ty, 0.0f});
            tip.textVisual.Size(
                {tip.textSurface.widthPx, tip.textSurface.heightPx});
        }

        // Position 10 px*s above hovered icon's magnified top (or beside for left/right)
        const auto& ir = item.iconRect;
        const float gap = 10.0f * s;
        float x = 0.0f, y = 0.0f;
        if (g_settings.position == DockPosition::Bottom) {
            x = (ir.left + ir.right - tip.widthPx) * 0.5f;
            y = ir.top - gap - tip.heightPx;
            x = std::clamp(x, 6.0f * s,
                           std::max(6.0f * s,
                                    g_dock.windowSize.Width - tip.widthPx -
                                        6.0f * s));
            y = std::max(4.0f * s, y);
        } else if (g_settings.position == DockPosition::Left) {
            x = ir.right + gap;
            y = (ir.top + ir.bottom - tip.heightPx) * 0.5f;
            y = std::clamp(y, 6.0f * s,
                           std::max(6.0f * s,
                                    g_dock.windowSize.Height - tip.heightPx -
                                        6.0f * s));
        } else {
            x = ir.left - gap - tip.widthPx;
            y = (ir.top + ir.bottom - tip.heightPx) * 0.5f;
            y = std::clamp(y, 6.0f * s,
                           std::max(6.0f * s,
                                    g_dock.windowSize.Height - tip.heightPx -
                                        6.0f * s));
        }
        tip.posX = x;
        tip.posY = y;
        tip.container.Offset({x, y, 0.0f});
    }

    bool visible = tip.opacity > 0.01f;
    tip.container.Opacity(tip.opacity);
    tip.container.IsVisible(visible);

    if (visible && g_dock.hwnd && EnsureLiquidCaptureReady()) {
        const RECT& wr = g_dock.windowRect;
        if (wr.right > wr.left && wr.bottom > wr.top) {
            float easedHide = EaseInOutCubic(g_dock.hideProgress);
            float slidePx = easedHide * g_dock.slideExtentPx;
            float offX = (g_settings.position == DockPosition::Left)
                             ? -slidePx
                             : (g_settings.position == DockPosition::Right
                                    ? slidePx
                                    : 0.0f);
            float offY =
                (g_settings.position == DockPosition::Bottom) ? slidePx : 0.0f;
            LiquidGlassStyleParams lp{};
            lp.screenX = static_cast<float>(wr.left) + tip.posX + offX;
            lp.screenY = static_cast<float>(wr.top) + tip.posY + offY;
            lp.widthPx = tip.widthPx;
            lp.heightPx = tip.heightPx;
            lp.cornerRadiusPx = tip.heightPx * 0.5f;
            lp.scale = s;
            lp.darkMode = g_dock.darkMode;
            lp.useSquircle = true;
            lp.tintBoost = 0.14f;
            lp.blurMul = 1.25f;
            lp.rimDepthMul = 0.55f;
            lp.strengthMul = 0.85f;
            if (RenderLiquidGlassPlate(tip.liquidPlate, lp) &&
                tip.liquidPlate.visual) {
                if (tip.liquidPlate.visual.Parent() != tip.container) {
                    tip.container.Children().InsertAtBottom(
                        tip.liquidPlate.visual);
                }
                tip.liquidPlate.visual.IsVisible(true);
                if (tip.backdropVisual) tip.backdropVisual.IsVisible(false);
                if (tip.tintVisual) tip.tintVisual.IsVisible(false);
                if (tip.rimVisual) tip.rimVisual.IsVisible(false);
            }
        }
    } else {
        if (tip.liquidPlate.visual) tip.liquidPlate.visual.IsVisible(false);
        if (tip.backdropVisual) tip.backdropVisual.IsVisible(true);
        if (tip.tintVisual) tip.tintVisual.IsVisible(true);
        if (tip.rimVisual) tip.rimVisual.IsVisible(true);
    }
}

void LayoutAndApplyDock(double now) {
    if (!g_dock.hwnd || !g_gfx.compositor || !g_dock.contentRoot) {
        return;
    }

    const float s = g_dock.scale;
    const bool horiz = (g_settings.position == DockPosition::Bottom);
    const float winW = g_dock.windowSize.Width;
    const float winH = g_dock.windowSize.Height;
    const float monSpan = horiz ? winW : winH;

    // 1. True macOS auto-shrink: shrink effective tile size cleanly to fit monitor width.
    constexpr float kAppSlotRatio = 0.96f;
    constexpr float kDivSlotRatio = 0.34f;
    constexpr float kEndPadRatio = 0.18f;

    float totalUnits = 0.0f;
    for (const auto& item : g_items) {
        totalUnits +=
            (item.kind == DockItemKind::Divider ? kDivSlotRatio : kAppSlotRatio) *
            item.presence;
    }
    const float edgeMargin = 12.0f * s;
    const float maxShelfLen = std::max(64.0f * s, monSpan - 2.0f * edgeMargin);
    const float wantT = static_cast<float>(g_settings.iconSize) * s;
    const float denom = totalUnits + 2.0f * kEndPadRatio;
    const float T =
        (denom > 0.01f)
            ? std::max(20.0f * s, std::min(wantT, maxShelfLen / denom))
            : wantT;
    g_dock.effectiveTilePx = T;

    const auto sm = ComputeShelfMetrics(T, s);
    const float topPad = sm.topPad;
    const float botPad = sm.botPad;
    const float endPad = sm.endPad;
    const float shelfH = sm.shelfH;
    const float corner = sm.corner;
    const float edgeGap = static_cast<float>(g_settings.edgeGap) * s;

    // Rebuild 9-grid shadow automatically if auto-shrink changed shelf height
    if (std::fabs(shelfH - g_dock.lastShadowShelfH) > 2.0f) {
        RebuildShadowSurface(shelfH);
    }

    const float totalBase = totalUnits * T;
    const float baseStart = (monSpan - totalBase) * 0.5f;
    const float baseEnd = baseStart + totalBase;

    // 2. Single-pass macOS cosine-bell magnification with smooth screen-width clamp
    float maxMagPx = T * g_settings.EffectiveMagScale();
    if (T < wantT - 0.5f) {
        maxMagPx = std::min(maxMagPx, std::max(T * 2.15f, 68.0f * s));
    }
    const float M =
        g_settings.magnification ? std::max(1.0f, maxMagPx / T) : 1.0f;
    const float m = g_settings.magnification ? g_dock.magAmount : 0.0f;
    const float R = 2.6f * T;
    const float curCoord = g_dock.cursorSmoothValid ? g_dock.cursorMainSmooth
                                                    : g_dock.cursorMainCoord;
    const float c = std::clamp(curCoord, baseStart, baseEnd);
    const float kMag = (M - 1.0f) * m * R;

    float kMagUsed = kMag;
    if (kMag > 0.0001f) {
        float rawStart =
            (baseStart - endPad) + kMag * MagIntegralF((baseStart - endPad - c) / R);
        float rawEnd =
            (baseEnd + endPad) + kMag * MagIntegralF((baseEnd + endPad - c) / R);
        float rawSpan = rawEnd - rawStart;
        float baseSpan = totalBase + 2.0f * endPad;
        if (rawSpan > maxShelfLen && rawSpan > baseSpan + 0.5f &&
            maxShelfLen > baseSpan) {
            kMagUsed = kMag * Clamp01((maxShelfLen - baseSpan) /
                                      (rawSpan - baseSpan));
        }
    }
    auto mapFinal = [&](float u) -> float {
        if (kMagUsed <= 0.0001f) {
            return u;
        }
        return u + kMagUsed * MagIntegralF((u - c) / R);
    };

    float shelfStart = mapFinal(baseStart - endPad);
    float shelfEnd = mapFinal(baseEnd + endPad);
    float shift = 0.0f;
    if (shelfEnd - shelfStart > maxShelfLen) {
        shift = (monSpan - (shelfStart + shelfEnd)) * 0.5f;
    } else if (shelfStart < edgeMargin) {
        shift = edgeMargin - shelfStart;
    } else if (shelfEnd > monSpan - edgeMargin) {
        shift = (monSpan - edgeMargin) - shelfEnd;
    }
    shelfStart += shift;
    shelfEnd += shift;

    D2D1_RECT_F sr{};
    if (g_settings.position == DockPosition::Bottom) {
        float bottom = winH - edgeGap;
        sr = {shelfStart, bottom - shelfH, shelfEnd, bottom};
    } else if (g_settings.position == DockPosition::Left) {
        sr = {edgeGap, shelfStart, edgeGap + shelfH, shelfEnd};
    } else {
        float right = winW - edgeGap;
        sr = {right - shelfH, shelfStart, right, shelfEnd};
    }

    g_dock.shelfRect = sr;
    g_dock.shelfCornerRadius = corner;

    const float sw = std::max(1.0f, sr.right - sr.left);
    const float sh = std::max(1.0f, sr.bottom - sr.top);

    // Update shadow + glass container bounds
    if (g_dock.shadowVisual && g_dock.shadowSurface.surface) {
        float pad = g_dock.shadowSurface.padPx;
        g_dock.shadowVisual.Offset({sr.left - pad, sr.top - pad, 0.0f});
        g_dock.shadowVisual.Size({sw + pad * 2.0f, sh + pad * 2.0f});
    }

    g_dock.glassContainer.Offset({sr.left, sr.top, 0.0f});
    g_dock.glassContainer.Size({sw, sh});

    auto setFallbackShelfVisible = [&](bool vis) {
        if (g_dock.backdropVisual) g_dock.backdropVisual.IsVisible(vis);
        if (g_dock.tintVisual) g_dock.tintVisual.IsVisible(vis);
        if (g_dock.sheenVisual) g_dock.sheenVisual.IsVisible(vis);
        if (g_dock.bounceVisual) g_dock.bounceVisual.IsVisible(vis);
        if (g_dock.bandVisual) g_dock.bandVisual.IsVisible(vis);
        if (g_dock.rimVisual) g_dock.rimVisual.IsVisible(vis);
    };

    bool opticalReady = EnsureLiquidCaptureReady();
    if (!opticalReady) {
        if (g_dock.liquidPlate.visual) {
            g_dock.liquidPlate.visual.IsVisible(false);
        }
        setFallbackShelfVisible(true);

        g_dock.backdropVisual.Size({sw, sh});
        g_dock.tintVisual.Size({sw, sh});
        g_dock.sheenVisual.Size({sw, sh});
        g_dock.sheenBrush.StartPoint({0.0f, 0.0f});
        g_dock.sheenBrush.EndPoint({0.0f, sh});
        g_dock.bounceVisual.Size({sw, sh});
        g_dock.bounceBrush.StartPoint({0.0f, sh * 0.65f});
        g_dock.bounceBrush.EndPoint({0.0f, sh});

        const float strokeW = 1.15f * s;
        const float bandInset = 2.1f * s;
        g_dock.bandVisual.Size({sw, sh});
        g_dock.bandShape.StrokeThickness(1.4f * s);
        g_dock.rimVisual.Size({sw, sh});
        g_dock.rimShape.StrokeThickness(strokeW);
        g_dock.rimBrush.StartPoint({0.0f, 0.0f});
        g_dock.rimBrush.EndPoint({sw, sh});

        if (std::fabs(sw - g_dock.lastGlassW) > 0.25f ||
            std::fabs(sh - g_dock.lastGlassH) > 0.25f ||
            std::fabs(corner - g_dock.lastGlassR) > 0.25f) {
            g_dock.lastGlassW = sw;
            g_dock.lastGlassH = sh;
            g_dock.lastGlassR = corner;
            if (auto p = MakeCompositionSquirclePath(0.0f, 0.0f, sw, sh, corner)) {
                g_dock.glassClipGeom.Path(p);
            }
            if (auto bp = MakeCompositionSquirclePath(
                    bandInset, bandInset, std::max(1.0f, sw - 2.0f * bandInset),
                    std::max(1.0f, sh - 2.0f * bandInset),
                    std::max(1.0f, corner - bandInset * 0.7f))) {
                g_dock.bandGeom.Path(bp);
            }
            if (auto rp = MakeCompositionSquirclePath(
                    strokeW * 0.5f, strokeW * 0.5f, std::max(1.0f, sw - strokeW),
                    std::max(1.0f, sh - strokeW),
                    std::max(1.0f, corner - strokeW * 0.4f))) {
                g_dock.rimGeom.Path(rp);
            }
        }
    } else if (std::fabs(sw - g_dock.lastGlassW) > 0.25f ||
               std::fabs(sh - g_dock.lastGlassH) > 0.25f ||
               std::fabs(corner - g_dock.lastGlassR) > 0.25f) {
        // Only update outer clip geometry when optical glass is active (skips 2 unused fallback paths!)
        g_dock.lastGlassW = sw;
        g_dock.lastGlassH = sh;
        g_dock.lastGlassR = corner;
        if (auto p = MakeCompositionSquirclePath(0.0f, 0.0f, sw, sh, corner)) {
            g_dock.glassClipGeom.Path(p);
        }
    }

    // 3. Layout items, bounces, pixel-centered dots inside bottom dock channel, and active region
    float maxIconExtent = shelfH;
    float uCursor = baseStart;
    int newHovered = -1;
    std::vector<IconReflectionSpot> iconReflections;
    if (g_settings.dockIconReflections && opticalReady) {
        iconReflections.reserve(g_items.size());
    }

    const float iconBandCenterY =
        (sr.top + topPad + (sr.bottom - botPad)) * 0.5f;
    const float iconBandCenterXLeft =
        (sr.left + botPad + (sr.right - topPad)) * 0.5f;
    const float iconBandCenterXRight =
        (sr.left + topPad + (sr.right - botPad)) * 0.5f;

    for (size_t i = 0; i < g_items.size(); ++i) {
        auto& item = g_items[i];
        float baseSlot =
            (item.kind == DockItemKind::Divider ? kDivSlotRatio : kAppSlotRatio) *
            T * item.presence;
        float a = uCursor;
        float b = uCursor + baseSlot;
        uCursor = b;

        float xa = mapFinal(a) + shift;
        float xb = mapFinal(b) + shift;
        item.slotStart = xa;
        item.slotEnd = xb;

        if (!item.container) {
            continue;
        }
        item.container.Opacity(Clamp01(item.presence));

        if (item.kind == DockItemKind::Divider) {
            float divThick = std::max(1.0f, std::round(1.0f * s));
            float divLen = T * 0.70f * item.presence;
            if (horiz) {
                float cx = std::round((xa + xb) * 0.5f);
                item.dividerVisual.Offset(
                    {cx - divThick * 0.5f, iconBandCenterY - divLen * 0.5f,
                     0.0f});
                item.dividerVisual.Size({divThick, divLen});
            } else {
                float cx = (g_settings.position == DockPosition::Left)
                               ? iconBandCenterXLeft
                               : iconBandCenterXRight;
                float cy = std::round((xa + xb) * 0.5f);
                item.dividerVisual.Offset(
                    {cx - divLen * 0.5f, cy - divThick * 0.5f, 0.0f});
                item.dividerVisual.Size({divLen, divThick});
            }
            continue;
        }

        // Compute authentic macOS gravity bounce offset (away from screen edge)
        float bounce = 0.0f;
        if (item.attention) {
            double dt = now - item.attentionStart;
            constexpr double kPeriod = 1.15;
            constexpr int kMaxCycles = 6;
            if (dt >= kPeriod * kMaxCycles) {
                item.attention = false;
            } else {
                float phase =
                    static_cast<float>(std::fmod(dt, kPeriod) / kPeriod);
                if (phase < 0.42f) {
                    float u = phase / 0.42f;
                    bounce = 4.0f * u * (1.0f - u) * (0.85f * T);
                } else if (phase < 0.68f) {
                    float u = (phase - 0.42f) / 0.26f;
                    bounce = 4.0f * u * (1.0f - u) * (0.36f * T);
                }
            }
        } else if (item.launching) {
            double dt = now - item.launchStart;
            constexpr double kPeriod = 0.72;
            if (dt >= 8.0) {
                item.launching = false;
            } else {
                float phase =
                    static_cast<float>(std::fmod(dt, kPeriod) / kPeriod);
                if (phase < 0.72f) {
                    float u = phase / 0.72f;
                    bounce = 4.0f * u * (1.0f - u) * (0.48f * T);
                }
            }
        }
        item.bounceOffset = bounce;

        float scaleFactor = (baseSlot > 0.01f) ? ((xb - xa) / baseSlot) : 1.0f;
        item.currentScale = scaleFactor;
        float iconSize = T * scaleFactor * item.presence;
        maxIconExtent = std::max(maxIconExtent, botPad + iconSize);

        float pressDip = item.pressed ? (1.5f * s) : 0.0f;
        float x = 0.0f, y = 0.0f;
        if (g_settings.position == DockPosition::Bottom) {
            x = (xa + xb - iconSize) * 0.5f;
            y = (sr.bottom - botPad) - iconSize - bounce + pressDip;
        } else if (g_settings.position == DockPosition::Left) {
            x = sr.left + botPad + bounce - pressDip;
            y = (xa + xb - iconSize) * 0.5f;
        } else {
            x = (sr.right - botPad) - iconSize - bounce + pressDip;
            y = (xa + xb - iconSize) * 0.5f;
        }

        item.iconRect = {x, y, x + iconSize, y + iconSize};
        item.iconVisual.Offset({x, y, 0.0f});
        item.iconVisual.Size({iconSize, iconSize});
        item.pressVisual.Offset({x, y, 0.0f});
        item.pressVisual.Size({iconSize, iconSize});
        item.pressVisual.IsVisible(item.pressed);

        if (g_settings.dockIconReflections && opticalReady &&
            item.presence > 0.25f) {
            IconReflectionSpot sp;
            sp.localCenterX = (x + iconSize * 0.5f) - sr.left;
            sp.localCenterY = (g_settings.position == DockPosition::Bottom)
                                  ? (sh - 5.0f * s)
                                  : ((y + iconSize * 0.5f) - sr.top);
            sp.radiusPx = iconSize * 0.44f;
            sp.color = GetIconDominantColor(item.identityKey);
            iconReflections.push_back(sp);
        }

        // Hover hit check along main axis when cursor is in active region
        if (g_dock.cursorInActive && g_dock.cursorMainCoord >= xa &&
            g_dock.cursorMainCoord <= xb) {
            newHovered = static_cast<int>(i);
        }

        // Running / active focus indicator — subtle dot -> elongated macOS pill when focused
        bool showDot =
            g_settings.showIndicators && !item.windows.empty() &&
            (item.kind == DockItemKind::App || item.kind == DockItemKind::Finder);
        if (item.dotVisual) {
            item.dotVisual.IsVisible(showDot);
            if (showDot) {
                float f = Clamp01(item.focusAnim);
                float dotD = std::clamp(std::round(0.088f * T), 3.6f * s, 4.8f * s);
                float baseD = item.allMinimized ? (dotD * 0.84f) : dotD;
                float activeLen =
                    std::clamp(dotD * 3.1f, 11.0f * s, 16.0f * s);
                float mainLen = Lerp(baseD, activeLen, f);
                float crossLen = Lerp(baseD, dotD * 1.08f, f);
                float pillW = horiz ? mainLen : crossLen;
                float pillH = horiz ? crossLen : mainLen;
                float pillR = std::min(pillW, pillH) * 0.5f;

                float dotCx = 0.0f, dotCy = 0.0f;
                if (g_settings.position == DockPosition::Bottom) {
                    dotCx = (xa + xb) * 0.5f;
                    dotCy = std::round(sr.bottom - botPad * 0.46f);
                } else if (g_settings.position == DockPosition::Left) {
                    dotCx = std::round(sr.left + botPad * 0.46f);
                    dotCy = (xa + xb) * 0.5f;
                } else {
                    dotCx = std::round(sr.right - botPad * 0.46f);
                    dotCy = (xa + xb) * 0.5f;
                }
                item.dotVisual.Size({pillW, pillH});
                item.dotVisual.Offset(
                    {dotCx - pillW * 0.5f, dotCy - pillH * 0.5f, 0.0f});
                item.dotGeom.Size({pillW, pillH});
                item.dotGeom.CornerRadius({pillR, pillR});

                if (item.dotBrush) {
                    uint8_t baseR = g_dock.darkMode ? 228 : 28;
                    uint8_t baseG = g_dock.darkMode ? 236 : 32;
                    uint8_t baseB = g_dock.darkMode ? 246 : 40;
                    uint8_t baseA =
                        g_dock.darkMode
                            ? static_cast<uint8_t>(item.allMinimized ? 115 : 175)
                            : static_cast<uint8_t>(item.allMinimized ? 95 : 145);

                    uint8_t actR = g_dock.darkMode ? 255 : 18;
                    uint8_t actG = g_dock.darkMode ? 255 : 22;
                    uint8_t actB = g_dock.darkMode ? 255 : 30;
                    uint8_t actA = g_dock.darkMode ? 255 : 242;
                    if (g_settings.glassStyle == GlassStyle::Tinted ||
                        g_settings.iconStyle == DockIconStyle::Tinted) {
                        auto acc = GetAccentColor();
                        if (g_dock.darkMode) {
                            actR = static_cast<uint8_t>(
                                std::lround(Lerp(255.0f, acc.R, 0.28f)));
                            actG = static_cast<uint8_t>(
                                std::lround(Lerp(255.0f, acc.G, 0.28f)));
                            actB = static_cast<uint8_t>(
                                std::lround(Lerp(255.0f, acc.B, 0.28f)));
                        } else {
                            actR = acc.R;
                            actG = acc.G;
                            actB = acc.B;
                        }
                    }
                    item.dotBrush.Color(winrt::Windows::UI::Color{
                        static_cast<uint8_t>(
                            std::lround(Lerp(static_cast<float>(baseA),
                                             static_cast<float>(actA), f))),
                        static_cast<uint8_t>(
                            std::lround(Lerp(static_cast<float>(baseR),
                                             static_cast<float>(actR), f))),
                        static_cast<uint8_t>(
                            std::lround(Lerp(static_cast<float>(baseG),
                                             static_cast<float>(actG), f))),
                        static_cast<uint8_t>(
                            std::lround(Lerp(static_cast<float>(baseB),
                                             static_cast<float>(actB), f)))});
                }
            }
        }
    }

    int prevHovered = g_dock.hoveredIndex;
    g_dock.hoveredIndex = newHovered;
    if (newHovered != prevHovered) {
        OnDockHoveredIndexChanged(newHovered);
    }

    // Active hit region = shelf rect extended toward interior to tallest magnified icon while m > 0.
    D2D1_RECT_F active = sr;
    if (m > 0.001f) {
        if (g_settings.position == DockPosition::Bottom) {
            active.top = std::min(active.top, sr.bottom - maxIconExtent);
        } else if (g_settings.position == DockPosition::Left) {
            active.right = std::max(active.right, sr.left + maxIconExtent);
        } else {
            active.left = std::min(active.left, sr.right - maxIconExtent);
        }
    }
    g_dock.activeRegion = active;

    // Auto-hide / fullscreen slide offset applied to contentRoot
    float easedHide =
        (g_settings.autoHide && g_dock.autoHideRevealed)
            ? EaseOutCubic(g_dock.hideProgress)
            : EaseInOutCubic(g_dock.hideProgress);
    float slidePx = easedHide * g_dock.slideExtentPx;
    float slideX = 0.0f, slideY = 0.0f;
    if (g_settings.position == DockPosition::Bottom) {
        slideY = slidePx;
    } else if (g_settings.position == DockPosition::Left) {
        slideX = -slidePx;
    } else {
        slideX = slidePx;
    }
    g_dock.contentRoot.Offset({slideX, slideY, 0.0f});

    // Render true macOS 27 optical Liquid Glass shelf when DXGI capture is active
    bool shelfVisible = (g_dock.hideProgress < 0.98f);
    if (opticalReady && shelfVisible && g_dock.hwnd) {
        const RECT& wr = g_dock.windowRect;
        LiquidGlassStyleParams lp{};
        lp.screenX = static_cast<float>(wr.left) + sr.left + slideX;
        lp.screenY = static_cast<float>(wr.top) + sr.top + slideY;
        lp.widthPx = sw;
        lp.heightPx = sh;
        lp.cornerRadiusPx = corner;
        lp.scale = s;
        lp.darkMode = g_dock.darkMode;
        lp.useSquircle = true;
        lp.tintBoost = 0.0f;
        lp.blurMul = 1.0f;
        lp.rimDepthMul = 1.0f;
        lp.strengthMul = 1.0f;
        lp.reflections =
            g_settings.dockIconReflections ? &iconReflections : nullptr;
        g_dock.lastGlassParams = lp;
        g_dock.lastGlassParams.reflections = nullptr;
        g_dock.hasGlassParams = true;
        if (RenderLiquidGlassPlate(g_dock.liquidPlate, lp) &&
            g_dock.liquidPlate.visual) {
            if (!g_dock.liquidShelfAttached) {
                g_dock.glassContainer.Children().InsertAtTop(
                    g_dock.liquidPlate.visual);
                g_dock.liquidShelfAttached = true;
            }
            g_dock.liquidPlate.visual.IsVisible(true);
            setFallbackShelfVisible(false);
        } else {
            if (g_dock.liquidPlate.visual) {
                g_dock.liquidPlate.visual.IsVisible(false);
            }
            setFallbackShelfVisible(true);
        }
        RECT watchRc{
            static_cast<LONG>(std::floor(lp.screenX)) - 32,
            static_cast<LONG>(std::floor(lp.screenY)) - 32,
            static_cast<LONG>(std::ceil(lp.screenX + lp.widthPx)) + 32,
            static_cast<LONG>(std::ceil(lp.screenY + lp.heightPx)) + 32};
        std::vector<RECT> watchList;
        watchList.reserve(8);
        watchList.push_back(watchRc);
        AddAllOverlayWatchedRegions(watchList);
        UpdateLiquidCaptureWatchedRegions(watchList);
    } else if (g_dock.liquidPlate.visual) {
        g_dock.liquidPlate.visual.IsVisible(false);
        setFallbackShelfVisible(true);
    }

    g_liquidCapture.watching.store(shelfVisible && WantOpticalLiquidGlass() &&
                                       !g_liquidCapture.frozen,
                                   std::memory_order_relaxed);
    SetGlassMaintenanceTimer(WantOpticalLiquidGlass() &&
                             !g_liquidCapture.frozen &&
                             (!g_liquidCapture.worker ||
                              !g_dock.liquidPlate.active));

    UpdateTooltipVisual(g_dock.hoveredIndex, s);
}

void TickFrame(double now) {
    if (g_dock.lastTickTime <= 0.0) {
        g_dock.lastTickTime = now;
    }
    float dt = static_cast<float>(std::clamp(now - g_dock.lastTickTime, 0.0, 0.05));
    float effDt = (dt > 0.0f ? dt : 0.016f);
    g_dock.lastTickTime = now;

    bool stillAnimating = false;

    // 1. Exponential magnification ramp & cursor entry glide (responsive macOS feel)
    float targetMag =
        (g_settings.magnification && g_dock.cursorInActive &&
         g_dock.hideProgress < 0.25f && !g_dock.fullscreenHidden)
            ? 1.0f
            : 0.0f;
    if (g_dock.magAmount != targetMag) {
        float rate = (targetMag > g_dock.magAmount) ? 20.0f : 14.0f;
        float k = 1.0f - std::exp(-effDt * rate);
        g_dock.magAmount += (targetMag - g_dock.magAmount) * k;
        if (std::fabs(targetMag - g_dock.magAmount) < 0.002f) {
            g_dock.magAmount = targetMag;
        } else {
            stillAnimating = true;
        }
    }
    if (g_settings.magnification && g_dock.cursorInActive &&
        g_dock.cursorSmoothValid) {
        float diff = g_dock.cursorMainCoord - g_dock.cursorMainSmooth;
        if (std::fabs(diff) > 0.35f) {
            float k = 1.0f - std::exp(-effDt * 34.0f);
            g_dock.cursorMainSmooth += diff * k;
            stillAnimating = true;
        } else {
            g_dock.cursorMainSmooth = g_dock.cursorMainCoord;
        }
    }

    // 2. Auto-hide / fullscreen slide
    bool wantHidden = false;
    if (g_dock.fullscreenHidden && !g_dock.modalActive) {
        wantHidden = true;
    } else if (g_settings.autoHide) {
        if (g_dock.modalActive || IsWindowPreviewFlyoutOpen() ||
            AnyItemHasAttentionBounce()) {
            wantHidden = false;
        } else if (g_dock.autoHideRevealed) {
            if (!g_dock.cursorInActive && g_dock.cursorLeftTime > 0.0) {
                double leftMs = (now - g_dock.cursorLeftTime) * 1000.0;
                if (leftMs >= static_cast<double>(g_settings.autoHideDelay)) {
                    g_dock.autoHideRevealed = false;
                    wantHidden = true;
                } else {
                    stillAnimating = true;
                }
            }
        } else {
            wantHidden = true;
        }
    }

    float targetHide = wantHidden ? 1.0f : 0.0f;
    if (g_dock.hideProgress != targetHide) {
        float dur =
            std::max(0.05f, static_cast<float>(g_settings.autoHideAnimation) /
                                1000.0f);
        float step = effDt / dur;
        if (targetHide > g_dock.hideProgress) {
            g_dock.hideProgress = std::min(1.0f, g_dock.hideProgress + step);
        } else {
            g_dock.hideProgress = std::max(0.0f, g_dock.hideProgress - step);
        }
        if (g_dock.hideProgress != targetHide) {
            stillAnimating = true;
        }
    }

    // 3. Item presence springs, focus pill transitions & bounces
    bool anyErase = false;
    for (auto& item : g_items) {
        if (item.presence != item.targetPresence ||
            std::fabs(item.presenceVel) > 0.0005f) {
            if (StepCriticallyDampedSpring(item.presence, item.presenceVel,
                                           item.targetPresence, 18.0f, dt)) {
                stillAnimating = true;
            }
            item.presence = Clamp01(item.presence);
            if (item.targetPresence <= 0.0f && item.presence <= 0.01f) {
                anyErase = true;
            }
        }
        float targetFocus = item.focused ? 1.0f : 0.0f;
        if (item.focusAnim != targetFocus) {
            float k = 1.0f - std::exp(-effDt * 20.0f);
            item.focusAnim += (targetFocus - item.focusAnim) * k;
            if (std::fabs(targetFocus - item.focusAnim) < 0.004f) {
                item.focusAnim = targetFocus;
            } else {
                stillAnimating = true;
            }
        }
        if (item.launching || item.attention) {
            stillAnimating = true;
        }
    }

    if (anyErase) {
        for (auto it = g_items.begin(); it != g_items.end();) {
            if (it->targetPresence <= 0.0f && it->presence <= 0.01f) {
                DetachItemVisuals(*it);
                it = g_items.erase(it);
            } else {
                ++it;
            }
        }
    }

    // 4. Tooltip fade (100 ms)
    auto& tip = g_dock.tooltip;
    if (tip.opacity != tip.targetOpacity) {
        float step = effDt / 0.100f;
        if (tip.targetOpacity > tip.opacity) {
            tip.opacity = std::min(1.0f, tip.opacity + step);
        } else {
            tip.opacity = std::max(0.0f, tip.opacity - step);
        }
        if (tip.opacity != tip.targetOpacity) {
            stillAnimating = true;
        }
    }

    LayoutAndApplyDock(now);

    // Re-check tooltip target change (e.g. hover just started/stopped inside LayoutAndApplyDock)
    if (tip.opacity != tip.targetOpacity) {
        stillAnimating = true;
    }

    g_dock.animating = stillAnimating;
    g_dock.layoutDirty = false;
    g_liquidCapture.highRate.store(
        stillAnimating || HasOpenOverlayUi(),
        std::memory_order_relaxed);
}

// ============================================================================
// 12. Interactions (Launch, Activate, Minimize, Show in Explorer)
// ============================================================================

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
        ShowWindowAsync(hwnd, SW_RESTORE);
    }
    if (!SetForegroundWindow(hwnd)) {
        SwitchToThisWindow(hwnd, TRUE);
    }
}

void OpenTahoeSettingsPanel();
void OpenFolderStackPopup(const std::wstring& folderPath,
                          const std::wstring& folderName,
                          const std::wstring& stackId,
                          const D2D1_RECT_F& iconRectLocal);

bool LaunchAppItem(DockItem& item) {
    if (item.kind == DockItemKind::Finder) {
        ShellExecuteW(nullptr, L"open", L"explorer.exe", nullptr, nullptr,
                      SW_SHOWNORMAL);
        return true;
    }
    if (item.kind == DockItemKind::Apps) {
        if (g_settings.appsButtonAction == AppsButtonAction::AllApps) {
            ShellExecuteW(nullptr, L"open", L"explorer.exe",
                          L"shell:AppsFolder", nullptr, SW_SHOWNORMAL);
        } else {
            INPUT inputs[2]{};
            inputs[0].type = INPUT_KEYBOARD;
            inputs[0].ki.wVk = VK_LWIN;
            inputs[1].type = INPUT_KEYBOARD;
            inputs[1].ki.wVk = VK_LWIN;
            inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
            SendInput(2, inputs, sizeof(INPUT));
        }
        return true;
    }
    if (item.kind == DockItemKind::Trash) {
        ShellExecuteW(nullptr, L"open",
                      L"shell:::{645FF040-5081-101B-9F08-00AA002F954E}",
                      nullptr, nullptr, SW_SHOWNORMAL);
        return true;
    }
    if (item.kind == DockItemKind::Downloads ||
        item.kind == DockItemKind::FolderStack) {
        std::wstring p = (item.kind == DockItemKind::Downloads)
                             ? GetEffectiveDownloadsPath()
                             : item.path;
        if (!p.empty()) {
            ShellExecuteW(nullptr, L"open", p.c_str(), nullptr, nullptr,
                          SW_SHOWNORMAL);
        }
        return true;
    }

    bool launched = false;
    std::wstring resolvedShortcutPath;
    std::wstring resolvedShortcutArgs = item.args;
    std::wstring resolvedShortcutWorkdir = item.workdir;
    bool bypassWow64Shortcut = false;
    if (!item.shortcutPath.empty() && FileExistsNonDir(item.shortcutPath)) {
        if (auto resolved = ResolveShortcutFile(item.shortcutPath)) {
            if (!resolved->path.empty() &&
                NativeSystemPath(resolved->path) != resolved->path) {
                bypassWow64Shortcut = true;
                resolvedShortcutPath = NativeSystemPath(resolved->path);
                if (!resolved->args.empty()) {
                    resolvedShortcutArgs = resolved->args;
                }
                if (!resolved->workdir.empty()) {
                    resolvedShortcutWorkdir = resolved->workdir;
                }
            }
        }
        if (!bypassWow64Shortcut) {
            SHELLEXECUTEINFOW sei{};
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_NOASYNC;
            sei.lpVerb = L"open";
            sei.lpFile = item.shortcutPath.c_str();
            sei.nShow = SW_SHOWNORMAL;
            launched = ShellExecuteExW(&sei) != FALSE;
        }
    }
    if (!launched && (bypassWow64Shortcut || !item.path.empty())) {
        std::wstring execPath = bypassWow64Shortcut
                                    ? resolvedShortcutPath
                                    : NativeSystemPath(item.path);
        const std::wstring& execArgs =
            bypassWow64Shortcut ? resolvedShortcutArgs : item.args;
        const std::wstring& execWorkdir =
            bypassWow64Shortcut ? resolvedShortcutWorkdir : item.workdir;
        SHELLEXECUTEINFOW sei{};
        sei.cbSize = sizeof(sei);
        sei.fMask = SEE_MASK_NOASYNC | SEE_MASK_DOENVSUBST;
        sei.lpVerb = L"open";
        sei.lpFile = execPath.c_str();
        sei.lpParameters = execArgs.empty() ? nullptr : execArgs.c_str();
        sei.lpDirectory = execWorkdir.empty() ? nullptr : execWorkdir.c_str();
        sei.nShow = SW_SHOWNORMAL;
        launched = ShellExecuteExW(&sei) != FALSE;
    }
    if (!launched && !item.aumid.empty()) {
        winrt::com_ptr<IApplicationActivationManager> aam;
        if (SUCCEEDED(CoCreateInstance(CLSID_ApplicationActivationManager,
                                       nullptr, CLSCTX_LOCAL_SERVER,
                                       IID_PPV_ARGS(aam.put())))) {
            DWORD pid = 0;
            launched = SUCCEEDED(
                aam->ActivateApplication(item.aumid.c_str(), nullptr, AO_NONE,
                                         &pid));
        }
    }
    if (launched && g_settings.animateOpening &&
        (item.kind == DockItemKind::App || item.kind == DockItemKind::Recent)) {
        item.launching = true;
        item.launchStart = NowSeconds();
        RequestAnimationFrame();
    }
    return launched;
}

bool LaunchAppItemElevated(const DockItem& item) {
    if (item.kind == DockItemKind::Finder) {
        std::wstring ex = GetExplorerExePath();
        SHELLEXECUTEINFOW sei{};
        sei.cbSize = sizeof(sei);
        sei.fMask = SEE_MASK_NOASYNC;
        sei.lpVerb = L"runas";
        sei.lpFile = ex.c_str();
        sei.nShow = SW_SHOWNORMAL;
        return ShellExecuteExW(&sei) != FALSE;
    }
    std::wstring resolvedShortcutPath;
    std::wstring resolvedShortcutArgs = item.args;
    std::wstring resolvedShortcutWorkdir = item.workdir;
    bool bypassWow64Shortcut = false;
    if (!item.shortcutPath.empty() && FileExistsNonDir(item.shortcutPath)) {
        if (auto resolved = ResolveShortcutFile(item.shortcutPath)) {
            if (!resolved->path.empty() &&
                NativeSystemPath(resolved->path) != resolved->path) {
                bypassWow64Shortcut = true;
                resolvedShortcutPath = NativeSystemPath(resolved->path);
                if (!resolved->args.empty()) {
                    resolvedShortcutArgs = resolved->args;
                }
                if (!resolved->workdir.empty()) {
                    resolvedShortcutWorkdir = resolved->workdir;
                }
            }
        }
        if (!bypassWow64Shortcut) {
            SHELLEXECUTEINFOW sei{};
            sei.cbSize = sizeof(sei);
            sei.fMask = SEE_MASK_NOASYNC;
            sei.lpVerb = L"runas";
            sei.lpFile = item.shortcutPath.c_str();
            sei.nShow = SW_SHOWNORMAL;
            if (ShellExecuteExW(&sei)) {
                return true;
            }
        }
    }
    if (bypassWow64Shortcut || !item.path.empty()) {
        std::wstring execPath = bypassWow64Shortcut
                                    ? resolvedShortcutPath
                                    : NativeSystemPath(item.path);
        const std::wstring& execArgs =
            bypassWow64Shortcut ? resolvedShortcutArgs : item.args;
        const std::wstring& execWorkdir =
            bypassWow64Shortcut ? resolvedShortcutWorkdir : item.workdir;
        SHELLEXECUTEINFOW sei{};
        sei.cbSize = sizeof(sei);
        sei.fMask = SEE_MASK_NOASYNC | SEE_MASK_DOENVSUBST;
        sei.lpVerb = L"runas";
        sei.lpFile = execPath.c_str();
        sei.lpParameters = execArgs.empty() ? nullptr : execArgs.c_str();
        sei.lpDirectory = execWorkdir.empty() ? nullptr : execWorkdir.c_str();
        sei.nShow = SW_SHOWNORMAL;
        return ShellExecuteExW(&sei) != FALSE;
    }
    return false;
}

void HandleItemPrimaryClick(int idx) {
    if (idx < 0 || idx >= static_cast<int>(g_items.size())) {
        return;
    }
    auto& item = g_items[idx];
    if (item.kind == DockItemKind::Divider) {
        return;
    }
    CloseWindowPreviewFlyout();

    bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

    // Ctrl+Shift+Click => Run as administrator (Windows Taskbar shortcut)
    if (ctrl && shift &&
        (item.kind == DockItemKind::App || item.kind == DockItemKind::Recent ||
         item.kind == DockItemKind::Finder)) {
        LaunchAppItemElevated(item);
        return;
    }

    // Ctrl+Click => Show in File Explorer
    if (ctrl) {
        if (item.kind == DockItemKind::Downloads) {
            ShowInFileExplorer(GetEffectiveDownloadsPath());
        } else if (!item.path.empty()) {
            ShowInFileExplorer(item.path);
        } else if (!item.shortcutPath.empty()) {
            ShowInFileExplorer(item.shortcutPath);
        }
        return;
    }

    if (item.kind == DockItemKind::MinimizedWindow) {
        HWND target = item.minimizedHwnd;
        if (target && IsWindow(target)) {
            ActivateWindowForce(target);
        }
        return;
    }
    if (item.kind == DockItemKind::Downloads) {
        OpenFolderStackPopup(GetEffectiveDownloadsPath(), L"Downloads",
                             item.identityKey, item.iconRect);
        return;
    }
    if (item.kind == DockItemKind::FolderStack) {
        OpenFolderStackPopup(item.path, item.name, item.identityKey,
                             item.iconRect);
        return;
    }
    if (item.kind == DockItemKind::Apps || item.kind == DockItemKind::Trash) {
        LaunchAppItem(item);
        return;
    }

    // App / Recent / Finder (Shift+Click always opens a new instance/window)
    if (item.windows.empty() || shift) {
        LaunchAppItem(item);
        return;
    }

    HWND fg = GetForegroundWindow();
    bool isFg = false;
    bool allMin = true;
    for (HWND h : item.windows) {
        if (IsWindow(h)) {
            if (!IsIconic(h)) {
                allMin = false;
                if (h == fg) {
                    isFg = true;
                }
            }
        }
    }

    if (isFg) {
        if (g_settings.clickActiveToMinimize) {
            for (HWND h : item.windows) {
                if (IsWindow(h) && !IsIconic(h)) {
                    ShowWindowAsync(h, SW_MINIMIZE);
                }
            }
        } else if (item.windows.size() > 1) {
            HWND rootFg = GetAncestor(fg, GA_ROOTOWNER);
            int curIdx = -1;
            for (size_t i = 0; i < item.windows.size(); ++i) {
                if (item.windows[i] == fg || item.windows[i] == rootFg) {
                    curIdx = static_cast<int>(i);
                    break;
                }
            }
            for (size_t step = 1; step <= item.windows.size(); ++step) {
                size_t nextIdx =
                    (static_cast<size_t>(std::max(0, curIdx)) + step) %
                    item.windows.size();
                HWND cand = item.windows[nextIdx];
                if (cand != fg && IsWindow(cand)) {
                    ActivateWindowForce(cand);
                    break;
                }
            }
        }
        return;
    }

    if (allMin) {
        HWND target = (item.lastActive && IsWindow(item.lastActive))
                          ? item.lastActive
                          : item.windows.front();
        ActivateWindowForce(target);
        return;
    }

    if (g_settings.bringAllWindows) {
        HWND primary = (item.lastActive && IsWindow(item.lastActive))
                           ? item.lastActive
                           : item.windows.front();
        for (HWND h : item.windows) {
            if (h != primary && IsWindow(h)) {
                if (IsIconic(h)) {
                    ShowWindowAsync(h, SW_RESTORE);
                }
                SetWindowPos(h, HWND_TOP, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                                 SWP_ASYNCWINDOWPOS);
            }
        }
        ActivateWindowForce(primary);
    } else {
        HWND target = (item.lastActive && IsWindow(item.lastActive))
                          ? item.lastActive
                          : item.windows.front();
        ActivateWindowForce(target);
    }
}

int HitTestItemAtPoint(float lx, float ly) {
    for (size_t i = 0; i < g_items.size(); ++i) {
        const auto& item = g_items[i];
        if (item.kind == DockItemKind::Divider) {
            continue;
        }
        if (PointInRectF(item.iconRect, lx, ly)) {
            return static_cast<int>(i);
        }
    }
    // Fallback to slot hit inside shelf rect
    if (PointInRectF(g_dock.shelfRect, lx, ly)) {
        float coord = (g_settings.position == DockPosition::Bottom) ? lx : ly;
        for (size_t i = 0; i < g_items.size(); ++i) {
            const auto& item = g_items[i];
            if (item.kind != DockItemKind::Divider && coord >= item.slotStart &&
                coord <= item.slotEnd) {
                return static_cast<int>(i);
            }
        }
    }
    return -1;
}

int HitTestDividerAtPoint(float lx, float ly) {
    if (!PointInRectF(g_dock.shelfRect, lx, ly)) {
        return -1;
    }
    float coord = (g_settings.position == DockPosition::Bottom) ? lx : ly;
    float pad = 5.0f * g_dock.scale;
    for (size_t i = 0; i < g_items.size(); ++i) {
        const auto& item = g_items[i];
        if (item.kind == DockItemKind::Divider &&
            coord >= (item.slotStart - pad) && coord <= (item.slotEnd + pad)) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// ============================================================================
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
constexpr WCHAR kMenuClassName[] = L"WindhawkMacOS27MenuWnd";

void CloseAllMenus() {
    g_menus.subPopup.CloseWindow();
    g_menus.mainPopup.CloseWindow();
    CloseWindowPreviewFlyout();
    g_dock.modalActive = false;
    RequestAnimationFrame();
}

void PostDeferredDockAction(std::function<void()> fn) {
    g_menus.pendingAction = std::move(fn);
    if (g_dock.hwnd) {
        PostMessageW(g_dock.hwnd, kWmAppDeferredAction, 0, 0);
    }
}

void ApplyTaskbarHiding();
void RestoreTaskbar();

void ApplyLiveSettingsAfterOverride() {
    bool prevHide = g_settings.hideTaskbar;
    LoadSettings(false);
    if (prevHide && !g_settings.hideTaskbar) {
        RestoreTaskbar();
    } else if (g_settings.hideTaskbar) {
        ApplyTaskbarHiding();
    }
    g_dock.autoHideRevealed = !g_settings.autoHide;
    g_dock.cursorLeftTime = 0.0;
    UpdateGlassAppearance();
    ClearIconCache();
    g_dock.tooltip.currentText.clear();
    RebuildShadowSurface();
    ResizeDockWindowForMonitor();
    UpdateAppBarReservation();
    SyncPinnedAppsWithTaskbar(true);
    ReconcileDockItems(false);
    RequestAnimationFrame();
}

void PickCustomIconForKey(const std::wstring& storageKey,
                          const std::wstring& displayName) {
    g_dock.modalActive = true;
    WCHAR fileBuf[MAX_PATH * 2]{};
    std::wstring title = L"Choose Icon for " + displayName;
    static constexpr WCHAR kFilter[] =
        L"Icon & Image Files (*.png;*.icns;*.ico;*.jpg;*.jpeg;*.webp)\0"
        L"*.png;*.icns;*.ico;*.jpg;*.jpeg;*.webp\0"
        L"All Files (*.*)\0*.*\0";

    OPENFILENAMEW ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFilter = kFilter;
    ofn.lpstrFile = fileBuf;
    ofn.nMaxFile = ARRAYSIZE(fileBuf);
    ofn.lpstrTitle = title.c_str();
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR |
                OFN_EXPLORER;

    bool picked = GetOpenFileNameW(&ofn) != FALSE;
    g_dock.modalActive = false;

    if (picked && fileBuf[0]) {
        std::wstring persisted = PersistManagedCustomIcon(storageKey, fileBuf);
        if (persisted.empty()) {
            MessageBeep(MB_ICONWARNING);
            return;
        }
        std::wstring oldPath;
        if (auto it = g_customIcons.find(storageKey); it != g_customIcons.end()) {
            oldPath = it->second;
        }
        g_customIcons[storageKey] = persisted;
        SaveCustomIcons();
        if (!oldPath.empty() && NormalizePath(oldPath) != NormalizePath(persisted)) {
            MaybeDeleteUnreferencedManagedIcon(oldPath);
        }
        ClearIconCache();
        SyncItemIconsAndTheme();
        LayoutAndApplyDock(NowSeconds());
        RequestAnimationFrame();
    }
}

void ResetCustomIconForKey(const std::wstring& storageKey) {
    auto it = g_customIcons.find(storageKey);
    if (it != g_customIcons.end()) {
        std::wstring oldPath = std::move(it->second);
        g_customIcons.erase(it);
        SaveCustomIcons();
        MaybeDeleteUnreferencedManagedIcon(oldPath);
        ClearIconCache();
        SyncItemIconsAndTheme();
        RequestAnimationFrame();
    }
}

void ResetCustomIconForItem(const DockItem& item) {
    bool removed = false;
    std::vector<std::wstring> removedPaths;
    for (const auto& k : CollectCustomIconLookupKeys(item)) {
        auto it = g_customIcons.find(k);
        if (it != g_customIcons.end()) {
            removedPaths.push_back(std::move(it->second));
            g_customIcons.erase(it);
            removed = true;
        }
    }
    if (removed) {
        SaveCustomIcons();
        for (const auto& p : removedPaths) {
            MaybeDeleteUnreferencedManagedIcon(p);
        }
        ClearIconCache();
        SyncItemIconsAndTheme();
        RequestAnimationFrame();
    }
}

bool IsShortcutUninstallerName(const std::wstring& filename) {
    std::wstring low = ToLower(filename);
    return low.find(L"uninstall") != std::wstring::npos ||
           low.find(L"unins0") != std::wstring::npos;
}

std::wstring FindStartMenuShortcutForApp(const std::wstring& exePath,
                                         const std::wstring& aumid = {}) {
    std::wstring wantNorm = !exePath.empty() ? NormalizePath(exePath) : L"";
    bool wantAumid = IsRealAumid(aumid);
    if (wantNorm.empty() && !wantAumid) {
        return {};
    }
    std::wstring wantBase;
    if (!wantNorm.empty()) {
        const wchar_t* fn = PathFindFileNameW(wantNorm.c_str());
        if (fn && *fn) {
            wantBase = ToLower(fn);
        }
    }

    std::vector<std::wstring> roots;
    for (PCWSTR envVar : {L"%APPDATA%\\Microsoft\\Windows\\Start Menu\\Programs",
                          L"%PROGRAMDATA%\\Microsoft\\Windows\\Start Menu\\Programs"}) {
        WCHAR buf[MAX_PATH * 2]{};
        if (ExpandEnvironmentStringsW(envVar, buf, ARRAYSIZE(buf)) > 0 && buf[0]) {
            roots.emplace_back(buf);
        }
    }

    int scanned = 0;
    std::wstring matched;
    std::function<void(const std::wstring&, int)> scanDir =
        [&](const std::wstring& dir, int depth) {
            if (!matched.empty() || depth > 3 || scanned >= 1200) {
                return;
            }
            WIN32_FIND_DATAW fd{};
            HANDLE hFind =
                FindFirstFileW((dir + L"\\*").c_str(), &fd);
            if (hFind == INVALID_HANDLE_VALUE) {
                return;
            }
            do {
                if (wcscmp(fd.cFileName, L".") == 0 ||
                    wcscmp(fd.cFileName, L"..") == 0) {
                    continue;
                }
                std::wstring full = dir + L"\\" + fd.cFileName;
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
                        scanDir(full, depth + 1);
                    }
                    continue;
                }
                std::wstring fn = fd.cFileName;
                if (ToLower(fn).ends_with(L".lnk") &&
                    !IsShortcutUninstallerName(fn)) {
                    ++scanned;
                    if (auto sa = ResolveShortcutFile(full)) {
                        if (wantAumid && !sa->aumid.empty() &&
                            EqualsIgnoreCase(sa->aumid, aumid)) {
                            matched = full;
                            break;
                        }
                        if (!wantNorm.empty() && !sa->path.empty() &&
                            NormalizePath(sa->path) == wantNorm) {
                            matched = full;
                            break;
                        }
                        if (!wantBase.empty() && !sa->path.empty()) {
                            const wchar_t* sfn =
                                PathFindFileNameW(sa->path.c_str());
                            if (sfn && ToLower(sfn) == L"update.exe" &&
                                ToLower(sa->args).find(wantBase) !=
                                    std::wstring::npos) {
                                matched = full;
                                break;
                            }
                        }
                    }
                    if (scanned >= 1200) {
                        break;
                    }
                }
            } while (matched.empty() && FindNextFileW(hFind, &fd));
            FindClose(hFind);
        };

    for (const auto& r : roots) {
        if (!matched.empty()) break;
        scanDir(r, 0);
    }
    return matched;
}

PIDLIST_ABSOLUTE BuildPidlForApp(const std::wstring& shortcutPath,
                                 const std::wstring& path,
                                 const std::wstring& aumid) {
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (!shortcutPath.empty() && FileExistsNonDir(shortcutPath)) {
        if (SUCCEEDED(SHParseDisplayName(shortcutPath.c_str(), nullptr, &pidl,
                                         0, nullptr)) &&
            pidl) {
            return pidl;
        }
    }
    if (!aumid.empty()) {
        winrt::com_ptr<IShellItem> si;
        if (SUCCEEDED(SHCreateItemInKnownFolder(kFolderIdAppsFolder, 0,
                                                aumid.c_str(),
                                                IID_PPV_ARGS(si.put()))) &&
            si) {
            if (SUCCEEDED(SHGetIDListFromObject(si.get(), &pidl)) && pidl) {
                return pidl;
            }
        }
    }
    if (!path.empty() || IsRealAumid(aumid)) {
        std::wstring startLnk = FindStartMenuShortcutForApp(path, aumid);
        if (!startLnk.empty() &&
            SUCCEEDED(SHParseDisplayName(startLnk.c_str(), nullptr, &pidl, 0,
                                         nullptr)) &&
            pidl) {
            return pidl;
        }
    }
    if (!path.empty() && FileExistsNonDir(path)) {
        if (SUCCEEDED(
                SHParseDisplayName(path.c_str(), nullptr, &pidl, 0, nullptr)) &&
            pidl) {
            return pidl;
        }
    }
    return nullptr;
}

void PinItemToDock(const DockItem& item) {
    for (const auto& pa : g_pinnedApps) {
        if (AppsMatch(pa.aumid, pa.path, item.aumid, item.path) &&
            (item.shortcutPath.empty() ||
             NormalizePath(pa.shortcutPath) ==
                 NormalizePath(item.shortcutPath))) {
            return;
        }
    }

    auto pl = CreateTaskbarPinnedList();
    bool pinnedInTaskbar = false;
    HRESULT lastHr = E_FAIL;

    if (pl) {
        ULONG ccBefore = 0;
        pl->GetChangeCount(&ccBefore);

        auto tryModifyPin = [&](PIDLIST_ABSOLUTE pidl) -> bool {
            if (!pidl) return false;
            lastHr = pl->Modify(nullptr, pidl, PLMC_EXPLORER);
            ULONG ccAfter = ccBefore;
            pl->GetChangeCount(&ccAfter);
            bool ok = SUCCEEDED(lastHr) &&
                      (ccAfter != ccBefore || pl->IsPinned(pidl) == S_OK);
            ccBefore = ccAfter;
            CoTaskMemFree(pidl);
            return ok;
        };

        auto makePathPidl = [](const std::wstring& fsPath) -> PIDLIST_ABSOLUTE {
            if (fsPath.empty() || !FileExistsNonDir(fsPath)) return nullptr;
            PIDLIST_ABSOLUTE p = nullptr;
            if (SUCCEEDED(SHParseDisplayName(fsPath.c_str(), nullptr, &p, 0,
                                             nullptr)) &&
                p) {
                return p;
            }
            return nullptr;
        };

        // 1. Existing shortcutPath
        if (!item.shortcutPath.empty()) {
            pinnedInTaskbar = tryModifyPin(makePathPidl(item.shortcutPath));
        }

        // 2. AppsFolder AUMID (works cleanly for Store/UWP and registered Win32)
        if (!pinnedInTaskbar && !item.aumid.empty()) {
            winrt::com_ptr<IShellItem> si;
            PIDLIST_ABSOLUTE p = nullptr;
            if (SUCCEEDED(SHCreateItemInKnownFolder(kFolderIdAppsFolder, 0,
                                                    item.aumid.c_str(),
                                                    IID_PPV_ARGS(si.put()))) &&
                si && SUCCEEDED(SHGetIDListFromObject(si.get(), &p)) && p) {
                pinnedInTaskbar = tryModifyPin(p);
            }
        }

        // 3. Matching Start Menu .lnk
        std::wstring resolvedPath =
            FileExistsNonDir(item.path) ? item.path : ResolveWow64Path(item.path);
        if (!pinnedInTaskbar && (!resolvedPath.empty() || IsRealAumid(item.aumid))) {
            std::wstring startLnk =
                FindStartMenuShortcutForApp(resolvedPath, item.aumid);
            if (!startLnk.empty()) {
                pinnedInTaskbar = tryModifyPin(makePathPidl(startLnk));
            }
        }

        // 4. Synthesized temporary .lnk in %TEMP%\<SanitizedName>.lnk
        if (!pinnedInTaskbar && !resolvedPath.empty()) {
            WCHAR tempDir[MAX_PATH * 2]{};
            DWORD tlen = GetTempPathW(ARRAYSIZE(tempDir), tempDir);
            if (tlen > 0 && tlen < ARRAYSIZE(tempDir)) {
                auto sanitizeFileName = [](const std::wstring& raw) -> std::wstring {
                    std::wstring out;
                    out.reserve(raw.size());
                    for (wchar_t ch : raw) {
                        if (ch < 32 || wcschr(L"<>:\"/\\|?*", ch) != nullptr) {
                            continue;
                        }
                        out.push_back(ch);
                    }
                    while (!out.empty() &&
                           (out.back() == L' ' || out.back() == L'.')) {
                        out.pop_back();
                    }
                    return out;
                };
                std::wstring safeName = sanitizeFileName(item.name);
                if (safeName.empty()) {
                    const wchar_t* fn = PathFindFileNameW(resolvedPath.c_str());
                    std::wstring stem = fn ? fn : L"";
                    size_t dot = stem.find_last_of(L'.');
                    if (dot != std::wstring::npos) {
                        stem.resize(dot);
                    }
                    safeName = sanitizeFileName(stem);
                }
                if (safeName.empty()) {
                    safeName = L"Application";
                }
                std::wstring subDir = std::wstring(tempDir) + L"wh_tahoe_dock_" +
                                      std::to_wstring(GetCurrentProcessId()) +
                                      L"_" + std::to_wstring(GetTickCount64());
                CreateDirectoryW(subDir.c_str(), nullptr);
                std::wstring tempLnk = subDir + L"\\" + safeName + L".lnk";

                winrt::com_ptr<IShellLinkW> lnk;
                if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr,
                                               CLSCTX_INPROC_SERVER,
                                               IID_PPV_ARGS(lnk.put()))) &&
                    lnk) {
                    lnk->SetPath(resolvedPath.c_str());
                    if (!item.args.empty()) {
                        lnk->SetArguments(item.args.c_str());
                    }
                    if (!item.workdir.empty()) {
                        lnk->SetWorkingDirectory(item.workdir.c_str());
                    } else {
                        std::wstring dir = resolvedPath;
                        size_t sep = dir.find_last_of(L"\\/");
                        if (sep != std::wstring::npos) {
                            dir.resize(sep);
                            lnk->SetWorkingDirectory(dir.c_str());
                        }
                    }
                    if (!item.name.empty()) {
                        lnk->SetDescription(item.name.c_str());
                    }
                    lnk->SetIconLocation(resolvedPath.c_str(), 0);
                    if (IsRealAumid(item.aumid)) {
                        winrt::com_ptr<IPropertyStore> ps;
                        if (SUCCEEDED(lnk->QueryInterface(
                                IID_PPV_ARGS(ps.put()))) &&
                            ps) {
                            PROPVARIANT pv{};
                            pv.vt = VT_LPWSTR;
                            pv.pwszVal = const_cast<LPWSTR>(item.aumid.c_str());
                            ps->SetValue(kPkeyAppUserModelId, pv);
                            ps->Commit();
                        }
                    }
                    auto pf = lnk.try_as<IPersistFile>();
                    if (pf && SUCCEEDED(pf->Save(tempLnk.c_str(), TRUE))) {
                        pf = nullptr;
                        lnk = nullptr;
                        pinnedInTaskbar = tryModifyPin(makePathPidl(tempLnk));
                        DeleteFileW(tempLnk.c_str());
                    }
                }
                RemoveDirectoryW(subDir.c_str());
            }
        }

        // 5. Fallback direct .exe PIDL
        if (!pinnedInTaskbar && !resolvedPath.empty()) {
            pinnedInTaskbar = tryModifyPin(makePathPidl(resolvedPath));
        }
    }

    if (pinnedInTaskbar) {
        g_recentApps.erase(
            std::remove_if(g_recentApps.begin(), g_recentApps.end(),
                           [&](const StoredApp& r) {
                               return AppsMatch(r.aumid, r.path, item.aumid,
                                                item.path);
                           }),
            g_recentApps.end());
        SaveRecentApps();

        SyncPinnedAppsWithTaskbar(true);
        if (!LoadPinnedOrderOverlay().empty()) {
            SavePinnedOrderOverlay(g_pinnedApps);
        }
        ReconcileDockItems(false);
        RequestAnimationFrame();
        if (g_dock.hwnd) {
            SetTimer(g_dock.hwnd, kTimerTaskbarPinSync, 250, nullptr);
        }
        return;
    }

    if (pl) {
        Wh_Log(L"Pin to Taskbar failed for [%s] (path=%s aumid=%s hr=0x%08X)",
               item.name.c_str(), item.path.c_str(), item.aumid.c_str(),
               static_cast<unsigned>(lastHr));
        MessageBeep(MB_ICONWARNING);
        return;
    }

    // Legacy fallback only when IPinnedList3 COM server is unavailable
    StoredApp sa;
    sa.isAumid = item.path.empty();
    sa.aumid = item.aumid;
    sa.path = item.path;
    sa.shortcutPath = item.shortcutPath;
    sa.taskbarAppId = item.taskbarAppId;
    sa.args = item.args;
    sa.workdir = item.workdir;
    sa.name = item.name;
    g_pinnedApps.push_back(std::move(sa));
    SavePinnedApps();
    ReconcileDockItems(false);
    RequestAnimationFrame();
}

void UnpinItemFromDock(const DockItem& item) {
    std::vector<BYTE> pidlBytes = item.taskbarPidl;
    std::wstring tbAppId = item.taskbarAppId;
    std::wstring scPath = item.shortcutPath;
    std::wstring exePath = item.path;
    std::wstring aumid = item.aumid;
    std::wstring idKey = item.identityKey;

    auto harvestFrom = [&](const StoredApp& pa) {
        if (pidlBytes.empty()) pidlBytes = pa.taskbarPidl;
        if (tbAppId.empty()) tbAppId = pa.taskbarAppId;
        if (scPath.empty()) scPath = pa.shortcutPath;
        if (exePath.empty()) exePath = pa.path;
        if (aumid.empty()) aumid = pa.aumid;
    };
    bool harvested = false;
    for (const auto& pa : g_pinnedApps) {
        if (pa.IdentityKey() == idKey ||
            (!scPath.empty() && !pa.shortcutPath.empty() &&
             NormalizePath(pa.shortcutPath) == NormalizePath(scPath))) {
            harvestFrom(pa);
            harvested = true;
            break;
        }
    }
    if (!harvested) {
        for (const auto& pa : g_pinnedApps) {
            if (AppsMatch(pa.aumid, pa.path, item.aumid, item.path)) {
                harvestFrom(pa);
                break;
            }
        }
    }

    auto pl = CreateTaskbarPinnedList();
    bool unpinnedInTaskbar = false;
    HRESULT lastHr = E_FAIL;
    if (pl) {
        ULONG ccBefore = 0;
        pl->GetChangeCount(&ccBefore);

        auto tryUnpinPidl = [&](PCIDLIST_ABSOLUTE p) -> bool {
            if (!p) return false;
            lastHr = pl->Modify(p, nullptr, PLMC_EXPLORER);
            ULONG ccAfter = ccBefore;
            pl->GetChangeCount(&ccAfter);
            bool ok = SUCCEEDED(lastHr) &&
                      (ccAfter != ccBefore || pl->IsPinned(p) == S_FALSE);
            ccBefore = ccAfter;
            return ok;
        };

        auto matchId = [&](const std::wstring& cand) -> bool {
            if (cand.empty()) return false;
            return (!tbAppId.empty() && EqualsIgnoreCase(cand, tbAppId)) ||
                   (!aumid.empty() && EqualsIgnoreCase(cand, aumid));
        };

        // 1. Live lookup in IPinnedList3::EnumObjects() for exact matching taskbar PIDL
        winrt::com_ptr<IEnumFullIDList> en;
        if (SUCCEEDED(pl->EnumObjects(en.put())) && en) {
            PIDLIST_ABSOLUTE livePidl = nullptr;
            ULONG fetched = 0;
            while (!unpinnedInTaskbar &&
                   en->Next(1, &livePidl, &fetched) == S_OK && fetched > 0 &&
                   livePidl) {
                bool match = false;
                UINT lsz = ILGetSize(livePidl);
                if (!pidlBytes.empty() && lsz == pidlBytes.size() &&
                    memcmp(livePidl, pidlBytes.data(), lsz) == 0) {
                    match = true;
                }

                std::wstring liveAppId;
                PWSTR rawAppId = nullptr;
                if (SUCCEEDED(pl->GetAppIDForPinnedItem(livePidl, &rawAppId)) &&
                    rawAppId) {
                    liveAppId = rawAppId;
                    CoTaskMemFree(rawAppId);
                }

                winrt::com_ptr<IShellItem> si;
                std::wstring fsPath;
                std::wstring relName;
                std::wstring parseName;
                if (SUCCEEDED(SHCreateItemFromIDList(
                        livePidl, IID_PPV_ARGS(si.put()))) &&
                    si) {
                    auto grabName = [&](SIGDN sig) -> std::wstring {
                        PWSTR p = nullptr;
                        if (SUCCEEDED(si->GetDisplayName(sig, &p)) && p) {
                            std::wstring res = p;
                            CoTaskMemFree(p);
                            return res;
                        }
                        return {};
                    };
                    fsPath = grabName(SIGDN_FILESYSPATH);
                    relName = grabName(SIGDN_PARENTRELATIVEPARSING);
                    parseName = grabName(SIGDN_DESKTOPABSOLUTEPARSING);
                }

                if (!match && !scPath.empty() && !fsPath.empty() &&
                    NormalizePath(fsPath) == NormalizePath(scPath)) {
                    match = true;
                }
                if (!match && matchId(liveAppId)) {
                    match = true;
                }
                if (!match) {
                    std::wstring normParse = parseName;
                    if (normParse.rfind(L"::{", 0) == 0) {
                        size_t bs = normParse.find(L'\\');
                        if (bs != std::wstring::npos &&
                            bs + 1 < normParse.size()) {
                            normParse = normParse.substr(bs + 1);
                        }
                    }
                    if (matchId(relName) || matchId(normParse)) {
                        match = true;
                    }
                }
                if (!match && !fsPath.empty() &&
                    !IsDistinctAppAumid(aumid) &&
                    !IsDistinctAppAumid(tbAppId)) {
                    if (auto resolved = ResolveShortcutFile(fsPath)) {
                        const std::wstring& effAumid =
                            !resolved->aumid.empty() ? resolved->aumid
                                                     : liveAppId;
                        if (AppsMatch(effAumid, resolved->path, aumid,
                                      exePath)) {
                            match = true;
                        }
                    }
                }
                if (match) {
                    unpinnedInTaskbar = tryUnpinPidl(livePidl);
                }
                CoTaskMemFree(livePidl);
                livePidl = nullptr;
            }
        }

        // 2. Fallback: GetPinnedItemForAppID
        if (!unpinnedInTaskbar && (!tbAppId.empty() || !aumid.empty())) {
            PIDLIST_ABSOLUTE found = nullptr;
            PCWSTR idLookup = !tbAppId.empty() ? tbAppId.c_str() : aumid.c_str();
            if (SUCCEEDED(pl->GetPinnedItemForAppID(idLookup, &found)) &&
                found) {
                unpinnedInTaskbar = tryUnpinPidl(found);
                CoTaskMemFree(found);
            }
        }

        // 3. Fallback: stored raw PIDL bytes
        if (!unpinnedInTaskbar && !pidlBytes.empty()) {
            PIDLIST_ABSOLUTE mem =
                static_cast<PIDLIST_ABSOLUTE>(CoTaskMemAlloc(pidlBytes.size()));
            if (mem) {
                memcpy(mem, pidlBytes.data(), pidlBytes.size());
                unpinnedInTaskbar = tryUnpinPidl(mem);
                CoTaskMemFree(mem);
            }
        }

        // 4. Fallback: built PIDL
        if (!unpinnedInTaskbar) {
            if (PIDLIST_ABSOLUTE built = BuildPidlForApp(scPath, exePath, aumid)) {
                unpinnedInTaskbar = tryUnpinPidl(built);
                CoTaskMemFree(built);
            }
        }
    }

    if (pl && !unpinnedInTaskbar) {
        Wh_Log(L"Unpin from Taskbar failed for [%s] (hr=0x%08X)",
               item.name.c_str(), static_cast<unsigned>(lastHr));
        MessageBeep(MB_ICONWARNING);
        return;
    }

    size_t before = g_pinnedApps.size();
    g_pinnedApps.erase(
        std::remove_if(
            g_pinnedApps.begin(), g_pinnedApps.end(),
            [&](const StoredApp& pa) {
                return pa.IdentityKey() == idKey ||
                       (!scPath.empty() && !pa.shortcutPath.empty() &&
                        NormalizePath(pa.shortcutPath) ==
                            NormalizePath(scPath)) ||
                       AppsMatch(pa.aumid, pa.path, item.aumid, item.path);
            }),
        g_pinnedApps.end());
    if (g_pinnedApps.size() != before) {
        if (!LoadPinnedOrderOverlay().empty()) {
            SavePinnedOrderOverlay(g_pinnedApps);
        }
        SavePinnedApps();
    }
    if (unpinnedInTaskbar) {
        SyncPinnedAppsWithTaskbar(true);
        if (g_dock.hwnd) {
            SetTimer(g_dock.hwnd, kTimerTaskbarPinSync, 250, nullptr);
        }
    }
    ReconcileDockItems(false);
    RequestAnimationFrame();
}

struct JumpListEntry {
    std::wstring name;
    std::wstring targetPath;
    std::wstring args;
    std::wstring workdir;
};

std::vector<JumpListEntry> QueryWindowsJumpList(const std::wstring& appId,
                                                size_t maxItems = 6) {
    std::vector<JumpListEntry> out;
    if (appId.empty()) {
        return out;
    }
    winrt::com_ptr<IApplicationDocumentLists> adl;
    if (FAILED(CoCreateInstance(kClsidApplicationDocumentListsLocal, nullptr,
                                CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(adl.put()))) ||
        !adl) {
        return out;
    }
    if (FAILED(adl->SetAppID(appId.c_str()))) {
        return out;
    }

    auto appendFromList = [&](APPDOCLISTTYPE listType) {
        if (out.size() >= maxItems) {
            return;
        }
        winrt::com_ptr<IObjectArray> arr;
        if (FAILED(adl->GetList(listType, static_cast<UINT>(maxItems),
                                IID_PPV_ARGS(arr.put()))) ||
            !arr) {
            return;
        }
        UINT count = 0;
        if (FAILED(arr->GetCount(&count))) {
            return;
        }
        for (UINT i = 0; i < count && out.size() < maxItems; ++i) {
            JumpListEntry e;
            winrt::com_ptr<IShellItem> si;
            if (SUCCEEDED(arr->GetAt(i, IID_PPV_ARGS(si.put()))) && si) {
                PWSTR fs = nullptr;
                if (SUCCEEDED(si->GetDisplayName(SIGDN_FILESYSPATH, &fs)) &&
                    fs) {
                    e.targetPath = fs;
                    CoTaskMemFree(fs);
                }
                PWSTR disp = nullptr;
                if (SUCCEEDED(si->GetDisplayName(SIGDN_NORMALDISPLAY, &disp)) &&
                    disp) {
                    e.name = disp;
                    CoTaskMemFree(disp);
                }
            } else {
                winrt::com_ptr<IShellLinkW> lnk;
                if (SUCCEEDED(arr->GetAt(i, IID_PPV_ARGS(lnk.put()))) && lnk) {
                    WCHAR tbuf[MAX_PATH * 2]{};
                    WCHAR abuf[1024]{};
                    WCHAR wbuf[MAX_PATH * 2]{};
                    lnk->GetPath(tbuf, ARRAYSIZE(tbuf), nullptr, 0);
                    if (!tbuf[0]) {
                        lnk->GetPath(tbuf, ARRAYSIZE(tbuf), nullptr,
                                     SLGP_RAWPATH);
                    }
                    lnk->GetArguments(abuf, ARRAYSIZE(abuf));
                    lnk->GetWorkingDirectory(wbuf, ARRAYSIZE(wbuf));
                    std::wstring resolved = ResolveWow64Path(tbuf);
                    if (resolved.empty() && tbuf[0] &&
                        GetFileAttributesW(tbuf) != INVALID_FILE_ATTRIBUTES) {
                        resolved = tbuf;
                    }
                    e.targetPath = resolved;
                    e.args = abuf;
                    e.workdir = wbuf;
                    if (!e.targetPath.empty()) {
                        const wchar_t* fn =
                            PathFindFileNameW(e.targetPath.c_str());
                        e.name = (fn && *fn) ? fn : e.targetPath;
                    }
                }
            }
            if (e.targetPath.empty() ||
                GetFileAttributesW(e.targetPath.c_str()) ==
                    INVALID_FILE_ATTRIBUTES) {
                continue;
            }
            if (e.name.empty()) {
                const wchar_t* fn = PathFindFileNameW(e.targetPath.c_str());
                e.name = (fn && *fn) ? fn : e.targetPath;
            }
            bool dup = false;
            std::wstring normTarget = NormalizePath(e.targetPath);
            for (const auto& prev : out) {
                if (NormalizePath(prev.targetPath) == normTarget &&
                    EqualsIgnoreCase(prev.args, e.args)) {
                    dup = true;
                    break;
                }
            }
            if (!dup) {
                out.push_back(std::move(e));
            }
        }
    };

    appendFromList(ADLT_RECENT);
    appendFromList(ADLT_FREQUENT);
    return out;
}

void LaunchJumpListEntry(const JumpListEntry& e) {
    if (e.targetPath.empty()) {
        return;
    }
    SHELLEXECUTEINFOW sei{};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_NOASYNC;
    sei.lpVerb = L"open";
    sei.lpFile = e.targetPath.c_str();
    sei.lpParameters = e.args.empty() ? nullptr : e.args.c_str();
    sei.lpDirectory = e.workdir.empty() ? nullptr : e.workdir.c_str();
    sei.nShow = SW_SHOWNORMAL;
    ShellExecuteExW(&sei);
}

bool IsProtectedSystemProcess(DWORD pid) {
    if (pid == 0 || pid == GetCurrentProcessId()) {
        return true;
    }
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) {
        return false;
    }
    WCHAR exePath[MAX_PATH * 2]{};
    DWORD len = ARRAYSIZE(exePath);
    bool isProtected = false;
    if (QueryFullProcessImageNameW(hProc, 0, exePath, &len) && len > 0) {
        const wchar_t* base = PathFindFileNameW(exePath);
        if (base && (_wcsicmp(base, L"explorer.exe") == 0 ||
                     _wcsicmp(base, L"applicationframehost.exe") == 0)) {
            isProtected = true;
        }
    }
    CloseHandle(hProc);
    return isProtected;
}

void QuitItemWindows(const std::vector<HWND>& hwnds, bool forceQuit) {
    if (forceQuit) {
        std::unordered_set<DWORD> pids;
        for (HWND h : hwnds) {
            if (!IsWindow(h)) {
                continue;
            }
            DWORD pid = 0;
            GetWindowThreadProcessId(h, &pid);
            if (!pid) {
                continue;
            }
            WCHAR cls[64]{};
            GetClassNameW(h, cls, ARRAYSIZE(cls));
            if (_wcsicmp(cls, L"ApplicationFrameWindow") == 0) {
                DWORD childPid = FindCoreWindowChildPid(h, pid);
                if (childPid == 0 || childPid == pid) {
                    continue;
                }
                pid = childPid;
            }
            if (!IsProtectedSystemProcess(pid)) {
                pids.insert(pid);
            }
        }
        for (DWORD pid : pids) {
            if (HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pid)) {
                TerminateProcess(hProc, 1);
                CloseHandle(hProc);
            }
        }
    } else {
        for (HWND h : hwnds) {
            if (IsWindow(h)) {
                PostMessageW(h, WM_CLOSE, 0, 0);
            }
        }
    }
}

void HideItemWindows(const std::vector<HWND>& hwnds) {
    for (HWND h : hwnds) {
        if (IsWindow(h) && !IsIconic(h)) {
            ShowWindowAsync(h, SW_MINIMIZE);
        }
    }
}

void ShowAllItemWindows(const std::vector<HWND>& hwnds, HWND lastActive) {
    HWND primary = (lastActive && IsWindow(lastActive))
                       ? lastActive
                       : (hwnds.empty() ? nullptr : hwnds.front());
    for (HWND h : hwnds) {
        if (h != primary && IsWindow(h)) {
            if (IsIconic(h)) {
                ShowWindowAsync(h, SW_RESTORE);
            }
            SetWindowPos(h, HWND_TOP, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                             SWP_ASYNCWINDOWPOS);
        }
    }
    if (primary) {
        ActivateWindowForce(primary);
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

std::wstring GetWindowDisplayTitle(HWND hwnd) {
    WCHAR buf[128]{};
    if (IsWindow(hwnd) && GetWindowTextW(hwnd, buf, ARRAYSIZE(buf)) > 0) {
        return buf;
    }
    return L"Window";
}

void OpenContextMenuForDock(POINT ptScreen, float lx, float ly) {
    CloseAllMenus();

    const bool altHeld = (GetKeyState(VK_MENU) & 0x8000) != 0;
    int idx = HitTestItemAtPoint(lx, ly);

    // Anchor nicely above icon (bottom dock) or at cursor (left/right/background)
    POINT anchor = ptScreen;
    bool alignAbove = (g_settings.position == DockPosition::Bottom);
    RECT wndRect{};
    GetWindowRect(g_dock.hwnd, &wndRect);

    if (idx >= 0 && idx < static_cast<int>(g_items.size())) {
        const auto& it = g_items[idx];
        if (g_settings.position == DockPosition::Bottom) {
            anchor.x =
                wndRect.left + static_cast<LONG>((it.iconRect.left + it.iconRect.right) * 0.5f) -
                static_cast<LONG>(80.0f * g_dock.scale);
            anchor.y =
                wndRect.top + static_cast<LONG>(it.iconRect.top - 8.0f * g_dock.scale);
        } else if (g_settings.position == DockPosition::Left) {
            anchor.x =
                wndRect.left + static_cast<LONG>(it.iconRect.right + 8.0f * g_dock.scale);
            anchor.y = wndRect.top + static_cast<LONG>(it.iconRect.top);
            alignAbove = false;
        } else {
            anchor.x =
                wndRect.left + static_cast<LONG>(it.iconRect.left - 180.0f * g_dock.scale);
            anchor.y = wndRect.top + static_cast<LONG>(it.iconRect.top);
            alignAbove = false;
        }
    } else if (g_settings.position == DockPosition::Bottom) {
        anchor.y = wndRect.top + static_cast<LONG>(g_dock.shelfRect.top - 8.0f * g_dock.scale);
    }

    std::vector<MenuEntry> entries;
    auto addSep = [&]() {
        if (!entries.empty() &&
            entries.back().type != MenuEntry::Type::Separator) {
            entries.push_back(MenuEntry{.type = MenuEntry::Type::Separator});
        }
    };

    if (idx >= 0 && idx < static_cast<int>(g_items.size()) &&
        g_items[idx].kind != DockItemKind::Divider) {
        // Snapshot item identity/data safely so callbacks never dangle
        DockItem snap = g_items[idx];
        std::wstring iconKey = CustomIconStorageKey(snap, false);
        bool hasIcon = HasUserCustomIconForItem(snap, false);
        HWND fg = GetForegroundWindow();

        if (snap.kind == DockItemKind::Trash) {
            bool full = IsRecycleBinNonEmpty();
            bool hasFullIcon = HasUserCustomIcon(L"fixed:trash_full");
            entries.push_back({MenuEntry::Type::Action, L"Open", false, true,
                               [] {
                                   ShellExecuteW(
                                       nullptr, L"open",
                                       L"shell:::{645FF040-5081-101B-9F08-00AA002F954E}",
                                       nullptr, nullptr, SW_SHOWNORMAL);
                               },
                               {}});
            entries.push_back({MenuEntry::Type::Action, L"Empty Trash", false,
                               full,
                               [] {
                                   SHEmptyRecycleBinW(nullptr, nullptr, 0);
                                   InvalidateIconCacheForPrefixes({L"fixed:trash"});
                                   SyncItemIconsAndTheme();
                                   RequestAnimationFrame();
                               },
                               {}});
            addSep();
            entries.push_back({MenuEntry::Type::Action, L"Change Empty Icon\u2026",
                               false, true,
                               [] {
                                   PickCustomIconForKey(L"fixed:trash",
                                                        L"Trash (Empty)");
                               },
                               {}});
            entries.push_back({MenuEntry::Type::Action, L"Change Full Icon\u2026",
                               false, true,
                               [] {
                                   PickCustomIconForKey(L"fixed:trash_full",
                                                        L"Trash (Full)");
                               },
                               {}});
            entries.push_back({MenuEntry::Type::Action, L"Reset Trash Icons",
                               false, hasIcon || hasFullIcon,
                               [] {
                                   std::wstring pEmpty, pFull;
                                   if (auto it = g_customIcons.find(L"fixed:trash");
                                       it != g_customIcons.end()) {
                                       pEmpty = std::move(it->second);
                                       g_customIcons.erase(it);
                                   }
                                   if (auto it = g_customIcons.find(L"fixed:trash_full");
                                       it != g_customIcons.end()) {
                                       pFull = std::move(it->second);
                                       g_customIcons.erase(it);
                                   }
                                   SaveCustomIcons();
                                   MaybeDeleteUnreferencedManagedIcon(pEmpty);
                                   MaybeDeleteUnreferencedManagedIcon(pFull);
                                   InvalidateIconCacheForPrefixes({L"fixed:trash"});
                                   SyncItemIconsAndTheme();
                                   RequestAnimationFrame();
                               },
                               {}});
        } else if (snap.kind == DockItemKind::MinimizedWindow) {
            HWND target = snap.minimizedHwnd;
            entries.push_back({MenuEntry::Type::Action, L"Restore Window",
                               false, target && IsWindow(target),
                               [target] {
                                   if (target && IsWindow(target)) {
                                       ActivateWindowForce(target);
                                   }
                               },
                               {}});
            entries.push_back({MenuEntry::Type::Action, L"Close Window", false,
                               target && IsWindow(target),
                               [target] {
                                   if (target && IsWindow(target)) {
                                       PostMessageW(target, WM_CLOSE, 0, 0);
                                   }
                               },
                               {}});
        } else if (snap.kind == DockItemKind::Downloads ||
                   snap.kind == DockItemKind::FolderStack) {
            bool isDl = (snap.kind == DockItemKind::Downloads);
            std::wstring fPath =
                isDl ? GetEffectiveDownloadsPath() : snap.path;
            std::wstring fName = isDl ? L"Downloads" : snap.name;
            int curView = 0, curSort = 0;
            if (isDl) {
                GetDownloadsStackConfig(curView, curSort);
            } else {
                for (const auto& pf : g_pinnedFolders) {
                    if (NormalizePath(pf.path) == NormalizePath(fPath)) {
                        curView = pf.viewMode;
                        curSort = pf.sortMode;
                        break;
                    }
                }
            }
            auto setCfg = [isDl, fPath](int v, int so) {
                if (isDl) {
                    SetDownloadsStackConfig(v, so);
                } else {
                    for (auto& pf : g_pinnedFolders) {
                        if (NormalizePath(pf.path) == NormalizePath(fPath)) {
                            pf.viewMode = v;
                            pf.sortMode = so;
                            break;
                        }
                    }
                    SavePinnedFolders();
                }
            };

            std::vector<MenuEntry> viewSub;
            viewSub.push_back({MenuEntry::Type::Action, L"Fan", curView == 0,
                               true, [setCfg, curSort] { setCfg(0, curSort); },
                               {}});
            viewSub.push_back({MenuEntry::Type::Action, L"Grid", curView == 1,
                               true, [setCfg, curSort] { setCfg(1, curSort); },
                               {}});
            viewSub.push_back({MenuEntry::Type::Action, L"List", curView == 2,
                               true, [setCfg, curSort] { setCfg(2, curSort); },
                               {}});
            entries.push_back({MenuEntry::Type::Submenu, L"View Content As",
                               false, true, nullptr, std::move(viewSub)});

            std::vector<MenuEntry> sortSub;
            sortSub.push_back({MenuEntry::Type::Action, L"Date Modified",
                               curSort == 0, true,
                               [setCfg, curView] { setCfg(curView, 0); }, {}});
            sortSub.push_back({MenuEntry::Type::Action, L"Name", curSort == 1,
                               true, [setCfg, curView] { setCfg(curView, 1); },
                               {}});
            sortSub.push_back({MenuEntry::Type::Action, L"Kind", curSort == 2,
                               true, [setCfg, curView] { setCfg(curView, 2); },
                               {}});
            entries.push_back({MenuEntry::Type::Submenu, L"Sort By", false,
                               true, nullptr, std::move(sortSub)});

            addSep();
            entries.push_back({MenuEntry::Type::Action,
                               L"Open \"" + fName + L"\"", false, !fPath.empty(),
                               [fPath] {
                                   ShellExecuteW(nullptr, L"open",
                                                 fPath.c_str(), nullptr,
                                                 nullptr, SW_SHOWNORMAL);
                               },
                               {}});
            entries.push_back({MenuEntry::Type::Action, L"Show in File Explorer",
                               false, !fPath.empty(),
                               [fPath] { ShowInFileExplorer(fPath); }, {}});
            addSep();
            entries.push_back({MenuEntry::Type::Action, L"Change Icon\u2026",
                               false, true,
                               [iconKey, fName] {
                                   PickCustomIconForKey(iconKey, fName);
                               },
                               {}});
            entries.push_back({MenuEntry::Type::Action, L"Reset Icon", false,
                               hasIcon,
                               [snap] { ResetCustomIconForItem(snap); },
                               {}});
            if (!isDl) {
                addSep();
                entries.push_back({MenuEntry::Type::Action,
                                   L"Remove from Dock", false, true,
                                   [fPath] {
                                       std::wstring norm = NormalizePath(fPath);
                                       g_pinnedFolders.erase(
                                           std::remove_if(
                                               g_pinnedFolders.begin(),
                                               g_pinnedFolders.end(),
                                               [&](const PinnedFolderStack& p) {
                                                   return NormalizePath(
                                                              p.path) == norm;
                                               }),
                                           g_pinnedFolders.end());
                                       SavePinnedFolders();
                                       ReconcileDockItems(false);
                                   },
                                   {}});
            }
        } else if (snap.kind == DockItemKind::Apps) {
            entries.push_back(
                {MenuEntry::Type::Action,
                 (g_settings.appsButtonAction == AppsButtonAction::AllApps)
                     ? L"Open Applications"
                     : L"Open Start Menu",
                 false, true, [snap]() mutable { LaunchAppItem(snap); }, {}});
            addSep();
            entries.push_back({MenuEntry::Type::Action, L"Change Icon\u2026",
                               false, true,
                               [iconKey] { PickCustomIconForKey(iconKey, L"Apps"); },
                               {}});
            entries.push_back({MenuEntry::Type::Action, L"Reset Icon", false,
                               hasIcon,
                               [snap] { ResetCustomIconForItem(snap); },
                               {}});
        } else if (snap.kind == DockItemKind::Finder) {
            for (HWND h : snap.windows) {
                entries.push_back({MenuEntry::Type::Action,
                                   GetWindowDisplayTitle(h), h == fg, true,
                                   [h] { ActivateWindowForce(h); }, {}});
            }
            if (!snap.windows.empty()) {
                addSep();
            }
            entries.push_back({MenuEntry::Type::Action,
                               L"New File Explorer Window", false, true,
                               [] {
                                   ShellExecuteW(nullptr, L"open",
                                                 L"explorer.exe", nullptr,
                                                 nullptr, SW_SHOWNORMAL);
                               },
                               {}});
            if (!snap.windows.empty()) {
                entries.push_back({MenuEntry::Type::Action, L"Hide", false, true,
                                   [wins = snap.windows] {
                                       HideItemWindows(wins);
                                   },
                                   {}});
            }
            if (auto expRecent =
                    QueryWindowsJumpList(L"Microsoft.Windows.Explorer", 6);
                !expRecent.empty()) {
                std::vector<MenuEntry> recSub;
                for (const auto& jr : expRecent) {
                    recSub.push_back({MenuEntry::Type::Action, jr.name, false,
                                      true,
                                      [jr] { LaunchJumpListEntry(jr); }, {}});
                }
                addSep();
                entries.push_back({MenuEntry::Type::Submenu, L"Recent", false,
                                   true, nullptr, std::move(recSub)});
            }
            addSep();
            entries.push_back({MenuEntry::Type::Action, L"Change Icon\u2026",
                               false, true,
                               [iconKey] {
                                   PickCustomIconForKey(iconKey, L"Finder");
                               },
                               {}});
            entries.push_back({MenuEntry::Type::Action, L"Reset Icon", false,
                               hasIcon,
                               [snap] { ResetCustomIconForItem(snap); },
                               {}});
        } else {
            // App or Recent
            bool isRunning = !snap.windows.empty();
            if (isRunning) {
                for (HWND h : snap.windows) {
                    entries.push_back({MenuEntry::Type::Action,
                                       GetWindowDisplayTitle(h), h == fg, true,
                                       [h] { ActivateWindowForce(h); }, {}});
                }
                addSep();
            }

            std::wstring jlId =
                !snap.taskbarAppId.empty() ? snap.taskbarAppId : snap.aumid;
            if (auto jlItems = QueryWindowsJumpList(jlId, 6);
                !jlItems.empty()) {
                std::vector<MenuEntry> recSub;
                for (const auto& jr : jlItems) {
                    recSub.push_back({MenuEntry::Type::Action, jr.name, false,
                                      true,
                                      [jr] { LaunchJumpListEntry(jr); }, {}});
                }
                entries.push_back({MenuEntry::Type::Submenu, L"Recent", false,
                                   true, nullptr, std::move(recSub)});
            }

            if (snap.pinned) {
                entries.push_back(
                    {MenuEntry::Type::Action, L"Unpin from Taskbar", false,
                     true, [snap] { UnpinItemFromDock(snap); }, {}});
            } else {
                entries.push_back(
                    {MenuEntry::Type::Action, L"Pin to Taskbar", false, true,
                     [snap] { PinItemToDock(snap); }, {}});
            }

            // Options submenu (Run as Admin, Custom Icon, Show in Explorer)
            std::vector<MenuEntry> opts;
            if (!snap.shortcutPath.empty() || !snap.path.empty()) {
                opts.push_back({MenuEntry::Type::Action,
                                L"Run as administrator", false, true,
                                [snap] { LaunchAppItemElevated(snap); }, {}});
                opts.push_back({MenuEntry::Type::Separator});
            }
            opts.push_back({MenuEntry::Type::Action, L"Change Icon\u2026", false,
                            true,
                            [iconKey, name = snap.name] {
                                PickCustomIconForKey(iconKey, name);
                            },
                            {}});
            opts.push_back({MenuEntry::Type::Action, L"Reset Icon", false,
                            hasIcon,
                            [snap] { ResetCustomIconForItem(snap); }, {}});
            if (!snap.path.empty() || !snap.shortcutPath.empty()) {
                std::wstring revealPath =
                    !snap.path.empty() ? snap.path : snap.shortcutPath;
                opts.push_back({MenuEntry::Type::Separator});
                opts.push_back({MenuEntry::Type::Action,
                                L"Show in File Explorer", false, true,
                                [revealPath] {
                                    ShowInFileExplorer(revealPath);
                                },
                                {}});
            }
            entries.push_back({MenuEntry::Type::Submenu, L"Options", false, true,
                               nullptr, std::move(opts)});
            addSep();

            if (isRunning) {
                entries.push_back(
                    {MenuEntry::Type::Action, L"New Window", false, true,
                     [snap]() mutable { LaunchAppItem(snap); }, {}});
                if (snap.windows.size() > 1) {
                    entries.push_back(
                        {MenuEntry::Type::Action, L"Show All Windows", false,
                         true,
                         [wins = snap.windows, la = snap.lastActive] {
                             ShowAllItemWindows(wins, la);
                         },
                         {}});
                }
                entries.push_back(
                    {MenuEntry::Type::Action, L"Hide", false, true,
                     [wins = snap.windows] { HideItemWindows(wins); }, {}});
                entries.push_back(
                    {MenuEntry::Type::Action,
                     altHeld ? L"Force Quit"
                             : (snap.windows.size() > 1 ? L"Close All Windows"
                                                        : L"Close Window"),
                     false, true,
                     [wins = snap.windows, altHeld] {
                         QuitItemWindows(wins, altHeld);
                     },
                     {}});
            } else {
                entries.push_back({MenuEntry::Type::Action, L"Open", false, true,
                                   [snap]() mutable {
                                       LaunchAppItem(snap);
                                   },
                                   {}});
            }
        }
    } else {
        // Dock background / divider menu
        entries.push_back({MenuEntry::Type::Action,
                           g_settings.autoHide ? L"Turn Hiding Off"
                                               : L"Turn Hiding On",
                           false, true,
                           [] {
                               Wh_SetIntValue(L"ovr_autohide",
                                              g_settings.autoHide ? 0 : 1);
                               ApplyLiveSettingsAfterOverride();
                           },
                           {}});
        entries.push_back({MenuEntry::Type::Action,
                           g_settings.magnification ? L"Turn Magnification Off"
                                                    : L"Turn Magnification On",
                           false, true,
                           [] {
                               Wh_SetIntValue(L"ovr_magnify",
                                              g_settings.magnification ? 0 : 1);
                               ApplyLiveSettingsAfterOverride();
                           },
                           {}});
        entries.push_back({MenuEntry::Type::Action, L"Hide Windows Taskbar",
                           g_settings.hideTaskbar, true,
                           [] {
                               Wh_SetIntValue(L"ovr_hide_taskbar",
                                              g_settings.hideTaskbar ? 0 : 1);
                               ApplyLiveSettingsAfterOverride();
                           },
                           {}});

        std::vector<MenuEntry> posSub;
        posSub.push_back({MenuEntry::Type::Action, L"Left",
                          g_settings.position == DockPosition::Left, true,
                          [] {
                              Wh_SetIntValue(L"ovr_position", 1);
                              ApplyLiveSettingsAfterOverride();
                          },
                          {}});
        posSub.push_back({MenuEntry::Type::Action, L"Bottom",
                          g_settings.position == DockPosition::Bottom, true,
                          [] {
                              Wh_SetIntValue(L"ovr_position", 0);
                              ApplyLiveSettingsAfterOverride();
                          },
                          {}});
        posSub.push_back({MenuEntry::Type::Action, L"Right",
                          g_settings.position == DockPosition::Right, true,
                          [] {
                              Wh_SetIntValue(L"ovr_position", 2);
                              ApplyLiveSettingsAfterOverride();
                          },
                          {}});
        entries.push_back({MenuEntry::Type::Submenu, L"Position on Screen",
                           false, true, nullptr, std::move(posSub)});

        entries.push_back(
            {MenuEntry::Type::Action, L"Minimize Windows into App Icon",
             g_settings.minimizeIntoAppIcon, true,
             [] {
                 Wh_SetIntValue(L"ovr_min_into_icon",
                                g_settings.minimizeIntoAppIcon ? 0 : 1);
                 ApplyLiveSettingsAfterOverride();
             },
             {}});

        std::vector<MenuEntry> icoStyleSub;
        icoStyleSub.push_back(
            {MenuEntry::Type::Action, L"Clear (Frosted Glass)",
             g_settings.iconStyle == DockIconStyle::Clear, true,
             [] {
                 Wh_SetIntValue(L"ovr_icon_style", 0);
                 ApplyLiveSettingsAfterOverride();
             },
             {}});
        icoStyleSub.push_back(
            {MenuEntry::Type::Action, L"Dark (Smoked Glass)",
             g_settings.iconStyle == DockIconStyle::Dark, true,
             [] {
                 Wh_SetIntValue(L"ovr_icon_style", 1);
                 ApplyLiveSettingsAfterOverride();
             },
             {}});
        icoStyleSub.push_back(
            {MenuEntry::Type::Action, L"Default (Full Color)",
             g_settings.iconStyle == DockIconStyle::Default, true,
             [] {
                 Wh_SetIntValue(L"ovr_icon_style", 2);
                 ApplyLiveSettingsAfterOverride();
             },
             {}});
        icoStyleSub.push_back(
            {MenuEntry::Type::Action, L"Tinted (Accent Glass)",
             g_settings.iconStyle == DockIconStyle::Tinted, true,
             [] {
                 Wh_SetIntValue(L"ovr_icon_style", 3);
                 ApplyLiveSettingsAfterOverride();
             },
             {}});
        entries.push_back({MenuEntry::Type::Submenu, L"Icon Style (Tahoe)",
                           false, true, nullptr, std::move(icoStyleSub)});

        std::vector<MenuEntry> glassSub;
        glassSub.push_back(
            {MenuEntry::Type::Action, L"Lens Refraction (macOS 26 Tahoe)",
             g_settings.glassRefraction == GlassRefraction::Lens, true,
             [] {
                 Wh_SetIntValue(L"ovr_glass_refract", 0);
                 ApplyLiveSettingsAfterOverride();
             },
             {}});
        glassSub.push_back(
            {MenuEntry::Type::Action, L"Mirror Reflection",
             g_settings.glassRefraction == GlassRefraction::Reflection, true,
             [] {
                 Wh_SetIntValue(L"ovr_glass_refract", 1);
                 ApplyLiveSettingsAfterOverride();
             },
             {}});
        glassSub.push_back(
            {MenuEntry::Type::Action, L"Convex Stretch",
             g_settings.glassRefraction == GlassRefraction::Stretch, true,
             [] {
                 Wh_SetIntValue(L"ovr_glass_refract", 2);
                 ApplyLiveSettingsAfterOverride();
             },
             {}});
        glassSub.push_back(
            {MenuEntry::Type::Action, L"Frosted Blur Only",
             g_settings.glassRefraction == GlassRefraction::Off, true,
             [] {
                 Wh_SetIntValue(L"ovr_glass_refract", 3);
                 ApplyLiveSettingsAfterOverride();
             },
             {}});
        glassSub.push_back({MenuEntry::Type::Separator, L"", false, false,
                            nullptr, {}});
        glassSub.push_back(
            {MenuEntry::Type::Action, L"Clear Liquid Glass",
             g_settings.glassStyle == GlassStyle::Clear, true,
             [] {
                 Wh_SetIntValue(L"ovr_glass_style", 0);
                 ApplyLiveSettingsAfterOverride();
             },
             {}});
        glassSub.push_back(
            {MenuEntry::Type::Action, L"Tinted Liquid Glass",
             g_settings.glassStyle == GlassStyle::Tinted, true,
             [] {
                 Wh_SetIntValue(L"ovr_glass_style", 1);
                 ApplyLiveSettingsAfterOverride();
             },
             {}});
        glassSub.push_back({MenuEntry::Type::Separator, L"", false, false,
                            nullptr, {}});
        glassSub.push_back(
            {MenuEntry::Type::Action, L"Window Preview Flyouts on Hover",
             g_settings.showWindowPreviews, true,
             [] {
                 Wh_SetIntValue(L"ovr_previews",
                                g_settings.showWindowPreviews ? 0 : 1);
                 ApplyLiveSettingsAfterOverride();
             },
             {}});
        glassSub.push_back(
            {MenuEntry::Type::Action, L"Icon Reflections on Glass",
             g_settings.dockIconReflections, true,
             [] {
                 Wh_SetIntValue(L"ovr_icon_refl",
                                g_settings.dockIconReflections ? 0 : 1);
                 ApplyLiveSettingsAfterOverride();
             },
             {}});
        glassSub.push_back(
            {MenuEntry::Type::Action,
             g_liquidCapture.frozen
                 ? L"Unfreeze Glass (Live Refraction)"
                 : L"Freeze Glass for Screenshot",
             g_liquidCapture.frozen, true,
             [] {
                 g_liquidCapture.frozen = !g_liquidCapture.frozen;
                 if (g_liquidCapture.frozen) {
                     ApplyCaptureExclusionToAllModWindows(false);
                 } else {
                     g_liquidCapture.excludedApplied = false;
                 }
                 g_dock.liquidPlate.dirty = true;
                 RequestAnimationFrame();
             },
             {}});
        entries.push_back({MenuEntry::Type::Submenu, L"Liquid Glass", false,
                           true, nullptr, std::move(glassSub)});

        addSep();
        entries.push_back({MenuEntry::Type::Action, L"Task Manager", false,
                           true,
                           [] {
                               WCHAR sysDir[MAX_PATH]{};
                               UINT len = GetSystemDirectoryW(sysDir,
                                                              ARRAYSIZE(sysDir));
                               std::wstring tmPath =
                                   (len > 0 && len < ARRAYSIZE(sysDir))
                                       ? NativeSystemPath(
                                             std::wstring(sysDir) +
                                             L"\\taskmgr.exe")
                                       : std::wstring(L"taskmgr.exe");
                               ShellExecuteW(nullptr, L"open", tmPath.c_str(),
                                             nullptr, nullptr, SW_SHOWNORMAL);
                           },
                           {}});
        entries.push_back({MenuEntry::Type::Action,
                           L"Windows Taskbar Settings\u2026", false, true,
                           [] {
                               ShellExecuteW(nullptr, L"open",
                                             L"ms-settings:taskbar", nullptr,
                                             nullptr, SW_SHOWNORMAL);
                           },
                           {}});
        addSep();
        entries.push_back({MenuEntry::Type::Action,
                           L"Desktop & Dock Settings\u2026", false, true,
                           [] { OpenTahoeSettingsPanel(); }, {}});
        entries.push_back({MenuEntry::Type::Action,
                           L"Windhawk Mod Settings\u2026", false, true,
                           [] { OpenWindhawkUi(); }, {}});
    }

    OpenGlassMenuPopup(g_menus.mainPopup, std::move(entries), anchor,
                       alignAbove, false);
}

HWND GetGlassNotifyHwnd() {
    return g_dock.hwnd;
}

bool IsDockHoverAnimating() {
    return g_dock.animating && g_dock.magAmount > 0.01f &&
           g_dock.magAmount < 0.99f;
}

void ApplyCaptureExclusionToAllModWindows(bool excluded) {
    DWORD affinity = excluded ? kWdaExcludeFromCapture : WDA_NONE;
    auto applyOne = [affinity](HWND h) {
        if (h && IsWindow(h)) {
            SetWindowDisplayAffinity(h, affinity);
        }
    };
    applyOne(g_dock.hwnd);
    applyOne(g_menus.mainPopup.hwnd);
    applyOne(g_menus.subPopup.hwnd);
    ApplyOverlayCaptureExclusion(affinity);
}

void ResetAndRedrawGpuSurfaces() {
    if (!g_dock.hwnd) {
        return;
    }
    ShutdownLiquidCapture();
    g_dock.liquidPlate.ResetGpuResources();
    g_dock.tooltip.liquidPlate.ResetGpuResources();
    g_menus.mainPopup.liquidPlate.ResetGpuResources();
    g_menus.subPopup.liquidPlate.ResetGpuResources();
    ResetFlyoutGpuResources();
    ClearIconCache();
    g_dock.tooltip.currentText.clear();
    RebuildShadowSurface();
    SyncItemIconsAndTheme();
    RequestAnimationFrame();
}

void OnGraphicsDeviceLost() {
    if (!g_dock.hwnd) {
        return;
    }
    ShutdownLiquidCapture();
    g_dock.liquidPlate.ResetGpuResources();
    g_dock.tooltip.liquidPlate.ResetGpuResources();
    g_menus.mainPopup.liquidPlate.ResetGpuResources();
    g_menus.subPopup.liquidPlate.ResetGpuResources();
    ResetFlyoutGpuResources();
    ClearIconCache();
    if (InitD3DAndD2D() && g_gfx.compGraphics && g_gfx.d2dDevice) {
        auto gfxInterop =
            g_gfx.compGraphics.as<ICompositionGraphicsDeviceInteropLocal>();
        g_settingRenderingDevice.store(true, std::memory_order_release);
        gfxInterop->SetRenderingDevice(g_gfx.d2dDevice.get());
        g_settingRenderingDevice.store(false, std::memory_order_release);
    }
    g_dock.tooltip.currentText.clear();
    RebuildShadowSurface();
    SyncItemIconsAndTheme();
    RequestAnimationFrame();
}

bool BuildDockVisualTree() {
    auto& c = g_gfx.compositor;
    if (!c) {
        return false;
    }

    auto desktopInterop = c.as<ICompositorDesktopInteropLocal>();
    HRESULT hr = desktopInterop->CreateDesktopWindowTarget(
        g_dock.hwnd, TRUE, winrt::put_abi(g_dock.target));
    if (FAILED(hr) || !g_dock.target) {
        Wh_Log(L"CreateDesktopWindowTarget failed: 0x%08X", (unsigned)hr);
        return false;
    }

    g_dock.root = c.CreateContainerVisual();
    g_dock.target.Root(g_dock.root);

    g_dock.contentRoot = c.CreateContainerVisual();
    g_dock.root.Children().InsertAtTop(g_dock.contentRoot);

    // 1. Nine-grid shadow sprite
    g_dock.shadowVisual = c.CreateSpriteVisual();
    g_dock.shadowSurfaceBrush = c.CreateSurfaceBrush();
    g_dock.shadowSurfaceBrush.Stretch(wuc::CompositionStretch::Fill);
    g_dock.shadowNineGrid = c.CreateNineGridBrush();
    g_dock.shadowNineGrid.Source(g_dock.shadowSurfaceBrush);
    g_dock.shadowVisual.Brush(g_dock.shadowNineGrid);
    g_dock.contentRoot.Children().InsertAtTop(g_dock.shadowVisual);

    // 2. Clipped Apple squircle glass container
    g_dock.glassContainer = c.CreateContainerVisual();
    g_dock.glassClipGeom = c.CreatePathGeometry();
    g_dock.glassContainer.Clip(c.CreateGeometricClip(g_dock.glassClipGeom));
    g_dock.contentRoot.Children().InsertAtTop(g_dock.glassContainer);

    // 2a. Blurred + saturated live host backdrop
    g_dock.hostBackdrop = c.CreateHostBackdropBrush();
    g_dock.backdropVisual = c.CreateSpriteVisual();
    g_dock.glassContainer.Children().InsertAtTop(g_dock.backdropVisual);

    // 2b. Smoked Liquid Glass fill tint
    g_dock.tintBrush = c.CreateColorBrush();
    g_dock.tintVisual = c.CreateSpriteVisual();
    g_dock.tintVisual.Brush(g_dock.tintBrush);
    g_dock.glassContainer.Children().InsertAtTop(g_dock.tintVisual);

    // 2c. Upper convex specular dome sheen
    g_dock.sheenBrush = c.CreateLinearGradientBrush();
    g_dock.sheenBrush.MappingMode(wuc::CompositionMappingMode::Absolute);
    g_dock.sheenVisual = c.CreateSpriteVisual();
    g_dock.sheenVisual.Brush(g_dock.sheenBrush);
    g_dock.glassContainer.Children().InsertAtTop(g_dock.sheenVisual);

    // 2d. Lower cool caustic bounce light
    g_dock.bounceBrush = c.CreateLinearGradientBrush();
    g_dock.bounceBrush.MappingMode(wuc::CompositionMappingMode::Absolute);
    g_dock.bounceVisual = c.CreateSpriteVisual();
    g_dock.bounceVisual.Brush(g_dock.bounceBrush);
    g_dock.glassContainer.Children().InsertAtTop(g_dock.bounceVisual);

    // 2e. Inner glass thickness refraction band
    g_dock.bandGeom = c.CreatePathGeometry();
    g_dock.bandShape = c.CreateSpriteShape(g_dock.bandGeom);
    g_dock.bandBrush = c.CreateColorBrush();
    g_dock.bandShape.StrokeBrush(g_dock.bandBrush);
    g_dock.bandShape.StrokeThickness(1.4f);
    g_dock.bandVisual = c.CreateShapeVisual();
    g_dock.bandVisual.Shapes().Append(g_dock.bandShape);
    g_dock.glassContainer.Children().InsertAtTop(g_dock.bandVisual);

    // 2f. Crisp multi-stop diagonal specular catchlight rim
    g_dock.rimGeom = c.CreatePathGeometry();
    g_dock.rimShape = c.CreateSpriteShape(g_dock.rimGeom);
    g_dock.rimBrush = c.CreateLinearGradientBrush();
    g_dock.rimBrush.MappingMode(wuc::CompositionMappingMode::Absolute);
    g_dock.rimShape.StrokeBrush(g_dock.rimBrush);
    g_dock.rimShape.StrokeThickness(1.15f);
    g_dock.rimVisual = c.CreateShapeVisual();
    g_dock.rimVisual.Shapes().Append(g_dock.rimShape);
    g_dock.glassContainer.Children().InsertAtTop(g_dock.rimVisual);

    // 3. Dock items container
    g_dock.itemsContainer = c.CreateContainerVisual();
    g_dock.contentRoot.Children().InsertAtTop(g_dock.itemsContainer);

    // 4. Glass tooltip capsule
    {
        auto& tip = g_dock.tooltip;
        tip.container = c.CreateContainerVisual();
        tip.clipGeom = c.CreateRoundedRectangleGeometry();
        tip.container.Clip(c.CreateGeometricClip(tip.clipGeom));
        tip.container.IsVisible(false);
        tip.container.Opacity(0.0f);

        tip.backdropVisual = c.CreateSpriteVisual();
        tip.container.Children().InsertAtTop(tip.backdropVisual);

        tip.tintBrush = c.CreateColorBrush();
        tip.tintVisual = c.CreateSpriteVisual();
        tip.tintVisual.Brush(tip.tintBrush);
        tip.container.Children().InsertAtTop(tip.tintVisual);

        tip.rimGeom = c.CreateRoundedRectangleGeometry();
        tip.rimShape = c.CreateSpriteShape(tip.rimGeom);
        tip.rimBrush = c.CreateLinearGradientBrush();
        tip.rimBrush.MappingMode(wuc::CompositionMappingMode::Absolute);
        tip.rimShape.StrokeBrush(tip.rimBrush);
        tip.rimShape.StrokeThickness(1.0f);
        tip.rimVisual = c.CreateShapeVisual();
        tip.rimVisual.Shapes().Append(tip.rimShape);
        tip.container.Children().InsertAtTop(tip.rimVisual);

        tip.textBrush = c.CreateSurfaceBrush();
        tip.textBrush.Stretch(wuc::CompositionStretch::Fill);
        tip.textVisual = c.CreateSpriteVisual();
        tip.textVisual.Brush(tip.textBrush);
        tip.container.Children().InsertAtTop(tip.textVisual);

        g_dock.contentRoot.Children().InsertAtTop(tip.container);
    }

    g_dock.scale = GetMonitorDpiScale(GetPrimaryMonitor());
    LoadCustomIcons();
    UpdateGlassAppearance();
    RebuildShadowSurface();
    ResizeDockWindowForMonitor();
    SeedPinnedAppsIfNeeded();
    ReconcileDockItems(true);
    LayoutAndApplyDock(NowSeconds());
    return true;
}

void DestroyDockVisualTree() {
    CloseAllMenus();
    ShutdownLiquidCapture();
    g_dock.liquidPlate.Shutdown();
    g_dock.liquidShelfAttached = false;
    g_dock.tooltip.liquidPlate.Shutdown();
    for (auto& item : g_items) {
        DetachItemVisuals(item);
    }
    g_items.clear();
    ClearIconCache();
    g_iconColorCache.clear();
    g_dock.tooltip = {};
    g_dock.itemsContainer = nullptr;
    g_dock.rimVisual = nullptr;
    g_dock.rimShape = nullptr;
    g_dock.rimGeom = nullptr;
    g_dock.rimBrush = nullptr;
    g_dock.bandVisual = nullptr;
    g_dock.bandShape = nullptr;
    g_dock.bandGeom = nullptr;
    g_dock.bandBrush = nullptr;
    g_dock.bounceVisual = nullptr;
    g_dock.bounceBrush = nullptr;
    g_dock.sheenVisual = nullptr;
    g_dock.sheenBrush = nullptr;
    g_dock.tintVisual = nullptr;
    g_dock.tintBrush = nullptr;
    g_dock.backdropVisual = nullptr;
    g_dock.backdropEffectBrush = nullptr;
    g_dock.hostBackdrop = nullptr;
    g_dock.glassClipGeom = nullptr;
    g_dock.glassContainer = nullptr;
    g_dock.shadowVisual = nullptr;
    g_dock.shadowNineGrid = nullptr;
    g_dock.shadowSurfaceBrush = nullptr;
    g_dock.shadowSurface = {};
    g_dock.contentRoot = nullptr;
    g_dock.root = nullptr;
    g_dock.target = nullptr;
    g_dock.lastGlassW = g_dock.lastGlassH = g_dock.lastGlassR = -1.0f;
    g_dock.lastShadowShelfH = -1.0f;
}

// ============================================================================
// 15. Taskbar Hider
// ============================================================================

struct TaskbarHiderState {
    bool hiddenByUs = false;
    HWINEVENTHOOK showHook = nullptr;
    DWORD explorerPid = 0;
    UINT taskbarCreatedMsg = 0;
    std::vector<HWND> hiddenHwnds;
};

TaskbarHiderState g_taskbarHider;

bool IsTaskbarWindowClass(HWND hwnd) {
    WCHAR cls[64]{};
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        return false;
    }
    return _wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0;
}

void RememberHiddenTaskbarHwnd(HWND hwnd) {
    if (std::find(g_taskbarHider.hiddenHwnds.begin(),
                  g_taskbarHider.hiddenHwnds.end(),
                  hwnd) == g_taskbarHider.hiddenHwnds.end()) {
        g_taskbarHider.hiddenHwnds.push_back(hwnd);
    }
}

void HideAllTaskbarWindows() {
    if (HWND mainTray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
        if (IsWindowVisible(mainTray)) {
            ShowWindow(mainTray, SW_HIDE);
        }
        RememberHiddenTaskbarHwnd(mainTray);
    }

    HWND sec = nullptr;
    while ((sec = FindWindowExW(nullptr, sec, L"Shell_SecondaryTrayWnd",
                                nullptr)) != nullptr) {
        if (IsWindowVisible(sec)) {
            ShowWindow(sec, SW_HIDE);
        }
        RememberHiddenTaskbarHwnd(sec);
    }
}

void CALLBACK TaskbarShowEventProc(HWINEVENTHOOK,
                                   DWORD event,
                                   HWND hwnd,
                                   LONG idObject,
                                   LONG idChild,
                                   DWORD,
                                   DWORD) {
    if (event != EVENT_OBJECT_SHOW || idObject != OBJID_WINDOW ||
        idChild != CHILDID_SELF || !hwnd || !g_settings.hideTaskbar ||
        g_quitting) {
        return;
    }
    if (IsTaskbarWindowClass(hwnd)) {
        ShowWindow(hwnd, SW_HIDE);
        RememberHiddenTaskbarHwnd(hwnd);
    }
}

void EnsureTaskbarShowHook() {
    HWND mainTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD pid = 0;
    if (mainTray) {
        GetWindowThreadProcessId(mainTray, &pid);
    }
    if (!pid) {
        return;
    }
    if (g_taskbarHider.showHook && g_taskbarHider.explorerPid == pid) {
        return;
    }
    if (g_taskbarHider.showHook) {
        UnhookWinEvent(g_taskbarHider.showHook);
        g_taskbarHider.showHook = nullptr;
    }
    g_taskbarHider.explorerPid = pid;
    g_taskbarHider.showHook = SetWinEventHook(
        EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, nullptr, TaskbarShowEventProc,
        pid, 0, WINEVENT_OUTOFCONTEXT);
}

void ApplyTaskbarHiding() {
    if (!g_settings.hideTaskbar) {
        return;
    }
    HWND mainTray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!mainTray) {
        return;
    }
    if (!g_taskbarHider.hiddenByUs) {
        int saved = Wh_GetIntValue(L"restore_autohide", -1);
        if (saved == -1) {
            APPBARDATA abd{.cbSize = sizeof(abd), .hWnd = mainTray};
            UINT state = static_cast<UINT>(SHAppBarMessage(ABM_GETSTATE, &abd));
            Wh_SetIntValue(L"restore_autohide", static_cast<int>(state));
        }
        APPBARDATA abd{.cbSize = sizeof(abd),
                       .hWnd = mainTray,
                       .lParam = ABS_AUTOHIDE};
        SHAppBarMessage(ABM_SETSTATE, &abd);
        g_taskbarHider.hiddenByUs = true;
    } else {
        APPBARDATA abd{.cbSize = sizeof(abd), .hWnd = mainTray};
        UINT state = static_cast<UINT>(SHAppBarMessage(ABM_GETSTATE, &abd));
        if ((state & ABS_AUTOHIDE) == 0) {
            abd.lParam = static_cast<LPARAM>(state | ABS_AUTOHIDE);
            SHAppBarMessage(ABM_SETSTATE, &abd);
        }
    }

    HideAllTaskbarWindows();
    EnsureTaskbarShowHook();
}

void RestoreTaskbar() {
    if (g_taskbarHider.showHook) {
        UnhookWinEvent(g_taskbarHider.showHook);
        g_taskbarHider.showHook = nullptr;
    }
    g_taskbarHider.explorerPid = 0;

    int saved = Wh_GetIntValue(L"restore_autohide", -1);
    if (g_taskbarHider.hiddenByUs || saved != -1) {
        if (HWND mainTray = FindWindowW(L"Shell_TrayWnd", nullptr)) {
            if (saved != -1) {
                APPBARDATA abd{.cbSize = sizeof(abd),
                               .hWnd = mainTray,
                               .lParam = static_cast<LPARAM>(saved)};
                SHAppBarMessage(ABM_SETSTATE, &abd);
            }
            ShowWindow(mainTray, SW_SHOWNA);
        }

        HWND sec = nullptr;
        while ((sec = FindWindowExW(nullptr, sec, L"Shell_SecondaryTrayWnd",
                                    nullptr)) != nullptr) {
            ShowWindow(sec, SW_SHOWNA);
        }
        for (HWND h : g_taskbarHider.hiddenHwnds) {
            if (IsWindow(h)) {
                ShowWindow(h, SW_SHOWNA);
            }
        }
        g_taskbarHider.hiddenHwnds.clear();
        Wh_SetIntValue(L"restore_autohide", -1);
        g_taskbarHider.hiddenByUs = false;
    }
}

// ============================================================================
// 16. Reusable Popup Glass Shell & Shell URI Helper
// ============================================================================

void OpenShellUri(PCWSTR uri) {
    if (uri && *uri) {
        ShellExecuteW(nullptr, L"open", uri, nullptr, nullptr, SW_SHOWNORMAL);
    }
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

// ============================================================================
// 17. Drag & Drop (M5), Downloads Fan Stacks (M7), Previews (M8) & Settings (M11)
// ============================================================================

// ---- M5a. Internal Drag-to-Reorder & Drag-Off-to-Remove ----

struct DockDragState {
    bool candidate = false;
    bool active = false;
    bool removeArmed = false;
    bool reordered = false;
    std::wstring identityKey;
    std::wstring displayName;
    POINT downScreen{};
    HWND ghostHwnd = nullptr;
    winrt::com_ptr<IWICBitmapSource> ghostIcon;
    int ghostW = 0;
    int ghostH = 0;
};

DockDragState g_drag;
constexpr WCHAR kDragGhostClassName[] = L"WindhawkMacOS27DragGhostWnd";

void PaintDragGhost(POINT cursorScreen, bool showRemoveBadge) {
    if (!g_drag.ghostHwnd || !g_gfx.d2dFactory || !g_gfx.dwriteFactory) {
        return;
    }
    const float s = g_dock.scale;
    const float icoSz = std::max(44.0f * s, g_dock.effectiveTilePx * 1.12f);
    const int W = static_cast<int>(std::ceil(icoSz + 44.0f * s));
    const int H = static_cast<int>(std::ceil(icoSz + 40.0f * s));

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = W;
    bi.bmiHeader.biHeight = -H;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HDC memDC = CreateCompatibleDC(nullptr);
    HBITMAP dib =
        memDC ? CreateDIBSection(memDC, &bi, DIB_RGB_COLORS, &bits, nullptr, 0)
              : nullptr;
    if (!dib) {
        if (memDC) DeleteDC(memDC);
        return;
    }
    HGDIOBJ old = SelectObject(memDC, dib);

    auto props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED),
        96.0f, 96.0f);
    winrt::com_ptr<ID2D1DCRenderTarget> rt;
    if (SUCCEEDED(g_gfx.d2dFactory->CreateDCRenderTarget(&props, rt.put())) &&
        rt) {
        RECT rc{0, 0, W, H};
        if (SUCCEEDED(rt->BindDC(memDC, &rc))) {
            rt->BeginDraw();
            rt->Clear(D2D1::ColorF(0, 0, 0, 0));

            float ix = (W - icoSz) * 0.5f;
            float iy = 24.0f * s;
            if (g_drag.ghostIcon) {
                winrt::com_ptr<ID2D1Bitmap> bmp;
                if (SUCCEEDED(rt->CreateBitmapFromWicBitmap(
                        g_drag.ghostIcon.get(), nullptr, bmp.put())) &&
                    bmp) {
                    rt->DrawBitmap(bmp.get(),
                                   D2D1::RectF(ix, iy, ix + icoSz, iy + icoSz),
                                   showRemoveBadge ? 0.65f : 0.92f,
                                   D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
            }
            if (showRemoveBadge) {
                winrt::com_ptr<ID2D1SolidColorBrush> br;
                rt->CreateSolidColorBrush(D2D1::ColorF(0.12f, 0.12f, 0.16f,
                                                       0.88f),
                                          br.put());
                float pw = 74.0f * s;
                float ph = 22.0f * s;
                D2D1_RECT_F pr{(W - pw) * 0.5f, 2.0f * s, (W + pw) * 0.5f,
                               2.0f * s + ph};
                D2D1_ROUNDED_RECT prr = D2D1::RoundedRect(pr, ph * 0.5f,
                                                          ph * 0.5f);
                rt->FillRoundedRectangle(prr, br.get());
                br->SetColor(D2D1::ColorF(1.0f, 0.35f, 0.32f, 0.85f));
                rt->DrawRoundedRectangle(prr, br.get(), 1.0f * s);
                winrt::com_ptr<IDWriteTextFormat> fmt;
                g_gfx.dwriteFactory->CreateTextFormat(
                    ResolveFontFamily().c_str(), nullptr,
                    DWRITE_FONT_WEIGHT_SEMI_BOLD, DWRITE_FONT_STYLE_NORMAL,
                    DWRITE_FONT_STRETCH_NORMAL, 11.5f * s, L"en-us", fmt.put());
                if (fmt) {
                    fmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                    fmt->SetParagraphAlignment(
                        DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                    br->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.96f));
                    rt->DrawText(L"Remove", 6, fmt.get(), pr, br.get());
                }
            }
            rt->EndDraw();
        }
    }

    POINT dst{cursorScreen.x - W / 2, cursorScreen.y - H / 2};
    POINT src{0, 0};
    SIZE sz{W, H};
    BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    HDC screen = GetDC(nullptr);
    UpdateLayeredWindow(g_drag.ghostHwnd, screen, &dst, &sz, memDC, &src, 0,
                        &bf, ULW_ALPHA);
    ReleaseDC(nullptr, screen);
    SelectObject(memDC, old);
    DeleteObject(dib);
    DeleteDC(memDC);
}

void CloseDragGhost() {
    if (g_drag.ghostHwnd) {
        ShowWindow(g_drag.ghostHwnd, SW_HIDE);
        DestroyWindow(g_drag.ghostHwnd);
        g_drag.ghostHwnd = nullptr;
    }
    g_drag.ghostIcon = nullptr;
}

void BeginInternalDockDrag(POINT curScreen) {
    int idx = -1;
    for (size_t i = 0; i < g_items.size(); ++i) {
        if (g_items[i].identityKey == g_drag.identityKey) {
            idx = static_cast<int>(i);
            break;
        }
    }
    if (idx < 0 || !g_items[idx].pinned ||
        g_items[idx].kind != DockItemKind::App) {
        g_drag.candidate = false;
        return;
    }

    CloseWindowPreviewFlyout();
    g_drag.active = true;
    g_drag.reordered = false;
    g_drag.removeArmed = false;
    g_dock.modalActive = true;
    SetCapture(g_dock.hwnd);

    // Load icon for ghost
    const auto& it = g_items[idx];
    if (auto cp = GetCustomIconPathForItem(it, false); !cp.empty()) {
        g_drag.ghostIcon = LoadWicBitmapFromFile(cp);
    }
    if (!g_drag.ghostIcon && !it.shortcutPath.empty()) {
        g_drag.ghostIcon = LoadIconFromPath(it.shortcutPath, 128);
    }
    if (!g_drag.ghostIcon && !it.path.empty()) {
        g_drag.ghostIcon = LoadIconFromPath(it.path, 128);
    }
    if (!g_drag.ghostIcon && !it.aumid.empty()) {
        g_drag.ghostIcon = LoadIconFromAumid(it.aumid, 128);
    }

    HMODULE hSelf = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&BeginInternalDockDrag),
                       &hSelf);
    static bool s_reg = false;
    if (!s_reg) {
        WNDCLASSEXW wc{.cbSize = sizeof(wc),
                       .lpfnWndProc = DefWindowProcW,
                       .hInstance = hSelf,
                       .lpszClassName = kDragGhostClassName};
        RegisterClassExW(&wc);
        s_reg = true;
    }
    g_drag.ghostHwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW |
            WS_EX_NOACTIVATE,
        kDragGhostClassName, L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, hSelf,
        nullptr);
    if (g_drag.ghostHwnd) {
        ApplyWindowGlassCaptureAffinity(g_drag.ghostHwnd);
        ShowWindow(g_drag.ghostHwnd, SW_SHOWNOACTIVATE);
        PaintDragGhost(curScreen, false);
    }
}

void UpdateInternalDockDrag(POINT curScreen) {
    if (!g_drag.active || !g_dock.hwnd) {
        return;
    }
    RECT wr{};
    GetWindowRect(g_dock.hwnd, &wr);
    float lx = static_cast<float>(curScreen.x - wr.left);
    float ly = static_cast<float>(curScreen.y - wr.top);
    const auto& sr = g_dock.shelfRect;
    float perpDist = 0.0f;
    if (g_settings.position == DockPosition::Bottom) {
        perpDist = sr.top - ly;
    } else if (g_settings.position == DockPosition::Left) {
        perpDist = lx - sr.right;
    } else {
        perpDist = sr.left - lx;
    }
    g_drag.removeArmed = (perpDist > 82.0f * g_dock.scale);
    PaintDragGhost(curScreen, g_drag.removeArmed);

    if (g_drag.removeArmed) {
        return;
    }

    // Reorder within pinned app items based on cursor main coordinate
    float mainPos = (g_settings.position == DockPosition::Bottom) ? lx : ly;
    int dragItemIdx = -1;
    int targetItemIdx = -1;
    for (size_t i = 0; i < g_items.size(); ++i) {
        if (g_items[i].identityKey == g_drag.identityKey) {
            dragItemIdx = static_cast<int>(i);
        }
        if (g_items[i].kind == DockItemKind::App && g_items[i].pinned &&
            mainPos >= g_items[i].slotStart && mainPos <= g_items[i].slotEnd) {
            targetItemIdx = static_cast<int>(i);
        }
    }
    if (dragItemIdx >= 0 && targetItemIdx >= 0 && dragItemIdx != targetItemIdx) {
        // Find matching indices in g_pinnedApps
        int pFrom = -1, pTo = -1;
        for (size_t j = 0; j < g_pinnedApps.size(); ++j) {
            if (AppsMatch(g_pinnedApps[j].aumid, g_pinnedApps[j].path,
                          g_items[dragItemIdx].aumid,
                          g_items[dragItemIdx].path)) {
                pFrom = static_cast<int>(j);
            }
            if (AppsMatch(g_pinnedApps[j].aumid, g_pinnedApps[j].path,
                          g_items[targetItemIdx].aumid,
                          g_items[targetItemIdx].path)) {
                pTo = static_cast<int>(j);
            }
        }
        if (pFrom >= 0 && pTo >= 0 && pFrom != pTo) {
            auto movedPin = std::move(g_pinnedApps[pFrom]);
            g_pinnedApps.erase(g_pinnedApps.begin() + pFrom);
            g_pinnedApps.insert(g_pinnedApps.begin() + pTo,
                                std::move(movedPin));

            auto movedItem = std::move(g_items[dragItemIdx]);
            g_items.erase(g_items.begin() + dragItemIdx);
            g_items.insert(g_items.begin() + targetItemIdx,
                           std::move(movedItem));
            g_drag.reordered = true;
            RequestAnimationFrame();
        }
    }
}

void EndInternalDockDrag(bool commit) {
    bool wasActive = g_drag.active;
    bool removeArmed = g_drag.removeArmed;
    bool reordered = g_drag.reordered;
    std::wstring key = g_drag.identityKey;

    g_drag.candidate = false;
    g_drag.active = false;
    g_drag.removeArmed = false;
    g_drag.reordered = false;
    CloseDragGhost();

    if (wasActive) {
        if (GetCapture() == g_dock.hwnd) {
            ReleaseCapture();
        }
        g_dock.modalActive = false;
        if (commit && removeArmed) {
            for (const auto& it : g_items) {
                if (it.identityKey == key) {
                    UnpinItemFromDock(it);
                    break;
                }
            }
        } else if (commit && reordered) {
            SavePinnedOrderOverlay(g_pinnedApps);
            SavePinnedApps();
        }
        RequestAnimationFrame();
    }
}

// ---- M5b. OLE File Drop Target (CF_HDROP -> Pin / Open With / Trash / Downloads) ----

void ExecuteShellFileOp(UINT func, const std::vector<std::wstring>& files,
                        const std::wstring& destDir, FILEOP_FLAGS flags) {
    if (files.empty()) {
        return;
    }
    std::wstring fromBuf;
    for (const auto& f : files) {
        fromBuf += f;
        fromBuf.push_back(L'\0');
    }
    fromBuf.push_back(L'\0');
    std::wstring toBuf = destDir;
    if (!toBuf.empty()) {
        toBuf.push_back(L'\0');
        toBuf.push_back(L'\0');
    }
    SHFILEOPSTRUCTW op{};
    op.wFunc = func;
    op.pFrom = fromBuf.c_str();
    op.pTo = toBuf.empty() ? nullptr : toBuf.c_str();
    op.fFlags = flags;
    SHFileOperationW(&op);
}

void HandleDroppedFilesOnDock(const std::vector<std::wstring>& files,
                              POINT screenPt,
                              DWORD keyState) {
    if (files.empty() || !g_dock.hwnd) {
        return;
    }
    RECT wr{};
    GetWindowRect(g_dock.hwnd, &wr);
    float lx = static_cast<float>(screenPt.x - wr.left);
    float ly = static_cast<float>(screenPt.y - wr.top);
    int idx = HitTestItemAtPoint(lx, ly);

    if (idx >= 0 && idx < static_cast<int>(g_items.size())) {
        const auto& item = g_items[idx];
        if (item.kind == DockItemKind::Trash) {
            ExecuteShellFileOp(
                FO_DELETE, files, L"",
                FOF_ALLOWUNDO | FOF_WANTNUKEWARNING);
            ClearIconCache();
            SyncItemIconsAndTheme();
            RequestAnimationFrame();
            return;
        }
        if (item.kind == DockItemKind::Downloads ||
            item.kind == DockItemKind::FolderStack) {
            std::wstring dest = (item.kind == DockItemKind::Downloads)
                                    ? GetEffectiveDownloadsPath()
                                    : item.path;
            if (!dest.empty()) {
                UINT func = (keyState & MK_CONTROL) ? FO_COPY : FO_MOVE;
                ExecuteShellFileOp(func, files, dest, FOF_ALLOWUNDO);
            }
            return;
        }
        if (item.kind == DockItemKind::App && !item.path.empty()) {
            // Check if user dropped an .exe/.lnk to pin instead of opening with app
            bool allPinable = true;
            for (const auto& f : files) {
                std::wstring low = ToLower(f);
                if (!low.ends_with(L".exe") && !low.ends_with(L".lnk")) {
                    allPinable = false;
                    break;
                }
            }
            if (!allPinable) {
                for (const auto& f : files) {
                    std::wstring param = L"\"" + f + L"\"";
                    ShellExecuteW(nullptr, L"open", item.path.c_str(),
                                  param.c_str(), nullptr, SW_SHOWNORMAL);
                }
                return;
            }
        }
    }

    // Pin dropped .exe / .lnk apps OR pin dropped directories as right-zone Tahoe Stacks!
    bool pinnedAny = false;
    bool pinnedFolderAny = false;
    for (const auto& f : files) {
        DWORD attr = GetFileAttributesW(f.c_str());
        if (attr != INVALID_FILE_ATTRIBUTES &&
            (attr & FILE_ATTRIBUTE_DIRECTORY)) {
            std::wstring norm = NormalizePath(f);
            bool dup = (NormalizePath(GetEffectiveDownloadsPath()) == norm);
            for (const auto& pf : g_pinnedFolders) {
                if (NormalizePath(pf.path) == norm) {
                    dup = true;
                    break;
                }
            }
            if (!dup) {
                PinnedFolderStack pf;
                pf.path = f;
                const wchar_t* fn = PathFindFileNameW(f.c_str());
                pf.name = (fn && *fn) ? fn : f;
                pf.viewMode = 1;  // Grid default for custom Stacks
                pf.sortMode = 0;
                g_pinnedFolders.push_back(std::move(pf));
                pinnedFolderAny = true;
            }
            continue;
        }
        std::wstring low = ToLower(f);
        std::optional<StoredApp> sa;
        if (low.ends_with(L".lnk")) {
            sa = ResolveShortcutFile(f);
        } else if (low.ends_with(L".exe")) {
            StoredApp app;
            app.isAumid = false;
            app.path = f;
            app.name = GetFileDescriptionFromPath(f);
            sa = std::move(app);
        }
        if (sa) {
            DockItem tmp;
            tmp.kind = DockItemKind::App;
            tmp.aumid = sa->aumid;
            tmp.path = sa->path;
            tmp.shortcutPath = sa->shortcutPath;
            tmp.args = sa->args;
            tmp.workdir = sa->workdir;
            tmp.name = sa->name;
            PinItemToDock(tmp);
            pinnedAny = false;
        }
    }
    if (pinnedAny) {
        SavePinnedApps();
    }
    if (pinnedFolderAny) {
        SavePinnedFolders();
    }
    if (pinnedAny || pinnedFolderAny) {
        ReconcileDockItems(false);
    }
}

class DockOleDropTarget final : public IDropTarget {
public:
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid,
                                             void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IDropTarget) {
            *ppv = static_cast<IDropTarget*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    ULONG STDMETHODCALLTYPE AddRef() override {
        return static_cast<ULONG>(InterlockedIncrement(&m_ref));
    }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG r = static_cast<ULONG>(InterlockedDecrement(&m_ref));
        if (r == 0) delete this;
        return r;
    }

    HRESULT STDMETHODCALLTYPE DragEnter(IDataObject* pDataObj,
                                        DWORD grfKeyState,
                                        POINTL pt,
                                        DWORD* pdwEffect) override {
        FORMATETC fe{CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        m_hasFiles = pDataObj && SUCCEEDED(pDataObj->QueryGetData(&fe));
        return DragOver(grfKeyState, pt, pdwEffect);
    }

    HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState,
                                       POINTL pt,
                                       DWORD* pdwEffect) override {
        if (!pdwEffect) return E_INVALIDARG;
        if (!m_hasFiles) {
            *pdwEffect = DROPEFFECT_NONE;
            return S_OK;
        }
        PollCursorState();
        *pdwEffect = (grfKeyState & MK_CONTROL) ? DROPEFFECT_COPY
                                                : DROPEFFECT_LINK;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE DragLeave() override {
        m_hasFiles = false;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE Drop(IDataObject* pDataObj,
                                   DWORD grfKeyState,
                                   POINTL pt,
                                   DWORD* pdwEffect) override {
        if (pdwEffect) *pdwEffect = DROPEFFECT_NONE;
        if (!pDataObj) return E_INVALIDARG;
        FORMATETC fe{CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
        STGMEDIUM sm{};
        if (SUCCEEDED(pDataObj->GetData(&fe, &sm)) && sm.hGlobal) {
            HDROP hDrop = static_cast<HDROP>(GlobalLock(sm.hGlobal));
            if (hDrop) {
                UINT count = DragQueryFileW(hDrop, 0xFFFFFFFFu, nullptr, 0);
                std::vector<std::wstring> files;
                files.reserve(count);
                for (UINT i = 0; i < count; ++i) {
                    UINT len = DragQueryFileW(hDrop, i, nullptr, 0);
                    if (len > 0) {
                        std::wstring path(len, L'\0');
                        DragQueryFileW(hDrop, i, path.data(), len + 1);
                        files.push_back(std::move(path));
                    }
                }
                GlobalUnlock(sm.hGlobal);
                if (!files.empty()) {
                    if (pdwEffect) *pdwEffect = DROPEFFECT_COPY;
                    POINT sp{pt.x, pt.y};
                    PostDeferredDockAction([files = std::move(files), sp,
                                            grfKeyState] {
                        HandleDroppedFilesOnDock(files, sp, grfKeyState);
                    });
                }
            }
            ReleaseStgMedium(&sm);
        }
        m_hasFiles = false;
        return S_OK;
    }

private:
    LONG m_ref = 1;
    bool m_hasFiles = false;
};

DockOleDropTarget* g_dropTarget = nullptr;

// ---- M7. Tahoe Multi-Folder Stacks (Fan / Grid / List) & Shell Notify ----

struct FanFileItem {
    std::wstring fullPath;
    std::wstring name;
    std::wstring ext;
    bool isDir = false;
    FILETIME writeTime{};
    winrt::com_ptr<IWICBitmapSource> icon;
    D2D1_RECT_F hitRect{};
};

struct DownloadsFanState {
    HWND hwnd = nullptr;
    bool open = false;
    std::wstring folderPath;
    std::wstring folderName;
    std::wstring stackId;
    D2D1_RECT_F anchorIconRect{};
    int viewMode = 0;  // 0 = Fan, 1 = Grid, 2 = List
    int sortMode = 0;  // 0 = Date, 1 = Name, 2 = Kind
    float widthPx = 0.0f;
    float heightPx = 0.0f;
    int hoveredIdx = -1;  // -2 = Open in Finder, -10/-11/-12 = Fan/Grid/List pills
    D2D1_RECT_F openAllRect{};
    D2D1_RECT_F viewPillRects[3]{};
    std::vector<FanFileItem> files;

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

    ULONG shellNotifyDownloads = 0;
    ULONG shellNotifyTrash = 0;
    ULONG shellNotifyTaskbar = 0;
};

DownloadsFanState g_dlFan;
constexpr WCHAR kDownloadsFanClassName[] = L"WindhawkMacOS27DownloadsFanWnd";

void CloseDownloadsFan() {
    if (!g_dlFan.open && !g_dlFan.hwnd) {
        return;
    }
    g_dlFan.open = false;
    g_dlFan.files.clear();
    g_dlFan.liquidPlate.Shutdown();
    g_dlFan.uiVisual = nullptr;
    g_dlFan.uiBrush = nullptr;
    g_dlFan.uiSurface = nullptr;
    g_dlFan.fallbackTint = nullptr;
    g_dlFan.fallbackBackdrop = nullptr;
    g_dlFan.clipGeom = nullptr;
    g_dlFan.glassContainer = nullptr;
    g_dlFan.root = nullptr;
    g_dlFan.target = nullptr;
    if (g_dlFan.hwnd) {
        ShowWindow(g_dlFan.hwnd, SW_HIDE);
        DestroyWindow(g_dlFan.hwnd);
        g_dlFan.hwnd = nullptr;
    }
    g_dock.modalActive = false;
    RequestAnimationFrame();
}

void RenderDownloadsFan() {
    if (!g_dlFan.open || !g_dlFan.uiSurface || !g_gfx.dwriteFactory) {
        return;
    }
    const float s = g_dock.scale;
    const float W = g_dlFan.widthPx;
    const bool dark = g_dock.darkMode;
    auto acc = GetAccentColor();
    D2D1_COLOR_F accCol =
        D2D1::ColorF(acc.R / 255.0f, acc.G / 255.0f, acc.B / 255.0f, 0.88f);
    D2D1_COLOR_F fg = dark ? D2D1::ColorF(1, 1, 1, 0.94f)
                           : D2D1::ColorF(0.08f, 0.08f, 0.11f, 0.94f);
    D2D1_COLOR_F fgSub = dark ? D2D1::ColorF(1, 1, 1, 0.56f)
                              : D2D1::ColorF(0.08f, 0.08f, 0.11f, 0.56f);

    DrawCompositionSurface(g_dlFan.uiSurface, [&](ID2D1DeviceContext* dc) {
        winrt::com_ptr<ID2D1SolidColorBrush> br;
        dc->CreateSolidColorBrush(fg, br.put());

        winrt::com_ptr<IDWriteTextFormat> fmtName, fmtHead, fmtSmall;
        const auto& fam = ResolveFontFamily();
        g_gfx.dwriteFactory->CreateTextFormat(
            fam.c_str(), nullptr, DWRITE_FONT_WEIGHT_MEDIUM,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.2f * s,
            L"en-us", fmtName.put());
        g_gfx.dwriteFactory->CreateTextFormat(
            fam.c_str(), nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.2f * s,
            L"en-us", fmtHead.put());
        g_gfx.dwriteFactory->CreateTextFormat(
            fam.c_str(), nullptr, DWRITE_FONT_WEIGHT_MEDIUM,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 10.6f * s,
            L"en-us", fmtSmall.put());

        auto setEllip = [&](IDWriteTextFormat* f,
                            DWRITE_TEXT_ALIGNMENT align) {
            if (!f) return;
            f->SetTextAlignment(align);
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
        setEllip(fmtName.get(), DWRITE_TEXT_ALIGNMENT_LEADING);
        setEllip(fmtHead.get(), DWRITE_TEXT_ALIGNMENT_LEADING);
        setEllip(fmtSmall.get(), DWRITE_TEXT_ALIGNMENT_CENTER);

        const float pad = 11.0f * s;
        float y = pad;

        // Header row: Folder name + live Fan | Grid | List view switcher pills
        float pillW = 38.0f * s;
        float pillH = 22.0f * s;
        float pillGap = 3.0f * s;
        float segW = pillW * 3.0f + pillGap * 2.0f + 4.0f * s;
        float segX = W - pad - segW;
        D2D1_RECT_F segBg{segX, y, segX + segW, y + pillH + 4.0f * s};
        br->SetColor(dark ? D2D1::ColorF(1, 1, 1, 0.08f)
                          : D2D1::ColorF(0, 0, 0, 0.07f));
        dc->FillRoundedRectangle(
            D2D1::RoundedRect(segBg, (pillH + 4 * s) * 0.5f,
                              (pillH + 4 * s) * 0.5f),
            br.get());

        PCWSTR viewNames[3] = {L"Fan", L"Grid", L"List"};
        for (int m = 0; m < 3; ++m) {
            float px = segX + 2.0f * s + static_cast<float>(m) * (pillW + pillGap);
            D2D1_RECT_F pr{px, y + 2.0f * s, px + pillW, y + 2.0f * s + pillH};
            g_dlFan.viewPillRects[m] = pr;
            bool active = (g_dlFan.viewMode == m);
            bool hov = (g_dlFan.hoveredIdx == (-10 - m));
            if (active || hov) {
                br->SetColor(active
                                 ? accCol
                                 : (dark ? D2D1::ColorF(1, 1, 1, 0.14f)
                                         : D2D1::ColorF(0, 0, 0, 0.10f)));
                dc->FillRoundedRectangle(
                    D2D1::RoundedRect(pr, pillH * 0.5f, pillH * 0.5f),
                    br.get());
            }
            br->SetColor(active ? D2D1::ColorF(1, 1, 1, 0.98f) : fgSub);
            dc->DrawText(viewNames[m], static_cast<UINT32>(wcslen(viewNames[m])),
                         fmtSmall.get(), pr, br.get());
        }

        br->SetColor(fg);
        dc->DrawText(g_dlFan.folderName.c_str(),
                     static_cast<UINT32>(g_dlFan.folderName.size()),
                     fmtHead.get(),
                     D2D1::RectF(pad + 2 * s, y, segX - 8 * s, y + pillH + 4 * s),
                     br.get());
        y += pillH + 12.0f * s;

        if (g_dlFan.files.empty()) {
            br->SetColor(fgSub);
            setEllip(fmtHead.get(), DWRITE_TEXT_ALIGNMENT_CENTER);
            PCWSTR emptyTxt = L"Folder is empty";
            dc->DrawText(emptyTxt, static_cast<UINT32>(wcslen(emptyTxt)),
                         fmtHead.get(),
                         D2D1::RectF(pad, y, W - pad, y + 40.0f * s), br.get());
            y += 46.0f * s;
        } else if (g_dlFan.viewMode == 1) {
            // GRID VIEW (macOS 26 Tahoe Liquid Glass 4-column thumbnail grid)
            const int cols = 4;
            const float cellGap = 8.0f * s;
            const float cellW =
                (W - pad * 2.0f - cellGap * (cols - 1)) / cols;
            const float cellH = 84.0f * s;
            const float icoSz = 44.0f * s;
            for (size_t i = 0; i < g_dlFan.files.size(); ++i) {
                int r = static_cast<int>(i) / cols;
                int c = static_cast<int>(i) % cols;
                float cx = pad + c * (cellW + cellGap);
                float cy = y + r * (cellH + cellGap);
                D2D1_RECT_F rc{cx, cy, cx + cellW, cy + cellH};
                g_dlFan.files[i].hitRect = rc;
                bool hov = (static_cast<int>(i) == g_dlFan.hoveredIdx);
                if (hov) {
                    br->SetColor(accCol);
                    dc->FillRoundedRectangle(
                        D2D1::RoundedRect(rc, 11 * s, 11 * s), br.get());
                } else {
                    br->SetColor(dark ? D2D1::ColorF(1, 1, 1, 0.045f)
                                      : D2D1::ColorF(1, 1, 1, 0.30f));
                    dc->FillRoundedRectangle(
                        D2D1::RoundedRect(rc, 11 * s, 11 * s), br.get());
                }
                float ix = cx + (cellW - icoSz) * 0.5f;
                float iy = cy + 8.0f * s;
                if (g_dlFan.files[i].icon) {
                    winrt::com_ptr<ID2D1Bitmap> bmp;
                    if (SUCCEEDED(dc->CreateBitmapFromWicBitmap(
                            g_dlFan.files[i].icon.get(), nullptr, bmp.put())) &&
                        bmp) {
                        dc->DrawBitmap(bmp.get(),
                                       D2D1::RectF(ix, iy, ix + icoSz, iy + icoSz),
                                       1.0f,
                                       D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                }
                br->SetColor(hov ? D2D1::ColorF(1, 1, 1, 0.98f) : fg);
                dc->DrawText(g_dlFan.files[i].name.c_str(),
                             static_cast<UINT32>(g_dlFan.files[i].name.size()),
                             fmtSmall.get(),
                             D2D1::RectF(cx + 4 * s, iy + icoSz + 3 * s,
                                         cx + cellW - 4 * s, cy + cellH - 3 * s),
                             br.get());
            }
            int rows =
                (static_cast<int>(g_dlFan.files.size()) + cols - 1) / cols;
            y += rows * cellH + std::max(0, rows - 1) * cellGap + 10.0f * s;
        } else {
            // FAN (curved arc) or LIST (clean flush rows + extension pill)
            const bool isFan = (g_dlFan.viewMode == 0);
            const float rowH = isFan ? (38.0f * s) : (32.0f * s);
            const float icoSz = isFan ? (26.0f * s) : (20.0f * s);
            size_t n = g_dlFan.files.size();
            for (size_t i = 0; i < n; ++i) {
                float arcShift = 0.0f;
                if (isFan && n > 1) {
                    float frac = 1.0f - static_cast<float>(i) / (n - 1);
                    arcShift = frac * frac * 12.0f * s;
                }
                D2D1_RECT_F rc{pad + arcShift, y, W - pad, y + rowH - 2.0f * s};
                g_dlFan.files[i].hitRect = rc;

                bool hov = (static_cast<int>(i) == g_dlFan.hoveredIdx);
                br->SetColor(hov ? accCol
                                 : (dark ? D2D1::ColorF(1, 1, 1, isFan ? 0.06f : 0.035f)
                                         : D2D1::ColorF(1, 1, 1, isFan ? 0.36f : 0.24f)));
                dc->FillRoundedRectangle(D2D1::RoundedRect(rc, 9 * s, 9 * s),
                                         br.get());

                float ix = rc.left + 8.0f * s;
                float iy = rc.top + ((rc.bottom - rc.top) - icoSz) * 0.5f;
                if (g_dlFan.files[i].icon) {
                    winrt::com_ptr<ID2D1Bitmap> bmp;
                    if (SUCCEEDED(dc->CreateBitmapFromWicBitmap(
                            g_dlFan.files[i].icon.get(), nullptr, bmp.put())) &&
                        bmp) {
                        dc->DrawBitmap(bmp.get(),
                                       D2D1::RectF(ix, iy, ix + icoSz, iy + icoSz),
                                       1.0f,
                                       D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    }
                }
                float rightLimit = rc.right - 8.0f * s;
                if (!isFan && !g_dlFan.files[i].ext.empty()) {
                    float extW = 46.0f * s;
                    br->SetColor(hov ? D2D1::ColorF(1, 1, 1, 0.85f) : fgSub);
                    dc->DrawText(g_dlFan.files[i].ext.c_str(),
                                 static_cast<UINT32>(g_dlFan.files[i].ext.size()),
                                 fmtSmall.get(),
                                 D2D1::RectF(rc.right - extW - 6 * s, rc.top,
                                             rc.right - 6 * s, rc.bottom),
                                 br.get());
                    rightLimit = rc.right - extW - 10.0f * s;
                }
                br->SetColor(hov ? D2D1::ColorF(1, 1, 1, 0.98f) : fg);
                dc->DrawText(g_dlFan.files[i].name.c_str(),
                             static_cast<UINT32>(g_dlFan.files[i].name.size()),
                             fmtName.get(),
                             D2D1::RectF(ix + icoSz + 10.0f * s, rc.top,
                                         rightLimit, rc.bottom),
                             br.get());
                y += rowH;
            }
            y += 6.0f * s;
        }

        // Bottom pill: "Open in Finder >"
        g_dlFan.openAllRect = {pad, y, W - pad, y + 28.0f * s};
        br->SetColor(g_dlFan.hoveredIdx == -2
                         ? accCol
                         : (dark ? D2D1::ColorF(1, 1, 1, 0.09f)
                                 : D2D1::ColorF(0, 0, 0, 0.07f)));
        dc->FillRoundedRectangle(
            D2D1::RoundedRect(g_dlFan.openAllRect, 9 * s, 9 * s), br.get());
        br->SetColor(g_dlFan.hoveredIdx == -2 ? D2D1::ColorF(1, 1, 1, 0.98f)
                                              : fg);
        setEllip(fmtHead.get(), DWRITE_TEXT_ALIGNMENT_CENTER);
        std::wstring openTxt = L"Open \"" + g_dlFan.folderName + L"\" in Finder \u203A";
        dc->DrawText(openTxt.c_str(), static_cast<UINT32>(openTxt.size()),
                     fmtHead.get(), g_dlFan.openAllRect, br.get());
    });
}

void OpenFolderStackPopup(const std::wstring& folderPath,
                          const std::wstring& folderName,
                          const std::wstring& stackId,
                          const D2D1_RECT_F& iconRectLocal);

LRESULT CALLBACK DownloadsFanWndProc(HWND hwnd, UINT msg, WPARAM wp,
                                     LPARAM lp) {
    try {
        switch (msg) {
            case WM_ACTIVATE:
                if (LOWORD(wp) == WA_INACTIVE) {
                    CloseDownloadsFan();
                }
                return 0;
            case WM_KEYDOWN:
                if (wp == VK_ESCAPE) {
                    CloseDownloadsFan();
                }
                return 0;
            case WM_MOUSEMOVE: {
                float lx = static_cast<float>(static_cast<short>(LOWORD(lp)));
                float ly = static_cast<float>(static_cast<short>(HIWORD(lp)));
                int hov = -1;
                for (int m = 0; m < 3; ++m) {
                    if (PointInRectF(g_dlFan.viewPillRects[m], lx, ly)) {
                        hov = -10 - m;
                        break;
                    }
                }
                if (hov == -1 && PointInRectF(g_dlFan.openAllRect, lx, ly)) {
                    hov = -2;
                } else if (hov == -1) {
                    for (size_t i = 0; i < g_dlFan.files.size(); ++i) {
                        if (PointInRectF(g_dlFan.files[i].hitRect, lx, ly)) {
                            hov = static_cast<int>(i);
                            break;
                        }
                    }
                }
                if (hov != g_dlFan.hoveredIdx) {
                    g_dlFan.hoveredIdx = hov;
                    RenderDownloadsFan();
                }
                return 0;
            }
            case WM_LBUTTONUP: {
                float lx = static_cast<float>(static_cast<short>(LOWORD(lp)));
                float ly = static_cast<float>(static_cast<short>(HIWORD(lp)));
                for (int m = 0; m < 3; ++m) {
                    if (PointInRectF(g_dlFan.viewPillRects[m], lx, ly)) {
                        if (g_dlFan.viewMode != m) {
                            auto fp = g_dlFan.folderPath;
                            auto fn = g_dlFan.folderName;
                            auto sid = g_dlFan.stackId;
                            auto ar = g_dlFan.anchorIconRect;
                            int sm = g_dlFan.sortMode;
                            if (sid == L"fixed:downloads") {
                                SetDownloadsStackConfig(m, sm);
                            } else {
                                for (auto& pf : g_pinnedFolders) {
                                    if (NormalizePath(pf.path) ==
                                        NormalizePath(fp)) {
                                        pf.viewMode = m;
                                        break;
                                    }
                                }
                                SavePinnedFolders();
                            }
                            CloseDownloadsFan();
                            OpenFolderStackPopup(fp, fn, sid, ar);
                        }
                        return 0;
                    }
                }
                if (PointInRectF(g_dlFan.openAllRect, lx, ly)) {
                    auto dir = g_dlFan.folderPath;
                    CloseDownloadsFan();
                    if (!dir.empty()) {
                        OpenShellUri(dir.c_str());
                    }
                    return 0;
                }
                for (const auto& f : g_dlFan.files) {
                    if (PointInRectF(f.hitRect, lx, ly)) {
                        std::wstring path = f.fullPath;
                        CloseDownloadsFan();
                        OpenShellUri(path.c_str());
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

void OpenFolderStackPopup(const std::wstring& folderPath,
                          const std::wstring& folderName,
                          const std::wstring& stackId,
                          const D2D1_RECT_F& iconRectLocal) {
    if (g_dlFan.open && g_dlFan.stackId == stackId) {
        CloseDownloadsFan();
        return;
    }
    CloseDownloadsFan();
    CloseAllMenus();

    if (folderPath.empty() || !g_dock.hwnd) {
        return;
    }

    int viewMode = 0, sortMode = 0;
    if (stackId == L"fixed:downloads") {
        GetDownloadsStackConfig(viewMode, sortMode);
    } else {
        for (const auto& pf : g_pinnedFolders) {
            if (NormalizePath(pf.path) == NormalizePath(folderPath)) {
                viewMode = pf.viewMode;
                sortMode = pf.sortMode;
                break;
            }
        }
    }

    std::vector<FanFileItem> found;
    WIN32_FIND_DATAW fd{};
    std::wstring pat = folderPath + L"\\*";
    HANDLE hf = FindFirstFileW(pat.c_str(), &fd);
    if (hf != INVALID_HANDLE_VALUE) {
        do {
            if (wcscmp(fd.cFileName, L".") == 0 ||
                wcscmp(fd.cFileName, L"..") == 0) {
                continue;
            }
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) {
                continue;
            }
            FanFileItem fi;
            fi.name = fd.cFileName;
            fi.fullPath = folderPath + L"\\" + fd.cFileName;
            fi.isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            fi.writeTime = fd.ftLastWriteTime;
            if (fi.isDir) {
                fi.ext = L"Folder";
            } else {
                const wchar_t* dot = wcsrchr(fd.cFileName, L'.');
                if (dot && dot[1]) {
                    fi.ext = dot + 1;
                    for (auto& ch : fi.ext) ch = static_cast<wchar_t>(towupper(ch));
                }
            }
            found.push_back(std::move(fi));
        } while (FindNextFileW(hf, &fd));
        FindClose(hf);
    }

    if (sortMode == 1) {
        std::sort(found.begin(), found.end(),
                  [](const FanFileItem& a, const FanFileItem& b) {
                      return _wcsicmp(a.name.c_str(), b.name.c_str()) < 0;
                  });
    } else if (sortMode == 2) {
        std::sort(found.begin(), found.end(),
                  [](const FanFileItem& a, const FanFileItem& b) {
                      int c = _wcsicmp(a.ext.c_str(), b.ext.c_str());
                      return c != 0 ? (c < 0)
                                    : (_wcsicmp(a.name.c_str(), b.name.c_str()) < 0);
                  });
    } else {
        std::sort(found.begin(), found.end(),
                  [](const FanFileItem& a, const FanFileItem& b) {
                      return CompareFileTime(&a.writeTime, &b.writeTime) > 0;
                  });
    }

    size_t cap = (viewMode == 1) ? 16u : (viewMode == 2 ? 14u : 10u);
    if (found.size() > cap) {
        found.resize(cap);
    }
    if (viewMode == 0 && sortMode == 0) {
        // Fan view: oldest top -> newest closest to Dock bottom
        std::reverse(found.begin(), found.end());
    }
    int icoReq = (viewMode == 1) ? 96 : 64;
    for (auto& f : found) {
        f.icon = LoadIconFromPath(f.fullPath, icoReq);
    }

    const float s = g_dock.scale;
    float w = 0.0f, h = 0.0f;
    if (viewMode == 1) {
        // Grid view
        w = std::ceil(412.0f * s);
        int rows = found.empty()
                       ? 0
                       : (static_cast<int>(found.size()) + 3) / 4;
        float contentH =
            found.empty()
                ? 46.0f * s
                : (rows * 84.0f * s + std::max(0, rows - 1) * 8.0f * s + 10.0f * s);
        h = std::ceil(11.0f * s + 34.0f * s + contentH + 28.0f * s + 12.0f * s);
    } else {
        w = std::ceil((viewMode == 2 ? 336.0f : 316.0f) * s);
        float rowH = (viewMode == 0 ? 38.0f : 32.0f) * s;
        float contentH =
            found.empty()
                ? 46.0f * s
                : (static_cast<float>(found.size()) * rowH + 6.0f * s);
        h = std::ceil(11.0f * s + 34.0f * s + contentH + 28.0f * s + 12.0f * s);
    }

    const RECT& wr = g_dock.windowRect;
    const RECT& mr = g_dock.monitorRect;
    int iconCx =
        wr.left +
        static_cast<int>((iconRectLocal.left + iconRectLocal.right) * 0.5f);
    int anchorY = wr.top + static_cast<int>(iconRectLocal.top - 10.0f * s);
    int px = std::clamp(iconCx - static_cast<int>(w * 0.5f),
                        static_cast<int>(mr.left + 8),
                        std::max(static_cast<int>(mr.left + 8),
                                 static_cast<int>(mr.right - w - 8)));
    int py = std::clamp(anchorY - static_cast<int>(h),
                        static_cast<int>(mr.top + 36),
                        std::max(static_cast<int>(mr.top + 36),
                                 static_cast<int>(mr.bottom - h - 8)));

    HMODULE hSelf = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&OpenFolderStackPopup),
                       &hSelf);
    static bool s_reg = false;
    if (!s_reg) {
        WNDCLASSEXW wc{.cbSize = sizeof(wc),
                       .lpfnWndProc = DownloadsFanWndProc,
                       .hInstance = hSelf,
                       .hCursor = LoadCursorW(nullptr, IDC_ARROW),
                       .lpszClassName = kDownloadsFanClassName};
        RegisterClassExW(&wc);
        s_reg = true;
    }

    g_dlFan.hwnd = CreateWindowExW(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_LAYERED | WS_EX_TOPMOST |
            WS_EX_TOOLWINDOW,
        kDownloadsFanClassName, L"", WS_POPUP, px, py, static_cast<int>(w),
        static_cast<int>(h), nullptr, nullptr, hSelf, nullptr);
    if (!g_dlFan.hwnd) {
        return;
    }
    g_dlFan.folderPath = folderPath;
    g_dlFan.folderName = folderName.empty() ? L"Folder" : folderName;
    g_dlFan.stackId = stackId;
    g_dlFan.anchorIconRect = iconRectLocal;
    g_dlFan.viewMode = viewMode;
    g_dlFan.sortMode = sortMode;
    g_dlFan.widthPx = w;
    g_dlFan.heightPx = h;
    g_dlFan.hoveredIdx = -1;
    g_dlFan.files = std::move(found);
    g_dlFan.open = true;
    g_dock.modalActive = true;

    InitGlassPopupWindowShell(g_dlFan.hwnd, w, h, 18.0f * s, px, py,
                              g_dock.darkMode ? 0.16f : 0.14f, g_dlFan.target,
                              g_dlFan.root, g_dlFan.glassContainer,
                              g_dlFan.clipGeom, g_dlFan.fallbackBackdrop,
                              g_dlFan.fallbackTint, g_dlFan.liquidPlate,
                              g_dlFan.uiSurface, g_dlFan.uiBrush,
                              g_dlFan.uiVisual);
    RenderDownloadsFan();
    ShowWindow(g_dlFan.hwnd, SW_SHOW);
    SetForegroundWindow(g_dlFan.hwnd);
}

void RegisterShellNotifications(HWND hwnd) {
    if (!hwnd) {
        return;
    }
    if (!g_dlFan.shellNotifyTrash) {
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (SUCCEEDED(SHGetKnownFolderIDList(FOLDERID_RecycleBinFolder, 0,
                                             nullptr, &pidl)) &&
            pidl) {
            SHChangeNotifyEntry entry{pidl, TRUE};
            g_dlFan.shellNotifyTrash = SHChangeNotifyRegister(
                hwnd, SHCNRF_ShellLevel | SHCNRF_InterruptLevel,
                SHCNE_UPDATEITEM | SHCNE_DELETE | SHCNE_CREATE | SHCNE_RMDIR |
                    SHCNE_RENAMEITEM | SHCNE_UPDATEDIR,
                kWmAppShellNotify, 1, &entry);
            CoTaskMemFree(pidl);
        }
    }
    if (!g_dlFan.shellNotifyDownloads) {
        auto dl = GetEffectiveDownloadsPath();
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (!dl.empty() &&
            SUCCEEDED(SHParseDisplayName(dl.c_str(), nullptr, &pidl, 0,
                                         nullptr)) &&
            pidl) {
            SHChangeNotifyEntry entry{pidl, FALSE};
            g_dlFan.shellNotifyDownloads = SHChangeNotifyRegister(
                hwnd, SHCNRF_ShellLevel | SHCNRF_InterruptLevel,
                SHCNE_CREATE | SHCNE_DELETE | SHCNE_RENAMEITEM | SHCNE_UPDATEDIR,
                kWmAppShellNotify, 1, &entry);
            CoTaskMemFree(pidl);
        }
    }
    if (!g_dlFan.shellNotifyTaskbar) {
        auto tbDir = GetUserPinnedTaskbarDir();
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (!tbDir.empty() &&
            SUCCEEDED(SHParseDisplayName(tbDir.c_str(), nullptr, &pidl, 0,
                                         nullptr)) &&
            pidl) {
            SHChangeNotifyEntry entry{pidl, FALSE};
            g_dlFan.shellNotifyTaskbar = SHChangeNotifyRegister(
                hwnd, SHCNRF_ShellLevel | SHCNRF_InterruptLevel,
                SHCNE_CREATE | SHCNE_DELETE | SHCNE_RENAMEITEM |
                    SHCNE_UPDATEITEM | SHCNE_UPDATEDIR,
                kWmAppTaskbarPinsChanged, 1, &entry);
            CoTaskMemFree(pidl);
        }
    }
}

void UnregisterShellNotifications() {
    if (g_dlFan.shellNotifyTrash) {
        SHChangeNotifyDeregister(g_dlFan.shellNotifyTrash);
        g_dlFan.shellNotifyTrash = 0;
    }
    if (g_dlFan.shellNotifyDownloads) {
        SHChangeNotifyDeregister(g_dlFan.shellNotifyDownloads);
        g_dlFan.shellNotifyDownloads = 0;
    }
    if (g_dlFan.shellNotifyTaskbar) {
        SHChangeNotifyDeregister(g_dlFan.shellNotifyTaskbar);
        g_dlFan.shellNotifyTaskbar = 0;
    }
}

// ---- M8. Live Window Preview Flyout on Hover (macOS 27 Clear/Dark Glass) ----

struct FlyoutPreviewCard {
    HWND targetHwnd = nullptr;
    HTHUMBNAIL thumb = nullptr;
    std::wstring title;
    D2D1_RECT_F cardRect{};
    D2D1_RECT_F wellRect{};
    D2D1_RECT_F closeRect{};
    RECT thumbDestRect{};
    bool hasThumb = false;
    bool isActive = false;
    bool isMinimized = false;
};

struct WindowPreviewFlyoutState {
    HWND bgHwnd = nullptr;
    HWND thumbHwnd = nullptr;
    bool open = false;
    bool trackingMouse = false;
    std::wstring appIdentityKey;
    std::wstring appDisplayName;
    std::wstring pendingIdentityKey;
    int totalWindowCount = 0;

    float widthPx = 0.0f;
    float heightPx = 0.0f;
    RECT windowRect{};
    RECT iconScreenRect{};

    wuc::Desktop::DesktopWindowTarget target{nullptr};
    wuc::ContainerVisual root{nullptr};
    wuc::ContainerVisual glassContainer{nullptr};
    wuc::CompositionPathGeometry clipGeom{nullptr};
    wuc::SpriteVisual fallbackBackdrop{nullptr};
    wuc::SpriteVisual fallbackTint{nullptr};
    LiquidGlassPlate liquidPlate;
    LiquidGlassStyleParams lastGlassParams{};
    bool hasGlassParams = false;
    wuc::CompositionDrawingSurface uiSurface{nullptr};
    wuc::CompositionSurfaceBrush uiBrush{nullptr};
    wuc::SpriteVisual uiVisual{nullptr};

    winrt::com_ptr<ID2D1HwndRenderTarget> thumbRt;
    winrt::com_ptr<IDWriteTextFormat> fmtHeader;
    winrt::com_ptr<IDWriteTextFormat> fmtSub;
    winrt::com_ptr<IDWriteTextFormat> fmtTitle;
    float cachedFmtScale = 0.0f;

    std::vector<FlyoutPreviewCard> cards;
    int hoveredCard = -1;
    bool hoveredClose = false;
};

WindowPreviewFlyoutState g_flyout;
constexpr WCHAR kFlyoutBgClassName[] = L"WindhawkMacOS27FlyoutBgWnd";
constexpr WCHAR kFlyoutThumbClassName[] = L"WindhawkMacOS27FlyoutThumbWnd";

bool IsWindowPreviewFlyoutOpen() {
    return g_flyout.open;
}

void EnsureFlyoutTextFormats(float s) {
    if (!g_gfx.dwriteFactory) {
        return;
    }
    if (g_flyout.fmtHeader && std::fabs(g_flyout.cachedFmtScale - s) < 0.001f) {
        return;
    }
    g_flyout.fmtHeader = nullptr;
    g_flyout.fmtSub = nullptr;
    g_flyout.fmtTitle = nullptr;
    g_flyout.cachedFmtScale = s;

    const auto& fam = ResolveFontFamily();
    g_gfx.dwriteFactory->CreateTextFormat(
        fam.c_str(), nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.5f * s,
        L"en-us", g_flyout.fmtHeader.put());
    g_gfx.dwriteFactory->CreateTextFormat(
        fam.c_str(), nullptr, DWRITE_FONT_WEIGHT_MEDIUM,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 11.0f * s,
        L"en-us", g_flyout.fmtSub.put());
    g_gfx.dwriteFactory->CreateTextFormat(
        fam.c_str(), nullptr, DWRITE_FONT_WEIGHT_MEDIUM,
        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 11.5f * s,
        L"en-us", g_flyout.fmtTitle.put());

    auto setupEllipsis = [](IDWriteTextFormat* fmt) {
        if (!fmt) return;
        fmt->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
        winrt::com_ptr<IDWriteInlineObject> ell;
        if (SUCCEEDED(
                g_gfx.dwriteFactory->CreateEllipsisTrimmingSign(fmt, ell.put())) &&
            ell) {
            DWRITE_TRIMMING trim{DWRITE_TRIMMING_GRANULARITY_CHARACTER, 0, 0};
            fmt->SetTrimming(&trim, ell.get());
        }
    };
    setupEllipsis(g_flyout.fmtHeader.get());
    setupEllipsis(g_flyout.fmtSub.get());
    setupEllipsis(g_flyout.fmtTitle.get());
    if (g_flyout.fmtSub) {
        g_flyout.fmtSub->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
    }
}

void UnregisterFlyoutThumbnails() {
    for (auto& c : g_flyout.cards) {
        if (c.thumb) {
            DwmUnregisterThumbnail(c.thumb);
            c.thumb = nullptr;
        }
        c.hasThumb = false;
    }
}

void UpdateFlyoutThumbnailOpacities() {
    for (size_t i = 0; i < g_flyout.cards.size(); ++i) {
        const auto& c = g_flyout.cards[i];
        if (!c.thumb || !c.hasThumb) {
            continue;
        }
        bool hov = (static_cast<int>(i) == g_flyout.hoveredCard);
        DWM_THUMBNAIL_PROPERTIES props{};
        props.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE |
                        DWM_TNP_OPACITY | DWM_TNP_SOURCECLIENTAREAONLY;
        props.rcDestination = c.thumbDestRect;
        props.fVisible = TRUE;
        props.fSourceClientAreaOnly = FALSE;
        props.opacity = hov ? 255 : (c.isActive ? 248 : 234);
        DwmUpdateThumbnailProperties(c.thumb, &props);
    }
}

void RenderFlyoutBackground() {
    if (!g_flyout.open || !g_flyout.uiSurface || !g_gfx.dwriteFactory) {
        return;
    }
    const float s = g_dock.scale;
    const float W = g_flyout.widthPx;
    const bool dark = g_dock.darkMode;
    EnsureFlyoutTextFormats(s);

    auto acc = GetAccentColor();
    D2D1_COLOR_F accCol =
        D2D1::ColorF(acc.R / 255.0f, acc.G / 255.0f, acc.B / 255.0f, 1.0f);
    D2D1_COLOR_F fg = dark ? D2D1::ColorF(0.96f, 0.98f, 1.0f, 0.95f)
                           : D2D1::ColorF(0.08f, 0.09f, 0.12f, 0.94f);
    D2D1_COLOR_F fgSub = dark ? D2D1::ColorF(0.84f, 0.90f, 0.98f, 0.60f)
                              : D2D1::ColorF(0.10f, 0.12f, 0.16f, 0.56f);

    DrawCompositionSurface(g_flyout.uiSurface, [&](ID2D1DeviceContext* dc) {
        winrt::com_ptr<ID2D1SolidColorBrush> br;
        dc->CreateSolidColorBrush(fg, br.put());
        if (!br) {
            return;
        }

        const float padX = 12.0f * s;
        const float headerH = 26.0f * s;

        // Top header row: App name + window count
        if (g_flyout.fmtHeader) {
            br->SetColor(fg);
            D2D1_RECT_F hr{padX, 6.0f * s, W - padX - 88.0f * s,
                           6.0f * s + headerH};
            dc->DrawText(g_flyout.appDisplayName.c_str(),
                         static_cast<UINT32>(g_flyout.appDisplayName.size()),
                         g_flyout.fmtHeader.get(), hr, br.get());
        }
        if (g_flyout.fmtSub) {
            std::wstring sub;
            if (g_flyout.totalWindowCount >
                static_cast<int>(g_flyout.cards.size())) {
                sub = std::to_wstring(g_flyout.totalWindowCount) + L" windows";
            } else if (g_flyout.cards.size() > 1) {
                sub = std::to_wstring(g_flyout.cards.size()) + L" windows";
            } else if (!g_flyout.cards.empty() && g_flyout.cards[0].isMinimized) {
                sub = L"Minimized";
            } else {
                sub = L"Open";
            }
            br->SetColor(fgSub);
            D2D1_RECT_F sr{W - padX - 96.0f * s, 6.0f * s, W - padX,
                           6.0f * s + headerH};
            dc->DrawText(sub.c_str(), static_cast<UINT32>(sub.size()),
                         g_flyout.fmtSub.get(), sr, br.get());
        }

        // Subtle glass divider line under header
        br->SetColor(dark ? D2D1::ColorF(0.90f, 0.95f, 1.0f, 0.12f)
                          : D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.08f));
        dc->DrawLine(D2D1::Point2F(padX, 6.0f * s + headerH + 1.0f * s),
                     D2D1::Point2F(W - padX, 6.0f * s + headerH + 1.0f * s),
                     br.get(), 0.85f * s);

        // Preview cards
        for (size_t i = 0; i < g_flyout.cards.size(); ++i) {
            const auto& c = g_flyout.cards[i];
            bool hov = (static_cast<int>(i) == g_flyout.hoveredCard);
            bool hovClose = hov && g_flyout.hoveredClose;

            D2D1_ROUNDED_RECT cardRR =
                D2D1::RoundedRect(c.cardRect, 10.0f * s, 10.0f * s);

            // Card glass fill
            if (hov) {
                br->SetColor(dark ? D2D1::ColorF(0.86f, 0.92f, 1.0f, 0.15f)
                                  : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.64f));
            } else if (c.isActive) {
                br->SetColor(dark ? D2D1::ColorF(0.76f, 0.86f, 1.0f, 0.10f)
                                  : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.48f));
            } else {
                br->SetColor(dark ? D2D1::ColorF(0.04f, 0.06f, 0.10f, 0.28f)
                                  : D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.30f));
            }
            dc->FillRoundedRectangle(cardRR, br.get());

            // Crisp specular card border
            if (hov) {
                br->SetColor(dark ? D2D1::ColorF(0.94f, 0.97f, 1.0f, 0.56f)
                                  : D2D1::ColorF(accCol.r, accCol.g, accCol.b,
                                                 0.72f));
            } else if (c.isActive) {
                br->SetColor(dark ? D2D1::ColorF(0.88f, 0.94f, 1.0f, 0.30f)
                                  : D2D1::ColorF(accCol.r, accCol.g, accCol.b,
                                                 0.42f));
            } else {
                br->SetColor(dark ? D2D1::ColorF(0.86f, 0.92f, 1.0f, 0.14f)
                                  : D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.10f));
            }
            dc->DrawRoundedRectangle(cardRR, br.get(),
                                     hov ? 1.15f * s : 0.85f * s);

            // Card title text (left of close button)
            if (g_flyout.fmtTitle) {
                br->SetColor(c.isMinimized ? fgSub : fg);
                D2D1_RECT_F tr{c.cardRect.left + 8.0f * s,
                               c.cardRect.top + 3.0f * s,
                               c.closeRect.left - 4.0f * s,
                               c.cardRect.top + 25.0f * s};
                dc->DrawText(c.title.c_str(),
                             static_cast<UINT32>(c.title.size()),
                             g_flyout.fmtTitle.get(), tr, br.get());
            }

            // Close button (top-right circular macOS 'x')
            float ccx = (c.closeRect.left + c.closeRect.right) * 0.5f;
            float ccy = (c.closeRect.top + c.closeRect.bottom) * 0.5f;
            float cr = (c.closeRect.right - c.closeRect.left) * 0.5f;
            if (hovClose) {
                br->SetColor(D2D1::ColorF(1.0f, 0.37f, 0.34f, 0.96f));
                dc->FillEllipse(D2D1::Ellipse(D2D1::Point2F(ccx, ccy), cr, cr),
                                br.get());
                br->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.98f));
            } else if (hov) {
                br->SetColor(dark ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.16f)
                                  : D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.10f));
                dc->FillEllipse(D2D1::Ellipse(D2D1::Point2F(ccx, ccy), cr, cr),
                                br.get());
                br->SetColor(dark ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.85f)
                                  : D2D1::ColorF(0.12f, 0.12f, 0.15f, 0.78f));
            } else {
                br->SetColor(dark ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.38f)
                                  : D2D1::ColorF(0.12f, 0.12f, 0.15f, 0.34f));
            }
            float xArm = cr * 0.44f;
            float xStroke = hovClose ? 1.55f * s : 1.25f * s;
            dc->DrawLine(D2D1::Point2F(ccx - xArm, ccy - xArm),
                         D2D1::Point2F(ccx + xArm, ccy + xArm), br.get(),
                         xStroke);
            dc->DrawLine(D2D1::Point2F(ccx + xArm, ccy - xArm),
                         D2D1::Point2F(ccx - xArm, ccy + xArm), br.get(),
                         xStroke);

            // Recessed thumbnail well
            D2D1_ROUNDED_RECT wellRR =
                D2D1::RoundedRect(c.wellRect, 6.0f * s, 6.0f * s);
            br->SetColor(dark ? D2D1::ColorF(0.02f, 0.03f, 0.06f, 0.44f)
                              : D2D1::ColorF(0.06f, 0.08f, 0.12f, 0.12f));
            dc->FillRoundedRectangle(wellRR, br.get());
            br->SetColor(dark ? D2D1::ColorF(0.90f, 0.95f, 1.0f, 0.14f)
                              : D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.10f));
            dc->DrawRoundedRectangle(wellRR, br.get(), 0.75f * s);

            // Fallback label if DWM thumbnail unavailable (e.g. minimized without cache)
            if (!c.hasThumb && g_flyout.fmtTitle) {
                br->SetColor(fgSub);
                g_flyout.fmtTitle->SetTextAlignment(
                    DWRITE_TEXT_ALIGNMENT_CENTER);
                PCWSTR msg = c.isMinimized ? L"Minimized Window" : L"Preview";
                dc->DrawText(msg, static_cast<UINT32>(wcslen(msg)),
                             g_flyout.fmtTitle.get(), c.wellRect, br.get());
                g_flyout.fmtTitle->SetTextAlignment(
                    DWRITE_TEXT_ALIGNMENT_LEADING);
            }
        }
    });
}

void CloseWindowPreviewFlyout() {
    if (g_dock.hwnd) {
        KillTimer(g_dock.hwnd, kTimerFlyoutHover);
        KillTimer(g_dock.hwnd, kTimerFlyoutLeave);
    }
    g_flyout.pendingIdentityKey.clear();
    if (!g_flyout.open && !g_flyout.bgHwnd && !g_flyout.thumbHwnd) {
        return;
    }
    g_flyout.open = false;
    g_flyout.trackingMouse = false;
    g_flyout.hasGlassParams = false;
    g_flyout.hoveredCard = -1;
    g_flyout.hoveredClose = false;
    g_flyout.appIdentityKey.clear();
    g_flyout.appDisplayName.clear();
    g_flyout.windowRect = {};
    g_flyout.iconScreenRect = {};

    UnregisterFlyoutThumbnails();
    g_flyout.cards.clear();
    g_flyout.thumbRt = nullptr;

    if (g_flyout.thumbHwnd) {
        DestroyWindow(g_flyout.thumbHwnd);
        g_flyout.thumbHwnd = nullptr;
    }
    g_flyout.liquidPlate.Shutdown();
    g_flyout.uiVisual = nullptr;
    g_flyout.uiBrush = nullptr;
    g_flyout.uiSurface = nullptr;
    g_flyout.fallbackTint = nullptr;
    g_flyout.fallbackBackdrop = nullptr;
    g_flyout.clipGeom = nullptr;
    g_flyout.glassContainer = nullptr;
    g_flyout.root = nullptr;
    g_flyout.target = nullptr;
    if (g_flyout.bgHwnd) {
        DestroyWindow(g_flyout.bgHwnd);
        g_flyout.bgHwnd = nullptr;
    }
    RequestAnimationFrame();
}

void CloseTahoeSettingsPanel();

void ResetFlyoutGpuResources() {
    CloseWindowPreviewFlyout();
    CloseTahoeSettingsPanel();
    g_flyout.fmtHeader = nullptr;
    g_flyout.fmtSub = nullptr;
    g_flyout.fmtTitle = nullptr;
    g_flyout.cachedFmtScale = 0.0f;
}

void AddFlyoutWatchedRegion(std::vector<RECT>& watchList) {
    if (g_flyout.open && g_flyout.windowRect.right > g_flyout.windowRect.left) {
        RECT r = g_flyout.windowRect;
        InflateRect(&r, 16, 16);
        watchList.push_back(r);
    }
}

void PaintFlyoutThumbWindowTransparent(HWND hwnd) {
    PAINTSTRUCT ps{};
    BeginPaint(hwnd, &ps);
    RECT rc{};
    GetClientRect(hwnd, &rc);
    UINT w = static_cast<UINT>(std::max<LONG>(1, rc.right - rc.left));
    UINT h = static_cast<UINT>(std::max<LONG>(1, rc.bottom - rc.top));
    if (g_gfx.d2dFactory) {
        if (!g_flyout.thumbRt) {
            auto rtProps = D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                96.0f, 96.0f);
            auto hwndProps = D2D1::HwndRenderTargetProperties(
                hwnd, D2D1::SizeU(w, h), D2D1_PRESENT_OPTIONS_IMMEDIATELY);
            g_gfx.d2dFactory->CreateHwndRenderTarget(rtProps, hwndProps,
                                                     g_flyout.thumbRt.put());
        }
        if (g_flyout.thumbRt) {
            g_flyout.thumbRt->BeginDraw();
            g_flyout.thumbRt->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
            HRESULT hr = g_flyout.thumbRt->EndDraw();
            if (hr == static_cast<HRESULT>(D2DERR_RECREATE_TARGET)) {
                g_flyout.thumbRt = nullptr;
            }
        }
    }
    EndPaint(hwnd, &ps);
}

LRESULT CALLBACK FlyoutThumbWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_PAINT:
            PaintFlyoutThumbWindowTransparent(hwnd);
            return 0;
        case WM_MOUSEMOVE: {
            if (!g_flyout.trackingMouse) {
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
                g_flyout.trackingMouse = true;
            }
            if (g_dock.hwnd) {
                KillTimer(g_dock.hwnd, kTimerFlyoutLeave);
            }
            float mx = static_cast<float>(static_cast<short>(LOWORD(lp)));
            float my = static_cast<float>(static_cast<short>(HIWORD(lp)));
            int newCard = -1;
            bool newClose = false;
            for (size_t i = 0; i < g_flyout.cards.size(); ++i) {
                if (PointInRectF(g_flyout.cards[i].cardRect, mx, my)) {
                    newCard = static_cast<int>(i);
                    newClose =
                        PointInRectF(g_flyout.cards[i].closeRect, mx, my);
                    break;
                }
            }
            if (newCard != g_flyout.hoveredCard ||
                newClose != g_flyout.hoveredClose) {
                g_flyout.hoveredCard = newCard;
                g_flyout.hoveredClose = newClose;
                RenderFlyoutBackground();
                UpdateFlyoutThumbnailOpacities();
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            g_flyout.trackingMouse = false;
            if (g_flyout.hoveredCard != -1 || g_flyout.hoveredClose) {
                g_flyout.hoveredCard = -1;
                g_flyout.hoveredClose = false;
                RenderFlyoutBackground();
                UpdateFlyoutThumbnailOpacities();
            }
            if (g_dock.hwnd && g_flyout.open) {
                SetTimer(g_dock.hwnd, kTimerFlyoutLeave, 220, nullptr);
            }
            return 0;
        case WM_LBUTTONUP: {
            float mx = static_cast<float>(static_cast<short>(LOWORD(lp)));
            float my = static_cast<float>(static_cast<short>(HIWORD(lp)));
            for (size_t i = 0; i < g_flyout.cards.size(); ++i) {
                if (PointInRectF(g_flyout.cards[i].cardRect, mx, my)) {
                    HWND target = g_flyout.cards[i].targetHwnd;
                    bool onClose =
                        PointInRectF(g_flyout.cards[i].closeRect, mx, my);
                    if (onClose) {
                        if (target && IsWindow(target)) {
                            PostMessageW(target, WM_CLOSE, 0, 0);
                        }
                        if (g_flyout.cards.size() <= 1) {
                            CloseWindowPreviewFlyout();
                        }
                        ScheduleWindowRefresh();
                    } else {
                        CloseWindowPreviewFlyout();
                        if (target && IsWindow(target)) {
                            ActivateWindowForce(target);
                        }
                    }
                    return 0;
                }
            }
            return 0;
        }
        case WM_MBUTTONUP: {
            float mx = static_cast<float>(static_cast<short>(LOWORD(lp)));
            float my = static_cast<float>(static_cast<short>(HIWORD(lp)));
            for (const auto& c : g_flyout.cards) {
                if (PointInRectF(c.cardRect, mx, my) && c.targetHwnd &&
                    IsWindow(c.targetHwnd)) {
                    PostMessageW(c.targetHwnd, WM_CLOSE, 0, 0);
                    if (g_flyout.cards.size() <= 1) {
                        CloseWindowPreviewFlyout();
                    }
                    ScheduleWindowRefresh();
                    return 0;
                }
            }
            return 0;
        }
        case WM_RBUTTONUP:
            CloseWindowPreviewFlyout();
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void OpenWindowPreviewFlyoutForItem(const DockItem& item) {
    if (!g_settings.showWindowPreviews || g_dock.modalActive || g_drag.active ||
        g_dock.fullscreenHidden || g_dock.hideProgress > 0.25f ||
        !g_gfx.compositor || !g_dock.hwnd) {
        return;
    }

    // Collect valid open windows for this item
    std::vector<HWND> validWins;
    validWins.reserve(item.windows.size());
    for (HWND h : item.windows) {
        if (h && IsWindow(h)) {
            validWins.push_back(h);
        }
    }
    if (validWins.empty()) {
        CloseWindowPreviewFlyout();
        return;
    }

    CloseWindowPreviewFlyout();

    const float s = g_dock.scale;
    const RECT& mr = g_dock.monitorRect;
    const RECT& wr = g_dock.windowRect;
    const bool horizDock = (g_settings.position == DockPosition::Bottom);

    constexpr size_t kMaxCards = 6;
    const size_t nCards = std::min(validWins.size(), kMaxCards);
    const float outerPad = 12.0f * s;
    const float headerH = 28.0f * s;
    const float gap = 10.0f * s;

    float cardW = 216.0f * s;
    float cardH = 148.0f * s;
    const float maxTotalW =
        std::max(260.0f * s, static_cast<float>(mr.right - mr.left) * 0.88f);
    const float maxTotalH =
        std::max(200.0f * s, static_cast<float>(mr.bottom - mr.top) * 0.80f);

    bool stackVert = !horizDock && nCards > 2;
    float totalW = 0.0f, totalH = 0.0f;
    if (!stackVert) {
        float availPerCard =
            (maxTotalW - outerPad * 2.0f - gap * static_cast<float>(nCards - 1)) /
            static_cast<float>(nCards);
        if (availPerCard < cardW) {
            float ratio = std::max(0.62f, availPerCard / cardW);
            cardW *= ratio;
            cardH *= ratio;
        }
        totalW = outerPad * 2.0f + cardW * static_cast<float>(nCards) +
                 gap * static_cast<float>(nCards - 1);
        totalH = outerPad + headerH + 6.0f * s + cardH + outerPad;
    } else {
        float availPerCard =
            (maxTotalH - outerPad * 2.0f - headerH - 6.0f * s -
             gap * static_cast<float>(nCards - 1)) /
            static_cast<float>(nCards);
        if (availPerCard < cardH) {
            float ratio = std::max(0.55f, availPerCard / cardH);
            cardW *= ratio;
            cardH *= ratio;
        }
        totalW = outerPad * 2.0f + cardW;
        totalH = outerPad + headerH + 6.0f * s +
                 cardH * static_cast<float>(nCards) +
                 gap * static_cast<float>(nCards - 1) + outerPad;
    }

    int W = static_cast<int>(std::ceil(totalW));
    int H = static_cast<int>(std::ceil(totalH));

    // Anchor cleanly above (or beside) the hovered Dock icon
    RECT iconScr{
        wr.left + static_cast<LONG>(std::floor(item.iconRect.left)),
        wr.top + static_cast<LONG>(std::floor(item.iconRect.top)),
        wr.left + static_cast<LONG>(std::ceil(item.iconRect.right)),
        wr.top + static_cast<LONG>(std::ceil(item.iconRect.bottom))};
    int px = 0, py = 0;
    const int anchorGap = static_cast<int>(std::ceil(10.0f * s));
    if (g_settings.position == DockPosition::Bottom) {
        int iconCx = (iconScr.left + iconScr.right) / 2;
        int topAnchor = std::min(
            iconScr.top,
            wr.top + static_cast<LONG>(std::floor(g_dock.shelfRect.top)));
        px = std::clamp(iconCx - W / 2, static_cast<int>(mr.left + 10 * s),
                        std::max(static_cast<int>(mr.left + 10 * s),
                                 static_cast<int>(mr.right - W - 10 * s)));
        py = std::clamp(topAnchor - anchorGap - H,
                        static_cast<int>(mr.top + 38 * s),
                        std::max(static_cast<int>(mr.top + 38 * s),
                                 static_cast<int>(mr.bottom - H - 10 * s)));
    } else if (g_settings.position == DockPosition::Left) {
        int iconCy = (iconScr.top + iconScr.bottom) / 2;
        int rightAnchor = std::max(
            iconScr.right,
            wr.left + static_cast<LONG>(std::ceil(g_dock.shelfRect.right)));
        px = std::clamp(rightAnchor + anchorGap,
                        static_cast<int>(mr.left + 10 * s),
                        std::max(static_cast<int>(mr.left + 10 * s),
                                 static_cast<int>(mr.right - W - 10 * s)));
        py = std::clamp(iconCy - H / 2, static_cast<int>(mr.top + 38 * s),
                        std::max(static_cast<int>(mr.top + 38 * s),
                                 static_cast<int>(mr.bottom - H - 10 * s)));
    } else {
        int iconCy = (iconScr.top + iconScr.bottom) / 2;
        int leftAnchor = std::min(
            iconScr.left,
            wr.left + static_cast<LONG>(std::floor(g_dock.shelfRect.left)));
        px = std::clamp(leftAnchor - anchorGap - W,
                        static_cast<int>(mr.left + 10 * s),
                        std::max(static_cast<int>(mr.left + 10 * s),
                                 static_cast<int>(mr.right - W - 10 * s)));
        py = std::clamp(iconCy - H / 2, static_cast<int>(mr.top + 38 * s),
                        std::max(static_cast<int>(mr.top + 38 * s),
                                 static_cast<int>(mr.bottom - H - 10 * s)));
    }

    HMODULE hSelf = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&OpenWindowPreviewFlyoutForItem),
                       &hSelf);
    static bool s_regFlyout = false;
    if (!s_regFlyout) {
        WNDCLASSEXW wcBg{.cbSize = sizeof(wcBg),
                         .lpfnWndProc = DefWindowProcW,
                         .hInstance = hSelf,
                         .hCursor = LoadCursorW(nullptr, IDC_ARROW),
                         .lpszClassName = kFlyoutBgClassName};
        RegisterClassExW(&wcBg);
        WNDCLASSEXW wcTh{.cbSize = sizeof(wcTh),
                         .lpfnWndProc = FlyoutThumbWndProc,
                         .hInstance = hSelf,
                         .hCursor = LoadCursorW(nullptr, IDC_ARROW),
                         .lpszClassName = kFlyoutThumbClassName};
        RegisterClassExW(&wcTh);
        s_regFlyout = true;
    }

    // 1. Optical Liquid Glass backdrop window (click-through, no-activate)
    g_flyout.bgHwnd = CreateWindowExW(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_LAYERED | WS_EX_TOPMOST |
            WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
        kFlyoutBgClassName, L"", WS_POPUP, px, py, W, H, nullptr, nullptr,
        hSelf, nullptr);
    if (!g_flyout.bgHwnd) {
        return;
    }

    // 2. DWM live-thumbnail overlay & mouse-input window
    g_flyout.thumbHwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kFlyoutThumbClassName, L"", WS_POPUP, px, py, W, H, nullptr, nullptr,
        hSelf, nullptr);
    if (!g_flyout.thumbHwnd) {
        DestroyWindow(g_flyout.bgHwnd);
        g_flyout.bgHwnd = nullptr;
        return;
    }

    // Strip any default Windows 11 DWM border/rounding/shadow from thumbHwnd
    MARGINS mAll{-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(g_flyout.thumbHwnd, &mAll);
    DWMNCRENDERINGPOLICY ncrp = DWMNCRP_DISABLED;
    DwmSetWindowAttribute(g_flyout.thumbHwnd, DWMWA_NCRENDERING_POLICY, &ncrp,
                          sizeof(ncrp));
    constexpr DWORD kDwmwaCornerPref = 33;
    constexpr DWORD kDwmwcpDoNotRound = 1;
    constexpr DWORD kDwmwaBorderCol = 34;
    constexpr COLORREF kDwmwaColorNone = 0xFFFFFFFEu;
    DWORD cp = kDwmwcpDoNotRound;
    DwmSetWindowAttribute(g_flyout.thumbHwnd, kDwmwaCornerPref, &cp,
                          sizeof(cp));
    COLORREF noBorder = kDwmwaColorNone;
    DwmSetWindowAttribute(g_flyout.thumbHwnd, kDwmwaBorderCol, &noBorder,
                          sizeof(noBorder));
    ApplyWindowGlassCaptureAffinity(g_flyout.thumbHwnd);

    g_flyout.open = true;
    g_flyout.appIdentityKey = item.identityKey;
    g_flyout.appDisplayName = item.name.empty() ? L"Application" : item.name;
    g_flyout.totalWindowCount = static_cast<int>(validWins.size());
    g_flyout.widthPx = static_cast<float>(W);
    g_flyout.heightPx = static_cast<float>(H);
    g_flyout.windowRect = {px, py, px + W, py + H};
    g_flyout.iconScreenRect = iconScr;
    g_flyout.hoveredCard = -1;
    g_flyout.hoveredClose = false;

    InitGlassPopupWindowShell(
        g_flyout.bgHwnd, static_cast<float>(W), static_cast<float>(H),
        16.0f * s, px, py, g_dock.darkMode ? 0.14f : 0.10f, g_flyout.target,
        g_flyout.root, g_flyout.glassContainer, g_flyout.clipGeom,
        g_flyout.fallbackBackdrop, g_flyout.fallbackTint, g_flyout.liquidPlate,
        g_flyout.uiSurface, g_flyout.uiBrush, g_flyout.uiVisual);

    // Cache optical params for fast video-behind-glass refresh
    g_flyout.lastGlassParams = {};
    g_flyout.lastGlassParams.screenX = static_cast<float>(px);
    g_flyout.lastGlassParams.screenY = static_cast<float>(py);
    g_flyout.lastGlassParams.widthPx = static_cast<float>(W);
    g_flyout.lastGlassParams.heightPx = static_cast<float>(H);
    g_flyout.lastGlassParams.cornerRadiusPx = 16.0f * s;
    g_flyout.lastGlassParams.scale = s;
    g_flyout.lastGlassParams.darkMode = g_dock.darkMode;
    g_flyout.lastGlassParams.useSquircle = true;
    g_flyout.lastGlassParams.tintBoost = g_dock.darkMode ? 0.14f : 0.10f;
    g_flyout.lastGlassParams.blurMul = 1.65f;
    g_flyout.lastGlassParams.rimDepthMul = 0.85f;
    g_flyout.lastGlassParams.strengthMul = 0.90f;
    g_flyout.hasGlassParams = true;

    // Build preview cards & register live DWM GPU thumbnails
    HWND fgHwnd = GetForegroundWindow();
    g_flyout.cards.clear();
    g_flyout.cards.reserve(nCards);
    const float cardsTop = outerPad + headerH + 6.0f * s;
    for (size_t i = 0; i < nCards; ++i) {
        HWND target = validWins[i];
        FlyoutPreviewCard card;
        card.targetHwnd = target;
        card.isActive = (target == fgHwnd || target == item.lastActive);
        card.isMinimized = (IsIconic(target) != FALSE);

        WCHAR titleBuf[256]{};
        if (GetWindowTextW(target, titleBuf, ARRAYSIZE(titleBuf)) > 0 &&
            titleBuf[0]) {
            card.title = titleBuf;
        } else {
            card.title = g_flyout.appDisplayName;
        }

        float cx = stackVert
                       ? outerPad
                       : (outerPad + static_cast<float>(i) * (cardW + gap));
        float cy = stackVert
                       ? (cardsTop + static_cast<float>(i) * (cardH + gap))
                       : cardsTop;
        card.cardRect = {cx, cy, cx + cardW, cy + cardH};

        float closeD = 16.0f * s;
        card.closeRect = {card.cardRect.right - 6.0f * s - closeD,
                          card.cardRect.top + 6.0f * s,
                          card.cardRect.right - 6.0f * s,
                          card.cardRect.top + 6.0f * s + closeD};

        card.wellRect = {card.cardRect.left + 6.0f * s,
                         card.cardRect.top + 27.0f * s,
                         card.cardRect.right - 6.0f * s,
                         card.cardRect.bottom - 6.0f * s};

        HTHUMBNAIL th = nullptr;
        if (SUCCEEDED(DwmRegisterThumbnail(g_flyout.thumbHwnd, target, &th)) &&
            th) {
            SIZE srcSz{};
            if (SUCCEEDED(DwmQueryThumbnailSourceSize(th, &srcSz)) &&
                srcSz.cx > 0 && srcSz.cy > 0) {
                const float wellPad = 3.0f * s;
                float maxW = std::max(
                    8.0f,
                    (card.wellRect.right - card.wellRect.left) - wellPad * 2.0f);
                float maxH = std::max(
                    8.0f,
                    (card.wellRect.bottom - card.wellRect.top) - wellPad * 2.0f);
                float scaleFit = std::min(maxW / static_cast<float>(srcSz.cx),
                                          maxH / static_cast<float>(srcSz.cy));
                float dw = std::max(8.0f, static_cast<float>(srcSz.cx) * scaleFit);
                float dh = std::max(8.0f, static_cast<float>(srcSz.cy) * scaleFit);
                float dx =
                    (card.wellRect.left + card.wellRect.right - dw) * 0.5f;
                float dy =
                    (card.wellRect.top + card.wellRect.bottom - dh) * 0.5f;
                card.thumbDestRect = {static_cast<LONG>(std::round(dx)),
                                      static_cast<LONG>(std::round(dy)),
                                      static_cast<LONG>(std::round(dx + dw)),
                                      static_cast<LONG>(std::round(dy + dh))};
                card.thumb = th;
                card.hasThumb = true;
            } else {
                DwmUnregisterThumbnail(th);
            }
        }
        g_flyout.cards.push_back(std::move(card));
    }

    RenderFlyoutBackground();
    UpdateFlyoutThumbnailOpacities();

    SetWindowPos(g_flyout.bgHwnd, HWND_TOPMOST, px, py, W, H,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    SetWindowPos(g_flyout.thumbHwnd, HWND_TOPMOST, px, py, W, H,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(g_flyout.thumbHwnd, nullptr, FALSE);
    UpdateWindow(g_flyout.thumbHwnd);

    // Hide dock text tooltip immediately & update DXGI watched rects
    RequestAnimationFrame();
}

void OnDockHoveredIndexChanged(int newHovered) {
    if (!g_dock.hwnd) {
        return;
    }
    if (!g_settings.showWindowPreviews || g_dock.modalActive || g_drag.active ||
        g_dock.fullscreenHidden || g_dock.hideProgress > 0.25f) {
        KillTimer(g_dock.hwnd, kTimerFlyoutHover);
        g_flyout.pendingIdentityKey.clear();
        if (g_flyout.open) {
            CloseWindowPreviewFlyout();
        }
        return;
    }

    bool validRunningApp = false;
    if (newHovered >= 0 && newHovered < static_cast<int>(g_items.size())) {
        const auto& it = g_items[newHovered];
        if ((it.kind == DockItemKind::App || it.kind == DockItemKind::Finder) &&
            !it.windows.empty()) {
            validRunningApp = true;
            KillTimer(g_dock.hwnd, kTimerFlyoutLeave);
            if (g_flyout.open) {
                if (g_flyout.appIdentityKey != it.identityKey) {
                    KillTimer(g_dock.hwnd, kTimerFlyoutHover);
                    OpenWindowPreviewFlyoutForItem(it);
                }
            } else {
                g_flyout.pendingIdentityKey = it.identityKey;
                SetTimer(g_dock.hwnd, kTimerFlyoutHover, 260, nullptr);
            }
        }
    }

    if (!validRunningApp) {
        KillTimer(g_dock.hwnd, kTimerFlyoutHover);
        g_flyout.pendingIdentityKey.clear();
        if (g_flyout.open) {
            SetTimer(g_dock.hwnd, kTimerFlyoutLeave, 240, nullptr);
        }
    }
}

void OnFlyoutHoverTimer() {
    if (g_dock.hwnd) {
        KillTimer(g_dock.hwnd, kTimerFlyoutHover);
    }
    if (!g_settings.showWindowPreviews || g_dock.modalActive || g_drag.active ||
        g_dock.fullscreenHidden || g_dock.hideProgress > 0.25f ||
        g_flyout.pendingIdentityKey.empty()) {
        return;
    }
    int idx = g_dock.hoveredIndex;
    if (idx < 0 || idx >= static_cast<int>(g_items.size())) {
        return;
    }
    const auto& it = g_items[idx];
    if (it.identityKey == g_flyout.pendingIdentityKey && !it.windows.empty()) {
        OpenWindowPreviewFlyoutForItem(it);
    }
}

bool IsCursorInFlyoutOrBridge(POINT pt) {
    if (!g_flyout.open) {
        return false;
    }
    RECT expanded = g_flyout.windowRect;
    InflateRect(&expanded, 8, 8);
    if (PtInRect(&expanded, pt)) {
        return true;
    }
    // Generous hover bridge between the hovered Dock icon and the flyout window
    RECT bridge{};
    bridge.left = std::min(g_flyout.windowRect.left, g_flyout.iconScreenRect.left) - 10;
    bridge.right =
        std::max(g_flyout.windowRect.right, g_flyout.iconScreenRect.right) + 10;
    bridge.top = std::min(g_flyout.windowRect.top, g_flyout.iconScreenRect.top) - 4;
    bridge.bottom =
        std::max(g_flyout.windowRect.bottom, g_flyout.iconScreenRect.bottom) + 4;
    return PtInRect(&bridge, pt) != FALSE;
}

void OnFlyoutLeaveTimer() {
    if (g_dock.hwnd) {
        KillTimer(g_dock.hwnd, kTimerFlyoutLeave);
    }
    if (!g_flyout.open) {
        return;
    }
    POINT pt{};
    if (GetCursorPos(&pt)) {
        // Still hovering the same app icon on the Dock?
        if (g_dock.hoveredIndex >= 0 &&
            g_dock.hoveredIndex < static_cast<int>(g_items.size()) &&
            g_items[g_dock.hoveredIndex].identityKey ==
                g_flyout.appIdentityKey) {
            return;
        }
        // Still inside the flyout or traversing the gap bridge toward it?
        if (IsCursorInFlyoutOrBridge(pt)) {
            if (g_dock.hwnd) {
                SetTimer(g_dock.hwnd, kTimerFlyoutLeave, 180, nullptr);
            }
            return;
        }
    }
    CloseWindowPreviewFlyout();
}

void RefreshWindowPreviewFlyoutIfOpen() {
    if (!g_flyout.open || g_flyout.appIdentityKey.empty()) {
        return;
    }
    for (const auto& it : g_items) {
        if (it.identityKey == g_flyout.appIdentityKey) {
            if (it.windows.empty()) {
                CloseWindowPreviewFlyout();
                return;
            }
            bool same = (it.windows.size() == g_flyout.cards.size());
            if (same) {
                for (size_t i = 0; i < it.windows.size(); ++i) {
                    if (it.windows[i] != g_flyout.cards[i].targetHwnd) {
                        same = false;
                        break;
                    }
                }
            }
            if (!same) {
                OpenWindowPreviewFlyoutForItem(it);
            }
            return;
        }
    }
    CloseWindowPreviewFlyout();
}

// Fast-path DXGI background-only glass refresh (zero layout/text/menu churn!)
void RefreshOpticalGlassPlatesOnly() {
    SetGlassMaintenanceTimer(WantOpticalLiquidGlass() &&
                             !g_liquidCapture.frozen &&
                             (!g_liquidCapture.worker ||
                              !g_dock.liquidPlate.active));
    if (g_dock.animating || g_dock.layoutDirty) {
        return;
    }
    if (!g_dock.liquidPlate.active && g_dock.hideProgress < 0.98f) {
        g_dock.layoutDirty = true;
        RequestAnimationFrame();
        return;
    }
    ULONGLONG nowTick = GetTickCount64();
    ULONGLONG minInterval = (g_liquidCapture.highRate.load(std::memory_order_relaxed) || HasOpenOverlayUi()) ? 16u : 50u;
    if (nowTick - g_dock.liquidPlate.lastDrawTick < minInterval) {
        return;  // Cap background-video glass refresh cleanly
    }
    if (g_dock.hasGlassParams && g_dock.liquidPlate.active &&
        g_dock.hideProgress < 0.98f) {
        RenderLiquidGlassPlate(g_dock.liquidPlate, g_dock.lastGlassParams);
    }
    if (g_flyout.open && g_flyout.hasGlassParams &&
        g_flyout.liquidPlate.active) {
        RenderLiquidGlassPlate(g_flyout.liquidPlate, g_flyout.lastGlassParams);
    }
}

// ---- M9. Live Divider Drag-Resizing (macOS 26 Tahoe) ----

struct DividerResizeState {
    bool active = false;
    POINT startScreen{};
    int startIconSize = 48;
};

DividerResizeState g_divResize;

void EndDividerResize(bool commit) {
    if (!g_divResize.active) {
        return;
    }
    g_divResize.active = false;
    if (GetCapture() == g_dock.hwnd) {
        ReleaseCapture();
    }
    g_dock.modalActive = false;
    if (commit) {
        Wh_SetIntValue(L"ovr_icon_size", g_settings.iconSize);
    } else {
        g_settings.iconSize = g_divResize.startIconSize;
    }
    ClearIconCache();
    RebuildShadowSurface();
    ResizeDockWindowForMonitor();
    UpdateAppBarReservation();
    SyncItemIconsAndTheme();
    RequestAnimationFrame();
}

// ---- M11. Liquid Glass "Desktop & Dock" Live Settings Control Panel ----

struct TahoeSettingsState {
    HWND hwnd = nullptr;
    bool open = false;
    float widthPx = 0.0f;
    float heightPx = 0.0f;
    int draggingSlider = -1;  // 0=size, 1=mag, 2=blur, 3=tint
    int hoverId = -1;

    D2D1_RECT_F rcClose{};
    D2D1_RECT_F rcStyleCards[4]{};
    D2D1_RECT_F rcSliders[4]{};
    D2D1_RECT_F rcPosSeg[3]{};
    D2D1_RECT_F rcToggles[5]{};
    D2D1_RECT_F rcResetBtn{};
    D2D1_RECT_F rcAdvancedBtn{};

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

TahoeSettingsState g_settingsUi;
constexpr WCHAR kTahoeSettingsClassName[] = L"WindhawkMacOS27SettingsPanelWnd";

void CloseTahoeSettingsPanel() {
    if (!g_settingsUi.open && !g_settingsUi.hwnd) {
        return;
    }
    g_settingsUi.open = false;
    g_settingsUi.draggingSlider = -1;
    g_settingsUi.liquidPlate.Shutdown();
    g_settingsUi.uiVisual = nullptr;
    g_settingsUi.uiBrush = nullptr;
    g_settingsUi.uiSurface = nullptr;
    g_settingsUi.fallbackTint = nullptr;
    g_settingsUi.fallbackBackdrop = nullptr;
    g_settingsUi.clipGeom = nullptr;
    g_settingsUi.glassContainer = nullptr;
    g_settingsUi.root = nullptr;
    g_settingsUi.target = nullptr;
    if (g_settingsUi.hwnd) {
        ShowWindow(g_settingsUi.hwnd, SW_HIDE);
        DestroyWindow(g_settingsUi.hwnd);
        g_settingsUi.hwnd = nullptr;
    }
    g_dock.modalActive = false;
    RequestAnimationFrame();
}

void RenderTahoeSettingsPanel() {
    if (!g_settingsUi.open || !g_settingsUi.uiSurface || !g_gfx.dwriteFactory) {
        return;
    }
    const float s = g_dock.scale;
    const float W = g_settingsUi.widthPx;
    const float H = g_settingsUi.heightPx;
    const bool dark = g_dock.darkMode;
    auto acc = GetAccentColor();
    D2D1_COLOR_F accCol =
        D2D1::ColorF(acc.R / 255.0f, acc.G / 255.0f, acc.B / 255.0f, 0.94f);
    D2D1_COLOR_F fg = dark ? D2D1::ColorF(0.96f, 0.98f, 1.0f, 0.95f)
                           : D2D1::ColorF(0.08f, 0.09f, 0.12f, 0.94f);
    D2D1_COLOR_F fgSub = dark ? D2D1::ColorF(0.84f, 0.90f, 0.98f, 0.58f)
                              : D2D1::ColorF(0.10f, 0.12f, 0.16f, 0.56f);
    D2D1_COLOR_F cardBg = dark ? D2D1::ColorF(1, 1, 1, 0.07f)
                               : D2D1::ColorF(1, 1, 1, 0.42f);

    DrawCompositionSurface(g_settingsUi.uiSurface, [&](ID2D1DeviceContext* dc) {
        winrt::com_ptr<ID2D1SolidColorBrush> br;
        dc->CreateSolidColorBrush(fg, br.put());
        if (!br) return;

        winrt::com_ptr<IDWriteTextFormat> fmtTitle, fmtLbl, fmtVal, fmtCenter;
        const auto& fam = ResolveFontFamily();
        g_gfx.dwriteFactory->CreateTextFormat(
            fam.c_str(), nullptr, DWRITE_FONT_WEIGHT_SEMI_BOLD,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 14.0f * s,
            L"en-us", fmtTitle.put());
        g_gfx.dwriteFactory->CreateTextFormat(
            fam.c_str(), nullptr, DWRITE_FONT_WEIGHT_MEDIUM,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 12.0f * s,
            L"en-us", fmtLbl.put());
        g_gfx.dwriteFactory->CreateTextFormat(
            fam.c_str(), nullptr, DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 11.2f * s,
            L"en-us", fmtVal.put());
        g_gfx.dwriteFactory->CreateTextFormat(
            fam.c_str(), nullptr, DWRITE_FONT_WEIGHT_MEDIUM,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 11.0f * s,
            L"en-us", fmtCenter.put());

        if (fmtTitle) {
            fmtTitle->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            fmtTitle->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (fmtLbl) {
            fmtLbl->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (fmtVal) {
            fmtVal->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
            fmtVal->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
        if (fmtCenter) {
            fmtCenter->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            fmtCenter->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }

        const float pad = 16.0f * s;
        float y = 12.0f * s;

        // Traffic light close button + Title
        float dotR = 6.5f * s;
        g_settingsUi.rcClose = {pad, y + 3 * s, pad + dotR * 2.4f,
                                y + 3 * s + dotR * 2.4f};
        float clCx = (g_settingsUi.rcClose.left + g_settingsUi.rcClose.right) * 0.5f;
        float clCy = (g_settingsUi.rcClose.top + g_settingsUi.rcClose.bottom) * 0.5f;
        br->SetColor(D2D1::ColorF(0xFF5F57,
                                  (g_settingsUi.hoverId == 0) ? 1.0f : 0.92f));
        dc->FillEllipse(D2D1::Ellipse({clCx, clCy}, dotR, dotR), br.get());
        if (g_settingsUi.hoverId == 0) {
            br->SetColor(D2D1::ColorF(0.28f, 0.03f, 0.02f, 0.85f));
            float xr = dotR * 0.38f;
            dc->DrawLine({clCx - xr, clCy - xr}, {clCx + xr, clCy + xr}, br.get(),
                         1.3f * s);
            dc->DrawLine({clCx + xr, clCy - xr}, {clCx - xr, clCy + xr}, br.get(),
                         1.3f * s);
        }
        br->SetColor(fg);
        PCWSTR ttl = L"Desktop & Dock  \u2022  macOS 26 Tahoe";
        dc->DrawText(ttl, static_cast<UINT32>(wcslen(ttl)), fmtTitle.get(),
                     D2D1::RectF(pad + 26 * s, y, W - pad - 26 * s, y + 24 * s),
                     br.get());
        y += 32.0f * s;

        // 1. Visual 4-Card Tahoe Icon Style Selector
        PCWSTR stNames[4] = {L"Clear", L"Dark", L"Default", L"Tinted"};
        float cardGap = 8.0f * s;
        float cardW = (W - pad * 2.0f - cardGap * 3.0f) / 4.0f;
        float cardH = 62.0f * s;
        for (int i = 0; i < 4; ++i) {
            float cx = pad + i * (cardW + cardGap);
            D2D1_RECT_F cr{cx, y, cx + cardW, y + cardH};
            g_settingsUi.rcStyleCards[i] = cr;
            bool sel = (static_cast<int>(g_settings.iconStyle) == i);
            bool hov = (g_settingsUi.hoverId == 10 + i);
            br->SetColor(hov ? D2D1::ColorF(1, 1, 1, dark ? 0.13f : 0.62f)
                             : cardBg);
            dc->FillRoundedRectangle(D2D1::RoundedRect(cr, 12 * s, 12 * s),
                                     br.get());

            // Mini preview squircle inside card
            float pw = 24.0f * s;
            float px = cx + (cardW - pw) * 0.5f;
            float py = y + 9.0f * s;
            D2D1_RECT_F pRc{px, py, px + pw, py + pw};
            if (i == 0) {
                br->SetColor(dark ? D2D1::ColorF(0.22f, 0.25f, 0.30f, 0.72f)
                                  : D2D1::ColorF(0.88f, 0.92f, 0.97f, 0.85f));
            } else if (i == 1) {
                br->SetColor(D2D1::ColorF(0.11f, 0.12f, 0.16f, 0.96f));
            } else if (i == 2) {
                br->SetColor(D2D1::ColorF(0.20f, 0.55f, 0.96f, 0.96f));
            } else {
                br->SetColor(D2D1::ColorF(accCol.r, accCol.g, accCol.b, 0.72f));
            }
            dc->FillRoundedRectangle(D2D1::RoundedRect(pRc, 6 * s, 6 * s),
                                     br.get());
            // Mini glyph inside preview squircle
            br->SetColor(i == 0 ? D2D1::ColorF(0.92f, 0.96f, 1.0f, 0.90f)
                                : (i == 1 ? D2D1::ColorF(0.32f, 0.78f, 1.0f, 0.96f)
                                          : D2D1::ColorF(1, 1, 1, 0.95f)));
            dc->FillEllipse(
                D2D1::Ellipse({px + pw * 0.5f, py + pw * 0.5f}, pw * 0.22f,
                              pw * 0.22f),
                br.get());
            br->SetColor(D2D1::ColorF(1, 1, 1, 0.45f));
            dc->DrawRoundedRectangle(D2D1::RoundedRect(pRc, 6 * s, 6 * s),
                                     br.get(), 0.9f * s);

            br->SetColor(sel ? fg : fgSub);
            dc->DrawText(stNames[i], static_cast<UINT32>(wcslen(stNames[i])),
                         fmtCenter.get(),
                         D2D1::RectF(cx, py + pw + 3 * s, cx + cardW, y + cardH - 3 * s),
                         br.get());

            if (sel) {
                br->SetColor(accCol);
                dc->DrawRoundedRectangle(D2D1::RoundedRect(cr, 12 * s, 12 * s),
                                         br.get(), 1.8f * s);
            }
        }
        y += cardH + 12.0f * s;

        // 2. Four Interactive Sliders (Size, Magnification, Glass Blur, Glass Tint)
        struct SliderSpec {
            PCWSTR label;
            float frac;
            std::wstring valTxt;
        };
        WCHAR magScaleTxt[16]{};
        swprintf_s(magScaleTxt, L"%.2fx",
                   std::clamp(g_settings.magnificationScale, 100, 300) /
                       100.0f);
        SliderSpec sliders[4] = {
            {L"Dock Size",
             Clamp01((g_settings.iconSize - 28.0f) / (96.0f - 28.0f)),
             std::to_wstring(g_settings.iconSize) + L" px"},
            {L"Magnification Scale",
             Clamp01((std::clamp(g_settings.magnificationScale, 100, 300) -
                      100.0f) /
                     (300.0f - 100.0f)),
             magScaleTxt},
            {L"Liquid Glass Blur", Clamp01(g_settings.glassBlur / 100.0f),
             std::to_wstring(g_settings.glassBlur) + L"%"},
            {L"Liquid Glass Tint", Clamp01(g_settings.glassTint / 100.0f),
             std::to_wstring(g_settings.glassTint) + L"%"},
        };

        float sCardH = 148.0f * s;
        br->SetColor(cardBg);
        dc->FillRoundedRectangle(
            D2D1::RoundedRect(D2D1::RectF(pad, y, W - pad, y + sCardH), 12 * s,
                              12 * s),
            br.get());
        float sy = y + 8.0f * s;
        for (int i = 0; i < 4; ++i) {
            br->SetColor(fg);
            dc->DrawText(sliders[i].label,
                         static_cast<UINT32>(wcslen(sliders[i].label)),
                         fmtLbl.get(),
                         D2D1::RectF(pad + 12 * s, sy, pad + 160 * s, sy + 26 * s),
                         br.get());
            br->SetColor(fgSub);
            dc->DrawText(sliders[i].valTxt.c_str(),
                         static_cast<UINT32>(sliders[i].valTxt.size()),
                         fmtVal.get(),
                         D2D1::RectF(W - pad - 56 * s, sy, W - pad - 12 * s,
                                     sy + 26 * s),
                         br.get());

            D2D1_RECT_F tr{pad + 164 * s, sy + 10 * s, W - pad - 64 * s,
                           sy + 16 * s};
            g_settingsUi.rcSliders[i] = {tr.left - 8 * s, sy, tr.right + 8 * s,
                                         sy + 26 * s};
            br->SetColor(dark ? D2D1::ColorF(1, 1, 1, 0.16f)
                              : D2D1::ColorF(0, 0, 0, 0.14f));
            dc->FillRoundedRectangle(D2D1::RoundedRect(tr, 3 * s, 3 * s),
                                     br.get());
            float kx = tr.left + (tr.right - tr.left) * sliders[i].frac;
            br->SetColor(accCol);
            dc->FillRoundedRectangle(
                D2D1::RoundedRect(D2D1::RectF(tr.left, tr.top, kx, tr.bottom),
                                  3 * s, 3 * s),
                br.get());
            br->SetColor(D2D1::ColorF(1, 1, 1, 0.98f));
            dc->FillEllipse(
                D2D1::Ellipse({kx, (tr.top + tr.bottom) * 0.5f}, 7.0f * s,
                              7.0f * s),
                br.get());
            sy += 34.0f * s;
        }
        y += sCardH + 10.0f * s;

        // 3. Segmented Pill Selector (Position)
        auto drawSegRow = [&](PCWSTR lbl, const PCWSTR* opts, int count,
                              int activeIdx, D2D1_RECT_F* outRects) {
            br->SetColor(fg);
            dc->DrawText(lbl, static_cast<UINT32>(wcslen(lbl)), fmtLbl.get(),
                         D2D1::RectF(pad + 12 * s, y, pad + 170 * s, y + 28 * s),
                         br.get());
            float segL = pad + 174 * s;
            float segR = W - pad - 10 * s;
            float segW = segR - segL;
            float pw = segW / static_cast<float>(count);
            br->SetColor(dark ? D2D1::ColorF(1, 1, 1, 0.08f)
                              : D2D1::ColorF(0, 0, 0, 0.07f));
            dc->FillRoundedRectangle(
                D2D1::RoundedRect(D2D1::RectF(segL, y + 2 * s, segR, y + 26 * s),
                                  12 * s, 12 * s),
                br.get());
            for (int k = 0; k < count; ++k) {
                D2D1_RECT_F pr{segL + k * pw + 2 * s, y + 4 * s,
                               segL + (k + 1) * pw - 2 * s, y + 24 * s};
                outRects[k] = pr;
                if (k == activeIdx) {
                    br->SetColor(accCol);
                    dc->FillRoundedRectangle(
                        D2D1::RoundedRect(pr, 10 * s, 10 * s), br.get());
                } else if (g_settingsUi.hoverId == 20 + k) {
                    br->SetColor(dark ? D2D1::ColorF(1, 1, 1, 0.08f)
                                      : D2D1::ColorF(0, 0, 0, 0.07f));
                    dc->FillRoundedRectangle(
                        D2D1::RoundedRect(pr, 10 * s, 10 * s), br.get());
                }
                br->SetColor(k == activeIdx ? D2D1::ColorF(1, 1, 1, 0.98f) : fgSub);
                dc->DrawText(opts[k], static_cast<UINT32>(wcslen(opts[k])),
                             fmtCenter.get(), pr, br.get());
            }
            y += 32.0f * s;
        };

        float segCardY = y;
        br->SetColor(cardBg);
        dc->FillRoundedRectangle(
            D2D1::RoundedRect(D2D1::RectF(pad, segCardY, W - pad,
                                          segCardY + 44.0f * s),
                              12 * s, 12 * s),
            br.get());
        y += 6.0f * s;
        PCWSTR posOpts[3] = {L"Left", L"Bottom", L"Right"};
        int posCur = (g_settings.position == DockPosition::Left)
                         ? 0
                         : (g_settings.position == DockPosition::Right ? 2 : 1);
        drawSegRow(L"Position on screen", posOpts, 3, posCur,
                   g_settingsUi.rcPosSeg);
        y = segCardY + 54.0f * s;

        // 4. Five macOS Toggle Switches
        struct ToggleSpec {
            PCWSTR label;
            bool on;
        };
        ToggleSpec toggles[5] = {
            {L"Hover magnification wave", g_settings.magnification},
            {L"Automatically hide and show the Dock", g_settings.autoHide},
            {L"Minimize windows into application icon",
             g_settings.minimizeIntoAppIcon},
            {L"Live window preview flyouts on hover",
             g_settings.showWindowPreviews},
            {L"Show suggested & recent apps in Dock", g_settings.showRecents},
        };
        float togCardH = 152.0f * s;
        br->SetColor(cardBg);
        dc->FillRoundedRectangle(
            D2D1::RoundedRect(D2D1::RectF(pad, y, W - pad, y + togCardH), 12 * s,
                              12 * s),
            br.get());
        float ty = y + 6.0f * s;
        for (int i = 0; i < 5; ++i) {
            D2D1_RECT_F rowRc{pad + 8 * s, ty, W - pad - 8 * s, ty + 28 * s};
            g_settingsUi.rcToggles[i] = rowRc;
            if (g_settingsUi.hoverId == 30 + i) {
                br->SetColor(dark ? D2D1::ColorF(1, 1, 1, 0.06f)
                                  : D2D1::ColorF(1, 1, 1, 0.28f));
                dc->FillRoundedRectangle(
                    D2D1::RoundedRect(rowRc, 7 * s, 7 * s), br.get());
            }
            br->SetColor(fg);
            dc->DrawText(toggles[i].label,
                         static_cast<UINT32>(wcslen(toggles[i].label)),
                         fmtLbl.get(),
                         D2D1::RectF(pad + 12 * s, ty, W - pad - 56 * s,
                                     ty + 28 * s),
                         br.get());
            float swW = 36.0f * s;
            float swH = 20.0f * s;
            D2D1_RECT_F swRc{W - pad - 12 * s - swW, ty + 4 * s,
                             W - pad - 12 * s, ty + 4 * s + swH};
            br->SetColor(toggles[i].on
                             ? accCol
                             : (dark ? D2D1::ColorF(1, 1, 1, 0.18f)
                                     : D2D1::ColorF(0, 0, 0, 0.16f)));
            dc->FillRoundedRectangle(
                D2D1::RoundedRect(swRc, swH * 0.5f, swH * 0.5f), br.get());
            float kr = swH * 0.40f;
            float kx = toggles[i].on ? (swRc.right - swH * 0.5f)
                                     : (swRc.left + swH * 0.5f);
            br->SetColor(D2D1::ColorF(1, 1, 1, 0.98f));
            dc->FillEllipse(D2D1::Ellipse({kx, (swRc.top + swRc.bottom) * 0.5f},
                                          kr, kr),
                            br.get());
            ty += 28.5f * s;
        }
        y += togCardH + 10.0f * s;

        // 5. Footer action pills
        float btnH = 28.0f * s;
        float halfW = (W - pad * 2.0f - 10 * s) * 0.5f;
        g_settingsUi.rcResetBtn = {pad, y, pad + halfW, y + btnH};
        g_settingsUi.rcAdvancedBtn = {pad + halfW + 10 * s, y, W - pad,
                                      y + btnH};
        br->SetColor(g_settingsUi.hoverId == 40
                         ? D2D1::ColorF(1, 1, 1, dark ? 0.17f : 0.62f)
                         : D2D1::ColorF(dark ? 1.0f : 0.0f,
                                        dark ? 1.0f : 0.0f,
                                        dark ? 1.0f : 0.0f,
                                        dark ? 0.10f : 0.08f));
        dc->FillRoundedRectangle(
            D2D1::RoundedRect(g_settingsUi.rcResetBtn, 10 * s, 10 * s),
            br.get());
        br->SetColor(g_settingsUi.hoverId == 41
                         ? D2D1::ColorF(1, 1, 1, dark ? 0.17f : 0.62f)
                         : D2D1::ColorF(dark ? 1.0f : 0.0f,
                                        dark ? 1.0f : 0.0f,
                                        dark ? 1.0f : 0.0f,
                                        dark ? 0.10f : 0.08f));
        dc->FillRoundedRectangle(
            D2D1::RoundedRect(g_settingsUi.rcAdvancedBtn, 10 * s, 10 * s),
            br.get());
        br->SetColor(fg);
        PCWSTR rTxt = L"Reset Live Tweaks";
        PCWSTR aTxt = L"Windhawk Settings\u2026";
        dc->DrawText(rTxt, static_cast<UINT32>(wcslen(rTxt)), fmtCenter.get(),
                     g_settingsUi.rcResetBtn, br.get());
        dc->DrawText(aTxt, static_cast<UINT32>(wcslen(aTxt)), fmtCenter.get(),
                     g_settingsUi.rcAdvancedBtn, br.get());
        (void)H;
    });
}

void ApplyTahoeSettingsSliderAtX(int sliderIdx, float lx, bool commitHeavy = true) {
    if (sliderIdx < 0 || sliderIdx > 3) {
        return;
    }
    const auto& r = g_settingsUi.rcSliders[sliderIdx];
    float pad = 8.0f * g_dock.scale;
    float frac = Clamp01((lx - (r.left + pad)) /
                         std::max(1.0f, (r.right - r.left) - 2.0f * pad));
    if (sliderIdx == 0) {
        int val = static_cast<int>(std::round(28.0f + frac * (96.0f - 28.0f)));
        if (val != g_settings.iconSize) {
            Wh_SetIntValue(L"ovr_icon_size", val);
            g_settings.iconSize = val;
            if (commitHeavy) {
                ApplyLiveSettingsAfterOverride();
            } else {
                LayoutAndApplyDock(NowSeconds());
                RequestAnimationFrame();
            }
        } else if (commitHeavy) {
            ApplyLiveSettingsAfterOverride();
        }
    } else if (sliderIdx == 1) {
        int val =
            static_cast<int>(std::round(100.0f + frac * (300.0f - 100.0f)));
        if (val != g_settings.magnificationScale) {
            Wh_SetIntValue(L"ovr_mag_scale", val);
            g_settings.magnificationScale = val;
            if (commitHeavy) {
                ApplyLiveSettingsAfterOverride();
            } else {
                LayoutAndApplyDock(NowSeconds());
                RequestAnimationFrame();
            }
        } else if (commitHeavy) {
            ApplyLiveSettingsAfterOverride();
        }
    } else if (sliderIdx == 2) {
        int val = static_cast<int>(std::round(frac * 100.0f));
        if (val != g_settings.glassBlur) {
            Wh_SetIntValue(L"ovr_glass_blur", val);
            g_settings.glassBlur = val;
            if (commitHeavy) {
                ApplyLiveSettingsAfterOverride();
            } else {
                UpdateGlassAppearance();
                g_dock.liquidPlate.dirty = true;
                g_settingsUi.liquidPlate.dirty = true;
                RefreshOpticalGlassPlatesOnly();
            }
        } else if (commitHeavy) {
            ApplyLiveSettingsAfterOverride();
        }
    } else if (sliderIdx == 3) {
        int val = static_cast<int>(std::round(frac * 100.0f));
        if (val != g_settings.glassTint) {
            Wh_SetIntValue(L"ovr_glass_tint", val);
            g_settings.glassTint = val;
            if (commitHeavy) {
                ApplyLiveSettingsAfterOverride();
            } else {
                UpdateGlassAppearance();
                g_dock.liquidPlate.dirty = true;
                g_settingsUi.liquidPlate.dirty = true;
                RefreshOpticalGlassPlatesOnly();
            }
        } else if (commitHeavy) {
            ApplyLiveSettingsAfterOverride();
        }
    }
    RenderTahoeSettingsPanel();
}

int HitTestTahoeSettingsHoverId(float lx, float ly) {
    if (PointInRectF(g_settingsUi.rcClose, lx, ly)) return 0;
    for (int i = 0; i < 4; ++i) {
        if (PointInRectF(g_settingsUi.rcStyleCards[i], lx, ly)) return 10 + i;
    }
    for (int k = 0; k < 3; ++k) {
        if (PointInRectF(g_settingsUi.rcPosSeg[k], lx, ly)) return 20 + k;
    }
    for (int t = 0; t < 5; ++t) {
        if (PointInRectF(g_settingsUi.rcToggles[t], lx, ly)) return 30 + t;
    }
    if (PointInRectF(g_settingsUi.rcResetBtn, lx, ly)) return 40;
    if (PointInRectF(g_settingsUi.rcAdvancedBtn, lx, ly)) return 41;
    return -1;
}

LRESULT CALLBACK TahoeSettingsWndProc(HWND hwnd, UINT msg, WPARAM wp,
                                      LPARAM lp) {
    try {
        switch (msg) {
            case WM_ACTIVATE:
                if (LOWORD(wp) == WA_INACTIVE &&
                    g_settingsUi.draggingSlider < 0) {
                    CloseTahoeSettingsPanel();
                }
                return 0;
            case WM_KEYDOWN:
                if (wp == VK_ESCAPE) {
                    CloseTahoeSettingsPanel();
                }
                return 0;
            case WM_LBUTTONDOWN: {
                float lx = static_cast<float>(static_cast<short>(LOWORD(lp)));
                float ly = static_cast<float>(static_cast<short>(HIWORD(lp)));
                for (int i = 0; i < 4; ++i) {
                    if (PointInRectF(g_settingsUi.rcSliders[i], lx, ly)) {
                        g_settingsUi.draggingSlider = i;
                        SetCapture(hwnd);
                        ApplyTahoeSettingsSliderAtX(i, lx, false);
                        return 0;
                    }
                }
                return 0;
            }
            case WM_MOUSEMOVE: {
                float lx = static_cast<float>(static_cast<short>(LOWORD(lp)));
                if (g_settingsUi.draggingSlider >= 0 && (wp & MK_LBUTTON)) {
                    ApplyTahoeSettingsSliderAtX(g_settingsUi.draggingSlider, lx,
                                                false);
                    return 0;
                }
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
                float ly = static_cast<float>(static_cast<short>(HIWORD(lp)));
                int newHover = HitTestTahoeSettingsHoverId(lx, ly);
                if (newHover != g_settingsUi.hoverId) {
                    g_settingsUi.hoverId = newHover;
                    RenderTahoeSettingsPanel();
                }
                return 0;
            }
            case WM_MOUSELEAVE:
                if (g_settingsUi.hoverId != -1) {
                    g_settingsUi.hoverId = -1;
                    RenderTahoeSettingsPanel();
                }
                return 0;
            case WM_LBUTTONUP: {
                if (g_settingsUi.draggingSlider >= 0) {
                    int idx = g_settingsUi.draggingSlider;
                    g_settingsUi.draggingSlider = -1;
                    ReleaseCapture();
                    float lx =
                        static_cast<float>(static_cast<short>(LOWORD(lp)));
                    ApplyTahoeSettingsSliderAtX(idx, lx, true);
                    g_settingsUi.liquidPlate.dirty = true;
                    RefreshOpticalGlassPlatesOnly();
                    return 0;
                }
                float lx = static_cast<float>(static_cast<short>(LOWORD(lp)));
                float ly = static_cast<float>(static_cast<short>(HIWORD(lp)));
                if (PointInRectF(g_settingsUi.rcClose, lx, ly)) {
                    CloseTahoeSettingsPanel();
                    return 0;
                }
                for (int i = 0; i < 4; ++i) {
                    if (PointInRectF(g_settingsUi.rcStyleCards[i], lx, ly)) {
                        Wh_SetIntValue(L"ovr_icon_style", i);
                        ApplyLiveSettingsAfterOverride();
                        RenderTahoeSettingsPanel();
                        return 0;
                    }
                }
                for (int k = 0; k < 3; ++k) {
                    if (PointInRectF(g_settingsUi.rcPosSeg[k], lx, ly)) {
                        int posVal = (k == 0) ? 1 : (k == 2 ? 2 : 0);
                        Wh_SetIntValue(L"ovr_position", posVal);
                        ApplyLiveSettingsAfterOverride();
                        RenderTahoeSettingsPanel();
                        return 0;
                    }
                }
                for (int t = 0; t < 5; ++t) {
                    if (PointInRectF(g_settingsUi.rcToggles[t], lx, ly)) {
                        if (t == 0)
                            Wh_SetIntValue(L"ovr_magnify",
                                           g_settings.magnification ? 0 : 1);
                        else if (t == 1)
                            Wh_SetIntValue(L"ovr_autohide",
                                           g_settings.autoHide ? 0 : 1);
                        else if (t == 2)
                            Wh_SetIntValue(
                                L"ovr_min_into_icon",
                                g_settings.minimizeIntoAppIcon ? 0 : 1);
                        else if (t == 3)
                            Wh_SetIntValue(
                                L"ovr_previews",
                                g_settings.showWindowPreviews ? 0 : 1);
                        else if (t == 4)
                            Wh_SetIntValue(L"ovr_show_recents",
                                           g_settings.showRecents ? 0 : 1);
                        ApplyLiveSettingsAfterOverride();
                        RenderTahoeSettingsPanel();
                        return 0;
                    }
                }
                if (PointInRectF(g_settingsUi.rcResetBtn, lx, ly)) {
                    ClearRuntimeOverrides();
                    SaveNumberedStringList(L"dock_pin_order_v2", {});
                    Wh_DeleteValue(L"dock_pin_order_v1");
                    ApplyLiveSettingsAfterOverride();
                    g_settingsUi.liquidPlate.dirty = true;
                    RefreshOpticalGlassPlatesOnly();
                    RenderTahoeSettingsPanel();
                    return 0;
                }
                if (PointInRectF(g_settingsUi.rcAdvancedBtn, lx, ly)) {
                    CloseTahoeSettingsPanel();
                    OpenWindhawkUi();
                    return 0;
                }
                return 0;
            }
        }
    } catch (...) {
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

void OpenTahoeSettingsPanel() {
    if (g_settingsUi.open) {
        CloseTahoeSettingsPanel();
        return;
    }
    CloseAllMenus();
    CloseDownloadsFan();
    if (!g_dock.hwnd) {
        return;
    }

    const float s = g_dock.scale;
    const RECT& mr = g_dock.monitorRect;
    float w = std::ceil(510.0f * s);
    float h = std::ceil(536.0f * s);
    int px = mr.left + ((mr.right - mr.left) - static_cast<int>(w)) / 2;
    int py = mr.top + ((mr.bottom - mr.top) - static_cast<int>(h)) / 2;
    py = std::max(static_cast<int>(mr.top + 38.0f * s), py);

    HMODULE hSelf = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&OpenTahoeSettingsPanel),
                       &hSelf);
    static bool s_regSet = false;
    if (!s_regSet) {
        WNDCLASSEXW wc{.cbSize = sizeof(wc),
                       .lpfnWndProc = TahoeSettingsWndProc,
                       .hInstance = hSelf,
                       .hCursor = LoadCursorW(nullptr, IDC_ARROW),
                       .lpszClassName = kTahoeSettingsClassName};
        RegisterClassExW(&wc);
        s_regSet = true;
    }

    g_settingsUi.hwnd = CreateWindowExW(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_LAYERED | WS_EX_TOPMOST |
            WS_EX_TOOLWINDOW,
        kTahoeSettingsClassName, L"Desktop & Dock", WS_POPUP, px, py,
        static_cast<int>(w), static_cast<int>(h), nullptr, nullptr, hSelf,
        nullptr);
    if (!g_settingsUi.hwnd) {
        return;
    }
    g_settingsUi.widthPx = w;
    g_settingsUi.heightPx = h;
    g_settingsUi.draggingSlider = -1;
    g_settingsUi.hoverId = -1;
    g_settingsUi.open = true;
    g_dock.modalActive = true;

    InitGlassPopupWindowShell(
        g_settingsUi.hwnd, w, h, 20.0f * s, px, py,
        g_dock.darkMode ? 0.18f : 0.16f, g_settingsUi.target,
        g_settingsUi.root, g_settingsUi.glassContainer, g_settingsUi.clipGeom,
        g_settingsUi.fallbackBackdrop, g_settingsUi.fallbackTint,
        g_settingsUi.liquidPlate, g_settingsUi.uiSurface, g_settingsUi.uiBrush,
        g_settingsUi.uiVisual);
    RenderTahoeSettingsPanel();
    ShowWindow(g_settingsUi.hwnd, SW_SHOW);
    SetForegroundWindow(g_settingsUi.hwnd);
}

void ApplyOverlayCaptureExclusion(DWORD affinity) {
    for (HWND h : {g_dlFan.hwnd, g_flyout.bgHwnd, g_flyout.thumbHwnd,
                   g_settingsUi.hwnd, g_drag.ghostHwnd}) {
        if (h && IsWindow(h)) {
            SetWindowDisplayAffinity(h, affinity);
        }
    }
}

bool HasOpenOverlayUi() {
    return g_flyout.open || g_menus.mainPopup.open || g_dlFan.open ||
           g_settingsUi.open;
}

void AddAllOverlayWatchedRegions(std::vector<RECT>& watchList) {
    AddFlyoutWatchedRegion(watchList);
    auto addHwnd = [&](bool open, HWND h) {
        RECT r{};
        if (open && h && IsWindow(h) && GetWindowRect(h, &r) &&
            r.right > r.left && r.bottom > r.top) {
            InflateRect(&r, 16, 16);
            watchList.push_back(r);
        }
    };
    addHwnd(g_menus.mainPopup.open, g_menus.mainPopup.hwnd);
    addHwnd(g_menus.subPopup.open, g_menus.subPopup.hwnd);
    addHwnd(g_dlFan.open, g_dlFan.hwnd);
    addHwnd(g_settingsUi.open, g_settingsUi.hwnd);

    const auto& tip = g_dock.tooltip;
    if (tip.opacity > 0.01f && tip.widthPx > 1.0f && tip.heightPx > 1.0f &&
        g_dock.hwnd) {
        const RECT& wr = g_dock.windowRect;
        float easedHide = EaseInOutCubic(g_dock.hideProgress);
        float slidePx = easedHide * g_dock.slideExtentPx;
        float offX = (g_settings.position == DockPosition::Left)
                         ? -slidePx
                         : (g_settings.position == DockPosition::Right
                                ? slidePx
                                : 0.0f);
        float offY =
            (g_settings.position == DockPosition::Bottom) ? slidePx : 0.0f;
        float x0 = static_cast<float>(wr.left) + tip.posX + offX;
        float y0 = static_cast<float>(wr.top) + tip.posY + offY;
        RECT tr{
            static_cast<LONG>(std::floor(x0)) - 16,
            static_cast<LONG>(std::floor(y0)) - 16,
            static_cast<LONG>(std::ceil(x0 + tip.widthPx)) + 16,
            static_cast<LONG>(std::ceil(y0 + tip.heightPx)) + 16};
        if (tr.right > tr.left && tr.bottom > tr.top) {
            watchList.push_back(tr);
        }
    }
}

// ============================================================================
// 18. Window Procedure, UI Thread & Tool-Mod Boilerplate
// ============================================================================

LRESULT CALLBACK DockWndProc(HWND hwnd,
                             UINT msg,
                             WPARAM wParam,
                             LPARAM lParam) {
    try {
        if (g_taskbarHider.taskbarCreatedMsg &&
            msg == g_taskbarHider.taskbarCreatedMsg) {
            if (g_settings.hideTaskbar) {
                ApplyTaskbarHiding();
            }
            g_dock.appBarRegistered = false;
            g_dock.appBarFullscreen = false;
            UpdateAppBarReservation();
            if (SyncPinnedAppsWithTaskbar(true)) {
                ReconcileDockItems(false);
            }
            return 0;
        }

        if (g_tracker.shellHookMsg && msg == g_tracker.shellHookMsg) {
            if (wParam == kShellHookFlash) {
                OnWindowAttentionFlash(reinterpret_cast<HWND>(lParam));
            }
            return 0;
        }

        switch (msg) {
            case WM_TIMER:
                if (wParam == kTimerCursorPoll) {
                    PollCursorState();
                    return 0;
                }
                if (wParam == kTimerTaskbarGuard) {
                    if (g_settings.hideTaskbar) {
                        ApplyTaskbarHiding();
                    }
                    CheckFullscreenState();
                    if (SyncPinnedAppsWithTaskbar(false)) {
                        ReconcileDockItems(false);
                        RequestAnimationFrame();
                    }
                    return 0;
                }
                if (wParam == kTimerWindowRefresh) {
                    KillTimer(hwnd, kTimerWindowRefresh);
                    g_tracker.refreshScheduled = false;
                    ReconcileDockItems(false);
                    return 0;
                }
                if (wParam == kTimerFullscreenCheck) {
                    KillTimer(hwnd, kTimerFullscreenCheck);
                    CheckFullscreenState();
                    return 0;
                }
                if (wParam == kTimerGlassMaintenance) {
                    if (WantOpticalLiquidGlass() && !g_liquidCapture.frozen) {
                        uint64_t prevFrame = g_liquidCapture.frameId;
                        if (EnsureLiquidCaptureReady() &&
                            (g_liquidCapture.frameId != prevFrame ||
                             !g_dock.liquidPlate.active)) {
                            RefreshOpticalGlassPlatesOnly();
                        }
                    }
                    return 0;
                }
                if (wParam == kTimerFlyoutHover) {
                    OnFlyoutHoverTimer();
                    return 0;
                }
                if (wParam == kTimerFlyoutLeave) {
                    OnFlyoutLeaveTimer();
                    return 0;
                }
                if (wParam == kTimerTaskbarPinSync) {
                    KillTimer(hwnd, kTimerTaskbarPinSync);
                    if (SyncPinnedAppsWithTaskbar(true)) {
                        ReconcileDockItems(false);
                        RequestAnimationFrame();
                    }
                    return 0;
                }
                break;

            case kWmAppShellNotify:
                InvalidateIconCacheForPrefixes(
                    {L"fixed:trash", L"fixed:downloads", L"folder:"});
                SyncItemIconsAndTheme();
                RequestAnimationFrame();
                return 0;

            case kWmAppTaskbarPinsChanged:
                if (SyncPinnedAppsWithTaskbar(true)) {
                    ReconcileDockItems(false);
                    RequestAnimationFrame();
                }
                return 0;

            case kWmAppGlassFrame:
                g_liquidCapture.framePosted.store(false,
                                                  std::memory_order_release);
                if (EnsureLiquidCaptureReady()) {
                    RefreshOpticalGlassPlatesOnly();
                }
                return 0;

            case kWmAppDeviceLost:
                g_deviceLostPending.store(false, std::memory_order_release);
                OnGraphicsDeviceLost();
                return 0;

            case kWmAppDeviceRedraw:
                g_deviceRedrawPending.store(false, std::memory_order_release);
                ResetAndRedrawGpuSurfaces();
                return 0;

            case WM_HOTKEY:
                if (wParam == kHotkeyFreezeGlass) {
                    g_liquidCapture.frozen = !g_liquidCapture.frozen;
                    if (g_liquidCapture.frozen) {
                        ApplyCaptureExclusionToAllModWindows(false);
                    } else {
                        g_liquidCapture.excludedApplied = false;
                    }
                    g_dock.liquidPlate.dirty = true;
                    RequestAnimationFrame();
                    return 0;
                }
                break;

            case WM_SETCURSOR: {
                if (LOWORD(lParam) == HTCLIENT) {
                    if (g_divResize.active) {
                        SetCursor(LoadCursorW(
                            nullptr,
                            (g_settings.position == DockPosition::Bottom)
                                ? IDC_SIZENS
                                : IDC_SIZEWE));
                        return TRUE;
                    }
                    POINT pt{};
                    if (GetCursorPos(&pt)) {
                        float lx = static_cast<float>(pt.x - g_dock.windowRect.left);
                        float ly = static_cast<float>(pt.y - g_dock.windowRect.top);
                        if (HitTestDividerAtPoint(lx, ly) >= 0) {
                            SetCursor(LoadCursorW(
                                nullptr,
                                (g_settings.position == DockPosition::Bottom)
                                    ? IDC_SIZENS
                                    : IDC_SIZEWE));
                            return TRUE;
                        }
                    }
                }
                break;
            }

            case WM_MOUSEMOVE: {
                POINT cur{};
                GetCursorPos(&cur);
                if (g_divResize.active) {
                    int delta = 0;
                    if (g_settings.position == DockPosition::Bottom) {
                        delta = static_cast<int>(std::round(
                            static_cast<float>(g_divResize.startScreen.y - cur.y) /
                            std::max(0.5f, g_dock.scale)));
                    } else if (g_settings.position == DockPosition::Left) {
                        delta = static_cast<int>(std::round(
                            static_cast<float>(cur.x - g_divResize.startScreen.x) /
                            std::max(0.5f, g_dock.scale)));
                    } else {
                        delta = static_cast<int>(std::round(
                            static_cast<float>(g_divResize.startScreen.x - cur.x) /
                            std::max(0.5f, g_dock.scale)));
                    }
                    int newSz =
                        std::clamp(g_divResize.startIconSize + delta, 28, 96);
                    if (newSz != g_settings.iconSize) {
                        g_settings.iconSize = newSz;
                        LayoutAndApplyDock(NowSeconds());
                    }
                    return 0;
                }
                if (g_drag.active) {
                    UpdateInternalDockDrag(cur);
                    PollCursorState();
                    return 0;
                }
                if (g_drag.candidate && (wParam & MK_LBUTTON)) {
                    float dx = static_cast<float>(cur.x - g_drag.downScreen.x);
                    float dy = static_cast<float>(cur.y - g_drag.downScreen.y);
                    float thresh = 7.0f * g_dock.scale;
                    if (dx * dx + dy * dy >= thresh * thresh) {
                        BeginInternalDockDrag(cur);
                        if (g_drag.active) {
                            UpdateInternalDockDrag(cur);
                        }
                    }
                }
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
                PollCursorState();
                return 0;
            }

            case WM_MOUSELEAVE:
                PollCursorState();
                return 0;

            case WM_LBUTTONDOWN: {
                CloseAllMenus();
                CloseDownloadsFan();
                float lx = static_cast<float>(static_cast<short>(LOWORD(lParam)));
                float ly = static_cast<float>(static_cast<short>(HIWORD(lParam)));
                if (HitTestDividerAtPoint(lx, ly) >= 0) {
                    g_divResize.active = true;
                    GetCursorPos(&g_divResize.startScreen);
                    g_divResize.startIconSize = g_settings.iconSize;
                    g_dock.modalActive = true;
                    SetCapture(hwnd);
                    SetCursor(LoadCursorW(
                        nullptr,
                        (g_settings.position == DockPosition::Bottom)
                            ? IDC_SIZENS
                            : IDC_SIZEWE));
                    return 0;
                }
                int idx = HitTestItemAtPoint(lx, ly);
                g_drag.candidate = false;
                if (idx >= 0 && idx < static_cast<int>(g_items.size())) {
                    g_dock.pressedIndex = idx;
                    g_items[idx].pressed = true;
                    if (g_items[idx].kind == DockItemKind::App &&
                        g_items[idx].pinned) {
                        g_drag.candidate = true;
                        g_drag.identityKey = g_items[idx].identityKey;
                        g_drag.displayName = g_items[idx].name;
                        GetCursorPos(&g_drag.downScreen);
                    }
                    RequestAnimationFrame();
                }
                return 0;
            }

            case WM_CAPTURECHANGED:
                if (g_divResize.active) {
                    EndDividerResize(true);
                }
                if (g_drag.active) {
                    EndInternalDockDrag(false);
                }
                return 0;

            case WM_RBUTTONUP: {
                CloseWindowPreviewFlyout();
                float lx = static_cast<float>(static_cast<short>(LOWORD(lParam)));
                float ly = static_cast<float>(static_cast<short>(HIWORD(lParam)));
                POINT pt{static_cast<LONG>(lx), static_cast<LONG>(ly)};
                ClientToScreen(hwnd, &pt);
                OpenContextMenuForDock(pt, lx, ly);
                return 0;
            }

            case kWmAppDeferredAction: {
                auto fn = std::move(g_menus.pendingAction);
                g_menus.pendingAction = nullptr;
                if (fn) {
                    fn();
                }
                return 0;
            }

            case WM_LBUTTONUP: {
                if (g_divResize.active) {
                    EndDividerResize(true);
                    return 0;
                }
                bool wasDrag = g_drag.active;
                EndInternalDockDrag(true);
                float lx = static_cast<float>(static_cast<short>(LOWORD(lParam)));
                float ly = static_cast<float>(static_cast<short>(HIWORD(lParam)));
                int upIdx = HitTestItemAtPoint(lx, ly);
                int downIdx = g_dock.pressedIndex;
                g_dock.pressedIndex = -1;
                if (downIdx >= 0 && downIdx < static_cast<int>(g_items.size())) {
                    g_items[downIdx].pressed = false;
                    RequestAnimationFrame();
                }
                if (!wasDrag && downIdx >= 0 && downIdx == upIdx) {
                    HandleItemPrimaryClick(upIdx);
                }
                return 0;
            }

            case WM_MBUTTONUP: {
                float lx = static_cast<float>(static_cast<short>(LOWORD(lParam)));
                float ly = static_cast<float>(static_cast<short>(HIWORD(lParam)));
                int idx = HitTestItemAtPoint(lx, ly);
                if (idx >= 0 && idx < static_cast<int>(g_items.size())) {
                    auto& item = g_items[idx];
                    if (item.kind == DockItemKind::App ||
                        item.kind == DockItemKind::Recent ||
                        item.kind == DockItemKind::Finder) {
                        LaunchAppItem(item);
                    }
                }
                return 0;
            }

            case WM_SETTINGCHANGE:
                if (lParam && _wcsicmp(reinterpret_cast<PCWSTR>(lParam),
                                       L"ImmersiveColorSet") == 0) {
                    bool oldDark = g_dock.darkMode;
                    UpdateGlassAppearance();
                    if (oldDark != g_dock.darkMode) {
                        ClearIconCache();
                        RebuildShadowSurface();
                        SyncItemIconsAndTheme();
                        RequestAnimationFrame();
                    }
                } else if (wParam == SPI_SETWORKAREA) {
                    ResizeDockWindowForMonitor();
                    RequestAnimationFrame();
                }
                return 0;

            case WM_DWMCOLORIZATIONCOLORCHANGED:
                UpdateGlassAppearance();
                RequestAnimationFrame();
                return 0;

            case WM_DISPLAYCHANGE:
            case WM_DPICHANGED:
                CloseWindowPreviewFlyout();
                CloseTahoeSettingsPanel();
                ShutdownLiquidCapture();
                g_dock.liquidPlate.ResetGpuResources();
                g_dock.tooltip.liquidPlate.ResetGpuResources();
                ResetFlyoutGpuResources();
                g_dock.scale = GetMonitorDpiScale(GetPrimaryMonitor());
                ClearIconCache();
                g_dock.tooltip.currentText.clear();
                RebuildShadowSurface();
                ResizeDockWindowForMonitor();
                UpdateAppBarReservation();
                SyncItemIconsAndTheme();
                RequestAnimationFrame();
                return 0;

            case kWmAppSettings: {
                CloseWindowPreviewFlyout();
                bool prevHide = g_settings.hideTaskbar;
                LoadSettings(true);
                g_liquidCapture.consecutiveFailures = 0;
                g_liquidCapture.unsupported = false;
                if (prevHide && !g_settings.hideTaskbar) {
                    RestoreTaskbar();
                } else if (g_settings.hideTaskbar) {
                    ApplyTaskbarHiding();
                }
                g_dock.autoHideRevealed = !g_settings.autoHide;
                g_dock.cursorLeftTime = 0.0;
                UpdateGlassAppearance();
                ClearIconCache();
                g_dock.tooltip.currentText.clear();
                RebuildShadowSurface();
                ResizeDockWindowForMonitor();
                UpdateAppBarReservation();
                SyncPinnedAppsWithTaskbar(true);
                ReconcileDockItems(false);
                RequestAnimationFrame();
                return 0;
            }

            case kWmAppQuit:
                g_quitting = true;
                RestoreTaskbar();
                RemoveAppBar();
                EndDividerResize(false);
                EndInternalDockDrag(false);
                CloseWindowPreviewFlyout();
                CloseTahoeSettingsPanel();
                CloseDownloadsFan();
                CloseAllMenus();
                DestroyWindow(hwnd);
                return 0;

            case WM_DESTROY:
                UnregisterHotKey(hwnd, kHotkeyFreezeGlass);
                KillTimer(hwnd, kTimerCursorPoll);
                KillTimer(hwnd, kTimerTaskbarGuard);
                KillTimer(hwnd, kTimerWindowRefresh);
                KillTimer(hwnd, kTimerFullscreenCheck);
                KillTimer(hwnd, kTimerGlassMaintenance);
                g_dock.glassMaintenanceTimerActive = false;
                KillTimer(hwnd, kTimerFlyoutHover);
                KillTimer(hwnd, kTimerFlyoutLeave);
                KillTimer(hwnd, kTimerTaskbarPinSync);
                PostQuitMessage(0);
                return 0;
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"DockWndProc WinRT error: 0x%08X", (unsigned)e.code());
    } catch (...) {
        Wh_Log(L"DockWndProc unknown exception");
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool CreateDockWindow(HINSTANCE hInst) {
    WNDCLASSEXW wc{
        .cbSize = sizeof(wc),
        .lpfnWndProc = DockWndProc,
        .hInstance = hInst,
        .hCursor = LoadCursorW(nullptr, IDC_ARROW),
        .lpszClassName = kDockClassName,
    };
    RegisterClassExW(&wc);

    DWORD exStyle = WS_EX_NOREDIRECTIONBITMAP | WS_EX_LAYERED | WS_EX_TOPMOST |
                    WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT;
    g_dock.hwnd = CreateWindowExW(exStyle, kDockClassName, L"macOS 26 Tahoe Dock",
                                  WS_POPUP, 0, 0, 100, 100, nullptr, nullptr,
                                  hInst, nullptr);
    if (!g_dock.hwnd) {
        Wh_Log(L"CreateWindowExW failed: %lu", GetLastError());
        return false;
    }
    g_dock.clickThrough = true;

    SetLayeredWindowAttributes(g_dock.hwnd, 0, 255, LWA_ALPHA);

    const BOOL hostBackdrop = TRUE;
    HRESULT hr = DwmSetWindowAttribute(g_dock.hwnd, kDwmUseHostBackdropBrush,
                                       &hostBackdrop, sizeof(hostBackdrop));
    g_dock.hasHostBackdrop = SUCCEEDED(hr);
    if (FAILED(hr)) {
        Wh_Log(L"DWMWA_USE_HOSTBACKDROPBRUSH failed: 0x%08X", (unsigned)hr);
    }

    g_taskbarHider.taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");
    RegisterHotKey(g_dock.hwnd, kHotkeyFreezeGlass,
                   MOD_WIN | MOD_CONTROL | MOD_SHIFT | MOD_NOREPEAT, 'G');
    SetTimer(g_dock.hwnd, kTimerCursorPoll, 100, nullptr);
    SetTimer(g_dock.hwnd, kTimerTaskbarGuard, 2000, nullptr);
    SetGlassMaintenanceTimer(WantOpticalLiquidGlass());
    return true;
}

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
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"ShutdownQueueAsync failed: 0x%08X", (unsigned)e.code());
    } catch (...) {
    }
}

DWORD WINAPI UiThreadProc(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HRESULT oleHr = OleInitialize(nullptr);

    HMODULE hCoreMsg = LoadLibraryExW(L"CoreMessaging.dll", nullptr,
                                      LOAD_LIBRARY_SEARCH_SYSTEM32);
    using CreateDispatcherQueueController_t = HRESULT(WINAPI*)(
        DispatcherQueueOptions,
        ABI::Windows::System::IDispatcherQueueController**);
    auto createDqc = hCoreMsg
                         ? reinterpret_cast<CreateDispatcherQueueController_t>(
                               GetProcAddress(
                                   hCoreMsg,
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
            Wh_Log(L"UiThread: Compositor created");
            if (!InitGraphics(compositor)) {
                Wh_Log(L"UiThread: InitGraphics failed");
            } else if (!CreateDockWindow(hSelf)) {
                Wh_Log(L"UiThread: CreateDockWindow failed");
            } else if (!BuildDockVisualTree()) {
                Wh_Log(L"UiThread: BuildDockVisualTree failed");
            } else {
                Wh_Log(L"UiThread: Dock initialized (%zu items)", g_items.size());
                ok = true;
            }
        } else {
            Wh_Log(L"CreateDispatcherQueueController failed");
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"UiThread init WinRT error: 0x%08X", (unsigned)e.code());
    } catch (...) {
        Wh_Log(L"UiThread init unknown error");
    }

    if (ok) {
        CreateAppBarWindow(hSelf);
        StartWindowTracker(g_dock.hwnd);
        if (SUCCEEDED(oleHr) && g_dock.hwnd) {
            g_dropTarget = new DockOleDropTarget();
            RegisterDragDrop(g_dock.hwnd, g_dropTarget);
        }
        RegisterShellNotifications(g_dock.hwnd);
        if (g_settings.hideTaskbar) {
            ApplyTaskbarHiding();
        } else if (Wh_GetIntValue(L"restore_autohide", -1) != -1) {
            RestoreTaskbar();
        }
        UpdateAppBarReservation();
        CheckFullscreenState();
        g_dock.lastTickTime = NowSeconds();
        TickFrame(g_dock.lastTickTime);

        bool running = true;
        MSG msg;
        while (running) {
            if (g_dock.animating || g_dock.layoutDirty) {
                while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                    if (msg.message == WM_QUIT) {
                        running = false;
                        break;
                    }
                    TranslateMessage(&msg);
                    DispatchMessageW(&msg);
                }
                if (!running) {
                    break;
                }
                ULONGLONG frameStart = GetTickCount64();
                TickFrame(NowSeconds());
                HRESULT flushHr = DwmFlush();
                if (FAILED(flushHr) || (GetTickCount64() - frameStart) < 4u) {
                    Sleep(12);
                }
            } else {
                BOOL r = GetMessageW(&msg, nullptr, 0, 0);
                if (r <= 0) {
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
                if (g_dock.layoutDirty && !g_dock.animating) {
                    TickFrame(NowSeconds());
                }
            }
        }
    }

    StopWindowTracker(g_dock.hwnd);
    CloseTaskbarSyncKey();
    UnregisterShellNotifications();
    EndDividerResize(false);
    EndInternalDockDrag(false);
    CloseWindowPreviewFlyout();
    CloseTahoeSettingsPanel();
    CloseDownloadsFan();
    if (g_dock.hwnd && g_dropTarget) {
        RevokeDragDrop(g_dock.hwnd);
        g_dropTarget->Release();
        g_dropTarget = nullptr;
    }
    RemoveAppBar();
    if (g_dock.appBarHwnd) {
        DestroyWindow(g_dock.appBarHwnd);
        g_dock.appBarHwnd = nullptr;
    }
    UnregisterClassW(kAppBarClassName, hSelf);
    RestoreTaskbar();
    DestroyDockVisualTree();
    g_gfx.Shutdown();
    if (g_dock.hwnd) {
        DestroyWindow(g_dock.hwnd);
        g_dock.hwnd = nullptr;
    }
    UnregisterClassW(kDockClassName, hSelf);
    ShutdownDispatcherQueue();

    if (SUCCEEDED(oleHr)) {
        OleUninitialize();
    }
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"WhTool_ModInit");
    LoadSettings(false);
    g_quitting = false;
    g_uiThread =
        CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &g_uiThreadId);
    return g_uiThread != nullptr;
}

void WhTool_ModSettingsChanged() {
    if (g_dock.hwnd) {
        PostMessageW(g_dock.hwnd, kWmAppSettings, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"WhTool_ModUninit");
    g_quitting = true;
    if (g_dock.hwnd) {
        PostMessageW(g_dock.hwnd, kWmAppQuit, 0, 0);
    } else if (g_uiThreadId) {
        PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
    }
    if (g_uiThread) {
        if (WaitForSingleObject(g_uiThread, 5000) == WAIT_TIMEOUT) {
            Wh_Log(L"UI thread shutdown timed out");
        }
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
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
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
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

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
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
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
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
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
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
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
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

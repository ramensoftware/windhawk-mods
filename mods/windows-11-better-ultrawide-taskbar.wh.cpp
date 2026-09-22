// ==WindhawkMod==
// @id              windows-11-better-ultrawide-taskbar
// @name            Windows 11 Better Ultrawide Taskbar
// @description     Centers taskbar buttons and the system tray in a rounded dock while preserving native button animations
// @version         2.95
// @author          Molko
// @github          https://github.com/roeseth
// @include         explorer.exe
// @include         ShellHost.exe
// @include         ShellExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -ldwmapi -lwindowsapp -ld3d11 -ldxgi -ld3dcompiler -lgdi32 -luser32 -lkernel32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// Portions of the liquid-glass profile are adapted from @ybouane/liquidglass.
// The Gaussian box-radius calculation is adapted from FastGaussianBlur:
// Copyright (C) 2017-2022 Basile Fraboni
// Copyright (C) 2014 Ivan Kutskir
//
// Both upstream works are used under the MIT License:
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# Windows 11 Better Ultrawide Taskbar

Centers the taskbar buttons, a 12-DIP gap, and the system tray as one rounded
dock on each monitor. The mod uses Windows' native sizing and animation paths;
it follows the existing compositor motion instead of polling or copying its
easing.

The mod supports native Windows labels and the Taskbar Labels mod in fixed and
adaptive width modes. Quick Settings, notifications, and the calendar follow
the moved tray. Windows taskbar alignment must be set to Center.

Choose **Dock theme** in the mod settings: Windows, Glass, Frosted glass, or
Liquid glass. Liquid glass captures only desktop and app windows below the
taskbar and refracts them with a GPU shader. The taskbar remains visible in
screenshots and screen sharing. All themes follow the same animated dock
outline. Liquid keeps the native fill transparent while capture or GPU setup
is unavailable, and transient GPU failures are retried automatically. High
contrast retains the Windows material. Changing the setting reloads the mod.

**Font color override** can retain Windows' color, force light or dark text, or
adapt each text element from its local composed background. Adaptive changes
fade between colors. High contrast always retains Windows' colors.

Task-button hover uses Windows' native animation as a translucent tint over
the dock. An app requesting attention receives an additional inset refractive
lens with the native alert color.

**Task and tray buttons** can carry the raised gradient outline Windows draws on a
highlight. The dock's translucency fades the native one out, so the mod redraws it
at matching strength; turn the border off for a flat highlight.

Under the Liquid glass theme each task button can also become its own piece of
liquid glass inside the dock, carrying the same edge displacement, chromatic split,
specular and fresnel as the dock itself. It is the same shader pass, not a second
capture. Tray items share the dock lens.

Liquid glass includes the reference site's Regular, Frosted, and Dark presets.
Regular is the reference defaults with no backdrop blur; Frosted adds it, and
Dark also dims the backdrop. Choose Custom to use the individual optical
parameters.

The **Taskbar height and icon size** mod is detected automatically. When using
it, set that mod's taskbar height to:
`normal height + edge distance - 4`.
Native task and tray hover plates fit the dock without changing button
layout.

If native controls do not fit, the visual tree is unsupported, taskbar
ownership changes, or more than 64 children are realized, that taskbar stays
in its native XAML layout. Layout and capacity fallbacks recover on a later
layout pass; a taskbar whose translation, clip or tree ownership was taken over
stays native until the mod is re-enabled. The configured frame height remains
active until the mod is disabled because Windows exposes it as a process-wide
setting.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- theme: native
  $name: Dock theme (default Windows)
  $description: Choose a native material or screenshot-safe optical refraction.
  $options:
  - native: Windows
  - glass: Glass
  - frosted: Frosted glass
  - liquid: Liquid glass (refraction)
- fontColor: default
  $name: Font color override (default Windows)
  $description: Keep Windows' taskbar text color, force light or dark text, or fade each text element to the higher-contrast color for its local composed background.
  $options:
  - default: Windows
  - light: Light
  - dark: Dark
  - adaptive: Adaptive
- cornerRadius: 12
  $name: Taskbar corner radius (DIP, default 12)
  $description: Rounded dock corner radius from 0 to 100.
- buttonCornerRadius: 12
  $name: Button corner radius (DIP, default 12)
  $description: Rounded task and tray button highlights from 0 to 100.
- buttonVerticalPadding: 4
  $name: Button vertical padding (DIP, default 4)
  $description: Space above and below task and tray button highlights from 0 to 100.
- button:
  - border: true
    $name: Border (default on)
    $description: Draws the raised gradient outline Windows puts on task button highlights, which the dock's translucency would otherwise fade out.
  - liquidLayer: false
    $name: Per-button liquid glass layer (default off)
    $description: Renders every task button as its own piece of liquid glass inside the dock, with the same edge displacement, chromatic split, specular and fresnel as the dock. Liquid glass theme only.
  $name: Task and tray buttons
  $description: Appearance of the native task and tray button highlights.
- edgeDistance: 4
  $name: Distance from screen edge (DIP, default 4)
  $description: Gap from 0 to 100. The native taskbar frame adjusts with the gap so the dock keeps its height.
- liquid:
  - preset: custom
    $name: Preset (default Custom)
    $description: Reference presets override the parameters below. Custom uses them directly.
    $options:
    - custom: Custom
    - regular: Regular glass
    - frosted: Frosted glass
    - dark: Dark glass
  - zRadius: 40
    $name: Depth (DIP, default 40)
    $description: Curvature depth from 1 to 100, clamped to the dock thickness for a smooth surface. Used only by the Custom preset.
  - refraction: 69
    $name: Refraction (%, default 69)
    $description: Light bending from 0 to 200. Used only by the Custom preset.
  - chromaticAberration: 5
    $name: Chromatic aberration (%, default 5)
    $description: Color separation from 0 to 30. Used only by the Custom preset.
  - edgeHighlight: 5
    $name: Edge highlight (%, default 5)
    $description: Rim brightness from 0 to 100. Used only by the Custom preset.
  - specular: 0
    $name: Specular highlight (%, default 0)
    $description: Reflected light from 0 to 100. Used only by the Custom preset.
  - fresnel: 100
    $name: Fresnel reflection (%, default 100)
    $description: Edge reflection from 0 to 200. Used only by the Custom preset.
  - opacity: 100
    $name: Opacity (%, default 100)
    $description: Glass opacity from 0 to 100. Used only by the Custom preset.
  - brightness: 0
    $name: Brightness (%, default 0)
    $description: Backdrop brightness from -50 to 50. Used only by the Custom preset.
  - blur: 0
    $name: Blur (%, default 0)
    $description: Fast Gaussian backdrop blur from 0 to 100. Used only by the Custom preset.
  $name: Liquid glass
  $description: Optical parameters for the Liquid glass theme only.
*/
// ==/WindhawkModSettings==

// No button width, label, button-transform, easing, or divider overrides.
#include <windhawk_utils.h>
#include <atomic>
#include <cmath>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string_view>
#include <tlhelp32.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dwmapi.h>
#include <inspectable.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>

using namespace winrt::Windows::UI::Xaml;
using winrt::Windows::Foundation::Size;
using winrt::Windows::Foundation::Numerics::float3;
using namespace winrt::Windows::UI::Composition;
namespace capture = winrt::Windows::Graphics::Capture;
namespace directx = winrt::Windows::Graphics::DirectX;
namespace direct3d11 = winrt::Windows::Graphics::DirectX::Direct3D11;
namespace animation = winrt::Windows::UI::Xaml::Media::Animation;

static constexpr GUID kIID_IGraphicsCaptureItemInterop{
    0x3628E81B, 0x3CAC, 0x4C60,
    {0xB7, 0xF4, 0x23, 0xCE, 0x0E, 0x0C, 0x33, 0x56}};
static constexpr GUID kIID_IGraphicsCaptureItem{
    0x79C3F95B, 0x31F7, 0x4EC2,
    {0xA4, 0x64, 0x63, 0x2E, 0xF5, 0xD3, 0x07, 0x60}};
static constexpr GUID kIID_IDirect3DDxgiInterfaceAccess{
    0xA9B3D012, 0x3DF2, 0x4EE3,
    {0xB8, 0xD1, 0x86, 0x95, 0xF4, 0x57, 0xD3, 0xC1}};
static constexpr GUID kIID_ICompositorInterop{
    0x25297D5C, 0x3AD4, 0x4C9C,
    {0xB5, 0xCF, 0xE3, 0x6A, 0x38, 0x51, 0x23, 0x30}};

struct IGraphicsCaptureItemInterop : ::IUnknown {
    virtual HRESULT __stdcall CreateForWindow(HWND, REFIID, void**) = 0;
};
struct IDirect3DDxgiInterfaceAccess : ::IUnknown {
    virtual HRESULT __stdcall GetInterface(REFIID, void**) = 0;
};
struct ICompositorInterop : ::IUnknown {
    virtual HRESULT __stdcall CreateCompositionSurfaceForHandle(HANDLE, IUnknown**) = 0;
    virtual HRESULT __stdcall CreateCompositionSurfaceForSwapChain(IUnknown*, IUnknown**) = 0;
};
extern "C" HRESULT __stdcall CreateDirect3D11DeviceFromDXGIDevice(
    IDXGIDevice*, IInspectable**);

constexpr double kGap = 12;
constexpr double kDockPadding = 8;
constexpr double kDockInnerMargin = 4;
constexpr UINT kRestoreMessage = WM_APP + 43;
constexpr UINT kQueryTrayMessage = WM_APP + 44;
constexpr UINT kPrepareLiquidMessage = WM_APP + 45;
constexpr UINT kRefreshItemStatesMessage = WM_APP + 46;
constexpr UINT kForegroundSampleMessage = WM_APP + 47;
constexpr UINT_PTR kLiquidTimerId = 45;
// A transient measurement fails a handful of consecutive layout passes at most;
// an unsupported build fails every one. Tune down if a broken tree costs too many
// tree walks, up if a slow startup transition needs longer to settle.
constexpr unsigned kRefreshRetryLimit = 10;
// Translucency of the native highlight over the dock. Also divides the rim alphas
// below, so a rim drawn inside the plate lands at native strength after this.
constexpr double kPlateDimScale = 0.32;
// One lens per realized task button; UpdateFollower already refuses more than 64.
#define UW_BUTTON_LENS_CAP 64
#define UW_STRINGIZE_IMPL(value) #value
#define UW_STRINGIZE(value) UW_STRINGIZE_IMPL(value)
constexpr size_t kMaxButtonLenses = UW_BUTTON_LENS_CAP;
constexpr wchar_t kWindowClass[] = L"Windhawk.BetterUltrawideTaskbar.2";
std::atomic<bool> g_stopping{};
std::atomic<unsigned> g_active{};
std::atomic<DWORD> g_ownerThread{};
std::atomic<HWND> g_dispatch{};
std::atomic<bool> g_dockingDisabled{};
std::atomic<bool> g_taskbarViewHookStarted{};
std::atomic<bool> g_taskbarViewReady{};
ATOM g_windowClass{};
bool g_busy{};  // Owner thread only.
bool g_explorer{};
bool g_preparingLiquid{};  // Owner thread only.
bool g_liquidPreparePending{};  // Owner thread only.
bool g_itemStateRefreshPending{};  // Owner thread only.
bool g_liquidTimerArmed{};  // Owner thread only.
UINT g_discoverMessage{};
enum class DockTheme { Native, Glass, Frosted, Liquid };
enum class FontColorMode { Default, Light, Dark, Adaptive };
DockTheme g_theme{};
FontColorMode g_fontColorMode{};
double g_dockCornerRadius{12};
double g_buttonCornerRadius{12};
double g_buttonVerticalPadding{4};
double g_dockEdgeDistance{4};
bool g_keepButtonRim{true};
bool g_buttonLiquidLayer{};

// Holds off the teardown in Wh_ModBeforeUninit while a hook or callback runs.
struct ActiveScope {
    ActiveScope() { ++g_active; }
    ~ActiveScope() { --g_active; }
};

struct DockInsets {
    double top{};
    double bottom{};
};

bool TaskbarAtTop(RECT const& taskbar, RECT const& monitor) {
    auto topDistance = std::abs(int64_t{taskbar.top} - monitor.top);
    auto bottomDistance = std::abs(int64_t{monitor.bottom} - taskbar.bottom);
    return topDistance < bottomDistance;
}

bool TaskbarAtTop(HWND taskbar) {
    RECT taskbarRect{};
    MONITORINFO monitor{};
    monitor.cbSize = sizeof(monitor);
    return taskbar && GetWindowRect(taskbar, &taskbarRect) &&
           GetMonitorInfo(MonitorFromWindow(
                              taskbar, MONITOR_DEFAULTTONEAREST),
                          &monitor) &&
           TaskbarAtTop(taskbarRect, monitor.rcMonitor);
}

DockInsets PlanDockInsets(bool taskbarAtTop, double height,
                          double edgeDistance) {
    double outer = std::clamp(
        edgeDistance, 0.0,
        std::max(0.0, height - kDockInnerMargin - 1));
    return taskbarAtTop ? DockInsets{outer, kDockInnerMargin}
                        : DockInsets{kDockInnerMargin, outer};
}

double DockFrameAdjustment(double edgeDistance) {
    return edgeDistance - kDockInnerMargin;
}

winrt::Windows::Foundation::Rect PlanVerticalClip(
    double width, double height, double top, double bottom) {
    if (!std::isfinite(width) || !std::isfinite(height) ||
        !std::isfinite(top) || !std::isfinite(bottom) ||
        width <= 0 || height <= 0 || bottom <= top)
        return {};
    top = std::clamp(top, 0.0, height);
    bottom = std::clamp(bottom, top, height);
    return {0, static_cast<float>(top), static_cast<float>(width),
            static_cast<float>(bottom - top)};
}

bool IsTaskbarIconSizeModuleName(std::wstring_view name) {
    if (auto separator = name.find_last_of(L"\\/");
        separator != std::wstring_view::npos)
        name.remove_prefix(separator + 1);
    if (name.starts_with(L"local@")) name.remove_prefix(6);
    return name.starts_with(L"taskbar-icon-size_") &&
           name.ends_with(L".dll");
}

bool TaskbarIconSizeLoaded() {
    HANDLE snapshot = CreateToolhelp32Snapshot(
        TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
    if (snapshot == INVALID_HANDLE_VALUE) return false;
    MODULEENTRY32W module{};
    module.dwSize = sizeof(module);
    bool found = Module32FirstW(snapshot, &module);
    while (found && !IsTaskbarIconSizeModuleName(module.szModule))
        found = Module32NextW(snapshot, &module);
    CloseHandle(snapshot);
    return found;
}

DockInsets GetDockInsets(HWND taskbar, double height) {
    return PlanDockInsets(TaskbarAtTop(taskbar), height, g_dockEdgeDistance);
}

DockInsets GetButtonInsets(DockInsets insets, double height) {
    double padding = std::clamp(
        g_buttonVerticalPadding, 0.0,
        std::max(0.0, (height - insets.top - insets.bottom - 1) / 2));
    return {insets.top + padding, insets.bottom + padding};
}

// Orientation only swaps the two insets, so their sum is identical before and
// after the native taskbar is resolved. Lets Refresh check the height it was
// handed without reaching for the window.
double DockInsetSum(double height) {
    auto insets = PlanDockInsets(false, height, g_dockEdgeDistance);
    return insets.top + insets.bottom;
}

double EffectiveDockRadius(double width, double height) {
    return std::min(g_dockCornerRadius,
                    std::max(0.0, std::min(width, height) / 2));
}

struct LiquidSettings {
    float zRadius{40};
    float refraction{0.69f};
    float chroma{0.05f};
    float edgeHighlight{0.05f};
    float specular{};
    float fresnel{1};
    float opacity{1};
    float brightness{};
    float blur{};
} g_liquidSettings;
static_assert(sizeof(LiquidSettings) == 9 * sizeof(float));

LiquidSettings ApplyLiquidPreset(std::wstring_view preset,
                                 LiquidSettings custom) {
    if (preset != L"regular" && preset != L"frosted" && preset != L"dark")
        return custom;
    // This struct's initialisers are ybouane/liquidglass DEFAULTS verbatim, so
    // Regular is those untouched. Frosted and Dark are the reference site's only
    // optical overrides: {blurAmount: 0.25} and {brightness: -0.3, blurAmount: 0.25}.
    LiquidSettings result;
    if (preset != L"regular") result.blur = 0.25f;
    if (preset == L"dark") result.brightness = -0.3f;
    return result;
}

void FastGaussianBoxRadii(float sigma, int (&radii)[3]) {
    float idealWidth = std::sqrt(4 * sigma * sigma + 1);
    int lowerWidth = static_cast<int>(idealWidth);
    if (lowerWidth % 2 == 0) --lowerWidth;
    int upperWidth = lowerWidth + 2;
    float idealLowerCount =
        (12 * sigma * sigma - 3 * lowerWidth * lowerWidth -
         12 * lowerWidth - 9) /
        (-4.0f * lowerWidth - 4);
    int lowerCount = static_cast<int>(idealLowerCount + 0.5f);
    for (int i = 0; i < 3; ++i)
        radii[i] = ((i < lowerCount ? lowerWidth : upperWidth) - 1) / 2;
}

DockTheme ParseTheme(std::wstring_view name) {
    if (name == L"glass") return DockTheme::Glass;
    if (name == L"frosted") return DockTheme::Frosted;
    if (name == L"liquid") return DockTheme::Liquid;
    return DockTheme::Native;
}

FontColorMode ParseFontColor(std::wstring_view name) {
    if (name == L"light") return FontColorMode::Light;
    if (name == L"dark") return FontColorMode::Dark;
    if (name == L"adaptive") return FontColorMode::Adaptive;
    return FontColorMode::Default;
}

bool PreferLightForeground(double luminance, bool previous, bool ready) {
    constexpr double pivot = 0.179;  // Equal black/white WCAG contrast.
    constexpr double hysteresis = 0.02;
    if (!ready) return luminance < pivot;
    return previous ? luminance < pivot + hysteresis
                    : luminance < pivot - hysteresis;
}

using TaskbarConfiguration_GetFrameSize_t = double(WINAPI*)(int);
TaskbarConfiguration_GetFrameSize_t TaskbarConfiguration_GetFrameSize_Original;
double WINAPI TaskbarConfiguration_GetFrameSize_Hook(int taskbarSize) {
    double height = TaskbarConfiguration_GetFrameSize_Original(taskbarSize);
    double adjustment = DockFrameAdjustment(g_dockEdgeDistance);
    if (!g_dockingDisabled && !TaskbarIconSizeLoaded() &&
        (taskbarSize == 1 || taskbarSize == 2) &&
        std::isfinite(height) && height > 0 && height + adjustment > 0)
        height += adjustment;
    return height;
}

// ---- taskbar.dll symbol lookup and XAML root discovery ------------------
// Same taskbar.dll root lookup used by hide-start-button and
// taskbar-centered-start-split-icons. Resolve functions without hooking them.
void* g_taskBandVtable{};
void* g_secondaryTaskBandVtable{};
using GetTaskbarHost_t = void*(WINAPI*)(void*, void**);
GetTaskbarHost_t GetTaskbarHost_Original;
GetTaskbarHost_t GetSecondaryTaskbarHost_Original;
using Decref_t = void(WINAPI*)(void*);
Decref_t Decref_Original;
void* g_frameHeight{};
size_t g_hostElementOffset{};
bool ClassIs(HWND window, const wchar_t* expected);

template<typename T>
bool ReadLocal(const void* address, T& value) {
    SIZE_T bytes{};
    return address && ReadProcessMemory(GetCurrentProcess(), address, &value, sizeof(value), &bytes) &&
           bytes == sizeof(value);
}

bool HostElementOffset(const BYTE (&code)[8], size_t& offset) {
    // x64: sub rsp,imm8; add rcx,imm8. No guessed offset on a changed build.
    if (code[0] != 0x48 || code[1] != 0x83 || code[2] != 0xEC ||
        code[4] != 0x48 || code[5] != 0x83 || code[6] != 0xC1 ||
        code[7] < sizeof(void*) || code[7] > 0x7F || code[7] % sizeof(void*)) return false;
    offset = code[7];
    return true;
}

void* TaskBandInterface(void* object, void* expectedVtable) {
    if (!object || !expectedVtable) return nullptr;
    // Bounded interface search from the existing mods, using checked reads so
    // missing/recreated taskband data cannot cause an unchecked dereference.
    for (size_t i = 0; i < 20; ++i) {
        auto address = reinterpret_cast<uintptr_t>(object) + i * sizeof(void*);
        void* vtable{};
        if (!ReadLocal(reinterpret_cast<void*>(address), vtable)) return nullptr;
        if (vtable == expectedVtable) return reinterpret_cast<void*>(address);
    }
    return nullptr;
}

XamlRoot RootFromHost(void* (&host)[2]) {
    struct Release {
        void** host;
        ~Release() { if (host[1]) Decref_Original(host[1]); }
    } release{host};
    if (!host[0] || !host[1] || !g_hostElementOffset) return nullptr;
    IUnknown* element{};
    if (!ReadLocal(reinterpret_cast<BYTE*>(host[0]) + g_hostElementOffset, element) || !element)
        return nullptr;
    FrameworkElement frame{nullptr};
    if (FAILED(element->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(frame))))
        return nullptr;
    return frame ? frame.XamlRoot() : nullptr;
}

XamlRoot ExistingRoot(HWND taskbar) {
    DWORD pid{};
    DWORD thread = GetWindowThreadProcessId(taskbar, &pid);
    bool secondary = ClassIs(taskbar, L"Shell_SecondaryTrayWnd");
    if (pid != GetCurrentProcessId() || thread != GetCurrentThreadId() ||
        (!secondary && !ClassIs(taskbar, L"Shell_TrayWnd"))) return nullptr;
    HWND bandWindow = secondary ? FindWindowEx(taskbar, nullptr, L"WorkerW", nullptr)
                                : static_cast<HWND>(GetProp(taskbar, L"TaskbandHWND"));
    DWORD bandPid{};
    if (!bandWindow || GetWindowThreadProcessId(bandWindow, &bandPid) != thread || bandPid != pid)
        return nullptr;
    void* site = TaskBandInterface(reinterpret_cast<void*>(GetWindowLongPtr(bandWindow, 0)),
                                  secondary ? g_secondaryTaskBandVtable : g_taskBandVtable);
    if (!site) return nullptr;
    void* host[2]{};
    (secondary ? GetSecondaryTaskbarHost_Original : GetTaskbarHost_Original)(site, host);
    return RootFromHost(host);
}

// ---- Cross-process flyout positioning -----------------------------------
// Numeric HWND properties only: no pointers shared between Shell processes.
constexpr wchar_t kTrayShift[] = L"Windhawk.UWPOS05.TrayShift";
constexpr wchar_t kTrayDispatcher[] = L"Windhawk.UWPOS08.Dispatcher";
constexpr wchar_t kPanelPosition[] = L"Windhawk.UWPOS05.PanelPosition";
constexpr wchar_t kPanelTaskbar[] = L"Windhawk.UWPOS05.PanelTaskbar";
#ifndef UWPANEL_PENDING_PROPERTY
#define UWPANEL_PENDING_PROPERTY L"Windhawk.UWPOS09.PanelPending"
#endif
constexpr wchar_t kPanelPending[] = UWPANEL_PENDING_PROPERTY;
decltype(&SetWindowPos) SetWindowPos_Original;
decltype(&DwmSetWindowAttribute) DwmSetWindowAttribute_Original;
decltype(&DestroyWindow) DestroyWindow_Original;
thread_local bool g_panelBusy{};
HWINEVENTHOOK g_panelLocationHook{};

HANDLE EncodeShift(int shift) {
    return reinterpret_cast<HANDLE>(uint64_t{static_cast<uint32_t>(shift)} + 1);
}
int DecodeShift(HANDLE value) {
    return static_cast<int32_t>(reinterpret_cast<uintptr_t>(value) - 1);
}
HANDLE EncodePosition(int nativeX, int appliedX) {
    return reinterpret_cast<HANDLE>((uint64_t{static_cast<uint32_t>(nativeX)} << 32) |
                                    static_cast<uint32_t>(appliedX));
}
void DecodePosition(HANDLE value, int& nativeX, int& appliedX) {
    auto bits = reinterpret_cast<uintptr_t>(value);
    nativeX = static_cast<int32_t>(bits >> 32);
    appliedX = static_cast<int32_t>(bits);
}

bool ClassIs(HWND window, const wchar_t* expected) {
    wchar_t name[64];
    return GetClassName(window, name, ARRAYSIZE(name)) && !wcscmp(name, expected);
}

bool IsPanel(HWND window) {
    bool controls = ClassIs(window, L"ControlCenterWindow");
    if (!controls && !ClassIs(window, L"Windows.UI.Core.CoreWindow")) return false;
    DWORD pid{};
    DWORD tid = GetWindowThreadProcessId(window, &pid);
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return false;
    wchar_t path[MAX_PATH];
    DWORD length = ARRAYSIZE(path);
    bool read = QueryFullProcessImageName(process, 0, path, &length);
    CloseHandle(process);
    auto name = read ? wcsrchr(path, L'\\') : nullptr;
    if (!name || _wcsicmp(name + 1, controls ? L"ShellHost.exe" : L"ShellExperienceHost.exe")) return false;
    if (controls) return true;
    HANDLE thread = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, tid);
    if (!thread) return false;
    PWSTR description{};
    bool actionCenter = SUCCEEDED(GetThreadDescription(thread, &description)) &&
                        description && !wcscmp(description, L"ActionCenter");
    LocalFree(description);
    CloseHandle(thread);
    return actionCenter;
}

// Require the observed full-height right-edge host, not every ActionCenter window.
bool PlanPanel(RECT work, RECT requested, int shift, HANDLE previous, int& nativeX, int& targetX) {
    int64_t width = int64_t{requested.right} - requested.left;
    int64_t workWidth = int64_t{work.right} - work.left;
    if (width <= 0 || width >= workWidth || requested.top != work.top ||
        requested.bottom != work.bottom || std::abs(int64_t{shift}) > workWidth) return false;
    nativeX = requested.left;
    int oldNative{}, oldApplied{};
    if (previous) {
        DecodePosition(previous, oldNative, oldApplied);
        if (requested.right != work.right && nativeX == oldApplied &&
            oldNative >= work.left && oldNative < work.right)
            nativeX = static_cast<int>(work.right - width);
    }
    if (int64_t{nativeX} + width != work.right) return false;
    targetX = static_cast<int>(std::clamp(int64_t{nativeX} + shift,
                                        int64_t{work.left}, int64_t{work.right} - width));
    return true;
}

HWND PublishedTaskbar(HMONITOR monitor) {
    HWND found{};
    // FindWindowEx includes the Shell windows omitted by EnumWindows here.
    HWND window{};
    for (int i = 0; i < 4096 && (window = FindWindowEx(nullptr, window, nullptr, nullptr)); ++i) {
        if (MonitorFromWindow(window, MONITOR_DEFAULTTONULL) != monitor ||
            !GetProp(window, kTrayShift) ||
            (!ClassIs(window, L"Shell_TrayWnd") && !ClassIs(window, L"Shell_SecondaryTrayWnd"))) continue;
        if (found) return nullptr;
        found = window;
    }
    return found;
}

HANDLE QueryTrayShift(HWND taskbar) {
    if (!taskbar) return nullptr;
    HWND dispatcher = static_cast<HWND>(GetProp(taskbar, kTrayDispatcher));
    if (!dispatcher) return GetProp(taskbar, kTrayShift); // Older/static publisher.
    DWORD barPid{}, dispatcherPid{};
    GetWindowThreadProcessId(taskbar, &barPid);
    GetWindowThreadProcessId(dispatcher, &dispatcherPid);
    if (!barPid || dispatcherPid != barPid || !ClassIs(dispatcher, kWindowClass)) return nullptr;
    DWORD_PTR result{};
    // No pointers cross process boundaries. Fail native if the UI thread is busy.
    if (!SendMessageTimeout(dispatcher, kQueryTrayMessage, reinterpret_cast<WPARAM>(taskbar), 0,
                            SMTO_ABORTIFHUNG | SMTO_BLOCK, 50, &result)) return nullptr;
    return reinterpret_cast<HANDLE>(result);
}

void RestorePanel(HWND window) {
    auto dpiContext = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    struct RestoreDpi {
        DPI_AWARENESS_CONTEXT value;
        ~RestoreDpi() { if (value) SetThreadDpiAwarenessContext(value); }
    } restoreDpi{dpiContext};
    HANDLE record = GetProp(window, kPanelPosition);
    if (!record) return;
    int nativeX{}, appliedX{};
    DecodePosition(record, nativeX, appliedX);
    RECT rect{};
    bool owned = IsPanel(window) && GetWindowRect(window, &rect) && rect.left == appliedX;
    // Remove first so a synchronous reentrant call cannot apply the offset again.
    RemoveProp(window, kPanelPosition);
    RemoveProp(window, kPanelTaskbar);
    if (owned) {
        bool busy = g_panelBusy;
        g_panelBusy = true;
        BOOL moved = SetWindowPos_Original(window, nullptr, nativeX, rect.top, 0, 0,
                                          SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
        g_panelBusy = busy;
        RECT after{};
        Wh_Log(L"[UWPANEL] restore hwnd=%p nativeX=%d ok=%d verified=%d", window, nativeX,
               moved, moved && GetWindowRect(window, &after) && after.left == nativeX);
    }
}

void RestorePanels(HWND taskbar = nullptr, bool ownProcessOnly = false) {
    HWND window{};
    for (int i = 0; i < 4096 && (window = FindWindowEx(nullptr, window, nullptr, nullptr)); ++i) {
        if (GetProp(window, kPanelPending)) RemoveProp(window, kPanelPending);
        if (!GetProp(window, kPanelPosition) ||
            (taskbar && GetProp(window, kPanelTaskbar) != taskbar)) continue;
        DWORD pid{};
        GetWindowThreadProcessId(window, &pid);
        if (!ownProcessOnly || pid == GetCurrentProcessId()) RestorePanel(window);
    }
}

BOOL WINAPI PanelSetWindowPos(HWND window, HWND after, int x, int y, int cx, int cy, UINT flags) {
    ActiveScope active;
    auto original = [&] { return SetWindowPos_Original(window, after, x, y, cx, cy, flags); };
    if (g_stopping || g_panelBusy || (flags & (SWP_HIDEWINDOW | SWP_ASYNCWINDOWPOS)) ||
        !IsPanel(window))
        return original();
    // Keep native coordinate units. Only consume physical pixels in a PM-aware call.
    if (GetAwarenessFromDpiAwarenessContext(GetThreadDpiAwarenessContext()) != DPI_AWARENESS_PER_MONITOR_AWARE)
        return original();
    RECT current{};
    if (!GetWindowRect(window, &current)) return original();
    int left = flags & SWP_NOMOVE ? current.left : x;
    int top = flags & SWP_NOMOVE ? current.top : y;
    int width = flags & SWP_NOSIZE ? current.right - current.left : cx;
    int height = flags & SWP_NOSIZE ? current.bottom - current.top : cy;
    if (width <= 0 || height <= 0 || int64_t{left} + width > INT_MAX ||
        int64_t{top} + height > INT_MAX) return original();
    RECT requested{left, top, left + width, top + height};
    HMONITOR monitor = MonitorFromRect(&requested, MONITOR_DEFAULTTONULL);
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    HWND taskbar = PublishedTaskbar(monitor);
    HANDLE publication = QueryTrayShift(taskbar);
    if (!publication || !GetMonitorInfo(monitor, &info)) return original();
    HANDLE previous = GetProp(window, kPanelPosition);
    HANDLE previousTaskbar = GetProp(window, kPanelTaskbar);
    int nativeX{}, targetX{};
    if (!PlanPanel(info.rcWork, requested, DecodeShift(publication), previous, nativeX, targetX)) {
        Wh_Log(L"[UWPANEL] native fallback hwnd=%p rect=%ld,%ld,%ld,%ld flags=%x", window,
               requested.left, requested.top, requested.right, requested.bottom, flags);
        return original();
    }
    HANDLE record = EncodePosition(nativeX, targetX);
    if (nativeX == targetX) {
        if (previous) {
            x = targetX;
            y = top;
            flags &= ~SWP_NOMOVE;
        }
        BOOL result = original();
        RECT actual{};
        if (result && GetWindowRect(window, &actual) && actual.left == nativeX) {
            RemoveProp(window, kPanelPosition);
            RemoveProp(window, kPanelTaskbar);
        }
        return result;
    }
    if (!SetProp(window, kPanelTaskbar, taskbar)) return original();
    if (!SetProp(window, kPanelPosition, record)) {
        if (previousTaskbar) SetProp(window, kPanelTaskbar, previousTaskbar);
        else RemoveProp(window, kPanelTaskbar);
        return original();
    }
    x = targetX;
    y = top;
    flags &= ~SWP_NOMOVE;
    g_panelBusy = true;
    BOOL result = original();
    g_panelBusy = false;
    RECT readback{};
    bool verified = result && GetWindowRect(window, &readback) && readback.left == targetX;
    Wh_Log(L"[UWPANEL] position hwnd=%p taskbar=%p nativeX=%d targetX=%d shift=%d flags=%x ok=%d verified=%d",
           window, taskbar, nativeX, targetX, DecodeShift(publication), flags, result, verified);
    if (!verified) {
        // A failed call retains the preceding owned position when it still matches.
        int oldNative{}, oldApplied{};
        DecodePosition(previous, oldNative, oldApplied);
        if (previous && GetWindowRect(window, &readback) && readback.left == oldApplied) {
            SetProp(window, kPanelPosition, previous);
            if (previousTaskbar) SetProp(window, kPanelTaskbar, previousTaskbar);
        } else {
            RemoveProp(window, kPanelPosition);
            RemoveProp(window, kPanelTaskbar);
        }
    } else {
        RemoveProp(window, kPanelPending);
    }
    if (verified && (!GetProp(taskbar, kTrayShift) || g_stopping)) {
        // Publication may be withdrawn while the native cross-process call runs.
        if (!GetProp(window, kPanelPosition)) {
            SetProp(window, kPanelTaskbar, taskbar);
            SetProp(window, kPanelPosition, record);
        }
        RestorePanel(window);
    }
    return result;
}

HRESULT WINAPI PanelDwmAttribute(HWND window, DWORD attribute, LPCVOID value, DWORD size) {
    ActiveScope active;
    if (!g_stopping && attribute == DWMWA_CLOAK && value && size == sizeof(BOOL) &&
        !*static_cast<const BOOL*>(value) && IsPanel(window)) {
        // Existing taskbar-vertical precedent: position BEFORE native uncloaking.
        Wh_Log(L"[UWPANEL] before-uncloak hwnd=%p", window);
        RECT rect{};
        if (GetWindowRect(window, &rect) &&
            (rect.right - rect.left <= 1 || rect.bottom - rect.top <= 1)) {
            SetProp(window, kPanelPending, reinterpret_cast<HANDLE>(1));
            Wh_Log(L"[UWPANEL] transient-uncloak hwnd=%p rect=%ld,%ld,%ld,%ld",
                   window, rect.left, rect.top, rect.right, rect.bottom);
        }
        PanelSetWindowPos(window, nullptr, 0, 0, 0, 0,
                          SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
    return DwmSetWindowAttribute_Original(window, attribute, value, size);
}

void CALLBACK PanelLocationEvent(HWINEVENTHOOK, DWORD event, HWND window,
                                 LONG object, LONG child, DWORD, DWORD) {
    if (g_stopping || event != EVENT_OBJECT_LOCATIONCHANGE || object != OBJID_WINDOW ||
        child != CHILDID_SELF || !window || !GetProp(window, kPanelPending)) return;
    RECT rect{};
    if (!IsPanel(window) || !GetWindowRect(window, &rect)) {
        RemoveProp(window, kPanelPending);
        return;
    }
    if (rect.right - rect.left <= 1 || rect.bottom - rect.top <= 1) return;
    Wh_Log(L"[UWPANEL] final-location hwnd=%p rect=%ld,%ld,%ld,%ld", window,
           rect.left, rect.top, rect.right, rect.bottom);
    PanelSetWindowPos(window, nullptr, 0, 0, 0, 0,
                      SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

BOOL WINAPI PanelDestroyWindow(HWND window) {
    ActiveScope active;
    // Property records belong to this HWND lifetime; never carry them to reuse.
    RemoveProp(window, kPanelPosition);
    RemoveProp(window, kPanelTaskbar);
    RemoveProp(window, kPanelPending);
    RemoveProp(window, kTrayDispatcher);
    if (RemoveProp(window, kTrayShift)) RestorePanels(window);
    return DestroyWindow_Original(window);
}

// ---- Dock geometry and the native-animation follower --------------------
struct Placement { double buttonsX; double trayX; };

// One measurement pass over the native layout, taken in Refresh and handed to
// the dock, follower and liquid stages. Keeping it together stops them
// disagreeing about geometry or re-reading it from the window mid-pass, and
// names what used to be eleven positional doubles.
struct Measurements {
    double host{};
    double origin{};
    double startLeft{};
    double trayLeft{};
    double trayWidth{};
    double buttonsWidth{};
    double shift{};
    double backgroundOrigin{};
    double dockHeight{};
    DockInsets insets{};

    double dockOrigin() const { return origin - backgroundOrigin; }
    double visibleDockHeight() const {
        return dockHeight - insets.top - insets.bottom;
    }
};

bool Plan(double host, double buttons, double tray, double nativeCenter,
          double nativeTrayLeft, double nativeStartDelta, Placement& result) {
    if (!std::isfinite(host) || !std::isfinite(buttons) ||
        !std::isfinite(tray) || !std::isfinite(nativeCenter) ||
        !std::isfinite(nativeTrayLeft) || !std::isfinite(nativeStartDelta) ||
        host <= 0 || buttons <= 0 || tray <= 0 || buttons + kGap + tray > host ||
        std::abs(nativeCenter - host / 2) > 1 || nativeStartDelta > 1 ||
        nativeCenter + nativeStartDelta - buttons / 2 < -1 ||
        nativeCenter + nativeStartDelta + buttons / 2 > host + 1) {
        return false;
    }
    double left = (host - buttons - kGap - tray) / 2;
    // buttonsX is the pure specification of the button shift; production routes
    // it through BoundaryThreshold's animated correction instead, but the check
    // harness asserts the 12-DIP gap and centering from the pair. Keep both.
    result = {left + buttons / 2 - nativeCenter - nativeStartDelta,
              left + buttons + kGap - nativeTrayLeft};
    return true;
}

struct OwnedTranslation {
    FrameworkElement element{nullptr};
    float3 original{};
    float3 applied{};
    bool installed{};
};
struct LiquidRenderer;
struct OwnedDock {
    FrameworkElement background{nullptr};
    Visual visual{nullptr};
    CompositionClip originalClip{nullptr};
    CompositionGeometricClip clip{nullptr};
    CompositionRoundedRectangleGeometry geometry{nullptr};
    Shapes::Rectangle fill{nullptr};
    Shapes::Rectangle stroke{nullptr};
    winrt::Windows::Foundation::IInspectable originalFill{nullptr};
    winrt::Windows::Foundation::IInspectable originalStroke{nullptr};
    Media::Brush material{nullptr};
    Media::SolidColorBrush transparentStroke{nullptr};
    ShapeVisual highlights{nullptr};
    Visual attachedVisual{nullptr};
    std::shared_ptr<LiquidRenderer> liquid;
    ElementTheme materialTheme{};
    bool materialBlocked{};
    ULONGLONG liquidRetryAt{};
    unsigned liquidFailures{};
};
struct Root {
    struct StatePlate {
        Controls::Border element{nullptr};
        winrt::Windows::Foundation::IInspectable originalOpacity{nullptr};
        double appliedOpacity{};
        bool opacityOwned{};
        bool opacityBlocked{};
        ShapeVisual rim{nullptr};
        ElementTheme rimTheme{};
        bool rimThemeReady{};
        bool rimDimmed{};
        Visual visual{nullptr};
        CompositionGeometricClip clip{nullptr};
        CompositionRoundedRectangleGeometry clipGeometry{nullptr};
        Media::CompositeTransform stretch{nullptr};
        bool blocked{};
    };
    FrameworkElement frame{nullptr};
    winrt::event_token layoutToken{};
    bool subscribed{};
    OwnedTranslation buttonsTranslation;
    OwnedTranslation trayTranslation;
    OwnedDock dock;
    bool blocked{};
    unsigned refreshFailures{};  // Consecutive retryable failures.
    bool fallbackReported{};
    HWND taskbar{};
    HANDLE publishedShift{};
    bool mappingReported{};
    struct Source {
        uintptr_t identity{};
        winrt::weak_ref<FrameworkElement> element;
        Visual visual{nullptr};
        enum class Life { Active, Hiding, Retired } life{};
        bool requestingAttention{};
        bool included{};
        Controls::Border plate{nullptr};
    };
    struct ForegroundState {
        winrt::weak_ref<Controls::TextBlock> element;
        uintptr_t identity{};
        winrt::Windows::Foundation::IInspectable original{nullptr};
        Media::SolidColorBrush brush{nullptr};
        animation::Storyboard transition{nullptr};
        bool owned{};
        bool light{};
        bool ready{};
    };
    std::vector<Source> sources;
    std::vector<StatePlate> statePlates;
    std::vector<ForegroundState> foregrounds;
    ULONGLONG foregroundSampleAt{};
    bool foregroundSamplePending{};
    std::vector<uintptr_t> expressionSources;
    ExpressionAnimation follower{nullptr};
    ExpressionAnimation buttonsFollower{nullptr};
    uintptr_t anchorIdentity{};
    float anchorPrefix{};
    CompositionPropertySet metrics{nullptr};
    double nativeRightOverhang{NAN};
    double hostWidth{};
    Measurements measurements{};
    bool measurementsReady{};
    bool retired{};
};
std::vector<Root> g_roots;  // Confined to the one observed taskbar UI thread.

// ---- Liquid glass: windowed capture and GPU refraction ------------------
struct LiquidRenderer : std::enable_shared_from_this<LiquidRenderer> {
    struct LensConstants {
        float imageSize[2]{};
        float center[2]{};
        float halfSize[2]{};
        float radius{};
        LiquidSettings optics;
        // Clip bounds, tint, native corner radii, native bounds, and the
        // remaining shape parameters are parallel arrays indexed by lensCount.
        // Both rounded rectangles are retained because the visible plate is
        // their intersection, not another rounded rectangle.
        float lensRects[kMaxButtonLenses][4]{};
        float lensTints[kMaxButtonLenses][4]{};
        float lensRadii[kMaxButtonLenses][4]{};
        float lensNativeRects[kMaxButtonLenses][4]{};
        float lensShapeParams[kMaxButtonLenses][4]{};
        uint32_t lensCount{};
        float lensRefraction{};
        float alertTintOpacity{};
        float lensBlend{};
    };
    static_assert(sizeof(LensConstants) % 16 == 0);
    // Pins the cbuffer arithmetic the HLSL side relies on: a 64-byte header,
    // five float4 arrays, then four trailing scalars. Adding a member without
    // accounting for it here would silently shift every lens the shader reads.
    static_assert(sizeof(LensConstants) ==
                  64 + 5 * 16 * kMaxButtonLenses + 16);

    struct CopyConstants {
        float offset[2]{};
        float scale[2]{};
    };
    static_assert(sizeof(CopyConstants) % 16 == 0);

    struct BlurConstants {
        float step[2]{};
        int radius{};
        float padding{};
    };
    static_assert(sizeof(BlurConstants) % 16 == 0);

    struct Source {
        HWND window{};
        RECT rect{};
        capture::GraphicsCaptureItem item{nullptr};
        capture::Direct3D11CaptureFramePool pool{nullptr};
        capture::GraphicsCaptureSession session{nullptr};
        winrt::event_token frameToken{};
        winrt::com_ptr<ID3D11Texture2D> latest;
        winrt::com_ptr<ID3D11ShaderResourceView> latestView;
        decltype(item.Size()) poolSize{};
        decltype(item.Size()) contentSize{};
        decltype(item.Size()) textureSize{};
        std::chrono::steady_clock::time_point lastFrame{};
        bool needsFullCopy{true};
        std::atomic<bool> active{};
        std::mutex callbackMutex;
    };

    static constexpr size_t kMaxAppSources = 7;
    static constexpr char kShader[] = R"(
Texture2D<float4> backdrop : register(t0);
Texture2D<float4> blurredBackdrop : register(t1);
SamplerState linearClamp : register(s0);
cbuffer Lens : register(b0) {
    float2 imageSize;
    float2 center;
    float2 halfSize;
    float radius;
    float zRadius;
    float refraction;
    float chroma;
    float edgeHighlight;
    float specular;
    float fresnel;
    float opacity;
    float brightness;
    float blurAmount;
    float4 lensRects[BUTTON_LENS_CAP];
    float4 lensTints[BUTTON_LENS_CAP];
    float4 lensRadii[BUTTON_LENS_CAP];
    float4 lensNativeRects[BUTTON_LENS_CAP];
    float4 lensShapeParams[BUTTON_LENS_CAP];
    uint lensCount;
    float lensRefraction;
    float alertTintOpacity;
    float lensBlend;
};
cbuffer Copy : register(b1) {
    float2 copyOffset;
    float2 copyScale;
};
cbuffer Blur : register(b2) {
    float2 blurStep;
    int blurRadius;
    float blurPadding;
};
struct VertexOutput { float4 position : SV_Position; float2 uv : TEXCOORD0; };
VertexOutput vs(uint id : SV_VertexID) {
    VertexOutput output;
    output.uv = float2((id << 1) & 2, id & 2);
    output.position = float4(output.uv * float2(2, -2) + float2(-1, 1), 0, 1);
    return output;
}
float4 copyPs(VertexOutput input) : SV_Target {
    return backdrop.SampleLevel(linearClamp,
        copyOffset + input.uv * copyScale, 0);
}
float4 boxBlurPs(VertexOutput input) : SV_Target {
    float4 color = 0;
    for (int offset = -blurRadius; offset <= blurRadius; ++offset)
        color += backdrop.SampleLevel(
            linearClamp, input.uv + blurStep * offset, 0);
    return color / (2 * blurRadius + 1);
}
float roundedDistanceFor(float2 p, float2 size, float cornerRadius) {
    float2 q = abs(p) - size + cornerRadius;
    return length(max(q, 0)) + min(max(q.x, q.y), 0) - cornerRadius;
}
float softAbs(float value, float softness) {
    float magnitude = abs(value);
    if (magnitude >= softness) return magnitude;
    return magnitude * magnitude / (2 * softness) + softness * 0.5;
}
float softMax(float a, float b, float softness) {
    float blend = saturate(0.5 + 0.5 * (a - b) / softness);
    return lerp(b, a, blend) + softness * blend * (1 - blend);
}
float opticalDistanceFor(float2 p, float2 size, float cornerRadius) {
    float softness = max(2, min(cornerRadius * 0.5,
                                min(size.x, size.y) * 0.25));
    float2 q = float2(softAbs(p.x, softness), softAbs(p.y, softness)) -
               size + cornerRadius;
    return length(max(q, 0)) + min(softMax(q.x, q.y, softness), 0) -
           cornerRadius;
}
float roundedDistance(float2 p) {
    return roundedDistanceFor(p, halfSize, radius);
}
float opticalDistance(float2 p) {
    return opticalDistanceFor(p, halfSize, radius);
}
float cornerValue(float2 p, float4 values) {
    return p.x < 0
        ? (p.y < 0 ? values.x : values.w)
        : (p.y < 0 ? values.y : values.z);
}
float roundedDistanceForCorners(float2 p, float2 size, float4 radii) {
    float cornerRadius = cornerValue(p, radii);
    return roundedDistanceFor(
        p, size, min(cornerRadius, min(size.x, size.y)));
}
float opticalDistanceForCorners(float2 p, float2 size, float4 radii) {
    float cornerRadius = cornerValue(p, radii);
    return opticalDistanceFor(
        p, size, min(cornerRadius, min(size.x, size.y)));
}
// Signed-distance approximation for an axis-aligned rounded rectangle whose
// corners may be elliptical. Its zero contour is exact; the radial conversion
// keeps distances in pixels so antialiasing and bevel depth remain stable.
float ellipticalDistanceForCorners(float2 p, float2 size, float4 radiiX,
                                   float4 radiiY) {
    float2 cornerRadius = min(float2(cornerValue(p, radiiX),
                                    cornerValue(p, radiiY)), size);
    float2 outer = abs(p) - size;
    float2 corner = outer + cornerRadius;
    if (cornerRadius.x > 0 && cornerRadius.y > 0 &&
        corner.x > 0 && corner.y > 0) {
        float normalized = length(corner / cornerRadius);
        return (normalized - 1) * length(corner) / max(normalized, 0.0001);
    }
    return length(max(outer, 0)) + min(max(outer.x, outer.y), 0);
}
float intersectedDistance(float2 pixel, float4 nativeRect,
                          float4 nativeRadiiX, float nativeYOverX,
                          float4 clipRect, float2 clipRadius) {
    float nativeDistance = ellipticalDistanceForCorners(
        pixel - nativeRect.xy, nativeRect.zw, nativeRadiiX,
        nativeRadiiX * nativeYOverX);
    float4 clipRadiiX = clipRadius.x;
    float4 clipRadiiY = clipRadius.y;
    float clipDistance = ellipticalDistanceForCorners(
        pixel - clipRect.xy, clipRect.zw, clipRadiiX, clipRadiiY);
    // Intersection SDF: the furthest outside (or least inside) constituent wins.
    return max(nativeDistance, clipDistance);
}
float bevelHeightFor(float inside, float depthRadius) {
    if (inside <= 0) return 0;
    if (inside >= depthRadius) return depthRadius;
    return sqrt(inside * (2 * depthRadius - inside));
}
float3 blurredColor(float2 pixel) {
    return blurredBackdrop.SampleLevel(linearClamp, pixel / imageSize, 0).rgb;
}
// One glass surface. The dock and every button are the same material at different
// scales, so they share this and differ only in the gains handed in.
struct Glass {
    float distance;
    float inside;
    float3 normal;
    float depth;
    float2 shift;
    float edge;
};
Glass glassAt(float2 p, float2 halfExtent, float4 radii, float depthRadius,
              float refractionAmount, float shiftGain, float radialGain,
              float edgeGain) {
    Glass g;
    g.distance = roundedDistanceForCorners(p, halfExtent, radii);
    g.inside = -g.distance;
    float e = 2;
    float opticalInside = -opticalDistanceForCorners(p, halfExtent, radii);
    float lensZ = min(depthRadius, min(halfExtent.x, halfExtent.y));
    float hCenter = bevelHeightFor(opticalInside, lensZ);
    float2 gradient = float2(
        bevelHeightFor(-opticalDistanceForCorners(p + float2(e, 0), halfExtent, radii), lensZ) -
            bevelHeightFor(-opticalDistanceForCorners(p - float2(e, 0), halfExtent, radii), lensZ),
        bevelHeightFor(-opticalDistanceForCorners(p + float2(0, e), halfExtent, radii), lensZ) -
            bevelHeightFor(-opticalDistanceForCorners(p - float2(0, e), halfExtent, radii), lensZ)) / (2 * e);
    g.normal = normalize(float3(-gradient, 1));
    g.depth = smoothstep(0, max(lensZ, 0.5), opticalInside);
    float refractivePower = 1 - 1 / 1.5;
    float normalizedThickness = hCenter / max(lensZ, 0.5);
    g.shift = gradient * refractivePower * (2 + normalizedThickness * 0.5) *
              refractionAmount * shiftGain;
    g.shift += -p / max(halfExtent, 1) * refractionAmount * radialGain * g.depth;
    g.edge = smoothstep(min(halfExtent.x, halfExtent.y) * edgeGain, 0, g.inside);
    return g;
}
// A button surface is the intersection of the native Border and the inset clip
// installed by PrepareStatePlate. Coverage and every normal sample use the same
// two shapes in output-pixel coordinates.
Glass glassAtIntersection(float2 pixel, float4 nativeRect,
                          float4 nativeRadiiX, float nativeYOverX,
                          float4 clipRect, float2 clipRadius,
                          float depthRadius, float refractionAmount,
                          float shiftGain, float radialGain, float edgeGain) {
    Glass g;
    g.distance = intersectedDistance(pixel, nativeRect, nativeRadiiX,
                                     nativeYOverX, clipRect, clipRadius);
    g.inside = -g.distance;
    float e = 2;
    // Coverage and all finite-difference samples use this same intersected
    // field, so smoothing cannot move the max() seam or invent inset coverage.
    float opticalInside = g.inside;
    float lensZ = min(depthRadius, min(clipRect.z, clipRect.w));
    float hCenter = bevelHeightFor(opticalInside, lensZ);
    float2 gradient = float2(
        bevelHeightFor(-intersectedDistance(pixel + float2(e, 0), nativeRect,
            nativeRadiiX, nativeYOverX, clipRect, clipRadius), lensZ) -
        bevelHeightFor(-intersectedDistance(pixel - float2(e, 0), nativeRect,
            nativeRadiiX, nativeYOverX, clipRect, clipRadius), lensZ),
        bevelHeightFor(-intersectedDistance(pixel + float2(0, e), nativeRect,
            nativeRadiiX, nativeYOverX, clipRect, clipRadius), lensZ) -
        bevelHeightFor(-intersectedDistance(pixel - float2(0, e), nativeRect,
            nativeRadiiX, nativeYOverX, clipRect, clipRadius), lensZ)) /
        (2 * e);
    g.normal = normalize(float3(-gradient, 1));
    g.depth = smoothstep(0, max(lensZ, 0.5), opticalInside);
    float refractivePower = 1 - 1 / 1.5;
    float normalizedThickness = hCenter / max(lensZ, 0.5);
    g.shift = gradient * refractivePower * (2 + normalizedThickness * 0.5) *
              refractionAmount * shiftGain;
    float2 p = pixel - clipRect.xy;
    g.shift += -p / max(clipRect.zw, 1) * refractionAmount * radialGain * g.depth;
    g.edge = smoothstep(min(clipRect.z, clipRect.w) * edgeGain, 0, g.inside);
    return g;
}
// Refracted, chromatically split and optionally blurred backdrop for one surface.
float3 refractedBackdrop(float2 pixel, Glass g) {
    float chromaPixels = chroma * 36 * (g.edge * 0.7 + 0.3);
    float2 chromaShift = g.normal.xy * chromaPixels;
    float2 samplePixel = pixel + g.shift;
    float3 sharp;
    sharp.r = backdrop.SampleLevel(linearClamp,
        (samplePixel + chromaShift) / imageSize, 0).r;
    sharp.g = backdrop.SampleLevel(linearClamp, samplePixel / imageSize, 0).g;
    sharp.b = backdrop.SampleLevel(linearClamp,
        (samplePixel - chromaShift) / imageSize, 0).b;
    if (blurAmount > 0.001)
        return lerp(sharp, blurredColor(samplePixel), 1 - g.edge * 0.15);
    return sharp;
}
// Specular, fresnel, rim and inner stroke for one surface.
float3 glassLighting(Glass g, float2 p, float2 halfExtent, out float fresnelAmount) {
    fresnelAmount = pow(1 - abs(g.normal.z), 4) * fresnel;
    float3 view = float3(0, 0, 1);
    float highlight = pow(saturate(dot(g.normal,
        normalize(normalize(float3(0.4, 0.7, 1)) + view))), 90);
    highlight += pow(saturate(dot(g.normal,
        normalize(normalize(float3(-0.3, -0.5, 1)) + view))), 50) * 0.3;
    float innerStroke = smoothstep(-2.5, -1.5, g.distance) *
                        (1 - smoothstep(-1, 0, g.distance));
    float topBias = 0.5 + 0.5 * (-p.y / halfExtent.y);
    innerStroke *= 0.4 + 0.6 * topBias;
    float rim = g.edge * edgeHighlight * 0.22;
    float innerGlow = smoothstep(5, 0, g.inside) * edgeHighlight * 0.15;
    float environment = (g.normal.y * 0.5 + 0.5) * fresnelAmount * 0.08;
    return highlight * specular + rim + innerGlow +
           innerStroke * edgeHighlight * 0.55 + environment;
}
float4 ps(VertexOutput input) : SV_Target {
    float2 pixel = input.position.xy;
    float2 p = pixel - center;
    float distance = roundedDistance(p);
    if (distance > 0) return 0;
    float4 original = backdrop.SampleLevel(linearClamp, pixel / imageSize, 0);
    if (original.a <= 0.001) return 0;

    // Each button is a glass surface over the shared dock. Buttons do not overlap,
    // so the first hit wins and the cheap distance rejects every other lens. The
    // dock remains beneath the lens so sampled button geometry can't expose the
    // raw backdrop during native motion.
    float coverage = 0;
    float3 buttonColor = 0;
    for (uint i = 0; i < min(lensCount, BUTTON_LENS_CAP); ++i) {
        float4 clipRect = lensRects[i];
        float4 nativeRect = lensNativeRects[i];
        float4 shapeParams = lensShapeParams[i];
        float distance = intersectedDistance(
            pixel, nativeRect, lensRadii[i], shapeParams.x,
            clipRect, shapeParams.yz);
        if (distance > 0.5) continue;
        Glass item = glassAtIntersection(
            pixel, nativeRect, lensRadii[i], shapeParams.x,
            clipRect, shapeParams.yz, zRadius * 0.65,
            lensRefraction, 18, 3, 0.45);
        float itemMask = 1 - smoothstep(-1.5, 0.5, item.distance);
        if (itemMask <= 0) continue;
        buttonColor = refractedBackdrop(pixel, item);
        buttonColor *= (1 + brightness) * (1 + 0.08 * item.depth);
        buttonColor = lerp(buttonColor, lensTints[i].rgb,
                           alertTintOpacity * lensTints[i].a);
        float itemFresnel;
        float2 itemPoint = pixel - clipRect.xy;
        buttonColor += glassLighting(item, itemPoint, clipRect.zw, itemFresnel);
        buttonColor = lerp(buttonColor, 1, itemFresnel * 0.2);
        coverage = itemMask * lensBlend;
        break;
    }

    // Biconvex pill profile adapted from ybouane/liquidglass (MIT):
    // https://github.com/ybouane/liquidglass. The curved height field creates
    // the reference's raised glass distortion across the whole surface. These are
    // the dock's gains; radius is already clamped to min(halfSize), so four equal
    // corners here match the single-radius distance functions this replaced.
    float3 color = buttonColor;
    if (coverage < 0.999) {
        Glass dock = glassAt(p, halfSize, float4(radius, radius, radius, radius),
                             zRadius, refraction, 30, 4, 0.35);
        float3 dockColor = refractedBackdrop(pixel, dock);
        dockColor *= (1 + brightness) * (1 + 0.06 * dock.depth);
        float fresnelAmount;
        dockColor += glassLighting(dock, p, halfSize, fresnelAmount);
        dockColor = lerp(dockColor, 1, fresnelAmount * 0.2);
        color = lerp(dockColor, buttonColor, coverage);
    }

    float mask = 1 - smoothstep(-1.5, 0.5, distance);
    float alpha = mask * opacity * original.a;
    return float4(color * alpha, alpha);
}
)";

    std::mutex mutex;
    bool running{};
    struct ShaderBytecode {
        winrt::com_ptr<ID3DBlob> vertex;
        winrt::com_ptr<ID3DBlob> pixel;
        winrt::com_ptr<ID3DBlob> copy;
        winrt::com_ptr<ID3DBlob> blur;
    };

    static ShaderBytecode const& GetShaderBytecode() {
        static std::mutex cacheMutex;
        static ShaderBytecode cache;
        std::lock_guard lock(cacheMutex);
        if (cache.vertex && cache.pixel && cache.copy && cache.blur)
            return cache;
        auto compile = [](const char* entry, const char* target) {
            winrt::com_ptr<ID3DBlob> code;
            D3D_SHADER_MACRO macros[]{
                {"BUTTON_LENS_CAP", UW_STRINGIZE(UW_BUTTON_LENS_CAP)},
                {nullptr, nullptr}};
            winrt::check_hresult(D3DCompile(
                kShader, sizeof(kShader) - 1, "liquid-glass", macros,
                nullptr, entry, target, D3DCOMPILE_ENABLE_STRICTNESS, 0,
                code.put(), nullptr));
            return code;
        };
        ShaderBytecode compiled;
        compiled.vertex = compile("vs", "vs_5_0");
        compiled.pixel = compile("ps", "ps_5_0");
        compiled.copy = compile("copyPs", "ps_5_0");
        compiled.blur = compile("boxBlurPs", "ps_5_0");
        cache = std::move(compiled);
        return cache;
    }

    bool geometryReady{};
    HWND taskbar{};
    RECT taskbarRect{};
    UINT width{};
    UINT height{};
    UINT blurWidth{};
    UINT blurHeight{};
    bool pendingPresent{};
    bool presentWorkPending{};
    LensConstants lens{};
    LensConstants motionBase{};
    LensConstants motionObserved{};
    LensConstants motionVelocity{};
    std::chrono::steady_clock::time_point motionSampleAt{};
    std::chrono::steady_clock::time_point motionObservationAt{};
    float motionSampleSeconds{1.0f / 60};
    bool motionActive{};
    bool motionObservationPending{};
    HRESULT failure{S_OK};
    std::chrono::steady_clock::time_point lastPresent{};
    PTP_WORK presentWork{};
    HANDLE frameLatencyWaitable{};
    struct PresentWorkContext {
        LiquidRenderer* renderer{};
    };
    PresentWorkContext* presentWorkContext{};
    std::vector<std::shared_ptr<Source>> sources;

    winrt::com_ptr<ID3D11Device> device;
    winrt::com_ptr<ID3D11DeviceContext> context;
    direct3d11::IDirect3DDevice graphicsDevice{nullptr};
    winrt::com_ptr<IDXGISwapChain1> swapChain;
    winrt::com_ptr<ID3D11Texture2D> composite;
    winrt::com_ptr<ID3D11RenderTargetView> compositeTarget;
    winrt::com_ptr<ID3D11Texture2D> backBuffer;
    winrt::com_ptr<ID3D11RenderTargetView> backTarget;
    winrt::com_ptr<ID3D11ShaderResourceView> compositeView;
    winrt::com_ptr<ID3D11Texture2D> blurTextures[2];
    winrt::com_ptr<ID3D11RenderTargetView> blurTargets[2];
    winrt::com_ptr<ID3D11ShaderResourceView> blurViews[2];
    winrt::com_ptr<ID3D11VertexShader> vertexShader;
    winrt::com_ptr<ID3D11PixelShader> pixelShader;
    winrt::com_ptr<ID3D11PixelShader> copyPixelShader;
    winrt::com_ptr<ID3D11PixelShader> boxBlurPixelShader;
    winrt::com_ptr<ID3D11Buffer> constants;
    winrt::com_ptr<ID3D11Buffer> copyConstants;
    winrt::com_ptr<ID3D11Buffer> blurConstants;
    winrt::com_ptr<ID3D11SamplerState> sampler;
    winrt::com_ptr<ID3D11BlendState> copyBlend;
    ContainerVisual visualRoot{nullptr};

    ~LiquidRenderer() { Stop(); }

    bool Initialize(Compositor const& compositor, HWND taskbarWindow,
                    UINT pixelWidth, UINT pixelHeight) {
        try {
            if (!taskbarWindow || !pixelWidth || !pixelHeight ||
                !capture::GraphicsCaptureSession::IsSupported()) return false;
            taskbar = taskbarWindow;
            UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
            D3D_FEATURE_LEVEL levels[]{D3D_FEATURE_LEVEL_11_1,
                                      D3D_FEATURE_LEVEL_11_0,
                                      D3D_FEATURE_LEVEL_10_1,
                                      D3D_FEATURE_LEVEL_10_0};
            D3D_FEATURE_LEVEL selected{};
            HRESULT hr = D3D11CreateDevice(
                nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, levels,
                ARRAYSIZE(levels), D3D11_SDK_VERSION, device.put(), &selected,
                context.put());
            if (FAILED(hr)) return false;
            auto dxgiDevice = device.as<IDXGIDevice>();
            winrt::com_ptr<IInspectable> inspectable;
            winrt::check_hresult(CreateDirect3D11DeviceFromDXGIDevice(
                dxgiDevice.get(), inspectable.put()));
            graphicsDevice = inspectable.as<direct3d11::IDirect3DDevice>();
            winrt::com_ptr<IDXGIAdapter> adapter;
            winrt::check_hresult(dxgiDevice->GetAdapter(adapter.put()));
            winrt::com_ptr<IDXGIFactory2> factory;
            winrt::check_hresult(adapter->GetParent(
                __uuidof(IDXGIFactory2), factory.put_void()));

            auto const& code = GetShaderBytecode();
            winrt::check_hresult(device->CreateVertexShader(
                code.vertex->GetBufferPointer(), code.vertex->GetBufferSize(),
                nullptr, vertexShader.put()));
            winrt::check_hresult(device->CreatePixelShader(
                code.pixel->GetBufferPointer(), code.pixel->GetBufferSize(),
                nullptr, pixelShader.put()));
            winrt::check_hresult(device->CreatePixelShader(
                code.copy->GetBufferPointer(), code.copy->GetBufferSize(),
                nullptr, copyPixelShader.put()));
            winrt::check_hresult(device->CreatePixelShader(
                code.blur->GetBufferPointer(), code.blur->GetBufferSize(),
                nullptr, boxBlurPixelShader.put()));
            D3D11_BUFFER_DESC constantDescription{};
            constantDescription.ByteWidth = sizeof(LensConstants);
            constantDescription.Usage = D3D11_USAGE_DEFAULT;
            constantDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            winrt::check_hresult(device->CreateBuffer(
                &constantDescription, nullptr, constants.put()));
            constantDescription.ByteWidth = sizeof(CopyConstants);
            winrt::check_hresult(device->CreateBuffer(
                &constantDescription, nullptr, copyConstants.put()));
            constantDescription.ByteWidth = sizeof(BlurConstants);
            winrt::check_hresult(device->CreateBuffer(
                &constantDescription, nullptr, blurConstants.put()));
            D3D11_SAMPLER_DESC samplerDescription{};
            samplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            samplerDescription.AddressU = samplerDescription.AddressV =
                samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;
            winrt::check_hresult(device->CreateSamplerState(
                &samplerDescription, sampler.put()));
            D3D11_BLEND_DESC blendDescription{};
            auto& blend = blendDescription.RenderTarget[0];
            blend.BlendEnable = TRUE;
            blend.SrcBlend = blend.SrcBlendAlpha = D3D11_BLEND_ONE;
            blend.DestBlend = blend.DestBlendAlpha =
                D3D11_BLEND_INV_SRC_ALPHA;
            blend.BlendOp = blend.BlendOpAlpha = D3D11_BLEND_OP_ADD;
            blend.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            winrt::check_hresult(device->CreateBlendState(
                &blendDescription, copyBlend.put()));

            DXGI_SWAP_CHAIN_DESC1 swapDescription{};
            swapDescription.Width = pixelWidth;
            swapDescription.Height = pixelHeight;
            swapDescription.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            swapDescription.SampleDesc.Count = 1;
            swapDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapDescription.BufferCount = 2;
            swapDescription.Scaling = DXGI_SCALING_STRETCH;
            swapDescription.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
            swapDescription.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
            swapDescription.Flags =
                DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
            winrt::check_hresult(factory->CreateSwapChainForComposition(
                device.get(), &swapDescription, nullptr, swapChain.put()));
            auto pacedSwapChain = swapChain.as<IDXGISwapChain2>();
            winrt::check_hresult(pacedSwapChain->SetMaximumFrameLatency(1));
            frameLatencyWaitable =
                pacedSwapChain->GetFrameLatencyWaitableObject();
            if (!frameLatencyWaitable)
                throw winrt::hresult_error(E_FAIL);
            winrt::com_ptr<ICompositorInterop> compositorInterop;
            winrt::check_hresult(
                reinterpret_cast<IUnknown*>(winrt::get_abi(compositor))
                    ->QueryInterface(kIID_ICompositorInterop,
                                     compositorInterop.put_void()));
            winrt::com_ptr<IUnknown> abiSurface;
            winrt::check_hresult(
                compositorInterop->CreateCompositionSurfaceForSwapChain(
                    swapChain.get(), abiSurface.put()));
            ICompositionSurface surface{
                abiSurface.detach(), winrt::take_ownership_from_abi};
            auto surfaceBrush = compositor.CreateSurfaceBrush(surface);
            surfaceBrush.Stretch(CompositionStretch::Fill);
            auto pixels = compositor.CreateSpriteVisual();
            pixels.RelativeSizeAdjustment({1, 1});
            pixels.Brush(surfaceBrush);
            visualRoot = compositor.CreateContainerVisual();
            visualRoot.RelativeSizeAdjustment({1, 1});
            visualRoot.Children().InsertAtBottom(pixels);
            if (!ResizeLocked(pixelWidth, pixelHeight)) return false;
            presentWorkContext = new PresentWorkContext{this};
            presentWork = CreateThreadpoolWork(
                [](PTP_CALLBACK_INSTANCE, void* raw, PTP_WORK) {
                    ActiveScope active;
                    auto renderer =
                        static_cast<PresentWorkContext*>(raw)->renderer;
                    HANDLE waitable{};
                    {
                        std::lock_guard lock(renderer->mutex);
                        if (renderer->running && !FAILED(renderer->failure) &&
                            renderer->geometryReady &&
                            renderer->pendingPresent)
                            waitable = renderer->frameLatencyWaitable;
                    }
                    DWORD waited = waitable
                                       ? WaitForSingleObject(waitable, 100)
                                       : WAIT_FAILED;
                    std::lock_guard lock(renderer->mutex);
                    if (!waitable || waited == WAIT_FAILED) {
                        DWORD error = GetLastError();
                        renderer->failure =
                            error ? HRESULT_FROM_WIN32(error) : E_FAIL;
                    } else if (renderer->running &&
                               !FAILED(renderer->failure) &&
                               renderer->geometryReady &&
                               renderer->pendingPresent) {
                        renderer->RenderLocked();
                        renderer->lastPresent =
                            std::chrono::steady_clock::now();
                    }
                    renderer->presentWorkPending = false;
                    if (renderer->motionActive)
                        renderer->SchedulePresentLocked();
                }, presentWorkContext, nullptr);
            if (!presentWork) {
                delete presentWorkContext;
                presentWorkContext = nullptr;
                winrt::throw_last_error();
            }
            running = true;
            Wh_Log(L"[UWLIQUID] renderer initialized taskbar=%p size=%ux%u feature=%x",
                   taskbar, width, height, unsigned(selected));
            return true;
        } catch (...) {
            Wh_Log(L"[UWLIQUID] renderer init failed hr=%08X",
                   winrt::to_hresult().value);
            return false;
        }
    }

    bool ResizeLocked(UINT pixelWidth, UINT pixelHeight) {
        if (!pixelWidth || !pixelHeight) return false;
        if (width == pixelWidth && height == pixelHeight && composite) return true;
        ID3D11RenderTargetView* noTarget{};
        context->OMSetRenderTargets(1, &noTarget, nullptr);
        backTarget = nullptr;
        backBuffer = nullptr;
        compositeView = nullptr;
        compositeTarget = nullptr;
        composite = nullptr;
        blurWidth = 0;
        blurHeight = 0;
        for (int i = 0; i < 2; ++i) {
            blurViews[i] = nullptr;
            blurTargets[i] = nullptr;
            blurTextures[i] = nullptr;
        }
        if (width && height) {
            context->Flush();
            HRESULT hr = swapChain->ResizeBuffers(
                2, pixelWidth, pixelHeight, DXGI_FORMAT_B8G8R8A8_UNORM,
                DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
            if (FAILED(hr)) return false;
        }
        D3D11_TEXTURE2D_DESC description{};
        description.Width = pixelWidth;
        description.Height = pixelHeight;
        description.MipLevels = 1;
        description.ArraySize = 1;
        description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        description.SampleDesc.Count = 1;
        description.Usage = D3D11_USAGE_DEFAULT;
        description.BindFlags = D3D11_BIND_RENDER_TARGET |
                                D3D11_BIND_SHADER_RESOURCE;
        winrt::com_ptr<ID3D11Texture2D> newComposite;
        winrt::com_ptr<ID3D11RenderTargetView> newCompositeTarget;
        winrt::com_ptr<ID3D11ShaderResourceView> newCompositeView;
        if (FAILED(device->CreateTexture2D(
                &description, nullptr, newComposite.put())) ||
            FAILED(device->CreateRenderTargetView(
                newComposite.get(), nullptr, newCompositeTarget.put())) ||
            FAILED(device->CreateShaderResourceView(
                newComposite.get(), nullptr, newCompositeView.put())))
            return false;
        description.Width = std::max(1u, pixelWidth / 2);
        description.Height = std::max(1u, pixelHeight / 2);
        winrt::com_ptr<ID3D11Texture2D> newBlurTextures[2];
        winrt::com_ptr<ID3D11RenderTargetView> newBlurTargets[2];
        winrt::com_ptr<ID3D11ShaderResourceView> newBlurViews[2];
        for (int i = 0; i < 2; ++i) {
            if (FAILED(device->CreateTexture2D(
                    &description, nullptr, newBlurTextures[i].put())) ||
                FAILED(device->CreateRenderTargetView(
                    newBlurTextures[i].get(), nullptr,
                    newBlurTargets[i].put())) ||
                FAILED(device->CreateShaderResourceView(
                    newBlurTextures[i].get(), nullptr, newBlurViews[i].put())))
                return false;
        }
        composite = std::move(newComposite);
        compositeTarget = std::move(newCompositeTarget);
        compositeView = std::move(newCompositeView);
        for (int i = 0; i < 2; ++i) {
            blurTextures[i] = std::move(newBlurTextures[i]);
            blurTargets[i] = std::move(newBlurTargets[i]);
            blurViews[i] = std::move(newBlurViews[i]);
        }
        blurWidth = description.Width;
        blurHeight = description.Height;
        width = pixelWidth;
        height = pixelHeight;
        return true;
    }

    // imageSize is filled in by RenderLocked, so the caller's copy always
    // differs from the stored one; compare everything else.
    static bool SameLensInput(LensConstants const& a, LensConstants const& b) {
        constexpr size_t offset = sizeof(a.imageSize);
        return memcmp(reinterpret_cast<char const*>(&a) + offset,
                      reinterpret_cast<char const*>(&b) + offset,
                      sizeof(a) - offset) == 0;
    }

    static bool SameMotionGeometry(LensConstants const& a,
                                   LensConstants const& b) {
        if (a.lensCount != b.lensCount || a.radius != b.radius)
            return false;
        for (int axis = 0; axis < 2; ++axis)
            if (a.center[axis] != b.center[axis] ||
                a.halfSize[axis] != b.halfSize[axis])
                return false;
        for (uint32_t i = 0; i < a.lensCount; ++i) {
            for (int component = 0; component < 4; ++component)
                if (a.lensRects[i][component] != b.lensRects[i][component] ||
                    a.lensRadii[i][component] != b.lensRadii[i][component] ||
                    a.lensNativeRects[i][component] !=
                        b.lensNativeRects[i][component])
                    return false;
            for (int component = 0; component < 3; ++component)
                if (a.lensShapeParams[i][component] !=
                    b.lensShapeParams[i][component])
                    return false;
        }
        return true;
    }

    static void PredictMotionGeometry(LensConstants& output,
                                      LensConstants const& base,
                                      LensConstants const& velocity,
                                      float seconds) {
        auto predict = [seconds](float value, float speed) {
            return value + speed * seconds;
        };
        output.radius = std::max(0.0f, predict(base.radius, velocity.radius));
        for (int axis = 0; axis < 2; ++axis) {
            output.center[axis] =
                predict(base.center[axis], velocity.center[axis]);
            output.halfSize[axis] = std::max(
                1.0f, predict(base.halfSize[axis], velocity.halfSize[axis]));
        }
        for (uint32_t i = 0; i < output.lensCount; ++i) {
            for (int component = 0; component < 4; ++component) {
                output.lensRects[i][component] = predict(
                    base.lensRects[i][component],
                    velocity.lensRects[i][component]);
                output.lensRadii[i][component] = std::max(
                    0.0f, predict(base.lensRadii[i][component],
                                  velocity.lensRadii[i][component]));
                output.lensNativeRects[i][component] = predict(
                    base.lensNativeRects[i][component],
                    velocity.lensNativeRects[i][component]);
            }
            output.lensRects[i][2] = std::max(1.0f, output.lensRects[i][2]);
            output.lensRects[i][3] = std::max(1.0f, output.lensRects[i][3]);
            output.lensNativeRects[i][2] =
                std::max(1.0f, output.lensNativeRects[i][2]);
            output.lensNativeRects[i][3] =
                std::max(1.0f, output.lensNativeRects[i][3]);
            for (int component = 0; component < 3; ++component)
                output.lensShapeParams[i][component] = predict(
                    base.lensShapeParams[i][component],
                    velocity.lensShapeParams[i][component]);
            output.lensShapeParams[i][0] =
                std::max(0.01f, output.lensShapeParams[i][0]);
        }
    }

    LensConstants PredictedLensLocked(
        std::chrono::steady_clock::time_point now) const {
        LensConstants result = lens;
        if (!motionActive || motionBase.lensCount != lens.lensCount ||
            motionSampleAt.time_since_epoch().count() == 0)
            return result;
        float horizon = std::clamp(
            motionSampleSeconds * 1.25f, 1.0f / 120, 0.025f);
        float seconds = std::clamp(
            std::chrono::duration<float>(now - motionSampleAt).count(),
            0.0f, horizon);
        PredictMotionGeometry(result, motionBase, motionVelocity, seconds);
        return result;
    }

    void ObserveMotionLocked(
        LensConstants const& newLens,
        std::chrono::steady_clock::time_point now) {
        float elapsed =
            std::chrono::duration<float>(now - motionSampleAt).count();
        if (motionBase.lensCount != newLens.lensCount || elapsed <= 0.001f ||
            elapsed > 0.1f) {
            motionBase = newLens;
            motionVelocity = {};
        } else {
            auto current = PredictedLensLocked(now);
            float sampleSeconds =
                std::clamp(elapsed, 1.0f / 240, 1.0f / 30);
            float correctionSeconds =
                std::clamp(elapsed, 1.0f / 240, 0.02f);
            auto previousVelocity = motionVelocity;
            auto velocity = [sampleSeconds, correctionSeconds](
                                float previous, float next, float displayed,
                                float previousSpeed) {
                float observed = (next - previous) / sampleSeconds;
                float correction = (next - displayed) / correctionSeconds;
                float correctionLimit =
                    std::max(std::abs(observed), std::abs(previousSpeed) * 0.5f);
                correction =
                    std::clamp(correction, -correctionLimit, correctionLimit);
                float result = observed + correction;
                if (observed > 0) return std::max(0.0f, result);
                if (observed < 0) return std::min(0.0f, result);
                return result;
            };
            motionVelocity = {};
            motionVelocity.lensCount = newLens.lensCount;
            motionVelocity.radius =
                velocity(motionObserved.radius, newLens.radius, current.radius,
                         previousVelocity.radius);
            for (int axis = 0; axis < 2; ++axis) {
                motionVelocity.center[axis] =
                    velocity(motionObserved.center[axis], newLens.center[axis],
                             current.center[axis],
                             previousVelocity.center[axis]);
                motionVelocity.halfSize[axis] =
                    velocity(motionObserved.halfSize[axis],
                             newLens.halfSize[axis],
                             current.halfSize[axis],
                             previousVelocity.halfSize[axis]);
            }
            for (uint32_t i = 0; i < newLens.lensCount; ++i) {
                for (int component = 0; component < 4; ++component) {
                    motionVelocity.lensRects[i][component] = velocity(
                        motionObserved.lensRects[i][component],
                        newLens.lensRects[i][component],
                        current.lensRects[i][component],
                        previousVelocity.lensRects[i][component]);
                    motionVelocity.lensRadii[i][component] = velocity(
                        motionObserved.lensRadii[i][component],
                        newLens.lensRadii[i][component],
                        current.lensRadii[i][component],
                        previousVelocity.lensRadii[i][component]);
                    motionVelocity.lensNativeRects[i][component] = velocity(
                        motionObserved.lensNativeRects[i][component],
                        newLens.lensNativeRects[i][component],
                        current.lensNativeRects[i][component],
                        previousVelocity.lensNativeRects[i][component]);
                }
                for (int component = 0; component < 3; ++component)
                    motionVelocity.lensShapeParams[i][component] = velocity(
                        motionObserved.lensShapeParams[i][component],
                        newLens.lensShapeParams[i][component],
                        current.lensShapeParams[i][component],
                        previousVelocity.lensShapeParams[i][component]);
            }
            motionBase = current;
            motionBase.lensCount = newLens.lensCount;
            motionSampleSeconds = sampleSeconds;
        }
        motionSampleAt = now;
    }

    void CommitMotionObservationLocked(
        std::chrono::steady_clock::time_point now) {
        if (!motionObservationPending ||
            now - motionObservationAt < std::chrono::milliseconds(1))
            return;
        ObserveMotionLocked(lens, motionObservationAt);
        motionObserved = lens;
        motionObservationPending = false;
    }

    bool UpdateGeometry(RECT const& taskbarBounds, UINT pixelWidth,
                        UINT pixelHeight, LensConstants const& newLens) {
        std::lock_guard lock(mutex);
        if (!running || FAILED(failure) ||
            !ResizeLocked(pixelWidth, pixelHeight))
            return false;
        auto now = std::chrono::steady_clock::now();
        bool changed = !geometryReady ||
                       !EqualRect(&taskbarRect, &taskbarBounds) ||
                       !SameLensInput(lens, newLens);
        bool motionChanged =
            !geometryReady || !SameMotionGeometry(lens, newLens);
        if (motionActive && motionChanged) {
            if (geometryReady) {
                motionObservationAt = now;
                motionObservationPending = true;
            } else {
                motionBase = newLens;
                motionObserved = newLens;
                motionVelocity = {};
                motionSampleAt = now;
            }
        }
        taskbarRect = taskbarBounds;
        lens = newLens;
        geometryReady = true;
        if (changed || pendingPresent) SchedulePresentLocked();
        return true;
    }

    void SetMotionActive(bool active) noexcept {
        std::lock_guard lock(mutex);
        if (motionActive == active) return;
        motionActive = active;
        motionVelocity = {};
        motionSampleSeconds = 1.0f / 60;
        motionSampleAt = active ? std::chrono::steady_clock::now()
                                : std::chrono::steady_clock::time_point{};
        motionObservationPending = false;
        if (geometryReady) {
            motionBase = lens;
            motionObserved = lens;
            SchedulePresentLocked();
        }
    }

    // Producers only mark the latest state dirty. One zero-delay worker owns
    // presentation, so updates coalesce without imposing a refresh-rate cap.
    void SchedulePresentLocked() {
        pendingPresent = true;
        if (!running || FAILED(failure) || !geometryReady ||
            !presentWork || presentWorkPending)
            return;
        presentWorkPending = true;
        SubmitThreadpoolWork(presentWork);
    }

    HRESULT Failure() {
        std::lock_guard lock(mutex);
        return failure;
    }

    std::vector<std::pair<HWND, RECT>> CandidateWindows() {
        RECT dockStrip{};
        {
            std::lock_guard lock(mutex);
            dockStrip = taskbarRect;
        }
        std::vector<std::pair<HWND, RECT>> result;
        bool belowTaskbar = false;
        HWND window = GetTopWindow(nullptr);
        for (int scanned = 0; window && scanned < 4096;
             window = GetWindow(window, GW_HWNDNEXT), ++scanned) {
            if (window == taskbar) {
                belowTaskbar = true;
                continue;
            }
            if (!belowTaskbar || !IsWindowVisible(window) || IsIconic(window) ||
                ClassIs(window, L"Shell_TrayWnd") ||
                ClassIs(window, L"Shell_SecondaryTrayWnd") ||
                ClassIs(window, L"Progman") ||
                ClassIs(window, L"WorkerW")) continue;
            DWORD cloaked{};
            if (SUCCEEDED(DwmGetWindowAttribute(
                    window, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked)
                continue;
            RECT rect{}, intersection{};
            if (!GetWindowRect(window, &rect) ||
                !IntersectRect(&intersection, &rect, &dockStrip)) continue;
            result.push_back({window, rect});
            if (result.size() == kMaxAppSources) break;
        }
        HWND desktop = GetShellWindow();
        RECT desktopRect{};
        if (desktop && GetWindowRect(desktop, &desktopRect))
            result.push_back({desktop, desktopRect});
        return result;
    }

    std::shared_ptr<Source> CreateSource(HWND window, RECT const& rect) {
        try {
            auto activationFactory =
                winrt::get_activation_factory<capture::GraphicsCaptureItem>();
            winrt::com_ptr<IGraphicsCaptureItemInterop> interop;
            winrt::check_hresult(
                reinterpret_cast<IUnknown*>(winrt::get_abi(activationFactory))
                    ->QueryInterface(kIID_IGraphicsCaptureItemInterop,
                                     interop.put_void()));
            void* rawItem{};
            winrt::check_hresult(interop->CreateForWindow(
                window, kIID_IGraphicsCaptureItem, &rawItem));
            auto source = std::make_shared<Source>();
            source->window = window;
            source->rect = rect;
            source->item = capture::GraphicsCaptureItem{
                rawItem, winrt::take_ownership_from_abi};
            source->poolSize = source->item.Size();
            if (source->poolSize.Width <= 0 || source->poolSize.Height <= 0)
                return nullptr;
            source->pool =
                capture::Direct3D11CaptureFramePool::CreateFreeThreaded(
                    graphicsDevice,
                    directx::DirectXPixelFormat::B8G8R8A8UIntNormalized,
                    2, source->poolSize);
            source->session = source->pool.CreateCaptureSession(source->item);
            std::weak_ptr<LiquidRenderer> weakRenderer = shared_from_this();
            std::weak_ptr<Source> weakSource = source;
            source->frameToken = source->pool.FrameArrived(
                [weakRenderer, weakSource](
                    capture::Direct3D11CaptureFramePool const& sender,
                    winrt::Windows::Foundation::IInspectable const&) {
                    ActiveScope active;
                    if (g_stopping) return;
                    auto source = weakSource.lock();
                    auto renderer = weakRenderer.lock();
                    if (!source || !renderer) return;
                    {
                        std::lock_guard callbackLock(source->callbackMutex);
                        if (!source->active.load(std::memory_order_acquire)) return;
                        renderer->OnFrame(source, sender);
                    }
                });
            try { source->session.IsBorderRequired(false); } catch (...) {}
            try { source->session.IsCursorCaptureEnabled(false); } catch (...) {}
            return source;
        } catch (...) {
            Wh_Log(L"[UWLIQUID] capture prepare failed hwnd=%p hr=%08X", window,
                   winrt::to_hresult().value);
            return nullptr;
        }
    }

    static void CloseSource(std::shared_ptr<Source> const& source) {
        if (!source) return;
        source->active.store(false, std::memory_order_release);
        // Drain an in-flight copy, but do not hold this lock while WinRT closes
        // the pool: a queued callback must be able to enter and observe inactive.
        { std::lock_guard callbackLock(source->callbackMutex); }
        // A source may be closed twice (explicit teardown, then scene
        // removal); WinRT closing an already-closed pool is not safe to repeat.
        auto session = std::exchange(source->session, nullptr);
        auto pool = std::exchange(source->pool, nullptr);
        try {
            if (pool) pool.FrameArrived(source->frameToken);
        } catch (...) {}
        try { if (session) session.Close(); } catch (...) {}
        try { if (pool) pool.Close(); } catch (...) {}
    }

    bool StartSource(std::shared_ptr<Source> const& source) {
        try {
            source->session.StartCapture();
            Wh_Log(L"[UWLIQUID] capture started hwnd=%p size=%dx%d",
                   source->window, source->poolSize.Width,
                   source->poolSize.Height);
            return true;
        } catch (...) {
            Wh_Log(L"[UWLIQUID] capture start failed hwnd=%p hr=%08X",
                   source->window, winrt::to_hresult().value);
            return false;
        }
    }

    void UpdateSources() { UpdateSources(CandidateWindows()); }

    void UpdateSources(
        std::vector<std::pair<HWND, RECT>> const& candidates) {
        std::vector<std::shared_ptr<Source>> current, stale;
        {
            std::lock_guard lock(mutex);
            if (!running) return;
            current = sources;
            // A frame can report a grown ContentSize on a surface still sized
            // for the old pool; Recreate only helps if another frame follows.
            // A static window sends none, so recycle the session instead.
            auto now = std::chrono::steady_clock::now();
            for (auto const& source : sources)
                if (source->latest &&
                    (source->textureSize.Width != source->contentSize.Width ||
                     source->textureSize.Height != source->contentSize.Height) &&
                    now - source->lastFrame > std::chrono::milliseconds(100))
                    stale.push_back(source);
        }
        std::vector<std::shared_ptr<Source>> next;
        std::vector<std::shared_ptr<Source>> added;
        std::vector<RECT> nextRects;
        for (auto const& [window, rect] : candidates) {
            auto found = std::find_if(current.begin(), current.end(),
                [&](auto const& source) { return source->window == window; });
            if (found != current.end() &&
                std::find(stale.begin(), stale.end(), *found) == stale.end()) {
                next.push_back(*found);
                nextRects.push_back(rect);
            } else if (auto source = CreateSource(window, rect)) {
                next.push_back(source);
                nextRects.push_back(rect);
                added.push_back(std::move(source));
            }
        }

        std::vector<std::shared_ptr<Source>> removed;
        bool committed{};
        {
            std::lock_guard lock(mutex);
            if (running && sources == current) {
                bool changed = sources.size() != next.size();
                for (size_t i = 0; i < next.size(); ++i) {
                    if (i >= sources.size() || sources[i] != next[i] ||
                        !EqualRect(&next[i]->rect, &nextRects[i]))
                        changed = true;
                    next[i]->rect = nextRects[i];
                }
                for (auto const& source : sources)
                    if (std::find(next.begin(), next.end(), source) == next.end())
                        removed.push_back(source);
                sources = next;
                for (auto const& source : added)
                    source->active.store(true, std::memory_order_release);
                if (changed) SchedulePresentLocked();
                committed = true;
            }
        }
        if (!committed) {
            for (auto const& source : added) CloseSource(source);
            return;
        }
        for (auto const& source : removed) CloseSource(source);
        for (auto const& source : added) {
            if (StartSource(source)) continue;
            source->active.store(false, std::memory_order_release);
            {
                std::lock_guard lock(mutex);
                auto found = std::find(sources.begin(), sources.end(), source);
                if (found != sources.end()) {
                    sources.erase(found);
                    SchedulePresentLocked();
                }
            }
            CloseSource(source);
        }
    }

    void OnFrame(std::shared_ptr<Source> const& source,
                 capture::Direct3D11CaptureFramePool const& sender) {
        capture::Direct3D11CaptureFrame frame{nullptr};
        decltype(source->poolSize) recreateSize{};
        bool recreatePool{};
        struct FrameCloser {
            capture::Direct3D11CaptureFrame& frame;
            ~FrameCloser() { try { if (frame) frame.Close(); } catch (...) {} }
        } frameCloser{frame};
        try {
            frame = sender.TryGetNextFrame();
            if (!frame) return;
            auto contentSize = frame.ContentSize();
            if (contentSize.Width <= 0 || contentSize.Height <= 0) return;
            auto frameSurface = frame.Surface();
            winrt::com_ptr<IDirect3DDxgiInterfaceAccess> access;
            winrt::check_hresult(
                reinterpret_cast<IUnknown*>(winrt::get_abi(frameSurface))
                    ->QueryInterface(kIID_IDirect3DDxgiInterfaceAccess,
                                     access.put_void()));
            winrt::com_ptr<ID3D11Texture2D> texture;
            winrt::check_hresult(access->GetInterface(
                __uuidof(ID3D11Texture2D), texture.put_void()));
            D3D11_TEXTURE2D_DESC captured{};
            texture->GetDesc(&captured);
            UINT validWidth = std::min<UINT>(contentSize.Width, captured.Width);
            UINT validHeight = std::min<UINT>(contentSize.Height, captured.Height);
            if (!validWidth || !validHeight) return;

            RECT currentRect{};
            bool haveRect = GetWindowRect(source->window, &currentRect);
            std::lock_guard lock(mutex);
            if (!running ||
                !source->active.load(std::memory_order_acquire) ||
                std::find(sources.begin(), sources.end(), source) == sources.end())
                return;

            D3D11_TEXTURE2D_DESC latest{};
            if (source->latest) source->latest->GetDesc(&latest);
            if (!source->latest || !source->latestView ||
                latest.Width != validWidth || latest.Height != validHeight ||
                latest.Format != captured.Format) {
                D3D11_TEXTURE2D_DESC owned = captured;
                owned.Width = validWidth;
                owned.Height = validHeight;
                owned.Usage = D3D11_USAGE_DEFAULT;
                owned.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                owned.CPUAccessFlags = 0;
                owned.MiscFlags = 0;
                owned.MipLevels = 1;
                owned.ArraySize = 1;
                winrt::com_ptr<ID3D11Texture2D> newTexture;
                winrt::com_ptr<ID3D11ShaderResourceView> newView;
                winrt::check_hresult(device->CreateTexture2D(
                    &owned, nullptr, newTexture.put()));
                winrt::check_hresult(device->CreateShaderResourceView(
                    newTexture.get(), nullptr, newView.put()));
                source->latest = std::move(newTexture);
                source->latestView = std::move(newView);
                source->needsFullCopy = true;
            }
            bool copied = false;
            bool resized = source->contentSize.Width != contentSize.Width ||
                           source->contentSize.Height != contentSize.Height;
            auto dirty = frame.try_as<capture::IDirect3D11CaptureFrame2>();
            if (!source->needsFullCopy && !resized && dirty &&
                dirty.DirtyRegionMode() ==
                    capture::GraphicsCaptureDirtyRegionMode::ReportOnly) {
                // Keep a complete snapshot, but update only changed pixels.
                // Cropping to today's taskbar strip would lose content exposed
                // by moving an otherwise static window without another frame.
                for (auto const& region : dirty.DirtyRegions()) {
                    LONG left = std::clamp<LONG>(region.X, 0, validWidth);
                    LONG top = std::clamp<LONG>(region.Y, 0, validHeight);
                    LONG right = static_cast<LONG>(std::clamp<int64_t>(
                        int64_t(region.X) + region.Width, 0, validWidth));
                    LONG bottom = static_cast<LONG>(std::clamp<int64_t>(
                        int64_t(region.Y) + region.Height, 0, validHeight));
                    if (left >= right || top >= bottom) continue;
                    D3D11_BOX box{static_cast<UINT>(left), static_cast<UINT>(top), 0,
                                 static_cast<UINT>(right), static_cast<UINT>(bottom), 1};
                    context->CopySubresourceRegion(
                        source->latest.get(), 0, left, top, 0, texture.get(), 0, &box);
                    copied = true;
                }
            } else {
                D3D11_BOX box{0, 0, 0, validWidth, validHeight, 1};
                context->CopySubresourceRegion(
                    source->latest.get(), 0, 0, 0, 0, texture.get(), 0, &box);
                copied = true;
            }
            source->needsFullCopy = false;
            source->contentSize = contentSize;
            source->textureSize = {static_cast<int32_t>(validWidth),
                                   static_cast<int32_t>(validHeight)};
            source->lastFrame = std::chrono::steady_clock::now();
            bool moved = haveRect && !EqualRect(&source->rect, &currentRect);
            if (haveRect) source->rect = currentRect;
            if (source->poolSize.Width != contentSize.Width ||
                source->poolSize.Height != contentSize.Height) {
                recreateSize = contentSize;
                recreatePool = true;
            }
            if (copied || moved) SchedulePresentLocked();
        } catch (...) {
            HRESULT error = winrt::to_hresult().value;
            HRESULT removed = device ? device->GetDeviceRemovedReason() : S_OK;
            {
                std::lock_guard lock(mutex);
                source->needsFullCopy = true;
                if (FAILED(removed)) failure = removed;
            }
            Wh_Log(L"[UWLIQUID] frame failed hwnd=%p hr=%08X removed=%08X",
                   source ? source->window : nullptr, error, removed);
        }
        try {
            if (frame) {
                frame.Close();
                frame = nullptr;
            }
        } catch (...) {}
        if (recreatePool && source->active.load(std::memory_order_acquire)) {
            try {
                source->pool.Recreate(
                    graphicsDevice,
                    directx::DirectXPixelFormat::B8G8R8A8UIntNormalized,
                    2, recreateSize);
                std::lock_guard lock(mutex);
                if (source->active.load(std::memory_order_acquire))
                    source->poolSize = recreateSize;
            } catch (...) {
                Wh_Log(L"[UWLIQUID] capture resize failed hwnd=%p hr=%08X",
                       source->window, winrt::to_hresult().value);
            }
        }
    }

    void RenderLocked() {
        if (!running || !composite || !swapChain || width == 0 || height == 0)
            return;
        pendingPresent = false;
        CommitMotionObservationLocked(std::chrono::steady_clock::now());
        float clear[]{0, 0, 0, 0};
        context->ClearRenderTargetView(compositeTarget.get(), clear);
        int taskbarWidth = taskbarRect.right - taskbarRect.left;
        int taskbarHeight = taskbarRect.bottom - taskbarRect.top;
        if (taskbarWidth <= 0 || taskbarHeight <= 0) return;
        auto compositeRenderTarget = compositeTarget.get();
        auto sample = sampler.get();
        auto copyConstantBuffer = copyConstants.get();
        float blendFactor[4]{};
        context->OMSetRenderTargets(1, &compositeRenderTarget, nullptr);
        context->OMSetBlendState(copyBlend.get(), blendFactor, UINT_MAX);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->VSSetShader(vertexShader.get(), nullptr, 0);
        context->PSSetShader(copyPixelShader.get(), nullptr, 0);
        context->PSSetSamplers(0, 1, &sample);
        context->PSSetConstantBuffers(1, 1, &copyConstantBuffer);
        for (auto iterator = sources.rbegin(); iterator != sources.rend(); ++iterator) {
            auto const& source = **iterator;
            if (!source.latestView || source.contentSize.Width <= 0 ||
                source.contentSize.Height <= 0) continue;
            int sourceWidth = source.rect.right - source.rect.left;
            int sourceHeight = source.rect.bottom - source.rect.top;
            if (sourceWidth <= 0 || sourceHeight <= 0) continue;
            RECT validRect = source.rect;
            validRect.right = validRect.left + MulDiv(
                sourceWidth, source.textureSize.Width, source.contentSize.Width);
            validRect.bottom = validRect.top + MulDiv(
                sourceHeight, source.textureSize.Height, source.contentSize.Height);
            RECT intersection{};
            if (!IntersectRect(&intersection, &validRect, &taskbarRect)) continue;
            // A growing item can exceed its old pool for one frame. Clip the
            // unavailable area instead of stretching the valid subrectangle.
            float inverseWidth = float(source.contentSize.Width) /
                                 (sourceWidth * float(source.textureSize.Width));
            float inverseHeight = float(source.contentSize.Height) /
                                  (sourceHeight * float(source.textureSize.Height));
            CopyConstants copy{};
            copy.offset[0] = (intersection.left - source.rect.left) * inverseWidth;
            copy.offset[1] = (intersection.top - source.rect.top) * inverseHeight;
            copy.scale[0] = (intersection.right - intersection.left) * inverseWidth;
            copy.scale[1] = (intersection.bottom - intersection.top) * inverseHeight;
            context->UpdateSubresource(
                copyConstants.get(), 0, nullptr, &copy, 0, 0);
            D3D11_VIEWPORT sourceViewport{
                float(intersection.left - taskbarRect.left) * width / taskbarWidth,
                float(intersection.top - taskbarRect.top) * height / taskbarHeight,
                float(intersection.right - intersection.left) * width / taskbarWidth,
                float(intersection.bottom - intersection.top) * height / taskbarHeight,
                0, 1};
            context->RSSetViewports(1, &sourceViewport);
            auto sourceView = source.latestView.get();
            context->PSSetShaderResources(0, 1, &sourceView);
            context->Draw(3, 0);
        }
        ID3D11ShaderResourceView* noSource{};
        context->PSSetShaderResources(0, 1, &noSource);
        context->OMSetBlendState(nullptr, blendFactor, UINT_MAX);

        ID3D11ShaderResourceView* blurredView = compositeView.get();
        if (lens.optics.blur > 0.001f) {
            // GPU adaptation of bfraboni/FastGaussianBlur (MIT): approximate a
            // Gaussian with three horizontal and three vertical box passes.
            // https://github.com/bfraboni/FastGaussianBlur
            int radii[3];
            FastGaussianBoxRadii(lens.optics.blur * 12, radii);
            context->PSSetShader(boxBlurPixelShader.get(), nullptr, 0);
            auto blurConstantBuffer = blurConstants.get();
            context->PSSetConstantBuffers(2, 1, &blurConstantBuffer);
            D3D11_VIEWPORT blurViewport{
                0, 0, static_cast<float>(blurWidth),
                static_cast<float>(blurHeight), 0, 1};
            context->RSSetViewports(1, &blurViewport);
            for (int pass = 0; pass < 6; ++pass) {
                int targetIndex = pass % 2;
                auto target = blurTargets[targetIndex].get();
                auto source = pass == 0 ? compositeView.get()
                                        : blurViews[1 - targetIndex].get();
                BlurConstants blur{};
                blur.step[pass < 3 ? 0 : 1] =
                    1.0f / (pass < 3 ? blurWidth : blurHeight);
                blur.radius = radii[pass % 3];
                context->UpdateSubresource(
                    blurConstants.get(), 0, nullptr, &blur, 0, 0);
                context->OMSetRenderTargets(1, &target, nullptr);
                context->PSSetShaderResources(0, 1, &source);
                context->Draw(3, 0);
                context->PSSetShaderResources(0, 1, &noSource);
            }
            blurredView = blurViews[1].get();
        }

        HRESULT hr = S_OK;
        if (!backBuffer || !backTarget) {
            winrt::com_ptr<ID3D11Texture2D> newBackBuffer;
            winrt::com_ptr<ID3D11RenderTargetView> newBackTarget;
            hr = swapChain->GetBuffer(
                0, __uuidof(ID3D11Texture2D), newBackBuffer.put_void());
            if (SUCCEEDED(hr))
                hr = device->CreateRenderTargetView(
                    newBackBuffer.get(), nullptr, newBackTarget.put());
            if (FAILED(hr)) {
                failure = hr;
                return;
            }
            backBuffer = std::move(newBackBuffer);
            backTarget = std::move(newBackTarget);
        }
        context->ClearRenderTargetView(backTarget.get(), clear);
        auto presentLens = PredictedLensLocked(
            std::chrono::steady_clock::now());
        presentLens.imageSize[0] = static_cast<float>(width);
        presentLens.imageSize[1] = static_cast<float>(height);
        context->UpdateSubresource(
            constants.get(), 0, nullptr, &presentLens, 0, 0);
        D3D11_VIEWPORT viewport{0, 0, static_cast<float>(width),
                               static_cast<float>(height), 0, 1};
        auto renderTarget = backTarget.get();
        ID3D11ShaderResourceView* sourceViews[]{compositeView.get(), blurredView};
        auto constantBuffer = constants.get();
        context->OMSetRenderTargets(1, &renderTarget, nullptr);
        context->RSSetViewports(1, &viewport);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->VSSetShader(vertexShader.get(), nullptr, 0);
        context->PSSetShader(pixelShader.get(), nullptr, 0);
        context->PSSetShaderResources(0, 2, sourceViews);
        context->PSSetSamplers(0, 1, &sample);
        context->PSSetConstantBuffers(0, 1, &constantBuffer);
        context->Draw(3, 0);
        ID3D11ShaderResourceView* noSources[2]{};
        context->PSSetShaderResources(0, 2, noSources);
        hr = swapChain->Present(0, 0);
        if (FAILED(hr)) failure = hr;
    }

    void Stop() {
        std::vector<std::shared_ptr<Source>> oldSources;
        PTP_WORK work{};
        PresentWorkContext* workContext{};
        HANDLE waitable{};
        {
            std::lock_guard lock(mutex);
            if (!running && sources.empty() && !presentWork &&
                !frameLatencyWaitable)
                return;
            running = false;
            pendingPresent = false;
            presentWorkPending = false;
            motionActive = false;
            motionObservationPending = false;
            oldSources = std::move(sources);
            work = presentWork;
            presentWork = nullptr;
            workContext = presentWorkContext;
            presentWorkContext = nullptr;
            waitable = std::exchange(frameLatencyWaitable, nullptr);
        }
        if (work) {
            WaitForThreadpoolWorkCallbacks(work, TRUE);
            CloseThreadpoolWork(work);
            delete workContext;
        }
        if (waitable) CloseHandle(waitable);
        for (auto const& source : oldSources) CloseSource(source);
    }
};

void PrepareLiquid(Root& root, Measurements const& measurements);

struct LiquidMotionBatch {
    CompositionScopedBatch batch{nullptr};
    winrt::event_token completed{};
};
std::vector<LiquidMotionBatch> g_liquidMotionBatches;  // Owner thread only.
winrt::event_token g_liquidRenderingToken{};
bool g_liquidRenderingSubscribed{};
unsigned g_liquidFinalFrames{};

void SetLiquidMotionActive(bool active) noexcept {
    for (auto& root : g_roots)
        if (!root.retired && root.dock.liquid)
            root.dock.liquid->SetMotionActive(active);
}

void StopLiquidRendering() noexcept {
    if (!g_liquidRenderingSubscribed) return;
    try {
        Media::CompositionTarget::Rendering(g_liquidRenderingToken);
    } catch (...) {
        Wh_Log(L"[UWLIQUID] rendering unsubscribe failed hr=%08X",
               winrt::to_hresult().value);
    }
    g_liquidRenderingSubscribed = false;
}

void StopLiquidMotionTracking() noexcept {
    StopLiquidRendering();
    for (auto& motion : g_liquidMotionBatches) {
        try { motion.batch.Completed(motion.completed); }
        catch (...) {}
    }
    g_liquidMotionBatches.clear();
    g_liquidFinalFrames = 0;
    SetLiquidMotionActive(false);
}

void OnLiquidRendering() noexcept {
    ActiveScope active;
    if (g_stopping || GetCurrentThreadId() != g_ownerThread || g_busy) return;
    g_busy = true;
    struct Unbusy { ~Unbusy() { g_busy = false; } } unbusy;
    bool needed = false;
    try {
        for (auto& root : g_roots) {
            if (root.retired || !root.measurementsReady || !root.dock.liquid)
                continue;
            needed = true;
            // Sample only optical geometry, never rediscover/rearrange the tree.
            PrepareLiquid(root, root.measurements);
        }
        if (g_liquidMotionBatches.empty()) {
            if (g_liquidFinalFrames) --g_liquidFinalFrames;
            else needed = false;
        }
        if (!needed) StopLiquidRendering();
    } catch (...) {
        Wh_Log(L"[UWLIQUID] motion sample failed hr=%08X",
               winrt::to_hresult().value);
        StopLiquidMotionTracking();
    }
}

CompositionScopedBatch BeginLiquidMotionBatch(const void* projectedAnimation) noexcept {
    if (g_stopping || g_theme != DockTheme::Liquid ||
        GetCurrentThreadId() != g_ownerThread ||
        std::none_of(g_roots.begin(), g_roots.end(), [](auto const& root) {
            return !root.retired && root.measurementsReady;
        }))
        return nullptr;
    CompositionScopedBatch batch{nullptr};
    winrt::event_token token{};
    try {
        void* abi{};
        if (!ReadLocal(projectedAnimation, abi) || !abi) return nullptr;
        winrt::Windows::Foundation::IInspectable animation{nullptr};
        winrt::copy_from_abi(animation, abi);
        auto finite = [](CompositionAnimation const& value) {
            auto keyframes = value.try_as<KeyFrameAnimation>();
            return keyframes &&
                   keyframes.IterationBehavior() == AnimationIterationBehavior::Count;
        };
        Compositor compositor{nullptr};
        if (auto group = animation.try_as<CompositionAnimationGroup>()) {
            bool any = false;
            for (auto const& member : group) {
                if (!finite(member)) return nullptr;
                any = true;
            }
            if (!any) return nullptr;
            compositor = group.Compositor();
        } else if (auto single = animation.try_as<CompositionAnimation>();
                   single && finite(single)) {
            compositor = single.Compositor();
        } else {
            return nullptr;  // An expression/infinite animation never completes.
        }
        batch = compositor.CreateScopedBatch(CompositionBatchTypes::Animation);
        token = batch.Completed([](auto const& sender, auto const&) noexcept {
            ActiveScope active;
            if (g_stopping || GetCurrentThreadId() != g_ownerThread) return;
            try {
                auto completed = sender.template as<CompositionScopedBatch>();
                auto found = std::find_if(
                    g_liquidMotionBatches.begin(), g_liquidMotionBatches.end(),
                    [&](auto const& motion) { return motion.batch == completed; });
                if (found != g_liquidMotionBatches.end()) {
                    found->batch.Completed(found->completed);
                    g_liquidMotionBatches.erase(found);
                }
                if (g_liquidMotionBatches.empty()) {
                    SetLiquidMotionActive(false);
                    g_liquidFinalFrames = 2;
                }
            } catch (...) {
                Wh_Log(L"[UWLIQUID] motion completion failed hr=%08X",
                       winrt::to_hresult().value);
            }
        });
        g_liquidMotionBatches.push_back({batch, token});
        SetLiquidMotionActive(true);
        if (!g_liquidRenderingSubscribed) {
            g_liquidRenderingToken = Media::CompositionTarget::Rendering(
                [](auto const&, auto const&) { OnLiquidRendering(); });
            g_liquidRenderingSubscribed = true;
        }
        return batch;
    } catch (...) {
        if (batch) {
            try { batch.Completed(token); } catch (...) {}
            std::erase_if(g_liquidMotionBatches,
                          [&](auto const& motion) { return motion.batch == batch; });
            try { batch.End(); } catch (...) {}
        }
        Wh_Log(L"[UWLIQUID] motion observation unavailable hr=%08X",
               winrt::to_hresult().value);
        return nullptr;
    }
}

void EndLiquidMotionBatch(CompositionScopedBatch const& batch) noexcept {
    if (!batch) return;
    try { batch.End(); }
    catch (...) {
        StopLiquidMotionTracking();
        Wh_Log(L"[UWLIQUID] motion batch end failed hr=%08X",
               winrt::to_hresult().value);
    }
}

#undef UW_STRINGIZE
#undef UW_STRINGIZE_IMPL
#undef UW_BUTTON_LENS_CAP

winrt::weak_ref<FrameworkElement> MotionEvent(
    void* abi, Root::Source::Life life, bool cancelled = false,
    bool refresh = true) noexcept;
void OnLayout(winrt::weak_ref<FrameworkElement> const& weak);
void Refresh(Root& root);
void RefreshTaskbarHeight();

// ABI boundaries: raw IElementAnimatorOverrides arguments versus one-pointer
// C++/WinRT projected objects passed by const reference.
using HideAnimation_t = int(WINAPI*)(void*, void*, unsigned);
using ProjectedAnimation_t = void(WINAPI*)(void*, const void*);
using UpdateVisualStates_t = void(WINAPI*)(void*);
HideAnimation_t HideAnimation_Original;
ProjectedAnimation_t StartAnimation_Original;
ProjectedAnimation_t HideCompleted_Original;
UpdateVisualStates_t UpdateVisualStates_Original;

// ---- Native animation hooks ---------------------------------------------
uintptr_t CanonicalIdentity(FrameworkElement const& element) {
    winrt::com_ptr<::IUnknown> identity;
    winrt::check_hresult(static_cast<::IUnknown*>(winrt::get_abi(element))->QueryInterface(
        __uuidof(::IUnknown), identity.put_void()));
    return reinterpret_cast<uintptr_t>(identity.get());
}

int WINAPI HideAnimation_Hook(void* self, void* element, unsigned context) {
    ActiveScope active;
    MotionEvent(element, Root::Source::Life::Hiding);
    int result = HideAnimation_Original(self, element, context);
    if (FAILED(result)) MotionEvent(element, Root::Source::Life::Active, true);
    return result;
}

void WINAPI StartAnimation_Hook(void* projectedElement, const void* animation) {
    ActiveScope active;
    void* abi{};
    if (!g_stopping && GetCurrentThreadId() == g_ownerThread && ReadLocal(projectedElement, abi))
    {
        MotionEvent(abi, Root::Source::Life::Active);
    }
    auto batch = BeginLiquidMotionBatch(animation);
    struct EndBatch {
        CompositionScopedBatch const& batch;
        ~EndBatch() { EndLiquidMotionBatch(batch); }
    } endBatch{batch};
    StartAnimation_Original(projectedElement, animation);
}

void WINAPI HideCompleted_Hook(void* projectedAnimator, const void* projectedElement) {
    ActiveScope active;
    void* abi{};
    if (!g_stopping && GetCurrentThreadId() == g_ownerThread && ReadLocal(projectedElement, abi))
    {
        auto frame =
            MotionEvent(abi, Root::Source::Life::Retired, false, false);
        bool previousBusy = g_busy;
        g_busy = true;
        try {
            HideCompleted_Original(projectedAnimator, projectedElement);
        } catch (...) {
            g_busy = previousBusy;
            throw;
        }
        g_busy = previousBusy;
        if (!previousBusy && frame) OnLayout(frame);
        return;
    }
    HideCompleted_Original(projectedAnimator, projectedElement);
}

void WINAPI UpdateVisualStates_Hook(void* self) {
    ActiveScope active;
    UpdateVisualStates_Original(self);
    if (g_stopping || GetCurrentThreadId() != g_ownerThread ||
        g_itemStateRefreshPending || !g_dispatch)
        return;
    if (PostMessage(g_dispatch, kRefreshItemStatesMessage, 0, 0))
        g_itemStateRefreshPending = true;
}

FrameworkElement Child(FrameworkElement parent, const wchar_t* name, bool byClass = false);
void RetireRoot(Root& root);
void StopLiquid(OwnedDock& dock);

void WithdrawTray(Root& root) {
    if (root.taskbar && GetProp(root.taskbar, kTrayDispatcher) == g_dispatch)
        RemoveProp(root.taskbar, kTrayDispatcher);
    if (root.taskbar && root.publishedShift &&
        GetProp(root.taskbar, kTrayShift) == root.publishedShift) {
        RemoveProp(root.taskbar, kTrayShift);
        RestorePanels(root.taskbar);
    }
    root.publishedShift = nullptr;
}

bool ResolveTaskbar(Root& root, double host) {
    // UWP lacks WinUI's ContentIslandEnvironment HWND accessor. Match one
    // native taskbar by owner thread, full width and raster scale, once per
    // refresh; publication below reuses this association.
    double scale = root.frame.XamlRoot().RasterizationScale();
    HWND match{}, window{};
    for (int i = 0; i < 4096 &&
                    (window = FindWindowEx(nullptr, window, nullptr, nullptr));
         ++i) {
        if ((!ClassIs(window, L"Shell_TrayWnd") &&
             !ClassIs(window, L"Shell_SecondaryTrayWnd")) ||
            GetWindowThreadProcessId(window, nullptr) != g_ownerThread)
            continue;
        RECT rect{};
        if (!GetWindowRect(window, &rect) ||
            std::abs(GetDpiForWindow(window) / 96.0 - scale) > 0.01 ||
            std::abs(rect.right - rect.left - host * scale) > 2)
            continue;
        if (match) {
            match = nullptr;
            break;
        }
        match = window;
    }
    for (auto& other : g_roots) {
        if (&other == &root || other.retired || !other.taskbar ||
            other.taskbar != match)
            continue;
        bool attached = false;
        bool sameXamlRoot = false;
        try {
            attached = other.frame &&
                       Media::VisualTreeHelper::GetParent(other.frame) != nullptr;
            sameXamlRoot = other.frame &&
                           other.frame.XamlRoot() == root.frame.XamlRoot();
        } catch (...) {
        }
        if (!attached || sameXamlRoot)
            RetireRoot(other);
        else
            match = nullptr;
        break;
    }
    if (match != root.taskbar) {
        StopLiquid(root.dock);
        WithdrawTray(root);
        root.taskbar = nullptr;
    }
    if (!match) {
        if (!root.mappingReported)
            Wh_Log(L"[UWPANEL] no unambiguous taskbar mapping frame=%p host=%.3f scale=%.3f",
                   winrt::get_abi(root.frame), host, scale);
        root.mappingReported = true;
        return false;
    }
    root.mappingReported = false;
    root.taskbar = match;
    return true;
}

Controls::Border SourcePlate(
    FrameworkElement const& button,
    std::vector<Root::StatePlate> const& states) {
    for (auto const& state : states) {
        if (!state.element || state.element.Name() != L"BackgroundElement")
            continue;
        auto node = state.element.try_as<FrameworkElement>();
        for (unsigned depth = 0; node && depth < 16; ++depth) {
            if (node == button) return state.element;
            node = Media::VisualTreeHelper::GetParent(node)
                       .try_as<FrameworkElement>();
        }
    }
    return nullptr;
}

bool PublishTray(Root& root, double shift) {
    if (!root.taskbar) return false;
    double scale = root.frame.XamlRoot().RasterizationScale();
    if (!std::isfinite(scale * shift) || std::abs(scale * shift) > INT_MAX) {
        WithdrawTray(root);
        root.taskbar = nullptr;
        return false;
    }
    HANDLE value = EncodeShift(static_cast<int>(std::lround(scale * shift)));
    if (value == root.publishedShift &&
        GetProp(root.taskbar, kTrayShift) == value &&
        (!root.follower ||
         GetProp(root.taskbar, kTrayDispatcher) == g_dispatch))
        return true;
    if (SetProp(root.taskbar, kTrayShift, value)) {
        if (root.follower &&
            !SetProp(root.taskbar, kTrayDispatcher, g_dispatch)) {
            root.publishedShift = value;
            WithdrawTray(root);
            root.taskbar = nullptr;
            return false;
        }
        root.publishedShift = value;
        return true;
    }
    HWND failed = root.taskbar;
    WithdrawTray(root);
    root.taskbar = nullptr;
    Wh_Log(L"[UWPANEL] publication failed taskbar=%p error=%lu", failed,
           GetLastError());
    return false;
}


FrameworkElement Child(FrameworkElement parent, const wchar_t* name,
                       bool byClass) {
    if (parent) {
        int count = Media::VisualTreeHelper::GetChildrenCount(parent);
        for (int i = 0; i < count; i++) {
            auto child = Media::VisualTreeHelper::GetChild(parent, i)
                             .try_as<FrameworkElement>();
            if (child && (byClass ? winrt::get_class_name(child) == name
                                  : child.Name() == name)) {
                return child;
            }
        }
    }
    return nullptr;
}

void CollectStatePlates(FrameworkElement const& parent,
                        std::vector<Controls::Border>& result,
                        unsigned depth = 16) {
    if (!parent || !depth) return;
    int count = Media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto child = Media::VisualTreeHelper::GetChild(parent, i)
                         .try_as<FrameworkElement>();
        if (!child) continue;
        if (auto border = child.try_as<Controls::Border>()) {
            auto name = border.Name();
            if (name == L"BackgroundElement" ||
                name == L"BackgroundBorder")
                result.push_back(border);
        }
        CollectStatePlates(child, result, depth - 1);
    }
}

void CollectTextBlocks(FrameworkElement const& parent,
                       std::vector<Controls::TextBlock>& result,
                       unsigned depth = 32) {
    if (!parent || !depth) return;
    int count = Media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto child = Media::VisualTreeHelper::GetChild(parent, i)
                         .try_as<FrameworkElement>();
        if (!child) continue;
        if (auto text = child.try_as<Controls::TextBlock>())
            result.push_back(text);
        CollectTextBlocks(child, result, depth - 1);
    }
}

bool IsAttentionStateName(std::wstring_view name) {
    return name.find(L"RequestingAttention") != std::wstring_view::npos;
}

bool HasAttentionState(FrameworkElement const& element, unsigned depth = 4) {
    if (!element || !depth) return false;
    for (auto const& group : VisualStateManager::GetVisualStateGroups(element)) {
        auto state = group.CurrentState();
        if (state) {
            auto name = state.Name();
            if (IsAttentionStateName(name.c_str()))
                return true;
        }
    }
    int count = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; ++i) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (child && HasAttentionState(child, depth - 1)) return true;
    }
    return false;
}

void RestorePlateOpacity(Root::StatePlate& state) {
    if (!state.opacityOwned || !state.element) return;
    try {
        if (std::abs(state.element.Opacity() - state.appliedOpacity) < 0.001) {
            if (state.originalOpacity == DependencyProperty::UnsetValue())
                state.element.ClearValue(UIElement::OpacityProperty());
            else
                state.element.SetValue(UIElement::OpacityProperty(),
                                       state.originalOpacity);
        } else {
            Wh_Log(L"[UWITEM] state plate opacity ownership lost");
        }
    } catch (...) {
        Wh_Log(L"[UWITEM] state plate opacity restore failed hr=%08X",
               winrt::to_hresult().value);
    }
    state.opacityOwned = false;
    state.originalOpacity = nullptr;
}

void RestoreStatePlate(Root::StatePlate& state) {
    if (!state.element) return;
    try {
        bool transformMatch =
            state.stretch && state.element.RenderTransform() == state.stretch;
        bool clipMatch =
            state.clip && state.visual && state.visual.Clip() == state.clip;
        if (state.stretch) {
            if (transformMatch) {
                state.element.ClearValue(UIElement::RenderTransformProperty());
            } else {
                Wh_Log(L"[UWITEM] state plate transform ownership lost");
            }
        }
        if (state.clip) {
            if (clipMatch)
                state.visual.Clip(nullptr);
            else
                Wh_Log(L"[UWITEM] state plate clip ownership lost");
        }
        if (state.rim) {
            if (Hosting::ElementCompositionPreview::GetElementChildVisual(
                    state.element) == state.rim)
                Hosting::ElementCompositionPreview::SetElementChildVisual(
                    state.element, nullptr);
            else
                Wh_Log(L"[UWITEM] state plate rim ownership lost");
        }
    } catch (...) {
        Wh_Log(L"[UWITEM] state plate restore failed hr=%08X",
               winrt::to_hresult().value);
    }
    RestorePlateOpacity(state);
    state.opacityBlocked = false;
    state.rim = nullptr;
    state.rimThemeReady = false;
    state.rimDimmed = false;
    state.visual = nullptr;
    state.clip = nullptr;
    state.clipGeometry = nullptr;
    state.stretch = nullptr;
    state.element = nullptr;
}

// Give up on one plate without losing track of it: RestoreStatePlate clears
// element, but the blocked entry must keep identifying its plate.
void BlockStatePlate(Root::StatePlate& state) {
    auto element = state.element;
    RestoreStatePlate(state);
    state.element = element;
    state.blocked = true;
}

// Replicates the native rim, measured off a live taskbar. Alpha compensation
// applies only when this mod actually owns the plate's dimming multiplier.
ShapeVisual ButtonRim(CompositionRoundedRectangleGeometry const& outline,
                      bool dark, double dimScale) {
    auto compositor = outline.Compositor();
    auto visual = compositor.CreateShapeVisual();
    visual.RelativeSizeAdjustment({1, 1});
    auto lift = [dimScale](unsigned alpha) {
        return static_cast<uint8_t>(std::min<unsigned>(
            255, static_cast<unsigned>(std::lround(alpha / dimScale))));
    };
    using Color = winrt::Windows::UI::Color;
    auto gradient = compositor.CreateLinearGradientBrush();
    gradient.StartPoint({0, 0});
    gradient.EndPoint({0, 1});
    gradient.ColorStops().Append(compositor.CreateColorGradientStop(
        0, dark ? Color{lift(0x1A), 255, 255, 255} : Color{lift(0x05), 0, 0, 0}));
    gradient.ColorStops().Append(compositor.CreateColorGradientStop(
        1, dark ? Color{lift(0x0F), 255, 255, 255} : Color{lift(0x0F), 0, 0, 0}));
    auto shape = compositor.CreateSpriteShape(outline);
    shape.StrokeBrush(gradient);
    shape.StrokeThickness(2);  // Outer half is clipped, leaving a 1-DIP rim.
    visual.Shapes().Append(shape);
    return visual;
}

void PrepareStatePlate(Root& root, Root::StatePlate& state,
                       DockInsets insets) {
    if (state.blocked || !state.element) return;
    auto plate = state.element;
    try {
        auto parent = Media::VisualTreeHelper::GetParent(plate)
                          .try_as<FrameworkElement>();
        if (!parent) return;
        // Native keeps the dependency property entirely untouched. For dimmed
        // themes, do not replace a local value/binding, and relinquish ownership
        // permanently if another writer changes the value we installed.
        bool dim = g_theme != DockTheme::Native;
        if (!dim) {
            RestorePlateOpacity(state);
        } else if (state.opacityOwned &&
                   std::abs(plate.Opacity() - state.appliedOpacity) >= 0.001) {
            state.opacityOwned = false;
            state.opacityBlocked = true;
            state.originalOpacity = nullptr;
            Wh_Log(L"[UWITEM] external opacity retained");
        } else if (!state.opacityOwned && !state.opacityBlocked) {
            auto local = plate.ReadLocalValue(UIElement::OpacityProperty());
            if (local != DependencyProperty::UnsetValue()) {
                state.opacityBlocked = true;
            } else {
                state.originalOpacity = local;
                state.appliedOpacity = plate.Opacity() * kPlateDimScale;
                plate.Opacity(state.appliedOpacity);
                state.opacityOwned = true;
            }
        }
        if (!state.clip && plate.Clip()) return;
        if (state.clip) {
            auto currentClip =
                state.visual ? state.visual.Clip() : nullptr;
            if (state.visual && !currentClip) {
                state.visual.Clip(state.clip);
            } else if (currentClip != state.clip) {
                BlockStatePlate(state);
                return;
            }
        }
        if (state.stretch) {
            auto currentTransform = plate.RenderTransform();
            if (!currentTransform) {
                plate.RenderTransform(state.stretch);
            } else if (currentTransform != state.stretch) {
                BlockStatePlate(state);
                return;
            }
        }
        auto transform = root.dock.background.TransformToVisual(parent);
        double plateTop = plate.ActualOffset().y;
        double top = transform.TransformPoint(
                         {0, static_cast<float>(insets.top)})
                         .Y -
                     plateTop;
        double bottom = transform.TransformPoint(
                            {0, static_cast<float>(
                                    root.dock.background.ActualHeight() -
                                    insets.bottom)})
                            .Y -
                        plateTop;
        auto rect = PlanVerticalClip(
            plate.ActualWidth(), plate.ActualHeight(), top, bottom);
        if (rect.Width <= 0 || rect.Height <= 0) return;
        bool needsStretch =
            top < 0 || bottom > plate.ActualHeight();
        if (needsStretch &&
            plate.ReadLocalValue(UIElement::RenderTransformProperty()) ==
                DependencyProperty::UnsetValue()) {
            state.stretch = Media::CompositeTransform();
            plate.RenderTransform(state.stretch);
        }
        if (needsStretch && state.stretch) {
            state.stretch.ScaleY(
                (bottom - top) / plate.ActualHeight());
            state.stretch.TranslateY(top);
            rect = PlanVerticalClip(plate.ActualWidth(),
                                    plate.ActualHeight(), 0,
                                    plate.ActualHeight());
        } else if (!needsStretch && state.stretch) {
            plate.ClearValue(UIElement::RenderTransformProperty());
            state.stretch = nullptr;
        }
        if (!state.clip) {
            state.visual =
                Hosting::ElementCompositionPreview::GetElementVisual(plate);
            if (state.visual.Clip()) {
                state.visual = nullptr;
                return;
            }
            state.clipGeometry =
                state.visual.Compositor().CreateRoundedRectangleGeometry();
            state.clip = state.visual.Compositor().CreateGeometricClip(
                state.clipGeometry);
            state.visual.Clip(state.clip);
        }
        // Only the fill plate. BackgroundBorder carries the same native gradient on
        // its own control, and a control holding both would otherwise get two
        // stacked rims. Never take a child visual someone else is using.
        ElementTheme rimTheme = plate.ActualTheme();
        bool rimDimmed = state.opacityOwned;
        if (state.rim && state.rimThemeReady &&
            (state.rimTheme != rimTheme || state.rimDimmed != rimDimmed)) {
            if (Hosting::ElementCompositionPreview::GetElementChildVisual(plate) ==
                state.rim) {
                Hosting::ElementCompositionPreview::SetElementChildVisual(plate,
                                                                           nullptr);
                state.rim = nullptr;
                state.rimThemeReady = false;
            } else {
                // Another owner replaced our child visual. Forget our visual,
                // but never overwrite the external one on this plate.
                state.rim = nullptr;
                state.rimThemeReady = false;
                state.blocked = true;
            }
        }
        if (g_keepButtonRim && !state.blocked && !state.rim &&
            plate.Name() == L"BackgroundElement" &&
            !Hosting::ElementCompositionPreview::GetElementChildVisual(plate)) {
            state.rim = ButtonRim(state.clipGeometry,
                                  rimTheme == ElementTheme::Dark,
                                  rimDimmed ? kPlateDimScale : 1.0);
            state.rimTheme = rimTheme;
            state.rimThemeReady = true;
            state.rimDimmed = rimDimmed;
            Hosting::ElementCompositionPreview::SetElementChildVisual(
                plate, state.rim);
        }
        double scaleY = state.stretch ? state.stretch.ScaleY() : 1;
        float radius = static_cast<float>(std::min(
            {g_buttonCornerRadius, static_cast<double>(rect.Width) / 2,
             rect.Height * scaleY / 2}));
        state.clipGeometry.Offset({rect.X, rect.Y});
        state.clipGeometry.Size({rect.Width, rect.Height});
        state.clipGeometry.CornerRadius(
            {radius, static_cast<float>(radius / scaleY)});
    } catch (...) {
        BlockStatePlate(state);
        Wh_Log(L"[UWITEM] state plate unavailable hr=%08X",
               winrt::to_hresult().value);
    }
}

void PrepareStatePlates(Root& root, FrameworkElement const& buttons,
                        FrameworkElement const& tray, DockInsets insets) {
    insets = GetButtonInsets(
        insets, root.dock.background.ActualHeight());
    std::vector<Controls::Border> plates;
    CollectStatePlates(buttons, plates);
    CollectStatePlates(tray, plates);
    std::erase_if(root.statePlates, [&](auto& state) {
        if (std::find(plates.begin(), plates.end(), state.element) !=
            plates.end())
            return false;
        RestoreStatePlate(state);
        return true;
    });
    for (auto const& plate : plates) {
        auto state = std::find_if(
            root.statePlates.begin(), root.statePlates.end(),
            [&](auto const& value) { return value.element == plate; });
        if (state == root.statePlates.end()) {
            root.statePlates.push_back({plate});
            state = root.statePlates.end() - 1;
        }
        PrepareStatePlate(root, *state, insets);
    }
}

void RestoreStatePlates(Root& root) {
    for (auto& state : root.statePlates) RestoreStatePlate(state);
    root.statePlates.clear();
}

void RefreshAttentionStates(Root& root) {
    // AddButtonLenses is the only reader, and PrepareLiquid returns before it
    // on every other theme. HasAttentionState walks four levels of visual tree and
    // every visual state group per source, on every coalesced state change.
    if (g_theme != DockTheme::Liquid) return;
    for (auto& source : root.sources) {
        if (auto element = source.element.get())
            source.requestingAttention = HasAttentionState(element);
    }
}

bool StillOwned(OwnedTranslation const& owned) {
    // ponytail: value comparison detects changed writes, not equal-value writes
    // or hidden animation ownership. Broader compatibility needs an ownership trace.
    return owned.element.Translation() == owned.applied;
}

void Restore(OwnedTranslation& owned) {
    if (!owned.installed) return;
    if (StillOwned(owned)) {
        owned.element.Translation(owned.original);
        auto restored = owned.element.Translation();
        Wh_Log(L"[UWPOS] restored translation element=%p value=%.3f,%.3f,%.3f verified=%d",
               winrt::get_abi(owned.element), restored.x, restored.y, restored.z,
               restored == owned.original);
    } else {
        Wh_Log(L"[UWPOS] ownership lost element=%p; external value retained",
               winrt::get_abi(owned.element));
    }
    owned.installed = false;
}

bool Prepare(FrameworkElement element, OwnedTranslation& owned) {
    if (owned.installed) {
        return owned.element == element && StillOwned(owned);
    }
    auto original = element.Translation();
    // Only try an idle translation channel. Never replace RenderTransform.
    if (original != float3{}) {
        Wh_Log(L"[UWPOS] existing translation element=%p value=%.3f,%.3f,%.3f",
               winrt::get_abi(element), original.x, original.y, original.z);
        return false;
    }
    owned.element = element;
    owned.original = original;
    owned.applied = original;
    return true;
}

bool EligibleSlot(double x, double width, double host) {
    return std::isfinite(x) && std::isfinite(width) && std::isfinite(host) &&
           width > 0 && host > 0 && x >= -1 && x + width <= host + 1;
}

Root::Source::Life NextLife(Root::Source::Life current, Root::Source::Life event, bool cancelled) {
    // Exit animations also call StartAnimation. They must not revive a hiding source.
    if (event == Root::Source::Life::Active && current == Root::Source::Life::Hiding && !cancelled)
        return current;
    return event;
}

void RestoreGlass(OwnedDock& dock) {
    if (!dock.material) return;
    // Restore independently: an unavailable element must not prevent the clip
    // or the other brush from being restored. Preserve later external writes.
    for (bool stroke : {false, true}) {
        try {
            auto rectangle = stroke ? dock.stroke : dock.fill;
            Media::Brush applied = stroke ? dock.transparentStroke.as<Media::Brush>()
                                          : dock.material;
            auto original = stroke ? dock.originalStroke : dock.originalFill;
            if (rectangle.Fill() == applied) {
                if (original == DependencyProperty::UnsetValue())
                    rectangle.ClearValue(Shapes::Shape::FillProperty());
                else rectangle.SetValue(Shapes::Shape::FillProperty(), original);
                Wh_Log(L"[UWGLASS] restored %s", stroke ? L"stroke" : L"fill");
            } else Wh_Log(L"[UWGLASS] external %s retained", stroke ? L"stroke" : L"fill");
        } catch (...) { Wh_Log(L"[UWGLASS] brush restore failed hr=%08X", winrt::to_hresult().value); }
    }
    try {
        if (Hosting::ElementCompositionPreview::GetElementChildVisual(dock.background) == dock.attachedVisual)
            Hosting::ElementCompositionPreview::SetElementChildVisual(dock.background, nullptr);
    } catch (...) { Wh_Log(L"[UWGLASS] highlight detach failed hr=%08X", winrt::to_hresult().value); }
    if (dock.liquid) dock.liquid->Stop();
    dock.liquid.reset();
    dock.highlights = nullptr;
    dock.attachedVisual = nullptr;
    dock.material = nullptr;
    dock.transparentStroke = nullptr;
    dock.fill = nullptr;
    dock.stroke = nullptr;
    dock.originalFill = nullptr;
    dock.originalStroke = nullptr;
}

ShapeVisual GlassHighlights(CompositionRoundedRectangleGeometry const& outline,
                            DockTheme theme, bool dark) {
    auto compositor = outline.Compositor();
    auto visual = compositor.CreateShapeVisual();
    visual.RelativeSizeAdjustment({1, 1});
    // Liquid draws the reference shader's 1.5-pixel, top-biased inner stroke.
    // A second Composition outline changes both its width and color profile.
    if (theme == DockTheme::Liquid) return visual;
    auto sheen = compositor.CreateLinearGradientBrush();
    sheen.StartPoint({0, 0});
    sheen.EndPoint({0, 1});
    using Color = winrt::Windows::UI::Color;
    sheen.ColorStops().Append(compositor.CreateColorGradientStop(0, Color{BYTE(dark ? 30 : 60), 255, 255, 255}));
    sheen.ColorStops().Append(compositor.CreateColorGradientStop(0.45f, Color{0, 255, 255, 255}));
    sheen.ColorStops().Append(compositor.CreateColorGradientStop(1, Color{BYTE(dark ? 24 : 14), 0, 0, 0}));
    auto rim = compositor.CreateLinearGradientBrush();
    rim.StartPoint({0, 0});
    rim.EndPoint({0.3f, 1});
    rim.ColorStops().Append(compositor.CreateColorGradientStop(0, Color{190, 255, 255, 255}));
    rim.ColorStops().Append(compositor.CreateColorGradientStop(
        0.4f, Color{45, 255, 255, 255}));
    rim.ColorStops().Append(compositor.CreateColorGradientStop(
        0.75f, Color{25, 255, 255, 255}));
    rim.ColorStops().Append(compositor.CreateColorGradientStop(1, Color{100, 255, 255, 255}));
    // Share the live clip geometry; no independent resize animation or layout.
    auto shape = compositor.CreateSpriteShape(outline);
    shape.FillBrush(sheen);
    shape.StrokeBrush(rim);
    shape.StrokeThickness(2); // Outer half is clipped, leaving a 1-DIP inner rim.
    visual.Shapes().Append(shape);
    return visual;
}

bool HighContrastActive() {
    HIGHCONTRAST contrast{};
    contrast.cbSize = sizeof(contrast);
    return !SystemParametersInfo(
               SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0) ||
           (contrast.dwFlags & HCF_HIGHCONTRASTON);
}

void PrepareGlass(OwnedDock& dock) {
    if (g_theme == DockTheme::Native || dock.materialBlocked) return;
    if (HighContrastActive()) {
        RestoreGlass(dock);
        return;
    }
    try {
        auto theme = dock.background.ActualTheme();
        if (dock.material) {
            if (dock.fill.Fill() != dock.material || dock.stroke.Fill() != dock.transparentStroke ||
                Hosting::ElementCompositionPreview::GetElementChildVisual(dock.background) != dock.attachedVisual) {
                RestoreGlass(dock);
                dock.materialBlocked = true;
                Wh_Log(L"[UWGLASS] appearance ownership changed; native/external material retained");
                return;
            }
            if (dock.materialTheme == theme) return;
            RestoreGlass(dock);
        }
        auto grid = Child(dock.background, L"Windows.UI.Xaml.Controls.Grid", true);
        auto fillElement = Child(grid, L"BackgroundFill");
        auto strokeElement = Child(grid, L"BackgroundStroke");
        auto fill = fillElement ? fillElement.try_as<Shapes::Rectangle>() : nullptr;
        auto stroke = strokeElement ? strokeElement.try_as<Shapes::Rectangle>() : nullptr;
        if (!fill || !stroke || Hosting::ElementCompositionPreview::GetElementChildVisual(dock.background))
            throw winrt::hresult_error(E_NOTIMPL, L"Background tree or child visual unavailable");
        auto originalFill = fill.ReadLocalValue(Shapes::Shape::FillProperty());
        auto originalStroke = stroke.ReadLocalValue(Shapes::Shape::FillProperty());
        // Do not turn a binding expression into a captured constant on restore.
        for (auto const& value : {originalFill, originalStroke})
            if (value && value != DependencyProperty::UnsetValue() && !value.try_as<Media::Brush>())
                throw winrt::hresult_error(E_NOTIMPL, L"Background brush binding retained");
        bool dark = theme == ElementTheme::Dark;
        bool frosted = g_theme == DockTheme::Frosted;
        Media::SolidColorBrush transparent(
            winrt::Windows::UI::Color{0, 0, 0, 0});
        Media::Brush material = transparent;
        if (g_theme != DockTheme::Liquid) {
            Media::AcrylicBrush acrylic;
            acrylic.BackgroundSource(Media::AcrylicBackgroundSource::Backdrop);
            acrylic.TintColor(
                dark ? winrt::Windows::UI::Color{255, 24, 26, 32}
                     : winrt::Windows::UI::Color{255, 250, 252, 255});
            acrylic.TintOpacity(frosted ? 0.45 : 0.08);
            acrylic.TintLuminosityOpacity(frosted ? 0.65 : 0.12);
            acrylic.FallbackColor(
                dark ? winrt::Windows::UI::Color{255, 32, 32, 32}
                     : winrt::Windows::UI::Color{255, 243, 243, 243});
            material = acrylic;
        }
        auto highlights = GlassHighlights(dock.geometry, g_theme, dark);
        dock.fill = fill;
        dock.stroke = stroke;
        dock.originalFill = originalFill;
        dock.originalStroke = originalStroke;
        dock.material = material;
        dock.transparentStroke = transparent;
        dock.highlights = highlights;
        dock.attachedVisual = highlights;
        dock.materialTheme = theme;
        fill.Fill(material);
        stroke.Fill(transparent);
        Hosting::ElementCompositionPreview::SetElementChildVisual(dock.background, dock.attachedVisual);
        Wh_Log(L"[UWGLASS] installed theme=%d dark=%d background=%p", static_cast<int>(g_theme), dark,
               winrt::get_abi(dock.background));
    } catch (...) {
        auto error = winrt::to_hresult();
        RestoreGlass(dock);
        dock.materialBlocked = true;
        Wh_Log(L"[UWGLASS] native material fallback hr=%08X", error.value);
    }
}

void StopLiquid(OwnedDock& dock) {
    if (!dock.liquid) return;
    try {
        if (Hosting::ElementCompositionPreview::GetElementChildVisual(
                dock.background) == dock.attachedVisual) {
            Hosting::ElementCompositionPreview::SetElementChildVisual(
                dock.background, dock.highlights);
            dock.attachedVisual = dock.highlights;
        }
    } catch (...) {
        Wh_Log(L"[UWLIQUID] visual fallback failed hr=%08X",
               winrt::to_hresult().value);
    }
    dock.liquid->Stop();
    dock.liquid.reset();
}

DWORD LiquidRetryDelay(unsigned failures) {
    return 1000u << std::min(failures, 5u);
}

// Source discovery/recovery is deliberately slower than presentation. Re-arming
// an existing id restarts it, so keep this timer independent of layout refreshes.
void ArmLiquidTimer() {
    if (g_liquidTimerArmed || !g_dispatch) return;
    if (SetTimer(g_dispatch, kLiquidTimerId, 250, nullptr))
        g_liquidTimerArmed = true;
}

void ScheduleLiquidRetry(OwnedDock& dock, HRESULT error) {
    DWORD delay = LiquidRetryDelay(dock.liquidFailures++);
    dock.liquidRetryAt = GetTickCount64() + delay;
    ArmLiquidTimer();
    Wh_Log(L"[UWLIQUID] transparent fallback hr=%08X; retry in %lu ms",
           error, delay);
}

double LinearColor(BYTE channel) {
    double value = channel / 255.0;
    return value <= 0.04045 ? value / 12.92
                            : std::pow((value + 0.055) / 1.055, 2.4);
}

double PixelLuminance(uint32_t pixel) {
    return 0.2126 * LinearColor((pixel >> 16) & 255) +
           0.7152 * LinearColor((pixel >> 8) & 255) +
           0.0722 * LinearColor(pixel & 255);
}

winrt::Windows::UI::Color ForegroundColor(bool light) {
    BYTE channel = light ? 255 : 0;
    return {255, channel, channel, channel};
}

struct ForegroundSampleRequest {
    struct Item {
        uintptr_t identity{};
        RECT region{};
    };
    HWND taskbar{};
    uintptr_t rootIdentity{};
    RECT bounds{};
    std::vector<Item> items;
};

struct ForegroundSampleResult {
    struct Item {
        uintptr_t identity{};
        double luminance{};
    };
    HWND taskbar{};
    uintptr_t rootIdentity{};
    std::vector<Item> items;
};

bool SampleRegionLuminance(uint32_t const* pixels, int width, int height,
                           RECT const& region, double& luminance) {
    RECT clipped{};
    RECT bounds{0, 0, width, height};
    if (!pixels || !IntersectRect(&clipped, &region, &bounds)) return false;
    int regionWidth = clipped.right - clipped.left;
    int regionHeight = clipped.bottom - clipped.top;
    if (regionWidth <= 1 || regionHeight <= 1) return false;
    constexpr int points[][2]{
        {0, 0}, {6, 0}, {0, 6}, {6, 6}, {0, 3},
    };
    std::vector<double> samples;
    samples.reserve(ARRAYSIZE(points));
    for (auto const& point : points) {
        int x = clipped.left + point[0] * (regionWidth - 1) / 6;
        int y = clipped.top + point[1] * (regionHeight - 1) / 6;
        samples.push_back(PixelLuminance(pixels[y * width + x]));
    }
    auto middle = samples.begin() + samples.size() / 2;
    std::nth_element(samples.begin(), middle, samples.end());
    luminance = *middle;
    return true;
}

bool ElementSampleRegion(Root const& root, Controls::TextBlock const& text,
                         RECT const& taskbar, UINT dpi, RECT& region) {
    try {
        if (!root.frame || text.ActualWidth() <= 0 ||
            text.ActualHeight() <= 0)
            return false;
        auto bounds = text.TransformToVisual(root.frame).TransformBounds(
            {0, 0, static_cast<float>(text.ActualWidth()),
             static_cast<float>(text.ActualHeight())});
        double scale = dpi / 96.0;
        constexpr double padding = 4;
        RECT padded{
            taskbar.left + static_cast<int>(
                               std::lround((bounds.X - padding) * scale)),
            taskbar.top + static_cast<int>(
                              std::lround((bounds.Y - padding) * scale)),
            taskbar.left + static_cast<int>(std::lround(
                               (bounds.X + bounds.Width + padding) * scale)),
            taskbar.top + static_cast<int>(std::lround(
                              (bounds.Y + bounds.Height + padding) * scale)),
        };
        return IntersectRect(&region, &padded, &taskbar) &&
               region.right - region.left > 1 &&
               region.bottom - region.top > 1;
    } catch (...) {
        return false;
    }
}

bool CaptureForegroundSamples(ForegroundSampleRequest const& request,
                              ForegroundSampleResult& result) {
    int width = request.bounds.right - request.bounds.left;
    int height = request.bounds.bottom - request.bounds.top;
    if (width <= 1 || height <= 1) return false;
    HDC screen = GetDC(nullptr);
    if (!screen) return false;
    HDC memory = CreateCompatibleDC(screen);
    void* bits{};
    HBITMAP bitmap{};
    HGDIOBJ previous{};
    bool captured{};
    if (memory) {
        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(info.bmiHeader);
        info.bmiHeader.biWidth = width;
        info.bmiHeader.biHeight = -height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        bitmap = CreateDIBSection(screen, &info, DIB_RGB_COLORS, &bits,
                                  nullptr, 0);
        if (bitmap) {
            previous = SelectObject(memory, bitmap);
            captured = previous && previous != HGDI_ERROR &&
                       BitBlt(memory, 0, 0, width, height, screen,
                              request.bounds.left, request.bounds.top,
                              SRCCOPY | CAPTUREBLT);
        }
    }
    result.taskbar = request.taskbar;
    result.rootIdentity = request.rootIdentity;
    result.items.clear();
    if (captured) {
        GdiFlush();
        auto pixels = static_cast<uint32_t const*>(bits);
        result.items.reserve(request.items.size());
        for (auto const& item : request.items) {
            double luminance{};
            if (SampleRegionLuminance(pixels, width, height, item.region,
                                      luminance))
                result.items.push_back({item.identity, luminance});
        }
    }
    if (previous && previous != HGDI_ERROR) SelectObject(memory, previous);
    if (bitmap) DeleteObject(bitmap);
    if (memory) DeleteDC(memory);
    ReleaseDC(nullptr, screen);
    return captured;
}

DWORD WINAPI ForegroundSampleWorker(void* raw) {
    std::unique_ptr<ForegroundSampleRequest> request(
        static_cast<ForegroundSampleRequest*>(raw));
    try {
        auto result = std::make_unique<ForegroundSampleResult>();
        CaptureForegroundSamples(*request, *result);
        HWND dispatch = g_dispatch;
        if (!g_stopping && dispatch) {
            auto rawResult = result.release();
            if (PostMessage(dispatch, kForegroundSampleMessage, 0,
                            reinterpret_cast<LPARAM>(rawResult)))
                return 0;
            delete rawResult;
        }
    } catch (...) {
    }
    --g_active;
    return 0;
}

bool SetForegroundColor(Root::ForegroundState& state, bool light,
                        bool fade) {
    if (!state.brush || (state.ready && state.light == light)) return false;
    auto target = ForegroundColor(light);
    try {
        auto from = state.brush.Color();
        if (state.transition) {
            state.transition.Stop();
            state.transition = nullptr;
        }
        if (fade && state.ready) {
            state.brush.Color(target);
            animation::ColorAnimation color;
            color.From(from);
            color.To(target);
            Duration duration;
            duration.Type = DurationType::TimeSpan;
            duration.TimeSpan = winrt::Windows::Foundation::TimeSpan(
                std::chrono::milliseconds(180));
            color.Duration(duration);
            animation::Storyboard storyboard;
            storyboard.Children().Append(color);
            animation::Storyboard::SetTarget(color, state.brush);
            animation::Storyboard::SetTargetProperty(color, L"Color");
            state.transition = storyboard;
            storyboard.Begin();
        } else {
            state.brush.Color(target);
        }
        state.light = light;
        state.ready = true;
        return true;
    } catch (...) {
        try {
            state.brush.Color(target);
            state.transition = nullptr;
            state.light = light;
            state.ready = true;
            return true;
        } catch (...) {
            return false;
        }
    }
}

void RestoreForegroundState(Root::ForegroundState& state) {
    auto text = state.element.get();
    if (!text || !state.owned) return;
    try {
        if (state.transition) state.transition.Stop();
        auto local =
            text.ReadLocalValue(Controls::TextBlock::ForegroundProperty())
                .try_as<Media::Brush>();
        if (local == state.brush) {
            if (state.original == DependencyProperty::UnsetValue())
                text.ClearValue(Controls::TextBlock::ForegroundProperty());
            else
                text.SetValue(Controls::TextBlock::ForegroundProperty(),
                              state.original);
        } else {
            Wh_Log(L"[UWFONT] foreground ownership lost");
        }
    } catch (...) {
        Wh_Log(L"[UWFONT] foreground restore failed hr=%08X",
               winrt::to_hresult().value);
    }
    state.owned = false;
    state.original = nullptr;
    state.brush = nullptr;
    state.transition = nullptr;
}

void RestoreForeground(Root& root) {
    for (auto& state : root.foregrounds)
        RestoreForegroundState(state);
    root.foregrounds.clear();
    root.foregroundSampleAt = 0;
}

void ApplyAdaptiveForeground(ForegroundSampleResult const& result) {
    auto root = std::find_if(g_roots.begin(), g_roots.end(), [&](auto const& r) {
        return !r.retired && r.taskbar == result.taskbar && r.frame &&
               reinterpret_cast<uintptr_t>(winrt::get_abi(r.frame)) ==
                   result.rootIdentity;
    });
    if (root == g_roots.end()) return;
    root->foregroundSamplePending = false;
    if (g_fontColorMode != FontColorMode::Adaptive ||
        root->foregrounds.empty())
        return;
    size_t initialized{}, changed{}, lightCount{}, darkCount{};
    for (auto const& sample : result.items) {
        auto state = std::find_if(
            root->foregrounds.begin(), root->foregrounds.end(),
            [&](auto const& candidate) {
                return candidate.identity == sample.identity;
            });
        if (state == root->foregrounds.end() || !state->owned) continue;
        bool wasReady = state->ready;
        bool light = PreferLightForeground(sample.luminance, state->light,
                                           state->ready);
        if (SetForegroundColor(*state, light, true)) {
            if (wasReady)
                ++changed;
            else
                ++initialized;
        }
        if (state->light)
            ++lightCount;
        else
            ++darkCount;
    }
    if (initialized || changed)
        Wh_Log(L"[UWFONT] adaptive initialized=%zu changed=%zu light=%zu dark=%zu",
               initialized, changed, lightCount, darkCount);
}

void UpdateAdaptiveForeground(Root& root) {
    if (g_fontColorMode != FontColorMode::Adaptive ||
        root.foregrounds.empty() || root.foregroundSamplePending)
        return;
    ULONGLONG now = GetTickCount64();
    if (now < root.foregroundSampleAt) return;
    try {
        RECT taskbar{};
        UINT dpi = GetDpiForWindow(root.taskbar);
        if (!root.taskbar || !dpi ||
            !GetWindowRect(root.taskbar, &taskbar))
            return;
        auto request = std::make_unique<ForegroundSampleRequest>();
        request->taskbar = root.taskbar;
        request->rootIdentity =
            reinterpret_cast<uintptr_t>(winrt::get_abi(root.frame));
        request->bounds = taskbar;
        request->items.reserve(root.foregrounds.size());
        for (auto& state : root.foregrounds) {
            auto text = state.element.get();
            if (!text || !state.owned) continue;
            RECT region{};
            if (!ElementSampleRegion(root, text, taskbar, dpi, region))
                continue;
            OffsetRect(&region, -taskbar.left, -taskbar.top);
            request->items.push_back({state.identity, region});
        }
        if (request->items.empty()) return;
        root.foregroundSamplePending = true;
        root.foregroundSampleAt = now + 500;
        ++g_active;
        auto rawRequest = request.release();
        if (!QueueUserWorkItem(ForegroundSampleWorker, rawRequest,
                               WT_EXECUTEDEFAULT)) {
            delete rawRequest;
            --g_active;
            root.foregroundSamplePending = false;
            root.foregroundSampleAt = 0;
            return;
        }
    } catch (...) {
        Wh_Log(L"[UWFONT] sample queue failed hr=%08X",
               winrt::to_hresult().value);
    }
}

void PrepareForeground(Root& root, FrameworkElement const& buttons,
                       FrameworkElement const& tray) {
    if (g_fontColorMode == FontColorMode::Default ||
        HighContrastActive()) {
        RestoreForeground(root);
        return;
    }
    try {
        std::vector<Controls::TextBlock> texts;
        CollectTextBlocks(buttons, texts);
        CollectTextBlocks(tray, texts);
        std::erase_if(root.foregrounds, [&](auto& state) {
            auto text = state.element.get();
            if (!text) return true;
            if (std::find(texts.begin(), texts.end(), text) != texts.end())
                return false;
            RestoreForegroundState(state);
            return true;
        });
        for (auto const& text : texts) {
            auto state = std::find_if(
                root.foregrounds.begin(), root.foregrounds.end(),
                [&](auto const& candidate) {
                    return candidate.element.get() == text;
                });
            if (state != root.foregrounds.end()) {
                if (state->owned &&
                    text.ReadLocalValue(
                            Controls::TextBlock::ForegroundProperty())
                            .try_as<Media::Brush>() != state->brush) {
                    if (state->transition) state->transition.Stop();
                    state->owned = false;
                    state->original = nullptr;
                    state->brush = nullptr;
                    state->transition = nullptr;
                    Wh_Log(L"[UWFONT] external foreground retained");
                } else if (state->owned &&
                           g_fontColorMode != FontColorMode::Adaptive) {
                    SetForegroundColor(
                        *state, g_fontColorMode == FontColorMode::Light,
                        false);
                }
                continue;
            }
            Root::ForegroundState next;
            next.element = winrt::make_weak(text);
            auto local =
                text.ReadLocalValue(
                    Controls::TextBlock::ForegroundProperty());
            root.foregrounds.push_back(std::move(next));
            auto& saved = root.foregrounds.back();
            if (local == DependencyProperty::UnsetValue() ||
                local.try_as<Media::Brush>()) {
                saved.identity = CanonicalIdentity(text);
                saved.original = local;
                saved.light =
                    g_fontColorMode == FontColorMode::Light ||
                    (g_fontColorMode == FontColorMode::Adaptive &&
                     root.frame.ActualTheme() != ElementTheme::Light);
                saved.ready = g_fontColorMode != FontColorMode::Adaptive;
                saved.brush =
                    Media::SolidColorBrush(ForegroundColor(saved.light));
                text.Foreground(saved.brush);
                saved.owned = true;
            }
        }
        if (g_fontColorMode == FontColorMode::Adaptive) {
            UpdateAdaptiveForeground(root);
            ArmLiquidTimer();
        }
    } catch (...) {
        Wh_Log(L"[UWFONT] foreground unavailable hr=%08X",
               winrt::to_hresult().value);
    }
}

void AddButtonLenses(Root& root, LiquidRenderer::LensConstants& lens,
                     double scale) {
    size_t dropped = 0;
    for (auto const& source : root.sources) {
        // included is the shared eligibility/lifecycle decision. Keep the
        // explicit retirement guard so stale cached inclusion can never emit a
        // lens while a source waits for parking or reuse.
        if (!source.included || source.life == Root::Source::Life::Retired)
            continue;
        if (!source.requestingAttention && !g_buttonLiquidLayer) continue;
        auto plate = source.plate;
        if (!plate) continue;
        if (lens.lensCount == kMaxButtonLenses) {
            ++dropped;
            continue;
        }
        auto owned = std::find_if(
            root.statePlates.begin(), root.statePlates.end(),
            [&](auto const& candidate) { return candidate.element == plate; });
        if (owned == root.statePlates.end() || !owned->clipGeometry) continue;

        // Express both the native Border and our inset clip in the dock's
        // coordinate space. TransformToVisual carries the optional vertical
        // stretch; separate native and clip bounds retain inset corners exactly.
        auto transform = plate.TransformToVisual(root.dock.background);
        auto nativeTopLeft = transform.TransformPoint({0, 0});
        auto nativeBottomRight = transform.TransformPoint(
            {static_cast<float>(plate.ActualWidth()),
             static_cast<float>(plate.ActualHeight())});
        auto offset = owned->clipGeometry.Offset();
        auto extent = owned->clipGeometry.Size();
        auto clipTopLeft = transform.TransformPoint({offset.x, offset.y});
        auto clipBottomRight = transform.TransformPoint(
            {offset.x + extent.x, offset.y + extent.y});
        double nativeWidth = nativeBottomRight.X - nativeTopLeft.X;
        double nativeHeight = nativeBottomRight.Y - nativeTopLeft.Y;
        double clipWidth = clipBottomRight.X - clipTopLeft.X;
        double clipHeight = clipBottomRight.Y - clipTopLeft.Y;
        double xScale = nativeWidth / plate.ActualWidth();
        double yScale = nativeHeight / plate.ActualHeight();
        if (!std::isfinite(nativeTopLeft.X) ||
            !std::isfinite(nativeTopLeft.Y) ||
            !std::isfinite(clipTopLeft.X) ||
            !std::isfinite(clipTopLeft.Y) ||
            !std::isfinite(xScale) || !std::isfinite(yScale) ||
            nativeWidth <= 2 || nativeHeight <= 2 ||
            clipWidth <= 2 || clipHeight <= 2 || xScale <= 0 || yScale <= 0)
            continue;

        auto index = lens.lensCount++;
        auto storeRect = [&](float (&target)[4], auto topLeft, double width,
                             double height) {
            target[0] = static_cast<float>((topLeft.X + width / 2) * scale);
            target[1] = static_cast<float>((topLeft.Y + height / 2) * scale);
            target[2] = static_cast<float>(width * scale / 2);
            target[3] = static_cast<float>(height * scale / 2);
        };
        storeRect(lens.lensRects[index], clipTopLeft, clipWidth, clipHeight);
        storeRect(lens.lensNativeRects[index], nativeTopLeft,
                  nativeWidth, nativeHeight);

        winrt::Windows::UI::Color tint{255, 232, 72, 68};
        if (auto brush = plate.Background().try_as<Media::SolidColorBrush>())
            tint = brush.Color();
        lens.lensTints[index][0] = tint.R / 255.0f;
        lens.lensTints[index][1] = tint.G / 255.0f;
        lens.lensTints[index][2] = tint.B / 255.0f;
        lens.lensTints[index][3] =
            source.requestingAttention ? 1.0f : 0.0f;

        auto corners = plate.CornerRadius();
        for (auto [slot, corner] : {std::pair{0, corners.TopLeft},
                                    std::pair{1, corners.TopRight},
                                    std::pair{2, corners.BottomRight},
                                    std::pair{3, corners.BottomLeft}})
            lens.lensRadii[index][slot] =
                static_cast<float>(corner * xScale * scale);
        auto clipRadius = owned->clipGeometry.CornerRadius();
        lens.lensShapeParams[index][0] = static_cast<float>(yScale / xScale);
        lens.lensShapeParams[index][1] =
            static_cast<float>(clipRadius.x * xScale * scale);
        lens.lensShapeParams[index][2] =
            static_cast<float>(clipRadius.y * yScale * scale);
    }
    if (dropped)
        Wh_Log(L"[UWLIQUID] %zu button lenses past the %zu cap", dropped,
               kMaxButtonLenses);
    lens.lensRefraction = lens.optics.refraction * 0.78f;
    lens.alertTintOpacity = 0.22f;
    lens.lensBlend = 0.72f;
}

void PrepareLiquid(Root& root, Measurements const& m) {
    auto& dock = root.dock;
    if (g_theme != DockTheme::Liquid) {
        StopLiquid(dock);
        dock.liquidRetryAt = 0;
        dock.liquidFailures = 0;
        return;
    }
    if (!dock.highlights || !root.taskbar || !root.publishedShift) {
        StopLiquid(dock);
        return;
    }
    if (dock.liquidRetryAt && GetTickCount64() < dock.liquidRetryAt) return;
    // Startup discovery arrives through SendMessage/WH_CALLWNDPROC. WinRT
    // rejects compositor setup from that input-synchronous call (8001010D).
    if (!dock.liquid && !g_preparingLiquid) {
        if (!g_liquidPreparePending && g_dispatch &&
            PostMessage(g_dispatch, kPrepareLiquidMessage, 0, 0))
            g_liquidPreparePending = true;
        return;
    }
    try {
        double scale = root.frame.XamlRoot().RasterizationScale();
        RECT taskbarRect{};
        if (!GetWindowRect(root.taskbar, &taskbarRect))
            throw winrt::hresult_error(HRESULT_FROM_WIN32(GetLastError()));
        UINT pixelWidth = static_cast<UINT>(std::lround(
            dock.background.ActualWidth() * scale));
        UINT pixelHeight = static_cast<UINT>(std::lround(
            dock.background.ActualHeight() * scale));
        bool created = false;
        if (!dock.liquid) {
            auto renderer = std::make_shared<LiquidRenderer>();
            if (!renderer->Initialize(dock.visual.Compositor(), root.taskbar,
                                      pixelWidth, pixelHeight))
                throw winrt::hresult_error(E_FAIL);
            dock.liquid = std::move(renderer);
            if (!g_liquidMotionBatches.empty())
                dock.liquid->SetMotionActive(true);
            dock.attachedVisual = dock.liquid->visualRoot;
            Hosting::ElementCompositionPreview::SetElementChildVisual(
                dock.background, dock.attachedVisual);
            created = true;
        }
        auto anchor = std::find_if(root.sources.begin(), root.sources.end(),
            [&](auto const& source) { return source.identity == root.anchorIdentity; });
        auto anchorElement =
            anchor != root.sources.end() ? anchor->element.get() : nullptr;
        if (!anchorElement)
            throw winrt::hresult_error(E_PENDING, L"Optical anchor unavailable");
        // Includes native child motion and our compositor-driven parent
        // translation: the same anchor/prefix drives the dock expression.
        double dockLeft = anchorElement.TransformToVisual(dock.background)
                              .TransformPoint({0, 0}).X -
                          root.anchorPrefix - kDockPadding;
        double dockWidth = 2 * (m.host / 2 - m.backgroundOrigin - dockLeft);
        if (!std::isfinite(dockLeft) || !std::isfinite(dockWidth) ||
            dockWidth <= 0)
            throw winrt::hresult_error(E_PENDING, L"Optical bounds unavailable");
        auto insets = m.insets;
        double dockHeight = m.visibleDockHeight();
        LiquidRenderer::LensConstants lens{};
        lens.center[0] = static_cast<float>(
            (dockLeft + dockWidth / 2) * scale);
        lens.center[1] = static_cast<float>(
            (insets.top + dockHeight / 2) * scale);
        lens.halfSize[0] = static_cast<float>(dockWidth * scale / 2);
        lens.halfSize[1] = static_cast<float>(dockHeight * scale / 2);
        lens.radius = static_cast<float>(
            EffectiveDockRadius(dockWidth, dockHeight) * scale);
        lens.optics = g_liquidSettings;
        lens.optics.zRadius *= static_cast<float>(scale);
        AddButtonLenses(root, lens, scale);
        if (!dock.liquid->UpdateGeometry(
                taskbarRect, pixelWidth, pixelHeight, lens))
            throw winrt::hresult_error(E_FAIL, L"Liquid renderer unavailable");
        if (created) dock.liquid->UpdateSources();
        dock.liquidRetryAt = 0;
        dock.liquidFailures = 0;
        ArmLiquidTimer();
    } catch (...) {
        HRESULT error = winrt::to_hresult().value;
        StopLiquid(dock);
        ScheduleLiquidRetry(dock, error);
    }
}

void Restore(OwnedDock& dock) {
    dock.liquidRetryAt = 0;
    dock.liquidFailures = 0;
    RestoreGlass(dock);
    if (!dock.clip) return;
    if (dock.visual.Clip() == dock.clip) {
        dock.visual.Clip(dock.originalClip);
        auto restored = dock.visual.Clip();
        Wh_Log(L"[UWDOCK] restored clip visual=%p original=%p current=%p verified=%d",
               winrt::get_abi(dock.visual), winrt::get_abi(dock.originalClip),
               winrt::get_abi(restored), restored == dock.originalClip);
    }
    else Wh_Log(L"[UWDOCK] clip ownership lost; external clip retained");
    dock.geometry.StopAnimation(L"Offset");
    dock.geometry.StopAnimation(L"Size");
    dock.geometry = nullptr;
    dock.clip = nullptr;
    dock.visual = nullptr;
    dock.originalClip = nullptr;
}

bool PrepareDock(OwnedDock& dock, FrameworkElement background,
                 DockInsets insets) {
    if (dock.clip) {
        return dock.background == background && dock.visual.Clip() == dock.clip;
    }
    auto visual = Hosting::ElementCompositionPreview::GetElementVisual(background);
    auto compositor = visual.Compositor();
    auto geometry = compositor.CreateRoundedRectangleGeometry();
    double height = background.ActualHeight() - insets.top - insets.bottom;
    float radius = static_cast<float>(
        EffectiveDockRadius(background.ActualWidth(), height));
    geometry.CornerRadius({radius, radius});
    geometry.Offset({0, static_cast<float>(insets.top)});
    geometry.Size({static_cast<float>(background.ActualWidth()),
                   static_cast<float>(height)});
    auto clip = compositor.CreateGeometricClip(geometry);
    dock.background = background;
    dock.visual = visual;
    dock.originalClip = visual.Clip();
    dock.clip = clip;
    dock.geometry = geometry;
    visual.Clip(clip);
    return true;
}

bool IncludeSource(bool eligible, Root::Source::Life life) {
    return life != Root::Source::Life::Retired && (eligible || life == Root::Source::Life::Hiding);
}

bool WholeGenerationReplaced(std::vector<uintptr_t> const& before,
                             std::vector<uintptr_t> const& after,
                             float oldPrefix, float newPrefix) {
    return !before.empty() && before.size() == after.size() &&
           std::abs(oldPrefix - newPrefix) <= 1 &&
           std::none_of(after.begin(), after.end(), [&](uintptr_t identity) {
               return std::find(before.begin(), before.end(), identity) != before.end();
           });
}

double BoundaryThreshold(double& overhang, double start, double buttons,
                         double width, double localTrayLeft) {
    // Native left = min((width-buttons)/2, rightLimit-buttons). The two
    // branches meet at left = width-rightLimit. Learn rightLimit only where
    // native layout is constrained; retain its tray-relative offset on exit.
    if (start - (width - buttons) / 2 < -1)
        overhang = start + buttons - localTrayLeft;
    return std::isfinite(overhang) ? width - localTrayLeft - overhang : -width;
}

std::wstring EdgeExpression(bool buttons = false) {
    // No retiring right edge feeds the parent: it would add a second reflow.
    std::wstring correction = L"Max(0, (geometry.threshold - (anchor.Offset.X - geometry.prefix)) / 2)";
    if (buttons)
        return L"Vector3(geometry.shift + " + correction +
               L", geometry.vertical, 0)";
    return L"Vector3(Clamp(dock.Offset.X + dock.Size.X + geometry.trayDockBase"
           L", geometry.minX, geometry.maxX), geometry.vertical, 0)";
}

void StopFollower(Root& root) {
    if (root.follower) {
        root.trayTranslation.element.StopAnimation(root.follower);
        root.buttonsTranslation.element.StopAnimation(root.buttonsFollower);
        root.trayTranslation.applied = root.trayTranslation.element.Translation();
        root.buttonsTranslation.applied = root.buttonsTranslation.element.Translation();
    }
    root.follower = nullptr;
    root.buttonsFollower = nullptr;
    root.anchorIdentity = 0;
    root.anchorPrefix = 0;
    root.nativeRightOverhang = NAN;
    root.metrics = nullptr;
    root.expressionSources.clear();
    RestoreStatePlates(root);
    root.sources.clear();
}

void UpdateFollower(Root& root, FrameworkElement const& repeater,
                    Measurements const& m) {
    int count = Media::VisualTreeHelper::GetChildrenCount(repeater);
    std::erase_if(root.sources, [](auto& source) {
        if (!source.element.get()) return true;
        source.included = false;
        source.plate = nullptr;
        return false;
    });
    if (count > 64) throw winrt::hresult_error(E_BOUNDS);
    std::vector<uintptr_t> seen, selected;
    bool acquired = false;
    uintptr_t anchorIdentity{};
    float anchorX{};
    for (int i = 0; i < count; ++i) {
        auto child = Media::VisualTreeHelper::GetChild(repeater, i).try_as<FrameworkElement>();
        if (!child) continue;
        auto slot = Controls::Primitives::LayoutInformation::GetLayoutSlot(child);
        bool eligible = child.Visibility() == Visibility::Visible &&
                        EligibleSlot(slot.X, slot.Width, repeater.ActualWidth());
        if (winrt::get_class_name(child) != L"Taskbar.TaskListButton") continue;
        auto identity = CanonicalIdentity(child);
        seen.push_back(identity);
        // Identity alone is unambiguous here: the erase_if above dropped every
        // source whose element is gone, so a surviving entry's canonical IUnknown
        // cannot have been recycled by a different live button. MotionEvent also
        // compares the element because it has no such prune in front of it.
        auto source = std::find_if(root.sources.begin(), root.sources.end(),
            [&](auto const& value) { return value.identity == identity; });
        if (source == root.sources.end()) {
            if (!eligible) continue;
            // Windows uses handout Visuals for these outer buttons. Obtain that
            // same Visual for startup too; never request
            // a handout for the tray, repeater, icon or label facade targets.
            auto visual = Hosting::ElementCompositionPreview::GetElementVisual(child);
            root.sources.push_back({identity, winrt::make_weak(child), visual,
                                    Root::Source::Life::Active});
            acquired = true;
            source = root.sources.end() - 1;
            source->requestingAttention =
                g_theme == DockTheme::Liquid && HasAttentionState(child);
        }
        source->included = IncludeSource(eligible, source->life);
        if (source->included) {
            selected.push_back(identity);
            source->plate = SourcePlate(child, root.statePlates);
        }
        if (eligible && source->life == Root::Source::Life::Active &&
            (!anchorIdentity || slot.X < anchorX)) {
            anchorIdentity = identity;
            anchorX = slot.X;
        }
        // A completed button remains excluded until parking or a new native
        // start proves reuse, even while its old slot still looks visible.
        if (!eligible && source->life != Root::Source::Life::Hiding) seen.pop_back();
    }
    std::erase_if(root.sources, [&](auto& source) {
        if (std::find(seen.begin(), seen.end(), source.identity) != seen.end())
            return false;
        return true;
    });
    float prefix = anchorX - static_cast<float>(m.startLeft);
    bool replaced = root.anchorIdentity && WholeGenerationReplaced(
        root.expressionSources, selected, root.anchorPrefix, prefix);
    // A partial identity change is an app reorder/removal. A complete same-size
    // replacement is the native element rebuild observed from Taskbar Labels.
    if (!anchorIdentity || prefix < 0 ||
        (root.anchorIdentity && (root.anchorIdentity != anchorIdentity ||
                                 std::abs(root.anchorPrefix - prefix) > 1) && !replaced))
        throw winrt::hresult_error(E_NOTIMPL, L"Leading app reference changed or unavailable");
    if (replaced) Wh_Log(L"[UWFOLLOW] button generation replaced root=%p sources=%zu",
                         winrt::get_abi(root.frame), selected.size());
    auto anchor = std::find_if(root.sources.begin(), root.sources.end(),
        [&](auto const& source) { return source.identity == anchorIdentity; });
    // Guards every anchor->visual dereference below, not just this first use.
    if (anchor == root.sources.end()) throw winrt::hresult_error(E_PENDING);
    if (!root.metrics)
        root.metrics = anchor->visual.Compositor().CreatePropertySet();
    bool knownBoundary = std::isfinite(root.nativeRightOverhang);
    double threshold = BoundaryThreshold(
        root.nativeRightOverhang, m.startLeft, m.buttonsWidth,
        repeater.ActualWidth(), m.trayLeft - m.origin);
    if (!knownBoundary && std::isfinite(root.nativeRightOverhang))
        Wh_Log(L"[UWFOLLOW] boundary root=%p threshold=%.3f overhang=%.3f tick=%llu",
               winrt::get_abi(root.frame), threshold, root.nativeRightOverhang, GetTickCount64());
    root.metrics.InsertScalar(L"threshold", static_cast<float>(threshold));
    root.metrics.InsertScalar(L"shift", static_cast<float>(m.shift));
    root.metrics.InsertScalar(L"prefix", prefix);
    root.metrics.InsertScalar(L"minX", static_cast<float>(-m.trayLeft));
    root.metrics.InsertScalar(L"maxX", static_cast<float>(m.host - m.trayWidth - m.trayLeft));
    root.metrics.InsertScalar(
        L"trayDockBase",
        static_cast<float>(m.origin - m.dockOrigin() - kDockPadding -
                           m.trayWidth - m.trayLeft));
    root.metrics.InsertScalar(L"dockOrigin", static_cast<float>(m.dockOrigin()));
    root.metrics.InsertScalar(
        L"center",
        static_cast<float>(m.host / 2 - (m.origin - m.dockOrigin())));
    root.metrics.InsertScalar(L"padding", static_cast<float>(kDockPadding));
    root.metrics.InsertScalar(L"margin", static_cast<float>(m.insets.top));
    root.metrics.InsertScalar(L"height",
                              static_cast<float>(m.visibleDockHeight()));
    root.metrics.InsertScalar(
        L"vertical",
        static_cast<float>((m.insets.top - m.insets.bottom) / 2));
    float radius = static_cast<float>(
        EffectiveDockRadius(m.host, m.visibleDockHeight()));
    root.dock.geometry.CornerRadius({radius, radius});
    if (root.follower && !acquired && selected == root.expressionSources) return;
    auto animation =
        root.metrics.Compositor().CreateExpressionAnimation(EdgeExpression());
    auto buttonsAnimation =
        root.metrics.Compositor().CreateExpressionAnimation(EdgeExpression(true));
    std::wstring dockLeft =
        L"geometry.dockOrigin + geometry.shift + Max(0, (geometry.threshold - "
        L"(anchor.Offset.X - geometry.prefix)) / 2) + anchor.Offset.X - "
        L"geometry.prefix - geometry.padding";
    auto dockOffsetAnimation = root.metrics.Compositor().CreateExpressionAnimation(
        L"Vector2(" + dockLeft + L", geometry.margin)");
    // New buttons receive their final right edge before their native entrance.
    // The combined group is centered, so its animated leading edge is sufficient.
    auto dockSizeAnimation = root.metrics.Compositor().CreateExpressionAnimation(
        L"Vector2(2 * Max(0, geometry.center - (" + dockLeft +
        L")), geometry.height)");
    animation.Target(L"Translation");
    buttonsAnimation.Target(L"Translation");
    dockOffsetAnimation.Target(L"Offset");
    dockSizeAnimation.Target(L"Size");
    for (auto const& expression : {animation, buttonsAnimation, dockOffsetAnimation, dockSizeAnimation}) {
        expression.SetReferenceParameter(L"geometry", root.metrics);
        expression.SetReferenceParameter(L"anchor", anchor->visual);
    }
    animation.SetReferenceParameter(L"dock", root.dock.geometry);
    // Keep cleanup armed before starting; a failed start is restored by Refresh's caller.
    root.expressionSources = std::move(selected);
    root.follower = animation;
    root.buttonsFollower = buttonsAnimation;
    root.anchorIdentity = anchorIdentity;
    root.anchorPrefix = prefix;
    root.trayTranslation.installed = true;
    root.buttonsTranslation.installed = true;
    root.buttonsTranslation.element.StartAnimation(buttonsAnimation);
    root.trayTranslation.element.StartAnimation(animation);
    root.dock.geometry.StartAnimation(L"Offset", dockOffsetAnimation);
    root.dock.geometry.StartAnimation(L"Size", dockSizeAnimation);
}

void RestoreRoot(Root& root) {
    WithdrawTray(root);
    try { RestoreForeground(root); }
    catch (...) { Wh_Log(L"[UWFONT] restoration-error"); }
    try { StopFollower(root); }
    catch (...) { Wh_Log(L"[UWFOLLOW] stop failed hr=%08X", winrt::to_hresult().value); }
    // Attempt both even if one XAML object is already closed.
    for (auto* owned : {&root.buttonsTranslation, &root.trayTranslation}) {
        try { Restore(*owned); }
        catch (...) { Wh_Log(L"[UWPOS] restoration-error element=%p", winrt::get_abi(owned->element)); }
    }
    try { Restore(root.dock); }
    catch (...) { Wh_Log(L"[UWDOCK] restoration-error"); }
    root.measurementsReady = false;
}

void RetireRoot(Root& root) {
    if (root.retired) return;
    root.retired = true;
    if (root.subscribed && root.frame) {
        try {
            root.frame.LayoutUpdated(root.layoutToken);
        } catch (...) {
            Wh_Log(L"[UWPOS] unsubscribe-error frame=%p",
                   winrt::get_abi(root.frame));
        }
        root.subscribed = false;
    }
    RestoreRoot(root);
    root.taskbar = nullptr;
    root.frame = nullptr;
}

void ReapRetiredRoots() {
    std::erase_if(g_roots, [](Root const& root) { return root.retired; });
}

// ---- The layout pass ----------------------------------------------------
void Refresh(Root& root) {
    if (root.retired) return;
    if (g_dockingDisabled || root.blocked) {
        RestoreRoot(root);
        return;
    }
    auto frame = root.frame;
    auto parent = Media::VisualTreeHelper::GetParent(frame).try_as<FrameworkElement>();
    auto grid = Child(frame, L"RootGrid");
    auto repeater = Child(grid, L"TaskbarFrameRepeater");
    auto start = Child(repeater, L"LaunchListButton");
    auto tray = Child(parent, L"SystemTray.SystemTrayFrame", true);
    auto background = Child(grid, L"Taskbar.TaskbarBackground", true);
    int realizedCount = repeater
                            ? Media::VisualTreeHelper::GetChildrenCount(repeater)
                            : 0;
    if (!parent || !repeater || !start || !tray || !background) {
        if (!root.fallbackReported)
            Wh_Log(L"[UWPOS] native fallback: required tree element missing");
        root.fallbackReported = true;
        RestoreRoot(root);
        return;
    }
    if (realizedCount > 64) {
        if (!root.fallbackReported)
            Wh_Log(L"[UWPOS] native fallback: realized child capacity=%d",
                   realizedCount);
        root.fallbackReported = true;
        RestoreRoot(root);
        return;
    }
    if ((root.buttonsTranslation.installed && root.buttonsTranslation.element != repeater) ||
        (root.trayTranslation.installed && root.trayTranslation.element != tray) ||
        (root.buttonsTranslation.installed && !root.follower && !StillOwned(root.buttonsTranslation)) ||
        (root.trayTranslation.installed && !root.follower && !StillOwned(root.trayTranslation))) {
        root.blocked = true;
        Wh_Log(L"[UWPOS] root blocked after ownership/tree change; re-enable to retry");
        RestoreRoot(root);
        return;
    }
    double host = parent.ActualWidth();
    double buttons = repeater.DesiredSize().Width;
    double trayWidth = tray.ActualWidth();
    auto overflow = Child(repeater, L"OverflowButton");
    bool overflowing = overflow &&
        Controls::Primitives::LayoutInformation::GetLayoutSlot(overflow).Width > 0;
    // ActualOffset excludes our facade Translation, so an animated parent
    // cannot feed back into the source geometry.
    double origin = frame.ActualOffset().x + grid.ActualOffset().x + repeater.ActualOffset().x;
    if (!root.follower && std::abs(origin - frame.ActualOffset().x -
        repeater.TransformToVisual(frame).TransformPoint({0, 0}).X) > 1)
        throw winrt::hresult_error(E_NOTIMPL);
    double repeaterWidth = repeater.ActualWidth();
    double nativeCenter = origin + repeaterWidth / 2;
    double startTarget = Controls::Primitives::LayoutInformation::GetLayoutSlot(start).X;
    double expectedStart = (repeaterWidth - buttons) / 2;
    double trayLeft = tray.ActualOffset().x;
    Placement target{};
    // Keep the full centered repeater budget. Account for the native strip's
    // layout offset without reading per-button animation transforms.
    bool supported = !overflowing && background.ActualWidth() > 0 &&
        background.ActualHeight() > DockInsetSum(background.ActualHeight()) &&
        std::abs(frame.ActualWidth() - host) <= 1 &&
        Plan(host, buttons, trayWidth, nativeCenter, trayLeft, startTarget - expectedStart, target);
    if (!supported) {
        if (!root.fallbackReported) Wh_Log(L"[UWPOS] native fallback host=%.3f frame=%.3f buttons=%.3f tray=%.3f center=%.3f start=%.3f overflow=%d root=%p repeaterWidth=%.3f expectedStart=%.3f trayLeft=%.3f",
            host, frame.ActualWidth(), buttons, trayWidth, nativeCenter, startTarget, overflowing,
            winrt::get_abi(frame), repeaterWidth, expectedStart, trayLeft);
        root.fallbackReported = true;
        RestoreRoot(root);
        return;
    }
    if (!root.follower && (!Prepare(repeater, root.buttonsTranslation) ||
                                   !Prepare(tray, root.trayTranslation))) {
        // A non-zero Translation may be a native animation in flight, so retry
        // rather than latch; an owner that releases the channel is picked up.
        if (!root.fallbackReported)
            Wh_Log(L"[UWPOS] translation unavailable; native layout retained");
        root.fallbackReported = true;
        RestoreRoot(root);
        return;
    }
    root.fallbackReported = false;
    root.hostWidth = host;
    // Resolve the native owner once. All later publication in this refresh
    // reuses this association and cannot disagree with the chosen insets.
    if (!ResolveTaskbar(root, host) || !PublishTray(root, target.trayX)) {
        RestoreRoot(root);
        return;
    }
    Measurements m{
        .host = host,
        .origin = origin,
        .startLeft = startTarget,
        .trayLeft = trayLeft,
        .trayWidth = trayWidth,
        .buttonsWidth = buttons,
        .shift = (host - trayWidth - kGap) / 2 - nativeCenter,
        .backgroundOrigin =
            background.TransformToVisual(parent).TransformPoint({0, 0}).X,
        .dockHeight = background.ActualHeight(),
        .insets = GetDockInsets(root.taskbar, background.ActualHeight()),
    };
    root.measurements = m;
    root.measurementsReady = true;
    if (!PrepareDock(root.dock, background, m.insets)) {
        root.blocked = true;
        Wh_Log(L"[UWDOCK] clip ownership/tree change; root left native");
        RestoreRoot(root);
        return;
    }
    PrepareStatePlates(root, repeater, tray, m.insets);
    UpdateFollower(root, repeater, m);
    PrepareGlass(root.dock);
    // Publication marks availability; popup hooks query current transformed
    // tray bounds on the owner thread instead of consuming this layout estimate.
    if (!PublishTray(root, target.trayX)) {
        RestoreRoot(root);
        return;
    }
    PrepareLiquid(root, m);
    PrepareForeground(root, repeater, tray);
}

void OnLayout(winrt::weak_ref<FrameworkElement> const& weak) {
    if (g_stopping) return;
    ActiveScope active;
    if (g_stopping || GetCurrentThreadId() != g_ownerThread || g_busy) return;
    FrameworkElement frame{nullptr};
    try { frame = weak.get(); }
    catch (...) { Wh_Log(L"[UWPOS] layout owner unavailable"); return; }
    if (!frame) return;
    g_busy = true;
    struct Unbusy { ~Unbusy() { g_busy = false; } } unbusy;
    for (auto& root : g_roots) {
        if (root.frame != frame) continue;
        try {
            Refresh(root);
            root.refreshFailures = 0;
        } catch (...) {
            auto error = winrt::to_hresult();
            bool expected = error.value == E_BOUNDS ||
                            error.value == E_PENDING ||
                            error.value == E_NOTIMPL;
            bool latch = !expected &&
                         ++root.refreshFailures >= kRefreshRetryLimit;
            if (expected) root.refreshFailures = 0;
            if (latch) root.blocked = true;
            if (expected || root.refreshFailures == 1 || latch)
                Wh_Log(L"[UWPOS] refresh-error hr=%08X attempt=%u%s",
                       error.value, root.refreshFailures,
                       expected ? L"; retryable native state" :
                       latch ? L"; root blocked, re-enable to retry" :
                               L"; retrying");
            RestoreRoot(root);
        }
        break;
    }
}

winrt::weak_ref<FrameworkElement> MotionEvent(
    void* abi, Root::Source::Life life, bool cancelled, bool refresh) noexcept {
    if (g_stopping || GetCurrentThreadId() != g_ownerThread || !abi) return {};
    try {
        winrt::Windows::Foundation::IUnknown borrowed{nullptr};
        winrt::copy_from_abi(borrowed, abi);
        auto node = borrowed.try_as<FrameworkElement>();
        for (unsigned depth = 0; node && depth < 32; ++depth) {
            if (winrt::get_class_name(node) == L"Taskbar.TaskListButton") break;
            node = Media::VisualTreeHelper::GetParent(node).try_as<FrameworkElement>();
        }
        if (!node || winrt::get_class_name(node) != L"Taskbar.TaskListButton")
            return {};
        auto identity = CanonicalIdentity(node);
        for (auto& root : g_roots) {
            auto source = std::find_if(root.sources.begin(), root.sources.end(),
                [&](auto const& value) { return value.identity == identity && value.element.get() == node; });
            if (source == root.sources.end()) continue;
            auto next = NextLife(source->life, life, cancelled);
            if (next == source->life) return {};
            auto frame = winrt::make_weak(root.frame);
            source->life = next;
            if (refresh && !g_busy) OnLayout(frame);
            return frame;
        }
    } catch (...) { Wh_Log(L"[UWFOLLOW] lifecycle failed hr=%08X", winrt::to_hresult().value); }
    return {};
}

LRESULT QueryCurrentTray(HWND taskbar) {
    if (g_stopping || g_busy || GetCurrentThreadId() != g_ownerThread) return 0;
    ActiveScope active;
    for (auto& root : g_roots) {
        if (root.taskbar != taskbar || !root.follower || !GetProp(taskbar, kTrayShift)) continue;
        try {
            auto tray = root.trayTranslation.element;
            auto parent = Media::VisualTreeHelper::GetParent(tray).try_as<FrameworkElement>();
            if (!parent) return 0;
            double shift = tray.TransformToVisual(parent).TransformPoint({0, 0}).X - tray.ActualOffset().x;
            double scale = root.frame.XamlRoot().RasterizationScale();
            if (!std::isfinite(shift * scale) || std::abs(shift) > root.hostWidth ||
                std::abs(shift * scale) > INT_MAX) return 0;
            // TransformToVisual is an on-demand XAML observation, not a promise
            // of exact compositor-frame readback during concurrent motion.
            return reinterpret_cast<LRESULT>(EncodeShift(static_cast<int>(std::lround(shift * scale))));
        } catch (...) { Wh_Log(L"[UWFOLLOW] popup sample failed hr=%08X", winrt::to_hresult().value); }
    }
    return 0;
}

void DiscoverExisting(HWND taskbar);

void RefreshWatchedFrames() {
    std::vector<winrt::weak_ref<FrameworkElement>> frames;
    frames.reserve(g_roots.size());
    for (auto const& root : g_roots)
        if (!root.retired && root.frame) frames.push_back(winrt::make_weak(root.frame));
    for (auto const& frame : frames) OnLayout(frame);
}

LRESULT CALLBACK DispatchProc(HWND window, UINT message, WPARAM wp, LPARAM lp) {
    if (message == kForegroundSampleMessage) {
        if (!lp) return 0;
        std::unique_ptr<ForegroundSampleResult> result(
            reinterpret_cast<ForegroundSampleResult*>(lp));
        struct Complete {
            ~Complete() { --g_active; }
        } complete;
        if (!g_stopping && result) {
            try {
                ApplyAdaptiveForeground(*result);
            } catch (...) {
                Wh_Log(L"[UWFONT] sample apply failed hr=%08X",
                       winrt::to_hresult().value);
            }
        }
        return 0;
    }
    if (message == kQueryTrayMessage) return QueryCurrentTray(reinterpret_cast<HWND>(wp));
    if (message == kPrepareLiquidMessage) {
        g_liquidPreparePending = false;
        ActiveScope active;
        if (g_stopping) return 0;
        g_preparingLiquid = true;
        struct Unprepare { ~Unprepare() { g_preparingLiquid = false; } } unprepare;
        RefreshWatchedFrames();
        return 0;
    }
    if (message == kRefreshItemStatesMessage) {
        g_itemStateRefreshPending = false;
        if (g_stopping) return 0;
        for (auto& root : g_roots) RefreshAttentionStates(root);
        RefreshWatchedFrames();
        return 0;
    }
    if (message == WM_TIMER && wp == kLiquidTimerId) {
        bool active = false;
        std::vector<winrt::weak_ref<FrameworkElement>> retries;
        ULONGLONG now = GetTickCount64();
        for (auto& root : g_roots) {
            auto& dock = root.dock;
            if (g_fontColorMode == FontColorMode::Adaptive &&
                !root.foregrounds.empty() && root.measurementsReady) {
                active = true;
                UpdateAdaptiveForeground(root);
            }
            if (dock.liquid) {
                active = true;
                HRESULT failure = dock.liquid->Failure();
                if (FAILED(failure)) {
                    Wh_Log(L"[UWLIQUID] renderer lost hr=%08X", failure);
                    StopLiquid(dock);
                    ScheduleLiquidRetry(dock, failure);
                } else {
                    dock.liquid->UpdateSources();
                }
            } else if (dock.liquidRetryAt) {
                active = true;
                if (now >= dock.liquidRetryAt)
                    retries.push_back(winrt::make_weak(root.frame));
            }
        }
        for (auto const& frame : retries) OnLayout(frame);
        if (!active) {
            KillTimer(window, kLiquidTimerId);
            g_liquidTimerArmed = false;
        }
        return 0;
    }
    if (message == kRestoreMessage) {
        g_liquidPreparePending = false;
        g_itemStateRefreshPending = false;
        KillTimer(window, kLiquidTimerId);
        g_liquidTimerArmed = false;
        StopLiquidMotionTracking();
        if (g_panelLocationHook) {
            UnhookWinEvent(g_panelLocationHook);
            g_panelLocationHook = nullptr;
        }
        for (auto& root : g_roots) RetireRoot(root);
        g_roots.clear();
        DestroyWindow(window);
        g_dispatch = nullptr;
        return 0;
    }
    return DefWindowProc(window, message, wp, lp);
}

bool EnsureDispatcher() {
    DWORD current = GetCurrentThreadId(), empty = 0;
    g_ownerThread.compare_exchange_strong(empty, current);
    // ponytail: one taskbar UI thread, as observed on this machine. Additional
    // threads need separate dispatchers before extending the runtime matrix.
    if (g_ownerThread != current) return false;
    if (!g_dispatch) {
        g_dispatch = CreateWindowEx(0, kWindowClass, L"", 0, 0, 0, 0, 0,
                                    HWND_MESSAGE, nullptr,
                                    GetModuleHandle(nullptr), nullptr);
    }
    if (g_dispatch && !g_panelLocationHook) {
        g_panelLocationHook = SetWinEventHook(
            EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
            PanelLocationEvent, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
        if (!g_panelLocationHook)
            Wh_Log(L"[UWPANEL] location hook failed error=%lu", GetLastError());
    }
    return g_dispatch != nullptr;
}

void WatchFrame(FrameworkElement frame) {
    if (!frame || !EnsureDispatcher()) return;
    for (auto& root : g_roots)
        if (!root.retired && root.frame == frame) return;
    g_busy = true;
    struct Unbusy {
        ~Unbusy() { g_busy = false; }
    } unbusy;
    // Retirement only marks entries while references/iterations may exist.
    // WatchFrame is the mutation boundary and reaps before appending.
    for (auto& root : g_roots) {
        if (root.retired || !root.frame) continue;
        try {
            if (!Media::VisualTreeHelper::GetParent(root.frame))
                RetireRoot(root);
        } catch (...) {
            RetireRoot(root);
        }
    }
    ReapRetiredRoots();
    Root root;
    root.frame = frame;
    auto weak = winrt::make_weak(frame);
    root.layoutToken =
        frame.LayoutUpdated([weak](auto&&, auto&&) { OnLayout(weak); });
    root.subscribed = true;
    try {
        g_roots.push_back(std::move(root));
    } catch (...) {
        frame.LayoutUpdated(root.layoutToken);
        throw;
    }
    Wh_Log(L"[UWPOS] watching frame=%p thread=%lu", winrt::get_abi(frame),
           GetCurrentThreadId());
}

void DiscoverExisting(HWND taskbar) {
    ActiveScope active;
    if (g_stopping) return;
    if (g_ownerThread && g_ownerThread != GetCurrentThreadId()) {
        Wh_Log(L"[UWPOS] startup unsupported UI thread taskbar=%p thread=%lu", taskbar, GetCurrentThreadId());
        return;
    }
    if (g_busy) return;
    try {
        auto xamlRoot = ExistingRoot(taskbar);
        auto frame = xamlRoot ? Child(xamlRoot.Content().try_as<FrameworkElement>(),
                                      L"Taskbar.TaskbarFrame", true) : nullptr;
        if (!frame) {
            Wh_Log(L"[UWPOS] startup root unavailable taskbar=%p; native layout retained", taskbar);
            return;
        }
        // The native HWND identifies replacement generations. Retire its old
        // frame before the replacement publishes, without erasing during any
        // active root traversal.
        for (auto& root : g_roots) {
            if (!root.retired && root.taskbar == taskbar && root.frame != frame)
                RetireRoot(root);
        }
        WatchFrame(frame);
        // The existing root is already arranged. Use the same guarded Refresh
        // path as later LayoutUpdated events without invalidating its geometry.
        OnLayout(winrt::make_weak(frame));
        Wh_Log(L"[UWPOS] startup discovered taskbar=%p frame=%p published=%d", taskbar,
               winrt::get_abi(frame), GetProp(taskbar, kTrayShift) != nullptr);
    } catch (...) {
        Wh_Log(L"[UWPOS] startup discovery failed taskbar=%p hr=%08X", taskbar, winrt::to_hresult().value);
    }
}

LRESULT CALLBACK DiscoverHook(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION && lp) {
        const auto& message = *reinterpret_cast<CWPSTRUCT*>(lp);
        if (g_discoverMessage && message.message == g_discoverMessage &&
            message.wParam == reinterpret_cast<WPARAM>(&g_discoverMessage)) DiscoverExisting(message.hwnd);
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

bool RequestDiscovery(HWND taskbar) {
    DWORD pid{};
    DWORD thread = GetWindowThreadProcessId(taskbar, &pid);
    if (!g_discoverMessage || g_stopping || !thread || pid != GetCurrentProcessId() ||
        (!ClassIs(taskbar, L"Shell_TrayWnd") && !ClassIs(taskbar, L"Shell_SecondaryTrayWnd"))) return false;
    if (thread == GetCurrentThreadId()) {
        DiscoverExisting(taskbar);
        return true;
    }
    // Same-process, one-shot UI-thread dispatch used by hide-start-button.
    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, DiscoverHook, nullptr, thread);
    if (!hook) return false;
    SendMessage(taskbar, g_discoverMessage, reinterpret_cast<WPARAM>(&g_discoverMessage), 0);
    BOOL removed = UnhookWindowsHookEx(hook);
    if (!removed) Wh_Log(L"[UWPOS] startup dispatch unhook failed error=%lu", GetLastError());
    return removed;
}

using Arrange_t = int(WINAPI*)(void*, Size, Size*);
Arrange_t Arrange_Original;
int WINAPI Arrange_Hook(void* self, Size size, Size* result) {
    ActiveScope active;
    int hr = Arrange_Original(self, size, result);
    if (g_stopping || FAILED(hr) || g_busy) return hr;
    try {
        if (!EnsureDispatcher()) return hr;
        FrameworkElement frame{nullptr};
        winrt::check_hresult(static_cast<IUnknown*>(self)->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(frame)));
        WatchFrame(frame);
    } catch (...) { Wh_Log(L"[UWPOS] discovery-error; no positioning for this pass"); }
    return hr;
}

bool ResolveStartupSymbols() {
    HMODULE module = LoadLibraryEx(
        L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"[UWPOS] failed to load taskbar.dll error=%lu", GetLastError());
        return false;
    }
    WindhawkUtils::SYMBOL_HOOK symbols[] = {
        {{LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"}, &g_taskBandVtable},
        {{LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"}, &g_secondaryTaskBandVtable},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
         &GetTaskbarHost_Original},
        {{LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
         &GetSecondaryTaskbarHost_Original},
        {{LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"}, &g_frameHeight},
        {{LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"}, &Decref_Original},
    };
    if (!WindhawkUtils::HookSymbols(module, symbols, ARRAYSIZE(symbols))) return false;
    BYTE code[8]{};
    if (!ReadLocal(g_frameHeight, code) || !HostElementOffset(code, g_hostElementOffset)) {
        Wh_Log(L"[UWPOS] unsupported TaskbarHost::FrameHeight; startup lookup refused");
        return false;
    }
    g_discoverMessage = RegisterWindowMessage(L"Windhawk.UWPOS06.DiscoverExisting");
    Wh_Log(L"[UWPOS] startup symbols resolved hostElementOffset=%zu", g_hostElementOffset);
    return g_discoverMessage != 0;
}

bool HookTaskbarViewSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK hooks[] = {{
        {LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
        &TaskbarConfiguration_GetFrameSize_Original,
        TaskbarConfiguration_GetFrameSize_Hook,
    }, {
        {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::ArrangeOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
        &Arrange_Original, Arrange_Hook,
    }, {
        {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarElementAnimator,struct winrt::Microsoft::UI::Xaml::Controls::IElementAnimatorOverrides>::StartHideAnimation(void *,unsigned int))"},
        &HideAnimation_Original, HideAnimation_Hook,
    }, {
        {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IUIElement9<struct winrt::Windows::UI::Xaml::Controls::Image>::StartAnimation(struct winrt::Windows::UI::Composition::ICompositionAnimationBase const &)const )"},
        &StartAnimation_Original, StartAnimation_Hook,
    }, {
        {LR"(public: __cdecl winrt::impl::consume_Microsoft_UI_Xaml_Controls_IElementAnimatorProtected<struct winrt::Taskbar::implementation::TaskbarElementAnimator>::OnHideAnimationCompleted(struct winrt::Windows::UI::Xaml::UIElement const &)const )"},
        &HideCompleted_Original, HideCompleted_Hook,
    }, {
        {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
        &UpdateVisualStates_Original, UpdateVisualStates_Hook,
    }};
    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks)))
        return false;
    Wh_Log(L"[UWPOS] initialized v" WH_MOD_VERSION
           L"; screenshot-safe liquid glass, native animation follower, gap=12 theme=%d fontColor=%d heightMethod=automatic",
           static_cast<int>(g_theme), static_cast<int>(g_fontColorMode));
    return true;
}

bool TryHookTaskbarView(HMODULE module, bool apply) {
    bool expected = false;
    if (!module ||
        !g_taskbarViewHookStarted.compare_exchange_strong(expected, true))
        return false;
    if (!HookTaskbarViewSymbols(module)) {
        Wh_Log(L"[UWPOS] Taskbar.View.dll symbol hook failed");
        return false;
    }
    if (apply) Wh_ApplyHookOperations();
    g_taskbarViewReady = true;
    return true;
}

void Wh_ModAfterInit();

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR path, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(path, file, flags);
    if (module && GetModuleHandle(L"Taskbar.View.dll") == module &&
        TryHookTaskbarView(module, true))
        Wh_ModAfterInit();
    return module;
}

// ---- Mod lifecycle ------------------------------------------------------
BOOL Wh_ModInit() {
    g_dockCornerRadius = std::clamp(
        Wh_GetIntSetting(L"cornerRadius"), 0, 100);
    g_buttonCornerRadius = std::clamp(
        Wh_GetIntSetting(L"buttonCornerRadius"), 0, 100);
    g_buttonVerticalPadding = std::clamp(
        Wh_GetIntSetting(L"buttonVerticalPadding"), 0, 100);
    g_dockEdgeDistance = std::clamp(
        Wh_GetIntSetting(L"edgeDistance"), 0, 100);
    g_keepButtonRim = Wh_GetIntSetting(L"button.border") != 0;
    g_buttonLiquidLayer = Wh_GetIntSetting(L"button.liquidLayer") != 0;
    auto theme = WindhawkUtils::StringSetting::make(L"theme");
    g_theme = ParseTheme(theme.get());
    auto fontColor = WindhawkUtils::StringSetting::make(L"fontColor");
    g_fontColorMode = ParseFontColor(fontColor.get());
    if (g_theme == DockTheme::Liquid) {
        auto percent = [](const wchar_t* name, int low, int high) {
            return std::clamp(Wh_GetIntSetting(name), low, high) / 100.0f;
        };
        g_liquidSettings.blur = percent(L"liquid.blur", 0, 100);
        g_liquidSettings.zRadius = static_cast<float>(
            std::clamp(Wh_GetIntSetting(L"liquid.zRadius"), 1, 100));
        g_liquidSettings.refraction = percent(L"liquid.refraction", 0, 200);
        g_liquidSettings.chroma = percent(L"liquid.chromaticAberration", 0, 30);
        g_liquidSettings.edgeHighlight = percent(L"liquid.edgeHighlight", 0, 100);
        g_liquidSettings.specular = percent(L"liquid.specular", 0, 100);
        g_liquidSettings.fresnel = percent(L"liquid.fresnel", 0, 200);
        g_liquidSettings.opacity = percent(L"liquid.opacity", 0, 100);
        g_liquidSettings.brightness = percent(L"liquid.brightness", -50, 50);
        auto preset = WindhawkUtils::StringSetting::make(L"liquid.preset");
        g_liquidSettings = ApplyLiquidPreset(preset.get(), g_liquidSettings);
    }
    wchar_t path[MAX_PATH];
    if (!GetModuleFileName(nullptr, path, ARRAYSIZE(path))) return FALSE;
    auto name = wcsrchr(path, L'\\');
    g_explorer = name && !_wcsicmp(name + 1, L"explorer.exe");
    if (!WindhawkUtils::SetFunctionHook(SetWindowPos, PanelSetWindowPos, &SetWindowPos_Original) ||
        !WindhawkUtils::SetFunctionHook(DestroyWindow, PanelDestroyWindow, &DestroyWindow_Original)) return FALSE;
    if (!g_explorer) {
        Wh_Log(L"[UWPANEL] initialized v" WH_MOD_VERSION
               L" Shell positioning hooks");
        return TRUE;
    }
    if (!WindhawkUtils::SetFunctionHook(DwmSetWindowAttribute, PanelDwmAttribute,
                                        &DwmSetWindowAttribute_Original)) return FALSE;
    DWORD alignment = 0, bytes = sizeof(alignment);
    if (RegGetValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                    L"TaskbarAl", RRF_RT_REG_DWORD, nullptr, &alignment, &bytes) != ERROR_SUCCESS || alignment != 1) {
        Wh_Log(L"[UWPOS] centered native alignment required");
        return FALSE;
    }
    if (!ResolveStartupSymbols()) return FALSE;
    WNDCLASS wc{};
    wc.lpfnWndProc = DispatchProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = kWindowClass;
    g_windowClass = RegisterClass(&wc);
    if (!g_windowClass) return FALSE;
    if (HMODULE module = GetModuleHandle(L"Taskbar.View.dll")) {
        if (!TryHookTaskbarView(module, false)) {
            UnregisterClass(kWindowClass, wc.hInstance);
            g_windowClass = 0;
            return FALSE;
        }
    } else {
        HMODULE kernelBase = GetModuleHandle(L"kernelbase.dll");
        auto loadLibraryExW = kernelBase
            ? reinterpret_cast<LoadLibraryExW_t>(
                  GetProcAddress(kernelBase, "LoadLibraryExW"))
            : nullptr;
        if (!loadLibraryExW ||
            !WindhawkUtils::SetFunctionHook(
                loadLibraryExW, LoadLibraryExW_Hook,
                &LoadLibraryExW_Original)) {
            UnregisterClass(kWindowClass, wc.hInstance);
            g_windowClass = 0;
            return FALSE;
        }
        Wh_Log(L"[UWPOS] Taskbar.View.dll not loaded yet; waiting");
    }
    return TRUE;
}

void RefreshTaskbarHeight() {
    HWND taskbar = FindWindow(L"Shell_TrayWnd", nullptr);
    DWORD pid{};
    if (!taskbar || !GetWindowThreadProcessId(taskbar, &pid) ||
        pid != GetCurrentProcessId())
        return;
    SendMessage(taskbar, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);
}

void Wh_ModAfterInit() {
    if (!g_explorer || g_stopping) return;
    if (!g_taskbarViewReady) {
        HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) TryHookTaskbarView(module, true);
    }
    if (!g_taskbarViewReady) return;
    for (auto name : {L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd"}) {
        HWND window{};
        for (int i = 0; i < 16 && (window = FindWindowEx(nullptr, window, name, nullptr)); ++i) {
            DWORD pid{};
            GetWindowThreadProcessId(window, &pid);
            if (pid != GetCurrentProcessId()) continue;
            if (!RequestDiscovery(window)) Wh_Log(L"[UWPOS] startup dispatch failed taskbar=%p", window);
        }
    }
    RefreshTaskbarHeight();
}

void Wh_ModBeforeUninit() {
    g_stopping = true;
    g_dockingDisabled = true;
    // Wait for mutation/discovery to finish before dispatching UI-thread cleanup.
    while (g_active) Sleep(1);
    if (HWND window = g_dispatch) SendMessage(window, kRestoreMessage, 0, 0);
    if (g_explorer) RefreshTaskbarHeight();
    // Closing the free-threaded capture pools prevents new frame callbacks.
    while (g_active) Sleep(1);
    RestorePanels(nullptr, !g_explorer);
}

void Wh_ModUninit() {
    if (g_windowClass) UnregisterClass(kWindowClass, GetModuleHandle(nullptr));
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE; // Reuse UI-thread teardown and startup for a clean theme switch.
    return TRUE;
}

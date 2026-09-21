// ==WindhawkMod==
// @id              windows-11-better-ultrawide-taskbar
// @name            Windows 11 Better Ultrawide Taskbar
// @description     Centers taskbar buttons and the system tray in a rounded dock while preserving native button animations
// @version         2.80
// @author          Molko
// @github          https://github.com/roeseth
// @include         explorer.exe
// @include         ShellHost.exe
// @include         ShellExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -ldwmapi -lwindowsapp -ld3d11 -ldxgi -ld3dcompiler -luser32 -lkernel32
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

Task-button hover uses Windows' native animation as a translucent tint over
the dock. An app requesting attention receives an additional inset refractive
lens with the native alert color.

**Task and tray buttons** can carry the raised gradient outline Windows draws on a
highlight. The dock's translucency fades the native one out, so the mod redraws it
at matching strength; turn the border off for a flat highlight.

Under the Liquid glass theme each task button can also carry its own refractive
lens inside the dock, rendered by the same shader pass as the dock itself rather
than a second capture. Tray items share the dock lens.

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
    $description: Gives every task button its own refractive lens inside the dock. Liquid glass theme only.
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
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>

using namespace winrt::Windows::UI::Xaml;
using winrt::Windows::Foundation::Size;
using winrt::Windows::Foundation::Numerics::float3;
using namespace winrt::Windows::UI::Composition;
namespace capture = winrt::Windows::Graphics::Capture;
namespace directx = winrt::Windows::Graphics::DirectX;
namespace direct3d11 = winrt::Windows::Graphics::DirectX::Direct3D11;

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
constexpr UINT_PTR kLiquidTimerId = 45;
constexpr DWORD kLiquidShrinkDelay = 700;
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
ATOM g_windowClass{};
bool g_busy{};  // Owner thread only.
bool g_explorer{};
bool g_preparingLiquid{};  // Owner thread only.
bool g_liquidPreparePending{};  // Owner thread only.
bool g_itemStateRefreshPending{};  // Owner thread only.
bool g_liquidTimerArmed{};  // Owner thread only.
UINT g_discoverMessage{};
enum class DockTheme { Native, Glass, Frosted, Liquid };
DockTheme g_theme{};
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
    ULONGLONG liquidShrinkReadyAt{};
    float liquidShrinkTarget{NAN};
    ULONGLONG liquidRetryAt{};
    unsigned liquidFailures{};
};
struct Root {
    struct StatePlate {
        Controls::Border element{nullptr};
        double originalOpacity{};
        double appliedOpacity{};
        bool opacityOwned{};
        bool opacityWasLocal{};
        ShapeVisual rim{nullptr};
        Visual visual{nullptr};
        CompositionGeometricClip clip{nullptr};
        CompositionRoundedRectangleGeometry clipGeometry{nullptr};
        Media::CompositeTransform stretch{nullptr};
        bool blocked{};
    };
    FrameworkElement frame{nullptr};
    winrt::event_token layoutToken{};
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
    };
    std::vector<Source> sources;
    std::vector<StatePlate> statePlates;
    std::vector<uintptr_t> expressionSources;
    ExpressionAnimation follower{nullptr};
    ExpressionAnimation buttonsFollower{nullptr};
    uintptr_t anchorIdentity{};
    float anchorPrefix{};
    CompositionPropertySet metrics{nullptr};
    double nativeRightOverhang{NAN};
    double hostWidth{};
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
        float lensRects[kMaxButtonLenses][4]{};
        float lensTints[kMaxButtonLenses][4]{};
        float lensRadii[kMaxButtonLenses][4]{};
        uint32_t lensCount{};
        float lensRefraction{};
        float alertTintOpacity{};
        float lensPadding{};
    };
    static_assert(sizeof(LensConstants) % 16 == 0);
    // Pins the cbuffer arithmetic the HLSL side relies on: a 64-byte header,
    // three float4 arrays, then four trailing scalars. Adding a member without
    // accounting for it here would silently shift every lens the shader reads.
    static_assert(sizeof(LensConstants) ==
                  64 + 3 * 16 * kMaxButtonLenses + 16);

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
    uint lensCount;
    float lensRefraction;
    float alertTintOpacity;
    float lensPadding;
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
float roundedDistanceForCorners(float2 p, float2 size, float4 radii) {
    float cornerRadius = p.x < 0
        ? (p.y < 0 ? radii.x : radii.w)
        : (p.y < 0 ? radii.y : radii.z);
    return roundedDistanceFor(
        p, size, min(cornerRadius, min(size.x, size.y)));
}
float opticalDistanceForCorners(float2 p, float2 size, float4 radii) {
    float cornerRadius = p.x < 0
        ? (p.y < 0 ? radii.x : radii.w)
        : (p.y < 0 ? radii.y : radii.z);
    return opticalDistanceFor(
        p, size, min(cornerRadius, min(size.x, size.y)));
}
float bevelHeightFor(float inside, float depthRadius) {
    if (inside <= 0) return 0;
    if (inside >= depthRadius) return depthRadius;
    return sqrt(inside * (2 * depthRadius - inside));
}
float3 blurredColor(float2 pixel) {
    return blurredBackdrop.SampleLevel(linearClamp, pixel / imageSize, 0).rgb;
}
float4 ps(VertexOutput input) : SV_Target {
    float2 pixel = input.position.xy;
    float2 p = pixel - center;
    float distance = roundedDistance(p);
    if (distance > 0) return 0;
    float4 original = backdrop.SampleLevel(linearClamp, pixel / imageSize, 0);
    if (original.a <= 0.001) return 0;

    // Biconvex pill profile adapted from ybouane/liquidglass (MIT):
    // https://github.com/ybouane/liquidglass. The curved height field creates
    // the reference's raised glass distortion across the whole surface.
    float inside = -distance;
    float e = 2;
    float opticalInside = -opticalDistance(p);
    float lensZ = min(zRadius, min(halfSize.x, halfSize.y));
    float hCenter = bevelHeightFor(opticalInside, lensZ);
    float2 heightGradient = float2(
        bevelHeightFor(-opticalDistance(p + float2(e, 0)), lensZ) -
            bevelHeightFor(-opticalDistance(p - float2(e, 0)), lensZ),
        bevelHeightFor(-opticalDistance(p + float2(0, e)), lensZ) -
            bevelHeightFor(-opticalDistance(p - float2(0, e)), lensZ)) / (2 * e);
    float3 normal = normalize(float3(-heightGradient, 1));
    float depth = smoothstep(0, lensZ, opticalInside);
    float refractivePower = 1 - 1 / 1.5;
    float normalizedThickness = hCenter / max(lensZ, 0.5);
    float2 shift = heightGradient * refractivePower *
        (2 + normalizedThickness * 0.5) * refraction * 30;
    shift += -p / max(halfSize, 1) * refraction * 4 * depth;

    float maxDepth = min(halfSize.x, halfSize.y);
    float edge = smoothstep(maxDepth * 0.35, 0, inside);
    float chromaPixels = chroma * 36 * (edge * 0.7 + 0.3);
    float2 chromaShift = normal.xy * chromaPixels;
    float2 samplePixel = pixel + shift;
    float3 sharp;
    sharp.r = backdrop.SampleLevel(linearClamp,
        (samplePixel + chromaShift) / imageSize, 0).r;
    sharp.g = backdrop.SampleLevel(linearClamp,
        samplePixel / imageSize, 0).g;
    sharp.b = backdrop.SampleLevel(linearClamp,
        (samplePixel - chromaShift) / imageSize, 0).b;
    float3 color = sharp;
    if (blurAmount > 0.001)
        color = lerp(sharp, blurredColor(samplePixel),
                     1 - edge * 0.15);
    color *= (1 + brightness) * (1 + 0.06 * depth);

    float fresnelAmount = pow(1 - abs(normal.z), 4) * fresnel;
    float3 view = float3(0, 0, 1);
    float highlight = pow(saturate(dot(normal,
        normalize(normalize(float3(0.4, 0.7, 1)) + view))), 90);
    highlight += pow(saturate(dot(normal,
        normalize(normalize(float3(-0.3, -0.5, 1)) + view))), 50) * 0.3;
    color += highlight * specular;

    float innerStroke = smoothstep(-2.5, -1.5, distance) *
                        (1 - smoothstep(-1, 0, distance));
    float topBias = 0.5 + 0.5 * (-p.y / halfSize.y);
    innerStroke *= 0.4 + 0.6 * topBias;
    float rim = edge * edgeHighlight * 0.22;
    float innerGlow = smoothstep(5, 0, inside) * edgeHighlight * 0.15;
    float environment = (normal.y * 0.5 + 0.5) * fresnelAmount * 0.08;
    color += rim + innerGlow + innerStroke * edgeHighlight * 0.55 + environment;
    color = lerp(color, 1, fresnelAmount * 0.2);

    // A second, smaller lens per button over the shared dock capture. Hover stays
    // the native animated plate with reduced opacity, and the alert tint applies
    // only to the lenses flagged for it in w.
    for (uint i = 0; i < min(lensCount, BUTTON_LENS_CAP); ++i) {
        float4 rect = lensRects[i];
        float2 itemPoint = pixel - rect.xy;
        float4 itemRadii = lensRadii[i];
        float itemDistance = roundedDistanceForCorners(
            itemPoint, rect.zw, itemRadii);
        if (itemDistance > 0) continue;
        float itemInside = -itemDistance;
        float itemZ = min(zRadius * 0.65, min(rect.z, rect.w));
        float2 itemGradient = float2(
            bevelHeightFor(-opticalDistanceForCorners(itemPoint + float2(2, 0), rect.zw, itemRadii), itemZ) -
                bevelHeightFor(-opticalDistanceForCorners(itemPoint - float2(2, 0), rect.zw, itemRadii), itemZ),
            bevelHeightFor(-opticalDistanceForCorners(itemPoint + float2(0, 2), rect.zw, itemRadii), itemZ) -
                bevelHeightFor(-opticalDistanceForCorners(itemPoint - float2(0, 2), rect.zw, itemRadii), itemZ)) / 4;
        float itemDepth = smoothstep(0, max(itemZ, 0.5), itemInside);
        float2 itemShift = itemGradient * (1 - 1 / 1.5) *
            lensRefraction * 18;
        itemShift += -itemPoint / max(rect.zw, 1) *
            lensRefraction * 3 * itemDepth;
        float3 itemGlass = blurAmount > 0.001
            ? blurredColor(pixel + itemShift)
            : backdrop.SampleLevel(
                  linearClamp, (pixel + itemShift) / imageSize, 0).rgb;
        itemGlass *= (1 + brightness) * (1 + 0.08 * itemDepth);
        float itemEdge = smoothstep(min(rect.z, rect.w) * 0.45, 0, itemInside);
        float itemMask = 1 - smoothstep(-1.5, 0.5, itemDistance);
        color = lerp(color, itemGlass, itemMask * 0.72);
        color = lerp(color, lensTints[i].rgb,
                     itemMask * alertTintOpacity * lensTints[i].a);
        float itemInnerStroke = smoothstep(-2.5, -1.5, itemDistance) *
            (1 - smoothstep(-1, 0, itemDistance));
        float itemTopBias = 0.5 + 0.5 * (-itemPoint.y / rect.w);
        itemInnerStroke *= 0.4 + 0.6 * itemTopBias;
        float itemRim = itemEdge * edgeHighlight * 0.22;
        float itemInnerGlow = smoothstep(5, 0, itemInside) *
                              edgeHighlight * 0.15;
        color += itemMask * (itemRim + itemInnerGlow +
            itemInnerStroke * edgeHighlight * 0.55);
    }

    float mask = 1 - smoothstep(-1.5, 0.5, distance);
    float alpha = mask * opacity * original.a;
    return float4(color * alpha, alpha);
}
)";

    std::mutex mutex;
    bool running{};
    bool geometryReady{};
    HWND taskbar{};
    RECT taskbarRect{};
    UINT width{};
    UINT height{};
    UINT blurWidth{};
    UINT blurHeight{};
    bool pendingPresent{};
    LensConstants lens{};
    HRESULT failure{S_OK};
    std::chrono::steady_clock::time_point lastPresent{};
    std::vector<std::shared_ptr<Source>> sources;

    winrt::com_ptr<ID3D11Device> device;
    winrt::com_ptr<ID3D11DeviceContext> context;
    direct3d11::IDirect3DDevice graphicsDevice{nullptr};
    winrt::com_ptr<IDXGISwapChain1> swapChain;
    winrt::com_ptr<ID3D11Texture2D> composite;
    winrt::com_ptr<ID3D11RenderTargetView> compositeTarget;
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

            auto compile = [](const char* entry, const char* target) {
                winrt::com_ptr<ID3DBlob> code;
                D3D_SHADER_MACRO macros[]{
                    {"BUTTON_LENS_CAP",
                     UW_STRINGIZE(UW_BUTTON_LENS_CAP)},
                    {nullptr, nullptr}};
                winrt::check_hresult(D3DCompile(
                    kShader, sizeof(kShader) - 1, "liquid-glass", macros,
                    nullptr, entry, target, D3DCOMPILE_ENABLE_STRICTNESS, 0,
                    code.put(), nullptr));
                return code;
            };
            auto vertexCode = compile("vs", "vs_5_0");
            auto pixelCode = compile("ps", "ps_5_0");
            auto copyCode = compile("copyPs", "ps_5_0");
            auto blurCode = compile("boxBlurPs", "ps_5_0");
            winrt::check_hresult(device->CreateVertexShader(
                vertexCode->GetBufferPointer(), vertexCode->GetBufferSize(),
                nullptr, vertexShader.put()));
            winrt::check_hresult(device->CreatePixelShader(
                pixelCode->GetBufferPointer(), pixelCode->GetBufferSize(),
                nullptr, pixelShader.put()));
            winrt::check_hresult(device->CreatePixelShader(
                copyCode->GetBufferPointer(), copyCode->GetBufferSize(),
                nullptr, copyPixelShader.put()));
            winrt::check_hresult(device->CreatePixelShader(
                blurCode->GetBufferPointer(), blurCode->GetBufferSize(),
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
            winrt::check_hresult(factory->CreateSwapChainForComposition(
                device.get(), &swapDescription, nullptr, swapChain.put()));
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
            HRESULT hr = swapChain->ResizeBuffers(
                2, pixelWidth, pixelHeight, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
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
        if (FAILED(device->CreateTexture2D(
                &description, nullptr, composite.put())) ||
            FAILED(device->CreateRenderTargetView(
                composite.get(), nullptr, compositeTarget.put())) ||
            FAILED(device->CreateShaderResourceView(
                composite.get(), nullptr, compositeView.put()))) return false;
        // Blur at half resolution: the same box radii cover twice the backdrop
        // distance while each pass shades one quarter as many pixels.
        description.Width = std::max(1u, pixelWidth / 2);
        description.Height = std::max(1u, pixelHeight / 2);
        for (int i = 0; i < 2; ++i) {
            if (FAILED(device->CreateTexture2D(
                    &description, nullptr, blurTextures[i].put())) ||
                FAILED(device->CreateRenderTargetView(
                    blurTextures[i].get(), nullptr, blurTargets[i].put())) ||
                FAILED(device->CreateShaderResourceView(
                    blurTextures[i].get(), nullptr, blurViews[i].put())))
                return false;
        }
        blurWidth = description.Width;
        blurHeight = description.Height;
        width = pixelWidth;
        height = pixelHeight;
        return true;
    }

    // imageSize is filled in by RenderLocked, so the caller's copy always
    // differs from the stored one; compare everything else.
    static bool SameLensInput(LensConstants a, LensConstants b) {
        a.imageSize[0] = b.imageSize[0] = 0;
        a.imageSize[1] = b.imageSize[1] = 0;
        return memcmp(&a, &b, sizeof(a)) == 0;
    }

    bool UpdateGeometry(RECT const& taskbarBounds, UINT pixelWidth,
                        UINT pixelHeight, LensConstants const& newLens) {
        std::lock_guard lock(mutex);
        if (!running || FAILED(failure) ||
            !ResizeLocked(pixelWidth, pixelHeight))
            return false;
        bool changed = !geometryReady ||
                       !EqualRect(&taskbarRect, &taskbarBounds) ||
                       !SameLensInput(lens, newLens);
        taskbarRect = taskbarBounds;
        lens = newLens;
        geometryReady = true;
        if (changed || pendingPresent) PresentLocked();
        return true;
    }

    // Capture frames are change-driven, so a geometry-only change (dock resize,
    // a new attention lens) reaches the swap chain only if we present it. One
    // present per refresh interval: 14ms would let this outrun a 60Hz compositor
    // and block the taskbar UI thread on buffer release. A throttled present is
    // owed, never dropped - the next UpdateGeometry or timer tick drains it.
    void PresentLocked() {
        auto now = std::chrono::steady_clock::now();
        if (now - lastPresent < std::chrono::milliseconds(17)) {
            pendingPresent = true;
            return;
        }
        RenderLocked();
        lastPresent = now;
    }

    void FlushPendingPresent() {
        std::lock_guard lock(mutex);
        if (running && !FAILED(failure) && geometryReady && pendingPresent)
            PresentLocked();
    }

    float CurrentHalfWidth() {
        std::lock_guard lock(mutex);
        return geometryReady ? lens.halfSize[0] : NAN;
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
            auto size = source->item.Size();
            if (size.Width <= 0 || size.Height <= 0) return nullptr;
            source->pool =
                capture::Direct3D11CaptureFramePool::CreateFreeThreaded(
                    graphicsDevice,
                    directx::DirectXPixelFormat::B8G8R8A8UIntNormalized,
                    2, size);
            std::weak_ptr<LiquidRenderer> weak = shared_from_this();
            source->frameToken = source->pool.FrameArrived(
                [weak, window](capture::Direct3D11CaptureFramePool const& sender,
                               winrt::Windows::Foundation::IInspectable const&) {
                    ActiveScope active;
                    if (g_stopping) return;
                    if (auto renderer = weak.lock())
                        renderer->OnFrame(window, sender);
                });
            source->session = source->pool.CreateCaptureSession(source->item);
            try { source->session.IsBorderRequired(false); } catch (...) {}
            try { source->session.IsCursorCaptureEnabled(false); } catch (...) {}
            source->session.StartCapture();
            Wh_Log(L"[UWLIQUID] capture started hwnd=%p size=%dx%d",
                   window, size.Width, size.Height);
            return source;
        } catch (...) {
            Wh_Log(L"[UWLIQUID] capture start failed hwnd=%p hr=%08X", window,
                   winrt::to_hresult().value);
            return nullptr;
        }
    }

    static void CloseSource(std::shared_ptr<Source> const& source) {
        if (!source) return;
        try {
            if (source->pool) source->pool.FrameArrived(source->frameToken);
        } catch (...) {}
        try { if (source->session) source->session.Close(); } catch (...) {}
        try { if (source->pool) source->pool.Close(); } catch (...) {}
    }

    void UpdateSources() {
        auto candidates = CandidateWindows();
        std::vector<std::shared_ptr<Source>> current;
        {
            std::lock_guard lock(mutex);
            if (!running) return;
            current = sources;
        }
        std::vector<std::shared_ptr<Source>> next;
        for (auto const& [window, rect] : candidates) {
            auto found = std::find_if(current.begin(), current.end(),
                [&](auto const& source) { return source->window == window; });
            if (found != current.end()) {
                {
                    std::lock_guard lock(mutex);
                    (*found)->rect = rect;
                }
                next.push_back(*found);
            } else if (auto source = CreateSource(window, rect)) {
                next.push_back(std::move(source));
            }
        }
        std::vector<std::shared_ptr<Source>> removed;
        {
            std::lock_guard lock(mutex);
            if (!running) {
                removed = std::move(next);
            } else {
                for (auto const& source : sources)
                    if (std::find(next.begin(), next.end(), source) == next.end())
                        removed.push_back(source);
                sources = std::move(next);
            }
        }
        for (auto const& source : removed) CloseSource(source);
    }

    void OnFrame(HWND window,
                 capture::Direct3D11CaptureFramePool const& sender) {
        capture::Direct3D11CaptureFrame frame{nullptr};
        try {
            frame = sender.TryGetNextFrame();
            if (!frame) return;
            auto frameSurface = frame.Surface();
            winrt::com_ptr<IDirect3DDxgiInterfaceAccess> access;
            winrt::check_hresult(
                reinterpret_cast<IUnknown*>(winrt::get_abi(frameSurface))
                    ->QueryInterface(kIID_IDirect3DDxgiInterfaceAccess,
                                     access.put_void()));
            winrt::com_ptr<ID3D11Texture2D> texture;
            winrt::check_hresult(access->GetInterface(
                __uuidof(ID3D11Texture2D), texture.put_void()));
            D3D11_TEXTURE2D_DESC description{};
            texture->GetDesc(&description);
            RECT currentRect{};
            GetWindowRect(window, &currentRect);
            std::lock_guard lock(mutex);
            if (!running) return;
            auto found = std::find_if(sources.begin(), sources.end(),
                [&](auto const& source) { return source->window == window; });
            if (found == sources.end()) return;
            auto& source = **found;
            D3D11_TEXTURE2D_DESC latestDescription{};
            if (source.latest) source.latest->GetDesc(&latestDescription);
            if (!source.latest || latestDescription.Width != description.Width ||
                latestDescription.Height != description.Height ||
                latestDescription.Format != description.Format) {
                source.latestView = nullptr;
                description.Usage = D3D11_USAGE_DEFAULT;
                description.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                description.CPUAccessFlags = 0;
                description.MiscFlags = 0;
                description.MipLevels = 1;
                description.ArraySize = 1;
                winrt::check_hresult(device->CreateTexture2D(
                    &description, nullptr, source.latest.put()));
                winrt::check_hresult(device->CreateShaderResourceView(
                    source.latest.get(), nullptr, source.latestView.put()));
            }
            context->CopyResource(source.latest.get(), texture.get());
            source.rect = currentRect;
            auto now = std::chrono::steady_clock::now();
            if (now - lastPresent >= std::chrono::milliseconds(14)) {
                RenderLocked();
                lastPresent = now;
            } else {
                pendingPresent = true;
            }
        } catch (...) {
            HRESULT error = winrt::to_hresult().value;
            HRESULT removed = device ? device->GetDeviceRemovedReason() : S_OK;
            if (FAILED(removed)) {
                std::lock_guard lock(mutex);
                failure = removed;
            }
            Wh_Log(L"[UWLIQUID] frame failed hwnd=%p hr=%08X removed=%08X",
                   window, error, removed);
        }
        try { if (frame) frame.Close(); } catch (...) {}
    }

    void RenderLocked() {
        if (!running || !composite || !swapChain || width == 0 || height == 0)
            return;
        pendingPresent = false;
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
            if (!source.latestView) continue;
            RECT intersection{};
            if (!IntersectRect(&intersection, &source.rect, &taskbarRect)) continue;
            int sourceWidth = source.rect.right - source.rect.left;
            int sourceHeight = source.rect.bottom - source.rect.top;
            if (sourceWidth <= 0 || sourceHeight <= 0) continue;
            CopyConstants copy{};
            copy.offset[0] = float(intersection.left - source.rect.left) /
                             sourceWidth;
            copy.offset[1] = float(intersection.top - source.rect.top) /
                             sourceHeight;
            copy.scale[0] = float(intersection.right - intersection.left) /
                            sourceWidth;
            copy.scale[1] = float(intersection.bottom - intersection.top) /
                            sourceHeight;
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

        winrt::com_ptr<ID3D11Texture2D> backBuffer;
        HRESULT hr = swapChain->GetBuffer(
            0, __uuidof(ID3D11Texture2D), backBuffer.put_void());
        if (FAILED(hr)) {
            failure = hr;
            return;
        }
        winrt::com_ptr<ID3D11RenderTargetView> backTarget;
        hr = device->CreateRenderTargetView(
            backBuffer.get(), nullptr, backTarget.put());
        if (FAILED(hr)) {
            failure = hr;
            return;
        }
        context->ClearRenderTargetView(backTarget.get(), clear);
        lens.imageSize[0] = static_cast<float>(width);
        lens.imageSize[1] = static_cast<float>(height);
        context->UpdateSubresource(constants.get(), 0, nullptr, &lens, 0, 0);
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
        {
            std::lock_guard lock(mutex);
            if (!running && sources.empty()) return;
            running = false;
            oldSources = std::move(sources);
        }
        for (auto const& source : oldSources) CloseSource(source);
    }
};

bool HoldLiquidGeometry(Root const& root, LiquidRenderer const& renderer) {
    return renderer.geometryReady &&
           std::any_of(root.sources.begin(), root.sources.end(),
                       [](auto const& source) {
                           return source.life == Root::Source::Life::Hiding;
                       });
}

bool DeferLiquidShrink(float currentHalfWidth, float nextHalfWidth,
                       ULONGLONG now, ULONGLONG& readyAt,
                       float& pendingTarget) {
    if (!std::isfinite(currentHalfWidth) ||
        nextHalfWidth >= currentHalfWidth - 0.25f) {
        readyAt = 0;
        pendingTarget = NAN;
        return false;
    }
    if (!std::isfinite(pendingTarget) ||
        std::abs(pendingTarget - nextHalfWidth) > 0.25f) {
        pendingTarget = nextHalfWidth;
        readyAt = now + kLiquidShrinkDelay;
    }
    if (now < readyAt) return true;
    readyAt = 0;
    pendingTarget = NAN;
    return false;
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

bool PublishTray(Root& root, double host, double shift) {
    // ponytail: UWP lacks WinUI's ContentIslandEnvironment HWND accessor. Match
    // one native taskbar by owner thread, full width and raster scale; equal-size
    // candidates or multiple XAML roots for that HWND must remain unpublished.
    double scale = root.frame.XamlRoot().RasterizationScale();
    HWND match{}, window{};
    for (int i = 0; i < 4096 && (window = FindWindowEx(nullptr, window, nullptr, nullptr)); ++i) {
        if ((!ClassIs(window, L"Shell_TrayWnd") && !ClassIs(window, L"Shell_SecondaryTrayWnd")) ||
            GetWindowThreadProcessId(window, nullptr) != g_ownerThread) continue;
        RECT rect{};
        if (!GetWindowRect(window, &rect) || std::abs(GetDpiForWindow(window) / 96.0 - scale) > 0.01 ||
            std::abs(rect.right - rect.left - host * scale) > 2) continue;
        if (match) { match = nullptr; break; }
        match = window;
    }
    for (auto& other : g_roots) {
        if (&other != &root && other.taskbar && other.taskbar == match) {
            WithdrawTray(other);
            match = nullptr;
            break;
        }
    }
    if (match != root.taskbar) {
        WithdrawTray(root);
        root.taskbar = nullptr;
    }
    if (!match || !std::isfinite(scale * shift) || std::abs(scale * shift) > INT_MAX) {
        WithdrawTray(root);
        root.taskbar = nullptr;
        if (!root.mappingReported) Wh_Log(L"[UWPANEL] no unambiguous taskbar mapping frame=%p host=%.3f scale=%.3f",
                                          winrt::get_abi(root.frame), host, scale);
        root.mappingReported = true;
        return false;
    }
    root.mappingReported = false;
    root.taskbar = match;
    HANDLE value = EncodeShift(static_cast<int>(std::lround(scale * shift)));
    if (value == root.publishedShift && GetProp(match, kTrayShift) == value &&
        (!root.follower || GetProp(match, kTrayDispatcher) == g_dispatch))
        return true;
    if (SetProp(match, kTrayShift, value)) {
        if (root.follower && !SetProp(match, kTrayDispatcher, g_dispatch)) {
            root.publishedShift = value;
            WithdrawTray(root);
            root.taskbar = nullptr;
            return false;
        }
        root.publishedShift = value;
        return true;
    } else {
        WithdrawTray(root);
        root.taskbar = nullptr;
        Wh_Log(L"[UWPANEL] publication failed taskbar=%p error=%lu", match, GetLastError());
        return false;
    }
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

void RestoreStatePlate(Root::StatePlate& state) {
    if (!state.element) return;
    try {
        bool transformMatch =
            state.stretch && state.element.RenderTransform() == state.stretch;
        bool clipMatch =
            state.clip && state.visual && state.visual.Clip() == state.clip;
        bool opacityMatch =
            state.opacityOwned &&
            std::abs(state.element.Opacity() - state.appliedOpacity) < 0.001;
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
        if (opacityMatch) {
            if (state.opacityWasLocal)
                state.element.Opacity(state.originalOpacity);
            else
                state.element.ClearValue(UIElement::OpacityProperty());
        }
    } catch (...) {
        Wh_Log(L"[UWITEM] state plate restore failed hr=%08X",
               winrt::to_hresult().value);
    }
    state.opacityOwned = false;
    state.opacityWasLocal = false;
    state.rim = nullptr;
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

// Replicates the native rim, measured off a live taskbar: a gradient band that is
// black along the bottom edge in light theme and white along the top in dark. The
// native brush spans an absolute 3 DIP, but only the two horizontal edges of a
// 1-DIP stroke ever sample it, so a relative top-to-bottom ramp carries the same
// cue. Alphas are divided by the plate dim so the result lands at native strength
// after the element opacity multiplies it.
ShapeVisual ButtonRim(CompositionRoundedRectangleGeometry const& outline,
                      bool dark) {
    auto compositor = outline.Compositor();
    auto visual = compositor.CreateShapeVisual();
    visual.RelativeSizeAdjustment({1, 1});
    auto lift = [](unsigned alpha) {
        return static_cast<uint8_t>(std::min<unsigned>(
            255, static_cast<unsigned>(std::lround(alpha / kPlateDimScale))));
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
        // Dim the whole element. Mirroring Windows' per-state Background instead
        // was tried and reverted: the resting value comes from the template and
        // InactiveNormal sets no brush, so a local write outranks both and the
        // hover highlight never goes away.
        if (!state.opacityOwned) {
            state.originalOpacity = plate.Opacity();
            // A template or visual-state value must be handed back by clearing
            // ours, not by pinning the number we happened to read.
            state.opacityWasLocal =
                plate.ReadLocalValue(UIElement::OpacityProperty()) !=
                DependencyProperty::UnsetValue();
            state.appliedOpacity =
                g_theme == DockTheme::Native
                    ? state.originalOpacity
                    : state.originalOpacity * kPlateDimScale;
            state.opacityOwned = true;
        }
        plate.Opacity(state.appliedOpacity);
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
        if (g_keepButtonRim && !state.rim &&
            plate.Name() == L"BackgroundElement" &&
            !Hosting::ElementCompositionPreview::GetElementChildVisual(plate)) {
            state.rim = ButtonRim(state.clipGeometry,
                                  plate.ActualTheme() == ElementTheme::Dark);
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
    dock.liquidShrinkReadyAt = 0;
    dock.liquidShrinkTarget = NAN;
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

void PrepareGlass(OwnedDock& dock) {
    if (g_theme == DockTheme::Native || dock.materialBlocked) return;
    HIGHCONTRAST contrast{};
    contrast.cbSize = sizeof(contrast);
    if (!SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0) ||
        (contrast.dwFlags & HCF_HIGHCONTRASTON)) {
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
    dock.liquidShrinkReadyAt = 0;
    dock.liquidShrinkTarget = NAN;
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

// SetTimer on a live id restarts the elapsed time, so re-arming from every layout
// pass starved the poll that drives UpdateSources, the failure check and the
// deferred present. Arm once and leave it running.
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

void AddButtonLenses(Root& root, LiquidRenderer::LensConstants& lens,
                        double scale) {
    size_t dropped = 0;
    for (auto const& source : root.sources) {
        auto element = source.element.get();
        if (!element) continue;
        // Attention always earns a lens. Every other button only when the
        // per-button layer is switched on.
        if (!source.requestingAttention && !g_buttonLiquidLayer) continue;
        if (lens.lensCount == kMaxButtonLenses) {
            ++dropped;
            continue;
        }
        std::vector<Controls::Border> plates;
        CollectStatePlates(element, plates, 4);
        auto plate = std::find_if(plates.begin(), plates.end(),
                                  [](auto const& candidate) {
                                      return candidate.Name() ==
                                             L"BackgroundElement";
                                  });
        if (plate == plates.end()) continue;
        auto point = plate->TransformToVisual(root.dock.background)
                         .TransformPoint({0, 0});
        double width = plate->ActualWidth();
        double height = plate->ActualHeight();
        if (!std::isfinite(point.X) || !std::isfinite(point.Y) ||
            width <= 2 || height <= 2)
            continue;
        auto index = lens.lensCount++;
        lens.lensRects[index][0] = static_cast<float>(
            (point.X + width / 2) * scale);
        lens.lensRects[index][1] = static_cast<float>(
            (point.Y + height / 2) * scale);
        lens.lensRects[index][2] = static_cast<float>(width * scale / 2);
        lens.lensRects[index][3] = static_cast<float>(height * scale / 2);

        winrt::Windows::UI::Color tint{255, 232, 72, 68};
        if (auto brush = plate->Background().try_as<Media::SolidColorBrush>())
            tint = brush.Color();
        lens.lensTints[index][0] = tint.R / 255.0f;
        lens.lensTints[index][1] = tint.G / 255.0f;
        lens.lensTints[index][2] = tint.B / 255.0f;
        // The fourth component selects the alert tint, so a plain button lens
        // refracts without being coloured.
        lens.lensTints[index][3] =
            source.requestingAttention ? 1.0f : 0.0f;
        // The visible plate is its own rounded rect intersected with our clip, and
        // a clip can only take area away, so the larger radius wins per corner. The
        // lens tracked the native radius alone and so under-rounded whenever
        // buttonCornerRadius exceeded it, which is the default.
        double clipRadius =
            std::min({g_buttonCornerRadius, width / 2, height / 2});
        auto corners = plate->CornerRadius();
        for (auto [slot, corner] : {std::pair{0, corners.TopLeft},
                                    std::pair{1, corners.TopRight},
                                    std::pair{2, corners.BottomRight},
                                    std::pair{3, corners.BottomLeft}})
            lens.lensRadii[index][slot] = static_cast<float>(
                std::max(corner, clipRadius) * scale);
    }
    if (dropped)
        Wh_Log(L"[UWLIQUID] %zu button lenses past the %zu cap", dropped,
               kMaxButtonLenses);
    lens.lensRefraction = lens.optics.refraction * 0.78f;
    lens.alertTintOpacity = 0.22f;
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
    if (dock.liquid && HoldLiquidGeometry(root, *dock.liquid)) return;
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
            renderer->visualRoot.Children().InsertAtTop(dock.highlights);
            dock.liquid = std::move(renderer);
            dock.attachedVisual = dock.liquid->visualRoot;
            Hosting::ElementCompositionPreview::SetElementChildVisual(
                dock.background, dock.attachedVisual);
            created = true;
        }
        double dockLeft = (m.host - m.buttonsWidth - kGap - m.trayWidth) / 2 -
                          kDockPadding - m.backgroundOrigin;
        double dockWidth =
            m.buttonsWidth + kGap + m.trayWidth + 2 * kDockPadding;
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
        if (DeferLiquidShrink(
                dock.liquid->CurrentHalfWidth(), lens.halfSize[0],
                GetTickCount64(), dock.liquidShrinkReadyAt,
                dock.liquidShrinkTarget))
            return;
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
    // ponytail: bound the experiment to 64 realized children; larger trees fall
    // back to native geometry instead of producing an unbounded expression.
    if (count > 64) throw winrt::hresult_error(E_BOUNDS);
    std::erase_if(root.sources, [](auto& source) {
        return !source.element.get();
    });
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
            root.sources.push_back({identity, winrt::make_weak(child), visual, Root::Source::Life::Active});
            acquired = true;
            source = root.sources.end() - 1;
            source->requestingAttention =
                g_theme == DockTheme::Liquid && HasAttentionState(child);
        }
        if (IncludeSource(eligible, source->life)) selected.push_back(identity);
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
    try { StopFollower(root); }
    catch (...) { Wh_Log(L"[UWFOLLOW] stop failed hr=%08X", winrt::to_hresult().value); }
    // Attempt both even if one XAML object is already closed.
    for (auto* owned : {&root.buttonsTranslation, &root.trayTranslation}) {
        try { Restore(*owned); }
        catch (...) { Wh_Log(L"[UWPOS] restoration-error element=%p", winrt::get_abi(owned->element)); }
    }
    try { Restore(root.dock); }
    catch (...) { Wh_Log(L"[UWDOCK] restoration-error"); }
}

// ---- The layout pass ----------------------------------------------------
void Refresh(Root& root) {
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
    if (!parent || !repeater || !start || !tray || !background) {
        if (!root.fallbackReported) Wh_Log(L"[UWPOS] native fallback: required tree element missing");
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
    // The first publication resolves this XAML root to its native taskbar so
    // the dock can choose the monitor-facing edge before creating its clip.
    if (!PublishTray(root, host, target.trayX)) {
        RestoreRoot(root);
        return;
    }
    // PublishTray has resolved root.taskbar, so the orientation-dependent insets
    // are settled here and every later stage reads this one copy.
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
    if (!PublishTray(root, host, target.trayX)) {
        RestoreRoot(root);
        return;
    }
    PrepareLiquid(root, m);
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
            // A native transition transform, an anchor that moved mid-flight and
            // more than 64 realized children all clear on a later pass, so these
            // retry. Ownership changes latch separately, above.
            bool latch = ++root.refreshFailures >= kRefreshRetryLimit;
            if (latch) root.blocked = true;
            if (root.refreshFailures == 1 || latch)
                Wh_Log(L"[UWPOS] refresh-error hr=%08X attempt=%u%s", error.value,
                       root.refreshFailures,
                       latch ? L"; root blocked, re-enable to retry" : L"; retrying");
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

LRESULT CALLBACK DispatchProc(HWND window, UINT message, WPARAM wp, LPARAM lp) {
    if (message == kQueryTrayMessage) return QueryCurrentTray(reinterpret_cast<HWND>(wp));
    if (message == kPrepareLiquidMessage) {
        g_liquidPreparePending = false;
        ActiveScope active;
        if (g_stopping) return 0;
        g_preparingLiquid = true;
        struct Unprepare { ~Unprepare() { g_preparingLiquid = false; } } unprepare;
        for (auto& root : g_roots) OnLayout(winrt::make_weak(root.frame));
        return 0;
    }
    if (message == kRefreshItemStatesMessage) {
        g_itemStateRefreshPending = false;
        if (g_stopping) return 0;
        for (auto& root : g_roots) RefreshAttentionStates(root);
        for (auto& root : g_roots) OnLayout(winrt::make_weak(root.frame));
        return 0;
    }
    if (message == WM_TIMER && wp == kLiquidTimerId) {
        bool active = false;
        std::vector<winrt::weak_ref<FrameworkElement>> retries;
        ULONGLONG now = GetTickCount64();
        for (auto& root : g_roots) {
            auto& dock = root.dock;
            if (dock.liquid) {
                active = true;
                HRESULT failure = dock.liquid->Failure();
                if (FAILED(failure)) {
                    Wh_Log(L"[UWLIQUID] renderer lost hr=%08X", failure);
                    StopLiquid(dock);
                    ScheduleLiquidRetry(dock, failure);
                } else {
                    dock.liquid->UpdateSources();
                    dock.liquid->FlushPendingPresent();
                    if (dock.liquidShrinkReadyAt &&
                        now >= dock.liquidShrinkReadyAt)
                        retries.push_back(winrt::make_weak(root.frame));
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
        if (g_panelLocationHook) {
            UnhookWinEvent(g_panelLocationHook);
            g_panelLocationHook = nullptr;
        }
        for (auto& root : g_roots) {
            try { root.frame.LayoutUpdated(root.layoutToken); }
            catch (...) { Wh_Log(L"[UWPOS] unsubscribe-error frame=%p", winrt::get_abi(root.frame)); }
            RestoreRoot(root);
        }
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
                                    HWND_MESSAGE, nullptr, GetModuleHandle(nullptr), nullptr);
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
    for (auto& root : g_roots) if (root.frame == frame) return;
    g_busy = true;
    struct Unbusy { ~Unbusy() { g_busy = false; } } unbusy;
    Root root;
    root.frame = frame;
    auto weak = winrt::make_weak(frame);
    root.layoutToken = frame.LayoutUpdated([weak](auto&&, auto&&) { OnLayout(weak); });
    try { g_roots.push_back(root); }
    catch (...) { frame.LayoutUpdated(root.layoutToken); throw; }
    Wh_Log(L"[UWPOS] watching frame=%p thread=%lu", winrt::get_abi(frame), GetCurrentThreadId());
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
    HMODULE module = GetModuleHandle(L"taskbar.dll");
    if (!module) return false;
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
    auto module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) return FALSE;
    if (!ResolveStartupSymbols()) return FALSE;
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
    WNDCLASS wc{};
    wc.lpfnWndProc = DispatchProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = kWindowClass;
    g_windowClass = RegisterClass(&wc);
    if (!g_windowClass) return FALSE;
    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
        UnregisterClass(kWindowClass, wc.hInstance);
        g_windowClass = 0;
        return FALSE;
    }
    Wh_Log(L"[UWPOS] initialized v" WH_MOD_VERSION
           L"; screenshot-safe liquid glass, native animation follower, gap=12 theme=%d heightMethod=automatic",
           static_cast<int>(g_theme));
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

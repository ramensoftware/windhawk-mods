// ==WindhawkMod==
// @id              windows-11-better-ultrawide-taskbar
// @name            Windows 11 Better Ultrawide Taskbar
// @description     Centers taskbar buttons and the system tray in a rounded dock while preserving native button animations
// @version         0.3.86
// @author          Molko
// @github          https://github.com/roeseth
// @include         explorer.exe
// @include         ShellHost.exe
// @include         ShellExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -ldwmapi -lwindowsapp -ld3d11 -ldxgi -ld3dcompiler -lgdi32 -luser32 -lkernel32 -lshell32 -luuid -lCoreMessaging -lwindowscodecs -lshcore -ldxguid -lwinhttp -ld2d1
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// Portions of the liquid-glass profile are adapted from @ybouane/liquidglass.
// The Gaussian box-radius calculation is adapted from FastGaussianBlur:
// Copyright (C) 2017-2022 Basile Fraboni
// Copyright (C) 2014 Ivan Kutskir
//
// Layered refraction adapted from dashersw/liquid-glass-js:
// Copyright (c) 2025 Armagan Amcalar
// Bevel refraction, tint and interaction adapted from naughtyduk/liquidGL:
// Copyright (c) NaughtyDuk
// These upstream works are used under the MIT License:
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

Choose **Dock theme** in the taskbar settings editor: Windows, Glass, Frosted glass, or
Liquid glass. Liquid glass captures only desktop and app windows below the
taskbar and refracts them with a GPU shader. The taskbar remains visible in
screenshots and screen sharing. All themes follow the same animated dock
outline. Liquid keeps the native fill transparent while capture or GPU setup
is unavailable, and transient GPU failures are retried automatically. High
contrast retains the Windows material. **Save & apply** updates the taskbar.

**Text color** can retain Windows' color, force light or dark text, or
adapt each text element from its local composed background. Adaptive color uses
a rolling, time-weighted brightness average to reduce switching on animated
wallpapers. **Adaptive averaging (seconds)** defaults to 2 seconds; increase it
for steadier colors, or use 0 for immediate sampling. It is saved as
`windowSeconds` in `[settings.adaptive]` and also applies to adaptive tray artwork.
Color changes fade between colors. High contrast always retains Windows' colors.

**Custom tray icons** lists current app icons, including overflow icons.
**Enable custom tray icons** controls grayscale and all icon replacements.
Turning it off restores original icons without deleting your saved choices.
The toggle is saved as `enabled` in `[settings.tray]` and defaults to on.
**Use grayscale** removes their color while preserving transparency and live
icon updates. **Choose image** assigns a local PNG, ICO, JPEG, BMP, GIF, or TIFF
to an icon; **Use original** removes that assignment. Grayscale also applies
to custom images. System glyphs are excluded. Assignments use Windows' icon
identifiers and are saved in `[settings.tray.icons]`. **Choose icon → Dashboard
Icons** searches the [Dashboard Icons](https://dashboardicons.com/icons) catalog,
including selfh.st, LobeHub, and Simple Icons. The image grid labels each source;
available wordmarks appear as separate choices. Icons marked **Auto** follow text
color automatically, using the same background sampling as adaptive text and
switching between cached light/dark variants without network access.
Other icons use their original artwork. Fixed images are saved as a path;
adaptive images are saved as `[lightArtworkPath, darkArtworkPath]`.
Catalogs and PNG assets are cached beside `settings.toml`. Source attribution
and licenses are available through the Dashboard Icons link.

When **Separate System Tray Icons** is installed, the Control Center button's
right-click menu includes **Ultrawide taskbar settings...**. It opens a native
XAML editor with Taskbar, Liquid glass, Tray icons, and Settings UI pages,
and Windows theme colors. A centered, pinned preview updates as you edit, using a sample
desktop and the same liquid-glass shader as the taskbar. It measures the taskbar
on the editor's monitor and shows frame height and rendered app-icon box size
in pixels and DIPs. **Refresh** remeasures after external changes. Unsupported
icon trees use a clearly marked estimate. The samples use real Windows app
icon resources; transparent padding within an icon and Windows materials can
vary. The preview stays centered regardless of taskbar position or screen-edge
distance. **Save & apply** (Ctrl+S) saves to:
`%LOCALAPPDATA%\Windhawk\windows-11-better-ultrawide-taskbar\settings.toml`.
The **Settings UI** page independently controls the window's main-body material
(Native, Mica, Acrylic, or Liquid glass), body blur, and floating panels' blur,
refraction, depth, and highlights. Panels sample the body beneath them. These values live
in `[settings.ui]`; saving only UI appearance does not rebuild the taskbar.
The full-height sidebar contains the app header. Native follows the Windows theme.
Mica uses Windows' blurred wallpaper backdrop, Acrylic uses the native Acrylic brush,
and Liquid glass refracts the desktop backdrop. The renderer applies the continuous
window mask to the material so its corners remain smooth.
The body-blur control applies to Liquid glass. Glass refraction,
depth, and highlights apply to the panels and a Liquid glass body.
Floating panels sample the rendered body layer underneath them, with no panel
shadows. The body is isolated from the panels and controls to prevent feedback.
Decorative outlines are removed; keyboard focus remains visible.
Panel controls have clear fills, and the window and glass panels use continuous
superellipse corners (44 DIP for the window, 32 DIP for panels), with antialiased
coverage. The XAML host blends edge coverage against the raw desktop backdrop;
the opaque body stays custom-rendered. Maximized windows keep square corners.
Settings UI glass also exposes refraction models, surface profiles, Snell thickness
and refractive index, magnification, distortion, chromatic aberration, lighting,
Fresnel reflection, brightness, saturation, and panel opacity. These controls affect
the floating panels and, when selected, the Liquid glass body independently of the taskbar.
This is the mod's only settings file; Windhawk has no separate customization
values. The first click creates a missing file from built-in defaults.
The `[settings]`, `[settings.button]`, `[settings.liquid]`, `[settings.tray]`,
`[settings.adaptive]`, and `[settings.ui]` tables hold the
settings; missing or invalid fields use built-in defaults. The settings reader supports
single-line strings, booleans, decimal numbers, named tables, `#` comments,
and double-quoted light/dark path pairs for tray assignments.
The editor applies saved changes immediately. **Reload** discards unsaved edits
and rereads the file; **Open TOML** opens it in Notepad. Externally edited files
are not overwritten by an older editor snapshot. Reload the mod after editing
the file outside the GUI, or use **Reload** then **Save & apply**.
To reset all settings, remove the file and reload the mod. Opening the editor
never overwrites an existing file.

Expert optics are grouped behind disclosure controls. **Preview Liquid glass**
changes only the draft theme; Save & apply still controls when the taskbar changes.
Invalid numbers and colors show an inline explanation while the preview keeps
the last valid value. The window adapts to Windows text size, using a shorter
preview above wider controls when space or larger text requires it.

Ordinary app-button and tray highlights retain Windows' native colors, brush
transparency and hover/attention animations. The mod does not dim app fills.
When **Give each button its own liquid layer** is enabled, accepted app lenses
replace the native fill; attention lenses use the alert tint.

**Button border** controls app-button outlines. With a liquid lens, the mod
draws its matching rim; otherwise Windows supplies the outline.

**Button corner radius** and **Button vertical padding** also apply when separate
button glass is off, including grouped-window backgrounds. **Tray icons → Tray
highlights** has a separate shared padding and radius for all tray icons. These
geometry controls preserve icon layout and native button hit targets. High
contrast retains native fills.

Under the Liquid glass theme each task button can also become its own piece of
liquid glass inside the dock, carrying the same edge displacement, chromatic split,
specular and fresnel as the dock itself. It is the same shader pass, not a second
capture. Tray items share the dock lens.

Liquid glass includes the reference site's Regular, Frosted, and Dark presets.
Regular is the reference defaults with no backdrop blur; Frosted adds it, and
Dark also dims the backdrop. Choose Custom to use the individual optical
parameters.

Under Custom, **Refraction model** offers the original Biconvex approximation,
**Snell's law**, **Dome magnifier**, **Layered refraction**, and **Bevel lens**. Snell traces a single air-to-glass
refraction toward the background plane; it is not a complete multi-surface lens
simulation. Its **Bezel width**, **Glass thickness**, and **Refractive index**
are independent. An index of 1.00 is neutral; ordinary glass is approximately
1.50. The same decimal value is stored in TOML. The profile construction follows
[Kube's optical explanation](https://kube.io/blog/liquid-glass-css-svg/), evaluated
directly in HLSL rather than using SVG displacement images.

Layered refraction combines a broad edge bend with a narrower curled rim,
adapted from [liquid-glass-js](https://github.com/dashersw/liquid-glass-js).
Bevel lens concentrates distortion along a pronounced bevel, adapted from
[liquidGL](https://github.com/naughtyduk/liquidGL). Both use **Bezel width** and
**Rim refraction**; Layered adds **Rim width**, while Bevel adds **Bevel sharpness**.

All models offer convex circle, convex squircle, concave, and raised-lip surface
profiles. Additional magnification, chromatic aberration, and micro-distortion
can be combined with any model. **Corner boost** strengthens the corners;
**Edge ripple** and **Ripple spacing** add a structured wave along the edge.
**Capture edge softness** eases displaced samples back inside the captured area;
0 retains clamped sampling.

**Lighting** offers Original, Rich specular, or Directional rim. The directional
rim has angle, opacity, and color-saturation controls. Finish controls include
overall saturation and cool glass tint. These effects also apply to enabled
button lenses. **Shadow** controls the dock's opacity, softness, and vertical
offset; its mask follows the native dock animation and cannot extend beyond the
taskbar's window area. The original optical model remains the default, with the
new tint, saturation adjustment, distortion, magnification, and shadow disabled.

**Highlight width** and **Highlight softness** shape the edge light independently
of refraction. Width 0 retains automatic sizing. **Polished edge** controls sharp
detail around a frosted center. **Color dispersion** can follow refraction, so
Snell's neutral index also removes color fringing. **Tint color** accepts a
`#RRGGBB` color; **Tint strength** controls its effect without adding a white fill.

**Fluid response** follows pointer motion and settles when it stops. **Moving
glints** can animate along the edge or follow the pointer. Both default off,
respect Windows' animation preference, and leave controls and click targets in
place. These controls are also available independently under **Settings UI**,
where they apply to the floating panels and to a Liquid glass body.

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
#include <utility>
#include <filesystem>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <set>
#include <map>
#include <array>
#include <span>
#include <optional>
#include <future>
#include <cerrno>
#include <dispatcherqueue.h>
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>
#include <shlobj.h>
#include <shellapi.h>
#include <roapi.h>
#include <robuffer.h>
#include <wincodec.h>
#include <shcore.h>
#include <shobjidl.h>
#include <d2d1effects.h>
#include <d2d1_3.h>
#include <d2d1svg.h>
#include <winhttp.h>
#include <tlhelp32.h>
#include <wtsapi32.h>
#include <d3d11_3.h>
#include <d3dcompiler.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dwmapi.h>
#include <inspectable.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.System.h>

using namespace winrt::Windows::UI::Xaml;
using winrt::Windows::Foundation::Size;
using winrt::Windows::Foundation::Numerics::float3;
using winrt::Windows::Foundation::Numerics::float4;
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
constexpr float kEditorWindowRadius = 44;
constexpr UINT kRestoreMessage = WM_APP + 43;
constexpr UINT kQueryTrayMessage = WM_APP + 44;
constexpr UINT kPrepareLiquidMessage = WM_APP + 45;
constexpr UINT kRefreshItemStatesMessage = WM_APP + 46;
constexpr UINT kApplySettingsMessage = WM_APP + 47;
constexpr UINT kSettingsAppliedMessage = WM_APP + 48;
constexpr UINT kActivateSettingsMessage = WM_APP + 49;
constexpr UINT kQueryPreviewMetricsMessage = WM_APP + 50;
constexpr UINT kPreviewMetricsMessage = WM_APP + 51;
constexpr UINT kQueryTrayCatalogMessage = WM_APP + 52;
constexpr UINT kTrayCatalogMessage = WM_APP + 53;
constexpr UINT kCatalogIconsMessage = WM_APP + 55;
constexpr UINT_PTR kCatalogSearchTimerId = 56;
constexpr UINT kEditorThemeMessage = WM_APP + 57;
constexpr UINT kRefreshPlateFillsMessage = WM_APP + 60;
constexpr UINT_PTR kEditorBackdropTimerId = 58;
constexpr UINT_PTR kDiscoveryTimerId = 59;
constexpr UINT_PTR kLiquidTimerId = 45;
// A transient measurement fails a handful of consecutive layout passes at most;
// an unsupported build fails every one. Tune down if a broken tree costs too many
// tree walks, up if a slow startup transition needs longer to settle.
constexpr unsigned kRefreshRetryLimit = 10;
// One lens per realized task button; UpdateFollower already refuses more than 64.
#define UW_BUTTON_LENS_CAP 64
#define UW_STRINGIZE_IMPL(value) #value
#define UW_STRINGIZE(value) UW_STRINGIZE_IMPL(value)
constexpr size_t kMaxButtonLenses = UW_BUTTON_LENS_CAP;
constexpr wchar_t kWindowClass[] = L"Windhawk.BetterUltrawideTaskbar.2";
std::atomic<bool> g_stopping{};
std::atomic<unsigned> g_active{};
std::atomic<unsigned> g_retiring{};
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
std::set<HWND> g_pendingDiscovery; // Taskbar UI thread only.
enum class DockTheme { Native, Glass, Frosted, Liquid };
enum class FontColorMode { Default, Light, Dark, Adaptive };
DockTheme g_theme{};
FontColorMode g_fontColorMode{};
ULONGLONG g_adaptiveWindowMs{2000};
double g_dockCornerRadius{12};
double g_buttonCornerRadius{12};
double g_buttonVerticalPadding{4};
double g_trayHighlightPadding{8},g_trayHighlightRadius{12};
double g_dockEdgeDistance{4};
bool g_keepButtonRim{true};
bool g_buttonLiquidLayer{};
bool g_grayscaleTrayIcons{};
struct TrayIconFiles {
    std::wstring light, dark;
    bool Adaptive() const { return light != dark; }
    bool empty() const { return light.empty() || dark.empty(); }
    bool operator==(TrayIconFiles const&) const = default;
};
using TrayOverrides = std::map<std::wstring, TrayIconFiles>;
TrayOverrides g_customTrayIcons;

// Holds off the teardown in Wh_ModBeforeUninit while a hook or callback runs.
struct ActiveScope {
    ActiveScope() { ++g_active; }
    ~ActiveScope() { --g_active; }
};
struct RetiringScope {
    RetiringScope() { ++g_retiring; }
    ~RetiringScope() { --g_retiring; }
};

// Local settings are loaded at startup and applied on the taskbar's UI thread.
namespace settingsjson = winrt::Windows::Data::Json;
std::wstring g_settingsPath;
std::string g_appliedSettingsDocument;

std::wstring LocalSettingsPath() {
    PWSTR folder{};
    winrt::check_hresult(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &folder));
    std::wstring result(folder);
    CoTaskMemFree(folder);
    return result + L"\\Windhawk\\windows-11-better-ultrawide-taskbar\\settings.toml";
}

settingsjson::JsonObject ParseSettingsToml(std::string_view text) {
    // ponytail: settings scalars and JSON-compatible path pairs only. Use a full
    // TOML library if settings need general arrays, dates, or multiline strings.
    auto trim = [](std::string_view value) {
        auto begin = value.find_first_not_of(" \t\r");
        if (begin == value.npos) return std::string_view{};
        return value.substr(begin, value.find_last_not_of(" \t\r") - begin + 1);
    };
    auto scalar = [&](std::string_view raw) -> settingsjson::IJsonValue {
        raw = trim(raw);
        if (raw.size() >= 2 && raw.front() == '\'' && raw.back() == '\'')
            return settingsjson::JsonValue::CreateStringValue(winrt::to_hstring(raw.substr(1, raw.size() - 2)));
        std::string value(raw);
        if (!value.empty() && value.front() != '"' && value.front() != '[') {
            for (size_t i = 0; i < value.size(); ++i) {
                if (value[i] == '_' && (i == 0 || i + 1 == value.size() ||
                    value[i - 1] < '0' || value[i - 1] > '9' ||
                    value[i + 1] < '0' || value[i + 1] > '9'))
                    throw std::runtime_error("Invalid number separator");
            }
            std::erase(value, '_');
            if (value.starts_with('+')) value.erase(0, 1);
        }
        settingsjson::JsonValue parsed{nullptr};
        if (!settingsjson::JsonValue::TryParse(winrt::to_hstring(value), parsed))
            throw std::runtime_error("Invalid TOML scalar");
        auto type = parsed.ValueType();
        if (type != settingsjson::JsonValueType::String &&
            type != settingsjson::JsonValueType::Number &&
            type != settingsjson::JsonValueType::Boolean &&
            type != settingsjson::JsonValueType::Array)
            throw std::runtime_error("Unsupported TOML value");
        return parsed;
    };
    settingsjson::JsonObject values;
    std::set<std::string> tables;
    std::string section;
    bool version = false;
    size_t position = 0;
    while (position < text.size()) {
        auto end = text.find('\n', position);
        if (end == text.npos) end = text.size();
        auto line = trim(text.substr(position, end - position));
        position = end + 1;
        char quote = 0;
        bool escape = false;
        size_t equals = line.npos;
        for (size_t i = 0; i < line.size(); ++i) {
            char c = line[i];
            if (escape) { escape = false; continue; }
            if (quote == '"' && c == '\\') { escape = true; continue; }
            if (quote) { if (c == quote) quote = 0; continue; }
            if (c == '"' || c == '\'') quote = c;
            else if (c == '#') { line = trim(line.substr(0, i)); break; }
            else if (c == '=' && equals == line.npos) equals = i;
        }
        if (quote) throw std::runtime_error("Unterminated TOML string");
        if (line.empty()) continue;
        if (line.front() == '[' && line.back() == ']') {
            section = std::string(trim(line.substr(1, line.size() - 2)));
            if ((section != "settings" && !section.starts_with("settings.")) ||
                section.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.-") != section.npos ||
                section.ends_with('.') || section.find("..") != section.npos ||
                !tables.insert(section).second)
                throw std::runtime_error("Invalid or duplicate TOML table");
            continue;
        }
        if (equals == line.npos) throw std::runtime_error("Missing TOML assignment");
        auto rawKey = trim(line.substr(0, equals));
        std::string key;
        if (!rawKey.empty() && (rawKey.front() == '"' || rawKey.front() == '\'')) {
            auto parsed = scalar(rawKey);
            if (parsed.ValueType() != settingsjson::JsonValueType::String)
                throw std::runtime_error("Invalid quoted TOML key");
            key = winrt::to_string(parsed.GetString());
        } else {
            key = std::string(rawKey);
            if (key.empty() || key.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.-") != key.npos ||
                key.starts_with('.') || key.ends_with('.') || key.find("..") != key.npos)
                throw std::runtime_error("Invalid TOML key");
        }
        auto value = scalar(line.substr(equals + 1));
        std::string full = section.empty() ? key : section + "." + key;
        if (value.ValueType() == settingsjson::JsonValueType::Array) {
            auto paths = value.GetArray();
            if (!full.starts_with("settings.tray.icons.") || paths.Size() != 2 ||
                paths.GetAt(0).ValueType() != settingsjson::JsonValueType::String ||
                paths.GetAt(1).ValueType() != settingsjson::JsonValueType::String)
                throw std::runtime_error("Expected a light/dark icon path pair");
        }
        if (full == "schemaVersion") {
            if (version || value.ValueType() != settingsjson::JsonValueType::Number || value.GetNumber() != 1)
                throw std::runtime_error("Unsupported TOML schema version");
            version = true;
        } else if (full.starts_with("settings.")) {
            auto name = winrt::to_hstring(full.substr(9));
            if (values.HasKey(name)) throw std::runtime_error("Duplicate setting");
            values.Insert(name, value);
        } else {
            throw std::runtime_error("Unknown TOML root setting");
        }
    }
    if (!version) throw std::runtime_error("Missing TOML schema version");
    return values;
}

std::optional<std::string> ReadLocalFile(std::wstring const& path, uint64_t limit = 65536) {
    std::error_code error;
    auto size = std::filesystem::file_size(path, error);
    if (error == std::errc::no_such_file_or_directory) return std::nullopt;
    if (error) throw std::system_error(error);
    if (size > limit) throw std::runtime_error("File exceeds size limit");
    std::ifstream file(std::filesystem::path(path), std::ios::binary);
    file.exceptions(std::ios::badbit | std::ios::failbit);
    std::string text(static_cast<size_t>(size), '\0');
    if (size) file.read(text.data(), text.size());
    return text;
}

settingsjson::JsonObject ReadLocalSettings(std::wstring const& path) {
    try {
        auto content = ReadLocalFile(path);
        if (!content) return nullptr;
        auto text = *content;
        if (text.starts_with("\xEF\xBB\xBF")) text.erase(0, 3);
        return ParseSettingsToml(text);
    } catch (...) {
        Wh_Log(L"[UWSET] invalid/unreadable settings; using built-in defaults and preserving %s",
               path.c_str());
        return nullptr;
    }
}

enum class SettingKind { Choice, Number, Toggle, Color };
struct SettingChoice { PCWSTR value; PCWSTR label; };
constexpr SettingChoice kThemeChoices[]{
    {L"native", L"Windows"}, {L"glass", L"Glass"},
    {L"frosted", L"Frosted glass"}, {L"liquid", L"Liquid glass"}};
constexpr SettingChoice kUiBodyChoices[]{
    {L"native", L"Native"}, {L"mica", L"Mica"},
    {L"acrylic", L"Acrylic"}, {L"liquid", L"Liquid glass"}};
constexpr SettingChoice kFontChoices[]{
    {L"default", L"Windows"}, {L"light", L"Light"},
    {L"dark", L"Dark"}, {L"adaptive", L"Adaptive"}};
constexpr SettingChoice kPresetChoices[]{
    {L"custom", L"Custom"}, {L"regular", L"Regular glass"},
    {L"frosted", L"Frosted glass"}, {L"dark", L"Dark glass"}};
constexpr SettingChoice kRefractionChoices[]{
    {L"biconvex", L"Biconvex"}, {L"snell", L"Snell's law"},
    {L"dome", L"Dome magnifier"}, {L"layered", L"Layered refraction"},
    {L"bevel", L"Bevel lens"}};
constexpr SettingChoice kDispersionChoices[]{{L"independent", L"Independent"}, {L"coupled", L"With refraction"}};
constexpr SettingChoice kGlintChoices[]{{L"off", L"Off"}, {L"animated", L"Animated"}, {L"pointer", L"Follow pointer"}};
constexpr SettingChoice kSurfaceChoices[]{
    {L"circle", L"Convex circle"}, {L"squircle", L"Convex squircle"},
    {L"concave", L"Concave"}, {L"lip", L"Raised lip"}};
constexpr SettingChoice kLightingChoices[]{
    {L"original", L"Original"}, {L"rich", L"Rich specular"},
    {L"rim", L"Directional rim"}};
struct SettingField {
    PCWSTR key;
    PCWSTR label;
    SettingKind kind;
    int page;
    int defaultValue = 0; // Number, boolean, or choice index.
    int minimum = 0;
    int maximum = 0;
    std::span<const SettingChoice> choices{};
    double displayDivisor = 1;
};
constexpr SettingField kSettingFields[]{
    {L"theme", L"Dock theme", SettingKind::Choice, 0, 0, 0, 0, kThemeChoices},
    {L"fontColor", L"Text color", SettingKind::Choice, 0, 0, 0, 0, kFontChoices},
    {L"tray.grayscale", L"Use grayscale", SettingKind::Toggle, 2},
    {L"cornerRadius", L"Dock corner radius (DIP)", SettingKind::Number, 0, 12, 0, 100},
    {L"buttonCornerRadius", L"Button corner radius (DIP)", SettingKind::Number, 0, 12, 0, 100},
    {L"buttonVerticalPadding", L"Button vertical padding (DIP)", SettingKind::Number, 0, 4, 0, 100},
    {L"edgeDistance", L"Distance from screen edge (DIP)", SettingKind::Number, 0, 4, 0, 100},
    {L"button.border", L"Show button borders", SettingKind::Toggle, 0, 1},
    {L"button.liquidLayer", L"Give each button its own liquid layer", SettingKind::Toggle, 0},
    {L"liquid.preset", L"Preset", SettingKind::Choice, 1, 0, 0, 0, kPresetChoices},
    {L"liquid.zRadius", L"Depth (DIP)", SettingKind::Number, 1, 40, 1, 100},
    {L"liquid.refraction", L"Refraction (%)", SettingKind::Number, 1, 69, 0, 200},
    {L"liquid.chromaticAberration", L"Chromatic aberration (%)", SettingKind::Number, 1, 5, 0, 30},
    {L"liquid.edgeHighlight", L"Edge highlight (%)", SettingKind::Number, 1, 5, 0, 100},
    {L"liquid.specular", L"Specular highlight (%)", SettingKind::Number, 1, 0, 0, 100},
    {L"liquid.fresnel", L"Fresnel reflection (%)", SettingKind::Number, 1, 100, 0, 200},
    {L"liquid.opacity", L"Opacity (%)", SettingKind::Number, 1, 100, 0, 100},
    {L"liquid.brightness", L"Brightness (%)", SettingKind::Number, 1, 0, -50, 50},
    {L"liquid.blur", L"Blur (%)", SettingKind::Number, 1, 0, 0, 100},
    {L"tray.enabled", L"Enable custom tray icons", SettingKind::Toggle, 2, 1},
    {L"liquid.model", L"Refraction model", SettingKind::Choice, 1, 0, 0, 0, kRefractionChoices},
    {L"liquid.surface", L"Surface profile", SettingKind::Choice, 1, 0, 0, 0, kSurfaceChoices},
    {L"liquid.bezelWidth", L"Bezel width (DIP)", SettingKind::Number, 1, 12, 1, 100},
    {L"liquid.thickness", L"Glass thickness (DIP)", SettingKind::Number, 1, 20, 1, 200},
    {L"liquid.refractiveIndex", L"Refractive index", SettingKind::Number, 1, 150, 100, 250, {}, 100},
    {L"liquid.magnification", L"Additional magnification (%)", SettingKind::Number, 1, 0, 0, 200},
    {L"liquid.distortion", L"Micro-distortion (%)", SettingKind::Number, 1, 0, 0, 100},
    {L"liquid.saturation", L"Saturation (%)", SettingKind::Number, 1, 0, -100, 100},
    {L"liquid.tint", L"Cool glass tint (%)", SettingKind::Number, 1, 0, 0, 100},
    {L"liquid.lighting", L"Lighting", SettingKind::Choice, 1, 0, 0, 0, kLightingChoices},
    {L"liquid.lightAngle", L"Rim light angle (degrees)", SettingKind::Number, 1, -60, -180, 180},
    {L"liquid.rimOpacity", L"Rim light opacity (%)", SettingKind::Number, 1, 40, 0, 100},
    {L"liquid.rimSaturation", L"Rim color saturation (%)", SettingKind::Number, 1, 400, 0, 1000},
    {L"liquid.shadowOpacity", L"Shadow opacity (%)", SettingKind::Number, 1, 0, 0, 100},
    {L"liquid.shadowBlur", L"Shadow softness (DIP)", SettingKind::Number, 1, 12, 0, 72},
    {L"liquid.shadowOffset", L"Shadow vertical offset (DIP)", SettingKind::Number, 1, 4, -40, 40},
    {L"adaptive.windowSeconds", L"Adaptive averaging (seconds)", SettingKind::Number, 0, 200, 0, 1000, {}, 100},
    {L"ui.bodyBlur", L"Body blur (%)", SettingKind::Number, 3, 40, 0, 100},
    {L"ui.panelBlur", L"Panel blur (%)", SettingKind::Number, 3, 25, 0, 100},
    {L"ui.panelRefraction", L"Glass refraction (%)", SettingKind::Number, 3, 100, 0, 200},
    {L"ui.panelDepth", L"Glass depth (DIP)", SettingKind::Number, 3, 40, 1, 100},
    {L"ui.panelHighlights", L"Glass highlights (%)", SettingKind::Number, 3, 5, 0, 100},
    {L"ui.bodyMaterial", L"Main body", SettingKind::Choice, 3, 2, 0, 0, kUiBodyChoices},
    {L"ui.glassModel", L"Refraction model", SettingKind::Choice, 3, 0, 0, 0, kRefractionChoices},
    {L"ui.glassSurface", L"Surface profile", SettingKind::Choice, 3, 0, 0, 0, kSurfaceChoices},
    {L"ui.glassBezelWidth", L"Bezel width (DIP)", SettingKind::Number, 3, 12, 1, 100},
    {L"ui.glassThickness", L"Glass thickness (DIP)", SettingKind::Number, 3, 20, 1, 200},
    {L"ui.glassRefractiveIndex", L"Refractive index", SettingKind::Number, 3, 150, 100, 250, {}, 100},
    {L"ui.glassMagnification", L"Magnification (%)", SettingKind::Number, 3, 0, 0, 200},
    {L"ui.glassDistortion", L"Micro-distortion (%)", SettingKind::Number, 3, 0, 0, 100},
    {L"ui.glassChromaticAberration", L"Color fringing (%)", SettingKind::Number, 3, 0, 0, 30},
    {L"ui.glassBrightness", L"Brightness (%)", SettingKind::Number, 3, 0, -50, 50},
    {L"ui.glassSaturation", L"Saturation (%)", SettingKind::Number, 3, 0, -100, 100},
    {L"ui.glassFresnel", L"Fresnel reflection (%)", SettingKind::Number, 3, 100, 0, 200},
    {L"ui.glassLighting", L"Lighting", SettingKind::Choice, 3, 1, 0, 0, kLightingChoices},
    {L"ui.glassLightAngle", L"Light angle (degrees)", SettingKind::Number, 3, -60, -180, 180},
    {L"ui.glassRimOpacity", L"Rim light strength (%)", SettingKind::Number, 3, 40, 0, 100},
    {L"ui.glassRimSaturation", L"Rim color saturation (%)", SettingKind::Number, 3, 400, 0, 1000},
    {L"ui.panelOpacity", L"Panel opacity (%)", SettingKind::Number, 3, 100, 0, 100},
    {L"ui.glassEdgeHighlight", L"Edge highlight (%)", SettingKind::Number, 3, 0, 0, 100},
    {L"liquid.rimRefraction", L"Rim refraction (%)", SettingKind::Number, 1, 50, 0, 200},
    {L"liquid.rimWidth", L"Rim width (DIP)", SettingKind::Number, 1, 3, 1, 60},
    {L"liquid.bevelSharpness", L"Bevel sharpness", SettingKind::Number, 1, 10, 2, 32},
    {L"liquid.cornerBoost", L"Corner boost (%)", SettingKind::Number, 1, 0, 0, 100},
    {L"liquid.rippleStrength", L"Edge ripple (%)", SettingKind::Number, 1, 0, 0, 100},
    {L"liquid.rippleWavelength", L"Ripple spacing (DIP)", SettingKind::Number, 1, 12, 2, 100},
    {L"liquid.edgeWidth", L"Highlight width (DIP; 0 = auto)", SettingKind::Number, 1, 0, 0, 100},
    {L"liquid.edgeSoftness", L"Highlight softness (%)", SettingKind::Number, 1, 100, 10, 400},
    {L"liquid.frostEdge", L"Polished edge (%)", SettingKind::Number, 1, 15, 0, 100},
    {L"liquid.dispersion", L"Color dispersion", SettingKind::Choice, 1, 0, 0, 0, kDispersionChoices},
    {L"liquid.dye", L"Tint color (#RRGGBB)", SettingKind::Color, 1, 0, 0, 0},
    {L"liquid.dyeStrength", L"Tint strength (%)", SettingKind::Number, 1, 0, 0, 100},
    {L"liquid.fluidStrength", L"Fluid response (%)", SettingKind::Number, 1, 0, 0, 100},
    {L"liquid.fluidRadius", L"Fluid reach (%)", SettingKind::Number, 1, 35, 5, 100},
    {L"liquid.fluidViscosity", L"Fluid viscosity (%)", SettingKind::Number, 1, 65, 0, 100},
    {L"liquid.glintMode", L"Moving glints", SettingKind::Choice, 1, 0, 0, 0, kGlintChoices},
    {L"liquid.glintStrength", L"Glint strength (%)", SettingKind::Number, 1, 15, 0, 100},
    {L"liquid.glintSpeed", L"Glint speed (%)", SettingKind::Number, 1, 50, 1, 100},
    {L"liquid.glintSize", L"Glint size (%)", SettingKind::Number, 1, 35, 5, 100},
    {L"liquid.boundaryFade", L"Capture edge softness (DIP)", SettingKind::Number, 1, 2, 0, 20},
    {L"ui.glassRimRefraction", L"Rim refraction (%)", SettingKind::Number, 3, 50, 0, 200},
    {L"ui.glassRimWidth", L"Rim width (DIP)", SettingKind::Number, 3, 3, 1, 60},
    {L"ui.glassBevelSharpness", L"Bevel sharpness", SettingKind::Number, 3, 10, 2, 32},
    {L"ui.glassCornerBoost", L"Corner boost (%)", SettingKind::Number, 3, 0, 0, 100},
    {L"ui.glassRippleStrength", L"Edge ripple (%)", SettingKind::Number, 3, 0, 0, 100},
    {L"ui.glassRippleWavelength", L"Ripple spacing (DIP)", SettingKind::Number, 3, 12, 2, 100},
    {L"ui.glassEdgeWidth", L"Highlight width (DIP; 0 = auto)", SettingKind::Number, 3, 0, 0, 100},
    {L"ui.glassEdgeSoftness", L"Highlight softness (%)", SettingKind::Number, 3, 100, 10, 400},
    {L"ui.glassFrostEdge", L"Polished edge (%)", SettingKind::Number, 3, 15, 0, 100},
    {L"ui.glassDispersion", L"Color dispersion", SettingKind::Choice, 3, 0, 0, 0, kDispersionChoices},
    {L"ui.glassDye", L"Tint color (#RRGGBB)", SettingKind::Color, 3, 0, 0, 0},
    {L"ui.glassDyeStrength", L"Tint strength (%)", SettingKind::Number, 3, 0, 0, 100},
    {L"ui.glassFluidStrength", L"Fluid response (%)", SettingKind::Number, 3, 0, 0, 100},
    {L"ui.glassFluidRadius", L"Fluid reach (%)", SettingKind::Number, 3, 35, 5, 100},
    {L"ui.glassFluidViscosity", L"Fluid viscosity (%)", SettingKind::Number, 3, 65, 0, 100},
    {L"ui.glassGlintMode", L"Moving glints", SettingKind::Choice, 3, 0, 0, 0, kGlintChoices},
    {L"ui.glassGlintStrength", L"Glint strength (%)", SettingKind::Number, 3, 15, 0, 100},
    {L"ui.glassGlintSpeed", L"Glint speed (%)", SettingKind::Number, 3, 50, 1, 100},
    {L"ui.glassGlintSize", L"Glint size (%)", SettingKind::Number, 3, 35, 5, 100},
    {L"ui.glassBoundaryFade", L"Capture edge softness (DIP)", SettingKind::Number, 3, 2, 0, 20},
    {L"tray.highlightPadding", L"Highlight vertical padding (DIP)", SettingKind::Number, 2, 8, 0, 100},
    {L"tray.highlightRadius", L"Highlight corner radius (DIP)", SettingKind::Number, 2, 12, 0, 100},
};
SettingField const& SettingDefinition(PCWSTR key) {
    for (auto const& field : kSettingFields)
        if (std::wstring_view(field.key) == key) return field;
    throw std::logic_error("Unknown setting");
}

int LocalInt(settingsjson::JsonObject const& settings, PCWSTR key) {
    if (settings) {
        auto value = settings.GetNamedValue(key, nullptr);
        if (value && value.ValueType() == settingsjson::JsonValueType::Number) {
            auto divisor = SettingDefinition(key).displayDivisor;
            double number = value.GetNumber() * divisor;
            bool integral = divisor == 1 ? std::trunc(number) == number :
                            std::abs(number - std::round(number)) <= 0.000001;
            if (std::isfinite(number) && integral &&
                number >= std::numeric_limits<int>::min() &&
                number <= std::numeric_limits<int>::max())
                return static_cast<int>(std::round(number));
        }
    }
    return SettingDefinition(key).defaultValue;
}

bool LocalBool(settingsjson::JsonObject const& settings, PCWSTR key) {
    if (settings) {
        auto value = settings.GetNamedValue(key, nullptr);
        if (value && value.ValueType() == settingsjson::JsonValueType::Boolean)
            return value.GetBoolean();
    }
    return SettingDefinition(key).defaultValue != 0;
}

bool ValidGlassColor(std::wstring_view text) {
    return text.size() == 7 && text[0] == L'#' &&
        text.substr(1).find_first_not_of(L"0123456789abcdefABCDEF") == text.npos;
}

std::wstring LocalString(settingsjson::JsonObject const& settings, PCWSTR key) {
    auto const& field = SettingDefinition(key);
    if (settings) {
        auto value = settings.GetNamedValue(key, nullptr);
        if (value && value.ValueType() == settingsjson::JsonValueType::String) {
            auto text = value.GetString();
            if (field.kind == SettingKind::Color && ValidGlassColor(text))
                return std::wstring(text);
            for (auto const& choice : field.choices)
                if (text == choice.value) return std::wstring(text);
        }
    }
    return field.kind == SettingKind::Color ? L"#FFFFFF" : field.choices[field.defaultValue].value;
}

std::wstring CanonicalTrayId(std::wstring const& text) {
    if (text.empty() || text.size() > 20 ||
        text.find_first_not_of(L"0123456789") != text.npos) return {};
    wchar_t* end{};
    errno = 0;
    auto value = wcstoull(text.c_str(), &end, 10);
    if (errno == ERANGE || *end || !value) return {};
    return std::to_wstring(value);
}

TrayOverrides ReadTrayOverrides(settingsjson::JsonObject const& settings) {
    TrayOverrides result;
    if (!settings) return result;
    for (auto const& pair : settings) {
        auto keyName = pair.Key();
        std::wstring_view name(keyName);
        constexpr std::wstring_view prefix = L"tray.icons.";
        if (!name.starts_with(prefix)) continue;
        auto key = CanonicalTrayId(std::wstring(name.substr(prefix.size())));
        if (key.empty()) continue;
        TrayIconFiles paths;
        auto value = pair.Value();
        if (value.ValueType() == settingsjson::JsonValueType::String) {
            paths.light = paths.dark = value.GetString();
        } else if (value.ValueType() == settingsjson::JsonValueType::Array) {
            auto array = value.GetArray();
            if (array.Size() != 2 || array.GetAt(0).ValueType() != settingsjson::JsonValueType::String ||
                array.GetAt(1).ValueType() != settingsjson::JsonValueType::String) continue;
            paths.light = array.GetStringAt(0);
            paths.dark = array.GetStringAt(1);
        }
        if (!paths.empty()) result[key] = std::move(paths);
    }
    return result;
}

void AddTrayOverrides(settingsjson::JsonObject const& values, TrayOverrides const& overrides) {
    for (auto const& [key, paths] : overrides) {
        if (!paths.Adaptive()) {
            values.Insert(L"tray.icons." + key, settingsjson::JsonValue::CreateStringValue(paths.light));
        } else {
            settingsjson::JsonArray array;
            array.Append(settingsjson::JsonValue::CreateStringValue(paths.light));
            array.Append(settingsjson::JsonValue::CreateStringValue(paths.dark));
            values.Insert(L"tray.icons." + key, array);
        }
    }
}

std::string SettingsDocument(settingsjson::JsonObject const& settings, bool includeUi = true) {
    settingsjson::JsonObject values;
    for (auto const& field : kSettingFields) {
        if (!includeUi && field.page == 3) continue;
        if (field.kind == SettingKind::Number)
            values.Insert(field.key, settingsjson::JsonValue::CreateNumberValue(
                LocalInt(settings, field.key) / field.displayDivisor));
        else if (field.kind == SettingKind::Toggle)
            values.Insert(field.key, settingsjson::JsonValue::CreateBooleanValue(LocalBool(settings, field.key)));
        else
            values.Insert(field.key, settingsjson::JsonValue::CreateStringValue(LocalString(settings, field.key)));
    }
    std::string document =
        "# Settings for Windows 11 Better Ultrawide Taskbar.\n"
        "# The settings editor saves and applies changes. Missing values use built-in defaults.\n"
        "schemaVersion = 1\n";
    for (auto group : {"", "button", "liquid", "tray", "adaptive", "ui"}) {
        if (!includeUi && std::string_view(group) == "ui") continue;
        std::string prefix = *group ? std::string(group) + "." : "";
        document += "\n[settings" + (*group ? std::string(".") + group : "") + "]\n";
        for (auto const& pair : values) {
            auto key = winrt::to_string(pair.Key());
            if (prefix.empty() ? key.find('.') != key.npos : !key.starts_with(prefix)) continue;
            document += key.substr(prefix.size()) + " = " +
                        winrt::to_string(pair.Value().Stringify()) + "\n";
        }
    }
    auto overrides = ReadTrayOverrides(settings);
    if (!overrides.empty()) {
        document += "\n[settings.tray.icons]\n";
        settingsjson::JsonObject paths;
        AddTrayOverrides(paths, overrides);
        for (auto const& [key, files] : overrides)
            document += winrt::to_string(settingsjson::JsonValue::CreateStringValue(key).Stringify()) +
                " = " + winrt::to_string(paths.GetNamedValue(L"tray.icons." + key).Stringify()) + "\n";
    }
    return document;
}

bool WriteLocalFile(std::wstring const& path, std::string const& seed, bool replace, size_t limit = 65536) {
    if (path.empty() || seed.empty() || seed.size() > limit) return false;
    auto target = std::filesystem::path(path);
    std::error_code error;
    if (std::filesystem::exists(target, error)) {
        if (!std::filesystem::is_regular_file(target, error)) return false;
        if (!replace) return true;
    }
    if (error) return false;
    std::filesystem::create_directories(target.parent_path(), error);
    if (error) return false;
    auto temporary = path + L"." + std::to_wstring(GetCurrentProcessId()) + L"." +
                     std::to_wstring(GetTickCount64()) + L".tmp";
    HANDLE file = CreateFileW(temporary.c_str(), GENERIC_WRITE, 0, nullptr,
                              CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    DWORD written{};
    bool saved = WriteFile(file, seed.data(), static_cast<DWORD>(seed.size()), &written, nullptr) &&
                 written == seed.size() && FlushFileBuffers(file);
    CloseHandle(file);
    DWORD flags = MOVEFILE_WRITE_THROUGH | (replace ? MOVEFILE_REPLACE_EXISTING : 0);
    if (saved) saved = MoveFileExW(temporary.c_str(), path.c_str(), flags);
    DeleteFileW(temporary.c_str());
    return saved || (!replace && GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES);
}

bool EnsureSettingsFile(std::wstring const& path, std::string const& seed) {
    return WriteLocalFile(path, seed, false);
}

bool OpenSettingsFile() {
    std::wstring arguments = L"\"" + g_settingsPath + L"\"";
    wchar_t systemDirectory[MAX_PATH]{};
    UINT length = GetSystemDirectoryW(systemDirectory, ARRAYSIZE(systemDirectory));
    if (!length || length >= ARRAYSIZE(systemDirectory)) return false;
    std::wstring editor = std::wstring(systemDirectory) + L"\\notepad.exe";
    auto result = ShellExecuteW(nullptr, L"open", editor.c_str(), arguments.c_str(),
                               nullptr, SW_SHOWNORMAL);
    return reinterpret_cast<INT_PTR>(result) > 32;
}

struct IconPixels {
    UINT width{}, height{};
    std::vector<uint32_t> pixels;
};

IconPixels ReadIconPixels(IWICImagingFactory* factory, IWICBitmapSource* source) {
    UINT width{}, height{};
    winrt::check_hresult(source->GetSize(&width, &height));
    if (!width || !height || width > 8192 || height > 8192 ||
        uint64_t(width) * height > 16 * 1024 * 1024)
        throw winrt::hresult_invalid_argument(L"Choose an image no larger than 16 megapixels.");
    double scale = std::min(1.0, 128.0 / std::max(width, height));
    IconPixels result;
    result.width = std::max(1u, static_cast<UINT>(std::lround(width * scale)));
    result.height = std::max(1u, static_cast<UINT>(std::lround(height * scale)));
    winrt::com_ptr<IWICBitmapScaler> scaled;
    winrt::check_hresult(factory->CreateBitmapScaler(scaled.put()));
    winrt::check_hresult(scaled->Initialize(source, result.width, result.height, WICBitmapInterpolationModeFant));
    winrt::com_ptr<IWICFormatConverter> converted;
    winrt::check_hresult(factory->CreateFormatConverter(converted.put()));
    winrt::check_hresult(converted->Initialize(scaled.get(), GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom));
    result.pixels.resize(size_t(result.width) * result.height);
    winrt::check_hresult(converted->CopyPixels(nullptr, result.width * 4,
        static_cast<UINT>(result.pixels.size() * 4), reinterpret_cast<BYTE*>(result.pixels.data())));
    return result;
}

IconPixels LoadTrayImage(std::wstring const& path) {
    auto file = std::filesystem::path(path);
    if (!file.is_absolute() || file.root_name().native().size() != 2)
        throw winrt::hresult_invalid_argument(L"Choose a local image file.");
    winrt::com_ptr<IWICImagingFactory> factory;
    winrt::check_hresult(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
        __uuidof(IWICImagingFactory), factory.put_void()));
    winrt::com_ptr<IWICBitmapDecoder> decoder;
    winrt::check_hresult(factory->CreateDecoderFromFilename(path.c_str(), nullptr, GENERIC_READ,
        WICDecodeMetadataCacheOnDemand, decoder.put()));
    winrt::com_ptr<IWICBitmapFrameDecode> frame;
    winrt::check_hresult(decoder->GetFrame(0, frame.put()));
    return ReadIconPixels(factory.get(), frame.get());
}

uint32_t GrayscalePixel(uint32_t pixel) {
    UINT gray = (54 * ((pixel >> 16) & 255) + 183 * ((pixel >> 8) & 255) + 19 * (pixel & 255) + 128) >> 8;
    return (pixel & 0xFF000000u) | gray * 0x010101u;
}

Media::Imaging::WriteableBitmap IconBitmap(IconPixels const& image, bool grayscale = false) {
    if (image.pixels.empty()) return nullptr;
    Media::Imaging::WriteableBitmap bitmap(image.width, image.height);
    BYTE* bytes{};
    winrt::check_hresult(bitmap.PixelBuffer().as<::Windows::Storage::Streams::IBufferByteAccess>()->Buffer(&bytes));
    if (grayscale) {
        auto target = reinterpret_cast<uint32_t*>(bytes);
        std::transform(image.pixels.begin(), image.pixels.end(), target, GrayscalePixel);
    } else {
        memcpy(bytes, image.pixels.data(), image.pixels.size() * 4);
    }
    bitmap.Invalidate();
    return bitmap;
}

struct TrayImageAsset {
    Media::LoadedImageSurface surface{nullptr};
    ULONGLONG started = GetTickCount64();
    bool failed{};
    ~TrayImageAsset() { if (surface) surface.Close(); }
    bool Ready() {
        if (surface.DecodedPhysicalSize().Width > 0) return true;
        if (GetTickCount64() - started > 5000) failed = true;
        return false;
    }
};

winrt::com_ptr<IStream> EncodeIconPng(IconPixels const& image) {
    winrt::com_ptr<IWICImagingFactory> factory;
    winrt::check_hresult(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
        __uuidof(IWICImagingFactory), factory.put_void()));
    winrt::com_ptr<IWICBitmap> bitmap;
    winrt::check_hresult(factory->CreateBitmapFromMemory(image.width, image.height,
        GUID_WICPixelFormat32bppPBGRA, image.width * 4, static_cast<UINT>(image.pixels.size() * 4),
        reinterpret_cast<BYTE*>(const_cast<uint32_t*>(image.pixels.data())), bitmap.put()));
    winrt::com_ptr<IStream> stream;
    winrt::check_hresult(CreateStreamOnHGlobal(nullptr, TRUE, stream.put()));
    winrt::com_ptr<IWICBitmapEncoder> encoder;
    winrt::check_hresult(factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, encoder.put()));
    winrt::check_hresult(encoder->Initialize(stream.get(), WICBitmapEncoderNoCache));
    winrt::com_ptr<IWICBitmapFrameEncode> frame;
    winrt::check_hresult(encoder->CreateNewFrame(frame.put(), nullptr));
    winrt::check_hresult(frame->Initialize(nullptr));
    winrt::check_hresult(frame->SetSize(image.width, image.height));
    auto format = GUID_WICPixelFormat32bppBGRA;
    winrt::check_hresult(frame->SetPixelFormat(&format));
    winrt::check_hresult(frame->WriteSource(bitmap.get(), nullptr));
    winrt::check_hresult(frame->Commit());
    winrt::check_hresult(encoder->Commit());
    winrt::check_hresult(stream->Seek({}, STREAM_SEEK_SET, nullptr));
    return stream;
}

std::shared_ptr<TrayImageAsset> LoadTraySurface(IconPixels const& image) {
    auto stream = EncodeIconPng(image);
    winrt::Windows::Storage::Streams::IRandomAccessStream random{nullptr};
    winrt::check_hresult(CreateRandomAccessStreamOverStream(stream.get(), BSOS_DEFAULT,
        winrt::guid_of<decltype(random)>(), winrt::put_abi(random)));
    auto asset = std::make_shared<TrayImageAsset>();
    asset->surface = Media::LoadedImageSurface::StartLoadFromStream(random);
    return asset;
}

constexpr size_t kCatalogLimit = 8 * 1024 * 1024;
constexpr size_t kIconDownloadLimit = 4 * 1024 * 1024;
constexpr size_t kSimpleIconsLimit = 256 * 1024;
struct CatalogIconEntry {
    std::wstring title, slug, search, source;
    // Original, light artwork, dark artwork. These are normalized across sources.
    std::array<std::wstring, 3> urls;
    std::string brandColor;
    bool FollowsTextColor() const {
        // Simple Icons renders the same SVG in white and black.
        return source == L"simpleicons" ||
            (urls[1].empty() ? urls[0] : urls[1]) !=
            (urls[2].empty() ? urls[0] : urls[2]);
    }
};

bool CatalogIconSlug(std::wstring_view slug) {
    return !slug.empty() && slug.size() <= 100 &&
        slug.find_first_not_of(L"abcdefghijklmnopqrstuvwxyz0123456789_-") == slug.npos &&
        slug.front() != '-' && slug.front() != '_';
}

std::wstring CatalogSearchText(std::wstring value) {
    if (!value.empty()) CharLowerBuffW(value.data(), static_cast<DWORD>(value.size()));
    return value;
}

PCWSTR CatalogSourceName(std::wstring_view source) {
    if (source == L"native") return L"Dashboard Icons";
    if (source == L"selfhst") return L"selfh.st";
    if (source == L"lobehub") return L"LobeHub";
    if (source == L"simpleicons") return L"Simple Icons";
    return L"";
}

bool AllowedCatalogUrl(std::wstring_view url) {
    if (url == L"https://dashboardicons.com/api/icons/search" ||
        url == L"https://raw.githubusercontent.com/homarr-labs/dashboard-icons/main/metadata.json") return true;
    constexpr std::wstring_view prefixes[]{
        L"https://raw.githubusercontent.com/homarr-labs/dashboard-icons/main/png/",
        L"https://cdn.jsdelivr.net/gh/selfhst/icons/png/",
        L"https://cdn.jsdelivr.net/npm/@lobehub/icons-static-png@latest/light/",
        L"https://cdn.jsdelivr.net/npm/@lobehub/icons-static-png@latest/dark/",
        L"https://raw.githubusercontent.com/simple-icons/simple-icons/develop/icons/"};
    for (auto prefix : prefixes) {
        if (!url.starts_with(prefix)) continue;
        auto file = url.substr(prefix.size());
        auto extension = prefix.ends_with(L"/icons/") ? L".svg" : L".png";
        return file.ends_with(extension) && CatalogIconSlug(file.substr(0, file.size() - 4));
    }
    return false;
}

std::vector<CatalogIconEntry> ParseDashboardCatalog(std::string_view text, std::string_view metadata) {
    if (text.size() > kCatalogLimit || metadata.size() > kCatalogLimit)
        throw std::runtime_error("Icon catalog is too large");
    auto catalog = settingsjson::JsonArray::Parse(winrt::to_hstring(text));
    auto native = settingsjson::JsonObject::Parse(winrt::to_hstring(metadata));
    if (catalog.Size() > 20000) throw std::runtime_error("Icon catalog is too large");
    std::vector<CatalogIconEntry> entries;
    std::set<std::wstring> ids;
    auto png = [](std::wstring const& slug) {
        return CatalogIconSlug(slug)
            ? L"https://raw.githubusercontent.com/homarr-labs/dashboard-icons/main/png/" + slug + L".png"
            : std::wstring{};
    };
    auto append = [&](CatalogIconEntry entry) {
        if (!AllowedCatalogUrl(entry.urls[0]) || !ids.insert(entry.source + L"/" + entry.slug).second) return;
        for (size_t i = 1; i < 3; ++i)
            if (!AllowedCatalogUrl(entry.urls[i])) entry.urls[i].clear();
        entries.push_back(std::move(entry));
    };
    for (auto const& value : catalog) {
        try {
            auto item = value.GetObject();
            CatalogIconEntry entry;
            entry.title = item.GetNamedString(L"name");
            entry.slug = item.GetNamedString(L"slug");
            entry.source = item.GetNamedString(L"source");
            if (!CatalogIconSlug(entry.slug) || entry.title.empty() || entry.title.size() > 200 ||
                !*CatalogSourceName(entry.source)) continue;
            entry.search = entry.title + L" " + entry.slug + L" " + CatalogSourceName(entry.source);
            for (auto const& alias : item.GetNamedObject(L"data").GetNamedArray(L"aliases", settingsjson::JsonArray()))
                if (alias.ValueType() == settingsjson::JsonValueType::String && alias.GetString().size() < 200)
                    entry.search += L" " + std::wstring(alias.GetString());
            entry.search = CatalogSearchText(entry.search);
            if (entry.source == L"native") {
                entry.urls[0] = png(entry.slug);
                auto info = native.GetNamedObject(entry.slug, settingsjson::JsonObject());
                auto colors = info.GetNamedObject(L"colors", settingsjson::JsonObject());
                entry.urls[1] = png(std::wstring(colors.GetNamedString(L"light", L"")));
                entry.urls[2] = png(std::wstring(colors.GetNamedString(L"dark", L"")));
                append(entry);
                auto wordmark = info.GetNamedObject(L"wordmark", settingsjson::JsonObject());
                if (wordmark.Size()) {
                    entry.title += L" · Wordmark";
                    entry.slug += L"-wordmark";
                    entry.search += L" wordmark";
                    entry.urls[1] = png(std::wstring(wordmark.GetNamedString(L"light", L"")));
                    entry.urls[2] = png(std::wstring(wordmark.GetNamedString(L"dark", L"")));
                    entry.urls[0] = entry.urls[1].empty() ? entry.urls[2] : entry.urls[1];
                    append(std::move(entry));
                }
            } else if (entry.source == L"simpleicons") {
                auto hex = winrt::to_string(item.GetNamedObject(L"external").GetNamedString(L"brand_color", L"000000"));
                entry.brandColor = hex.size() == 6 && hex.find_first_not_of("0123456789ABCDEFabcdef") == hex.npos ? hex : "000000";
                entry.urls.fill(L"https://raw.githubusercontent.com/simple-icons/simple-icons/develop/icons/" + entry.slug + L".svg");
                append(std::move(entry));
            } else {
                auto templates = item.GetNamedObject(L"external").GetNamedObject(L"url_templates");
                for (size_t i = 0; i < 3; ++i) {
                    // LobeHub names its background theme; selfh.st names the artwork color.
                    PCWSTR key = i == 0 ? L"png" :
                        ((i == 1) == (entry.source == L"selfhst") ? L"png_light" : L"png_dark");
                    std::wstring url(templates.GetNamedString(key, L""));
                    size_t at{};
                    while ((at = url.find(L"{slug}")) != url.npos) url.replace(at, 6, entry.slug);
                    entry.urls[i] = std::move(url);
                }
                append(std::move(entry));
            }
        } catch (winrt::hresult_error const&) { /* A malformed entry does not hide the rest of the catalog. */ }
    }
    if (entries.empty()) throw std::runtime_error("Invalid Dashboard Icons catalog");
    std::stable_sort(entries.begin(), entries.end(), [](auto const& a, auto const& b) {
        return CatalogSearchText(a.title) < CatalogSearchText(b.title);
    });
    return entries;
}

std::string SimpleSvgPath(std::string_view svg) {
    if (svg.size() > kSimpleIconsLimit || svg.find("viewBox=\"0 0 24 24\"") == svg.npos)
        throw std::runtime_error("Unsupported Simple Icons SVG");
    auto path = svg.find("<path ");
    if (path == svg.npos || svg.find("<path ", path + 1) != svg.npos)
        throw std::runtime_error("Expected one icon path");
    auto endTag = svg.find('>', path);
    auto begin = svg.find(" d=\"", path);
    if (begin == svg.npos || endTag == svg.npos || begin > endTag)
        throw std::runtime_error("Missing icon path data");
    begin += 4;
    auto end = svg.find('"', begin);
    if (end == svg.npos || end > endTag) throw std::runtime_error("Invalid icon path data");
    auto data = svg.substr(begin, end - begin);
    if (data.empty() || data.find_first_not_of("MmZzLlHhVvCcSsQqTtAaEe0123456789+-. ,\t\r\n") != data.npos)
        throw std::runtime_error("Invalid icon path commands");
    return std::string(data);
}

IconPixels RenderSimpleIcon(std::string_view path, std::string_view hex) {
    if (hex.size() != 6 || hex.find_first_not_of("0123456789ABCDEFabcdef") != hex.npos)
        throw std::runtime_error("Invalid icon color");
    // Only validated path commands enter this SVG; downloaded markup is never executed.
    std::string svg = "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 24 24\">"
        "<path fill=\"#" + std::string(hex) + "\" d=\"" + std::string(path) + "\"/></svg>";
    winrt::com_ptr<ID3D11Device> device;
    HRESULT created = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0, D3D11_SDK_VERSION, device.put(), nullptr, nullptr);
    if (FAILED(created))
        winrt::check_hresult(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr,
            D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0, D3D11_SDK_VERSION, device.put(), nullptr, nullptr));
    winrt::com_ptr<ID2D1Factory1> factory;
    winrt::check_hresult(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
        __uuidof(ID2D1Factory1), nullptr, factory.put_void()));
    winrt::com_ptr<ID2D1Device> drawingDevice;
    winrt::check_hresult(factory->CreateDevice(device.as<IDXGIDevice>().get(), drawingDevice.put()));
    winrt::com_ptr<ID2D1DeviceContext> base;
    winrt::check_hresult(drawingDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, base.put()));
    auto context = base.as<ID2D1DeviceContext5>();
    D2D1_BITMAP_PROPERTIES1 properties{};
    properties.pixelFormat = {DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED};
    properties.dpiX = properties.dpiY = 96;
    properties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;
    winrt::com_ptr<ID2D1Bitmap1> target;
    winrt::check_hresult(context->CreateBitmap({128, 128}, nullptr, 0, &properties, target.put()));
    winrt::com_ptr<IStream> stream;
    winrt::check_hresult(CreateStreamOnHGlobal(nullptr, TRUE, stream.put()));
    ULONG written{};
    winrt::check_hresult(stream->Write(svg.data(), static_cast<ULONG>(svg.size()), &written));
    if (written != svg.size()) throw winrt::hresult_error(E_FAIL);
    winrt::check_hresult(stream->Seek({}, STREAM_SEEK_SET, nullptr));
    winrt::com_ptr<ID2D1SvgDocument> document;
    winrt::check_hresult(context->CreateSvgDocument(stream.get(), {24, 24}, document.put()));
    context->SetTarget(target.get());
    D2D1_MATRIX_3X2_F transform{};
    transform._11 = transform._22 = 128.0f / 24;
    context->SetTransform(transform);
    context->BeginDraw();
    context->Clear({0, 0, 0, 0});
    context->DrawSvgDocument(document.get());
    winrt::check_hresult(context->EndDraw());
    context->SetTarget(nullptr);
    properties.bitmapOptions = D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    winrt::com_ptr<ID2D1Bitmap1> staging;
    winrt::check_hresult(context->CreateBitmap({128, 128}, nullptr, 0, &properties, staging.put()));
    winrt::check_hresult(staging->CopyFromBitmap(nullptr, target.get(), nullptr));
    D2D1_MAPPED_RECT mapped{};
    winrt::check_hresult(staging->Map(D2D1_MAP_OPTIONS_READ, &mapped));
    IconPixels result{128, 128, std::vector<uint32_t>(128 * 128)};
    for (UINT y = 0; y < 128; ++y)
        memcpy(result.pixels.data() + y * 128, mapped.bits + y * mapped.pitch, 128 * 4);
    winrt::check_hresult(staging->Unmap());
    return result;
}

struct CatalogIconJob {
    HWND window{};
    UINT_PTR generation{};
    std::wstring directory;
    CatalogIconEntry entry;
    bool previewLight = true;
    HANDLE thread{};
    std::atomic<HINTERNET> request{};
    std::atomic<bool> cancel{}, done{};
    std::vector<CatalogIconEntry> entries;
    IconPixels pixels;
    TrayIconFiles files;
    std::wstring error;
    size_t tileIndex = SIZE_MAX;
};

std::string DownloadCatalogFile(CatalogIconJob& job, std::wstring const& url, size_t limit) {
    if (!AllowedCatalogUrl(url)) throw std::runtime_error("Unsupported icon URL");
    struct InternetHandle {
        HINTERNET value{};
        ~InternetHandle() { if (value) WinHttpCloseHandle(value); }
    };
    InternetHandle session{WinHttpOpen(L"Windhawk-Ultrawide-Settings", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                                      WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0)};
    if (!session.value) winrt::throw_last_error();
    WinHttpSetTimeouts(session.value, 3000, 5000, 5000, 5000);
    auto slash = url.find(L'/', 8);
    auto host = url.substr(8, slash - 8);
    InternetHandle connection{WinHttpConnect(session.value, host.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0)};
    if (!connection.value) winrt::throw_last_error();
    auto path = url.substr(slash);
    HINTERNET request = WinHttpOpenRequest(connection.value, L"GET", path.c_str(), nullptr,
        WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (!request) winrt::throw_last_error();
    job.request = request;
    struct CloseRequest {
        CatalogIconJob& job;
        ~CloseRequest() { if (auto request = job.request.exchange(nullptr)) WinHttpCloseHandle(request); }
    } close{job};
    if (job.cancel) throw std::runtime_error("Cancelled");
    DWORD redirects = WINHTTP_OPTION_REDIRECT_POLICY_NEVER;
    DWORD credentials = WINHTTP_AUTOLOGON_SECURITY_LEVEL_HIGH;
    if (!WinHttpSetOption(request, WINHTTP_OPTION_REDIRECT_POLICY, &redirects, sizeof(redirects)) ||
        !WinHttpSetOption(request, WINHTTP_OPTION_AUTOLOGON_POLICY, &credentials, sizeof(credentials)) ||
        !WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
        !WinHttpReceiveResponse(request, nullptr))
        winrt::throw_last_error();
    DWORD status{}, size = sizeof(status);
    if (!WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
            WINHTTP_HEADER_NAME_BY_INDEX, &status, &size, WINHTTP_NO_HEADER_INDEX) || status != 200)
        throw std::runtime_error("Dashboard Icons download failed");
    std::string result;
    char chunk[8192];
    DWORD read{};
    do {
        if (job.cancel) throw std::runtime_error("Cancelled");
        if (!WinHttpReadData(request, chunk, sizeof(chunk), &read)) winrt::throw_last_error();
        if (result.size() + read > limit) throw std::runtime_error("Icon response too large");
        result.append(chunk, read);
    } while (read);
    return result;
}

IconPixels DecodeCatalogPng(std::string const& data) {
    if (!data.starts_with("\x89PNG\r\n\x1a\n") || data.size() > kIconDownloadLimit)
        throw std::runtime_error("Expected an icon PNG");
    winrt::com_ptr<IWICImagingFactory> factory;
    winrt::check_hresult(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
        __uuidof(IWICImagingFactory), factory.put_void()));
    winrt::com_ptr<IWICStream> stream;
    winrt::check_hresult(factory->CreateStream(stream.put()));
    winrt::check_hresult(stream->InitializeFromMemory(
        reinterpret_cast<BYTE*>(const_cast<char*>(data.data())), static_cast<DWORD>(data.size())));
    winrt::com_ptr<IWICBitmapDecoder> decoder;
    winrt::check_hresult(factory->CreateDecoderFromStream(stream.get(), nullptr, WICDecodeMetadataCacheOnDemand, decoder.put()));
    winrt::com_ptr<IWICBitmapFrameDecode> frame;
    winrt::check_hresult(decoder->GetFrame(0, frame.put()));
    return ReadIconPixels(factory.get(), frame.get());
}

std::wstring LoadCatalogVariant(CatalogIconJob& job, size_t variant, IconPixels& pixels) {
    auto const& entry = job.entry;
    if (entry.urls[variant].empty()) variant = 0;
    auto file = (std::filesystem::path(job.directory) /
        (entry.source + L"-" + entry.slug + L"-" + std::to_wstring(variant) + L".png")).wstring();
    try { pixels = LoadTrayImage(file); } catch (...) {}
    if (!pixels.pixels.empty()) return file;
    auto data = DownloadCatalogFile(job, entry.urls[variant],
        entry.source == L"simpleicons" ? kSimpleIconsLimit : kIconDownloadLimit);
    if (entry.source == L"simpleicons") {
        pixels = RenderSimpleIcon(SimpleSvgPath(data), variant == 1 ? "FFFFFF" :
            variant == 2 ? "000000" : entry.brandColor);
        auto stream = EncodeIconPng(pixels);
        STATSTG info{};
        winrt::check_hresult(stream->Stat(&info, STATFLAG_NONAME));
        if (info.cbSize.QuadPart > kIconDownloadLimit) throw std::runtime_error("Icon PNG too large");
        data.resize(static_cast<size_t>(info.cbSize.QuadPart));
        ULONG read{};
        winrt::check_hresult(stream->Read(data.data(), static_cast<ULONG>(data.size()), &read));
        if (read != data.size()) throw std::runtime_error("Incomplete icon PNG");
    } else {
        pixels = DecodeCatalogPng(data);
    }
    if (job.cancel) throw std::runtime_error("Cancelled");
    if (!WriteLocalFile(file, data, true, kIconDownloadLimit))
        throw std::runtime_error("Cannot cache icon image");
    return file;
}

DWORD WINAPI CatalogIconWorker(void* raw) {
    auto& job = *static_cast<CatalogIconJob*>(raw);
    HRESULT apartment = RoInitialize(RO_INIT_MULTITHREADED);
    try {
        winrt::check_hresult(apartment);
        auto directory = std::filesystem::path(job.directory);
        if (job.entry.slug.empty()) {
            auto path = (directory / L"catalog.json").wstring();
            auto metadataPath = (directory / L"metadata.json").wstring();
            try {
                auto text = ReadLocalFile(path, kCatalogLimit);
                auto metadata = ReadLocalFile(metadataPath, kCatalogLimit);
                if (text && metadata) job.entries = ParseDashboardCatalog(*text, *metadata);
            }
            catch (...) {}
            if (job.entries.empty()) {
                auto text = DownloadCatalogFile(job, L"https://dashboardicons.com/api/icons/search", kCatalogLimit);
                auto metadata = DownloadCatalogFile(job,
                    L"https://raw.githubusercontent.com/homarr-labs/dashboard-icons/main/metadata.json", kCatalogLimit);
                job.entries = ParseDashboardCatalog(text, metadata);
                if (!WriteLocalFile(path, text, true, kCatalogLimit) ||
                    !WriteLocalFile(metadataPath, metadata, true, kCatalogLimit))
                    throw std::runtime_error("Cannot cache the Dashboard Icons catalog");
            }
        } else {
            if (!CatalogIconSlug(job.entry.slug) || !*CatalogSourceName(job.entry.source))
                throw std::runtime_error("Invalid icon selection");
            if (job.entry.FollowsTextColor()) {
                IconPixels light, dark;
                job.files.light = LoadCatalogVariant(job, 1, light);
                job.files.dark = LoadCatalogVariant(job, 2, dark);
                job.pixels = job.previewLight ? std::move(light) : std::move(dark);
            } else {
                auto path = LoadCatalogVariant(job, 0, job.pixels);
                job.files = {path, path};
            }
        }
    } catch (winrt::hresult_error const& error) {
        job.error = L"Could not load Dashboard Icons. " + std::wstring(error.message());
    } catch (std::exception const& error) {
        job.error = L"Could not load Dashboard Icons. " + std::wstring(winrt::to_hstring(error.what()));
    } catch (...) {
        job.error = L"Could not load Dashboard Icons.";
    }
    if (SUCCEEDED(apartment)) RoUninitialize();
    job.done = true;
    // Cancelled thumbnail jobs still notify the UI so they can be reaped without
    // waiting inside a textbox/input event.
    PostMessageW(job.window, kCatalogIconsMessage, job.generation, 0);
    return 0;
}

struct CatalogIconTile {
    Controls::Image image{nullptr};
    Controls::Border background{nullptr};
    Controls::TextBlock loading{nullptr}, source{nullptr};
    TrayIconFiles files;
    bool started{}, failed{};
};

struct CatalogIconPicker {
    Controls::Grid panel{nullptr};
    Controls::TextBox search{nullptr};
    Controls::GridView results{nullptr};
    Controls::Image preview{nullptr};
    Controls::Border previewBackground{nullptr};
    Controls::TextBlock status{nullptr}, selectedName{nullptr};
    Controls::Button retry{nullptr}, use{nullptr}, cancel{nullptr};
    Controls::TextBox::TextChanged_revoker searchChanged;
    Controls::GridView::SelectionChanged_revoker selectionChanged;
    Controls::Button::Click_revoker retryClick;
    Controls::Button::Click_revoker useClick, cancelClick;
    FrameworkElement::Loaded_revoker loaded;
    std::shared_ptr<CatalogIconJob> job;
    std::vector<std::shared_ptr<CatalogIconJob>> thumbnailJobs;
    std::vector<std::shared_ptr<CatalogIconJob>> retiredJobs;
    std::vector<CatalogIconTile> tiles;
    std::vector<CatalogIconEntry> entries;
    std::vector<size_t> filtered;
    std::wstring trayKey, selectedSlug;
    TrayIconFiles selectedFiles;
    UINT_PTR generation{};
    size_t matchCount{};
    bool open{}, filtering{}, searchPending{};
};

void CancelCatalogJob(CatalogIconJob& job) {
    job.cancel = true;
    if (auto request = job.request.exchange(nullptr)) WinHttpCloseHandle(request);
}

void JoinCatalogJob(CatalogIconJob& job) {
    if (job.thread) {
        WaitForSingleObject(job.thread, INFINITE);
        CloseHandle(job.thread);
        job.thread = nullptr;
    }
}

void StopCatalogJob(CatalogIconPicker& picker) {
    if (!picker.job) return;
    auto job = std::move(picker.job);
    CancelCatalogJob(*job);
    JoinCatalogJob(*job);
}

void StopCatalogThumbnails(CatalogIconPicker& picker) {
    for (auto const& job : picker.thumbnailJobs) CancelCatalogJob(*job);
    for (auto const& job : picker.retiredJobs) CancelCatalogJob(*job);
    for (auto const& job : picker.thumbnailJobs) JoinCatalogJob(*job);
    for (auto const& job : picker.retiredJobs) JoinCatalogJob(*job);
    picker.thumbnailJobs.clear();
    picker.retiredJobs.clear();
}

void RetireCatalogThumbnails(CatalogIconPicker& picker) {
    for (auto const& job : picker.thumbnailJobs) {
        job->cancel = true;
        if (job->done) JoinCatalogJob(*job);
        else picker.retiredJobs.push_back(job);
    }
    picker.thumbnailJobs.clear();
}

// Native Direct2D saturation effect; no Win2D dependency.
MIDL_INTERFACE("2FC57384-A068-44D7-A331-30982FCF7177")
TrayEffectInterop : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetEffectId(GUID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR, UINT*, DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperty(UINT, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSource(UINT, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSourceCount(UINT*) = 0;
};
__CRT_UUID_DECL(TrayEffectInterop, 0x2FC57384, 0xA068, 0x44D7, 0xA3,0x31,0x30,0x98,0x2F,0xCF,0x71,0x77)
struct TrayGrayscaleEffect : winrt::implements<TrayGrayscaleEffect,
    winrt::Windows::Graphics::Effects::IGraphicsEffect,
    winrt::Windows::Graphics::Effects::IGraphicsEffectSource, TrayEffectInterop> {
    winrt::hstring name;
    CompositionEffectSourceParameter input{L"Icon"};
    winrt::hstring Name() { return name; }
    void Name(winrt::hstring value) { name = value; }
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        if (!id) return E_POINTER;
        *id = CLSID_D2D1Saturation; return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR, UINT*, DWORD*) noexcept override { return E_INVALIDARG; }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override {
        if (!count) return E_POINTER;
        *count = 1; return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, void** value) noexcept override {
        if (!value) return E_POINTER;
        *value = nullptr;
        if (index) return E_BOUNDS;
        try { winrt::copy_to_abi(winrt::Windows::Foundation::PropertyValue::CreateSingle(0)
                    .as<winrt::Windows::Foundation::IPropertyValue>(), *value); return S_OK; }
        catch (...) { return winrt::to_hresult(); }
    }
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, void** value) noexcept override {
        if (!value) return E_POINTER;
        *value = nullptr;
        if (index) return E_BOUNDS;
        winrt::copy_to_abi(input.as<winrt::Windows::Graphics::Effects::IGraphicsEffectSource>(), *value); return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override {
        if (!count) return E_POINTER;
        *count = 1; return S_OK;
    }
};

CompositionBrush TrayBrush(Compositor const& compositor, CompositionBrush const& source, bool grayscale) {
    if (!grayscale) return source;
    auto effect = winrt::make<TrayGrayscaleEffect>();
    auto brush = compositor.CreateEffectFactory(effect).CreateBrush();
    brush.SetSourceParameter(L"Icon", source);
    return brush;
}

struct TrayCatalogItem {
    std::wstring key, label;
    IconPixels original;
    bool overflow{};
};
std::mutex g_trayCatalogMutex;
std::vector<TrayCatalogItem> g_trayCatalog;
bool g_trayCatalogAvailable{};

struct TrayEditorRow {
    Controls::Button choose{nullptr}, reset{nullptr};
    Controls::MenuFlyoutItem local{nullptr}, simple{nullptr};
    Controls::MenuFlyoutItem::Click_revoker localClick, simpleClick;
    Controls::Button::Click_revoker resetClick;
};

constexpr int kEditorSave = 7101, kEditorReload = 7102, kEditorOpenFile = 7103,
              kEditorRefreshPreview = 7104, kEditorRefreshTray = 7105;
constexpr PCWSTR kEditorClass = L"Windhawk.UltrawideSettings." WH_MOD_ID;
constexpr PCWSTR kEditorFrameProperty = L"Windhawk.UltrawideSettings.Frame";
constexpr int kEditorCaptionHeight = 48, kEditorCaptionButtonWidth = 46;
std::mutex g_editorMutex;
HANDLE g_editorThread{};
std::atomic<HWND> g_editorWindow{};
std::atomic<bool> g_editorCancel{};

struct EditorControl {
    Controls::Control input{nullptr};
    Controls::Slider slider{nullptr};
    Controls::Grid row{nullptr};
    Controls::Border card{nullptr};
    Controls::TextBlock error{nullptr};
    Controls::Primitives::ToggleButton disclosure{nullptr};
    std::wstring help, lastValidColor;
    Controls::ComboBox::SelectionChanged_revoker selection;
    Controls::ToggleSwitch::Toggled_revoker toggled;
    Controls::TextBox::TextChanging_revoker textChanged;
    Controls::Slider::ValueChanged_revoker valueChanged;
};
struct EditorSection {
    int page{};
    Controls::Primitives::ToggleButton header{nullptr};
    Controls::StackPanel body{nullptr};
    std::wstring title;
    Controls::Primitives::ToggleButton::Checked_revoker checked;
    Controls::Primitives::ToggleButton::Unchecked_revoker unchecked;
};
struct LiquidRenderer;
struct GlassShadow;
// Owned by the XAML host, so handlers are detached on its UI thread.
struct GlassPointerBinding {
    UIElement source{nullptr};
    winrt::Windows::Foundation::IInspectable handler{nullptr}, cancelled{nullptr};
    GlassPointerBinding(UIElement const& source, FrameworkElement const& reference,
                        std::shared_ptr<LiquidRenderer> const& renderer);
    ~GlassPointerBinding() {
        if (source && handler) {
            try {
                source.RemoveHandler(UIElement::PointerMovedEvent(), handler);
                source.RemoveHandler(UIElement::PointerExitedEvent(), handler);
                source.RemoveHandler(UIElement::PointerCanceledEvent(), cancelled);
            } catch (...) {}
        }
    }
};
struct TaskbarPreviewMetrics {
    double frameHeight = 48, dockHeight = 40;
    double iconWidth = 24, iconHeight = 24, iconMin = 24, iconMax = 24;
    UINT dpi = 96;
    bool frameMeasured{}, iconMeasured{};
};
std::mutex g_previewMetricsMutex;
TaskbarPreviewMetrics g_previewMetrics;
struct EditorPreview {
    int width = 640, height = 420;
    TaskbarPreviewMetrics metrics;
    Controls::Canvas canvas{nullptr};
    Controls::Image backdrop{nullptr};
    Controls::Border material{nullptr};
    Controls::Border trayHighlight{nullptr};
    Controls::Grid liquidHost{nullptr};
    Controls::TextBlock caption{nullptr}, clock{nullptr};
    Controls::TextBlock measurement{nullptr};
    Controls::HyperlinkButton refresh{nullptr};
    Controls::HyperlinkButton::Click_revoker refreshClick;
    Shapes::Path startIcon{nullptr};
    std::vector<Controls::Border> buttons;
    std::vector<Controls::FontIcon> icons;
    std::vector<Controls::Image> appIcons;
    std::vector<uint32_t> pixels;
    std::shared_ptr<LiquidRenderer> renderer;
    std::unique_ptr<GlassPointerBinding> glassPointer;
    std::shared_ptr<GlassShadow> shadow;
    CompositionRoundedRectangleGeometry shadowGeometry{nullptr};
    bool unavailable{};
    std::array<bool,3> foregroundLight{},foregroundReady{};
    std::future<std::array<double,3>> foregroundSample;
    uint64_t foregroundRevision{},foregroundSampleRevision{};
    bool foregroundDirty{};
};
struct SettingsEditor {
    HWND window{}, child{};
    WNDPROC childProc{};
    int captionPressed{}, captionHovered{};
    std::array<int, 3> captionStates{};
    int captionAppearance = -100;
    std::array<std::optional<int64_t>, 3> captionPressedTokens;
    DWORD captionClickTime{};
    POINT captionClickPoint{};
    bool framePointerOver{};
    UINT dpi{96};
    bool syncing{}, valid{}, dirty{}, navigatingFocus{}, compactLayout{};
    double layoutScale{};
    Hosting::DesktopWindowXamlSource island{nullptr};
    Controls::Grid root{nullptr};
    Controls::UserControl content{nullptr};
    Controls::ListBox navigation{nullptr};
    Controls::ScrollViewer scroll{nullptr};
    std::array<Controls::StackPanel, 4> pages{nullptr, nullptr, nullptr, nullptr};
    Controls::TextBlock status{nullptr};
    Controls::Button save{nullptr}, reload{nullptr}, openFile{nullptr};
    Controls::Button enableLiquid{nullptr};
    Controls::TextBlock liquidHint{nullptr};
    Controls::Button::Click_revoker enableLiquidClick;
    std::vector<EditorSection> sections;
    Controls::ListBox::SelectionChanged_revoker navigationChanged;
    Controls::ScrollViewer::ViewChanged_revoker surfaceScrolled;
    Controls::Button::Click_revoker saveClick, reloadClick, openFileClick;
    Hosting::DesktopWindowXamlSource::TakeFocusRequested_revoker takeFocus;
    std::array<EditorControl, std::size(kSettingFields)> controls{};
    EditorPreview preview;
    Controls::Grid surface{nullptr}, liquidBackground{nullptr};
    Controls::Grid bodySource{nullptr};
    Controls::Border bodyMaterial{nullptr};
    SpriteVisual bodyBackdrop{nullptr}, bodySample{nullptr};
    CompositionVisualSurface bodySurface{nullptr};
    FrameworkElement header{nullptr};
    RECT headerBounds{}; // Client pixels, refreshed with the window layout.
    Controls::Border sidebar{nullptr}, inspector{nullptr}, footer{nullptr};
    std::array<Controls::Button, 3> captionButtons{nullptr, nullptr, nullptr};
    std::array<Controls::Button::Click_revoker, 3> captionClicks;
    Controls::FontIcon maximizeGlyph{nullptr};
    Media::Brush cardMaterial{nullptr};
    std::shared_ptr<LiquidRenderer> backgroundRenderer;
    std::shared_ptr<LiquidRenderer> desktopRenderer;
    std::unique_ptr<GlassPointerBinding> glassPointer;
    CompositionRoundedRectangleGeometry backgroundGeometry{nullptr};
    SIZE windowShapeSize{};
    float windowShapeRadius = -1;
    int surfaceAppearance = -1;
    bool updatingSurface{};
    winrt::Windows::UI::ViewManagement::UISettings systemColors;
    winrt::Windows::UI::ViewManagement::UISettings::ColorValuesChanged_revoker colorsChanged;
    winrt::Windows::UI::ViewManagement::UISettings::TextScaleFactorChanged_revoker textScaleChanged;
    TrayOverrides trayOverrides;
    std::vector<TrayCatalogItem> catalog;
    std::vector<TrayEditorRow> trayRows;
    Controls::StackPanel trayList{nullptr};
    Controls::ToggleSwitch trayEnabled{nullptr};
    Controls::TextBlock trayStatus{nullptr};
    Controls::Button refreshTray{nullptr};
    Controls::Button::Click_revoker refreshTrayClick;
    winrt::com_ptr<IFileOpenDialog> filePicker;
    CatalogIconPicker iconPicker;
    std::optional<std::string> originalText;
    std::string originalDocument;
};
void CreateEditorPreview(SettingsEditor& editor);
void UpdateEditorPreview(SettingsEditor& editor) noexcept;
void UpdateEditorSurface(SettingsEditor& editor, bool updateCards = true) noexcept;
void UpdateEditorPosition(SettingsEditor& editor) noexcept;
void CloseEditorSurface(SettingsEditor& editor);
void CloseEditorPreview(SettingsEditor& editor);
void LayoutEditor(SettingsEditor& editor);
void ApplyEditorLayout(SettingsEditor& editor, double width, double textScale);
void UpdatePreviewForeground(SettingsEditor& editor);
void RequestPreviewMetrics(SettingsEditor& editor);
void ApplyPreviewMetrics(SettingsEditor& editor, TaskbarPreviewMetrics const& metrics);
void RequestTrayCatalog(SettingsEditor& editor);
void RebuildTrayRows(SettingsEditor& editor);
void ChooseTrayImage(SettingsEditor& editor, std::wstring const& key);
void QueryTrayCatalog(HWND editor);
void OpenCatalogPicker(SettingsEditor& editor, std::wstring const& key);
void CloseCatalogPicker(SettingsEditor& editor);
void CatalogIconResult(SettingsEditor& editor, UINT_PTR generation);

void RequestPreviewMetrics(SettingsEditor& editor) {
    auto dispatcher = g_dispatch.load();
    if (dispatcher && PostMessageW(dispatcher, kQueryPreviewMetricsMessage,
            reinterpret_cast<WPARAM>(editor.window),
            reinterpret_cast<LPARAM>(MonitorFromWindow(editor.window, MONITOR_DEFAULTTONEAREST)))) {
        editor.preview.measurement.Text(L"Measuring taskbar...");
    } else {
        ApplyPreviewMetrics(editor, {});
    }
}

int EditorScale(SettingsEditor const& editor, int value) {
    return MulDiv(value, editor.dpi, 96);
}

void EditorStatus(SettingsEditor& editor, PCWSTR message) {
    if (editor.status) editor.status.Text(message);
}

bool EditorNumber(Controls::TextBox const& input, SettingField const& field, int& result) {
    auto text = input.Text();
    wchar_t* end{};
    errno = 0;
    double value = wcstod(text.c_str(), &end) * field.displayDivisor;
    if (end == text.c_str() || end != text.c_str() + text.size() ||
        errno == ERANGE || !std::isfinite(value) ||
        value < field.minimum || value > field.maximum ||
        std::abs(value - std::round(value)) > 0.000001)
        return false;
    result = static_cast<int>(std::lround(value));
    return true;
}

std::wstring EditorNumberText(int value, SettingField const& field) {
    if (field.displayDivisor == 1) return std::to_wstring(value);
    wchar_t text[32]{};
    swprintf_s(text, L"%.2f", value / field.displayDivisor);
    return text;
}

std::wstring EditorFieldError(EditorControl const& control, SettingField const& field) {
    if (field.kind == SettingKind::Color)
        return ValidGlassColor(control.input.as<Controls::TextBox>().Text())
            ? L"" : L"Enter a color such as #88AAFF.";
    int value{};
    if (field.kind != SettingKind::Number || EditorNumber(control.input.as<Controls::TextBox>(),field,value))
        return {};
    return (field.displayDivisor == 1 ? std::wstring(L"Enter a whole number from ") :
            std::wstring(L"Enter a value with up to two decimals from ")) +
        EditorNumberText(field.minimum,field) + L" to " + EditorNumberText(field.maximum,field) + L".";
}

bool UpdateEditorValidation(SettingsEditor& editor) {
    bool valid=true;
    for(size_t i=0;i<editor.controls.size();++i) {
        auto& control=editor.controls[i];
        if(!control.error) continue;
        auto message=EditorFieldError(control,kSettingFields[i]);
        if(message.empty() && kSettingFields[i].kind==SettingKind::Color)
            control.lastValidColor=control.input.as<Controls::TextBox>().Text();
        if(!control.input.IsEnabled()) message.clear();
        valid &= message.empty();
        auto displayed=message.empty()?message:message+L" Preview keeps the last valid value.";
        if(control.error.Text()!=displayed) control.error.Text(displayed);
        control.error.Visibility(message.empty()?Visibility::Collapsed:Visibility::Visible);
        Automation::AutomationProperties::SetHelpText(control.input,
            message.empty()?control.help:displayed);
    }
    return valid;
}

void UpdateEditorSection(EditorSection& section,bool available=true) {
    bool open=section.header.IsChecked().Value();
    section.header.Visibility(available?Visibility::Visible:Visibility::Collapsed);
    section.body.Visibility(available && open?Visibility::Visible:Visibility::Collapsed);
    auto label=(open?L"\x25BE ":L"\x25B8 ")+section.title;
    if(winrt::unbox_value_or<winrt::hstring>(section.header.Content(),{})!=label)
        section.header.Content(winrt::box_value(label));
}

void UpdateEditorEnabled(SettingsEditor& editor) {
    bool liquid = editor.controls[0].input.as<Controls::ComboBox>().SelectedIndex() == 3;
    bool custom = editor.controls[9].input.as<Controls::ComboBox>().SelectedIndex() == 0;
    int model = editor.controls[20].input.as<Controls::ComboBox>().SelectedIndex();
    bool snell = model == 1;
    bool rim = editor.controls[29].input.as<Controls::ComboBox>().SelectedIndex() == 2;
    int uiModel = editor.controls[43].input.as<Controls::ComboBox>().SelectedIndex();
    bool uiSnell = uiModel == 1;
    bool uiRim = editor.controls[54].input.as<Controls::ComboBox>().SelectedIndex() == 2;
    for (size_t i = 0; i < editor.controls.size(); ++i) {
        bool enabled = true;
        if (i == 8 || i == 9) enabled &= liquid;
        if (kSettingFields[i].page == 1 && i != 9) enabled &= liquid && custom;
        if (i == 2) enabled &= editor.trayEnabled.IsOn();
        if (i == 10) enabled &= !snell && model < 3;
        if (i == 22) enabled &= snell || model >= 3 ||
            editor.controls[63].slider.Value()>0 || editor.controls[64].slider.Value()>0;
        if (i >= 23 && i <= 24) enabled &= snell;
        if (i == 14) enabled &= !rim;
        if (i >= 30 && i <= 32) enabled &= rim;
        if (i == 34 || i == 35) enabled &= editor.controls[33].slider.Value() > 0;
        if (i == 36) enabled &= editor.controls[1].input.as<Controls::ComboBox>().SelectedIndex() == 3;
        if (i == 37) enabled &= editor.controls[42].input.as<Controls::ComboBox>().SelectedIndex() == 3;
        if (i == 40) enabled &= !uiSnell && uiModel < 3;
        if (i == 45) enabled &= uiSnell || uiModel >= 3 ||
            editor.controls[83].slider.Value()>0 || editor.controls[84].slider.Value()>0;
        if (i >= 46 && i <= 47) enabled &= uiSnell;
        if (i == 41) enabled &= !uiRim;
        if (i >= 55 && i <= 57) enabled &= uiRim;
        if (i >= 60 && i < 100) {
            size_t base = i < 80 ? 60 : 80, option = i - base;
            int selectedModel = i < 80 ? model : uiModel;
            int glints = editor.controls[base+15].input.as<Controls::ComboBox>().SelectedIndex();
            if (option == 0) enabled &= selectedModel >= 3;
            if (option == 1) enabled &= selectedModel == 3;
            if (option == 2) enabled &= selectedModel == 4;
            if (option == 5) enabled &= editor.controls[base+4].slider.Value() > 0;
            if (option == 8) enabled &= i<80 ? editor.controls[18].slider.Value()>0 :
                editor.controls[38].slider.Value()>0 ||
                (editor.controls[42].input.as<Controls::ComboBox>().SelectedIndex()==3 &&
                 editor.controls[37].slider.Value()>0);
            if (option == 10) enabled &= editor.controls[base+11].slider.Value() > 0;
            if (option == 13 || option == 14) enabled &= editor.controls[base+12].slider.Value() > 0;
            if (option == 16 || option == 18) enabled &= glints > 0;
            if (option == 17) enabled &= glints == 1;
        }
        bool expert = (kSettingFields[i].page==1 && i!=9) ||
            (kSettingFields[i].page==3 && i!=42 && i!=58 && i!=59);
        editor.controls[i].card.Visibility((i==9 ? liquid : !expert || enabled)
            ?Visibility::Visible:Visibility::Collapsed);
        editor.controls[i].input.IsEnabled(editor.valid && enabled);
        if (editor.controls[i].slider) editor.controls[i].slider.IsEnabled(editor.valid && enabled);
        editor.controls[i].row.Opacity(editor.valid && enabled ? 1 : 0.5);
    }
    for(auto& section:editor.sections) UpdateEditorSection(section,section.page!=1 || (liquid && custom));
    editor.enableLiquid.Visibility(liquid && custom?Visibility::Collapsed:Visibility::Visible);
    editor.enableLiquid.Content(winrt::box_value(liquid?L"Use custom controls":L"Preview Liquid glass"));
    editor.enableLiquid.IsEnabled(editor.valid);
    editor.liquidHint.Text(!liquid?L"Preview the Liquid glass theme before applying it." :
        !custom?L"This preset sets the effect. Choose Custom to adjust individual settings." :
        L"Open a group to fine-tune the effect. Changes stay in the preview until you save.");
    editor.save.IsEnabled(editor.valid);
}

void EditorChanged(SettingsEditor& editor) {
    if (editor.syncing) return;
    editor.dirty = true;
    UpdateEditorEnabled(editor);
    EditorStatus(editor, UpdateEditorValidation(editor)?L"Unsaved changes. Save to apply.":
        L"Check the highlighted values. The preview keeps their last valid settings.");
    UpdateEditorSurface(editor);
    UpdateEditorPreview(editor);
}

void RequestTrayCatalog(SettingsEditor& editor) {
    if (!editor.trayStatus) return;
    if (auto dispatcher = g_dispatch.load(); dispatcher &&
        PostMessageW(dispatcher, kQueryTrayCatalogMessage, reinterpret_cast<WPARAM>(editor.window), 0))
        editor.trayStatus.Text(L"Reading current tray icons...");
    else
        editor.trayStatus.Text(L"Open settings from the taskbar to read its tray icons.");
}

void RebuildTrayRows(SettingsEditor& editor) {
    if (!editor.trayList) return;
    editor.trayRows.clear(); // Revoke handlers before removing their buttons.
    editor.trayList.Children().Clear();
    bool enabled = editor.trayEnabled.IsOn();
    bool grayscale = enabled && editor.controls[2].input.as<Controls::ToggleSwitch>().IsOn();
    for (auto const& item : editor.catalog) {
        auto card = Markup::XamlReader::Load(LR"XAML(
<Border xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        Background="{ThemeResource SystemControlBackgroundChromeMediumLowBrush}"
        CornerRadius="10" Padding="12" Margin="0,0,0,8">
  <Grid>
    <Grid.RowDefinitions><RowDefinition Height="Auto"/><RowDefinition Height="Auto"/></Grid.RowDefinitions>
    <Grid.ColumnDefinitions><ColumnDefinition Width="52"/><ColumnDefinition/></Grid.ColumnDefinitions>
    <Border Width="40" Height="40" VerticalAlignment="Center">
      <Grid>
        <TextBlock x:Name="Placeholder" Text="App" FontSize="11" Opacity="0.6" HorizontalAlignment="Center" VerticalAlignment="Center"/>
        <Image x:Name="Icon" Stretch="Uniform"/>
      </Grid>
    </Border>
    <StackPanel Grid.Column="1" VerticalAlignment="Center" Margin="4,0,12,0" Spacing="4">
      <TextBlock x:Name="Label" FontWeight="SemiBold" TextTrimming="CharacterEllipsis"/>
      <TextBlock x:Name="Path" FontSize="12" Opacity="0.7" TextTrimming="CharacterEllipsis"/>
    </StackPanel>
    <StackPanel Grid.Row="1" Grid.ColumnSpan="2" Orientation="Horizontal" Margin="0,10,0,0" Spacing="6">
      <Button x:Name="Choose" Content="Choose icon"/>
      <Button x:Name="Reset" Content="Use original"/>
    </StackPanel>
  </Grid>
</Border>)XAML").as<Controls::Border>();
        card.FindName(L"Label").as<Controls::TextBlock>().Text(item.label);
        auto detail = card.FindName(L"Path").as<Controls::TextBlock>();
        auto icon = card.FindName(L"Icon").as<Controls::Image>();
        auto override = editor.trayOverrides.find(item.key);
        bool custom = override != editor.trayOverrides.end();
        IconPixels pixels = item.original;
        detail.Text(item.overflow ? L"Original icon · Overflow" : L"Original icon");
        if (custom && !enabled) {
            detail.Text(L"Custom icon saved · Disabled");
        } else if (custom) {
            auto const& paths = override->second;
            bool light = editor.controls[1].input.as<Controls::ComboBox>().SelectedIndex() == 1 ||
                (editor.controls[1].input.as<Controls::ComboBox>().SelectedIndex() != 2 &&
                 editor.content.RequestedTheme() == ElementTheme::Dark);
            auto const& path = light ? paths.light : paths.dark;
            detail.Text(paths.Adaptive() ? L"Follows text color · " + path : path);
            try { pixels = LoadTrayImage(path); }
            catch (...) { detail.Text(L"File unavailable: " + path); }
        }
        if (!pixels.pixels.empty()) {
            icon.Source(IconBitmap(pixels, grayscale));
            card.FindName(L"Placeholder").as<Controls::TextBlock>().Visibility(Visibility::Collapsed);
        }
        TrayEditorRow row;
        row.choose = card.FindName(L"Choose").as<Controls::Button>();
        row.reset = card.FindName(L"Reset").as<Controls::Button>();
        row.choose.IsEnabled(editor.valid && enabled);
        row.reset.IsEnabled(editor.valid && enabled && custom);
        Automation::AutomationProperties::SetName(row.choose, L"Choose image for " + item.label);
        Automation::AutomationProperties::SetName(row.reset, L"Use original icon for " + item.label);
        Controls::MenuFlyout menu;
        row.local = Controls::MenuFlyoutItem();
        row.local.Text(L"Local image...");
        row.simple = Controls::MenuFlyoutItem();
        row.simple.Text(L"Dashboard Icons...");
        menu.Items().Append(row.local); menu.Items().Append(row.simple);
        row.choose.Flyout(menu);
        row.localClick = row.local.Click(winrt::auto_revoke,
            [&editor, key = item.key](auto const&, auto const&) { ChooseTrayImage(editor, key); });
        row.simpleClick = row.simple.Click(winrt::auto_revoke,
            [&editor, key = item.key](auto const&, auto const&) { OpenCatalogPicker(editor, key); });
        row.resetClick = row.reset.Click(winrt::auto_revoke,
            [&editor, key = item.key](auto const&, auto const&) {
                editor.trayOverrides.erase(key);
                EditorChanged(editor);
                RebuildTrayRows(editor);
            });
        editor.trayRows.push_back(std::move(row));
        editor.trayList.Children().Append(card);
    }
    UpdateEditorSurface(editor);
}

void ChooseTrayImage(SettingsEditor& editor, std::wstring const& key) {
    try {
        winrt::com_ptr<IFileOpenDialog> dialog;
        winrt::check_hresult(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER,
            IID_IFileOpenDialog, dialog.put_void()));
        DWORD options{};
        winrt::check_hresult(dialog->GetOptions(&options));
        winrt::check_hresult(dialog->SetOptions(options | FOS_FORCEFILESYSTEM | FOS_FILEMUSTEXIST |
            FOS_PATHMUSTEXIST | FOS_NOCHANGEDIR | FOS_DONTADDTORECENT));
        COMDLG_FILTERSPEC filters[]{
            {L"Image files", L"*.png;*.ico;*.jpg;*.jpeg;*.bmp;*.gif;*.tif;*.tiff"}};
        winrt::check_hresult(dialog->SetFileTypes(ARRAYSIZE(filters), filters));
        dialog->SetTitle(L"Choose a tray icon image");
        editor.filePicker = dialog;
        HRESULT selected = dialog->Show(editor.window);
        editor.filePicker = nullptr;
        if (selected == HRESULT_FROM_WIN32(ERROR_CANCELLED) || !IsWindow(editor.window)) return;
        winrt::check_hresult(selected);
        winrt::com_ptr<IShellItem> file;
        winrt::check_hresult(dialog->GetResult(file.put()));
        PWSTR raw{};
        winrt::check_hresult(file->GetDisplayName(SIGDN_FILESYSPATH, &raw));
        std::wstring path(raw);
        CoTaskMemFree(raw);
        LoadTrayImage(path); // Validate before changing the draft or saved file.
        editor.trayOverrides[key] = {path, path};
        EditorChanged(editor);
        RebuildTrayRows(editor);
    } catch (winrt::hresult_error const& error) {
        editor.filePicker = nullptr;
        EditorStatus(editor, (L"Cannot use that image. " + std::wstring(error.message())).c_str());
    } catch (...) {
        editor.filePicker = nullptr;
        EditorStatus(editor, L"Cannot read that image. Choose a supported local image file.");
    }
}

bool CatalogPreviewLight(SettingsEditor const& editor) {
    int color = editor.controls[1].input.as<Controls::ComboBox>().SelectedIndex();
    return color == 1 || (color != 2 && editor.content.RequestedTheme() == ElementTheme::Dark);
}

std::shared_ptr<CatalogIconJob> StartCatalogDownload(SettingsEditor& editor, size_t tile = SIZE_MAX) {
    auto& picker = editor.iconPicker;
    auto job = std::make_shared<CatalogIconJob>();
    job->window = editor.window;
    job->generation = ++picker.generation;
    job->directory = (std::filesystem::path(g_settingsPath).parent_path() /
                      L"dashboard-icons" / L"v1").wstring();
    if (tile < picker.filtered.size()) job->entry = picker.entries[picker.filtered[tile]];
    job->tileIndex = tile;
    job->previewLight = CatalogPreviewLight(editor);
    job->thread = CreateThread(nullptr, 0, CatalogIconWorker, job.get(), 0, nullptr);
    if (!job->thread) winrt::throw_last_error();
    return job;
}

void StartCatalogJob(SettingsEditor& editor) {
    auto& picker = editor.iconPicker;
    StopCatalogJob(picker);
    picker.selectedFiles = {};
    picker.use.IsEnabled(false);
    picker.preview.Source(nullptr);
    picker.retry.Visibility(Visibility::Collapsed);
    picker.status.Text(L"Loading Dashboard Icons and its sources...");
    try { picker.job = StartCatalogDownload(editor); }
    catch (...) {
        picker.status.Text(L"Could not start the download.");
        picker.retry.Visibility(Visibility::Visible);
    }
}

void UpdateCatalogSelection(SettingsEditor& editor) {
    auto& picker = editor.iconPicker;
    if (picker.searchPending) {
        picker.selectedFiles = {};
        picker.use.IsEnabled(false);
        return;
    }
    int index = picker.results.SelectedIndex();
    if (index < 0 || static_cast<size_t>(index) >= picker.tiles.size()) {
        picker.selectedSlug.clear();
        picker.selectedFiles = {};
        picker.selectedName.Text(L"Select an icon to preview it");
        picker.preview.Source(nullptr);
        picker.use.IsEnabled(false);
        return;
    }
    auto const& entry = picker.entries[picker.filtered[index]];
    auto const& tile = picker.tiles[index];
    picker.selectedSlug = entry.slug;
    auto label = entry.title + L" · " + CatalogSourceName(entry.source);
    label += entry.FollowsTextColor() ? L" · Follows text color" : L" · Original artwork";
    picker.selectedName.Text(label);
    if (picker.selectedFiles != tile.files) {
        picker.selectedFiles = tile.files;
        picker.preview.Source(tile.files.empty() ? nullptr : tile.image.Source());
    }
    picker.use.IsEnabled(!tile.files.empty());
}

void UpdateCatalogGridStatus(CatalogIconPicker& picker) {
    if (picker.entries.empty()) return;
    if (!picker.matchCount) {
        picker.status.Text(L"No matching icons. Try another name.");
        return;
    }
    size_t loaded{}, failed{};
    for (auto const& tile : picker.tiles) {
        loaded += !tile.files.empty();
        failed += tile.failed;
    }
    auto text = std::to_wstring(picker.tiles.size()) + L" of " +
                std::to_wstring(picker.matchCount) + L" icons.";
    if (loaded + failed < picker.tiles.size())
        text += L" Loading " + std::to_wstring(loaded) + L"/" + std::to_wstring(picker.tiles.size()) + L"...";
    else if (failed)
        text += L" Some icons could not load. Retry or choose another icon.";
    else if (picker.matchCount > picker.tiles.size())
        text += L" Type to narrow the grid.";
    picker.status.Text(text);
    picker.retry.Visibility(failed ? Visibility::Visible : Visibility::Collapsed);
}

void LoadCatalogThumbnails(SettingsEditor& editor) {
    auto& picker = editor.iconPicker;
    if (picker.searchPending) return;
    while (picker.open && picker.thumbnailJobs.size() + picker.retiredJobs.size() < 4) {
        size_t index = picker.tiles.size();
        int selected = picker.results.SelectedIndex();
        if (selected >= 0 && static_cast<size_t>(selected) < picker.tiles.size() &&
            !picker.tiles[selected].started) index = selected;
        if (index == picker.tiles.size())
            for (size_t i = 0; i < picker.tiles.size(); ++i)
                if (!picker.tiles[i].started) { index = i; break; }
        if (index == picker.tiles.size()) break;
        auto& tile = picker.tiles[index];
        tile.started = true;
        try {
            picker.thumbnailJobs.push_back(StartCatalogDownload(editor, index));
        } catch (...) {
            tile.failed = true;
            tile.loading.Text(L"!");
        }
    }
    UpdateCatalogGridStatus(picker);
}

void ReloadCatalogThumbnails(SettingsEditor& editor) {
    auto& picker = editor.iconPicker;
    RetireCatalogThumbnails(picker);
    bool light = CatalogPreviewLight(editor);
    auto background = Media::SolidColorBrush(light
        ? winrt::Windows::UI::Color{255,32,38,50} : winrt::Windows::UI::Color{255,243,245,248});
    picker.previewBackground.Background(background);
    for (auto& tile : picker.tiles) {
        tile.started = tile.failed = false;
        tile.files = {};
        tile.image.Source(nullptr);
        tile.background.Background(background);
        tile.loading.Text(L"…");
        tile.loading.Visibility(Visibility::Visible);
        tile.loading.Foreground(Media::SolidColorBrush(light
            ? winrt::Windows::UI::Color{255,255,255,255} : winrt::Windows::UI::Color{255,20,22,28}));
        tile.source.Foreground(tile.loading.Foreground());
    }
    UpdateCatalogSelection(editor);
    LoadCatalogThumbnails(editor);
}

void FilterCatalogIcons(SettingsEditor& editor) {
    auto& picker = editor.iconPicker;
    KillTimer(editor.window, kCatalogSearchTimerId);
    picker.searchPending = false;
    RetireCatalogThumbnails(picker);
    picker.filtering = true;
    picker.results.Items().Clear();
    picker.filtered.clear();
    picker.tiles.clear();
    picker.selectedSlug.clear();
    picker.selectedFiles = {};
    picker.preview.Source(nullptr);
    picker.selectedName.Text(L"Select an icon to preview it");
    picker.use.IsEnabled(false);
    auto query = CatalogSearchText(std::wstring(picker.search.Text()));
    size_t count{};
    for (size_t i = 0; i < picker.entries.size(); ++i) {
        auto const& entry = picker.entries[i];
        if (!query.empty() && entry.search.find(query) == entry.search.npos) continue;
        ++count;
        if (picker.filtered.size() >= 80) continue;
        picker.filtered.push_back(i);
        auto item = Markup::XamlReader::Load(LR"XAML(
<GridViewItem xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
              xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
              Width="72" Height="104" Padding="4" HorizontalContentAlignment="Stretch"
              VerticalContentAlignment="Stretch">
  <Border x:Name="TileBackground" CornerRadius="8" Padding="6">
    <Grid>
      <Grid.RowDefinitions><RowDefinition/><RowDefinition Height="Auto"/><RowDefinition Height="18"/></Grid.RowDefinitions>
      <TextBlock x:Name="Loading" Text="…" HorizontalAlignment="Center" VerticalAlignment="Center"/>
      <Image x:Name="Thumbnail" Stretch="Uniform"/>
      <TextBlock x:Name="Source" Grid.Row="1" FontSize="9" Margin="0,5,0,0"
                 HorizontalAlignment="Center" TextTrimming="CharacterEllipsis"/>
      <Border x:Name="AdaptiveBadge" Grid.Row="2" CornerRadius="4" Padding="5,0"
              Margin="0,3,0,0" HorizontalAlignment="Center" Visibility="Collapsed"
              Background="{ThemeResource SystemControlHighlightAccentBrush}">
        <TextBlock Text="Auto" FontSize="10" Foreground="{ThemeResource SystemControlForegroundChromeWhiteBrush}"/>
      </Border>
    </Grid>
  </Border>
</GridViewItem>)XAML").as<Controls::GridViewItem>();
        auto label = entry.title + L" · " + CatalogSourceName(entry.source);
        label += entry.FollowsTextColor() ? L" · Follows text color" : L" · Original artwork";
        Automation::AutomationProperties::SetName(item, label);
        Controls::ToolTipService::SetToolTip(item, winrt::box_value(label));
        item.FindName(L"AdaptiveBadge").as<Controls::Border>().Visibility(
            entry.FollowsTextColor() ? Visibility::Visible : Visibility::Collapsed);
        auto sourceLabel = item.FindName(L"Source").as<Controls::TextBlock>();
        sourceLabel.Text(entry.source == L"native" ? L"Dashboard" :
            entry.source == L"simpleicons" ? L"Simple" : CatalogSourceName(entry.source));
        sourceLabel.Foreground(Media::SolidColorBrush(CatalogPreviewLight(editor)
            ? winrt::Windows::UI::Color{255,255,255,255} : winrt::Windows::UI::Color{255,20,22,28}));
        CatalogIconTile tile;
        tile.source = sourceLabel;
        tile.image = item.FindName(L"Thumbnail").as<Controls::Image>();
        tile.background = item.FindName(L"TileBackground").as<Controls::Border>();
        tile.loading = item.FindName(L"Loading").as<Controls::TextBlock>();
        picker.tiles.push_back(std::move(tile));
        picker.results.Items().Append(item);
    }
    picker.filtering = false;
    picker.matchCount = count;
    ReloadCatalogThumbnails(editor);
}

void CloseCatalogPicker(SettingsEditor& editor) {
    auto& picker = editor.iconPicker;
    bool wasOpen = std::exchange(picker.open, false);
    KillTimer(editor.window, kCatalogSearchTimerId);
    picker.searchPending = false;
    ++picker.generation;
    StopCatalogJob(picker);
    StopCatalogThumbnails(picker);
    picker.searchChanged.revoke();
    picker.selectionChanged.revoke();
    picker.retryClick.revoke();
    picker.useClick.revoke();
    picker.cancelClick.revoke();
    picker.loaded.revoke();
    if (picker.panel && editor.root) {
        uint32_t index{};
        if (editor.root.Children().IndexOf(picker.panel, index))
            editor.root.Children().RemoveAt(index);
    }
    if (wasOpen) {
        editor.content.IsEnabled(true);
        auto found = std::find_if(editor.catalog.begin(), editor.catalog.end(),
            [&](auto const& item) { return item.key == picker.trayKey; });
        auto index = static_cast<size_t>(found - editor.catalog.begin());
        if (index < editor.trayRows.size()) editor.trayRows[index].choose.Focus(FocusState::Programmatic);
    }
    picker.panel = nullptr;
}

void OpenCatalogPicker(SettingsEditor& editor, std::wstring const& key) {
    if (!editor.valid) return;
    try {
        CloseCatalogPicker(editor);
        auto& picker = editor.iconPicker;
        picker.trayKey = key;
        picker.panel = Markup::XamlReader::Load(LR"XAML(
<Grid xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
      xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
      Background="#66000000" TabFocusNavigation="Cycle" AutomationProperties.Name="Dashboard Icons">
 <Border MaxWidth="520" Margin="24" Padding="24" CornerRadius="8"
         VerticalAlignment="Center" Background="{ThemeResource ApplicationPageBackgroundThemeBrush}"
         BorderThickness="0">
  <Grid>
   <Grid.RowDefinitions><RowDefinition Height="Auto"/><RowDefinition/><RowDefinition Height="Auto"/></Grid.RowDefinitions>
   <TextBlock Text="Dashboard Icons" FontSize="24" Margin="0,0,0,16"/>
   <ScrollViewer Grid.Row="1" VerticalScrollBarVisibility="Auto" HorizontalScrollBarVisibility="Disabled">
    <StackPanel Spacing="12">
    <TextBox x:Name="Search" PlaceholderText="Search brands, e.g. Steam or Discord"
             AutomationProperties.Name="Search Dashboard Icons"/>
    <GridView x:Name="Results" Height="228" SelectionMode="Single"
              ScrollViewer.HorizontalScrollMode="Disabled" ScrollViewer.HorizontalScrollBarVisibility="Disabled"
              ScrollViewer.VerticalScrollMode="Auto" ScrollViewer.VerticalScrollBarVisibility="Auto"
              AutomationProperties.Name="Matching icons">
      <GridView.ItemsPanel>
        <ItemsPanelTemplate><ItemsWrapGrid Orientation="Horizontal" MaximumRowsOrColumns="6"/></ItemsPanelTemplate>
      </GridView.ItemsPanel>
    </GridView>
    <Grid>
      <Grid.ColumnDefinitions><ColumnDefinition Width="80"/><ColumnDefinition/></Grid.ColumnDefinitions>
      <Border x:Name="PreviewBackground" Width="64" Height="64" CornerRadius="8" Background="#202632">
        <Image x:Name="IconPreview" Width="44" Height="44" Stretch="Uniform"/>
      </Border>
      <TextBlock x:Name="SelectedName" Grid.Column="1" VerticalAlignment="Center"
                 Text="Select an icon to preview it" TextWrapping="Wrap"/>
    </Grid>
    <TextBlock x:Name="Status" FontSize="12" Opacity="0.7" TextWrapping="Wrap"/>
    <Button x:Name="Retry" Content="Retry" Visibility="Collapsed"/>
    <TextBlock Text="Auto icons follow text color. Other icons use their original artwork." FontSize="12" Opacity="0.7" TextWrapping="Wrap"/>
    <HyperlinkButton Content="Dashboard Icons · Sources and attribution" NavigateUri="https://dashboardicons.com/icons"
                     Padding="0" HorizontalAlignment="Left"/>
    </StackPanel>
   </ScrollViewer>
   <Grid Grid.Row="2" Margin="0,20,0,0">
    <Grid.ColumnDefinitions><ColumnDefinition/><ColumnDefinition Width="8"/><ColumnDefinition/></Grid.ColumnDefinitions>
    <Button x:Name="Use" Content="Use icon" IsEnabled="False" HorizontalAlignment="Stretch" MinHeight="40"/>
    <Button x:Name="Cancel" Grid.Column="2" Content="Cancel" HorizontalAlignment="Stretch" MinHeight="40"/>
   </Grid>
  </Grid>
 </Border>
</Grid>)XAML").as<Controls::Grid>();
        picker.panel.RequestedTheme(editor.content.RequestedTheme());
        auto find = [&](PCWSTR name) { return picker.panel.FindName(name); };
        picker.search = find(L"Search").as<Controls::TextBox>();
        picker.results = find(L"Results").as<Controls::GridView>();
        picker.preview = find(L"IconPreview").as<Controls::Image>();
        picker.previewBackground = find(L"PreviewBackground").as<Controls::Border>();
        picker.selectedName = find(L"SelectedName").as<Controls::TextBlock>();
        picker.status = find(L"Status").as<Controls::TextBlock>();
        picker.retry = find(L"Retry").as<Controls::Button>();
        picker.use = find(L"Use").as<Controls::Button>();
        picker.cancel = find(L"Cancel").as<Controls::Button>();
        picker.searchChanged = picker.search.TextChanged(winrt::auto_revoke,
            [&editor](auto const&, auto const&) {
                auto& picker = editor.iconPicker;
                if (!picker.open) return;
                picker.searchPending = true;
                picker.selectedFiles = {};
                picker.use.IsEnabled(false);
                // Never change the grid or wait for I/O while TextBox is
                // processing a keystroke or an IME composition update.
                SetTimer(editor.window, kCatalogSearchTimerId, 150, nullptr);
            });
        picker.selectionChanged = picker.results.SelectionChanged(winrt::auto_revoke,
            [&editor](auto const&, auto const&) {
                auto& picker = editor.iconPicker;
                if (picker.filtering) return;
                UpdateCatalogSelection(editor);
                LoadCatalogThumbnails(editor);
            });
        picker.retryClick = picker.retry.Click(winrt::auto_revoke,
            [&editor](auto const&, auto const&) {
                if (editor.iconPicker.entries.empty()) StartCatalogJob(editor);
                else ReloadCatalogThumbnails(editor);
            });
        picker.useClick = picker.use.Click(winrt::auto_revoke,
            [&editor](auto const&, auto const&) {
                auto& picker = editor.iconPicker;
                if (picker.selectedFiles.empty()) return;
                editor.trayOverrides[picker.trayKey] = picker.selectedFiles;
                EditorChanged(editor);
                RebuildTrayRows(editor);
                CloseCatalogPicker(editor);
            });
        picker.cancelClick = picker.cancel.Click(winrt::auto_revoke,
            [&editor](auto const&, auto const&) { CloseCatalogPicker(editor); });
        picker.loaded = picker.panel.Loaded(winrt::auto_revoke,
            [&editor](auto const&, auto const&) {
                editor.iconPicker.search.Focus(FocusState::Keyboard);
            });
        picker.open = true;
        // A ContentDialog's popup can receive keys without inserting text in
        // a desktop XAML island. Keep the picker in the main input site.
        editor.root.Children().Append(picker.panel);
        editor.content.IsEnabled(false);
        if (picker.entries.empty()) StartCatalogJob(editor);
        else FilterCatalogIcons(editor);
    } catch (...) {
        CloseCatalogPicker(editor);
        EditorStatus(editor, L"Could not open the Dashboard Icons picker.");
    }
}

void CatalogIconResult(SettingsEditor& editor, UINT_PTR generation) {
    auto& picker = editor.iconPicker;
    if (!picker.open) return;
    auto retired = std::find_if(picker.retiredJobs.begin(), picker.retiredJobs.end(),
        [generation](auto const& job) { return job->generation == generation; });
    if (retired != picker.retiredJobs.end()) {
        if (!(*retired)->done) return;
        JoinCatalogJob(**retired);
        picker.retiredJobs.erase(retired);
        LoadCatalogThumbnails(editor);
        return;
    }
    auto thumbnail = std::find_if(picker.thumbnailJobs.begin(), picker.thumbnailJobs.end(),
        [generation](auto const& job) { return job->generation == generation; });
    if (thumbnail != picker.thumbnailJobs.end()) {
        auto job = *thumbnail;
        if (!job->done) return;
        JoinCatalogJob(*job);
        picker.thumbnailJobs.erase(thumbnail);
        if (job->tileIndex < picker.tiles.size()) {
            auto& tile = picker.tiles[job->tileIndex];
            tile.failed = !job->error.empty();
            if (!tile.failed) {
                tile.files = job->files;
                tile.image.Source(IconBitmap(job->pixels));
                tile.loading.Visibility(Visibility::Collapsed);
            } else {
                tile.loading.Text(L"!");
            }
        }
        UpdateCatalogSelection(editor);
        LoadCatalogThumbnails(editor);
        return;
    }
    if (!picker.job || picker.job->generation != generation || !picker.job->done) return;
    auto job = picker.job;
    StopCatalogJob(picker);
    if (!job->error.empty()) {
        picker.status.Text(job->error);
        picker.retry.Visibility(Visibility::Visible);
        return;
    }
    picker.entries = std::move(job->entries);
    FilterCatalogIcons(editor);
}

void ReloadEditor(SettingsEditor& editor) {
    editor.syncing = true;
    try {
        auto text = ReadLocalFile(g_settingsPath);
        settingsjson::JsonObject local{nullptr};
        if (text) {
            std::string_view input(*text);
            if (input.starts_with("\xEF\xBB\xBF")) input.remove_prefix(3);
            local = ParseSettingsToml(input);
        }
        auto values = ParseSettingsToml(SettingsDocument(local));
        editor.trayOverrides = ReadTrayOverrides(local);
        for (size_t i = 0; i < std::size(kSettingFields); ++i) {
            auto const& field = kSettingFields[i];
            auto const& control = editor.controls[i];
            if (field.kind == SettingKind::Toggle) {
                control.input.as<Controls::ToggleSwitch>().IsOn(values.GetNamedBoolean(field.key));
            } else if (field.kind == SettingKind::Choice) {
                auto value = values.GetNamedString(field.key);
                int index = 0;
                for (size_t j = 0; j < field.choices.size(); ++j)
                    if (value == field.choices[j].value) index = static_cast<int>(j);
                control.input.as<Controls::ComboBox>().SelectedIndex(index);
            } else if (field.kind == SettingKind::Color) {
                control.input.as<Controls::TextBox>().Text(LocalString(values, field.key));
            } else {
                int value = std::clamp(LocalInt(values, field.key), field.minimum, field.maximum);
                control.input.as<Controls::TextBox>().Text(EditorNumberText(value, field));
                control.slider.Value(value / field.displayDivisor);
            }
        }
        editor.originalText = std::move(text);
        editor.originalDocument = SettingsDocument(values);
        editor.valid = true;
        editor.dirty = false;
        EditorStatus(editor, L"Changes apply when you save.");
    } catch (...) {
        editor.valid = false;
        EditorStatus(editor, L"Cannot read settings. Open TOML to repair the file, then Reload.");
    }
    editor.syncing = false;
    UpdateEditorEnabled(editor);
    UpdateEditorValidation(editor);
    UpdateEditorSurface(editor);
    UpdateEditorPreview(editor);
    RebuildTrayRows(editor);
}

settingsjson::JsonObject EditorPageValues(SettingsEditor const& editor, int page) {
    settingsjson::JsonObject values;
    for (size_t i = 0; i < std::size(kSettingFields); ++i) {
        auto const& field = kSettingFields[i];
        if (field.page != page) continue;
        auto const& control = editor.controls[i];
        if (field.kind == SettingKind::Number) {
            double value = std::round(control.slider.Value() * field.displayDivisor) /
                           field.displayDivisor;
            values.Insert(field.key, settingsjson::JsonValue::CreateNumberValue(value));
        } else if (field.kind == SettingKind::Choice) {
            auto index = control.input.as<Controls::ComboBox>().SelectedIndex();
            if (index >= 0 && static_cast<size_t>(index) < field.choices.size())
                values.Insert(field.key, settingsjson::JsonValue::CreateStringValue(field.choices[index].value));
        } else if (field.kind == SettingKind::Color) {
            values.Insert(field.key, settingsjson::JsonValue::CreateStringValue(
                control.lastValidColor));
        }
    }
    return values;
}

bool SaveEditor(SettingsEditor& editor) {
    if (!editor.valid) return false;
    UpdateEditorValidation(editor);
    settingsjson::JsonObject values;
    for (size_t i = 0; i < std::size(kSettingFields); ++i) {
        auto const& field = kSettingFields[i];
        auto input = editor.controls[i].input;
        if (field.kind == SettingKind::Toggle) {
            values.Insert(field.key, settingsjson::JsonValue::CreateBooleanValue(
                input.as<Controls::ToggleSwitch>().IsOn()));
        } else if (field.kind == SettingKind::Choice) {
            auto index = input.as<Controls::ComboBox>().SelectedIndex();
            if (index < 0 || static_cast<size_t>(index) >= field.choices.size()) return false;
            values.Insert(field.key, settingsjson::JsonValue::CreateStringValue(field.choices[index].value));
        } else if (field.kind == SettingKind::Color) {
            auto color = input.as<Controls::TextBox>().Text();
            if(!input.IsEnabled() && !ValidGlassColor(color)) color=editor.controls[i].lastValidColor;
            if (!ValidGlassColor(color)) {
                EditorStatus(editor, L"Enter a tint color as #RRGGBB, for example #88AAFF.");
                editor.navigation.SelectedIndex(field.page);
                if(editor.controls[i].disclosure) editor.controls[i].disclosure.IsChecked(true);
                input.Focus(FocusState::Programmatic);
                input.StartBringIntoView();
                return false;
            }
            values.Insert(field.key, settingsjson::JsonValue::CreateStringValue(color));
        } else {
            int value{};
            if (!EditorNumber(input.as<Controls::TextBox>(), field, value)) {
                if(!input.IsEnabled()) {
                    value=static_cast<int>(std::lround(editor.controls[i].slider.Value()*field.displayDivisor));
                } else {
                    auto message=std::wstring(field.label)+L": "+EditorFieldError(editor.controls[i],field);
                    EditorStatus(editor,message.c_str());
                    editor.navigation.SelectedIndex(field.page);
                    if(editor.controls[i].disclosure) editor.controls[i].disclosure.IsChecked(true);
                    input.Focus(FocusState::Programmatic);
                    input.StartBringIntoView();
                    return false;
                }
            }
            values.Insert(field.key, settingsjson::JsonValue::CreateNumberValue(value / field.displayDivisor));
        }
    }
    AddTrayOverrides(values, editor.trayOverrides);
    auto document = SettingsDocument(values);
    if (ReadLocalFile(g_settingsPath) != editor.originalText) {
        EditorStatus(editor, L"The file changed outside this window. Reload before saving.");
        return false;
    }
    if (!editor.originalText || document != editor.originalDocument) {
        if (!WriteLocalFile(g_settingsPath, document, true)) {
            EditorStatus(editor, L"Could not save settings. Your previous file was kept.");
            return false;
        }
        editor.originalText = document;
        editor.originalDocument = document;
    }
    editor.dirty = false;
    UpdateEditorEnabled(editor);
    if (auto dispatcher = g_dispatch.load(); dispatcher &&
        PostMessageW(dispatcher, kApplySettingsMessage, reinterpret_cast<WPARAM>(editor.window), 0)) {
        EditorStatus(editor, L"Saved. Applying settings...");
    } else {
        EditorStatus(editor, L"Saved. Reload the mod to apply.");
    }
    return true;
}

void UpdateEditorTheme(SettingsEditor& editor) {
    using namespace winrt::Windows::UI::ViewManagement;
    DWORD light{}, size = sizeof(light);
    BOOL dark = RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &light, &size) == ERROR_SUCCESS
        ? !light : editor.systemColors.GetColorValue(UIColorType::Background).R < 128;
    auto theme = dark ? ElementTheme::Dark : ElementTheme::Light;
    bool changed = editor.content.RequestedTheme() != theme;
    if (editor.root) editor.root.RequestedTheme(theme);
    editor.content.RequestedTheme(theme);
    DwmSetWindowAttribute(editor.window, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
    if (editor.iconPicker.open) {
        editor.iconPicker.panel.RequestedTheme(theme);
        if (changed) ReloadCatalogThumbnails(editor);
    }
    LayoutEditor(editor);
    if (changed) RebuildTrayRows(editor);
}

bool EditorCaptionButton(int hit) {
    return hit == HTMINBUTTON || hit == HTMAXBUTTON || hit == HTCLOSE;
}

LRESULT EditorFrameHitTest(SettingsEditor const& editor, POINT point) {
    RECT rect{};
    if (!GetWindowRect(editor.window, &rect)) return HTCLIENT;
    if (point.x < rect.left || point.x >= rect.right ||
        point.y < rect.top || point.y >= rect.bottom) return HTNOWHERE;
    if (!IsZoomed(editor.window)) {
        int border = std::max(1, EditorScale(editor, 6));
        bool left = point.x < rect.left + border, right = point.x >= rect.right - border;
        bool top = point.y < rect.top + border, bottom = point.y >= rect.bottom - border;
        if (top) return left ? HTTOPLEFT : right ? HTTOPRIGHT : HTTOP;
        if (bottom) return left ? HTBOTTOMLEFT : right ? HTBOTTOMRIGHT : HTBOTTOM;
        if (left) return HTLEFT;
        if (right) return HTRIGHT;
    }
    RECT client{};
    POINT origin{};
    if (GetClientRect(editor.window, &client) && ClientToScreen(editor.window, &origin)) {
        rect = {origin.x, origin.y, origin.x + client.right, origin.y + client.bottom};
    }
    if (point.y < rect.top + EditorScale(editor, kEditorCaptionHeight)) {
        int button = EditorScale(editor, kEditorCaptionButtonWidth);
        if (point.x >= rect.right - button) return HTCLOSE;
        if (point.x >= rect.right - button * 2) return HTMAXBUTTON;
        if (point.x >= rect.right - button * 3) return HTMINBUTTON;
        return HTCAPTION;
    }
    POINT local{point.x - rect.left, point.y - rect.top};
    if (PtInRect(&editor.headerBounds, local)) return HTCAPTION;
    return HTCLIENT;
}

winrt::Windows::UI::Color EditorCaptionTint(SettingsEditor const& editor, size_t i) {
    int state = editor.captionStates[i];
    if (!state) return {};
    if (i == 2) return {BYTE(state == 2 ? 210 : 176), 196, 43, 28};
    BYTE channel = editor.content.RequestedTheme() == ElementTheme::Dark ? 255 : 0;
    return {BYTE(state == 2 ? 72 : 40), channel, channel, channel};
}

void UpdateEditorCaption(SettingsEditor& editor) {
    if (!editor.maximizeGlyph) return;
    bool maximized = IsZoomed(editor.window);
    constexpr int hits[]{HTMINBUTTON, HTMAXBUTTON, HTCLOSE};
    std::array<int, 3> states{};
    for (size_t i = 0; i < states.size(); ++i)
        states[i] = editor.captionButtons[i].IsPressed() ||
            (editor.captionPressed == hits[i] && editor.captionHovered == hits[i]) ? 2 :
            editor.captionHovered == hits[i] ? 1 : 0;
    bool changed = states != editor.captionStates;
    int appearance = editor.surfaceAppearance * 4 + (maximized ? 2 : 0) +
                     (editor.backgroundRenderer ? 1 : 0);
    if (!changed && appearance == editor.captionAppearance) return;
    editor.captionStates = states;
    editor.captionAppearance = appearance;
    editor.maximizeGlyph.Glyph(maximized ? L"\xE923" : L"\xE922");
    PCWSTR name = maximized ? L"Restore down" : L"Maximize";
    Automation::AutomationProperties::SetName(editor.captionButtons[1], name);
    Controls::ToolTipService::SetToolTip(editor.captionButtons[1], winrt::box_value(name));
    bool contrast = editor.surfaceAppearance >= 10;
    auto systemColor = [](int index) {
        COLORREF color = GetSysColor(index);
        return winrt::Windows::UI::Color{255, GetRValue(color), GetGValue(color), GetBValue(color)};
    };
    for (size_t i = 0; i < editor.captionButtons.size(); ++i) {
        auto button = editor.captionButtons[i];
        auto tint = EditorCaptionTint(editor, i);
        button.CornerRadius(i == 2 && !maximized ? CornerRadius{0,8,0,0} : CornerRadius{});
        if (contrast && states[i])
            button.Foreground(Media::SolidColorBrush(systemColor(COLOR_HIGHLIGHTTEXT)));
        else button.Foreground(i == 2 && states[i] ?
            Media::SolidColorBrush({255,255,255,255}) : editor.content.Foreground());
        if (states[i] && !editor.backgroundRenderer && !contrast) {
            Media::AcrylicBrush glass;
            glass.BackgroundSource(Media::AcrylicBackgroundSource::Backdrop);
            double opacity = tint.A / 255.0;
            glass.TintOpacity(opacity);
            BYTE background = editor.content.RequestedTheme() == ElementTheme::Dark ? 32 : 243;
            auto blend = [&](BYTE channel) {
                return BYTE(std::lround(channel * opacity + background * (1 - opacity)));
            };
            glass.FallbackColor({255, blend(tint.R), blend(tint.G), blend(tint.B)});
            tint.A = 255;
            glass.TintColor(tint);
            glass.TintLuminosityOpacity(0.08);
            button.Background(glass);
        } else {
            button.Background(Media::SolidColorBrush(contrast && states[i] ?
                systemColor(COLOR_HIGHLIGHT) :
                winrt::Windows::UI::Color{0,0,0,0}));
        }
        VisualStateManager::GoToState(button,
            states[i] == 2 ? L"Pressed" : states[i] ? L"PointerOver" : L"Normal", false);
    }
    if (changed) UpdateEditorSurface(editor, false);
}

LRESULT CALLBACK EditorIslandFrameProc(HWND window, UINT message, WPARAM wp, LPARAM lp) {
    auto* editor = reinterpret_cast<SettingsEditor*>(GetPropW(window, kEditorFrameProperty));
    if (!editor || !editor->childProc) return DefWindowProcW(window, message, wp, lp);
    auto original = editor->childProc;
    if (message == WM_NCHITTEST) {
        POINT point{short(LOWORD(lp)), short(HIWORD(lp))};
        auto hit = EditorFrameHitTest(*editor, point);
        if (hit != HTCLIENT && hit != HTNOWHERE) return HTTRANSPARENT;
    }
    if (message == WM_NCDESTROY) {
        RemovePropW(window, kEditorFrameProperty);
        editor->childProc = nullptr;
    }
    return CallWindowProcW(original, window, message, wp, lp);
}

void ApplyEditorLayout(SettingsEditor& editor,double width,double textScale) {
    if(!editor.surface || !editor.preview.canvas) return;
    textScale=std::clamp(textScale,1.0,3.0);
    bool compact=textScale>1.35 || width<924*textScale;
    editor.surface.ColumnDefinitions().GetAt(0).Width({std::min(300.0,184*textScale),GridUnitType::Pixel});
    editor.surface.ColumnDefinitions().GetAt(2).Width({compact?0.0:420*textScale,GridUnitType::Pixel});
    editor.surface.RowDefinitions().GetAt(1).Height(compact
        ?GridLength{std::min(240.0,200+40*(textScale-1)),GridUnitType::Pixel}
        :GridLength{1,GridUnitType::Star});
    editor.surface.RowDefinitions().GetAt(2).Height(compact
        ?GridLength{1,GridUnitType::Star}:GridLength{0,GridUnitType::Pixel});
    Controls::Grid::SetColumn(editor.inspector,compact?1:2);
    Controls::Grid::SetRow(editor.inspector,compact?2:1);
    auto workspace=editor.content.FindName(L"PreviewWorkspace").as<FrameworkElement>();
    workspace.Margin(compact?Thickness{20,8,12,8}:Thickness{24,16,24,12});
    editor.preview.caption.Visibility(compact?Visibility::Collapsed:Visibility::Visible);
    double actionWidth=(compact?width-std::min(300.0,184*textScale):420*textScale)-44;
    bool stacked=actionWidth<320*textScale;
    Controls::Grid::SetRow(editor.reload,stacked?1:0);
    Controls::Grid::SetRow(editor.openFile,stacked?1:0);
    Controls::Grid::SetColumn(editor.save,stacked?0:2);
    Controls::Grid::SetColumnSpan(editor.save,stacked?3:1);
    editor.reload.Margin({0,stacked?8.0:0.0,0,0});
    editor.openFile.Margin({8,stacked?8.0:0.0,8,0});
    if(editor.layoutScale!=textScale) {
        for(size_t i=0;i<editor.controls.size();++i) {
            auto kind=kSettingFields[i].kind;
            if(kind==SettingKind::Choice) editor.controls[i].input.Width(152*textScale);
            else if(kind==SettingKind::Number) editor.controls[i].input.Width(64*textScale);
            else if(kind==SettingKind::Color) editor.controls[i].input.Width(105*textScale);
        }
        editor.layoutScale=textScale;
    }
    if(editor.compactLayout!=compact) {
        editor.compactLayout=compact;
        ApplyPreviewMetrics(editor,editor.preview.metrics);
    }
}

void LayoutEditor(SettingsEditor& editor) {
    if (!editor.child) return;
    RECT client{};
    GetClientRect(editor.window, &client);
    SetWindowPos(editor.child, nullptr, 0, 0, client.right, client.bottom,
                  SWP_NOZORDER | SWP_NOACTIVATE);
    ApplyEditorLayout(editor,client.right*96.0/editor.dpi,editor.systemColors.TextScaleFactor());
    UpdateEditorCaption(editor);
    UpdateEditorSurface(editor);
    UpdateEditorPreview(editor);
}

void FocusEditor(SettingsEditor& editor, Hosting::XamlSourceFocusNavigationReason reason) {
    // A hidden/not-yet-arranged island can immediately request focus back.
    if (editor.navigatingFocus) return;
    editor.navigatingFocus = true;
    struct Reset { bool& value; ~Reset() { value = false; } } reset{editor.navigatingFocus};
    if (GetFocus() == editor.window && editor.child) SetFocus(editor.child);
    editor.island.NavigateFocus(Hosting::XamlSourceFocusNavigationRequest(reason));
}

bool RouteEditorFrameInput(SettingsEditor& editor, MSG const& message) {
    if (message.message == WM_POINTERLEAVE || message.message == WM_MOUSELEAVE) {
        if (editor.framePointerOver) SendMessageW(editor.window, WM_NCMOUSELEAVE, 0, 0);
        return false;
    }
    bool pointer = message.message == WM_POINTERDOWN || message.message == WM_POINTERUP ||
                   message.message == WM_POINTERUPDATE;
    bool mouse = message.message >= WM_MOUSEMOVE && message.message <= WM_MBUTTONDBLCLK;
    if (!pointer && !mouse) return false;
    POINT point{short(LOWORD(message.lParam)), short(HIWORD(message.lParam))};
    if (!pointer) ClientToScreen(message.hwnd, &point);
    LPARAM position = MAKELPARAM(point.x, point.y);
    int hit = int(SendMessageW(editor.window, WM_NCHITTEST, 0, position));
    bool captured = GetCapture() == editor.window;
    if (!captured && (hit == HTCLIENT || hit == HTNOWHERE)) {
        if (editor.framePointerOver)
            SendMessageW(editor.window, WM_NCMOUSELEAVE, 0, 0);
        return false;
    }
    if (pointer) {
        // XAML's hidden InputSite receives mouse input as pointer messages.
        // Default processing promotes them to real mouse messages, updating the
        // native button state required by the caption move/snap loop.
        DefWindowProcW(message.hwnd, message.message, message.wParam, message.lParam);
        return true;
    }
    // A caption button must also receive its release when dragged into the body.
    if (captured) {
        ScreenToClient(editor.window, &point);
        SendMessageW(editor.window, message.message, message.wParam, MAKELPARAM(point.x, point.y));
        return true;
    }
    UINT nativeMessage = message.message - WM_MOUSEMOVE + WM_NCMOUSEMOVE;
    if (message.message == WM_LBUTTONDOWN && hit == HTCAPTION) {
        // InputSite doesn't classify caption double-clicks. Use Windows' timing
        // and distance, then let DefWindowProc perform the native caption action.
        bool doubleClick = editor.captionClickTime &&
            message.time - editor.captionClickTime <= GetDoubleClickTime() &&
            std::abs(point.x-editor.captionClickPoint.x) <= GetSystemMetrics(SM_CXDOUBLECLK)/2 &&
            std::abs(point.y-editor.captionClickPoint.y) <= GetSystemMetrics(SM_CYDOUBLECLK)/2;
        editor.captionClickTime = doubleClick ? 0 : message.time;
        editor.captionClickPoint = point;
        if (doubleClick) nativeMessage = WM_NCLBUTTONDBLCLK;
    }
    SendMessageW(editor.window, nativeMessage, hit, position);
    return true;
}

void CreateEditorContent(SettingsEditor& editor) {
    editor.content = Markup::XamlReader::Load(LR"XAML(
<UserControl xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
             xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml" IsTabStop="False" Background="Transparent">
<Grid x:Name="EditorSurface" Background="{ThemeResource ApplicationPageBackgroundThemeBrush}">
  <Grid.Resources>
    <Style x:Key="CaptionButtonStyle" TargetType="Button">
      <Setter Property="MinHeight" Value="0"/>
      <Setter Property="HorizontalContentAlignment" Value="Center"/>
      <Setter Property="VerticalContentAlignment" Value="Center"/>
      <Setter Property="UseSystemFocusVisuals" Value="True"/>
      <Setter Property="Template">
        <Setter.Value>
          <ControlTemplate TargetType="Button">
            <Grid Background="Transparent">
              <VisualStateManager.VisualStateGroups>
                <VisualStateGroup x:Name="CommonStates">
                  <VisualState x:Name="Normal"/>
                  <VisualState x:Name="PointerOver"/>
                  <VisualState x:Name="Pressed"/>
                  <VisualState x:Name="Disabled">
                    <VisualState.Setters><Setter Target="Presenter.Opacity" Value="0.4"/></VisualState.Setters>
                  </VisualState>
                </VisualStateGroup>
              </VisualStateManager.VisualStateGroups>
              <Border Background="{TemplateBinding Background}" CornerRadius="{TemplateBinding CornerRadius}"/>
              <ContentPresenter x:Name="Presenter" Content="{TemplateBinding Content}"
                  ContentTemplate="{TemplateBinding ContentTemplate}" Foreground="{TemplateBinding Foreground}"
                  HorizontalAlignment="{TemplateBinding HorizontalContentAlignment}"
                  VerticalAlignment="{TemplateBinding VerticalContentAlignment}"/>
            </Grid>
          </ControlTemplate>
        </Setter.Value>
      </Setter>
    </Style>
    <Style TargetType="Button">
      <Setter Property="MinHeight" Value="36"/>
      <Setter Property="Padding" Value="12,8"/>
      <Setter Property="CornerRadius" Value="8"/>
      <Setter Property="UseSystemFocusVisuals" Value="True"/>
    </Style>
    <Style TargetType="ComboBox">
      <Setter Property="MinHeight" Value="36"/>
      <Setter Property="CornerRadius" Value="7"/>
    </Style>
    <Style TargetType="TextBox">
      <Setter Property="MinHeight" Value="32"/>
      <Setter Property="CornerRadius" Value="7"/>
    </Style>
  </Grid.Resources>
  <Grid.RowDefinitions>
    <RowDefinition Height="48"/><RowDefinition/><RowDefinition Height="0"/>
  </Grid.RowDefinitions>
  <Grid.ColumnDefinitions>
    <ColumnDefinition Width="184"/><ColumnDefinition/><ColumnDefinition Width="420"/>
  </Grid.ColumnDefinitions>
  <Grid x:Name="EditorBodySource" Grid.RowSpan="3" Grid.ColumnSpan="3" IsHitTestVisible="False"
        Background="{ThemeResource ApplicationPageBackgroundThemeBrush}">
    <Border x:Name="EditorBodyBackdrop"/>
    <Border x:Name="EditorBodyMaterial"/>
  </Grid>
  <Grid x:Name="EditorLiquid" Grid.RowSpan="3" Grid.ColumnSpan="3" IsHitTestVisible="False"/>

  <Grid x:Name="EditorTitlebar" Grid.Column="1" Grid.ColumnSpan="2" Background="Transparent">
    <StackPanel Orientation="Horizontal" HorizontalAlignment="Right" VerticalAlignment="Top">
      <Button x:Name="MinimizeWindow" Style="{StaticResource CaptionButtonStyle}" Width="46" Height="44" MinWidth="0" Padding="0"
              CornerRadius="0" Background="Transparent" BorderThickness="0"
              AutomationProperties.Name="Minimize" ToolTipService.ToolTip="Minimize">
        <FontIcon Glyph="&#xE921;" FontSize="11"/>
      </Button>
      <Button x:Name="MaximizeWindow" Style="{StaticResource CaptionButtonStyle}" Width="46" Height="44" MinWidth="0" Padding="0"
              CornerRadius="0" Background="Transparent" BorderThickness="0"
              AutomationProperties.Name="Maximize" ToolTipService.ToolTip="Maximize">
        <FontIcon x:Name="MaximizeGlyph" Glyph="&#xE922;" FontSize="11"/>
      </Button>
      <Button x:Name="CloseWindow" Style="{StaticResource CaptionButtonStyle}" Width="46" Height="44" MinWidth="0" Padding="0"
              CornerRadius="0" Background="Transparent" BorderThickness="0"
              AutomationProperties.Name="Close" ToolTipService.ToolTip="Close">
        <FontIcon Glyph="&#xE8BB;" FontSize="11"/>
      </Button>
    </StackPanel>
  </Grid>

  <Border x:Name="EditorSidebar" Grid.RowSpan="3" CornerRadius="32"
          Margin="12,12,0,12" Padding="8,18,8,16"
          Background="{ThemeResource SystemControlBackgroundAltHighBrush}">
    <Grid>
      <Grid.RowDefinitions><RowDefinition Height="Auto"/><RowDefinition/><RowDefinition Height="Auto"/></Grid.RowDefinitions>
      <StackPanel x:Name="EditorHeader" Spacing="6" Margin="12,0,12,20" IsHitTestVisible="False">
        <FontIcon x:Name="EditorMark" Glyph="&#xE7F4;" FontSize="24" HorizontalAlignment="Left"/>
        <TextBlock Text="Ultrawide" FontSize="20" FontWeight="SemiBold" TextWrapping="Wrap" Margin="0,4,0,0"/>
        <TextBlock Text="Taskbar settings" FontSize="12" TextWrapping="Wrap"/>
      </StackPanel>
      <ListBox x:Name="Navigation" Grid.Row="1" Background="Transparent" BorderThickness="0"
               SelectedIndex="0" AutomationProperties.Name="Settings pages">
        <ListBoxItem Padding="10" MinHeight="44" CornerRadius="8" Margin="0,0,0,5" AutomationProperties.Name="Taskbar">
          <Grid>
            <Grid.ColumnDefinitions><ColumnDefinition Width="Auto"/><ColumnDefinition/></Grid.ColumnDefinitions>
            <FontIcon Glyph="&#xE790;" FontSize="15" Margin="0,0,10,0"/>
            <TextBlock Grid.Column="1" Text="Taskbar" FontSize="13" TextWrapping="Wrap"/>
          </Grid>
        </ListBoxItem>
        <ListBoxItem Padding="10" MinHeight="44" CornerRadius="8" Margin="0,0,0,5" AutomationProperties.Name="Liquid glass">
          <Grid>
            <Grid.ColumnDefinitions><ColumnDefinition Width="Auto"/><ColumnDefinition/></Grid.ColumnDefinitions>
            <FontIcon Glyph="&#xE81E;" FontSize="15" Margin="0,0,10,0"/>
            <TextBlock Grid.Column="1" Text="Liquid glass" FontSize="13" TextWrapping="Wrap"/>
          </Grid>
        </ListBoxItem>
        <ListBoxItem Padding="10" MinHeight="44" CornerRadius="8" AutomationProperties.Name="Tray icons">
          <Grid>
            <Grid.ColumnDefinitions><ColumnDefinition Width="Auto"/><ColumnDefinition/></Grid.ColumnDefinitions>
            <FontIcon Glyph="&#xE91B;" FontSize="15" Margin="0,0,10,0"/>
            <TextBlock Grid.Column="1" Text="Tray icons" FontSize="13" TextWrapping="Wrap"/>
          </Grid>
        </ListBoxItem>
        <ListBoxItem Padding="10" MinHeight="44" CornerRadius="8" AutomationProperties.Name="Settings UI">
          <Grid>
            <Grid.ColumnDefinitions><ColumnDefinition Width="Auto"/><ColumnDefinition/></Grid.ColumnDefinitions>
            <FontIcon Glyph="&#xE713;" FontSize="15" Margin="0,0,10,0"/>
            <TextBlock Grid.Column="1" Text="Settings UI" FontSize="13" TextWrapping="Wrap"/>
          </Grid>
        </ListBoxItem>
      </ListBox>
    </Grid>
  </Border>

  <Grid x:Name="PreviewWorkspace" Grid.Row="1" Grid.Column="1" Margin="24,16,24,12">
    <Grid.RowDefinitions><RowDefinition Height="Auto"/><RowDefinition/><RowDefinition Height="Auto"/></Grid.RowDefinitions>
    <Grid>
      <Grid.ColumnDefinitions><ColumnDefinition/><ColumnDefinition Width="Auto"/></Grid.ColumnDefinitions>
      <StackPanel Spacing="6" Margin="0,0,12,0">
        <TextBlock Text="Preview" FontSize="20" FontWeight="SemiBold" TextWrapping="Wrap"/>
        <TextBlock x:Name="PreviewCaption" Text="Windows app samples" FontSize="12"/>
      </StackPanel>
      <HyperlinkButton x:Name="PreviewRefresh" Grid.Column="1" Content="Refresh" Padding="4,0" MinHeight="32"
                       HorizontalAlignment="Right" VerticalAlignment="Top"
                       ToolTipService.ToolTip="Measure the taskbar on this monitor"/>
    </Grid>
    <Viewbox Grid.Row="1" Stretch="Uniform" HorizontalAlignment="Stretch" VerticalAlignment="Center">
      <Canvas x:Name="PreviewCanvas" Width="640" Height="420" IsHitTestVisible="False"
              AutomationProperties.Name="Taskbar appearance preview">
        <Image x:Name="PreviewBackdrop" Width="640" Height="420"/>
        <Border x:Name="PreviewMaterial" Width="544" Height="48" Canvas.Left="48"/>
        <Grid x:Name="PreviewLiquid" Width="640" Height="420"/>
      </Canvas>
    </Viewbox>
    <StackPanel Grid.Row="2" Spacing="6" Margin="0,8,0,0">
      <TextBlock x:Name="PreviewMeasurement" Text="Measuring taskbar..." FontSize="12"
                 TextWrapping="Wrap"/>
    </StackPanel>
  </Grid>

  <Border x:Name="EditorInspector" Grid.Row="1" Grid.Column="2" Margin="0,8,12,12"
          CornerRadius="32" Background="{ThemeResource SystemControlBackgroundAltHighBrush}">
    <Grid>
    <Grid.RowDefinitions><RowDefinition/><RowDefinition Height="Auto"/></Grid.RowDefinitions>
    <ScrollViewer x:Name="SettingsScroll" Padding="16,20,16,20"
                  VerticalScrollBarVisibility="Auto" HorizontalScrollBarVisibility="Disabled"
                  HorizontalContentAlignment="Stretch" IsTabStop="False">
      <Grid>
        <StackPanel x:Name="AppearancePage">
          <TextBlock Text="Taskbar appearance" FontSize="22" FontWeight="SemiBold" TextWrapping="Wrap" Margin="0,0,0,8"/>
          <TextBlock Text="Shape your dock and choose how its content looks." FontSize="12" TextWrapping="Wrap"/>
          <StackPanel x:Name="AppearanceFields"/>
        </StackPanel>
        <StackPanel x:Name="LiquidPage" Visibility="Collapsed">
          <TextBlock Text="Liquid glass" FontSize="22" FontWeight="SemiBold" TextWrapping="Wrap" Margin="0,0,0,8"/>
          <TextBlock Text="Tune the light, depth, and finish." FontSize="12" TextWrapping="Wrap"/>
          <TextBlock x:Name="LiquidHint" TextWrapping="Wrap" FontSize="12" Margin="0,12,0,12"/>
          <Button x:Name="EnableLiquid" Content="Preview Liquid glass" HorizontalAlignment="Left"/>
          <StackPanel x:Name="LiquidFields"/>
        </StackPanel>
        <StackPanel x:Name="TrayPage" Visibility="Collapsed">
          <TextBlock Text="Tray icons" FontSize="22" FontWeight="SemiBold" TextWrapping="Wrap" Margin="0,0,0,8"/>
          <TextBlock Text="Unify icon colors or choose your own artwork." FontSize="12" TextWrapping="Wrap" Margin="0,0,0,16"/>
          <StackPanel x:Name="TrayFields"/>
          <TextBlock Text="Turning customization off keeps your choices saved." FontSize="12"
                     TextWrapping="Wrap" Margin="0,4,0,16"/>
          <Grid Margin="0,0,0,12">
            <Grid.ColumnDefinitions><ColumnDefinition/><ColumnDefinition Width="Auto"/></Grid.ColumnDefinitions>
            <TextBlock Text="App tray icons" FontSize="14" FontWeight="SemiBold" TextWrapping="Wrap" VerticalAlignment="Center" Margin="0,0,12,0"/>
            <Button x:Name="RefreshTray" Grid.Column="1" Content="Refresh" HorizontalAlignment="Right"/>
          </Grid>
          <TextBlock x:Name="TrayStatus" FontSize="12" TextWrapping="Wrap" Margin="0,0,0,12"/>
          <StackPanel x:Name="TrayList"/>
        </StackPanel>
        <StackPanel x:Name="UiPage" Visibility="Collapsed">
          <TextBlock Text="Settings UI" FontSize="22" FontWeight="SemiBold" TextWrapping="Wrap" Margin="0,0,0,8"/>
          <TextBlock Text="Choose the window material and customize its glass surfaces."
                     FontSize="12" TextWrapping="Wrap"/>
          <StackPanel x:Name="UiFields"/>
        </StackPanel>
      </Grid>
    </ScrollViewer>
  <Border x:Name="EditorFooter" Grid.Row="1" Margin="16,12,16,16">
    <StackPanel Spacing="12">
      <TextBlock x:Name="Status" TextWrapping="Wrap" FontSize="12" VerticalAlignment="Center"
                 AutomationProperties.LiveSetting="Polite"/>
      <Grid x:Name="EditorActions">
        <Grid.RowDefinitions><RowDefinition Height="Auto"/><RowDefinition Height="Auto"/></Grid.RowDefinitions>
        <Grid.ColumnDefinitions><ColumnDefinition Width="Auto"/><ColumnDefinition Width="Auto"/><ColumnDefinition/></Grid.ColumnDefinitions>
        <Button x:Name="Reload" Content="Reload" ToolTipService.ToolTip="Discard unsaved changes and reread the file"/>
        <Button x:Name="OpenFile" Grid.Column="1" Content="Open TOML" Margin="8,0,8,0"/>
        <Button x:Name="Save" Grid.Column="2" Content="Save &amp; apply" FontWeight="SemiBold" HorizontalAlignment="Right"
                 ToolTipService.ToolTip="Save and apply (Ctrl+S)"/>
      </Grid>
    </StackPanel>
  </Border>
    </Grid>
  </Border>
</Grid>
</UserControl>)XAML").as<Controls::UserControl>();
    auto find = [&](PCWSTR name) { return editor.content.FindName(name); };
    editor.surface = find(L"EditorSurface").as<Controls::Grid>();
    editor.header = find(L"EditorHeader").as<FrameworkElement>();
    editor.liquidBackground = find(L"EditorLiquid").as<Controls::Grid>();
    editor.bodySource = find(L"EditorBodySource").as<Controls::Grid>();
    editor.bodyMaterial = find(L"EditorBodyMaterial").as<Controls::Border>();
    auto sourceVisual = Hosting::ElementCompositionPreview::GetElementVisual(editor.bodySource);
    auto compositor = sourceVisual.Compositor();
    editor.bodySurface = compositor.CreateVisualSurface();
    editor.bodySurface.SourceVisual(sourceVisual);
    editor.bodySample = compositor.CreateSpriteVisual();
    editor.bodySample.Brush(compositor.CreateSurfaceBrush(editor.bodySurface));
    editor.bodyBackdrop = compositor.CreateSpriteVisual();
    Hosting::ElementCompositionPreview::SetElementChildVisual(
        find(L"EditorBodyBackdrop").as<Controls::Border>(), editor.bodyBackdrop);
    sourceVisual.Opacity(0); // VisualSurface samples content without the source root's opacity.
    editor.sidebar = find(L"EditorSidebar").as<Controls::Border>();
    editor.inspector = find(L"EditorInspector").as<Controls::Border>();
    editor.footer = find(L"EditorFooter").as<Controls::Border>();
    editor.captionButtons = {find(L"MinimizeWindow").as<Controls::Button>(),
                             find(L"MaximizeWindow").as<Controls::Button>(),
                             find(L"CloseWindow").as<Controls::Button>()};
    editor.maximizeGlyph = find(L"MaximizeGlyph").as<Controls::FontIcon>();
    for (size_t i = 0; i < editor.captionButtons.size(); ++i)
        editor.captionPressedTokens[i] = editor.captionButtons[i].RegisterPropertyChangedCallback(
            Controls::Primitives::ButtonBase::IsPressedProperty(),
            [&editor](auto const&, auto const&) { UpdateEditorCaption(editor); });
    for (size_t i = 0; i < editor.captionButtons.size(); ++i)
        editor.captionClicks[i] = editor.captionButtons[i].Click(winrt::auto_revoke,
            [&editor, i](auto const&, auto const&) {
                UINT command = i == 0 ? SC_MINIMIZE : i == 2 ? SC_CLOSE :
                    IsZoomed(editor.window) ? SC_RESTORE : SC_MAXIMIZE;
                PostMessageW(editor.window, WM_SYSCOMMAND, command, 0);
            });
    editor.navigation = find(L"Navigation").as<Controls::ListBox>();
    editor.scroll = find(L"SettingsScroll").as<Controls::ScrollViewer>();
    editor.pages = {find(L"AppearancePage").as<Controls::StackPanel>(),
                    find(L"LiquidPage").as<Controls::StackPanel>(),
                    find(L"TrayPage").as<Controls::StackPanel>(),
                    find(L"UiPage").as<Controls::StackPanel>()};
    editor.status = find(L"Status").as<Controls::TextBlock>();
    editor.save = find(L"Save").as<Controls::Button>();
    editor.reload = find(L"Reload").as<Controls::Button>();
    editor.openFile = find(L"OpenFile").as<Controls::Button>();
    editor.enableLiquid = find(L"EnableLiquid").as<Controls::Button>();
    editor.liquidHint = find(L"LiquidHint").as<Controls::TextBlock>();
    auto appearance = find(L"AppearanceFields").as<Controls::StackPanel>();
    auto liquid = find(L"LiquidFields").as<Controls::StackPanel>();
    auto tray = find(L"TrayFields").as<Controls::StackPanel>();
    auto ui = find(L"UiFields").as<Controls::StackPanel>();
    editor.trayList = find(L"TrayList").as<Controls::StackPanel>();
    editor.trayStatus = find(L"TrayStatus").as<Controls::TextBlock>();
    editor.refreshTray = find(L"RefreshTray").as<Controls::Button>();
    editor.syncing = true;
    constexpr size_t displayOrder[]{
        0, 1, 36, 3, 4, 5, 6, 7, 8, 100, 101, 19, 2,
        9, 20, 21, 10, 22, 23, 24, 11, 25, 12, 26,
        29, 14, 13, 15, 30, 31, 32, 16, 17, 18, 27, 28, 33, 34, 35,
        60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
        42, 58, 59, 37,
        43, 44, 39, 40, 45, 46, 47, 48, 49, 50,
        54, 41, 53, 55, 56, 57,
        38, 51, 52,
        80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99};
    static_assert(std::size(displayOrder) == std::size(kSettingFields));
    std::array<Controls::StackPanel,4> containers{appearance,liquid,tray,ui};
    auto targets=containers;
    std::array<Controls::Primitives::ToggleButton,4> disclosures{nullptr,nullptr,nullptr,nullptr};
    for (size_t i : displayOrder) {
        auto const& field = kSettingFields[i];
        auto& control = editor.controls[i];
        auto fields = targets[field.page];
        PCWSTR section = i == 100 ? L"Tray highlights" : i == 0 ? L"Style & color" : i == 3 ? L"Geometry" :
            i == 7 ? L"Buttons" : i == 9 ? L"Material" : i == 21 ? L"Surface" :
            i == 11 ? L"Refraction" : i == 29 ? L"Lighting" :
            i == 16 ? L"Finish" : i == 33 ? L"Shadow" :
            i == 42 ? L"Window appearance" : i == 43 ? L"Glass shape" :
            i == 54 ? L"Lighting" : i == 38 ? L"Finish" :
            i == 60 || i == 80 ? L"Edge shaping" :
            i == 68 || i == 88 ? L"Frost & color" :
            i == 72 || i == 92 ? L"Interaction & glints" : nullptr;
        if (section) {
            fields=containers[field.page];
            disclosures[field.page]=nullptr;
            if((field.page==1 && i!=9) || (field.page==3 && i!=42)) {
                EditorSection group;
                group.page=field.page;group.title=section;
                group.header=Controls::Primitives::ToggleButton();
                group.header.IsChecked(false);
                group.header.HorizontalAlignment(HorizontalAlignment::Stretch);
                group.header.HorizontalContentAlignment(HorizontalAlignment::Left);
                group.header.MinHeight(40);group.header.Padding({12,8,12,8});
                group.header.Margin({0,12,0,6});group.header.CornerRadius({8,8,8,8});
                Automation::AutomationProperties::SetName(group.header,std::wstring(section)+L" settings");
                group.body=Controls::StackPanel();
                size_t index=editor.sections.size();
                group.checked=group.header.Checked(winrt::auto_revoke,[&editor,index](auto const&,auto const&) {
                    UpdateEditorSection(editor.sections[index]);UpdateEditorSurface(editor);
                });
                group.unchecked=group.header.Unchecked(winrt::auto_revoke,[&editor,index](auto const&,auto const&) {
                    UpdateEditorSection(editor.sections[index]);UpdateEditorSurface(editor);
                });
                UpdateEditorSection(group);
                fields.Children().Append(group.header);fields.Children().Append(group.body);
                fields=group.body;disclosures[field.page]=group.header;
                editor.sections.push_back(std::move(group));
            } else {
                Controls::TextBlock title;
                title.Text(section);
                title.FontSize(13);
                title.FontWeight(winrt::Windows::UI::Text::FontWeights::SemiBold());
                title.TextWrapping(TextWrapping::Wrap);
                title.Margin({0,24,0,10});
                fields.Children().Append(title);
            }
            targets[field.page]=fields;
        }
        control.disclosure=disclosures[field.page];
        auto card = Markup::XamlReader::Load(LR"XAML(
<Border xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        CornerRadius="10" Padding="12,10" Margin="0,0,0,6"
        Background="{ThemeResource SystemControlBackgroundChromeMediumLowBrush}">
  <Grid x:Name="Row">
    <Grid.RowDefinitions><RowDefinition Height="Auto"/><RowDefinition Height="Auto"/><RowDefinition Height="Auto"/></Grid.RowDefinitions>
    <Grid.ColumnDefinitions><ColumnDefinition/><ColumnDefinition Width="Auto"/></Grid.ColumnDefinitions>
    <TextBlock x:Name="Label" FontSize="13" VerticalAlignment="Center" TextWrapping="Wrap" Margin="0,0,10,0"/>
  </Grid>
</Border>)XAML").as<Controls::Border>();
        control.card = card;
        control.row = card.FindName(L"Row").as<Controls::Grid>();
        auto label = card.FindName(L"Label").as<Controls::TextBlock>();
        label.Text(field.label);
        if (field.kind == SettingKind::Choice) {
            Controls::ComboBox choice;
            choice.Width(152);
            for (auto const& item : field.choices) choice.Items().Append(winrt::box_value(item.label));
            control.input = choice;
            control.selection = choice.SelectionChanged(winrt::auto_revoke,
                [&editor](auto const&, auto const&) { EditorChanged(editor); });
        } else if (field.kind == SettingKind::Toggle) {
            Controls::ToggleSwitch toggle;
            toggle.OnContent(winrt::box_value(L""));
            toggle.OffContent(winrt::box_value(L""));
            toggle.MinWidth(0);
            control.input = toggle;
            control.toggled = toggle.Toggled(winrt::auto_revoke,
                [&editor, i](auto const&, auto const&) {
                    EditorChanged(editor);
                    if (kSettingFields[i].page == 2 && !editor.syncing) RebuildTrayRows(editor);
                });
        } else if (field.kind == SettingKind::Color) {
            Controls::TextBox color;
            color.Width(105);
            color.MaxLength(7);
            color.PlaceholderText(L"#FFFFFF");
            control.input = color;
            control.textChanged = color.TextChanging(winrt::auto_revoke,
                [&editor](auto const&, auto const&) { EditorChanged(editor); });
        } else {
            Controls::TextBox number;
            number.Width(64);
            number.MaxLength(12);
            number.TextAlignment(TextAlignment::Right);
            control.input = number;
            control.slider = Controls::Slider();
            control.slider.Minimum(field.minimum / field.displayDivisor);
            control.slider.Maximum(field.maximum / field.displayDivisor);
            control.slider.StepFrequency(1 / field.displayDivisor);
            control.slider.Margin({0, 4, 0, 0});
            Controls::Grid::SetRow(control.slider, 1);
            Controls::Grid::SetColumnSpan(control.slider, 2);
            Automation::AutomationProperties::SetName(control.slider, field.label);
            control.row.Children().Append(control.slider);
            control.textChanged = number.TextChanging(winrt::auto_revoke,
                [&editor, i](auto const&, auto const&) {
                    if (editor.syncing) return;
                    auto const& c = editor.controls[i];
                    int value{};
                    editor.syncing = true;
                    if (EditorNumber(c.input.as<Controls::TextBox>(), kSettingFields[i], value))
                        c.slider.Value(value / kSettingFields[i].displayDivisor);
                    editor.syncing = false;
                    EditorChanged(editor);
                });
            control.valueChanged = control.slider.ValueChanged(winrt::auto_revoke,
                [&editor, i](auto const&, auto const&) {
                    if (editor.syncing) return;
                    auto const& c = editor.controls[i];
                    editor.syncing = true;
                    auto const& definition = kSettingFields[i];
                    c.input.as<Controls::TextBox>().Text(EditorNumberText(
                        static_cast<int>(std::lround(c.slider.Value() * definition.displayDivisor)), definition));
                    editor.syncing = false;
                    EditorChanged(editor);
                });
        }
        Controls::Grid::SetColumn(control.input, 1);
        control.input.VerticalAlignment(VerticalAlignment::Center);
        Automation::AutomationProperties::SetName(control.input, field.label);
        Automation::AutomationProperties::SetAutomationId(control.input, field.key);
        PCWSTR help = i == 42 ? L"Native follows Windows. Mica uses the wallpaper backdrop, Acrylic uses Windows' Acrylic brush, and Liquid glass refracts the desktop. Panels sample the finished body." :
            i == 3 || i == 4 ? L"Rounded corners are limited to half the visible shape's width or height." :
            i == 5 ? L"Space above and below each button background. Icons and click targets keep their native size." :
            i == 37 ? L"Blur strength for a Liquid glass body." :
            i == 20 || i == 43 ? L"Biconvex curves the glass. Snell traces light through the surface. Dome magnifies. Layered adds a curled rim; Bevel concentrates distortion at the edge." :
            i == 21 || i == 44 ? L"Circle and squircle are convex; concave bends outward; raised lip blends convex and concave curves." :
            i == 22 || i == 45 ? L"Width of the curved edge, independent of the glass base thickness." :
            i == 24 || i == 47 ? L"1.00 does not bend light; ordinary glass is approximately 1.50." :
            i == 29 || i == 54 ? L"Original and Rich specular use the highlight intensity. At 0%, specular highlights are disabled; refraction and Fresnel remain independent." :
            i == 58 ? L"Transparency of the floating panels. The main body remains opaque." :
            i == 59 ? L"Edge glow on floating panels and a Liquid glass body. 0% disables it." :
            i == 66 || i == 86 ? L"Width of the highlight, independent of refraction. 0 keeps automatic sizing." :
            i == 68 || i == 88 ? L"Sharp detail retained at the edge of frosted glass. 0 frosts the entire surface." :
            i == 69 || i == 89 ? L"With refraction links color fringing to the actual displacement of light." :
            i == 72 || i == 92 ? L"Glass follows pointer motion and settles after it stops. Controls stay in place. Windows animation settings apply." :
            i == 75 || i == 95 ? L"Optional glints along the glass edge. Windows animation settings apply." :
            field.page == 3 && i != 38 ? L"Applies to floating panels and to the body when Liquid glass is selected." :
            i == 33 ? L"Shadows follow the dock and are limited to the taskbar's window area." :
            i == 36 ? L"Average background brightness over this many seconds. Longer windows reduce color changes on animated wallpapers. 0 reacts to each sample." : nullptr;
        if (help) {
            Controls::ToolTipService::SetToolTip(control.row, winrt::box_value(help));
            Automation::AutomationProperties::SetHelpText(control.input, help);
            control.help=help;
        }
        control.row.Children().Append(control.input);
        if(field.kind==SettingKind::Number || field.kind==SettingKind::Color) {
            control.error=Controls::TextBlock();
            control.error.FontSize(12);control.error.TextWrapping(TextWrapping::Wrap);
            control.error.Margin({0,6,0,0});control.error.Visibility(Visibility::Collapsed);
            Automation::AutomationProperties::SetLiveSetting(control.error,Automation::Peers::AutomationLiveSetting::Polite);
            Controls::Grid::SetRow(control.error,2);Controls::Grid::SetColumnSpan(control.error,2);
            control.row.Children().Append(control.error);
        }
        if (std::wstring_view(field.key) == L"tray.enabled") {
            editor.trayEnabled = control.input.as<Controls::ToggleSwitch>();
            fields.Children().InsertAt(0, card);
        } else fields.Children().Append(card);
    }
    editor.navigationChanged = editor.navigation.SelectionChanged(winrt::auto_revoke,
        [&editor](auto const&, auto const&) {
            int page = editor.navigation.SelectedIndex();
            for (int i = 0; i < static_cast<int>(editor.pages.size()); ++i)
                editor.pages[i].Visibility(page == i ? Visibility::Visible : Visibility::Collapsed);
            editor.scroll.ChangeView(nullptr, 0.0, nullptr, true);
            if (page == 2) RequestTrayCatalog(editor);
            UpdateEditorSurface(editor);
        });
    editor.surfaceScrolled = editor.scroll.ViewChanged(winrt::auto_revoke,
        [&editor](auto const&, auto const&) { UpdateEditorSurface(editor, false); });
    auto command = [window = editor.window](int id) {
        SendMessageW(window, WM_COMMAND, id, 0);
    };
    editor.saveClick = editor.save.Click(winrt::auto_revoke,
        [command](auto const&, auto const&) { command(kEditorSave); });
    editor.reloadClick = editor.reload.Click(winrt::auto_revoke,
        [command](auto const&, auto const&) { command(kEditorReload); });
    editor.openFileClick = editor.openFile.Click(winrt::auto_revoke,
        [command](auto const&, auto const&) { command(kEditorOpenFile); });
    editor.refreshTrayClick = editor.refreshTray.Click(winrt::auto_revoke,
        [command](auto const&, auto const&) { command(kEditorRefreshTray); });
    editor.enableLiquidClick=editor.enableLiquid.Click(winrt::auto_revoke,
        [&editor](auto const&,auto const&) {
            editor.syncing=true;
            editor.controls[0].input.as<Controls::ComboBox>().SelectedIndex(3);
            editor.controls[9].input.as<Controls::ComboBox>().SelectedIndex(0);
            editor.syncing=false;EditorChanged(editor);
        });
    CreateEditorPreview(editor);
    ReloadEditor(editor);
    UpdateEditorTheme(editor);
}

LRESULT CALLBACK SettingsEditorProc(HWND window, UINT message, WPARAM wp, LPARAM lp) {
    auto* editor = reinterpret_cast<SettingsEditor*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    try {
        if (message == WM_NCCREATE) {
            editor = static_cast<SettingsEditor*>(reinterpret_cast<CREATESTRUCTW*>(lp)->lpCreateParams);
            editor->window = window;
            SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(editor));
        }
        if (!editor) return DefWindowProcW(window, message, wp, lp);
        if (message == WM_NCCALCSIZE && lp) {
            // Keep the sizing/maximize styles, but make the entire frame client
            // area. The XAML header supplies its own caption and window buttons.
            if (IsZoomed(window)) {
                MONITORINFO monitor{};
                monitor.cbSize = sizeof(monitor);
                if (GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &monitor))
                    *reinterpret_cast<RECT*>(lp) = monitor.rcWork;
            }
            return 0;
        }
        if (message == WM_NCHITTEST)
            return EditorFrameHitTest(*editor, {short(LOWORD(lp)), short(HIWORD(lp))});
        if (message == WM_NCMOUSEMOVE) {
            editor->framePointerOver = true;
            editor->captionHovered = EditorCaptionButton(int(wp)) ? int(wp) : 0;
            UpdateEditorCaption(*editor);
            return 0; // InputSite pointer exit, not native mouse tracking, ends this hover.
        }
        if (message == WM_NCMOUSELEAVE) {
            editor->framePointerOver = false;
            editor->captionHovered = 0;
            UpdateEditorCaption(*editor);
            return 0;
        }
        if (message == WM_ACTIVATE && LOWORD(wp) == WA_INACTIVE) {
            editor->captionClickTime = 0;
            if (editor->framePointerOver) SendMessageW(window, WM_NCMOUSELEAVE, 0, 0);
        }
        if (message == WM_NCLBUTTONDOWN && EditorCaptionButton(int(wp))) {
            editor->captionPressed = int(wp);
            editor->captionHovered = int(wp);
            SetCapture(window);
            UpdateEditorCaption(*editor);
            return 0;
        }
        if ((message == WM_LBUTTONUP || message == WM_NCLBUTTONUP) && editor->captionPressed) {
            POINT point{short(LOWORD(lp)), short(HIWORD(lp))};
            if (message == WM_LBUTTONUP) ClientToScreen(window, &point);
            int pressed = std::exchange(editor->captionPressed, 0);
            bool activate = EditorFrameHitTest(*editor, point) == pressed;
            ReleaseCapture();
            UpdateEditorCaption(*editor);
            if (activate) {
                UINT command = pressed == HTCLOSE ? SC_CLOSE : pressed == HTMINBUTTON ? SC_MINIMIZE :
                    IsZoomed(window) ? SC_RESTORE : SC_MAXIMIZE;
                PostMessageW(window, WM_SYSCOMMAND, command, 0);
            }
            return 0;
        }
        if (message == WM_MOUSEMOVE && editor->captionPressed) {
            POINT point{short(LOWORD(lp)), short(HIWORD(lp))};
            ClientToScreen(window, &point);
            int hit = EditorFrameHitTest(*editor, point);
            editor->captionHovered = EditorCaptionButton(hit) ? hit : 0;
            UpdateEditorCaption(*editor);
            return 0;
        }
        if (message == WM_CAPTURECHANGED) {
            editor->captionPressed = 0;
            UpdateEditorCaption(*editor);
        }
        if (message == WM_SETFOCUS && editor->island && editor->child) {
            // Re-activation can focus the Win32 host while XAML still remembers
            // a focused TextBox. Return native focus to the island's input site.
            FocusEditor(*editor, Hosting::XamlSourceFocusNavigationReason::Restore);
            if (editor->iconPicker.open)
                editor->iconPicker.search.Focus(FocusState::Keyboard);
            return 0;
        }
        if (message == WM_TIMER && wp == kCatalogSearchTimerId) {
            KillTimer(window, kCatalogSearchTimerId);
            if (editor->iconPicker.open) FilterCatalogIcons(*editor);
            return 0;
        }
        if (message == WM_TIMER && wp == kEditorBackdropTimerId) {
            UpdateEditorSurface(*editor, false);
            UpdatePreviewForeground(*editor);
            return 0;
        }
        if (message == kCatalogIconsMessage) { CatalogIconResult(*editor, wp); return 0; }
        if (message == WM_CREATE) { editor->dpi = GetDpiForWindow(window); return 0; }
        if (message == WM_SIZE) {
            LayoutEditor(*editor);
            if (wp == SIZE_MINIMIZED) CloseEditorPreview(*editor);
            else if (!editor->preview.renderer) UpdateEditorPreview(*editor);
            return 0;
        }
        if (message == WM_DPICHANGED) {
            editor->dpi = HIWORD(wp);
            auto rect = reinterpret_cast<RECT*>(lp);
            SetWindowPos(window, nullptr, rect->left, rect->top, rect->right - rect->left,
                         rect->bottom - rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
            LayoutEditor(*editor);
            return 0;
        }
        if (message == WM_GETMINMAXINFO) {
            auto info = reinterpret_cast<MINMAXINFO*>(lp);
            info->ptMinTrackSize = {EditorScale(*editor, 1000), EditorScale(*editor, 620)};
            MONITORINFO monitor{};
            monitor.cbSize = sizeof(monitor);
            if (GetMonitorInfoW(MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &monitor)) {
                LONG width = monitor.rcWork.right - monitor.rcWork.left;
                LONG height = monitor.rcWork.bottom - monitor.rcWork.top;
                info->ptMinTrackSize.x = std::min(info->ptMinTrackSize.x, width);
                info->ptMinTrackSize.y = std::min(info->ptMinTrackSize.y, height);
                info->ptMaxPosition = {monitor.rcWork.left - monitor.rcMonitor.left,
                                       monitor.rcWork.top - monitor.rcMonitor.top};
                info->ptMaxSize = {width, height};
            }
            return 0;
        }
        if ((message == WM_SETTINGCHANGE || message == WM_THEMECHANGED ||
             message == kEditorThemeMessage) && editor->content && !editor->updatingSurface) {
            editor->surfaceAppearance = -1;
            UpdateEditorTheme(*editor);
        }
        if (message == WM_MOVE && editor->content) UpdateEditorPosition(*editor);
        if (message == WM_SHOWWINDOW && editor->content) {
            if (!wp) {
                CloseEditorPreview(*editor);
                CloseEditorSurface(*editor);
            }
            else PostMessageW(window, kEditorThemeMessage, 0, 0);
        }
        if (message == WM_ENTERSIZEMOVE) editor->captionClickTime = 0;
        if (message == WM_EXITSIZEMOVE) { RequestPreviewMetrics(*editor); return 0; }
        if (message == kPreviewMetricsMessage) {
            TaskbarPreviewMetrics metrics;
            { std::lock_guard lock(g_previewMetricsMutex); metrics = g_previewMetrics; }
            ApplyPreviewMetrics(*editor, metrics);
            return 0;
        }
        if (message == kTrayCatalogMessage) {
            bool available{};
            {
                std::lock_guard lock(g_trayCatalogMutex);
                editor->catalog = g_trayCatalog;
                available = g_trayCatalogAvailable;
            }
            editor->trayStatus.Text(!available ? L"Tray list unavailable in this host. Open settings from the taskbar."
                : editor->catalog.empty() ? L"No app tray icons are currently registered."
                : (std::to_wstring(editor->catalog.size()) + L" app tray icons. Changes apply when you save."));
            RebuildTrayRows(*editor);
            return 0;
        }
        if (message == WM_COMMAND) {
            int id = LOWORD(wp);
            if (id == IDCANCEL) { DestroyWindow(window); return 0; }
            if (id == kEditorSave) { SaveEditor(*editor); return 0; }
            if (id == kEditorReload) { ReloadEditor(*editor); RequestPreviewMetrics(*editor); return 0; }
            if (id == kEditorRefreshPreview) { RequestPreviewMetrics(*editor); return 0; }
            if (id == kEditorRefreshTray) { RequestTrayCatalog(*editor); return 0; }
            if (id == kEditorOpenFile) {
                if (!OpenSettingsFile()) EditorStatus(*editor, L"Could not open the TOML file.");
                return 0;
            }
        }
        if (message == kSettingsAppliedMessage) {
            if (!editor->dirty)
                EditorStatus(*editor, wp ? L"Saved, but could not apply. Reload the mod to retry."
                                         : L"Saved and applied");
            RequestPreviewMetrics(*editor);
            return 0;
        }
        if (message == kActivateSettingsMessage) {
            ShowWindow(window, SW_RESTORE);
            SetForegroundWindow(window);
            return 0;
        }
        if (message == WM_CLOSE) {
            CloseCatalogPicker(*editor);
            if (editor->filePicker) editor->filePicker->Close(HRESULT_FROM_WIN32(ERROR_CANCELLED));
            DestroyWindow(window);
            return 0;
        }
        if (message == WM_DESTROY) {
            editor->colorsChanged.revoke();
            editor->textScaleChanged.revoke();
            CloseEditorPreview(*editor);
            CloseEditorSurface(*editor);
            g_editorWindow = nullptr;
            PostQuitMessage(0);
            return 0;
        }
    } catch (...) {
        Wh_Log(L"[UWSET] editor operation failed hr=%08X", winrt::to_hresult().value);
        if (editor) EditorStatus(*editor, L"Could not complete this operation.");
        if (message == WM_CREATE) return -1;
        return 0;
    }
    return DefWindowProcW(window, message, wp, lp);
}

DWORD WINAPI SettingsEditorThread(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    HRESULT apartment = RoInitialize(RO_INIT_SINGLETHREADED);
    winrt::Windows::System::DispatcherQueueController queue{nullptr};
    Hosting::WindowsXamlManager manager{nullptr};
    HINSTANCE instance = GetModuleHandleW(nullptr);
    WNDCLASSW windowClass{};
    windowClass.hInstance = instance;
    windowClass.lpfnWndProc = SettingsEditorProc;
    windowClass.lpszClassName = kEditorClass;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    ATOM registered{};
    try {
        winrt::check_hresult(apartment);
        winrt::check_hresult(CreateDispatcherQueueController(
            {sizeof(DispatcherQueueOptions), DQTYPE_THREAD_CURRENT, DQTAT_COM_NONE},
            reinterpret_cast<ABI::Windows::System::IDispatcherQueueController**>(winrt::put_abi(queue))));
        manager = Hosting::WindowsXamlManager::InitializeForCurrentThread();
        if (!(registered = RegisterClassW(&windowClass))) winrt::throw_last_error();
        SettingsEditor editor;
        try {
            POINT cursor{};
            GetCursorPos(&cursor);
            MONITORINFO monitor{};
            monitor.cbSize = sizeof(monitor);
            if (!GetMonitorInfoW(MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST), &monitor))
                winrt::throw_last_error();
            HWND window = CreateWindowExW(WS_EX_APPWINDOW | WS_EX_NOREDIRECTIONBITMAP, kEditorClass,
                L"Ultrawide taskbar settings", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                monitor.rcWork.left, monitor.rcWork.top, 960, 760, nullptr, nullptr, instance, &editor);
            if (!window) winrt::throw_last_error();
            editor.island = Hosting::DesktopWindowXamlSource();
            auto native = editor.island.as<IDesktopWindowXamlSourceNative2>();
            winrt::check_hresult(native->AttachToWindow(window));
            winrt::check_hresult(native->get_WindowHandle(&editor.child));
            if (!SetPropW(editor.child, kEditorFrameProperty, &editor)) winrt::throw_last_error();
            editor.childProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(
                editor.child, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(EditorIslandFrameProc)));
            if (!editor.childProc) {
                RemovePropW(editor.child, kEditorFrameProperty);
                winrt::throw_last_error();
            }
            CreateEditorContent(editor);
            editor.root = Controls::Grid();
            editor.root.Children().Append(editor.content);
            editor.island.Content(editor.root);
            UpdateEditorTheme(editor);
            editor.takeFocus = editor.island.TakeFocusRequested(winrt::auto_revoke,
                [&editor](auto const&, Hosting::DesktopWindowXamlSourceTakeFocusRequestedEventArgs const& args) {
                    auto reason = args.Request().Reason();
                    if (reason == Hosting::XamlSourceFocusNavigationReason::First ||
                        reason == Hosting::XamlSourceFocusNavigationReason::Last)
                        FocusEditor(editor, reason);
                });
            int width = std::min(EditorScale(editor, 1280), static_cast<int>(monitor.rcWork.right - monitor.rcWork.left));
            int height = std::min(EditorScale(editor, 840), static_cast<int>(monitor.rcWork.bottom - monitor.rcWork.top));
            int x = monitor.rcWork.left + (monitor.rcWork.right - monitor.rcWork.left - width) / 2;
            int y = monitor.rcWork.top + (monitor.rcWork.bottom - monitor.rcWork.top - height) / 2;
            SetWindowPos(window, nullptr, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
            DWM_WINDOW_CORNER_PREFERENCE corners = DWMWCP_DONOTROUND;
            DwmSetWindowAttribute(window, DWMWA_WINDOW_CORNER_PREFERENCE, &corners, sizeof(corners));
            COLORREF border = DWMWA_COLOR_NONE;
            DwmSetWindowAttribute(window, DWMWA_BORDER_COLOR, &border, sizeof(border));
            LayoutEditor(editor);
            ShowWindow(editor.child, SW_SHOW);
            g_editorWindow = window;
            auto refreshAppearance=[window](auto const&, auto const&) {
                ActiveScope active;
                if (!g_stopping && g_editorWindow.load() == window)
                    PostMessageW(window, kEditorThemeMessage, 0, 0);
            };
            editor.colorsChanged=editor.systemColors.ColorValuesChanged(winrt::auto_revoke,refreshAppearance);
            editor.textScaleChanged=editor.systemColors.TextScaleFactorChanged(winrt::auto_revoke,refreshAppearance);
            RequestPreviewMetrics(editor);
            RequestTrayCatalog(editor);
            if (g_stopping || g_editorCancel) {
                DestroyWindow(window);
            } else {
                ShowWindow(window, SW_SHOW);
                SetForegroundWindow(window);
                FocusEditor(editor, Hosting::XamlSourceFocusNavigationReason::First);
                editor.navigation.Focus(FocusState::Programmatic);
            }
            MSG message{};
            while (GetMessageW(&message, nullptr, 0, 0) > 0) {
                if (RouteEditorFrameInput(editor, message)) continue;
                if (message.message == WM_KEYDOWN && message.wParam == 'S' && GetKeyState(VK_CONTROL) < 0) {
                    if (!editor.iconPicker.open) SendMessageW(window, WM_COMMAND, kEditorSave, 0);
                    continue;
                }
                BOOL handled{};
                winrt::check_hresult(native->PreTranslateMessage(&message, &handled));
                if (handled) continue;
                if (message.message == WM_KEYDOWN && message.wParam == VK_ESCAPE) {
                    if (editor.iconPicker.open) { CloseCatalogPicker(editor); continue; }
                    PostMessageW(window, WM_CLOSE, 0, 0);
                    continue;
                }
                if (message.message == WM_KEYDOWN && message.wParam == VK_TAB && !editor.island.HasFocus()) {
                    FocusEditor(editor,
                        GetKeyState(VK_SHIFT) < 0 ? Hosting::XamlSourceFocusNavigationReason::Last :
                                                   Hosting::XamlSourceFocusNavigationReason::First);
                    continue;
                }
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        } catch (winrt::hresult_error const& error) {
            Wh_Log(L"[UWSET] XAML editor failed hr=%08X %s", error.code().value, error.message().c_str());
        } catch (...) {
            Wh_Log(L"[UWSET] XAML editor failed hr=%08X", winrt::to_hresult().value);
        }
        editor.takeFocus.revoke();
        editor.colorsChanged.revoke();
        editor.textScaleChanged.revoke();
        editor.navigationChanged.revoke();
        editor.surfaceScrolled.revoke();
        editor.saveClick.revoke();
        editor.reloadClick.revoke();
        editor.openFileClick.revoke();
        for (auto& click : editor.captionClicks) click.revoke();
        for (size_t i = 0; i < editor.captionButtons.size(); ++i)
            if (editor.captionPressedTokens[i])
                editor.captionButtons[i].UnregisterPropertyChangedCallback(
                    Controls::Primitives::ButtonBase::IsPressedProperty(), *editor.captionPressedTokens[i]);
        editor.refreshTrayClick.revoke();
        editor.enableLiquidClick.revoke();
        editor.sections.clear();
        editor.trayRows.clear();
        CloseCatalogPicker(editor);
        editor.preview.refreshClick.revoke();
        CloseEditorPreview(editor);
        CloseEditorSurface(editor);
        editor.controls = {};
        if (editor.childProc && IsWindow(editor.child) &&
            reinterpret_cast<WNDPROC>(GetWindowLongPtrW(editor.child, GWLP_WNDPROC)) == EditorIslandFrameProc) {
            SetWindowLongPtrW(editor.child, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(editor.childProc));
            RemovePropW(editor.child, kEditorFrameProperty);
            editor.childProc = nullptr;
        }
        if (editor.island) { editor.island.Content(nullptr); editor.island.Close(); }
        if (IsWindow(editor.window)) DestroyWindow(editor.window);
        // All UI callbacks and references leave scope before XAML/queue shutdown.
    } catch (...) {
        Wh_Log(L"[UWSET] could not start XAML editor hr=%08X", winrt::to_hresult().value);
    }
    g_editorWindow = nullptr;
    if (registered) UnregisterClassW(kEditorClass, instance);
    if (manager) manager.Close();
    if (queue) {
        auto shutdown = queue.ShutdownQueueAsync();
        while (shutdown.Status() == winrt::Windows::Foundation::AsyncStatus::Started) {
            MSG message{};
            while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
            MsgWaitForMultipleObjectsEx(0, nullptr, 10, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        }
    }
    manager = nullptr;
    queue = nullptr;
    if (SUCCEEDED(apartment)) RoUninitialize();
    return 0;
}

void OpenLocalSettings() {
    if (g_stopping) return;
    if (!EnsureSettingsFile(g_settingsPath, SettingsDocument(nullptr))) {
        MessageBoxW(nullptr, L"Could not create the local settings file.",
                    L"Ultrawide taskbar settings", MB_OK | MB_ICONERROR);
        return;
    }
    std::lock_guard lock(g_editorMutex);
    if (auto window = g_editorWindow.load()) {
        PostMessageW(window, kActivateSettingsMessage, 0, 0);
        return;
    }
    if (g_editorThread) {
        if (WaitForSingleObject(g_editorThread, 0) != WAIT_OBJECT_0) return;
        CloseHandle(g_editorThread);
        g_editorThread = nullptr;
    }
    if (g_stopping) return;
    g_editorCancel = false;
    g_editorThread = CreateThread(nullptr, 0, SettingsEditorThread, nullptr, 0, nullptr);
    if (!g_editorThread) Wh_Log(L"[UWSET] could not start settings window");
}

void CloseSettingsEditor() {
    HANDLE thread{};
    {
        std::lock_guard lock(g_editorMutex);
        g_editorCancel = true;
        thread = g_editorThread;
        if (auto window = g_editorWindow.load()) PostMessageW(window, WM_CLOSE, 0, 0);
    }
    if (thread) {
        WaitForSingleObject(thread, INFINITE);
        std::lock_guard lock(g_editorMutex);
        CloseHandle(thread);
        g_editorThread = nullptr;
    }
}

struct SettingsMenuEntry {
    winrt::weak_ref<Controls::MenuFlyout> menu;
    winrt::weak_ref<Controls::MenuFlyoutItem> item;
    winrt::weak_ref<Controls::MenuFlyoutSeparator> separator;
    winrt::event_token click{};
};
std::vector<SettingsMenuEntry> g_settingsMenus;
constexpr PCWSTR kSettingsMenuName = L"WindhawkUltrawideLocalSettings";

void RemoveSettingsMenuEntry(SettingsMenuEntry const& entry) {
    auto menu = entry.menu.get();
    auto item = entry.item.get();
    auto separator = entry.separator.get();
    if (item) item.Click(entry.click);
    if (!menu) return;
    auto items = menu.Items();
    uint32_t index{};
    if (item && items.IndexOf(item, index)) items.RemoveAt(index);
    if (separator && items.IndexOf(separator, index)) items.RemoveAt(index);
}

void RestoreSettingsMenus() {
    for (auto const& entry : g_settingsMenus) {
        try { RemoveSettingsMenuEntry(entry); } catch (...) {}
    }
    g_settingsMenus.clear();
}

void AddSettingsMenuEntry(Controls::MenuFlyout const& menu, FrameworkElement const& target) {
    if (!menu || !target || target.Name() != L"SeparateQuickSettingsXamlControlCenter") return;
    auto items = menu.Items();
    for (auto const& item : items)
        if (item.Name() == kSettingsMenuName) return;
    std::erase_if(g_settingsMenus, [](auto const& entry) {
        if (entry.menu.get()) return false;
        RemoveSettingsMenuEntry(entry);
        return true;
    });
    Controls::MenuFlyoutItem item;
    item.Name(kSettingsMenuName);
    item.Text(L"Ultrawide taskbar settings...");
    Controls::FontIcon icon;
    icon.FontFamily(Media::FontFamily(L"Segoe Fluent Icons"));
    icon.Glyph(L"\xE713");
    icon.FontSize(16);
    icon.Width(16);
    icon.Height(16);
    item.Icon(icon);
    auto token = item.Click([](auto const&, auto const&) {
        ActiveScope active;
        if (!g_stopping) OpenLocalSettings();
    });
    Controls::MenuFlyoutSeparator separator;
    SettingsMenuEntry entry{winrt::make_weak(menu), winrt::make_weak(item),
                            winrt::make_weak(separator), token};
    g_settingsMenus.push_back(entry);
    try {
        items.Append(separator);
        items.Append(item);
    } catch (...) {
        RemoveSettingsMenuEntry(entry);
        g_settingsMenus.pop_back();
        throw;
    }
}

using FlyoutShowAt_t = HRESULT(WINAPI*)(void*, void*);
using FlyoutShowAtOptions_t = HRESULT(WINAPI*)(void*, void*, void*);
FlyoutShowAt_t FlyoutShowAt_Original{};
FlyoutShowAtOptions_t FlyoutShowAtOptions_Original{};
bool g_settingsMenuHooksAttempted{};

void PrepareSettingsMenu(void* self, void* target) {
    if (g_stopping || GetCurrentThreadId() != g_ownerThread) return;
    try {
        winrt::Windows::Foundation::IInspectable object{nullptr}, owner{nullptr};
        winrt::copy_from_abi(object, self);
        winrt::copy_from_abi(owner, target);
        AddSettingsMenuEntry(object.try_as<Controls::MenuFlyout>(), owner.try_as<FrameworkElement>());
    } catch (...) {
        Wh_Log(L"[UWSET] context-menu entry unavailable");
    }
}

HRESULT WINAPI FlyoutShowAt_Hook(void* self, void* target) {
    ActiveScope active;
    PrepareSettingsMenu(self, target);
    return FlyoutShowAt_Original(self, target);
}

HRESULT WINAPI FlyoutShowAtOptions_Hook(void* self, void* target, void* options) {
    ActiveScope active;
    PrepareSettingsMenu(self, target);
    return FlyoutShowAtOptions_Original(self, target, options);
}

void InstallSettingsMenuHooks() {
    if (g_settingsMenuHooksAttempted) return;
    g_settingsMenuHooksAttempted = true;
    try {
        Controls::MenuFlyout menu;
        auto base = menu.as<Controls::Primitives::IFlyoutBase>();
        auto options = menu.as<Controls::Primitives::IFlyoutBase5>();
        // Published WinRT ABI: six IInspectable entries, then ShowAt at
        // IFlyoutBase method 8 / IFlyoutBase5 method 6.
        auto baseTable = *reinterpret_cast<void***>(winrt::get_abi(base));
        auto optionsTable = *reinterpret_cast<void***>(winrt::get_abi(options));
        bool first = WindhawkUtils::SetFunctionHook(
            reinterpret_cast<FlyoutShowAt_t>(baseTable[14]), FlyoutShowAt_Hook, &FlyoutShowAt_Original);
        bool second = WindhawkUtils::SetFunctionHook(
            reinterpret_cast<FlyoutShowAtOptions_t>(optionsTable[12]), FlyoutShowAtOptions_Hook, &FlyoutShowAtOptions_Original);
        if (first || second) Wh_ApplyHookOperations();
        Wh_Log(L"[UWSET] menu hooks: ShowAt=%d ShowAtOptions=%d", first, second);
    } catch (...) {
        Wh_Log(L"[UWSET] menu hooks unavailable");
    }
}

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

bool IsModModuleName(std::wstring_view name, std::wstring_view id) {
    if (auto separator = name.find_last_of(L"\\/");
        separator != std::wstring_view::npos)
        name.remove_prefix(separator + 1);
    if (name.starts_with(L"local@")) name.remove_prefix(6);
    return name.starts_with(id) && name.substr(id.size()).starts_with(L'_') &&
           name.ends_with(L".dll");
}

bool ModLoaded(std::span<const std::wstring_view> ids) {
    HANDLE snapshot = CreateToolhelp32Snapshot(
        TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
    if (snapshot == INVALID_HANDLE_VALUE) return false;
    MODULEENTRY32W module{};
    module.dwSize = sizeof(module);
    bool found = Module32FirstW(snapshot, &module);
    while (found && std::none_of(ids.begin(), ids.end(), [&](auto id) {
               return IsModModuleName(module.szModule, id);
           }))
        found = Module32NextW(snapshot, &module);
    CloseHandle(snapshot);
    return found;
}

bool TaskbarIconSizeLoaded() {
    constexpr std::wstring_view id = L"taskbar-icon-size";
    return ModLoaded({&id, 1});
}

DockInsets GetDockInsets(HWND taskbar, double height) {
    return PlanDockInsets(TaskbarAtTop(taskbar), height, g_dockEdgeDistance);
}

DockInsets GetButtonInsets(DockInsets insets, double height,
                           double buttonPadding=g_buttonVerticalPadding) {
    double padding = std::clamp(
        buttonPadding, 0.0,
        std::max(0.0, (height - insets.top - insets.bottom - 1) / 2));
    return {insets.top + padding, insets.bottom + padding};
}

float ButtonFillOpacity(bool glass, bool highContrast=false) {
    return glass && !highContrast ? 0 : 1;
}

constexpr winrt::Windows::UI::Color kButtonAttentionTint{255,232,72,68};

bool ButtonGlassEnabled(DockTheme theme, bool layer) {
    return theme==DockTheme::Liquid && layer;
}

bool ButtonLensFits(double width,double height) { return width>2 && height>2; }

double ButtonClipRadius(double width,double height,double radius) {
    return std::min({radius,width/2,height/2});
}

std::array<winrt::Windows::UI::Color,2> ButtonRimColors(bool dark) {
    using Color=winrt::Windows::UI::Color;
    return {dark?Color{0x1A,255,255,255}:Color{0x05,0,0,0},
            dark?Color{0x0F,255,255,255}:Color{0x0F,0,0,0}};
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
    float model{}, surface{};
    float bezelWidth{12}, thickness{20}, refractiveIndex{1.5f};
    float magnification{}, distortion{}, saturation{}, tint{};
    float lighting{}, lightAngle{-1.04719755f}, rimOpacity{0.4f}, rimSaturation{4};
    float shadowOpacity{}, shadowBlur{12}, shadowOffset{4};
    float rimRefraction{0.5f}, rimWidth{3}, bevelSharpness{10}, cornerBoost{};
    float rippleStrength{}, rippleWavelength{12}, edgeWidth{}, edgeSoftness{1};
    float frostEdge{0.15f}, coupledChroma{}, dyeRed{1}, dyeGreen{1};
    float dyeBlue{1}, dyeStrength{}, fluidStrength{}, fluidRadius{0.35f};
    float fluidViscosity{0.65f}, glintMode{}, glintStrength{0.15f}, glintSpeed{0.5f};
    float glintSize{0.35f}, boundaryFade{2}, extraPadding0{}, extraPadding1{};
} g_liquidSettings;
static_assert(sizeof(LiquidSettings) == 49 * sizeof(float));

void ScaleLiquidSettings(LiquidSettings& value, float scale) {
    value.zRadius *= scale; value.bezelWidth *= scale; value.thickness *= scale;
    value.rimWidth *= scale; value.rippleWavelength *= scale;
    value.edgeWidth *= scale; value.boundaryFade *= scale;
}

LiquidSettings ApplyLiquidPreset(std::wstring_view preset,
                                 LiquidSettings custom) {
    if (preset != L"regular" && preset != L"frosted" && preset != L"dark")
        return custom;
    // The original optical defaults match ybouane/liquidglass; added effects
    // default off. Frosted and Dark retain the reference site's only
    // optical overrides: {blurAmount: 0.25} and {brightness: -0.3, blurAmount: 0.25}.
    LiquidSettings result;
    if (preset != L"regular") result.blur = 0.25f;
    if (preset == L"dark") result.brightness = -0.3f;
    return result;
}

LiquidSettings ReadLiquidSettings(settingsjson::JsonObject const& settings) {
    auto percent = [&settings](PCWSTR key, int low, int high) {
        return std::clamp(LocalInt(settings, key), low, high) / 100.0f;
    };
    LiquidSettings result;
    auto choice = [&](PCWSTR key) {
        auto value = LocalString(settings, key);
        auto choices = SettingDefinition(key).choices;
        for (size_t i = 0; i < choices.size(); ++i)
            if (value == choices[i].value) return static_cast<float>(i);
        return 0.0f;
    };
    result.zRadius = static_cast<float>(std::clamp(LocalInt(settings, L"liquid.zRadius"), 1, 100));
    result.refraction = percent(L"liquid.refraction", 0, 200);
    result.chroma = percent(L"liquid.chromaticAberration", 0, 30);
    result.edgeHighlight = percent(L"liquid.edgeHighlight", 0, 100);
    result.specular = percent(L"liquid.specular", 0, 100);
    result.fresnel = percent(L"liquid.fresnel", 0, 200);
    result.opacity = percent(L"liquid.opacity", 0, 100);
    result.brightness = percent(L"liquid.brightness", -50, 50);
    result.blur = percent(L"liquid.blur", 0, 100);
    result.model = choice(L"liquid.model");
    result.surface = choice(L"liquid.surface");
    result.bezelWidth = static_cast<float>(std::clamp(LocalInt(settings, L"liquid.bezelWidth"), 1, 100));
    result.thickness = static_cast<float>(std::clamp(LocalInt(settings, L"liquid.thickness"), 1, 200));
    result.refractiveIndex = percent(L"liquid.refractiveIndex", 100, 250);
    result.magnification = percent(L"liquid.magnification", 0, 200);
    result.distortion = percent(L"liquid.distortion", 0, 100);
    result.saturation = percent(L"liquid.saturation", -100, 100);
    result.tint = percent(L"liquid.tint", 0, 100);
    result.lighting = choice(L"liquid.lighting");
    result.lightAngle = std::clamp(LocalInt(settings, L"liquid.lightAngle"), -180, 180) *
                        (3.14159265f / 180);
    result.rimOpacity = percent(L"liquid.rimOpacity", 0, 100);
    result.rimSaturation = percent(L"liquid.rimSaturation", 0, 1000);
    result.shadowOpacity = percent(L"liquid.shadowOpacity", 0, 100);
    result.shadowBlur = static_cast<float>(std::clamp(LocalInt(settings, L"liquid.shadowBlur"), 0, 72));
    result.shadowOffset = static_cast<float>(std::clamp(LocalInt(settings, L"liquid.shadowOffset"), -40, 40));
    result.rimRefraction = percent(L"liquid.rimRefraction", 0, 200);
    result.rimWidth = float(std::clamp(LocalInt(settings, L"liquid.rimWidth"), 1, 60));
    result.bevelSharpness = float(std::clamp(LocalInt(settings, L"liquid.bevelSharpness"), 2, 32));
    result.cornerBoost = percent(L"liquid.cornerBoost", 0, 100);
    result.rippleStrength = percent(L"liquid.rippleStrength", 0, 100);
    result.rippleWavelength = float(std::clamp(LocalInt(settings, L"liquid.rippleWavelength"), 2, 100));
    result.edgeWidth = float(std::clamp(LocalInt(settings, L"liquid.edgeWidth"), 0, 100));
    result.edgeSoftness = percent(L"liquid.edgeSoftness", 10, 400);
    result.frostEdge = percent(L"liquid.frostEdge", 0, 100);
    result.coupledChroma = choice(L"liquid.dispersion");
    auto dye = LocalString(settings, L"liquid.dye");
    unsigned rgb = wcstoul(dye.c_str() + 1, nullptr, 16);
    result.dyeRed = ((rgb >> 16) & 255) / 255.0f;
    result.dyeGreen = ((rgb >> 8) & 255) / 255.0f;
    result.dyeBlue = (rgb & 255) / 255.0f;
    result.dyeStrength = percent(L"liquid.dyeStrength", 0, 100);
    result.fluidStrength = percent(L"liquid.fluidStrength", 0, 100);
    result.fluidRadius = percent(L"liquid.fluidRadius", 5, 100);
    result.fluidViscosity = percent(L"liquid.fluidViscosity", 0, 100);
    result.glintMode = choice(L"liquid.glintMode");
    result.glintStrength = percent(L"liquid.glintStrength", 0, 100);
    result.glintSpeed = percent(L"liquid.glintSpeed", 1, 100);
    result.glintSize = percent(L"liquid.glintSize", 5, 100);
    result.boundaryFade = float(std::clamp(LocalInt(settings, L"liquid.boundaryFade"), 0, 20));
    return ApplyLiquidPreset(LocalString(settings, L"liquid.preset"), result);
}

LiquidSettings ReadUiLiquidSettings(settingsjson::JsonObject const& ui, float scale) {
    // Reuse the optical decoder with only UI values, never taskbar settings.
    constexpr std::pair<PCWSTR, PCWSTR> bindings[]{
        {L"ui.panelDepth", L"liquid.zRadius"},
        {L"ui.panelRefraction", L"liquid.refraction"},
        {L"ui.panelBlur", L"liquid.blur"},
        {L"ui.panelHighlights", L"liquid.specular"},
        {L"ui.glassEdgeHighlight", L"liquid.edgeHighlight"},
        {L"ui.glassModel", L"liquid.model"},
        {L"ui.glassSurface", L"liquid.surface"},
        {L"ui.glassBezelWidth", L"liquid.bezelWidth"},
        {L"ui.glassThickness", L"liquid.thickness"},
        {L"ui.glassRefractiveIndex", L"liquid.refractiveIndex"},
        {L"ui.glassMagnification", L"liquid.magnification"},
        {L"ui.glassDistortion", L"liquid.distortion"},
        {L"ui.glassChromaticAberration", L"liquid.chromaticAberration"},
        {L"ui.glassBrightness", L"liquid.brightness"},
        {L"ui.glassSaturation", L"liquid.saturation"},
        {L"ui.glassFresnel", L"liquid.fresnel"},
        {L"ui.glassLighting", L"liquid.lighting"},
        {L"ui.glassLightAngle", L"liquid.lightAngle"},
        {L"ui.glassRimOpacity", L"liquid.rimOpacity"},
        {L"ui.glassRimSaturation", L"liquid.rimSaturation"},
        {L"ui.glassRimRefraction", L"liquid.rimRefraction"},
        {L"ui.glassRimWidth", L"liquid.rimWidth"},
        {L"ui.glassBevelSharpness", L"liquid.bevelSharpness"},
        {L"ui.glassCornerBoost", L"liquid.cornerBoost"},
        {L"ui.glassRippleStrength", L"liquid.rippleStrength"},
        {L"ui.glassRippleWavelength", L"liquid.rippleWavelength"},
        {L"ui.glassEdgeWidth", L"liquid.edgeWidth"},
        {L"ui.glassEdgeSoftness", L"liquid.edgeSoftness"},
        {L"ui.glassFrostEdge", L"liquid.frostEdge"},
        {L"ui.glassDispersion", L"liquid.dispersion"},
        {L"ui.glassDye", L"liquid.dye"},
        {L"ui.glassDyeStrength", L"liquid.dyeStrength"},
        {L"ui.glassFluidStrength", L"liquid.fluidStrength"},
        {L"ui.glassFluidRadius", L"liquid.fluidRadius"},
        {L"ui.glassFluidViscosity", L"liquid.fluidViscosity"},
        {L"ui.glassGlintMode", L"liquid.glintMode"},
        {L"ui.glassGlintStrength", L"liquid.glintStrength"},
        {L"ui.glassGlintSpeed", L"liquid.glintSpeed"},
        {L"ui.glassGlintSize", L"liquid.glintSize"},
        {L"ui.glassBoundaryFade", L"liquid.boundaryFade"},
    };
    settingsjson::JsonObject optics;
    for (auto [uiKey, opticalKey] : bindings) {
        auto const& field = SettingDefinition(uiKey);
        optics.Insert(opticalKey, field.kind == SettingKind::Choice || field.kind == SettingKind::Color ?
            settingsjson::JsonValue::CreateStringValue(LocalString(ui, uiKey)) :
            settingsjson::JsonValue::CreateNumberValue(LocalInt(ui, uiKey) / field.displayDivisor));
    }
    auto result = ReadLiquidSettings(optics);
    ScaleLiquidSettings(result, scale);
    return result;
}

// A shadow-only sibling avoids clipping the shadow to the glass itself. The
// mask shares the compositor's animated geometry, including between GPU frames.
struct GlassShadow {
    Controls::Panel parent{nullptr};
    Controls::Canvas host{nullptr};
    ShapeVisual mask{nullptr};
    CompositionVisualSurface surface{nullptr};
    SpriteVisual visual{nullptr};
    DropShadow shadow{nullptr};

    GlassShadow(Controls::Panel const& panel, FrameworkElement const& before,
                CompositionRoundedRectangleGeometry const& geometry) {
        auto compositor = geometry.Compositor();
        host = Controls::Canvas();
        host.IsHitTestVisible(false);
        host.HorizontalAlignment(HorizontalAlignment::Left);
        host.VerticalAlignment(VerticalAlignment::Top);
        auto shape = compositor.CreateSpriteShape(geometry);
        shape.FillBrush(compositor.CreateColorBrush({255, 255, 255, 255}));
        mask = compositor.CreateShapeVisual();
        mask.Shapes().Append(shape);
        surface = compositor.CreateVisualSurface();
        surface.SourceVisual(mask);
        shadow = compositor.CreateDropShadow();
        shadow.Color({255, 0, 0, 0});
        shadow.Mask(compositor.CreateSurfaceBrush(surface));
        visual = compositor.CreateSpriteVisual();
        visual.Shadow(shadow);
        Hosting::ElementCompositionPreview::SetElementChildVisual(host, visual);
        uint32_t index{};
        if (!panel.Children().IndexOf(before, index))
            throw winrt::hresult_error(E_PENDING);
        panel.Children().InsertAt(index, host);
        parent = panel;
    }
    void Update(winrt::Windows::Foundation::Numerics::float2 size,
                winrt::Windows::Foundation::Numerics::float2 origin,
                LiquidSettings const& optics) {
        host.Width(size.x); host.Height(size.y);
        host.Translation({origin.x, origin.y, 0});
        mask.Size(size); visual.Size(size); surface.SourceSize(size);
        shadow.Opacity(optics.shadowOpacity * optics.opacity);
        shadow.BlurRadius(optics.shadowBlur);
        shadow.Offset({0, optics.shadowOffset, 0});
    }
    ~GlassShadow() {
        try {
            if (host) Hosting::ElementCompositionPreview::SetElementChildVisual(host, nullptr);
        } catch (...) {}
        try {
            uint32_t index{};
            if (parent && parent.Children().IndexOf(host, index))
                parent.Children().RemoveAt(index);
        } catch (...) {}
    }
};

Media::AcrylicBrush GlassMaterial(bool frosted, bool dark) {
    Media::AcrylicBrush acrylic;
    acrylic.BackgroundSource(Media::AcrylicBackgroundSource::Backdrop);
    acrylic.TintColor(dark ? winrt::Windows::UI::Color{255, 24, 26, 32}
                           : winrt::Windows::UI::Color{255, 250, 252, 255});
    acrylic.TintOpacity(frosted ? 0.45 : 0.08);
    acrylic.TintLuminosityOpacity(frosted ? 0.65 : 0.12);
    acrylic.FallbackColor(dark ? winrt::Windows::UI::Color{255, 32, 32, 32}
                               : winrt::Windows::UI::Color{255, 243, 243, 243});
    return acrylic;
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

struct ForegroundLuminanceHistory {
    struct Sample { ULONGLONG at; double luminance; };
    std::vector<Sample> samples;
    double average{};

    double Add(double luminance, ULONGLONG now, ULONGLONG windowMs) {
        if (!samples.empty() && now <= samples.back().at) return average;
        // Start immediately, and forget history after a capture gap or resume.
        if (!windowMs || samples.empty() || now - samples.back().at >= windowMs) {
            samples.assign(1, {now, luminance});
            return average = luminance;
        }
        ULONGLONG start = now > windowMs ? now - windowMs : 0;
        // Retain the sample spanning the left edge of the window.
        while (samples.size() > 1 && samples[1].at <= start)
            samples.erase(samples.begin());
        samples.push_back({now, luminance});
        start = std::max(start, samples.front().at);
        double total{};
        for (size_t i = 1; i < samples.size(); ++i) {
            auto const& previous = samples[i - 1];
            total += previous.luminance *
                (samples[i].at - std::max(start, previous.at));
        }
        // Weight elapsed time, so timer jitter cannot bias the brightness.
        return average = total / (now - start);
    }
};

using TaskbarConfiguration_GetFrameSize_t = double(WINAPI*)(int);
TaskbarConfiguration_GetFrameSize_t TaskbarConfiguration_GetFrameSize_Original;
double WINAPI TaskbarConfiguration_GetFrameSize_Hook(int taskbarSize) {
    double height = TaskbarConfiguration_GetFrameSize_Original(taskbarSize);
    double adjustment = DockFrameAdjustment(g_dockEdgeDistance);
    // taskbar-icon-size returns an absolute height.
    if (adjustment != 0 && !g_dockingDisabled &&
        (taskbarSize == 1 || taskbarSize == 2) &&
        std::isfinite(height) && height > 0 && height + adjustment > 0 &&
        !TaskbarIconSizeLoaded())
        height += adjustment;
    return height;
}

// ---- taskbar.dll symbol lookup and XAML root discovery ------------------
// Same taskbar.dll root lookup used by hide-start-button and
// taskbar-centered-start-split-icons.
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

void* g_trayPreferenceVtable{};
size_t g_trayPreferenceOffset{};
std::set<uint64_t> g_verifiedTrayIds; // Taskbar owner thread.

size_t TrayPreferenceOffset(std::span<const BYTE> code) {
    // NotificationAreaIcon2 saves the constructor's (id, registry-key) pair.
    // Recognize this compiler shape rather than assuming an object offset.
    constexpr BYTE savePair[]{0x4D, 0x8B, 0xF0}; // mov r14,r8
    constexpr BYTE saveThis[]{0x48, 0x8B, 0xF9}; // mov rdi,rcx
    constexpr BYTE storeId[]{0x49,0x8B,0x06,0x48,0x89,0x87}; // mov rax,[r14]; mov [rdi+offset],rax
    auto prefix = code.first(std::min<size_t>(64, code.size()));
    if (std::search(prefix.begin(), prefix.end(), std::begin(savePair), std::end(savePair)) == prefix.end() ||
        std::search(prefix.begin(), prefix.end(), std::begin(saveThis), std::end(saveThis)) == prefix.end())
        return 0;
    auto match = std::search(code.begin(), code.end(), std::begin(storeId), std::end(storeId));
    if (match == code.end() || code.end() - match < 10) return 0;
    uint32_t offset{};
    memcpy(&offset, &*(match + 6), sizeof(offset));
    if (offset < 64 || offset > 1024 || offset % sizeof(void*)) return 0;
    if (std::search(match + 10, code.end(), std::begin(storeId), std::end(storeId)) != code.end()) return 0;
    return offset;
}

void ResolveTrayPreferenceSymbols() {
    HMODULE module = GetModuleHandleW(L"taskbar.dll");
    WH_FIND_SYMBOL_OPTIONS options{sizeof(options), nullptr, TRUE};
    WH_FIND_SYMBOL symbol{};
    HANDLE search = Wh_FindFirstSymbol(module, &options, &symbol);
    if (!search) return;
    void* constructor{};
    do {
        auto text = symbol.symbolDecorated ? symbol.symbolDecorated : symbol.symbol;
        if (!text) continue;
        std::wstring_view name(text);
        if (name.starts_with(L"??0NotificationAreaIcon2@implementation@Shell@UI@WindowsUdk@winrt@@") &&
            name.find(L"NotificationAreaIconIdentity") != name.npos)
            constructor = symbol.address;
        if (name.starts_with(L"??_7?$heap_implements@UNotificationAreaIcon2@implementation@Shell@UI@WindowsUdk@winrt@@"))
            g_trayPreferenceVtable = symbol.address;
        if (constructor && g_trayPreferenceVtable) break;
    } while (Wh_FindNextSymbol(search, &symbol));
    Wh_FindCloseSymbol(search);
    std::array<BYTE, 0x300> code{};
    if (constructor && g_trayPreferenceVtable && ReadLocal(constructor, code))
        g_trayPreferenceOffset = TrayPreferenceOffset(code);
    Wh_Log(L"[UWTRAY] persistent identity support=%d", bool(g_trayPreferenceOffset));
}

uint64_t TrayPreferenceIdFromAbi(void* pointer) {
    if (!pointer || !g_trayPreferenceVtable || !g_trayPreferenceOffset) return 0;
    auto address = reinterpret_cast<uintptr_t>(pointer);
    for (size_t back = 0; back <= 64 && address >= back; back += sizeof(void*)) {
        void* vtable{};
        if (!ReadLocal(reinterpret_cast<void*>(address - back), vtable)) continue;
        if (vtable != g_trayPreferenceVtable) continue;
        uint64_t id{};
        if (ReadLocal(reinterpret_cast<void*>(address - back + g_trayPreferenceOffset), id)) return id;
    }
    return 0;
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
    std::shared_ptr<GlassShadow> shadow;
    ElementTheme materialTheme{};
    bool materialBlocked{};
    ULONGLONG liquidRetryAt{};
    unsigned liquidFailures{};
};
struct NativePlateFill;
struct PlateTemplateValues;
struct ButtonStateObserver;

// LayoutUpdated is shared by the XAML layout manager, even when this root did
// not change. Pin the current XAML peers and native layout, not animation samples.
struct RootLayoutSnapshot {
    struct Values {
        winrt::hstring name;
        int children{};
        winrt::Windows::Foundation::Rect slot{};
        winrt::Windows::Foundation::Size size{},desired{};
        float3 offset{};
        Thickness margin{};
        CornerRadius corners{};
        Visibility visibility{};
        ElementTheme theme{};
        bool operator==(Values const&) const = default;
    };
    struct Node {
        DependencyObject element{nullptr};
        Values values;
    };
    std::vector<Node> nodes;
    std::vector<std::pair<size_t,Values>> changes;
    RECT window{};
    double scale{};
    DockInsets insets{};
    bool highContrast{},ready{};

    bool Visit(DependencyObject const& element, size_t& index, bool capture, unsigned depth=32) {
        if (!element || !depth) return false;
        Values values;
        values.children=Media::VisualTreeHelper::GetChildrenCount(element);
        if (auto frame=element.try_as<FrameworkElement>()) {
            values.name=frame.Name();
            values.slot=Controls::Primitives::LayoutInformation::GetLayoutSlot(frame);
            values.size={float(frame.ActualWidth()),float(frame.ActualHeight())};
            values.desired=frame.DesiredSize();
            values.offset=frame.ActualOffset();
            values.margin=frame.Margin();
            values.visibility=frame.Visibility();
            values.theme=frame.ActualTheme();
            if (auto border=frame.try_as<Controls::Border>()) values.corners=border.CornerRadius();
        }
        if (capture) nodes.push_back({element,values});
        else {
            if (index>=nodes.size() || nodes[index].element!=element ||
                nodes[index].values.children!=values.children) return false;
            if (nodes[index].values!=values) changes.emplace_back(index,values);
        }
        ++index;
        for (int i=0;i<values.children;++i)
            if (!Visit(Media::VisualTreeHelper::GetChild(element,i),index,capture,depth-1)) return false;
        return true;
    }
    void Clear() { ready=false; nodes.clear(); changes.clear(); }
};

struct Root {
    struct StatePlate {
        Controls::Border element{nullptr};
        winrt::Windows::Foundation::IInspectable originalCorners{nullptr};
        Data::Binding originalCornerBinding{nullptr};
        CornerRadius appliedCorners{};
        bool cornersOwned{};
        std::array<winrt::Windows::Foundation::IInspectable,3> originalLayout{nullptr,nullptr,nullptr};
        std::array<Data::Binding,3> layoutBindings{nullptr,nullptr,nullptr};
        Thickness appliedMargin{};
        double appliedHeight{};
        bool layoutOwned{};
        ShapeVisual rim{nullptr};
        ElementTheme rimTheme{};
        bool rimThemeReady{};
        Visual visual{nullptr};
        CompositionGeometricClip clip{nullptr};
        CompositionRoundedRectangleGeometry clipGeometry{nullptr};
        CompositionPropertySet clipBounds{nullptr};
        bool fillHidden{};
        std::shared_ptr<NativePlateFill> fill{};
        std::shared_ptr<PlateTemplateValues> templateValues{};
        bool templateChecked{};
        uintptr_t buttonOwner{};
        bool trayHighlight{},stateRefreshPending{};
        std::shared_ptr<ButtonStateObserver> trayStates{};
        bool blocked{};
    };
    FrameworkElement frame{nullptr};
    winrt::event_token layoutToken{};
    bool subscribed{};
    OwnedTranslation buttonsTranslation;
    OwnedTranslation trayTranslation;
    OwnedDock dock;
    std::unique_ptr<GlassPointerBinding> glassPointer;
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
        bool stateRefreshPending{};
        bool included{};
        Controls::Border plate{nullptr};
        std::shared_ptr<ButtonStateObserver> states{};
    };
    struct ForegroundState {
        // Keep the peer alive until its owned foreground has been restored.
        Controls::TextBlock element{nullptr};
        uintptr_t identity{};
        winrt::Windows::Foundation::IInspectable original{nullptr};
        Media::SolidColorBrush brush{nullptr};
        animation::Storyboard transition{nullptr};
        bool owned{};
        bool light{};
        bool ready{};
        ForegroundLuminanceHistory luminance;
    };
    struct TrayIconState {
        winrt::weak_ref<FrameworkElement> element;
        winrt::weak_ref<Controls::Image> image;
        HWND taskbar{};
        std::wstring key;
        TrayIconFiles files;
        Visual originalVisual{nullptr};
        float originalOpacity{};
        bool opacityOwned{}, light{}, ready{};
        ForegroundLuminanceHistory luminance;
        int appliedVariant = -1;
        CompositionVisualSurface surface{nullptr};
        SpriteVisual overlay{nullptr};
        CompositionBrush nativeBrush{nullptr};
        std::array<std::shared_ptr<TrayImageAsset>, 2> custom;
    };
    std::vector<Source> sources;
    std::vector<StatePlate> statePlates;
    std::vector<ForegroundState> foregrounds;
    ULONGLONG foregroundSampleAt{};
    bool foregroundSamplePending{};
    ExpressionAnimation follower{nullptr};
    ExpressionAnimation buttonsFollower{nullptr};
    winrt::weak_ref<FrameworkElement> anchor;
    CompositionPropertySet metrics{nullptr};
    double nativeRightOverhang{NAN};
    double hostWidth{};
    Measurements measurements{};
    bool measurementsReady{};
    bool sourceLifecyclePending{};
    RootLayoutSnapshot layout;
    bool retired{};
};
template <class T> void ReleaseVector(std::vector<T>& vector) { std::vector<T>().swap(vector); }
// Globals marked no_destroy skip the CRT destructor, which at process exit
// would release XAML and composition state after XAML has shut down.
[[clang::no_destroy]] std::vector<Root> g_roots;  // Confined to the one observed taskbar UI thread.

void PostItemStateRefresh() {
    if (g_stopping || g_busy || g_itemStateRefreshPending || !g_dispatch) return;
    for (auto const& root : g_roots) {
        if (root.retired || root.blocked || !root.measurementsReady) continue;
        if (std::any_of(root.sources.begin(), root.sources.end(),
                        [](auto const& source) { return source.stateRefreshPending; }) ||
            std::any_of(root.statePlates.begin(),root.statePlates.end(),
                        [](auto const& plate) { return plate.trayHighlight && plate.stateRefreshPending; })) {
            if (PostMessageW(g_dispatch, kRefreshItemStatesMessage, 0, 0))
                g_itemStateRefreshPending = true;
            return;
        }
    }
}

void RequestItemStateRefresh(uintptr_t owner) {
    ActiveScope active;
    if (!owner || g_stopping || GetCurrentThreadId() != g_ownerThread) return;
    for (auto& root : g_roots) {
        if (root.retired) continue;
        for (auto& source : root.sources) {
            if (source.identity != owner) continue;
            source.stateRefreshPending = true;
            PostItemStateRefresh();
            return;
        }
        bool found=false;
        for(auto& plate:root.statePlates)
            if(plate.trayHighlight && plate.buttonOwner==owner) {
                plate.stateRefreshPending=true;
                found=true;
            }
        if(found) {PostItemStateRefresh();return;}
    }
}

[[clang::no_destroy]] std::vector<Root::TrayIconState> g_trayIcons; // Includes realized overflow views.
struct LoadedTrayView {
    winrt::weak_ref<FrameworkElement> element;
    winrt::event_token sizeChanged{};
};
std::vector<LoadedTrayView> g_loadedTrayViews;

// WGC sessions can outlive DWM without reporting a D3D device removal. Track
// compositor identity separately so cached frames never survive that boundary.
DWORD QueryCompositorProcessId() noexcept {
    DWORD current{};
    // DWM is protected: ProcessIdToSessionId/OpenProcess can fail even for a
    // read-only query. WTS exposes process identity for our session directly.
    HMODULE module = LoadLibraryExW(L"wtsapi32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) return current;
    auto enumerate = reinterpret_cast<decltype(&WTSEnumerateProcessesW)>(
        GetProcAddress(module, "WTSEnumerateProcessesW"));
    auto release = reinterpret_cast<decltype(&WTSFreeMemory)>(
        GetProcAddress(module, "WTSFreeMemory"));
    WTS_PROCESS_INFOW* processes{};
    DWORD session{}, count{};
    if (enumerate && release && ProcessIdToSessionId(GetCurrentProcessId(), &session) &&
        enumerate(WTS_CURRENT_SERVER_HANDLE, 0, 1, &processes, &count)) {
        for (DWORD i = 0; i < count; ++i) {
            if (processes[i].SessionId == session && processes[i].pProcessName &&
                _wcsicmp(processes[i].pProcessName, L"dwm.exe") == 0) {
                current = processes[i].ProcessId;
                break;
            }
        }
        release(processes);
    }
    FreeLibrary(module);
    return current;
}

struct CompositorProcessCache {
    std::mutex mutex;
    DWORD current{};
    ULONGLONG nextProbe{};
    PTP_WORK work{};
    bool pending{},stopping{};
    DWORD Get() {
        std::lock_guard lock(mutex);
        auto now=GetTickCount64();
        if(g_stopping || stopping || pending || now<nextProbe) return current;
        if(!work) work=CreateThreadpoolWork([](PTP_CALLBACK_INSTANCE,void* context,PTP_WORK) {
            ActiveScope active;
            auto& cache=*static_cast<CompositorProcessCache*>(context);
            DWORD process=g_stopping?0:QueryCompositorProcessId();
            std::lock_guard lock(cache.mutex);
            if(process) cache.current=process;
            cache.pending=false;
        },this,nullptr);
        nextProbe=now+1000;
        if(work) {
            pending=true;
            SubmitThreadpoolWork(work);
        }
        return current;
    }
    void Stop() {
        PTP_WORK old{};
        {std::lock_guard lock(mutex);stopping=true;old=std::exchange(work,nullptr);}
        if(old) {
            WaitForThreadpoolWorkCallbacks(old,TRUE);
            CloseThreadpoolWork(old);
        }
        std::lock_guard lock(mutex);
        pending=false;
    }
} g_compositorProcessCache;

DWORD CompositorProcessId() {
    // A WTS enumeration can take tens of milliseconds. UI and capture callers
    // only read the cached identity; one owned worker refreshes it.
    return g_compositorProcessCache.Get();
}

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
        float lensFades[kMaxButtonLenses][4]{}; // x: 0 visible, 1 faded; remaining components reserved for cbuffer alignment.
        uint32_t lensCount{};
        float lensRefraction{};
        float alertTintOpacity{};
        float lensBlend{};
        uint32_t compositorClip{};
        float bodyMode{}; // 0: taskbar; 1: UI glass; 3: composed; 4: Native; 5: Mica; 6: Acrylic.
        float bodyBlur{};
        float clipPadding{};
        float bodyMaterialTint[4]{};
        float fluidPointer[4]{}; // Position and settled displacement, output pixels.
        float glintClock[4]{}; // Time, pointer present, animation allowed, reserved.
    };
    static_assert(sizeof(LensConstants) % 16 == 0);
    // Pins the cbuffer arithmetic the HLSL side relies on: a 224-byte header,
    // six float4 arrays, then five trailing 16-byte registers. Adding a member without
    // accounting for it here would silently shift every lens the shader reads.
    static_assert(sizeof(LensConstants) ==
                   224 + 6 * 16 * kMaxButtonLenses + 80);

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
        bool visualBackdrop{};
        RECT rect{};
        capture::GraphicsCaptureItem item{nullptr};
        capture::Direct3D11CaptureFramePool pool{nullptr};
        capture::GraphicsCaptureSession session{nullptr};
        winrt::event_token frameToken{};
        winrt::event_token closedToken{};
        winrt::com_ptr<ID3D11Texture2D> latest;
        winrt::com_ptr<ID3D11ShaderResourceView> latestView;
        decltype(item.Size()) poolSize{};
        decltype(item.Size()) contentSize{};
        decltype(item.Size()) textureSize{};
        RECT latestTexels{};
        std::chrono::steady_clock::time_point lastFrame{};
        std::atomic<bool> active{};
        std::atomic<bool> closed{};
        std::mutex callbackMutex;
        std::mutex lifecycleMutex;
        winrt::handle readbackReady;
    };

    static constexpr size_t kMaxAppSources = 7;
    static constexpr int32_t kPoolBuffers = 2;
    static constexpr char kShader[] = R"(
Texture2D<float4> backdrop : register(t0);
Texture2D<float4> blurredBackdrop : register(t1);
Texture2D<float4> bodyBackdrop : register(t2);
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
    float model;
    float surfaceProfile;
    float bezelWidth;
    float glassThickness;
    float refractiveIndex;
    float magnification;
    float distortion;
    float saturation;
    float glassTint;
    float lighting;
    float lightAngle;
    float rimOpacity;
    float rimSaturation;
    float shadowOpacity;
    float shadowBlur;
    float shadowOffset;
    float rimRefraction, rimWidth, bevelSharpness, cornerBoost;
    float rippleStrength, rippleWavelength, edgeWidth, edgeSoftness;
    float frostEdge, coupledChroma, dyeRed, dyeGreen;
    float dyeBlue, dyeStrength, fluidStrength, fluidRadius;
    float fluidViscosity, glintMode, glintStrength, glintSpeed;
    float glintSize, boundaryFade, extraPadding0, extraPadding1;
    float4 lensRects[BUTTON_LENS_CAP];
    float4 lensTints[BUTTON_LENS_CAP];
    float4 lensRadii[BUTTON_LENS_CAP];
    float4 lensNativeRects[BUTTON_LENS_CAP];
    float4 lensShapeParams[BUTTON_LENS_CAP];
    float4 lensFades[BUTTON_LENS_CAP];
    uint lensCount;
    float lensRefraction;
    float alertTintOpacity;
    float lensBlend;
    uint compositorClip;
    float bodyMode;
    float bodyBlur;
    float clipPadding;
    float4 bodyMaterialTint;
    float4 fluidPointer;
    float4 glintClock;
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
float continuousDistanceFor(float2 p, float2 size, float cornerRadius) {
    float r = min(cornerRadius, min(size.x, size.y));
    float2 q = abs(p) - size + r;
    float2 v = max(q, 0);
    float norm = pow(dot(v * v, v * v), 0.25);
    // Fourth-power corners meet straight edges with zero curvature. Normalize
    // the implicit field's gradient to keep coverage/bevel distances in pixels.
    float2 n = v / max(norm, 0.0001);
    return (norm + min(max(q.x, q.y), 0) - r) /
            max(length(n * n * n), 0.59460356);
}
float continuousCoverage(float2 p, float2 size, float r) {
    float distance = continuousDistanceFor(p, size, r);
    if (distance <= -1) return 1;
    if (distance >= 1) return 0;
    // Integrate the silhouette over the device pixel only at its boundary.
    // This retains partial coverage on diagonals without blurring the surface.
    float coverage = 0;
    [unroll] for (uint aaY = 0; aaY < 4; ++aaY)
        [unroll] for (uint aaX = 0; aaX < 4; ++aaX)
            coverage += continuousDistanceFor(
                p + (float2(aaX, aaY) + .5) / 4 - .5, size, r) <= 0 ? 1 : 0;
    return coverage / 16;
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
    if (bodyMode != 0) return continuousDistanceFor(p, halfSize, radius);
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
    if (bodyMode == 1) return continuousDistanceFor(p, size, cornerRadius);
    return roundedDistanceFor(
        p, size, min(cornerRadius, min(size.x, size.y)));
}
float opticalDistanceForCorners(float2 p, float2 size, float4 radii) {
    float cornerRadius = cornerValue(p, radii);
    if (bodyMode == 1) return continuousDistanceFor(p, size, cornerRadius);
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
                          float4 clipRect, float2 clipRadius, bool continuous) {
    float nativeDistance = continuous ? continuousDistanceFor(
        pixel - nativeRect.xy, nativeRect.zw, cornerValue(pixel - nativeRect.xy, nativeRadiiX)) :
        ellipticalDistanceForCorners(
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
    if (surfaceProfile < 0.5) {
        if (inside <= 0) return 0;
        if (inside >= depthRadius) return depthRadius;
        return sqrt(inside * (2 * depthRadius - inside));
    }
    float x = saturate(inside / max(depthRadius, 0.5));
    float circle = sqrt(saturate(1 - (1 - x) * (1 - x)));
    if (surfaceProfile < 1.5)
        return depthRadius * pow(saturate(1 - pow(1 - x, 4)), 0.25);
    if (surfaceProfile < 2.5) return depthRadius * (1 - circle);
    float blend = x * x * x * (x * (x * 6 - 15) + 10);
    return depthRadius * lerp(circle, 1 - circle, blend);
}
float3 blurredColor(float2 pixel, bool body) {
    return body ? bodyBackdrop.SampleLevel(linearClamp, pixel / imageSize, 0).rgb :
                  blurredBackdrop.SampleLevel(linearClamp, pixel / imageSize, 0).rgb;
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
float edgeEnvelope(float inside, float2 halfExtent, float automaticWidth) {
    float width = edgeWidth > 0 ? edgeWidth : min(halfExtent.x, halfExtent.y) * automaticWidth;
    return pow(smoothstep(max(width, 0.5), 0, inside), 1 / max(edgeSoftness, 0.1));
}
void glassRefraction(inout Glass g, float2 gradient, float height, float bevel,
                     float2 p, float2 halfExtent, float amount,
                     float shiftGain, float radialGain) {
    g.normal = normalize(float3(-gradient, 1));
    if (model > 0.5 && model < 1.5) {
        // Snell's law at one air/glass interface; intersect the refracted ray
        // with the plane below the base thickness and curved bezel.
        float3 ray = refract(float3(0, 0, -1), g.normal,
                             1 / max(refractiveIndex, 1));
        g.shift = ray.xy * ((glassThickness + height) / max(-ray.z, 0.05)) * amount;
    } else if (model > 2.5) {
        // Artistic models adapted from dashersw/liquid-glass-js and liquidGL.
        // Work in output pixels, so capture size cannot change their strength.
        float inside = max(0, g.inside);
        float width = max(bezelWidth, 0.5);
        float2 radial = p / max(halfExtent, 1);
        float2 direction = length(g.normal.xy) > 0.0001 ?
            normalize(g.normal.xy) : radial / max(length(radial), 0.0001);
        float edge = saturate(1 - smoothstep(0, width, inside));
        float intensity = model < 3.5 ?
            exp(-inside * 3 / width) + rimRefraction * exp(-inside / max(rimWidth, 0.5)) :
            edge + rimRefraction * pow(edge, bevelSharpness);
        float centreFade = smoothstep(0, 0.4, length(radial));
        g.shift = direction * width * amount * intensity * centreFade;
    } else if (model > 1.5) {
        g.shift = -p * amount * g.depth * 0.35;
    } else {
        float refractivePower = 1 - 1 / 1.5;
        float normalizedThickness = height / max(bevel, 0.5);
        g.shift = gradient * refractivePower * (2 + normalizedThickness * 0.5) *
                  amount * shiftGain;
        g.shift += -p / max(halfExtent, 1) * amount * radialGain * g.depth;
    }
    if (cornerBoost > 0 || rippleStrength > 0) {
        float normalLength = length(g.normal.xy);
        float2 direction = g.normal.xy / max(normalLength, 0.0001);
        float edge = exp(-max(0, g.inside) / max(bezelWidth, 0.5));
        float corners = abs(direction.x * direction.y) * 2;
        g.shift += direction * cornerBoost * corners * amount * bezelWidth * edge;
        g.shift += float2(-direction.y, direction.x) * sin(g.inside * 6.2831853 / max(rippleWavelength, 1)) *
                   rippleStrength * amount * min(bezelWidth, 12) * edge;
    }
    g.shift += -p * (1 - 1 / (1 + magnification)) * g.depth;
}
Glass glassAt(float2 p, float2 halfExtent, float4 radii, float depthRadius,
              float refractionAmount, float shiftGain, float radialGain,
              float edgeGain) {
    Glass g = (Glass)0;
    g.distance = roundedDistanceForCorners(p, halfExtent, radii);
    g.inside = -g.distance;
    float e = 2;
    float opticalInside = -opticalDistanceForCorners(p, halfExtent, radii);
    float lensZ = min(model > 0.5 && model < 1.5 ? bezelWidth : depthRadius,
                      min(halfExtent.x, halfExtent.y));
    float hCenter = bevelHeightFor(opticalInside, lensZ);
    float2 gradient = float2(
        bevelHeightFor(-opticalDistanceForCorners(p + float2(e, 0), halfExtent, radii), lensZ) -
            bevelHeightFor(-opticalDistanceForCorners(p - float2(e, 0), halfExtent, radii), lensZ),
        bevelHeightFor(-opticalDistanceForCorners(p + float2(0, e), halfExtent, radii), lensZ) -
            bevelHeightFor(-opticalDistanceForCorners(p - float2(0, e), halfExtent, radii), lensZ)) / (2 * e);
    g.depth = smoothstep(0, max(lensZ, 0.5), opticalInside);
    glassRefraction(g, gradient, hCenter, lensZ, p, halfExtent,
                    refractionAmount, shiftGain, radialGain);
    g.edge = edgeEnvelope(g.inside, halfExtent, edgeGain);
    return g;
}
// A button surface is the intersection of the native Border and the inset clip
// installed by PrepareStatePlate. Coverage and every normal sample use the same
// two shapes in output-pixel coordinates.
Glass glassAtIntersection(float2 pixel, float4 nativeRect,
                          float4 nativeRadiiX, float nativeYOverX,
                          float4 clipRect, float2 clipRadius,
                          float depthRadius, float refractionAmount,
                          float shiftGain, float radialGain, float edgeGain, bool continuous) {
    Glass g = (Glass)0;
    g.distance = intersectedDistance(pixel, nativeRect, nativeRadiiX,
                                     nativeYOverX, clipRect, clipRadius, continuous);
    g.inside = -g.distance;
    float e = 2;
    // Coverage and all finite-difference samples use this same intersected
    // field, so smoothing cannot move the max() seam or invent inset coverage.
    float opticalInside = g.inside;
    float lensZ = min(model > 0.5 && model < 1.5 ? bezelWidth : depthRadius,
                      min(clipRect.z, clipRect.w));
    float hCenter = bevelHeightFor(opticalInside, lensZ);
    float2 gradient = float2(
        bevelHeightFor(-intersectedDistance(pixel + float2(e, 0), nativeRect,
            nativeRadiiX, nativeYOverX, clipRect, clipRadius, continuous), lensZ) -
        bevelHeightFor(-intersectedDistance(pixel - float2(e, 0), nativeRect,
            nativeRadiiX, nativeYOverX, clipRect, clipRadius, continuous), lensZ),
        bevelHeightFor(-intersectedDistance(pixel + float2(0, e), nativeRect,
            nativeRadiiX, nativeYOverX, clipRect, clipRadius, continuous), lensZ) -
        bevelHeightFor(-intersectedDistance(pixel - float2(0, e), nativeRect,
            nativeRadiiX, nativeYOverX, clipRect, clipRadius, continuous), lensZ)) /
        (2 * e);
    g.depth = smoothstep(0, max(lensZ, 0.5), opticalInside);
    float2 p = pixel - clipRect.xy;
    glassRefraction(g, gradient, hCenter, lensZ, p, clipRect.zw,
                    refractionAmount, shiftGain, radialGain);
    g.edge = edgeEnvelope(g.inside, clipRect.zw, edgeGain);
    return g;
}
// Refracted, chromatically split and optionally blurred backdrop for one surface.
float glassNoise(float2 p) {
    return frac(sin(dot(p, float2(127.1, 311.7))) * 43758.5453);
}
float2 boundedSample(float2 original, float2 candidate) {
    if (boundaryFade <= 0) return candidate;
    float2 outside = max(0.5 - candidate, candidate - (imageSize - 0.5));
    return lerp(candidate, original, smoothstep(0, max(boundaryFade, 0.5), max(outside.x, outside.y)));
}
float3 sharpColor(float2 original, float2 candidate) {
    float4 value = backdrop.SampleLevel(linearClamp, candidate / imageSize, 0);
    return value.a < 0.1 ? backdrop.SampleLevel(linearClamp, original / imageSize, 0).rgb : value.rgb;
}
float3 refractedBackdrop(float2 pixel, Glass g, float2 localPoint, float2 halfExtent, bool body) {
    float chromaPixels = chroma * 36 * (g.edge * 0.7 + 0.3);
    float2 chromaShift = g.normal.xy * chromaPixels;
    if (coupledChroma > 0.5) chromaShift = -g.shift * chroma;
    float2 samplePixel = pixel + g.shift;
    if (glintClock.z > 0.5 && fluidStrength > 0) {
        float extent = min(halfExtent.x, halfExtent.y) * 2;
        float reach = max(1, extent * fluidRadius);
        float influence = 1 - smoothstep(0, reach, length(pixel - fluidPointer.xy));
        float displacementLength = max(0.001, length(fluidPointer.zw));
        samplePixel -= fluidPointer.zw * min(1, extent * 0.12 * fluidStrength / displacementLength) * influence;
    }
    float2 noise = localPoint * 0.08;
    if (distortion > 0.001)
        samplePixel += (float2(glassNoise(noise), glassNoise(noise + 37)) - 0.5) *
                       distortion * 4;
    float2 greenPixel = boundedSample(pixel, samplePixel);
    float2 redPixel = boundedSample(pixel, samplePixel + chromaShift);
    float2 bluePixel = boundedSample(pixel, samplePixel - chromaShift);
    float3 sharp = sharpColor(pixel, greenPixel);
    if (chroma > 0.001) {
        sharp.r = sharpColor(pixel, redPixel).r;
        sharp.b = sharpColor(pixel, bluePixel).b;
    }
    if ((body ? bodyBlur : blurAmount) > 0.001) {
        float3 blurred = float3(
            blurredColor(redPixel, body).r,
            blurredColor(greenPixel, body).g,
            blurredColor(bluePixel, body).b);
        sharp = lerp(sharp, blurred, 1 - g.edge * frostEdge);
    }
    return sharp;
}
float directionalRim(Glass g) {
    return pow(saturate(dot(g.normal.xy, float2(cos(lightAngle), sin(lightAngle)))), 2);
}
float3 glassFinish(float3 color, Glass g) {
    // A depth-dependent lift turns the flat interior into a visible inset fill.
    // Keep all glass neutral unless its Brightness control is changed.
    color *= 1 + brightness;
    float luminance = dot(color, float3(0.299, 0.587, 0.114));
    color = lerp(luminance, color, 1 + saturation);
    color = lerp(color, color * float3(0.92, 0.95, 1.05), glassTint);
    color *= lerp(float3(1, 1, 1), float3(dyeRed, dyeGreen, dyeBlue), dyeStrength);
    if (lighting > 1.5) {
        float3 saturated = lerp(dot(color, float3(0.299, 0.587, 0.114)),
                                color, rimSaturation);
        color = lerp(color, saturated, directionalRim(g) * rimOpacity);
    }
    return max(color, 0);
}
// Specular, fresnel, rim and inner stroke for one surface.
float3 glassLighting(Glass g, float2 pixel, float2 p, float2 halfExtent, out float fresnelAmount) {
    fresnelAmount = pow(1 - abs(g.normal.z), 4) * fresnel;
    float3 view = float3(0, 0, 1);
    float highlight = pow(saturate(dot(g.normal,
        normalize(normalize(float3(0.4, 0.7, 1)) + view))), 90);
    highlight += pow(saturate(dot(g.normal,
        normalize(normalize(float3(-0.3, -0.5, 1)) + view))), 50) * 0.3;
    if (lighting > 0.5 && lighting < 1.5) {
        highlight += pow(saturate(dot(g.normal, normalize(float3(0.1, 0.3, 1)))), 6) * 0.1;
        highlight += pow(saturate(dot(g.normal,
            normalize(normalize(float3(0, 0.9, 0.4)) + view))), 120) * 0.6;
    }
    highlight = lighting > 1.5 ? directionalRim(g) * rimOpacity : highlight * specular;
    highlight *= g.edge;
    if (glintClock.z > 0.5 && glintMode > 0.5) {
        float2 light = fluidPointer.xy - (pixel - p);
        float active = glintClock.y;
        if (glintMode < 1.5) {
            float t = glintClock.x;
            light = float2(cos(t * 0.7), sin(t * 0.9)) * halfExtent;
            active = 1;
        }
        float reach = max(1, min(halfExtent.x, halfExtent.y) * 2 * glintSize);
        highlight += (1 - smoothstep(0, reach, length(p - light))) * glintStrength * g.edge * active;
    }
    float innerStroke = smoothstep(-2.5, -1.5, g.distance) *
                        (1 - smoothstep(-1, 0, g.distance));
    float topBias = 0.5 + 0.5 * (-p.y / halfExtent.y);
    innerStroke *= 0.4 + 0.6 * topBias;
    float rim = g.edge * edgeHighlight * 0.22;
    float innerGlow = smoothstep(5, 0, g.inside) * edgeHighlight * 0.15;
    float environment = (g.normal.y * 0.5 + 0.5) * fresnelAmount * 0.08;
    return highlight + rim + innerGlow +
           innerStroke * edgeHighlight * 0.55 + environment;
}
float4 shadePixel(float2 pixel) {
    // The surface stays in screen coordinates. Only the native clip moves;
    // resizing a sprite would stretch a crop from the final destination.
    float2 p = pixel - center;
    float distance = roundedDistance(p);
    // Runtime already has the compositor's animated rounded clip. Sampled
    // shader bounds can reach the layout target before native motion starts;
    // a second mask would cut away the still-visible part of the dock.
    if (!compositorClip && bodyMode == 0 && distance > 0) return 0;
    if (bodyMode == 3 && distance > 1) return 0;
    float4 original = backdrop.SampleLevel(linearClamp, pixel / imageSize, 0);
    if (original.a <= 0.001) return 0;

    // Each button is a glass surface over the shared dock. Buttons do not overlap,
    // so the first hit wins and the cheap distance rejects every other lens. The
    // dock remains beneath the lens so sampled button geometry can't expose the
    // raw backdrop during native motion.
    float coverage = 0;
    float3 buttonColor = 0;
    for (uint i = 0; i < min(lensCount, BUTTON_LENS_CAP); ++i) {
        float nativeOpacity = 1 - saturate(lensFades[i].x);
        if (nativeOpacity <= 0) continue;
        float4 clipRect = lensRects[i];
        float4 nativeRect = lensNativeRects[i];
        float4 shapeParams = lensShapeParams[i];
        float distance = intersectedDistance(
            pixel, nativeRect, lensRadii[i], shapeParams.x,
            clipRect, shapeParams.yz, shapeParams.w == 1);
        if (distance > (shapeParams.w == 1 ? 1 : 0.5)) continue;
        Glass item = glassAtIntersection(
            pixel, nativeRect, lensRadii[i], shapeParams.x,
            clipRect, shapeParams.yz, zRadius * 0.65,
            lensRefraction, 18, 3, 0.45, shapeParams.w == 1);
        float itemMask = 1 - smoothstep(-1.5, 0.5, item.distance);
        if (shapeParams.w == 1) {
            float2 panelPoint = pixel - nativeRect.xy;
            itemMask = continuousCoverage(panelPoint, nativeRect.zw, cornerValue(panelPoint, lensRadii[i]));
            float2 clipCoverage = saturate(clipRect.zw - abs(pixel - clipRect.xy) + .5);
            itemMask *= clipCoverage.x * clipCoverage.y;
        }
        if (itemMask <= 0) continue;
        buttonColor = refractedBackdrop(pixel, item, pixel - clipRect.xy, clipRect.zw, false);
        buttonColor = glassFinish(buttonColor, item);
        buttonColor = lerp(buttonColor, lensTints[i].rgb,
                           alertTintOpacity * lensTints[i].a);
        float itemFresnel;
        float2 itemPoint = pixel - clipRect.xy;
        buttonColor += glassLighting(item, pixel, itemPoint, clipRect.zw, itemFresnel);
        buttonColor = lerp(buttonColor, 1, itemFresnel * 0.2);
        coverage = itemMask * (shapeParams.w == 2 ? 1 : lensBlend) * nativeOpacity;
        break;
    }

    // Biconvex pill profile adapted from ybouane/liquidglass (MIT):
    // https://github.com/ybouane/liquidglass. The curved height field creates
    // the reference's raised glass distortion across the whole surface. These are
    // the dock's gains; radius is already clamped to min(halfSize), so four equal
    // corners here match the single-radius distance functions this replaced.
    float3 color = buttonColor;
    if (coverage < 0.999) {
        float3 dockColor = original.rgb;
        if (bodyMode == 5) {
            // Windows supplies the blurred wallpaper; apply the theme's Mica tint.
            dockColor = lerp(dockColor, bodyMaterialTint.rgb, bodyMaterialTint.a);
        }
        if (bodyMode < 2.5) {
            Glass dock = glassAt(p, halfSize, float4(radius, radius, radius, radius),
                                 zRadius, refraction, 30, 4, 0.35);
            dockColor = refractedBackdrop(pixel, dock, p, halfSize, bodyMode > 0.5);
            dockColor = glassFinish(dockColor, dock);
            float fresnelAmount;
            dockColor += glassLighting(dock, pixel, p, halfSize, fresnelAmount);
            dockColor = lerp(dockColor, 1, fresnelAmount * 0.2);
        }
        color = lerp(dockColor, buttonColor, coverage);
    }

    // Clip the finished UI once; keep its intermediate body opaque so edge
    // antialiasing isn't multiplied when panels are composed over it.
    float mask = bodyMode == 3 ? continuousCoverage(p, halfSize, radius) :
        !compositorClip && bodyMode == 0 ? 1 - smoothstep(-1.5, 0.5, distance) : 1;
    float alpha = mask * opacity * original.a;
    return float4(color * alpha, alpha);
}
float4 ps(VertexOutput input) : SV_Target {
    float2 pixel = input.position.xy;
    float distance = roundedDistance(pixel - center);
    // Refraction and lighting can vary faster than a pixel around the rim.
    // Filter that narrow band without adding a second runtime silhouette mask:
    // the compositor still owns the dock's animated clip.
    bool filterRim = bodyMode == 0 && distance >= -8 && distance <= 1;
    float weight = smoothstep(-8, -6, distance);
    uint samples = filterRim ? (distance < -6 ? 5 : 4) : 1;
    float4 color = 0;
    [loop] for (uint i = 0; i < samples; ++i) {
        bool middle = samples == 1 || i == 4;
        float2 offset = middle ? 0 : float2(i & 1, i >> 1) * .5 - .25;
        float contribution = samples == 1 ? 1 : middle ? 1 - weight : weight * .25;
        color += saturate(shadePixel(pixel + offset)) * contribution;
    }
    return color;
}
)";

    std::timed_mutex mutex;
    DWORD compositorProcess{};
    ContainerVisual geometryVisual{nullptr};
    std::array<CompositionColorBrush, 4> dockSamples{nullptr, nullptr, nullptr, nullptr};
    std::shared_ptr<Source> geometrySource;
    winrt::com_ptr<ID3D11Texture2D> geometryReadback;
    float compositorBounds[4]{};
    struct LensProbe {
        Visual button{nullptr}, anchor{nullptr};
        FrameworkElement plate{nullptr};
        CompositionRoundedRectangleGeometry dock{nullptr};
        ContainerVisual row{nullptr};
        CompositionPropertySet values{nullptr};
        uint32_t tag{};
        float nativeCenter{}, clipCenter{};
    };
    std::array<LensProbe, kMaxButtonLenses> lensProbes;
    float sampledLensX[kMaxButtonLenses][2]{};
    float sampledLensOpacity[kMaxButtonLenses]{};
    uint32_t sampledLensTags[kMaxButtonLenses]{};
    uint32_t nextLensTag{};
    float geometryScale{1};
    bool haveCompositorBounds{};
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
    bool effectsAnimating{}, pointerActive{};
    BOOL effectsAllowed{TRUE};
    float pointerX{}, pointerY{}, velocityX{}, velocityY{};
    float fluidX{}, fluidY{}, fluidDx{}, fluidDy{};
    double effectSeconds{};
    std::chrono::steady_clock::time_point pointerAt{}, effectAt{}, preferenceAt{};
    bool motionObservationPending{};
    HRESULT failure{S_OK};
    std::chrono::steady_clock::time_point lastPresent{};
    HMONITOR presentMonitor{};
    HWND presentationWindow{}; // Static samples can have a display owner without a capture HWND.
    std::chrono::steady_clock::time_point nextDisplayProbe{};
    std::chrono::nanoseconds presentInterval{};
    bool limitToDisplay{true};
    PTP_WORK presentWork{};
    HANDLE frameLatencyWaitable{};
    HANDLE presentTimer{};
    struct PresentWorkContext {
        LiquidRenderer* renderer{};
    };
    PresentWorkContext* presentWorkContext{};
    std::vector<std::shared_ptr<Source>> sources;
    std::vector<std::shared_ptr<Source>> parked;
    PTP_WORK sourceWork{};
    bool sourceWorkPending{},sourceUpdatePending{};
    uint64_t sourceRevision{};
    std::vector<std::pair<HWND,RECT>> requestedSources;
    Visual bodyVisual{nullptr};

    winrt::com_ptr<ID3D11Device> device;
    winrt::com_ptr<ID3D11DeviceContext> context;
    direct3d11::IDirect3DDevice graphicsDevice{nullptr};
    winrt::com_ptr<IDXGISwapChain1> swapChain;
    winrt::com_ptr<ID3D11Texture2D> composite;
    winrt::com_ptr<ID3D11RenderTargetView> compositeTarget;
    winrt::com_ptr<ID3D11Texture2D> backBuffer;
    winrt::com_ptr<ID3D11RenderTargetView> backTarget;
    winrt::com_ptr<ID3D11ShaderResourceView> compositeView;
    winrt::com_ptr<ID3D11Texture2D> renderedBody;
    winrt::com_ptr<ID3D11RenderTargetView> renderedBodyTarget;
    winrt::com_ptr<ID3D11ShaderResourceView> renderedBodyView;
    // Shared scratch, panel result, and independently blurred window body.
    winrt::com_ptr<ID3D11Texture2D> blurTextures[3];
    winrt::com_ptr<ID3D11RenderTargetView> blurTargets[3];
    winrt::com_ptr<ID3D11ShaderResourceView> blurViews[3];
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
    SpriteVisual pixels{nullptr};

    ~LiquidRenderer() { TakeUnderLock().Drain(); }

    bool Initialize(Compositor const& compositor, HWND taskbarWindow,
                    UINT pixelWidth, UINT pixelHeight, HWND displayWindow=nullptr,
                    bool capToDisplay=true) {
        try {
            // A null taskbar hosts the editor's static sample, with no capture.
            if (!pixelWidth || !pixelHeight ||
                (taskbarWindow && !capture::GraphicsCaptureSession::IsSupported())) return false;
            taskbar = taskbarWindow;
            presentationWindow = displayWindow;
            limitToDisplay = capToDisplay;
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
            pixels = compositor.CreateSpriteVisual();
            pixels.RelativeSizeAdjustment({1, 1});
            pixels.Brush(surfaceBrush);
            visualRoot = compositor.CreateContainerVisual();
            visualRoot.RelativeSizeAdjustment({1, 1});
            visualRoot.Children().InsertAtBottom(pixels);
            if (!ResizeLocked(pixelWidth, pixelHeight)) return false;
            presentTimer = CreateWaitableTimerExW(nullptr, nullptr,
                CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_MODIFY_STATE | SYNCHRONIZE);
            if (!presentTimer) winrt::throw_last_error();
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
                    if (waited == WAIT_OBJECT_0) {
                        std::chrono::steady_clock::time_point deadline;
                        HANDLE timer{};
                        {
                            std::lock_guard lock(renderer->mutex);
                            deadline = renderer->lastPresent + renderer->presentInterval;
                            timer = renderer->presentTimer;
                        }
                        auto remaining = deadline - std::chrono::steady_clock::now();
                        if (timer && remaining > std::chrono::steady_clock::duration::zero()) {
                            using Tick = std::chrono::duration<LONGLONG, std::ratio<1,10000000>>;
                            LARGE_INTEGER due{};
                            due.QuadPart = -std::chrono::ceil<Tick>(remaining).count();
                            waited = SetWaitableTimer(timer, &due, 0, nullptr, nullptr, FALSE) ?
                                WaitForSingleObject(timer, 100) : WAIT_FAILED;
                        }
                    }
                    std::lock_guard lock(renderer->mutex);
                    if (!renderer->running || g_stopping) {
                        renderer->presentWorkPending = false;
                        return;
                    }
                    if (!waitable || waited == WAIT_FAILED) {
                        DWORD error = GetLastError();
                        renderer->failure =
                            error ? HRESULT_FROM_WIN32(error) : E_FAIL;
                    } else if (waited == WAIT_OBJECT_0 && renderer->running &&
                               !FAILED(renderer->failure) &&
                               renderer->geometryReady &&
                               renderer->pendingPresent) {
                        renderer->lastPresent =
                            std::chrono::steady_clock::now();
                        renderer->RenderLocked();
                    }
                    renderer->presentWorkPending = false;
                    if (renderer->motionActive || renderer->effectsAnimating || renderer->pendingPresent)
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

    void FollowDock(CompositionRoundedRectangleGeometry const& geometry, bool observe = true) {
        if (!observe) return;
        auto compositor = pixels.Compositor();
        if (!geometryVisual) {
            auto visual = compositor.CreateContainerVisual();
            visual.Size({8,float(2*(kMaxButtonLenses+1))});
            for (int i=0; i<4; ++i) {
                auto sample = compositor.CreateSpriteVisual();
                sample.Size({2,2}); sample.Offset({float(i*2),0,0});
                dockSamples[i] = compositor.CreateColorBrush();
                sample.Brush(dockSamples[i]);
                visual.Children().InsertAtTop(sample);
            }
            geometryVisual = visual;
        }
        PCWSTR properties[]{L"Offset.X",L"Offset.Y",L"Size.X",L"Size.Y"};
        for (int i=0; i<4; ++i) {
            // Encode the evaluated compositor value to 24 bits, at 1/16 DIP.
            // A capture of these 16 offscreen pixels observes native implicit
            // motion; XAML getters only expose its layout destination.
            std::wstring value = L"Floor((dock." + std::wstring(properties[i]) + L" + 32768) * 16)";
            auto expression = compositor.CreateExpressionAnimation(
                L"ColorRGB(255, Floor(" + value + L" / 65536), Floor(" + value +
                L" / 256) - Floor(" + value + L" / 65536) * 256, " + value +
                L" - Floor(" + value + L" / 256) * 256)");
            expression.SetReferenceParameter(L"dock",geometry);
            dockSamples[i].StartAnimation(L"Color",expression);
        }
        // Captured directly, never attached to the displayed visual tree.
        EnsureGeometryCapture();
    }

    void TrackLens(size_t index, Visual const& button, Visual const& anchor,
                    FrameworkElement const& plate,
                    CompositionRoundedRectangleGeometry const& dock,
                    float nativeCenter, float clipCenter) {
        if (!geometryVisual || index >= kMaxButtonLenses) return;
        std::lock_guard lock(mutex);
        auto& probe = lensProbes[index];
        if (probe.button == button && probe.anchor == anchor && probe.plate == plate && probe.dock == dock &&
            probe.nativeCenter == nativeCenter && probe.clipCenter == clipCenter) return;
        if (probe.row) geometryVisual.Children().Remove(probe.row);
        auto compositor = geometryVisual.Compositor();
        probe = {};
        probe.button = button; probe.anchor = anchor;
        probe.plate = plate; probe.dock = dock;
        probe.nativeCenter = nativeCenter; probe.clipCenter = clipCenter;
        probe.tag = (++nextLensTag & 0xffffffu);
        if (!probe.tag) probe.tag = (++nextLensTag & 0xffffffu);
        probe.values = compositor.CreatePropertySet();
        probe.values.InsertScalar(L"nativeCenter",nativeCenter);
        probe.values.InsertScalar(L"clipCenter",clipCenter);
        probe.values.InsertScalar(L"padding",float(kDockPadding));
        probe.row = compositor.CreateContainerVisual();
        probe.row.Size({8,2});
        probe.row.Offset({0,float(2*(index+1)),0});
        for (int component=0;component<2;++component) {
            auto visual = compositor.CreateSpriteVisual();
            visual.Size({2,2}); visual.Offset({float(component*2),0,0});
            auto brush = compositor.CreateColorBrush();
            std::wstring value = L"Floor((dock.Offset.X + local.padding + button.Offset.X - anchor.Offset.X + local." +
                std::wstring(component ? L"clipCenter" : L"nativeCenter") + L" + 32768) * 16)";
            auto expression = compositor.CreateExpressionAnimation(
                L"ColorRGB(255, Floor("+value+L"/65536), Floor("+value+L"/256)-Floor("+value+
                L"/65536)*256, "+value+L"-Floor("+value+L"/256)*256)");
            expression.SetReferenceParameter(L"dock",dock);
            expression.SetReferenceParameter(L"button",button);
            expression.SetReferenceParameter(L"anchor",anchor);
            expression.SetReferenceParameter(L"local",probe.values);
            brush.StartAnimation(L"Color",expression);
            visual.Brush(brush);
            probe.row.Children().InsertAtTop(visual);
        }
        auto tag = compositor.CreateSpriteVisual();
        tag.Size({2,2}); tag.Offset({4,0,0});
        tag.Brush(compositor.CreateColorBrush({255,BYTE(probe.tag>>16),BYTE(probe.tag>>8),BYTE(probe.tag)}));
        probe.row.Children().InsertAtTop(tag);
        // The last two pixels already fit in the geometry row. Refer to the
        // XAML facade used by native StartAnimation, plus its button visual,
        // so compositor fades are sampled without UI-thread polling.
        auto opacity = compositor.CreateSpriteVisual();
        opacity.Size({2,2}); opacity.Offset({6,0,0});
        auto opacityBrush = compositor.CreateColorBrush();
        auto opacityExpression = compositor.CreateExpressionAnimation(
            L"ColorRGB(255, Floor(Clamp(plate.Opacity * button.Opacity, 0, 1) * 255 + 0.5), 0, 0)");
        opacityExpression.SetExpressionReferenceParameter(L"plate",plate.as<IAnimationObject>());
        opacityExpression.SetReferenceParameter(L"button",button);
        opacityBrush.StartAnimation(L"Color",opacityExpression);
        opacity.Brush(opacityBrush);
        probe.row.Children().InsertAtTop(opacity);
        geometryVisual.Children().InsertAtTop(probe.row);
        sampledLensTags[index] = 0;
    }

    void TrimLensProbes(size_t count) {
        if (!geometryVisual) return;
        std::lock_guard lock(mutex);
        for (size_t i=count;i<kMaxButtonLenses;++i) {
            if (lensProbes[i].row) geometryVisual.Children().Remove(lensProbes[i].row);
            lensProbes[i] = {};
            sampledLensTags[i] = 0;
        }
    }

    void EnsureGeometryCapture(bool restart = false) {
        if (!geometryVisual) return;
        std::shared_ptr<Source> old;
        {
            std::lock_guard lock(mutex);
            if (!running || (geometrySource && !geometrySource->closed && !restart)) return;
            old = std::exchange(geometrySource, nullptr);
            haveCompositorBounds = false;
            geometryReadback = nullptr;
        }
        RetireSources({std::move(old)});
        try {
            auto item = capture::GraphicsCaptureItem::CreateFromVisual(geometryVisual);
            auto source = CreateCaptureSource(item, nullptr, {});
            if (!source) return;
            bool accepted{};
            {
                std::lock_guard lock(mutex);
                if (running) {
                    geometrySource = source;
                    source->active = true;
                    accepted = true;
                }
            }
            if (!accepted) { RetireSources({source}); return; }
            if (!StartSource(source)) source->closed = true;
        } catch (...) {
            Wh_Log(L"[UWLIQUID] compositor sampling unavailable hr=%08X", winrt::to_hresult().value);
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
        renderedBodyView = nullptr;
        renderedBodyTarget = nullptr;
        renderedBody = nullptr;
        blurWidth = 0;
        blurHeight = 0;
        for (int i = 0; i < 3; ++i) {
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
        winrt::com_ptr<ID3D11Texture2D> newBlurTextures[3];
        winrt::com_ptr<ID3D11RenderTargetView> newBlurTargets[3];
        winrt::com_ptr<ID3D11ShaderResourceView> newBlurViews[3];
        for (int i = 0; i < 3; ++i) {
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
        D3D11_TEXTURE2D_DESC bodyDescription{};
        composite->GetDesc(&bodyDescription);
        if (FAILED(device->CreateTexture2D(&bodyDescription,nullptr,renderedBody.put())) ||
            FAILED(device->CreateRenderTargetView(renderedBody.get(),nullptr,renderedBodyTarget.put())) ||
            FAILED(device->CreateShaderResourceView(renderedBody.get(),nullptr,renderedBodyView.put())))
            return false;
        for (int i = 0; i < 3; ++i) {
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

    void SetPreviewBackdrop(std::span<const uint32_t> pixels) {
        std::lock_guard lock(mutex);
        if (taskbar || pixels.size() != size_t(width) * height)
            throw winrt::hresult_invalid_argument();
        ++sourceRevision;
        sourceUpdatePending=false;
        requestedSources.clear();
        auto source = std::make_shared<Source>();
        source->rect = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
        source->latestTexels = source->rect;
        source->contentSize = source->textureSize = {
            static_cast<int32_t>(width), static_cast<int32_t>(height)};
        D3D11_TEXTURE2D_DESC description{};
        description.Width = width;
        description.Height = height;
        description.MipLevels = description.ArraySize = description.SampleDesc.Count = 1;
        description.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        description.Usage = D3D11_USAGE_IMMUTABLE;
        description.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        D3D11_SUBRESOURCE_DATA data{pixels.data(), width * 4, 0};
        winrt::check_hresult(device->CreateTexture2D(&description, &data, source->latest.put()));
        winrt::check_hresult(device->CreateShaderResourceView(
            source->latest.get(), nullptr, source->latestView.put()));
        sources = {source};
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

    void UpdateDisplayCadenceLocked() {
        if (!limitToDisplay) { presentInterval={}; return; }
        auto window=presentationWindow ? presentationWindow : taskbar;
        if (!window) return;
        auto now = std::chrono::steady_clock::now();
        auto monitor = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
        if (monitor == presentMonitor && now < nextDisplayProbe) return;
        nextDisplayProbe = now + std::chrono::seconds(1);
        MONITORINFOEXW info{}; info.cbSize = sizeof(info);
        DEVMODEW mode{}; mode.dmSize = sizeof(mode);
        if (GetMonitorInfoW(monitor, reinterpret_cast<MONITORINFO*>(&info)) &&
            EnumDisplaySettingsW(info.szDevice, ENUM_CURRENT_SETTINGS, &mode) &&
            mode.dmDisplayFrequency > 1) {
            auto hz = static_cast<LONGLONG>(mode.dmDisplayFrequency);
            presentInterval = std::chrono::nanoseconds((1000000000LL + hz - 1) / hz);
            presentMonitor = monitor;
        }
    }

    bool UpdateGeometry(RECT const& taskbarBounds, UINT pixelWidth,
                        UINT pixelHeight, LensConstants const& newLens,
                        float dipScale = 1) {
        std::lock_guard lock(mutex);
        UpdateDisplayCadenceLocked();
        if (!running || FAILED(failure) ||
            !ResizeLocked(pixelWidth, pixelHeight))
            return false;
        auto now = std::chrono::steady_clock::now();
        BOOL allowEffects = FALSE;
        SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &allowEffects, 0);
        bool preferenceChanged = allowEffects != effectsAllowed;
        effectsAllowed = allowEffects;
        preferenceAt = now;
        bool changed = !geometryReady ||
                       preferenceChanged ||
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
        for (auto const& source : sources)
            if (source->visualBackdrop) source->rect = taskbarBounds;
        geometryScale = dipScale;
        lens = newLens;
        geometryReady = true;
        if (changed || pendingPresent) SchedulePresentLocked();
        return true;
    }

    bool UpdatePosition(POINT origin) {
        std::lock_guard lock(mutex);
        if(!running || FAILED(failure) || !geometryReady) return false;
        if(taskbarRect.left==origin.x && taskbarRect.top==origin.y) return true;
        OffsetRect(&taskbarRect,origin.x-taskbarRect.left,origin.y-taskbarRect.top);
        for(auto const& source:sources) if(source->visualBackdrop) source->rect=taskbarRect;
        SchedulePresentLocked();
        return true;
    }

    void SetPointer(float x, float y, bool active) noexcept {
        std::lock_guard lock(mutex);
        if (!running || (lens.optics.fluidStrength <= 0 && lens.optics.glintMode != 2)) return;
        if (!std::isfinite(x) || !std::isfinite(y)) { x = y = 0; active = false; }
        auto now = std::chrono::steady_clock::now();
        if (!effectsAllowed && now - preferenceAt < std::chrono::seconds(1)) return;
        float dt = std::chrono::duration<float, std::milli>(now - pointerAt).count();
        bool continuous = active && pointerActive && dt > 0 && dt < 100;
        velocityX = continuous ? (x * width - pointerX) / dt : 0;
        velocityY = continuous ? (y * height - pointerY) / dt : 0;
        pointerX = x * width; pointerY = y * height;
        if (active && !pointerActive) { fluidX = pointerX; fluidY = pointerY; }
        pointerActive = active; pointerAt = now;
        SchedulePresentLocked();
    }

    void UpdateEffectsLocked(LensConstants& output) {
        auto now = std::chrono::steady_clock::now();
        if (now - preferenceAt > std::chrono::seconds(1)) {
            effectsAllowed = FALSE;
            SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &effectsAllowed, 0);
            preferenceAt = now;
        }
        float dt = effectAt.time_since_epoch().count() ?
            std::min(50.0f, std::chrono::duration<float, std::milli>(now - effectAt).count()) : 16;
        effectAt = now;
        auto const& settings = output.optics;
        effectsAnimating = false;
        if (!effectsAllowed || (settings.fluidStrength <= 0 && settings.glintMode <= 0)) {
            fluidDx = fluidDy = 0;
            return;
        }
        effectSeconds = std::fmod(effectSeconds + dt * .001 * settings.glintSpeed, 62.8318530718);
        float follow = 1 - std::exp(-dt / (25 + settings.fluidViscosity * 180));
        float settle = 1 - std::exp(-dt / (80 + settings.fluidViscosity * 620));
        fluidX += (pointerX - fluidX) * follow;
        fluidY += (pointerY - fluidY) * follow;
        float age = std::chrono::duration<float, std::milli>(now - pointerAt).count();
        float speed = std::hypot(velocityX, velocityY);
        float amount = pointerActive && speed > 0 && age < 1000 ?
            std::min(std::min(width, height) * .12f, speed * 45) *
                settings.fluidStrength * std::exp(-age / 70) : 0;
        float dx = speed > 0 ? velocityX / speed * amount : 0;
        float dy = speed > 0 ? velocityY / speed * amount : 0;
        float easing = amount > std::hypot(fluidDx, fluidDy) ? follow : settle;
        fluidDx += (dx - fluidDx) * easing; fluidDy += (dy - fluidDy) * easing;
        if (settings.fluidStrength <= 0 || (amount < .01f && std::hypot(fluidDx, fluidDy) < .01f))
            fluidDx = fluidDy = 0;
        output.fluidPointer[0] = settings.fluidStrength > 0 ? fluidX : pointerX;
        output.fluidPointer[1] = settings.fluidStrength > 0 ? fluidY : pointerY;
        output.fluidPointer[2] = fluidDx; output.fluidPointer[3] = fluidDy;
        output.glintClock[0] = float(effectSeconds);
        output.glintClock[1] = pointerActive ? 1 : 0;
        output.glintClock[2] = 1;
        effectsAnimating = (settings.glintMode == 1 && settings.glintStrength > 0) ||
            fluidDx != 0 || fluidDy != 0;
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
    // presentation. DXGI can use the fastest desktop output; the high-resolution
    // timer additionally caps work to the HWND's own monitor without holding mutex.
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
            RECT rect{}, visible{}, intersection{};
            if (!GetWindowRect(window, &rect)) continue;
            // The window rect includes the invisible resize border, so a
            // maximized or snapped window next to the taskbar would qualify
            // and paint its edge rows into the strip. Qualify by the visible
            // frame; the texture is still placed on the window rect.
            if (FAILED(DwmGetWindowAttribute(window, DWMWA_EXTENDED_FRAME_BOUNDS,
                                             &visible, sizeof(visible))))
                visible = rect;
            if (!IntersectRect(&intersection, &visible, &dockStrip)) continue;
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
            return CreateCaptureSource(capture::GraphicsCaptureItem{
                rawItem, winrt::take_ownership_from_abi}, window, rect);
        } catch (...) {
            Wh_Log(L"[UWLIQUID] capture item failed hwnd=%p hr=%08X", window, winrt::to_hresult().value);
            return nullptr;
        }
    }

    std::shared_ptr<Source> CreateCaptureSource(capture::GraphicsCaptureItem const& item,
                                               HWND window, RECT const& rect, bool visualBackdrop = false) {
        try {
            auto source = std::make_shared<Source>();
            source->window = window;
            source->visualBackdrop = visualBackdrop;
            source->rect = rect;
            source->item = item;
            source->poolSize = source->item.Size();
            if (source->poolSize.Width <= 0 || source->poolSize.Height <= 0)
                return nullptr;
            source->pool =
                capture::Direct3D11CaptureFramePool::CreateFreeThreaded(
                    graphicsDevice,
                    directx::DirectXPixelFormat::B8G8R8A8UIntNormalized,
                    kPoolBuffers, source->poolSize);
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
                        if (source->window || source->visualBackdrop) renderer->OnFrame(source, sender);
                        else renderer->OnGeometryFrame(source, sender);
                    }
                });
            source->closedToken = source->item.Closed(
                [weakRenderer, weakSource](auto const&, auto const&) {
                    ActiveScope active;
                    auto source = weakSource.lock();
                    auto renderer = weakRenderer.lock();
                    if (!source || !renderer) return;
                    source->closed = true;
                    source->active = false;
                    std::lock_guard lock(renderer->mutex);
                    if (renderer->running) renderer->SchedulePresentLocked();
                });
            try { source->session.DirtyRegionMode(capture::GraphicsCaptureDirtyRegionMode::ReportOnly); } catch (...) {}
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
        std::lock_guard lifecycleLock(source->lifecycleMutex);
        source->active.store(false, std::memory_order_release);
        // Drain an in-flight copy, but do not hold this lock while WinRT closes
        // the pool: a queued callback must be able to enter and observe inactive.
        { std::lock_guard callbackLock(source->callbackMutex); }
        // A source may be closed twice (explicit teardown, then scene
        // removal); WinRT closing an already-closed pool is not safe to repeat.
        auto session = std::exchange(source->session, nullptr);
        auto pool = std::exchange(source->pool, nullptr);
        try { if (source->item) source->item.Closed(source->closedToken); } catch (...) {}
        try {
            if (pool) pool.FrameArrived(source->frameToken);
        } catch (...) {}
        try { if (session) session.Close(); } catch (...) {}
        try { if (pool) pool.Close(); } catch (...) {}
    }

    bool StartSource(std::shared_ptr<Source> const& source) {
        std::lock_guard lifecycleLock(source->lifecycleMutex);
        if (!source->session || !source->active.load(std::memory_order_acquire)) return false;
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

    // A parked pool is full, which is what stopped delivery; reading it restarts
    // delivery. Every queued frame is stored, because a fresh frame can land in
    // the first freed buffer, and a window that then stays static sends no other.
    void ResumeSource(std::shared_ptr<Source> const& source) {
        std::lock_guard lifecycleLock(source->lifecycleMutex);
        std::lock_guard callbackLock(source->callbackMutex);
        if (!source->pool || !source->active.load(std::memory_order_acquire)) return;
        for (int32_t i = 0; i < kPoolBuffers; ++i) OnFrame(source, source->pool);
    }

    void SetBodyVisual(Visual const& visual) {
        DWORD compositor = CompositorProcessId();
        std::vector<std::shared_ptr<Source>> old;
        {
            std::lock_guard lock(mutex);
            bool restart = compositor && compositorProcess && compositor != compositorProcess;
            if (bodyVisual == visual && !restart &&
                (!visual || (sources.size()==1 && sources[0]->active && !sources[0]->closed))) {
                if(compositor) compositorProcess=compositor;
                return;
            }
            ++sourceRevision;
            sourceUpdatePending=false;
            requestedSources.clear();
            bodyVisual = visual;
            old = std::move(sources);
            old.insert(old.end(), parked.begin(), parked.end());
            parked.clear();
            if (compositor) compositorProcess = compositor;
        }
        RetireSources(std::move(old));
        if (!visual) return;
        auto item = capture::GraphicsCaptureItem::CreateFromVisual(visual);
        auto source = CreateCaptureSource(item,nullptr,{},true);
        if (!source) throw winrt::hresult_error(E_FAIL,L"Native body capture unavailable");
        bool accepted{};
        {
            std::lock_guard lock(mutex);
            if (running) {
                source->rect = taskbarRect;
                sources = {source};
                source->active = true;
                accepted = true;
            }
        }
        if (!accepted) { RetireSources({source}); return; }
        if (!StartSource(source)) source->closed = true;
    }

    void UpdateSources() {
        if (bodyVisual) { SetBodyVisual(bodyVisual); return; }
        bool editorBody{};
        { std::lock_guard lock(mutex); editorBody = lens.bodyMode != 0; }
        if (editorBody) { UpdateSources({}); return; }
        UpdateSources(CandidateWindows());
    }

    void UpdateSources(
        std::vector<std::pair<HWND, RECT>> const& candidates) {
        std::lock_guard lock(mutex);
        if (!running || FAILED(failure)) return;
        // Repeated polling/movement must not starve a capture still starting.
        // A different set/order of windows supersedes the in-flight request.
        if (sourceWorkPending && requestedSources.size()==candidates.size() &&
            std::equal(requestedSources.begin(),requestedSources.end(),candidates.begin(),
                [](auto const& a,auto const& b) { return a.first==b.first; })) {
            if (!std::equal(requestedSources.begin(),requestedSources.end(),candidates.begin(),
                [](auto const& a,auto const& b) { return EqualRect(&a.second,&b.second); })) {
                requestedSources=candidates;
                sourceUpdatePending=true;
            }
            return;
        }
        requestedSources=candidates;
        ++sourceRevision;
        sourceUpdatePending=true;
        if (!sourceWork) {
            // Like presentWork, the raw owner stays alive until Stop drains it.
            sourceWork=CreateThreadpoolWork([](PTP_CALLBACK_INSTANCE,void* raw,PTP_WORK) {
                ActiveScope active;
                auto renderer=static_cast<LiquidRenderer*>(raw);
                try {
                    winrt::init_apartment(winrt::apartment_type::multi_threaded);
                    struct Uninit { ~Uninit() { winrt::uninit_apartment(); } } uninit;
                    for (;;) {
                        std::vector<std::pair<HWND,RECT>> candidates;
                        uint64_t revision{};
                        {
                            std::lock_guard lock(renderer->mutex);
                            if (!renderer->running || FAILED(renderer->failure) || !renderer->sourceUpdatePending) {
                                renderer->sourceWorkPending=false;
                                return;
                            }
                            candidates=renderer->requestedSources;
                            revision=renderer->sourceRevision;
                            renderer->sourceUpdatePending=false;
                        }
                        try { renderer->ReconcileSources(candidates,revision); }
                        catch (...) {
                            auto error=winrt::to_hresult().value;
                            std::lock_guard lock(renderer->mutex);
                            if (renderer->running && revision==renderer->sourceRevision)
                                renderer->failure=error;
                        }
                    }
                } catch (...) {
                    auto error=winrt::to_hresult().value;
                    std::lock_guard lock(renderer->mutex);
                    if (renderer->running) renderer->failure=error;
                    renderer->sourceWorkPending=false;
                }
            },this,nullptr);
            if (!sourceWork) {
                DWORD error=GetLastError();
                failure=error?HRESULT_FROM_WIN32(error):E_FAIL;
                sourceUpdatePending=false;
                return;
            }
        }
        if (!sourceWorkPending) {
            sourceWorkPending=true;
            SubmitThreadpoolWork(sourceWork);
        }
    }

    void ReconcileSources(
        std::vector<std::pair<HWND,RECT>> const& candidates,uint64_t revision) {
        DWORD compositor = CompositorProcessId();
        bool restarted{};
        std::vector<std::shared_ptr<Source>> current, idle, stale;
        {
            std::lock_guard lock(mutex);
            if (!running || revision!=sourceRevision) return;
            UpdateDisplayCadenceLocked();
            current = sources;
            idle = parked;
            restarted = compositor && compositorProcess && compositor != compositorProcess;
            if (restarted)
                Wh_Log(L"[UWLIQUID] compositor changed %lu -> %lu; replacing %zu capture sessions",
                       compositorProcess, compositor, sources.size());
            if (compositor) compositorProcess = compositor;
            auto now = std::chrono::steady_clock::now();
            for (auto const& source : sources)
                if (restarted || source->closed || StaleLocked(*source, now))
                    stale.push_back(source);
        }
        EnsureGeometryCapture(restarted);
        std::vector<std::shared_ptr<Source>> next;
        std::vector<std::shared_ptr<Source>> added, resumed;
        std::vector<RECT> nextRects;
        for (auto const& [window, rect] : candidates) {
            auto byWindow = [&](auto const& source) { return source->window == window; };
            auto found = std::find_if(current.begin(), current.end(), byWindow);
            auto kept = std::find_if(idle.begin(), idle.end(), byWindow);
            if (found != current.end() &&
                std::find(stale.begin(), stale.end(), *found) == stale.end()) {
                next.push_back(*found);
                nextRects.push_back(rect);
            } else if (!restarted && kept != idle.end() && !(*kept)->closed) {
                next.push_back(*kept);
                nextRects.push_back(rect);
                resumed.push_back(*kept);
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
            if (running && revision==sourceRevision && sources == current && parked == idle) {
                bool changed = sources.size() != next.size();
                for (size_t i = 0; i < next.size(); ++i) {
                    if (i >= sources.size() || sources[i] != next[i] ||
                        !EqualRect(&next[i]->rect, &nextRects[i]))
                        changed = true;
                    next[i]->rect = nextRects[i];
                }
                for (auto const& source : resumed) {
                    std::erase(parked, source);
                    source->active.store(true, std::memory_order_release);
                }
                // A closed capture session leaves one frame of its size in the
                // capture runtime until Explorer exits, so a window that only
                // left the strip keeps its session parked.
                for (auto const& source : sources) {
                    if (std::find(next.begin(), next.end(), source) != next.end()) continue;
                    if (std::find(stale.begin(), stale.end(), source) == stale.end() &&
                        !source->closed && source->window && IsWindow(source->window)) {
                        source->active.store(false, std::memory_order_release);
                        parked.push_back(source);
                    } else removed.push_back(source);
                }
                sources = next;
                for (auto const& source : added)
                    source->active.store(true, std::memory_order_release);
                std::erase_if(parked, [&](auto const& source) {
                    bool evict = restarted || source->closed || !IsWindow(source->window);
                    if (evict) removed.push_back(source);
                    return evict;
                });
                while (parked.size() > kMaxAppSources) {
                    removed.push_back(parked.front());
                    parked.erase(parked.begin());
                }
                if (changed) SchedulePresentLocked();
                committed = true;
            }
        }
        if (!committed) {
            for (auto const& source : added) CloseSource(source);
            return;
        }
        // A removed source's window may already be destroyed, and the capture
        // server can leave that StopCapture unanswered (issue 16).
        RetireSources(std::move(removed));
        for (auto const& source : resumed) ResumeSource(source);
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

    void OnGeometryFrame(std::shared_ptr<Source> const& source,
                         capture::Direct3D11CaptureFramePool const& sender) noexcept {
        capture::Direct3D11CaptureFrame frame{nullptr};
        try {
            frame = sender.TryGetNextFrame();
            if (!frame) return;
            auto surface = frame.Surface();
            winrt::com_ptr<IDirect3DDxgiInterfaceAccess> access;
            winrt::check_hresult(reinterpret_cast<IUnknown*>(winrt::get_abi(surface))->QueryInterface(
                kIID_IDirect3DDxgiInterfaceAccess, access.put_void()));
            winrt::com_ptr<ID3D11Texture2D> texture;
            winrt::check_hresult(access->GetInterface(__uuidof(ID3D11Texture2D), texture.put_void()));
            D3D11_TEXTURE2D_DESC desc{}; texture->GetDesc(&desc);
            if (desc.Width < 8 || desc.Height < 2) { frame.Close(); return; }
            std::unique_lock lock(mutex);
            if (!running || geometrySource != source || !source->active) { frame.Close(); return; }
            if (!geometryReadback) {
                desc.Usage = D3D11_USAGE_STAGING;
                desc.BindFlags = desc.MiscFlags = 0;
                desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
                winrt::check_hresult(device->CreateTexture2D(&desc, nullptr, geometryReadback.put()));
            }
            if (!source->readbackReady) {
                source->readbackReady.attach(CreateEventW(nullptr, FALSE, FALSE, nullptr));
                if (!source->readbackReady) winrt::throw_last_error();
            }
            context->CopyResource(geometryReadback.get(), texture.get());
            // The source callback lock serializes readbacks and teardown. Let
            // layout, pointer updates and presentation proceed while the GPU
            // finishes this copy; never wait for it with the renderer lock held.
            D3D11_MAPPED_SUBRESOURCE map{};
            auto completion = context.as<ID3D11DeviceContext3>();
            for (;;) {
                completion->Flush1(D3D11_CONTEXT_TYPE_ALL, source->readbackReady.get());
                lock.unlock();
                DWORD waited = WaitForSingleObject(source->readbackReady.get(), 1000);
                if (waited == WAIT_FAILED) winrt::throw_last_error();
                if (waited != WAIT_OBJECT_0) winrt::throw_hresult(HRESULT_FROM_WIN32(ERROR_TIMEOUT));
                lock.lock();
                if (!running || geometrySource != source || !source->active) { frame.Close(); return; }
                HRESULT mapped = context->Map(
                    geometryReadback.get(), 0, D3D11_MAP_READ, D3D11_MAP_FLAG_DO_NOT_WAIT, &map);
                // Some drivers defer staging work until the first Map. Submit
                // that work and wait outside the lock again if it is not ready.
                if (mapped == DXGI_ERROR_WAS_STILL_DRAWING) continue;
                winrt::check_hresult(mapped);
                break;
            }
            auto colors = static_cast<uint32_t*>(map.pData);
            float bounds[4]{};
            for (int i=0; i<4; ++i) bounds[i] = float(colors[i*2] & 0x00FFFFFFu) / 16 - 32768;
            bool changed = !haveCompositorBounds || !std::equal(bounds, bounds+4, compositorBounds);
            for (size_t i=0;i<kMaxButtonLenses && 2*(i+1)<desc.Height;++i) {
                auto row = reinterpret_cast<uint32_t*>(
                    static_cast<BYTE*>(map.pData) + map.RowPitch*2*(i+1));
                float nativeX = float(row[0]&0xffffffu)/16-32768;
                float clipX = float(row[2]&0xffffffu)/16-32768;
                uint32_t tag = row[4]&0xffffffu;
                float opacity = float((row[6]>>16)&255)/255;
                if (i < lens.lensCount && lensProbes[i].tag &&
                    (tag == lensProbes[i].tag || sampledLensTags[i] == lensProbes[i].tag))
                    changed |= nativeX != sampledLensX[i][0] || clipX != sampledLensX[i][1] ||
                        tag != sampledLensTags[i] || opacity != sampledLensOpacity[i];
                sampledLensX[i][0] = nativeX;
                sampledLensX[i][1] = clipX;
                sampledLensTags[i] = tag;
                sampledLensOpacity[i] = opacity;
            }
            context->Unmap(geometryReadback.get(), 0);
            if (bounds[2] > 0 && bounds[3] > 0) {
                std::copy_n(bounds, 4, compositorBounds);
                haveCompositorBounds = true;
                if (changed) SchedulePresentLocked();
            }
        } catch (...) {
            source->closed = true;
            Wh_Log(L"[UWLIQUID] compositor frame failed hr=%08X", winrt::to_hresult().value);
        }
        try { if (frame) frame.Close(); } catch (...) {}
    }

    bool StripSourceLocked(Source const& source) const {
        return lens.bodyMode == 0 && source.window && source.window == GetShellWindow();
    }

    RECT NeededTexelsLocked(Source const& source) const {
        RECT shown{}, texels{};
        if (!IntersectRect(&shown, &source.rect, &taskbarRect)) return texels;
        LONGLONG sourceWidth = source.rect.right - source.rect.left;
        LONGLONG sourceHeight = source.rect.bottom - source.rect.top;
        auto floorDiv = [](LONGLONG n, LONGLONG d) { return LONG(n / d); };
        auto ceilDiv = [](LONGLONG n, LONGLONG d) { return LONG((n + d - 1) / d); };
        constexpr LONG bilinearGuard = 1;
        RECT needed{
            floorDiv(LONGLONG(shown.left - source.rect.left) * source.contentSize.Width, sourceWidth) - bilinearGuard,
            floorDiv(LONGLONG(shown.top - source.rect.top) * source.contentSize.Height, sourceHeight) - bilinearGuard,
            ceilDiv(LONGLONG(shown.right - source.rect.left) * source.contentSize.Width, sourceWidth) + bilinearGuard,
            ceilDiv(LONGLONG(shown.bottom - source.rect.top) * source.contentSize.Height, sourceHeight) + bilinearGuard};
        RECT valid{0, 0, source.textureSize.Width, source.textureSize.Height};
        IntersectRect(&texels, &needed, &valid);
        return texels;
    }

    // A frame can report a grown ContentSize on a surface still sized for the
    // old pool; Recreate only helps if another frame follows. A static window
    // sends none, so recycle the session instead.
    bool StaleLocked(Source const& source, std::chrono::steady_clock::time_point now) const {
        if (!source.latest || now - source.lastFrame <= std::chrono::milliseconds(100)) return false;
        return source.textureSize.Width != source.contentSize.Width ||
               source.textureSize.Height != source.contentSize.Height ||
               StripMissesTaskbarLocked(source);
    }

    bool StripMissesTaskbarLocked(Source const& source) const {
        RECT needed = NeededTexelsLocked(source), covered{};
        return !IsRectEmpty(&needed) &&
               !(IntersectRect(&covered, &needed, &source.latestTexels) && EqualRect(&covered, &needed));
    }

    bool StoreFrameLocked(std::shared_ptr<Source> const& source, ID3D11Texture2D* texture,
                          decltype(Source::contentSize) contentSize, RECT const* currentRect) {
        D3D11_TEXTURE2D_DESC captured{};
        texture->GetDesc(&captured);
        UINT validWidth = std::min<UINT>(contentSize.Width, captured.Width);
        UINT validHeight = std::min<UINT>(contentSize.Height, captured.Height);
        if (!validWidth || !validHeight) return false;
        source->contentSize = contentSize;
        source->textureSize = {static_cast<int32_t>(validWidth),
                               static_cast<int32_t>(validHeight)};
        if (currentRect) source->rect = *currentRect;
        RECT copied{0, 0, LONG(validWidth), LONG(validHeight)};
        if (StripSourceLocked(*source)) {
            RECT strip = NeededTexelsLocked(*source);
            if (!IsRectEmpty(&strip)) copied = strip;
        }
        UINT copyWidth = copied.right - copied.left;
        UINT copyHeight = copied.bottom - copied.top;
        D3D11_TEXTURE2D_DESC latest{};
        if (source->latest) source->latest->GetDesc(&latest);
        if (!source->latest || !source->latestView ||
            latest.Width != copyWidth || latest.Height != copyHeight ||
            latest.Format != captured.Format) {
            D3D11_TEXTURE2D_DESC owned = captured;
            owned.Width = copyWidth;
            owned.Height = copyHeight;
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
        }
        // WGC can drop frames. DirtyRegions describes only the current
        // update, so applying deltas leaves permanent stale pixels when an
        // earlier update was missed. ReportOnly surfaces contain the full
        // image; replace our snapshot on every delivered frame.
        D3D11_BOX box{UINT(copied.left), UINT(copied.top), 0,
                      UINT(copied.right), UINT(copied.bottom), 1};
        context->CopySubresourceRegion(
            source->latest.get(), 0, 0, 0, 0, texture, 0, &box);
        source->latestTexels = copied;
        source->lastFrame = std::chrono::steady_clock::now();
        return true;
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
            RECT currentRect{};
            // Free-threaded WGC callbacks inherit the process DPI default, not
            // the UI thread's context. Textures and compositor bounds are in
            // physical pixels; never overwrite them with virtualized rectangles.
            auto previousDpi = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
            bool haveRect = source->window && previousDpi && GetWindowRect(source->window, &currentRect);
            if (previousDpi) SetThreadDpiAwarenessContext(previousDpi);
            std::lock_guard lock(mutex);
            if (!running ||
                !source->active.load(std::memory_order_acquire) ||
                std::find(sources.begin(), sources.end(), source) == sources.end())
                return;
            if (!StoreFrameLocked(source, texture.get(), contentSize,
                                  haveRect ? &currentRect : nullptr))
                return;
            if (source->poolSize.Width != contentSize.Width ||
                source->poolSize.Height != contentSize.Height) {
                recreateSize = contentSize;
                recreatePool = true;
            }
            SchedulePresentLocked();
        } catch (...) {
            HRESULT error = winrt::to_hresult().value;
            HRESULT removed = device ? device->GetDeviceRemovedReason() : S_OK;
            {
                std::lock_guard lock(mutex);
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
                    kPoolBuffers, recreateSize);
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
        float native[]{lens.bodyMaterialTint[0],lens.bodyMaterialTint[1],lens.bodyMaterialTint[2],1};
        context->ClearRenderTargetView(compositeTarget.get(), lens.bodyMode != 0 ? native : clear);
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
            if (source.closed || !source.latestView || source.contentSize.Width <= 0 ||
                source.contentSize.Height <= 0) continue;
            int sourceWidth = source.rect.right - source.rect.left;
            int sourceHeight = source.rect.bottom - source.rect.top;
            if (sourceWidth <= 0 || sourceHeight <= 0) continue;
            RECT const& copied = source.latestTexels;
            RECT validRect{
                source.rect.left + MulDiv(copied.left, sourceWidth, source.contentSize.Width),
                source.rect.top + MulDiv(copied.top, sourceHeight, source.contentSize.Height),
                source.rect.left + MulDiv(copied.right, sourceWidth, source.contentSize.Width),
                source.rect.top + MulDiv(copied.bottom, sourceHeight, source.contentSize.Height)};
            RECT intersection{};
            if (!IntersectRect(&intersection, &validRect, &taskbarRect)) continue;
            // A growing item can exceed its old pool for one frame. Clip the
            // unavailable area instead of stretching the valid subrectangle.
            int copiedWidth = copied.right - copied.left;
            int copiedHeight = copied.bottom - copied.top;
            float inverseWidth = float(source.contentSize.Width) /
                                 (sourceWidth * float(copiedWidth));
            float inverseHeight = float(source.contentSize.Height) /
                                  (sourceHeight * float(copiedHeight));
            CopyConstants copy{};
            copy.offset[0] = (intersection.left - source.rect.left) * inverseWidth -
                             float(copied.left) / copiedWidth;
            copy.offset[1] = (intersection.top - source.rect.top) * inverseHeight -
                             float(copied.top) / copiedHeight;
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

        auto drawGlass = [&](ID3D11RenderTargetView* target, LensConstants parameters,
                              ID3D11ShaderResourceView* sharp,
                              ID3D11ShaderResourceView* blurred,
                              ID3D11ShaderResourceView* body) {
            parameters.imageSize[0]=float(width); parameters.imageSize[1]=float(height);
            parameters.compositorClip=taskbar!=nullptr;
            context->UpdateSubresource(constants.get(),0,nullptr,&parameters,0,0);
            auto buffer=constants.get();
            D3D11_VIEWPORT viewport{0,0,float(width),float(height),0,1};
            ID3D11ShaderResourceView* inputs[]{sharp,blurred,body};
            context->OMSetRenderTargets(1,&target,nullptr);
            context->RSSetViewports(1,&viewport);
            context->VSSetShader(vertexShader.get(),nullptr,0);
            context->PSSetShader(pixelShader.get(),nullptr,0);
            context->PSSetShaderResources(0,3,inputs);
            context->PSSetSamplers(0,1,&sample);
            context->PSSetConstantBuffers(0,1,&buffer);
            context->Draw(3,0);
            ID3D11ShaderResourceView* empty[3]{};
            context->PSSetShaderResources(0,3,empty);
            context->OMSetRenderTargets(0,nullptr,nullptr);
        };
        auto blurBackdrop = [&](ID3D11ShaderResourceView* input, float strength, int resultIndex) -> ID3D11ShaderResourceView* {
            if (strength <= 0.001f) return input;
            // GPU adaptation of bfraboni/FastGaussianBlur (MIT): approximate a
            // Gaussian with three horizontal and three vertical box passes.
            // https://github.com/bfraboni/FastGaussianBlur
            int radii[3];
            FastGaussianBoxRadii(strength * 12, radii);
            context->PSSetShader(boxBlurPixelShader.get(), nullptr, 0);
            auto blurConstantBuffer = blurConstants.get();
            context->PSSetConstantBuffers(2, 1, &blurConstantBuffer);
            D3D11_VIEWPORT blurViewport{
                0, 0, static_cast<float>(blurWidth),
                static_cast<float>(blurHeight), 0, 1};
            context->RSSetViewports(1, &blurViewport);
            for (int pass = 0; pass < 6; ++pass) {
                int targetIndex = pass % 2 ? resultIndex : 0;
                auto target = blurTargets[targetIndex].get();
                auto source = pass == 0 ? input
                                        : blurViews[targetIndex ? 0 : resultIndex].get();
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
            return blurViews[resultIndex].get();
        };
        ID3D11ShaderResourceView* panelInput=compositeView.get();
        auto effectsLens = lens;
        UpdateEffectsLocked(effectsLens);
        if (lens.bodyMode != 0) {
            auto bodyBlur=blurBackdrop(compositeView.get(),
                lens.bodyMode == 1 ? lens.bodyBlur : 0, 2);
            auto bodyLens=effectsLens;
            bodyLens.lensCount=0; // Body first; no panels can feed back into it.
            context->ClearRenderTargetView(renderedBodyTarget.get(),clear);
            drawGlass(renderedBodyTarget.get(),bodyLens,compositeView.get(),bodyBlur,bodyBlur);
            panelInput=renderedBodyView.get();
        }
        auto blurredView=blurBackdrop(panelInput,lens.optics.blur,1);

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
        std::copy_n(effectsLens.fluidPointer, 4, presentLens.fluidPointer);
        std::copy_n(effectsLens.glintClock, 4, presentLens.glintClock);
        if (presentLens.bodyMode != 0)
            presentLens.bodyMode=3; // Compose panels over the completed body, without shading it twice.
        if (haveCompositorBounds) {
            for (int axis=0; axis<2; ++axis) {
                presentLens.halfSize[axis] = compositorBounds[axis+2] * geometryScale / 2;
                presentLens.center[axis] = compositorBounds[axis] * geometryScale + presentLens.halfSize[axis];
            }
            presentLens.radius = std::min({presentLens.radius, presentLens.halfSize[0], presentLens.halfSize[1]});
            for (size_t i=0;i<presentLens.lensCount;++i)
                if (lensProbes[i].tag && lensProbes[i].tag == sampledLensTags[i]) {
                    presentLens.lensNativeRects[i][0] = sampledLensX[i][0]*geometryScale;
                    presentLens.lensRects[i][0] = sampledLensX[i][1]*geometryScale;
                    presentLens.lensFades[i][0] = 1 - sampledLensOpacity[i];
                }
        }
        drawGlass(backTarget.get(),presentLens,panelInput,blurredView,panelInput);
        // Visibility can change after the latency wait. Never block with the
        // renderer mutex held while the UI thread is minimizing or closing it.
        hr = swapChain->Present(0, DXGI_PRESENT_DO_NOT_WAIT);
        if (hr == DXGI_ERROR_WAS_STILL_DRAWING) pendingPresent = true;
        else if (FAILED(hr)) failure = hr;
    }

    struct Teardown {
        struct DrainWork {
            void operator()(PTP_WORK work) const noexcept {
                WaitForThreadpoolWorkCallbacks(work, TRUE);
                CloseThreadpoolWork(work);
            }
        };
        std::unique_ptr<TP_WORK, DrainWork> sourceWork, presentWork;
        std::unique_ptr<PresentWorkContext> presentContext;
        winrt::handle waitable, timer;
        std::vector<std::shared_ptr<Source>> sources;

        bool empty() const noexcept {
            return !sourceWork && !presentWork && !waitable && !timer && sources.empty();
        }
        void Drain() noexcept {
            sourceWork.reset();
            presentWork.reset();
            presentContext.reset();
            waitable.close();
            timer.close();
            for (auto const& source : sources) CloseSource(source);
            sources.clear();
        }
        Teardown() = default;
        Teardown(Teardown&&) noexcept = default;
        Teardown& operator=(Teardown&&) = delete;
        ~Teardown() { Drain(); }
    };

    // The threadpool tears down because every release here can wait on the capture
    // server or the D3D device, and the owner thread is the taskbar's (issue 15).
    struct Retirement {
        RetiringScope retiring;
        std::shared_ptr<LiquidRenderer> renderer;
        std::optional<Teardown> taken;

        static void CALLBACK Run(PTP_CALLBACK_INSTANCE, void* raw) noexcept {
            std::unique_ptr<Retirement> job(static_cast<Retirement*>(raw));
            bool apartment = SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
            auto started = GetTickCount64();
            {
                Teardown teardown = job->taken ? std::move(*job->taken) : job->renderer->TakeUnderLock();
                teardown.Drain();
            }
            if (auto ms = GetTickCount64() - started; ms >= 1000)
                Wh_Log(L"[UWLIQUID] capture teardown took %llu ms", ms);
            job->renderer.reset();
            if (apartment) CoUninitialize();
        }
    };

    Teardown TakeLocked() {
        running = false;
        ++sourceRevision;
        sourceUpdatePending=false;
        sourceWorkPending=false;
        requestedSources.clear();
        pendingPresent = false;
        presentWorkPending = false;
        motionActive = false;
        effectsAnimating = false;
        motionObservationPending = false;
        bodyVisual = nullptr;
        haveCompositorBounds = false;
        lensProbes = {};
        std::fill_n(sampledLensTags,kMaxButtonLenses,0);
        Teardown taken;
        taken.sources = std::exchange(sources, {});
        taken.sources.insert(taken.sources.end(), parked.begin(), parked.end());
        parked.clear();
        if (geometrySource) taken.sources.push_back(std::exchange(geometrySource, nullptr));
        for (auto const& source : taken.sources) source->active.store(false, std::memory_order_release);
        taken.sourceWork.reset(std::exchange(sourceWork, nullptr));
        taken.presentWork.reset(std::exchange(presentWork, nullptr));
        taken.presentContext.reset(std::exchange(presentWorkContext, nullptr));
        taken.waitable.attach(std::exchange(frameLatencyWaitable, nullptr));
        taken.timer.attach(std::exchange(presentTimer, nullptr));
        return taken;
    }

    Teardown TakeUnderLock() {
        std::lock_guard lock(mutex);
        return TakeLocked();
    }

    void Retire(std::optional<Teardown> taken) {
        auto job = new Retirement{{}, shared_from_this(), std::move(taken)};
        if (!TrySubmitThreadpoolCallback(Retirement::Run, job, nullptr)) {
            Wh_Log(L"[UWLIQUID] threadpool refused the capture teardown; draining inline");
            Retirement::Run(nullptr, job);
        }
    }

    void RetireSources(std::vector<std::shared_ptr<Source>> old) {
        std::erase(old, nullptr);
        if (old.empty()) return;
        for (auto const& source : old) source->active.store(false, std::memory_order_release);
        Teardown taken;
        taken.sources = std::move(old);
        Retire(std::move(taken));
    }

    // The longest healthy hold on `mutex` is one RenderLocked. A longer hold is a D3D
    // call stuck behind the device lock, which Stop must not wait out.
    static constexpr std::chrono::milliseconds kStopLockBudget{20};

    void Stop() {
        std::optional<Teardown> taken;
        if (std::unique_lock lock(mutex, kStopLockBudget); lock) {
            taken.emplace(TakeLocked());
            if (taken->empty()) return;
        } else {
            Wh_Log(L"[UWLIQUID] stop deferred: renderer lock busy over %lld ms",
                   static_cast<long long>(kStopLockBudget.count()));
        }
        Retire(std::move(taken));
    }

    bool Adopt() {
        std::unique_lock lock(mutex, kStopLockBudget);
        if (!lock || !running || FAILED(failure)) return false;
        pointerActive = false;
        velocityX = velocityY = 0;
        return true;
    }
};

// Both callers supply measured/synthetic device-pixel rectangles. Appearance
// decisions happen before packing; this function never changes a XAML element.
uint32_t AppendButtonLens(LiquidRenderer::LensConstants& lens,
                          float4 nativeRect,float4 clipRect,float4 radii,float3 shape,
                          bool attention,double opacity) {
    auto index=lens.lensCount++;
    auto store=[](float (&target)[4],float4 value) {
        target[0]=value.x;target[1]=value.y;target[2]=value.z;target[3]=value.w;
    };
    store(lens.lensNativeRects[index],nativeRect);
    store(lens.lensRects[index],clipRect);
    store(lens.lensRadii[index],radii);
    store(lens.lensShapeParams[index],{shape.x,shape.y,shape.z,0});
    store(lens.lensTints[index],{kButtonAttentionTint.R/255.0f,kButtonAttentionTint.G/255.0f,
        kButtonAttentionTint.B/255.0f,attention?1.0f:0.0f});
    lens.lensFades[index][0]=1-static_cast<float>(std::clamp(opacity,0.0,1.0));
    lens.lensRefraction=lens.optics.refraction*.78f;
    lens.alertTintOpacity=.22f;
    lens.lensBlend=.72f;
    return index;
}

GlassPointerBinding::GlassPointerBinding(UIElement const& element, FrameworkElement const& reference,
                                        std::shared_ptr<LiquidRenderer> const& renderer)
    : source(element) {
    auto weakElement = winrt::make_weak(reference);
    handler = winrt::box_value(Input::PointerEventHandler([weakElement, weak = std::weak_ptr<LiquidRenderer>(renderer)]
        (auto const&, Input::PointerRoutedEventArgs const& args) {
        try {
            auto target = weak.lock(); auto relative = weakElement.get();
            if (!target || !relative || relative.ActualWidth() <= 0 || relative.ActualHeight() <= 0) return;
            auto point = args.GetCurrentPoint(relative);
            auto p = point.Position();
            float x = float(p.X / relative.ActualWidth()), y = float(p.Y / relative.ActualHeight());
            bool active = x >= 0 && x <= 1 && y >= 0 && y <= 1 &&
                (point.PointerDevice().PointerDeviceType() == winrt::Windows::Devices::Input::PointerDeviceType::Mouse ||
                 point.IsInContact());
            target->SetPointer(x, y, active);
        } catch (...) {}
    }));
    cancelled = winrt::box_value(Input::PointerEventHandler(
        [weak = std::weak_ptr<LiquidRenderer>(renderer)](auto const&, auto const&) {
            if (auto target = weak.lock()) target->SetPointer(0, 0, false);
        }));
    source.AddHandler(UIElement::PointerMovedEvent(), handler, true);
    source.AddHandler(UIElement::PointerExitedEvent(), handler, true);
    source.AddHandler(UIElement::PointerCanceledEvent(), cancelled, true);
}

double PixelLuminance(uint32_t pixel);
winrt::Windows::UI::Color ForegroundColor(bool light);
bool HighContrastActive();

void BuildPreviewBackdrop(EditorPreview& preview) {
    preview.pixels.resize(preview.width * preview.height);
    // A fixed, contrasting sample makes refraction/blur comparable between edits.
    for (int y = 0; y < preview.height; ++y) {
        for (int x = 0; x < preview.width; ++x) {
            double t = double(x) / preview.width;
            double sceneY = y * 180.0 / preview.height;
            double wave = std::sin(x * 0.014 + sceneY * 0.018);
            bool warm = sceneY > 70 + 36 * std::sin(x * 0.009);
            int r = static_cast<int>(warm ? 130 + 70 * t : 30 + 25 * wave);
            int g = static_cast<int>(warm ? 65 + 50 * t : 90 + 60 * t);
            int b = static_cast<int>(warm ? 115 + 30 * wave : 160 + 40 * t);
            if ((x + y / 3) % 64 < 2 || y % 40 == 0) {
                r += 35; g += 35; b += 35;
            }
            preview.pixels[y * preview.width + x] = 0xFF000000u |
                (std::clamp(r, 0, 255) << 16) | (std::clamp(g, 0, 255) << 8) |
                std::clamp(b, 0, 255);
        }
    }
    Media::Imaging::WriteableBitmap bitmap(preview.width, preview.height);
    auto access = bitmap.PixelBuffer().as<::Windows::Storage::Streams::IBufferByteAccess>();
    byte* bytes{};
    winrt::check_hresult(access->Buffer(&bytes));
    memcpy(bytes, preview.pixels.data(), preview.pixels.size() * sizeof(uint32_t));
    bitmap.Invalidate();
    preview.backdrop.Source(bitmap);
    for (FrameworkElement element : {preview.canvas.as<FrameworkElement>(),
                                     preview.backdrop.as<FrameworkElement>(),
                                     preview.liquidHost.as<FrameworkElement>()}) {
        element.Width(preview.width);
        element.Height(preview.height);
    }
    auto visual = Hosting::ElementCompositionPreview::GetElementVisual(preview.canvas);
    auto geometry = visual.Compositor().CreateRoundedRectangleGeometry();
    geometry.Size({float(preview.width), float(preview.height)});
    geometry.CornerRadius({14,14});
    visual.Clip(visual.Compositor().CreateGeometricClip(geometry));
}

Media::Imaging::WriteableBitmap PreviewAppIcon(IWICImagingFactory* factory, PCWSTR path) {
    struct Icon {
        HICON value{};
        ~Icon() { if (value) DestroyIcon(value); }
    } icon;
    HRESULT extracted = SHDefExtractIconW(path, 0, 0, &icon.value, nullptr, 128);
    if (FAILED(extracted) || !icon.value) {
        SHSTOCKICONINFO stock{};
        stock.cbSize = sizeof(stock);
        winrt::check_hresult(SHGetStockIconInfo(SIID_APPLICATION, SHGSI_ICON | SHGSI_LARGEICON, &stock));
        icon.value = stock.hIcon;
    }
    winrt::com_ptr<IWICBitmap> source;
    winrt::check_hresult(factory->CreateBitmapFromHICON(icon.value, source.put()));
    winrt::com_ptr<IWICFormatConverter> converted;
    winrt::check_hresult(factory->CreateFormatConverter(converted.put()));
    winrt::check_hresult(converted->Initialize(source.get(), GUID_WICPixelFormat32bppPBGRA,
        WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom));
    UINT width{}, height{};
    winrt::check_hresult(converted->GetSize(&width, &height));
    Media::Imaging::WriteableBitmap bitmap(width, height);
    byte* bytes{};
    winrt::check_hresult(bitmap.PixelBuffer().as<::Windows::Storage::Streams::IBufferByteAccess>()->Buffer(&bytes));
    winrt::check_hresult(converted->CopyPixels(nullptr, width * 4, width * height * 4, bytes));
    bitmap.Invalidate();
    return bitmap;
}

// The HWND region only limits hit testing. Pad it beyond the visible contour so
// its integer coordinates never cut off the shader's antialiased edge pixels.
void UpdateEditorWindowShape(SettingsEditor& editor, UINT width, UINT height, float radius) {
    radius = std::min(radius, std::min(width, height) / 2.0f);
    if (editor.windowShapeSize.cx == LONG(width) && editor.windowShapeSize.cy == LONG(height) &&
        editor.windowShapeRadius == radius) return;
    HRGN region{};
    if (radius > 0) {
        std::vector<POINT> points;
        for (int corner = 0; corner < 4; ++corner) {
            double cx = corner < 2 ? width - radius : radius;
            double cy = corner == 0 || corner == 3 ? radius : height - radius;
            for (int step = 0; step <= 96; ++step) {
                double angle = (corner - 1 + step / 96.0) * 1.5707963267948966;
                auto axis = [radius](double value) {
                    return (radius + 2) * std::copysign(std::sqrt(std::abs(value)), value);
                };
                points.push_back({std::clamp(LONG(std::lround(cx + axis(std::cos(angle)))),0L,LONG(width)),
                                  std::clamp(LONG(std::lround(cy + axis(std::sin(angle)))),0L,LONG(height))});
            }
        }
        region = CreatePolygonRgn(points.data(), int(points.size()), WINDING);
        if (!region) winrt::throw_last_error();
    }
    if (!SetWindowRgn(editor.window, region, TRUE)) {
        if (region) DeleteObject(region);
        winrt::throw_last_error();
    }
    // Windows owns the region after a successful SetWindowRgn.
    editor.windowShapeSize = {LONG(width), LONG(height)};
    editor.windowShapeRadius = radius;
}

void CloseEditorSurface(SettingsEditor& editor) {
    editor.glassPointer.reset();
    KillTimer(editor.window, kEditorBackdropTimerId);
    if (editor.backgroundRenderer) {
        try {
            if (Hosting::ElementCompositionPreview::GetElementChildVisual(editor.liquidBackground) ==
                editor.backgroundRenderer->visualRoot)
                Hosting::ElementCompositionPreview::SetElementChildVisual(editor.liquidBackground, nullptr);
        } catch (...) {}
        editor.backgroundRenderer->Stop();
        editor.backgroundRenderer.reset();
    }
    if (editor.desktopRenderer) {
        editor.bodyBackdrop.Brush(nullptr);
        editor.desktopRenderer->Stop();
        editor.desktopRenderer.reset();
    }
    editor.backgroundGeometry = nullptr;
    if (editor.liquidBackground) editor.liquidBackground.Visibility(Visibility::Collapsed);
    UpdateEditorCaption(editor);
}

void UpdateEditorPosition(SettingsEditor& editor) noexcept {
    if(!editor.backgroundRenderer) {
        UpdateEditorSurface(editor,false);
        return;
    }
    try {
        POINT origin{};
        if(!ClientToScreen(editor.window,&origin)) return;
        bool ready=editor.backgroundRenderer->UpdatePosition(origin);
        if(editor.desktopRenderer) ready=editor.desktopRenderer->UpdatePosition(origin) && ready;
        if(!ready) UpdateEditorSurface(editor,false);
    } catch (...) {
        Wh_Log(L"[UWSET] background position update failed hr=%08X",winrt::to_hresult().value);
    }
}

void UpdateEditorSurface(SettingsEditor& editor, bool updateCards) noexcept {
    if (!editor.surface || editor.syncing || editor.updatingSurface) return;
    editor.updatingSurface = true;
    struct Reset { bool& value; ~Reset() { value = false; } } reset{editor.updatingSurface};
    try {
        using Color = winrt::Windows::UI::Color;
        using namespace winrt::Windows::UI::ViewManagement;
        bool contrast = HighContrastActive();
        bool dark = editor.content.RequestedTheme() == ElementTheme::Dark;
        bool glass = !contrast;
        int body = std::clamp(editor.controls[42].input.as<Controls::ComboBox>().SelectedIndex(), 0, 3);
        int appearance = body + (dark ? 5 : 0) + (contrast ? 10 : 0);
        auto solid = [](Color color) { return Media::SolidColorBrush(color); };
        auto resourceColor=[&](ResourceDictionary const& resources,std::wstring const& name,Color color) {
            auto key=winrt::box_value(name);
            // Existing templates can retain the resolved brush across a theme
            // change. Update that brush rather than replacing its dictionary entry.
            if(resources.HasKey(key))
                if(auto brush=resources.Lookup(key).try_as<Media::SolidColorBrush>()) {
                    brush.Color(color);
                    return;
                }
            resources.Insert(key,solid(color));
        };
        if (appearance != editor.surfaceAppearance) {
            updateCards = true;
            editor.surfaceAppearance = appearance;
            auto background = contrast ? editor.systemColors.GetColorValue(UIColorType::Background) :
                dark ? Color{255,32,32,32} : Color{255,243,243,243};
            auto foreground = contrast ? editor.systemColors.GetColorValue(UIColorType::Foreground) :
                dark ? Color{255,245,245,245} : Color{255,20,20,20};
            editor.surface.Background(solid(glass ? Color{0,0,0,0} : background));
            editor.content.Foreground(solid(foreground));
            if (editor.preview.refresh) editor.preview.refresh.Foreground(solid(foreground));
            editor.content.FindName(L"EditorMark").as<Controls::FontIcon>().Foreground(solid(foreground));
            for (auto const& section : {editor.sidebar, editor.inspector}) {
                section.RequestedTheme(ElementTheme::Default);
                section.Background(solid(glass ? Color{0,0,0,0} : background));
                section.BorderThickness({0,0,0,0});
                section.BorderBrush(nullptr);
                section.Translation({0,0,contrast ? 0.0f : 16.0f});
                section.Shadow(nullptr);
            }
            editor.footer.Background(solid(glass ? Color{0,0,0,0} : background));
            editor.cardMaterial = solid(glass ? Color{0,0,0,0} : background);
            // XAML islands otherwise composite partial-alpha pixels over white.
            // Supply only the raw desktop at the edge; the opaque GPU body
            // covers it everywhere else. No DWM material fill is enabled.
            BOOL hostBackdrop = !contrast;
            DwmSetWindowAttribute(editor.window, DWMWA_USE_HOSTBACKDROPBRUSH,
                                  &hostBackdrop, sizeof(hostBackdrop));
            DWM_SYSTEMBACKDROP_TYPE backdrop = DWMSBT_NONE;
            DwmSetWindowAttribute(editor.window, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));
            MARGINS margins{};
            DwmExtendFrameIntoClientArea(editor.window, &margins);
            if (auto target = editor.island.try_as<ICompositionSupportsSystemBackdrop>()) {
                auto compositor = Hosting::ElementCompositionPreview::GetElementVisual(editor.content).Compositor();
                target.SystemBackdrop(contrast ? nullptr : compositor.CreateHostBackdropBrush());
            }
            editor.bodyMaterial.Background(nullptr);
            editor.bodyBackdrop.Brush(nullptr);
            auto compositor = editor.bodyBackdrop.Compositor();
            if (body == 2) {
                auto acrylic = GlassMaterial(true, dark);
                // In-app Acrylic stays active in XAML islands. Its native blur
                // samples the raw host backdrop placed directly beneath it.
                acrylic.BackgroundSource(Media::AcrylicBackgroundSource::Backdrop);
                editor.bodyBackdrop.Brush(compositor.CreateHostBackdropBrush());
                editor.bodyMaterial.Background(acrylic);
            } else if (body == 1) {
                if (auto wallpaper = compositor.try_as<ICompositorWithBlurredWallpaperBackdropBrush>())
                    editor.bodyBackdrop.Brush(wallpaper.TryCreateBlurredWallpaperBackdropBrush());
            } else if (body == 3) {
                editor.bodyBackdrop.Brush(compositor.CreateHostBackdropBrush());
            }
        }
        // Rows share their panel's material; only keyboard focus draws an outline.
        if (updateCards) {
            auto buttonPalette=[&](Controls::Control const& button,bool primary=false) {
                if(!button) return;
                std::array<Color,3> backgrounds{},foregrounds{};
                for(size_t state=0;state<3;++state) {
                    BYTE shade=dark?(state==2?40:state==1?76:56):(state==2?206:state==1?218:234);
                    auto bg=Color{255,shade,shade,shade};
                    if(primary) {
                        bg=editor.systemColors.GetColorValue(UIColorType::Accent);
                        double strength=state==2?.8:state==1?.9:1;
                        bg.R=BYTE(bg.R*strength);bg.G=BYTE(bg.G*strength);bg.B=BYTE(bg.B*strength);
                    }
                    uint32_t rgb=(uint32_t(bg.R)<<16)|(uint32_t(bg.G)<<8)|bg.B;
                    backgrounds[state]=contrast?editor.systemColors.GetColorValue(UIColorType::Background):bg;
                    foregrounds[state]=contrast?editor.systemColors.GetColorValue(UIColorType::Foreground):
                        ForegroundColor(PixelLuminance(rgb)<0.179);
                }
                auto localColor=[&](DependencyProperty const& property,Color color) {
                    auto brush=button.ReadLocalValue(property).try_as<Media::SolidColorBrush>();
                    if(brush) {if(brush.Color()!=color) brush.Color(color);}
                    else button.SetValue(property,solid(color));
                };
                localColor(Controls::Control::BackgroundProperty(),backgrounds[0]);
                localColor(Controls::Control::ForegroundProperty(),foregrounds[0]);
                // The inbox XAML templates use these semantic resources for
                // hover, press and checked states. Scope them to each button.
                auto resources=button.Resources();
                resourceColor(resources,L"SystemControlBackgroundBaseLowBrush",backgrounds[0]);
                resourceColor(resources,L"SystemControlHighlightBaseLowBrush",backgrounds[1]);
                resourceColor(resources,L"SystemControlHighlightBaseMediumLowBrush",backgrounds[2]);
                resourceColor(resources,L"SystemControlHighlightAccentBrush",backgrounds[1]);
                resourceColor(resources,L"SystemControlForegroundBaseHighBrush",foregrounds[0]);
                resourceColor(resources,L"SystemControlHighlightBaseHighBrush",foregrounds[1]);
                resourceColor(resources,L"SystemControlHighlightAltBaseHighBrush",foregrounds[1]);
                resourceColor(resources,L"SystemControlHighlightAltChromeWhiteBrush",foregrounds[1]);
            };
            buttonPalette(editor.save,true);
            for(auto const& button:{editor.reload,editor.openFile,editor.enableLiquid,editor.refreshTray})
                buttonPalette(button);
            for(auto const& section:editor.sections) buttonPalette(section.header);
            for(auto const& row:editor.trayRows) {buttonPalette(row.choose);buttonPalette(row.reset);}
            for (auto const& control : editor.controls)
                if (control.card) {
                    control.card.Background(editor.cardMaterial);
                    control.card.BorderThickness({0,0,0,0});
                }
            if (editor.trayList)
                for (auto const& item : editor.trayList.Children())
                    if (auto border = item.try_as<Controls::Border>()) border.Background(editor.cardMaterial);
        }
        for (uint32_t i = 0; i < editor.navigation.Items().Size(); ++i) {
            auto item = editor.navigation.Items().GetAt(i).as<Controls::ListBoxItem>();
            bool selected = int(i) == editor.navigation.SelectedIndex();
            item.FontWeight(selected ? winrt::Windows::UI::Text::FontWeights::SemiBold() :
                                       winrt::Windows::UI::Text::FontWeights::Normal());
            item.Content().as<Controls::Panel>().Children().GetAt(1)
                .as<Controls::TextBlock>().FontWeight(item.FontWeight());
        }
        RECT shapeBounds{};
        if (GetClientRect(editor.window, &shapeBounds) && shapeBounds.right > 0 && shapeBounds.bottom > 0)
            UpdateEditorWindowShape(editor, shapeBounds.right, shapeBounds.bottom,
                IsZoomed(editor.window) || contrast ? 0 : kEditorWindowRadius * editor.dpi / 96.0f);
        if (editor.header && editor.header.ActualWidth() > 0) {
            float scale = editor.dpi / 96.0f;
            auto header = editor.header.TransformToVisual(editor.content).TransformPoint({0,0});
            editor.headerBounds = {LONG(header.X*scale), LONG(header.Y*scale),
                LONG((header.X + editor.header.ActualWidth())*scale),
                LONG((header.Y + editor.header.ActualHeight())*scale)};
        }
        if (contrast || !editor.root ||
            !IsWindowVisible(editor.window) || IsIconic(editor.window)) {
            CloseEditorSurface(editor);
            return;
        }
        // WinRT capture activation cannot run inside a cross-thread SendMessage.
        // The posted theme refresh runs after that synchronous caller returns.
        if (!editor.backgroundRenderer && InSendMessage()) {
            PostMessageW(editor.window, kEditorThemeMessage, 0, 0);
            return;
        }
        RECT rect{};
        if (!GetClientRect(editor.window, &rect) || rect.right <= 0 || rect.bottom <= 0) return;
        UINT width = rect.right, height = rect.bottom;
        POINT origin{};
        if (!ClientToScreen(editor.window, &origin)) winrt::throw_last_error();
        OffsetRect(&rect, origin.x, origin.y);
        float scale = GetDpiForWindow(editor.window) / 96.0f;
        float dipWidth = width / scale, dipHeight = height / scale;
        editor.bodySurface.SourceSize({dipWidth,dipHeight});
        editor.bodySample.Size({float(width),float(height)});
        editor.bodyBackdrop.Size({dipWidth,dipHeight});
        if (body>=2) {
            // A host-backdrop brush becomes a stale snapshot through a captured
            // VisualSurface. Feed live window pixels underneath native Acrylic
            // instead; its material and the glass panels retain their own pass.
            if (!editor.desktopRenderer) {
                auto renderer=std::make_shared<LiquidRenderer>();
                if (!renderer->Initialize(editor.bodyBackdrop.Compositor(),editor.window,
                                          width,height,nullptr,false))
                    throw winrt::hresult_error(E_FAIL,L"Settings backdrop unavailable");
                editor.desktopRenderer=std::move(renderer);
            }
            LiquidRenderer::LensConstants desktop{};
            desktop.bodyMode=4; // Unshaded, opaque input to the native material.
            desktop.center[0]=desktop.halfSize[0]=width/2.0f;
            desktop.center[1]=desktop.halfSize[1]=height/2.0f;
            if (!editor.desktopRenderer->UpdateGeometry(rect,width,height,desktop))
                throw winrt::hresult_error(E_FAIL,L"Settings backdrop device unavailable");
            editor.bodyBackdrop.Brush(editor.desktopRenderer->pixels.Brush());
            if (!InSendMessage())
                editor.desktopRenderer->UpdateSources(editor.desktopRenderer->CandidateWindows());
        } else if (editor.desktopRenderer) {
            editor.desktopRenderer->Stop();
            editor.desktopRenderer.reset();
        }
        if (!editor.backgroundRenderer) {
            auto visual = Hosting::ElementCompositionPreview::GetElementVisual(editor.liquidBackground);
            auto compositor = visual.Compositor();
            auto renderer = std::make_shared<LiquidRenderer>();
            if (!renderer->Initialize(compositor, editor.window, width, height,nullptr,false))
                throw winrt::hresult_error(E_FAIL, L"Settings glass unavailable");
            auto geometry = compositor.CreateRoundedRectangleGeometry();
            geometry.CornerRadius({0,0}); // The shader and Win32 region share the continuous outline.
            visual.Clip(compositor.CreateGeometricClip(geometry));
            renderer->FollowDock(geometry, false);
            editor.backgroundGeometry = geometry;
            editor.backgroundRenderer = renderer;
            editor.glassPointer = std::make_unique<GlassPointerBinding>(editor.content, editor.content, renderer);
            Hosting::ElementCompositionPreview::SetElementChildVisual(editor.liquidBackground, renderer->visualRoot);
        }
        editor.backgroundGeometry.Size({dipWidth, dipHeight});
        float windowRadius = IsZoomed(editor.window) ? 0.0f : kEditorWindowRadius;
        LiquidRenderer::LensConstants lens{};
        lens.center[0] = lens.halfSize[0] = width / 2.0f;
        lens.center[1] = lens.halfSize[1] = height / 2.0f;
        lens.radius = windowRadius * scale;
        auto ui = EditorPageValues(editor, 3);
        auto value = [&](PCWSTR key) {
            auto const& field = SettingDefinition(key);
            return std::clamp(LocalInt(ui, key), field.minimum, field.maximum);
        };
        lens.bodyMode = body == 3 ? 1 : float(body + 4);
        lens.bodyBlur = value(L"ui.bodyBlur") / 100.0f;
        lens.optics = ReadUiLiquidSettings(ui, scale);
        std::fill_n(lens.bodyMaterialTint, 3, dark ? 32.0f / 255 : 243.0f / 255);
        lens.bodyMaterialTint[3] = 0.8f;
        auto addLayer = [&](FrameworkElement const& element, CornerRadius radius,
                            float kind, Color tint = Color{}) {
            if (!element || element.ActualWidth() <= 0 || element.ActualHeight() <= 0 ||
                lens.lensCount == kMaxButtonLenses) return;
            auto origin = element.TransformToVisual(editor.content).TransformPoint({0,0});
            float left = std::max(origin.X, 0.0f);
            float top = std::max(origin.Y, 0.0f);
            float right = std::min(origin.X + float(element.ActualWidth()), dipWidth);
            float bottom = std::min(origin.Y + float(element.ActualHeight()), dipHeight);
            if (right <= left || bottom <= top) return;
            auto i = lens.lensCount++;
            float clipped[]{(left + right) * scale / 2, (top + bottom) * scale / 2,
                            (right - left) * scale / 2, (bottom - top) * scale / 2};
            float native[]{(origin.X + float(element.ActualWidth()) / 2) * scale,
                           (origin.Y + float(element.ActualHeight()) / 2) * scale,
                           float(element.ActualWidth()) * scale / 2, float(element.ActualHeight()) * scale / 2};
            std::copy_n(clipped, 4, lens.lensRects[i]);
            std::copy_n(native, 4, lens.lensNativeRects[i]);
            float radii[]{float(radius.TopLeft) * scale, float(radius.TopRight) * scale,
                          float(radius.BottomRight) * scale, float(radius.BottomLeft) * scale};
            std::copy_n(radii, 4, lens.lensRadii[i]);
            lens.lensShapeParams[i][0] = 1;
            lens.lensShapeParams[i][3] = kind;
            float color[]{tint.R/255.0f, tint.G/255.0f, tint.B/255.0f, tint.A/255.0f};
            std::copy_n(color, 4, lens.lensTints[i]);
        };
        for (size_t i = 0; i < editor.captionButtons.size(); ++i)
            if (editor.captionStates[i])
                addLayer(editor.captionButtons[i], editor.captionButtons[i].CornerRadius(),
                         2, EditorCaptionTint(editor, i));
        addLayer(editor.sidebar, editor.sidebar.CornerRadius(), 1);
        addLayer(editor.inspector, editor.inspector.CornerRadius(), 1);
        lens.lensRefraction = lens.optics.refraction;
        lens.lensBlend = value(L"ui.panelOpacity") / 100.0f;
        lens.alertTintOpacity = 1;
        if (!editor.backgroundRenderer->UpdateGeometry(rect, width, height, lens))
            throw winrt::hresult_error(E_FAIL, L"Settings glass device unavailable");
        if (!InSendMessage()) editor.backgroundRenderer->SetBodyVisual(editor.bodySample);
        if (!InSendMessage()) editor.backgroundRenderer->UpdateSources();
        editor.liquidBackground.Visibility(Visibility::Visible);
        SetTimer(editor.window, kEditorBackdropTimerId, 250, nullptr);
        UpdateEditorCaption(editor);
    } catch (...) {
        Wh_Log(L"[UWSET] window material failed hr=%08X", winrt::to_hresult().value);
        try { CloseEditorSurface(editor); } catch (...) {}
    }
}

void CreateEditorPreview(SettingsEditor& editor) {
    auto& preview = editor.preview;
    auto find = [&](PCWSTR name) { return editor.content.FindName(name); };
    preview.canvas = find(L"PreviewCanvas").as<Controls::Canvas>();
    preview.backdrop = find(L"PreviewBackdrop").as<Controls::Image>();
    preview.material = find(L"PreviewMaterial").as<Controls::Border>();
    preview.liquidHost = find(L"PreviewLiquid").as<Controls::Grid>();
    preview.caption = find(L"PreviewCaption").as<Controls::TextBlock>();
    preview.measurement = find(L"PreviewMeasurement").as<Controls::TextBlock>();
    preview.refresh = find(L"PreviewRefresh").as<Controls::HyperlinkButton>();
    preview.refreshClick = preview.refresh.Click(winrt::auto_revoke,
        [window = editor.window](auto const&, auto const&) {
            SendMessageW(window, WM_COMMAND, kEditorRefreshPreview, 0);
        });
    BuildPreviewBackdrop(preview);
    for (int i = 0; i < 5; ++i) {
        Controls::Border button;
        button.Width(44);
        Controls::Canvas::SetLeft(button, 64 + i * 52);
        preview.canvas.Children().Append(button);
        preview.buttons.push_back(button);
    }
    winrt::com_ptr<IWICImagingFactory> factory;
    winrt::check_hresult(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
        __uuidof(IWICImagingFactory), factory.put_void()));
    wchar_t system[MAX_PATH]{}, windows[MAX_PATH]{};
    if (!GetSystemDirectoryW(system, ARRAYSIZE(system)) || !GetWindowsDirectoryW(windows, ARRAYSIZE(windows)))
        winrt::throw_last_error();
    std::wstring paths[]{std::wstring(windows) + L"\\explorer.exe", std::wstring(system) + L"\\notepad.exe",
                         std::wstring(system) + L"\\Taskmgr.exe", std::wstring(system) + L"\\control.exe"};
    constexpr PCWSTR names[]{L"File Explorer", L"Notepad", L"Task Manager", L"Control Panel"};
    for (size_t i = 0; i < std::size(paths); ++i) {
        Controls::Image image;
        image.Source(PreviewAppIcon(factory.get(), paths[i].c_str()));
        image.Stretch(Media::Stretch::Uniform);
        Automation::AutomationProperties::SetName(image, names[i]);
        preview.canvas.Children().Append(image);
        preview.appIcons.push_back(image);
    }
    constexpr PCWSTR glyphs[]{L"\xE753", L"\xE767", L"\xE701"};
    constexpr winrt::Windows::UI::Color colors[]{
        {255, 60, 200, 250},
        {255, 255, 255, 255}, {255, 255, 255, 255}};
    preview.trayHighlight=Controls::Border();
    preview.canvas.Children().Append(preview.trayHighlight);
    for (size_t i = 0; i < std::size(glyphs); ++i) {
        Controls::FontIcon icon;
        icon.Glyph(glyphs[i]);
        icon.FontSize(15);
        icon.IsTextScaleFactorEnabled(false);
        icon.Width(28);
        icon.Height(40);
        icon.Foreground(Media::SolidColorBrush(colors[i]));
        preview.canvas.Children().Append(icon);
        preview.icons.push_back(icon);
    }
    preview.startIcon = Markup::XamlReader::Load(LR"XAML(
<Path xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
      Width="22" Height="22" Fill="#3AB7FF" Stretch="Fill"
      Data="M0,0 H10 V10 H0 Z M12,0 H22 V10 H12 Z M0,12 H10 V22 H0 Z M12,12 H22 V22 H12 Z"/>
)XAML").as<Shapes::Path>();
    Controls::Canvas::SetLeft(preview.startIcon, 75);
    preview.canvas.Children().Append(preview.startIcon);
    preview.clock = Controls::TextBlock();
    preview.clock.Text(L"10:24 AM\nSat, 26 Sep");
    preview.clock.FontSize(11);
    preview.clock.IsTextScaleFactorEnabled(false);
    preview.clock.TextAlignment(TextAlignment::Right);
    preview.clock.Width(126);
    Controls::Canvas::SetLeft(preview.clock, 446);
    preview.canvas.Children().Append(preview.clock);
}

void ApplyPreviewMetrics(SettingsEditor& editor, TaskbarPreviewMetrics const& metrics) {
    auto& preview = editor.preview;
    preview.metrics = metrics;
    double step = std::max(44.0, metrics.iconWidth + 16) + 8;
    int width = static_cast<int>(std::ceil(5 * step + 380));
    int height = static_cast<int>(std::ceil(std::max(editor.compactLayout?140.0:420.0, metrics.dockHeight + 48)));
    if (preview.width != width || preview.height != height) {
        CloseEditorPreview(editor);
        preview.width = width;
        preview.height = height;
        preview.unavailable = false;
        BuildPreviewBackdrop(preview);
    }
    auto number = [](double value) {
        wchar_t text[32]{};
        swprintf(text, ARRAYSIZE(text), std::abs(value - std::round(value)) < 0.05 ? L"%.0f" : L"%.1f", value);
        return std::wstring(text);
    };
    double scale = metrics.dpi / 96.0;
    std::wstring label = L"Taskbar: " + (metrics.frameMeasured
        ? number(metrics.frameHeight * scale) + L" px (" + number(metrics.frameHeight) + L" DIP)"
        : L"48 DIP (estimate)");
    label += L"  ·  Icon boxes: ";
    if (!metrics.iconMeasured) {
        label += L"24 DIP (estimate)";
    } else if (metrics.iconMax - metrics.iconMin > 0.1) {
        label += number(metrics.iconMin * scale) + L"–" + number(metrics.iconMax * scale) +
                 L" px (" + number(metrics.iconMin) + L"–" + number(metrics.iconMax) + L" DIP)";
    } else if (std::abs(metrics.iconWidth - metrics.iconHeight) > 0.1) {
        label += number(metrics.iconWidth * scale) + L"×" + number(metrics.iconHeight * scale) +
                 L" px (" + number(metrics.iconWidth) + L"×" + number(metrics.iconHeight) + L" DIP)";
    } else {
        label += number(metrics.iconHeight * scale) + L" px (" + number(metrics.iconHeight) + L" DIP)";
    }
    if (metrics.frameMeasured) label += L"  ·  " + number(scale * 100) + L"%";
    preview.measurement.Text(label);
    UpdateEditorPreview(editor);
}

winrt::Windows::UI::Color PreviewHighlightColor(bool dark,bool highContrast) {
    if(highContrast) {
        auto color=GetSysColor(COLOR_HIGHLIGHT);
        return {255,GetRValue(color),GetGValue(color),GetBValue(color)};
    }
    return dark?winrt::Windows::UI::Color{32,255,255,255}:winrt::Windows::UI::Color{24,0,0,0};
}

// The sample owns its brushes outright. Runtime retains native bindings and
// animations, but both use the same fill, border and glass participation rules.
void ApplyPreviewButtonAppearance(SettingsEditor& editor,bool accepted) {
    if(!editor.valid) return;
    auto& preview=editor.preview;
    bool contrast=HighContrastActive();
    auto theme=contrast?DockTheme::Native:
        DockTheme(editor.controls[0].input.as<Controls::ComboBox>().SelectedIndex());
    bool layer=editor.controls[8].input.as<Controls::ToggleSwitch>().IsOn();
    bool borders=contrast || editor.controls[7].input.as<Controls::ToggleSwitch>().IsOn();
    bool dark=editor.content.RequestedTheme()==ElementTheme::Dark;
    auto colors=ButtonRimColors(dark);
    if(contrast) {
        auto color=GetSysColor(COLOR_HIGHLIGHTTEXT);
        colors.fill({255,GetRValue(color),GetGValue(color),GetBValue(color)});
    }
    for(size_t i=0;i<preview.buttons.size();++i) {
        auto button=preview.buttons[i];
        bool attention=i==3,visible=i==1 || attention;
        bool glass=accepted && i>0 && ButtonLensFits(button.Width(),button.Height()) &&
            ButtonGlassEnabled(theme,layer);
        if(button.Opacity()!=(visible?1.0:0.0)) button.Opacity(visible?1:0);
        auto color=attention && !contrast?kButtonAttentionTint:PreviewHighlightColor(dark,contrast);
        float opacity=ButtonFillOpacity(glass,contrast);
        auto brush=button.Background().try_as<Media::SolidColorBrush>();
        if(!brush || brush.Color()!=color || brush.Opacity()!=opacity) {
            brush=Media::SolidColorBrush(color);brush.Opacity(opacity);button.Background(brush);
        }
        Thickness thickness{1,1,1,1};
        if(button.BorderThickness()!=thickness) button.BorderThickness(thickness);
        auto rim=button.BorderBrush().try_as<Media::LinearGradientBrush>();
        if(!rim || rim.GradientStops().Size()!=2 ||
           rim.GradientStops().GetAt(0).Color()!=colors[0] ||
           rim.GradientStops().GetAt(1).Color()!=colors[1]) {
            rim=Media::LinearGradientBrush();rim.StartPoint({0,0});rim.EndPoint({0,1});
            for(size_t stop=0;stop<2;++stop) {
                Media::GradientStop value;value.Offset(double(stop));value.Color(colors[stop]);
                rim.GradientStops().Append(value);
            }
            button.BorderBrush(rim);
        }
        if(rim.Opacity()!=(borders?1.0:0.0)) rim.Opacity(borders?1:0);
    }
}

void UpdateEditorPreview(SettingsEditor& editor) noexcept {
    auto& preview = editor.preview;
    if (!preview.canvas) return;
    try {
        // Metrics/theme notifications can arrive after hiding or minimizing.
        // They must not recreate a presenter for an invisible editor.
        if (!IsWindowVisible(editor.window) || IsIconic(editor.window)) {
            CloseEditorPreview(editor);
            return;
        }
        bool highContrast = HighContrastActive();
        if (!editor.valid) {
            if (highContrast) CloseEditorPreview(editor);
            return;
        }
        auto number = [&](size_t index) { return editor.controls[index].slider.Value(); };
        auto toggle = [&](size_t index) {
            return editor.controls[index].input.as<Controls::ToggleSwitch>().IsOn();
        };
        auto choice = [&](size_t index) {
            return editor.controls[index].input.as<Controls::ComboBox>().SelectedIndex();
        };
        bool dark = editor.content.RequestedTheme() == ElementTheme::Dark;
        auto theme = highContrast ? DockTheme::Native : static_cast<DockTheme>(choice(0));
        bool liquid = theme == DockTheme::Liquid;
        if (!liquid || preview.unavailable) CloseEditorPreview(editor);
        double dockHeight = preview.metrics.dockHeight;
        double buttonWidth = std::max(44.0, preview.metrics.iconWidth + 16);
        double step = buttonWidth + 8;
        double dockWidth = 5 * step + 284;
        double trayLeft = 64 + 5 * step + 20;
        double y = (preview.height - dockHeight) / 2;
        double radius = std::min(number(3), dockHeight / 2);
        auto buttonInsets=GetButtonInsets({0,0},dockHeight,number(5));
        double padding=buttonInsets.top;
        double buttonHeight=float(dockHeight-buttonInsets.top-buttonInsets.bottom);
        double buttonRadius=ButtonClipRadius(buttonWidth,buttonHeight,number(4));
        Controls::Canvas::SetTop(preview.material, y);
        preview.material.Height(dockHeight);
        preview.material.Width(dockWidth);
        preview.material.CornerRadius({radius, radius, radius, radius});
        for (size_t i = 0; i < preview.buttons.size(); ++i) {
            auto button = preview.buttons[i];
            button.Width(buttonWidth);
            Controls::Canvas::SetLeft(button, 64 + i * step);
            button.Height(buttonHeight);
            button.CornerRadius({number(4),number(4),number(4),number(4)});
            Controls::Canvas::SetTop(button, y + padding);
        }
        for (size_t i = 0; i < preview.appIcons.size(); ++i) {
            auto image = preview.appIcons[i];
            image.Width(preview.metrics.iconWidth);
            image.Height(preview.metrics.iconHeight);
            Controls::Canvas::SetLeft(image, 64 + (i + 1) * step + (buttonWidth - preview.metrics.iconWidth) / 2);
            Controls::Canvas::SetTop(image, y + (dockHeight - preview.metrics.iconHeight) / 2);
        }
        constexpr double trayOffsets[]{0, 38, 66};
        auto trayInsets=GetButtonInsets({0,0},dockHeight,number(100));
        preview.trayHighlight.Width(28);
        preview.trayHighlight.Height(dockHeight-trayInsets.top-trayInsets.bottom);
        preview.trayHighlight.CornerRadius({number(101),number(101),number(101),number(101)});
        preview.trayHighlight.Background(Media::SolidColorBrush(PreviewHighlightColor(dark,highContrast)));
        Controls::Canvas::SetLeft(preview.trayHighlight,trayLeft+trayOffsets[1]);
        Controls::Canvas::SetTop(preview.trayHighlight,y+trayInsets.top);
        for (size_t i = 0; i < preview.icons.size(); ++i) {
            preview.icons[i].Height(dockHeight);
            Controls::Canvas::SetLeft(preview.icons[i], trayLeft + trayOffsets[i]);
            Controls::Canvas::SetTop(preview.icons[i], y);
        }
        preview.startIcon.Width(preview.metrics.iconWidth);
        preview.startIcon.Height(preview.metrics.iconHeight);
        Controls::Canvas::SetLeft(preview.startIcon, 64 + (buttonWidth - preview.metrics.iconWidth) / 2);
        Controls::Canvas::SetTop(preview.startIcon, y + (dockHeight - preview.metrics.iconHeight) / 2);
        Controls::Canvas::SetLeft(preview.clock, trayLeft + 102);
        Controls::Canvas::SetTop(preview.clock, y + (dockHeight - 34) / 2);

        if (liquid && !preview.unavailable) {
            if (!preview.renderer) {
                auto renderer = std::make_shared<LiquidRenderer>();
                auto compositor = Hosting::ElementCompositionPreview::GetElementVisual(
                    preview.liquidHost).Compositor();
                if (!renderer->Initialize(compositor, nullptr, preview.width, preview.height,editor.window))
                    throw winrt::hresult_error(E_FAIL);
                renderer->SetPreviewBackdrop(preview.pixels);
                Hosting::ElementCompositionPreview::SetElementChildVisual(
                    preview.liquidHost, renderer->visualRoot);
                preview.renderer = std::move(renderer);
                preview.glassPointer = std::make_unique<GlassPointerBinding>(preview.canvas, preview.canvas, preview.renderer);
            }
            LiquidRenderer::LensConstants lens{};
            lens.center[0] = static_cast<float>(48 + dockWidth / 2);
            lens.center[1] = static_cast<float>(y + dockHeight / 2);
            lens.halfSize[0] = static_cast<float>(dockWidth / 2);
            lens.halfSize[1] = static_cast<float>(dockHeight / 2);
            lens.radius = static_cast<float>(radius);
            lens.optics = ReadLiquidSettings(EditorPageValues(editor, 1));
            if (lens.optics.shadowOpacity > 0) {
                if (!preview.shadow) {
                    auto compositor = Hosting::ElementCompositionPreview::GetElementVisual(
                        preview.liquidHost).Compositor();
                    preview.shadowGeometry = compositor.CreateRoundedRectangleGeometry();
                    preview.shadow = std::make_shared<GlassShadow>(
                        preview.canvas, preview.material, preview.shadowGeometry);
                }
                preview.shadowGeometry.Offset({lens.center[0] - lens.halfSize[0],
                                                lens.center[1] - lens.halfSize[1]});
                preview.shadowGeometry.Size({lens.halfSize[0] * 2, lens.halfSize[1] * 2});
                preview.shadowGeometry.CornerRadius({lens.radius, lens.radius});
                preview.shadow->Update({float(preview.width), float(preview.height)}, {}, lens.optics);
            } else {
                preview.shadow.reset();
                preview.shadowGeometry = nullptr;
            }
            if(ButtonLensFits(buttonWidth,buttonHeight)) {
                // Start is not a TaskListButton. Only the four app samples
                // participate; every app lens follows the separate-layer option.
                for(size_t i=1;i<preview.buttons.size();++i) {
                    bool attention=i==3;
                    if(!ButtonGlassEnabled(theme,toggle(8))) continue;
                    float x = static_cast<float>(64 + i * step + buttonWidth / 2);
                    float centerY = static_cast<float>(y + dockHeight / 2);
                    float4 rect{x,centerY,float(buttonWidth/2),float(buttonHeight/2)};
                    float nativeRadius=float(number(4)),clipRadius=float(buttonRadius);
                    AppendButtonLens(lens,rect,rect,{nativeRadius,nativeRadius,nativeRadius,nativeRadius},
                        {1,clipRadius,clipRadius},attention,(i==1 || attention)?1:0);
                }
            }
            if (!preview.renderer->UpdateGeometry(
                    {0, 0, preview.width, preview.height}, preview.width, preview.height, lens))
                throw winrt::hresult_error(E_FAIL);
        }
        preview.liquidHost.Visibility(liquid && !preview.unavailable ? Visibility::Visible : Visibility::Collapsed);
        if (liquid)
            preview.material.Background(nullptr);
        else if (theme == DockTheme::Native)
            preview.material.Background(Media::SolidColorBrush(
                dark ? winrt::Windows::UI::Color{240, 32, 32, 32}
                     : winrt::Windows::UI::Color{240, 243, 243, 243}));
        else
            preview.material.Background(GlassMaterial(theme != DockTheme::Glass, dark));
        ApplyPreviewButtonAppearance(editor,liquid && !preview.unavailable);

        auto mode = highContrast ? FontColorMode::Default : static_cast<FontColorMode>(choice(1));
        auto foreground = [&](size_t index) {
            bool light = mode == FontColorMode::Light || (mode == FontColorMode::Default && dark);
            if (mode == FontColorMode::Adaptive) {
                light=preview.foregroundReady[index]?preview.foregroundLight[index]:dark;
            } else {
                preview.foregroundReady[index]=false;
            }
            auto color=ForegroundColor(light);
            if(highContrast) {
                auto native=GetSysColor(COLOR_WINDOWTEXT);
                color={255,GetRValue(native),GetGValue(native),GetBValue(native)};
            }
            return Media::SolidColorBrush(color);
        };
        preview.clock.Foreground(foreground(0));
        auto appColor = toggle(2) ? GrayscalePixel(0xFF3CC8FAu) : 0xFF3CC8FAu;
        preview.icons[0].Foreground(Media::SolidColorBrush(winrt::Windows::UI::Color{
            255, static_cast<BYTE>(appColor >> 16), static_cast<BYTE>(appColor >> 8), static_cast<BYTE>(appColor)}));
        preview.icons[1].Foreground(foreground(1));
        preview.icons[2].Foreground(foreground(2));
        ++preview.foregroundRevision;
        preview.foregroundDirty=mode==FontColorMode::Adaptive;
        preview.caption.Text(liquid && preview.unavailable ? L"Liquid preview unavailable" : L"Windows app samples");
    } catch (...) {
        Wh_Log(L"[UWSET] preview failed hr=%08X", winrt::to_hresult().value);
        preview.unavailable = true;
        CloseEditorPreview(editor);
        preview.caption.Text(L"Liquid preview unavailable");
    }
}

void CloseEditorPreview(SettingsEditor& editor) {
    auto& preview=editor.preview;
    ++preview.foregroundRevision;
    preview.foregroundDirty=false;
    // No UI delegate is queued by the capture job. Drain it before the window
    // or mod can release the code that performs the capture.
    if(preview.foregroundSample.valid()) {
        try { preview.foregroundSample.get(); } catch (...) {}
    }
    preview.glassPointer.reset();
    preview.shadow.reset();
    preview.shadowGeometry=nullptr;
    if (preview.renderer) {
        try {
            if (preview.liquidHost &&
                Hosting::ElementCompositionPreview::GetElementChildVisual(preview.liquidHost)==preview.renderer->visualRoot)
                Hosting::ElementCompositionPreview::SetElementChildVisual(preview.liquidHost,nullptr);
        } catch (...) {}
        preview.renderer->Stop();
        preview.renderer.reset();
    }
    if (preview.liquidHost) preview.liquidHost.Visibility(Visibility::Collapsed);
    ApplyPreviewButtonAppearance(editor,false);
    if(editor.valid && preview.material &&
       editor.controls[0].input.as<Controls::ComboBox>().SelectedIndex()==int(DockTheme::Liquid))
        preview.material.Background(nullptr);
}

void PrepareLiquid(Root& root, Measurements const& measurements);

struct LiquidMotionBatch {
    CompositionScopedBatch batch{nullptr};
    winrt::event_token completed{};
};
[[clang::no_destroy]] std::vector<LiquidMotionBatch> g_liquidMotionBatches;  // Owner thread only.
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
    ReleaseVector(g_liquidMotionBatches);
    g_liquidFinalFrames = 0;
    SetLiquidMotionActive(false);
}

void OnLiquidRendering() noexcept {
    ActiveScope active;
    if (g_stopping || GetCurrentThreadId() != g_ownerThread || g_busy) return;
    g_busy = true;
    struct Unbusy { ~Unbusy() { g_busy = false; PostItemStateRefresh(); } } unbusy;
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
void OnLayout(winrt::weak_ref<FrameworkElement> const& weak, bool notification=false);
void Refresh(Root& root);
void RefreshTaskbarHeight();

// ABI boundaries: raw IElementAnimatorOverrides arguments versus one-pointer
// C++/WinRT projected objects passed by const reference.
using HideAnimation_t = int(WINAPI*)(void*, void*, unsigned);
using ProjectedAnimation_t = void(WINAPI*)(void*, const void*);
HideAnimation_t HideAnimation_Original;
ProjectedAnimation_t StartAnimation_Original;
ProjectedAnimation_t HideCompleted_Original;

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
            PostItemStateRefresh();
            throw;
        }
        g_busy = previousBusy;
        PostItemStateRefresh();
        if (!previousBusy && frame) OnLayout(frame);
        return;
    }
    HideCompleted_Original(projectedAnimator, projectedElement);
}

FrameworkElement Child(FrameworkElement parent, const wchar_t* name, bool byClass = false);
void RetireRoot(Root& root);
void StopLiquid(Root& root);

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
        StopLiquid(root);
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
                name == L"BackgroundBorder" || name == L"MultiWindowElement")
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

Controls::Image FindImage(FrameworkElement const& parent,
                          unsigned depth = 8) {
    if (!parent || !depth) return nullptr;
    int count = Media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto child = Media::VisualTreeHelper::GetChild(parent, i)
                         .try_as<FrameworkElement>();
        if (!child) continue;
        if (auto image = child.try_as<Controls::Image>()) return image;
        if (auto image = FindImage(child, depth - 1)) return image;
    }
    return nullptr;
}

Size LargestAppIconBox(FrameworkElement const& button, FrameworkElement const& root) {
    Size result{};
    unsigned remaining = 256;
    auto visit = [&](auto&& self, FrameworkElement const& element, unsigned depth) -> void {
        if (!element || !depth || !remaining) return;
        --remaining;
        if (element.Visibility() != Visibility::Visible || element.Opacity() == 0) return;
        if (auto image = element.try_as<Controls::Image>(); image && image.Source() &&
            image.ActualWidth() > 0 && image.ActualHeight() > 0) {
            auto box = image.TransformToVisual(root).TransformBounds(
                {0, 0, static_cast<float>(image.ActualWidth()), static_cast<float>(image.ActualHeight())});
            if (std::isfinite(box.Width) && std::isfinite(box.Height) &&
                box.Width >= 8 && box.Height >= 8 && box.Width <= 256 && box.Height <= 256 &&
                box.Width * box.Height > result.Width * result.Height)
                result = {box.Width, box.Height};
        }
        int count = Media::VisualTreeHelper::GetChildrenCount(element);
        for (int i = 0; i < count && remaining; ++i)
            self(self, Media::VisualTreeHelper::GetChild(element, i).try_as<FrameworkElement>(), depth - 1);
    };
    visit(visit, button, 16);
    return result;
}

TaskbarPreviewMetrics MeasureTaskbarPreview(HMONITOR monitor) {
    TaskbarPreviewMetrics result;
    HWND taskbar{};
    for (auto name : {L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd"}) {
        HWND window{};
        for (int i = 0; i < 16 && (window = FindWindowExW(nullptr, window, name, nullptr)); ++i)
            if (MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST) == monitor) {
                taskbar = window;
                break;
            }
        if (taskbar) break;
    }
    if (!taskbar) return result;
    auto context = SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    struct RestoreDpi {
        DPI_AWARENESS_CONTEXT value;
        ~RestoreDpi() { if (value) SetThreadDpiAwarenessContext(value); }
    } restore{context};
    RECT bounds{};
    UINT dpi = GetDpiForWindow(taskbar);
    if (!dpi || !GetWindowRect(taskbar, &bounds) || bounds.bottom <= bounds.top ||
        bounds.bottom - bounds.top >= bounds.right - bounds.left) return result;
    result.dpi = dpi;
    result.frameHeight = (bounds.bottom - bounds.top) * 96.0 / dpi;
    result.frameMeasured = true;
    result.dockHeight = std::max(1.0, result.frameHeight - 2 * kDockInnerMargin);
    if (g_busy) return result;
    // XAML objects stay on the taskbar owner thread; only dimensions cross over.
    for (auto const& root : g_roots) {
        if (root.retired || root.taskbar != taskbar || !root.frame) continue;
        try {
            double scale = root.frame.XamlRoot().RasterizationScale();
            if (std::isfinite(scale) && scale > 0) {
                result.dpi = static_cast<UINT>(std::lround(scale * 96));
                result.frameHeight = (bounds.bottom - bounds.top) / scale;
            }
            if (root.measurementsReady && root.dock.background) {
                auto const& m = root.measurements;
                auto dock = root.dock.background.TransformToVisual(root.frame).TransformBounds({
                    0, static_cast<float>(m.insets.top), 1, static_cast<float>(m.visibleDockHeight())});
                if (std::isfinite(dock.Height) && dock.Height > 0) result.dockHeight = dock.Height;
            }
            std::vector<Size> icons;
            for (auto const& source : root.sources) {
                auto button = source.element.get();
                if (source.life != Root::Source::Life::Active || !source.included || !button) continue;
                auto size = LargestAppIconBox(button, root.frame);
                if (size.Width > 0 && size.Height > 0) icons.push_back(size);
            }
            if (!icons.empty()) {
                std::sort(icons.begin(), icons.end(), [](auto a, auto b) { return a.Height < b.Height; });
                auto size = icons[icons.size() / 2];
                result.iconWidth = size.Width;
                result.iconHeight = size.Height;
                result.iconMin = icons.front().Height;
                result.iconMax = icons.back().Height;
                result.iconMeasured = true;
            }
        } catch (...) {
            Wh_Log(L"[UWSET] taskbar metrics unavailable hr=%08X", winrt::to_hresult().value);
        }
        break;
    }
    return result;
}

void CollectTrayIconImages(
    FrameworkElement const& parent,
    std::vector<std::pair<FrameworkElement, Controls::Image>>& result,
    unsigned depth = 16) {
    if (!parent || !depth) return;
    int count = Media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; ++i) {
        auto child = Media::VisualTreeHelper::GetChild(parent, i)
                         .try_as<FrameworkElement>();
        if (!child) continue;
        if (child.Name() == L"NotifyItemIcon") {
            if (auto image = FindImage(child)) result.emplace_back(child, image);
            continue;
        }
        CollectTrayIconImages(child, result, depth - 1);
    }
}

// Versioned WinRT contracts, checked against SystemTray.dll's matching symbols.
// QueryInterface gates every call. Persistent IDs additionally use the symbol-
// checked registration field above, not the temporary configuration GUID.
constexpr GUID kTrayFrameIid{0x0a62af9e,0x208e,0x5bd8,{0x83,0xad,0x65,0x6b,0xca,0x42,0x3c,0x7b}};
constexpr GUID kTrayViewIid{0xcbdd88d7,0x22a3,0x58d8,{0x8b,0xf3,0x1c,0xd6,0x14,0x09,0xed,0x9a}};
constexpr GUID kTrayViewModelIid{0x87e53d34,0xbc57,0x586d,{0xb4,0x33,0xc0,0xfd,0x65,0xb6,0x61,0x85}};
constexpr GUID kTrayConfigurationIid{0xe1ebd49a,0x19bb,0x5174,{0xaf,0x4e,0xff,0x26,0x62,0x85,0x5f,0xf3}};
constexpr GUID kTrayDataModelIid{0xddda4275,0xd8be,0x5efd,{0x9a,0x2c,0xea,0x0f,0xf6,0xd8,0x6a,0x03}};
constexpr GUID kTrayDataIid{0x7be2b075,0xd63d,0x5faa,{0xa0,0x7c,0x10,0x32,0xbd,0xa6,0x9c,0xea}};
constexpr GUID kNotifyDataModelIid{0x9ad44317,0xfbb6,0x5376,{0xa1,0x4e,0x2b,0xb4,0xfa,0x66,0xc7,0x65}};
constexpr GUID kNativeTrayIconIid{0x6130d2bf,0x470d,0x5859,{0xa5,0xaf,0x95,0x17,0xeb,0x7d,0x37,0xe7}};
constexpr GUID kNativeTrayNameIid{0xd76cad80,0x3103,0x54f4,{0x9a,0xbe,0x5f,0x04,0x7c,0xbf,0xe8,0x5c}};
using TrayInspectable = winrt::Windows::Foundation::IInspectable;

winrt::com_ptr<::IInspectable> TrayInterface(TrayInspectable const& object, GUID const& iid) {
    winrt::com_ptr<::IInspectable> result;
    if (!object) throw winrt::hresult_no_interface();
    winrt::check_hresult(reinterpret_cast<::IUnknown*>(winrt::get_abi(object))->QueryInterface(iid, result.put_void()));
    return result;
}

TrayInspectable TrayObject(TrayInspectable const& object, GUID const& iid, size_t slot) {
    auto queried = TrayInterface(object, iid);
    auto table = *reinterpret_cast<void***>(queried.get());
    TrayInspectable result{nullptr};
    winrt::check_hresult(reinterpret_cast<HRESULT(WINAPI*)(void*, void**)>(table[slot])(
        queried.get(), winrt::put_abi(result)));
    return result;
}

std::wstring TrayText(TrayInspectable const& data, size_t slot, GUID const& iid = kTrayDataIid) {
    auto queried = TrayInterface(data, iid);
    auto table = *reinterpret_cast<void***>(queried.get());
    winrt::hstring result;
    winrt::check_hresult(reinterpret_cast<HRESULT(WINAPI*)(void*, void**)>(table[slot])(
        queried.get(), winrt::put_abi(result)));
    return std::wstring(result);
}

std::wstring TrayConfigurationId(TrayInspectable const& configuration) {
    auto dataModel = TrayObject(configuration, kTrayConfigurationIid, 6);
    auto native = TrayObject(dataModel, kNotifyDataModelIid, 6); // UDKObject
    auto queried = TrayInterface(native, kNativeTrayIconIid);
    auto id = TrayPreferenceIdFromAbi(queried.get());
    if (!id) return {};
    auto key = std::to_wstring(id);
    if (!g_verifiedTrayIds.contains(id)) {
        HKEY registration{};
        auto path = L"Control Panel\\NotifyIconSettings\\" + key;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, path.c_str(), 0, KEY_QUERY_VALUE, &registration) != ERROR_SUCCESS)
            return {};
        RegCloseKey(registration);
        g_verifiedTrayIds.insert(id);
    }
    return key;
}

std::wstring TrayElementId(FrameworkElement const& element) {
    try {
        auto model = TrayObject(element, kTrayViewIid, 10); // ViewModel
        return TrayConfigurationId(TrayObject(model, kTrayViewModelIid, 6)); // Configuration
    } catch (...) { return {}; }
}

IconPixels SnapshotTrayBitmap(TrayInspectable const& image) {
    IconPixels result;
    if (!image) return result;
    if (auto bitmap = image.try_as<Media::Imaging::WriteableBitmap>(); bitmap &&
        bitmap.PixelWidth() > 0 && bitmap.PixelHeight() > 0 &&
        bitmap.PixelWidth() <= 512 && bitmap.PixelHeight() <= 512) {
        UINT count = bitmap.PixelWidth() * bitmap.PixelHeight();
        if (bitmap.PixelBuffer().Capacity() < count * 4) return result;
        BYTE* bytes{};
        winrt::check_hresult(bitmap.PixelBuffer().as<::Windows::Storage::Streams::IBufferByteAccess>()->Buffer(&bytes));
        result.width = bitmap.PixelWidth(); result.height = bitmap.PixelHeight();
        result.pixels.resize(count);
        memcpy(result.pixels.data(), bytes, count * 4);
    }
    return result;
}

IconPixels RegisteredTrayBitmap(std::wstring const& key) {
    auto path = L"Control Panel\\NotifyIconSettings\\" + key;
    DWORD size{};
    if (RegGetValueW(HKEY_CURRENT_USER, path.c_str(), L"IconSnapshot", RRF_RT_REG_BINARY, nullptr, nullptr, &size) != ERROR_SUCCESS ||
        !size || size > 1024 * 1024) return {};
    std::vector<BYTE> bytes(size);
    if (RegGetValueW(HKEY_CURRENT_USER, path.c_str(), L"IconSnapshot", RRF_RT_REG_BINARY,
                    nullptr, bytes.data(), &size) != ERROR_SUCCESS) return {};
    winrt::com_ptr<IWICImagingFactory> factory;
    winrt::check_hresult(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
        __uuidof(IWICImagingFactory), factory.put_void()));
    winrt::com_ptr<IWICStream> stream;
    winrt::check_hresult(factory->CreateStream(stream.put()));
    winrt::check_hresult(stream->InitializeFromMemory(bytes.data(), size));
    winrt::com_ptr<IWICBitmapDecoder> decoder;
    winrt::check_hresult(factory->CreateDecoderFromStream(stream.get(), nullptr, WICDecodeMetadataCacheOnLoad, decoder.put()));
    winrt::com_ptr<IWICBitmapFrameDecode> frame;
    winrt::check_hresult(decoder->GetFrame(0, frame.put()));
    return ReadIconPixels(factory.get(), frame.get());
}

void QueryTrayCatalog(HWND editor) {
    ActiveScope active;
    if (g_stopping || editor != g_editorWindow || GetCurrentThreadId() != g_ownerThread) return;
    std::vector<TrayCatalogItem> items;
    std::set<std::wstring> seen;
    bool available = false;
    bool missingIdentity = false;
    for (auto const& root : g_roots) {
        if (root.retired || !root.frame) continue;
        try {
            auto parent = Media::VisualTreeHelper::GetParent(root.frame).try_as<FrameworkElement>();
            auto tray = Child(parent, L"SystemTray.SystemTrayFrame", true);
            if (!tray) continue;
            for (auto slot : {13u, 14u}) { // NotificationAreaIcons / NotificationAreaOverflowIcons
                auto collection = TrayObject(tray, kTrayFrameIid, slot);
                if (!collection) continue;
                auto models = collection.as<winrt::Windows::Foundation::Collections::IVector<TrayInspectable>>();
                available = true;
                for (auto const& model : models) {
                    try {
                        auto configuration = winrt::get_class_name(model) == L"SystemTray.IconConfiguration"
                            ? model : TrayObject(model, kTrayViewModelIid, 6);
                        auto key = TrayConfigurationId(configuration);
                        if (key.empty()) { missingIdentity = true; continue; }
                        if (seen.contains(key)) continue;
                        auto dataModel = TrayObject(configuration, kTrayConfigurationIid, 6);
                        auto data = TrayObject(dataModel, kTrayDataModelIid, 14); // CurrentData
                        auto image = data ? TrayObject(data, kTrayDataIid, 10) : nullptr; // IconImageSource
                        auto label = data ? TrayText(data, 7) : std::wstring{}; // AltText
                        if (label.empty() && data) label = TrayText(data, 8); // ToolTipText
                        if (label.empty()) {
                            auto native = TrayObject(dataModel, kNotifyDataModelIid, 6);
                            label = TrayText(native, 6, kNativeTrayNameIid);
                        }
                        if (auto end = label.find_first_of(L"\r\n"); end != label.npos) label.resize(end);
                        if (label.empty()) label = L"Tray icon " + key;
                        auto pixels = SnapshotTrayBitmap(image);
                        if (pixels.pixels.empty()) {
                            try { pixels = RegisteredTrayBitmap(key); } catch (...) {}
                        }
                        items.push_back({key, label, std::move(pixels), slot == 14});
                        seen.insert(key);
                    } catch (...) {
                        Wh_Log(L"[UWTRAY] unavailable tray item hr=%08X", winrt::to_hresult().value);
                    }
                }
            }
        } catch (...) {
            Wh_Log(L"[UWTRAY] tray catalog unavailable hr=%08X", winrt::to_hresult().value);
        }
    }
    {
        std::lock_guard lock(g_trayCatalogMutex);
        g_trayCatalog = std::move(items);
        g_trayCatalogAvailable = available && !missingIdentity;
    }
    if (editor == g_editorWindow) PostMessageW(editor, kTrayCatalogMessage, 0, 0);
}

bool IsAttentionStateName(std::wstring_view name) {
    return name.find(L"RequestingAttention") != std::wstring_view::npos;
}

// The search box's panel reports one visual state group, but reading it
// dereferences null and crashes Explorer (windows-11-taskbar-styler skips the
// same elements: the "Icon and label" search box and the 2025 pill layout).
bool HasBrokenVisualStateGroups(FrameworkElement const& element) {
    constexpr std::pair<std::wstring_view, std::wstring_view> kBroken[]{
        {L"Taskbar.TaskListButtonPanel", L"Taskbar.SearchBoxLaunchListButton"},
        {L"SearchUx.SearchUI.SearchButtonRootGrid", L"SearchUx.SearchUI.SearchPillButton"}};
    auto name = winrt::get_class_name(element);
    for (auto [child, parent] : kBroken) {
        if (std::wstring_view(name) != child) continue;
        auto owner = Media::VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
        return owner && std::wstring_view(winrt::get_class_name(owner)) == parent;
    }
    return false;
}

// Observe the native groups once per button, including high contrast where the
// private brush controller is released. A changed template replaces this owner.
struct ButtonStateObserver {
    winrt::weak_ref<FrameworkElement> templateRoot;
    struct Group {
        VisualStateGroup element;
        VisualStateGroup::CurrentStateChanging_revoker changing;
        VisualStateGroup::CurrentStateChanged_revoker changed;
    };
    std::vector<Group> groups;

    void Attach(FrameworkElement const& element, uintptr_t owner, unsigned depth=4) {
        if (!element || !depth) return;
        if (!HasBrokenVisualStateGroups(element))
            for (auto const& group : VisualStateManager::GetVisualStateGroups(element)) {
                if (std::wstring_view(group.Name()).starts_with(L"UWPlate_")) continue;
                auto notify=[owner](auto const&, auto const&) { RequestItemStateRefresh(owner); };
                groups.push_back({group,group.CurrentStateChanging(winrt::auto_revoke,notify),
                    group.CurrentStateChanged(winrt::auto_revoke,notify)});
            }
        int count=Media::VisualTreeHelper::GetChildrenCount(element);
        for (int i=0;i<count;++i)
            Attach(Media::VisualTreeHelper::GetChild(element,i).try_as<FrameworkElement>(),owner,depth-1);
    }

    bool RequestingAttention() const {
        return std::any_of(groups.begin(),groups.end(),[](auto const& group) {
            auto state=group.element.CurrentState();
            return state && IsAttentionStateName(state.Name().c_str());
        });
    }
};

void ObserveButtonStates(Root::Source& source, FrameworkElement const& element) {
    auto templateRoot=Media::VisualTreeHelper::GetChildrenCount(element)
        ? Media::VisualTreeHelper::GetChild(element,0).try_as<FrameworkElement>() : nullptr;
    if (!source.states || source.states->templateRoot.get()!=templateRoot) {
        auto states=std::make_shared<ButtonStateObserver>();
        if(templateRoot) states->templateRoot=winrt::make_weak(templateRoot);
        states->Attach(element,source.identity);
        source.states=std::move(states);
    }
    source.requestingAttention=g_theme==DockTheme::Liquid && source.states->RequestingAttention();
    source.stateRefreshPending=false;
}

winrt::Windows::Foundation::IInspectable ReadPlateLocalValue(
    FrameworkElement const& element, DependencyProperty const& property,
    Data::Binding& binding) {
    binding=nullptr;
    auto value=element.ReadLocalValue(property);
    if(auto expression=value.try_as<Data::BindingExpression>())
        binding=expression.ParentBinding();
    else if(value && winrt::get_class_name(value)==L"Windows.UI.Xaml.Data.BindingExpressionBase") {
        // The template override path retains these bindings in XAML. Other
        // callers can only retain the base value of this opaque expression.
        value=element.GetAnimationBaseValue(property);
    }
    return value;
}

// Visual-state setters preserve underlying bindings/local values while native
// taskbar states change the effective appearance. SetValue detaches bindings.
struct PlateTemplateValues {
    Controls::Border element{nullptr};
    Controls::Control control{nullptr};
    FrameworkElement root{nullptr};
    VisualStateGroup group{nullptr};
    VisualState active{nullptr};
    std::wstring activeName, releasedName;
    std::array<Setter,8> setters{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
    std::array<winrt::Windows::Foundation::IInspectable,8> locals{};
    std::array<bool,8> managed{};

    static DependencyProperty Property(size_t i) {
        switch(i) {
        case 0: return Controls::Border::CornerRadiusProperty();
        case 1: return FrameworkElement::MarginProperty();
        case 2: return FrameworkElement::HeightProperty();
        case 3: return FrameworkElement::VerticalAlignmentProperty();
        case 4: return Controls::Border::BackgroundProperty();
        case 5: return Controls::Border::BorderBrushProperty();
        case 6: return FrameworkElement::MaxHeightProperty();
        default: return FrameworkElement::MinHeightProperty();
        }
    }
    static std::shared_ptr<PlateTemplateValues> Create(Controls::Border const& element) {
        std::array<bool,8> managed{};
        for(size_t i=0;i<8;++i) {
            auto local=element.ReadLocalValue(Property(i));
            managed[i]=i<4 || i>=6 || !local || winrt::get_class_name(local)==L"Windows.UI.Xaml.Data.BindingExpressionBase";
        }
        auto parent=Media::VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
        Controls::Control control{nullptr};
        for(unsigned depth=0;parent && depth<16;++depth) {
            if((control=parent.try_as<Controls::Control>())) break;
            parent=Media::VisualTreeHelper::GetParent(parent).try_as<FrameworkElement>();
        }
        if(!control || !Media::VisualTreeHelper::GetChildrenCount(control)) return {};
        auto root=Media::VisualTreeHelper::GetChild(control,0).as<FrameworkElement>();
        // A distant ScrollViewer/ContentControl does not own the states of
        // arbitrary content. Only use the named part's actual template owner.
        auto templateOwner=control.try_as<Controls::IControlProtected>();
        if(!templateOwner || templateOwner.GetTemplateChild(element.Name())!=element ||
           HasBrokenVisualStateGroups(root)) return {};
        auto result=std::make_shared<PlateTemplateValues>();
        result->managed=managed;
        result->element=element; result->control=control;
        result->root=root;
        auto name=L"UWPlate_"+std::to_wstring(CanonicalIdentity(element));
        result->activeName=name+L"Active"; result->releasedName=name+L"Released";
        result->group=Markup::XamlReader::Load(
            L"<VisualStateGroup xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" "
            L"xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\" x:Name=\""+name+
            L"\"><VisualState x:Name=\""+result->activeName+L"\"/><VisualState x:Name=\""+
            result->releasedName+L"\"/></VisualStateGroup>").as<VisualStateGroup>();
        result->active=result->group.States().GetAt(0);
        VisualStateManager::GetVisualStateGroups(result->root).Append(result->group);
        return result;
    }
    void Apply(size_t i, winrt::Windows::Foundation::IInspectable const& value) {
        // Re-enter to take precedence after a native group changes the same
        // property. GoToState(active) alone leaves an unchanged setter behind.
        if(active.Setters().Size() && !VisualStateManager::GoToState(control,releasedName,false))
            winrt::throw_hresult(E_FAIL);
        if(!setters[i]) {
            locals[i]=element.ReadLocalValue(Property(i));
            static constexpr PCWSTR names[]{L"CornerRadius",L"Margin",L"Height",
                L"VerticalAlignment",L"Background",L"BorderBrush",L"MaxHeight",L"MinHeight"};
            Setter setter; TargetPropertyPath target;
            target.Target(element); target.Path(PropertyPath(names[i]));
            setter.Target(target); setters[i]=setter;
            active.Setters().Append(setter);
        }
        setters[i].Value(value);
        if(!VisualStateManager::GoToState(control,activeName,false)) winrt::throw_hresult(E_FAIL);
    }
    void Remove(size_t i) {
        if(!setters[i]) return;
        // A same-state update does not release removed setters. Leave first,
        // then reapply the remaining overrides in this UI-thread operation.
        if(!VisualStateManager::GoToState(control,releasedName,false)) winrt::throw_hresult(E_FAIL);
        uint32_t index{};
        if(active.Setters().IndexOf(setters[i],index)) active.Setters().RemoveAt(index);
        setters[i]=nullptr; locals[i]=nullptr;
        if(active.Setters().Size() && !VisualStateManager::GoToState(control,activeName,false))
            winrt::throw_hresult(E_FAIL);
    }
    bool LocalUnchanged(size_t i) const {
        if(!setters[i]) return true;
        auto current=element.ReadLocalValue(Property(i));
        if(current==locals[i]) return true;
        auto templateBinding=[](auto const& value) {
            return !value || (winrt::get_class_name(value)==L"Windows.UI.Xaml.Data.BindingExpressionBase" &&
                              !value.template try_as<Data::BindingExpression>());
        };
        if(templateBinding(current) && templateBinding(locals[i])) return true;
        if(!current || !locals[i]) return false;
        try {
            switch(i) {
            case 0: return winrt::unbox_value<CornerRadius>(current)==winrt::unbox_value<CornerRadius>(locals[i]);
            case 1: return winrt::unbox_value<Thickness>(current)==winrt::unbox_value<Thickness>(locals[i]);
            case 2: {
                auto a=winrt::unbox_value<double>(current),b=winrt::unbox_value<double>(locals[i]);
                return a==b || (std::isnan(a) && std::isnan(b));
            }
            case 6: case 7: return winrt::unbox_value<double>(current)==winrt::unbox_value<double>(locals[i]);
            case 3: return winrt::unbox_value<VerticalAlignment>(current)==winrt::unbox_value<VerticalAlignment>(locals[i]);
            default: return false;
            }
        } catch (...) { return false; }
    }
    ~PlateTemplateValues() {
        if(!group) return;
        try {
            VisualStateManager::GoToState(control,releasedName,false);
            uint32_t index{};
            auto groups=VisualStateManager::GetVisualStateGroups(root);
            if(groups.IndexOf(group,index)) groups.RemoveAt(index);
        } catch (...) {
            Wh_Log(L"[UWITEM] template overrides release failed hr=%08X",winrt::to_hresult().value);
        }
    }
};

void EnsurePlateTemplate(Root::StatePlate& state) {
    if(state.templateChecked) return;
    state.templateValues=PlateTemplateValues::Create(state.element);
    state.templateChecked=true;
}

// VSM opacity animations can replace the handout visual's opacity too. Keep the
// multiplier on private brushes instead. Their properties stay bound to the
// native brush: VSM can change a named brush without replacing Border.Background.
// Owner-thread data only: queued messages contain no DLL-owned delegates.
std::vector<std::weak_ptr<NativePlateFill>> g_pendingPlateFills;
bool g_plateFillMessagePending{};

struct NativePlateFill : std::enable_shared_from_this<NativePlateFill> {
    Controls::Border element{nullptr};
    std::shared_ptr<PlateTemplateValues> templateValues;
    std::array<Media::Brush,2> source{nullptr,nullptr}, paint{nullptr,nullptr};
    std::array<Media::Brush,2> animatedPaint{nullptr,nullptr};
    std::array<VisualStateGroup,2> brushStateOwner{nullptr,nullptr};
    std::array<winrt::Windows::Foundation::IInspectable,2> original{nullptr,nullptr};
    std::array<Data::Binding,2> binding{nullptr,nullptr};
    std::array<int64_t,2> tokens{-1,-1};
    std::array<int64_t,2> opacityTokens{-1,-1};
    std::vector<std::pair<VisualStateGroup,VisualStateGroup::CurrentStateChanging_revoker>> stateChanging;
    float factor{1};
    bool borders{true};
    bool updating{};
    bool refreshPending{};
    bool refreshSucceeded{};
    static DependencyProperty Property(size_t i) {
        return i ? Controls::Border::BorderBrushProperty() : Controls::Border::BackgroundProperty();
    }
    static Media::Brush Copy(Media::Brush const& brush) {
        if (!brush) return nullptr;
        auto follow=[](DependencyObject const& target, DependencyObject const& from,
                       DependencyProperty const& property, PCWSTR path) {
            Data::Binding binding;
            binding.Source(from); binding.Path(PropertyPath(path));
            binding.Mode(Data::BindingMode::OneWay);
            Data::BindingOperations::SetBinding(target,property,binding);
        };
        Media::Brush result{nullptr};
        if (auto solid=brush.try_as<Media::SolidColorBrush>()) {
            result=Media::SolidColorBrush();
            follow(result,solid,Media::SolidColorBrush::ColorProperty(),L"Color");
        } else if (auto gradient=brush.try_as<Media::LinearGradientBrush>()) {
            Media::LinearGradientBrush copy;
            follow(copy,gradient,Media::LinearGradientBrush::StartPointProperty(),L"StartPoint");
            follow(copy,gradient,Media::LinearGradientBrush::EndPointProperty(),L"EndPoint");
            follow(copy,gradient,Media::GradientBrush::MappingModeProperty(),L"MappingMode");
            follow(copy,gradient,Media::GradientBrush::SpreadMethodProperty(),L"SpreadMethod");
            follow(copy,gradient,Media::GradientBrush::ColorInterpolationModeProperty(),L"ColorInterpolationMode");
            for (auto const& stop:gradient.GradientStops()) {
                Media::GradientStop value;
                follow(value,stop,Media::GradientStop::ColorProperty(),L"Color");
                follow(value,stop,Media::GradientStop::OffsetProperty(),L"Offset");
                copy.GradientStops().Append(value);
            }
            result=copy;
        } else {
            // Unsupported native materials fall through to the dock's glass.
            result=Media::SolidColorBrush({0,0,0,0});
        }
        follow(result,brush,Media::Brush::TransformProperty(),L"Transform");
        follow(result,brush,Media::Brush::RelativeTransformProperty(),L"RelativeTransform");
        return result;
    }
    void UnwatchOpacity(size_t i) {
        if (source[i] && opacityTokens[i]>=0)
            source[i].UnregisterPropertyChangedCallback(Media::Brush::OpacityProperty(),opacityTokens[i]);
        opacityTokens[i]=-1;
    }
    void RestoreProperty(size_t i) {
        UnwatchOpacity(i);
        auto local=element.ReadLocalValue(Property(i));
        auto current=element.GetValue(Property(i));
        if(templateValues && templateValues->managed[i+4]) {
            templateValues->Remove(i+4);
        } else if (paint[i] && (local==paint[i] || current==paint[i] ||
                        (animatedPaint[i] && (local==animatedPaint[i] || current==animatedPaint[i])))) {
            if (binding[i]) element.SetBinding(Property(i),binding[i]);
            else if (original[i]==DependencyProperty::UnsetValue()) element.ClearValue(Property(i));
            else element.SetValue(Property(i),original[i]);
        }
        animatedPaint[i]=nullptr; paint[i]=nullptr; source[i]=nullptr; original[i]=nullptr; binding[i]=nullptr;
    }
    void Refresh() {
        if (updating || !element) return;
        refreshSucceeded=false;
        updating=true;
        struct Reset { bool& value; ~Reset(){value=false;} } reset{updating};
        for (size_t i=0;i<2;++i) {
            auto property=Property(i);
            bool templateProperty=templateValues && templateValues->managed[i+4];
            bool outline=i==1 || element.Name()==L"BackgroundBorder";
            double multiplier=factor==0 ? 0 : outline ? (borders ? 1 : 0) : factor;
            if (multiplier==1) {
                RestoreProperty(i);
                continue;
            }
            auto current=element.GetValue(property).try_as<Media::Brush>();
            bool animated=paint[i] && std::any_of(stateChanging.begin(),stateChanging.end(),
                [&](auto const& watch){return TouchesBrush(watch.first.CurrentState(),i,false);});
            if(templateProperty && !animated && current==paint[i] && !brushStateOwner[i]) {
                auto base=element.GetAnimationBaseValue(property).try_as<Media::Brush>();
                if(base!=source[i]) current=base;
            }
            if(animatedPaint[i] && !animated) {
                if(current==animatedPaint[i]) {
                    if(templateProperty) templateValues->Apply(i+4,paint[i]);
                    else element.SetValue(property,paint[i]);
                    current=paint[i];
                }
                animatedPaint[i]=nullptr;
            }
            auto local=element.ReadLocalValue(property);
            if (animated && current && current!=paint[i]) {
                // XAML animates a copy of the local brush. Replacing that copy
                // detaches its animation and compounds our opacity multiplier.
                animatedPaint[i]=current;
                if(source[i]) {
                    double opacity=source[i].Opacity()*multiplier;
                    if(std::abs(current.Opacity()-opacity)>.001) current.Opacity(opacity);
                }
            } else if (current!=paint[i]) {
                UnwatchOpacity(i);
                if(source[i] && current!=source[i]) brushStateOwner[i]=nullptr;
                if(local!=paint[i]) {
                    original[i]=ReadPlateLocalValue(element,property,binding[i]);
                }
                source[i]=current;
                if(templateProperty && !brushStateOwner[i])
                    for(auto const& watch:stateChanging)
                        if(TouchesBrush(watch.first.CurrentState(),i,true,watch.first)) {
                            // CurrentStateChanging can precede applying its brush.
                            // Resolve its owner once the deferred refresh sees it.
                            brushStateOwner[i]=watch.first;
                            break;
                        }
                paint[i]=Copy(current);
                if (paint[i]) {
                    opacityTokens[i]=source[i].RegisterPropertyChangedCallback(Media::Brush::OpacityProperty(),
                        [weak=weak_from_this()](auto const&,auto const&) {
                            if(auto self=weak.lock()) {
                                try { self->Refresh(); }
                                catch (...) { Wh_Log(L"[UWITEM] fill opacity refresh failed hr=%08X",winrt::to_hresult().value); }
                            }
                        });
                    paint[i].Opacity(source[i].Opacity()*multiplier);
                    if(templateProperty) templateValues->Apply(i+4,paint[i]);
                    else element.SetValue(property,paint[i]);
                } else if(templateProperty) {
                    templateValues->Remove(i+4);
                }
            }
            if (paint[i] && source[i]) {
                double opacity=source[i].Opacity()*multiplier;
                if (std::abs(paint[i].Opacity()-opacity)>.001) paint[i].Opacity(opacity);
            }
        }
        refreshSucceeded=true;
    }
    void ScheduleRefresh() {
        if(updating || refreshPending || !element || g_stopping ||
           !g_dispatch || GetCurrentThreadId()!=g_ownerThread) return;
        try {
            if(!g_plateFillMessagePending) {
                if(!PostMessageW(g_dispatch,kRefreshPlateFillsMessage,0,0))
                    winrt::throw_last_error();
                g_plateFillMessagePending=true;
            }
            g_pendingPlateFills.emplace_back(weak_from_this());
            refreshPending=true;
        } catch (...) {
            refreshSucceeded=false;
            Wh_Log(L"[UWITEM] fill refresh failed hr=%08X",winrt::to_hresult().value);
        }
    }
    bool TouchesBrush(VisualState const& state, size_t i, bool whole,
                      VisualStateGroup const& group=nullptr) const {
        if(!state) return false;
        auto matches=[i,whole](std::wstring_view path) {
            if(!whole)
                return i ? path.starts_with(L"BorderBrush.") || path.starts_with(L"(Border.BorderBrush).")
                         : path.starts_with(L"Background.") || path.starts_with(L"(Border.Background).");
            return i ? path==L"BorderBrush" || path.ends_with(L".BorderBrush)")
                     : path==L"Background" || path.ends_with(L".Background)");
        };
        auto name=element.Name();
        for(auto const& value:state.Setters()) {
            auto setter=value.try_as<Setter>();
            if(!setter) continue;
            auto target=setter.Target();
            if(!target || !target.Path()) continue;
            if(!matches(target.Path().Path().c_str())) continue;
            // Named XAML setter targets don't expose Target through this API.
            // Match the actual brush value and remember which group supplied it.
            auto brush=setter.Value().try_as<Media::Brush>();
            if(brush && (brush==element.GetValue(Property(i)) ||
                         ((!brushStateOwner[i] || group==brushStateOwner[i]) && brush==source[i]))) return true;
        }
        auto animationTargets=[&](auto&& self, animation::Timeline const& timeline)->bool {
            if(!timeline) return false;
            if(auto storyboard=timeline.try_as<animation::Storyboard>())
                for(auto const& child:storyboard.Children())
                    if(self(self,child)) return true;
            return animation::Storyboard::GetTargetName(timeline)==name &&
                   matches(animation::Storyboard::GetTargetProperty(timeline).c_str());
        };
        return animationTargets(animationTargets,state.Storyboard());
    }
    void Attach(Controls::Border const& plate) {
        element=plate;
        // Restore a whole-brush override when its group leaves it. The incoming
        // value may already be applied when CurrentStateChanging is delivered.
        // Color/opacity animations must retain the brush they already target.
        auto parent=Media::VisualTreeHelper::GetParent(plate).try_as<FrameworkElement>();
        for(unsigned depth=0;parent && depth<16;++depth) {
            if(!HasBrokenVisualStateGroups(parent))
                for(auto const& group:VisualStateManager::GetVisualStateGroups(parent)) {
                    if(std::wstring_view(group.Name()).starts_with(L"UWPlate_")) continue;
                    stateChanging.emplace_back(group,group.CurrentStateChanging(winrt::auto_revoke,
                        [weak=weak_from_this(),groupWeak=winrt::make_weak(group)](auto const&,VisualStateChangedEventArgs const& args) {
                            if(auto self=weak.lock()) {
                                if(self->updating || !self->element) return;
                                self->updating=true;
                                try {
                                    auto group=groupWeak.get();
                                    for(size_t i=0;i<2;++i) {
                                        if(self->templateValues && self->templateValues->managed[i+4] &&
                                           self->TouchesBrush(args.NewState(),i,false)) {
                                            // XAML promotes the animated brush to a local value,
                                            // detaching TemplateBinding even without the mod.
                                            // Release our setter so it cannot mask that animation.
                                            self->templateValues->Remove(i+4);
                                            self->templateValues->managed[i+4]=false;
                                            self->original[i]=self->source[i];
                                            self->binding[i]=nullptr;
                                        }
                                        bool entering=self->TouchesBrush(args.NewState(),i,true,group);
                                        bool leaving=self->TouchesBrush(args.OldState(),i,true,group);
                                        if(entering) {
                                            self->brushStateOwner[i]=group;
                                        } else if(leaving) {
                                            self->RestoreProperty(i);
                                            self->brushStateOwner[i]=nullptr;
                                        }
                                    }
                                }
                                catch (...) { Wh_Log(L"[UWITEM] state fill restore failed hr=%08X",winrt::to_hresult().value); }
                                self->updating=false;
                                self->ScheduleRefresh();
                            }
                        }));
                }
            if(parent.try_as<Controls::Control>()) break;
            parent=Media::VisualTreeHelper::GetParent(parent).try_as<FrameworkElement>();
        }
        for (size_t i=0;i<2;++i)
            tokens[i]=element.RegisterPropertyChangedCallback(Property(i),
                [weak=weak_from_this()](auto const&,auto const&) {
                    if(auto self=weak.lock()) self->ScheduleRefresh();
                });
        Refresh();
    }
    void Restore() noexcept {
        if (!element) return;
        updating=true;
        stateChanging.clear();
        for (size_t i=0;i<2;++i) {
            try {
                if (tokens[i]>=0) element.UnregisterPropertyChangedCallback(Property(i),tokens[i]);
                tokens[i]=-1;
                RestoreProperty(i);
            } catch (...) { Wh_Log(L"[UWITEM] fill restore failed hr=%08X",winrt::to_hresult().value); }
        }
        element=nullptr;
    }
    ~NativePlateFill(){Restore();}
};

void RefreshQueuedPlateFills() {
    ActiveScope active;
    g_plateFillMessagePending=false;
    auto pending=std::exchange(g_pendingPlateFills,{});
    for(auto const& weak:pending) if(auto fill=weak.lock()) {
        fill->refreshPending=false;
        if(g_stopping) continue;
        try { fill->Refresh(); }
        catch (...) { Wh_Log(L"[UWITEM] fill refresh failed hr=%08X",winrt::to_hresult().value); }
    }
}

void RestorePlateFill(Root::StatePlate& state) {
    if (state.fill) state.fill->Restore();
    state.fill.reset();
    state.fillHidden=false;
}

void ApplyPlateFill(Root::StatePlate& state, float factor) {
    if (!state.fill) {
        EnsurePlateTemplate(state);
        state.fill=std::make_shared<NativePlateFill>();
        state.fill->templateValues=state.templateValues;
        state.fill->factor=factor;
        state.fill->borders=g_keepButtonRim;
        state.fill->Attach(state.element);
    } else if (!state.fill->refreshSucceeded || state.fill->factor!=factor ||
               state.fill->borders!=g_keepButtonRim) {
        // Native changes are already observed by the brush/VSM callbacks.
        // Reapply only a changed policy or a previously failed brush update.
        state.fill->factor=factor;
        state.fill->borders=g_keepButtonRim;
        state.fill->Refresh();
    }
    state.fillHidden = factor == 0;
}

void ApplyPlateAppearance(Root::StatePlate& state, bool glass, bool highContrast) {
    if(state.trayHighlight) return; // Native tray brushes/animations are the highlight, not app chrome.
    // Like tray highlights, ordinary app fills keep their native color/opacity.
    // Only an explicit border switch or accepted liquid lens needs brush overrides.
    if(highContrast || (!glass && g_keepButtonRim)) RestorePlateFill(state);
    else ApplyPlateFill(state,ButtonFillOpacity(glass));
}

std::array<DependencyProperty,3> PlateLayoutProperties() {
    return {FrameworkElement::MarginProperty(),FrameworkElement::HeightProperty(),
            FrameworkElement::VerticalAlignmentProperty()};
}

void RestoreStatePlate(Root::StatePlate& state) {
    if (!state.element) return;
    state.trayStates.reset();
    state.stateRefreshPending=false;
    RestorePlateFill(state);
    if(state.templateValues) for(size_t i:{6u,7u}) {
        try {state.templateValues->Remove(i);}
        catch (...) {Wh_Log(L"[UWITEM] tray height limit restore failed");}
    }
    // A rejected property restore must not strand other owned properties,
    // the composition clip or the rim. Each attempt keeps its ownership check.
    try {
        if(state.templateValues && state.templateValues->managed[0]) {
            state.templateValues->Remove(0);
        } else if (state.cornersOwned && state.element.CornerRadius() == state.appliedCorners) {
            if (state.originalCornerBinding)
                state.element.SetBinding(Controls::Border::CornerRadiusProperty(), state.originalCornerBinding);
            else if (state.originalCorners == DependencyProperty::UnsetValue())
                state.element.ClearValue(Controls::Border::CornerRadiusProperty());
            else
                state.element.SetValue(Controls::Border::CornerRadiusProperty(), state.originalCorners);
        }
    } catch (...) {
        Wh_Log(L"[UWITEM] state plate corners restore failed hr=%08X",
               winrt::to_hresult().value);
    }
    if (state.layoutOwned) {
        for(size_t i=0;i<state.originalLayout.size();++i) {
            try {
                if(state.templateValues && state.templateValues->managed[i+1]) {
                    state.templateValues->Remove(i+1);
                    continue;
                }
                auto property=PlateLayoutProperties()[i];
                bool matches=i==0 ? state.element.Margin()==state.appliedMargin :
                             i==1 ? state.element.Height()==state.appliedHeight :
                                    state.element.VerticalAlignment()==VerticalAlignment::Top;
                if(!matches) continue;
                if(state.layoutBindings[i]) state.element.SetBinding(property,state.layoutBindings[i]);
                else if(state.originalLayout[i]==DependencyProperty::UnsetValue()) state.element.ClearValue(property);
                else state.element.SetValue(property,state.originalLayout[i]);
            } catch (...) {
                Wh_Log(L"[UWITEM] state plate layout[%zu] restore failed hr=%08X",
                       i,winrt::to_hresult().value);
            }
        }
    }
    try {
        bool clipMatch =
            state.clip && state.visual && state.visual.Clip() == state.clip;
        if (state.clip) {
            if (clipMatch)
                state.visual.Clip(nullptr);
            else
                Wh_Log(L"[UWITEM] state plate clip ownership lost");
        }
    } catch (...) {
        Wh_Log(L"[UWITEM] state plate clip restore failed hr=%08X",
               winrt::to_hresult().value);
    }
    try {
        if (state.rim) {
            if (Hosting::ElementCompositionPreview::GetElementChildVisual(
                    state.element) == state.rim)
                Hosting::ElementCompositionPreview::SetElementChildVisual(
                    state.element, nullptr);
            else
                Wh_Log(L"[UWITEM] state plate rim ownership lost");
        }
    } catch (...) {
        Wh_Log(L"[UWITEM] state plate rim restore failed hr=%08X",
               winrt::to_hresult().value);
    }
    state.cornersOwned = false;
    state.originalCorners = nullptr;
    state.originalCornerBinding = nullptr;
    state.layoutOwned=false;
    state.originalLayout={nullptr,nullptr,nullptr};
    state.layoutBindings={nullptr,nullptr,nullptr};
    state.rim = nullptr;
    state.rimThemeReady = false;
    state.visual = nullptr;
    state.clip = nullptr;
    state.clipGeometry = nullptr;
    state.clipBounds = nullptr;
    state.buttonOwner = 0;
    state.templateValues.reset();
    state.templateChecked=false;
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

// Replicates the native rim, measured off a live taskbar.
ShapeVisual ButtonRim(CompositionRoundedRectangleGeometry const& outline,
                      bool dark) {
    auto compositor = outline.Compositor();
    auto visual = compositor.CreateShapeVisual();
    visual.RelativeSizeAdjustment({1, 1});
    auto colors=ButtonRimColors(dark);
    auto gradient = compositor.CreateLinearGradientBrush();
    gradient.StartPoint({0, 0});
    gradient.EndPoint({0, 1});
    gradient.ColorStops().Append(compositor.CreateColorGradientStop(
        0, colors[0]));
    gradient.ColorStops().Append(compositor.CreateColorGradientStop(
        1, colors[1]));
    auto shape = compositor.CreateSpriteShape(outline);
    shape.StrokeBrush(gradient);
    shape.StrokeThickness(2);  // Outer half is clipped, leaving a 1-DIP rim.
    visual.Shapes().Append(shape);
    return visual;
}

void PrepareStatePlate(Root& root, Root::StatePlate& state,
                       DockInsets insets, bool glassLayer) {
    if (state.blocked || !state.element) return;
    auto plate = state.element;
    try {
        auto parent = Media::VisualTreeHelper::GetParent(plate)
                          .try_as<FrameworkElement>();
        if (!parent) return;
        EnsurePlateTemplate(state);
        if(state.trayHighlight && !state.templateValues) return;
        // Radius, layout and the glass clip describe one shape. Yield the plate
        // if another writer changes it, before replacing our applied snapshots
        // or resetting them on a glass/theme transition.
        auto geometryChanged=[&](size_t i,bool changed) {
            // Tray highlight controls deliberately take precedence over the
            // native/local size policies used by different tray providers.
            // VSM setters retain their latest underlying values for cleanup.
            if(state.trayHighlight) return false;
            return state.templateValues ? !state.templateValues->LocalUnchanged(i) : changed;
        };
        if ((state.cornersOwned && geometryChanged(0,plate.CornerRadius()!=state.appliedCorners)) ||
            (state.layoutOwned && (geometryChanged(1,plate.Margin()!=state.appliedMargin) ||
                geometryChanged(2,plate.Height()!=state.appliedHeight) ||
                geometryChanged(3,plate.VerticalAlignment()!=VerticalAlignment::Top)))) {
            BlockStatePlate(state);
            Wh_Log(L"[UWITEM] external state plate geometry retained");
            return;
        }
        bool highContrast = HighContrastActive();
        if ((!glassLayer || highContrast) && (state.clip || state.fillHidden)) {
            auto owner=state.buttonOwner;
            RestoreStatePlate(state);
            state.element=plate;
            state.buttonOwner=owner;
            EnsurePlateTemplate(state);
        }
        // Rounding is a native Border property; do not crop or stretch ordinary
        // hover/active plates just to change their corner radius.
        if (!state.cornersOwned) {
            state.originalCorners = ReadPlateLocalValue(
                plate,Controls::Border::CornerRadiusProperty(),state.originalCornerBinding);
            state.cornersOwned = true;
        }
        double corner=state.trayHighlight?g_trayHighlightRadius:g_buttonCornerRadius;
        state.appliedCorners = {corner,corner,corner,corner};
        if (plate.CornerRadius() != state.appliedCorners ||
            (state.templateValues && !state.templateValues->setters[0])) {
            if(state.templateValues && state.templateValues->managed[0])
                state.templateValues->Apply(0,winrt::box_value(state.appliedCorners));
            else plate.CornerRadius(state.appliedCorners);
        }
        // Keep native fills intact unless an accepted liquid lens replaces them.
        // Border visibility follows its separate explicit control.
        ApplyPlateAppearance(state,state.fillHidden,highContrast);
        // Size the background in its native layout slot. Its owner, icons,
        // horizontal margins and native transforms are untouched.
        auto transform=root.dock.background.TransformToVisual(parent);
        auto slot=Controls::Primitives::LayoutInformation::GetLayoutSlot(plate);
        if(slot.Height<=0) return;
        double top=transform.TransformPoint({0,float(insets.top)}).Y-slot.Y;
        double bottom=transform.TransformPoint(
            {0,float(root.dock.background.ActualHeight()-insets.bottom)}).Y-slot.Y;
        if(state.trayHighlight) {
            if(plate.MaxHeight()!=INFINITY || !state.templateValues->setters[6])
                state.templateValues->Apply(6,winrt::box_value(double(INFINITY)));
            if(plate.MinHeight()!=0 || !state.templateValues->setters[7])
                state.templateValues->Apply(7,winrt::box_value(0.0));
        } else {
            top=std::clamp(top,0.0,double(slot.Height));
            bottom=std::clamp(bottom,top,double(slot.Height));
        }
        if(bottom-top<1) return;
        if(!state.layoutOwned) {
            auto properties=PlateLayoutProperties();
            for(size_t i=0;i<properties.size();++i) {
                state.originalLayout[i]=ReadPlateLocalValue(plate,properties[i],state.layoutBindings[i]);
            }
            state.layoutOwned=true;
        }
        auto margin=plate.Margin();
        // Keep the whole native slot occupied. Dropping the bottom remainder
        // shrinks auto-sized parents, feeding their changed position back into
        // the next inset calculation and repeatedly invalidating XAML layout.
        // XAML stores these dimensions as floats although WinRT exposes doubles.
        // Cache that same precision: a double subtraction can otherwise differ
        // from every readback and restart layout indefinitely.
        state.appliedMargin={margin.Left,float(top),margin.Right,float(double(slot.Height)-bottom)};
        state.appliedHeight=float(bottom-top);
        if(plate.Margin()!=state.appliedMargin ||
           (state.templateValues && !state.templateValues->setters[1])) {
            if(state.templateValues && state.templateValues->managed[1])
                state.templateValues->Apply(1,winrt::box_value(state.appliedMargin));
            else plate.Margin(state.appliedMargin);
        }
        if(plate.Height()!=state.appliedHeight ||
           (state.templateValues && !state.templateValues->setters[2])) {
            if(state.templateValues && state.templateValues->managed[2])
                state.templateValues->Apply(2,winrt::box_value(state.appliedHeight));
            else plate.Height(state.appliedHeight);
        }
        if(plate.VerticalAlignment()!=VerticalAlignment::Top ||
           (state.templateValues && !state.templateValues->setters[3])) {
            if(state.templateValues && state.templateValues->managed[3])
                state.templateValues->Apply(3,winrt::box_value(VerticalAlignment::Top));
            else plate.VerticalAlignment(VerticalAlignment::Top);
        }
        if(state.rim && !g_keepButtonRim) {
            if(Hosting::ElementCompositionPreview::GetElementChildVisual(plate)==state.rim)
                Hosting::ElementCompositionPreview::SetElementChildVisual(plate,nullptr);
            state.rim=nullptr; state.rimThemeReady=false;
        }
        if (!glassLayer || highContrast) return;
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
        auto rect = PlanVerticalClip(
            plate.ActualWidth(), plate.ActualHeight(), 0, plate.ActualHeight());
        if (rect.Width <= 0 || rect.Height <= 0) return;
        if (!state.clip) {
            state.visual =
                Hosting::ElementCompositionPreview::GetElementVisual(plate);
            if (state.visual.Clip()) {
                return;
            }
            state.clipGeometry =
                state.visual.Compositor().CreateRoundedRectangleGeometry();
            state.clip = state.visual.Compositor().CreateGeometricClip(
                state.clipGeometry);
            state.visual.Clip(state.clip);
            state.clipBounds = state.visual.Compositor().CreatePropertySet();
            state.clipBounds.InsertScalar(L"height",rect.Height);
            auto size = state.visual.Compositor().CreateExpressionAnimation(
                L"Vector2(Max(0, plate.Size.X), bounds.height)");
            size.SetReferenceParameter(L"plate",state.visual);
            size.SetReferenceParameter(L"bounds",state.clipBounds);
            state.clipGeometry.StartAnimation(L"Size",size);
        }
        // Only the fill plate. BackgroundBorder carries the same native gradient on
        // its own control, and a control holding both would otherwise get two
        // stacked rims. Never take a child visual someone else is using.
        ElementTheme rimTheme = plate.ActualTheme();
        if (state.rim && state.rimThemeReady && state.rimTheme != rimTheme) {
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
                                  rimTheme == ElementTheme::Dark);
            state.rimTheme = rimTheme;
            state.rimThemeReady = true;
            Hosting::ElementCompositionPreview::SetElementChildVisual(
                plate, state.rim);
        }
        float radius = static_cast<float>(ButtonClipRadius(rect.Width,rect.Height,g_buttonCornerRadius));
        state.clipGeometry.Offset({rect.X, rect.Y});
        if (state.clipBounds) state.clipBounds.InsertScalar(L"height",rect.Height);
        state.clipGeometry.Size({rect.Width, rect.Height});
        state.clipGeometry.CornerRadius({radius,radius});
    } catch (...) {
        BlockStatePlate(state);
        Wh_Log(L"[UWITEM] state plate unavailable hr=%08X",
               winrt::to_hresult().value);
    }
}

void PrepareStatePlates(Root& root, FrameworkElement const& buttons,
                        FrameworkElement const& tray, DockInsets insets) {
    auto buttonInsets=GetButtonInsets(insets,root.dock.background.ActualHeight());
    auto trayInsets=GetButtonInsets(insets,root.dock.background.ActualHeight(),g_trayHighlightPadding);
    std::vector<Controls::Border> plates;
    CollectStatePlates(buttons, plates);
    std::vector<Controls::Border> trayPlates;
    CollectStatePlates(tray,trayPlates);
    plates.insert(plates.end(),trayPlates.begin(),trayPlates.end());
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
        state->trayHighlight=std::find(trayPlates.begin(),trayPlates.end(),plate)!=trayPlates.end();
        if(state->trayHighlight) {
            EnsurePlateTemplate(*state);
            if(!state->templateValues) continue;
            state->buttonOwner=CanonicalIdentity(state->templateValues->control);
            if(!state->trayStates) {
                state->trayStates=std::make_shared<ButtonStateObserver>();
                state->trayStates->Attach(state->templateValues->control,state->buttonOwner);
            }
            PrepareStatePlate(root,*state,trayInsets,false);
            continue;
        }
        FrameworkElement owner{nullptr};
        auto node=plate.as<FrameworkElement>();
        for (unsigned depth=0; node && depth<16; ++depth) {
            if (winrt::get_class_name(node)==L"Taskbar.TaskListButton") { owner=node; break; }
            node=Media::VisualTreeHelper::GetParent(node).try_as<FrameworkElement>();
        }
        state->buttonOwner=owner?CanonicalIdentity(owner):0;
        auto source=std::find_if(root.sources.begin(),root.sources.end(),
            [&](auto const& value) { return value.identity==state->buttonOwner; });
        if(source!=root.sources.end() && source->included && !source->plate &&
           plate.Name()==L"BackgroundElement")
            source->plate=plate;
        bool glassLayer=source!=root.sources.end() &&
            ButtonGlassEnabled(g_theme,g_buttonLiquidLayer);
        PrepareStatePlate(root, *state, buttonInsets, glassLayer);
    }
}

void RestoreStatePlates(Root& root) {
    for (auto& state : root.statePlates) RestoreStatePlate(state);
    root.statePlates.clear();
}

// Accepted primary plates and their native button owners. Lens packing returns
// this plan without changing XAML; only a successful renderer update commits it.
using ButtonGlassSources = std::vector<std::pair<Controls::Border,uintptr_t>>;

bool CommitButtonGlass(Root& root, ButtonGlassSources const& accepted={}) {
    bool complete=true;
    bool highContrast=HighContrastActive();
    for(auto& state:root.statePlates) {
        if(!state.element || state.blocked) continue;
        try {
            bool glass=g_theme==DockTheme::Liquid && !highContrast &&
                std::any_of(accepted.begin(),accepted.end(),[&](auto const& source) {
                    return source.first==state.element ||
                        (source.second && source.second==state.buttonOwner);
                });
            if(!glass && (state.clip || state.fillHidden)) {
                // Release glass-only clip/rim/fill while retaining configured
                // native rounding/padding. Other plates still restore if one fails.
                auto insets=GetButtonInsets(root.measurements.insets,root.dock.background.ActualHeight());
                PrepareStatePlate(root,state,insets,false);
            }
            if(state.element && !state.blocked) ApplyPlateAppearance(state,glass,highContrast);
        } catch (...) {
            complete=false;
            Wh_Log(L"[UWITEM] button appearance commit failed hr=%08X",winrt::to_hresult().value);
        }
    }
    return complete;
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

void RestoreGlass(Root& root) {
    auto& dock=root.dock;
    StopLiquid(root);
    dock.shadow.reset();
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

void PrepareGlass(Root& root) {
    auto& dock=root.dock;
    if (g_theme == DockTheme::Native || dock.materialBlocked) {
        RestoreGlass(root);
        return;
    }
    if (HighContrastActive()) {
        RestoreGlass(root);
        return;
    }
    try {
        auto theme = dock.background.ActualTheme();
        if (dock.material) {
            if (dock.fill.Fill() != dock.material || dock.stroke.Fill() != dock.transparentStroke ||
                Hosting::ElementCompositionPreview::GetElementChildVisual(dock.background) != dock.attachedVisual) {
                RestoreGlass(root);
                dock.materialBlocked = true;
                Wh_Log(L"[UWGLASS] appearance ownership changed; native/external material retained");
                return;
            }
            if (dock.materialTheme == theme) return;
            RestoreGlass(root);
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
            material = GlassMaterial(frosted, dark);
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
        RestoreGlass(root);
        dock.materialBlocked = true;
        Wh_Log(L"[UWGLASS] native material fallback hr=%08X", error.value);
    }
}

void StopLiquid(Root& root) {
    auto& dock=root.dock;
    root.glassPointer.reset();
    if(dock.liquid) {
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
    CommitButtonGlass(root);
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

// Closing a started Windows.Graphics.Capture session leaves one frame of the item's size in the
// process until it exits.
struct LiquidHandoff {
    // ponytail: a fixed wait. A rebuild that keeps failing past the retry back-off (1 s, 2 s, 4 s)
    // misses it and pays the old cost; release when that taskbar's rebuilt Root gives up instead.
    static constexpr ULONGLONG kWaitMs = 5000;
    std::vector<std::shared_ptr<LiquidRenderer>> held;
    ULONGLONG until{};

    void Hold(std::shared_ptr<LiquidRenderer> renderer) {
        if (!renderer) return;
        held.push_back(std::move(renderer));
        until = GetTickCount64() + kWaitMs;
        ArmLiquidTimer();
    }
    std::shared_ptr<LiquidRenderer> Take(HWND taskbar, Compositor const& compositor) {
        std::shared_ptr<LiquidRenderer> taken;
        bool found{};
        std::erase_if(held, [&](auto const& renderer) {
            if (renderer->taskbar != taskbar) return false;
            found = true;
            if (!taken && renderer->visualRoot.Compositor() == compositor && renderer->Adopt()) taken = renderer;
            else renderer->Stop();
            return true;
        });
        if (found)
            Wh_Log(L"[UWLIQUID] renderer %s taskbar=%p", taken ? L"handed over" : L"not reusable", taskbar);
        return taken;
    }
    bool Expire(ULONGLONG now) {
        if (!held.empty() && now >= until) {
            Wh_Log(L"[UWLIQUID] handoff expired; stopping %zu renderers", held.size());
            Release();
        }
        return !held.empty();
    }
    void Release() {
        for (auto const& renderer : held) renderer->Stop();
        ReleaseVector(held);
    }
};
[[clang::no_destroy]] LiquidHandoff g_liquidHandoff;  // Owner thread only.

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

bool UpdateTrayIconGeometry(Root::TrayIconState& state) {
    try {
        auto element = state.element.get();
        auto image = state.image.get();
        if (!element || !image || !state.surface || !state.overlay ||
            image.ActualWidth() <= 0 || image.ActualHeight() <= 0) return false;
        float width = static_cast<float>(image.ActualWidth());
        float height = static_cast<float>(image.ActualHeight());
        auto transform = image.TransformToVisual(element);
        auto origin = transform.TransformPoint({0, 0});
        auto right = transform.TransformPoint({width, 0});
        auto bottom = transform.TransformPoint({0, height});
        state.surface.SourceSize({width, height});
        state.overlay.Size({width, height});
        state.overlay.Offset({origin.X, origin.Y, 0});
        state.overlay.TransformMatrix({
            (right.X - origin.X) / width, (right.Y - origin.Y) / width, 0, 0,
            (bottom.X - origin.X) / height, (bottom.Y - origin.Y) / height, 0, 0,
            0, 0, 1, 0, 0, 0, 0, 1});
        int variant = state.light ? 0 : 1;
        auto const& asset = state.custom[variant];
        if (asset && state.appliedVariant != variant && !asset->failed && asset->Ready()) {
            auto compositor = state.overlay.Compositor();
            auto replacement = compositor.CreateSurfaceBrush(asset->surface);
            replacement.Stretch(CompositionStretch::Uniform);
            state.overlay.Brush(TrayBrush(compositor, replacement, g_grayscaleTrayIcons));
            state.appliedVariant = variant;
        }
        return true;
    } catch (...) {
        Wh_Log(L"[UWTRAY] icon update failed hr=%08X", winrt::to_hresult().value);
        return false;
    }
}

void RestoreTrayIconState(Root::TrayIconState& state) {
    try {
        if (auto element = state.element.get(); element && state.overlay &&
            Hosting::ElementCompositionPreview::GetElementChildVisual(element) == state.overlay)
            Hosting::ElementCompositionPreview::SetElementChildVisual(element, nullptr);
    } catch (...) {}
    try {
        if (state.opacityOwned && state.originalVisual && state.originalVisual.Opacity() == 0)
            state.originalVisual.Opacity(state.originalOpacity);
    } catch (...) {}
    state.opacityOwned = false;
    state.surface = nullptr;
    state.overlay = nullptr;
    state.nativeBrush = nullptr;
    state.custom = {};
}

void RestoreTrayIcons(Root& root) {
    std::erase_if(g_trayIcons, [&](auto& state) {
        if (state.taskbar != root.taskbar) return false;
        RestoreTrayIconState(state);
        return true;
    });
}

void PrepareTrayIcon(FrameworkElement const& element, Controls::Image const& image, HWND taskbar) {
    auto key = TrayElementId(element);
    auto replacement = g_customTrayIcons.find(key);
    TrayIconFiles files = replacement == g_customTrayIcons.end() ? TrayIconFiles{} : replacement->second;
    auto state = std::find_if(g_trayIcons.begin(), g_trayIcons.end(),
        [&](auto const& candidate) { return candidate.image.get() == image; });
    if (state != g_trayIcons.end()) {
        if (state->key == key && state->files == files &&
            state->originalVisual.Opacity() == 0 &&
            Hosting::ElementCompositionPreview::GetElementChildVisual(element) == state->overlay) {
            if (g_fontColorMode != FontColorMode::Adaptive)
                state->light = g_fontColorMode == FontColorMode::Light ||
                    (g_fontColorMode == FontColorMode::Default && element.ActualTheme() == ElementTheme::Dark);
            UpdateTrayIconGeometry(*state);
            return;
        }
        RestoreTrayIconState(*state);
        g_trayIcons.erase(state);
    }
    if ((!g_grayscaleTrayIcons && files.empty()) ||
        Hosting::ElementCompositionPreview::GetElementChildVisual(element)) return;
    Root::TrayIconState next;
    next.element = winrt::make_weak(element);
    next.image = winrt::make_weak(image);
    next.taskbar = taskbar;
    next.key = std::move(key);
    next.files = files;
    next.light = g_fontColorMode == FontColorMode::Light ||
        (g_fontColorMode != FontColorMode::Dark && element.ActualTheme() == ElementTheme::Dark);
    try {
        next.originalVisual = Hosting::ElementCompositionPreview::GetElementVisual(image);
        next.originalOpacity = next.originalVisual.Opacity();
        if (next.originalOpacity <= 0) return;
        auto compositor = next.originalVisual.Compositor();
        next.surface = compositor.CreateVisualSurface();
        next.surface.SourceVisual(next.originalVisual);
        next.nativeBrush = TrayBrush(compositor, compositor.CreateSurfaceBrush(next.surface), g_grayscaleTrayIcons);
        next.overlay = compositor.CreateSpriteVisual();
        next.overlay.Brush(next.nativeBrush);
        next.overlay.Opacity(next.originalOpacity);
        if (!files.empty()) {
            for (size_t i = 0; i < 2; ++i) {
                if (i == 1 && !files.Adaptive()) { next.custom[1] = next.custom[0]; break; }
                try { next.custom[i] = LoadTraySurface(LoadTrayImage(i == 0 ? files.light : files.dark)); }
                catch (...) { Wh_Log(L"[UWTRAY] custom image unavailable; original retained hr=%08X", winrt::to_hresult().value); }
            }
        }
        if (!UpdateTrayIconGeometry(next)) return;
        Hosting::ElementCompositionPreview::SetElementChildVisual(element, next.overlay);
        // VisualSurface renders SourceVisual as its root, ignoring its own opacity.
        // Hide the native image only after the replacement is ready to draw.
        next.originalVisual.Opacity(0);
        next.opacityOwned = true;
        if (next.custom[0] || next.custom[1]) ArmLiquidTimer();
        g_trayIcons.push_back(std::move(next));
    } catch (...) {
        RestoreTrayIconState(next);
        Wh_Log(L"[UWTRAY] icon customization unavailable hr=%08X", winrt::to_hresult().value);
    }
}

constexpr UINT kRefreshTrayImagesMessage = WM_APP + 54;

void TrackTrayView(FrameworkElement const& element) {
    if (!element || element.Name() != L"NotifyItemIcon") return;
    std::erase_if(g_loadedTrayViews, [](auto const& view) { return !view.element.get(); });
    for (auto const& view : g_loadedTrayViews) if (view.element.get() == element) return;
    auto token = element.SizeChanged([](auto const&, auto const&) {
        ActiveScope active;
        if (!g_stopping && g_dispatch) PostMessageW(g_dispatch, kRefreshTrayImagesMessage, 0, 0);
    });
    g_loadedTrayViews.push_back({winrt::make_weak(element), token});
}

void RefreshLoadedTrayIcons() {
    if (g_stopping || g_busy || GetCurrentThreadId() != g_ownerThread) return;
    std::erase_if(g_trayIcons, [](auto& state) {
        auto element = state.element.get();
        if (element && element.IsLoaded() && state.image.get()) return false;
        RestoreTrayIconState(state);
        return true;
    });
    if ((!g_grayscaleTrayIcons && g_customTrayIcons.empty()) || HighContrastActive()) return;
    HWND taskbar{};
    for (auto const& root : g_roots) {
        if (root.retired || !root.taskbar) continue;
        taskbar = root.taskbar;
        if (ClassIs(taskbar, L"Shell_TrayWnd")) break;
    }
    if (!taskbar) return;
    for (auto const& view : g_loadedTrayViews) {
        try {
            auto element = view.element.get();
            if (!element || !element.IsLoaded()) continue;
            if (auto image = FindImage(element)) PrepareTrayIcon(element, image, taskbar);
        } catch (...) {}
    }
}

void PrepareTrayIcons(Root& root, FrameworkElement const& tray) {
    if ((!g_grayscaleTrayIcons && g_customTrayIcons.empty()) || HighContrastActive()) {
        RestoreTrayIcons(root);
        return;
    }
    try {
        std::vector<std::pair<FrameworkElement, Controls::Image>> images;
        CollectTrayIconImages(tray, images);
        for (auto const& [element, image] : images) {
            TrackTrayView(element);
            PrepareTrayIcon(element, image, root.taskbar);
        }
    } catch (...) {
        Wh_Log(L"[UWTRAY] discovery unavailable hr=%08X", winrt::to_hresult().value);
    }
}

bool RefreshTrayImageLoads() {
    bool pending = false;
    for (auto& state : g_trayIcons) {
        for (auto const& asset : state.custom)
            if (asset && !asset->failed) pending |= !asset->Ready();
        UpdateTrayIconGeometry(state);
    }
    return pending;
}

void ClearLoadedTrayViews() {
    for (auto const& view : g_loadedTrayViews) {
        try { if (auto element = view.element.get()) element.SizeChanged(view.sizeChanged); }
        catch (...) {}
    }
    g_loadedTrayViews.clear();
    for (auto& state : g_trayIcons) RestoreTrayIconState(state);
    ReleaseVector(g_trayIcons);
}

using NotifyIconLoaded_t = void(WINAPI*)(void*, TrayInspectable const&, RoutedEventArgs const&);
NotifyIconLoaded_t NotifyIconLoaded_Original;
std::atomic<bool> g_trayLoadedHookStarted{};

void WINAPI NotifyIconLoaded_Hook(void* self, TrayInspectable const& sender, RoutedEventArgs const& args) {
    ActiveScope active;
    NotifyIconLoaded_Original(self, sender, args);
    if (g_stopping || GetCurrentThreadId() != g_ownerThread) return;
    try {
        auto element = sender.try_as<FrameworkElement>();
        TrackTrayView(element);
        if (g_dispatch) PostMessageW(g_dispatch, kRefreshTrayImagesMessage, 0, 0);
    } catch (...) {}
}

void HookTrayLoaded(HMODULE module, bool apply) {
    if (!module || g_trayLoadedHookStarted.exchange(true)) return;
    WindhawkUtils::SYMBOL_HOOK hooks[]{{
        {LR"(private: void __cdecl winrt::SystemTray::implementation::NotifyIconView::OnLoaded(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Windows::UI::Xaml::RoutedEventArgs const &))"},
        &NotifyIconLoaded_Original, NotifyIconLoaded_Hook,
    }};
    if (WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks)) && apply)
        Wh_ApplyHookOperations();
}

struct ForegroundSampleRequest {
    struct Item {
        uintptr_t identity{};
        RECT region{};
    };
    HWND taskbar{};
    uintptr_t rootIdentity{};
    uint64_t configurationGeneration{};
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
    uint64_t configurationGeneration{};
    ULONGLONG capturedAt{};
    std::vector<Item> items;
};
std::mutex g_foregroundResultsMutex;
std::vector<std::unique_ptr<ForegroundSampleResult>> g_foregroundResults;
uint64_t g_configurationGeneration{}; // Taskbar UI thread; workers copy request values.

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

bool ElementSampleRegion(Root const& root, FrameworkElement const& element,
                         RECT const& taskbar, UINT dpi, RECT& region) {
    try {
        if (!root.frame || element.ActualWidth() <= 0 ||
            element.ActualHeight() <= 0)
            return false;
        auto bounds = element.TransformToVisual(root.frame).TransformBounds(
            {0, 0, static_cast<float>(element.ActualWidth()),
             static_cast<float>(element.ActualHeight())});
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
    result.taskbar = request.taskbar;
    result.rootIdentity = request.rootIdentity;
    result.configurationGeneration = request.configurationGeneration;
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
    result.items.clear();
    if (captured) {
        GdiFlush();
        result.capturedAt = GetTickCount64();
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

void UpdatePreviewForeground(SettingsEditor& editor) {
    auto& preview=editor.preview;
    if(!editor.valid || !preview.canvas || !IsWindowVisible(editor.window) ||
       IsIconic(editor.window) || HighContrastActive() ||
       editor.controls[1].input.as<Controls::ComboBox>().SelectedIndex()!=int(FontColorMode::Adaptive))
        return;
    try {
        std::array<FrameworkElement,3> elements{preview.clock,preview.icons[1],preview.icons[2]};
        if(preview.foregroundSample.valid()) {
            if(preview.foregroundSample.wait_for(std::chrono::seconds(0))!=std::future_status::ready) return;
            auto samples=preview.foregroundSample.get();
            if(preview.foregroundSampleRevision==preview.foregroundRevision)
                for(size_t i=0;i<samples.size();++i) {
                    if(!std::isfinite(samples[i])) continue;
                    bool light=PreferLightForeground(samples[i],preview.foregroundLight[i],preview.foregroundReady[i]);
                    preview.foregroundLight[i]=light;preview.foregroundReady[i]=true;
                    auto color=ForegroundColor(light);
                    if(i==0) preview.clock.Foreground(Media::SolidColorBrush(color));
                    else preview.icons[i].Foreground(Media::SolidColorBrush(color));
                }
        }
        if(!preview.foregroundDirty) return;
        ForegroundSampleRequest request;
        request.taskbar=editor.window;
        if(!GetClientRect(editor.window,&request.bounds)) return;
        POINT origin{};if(!ClientToScreen(editor.window,&origin)) return;
        OffsetRect(&request.bounds,origin.x,origin.y);
        Root samplingRoot;samplingRoot.frame=editor.content;
        for(size_t i=0;i<elements.size();++i) {
            RECT region{};
            if(!ElementSampleRegion(samplingRoot,elements[i],request.bounds,editor.dpi,region)) return;
            // Do not adapt the preview to another window covering its sample.
            POINT middle{region.left+(region.right-region.left)/2,region.top+(region.bottom-region.top)/2};
            auto visible=WindowFromPoint(middle);
            if(!visible || GetAncestor(visible,GA_ROOT)!=editor.window) return;
            OffsetRect(&region,-request.bounds.left,-request.bounds.top);
            request.items.push_back({i,region});
        }
        preview.foregroundSampleRevision=preview.foregroundRevision;
        preview.foregroundDirty=false;
        preview.foregroundSample=std::async(std::launch::async,[request=std::move(request)] {
            std::array<double,3> samples{NAN,NAN,NAN};
            ForegroundSampleResult result;
            if(CaptureForegroundSamples(request,result))
                for(auto const& item:result.items)
                    if(item.identity<samples.size()) samples[item.identity]=item.luminance;
            return samples;
        });
    } catch(...) {
        preview.foregroundDirty=false;
        Wh_Log(L"[UWSET] preview foreground sample failed hr=%08X",winrt::to_hresult().value);
    }
}

DWORD WINAPI ForegroundSampleWorker(void* raw) {
    struct Complete {
        ~Complete() { --g_active; }
    } complete;
    std::unique_ptr<ForegroundSampleRequest> request(
        static_cast<ForegroundSampleRequest*>(raw));
    try {
        auto result = std::make_unique<ForegroundSampleResult>();
        CaptureForegroundSamples(*request, *result);
        if (!g_stopping) {
            {
                std::lock_guard lock(g_foregroundResultsMutex);
                if (g_stopping) return 0;
                g_foregroundResults.push_back(std::move(result));
            }
        }
    } catch (...) {
    }
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
    auto text = state.element;
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
    if (result.configurationGeneration != g_configurationGeneration) return;
    auto root = std::find_if(g_roots.begin(), g_roots.end(), [&](auto const& r) {
        return !r.retired && r.taskbar == result.taskbar && r.frame &&
               reinterpret_cast<uintptr_t>(winrt::get_abi(r.frame)) ==
                   result.rootIdentity;
    });
    if (root == g_roots.end()) return;
    root->foregroundSamplePending = false;
    if (g_fontColorMode != FontColorMode::Adaptive) return;
    size_t initialized{}, changed{}, lightCount{}, darkCount{};
    for (auto const& sample : result.items) {
        auto state = std::find_if(
            root->foregrounds.begin(), root->foregrounds.end(),
            [&](auto const& candidate) {
                return candidate.identity == sample.identity;
            });
        if (state != root->foregrounds.end() && state->owned) {
            bool wasReady = state->ready;
            double luminance = state->luminance.Add(sample.luminance, result.capturedAt, g_adaptiveWindowMs);
            bool light = PreferLightForeground(luminance, state->light,
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
            continue;
        }
        for (auto& icon : g_trayIcons) {
            auto element = icon.element.get();
            if (icon.taskbar != result.taskbar || !icon.files.Adaptive() || !element ||
                reinterpret_cast<uintptr_t>(winrt::get_abi(element)) != sample.identity) continue;
            double luminance = icon.luminance.Add(sample.luminance, result.capturedAt, g_adaptiveWindowMs);
            icon.light = PreferLightForeground(luminance, icon.light, icon.ready);
            icon.ready = true;
            UpdateTrayIconGeometry(icon);
            break;
        }
    }
    if (initialized || changed)
        Wh_Log(L"[UWFONT] adaptive initialized=%zu changed=%zu light=%zu dark=%zu",
               initialized, changed, lightCount, darkCount);
}

void ApplyPendingForegroundSamples() {
    std::vector<std::unique_ptr<ForegroundSampleResult>> results;
    {
        std::lock_guard lock(g_foregroundResultsMutex);
        results.swap(g_foregroundResults);
    }
    if (g_stopping) return;
    for (auto const& result : results) {
        try {
            ApplyAdaptiveForeground(*result);
        } catch (...) {
            Wh_Log(L"[UWFONT] sample apply failed hr=%08X",
                   winrt::to_hresult().value);
        }
    }
}

void UpdateAdaptiveForeground(Root& root) {
    if (g_fontColorMode != FontColorMode::Adaptive ||
        root.foregroundSamplePending)
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
        request->configurationGeneration = g_configurationGeneration;
        request->bounds = taskbar;
        request->items.reserve(root.foregrounds.size());
        for (auto& state : root.foregrounds) {
            auto text = state.element;
            if (!text || !state.owned) continue;
            RECT region{};
            if (!ElementSampleRegion(root, text, taskbar, dpi, region))
                continue;
            OffsetRect(&region, -taskbar.left, -taskbar.top);
            request->items.push_back({state.identity, region});
        }
        for (auto const& icon : g_trayIcons) {
            auto element = icon.element.get();
            if (icon.taskbar != root.taskbar || !icon.files.Adaptive() || !element) continue;
            RECT region{};
            if (!ElementSampleRegion(root, element, taskbar, dpi, region)) continue;
            OffsetRect(&region, -taskbar.left, -taskbar.top);
            request->items.push_back({reinterpret_cast<uintptr_t>(winrt::get_abi(element)), region});
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
            auto text = state.element;
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
                    return candidate.element == text;
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
            next.element = text;
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

ButtonGlassSources AddButtonLenses(Root const& root, LiquidRenderer::LensConstants& lens,
                                   double scale) {
    ButtonGlassSources accepted;
    auto anchor = root.anchor.get();
    auto anchorVisual = anchor ? Hosting::ElementCompositionPreview::GetElementVisual(anchor) : nullptr;
    size_t dropped = 0;
    for (auto const& source : root.sources) {
        // included is the shared eligibility/lifecycle decision. Keep the
        // explicit retirement guard so stale cached inclusion can never emit a
        // lens while a source waits for parking or reuse.
        if (!source.included || source.life == Root::Source::Life::Retired)
            continue;
        if (!ButtonGlassEnabled(g_theme,g_buttonLiquidLayer)) continue;
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
        // Border minima can exceed the styled height. Do not turn a one-DIP
        // padded band into a larger glass lens using that minimum layout size.
        if(owned->layoutOwned && !ButtonLensFits(plate.ActualWidth(),owned->appliedHeight)) continue;

        // Express both the native Border and our inset clip in the dock's
        // coordinate space. TransformToVisual carries native transforms;
        // separate native and clip bounds retain the same inset shape.
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

        auto corners=plate.CornerRadius();
        auto clipRadius=owned->clipGeometry.CornerRadius();
        auto rect=[&](auto topLeft,double width,double height) {
            return float4{float((topLeft.X+width/2)*scale),float((topLeft.Y+height/2)*scale),
                          float(width*scale/2),float(height*scale/2)};
        };
        auto index=AppendButtonLens(lens,
            rect(nativeTopLeft,nativeWidth,nativeHeight),rect(clipTopLeft,clipWidth,clipHeight),
            {float(corners.TopLeft*xScale*scale),float(corners.TopRight*xScale*scale),
             float(corners.BottomRight*xScale*scale),float(corners.BottomLeft*xScale*scale)},
            {float(yScale/xScale),float(clipRadius.x*xScale*scale),float(clipRadius.y*yScale*scale)},
            source.requestingAttention,plate.Opacity()*source.visual.Opacity());
        if(accepted.empty()) accepted.reserve(std::min(root.sources.size(),kMaxButtonLenses));
        accepted.emplace_back(plate,source.identity);
        if (root.dock.liquid && anchorVisual) {
            auto button = source.element.get();
            if (button) {
                auto local = plate.TransformToVisual(button);
                auto nativeLeft = local.TransformPoint({0,0});
                auto nativeRight = local.TransformPoint({float(plate.ActualWidth()),float(plate.ActualHeight())});
                auto clipLeft = local.TransformPoint({offset.x,offset.y});
                auto clipRight = local.TransformPoint({offset.x+extent.x,offset.y+extent.y});
                root.dock.liquid->TrackLens(index,source.visual,anchorVisual,plate,root.dock.geometry,
                    (nativeLeft.X+nativeRight.X)/2,(clipLeft.X+clipRight.X)/2);
            }
        }

    }
    if (dropped)
        Wh_Log(L"[UWLIQUID] %zu button lenses past the %zu cap", dropped,
                kMaxButtonLenses);
    if (root.dock.liquid) root.dock.liquid->TrimLensProbes(lens.lensCount);
    return accepted;
}

void PrepareLiquid(Root& root, Measurements const& m) {
    auto& dock = root.dock;
    if (g_theme != DockTheme::Liquid) {
        dock.shadow.reset();
        StopLiquid(root);
        dock.liquidRetryAt = 0;
        dock.liquidFailures = 0;
        return;
    }
    if (!dock.highlights || !root.taskbar || !root.publishedShift) {
        dock.shadow.reset();
        StopLiquid(root);
        return;
    }
    if (dock.liquidRetryAt && GetTickCount64() < dock.liquidRetryAt) {
        StopLiquid(root);
        return;
    }
    // Startup discovery arrives through SendMessage/WH_CALLWNDPROC. WinRT
    // rejects compositor setup from that input-synchronous call (8001010D).
    if (!dock.liquid && !g_preparingLiquid) {
        if (!g_liquidPreparePending && g_dispatch &&
            PostMessage(g_dispatch, kPrepareLiquidMessage, 0, 0))
            g_liquidPreparePending = true;
        StopLiquid(root);
        return;
    }
    try {
        if (g_liquidSettings.shadowOpacity > 0) {
            if (!dock.shadow) {
                auto parent = Media::VisualTreeHelper::GetParent(dock.background).try_as<Controls::Panel>();
                if (!parent) throw winrt::hresult_error(E_NOTIMPL, L"Shadow parent unavailable");
                dock.shadow = std::make_shared<GlassShadow>(parent, dock.background, dock.geometry);
            }
            auto offset = dock.background.ActualOffset();
            dock.shadow->Update({float(dock.background.ActualWidth()), float(dock.background.ActualHeight())},
                                {offset.x, offset.y}, g_liquidSettings);
        } else dock.shadow.reset();
        double scale = root.frame.XamlRoot().RasterizationScale();
        RECT taskbarRect{};
        if (!GetWindowRect(root.taskbar, &taskbarRect))
            throw winrt::hresult_error(HRESULT_FROM_WIN32(GetLastError()));
        UINT pixelWidth = static_cast<UINT>(std::lround(
            dock.background.ActualWidth() * scale));
        UINT pixelHeight = static_cast<UINT>(std::lround(
            dock.background.ActualHeight() * scale));
        auto anchorElement = root.anchor.get();
        if (!anchorElement)
            throw winrt::hresult_error(E_PENDING, L"Optical anchor unavailable");
        // Start's native motion and our compositor-driven parent translation
        // match the dock expression, independently of app order or dragging.
        double dockLeft = anchorElement.TransformToVisual(dock.background)
                              .TransformPoint({0, 0}).X - kDockPadding;
        double dockWidth = 2 * (m.host / 2 - m.backgroundOrigin - dockLeft);
        if (!std::isfinite(dockLeft) || !std::isfinite(dockWidth) ||
            dockWidth <= 0)
            throw winrt::hresult_error(E_PENDING, L"Optical bounds unavailable");
        bool attached = false;
        if (!dock.liquid) {
            auto renderer = g_liquidHandoff.Take(root.taskbar, dock.visual.Compositor());
            if (!renderer) {
                renderer = std::make_shared<LiquidRenderer>();
                if (!renderer->Initialize(dock.visual.Compositor(), root.taskbar,
                                          pixelWidth, pixelHeight))
                    throw winrt::hresult_error(E_FAIL);
            }
            renderer->FollowDock(dock.geometry);
            dock.liquid = std::move(renderer);
            root.glassPointer = std::make_unique<GlassPointerBinding>(root.frame, dock.background, dock.liquid);
            dock.liquid->SetMotionActive(!g_liquidMotionBatches.empty());
            dock.attachedVisual = dock.liquid->visualRoot;
            Hosting::ElementCompositionPreview::SetElementChildVisual(
                dock.background, dock.attachedVisual);
            attached = true;
        }
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
        ScaleLiquidSettings(lens.optics, float(scale));
        auto accepted=AddButtonLenses(root, lens, scale);
        if (!dock.liquid->UpdateGeometry(
                taskbarRect, pixelWidth, pixelHeight, lens, float(scale)))
            throw winrt::hresult_error(E_FAIL, L"Liquid renderer unavailable");
        if (attached) dock.liquid->UpdateSources();
        if(!CommitButtonGlass(root,accepted))
            throw winrt::hresult_error(E_FAIL, L"Button appearance unavailable");
        dock.liquidRetryAt = 0;
        dock.liquidFailures = 0;
        ArmLiquidTimer();
    } catch (...) {
        HRESULT error = winrt::to_hresult().value;
        dock.shadow.reset();
        StopLiquid(root);
        ScheduleLiquidRetry(dock, error);
    }
}

void RestoreDock(Root& root) {
    auto& dock=root.dock;
    dock.liquidRetryAt = 0;
    dock.liquidFailures = 0;
    RestoreGlass(root);
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

double BoundaryThreshold(double& overhang, double start, double buttons,
                         double width, double localTrayLeft) {
    // Native left = min((width-buttons)/2, rightLimit-buttons). The two
    // branches meet at left = width-rightLimit. Learn rightLimit only where
    // native layout is constrained; retain its tray-relative offset on exit.
    if (start - (width - buttons) / 2 < -1)
        overhang = start + buttons - localTrayLeft;
    else if (std::isfinite(overhang) && width - localTrayLeft - overhang > start + 1)
        // A centered layout must be on the centered side of the learned limit.
        // A transient measurement or changed tray can invalidate that limit;
        // retaining it would permanently shift the tray into the app buttons.
        overhang = NAN;
    return std::isfinite(overhang) ? width - localTrayLeft - overhang : -width;
}

std::wstring EdgeExpression(bool buttons = false) {
    // No retiring right edge feeds the parent: it would add a second reflow.
    std::wstring correction = L"Max(0, (geometry.threshold - anchor.Offset.X) / 2)";
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
    root.anchor = {};
    root.nativeRightOverhang = NAN;
    root.metrics = nullptr;
    RestoreStatePlates(root);
    root.sources.clear();
}

void UpdateFollower(Root& root, FrameworkElement const& repeater,
                    Measurements const& m) {
    root.sourceLifecyclePending=false;
    int count = Media::VisualTreeHelper::GetChildrenCount(repeater);
    std::erase_if(root.sources, [](auto& source) {
        if (!source.element.get()) return true;
        source.included = false;
        source.plate = nullptr;
        return false;
    });
    if (count > 64) throw winrt::hresult_error(E_BOUNDS);
    std::vector<uintptr_t> seen;
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
            source = root.sources.end() - 1;
        }
        ObserveButtonStates(*source,child);
        source->included = IncludeSource(eligible, source->life);
        // A completed button remains excluded until parking or a new native
        // start proves reuse, even while its old slot still looks visible.
        if (!eligible && source->life != Root::Source::Life::Hiding) seen.pop_back();
    }
    std::erase_if(root.sources, [&](auto& source) {
        if (std::find(seen.begin(), seen.end(), source.identity) != seen.end())
            return false;
        return true;
    });
    // ExperienceToggleButton already owns an Offset implicit animation on this
    // handout Visual. Start follows native centering but never participates in
    // app reordering; an app anchor would move the dock or force native fallback.
    auto start = Child(repeater, L"LaunchListButton");
    if (!start || start.Visibility() != Visibility::Visible ||
        start.ActualWidth() <= 0)
        throw winrt::hresult_error(E_PENDING, L"Start reference unavailable");
    bool anchorChanged = root.anchor.get() != start;
    if (!root.metrics)
        root.metrics = root.dock.visual.Compositor().CreatePropertySet();
    bool knownBoundary = std::isfinite(root.nativeRightOverhang);
    double threshold = BoundaryThreshold(
        root.nativeRightOverhang, m.startLeft, m.buttonsWidth,
        repeater.ActualWidth(), m.trayLeft - m.origin);
    if (!knownBoundary && std::isfinite(root.nativeRightOverhang))
        Wh_Log(L"[UWFOLLOW] boundary root=%p threshold=%.3f overhang=%.3f tick=%llu",
               winrt::get_abi(root.frame), threshold, root.nativeRightOverhang, GetTickCount64());
    root.metrics.InsertScalar(L"threshold", static_cast<float>(threshold));
    root.metrics.InsertScalar(L"shift", static_cast<float>(m.shift));
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
    if (root.follower && !anchorChanged) return;
    auto anchorVisual = Hosting::ElementCompositionPreview::GetElementVisual(start);
    auto animation =
        root.metrics.Compositor().CreateExpressionAnimation(EdgeExpression());
    auto buttonsAnimation =
        root.metrics.Compositor().CreateExpressionAnimation(EdgeExpression(true));
    std::wstring dockLeft =
        L"geometry.dockOrigin + geometry.shift + Max(0, (geometry.threshold - "
        L"anchor.Offset.X) / 2) + anchor.Offset.X - geometry.padding";
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
        expression.SetReferenceParameter(L"anchor", anchorVisual);
    }
    animation.SetReferenceParameter(L"dock", root.dock.geometry);
    // Keep cleanup armed before starting; a failed start is restored by Refresh's caller.
    root.follower = animation;
    root.buttonsFollower = buttonsAnimation;
    root.anchor = winrt::make_weak(start);
    root.trayTranslation.installed = true;
    root.buttonsTranslation.installed = true;
    root.buttonsTranslation.element.StartAnimation(buttonsAnimation);
    root.trayTranslation.element.StartAnimation(animation);
    root.dock.geometry.StartAnimation(L"Offset", dockOffsetAnimation);
    root.dock.geometry.StartAnimation(L"Size", dockSizeAnimation);
}

void RestoreRoot(Root& root) {
    root.layout.Clear();
    WithdrawTray(root);
    try { RestoreTrayIcons(root); }
    catch (...) { Wh_Log(L"[UWICON] restoration-error"); }
    try { RestoreForeground(root); }
    catch (...) { Wh_Log(L"[UWFONT] restoration-error"); }
    try { StopFollower(root); }
    catch (...) { Wh_Log(L"[UWFOLLOW] stop failed hr=%08X", winrt::to_hresult().value); }
    // Attempt both even if one XAML object is already closed.
    for (auto* owned : {&root.buttonsTranslation, &root.trayTranslation}) {
        try { Restore(*owned); }
        catch (...) { Wh_Log(L"[UWPOS] restoration-error element=%p", winrt::get_abi(owned->element)); }
    }
    try { RestoreDock(root); }
    catch (...) { Wh_Log(L"[UWDOCK] restoration-error"); }
    root.measurementsReady = false;
}

void RetireRoot(Root& root) {
    if (root.retired) return;
    root.retired = true;
    root.glassPointer.reset();
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
    // A material/theme change may stop the old renderer and release its button
    // glass. Prepare the new button geometry after that fallback has completed.
    PrepareGlass(root);
    UpdateFollower(root, repeater, m);
    PrepareStatePlates(root, repeater, tray, m.insets);
    PrepareTrayIcons(root, tray);
    // Publication marks availability; popup hooks query current transformed
    // tray bounds on the owner thread instead of consuming this layout estimate.
    if (!PublishTray(root, target.trayX)) {
        RestoreRoot(root);
        return;
    }
    PrepareLiquid(root, m);
    PrepareForeground(root, repeater, tray);
}

// Keep ownership and failed brush reconciliation live even when layout is
// unchanged. These are the same channels checked by the full preparation pass.
bool RootLayoutUnchanged(Root& root) {
    auto& saved=root.layout;
    if (!saved.ready || !root.measurementsReady || root.sourceLifecyclePending || root.retired || root.blocked ||
        g_dockingDisabled) return false;
    RECT window{};
    auto insets=GetDockInsets(root.taskbar,root.dock.background.ActualHeight());
    if (!GetWindowRect(root.taskbar,&window) || !EqualRect(&window,&saved.window) ||
        root.frame.XamlRoot().RasterizationScale()!=saved.scale ||
        insets.top!=saved.insets.top || insets.bottom!=saved.insets.bottom ||
        HighContrastActive()!=saved.highContrast) return false;
    for (auto const* owned : {&root.buttonsTranslation,&root.trayTranslation})
        if (owned->installed && !root.follower && !StillOwned(*owned)) return false;
    auto& dock=root.dock;
    if (dock.clip && dock.visual.Clip()!=dock.clip) return false;
    if (dock.material && (dock.fill.Fill()!=dock.material || dock.stroke.Fill()!=dock.transparentStroke ||
        Hosting::ElementCompositionPreview::GetElementChildVisual(dock.background)!=dock.attachedVisual))
        return false;
    for (auto const& plate:root.statePlates) {
        if (plate.blocked || !plate.element) continue;
        if (plate.fill && !plate.fill->refreshSucceeded) return false;
        if (plate.templateValues && !plate.trayHighlight)
            for (size_t i=0;i<4;++i)
                if (!plate.templateValues->LocalUnchanged(i)) return false;
        if (plate.clip && plate.visual.Clip()!=plate.clip) return false;
    }
    for (auto const& state:root.foregrounds)
        if (state.owned)
            if (auto text=state.element; !text ||
                text.ReadLocalValue(Controls::TextBlock::ForegroundProperty()).try_as<Media::Brush>()!=state.brush)
                return false;
    size_t index{};
    auto parent=Media::VisualTreeHelper::GetParent(root.frame);
    saved.changes.clear();
    if (!saved.Visit(parent,index,false) || index!=saved.nodes.size()) return false;
    // Native hover setters temporarily resize/re-round their background parts.
    // Those changes belong to the existing button queue, not dock discovery.
    // Validate the entire diff first: parent layout, topology and unowned parts
    // still need the full refresh, even when a plate changed at the same time.
    std::vector<uintptr_t> owners;
    for (auto const& [node,values]:saved.changes) {
        auto const& previous=saved.nodes[node];
        auto measured=values;
        measured.desired=previous.values.desired;
        bool onlyDesired=measured==previous.values;
        auto plate=std::find_if(root.statePlates.begin(),root.statePlates.end(),
            [&](auto const& state) {
                if (state.element==previous.element) return true;
                // Applying a new plate's height also updates its template's
                // desired size. Its arranged bounds and the outer button can
                // stay unchanged; finish that measurement in the button queue.
                return onlyDesired && state.templateValues &&
                       state.templateValues->root==previous.element;
            });
        if (plate==root.statePlates.end() || plate->blocked || !plate->templateValues ||
            !plate->buttonOwner) return false;
        if(plate->trayHighlight) {owners.push_back(plate->buttonOwner);continue;}
        auto source=std::find_if(root.sources.begin(),root.sources.end(),
            [&](auto const& value) { return value.identity==plate->buttonOwner; });
        if (source==root.sources.end() || source->life==Root::Source::Life::Retired) return false;
        owners.push_back(plate->buttonOwner);
    }
    for (auto& [node,values]:saved.changes) saved.nodes[node].values=std::move(values);
    saved.changes.clear();
    for (auto owner:owners) RequestItemStateRefresh(owner);
    return true;
}

void RememberRootLayout(Root& root) {
    RootLayoutSnapshot saved;
    if (!root.measurementsReady || root.sourceLifecyclePending || !root.taskbar ||
        !GetWindowRect(root.taskbar,&saved.window)) {
        root.layout.Clear();
        return;
    }
    saved.scale=root.frame.XamlRoot().RasterizationScale();
    saved.insets=GetDockInsets(root.taskbar,root.dock.background.ActualHeight());
    saved.highContrast=HighContrastActive();
    size_t index{};
    saved.ready=saved.Visit(Media::VisualTreeHelper::GetParent(root.frame),index,true);
    if (!saved.ready) saved.Clear();
    root.layout=std::move(saved);
}

void OnLayout(winrt::weak_ref<FrameworkElement> const& weak, bool notification) {
    if (g_stopping) return;
    ActiveScope active;
    if (g_stopping || GetCurrentThreadId() != g_ownerThread || g_busy) return;
    FrameworkElement frame{nullptr};
    try { frame = weak.get(); }
    catch (...) { Wh_Log(L"[UWPOS] layout owner unavailable"); return; }
    if (!frame) return;
    g_busy = true;
    struct Unbusy { ~Unbusy() { g_busy = false; PostItemStateRefresh(); } } unbusy;
    for (auto& root : g_roots) {
        if (root.frame != frame) continue;
        try {
            if (!notification || !RootLayoutUnchanged(root)) {
                Refresh(root);
                RememberRootLayout(root);
            }
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
            root.sourceLifecyclePending=true;
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

std::optional<float> ButtonShift(HMONITOR monitor) {
    if (!monitor || g_busy) return {};
    Root* match{};
    for (auto& root : g_roots) {
        if (root.retired || MonitorFromWindow(root.taskbar, MONITOR_DEFAULTTONULL) != monitor) continue;
        if (match) return {};
        match = &root;
    }
    if (!match || !match->buttonsFollower) return {};
    auto repeater = match->buttonsTranslation.element;
    auto parent = Media::VisualTreeHelper::GetParent(repeater).try_as<FrameworkElement>();
    if (!parent) return {};
    double shift = repeater.TransformToVisual(parent).TransformPoint({0, 0}).X - repeater.ActualOffset().x;
    double scale = match->frame.XamlRoot().RasterizationScale();
    if (!std::isfinite(shift * scale) || std::abs(shift) > match->hostWidth) return {};
    return static_cast<float>(shift * scale);
}

// Each of these also rewrites the jump list's X, so adding our shift would move
// it twice.
constexpr std::wstring_view kJumpListAnchorMods[] = {
    L"taskbar-margin", L"taskbar-split", L"taskbar-multirow",
    L"taskbar-vertical", L"taskbar-jump-list-on-cursor-pos"};

using ComputeJumpViewPosition_t = HRESULT(WINAPI*)(
    void*, void*, int, winrt::Windows::Foundation::Point*, HorizontalAlignment*, VerticalAlignment*);
ComputeJumpViewPosition_t ComputeJumpViewPosition_Original;
HRESULT WINAPI ComputeJumpViewPosition_Hook(
    void* self, void* group, int index, winrt::Windows::Foundation::Point* point,
    HorizontalAlignment* horizontal, VerticalAlignment* vertical) {
    ActiveScope active;
    HRESULT hr = ComputeJumpViewPosition_Original(self, group, index, point, horizontal, vertical);
    if (FAILED(hr) || g_stopping) return hr;
    if (GetCurrentThreadId() != g_ownerThread) {
        static std::atomic<bool> reported;
        if (g_ownerThread && !reported.exchange(true))
            Wh_Log(L"[UWJUMP] called off the taskbar UI thread tid=%lu owner=%lu; native position kept",
                   GetCurrentThreadId(), g_ownerThread.load());
        return hr;
    }
    if (ModLoaded(kJumpListAnchorMods)) return hr;
    try {
        auto monitor = MonitorFromPoint({std::lround(point->X), std::lround(point->Y)},
                                        MONITOR_DEFAULTTONULL);
        if (auto shift = ButtonShift(monitor)) point->X += *shift;
    } catch (...) { Wh_Log(L"[UWJUMP] shift sample failed hr=%08X", winrt::to_hresult().value); }
    return hr;
}

void DiscoverExisting(HWND taskbar);
bool QueueTaskbarDiscovery(HWND taskbar);
void ApplySavedSettings(HWND editor);

void RefreshWatchedFrames() {
    std::vector<winrt::weak_ref<FrameworkElement>> frames;
    frames.reserve(g_roots.size());
    for (auto const& root : g_roots)
        if (!root.retired && root.frame) frames.push_back(winrt::make_weak(root.frame));
    for (auto const& frame : frames) OnLayout(frame);
}

void RefreshItemStates() {
    ActiveScope active;
    g_itemStateRefreshPending=false;
    if (g_stopping || g_busy || GetCurrentThreadId()!=g_ownerThread) return;
    g_busy=true;
    struct Unbusy { ~Unbusy() { g_busy=false; PostItemStateRefresh(); } } unbusy;
    for (auto& root : g_roots) {
        if (root.retired || root.blocked || !root.measurementsReady) continue;
        if (std::none_of(root.sources.begin(),root.sources.end(),
                        [](auto const& source) { return source.stateRefreshPending; }) &&
            std::none_of(root.statePlates.begin(),root.statePlates.end(),
                        [](auto const& plate) { return plate.trayHighlight && plate.stateRefreshPending; })) continue;
        bool changed=false;
        try {
            auto insets=GetButtonInsets(root.measurements.insets,root.dock.background.ActualHeight());
            for (auto& source : root.sources) {
                if (!std::exchange(source.stateRefreshPending,false)) continue;
                if (source.life==Root::Source::Life::Retired) continue;
                auto element=source.element.get();
                if (!element) continue;
                changed=true;
                ObserveButtonStates(source,element);
                bool glass=ButtonGlassEnabled(g_theme,g_buttonLiquidLayer);
                for (auto& plate : root.statePlates)
                    if (plate.buttonOwner==source.identity)
                        PrepareStatePlate(root,plate,insets,glass);
            }
            auto trayInsets=GetButtonInsets(root.measurements.insets,root.dock.background.ActualHeight(),g_trayHighlightPadding);
            for(auto& plate:root.statePlates)
                if(plate.trayHighlight && std::exchange(plate.stateRefreshPending,false))
                    PrepareStatePlate(root,plate,trayInsets,false);
            // Repack once for the affected root. No discovery, tray movement,
            // foreground setup or native appearance work on unrelated buttons.
            if (changed && g_theme==DockTheme::Liquid)
                PrepareLiquid(root,root.measurements);
        } catch (...) {
            Wh_Log(L"[UWITEM] button state refresh failed hr=%08X",winrt::to_hresult().value);
            RestoreRoot(root);
        }
    }
}

LRESULT CALLBACK DispatchProc(HWND window, UINT message, WPARAM wp, LPARAM lp) {
    if(message==kRefreshPlateFillsMessage) {
        RefreshQueuedPlateFills();
        return 0;
    }
    if (message == WM_TIMER && wp == kDiscoveryTimerId) {
        ActiveScope active;
        if (g_busy && !g_stopping) return 0;
        KillTimer(window, kDiscoveryTimerId);
        auto pending = std::move(g_pendingDiscovery);
        g_pendingDiscovery.clear();
        if (g_stopping) return 0;
        for (HWND taskbar : pending) {
            DWORD pid{};
            if (GetWindowThreadProcessId(taskbar, &pid) == GetCurrentThreadId() &&
                pid == GetCurrentProcessId() &&
                (ClassIs(taskbar, L"Shell_TrayWnd") || ClassIs(taskbar, L"Shell_SecondaryTrayWnd")))
                DiscoverExisting(taskbar);
        }
        return 0;
    }
    if (message == kQueryTrayCatalogMessage) {
        QueryTrayCatalog(reinterpret_cast<HWND>(wp));
        return 0;
    }
    if (message == kRefreshTrayImagesMessage) {
        ActiveScope active;
        RefreshLoadedTrayIcons();
        return 0;
    }
    if (message == kQueryPreviewMetricsMessage) {
        ActiveScope active;
        auto editor = reinterpret_cast<HWND>(wp);
        if (g_stopping || editor != g_editorWindow || GetCurrentThreadId() != g_ownerThread) return 0;
        auto metrics = MeasureTaskbarPreview(reinterpret_cast<HMONITOR>(lp));
        { std::lock_guard lock(g_previewMetricsMutex); g_previewMetrics = metrics; }
        if (editor == g_editorWindow) PostMessageW(editor, kPreviewMetricsMessage, 0, 0);
        return 0;
    }
    if (message == kApplySettingsMessage) {
        ApplySavedSettings(reinterpret_cast<HWND>(wp));
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
        RefreshItemStates();
        return 0;
    }
    if (message == WM_TIMER && wp == kLiquidTimerId) {
        ApplyPendingForegroundSamples();
        bool active = RefreshTrayImageLoads();
        std::vector<winrt::weak_ref<FrameworkElement>> retries;
        ULONGLONG now = GetTickCount64();
        for (auto& root : g_roots) {
            auto& dock = root.dock;
            if (g_fontColorMode == FontColorMode::Adaptive &&
                (!root.foregrounds.empty() || !g_customTrayIcons.empty()) &&
                root.measurementsReady) {
                active = true;
                UpdateAdaptiveForeground(root);
            }
            if (dock.liquid) {
                active = true;
                HRESULT failure = dock.liquid->Failure();
                if (FAILED(failure)) {
                    Wh_Log(L"[UWLIQUID] renderer lost hr=%08X", failure);
                    StopLiquid(root);
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
        if (g_liquidHandoff.Expire(now)) active = true;
        for (auto const& frame : retries) OnLayout(frame);
        if (!active) {
            KillTimer(window, kLiquidTimerId);
            g_liquidTimerArmed = false;
        }
        return 0;
    }
    if (message == kRestoreMessage) {
        g_pendingPlateFills.clear();
        g_plateFillMessagePending=false;
        KillTimer(window, kDiscoveryTimerId);
        g_pendingDiscovery.clear();
        RestoreSettingsMenus();
        g_liquidPreparePending = false;
        g_itemStateRefreshPending = false;
        KillTimer(window, kLiquidTimerId);
        g_liquidTimerArmed = false;
        ApplyPendingForegroundSamples();
        StopLiquidMotionTracking();
        if (g_panelLocationHook) {
            UnhookWinEvent(g_panelLocationHook);
            g_panelLocationHook = nullptr;
        }
        for (auto& root : g_roots) RetireRoot(root);
        g_liquidHandoff.Release();
        ReleaseVector(g_roots);
        ClearLoadedTrayViews();
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
    if (g_dispatch) InstallSettingsMenuHooks();
    return g_dispatch != nullptr;
}

bool QueueTaskbarDiscovery(HWND taskbar) {
    if (g_stopping || !EnsureDispatcher()) return false;
    g_pendingDiscovery.insert(taskbar);
    // A timer avoids a repost loop if a nested COM message pump runs while the
    // layout is busy. Discovery never executes inside the synchronous hook.
    return SetTimer(g_dispatch, kDiscoveryTimerId, 50, nullptr) != 0;
}

void WatchFrame(FrameworkElement frame) {
    if (!frame || !EnsureDispatcher()) return;
    for (auto& root : g_roots)
        if (!root.retired && root.frame == frame) return;
    g_busy = true;
    struct Unbusy {
        ~Unbusy() { g_busy = false; PostItemStateRefresh(); }
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
        frame.LayoutUpdated([weak](auto&&, auto&&) { OnLayout(weak,true); });
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
    if (g_busy) {
        QueueTaskbarDiscovery(taskbar);
        return;
    }
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
            message.wParam == reinterpret_cast<WPARAM>(&g_discoverMessage)) QueueTaskbarDiscovery(message.hwnd);
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

bool RequestDiscovery(HWND taskbar) {
    DWORD pid{};
    DWORD thread = GetWindowThreadProcessId(taskbar, &pid);
    if (!g_discoverMessage || g_stopping || !thread || pid != GetCurrentProcessId() ||
        (!ClassIs(taskbar, L"Shell_TrayWnd") && !ClassIs(taskbar, L"Shell_SecondaryTrayWnd"))) return false;
    if (thread == GetCurrentThreadId()) {
        return QueueTaskbarDiscovery(taskbar);
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
        {{LR"(protected: long __cdecl CTaskListWnd::_ComputeJumpViewPosition(struct ITaskBtnGroup *,int,struct Windows::Foundation::Point &,enum Windows::UI::Xaml::HorizontalAlignment &,enum Windows::UI::Xaml::VerticalAlignment &)const )"},
         &ComputeJumpViewPosition_Original, ComputeJumpViewPosition_Hook,
         true},
    };
    if (!WindhawkUtils::HookSymbols(module, symbols, ARRAYSIZE(symbols))) return false;
    Wh_Log(L"[UWJUMP] jump-list anchor hook %s",
           ComputeJumpViewPosition_Original ? L"resolved" : L"unavailable; jump lists stay native");
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
    }};
    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks)))
        return false;
    Wh_Log(L"[UWPOS] initialized v" WH_MOD_VERSION
           L"; screenshot-safe liquid glass, native animation follower, gap=12 theme=%d fontColor=%d trayGrayscale=%d heightMethod=automatic",
           static_cast<int>(g_theme), static_cast<int>(g_fontColorMode),
           g_grayscaleTrayIcons);
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
    if (module && GetModuleHandleW(L"SystemTray.dll") == module) HookTrayLoaded(module, true);
    if (module && GetModuleHandle(L"Taskbar.View.dll") == module &&
        TryHookTaskbarView(module, true))
        Wh_ModAfterInit();
    return module;
}

// ---- Mod lifecycle ------------------------------------------------------
void SetModConfiguration(settingsjson::JsonObject const& local) {
    g_dockCornerRadius = std::clamp(
        LocalInt(local, L"cornerRadius"), 0, 100);
    g_buttonCornerRadius = std::clamp(
        LocalInt(local, L"buttonCornerRadius"), 0, 100);
    g_buttonVerticalPadding = std::clamp(
        LocalInt(local, L"buttonVerticalPadding"), 0, 100);
    g_trayHighlightPadding=std::clamp(LocalInt(local,L"tray.highlightPadding"),0,100);
    g_trayHighlightRadius=std::clamp(LocalInt(local,L"tray.highlightRadius"),0,100);
    g_dockEdgeDistance = std::clamp(
        LocalInt(local, L"edgeDistance"), 0, 100);
    g_keepButtonRim = LocalBool(local, L"button.border");
    g_buttonLiquidLayer = LocalBool(local, L"button.liquidLayer");
    bool trayEnabled = LocalBool(local, L"tray.enabled");
    g_grayscaleTrayIcons = trayEnabled && LocalBool(local, L"tray.grayscale");
    g_customTrayIcons = trayEnabled ? ReadTrayOverrides(local) : TrayOverrides{};
    g_theme = ParseTheme(LocalString(local, L"theme"));
    g_fontColorMode = ParseFontColor(LocalString(local, L"fontColor"));
    g_adaptiveWindowMs = std::clamp(LocalInt(local, L"adaptive.windowSeconds"), 0, 1000) * 10;
    if (g_theme == DockTheme::Liquid) g_liquidSettings = ReadLiquidSettings(local);
    try {
        g_appliedSettingsDocument = SettingsDocument(local);
    } catch (...) {
        Wh_Log(L"[UWSET] settings document unavailable");
    }
}

void LoadModConfiguration() {
    struct Apartment {
        HRESULT initialized = RoInitialize(RO_INIT_MULTITHREADED);
        ~Apartment() { if (SUCCEEDED(initialized)) RoUninitialize(); }
    } apartment;
    settingsjson::JsonObject local{nullptr};
    try {
        g_settingsPath = LocalSettingsPath();
        local = ReadLocalSettings(g_settingsPath);
    } catch (...) {
        Wh_Log(L"[UWSET] local settings path unavailable; using built-in defaults");
    }
    SetModConfiguration(local);
}

void ApplySavedSettings(HWND editor) {
    ActiveScope active;
    if (g_stopping) return;
    if (g_busy) {
        PostMessageW(g_dispatch, kApplySettingsMessage, reinterpret_cast<WPARAM>(editor), 0);
        return;
    }
    bool applied = false;
    try {
        auto settings = ReadLocalSettings(g_settingsPath);
        auto document = settings ? SettingsDocument(settings) : std::string{};
        bool unchanged = settings && document == g_appliedSettingsDocument;
        bool uiOnly = settings && !unchanged && !g_appliedSettingsDocument.empty() &&
            SettingsDocument(settings, false) ==
                SettingsDocument(ParseSettingsToml(g_appliedSettingsDocument), false);
        if (settings && (uiOnly || (unchanged && g_customTrayIcons.empty()))) {
            g_appliedSettingsDocument = std::move(document);
            applied = true;
        } else if (settings) {
            {
                g_busy = true;
                struct Unbusy { ~Unbusy() { g_busy = false; PostItemStateRefresh(); } } unbusy;
                ++g_configurationGeneration;
                for (auto& root : g_roots) {
                    g_liquidHandoff.Hold(std::move(root.dock.liquid));
                    RetireRoot(root);
                }
                g_roots.clear();
                {
                    std::lock_guard lock(g_foregroundResultsMutex);
                    g_foregroundResults.clear();
                }
                SetModConfiguration(settings);
                if (g_theme != DockTheme::Liquid) g_liquidHandoff.Release();
            }
            Wh_ModAfterInit();
            RefreshLoadedTrayIcons();
            applied = !g_stopping;
        }
    } catch (...) {
        Wh_Log(L"[UWSET] could not apply saved settings");
    }
    if (editor && editor == g_editorWindow)
        PostMessageW(editor, kSettingsAppliedMessage, applied ? 0 : 1, 0);
}

BOOL Wh_ModInit() {
    LoadModConfiguration();
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
    LSTATUS alignmentRead = RegGetValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                    L"TaskbarAl", RRF_RT_REG_DWORD, nullptr, &alignment, &bytes);
    if (alignmentRead == ERROR_FILE_NOT_FOUND) {
        Wh_Log(L"[UWPOS] TaskbarAl not set; using the Windows default, Center");
    } else if (alignmentRead != ERROR_SUCCESS || alignment != 1) {
        Wh_Log(L"[UWPOS] centered native alignment required");
        return FALSE;
    }
    if (!ResolveStartupSymbols()) return FALSE;
    ResolveTrayPreferenceSymbols();
    WNDCLASS wc{};
    wc.lpfnWndProc = DispatchProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = kWindowClass;
    g_windowClass = RegisterClass(&wc);
    if (!g_windowClass) return FALSE;
    auto taskbarView = GetModuleHandleW(L"Taskbar.View.dll");
    auto systemTray = GetModuleHandleW(L"SystemTray.dll");
    if (taskbarView) {
        if (!TryHookTaskbarView(taskbarView, false)) {
            UnregisterClass(kWindowClass, wc.hInstance);
            g_windowClass = 0;
            return FALSE;
        }
    }
    if (systemTray) HookTrayLoaded(systemTray, false);
    if (!taskbarView || !systemTray) {
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
        Wh_Log(L"[UWPOS] waiting for taskbar XAML modules");
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
    std::vector<HWND> taskbars;
    for (auto name : {L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd"}) {
        HWND window{};
        for (int i = 0; i < 16 && (window = FindWindowEx(nullptr, window, name, nullptr)); ++i) {
            DWORD pid{};
            GetWindowThreadProcessId(window, &pid);
            if (pid != GetCurrentProcessId()) continue;
            taskbars.push_back(window);
        }
    }
    for (HWND window : taskbars)
        if (!RequestDiscovery(window)) Wh_Log(L"[UWPOS] startup dispatch failed taskbar=%p", window);
    RefreshTaskbarHeight();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"[UWLIQUID] unload thread=%lu owner=%lu", GetCurrentThreadId(), g_ownerThread.load());
    g_stopping = true;
    g_dockingDisabled = true;
    g_compositorProcessCache.Stop();
    CloseSettingsEditor();
    // Wait for mutation/discovery to finish before dispatching UI-thread cleanup.
    while (g_active) Sleep(1);
    if (HWND window = g_dispatch) SendMessage(window, kRestoreMessage, 0, 0);
    if (g_explorer) RefreshTaskbarHeight();
    // Unbounded on purpose: Windhawk unmaps the DLL after Wh_ModUninit, and a retirement
    // still inside a capture call would return into unmapped code.
    auto started = GetTickCount64();
    bool logged = false;
    while (g_active || g_retiring) {
        if (!logged && GetTickCount64() - started >= 2000) {
            logged = true;
            Wh_Log(L"[UWLIQUID] unload waiting for capture teardown");
        }
        Sleep(1);
    }
    RestorePanels(nullptr, !g_explorer);
}

void Wh_ModUninit() {
    if (g_windowClass) UnregisterClass(kWindowClass, GetModuleHandle(nullptr));
}

// ==WindhawkMod==
// @id              task-view-customizer
// @name            Task View Customizer
// @description     Customize Windows 11 Task View with desktop-only layouts, background materials, theme colors, highlights, animations, hover switching and New desktop button controls
// @version         1.0.0
// @author          Asteski
// @github          https://github.com/Asteski
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Task View Customizer

Customize Windows 11 Task View's layout and appearance using its existing
virtual desktop thumbnails and window overview. Choose where the desktop
list appears or show it without the window overview. Customize background
materials, theme colors, desktop highlights and the New desktop button.
Control hover switching, slide direction and scrolling speed while keeping
the native controls.

## Task View modes

- **Windows default**: keeps desktops at the bottom while applying the mod's
  layout and appearance settings.
  This is the default mode.
- **Full Task View with desktops at the top**: moves the desktop list above
  the window overview.
- **No Task View with Desktop list in the top**: shows only the desktop list
  near the top of the screen.
- **No Task View with Desktop list at the center**: shows only the desktop
  list, centered on the screen.
- **No Task View with Desktop list at the bottom**: shows only the desktop
  list near the bottom.

The mod does not change Alt+Tab or the desktop preview shown when hovering
over the taskbar's Task View button.

## Settings

### Task View behavior

Choose the mode, hide the New desktop button, or close Task View in any mode when
the Windows key is released. In list-only modes, Tab navigates desktops and
clicking outside the list closes it. Key-release closing requires holding
Windows while using the list, then releasing it.

### Desktop list layout

Choose full width, fit the desktop tiles, or set a percentage of the screen.
Adjust the frame margin. Taskbar clearance follows the taskbar's actual edge
and size on each monitor, including small/default taskbars and display scaling.
Space already excluded by Windows is not counted twice. Windows default mode
keeps desktops at the bottom while applying these sizing controls.
Enable **Prevent switching desktops on hover** to suppress delayed mouse-hover
selection of desktop thumbnails. Click a desktop to switch instead. The option
applies to all Task View modes and leaves keyboard navigation available.
**Desktop hover switching delay (ms)** sets the wait before that preview switches.
The default is **400 ms**, matching the current Windows mouse-hover delay read
by Task View. Enter 0–600000; blank or invalid text preserves the native delay.
Prevent switching desktops on hover takes priority over this setting.

### New desktop button

The button follows the last desktop by default. Enable **Move New desktop
button to the right** to keep it at the list's right edge while desktops scroll.
Width, stretch, background and plus-sign alignment controls apply in both
positions. Adjust the button width, hide its background, or position the plus sign.
**Stretch New desktop button to highlight height** extends the button to
match its hover/selection frame and hides the New desktop label. The width
and plus-sign alignment settings still apply. Turn the toggle off to restore
the original vertical layout. Stretching also applies to Windows default mode through the shared button layout.

### Background materials

Choose **Native**, **Solid color**, **Acrylic**, **Clear**, or experimental
**Mica**. Solid starts opaque and can be made translucent without blur.
Acrylic uses a live blurred backdrop and tint. Clear removes the material.
You can also hide the list background in list-only modes or remove its
outer border without removing desktop selection borders.

Material transparency accepts **0-100**. Zero keeps the material's default
strength; 100 makes Native, Solid and Acrylic backgrounds clear. Values
outside that range use default strength; **-1** is the default setting.
Thumbnails and labels remain visible, and native hover/focus brush alpha is
preserved rather than turning faint highlights opaque.

Mica uses a panel-local blurred wallpaper brush with color and luminosity
blends. Its transparency control reduces tint/luminosity while keeping the
wallpaper layer opaque. This is an experimental approximation of the native
window material: its appearance can differ from apps using Windows' DWM
Mica backdrop. It falls back to a solid color when unavailable.
Windows can also use Acrylic's solid fallback when transparency is disabled.
Hiding the desktop list background takes priority over material styling.

### Desktop highlights

Keep Windows' highlight or choose border only, background only, or both.
The highlight background opacity controls the hovered/selected desktop fill.

### Theme colors

Set background and border colors independently for dark and light mode.
Accepts **native**, **accent**, **#RRGGBB**, or **#AARRGGBB**.
`accent` follows the current Windows accent color; invalid values preserve
native colors. Background colors also provide Acrylic/Mica tint and custom
highlight fill. Border colors apply to panel borders and custom highlights.

### Full-screen backdrop

Keep the native backdrop, clear it, or use Acrylic with adjustable dimming.
The list-only disable-blur option keeps the wallpaper clear even when
Windows transparency is enabled. It is independent of the desktop strip's
background. An explicit backdrop choice overrides the disable-blur option.

### Animations and scrolling

Choose **Desktop list animation direction**: **From the bottom** (default) or
**From the top**, independently of Task View mode. This controls entrance and
exit motion. Disable Task View animations independently of the background
material. Adjust desktop reveal scrolling speed from 10-500%:
100% uses Windows' current speed, 50% is half speed, and 200% is double speed.

## Installation and use

Install as a local Windhawk mod, compile and enable it. Close and reopen
Task View after changing mode, materials or colors. Disabling the mod restores
saved native layout properties, brushes and bindings.

Task View is detected through Switcher's native Loaded callbacks and animation
targets. No Taskbar Styler or other XAML diagnostics consumer is required.
Windhawk downloads Windows debugging symbols when needed.

## Compatibility

Designed for Windows 11. Windows builds can change Task View's internal
control names and layout; unrecognized controls are left alone. Mica remains
experimental. Mixed-DPI monitors, desktop drag and drop, and interactions
with other shell customization mods can depend on the Windows build.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- General:
    - viewMode: "0"
      $name: Task View mode
      $options:
      - "0": Windows default
      - "1": Full Task View with desktops at the top
      - "2": No Task View with Desktop list in the top
      - "3": No Task View with Desktop list at the center
      - "4": No Task View with Desktop list at the bottom
      $description: "Windows default keeps desktops at the bottom; customization settings apply in all modes. List-only modes hide Task View's window overview and background. Choose slide direction separately under Animations and scrolling. Reopen Task View after changing this setting."
    - closeOnWindowsRelease: false
      $name: Close Task View when Windows key is released
      $description: "All modes: hold Windows while using Task View, then release it to close. Task View opened by clicking the taskbar button stays open unless you hold and release Windows. Does not affect Alt+Tab."
  $name: Task View behavior
- Animations:
    - desktopSlideDirection: bottom
      $name: Desktop list animation direction
      $description: "Entrance and exit direction for the desktop list in every Task View mode. Independent of desktop list position. Has no effect when Task View animations are disabled."
      $options:
      - bottom: From the bottom
      - top: From the top
    - disableListAnimations: false
      $name: Disable Task View animations
      $description: "Disables entrance and exit motion. Independent of background material and transparency."
    - scrollAnimationSpeed: 100
      $name: Desktop scroll animation speed (%)
      $description: "100 = current Windows speed (default). 50 = half speed. 200 = double speed. Range: 10–500. Applies to horizontal scrolling that reveals a desktop tile."
  $name: Animations and scrolling
- DesktopList:
    - preventHoverSwitch: false
      $name: Prevent switching desktops on hover
      $description: "Require a click to switch desktops instead of the delayed mouse-hover preview. Applies to Task View in all modes. Keyboard switching remains available. Reopen Task View after changing this option."
    - hoverSwitchDelayMs: "400"
      $name: Desktop hover switching delay (ms)
      $description: "Time before hovering a desktop switches its preview. Windows currently uses 400 ms (the system mouse-hover delay). Enter 0–600000; blank or invalid input follows the system delay. Prevent switching desktops on hover overrides this value. Reopen Task View after changing."
    - desktopFrameWidth: "full"
      $name: Desktop list background width
      $options:
      - full: Full available width
      - fit: Fit desktop tiles
      - percent: Percentage of screen
    - desktopFramePercent: 70
      $name: Desktop list width (% of screen)
      $description: "Used with Percentage of screen. Range: 10–100. Overflowing desktop tiles remain scrollable."
    - desktopFrameMargin: 8
      $name: Virtual desktop list frame margin (px)
      $description: "Extra space around the desktop list frame, outside the taskbar area. Taskbar clearance follows its actual edge, size and monitor DPI, including small taskbars. Default: 8. Range: 0–200."
  $name: Desktop list layout
- NewDesktop:
    - moveToRight: false
      $name: Move New desktop button to the right
      $description: "Off: place the button immediately after the last desktop. On: keep it at the right edge while desktops scroll. Width, stretch and appearance settings apply in both positions."
    - stretchToHighlight: false
      $name: Stretch New desktop button to highlight height
      $description: "Extend the button vertically to match its selection/hover frame and hide the New desktop label in every mode. Keeps selected width and plus alignment."
    - newDesktopWidth: "100"
      $name: New desktop button width
      $options:
      - "100": 100% (default)
      - "75": 75%
      - "50": 50%
      - "25": 25%
      $description: "Width relative to a desktop tile. The button height stays unchanged."
    - hideNewDesktop: false
      $name: Hide New desktop button
    - disableBackground: false
      $name: Hide New desktop button background
      $description: "Hide the button background and hover borders while keeping the plus sign clickable."
    - alignment: center
      $name: Plus sign alignment within the tile
      $options:
      - topLeft: Top left
      - topCenter: Top center
      - topRight: Top right
      - middleLeft: Middle left
      - center: Center
      - middleRight: Middle right
      - bottomLeft: Bottom left
      - bottomCenter: Bottom center
      - bottomRight: Bottom right
  $name: New desktop button
- Materials:
    - materialStyle: native
      $name: Background material style
      $options:
      - native: Native Windows material
      - solid: Solid color (no blur)
      - acrylic: Acrylic
      - clear: Clear (no background)
      - mica: Mica (wallpaper material, experimental)
      $description: "Solid starts opaque; the transparency percentage can make it translucent without blur. Acrylic blurs content behind panels. Clear removes the material. Mica uses blurred wallpaper with a theme-colored tint, falling back to solid if unavailable. Disable desktop list background takes priority."
    - materialTransparency: -1
      $name: Task View background material transparency (%)
      $description: "0 = default material strength; 100 = clear for Native, Solid and Acrylic. For Mica, reduces tint/luminosity to reveal more wallpaper while keeping the wallpaper layer opaque. Thumbnails and labels stay solid. Outside 0–100 uses default strength (-1). Hidden strip settings take priority. Reopen Task View after changing."
    - removeListBorder: false
      $name: Remove desktop list background border
      $description: "Removes the outer desktop strip frame, without removing desktop selection borders."
    - disableListBackground: true
      $name: Disable desktop list background
      $description: "Hide the desktop list's background, including blur when Windows transparency effects are enabled. Applies to every Task View mode."
  $name: Background materials
- Highlights:
    - highlightStyle: native
      $name: Desktop highlight style
      $options:
      - native: Native Windows highlight
      - border: Border only
      - fill: Background only
      - both: Border and background
    - highlightOpacity: 20
      $name: Highlight background opacity (%)
      $description: "Accent-colored fill for hovered or selected desktops. Range: 0–100. Used with Background only and Border and background."
  $name: Desktop highlights
- Colors:
    - backgroundColorDark: "native"
      $name: Background color (dark mode)
      $description: "native, accent, #RRGGBB or #AARRGGBB. Solid defaults to #202020. Acrylic/Mica use this as their tint. Reopen Task View after changing colors."
    - borderColorDark: "native"
      $name: Border color (dark mode)
      $description: "native, accent, #RRGGBB or #AARRGGBB. Applies to panel borders and custom desktop highlights. Invalid values preserve native colors."
    - backgroundColorLight: "native"
      $name: Background color (light mode)
      $description: "native, accent, #RRGGBB or #AARRGGBB. Solid defaults to #F3F3F3."
    - borderColorLight: "native"
      $name: Border color (light mode)
      $description: "native, accent, #RRGGBB or #AARRGGBB. accent follows the current Windows accent color."
  $name: Theme colors
- Backdrop:
    - backdropBlur: native
      $name: Backdrop Blur
      $options:
      - native: Follow native background / disable-blur setting
      - off: Off (clear full-screen background)
      - acrylic: Acrylic backdrop
      $description: "Separate from panel materials. An explicit backdrop selection overrides Disable full-screen background blur. Acrylic uses a live XAML backdrop, rather than SWS's captured snapshot; Windows may use its solid fallback when system transparency is disabled."
    - backdropDimOpacity: 18
      $name: Backdrop Dim Opacity (%)
      $description: "Black tint strength of the Acrylic backdrop, 0–100. Separate from panel transparency."
    - disableFullscreenBlur: true
      $name: Disable full-screen background blur
      $description: "Keep the wallpaper clear even when Windows transparency effects are enabled. Independent of the desktop thumbnail strip background. Applies to every Task View mode. Reopen Task View after changing this setting."
  $name: Full-screen backdrop
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_utils.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.Graphics.Effects.h>

#include <atomic>
#include <cmath>
#include <memory>
#include <mutex>
#include <vector>
#include <string>
#include <optional>
#include <functional>

namespace wf = winrt::Windows::Foundation;
namespace xaml = winrt::Windows::UI::Xaml;
namespace controls = winrt::Windows::UI::Xaml::Controls;
namespace media = winrt::Windows::UI::Xaml::Media;
namespace animation = winrt::Windows::UI::Xaml::Media::Animation;

// Composition effects interop isn't shipped with Windhawk's SDK headers.
struct EffectInterop : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetEffectId(GUID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR, UINT*, UINT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperty(UINT, winrt::impl::abi_t<wf::IPropertyValue>**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSource(UINT, void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSourceCount(UINT*) = 0;
};
template <> inline constexpr winrt::guid winrt::impl::guid_v<EffectInterop>{
    0x2FC57384, 0xA068, 0x44D7, {0xA3, 0x31, 0x30, 0x98, 0x2F, 0xCF, 0x71, 0x77}};

namespace {
std::atomic<bool> g_stopping{false};
std::atomic<unsigned> g_settingsRevision{0};
std::atomic<bool> g_hooksReady{false};
std::atomic_flag g_installing = ATOMIC_FLAG_INIT;
std::atomic<int> g_scrollSpeed{100};
std::atomic<bool> g_desktopSlideFromTop{false};
std::atomic<int> g_desktopFrameMargin{8};
std::atomic<int> g_viewMode{0};
bool IsListOnlyMode() { int mode = g_viewMode.load(); return mode >= 2 && mode <= 4; }
std::atomic<bool> g_hideNewDesktop{false};
std::atomic<bool> g_preventHoverSwitch{false};
std::atomic<int> g_hoverSwitchDelayMs{400};
std::atomic<bool> g_newDesktopMoveToRight{false};
std::atomic<int> g_newDesktopWidth{100};
std::atomic<bool> g_newDesktopNoBackground{false};
std::atomic<bool> g_newDesktopStretch{false};
std::atomic<int> g_newDesktopAlignment{4};
std::atomic<int> g_frameWidthMode{0};
std::atomic<int> g_framePercent{70};
std::atomic<bool> g_disableListAnimations{false};
std::atomic<bool> g_disableListBackground{true};
std::atomic<bool> g_disableFullscreenBlur{true};
std::atomic<int> g_materialTransparency{-1};
std::atomic<int> g_materialStyle{0}, g_highlightStyle{0}, g_highlightOpacity{20};
std::atomic<int> g_backdropBlur{0}, g_backdropDimOpacity{18};
std::atomic<bool> g_closeOnWindowsRelease{true};
std::atomic<bool> g_removeListBorder{false};
std::mutex g_colorMutex;
std::wstring g_colors[4];
std::atomic<int> g_colorRevision{0};
void LoadFrameSettings() {
    ++g_settingsRevision;
    g_preventHoverSwitch = Wh_GetIntSetting(L"DesktopList.preventHoverSwitch") != 0;
    auto hoverDelay = Wh_GetStringSetting(L"DesktopList.hoverSwitchDelayMs");
    int64_t delay = 0;
    bool validDelay = hoverDelay && *hoverDelay;
    for (auto p = hoverDelay; validDelay && p && *p; ++p) {
        if (*p < L'0' || *p > L'9') { validDelay = false; break; }
        delay = delay * 10 + (*p - L'0');
        if (delay > 600000) validDelay = false;
    }
    g_hoverSwitchDelayMs = validDelay ? static_cast<int>(delay) : -1;
    Wh_FreeStringSetting(hoverDelay);
    g_newDesktopMoveToRight = Wh_GetIntSetting(L"NewDesktop.moveToRight") != 0;
    g_newDesktopNoBackground = Wh_GetIntSetting(L"NewDesktop.disableBackground") != 0;
    g_newDesktopStretch = Wh_GetIntSetting(L"NewDesktop.stretchToHighlight") != 0;
    auto alignment = Wh_GetStringSetting(L"NewDesktop.alignment");
    PCWSTR alignments[] = {L"topLeft", L"topCenter", L"topRight", L"middleLeft", L"center", L"middleRight", L"bottomLeft", L"bottomCenter", L"bottomRight"};
    int alignmentIndex = 4;
    for (int i = 0; i < 9; ++i) if (alignment && wcscmp(alignment, alignments[i]) == 0) alignmentIndex = i;
    Wh_FreeStringSetting(alignment);
    g_newDesktopAlignment = alignmentIndex;
    auto buttonWidth = Wh_GetStringSetting(L"NewDesktop.newDesktopWidth");
    int percent = buttonWidth ? _wtoi(buttonWidth) : 100;
    Wh_FreeStringSetting(buttonWidth);
    g_newDesktopWidth = percent == 75 || percent == 50 || percent == 25 ? percent : 100;
    auto readChoice = [](PCWSTR key, std::initializer_list<PCWSTR> choices) {
        auto value = Wh_GetStringSetting(key);
        int result = 0, index = 0;
        for (auto choice : choices) { if (value && wcscmp(value, choice) == 0) result = index; index++; }
        Wh_FreeStringSetting(value);
        return result;
    };
    g_materialStyle = readChoice(L"Materials.materialStyle", {L"native", L"solid", L"acrylic", L"clear", L"mica"});
    g_removeListBorder = Wh_GetIntSetting(L"Materials.removeListBorder") != 0;
    PCWSTR colorKeys[] = {L"Colors.backgroundColorDark", L"Colors.backgroundColorLight", L"Colors.borderColorDark", L"Colors.borderColorLight"};
    { std::lock_guard lock(g_colorMutex);
      for (int i = 0; i < 4; i++) {
          auto value = Wh_GetStringSetting(colorKeys[i]);
          g_colors[i] = value ? value : L"native";
          Wh_FreeStringSetting(value);
      }
    }
    ++g_colorRevision;
    g_highlightStyle = readChoice(L"Highlights.highlightStyle", {L"native", L"border", L"fill", L"both"});
    g_highlightOpacity = (std::max)(0, (std::min)(100, Wh_GetIntSetting(L"Highlights.highlightOpacity")));
    g_backdropBlur = readChoice(L"Backdrop.backdropBlur", {L"native", L"off", L"acrylic"});
    g_backdropDimOpacity = (std::max)(0, (std::min)(100, Wh_GetIntSetting(L"Backdrop.backdropDimOpacity")));
    g_closeOnWindowsRelease = Wh_GetIntSetting(L"General.closeOnWindowsRelease") != 0;
    g_disableListAnimations = Wh_GetIntSetting(L"Animations.disableListAnimations") != 0;
    auto slideDirection = Wh_GetStringSetting(L"Animations.desktopSlideDirection");
    g_desktopSlideFromTop = slideDirection && wcscmp(slideDirection, L"top") == 0;
    Wh_FreeStringSetting(slideDirection);
    g_disableListBackground = Wh_GetIntSetting(L"Materials.disableListBackground") != 0;
    g_disableFullscreenBlur = Wh_GetIntSetting(L"Backdrop.disableFullscreenBlur") != 0;
    // Do not clamp: an out-of-range value explicitly requests native opacity.
    g_materialTransparency = Wh_GetIntSetting(L"Materials.materialTransparency");
    g_hideNewDesktop = Wh_GetIntSetting(L"NewDesktop.hideNewDesktop") != 0;
    auto width = Wh_GetStringSetting(L"DesktopList.desktopFrameWidth");
    g_frameWidthMode = width && wcscmp(width, L"fit") == 0 ? 1 :
                       width && wcscmp(width, L"percent") == 0 ? 2 : 0;
    Wh_FreeStringSetting(width);
    g_framePercent = (std::max)(10, (std::min)(Wh_GetIntSetting(L"DesktopList.desktopFramePercent"), 100));
}
std::atomic<int> g_nativeScrollDuration{250};
thread_local bool g_insideLayout = false;
thread_local bool g_insideScroll = false;

struct LayoutGuard {
    bool previous = g_insideLayout;
    LayoutGuard() { g_insideLayout = true; }
    ~LayoutGuard() { g_insideLayout = previous; }
};

// Preserve local values and bindings, not just effective values. Clearing an
// originally unset property restores the theme/style's ownership of it.
struct SavedProperty {
    xaml::DependencyProperty property{nullptr};
    wf::IInspectable local{nullptr};
    xaml::Data::Binding binding{nullptr};

    SavedProperty() = default;
    SavedProperty(xaml::FrameworkElement const& element,
                  xaml::DependencyProperty const& dependencyProperty)
        : property(dependencyProperty), local(element.ReadLocalValue(property)) {
        if (auto expression = element.GetBindingExpression(property)) {
            binding = expression.ParentBinding();
        }
    }

    bool Restore(xaml::FrameworkElement const& element) const noexcept {
        try {
            if (binding) element.SetBinding(property, binding);
            else if (local == xaml::DependencyProperty::UnsetValue()) element.ClearValue(property);
            else element.SetValue(property, local);
            return true;
        } catch (...) { Wh_Log(L"Property restoration failed: %08X", winrt::to_hresult()); return false; }
    }
};

bool SameMargin(xaml::Thickness const& a, xaml::Thickness const& b) {
    return a.Left == b.Left && a.Top == b.Top && a.Right == b.Right &&
           a.Bottom == b.Bottom;
}

// Pixel rectangles may already exclude the taskbar. Only reserve the portion
// overlapping the actual XAML viewport, and convert it using that root's DPI.
xaml::Thickness TaskbarClearance(RECT viewport, RECT available, double scale) {
    if (scale <= 0) scale = 1;
    return {(std::max)(0L, available.left - viewport.left) / scale,
            (std::max)(0L, available.top - viewport.top) / scale,
            (std::max)(0L, viewport.right - available.right) / scale,
            (std::max)(0L, viewport.bottom - available.bottom) / scale};
}

xaml::Thickness GetTaskbarClearance(xaml::FrameworkElement const& element, DWORD threadId) {
    auto xamlRoot = element.XamlRoot();
    if (!xamlRoot) return {};
    double scale = xamlRoot.RasterizationScale();
    if (scale <= 0) scale = 1;
    auto size = xamlRoot.Size();
    struct HostSearch { double width, height, score = 1e30; HWND window = nullptr; POINT origin{}; };
    HostSearch search{size.Width * scale, size.Height * scale};
    auto candidate = [](HWND window, LPARAM parameter) __attribute__((stdcall)) -> BOOL {
        auto& search = *reinterpret_cast<HostSearch*>(parameter);
        wchar_t name[128]{};
        GetClassNameW(window, name, ARRAYSIZE(name));
        if (!IsWindowVisible(window) ||
            (wcscmp(name, L"MultitaskingViewFrame") && wcscmp(name, L"XamlExplorerHostIslandWindow") &&
             wcscmp(name, L"Windows.UI.Composition.DesktopWindowContentBridge"))) return TRUE;
        RECT rect{}; POINT origin{};
        if (!GetClientRect(window, &rect) || !ClientToScreen(window, &origin)) return TRUE;
        double score = std::abs((rect.right - rect.left) - search.width) +
                       std::abs((rect.bottom - rect.top) - search.height);
        if (score < search.score) { search.score = score; search.window = window; search.origin = origin; }
        return TRUE;
    };
    struct Enumeration { decltype(candidate) visit; HostSearch* search; } enumeration{candidate, &search};
    EnumThreadWindows(threadId, [](HWND window, LPARAM parameter) __attribute__((stdcall)) -> BOOL {
        auto& context = *reinterpret_cast<Enumeration*>(parameter);
        context.visit(window, reinterpret_cast<LPARAM>(context.search));
        EnumChildWindows(window, context.visit, reinterpret_cast<LPARAM>(context.search));
        return TRUE;
    }, reinterpret_cast<LPARAM>(&enumeration));
    POINT cursor{}; GetCursorPos(&cursor);
    HMONITOR monitor = search.window ? MonitorFromWindow(search.window, MONITOR_DEFAULTTONEAREST) :
                                      MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info{sizeof(info)};
    if (!GetMonitorInfoW(monitor, &info)) return {};
    RECT viewport = info.rcMonitor;
    if (search.window) {
        auto offset = element.TransformToVisual(xamlRoot.Content()).TransformPoint({0, 0});
        viewport.left = search.origin.x + static_cast<LONG>(std::lround(offset.X * scale));
        viewport.top = search.origin.y + static_cast<LONG>(std::lround(offset.Y * scale));
        viewport.right = viewport.left + static_cast<LONG>(std::lround(element.ActualWidth() * scale));
        viewport.bottom = viewport.top + static_cast<LONG>(std::lround(element.ActualHeight() * scale));
    }
    struct BarSearch { HMONITOR monitor; RECT screen, available; bool found = false; } bar{monitor, info.rcMonitor, info.rcWork};
    auto inspectBar = [](HWND window, LPARAM parameter) -> BOOL {
        auto& bar = *reinterpret_cast<BarSearch*>(parameter);
        wchar_t name[64]{}; GetClassNameW(window, name, ARRAYSIZE(name));
        if ((wcscmp(name, L"Shell_TrayWnd") && wcscmp(name, L"Shell_SecondaryTrayWnd")) ||
            !IsWindowVisible(window) || MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST) != bar.monitor) return TRUE;
        RECT rect{}, visible{};
        if (!GetWindowRect(window, &rect) || !IntersectRect(&visible, &rect, &bar.screen)) return TRUE;
        bar.available = bar.screen;
        if (rect.right - rect.left >= rect.bottom - rect.top) {
            if (std::abs(rect.top - bar.screen.top) <= std::abs(rect.bottom - bar.screen.bottom))
                bar.available.top = visible.bottom;
            else bar.available.bottom = visible.top;
        } else {
            if (std::abs(rect.left - bar.screen.left) <= std::abs(rect.right - bar.screen.right))
                bar.available.left = visible.right;
            else bar.available.right = visible.left;
        }
        bar.found = true;
        return FALSE;
    };
    if (auto primary = FindWindowW(L"Shell_TrayWnd", nullptr)) inspectBar(primary, reinterpret_cast<LPARAM>(&bar));
    for (HWND secondary = nullptr; !bar.found && (secondary = FindWindowExW(nullptr, secondary, L"Shell_SecondaryTrayWnd", nullptr));)
        inspectBar(secondary, reinterpret_cast<LPARAM>(&bar));
    return TaskbarClearance(viewport, bar.available, scale);
}

void ClearAnimationTargets();

struct ScrollCallGuard {
    ScrollCallGuard() { g_insideScroll = true; }
    ~ScrollCallGuard() { g_insideScroll = false; }
};

struct ScrollMotion : std::enable_shared_from_this<ScrollMotion> {
    winrt::weak_ref<controls::ScrollViewer> scroll;
    winrt::event_token renderingToken{}, viewChangedToken{};
    bool rendering = false;
    bool observing = false;
    ULONGLONG start = 0, nativeStart = 0;
    double from = 0, target = 0, duration = 0;

    void CancelRendering() noexcept {
        if (!rendering) return;
        rendering = false;
        try { media::CompositionTarget::Rendering(renderingToken); }
        catch (...) { Wh_Log(L"Scroll rendering detach failed: %08X", winrt::to_hresult()); }
    }
    void Stop() {
        CancelRendering();
        if (observing) {
            observing = false;
            try { if (auto control = scroll.get()) control.ViewChanged(viewChangedToken); }
            catch (...) { Wh_Log(L"Scroll ViewChanged detach failed: %08X", winrt::to_hresult()); }
        }
    }
    bool Request(double offset, int speed) {
        auto control = scroll.get();
        if (!control) return false;
        offset = (std::max)(0.0, (std::min)(offset, control.ScrollableWidth()));
        if (g_disableListAnimations) {
            CancelRendering();
            nativeStart = 0;
            ScrollCallGuard guard;
            return control.ChangeView(wf::IReference<double>{offset}, nullptr, nullptr, true);
        }
        if (!observing) {
            viewChangedToken = control.ViewChanged([weak = weak_from_this()](auto const&, auto const& args) {
                if (auto current = weak.lock(); current && current->nativeStart && !args.IsIntermediate()) {
                    auto elapsed = GetTickCount64() - current->nativeStart;
                    current->nativeStart = 0;
                    if (elapsed >= 40 && elapsed <= 2000) {
                        g_nativeScrollDuration = static_cast<int>(elapsed);
                    }
                }
            });
            observing = true;
        }
        if (speed == 100) {
            CancelRendering();
            nativeStart = GetTickCount64();
            ScrollCallGuard guard;
            bool changed = control.ChangeView(wf::IReference<double>{offset}, nullptr, nullptr, false);
            if (!changed) nativeStart = 0;
            return changed;
        }
        nativeStart = 0;
        if (rendering && target == offset) return true;
        CancelRendering();
        from = control.HorizontalOffset();
        target = offset;
        if (std::abs(target - from) < 0.5) return false;
        duration = static_cast<double>(g_nativeScrollDuration.load()) * 100.0 / speed;
        start = GetTickCount64();
        renderingToken = media::CompositionTarget::Rendering([weak = weak_from_this()](auto const&, auto const&) {
            auto current = weak.lock();
            if (!current) return;
            try {
                auto control = current->scroll.get();
                if (!control || g_stopping) { current->CancelRendering(); return; }
                double progress = (std::min)(1.0, (GetTickCount64() - current->start) / current->duration);
                double remaining = 1.0 - progress;
                double eased = 1.0 - remaining * remaining * remaining;
                double offset = current->from + (current->target - current->from) * eased;
                { ScrollCallGuard guard;
                  control.ChangeView(wf::IReference<double>{offset}, nullptr, nullptr, true); }
                if (progress >= 1) current->CancelRendering();
            } catch (...) { current->CancelRendering(); }
        });
        rendering = true;
        return true;
    }
};

template <typename T>
T FindNamedChild(xaml::DependencyObject const& parent, wchar_t const* name, int depth = 0) {
    if (!parent || depth > 16) return T{nullptr};
    if (auto element = parent.try_as<xaml::FrameworkElement>(); element && element.Name() == name)
        return parent.try_as<T>();
    int count = media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        if (auto found = FindNamedChild<T>(media::VisualTreeHelper::GetChild(parent, i), name, depth + 1))
            return found;
    }
    return T{nullptr};
}

xaml::FrameworkElement FindDesktopTile(xaml::DependencyObject const& parent, int depth = 0) {
    if (!parent || depth > 16) return nullptr;
    if (winrt::get_class_name(parent) ==
        L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopElementThemed")
        return parent.try_as<xaml::FrameworkElement>();
    int count = media::VisualTreeHelper::GetChildrenCount(parent);
    for (int i = 0; i < count; i++) {
        if (auto found = FindDesktopTile(media::VisualTreeHelper::GetChild(parent, i), depth + 1))
            return found;
    }
    return nullptr;
}

void DisconnectWallpaperMaterial(media::Brush const& brush);

struct MaterialState {
    winrt::weak_ref<xaml::FrameworkElement> element;
    SavedProperty property;
    media::Brush original{nullptr}, replacement{nullptr};
    int percent = -1;
    int style = -1;
    int revision = -1;
    bool dark = false;
    bool wallpaperReplacement = false;
    void ReleaseReplacement() {
        if (wallpaperReplacement && replacement) DisconnectWallpaperMaterial(replacement);
        replacement = nullptr;
        wallpaperReplacement = false;
    }
    winrt::Windows::UI::Color color{};
    bool applied = false;
    void Restore() {
        if (!applied) return;
        if (wallpaperReplacement && replacement) DisconnectWallpaperMaterial(replacement);
        if (auto current = element.get()) {
            if (!property.Restore(current) && wallpaperReplacement) current.ClearValue(property.property);
        }
        applied = false;
        percent = -1;
        style = -1;
        ReleaseReplacement();
    }
};

using Color = winrt::Windows::UI::Color;
namespace composition = winrt::Windows::UI::Composition;
namespace effects = winrt::Windows::Graphics::Effects;

bool DarkTheme(xaml::FrameworkElement const& element) {
    return element.ActualTheme() == xaml::ElementTheme::Dark;
}
std::atomic<uint32_t> g_accentColor{0xFF0078D7};
Color AccentColor() {
    auto value = g_accentColor.load();
    return {static_cast<uint8_t>(value >> 24), static_cast<uint8_t>(value >> 16),
            static_cast<uint8_t>(value >> 8), static_cast<uint8_t>(value)};
}
bool CustomColor(xaml::FrameworkElement const& element, bool border, Color& color) {
    std::wstring value;
    { std::lock_guard lock(g_colorMutex); value = g_colors[(border ? 2 : 0) + (DarkTheme(element) ? 0 : 1)]; }
    if (_wcsicmp(value.c_str(), L"accent") == 0) { color = AccentColor(); return true; }
    if (value.size() != 7 && value.size() != 9) return false;
    if (value[0] != L'#') return false;
    uint32_t number = 0;
    for (size_t i = 1; i < value.size(); i++) {
        wchar_t c = value[i];
        int digit = c >= L'0' && c <= L'9' ? c - L'0' :
            c >= L'a' && c <= L'f' ? c - L'a' + 10 : c >= L'A' && c <= L'F' ? c - L'A' + 10 : -1;
        if (digit < 0) return false;
        number = (number << 4) | digit;
    }
    color = {static_cast<uint8_t>(value.size() == 7 ? 255 : number >> 24),
             static_cast<uint8_t>(number >> 16), static_cast<uint8_t>(number >> 8), static_cast<uint8_t>(number)};
    return true;
}

std::atomic<int> g_liveWallpaperEffects{0}, g_liveWallpaperMaterials{0};

struct WallpaperTintEffect : winrt::implements<WallpaperTintEffect,
    effects::IGraphicsEffect, effects::IGraphicsEffectSource, EffectInterop> {
    winrt::hstring name;
    uint32_t mode;
    effects::IGraphicsEffectSource background{nullptr}, foreground{nullptr};
    WallpaperTintEffect(uint32_t blendMode, effects::IGraphicsEffectSource const& back,
                        effects::IGraphicsEffectSource const& front)
        : mode(blendMode), background(back), foreground(front) { ++g_liveWallpaperEffects; }
    ~WallpaperTintEffect() { --g_liveWallpaperEffects; Wh_Log(L"WallpaperTintEffect released"); }
    winrt::hstring Name() { return name; }
    void Name(winrt::hstring const& value) { name = value; }
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override {
        // D2D1 Blend: unlike a source-over wash, Color and Luminosity blends
        // preserve the wallpaper's color variation in the Mica material.
        *id = {0x81c5b77b, 0x13f8, 0x4cdd, {0xad, 0x20, 0xc8, 0x90, 0x54, 0x7a, 0xc6, 0x5d}};
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR, UINT*, UINT*) noexcept override { return E_INVALIDARG; }
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override { *count = 1; return S_OK; }
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, winrt::impl::abi_t<wf::IPropertyValue>** value) noexcept override {
        if (index) return E_INVALIDARG;
        try { auto property = wf::PropertyValue::CreateUInt32(mode).as<wf::IPropertyValue>();
            *value = reinterpret_cast<winrt::impl::abi_t<wf::IPropertyValue>*>(winrt::detach_abi(property)); return S_OK; }
        catch (...) { return winrt::to_hresult(); }
    }
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, void** source) noexcept override {
        if (index > 1) return E_INVALIDARG;
        try { winrt::copy_to_abi(index ? foreground : background, *source); return S_OK; }
        catch (...) { return winrt::to_hresult(); }
    }
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override { *count = 2; return S_OK; }
};

struct WallpaperMaterial : media::XamlCompositionBrushBaseT<WallpaperMaterial> {
    winrt::weak_ref<xaml::FrameworkElement> element;
    Color tint;
    double strength;
    WallpaperMaterial(xaml::FrameworkElement const& target, Color color, double opacity)
        : element(winrt::make_weak(target)), tint(color), strength(opacity) {
        auto fallback = color; fallback.A = 255; FallbackColor(fallback);
        ++g_liveWallpaperMaterials;
    }
    ~WallpaperMaterial() { --g_liveWallpaperMaterials; Wh_Log(L"WallpaperMaterial released"); }
    void OnConnected() {
        try {
            auto target = element.get();
            if (!target) return;
            auto compositor = xaml::Hosting::ElementCompositionPreview::GetElementVisual(target).Compositor();
            auto wallpaper = compositor.TryCreateBlurredWallpaperBackdropBrush();
            if (!wallpaper) { Wh_Log(L"Mica wallpaper brush unavailable; using solid fallback"); return; }
            auto luminosity = winrt::make<WallpaperTintEffect>(23,
                composition::CompositionEffectSourceParameter(L"Wallpaper"),
                composition::CompositionEffectSourceParameter(L"Luminosity"));
            auto colorBlend = winrt::make<WallpaperTintEffect>(22, luminosity,
                composition::CompositionEffectSourceParameter(L"Tint"));
            auto effect = compositor.CreateEffectFactory(colorBlend.as<effects::IGraphicsEffect>()).CreateBrush();
            effect.SetSourceParameter(L"Wallpaper", wallpaper);
            auto luminosityColor = tint;
            luminosityColor.A = static_cast<uint8_t>(255 * strength);
            auto tintColor = tint;
            tintColor.A = static_cast<uint8_t>(tintColor.A * strength);
            effect.SetSourceParameter(L"Luminosity", compositor.CreateColorBrush(luminosityColor));
            effect.SetSourceParameter(L"Tint", compositor.CreateColorBrush(tintColor));
            CompositionBrush(effect);
            Wh_Log(L"Mica wallpaper material connected (tint %u, strength %.2f)", tint.A, strength);
        } catch (...) { Wh_Log(L"Mica wallpaper material unavailable: %08X; using solid fallback", winrt::to_hresult()); }
    }
    void Disconnect() {
        auto brush = CompositionBrush();
        CompositionBrush(nullptr);
        if (brush) brush.Close();
    }
    void OnDisconnected() { Disconnect(); }
};

void DisconnectWallpaperMaterial(media::Brush const& brush) {
    if (brush) winrt::get_self<WallpaperMaterial>(brush.as<media::IXamlCompositionBrushBaseOverrides>())->Disconnect();
}

struct BorderState {
    winrt::weak_ref<controls::Border> element;
    SavedProperty brush, thickness;
    media::SolidColorBrush replacement{nullptr};
    bool applied = false;
    bool removed = false;
    void Restore() {
        if (applied) if (auto target = element.get()) { brush.Restore(target); thickness.Restore(target); }
        applied = false;
        removed = false;
    }
};
void ApplyBorders(xaml::DependencyObject const& node, std::vector<BorderState>& saved, int depth = 0) {
    if (!node || depth > 24) return;
    if (auto border = node.try_as<controls::Border>()) {
        bool frame = border.Name() == L"VirtualDesktopBarBackground" || border.Name() == L"VirtualDesktopSwitcherBackground";
        bool remove = frame && g_removeListBorder;
        Color color{};
        bool custom = CustomColor(border, true, color);
        // Custom highlight borders have their own saved state.
        if (!(g_highlightStyle != 0 && border.Name() == L"MainBorder")) {
            BorderState* state = nullptr;
            for (auto& candidate : saved) if (candidate.element.get() == border) { state = &candidate; break; }
            if (!state && (remove || custom)) {
                saved.emplace_back(); state = &saved.back(); state->element = winrt::make_weak(border);
                state->brush = SavedProperty(border, controls::Border::BorderBrushProperty());
                state->thickness = SavedProperty(border, controls::Border::BorderThicknessProperty());
            }
            if (state) {
                if (!remove && !custom) state->Restore();
                else {
                    if (remove) {
                        if (!SameMargin(border.BorderThickness(), {})) border.BorderThickness({});
                    } else if (state->removed) state->thickness.Restore(border);
                    state->removed = remove;
                    if (custom) {
                        if (!state->replacement) state->replacement = media::SolidColorBrush(color);
                        else state->replacement.Color(color);
                        if (border.BorderBrush() != state->replacement) border.BorderBrush(state->replacement);
                    } else if (state->applied) state->brush.Restore(border);
                    state->applied = true;
                }
            }
        }
    }
    for (int i = 0, count = media::VisualTreeHelper::GetChildrenCount(node); i < count; i++)
        ApplyBorders(media::VisualTreeHelper::GetChild(node, i), saved, depth + 1);
}

struct HighlightState {
    winrt::weak_ref<xaml::FrameworkElement> tile, container;
    winrt::weak_ref<controls::Grid> panel;
    winrt::weak_ref<controls::Border> border;
    SavedProperty background, borderBrush, borderThickness, primaryFocus, secondaryFocus;
    SavedProperty tilePrimaryFocus, tileSecondaryFocus;
    winrt::event_token entered{}, exited{};
    bool hovered = false, applied = false;
    std::function<void()> changed;
    media::SolidColorBrush fill{nullptr}, accent{nullptr}, clear{nullptr};
    void Restore() {
        if (!applied) return;
        if (auto current = panel.get()) background.Restore(current);
        if (auto current = border.get()) { borderBrush.Restore(current); borderThickness.Restore(current); }
        if (auto current = container.get()) { primaryFocus.Restore(current); secondaryFocus.Restore(current); }
        if (auto current = tile.get()) { tilePrimaryFocus.Restore(current); tileSecondaryFocus.Restore(current); }
        applied = false;
    }
    void Detach() noexcept {
        try { if (auto current = tile.get()) current.PointerEntered(entered); } catch (...) {}
        try { if (auto current = tile.get()) current.PointerExited(exited); } catch (...) {}
    }
    void Stop() { Detach(); Restore(); }
};

void ApplyMaterials(xaml::DependencyObject const& node,
                    std::vector<MaterialState>& saved, int depth = 0) {
    if (!node || depth > 24) return;
    auto element = node.try_as<xaml::FrameworkElement>();
    if (element) {
        auto name = element.Name();
        // These visuals are owned by the independent background-hide options.
        bool hiddenStrip = g_disableListBackground &&
            (name == L"VirtualDesktopBarBackground" || name == L"VirtualDesktopSwitcherBackground");
        if (hiddenStrip) return;
        xaml::DependencyProperty property{nullptr};
        if (element.try_as<controls::Border>()) property = controls::Border::BackgroundProperty();
        else if (element.try_as<controls::Panel>()) property = controls::Panel::BackgroundProperty();
        else if (element.try_as<controls::Control>()) property = controls::Control::BackgroundProperty();
        if (g_highlightStyle != 0 && name == L"MainGrid") {
            auto ancestor = media::VisualTreeHelper::GetParent(element);
            for (int level = 0; ancestor && level < 6; level++) {
                if (winrt::get_class_name(ancestor) ==
                    L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopElementThemed") {
                    property = nullptr; // Owned by the independent highlight setting.
                    break;
                }
                ancestor = media::VisualTreeHelper::GetParent(ancestor);
            }
        }
        if (property) {
            auto brush = element.GetValue(property).try_as<media::Brush>();
            MaterialState* state = nullptr;
            for (auto& candidate : saved) {
                if (candidate.element.get() == element) { state = &candidate; break; }
            }
            int percent = g_materialTransparency.load();
            int style = g_materialStyle.load();
            bool frame = name == L"VirtualDesktopBarBackground" || name == L"VirtualDesktopSwitcherBackground";
            Color configured{};
            bool custom = CustomColor(element, false, configured);
            if (brush) {
                // Capture changes from native visual states or other stylers.
                if (!state || (brush != state->original && brush != state->replacement)) {
                    auto solid = brush.try_as<media::SolidColorBrush>();
                    auto acrylic = brush.try_as<media::AcrylicBrush>();
                    // Leave intentionally clear surfaces and image brushes alone.
                    if (acrylic || (solid && ((solid.Color().A != 0 && brush.Opacity() != 0) || frame))) {
                        if (!state) {
                            saved.emplace_back();
                            state = &saved.back();
                        }
                        state->element = winrt::make_weak(element);
                        state->property = SavedProperty(element, property);
                        state->original = brush;
                        state->ReleaseReplacement();
                        state->percent = -1;
                        state->style = -1;
                        state->applied = false;
                        double nativeOpacity = acrylic ? acrylic.TintOpacity() : solid.Color().A / 255.0;
                        Wh_Log(L"Material %s: native tint/color transparency %.1f%%, brush opacity %.3f",
                               name.c_str(), (1.0 - nativeOpacity) * 100, brush.Opacity());
                    }
                }
                if (state && style == 0 && !custom && (percent < 0 || percent > 100)) {
                    state->Restore();
                } else if (state && (state->percent != percent || state->style != style ||
                           state->revision != g_colorRevision || state->color != configured || state->dark != DarkTheme(element))) {
                    double opacity = percent >= 0 && percent <= 100 ? 1.0 - percent / 100.0 : 1.0;
                    auto acrylic = state->original.try_as<media::AcrylicBrush>();
                    auto solid = state->original.try_as<media::SolidColorBrush>();
                    auto color = acrylic ? acrylic.TintColor() : solid ? solid.Color() : winrt::Windows::UI::Color{255, 32, 32, 32};
                    // Material conversion is limited to base panels; faint solid
                    // hover brushes retain their color and alpha.
                    bool basePanel = frame || acrylic || (solid && solid.Color().A * solid.Opacity() >= 128);
                    if (basePanel && custom) color = configured;
                    state->ReleaseReplacement();
                    if (style == 3 && basePanel) {
                        state->replacement = media::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0});
                    } else if (style == 1 && basePanel) {
                        if (!custom) color = DarkTheme(element) ? Color{255, 32, 32, 32} : Color{255, 243, 243, 243};
                        color.A = static_cast<uint8_t>(std::lround(color.A * opacity));
                        state->replacement = media::SolidColorBrush(color);
                    } else if (style == 4 && basePanel) {
                        if (!custom) color = DarkTheme(element) ? Color{204, 32, 32, 32} : Color{128, 243, 243, 243};
                        state->replacement = winrt::make<WallpaperMaterial>(element, color, opacity);
                        state->wallpaperReplacement = true;
                    } else if ((style == 2 && basePanel) || (style == 0 && acrylic)) {
                        auto original = acrylic;
                        media::AcrylicBrush replacement;
                        replacement.BackgroundSource(original ? original.BackgroundSource() : media::AcrylicBackgroundSource::Backdrop);
                        replacement.AlwaysUseFallback(original ? original.AlwaysUseFallback() : false);
                        replacement.TintColor(color);
                        replacement.TintOpacity(original ? original.TintOpacity() : 0.65);
                        if (original) replacement.TintLuminosityOpacity(original.TintLuminosityOpacity());
                        auto fallback = original && !custom ? original.FallbackColor() : color;
                        replacement.FallbackColor(fallback);
                        // Fade the material itself, including blur/noise. Tint
                        // alone may remain opaque due to the luminosity layer.
                        replacement.Opacity(state->original.Opacity() * opacity);
                        state->replacement = replacement;
                    } else if (auto original = state->original.try_as<media::SolidColorBrush>()) {
                        auto color = original.Color();
                        if (basePanel && custom) color = configured;
                        // Hover/focus brushes often use faint white. Increasing
                        // their alpha to the requested opacity creates white tiles.
                        color.A = static_cast<uint8_t>(std::lround(color.A * opacity));
                        state->replacement = media::SolidColorBrush(color);
                        state->replacement.Opacity(original.Opacity());
                    }
                    if (state->replacement) {
                        element.SetValue(property, state->replacement);
                        state->percent = percent;
                        state->style = style;
                        state->revision = g_colorRevision;
                        state->dark = DarkTheme(element);
                        state->color = configured;
                        state->applied = true;
                    }
                }
            }
        }
    }
    int count = media::VisualTreeHelper::GetChildrenCount(node);
    for (int i = 0; i < count; i++)
        ApplyMaterials(media::VisualTreeHelper::GetChild(node, i), saved, depth + 1);
}

struct LayoutState : std::enable_shared_from_this<LayoutState> {
    struct HoverTimer {
        winrt::weak_ref<xaml::DispatcherTimer> timer;
        winrt::Windows::Foundation::TimeSpan interval;
    };
    std::vector<HoverTimer> hoverTimers;
    struct NewDesktopTemplateProperty {
        winrt::weak_ref<xaml::FrameworkElement> element;
        SavedProperty property;
    };
    std::vector<NewDesktopTemplateProperty> newDesktopTemplateProperties;
    std::vector<NewDesktopTemplateProperty> newDesktopAppearanceProperties;
    std::vector<NewDesktopTemplateProperty> newDesktopStretchProperties;
    media::SolidColorBrush newDesktopClear{winrt::Windows::UI::Color{0,0,0,0}};

    void SaveNewDesktopAppearance(xaml::FrameworkElement const& element, xaml::DependencyProperty const& property) {
        for (auto const& saved : newDesktopAppearanceProperties)
            if (saved.element.get() == element && saved.property.property == property) return;
        newDesktopAppearanceProperties.push_back({winrt::make_weak(element), SavedProperty(element, property)});
    }

    void ApplyNewDesktopStretch(xaml::FrameworkElement const& button, bool allowStretch) {
        if (!g_newDesktopStretch || !allowStretch) {
            for (auto const& saved : newDesktopStretchProperties)
                if (auto element = saved.element.get()) saved.property.Restore(element);
            newDesktopStretchProperties.clear();
            return;
        }
        auto main = FindNamedChild<controls::Grid>(button, L"MainGrid");
        auto thumbnail = FindNamedChild<xaml::FrameworkElement>(button, L"ThumbnailButtonElement");
        if (!main || !thumbnail) return;
        auto save = [&](xaml::FrameworkElement const& element, xaml::DependencyProperty const& property) {
            for (auto const& saved : newDesktopStretchProperties)
                if (saved.element.get() == element && saved.property.property == property) return;
            newDesktopStretchProperties.push_back({winrt::make_weak(element), SavedProperty(element, property)});
        };
        auto hideLabel = [&](auto&& walk, xaml::DependencyObject const& node, int depth) -> void {
            if (!node || node == thumbnail || depth > 12) return;
            if (auto label = node.try_as<controls::TextBlock>()) {
                save(label, xaml::UIElement::VisibilityProperty());
                if (label.Visibility() != xaml::Visibility::Collapsed) label.Visibility(xaml::Visibility::Collapsed);
                return;
            }
            for (int i = 0, count = media::VisualTreeHelper::GetChildrenCount(node); i < count; i++)
                walk(walk, media::VisualTreeHelper::GetChild(node, i), depth + 1);
        };
        hideLabel(hideLabel, main, 0);
        // Occupy the same rows as the outer hover/selection frame, including
        // the space normally reserved for the desktop's title.
        auto frame = FindNamedChild<controls::Border>(main, L"MainBorder");
        save(thumbnail, controls::Grid::RowProperty());
        save(thumbnail, controls::Grid::RowSpanProperty());
        int row = frame ? controls::Grid::GetRow(frame) : 0;
        int span = frame ? controls::Grid::GetRowSpan(frame) : (std::max)(1, static_cast<int>(main.RowDefinitions().Size()));
        if (controls::Grid::GetRow(thumbnail) != row) controls::Grid::SetRow(thumbnail, row);
        if (controls::Grid::GetRowSpan(thumbnail) != span) controls::Grid::SetRowSpan(thumbnail, span);
        save(thumbnail, xaml::FrameworkElement::MarginProperty());
        auto margin = thumbnail.Margin();
        auto inset = frame ? frame.Margin() : xaml::Thickness{};
        // Preserve the visible horizontal inset at both vertical ends too.
        // Copying the frame margin alone made the stretched fill touch it.
        double gap = (std::max)(0.0, (std::max)(margin.Left - inset.Left, margin.Right - inset.Right));
        if (frame && frame.ActualWidth() > 0) {
            auto painted = FindNamedChild<controls::Grid>(thumbnail, L"RootGrid");
            if (painted && painted.ActualWidth() > 0) {
                auto paintOrigin = painted.TransformToVisual(main).TransformPoint({0, 0});
                auto frameOrigin = frame.TransformToVisual(main).TransformPoint({0, 0});
                double visibleGap = paintOrigin.X - frameOrigin.X;
                if (visibleGap > 0 && visibleGap <= 16) gap = visibleGap;
            }
        }
        // The first layout may not have arranged the template yet.
        if (gap <= 0) gap = 4;
        margin.Top = inset.Top + gap; margin.Bottom = inset.Bottom + gap;
        if (!SameMargin(thumbnail.Margin(), margin)) thumbnail.Margin(margin);
        auto stretch = [&](auto&& walk, xaml::DependencyObject const& node, int depth) -> void {
            if (!node || depth > 16) return;
            if (auto element = node.try_as<xaml::FrameworkElement>()) {
                // Leave the plus glyph's size/alignment under its own setting.
                if (element.try_as<controls::FontIcon>() || element.try_as<controls::TextBlock>()) return;
                for (auto const& property : {xaml::FrameworkElement::HeightProperty(),
                     xaml::FrameworkElement::MinHeightProperty(), xaml::FrameworkElement::MaxHeightProperty(),
                     xaml::FrameworkElement::VerticalAlignmentProperty()}) save(element, property);
                if (!std::isnan(element.Height())) element.Height(NAN);
                if (element.MinHeight() != 0) element.MinHeight(0);
                if (!std::isinf(element.MaxHeight())) element.MaxHeight(INFINITY);
                if (element.VerticalAlignment() != xaml::VerticalAlignment::Stretch)
                    element.VerticalAlignment(xaml::VerticalAlignment::Stretch);
            }
            for (int i = 0, count = media::VisualTreeHelper::GetChildrenCount(node); i < count; i++)
                walk(walk, media::VisualTreeHelper::GetChild(node, i), depth + 1);
        };
        stretch(stretch, thumbnail, 0);
    }

    void ApplyNewDesktopAppearance(xaml::FrameworkElement const& button, bool allowStretch = true) {
        ApplyNewDesktopStretch(button, allowStretch);
        // These decorative siblings are outside the thumbnail subtree. Native
        // pointer-over storyboards animate their opacity, so suppress visibility
        // rather than only clearing the normal-state thumbnail brushes.
        if (auto main = FindNamedChild<controls::Grid>(button, L"MainGrid")) {
            for (auto name : {L"MainBorder", L"BorderHighlight"}) {
                if (auto border = FindNamedChild<controls::Border>(main, name)) {
                    auto visibility = xaml::UIElement::VisibilityProperty();
                    if (g_newDesktopNoBackground) {
                        SaveNewDesktopAppearance(border, visibility);
                        if (border.Visibility() != xaml::Visibility::Collapsed)
                            border.Visibility(xaml::Visibility::Collapsed);
                    } else {
                        for (auto const& saved : newDesktopAppearanceProperties)
                            if (saved.element.get() == border && saved.property.property == visibility)
                                saved.property.Restore(border);
                    }
                }
            }
        }
        auto thumbnail = FindNamedChild<xaml::FrameworkElement>(button, L"ThumbnailButtonElement");
        if (!thumbnail) return;
        int index = g_newDesktopAlignment.load();
        xaml::HorizontalAlignment horizontal[] = {xaml::HorizontalAlignment::Left, xaml::HorizontalAlignment::Center, xaml::HorizontalAlignment::Right};
        xaml::VerticalAlignment vertical[] = {xaml::VerticalAlignment::Top, xaml::VerticalAlignment::Center, xaml::VerticalAlignment::Bottom};
        auto h = horizontal[index % 3];
        auto v = vertical[index / 3];
        auto visit = [&](auto&& walk, xaml::DependencyObject const& node, int depth) -> void {
            if (depth > 16) return;
            if (auto element = node.try_as<xaml::FrameworkElement>()) {
                if (auto icon = element.try_as<controls::FontIcon>()) {
                    SaveNewDesktopAppearance(icon, xaml::FrameworkElement::HorizontalAlignmentProperty());
                    SaveNewDesktopAppearance(icon, xaml::FrameworkElement::VerticalAlignmentProperty());
                    if (icon.HorizontalAlignment() != h) icon.HorizontalAlignment(h);
                    if (icon.VerticalAlignment() != v) icon.VerticalAlignment(v);
                    return;
                }
                if (auto presenter = element.try_as<controls::ContentPresenter>()) {
                    SaveNewDesktopAppearance(presenter, controls::ContentPresenter::HorizontalContentAlignmentProperty());
                    SaveNewDesktopAppearance(presenter, controls::ContentPresenter::VerticalContentAlignmentProperty());
                    if (presenter.HorizontalContentAlignment() != h) presenter.HorizontalContentAlignment(h);
                    if (presenter.VerticalContentAlignment() != v) presenter.VerticalContentAlignment(v);
                }
                xaml::DependencyProperty background{nullptr};
                xaml::DependencyProperty borderBrush{nullptr};
                xaml::DependencyProperty borderThickness{nullptr};
                if (element.try_as<controls::Panel>()) background = controls::Panel::BackgroundProperty();
                else if (element.try_as<controls::Border>()) background = controls::Border::BackgroundProperty();
                else if (element.try_as<controls::Control>()) background = controls::Control::BackgroundProperty();
                if (element.try_as<controls::Grid>()) borderBrush = controls::Grid::BorderBrushProperty();
                else if (element.try_as<controls::Border>()) borderBrush = controls::Border::BorderBrushProperty();
                else if (element.try_as<controls::Control>()) borderBrush = controls::Control::BorderBrushProperty();
                if (element.try_as<controls::Grid>()) borderThickness = controls::Grid::BorderThicknessProperty();
                else if (element.try_as<controls::Border>()) borderThickness = controls::Border::BorderThicknessProperty();
                else if (element.try_as<controls::Control>()) borderThickness = controls::Control::BorderThicknessProperty();
                if (borderThickness) {
                    if (g_newDesktopNoBackground) {
                        SaveNewDesktopAppearance(element, borderThickness);
                        if (!SameMargin(winrt::unbox_value<xaml::Thickness>(element.GetValue(borderThickness)), {}))
                            element.SetValue(borderThickness, winrt::box_value(xaml::Thickness{}));
                    } else {
                        for (auto const& saved : newDesktopAppearanceProperties)
                            if (saved.element.get() == element && saved.property.property == borderThickness)
                                saved.property.Restore(element);
                    }
                }
                for (auto const& property : {background, borderBrush}) if (property) {
                    if (g_newDesktopNoBackground) {
                        SaveNewDesktopAppearance(element, property);
                        if (element.GetValue(property) != newDesktopClear)
                            element.SetValue(property, newDesktopClear);
                    } else {
                        for (auto const& saved : newDesktopAppearanceProperties)
                            if (saved.element.get() == element && saved.property.property == property)
                                saved.property.Restore(element);
                    }
                }
            }
            int count = media::VisualTreeHelper::GetChildrenCount(node);
            for (int i = 0; i < count; ++i) walk(walk, media::VisualTreeHelper::GetChild(node, i), depth + 1);
        };
        visit(visit, thumbnail, 0);
    }

    void SaveNewDesktopTemplateProperty(xaml::FrameworkElement const& element,
                                       xaml::DependencyProperty const& property) {
        for (auto const& saved : newDesktopTemplateProperties)
            if (saved.element.get() == element && saved.property.property == property) return;
        newDesktopTemplateProperties.push_back({winrt::make_weak(element), SavedProperty(element, property)});
    }

    void RestoreNewDesktopTemplate() {
        for (auto const& saved : newDesktopTemplateProperties)
            if (auto element = saved.element.get()) saved.property.Restore(element);
        newDesktopTemplateProperties.clear();
    }

    void ResizeNewDesktopTemplate(xaml::FrameworkElement const& button) {
        if (g_newDesktopWidth == 100) { RestoreNewDesktopTemplate(); return; }
        if (auto main = FindNamedChild<controls::Grid>(button, L"MainGrid")) {
            for (auto const& child : main.Children()) {
                if (auto label = child.try_as<controls::TextBlock>()) {
                    if (g_newDesktopWidth == 25) {
                        SaveNewDesktopTemplateProperty(label, xaml::UIElement::OpacityProperty());
                        // Preserve its layout space to keep the button and
                        // highlight at their current vertical positions.
                        if (label.Opacity() != 0) label.Opacity(0);
                    } else {
                        for (auto const& saved : newDesktopTemplateProperties)
                            if (saved.element.get() == label && saved.property.property == xaml::UIElement::OpacityProperty())
                                saved.property.Restore(label);
                    }
                }
            }
        }
        auto resize = [&](xaml::FrameworkElement const& element, double width = NAN) {
            SaveNewDesktopTemplateProperty(element, xaml::FrameworkElement::WidthProperty());
            SaveNewDesktopTemplateProperty(element, xaml::FrameworkElement::MinWidthProperty());
            SaveNewDesktopTemplateProperty(element, xaml::FrameworkElement::HorizontalAlignmentProperty());
            if (std::isnan(width)) {
                if (!std::isnan(element.Width())) element.Width(NAN);
            } else {
                SaveNewDesktopTemplateProperty(element, xaml::FrameworkElement::MaxWidthProperty());
                if (element.MaxWidth() != width) element.MaxWidth(width);
                if (element.Width() != width) element.Width(width);
            }
            if (element.MinWidth() != 0) element.MinWidth(0);
            auto alignment = std::isnan(width) ? xaml::HorizontalAlignment::Stretch : xaml::HorizontalAlignment::Left;
            if (element.HorizontalAlignment() != alignment)
                element.HorizontalAlignment(alignment);
        };
        // Resize the native rounded visuals instead of clipping a full-width
        // thumbnail at the outer control's narrower bounds.
        if (auto main = FindNamedChild<controls::Grid>(button, L"MainGrid")) {
            resize(main);
            for (auto name : {L"MainBorder", L"BorderHighlight"})
                if (auto border = FindNamedChild<controls::Border>(main, name)) resize(border);
        }
        auto thumbnail = FindNamedChild<xaml::FrameworkElement>(button, L"ThumbnailButtonElement");
        if (!thumbnail) return;
        double innerWidth = button.Width();
        if (auto main = FindNamedChild<controls::Grid>(button, L"MainGrid")) {
            auto margin = main.Margin();
            auto padding = main.Padding();
            innerWidth -= margin.Left + margin.Right + padding.Left + padding.Right;
        }
        auto thumbnailMargin = thumbnail.Margin();
        innerWidth = (std::max)(1.0, innerWidth - thumbnailMargin.Left - thumbnailMargin.Right);
        if (auto background = FindNamedChild<controls::Grid>(thumbnail, L"RootGrid")) {
            auto radius = background.CornerRadius();
            if (radius.TopLeft == 0 && radius.TopRight == 0 && radius.BottomLeft == 0 && radius.BottomRight == 0) {
                SaveNewDesktopTemplateProperty(background, controls::Grid::CornerRadiusProperty());
                background.CornerRadius({4, 4, 4, 4});
            }
        }
        auto visit = [&](auto&& walk, xaml::DependencyObject const& node, int depth) -> void {
            if (depth > 16) return;
            if (auto element = node.try_as<xaml::FrameworkElement>()) {
                if (element.try_as<controls::FontIcon>()) return;
                if (!element.try_as<controls::TextBlock>()) resize(element, innerWidth);

            }
            int count = media::VisualTreeHelper::GetChildrenCount(node);
            for (int i = 0; i < count; ++i) walk(walk, media::VisualTreeHelper::GetChild(node, i), depth + 1);
        };
        visit(visit, thumbnail, 0);


    }
    std::vector<std::shared_ptr<HighlightState>> highlights;
    media::AcrylicBrush backdropAcrylic{nullptr};
    void ApplyHighlights() {
        auto list = FindNamedChild<controls::ListView>(desktops.get(), L"DesktopsList");
        if (!list) return;
        for (uint32_t i = 0; i < list.Items().Size(); i++) {
            auto container = list.ContainerFromIndex(i).try_as<xaml::FrameworkElement>();
            auto tile = FindDesktopTile(container);
            if (!tile || !container) continue;
            std::shared_ptr<HighlightState> saved;
            for (auto const& current : highlights) if (current->tile.get() == tile) { saved = current; break; }
            if (!saved) {
                if (g_highlightStyle == 0) continue;
                auto panel = FindNamedChild<controls::Grid>(tile, L"MainGrid");
                auto border = FindNamedChild<controls::Border>(tile, L"MainBorder");
                if (!panel || !border) continue;
                saved = std::make_shared<HighlightState>();
                saved->tile = winrt::make_weak(tile); saved->container = winrt::make_weak(container);
                saved->panel = winrt::make_weak(panel); saved->border = winrt::make_weak(border);
                saved->background = SavedProperty(panel, controls::Panel::BackgroundProperty());
                saved->borderBrush = SavedProperty(border, controls::Border::BorderBrushProperty());
                saved->borderThickness = SavedProperty(border, controls::Border::BorderThicknessProperty());
                saved->primaryFocus = SavedProperty(container, xaml::FrameworkElement::FocusVisualPrimaryThicknessProperty());
                saved->secondaryFocus = SavedProperty(container, xaml::FrameworkElement::FocusVisualSecondaryThicknessProperty());
                saved->tilePrimaryFocus = SavedProperty(tile, xaml::FrameworkElement::FocusVisualPrimaryThicknessProperty());
                saved->tileSecondaryFocus = SavedProperty(tile, xaml::FrameworkElement::FocusVisualSecondaryThicknessProperty());
                auto color = winrt::Windows::UI::Color{255, 0, 120, 215};
                try { color = winrt::unbox_value<winrt::Windows::UI::Color>(xaml::Application::Current().Resources().Lookup(winrt::box_value(L"SystemAccentColor"))); } catch (...) {}
                saved->accent = media::SolidColorBrush(color);
                color.A = static_cast<uint8_t>(g_highlightOpacity * 255 / 100);
                saved->fill = media::SolidColorBrush(color);
                saved->clear = media::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0});
                saved->changed = [weak = weak_from_this()] {
                    if (g_stopping || g_insideLayout) return;
                    try { if (auto state = weak.lock(); state && !state->detached) { LayoutGuard guard; state->ApplyHighlights(); } }
                    catch (...) { Wh_Log(L"Highlight event update failed: %08X", winrt::to_hresult()); }
                };
                saved->entered = tile.PointerEntered([weak = std::weak_ptr(saved)](auto const&, auto const&) {
                    if (auto current = weak.lock()) { current->hovered = true; if (current->changed) current->changed(); }
                });
                saved->exited = tile.PointerExited([weak = std::weak_ptr(saved)](auto const&, auto const&) {
                    if (auto current = weak.lock()) { current->hovered = false; if (current->changed) current->changed(); }
                });
                highlights.push_back(saved);
            }
            int style = g_highlightStyle.load();
            if (style == 0) { saved->Restore(); continue; }
            auto panel = saved->panel.get(); auto border = saved->border.get();
            if (!panel || !border) continue;
            bool active = saved->hovered || list.SelectedIndex() == static_cast<int>(i);
            auto color = AccentColor();
            CustomColor(border, true, color);
            if (saved->accent.Color() != color) saved->accent.Color(color);
            color = AccentColor();
            CustomColor(panel, false, color);
            color.A = static_cast<uint8_t>(color.A * g_highlightOpacity / 100);
            if (saved->fill.Color() != color) saved->fill.Color(color);
            auto background = active && style != 1 ? saved->fill : saved->clear;
            if (panel.Background() != background) panel.Background(background);
            auto brush = active && style != 2 ? saved->accent : saved->clear;
            if (border.BorderBrush() != brush) border.BorderBrush(brush);
            xaml::Thickness thickness = active && style != 2 ? xaml::Thickness{2, 2, 2, 2} : xaml::Thickness{};
            if (!SameMargin(border.BorderThickness(), thickness)) border.BorderThickness(thickness);
            if (!SameMargin(container.FocusVisualPrimaryThickness(), {})) container.FocusVisualPrimaryThickness({});
            if (!SameMargin(container.FocusVisualSecondaryThickness(), {})) container.FocusVisualSecondaryThickness({});
            if (!SameMargin(tile.FocusVisualPrimaryThickness(), {})) tile.FocusVisualPrimaryThickness({});
            if (!SameMargin(tile.FocusVisualSecondaryThickness(), {})) tile.FocusVisualSecondaryThickness({});
            saved->applied = true;
        }
    }
    winrt::event_token keyUpToken{}, unloadedToken{}, sizeToken{}, themeToken{};
    winrt::event_token selectionToken{}, containersToken{};
    winrt::weak_ref<controls::ListView> desktopList;
    bool DismissList() {
        HWND foreground = GetForegroundWindow();
        DWORD process = 0;
        DWORD thread = GetWindowThreadProcessId(foreground, &process);
        if (process != GetCurrentProcessId()) return false;
        GUITHREADINFO info{sizeof(info)};
        HWND target = foreground;
        if (GetGUIThreadInfo(thread, &info) && info.hwndFocus) target = info.hwndFocus;
        PostMessageW(target, WM_KEYDOWN, VK_ESCAPE, 0x00010001);
        PostMessageW(target, WM_KEYUP, VK_ESCAPE, 0xC0010001);
        return true;
    }
    void Navigate(int direction) {
        auto stack = desktopStack.get();
        auto list = FindNamedChild<controls::ListView>(stack, L"DesktopsList");
        if (!list || list.Items().Size() == 0) return;
        int count = static_cast<int>(list.Items().Size());
        int index = list.SelectedIndex();
        int next = index < 0 ? (direction < 0 ? count - 1 : 0) : (index + direction + count) % count;
        list.SelectedIndex(next);
        list.ScrollIntoView(list.Items().GetAt(next));
        if (auto container = list.ContainerFromIndex(next).try_as<controls::Control>())
            container.Focus(xaml::FocusState::Keyboard);
    }
    winrt::event_token keyToken{};
    winrt::event_token outsideClickToken{};
    SavedProperty originalInputBackground;
    bool inputBackgroundApplied = false;
    std::vector<MaterialState> materials;
    std::vector<BorderState> borders;
    struct ModeElement {
        winrt::weak_ref<xaml::FrameworkElement> element;
        SavedProperty opacity;
        SavedProperty hitTest;
    };
    std::vector<ModeElement> modeElements;
    std::vector<std::shared_ptr<ScrollMotion>> scrollMotions;
    DWORD threadId;
    winrt::weak_ref<xaml::FrameworkElement> root;
    winrt::weak_ref<xaml::FrameworkElement> desktops;
    winrt::weak_ref<xaml::FrameworkElement> windows;
    winrt::weak_ref<controls::Grid> grid;
    std::vector<controls::RowDefinition> originalRows;
    std::vector<SavedProperty> desktopProperties;
    std::vector<SavedProperty> windowProperties;
    struct SavedKeyFrame {
        animation::DoubleKeyFrame frame{nullptr};
        double original;
    };
    std::vector<SavedKeyFrame> animationFrames[2];
    winrt::event_token layoutToken{};
    bool applied = false;
    bool detached = false;
    int appliedMode = -1;
    bool appearanceDirty = true, clearanceDirty = true;
    unsigned appearanceRevision = 0;
    uint32_t desktopCount = UINT32_MAX;
    double clearanceWidth = -1, clearanceHeight = -1, clearanceScale = -1;
    xaml::Thickness cachedClearance{};
    winrt::Windows::UI::ViewManagement::UISettings uiSettings{nullptr};
    winrt::weak_ref<controls::Grid> desktopGrid;
    winrt::weak_ref<controls::StackPanel> desktopStack;
    winrt::weak_ref<controls::ScrollViewer> desktopScroll;
    winrt::weak_ref<xaml::FrameworkElement> newDesktopButton;
    winrt::weak_ref<xaml::FrameworkElement> desktopBackground;
    struct ListBackground {
        winrt::weak_ref<controls::Border> element;
        SavedProperty background;
        SavedProperty opacity;
        media::SolidColorBrush transparent{nullptr};
        bool hidden = false;
    };
    ListBackground listBackgrounds[2];
    ListBackground fullscreenBackground;
    std::vector<controls::ColumnDefinition> originalDesktopColumns;
    std::vector<SavedProperty> buttonProperties;
    std::vector<SavedProperty> scrollProperties;
    std::vector<SavedProperty> backgroundProperties;
    uint32_t originalButtonIndex = 0;
    bool buttonPinned = false;
    int buttonColumnMode = -1;
    SavedProperty originalScrollWidth, originalScrollMinWidth;
    bool inlineScrollSized = false;
    winrt::weak_ref<xaml::FrameworkElement> nativeButton;
    SavedProperty nativeButtonVisibility;
    SavedProperty nativeButtonWidth;
    SavedProperty nativeButtonMinWidth;
    bool nativeButtonResized = false;
    bool nativeButtonHidden = false;
    xaml::Thickness originalButtonMargin{};
    double originalButtonMinWidth = 0;
    xaml::Thickness originalScrollMargin{};

    void CenterDesktopList() {
        auto scroll = desktopScroll.get();
        auto stack = desktopStack.get();
        auto host = desktopGrid.get();
        auto button = newDesktopButton.get();
        if (!scroll || !stack || !host || !button || host.ActualWidth() <= 0) return;
        auto buttonMargin = button.Margin();
        double reservedRight = button.Visibility() == xaml::Visibility::Collapsed ? 0 :
            button.ActualWidth() + buttonMargin.Left + buttonMargin.Right;
        double free = host.ActualWidth() - reservedRight - originalScrollMargin.Left -
                      originalScrollMargin.Right - stack.DesiredSize().Width;
        // Balance the fixed button's space on the left when the list fits.
        // Release that space as the list fills, preserving the full scrolling
        // viewport and keeping thumbnails from overlapping the fixed button.
        auto margin = originalScrollMargin;
        // Fit mode centers the complete group, including New desktop, rather
        // than reserving an empty button-sized column on the left.
        if (g_frameWidthMode.load() != 1)
            margin.Left += (std::max)(0.0, (std::min)(reservedRight, free));
        if (!SameMargin(scroll.Margin(), margin)) scroll.Margin(margin);
    }

    void PositionNewDesktopButton() {
        auto host = desktopGrid.get(); auto scroll = desktopScroll.get();
        auto stack = desktopStack.get(); auto button = newDesktopButton.get();
        if (!host || !scroll || !stack || !button) return;
        int mode = g_newDesktopMoveToRight ? 1 : 0;
        if (buttonColumnMode != mode) {
            host.ColumnDefinitions().Clear();
            for (int i = 0, count = mode ? 2 : 4; i < count; i++) {
                controls::ColumnDefinition column;
                bool spacer = mode ? i == 0 : i == 0 || i == 3;
                column.Width({1, spacer ? xaml::GridUnitType::Star : xaml::GridUnitType::Auto});
                host.ColumnDefinitions().Append(column);
            }
            controls::Grid::SetColumn(scroll, mode ? 0 : 1);
            controls::Grid::SetColumn(button, mode ? 1 : 2);
            if (auto background = desktopBackground.get()) controls::Grid::SetColumnSpan(background, mode ? 2 : 4);
            buttonColumnMode = mode;
        }
        if (mode) {
            if (inlineScrollSized) {
                originalScrollWidth.Restore(scroll);
                originalScrollMinWidth.Restore(scroll);
                inlineScrollSized = false;
            }
            CenterDesktopList();
        } else {
            // Independent, bounded controls avoid the native template's stretch
            // feedback loop. Equal spacers center the desktop-and-button group.
            if (!SameMargin(scroll.Margin(), originalScrollMargin)) scroll.Margin(originalScrollMargin);
            double reserve = 0;
            if (button.Visibility() != xaml::Visibility::Collapsed) {
                auto margin = button.Margin();
                reserve = button.Width() + margin.Left + margin.Right;
            }
            double available = (std::max)(0.0, host.ActualWidth() - reserve -
                originalScrollMargin.Left - originalScrollMargin.Right);
            if (host.ActualWidth() > 0 && stack.DesiredSize().Width > 0) {
                double width = (std::min)(available, static_cast<double>(stack.DesiredSize().Width));
                if (scroll.MinWidth() != 0) scroll.MinWidth(0);
                if (scroll.Width() != width) scroll.Width(width);
                inlineScrollSized = true;
            }
        }
    }

    void MatchNewDesktopFrame() {
        auto button = newDesktopButton.get();
        auto stack = desktopStack.get();
        auto host = desktopGrid.get();
        if (!button || !stack || !host) return;
        auto tile = FindDesktopTile(stack);
        if (!tile || tile.ActualHeight() <= 0) return;
        // Align the pinned control with the desktop tiles, but keep its native
        // measured height unless stretching was explicitly requested. Its
        // title/thumbnail template has different chrome from a desktop tile;
        // forcing the tile's height clips the top/bottom border strokes.
        auto origin = tile.TransformToVisual(host).TransformPoint({0, 0});
        auto margin = originalButtonMargin;
        margin.Top = origin.Y - 2;
        margin.Right += 7;
        if (!SameMargin(button.Margin(), margin)) button.Margin(margin);
        if (button.VerticalAlignment() != xaml::VerticalAlignment::Top)
            button.VerticalAlignment(xaml::VerticalAlignment::Top);
        if (g_newDesktopStretch) {
            if (button.Height() != tile.ActualHeight()) button.Height(tile.ActualHeight());
        } else if (!std::isnan(button.Height())) {
            button.Height(NAN);
        }
        double width = tile.ActualWidth() * g_newDesktopWidth.load() / 100.0;
        double minWidth = g_newDesktopWidth == 100 ? originalButtonMinWidth : 0;
        if (button.MinWidth() != minWidth) button.MinWidth(minWidth);
        if (width > 0 && button.Width() != width) button.Width(width);
        ResizeNewDesktopTemplate(button);
        ApplyNewDesktopAppearance(button);
    }

    void PinNewDesktopButton(xaml::FrameworkElement const& desktopElement) {
        if (buttonPinned) return;
        auto host = FindNamedChild<controls::Grid>(desktopElement, L"GridElement");
        auto scroll = FindNamedChild<controls::ScrollViewer>(host, L"DesktopsListScrollViewer");
        auto stack = FindNamedChild<controls::StackPanel>(scroll, L"DesktopsListStackPanel");
        auto button = FindNamedChild<xaml::FrameworkElement>(stack, L"NewVirtualDesktopButtonThemed");
        if (!host || !scroll || !stack || !button ||
            media::VisualTreeHelper::GetParent(button) != stack) return;
        auto background = FindNamedChild<xaml::FrameworkElement>(host, L"VirtualDesktopBarBackground");
        uint32_t index = 0;
        if (!stack.Children().IndexOf(button, index)) return;

        for (auto const& column : host.ColumnDefinitions()) originalDesktopColumns.push_back(column);
        for (auto const& property : {controls::Grid::ColumnProperty(), controls::Grid::ColumnSpanProperty()}) {
            buttonProperties.emplace_back(button, property);
            scrollProperties.emplace_back(scroll, property);
            if (background) backgroundProperties.emplace_back(background, property);
        }
        for (auto const& property : {xaml::FrameworkElement::MarginProperty(),
                                    xaml::FrameworkElement::HeightProperty(),
                                    xaml::FrameworkElement::WidthProperty(),
                                    xaml::FrameworkElement::MinWidthProperty(),
                                    xaml::FrameworkElement::VerticalAlignmentProperty(),
                                    xaml::UIElement::VisibilityProperty()})
            buttonProperties.emplace_back(button, property);
        originalButtonMargin = button.Margin();
        originalButtonMinWidth = button.MinWidth();
        originalScrollMargin = scroll.Margin();
        originalScrollWidth = SavedProperty(scroll, xaml::FrameworkElement::WidthProperty());
        originalScrollMinWidth = SavedProperty(scroll, xaml::FrameworkElement::MinWidthProperty());
        scrollProperties.emplace_back(scroll, xaml::FrameworkElement::MarginProperty());
        // Move the native control, retaining its click handlers, accessibility,
        // theme and data bindings. Auto measures its current thumbnail size.
        stack.Children().RemoveAt(index);
        try {
            host.Children().Append(button);
        } catch (...) {
            stack.Children().InsertAt(index, button);
            throw;
        }
        desktopGrid = winrt::make_weak(host);
        desktopStack = winrt::make_weak(stack);
        desktopScroll = winrt::make_weak(scroll);
        newDesktopButton = winrt::make_weak(button);
        if (background) desktopBackground = winrt::make_weak(background);
        originalButtonIndex = index;
        buttonPinned = true;
        controls::ColumnDefinition listColumn;
        controls::ColumnDefinition buttonColumn;
        listColumn.Width({1, xaml::GridUnitType::Star});
        buttonColumn.Width({1, xaml::GridUnitType::Auto});
        host.ColumnDefinitions().Clear();
        host.ColumnDefinitions().Append(listColumn);
        host.ColumnDefinitions().Append(buttonColumn);
        controls::Grid::SetColumn(scroll, 0);
        controls::Grid::SetColumnSpan(scroll, 1);
        controls::Grid::SetColumn(button, 1);
        controls::Grid::SetColumnSpan(button, 1);
        if (background) {
            controls::Grid::SetColumn(background, 0);
            controls::Grid::SetColumnSpan(background, 2);
        }
    }

    void UnpinNewDesktopButton() {
        if (buttonPinned) {
            auto host = desktopGrid.get();
            auto stack = desktopStack.get();
            auto button = newDesktopButton.get();
            if (host && stack && button) {
                uint32_t index;
                if (host.Children().IndexOf(button, index)) {
                    host.Children().RemoveAt(index);
                    stack.Children().InsertAt((std::min)(originalButtonIndex, stack.Children().Size()), button);
                }
                for (auto const& property : buttonProperties) property.Restore(button);
            }
            if (auto scroll = desktopScroll.get())
            {
                for (auto const& property : scrollProperties) property.Restore(scroll);
                if (inlineScrollSized) {
                    originalScrollWidth.Restore(scroll);
                    originalScrollMinWidth.Restore(scroll);
                    inlineScrollSized = false;
                }
            }
            if (auto background = desktopBackground.get())
                for (auto const& property : backgroundProperties) property.Restore(background);
            if (host) {
                host.ColumnDefinitions().Clear();
                for (auto const& column : originalDesktopColumns) host.ColumnDefinitions().Append(column);
            }
            buttonPinned = false;
            buttonColumnMode = -1;
            originalDesktopColumns.clear(); buttonProperties.clear();
            scrollProperties.clear(); backgroundProperties.clear();
        }
    }

    void Apply() {
        if (g_insideLayout || g_stopping) return;
        LayoutGuard guard;
        auto desktopElement = desktops.get();
        auto windowElement = windows.get();
        if (!desktopElement || !windowElement) return;

        auto layoutGrid = grid.get();
        if (!layoutGrid) return;
        int mode = g_viewMode.load();
        bool listOnly = IsListOnlyMode();
        // The full-screen dimming/Acrylic border can belong to the outer
        // switcher host, rather than the TaskViewTimeline's RootGridElement.
        auto fullscreen = fullscreenBackground.element.get();
        if (!fullscreen) {
            xaml::DependencyObject ancestor = root.get();
            for (int depth = 0; ancestor && depth < 4; depth++) {
                fullscreen = FindNamedChild<controls::Border>(ancestor, L"BackgroundDimmingLayer");
                if (fullscreen) break;
                ancestor = media::VisualTreeHelper::GetParent(ancestor);
            }
            if (fullscreen) {
                fullscreenBackground.element = winrt::make_weak(fullscreen);
                fullscreenBackground.background = SavedProperty(fullscreen, controls::Border::BackgroundProperty());
                fullscreenBackground.opacity = SavedProperty(fullscreen, xaml::UIElement::OpacityProperty());
                fullscreenBackground.transparent = media::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0});
                fullscreenBackground.hidden = false;
                Wh_Log(L"Found full-screen Task View background");
            }
        }
        if (fullscreen) {
            int backdrop = g_backdropBlur.load();
            if (backdrop == 2) {
                if (!backdropAcrylic) {
                    backdropAcrylic = media::AcrylicBrush();
                    backdropAcrylic.BackgroundSource(media::AcrylicBackgroundSource::Backdrop);
                    backdropAcrylic.TintColor({255, 0, 0, 0});
                    backdropAcrylic.FallbackColor({255, 24, 24, 24});
                    backdropAcrylic.TintLuminosityOpacity(wf::IReference<double>{0.0});
                }
                double dim = g_backdropDimOpacity.load() / 100.0;
                if (backdropAcrylic.TintOpacity() != dim) backdropAcrylic.TintOpacity(dim);
                if (fullscreen.Background() != backdropAcrylic) fullscreen.Background(backdropAcrylic);
                if (fullscreen.Opacity() != 1) fullscreen.Opacity(1);
                fullscreenBackground.hidden = true;
            } else if (backdrop == 1 || g_disableFullscreenBlur) {
                if (fullscreen.Background() != fullscreenBackground.transparent)
                    fullscreen.Background(fullscreenBackground.transparent);
                if (fullscreen.Opacity() != 0) fullscreen.Opacity(0);
                fullscreenBackground.hidden = true;
            } else if (fullscreenBackground.hidden) {
                fullscreenBackground.background.Restore(fullscreen);
                fullscreenBackground.opacity.Restore(fullscreen);
                fullscreenBackground.hidden = false;
            }
        }
        // Windows uses separate backgrounds for the Task View desktop bar and
        // desktop switcher. Replace the brushes as well as hiding the visuals:
        // lowering opacity alone leaves an Acrylic/composition effect attached.
        wchar_t const* backgroundNames[] = {
            L"VirtualDesktopBarBackground", L"VirtualDesktopSwitcherBackground"};
        for (int i = 0; i < 2; i++) {
            auto& saved = listBackgrounds[i];
            auto background = saved.element.get();
            if (!background) {
                background = FindNamedChild<controls::Border>(desktopElement, backgroundNames[i]);
                if (!background) continue;
                saved.element = winrt::make_weak(background);
                saved.background = SavedProperty(background, controls::Border::BackgroundProperty());
                saved.opacity = SavedProperty(background, xaml::UIElement::OpacityProperty());
                saved.transparent = media::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0});
                saved.hidden = false;
                Wh_Log(L"Found desktop list background: %s", backgroundNames[i]);
            }
            if (g_disableListBackground) {
                if (background.Background() != saved.transparent) background.Background(saved.transparent);
                if (background.Opacity() != 0) background.Opacity(0);
                saved.hidden = true;
            } else if (saved.hidden) {
                saved.background.Restore(background);
                saved.opacity.Restore(background);
                saved.hidden = false;
            }
        }
        auto revision = g_settingsRevision.load();
        if (appearanceRevision != revision) { appearanceDirty = true; clearanceDirty = true; }
        if (uiSettings) {
            auto accent = uiSettings.GetColorValue(winrt::Windows::UI::ViewManagement::UIColorType::Accent);
            uint32_t packed = uint32_t(accent.A) << 24 | uint32_t(accent.R) << 16 | uint32_t(accent.G) << 8 | accent.B;
            if (g_accentColor.exchange(packed) != packed) appearanceDirty = true;
        }
        if (auto list = desktopList.get()) {
            auto count = list.Items().Size();
            if (desktopCount != count) { desktopCount = count; appearanceDirty = true; }
        }
        if (appearanceDirty) {
            appearanceDirty = false;
            try {
                ApplyMaterials(desktopElement, materials);
                ApplyMaterials(windowElement, materials);
                ApplyBorders(desktopElement, borders);
                ApplyBorders(windowElement, borders);
                ApplyHighlights();
                appearanceRevision = revision;
            } catch (...) { appearanceDirty = true; throw; }
        }
        // A transparent background receives clicks in the empty part of this
        // XAML surface without changing the underlying desktop's appearance.
        if (listOnly && !inputBackgroundApplied) {
            layoutGrid.Background(media::SolidColorBrush(winrt::Windows::UI::Color{0, 0, 0, 0}));
            inputBackgroundApplied = true;
        } else if (!listOnly && inputBackgroundApplied) {
            originalInputBackground.Restore(layoutGrid);
            inputBackgroundApplied = false;
        }
        for (auto const& saved : modeElements) {
            if (auto element = saved.element.get()) {
                if (listOnly) {
                    if (element.Opacity() != 0) element.Opacity(0);
                    if (element.IsHitTestVisible()) element.IsHitTestVisible(false);
                } else if (element.Opacity() == 0) {
                    saved.opacity.Restore(element);
                    saved.hitTest.Restore(element);
                }
            }
        }
        if (!applied || appliedMode != mode) {
            controls::RowDefinition desktopRow;
            controls::RowDefinition windowRow;
            desktopRow.Height({1, xaml::GridUnitType::Auto});
            windowRow.Height({1, xaml::GridUnitType::Star});
            layoutGrid.RowDefinitions().Clear();
            layoutGrid.RowDefinitions().Append(mode == 0 ? windowRow : desktopRow);
            layoutGrid.RowDefinitions().Append(mode == 0 ? desktopRow : windowRow);
            Wh_Log(L"Applied Task View desktop row 0 and window row 1");
        }
        int desktopRowIndex = mode == 0 ? 1 : 0;
        int windowRowIndex = mode == 0 ? 0 : 1;
        if (controls::Grid::GetRow(desktopElement) != desktopRowIndex) controls::Grid::SetRow(desktopElement, desktopRowIndex);
        int desktopSpan = mode == 3 || mode == 4 ? 2 : 1;
        if (controls::Grid::GetRowSpan(desktopElement) != desktopSpan) controls::Grid::SetRowSpan(desktopElement, desktopSpan);
        if (controls::Grid::GetRow(windowElement) != windowRowIndex) controls::Grid::SetRow(windowElement, windowRowIndex);
        if (controls::Grid::GetRowSpan(windowElement) != 1) controls::Grid::SetRowSpan(windowElement, 1);
        auto alignment = mode == 3 ? xaml::VerticalAlignment::Center : mode == 0 || mode == 4 ? xaml::VerticalAlignment::Bottom : xaml::VerticalAlignment::Top;
        if (desktopElement.VerticalAlignment() != alignment) {
            desktopElement.VerticalAlignment(alignment);
        }
        if (windowElement.VerticalAlignment() != xaml::VerticalAlignment::Stretch)
            windowElement.VerticalAlignment(xaml::VerticalAlignment::Stretch);
        if (!std::isnan(desktopElement.Height())) desktopElement.Height(NAN);
        if (!std::isnan(windowElement.Height())) windowElement.Height(NAN);
        xaml::Thickness zero{};
        double frameMargin = g_desktopFrameMargin.load();
        auto scale = layoutGrid.XamlRoot() ? layoutGrid.XamlRoot().RasterizationScale() : 1.0;
        if (clearanceDirty || clearanceWidth != layoutGrid.ActualWidth() ||
            clearanceHeight != layoutGrid.ActualHeight() || clearanceScale != scale) {
            cachedClearance = GetTaskbarClearance(layoutGrid, threadId);
            clearanceWidth = layoutGrid.ActualWidth(); clearanceHeight = layoutGrid.ActualHeight(); clearanceScale = scale;
            clearanceDirty = false;
        }
        auto clearance = cachedClearance;
        bool topAnchored = mode == 1 || mode == 2;
        bool bottomAnchored = mode == 0 || mode == 4;
        xaml::Thickness desktopMargin{frameMargin + clearance.Left,
            frameMargin + (bottomAnchored ? 0 : clearance.Top), frameMargin + clearance.Right,
            frameMargin + (topAnchored ? 0 : clearance.Bottom)};
        if (!SameMargin(desktopElement.Margin(), desktopMargin)) desktopElement.Margin(desktopMargin);
        if (!SameMargin(windowElement.Margin(), zero)) windowElement.Margin(zero);
        PinNewDesktopButton(desktopElement);
        if (buttonPinned) {
            auto button = newDesktopButton.get();
            if (button) {
                auto visibility = g_hideNewDesktop ? xaml::Visibility::Collapsed : xaml::Visibility::Visible;
                if (button.Visibility() != visibility) button.Visibility(visibility);
            }
            int widthMode = g_frameWidthMode.load();
            MatchNewDesktopFrame();
            auto horizontal = widthMode ? xaml::HorizontalAlignment::Center : xaml::HorizontalAlignment::Stretch;
            if (desktopElement.HorizontalAlignment() != horizontal)
                desktopElement.HorizontalAlignment(horizontal);
            double width = NAN;
            double available = (std::max)(0.0, layoutGrid.ActualWidth() - desktopMargin.Left - desktopMargin.Right);
            if (widthMode == 2) width = (std::min)(available, layoutGrid.ActualWidth() * g_framePercent.load() / 100.0);
            if (widthMode == 1) {
                auto stack = buttonPinned ? desktopStack.get() :
                    FindNamedChild<controls::StackPanel>(desktopElement, L"DesktopsListStackPanel");
                if (stack && stack.DesiredSize().Width > 0) {
                    double extra = 0;
                    if (buttonPinned && button && !g_hideNewDesktop) {
                        auto margin = button.Margin();
                        extra = button.Width() + margin.Left + margin.Right;
                    }
                    // Fit the tiles and pinned button once, plus frame padding.
                    width = (std::min)(available, stack.DesiredSize().Width + extra + 16);
                }
            }
            if ((std::isnan(width) && !std::isnan(desktopElement.Width())) ||
                (!std::isnan(width) && desktopElement.Width() != width)) desktopElement.Width(width);
            PositionNewDesktopButton();

        }
        applied = true;
        appliedMode = mode;
    }

    void Detach() noexcept {
        if (detached) return;
        detached = true;
        auto attempt = [](auto&& action) { try { action(); } catch (...) { Wh_Log(L"Task View event detach failed: %08X", winrt::to_hresult()); } };
        attempt([&] { if (auto element = root.get()) element.LayoutUpdated(layoutToken); });
        attempt([&] { if (auto element = root.get()) element.KeyDown(keyToken); });
        attempt([&] { if (auto element = root.get()) element.KeyUp(keyUpToken); });
        attempt([&] { if (auto element = root.get()) element.PointerPressed(outsideClickToken); });
        attempt([&] { if (auto element = root.get()) element.Unloaded(unloadedToken); });
        attempt([&] { if (auto element = root.get()) element.SizeChanged(sizeToken); });
        attempt([&] { if (auto element = root.get()) element.ActualThemeChanged(themeToken); });
        attempt([&] { if (auto list = desktopList.get()) list.SelectionChanged(selectionToken); });
        attempt([&] { if (auto list = desktopList.get()) list.ContainerContentChanging(containersToken); });
        for (auto const& highlight : highlights) highlight->Detach();
        for (auto const& motion : scrollMotions) attempt([&] { motion->Stop(); });
    }

    void Restore() {
        Detach();
        LayoutGuard guard;
        auto attempt = [](auto&& action) { try { action(); } catch (...) { Wh_Log(L"Task View property cleanup failed: %08X", winrt::to_hresult()); } };
        for (auto const& saved : hoverTimers)
            attempt([&] { if (auto timer = saved.timer.get()) timer.Interval(saved.interval); });
        hoverTimers.clear();
        attempt([&] { RestoreNewDesktopTemplate(); });
        for (auto const& saved : newDesktopStretchProperties)
            attempt([&] { if (auto element = saved.element.get()) saved.property.Restore(element); });
        newDesktopStretchProperties.clear();
        for (auto const& saved : newDesktopAppearanceProperties)
            attempt([&] { if (auto element = saved.element.get()) saved.property.Restore(element); });
        newDesktopAppearanceProperties.clear();
        attempt([&] { if (nativeButtonResized) if (auto button = nativeButton.get()) {
            nativeButtonWidth.Restore(button); nativeButtonMinWidth.Restore(button);
        } });
        ClearAnimationTargets();
        attempt([&] { if (nativeButtonHidden) if (auto button = nativeButton.get()) nativeButtonVisibility.Restore(button); });
        for (auto const& highlight : highlights) attempt([&] { highlight->Restore(); });
        for (auto& material : materials) attempt([&] { material.Restore(); });
        for (auto& border : borders) attempt([&] { border.Restore(); });
        auto restoreBackground = [&](ListBackground& saved) {
            attempt([&] { if (saved.hidden) if (auto background = saved.element.get()) {
                saved.background.Restore(background); saved.opacity.Restore(background);
            } });
            saved.hidden = false;
        };
        restoreBackground(fullscreenBackground);
        for (auto& saved : listBackgrounds) restoreBackground(saved);
        for (auto const& saved : modeElements) attempt([&] { if (auto element = saved.element.get()) {
            saved.opacity.Restore(element); saved.hitTest.Restore(element);
        } });
        attempt([&] { UnpinNewDesktopButton(); });
        for (auto const& frames : animationFrames)
            for (auto const& saved : frames) attempt([&] { saved.frame.Value(saved.original); });
        for (auto const& property : desktopProperties)
            attempt([&] { if (auto element = desktops.get()) property.Restore(element); });
        for (auto const& property : windowProperties)
            attempt([&] { if (auto element = windows.get()) property.Restore(element); });
        attempt([&] { if (applied) if (auto layoutGrid = grid.get()) {
            if (inputBackgroundApplied) originalInputBackground.Restore(layoutGrid);
            layoutGrid.RowDefinitions().Clear();
            for (auto const& row : originalRows) layoutGrid.RowDefinitions().Append(row);
        } });
    }

};

std::mutex g_statesMutex;
[[clang::no_destroy]] std::optional<std::vector<std::shared_ptr<LayoutState>>> g_states{std::in_place};
void RetireState(std::shared_ptr<LayoutState> state) {
    if (!state || state->threadId != GetCurrentThreadId()) return;
    try { state->Restore(); } catch (...) { Wh_Log(L"Unloaded Task View restoration failed: %08X", winrt::to_hresult()); }
    std::lock_guard lock(g_statesMutex);
    std::erase(*g_states, state);
    Wh_Log(L"Task View state retired on thread %u; states remaining: %zu", GetCurrentThreadId(), g_states->size());
}
thread_local LayoutState* g_hoverTimerContext = nullptr;
using HoverTimerStart_t = HRESULT(WINAPI*)(void*);
HoverTimerStart_t HoverTimerStart_Original = nullptr;
HRESULT WINAPI HoverTimerStart_Hook(void* self) {
    if (!g_stopping && g_hoverTimerContext) {
        try {
            xaml::IDispatcherTimer timerInterface{nullptr};
            winrt::copy_from_abi(timerInterface, self);
            auto timer = timerInterface.as<xaml::DispatcherTimer>();
            auto& timers = g_hoverTimerContext->hoverTimers;
            auto saved = std::find_if(timers.begin(), timers.end(), [&](auto const& entry) {
                return entry.timer.get() == timer;
            });
            if (saved == timers.end()) {
                auto interval = timer.Interval();
                timers.push_back({winrt::make_weak(timer), interval});
                saved = std::prev(timers.end());
            }
            int delay = g_hoverSwitchDelayMs;
            auto interval = delay >= 0 ? winrt::Windows::Foundation::TimeSpan{static_cast<int64_t>(delay) * 10000} : saved->interval;
            if (timer.Interval() != interval) timer.Interval(interval);
        } catch (...) { Wh_Log(L"Desktop hover delay update failed: %08X", winrt::to_hresult()); }
    }
    return HoverTimerStart_Original(self);
}

using PointerAdd_t = HRESULT(WINAPI*)(void*, void*, winrt::event_token*);
using PointerRemove_t = HRESULT(WINAPI*)(void*, winrt::event_token);
using NativePointerInvoke_t = HRESULT(WINAPI*)(void*, void*, void*);
PointerAdd_t PointerEnterAdd_Original, PointerMoveAdd_Original, LoadedAdd_Original;
PointerRemove_t PointerEnterRemove_Original, PointerMoveRemove_Original, LoadedRemove_Original;
struct NativeHoverHook { void* target; NativePointerInvoke_t original = nullptr; };
struct NativeHoverSubscription {
    void* element; void* handler; winrt::event_token token; bool moved;
    winrt::weak_ref<xaml::UIElement> owner;
    DWORD thread;
    bool loaded = false;
};
std::mutex g_hoverHandlersMutex;
std::vector<std::shared_ptr<NativeHoverHook>> g_nativeHoverHooks;
std::vector<NativeHoverSubscription> g_hoverSubscriptions;
void ObserveElement(xaml::FrameworkElement const& element, bool walkAncestors);
bool IsSwitcherCode(void* address) {
    HMODULE module = nullptr;
    return address && GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCWSTR>(address), &module) &&
        (module == GetModuleHandleW(L"WindowsInternal.ComposableShell.Experiences.Switcher.dll") ||
         module == GetModuleHandleW(L"twinui.pcshell.dll"));
}

bool IsDesktopMouseHover(xaml::Input::PointerRoutedEventArgs const& args, xaml::FrameworkElement* root = nullptr) {
    if (g_stopping) return false;
    try {
        if (args.Pointer().PointerDeviceType() != winrt::Windows::Devices::Input::PointerDeviceType::Mouse) return false;
        if (args.GetCurrentPoint(nullptr).Properties().PointerUpdateKind() !=
            winrt::Windows::UI::Input::PointerUpdateKind::Other) return false;
        if (((GetAsyncKeyState(VK_LBUTTON) | GetAsyncKeyState(VK_RBUTTON) | GetAsyncKeyState(VK_MBUTTON) |
            GetAsyncKeyState(VK_XBUTTON1) | GetAsyncKeyState(VK_XBUTTON2)) & 0x8000) != 0) return false;
        auto node = args.OriginalSource().try_as<xaml::DependencyObject>();
        bool desktop = false;
        for (int depth = 0; node && depth < 32; depth++) {
            if (node.try_as<controls::TextBox>()) return false;
            auto type = winrt::get_class_name(node);
            if (type == L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopElementThemed") desktop = true;
            if (!desktop && node.try_as<controls::ListViewItem>()) desktop = static_cast<bool>(FindDesktopTile(node));
            if (type == L"WindowsInternal.ComposableShell.Experiences.Switcher.TaskViewTimeline") {
                if (desktop && root) *root = node.try_as<xaml::FrameworkElement>();
                return desktop;
            }
            node = media::VisualTreeHelper::GetParent(node);
        }
    } catch (...) { Wh_Log(L"Desktop hover target inspection failed: %08X", winrt::to_hresult()); }
    return false;
}


HRESULT WINAPI NativeHoverInvoke_Hook(void* self, void* sender, void* arguments) {
    NativePointerInvoke_t original = nullptr;
    bool hoverSubscriber = false;
    xaml::FrameworkElement loadedElement{nullptr};
    auto target = (*reinterpret_cast<void***>(self))[3];
    {
        std::lock_guard lock(g_hoverHandlersMutex);
        for (auto const& hook : g_nativeHoverHooks) if (hook->target == target) { original = hook->original; break; }
        if (!g_stopping) for (auto const& saved : g_hoverSubscriptions)
            if (saved.handler == self && saved.thread == GetCurrentThreadId()) {
                try { if (auto owner = saved.owner.get()) {
                    if (saved.loaded) loadedElement = owner.as<xaml::FrameworkElement>();
                    else hoverSubscriber = true;
                    break;
                } }
                catch (...) {} // A disappearing UI owner must retain native behavior.
            }
    }
    if (!original) return E_UNEXPECTED;
    if (loadedElement && !g_stopping) {
        try { ObserveElement(loadedElement, true); }
        catch (...) { Wh_Log(L"Task View Loaded observation failed: %08X", winrt::to_hresult()); }
        return original(self, sender, arguments);
    }
    std::shared_ptr<LayoutState> hoverState;
    if (hoverSubscriber && arguments && !g_stopping) {
        try {
            xaml::Input::PointerRoutedEventArgs args{nullptr}; winrt::copy_from_abi(args, arguments);
            xaml::FrameworkElement root{nullptr};
            if (IsDesktopMouseHover(args, &root)) {
                if (g_preventHoverSwitch) {
                    return S_OK;
                }
                std::lock_guard lock(g_statesMutex);
                for (auto const& state : *g_states)
                    if (state->threadId == GetCurrentThreadId() && !state->detached && state->root.get() == root) {
                        hoverState = state;
                        break;
                    }
            }
        } catch (...) { Wh_Log(L"Desktop hover callback inspection failed: %08X", winrt::to_hresult()); }
    }
    struct ContextGuard {
        LayoutState* previous = g_hoverTimerContext;
        ~ContextGuard() { g_hoverTimerContext = previous; }
    } guard;
    g_hoverTimerContext = hoverState.get();
    return original(self, sender, arguments);
}

void TrackDesktopPointerHandler(void* self, void* handler, winrt::event_token token, bool moved, void* caller, bool loaded = false) {
    if (g_stopping || !handler) return;
    auto invoke = (*reinterpret_cast<void***>(handler))[3];
    if (!IsSwitcherCode(caller) && !IsSwitcherCode(invoke)) return;
    xaml::UIElement owner{nullptr};
    if (loaded) {
        xaml::IFrameworkElement element{nullptr}; winrt::copy_from_abi(element, self);
        owner = element.as<xaml::UIElement>();
    } else {
        xaml::IUIElement element{nullptr}; winrt::copy_from_abi(element, self);
        owner = element.as<xaml::UIElement>();
    }
    NativeHoverSubscription subscription{self, handler, token, moved,
        winrt::make_weak(owner), GetCurrentThreadId(), loaded};
    // Leave the native delegate object and event token unchanged. Hook only
    // its Invoke entry, forwarding every call except a tracked desktop hover.
    std::shared_ptr<NativeHoverHook> hook;
    {
        std::lock_guard lock(g_hoverHandlersMutex);
        std::erase_if(g_hoverSubscriptions, [](auto const& saved) {
            if (saved.thread != GetCurrentThreadId()) return false;
            try { return !saved.owner.get(); } catch (...) { return true; }
        });
        for (auto const& candidate : g_nativeHoverHooks) if (candidate->target == invoke) { hook = candidate; break; }
        if (hook) { g_hoverSubscriptions.push_back(subscription); return; }
    }
    hook = std::make_shared<NativeHoverHook>(); hook->target = invoke;
    {
        std::lock_guard lock(g_hoverHandlersMutex);
        // Allocate bookkeeping before registering any patch so an allocation
        // failure cannot leave an untracked, pending callback hook behind.
        g_nativeHoverHooks.reserve(g_nativeHoverHooks.size() + 1);
        g_hoverSubscriptions.reserve(g_hoverSubscriptions.size() + 1);
        g_nativeHoverHooks.push_back(hook);
        g_hoverSubscriptions.push_back(subscription);
    }
    if (!WindhawkUtils::SetFunctionHook(reinterpret_cast<NativePointerInvoke_t>(invoke),
            NativeHoverInvoke_Hook, &hook->original)) {
        std::lock_guard lock(g_hoverHandlersMutex);
        std::erase(g_nativeHoverHooks, hook);
        std::erase_if(g_hoverSubscriptions, [&](auto const& saved) {
            return saved.element == self && saved.token == token && saved.moved == moved && !saved.loaded;
        });
        Wh_Log(L"Could not hook native desktop hover callback"); return;
    }
    Wh_ApplyHookOperations();
}
HRESULT AddDesktopPointerHandler(void* self, void* handler, winrt::event_token* token, bool moved, void* caller) {
    auto original = moved ? PointerMoveAdd_Original : PointerEnterAdd_Original;
    auto result = original(self, handler, token);
    if (SUCCEEDED(result) && token) {
        try { TrackDesktopPointerHandler(self, handler, *token, moved, caller); }
        catch (...) { Wh_Log(L"Native hover callback tracking failed: %08X", winrt::to_hresult()); }
    }
    return result;
}
HRESULT RemoveDesktopPointerHandler(void* self, winrt::event_token token, bool moved) {
    auto result = (moved ? PointerMoveRemove_Original : PointerEnterRemove_Original)(self, token);
    if (SUCCEEDED(result) && !g_stopping) {
        std::lock_guard lock(g_hoverHandlersMutex);
        std::erase_if(g_hoverSubscriptions, [&](auto const& saved) {
            return saved.element == self && saved.token == token && saved.moved == moved && !saved.loaded;
        });
    }
    return result;
}
HRESULT WINAPI PointerEnterAdd_Hook(void* self, void* handler, winrt::event_token* token) {
    return AddDesktopPointerHandler(self, handler, token, false, __builtin_return_address(0));
}
HRESULT WINAPI PointerMoveAdd_Hook(void* self, void* handler, winrt::event_token* token) {
    return AddDesktopPointerHandler(self, handler, token, true, __builtin_return_address(0));
}
HRESULT WINAPI PointerEnterRemove_Hook(void* self, winrt::event_token token) { return RemoveDesktopPointerHandler(self, token, false); }
HRESULT WINAPI PointerMoveRemove_Hook(void* self, winrt::event_token token) { return RemoveDesktopPointerHandler(self, token, true); }

void ObserveElementCore(xaml::FrameworkElement const& element, bool walkAncestors) {
    if (!element || g_stopping) return;
    LayoutGuard guard;
    auto name = element.Name();
    if (!walkAncestors && name != L"VirtualDesktopBar" && name != L"SwitchItemListControl" &&
        name != L"RootGridElement" && name != L"TaskViewTimelineRoot") return;

    xaml::FrameworkElement rootElement = element;
    // Require the full Task View root. The taskbar hover preview has a desktop
    // strip too and must not be changed.
    for (int depth = 0; depth < 16; depth++) {
        if (winrt::get_class_name(rootElement) ==
            L"WindowsInternal.ComposableShell.Experiences.Switcher.TaskViewTimeline") {
            break;
        }
        rootElement = media::VisualTreeHelper::GetParent(rootElement)
                          .try_as<xaml::FrameworkElement>();
        if (!rootElement) return;
    }
    if (winrt::get_class_name(rootElement) !=
        L"WindowsInternal.ComposableShell.Experiences.Switcher.TaskViewTimeline") return;

    {
        std::lock_guard lock(g_statesMutex);
        for (auto const& state : *g_states) {
            if (state->threadId == GetCurrentThreadId() &&
                state->root.get() == rootElement) return;
        }
    }

    // Locate only the immediate overview controls, never a nested Alt+Tab list.
    controls::Grid grid{nullptr};
    auto childCount = media::VisualTreeHelper::GetChildrenCount(rootElement);
    for (int i = 0; i < childCount; i++) {
        auto child = media::VisualTreeHelper::GetChild(rootElement, i).try_as<controls::Grid>();
        if (child && child.Name() == L"RootGridElement") { grid = child; break; }
    }
    if (!grid) return;
    xaml::FrameworkElement desktops{nullptr}, windows{nullptr};
    for (auto const& child : grid.Children()) {
        if (auto frameworkChild = child.try_as<xaml::FrameworkElement>()) {
            if (frameworkChild.Name() == L"VirtualDesktopBar") desktops = frameworkChild;
            if (frameworkChild.Name() == L"SwitchItemListControl") windows = frameworkChild;
        }
    }
    if (!desktops || !windows) return;

    auto state = std::make_shared<LayoutState>();
    struct RegistrationGuard {
        std::shared_ptr<LayoutState> state;
        bool published = false;
        ~RegistrationGuard() { if (!published) state->Detach(); }
    } registration{state};
    state->threadId = GetCurrentThreadId();
    state->root = winrt::make_weak(rootElement);
    state->uiSettings = winrt::Windows::UI::ViewManagement::UISettings();
    state->unloadedToken = rootElement.Unloaded([weak = std::weak_ptr(state)](auto const&, auto const&) {
        if (auto current = weak.lock()) RetireState(std::move(current));
    });
    state->sizeToken = rootElement.SizeChanged([weak = std::weak_ptr(state)](auto const&, auto const&) {
        if (auto current = weak.lock()) current->clearanceDirty = true;
    });
    state->themeToken = rootElement.ActualThemeChanged([weak = std::weak_ptr(state)](auto const&, auto const&) {
        if (auto current = weak.lock()) { current->appearanceDirty = true; current->Apply(); }
    });
    state->desktops = winrt::make_weak(desktops);
    state->windows = winrt::make_weak(windows);
    state->grid = winrt::make_weak(grid);
    state->originalInputBackground = SavedProperty(grid, controls::Panel::BackgroundProperty());
    state->keyUpToken = rootElement.KeyUp([weak = std::weak_ptr(state)](auto const&, xaml::Input::KeyRoutedEventArgs const& args) {
        if (g_stopping || !g_closeOnWindowsRelease) return;
        if (args.Key() != winrt::Windows::System::VirtualKey::LeftWindows &&
            args.Key() != winrt::Windows::System::VirtualKey::RightWindows) return;
        try { if (auto current = weak.lock(); current && !current->detached) current->DismissList(); }
        catch (...) { Wh_Log(L"Windows-key release dismissal failed: %08X", winrt::to_hresult()); }
    });
    state->outsideClickToken = rootElement.PointerPressed(
        [weak = std::weak_ptr(state)](auto const&, xaml::Input::PointerRoutedEventArgs const& args) {
            if (g_stopping || !IsListOnlyMode()) return;
            try {
                auto current = weak.lock();
                if (!current || current->detached) return;
                auto desktops = current->desktops.get();
                if (!desktops) return;
                auto point = args.GetCurrentPoint(desktops).Position();
                if (point.X >= 0 && point.X < desktops.ActualWidth() &&
                    point.Y >= 0 && point.Y < desktops.ActualHeight()) return;
                if (!current->DismissList()) return;
                args.Handled(true);
            } catch (...) { Wh_Log(L"Outside-click dismissal failed: %08X", winrt::to_hresult()); }
        });
    state->keyToken = rootElement.KeyDown([weak = std::weak_ptr(state)](auto const&, xaml::Input::KeyRoutedEventArgs const& args) {
        if (g_stopping || !IsListOnlyMode() || args.Key() != winrt::Windows::System::VirtualKey::Tab) return;
        try {
            auto current = weak.lock();
            if (!current) return;
            auto stack = current->desktopStack.get();
            if (!stack) return;
            current->Navigate((GetAsyncKeyState(VK_SHIFT) & 0x8000) ? -1 : 1);
            args.Handled(true);
        } catch (...) {}
    });
    for (auto const* name : {L"SwitchItemListControl", L"BackgroundThumbnailHost"}) {
        if (auto element = FindNamedChild<xaml::FrameworkElement>(grid, name)) {
            state->modeElements.push_back({winrt::make_weak(element),
                SavedProperty(element, xaml::UIElement::OpacityProperty()),
                SavedProperty(element, xaml::UIElement::IsHitTestVisibleProperty())});
        }
    }
    for (auto const& row : grid.RowDefinitions()) state->originalRows.push_back(row);
    for (auto const& property : {controls::Grid::RowProperty(), controls::Grid::RowSpanProperty(),
                                xaml::FrameworkElement::VerticalAlignmentProperty(),
                                xaml::FrameworkElement::HorizontalAlignmentProperty(),
                                xaml::FrameworkElement::WidthProperty(),
                                xaml::FrameworkElement::HeightProperty(),
                                xaml::FrameworkElement::MarginProperty()}) {
        state->desktopProperties.emplace_back(desktops, property);
        state->windowProperties.emplace_back(windows, property);
    }
    if (auto list = FindNamedChild<controls::ListView>(desktops, L"DesktopsList")) {
        state->desktopList = winrt::make_weak(list);
        state->selectionToken = list.SelectionChanged([weak = std::weak_ptr(state)](auto const&, auto const&) {
            if (g_stopping || g_insideLayout) return;
            try { if (auto current = weak.lock(); current && !current->detached) { LayoutGuard guard; current->ApplyHighlights(); } }
            catch (...) { Wh_Log(L"Selection highlight update failed: %08X", winrt::to_hresult()); }
        });
        state->containersToken = list.ContainerContentChanging([weak = std::weak_ptr(state)](auto const&, auto const&) {
            if (auto current = weak.lock()) current->appearanceDirty = true;
        });
    }
    // The event owns only a weak state; the registry owns the state itself.
    state->layoutToken = rootElement.LayoutUpdated(
        [weak = std::weak_ptr(state)](auto const&, auto const&) {
            try {
                if (auto current = weak.lock(); current && !current->detached) current->Apply();
            } catch (...) {
                Wh_Log(L"Task View layout update failed: %08X", winrt::to_hresult());
            }
        });
    {
        std::lock_guard lock(g_statesMutex);
        g_states->push_back(state);
        registration.published = true;
    }
    Wh_Log(L"Found full Task View root on thread %u", state->threadId);
    // Apply on the next layout pass; the reentrancy guard is active here.
    rootElement.InvalidateArrange();
}

void ObserveElement(xaml::FrameworkElement const& element, bool walkAncestors) {
    if (g_insideLayout || g_stopping) return;
    // Retire expired owners on this UI thread even if Windows omitted Unloaded.
    std::vector<std::shared_ptr<LayoutState>> stale;
    {
        std::lock_guard lock(g_statesMutex);
        for (auto const& state : *g_states)
            if (state->threadId == GetCurrentThreadId() && !state->root.get()) stale.push_back(state);
    }
    for (auto& state : stale) RetireState(std::move(state));
    ObserveElementCore(element, walkAncestors);
}

struct AnimationTarget {
    winrt::weak_ref<animation::Timeline> timeline;
    winrt::weak_ref<xaml::FrameworkElement> target;
};
thread_local std::vector<AnimationTarget> g_animationTargets;
void ClearAnimationTargets() { g_animationTargets.clear(); }
bool IsTaskViewAnimation(animation::Timeline const& timeline) {
    for (auto const& tracked : g_animationTargets)
        if (tracked.timeline.get() == timeline) return true;
    if (auto storyboard = timeline.try_as<animation::Storyboard>())
        for (auto const& child : storyboard.Children())
            if (IsTaskViewAnimation(child)) return true;
    return false;
}
using GetPeer_t = HRESULT(WINAPI*)(void*, GUID const&, void**);
GetPeer_t GetPeer;
using CoreSetTarget_t = HRESULT(WINAPI*)(void*, void*);
CoreSetTarget_t CoreSetTarget_Original;
HRESULT WINAPI CoreSetTarget_Hook(void* self, void* target) {
    HRESULT result = CoreSetTarget_Original(self, target);
    if (SUCCEEDED(result) && !g_stopping && target) {
        try {
            animation::Timeline timeline{nullptr};
            xaml::FrameworkElement element{nullptr};
            HRESULT timelineResult = GetPeer(self, winrt::guid_of<animation::ITimeline>(), winrt::put_abi(timeline));
            HRESULT targetResult = GetPeer(target, winrt::guid_of<xaml::IFrameworkElement>(), winrt::put_abi(element));
            if (SUCCEEDED(timelineResult) && SUCCEEDED(targetResult) && timeline && element) {
                auto ancestor = element;
                bool fullTaskView = false;
                for (int depth = 0; ancestor && depth < 16; depth++) {
                    if (winrt::get_class_name(ancestor) ==
                        L"WindowsInternal.ComposableShell.Experiences.Switcher.TaskViewTimeline") {
                        fullTaskView = true;
                        break;
                    }
                    ancestor = media::VisualTreeHelper::GetParent(ancestor).try_as<xaml::FrameworkElement>();
                }
                if (!fullTaskView) return result;
                ObserveElement(element, true);
                for (auto it = g_animationTargets.begin(); it != g_animationTargets.end();) {
                    auto existing = it->timeline.get();
                    if (!existing || existing == timeline) it = g_animationTargets.erase(it);
                    else ++it;
                }
                g_animationTargets.push_back({winrt::make_weak(timeline), winrt::make_weak(element)});
            }
        } catch (...) {}
    }
    return result;
}
void ApplyDesktopSlideDirection(animation::Timeline const& timeline) {
    if (auto storyboard = timeline.try_as<animation::Storyboard>()) {
        for (auto const& child : storyboard.Children()) ApplyDesktopSlideDirection(child);
        return;
    }
    auto property = animation::Storyboard::GetTargetProperty(timeline);
    for (auto const& tracked : g_animationTargets) {
        if (tracked.timeline.get() != timeline) continue;
        if (auto target = tracked.target.get()) {
        }
    }
    if (property != L"(UIElement.RenderTransform).(TranslateTransform.Y)" &&
        property != L"(UIElement.RenderTransform).(CompositeTransform.TranslateY)" &&
        property != L"(UIElement.Translation).Y" && property != L"Y" && property != L"TranslateY") return;
    auto keyframes = timeline.try_as<animation::DoubleAnimationUsingKeyFrames>();
    if (!keyframes) return;
    for (auto const& tracked : g_animationTargets) {
        if (tracked.timeline.get() != timeline) continue;
        auto target = tracked.target.get();
        if (!target) return;
        std::lock_guard lock(g_statesMutex);
        for (auto const& state : *g_states) {
            if (state->threadId != GetCurrentThreadId() || state->desktops.get() != target) continue;
            auto frames = keyframes.KeyFrames();
            if (frames.Size() == 0) return;
            auto& savedFrames = state->animationFrames[frames.GetAt(0).Value() == 0 ? 1 : 0];
            for (auto const& frame : frames) {
                auto saved = std::find_if(savedFrames.begin(), savedFrames.end(), [&](auto const& item) {
                    return item.frame == frame;
                });
                if (saved == savedFrames.end()) savedFrames.push_back({frame, frame.Value()});
                frame.Value(g_desktopSlideFromTop ? -std::abs(frame.Value()) : std::abs(frame.Value()));
            }
            return;
        }
    }
}

// Verified MSVC x64 ABI: the xref_ptr result is constructed in the caller's
// second argument and returned unchanged. Its ownership stays with the caller.
using TimelineGetTarget_t = void*(WINAPI*)(void*, void**);
TimelineGetTarget_t TimelineGetTarget_Original;
// Symbol-resolved helpers, not global hooks. Their calling conventions and
// matching ownership operations were checked against the installed DLL/PDB.
using QueryNativePeer_t = HRESULT(WINAPI*)(void**, void*);
using ReleaseNativePeer_t = void(WINAPI*)(void**);
using NativePeerHandle_t = void*(WINAPI*)(void*);
using CoreRelease_t = void(WINAPI*)(void*);
QueryNativePeer_t QueryNativePeer;
ReleaseNativePeer_t ReleaseNativePeer;
NativePeerHandle_t NativePeerHandle;
CoreRelease_t CoreRelease;

void ResolveDesktopSlideTargets(animation::Timeline const& timeline) {
    if (auto storyboard = timeline.try_as<animation::Storyboard>()) {
        for (auto const& child : storyboard.Children()) ResolveDesktopSlideTargets(child);
        return;
    }
    if (!timeline.try_as<animation::DoubleAnimationUsingKeyFrames>()) return;
    struct NativePeer {
        void* value = nullptr;
        ~NativePeer() { if (value) ReleaseNativePeer(&value); }
    } peer;
    if (FAILED(QueryNativePeer(&peer.value, winrt::get_abi(timeline))) || !peer.value) return;
    auto core = NativePeerHandle(peer.value);
    if (!core) return;
    struct CoreTarget {
        void* value = nullptr;
        ~CoreTarget() { if (value) CoreRelease(value); }
    } coreTarget;
    TimelineGetTarget_Original(core, &coreTarget.value);
    if (coreTarget.value) {
            xaml::DependencyObject target{nullptr};
            if (FAILED(GetPeer(coreTarget.value, winrt::guid_of<xaml::IDependencyObject>(), winrt::put_abi(target)))) return;
            // Discovery from a Switcher animation does not require any other
            // diagnostics consumer. Identity checks below still gate mutation.
            ObserveElementCore(target.try_as<xaml::FrameworkElement>(), true);
            xaml::FrameworkElement bar{nullptr};
            {
                std::lock_guard lock(g_statesMutex);
                for (auto const& state : *g_states) {
                    if (state->threadId != GetCurrentThreadId() || state->detached) continue;
                    auto candidate = state->desktops.get();
                    if (candidate && (target == candidate || target == candidate.RenderTransform())) {
                        bar = candidate;
                        break;
                    }
                }
            }
            if (!bar) return;
            std::erase_if(g_animationTargets, [&](auto const& entry) {
                auto existing = entry.timeline.get();
                return !existing || existing == timeline;
            });
            g_animationTargets.push_back({winrt::make_weak(timeline), winrt::make_weak(bar)});
            ApplyDesktopSlideDirection(timeline);
    }
}

using CoreStoryboardBegin_t = HRESULT(WINAPI*)(void*, unsigned int);
CoreStoryboardBegin_t CoreStoryboardBegin_Original;
HRESULT WINAPI CoreStoryboardBegin_Hook(void* self, unsigned int flags) {
    bool taskViewThread = false;
    if (!g_stopping && !g_insideLayout) {
        std::lock_guard lock(g_statesMutex);
        for (auto const& state : *g_states)
            if (state->threadId == GetCurrentThreadId() && !state->detached) {
                taskViewThread = true;
                break;
            }
    }
    if (!g_stopping && !g_insideLayout && !taskViewThread) {
        auto foreground = GetForegroundWindow();
        DWORD process = 0;
        auto foregroundThread = GetWindowThreadProcessId(foreground, &process);
        if (process == GetCurrentProcessId() && foregroundThread == GetCurrentThreadId()) {
            wchar_t name[80]{};
            GetClassNameW(foreground, name, ARRAYSIZE(name));
            taskViewThread = wcscmp(name, L"MultitaskingViewFrame") == 0 ||
                             wcscmp(name, L"XamlExplorerHostIslandWindow") == 0;
        }
        taskViewThread = taskViewThread || IsSwitcherCode(__builtin_return_address(0));
    }
    if (taskViewThread) {
        LayoutGuard guard;
        try {
            animation::Timeline timeline{nullptr};
            if (SUCCEEDED(GetPeer(self, winrt::guid_of<animation::ITimeline>(), winrt::put_abi(timeline))) && timeline) {
                ResolveDesktopSlideTargets(timeline);
            }
        } catch (...) { Wh_Log(L"Native storyboard inspection failed: %08X", winrt::to_hresult()); }
    }
    return CoreStoryboardBegin_Original(self, flags);
}

using StoryboardBegin_t = HRESULT(WINAPI*)(void*);
StoryboardBegin_t StoryboardBegin_Original;
HRESULT WINAPI StoryboardBegin_Hook(void* self) {
    bool skip = false;
    if (!g_stopping && !g_insideLayout) {
        try {
            animation::IStoryboard storyboardInterface{nullptr};
            winrt::copy_from_abi(storyboardInterface, self);
            auto storyboard = storyboardInterface.as<animation::Storyboard>();
            skip = g_disableListAnimations && IsTaskViewAnimation(storyboard);
            { LayoutGuard guard; ApplyDesktopSlideDirection(storyboard); }
            // Windows reuses storyboards and can start unrelated storyboards
            // between SetTarget and Begin. Keep weak target associations until
            // the timeline expires instead of discarding them after any Begin.
            std::erase_if(g_animationTargets, [](auto const& entry) {
                return !entry.timeline.get() || !entry.target.get();
            });
        } catch (...) {}
    }
    auto result = StoryboardBegin_Original(self);
    if (SUCCEEDED(result) && skip) {
        try {
            animation::IStoryboard storyboard{nullptr};
            winrt::copy_from_abi(storyboard, self);
            storyboard.as<animation::Storyboard>().SkipToFill();
        } catch (...) {}
    }
    return result;
}

bool IsDesktopScroll(controls::ScrollViewer const& scroll) {
    if (!scroll || g_stopping || g_insideLayout) return false;
    xaml::FrameworkElement ancestor = scroll;
    bool desktopBar = false;
    for (int depth = 0; ancestor && depth < 24; depth++) {
        auto className = winrt::get_class_name(ancestor);
        if (className == L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement")
            desktopBar = true;
        if (className == L"WindowsInternal.ComposableShell.Experiences.Switcher.TaskViewTimeline")
            return desktopBar;
        ancestor = media::VisualTreeHelper::GetParent(ancestor).try_as<xaml::FrameworkElement>();
    }
    return false;
}

using ChangeView_t = HRESULT(WINAPI*)(void*, void*, void*, void*, bool, bool*);
ChangeView_t ChangeView_Original;
bool AnimateDesktopScroll(controls::ScrollViewer const& scroll, double offset) {
    std::shared_ptr<ScrollMotion> motion;
    {
        std::lock_guard lock(g_statesMutex);
        for (auto const& state : *g_states) {
            if (state->threadId != GetCurrentThreadId() || !state->root.get()) continue;
            for (auto const& candidate : state->scrollMotions) {
                if (candidate->scroll.get() == scroll) { motion = candidate; break; }
            }
            if (!motion) {
                motion = std::make_shared<ScrollMotion>();
                motion->scroll = winrt::make_weak(scroll);
                state->scrollMotions.push_back(motion);
            }
            break;
        }
    }
    if (motion) return motion->Request(offset, g_scrollSpeed.load());
    ScrollCallGuard guard;
    return scroll.ChangeView(wf::IReference<double>{offset}, nullptr, nullptr, false);
}
HRESULT WINAPI ChangeView_Hook(void* self, void* horizontal, void* vertical,
                               void* zoom, bool disableAnimation, bool* changed) {
    if (horizontal && !g_insideScroll && !g_stopping && !g_insideLayout) {
        try {
            controls::IScrollViewer2 scrollInterface{nullptr};
            winrt::copy_from_abi(scrollInterface, self);
            auto scroll = scrollInterface.as<controls::ScrollViewer>();
            if (IsDesktopScroll(scroll)) {
                // Preserve requests that also change another axis or zoom.
                // Windows sometimes passes the unchanged values explicitly.
                if (vertical) {
                    wf::IReference<double> value{nullptr};
                    winrt::copy_from_abi(value, vertical);
                    if (std::abs(value.Value() - scroll.VerticalOffset()) > 0.5)
                        return ChangeView_Original(self, horizontal, vertical, zoom, false, changed);
                }
                if (zoom) {
                    wf::IReference<float> value{nullptr};
                    winrt::copy_from_abi(value, zoom);
                    if (std::abs(value.Value() - scroll.ZoomFactor()) > 0.001f)
                        return ChangeView_Original(self, horizontal, vertical, zoom, false, changed);
                }
                wf::IReference<double> offset{nullptr};
                winrt::copy_from_abi(offset, horizontal);
                bool result = AnimateDesktopScroll(scroll, offset.Value());
                if (changed) *changed = result;
                return S_OK;
            }
        } catch (...) {}
    }
    return ChangeView_Original(self, horizontal, vertical, zoom, disableAnimation, changed);
}

using ScrollHorizontal_t = HRESULT(WINAPI*)(void*, double);
ScrollHorizontal_t ScrollHorizontal_Original;
HRESULT WINAPI ScrollHorizontal_Hook(void* self, double offset) {
    if (!g_stopping && !g_insideLayout && !g_insideScroll) {
        try {
            controls::IScrollViewer scrollInterface{nullptr};
            winrt::copy_from_abi(scrollInterface, self);
            auto scroll = scrollInterface.as<controls::ScrollViewer>();
            if (IsDesktopScroll(scroll)) {
                AnimateDesktopScroll(scroll, offset);
                return S_OK;
            }
        } catch (...) {}
    }
    return ScrollHorizontal_Original(self, offset);
}

// IFrameworkElement Loaded slots are SDK-defined (43/44). Capture only
// Switcher-owned native delegates. We add no event delegate of our own here.
HRESULT WINAPI LoadedAdd_Hook(void* self, void* handler, winrt::event_token* token) {
    auto caller = __builtin_return_address(0);
    auto result = LoadedAdd_Original(self, handler, token);
    if (SUCCEEDED(result) && token && !g_stopping) {
        try { TrackDesktopPointerHandler(self, handler, *token, false, caller, true); }
        catch (...) { Wh_Log(L"Switcher Loaded handler tracking failed: %08X", winrt::to_hresult()); }
    }
    return result;
}
HRESULT WINAPI LoadedRemove_Hook(void* self, winrt::event_token token) {
    auto result = LoadedRemove_Original(self, token);
    if (SUCCEEDED(result) && !g_stopping) {
        std::lock_guard lock(g_hoverHandlersMutex);
        std::erase_if(g_hoverSubscriptions, [&](auto const& saved) {
            return saved.loaded && saved.element == self && saved.token == token;
        });
    }
    return result;
}

void InstallXamlHooks() {
    if (g_hooksReady || g_stopping || g_installing.test_and_set()) return;
    try {
        // Run on an existing Explorer XAML thread. Creating a Grid on Windhawk's
        // worker thread would violate XAML's thread-affinity requirement.
        controls::Grid probe;
        auto ui = probe.as<xaml::IUIElement>();
        auto framework = probe.as<xaml::IFrameworkElement>();
        auto uiVtable = *reinterpret_cast<void***>(winrt::get_abi(ui));
        auto frameworkVtable = *reinterpret_cast<void***>(winrt::get_abi(framework));
        controls::ScrollViewer scrollProbe;
        auto scrollInterface = scrollProbe.as<controls::IScrollViewer>();
        auto scrollInterface2 = scrollProbe.as<controls::IScrollViewer2>();
        auto scrollVtable = *reinterpret_cast<void***>(winrt::get_abi(scrollInterface));
        auto scrollVtable2 = *reinterpret_cast<void***>(winrt::get_abi(scrollInterface2));
        // Public ABI slots verified against the installed C++/WinRT headers.
        bool scrollOffset = WindhawkUtils::SetFunctionHook(
            reinterpret_cast<ScrollHorizontal_t>(scrollVtable[58]), ScrollHorizontal_Hook, &ScrollHorizontal_Original);
        bool changeView = WindhawkUtils::SetFunctionHook(
            reinterpret_cast<ChangeView_t>(scrollVtable2[15]), ChangeView_Hook, &ChangeView_Original);
        animation::Storyboard storyboard;
        auto storyboardInterface = storyboard.as<animation::IStoryboard>();
        auto storyboardVtable = *reinterpret_cast<void***>(winrt::get_abi(storyboardInterface));
        bool begin = WindhawkUtils::SetFunctionHook(reinterpret_cast<StoryboardBegin_t>(storyboardVtable[9]),
                                      StoryboardBegin_Hook, &StoryboardBegin_Original);
        // Published IUIElement ABI slots, verified against the SDK headers.
        // Observe timer starts only while a verified desktop-hover delegate runs.
        xaml::DispatcherTimer timerProbe;
        auto timerInterface = timerProbe.as<xaml::IDispatcherTimer>();
        auto timerVtable = *reinterpret_cast<void***>(winrt::get_abi(timerInterface));
        bool hoverTimer = WindhawkUtils::SetFunctionHook(reinterpret_cast<HoverTimerStart_t>(timerVtable[11]),
            HoverTimerStart_Hook, &HoverTimerStart_Original);
        bool enterAdd = WindhawkUtils::SetFunctionHook(reinterpret_cast<PointerAdd_t>(uiVtable[63]),
            PointerEnterAdd_Hook, &PointerEnterAdd_Original);
        bool enterRemove = WindhawkUtils::SetFunctionHook(reinterpret_cast<PointerRemove_t>(uiVtable[64]),
            PointerEnterRemove_Hook, &PointerEnterRemove_Original);
        bool moveAdd = WindhawkUtils::SetFunctionHook(reinterpret_cast<PointerAdd_t>(uiVtable[59]),
            PointerMoveAdd_Hook, &PointerMoveAdd_Original);
        bool moveRemove = WindhawkUtils::SetFunctionHook(reinterpret_cast<PointerRemove_t>(uiVtable[60]),
            PointerMoveRemove_Hook, &PointerMoveRemove_Original);
        bool loadedAdd = WindhawkUtils::SetFunctionHook(reinterpret_cast<PointerAdd_t>(frameworkVtable[43]),
            LoadedAdd_Hook, &LoadedAdd_Original);
        bool loadedRemove = WindhawkUtils::SetFunctionHook(reinterpret_cast<PointerRemove_t>(frameworkVtable[44]),
            LoadedRemove_Hook, &LoadedRemove_Original);
        bool hoverHooks = enterAdd && enterRemove && moveAdd && moveRemove;
        if (!hoverHooks) {
            if (enterAdd) Wh_RemoveFunctionHook(uiVtable[63]);
            if (enterRemove) Wh_RemoveFunctionHook(uiVtable[64]);
            if (moveAdd) Wh_RemoveFunctionHook(uiVtable[59]);
            if (moveRemove) Wh_RemoveFunctionHook(uiVtable[60]);
        }
        if (begin && scrollOffset && changeView && loadedAdd && loadedRemove) {
            Wh_ApplyHookOperations();
            g_hooksReady = true;
            Wh_Log(L"Installed XAML Task View event and animation hooks");
        } else {
            if (loadedAdd) Wh_RemoveFunctionHook(frameworkVtable[43]);
            if (loadedRemove) Wh_RemoveFunctionHook(frameworkVtable[44]);
            if (hoverTimer) Wh_RemoveFunctionHook(timerVtable[11]);
            if (scrollOffset) Wh_RemoveFunctionHook(scrollVtable[58]);
            if (changeView) Wh_RemoveFunctionHook(scrollVtable2[15]);
            if (begin) Wh_RemoveFunctionHook(storyboardVtable[9]);
            if (hoverHooks) {
                Wh_RemoveFunctionHook(uiVtable[63]); Wh_RemoveFunctionHook(uiVtable[64]);
                Wh_RemoveFunctionHook(uiVtable[59]); Wh_RemoveFunctionHook(uiVtable[60]);
            }
            Wh_ApplyHookOperations();
            g_installing.clear();
            Wh_Log(L"Couldn't install all XAML hooks");
            return;
        }
    } catch (...) {
        Wh_Log(L"XAML thread initialization failed: %08X", winrt::to_hresult());
    }
    g_installing.clear();
}

using WindowThreadProc = void(*)(void*);
struct WindowThreadCall { WindowThreadProc proc; void* parameter; bool completed = false; };
UINT g_threadMessage;

LRESULT CALLBACK CallWindowHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        auto message = reinterpret_cast<CWPSTRUCT*>(lParam);
        if (message->message == g_threadMessage && message->lParam) {
            auto call = reinterpret_cast<WindowThreadCall*>(message->lParam);
            call->proc(call->parameter);
            call->completed = true;
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

bool RunOnWindowThread(HWND window, WindowThreadProc proc, void* parameter) {
    DWORD processId = 0;
    DWORD threadId = GetWindowThreadProcessId(window, &processId);
    if (!threadId || processId != GetCurrentProcessId()) return false;
    if (threadId == GetCurrentThreadId()) {
        proc(parameter);
        return true;
    }
    HHOOK hook = SetWindowsHookExW(WH_CALLWNDPROC, CallWindowHook, nullptr, threadId);
    if (!hook) return false;
    WindowThreadCall call{proc, parameter};
    SendMessageW(window, g_threadMessage, 0, reinterpret_cast<LPARAM>(&call));
    UnhookWindowsHookEx(hook);
    return call.completed;
}

void InitializeWindow(HWND window) {
    wchar_t className[128];
    if (g_hooksReady || g_stopping ||
        !GetClassNameW(window, className, ARRAYSIZE(className))) return;
    if (wcscmp(className, L"Windows.UI.Composition.DesktopWindowContentBridge") == 0 ||
        wcscmp(className, L"XamlExplorerHostIslandWindow") == 0) {
        RunOnWindowThread(window, [](void*) { InstallXamlHooks(); }, nullptr);
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR className,
                               LPCWSTR title, DWORD style, int x, int y,
                               int width, int height, HWND parent, HMENU menu,
                               HINSTANCE instance, LPVOID parameter) {
    HWND window = CreateWindowExW_Original(exStyle, className, title, style, x, y,
                                        width, height, parent, menu, instance, parameter);
    if (window) InitializeWindow(window);
    return window;
}
}  // namespace

BOOL Wh_ModInit() {
    LoadFrameSettings();
    const wchar_t* viewMode = Wh_GetStringSetting(L"General.viewMode");
    g_viewMode = (std::max)(0, (std::min)(viewMode ? _wtoi(viewMode) : 0, 4));
    Wh_FreeStringSetting(viewMode);
    g_desktopFrameMargin = (std::max)(0, (std::min)(Wh_GetIntSetting(L"DesktopList.desktopFrameMargin"), 200));
    int speed = Wh_GetIntSetting(L"Animations.scrollAnimationSpeed");
    g_scrollSpeed = (std::max)(10, (std::min)(speed > 0 ? speed : 100, 500));
    g_nativeScrollDuration = 250;
    HMODULE xamlModule = LoadLibraryExW(L"Windows.UI.Xaml.dll", nullptr,
                                      LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!xamlModule) return FALSE;
    // Windows.UI.Xaml.dll
    WindhawkUtils::SYMBOL_HOOK windowsUiXamlHooks[] = {
        {{L"long __cdecl DirectUI::DXamlServices::GetPeer(class CDependencyObject *,struct _GUID const &,void * *)"},
         &GetPeer},
        {{L"long __cdecl Storyboard_SetTarget(class CTimeline *,class CDependencyObject *)"},
         &CoreSetTarget_Original, CoreSetTarget_Hook},
        {{L"public: class xref_ptr<class CDependencyObject> __cdecl CTimeline::GetTargetObject(void)const "},
         &TimelineGetTarget_Original},
        {{L"long __cdecl ctl::do_query_interface<class DirectUI::DependencyObject,struct IInspectable>(class ctl::ComPtr<class DirectUI::DependencyObject> &,struct IInspectable *)"}, &QueryNativePeer},
        {{L"protected: void __cdecl ctl::ComPtr<class DirectUI::DependencyObject>::InternalRelease(void)"}, &ReleaseNativePeer},
        {{L"public: class CDependencyObject * __cdecl DirectUI::DependencyObject::GetHandle(void)const "}, &NativePeerHandle},
        {{L"public: void __cdecl CDependencyObject::Release(void)"}, &CoreRelease},
        {{L"public: long __cdecl CStoryboard::BeginPrivate(unsigned int)"}, &CoreStoryboardBegin_Original, CoreStoryboardBegin_Hook},
    };
    if (!WindhawkUtils::HookSymbols(xamlModule, windowsUiXamlHooks,
                                    ARRAYSIZE(windowsUiXamlHooks))) {
        Wh_Log(L"Required XAML animation symbols are unavailable");
        return FALSE;
    }
    Wh_Log(L"Installed XAML animation symbol hooks");
    g_threadMessage = RegisterWindowMessageW(
        L"Windhawk.TaskViewCustomizer.RunOnThread");
    return g_threadMessage && WindhawkUtils::SetFunctionHook(
        CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original);
}

void Wh_ModAfterInit() {
    EnumWindows([](HWND window, LPARAM) __attribute__((stdcall)) -> BOOL {
        DWORD processId;
        GetWindowThreadProcessId(window, &processId);
        if (processId != GetCurrentProcessId()) return TRUE;
        InitializeWindow(window);
        EnumChildWindows(window, [](HWND child, LPARAM) __attribute__((stdcall)) -> BOOL {
            InitializeWindow(child);
            return TRUE;
        }, 0);
        return TRUE;
    }, 0);
    if (!g_hooksReady) Wh_Log(L"Waiting for an Explorer XAML window");
}

void Wh_ModSettingsChanged() {
    LoadFrameSettings();
    const wchar_t* viewMode = Wh_GetStringSetting(L"General.viewMode");
    g_viewMode = (std::max)(0, (std::min)(viewMode ? _wtoi(viewMode) : 0, 4));
    Wh_FreeStringSetting(viewMode);
    g_desktopFrameMargin = (std::max)(0, (std::min)(Wh_GetIntSetting(L"DesktopList.desktopFrameMargin"), 200));
    int speed = Wh_GetIntSetting(L"Animations.scrollAnimationSpeed");
    g_scrollSpeed = (std::max)(10, (std::min)(speed > 0 ? speed : 100, 500));
    std::vector<DWORD> threads;
    {
        std::lock_guard lock(g_statesMutex);
        for (auto const& state : *g_states)
            if (std::find(threads.begin(), threads.end(), state->threadId) == threads.end()) threads.push_back(state->threadId);
    }
    for (auto thread : threads) {
        HWND window = nullptr;
        EnumThreadWindows(thread, [](HWND candidate, LPARAM parameter) __attribute__((stdcall)) -> BOOL {
            *reinterpret_cast<HWND*>(parameter) = candidate; return FALSE;
        }, reinterpret_cast<LPARAM>(&window));
        if (window) RunOnWindowThread(window, [](void*) {
            std::vector<std::shared_ptr<LayoutState>> states;
            {
                std::lock_guard lock(g_statesMutex);
                for (auto const& state : *g_states) if (state->threadId == GetCurrentThreadId()) states.push_back(state);
            }
            for (auto const& state : states) {
                state->appearanceDirty = state->clearanceDirty = true;
                try { state->Apply(); } catch (...) { Wh_Log(L"Task View settings update failed: %08X", winrt::to_hresult()); }
            }
        }, nullptr);
    }
}

void Wh_ModBeforeUninit() {
    g_stopping = true;
    std::vector<std::shared_ptr<LayoutState>> states;
    {
        std::lock_guard lock(g_statesMutex);
        states.swap(*g_states);
    }
    for (auto& state : states) {
        HWND threadWindow = nullptr;
        EnumThreadWindows(state->threadId, [](HWND window, LPARAM parameter) __attribute__((stdcall)) -> BOOL {
            *reinterpret_cast<HWND*>(parameter) = window;
            return FALSE;
        }, reinterpret_cast<LPARAM>(&threadWindow));
        bool released = threadWindow && RunOnWindowThread(threadWindow, [](void* parameter) {
            auto& current = *static_cast<std::shared_ptr<LayoutState>*>(parameter);
            try { current->Restore(); }
            catch (...) { Wh_Log(L"Task View restoration failed: %08X", winrt::to_hresult()); }
            current.reset(); // Last XAML references released on their owning UI thread.
        }, &state);
        if (!released) {
            // The UI thread may already have exited. Never destroy its XAML
            // references on the engine thread or pin another copy of the mod.
            (void)new std::shared_ptr<LayoutState>(std::move(state));
            Wh_Log(L"UI thread unavailable; leaving its affine references untouched");
        }
    }
}

void Wh_ModUninit() {
    // Empty vector storage is safe to release here; UI-owned states were
    // already released on their UI threads in Wh_ModBeforeUninit.
    g_states.reset();
    Wh_Log(L"Mica objects remaining at unload: %d materials, %d effects",
           g_liveWallpaperMaterials.load(), g_liveWallpaperEffects.load());
}

// ==WindhawkMod==
// @id              taskbar-separators-dynamic-centering
// @name            Taskbar Separators Dynamic Centering
// @name:zh-CN      任务栏分隔线动态居中
// @description     Add customizable taskbar separators that dynamically center the icon group after a chosen position (fork of Taskbar Separators). Requires the taskbar to be set to Left alignment.
// @description:zh-CN 在任务栏应用按钮之间添加可自定义的分隔线，并可把指定位置之后的图标作为一组相对整条任务栏居中（Taskbar Separators 分叉）。需要把任务栏对齐方式设为左对齐。
// @version         1.0.0
// @author          Suioio
// @github          https://github.com/Suioio
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -ladvapi32
// ==/WindhawkMod==

// Copyright (C) 2026 digART
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// Taskbar hook and UI-thread infrastructure includes code and patterns adapted
// from Windhawk mods by Michael Maltsev (m417z), including Taskbar Labels for
// Windows 11, Taskbar Multirow, and Windows 11 Taskbar Styler.
//
// App-name targeting and before/after separator placement are based on
// a contribution from mileso in GitHub PR #2.
//
// This mod is a fork of Taskbar Separators by digART:
// https://github.com/digart11/taskbar-separators
// ==WindhawkModReadme==
/*

# Taskbar Separators Dynamic Centering

Add clean, customizable visual separators between application buttons on the
Windows 11 taskbar, and optionally center the icon group after a chosen
separator position relative to the whole taskbar.

> This is a fork of [Taskbar Separators](https://windhawk.net/mods/taskbar-separators)
> by digART, based on version 1.3.0.
> **Do not enable it together with the original Taskbar Separators mod** — both
> mods rewrite taskbar button margins and will fight over the layout.

> **`Opacity` defaults to `0`, and that is intentional.** The dividers
> draw no line and, with the default **Divider gap** of `0`, reserve no space of
> their own either, so what stays visible is the gap produced by dynamic
> centering, not a line. Raise **Opacity** to make the dividers themselves
> visible.

> **No dynamic centering means no dividers.** The dividers are only the
> mechanical means this fork uses to center the icon group, so the mod creates
> them only while dynamic centering is in effect: on a **left-aligned,
> horizontal** taskbar. On a centered taskbar, on a vertical taskbar, and on a
> mirrored (right-to-left) taskbar, no dividers exist, and they are created
> again as soon as the taskbar is left-aligned (and horizontal) again.

## Preview

![Taskbar Separators Dynamic Centering preview](https://raw.githubusercontent.com/Suioio/taskbar-separators-dynamic-centering/main/preview.png)

![Dynamic centering in action: the icon group after the chosen position re-centers as apps open and close](https://raw.githubusercontent.com/Suioio/taskbar-separators-dynamic-centering/main/dynamic-centering.gif)

## What this fork adds

- **Dynamic centering**: the icons after a chosen position are centered as a
  group relative to the whole taskbar. It is applied only while the taskbar is
  left-aligned and horizontal, and the dividers exist only while it is applied
- **Dynamic separator hiding**: hide the dynamic separator and stop dynamic
  centering once the number of icons after the position reaches a configured
  count
- **Drag freeze**: separator geometry is frozen while taskbar icons are being
  dragged or reordered, avoiding layout feedback loops
- **Post-release rebuild**: spacing is rebuilt over several frames after the
  mouse is released, fixing leftover temporary margins
- **Immediate rebuild on app close**: the taskbar panel layout is watched, so
  spacing is rebuilt as soon as an application is closed instead of waiting for
  the next click
- Drag-side spacing is aligned to the physical pixel grid

## Getting started

1. Open the mod's **Settings** tab.
2. Add taskbar positions or application names to the **Separators** list.
3. For example, `3` places a separator after the third application button,
   while `+Notepad` places one before Notepad.
4. Select a style and adjust its appearance.
5. Use **Divider gap** to reserve additional physical space around configured
   separators.

## Position behavior

Separators can target either a taskbar position or a specific application.

Placement prefixes:

- `+` = before
- `-` = after
- `+-` or `-+` = before and after

Examples: `+2`, `-3`, `+-3`, `+Notepad`, `-Notepad`, `+-Notepad`.

Positions follow the current visual order of taskbar application buttons.
Application-name separators are matched against the taskbar button's accessible
name, with tooltip text as a fallback, case-insensitively and by whole word.
Start, Search, Widgets, Task View and other system buttons are not counted as
application buttons.

## Dynamic centering

**Dynamic centering** centers the icons after **Dynamic centering position** as
a group relative to the whole taskbar, by inserting a computed physical gap at
that position. It is applied only while the taskbar is **left-aligned and
horizontal**. **Hide when middle icon count is at least** hides the dynamic
divider when the number of icons after the position reaches the configured
value.

The dividers themselves are only the mechanical means of dynamic centering:
**without dynamic centering there are no dividers.** While dynamic centering is
not in effect — a centered taskbar, a vertical taskbar, or a mirrored
(right-to-left) taskbar — the mod creates no dividers and removes the ones that
were there. The next section describes how that is detected.

## Taskbar alignment

Windows centers the whole button row itself in its own **Center** taskbar
alignment mode, which leaves no room for the mod to position the icons after the
dynamic separator independently. Dynamic centering therefore only works on a
**left-aligned, horizontal** taskbar. The Windows setting behind this is
`TaskbarAl`: `0` means left, any other value (usually `1`) means center.

Dynamic centering has no separate on/off setting: it is applied whenever the
taskbar is **left-aligned and horizontal**, and it is never applied while the
taskbar is centered, vertical, or mirrored (right-to-left). On a mirrored
taskbar the buttons are laid out in the opposite direction and the centering
geometry is written for the left-to-right order only, so dynamic centering is
deliberately not applied there instead of silently collapsing the gap.

**The mod never writes the taskbar alignment or any other Windows setting.** It
only reads `TaskbarAl` to find out whether the taskbar is left-aligned (`0`) or
centered (any other value, usually `1`).

**The mod goes by the stored `TaskbarAl` value**, which it reads from the
registry. If another mod forces left alignment by hooking the read instead of
changing that stored value — taskbar-multirow does exactly that — this mod
still sees a centered taskbar and stays inactive.

**To use dynamic centering, switch the taskbar to Left alignment in Windows'
taskbar settings.**

- With a **centered** taskbar nothing is applied: Windows centers the whole
  button row itself in that mode, the mod deliberately does not change the
  alignment behind your back, and **no dividers are created**.
- The alignment monitor runs for as long as the mod does and reacts in **both
  directions**. Switching the taskbar to **Center** while dynamic centering is
  active turns dynamic centering off and removes the dividers; switching it back
  to **Left** turns dynamic centering on again and recreates the dividers. No
  setting change and no mod reload is needed. The reason is written to the
  Windhawk log:
  `the user changed the taskbar alignment; dynamic centering is off and the dividers are removed`.
- Dynamic centering is **not supported on vertical taskbars**, and a vertical
  taskbar keeps no dividers either: the dividers exist only as the means of
  dynamic centering.

## Settings

All appearance settings (`Style`, `Color`, `Opacity`, `Thickness`, `Length`,
`Shape size`, `Corner radius`, `Fade amount`, `Glow size`, `Glow opacity`,
`Double gap`, `Divider gap`, `Taskbar orientation`, `Animation compatibility`,
`Separator before first app`) work as in the original mod.

**Opacity defaults to `0`, which is intentional**: the dividers then draw
nothing and, with the default **Divider gap** of `0`, reserve no space of their
own either, so what stays visible is the gap produced by dynamic centering, not
a line. Raise **Opacity** to make the dividers themselves visible. The dynamic
divider also needs its position listed in `Separators`, otherwise no gap is
reserved for it, and the dividers themselves exist only while dynamic centering
is in effect (a left-aligned, horizontal taskbar).

The settings this fork adds:

| Setting | Values | Meaning |
| --- | --- | --- |
| Dynamic centering position | taskbar position, default `4` | The separator position used for the dynamic centering and gap behavior. It must also be listed in `Separators`, otherwise no gap is reserved for it. An app-name separator that resolves to this position carries the centering gap just as well. It is not applied on a mirrored (right-to-left) taskbar. |
| Hide when middle icon count is at least | icon count, default `11` | Hides the dynamic separator and stops dynamic centering once the number of icons after the dynamic position reaches this value. |

## Compatibility

- Windows 11 horizontal taskbars
- Vertical taskbars via Vertical Taskbar for Windows 11: dynamic centering is
  not supported there, and a vertical taskbar keeps no dividers either
- Mirrored (right-to-left) taskbars: dynamic centering is not applied there and
  no dividers are created
- Compatible with Windows 11 Taskbar Styler in normal configurations
- Taskbar labels and uncombined or otherwise variable-width taskbar buttons
- Mixed multi-monitor layouts with different button modes on each taskbar
- **Not compatible with the original Taskbar Separators mod** (do not enable
  both)

## Uninstall and conflicts

- The mod never writes the taskbar alignment or any other Windows setting, so
  there is nothing to restore: disabling or removing it simply stops dynamic
  centering and removes the dividers, and your own alignment applies as it
  always did.
- While dynamic centering is active, if you switch the taskbar alignment back to
  Center, the mod does not override your choice. It turns dynamic centering off,
  removes the dividers, and logs the reason
  (`the user changed the taskbar alignment; dynamic centering is off and the dividers are removed`).
  Switching the alignment back to Left recreates the dividers automatically.

## License and attribution

Licensed under the GNU General Public License v3.0.

This mod is a fork of Taskbar Separators by digART, and the original mod and its
copyright belong to digART.

Taskbar hook and UI-thread infrastructure includes code and patterns adapted
from Windhawk mods by Michael Maltsev (m417z), including Taskbar Labels for
Windows 11, Taskbar Multirow, and Windows 11 Taskbar Styler.

App-name targeting and before/after separator placement are based on a
contribution from mileso in GitHub PR #2.

## 中文说明

本模组是 [Taskbar Separators](https://windhawk.net/mods/taskbar-separators)
（作者 digART）的分叉，基于其 1.3.0 版本。

**请勿与原版 Taskbar Separators 同时启用**，两者都会改写任务栏按钮间距而互相冲突。

> **`Opacity` 默认是 `0`，这是有意为之。** 分隔线不画线，本身也不额外占用空间
> （**Divider gap** 默认同样是 `0`）；画面里留下的是动态居中产生的空隙，而不是线条。
> 想让分隔线本身可见，就把 **Opacity** 调高。

> **没有动态居中，就没有分隔线。** 分隔线只是本分叉用来把图标组居中的机械手段，因此
> 模组只在动态居中生效期间——**左对齐且水平**的任务栏——创建分隔线。任务栏居中时、
> 垂直任务栏上、以及镜像（从右到左）的任务栏上都不存在分隔线；任务栏切回左对齐
> （且水平）后会重新创建。

### 相对原版新增

- **动态居中**：以指定位置为界，把其后的图标作为一组相对整条任务栏居中；它只在任务栏**左对齐且水平**时生效，分隔线也只在它生效期间存在
- 右侧图标数量达到设定值时隐藏动态分隔线，并同时停止动态居中
- 拖动/互换任务栏图标时冻结分隔线几何，避免布局反馈回环
- 释放鼠标后连续多帧重建间距，修复临时边距残留
- 监听任务栏面板布局，关闭应用后立即重建间距
- 拖拽侧间距对齐物理像素网格

### 设置项

设置项的名称与取值同上面的英文一节，这里只强调两条本分叉新增的行为：
**Dynamic centering position** 指定用于动态居中的分隔线位置，该位置必须同时出现在
`Separators` 列表中，否则不会为它预留间距；按应用名配置的分割线解析后落在该位置也算。
**Hide when middle icon count is at least** 在动态位置之后的图标数量达到该值时隐藏动态
分隔线，并同时停止动态居中。

### 动态居中需要左对齐且水平

Windows 在“居中”模式下由系统居中整排按钮，模组无法单独定位分隔线之后的那组图标，
因此动态居中只在**左对齐且水平**的任务栏上生效。任务栏对齐设置 `TaskbarAl` 中，`0` 表示左对齐，
其它值（通常是 `1`）表示居中。动态居中**没有单独的开关**：条件满足时自动生效；任务栏居中、
垂直或镜像时一律不生效。镜像（从右到左）的任务栏上按钮按相反方向排布，而居中几何只按
从左到右的顺序计算，因此模组直接不应用动态居中，而不是让间距静默塌陷，自然也不创建分隔线。

**模组从不写入任务栏对齐设置，也不写入任何其它 Windows 设置**，它只读取注册表里**存储的**
`TaskbarAl` 值。若有另一个模组通过 hook 读取来强制左对齐（例如 taskbar-multirow 就是这么
做的）而没有改动这个存储值，本模组仍会认为任务栏是居中的，从而保持不激活。

**要使用动态居中，请在 Windows 的任务栏设置里把对齐方式改为左对齐。**

- 任务栏**居中**时不会生效：该模式下整排按钮由系统居中，模组也不会背着你改对齐设置，
  并且**不创建任何分隔线**。
- 对齐监控在模组运行期间常驻，并**双向**响应：把任务栏切到**居中**，动态居中会关闭并移除分隔线；
  切回**左对齐**，动态居中会重新开启并重建分隔线。不需要改动设置，也不需要重新加载模组。
  原因会写入 Windhawk 日志：
  `the user changed the taskbar alignment; dynamic centering is off and the dividers are removed`。
- **垂直任务栏不支持动态居中**，垂直任务栏上同样不保留分隔线，因为分隔线只是动态居中的实现手段。
- **镜像（RTL）任务栏同样不支持动态居中**，也不保留分隔线。

## 许可与署名

本模组以 GNU General Public License v3.0 发布。

本模组是 digART 的 Taskbar Separators 的分叉，原模组及其版权归 digART 所有。

任务栏挂钩与 UI 线程基础设施包含改编自 Michael Maltsev（m417z）模组的代码与模式，
包括 Taskbar Labels for Windows 11、Taskbar Multirow 与 Windows 11 Taskbar Styler。

按应用名定位分隔线与前后放置逻辑基于 mileso 在 GitHub PR #2 中的贡献。
*/
// ==/WindhawkModReadme==

// clang-format off
// ==WindhawkModSettings==
/*
- style: fade
  $name: Style
  $description: "Line styles: Solid, Fade, Double, Rounded, and Glow. Shape styles:
    Dot, Ring, Square, and Diamond."
  $options:
  - solid: Solid
  - fade: Fade
  - double: Double
  - rounded: Rounded
  - glow: Glow
  - dot: Dot
  - ring: Ring
  - square: Square
  - diamond: Diamond
- separatorGap: 0
  $name: Divider gap
  $description: Extra physical space reserved at each divider position, from 0 to 32 pixels. Set to 0 for the original overlay-only behavior. Only in effect while dynamic centering is applied, that is on a left-aligned, horizontal taskbar.
- color: "#FFFF00"
  $name: Color
  $description: "Divider color in #RRGGBB or #AARRGGBB format."
- opacity: 0
  $name: Opacity
  $description: The default 0 is intentional. The divider draws no line and, with the default Divider gap of 0, reserves no space of its own either; raise this value to make the dividers visible. Divider opacity as a percentage, from 0 to 100.
  $description:zh-CN: 默认值 0 是有意为之：分隔线不画线，本身也不额外占用空间（Divider gap 默认同样是 0）；想让分隔线本身可见就把该值调高。分隔线不透明度百分比，取值 0 到 100。
- width: 2
  $name: Thickness
  $description: Line thickness, and Ring stroke thickness, in pixels from 1 to 8.
- height: 22
  $name: Length
  $description: Divider length for line styles in pixels, from 4 to 48.
- shapeSize: 8
  $name: Shape size
  $description: Size of Dot, Ring, Square, and Diamond styles in pixels, from 2 to 24.
- orientation: auto
  $name: Taskbar orientation
  $description: Automatic uses the direction between adjacent realized icons.
  $options:
  - auto: Automatic
  - horizontal: Horizontal taskbar
  - vertical: Vertical taskbar
- animationCompatibility: "off"
  $name: Animation compatibility
  $description: Track animated taskbar icons. Turn off for static separators
    with no animation-tracking overhead.
  $options:
  - "on": On
  - "off": Off
- separatorBeforeFirstApp: false
  $name: Separator before first app
  $description: Show a separator before the first taskbar application button.
- dynamicPosition: 4
  $name: Dynamic centering position
  $name:zh-CN: 动态居中位置
  $description: The separator position used for the dynamic centering and gap behavior. It must also be listed in Separators, otherwise no gap is reserved for it. An app-name separator that resolves to this position carries the centering gap just as well. Dynamic centering is applied only on a left-aligned, horizontal taskbar, and never on a mirrored (right-to-left) one.
  $description:zh-CN: 用于动态居中与间距行为的任务栏位置。该位置必须同时出现在 Separators 列表中，否则不会为它预留间距。按应用名配置的分割线在解析后可能正好落在此位置，同样可以承载居中间距。动态居中只在左对齐且水平的任务栏上生效，镜像（从右到左）的任务栏上不生效。
- hideWhenMiddleIconCountAtLeast: 11
  $name: Hide when middle icon count is at least
  $name:zh-CN: 右侧图标数量达到此值时隐藏
  $description: Hide the dynamic separator and stop dynamic centering once the number of icons after the dynamic position reaches this value.
  $description:zh-CN: 当动态位置之后的图标数量达到此值时隐藏动态分隔线，并停止动态居中。
- separators:
  - '4'
  $name: Separators
  $description: |
    Place dividers by taskbar position or app name.

    Dividers exist only while dynamic centering is applied, that is on a
    left-aligned, horizontal taskbar.

    + = before
    - = after
    +- or -+ = both

    Examples:
    +-3 = before and after the 3rd icon
    +-Notepad = before and after Notepad
    +2 = before the 2nd icon

    App-name dividers follow the app when it is moved.
- cornerRadius: 2
  $name: Corner radius
  $description: Rounded style corner radius in pixels, from 0 to 12.
- fadeAmount: 70
  $name: Fade amount
  $description: Fade style end-region size as a percentage, from 0 to 100.
- glowSize: 4
  $name: Glow size
  $description: Glow style expansion in pixels, from 0 to 16.
- glowOpacity: 30
  $name: Glow opacity
  $description: Glow layer opacity as a percentage, from 0 to 100.
- doubleGap: 3
  $name: Double gap
  $description: Distance between double-line centers in pixels, from 1 to 12.
*/
// ==/WindhawkModSettings==
// clang-format on

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <Windows.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.h>
#include <winrt/base.h>

#include <algorithm>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cwchar>
#include <cwctype>
#include <limits>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

namespace {

enum class ReconcileResult {
    succeeded,
    succeededPartial,
    temporarilyNotReady,
};

enum class TaskbarOrientation {
    horizontal,
    vertical,
};

enum class DividerGeometryMode {
    iconCenters,
    buttonBoundaries,
};

enum class OrientationSetting {
    automatic,
    horizontal,
    vertical,
};

enum class DividerStyle {
    solid,
    rounded,
    fade,
    glow,
    doubleLine,
    dot,
    ring,
    square,
    diamond,
};

struct SeparatorSettings {
    size_t settingsIndex = 0;
    int position = 4;
    std::wstring appName;
    bool before = false;
};

struct Settings {
    int width = 2;
    int height = 22;
    int opacityPercent = 100;
    winrt::Windows::UI::Color color{255, 255, 255, 0};
    DividerStyle style = DividerStyle::fade;
    int cornerRadius = 2;
    int fadeAmount = 70;
    int glowSize = 4;
    int glowOpacityPercent = 30;
    int doubleGap = 3;
    int shapeSize = 8;
    int separatorGap = 0;
    OrientationSetting orientation = OrientationSetting::automatic;
    bool animationCompatibility = false;
    bool separatorBeforeFirstApp = false;
    std::vector<SeparatorSettings> separators;

    // Dynamic centering: center the icons after a chosen position as a
    // group whenever the taskbar is left-aligned. Whether centering is
    // applied right now is tracked separately in g_dynamicCenteringActive,
    // which is false on a centered taskbar.
    int dynamicPosition = 4;
    int hideWhenMiddleIconCountAtLeast = 11;
};

std::mutex g_settingsMutex;
Settings g_settings;
std::atomic<unsigned int> g_settingsGeneration{0};

std::atomic<bool> g_taskbarViewDllLoaded{false};
std::atomic<bool> g_unloading{false};

// Dynamic centering: while the taskbar is left-aligned and horizontal the mod
// watches the taskbar alignment value (TaskbarAl) through
// RegNotifyChangeKeyValue. The mod never writes that value. The monitor is
// resident and reacts in both directions: switching the taskbar to center turns
// dynamic centering off (and with it the dividers), switching it back to left
// turns it on again.
// Stop uses an event so uninit never blocks on a long Sleep.
// g_dynamicCenteringActive is the runtime answer to "is dynamic centering in
// effect right now". The layout code must use this flag rather than re-reading
// the taskbar alignment: this flag says what is actually applied.
std::atomic<bool> g_alignMonitorStop{false};
std::atomic<bool> g_dynamicCenteringActive{false};
HANDLE g_alignMonitorThread = nullptr;
HANDLE g_alignStopEvent = nullptr;
// StartTaskbarAlignMonitor runs on the load/settings-change path (including any
// explorer thread through LoadLibraryExW_Hook) while StopTaskbarAlignMonitor
// runs on the unload path, so the monitor handles above are only touched under
// this mutex.
std::mutex g_alignMonitorMutex;

struct AnimationDividerCache {
    winrt::weak_ref<Controls::Canvas> host;
    winrt::weak_ref<FrameworkElement> previousIcon;
    winrt::weak_ref<FrameworkElement> targetIcon;
    winrt::weak_ref<FrameworkElement> nextIcon;
    winrt::weak_ref<FrameworkElement> previousButton;
    winrt::weak_ref<FrameworkElement> targetButton;
    winrt::weak_ref<FrameworkElement> nextButton;
    TaskbarOrientation orientation = TaskbarOrientation::horizontal;
    DividerGeometryMode geometryMode = DividerGeometryMode::iconCenters;
    bool beforeFirst = false;
    double primaryOffset = 0;
    bool hasLastPosition = false;
    double lastLeft = 0;
    double lastTop = 0;
};

using AnimationClock = std::chrono::steady_clock;
constexpr auto kAnimationTrackingTimeout = std::chrono::seconds(3);
constexpr auto kNativeSettlingTimeout = std::chrono::seconds(1);
constexpr int kGeometryStableFrameThreshold = 12;

struct ReconciledButtonSignature {
    int itemIndex = -1;
    winrt::weak_ref<FrameworkElement> button;
    winrt::weak_ref<FrameworkElement> iconPanel;
    winrt::weak_ref<FrameworkElement> icon;
    bool iconResolved = false;
    double actualWidth = 0;
    double actualHeight = 0;
    Thickness margin{};
    Visibility visibility = Visibility::Visible;
};

struct ReconciledSeparatorVisual {
    std::wstring name;
    winrt::weak_ref<Controls::Canvas> host;
};

struct TrackedButtonMarginState {
    winrt::weak_ref<FrameworkElement> button;
    Thickness baseMargin{};
    Thickness lastAppliedMargin{};
    bool baseMarginWasLocal = false;
    bool hasAppliedMargin = false;
};

struct ButtonGapContribution {
    double left = 0;
    double top = 0;
    double right = 0;
    double bottom = 0;
};

struct TrackedTaskbarState {
    size_t id = 0;
    winrt::weak_ref<FrameworkElement> repeater;
    winrt::weak_ref<Controls::Grid> rootGrid;
    winrt::weak_ref<Controls::Canvas> overlayCanvas;

    // Cached SystemTray.SystemTrayFrame of this taskbar. The dynamic-centering
    // gap clamps the centered group against the tray, and the lookup is a
    // depth-12 visual-tree walk, so the element is resolved once and reused
    // while it is still under the same root grid.
    winrt::weak_ref<FrameworkElement> trayFrame;
    unsigned int appliedSettingsGeneration = 0;
    size_t lastActiveDividerCount = static_cast<size_t>(-1);

    bool reconciliationSignatureValid = false;
    ReconcileResult cachedReconcileResult =
        ReconcileResult::temporarilyNotReady;
    winrt::weak_ref<FrameworkElement> reconciledRepeater;
    winrt::weak_ref<Controls::Grid> reconciledRootGrid;
    double reconciledRootWidth = 0;
    double reconciledRootHeight = 0;
    winrt::Windows::Foundation::Rect reconciledRepeaterBounds{};
    std::vector<ReconciledButtonSignature> reconciledButtons;
    bool reconciledOverlayExpected = false;
    uint32_t reconciledOverlayChildCount = 0;
    bool reconciledAnimationCompatibility = false;
    std::vector<ReconciledSeparatorVisual> reconciledSeparators;

    // Physical divider gaps are applied as a delta on top of the taskbar
    // button margins that already exist (Windows and/or another mod).
    std::vector<TrackedButtonMarginState> buttonMargins;

    std::vector<AnimationDividerCache> animationDividers;
    winrt::weak_ref<Controls::Canvas> animationOverlayCanvas;
    winrt::weak_ref<Controls::Grid> animationPointerSource;
    winrt::Windows::Foundation::IInspectable animationPointerMovedHandler{
        nullptr};
    winrt::event_token animationPointerExitedToken{};
    winrt::event_token animationRenderingToken{};
    bool animationPointerHandlersAttached = false;
    bool animationPointerExitedHandlerAttached = false;

    // A taskbar-button drag can finish after the last UpdateVisualStates call
    // that we observed. Catch PointerReleased on the taskbar root and request
    // one structural reconcile on the next composition frame, after Windows
    // has committed the new item order.
    winrt::weak_ref<Controls::Grid> reorderPointerSource;
    winrt::Windows::Foundation::IInspectable reorderPointerPressedHandler{
        nullptr};
    winrt::Windows::Foundation::IInspectable reorderPointerReleasedHandler{
        nullptr};
    bool reorderPointerHandlerAttached = false;
    bool reorderStructuralReconcilePending = false;

    // Frames left to force a full reconcile after release.
    int postReleaseReconcileFrames = 0;

    // Layout-change monitor for the taskbar repeater panel.
    // Closing an app removes a button out from under the split margins,
    // which halves the gap until a reconcile runs; LayoutUpdated fires on
    // any such relayout so the removed/re-added button count is detected
    // and a forced reconcile restores the spacing immediately.
    bool layoutMonitorAttached = false;
    winrt::event_token layoutUpdatedToken{};
    winrt::weak_ref<Controls::Panel> layoutMonitorPanel;
    // The forced reconcile above writes margins, which schedules another layout
    // pass and raises LayoutUpdated again, so this handler never re-enters
    // itself and gives up after a bounded number of tries per observed
    // realized-button count.
    bool layoutForcedReconcileActive = false;
    int layoutForcedReconcileAttempts = 0;
    size_t layoutObservedButtonCount = 0;
    bool layoutObservedButtonCountValid = false;

    // Drag freeze state for the dynamic gap.
    bool reorderDragActive = false;
    bool hasFrozenDynamicGap = false;
    double frozenDynamicGap = 0;
    bool frozenDynamicSeparatorVisible = true;
    double lastAppliedDynamicGap = 0;
    bool lastDynamicSeparatorVisible = true;

    // Remember which button is being dragged so the
    // dynamic gap can be kept on the correct side while dragging.
    winrt::weak_ref<FrameworkElement> draggedButton;
    int draggedButtonIndex = -1;

    bool animationRenderingSubscribed = false;
    bool animationPointerInside = false;
    bool animationRenderingCallbackActive = false;
    int animationStableFrames = 0;
    AnimationClock::time_point animationLastActivity{};
    bool nativeSettlingActive = false;
    int nativeSettlingStableFrames = 0;
    AnimationClock::time_point nativeSettlingStarted{};
};

struct TaskbarAppDetails {
    std::wstring displayName;
};

bool IsWordBoundary(wchar_t ch) {
    return std::iswspace(ch) || ch == L'-' || ch == L'(' || ch == L')' ||
           ch == L'[' || ch == L']' || ch == L',' || ch == L'.' || ch == L':' ||
           ch == L'/' || ch == L'\\';
}

bool ContainsWholeIgnoreCase(std::wstring_view source,
                             std::wstring_view target) {
    if (source.empty() || target.empty() || target.size() > source.size()) {
        return false;
    }

    for (size_t i = 0; i + target.size() <= source.size(); ++i) {
        if (CompareStringOrdinal(source.data() + i,
                                 static_cast<int>(target.size()), target.data(),
                                 static_cast<int>(target.size()),
                                 TRUE) != CSTR_EQUAL) {
            continue;
        }

        bool leftBoundary = i == 0 || IsWordBoundary(source[i - 1]);

        size_t end = i + target.size();

        bool rightBoundary =
            end == source.size() || IsWordBoundary(source[end]);

        if (leftBoundary && rightBoundary) {
            return true;
        }
    }

    return false;
}

bool IsMatch(const TaskbarAppDetails& details,
             std::wstring_view searchPattern) {
    if (searchPattern.empty() || details.displayName.empty()) {
        return false;
    }

    return ContainsWholeIgnoreCase(details.displayName, searchPattern);
}

std::wstring TrimWhitespace(std::wstring name) {
    while (!name.empty() && std::iswspace(name.front())) {
        name.erase(name.begin());
    }

    while (!name.empty() && std::iswspace(name.back())) {
        name.pop_back();
    }

    return name;
}

// --- Taskbar Button Details Extractor ---
TaskbarAppDetails GetTaskbarButtonDetails(FrameworkElement const& button) {
    TaskbarAppDetails details;

    if (!button) {
        return details;
    }

    try {
        auto autoName =
            winrt::Windows::UI::Xaml::Automation::AutomationProperties::GetName(
                button);

        if (!autoName.empty()) {
            details.displayName = autoName.c_str();
        }
    } catch (...) {
    }

    if (details.displayName.empty()) {
        try {
            auto tooltipObject = Controls::ToolTipService::GetToolTip(button);

            if (auto tooltipText = tooltipObject.try_as<winrt::hstring>()) {
                details.displayName = tooltipText->c_str();
            }
        } catch (...) {
        }
    }

    details.displayName = TrimWhitespace(details.displayName);

    return details;
}

using TrackedTaskbarCollection = std::vector<TrackedTaskbarState>;

// Accessed only from taskbar XAML/UI-thread callbacks. Keep the TLS object
// itself trivially destructible: normal unload cleanup explicitly destroys
// the collection on the taskbar UI thread, and no TLS destructor can run
// after this mod DLL has been unloaded.
thread_local TrackedTaskbarCollection* g_trackedTaskbars = nullptr;
thread_local size_t g_nextTrackedTaskbarId = 1;
thread_local bool g_reconcilingTaskbars = false;

Controls::Grid DiscoverPrimaryTaskbarRootGrid();
ReconcileResult ReconcileTaskbarRepeater(FrameworkElement const& repeater,
                                         bool forceStructuralReconcile);
bool HasAppNameSeparators(Settings const& settings);

TrackedTaskbarCollection& GetTrackedTaskbars() {
    if (!g_trackedTaskbars) {
        g_trackedTaskbars = new TrackedTaskbarCollection;
    }

    return *g_trackedTaskbars;
}

void DestroyTrackedTaskbars() {
    auto* trackedTaskbars = g_trackedTaskbars;
    g_trackedTaskbars = nullptr;
    delete trackedTaskbars;
}

int HexDigitValue(wchar_t character) {
    if (character >= L'0' && character <= L'9') {
        return character - L'0';
    }

    if (character >= L'a' && character <= L'f') {
        return character - L'a' + 10;
    }

    if (character >= L'A' && character <= L'F') {
        return character - L'A' + 10;
    }

    return -1;
}

bool ParseHexByte(PCWSTR text, uint8_t* result) {
    int high = HexDigitValue(text[0]);
    int low = HexDigitValue(text[1]);
    if (high < 0 || low < 0) {
        return false;
    }

    *result = static_cast<uint8_t>((high << 4) | low);
    return true;
}

bool ParseColor(PCWSTR text, winrt::Windows::UI::Color* result) {
    if (text[0] != L'#') {
        return false;
    }

    size_t length = wcslen(text);
    if (length != 7 && length != 9) {
        return false;
    }

    winrt::Windows::UI::Color color{255, 255, 255, 255};
    size_t rgbOffset = 1;

    if (length == 9) {
        if (!ParseHexByte(text + 1, &color.A)) {
            return false;
        }
        rgbOffset = 3;
    }

    if (!ParseHexByte(text + rgbOffset, &color.R) ||
        !ParseHexByte(text + rgbOffset + 2, &color.G) ||
        !ParseHexByte(text + rgbOffset + 4, &color.B)) {
        return false;
    }

    *result = color;
    return true;
}

DividerStyle ParseDividerStyle(PCWSTR text) {
    if (wcscmp(text, L"solid") == 0) {
        return DividerStyle::solid;
    }
    if (wcscmp(text, L"rounded") == 0) {
        return DividerStyle::rounded;
    }
    if (wcscmp(text, L"fade") == 0) {
        return DividerStyle::fade;
    }
    if (wcscmp(text, L"glow") == 0) {
        return DividerStyle::glow;
    }
    if (wcscmp(text, L"double") == 0) {
        return DividerStyle::doubleLine;
    }
    if (wcscmp(text, L"dot") == 0) {
        return DividerStyle::dot;
    }
    if (wcscmp(text, L"ring") == 0) {
        return DividerStyle::ring;
    }
    if (wcscmp(text, L"square") == 0) {
        return DividerStyle::square;
    }
    if (wcscmp(text, L"diamond") == 0) {
        return DividerStyle::diamond;
    }
    return DividerStyle::fade;
}

void LoadSettings() {
    Settings settings;

    settings.width = std::clamp(Wh_GetIntSetting(L"width"), 1, 8);
    settings.height = std::clamp(Wh_GetIntSetting(L"height"), 4, 48);
    settings.opacityPercent = std::clamp(Wh_GetIntSetting(L"opacity"), 0, 100);

    auto colorText = WindhawkUtils::StringSetting::make(L"color");
    if (!ParseColor(colorText.get(), &settings.color)) {
        Wh_Log(L"Invalid color; using #FFFF00");
        settings.color = winrt::Windows::UI::Color{255, 255, 255, 0};
    }

    auto styleText = WindhawkUtils::StringSetting::make(L"style");
    settings.style = ParseDividerStyle(styleText.get());
    settings.cornerRadius =
        std::clamp(Wh_GetIntSetting(L"cornerRadius"), 0, 12);
    settings.fadeAmount = std::clamp(Wh_GetIntSetting(L"fadeAmount"), 0, 100);
    settings.glowSize = std::clamp(Wh_GetIntSetting(L"glowSize"), 0, 16);
    settings.glowOpacityPercent =
        std::clamp(Wh_GetIntSetting(L"glowOpacity"), 0, 100);
    settings.doubleGap = std::clamp(Wh_GetIntSetting(L"doubleGap"), 1, 12);
    settings.shapeSize = std::clamp(Wh_GetIntSetting(L"shapeSize"), 2, 24);
    settings.separatorGap =
        std::clamp(Wh_GetIntSetting(L"separatorGap"), 0, 32);

    // Dynamic centering settings.
    settings.dynamicPosition =
        std::max(1, Wh_GetIntSetting(L"dynamicPosition"));
    settings.hideWhenMiddleIconCountAtLeast =
        std::max(1, Wh_GetIntSetting(L"hideWhenMiddleIconCountAtLeast"));

    auto orientationText = WindhawkUtils::StringSetting::make(L"orientation");
    if (wcscmp(orientationText.get(), L"horizontal") == 0) {
        settings.orientation = OrientationSetting::horizontal;
    } else if (wcscmp(orientationText.get(), L"vertical") == 0) {
        settings.orientation = OrientationSetting::vertical;
    }

    auto animationCompatibilityText =
        WindhawkUtils::StringSetting::make(L"animationCompatibility");
    settings.animationCompatibility =
        wcscmp(animationCompatibilityText.get(), L"on") == 0;
    settings.separatorBeforeFirstApp =
        Wh_GetIntSetting(L"separatorBeforeFirstApp") != 0;

    int settingsIndexOffset = 0;
    auto appendSeparator = [&](size_t settingsIndex, PCWSTR positionStr) {
        if (!positionStr) {
            return;
        }

        std::wstring value = positionStr;

        // Trim leading/trailing whitespace.
        while (!value.empty() && std::iswspace(value.front())) {
            value.erase(value.begin());
        }

        while (!value.empty() && std::iswspace(value.back())) {
            value.pop_back();
        }

        if (value.empty()) {
            return;
        }

        SeparatorSettings separator;
        separator.settingsIndex = settingsIndex + settingsIndexOffset;
        separator.before = false;

        bool beforeAndAfter = false;

        // Placement prefixes:
        // +target   = before
        // -target   = after
        // +-target  = before and after
        // -+target  = before and after
        if (value.rfind(L"+-", 0) == 0 || value.rfind(L"-+", 0) == 0) {
            separator.before = true;
            beforeAndAfter = true;
            value.erase(0, 2);
        } else if (value[0] == L'+') {
            separator.before = true;
            value.erase(0, 1);
        } else if (value[0] == L'-') {
            separator.before = false;
            value.erase(0, 1);
        }

        // Trim again after removing a prefix.
        while (!value.empty() && std::iswspace(value.front())) {
            value.erase(value.begin());
        }

        while (!value.empty() && std::iswspace(value.back())) {
            value.pop_back();
        }

        if (value.empty()) {
            return;
        }

        errno = 0;
        wchar_t* end = nullptr;
        long long numericPosition = std::wcstoll(value.c_str(), &end, 10);
        bool isNumeric = end && end != value.c_str() && *end == L'\0';

        if (isNumeric) {
            if (errno == ERANGE || numericPosition <= 0 ||
                numericPosition > std::numeric_limits<int>::max()) {
                return;
            }

            separator.position = static_cast<int>(numericPosition);
        } else {
            separator.appName = value;
        }

        settings.separators.push_back(separator);

        if (beforeAndAfter) {
            SeparatorSettings afterSeparator = separator;
            afterSeparator.before = false;
            afterSeparator.settingsIndex++;

            settingsIndexOffset++;

            settings.separators.push_back(afterSeparator);
        }
    };

    for (int index = 0; index < 128; index++) {
        PCWSTR positionStr = Wh_GetStringSetting(L"separators[%d]", index);
        appendSeparator(static_cast<size_t>(index), positionStr);
        Wh_FreeStringSetting(positionStr);
    }

    // The dynamic position only reserves a gap while it is also a configured
    // separator; the rebuild path silently ignores it otherwise. An app-name
    // separator only learns its position during the reconcile, so such a
    // configuration can cover the dynamic position even though no entry names
    // it literally. Only warn when neither form can reserve the gap.
    if (!HasAppNameSeparators(settings) &&
        std::none_of(settings.separators.begin(), settings.separators.end(),
                     [&](SeparatorSettings const& separator) {
                         int position = separator.position;
                         if (separator.before) {
                             if (position == 1) {
                                 return false;
                             }
                             position--;
                         }
                         return position == settings.dynamicPosition;
                     })) {
        Wh_Log(L"dynamicPosition %d is not in the separators list; no gap is "
               L"reserved for it",
               settings.dynamicPosition);
    }

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_settings = settings;
    }

    g_settingsGeneration.fetch_add(1, std::memory_order_release);
}

Settings GetSettingsSnapshot() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_settings;
}

bool HasAppNameSeparators(Settings const& settings) {
    return std::any_of(settings.separators.begin(), settings.separators.end(),
                       [](SeparatorSettings const& separator) {
                           return !separator.appName.empty();
                       });
}

bool ColorsEqual(winrt::Windows::UI::Color const& left,
                 winrt::Windows::UI::Color const& right) {
    return left.A == right.A && left.R == right.R && left.G == right.G &&
           left.B == right.B;
}

bool ThicknessApproximatelyEqual(Thickness const& left,
                                 Thickness const& right) {
    constexpr double kMarginEpsilon = 0.01;
    return std::isfinite(left.Left) && std::isfinite(left.Top) &&
           std::isfinite(left.Right) && std::isfinite(left.Bottom) &&
           std::isfinite(right.Left) && std::isfinite(right.Top) &&
           std::isfinite(right.Right) && std::isfinite(right.Bottom) &&
           std::fabs(left.Left - right.Left) <= kMarginEpsilon &&
           std::fabs(left.Top - right.Top) <= kMarginEpsilon &&
           std::fabs(left.Right - right.Right) <= kMarginEpsilon &&
           std::fabs(left.Bottom - right.Bottom) <= kMarginEpsilon;
}

std::vector<HWND> EnumerateCurrentProcessTaskbarWindows() {
    struct TaskbarWindows {
        std::vector<HWND> primary;
        std::vector<HWND> secondary;
    } taskbarWindows;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD processId = 0;
            WCHAR className[32]{};

            if (GetWindowThreadProcessId(hWnd, &processId) &&
                processId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className))) {
                auto* taskbarWindows =
                    reinterpret_cast<TaskbarWindows*>(lParam);
                if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
                    taskbarWindows->primary.push_back(hWnd);
                } else if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") ==
                           0) {
                    taskbarWindows->secondary.push_back(hWnd);
                }
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&taskbarWindows));

    std::vector<HWND> result;
    result.reserve(taskbarWindows.primary.size() +
                   taskbarWindows.secondary.size());
    result.insert(result.end(), taskbarWindows.primary.begin(),
                  taskbarWindows.primary.end());
    result.insert(result.end(), taskbarWindows.secondary.begin(),
                  taskbarWindows.secondary.end());
    return result;
}

HWND FindCurrentProcessTaskbarWnd() {
    auto taskbarWindows = EnumerateCurrentProcessTaskbarWindows();
    return taskbarWindows.empty() ? nullptr : taskbarWindows.front();
}

HWND GetTaskbarDispatchWindow(HWND taskbarWnd) {
    if (!taskbarWnd) {
        return nullptr;
    }

    HWND taskbarUiWnd = FindWindowEx(
        taskbarWnd, nullptr,
        L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
    return taskbarUiWnd ? taskbarUiWnd : taskbarWnd;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

// Adapted from the official Windows 11 Taskbar Styler mod. SendMessage is
// blocking: it returns only after the hook procedure has run the callback, so
// the parameter can live on the caller's stack and be destroyed with it, and
// the hook is always removed before this function returns. A true return means
// the hook observed the message and the callback returned.
bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
        bool callbackRan;
    };

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (threadId == 0) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp =
                    reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto* param =
                        reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(
                            cwp->lParam);
                    param->callbackRan = true;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param{proc, procParam, false};
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0,
                reinterpret_cast<LPARAM>(&param));

    UnhookWindowsHookEx(hook);
    return param.callbackRan;
}

void* CTaskBand_ITaskListWndSite_vftable;
void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

// Adapted from the official Taskbar Multirow mod. This obtains the existing
// taskbar XamlRoot without installing another XAML diagnostics client, which
// keeps this mod compatible with Taskbar Styler.
XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0]) {
        if (taskbarHostSharedPtr[1]) {
            std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
        }
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x10;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[3] == 0x28 &&
            b[4] == 0x48 && b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_ARM64)
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#else
#error "Unsupported architecture"
#endif

    auto* taskbarElementIUnknown = *reinterpret_cast<IUnknown**>(
        static_cast<BYTE*>(taskbarHostSharedPtr[0]) +
        taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    if (taskbarElementIUnknown) {
        taskbarElementIUnknown->QueryInterface(
            winrt::guid_of<FrameworkElement>(), winrt::put_abi(taskbarElement));
    }

    XamlRoot result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    if (taskbarHostSharedPtr[1]) {
        std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);
    }

    return result;
}

XamlRoot GetTaskbarXamlRoot(HWND taskbarWnd) {
    HWND taskSwitchWnd =
        reinterpret_cast<HWND>(GetProp(taskbarWnd, L"TaskbandHWND"));
    if (!taskSwitchWnd) {
        return nullptr;
    }

    void* taskBand =
        reinterpret_cast<void*>(GetWindowLongPtr(taskSwitchWnd, 0));
    if (!taskBand) {
        return nullptr;
    }

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *reinterpret_cast<void**>(taskBandForTaskListWndSite) !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite =
            reinterpret_cast<void**>(taskBandForTaskListWndSite) + 1;
    }
    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

// Established taskbar-multirow pattern for Shell_SecondaryTrayWnd: the
// secondary task-band object is stored on its WorkerW child, not in the
// primary-only TaskbandHWND property.
XamlRoot GetSecondaryTaskbarXamlRoot(HWND secondaryTaskbarWnd) {
    HWND taskSwitchWnd =
        FindWindowEx(secondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!taskSwitchWnd) {
        return nullptr;
    }

    void* taskBand =
        reinterpret_cast<void*>(GetWindowLongPtr(taskSwitchWnd, 0));
    if (!taskBand) {
        return nullptr;
    }

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *reinterpret_cast<void**>(taskBandForTaskListWndSite) !=
                    CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite =
            reinterpret_cast<void**>(taskBandForTaskListWndSite) + 1;
    }
    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                               taskbarHostSharedPtr);
    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetTaskbarXamlRootForWindow(HWND taskbarWnd) {
    WCHAR className[32]{};
    if (!taskbarWnd ||
        !GetClassName(taskbarWnd, className, ARRAYSIZE(className))) {
        return nullptr;
    }

    if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
        return GetSecondaryTaskbarXamlRoot(taskbarWnd);
    }
    if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
        return GetTaskbarXamlRoot(taskbarWnd);
    }
    return nullptr;
}

// {0BD894F2-EDFC-5DDF-A166-2DB14BBFDF35}
constexpr winrt::guid IItemsRepeater{
    0x0BD894F2,
    0xEDFC,
    0x5DDF,
    {0xA1, 0x66, 0x2D, 0xB1, 0x4B, 0xBF, 0xDF, 0x35}};

int ItemsRepeater_GetElementIndex(FrameworkElement repeater,
                                  UIElement element) {
    winrt::Windows::Foundation::IUnknown repeaterUnknown = nullptr;
    repeater.as(IItemsRepeater, winrt::put_abi(repeaterUnknown));

    using GetElementIndex_t =
        HRESULT(WINAPI*)(void* pThis, void* element, void* index);

    void** vtable = *(void***)winrt::get_abi(repeaterUnknown);
    auto getElementIndex = (GetElementIndex_t)vtable[19];

    int index = -1;
    getElementIndex(winrt::get_abi(repeaterUnknown), winrt::get_abi(element),
                    &index);
    return index;
}

FrameworkElement FindRepeaterAncestor(FrameworkElement element) {
    auto current = element;

    for (int depth = 0; depth < 16 && current; depth++) {
        auto parentObject = Media::VisualTreeHelper::GetParent(current);
        auto parent = parentObject.try_as<FrameworkElement>();
        if (!parent) {
            return nullptr;
        }

        if (parent.Name() == L"TaskbarFrameRepeater") {
            return parent;
        }

        current = parent;
    }

    return nullptr;
}

FrameworkElement FindDescendantByName(FrameworkElement root,
                                      PCWSTR name,
                                      int depth = 0) {
    if (!root || depth > 12) {
        return nullptr;
    }

    int childCount = Media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < childCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(root, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }

        if (child.Name() == name) {
            return child;
        }

        auto found = FindDescendantByName(child, name, depth + 1);
        if (found) {
            return found;
        }
    }

    return nullptr;
}

FrameworkElement FindDescendantByClassName(FrameworkElement root,
                                           PCWSTR className,
                                           int depth = 0) {
    if (!root || depth > 12) {
        return nullptr;
    }

    int childCount = Media::VisualTreeHelper::GetChildrenCount(root);
    for (int i = 0; i < childCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(root, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }

        if (winrt::get_class_name(child) == className) {
            return child;
        }

        auto found = FindDescendantByClassName(child, className, depth + 1);
        if (found) {
            return found;
        }
    }

    return nullptr;
}
FrameworkElement FindAncestorByName(FrameworkElement element, PCWSTR name) {
    for (int depth = 0; depth < 16 && element; depth++) {
        if (element.Name() == name) {
            return element;
        }
        element = Media::VisualTreeHelper::GetParent(element)
                      .try_as<FrameworkElement>();
    }
    return nullptr;
}

Controls::Grid FindRootGridAncestor(FrameworkElement element) {
    auto current = element;

    for (int depth = 0; depth < 16 && current; depth++) {
        auto parent = Media::VisualTreeHelper::GetParent(current)
                          .try_as<FrameworkElement>();
        if (!parent) {
            return nullptr;
        }

        if (parent.Name() == L"RootGrid") {
            return parent.try_as<Controls::Grid>();
        }

        current = parent;
    }

    return nullptr;
}

struct DividerGeometry {
    winrt::Windows::Foundation::Rect targetBounds{};
    winrt::Windows::Foundation::Rect nextBounds{};
    double center = 0;
    double left = 0;
    double top = 0;
};

bool TryGetElementBounds(Controls::Canvas const& overlayCanvas,
                         FrameworkElement const& element,
                         winrt::Windows::Foundation::Rect* bounds) {
    if (!overlayCanvas || !element || !bounds) {
        return false;
    }

    double width = element.ActualWidth();
    double height = element.ActualHeight();
    if (!std::isfinite(width) || width <= 0 || !std::isfinite(height) ||
        height <= 0) {
        return false;
    }

    auto transformedBounds =
        element.TransformToVisual(overlayCanvas)
            .TransformBounds(winrt::Windows::Foundation::Rect{
                0, 0, static_cast<float>(width), static_cast<float>(height)});
    if (!std::isfinite(transformedBounds.X) ||
        !std::isfinite(transformedBounds.Y) ||
        !std::isfinite(transformedBounds.Width) ||
        !std::isfinite(transformedBounds.Height) ||
        transformedBounds.Width <= 0 || transformedBounds.Height <= 0) {
        return false;
    }

    *bounds = transformedBounds;
    return true;
}

double PrimaryStart(winrt::Windows::Foundation::Rect const& bounds,
                    TaskbarOrientation orientation) {
    return orientation == TaskbarOrientation::horizontal ? bounds.X : bounds.Y;
}

double PrimarySize(winrt::Windows::Foundation::Rect const& bounds,
                   TaskbarOrientation orientation) {
    return orientation == TaskbarOrientation::horizontal ? bounds.Width
                                                         : bounds.Height;
}

double PrimaryCenter(winrt::Windows::Foundation::Rect const& bounds,
                     TaskbarOrientation orientation) {
    return PrimaryStart(bounds, orientation) +
           PrimarySize(bounds, orientation) / 2.0;
}

double CrossCenter(winrt::Windows::Foundation::Rect const& bounds,
                   TaskbarOrientation orientation) {
    return orientation == TaskbarOrientation::horizontal
               ? bounds.Y + bounds.Height / 2.0
               : bounds.X + bounds.Width / 2.0;
}

double ElementPrimarySize(FrameworkElement const& element,
                          TaskbarOrientation orientation) {
    return orientation == TaskbarOrientation::horizontal
               ? element.ActualWidth()
               : element.ActualHeight();
}

double ElementCrossSize(FrameworkElement const& element,
                        TaskbarOrientation orientation) {
    return orientation == TaskbarOrientation::horizontal
               ? element.ActualHeight()
               : element.ActualWidth();
}

bool IsUsableApplicationButton(FrameworkElement const& button) {
    if (!button || button.Visibility() != Visibility::Visible) {
        return false;
    }

    double width = button.ActualWidth();
    double height = button.ActualHeight();
    return std::isfinite(width) && width > 0 && std::isfinite(height) &&
           height > 0;
}

DividerGeometryMode DetectDividerGeometryMode(
    std::vector<FrameworkElement> const& buttons,
    std::vector<FrameworkElement> const& icons,
    TaskbarOrientation orientation) {
    double minimumButtonPrimarySize = std::numeric_limits<double>::max();
    double maximumButtonPrimarySize = 0;
    size_t validButtonCount = 0;
    bool hasLabelLikePrimaryExpansion = false;

    for (size_t index = 0; index < buttons.size(); index++) {
        auto const& button = buttons[index];
        if (!IsUsableApplicationButton(button)) {
            continue;
        }

        double buttonPrimarySize = ElementPrimarySize(button, orientation);
        double buttonCrossSize = ElementCrossSize(button, orientation);

        minimumButtonPrimarySize =
            std::min(minimumButtonPrimarySize, buttonPrimarySize);
        maximumButtonPrimarySize =
            std::max(maximumButtonPrimarySize, buttonPrimarySize);
        validButtonCount++;

        auto icon = index < icons.size() ? icons[index] : nullptr;
        if (!icon) {
            continue;
        }

        double iconPrimarySize = ElementPrimarySize(icon, orientation);
        double iconCrossSize = ElementCrossSize(icon, orientation);
        if (!std::isfinite(iconPrimarySize) || iconPrimarySize <= 0 ||
            !std::isfinite(iconCrossSize) || iconCrossSize <= 0) {
            continue;
        }

        double primaryChrome = buttonPrimarySize - iconPrimarySize;
        double crossChrome = buttonCrossSize - iconCrossSize;
        double materialExpansion = std::max(8.0, buttonCrossSize * 0.15);
        if (primaryChrome - crossChrome > materialExpansion) {
            hasLabelLikePrimaryExpansion = true;
        }
    }

    bool hasVariableButtonSizes = false;
    if (validButtonCount >= 2) {
        double materialVariation =
            std::max(2.0, minimumButtonPrimarySize * 0.05);
        hasVariableButtonSizes =
            maximumButtonPrimarySize - minimumButtonPrimarySize >
            materialVariation;
    }

    return hasVariableButtonSizes || hasLabelLikePrimaryExpansion
               ? DividerGeometryMode::buttonBoundaries
               : DividerGeometryMode::iconCenters;
}

bool TryGetPrimaryOrderingDirection(
    Controls::Canvas const& overlayCanvas,
    std::vector<FrameworkElement> const& elements,
    TaskbarOrientation orientation,
    double* direction) {
    if (!direction) {
        return false;
    }

    for (size_t index = 1; index < elements.size(); index++) {
        winrt::Windows::Foundation::Rect previousBounds{};
        winrt::Windows::Foundation::Rect currentBounds{};
        if (!elements[index - 1] || !elements[index] ||
            !TryGetElementBounds(overlayCanvas, elements[index - 1],
                                 &previousBounds) ||
            !TryGetElementBounds(overlayCanvas, elements[index],
                                 &currentBounds)) {
            continue;
        }

        double primaryMovement = PrimaryCenter(currentBounds, orientation) -
                                 PrimaryCenter(previousBounds, orientation);
        double crossMovement = CrossCenter(currentBounds, orientation) -
                               CrossCenter(previousBounds, orientation);
        if (std::isfinite(primaryMovement) && std::isfinite(crossMovement) &&
            std::fabs(primaryMovement) > 0.1 &&
            std::fabs(primaryMovement) > std::fabs(crossMovement)) {
            *direction = primaryMovement;
            return true;
        }
    }

    return false;
}

bool IsMirroredPrimaryOrdering(
    Controls::Canvas const& overlayCanvas,
    std::vector<FrameworkElement> const& elements,
    TaskbarOrientation orientation) {
    bool sawQualifyingPair = false;

    for (size_t index = 1; index < elements.size(); index++) {
        winrt::Windows::Foundation::Rect previousBounds{};
        winrt::Windows::Foundation::Rect currentBounds{};
        if (!elements[index - 1] || !elements[index] ||
            !TryGetElementBounds(overlayCanvas, elements[index - 1],
                                 &previousBounds) ||
            !TryGetElementBounds(overlayCanvas, elements[index],
                                 &currentBounds)) {
            continue;
        }

        double primaryMovement = PrimaryCenter(currentBounds, orientation) -
                                 PrimaryCenter(previousBounds, orientation);
        double crossMovement = CrossCenter(currentBounds, orientation) -
                               CrossCenter(previousBounds, orientation);
        if (!std::isfinite(primaryMovement) || !std::isfinite(crossMovement) ||
            std::fabs(primaryMovement) <= 0.1 ||
            std::fabs(primaryMovement) <= std::fabs(crossMovement)) {
            continue;
        }

        // A single pair moving the other way is enough to disprove a mirrored
        // ordering, and it is deliberately not enough to prove one: while a
        // reorder animates, the repeater already holds the new order while the
        // buttons are still travelling, so one pair can read as reversed for a
        // frame even on a plain left-to-right taskbar.
        if (primaryMovement > 0) {
            return false;
        }

        sawQualifyingPair = true;
    }

    // No adjacent pair is laid out far enough to say anything yet, so the
    // ordering is not known to be mirrored and nothing is cleared.
    return sawQualifyingPair;
}

double GetRasterizationScale(FrameworkElement const& element) {
    try {
        auto xamlRoot = element ? element.XamlRoot() : nullptr;
        double scale = xamlRoot ? xamlRoot.RasterizationScale() : 1.0;
        if (std::isfinite(scale) && scale > 0) {
            return scale;
        }
    } catch (...) {
    }

    return 1.0;
}

double SnapToPhysicalPixel(double value, double rasterizationScale) {
    return std::round(value * rasterizationScale) / rasterizationScale;
}

void SnapDividerGeometryToPhysicalPixels(
    FrameworkElement const& referenceElement,
    DividerGeometry* geometry) {
    if (!geometry) {
        return;
    }

    double scale = GetRasterizationScale(referenceElement);
    geometry->left = SnapToPhysicalPixel(geometry->left, scale);
    geometry->top = SnapToPhysicalPixel(geometry->top, scale);
}

double GetBoundaryDividerPrimaryOffset(DividerGeometry const& geometry,
                                       TaskbarOrientation orientation,
                                       bool beforeFirst,
                                       bool afterLast,
                                       double separatorGap) {
    if ((!beforeFirst && !afterLast) || separatorGap <= 0) {
        return 0;
    }

    double direction = PrimaryCenter(geometry.nextBounds, orientation) -
                       PrimaryCenter(geometry.targetBounds, orientation);
    if (!std::isfinite(direction) || std::fabs(direction) <= 0.1) {
        return 0;
    }

    double signedHalfGap = std::copysign(separatorGap / 2.0, direction);
    return beforeFirst ? -signedHalfGap : signedHalfGap;
}

void ApplyDividerPrimaryOffset(FrameworkElement const& referenceElement,
                               TaskbarOrientation orientation,
                               double primaryOffset,
                               DividerGeometry* geometry) {
    if (orientation == TaskbarOrientation::horizontal) {
        geometry->left += primaryOffset;
    } else {
        geometry->top += primaryOffset;
    }
    SnapDividerGeometryToPhysicalPixels(referenceElement, geometry);
}

bool TryCalculateDividerPosition(double primaryCenter,
                                 double crossOrigin,
                                 TaskbarOrientation orientation,
                                 double rectangleWidth,
                                 double rectangleHeight,
                                 double* left,
                                 double* top) {
    if (orientation == TaskbarOrientation::horizontal) {
        *left = primaryCenter - rectangleWidth / 2.0;
        *top = crossOrigin;
    } else {
        *left = crossOrigin;
        *top = primaryCenter - rectangleHeight / 2.0;
    }
    return std::isfinite(primaryCenter) && std::isfinite(*left) &&
           std::isfinite(*top);
}

double GetDirectionalButtonMargin(FrameworkElement const& button,
                                  TaskbarOrientation orientation,
                                  double direction,
                                  bool leading) {
    auto margin = button.Margin();
    if (orientation == TaskbarOrientation::horizontal) {
        if (leading) {
            return direction > 0 ? margin.Left : margin.Right;
        }
        return direction > 0 ? margin.Right : margin.Left;
    }

    if (leading) {
        return direction > 0 ? margin.Top : margin.Bottom;
    }
    return direction > 0 ? margin.Bottom : margin.Top;
}

void AddDirectionalButtonGap(ButtonGapContribution* contribution,
                             TaskbarOrientation orientation,
                             double direction,
                             bool leading,
                             double amount) {
    bool usePrimaryStart = leading == (direction > 0);
    if (orientation == TaskbarOrientation::horizontal) {
        (usePrimaryStart ? contribution->left : contribution->right) += amount;
    } else {
        (usePrimaryStart ? contribution->top : contribution->bottom) += amount;
    }
}

// Dynamic centering: count the application buttons after the position
// (1-based position, so separator after that button).
int CountMiddleIcons(std::vector<FrameworkElement> const& appButtons,
                     int position) {
    if (position <= 0 || position >= static_cast<int>(appButtons.size())) {
        return 0;
    }

    return static_cast<int>(appButtons.size()) - position;
}

// Dynamic centering: compute the physical gap needed to center the
// right-side group relative to the whole taskbar.
double CalculateDynamicCenteredGap(
    Controls::Grid const& rootGrid,
    Controls::Canvas const& overlayCanvas,
    std::vector<FrameworkElement> const& appButtons,
    int position,
    TaskbarOrientation orientation,
    bool* separatorVisible,
    winrt::weak_ref<FrameworkElement>* trayFrameCache) {
    if (separatorVisible) {
        *separatorVisible = true;
    }

    if (position <= 0 || !rootGrid) {
        return 0;
    }

    int middleIconCount = CountMiddleIcons(appButtons, position);
    Settings settings = GetSettingsSnapshot();
    if (settings.hideWhenMiddleIconCountAtLeast > 0 &&
        middleIconCount >= settings.hideWhenMiddleIconCountAtLeast) {
        if (separatorVisible) {
            *separatorVisible = false;
        }
        return 0;
    }

    size_t leftIndex = static_cast<size_t>(position - 1);
    if (leftIndex >= appButtons.size() || !appButtons[leftIndex]) {
        return 0;
    }

    FrameworkElement leftButton = appButtons[leftIndex];
    double leftGroupRight = 0;
    if (overlayCanvas) {
        winrt::Windows::Foundation::Rect bounds{};
        if (!TryGetElementBounds(overlayCanvas, leftButton, &bounds)) {
            return 0;
        }
        leftGroupRight =
            PrimaryStart(bounds, orientation) + PrimarySize(bounds, orientation);
    } else {
        auto point = leftButton.TransformToVisual(rootGrid)
                         .TransformPoint({0, 0});
        leftGroupRight =
            point.X + ElementPrimarySize(leftButton, orientation);
    }

    double rightGroupWidth = 0;
    for (size_t index = static_cast<size_t>(position);
         index < appButtons.size(); index++) {
        if (appButtons[index]) {
            rightGroupWidth +=
                ElementPrimarySize(appButtons[index], orientation);
        }
    }

    if (rightGroupWidth <= 0) {
        return 0;
    }

    double taskbarWidth = rootGrid.ActualWidth();
    if (!std::isfinite(taskbarWidth) || taskbarWidth <= 0) {
        return 0;
    }

    double desiredLeft = taskbarWidth / 2.0 - rightGroupWidth / 2.0;
    double desiredGap = desiredLeft - leftGroupRight;
    if (desiredGap < 0) {
        desiredGap = 0;
    }

    // Avoid pushing the right group into the system tray.
    double trayLeft = taskbarWidth;
    FrameworkElement trayFrame =
        trayFrameCache ? trayFrameCache->get() : nullptr;
    if (trayFrame) {
        // A cached frame is only reusable while it is still part of this root
        // grid: a rebuilt taskbar leaves the old one alive but detached, and
        // transforming against a detached element is invalid.
        auto cachedRoot = FindRootGridAncestor(trayFrame);
        if (!cachedRoot ||
            winrt::get_abi(cachedRoot) != winrt::get_abi(rootGrid)) {
            trayFrame = nullptr;
        }
    }
    if (!trayFrame) {
        trayFrame =
            FindDescendantByClassName(rootGrid, L"SystemTray.SystemTrayFrame");
        if (trayFrame && trayFrameCache) {
            *trayFrameCache = winrt::make_weak(trayFrame);
        }
    }
    if (trayFrame) {
        auto trayPoint =
            trayFrame.TransformToVisual(rootGrid).TransformPoint({0, 0});
        trayLeft = trayPoint.X;
    }

    double maxGap = trayLeft - rightGroupWidth - leftGroupRight;
    if (maxGap < 0) {
        maxGap = 0;
    }

    return std::min(desiredGap, maxGap);
}

bool TryGetButtonBoundaryDividerGeometry(Controls::Canvas const& overlayCanvas,
                                         FrameworkElement const& previousButton,
                                         FrameworkElement const& targetButton,
                                         FrameworkElement const& nextButton,
                                         bool beforeFirst,
                                         bool animatedCrossAxis,
                                         TaskbarOrientation orientation,
                                         double taskbarWidth,
                                         double taskbarHeight,
                                         double rectangleWidth,
                                         double rectangleHeight,
                                         DividerGeometry* geometry) {
    if (!overlayCanvas || !targetButton || !geometry ||
        !std::isfinite(rectangleWidth) || rectangleWidth <= 0 ||
        !std::isfinite(rectangleHeight) || rectangleHeight <= 0 ||
        !std::isfinite(taskbarWidth) || taskbarWidth <= 0 ||
        !std::isfinite(taskbarHeight) || taskbarHeight <= 0) {
        return false;
    }

    winrt::Windows::Foundation::Rect targetBounds{};
    if (!TryGetElementBounds(overlayCanvas, targetButton, &targetBounds)) {
        return false;
    }

    double direction = 0;
    double crossDirection = 0;
    winrt::Windows::Foundation::Rect adjacentBounds{};
    winrt::Windows::Foundation::Rect nextBounds{};
    if (nextButton) {
        if (!TryGetElementBounds(overlayCanvas, nextButton, &nextBounds)) {
            return false;
        }
        direction = PrimaryCenter(nextBounds, orientation) -
                    PrimaryCenter(targetBounds, orientation);
        crossDirection = CrossCenter(nextBounds, orientation) -
                         CrossCenter(targetBounds, orientation);
        adjacentBounds = nextBounds;
    } else if (previousButton && !beforeFirst) {
        winrt::Windows::Foundation::Rect previousBounds{};
        if (!TryGetElementBounds(overlayCanvas, previousButton,
                                 &previousBounds)) {
            return false;
        }
        direction = PrimaryCenter(targetBounds, orientation) -
                    PrimaryCenter(previousBounds, orientation);
        crossDirection = CrossCenter(targetBounds, orientation) -
                         CrossCenter(previousBounds, orientation);
        adjacentBounds = previousBounds;
    } else {
        return false;
    }

    if (!std::isfinite(direction) || !std::isfinite(crossDirection) ||
        std::fabs(direction) <= 0.1 ||
        std::fabs(direction) <= std::fabs(crossDirection)) {
        return false;
    }

    double targetStart = PrimaryStart(targetBounds, orientation);
    double targetEnd = targetStart + PrimarySize(targetBounds, orientation);
    double center = 0;
    if (beforeFirst) {
        double leadingEdge = direction > 0 ? targetStart : targetEnd;
        double availableSpacing = GetDirectionalButtonMargin(
            targetButton, orientation, direction, true);
        if (!std::isfinite(availableSpacing) || availableSpacing < -0.1) {
            return false;
        }
        center =
            leadingEdge -
            std::copysign(std::max(0.0, availableSpacing) / 2.0, direction);
    } else if (nextButton) {
        double nextStart = PrimaryStart(nextBounds, orientation);
        double nextEnd = nextStart + PrimarySize(nextBounds, orientation);
        double targetTrailingEdge = direction > 0 ? targetEnd : targetStart;
        double nextLeadingEdge = direction > 0 ? nextStart : nextEnd;
        center = (targetTrailingEdge + nextLeadingEdge) / 2.0;
    } else {
        double trailingEdge = direction > 0 ? targetEnd : targetStart;
        double availableSpacing = GetDirectionalButtonMargin(
            targetButton, orientation, direction, false);
        if (!std::isfinite(availableSpacing) || availableSpacing < -0.1) {
            return false;
        }
        center =
            trailingEdge +
            std::copysign(std::max(0.0, availableSpacing) / 2.0, direction);
    }

    double crossCenter =
        animatedCrossAxis ? (CrossCenter(targetBounds, orientation) +
                             CrossCenter(adjacentBounds, orientation)) /
                                2.0
        : orientation == TaskbarOrientation::horizontal ? taskbarHeight / 2.0
                                                        : taskbarWidth / 2.0;
    double crossOrigin = orientation == TaskbarOrientation::horizontal
                             ? crossCenter - rectangleHeight / 2.0
                             : crossCenter - rectangleWidth / 2.0;
    double left = 0;
    double top = 0;
    if (!TryCalculateDividerPosition(center, crossOrigin, orientation,
                                     rectangleWidth, rectangleHeight, &left,
                                     &top)) {
        return false;
    }

    geometry->targetBounds = targetBounds;
    geometry->nextBounds = adjacentBounds;
    geometry->center = center;
    geometry->left = left;
    geometry->top = top;
    SnapDividerGeometryToPhysicalPixels(overlayCanvas, geometry);
    return true;
}

bool TryGetDividerGeometry(Controls::Canvas const& overlayCanvas,
                           FrameworkElement const& previousIcon,
                           FrameworkElement const& targetIcon,
                           FrameworkElement const& nextIcon,
                           TaskbarOrientation orientation,
                           double taskbarWidth,
                           double taskbarHeight,
                           double rectangleWidth,
                           double rectangleHeight,
                           DividerGeometry* geometry) {
    if (!overlayCanvas || !targetIcon || !geometry ||
        !std::isfinite(rectangleWidth) || rectangleWidth <= 0 ||
        !std::isfinite(rectangleHeight) || rectangleHeight <= 0) {
        return false;
    }

    winrt::Windows::Foundation::Rect targetBounds{};
    winrt::Windows::Foundation::Rect nextBounds{};
    if (!TryGetElementBounds(overlayCanvas, targetIcon, &targetBounds)) {
        return false;
    }

    if (nextIcon) {
        if (!TryGetElementBounds(overlayCanvas, nextIcon, &nextBounds)) {
            return false;
        }
    } else {
        winrt::Windows::Foundation::Rect previousBounds{};
        if (!previousIcon || !TryGetElementBounds(overlayCanvas, previousIcon,
                                                  &previousBounds)) {
            return false;
        }

        double previousCenter = PrimaryCenter(previousBounds, orientation);
        double targetCenter = PrimaryCenter(targetBounds, orientation);
        double spacing = targetCenter - previousCenter;
        double previousCrossCenter = CrossCenter(previousBounds, orientation);
        double targetCrossCenter = CrossCenter(targetBounds, orientation);
        double crossSpacing = targetCrossCenter - previousCrossCenter;
        if (!std::isfinite(spacing) || !std::isfinite(crossSpacing) ||
            std::fabs(spacing) <= 0.1 ||
            std::fabs(spacing) <= std::fabs(crossSpacing)) {
            return false;
        }

        nextBounds = targetBounds;
        if (orientation == TaskbarOrientation::horizontal) {
            nextBounds.X = static_cast<float>(targetBounds.X + spacing);
        } else {
            nextBounds.Y = static_cast<float>(targetBounds.Y + spacing);
        }
    }

    double targetStart = PrimaryStart(targetBounds, orientation);
    double targetSize = PrimarySize(targetBounds, orientation);
    double targetCenter = targetStart + targetSize / 2.0;
    double nextStart = PrimaryStart(nextBounds, orientation);
    double nextSize = PrimarySize(nextBounds, orientation);
    double nextCenter = nextStart + nextSize / 2.0;
    double center;
    if (nextCenter >= targetCenter) {
        double targetEnd = targetStart + targetSize;
        center = nextStart >= targetEnd ? (targetEnd + nextStart) / 2.0
                                        : (targetCenter + nextCenter) / 2.0;
    } else {
        double nextEnd = nextStart + nextSize;
        center = nextEnd <= targetStart ? (targetStart + nextEnd) / 2.0
                                        : (targetCenter + nextCenter) / 2.0;
    }

    if (!std::isfinite(taskbarWidth) || taskbarWidth <= 0 ||
        !std::isfinite(taskbarHeight) || taskbarHeight <= 0) {
        return false;
    }

    double crossOrigin = orientation == TaskbarOrientation::horizontal
                             ? (taskbarHeight - rectangleHeight) / 2.0
                             : (taskbarWidth - rectangleWidth) / 2.0;
    double left;
    double top;
    if (!TryCalculateDividerPosition(center, crossOrigin, orientation,
                                     rectangleWidth, rectangleHeight, &left,
                                     &top)) {
        return false;
    }

    geometry->targetBounds = targetBounds;
    geometry->nextBounds = nextBounds;
    geometry->center = center;
    geometry->left = left;
    geometry->top = top;
    SnapDividerGeometryToPhysicalPixels(overlayCanvas, geometry);
    return true;
}

bool TryGetBeforeFirstDividerGeometry(Controls::Canvas const& overlayCanvas,
                                      FrameworkElement const& firstIcon,
                                      FrameworkElement const& secondIcon,
                                      TaskbarOrientation orientation,
                                      double rectangleWidth,
                                      double rectangleHeight,
                                      DividerGeometry* geometry) {
    if (!overlayCanvas || !firstIcon || !secondIcon || !geometry ||
        !std::isfinite(rectangleWidth) || rectangleWidth <= 0 ||
        !std::isfinite(rectangleHeight) || rectangleHeight <= 0) {
        return false;
    }

    winrt::Windows::Foundation::Rect firstBounds{};
    winrt::Windows::Foundation::Rect secondBounds{};
    if (!TryGetElementBounds(overlayCanvas, firstIcon, &firstBounds)) {
        return false;
    }

    double firstCenter = PrimaryCenter(firstBounds, orientation);
    double firstCrossCenter = CrossCenter(firstBounds, orientation);
    if (!TryGetElementBounds(overlayCanvas, secondIcon, &secondBounds)) {
        return false;
    }

    double secondCenter = PrimaryCenter(secondBounds, orientation);
    double pitch = secondCenter - firstCenter;
    double secondCrossCenter = CrossCenter(secondBounds, orientation);
    double crossPitch = secondCrossCenter - firstCrossCenter;
    if (!std::isfinite(pitch) || !std::isfinite(crossPitch) ||
        std::fabs(pitch) <= 0.1 || std::fabs(pitch) <= std::fabs(crossPitch)) {
        return false;
    }

    double virtualPreviousCenter = firstCenter - pitch;
    double center = (virtualPreviousCenter + firstCenter) / 2.0;
    double virtualPreviousCrossCenter = firstCrossCenter - crossPitch;
    double crossCenter = (virtualPreviousCrossCenter + firstCrossCenter) / 2.0;
    double crossOrigin = orientation == TaskbarOrientation::horizontal
                             ? crossCenter - rectangleHeight / 2.0
                             : crossCenter - rectangleWidth / 2.0;
    double left;
    double top;
    if (!TryCalculateDividerPosition(center, crossOrigin, orientation,
                                     rectangleWidth, rectangleHeight, &left,
                                     &top)) {
        return false;
    }

    geometry->targetBounds = firstBounds;
    geometry->nextBounds = secondBounds;
    geometry->center = center;
    geometry->left = left;
    geometry->top = top;
    SnapDividerGeometryToPhysicalPixels(overlayCanvas, geometry);
    return true;
}

bool TryGetAnimatedDividerGeometry(Controls::Canvas const& overlayCanvas,
                                   FrameworkElement const& previousIcon,
                                   FrameworkElement const& targetIcon,
                                   FrameworkElement const& nextIcon,
                                   TaskbarOrientation orientation,
                                   double taskbarWidth,
                                   double taskbarHeight,
                                   double rectangleWidth,
                                   double rectangleHeight,
                                   DividerGeometry* geometry) {
    if (!TryGetDividerGeometry(overlayCanvas, previousIcon, targetIcon,
                               nextIcon, orientation, taskbarWidth,
                               taskbarHeight, rectangleWidth, rectangleHeight,
                               geometry)) {
        return false;
    }

    double targetCrossCenter = CrossCenter(geometry->targetBounds, orientation);
    double nextCrossCenter = CrossCenter(geometry->nextBounds, orientation);
    double crossCenter = (targetCrossCenter + nextCrossCenter) / 2.0;
    double crossOrigin = orientation == TaskbarOrientation::horizontal
                             ? crossCenter - rectangleHeight / 2.0
                             : crossCenter - rectangleWidth / 2.0;
    double left;
    double top;
    bool positionValid = TryCalculateDividerPosition(
        geometry->center, crossOrigin, orientation, rectangleWidth,
        rectangleHeight, &left, &top);
    if (orientation == TaskbarOrientation::horizontal) {
        geometry->top = top;
    } else {
        geometry->left = left;
    }
    if (positionValid) {
        SnapDividerGeometryToPhysicalPixels(overlayCanvas, geometry);
    }
    return positionValid;
}

TrackedTaskbarState* FindTrackedTaskbarById(size_t taskbarId) {
    if (!g_trackedTaskbars) {
        return nullptr;
    }

    auto& trackedTaskbars = *g_trackedTaskbars;
    auto found = std::find_if(trackedTaskbars.begin(), trackedTaskbars.end(),
                              [&](TrackedTaskbarState const& taskbar) {
                                  return taskbar.id == taskbarId;
                              });
    return found != trackedTaskbars.end() ? &*found : nullptr;
}

void ClearAnimationElementCache(TrackedTaskbarState& taskbar) {
    taskbar.animationOverlayCanvas = {};
    taskbar.animationDividers.clear();
}

void InvalidateReconciliationSignature(TrackedTaskbarState& taskbar) {
    taskbar.nativeSettlingActive = false;
    taskbar.nativeSettlingStableFrames = 0;
    taskbar.nativeSettlingStarted = {};
    taskbar.reconciliationSignatureValid = false;
    taskbar.cachedReconcileResult = ReconcileResult::temporarilyNotReady;
    taskbar.reconciledRepeater = {};
    taskbar.reconciledRootGrid = {};
    taskbar.reconciledButtons.clear();
    taskbar.reconciledOverlayExpected = false;
    taskbar.reconciledOverlayChildCount = 0;
    taskbar.reconciledAnimationCompatibility = false;
    taskbar.reconciledSeparators.clear();
}

void UnsubscribeAnimationRendering(TrackedTaskbarState& taskbar) {
    if (!taskbar.animationRenderingSubscribed) {
        return;
    }

    try {
        Media::CompositionTarget::Rendering(taskbar.animationRenderingToken);
    } catch (...) {
    }
    taskbar.animationRenderingToken = {};
    taskbar.animationRenderingSubscribed = false;
    for (auto& cache : taskbar.animationDividers) {
        cache.hasLastPosition = false;
    }
    taskbar.animationStableFrames = 0;
    taskbar.nativeSettlingStableFrames = 0;
}

void StopAnimationTracking(TrackedTaskbarState& taskbar) {
    taskbar.animationLastActivity = {};
    taskbar.animationStableFrames = 0;
    if (!taskbar.nativeSettlingActive) {
        UnsubscribeAnimationRendering(taskbar);
    }
}

void StopNativeSettlingTracking(TrackedTaskbarState& taskbar) {
    taskbar.nativeSettlingActive = false;
    taskbar.nativeSettlingStableFrames = 0;
    taskbar.nativeSettlingStarted = {};
    if (taskbar.animationLastActivity == AnimationClock::time_point{}) {
        UnsubscribeAnimationRendering(taskbar);
    }
}

void StopAllGeometryTracking(TrackedTaskbarState& taskbar) {
    taskbar.animationLastActivity = {};
    taskbar.animationStableFrames = 0;
    taskbar.nativeSettlingActive = false;
    taskbar.nativeSettlingStableFrames = 0;
    taskbar.nativeSettlingStarted = {};
    UnsubscribeAnimationRendering(taskbar);
}

bool RefreshCachedDividerGeometry(TrackedTaskbarState& taskbar,
                                  bool animatedCrossAxis,
                                  bool* allStable) {
    auto overlayCanvas = taskbar.animationOverlayCanvas.get();
    if (!overlayCanvas || taskbar.animationDividers.empty()) {
        return false;
    }

    bool stable = true;
    for (auto& cache : taskbar.animationDividers) {
        auto host = cache.host.get();
        auto previousIcon = cache.previousIcon.get();
        auto targetIcon = cache.targetIcon.get();
        auto nextIcon = cache.nextIcon.get();
        auto previousButton = cache.previousButton.get();
        auto targetButton = cache.targetButton.get();
        auto nextButton = cache.nextButton.get();
        bool requiredElementsAvailable =
            cache.geometryMode == DividerGeometryMode::buttonBoundaries
                ? targetButton &&
                      (nextButton || (!cache.beforeFirst && previousButton))
                : targetIcon &&
                      (nextIcon || (!cache.beforeFirst && previousIcon));
        if (!host || !requiredElementsAvailable) {
            return false;
        }

        DividerGeometry geometry;
        bool geometryValid = false;
        if (cache.geometryMode == DividerGeometryMode::buttonBoundaries) {
            geometryValid = TryGetButtonBoundaryDividerGeometry(
                overlayCanvas, previousButton, targetButton, nextButton,
                cache.beforeFirst, animatedCrossAxis, cache.orientation,
                taskbar.reconciledRootWidth, taskbar.reconciledRootHeight,
                host.Width(), host.Height(), &geometry);
        } else {
            geometryValid =
                cache.beforeFirst ? TryGetBeforeFirstDividerGeometry(
                                        overlayCanvas, targetIcon, nextIcon,
                                        cache.orientation, host.Width(),
                                        host.Height(), &geometry)
                : animatedCrossAxis
                    ? TryGetAnimatedDividerGeometry(
                          overlayCanvas, previousIcon, targetIcon, nextIcon,
                          cache.orientation, taskbar.reconciledRootWidth,
                          taskbar.reconciledRootHeight, host.Width(),
                          host.Height(), &geometry)
                    : TryGetDividerGeometry(
                          overlayCanvas, previousIcon, targetIcon, nextIcon,
                          cache.orientation, taskbar.reconciledRootWidth,
                          taskbar.reconciledRootHeight, host.Width(),
                          host.Height(), &geometry);
        }
        if (!geometryValid) {
            return false;
        }

        ApplyDividerPrimaryOffset(overlayCanvas, cache.orientation,
                                  cache.primaryOffset, &geometry);

        constexpr double kWriteEpsilon = 0.01;
        double currentLeft = Controls::Canvas::GetLeft(host);
        double currentTop = Controls::Canvas::GetTop(host);
        if (!std::isfinite(currentLeft) ||
            std::fabs(currentLeft - geometry.left) > kWriteEpsilon) {
            Controls::Canvas::SetLeft(host, geometry.left);
        }
        if (!std::isfinite(currentTop) ||
            std::fabs(currentTop - geometry.top) > kWriteEpsilon) {
            Controls::Canvas::SetTop(host, geometry.top);
        }

        if (!cache.hasLastPosition ||
            std::fabs(cache.lastLeft - geometry.left) > kWriteEpsilon ||
            std::fabs(cache.lastTop - geometry.top) > kWriteEpsilon) {
            stable = false;
        }
        cache.lastLeft = geometry.left;
        cache.lastTop = geometry.top;
        cache.hasLastPosition = true;
    }

    if (allStable) {
        *allStable = stable;
    }
    return true;
}

void OnAnimationRendering(size_t taskbarId,
                          winrt::Windows::Foundation::IInspectable const&,
                          winrt::Windows::Foundation::IInspectable const&) {
    auto* taskbarState = FindTrackedTaskbarById(taskbarId);
    if (!taskbarState) {
        return;
    }
    auto& taskbar = *taskbarState;

    if (g_unloading) {
        StopAllGeometryTracking(taskbar);
        return;
    }

    // The MUX drag may swallow PointerReleased; treat a
    // released left button as the end of the drag so the final order is
    // always reconciled.
    if (taskbar.reorderDragActive && !(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
        taskbar.reorderDragActive = false;
        taskbar.hasFrozenDynamicGap = false;
        taskbar.draggedButton = {};
        taskbar.draggedButtonIndex = -1;
        taskbar.postReleaseReconcileFrames = 2;
        taskbar.reorderStructuralReconcilePending = true;
    }

    if (taskbar.reorderStructuralReconcilePending) {
        taskbar.reorderStructuralReconcilePending = false;
        try {
            auto repeater = taskbar.repeater.get();
            if (repeater) {
                // PointerReleased is observed before Windows necessarily
                // finishes its drag/reorder layout. Running on the following
                // composition frame gives the repeater a chance to expose its
                // final order before we reassign gaps and divider geometry.
                // Don't keep a TrackedTaskbarState reference across this call:
                // reconciliation can prune/reallocate the tracked-taskbar list.
                // Always force a full reconciliation on release so the
                // temporary drag margins are restored even when the button
                // order is unchanged.
                // While forced frames remain, re-arm the reconcile for
                // the next frame so the gap follows the final order.
                if (taskbar.postReleaseReconcileFrames > 0) {
                    taskbar.postReleaseReconcileFrames--;
                    if (taskbar.postReleaseReconcileFrames > 0) {
                        taskbar.reorderStructuralReconcilePending = true;
                    }
                }
                ReconcileTaskbarRepeater(repeater, true);
            }
        } catch (...) {
        }

        auto* refreshed = FindTrackedTaskbarById(taskbarId);
        if (refreshed && !refreshed->nativeSettlingActive &&
            refreshed->animationLastActivity == AnimationClock::time_point{}) {
            StopAllGeometryTracking(*refreshed);
        }
        return;
    }

    auto now = AnimationClock::now();
    if (taskbar.animationLastActivity != AnimationClock::time_point{} &&
        now - taskbar.animationLastActivity >= kAnimationTrackingTimeout) {
        StopAnimationTracking(taskbar);
    }
    if (taskbar.nativeSettlingActive &&
        now - taskbar.nativeSettlingStarted >= kNativeSettlingTimeout) {
        StopNativeSettlingTracking(taskbar);
    }
    if (!taskbar.animationRenderingSubscribed) {
        return;
    }

    if (taskbar.animationRenderingCallbackActive) {
        return;
    }

    taskbar.animationRenderingCallbackActive = true;
    struct CallbackGuard {
        TrackedTaskbarState* taskbar;
        ~CallbackGuard() { taskbar->animationRenderingCallbackActive = false; }
    } callbackGuard{&taskbar};

    try {
        bool allStable = false;
        if (taskbar.reorderDragActive && taskbar.hasFrozenDynamicGap) {
            // Freeze divider geometry while a reorder drag is in flight.
            allStable = true;
        } else if (!RefreshCachedDividerGeometry(taskbar, true, &allStable)) {
            Wh_Log(L"Divider geometry cache is invalid");
            ClearAnimationElementCache(taskbar);
            StopAllGeometryTracking(taskbar);
            return;
        }

        if (taskbar.nativeSettlingActive) {
            taskbar.nativeSettlingStableFrames =
                allStable ? taskbar.nativeSettlingStableFrames + 1 : 0;
            if (taskbar.nativeSettlingStableFrames >=
                kGeometryStableFrameThreshold) {
                StopNativeSettlingTracking(taskbar);
            }
        }

        if (taskbar.animationLastActivity != AnimationClock::time_point{} &&
            taskbar.animationPointerInside) {
            taskbar.animationStableFrames = 0;
        } else if (taskbar.animationLastActivity !=
                   AnimationClock::time_point{}) {
            taskbar.animationStableFrames =
                allStable ? taskbar.animationStableFrames + 1 : 0;
            if (taskbar.animationStableFrames >=
                kGeometryStableFrameThreshold) {
                StopAnimationTracking(taskbar);
            }
        }
    } catch (...) {
        Wh_Log(L"Divider geometry cache is invalid");
        ClearAnimationElementCache(taskbar);
        StopAllGeometryTracking(taskbar);
    }
}

void EnsureGeometryRenderingSubscribed(TrackedTaskbarState& taskbar) {
    if (taskbar.animationRenderingSubscribed) {
        return;
    }

    size_t taskbarId = taskbar.id;
    taskbar.animationRenderingToken = Media::CompositionTarget::Rendering(
        [taskbarId](winrt::Windows::Foundation::IInspectable const& sender,
                    winrt::Windows::Foundation::IInspectable const& args) {
            OnAnimationRendering(taskbarId, sender, args);
        });
    taskbar.animationRenderingSubscribed = true;
}

void StartNativeSettlingTracking(TrackedTaskbarState& taskbar) {
    if (g_unloading || !taskbar.animationOverlayCanvas.get() ||
        taskbar.animationDividers.empty()) {
        return;
    }

    taskbar.nativeSettlingActive = true;
    taskbar.nativeSettlingStarted = AnimationClock::now();
    taskbar.nativeSettlingStableFrames = 0;
    for (auto& cache : taskbar.animationDividers) {
        cache.hasLastPosition = false;
    }
    EnsureGeometryRenderingSubscribed(taskbar);
}

void StartAnimationTracking(TrackedTaskbarState& taskbar) {
    if (g_unloading || !taskbar.animationOverlayCanvas.get() ||
        taskbar.animationDividers.empty()) {
        return;
    }

    taskbar.animationLastActivity = AnimationClock::now();

    for (auto& cache : taskbar.animationDividers) {
        cache.hasLastPosition = false;
    }
    taskbar.animationStableFrames = 0;
    EnsureGeometryRenderingSubscribed(taskbar);
}

void OnAnimationPointerMoved(size_t taskbarId,
                             winrt::Windows::Foundation::IInspectable const&,
                             Input::PointerRoutedEventArgs const&) {
    auto* taskbarState = FindTrackedTaskbarById(taskbarId);
    if (!taskbarState) {
        return;
    }
    auto& taskbar = *taskbarState;

    if (g_unloading) {
        return;
    }

    taskbar.animationPointerInside = true;
    taskbar.animationStableFrames = 0;
    StartAnimationTracking(taskbar);
}

void OnAnimationPointerExited(size_t taskbarId,
                              winrt::Windows::Foundation::IInspectable const&,
                              Input::PointerRoutedEventArgs const&) {
    auto* taskbarState = FindTrackedTaskbarById(taskbarId);
    if (!taskbarState) {
        return;
    }
    auto& taskbar = *taskbarState;

    if (g_unloading) {
        return;
    }

    taskbar.animationPointerInside = false;
    taskbar.animationStableFrames = 0;
    taskbar.animationLastActivity = AnimationClock::now();
}

void OnReorderPointerPressed(
    size_t taskbarId,
    winrt::Windows::Foundation::IInspectable const&,
    Input::PointerRoutedEventArgs const& e) {
    auto* taskbarState = FindTrackedTaskbarById(taskbarId);
    if (!taskbarState || g_unloading.load(std::memory_order_acquire)) {
        return;
    }

    // Freeze the dynamic gap when a mouse button goes down on the taskbar.
    // This is intentionally simple: any press freezes until release, which
    // avoids mid-drag layout feedback loops. A normal click is short enough
    // that the deferred reconcile on release is not noticeable.
    if (!taskbarState->reorderDragActive) {
        taskbarState->reorderDragActive = true;
        taskbarState->hasFrozenDynamicGap = true;
        taskbarState->frozenDynamicGap =
            taskbarState->lastAppliedDynamicGap;
        taskbarState->frozenDynamicSeparatorVisible =
            taskbarState->lastDynamicSeparatorVisible;

        // Remember which TaskListButton was pressed so the dynamic
        // gap can be kept on the correct side while dragging.
        auto source = e.OriginalSource().try_as<FrameworkElement>();
        auto pressedButton = FindAncestorByName(source, L"TaskListButton");
        taskbarState->draggedButton =
            pressedButton ? winrt::make_weak(pressedButton)
                          : winrt::weak_ref<FrameworkElement>{};
        taskbarState->draggedButtonIndex = -1;
        if (pressedButton) {
            if (auto repeater = taskbarState->repeater.get()) {
                taskbarState->draggedButtonIndex =
                    ItemsRepeater_GetElementIndex(repeater, pressedButton);
            }
        }

        // The press-time forced reconcile is exempt from the
        // drag freeze (see ReconcileTrackedTaskbar), so it applies the
        // frozen gap fully onto the boundary button opposite the pressed
        // one; that shape stays frozen for the rest of the drag and is
        // recomputed on release.
        if (auto repeater = taskbarState->repeater.get()) {
            ReconcileTaskbarRepeater(repeater, true);
        }
    }
}

void OnReorderPointerReleased(size_t taskbarId,
                              winrt::Windows::Foundation::IInspectable const&,
                              Input::PointerRoutedEventArgs const&) {
    auto* taskbarState = FindTrackedTaskbarById(taskbarId);
    if (!taskbarState || g_unloading.load(std::memory_order_acquire)) {
        return;
    }

    taskbarState->reorderDragActive = false;
    taskbarState->draggedButton = {};
    taskbarState->draggedButtonIndex = -1;
    taskbarState->reorderStructuralReconcilePending = true;
    // Force a couple of post-release reconciliation frames so
    // the gap is recomputed against the final button order.
    taskbarState->postReleaseReconcileFrames = 2;
    EnsureGeometryRenderingSubscribed(*taskbarState);
}

void DetachReorderPointerHandler(TrackedTaskbarState& taskbar) {
    if (taskbar.reorderPointerHandlerAttached) {
        if (auto source = taskbar.reorderPointerSource.get()) {
            if (taskbar.reorderPointerPressedHandler) {
                try {
                    source.RemoveHandler(UIElement::PointerPressedEvent(),
                                         taskbar.reorderPointerPressedHandler);
                } catch (...) {
                    // The root can disappear while Explorer rebuilds.
                }
            }
            if (taskbar.reorderPointerReleasedHandler) {
                try {
                    source.RemoveHandler(UIElement::PointerReleasedEvent(),
                                         taskbar.reorderPointerReleasedHandler);
                } catch (...) {
                    // The root can disappear while Explorer rebuilds.
                }
            }
        }
    }

    taskbar.reorderPointerPressedHandler = nullptr;
    taskbar.reorderPointerReleasedHandler = nullptr;
    taskbar.reorderPointerSource = {};
    taskbar.reorderPointerHandlerAttached = false;
    taskbar.reorderStructuralReconcilePending = false;
    // Reset the post-release reconcile frame counter.
    taskbar.postReleaseReconcileFrames = 0;
    taskbar.reorderDragActive = false;
    taskbar.draggedButton = {};
    taskbar.draggedButtonIndex = -1;
    taskbar.hasFrozenDynamicGap = false;
}

void AttachReorderPointerHandler(TrackedTaskbarState& taskbar,
                                 Controls::Grid const& source) {
    if (!source || g_unloading.load(std::memory_order_acquire)) {
        return;
    }

    if (taskbar.reorderPointerHandlerAttached) {
        auto currentSource = taskbar.reorderPointerSource.get();
        if (currentSource &&
            winrt::get_abi(currentSource) == winrt::get_abi(source)) {
            return;
        }
        DetachReorderPointerHandler(taskbar);
    }

    try {
        taskbar.reorderPointerSource = winrt::make_weak(source);
        size_t taskbarId = taskbar.id;
        taskbar.reorderPointerPressedHandler =
            winrt::box_value(Input::PointerEventHandler{
                [taskbarId](
                    winrt::Windows::Foundation::IInspectable const& sender,
                    Input::PointerRoutedEventArgs const& args) {
                    OnReorderPointerPressed(taskbarId, sender, args);
                }});
        taskbar.reorderPointerReleasedHandler =
            winrt::box_value(Input::PointerEventHandler{
                [taskbarId](
                    winrt::Windows::Foundation::IInspectable const& sender,
                    Input::PointerRoutedEventArgs const& args) {
                    OnReorderPointerReleased(taskbarId, sender, args);
                }});
        source.AddHandler(UIElement::PointerPressedEvent(),
                          taskbar.reorderPointerPressedHandler, true);
        source.AddHandler(UIElement::PointerReleasedEvent(),
                          taskbar.reorderPointerReleasedHandler, true);
        taskbar.reorderPointerHandlerAttached = true;
    } catch (...) {
        DetachReorderPointerHandler(taskbar);
    }
}

void DetachAnimationPointerHandlers(TrackedTaskbarState& taskbar) {
    StopAnimationTracking(taskbar);

    if (taskbar.animationPointerHandlersAttached) {
        if (auto source = taskbar.animationPointerSource.get()) {
            if (taskbar.animationPointerMovedHandler) {
                try {
                    source.RemoveHandler(UIElement::PointerMovedEvent(),
                                         taskbar.animationPointerMovedHandler);
                } catch (...) {
                    // The source can be disconnected while Explorer rebuilds
                    // the taskbar. Clearing our delegate is sufficient then.
                }
            }
            if (taskbar.animationPointerExitedHandlerAttached) {
                try {
                    source.PointerExited(taskbar.animationPointerExitedToken);
                } catch (...) {
                    // The source can be disconnected while Explorer rebuilds
                    // the taskbar. Clearing our token is sufficient then.
                }
            }
        }
    }

    taskbar.animationPointerMovedHandler = nullptr;
    taskbar.animationPointerExitedToken = {};
    taskbar.animationPointerSource = {};
    taskbar.animationPointerHandlersAttached = false;
    taskbar.animationPointerExitedHandlerAttached = false;
    taskbar.animationPointerInside = false;
    taskbar.animationLastActivity = {};
}

void AttachAnimationPointerHandlers(TrackedTaskbarState& taskbar,
                                    Controls::Grid const& source) {
    if (!source || g_unloading) {
        return;
    }

    if (taskbar.animationPointerHandlersAttached) {
        auto currentSource = taskbar.animationPointerSource.get();
        if (currentSource &&
            winrt::get_abi(currentSource) == winrt::get_abi(source)) {
            return;
        }
        DetachAnimationPointerHandlers(taskbar);
    }

    try {
        taskbar.animationPointerSource = winrt::make_weak(source);
        size_t taskbarId = taskbar.id;
        taskbar.animationPointerMovedHandler =
            winrt::box_value(Input::PointerEventHandler{
                [taskbarId](
                    winrt::Windows::Foundation::IInspectable const& sender,
                    Input::PointerRoutedEventArgs const& args) {
                    OnAnimationPointerMoved(taskbarId, sender, args);
                }});
        source.AddHandler(UIElement::PointerMovedEvent(),
                          taskbar.animationPointerMovedHandler, true);
        taskbar.animationPointerHandlersAttached = true;
        taskbar.animationPointerExitedToken =
            source.PointerExited(Input::PointerEventHandler{
                [taskbarId](
                    winrt::Windows::Foundation::IInspectable const& sender,
                    Input::PointerRoutedEventArgs const& args) {
                    OnAnimationPointerExited(taskbarId, sender, args);
                }});
        taskbar.animationPointerExitedHandlerAttached = true;
    } catch (...) {
        DetachAnimationPointerHandlers(taskbar);
    }
}

// At most this many forced reconciles per observed realized-button count: the
// forced reconcile writes margins, which schedules another layout pass and can
// raise LayoutUpdated again while the count still does not match (for example
// while a removed button is still animating), so the handler is bounded instead
// of spinning.
constexpr int kMaxLayoutForcedReconciles = 3;

// Layout-change monitor for the taskbar repeater panel.
// Closing an app removes its TaskListButton out from under the split half-gap
// margins: the removed button's halfGap disappears (gap halves) and, because
// a removed button never raises UpdateVisualStates again, no reconcile fires
// to restore it. LayoutUpdated fires on the repeater panel after any real
// relayout, so we recount the realized buttons there and force a structural
// reconcile whenever the count no longer matches the last committed set.

void OnTaskbarLayoutUpdated(
    size_t taskbarId,
    winrt::Windows::Foundation::IInspectable const&,
    winrt::Windows::Foundation::IInspectable const&) {
    auto* taskbarState = FindTrackedTaskbarById(taskbarId);
    if (!taskbarState || g_unloading.load(std::memory_order_acquire)) {
        return;
    }

    // Our own forced reconcile writes margins, which raises LayoutUpdated again
    // while it is still running; that pass must not queue another forced
    // reconcile.
    if (taskbarState->layoutForcedReconcileActive) {
        return;
    }

    // While a reorder drag is in flight the realized button set is
    // intentionally transient; the press/release handlers own that lifecycle.
    if (taskbarState->reorderDragActive) {
        return;
    }

    auto repeater = taskbarState->repeater.get();
    if (!repeater) {
        return;
    }

    auto panel = repeater.try_as<Controls::Panel>();
    if (!panel) {
        return;
    }

    // Count the realized, usable TaskListButton children exactly the way
    // CaptureTaskbarReconciliationSnapshot does, so a match means the set the
    // last reconcile committed is still the set that is laid out right now.
    size_t realizedButtonCount = 0;
    for (auto const& panelChild : panel.Children()) {
        auto child = panelChild.try_as<FrameworkElement>();
        if (!child) {
            continue;
        }
        if (child.Name() != L"TaskListButton") {
            continue;
        }
        if (ItemsRepeater_GetElementIndex(repeater, child) < 0) {
            continue;
        }
        if (!IsUsableApplicationButton(child)) {
            continue;
        }
        realizedButtonCount++;
    }

    // Only a count differing from the last observed one is a new situation,
    // which makes the retries below worth resetting.
    if (!taskbarState->layoutObservedButtonCountValid ||
        taskbarState->layoutObservedButtonCount != realizedButtonCount) {
        taskbarState->layoutObservedButtonCount = realizedButtonCount;
        taskbarState->layoutObservedButtonCountValid = true;
        taskbarState->layoutForcedReconcileAttempts = 0;
    }

    if (realizedButtonCount == taskbarState->reconciledButtons.size()) {
        return;
    }

    // A removed or added button left the split margins stale; force a
    // structural reconcile to rebuild them. The attempts are bounded per
    // observed count, and a LayoutUpdated raised by our own forced reconcile is
    // ignored at the top of this handler.
    if (taskbarState->layoutForcedReconcileAttempts >=
        kMaxLayoutForcedReconciles) {
        return;
    }

    taskbarState->layoutForcedReconcileAttempts++;
    taskbarState->layoutForcedReconcileActive = true;
    try {
        ReconcileTaskbarRepeater(repeater, true);
    } catch (...) {
    }

    // Reconciliation prunes the tracked-taskbar list and can reallocate it, so
    // the pointer taken at the top of this handler may dangle here. Resolve the
    // state again before writing the flag.
    if (auto* refreshed = FindTrackedTaskbarById(taskbarId)) {
        refreshed->layoutForcedReconcileActive = false;
    }
}

void DetachLayoutChangeMonitor(TrackedTaskbarState& taskbar) {
    if (taskbar.layoutMonitorAttached) {
        if (auto panel = taskbar.layoutMonitorPanel.get()) {
            try {
                panel.LayoutUpdated(taskbar.layoutUpdatedToken);
            } catch (...) {
                // The panel can be disconnected while Explorer rebuilds the
                // taskbar. Clearing our token is sufficient then.
            }
        }
    }

    taskbar.layoutMonitorPanel = {};
    taskbar.layoutUpdatedToken = {};
    taskbar.layoutMonitorAttached = false;
}

void AttachLayoutChangeMonitor(TrackedTaskbarState& taskbar,
                               Controls::Panel const& panel) {
    if (!panel || g_unloading) {
        return;
    }

    if (taskbar.layoutMonitorAttached) {
        auto currentPanel = taskbar.layoutMonitorPanel.get();
        if (currentPanel &&
            winrt::get_abi(currentPanel) == winrt::get_abi(panel)) {
            return;
        }
        DetachLayoutChangeMonitor(taskbar);
    }

    try {
        taskbar.layoutMonitorPanel = winrt::make_weak(panel);
        size_t taskbarId = taskbar.id;
        taskbar.layoutUpdatedToken =
            panel.LayoutUpdated(winrt::Windows::Foundation::EventHandler<
                                winrt::Windows::Foundation::IInspectable>{
                [taskbarId](
                    winrt::Windows::Foundation::IInspectable const& sender,
                    winrt::Windows::Foundation::IInspectable const& args) {
                    OnTaskbarLayoutUpdated(taskbarId, sender, args);
                }});
        taskbar.layoutMonitorAttached = true;
    } catch (...) {
        DetachLayoutChangeMonitor(taskbar);
    }
}

void ClearAnimationTrackingForUnload() {
    if (!g_trackedTaskbars) {
        return;
    }

    for (auto& taskbar : *g_trackedTaskbars) {
        StopAllGeometryTracking(taskbar);
        try {
            DetachAnimationPointerHandlers(taskbar);
        } catch (...) {
        }
        try {
            DetachReorderPointerHandler(taskbar);
        } catch (...) {
        }
        try {
            DetachLayoutChangeMonitor(taskbar);
        } catch (...) {
        }
        ClearAnimationElementCache(taskbar);
    }
}

// Element names are suffixed with the mod id. The base name deliberately shares
// no prefix with the original Taskbar Separators mod: that mod matches and
// removes its own elements by the bare prefix L"WindhawkTaskbarSeparator_", so
// a shared prefix would let it delete our dividers. Both mods can be installed
// side by side, and neither may claim or remove the other's elements.
constexpr WCHAR kSeparatorElementPrefix[] = L"WhDynamicCenteringSeparator";

std::wstring GetSeparatorOverlayName() {
    return std::wstring(kSeparatorElementPrefix) + L"Overlay_" WH_MOD_ID;
}

std::wstring GetDividerName(size_t settingsIndex) {
    return std::wstring(kSeparatorElementPrefix) + L"_" WH_MOD_ID L"_" +
           std::to_wstring(settingsIndex);
}

std::wstring GetBeforeFirstDividerName() {
    return std::wstring(kSeparatorElementPrefix) + L"_" WH_MOD_ID L"_" +
           L"BeforeFirst";
}

bool IsOwnedDividerName(std::wstring_view name) {
    static const std::wstring prefix =
        std::wstring(kSeparatorElementPrefix) + L"_" WH_MOD_ID L"_";
    return name.size() >= prefix.size() &&
           name.compare(0, prefix.size(), prefix) == 0;
}

bool IsOwnedOverlayName(std::wstring_view name) {
    static const std::wstring overlayName = GetSeparatorOverlayName();
    return name == overlayName;
}

void SweepOwnedElementsFromPanel(Controls::Panel const& panel, int depth = 0) {
    if (!panel || depth > 24) {
        return;
    }

    try {
        auto children = panel.Children();
        for (uint32_t childIndex = 0; childIndex < children.Size();) {
            FrameworkElement child = nullptr;
            try {
                child = children.GetAt(childIndex).try_as<FrameworkElement>();
            } catch (...) {
                childIndex++;
                continue;
            }

            if (!child) {
                childIndex++;
                continue;
            }

            bool ownedOverlay = false;
            bool ownedDivider = false;
            try {
                std::wstring childName{child.Name()};
                ownedOverlay = IsOwnedOverlayName(childName);
                ownedDivider = IsOwnedDividerName(childName);
            } catch (...) {
                childIndex++;
                continue;
            }

            if (ownedOverlay || ownedDivider) {
                if (auto childPanel = child.try_as<Controls::Panel>()) {
                    // Release any owned divider hosts inside an overlay before
                    // detaching the overlay itself.
                    SweepOwnedElementsFromPanel(childPanel, depth + 1);
                }

                try {
                    children.RemoveAt(childIndex);
                    continue;
                } catch (...) {
                    childIndex++;
                    continue;
                }
            }

            if (auto childPanel = child.try_as<Controls::Panel>()) {
                SweepOwnedElementsFromPanel(childPanel, depth + 1);
            }
            childIndex++;
        }
    } catch (...) {
    }
}

Controls::Grid ResolveTrackedTaskbarRoot(TrackedTaskbarState& taskbar) {
    try {
        if (auto rootGrid = taskbar.rootGrid.get()) {
            return rootGrid;
        }
    } catch (...) {
    }

    try {
        if (auto repeater = taskbar.repeater.get()) {
            if (auto rootGrid = FindRootGridAncestor(repeater)) {
                return rootGrid;
            }
        }
    } catch (...) {
    }

    try {
        if (auto overlayCanvas = taskbar.overlayCanvas.get()) {
            return Media::VisualTreeHelper::GetParent(overlayCanvas)
                .try_as<Controls::Grid>();
        }
    } catch (...) {
    }

    return nullptr;
}

void RemoveTrackedOverlayByExactName(TrackedTaskbarState& taskbar) {
    try {
        auto overlayCanvas = taskbar.overlayCanvas.get();
        if (!overlayCanvas ||
            !IsOwnedOverlayName(overlayCanvas.Name())) {
            return;
        }

        auto overlayParent = Media::VisualTreeHelper::GetParent(overlayCanvas)
                                 .try_as<Controls::Panel>();
        if (!overlayParent) {
            return;
        }

        SweepOwnedElementsFromPanel(overlayCanvas);
        uint32_t overlayIndex = 0;
        auto parentChildren = overlayParent.Children();
        if (parentChildren.IndexOf(overlayCanvas, overlayIndex)) {
            parentChildren.RemoveAt(overlayIndex);
        }
    } catch (...) {
    }
}

void RestoreTrackedButtonMargin(TrackedButtonMarginState& tracked) {
    if (!tracked.hasAppliedMargin) {
        return;
    }

    try {
        auto button = tracked.button.get();
        if (!button) {
            return;
        }

        // Only undo our value if it's still the value currently on the
        // element. If Windows or another mod changed Margin afterwards, that
        // newer value belongs to them and must not be overwritten.
        Thickness current = button.Margin();
        if (ThicknessApproximatelyEqual(current, tracked.lastAppliedMargin)) {
            if (tracked.baseMarginWasLocal) {
                button.Margin(tracked.baseMargin);
            } else {
                button.ClearValue(FrameworkElement::MarginProperty());
            }
        }
    } catch (...) {
    }

    tracked.hasAppliedMargin = false;
}

void RestoreTrackedButtonMargins(TrackedTaskbarState& taskbar) {
    for (auto& tracked : taskbar.buttonMargins) {
        RestoreTrackedButtonMargin(tracked);
    }
    taskbar.buttonMargins.clear();
}

void CaptureBaseMargin(TrackedButtonMarginState& tracked,
                       FrameworkElement const& button,
                       Thickness const& margin) {
    tracked.baseMargin = margin;
    auto buttonDo = button.as<DependencyObject>();
    tracked.baseMarginWasLocal =
        buttonDo.ReadLocalValue(FrameworkElement::MarginProperty()) !=
        DependencyProperty::UnsetValue();
    tracked.lastAppliedMargin = margin;
    tracked.hasAppliedMargin = false;
}

void SynchronizeTrackedButtonMargins(
    TrackedTaskbarState& taskbar,
    std::vector<FrameworkElement> const& appButtons) {
    std::vector<TrackedButtonMarginState> synchronized;
    synchronized.reserve(appButtons.size());

    std::vector<bool> reused(taskbar.buttonMargins.size(), false);
    for (auto const& button : appButtons) {
        if (!button) {
            continue;
        }

        Thickness current = button.Margin();
        TrackedButtonMarginState tracked;
        bool found = false;
        for (size_t index = 0; index < taskbar.buttonMargins.size(); index++) {
            auto existingButton = taskbar.buttonMargins[index].button.get();
            if (!existingButton ||
                winrt::get_abi(existingButton) != winrt::get_abi(button)) {
                continue;
            }

            tracked = taskbar.buttonMargins[index];
            reused[index] = true;
            found = true;
            break;
        }

        if (!found) {
            tracked.button = winrt::make_weak(button);
            CaptureBaseMargin(tracked, button, current);
        } else {
            tracked.button = winrt::make_weak(button);

            if (tracked.hasAppliedMargin) {
                if (!ThicknessApproximatelyEqual(current,
                                                 tracked.lastAppliedMargin)) {
                    // Something else changed Margin after our last write.
                    // Treat that as the new base and layer our gap on top.
                    CaptureBaseMargin(tracked, button, current);
                }
            } else {
                CaptureBaseMargin(tracked, button, current);
            }
        }

        synchronized.push_back(std::move(tracked));
    }

    // A realized button can disappear or be recycled. Don't leave our margin
    // delta attached to an element that we no longer own.
    for (size_t index = 0; index < taskbar.buttonMargins.size(); index++) {
        if (!reused[index]) {
            RestoreTrackedButtonMargin(taskbar.buttonMargins[index]);
        }
    }

    taskbar.buttonMargins = std::move(synchronized);
}

void RemoveTrackedTaskbarElements(TrackedTaskbarState& taskbar) {
    RestoreTrackedButtonMargins(taskbar);
    InvalidateReconciliationSignature(taskbar);
    StopAllGeometryTracking(taskbar);
    try {
        DetachAnimationPointerHandlers(taskbar);
    } catch (...) {
    }
    try {
        DetachLayoutChangeMonitor(taskbar);
    } catch (...) {
    }
    try {
        DetachReorderPointerHandler(taskbar);
    } catch (...) {
    }
    ClearAnimationElementCache(taskbar);

    if (auto rootGrid = ResolveTrackedTaskbarRoot(taskbar)) {
        SweepOwnedElementsFromPanel(rootGrid);
    }
    RemoveTrackedOverlayByExactName(taskbar);

    taskbar.overlayCanvas = {};
    taskbar.rootGrid = {};
    // The cached tray frame belongs to the root grid that is being dropped.
    taskbar.trayFrame = {};
}

void PruneExpiredTrackedTaskbars() {
    if (!g_trackedTaskbars) {
        return;
    }

    auto& trackedTaskbars = *g_trackedTaskbars;
    for (size_t index = 0; index < trackedTaskbars.size();) {
        bool taskbarIsLive = false;
        try {
            auto repeater = trackedTaskbars[index].repeater.get();
            taskbarIsLive = repeater && FindRootGridAncestor(repeater);
        } catch (...) {
        }

        if (taskbarIsLive) {
            index++;
            continue;
        }

        RemoveTrackedTaskbarElements(trackedTaskbars[index]);
        trackedTaskbars.erase(trackedTaskbars.begin() + index);
    }
}

TrackedTaskbarState* TrackTaskbarRepeater(FrameworkElement const& repeater) {
    if (!repeater) {
        return nullptr;
    }

    PruneExpiredTrackedTaskbars();
    auto& trackedTaskbars = GetTrackedTaskbars();
    for (auto& taskbar : trackedTaskbars) {
        auto trackedRepeater = taskbar.repeater.get();
        if (trackedRepeater &&
            winrt::get_abi(trackedRepeater) == winrt::get_abi(repeater)) {
            return &taskbar;
        }
    }

    auto rootGrid = FindRootGridAncestor(repeater);
    if (rootGrid) {
        for (auto& taskbar : trackedTaskbars) {
            auto trackedRootGrid = taskbar.rootGrid.get();
            if (trackedRootGrid &&
                winrt::get_abi(trackedRootGrid) == winrt::get_abi(rootGrid)) {
                auto trackedRepeater = taskbar.repeater.get();
                if (!trackedRepeater || winrt::get_abi(trackedRepeater) !=
                                            winrt::get_abi(repeater)) {
                    InvalidateReconciliationSignature(taskbar);
                }
                taskbar.repeater = winrt::make_weak(repeater);
                return &taskbar;
            }
        }
    }

    TrackedTaskbarState taskbar;
    taskbar.id = g_nextTrackedTaskbarId++;
    taskbar.repeater = winrt::make_weak(repeater);
    if (rootGrid) {
        taskbar.rootGrid = winrt::make_weak(rootGrid);
    }
    trackedTaskbars.push_back(std::move(taskbar));
    return &trackedTaskbars.back();
}

size_t StyleChildCount(DividerStyle style) {
    if (style == DividerStyle::fade) {
        return 3;
    }
    return style == DividerStyle::glow || style == DividerStyle::doubleLine ? 2
                                                                            : 1;
}

PCWSTR ExpectedStyleChildName(DividerStyle style, uint32_t index) {
    if (style == DividerStyle::fade) {
        constexpr PCWSTR names[] = {L"DividerFadeOuter", L"DividerFadeMiddle",
                                    L"DividerFadeCore"};
        return names[index];
    }
    if (style == DividerStyle::glow) {
        return index == 0 ? L"DividerGlow" : L"DividerMain";
    }
    if (style == DividerStyle::doubleLine) {
        return index == 0 ? L"DividerLine1" : L"DividerLine2";
    }
    return L"DividerMain";
}

Shapes::Rectangle AppendStyleRectangle(Controls::Canvas const& host,
                                       PCWSTR name,
                                       int zIndex) {
    Shapes::Rectangle rectangle;
    rectangle.Name(name);
    rectangle.HorizontalAlignment(HorizontalAlignment::Left);
    rectangle.VerticalAlignment(VerticalAlignment::Top);
    rectangle.IsHitTestVisible(false);
    rectangle.UseLayoutRounding(true);
    Controls::Canvas::SetZIndex(rectangle, zIndex);
    host.Children().Append(rectangle);
    return rectangle;
}

Shapes::Ellipse AppendStyleEllipse(Controls::Canvas const& host,
                                   PCWSTR name,
                                   int zIndex) {
    Shapes::Ellipse ellipse;
    ellipse.Name(name);
    ellipse.HorizontalAlignment(HorizontalAlignment::Left);
    ellipse.VerticalAlignment(VerticalAlignment::Top);
    ellipse.IsHitTestVisible(false);
    ellipse.UseLayoutRounding(true);
    Controls::Canvas::SetZIndex(ellipse, zIndex);
    host.Children().Append(ellipse);
    return ellipse;
}

void RebuildStyleChildren(Controls::Canvas const& host, DividerStyle style) {
    host.Children().Clear();
    switch (style) {
        case DividerStyle::fade:
            AppendStyleRectangle(host, L"DividerFadeOuter", 0);
            AppendStyleRectangle(host, L"DividerFadeMiddle", 1);
            AppendStyleRectangle(host, L"DividerFadeCore", 2);
            break;
        case DividerStyle::glow:
            AppendStyleRectangle(host, L"DividerGlow", 0);
            AppendStyleRectangle(host, L"DividerMain", 1);
            break;
        case DividerStyle::doubleLine:
            AppendStyleRectangle(host, L"DividerLine1", 0);
            AppendStyleRectangle(host, L"DividerLine2", 1);
            break;
        case DividerStyle::dot:
        case DividerStyle::ring:
            AppendStyleEllipse(host, L"DividerMain", 0);
            break;
        default:
            AppendStyleRectangle(host, L"DividerMain", 0);
            break;
    }
    host.Tag(winrt::box_value(static_cast<int32_t>(style)));
}

bool StyleChildrenValid(Controls::Canvas const& host, DividerStyle style) {
    auto children = host.Children();
    if (children.Size() != StyleChildCount(style)) {
        return false;
    }

    for (uint32_t index = 0; index < children.Size(); index++) {
        auto child = children.GetAt(index).try_as<FrameworkElement>();
        if (!child || child.Name() != ExpectedStyleChildName(style, index)) {
            return false;
        }

        if (style == DividerStyle::dot || style == DividerStyle::ring) {
            if (!child.try_as<Shapes::Ellipse>()) {
                return false;
            }
        } else if (!child.try_as<Shapes::Rectangle>()) {
            return false;
        }
    }

    return winrt::unbox_value_or<int32_t>(host.Tag(), -1) ==
           static_cast<int32_t>(style);
}

void SetRectangleBounds(Shapes::Rectangle const& rectangle,
                        double left,
                        double top,
                        double width,
                        double height) {
    if (rectangle.IsHitTestVisible()) {
        rectangle.IsHitTestVisible(false);
    }
    if (!rectangle.UseLayoutRounding()) {
        rectangle.UseLayoutRounding(true);
    }
    if (rectangle.Width() != width) {
        rectangle.Width(width);
    }
    if (rectangle.Height() != height) {
        rectangle.Height(height);
    }
    if (Controls::Canvas::GetLeft(rectangle) != left) {
        Controls::Canvas::SetLeft(rectangle, left);
    }
    if (Controls::Canvas::GetTop(rectangle) != top) {
        Controls::Canvas::SetTop(rectangle, top);
    }
    if (rectangle.Opacity() != 1) {
        rectangle.Opacity(1);
    }
}

void SetRectangleRadius(Shapes::Rectangle const& rectangle, double radius) {
    if (rectangle.RadiusX() != radius) {
        rectangle.RadiusX(radius);
    }
    if (rectangle.RadiusY() != radius) {
        rectangle.RadiusY(radius);
    }
}

void SetEllipseBounds(Shapes::Ellipse const& ellipse,
                      double left,
                      double top,
                      double width,
                      double height) {
    if (ellipse.IsHitTestVisible()) {
        ellipse.IsHitTestVisible(false);
    }
    if (!ellipse.UseLayoutRounding()) {
        ellipse.UseLayoutRounding(true);
    }
    if (ellipse.Width() != width) {
        ellipse.Width(width);
    }
    if (ellipse.Height() != height) {
        ellipse.Height(height);
    }
    if (Controls::Canvas::GetLeft(ellipse) != left) {
        Controls::Canvas::SetLeft(ellipse, left);
    }
    if (Controls::Canvas::GetTop(ellipse) != top) {
        Controls::Canvas::SetTop(ellipse, top);
    }
    if (ellipse.Opacity() != 1) {
        ellipse.Opacity(1);
    }
}

void SetEllipseFill(Shapes::Ellipse const& ellipse,
                    winrt::Windows::UI::Color color) {
    auto brush = ellipse.Fill().try_as<Media::SolidColorBrush>();
    if (!brush) {
        brush = Media::SolidColorBrush();
        ellipse.Fill(brush);
    }
    if (!ColorsEqual(brush.Color(), color)) {
        brush.Color(color);
    }
}

void SetEllipseStroke(Shapes::Ellipse const& ellipse,
                      winrt::Windows::UI::Color color,
                      double thickness) {
    auto brush = ellipse.Stroke().try_as<Media::SolidColorBrush>();
    if (!brush) {
        brush = Media::SolidColorBrush();
        ellipse.Stroke(brush);
    }
    if (!ColorsEqual(brush.Color(), color)) {
        brush.Color(color);
    }
    if (ellipse.StrokeThickness() != thickness) {
        ellipse.StrokeThickness(thickness);
    }
}

void SetSolidFill(Shapes::Rectangle const& rectangle,
                  winrt::Windows::UI::Color color) {
    auto brush = rectangle.Fill().try_as<Media::SolidColorBrush>();
    if (!brush) {
        brush = Media::SolidColorBrush();
        rectangle.Fill(brush);
    }
    if (!ColorsEqual(brush.Color(), color)) {
        brush.Color(color);
    }
}

enum class FadeLayer {
    outer,
    middle,
    core,
};

void SetFadeFill(Shapes::Rectangle const& rectangle,
                 winrt::Windows::UI::Color color,
                 int fadeAmount,
                 TaskbarOrientation orientation,
                 FadeLayer layer) {
    auto brush = rectangle.Fill().try_as<Media::LinearGradientBrush>();
    if (!brush) {
        brush = Media::LinearGradientBrush();
        rectangle.Fill(brush);
    }

    if (orientation == TaskbarOrientation::horizontal) {
        brush.StartPoint({0.5f, 0.0f});
        brush.EndPoint({0.5f, 1.0f});
    } else {
        brush.StartPoint({0.0f, 0.5f});
        brush.EndPoint({1.0f, 0.5f});
    }

    auto stops = brush.GradientStops();
    constexpr uint32_t kFadeStopCount = 7;
    if (stops.Size() != kFadeStopCount) {
        stops.Clear();
        for (uint32_t index = 0; index < kFadeStopCount; index++) {
            Media::GradientStop stop;
            stops.Append(stop);
        }
    }

    double lowOpacityBaseOffset = 0.10;
    double highOpacityBaseOffset = 0.25;
    if (layer == FadeLayer::middle) {
        lowOpacityBaseOffset = 0.18;
        highOpacityBaseOffset = 0.36;
    } else if (layer == FadeLayer::outer) {
        lowOpacityBaseOffset = 0.28;
        highOpacityBaseOffset = 0.44;
    }

    // fadeAmount 70 uses each layer's default profile. Lower values compress
    // the fades toward the ends; higher values move them toward the midpoint.
    double softnessScale = std::clamp(fadeAmount / 70.0, 0.0, 100.0 / 70.0);
    double lowOpacityOffset =
        std::clamp(lowOpacityBaseOffset * softnessScale, 0.0, 0.5);
    double highOpacityOffset = std::clamp(highOpacityBaseOffset * softnessScale,
                                          lowOpacityOffset, 0.5);
    double mirroredHighOpacityOffset =
        std::clamp(1.0 - highOpacityOffset, 0.5, 1.0);
    double mirroredLowOpacityOffset =
        std::clamp(1.0 - lowOpacityOffset, mirroredHighOpacityOffset, 1.0);
    double offsets[kFadeStopCount] = {
        0.0, lowOpacityOffset,          highOpacityOffset,
        0.5, mirroredHighOpacityOffset, mirroredLowOpacityOffset,
        1.0,
    };
    double opacityFactors[kFadeStopCount] = {0.0,  0.45, 0.85, 1.0,
                                             0.85, 0.45, 0.0};

    for (uint32_t index = 0; index < kFadeStopCount; index++) {
        auto stopColor = color;
        stopColor.A =
            static_cast<uint8_t>(std::lround(color.A * opacityFactors[index]));
        auto stop = stops.GetAt(index);
        if (!ColorsEqual(stop.Color(), stopColor)) {
            stop.Color(stopColor);
        }
        if (stop.Offset() != offsets[index]) {
            stop.Offset(offsets[index]);
        }
    }
}

void GetStyleHostSize(Settings const& settings,
                      TaskbarOrientation orientation,
                      double* width,
                      double* height) {
    if (settings.style == DividerStyle::dot ||
        settings.style == DividerStyle::ring ||
        settings.style == DividerStyle::square ||
        settings.style == DividerStyle::diamond) {
        *width = settings.shapeSize;
        *height = settings.shapeSize;
        return;
    }

    double baseWidth = orientation == TaskbarOrientation::horizontal
                           ? settings.width
                           : settings.height;
    double baseHeight = orientation == TaskbarOrientation::horizontal
                            ? settings.height
                            : settings.width;
    *width = baseWidth;
    *height = baseHeight;

    if (settings.style == DividerStyle::fade) {
        double outerThickness = std::max<double>(settings.width + 4, 5);
        if (orientation == TaskbarOrientation::horizontal) {
            *width = outerThickness;
        } else {
            *height = outerThickness;
        }
    } else if (settings.style == DividerStyle::glow) {
        *width += settings.glowSize * 2.0;
        *height += settings.glowSize * 2.0;
    } else if (settings.style == DividerStyle::doubleLine) {
        double doubleThickness = settings.width * 2.0 + settings.doubleGap;
        if (orientation == TaskbarOrientation::horizontal) {
            *width = doubleThickness;
        } else {
            *height = doubleThickness;
        }
    }
}

void ConfigureStyleHost(Controls::Canvas const& host,
                        Settings const& settings,
                        TaskbarOrientation orientation,
                        double hostWidth,
                        double hostHeight) {
    if (!StyleChildrenValid(host, settings.style)) {
        RebuildStyleChildren(host, settings.style);
    }

    auto children = host.Children();

    if (settings.style == DividerStyle::dot ||
        settings.style == DividerStyle::ring) {
        auto ellipse = children.GetAt(0).as<Shapes::Ellipse>();
        SetEllipseBounds(ellipse, 0, 0, hostWidth, hostHeight);

        if (settings.style == DividerStyle::dot) {
            SetEllipseFill(ellipse, settings.color);
            ellipse.Stroke(nullptr);
            ellipse.StrokeThickness(0);
        } else {
            ellipse.Fill(nullptr);
            double ringThickness =
                std::min<double>(settings.width, settings.shapeSize / 2.0);
            SetEllipseStroke(ellipse, settings.color, ringThickness);
        }
        return;
    }

    if (settings.style == DividerStyle::square ||
        settings.style == DividerStyle::diamond) {
        auto main = children.GetAt(0).as<Shapes::Rectangle>();
        double side = settings.style == DividerStyle::diamond
                          ? settings.shapeSize / std::sqrt(2.0)
                          : settings.shapeSize;
        double left = (hostWidth - side) / 2.0;
        double top = (hostHeight - side) / 2.0;
        SetRectangleBounds(main, left, top, side, side);
        SetRectangleRadius(main, 0);
        SetSolidFill(main, settings.color);

        if (settings.style == DividerStyle::diamond) {
            auto rotate =
                main.RenderTransform().try_as<Media::RotateTransform>();
            if (!rotate) {
                rotate = Media::RotateTransform();
                main.RenderTransform(rotate);
            }
            rotate.CenterX(side / 2.0);
            rotate.CenterY(side / 2.0);
            rotate.Angle(45);
        } else if (main.RenderTransform()) {
            main.RenderTransform(nullptr);
        }
        return;
    }

    double baseWidth = orientation == TaskbarOrientation::horizontal
                           ? settings.width
                           : settings.height;
    double baseHeight = orientation == TaskbarOrientation::horizontal
                            ? settings.height
                            : settings.width;

    if (settings.style == DividerStyle::fade) {
        auto outer = children.GetAt(0).as<Shapes::Rectangle>();
        auto middle = children.GetAt(1).as<Shapes::Rectangle>();
        auto core = children.GetAt(2).as<Shapes::Rectangle>();
        double outerThickness = std::max<double>(settings.width + 4, 5);
        double middleThickness = std::max<double>(settings.width + 2, 3);

        if (orientation == TaskbarOrientation::horizontal) {
            SetRectangleBounds(outer, (hostWidth - outerThickness) / 2.0, 0,
                               outerThickness, baseHeight);
            SetRectangleBounds(middle, (hostWidth - middleThickness) / 2.0, 0,
                               middleThickness, baseHeight);
            SetRectangleBounds(core, (hostWidth - settings.width) / 2.0, 0,
                               settings.width, baseHeight);
        } else {
            SetRectangleBounds(outer, 0, (hostHeight - outerThickness) / 2.0,
                               baseWidth, outerThickness);
            SetRectangleBounds(middle, 0, (hostHeight - middleThickness) / 2.0,
                               baseWidth, middleThickness);
            SetRectangleBounds(core, 0, (hostHeight - settings.width) / 2.0,
                               baseWidth, settings.width);
        }

        SetRectangleRadius(outer, 0);
        SetRectangleRadius(middle, 0);
        SetRectangleRadius(core, 0);
        SetFadeFill(outer, settings.color, settings.fadeAmount, orientation,
                    FadeLayer::outer);
        SetFadeFill(middle, settings.color, settings.fadeAmount, orientation,
                    FadeLayer::middle);
        SetFadeFill(core, settings.color, settings.fadeAmount, orientation,
                    FadeLayer::core);
        outer.Opacity(0.12);
        middle.Opacity(0.30);
        core.Opacity(1.0);
        return;
    }

    if (settings.style == DividerStyle::glow) {
        auto glow = children.GetAt(0).as<Shapes::Rectangle>();
        auto main = children.GetAt(1).as<Shapes::Rectangle>();
        SetRectangleBounds(glow, 0, 0, hostWidth, hostHeight);
        SetRectangleRadius(glow, 0);
        SetSolidFill(glow, settings.color);
        glow.Opacity(settings.glowOpacityPercent / 100.0);
        SetRectangleBounds(main, settings.glowSize, settings.glowSize,
                           baseWidth, baseHeight);
        SetRectangleRadius(main, 0);
        SetSolidFill(main, settings.color);
        return;
    }

    if (settings.style == DividerStyle::doubleLine) {
        auto first = children.GetAt(0).as<Shapes::Rectangle>();
        auto second = children.GetAt(1).as<Shapes::Rectangle>();
        double secondLeft = orientation == TaskbarOrientation::horizontal
                                ? settings.width + settings.doubleGap
                                : 0;
        double secondTop = orientation == TaskbarOrientation::vertical
                               ? settings.width + settings.doubleGap
                               : 0;
        SetRectangleBounds(first, 0, 0, baseWidth, baseHeight);
        SetRectangleBounds(second, secondLeft, secondTop, baseWidth,
                           baseHeight);
        SetRectangleRadius(first, 0);
        SetRectangleRadius(second, 0);
        SetSolidFill(first, settings.color);
        SetSolidFill(second, settings.color);
        first.Opacity(1.0);
        second.Opacity(1.0);
        return;
    }

    auto main = children.GetAt(0).as<Shapes::Rectangle>();
    SetRectangleBounds(main, 0, 0, baseWidth, baseHeight);
    double radius =
        settings.style == DividerStyle::rounded
            ? std::min<double>(settings.cornerRadius, settings.width / 2.0)
            : 0;
    SetRectangleRadius(main, radius);
    SetSolidFill(main, settings.color);
}

bool TryGetTaskbarOrientation(OrientationSetting orientationSetting,
                              Controls::Canvas const& overlayCanvas,
                              double taskbarWidth,
                              double taskbarHeight,
                              std::vector<FrameworkElement> const& icons,
                              TaskbarOrientation* orientation) {
    if (!orientation) {
        return false;
    }

    if (orientationSetting == OrientationSetting::horizontal) {
        *orientation = TaskbarOrientation::horizontal;
        return true;
    }
    if (orientationSetting == OrientationSetting::vertical) {
        *orientation = TaskbarOrientation::vertical;
        return true;
    }

    double totalHorizontalMovement = 0;
    double totalVerticalMovement = 0;
    bool foundPair = false;
    for (size_t index = 1; index < icons.size(); index++) {
        winrt::Windows::Foundation::Rect previousBounds{};
        winrt::Windows::Foundation::Rect currentBounds{};
        if (!icons[index - 1] || !icons[index] ||
            !TryGetElementBounds(overlayCanvas, icons[index - 1],
                                 &previousBounds) ||
            !TryGetElementBounds(overlayCanvas, icons[index], &currentBounds)) {
            continue;
        }

        double previousCenterX = previousBounds.X + previousBounds.Width / 2.0;
        double previousCenterY = previousBounds.Y + previousBounds.Height / 2.0;
        double currentCenterX = currentBounds.X + currentBounds.Width / 2.0;
        double currentCenterY = currentBounds.Y + currentBounds.Height / 2.0;
        totalHorizontalMovement += std::fabs(currentCenterX - previousCenterX);
        totalVerticalMovement += std::fabs(currentCenterY - previousCenterY);
        foundPair = true;
    }

    if (!foundPair) {
        if (!std::isfinite(taskbarWidth) || !std::isfinite(taskbarHeight) ||
            taskbarWidth < 0 || taskbarHeight < 0) {
            return false;
        }

        *orientation = taskbarWidth >= taskbarHeight
                           ? TaskbarOrientation::horizontal
                           : TaskbarOrientation::vertical;
        return true;
    }

    if (std::fabs(totalHorizontalMovement - totalVerticalMovement) <= 0.1) {
        return false;
    }

    *orientation = totalHorizontalMovement > totalVerticalMovement
                       ? TaskbarOrientation::horizontal
                       : TaskbarOrientation::vertical;
    return true;
}

struct RealizedButtonSnapshot {
    int itemIndex = -1;
    FrameworkElement button{nullptr};
    FrameworkElement iconPanel{nullptr};
    FrameworkElement icon{nullptr};
    double actualWidth = 0;
    double actualHeight = 0;
    Thickness margin{};
    Visibility visibility = Visibility::Visible;
};

struct TaskbarReconciliationSnapshot {
    bool signatureValid = false;
    Controls::Grid rootGrid{nullptr};
    double rootWidth = 0;
    double rootHeight = 0;
    winrt::Windows::Foundation::Rect repeaterBounds{};
    std::vector<RealizedButtonSnapshot> buttons;
};

void ApplyTrackedButtonGapMargins(
    TrackedTaskbarState& taskbar,
    std::vector<FrameworkElement> const& appButtons,
    std::vector<ButtonGapContribution> const& contributions,
    TaskbarReconciliationSnapshot* snapshot) {
    if (contributions.size() != appButtons.size()) {
        return;
    }

    for (size_t index = 0; index < appButtons.size(); index++) {
        auto const& button = appButtons[index];
        if (!button || index >= taskbar.buttonMargins.size()) {
            continue;
        }

        auto& tracked = taskbar.buttonMargins[index];
        auto const& gap = contributions[index];
        bool hasGap =
            std::fabs(gap.left) > 0.001 || std::fabs(gap.top) > 0.001 ||
            std::fabs(gap.right) > 0.001 || std::fabs(gap.bottom) > 0.001;
        if (hasGap) {
            Thickness desired = tracked.baseMargin;
            desired.Left += gap.left;
            desired.Top += gap.top;
            desired.Right += gap.right;
            desired.Bottom += gap.bottom;

            Thickness current = button.Margin();
            if (!ThicknessApproximatelyEqual(current, desired)) {
                button.Margin(desired);
            }
            tracked.lastAppliedMargin = desired;
            tracked.hasAppliedMargin = true;
        } else if (tracked.hasAppliedMargin) {
            RestoreTrackedButtonMargin(tracked);
        }

        // The reconciliation signature must describe the value we just
        // applied or restored, otherwise our own margin write looks like an
        // external layout change on the next UpdateVisualStates call.
        if (snapshot && index < snapshot->buttons.size()) {
            snapshot->buttons[index].margin = button.Margin();
        }
    }
}

bool IsFiniteThickness(Thickness const& thickness) {
    return std::isfinite(thickness.Left) && std::isfinite(thickness.Top) &&
           std::isfinite(thickness.Right) && std::isfinite(thickness.Bottom);
}

bool LayoutScalarMatches(double left, double right) {
    constexpr double kLayoutEpsilon = 0.01;
    return std::isfinite(left) && std::isfinite(right) &&
           std::fabs(left - right) <= kLayoutEpsilon;
}

bool LayoutRectMatches(winrt::Windows::Foundation::Rect const& left,
                       winrt::Windows::Foundation::Rect const& right) {
    return LayoutScalarMatches(left.X, right.X) &&
           LayoutScalarMatches(left.Y, right.Y) &&
           LayoutScalarMatches(left.Width, right.Width) &&
           LayoutScalarMatches(left.Height, right.Height);
}

bool LayoutThicknessMatches(Thickness const& left, Thickness const& right) {
    return LayoutScalarMatches(left.Left, right.Left) &&
           LayoutScalarMatches(left.Top, right.Top) &&
           LayoutScalarMatches(left.Right, right.Right) &&
           LayoutScalarMatches(left.Bottom, right.Bottom);
}

TaskbarReconciliationSnapshot CaptureTaskbarReconciliationSnapshot(
    FrameworkElement const& repeater) {
    TaskbarReconciliationSnapshot snapshot;
    auto repeaterPanel = repeater.try_as<Controls::Panel>();
    if (!repeaterPanel) {
        return snapshot;
    }

    struct RealizedElement {
        int itemIndex;
        FrameworkElement element;
    };

    std::vector<RealizedElement> realizedElements;
    for (auto const& panelChild : repeaterPanel.Children()) {
        auto child = panelChild.try_as<FrameworkElement>();
        if (!child) {
            continue;
        }

        int itemIndex = ItemsRepeater_GetElementIndex(repeater, child);
        if (itemIndex >= 0) {
            realizedElements.push_back({itemIndex, child});
        }
    }

    std::sort(realizedElements.begin(), realizedElements.end(),
              [](RealizedElement const& left, RealizedElement const& right) {
                  return left.itemIndex < right.itemIndex;
              });

    bool buttonLayoutValid = true;
    for (auto const& realized : realizedElements) {
        if (realized.element.Name() != L"TaskListButton") {
            continue;
        }

        if (!IsUsableApplicationButton(realized.element)) {
            continue;
        }

        RealizedButtonSnapshot button;
        button.itemIndex = realized.itemIndex;
        button.button = realized.element;
        button.actualWidth = realized.element.ActualWidth();
        button.actualHeight = realized.element.ActualHeight();
        button.margin = realized.element.Margin();
        button.visibility = realized.element.Visibility();
        if (!IsFiniteThickness(button.margin)) {
            buttonLayoutValid = false;
        }
        snapshot.buttons.push_back(std::move(button));
    }

    snapshot.rootGrid = FindRootGridAncestor(repeater);
    if (!snapshot.rootGrid) {
        return snapshot;
    }

    snapshot.rootWidth = snapshot.rootGrid.ActualWidth();
    snapshot.rootHeight = snapshot.rootGrid.ActualHeight();
    double repeaterWidth = repeater.ActualWidth();
    double repeaterHeight = repeater.ActualHeight();
    if (!buttonLayoutValid || !std::isfinite(snapshot.rootWidth) ||
        !std::isfinite(snapshot.rootHeight) || !std::isfinite(repeaterWidth) ||
        !std::isfinite(repeaterHeight) || snapshot.rootWidth <= 0 ||
        snapshot.rootHeight <= 0 || repeaterWidth < 0 || repeaterHeight < 0) {
        return snapshot;
    }

    snapshot.repeaterBounds =
        repeater.TransformToVisual(snapshot.rootGrid)
            .TransformBounds(winrt::Windows::Foundation::Rect{
                0, 0, static_cast<float>(repeaterWidth),
                static_cast<float>(repeaterHeight)});
    if (!std::isfinite(snapshot.repeaterBounds.X) ||
        !std::isfinite(snapshot.repeaterBounds.Y) ||
        !std::isfinite(snapshot.repeaterBounds.Width) ||
        !std::isfinite(snapshot.repeaterBounds.Height)) {
        return snapshot;
    }

    snapshot.signatureValid = true;
    return snapshot;
}

bool CachedSeparatorVisualsAreValid(TrackedTaskbarState& taskbar,
                                    Controls::Grid const& rootGrid) {
    if (!taskbar.reconciledOverlayExpected) {
        return taskbar.reconciledSeparators.empty() &&
               !taskbar.overlayCanvas.get();
    }

    if (taskbar.reconciledAnimationCompatibility) {
        auto pointerSource = taskbar.animationPointerSource.get();
        if (!taskbar.animationPointerHandlersAttached ||
            !taskbar.animationPointerExitedHandlerAttached ||
            !taskbar.animationPointerMovedHandler || !pointerSource ||
            winrt::get_abi(pointerSource) != winrt::get_abi(rootGrid)) {
            return false;
        }
    }

    auto overlayCanvas = taskbar.overlayCanvas.get();
    if (!overlayCanvas ||
        !IsOwnedOverlayName(overlayCanvas.Name()) ||
        overlayCanvas.Children().Size() !=
            taskbar.reconciledOverlayChildCount) {
        return false;
    }

    auto overlayParent = Media::VisualTreeHelper::GetParent(overlayCanvas)
                             .try_as<Controls::Grid>();
    if (!overlayParent ||
        winrt::get_abi(overlayParent) != winrt::get_abi(rootGrid)) {
        return false;
    }

    if (taskbar.reconciledOverlayExpected) {
        auto animationOverlay = taskbar.animationOverlayCanvas.get();
        if (!animationOverlay ||
            winrt::get_abi(animationOverlay) != winrt::get_abi(overlayCanvas) ||
            taskbar.animationDividers.size() !=
                taskbar.reconciledSeparators.size()) {
            return false;
        }

        for (auto const& animationDivider : taskbar.animationDividers) {
            bool requiredElementsAvailable =
                animationDivider.geometryMode ==
                        DividerGeometryMode::buttonBoundaries
                    ? animationDivider.targetButton.get() &&
                          (animationDivider.nextButton.get() ||
                           (!animationDivider.beforeFirst &&
                            animationDivider.previousButton.get()))
                    : animationDivider.targetIcon.get() &&
                          (animationDivider.nextIcon.get() ||
                           (!animationDivider.beforeFirst &&
                            animationDivider.previousIcon.get()));
            if (!animationDivider.host.get() || !requiredElementsAvailable) {
                return false;
            }
        }
    }

    for (auto const& separator : taskbar.reconciledSeparators) {
        auto host = separator.host.get();
        if (!host || host.Name() != separator.name) {
            return false;
        }

        auto hostParent =
            Media::VisualTreeHelper::GetParent(host).try_as<Controls::Canvas>();
        if (!hostParent ||
            winrt::get_abi(hostParent) != winrt::get_abi(overlayCanvas)) {
            return false;
        }
    }

    return true;
}

bool IsVisualDescendantOf(FrameworkElement const& descendant,
                          FrameworkElement const& ancestor) {
    auto current = descendant;
    for (int depth = 0; depth < 16 && current; depth++) {
        if (winrt::get_abi(current) == winrt::get_abi(ancestor)) {
            return true;
        }
        current = Media::VisualTreeHelper::GetParent(current)
                      .try_as<FrameworkElement>();
    }
    return false;
}

bool CanReuseReconciledTaskbar(TrackedTaskbarState& taskbar,
                               FrameworkElement const& repeater,
                               TaskbarReconciliationSnapshot const& snapshot,
                               unsigned int settingsGeneration) {
    if (!taskbar.reconciliationSignatureValid || !snapshot.signatureValid ||
        taskbar.appliedSettingsGeneration != settingsGeneration) {
        return false;
    }

    auto reconciledRepeater = taskbar.reconciledRepeater.get();
    auto reconciledRootGrid = taskbar.reconciledRootGrid.get();
    if (!reconciledRepeater || !reconciledRootGrid ||
        winrt::get_abi(reconciledRepeater) != winrt::get_abi(repeater) ||
        winrt::get_abi(reconciledRootGrid) !=
            winrt::get_abi(snapshot.rootGrid) ||
        !LayoutScalarMatches(taskbar.reconciledRootWidth, snapshot.rootWidth) ||
        !LayoutScalarMatches(taskbar.reconciledRootHeight,
                             snapshot.rootHeight) ||
        !LayoutRectMatches(taskbar.reconciledRepeaterBounds,
                           snapshot.repeaterBounds) ||
        taskbar.reconciledButtons.size() != snapshot.buttons.size()) {
        return false;
    }

    for (size_t index = 0; index < snapshot.buttons.size(); index++) {
        auto const& cached = taskbar.reconciledButtons[index];
        auto const& current = snapshot.buttons[index];
        auto cachedButton = cached.button.get();
        if (!cachedButton ||
            winrt::get_abi(cachedButton) != winrt::get_abi(current.button) ||
            cached.itemIndex != current.itemIndex ||
            !LayoutScalarMatches(cached.actualWidth, current.actualWidth) ||
            !LayoutScalarMatches(cached.actualHeight, current.actualHeight) ||
            !LayoutThicknessMatches(cached.margin, current.margin) ||
            cached.visibility != current.visibility) {
            return false;
        }

        if (cached.iconResolved) {
            auto cachedIconPanel = cached.iconPanel.get();
            auto cachedIcon = cached.icon.get();
            if (!cachedIconPanel || !cachedIcon ||
                cachedIconPanel.Name() != L"IconPanel" ||
                cachedIcon.Name() != L"Icon" ||
                !IsVisualDescendantOf(cachedIconPanel, current.button) ||
                !IsVisualDescendantOf(cachedIcon, cachedIconPanel)) {
                return false;
            }
        } else {
            auto currentIconPanel =
                FindDescendantByName(current.button, L"IconPanel")
                    .try_as<Controls::Panel>();
            if (currentIconPanel &&
                FindDescendantByName(currentIconPanel, L"Icon")) {
                // A previously unrealized button is now placeable.
                return false;
            }
        }
    }

    return CachedSeparatorVisualsAreValid(taskbar, snapshot.rootGrid);
}

void CommitReconciledTaskbar(
    TrackedTaskbarState& taskbar,
    FrameworkElement const& repeater,
    TaskbarReconciliationSnapshot const& snapshot,
    ReconcileResult result,
    bool overlayExpected,
    uint32_t overlayChildCount,
    bool animationCompatibility,
    std::vector<ReconciledSeparatorVisual>&& separators) {
    if (!snapshot.signatureValid ||
        result == ReconcileResult::temporarilyNotReady) {
        return;
    }

    taskbar.cachedReconcileResult = result;
    taskbar.reconciledRepeater = winrt::make_weak(repeater);
    taskbar.reconciledRootGrid = winrt::make_weak(snapshot.rootGrid);
    taskbar.reconciledRootWidth = snapshot.rootWidth;
    taskbar.reconciledRootHeight = snapshot.rootHeight;
    taskbar.reconciledRepeaterBounds = snapshot.repeaterBounds;
    taskbar.reconciledButtons.clear();
    taskbar.reconciledButtons.reserve(snapshot.buttons.size());
    for (auto const& button : snapshot.buttons) {
        bool iconResolved = button.iconPanel && button.icon;
        taskbar.reconciledButtons.push_back(
            {button.itemIndex, winrt::make_weak(button.button),
             button.iconPanel ? winrt::make_weak(button.iconPanel)
                              : winrt::weak_ref<FrameworkElement>{},
             button.icon ? winrt::make_weak(button.icon)
                         : winrt::weak_ref<FrameworkElement>{},
             iconResolved, button.actualWidth, button.actualHeight,
             button.margin, button.visibility});
    }
    taskbar.reconciledOverlayExpected = overlayExpected;
    taskbar.reconciledOverlayChildCount = overlayChildCount;
    taskbar.reconciledAnimationCompatibility = animationCompatibility;
    taskbar.reconciledSeparators = std::move(separators);
    taskbar.reconciliationSignatureValid = true;
}

// This is the only overlay function that mutates the taskbar XAML visual tree.
// Callers must already be running on the taskbar XAML/UI thread.
ReconcileResult ReconcileTrackedTaskbar(TrackedTaskbarState& taskbar,
                                        FrameworkElement const& repeater,
                                        bool forceStructuralReconcile) {
    ReconcileResult result = ReconcileResult::temporarilyNotReady;

    try {
        if (repeater && repeater.Dispatcher().HasThreadAccess()) {
            if (forceStructuralReconcile) {
                InvalidateReconciliationSignature(taskbar);
            }

            auto snapshot = CaptureTaskbarReconciliationSnapshot(repeater);
            unsigned int currentSettingsGeneration =
                g_settingsGeneration.load(std::memory_order_acquire);
            // UpdateVisualStates is a hot path. A matching weak/scalar
            // snapshot proves that structural work is unchanged; active
            // animation geometry remains the Rendering callback's job.
            if (!forceStructuralReconcile &&
                CanReuseReconciledTaskbar(taskbar, repeater, snapshot,
                                          currentSettingsGeneration)) {
                if (taskbar.reconciledAnimationCompatibility &&
                    taskbar.reconciledOverlayExpected) {
                    // UpdateVisualStates can be raised by non-pointer
                    // composition changes. Reuse the existing bounded
                    // Rendering refresh without repeating structural work.
                    StartAnimationTracking(taskbar);
                }
                return taskbar.cachedReconcileResult;
            }

            // while a reorder drag is in flight, never touch margins or
            // divider geometry. The live queue is drag-distorted (the dragged button
            // becomes a floating ghost), so any recomputation fights the drag
            // animation and makes the gap vanish or flicker. The press-time forced
            // reconcile (forceStructuralReconcile) is still allowed to run: it applies
            // the frozen gap fully on the side opposite the pressed button, which
            // keeps the visible spacing intact when that button becomes the ghost.
            if (taskbar.reorderDragActive && taskbar.hasFrozenDynamicGap &&
                !forceStructuralReconcile) {
                return taskbar.cachedReconcileResult;
            }

            InvalidateReconciliationSignature(taskbar);

            Settings settings = GetSettingsSnapshot();
            // Whether dynamic centering is in effect right now; the
            // setting only says what the mod was asked to do.
            const bool dynamicCenteringActive =
                g_dynamicCenteringActive.load(std::memory_order_acquire);
            unsigned int settingsGeneration =
                g_settingsGeneration.load(std::memory_order_acquire);
            bool settingsChanged =
                taskbar.appliedSettingsGeneration != settingsGeneration;

            if (!settings.animationCompatibility) {
                StopAnimationTracking(taskbar);
                DetachAnimationPointerHandlers(taskbar);
                ClearAnimationElementCache(taskbar);
            }

            std::vector<FrameworkElement> appButtons;
            appButtons.reserve(snapshot.buttons.size());
            for (auto const& button : snapshot.buttons) {
                appButtons.push_back(button.button);
            }

            std::vector<FrameworkElement> appIcons;
            appIcons.reserve(appButtons.size());
            for (size_t index = 0; index < appButtons.size(); index++) {
                appIcons.push_back(nullptr);
            }
            for (size_t buttonIndex = 0; buttonIndex < appButtons.size();
                 buttonIndex++) {
                auto iconPanel =
                    FindDescendantByName(appButtons[buttonIndex], L"IconPanel")
                        .try_as<Controls::Panel>();
                if (!iconPanel) {
                    continue;
                }

                snapshot.buttons[buttonIndex].iconPanel = iconPanel;
                appIcons[buttonIndex] =
                    FindDescendantByName(iconPanel, L"Icon");
                snapshot.buttons[buttonIndex].icon = appIcons[buttonIndex];
            }

            struct ActiveSeparator {
                SeparatorSettings settings;
                std::wstring name;
                bool beforeFirst = false;
            };

            // Gating A below clears every active separator while dynamic
            // centering is not in effect, so resolving app names for them
            // would only produce work whose result is discarded.
            std::vector<TaskbarAppDetails> appDetails;
            if (dynamicCenteringActive && HasAppNameSeparators(settings)) {
                appDetails.reserve(appButtons.size());
                for (auto const& button : appButtons) {
                    try {
                        appDetails.push_back(GetTaskbarButtonDetails(button));
                    } catch (...) {
                        appDetails.emplace_back();
                    }
                }
            }

            std::vector<ActiveSeparator> activeSeparators;
            std::vector<int> usedPositions;
            bool beforeFirstUsed = false;

            if (settings.separatorBeforeFirstApp) {
                activeSeparators.push_back(
                    {SeparatorSettings{}, GetBeforeFirstDividerName(), true});
                beforeFirstUsed = true;
            }

            for (const auto& separator : settings.separators) {
                int position = separator.position;

                if (!separator.appName.empty()) {
                    position = -1;

                    for (size_t i = 0; i < appDetails.size(); ++i) {
                        if (IsMatch(appDetails[i], separator.appName)) {
                            position = static_cast<int>(i + 1);
                            break;
                        }
                    }
                }

                if (position <= 0) {
                    continue;
                }

                SeparatorSettings resolvedSeparator = separator;

                if (separator.before) {
                    if (position == 1) {
                        if (!beforeFirstUsed) {
                            activeSeparators.push_back(
                                {resolvedSeparator,
                                 GetDividerName(separator.settingsIndex),
                                 true});
                            beforeFirstUsed = true;
                        }

                        continue;
                    }

                    position--;
                }

                if (std::find(usedPositions.begin(), usedPositions.end(),
                              position) != usedPositions.end()) {
                    continue;
                }

                resolvedSeparator.position = position;

                usedPositions.push_back(position);

                activeSeparators.push_back(
                    {resolvedSeparator, GetDividerName(separator.settingsIndex),
                     false});
            }

            auto rootGrid = snapshot.rootGrid;
            auto trackedRootGrid = taskbar.rootGrid.get();
            if (trackedRootGrid &&
                (!rootGrid ||
                 winrt::get_abi(trackedRootGrid) != winrt::get_abi(rootGrid))) {
                RemoveTrackedTaskbarElements(taskbar);
            }
            if (rootGrid) {
                taskbar.rootGrid = winrt::make_weak(rootGrid);
            }

            // Product rule: the dividers are only the mechanical means of
            // dynamic centering, so while dynamic centering is not in effect
            // (taskbar center-aligned) the mod creates and keeps no dividers at
            // all. The overlay container is dropped with them, and because this
            // is re-decided on every rebuild, a later layout event cannot bring
            // the dividers back on its own.
            if (!dynamicCenteringActive) {
                activeSeparators.clear();
            }

            Controls::Canvas overlayCanvas = nullptr;
            bool needOverlay = !activeSeparators.empty();

            if (rootGrid) {
                auto rootChildren = rootGrid.Children();
                for (uint32_t childIndex = 0;
                     childIndex < rootChildren.Size();) {
                    auto child = rootChildren.GetAt(childIndex)
                                     .try_as<FrameworkElement>();
                    if (!child) {
                        childIndex++;
                        continue;
                    }

                    if (IsOwnedOverlayName(child.Name())) {
                        auto canvas = child.try_as<Controls::Canvas>();
                        if (needOverlay && canvas && !overlayCanvas) {
                            overlayCanvas = canvas;
                            childIndex++;
                        } else {
                            rootChildren.RemoveAt(childIndex);
                        }
                        continue;
                    }

                    std::wstring childName{child.Name()};
                    if (IsOwnedDividerName(childName)) {
                        rootChildren.RemoveAt(childIndex);
                        continue;
                    }

                    childIndex++;
                }

                if (needOverlay && !overlayCanvas) {
                    Controls::Canvas overlay;
                    overlay.Name(
                        winrt::hstring(GetSeparatorOverlayName()));
                    overlay.HorizontalAlignment(HorizontalAlignment::Stretch);
                    overlay.VerticalAlignment(VerticalAlignment::Stretch);
                    overlay.IsHitTestVisible(false);
                    Controls::Canvas::SetZIndex(overlay, 1000);
                    rootChildren.Append(overlay);
                    overlayCanvas = overlay;
                }

                if (overlayCanvas) {
                    if (overlayCanvas.HorizontalAlignment() !=
                        HorizontalAlignment::Stretch) {
                        overlayCanvas.HorizontalAlignment(
                            HorizontalAlignment::Stretch);
                    }
                    if (overlayCanvas.VerticalAlignment() !=
                        VerticalAlignment::Stretch) {
                        overlayCanvas.VerticalAlignment(
                            VerticalAlignment::Stretch);
                    }
                    if (overlayCanvas.IsHitTestVisible()) {
                        overlayCanvas.IsHitTestVisible(false);
                    }
                    if (Controls::Canvas::GetZIndex(overlayCanvas) != 1000) {
                        Controls::Canvas::SetZIndex(overlayCanvas, 1000);
                    }
                }
            }

            if (overlayCanvas) {
                taskbar.overlayCanvas = winrt::make_weak(overlayCanvas);
            } else {
                taskbar.overlayCanvas = {};
            }

            if (settings.animationCompatibility && needOverlay && rootGrid) {
                AttachAnimationPointerHandlers(taskbar, rootGrid);
            } else {
                DetachAnimationPointerHandlers(taskbar);
            }

            // Watch the repeater panel's layout so a button
            // removed by closing an app (which never raises UpdateVisualStates)
            // is caught by the realized-button count mismatch below and forces
            // a structural reconcile to restore the split margins.
            if (auto repeaterPanel = repeater.try_as<Controls::Panel>()) {
                AttachLayoutChangeMonitor(taskbar, repeaterPanel);
            } else {
                DetachLayoutChangeMonitor(taskbar);
            }

            std::vector<Controls::Canvas> existingHosts;
            existingHosts.reserve(activeSeparators.size());
            for (size_t index = 0; index < activeSeparators.size(); index++) {
                existingHosts.push_back(nullptr);
            }
            if (overlayCanvas) {
                auto overlayChildren = overlayCanvas.Children();
                for (uint32_t childIndex = 0;
                     childIndex < overlayChildren.Size();) {
                    auto child = overlayChildren.GetAt(childIndex)
                                     .try_as<FrameworkElement>();
                    if (!child) {
                        childIndex++;
                        continue;
                    }

                    std::wstring childName{child.Name()};
                    if (!IsOwnedDividerName(childName)) {
                        childIndex++;
                        continue;
                    }

                    auto desired = std::find_if(
                        activeSeparators.begin(), activeSeparators.end(),
                        [&](ActiveSeparator const& separator) {
                            return separator.name == childName;
                        });
                    auto host = child.try_as<Controls::Canvas>();
                    if (desired == activeSeparators.end() || !host) {
                        overlayChildren.RemoveAt(childIndex);
                        continue;
                    }

                    size_t desiredIndex =
                        static_cast<size_t>(desired - activeSeparators.begin());
                    if (existingHosts[desiredIndex]) {
                        overlayChildren.RemoveAt(childIndex);
                        continue;
                    }

                    existingHosts[desiredIndex] = host;
                    childIndex++;
                }
            }

            TaskbarOrientation taskbarOrientation =
                TaskbarOrientation::horizontal;
            bool orientationValid =
                overlayCanvas &&
                TryGetTaskbarOrientation(
                    settings.orientation, overlayCanvas, snapshot.rootWidth,
                    snapshot.rootHeight, appIcons, &taskbarOrientation);
            DividerGeometryMode geometryMode =
                orientationValid ? DetectDividerGeometryMode(
                                       appButtons, appIcons, taskbarOrientation)
                                 : DividerGeometryMode::iconCenters;
            double primaryOrderingDirection = 0;
            bool primaryOrderingValid =
                orientationValid &&
                TryGetPrimaryOrderingDirection(overlayCanvas, appButtons,
                                               taskbarOrientation,
                                               &primaryOrderingDirection);

            // Dynamic centering is in effect only on a horizontal taskbar, so
            // a vertical one keeps no dividers either. Nothing is created once
            // the divider list is empty, and the shared "no active divider"
            // cleanup below also drops the overlay container.
            if (orientationValid &&
                taskbarOrientation != TaskbarOrientation::horizontal) {
                activeSeparators.clear();
            }

            // A mirrored, right-to-left taskbar lays its buttons out along
            // decreasing primary coordinates, while the dynamic-centering
            // geometry is written for increasing ones, so the centered gap
            // cannot be computed there. Rather than applying a silently
            // collapsed gap, the mirrored case is treated like the vertical
            // one: dynamic centering is not applied, so no dividers exist.
            //
            // The test must stay conservative, so it uses
            // IsMirroredPrimaryOrdering rather than the first-pair-wins
            // lookup that places the gaps. In that lookup a single pair can
            // read as reversed for one frame while reordered buttons are still
            // animating, and acting on it would clear every divider at once,
            // making the centering collapse and then snap back on the next
            // frame, which is visible even on an ordinary left-to-right
            // taskbar. IsMirroredPrimaryOrdering reports a mirrored ordering
            // only once every adjacent pair that is laid out far enough
            // agrees on a negative primary movement, and it reports false
            // while the buttons are not laid out yet, so a taskbar that has
            // not been measured is never mistaken for a mirrored one. A drag
            // in progress suppresses the test as well, so it is never fed a
            // layout that is mid-flight.
            if (orientationValid &&
                taskbarOrientation == TaskbarOrientation::horizontal &&
                !taskbar.reorderDragActive &&
                IsMirroredPrimaryOrdering(overlayCanvas, appButtons,
                                          taskbarOrientation)) {
                activeSeparators.clear();
            }

            // Keep a stable base margin for each realized TaskListButton. Any
            // physical divider gap is then added as our own delta on top of
            // that base, preserving margins supplied by Windows or other mods.
            SynchronizeTrackedButtonMargins(taskbar, appButtons);
            std::vector<ButtonGapContribution> gapContributions(
                appButtons.size());

            bool hasDynamicSeparator = false;
            for (auto const& activeSeparator : activeSeparators) {
                if (!activeSeparator.beforeFirst &&
                    activeSeparator.settings.position ==
                        settings.dynamicPosition &&
                    dynamicCenteringActive) {
                    hasDynamicSeparator = true;
                    break;
                }
            }

            if (orientationValid && primaryOrderingValid &&
                (settings.separatorGap > 0 || hasDynamicSeparator)) {
                for (auto const& activeSeparator : activeSeparators) {
                    bool isDynamic =
                        !activeSeparator.beforeFirst &&
                        activeSeparator.settings.position ==
                            settings.dynamicPosition &&
                        dynamicCenteringActive;
                    double fullGap =
                        static_cast<double>(settings.separatorGap);
                    bool dynamicVisible = true;

                    if (isDynamic) {
                        if (taskbar.reorderDragActive &&
                            taskbar.hasFrozenDynamicGap) {
                            dynamicVisible =
                                taskbar.frozenDynamicSeparatorVisible;
                            fullGap = taskbar.frozenDynamicGap;
                        } else {
                            fullGap = CalculateDynamicCenteredGap(
                                snapshot.rootGrid, overlayCanvas, appButtons,
                                activeSeparator.settings.position,
                                taskbarOrientation, &dynamicVisible,
                                &taskbar.trayFrame);
                            if (dynamicVisible && fullGap < 1.0) {
                                // Right after a drag
                                // release the buttons are still in the
                                // settle animation and the live
                                // geometry can be invalid, which makes
                                // the centered-gap calculation return
                                // ~0 and collapses the spacing (flash).
                                // Fall back to the last valid value.
                                fullGap =
                                    taskbar.lastAppliedDynamicGap;
                            }
                        }
                        taskbar.lastAppliedDynamicGap = fullGap;
                        taskbar.lastDynamicSeparatorVisible =
                            dynamicVisible;
                    }

                    if (activeSeparator.beforeFirst) {
                        if (fullGap <= 0) {
                            continue;
                        }
                        // Both geometry paths need a neighboring realized
                        // button to establish signed application ordering.
                        bool canPlaceBeforeFirst =
                            geometryMode ==
                                    DividerGeometryMode::buttonBoundaries
                                ? appButtons.size() >= 2 && appButtons[0] &&
                                      appButtons[1]
                                : appIcons.size() >= 2 && appIcons[0] &&
                                      appIcons[1];
                        if (canPlaceBeforeFirst) {
                            AddDirectionalButtonGap(
                                &gapContributions[0], taskbarOrientation,
                                primaryOrderingDirection, true, fullGap);
                        }
                        continue;
                    }

                    if (isDynamic && !dynamicVisible) {
                        continue;
                    }

                    if (!isDynamic && fullGap <= 0) {
                        continue;
                    }

                    size_t buttonIndex = static_cast<size_t>(
                        activeSeparator.settings.position - 1);
                    if (buttonIndex >= appButtons.size()) {
                        continue;
                    }

                    bool hasPrevious = buttonIndex > 0;
                    bool hasNext = buttonIndex + 1 < appButtons.size();
                    if (!hasPrevious && !hasNext) {
                        continue;
                    }

                    if (hasNext) {
                        // While a drag is in progress, put the
                        // full gap on whichever side of the divider the
                        // dragged button is NOT currently on, so the spacing
                        // stays put no matter where the button is dragged.
                        bool isDraggingDynamic =
                            taskbar.reorderDragActive &&
                            taskbar.hasFrozenDynamicGap &&
                            isDynamic;

                        if (isDraggingDynamic) {
                            // Align the drag-side single gap to the physical
                            // pixel grid so the split margin never leaves
                            // sub-pixel coordinates (measured ~0.7 DIP
                            // right-group shift when the button count
                            // rounding kicks in).
                            double alignedGap = fullGap;
                            if (overlayCanvas) {
                                alignedGap = SnapToPhysicalPixel(
                                    fullGap,
                                    GetRasterizationScale(overlayCanvas));
                            }
                            // Locate the dragged button in the current order;
                            // fall back to the index recorded at press time
                            // when it cannot be found.
                            int currentDraggedIndex = -1;
                            if (auto dragged = taskbar.draggedButton.get()) {
                                for (size_t i = 0; i < appButtons.size();
                                     i++) {
                                    if (appButtons[i] &&
                                        winrt::get_abi(appButtons[i]) ==
                                            winrt::get_abi(dragged)) {
                                        currentDraggedIndex =
                                            static_cast<int>(i);
                                        break;
                                    }
                                }
                            }
                            if (currentDraggedIndex < 0) {
                                currentDraggedIndex =
                                    taskbar.draggedButtonIndex;
                            }

                            if (currentDraggedIndex >
                                static_cast<int>(buttonIndex)) {
                                // The dragged button is currently to the
                                // right of this divider: give the full gap
                                // to the left boundary button.
                                AddDirectionalButtonGap(
                                    &gapContributions[buttonIndex],
                                    taskbarOrientation,
                                    primaryOrderingDirection,
                                    false, alignedGap);
                            } else {
                                // The dragged button is currently to the
                                // left of this divider (or is its left
                                // boundary button): give the full gap to the
                                // right boundary button.
                                AddDirectionalButtonGap(
                                    &gapContributions[buttonIndex + 1],
                                    taskbarOrientation,
                                    primaryOrderingDirection,
                                    true, alignedGap);
                            }
                        } else {
                            // Interior divider: split the requested space
                            // across the two neighboring buttons so the
                            // overlay stays centered in the new physical gap.
                            // Each half is snapped to the physical pixel grid
                            // independently, so the two snapped halves can
                            // miss fullGap by up to half a physical pixel.
                            // That rounding is deliberate: it keeps the
                            // margins from carrying sub-pixel values that the
                            // layout engine rounds differently for even/odd
                            // button counts (~0.7 DIP shift).
                            double halfLeft = fullGap / 2.0;
                            double halfRight = fullGap - halfLeft;
                            if (overlayCanvas) {
                                double scale =
                                    GetRasterizationScale(overlayCanvas);
                                halfLeft =
                                    SnapToPhysicalPixel(halfLeft, scale);
                                halfRight = SnapToPhysicalPixel(
                                    fullGap - halfLeft, scale);
                            }
                            AddDirectionalButtonGap(
                                &gapContributions[buttonIndex],
                                taskbarOrientation, primaryOrderingDirection,
                                false, halfLeft);
                            AddDirectionalButtonGap(
                                &gapContributions[buttonIndex + 1],
                                taskbarOrientation, primaryOrderingDirection,
                                true, halfRight);
                        }
                    } else {
                        // Divider after the final realized app button.
                        AddDirectionalButtonGap(
                            &gapContributions[buttonIndex],
                            taskbarOrientation, primaryOrderingDirection,
                            false, fullGap);
                    }
                }
            }

            bool hasGapContributions =
                std::any_of(gapContributions.begin(), gapContributions.end(),
                            [](ButtonGapContribution const& gap) {
                                return std::fabs(gap.left) > 0.001 ||
                                       std::fabs(gap.top) > 0.001 ||
                                       std::fabs(gap.right) > 0.001 ||
                                       std::fabs(gap.bottom) > 0.001;
                            });

            // A zero gap, removed divider, or now-invalid position naturally
            // produces a zero contribution and restores our previous delta.
            // If orientation is temporarily unavailable while a non-zero gap
            // is configured, keep the current margin until the next valid
            // reconciliation rather than guessing an axis. A lone usable
            // button cannot establish ordering, but needs no direction to
            // remove a previously tracked contribution.
            if (settings.separatorGap == 0 ||
                (orientationValid && primaryOrderingValid) ||
                (appButtons.size() < 2 && !hasGapContributions) ||
                activeSeparators.empty()) {
                ApplyTrackedButtonGapMargins(taskbar, appButtons,
                                             gapContributions, &snapshot);
            }

            std::vector<AnimationDividerCache> animationDividers;
            std::vector<ReconciledSeparatorVisual> reconciledSeparators;
            reconciledSeparators.reserve(activeSeparators.size());
            size_t activeDividerCount = 0;
            size_t intentionallyHiddenSeparators = 0;
            bool geometryFailureWasTransient = false;
            for (size_t activeIndex = 0; activeIndex < activeSeparators.size();
                 activeIndex++) {
                auto const& activeSeparator = activeSeparators[activeIndex];
                auto const& separator = activeSeparator.settings;
                bool isDynamicSeparator =
                    !activeSeparator.beforeFirst &&
                    separator.position == settings.dynamicPosition &&
                    dynamicCenteringActive;
                if (isDynamicSeparator && !taskbar.lastDynamicSeparatorVisible) {
                    auto staleHost = existingHosts[activeIndex];
                    if (staleHost && overlayCanvas) {
                        uint32_t staleIndex = 0;
                        auto overlayChildren = overlayCanvas.Children();
                        if (overlayChildren.IndexOf(staleHost, staleIndex)) {
                            overlayChildren.RemoveAt(staleIndex);
                        }
                    }
                    intentionallyHiddenSeparators++;
                    continue;
                }

                FrameworkElement previousIcon = nullptr;
                FrameworkElement targetIcon = nullptr;
                FrameworkElement nextIcon = nullptr;
                FrameworkElement previousButton = nullptr;
                FrameworkElement targetButton = nullptr;
                FrameworkElement nextButton = nullptr;
                size_t buttonIndex = 0;
                if (activeSeparator.beforeFirst) {
                    if (!appButtons.empty()) {
                        targetButton = appButtons[0];
                        if (appButtons.size() >= 2) {
                            nextButton = appButtons[1];
                        }
                    }
                    if (!appIcons.empty()) {
                        targetIcon = appIcons[0];
                        if (appIcons.size() >= 2) {
                            nextIcon = appIcons[1];
                        }
                    }
                } else {
                    buttonIndex = static_cast<size_t>(separator.position - 1);
                    if (buttonIndex < appButtons.size()) {
                        targetButton = appButtons[buttonIndex];
                        if (buttonIndex > 0) {
                            previousButton = appButtons[buttonIndex - 1];
                        }
                        if (buttonIndex + 1 < appButtons.size()) {
                            nextButton = appButtons[buttonIndex + 1];
                        }
                    }
                    if (buttonIndex < appIcons.size()) {
                        targetIcon = appIcons[buttonIndex];
                        if (buttonIndex > 0) {
                            previousIcon = appIcons[buttonIndex - 1];
                        }
                        if (buttonIndex + 1 < appIcons.size()) {
                            nextIcon = appIcons[buttonIndex + 1];
                        }
                    }
                }

                double hostWidth = 0;
                double hostHeight = 0;
                GetStyleHostSize(settings, taskbarOrientation, &hostWidth,
                                 &hostHeight);
                bool geometryExpectedFromCurrentButtons =
                    geometryMode == DividerGeometryMode::buttonBoundaries
                        ? targetButton &&
                              (nextButton ||
                               (!activeSeparator.beforeFirst && previousButton))
                    : activeSeparator.beforeFirst
                        ? targetIcon && nextIcon
                        : targetIcon && (nextIcon || previousIcon);
                DividerGeometry geometry;
                bool geometryValid = false;
                bool beforeFirstHasReliablePitch =
                    !activeSeparator.beforeFirst || nextIcon;
                if (orientationValid &&
                    geometryMode == DividerGeometryMode::buttonBoundaries) {
                    geometryValid = TryGetButtonBoundaryDividerGeometry(
                        overlayCanvas, previousButton, targetButton, nextButton,
                        activeSeparator.beforeFirst, false, taskbarOrientation,
                        snapshot.rootWidth, snapshot.rootHeight, hostWidth,
                        hostHeight, &geometry);
                } else if (orientationValid && targetIcon &&
                           beforeFirstHasReliablePitch) {
                    geometryValid =
                        activeSeparator.beforeFirst
                            ? TryGetBeforeFirstDividerGeometry(
                                  overlayCanvas, targetIcon, nextIcon,
                                  taskbarOrientation, hostWidth, hostHeight,
                                  &geometry)
                            : (nextIcon || previousIcon) &&
                                  TryGetDividerGeometry(
                                      overlayCanvas, previousIcon, targetIcon,
                                      nextIcon, taskbarOrientation,
                                      snapshot.rootWidth, snapshot.rootHeight,
                                      hostWidth, hostHeight, &geometry);
                }
                if (!geometryValid) {
                    if (geometryExpectedFromCurrentButtons) {
                        // The required realized geometry elements exist, so
                        // a failed orientation/transform indicates a
                        // rebuilding or otherwise transient visual tree.
                        geometryFailureWasTransient = true;
                    }
                    auto staleHost = existingHosts[activeIndex];
                    if (staleHost && overlayCanvas) {
                        uint32_t staleIndex = 0;
                        auto overlayChildren = overlayCanvas.Children();
                        if (overlayChildren.IndexOf(staleHost, staleIndex)) {
                            overlayChildren.RemoveAt(staleIndex);
                        }
                    }
                    continue;
                }
                double effectiveGap =
                    static_cast<double>(settings.separatorGap);
                if (isDynamicSeparator) {
                    effectiveGap = taskbar.lastAppliedDynamicGap;
                }

                bool afterLast = !activeSeparator.beforeFirst &&
                                 buttonIndex + 1 >= appButtons.size();
                double primaryOffset =
                    geometryMode == DividerGeometryMode::iconCenters
                        ? GetBoundaryDividerPrimaryOffset(
                              geometry, taskbarOrientation,
                              activeSeparator.beforeFirst, afterLast,
                              effectiveGap)
                        : 0;
                ApplyDividerPrimaryOffset(overlayCanvas, taskbarOrientation,
                                          primaryOffset, &geometry);

                auto host = existingHosts[activeIndex];
                if (!host) {
                    Controls::Canvas newHost;
                    newHost.Name(activeSeparator.name);
                    newHost.HorizontalAlignment(HorizontalAlignment::Left);
                    newHost.VerticalAlignment(VerticalAlignment::Top);
                    newHost.IsHitTestVisible(false);
                    newHost.UseLayoutRounding(true);
                    Controls::Canvas::SetZIndex(newHost, 1000);
                    overlayCanvas.Children().Append(newHost);
                    host = newHost;
                }

                if (host.HorizontalAlignment() != HorizontalAlignment::Left) {
                    host.HorizontalAlignment(HorizontalAlignment::Left);
                }
                if (host.VerticalAlignment() != VerticalAlignment::Top) {
                    host.VerticalAlignment(VerticalAlignment::Top);
                }
                if (host.IsHitTestVisible()) {
                    host.IsHitTestVisible(false);
                }
                if (!host.UseLayoutRounding()) {
                    host.UseLayoutRounding(true);
                }
                if (Controls::Canvas::GetZIndex(host) != 1000) {
                    Controls::Canvas::SetZIndex(host, 1000);
                }
                if (host.Width() != hostWidth) {
                    host.Width(hostWidth);
                }
                if (host.Height() != hostHeight) {
                    host.Height(hostHeight);
                }
                if (Controls::Canvas::GetLeft(host) != geometry.left) {
                    Controls::Canvas::SetLeft(host, geometry.left);
                }
                if (Controls::Canvas::GetTop(host) != geometry.top) {
                    Controls::Canvas::SetTop(host, geometry.top);
                }

                double opacity = settings.opacityPercent / 100.0;
                if (host.Opacity() != opacity) {
                    host.Opacity(opacity);
                }
                ConfigureStyleHost(host, settings, taskbarOrientation,
                                   hostWidth, hostHeight);

                reconciledSeparators.push_back(
                    {activeSeparator.name, winrt::make_weak(host)});

                AnimationDividerCache cache;
                cache.host = winrt::make_weak(host);
                if (previousIcon) {
                    cache.previousIcon = winrt::make_weak(previousIcon);
                }
                if (targetIcon) {
                    cache.targetIcon = winrt::make_weak(targetIcon);
                }
                if (nextIcon) {
                    cache.nextIcon = winrt::make_weak(nextIcon);
                }
                if (previousButton) {
                    cache.previousButton = winrt::make_weak(previousButton);
                }
                if (targetButton) {
                    cache.targetButton = winrt::make_weak(targetButton);
                }
                if (nextButton) {
                    cache.nextButton = winrt::make_weak(nextButton);
                }
                cache.orientation = taskbarOrientation;
                cache.geometryMode = geometryMode;
                cache.beforeFirst = activeSeparator.beforeFirst;
                cache.primaryOffset = primaryOffset;
                animationDividers.push_back(std::move(cache));
                activeDividerCount++;
            }

            if (activeDividerCount == 0 && overlayCanvas && rootGrid) {
                uint32_t overlayIndex = 0;
                auto rootChildren = rootGrid.Children();
                if (rootChildren.IndexOf(overlayCanvas, overlayIndex)) {
                    rootChildren.RemoveAt(overlayIndex);
                    overlayCanvas = nullptr;
                    taskbar.overlayCanvas = {};
                    needOverlay = false;
                } else {
                    geometryFailureWasTransient = true;
                }
            }

            taskbar.animationDividers = std::move(animationDividers);
            if (!taskbar.animationDividers.empty() && overlayCanvas) {
                taskbar.animationOverlayCanvas =
                    winrt::make_weak(overlayCanvas);
            } else {
                StopAllGeometryTracking(taskbar);
                ClearAnimationElementCache(taskbar);
                DetachAnimationPointerHandlers(taskbar);
            }

            if (needOverlay && rootGrid && activeDividerCount > 0) {
                AttachReorderPointerHandler(taskbar, rootGrid);
            } else {
                DetachReorderPointerHandler(taskbar);
            }

            size_t expectedSeparatorCount =
                activeSeparators.size() - intentionallyHiddenSeparators;
            if (expectedSeparatorCount == 0) {
                result = ReconcileResult::succeeded;
            } else if (geometryFailureWasTransient) {
                result = ReconcileResult::temporarilyNotReady;
            } else if (activeDividerCount == expectedSeparatorCount) {
                result = ReconcileResult::succeeded;
            } else {
                result = ReconcileResult::succeededPartial;
            }

            if (settingsChanged ||
                taskbar.lastActiveDividerCount != activeDividerCount) {
                if (result == ReconcileResult::temporarilyNotReady) {
                    Wh_Log(L"Failed to reconcile dividers: active %zu of %zu",
                           activeDividerCount, expectedSeparatorCount);
                }
                taskbar.lastActiveDividerCount = activeDividerCount;
            }

            if (result != ReconcileResult::temporarilyNotReady) {
                taskbar.appliedSettingsGeneration = settingsGeneration;
                CommitReconciledTaskbar(
                    taskbar, repeater, snapshot, result, needOverlay,
                    overlayCanvas ? overlayCanvas.Children().Size() : 0,
                    settings.animationCompatibility,
                    std::move(reconciledSeparators));
                if (taskbar.reconciliationSignatureValid &&
                    (result == ReconcileResult::succeeded ||
                     result == ReconcileResult::succeededPartial)) {
                    StartNativeSettlingTracking(taskbar);
                    if (settings.animationCompatibility) {
                        StartAnimationTracking(taskbar);
                    }
                }
            }
        }
    } catch (winrt::hresult_error const& e) {
        InvalidateReconciliationSignature(taskbar);
        Wh_Log(L"Failed to reconcile dividers: 0x%08X %s",
               static_cast<unsigned int>(e.code().value), e.message().c_str());
    } catch (...) {
        InvalidateReconciliationSignature(taskbar);
        Wh_Log(L"Failed to reconcile dividers with an unknown exception");
    }

    return result;
}

FrameworkElement DiscoverTaskbarRepeater(HWND taskbarWnd) {
    XamlRoot xamlRoot = GetTaskbarXamlRootForWindow(taskbarWnd);
    auto root =
        xamlRoot ? xamlRoot.Content().try_as<FrameworkElement>() : nullptr;
    return FindDescendantByName(root, L"TaskbarFrameRepeater");
}

struct TaskbarDiscoveryResult {
    std::vector<FrameworkElement> repeaters;
};

TaskbarDiscoveryResult DiscoverCurrentThreadTaskbars() {
    TaskbarDiscoveryResult result;
    std::vector<HWND> taskbarWindows;
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            WCHAR className[32];
            if (GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
                 _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0)) {
                reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(hWnd);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&taskbarWindows));

    for (HWND taskbarWnd : taskbarWindows) {
        try {
            auto repeater = DiscoverTaskbarRepeater(taskbarWnd);
            if (!repeater) {
                continue;
            }

            auto existing =
                std::find_if(result.repeaters.begin(), result.repeaters.end(),
                             [&](FrameworkElement const& existingRepeater) {
                                 return winrt::get_abi(existingRepeater) ==
                                        winrt::get_abi(repeater);
                             });
            if (existing == result.repeaters.end()) {
                result.repeaters.push_back(repeater);
            }
        } catch (...) {
            // A taskbar can be rebuilding during discovery. Failure of one
            // window must not skip the others.
        }
    }

    return result;
}

Controls::Grid DiscoverPrimaryTaskbarRootGrid() {
    HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
    XamlRoot xamlRoot = taskbarWnd ? GetTaskbarXamlRoot(taskbarWnd) : nullptr;
    auto root =
        xamlRoot ? xamlRoot.Content().try_as<FrameworkElement>() : nullptr;
    if (!root) {
        return nullptr;
    }

    if (root.Name() == L"RootGrid") {
        return root.try_as<Controls::Grid>();
    }

    return FindDescendantByName(root, L"RootGrid").try_as<Controls::Grid>();
}

void CleanupAllTaskbarsForUnload() {
    // Stop every callback source before touching the visual tree. A queued
    // callback is still harmless because every asynchronous reconcile entry
    // checks g_unloading.
    ClearAnimationTrackingForUnload();

    std::vector<Controls::Grid> rootsToSweep;
    auto addUniqueRoot = [&](Controls::Grid const& rootGrid) {
        if (!rootGrid) {
            return;
        }

        auto found = std::find_if(rootsToSweep.begin(), rootsToSweep.end(),
                                  [&](Controls::Grid const& existing) {
                                      return winrt::get_abi(existing) ==
                                             winrt::get_abi(rootGrid);
                                  });
        if (found == rootsToSweep.end()) {
            rootsToSweep.push_back(rootGrid);
        }
    };

    try {
        addUniqueRoot(DiscoverPrimaryTaskbarRootGrid());
    } catch (...) {
    }

    if (g_trackedTaskbars) {
        for (auto& taskbar : *g_trackedTaskbars) {
            // Restore only the margin value that we still own before removing
            // any visual elements or releasing weak references.
            RestoreTrackedButtonMargins(taskbar);

            try {
                auto rootGrid = taskbar.rootGrid.get();
                addUniqueRoot(rootGrid);
            } catch (...) {
            }

            try {
                auto repeater = taskbar.repeater.get();
                auto rootGrid =
                    repeater ? FindRootGridAncestor(repeater) : nullptr;
                addUniqueRoot(rootGrid);
            } catch (...) {
            }

            try {
                auto overlayCanvas = taskbar.overlayCanvas.get();
                if (overlayCanvas &&
                    IsOwnedOverlayName(overlayCanvas.Name())) {
                    auto overlayParent =
                        Media::VisualTreeHelper::GetParent(overlayCanvas)
                            .try_as<Controls::Grid>();
                    addUniqueRoot(overlayParent);
                }
            } catch (...) {
            }
        }
    }

    for (size_t rootIndex = 0; rootIndex < rootsToSweep.size(); rootIndex++) {
        try {
            SweepOwnedElementsFromPanel(rootsToSweep[rootIndex]);
        } catch (...) {
        }
    }

    // Normally the root sweeps removed these. This exact-name fallback also
    // covers a live tracked overlay whose parent isn't the stored root grid.
    if (g_trackedTaskbars) {
        for (auto& taskbar : *g_trackedTaskbars) {
            RemoveTrackedOverlayByExactName(taskbar);
        }

        // Do not release any tracked weak references or delegates until all
        // discoverable primary and secondary visual trees have been swept.
        for (auto& taskbar : *g_trackedTaskbars) {
            taskbar.repeater = {};
            taskbar.rootGrid = {};
            taskbar.overlayCanvas = {};
            taskbar.trayFrame = {};
            taskbar.animationPointerSource = {};
            taskbar.animationPointerMovedHandler = nullptr;
            taskbar.animationPointerExitedToken = {};
            taskbar.animationPointerHandlersAttached = false;
            taskbar.animationPointerExitedHandlerAttached = false;
            taskbar.reorderPointerSource = {};
            taskbar.reorderPointerPressedHandler = nullptr;
            taskbar.reorderPointerReleasedHandler = nullptr;
            taskbar.reorderPointerHandlerAttached = false;
            taskbar.reorderStructuralReconcilePending = false;
            taskbar.layoutMonitorPanel = {};
            taskbar.layoutUpdatedToken = {};
            taskbar.layoutMonitorAttached = false;
            ClearAnimationElementCache(taskbar);
        }
        g_trackedTaskbars->clear();
        DestroyTrackedTaskbars();
    }
}

struct ReconcileGuard {
    ~ReconcileGuard() { g_reconcilingTaskbars = false; }
};

ReconcileResult ReconcileTaskbarRepeater(FrameworkElement const& repeater,
                                         bool forceStructuralReconcile) {
    if (g_reconcilingTaskbars) {
        return ReconcileResult::temporarilyNotReady;
    }

    g_reconcilingTaskbars = true;
    ReconcileGuard guard;
    try {
        if (!repeater || repeater.Name() != L"TaskbarFrameRepeater" ||
            !repeater.Dispatcher().HasThreadAccess()) {
            return ReconcileResult::temporarilyNotReady;
        }

        auto* taskbar = TrackTaskbarRepeater(repeater);
        if (!taskbar) {
            return ReconcileResult::temporarilyNotReady;
        }

        return ReconcileTrackedTaskbar(*taskbar, repeater,
                                       forceStructuralReconcile);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Failed to reconcile dividers: 0x%08X %s",
               static_cast<unsigned int>(e.code().value), e.message().c_str());
    } catch (...) {
        Wh_Log(L"Failed to reconcile dividers with an unknown exception");
    }

    return ReconcileResult::temporarilyNotReady;
}

void ReconcileDividers(bool forceStructuralReconcile) {
    if (g_reconcilingTaskbars) {
        return;
    }

    g_reconcilingTaskbars = true;
    ReconcileGuard guard;

    try {
        PruneExpiredTrackedTaskbars();
        auto discovery = DiscoverCurrentThreadTaskbars();
        for (auto const& repeater : discovery.repeaters) {
            TrackTaskbarRepeater(repeater);
        }

        if (g_trackedTaskbars) {
            for (auto& taskbar : *g_trackedTaskbars) {
                try {
                    auto repeater = taskbar.repeater.get();
                    if (!repeater) {
                        continue;
                    }

                    if (!repeater.Dispatcher().HasThreadAccess()) {
                        continue;
                    }

                    ReconcileTrackedTaskbar(taskbar, repeater,
                                            forceStructuralReconcile);
                } catch (...) {
                }
            }
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Failed to reconcile dividers: 0x%08X %s",
               static_cast<unsigned int>(e.code().value), e.message().c_str());
    } catch (...) {
        Wh_Log(L"Failed to reconcile dividers with an unknown exception");
    }
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;

void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    TaskListButton_UpdateVisualStates_Original(pThis);

    if (g_unloading.load(std::memory_order_acquire)) {
        return;
    }

    try {
        void* taskListButtonIUnknownPtr = (void**)pThis + 3;
        winrt::Windows::Foundation::IUnknown taskListButtonIUnknown;
        winrt::copy_from_abi(taskListButtonIUnknown, taskListButtonIUnknownPtr);

        auto taskListButton = taskListButtonIUnknown.try_as<FrameworkElement>();
        if (taskListButton) {
            auto repeater = FindRepeaterAncestor(taskListButton);
            if (repeater) {
                ReconcileTaskbarRepeater(repeater, false);
            }
        }
    } catch (...) {
    }
}

bool RunReconcileOnTaskbarThread(bool enabled) {
    if (enabled && g_unloading.load(std::memory_order_acquire)) {
        return true;
    }

    struct RECONCILE_REQUEST {
        bool enabled;
        bool completed;
    };

    std::vector<HWND> taskbarUiWindows;
    for (HWND taskbarWnd : EnumerateCurrentProcessTaskbarWindows()) {
        HWND taskbarUiWnd = GetTaskbarDispatchWindow(taskbarWnd);
        DWORD processId = 0;
        DWORD threadId =
            taskbarUiWnd ? GetWindowThreadProcessId(taskbarUiWnd, &processId)
                         : 0;
        if (threadId != 0 && processId == GetCurrentProcessId()) {
            taskbarUiWindows.push_back(taskbarUiWnd);
        }
    }

    if (taskbarUiWindows.empty()) {
        if (enabled) {
            Wh_Log(L"Taskbar UI window not found for reconciliation");
        }
        return false;
    }

    auto reconcileProc = [](PVOID parameter) {
        try {
            auto* request = static_cast<RECONCILE_REQUEST*>(parameter);
            if (request->enabled &&
                g_unloading.load(std::memory_order_acquire)) {
                request->completed = true;
                return;
            }
            if (!request->enabled) {
                CleanupAllTaskbarsForUnload();
            } else {
                ReconcileDividers(true);
            }
            request->completed = true;
        } catch (...) {
        }
    };

    for (HWND taskbarUiWnd : taskbarUiWindows) {
        RECONCILE_REQUEST request{enabled, false};
        bool callbackRan =
            RunFromWindowThread(taskbarUiWnd, reconcileProc, &request);
        if (callbackRan && request.completed) {
            return true;
        }
    }

    if (enabled) {
        Wh_Log(
            L"Failed to run reconciliation synchronously on the taskbar UI "
            L"thread");
    }
    return false;
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
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
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols for taskbar.dll failed");
        return false;
    }

    return true;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
            &TaskListButton_UpdateVisualStates_Original,
            TaskListButton_UpdateVisualStates_Hook,
        },
    };

    if (!HookSymbols(module, taskbarViewHooks, ARRAYSIZE(taskbarViewHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}

BOOL ModInitWithTaskbarView(HMODULE taskbarViewModule) {
    return HookTaskbarViewDllSymbols(taskbarViewModule) ? TRUE : FALSE;
}

// Defined with the taskbar alignment code below. Declared here because the
// taskbar view can also be loaded after Wh_ModAfterInit, and that is the
// first moment dynamic centering can be applied in this process.
void ApplyDynamicCenteringSetting();

void HandleLoadedModuleIfTaskbarView(HMODULE module) {
    if (g_unloading.load(std::memory_order_acquire)) {
        return;
    }

    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        if (ModInitWithTaskbarView(module)) {
            Wh_ApplyHookOperations();
            ApplyDynamicCenteringSetting();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module);
    }
    return module;
}

// Dynamic centering only works on a left-aligned taskbar: center alignment
// stacks on top of the group and makes the reserved gap look wrong.
//
// The mod only *reads* the taskbar alignment (TaskbarAl). It never writes it and
// never changes what Explorer reads, so the user's own setting is always left
// alone; with a center-aligned taskbar dynamic centering is simply not applied.
static const wchar_t kTaskbarAdvancedKey[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";

// Missing value is reported as 1 (Windows 11 default = center).
bool ReadTaskbarAl(DWORD* outValue) {
    if (!outValue) {
        return false;
    }
    HKEY key = nullptr;
    LSTATUS status = RegOpenKeyExW(HKEY_CURRENT_USER, kTaskbarAdvancedKey, 0,
                                   KEY_QUERY_VALUE, &key);
    if (status != ERROR_SUCCESS) {
        return false;
    }
    DWORD taskbarAl = 1;
    DWORD size = sizeof(taskbarAl);
    status = RegQueryValueExW(key, L"TaskbarAl", nullptr, nullptr,
                              reinterpret_cast<LPBYTE>(&taskbarAl), &size);
    RegCloseKey(key);
    if (status != ERROR_SUCCESS) {
        taskbarAl = 1;
    }
    *outValue = taskbarAl;
    return true;
}

// Registry watch owned by the alignment monitor thread. Both handles are
// released by the destructor, including on every early return path.
struct TaskbarAlignWatch {
    HKEY key = nullptr;
    HANDLE changeEvent = nullptr;

    ~TaskbarAlignWatch() {
        if (key) {
            RegCloseKey(key);
        }
        if (changeEvent) {
            CloseHandle(changeEvent);
        }
    }

    bool Open() {
        LSTATUS status =
            RegOpenKeyExW(HKEY_CURRENT_USER, kTaskbarAdvancedKey, 0,
                          KEY_NOTIFY | KEY_QUERY_VALUE, &key);
        if (status != ERROR_SUCCESS) {
            Wh_Log(L"open Advanced key for notifications failed: %ld", status);
            key = nullptr;
            return false;
        }

        // Manual-reset event: it is reset before every re-registration, which
        // RegNotifyChangeKeyValue requires because its registration is
        // one-shot.
        changeEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!changeEvent) {
            Wh_Log(L"failed to create the registry change event");
            return false;
        }

        return true;
    }
};

// Called when the user changes the taskbar alignment away from left while
// dynamic centering is active. The mod does not fight the user: dynamic
// centering is turned off and the gap it reserved is dropped. The dividers
// exist only as the means of dynamic centering, so they go away with it: the
// reconcile run below removes them, and while g_dynamicCenteringActive is false
// no later rebuild can create them again. This runs on the alignment monitor
// thread, which no longer exits right after it, so it never joins itself.
void YieldDynamicCenteringForUserAlignment() {
    Wh_Log(L"the user changed the taskbar alignment; dynamic centering is off "
           L"and the dividers are removed");

    g_dynamicCenteringActive.store(false, std::memory_order_release);

    // The alignment value belongs to the user: this mod never wrote it, so
    // there is nothing to restore. One rebuild is enough to drop the reserved
    // dynamic gap and the dividers, and it goes through the usual reconcile
    // path.
    if (g_taskbarViewDllLoaded) {
        if (!RunReconcileOnTaskbarThread(false)) {
            Wh_Log(L"the dynamic gap could not be removed on the taskbar UI "
                   L"thread");
        }
    }
}

DWORD WINAPI TaskbarAlignMonitorThreadProc(LPVOID) {
    // Event-driven watch instead of registry polling: RegNotifyChangeKeyValue
    // reports a TaskbarAl change as soon as it happens, and the thread also
    // waits on the stop event, so unloading wakes it immediately instead of
    // leaving it in a long Sleep. The thread is resident: it keeps watching in
    // both directions instead of exiting after the first alignment change.
    TaskbarAlignWatch watch;
    if (!watch.Open()) {
        return 0;
    }

    // Let AfterInit settle before the first check.
    if (g_alignStopEvent) {
        WaitForSingleObject(g_alignStopEvent, 1000);
    } else {
        Sleep(1000);
    }

    HANDLE waitHandles[2] = {g_alignStopEvent, watch.changeEvent};

    while (!g_alignMonitorStop.load(std::memory_order_acquire) &&
           !g_unloading.load(std::memory_order_acquire)) {
        // The registration is one-shot, so it is renewed every iteration. It is
        // registered before the value is read: a change that lands between the
        // two still signals the event and is handled by the next iteration.
        ResetEvent(watch.changeEvent);

        // Only this key is watched: TaskbarAl lives in the key itself, not in a
        // subkey.
        LSTATUS status = RegNotifyChangeKeyValue(
            watch.key, FALSE, REG_NOTIFY_CHANGE_LAST_SET, watch.changeEvent,
            TRUE);
        if (status != ERROR_SUCCESS) {
            // A failed registration is not necessarily permanent, so the
            // resident monitor waits and retries instead of exiting for good.
            // Only the stop event ends this thread.
            Wh_Log(L"RegNotifyChangeKeyValue failed: %ld", status);
            if (WaitForSingleObject(g_alignStopEvent, 1000) != WAIT_TIMEOUT) {
                break;
            }
            continue;
        }

        // The monitor reacts in both directions. Leaving left turns dynamic
        // centering off (and removes the dividers); coming back to left turns it
        // on again and rebuilds the dividers. The registration above is one-shot
        // and is renewed by the next iteration, so the loop keeps waiting here
        // instead of exiting.
        DWORD taskbarAl = 0;
        if (ReadTaskbarAl(&taskbarAl)) {
            if (taskbarAl != 0) {
                if (g_dynamicCenteringActive.load(std::memory_order_acquire)) {
                    YieldDynamicCenteringForUserAlignment();
                }
            } else if (!g_dynamicCenteringActive.load(
                           std::memory_order_acquire)) {
                Wh_Log(L"the taskbar is left-aligned again; dynamic centering "
                       L"is applied again");
                g_dynamicCenteringActive.store(true, std::memory_order_release);
                if (g_taskbarViewDllLoaded &&
                    !RunReconcileOnTaskbarThread(true)) {
                    Wh_Log(L"the dividers could not be rebuilt on the taskbar "
                           L"UI thread");
                }
            }
        }

        // The stop event wakes this thread for unloading, and the change event
        // fires for a TaskbarAl change, which the next iteration handles. The
        // stop event is created before this thread, so both handles stay valid
        // for as long as it runs.
        DWORD waitResult =
            WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);
        if (waitResult != WAIT_OBJECT_0 + 1) {
            // The stop event, or a wait failure: either way this thread is done
            // and the cleanup belongs to the unload path.
            break;
        }
    }

    return 0;
}

void StartTaskbarAlignMonitor() {
    // Serialized against StopTaskbarAlignMonitor: the monitor is started on the
    // load/settings-change path and stopped on the unload path, and those can
    // run on different threads.
    std::lock_guard<std::mutex> lock(g_alignMonitorMutex);

    // Re-checked under the lock and before CreateThread: a start that raced with
    // the unload must lose, or the thread would outlive the mod.
    if (g_unloading.load(std::memory_order_acquire)) {
        return;
    }

    if (g_alignMonitorThread) {
        // The monitor exits on the stop event, so a stale handle is reaped here
        // instead of blocking a later settings change from starting a new
        // monitor.
        if (WaitForSingleObject(g_alignMonitorThread, 0) != WAIT_OBJECT_0) {
            return;
        }
        CloseHandle(g_alignMonitorThread);
        g_alignMonitorThread = nullptr;
    }

    g_alignMonitorStop.store(false, std::memory_order_release);
    if (!g_alignStopEvent) {
        g_alignStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!g_alignStopEvent) {
            Wh_Log(L"failed to create align stop event");
            return;
        }
    } else {
        ResetEvent(g_alignStopEvent);
    }

    g_alignMonitorThread =
        CreateThread(nullptr, 0, TaskbarAlignMonitorThreadProc, nullptr, 0,
                     nullptr);
    if (!g_alignMonitorThread) {
        Wh_Log(L"failed to start align monitor thread");
        CloseHandle(g_alignStopEvent);
        g_alignStopEvent = nullptr;
    }
}

void StopTaskbarAlignMonitor() {
    // Serialized with StartTaskbarAlignMonitor. The wait below is unbounded on
    // purpose, so an unloaded image can never leave a mod thread running.
    std::lock_guard<std::mutex> lock(g_alignMonitorMutex);

    g_alignMonitorStop.store(true, std::memory_order_release);
    if (g_alignStopEvent) {
        SetEvent(g_alignStopEvent);
    }

    if (g_alignMonitorThread) {
        // The monitor waits on the stop event, so this returns immediately; the
        // wait is unbounded so that an unloaded image can never leave a mod
        // thread running. A failed wait must not abandon a thread that may
        // still be running, so it is retried rather than returning with the
        // handle left set: the handle is closed only once the thread has
        // really exited.
        for (;;) {
            if (WaitForSingleObject(g_alignMonitorThread, INFINITE) ==
                WAIT_OBJECT_0) {
                break;
            }

            Wh_Log(L"waiting for the alignment monitor thread failed: %lu",
                   GetLastError());
            Sleep(1);
        }

        CloseHandle(g_alignMonitorThread);
        g_alignMonitorThread = nullptr;
    }

    if (g_alignStopEvent) {
        CloseHandle(g_alignStopEvent);
        g_alignStopEvent = nullptr;
    }
}

// Applies dynamic centering according to the current taskbar alignment. The mod
// never writes the alignment, so this is read-only: dynamic centering is applied
// when the taskbar is left-aligned, and it is not applied while it is centered.
// It is called on load and on every settings change, in the process that hosts
// the taskbar view.
void ApplyDynamicCenteringSetting() {
    if (g_unloading.load(std::memory_order_acquire)) {
        return;
    }

    DWORD taskbarAl = 1;
    if (!ReadTaskbarAl(&taskbarAl)) {
        Wh_Log(L"could not read the taskbar alignment; dynamic centering stays "
               L"inactive");
        g_dynamicCenteringActive.store(false, std::memory_order_release);
        // The monitor stays resident: a readable alignment value is picked up
        // without changing a setting or reloading the mod.
        StartTaskbarAlignMonitor();
        return;
    }

    if (taskbarAl != 0) {
        // The taskbar is center-aligned: Windows centers the whole button row
        // itself there, so dynamic centering is not applied, and with it no
        // dividers exist. The alignment setting is not changed; switching the
        // taskbar to Left alignment in Windows' taskbar settings is what enables
        // dynamic centering.
        Wh_Log(L"the taskbar is center-aligned; dynamic centering stays "
               L"inactive");
        g_dynamicCenteringActive.store(false, std::memory_order_release);
        // The monitor stays resident: it is what notices the taskbar being
        // switched back to left, which turns dynamic centering on again.
        StartTaskbarAlignMonitor();
        return;
    }

    // The taskbar is left-aligned: dynamic centering is applied, and the
    // monitor watches in both directions.
    // StartTaskbarAlignMonitor is a no-op while the thread is already running.
    g_dynamicCenteringActive.store(true, std::memory_order_release);
    StartTaskbarAlignMonitor();
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!ModInitWithTaskbarView(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto loadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
            kernelBaseModule, "LoadLibraryExW");
        if (!loadLibraryExW) {
            Wh_Log(L"Failed to resolve LoadLibraryExW");
            return FALSE;
        }

        WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_unloading.load(std::memory_order_acquire)) {
        return;
    }

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                if (ModInitWithTaskbarView(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    if (g_taskbarViewDllLoaded) {
        // Only the process that hosts the taskbar view applies dynamic
        // centering, so only it reads the taskbar alignment.
        ApplyDynamicCenteringSetting();
        RunReconcileOnTaskbarThread(true);
    }
}

void Wh_ModBeforeUninit() {
    // The unloading flag is raised first: a concurrent start of the alignment
    // monitor must see it and lose the race, and the monitor thread itself stops
    // at its next iteration.
    g_unloading.store(true, std::memory_order_release);
    g_dynamicCenteringActive.store(false, std::memory_order_release);

    // Stop the alignment monitor and wait for the thread to really exit:
    // this mod never writes the alignment, so there is nothing to put back.
    StopTaskbarAlignMonitor();

    if (g_taskbarViewDllLoaded) {
        // On success this stays synchronous, waiting for the taskbar UI
        // thread, until all tracked taskbars have been cleaned.
        if (!RunReconcileOnTaskbarThread(false)) {
            Wh_Log(
                L"Unload cleanup did not reach the taskbar UI "
                L"thread");
        }
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    if (g_taskbarViewDllLoaded && !g_unloading) {
        // The setting is re-resolved against the alignment on every
        // settings change, in the taskbar-view process only.
        ApplyDynamicCenteringSetting();
        RunReconcileOnTaskbarThread(true);
    }
}


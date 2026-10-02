// ==WindhawkMod==
// @id              taskbar-icon-group-centering
// @name            Taskbar Icon Group Centering
// @name:zh-CN      任务栏图标组居中
// @description     Center the taskbar icons after a chosen position as one group relative to the whole taskbar. Requires the taskbar to be set to Left alignment.
// @description:zh-CN 把任务栏中指定位置之后的图标作为一组相对整条任务栏居中。需要把任务栏对齐方式设为左对齐。
// @version         1.3.2
// @author          Suioio
// @github          https://github.com/Suioio
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -ladvapi32
// ==/WindhawkMod==

/* Copyright (C) 2026 digART
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This mod is extracted from Taskbar Separators Dynamic Centering, a fork of
 * Taskbar Separators by digART, and the original mod and its copyright belong
 * to digART: https://github.com/digart11/taskbar-separators
 *
 * Taskbar hook and UI-thread infrastructure includes code and patterns adapted
 * from Windhawk mods by Michael Maltsev (m417z), including Taskbar Labels for
 * Windows 11, Taskbar Multirow, and Windows 11 Taskbar Styler.
 */

// ==WindhawkModReadme==
/*

# Taskbar Icon Group Centering

Center the taskbar icons after a chosen position as one group, relative to the
whole taskbar. There is no line, dot or shape anywhere: the mod only measures
the buttons and writes the margins that move the group, and it adds no element
of its own to the taskbar.

## Preview

![Taskbar Icon Group Centering preview](https://raw.githubusercontent.com/Suioio/taskbar-separators-dynamic-centering/main/preview.png)

![The icon group after the chosen position re-centers as apps open and close](https://raw.githubusercontent.com/Suioio/taskbar-separators-dynamic-centering/main/dynamic-centering.gif)

## What it does

- The icons after **Icon group position** form a single group that is centered
  relative to the entire taskbar, including the system tray.
- The group is clamped so it cannot run into the tray: when centering would
  push it under the tray, the gap is reduced instead.
- **Stop centering when the icon count after the position reaches** stops
  centering once that many icons are after the position, which restores the
  normal left-aligned layout. The default 0 never stops.
- The mod only adds its own delta on top of the button margins that already
  exist, and hands back exactly the value it still owns when centering stops,
  so margins written by Windows or by another mod are never overwritten.

## When it applies

Only while the taskbar is **left-aligned**, **horizontal** and **not mirrored
(right-to-left)**. On a centered taskbar Windows centers the whole row itself
and this mod has nothing to add: the gap it computes there comes out as zero,
so no margin is written. If no getter has produced a value and the stored one
cannot be read either, the mod assumes left and does the same work with the same
zero result instead of going quiet. Nothing is applied on a
vertical or mirrored taskbar, and the mod never writes the taskbar alignment or
any other Windows setting.

To use it, switch the taskbar to **Left** alignment in Windows' taskbar
settings. Whether the taskbar is left-aligned is the value the shell's own
alignment getter returns, which this mod hooks, and which is seeded once from
the stored setting when the mod is loaded after the shell has already read it -
so switching the alignment is picked up in both directions on the fly, without
a mod reload. Because that getter sits above the stored value, the mod follows
the alignment **the shell itself reads**. The one seed covers the case where
the shell has already read it before the mod was loaded. There is no polling
and no thread of its own: the layout is watched through an event the shell
already raises, a switch to the alignment is followed inside a bounded settling
window instead of being polled for, and where the getter could not be hooked
the stored value is re-read at most once per second.

## Differences from the neighbouring mods

- **taskbar-separators** groups icons by *drawing separators* between them.
  This mod draws nothing at all; it only moves the icons after the position.
- **taskbar-split** inserts flexible space between the running and the pinned
  group. This mod centers a single group whose position you pick.
- **taskbar-centered-start-split-icons** splits the buttons into two groups
  around a **centered Start button**. This mod never touches the Start button,
  and the position is a plain setting rather than a consequence of Start.

## Settings

| Setting | Default | Meaning |
| --- | --- | --- |
| Icon group position | 2 | 1-based position counted from the first application button (the Start button is not counted). The icons *after* this position are the centered group. |
| Stop centering when the icon count after the position reaches | 0 | Stop centering once this many usable application buttons are after the position, which restores the normal left-aligned layout. The default 0 never stops. |

## Compatibility

- Windows 11, horizontal taskbar, x86-64
- Compatible with Windows 11 Taskbar Styler in normal configurations
- Taskbar labels, and uncombined or otherwise variable-width taskbar buttons
- Mixed multi-monitor layouts with different button modes on each taskbar
- The mod adds no element to the taskbar at all: it only writes button margins,
  and unload hands every tracked margin back, so nothing of the mod is left
  behind
- Layered margin mods: any mod that rewrites taskbar button margins by reading
  the current value and adding its own gap on top compounds with this one - both
  ledgers keep adding to each other without bound, so do not run two of those at
  once. A mod that writes an **absolute** margin is fine: this mod re-bases on
  top of whatever it finds. Taskbar Separators is the layering kind; its default
  Divider gap is 0, in which case it writes no button margins and the two
  coexist, but raise that gap and the two should not be enabled together.

## Uninstall

Disabling or removing the mod stops centering and hands back the button margins
it applied. The taskbar alignment is never written, so there is nothing to
restore.

## License and attribution

Licensed under the GNU General Public License v3.0.

This mod is extracted from **Taskbar Separators Dynamic Centering**, which is a
fork of **Taskbar Separators** by digART; the original mod and its copyright
belong to digART. The centering geometry in this mod is kept from that fork.

Taskbar hook and UI-thread infrastructure includes code and patterns adapted
from Windhawk mods by Michael Maltsev (m417z), including Taskbar Labels for
Windows 11, Taskbar Multirow, and Windows 11 Taskbar Styler.

## Known limitations

- Closing a pinned-release icon that sits exactly on the split — the last icon of
  the left group or the first icon of the centered group — removes a button the
  mod is measuring, so the spacing can look one layout pass stale for a few tens
  of milliseconds before it settles. The split is anchored on that button itself
  rather than on its position in the realized list, so a button appearing or
  disappearing anywhere else no longer moves it, and the anchor is relearned only
  once the boundary button has really been gone for a quarter of a second.
- While an icon is being dragged, Explorer draws the dragged icon as a floating
  copy that follows the cursor. Holding it inside the spacing makes the spacing
  look like two halves with the icon in between. The margins the mod writes are
  unaffected: the spacing returns as soon as the icon is dropped.
- The taskbar hooks rely on Windows component symbols, vtable slots and a
  machine-code pattern. A Windows update that changes them can stop the mod from
  working; a symbol that fails to resolve and every failed reconciliation are
  logged. The alignment is read from the two getters Windows has served it
  from and from the stored value; a build that removes or changes all three
  stops the mod from centering.
- If unload cannot detach its event handlers from the taskbar thread, those
  handlers stay registered and applied margins may not be handed back; the mod
  log records it. This limitation is shared with the mod it was derived from.

## 中文说明

把任务栏中**指定位置之后**的图标作为一组，相对**整条任务栏**（含系统托盘）
居中。画面里没有任何线条或方块：本模组只测量按钮并写入移动该组所需的边距，
不向任务栏添加任何元素。

### 功能

- **图标组位置**之后的图标构成一个整体，相对整条任务栏居中。
- 该组会被钳制，不会压到系统托盘：居中会把组推入托盘时，间距会被减小。
- **位置之后的图标数量达到该值时停止居中**：达到设定值后停止居中，恢复正常的
  左对齐排布。默认 0 表示永不停止。
- 本模组只在已有按钮边距之上叠加自己的增量，并在停止居中时只归还自己仍然持有的
  那个值，因此不会覆盖 Windows 或其它模组写入的边距。

### 生效条件

只在任务栏**左对齐**、**水平**且**非镜像（从右到左）**时生效。任务栏居中时整排按钮
由系统居中，本模组保持不生效；垂直任务栏与镜像任务栏上也不生效。模组从不写入任务栏
对齐设置或任何其它 Windows 设置。

请在 Windows 的任务栏设置里把对齐方式改为**左对齐**。是否左对齐取的是系统自身的
对齐 getter 返回值（本模组挂钩了它），并在模组晚于系统首次读取才启用时用存储的设置值
播种一次——因此切换对齐会被实时识别，**双向**响应，无需重新加载模组。由于该 getter
位于存储值之上，本模组跟随的是“shell 自己读到的对齐”。那次播种用于覆盖“shell
在本模组加载之前就已经读过该值”的情形。模组不轮询、也
不保留自己的线程：布局只
通过 shell 本就会触发的事件观察，切换对齐在有界的沉降窗口内跟随而不是靠轮询；没有
getter 产生过值时，存储值至多每秒回读一次；若连存储值也读不到，模组按左对齐处理：此时在真居中的任务栏上算出的间隙为 0，不会写入任何边距，只是仍会做同样的计算，而不是保持沉默。

### 与相邻模组的区别

- **taskbar-separators**：用**画分隔线**的方式把图标分组；本模组什么都不画，
  只移动指定位置之后的图标。
- **taskbar-split**：在“运行中”和“固定”两组之间插入弹性空白；本模组居中单个
  由你指定位置的组。
- **taskbar-centered-start-split-icons**：围绕**居中的开始按钮**把按钮分成两组；
  本模组完全不涉及开始按钮，位置只是一个普通设置项。

### 设置项

| 设置项 | 默认 | 说明 |
| --- | --- | --- |
| 图标组位置 | 2 | 从第一个应用按钮算起的 1 基位置（不含开始按钮）；该位置**之后**的图标构成居中的那一组。 |
| 位置之后的图标数量达到该值时停止居中 | 0 | 该位置之后的可用应用按钮数量达到该值时停止居中，恢复正常左对齐排布。默认 0 表示永不停止。 |

### 兼容性

- Windows 11、水平任务栏、x86-64
- 正常情况下与 Windows 11 Taskbar Styler 兼容
- 任务栏标签，以及不合并或其它可变宽度的任务栏按钮
- 多显示器下每条任务栏按钮模式不同的混合布局
- 模组不向任务栏添加任何元素，只写入按钮边距；卸载时会归还全部被跟踪的边距，
  不留下模组的任何痕迹
- 叠加式边距模组：任何“先读当前值、再把自己的间隙加上去”的模组都会与本模组互相累积，两套账本会无上限地叠加，因此不要同时启用两个这类模组。写**绝对值**边距的模组没有问题——本模组会自动在其之上重设基准。Taskbar Separators 属于叠加式：它的 Divider gap 默认为 0（此时不写按钮边距，可共存）；若把该值调大于 0，请勿同时启用两个模组。

### 卸载

停用或移除模组会停止居中并归还它写入的按钮边距。任务栏对齐设置从未被写入，
因此没有需要恢复的内容。

### 许可与署名

以 GNU General Public License v3.0 发布。

本模组抽取自 **Taskbar Separators Dynamic Centering**，后者是 digART 的
**Taskbar Separators** 的分叉；原模组及其版权归 digART 所有。本模组中的居中几何
代码取自该分叉。

任务栏挂钩与 UI 线程基础设施包含改编自 Michael Maltsev（m417z）模组的代码与模式，
包括 Taskbar Labels for Windows 11、Taskbar Multirow 与 Windows 11 Taskbar Styler。

### 已知限制

- 关闭正好位于分割点上的“未固定”图标（左组最后一个、或居中组第一个）时，被移除的正是模组正在测量的按钮，间距可能有一个布局帧的陈旧感（几十毫秒）后自行恢复正常。分割点锚定在该按钮**本身**、而不是它在“已实现列表”中的位置，因此其它位置出现或消失按钮都不会再让它移位；只有边界按钮确实消失约四分之一秒后才会重新锚定。
- 拖动图标时，Explorer 会把被拖图标做成跟随光标的浮动副本；把它停在空隙中间时，视觉上会像“空隙被劈成两半、中间夹着图标”。模组写入的间距并未改变，放下图标即恢复。
- 任务栏挂钩依赖 Windows 组件的符号、虚表槽位与一段机器码特征；Windows 更新若改变它们，模组可能失效，届时模组日志会记录重排失败。对齐值会同时从 Windows 曾提供的两个 getter 与存储值读取；若某个版本把这三者都改掉或去掉，模组就不再居中。
- 若卸载时无法从任务栏线程注销事件处理器，这些处理器会保留、已应用的边距可能未归还；模组日志会记录。这一限制与其来源模组相同。

*/
// ==/WindhawkModReadme==

// clang-format off
// ==WindhawkModSettings==
/*
- groupPosition: 2
  $name: Icon group position
  $name:zh-CN: 图标组位置
  $description: 1-based position counted from the first application button (the Start button is not counted). The icons after this position are centered as one group relative to the whole taskbar. Applied only on a left-aligned, horizontal taskbar and never on a mirrored, right-to-left one.
  $description:zh-CN: 从第一个应用按钮算起的 1 基位置（不含开始按钮）。该位置之后的图标作为一组相对整条任务栏居中。只在左对齐且水平的任务栏上生效，镜像（从右到左）的任务栏上不生效。
- stopCenteringIconCount: 0
  $name: Stop centering when the icon count after the position reaches
  $name:zh-CN: 位置之后的图标数量达到该值时停止居中
  $description: Stop centering once the number of icons after the position reaches this value, which restores the normal left-aligned layout. Those are all usable application buttons after the position. The default 0 never stops centering because of the icon count.
  $description:zh-CN: 位置之后的图标数量达到该值时停止居中，恢复正常左对齐排布。该数量指该位置之后所有可用应用按钮的数量。默认 0 表示永不因图标数量而停止居中。
*/
// ==/WindhawkModSettings==
// clang-format on

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <Windows.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/base.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

namespace {

enum class ReconcileResult {
    succeeded,
    temporarilyNotReady,
};

// The compiled-in defaults, in one place: LoadSettings falls back to them.
constexpr int kDefaultGroupPosition = 2;
// The count of icons after the position at which centering stops.
constexpr int kDefaultStopCenteringIconCount = 0;

// Read on the taskbar UI thread by every reconcile, so they are plain atomics
// instead of a value copied around under a lock; the generation counter is what
// tells a reconcile that the settings it applied are stale.
std::atomic<int> g_groupPosition{kDefaultGroupPosition};
std::atomic<int> g_stopCenteringIconCount{kDefaultStopCenteringIconCount};
std::atomic<unsigned int> g_settingsGeneration{0};

std::atomic<bool> g_taskbarViewDllLoaded{false};
std::atomic<bool> g_unloading{false};

// The alignment the shell itself reads: 0 = left, 1 = centered. Two getters
// feed it, TaskbarSettings::get_Alignment in taskbar.dll and TaskbarFrame::
// get_Alignment in Taskbar.View.dll, because Windows moved the value between
// builds. The starting value is 0 (left), not the Windows default of 1
// (centered): if no getter is found and the stored value cannot be read
// either, centering an already centered taskbar writes a gap of about zero.
std::atomic<int> g_taskbarAlignment{0};

// The stored alignment, re-read once per interval as a cross-check; a failed
// read leaves the starting value above in place, so the mod still applies.
static const wchar_t kTaskbarAdvancedKey[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";

// Seeds g_taskbarAlignment once from the stored value the shell reads, for the
// case where the shell read it before this mod was loaded. Only a successful
// read is adopted; a failed one leaves the starting value in place.
void SeedTaskbarAlignmentFromRegistry() {
    DWORD alignment = 1;
    DWORD size = sizeof(alignment);
    if (RegGetValueW(HKEY_CURRENT_USER, kTaskbarAdvancedKey, L"TaskbarAl",
                     RRF_RT_REG_DWORD, nullptr, &alignment, &size) ==
        ERROR_SUCCESS) {
        // This runs once a second while no getter has produced a value, so it
        // only logs a real change.
        const int previous = g_taskbarAlignment.exchange(
            static_cast<int>(alignment), std::memory_order_acq_rel);
        if (previous != static_cast<int>(alignment)) {
            Wh_Log(L"seed: stored alignment is %d",
                   static_cast<int>(alignment));
        }
    } else {
        static bool loggedMissing = false;
        if (!loggedMissing) {
            loggedMissing = true;
            Wh_Log(L"seed: no stored alignment; keeping the current value");
        }
    }
}

using AnimationClock = std::chrono::steady_clock;
constexpr auto kPostReleaseSettlingTimeout = std::chrono::seconds(1);
// How long the anchored boundary button may stay unrealized before the split is
// anchored somewhere else. The close animation that filters that button out
// lasts a few tens of milliseconds, while a button the user really closed does
// not come back.
constexpr auto kBoundaryAnchorMissingTimeout = std::chrono::milliseconds(250);

struct ReconciledButtonSignature {
    int itemIndex = -1;
    winrt::weak_ref<FrameworkElement> button;
    double actualWidth = 0;
    double actualHeight = 0;
    Thickness margin{};
    Visibility visibility = Visibility::Visible;
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
    double right = 0;
};

struct TrackedTaskbarState {
    size_t id = 0;
    winrt::weak_ref<FrameworkElement> repeater;
    winrt::weak_ref<Controls::Grid> rootGrid;

    // What the last reconcile applied: layoutLeftAligned is the raw alignment
    // verdict the hooked getter returned, and centeringApplies additionally
    // requires a horizontal, non-mirrored taskbar. The pointer handlers and the
    // margin ledger read centeringApplies, never the taskbar alignment setting.
    bool layoutLeftAligned = false;
    bool centeringApplies = false;
    // False until the first reconcile has decided the verdict, so that first
    // pass seeds it instead of reading as an alignment change and arming the
    // settle window for a taskbar that was simply not looked at yet.
    bool alignmentVerdictValid = false;
    bool loggedFirstGap = false;
    bool loggedNotReady = false;

    // B1: the split is anchored on the itemIndex of its left boundary button,
    // not on its position in the realized list. boundaryButtonIndex is where
    // that button sits in the realized list and in the margin ledger right now,
    // or -1 while there is no usable boundary.
    int boundaryButtonIndex = -1;
    int boundaryAnchorItemIndex = -1;
    unsigned int boundaryAnchorGeneration = 0;
    AnimationClock::time_point boundaryAnchorMissingSince{};

    // Set once an out-of-range group position has been reported, so the log
    // line names the problem when it appears instead of every frame.
    bool positionBeyondButtonsLogged = false;

    // Set while an alignment change is being followed: the row slides, so the
    // gap is recomputed per composition frame until the geometry stops moving.
    bool alignmentSettleActive = false;

    // Timestamp of the last fallback read of the stored alignment. It is only
    // used on builds where the alignment getter could not be hooked, and it is
    // what keeps that read off the per-frame path.
    AnimationClock::time_point alignmentFallbackReadAt{};

    // Cached SystemTray.SystemTrayFrame of this taskbar. The centering gap
    // clamps the centered group against the tray, and the lookup is a depth-12
    // visual-tree walk, so the element is resolved once and reused while it is
    // still under the same root grid.
    winrt::weak_ref<FrameworkElement> trayFrame;
    unsigned int appliedSettingsGeneration = 0;

    bool reconciliationSignatureValid = false;
    ReconcileResult cachedReconcileResult =
        ReconcileResult::temporarilyNotReady;
    winrt::weak_ref<FrameworkElement> reconciledRepeater;
    winrt::weak_ref<Controls::Grid> reconciledRootGrid;
    double reconciledRootWidth = 0;
    double reconciledRootHeight = 0;
    winrt::Windows::Foundation::Rect reconciledRepeaterBounds{};
    std::vector<ReconciledButtonSignature> reconciledButtons;

    // The gap is applied as a delta on top of the taskbar button margins that
    // already exist (Windows and/or another mod).
    std::vector<TrackedButtonMarginState> buttonMargins;

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

    // Wall-clock end of the post-release settling window. Only the release
    // path arms it: arming it from a successful reconcile would keep the
    // window permanently open while the user hovers or clicks the taskbar.
    AnimationClock::time_point postReleaseSettlingUntil{};

    // Layout-change monitor for the taskbar repeater panel.
    // Closing an app removes a button out from under the centering gap margins,
    // which halves the gap until a reconcile runs; LayoutUpdated fires on
    // any such relayout so the removed/re-added button count is detected
    // and a forced reconcile restores the spacing immediately.
    bool layoutMonitorAttached = false;
    winrt::event_token layoutUpdatedToken{};
    winrt::weak_ref<Controls::Panel> layoutMonitorPanel;
    // Reentrancy guard for the forced reconcile below, not a feedback-loop
    // bound: the flag is cleared before the LayoutUpdated caused by the margins
    // it wrote arrives, so that event does reach this handler. What bounds the
    // repeats is kMaxLayoutForcedReconciles.
    bool layoutForcedReconcileActive = false;
    // A guard older than the watchdog below is treated as stale: the clear is
    // skipped when the tracked state is pruned while the forced reconcile runs.
    AnimationClock::time_point layoutForcedReconcileActiveSince{};
    int layoutForcedReconcileAttempts = 0;
    // Start of the current budget window for the retries below, so a count that
    // stops changing cannot spend the retries for good.
    AnimationClock::time_point layoutForcedReconcileWindowStart{};
    size_t layoutObservedButtonCount = 0;
    bool layoutObservedButtonCountValid = false;
    // Last panel.Children().Size() seen by the LayoutUpdated handler, a cheap
    // pre-filter before the per-child ABI-crossing walk. The invalid marker
    // means the first pass always walks.
    size_t layoutPanelChildCount = static_cast<size_t>(-1);

    // Drag freeze state for the centering gap.
    bool reorderDragActive = false;
    bool hasFrozenDynamicGap = false;
    double frozenDynamicGap = 0;
    bool frozenCenteringGapInEffect = true;
    double lastAppliedDynamicGap = 0;
    // The tray clamp that came with the last successful measurement. A gap
    // that is reused while the live geometry cannot be measured is clamped
    // with it again, so a reused value can never exceed the last known tray
    // limit and quietly run the group into the tray.
    double lastMeasuredGapMax = 0;
    bool lastCenteringGapInEffect = true;
    // Consecutive rendering frames in which the released-left-button fallback
    // read the button as up. A single asynchronous false read must not end the
    // drag, or the gap is recomputed from drag-distorted geometry.
    int dragEndConfirmFrames = 0;

    // Remember which button is being dragged so the gap can be kept on the
    // correct side while dragging.
    winrt::weak_ref<FrameworkElement> draggedButton;
    int draggedButtonIndex = -1;
    // True while the dragged button is one of the two buttons that straddle
    // the dynamic centering boundary. Only such a drag keeps the frozen
    // margins; any other drag lets the live order drive the gap so Windows can
    // still match the button under the cursor. Unknown state stays true.
    bool draggedButtonIsBoundary = false;

    // The rendering callback owns the released-left-button check that ends a
    // drag whose PointerReleased the shell swallowed.
    winrt::event_token animationRenderingToken{};
    bool animationRenderingSubscribed = false;
};

using TrackedTaskbarCollection = std::vector<TrackedTaskbarState>;

// Accessed only from taskbar XAML/UI-thread callbacks. Keep the TLS object
// itself trivially destructible: normal unload cleanup explicitly destroys
// the collection on the taskbar UI thread, and no TLS destructor can run
// after this mod DLL has been unloaded.
thread_local TrackedTaskbarCollection* g_trackedTaskbars = nullptr;
thread_local size_t g_nextTrackedTaskbarId = 1;
thread_local bool g_reconcilingTaskbars = false;

ReconcileResult ReconcileTaskbarRepeater(FrameworkElement const& repeater,
                                         bool forceStructuralReconcile);

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

void LoadSettings() {
    // 1-based: the icons after this position are the centered group. A key that
    // is missing or not positive keeps the compiled-in default, and says so,
    // because a configured position of 0 would otherwise look like a setting
    // that is silently ignored.
    int groupPosition = Wh_GetIntSetting(L"groupPosition");
    if (groupPosition <= 0) {
        Wh_Log(L"group position %d is not positive; using the default %d",
               groupPosition, kDefaultGroupPosition);
        groupPosition = kDefaultGroupPosition;
    }
    g_groupPosition.store(groupPosition, std::memory_order_release);

    // 0 means never stop: the count check in CalculateDynamicCenteredGap is
    // skipped for 0, so centering keeps running.
    g_stopCenteringIconCount.store(
        std::max(0, Wh_GetIntSetting(L"stopCenteringIconCount")),
        std::memory_order_release);

    g_settingsGeneration.fetch_add(1, std::memory_order_release);
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

// Both getters record here: any thread stores, the UI-thread paths read. One
// line per real change, so a switch is visible in the log without spamming it.
void RecordTaskbarAlignment(int alignment, const wchar_t* source) {
    // The stored-value cross-check keeps running regardless, so a getter that
    // is only called occasionally cannot latch a stale value for the session.
    const int previous =
        g_taskbarAlignment.exchange(alignment, std::memory_order_acq_rel);
    if (previous != alignment) {
        Wh_Log(L"taskbar alignment changed to %d (from %s)", alignment,
               source);
    }
}

// Same technique as the taskbar-on-top mod: the alignment the shell itself
// reads is taken from the getter instead of being guessed from the layout.
// Windows has moved that getter between builds: taskbar.dll serves it as
// TaskbarSettings::get_Alignment, Taskbar.View.dll as TaskbarFrame::
// get_Alignment. Both are hooked as optional entries.
using ITaskbarSettings_get_Alignment_t = HRESULT(WINAPI*)(void* pThis,
                                                          int* alignment);
ITaskbarSettings_get_Alignment_t ITaskbarSettings_get_Alignment_Original;

HRESULT WINAPI ITaskbarSettings_get_Alignment_Hook(void* pThis,
                                                   int* alignment) {
    HRESULT ret = ITaskbarSettings_get_Alignment_Original(pThis, alignment);
    if (SUCCEEDED(ret)) {
        RecordTaskbarAlignment(*alignment, L"taskbar.dll");
    }
    return ret;
}

using TaskbarFrame_get_Alignment_t = HRESULT(WINAPI*)(void* pThis,
                                                      int* alignment);
TaskbarFrame_get_Alignment_t TaskbarFrame_get_Alignment_Original;

HRESULT WINAPI TaskbarFrame_get_Alignment_Hook(void* pThis, int* alignment) {
    HRESULT ret = TaskbarFrame_get_Alignment_Original(pThis, alignment);
    if (SUCCEEDED(ret)) {
        // Both getters record unconditionally: which one a build actually
        // calls is what decides, not which symbol happened to resolve, since
        // a build can export both and serve the live value through one.
        RecordTaskbarAlignment(*alignment, L"Taskbar.View.dll");
    }
    return ret;
}

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

// Established Taskbar Multirow pattern for Shell_SecondaryTrayWnd: the
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
    // The cap is what keeps this from walking forever, but it is also what
    // makes this path the fragile one: a taskbar tree that grew a level or
    // two would not be found here while the upward search still finds it,
    // and the mod would only come to life once a button is hovered.
    if (!root || depth > 20) {
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

// Measures an element in the coordinate space of any other element of the same
// tree, which is what the centering geometry needs: every value it combines is
// in root-grid coordinates, so the root grid is the reference.
bool TryGetElementBounds(FrameworkElement const& referenceFrame,
                         FrameworkElement const& element,
                         winrt::Windows::Foundation::Rect* bounds) {
    if (!referenceFrame || !element || !bounds) {
        return false;
    }

    double width = element.ActualWidth();
    double height = element.ActualHeight();
    if (!std::isfinite(width) || width <= 0 || !std::isfinite(height) ||
        height <= 0) {
        return false;
    }

    auto transformedBounds =
        element.TransformToVisual(referenceFrame)
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

bool IsUsableApplicationButton(FrameworkElement const& button) {
    if (!button || button.Visibility() != Visibility::Visible) {
        return false;
    }

    double width = button.ActualWidth();
    double height = button.ActualHeight();
    return std::isfinite(width) && width > 0 && std::isfinite(height) &&
           height > 0;
}

// Safety gate: a mirrored, right-to-left taskbar lays its buttons out along
// decreasing X, while the centering geometry is written for increasing X, so
// centering is not applied there instead of collapsing the gap silently.
bool IsMirroredPrimaryOrdering(FrameworkElement const& referenceFrame,
                               std::vector<FrameworkElement> const& elements) {
    bool sawQualifyingPair = false;

    // One measurement per button instead of one per end of every pair: each
    // button's centre is compared with the previous one's, so a structural
    // reconcile pays N transforms rather than 2(N-1).
    winrt::Windows::Foundation::Rect previousBounds{};
    bool previousValid = false;
    for (size_t index = 0; index < elements.size(); index++) {
        winrt::Windows::Foundation::Rect currentBounds{};
        if (!elements[index] ||
            !TryGetElementBounds(referenceFrame, elements[index],
                                 &currentBounds)) {
            previousValid = false;
            continue;
        }

        if (previousValid) {
            double primaryMovement =
                (currentBounds.X + currentBounds.Width / 2.0) -
                (previousBounds.X + previousBounds.Width / 2.0);
            double crossMovement =
                (currentBounds.Y + currentBounds.Height / 2.0) -
                (previousBounds.Y + previousBounds.Height / 2.0);
            if (std::isfinite(primaryMovement) &&
                std::isfinite(crossMovement) &&
                std::fabs(primaryMovement) > 0.1 &&
                std::fabs(primaryMovement) > std::fabs(crossMovement)) {
                // A single pair moving the other way is enough to disprove a
                // mirrored ordering, and it is deliberately not enough to
                // prove one: while a reorder animates, the repeater already
                // holds the new order while the buttons are still travelling,
                // so one pair can read as reversed for a frame even on a plain
                // left-to-right taskbar.
                if (primaryMovement > 0) {
                    return false;
                }

                sawQualifyingPair = true;
            }
        }

        previousBounds = currentBounds;
        previousValid = true;
    }

    // No adjacent pair is laid out far enough to say anything yet, so the
    // ordering is not known to be mirrored and nothing is cleared.
    return sawQualifyingPair;
}

// The row runs left to right and the boundary gap is split over the two buttons
// that straddle it: leading picks the side of the button that faces the group.
void AddButtonGap(ButtonGapContribution* contribution,
                  bool leading,
                  double amount) {
    (leading ? contribution->left : contribution->right) += amount;
}

// Centering: compute the physical gap needed to center the group that follows
// the boundary button relative to the whole taskbar. boundaryIndex is the
// position of the last button of the left group in appButtons, and
// buttonMargins is the base-margin ledger parallel to appButtons: the gap is
// written on top of each button's own base margin, so the boundary pair's base
// margins are part of the distance between the two groups.
double CalculateDynamicCenteredGap(
    Controls::Grid const& rootGrid,
    std::vector<FrameworkElement> const& appButtons,
    std::vector<TrackedButtonMarginState> const& buttonMargins,
    int boundaryIndex,
    bool* centeringGapInEffect,
    bool* measured,
    double* trayMaxGap,
    winrt::weak_ref<FrameworkElement>* trayFrameCache) {
    if (centeringGapInEffect) {
        *centeringGapInEffect = true;
    }
    // measured reports whether real geometry was read here, and trayMaxGap the
    // clamp that geometry produced. A zero answer therefore has two meanings
    // the caller has to tell apart: nothing could be measured (the settling
    // window may then reuse the last applied gap), or a real zero produced by
    // the centre position or by the tray clamp, which must never be replaced.
    if (measured) {
        *measured = false;
    }
    if (trayMaxGap) {
        *trayMaxGap = 0;
    }

    if (boundaryIndex < 0 || !rootGrid ||
        boundaryIndex >= static_cast<int>(appButtons.size())) {
        return 0;
    }

    int middleIconCount =
        static_cast<int>(appButtons.size()) - boundaryIndex - 1;
    const int stopCenteringIconCount =
        g_stopCenteringIconCount.load(std::memory_order_acquire);
    if (stopCenteringIconCount > 0 &&
        middleIconCount >= stopCenteringIconCount) {
        if (centeringGapInEffect) {
            *centeringGapInEffect = false;
        }
        return 0;
    }

    FrameworkElement leftButton = appButtons[boundaryIndex];
    if (!leftButton) {
        return 0;
    }

    // Both ends are measured in root-grid coordinates, the same space the gap
    // is applied in, so both are transformed against the root grid itself.
    winrt::Windows::Foundation::Rect leftBounds{};
    if (!TryGetElementBounds(rootGrid, leftButton, &leftBounds)) {
        return 0;
    }
    double leftGroupRight = leftBounds.X + leftBounds.Width;

    // The group's real extent, interior margins included: summing the button
    // widths would leave those margins out and place the group that much too
    // far right, and it under-counts the tray clamp below in the same way.
    double rightGroupWidth = 0;
    if (boundaryIndex + 1 < static_cast<int>(appButtons.size())) {
        winrt::Windows::Foundation::Rect firstBounds{};
        winrt::Windows::Foundation::Rect lastBounds{};
        if (!TryGetElementBounds(rootGrid, appButtons[boundaryIndex + 1],
                                 &firstBounds) ||
            !TryGetElementBounds(rootGrid, appButtons.back(), &lastBounds)) {
            return 0;
        }

        rightGroupWidth = (lastBounds.X + lastBounds.Width) - firstBounds.X;
    }

    if (rightGroupWidth <= 0) {
        // No app button follows the boundary: there is nothing to center, so
        // zero is the measured answer and not a failed measurement. Any other
        // non-positive width means the two measured edges crossed while the
        // row was animating, which stays a failed measurement.
        if (boundaryIndex + 1 >= static_cast<int>(appButtons.size())) {
            if (measured) {
                *measured = true;
            }
        }
        return 0;
    }

    double taskbarWidth = rootGrid.ActualWidth();
    if (!std::isfinite(taskbarWidth) || taskbarWidth <= 0) {
        return 0;
    }

    // The gap is written as base margin + gap on the two buttons that straddle
    // the boundary, so their own base margins already span part of the distance
    // between the two groups and must come off the gap; left in, they land the
    // group exactly that much right of centre. Zero for stock Windows margins,
    // which is why a theme that sets a button Margin is what shows it.
    double boundaryBaseMargins = 0;
    const size_t boundaryPosition = static_cast<size_t>(boundaryIndex);
    if (boundaryPosition + 1 < buttonMargins.size()) {
        const Thickness& leftBase = buttonMargins[boundaryPosition].baseMargin;
        const Thickness& rightBase =
            buttonMargins[boundaryPosition + 1].baseMargin;
        if (std::isfinite(leftBase.Right) && std::isfinite(rightBase.Left)) {
            boundaryBaseMargins = leftBase.Right + rightBase.Left;
        }
    }

    double desiredLeft = taskbarWidth / 2.0 - rightGroupWidth / 2.0;
    double desiredGap = desiredLeft - leftGroupRight - boundaryBaseMargins;
    if (desiredGap < 0) {
        desiredGap = 0;
    }

    // Avoid pushing the right group into the system tray. The clamp uses the
    // same measured extent as the centring above, so it is not optimistic about
    // how wide the group is.
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

    // The base margins of the boundary pair are part of this distance too, so
    // the tray clamp has to discount them the same way or it would allow a
    // group that overlaps the tray by exactly their sum.
    double maxGap =
        trayLeft - rightGroupWidth - leftGroupRight - boundaryBaseMargins;
    if (maxGap < 0) {
        maxGap = 0;
    }

    // The only exit that read real geometry, so it is what reports the
    // measurement and the clamp that came with it.
    if (measured) {
        *measured = true;
    }
    if (trayMaxGap) {
        *trayMaxGap = maxGap;
    }

    return std::min(desiredGap, maxGap);
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

void InvalidateReconciliationSignature(TrackedTaskbarState& taskbar) {
    taskbar.reconciliationSignatureValid = false;
    taskbar.cachedReconcileResult = ReconcileResult::temporarilyNotReady;
    taskbar.reconciledRepeater = {};
    taskbar.reconciledRootGrid = {};
    taskbar.reconciledButtons.clear();
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
}

// The rendering callback owns the only check that can end a drag whose
// PointerReleased the shell swallowed, so it must stay subscribed while the
// gap is frozen: dropping it here would kill that check and leave the freeze
// in place until the next press and release.
void OnAnimationRendering(size_t taskbarId,
                          winrt::Windows::Foundation::IInspectable const&,
                          winrt::Windows::Foundation::IInspectable const&) {
    auto* taskbarState = FindTrackedTaskbarById(taskbarId);
    if (!taskbarState) {
        return;
    }
    auto& taskbar = *taskbarState;

    if (g_unloading) {
        UnsubscribeAnimationRendering(taskbar);
        return;
    }

    // The shell's drag can swallow PointerReleased; a released left button is
    // therefore treated as the end of the drag so the final order is always
    // reconciled. GetAsyncKeyState is asynchronous and can read the button as
    // up while the drag is still live, so three consecutive released frames are
    // required: one false read would end the drag and recompute the gap from
    // drag-distorted geometry, which visibly halves the spacing.
    if (taskbar.reorderDragActive) {
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
            taskbar.dragEndConfirmFrames = 0;
        } else if (++taskbar.dragEndConfirmFrames >= 3) {
            taskbar.reorderDragActive = false;
            taskbar.hasFrozenDynamicGap = false;
            taskbar.draggedButton = {};
            taskbar.draggedButtonIndex = -1;
            taskbar.draggedButtonIsBoundary = false;
            taskbar.dragEndConfirmFrames = 0;
            taskbar.postReleaseReconcileFrames = 2;
            // Arm the same settling window here: this is the release path used
            // when the shell swallows PointerReleased.
            taskbar.postReleaseSettlingUntil =
                AnimationClock::now() + kPostReleaseSettlingTimeout;
            taskbar.reorderStructuralReconcilePending = true;
        }
    }

    const bool frozenDynamicGap =
        taskbar.reorderDragActive && taskbar.hasFrozenDynamicGap;

    if (taskbar.reorderStructuralReconcilePending) {
        taskbar.reorderStructuralReconcilePending = false;
        try {
            auto repeater = taskbar.repeater.get();
            if (repeater) {
                // Running on the following composition frame gives the
                // repeater a chance to expose its final order before we reassign
                // the gaps. Don't keep a TrackedTaskbarState reference across
                // this call: reconciliation can prune/reallocate the
                // tracked-taskbar list. While forced frames remain, re-arm the
                // reconcile for the next frame so the gap follows that order.
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

        // Only drop the callback once the queued forced frames have run: the
        // release path queues two of them and the second is re-armed just
        // above, so testing the freeze alone would cut the remedy down to one
        // frame.

        // The alignment settle re-arms itself for every frame of its bounded
        // window: a switch slides the whole row, so the gap is recomputed from
        // the settled geometry and not from the geometry caught mid-slide. The
        // reconcile that sees the row stop moving clears the flag, and the
        // settling deadline ends it either way.
        auto* refreshed = FindTrackedTaskbarById(taskbarId);
        if (refreshed && refreshed->alignmentSettleActive) {
            if (AnimationClock::now() >= refreshed->postReleaseSettlingUntil) {
                refreshed->alignmentSettleActive = false;
            } else {
                refreshed->reorderStructuralReconcilePending = true;
            }
        }
        if (refreshed && !frozenDynamicGap &&
            refreshed->postReleaseReconcileFrames == 0 &&
            !refreshed->reorderStructuralReconcilePending) {
            UnsubscribeAnimationRendering(*refreshed);
        }
        return;
    }

    // Nothing is measured per frame any more, so this callback has one job
    // left: stay alive while the freeze needs the released-button check above,
    // and drop itself once the freeze is over, no alignment settle is pending
    // and the settling window has closed.
    if (!frozenDynamicGap && !taskbar.reorderDragActive &&
        !taskbar.alignmentSettleActive &&
        taskbar.postReleaseSettlingUntil != AnimationClock::time_point{} &&
        AnimationClock::now() >= taskbar.postReleaseSettlingUntil) {
        UnsubscribeAnimationRendering(taskbar);
    }
}

void EnsureGeometryRenderingSubscribed(TrackedTaskbarState& taskbar) {
    if (taskbar.animationRenderingSubscribed) {
        return;
    }

    size_t taskbarId = taskbar.id;
    try {
        taskbar.animationRenderingToken = Media::CompositionTarget::Rendering(
            [taskbarId](
                winrt::Windows::Foundation::IInspectable const& sender,
                winrt::Windows::Foundation::IInspectable const& args) {
                OnAnimationRendering(taskbarId, sender, args);
            });
        taskbar.animationRenderingSubscribed = true;
    } catch (...) {
        // The taskbar can be tearing down. The press/release handlers then
        // keep the freeze on their own, and the next press retries this.
    }
}

void OnReorderPointerPressed(
    size_t taskbarId,
    winrt::Windows::Foundation::IInspectable const&,
    Input::PointerRoutedEventArgs const& e) {
    auto* taskbarState = FindTrackedTaskbarById(taskbarId);
    if (!taskbarState || g_unloading.load(std::memory_order_acquire)) {
        return;
    }

    if (taskbarState->reorderDragActive) {
        return;
    }

    // A press that did not land on a TaskListButton - Start, the clock, the
    // tray - can never reorder buttons, so it must neither freeze the gap nor
    // force a reconcile: doing that on every click rewrote both boundary
    // margins once on press (the whole gap on one side) and again on release
    // (two separately snapped halves), which is enough to twitch the group by
    // one physical pixel.
    auto source = e.OriginalSource().try_as<FrameworkElement>();
    auto pressedButton = FindAncestorByName(source, L"TaskListButton");
    if (!pressedButton) {
        return;
    }

    // A press that did not come from the left button can never reorder: a
    // right-click opens the context menu and a middle-click opens a new
    // instance, so the freeze, the forced reconcile and the rendering
    // subscription are all skipped for those clicks.
    if (!e.GetCurrentPoint(nullptr).Properties().IsLeftButtonPressed()) {
        return;
    }

    // Freeze the gap for the drag. A normal click is short enough that the
    // deferred reconcile on release is not noticeable.
    taskbarState->reorderDragActive = true;
    taskbarState->hasFrozenDynamicGap = true;
    taskbarState->frozenDynamicGap = taskbarState->lastAppliedDynamicGap;
    taskbarState->frozenCenteringGapInEffect =
        taskbarState->lastCenteringGapInEffect;

    // Drop a release-scheduled reconcile left over from a previous drag: the
    // forced reconcile below covers the current order anyway, and leaving the
    // flag set would let this drag's first rendering frame consume it as if it
    // belonged to the previous release.
    taskbarState->reorderStructuralReconcilePending = false;

    // Remember which TaskListButton was pressed so the gap can be kept on the
    // correct side while dragging.
    taskbarState->draggedButton = winrt::make_weak(pressedButton);
    taskbarState->draggedButtonIndex = -1;
    if (auto repeater = taskbarState->repeater.get()) {
        taskbarState->draggedButtonIndex =
            ItemsRepeater_GetElementIndex(repeater, pressedButton);
    }

    // Only a drag of the two buttons that straddle the centering boundary keeps
    // the frozen margins. Any other button releases the freeze so the gap
    // follows the live order and Windows can still match the button under the
    // cursor; an unidentified button, or no actively applied centering, falls
    // back to the old freeze instead of risking the spacing.
    bool pressedButtonIdentified = false;
    bool pressedButtonIsBoundary = false;
    if (taskbarState->centeringApplies &&
        taskbarState->boundaryButtonIndex >= 0) {
        const int boundaryIndex = taskbarState->boundaryButtonIndex;
        const auto& trackedButtons = taskbarState->reconciledButtons;
        for (size_t index = 0; index < trackedButtons.size(); index++) {
            auto trackedButton = trackedButtons[index].button.get();
            if (!trackedButton ||
                winrt::get_abi(trackedButton) !=
                    winrt::get_abi(pressedButton)) {
                continue;
            }
            pressedButtonIdentified = true;
            pressedButtonIsBoundary =
                static_cast<int>(index) == boundaryIndex ||
                static_cast<int>(index) == boundaryIndex + 1;
            break;
        }
    }
    taskbarState->draggedButtonIsBoundary =
        !pressedButtonIdentified || pressedButtonIsBoundary;

    // The press-time forced reconcile is exempt from the drag freeze (see
    // ReconcileTrackedTaskbar), so it applies the frozen gap fully onto the
    // boundary button opposite the pressed one; that shape stays frozen for the
    // rest of the drag and is recomputed on release.
    if (auto repeater = taskbarState->repeater.get()) {
        ReconcileTaskbarRepeater(repeater, true);
    }

    // The released-button check runs in the rendering callback, so the
    // subscription is armed here as well: without it a swallowed
    // PointerReleased would leave the freeze in place until the next press and
    // release pair. The forced reconcile above prunes the tracked list and can
    // reallocate it, so the pointer taken at the top may dangle; resolve the
    // state again by id before it is used.
    if (auto* refreshed = FindTrackedTaskbarById(taskbarId)) {
        EnsureGeometryRenderingSubscribed(*refreshed);
    }
}

void OnReorderPointerReleased(size_t taskbarId,
                              winrt::Windows::Foundation::IInspectable const&,
                              Input::PointerRoutedEventArgs const&) {
    auto* taskbarState = FindTrackedTaskbarById(taskbarId);
    if (!taskbarState || g_unloading.load(std::memory_order_acquire)) {
        return;
    }

    // Cleared here as well as in the rendering fallback at the top of this
    // file, so both release paths leave the same state behind.
    taskbarState->reorderDragActive = false;
    taskbarState->hasFrozenDynamicGap = false;
    taskbarState->draggedButton = {};
    taskbarState->draggedButtonIndex = -1;
    taskbarState->draggedButtonIsBoundary = false;
    taskbarState->dragEndConfirmFrames = 0;
    taskbarState->reorderStructuralReconcilePending = true;
    // Force a couple of post-release reconciliation frames so the gap is
    // recomputed against the final button order.
    taskbarState->postReleaseReconcileFrames = 2;
    // Arm the settling window from the release path only. The frame counter
    // is decremented before every forced reconcile, so it is already 0 on
    // the second forced frame; this deadline is what still covers it.
    taskbarState->postReleaseSettlingUntil =
        AnimationClock::now() + kPostReleaseSettlingTimeout;
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
    // Reset the post-release reconcile frame counter and its settling window.
    taskbar.postReleaseReconcileFrames = 0;
    taskbar.postReleaseSettlingUntil = {};
    taskbar.reorderDragActive = false;
    taskbar.draggedButton = {};
    taskbar.draggedButtonIndex = -1;
    taskbar.draggedButtonIsBoundary = false;
    taskbar.dragEndConfirmFrames = 0;
    taskbar.hasFrozenDynamicGap = false;
    taskbar.alignmentSettleActive = false;
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

void RestoreTrackedButtonMargin(TrackedButtonMarginState& tracked);

// True when a tracked entry still holds our gap increment while its button is
// not one of the buttons that may carry the gap right now. Only the ledger is
// inspected: the weak reference of an entry that holds an increment is the one
// thing resolved, and no measurement or ABI-crossing call is made.
bool IsOrphanGapMarginEntry(TrackedButtonMarginState const& tracked,
                            FrameworkElement const* hosts,
                            size_t hostCount) {
    if (!tracked.hasAppliedMargin) {
        return false;
    }

    auto button = tracked.button.get();
    if (!button) {
        // The host element is gone, so the increment can never be handed back.
        return true;
    }

    const void* buttonAbi = winrt::get_abi(button);
    for (size_t index = 0; index < hostCount; index++) {
        if (hosts[index] && winrt::get_abi(hosts[index]) == buttonAbi) {
            return false;
        }
    }

    return true;
}

// Hands back every entry that is not allowed to hold the gap under the hosts
// passed in, and returns how many were handed back. This writes no new gap and
// measures nothing: it only returns a stale increment to Windows.
size_t RestoreOrphanGapMargins(TrackedTaskbarState& taskbar,
                               FrameworkElement const* hosts,
                               size_t hostCount) {
    size_t restored = 0;
    for (auto& tracked : taskbar.buttonMargins) {
        if (IsOrphanGapMarginEntry(tracked, hosts, hostCount)) {
            RestoreTrackedButtonMargin(tracked);
            restored++;
        }
    }

    return restored;
}

// The at most two ledger buttons the current shape may hold the gap on, read
// from the tracked entries themselves: their order is the realized order of the
// last reconcile, so the boundary positions need no measurement. Centering that
// is not in effect yields an empty set, which makes every tracked increment an
// orphan by construction.
size_t CollectLedgerGapHosts(TrackedTaskbarState const& taskbar,
                             FrameworkElement hosts[2]) {
    // Centering that is not in effect, or a boundary that is not anchored,
    // yields an empty set, which makes every tracked increment an orphan.
    if (!taskbar.centeringApplies || taskbar.boundaryButtonIndex < 0) {
        return 0;
    }

    const size_t buttonIndex =
        static_cast<size_t>(taskbar.boundaryButtonIndex);
    size_t hostCount = 0;
    if (buttonIndex < taskbar.buttonMargins.size()) {
        if (auto host = taskbar.buttonMargins[buttonIndex].button.get()) {
            hosts[hostCount++] = host;
        }
    }
    if (buttonIndex + 1 < taskbar.buttonMargins.size()) {
        if (auto host = taskbar.buttonMargins[buttonIndex + 1].button.get()) {
            hosts[hostCount++] = host;
        }
    }

    return hostCount;
}

// At most this many forced reconciles per observed realized-button count: the
// forced reconcile writes margins, which schedules another layout pass and can
// raise LayoutUpdated again while the count still does not match (for example
// while a removed button is still animating), so the handler is bounded instead
// of spinning.
constexpr int kMaxLayoutForcedReconciles = 3;

// The retries above are also budgeted by wall clock. A count that stops
// changing while the geometry probe keeps failing would otherwise spend the
// three attempts for good and leave this monitor dead until the next real
// button change; the next layout event after this much time starts a fresh
// budget instead.
constexpr auto kLayoutForcedReconcileWindow = std::chrono::milliseconds(1000);

// A flag left set by a forced reconcile whose clear never ran is treated as
// stale after this much time. It is orders of magnitude longer than the
// synchronous reconcile it guards, so a live guard is never cleared.
constexpr auto kLayoutForcedReconcileWatchdog = std::chrono::milliseconds(2000);

// Where the alignment getter could not be hooked, the stored value is re-read
// while the layout keeps changing, at most once per this interval: a layout
// storm cannot turn the fallback into a registry poll. A hooked getter reads
// nothing at all.
constexpr auto kAlignmentFallbackReadInterval = std::chrono::milliseconds(1000);

// Layout-change monitor for the taskbar repeater panel.
// Closing an app removes its button out from under the centering gap margins,
// and a removed button never raises UpdateVisualStates again, so no reconcile
// fires to restore them. LayoutUpdated fires after any real relayout, so the
// realized buttons are recounted there and a structural reconcile is forced
// whenever the count no longer matches the last committed set.
void OnTaskbarLayoutUpdated(
    size_t taskbarId,
    winrt::Windows::Foundation::IInspectable const&,
    winrt::Windows::Foundation::IInspectable const&) {
    auto* taskbarState = FindTrackedTaskbarById(taskbarId);
    if (!taskbarState || g_unloading.load(std::memory_order_acquire)) {
        return;
    }

    // Reentrancy guard only, not a feedback-loop bound: the flag is set while
    // the forced reconcile at the end of this handler runs, and is already
    // cleared by the time the LayoutUpdated caused by the margins it wrote
    // arrives, so that event does reach this handler. What bounds the repeats
    // it can cause is kMaxLayoutForcedReconciles.
    if (taskbarState->layoutForcedReconcileActive) {
        const auto activeSince = taskbarState->layoutForcedReconcileActiveSince;
        // The flag is normally cleared right after the forced reconcile below.
        // When the tracked state is pruned while that call runs, the clear
        // never happens and this guard would return here forever; treat a flag
        // that old as stale and fall through instead.
        if (activeSince == AnimationClock::time_point{} ||
            AnimationClock::now() - activeSince <
                kLayoutForcedReconcileWatchdog) {
            return;
        }
        taskbarState->layoutForcedReconcileActive = false;
        taskbarState->layoutForcedReconcileActiveSince = {};
        Wh_Log(L"cleared a stale layout-forced-reconcile guard");
    }

    // Cheap orphan check, placed before every early return below so a stale
    // half gap cannot be protected by them. It reads the margin ledger and the
    // counts this handler already remembers: no measurement and no ABI-crossing
    // walk, and the weak references are only resolved for entries that still
    // hold an increment. Inside a drag freeze it is skipped entirely, because
    // the frozen single-sided shape belongs to the drag and must not be touched.
    auto repeater = taskbarState->repeater.get();
    bool layoutInvariantDirty = false;
    if (!taskbarState->reorderDragActive) {
        FrameworkElement gapHosts[2]{nullptr, nullptr};
        const size_t gapHostCount =
            CollectLedgerGapHosts(*taskbarState, gapHosts);
        if (RestoreOrphanGapMargins(*taskbarState, gapHosts, gapHostCount) >
            0) {
            layoutInvariantDirty = true;
        }

        // The realized set this handler last saw is not the set the last
        // committed reconcile established, so the spacing on the buttons is
        // stale by definition and the cheap pre-filters below must not hide it.
        if (taskbarState->layoutObservedButtonCountValid &&
            taskbarState->layoutObservedButtonCount !=
                taskbarState->reconciledButtons.size()) {
            layoutInvariantDirty = true;
        }

        // The alignment can change with no button added or removed, so it is
        // read here ahead of the count-based pre-filters: a switch slides the
        // row, which raises LayoutUpdated for as long as it moves, and a value
        // that disagrees with the applied one is what forces the rebuild below.
        auto alignmentRootGrid = taskbarState->rootGrid.get();
        if (repeater && alignmentRootGrid) {
            // The hooked getter keeps this current, and the stored value is
            // re-read as a cross-check, budgeted to one read per interval so a
            // burst of layout passes cannot become a registry poll. A getter
            // that is only called occasionally must not switch that off: the
            // stored read is also what follows a mod that forces the value by
            // hooking RegGetValueW, the same call shape used here.
            if (taskbarState->alignmentFallbackReadAt ==
                    AnimationClock::time_point{} ||
                AnimationClock::now() - taskbarState->alignmentFallbackReadAt >=
                    kAlignmentFallbackReadInterval) {
                taskbarState->alignmentFallbackReadAt = AnimationClock::now();
                SeedTaskbarAlignmentFromRegistry();
            }

            const bool leftAligned =
                g_taskbarAlignment.load(std::memory_order_acquire) == 0;
            if (leftAligned != taskbarState->layoutLeftAligned) {
                layoutInvariantDirty = true;
            }
        }
    }

    // While a reorder drag is in flight the realized button set is
    // intentionally transient; the press/release handlers own that lifecycle.
    if (taskbarState->reorderDragActive) {
        return;
    }

    if (!repeater) {
        return;
    }

    auto panel = repeater.try_as<Controls::Panel>();
    if (!panel) {
        return;
    }

    // Cheap heuristic pre-filter: the walk below crosses the ABI twice per
    // child and LayoutUpdated fires for every layout pass, so a panel whose
    // child count is unchanged is left to the previous pass. A relayout that
    // removes one child and adds another in the same pass is therefore not
    // noticed, and the spacing stays stale until the next one; a forced
    // reconcile resets this memory, so the retry path is never blocked.
    const size_t panelChildCount =
        static_cast<size_t>(panel.Children().Size());
    if (!layoutInvariantDirty &&
        taskbarState->layoutPanelChildCount == panelChildCount) {
        return;
    }
    taskbarState->layoutPanelChildCount = panelChildCount;

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

    // Only a count differing from the last observed one is a new situation.
    if (!taskbarState->layoutObservedButtonCountValid ||
        taskbarState->layoutObservedButtonCount != realizedButtonCount) {
        taskbarState->layoutObservedButtonCount = realizedButtonCount;
        taskbarState->layoutObservedButtonCountValid = true;
    }

    if (!layoutInvariantDirty &&
        realizedButtonCount == taskbarState->reconciledButtons.size()) {
        return;
    }

    // A margin write cannot change the realized button count, so the
    // LayoutUpdated events this handler causes itself always see an unchanged
    // count and can never grow a new budget: at most kMaxLayoutForcedReconciles
    // attempts per kLayoutForcedReconcileWindow keeps the write -> layout ->
    // LayoutUpdated feedback loop bounded instead of spinning at frame rate.
    const auto now = AnimationClock::now();
    if (taskbarState->layoutForcedReconcileWindowStart ==
            AnimationClock::time_point{} ||
        now - taskbarState->layoutForcedReconcileWindowStart >=
            kLayoutForcedReconcileWindow) {
        taskbarState->layoutForcedReconcileAttempts = 0;
        taskbarState->layoutForcedReconcileWindowStart = now;
    }

    // A removed or added button left the centering gap margins stale; force a
    // structural reconcile to rebuild them.
    if (taskbarState->layoutForcedReconcileAttempts >=
        kMaxLayoutForcedReconciles) {
        return;
    }

    taskbarState->layoutForcedReconcileAttempts++;
    taskbarState->layoutForcedReconcileActive = true;
    taskbarState->layoutForcedReconcileActiveSince = now;
    // Forget the remembered panel child count, so the LayoutUpdated raised
    // by the margins we are about to write cannot take the cheap early
    // exit above; the bounded retries then still get their frames.
    taskbarState->layoutPanelChildCount = static_cast<size_t>(-1);
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
        UnsubscribeAnimationRendering(taskbar);
        try {
            DetachReorderPointerHandler(taskbar);
        } catch (...) {
        }
        try {
            DetachLayoutChangeMonitor(taskbar);
        } catch (...) {
        }
    }
}

void RestoreTrackedButtonMargin(TrackedButtonMarginState& tracked) {
    if (!tracked.hasAppliedMargin) {
        return;
    }

    try {
        auto button = tracked.button.get();
        if (!button) {
            // The element is gone, so the increment cannot be handed back any
            // more. Drop the bookkeeping anyway, so the entry is not reported
            // as an orphan by every later sweep.
            tracked.hasAppliedMargin = false;
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
    UnsubscribeAnimationRendering(taskbar);
    try {
        DetachLayoutChangeMonitor(taskbar);
    } catch (...) {
    }
    try {
        DetachReorderPointerHandler(taskbar);
    } catch (...) {
    }

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
    // Once per taskbar thread: a build where the root grid is not found under
    // that name prunes and re-adds every pass, and this line would flood.
    thread_local bool loggedTracking = false;
    if (!loggedTracking) {
        loggedTracking = true;
        Wh_Log(L"tracking a taskbar: id=%d rootGrid=%d",
               static_cast<int>(trackedTaskbars.back().id), rootGrid ? 1 : 0);
    }
    return &trackedTaskbars.back();
}

struct RealizedButtonSnapshot {
    int itemIndex = -1;
    FrameworkElement button{nullptr};
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
            std::fabs(gap.left) > 0.001 || std::fabs(gap.right) > 0.001;
        if (hasGap) {
            Thickness desired = tracked.baseMargin;
            desired.Left += gap.left;
            desired.Right += gap.right;

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

bool CanReuseReconciledTaskbar(TrackedTaskbarState& taskbar,
                               FrameworkElement const& repeater,
                               TaskbarReconciliationSnapshot const& snapshot,
                               unsigned int settingsGeneration) {
    if (!taskbar.reconciliationSignatureValid || !snapshot.signatureValid ||
        taskbar.appliedSettingsGeneration != settingsGeneration) {
        return false;
    }

    // A pure alignment change leaves every measured value identical, so the
    // alignment verdict is part of the reuse test as well: without it the light
    // path would answer with the cached result and the switch would only be
    // picked up by whichever layout event happens to arrive.
    if (taskbar.layoutLeftAligned !=
        (g_taskbarAlignment.load(std::memory_order_acquire) == 0)) {
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
    }

    return true;
}

void CommitReconciledTaskbar(
    TrackedTaskbarState& taskbar,
    FrameworkElement const& repeater,
    TaskbarReconciliationSnapshot const& snapshot,
    ReconcileResult result) {
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
        taskbar.reconciledButtons.push_back(
            {button.itemIndex, winrt::make_weak(button.button),
             button.actualWidth, button.actualHeight, button.margin,
             button.visibility});
    }
    taskbar.reconciliationSignatureValid = true;
}

// This is the only function that writes taskbar button margins. Callers must
// already be running on the taskbar XAML/UI thread.
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
            // Read once per reconcile: the reuse test, the boundary anchor
            // generation and the commit must all see the same generation, or a
            // settings change during the pass could be applied half way.
            const unsigned int settingsGeneration =
                g_settingsGeneration.load(std::memory_order_acquire);
            // UpdateVisualStates is a hot path. A matching weak/scalar
            // snapshot proves that nothing we measure has changed.
            if (!forceStructuralReconcile &&
                CanReuseReconciledTaskbar(taskbar, repeater, snapshot,
                                          settingsGeneration)) {
                return taskbar.cachedReconcileResult;
            }

            // While a reorder drag is in flight the live order is
            // drag-distorted (the dragged button becomes a floating ghost), so
            // any recomputation fights the drag animation. The press-time
            // forced reconcile is exempt: it applies the frozen gap fully on
            // the side opposite the pressed button, which keeps the visible
            // spacing intact when that button becomes the ghost.
            if (taskbar.reorderDragActive && taskbar.hasFrozenDynamicGap &&
                taskbar.draggedButtonIsBoundary &&
                !forceStructuralReconcile) {
                return taskbar.cachedReconcileResult;
            }

            InvalidateReconciliationSignature(taskbar);

            // The alignment settle ends once the row stops moving: the repeater
            // box and the root width are the two values a slide changes.
            // Compared against the last committed ones, which
            // InvalidateReconciliationSignature deliberately keeps.
            const bool geometryMoved =
                !LayoutRectMatches(taskbar.reconciledRepeaterBounds,
                                   snapshot.repeaterBounds) ||
                !LayoutScalarMatches(taskbar.reconciledRootWidth,
                                     snapshot.rootWidth);

            // Read once per reconcile as well, so every decision in this pass
            // sees the same position the snapshot was measured with.
            const int configuredGroupPosition =
                g_groupPosition.load(std::memory_order_acquire);

            std::vector<FrameworkElement> appButtons;
            appButtons.reserve(snapshot.buttons.size());
            for (auto const& button : snapshot.buttons) {
                appButtons.push_back(button.button);
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

            // Watch the repeater panel's layout so a button removed by closing
            // an app (which never raises UpdateVisualStates) is caught by the
            // realized-button count mismatch below and forces a structural
            // reconcile to restore the margins.
            if (auto repeaterPanel = repeater.try_as<Controls::Panel>()) {
                AttachLayoutChangeMonitor(taskbar, repeaterPanel);
            } else {
                DetachLayoutChangeMonitor(taskbar);
            }

            // Only the horizontal layout is supported. The gap is written on
            // the X axis and the tray clamp transforms a point's X, so the
            // vertical path this mod inherited was never reachable; the axis is
            // taken from the proportions the snapshot already carries.
            const bool horizontalLayoutValid =
                snapshot.signatureValid && std::isfinite(snapshot.rootWidth) &&
                std::isfinite(snapshot.rootHeight) &&
                snapshot.rootHeight > 0 &&
                snapshot.rootWidth >= snapshot.rootHeight;

            // Left alignment is the value the shell's own alignment getter
            // returned, which the hook on it keeps current and which Wh_ModInit
            // seeded once. No geometry is consulted, and a mod that forces left
            // alignment by hooking the stored value is honored, because that
            // getter sits above it.
            const bool leftAlignedDerived =
                g_taskbarAlignment.load(std::memory_order_acquire) == 0;

            // A mirrored, right-to-left taskbar lays its buttons out along
            // decreasing X while the centering geometry is written for
            // increasing ones, so centering is not applied there instead of
            // collapsing the gap silently. A drag in progress suppresses the
            // test as well, so it is never fed a layout that is mid-flight.
            const bool mirroredOrdering =
                leftAlignedDerived && horizontalLayoutValid &&
                !taskbar.reorderDragActive &&
                IsMirroredPrimaryOrdering(rootGrid, appButtons);

            // The direction of the row is deliberately not probed: on the only
            // layout this mod supports it runs left to right, so the gap goes on
            // the trailing side of the left boundary button and on the leading
            // side of the group. Probing it used to gate the whole feature, so a
            // taskbar whose adjacent button centres happened to agree within
            // 0.1 DIP got no centering at all.
            const bool centeringApplies = leftAlignedDerived &&
                                          horizontalLayoutValid &&
                                          !mirroredOrdering;

            // An invalid snapshot measured nothing, so this pass must leave the
            // verdict, the margin ledger and the handlers untouched: rewriting
            // the gap from it would hand every applied increment back and
            // collapse the spacing for one pass, and storing the verdict would
            // read as an alignment change once the snapshot is valid again. The
            // next pass retries, exactly like the other not-ready paths.
            if (!snapshot.signatureValid) {
                // Once per taskbar: this can repeat forever (no root grid, a
                // non-finite margin, a zero root width) and used to be silent.
                if (!taskbar.loggedNotReady) {
                    taskbar.loggedNotReady = true;
                    Wh_Log(L"reconcile: the layout is not ready yet");
                }
                return ReconcileResult::temporarilyNotReady;
            }

            // Once per taskbar and on every change: this is the verdict that
            // decides whether anything is written at all. Until now every way
            // of going inert without centring was silent.
            if (!taskbar.alignmentVerdictValid ||
                taskbar.centeringApplies != centeringApplies) {
                Wh_Log(L"verdict: leftAligned=%d horizontal=%d mirrored=%d "
                       L"centering=%d buttons=%u",
                       leftAlignedDerived ? 1 : 0, horizontalLayoutValid ? 1 : 0,
                       mirroredOrdering ? 1 : 0, centeringApplies ? 1 : 0,
                       static_cast<unsigned int>(appButtons.size()));
            }

            // Only a change against an already known verdict is a change: the
            // first pass on a left-aligned taskbar would otherwise arm the
            // settle window even though nothing is sliding.
            const bool alignmentChanged =
                taskbar.alignmentVerdictValid &&
                taskbar.centeringApplies != centeringApplies;
            taskbar.layoutLeftAligned = leftAlignedDerived;
            taskbar.centeringApplies = centeringApplies;
            taskbar.alignmentVerdictValid = true;

            // B1: the split follows the button that sat at the configured
            // position when the anchor was learned, not whatever realized
            // button carries that index now, so a button that is virtualized
            // away or filtered out for a frame cannot move the split onto its
            // neighbour.
            const int configuredBoundaryIndex = configuredGroupPosition - 1;
            int boundaryIndex = -1;
            for (size_t index = 0; index < snapshot.buttons.size(); index++) {
                if (snapshot.buttons[index].itemIndex ==
                    taskbar.boundaryAnchorItemIndex) {
                    boundaryIndex = static_cast<int>(index);
                    break;
                }
            }

            // The anchor is relearned from the configured position when the
            // position setting changed, when there is none yet, or when the
            // anchored button has been gone for longer than a close animation -
            // which is how "the user really closed it" is told apart from a
            // button that is only filtered out while it animates.
            bool relearnBoundary =
                taskbar.boundaryAnchorGeneration != settingsGeneration ||
                taskbar.boundaryAnchorItemIndex < 0;
            if (boundaryIndex >= 0) {
                taskbar.boundaryAnchorMissingSince =
                    AnimationClock::time_point{};
            } else if (!taskbar.reorderDragActive) {
                const auto now = AnimationClock::now();
                if (taskbar.boundaryAnchorMissingSince ==
                    AnimationClock::time_point{}) {
                    taskbar.boundaryAnchorMissingSince = now;
                } else if (now - taskbar.boundaryAnchorMissingSince >=
                           kBoundaryAnchorMissingTimeout) {
                    relearnBoundary = true;
                }
            }

            if (boundaryIndex < 0 || relearnBoundary) {
                // Without a usable anchor the configured position decides,
                // which is also the correct new boundary right after a button
                // before it was really removed.
                boundaryIndex = configuredBoundaryIndex;
                if (relearnBoundary) {
                    taskbar.boundaryAnchorItemIndex =
                        boundaryIndex >= 0 &&
                                boundaryIndex <
                                    static_cast<int>(snapshot.buttons.size())
                            ? snapshot.buttons[boundaryIndex].itemIndex
                            : -1;
                    taskbar.boundaryAnchorGeneration = settingsGeneration;
                    taskbar.boundaryAnchorMissingSince =
                        AnimationClock::time_point{};
                }
            }

            if (boundaryIndex < 0 ||
                boundaryIndex >= static_cast<int>(appButtons.size()) ||
                !appButtons[boundaryIndex]) {
                boundaryIndex = -1;
            }
            taskbar.boundaryButtonIndex = boundaryIndex;

            // A position past the last realized button produces no gap at all,
            // so it is reported once instead of leaving the user with a silent
            // no-op.
            if (configuredBoundaryIndex >=
                static_cast<int>(appButtons.size())) {
                if (!taskbar.positionBeyondButtonsLogged) {
                    taskbar.positionBeyondButtonsLogged = true;
                    Wh_Log(L"group position %d is past the last of %u "
                           L"application buttons; nothing to center",
                           configuredGroupPosition,
                           static_cast<unsigned int>(appButtons.size()));
                }
            } else {
                taskbar.positionBeyondButtonsLogged = false;
            }

            // Keep a stable base margin for each realized TaskListButton. The
            // physical gap is then added as our own delta on top of that base,
            // preserving margins supplied by Windows or other mods.
            SynchronizeTrackedButtonMargins(taskbar, appButtons);
            std::vector<ButtonGapContribution> gapContributions(
                appButtons.size());

            if (centeringApplies) {
                bool centeringGapInEffect = true;
                double fullGap = 0;
                if (taskbar.reorderDragActive &&
                    taskbar.hasFrozenDynamicGap) {
                    centeringGapInEffect =
                        taskbar.frozenCenteringGapInEffect;
                    fullGap = taskbar.frozenDynamicGap;
                } else {
                    bool gapMeasured = false;
                    double gapTrayMax = 0;
                    fullGap = CalculateDynamicCenteredGap(
                        rootGrid, appButtons, taskbar.buttonMargins,
                        boundaryIndex, &centeringGapInEffect, &gapMeasured,
                        &gapTrayMax, &taskbar.trayFrame);
                    if (gapMeasured) {
                        taskbar.lastMeasuredGapMax = gapTrayMax;
                    }
                    // Only a gap that could not be measured is replaced, and
                    // only inside the post-release settling window where the
                    // buttons are still animating. A real zero, from the centre
                    // position or from the tray clamp, is never replaced, and
                    // the reused value is clamped with the last measured tray
                    // limit so it cannot run into the tray either.
                    const bool postReleaseSettling =
                        taskbar.postReleaseReconcileFrames > 0 ||
                        (taskbar.postReleaseSettlingUntil !=
                             AnimationClock::time_point{} &&
                         AnimationClock::now() <
                             taskbar.postReleaseSettlingUntil);
                    if (centeringGapInEffect && !gapMeasured &&
                        postReleaseSettling) {
                        fullGap = std::min(taskbar.lastAppliedDynamicGap,
                                           taskbar.lastMeasuredGapMax);
                    }
                }
                taskbar.lastAppliedDynamicGap = fullGap;
                taskbar.lastCenteringGapInEffect = centeringGapInEffect;
                if (!taskbar.loggedFirstGap) {
                    taskbar.loggedFirstGap = true;
                    Wh_Log(L"first centring pass: buttons=%d boundary=%d "
                           L"centred=%d gap=%.2f",
                           static_cast<int>(appButtons.size()), boundaryIndex,
                           centeringGapInEffect ? 1 : 0, fullGap);
                }

                if (centeringGapInEffect && boundaryIndex >= 0) {
                    size_t buttonIndex = static_cast<size_t>(boundaryIndex);
                    if (buttonIndex < appButtons.size()) {
                        bool hasNext = buttonIndex + 1 < appButtons.size();
                        if (hasNext) {
                            // While a drag is in progress, put the full gap on
                            // whichever side of the boundary the dragged button
                            // is NOT currently on, so the spacing stays put no
                            // matter where the button is dragged.
                            bool isDragging =
                                taskbar.reorderDragActive &&
                                taskbar.hasFrozenDynamicGap;

                            if (isDragging) {
                                // Align the drag-side single gap to the physical
                                // pixel grid so that margin never leaves
                                // sub-pixel coordinates (measured ~0.7 DIP
                                // right-group shift when the button count
                                // rounding kicks in).
                                double alignedGap = SnapToPhysicalPixel(
                                    fullGap, GetRasterizationScale(rootGrid));
                                // Locate the dragged button in the current
                                // order; fall back to the index recorded at
                                // press time when it cannot be found.
                                int currentDraggedIndex = -1;
                                if (auto dragged =
                                        taskbar.draggedButton.get()) {
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
                                    // right of this boundary: give the full
                                    // gap to the left boundary button.
                                    AddButtonGap(
                                        &gapContributions[buttonIndex], false,
                                        alignedGap);
                                } else {
                                    // The dragged button is currently to the
                                    // left of this boundary (or is its left
                                    // boundary button): give the full gap to
                                    // the right boundary button.
                                    AddButtonGap(
                                        &gapContributions[buttonIndex + 1],
                                        true, alignedGap);
                                }
                            } else {
                                // Split the space across the two neighboring
                                // buttons, snapping each half to the physical
                                // pixel grid on its own. The halves can then
                                // miss fullGap by half a pixel, deliberately:
                                // sub-pixel margins are rounded differently by
                                // the layout engine for even and odd counts.
                                double halfLeft = fullGap / 2.0;
                                double scale = GetRasterizationScale(rootGrid);
                                halfLeft = SnapToPhysicalPixel(halfLeft, scale);
                                double halfRight = SnapToPhysicalPixel(
                                    fullGap - halfLeft, scale);
                                AddButtonGap(&gapContributions[buttonIndex],
                                             false, halfLeft);
                                AddButtonGap(&gapContributions[buttonIndex + 1],
                                             true, halfRight);
                            }
                        } else {
                            // The group starts after the final realized app
                            // button, so the whole gap goes on its trailing
                            // side.
                            AddButtonGap(&gapContributions[buttonIndex], false,
                                         fullGap);
                        }
                    }
                }
            }

            // A zero gap, a hidden group, or a position past the last realized
            // button produces a zero contribution and restores our previous
            // delta.
            ApplyTrackedButtonGapMargins(taskbar, appButtons, gapContributions,
                                         &snapshot);

            // Keep the margin ledger free of orphans: a tracked button may hold
            // our increment only while it is one of the buttons this shape puts
            // the gap on. This writes no new gap, it only hands a stale
            // increment back.
            FrameworkElement gapHosts[2]{nullptr, nullptr};
            size_t gapHostCount = 0;
            if (centeringApplies) {
                for (size_t index = 0;
                     index < gapContributions.size() && gapHostCount < 2;
                     index++) {
                    auto const& gap = gapContributions[index];
                    if (std::fabs(gap.left) > 0.001 ||
                        std::fabs(gap.right) > 0.001) {
                        gapHosts[gapHostCount++] = appButtons[index];
                    }
                }
            }
            RestoreOrphanGapMargins(taskbar, gapHosts, gapHostCount);

            // The invalid-snapshot case returned above, so reaching this point
            // means the pass really measured the layout.
            result = ReconcileResult::succeeded;

            // The pointer handlers belong to the root grid the margins are
            // written on, so they are attached whenever a root grid exists and
            // centering can matter for this taskbar.
            if (rootGrid && centeringApplies) {
                AttachReorderPointerHandler(taskbar, rootGrid);
            } else {
                DetachReorderPointerHandler(taskbar);
            }

            // A change of the applied alignment slides the whole row, so the
            // gap is rebuilt across a bounded window instead of once: the
            // rendering callback re-arms the forced reconcile for every frame
            // until the measured geometry stops moving, and the settling
            // deadline caps that window at one second.
            if (alignmentChanged && !taskbar.reorderDragActive) {
                taskbar.alignmentSettleActive = true;
                taskbar.postReleaseSettlingUntil =
                    AnimationClock::now() + kPostReleaseSettlingTimeout;
                taskbar.postReleaseReconcileFrames = 2;
                taskbar.reorderStructuralReconcilePending = true;
                EnsureGeometryRenderingSubscribed(taskbar);
            } else if (taskbar.alignmentSettleActive && !geometryMoved) {
                taskbar.alignmentSettleActive = false;
            }

            taskbar.appliedSettingsGeneration = settingsGeneration;
            CommitReconciledTaskbar(taskbar, repeater, snapshot, result);
        }
    } catch (winrt::hresult_error const& e) {
        InvalidateReconciliationSignature(taskbar);
        Wh_Log(L"Failed to reconcile the icon group: 0x%08X %s",
               static_cast<unsigned int>(e.code().value), e.message().c_str());
    } catch (...) {
        InvalidateReconciliationSignature(taskbar);
        Wh_Log(L"Failed to reconcile the icon group with an unknown exception");
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

void CleanupAllTaskbarsForUnload() {
    // Stop every callback source before touching the margins. A queued
    // callback is still harmless because every asynchronous reconcile entry
    // checks g_unloading.
    ClearAnimationTrackingForUnload();

    if (g_trackedTaskbars) {
        // Hand back only the margin value that we still own before removing
        // any scheduled callback or releasing weak references.
        for (auto& taskbar : *g_trackedTaskbars) {
            RestoreTrackedButtonMargins(taskbar);
        }

        // Do not release any tracked weak references or delegates until all
        // tracked margins have been handed back.
        for (auto& taskbar : *g_trackedTaskbars) {
            taskbar.repeater = {};
            taskbar.rootGrid = {};
            taskbar.trayFrame = {};
            taskbar.reorderPointerSource = {};
            taskbar.reorderPointerPressedHandler = nullptr;
            taskbar.reorderPointerReleasedHandler = nullptr;
            taskbar.reorderPointerHandlerAttached = false;
            taskbar.reorderStructuralReconcilePending = false;
            taskbar.layoutMonitorPanel = {};
            taskbar.layoutUpdatedToken = {};
            taskbar.layoutMonitorAttached = false;
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
        Wh_Log(L"Failed to reconcile the icon group: 0x%08X %s",
               static_cast<unsigned int>(e.code().value), e.message().c_str());
    } catch (...) {
        Wh_Log(L"Failed to reconcile the icon group with an unknown exception");
    }

    return ReconcileResult::temporarilyNotReady;
}

void ReconcileAllTaskbars(bool forceStructuralReconcile) {
    if (g_reconcilingTaskbars) {
        return;
    }

    g_reconcilingTaskbars = true;
    ReconcileGuard guard;

    try {
        PruneExpiredTrackedTaskbars();
        auto discovery = DiscoverCurrentThreadTaskbars();
        if (discovery.repeaters.empty() &&
            (!g_trackedTaskbars || g_trackedTaskbars->empty())) {
            // Once per process: discovery found nothing and nothing is
            // tracked, which is the case a log has to be able to name.
            static std::atomic<bool> loggedNoDiscovery{false};
            if (!loggedNoDiscovery.exchange(true, std::memory_order_acq_rel)) {
                Wh_Log(L"taskbar discovery found no repeater at all");
            }
        }
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
        Wh_Log(L"Failed to reconcile the icon group: 0x%08X %s",
               static_cast<unsigned int>(e.code().value), e.message().c_str());
    } catch (...) {
        Wh_Log(L"Failed to reconcile the icon group with an unknown exception");
    }
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;

// True while any taskbar tracked on this thread still holds a gap increment.
// Only ledger flags are read and no weak reference is resolved, so this stays a
// flag scan on the hot path below. It is deliberately a whole-thread question:
// which taskbar a button belongs to is only known after the ancestor walk this
// check exists to avoid.
bool AnyTrackedTaskbarHoldsAppliedMargin() {
    if (!g_trackedTaskbars) {
        return false;
    }

    for (auto const& taskbar : *g_trackedTaskbars) {
        for (auto const& tracked : taskbar.buttonMargins) {
            if (tracked.hasAppliedMargin) {
                return true;
            }
        }
    }

    return false;
}

void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    // Once per process: if this never appears in a log, the symbol resolved
    // but the hook is never hit, which otherwise looks like a working hook.
    static std::atomic<bool> loggedEntered{false};
    if (!loggedEntered.exchange(true, std::memory_order_acq_rel)) {
        Wh_Log(L"TaskListButton::UpdateVisualStates entered");
    }
    TaskListButton_UpdateVisualStates_Original(pThis);

    if (g_unloading.load(std::memory_order_acquire)) {
        return;
    }

    // Nothing is written while the taskbar is centered, and the switch back to
    // Left is caught by OnTaskbarLayoutUpdated, which compares the alignment
    // against the one the last reconcile applied and forces the rebuild. That
    // handler only exists once a taskbar is tracked, so this stops only when
    // this thread already tracks one; a taskbar that still holds an increment
    // keeps the full path.
    if (g_taskbarAlignment.load(std::memory_order_acquire) != 0 &&
        g_trackedTaskbars && !g_trackedTaskbars->empty() &&
        !AnyTrackedTaskbarHoldsAppliedMargin()) {
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
            } else {
                // Once per taskbar thread: this is the silent-inert case that
                // is otherwise impossible to tell from a working mod.
                thread_local bool loggedNoRepeater = false;
                if (!loggedNoRepeater) {
                    loggedNoRepeater = true;
                    Wh_Log(L"no taskbar repeater was found above the button");
                }
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
                ReconcileAllTaskbars(true);
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
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskbarSettings,struct winrt::WindowsUdk::UI::Shell::ITaskbarSettings>::get_Alignment(int *))"},
            &ITaskbarSettings_get_Alignment_Original,
            ITaskbarSettings_get_Alignment_Hook,
            true,  // Optional: Taskbar.View.dll serves this getter on newer builds.
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols for taskbar.dll failed");
        return false;
    }

    // The alignment getter is the one entry above that may be missing, because
    // Windows has moved it between builds. Marking it optional lets this call
    // succeed on such a build, so its symbol cache is persisted and no error is
    // logged. Taskbar.View.dll is where that value lives on newer builds; on
    // 26100.9445 it resolves there but the shell was not observed to call it,
    // so the stored value is re-read until a getter produces a value.
    if (ITaskbarSettings_get_Alignment_Original == nullptr) {
        Wh_Log(L"TaskbarSettings::get_Alignment is missing from taskbar.dll; "
               L"trying Taskbar.View.dll");
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
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Taskbar::ITaskbarFrame>::get_Alignment(int *))"},
            &TaskbarFrame_get_Alignment_Original,
            TaskbarFrame_get_Alignment_Hook,
            true,  // Optional: taskbar.dll serves the getter on older builds.
        },
    };

    if (!HookSymbols(module, taskbarViewHooks, ARRAYSIZE(taskbarViewHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    // Both getters have been attempted by now. Neither resolving is the one
    // case where the stored value is the only source, and the layout monitor
    // re-reads it until a getter actually produces a value.
    if (TaskbarFrame_get_Alignment_Original == nullptr &&
        ITaskbarSettings_get_Alignment_Original == nullptr) {
        Wh_Log(L"no taskbar alignment getter in taskbar.dll or Taskbar.View.dll; "
               L"treating the taskbar as left-aligned");
    }

    Wh_Log(L"view hooks: updateVisualStates=%d alignmentGetterResolved=%d",
           TaskListButton_UpdateVisualStates_Original != nullptr ? 1 : 0,
           TaskbarFrame_get_Alignment_Original != nullptr ? 1 : 0);

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

void HandleLoadedModuleIfTaskbarView(HMODULE module) {
    if (g_unloading.load(std::memory_order_acquire)) {
        return;
    }

    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        if (ModInitWithTaskbarView(module)) {
            Wh_ApplyHookOperations();
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

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();
    Wh_Log(L"init: settings loaded");

    // The hooks are installed when this function returns, so nothing the hook
    // stores can be overwritten by this read. The seed covers the one case the
    // hook cannot see: the shell read the alignment before this mod was
    // loaded. A failed read keeps the current value.
    SeedTaskbarAlignmentFromRegistry();

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
        // Only the process that hosts the taskbar view applies centering; the
        // reconcile reads the tracked alignment on the taskbar UI thread.
        RunReconcileOnTaskbarThread(true);
    }
}

void Wh_ModBeforeUninit() {
    // The unloading flag is raised first, so a queued callback or a later
    // reconcile sees it and stops.
    g_unloading.store(true, std::memory_order_release);

    if (g_taskbarViewDllLoaded) {
        // On success this stays synchronous, waiting for the taskbar UI
        // thread, until all tracked taskbars have been cleaned.
        if (!RunReconcileOnTaskbarThread(false)) {
            Wh_Log(L"Unload cleanup did not reach the taskbar UI thread");
        }
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    if (g_taskbarViewDllLoaded && !g_unloading) {
        // LoadSettings raises the settings generation, so the next reconcile
        // recomputes the gap from the new position on the taskbar UI thread.
        RunReconcileOnTaskbarThread(true);
    }
}

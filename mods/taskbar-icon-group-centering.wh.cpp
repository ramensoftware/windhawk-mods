// ==WindhawkMod==
// @id              taskbar-icon-group-centering
// @name            Taskbar Icon Group Centering
// @name:zh-CN      任务栏图标组居中
// @description     Center the taskbar icons after a chosen position as one group relative to the whole taskbar. Requires the taskbar to be set to Left alignment.
// @description:zh-CN 把任务栏中指定位置之后的图标作为一组相对整条任务栏居中。需要把任务栏对齐方式设为左对齐。
// @version         1.0.4
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
the buttons and writes the margins that move the group. It keeps one childless,
invisible canvas in the taskbar root purely as the coordinate reference for
those measurements, and removes it when the mod unloads.

## What it does

- The icons after **Icon group position** form a single group that is centered
  relative to the entire taskbar, including the system tray.
- The group is clamped so it cannot run into the tray: when centering would
  push it under the tray, the gap is reduced instead.
- **Hide when the icon count after the position is at least** stops centering
  once that many icons are after the position, which restores the normal
  left-aligned layout. A value of 0 never stops.
- The mod only adds its own delta on top of the button margins that already
  exist, and hands back exactly the value it still owns when centering stops,
  so margins written by Windows or by another mod are never overwritten.

## When it applies

Only while the taskbar is **left-aligned**, **horizontal** and **not mirrored
(right-to-left)**. The mod never writes the taskbar alignment or any other
Windows setting: it only reads the stored TaskbarAl value. On a centered
taskbar Windows centers the whole row itself and this mod stays inactive, and
nothing is applied on a vertical or mirrored taskbar either.

To use it, switch the taskbar to **Left** alignment in Windows' taskbar
settings. Switching the alignment is picked up on the fly, in both directions,
without a setting change or a mod reload.

**The mod goes by the stored TaskbarAl value.** If another mod forces left
alignment by hooking the read instead of changing that stored value - the
Taskbar Multirow mod does exactly that - this mod still sees a centered taskbar
and stays inactive.

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
| Icon group position | 4 | 1-based position counted from the first application button (the Start button is not counted). The icons *after* this position are the centered group. |
| Hide when the icon count after the position is at least | 11 | Stop centering once the number of icons after the position reaches this value. Those are all usable application buttons after the position. A value of 0 never stops. |

## Compatibility

- Windows 11, horizontal taskbar, x86-64
- Compatible with Windows 11 Taskbar Styler in normal configurations
- Taskbar labels, and uncombined or otherwise variable-width taskbar buttons
- Mixed multi-monitor layouts with different button modes on each taskbar
- The only element the mod adds is that childless, invisible measurement canvas,
  named after the mod id; unload removes it and hands every tracked margin back,
  so nothing of the mod is left behind
- Taskbar Separators: both mods rewrite taskbar button margins. Its default Divider gap is 0, in which case it writes no button margins and the two can coexist; if you raise that gap above 0, do not enable both mods at the same time, or the two margin ledgers will keep adding on top of each other. / 与 Taskbar Separators 同时使用时：两者都会改写任务栏按钮边距。它的 Divider gap 默认为 0（此时不写按钮边距，可共存）；若把该值调大于 0，请勿同时启用两个模组，否则两套边距账本会互相叠加。

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

## 中文说明

把任务栏中**指定位置之后**的图标作为一组，相对**整条任务栏**（含系统托盘）
居中。画面里没有任何线条或方块：本模组只测量按钮并写入移动该组所需的边距；它会在
任务栏根节点保留一个无子元素、不可见的画布，仅作为这些测量的坐标参照，并在模组
卸载时将它移除。

### 功能

- **图标组位置**之后的图标构成一个整体，相对整条任务栏居中。
- 该组会被钳制，不会压到系统托盘：居中会把组推入托盘时，间距会被减小。
- **位置之后的图标数量达到此值时停止居中**：达到设定值后停止居中，恢复正常的
  左对齐排布。设为 0 表示永不停止。
- 本模组只在已有按钮边距之上叠加自己的增量，并在停止居中时只归还自己仍然持有的
  那个值，因此不会覆盖 Windows 或其它模组写入的边距。

### 生效条件

只在任务栏**左对齐**、**水平**且**非镜像（从右到左）**时生效。模组从不写入任务栏
对齐设置或任何其它 Windows 设置，只读取注册表中存储的 TaskbarAl 值。任务栏居中时
整排按钮由系统居中，本模组保持不生效；垂直任务栏与镜像任务栏上也不生效。

请在 Windows 的任务栏设置里把对齐方式改为**左对齐**。切换对齐会被实时识别，**双向**
响应，无需改动设置或重新加载模组。

**模组以存储的 TaskbarAl 值为准。** 若有另一个模组通过 hook 读取来强制左对齐
（Taskbar Multirow 就是这么做的）而没有改动这个存储值，本模组仍会认为任务栏居中，
从而保持不生效。

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
| 图标组位置 | 4 | 从第一个应用按钮算起的 1 基位置（不含开始按钮）；该位置**之后**的图标构成居中的那一组。 |
| 位置之后的图标数量达到此值时停止居中 | 11 | 该位置之后的可用应用按钮数量达到此值时停止居中。设为 0 表示永不停止。 |

### 兼容性

- Windows 11、水平任务栏、x86-64
- 正常情况下与 Windows 11 Taskbar Styler 兼容
- 任务栏标签，以及不合并或其它可变宽度的任务栏按钮
- 多显示器下每条任务栏按钮模式不同的混合布局
- 模组唯一添加的元素就是这个以模组 id 命名的、无子元素且不可见的测量画布；卸载时
  会移除它并归还全部被跟踪的边距，不留下模组的任何痕迹
- Taskbar Separators：与它同时使用时，两者都会改写任务栏按钮边距。它的 Divider gap 默认为 0，此时不写按钮边距，可共存；若把该值调大于 0，请勿同时启用两个模组，否则两套边距账本会互相叠加。

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

### Known limitations / 已知限制

- Closing a pinned-release icon that sits exactly on the split — the last icon of the left group or the first icon of the centered group — can make the spacing flash for a few tens of milliseconds before it settles. The button is being removed from the taskbar tree while the mod measures it; the next layout pass writes the correct spacing again. / 关闭正好位于分割点上的“未固定”图标（左组最后一个、或居中组第一个）时，间距可能出现几十毫秒的闪烁后自行恢复正常。
- While an icon is being dragged, Explorer draws the dragged icon as a floating copy that follows the cursor. Holding it inside the spacing makes the spacing look like two halves with the icon in between. The margins the mod writes are unaffected: the spacing returns as soon as the icon is dropped. / 拖动图标时，Explorer 会把被拖图标做成跟随光标的浮动副本；把它停在空隙中间时，视觉上会像“空隙被劈成两半、中间夹着图标”。模组写入的间距并未改变，放下图标即恢复。
- The taskbar hooks rely on Windows component symbols, vtable slots and a machine-code pattern. A Windows update that changes them can stop the mod from working; the mod log records failed reconciliations in that case. / 任务栏挂钩依赖 Windows 组件的符号、虚表槽位与一段机器码特征；Windows 更新若改变它们，模组可能失效，届时模组日志会记录重排失败。
- If unload cannot detach its event handlers from the taskbar thread, those handlers stay registered and applied margins may not be handed back; the mod log records it. This limitation is shared with the mod it was derived from. / 若卸载时无法从任务栏线程注销事件处理器，这些处理器会保留、已应用的边距可能未归还；模组日志会记录。这一限制与其来源模组相同。

*/
// ==/WindhawkModReadme==

// clang-format off
// ==WindhawkModSettings==
/*
- groupPosition: 4
  $name: Icon group position
  $name:zh-CN: 图标组位置
  $description: 1-based position counted from the first application button, which is not counted itself. The icons after this position are centered as one group relative to the whole taskbar. Applied only on a left-aligned, horizontal taskbar and never on a mirrored, right-to-left one.
  $description:zh-CN: 从第一个应用按钮算起的 1 基位置（不含开始按钮）。该位置之后的图标作为一组相对整条任务栏居中。只在左对齐且水平的任务栏上生效，镜像（从右到左）的任务栏上不生效。
- hideWhenIconCountAtLeast: 11
  $name: Hide when the icon count after the position is at least
  $name:zh-CN: 位置之后的图标数量达到此值时停止居中
  $description: Stop centering once the number of icons after the position reaches this value. Those are all usable application buttons after the position. Set to 0 to never stop centering because of the icon count.
  $description:zh-CN: 位置之后的图标数量达到此值时停止居中。该数量指该位置之后所有可用应用按钮的数量。设为 0 表示永不因图标数量而停止居中。
*/
// ==/WindhawkModSettings==
// clang-format on

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <Windows.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/base.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <mutex>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

namespace {

enum class ReconcileResult {
    succeeded,
    temporarilyNotReady,
};

enum class TaskbarOrientation {
    horizontal,
    vertical,
};

enum class OrientationSetting {
    automatic,
    horizontal,
    vertical,
};

struct Settings {
    // Centering: the icons after this position are centered as one group. The
    // count field is read by CalculateDynamicCenteredGap, whose body is kept
    // from the fork this mod was extracted from.
    int groupPosition = 4;
    int hideWhenIconCountAtLeast = 11;
};

std::mutex g_settingsMutex;
Settings g_settings;
std::atomic<unsigned int> g_settingsGeneration{0};

std::atomic<bool> g_taskbarViewDllLoaded{false};
std::atomic<bool> g_unloading{false};

// Centering is applied only while the taskbar is left-aligned and horizontal.
// The alignment value (TaskbarAl) is watched through RegNotifyChangeKeyValue;
// the mod never writes it. The resident monitor reacts in both directions and
// stops on an event, so uninit never blocks on a long Sleep. The layout code
// must use g_dynamicCenteringActive rather than re-reading the alignment,
// because that flag says what is actually applied.
std::atomic<bool> g_alignMonitorStop{false};
std::atomic<bool> g_dynamicCenteringActive{false};
HANDLE g_alignMonitorThread = nullptr;
HANDLE g_alignStopEvent = nullptr;
// StartTaskbarAlignMonitor runs on the load/settings-change path (including any
// explorer thread through LoadLibraryExW_Hook) while StopTaskbarAlignMonitor
// runs on the unload path, so the monitor handles above are only touched under
// this mutex.
std::mutex g_alignMonitorMutex;

using AnimationClock = std::chrono::steady_clock;
constexpr auto kPostReleaseSettlingTimeout = std::chrono::seconds(1);

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
    double top = 0;
    double right = 0;
    double bottom = 0;
};

struct TrackedTaskbarState {
    size_t id = 0;
    winrt::weak_ref<FrameworkElement> repeater;
    winrt::weak_ref<Controls::Grid> rootGrid;

    // Coordinate reference for the locked geometry, which is written against a
    // canvas that shares the root grid origin. It carries no child and no
    // brush and is not hit-testable, so it is invisible by construction.
    winrt::weak_ref<Controls::Canvas> measurementFrame;

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
    // repeats it can cause is kMaxLayoutForcedReconciles, the cap on forced
    // reconciles per observed realized-button count (at most 3 per observed
    // count).
    bool layoutForcedReconcileActive = false;
    // When the flag above was set. A clear that never happens (the tracked
    // state is pruned while the forced reconcile runs) would otherwise disable
    // this monitor for good, so a flag older than the watchdog below is treated
    // as stale and cleared instead.
    AnimationClock::time_point layoutForcedReconcileActiveSince{};
    int layoutForcedReconcileAttempts = 0;
    // Start of the current budget window for the retries below. The budget is
    // counted per window as well as per observed realized-button count, so a
    // count that stops changing while the geometry probe keeps failing cannot
    // spend the retries for good.
    AnimationClock::time_point layoutForcedReconcileWindowStart{};
    size_t layoutObservedButtonCount = 0;
    bool layoutObservedButtonCountValid = false;
    // Last panel.Children().Size() seen by the LayoutUpdated handler, used as a
    // cheap pre-filter before the per-child ABI-crossing walk. The invalid
    // marker means the first pass always walks.
    size_t layoutPanelChildCount = static_cast<size_t>(-1);

    // Drag freeze state for the centering gap.
    bool reorderDragActive = false;
    bool hasFrozenDynamicGap = false;
    double frozenDynamicGap = 0;
    bool frozenDynamicGapVisible = true;
    double lastAppliedDynamicGap = 0;
    bool lastDynamicGapVisible = true;
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
    bool animationRenderingCallbackActive = false;
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
    Settings settings;

    // 1-based: the icons after this position are the centered group. A key
    // that is missing or not positive keeps the default built into Settings.
    int groupPosition = Wh_GetIntSetting(L"groupPosition");
    if (groupPosition > 0) {
        settings.groupPosition = groupPosition;
    }
    // 0 means never stop: the count check in CalculateDynamicCenteredGap is
    // skipped for 0, so centering keeps running.
    settings.hideWhenIconCountAtLeast =
        std::max(0, Wh_GetIntSetting(L"hideWhenIconCountAtLeast"));

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

// Name of the coordinate-reference canvas this mod adds to the taskbar root
// grid. It is suffixed with the mod id, so no other mod - and none of the
// separator mods, which match their own prefix - can claim or remove it.
constexpr WCHAR kMeasurementFrameName[] =
    L"WhIconGroupCenteringMeasure_" WH_MOD_ID;

// Detaches the measurement frame this mod created, and only that one: the name
// has to match exactly before anything is removed, and there is no child to
// sweep because this mod never puts anything in the frame.
void RemoveMeasurementFrame(TrackedTaskbarState& taskbar) {
    auto frame = taskbar.measurementFrame.get();
    taskbar.measurementFrame = {};
    if (!frame) {
        return;
    }

    try {
        if (frame.Name() != kMeasurementFrameName) {
            return;
        }

        auto parent =
            Media::VisualTreeHelper::GetParent(frame).try_as<Controls::Panel>();
        if (!parent) {
            return;
        }

        uint32_t index = 0;
        auto children = parent.Children();
        if (children.IndexOf(frame, index)) {
            children.RemoveAt(index);
        }
    } catch (...) {
    }
}

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

bool IsUsableApplicationButton(FrameworkElement const& button) {
    if (!button || button.Visibility() != Visibility::Visible) {
        return false;
    }

    double width = button.ActualWidth();
    double height = button.ActualHeight();
    return std::isfinite(width) && width > 0 && std::isfinite(height) &&
           height > 0;
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

// Centering: count the application buttons after the position (1-based
// position, so the group starts after that button).
int CountMiddleIcons(std::vector<FrameworkElement> const& appButtons,
                     int position) {
    if (position <= 0 || position >= static_cast<int>(appButtons.size())) {
        return 0;
    }

    return static_cast<int>(appButtons.size()) - position;
}

// Centering: compute the physical gap needed to center the right-side group
// relative to the whole taskbar.
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
    if (settings.hideWhenIconCountAtLeast > 0 &&
        middleIconCount >= settings.hideWhenIconCountAtLeast) {
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

void StopAllGeometryTracking(TrackedTaskbarState& taskbar) {
    UnsubscribeAnimationRendering(taskbar);
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
        StopAllGeometryTracking(taskbar);
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
        // frame. The queue itself is the bound here, not a time window: the
        // counter is only re-armed while it is still above zero.
        auto* refreshed = FindTrackedTaskbarById(taskbarId);
        if (refreshed && !frozenDynamicGap &&
            refreshed->postReleaseReconcileFrames == 0 &&
            !refreshed->reorderStructuralReconcilePending) {
            StopAllGeometryTracking(*refreshed);
        }
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

    // Nothing is measured per frame any more, so this callback has one job
    // left: stay alive while the freeze needs the released-button check above,
    // and drop itself once the freeze is over and the post-release window has
    // closed. The freeze is checked first, and deliberately so.
    if (!frozenDynamicGap && !taskbar.reorderDragActive &&
        taskbar.postReleaseSettlingUntil != AnimationClock::time_point{} &&
        AnimationClock::now() >= taskbar.postReleaseSettlingUntil) {
        StopAllGeometryTracking(taskbar);
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

    // Freeze the gap when a mouse button goes down on the taskbar. This is
    // intentionally simple: any press freezes until release, which avoids
    // mid-drag layout feedback loops. A normal click is short enough that the
    // deferred reconcile on release is not noticeable.
    if (!taskbarState->reorderDragActive) {
        taskbarState->reorderDragActive = true;
        taskbarState->hasFrozenDynamicGap = true;
        taskbarState->frozenDynamicGap =
            taskbarState->lastAppliedDynamicGap;
        taskbarState->frozenDynamicGapVisible =
            taskbarState->lastDynamicGapVisible;

        // Drop a release-scheduled reconcile left over from a previous drag.
        // The forced reconcile below covers the current order anyway and the
        // released-button check that would have consumed the flag is not
        // guaranteed to run, so leaving it set would let this drag's first
        // rendering frame consume it as if it belonged to the previous release.
        // The settling deadline and frame counter are deliberately untouched.
        taskbarState->reorderStructuralReconcilePending = false;

        // Remember which TaskListButton was pressed so the gap can be kept on
        // the correct side while dragging.
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

        // Only a drag of the two buttons that straddle the dynamic centering
        // boundary keeps the frozen margins. Any other button releases the
        // freeze so the gap follows the live order and Windows can still match
        // the button under the cursor; an unidentified button, or no actively
        // applied centering, falls back to the old freeze instead of risking
        // the spacing.
        bool pressedButtonIdentified = false;
        bool pressedButtonIsBoundary = false;
        if (pressedButton &&
            g_dynamicCenteringActive.load(std::memory_order_acquire)) {
            const size_t boundaryIndex = static_cast<size_t>(
                GetSettingsSnapshot().groupPosition - 1);
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
                    index == boundaryIndex || index == boundaryIndex + 1;
                break;
            }
        }
        taskbarState->draggedButtonIsBoundary =
            !pressedButtonIdentified || pressedButtonIsBoundary;

        // The press-time forced reconcile is exempt from the drag freeze (see
        // ReconcileTrackedTaskbar), so it applies the frozen gap fully onto the
        // boundary button opposite the pressed one; that shape stays frozen for
        // the rest of the drag and is recomputed on release.
        if (auto repeater = taskbarState->repeater.get()) {
            ReconcileTaskbarRepeater(repeater, true);
        }

        // The released-button check runs in the rendering callback, so the
        // subscription is armed here as well: without it a swallowed
        // PointerReleased would leave the freeze in place until the next press
        // and release pair. The forced reconcile above prunes the tracked list
        // and can reallocate it, so the pointer taken at the top may dangle;
        // resolve the state again by id before it is used.
        if (auto* refreshed = FindTrackedTaskbarById(taskbarId)) {
            EnsureGeometryRenderingSubscribed(*refreshed);
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
    if (!g_dynamicCenteringActive.load(std::memory_order_acquire)) {
        return 0;
    }

    const size_t buttonIndex =
        static_cast<size_t>(GetSettingsSnapshot().groupPosition - 1);
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
        StopAllGeometryTracking(taskbar);
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
    StopAllGeometryTracking(taskbar);
    try {
        DetachLayoutChangeMonitor(taskbar);
    } catch (...) {
    }
    try {
        DetachReorderPointerHandler(taskbar);
    } catch (...) {
    }

    RemoveMeasurementFrame(taskbar);

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
            unsigned int currentSettingsGeneration =
                g_settingsGeneration.load(std::memory_order_acquire);
            // UpdateVisualStates is a hot path. A matching weak/scalar
            // snapshot proves that nothing we measure has changed.
            if (!forceStructuralReconcile &&
                CanReuseReconciledTaskbar(taskbar, repeater, snapshot,
                                          currentSettingsGeneration)) {
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

            Settings settings = GetSettingsSnapshot();
            // Whether centering is in effect right now; the setting only says
            // what the mod was asked to do.
            const bool dynamicCenteringActive =
                g_dynamicCenteringActive.load(std::memory_order_acquire);
            unsigned int settingsGeneration =
                g_settingsGeneration.load(std::memory_order_acquire);

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

            // The locked geometry is written against a canvas that shares the
            // root grid origin, so this mod keeps exactly that: a canvas with no
            // child, no brush and no hit testing, added to the root grid only
            // while centering can need to measure. It is created lazily, reused
            // by name and removed only by this mod.
            Controls::Canvas measurementFrame = taskbar.measurementFrame.get();
            if (!measurementFrame && rootGrid && dynamicCenteringActive) {
                for (auto const& rootChild : rootGrid.Children()) {
                    auto child = rootChild.try_as<FrameworkElement>();
                    if (child && child.Name() == kMeasurementFrameName) {
                        measurementFrame = child.try_as<Controls::Canvas>();
                        break;
                    }
                }

                if (!measurementFrame) {
                    try {
                        Controls::Canvas frame;
                        frame.Name(winrt::hstring(kMeasurementFrameName));
                        frame.HorizontalAlignment(
                            HorizontalAlignment::Stretch);
                        frame.VerticalAlignment(VerticalAlignment::Stretch);
                        frame.IsHitTestVisible(false);
                        rootGrid.Children().Append(frame);
                        measurementFrame = frame;
                    } catch (...) {
                    }
                }

                if (measurementFrame) {
                    taskbar.measurementFrame =
                        winrt::make_weak(measurementFrame);
                } else {
                    taskbar.measurementFrame = {};
                }
            }

            // The realized icons are only needed by the orientation probe, which
            // compares adjacent icon movement before falling back to the
            // taskbar's own proportions.
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
                appIcons[buttonIndex] =
                    FindDescendantByName(iconPanel, L"Icon");
            }

            TaskbarOrientation taskbarOrientation =
                TaskbarOrientation::horizontal;
            bool orientationValid =
                measurementFrame &&
                TryGetTaskbarOrientation(OrientationSetting::automatic,
                                         measurementFrame, snapshot.rootWidth,
                                         snapshot.rootHeight, appIcons,
                                         &taskbarOrientation);

            double primaryOrderingDirection = 0;
            bool primaryOrderingValid =
                orientationValid &&
                TryGetPrimaryOrderingDirection(measurementFrame, appButtons,
                                               taskbarOrientation,
                                               &primaryOrderingDirection);

            // A mirrored, right-to-left taskbar lays its buttons out along
            // decreasing primary coordinates, while the centering geometry is
            // written for increasing ones, so centering is not applied there
            // instead of collapsing the gap silently. A drag in progress
            // suppresses the test as well, so it is never fed a layout that is
            // mid-flight.
            bool mirroredOrdering =
                orientationValid &&
                taskbarOrientation == TaskbarOrientation::horizontal &&
                !taskbar.reorderDragActive &&
                IsMirroredPrimaryOrdering(measurementFrame, appButtons,
                                          taskbarOrientation);

            // Keep a stable base margin for each realized TaskListButton. The
            // physical gap is then added as our own delta on top of that base,
            // preserving margins supplied by Windows or other mods.
            SynchronizeTrackedButtonMargins(taskbar, appButtons);
            std::vector<ButtonGapContribution> gapContributions(
                appButtons.size());

            const bool centeringApplies =
                dynamicCenteringActive && orientationValid &&
                taskbarOrientation == TaskbarOrientation::horizontal &&
                primaryOrderingValid && !mirroredOrdering;

            if (centeringApplies) {
                bool dynamicVisible = true;
                double fullGap = 0;
                if (taskbar.reorderDragActive &&
                    taskbar.hasFrozenDynamicGap) {
                    dynamicVisible =
                        taskbar.frozenDynamicGapVisible;
                    fullGap = taskbar.frozenDynamicGap;
                } else {
                    fullGap = CalculateDynamicCenteredGap(
                        rootGrid, measurementFrame, appButtons,
                        settings.groupPosition, taskbarOrientation,
                        &dynamicVisible, &taskbar.trayFrame);
                    // The last valid value is reused only inside the
                    // post-release settling window, where the buttons are still
                    // animating and the live geometry can be invalid: the gap
                    // then collapses to ~0 and the spacing would flash. That
                    // window is armed by the release path alone. Outside it a
                    // sub-pixel gap is the real answer and the tray clamp holds.
                    const bool postReleaseSettling =
                        taskbar.postReleaseReconcileFrames > 0 ||
                        (taskbar.postReleaseSettlingUntil !=
                             AnimationClock::time_point{} &&
                         AnimationClock::now() <
                             taskbar.postReleaseSettlingUntil);
                    if (dynamicVisible && fullGap < 1.0 &&
                        postReleaseSettling) {
                        fullGap = taskbar.lastAppliedDynamicGap;
                    }
                }
                taskbar.lastAppliedDynamicGap = fullGap;
                taskbar.lastDynamicGapVisible = dynamicVisible;

                if (dynamicVisible) {
                    size_t buttonIndex =
                        static_cast<size_t>(settings.groupPosition - 1);
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
                                    AddDirectionalButtonGap(
                                        &gapContributions[buttonIndex],
                                        taskbarOrientation,
                                        primaryOrderingDirection, false,
                                        alignedGap);
                                } else {
                                    // The dragged button is currently to the
                                    // left of this boundary (or is its left
                                    // boundary button): give the full gap to
                                    // the right boundary button.
                                    AddDirectionalButtonGap(
                                        &gapContributions[buttonIndex + 1],
                                        taskbarOrientation,
                                        primaryOrderingDirection, true,
                                        alignedGap);
                                }
                            } else {
                                // Split the space across the two neighboring
                                // buttons, snapping each half to the physical
                                // pixel grid on its own. The halves can then
                                // miss fullGap by half a pixel, deliberately:
                                // sub-pixel margins are rounded differently by
                                // the layout engine for even and odd counts.
                                double halfLeft = fullGap / 2.0;
                                double halfRight = fullGap - halfLeft;
                                double scale = GetRasterizationScale(rootGrid);
                                halfLeft = SnapToPhysicalPixel(halfLeft, scale);
                                halfRight = SnapToPhysicalPixel(
                                    fullGap - halfLeft, scale);
                                AddDirectionalButtonGap(
                                    &gapContributions[buttonIndex],
                                    taskbarOrientation,
                                    primaryOrderingDirection, false, halfLeft);
                                AddDirectionalButtonGap(
                                    &gapContributions[buttonIndex + 1],
                                    taskbarOrientation,
                                    primaryOrderingDirection, true, halfRight);
                            }
                        } else {
                            // The group starts after the final realized app
                            // button, so the whole gap goes on its trailing
                            // side.
                            AddDirectionalButtonGap(
                                &gapContributions[buttonIndex],
                                taskbarOrientation, primaryOrderingDirection,
                                false, fullGap);
                        }
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

            // A zero gap, a hidden group, or a position past the last realized
            // button produces a zero contribution and restores our previous
            // delta. If the ordering probe is temporarily unavailable while
            // centering is active, the current margin is kept until the next
            // valid reconciliation rather than guessed away; a lone usable
            // button needs no direction to drop a tracked contribution.
            const bool probeUsable = orientationValid && primaryOrderingValid;
            const bool applySkipped =
                !(probeUsable || !dynamicCenteringActive || mirroredOrdering ||
                  (appButtons.size() < 2 && !hasGapContributions));
            if (!applySkipped) {
                ApplyTrackedButtonGapMargins(taskbar, appButtons,
                                             gapContributions, &snapshot);
            }

            // Keep the margin ledger free of orphans. A tracked button may hold
            // our increment only while it is one of the buttons this shape puts
            // the gap on, and this is the only sweep that reaches the ledger
            // when the Apply above was skipped. It writes no new gap: it only
            // hands a stale increment back.
            FrameworkElement gapHosts[2]{nullptr, nullptr};
            size_t gapHostCount = 0;
            if (dynamicCenteringActive && !mirroredOrdering) {
                for (size_t index = 0;
                     index < gapContributions.size() && gapHostCount < 2;
                     index++) {
                    auto const& gap = gapContributions[index];
                    if (std::fabs(gap.left) > 0.001 ||
                        std::fabs(gap.top) > 0.001 ||
                        std::fabs(gap.right) > 0.001 ||
                        std::fabs(gap.bottom) > 0.001) {
                        gapHosts[gapHostCount++] = appButtons[index];
                    }
                }

                if (gapHostCount == 0 && !centeringApplies) {
                    // No contribution was produced because the geometry probe
                    // is unavailable, not because this shape has no gap: the
                    // buttons still carry the boundary pair the previous
                    // reconcile wrote. The pair is treated as hosts on purpose;
                    // while frozen the whole gap sits on one of them and the
                    // other is the dragged ghost, so it is never rolled back.
                    const size_t buttonIndex =
                        static_cast<size_t>(settings.groupPosition - 1);
                    if (buttonIndex < appButtons.size()) {
                        gapHosts[gapHostCount++] = appButtons[buttonIndex];
                        if (buttonIndex + 1 < appButtons.size()) {
                            gapHosts[gapHostCount++] =
                                appButtons[buttonIndex + 1];
                        }
                    }
                }
            }
            RestoreOrphanGapMargins(taskbar, gapHosts, gapHostCount);

            result = ReconcileResult::succeeded;
            if (!snapshot.signatureValid) {
                // Nothing was measured, so the layout is not ready yet; the
                // next pass retries instead of caching this as the state.
                result = ReconcileResult::temporarilyNotReady;
            } else if (applySkipped && dynamicCenteringActive &&
                       (!orientationValid || !primaryOrderingValid)) {
                // The geometry probe was unavailable while centering is in
                // effect, so the margins were neither rewritten nor handed
                // back. Reporting "not ready" keeps that shape out of the
                // committed signature (CommitReconciledTaskbar refuses it), so
                // the next layout event recomputes instead of reusing a stale,
                // half-applied gap.
                result = ReconcileResult::temporarilyNotReady;
            }

            // The pointer handlers belong to the root grid the margins are
            // written on, so they are attached whenever a root grid exists and
            // the taskbar is in a state where centering can matter.
            if (rootGrid && dynamicCenteringActive) {
                AttachReorderPointerHandler(taskbar, rootGrid);
            } else {
                DetachReorderPointerHandler(taskbar);
            }

            if (result != ReconcileResult::temporarilyNotReady) {
                taskbar.appliedSettingsGeneration = settingsGeneration;
                CommitReconciledTaskbar(taskbar, repeater, snapshot, result);
            }
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
        // tracked margins have been handed back and the measurement frame has
        // been detached from the taskbar root grid.
        for (auto& taskbar : *g_trackedTaskbars) {
            RemoveMeasurementFrame(taskbar);

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
// first moment centering can be applied in this process.
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

// Centering only works on a left-aligned taskbar: center alignment stacks on
// top of the group and makes the reserved gap look wrong.
//
// The mod only *reads* the taskbar alignment (TaskbarAl). It never writes it and
// never changes what Explorer reads, so the user's own setting is always left
// alone; with a center-aligned taskbar centering is simply not applied.
static const wchar_t kTaskbarAdvancedKey[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";

// Missing value is reported as 1 (Windows 11 default = center). A value stored
// as anything other than REG_DWORD is treated as a read failure rather than
// being reinterpreted from raw bytes.
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
    DWORD valueType = 0;
    DWORD size = sizeof(taskbarAl);
    status = RegQueryValueExW(key, L"TaskbarAl", nullptr, &valueType,
                              reinterpret_cast<LPBYTE>(&taskbarAl), &size);
    RegCloseKey(key);
    if (status != ERROR_SUCCESS || valueType != REG_DWORD ||
        size != sizeof(taskbarAl)) {
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
// centering is active. The mod does not fight the user: centering is turned
// off and the gap it reserved is dropped. This runs on the alignment monitor
// thread, which no longer exits right after it, so it never joins itself.
void YieldDynamicCenteringForUserAlignment() {
    Wh_Log(L"the user changed the taskbar alignment; centering is off");

    g_dynamicCenteringActive.store(false, std::memory_order_release);

    // The alignment value belongs to the user: this mod never wrote it, so
    // there is nothing to restore. The rebuild is not run here because the
    // caller repeats it across the same bounded window as the opposite
    // direction: the margin is handed back one button per tracked entry, and a
    // single run taken while the row is still sliding can return only some of
    // them.
}

// The bounded rebuild window, shared by both alignment directions: the first
// rebuild runs immediately and up to eight more follow at 500 ms intervals.
// Switching the alignment slides the whole button row, so a single rebuild can
// run while the geometry is still moving. Nothing is guaranteed to rebuild
// afterwards, so a later attempt taken once the geometry has settled finishes
// the job. Either stop event ends the window early.
void RepeatReconcileOnTaskbarThread(HANDLE stopEvent, HANDLE changeEvent) {
    HANDLE handles[2] = {stopEvent, changeEvent};

    for (int attempt = 0; attempt <= 8; attempt++) {
        if (attempt > 0) {
            // The unload path waits for this thread to exit, so neither the
            // remaining attempts nor the 500 ms wait may hold it up: both
            // stop conditions end the loop.
            if (g_alignMonitorStop.load(std::memory_order_acquire) ||
                g_unloading.load(std::memory_order_acquire)) {
                break;
            }
            // The alignment change event belongs in this wait as well: the
            // user can switch the alignment again inside this window, and
            // the loop must end at once instead of rebuilding for an
            // alignment that is no longer in effect. The monitor reads the
            // new value on its next iteration and acts on that instead.
            DWORD retryResult =
                WaitForMultipleObjects(2, handles, FALSE, 500);
            if (retryResult != WAIT_TIMEOUT) {
                break;
            }
        }
        if (g_taskbarViewDllLoaded && !RunReconcileOnTaskbarThread(true)) {
            Wh_Log(L"the icon group could not be rebuilt on the taskbar UI "
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

    // Let AfterInit settle before the first check. StartTaskbarAlignMonitor
    // returns early when the stop event cannot be created, so the handle is
    // always valid here.
    WaitForSingleObject(g_alignStopEvent, 1000);

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

        // The monitor reacts in both directions. Leaving left turns centering
        // off; coming back to left turns it on again. The registration above is
        // one-shot and is renewed by the next iteration, so the loop keeps
        // waiting here instead of exiting.
        DWORD taskbarAl = 0;
        if (ReadTaskbarAl(&taskbarAl)) {
            if (taskbarAl != 0) {
                if (g_dynamicCenteringActive.load(std::memory_order_acquire)) {
                    YieldDynamicCenteringForUserAlignment();
                    // The margin this mod applied is handed back one button
                    // per tracked entry, and the row is sliding right now, so a
                    // single rebuild can return only part of it: the rest would
                    // linger until some later rebuild that nothing guarantees.
                    // The same bounded window as the switch back to left
                    // alignment is therefore run in this direction too.
                    RepeatReconcileOnTaskbarThread(g_alignStopEvent,
                                                   watch.changeEvent);
                }
            } else if (!g_dynamicCenteringActive.load(
                           std::memory_order_acquire)) {
                Wh_Log(L"the taskbar is left-aligned again; centering is "
                       L"applied again");
                g_dynamicCenteringActive.store(true, std::memory_order_release);
                // Both directions share the bounded rebuild window: the
                // first rebuild is immediate and the later attempts cover
                // the slide animation, whose geometry has not settled yet.
                RepeatReconcileOnTaskbarThread(g_alignStopEvent,
                                               watch.changeEvent);
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
    // Serialized with StartTaskbarAlignMonitor, but the unbounded wait below is
    // not held under the mutex. The monitor thread can block on a taskbar UI
    // thread that is itself waiting for this mutex in
    // StartTaskbarAlignMonitor, which would deadlock the unload thread. Only
    // the stop signal and the handle handoff happen under the lock; the wait
    // itself is lock-free.
    HANDLE monitorThread = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_alignMonitorMutex);

        g_alignMonitorStop.store(true, std::memory_order_release);
        if (g_alignStopEvent) {
            SetEvent(g_alignStopEvent);
        }

        // The handle is taken out from under the lock and the stop event stays
        // set, so a concurrent start can never create a second monitor thread:
        // this stop path only runs with g_unloading raised first, and the start
        // re-checks it under the same lock before CreateThread.
        monitorThread = g_alignMonitorThread;
        g_alignMonitorThread = nullptr;
    }

    if (monitorThread) {
        // The monitor waits on the stop event, so this returns immediately;
        // the wait is unbounded so that an unloaded image can never leave a mod
        // thread running. A failed wait is retried rather than returning with
        // the handle left set, and waiting outside the mutex is what breaks the
        // deadlock: the monitor thread can be blocked on the taskbar UI thread,
        // which can itself be waiting for the mutex in StartTaskbarAlignMonitor.
        for (;;) {
            if (WaitForSingleObject(monitorThread, INFINITE) ==
                WAIT_OBJECT_0) {
                break;
            }

            Wh_Log(L"waiting for the alignment monitor thread failed: %lu",
                   GetLastError());
            Sleep(1);
        }

        CloseHandle(monitorThread);
    }

    // Re-locked only to close the stop event, which happens after the thread
    // has really exited and together with the handle, never half updated.
    {
        std::lock_guard<std::mutex> lock(g_alignMonitorMutex);
        if (g_alignStopEvent) {
            CloseHandle(g_alignStopEvent);
            g_alignStopEvent = nullptr;
        }
    }
}

// Applies centering according to the current taskbar alignment. The mod never
// writes the alignment, so this is read-only: centering is applied when the
// taskbar is left-aligned, and it is not applied while it is centered. It is
// called on load and on every settings change, in the process that hosts the
// taskbar view.
void ApplyDynamicCenteringSetting() {
    if (g_unloading.load(std::memory_order_acquire)) {
        return;
    }

    DWORD taskbarAl = 1;
    if (!ReadTaskbarAl(&taskbarAl)) {
        Wh_Log(L"could not read the taskbar alignment; centering stays "
               L"inactive");
        g_dynamicCenteringActive.store(false, std::memory_order_release);
        // The monitor stays resident: a readable alignment value is picked up
        // without changing a setting or reloading the mod.
        StartTaskbarAlignMonitor();
        return;
    }

    if (taskbarAl != 0) {
        // The taskbar is center-aligned: Windows centers the whole button row
        // itself there, so centering is not applied. The alignment setting is
        // not changed; switching the taskbar to Left alignment in Windows'
        // taskbar settings is what enables centering.
        Wh_Log(L"the taskbar is center-aligned; centering stays inactive");
        g_dynamicCenteringActive.store(false, std::memory_order_release);
        // The monitor stays resident: it is what notices the taskbar being
        // switched back to left, which turns centering on again.
        StartTaskbarAlignMonitor();
        return;
    }

    // The taskbar is left-aligned: centering is applied, and the monitor
    // watches in both directions.
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
        // Only the process that hosts the taskbar view applies centering, so
        // only it reads the taskbar alignment.
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
            Wh_Log(L"Unload cleanup did not reach the taskbar UI thread");
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

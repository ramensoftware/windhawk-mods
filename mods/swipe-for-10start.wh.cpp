// ==WindhawkMod==
// @id              swipe-for-10start
// @name            Swipe for 10Start
// @name:zh-CN      Swipe for 10Start
// @description     Touch and pen swipe gestures for ExplorerPatcher's legacy Windows 10 Start menu: pull it out with your finger, drag it back to close, with the same animation on the Windows key.
// @description:zh-CN 为 ExplorerPatcher 恢复的 Windows 10 经典开始菜单加上触摸与手写笔滑动：手指拉出、拖回关闭，Win 键开合播放同一套动画。
// @version         0.8.6
// @author          Zhanyuanium
// @github          https://github.com/Zhanyuanium
// @license         MIT
// @homepage        https://github.com/Zhanyuanium/swipe-for-10start
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luser32 -lhid -lshell32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Swipe for 10Start

Touch and pen swipe gestures for the legacy Windows 10 Start menu that
ExplorerPatcher restores on Windows 11. Swipe in from the taskbar edge and the
menu opens following your finger; drag the open menu back toward that edge and
it slides away and closes. Keyboard opens and closes get the same animation.
By Zhanyuanium, distributed as a Windhawk mod.

![Bottom taskbar: the menu follows the finger and a close is reversed](https://raw.githubusercontent.com/Zhanyuanium/swipe-for-10start/main/docs/images/demo-bottom.gif)

![Left taskbar: the same gesture](https://raw.githubusercontent.com/Zhanyuanium/swipe-for-10start/main/docs/images/demo-left.gif)

## What you need

- **Windows 11** with **ExplorerPatcher** set to the Windows 10 (legacy) Start
  menu (`Start_ShowClassicMode=1`). Verified on Windows 11 build 26300.10075
  (`twinui.pcshell.dll` fixed 10.0.26100.10068) and ExplorerPatcher 26100.9711.
  The Explorer half enables its private launcher calls only on that exact
  `twinui.pcshell.dll` build. After a Windows update changes it, the mod is
  inactive until a release supports the new image: no gesture follows your
  finger in either half, and the shell's Start opens and closes exactly as if
  the mod were not installed.
  After switching ExplorerPatcher to the Windows 10 Start, restart Explorer and
  StartMenuExperienceHost, or sign out and back in, so the mod can load.
- **Windhawk**.
- A touchscreen or pen. (The mouse is deliberately not used for these
  gestures.)

## The mod

One Windhawk mod ships, and it runs in both processes it needs: the gesture and
the animation live in StartMenuExperienceHost, the shell calls in Explorer.

Swipe in from the taskbar edge and the menu opens following your finger; drag
the open menu back toward the edge and it slides away. The Windows key, the
Start button and a click on the desktop use the same animation.

A press or a pull during a close takes the menu back while it is still on
screen: the close stops where it is and the panel reverses. A close that has
already landed is final for that finger; the next open needs a new touch.
Win+Tab, the lock screen and a display change pass through untouched.

If Explorer's half cannot run on this build, the mod is inactive: no gesture
follows your finger in either half, and the shell's Start behaves exactly as if
the mod were not installed, until a release supports the new image.

## Install

Install **Swipe for 10Start** from Windhawk and enable it. Enabling or updating
attaches to the menu host that is already running; no Explorer or Start menu
restart is needed.

## Settings

- **Menu animation style** (`AnimationStyle`, default `Windows 11`) — the
  shape of the slide. `Windows 11` is fitted to the motion of the system's own
  panels (the Quick Settings flyout); `Windows 10` is the feel the mod shipped
  with originally, a looser spring. Both run the same spring — the choice
  changes the feel, never the mechanism — and it also decides how opening and
  closing with the Windows key looks.
- **Animation start speed (% of swipe speed)** (`GestureStartSpeedPercent`,
  default 100, range 0-300) — how fast the menu keeps moving the moment you
  lift your finger, as a percentage of the finger's speed in the direction the
  menu is going. 100 continues at finger speed, higher values snap away
  faster, 0 always starts the animation from a standstill. A finger that was
  held still starts from rest whatever this says, and Windows-key opens always
  start from rest — this setting does not affect them.
- **Drag intent distance (% of travel)** (`CommitDistancePercent`,
  default 10, range 1-50) — how far a drag must go toward the target before
  releasing commits it. A pull that travels less than this springs back closed;
  a close drag that travels less than this reverses back open. The same value
  applies on every taskbar side, and a quick flick commits even when it is
  shorter.
- **Write a diagnostic trace file** (`DiagnosticTrace`, default off) — turns on
  the decision log described below. Leave it off for everyday use; a bug report
  or a maintainer will ask for it.

## What works where

- Taskbar on the **left**, **top**, **right** or **bottom**. **Auto-hide** is
  supported.
- **One display.** A taskbar on a secondary monitor or a rotated display is not
  supported; every measurement is the primary panel.
- **Touch** works. **Pen** input is decoded (hover is ignored) but has not yet
  been exercised by hand.
- On a top or bottom taskbar, a vertical swipe inside the open menu scrolls
  what is under your finger — the app list, or the grid of pinned tiles — and
  becomes a dismiss only once that area reaches its end.
- A pull that starts just inside the screen edge — up to about half a taskbar's
  height, the strip the system's own swipe opens from — opens the menu, and the
  menu follows the finger from the frame it appears. A tap on the taskbar
  background does not request a touch open: the mod asks for the menu only after
  the finger has travelled 24 DIP away from the edge. A tap on a pinned app,
  the clock or the tray stays a native press; a tap on the Start button still
  opens the menu, as it always does. A pull that starts further in does not open
  it: only the taskbar edge does that.
- Mouse drag is deliberately not enabled.
- After Explorer restarts, or the first time the mod is enabled, the first open
  and close are the shell's own: that press is what proves which thread may run
  the launcher calls. From the next press on, every trigger is the mod's.

## Try it

- Swipe in from the taskbar edge: the menu opens and follows your finger. A
  swipe that begins a little further in, about half a taskbar's height, keeps
  up with your hand too instead of snapping open. A quick flick that ends
  before the menu appears still carries its speed into the opening motion, and
  the menu rises from the bottom edge instead of appearing part-way open. A
  short or reversed pull springs back; a longer pull or a flick keeps it open.
- With the menu open, drag it toward the taskbar edge to dismiss it — past the
  drag-intent distance (a tenth of the panel's travel by default), or with a
  flick, it commits and slides out.
- Press the Windows key: the same animation plays on open and on close.

## If something looks wrong

- The mod can write a decision log into two files in its Windhawk storage
  folder, which Windhawk shows on the mod's page: `renderer.log` from the Start
  half and `bridge.log` from the Explorer half, both under `swipe-for-10start`.
  They are written only while **Write a diagnostic trace file**
  (`DiagnosticTrace`) is on; turn it on once from an elevated shell with
  `windhawk-cli mod settings set swipe-for-10start DiagnosticTrace=true`
  or from the mod's settings, then reproduce the problem.
- The root hook is found by public symbol, not by address. If `StartUI_.dll`
  does not carry `StartUI::App::PrepareRootElement` (or its symbols cannot be
  read), no gesture starts and the log shows `startui root symbol unresolved
  ...`; the shell then handles every press natively.
- While the menu is being dragged, its contents still receive touches at their
  original positions — a known limit, along with the assumption that the touch
  panel covers the primary monitor.
- The full technical documentation — what was measured, what was falsified,
  and every design decision — lives in
  [docs/](https://github.com/Zhanyuanium/swipe-for-10start/blob/main/docs/README.md),
  including the complete
  [known-limits list](https://github.com/Zhanyuanium/swipe-for-10start/blob/main/docs/limits.md).
- [CHANGELOG.md](https://github.com/Zhanyuanium/swipe-for-10start/blob/main/CHANGELOG.md)
  lists each release tag with the version it shipped.
- Building and installing from source is covered in
  [docs/development.md](https://github.com/Zhanyuanium/swipe-for-10start/blob/main/docs/development.md).


---

# Swipe for 10Start

ExplorerPatcher 在 Windows 11 上恢复了经典的 Windows 10 开始菜单；本模组为它加上触摸和手写笔滑动手势。从任务栏所在的屏幕边缘划入，菜单会跟随手指打开；把已打开的菜单往该边缘拖回，菜单会滑出并关闭。用键盘打开或关闭菜单时播放同一套动画。由 Zhanyuanium 开发，以 Windhawk 模组形式发布。

![任务栏在底部：菜单跟随手指，关闭被反转](https://raw.githubusercontent.com/Zhanyuanium/swipe-for-10start/main/docs/images/demo-bottom.gif)

![任务栏在左侧：同样的手势](https://raw.githubusercontent.com/Zhanyuanium/swipe-for-10start/main/docs/images/demo-left.gif)

## 你需要什么

- **Windows 11**，且 **ExplorerPatcher** 已设置为 Windows 10（经典）开始菜单（`Start_ShowClassicMode=1`）。已在 Windows 11 build 26300.10075（`twinui.pcshell.dll` 固定版本 10.0.26100.10068）和
  ExplorerPatcher 26100.9711 上验证。Explorer 半边只在这个精确的 `twinui.pcshell.dll` 版本上启用私有启动器调用；Windows 更新改变该文件后，模组整体不生效：两边的触摸和手写笔手势都不会跟随手指，系统开始菜单与未安装本模组时完全一致，直到新版本支持该镜像。切换到 Windows 10 开始菜单后，请重启 Explorer 和 StartMenuExperienceHost，或注销后重新登录，模组才会加载。
- **Windhawk**。
- 触摸屏或手写笔。（本模组刻意不响应鼠标拖动。）

## 模组由两部分组成

只发布一个 Windhawk 模组，同时运行在需要的两个进程中：手势和动画在 StartMenuExperienceHost 里，系统调用在 Explorer 里。

从任务栏边缘划入，菜单跟随手指打开；把已打开的菜单往该边缘拖回，菜单滑出关闭。Win 键、开始按钮和点击桌面播放同一套动画。

关闭过程中再按一次或再拉一次，只要窗口还在屏幕上，就会取消这次关闭，并从当前位置反向回弹；已经落地的关闭不会被同一根手指重新打开，下一次打开需要新的触摸。Win+Tab、锁屏和显示变化直接透传。

如果 Explorer 的半边在当前系统版本上无法运行，模组整体不生效：两边的触摸和手写笔手势都不会跟随手指，系统开始菜单与未安装本模组时完全一致，直到新版本支持该镜像。

## 安装

在 Windhawk 中安装 **Swipe for 10Start** 并启用。启用或更新会挂到正在运行的菜单宿主上，不需要重启 Explorer 或开始菜单宿主。

## 设置项

- **菜单动画样式**（`AnimationStyle`，默认 `Windows 11`）——滑动动画的曲线形状。`Windows 11` 拟合自系统自带面板（快速设置弹出面板）的动作；`Windows 10` 是本模组最初发布时的手感，弹簧更松。两者跑的是同一条弹簧——这个选择只改手感，不改机制——同一个选择也决定按 Windows 键开合菜单时的动画。
- **动画起步速度（占滑动速度的百分比）**（`GestureStartSpeedPercent`，默认 100，范围 0–300）——松手瞬间菜单继续滑动的速度，取手指朝菜单目标方向的速度分量的百分比。100 表示按手指速度继续；更大的值甩得更快；0 总是从静止起步。手指停住再抬起时无论设多少都从静止起步；用 Windows 键打开也总是从静止起步——这两种情况都不受这个设置影响。
- **拖动意图距离（占行程的百分比）**（`CommitDistancePercent`，默认 10，范围 1–50）——拖动朝目标方向走多远，松手才算数。拉开不足这个距离会弹回关闭状态；收回不足这个距离会回弹重新打开。四个任务栏方向共用同一个值；快速一甩即使距离更短也会生效。
- **写入诊断跟踪文件**（`DiagnosticTrace`，默认关闭）——打开下文所述的决策日志。日常使用请保持关闭；只有在维护者或缺陷报告要求时才打开。

## 适用的场景

- 任务栏在**左侧**、**顶部**、**右侧**或**底部**。支持**自动隐藏**。
- **单块显示器。** 任务栏放在副屏、或屏幕旋转后，均不支持；所有测量都来自主显示器。
- **触摸**可用。**手写笔**输入已解码（悬停被忽略），但尚未在真机上实测过。
- 任务栏在顶部或底部时，在已打开的菜单里上下滑动，会先滚动手指下方的内容（应用列表，或固定磁贴的网格）；等这块区域滚到底，同一次滑动才开始关闭菜单。
- 从屏幕边缘稍微靠里一点起手——大约半个任务栏高，也就是系统自带滑动的起手范围——菜单会打开，并且从出现的那一帧起就跟随手指。单击任务栏空白不会请求触摸打开：手指要朝桌面方向移动满 24 DIP，模组才会请求打开。点在固定应用、时钟或托盘上是一次原生点击；点开始按钮仍会打开菜单，和平时一样。起手位置更靠里则不会打开菜单：只有任务栏边缘能打开它。
- 鼠标拖动刻意未启用。
- Explorer 重启后，或模组第一次被启用后，第一次开合由系统自己完成——正是这一次按键证明哪条线程可以执行启动器调用；从下一次按键起，所有触发都走模组。

## 上手一试

- 从任务栏边缘划入：菜单打开并跟随手指。从稍微靠里一点（约半个任务栏高）起手也会跟随手指，不会在手底下直接弹开。快速一甩若在菜单出现之前就离手，菜单也会顺着这股速度从屏幕边缘升起，而不是先停一下、再以半开的样子出现。拉得短或往回推会弹回去；拉得更长或快速一甩则保持打开。
- 菜单打开时，把它往任务栏边缘拖即可关闭——拖过拖动意图距离（默认是面板行程的十分之一），或快速一甩，就构成一次关闭手势，菜单随之滑出。
- 按 Windows 键：打开和关闭时播放同一套动画。

## 如果看起来不对

- 模组可以把决策日志写进它的 Windhawk 存储目录（Windhawk 在模组页面上显示该路径）下的两个文件：Start 半边的 `renderer.log` 和 Explorer 半边的 `bridge.log`，都在 `swipe-for-10start` 下。只有打开 **写入诊断跟踪文件**（`DiagnosticTrace`）时才会写；打开一次即可：
  `windhawk-cli mod settings set swipe-for-10start DiagnosticTrace=true`，
  也可以在模组设置里打开（命令行需要管理员权限），然后复现问题。
- 根钩子按公开符号查找，不依赖固定地址。若 `StartUI_.dll` 不含 `StartUI::App::PrepareRootElement`（或读不到符号），手势不会启动，日志里会看到 `startui root symbol unresolved ...`；此时所有按键都交给系统自行处理。
- 拖动菜单期间，菜单内容仍在原位置接收触摸——这是已知限制；另一条已知限制：本模组假设触摸面板覆盖主显示器。
- 完整的技术文档在 [docs/](https://github.com/Zhanyuanium/swipe-for-10start/blob/main/docs/README.md)（英文），其中有测量数据、被证伪的方案和每项设计决定的理由，也包括完整的[已知限制列表](https://github.com/Zhanyuanium/swipe-for-10start/blob/main/docs/limits.md)。
- 每次发布的 tag 与对应的版本号见 [CHANGELOG.zh-CN.md](https://github.com/Zhanyuanium/swipe-for-10start/blob/main/CHANGELOG.zh-CN.md)（英文版 [CHANGELOG.md](https://github.com/Zhanyuanium/swipe-for-10start/blob/main/CHANGELOG.md)）。
- 从源码构建与安装的步骤见 [docs/development.md](https://github.com/Zhanyuanium/swipe-for-10start/blob/main/docs/development.md)。

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- AnimationStyle: Windows 11
  $name: Menu animation style
  $name:zh-CN: 菜单动画样式
  $description: >-
    The shape of the slide. Windows 11 is fitted to the system's Quick Settings
    flyout; Windows 10 is the looser spring the mod shipped with. Both run the
    same spring; the choice changes the feel, never the mechanism, and it also
    decides how opening and closing with the Windows key look.
  $description:zh-CN: 滑动动画的曲线形状。Windows 11 拟合自系统自带面板（快速设置弹出面板）的动作；Windows 10 是本模组最初发布时的手感，弹簧更松。两者跑的是同一条弹簧——这个选择只改手感，不改机制——同一个选择也决定按 Windows 键开合菜单时的动画。
  $options:
  - Windows 11: glides like the Quick Settings flyout
  - Windows 10: looser spring
- GestureStartSpeedPercent: 100
  $name: Animation start speed (% of swipe speed)
  $name:zh-CN: 动画起步速度（占滑动速度的百分比）
  $description: >-
    How fast the menu keeps moving the moment you lift your finger, as a
    percentage of the finger's speed in the direction the menu is going. 100
    continues at finger speed, higher values snap away faster, 0 always starts
    from a standstill. A finger held still starts from rest whatever this says,
    and Windows-key opens always start from rest; this setting does not affect
    them.
  $description:zh-CN: 松手瞬间菜单继续滑动的速度，取手指朝菜单目标方向的速度分量的百分比。100 表示按手指速度继续；更大的值甩得更快；0 总是从静止起步。手指停住再抬起时无论设多少都从静止起步；用 Windows 键打开也总是从静止起步——这两种情况都不受这个设置影响。
- CommitDistancePercent: 10
  $name: Drag intent distance (% of travel)
  $name:zh-CN: 拖动意图距离（占行程的百分比）
  $description: How far a drag must travel toward the target before releasing commits it. A shorter pull springs back closed; a shorter close drag reverses back open. The same value applies on every taskbar side; a quick flick commits even when it is shorter.
  $description:zh-CN: 拖动朝目标方向走多远，松手才算数。拉开不足这个距离会弹回关闭状态；收回不足这个距离会回弹重新打开。四个任务栏方向共用同一个值；快速一甩即使距离更短也会生效。
- DiagnosticTrace: false
  $name: Write a diagnostic trace file
  $name:zh-CN: 写入诊断跟踪文件
  $description: Turns on the decision log described below. Leave it off for everyday use; a bug report or a maintainer will ask for it.
  $description:zh-CN: 打开下文所述的决策日志。日常使用请保持关闭；只有在维护者或缺陷报告要求时才打开。
*/
// ==/WindhawkModSettings==

// One image, two roles. StartMenuExperienceHost.exe runs the controller: the Raw
// Input gesture, Progress, settles, the close record and the gates. explorer.exe
// runs the executor: the launcher call and the Win/Start-button/click-away triggers.
// The halves talk over mods/swipe_ipc_shared.h (Local\SwipeFor10Start_SingleMod6_*).
// The launcher layout is pinned to twinui.pcshell.dll 10.0.26100.10068.

#include <windhawk_utils.h>
#include <windows.h>
#undef GetCurrentTime

#include <shellapi.h>
#include <hidusage.h>
#include <hidpi.h>
#include <winver.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <memory>
#include <mutex>
#include <new>
#include <optional>
#include <utility>
#include <vector>

// ---- begin swipe_ipc_shared.h (inlined: the store takes one self-contained file) ----
// The shared contract defines the vocabulary for both sides. This file is one
// side of it, so the unused half of these constants is deliberate.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-const-variable"

#include <cstddef>
#include <cstdint>
#include <windows.h>


// Shared-memory contract between the Start-side controller and the explorer-side
// bridge: one session-local section and five auto-reset events, every record a
// seqlock. The names are Local\SwipeFor10Start_SingleMod6_*; that namespace is
// the ABI identity, and there is no product version word.

constexpr wchar_t kSwipeIpcStateName[] =
    L"Local\\SwipeFor10Start_SingleMod6_State";
constexpr wchar_t kSwipeIpcIntentEventName[] =
    L"Local\\SwipeFor10Start_SingleMod6_Intent";
constexpr wchar_t kSwipeIpcIntentAckEventName[] =
    L"Local\\SwipeFor10Start_SingleMod6_IntentAck";
constexpr wchar_t kSwipeIpcExecEventName[] =
    L"Local\\SwipeFor10Start_SingleMod6_Exec";
constexpr wchar_t kSwipeIpcExecAckEventName[] =
    L"Local\\SwipeFor10Start_SingleMod6_ExecAck";
constexpr wchar_t kSwipeIpcCapabilityEventName[] =
    L"Local\\SwipeFor10Start_SingleMod6_Capability";

constexpr uint32_t kSwipeIpcMagic = 0x53333053;

constexpr int32_t kSwipeIntentOpen = 0;
constexpr int32_t kSwipeIntentClose = 1;
constexpr int32_t kSwipeIntentExternal = 2;

constexpr int32_t kSwipeTriggerWin = 0;
constexpr int32_t kSwipeTriggerStartButton = 1;
constexpr int32_t kSwipeTriggerClickAway = 2;
constexpr int32_t kSwipeTriggerTouch = 3;
constexpr int32_t kSwipeTriggerOverride = 4;

constexpr int32_t kSwipeExecShow = 0;
constexpr int32_t kSwipeExecDismiss = 1;
constexpr int32_t kSwipeExecCancel = 2;

constexpr int32_t kSwipeAckPending = 0;
constexpr int32_t kSwipeAckExecuted = 1;
constexpr int32_t kSwipeAckRefused = 2;
constexpr int32_t kSwipeAckExpired = 3;

constexpr int32_t kSwipeAckConfirmed = 4;
constexpr int32_t kSwipeAckUnconfirmed = 5;
constexpr int32_t kSwipeAckUnknown = 6;

constexpr int32_t kSwipeAckSuperseded = 7;

constexpr int32_t kSwipeVisibilityUnknown = -1;
constexpr int32_t kSwipeVisibilityHidden = 0;
constexpr int32_t kSwipeVisibilityVisible = 1;

constexpr LONG kSwipeIpcBlank = 0;
constexpr LONG kSwipeIpcInitialising = 1;
constexpr LONG kSwipeIpcReady = 2;
constexpr LONG kSwipeIpcAbandoned = 3;

constexpr ULONGLONG kSwipeIpcInitWaitMs = 50;
constexpr ULONGLONG kSwipeIpcInitAbandonMs = 500;
constexpr ULONGLONG kSwipeIpcOpenBudgetMs =
    kSwipeIpcInitAbandonMs + kSwipeIpcInitWaitMs;

constexpr int kSwipeIpcReadAttempts = 8;

constexpr uint32_t kSwipePeerTickIntervalMs = 100;
constexpr uint32_t kSwipePeerStaleMs = 600;

constexpr int32_t kSwipePeerGone = 0;
constexpr int32_t kSwipePeerAlive = 1;

struct SwipeIpcState {
    uint32_t magic;

    volatile LONG64 openSeq;
    volatile LONG64 openTick;

    volatile LONG initState;
    volatile LONG layoutBytes;
    volatile LONG64 initTick;

    volatile LONG64 intentSeq;
    volatile LONG64 intentTick;
    volatile LONG intentKind;
    volatile LONG intentTrigger;
    volatile LONG intentMethod;
    volatile LONG intentFlags;
    volatile LONG64 intentOpen;

    volatile LONG64 intentAckSeq;
    volatile LONG64 intentAckFor;
    volatile LONG intentAckState;
    volatile LONG intentAckDuration;
    volatile LONG intentAckEntry;
    volatile LONG intentAckHr;

    volatile LONG64 execSeq;
    volatile LONG64 execTick;
    volatile LONG execKind;
    volatile LONG execMethod;
    volatile LONG execFlags;
    volatile LONG64 execExecuteAt;
    volatile LONG64 execOpen;
    volatile LONG64 execTarget;

    volatile LONG64 execAckSeq;
    volatile LONG64 execAckFor;
    volatile LONG execAckState;
    volatile LONG execAckHr;
    volatile LONG execAckVisible;

    volatile LONG controllerDirection;

    volatile LONG controllerTouch;

    volatile LONG peerTick;
    volatile LONG peerState;
    volatile LONG peerEpoch;

    volatile LONG peerPid;

    volatile LONG peerUiThread;

    volatile LONG64 intentTaken;

    volatile LONG execCapable;

    // The bridge's own process, the word after execCapable at 236. The Start
    // side holds one synchronisation handle to it: a crash leaves the word at
    // 1 and the handle signalled, and the controller stops acting.
    volatile LONG execPid;
};

static_assert(offsetof(SwipeIpcState, magic) == 0, "layout");
static_assert(offsetof(SwipeIpcState, openSeq) == 8, "layout");
static_assert(offsetof(SwipeIpcState, openTick) == 16, "layout");
static_assert(offsetof(SwipeIpcState, initState) == 24, "layout");
static_assert(offsetof(SwipeIpcState, layoutBytes) == 28, "layout");
static_assert(offsetof(SwipeIpcState, initTick) == 32, "layout");
static_assert(offsetof(SwipeIpcState, intentSeq) == 40, "layout");
static_assert(offsetof(SwipeIpcState, intentTick) == 48, "layout");
static_assert(offsetof(SwipeIpcState, intentKind) == 56, "layout");
static_assert(offsetof(SwipeIpcState, intentTrigger) == 60, "layout");
static_assert(offsetof(SwipeIpcState, intentMethod) == 64, "layout");
static_assert(offsetof(SwipeIpcState, intentFlags) == 68, "layout");
static_assert(offsetof(SwipeIpcState, intentOpen) == 72, "layout");
static_assert(offsetof(SwipeIpcState, intentAckSeq) == 80, "layout");
static_assert(offsetof(SwipeIpcState, intentAckFor) == 88, "layout");
static_assert(offsetof(SwipeIpcState, intentAckState) == 96, "layout");
static_assert(offsetof(SwipeIpcState, intentAckDuration) == 100, "layout");
static_assert(offsetof(SwipeIpcState, intentAckEntry) == 104, "layout");
static_assert(offsetof(SwipeIpcState, intentAckHr) == 108, "layout");
static_assert(offsetof(SwipeIpcState, execSeq) == 112, "layout");
static_assert(offsetof(SwipeIpcState, execTick) == 120, "layout");
static_assert(offsetof(SwipeIpcState, execKind) == 128, "layout");
static_assert(offsetof(SwipeIpcState, execMethod) == 132, "layout");
static_assert(offsetof(SwipeIpcState, execFlags) == 136, "layout");
static_assert(offsetof(SwipeIpcState, execExecuteAt) == 144, "layout");
static_assert(offsetof(SwipeIpcState, execOpen) == 152, "layout");
static_assert(offsetof(SwipeIpcState, execTarget) == 160, "layout");
static_assert(offsetof(SwipeIpcState, execAckSeq) == 168, "layout");
static_assert(offsetof(SwipeIpcState, execAckFor) == 176, "layout");
static_assert(offsetof(SwipeIpcState, execAckState) == 184, "layout");
static_assert(offsetof(SwipeIpcState, execAckHr) == 188, "layout");
static_assert(offsetof(SwipeIpcState, execAckVisible) == 192, "layout");
static_assert(offsetof(SwipeIpcState, controllerDirection) == 196, "layout");
static_assert(offsetof(SwipeIpcState, controllerTouch) == 200, "layout");
static_assert(offsetof(SwipeIpcState, peerTick) == 204, "layout");
static_assert(offsetof(SwipeIpcState, peerState) == 208, "layout");
static_assert(offsetof(SwipeIpcState, peerEpoch) == 212, "layout");
static_assert(offsetof(SwipeIpcState, peerPid) == 216, "layout");
static_assert(offsetof(SwipeIpcState, peerUiThread) == 220, "layout");
static_assert(offsetof(SwipeIpcState, intentTaken) == 224, "layout");
static_assert(offsetof(SwipeIpcState, execCapable) == 232, "layout");
static_assert(offsetof(SwipeIpcState, execPid) == 236, "layout");
static_assert(sizeof(SwipeIpcState) == 240, "layout");

struct SwipeIpcChannel {
    SwipeIpcState* state;
    HANDLE mapping;
    HANDLE intentEvent;
    HANDLE intentAckEvent;
    HANDLE execEvent;
    HANDLE execAckEvent;
    HANDLE capabilityEvent;
};

inline LONG64 SwipeIpcLoad64(volatile LONG64* value) {
    return InterlockedCompareExchange64(value, 0, 0);
}

inline LONG64 SwipeIpcBeginWrite(volatile LONG64* sequence) {
    const LONG64 current = SwipeIpcLoad64(sequence);
    if ((current & 1) != 0) {
        return InterlockedExchangeAdd64(sequence, 2) + 2;
    }
    return InterlockedIncrement64(sequence);
}

inline LONG64 SwipeIpcEndWrite(volatile LONG64* sequence) {
    return InterlockedIncrement64(sequence);
}

template <typename Copier>
inline bool SwipeIpcSnapshot(volatile LONG64* sequence, LONG64* outSequence,
                             Copier&& copy) {
    for (int attempt = 0; attempt < kSwipeIpcReadAttempts; ++attempt) {
        const LONG64 before = SwipeIpcLoad64(sequence);
        if ((before & 1) != 0) {
            MemoryBarrier();
            continue;
        }
        copy();
        MemoryBarrier();
        if (SwipeIpcLoad64(sequence) != before) {
            continue;
        }
        *outSequence = before;
        return true;
    }
    return false;
}

inline bool SwipeIpcOpen(SwipeIpcChannel* channel, bool* created, DWORD* error) {
    *created = false;
    *error = 0;

    HANDLE mapping = CreateFileMappingW(INVALID_HANDLE_VALUE, nullptr,
                                        PAGE_READWRITE, 0,
                                        sizeof(SwipeIpcState),
                                        kSwipeIpcStateName);
    if (!mapping) {
        *error = GetLastError();
        return false;
    }

    auto* state = static_cast<SwipeIpcState*>(
        MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0,
                      sizeof(SwipeIpcState)));
    if (!state) {
        *error = GetLastError();
        CloseHandle(mapping);
        return false;
    }

    const ULONGLONG deadline = GetTickCount64() + kSwipeIpcOpenBudgetMs;
    const auto fail = [&](DWORD code) -> bool {
        *error = code;
        UnmapViewOfFile(state);
        CloseHandle(mapping);
        return false;
    };

    bool owner = false;
    for (;;) {
        const LONG observed = InterlockedCompareExchange(&state->initState, 0, 0);
        if (observed == kSwipeIpcReady) {
            break;
        }
        if (observed == kSwipeIpcBlank || observed == kSwipeIpcAbandoned) {

            state->initTick = static_cast<LONG64>(GetTickCount64());
            if (InterlockedCompareExchange(&state->initState,
                                           kSwipeIpcInitialising,
                                           observed) == observed) {
                owner = true;
                break;
            }

            if (GetTickCount64() >= deadline) {
                return fail(ERROR_NOT_READY);
            }
            continue;
        }
        if (observed != kSwipeIpcInitialising) {

            return fail(ERROR_INVALID_DATA);
        }

        const ULONGLONG claimed =
            static_cast<ULONGLONG>(SwipeIpcLoad64(&state->initTick));
        const ULONGLONG now = GetTickCount64();
        if (claimed != 0 && now > claimed &&
            now - claimed > kSwipeIpcInitAbandonMs) {
            InterlockedCompareExchange(&state->initState, kSwipeIpcAbandoned,
                                       kSwipeIpcInitialising);

            if (GetTickCount64() >= deadline) {
                return fail(ERROR_NOT_READY);
            }
            continue;
        }
        if (now >= deadline) {
            return fail(ERROR_NOT_READY);
        }
        Sleep(1);
    }

    if (owner) {
        state->magic = kSwipeIpcMagic;
        state->layoutBytes = static_cast<LONG>(sizeof(SwipeIpcState));
        MemoryBarrier();
        InterlockedExchange(&state->initState, kSwipeIpcReady);
        *created = true;
    }

    if (state->initState != kSwipeIpcReady ||
        state->magic != kSwipeIpcMagic ||
        state->layoutBytes != static_cast<LONG>(sizeof(SwipeIpcState))) {

        *error = ERROR_INVALID_DATA;
        UnmapViewOfFile(state);
        CloseHandle(mapping);
        return false;
    }

    HANDLE intentEvent =
        CreateEventW(nullptr, FALSE, FALSE, kSwipeIpcIntentEventName);
    HANDLE intentAckEvent =
        CreateEventW(nullptr, FALSE, FALSE, kSwipeIpcIntentAckEventName);
    HANDLE execEvent =
        CreateEventW(nullptr, FALSE, FALSE, kSwipeIpcExecEventName);
    HANDLE execAckEvent =
        CreateEventW(nullptr, FALSE, FALSE, kSwipeIpcExecAckEventName);
    HANDLE capabilityEvent =
        CreateEventW(nullptr, FALSE, FALSE, kSwipeIpcCapabilityEventName);

    *error = 0;
    channel->state = state;
    channel->mapping = mapping;
    channel->intentEvent = intentEvent;
    channel->intentAckEvent = intentAckEvent;
    channel->execEvent = execEvent;
    channel->execAckEvent = execAckEvent;
    channel->capabilityEvent = capabilityEvent;
    return true;
}

inline void SwipeIpcClose(SwipeIpcChannel* channel) {
    if (channel->state) {
        UnmapViewOfFile(channel->state);
    }
    if (channel->mapping) {
        CloseHandle(channel->mapping);
    }
    if (channel->intentEvent) {
        CloseHandle(channel->intentEvent);
    }
    if (channel->intentAckEvent) {
        CloseHandle(channel->intentAckEvent);
    }
    if (channel->execEvent) {
        CloseHandle(channel->execEvent);
    }
    if (channel->execAckEvent) {
        CloseHandle(channel->execAckEvent);
    }
    if (channel->capabilityEvent) {
        CloseHandle(channel->capabilityEvent);
    }
    *channel = SwipeIpcChannel{};
}

inline LONG64 SwipeIpcPublishOpen(SwipeIpcChannel* channel, uint64_t tickMs) {
    if (!channel->state) {
        return 0;
    }

    SwipeIpcBeginWrite(&channel->state->openSeq);
    InterlockedExchange64(&channel->state->openTick,
                          static_cast<LONG64>(tickMs));
    return SwipeIpcEndWrite(&channel->state->openSeq);
}

inline bool SwipeIpcReadOpen(SwipeIpcChannel* channel, uint64_t* generation,
                             uint64_t* tickMs, DWORD* error = nullptr) {
    if (error) {
        *error = 0;
    }
    if (!channel->state) {
        if (error) {
            *error = ERROR_INVALID_HANDLE;
        }
        return false;
    }

    uint64_t tick = 0;
    LONG64 sequence = 0;
    if (!SwipeIpcSnapshot(&channel->state->openSeq, &sequence, [&]() {
            tick = static_cast<uint64_t>(
                SwipeIpcLoad64(&channel->state->openTick));
        })) {
        if (error) {
            *error = ERROR_RETRY;
        }
        return false;
    }
    if (sequence == 0) {
        if (error) {
            *error = ERROR_NOT_READY;
        }
        return false;
    }

    *generation = static_cast<uint64_t>(sequence);
    *tickMs = tick;
    return true;
}

struct SwipeIpcIntent {
    LONG64 seq = 0;
    uint64_t tick = 0;
    int32_t kind = kSwipeIntentOpen;
    int32_t trigger = kSwipeTriggerWin;
    int32_t method = 0;
    int32_t flags = 0;
    uint64_t open = 0;
};

struct SwipeIpcIntentAck {
    LONG64 seq = 0;
    LONG64 intent = 0;
    int32_t state = kSwipeAckPending;
    int32_t durationMs = 0;
    int32_t entryPercent = 0;
    int32_t hr = 0;
};

struct SwipeIpcExecCommand {
    LONG64 seq = 0;
    uint64_t tick = 0;
    int32_t kind = kSwipeExecShow;
    int32_t method = 0;
    int32_t flags = 0;
    uint64_t executeAt = 0;
    uint64_t open = 0;
    int64_t target = 0;
};

struct SwipeIpcExecAck {
    LONG64 seq = 0;
    LONG64 command = 0;
    int32_t state = kSwipeAckPending;
    int32_t hr = 0;
    int32_t visible = 0;
};

inline LONG64 SwipeIpcPublishIntent(SwipeIpcChannel* channel, int32_t kind,
                                    int32_t trigger, int32_t method,
                                    int32_t flags, uint64_t open) {
    if (!channel->state) {
        return 0;
    }
    SwipeIpcBeginWrite(&channel->state->intentSeq);
    InterlockedExchange64(&channel->state->intentTick,
                          static_cast<LONG64>(GetTickCount64()));
    InterlockedExchange(&channel->state->intentKind, kind);
    InterlockedExchange(&channel->state->intentTrigger, trigger);
    InterlockedExchange(&channel->state->intentMethod, method);
    InterlockedExchange(&channel->state->intentFlags, flags);
    InterlockedExchange64(&channel->state->intentOpen,
                          static_cast<LONG64>(open));
    const LONG64 sequence = SwipeIpcEndWrite(&channel->state->intentSeq);
    if (channel->intentEvent) {
        SetEvent(channel->intentEvent);
    }
    return sequence;
}

inline bool SwipeIpcReadIntent(SwipeIpcChannel* channel, SwipeIpcIntent* out) {
    if (!channel->state) {
        return false;
    }
    SwipeIpcIntent record{};
    LONG64 sequence = 0;
    if (!SwipeIpcSnapshot(&channel->state->intentSeq, &sequence, [&]() {
            record.tick = static_cast<uint64_t>(
                SwipeIpcLoad64(&channel->state->intentTick));
            record.kind =
                InterlockedCompareExchange(&channel->state->intentKind, 0, 0);
            record.trigger = InterlockedCompareExchange(
                &channel->state->intentTrigger, 0, 0);
            record.method = InterlockedCompareExchange(
                &channel->state->intentMethod, 0, 0);
            record.flags = InterlockedCompareExchange(
                &channel->state->intentFlags, 0, 0);
            record.open = static_cast<uint64_t>(
                SwipeIpcLoad64(&channel->state->intentOpen));
        })) {
        return false;
    }
    record.seq = sequence;
    *out = record;
    return sequence != 0;
}

inline void SwipeIpcTakeIntent(SwipeIpcChannel* channel, LONG64 seq) {
    if (channel->state && seq > 0) {
        InterlockedExchange64(&channel->state->intentTaken, seq);
    }
}

inline LONG64 SwipeIpcTakenIntent(SwipeIpcChannel* channel) {
    return channel->state ? SwipeIpcLoad64(&channel->state->intentTaken) : 0;
}

inline LONG64 SwipeIpcIntentSequence(SwipeIpcChannel* channel) {
    if (!channel->state) {
        return 0;
    }
    const LONG64 sequence = SwipeIpcLoad64(&channel->state->intentSeq);
    return (sequence & 1) != 0 ? 0 : sequence;
}

inline bool SwipeIpcAbortIntent(SwipeIpcChannel* channel, LONG64 seq) {
    if (!channel->state || seq <= 0 || (seq & 1) != 0) {
        return false;
    }
    return InterlockedCompareExchange64(&channel->state->intentSeq, seq + 1,
                                        seq) == seq;
}

inline bool SwipeIpcAnswerIntent(SwipeIpcChannel* channel, LONG64 intentSeq,
                                 int32_t state, int32_t durationMs,
                                 int32_t entryPercent, int32_t hr) {
    if (!channel->state || intentSeq <= 0) {
        return false;
    }
    SwipeIpcBeginWrite(&channel->state->intentAckSeq);
    InterlockedExchange64(&channel->state->intentAckFor, intentSeq);
    InterlockedExchange(&channel->state->intentAckState, state);
    InterlockedExchange(&channel->state->intentAckDuration, durationMs);
    InterlockedExchange(&channel->state->intentAckEntry, entryPercent);
    InterlockedExchange(&channel->state->intentAckHr, hr);
    SwipeIpcEndWrite(&channel->state->intentAckSeq);
    return channel->intentAckEvent &&
           SetEvent(channel->intentAckEvent) != FALSE;
}

inline bool SwipeIpcReadIntentAck(SwipeIpcChannel* channel,
                                  SwipeIpcIntentAck* out) {
    if (!channel->state) {
        return false;
    }
    SwipeIpcIntentAck record{};
    LONG64 sequence = 0;
    if (!SwipeIpcSnapshot(&channel->state->intentAckSeq, &sequence, [&]() {
            record.intent = SwipeIpcLoad64(
                &channel->state->intentAckFor);
            record.state = InterlockedCompareExchange(
                &channel->state->intentAckState, 0, 0);
            record.durationMs = InterlockedCompareExchange(
                &channel->state->intentAckDuration, 0, 0);
            record.entryPercent = InterlockedCompareExchange(
                &channel->state->intentAckEntry, 0, 0);
            record.hr = InterlockedCompareExchange(
                &channel->state->intentAckHr, 0, 0);
        })) {
        return false;
    }
    record.seq = sequence;
    *out = record;
    return sequence != 0;
}

inline LONG64 SwipeIpcPublishExec(SwipeIpcChannel* channel, int32_t kind,
                                  int32_t method, int32_t flags,
                                  uint64_t executeAt, uint64_t open,
                                  int64_t target = 0) {
    if (!channel->state) {
        return 0;
    }
    SwipeIpcBeginWrite(&channel->state->execSeq);
    InterlockedExchange64(&channel->state->execTick,
                          static_cast<LONG64>(GetTickCount64()));
    InterlockedExchange(&channel->state->execKind, kind);
    InterlockedExchange(&channel->state->execMethod, method);
    InterlockedExchange(&channel->state->execFlags, flags);
    InterlockedExchange64(&channel->state->execExecuteAt,
                          static_cast<LONG64>(executeAt));
    InterlockedExchange64(&channel->state->execOpen,
                          static_cast<LONG64>(open));
    InterlockedExchange64(&channel->state->execTarget, target);
    const LONG64 sequence = SwipeIpcEndWrite(&channel->state->execSeq);
    if (channel->execEvent) {
        SetEvent(channel->execEvent);
    }
    return sequence;
}

inline bool SwipeIpcReadExec(SwipeIpcChannel* channel,
                             SwipeIpcExecCommand* out) {
    if (!channel->state) {
        return false;
    }
    SwipeIpcExecCommand record{};
    LONG64 sequence = 0;
    if (!SwipeIpcSnapshot(&channel->state->execSeq, &sequence, [&]() {
            record.tick = static_cast<uint64_t>(
                SwipeIpcLoad64(&channel->state->execTick));
            record.kind = InterlockedCompareExchange(
                &channel->state->execKind, 0, 0);
            record.method = InterlockedCompareExchange(
                &channel->state->execMethod, 0, 0);
            record.flags = InterlockedCompareExchange(
                &channel->state->execFlags, 0, 0);
            record.executeAt = static_cast<uint64_t>(
                SwipeIpcLoad64(&channel->state->execExecuteAt));
            record.open = static_cast<uint64_t>(
                SwipeIpcLoad64(&channel->state->execOpen));
            record.target = static_cast<int64_t>(
                SwipeIpcLoad64(&channel->state->execTarget));
        })) {
        return false;
    }
    record.seq = sequence;
    *out = record;
    return sequence != 0;
}

inline bool SwipeIpcAnswerExec(SwipeIpcChannel* channel, LONG64 execSeq,
                               int32_t state, int32_t hr, int32_t visible) {
    if (!channel->state || execSeq <= 0) {
        return false;
    }
    SwipeIpcBeginWrite(&channel->state->execAckSeq);
    InterlockedExchange64(&channel->state->execAckFor, execSeq);
    InterlockedExchange(&channel->state->execAckState, state);
    InterlockedExchange(&channel->state->execAckHr, hr);
    InterlockedExchange(&channel->state->execAckVisible, visible);
    SwipeIpcEndWrite(&channel->state->execAckSeq);
    return channel->execAckEvent &&
           SetEvent(channel->execAckEvent) != FALSE;
}

inline bool SwipeIpcReadExecAck(SwipeIpcChannel* channel,
                                SwipeIpcExecAck* out) {
    if (!channel->state) {
        return false;
    }
    SwipeIpcExecAck record{};
    LONG64 sequence = 0;
    if (!SwipeIpcSnapshot(&channel->state->execAckSeq, &sequence, [&]() {
            record.command = SwipeIpcLoad64(
                &channel->state->execAckFor);
            record.state = InterlockedCompareExchange(
                &channel->state->execAckState, 0, 0);
            record.hr = InterlockedCompareExchange(
                &channel->state->execAckHr, 0, 0);
            record.visible = InterlockedCompareExchange(
                &channel->state->execAckVisible, 0, 0);
        })) {
        return false;
    }
    record.seq = sequence;
    *out = record;
    return sequence != 0;
}

constexpr int32_t kSwipeDirectionUnowned = 0;
constexpr int32_t kSwipeDirectionOpen = 1;
constexpr int32_t kSwipeDirectionClosing = 2;

inline void SwipeIpcSetControllerDirection(SwipeIpcChannel* channel,
                                           int32_t direction) {
    if (channel->state) {
        InterlockedExchange(&channel->state->controllerDirection, direction);
    }
}

inline int32_t SwipeIpcControllerDirection(SwipeIpcChannel* channel) {
    return channel->state
               ? InterlockedCompareExchange(
                     &channel->state->controllerDirection, 0, 0)
               : kSwipeDirectionUnowned;
}

inline void SwipeIpcSetControllerTouch(SwipeIpcChannel* channel, bool touch) {
    if (channel->state) {
        InterlockedExchange(&channel->state->controllerTouch, touch ? 1 : 0);
    }
}

inline bool SwipeIpcControllerTouch(SwipeIpcChannel* channel) {
    return channel->state &&
           InterlockedCompareExchange(&channel->state->controllerTouch, 0,
                                      0) != 0;
}

inline void SwipeIpcPublishPeerTick(SwipeIpcChannel* channel, uint64_t tickMs) {
    if (channel->state) {
        InterlockedExchange(
            &channel->state->peerTick,
            static_cast<LONG>(static_cast<uint32_t>(tickMs)));
    }
}

inline void SwipeIpcSetPeerState(SwipeIpcChannel* channel, int32_t state) {
    if (channel->state) {
        InterlockedExchange(&channel->state->peerState, state);
    }
}

inline uint32_t SwipeIpcBeginPeerActivation(SwipeIpcChannel* channel) {
    if (!channel->state) {
        return 0;
    }
    return static_cast<uint32_t>(
        InterlockedIncrement(&channel->state->peerEpoch));
}

inline void SwipeIpcSetPeerPid(SwipeIpcChannel* channel, uint32_t pid) {
    if (channel->state) {
        InterlockedExchange(&channel->state->peerPid,
                            static_cast<LONG>(pid));
    }
}

inline uint32_t SwipeIpcPeerPid(SwipeIpcChannel* channel) {
    return channel->state
               ? static_cast<uint32_t>(
                     InterlockedCompareExchange(&channel->state->peerPid, 0, 0))
               : 0;
}

inline uint32_t SwipeIpcPeerEpoch(SwipeIpcChannel* channel) {
    return channel->state
               ? static_cast<uint32_t>(
                     InterlockedCompareExchange(&channel->state->peerEpoch, 0,
                                                0))
               : 0;
}

inline void SwipeIpcSetPeerUiThread(SwipeIpcChannel* channel, uint32_t tid) {
    if (channel->state) {
        InterlockedExchange(&channel->state->peerUiThread,
                            static_cast<LONG>(tid));
    }
}

inline uint32_t SwipeIpcPeerUiThread(SwipeIpcChannel* channel) {
    return channel->state
               ? static_cast<uint32_t>(InterlockedCompareExchange(
                     &channel->state->peerUiThread, 0, 0))
               : 0;
}

inline int32_t SwipeIpcPeerState(SwipeIpcChannel* channel) {
    return channel->state
               ? InterlockedCompareExchange(&channel->state->peerState, 0, 0)
               : kSwipePeerGone;
}

inline uint32_t SwipeIpcPeerTick(SwipeIpcChannel* channel) {
    return channel->state
               ? static_cast<uint32_t>(InterlockedCompareExchange(
                     &channel->state->peerTick, 0, 0))
               : 0;
}

inline bool SwipeIpcPeerAlive(SwipeIpcChannel* channel, uint32_t staleMs) {
    if (!channel->state) {
        return false;
    }
    if (InterlockedCompareExchange(&channel->state->peerState, 0, 0) !=
        kSwipePeerAlive) {
        return false;
    }
    const uint32_t tick = static_cast<uint32_t>(
        InterlockedCompareExchange(&channel->state->peerTick, 0, 0));
    const uint32_t now = static_cast<uint32_t>(GetTickCount64());
    if (InterlockedCompareExchange(&channel->state->peerState, 0, 0) !=
        kSwipePeerAlive) {
        return false;
    }
    return now - tick < staleMs;
}

inline void SwipeIpcPublishExecCapable(SwipeIpcChannel* channel, bool capable) {
    if (!channel->state) {
        return;
    }
    InterlockedExchange(&channel->state->execCapable, capable ? 1 : 0);
    if (channel->capabilityEvent) {
        SetEvent(channel->capabilityEvent);
    }
}

inline bool SwipeIpcExecCapable(SwipeIpcChannel* channel) {
    return channel->state != nullptr &&
           InterlockedCompareExchange(&channel->state->execCapable, 0, 0) == 1;
}

inline void SwipeIpcSetExecPid(SwipeIpcChannel* channel, uint32_t pid) {
    if (channel->state) {
        InterlockedExchange(&channel->state->execPid, static_cast<LONG>(pid));
    }
}

inline uint32_t SwipeIpcExecPid(SwipeIpcChannel* channel) {
    return channel->state
               ? static_cast<uint32_t>(
                     InterlockedCompareExchange(&channel->state->execPid, 0, 0))
               : 0;
}
#pragma clang diagnostic pop
// ---- end swipe_ipc_shared.h ----
#if defined(__clang__)
#define NO_DESTROY [[clang::no_destroy]]
#else
#define NO_DESTROY
#endif

struct SwipeModRole {
    BOOL (*modInit)();
    void (*modAfterInit)();
    void (*modBeforeUninit)();
    void (*modUninit)();
    void (*modSettingsChanged)();
};

constexpr uint32_t kTraceMaxLines = 20000;

SRWLOCK g_traceLock = SRWLOCK_INIT;
std::atomic<bool> g_traceFile{false};

std::atomic<bool> g_traceCapFailed{false};
const wchar_t* g_traceFileName = L"trace.log";
uint32_t g_traceLines = 0;
bool g_traceSeeded = false;

void SetTraceFileName(const wchar_t* name) {
    g_traceFileName = name;
}

bool SeedTraceLines(const wchar_t* path) {
    g_traceLines = 0;
    HANDLE file = CreateFileW(path, GENERIC_READ,
                              FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        const DWORD error = GetLastError();
        return error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
    }
    char buffer[8192];
    DWORD read = 0;
    bool complete = true;
    for (;;) {
        if (!ReadFile(file, buffer, sizeof(buffer), &read, nullptr)) {
            complete = false;
            break;
        }
        if (read == 0) {
            break;
        }
        for (DWORD i = 0; i < read; ++i) {
            if (buffer[i] == '\n') {
                ++g_traceLines;
            }
        }
    }
    CloseHandle(file);
    if (!complete) {
        g_traceLines = 0;
    }
    return complete;
}

void RefreshTraceSetting() {
    g_traceFile.store(Wh_GetIntSetting(L"DiagnosticTrace") != 0,
                      std::memory_order_relaxed);
    g_traceCapFailed.store(false, std::memory_order_relaxed);
    AcquireSRWLockExclusive(&g_traceLock);
    g_traceSeeded = false;
    ReleaseSRWLockExclusive(&g_traceLock);
}

void TraceLog(const wchar_t* format, ...) {
    WCHAR message[900] = {};
    va_list args;
    va_start(args, format);
    _vsnwprintf_s(message, ARRAYSIZE(message), _TRUNCATE, format, args);
    va_end(args);

    Wh_Log(L"%s", message);

    if (!g_traceFile.load(std::memory_order_relaxed) ||
        g_traceCapFailed.load(std::memory_order_relaxed)) {
        return;
    }
    WCHAR storage[MAX_PATH] = {};
    if (!Wh_GetModStoragePath(storage, ARRAYSIZE(storage))) {
        return;
    }
    WCHAR path[MAX_PATH] = {};
    if (_snwprintf_s(path, ARRAYSIZE(path), _TRUNCATE, L"%s\\%s", storage,
                     g_traceFileName) <= 0) {
        return;
    }

    WCHAR line[1024] = {};
    _snwprintf_s(line, ARRAYSIZE(line), _TRUNCATE, L"t=%llu %s",
                 static_cast<unsigned long long>(GetTickCount64()), message);

    char utf8[4098] = {};
    int bytes = WideCharToMultiByte(CP_UTF8, 0, line, -1, utf8, sizeof(utf8) - 2,
                                    nullptr, nullptr);
    if (bytes <= 1) {
        return;
    }

    utf8[bytes - 1] = '\r';
    utf8[bytes] = '\n';
    const DWORD lineBytes = static_cast<DWORD>(bytes + 1);

    AcquireSRWLockExclusive(&g_traceLock);
    if (!g_traceSeeded) {
        g_traceSeeded = SeedTraceLines(path);
    }

    if (g_traceLines >= kTraceMaxLines) {
        HANDLE cut = CreateFileW(path, GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                                 OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        DWORD error = 0;
        bool truncated = false;
        if (cut != INVALID_HANDLE_VALUE) {
            LARGE_INTEGER zero{};
            const BOOL moved = SetFilePointerEx(cut, zero, nullptr, FILE_BEGIN);
            truncated = moved && SetEndOfFile(cut);
            error = truncated ? 0 : GetLastError();
            CloseHandle(cut);
        } else {
            error = GetLastError();
        }
        if (!truncated) {
            g_traceCapFailed.store(true, std::memory_order_relaxed);
            ReleaseSRWLockExclusive(&g_traceLock);
            Wh_Log(L"DiagnosticTrace stopped: the trace file could not be "
                   L"truncated at its %u-line cap (err=%u); Wh_Log keeps "
                   L"receiving the lines",
                   kTraceMaxLines, error);
            return;
        }
        g_traceLines = 0;
    }
    HANDLE file = CreateFileW(path, FILE_APPEND_DATA,
                              FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                              OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file != INVALID_HANDLE_VALUE) {
        DWORD written = 0;
        const BOOL ok = WriteFile(file, utf8, lineBytes, &written, nullptr);

        if (ok && written == lineBytes && g_traceSeeded) {
            ++g_traceLines;
        }
        CloseHandle(file);
    }
    ReleaseSRWLockExclusive(&g_traceLock);
}

constexpr uint32_t kSwipeShowFlagMask = 0x8 | 0x4;
constexpr uint32_t kSwipeShowMethodKeyboard = 1;
constexpr uint32_t kSwipeShowMethodStartButton = 11;
constexpr uint32_t kSwipeShowMethodTouch = 16;
constexpr uint32_t kSwipeDismissMethodWin = 1;
constexpr uint32_t kSwipeDismissMethodStartButton = 6;
constexpr uint32_t kSwipeDismissMethodClickAwayEvidence = 7;

bool SwipeShowAllowed(uint32_t method, uint32_t flags) {
    return (method == kSwipeShowMethodKeyboard ||
            method == kSwipeShowMethodStartButton ||
            method == kSwipeShowMethodTouch) &&
           (flags & ~kSwipeShowFlagMask) == 0;
}

bool SwipeDismissAllowed(uint32_t method) {
    return method == kSwipeDismissMethodWin ||
           method == kSwipeDismissMethodStartButton;
}

bool SwipeCloseIntentAllowed(uint32_t method) {
    return method == kSwipeDismissMethodWin ||
           method == kSwipeDismissMethodStartButton ||
           method == kSwipeDismissMethodClickAwayEvidence;
}

constexpr wchar_t kModVersion[] = L"0.8.6";

namespace start {

namespace wuc = winrt::Windows::UI::Composition;
namespace wucore = winrt::Windows::UI::Core;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxh = winrt::Windows::UI::Xaml::Hosting;

using winrt::Windows::Foundation::Numerics::float2;
using winrt::Windows::Foundation::Numerics::float3;

constexpr uintptr_t kPrepareRootElementExpectedRva = 0xAD730;
constexpr wchar_t kStartUIModuleName[] = L"StartUI_.dll";

constexpr double kWin10SpringDamping = 0.9;
constexpr double kWin11SpringDamping = 1.72;
constexpr double kSettleTimeConstants = 5.0;

constexpr float kSettleSpeedDipPerMs = 2.2f;
constexpr std::chrono::milliseconds kSettleMinDuration{160};
constexpr std::chrono::milliseconds kSettleMaxDuration{520};

constexpr double kSettleSegmentMs = 4.0;
constexpr int kSettleMinSteps = 12;
constexpr int kSettleMaxSteps = 128;

constexpr int kSettleMaxFrames = kSettleMaxSteps + 1;

constexpr int kGestureStartSpeedDefault = 100;
constexpr int kGestureStartSpeedMin = 0;
constexpr int kGestureStartSpeedMax = 300;

constexpr int kCommitDistancePercentDefault = 10;
constexpr int kCommitDistancePercentMin = 1;
constexpr int kCommitDistancePercentMax = 50;

constexpr uint64_t kCloseRepairMs = 800;

static_assert(static_cast<uint64_t>(kSettleMaxDuration.count()) <=
                  kCloseRepairMs,
              "the close dismiss must run inside the repair bound");

constexpr float kCommitVelocity = 0.45f;
constexpr float kScrollAbortPerp = 120.0f;
constexpr float kScrollAbortRatio = 1.5f;
constexpr float kScrollAbortMaxDelta = 60.0f;

constexpr float kProgressEpsilonDip = 0.5f;
constexpr float kWindowMarginDip = 4.0f;

constexpr float kScrollerEndSlopDip = 24.0f;
constexpr float kPanelBandSlopDip = 2.0f;
constexpr float kScrollerAxisSlopDip = 0.5f;
constexpr uint32_t kScrollerScanMaxDepth = 32;

constexpr uint64_t kDismissRejectLogMs = 500;

constexpr float kEdgeBandDip = 12.0f;

constexpr float kAdoptTravelDip = 24.0f;

constexpr float kBandTailTravelDip = 8.0f;

constexpr int kEndVelocitySamples = 8;

constexpr uint64_t kIdleReleaseMs = 18;

constexpr uint64_t kCandidateIdleHoldMs = 120;

constexpr UINT_PTR kIdleTimerId = 1;

constexpr uint64_t kOpenCheckIntervalMs = 20;

constexpr uint64_t kPeerHeartbeatMs = kSwipePeerTickIntervalMs;

constexpr int kRendererWaitRetries = 20;

constexpr uint64_t kRootProbeMs = 100;

constexpr uint64_t kCapabilityProbeMs = 100;

constexpr uint64_t kVisibilityFallbackMs = 100;

constexpr uint64_t kVisibilityGraceMs = 1000;

constexpr uint64_t kVisibilitySettleWatchMs = 500;

constexpr float kPanelLayoutMarginDip = 2.0f;
constexpr float kPanelAnchorMarginDip = 2.0f;

constexpr uint64_t kSettledMeasureDeadlineMs = 400;

constexpr uint64_t kCoastStartMs = 8;
constexpr std::chrono::milliseconds kCoastDuration{200};

constexpr float kCoastMinVelocityDip = 0.1f;

constexpr float kCoastMaxVelocityDip = 6.0f;

constexpr uint64_t kRevealCoastSettleMs = 50;

constexpr uint64_t kRevealChaseMaxStepMs = 50;

constexpr uint64_t kRevealEntryHoldMs = 30;

constexpr int kMaxHidDevices = 4;
constexpr int kMaxContactCollections = 8;
constexpr int kMaxLinkNodes = 64;
constexpr int kHidPathChars = 512;
constexpr wchar_t kHidWindowClass[] =
    L"WindhawkLegacyStartSwipeRawInputWindow";

constexpr UINT kInputWakeMessage = WM_APP + 0x61;

constexpr USAGE kUsagePageGeneric = 0x01;
constexpr USAGE kUsagePageDigitizer = 0x0D;
constexpr USAGE kUsageTouchScreen = 0x04;
constexpr USAGE kUsagePen = 0x02;
constexpr USAGE kUsageContact = 0x05;
constexpr USAGE kUsageFinger = 0x22;

constexpr USAGE kUsageStylus = 0x20;
constexpr USAGE kUsageX = 0x30;
constexpr USAGE kUsageY = 0x31;
constexpr USAGE kUsageInRange = 0x32;
constexpr USAGE kUsageTipSwitch = 0x42;

constexpr USAGE kUsageContactIdentifier = 0x51;
constexpr USAGE kUsageContactCount = 0x54;

constexpr GUID kCoreWindowInteropIid = {
    0x45D64A29, 0xA63E, 0x4CB6,
    {0xB4, 0x98, 0x57, 0x81, 0xD2, 0x98, 0xCB, 0x4F}};
using GetCoreWindowHandleAbi = HRESULT(STDMETHODCALLTYPE*)(void*, HWND*);

constexpr DWORD kDwmwaCloaked = 14;
using DwmGetWindowAttribute_t = HRESULT(WINAPI*)(HWND, DWORD, PVOID, DWORD);

constexpr UINT kContactTimerPeriodMs = 1;
using TimePeriod_t = UINT(WINAPI*)(UINT);

constexpr DWORD kCreateWaitableTimerHighResolution = 0x00000002;

std::atomic<TimePeriod_t> g_timeBeginPeriod{nullptr};
std::atomic<TimePeriod_t> g_timeEndPeriod{nullptr};

std::atomic<bool> g_highResolutionDeadlineTimer{false};

bool g_timerPeriodHeld = false;

void BeginContactTiming() {
    if (g_timerPeriodHeld ||
        g_highResolutionDeadlineTimer.load(std::memory_order_relaxed)) {
        return;
    }

    const TimePeriod_t begin =
        g_timeBeginPeriod.load(std::memory_order_relaxed);
    if (!begin || begin(kContactTimerPeriodMs) != 0) {

        return;
    }

    g_timerPeriodHeld = true;
}

void EndContactTiming() {
    if (!g_timerPeriodHeld) {
        return;
    }

    g_timerPeriodHeld = false;
    const TimePeriod_t end = g_timeEndPeriod.load(std::memory_order_relaxed);
    if (end) {
        end(kContactTimerPeriodMs);
    }
}

HANDLE CreateDeadlineTimer() {
    return CreateWaitableTimerExW(nullptr, nullptr,
                                 kCreateWaitableTimerHighResolution,
                                 TIMER_ALL_ACCESS);
}

bool ArmDeadlineTimer(HANDLE timer, uint64_t delayMs) {
    LARGE_INTEGER due{};
    due.QuadPart = -static_cast<LONGLONG>(delayMs) * 10000;
    return SetWaitableTimer(timer, &due, 0, nullptr, nullptr, FALSE) != FALSE;
}

void DisarmDeadlineTimer(HANDLE timer) {
    CancelWaitableTimer(timer);
}

enum : uint32_t {
    kVisibilityWantNone = 0,
    kVisibilityWantOpen = 1,
    kVisibilityWantClosed = 2,
};

uint64_t g_visibilityPollUntil = 0;
uint32_t g_visibilityWant = kVisibilityWantNone;
uint64_t g_lastVisibilityPollTick = 0;

// An unconfirmed dismiss can leave a visible menu at Progress=0. The open
// sample is the evidence that the menu owns the rest position; this watch is
// armed by that ack and spent by the first open sample.
uint64_t g_unconfirmedRepairUntil = 0;

void NoteVisibilityTransition(uint32_t want) {
    g_visibilityWant = want;
    const uint64_t now = GetTickCount64();
    const uint64_t until = now + kVisibilityGraceMs;
    if (until > g_visibilityPollUntil) {
        g_visibilityPollUntil = until;
    }
    g_lastVisibilityPollTick = now;
}

uint64_t QpcMsNow() {
    static const uint64_t frequency = []() {
        LARGE_INTEGER value{};
        QueryPerformanceFrequency(&value);
        return value.QuadPart > 0 ? static_cast<uint64_t>(value.QuadPart)
                                  : 1;
    }();

    LARGE_INTEGER counter{};
    QueryPerformanceCounter(&counter);
    return static_cast<uint64_t>(counter.QuadPart) * 1000 / frequency;
}

enum class DismissAxis : int32_t {
    Left = 0,
    Top = 1,
    Right = 2,
    Bottom = 3,
};

bool IsHorizontalAxis(int32_t axis) {
    return axis == static_cast<int32_t>(DismissAxis::Left) ||
           axis == static_cast<int32_t>(DismissAxis::Right);
}

void AxisTranslation(int32_t axis, float width, float height, float& dirX,
                     float& dirY, float& distance) {
    switch (static_cast<DismissAxis>(axis)) {
        case DismissAxis::Left:
            dirX = -1.0f;
            dirY = 0.0f;
            distance = width;
            break;
        case DismissAxis::Top:
            dirX = 0.0f;
            dirY = -1.0f;
            distance = height;
            break;
        case DismissAxis::Right:
            dirX = 1.0f;
            dirY = 0.0f;
            distance = width;
            break;
        case DismissAxis::Bottom:
            dirX = 0.0f;
            dirY = 1.0f;
            distance = height;
            break;
    }
}

std::atomic<bool> g_disabled{false};
std::atomic<bool> g_unloading{false};
std::atomic<bool> g_stopDismissCallback{false};

bool g_initialised = false;

uint64_t g_unloadStartTick = 0;

HANDLE g_moduleEntriesGone = nullptr;

std::atomic<DWORD> g_hidThreadId{0};

std::atomic<int> g_moduleEntryCount{0};

std::atomic<bool> g_uiTurnEverUsed{false};

std::atomic<int> g_uiOwnerDepth{0};
std::atomic<bool> g_uiTeardownPending{false};
HANDLE g_uiTeardownAck = nullptr;

SRWLOCK g_rootAdmissionLock = SRWLOCK_INIT;

class RootAdmissionShared {
  public:
    RootAdmissionShared() {
        if (TryAcquireSRWLockShared(&g_rootAdmissionLock)) {
            held_ = true;
            return;
        }

        if (!g_unloading.load(std::memory_order_relaxed)) {
            AcquireSRWLockShared(&g_rootAdmissionLock);
            held_ = true;
        }
    }
    ~RootAdmissionShared() {
        if (held_) {
            ReleaseSRWLockShared(&g_rootAdmissionLock);
        }
    }
    RootAdmissionShared(const RootAdmissionShared&) = delete;
    RootAdmissionShared& operator=(const RootAdmissionShared&) = delete;

  private:
    bool held_ = false;
};

class RootAdmissionExclusive {
  public:
    RootAdmissionExclusive() {
        AcquireSRWLockExclusive(&g_rootAdmissionLock);
    }
    ~RootAdmissionExclusive() {
        ReleaseSRWLockExclusive(&g_rootAdmissionLock);
    }
    RootAdmissionExclusive(const RootAdmissionExclusive&) = delete;
    RootAdmissionExclusive& operator=(const RootAdmissionExclusive&) = delete;
};

class ModuleEntryLease {
  public:
    ModuleEntryLease() {
        if (!g_uiTurnEverUsed.exchange(true, std::memory_order_release)) {
            TraceLog(L"module ui turn entry first used");
        }
        g_moduleEntryCount.fetch_add(1, std::memory_order_seq_cst);
    }
    ~ModuleEntryLease() {
        if (g_moduleEntryCount.fetch_sub(1, std::memory_order_acq_rel) == 1 &&
            g_moduleEntriesGone) {

            SetEvent(g_moduleEntriesGone);
        }
    }

    ModuleEntryLease(const ModuleEntryLease&) = delete;
    ModuleEntryLease& operator=(const ModuleEntryLease&) = delete;
};

void TeardownUi();

constexpr UINT kUiDeferredTeardownMessage = WM_APP + 0x65;
constexpr WPARAM kUiDeferredTeardownMagic = 0x53575444;
extern std::atomic<HWND> g_uiRendezvousHwnd;

class UiDepthGuard {
  public:
    UiDepthGuard() { ++g_uiOwnerDepth; }
    ~UiDepthGuard() {
        if (--g_uiOwnerDepth > 0 || !g_uiTeardownPending) {
            return;
        }
        g_uiTeardownPending = false;
        const HWND hwnd = g_uiRendezvousHwnd.load(std::memory_order_acquire);
        if (!hwnd ||
            !PostMessageW(hwnd, kUiDeferredTeardownMessage,
                          kUiDeferredTeardownMagic, 0)) {

            TraceLog(L"ui deferred teardown post failed hwnd=%p err=%u", hwnd,
                    hwnd ? GetLastError() : 0);
        }
    }

    UiDepthGuard(const UiDepthGuard&) = delete;
    UiDepthGuard& operator=(const UiDepthGuard&) = delete;
};

enum class SettleStyle : int32_t {
    Win11 = 0,
    Win10 = 1,
};

constexpr wchar_t kAnimationStyleWin10[] = L"Windows 10";

std::atomic<SettleStyle> g_settleStyle{SettleStyle::Win11};

const wchar_t* SettleStyleName(SettleStyle style) {
    return style == SettleStyle::Win10 ? L"win10" : L"win11";
}

void LoadAnimationStyle() {
    PCWSTR value = Wh_GetStringSetting(L"AnimationStyle");
    const SettleStyle style =
        wcscmp(value, kAnimationStyleWin10) == 0 ? SettleStyle::Win10
                                                 : SettleStyle::Win11;
    TraceLog(L"settle style selected=%s setting=%s", SettleStyleName(style),
            value);
    Wh_FreeStringSetting(value);
    g_settleStyle.store(style, std::memory_order_relaxed);
}

std::atomic<int> g_gestureStartSpeedPercent{kGestureStartSpeedDefault};

void LoadGestureStartSpeed() {
    const int percent =
        std::clamp(Wh_GetIntSetting(L"GestureStartSpeedPercent"),
                   kGestureStartSpeedMin, kGestureStartSpeedMax);
    g_gestureStartSpeedPercent.store(percent, std::memory_order_relaxed);
    TraceLog(L"settle start speed=%d%%", percent);
}

std::atomic<int> g_commitDistancePercent{kCommitDistancePercentDefault};

void LoadCommitDistance() {
    const int percent = std::clamp(Wh_GetIntSetting(L"CommitDistancePercent"),
                                   kCommitDistancePercentMin,
                                   kCommitDistancePercentMax);
    g_commitDistancePercent.store(percent, std::memory_order_relaxed);
    TraceLog(L"gesture commit distance=%d%%", percent);
}

constexpr float kProgressRest = 1.0f;

std::mutex g_compositionMutex;
NO_DESTROY wuc::Compositor g_compositor{nullptr};
NO_DESTROY wuc::CompositionPropertySet g_state{nullptr};
NO_DESTROY wuc::ExpressionAnimation g_translationExpression{nullptr};
NO_DESTROY wuc::Visual g_visual{nullptr};

NO_DESTROY wuc::CompositionScopedBatch g_dismissBatch{nullptr};

winrt::event_token g_dismissBatchToken{};

std::shared_ptr<ModuleEntryLease> g_settleLease;

struct PanelController {

    std::atomic<float> progress{kProgressRest};
    std::atomic<bool> coasting{false};

    float samples[kSettleMaxFrames] = {};
    int frames = 0;
    uint64_t startQpc = 0;
    uint64_t durationMs = 0;
    uint64_t coastStartQpc = 0;
    float coastFrom = 1.0f;
    float coastTo = 1.0f;

    void Begin(const float* position);
    bool Move(float position);
    void End(float to, float endVelocity);
    void Commit(float to);
};

PanelController g_panel;

NO_DESTROY wux::FrameworkElement g_root{nullptr};

NO_DESTROY wucore::CoreDispatcher g_dispatcher{nullptr};
std::atomic<DWORD> g_uiThreadId{0};
std::atomic<uint32_t> g_setupGeneration{0};

winrt::event_token g_layoutToken{};
uint32_t g_layoutTicksLeft = 0;

NO_DESTROY wux::FrameworkElement g_layoutRoot{nullptr};

uint32_t g_layoutWatchSerial = 0;
uint32_t g_layoutWatchMint = 0;

enum class ScrollerState {
    Unknown = 0,
    Nothing = 1,
    Scrollable = 2,
};

constexpr int kMaxScrollRegions = 6;
constexpr int kScrollerLabelChars = 32;

struct ScrollerRegion {
    float left = 0.0f;
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;
    float verticalOffset = 0.0f;
    float scrollableHeight = 0.0f;
    uint32_t depth = 0;
    int watchSlot = -1;

    wchar_t label[kScrollerLabelChars] = {};
};

struct ScrollerCache {
    ScrollerState state = ScrollerState::Unknown;
    uint32_t generation = 0;

    uint32_t layoutVersion = 0;
    int count = 0;
    ScrollerRegion regions[kMaxScrollRegions];
};

std::mutex g_scrollerMutex;
ScrollerCache g_scroller;

struct ScrollerWatch {
    wuxc::ScrollViewer owner{nullptr};
    wuxc::ScrollViewer viewer{nullptr};
    winrt::event_token token{};
};
NO_DESTROY std::optional<std::array<ScrollerWatch, kMaxScrollRegions>>
    g_scrollerWatches;

std::array<ScrollerWatch, kMaxScrollRegions>& ScrollerWatches() {
    if (!g_scrollerWatches) {
        g_scrollerWatches.emplace();
    }
    return *g_scrollerWatches;
}

bool g_scrollerNoneLogged = false;
bool g_scrollerOverflowLogged = false;
bool g_scrollerRegionLogged[kMaxScrollRegions] = {};
float g_scrollerLoggedRect[kMaxScrollRegions][4] = {};

uint64_t g_scrollerRejectLogTick = 0;

uint64_t g_bandTailLogTick = 0;

uint64_t g_closeCancelledTick = 0;

winrt::event_token g_loadedToken{};
winrt::event_token g_sizeToken{};

NO_DESTROY wux::FrameworkElement g_loadedRoot{nullptr};
NO_DESTROY wux::FrameworkElement g_sizeRoot{nullptr};

std::atomic<bool> g_dismissSettling{false};
std::atomic<uint32_t> g_dismissSerial{0};

enum class CloseKind {
    None,
    Dismiss,
    RevealCancel,
};

enum class ClosePhase {
    Settling,
    AwaitingHide,
    Done,
};

struct CloseRecord {
    ClosePhase phase = ClosePhase::Done;
    uint32_t serial = 0;
    uint64_t acceptedTick = 0;
    uint32_t openTick = 0;
    uint64_t openGeneration = 0;
    uint64_t repairAt = 0;
    CloseKind kind = CloseKind::None;
};

CloseRecord g_close;
uint32_t g_closeSerial = 0;

void TakeCloseOwnership(CloseKind kind, uint64_t tickMs, bool contactStillDown);
void EndClose(const wchar_t* reason);
void ReleaseExecutorOwnership();
void RetireShowWatch(const wchar_t* reason);

uint32_t g_pullSerial = 0;

uint64_t g_pendingTapTick = 0;
CloseKind g_pendingTapKind = CloseKind::None;
uint32_t g_pendingTapPull = 0;
uint32_t g_pendingTapSerial = 0;

struct CloseScheduleCancel {
    bool slot = false;
    bool command = false;
    LONG64 commandSeq = 0;
};

bool ClearLocalCloseTap();
CloseScheduleCancel CancelCloseSchedules(const wchar_t* reason);

void ScheduleCloseTap(CloseKind kind, uint64_t delayMs) {
    g_pendingTapTick = GetTickCount64() + delayMs;
    g_pendingTapKind = kind;
    g_pendingTapPull = g_pullSerial;
    g_pendingTapSerial = g_close.serial;
}

std::atomic<HWND> g_coreHwnd{nullptr};

std::atomic<HWND> g_uiRendezvousHwnd{nullptr};

std::atomic<DwmGetWindowAttribute_t> g_dwmGetWindowAttribute{nullptr};

std::atomic<HANDLE> g_cloakEvent{nullptr};
HWINEVENTHOOK g_cloakHook = nullptr;

void CALLBACK CloakWinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd,
                                LONG idObject, LONG idChild, DWORD, DWORD) {
    if (event != EVENT_OBJECT_CLOAKED && event != EVENT_OBJECT_UNCLOAKED) {
        return;
    }
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }
    HWND core = g_coreHwnd.load(std::memory_order_relaxed);
    if (!core || hwnd != core) {
        return;
    }
    HANDLE signal = g_cloakEvent.load(std::memory_order_relaxed);
    if (signal) {
        SetEvent(signal);
    }
}

int32_t g_dismissAxis = static_cast<int32_t>(DismissAxis::Left);
float g_dismissDistance = 0.0f;

std::atomic<bool> g_rendererReady{false};

std::atomic<bool> g_rendererInvalid{false};

bool RendererReady() {
    return g_rendererReady.load(std::memory_order_acquire) &&
           !g_rendererInvalid.load(std::memory_order_acquire);
}

HANDLE g_rendererEvent = nullptr;

void SignalRendererChanged() {
    if (g_rendererEvent) {
        SetEvent(g_rendererEvent);
    }
}

void SetRendererReady(bool ready) {
    g_rendererReady.store(ready, std::memory_order_release);
    SignalRendererChanged();
}

void SetRendererInvalid(bool invalid) {
    g_rendererInvalid.store(invalid, std::memory_order_release);
    SignalRendererChanged();
}

uint64_t g_skipLogTick = 0;

uint64_t g_unhealedLogTick = 0;
std::atomic<int64_t> g_winRectDip[4]{};

struct LayoutIdentity {
    int32_t axis = static_cast<int32_t>(DismissAxis::Left);
    float distance = 0.0f;
    float originX = 0.0f;
    float originY = 0.0f;
    float workLeft = 0.0f;
    float workTop = 0.0f;
    float workRight = 0.0f;
    float workBottom = 0.0f;
    float monitorLeft = 0.0f;
    float monitorTop = 0.0f;
    float monitorRight = 0.0f;
    float monitorBottom = 0.0f;
};

struct GestureGeometry {
    uint32_t generation = 0;
    uint32_t layoutVersion = 0;
    LayoutIdentity layout;
    bool panel = false;
    float left = 0.0f;
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;

    bool band = false;
    float bandLow = 0.0f;
    float bandHigh = 0.0f;

    int32_t axis() const { return layout.axis; }
    float distance() const { return layout.distance; }
};

constexpr float kLayoutDistanceEpsilonDip = 0.5f;

bool SameLayoutDistance(float a, float b) {
    return std::fabs(a - b) <= kLayoutDistanceEpsilonDip;
}

bool SameLayoutIdentity(const LayoutIdentity& a, const LayoutIdentity& b) {
    return a.axis == b.axis && SameLayoutDistance(a.distance, b.distance) &&
           a.originX == b.originX && a.originY == b.originY &&
           a.workLeft == b.workLeft && a.workTop == b.workTop &&
           a.workRight == b.workRight && a.workBottom == b.workBottom &&
           a.monitorLeft == b.monitorLeft && a.monitorTop == b.monitorTop &&
           a.monitorRight == b.monitorRight &&
           a.monitorBottom == b.monitorBottom;
}

SRWLOCK g_geometryLock = SRWLOCK_INIT;

std::atomic<uint32_t> g_layoutVersion{1};
GestureGeometry g_geometry;
GestureGeometry g_inputGeometry;

std::atomic<uint32_t> g_openGeneration{1};

std::atomic<bool> g_intentOpenActive{false};
std::atomic<float> g_intentOpenEntry{0.0f};
std::atomic<LONG64> g_pendingDismissExec{0};

std::atomic<uint64_t> g_pendingDismissExecAt{0};
std::atomic<LONG64> g_pendingShowExec{0};
std::atomic<uint64_t> g_pendingShowUntil{0};

// A show the executor calls successful is not yet an open menu: the shell
// animates it in. If no open sample arrives inside this bound, the menu never
// appeared and the controller must stop owning it.
constexpr uint64_t kShowUnfulfilledMs = 600;

uint64_t g_showWatchUntil = 0;
LONG64 g_showWatchSeq = 0;
LONG64 g_controllerShowSeq = 0;

LONG64 g_lastIntentSeq = 0;

struct CloseCancelTicket {
    bool pending = false;
    uint32_t openTick = 0;
    LONG64 dismissSeq = 0;

    uint64_t expireAt = 0;
};
CloseCancelTicket g_intentCancel;

LONG64 g_pendingDismissConfirm = 0;

uint64_t g_pendingDismissExecGeneration = 0;
uint64_t g_pendingDismissConfirmGeneration = 0;

uint64_t g_pendingDismissConfirmUntil = 0;
bool g_pendingDismissConfirmRetained = false;

void ForgetConfirmedDismiss() {
    g_pendingDismissConfirm = 0;
    g_pendingDismissConfirmGeneration = 0;
    g_pendingDismissConfirmUntil = 0;
    g_pendingDismissConfirmRetained = false;
}

void ForgetPendingDismiss() {
    g_pendingDismissExec.store(0, std::memory_order_relaxed);
    g_pendingDismissExecAt.store(0, std::memory_order_relaxed);
    g_pendingDismissExecGeneration = 0;
    ForgetConfirmedDismiss();
}

float TrailEdge(int32_t axis, float left, float top, float right, float bottom);
float GeometryTrailDip(const GestureGeometry& geometry);
float FallbackTrailDip(int32_t axis, float distance);
bool PanelContainsPress(const GestureGeometry& geometry, float x, float y);
GestureGeometry ReadGestureGeometry(bool waitForWriter = false);
bool GeometryLayoutCurrent(const GestureGeometry& geometry);

void StoreGestureGeometry(const GestureGeometry& candidate);
void PublishWindowRectDip();
void PublishGestureGeometry(const GestureGeometry& candidate);

std::atomic<bool> g_panelOwned{false};

std::atomic<uint32_t> g_layoutApplying{0};

class LayoutApplyGuard {
  public:
    LayoutApplyGuard() = default;
    LayoutApplyGuard(const LayoutApplyGuard&) = delete;
    LayoutApplyGuard& operator=(const LayoutApplyGuard&) = delete;

    ~LayoutApplyGuard() {
        if (held_) {
            g_layoutApplying.fetch_sub(1, std::memory_order_seq_cst);
        }
    }

    bool Begin() {
        g_layoutApplying.fetch_add(1, std::memory_order_seq_cst);
        if (g_panelOwned.load(std::memory_order_seq_cst)) {
            g_layoutApplying.fetch_sub(1, std::memory_order_seq_cst);
            return false;
        }
        held_ = true;
        return true;
    }

  private:
    bool held_ = false;
};

struct SettledMeasureRequest {

    uint64_t id = 0;
    uint32_t generation = 0;
    bool trailIdle = false;
    uint64_t dueTick = 0;
};

std::mutex g_measureMutex;
SettledMeasureRequest g_measureRequest;
uint64_t g_measureSerial = 0;

uint32_t AdvanceOpenGeneration();

bool MeasureRequestLiveLocked(uint32_t generation, uint64_t id);
SettledMeasureRequest StartMeasureRequest(bool trailIdle);
bool TakeMeasureDeadline(uint64_t tickMs, SettledMeasureRequest& request);

bool MeasureRequestLive(uint32_t generation, uint64_t id);
bool EndMeasureRequest(uint32_t generation, uint64_t id);
void CancelMeasureRequest();
SettledMeasureRequest SnapshotMeasureRequest();

constexpr uint64_t kScrollerRetryMs = 100;

constexpr uint64_t kScrollerRetryBackoffMaxMs = 400;
constexpr uint32_t kScrollerRetryLimit = 3;
std::atomic<uint64_t> g_scrollerRetryAt{0};
std::atomic<uint32_t> g_scrollerRetryGeneration{0};

bool g_panelTrailLogged = false;
bool g_panelTrailFallbackLogged = false;
bool g_panelTrailRejectedLogged = false;
uint32_t g_panelTrailRejectedGeneration = 0;

HANDLE g_hidThread = nullptr;
std::atomic<HWND> g_hidHwnd{nullptr};

HANDLE g_hidStopEvent = nullptr;

std::atomic<bool> g_rootHookReady{false};
std::atomic<bool> g_rootHookRefused{false};

std::atomic<uint32_t> g_uiRegistrationCount{0};

std::atomic<bool> g_measureDeferred{false};

void RevokeHandlers();
void ResetComposition();
void ResetRenderer();
void TeardownUi();
void RefreshTaskbarAppBarCache();
HWND FindStartCoreWindow();

bool PublishPanelTrailDip(bool measurePanel, uint32_t expectedGeneration,
                          uint64_t requestId);

void PostGeometryMeasurement(const SettledMeasureRequest& request, bool settled);

void ArmLayoutWatch(const SettledMeasureRequest& request);
void StopSettledTrailWatch();
void StopSettledTrailWatchIfGeneration(uint32_t maxGeneration);

void CacheScrollableElement(uint32_t expectedGeneration, uint64_t requestId);
void ReleaseScrollableElement();
void ReleaseScrollableElementIfGeneration(uint32_t maxGeneration);
void ClearScrollerRetry();

bool RefreshGestureGeometry();

void PublishPanelOwnership();

extern bool g_commitNeedsProgressReset;

void TakeCloseOwnership(CloseKind kind, uint64_t tickMs, bool contactStillDown);

void LandClosedMenu(uint64_t tickMs, const wchar_t* reason);

SwipeIpcChannel g_ipc;

// The touch word is the bridge's read-only signal that a touch owns the open.
// Only the claim and its activation set it, and a report stream that stops
// mid-reveal never clears it. The hold therefore expires: after this window the
// idle release runs and clears the word through the normal release path.
constexpr uint64_t kOpenClaimHoldMs = 1200;

uint64_t g_openClaimTick = 0;

void NoteOpenClaimTick() {
    g_openClaimTick = GetTickCount64();
}

void SetControllerTouch(bool touch) {
    if (touch) {
        NoteOpenClaimTick();
    }
    SwipeIpcSetControllerTouch(&g_ipc, touch);
}

uint64_t OpenClaimHoldUntil() {
    if (g_openClaimTick == 0) {
        return 0;
    }
    if (!SwipeIpcControllerTouch(&g_ipc) &&
        !g_intentOpenActive.load(std::memory_order_relaxed)) {
        return 0;
    }
    return g_openClaimTick + kOpenClaimHoldMs;
}

bool OpenClaimHoldActive() {
    const uint64_t until = OpenClaimHoldUntil();
    return until != 0 && GetTickCount64() < until;
}

void RecordPeerUiThread() {
    const DWORD tid = GetCurrentThreadId();
    g_uiThreadId.store(tid, std::memory_order_relaxed);
    if (!g_ipc.state || SwipeIpcPeerUiThread(&g_ipc) == tid) {
        return;
    }
    SwipeIpcSetPeerUiThread(&g_ipc, tid);
    TraceLog(L"ipc peer ui tid=%u", tid);
}

void ClearPeerUiThread() {
    g_uiThreadId.store(0, std::memory_order_relaxed);
    if (g_ipc.state) {
        SwipeIpcSetPeerUiThread(&g_ipc, 0);
    }
}

const wchar_t* CloseKindName(CloseKind kind);

bool StartWindowIsOpen();

float RevealProgressDip(const GestureGeometry& geometry, float distance, float x,
                        float y, float& panelTrailOut);

wucore::CoreDispatcher CopyDispatcher() {
    std::lock_guard lock(g_compositionMutex);
    return g_dispatcher;
}

template <typename Fn>
void PostCountedUiTurn(wucore::CoreDispatcher const& dispatcher, Fn&& turn) {
    std::shared_ptr<ModuleEntryLease> lease;
    try {
        lease = std::make_shared<ModuleEntryLease>();
    } catch (...) {
        return;
    }

    try {

        dispatcher.TryRunAsync(
            wucore::CoreDispatcherPriority::Normal,
            [lease = std::move(lease), turn = std::forward<Fn>(turn)]() {

                static_cast<void>(lease);
                UiDepthGuard depth;
                turn();
            });
    } catch (...) {

    }
}

void InvalidateDismissBatchLocked() {
    g_dismissSerial.fetch_add(1, std::memory_order_relaxed);
    g_dismissSettling.store(false, std::memory_order_relaxed);
}

bool RevokeDismissBatchLocked() {
    if (!g_dismissBatch || !g_dismissBatchToken.value) {
        g_dismissBatch = nullptr;
        g_dismissBatchToken = {};
        g_settleLease.reset();
        return true;
    }

    try {
        g_dismissBatch.Completed(g_dismissBatchToken);
    } catch (...) {
        return false;
    }

    g_dismissBatch = nullptr;
    g_dismissBatchToken = {};
    g_settleLease.reset();
    return true;
}

bool ShowAtRest() {
    std::lock_guard lock(g_compositionMutex);
    if (!g_state) {
        return false;
    }

    try {
        g_state.StopAnimation(L"Progress");

        g_panel.frames = 0;
        RevokeDismissBatchLocked();
        InvalidateDismissBatchLocked();
        g_panel.coasting.store(false, std::memory_order_relaxed);
        g_state.InsertScalar(L"Progress", kProgressRest);
        g_panel.progress.store(kProgressRest, std::memory_order_relaxed);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Composition reset failed: %08X %s", e.code(),
               e.message().c_str());
        return false;
    } catch (...) {
        Wh_Log(L"Composition reset failed with an unknown exception");
        return false;
    }

    return true;
}

bool InsertProgress(float progress) {
    std::lock_guard lock(g_compositionMutex);
    if (!g_state) {
        return false;
    }

    try {

        const bool coasting =
            g_panel.coasting.exchange(false, std::memory_order_relaxed);
        if (coasting || g_panel.frames > 0) {
            if (g_panel.frames > 0) {
                RevokeDismissBatchLocked();
                InvalidateDismissBatchLocked();
                g_panel.frames = 0;
            }
            g_state.StopAnimation(L"Progress");
        }
        g_state.InsertScalar(L"Progress", progress);
        g_panel.progress.store(progress, std::memory_order_relaxed);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Progress update failed: %08X %s", e.code(),
               e.message().c_str());
        return false;
    } catch (...) {
        Wh_Log(L"Progress update failed with an unknown exception");
        return false;
    }

    return true;
}

bool AnimateProgressTo(float to, std::chrono::milliseconds duration,
                       const float* settleSamples, int settleSampleCount,
                       uint32_t* serialOut) {
    if (serialOut) {
        *serialOut = 0;
    }
    std::lock_guard lock(g_compositionMutex);
    if (!g_state || !g_compositor || !settleSamples || settleSampleCount < 2) {
        return false;
    }

    try {
        wuc::ScalarKeyFrameAnimation animation =
            g_compositor.CreateScalarKeyFrameAnimation();

        const float last = static_cast<float>(settleSampleCount - 1);
        for (int i = 0; i < settleSampleCount; ++i) {
            animation.InsertKeyFrame(
                static_cast<float>(i) / last,
                std::clamp(settleSamples[i], 0.0f, 1.0f));
        }
        animation.Duration(duration);

        const uint64_t startQpc = QpcMsNow();
        g_state.StopAnimation(L"Progress");

        g_panel.frames = 0;
        RevokeDismissBatchLocked();
        g_panel.coasting.store(false, std::memory_order_relaxed);
        g_state.InsertScalar(L"Progress", std::clamp(to, 0.0f, 1.0f));
        g_state.StartAnimation(L"Progress", animation);
        g_panel.progress.store(std::clamp(to, 0.0f, 1.0f),
                            std::memory_order_relaxed);

        InvalidateDismissBatchLocked();
        if (serialOut) {
            *serialOut = g_dismissSerial.load(std::memory_order_relaxed);
        }

        const int frames = std::min(settleSampleCount, kSettleMaxFrames);
        std::memcpy(g_panel.samples, settleSamples, sizeof(float) * frames);
        g_panel.frames = frames;
        g_panel.startQpc = startQpc;
        g_panel.durationMs = static_cast<uint64_t>(duration.count());
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Progress animation failed: %08X %s", e.code(),
               e.message().c_str());
        return false;
    } catch (...) {
        Wh_Log(L"Progress animation failed with an unknown exception");
        return false;
    }

    return true;
}

float CoastValueAt(uint64_t qpcNow, float* slopeOut = nullptr) {
    const uint64_t duration = static_cast<uint64_t>(kCoastDuration.count());
    if (slopeOut) {
        *slopeOut = (g_panel.coastTo - g_panel.coastFrom) /
                    static_cast<float>(duration);
    }
    if (qpcNow <= g_panel.coastStartQpc) {
        return g_panel.coastFrom;
    }

    const uint64_t elapsed = qpcNow - g_panel.coastStartQpc;
    if (elapsed >= duration) {
        if (slopeOut) {
            *slopeOut = 0.0f;
        }
        return g_panel.coastTo;
    }

    const float t = static_cast<float>(elapsed) / static_cast<float>(duration);
    return std::clamp(g_panel.coastFrom + (g_panel.coastTo - g_panel.coastFrom) * t, 0.0f, 1.0f);
}

float SettleValueAt(uint64_t qpcNow, float* slopeOut = nullptr) {
    if (slopeOut) {
        *slopeOut = 0.0f;
    }
    if (g_panel.frames < 2 || g_panel.durationMs == 0) {
        return std::clamp(g_panel.progress.load(std::memory_order_relaxed), 0.0f,
                          1.0f);
    }

    const uint64_t elapsed =
        qpcNow > g_panel.startQpc ? qpcNow - g_panel.startQpc : 0;
    if (elapsed >= g_panel.durationMs) {
        return std::clamp(g_panel.samples[g_panel.frames - 1], 0.0f, 1.0f);
    }

    const double position = static_cast<double>(elapsed) /
                            static_cast<double>(g_panel.durationMs) *
                            (g_panel.frames - 1);
    const int index = std::min(static_cast<int>(position), g_panel.frames - 2);
    const float fraction = static_cast<float>(position - index);
    const float value =
        g_panel.samples[index] +
        (g_panel.samples[index + 1] - g_panel.samples[index]) * fraction;
    if (slopeOut) {
        const double spacing =
            static_cast<double>(g_panel.durationMs) / (g_panel.frames - 1);
        *slopeOut = static_cast<float>(
            (g_panel.samples[index + 1] - g_panel.samples[index]) / spacing);
    }
    return std::clamp(value, 0.0f, 1.0f);
}

float TakeProgressNow(const wchar_t* caller) {
    const uint64_t qpcNow = QpcMsNow();

    std::lock_guard lock(g_compositionMutex);
    if (!g_state) {

        TraceLog(L"motion sample caller=%s phase=%s p=%.3f v=%.3f qpc=%llu",
                caller, L"none", kProgressRest, 0.0f,
                static_cast<unsigned long long>(qpcNow));
        return kProgressRest;
    }

    const bool coasting = g_panel.coasting.load(std::memory_order_relaxed);
    const bool settling = g_panel.frames >= 2 && g_panel.durationMs != 0;
    if (!coasting && !settling) {

        const float resting =
            std::clamp(g_panel.progress.load(std::memory_order_relaxed), 0.0f,
                       1.0f);
        TraceLog(L"motion sample caller=%s phase=%s p=%.3f v=%.3f qpc=%llu",
                caller, L"rest", resting, 0.0f,
                static_cast<unsigned long long>(qpcNow));
        return resting;
    }

    float slope = 0.0f;
    const float value =
        std::clamp(coasting ? CoastValueAt(qpcNow, &slope)
                            : SettleValueAt(qpcNow, &slope),
                   0.0f, 1.0f);

    TraceLog(L"motion sample caller=%s phase=%s p=%.3f v=%.3f qpc=%llu", caller,
            coasting ? L"coast" : L"settle", value, slope,
            static_cast<unsigned long long>(qpcNow));

    if (settling && value < 0.05f) {
        const uint64_t elapsed =
            qpcNow > g_panel.startQpc ? qpcNow - g_panel.startQpc : 0;
        TraceLog(L"progress take settle at=%.3f elapsed=%llu dur=%llu caller=%s",
                value, static_cast<unsigned long long>(elapsed),
                static_cast<unsigned long long>(g_panel.durationMs), caller);
    }
    try {
        g_state.StopAnimation(L"Progress");
        g_state.InsertScalar(L"Progress", value);
        g_panel.progress.store(value, std::memory_order_relaxed);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Progress take failed: %08X %s",
               static_cast<unsigned>(e.code()), e.message().c_str());
        return value;
    } catch (...) {
        Wh_Log(L"Progress take failed with an unknown exception");
        return value;
    }

    g_panel.frames = 0;
    g_panel.durationMs = 0;
    g_panel.coasting.store(false, std::memory_order_relaxed);
    RevokeDismissBatchLocked();
    InvalidateDismissBatchLocked();
    return value;
}

float TakeVisibleProgressNow(const wchar_t* caller = L"visible") {
    const float live = TakeProgressNow(caller);
    return live >= 1.0f ? 0.0f : live;
}

float PeekProgressNow() {
    const uint64_t qpcNow = QpcMsNow();
    std::lock_guard lock(g_compositionMutex);
    if (!g_state) {
        return kProgressRest;
    }
    const bool coasting = g_panel.coasting.load(std::memory_order_relaxed);
    const bool settling = g_panel.frames >= 2 && g_panel.durationMs != 0;
    if (!coasting && !settling) {
        return std::clamp(g_panel.progress.load(std::memory_order_relaxed), 0.0f,
                          1.0f);
    }
    return std::clamp(coasting ? CoastValueAt(qpcNow) : SettleValueAt(qpcNow),
                      0.0f, 1.0f);
}

bool ProgressMotionActive() {
    std::lock_guard lock(g_compositionMutex);
    if (g_panel.coasting.load(std::memory_order_relaxed)) {
        return true;
    }
    if (g_panel.frames < 2 || g_panel.durationMs == 0) {
        return false;
    }
    const uint64_t qpcNow = QpcMsNow();
    const uint64_t elapsed =
        qpcNow > g_panel.startQpc ? qpcNow - g_panel.startQpc : 0;
    if (elapsed >= g_panel.durationMs) {
        return false;
    }
    return true;
}

bool StartCoast(float from, float to) {
    const uint64_t qpcNow = QpcMsNow();
    from = std::clamp(from, 0.0f, 1.0f);
    to = std::clamp(to, 0.0f, 1.0f);

    std::lock_guard lock(g_compositionMutex);
    if (!g_state || !g_compositor) {
        return false;
    }

    try {
        wuc::ScalarKeyFrameAnimation animation =
            g_compositor.CreateScalarKeyFrameAnimation();
        animation.InsertKeyFrame(0.0f, from);

        animation.InsertKeyFrame(1.0f, to);
        animation.Duration(kCoastDuration);

        g_state.StopAnimation(L"Progress");
        g_state.StartAnimation(L"Progress", animation);

        g_panel.frames = 0;
        RevokeDismissBatchLocked();
        InvalidateDismissBatchLocked();
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Progress coast failed: %08X %s", e.code(),
               e.message().c_str());
        return false;
    } catch (...) {
        Wh_Log(L"Progress coast failed with an unknown exception");
        return false;
    }

    g_panel.coastFrom = from;
    g_panel.coastTo = to;
    g_panel.coastStartQpc = qpcNow;
    g_panel.coasting.store(true, std::memory_order_relaxed);
    return true;
}

bool StopCoast(float& value) {
    const uint64_t qpcNow = QpcMsNow();
    value = CoastValueAt(qpcNow);

    std::lock_guard lock(g_compositionMutex);
    g_panel.coasting.store(false, std::memory_order_relaxed);
    if (!g_state) {
        return false;
    }

    try {

        g_state.StopAnimation(L"Progress");
        g_panel.frames = 0;
        RevokeDismissBatchLocked();
        InvalidateDismissBatchLocked();
        g_state.InsertScalar(L"Progress", value);
        g_panel.progress.store(value, std::memory_order_relaxed);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Progress coast stop failed: %08X %s", e.code(),
               e.message().c_str());
        return false;
    } catch (...) {
        Wh_Log(L"Progress coast stop failed with an unknown exception");
        return false;
    }

    return true;
}

void PanelController::Begin(const float* position) {
    if (position) {
        InsertProgress(*position);
    }
    TraceLog(L"panel intent=begin progress=%.3f source=touch",
            position ? std::clamp(*position, 0.0f, 1.0f)
                     : progress.load(std::memory_order_relaxed));
}

bool PanelController::Move(float position) {
    const float value = std::clamp(position, 0.0f, 1.0f);
    return InsertProgress(value);
}

void PanelController::End(float to, float endVelocity) {
    TraceLog(L"panel intent=end progress=%.3f velocity=%.2f target=%.0f",
            progress.load(std::memory_order_relaxed), endVelocity,
            std::clamp(to, 0.0f, 1.0f));
}

void PanelController::Commit(float to) {
    TraceLog(L"panel intent=commit progress=%.3f target=%.0f",
            progress.load(std::memory_order_relaxed),
            std::clamp(to, 0.0f, 1.0f));
}

int SettleStepsFor(int64_t durationMs) {
    const double steps =
        std::ceil(static_cast<double>(durationMs) / kSettleSegmentMs);
    return std::clamp(static_cast<int>(steps), kSettleMinSteps,
                      kSettleMaxSteps);
}

float TowardTarget(float progressPerMs, float from, float to) {
    if (to == from) {
        return 0.0f;
    }
    return std::max(0.0f, to > from ? progressPerMs : -progressPerMs);
}

float SpeedTowardDip(float progressPerMs, float from, float to, float distance) {
    return TowardTarget(progressPerMs, from, to) * std::max(distance, 0.0f);
}

int BuildSpringSamples(float start, float target, float velocity,
                       int64_t durationMs, double zeta, float* samples) {
    const float startScale =
        static_cast<float>(g_gestureStartSpeedPercent.load(
            std::memory_order_relaxed)) /
        100.0f;

    const float travel = target - start;
    const float toward = TowardTarget(velocity, start, target);

    const double tTotal = static_cast<double>(durationMs);

    const double y0 =
        std::fabs(travel) > 0.0f
            ? static_cast<double>(startScale * toward) /
                  static_cast<double>(std::fabs(travel))
            : 0.0;

    const bool overdamped = zeta > 1.0;
    const bool critical = std::fabs(zeta - 1.0) <= 1e-9;
    const double slowestFactor =
        overdamped ? zeta - std::sqrt(zeta * zeta - 1.0) : zeta;
    const double omega = kSettleTimeConstants / (slowestFactor * tTotal);
    const double decay = zeta * omega;
    const double wd =
        overdamped
            ? omega * std::sqrt(zeta * zeta - 1.0)
            : omega * std::sqrt(std::max(0.0, 1.0 - zeta * zeta));

    const auto curveAt = [&](double t) -> double {
        const double envelope = std::exp(-decay * t);
        if (overdamped) {
            return 1.0 - envelope *
                             (std::cosh(wd * t) +
                              ((decay - y0) / wd) * std::sinh(wd * t));
        }
        if (critical) {
            return 1.0 - envelope * (1.0 + (omega - y0) * t);
        }
        return 1.0 - envelope *
                         (std::cos(wd * t) +
                          ((decay - y0) / wd) * std::sin(wd * t));
    };

    const double endValue = std::clamp(curveAt(tTotal), 0.5, 1.0);

    const int steps = SettleStepsFor(durationMs);
    samples[0] = start;
    float previous = start;
    for (int i = 1; i < steps; ++i) {
        const double t = tTotal * i / static_cast<double>(steps);
        const double y = std::clamp(curveAt(t) / endValue, 0.0, 1.0);
        float value = std::clamp(
            start + (target - start) * static_cast<float>(y), 0.0f, 1.0f);

        value = target >= start ? std::max(value, previous)
                                : std::min(value, previous);
        samples[i] = value;
        previous = value;
    }
    samples[steps] = target;
    return steps;
}

void LogSettle(SettleStyle style, const wchar_t* direction, float dipVelocity,
               int64_t durationMs, float remaining, int steps, double zeta,
               const float* samples, float distance) {
    TraceLog(L"settle style=%s dir=%s vel=%.2f dur=%llu remaining=%.0f "
            L"frames=%d zeta=%.2f step0=%.1f stepmid=%.1f stepend=%.1f",
            SettleStyleName(style), direction, dipVelocity,
            static_cast<unsigned long long>(durationMs), remaining, steps + 1,
            zeta, (samples[1] - samples[0]) * distance,
            (samples[steps / 2 + 1] - samples[steps / 2]) * distance,
            (samples[steps] - samples[steps - 1]) * distance);
}

bool BeginGestureSettle(float from, float to, float velocity,
                        std::chrono::milliseconds& duration,
                        uint32_t* serialOut = nullptr) {
    const float start = std::clamp(from, 0.0f, 1.0f);
    const float target = std::clamp(to, 0.0f, 1.0f);
    g_panel.End(target, velocity);

    const SettleStyle style = g_settleStyle.load(std::memory_order_relaxed);
    const double zeta =
        style == SettleStyle::Win11 ? kWin11SpringDamping : kWin10SpringDamping;

    const float distance =
        std::max(ReadGestureGeometry().distance(), 0.0f);
    const float remaining = std::fabs(target - start) * distance;
    const int64_t durationMs = std::clamp(
        static_cast<int64_t>(std::llround(
            static_cast<double>(remaining) /
            static_cast<double>(kSettleSpeedDipPerMs))),
        static_cast<int64_t>(kSettleMinDuration.count()),
        static_cast<int64_t>(kSettleMaxDuration.count()));
    duration = std::chrono::milliseconds(durationMs);

    float samples[kSettleMaxFrames] = {};
    const int steps = BuildSpringSamples(start, target, velocity, durationMs,
                                         zeta, samples);
    if (!AnimateProgressTo(target, duration, samples, steps + 1, serialOut)) {
        return false;
    }

    LogSettle(style, target >= start ? L"open" : L"close",
              SpeedTowardDip(velocity, start, target, distance), durationMs,
              remaining, steps, zeta, samples, distance);
    return true;
}

bool BeginShellCloseSettle(float from, std::chrono::milliseconds& duration,
                           uint32_t* serialOut = nullptr) {
    const float start = std::clamp(from, 0.0f, 1.0f);
    g_panel.Commit(0.0f);

    const SettleStyle style = g_settleStyle.load(std::memory_order_relaxed);
    const double zeta =
        style == SettleStyle::Win11 ? kWin11SpringDamping : kWin10SpringDamping;

    const float distance =
        std::max(ReadGestureGeometry().distance(), 0.0f);
    const float remaining = distance;
    const int64_t durationMs = std::clamp(
        static_cast<int64_t>(std::llround(
            static_cast<double>(remaining) /
            static_cast<double>(kSettleSpeedDipPerMs))),
        static_cast<int64_t>(kSettleMinDuration.count()),
        static_cast<int64_t>(kSettleMaxDuration.count()));
    duration = std::chrono::milliseconds(durationMs);

    float openSamples[kSettleMaxFrames] = {};
    const int steps = BuildSpringSamples(0.0f, 1.0f, 0.0f, durationMs, zeta,
                                         openSamples);
    float samples[kSettleMaxFrames] = {};
    for (int i = 0; i <= steps; ++i) {
        samples[i] = std::clamp(start * (1.0f - openSamples[i]), 0.0f, 1.0f);
    }
    samples[steps] = 0.0f;

    if (!AnimateProgressTo(0.0f, duration, samples, steps + 1, serialOut)) {
        return false;
    }

    LogSettle(style, L"close", 0.0f, durationMs, remaining, steps, zeta,
              samples, distance);
    return true;
}

const wchar_t* CloseKindName(CloseKind kind) {
    return kind == CloseKind::Dismiss ? L"dismiss" : L"reveal";
}

void ArmGestureClose(CloseKind kind, std::chrono::milliseconds duration) {
    ScheduleCloseTap(kind, static_cast<uint64_t>(duration.count()));
}

bool ReleaseCommits(bool allowCommit, float travelDip, uint64_t elapsedMs,
                    float distance) {
    if (!allowCommit || !(distance > 0.0f)) {
        return false;
    }
    const float averageVelocity =
        travelDip / static_cast<float>(elapsedMs > 0 ? elapsedMs : 1);
    const float threshold =
        static_cast<float>(g_commitDistancePercent.load(
            std::memory_order_relaxed)) / 100.0f;
    return travelDip > threshold * distance ||
           averageVelocity > kCommitVelocity;
}

struct GestureClose {
    bool started = false;
};

GestureClose BeginGestureClose(float from, float velocity, CloseKind kind,
                               uint32_t* serialOut = nullptr) {
    GestureClose close;
    TakeCloseOwnership(kind, GetTickCount64(), false);
    std::chrono::milliseconds duration{0};
    if (!BeginGestureSettle(from, 0.0f, velocity, duration, serialOut)) {

        EndClose(L"no-settle");
        ReleaseExecutorOwnership();
        return close;
    }
    close.started = true;
    ArmGestureClose(kind, duration);
    return close;
}

bool AbandonClose(CloseKind kind) {
    TraceLog(L"close abandoned kind=%s", CloseKindName(kind));
    return ShowAtRest();
}

bool BeginGestureOpen(float from, float velocity) {
    std::chrono::milliseconds duration{0};
    return BeginGestureSettle(from, 1.0f, velocity, duration);
}

bool RevokeDismissBatch() {
    std::lock_guard lock(g_compositionMutex);
    return RevokeDismissBatchLocked();
}

enum class DismissClose {
    NotStarted,
    Started,
    StartedUnobserved,
};

struct DismissCloseResult {
    DismissClose start = DismissClose::NotStarted;
};

DismissCloseResult BeginDismissSettle(float from, float velocity) {
    DismissCloseResult result;
    wuc::CompositionScopedBatch batch{nullptr};
    winrt::event_token batchToken{};
    uint32_t serial = 0;
    bool handlerRegistered = false;

    RevokeDismissBatch();

    try {
        {
            std::lock_guard lock(g_compositionMutex);
            if (!g_state || !g_compositor) {
                return result;
            }

            batch = g_compositor.CreateScopedBatch(
                wuc::CompositionBatchTypes::Animation);
        }

        const GestureClose close =
            BeginGestureClose(from, velocity, CloseKind::Dismiss, &serial);
        if (!close.started) {

            {
                std::lock_guard lock(g_compositionMutex);
                InvalidateDismissBatchLocked();
            }
            batch.End();
            return result;
        }

        result.start = DismissClose::StartedUnobserved;

        std::shared_ptr<ModuleEntryLease> lease;
        try {
            lease = std::make_shared<ModuleEntryLease>();
        } catch (...) {

            TraceLog(L"close observer lost kind=dismiss");
            batch.End();
            return result;
        }

        bool serialLive = false;
        {
            std::lock_guard lock(g_compositionMutex);
            serialLive =
                serial != 0 &&
                serial == g_dismissSerial.load(std::memory_order_relaxed);
            if (serialLive) {
                g_dismissSettling.store(true, std::memory_order_relaxed);
            }
        }
        if (!serialLive) {
            TraceLog(L"close observer lost kind=dismiss");
            batch.End();
            return result;
        }

        batchToken = batch.Completed(
            [serial, lease](
                winrt::Windows::Foundation::IInspectable const&,
                wuc::CompositionBatchCompletedEventArgs const&) {

                static_cast<void>(lease);

                if (g_stopDismissCallback.load(std::memory_order_relaxed)) {
                    return;
                }

                try {
                    std::lock_guard lock(g_compositionMutex);

                    if (serial != g_dismissSerial.load(
                                      std::memory_order_relaxed)) {
                        return;
                    }

                    if (g_state) {
                        g_state.StopAnimation(L"Progress");
                    }
                    g_panel.frames = 0;
                    g_dismissSettling.store(false, std::memory_order_relaxed);
                } catch (...) {
                    g_dismissSettling.store(false, std::memory_order_relaxed);
                }
            });
        handlerRegistered = batchToken.value != 0;

        batch.End();

        bool keepHandler = false;
        {
            std::lock_guard lock(g_compositionMutex);
            if (handlerRegistered &&
                serial == g_dismissSerial.load(std::memory_order_relaxed)) {
                g_dismissBatch = batch;
                g_dismissBatchToken = batchToken;
                g_settleLease = lease;
                keepHandler = true;
            }
        }
        if (keepHandler) {
            result.start = DismissClose::Started;
            return result;
        }

        if (handlerRegistered) {
            try {
                batch.Completed(batchToken);
            } catch (...) {
            }
        }
        TraceLog(L"close observer lost kind=dismiss");
        return result;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Dismiss animation failed: %08X %s", e.code(),
               e.message().c_str());
        if (batch && batchToken.value) {
            try {
                batch.Completed(batchToken);
            } catch (...) {
            }
        }
        std::lock_guard lock(g_compositionMutex);
        InvalidateDismissBatchLocked();

        return result;
    } catch (...) {
        Wh_Log(L"Dismiss animation failed with an unknown exception");
        if (batch && batchToken.value) {
            try {
                batch.Completed(batchToken);
            } catch (...) {
            }
        }
        std::lock_guard lock(g_compositionMutex);
        InvalidateDismissBatchLocked();

        return result;
    }
}

uint32_t g_ipcOpenTickLow = 0;

uint32_t g_peerTickLast = 0;

uint32_t g_peerEpoch = 0;

std::atomic<bool> g_peerPublished{false};

bool g_touchRegistered = false;
bool g_penRegistered = false;

SRWLOCK g_peerLock = SRWLOCK_INIT;
bool g_peerTerminal = false;

void PublishPeerTick(uint64_t tickMs) {
    if (!g_ipc.state || !g_peerPublished.load(std::memory_order_relaxed)) {
        return;
    }
    const uint32_t now = static_cast<uint32_t>(tickMs);
    if (now - g_peerTickLast < kSwipePeerTickIntervalMs) {
        return;
    }
    g_peerTickLast = now;
    SwipeIpcPublishPeerTick(&g_ipc, tickMs);
}

void SetPeerGone(const wchar_t* why, bool terminal) {
    if (!g_ipc.state) {
        return;
    }
    bool logged = false;
    AcquireSRWLockExclusive(&g_peerLock);
    if (terminal) {
        g_peerTerminal = true;
    }
    SwipeIpcSetPeerState(&g_ipc, kSwipePeerGone);
    SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionUnowned);
    SetControllerTouch(false);
    logged = g_peerPublished.exchange(false, std::memory_order_relaxed);
    ReleaseSRWLockExclusive(&g_peerLock);
    if (logged) {
        TraceLog(L"ipc peer gone reason=%s terminal=%d", why, terminal ? 1 : 0);
    }
}

void PublishPeerGone(const wchar_t* why) {
    SetPeerGone(why, true);
}

// The capability word outlives an Explorer crash: the section stays mapped and
// the word keeps its 1 until a replacement bridge clears it. The pid the bridge
// publishes is the second source: a signalled handle turns the word into 0.
SRWLOCK g_execProcessLock = SRWLOCK_INIT;
HANDLE g_execProcess = nullptr;
uint32_t g_execProcessPid = 0;
uint32_t g_execProcessEpoch = 0;

void CloseExecProcessLocked() {
    if (g_execProcess) {
        CloseHandle(g_execProcess);
        g_execProcess = nullptr;
    }
    g_execProcessPid = 0;
    g_execProcessEpoch = 0;
}

// The probe keeps the handle across reads, so the Start unload is the one place
// that releases it: without this, every activation leaves one process handle
// in StartMenuExperienceHost.exe.
void ReleaseExecProcess() {
    AcquireSRWLockExclusive(&g_execProcessLock);
    CloseExecProcessLocked();
    ReleaseSRWLockExclusive(&g_execProcessLock);
}

enum class ExecProcess : int { Unknown = 0, Alive, Dead };

ExecProcess ExecProcessProbe(uint32_t pid) {
    if (!g_ipc.state || pid == 0) {
        return ExecProcess::Unknown;
    }
    const uint32_t epoch = g_peerEpoch;
    ExecProcess answer = ExecProcess::Unknown;
    AcquireSRWLockExclusive(&g_execProcessLock);
    if (g_execProcessEpoch != 0 &&
        (g_execProcessEpoch != epoch || g_execProcessPid != pid)) {
        CloseExecProcessLocked();
    }
    if (!g_execProcess) {
        HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
        if (!process) {
            const DWORD error = GetLastError();
            TraceLog(L"exec process open failed pid=%u epoch=%u err=%u", pid,
                    epoch, error);
            ReleaseSRWLockExclusive(&g_execProcessLock);
            return error == ERROR_INVALID_PARAMETER ? ExecProcess::Dead
                                                    : ExecProcess::Unknown;
        }
        g_execProcess = process;
        g_execProcessPid = pid;
        g_execProcessEpoch = epoch;
    }
    const DWORD wait = WaitForSingleObject(g_execProcess, 0);
    answer = wait == WAIT_OBJECT_0
                 ? ExecProcess::Dead
                 : (wait == WAIT_TIMEOUT ? ExecProcess::Alive
                                         : ExecProcess::Unknown);
    ReleaseSRWLockExclusive(&g_execProcessLock);
    return answer;
}

bool ExecutorProcessAlive(uint32_t pid) {
    return ExecProcessProbe(pid) == ExecProcess::Alive;
}

// The bridge clears the capability word, then publishes the new execPid, and
// only the worker sets the word again. One read of the two words can pair the
// old 1 with the new identity, so read the pid around the word, then probe that
// same pid: a changed pid is an election in flight, and a settled pair alone
// decides.
bool BridgeCapabilityNow() {
    if (!g_ipc.state) {
        return false;
    }
    for (int attempt = 0; attempt < 2; attempt++) {
        const uint32_t pid = SwipeIpcExecPid(&g_ipc);
        if (pid == 0) {
            return false;
        }
        const bool capable = SwipeIpcExecCapable(&g_ipc);
        if (SwipeIpcExecPid(&g_ipc) != pid) {
            continue;
        }
        return capable && ExecutorProcessAlive(pid);
    }
    return false;
}

bool PublishPeerReady(const wchar_t* why) {
    if (!g_ipc.state) {
        return false;
    }

    if (!g_rootHookReady.load(std::memory_order_acquire)) {
        TraceLog(L"ipc peer not published reason=no-root-hook refused=%d",
                g_rootHookRefused.load(std::memory_order_acquire) ? 1 : 0);
        return false;
    }

    if (!g_rendererReady.load(std::memory_order_acquire)) {
        TraceLog(L"ipc peer not published reason=no-renderer");
        return false;
    }
    if (!g_ipc.intentEvent) {

        TraceLog(L"ipc peer not published reason=no-intent-event");
        return false;
    }
    if (!BridgeCapabilityNow()) {

        TraceLog(L"ipc peer not published reason=no-capability");
        return false;
    }
    bool published = false;
    AcquireSRWLockExclusive(&g_peerLock);
    if (g_peerTerminal) {
        ReleaseSRWLockExclusive(&g_peerLock);
        TraceLog(L"ipc peer not published reason=terminal-withdrawal");
        return false;
    }
    const uint64_t now = GetTickCount64();
    g_peerTickLast = static_cast<uint32_t>(now);
    SwipeIpcPublishPeerTick(&g_ipc, now);
    SwipeIpcSetPeerState(&g_ipc, kSwipePeerAlive);
    g_peerPublished.store(true, std::memory_order_relaxed);
    published = true;
    ReleaseSRWLockExclusive(&g_peerLock);
    if (published) {
        TraceLog(L"ipc peer alive reason=%s epoch=%u touch=%d pen=%d", why,
                g_peerEpoch, g_touchRegistered ? 1 : 0,
                g_penRegistered ? 1 : 0);
    }
    return published;
}

void OpenIpcChannel() {
    bool created = false;
    DWORD error = 0;
    if (!SwipeIpcOpen(&g_ipc, &created, &error)) {
        TraceLog(L"ipc open failed err=%u", static_cast<unsigned>(error));
        return;
    }

    AcquireSRWLockExclusive(&g_peerLock);
    g_peerTerminal = false;
    ReleaseSRWLockExclusive(&g_peerLock);
    SetPeerGone(L"attach", false);

    g_lastIntentSeq = SwipeIpcLoad64(&g_ipc.state->intentSeq);
    g_ipcOpenTickLow = 0;

    SwipeIpcSetPeerPid(&g_ipc, GetCurrentProcessId());
    SwipeIpcSetPeerUiThread(&g_ipc, 0);
    g_peerEpoch = SwipeIpcBeginPeerActivation(&g_ipc);

    if (g_ipc.state) {
        SwipeIpcPublishExec(&g_ipc, kSwipeExecCancel, 0, 0, 0, 0, 0);
        TraceLog(L"exec cancel request target=0 reason=attach epoch=%u",
                g_peerEpoch);
    }

    TraceLog(L"ipc open created=%d intent=%lld epoch=%u", created ? 1 : 0,
            static_cast<long long>(g_lastIntentSeq), g_peerEpoch);
}

constexpr uint64_t kBridgeDismissLeadMs = 30;
constexpr uint64_t kBridgeAckTimeoutMs = 400;

constexpr uint64_t kDismissConfirmGraceMs = 300;

bool RequestBridgeShow(uint32_t method, uint32_t flags) {
    if (!BridgeCapabilityNow()) {

        TraceLog(L"exec show request skipped seq=none method=%u "
                L"reason=no-capability",
                method);
        return false;
    }
    if (!g_ipc.state || !g_ipc.execEvent) {
        TraceLog(L"exec show request skipped seq=none method=%u reason=%s",
                method, g_ipc.state ? L"no-executor" : L"no-channel");
        return false;
    }

    ForgetConfirmedDismiss();
    const LONG64 sequence = SwipeIpcPublishExec(
        &g_ipc, kSwipeExecShow, static_cast<int32_t>(method),
        static_cast<int32_t>(flags), 0, 0);
    if (sequence <= 0) {
        TraceLog(L"exec show request skipped seq=none method=%u "
                L"reason=no-sequence",
                method);
        return false;
    }
    g_pendingShowExec.store(sequence, std::memory_order_relaxed);
    g_pendingShowUntil.store(GetTickCount64() + kBridgeAckTimeoutMs,
                             std::memory_order_relaxed);
    g_controllerShowSeq = sequence;
    g_unconfirmedRepairUntil = 0;
    RetireShowWatch(L"new-show");

    NoteVisibilityTransition(kVisibilityWantOpen);
    TraceLog(L"exec show request seq=%lld method=%u flags=%u",
            static_cast<long long>(sequence), method, flags);
    return true;
}

constexpr int32_t kClickAwayShellMethod = 7;
constexpr uint32_t kClickAwayExecMethod = 6;

uint32_t ExecDismissMethodForIntent(int32_t intentMethod, int32_t intentTrigger) {
    if (intentTrigger == kSwipeTriggerClickAway &&
        intentMethod == kClickAwayShellMethod) {
        return kClickAwayExecMethod;
    }
    return static_cast<uint32_t>(intentMethod);
}

bool RequestBridgeDismiss(uint32_t method, uint64_t executeAt, uint64_t open) {
    if (!BridgeCapabilityNow()) {

        TraceLog(L"exec dismiss request skipped seq=none method=%u "
                L"reason=no-capability",
                method);
        return false;
    }
    if (!g_ipc.state || !g_ipc.execEvent) {
        TraceLog(L"exec dismiss request skipped seq=none method=%u reason=%s",
                method, g_ipc.state ? L"no-executor" : L"no-channel");
        return false;
    }

    uint64_t commandOpen = open;
    if (commandOpen == 0) {
        uint64_t currentGeneration = 0;
        uint64_t currentTick = 0;
        if (SwipeIpcReadOpen(&g_ipc, &currentGeneration, &currentTick)) {
            commandOpen = currentGeneration;
        }
    }
    const LONG64 sequence = SwipeIpcPublishExec(
        &g_ipc, kSwipeExecDismiss, static_cast<int32_t>(method), 0, executeAt,
        commandOpen);
    if (sequence <= 0) {
        TraceLog(L"exec dismiss request skipped seq=none method=%u "
                L"reason=no-sequence",
                method);
        return false;
    }
    g_pendingDismissExec.store(sequence, std::memory_order_relaxed);
    g_pendingDismissExecAt.store(executeAt, std::memory_order_relaxed);

    g_pendingDismissExecGeneration = commandOpen;

    // A close answers the show's question: the menu is not still opening.
    RetireShowWatch(L"dismiss");

    ForgetConfirmedDismiss();

    NoteVisibilityTransition(kVisibilityWantClosed);
    TraceLog(L"exec dismiss request seq=%lld method=%u executeAt=%llu open=%llu "
            L"intent_open=%llu",
            static_cast<long long>(sequence), method,
            static_cast<unsigned long long>(executeAt),
            static_cast<unsigned long long>(commandOpen),
            static_cast<unsigned long long>(open));
    return true;
}

LONG64 CancelPendingDismiss(const wchar_t* reason) {
    const LONG64 target =
        g_pendingDismissExec.exchange(0, std::memory_order_relaxed);
    g_pendingDismissExecAt.store(0, std::memory_order_relaxed);
    if (target == 0) {
        return 0;
    }
    if (g_ipc.state) {
        SwipeIpcPublishExec(&g_ipc, kSwipeExecCancel, 0, 0, 0, 0, target);
    }
    TraceLog(L"exec cancel request target=%lld reason=%s",
            static_cast<long long>(target), reason);
    return target;
}

// A show the executor has not dispatched still opens the menu it was published
// for. An external takeover cancels it on the same record the dismiss path
// uses, so the executor answers it superseded instead of running it.
LONG64 CancelPendingShow(const wchar_t* reason) {
    const LONG64 target =
        g_pendingShowExec.exchange(0, std::memory_order_relaxed);
    g_pendingShowUntil.store(0, std::memory_order_relaxed);
    if (target == 0) {
        return 0;
    }
    if (g_ipc.state) {
        SwipeIpcPublishExec(&g_ipc, kSwipeExecCancel, 0, 0, 0, 0, target);
    }
    TraceLog(L"exec cancel request target=%lld reason=%s",
            static_cast<long long>(target), reason);
    return target;
}

struct DismissBeforeCancel {
    uint64_t executeAt = 0;
    LONG64 confirm = 0;
};

DismissBeforeCancel ReadDismissBeforeCancel() {
    DismissBeforeCancel before;
    before.executeAt = g_pendingDismissExecAt.load(std::memory_order_relaxed);
    before.confirm = g_pendingDismissConfirm;
    return before;
}

void ArmCloseCancelTicket(LONG64 cancelled, const DismissBeforeCancel& before) {
    const LONG64 ticketSeq = cancelled != 0 ? cancelled : before.confirm;
    const uint64_t now = GetTickCount64();
    const uint64_t expireAt =
        ticketSeq != 0
            ? ((cancelled != 0 && before.executeAt != 0) ? before.executeAt : now) +
                  kBridgeAckTimeoutMs + kDismissConfirmGraceMs
            : 0;
    g_intentCancel =
        ticketSeq != 0
            ? CloseCancelTicket{true, g_ipcOpenTickLow, ticketSeq, expireAt}
            : CloseCancelTicket{};
}

void RetirePendingDismissForFreshOpen(uint64_t freshOpen) {
    if (g_pendingDismissConfirm != 0) {
        TraceLog(L"exec dismiss confirm dropped command=%lld gen=%llu "
                L"(fresh open=%llu)",
                static_cast<long long>(g_pendingDismissConfirm),
                static_cast<unsigned long long>(
                    g_pendingDismissConfirmGeneration),
                static_cast<unsigned long long>(freshOpen));
    }
    if (g_pendingDismissExec.load(std::memory_order_relaxed) != 0) {
        CancelPendingDismiss(L"fresh-open");
    }
    ForgetPendingDismiss();
}

void ReleaseExecutorOwnership();

struct HidDevice {
    HANDLE handle = nullptr;
    bool ready = false;
    bool decodeFailLogged = false;
    uint64_t lastUsedTick = 0;
    WCHAR path[kHidPathChars] = {};
    PHIDP_PREPARSED_DATA preparsed = nullptr;
    std::vector<HIDP_VALUE_CAPS> valueCaps;
    std::vector<HIDP_BUTTON_CAPS> buttonCaps;
    USHORT contactNodes[kMaxContactCollections] = {};
    int contactNodeCount = 0;
    USHORT contactCountLink = 0;
    bool hasContactCount = false;

    bool hasReportIds = false;
    bool hasTipSwitch = false;
    bool hasInRange = false;

    bool hasContactIdentifier = false;

    bool coordinatePath = false;
};

HidDevice g_hidDevices[kMaxHidDevices];
int g_hidDeviceCount = 0;
bool g_hidCacheFullLogged = false;

void CopyDevicePath(HANDLE handle, WCHAR* out, size_t outCount) {
    out[0] = L'\0';

    UINT chars = 0;
    SetLastError(0);
    if (GetRawInputDeviceInfoW(handle, RIDI_DEVICENAME, nullptr, &chars) ==
            static_cast<UINT>(-1) ||
        chars == 0) {
        wcsncpy_s(out, outCount, L"?", _TRUNCATE);
        return;
    }

    WCHAR* name = new (std::nothrow) WCHAR[chars + 1];
    if (!name) {
        wcsncpy_s(out, outCount, L"?", _TRUNCATE);
        return;
    }

    name[0] = L'\0';
    UINT fetched = chars;
    SetLastError(0);
    if (GetRawInputDeviceInfoW(handle, RIDI_DEVICENAME, name, &fetched) ==
        static_cast<UINT>(-1)) {
        wcsncpy_s(out, outCount, L"?", _TRUNCATE);
    } else {
        name[chars] = L'\0';
        wcsncpy_s(out, outCount, name, _TRUNCATE);
    }

    delete[] name;
}

void LogDecodeFail(HidDevice& device, unsigned error) {
    if (device.decodeFailLogged) {
        return;
    }

    device.decodeFailLogged = true;
    TraceLog(L"hid decodefail dev=%s err=%u", device.path, error);
}

void InitHidDevice(HidDevice& device) {
    CopyDevicePath(device.handle, device.path, ARRAYSIZE(device.path));

    UINT bytes = 0;
    SetLastError(0);
    if (GetRawInputDeviceInfoW(device.handle, RIDI_PREPARSEDDATA, nullptr,
                               &bytes) == static_cast<UINT>(-1) ||
        bytes == 0 || bytes > 64 * 1024) {
        LogDecodeFail(device, GetLastError());
        return;
    }

    BYTE* blob = new (std::nothrow) BYTE[bytes];
    if (!blob) {
        return;
    }

    UINT fetched = bytes;
    SetLastError(0);
    if (GetRawInputDeviceInfoW(device.handle, RIDI_PREPARSEDDATA, blob,
                               &fetched) == static_cast<UINT>(-1) ||
        fetched == 0) {
        LogDecodeFail(device, GetLastError());
        delete[] blob;
        return;
    }

    device.preparsed = reinterpret_cast<PHIDP_PREPARSED_DATA>(blob);

    HIDP_CAPS caps{};
    const NTSTATUS capsStatus = HidP_GetCaps(device.preparsed, &caps);
    if (capsStatus != HIDP_STATUS_SUCCESS) {
        device.decodeFailLogged = true;
        TraceLog(L"hid decodefail dev=%s err=0x%08X", device.path,
                static_cast<unsigned>(capsStatus));
        return;
    }

    if (caps.NumberInputValueCaps > 0) {
        device.valueCaps.resize(caps.NumberInputValueCaps);
        USHORT length = caps.NumberInputValueCaps;
        if (HidP_GetValueCaps(HidP_Input, device.valueCaps.data(), &length,
                              device.preparsed) == HIDP_STATUS_SUCCESS) {
            device.valueCaps.resize(length);
        } else {
            device.valueCaps.clear();
        }
    }

    if (caps.NumberInputButtonCaps > 0) {
        device.buttonCaps.resize(caps.NumberInputButtonCaps);
        USHORT length = caps.NumberInputButtonCaps;
        if (HidP_GetButtonCaps(HidP_Input, device.buttonCaps.data(), &length,
                               device.preparsed) == HIDP_STATUS_SUCCESS) {
            device.buttonCaps.resize(length);
        } else {
            device.buttonCaps.clear();
        }
    }

    for (const HIDP_VALUE_CAPS& cap : device.valueCaps) {
        if (cap.ReportID != 0) {
            device.hasReportIds = true;
            break;
        }
    }
    if (!device.hasReportIds) {
        for (const HIDP_BUTTON_CAPS& cap : device.buttonCaps) {
            if (cap.ReportID != 0) {
                device.hasReportIds = true;
                break;
            }
        }
    }

    HIDP_LINK_COLLECTION_NODE nodes[kMaxLinkNodes]{};
    ULONG nodeCount = kMaxLinkNodes;
    if (HidP_GetLinkCollectionNodes(nodes, &nodeCount, device.preparsed) ==
        HIDP_STATUS_SUCCESS) {
        if (nodeCount > kMaxLinkNodes) {
            nodeCount = kMaxLinkNodes;
        }

        for (int pass = 0; pass < 2; ++pass) {
            for (ULONG i = 0; i < nodeCount; ++i) {
                if (nodes[i].LinkUsagePage != kUsagePageDigitizer) {
                    continue;
                }
                const USAGE usage = nodes[i].LinkUsage;
                const bool isContact =
                    usage == kUsageContact || usage == kUsageFinger;
                const bool isPen = usage == kUsagePen || usage == kUsageStylus;
                if ((pass == 0) != isContact || (!isContact && !isPen)) {
                    continue;
                }
                if (device.contactNodeCount < kMaxContactCollections) {
                    device.contactNodes[device.contactNodeCount++] =
                        static_cast<USHORT>(i);
                }
            }
        }
    }

    for (const HIDP_BUTTON_CAPS& cap : device.buttonCaps) {
        if (cap.UsagePage != kUsagePageDigitizer) {
            continue;
        }
        const bool matchesTip =
            cap.IsRange ? (cap.Range.UsageMin <= kUsageTipSwitch &&
                           kUsageTipSwitch <= cap.Range.UsageMax)
                        : (cap.NotRange.Usage == kUsageTipSwitch);
        const bool matchesRange =
            cap.IsRange ? (cap.Range.UsageMin <= kUsageInRange &&
                           kUsageInRange <= cap.Range.UsageMax)
                        : (cap.NotRange.Usage == kUsageInRange);
        device.hasTipSwitch = device.hasTipSwitch || matchesTip;
        device.hasInRange = device.hasInRange || matchesRange;
    }

    device.coordinatePath = !device.hasTipSwitch && !device.hasInRange;

    for (const HIDP_VALUE_CAPS& cap : device.valueCaps) {
        if (cap.UsagePage != kUsagePageDigitizer) {
            continue;
        }
        const bool matches =
            cap.IsRange ? (cap.Range.UsageMin <= kUsageContactIdentifier &&
                           kUsageContactIdentifier <= cap.Range.UsageMax)
                        : (cap.NotRange.Usage == kUsageContactIdentifier);
        if (!matches) {
            continue;
        }
        for (int i = 0; i < device.contactNodeCount; ++i) {
            if (cap.LinkCollection == device.contactNodes[i]) {
                device.hasContactIdentifier = true;
                break;
            }
        }
        if (device.hasContactIdentifier) {
            break;
        }
    }

    for (const HIDP_VALUE_CAPS& cap : device.valueCaps) {
        if (cap.UsagePage != kUsagePageDigitizer) {
            continue;
        }
        const bool matches =
            cap.IsRange ? (cap.Range.UsageMin <= kUsageContactCount &&
                           kUsageContactCount <= cap.Range.UsageMax)
                        : (cap.NotRange.Usage == kUsageContactCount);
        if (!matches) {
            continue;
        }
        device.contactCountLink = cap.LinkCollection;
        device.hasContactCount = true;
        break;
    }
    TraceLog(L"hidcc dev=%s have=%d link=%u", device.path,
            device.hasContactCount ? 1 : 0,
            static_cast<unsigned>(device.contactCountLink));

    TraceLog(L"hid usages dev=%s tip=%d inrange=%d count=%d buttons=%u "
            L"reportids=%d coordpath=%d identity=%s",
            device.path, device.hasTipSwitch ? 1 : 0,
            device.hasInRange ? 1 : 0, device.hasContactCount ? 1 : 0,
            static_cast<unsigned>(device.buttonCaps.size()),
            device.hasReportIds ? 1 : 0, device.coordinatePath ? 1 : 0,
            device.hasContactIdentifier ? L"contactid" : L"node");

    device.ready = true;
}

HidDevice* GetHidDevice(HANDLE handle) {
    if (!handle) {
        return nullptr;
    }

    const uint64_t now = GetTickCount64();
    for (int i = 0; i < g_hidDeviceCount; ++i) {
        if (g_hidDevices[i].handle == handle) {
            g_hidDevices[i].lastUsedTick = now;
            return &g_hidDevices[i];
        }
    }

    HidDevice* device = nullptr;
    if (g_hidDeviceCount < kMaxHidDevices) {
        device = &g_hidDevices[g_hidDeviceCount++];
    } else {

        device = &g_hidDevices[0];
        for (int i = 1; i < g_hidDeviceCount; ++i) {
            if (g_hidDevices[i].lastUsedTick < device->lastUsedTick) {
                device = &g_hidDevices[i];
            }
        }
        delete[] reinterpret_cast<BYTE*>(device->preparsed);
        if (!g_hidCacheFullLogged) {
            g_hidCacheFullLogged = true;
            TraceLog(L"hid device table recycled (was full)");
        }
    }

    *device = HidDevice{};
    device->handle = handle;
    device->lastUsedTick = now;
    InitHidDevice(*device);
    return device;
}

void FreeHidDevices() {
    for (int i = 0; i < g_hidDeviceCount; ++i) {
        delete[] reinterpret_cast<BYTE*>(g_hidDevices[i].preparsed);
        g_hidDevices[i] = HidDevice{};
    }
    g_hidDeviceCount = 0;
}

enum class HidField : uint8_t { Ok, Absent, Error };

HidField ClassifyHidStatus(NTSTATUS status) {
    if (status == HIDP_STATUS_SUCCESS) {
        return HidField::Ok;
    }

    if (status == HIDP_STATUS_USAGE_NOT_FOUND ||
        status == HIDP_STATUS_INCOMPATIBLE_REPORT_ID ||
        status == HIDP_STATUS_REPORT_DOES_NOT_EXIST) {
        return HidField::Absent;
    }
    return HidField::Error;
}

HidField ReadUsageValue(const HidDevice& device, USAGE usagePage,
                        USHORT collection, USAGE usage, const BYTE* report,
                        ULONG reportLength, ULONG* value,
                        NTSTATUS* statusOut = nullptr) {
    if (!device.preparsed) {
        if (statusOut) {
            *statusOut = HIDP_STATUS_INVALID_PREPARSED_DATA;
        }
        return HidField::Error;
    }

    ULONG parsed = 0;
    const NTSTATUS status = HidP_GetUsageValue(
        HidP_Input, usagePage, collection, usage, &parsed, device.preparsed,
        reinterpret_cast<PCHAR>(const_cast<BYTE*>(report)), reportLength);
    if (statusOut) {
        *statusOut = status;
    }
    if (status != HIDP_STATUS_SUCCESS) {
        return ClassifyHidStatus(status);
    }

    *value = parsed;
    return HidField::Ok;
}

struct HidAxisRange {
    long min = 0;
    long max = 0;
    bool valid = false;
};

HidAxisRange FindAxisRange(const HidDevice& device, USAGE usage,
                           USHORT reportId, USHORT collection) {
    HidAxisRange range;

    for (const HIDP_VALUE_CAPS& cap : device.valueCaps) {
        if (cap.UsagePage != kUsagePageGeneric ||
            cap.LinkCollection != collection) {
            continue;
        }

        const bool usageMatches =
            cap.IsRange
                ? (usage >= cap.Range.UsageMin && usage <= cap.Range.UsageMax)
                : (cap.NotRange.Usage == usage);
        if (!usageMatches) {
            continue;
        }
        if (device.hasReportIds ? cap.ReportID != reportId
                                : cap.ReportID != 0) {
            continue;
        }
        if (cap.LogicalMax <= cap.LogicalMin) {
            continue;
        }

        range.min = cap.LogicalMin;
        range.max = cap.LogicalMax;
        range.valid = true;
        break;
    }

    return range;
}

struct HidContactButtons {
    HidField field = HidField::Absent;
    NTSTATUS status = HIDP_STATUS_SUCCESS;
    bool tipDeclared = false;
    bool tipDown = false;
    bool inRangeDeclared = false;
    bool inRangeDown = false;
};

HidContactButtons ReadContactButtons(const HidDevice& device, USHORT collection,
                                     USHORT reportId, const BYTE* report,
                                     ULONG reportLength) {
    HidContactButtons state;
    if (!device.preparsed) {
        state.field = HidField::Error;
        state.status = HIDP_STATUS_INVALID_PREPARSED_DATA;
        return state;
    }
    if (device.buttonCaps.empty()) {
        return state;
    }

    USAGE usages[128] = {};
    ULONG count = ARRAYSIZE(usages);
    const NTSTATUS status = HidP_GetUsages(
        HidP_Input, kUsagePageDigitizer, collection, usages, &count,
        device.preparsed, reinterpret_cast<PCHAR>(const_cast<BYTE*>(report)),
        reportLength);
    state.status = status;
    state.field = ClassifyHidStatus(status);
    if (state.field != HidField::Ok) {

        return state;
    }

    for (const HIDP_BUTTON_CAPS& cap : device.buttonCaps) {
        if (cap.UsagePage != kUsagePageDigitizer ||
            cap.LinkCollection != collection ||
            (device.hasReportIds ? cap.ReportID != reportId
                                 : cap.ReportID != 0)) {
            continue;
        }

        const auto pressed = [&usages, count](USAGE usage) {
            for (ULONG i = 0; i < count; ++i) {
                if (usages[i] == usage) {
                    return true;
                }
            }
            return false;
        };
        const bool matchesTip =
            cap.IsRange ? (cap.Range.UsageMin <= kUsageTipSwitch &&
                           kUsageTipSwitch <= cap.Range.UsageMax)
                        : (cap.NotRange.Usage == kUsageTipSwitch);
        const bool matchesRange =
            cap.IsRange ? (cap.Range.UsageMin <= kUsageInRange &&
                           kUsageInRange <= cap.Range.UsageMax)
                        : (cap.NotRange.Usage == kUsageInRange);
        if (matchesTip) {
            state.tipDeclared = true;
            state.tipDown = state.tipDown || pressed(kUsageTipSwitch);
        }
        if (matchesRange) {
            state.inRangeDeclared = true;
            state.inRangeDown = state.inRangeDown || pressed(kUsageInRange);
        }
    }

    return state;
}

struct HidContactSlot {
    bool declared = false;
    bool hasPosition = false;
    bool atOrigin = false;
    long rawX = 0;
    long rawY = 0;

    bool hasIdentifier = false;
    ULONG identifier = 0;
    HidContactButtons buttons{};
};

bool DecodeContactSlot(const HidDevice& device, USHORT collection,
                       USHORT reportId, const BYTE* report, ULONG reportLength,
                       HidContactSlot& slot, NTSTATUS& error) {
    ULONG x = 0;
    ULONG y = 0;
    NTSTATUS status = HIDP_STATUS_SUCCESS;
    const HidField xField = ReadUsageValue(device, kUsagePageGeneric, collection,
                                           kUsageX, report, reportLength, &x,
                                           &status);
    if (xField == HidField::Error) {
        error = status;
        return false;
    }
    const HidField yField = ReadUsageValue(device, kUsagePageGeneric, collection,
                                           kUsageY, report, reportLength, &y,
                                           &status);
    if (yField == HidField::Error) {
        error = status;
        return false;
    }

    slot.hasPosition = xField == HidField::Ok && yField == HidField::Ok;
    slot.atOrigin = slot.hasPosition && x == 0 && y == 0;
    slot.rawX = static_cast<long>(x);
    slot.rawY = static_cast<long>(y);

    if (device.hasContactIdentifier) {
        ULONG identifier = 0;
        const HidField idField =
            ReadUsageValue(device, kUsagePageDigitizer, collection,
                           kUsageContactIdentifier, report, reportLength,
                           &identifier, &status);
        if (idField == HidField::Error) {
            error = status;
            return false;
        }
        slot.hasIdentifier = idField == HidField::Ok;
        slot.identifier = identifier;
    }

    slot.buttons =
        ReadContactButtons(device, collection, reportId, report, reportLength);
    if (slot.buttons.field == HidField::Error) {
        error = slot.buttons.status;
        return false;
    }

    slot.declared = xField == HidField::Ok || yField == HidField::Ok ||
                    slot.buttons.field == HidField::Ok;
    return true;
}

enum class HidContactState : uint8_t { Contact, NotContact, Unknown };

HidContactState SlotContactState(const HidContactSlot& slot) {
    if (slot.buttons.tipDeclared) {
        return slot.buttons.tipDown ? HidContactState::Contact
                                    : HidContactState::NotContact;
    }
    if (slot.buttons.inRangeDeclared) {
        return slot.buttons.inRangeDown ? HidContactState::Contact
                                        : HidContactState::NotContact;
    }
    return HidContactState::Unknown;
}

int32_t ToScreenPixel(long raw, const HidAxisRange& range, int screenSize) {
    if (!range.valid || screenSize <= 1) {
        return static_cast<int32_t>(raw);
    }

    const int64_t span = static_cast<int64_t>(range.max) - range.min;
    if (span <= 0) {
        return static_cast<int32_t>(raw);
    }

    const int64_t value = static_cast<int64_t>(raw) - range.min;
    int64_t pixel = value * (screenSize - 1) / span;
    if (pixel < 0) {
        pixel = 0;
    }
    if (pixel > screenSize - 1) {
        pixel = screenSize - 1;
    }
    return static_cast<int32_t>(pixel);
}

HWND FindStartCoreWindow() {
    DWORD processId = GetCurrentProcessId();
    HWND hwnd = FindWindowW(L"Windows.UI.Core.CoreWindow", nullptr);
    while (hwnd) {
        DWORD owner = 0;
        GetWindowThreadProcessId(hwnd, &owner);
        if (owner == processId) {
            return hwnd;
        }
        hwnd = FindWindowExW(nullptr, hwnd, L"Windows.UI.Core.CoreWindow",
                             nullptr);
    }
    return nullptr;
}

using RunFromWindowThreadProc = void(WINAPI*)(PVOID);

struct RunFromWindowThreadParam {
    RunFromWindowThreadProc proc;
    PVOID param;
    bool ran;
};

constexpr WPARAM kRunFromWindowThreadMagic = 0x53575441;
RunFromWindowThreadParam g_attachCall{};
std::atomic<HHOOK> g_startAttachHook{nullptr};

std::atomic<int> g_startAttachFrames{0};
HANDLE g_startAttachIdle = nullptr;

UINT RunFromWindowThreadMessage() {
    static const UINT message = RegisterWindowMessageW(
        L"WindhawkSwipeFor10Start_RunFromWindowThread");
    return message;
}

LRESULT CALLBACK RunFromWindowThreadHook(int code, WPARAM wParam,
                                         LPARAM lParam) {
    g_startAttachFrames.fetch_add(1, std::memory_order_acq_rel);
    if (code == HC_ACTION && wParam == PM_REMOVE) {
        auto* message = reinterpret_cast<MSG*>(lParam);
        if (message && message->message == RunFromWindowThreadMessage() &&
            message->wParam == kRunFromWindowThreadMagic &&
            message->lParam == reinterpret_cast<LPARAM>(&g_attachCall) &&
            g_attachCall.proc) {
            g_attachCall.ran = true;
            g_attachCall.proc(g_attachCall.param);

            const HHOOK hook =
                g_startAttachHook.exchange(nullptr, std::memory_order_acq_rel);
            message->lParam = 0;
            if (hook) {
                UnhookWindowsHookEx(hook);
            }
        }
    }

    const LRESULT result = CallNextHookEx(nullptr, code, wParam, lParam);
    if (g_startAttachFrames.load(std::memory_order_acquire) == 1 &&
        g_startAttachIdle) {
        SetEvent(g_startAttachIdle);
    }
    g_startAttachFrames.fetch_sub(1, std::memory_order_acq_rel);
    return result;
}

bool RunFromWindowThread(HWND hwnd, RunFromWindowThreadProc proc,
                         PVOID param) {
    if (!hwnd || !proc) {
        return false;
    }
    if (g_startAttachHook.load(std::memory_order_acquire)) {
        TraceLog(L"run on window thread already posted");
        return false;
    }
    const UINT message = RunFromWindowThreadMessage();
    if (!message) {
        TraceLog(L"run on window thread: registered message unavailable");
        return false;
    }
    const DWORD threadId = GetWindowThreadProcessId(hwnd, nullptr);
    if (threadId == 0) {
        return false;
    }
    if (threadId == GetCurrentThreadId()) {
        proc(param);
        return true;
    }
    HHOOK hook = SetWindowsHookExW(WH_GETMESSAGE, RunFromWindowThreadHook,
                                   nullptr, threadId);
    if (!hook) {
        TraceLog(L"run on window thread: hook failed tid=%u err=%u", threadId,
                GetLastError());
        return false;
    }
    g_attachCall = RunFromWindowThreadParam{proc, param, false};
    g_startAttachHook.store(hook, std::memory_order_release);
    if (!PostMessageW(hwnd, message, kRunFromWindowThreadMagic,
                      reinterpret_cast<LPARAM>(&g_attachCall))) {
        g_startAttachHook.store(nullptr, std::memory_order_release);
        UnhookWindowsHookEx(hook);
        return false;
    }
    TraceLog(L"run on window thread posted hwnd=%p tid=%u", hwnd, threadId);
    return true;
}

void RemoveStartAttachHook() {
    HHOOK hook = g_startAttachHook.exchange(nullptr, std::memory_order_acq_rel);
    if (hook) {
        TraceLog(L"start attach hook removed before service");
        UnhookWindowsHookEx(hook);
    }
}

[[noreturn]] void BlockUnload(const wchar_t* reason);

constexpr DWORD kHookFrameWaitMs = 200;

void WaitForStartAttachFrames() {

    if (g_startAttachFrames.load(std::memory_order_acquire) != 0 &&
        g_uiThreadId.load(std::memory_order_relaxed) == GetCurrentThreadId()) {
        BlockUnload(L"own hook frame");
    }
    if (g_startAttachFrames.load(std::memory_order_acquire) == 0) {
        return;
    }
    if (!g_startAttachIdle) {
        BlockUnload(L"attach hook frame event missing");
    }
    while (g_startAttachFrames.load(std::memory_order_acquire) != 0) {
        const DWORD wait =
            WaitForSingleObject(g_startAttachIdle, kHookFrameWaitMs);
        if (wait != WAIT_OBJECT_0 && wait != WAIT_TIMEOUT) {
            BlockUnload(L"attach hook frame wait failed");
        }
    }
    TraceLog(L"unload attach hook frames drained");
}

bool g_openCheckValid = false;
bool g_openCheckOpen = false;
uint64_t g_lastOpenCheckTick = 0;

enum class CloakState { Open, Hidden, Unknown };

CloakState ReadCloakState() {
    HWND hwnd = g_coreHwnd.load(std::memory_order_relaxed);
    if (!hwnd) {
        return CloakState::Unknown;
    }

    DwmGetWindowAttribute_t dwmGetWindowAttribute =
        g_dwmGetWindowAttribute.load(std::memory_order_relaxed);
    if (!dwmGetWindowAttribute) {
        return CloakState::Unknown;
    }

    DWORD cloaked = 0;
    const HRESULT hr =
        dwmGetWindowAttribute(hwnd, kDwmwaCloaked, &cloaked, sizeof(cloaked));
    if (hr != S_OK) {
        return CloakState::Unknown;
    }
    return cloaked == 0 ? CloakState::Open : CloakState::Hidden;
}

const wchar_t* CloakStateName(CloakState state) {
    switch (state) {
        case CloakState::Open:
            return L"open";
        case CloakState::Hidden:
            return L"hidden";
        default:
            return L"unknown";
    }
}

bool StartWindowIsOpen() {
    return ReadCloakState() == CloakState::Open;
}

bool StartWindowIsOpenThrottled(uint64_t tickMs) {
    if (g_openCheckValid &&
        tickMs - g_lastOpenCheckTick < kOpenCheckIntervalMs) {
        return g_openCheckOpen;
    }

    g_lastOpenCheckTick = tickMs;
    g_openCheckValid = true;
    g_openCheckOpen = StartWindowIsOpen();
    return g_openCheckOpen;
}

HWND g_hidDpiHwnd = nullptr;
float g_hidDpiScale = 1.0f;

void ResetHidDpiScale() { g_hidDpiHwnd = nullptr; }

float HidDpiScale() {
    HWND hwnd = g_coreHwnd.load(std::memory_order_relaxed);
    if (!hwnd) {
        hwnd = FindStartCoreWindow();
        if (hwnd) {
            g_coreHwnd.store(hwnd, std::memory_order_relaxed);
        }
    }
    if (!hwnd) {
        return 1.0f;
    }

    if (hwnd != g_hidDpiHwnd) {
        const UINT dpi = GetDpiForWindow(hwnd);
        if (dpi == 0) {

            return 1.0f;
        }
        g_hidDpiHwnd = hwnd;
        g_hidDpiScale = static_cast<float>(dpi) / 96.0f;
    }

    return g_hidDpiScale;
}

struct HidGestureSession {
    bool active = false;
    bool suppress = false;
    long lastRawX = 0;
    long lastRawY = 0;
    float startX = 0.0f;
    float startY = 0.0f;

    float startProgress = 1.0f;
    uint64_t startTick = 0;
    uint64_t lastTick = 0;

    uint64_t lastSampleQpc = 0;

    uint64_t lastAliveQpc = 0;
    float curDelta = 0.0f;
    float maxDelta = 0.0f;
    float maxPerp = 0.0f;
    float lastProgress = 1.0f;
    int samples = 0;
};

HidGestureSession g_hidSession;

struct HidRevealSession {
    bool edgeCandidate = false;

    bool granted = false;

    bool claimAttempted = false;

    bool liveTakeover = false;

    bool liveBound = false;

    float liveOffset = 0.0f;
    float liveStart = 0.0f;
    bool active = false;
    float edgeStartX = 0.0f;
    float edgeStartY = 0.0f;

    bool grantDownInBand = false;
    float distance = 0.0f;
    float panelTrail = 0.0f;
    float travel = 0.0f;
    float maxTravel = 0.0f;
    float progress = 0.0f;

    float visible = 0.0f;
    float target = 0.0f;

    float activationGap = 0.0f;

    uint64_t chaseQpc = 0;
    float lastInserted = 1.0f;
    int samples = 0;
    uint64_t startTick = 0;
    uint64_t lastTick = 0;
    uint64_t lastSampleQpc = 0;
    uint64_t lastAliveQpc = 0;

    uint64_t activeSinceQpc = 0;
};

HidRevealSession g_reveal;

enum class HidDownAuthor {
    None,
    Band,
    Grab,
};

struct HidContactOrigin {
    bool valid = false;
    bool startedOpen = false;

    HidDownAuthor author = HidDownAuthor::None;
    float downX = 0.0f;
    float downY = 0.0f;
    float x = 0.0f;
    float y = 0.0f;
    uint64_t downTick = 0;

    struct Point {
        float x = 0.0f;
        float y = 0.0f;
        uint64_t qpc = 0;
    };
    Point points[kEndVelocitySamples];
    int pointCount = 0;
};

HidContactOrigin g_contactOrigin;

void SampleOriginPoint(float x, float y, uint64_t qpcMs) {
    if (g_contactOrigin.pointCount == kEndVelocitySamples) {
        std::memmove(g_contactOrigin.points, g_contactOrigin.points + 1,
                     sizeof(HidContactOrigin::Point) *
                         (kEndVelocitySamples - 1));
        g_contactOrigin.pointCount--;
    }
    g_contactOrigin.points[g_contactOrigin.pointCount++] =
        HidContactOrigin::Point{x, y, qpcMs};
}

struct RevealLateRelease {
    bool valid = false;
    uint64_t releaseQpc = 0;
    float travel = 0.0f;

    float position = 0.0f;
    float velocity = 0.0f;
    uint32_t pullSerial = 0;
};

RevealLateRelease g_lateRelease;

constexpr uint64_t kLateRevealWindowMs = 300;

bool g_contactDown = false;
uint64_t g_contactTick = 0;

struct HidContactTrack {
    HANDLE device = nullptr;
    int slot = -1;
    bool hasIdentifier = false;
    ULONG identifier = 0;
    uint64_t lastEvidenceTick = 0;
};

HidContactTrack g_trackedContact;

bool g_duplicateContactIdLogged = false;

void ForgetTrackedContact() {
    g_trackedContact = HidContactTrack{};
}

void EndConsumedContact() {
    g_hidSession.suppress = false;
    g_contactDown = false;
    g_contactTick = 0;
    g_contactOrigin = HidContactOrigin{};
    g_openCheckValid = false;
    EndContactTiming();
    ForgetTrackedContact();
}

float g_screenDipWidth = 0.0f;
float g_screenDipHeight = 0.0f;

bool g_commitNeedsProgressReset = false;
bool g_rootWasOpen = false;

bool CloseInFlight() {
    return g_close.phase != ClosePhase::Done;
}

bool CloseHideMayLand() {
    return g_pendingDismissExec.load(std::memory_order_relaxed) != 0 ||
           g_pendingDismissConfirm != 0;
}

const wchar_t* ClosePhaseName(ClosePhase phase) {
    switch (phase) {
        case ClosePhase::Settling:
            return L"settling";
        case ClosePhase::AwaitingHide:
            return L"awaiting-hide";
        case ClosePhase::Done:
            return L"done";
    }
    return L"done";
}

void TakeCloseOwnership(CloseKind kind, uint64_t tickMs,
                        bool contactStillDown) {

    EndClose(L"superseded");
    g_close.serial = ++g_closeSerial;
    g_close.phase = ClosePhase::Settling;
    g_close.acceptedTick = tickMs;
    g_close.repairAt = tickMs + kCloseRepairMs;
    g_close.kind = kind;

    uint64_t openGeneration = 0;
    uint64_t openTick = 0;
    if (SwipeIpcReadOpen(&g_ipc, &openGeneration, &openTick)) {
        g_close.openGeneration = openGeneration;
        g_close.openTick = static_cast<uint32_t>(openTick);
    } else {
        g_close.openGeneration = 0;
        g_close.openTick = 0;
    }
    TraceLog(L"close accepted serial=%u kind=%s open=%u gen=%llu repair=%llu",
            g_close.serial, CloseKindName(kind), g_close.openTick,
            static_cast<unsigned long long>(g_close.openGeneration),
            static_cast<unsigned long long>(g_close.repairAt));

    SetControllerTouch(false);

    g_unconfirmedRepairUntil = 0;
    RetireShowWatch(L"close");

    ForgetConfirmedDismiss();

    SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionClosing);

    g_intentCancel = CloseCancelTicket{};

    g_lateRelease = RevealLateRelease{};

    const bool contactLive =
        g_contactDown || g_hidSession.active || g_reveal.edgeCandidate;
    if (contactStillDown && contactLive) {
        g_hidSession.suppress = true;
    }

    g_hidSession.active = false;
    g_reveal.granted = false;
    g_reveal.edgeCandidate = false;
    g_reveal.active = false;
    EndContactTiming();

    PublishPanelOwnership();
}

void NoteCloseDismissPublished() {
    if (g_close.phase == ClosePhase::Settling) {
        g_close.phase = ClosePhase::AwaitingHide;
    }
}

void EndClose(const wchar_t* reason) {
    if (g_close.phase == ClosePhase::Done) {
        return;
    }
    const uint64_t now = GetTickCount64();
    TraceLog(L"close ended serial=%u kind=%s phase=%s reason=%s age=%llums",
            g_close.serial, CloseKindName(g_close.kind),
            ClosePhaseName(g_close.phase), reason,
            static_cast<unsigned long long>(now > g_close.acceptedTick
                                                ? now - g_close.acceptedTick
                                                : 0));
    g_close.phase = ClosePhase::Done;
}

void RepairExpiredClose(uint64_t tickMs) {
    if (g_close.phase == ClosePhase::Done || tickMs < g_close.repairAt) {
        return;
    }
    EndClose(L"repair-deadline");
    ReleaseExecutorOwnership();
}

struct CoastVelocitySample {
    float progress = 0.0f;
    uint64_t qpc = 0;
};

constexpr int kCoastVelocitySamples = 3;
CoastVelocitySample g_coastVelocity[kCoastVelocitySamples];
int g_coastVelocityCount = 0;

void ResetCoastVelocity() {
    g_coastVelocityCount = 0;
}

void SampleCoastVelocity(float progress, uint64_t qpcMs) {
    if (g_coastVelocityCount == kCoastVelocitySamples) {
        std::memmove(g_coastVelocity, g_coastVelocity + 1,
                     sizeof(CoastVelocitySample) *
                         (kCoastVelocitySamples - 1));
        g_coastVelocityCount--;
    }
    g_coastVelocity[g_coastVelocityCount++] =
        CoastVelocitySample{progress, qpcMs};
}

float CoastVelocity(float distance) {
    if (g_coastVelocityCount < 2 || !(distance > 0.0f)) {
        return 0.0f;
    }

    const CoastVelocitySample& oldest = g_coastVelocity[0];
    const CoastVelocitySample& newest =
        g_coastVelocity[g_coastVelocityCount - 1];
    if (newest.qpc <= oldest.qpc) {
        return 0.0f;
    }

    const float elapsedMs = static_cast<float>(newest.qpc - oldest.qpc);
    const float velocity = (newest.progress - oldest.progress) / elapsedMs;
    const float limit = kCoastMaxVelocityDip / distance;
    return std::clamp(velocity, -limit, limit);
}

constexpr uint64_t kEndVelocityWindowMs = 30;
constexpr uint64_t kEndVelocityMinMs = 8;

struct EndVelocitySample {
    float progress = 0.0f;
    uint64_t qpc = 0;
};

EndVelocitySample g_endVelocity[kEndVelocitySamples];
int g_endVelocityCount = 0;

void ResetEndVelocity() {
    g_endVelocityCount = 0;
}

void SampleEndVelocity(float progress, uint64_t qpcMs) {
    if (g_endVelocityCount > 0) {
        EndVelocitySample& newest = g_endVelocity[g_endVelocityCount - 1];
        if (newest.progress == progress) {
            newest.qpc = qpcMs;
            return;
        }
    }

    if (g_endVelocityCount == kEndVelocitySamples) {
        std::memmove(g_endVelocity, g_endVelocity + 1,
                     sizeof(EndVelocitySample) * (kEndVelocitySamples - 1));
        g_endVelocityCount--;
    }
    g_endVelocity[g_endVelocityCount++] = EndVelocitySample{progress, qpcMs};
}

float EndVelocityProgressPerMs(uint64_t releaseQpc) {
    float result = 0.0f;

    do {
        if (g_endVelocityCount < 2) {
            break;
        }

        const EndVelocitySample& newest =
            g_endVelocity[g_endVelocityCount - 1];
        if (releaseQpc > newest.qpc &&
            releaseQpc - newest.qpc > kEndVelocityWindowMs) {
            break;
        }
        const EndVelocitySample* first = nullptr;

        for (int i = 0; i < g_endVelocityCount - 1; ++i) {
            const uint64_t age =
                newest.qpc >= g_endVelocity[i].qpc
                    ? newest.qpc - g_endVelocity[i].qpc
                    : 0;
            if (age > kEndVelocityWindowMs) {
                continue;
            }
            if (age < kEndVelocityMinMs) {
                break;
            }
            first = &g_endVelocity[i];
            break;
        }

        if (!first || newest.qpc <= first->qpc) {
            break;
        }

        const float elapsed = static_cast<float>(newest.qpc - first->qpc);
        if (!(elapsed > 0.0f)) {
            break;
        }
        result = (newest.progress - first->progress) / elapsed;
    } while (false);

    return result;
}

void ResetActivationState() {
    ForgetTrackedContact();
    g_duplicateContactIdLogged = false;
    g_contactDown = false;
    g_contactTick = 0;
    g_contactOrigin = HidContactOrigin{};
    g_lateRelease = RevealLateRelease{};

    g_hidSession = HidGestureSession{};
    g_reveal = HidRevealSession{};

    g_close = CloseRecord{};
    g_closeSerial = 0;
    g_measureDeferred.store(false, std::memory_order_relaxed);
    g_panelOwned.store(false, std::memory_order_relaxed);
    g_dismissSettling.store(false, std::memory_order_relaxed);
    g_dismissSerial.store(0, std::memory_order_relaxed);

    g_pendingTapTick = 0;
    g_pendingTapKind = CloseKind::None;
    g_pendingTapPull = 0;
    g_pendingTapSerial = 0;
    g_pullSerial = 0;
    g_touchRegistered = false;
    g_penRegistered = false;

    g_pendingShowExec.store(0, std::memory_order_relaxed);
    g_pendingShowUntil.store(0, std::memory_order_relaxed);
    g_showWatchUntil = 0;
    g_showWatchSeq = 0;
    g_controllerShowSeq = 0;
    g_unconfirmedRepairUntil = 0;

    ForgetPendingDismiss();
    g_intentOpenActive.store(false, std::memory_order_relaxed);
    g_intentOpenEntry.store(0.0f, std::memory_order_relaxed);
    g_openClaimTick = 0;

    g_lastIntentSeq = 0;
    g_intentCancel = CloseCancelTicket{};

    g_peerEpoch = 0;

    g_panel.coasting.store(false, std::memory_order_relaxed);
    g_panel.coastStartQpc = 0;
    g_panel.coastFrom = 1.0f;
    g_panel.coastTo = 1.0f;
    ResetCoastVelocity();
    ResetEndVelocity();

    g_commitNeedsProgressReset = false;
    g_rootWasOpen = false;
    g_openCheckValid = false;
    g_openCheckOpen = false;
    g_lastOpenCheckTick = 0;
    g_skipLogTick = 0;
    g_unhealedLogTick = 0;
    g_scrollerRejectLogTick = 0;
    g_bandTailLogTick = 0;
    g_closeCancelledTick = 0;
    g_unconfirmedRepairUntil = 0;

    g_ipcOpenTickLow = 0;
    g_peerTickLast = 0;

    EndContactTiming();
    TraceLog(L"activation state reset");
}

bool ClaimPanelForAdmission() {
    g_panelOwned.store(true, std::memory_order_seq_cst);
    if (g_layoutApplying.load(std::memory_order_seq_cst) != 0) {
        g_panelOwned.store(g_hidSession.active || g_reveal.edgeCandidate,
                           std::memory_order_seq_cst);
        return false;
    }
    return true;
}

float AxisDelta(int32_t axis, const HidGestureSession& session, float x,
                float y) {
    switch (static_cast<DismissAxis>(axis)) {
        case DismissAxis::Left:
            return session.startX - x;
        case DismissAxis::Right:
            return x - session.startX;
        case DismissAxis::Top:
            return session.startY - y;
        case DismissAxis::Bottom:
            return y - session.startY;
    }
    return session.startX - x;
}

float PerpDelta(int32_t axis, const HidGestureSession& session, float x,
                float y) {
    if (IsHorizontalAxis(axis)) {
        return std::fabs(y - session.startY);
    }
    return std::fabs(x - session.startX);
}

bool PointInStartWindow(float x, float y) {
    const int64_t left = g_winRectDip[0].load(std::memory_order_relaxed);
    const int64_t top = g_winRectDip[1].load(std::memory_order_relaxed);
    const int64_t right = g_winRectDip[2].load(std::memory_order_relaxed);
    const int64_t bottom = g_winRectDip[3].load(std::memory_order_relaxed);

    if (right <= left || bottom <= top) {
        return true;
    }

    return x >= static_cast<float>(left) - kWindowMarginDip &&
           x <= static_cast<float>(right) + kWindowMarginDip &&
           y >= static_cast<float>(top) - kWindowMarginDip &&
           y <= static_cast<float>(bottom) + kWindowMarginDip;
}

bool RevealBandDip(const GestureGeometry& geometry, float& low, float& high) {
    const int32_t axis = geometry.axis();
    if (geometry.band) {
        low = std::min(geometry.bandLow, geometry.bandHigh);
        high = std::max(geometry.bandLow, geometry.bandHigh);
        return high > low;
    }
    switch (static_cast<DismissAxis>(axis)) {
        case DismissAxis::Left:
            low = 0.0f;
            high = kEdgeBandDip;
            return true;
        case DismissAxis::Top:
            low = 0.0f;
            high = kEdgeBandDip;
            return true;
        case DismissAxis::Right:
            if (!(g_screenDipWidth > kEdgeBandDip)) {
                return false;
            }
            low = g_screenDipWidth - kEdgeBandDip;
            high = g_screenDipWidth;
            return true;
        case DismissAxis::Bottom:
            if (!(g_screenDipHeight > kEdgeBandDip)) {
                return false;
            }
            low = g_screenDipHeight - kEdgeBandDip;
            high = g_screenDipHeight;
            return true;
    }
    return false;
}

float AxisScreenPosition(int32_t axis, float x, float y) {
    return IsHorizontalAxis(axis) ? x : y;
}

float AxisDirection(int32_t axis) {
    switch (static_cast<DismissAxis>(axis)) {
        case DismissAxis::Left:
        case DismissAxis::Top:
            return -1.0f;
        case DismissAxis::Right:
        case DismissAxis::Bottom:
            return 1.0f;
    }
    return -1.0f;
}

float AxisOpeningSign(int32_t axis) {
    return -AxisDirection(axis);
}

float RevealTravelDip(int32_t axis, float downX, float downY, float x, float y) {
    return std::max(
        0.0f,
        (AxisScreenPosition(axis, x, y) -
         AxisScreenPosition(axis, downX, downY)) *
            AxisOpeningSign(axis));
}

float TrailEdge(int32_t axis, float left, float top, float right, float bottom) {
    switch (static_cast<DismissAxis>(axis)) {
        case DismissAxis::Left:
            return right;
        case DismissAxis::Top:
            return bottom;
        case DismissAxis::Right:
            return left;
        case DismissAxis::Bottom:
            return top;
    }
    return right;
}

float GeometryTrailDip(const GestureGeometry& geometry) {
    return geometry.panel
               ? TrailEdge(geometry.axis(), geometry.left, geometry.top,
                           geometry.right, geometry.bottom)
               : 0.0f;
}

float FallbackTrailDip(int32_t axis, float distance) {
    switch (static_cast<DismissAxis>(axis)) {
        case DismissAxis::Left:
        case DismissAxis::Top:
            return distance;
        case DismissAxis::Right:
        case DismissAxis::Bottom:
            return 0.0f;
    }
    return distance;
}

float RevealEntryProgress(const GestureGeometry& geometry) {
    const float distance = geometry.distance();
    if (!(distance > 0.0f)) {
        return 0.0f;
    }
    const int32_t axis = geometry.axis();
    const float direction = AxisDirection(axis);
    const float trail = geometry.panel ? GeometryTrailDip(geometry)
                                       : FallbackTrailDip(axis, distance);
    const bool horizontal = IsHorizontalAxis(axis);
    const float edge =
        direction > 0.0f
            ? (horizontal ? geometry.layout.workRight : geometry.layout.workBottom)
            : (horizontal ? geometry.layout.workLeft : geometry.layout.workTop);
    return std::clamp(1.0f - direction * (edge - trail) / distance, 0.0f, 1.0f);
}

bool PanelContainsPress(const GestureGeometry& geometry, float x, float y) {
    if (!geometry.panel) {
        return true;
    }
    return x >= geometry.left - kPanelBandSlopDip &&
           x <= geometry.right + kPanelBandSlopDip &&
           y >= geometry.top - kPanelBandSlopDip &&
           y <= geometry.bottom + kPanelBandSlopDip;
}

bool PanelCrossContains(const GestureGeometry& geometry, float x, float y) {
    if (!geometry.panel) {
        return true;
    }
    const bool horizontal = IsHorizontalAxis(geometry.axis());

    const float position = horizontal ? y : x;
    const float low = horizontal ? geometry.top : geometry.left;
    const float high = horizontal ? geometry.bottom : geometry.right;
    return position >= low - kPanelBandSlopDip &&
           position <= high + kPanelBandSlopDip;
}

GestureGeometry ReadGestureGeometry(bool waitForWriter) {
    if (!waitForWriter) {
        if (TryAcquireSRWLockShared(&g_geometryLock)) {
            g_inputGeometry = g_geometry;
            ReleaseSRWLockShared(&g_geometryLock);
            return g_inputGeometry;
        }
        return g_inputGeometry;
    }

    AcquireSRWLockShared(&g_geometryLock);
    g_inputGeometry = g_geometry;
    ReleaseSRWLockShared(&g_geometryLock);
    return g_inputGeometry;
}

bool GeometryLayoutCurrent(const GestureGeometry& geometry) {
    return geometry.layoutVersion ==
           g_layoutVersion.load(std::memory_order_acquire);
}

bool VerticalDismissAllowed(const GestureGeometry& geometry, int32_t axis,
                            float x, float y, uint64_t tickMs) {
    if (IsHorizontalAxis(axis)) {
        return true;
    }

    ScrollerCache scroller{};
    {
        std::lock_guard lock(g_scrollerMutex);
        scroller = g_scroller;
    }

    if (scroller.generation !=
        g_openGeneration.load(std::memory_order_acquire)) {
        scroller = ScrollerCache{};
    }

    if (scroller.layoutVersion !=
        g_layoutVersion.load(std::memory_order_acquire)) {
        scroller = ScrollerCache{};
    }

    if (scroller.state == ScrollerState::Nothing) {
        return true;
    }
    if (scroller.state == ScrollerState::Unknown) {

        return !PanelContainsPress(geometry, x, y);
    }

    const ScrollerRegion* hit = nullptr;
    for (int i = 0; i < scroller.count; ++i) {
        const ScrollerRegion& region = scroller.regions[i];
        if (x < region.left || x > region.right || y < region.top ||
            y > region.bottom) {
            continue;
        }
        if (!hit || region.depth > hit->depth ||
            (region.depth == hit->depth &&
             (region.right - region.left) * (region.bottom - region.top) <
                 (hit->right - hit->left) * (hit->bottom - hit->top))) {
            hit = &region;
        }
    }
    if (!hit) {
        return true;
    }

    const bool atEnd =
        axis == static_cast<int32_t>(DismissAxis::Bottom)
            ? hit->verticalOffset <= kScrollerEndSlopDip
            : hit->verticalOffset >=
                  hit->scrollableHeight - kScrollerEndSlopDip;
    if (atEnd) {
        return true;
    }

    if (tickMs >= g_scrollerRejectLogTick &&
        tickMs - g_scrollerRejectLogTick >= kDismissRejectLogMs) {
        g_scrollerRejectLogTick = tickMs;
        TraceLog(L"dismiss deferred to scroll vo=%.0f sh=%.0f region=%s",
                hit->verticalOffset, hit->scrollableHeight, hit->label);
    }
    return false;
}

bool PublishOpenClaim(const GestureGeometry& geometry, bool touch,
                      float& entryOut) {
    if (!BridgeCapabilityNow()) {

        return false;
    }
    const float entry = std::clamp(RevealEntryProgress(geometry), 0.0f, 1.0f);
    InsertProgress(entry);
    g_intentOpenActive.store(true, std::memory_order_relaxed);
    g_intentOpenEntry.store(entry, std::memory_order_relaxed);
    SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionOpen);
    NoteOpenClaimTick();
    SetControllerTouch(touch);
    entryOut = entry;
    if (!RequestBridgeShow(1, 0)) {
        ReleaseExecutorOwnership();
        return false;
    }
    return true;
}

void StartRevealFollow(float downX, float downY, uint64_t downTick,
                       uint64_t tickMs, bool granted = false,
                       bool downInBand = false) {
    g_reveal = HidRevealSession{};
    g_reveal.edgeCandidate = true;
    g_reveal.granted = granted;
    g_reveal.grantDownInBand = downInBand;
    g_reveal.edgeStartX = downX;
    g_reveal.edgeStartY = downY;
    g_reveal.startTick = downTick;
    g_reveal.lastTick = tickMs;
    g_reveal.lastSampleQpc = QpcMsNow();
    g_reveal.lastAliveQpc = g_reveal.lastSampleQpc;
    BeginContactTiming();

    ResetEndVelocity();
}

void ConsumeGrabContact() {
    g_hidSession.suppress = true;
    g_reveal = HidRevealSession{};
    EndContactTiming();
    PublishPanelOwnership();
}

bool CancelCloseForOpenRequest() {

    const DismissBeforeCancel dismissed = ReadDismissBeforeCancel();
    const LONG64 cancelled = CancelPendingDismiss(L"open-intent");

    SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionOpen);

    ClearLocalCloseTap();
    EndClose(L"open-cancel");
    g_commitNeedsProgressReset = false;
    g_intentOpenActive.store(false, std::memory_order_relaxed);
    ForgetPendingDismiss();

    ArmCloseCancelTicket(cancelled, dismissed);
    return StartWindowIsOpen();
}

float SettlePressOpen() {
    const float live = TakeProgressNow(L"intent-open-cancel");
    std::chrono::milliseconds duration{0};
    if (!BeginGestureSettle(live, 1.0f, 0.0f, duration)) {
        ShowAtRest();
    }
    return live;
}

enum class GrabTransfer {
    Taken,
    Open,
    Refused,
};

struct GrabTransferResult {
    GrabTransfer outcome = GrabTransfer::Refused;
    const wchar_t* reason = nullptr;

    bool windowOpen = false;
    float entry = 0.0f;
};

GrabTransferResult BandPullOpen(const GestureGeometry& geometry,
                                const wchar_t* reason, bool closeInFlight) {
    GrabTransferResult open;
    open.outcome = GrabTransfer::Open;
    open.reason = reason;
    open.entry = std::clamp(RevealEntryProgress(geometry), 0.0f, 1.0f);
    if (closeInFlight) {
        open.windowOpen = CancelCloseForOpenRequest();
    } else {
        open.windowOpen = StartWindowIsOpen();

        SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionOpen);
    }
    if (!open.windowOpen &&
        !g_intentOpenActive.load(std::memory_order_relaxed)) {

        PublishOpenClaim(geometry, true, open.entry);
    }
    return open;
}

GrabTransferResult CancelCloseForQualifiedPull(const GestureGeometry& geometry,
                                               float x, float y,
                                               uint64_t tickMs) {
    if (g_pendingDismissConfirm != 0) {
        return {GrabTransfer::Refused, L"dismiss-entered"};
    }
    const float distance = geometry.distance();
    if (!(distance > 0.0f)) {
        return {GrabTransfer::Refused, L"geometry"};
    }

    if (!CloseInFlight()) {
        if (!g_reveal.grantDownInBand) {
            return {GrabTransfer::Refused, L"no-close"};
        }
        return BandPullOpen(geometry, L"no-close", false);
    }

    const float entry = RevealEntryProgress(geometry);
    if ((PeekProgressNow() - entry) * distance <= kProgressEpsilonDip) {
        if (!g_reveal.grantDownInBand) {
            return {GrabTransfer::Refused, L"offsurface"};
        }

        return BandPullOpen(geometry, L"offsurface", true);
    }
    const DismissBeforeCancel dismissed = ReadDismissBeforeCancel();

    if (dismissed.executeAt != 0 && tickMs >= dismissed.executeAt) {
        return {GrabTransfer::Refused, L"dispatch"};
    }
    const CloseScheduleCancel dropped = CancelCloseSchedules(L"pull");
    if (!dropped.slot && !dropped.command) {
        return {GrabTransfer::Refused, L"no-schedule"};
    }

    EndClose(L"pull-cancel");
    g_closeCancelledTick = tickMs;

    SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionOpen);
    ArmCloseCancelTicket(dropped.commandSeq, dismissed);

    g_commitNeedsProgressReset = false;
    g_intentOpenActive.store(false, std::memory_order_relaxed);

    const float live = TakeProgressNow(L"pull");

    float panelTrail = 0.0f;
    const float mapped = RevealProgressDip(geometry, distance, x, y, panelTrail);
    g_reveal.liveTakeover = true;
    g_reveal.liveBound = true;
    g_reveal.liveStart = live;
    g_reveal.liveOffset = mapped - live;
    return {GrabTransfer::Taken};
}

void TryClaimRevealOpen(const GestureGeometry& geometry, float x, float y,
                        uint64_t tickMs) {
    HidRevealSession& reveal = g_reveal;
    if (!reveal.edgeCandidate || reveal.active || reveal.claimAttempted) {
        return;
    }
    const float travel = RevealTravelDip(geometry.axis(), reveal.edgeStartX,
                                         reveal.edgeStartY, x, y);
    if (!(travel >= kAdoptTravelDip)) {
        return;
    }
    reveal.claimAttempted = true;
    if (reveal.granted) {

        const GrabTransferResult transfer =
            CancelCloseForQualifiedPull(geometry, x, y, tickMs);
        if (transfer.outcome == GrabTransfer::Taken) {
            reveal.granted = false;

            g_pullSerial++;
            g_lateRelease = RevealLateRelease{};
            TraceLog(L"reveal pull qualified travel=%.0f axis=%d grab=taken",
                    travel, geometry.axis());
        } else if (transfer.outcome == GrabTransfer::Open) {

            reveal.granted = false;
            g_pullSerial++;
            g_lateRelease = RevealLateRelease{};
            TraceLog(L"reveal pull qualified travel=%.0f axis=%d grab=open "
                    L"window=%s entry=%.3f reason=%s",
                    travel, geometry.axis(),
                    transfer.windowOpen ? L"open" : L"hidden", transfer.entry,
                    transfer.reason);
        } else {
            ConsumeGrabContact();
            TraceLog(L"reveal pull qualified travel=%.0f axis=%d grab=refused "
                    L"reason=%s",
                    travel, geometry.axis(), transfer.reason);
        }
        return;
    }
    TraceLog(L"reveal pull qualified travel=%.0f axis=%d grab=none", travel,
            geometry.axis());

    g_pullSerial++;
    g_lateRelease = RevealLateRelease{};

    if (StartWindowIsOpen() ||
        g_intentOpenActive.load(std::memory_order_relaxed)) {
        return;
    }
    if (g_pendingTapTick != 0) {
        CancelCloseSchedules(L"pull");
    }
    float entry = 0.0f;
    if (PublishOpenClaim(geometry, true, entry)) {
        TraceLog(L"reveal show request entry=%.3f", entry);
    } else {

        TraceLog(L"reveal show request failed entry=%.3f reason=no-executor",
                entry);
    }
}

float RevealProgressDip(const GestureGeometry& geometry, float distance, float x,
                        float y, float& panelTrailOut) {
    const int32_t axis = geometry.axis();
    const float finger = AxisScreenPosition(axis, x, y);

    panelTrailOut = geometry.panel ? GeometryTrailDip(geometry)
                                   : FallbackTrailDip(axis, distance);
    if (!(distance > 0.0f)) {
        return 0.0f;
    }
    return std::clamp(
        1.0f - (finger - panelTrailOut) / (AxisDirection(axis) * distance), 0.0f,
        1.0f);
}

void SeedRevealEndVelocity(const GestureGeometry& geometry, float distance) {
    ResetEndVelocity();
    float ignoredTrail = 0.0f;
    for (int i = 0; i < g_contactOrigin.pointCount; ++i) {
        const HidContactOrigin::Point& point = g_contactOrigin.points[i];
        SampleEndVelocity(
            RevealProgressDip(geometry, distance, point.x, point.y,
                              ignoredTrail),
            point.qpc);
    }
}

void AbandonRevealActivation(HidRevealSession& reveal, const wchar_t* reason) {
    reveal.edgeCandidate = false;
    EndContactTiming();
    SetControllerTouch(false);
    g_intentOpenActive.store(false, std::memory_order_relaxed);
    TraceLog(L"reveal activation abandoned reason=%s", reason);
}

bool ActivateRevealFollow(const GestureGeometry& geometry, float x, float y,
                          uint64_t sampleQpc) {
    HidRevealSession& reveal = g_reveal;
    if (reveal.granted) {

        return false;
    }
    if (CloseInFlight()) {

        AbandonRevealActivation(reveal, L"close");
        return false;
    }
    const float distance = geometry.distance();
    if (!(distance > 0.0f)) {

        AbandonRevealActivation(reveal, L"geometry");
        return false;
    }

    reveal.active = true;
    reveal.activeSinceQpc = sampleQpc;
    reveal.distance = distance;
    reveal.progress = 0.0f;
    reveal.samples = 0;

    ResetCoastVelocity();

    g_lateRelease = RevealLateRelease{};

    float panelTrail = 0.0f;
    const float mapped =
        RevealProgressDip(geometry, distance, x, y, panelTrail);
    reveal.panelTrail = panelTrail;

    float visible = 0.0f;
    if (reveal.liveTakeover) {

        const bool bound = reveal.liveBound;
        reveal.liveBound = false;
        const float taken =
            bound ? reveal.liveStart : TakeProgressNow(L"activate");
        visible = std::clamp(taken, 0.0f, 1.0f);
        reveal.target = mapped - reveal.liveOffset;
        reveal.activationGap = mapped - taken;
        reveal.chaseQpc = sampleQpc;
    } else {
        const float live = TakeProgressNow(L"activate");
        const float entry = RevealEntryProgress(geometry);

        visible = live >= 1.0f ? entry : std::max(live, entry);
        reveal.target = mapped;
        reveal.activationGap = mapped - visible;

        reveal.chaseQpc = sampleQpc + kRevealEntryHoldMs;
    }
    reveal.visible = visible;

    g_panel.Begin(&visible);
    reveal.lastInserted = visible;
    reveal.progress = visible;

    SeedRevealEndVelocity(geometry, distance);

    TraceLog(L"reveal takeover from=%.3f offset=%.3f live=%d", visible,
            reveal.activationGap, reveal.liveTakeover ? 1 : 0);

    g_intentOpenActive.store(false, std::memory_order_relaxed);
    NoteOpenClaimTick();
    return true;
}

bool ClearLocalCloseTap() {
    if (g_pendingTapTick == 0) {
        return false;
    }
    const CloseKind dropped = g_pendingTapKind;
    g_pendingTapTick = 0;
    g_pendingTapKind = CloseKind::None;
    g_pendingTapPull = 0;
    g_pendingTapSerial = 0;
    TraceLog(L"close dismiss cancelled kind=%s", CloseKindName(dropped));
    return true;
}

CloseScheduleCancel CancelCloseSchedules(const wchar_t* reason) {
    CloseScheduleCancel dropped;
    dropped.commandSeq = CancelPendingDismiss(reason);
    dropped.command = dropped.commandSeq != 0;
    dropped.slot = ClearLocalCloseTap();
    return dropped;
}

bool BeginEdgeCandidate(const GestureGeometry& geometry, float x, float y,
                        uint64_t tickMs) {
    float low = 0.0f;
    float high = 0.0f;
    const bool haveBand = RevealBandDip(geometry, low, high);
    const int32_t axis = geometry.axis();
    const float position = AxisScreenPosition(axis, x, y);
    const bool inBand = haveBand && position >= low && position <= high;

    const bool closeInFlight = CloseInFlight();
    if (!BridgeCapabilityNow()) {

        if (inBand || closeInFlight) {
            TraceLog(L"reveal admit x=%.0f y=%.0f axis=%d band=%.0f,%.0f ok=0 "
                    L"reason=no-capability",
                    x, y, axis, haveBand ? low : -1.0f,
                    haveBand ? high : -1.0f);
        }
        return false;
    }

    const float progress = (inBand || closeInFlight) ? PeekProgressNow() : -1.0f;
    const bool restoreOwed = g_commitNeedsProgressReset;

    const wchar_t* reason = L"admitted";
    bool admitted = false;
    do {
        if (!inBand && !closeInFlight) {
            break;
        }
        if (!RendererReady()) {
            reason = L"renderer";
            break;
        }
        if (!GeometryLayoutCurrent(geometry)) {
            reason = L"layout";
            break;
        }
        if (!(geometry.distance() > 0.0f)) {
            reason = L"distance";
            break;
        }
        if (closeInFlight) {
            if (!inBand) {

                float downTrail = 0.0f;
                const float downProgress = RevealProgressDip(
                    geometry, geometry.distance(), x, y, downTrail);
                const float slack = kProgressEpsilonDip / geometry.distance();
                if (!PanelCrossContains(geometry, x, y) ||
                    downProgress > progress + slack) {
                    reason = L"offsurface";
                    break;
                }
            }
            StartRevealFollow(x, y, tickMs, tickMs, true, inBand);
            admitted = true;
            reason = L"grab";
            break;
        }
        if (StartWindowIsOpenThrottled(tickMs)) {

            const float distance = geometry.distance();
            const float live = PeekProgressNow();
            const bool atRest =
                distance > 0.0f &&
                (1.0f - live) * distance < kProgressEpsilonDip;
            if (atRest) {
                reason = L"open";
                break;
            }
            reason = g_closeCancelledTick == tickMs ? L"cancelled" : L"unhealed";
        }
        StartRevealFollow(x, y, tickMs, tickMs);
        admitted = true;
    } while (false);

    if (closeInFlight && !admitted) {
        g_hidSession.suppress = true;
    }

    if (inBand) {
        g_contactOrigin.author = HidDownAuthor::Band;
    } else if (closeInFlight && admitted) {
        g_contactOrigin.author = HidDownAuthor::Grab;
    }

    if (inBand || closeInFlight) {
        TraceLog(L"reveal admit x=%.0f y=%.0f axis=%d band=%.0f,%.0f ok=%d "
                L"reason=%s progress=%.3f restore=%d",
                x, y, axis, haveBand ? low : -1.0f, haveBand ? high : -1.0f,
                admitted ? 1 : 0, reason, progress, restoreOwed ? 1 : 0);
    }
    if (inBand) {

        NoteVisibilityTransition(kVisibilityWantOpen);
    }
    return admitted;
}

void DriveRevealFollow(const GestureGeometry& geometry, float x, float y,
                       uint64_t sampleQpc) {
    HidRevealSession& reveal = g_reveal;
    const int32_t axis = geometry.axis();

    reveal.travel = RevealTravelDip(axis, reveal.edgeStartX, reveal.edgeStartY,
                                    x, y);
    reveal.maxTravel = std::max(reveal.maxTravel, reveal.travel);

    float panelTrail = 0.0f;
    const float mapped =
        RevealProgressDip(geometry, reveal.distance, x, y, panelTrail);
    reveal.panelTrail = panelTrail;

    const float follow = mapped - reveal.liveOffset;
    reveal.target = follow;

    float coastValue = reveal.visible;
    const bool resumed =
        g_panel.coasting.load(std::memory_order_relaxed) && StopCoast(coastValue);
    if (resumed) {
        reveal.visible = std::clamp(coastValue, 0.0f, 1.0f);
    }

    if (sampleQpc > reveal.chaseQpc) {
        const uint64_t elapsed = sampleQpc - reveal.chaseQpc;

        const uint64_t stepMs =
            std::min<uint64_t>(elapsed, kRevealChaseMaxStepMs);
        const float stepLimit =
            kSettleSpeedDipPerMs * static_cast<float>(stepMs) / reveal.distance;
        const float step =
            std::clamp(follow - reveal.visible, -stepLimit, stepLimit);
        reveal.visible = std::clamp(reveal.visible + step, 0.0f, 1.0f);
        reveal.chaseQpc = sampleQpc;
    }

    const float progress = reveal.visible;
    reveal.progress = progress;
    reveal.samples++;
    SampleCoastVelocity(progress, sampleQpc);
    if ((resumed ||
         std::fabs(progress - reveal.lastInserted) * reveal.distance >=
             kProgressEpsilonDip) &&
        g_panel.Move(progress)) {
        reveal.lastInserted = progress;
    }

    SampleEndVelocity(mapped, sampleQpc);
}

void UpdateRevealSession(const GestureGeometry& geometry, float x, float y,
                         uint64_t tickMs) {
    HidRevealSession& reveal = g_reveal;
    const uint64_t sampleQpc = QpcMsNow();
    reveal.lastTick = tickMs;
    reveal.lastSampleQpc = sampleQpc;
    reveal.lastAliveQpc = sampleQpc;

    if (!reveal.active) {
        if (reveal.granted) {

            return;
        }
        if (!StartWindowIsOpenThrottled(tickMs)) {
            return;
        }

        if (!reveal.claimAttempted) {
            const float travel =
                RevealTravelDip(geometry.axis(), reveal.edgeStartX,
                                reveal.edgeStartY, x, y);
            if (!(travel >= kAdoptTravelDip)) {
                return;
            }
        }
        if (!ActivateRevealFollow(geometry, x, y, sampleQpc)) {
            return;
        }
    }

    DriveRevealFollow(geometry, x, y, sampleQpc);
}

bool AdoptRevealContact(const GestureGeometry& geometry, uint64_t tickMs) {
    if (!BridgeCapabilityNow()) {
        return false;
    }
    if (g_reveal.edgeCandidate || g_reveal.active || g_hidSession.active ||
        g_hidSession.suppress || !g_contactOrigin.valid ||
        g_contactOrigin.author == HidDownAuthor::None ||
        g_contactOrigin.startedOpen || !RendererReady() ||
        !GeometryLayoutCurrent(geometry)) {
        return false;
    }

    if (CloseInFlight()) {
        return false;
    }

    const int32_t axis = geometry.axis();
    const float travel =
        RevealTravelDip(axis, g_contactOrigin.downX, g_contactOrigin.downY,
                        g_contactOrigin.x, g_contactOrigin.y);
    if (!(travel >= kAdoptTravelDip)) {
        return false;
    }

    float low = 0.0f;
    float high = 0.0f;
    const bool haveBand = RevealBandDip(geometry, low, high);
    StartRevealFollow(g_contactOrigin.downX, g_contactOrigin.downY,
                      g_contactOrigin.downTick, tickMs);
    const uint64_t sampleQpc = QpcMsNow();
    if (!ActivateRevealFollow(geometry, g_contactOrigin.x, g_contactOrigin.y,
                              sampleQpc)) {
        return false;
    }
    DriveRevealFollow(geometry, g_contactOrigin.x, g_contactOrigin.y,
                      sampleQpc);
    TraceLog(L"reveal adopt x=%.0f y=%.0f travel=%.0f axis=%d band=%.0f,%.0f gen=%u",
            g_contactOrigin.x, g_contactOrigin.y, travel, axis,
            haveBand ? low : -1.0f, haveBand ? high : -1.0f,
            geometry.generation);
    return true;
}

void PromoteRevealContact(const GestureGeometry& geometry, uint64_t tickMs) {
    if (g_reveal.active) {
        return;
    }
    if (g_reveal.granted) {

        return;
    }

    if (g_reveal.edgeCandidate) {

        const bool moved =
            g_contactOrigin.valid &&
            RevealTravelDip(geometry.axis(), g_reveal.edgeStartX,
                            g_reveal.edgeStartY, g_contactOrigin.x,
                            g_contactOrigin.y) >= kAdoptTravelDip;
        if (!moved) {
            return;
        }
        const uint64_t sampleQpc = QpcMsNow();
        if (ActivateRevealFollow(geometry, g_contactOrigin.x,
                                 g_contactOrigin.y, sampleQpc)) {
            DriveRevealFollow(geometry, g_contactOrigin.x, g_contactOrigin.y,
                              sampleQpc);
        }
        return;
    }

    AdoptRevealContact(geometry, tickMs);
}

bool ConsumeLateRevealRelease(const GestureGeometry& geometry) {
    RevealLateRelease& record = g_lateRelease;
    if (!record.valid) {
        return false;
    }

    const uint64_t qpcNow = QpcMsNow();
    const uint64_t age =
        qpcNow > record.releaseQpc ? qpcNow - record.releaseQpc : 0;
    if (age > kLateRevealWindowMs || record.pullSerial != g_pullSerial) {
        record = RevealLateRelease{};
        return false;
    }

    const float distance = geometry.distance();
    if (!(distance > 0.0f)) {
        record = RevealLateRelease{};
        return false;
    }

    record.valid = false;

    const float from = TakeVisibleProgressNow(L"late");
    std::chrono::milliseconds duration{0};
    if (!BeginGestureSettle(from, 1.0f, record.velocity, duration)) {
        return false;
    }

    g_commitNeedsProgressReset = false;

    SetControllerTouch(false);
    g_intentOpenActive.store(false, std::memory_order_relaxed);
    TraceLog(L"reveal adopt late travel=%.0f vel=%.2f age=%llu gen=%u from=%.3f "
            L"pos=%.3f",
            record.travel, SpeedTowardDip(record.velocity, from, 1.0f, distance),
            static_cast<unsigned long long>(age),
            g_openGeneration.load(std::memory_order_relaxed), from,
            record.position);
    return true;
}

bool BandTailReveal(const GestureGeometry& geometry, float x, float y) {
    if (!g_contactOrigin.valid || g_reveal.edgeCandidate) {
        return false;
    }
    float low = 0.0f;
    float high = 0.0f;
    if (!RevealBandDip(geometry, low, high)) {
        return false;
    }
    const int32_t axis = geometry.axis();
    const float down = AxisScreenPosition(axis, g_contactOrigin.downX,
                                          g_contactOrigin.downY);
    if (down < low || down > high) {
        return false;
    }
    return RevealTravelDip(axis, g_contactOrigin.downX, g_contactOrigin.downY,
                           x, y) >= kBandTailTravelDip;
}

void ReleaseRevealSession(const GestureGeometry& geometry) {
    HidRevealSession& reveal = g_reveal;
    if (!reveal.edgeCandidate) {
        return;
    }

    const bool grab = reveal.granted;
    reveal.edgeCandidate = false;
    reveal.granted = false;

    SetControllerTouch(false);
    if (!reveal.active) {
        if (grab) {

            EndContactTiming();
            return;
        }

        const uint64_t releaseQpc = QpcMsNow();
        const float distance = geometry.distance();
        const float travel =
            g_contactOrigin.valid
                ? RevealTravelDip(geometry.axis(), reveal.edgeStartX,
                                  reveal.edgeStartY, g_contactOrigin.x,
                                  g_contactOrigin.y)
                : 0.0f;
        if (travel >= kAdoptTravelDip && distance > 0.0f &&
            g_contactOrigin.valid) {

            SeedRevealEndVelocity(geometry, distance);
            const float velocity = EndVelocityProgressPerMs(releaseQpc);
            float panelTrail = 0.0f;
            const float position = RevealProgressDip(
                geometry, distance, g_contactOrigin.x, g_contactOrigin.y,
                panelTrail);
            g_lateRelease = RevealLateRelease{};
            g_lateRelease.valid = true;
            g_lateRelease.releaseQpc = releaseQpc;
            g_lateRelease.travel = travel;
            g_lateRelease.position = position;
            g_lateRelease.velocity = velocity;
            g_lateRelease.pullSerial = g_pullSerial;
            TraceLog(L"reveal candidate released unopened travel=%.0f axis=%d "
                    L"vel=%.2f pos=%.2f",
                    travel, geometry.axis(),
                    SpeedTowardDip(velocity, position, 1.0f, distance), position);
        } else if (travel >= kAdoptTravelDip) {

            TraceLog(L"reveal candidate released unopened travel=%.0f axis=%d",
                    travel, geometry.axis());
        }
        EndContactTiming();
        return;
    }
    reveal.active = false;

    const uint64_t elapsed = reveal.lastTick > reveal.startTick
                                 ? reveal.lastTick - reveal.startTick
                                 : 0;
    const float travel = std::max(0.0f, reveal.travel);
    const float distance = reveal.distance;
    const float progress = std::clamp(reveal.progress, 0.0f, 1.0f);

    const bool liveHeld =
        reveal.liveTakeover && progress >= reveal.liveStart;
    const bool commit =
        ReleaseCommits(true, travel, elapsed, distance) ||
        liveHeld;

    const uint64_t releaseQpc = QpcMsNow();
    EndContactTiming();

    const float from = TakeProgressNow(L"reveal-release");
    const float behind =
        std::clamp(reveal.target - progress, 0.0f, 1.0f);

    const float endVelocity = EndVelocityProgressPerMs(releaseQpc);
    const float releaseSpeed = SpeedTowardDip(endVelocity, from,
                                              commit ? 1.0f : 0.0f, distance);

    if (commit) {

        const bool started = BeginGestureOpen(from, endVelocity);
        TraceLog(L"open follow travel=%.0f max=%.0f progress=%.2f behind=%.3f "
                L"offset=%.3f vel=%.2f commit=%d live=%d panel=%.0f n=%d",
                travel, reveal.maxTravel, progress, behind,
                reveal.activationGap, releaseSpeed, commit ? 1 : 0,
                reveal.liveTakeover ? 1 : 0, reveal.panelTrail, reveal.samples);
        if (!started) {
            ShowAtRest();
        }
        return;
    }

    const GestureClose close =
        BeginGestureClose(from, endVelocity, CloseKind::RevealCancel);
    TraceLog(L"open follow travel=%.0f max=%.0f progress=%.2f behind=%.3f "
            L"offset=%.3f vel=%.2f commit=%d live=%d panel=%.0f n=%d",
            travel, reveal.maxTravel, progress, behind, reveal.activationGap,
            releaseSpeed, commit ? 1 : 0, reveal.liveTakeover ? 1 : 0,
            reveal.panelTrail, reveal.samples);
    if (close.started) {
        g_commitNeedsProgressReset = true;
        return;
    }

    AbandonClose(CloseKind::RevealCancel);
}

void LogHidRelease(const HidGestureSession& session, float velocity,
                   bool commit, uint64_t releaseQpc, const wchar_t* source) {
    const uint64_t releaseDelay = releaseQpc > session.lastSampleQpc
                                      ? releaseQpc - session.lastSampleQpc
                                      : 0;
    TraceLog(
        L"hid release src=%s delta=%.1f final=%.1f perp=%.1f vel=%.2f "
        L"commit=%d n=%d relms=%llu",
        source, session.maxDelta, session.curDelta, session.maxPerp, velocity,
        commit ? 1 : 0, session.samples,
        static_cast<unsigned long long>(releaseDelay));
}

void BeginHidSession(const GestureGeometry& geometry, float x, float y,
                     uint64_t tickMs) {

    if (!BridgeCapabilityNow()) {
        if (tickMs > g_skipLogTick + 1000) {
            g_skipLogTick = tickMs;
            TraceLog(L"dismiss skipped no-capability");
        }
        return;
    }

    if (!ClaimPanelForAdmission()) {
        return;
    }

    if (g_dismissSettling.load(std::memory_order_relaxed) || CloseInFlight()) {
        if (tickMs > g_skipLogTick + 1000) {
            g_skipLogTick = tickMs;
            TraceLog(L"dismiss skipped settling");
        }
        return;
    }

    const float distance = geometry.distance();
    if (!(distance > 0.0f)) {
        if (tickMs > g_skipLogTick + 1000) {
            g_skipLogTick = tickMs;
            TraceLog(L"dismiss skipped distance=0");
        }
        return;
    }

    const bool rendererReady = RendererReady();
    if (!rendererReady || !GeometryLayoutCurrent(geometry) ||
        !StartWindowIsOpenThrottled(tickMs)) {
        if (tickMs > g_skipLogTick + 1000) {
            g_skipLogTick = tickMs;
            TraceLog(L"dismiss skipped closed root=%d",
                    rendererReady ? 1 : 0);
        }
        return;
    }

    const int32_t axis = geometry.axis();

    if (!PanelContainsPress(geometry, x, y)) {
        if (tickMs > g_skipLogTick + 1000) {
            g_skipLogTick = tickMs;
            TraceLog(L"dismiss skipped outside panel axis=%d x=%.0f y=%.0f "
                    L"rect=%.0f,%.0f,%.0f,%.0f trail=%.0f gen=%u",
                    axis, x, y, geometry.left, geometry.top, geometry.right,
                    geometry.bottom, GeometryTrailDip(geometry),
                    geometry.generation);
        }
        return;
    }

    if (!VerticalDismissAllowed(geometry, axis, x, y, tickMs)) {
        if (tickMs > g_skipLogTick + 1000) {
            g_skipLogTick = tickMs;
            TraceLog(L"dismiss skipped vertical axis=%d x=%.0f y=%.0f", axis,
                    x, y);
        }
        return;
    }

    HidGestureSession& session = g_hidSession;
    session = HidGestureSession{};
    session.active = true;
    session.startX = x;
    session.startY = y;
    session.startTick = tickMs;
    session.lastTick = tickMs;
    session.lastSampleQpc = QpcMsNow();
    session.lastAliveQpc = session.lastSampleQpc;
    session.samples = 1;
    BeginContactTiming();

    ResetCoastVelocity();
    ResetEndVelocity();

    session.startProgress = TakeProgressNow(L"hid-begin");
    if (g_commitNeedsProgressReset && !CloseInFlight()) {
        g_commitNeedsProgressReset = false;
        session.startProgress = kProgressRest;
    }

    g_panel.Begin(&session.startProgress);
    session.lastProgress = session.startProgress;

    TraceLog(L"hid down x=%.1f y=%.1f axis=%d dist=%.0f", x, y, axis,
            distance);
}

void ReleaseHidSession(const GestureGeometry& geometry, bool allowCommit,
                       const wchar_t* source) {
    HidGestureSession& session = g_hidSession;
    if (!session.active) {
        return;
    }

    const uint64_t lastSample = session.lastTick;
    const uint64_t elapsed =
        lastSample > session.startTick ? lastSample - session.startTick : 0;

    const float finalDelta = std::max(0.0f, session.curDelta);
    const float distance = geometry.distance();
    const bool commit =
        ReleaseCommits(allowCommit, finalDelta, elapsed, distance);

    const uint64_t releaseQpc = QpcMsNow();
    EndContactTiming();

    const float from = TakeProgressNow(L"hid-release");

    const float endVelocity = EndVelocityProgressPerMs(releaseQpc);
    const float releaseSpeed = SpeedTowardDip(
        endVelocity, from, commit ? 0.0f : 1.0f, distance);

    if (commit) {
        const DismissCloseResult close = BeginDismissSettle(from, endVelocity);

        LogHidRelease(session, releaseSpeed, commit, releaseQpc, source);
        if (close.start != DismissClose::NotStarted) {
            g_commitNeedsProgressReset = true;
        } else {

            AbandonClose(CloseKind::Dismiss);
        }
        session.active = false;
        return;
    }

    session.active = false;

    if (g_dismissSettling.load(std::memory_order_relaxed) || CloseInFlight()) {
        TraceLog(L"cancel animation skipped (close in flight)");
        return;
    }
    if (distance > 0.0f) {
        BeginGestureOpen(from, endVelocity);
    } else {
        ShowAtRest();
    }
    LogHidRelease(session, releaseSpeed, commit, releaseQpc, source);
}

void AbortHidSession(const GestureGeometry& geometry) {
    HidGestureSession& session = g_hidSession;
    if (!session.active) {
        return;
    }

    const float distance = std::max(geometry.distance(), 0.0f);
    const uint64_t releaseQpc = QpcMsNow();
    const float velocity =
        std::fabs(EndVelocityProgressPerMs(releaseQpc)) * distance;

    LogHidRelease(session, velocity, false, releaseQpc, L"abort");
    session.active = false;
    EndContactTiming();
    ShowAtRest();
}

void UpdateHidSession(const GestureGeometry& geometry, float x, float y,
                      uint64_t tickMs) {
    HidGestureSession& session = g_hidSession;

    const float distance = geometry.distance();
    if (!(distance > 0.0f)) {
        AbortHidSession(geometry);
        return;
    }

    float coastValue = session.lastProgress;
    const bool resumed =
        g_panel.coasting.load(std::memory_order_relaxed) && StopCoast(coastValue);

    const int32_t axis = geometry.axis();
    const float delta = AxisDelta(axis, session, x, y);
    const float perpendicular = PerpDelta(axis, session, x, y);

    session.lastTick = tickMs;
    const uint64_t sampleQpc = QpcMsNow();
    session.lastSampleQpc = sampleQpc;
    session.lastAliveQpc = sampleQpc;
    session.curDelta = delta;
    session.maxDelta = std::max(session.maxDelta, delta);
    session.maxPerp = std::max(session.maxPerp, perpendicular);
    session.samples++;

    if (session.maxPerp > kScrollAbortPerp &&
        session.maxPerp > kScrollAbortRatio * session.maxDelta &&
        session.maxDelta < kScrollAbortMaxDelta) {
        TraceLog(L"dismiss aborted perpendicular perp=%.0f delta=%.0f",
                session.maxPerp, session.maxDelta);
        AbortHidSession(geometry);
        return;
    }

    const float progress = std::clamp(
        session.startProgress - delta / distance, 0.0f, 1.0f);
    SampleCoastVelocity(progress, sampleQpc);

    if (resumed ||
        std::fabs(progress - session.lastProgress) * distance >=
            kProgressEpsilonDip) {
        g_panel.Move(progress);
        session.lastProgress = progress;
    }

    SampleEndVelocity(session.lastProgress, sampleQpc);
}

void OnHidSampleStep(const GestureGeometry& geometry, int contacts, float xDip,
                     float yDip, uint64_t tickMs) {
    HidGestureSession& session = g_hidSession;

    if (contacts <= 0) {
        if (session.active) {
            ReleaseHidSession(geometry, true, L"report");
        }
        session.suppress = false;

        ReleaseRevealSession(geometry);
        g_contactOrigin = HidContactOrigin{};
        g_contactDown = false;
        g_openCheckValid = false;
        return;
    }

    if (!g_contactOrigin.valid) {
        g_contactOrigin.valid = true;

        const bool closeInFlight = CloseInFlight();
        const bool windowOpen =
            !closeInFlight && StartWindowIsOpenThrottled(tickMs);

        const bool unhealed =
            windowOpen && geometry.distance() > 0.0f &&
            PeekProgressNow() < RevealEntryProgress(geometry);
        g_contactOrigin.startedOpen = windowOpen && !unhealed;
        g_contactOrigin.downX = xDip;
        g_contactOrigin.downY = yDip;
        g_contactOrigin.downTick = tickMs;
        g_contactOrigin.pointCount = 0;

    }
    g_contactOrigin.x = xDip;
    g_contactOrigin.y = yDip;
    SampleOriginPoint(xDip, yDip, QpcMsNow());

    const bool newContact = !g_contactDown;

    if (session.suppress) {

        g_contactDown = true;
        g_contactTick = tickMs;
        return;
    }

    if (session.active) {
        UpdateHidSession(geometry, xDip, yDip, tickMs);
        return;
    }

    if (newContact && !ClaimPanelForAdmission()) {

        return;
    }
    if (newContact && !GeometryLayoutCurrent(geometry)) {

        return;
    }
    g_contactDown = true;
    g_contactTick = tickMs;
    if (newContact) {
        BeginEdgeCandidate(geometry, xDip, yDip, tickMs);
    }
    if (g_reveal.edgeCandidate) {

        TryClaimRevealOpen(geometry, xDip, yDip, tickMs);
        if (g_reveal.edgeCandidate) {
            UpdateRevealSession(geometry, xDip, yDip, tickMs);
        }
        return;
    }

    if (StartWindowIsOpenThrottled(tickMs) && AdoptRevealContact(geometry, tickMs)) {
        UpdateRevealSession(geometry, xDip, yDip, tickMs);
        return;
    }

    if (BandTailReveal(geometry, xDip, yDip)) {

        if (tickMs >= g_bandTailLogTick &&
            tickMs - g_bandTailLogTick >= kDismissRejectLogMs) {
            g_bandTailLogTick = tickMs;
            TraceLog(L"dismiss skipped band tail travel=%.0f",
                    RevealTravelDip(geometry.axis(), g_contactOrigin.downX,
                                    g_contactOrigin.downY, xDip, yDip));
        }
        return;
    }

    if (!PointInStartWindow(xDip, yDip)) {
        return;
    }
    BeginHidSession(geometry, xDip, yDip, tickMs);
}

void PublishPanelOwnership() {
    const bool owned = g_hidSession.active || g_reveal.edgeCandidate;

    g_panelOwned.store(owned, std::memory_order_seq_cst);
    if (!owned && g_measureDeferred.exchange(false, std::memory_order_relaxed) &&
        !g_unloading.load(std::memory_order_relaxed)) {

        const SettledMeasureRequest request = StartMeasureRequest(true);
        PostGeometryMeasurement(request, true);
    }
}

void OnHidSample(int contacts, float xDip, float yDip, uint64_t tickMs) {
    GestureGeometry geometry = ReadGestureGeometry();

    const bool newContact = !g_contactDown;
    if (newContact && !GeometryLayoutCurrent(geometry)) {
        geometry = ReadGestureGeometry(true);
    }
    OnHidSampleStep(geometry, contacts, xDip, yDip, tickMs);
    PublishPanelOwnership();
}

void MaybeCoast(const GestureGeometry& geometry, uint64_t qpcNow) {
    if (g_panel.coasting.load(std::memory_order_relaxed)) {

        if (qpcNow >= g_panel.coastStartQpc &&
            qpcNow - g_panel.coastStartQpc >=
                static_cast<uint64_t>(kCoastDuration.count())) {
            g_panel.Move(g_panel.coastTo);
        }
        return;
    }

    if (CloseInFlight()) {
        return;
    }

    const bool revealActive = g_reveal.active;
    if (!g_hidSession.active && !revealActive) {
        return;
    }

    const float distance = geometry.distance();
    if (!(distance > 0.0f)) {
        return;
    }

    if (revealActive &&
        (qpcNow < g_reveal.activeSinceQpc ||
         qpcNow - g_reveal.activeSinceQpc < kRevealCoastSettleMs)) {
        return;
    }

    const uint64_t lastSample =
        revealActive ? g_reveal.lastSampleQpc : g_hidSession.lastSampleQpc;
    if (qpcNow < lastSample || qpcNow - lastSample < kCoastStartMs) {
        return;
    }

    if (!StartWindowIsOpen()) {
        return;
    }

    const float velocity = CoastVelocity(distance);
    if (std::fabs(velocity) * distance < kCoastMinVelocityDip) {
        return;
    }

    const float from = std::clamp(
        revealActive ? g_reveal.lastInserted : g_hidSession.lastProgress,
        0.0f, 1.0f);
    const float to = std::clamp(
        from + velocity * static_cast<float>(kCoastDuration.count()), 0.0f,
        1.0f);
    if (to == from) {
        return;
    }

    StartCoast(from, to);
}

bool IdleReleaseHeld() {
    const bool touchShowInFlight = OpenClaimHoldActive();
    const bool holdThroughClose =
        CloseInFlight() && (g_reveal.edgeCandidate || g_reveal.active);
    return touchShowInFlight || holdThroughClose;
}

void CheckHidIdleRelease(uint64_t tickMs) {

    PublishPeerTick(tickMs);

    HidGestureSession& session = g_hidSession;
    const uint64_t qpcNow = QpcMsNow();

    const GestureGeometry geometry = ReadGestureGeometry();

    if (g_lateRelease.valid &&
        (qpcNow < g_lateRelease.releaseQpc ||
         qpcNow - g_lateRelease.releaseQpc > kLateRevealWindowMs)) {
        g_lateRelease = RevealLateRelease{};
    }

    MaybeCoast(geometry, qpcNow);

    SettledMeasureRequest due{};
    if (TakeMeasureDeadline(tickMs, due)) {
        PostGeometryMeasurement(due, true);
    }

    const uint64_t scrollerRetryAt =
        g_scrollerRetryAt.load(std::memory_order_relaxed);
    if (scrollerRetryAt != 0 && tickMs >= scrollerRetryAt &&
        g_scrollerRetryAt.exchange(0, std::memory_order_relaxed) ==
            scrollerRetryAt) {
        const uint32_t retryGeneration =
            g_scrollerRetryGeneration.load(std::memory_order_relaxed);
        if (retryGeneration != 0 &&
            retryGeneration ==
                g_openGeneration.load(std::memory_order_relaxed) &&
            StartWindowIsOpen()) {
            PostGeometryMeasurement(
                StartMeasureRequest(!g_hidSession.active &&
                                    !g_reveal.edgeCandidate),
                true);
        }
    }

    const bool held = IdleReleaseHeld();
    const bool sessionIdle =
        session.active && qpcNow >= session.lastAliveQpc &&
        qpcNow - session.lastAliveQpc >= kIdleReleaseMs;
    if (sessionIdle && !held) {
        ReleaseHidSession(geometry, true, L"idle");
    }

    const bool revealIdle =
        g_reveal.edgeCandidate && qpcNow >= g_reveal.lastAliveQpc &&
        qpcNow - g_reveal.lastAliveQpc >= kCandidateIdleHoldMs;
    if (revealIdle && !held) {

        const bool consumed = session.suppress;
        ReleaseRevealSession(geometry);
        if (!consumed) {

            g_contactDown = false;
            g_contactOrigin = HidContactOrigin{};
            g_openCheckValid = false;
            ForgetTrackedContact();
        }
    }

    const bool closeInFlight = CloseInFlight();
    const bool holdLatch =
        closeInFlight || (session.suppress && g_trackedContact.hasIdentifier);
    if (!session.active && !g_reveal.edgeCandidate && !holdLatch &&
        g_trackedContact.slot >= 0 &&
        tickMs > g_trackedContact.lastEvidenceTick &&
        tickMs - g_trackedContact.lastEvidenceTick >= kIdleReleaseMs) {
        if (!g_contactDown) {
            TraceLog(L"hid identity expired slot=%d latch=0",
                    g_trackedContact.slot);
        }
        ForgetTrackedContact();
    }

    if (!session.active && !g_reveal.edgeCandidate && !holdLatch &&
        g_contactDown && tickMs > g_contactTick &&
        tickMs - g_contactTick >= kIdleReleaseMs) {
        g_contactDown = false;
        g_contactOrigin = HidContactOrigin{};
        session.suppress = false;
        g_openCheckValid = false;
        ForgetTrackedContact();
    }

    PublishPanelOwnership();
}

void RunPendingCloseTap(uint64_t tickMs) {
    if (g_pendingTapTick == 0 || tickMs < g_pendingTapTick) {
        return;
    }

    const CloseKind kind = g_pendingTapKind;
    const uint32_t pull = g_pendingTapPull;

    if (g_pendingTapSerial != g_close.serial ||
        g_close.phase == ClosePhase::Done) {
        TraceLog(L"close dismiss skipped kind=%s reason=record-changed tap=%u "
                L"close=%u phase=%s",
                CloseKindName(kind), g_pendingTapSerial, g_close.serial,
                ClosePhaseName(g_close.phase));
        g_pendingTapTick = 0;
        g_pendingTapKind = CloseKind::None;
        g_pendingTapPull = 0;
        g_pendingTapSerial = 0;
        return;
    }

    if (g_pullSerial != pull || g_reveal.active ||
        (g_reveal.edgeCandidate && !g_reveal.granted)) {

        CancelCloseSchedules(L"superseded");
        EndClose(L"superseded");
        return;
    }

    g_pendingTapTick = 0;
    g_pendingTapKind = CloseKind::None;
    g_pendingTapPull = 0;
    g_pendingTapSerial = 0;

    // The close belongs to the menu it was accepted on. A keyboard reopen
    // between acceptance and this deadline is a different menu, and a dismiss
    // naming the old generation cannot be aimed at it.
    uint64_t openGeneration = 0;
    uint64_t openTick = 0;
    if (!SwipeIpcReadOpen(&g_ipc, &openGeneration, &openTick) ||
        openGeneration != g_close.openGeneration) {
        TraceLog(L"close dismiss skipped kind=%s reason=menu-changed gen=%llu "
                L"now=%llu",
                CloseKindName(kind),
                static_cast<unsigned long long>(g_close.openGeneration),
                static_cast<unsigned long long>(openGeneration));
        EndClose(L"menu-changed");
        ReleaseExecutorOwnership();
        return;
    }

    const CloakState cloak = ReadCloakState();
    if (cloak == CloakState::Hidden) {
        TraceLog(L"close dismiss skipped kind=%s (window hidden)",
                CloseKindName(kind));
        EndClose(L"window-closed");
        ReleaseExecutorOwnership();
        return;
    }
    if (cloak == CloakState::Unknown) {
        // An unknown sample is never evidence of a closed menu: a dismiss on a
        // hidden window is harmless, one on a visible window is the repair.
        TraceLog(L"close dismiss kind=%s cloak=unknown", CloseKindName(kind));
    }

    SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionClosing);

    if (!RequestBridgeDismiss(1, GetTickCount64(), openGeneration)) {

        EndClose(L"no-executor");
        ReleaseExecutorOwnership();
        TraceLog(L"close dismiss skipped kind=%s reason=no-executor",
                CloseKindName(kind));
        return;
    }
    NoteCloseDismissPublished();
    TraceLog(L"close dismiss kind=%s", CloseKindName(kind));
}

uint32_t AdvanceOpenGeneration() {
    std::lock_guard lock(g_measureMutex);
    uint32_t next = g_openGeneration.load(std::memory_order_relaxed) + 1;
    if (next == 0) {
        next = 1;
    }
    g_openGeneration.store(next, std::memory_order_relaxed);
    return next;
}

SettledMeasureRequest StartMeasureRequest(bool trailIdle) {
    std::lock_guard lock(g_measureMutex);
    if (++g_measureSerial == 0) {
        ++g_measureSerial;
    }
    g_measureRequest.id = g_measureSerial;
    g_measureRequest.generation =
        g_openGeneration.load(std::memory_order_relaxed);
    g_measureRequest.trailIdle = trailIdle;
    g_measureRequest.dueTick = GetTickCount64() + kSettledMeasureDeadlineMs;
    return g_measureRequest;
}

bool TakeMeasureDeadline(uint64_t tickMs, SettledMeasureRequest& request) {
    std::lock_guard lock(g_measureMutex);
    if (g_measureRequest.id == 0 || g_measureRequest.dueTick == 0 ||
        tickMs < g_measureRequest.dueTick) {
        return false;
    }
    g_measureRequest.dueTick = 0;
    request = g_measureRequest;
    return true;
}

uint64_t PeekMeasureDeadlineTick() {
    std::lock_guard lock(g_measureMutex);
    return g_measureRequest.id != 0 ? g_measureRequest.dueTick : 0;
}

bool MeasureRequestLiveLocked(uint32_t generation, uint64_t id) {
    return id != 0 && g_measureRequest.id == id &&
           g_measureRequest.generation == generation;
}

bool MeasureRequestLive(uint32_t generation, uint64_t id) {
    if (id == 0) {
        return false;
    }
    std::lock_guard lock(g_measureMutex);
    return MeasureRequestLiveLocked(generation, id);
}

bool EndMeasureRequest(uint32_t generation, uint64_t id) {
    if (id == 0) {
        return false;
    }
    std::lock_guard lock(g_measureMutex);
    if (g_measureRequest.id != id ||
        g_measureRequest.generation != generation) {
        return false;
    }
    g_measureRequest = SettledMeasureRequest{};
    return true;
}

void CancelMeasureRequest() {
    std::lock_guard lock(g_measureMutex);
    g_measureRequest = SettledMeasureRequest{};
}

SettledMeasureRequest SnapshotMeasureRequest() {
    std::lock_guard lock(g_measureMutex);
    return g_measureRequest;
}

void RunGeometryMeasurement(const SettledMeasureRequest& request, bool settled) {
    if (g_unloading.load(std::memory_order_relaxed)) {
        return;
    }
    if (!MeasureRequestLive(request.generation, request.id)) {
        return;
    }
    if (request.generation != g_openGeneration.load(std::memory_order_relaxed)) {

        EndMeasureRequest(request.generation, request.id);
        return;
    }
    if (!g_root) {
        EndMeasureRequest(request.generation, request.id);
        return;
    }
    bool haveRenderer = false;
    {
        std::lock_guard lock(g_compositionMutex);
        haveRenderer = g_state != nullptr;
    }
    if (!haveRenderer) {

        EndMeasureRequest(request.generation, request.id);
        return;
    }

    try {

        LayoutApplyGuard apply;
        if (!apply.Begin()) {
            g_measureDeferred.store(true, std::memory_order_relaxed);

            g_panelTrailLogged = false;

            TraceLog(L"gesture geometry deferred reason=panel-owned gen=%u",
                    request.generation);
            EndMeasureRequest(request.generation, request.id);
            return;
        }
        if (!RefreshGestureGeometry()) {

            EndMeasureRequest(request.generation, request.id);
            return;
        }
        if (!MeasureRequestLive(request.generation, request.id)) {
            return;
        }
        if (!settled) {

            g_panelTrailLogged = false;
        }

        const bool edgeIdle =
            request.trailIdle && !g_panelOwned.load(std::memory_order_relaxed);
        if (!PublishPanelTrailDip(edgeIdle, request.generation, request.id)) {
            EndMeasureRequest(request.generation, request.id);
            return;
        }
        if (!MeasureRequestLive(request.generation, request.id)) {
            return;
        }

        if (settled) {
            CacheScrollableElement(request.generation, request.id);
        }
    } catch (...) {

        EndMeasureRequest(request.generation, request.id);
        return;
    }

    if (settled) {
        EndMeasureRequest(request.generation, request.id);
        return;
    }

    ArmLayoutWatch(request);
}

void PostGeometryMeasurement(const SettledMeasureRequest& request, bool settled) {
    if (g_unloading.load(std::memory_order_relaxed)) {
        return;
    }
    wucore::CoreDispatcher dispatcher = CopyDispatcher();
    if (!dispatcher) {
        return;
    }
    try {
        PostCountedUiTurn(dispatcher, [request, settled]() {
            RunGeometryMeasurement(request, settled);
        });
    } catch (winrt::hresult_error const& e) {
        TraceLog(L"gesture geometry queue err=%08X",
                static_cast<unsigned>(e.code()));
    } catch (...) {
        TraceLog(L"gesture geometry queue err=unknown");
    }
}

void PostUiOpenWatchRelease(uint32_t closedGeneration,
                            const SettledMeasureRequest& closing) {
    if (g_unloading.load(std::memory_order_relaxed)) {
        return;
    }

    wucore::CoreDispatcher dispatcher = CopyDispatcher();
    if (!dispatcher) {
        return;
    }

    try {
        PostCountedUiTurn(dispatcher, [closedGeneration, closing]() {
            try {
                if (g_unloading.load(std::memory_order_relaxed)) {
                    return;
                }
                EndMeasureRequest(closing.generation, closing.id);
                StopSettledTrailWatchIfGeneration(closedGeneration);
                ReleaseScrollableElementIfGeneration(closedGeneration);

            } catch (...) {

            }
        });
    } catch (winrt::hresult_error const& e) {
        TraceLog(L"open watch release err=%08X", static_cast<unsigned>(e.code()));
    } catch (...) {
        TraceLog(L"open watch release err=unknown");
    }
}

void ApplyBridgeIntentRecord(const SwipeIpcIntent& intent) {
    if (intent.seq == 0 || intent.seq == g_lastIntentSeq) {
        return;
    }
    g_lastIntentSeq = intent.seq;
    if (!BridgeCapabilityNow()) {

        TraceLog(L"intent drop seq=%lld reason=no-capability",
                static_cast<long long>(intent.seq));
        SwipeIpcAnswerIntent(&g_ipc, intent.seq, kSwipeAckRefused, 0, 0, E_ABORT);
        return;
    }
    const wchar_t* kind = intent.kind == kSwipeIntentOpen
                              ? L"open"
                              : (intent.kind == kSwipeIntentClose ? L"close"
                                                                  : L"external");
    const wchar_t* trigger =
        intent.trigger == kSwipeTriggerWin
            ? L"win"
            : (intent.trigger == kSwipeTriggerStartButton
                   ? L"startbutton"
                   : (intent.trigger == kSwipeTriggerClickAway ? L"clickaway"
                                                               : L"override"));
    TraceLog(L"intent recv seq=%lld kind=%s trigger=%s method=%u flags=%u "
            L"open=%llu",
            static_cast<long long>(intent.seq), kind, trigger, intent.method,
            intent.flags, static_cast<unsigned long long>(intent.open));

    int32_t ackState = kSwipeAckExecuted;
    int32_t ackDuration = 0;
    int32_t ackEntry = 0;
    HRESULT ackHr = S_OK;

    if (intent.kind == kSwipeIntentExternal) {

        const CloseScheduleCancel dropped = CancelCloseSchedules(L"external");
        const LONG64 show = CancelPendingShow(L"external");
        RetireShowWatch(L"external");
        g_controllerShowSeq = 0;
        g_unconfirmedRepairUntil = 0;
        g_intentCancel = CloseCancelTicket{};
        EndClose(L"external");
        g_intentOpenActive.store(false, std::memory_order_relaxed);
        ForgetPendingDismiss();
        if (g_hidSession.active) {
            g_hidSession.active = false;
            EndContactTiming();
        }
        g_reveal = HidRevealSession{};
        g_contactOrigin = HidContactOrigin{};
        g_lateRelease = RevealLateRelease{};
        SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionUnowned);
        SetControllerTouch(false);
        g_commitNeedsProgressReset = false;

        NoteVisibilityTransition(kVisibilityWantClosed);
        TraceLog(L"intent external drop trigger=%s slot=%d command=%d "
                L"show=%lld",
                trigger, dropped.slot ? 1 : 0, dropped.command ? 1 : 0,
                static_cast<long long>(show));
    } else if (intent.kind == kSwipeIntentOpen) {
        const GestureGeometry geometry = ReadGestureGeometry();
        if (StartWindowIsOpen()) {

            CancelCloseForOpenRequest();
            const float live = SettlePressOpen();
            TraceLog(L"intent open cancel from=%.3f ticket=%d ticket_open=%u "
                    L"ticket_seq=%lld ticket_expire=%llu",
                    live, g_intentCancel.pending ? 1 : 0,
                    g_intentCancel.openTick,
                    static_cast<long long>(g_intentCancel.dismissSeq),
                    static_cast<unsigned long long>(g_intentCancel.expireAt));
        } else {

            CancelPendingDismiss(L"open-intent");
            SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionOpen);
            ForgetPendingDismiss();
            const float entry =
                std::clamp(RevealEntryProgress(geometry), 0.0f, 1.0f);
            g_intentOpenEntry.store(entry, std::memory_order_relaxed);
            g_intentOpenActive.store(true, std::memory_order_relaxed);
            NoteOpenClaimTick();
            g_panel.Begin(&entry);
            if (RequestBridgeShow(intent.method ? intent.method : 1,
                                  intent.flags)) {
                ackEntry = static_cast<int32_t>(std::llround(entry * 1000.0f));
                TraceLog(L"intent open entry=%.3f", entry);
            } else {

                ShowAtRest();
                g_commitNeedsProgressReset = false;
                ReleaseExecutorOwnership();
                ackState = kSwipeAckRefused;
                ackHr = E_FAIL;
                TraceLog(L"intent open refused entry=%.3f reason=no-executor",
                        entry);
            }
        }
    } else if (intent.kind == kSwipeIntentClose) {

        CancelCloseSchedules(L"close-intent");

        ForgetPendingDismiss();

        TakeCloseOwnership(CloseKind::Dismiss, GetTickCount64(), g_contactDown);
        const float from = TakeProgressNow(L"intent-close");
        std::chrono::milliseconds duration{0};
        if (!BeginShellCloseSettle(from, duration)) {

            ShowAtRest();
            g_commitNeedsProgressReset = false;
            EndClose(L"no-settle");
            ReleaseExecutorOwnership();
            ackState = kSwipeAckRefused;
            ackHr = E_FAIL;
        } else {
            g_commitNeedsProgressReset = true;
            SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionClosing);
            const uint64_t executeAt =
                GetTickCount64() + static_cast<uint64_t>(duration.count()) +
                kBridgeDismissLeadMs;
            if (RequestBridgeDismiss(
                    ExecDismissMethodForIntent(intent.method, intent.trigger),
                    executeAt, intent.open)) {
                NoteCloseDismissPublished();
                ackDuration = static_cast<int32_t>(duration.count());
                TraceLog(L"intent close from=%.3f dur=%lld", from,
                        static_cast<long long>(duration.count()));
            } else {

                ShowAtRest();
                g_commitNeedsProgressReset = false;
                EndClose(L"no-executor");
                ReleaseExecutorOwnership();
                ackState = kSwipeAckRefused;
                ackHr = E_FAIL;
                TraceLog(L"intent close refused from=%.3f reason=no-executor",
                        from);
            }
        }
    }

    SwipeIpcAnswerIntent(&g_ipc, intent.seq, ackState, ackDuration, ackEntry,
                         static_cast<int32_t>(ackHr));
}

void AnswerRevokedIntent(LONG64 sequence) {
    TraceLog(L"intent revoked seq=%lld", static_cast<long long>(sequence));
    SwipeIpcAnswerIntent(&g_ipc, sequence, kSwipeAckSuperseded, 0, 0, S_OK);
}

void ApplyBridgeIntent() {
    const LONG64 sequence = SwipeIpcIntentSequence(&g_ipc);
    if (sequence == 0 || sequence == g_lastIntentSeq) {
        return;
    }
    SwipeIpcTakeIntent(&g_ipc, sequence);
    SwipeIpcIntent intent{};
    if (!SwipeIpcReadIntent(&g_ipc, &intent) || intent.seq != sequence) {
        TraceLog(L"intent drop seq=%lld reason=record-moved",
                static_cast<long long>(sequence));
        AnswerRevokedIntent(sequence);
        return;
    }
    ApplyBridgeIntentRecord(intent);
}

void ReleaseExecutorOwnership() {
    SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionUnowned);
    SetControllerTouch(false);
    g_intentOpenActive.store(false, std::memory_order_relaxed);
    g_intentOpenEntry.store(0.0f, std::memory_order_relaxed);
}

void ArmShowWatch(LONG64 sequence) {
    if (CloseInFlight() || CloseHideMayLand()) {

        // A close owns the menu now; its own record answers whether it opened.
        TraceLog(L"show watch skipped seq=%lld reason=close",
                static_cast<long long>(sequence));
        return;
    }
    g_showWatchUntil = GetTickCount64() + kShowUnfulfilledMs;
    g_showWatchSeq = sequence;
    TraceLog(L"show watch armed seq=%lld bound=%llu",
            static_cast<long long>(sequence),
            static_cast<unsigned long long>(kShowUnfulfilledMs));
}

void RetireShowWatch(const wchar_t* reason) {
    if (g_showWatchUntil == 0) {
        return;
    }
    g_showWatchUntil = 0;
    g_showWatchSeq = 0;
    TraceLog(L"show watch retired reason=%s", reason);
}

void CheckShowWatch(uint64_t tickMs) {
    if (g_showWatchUntil == 0) {
        return;
    }
    if (CloseInFlight() || CloseHideMayLand()) {
        RetireShowWatch(L"close");
        return;
    }
    if (tickMs < g_showWatchUntil) {
        return;
    }
    const LONG64 sequence = g_showWatchSeq;
    const CloakState cloak = ReadCloakState();
    RetireShowWatch(cloak == CloakState::Open ? L"cloak-open"
                                              : L"show-unfulfilled");
    if (cloak == CloakState::Open) {
        return;
    }
    if (sequence != g_controllerShowSeq ||
        !g_intentOpenActive.load(std::memory_order_relaxed)) {
        TraceLog(L"show watch stale seq=%lld current=%lld open=%d",
                static_cast<long long>(sequence),
                static_cast<long long>(g_controllerShowSeq),
                g_intentOpenActive.load(std::memory_order_relaxed) ? 1 : 0);
        return;
    }

    // The show succeeded but no open sample arrived: the menu never appeared.
    // Give it back to the shell instead of owning a hidden menu as open.
    ShowAtRest();
    g_commitNeedsProgressReset = false;
    ReleaseExecutorOwnership();
    TraceLog(L"show unfulfilled seq=%lld cloak=%s",
            static_cast<long long>(sequence), CloakStateName(cloak));
}

bool DismissGenerationCurrent(uint64_t commandGeneration) {
    uint64_t currentGeneration = 0;
    uint64_t openTick = 0;
    if (!SwipeIpcReadOpen(&g_ipc, &currentGeneration, &openTick)) {
        return false;
    }
    return commandGeneration == currentGeneration;
}

void ApplyExecAck() {
    SwipeIpcExecAck ack{};
    if (!SwipeIpcReadExecAck(&g_ipc, &ack)) {
        return;
    }

    TraceLog(L"exec ack seq=%lld command=%lld state=%u hr=%08X visible=%d",
            static_cast<long long>(ack.seq),
            static_cast<long long>(ack.command), ack.state,
            static_cast<unsigned>(ack.hr), ack.visible);
    const bool failed =
        ack.state == kSwipeAckRefused || ack.state == kSwipeAckExpired;
    const bool executed = ack.state == kSwipeAckExecuted;
    const bool confirmed = ack.state == kSwipeAckConfirmed;
    const bool unconfirmed = ack.state == kSwipeAckUnconfirmed ||
                             ack.state == kSwipeAckUnknown;
    const bool dismissPending =
        ack.command != 0 &&
        ack.command == g_pendingDismissExec.load(std::memory_order_relaxed);
    const bool dismissConfirming =
        ack.command != 0 && ack.command == g_pendingDismissConfirm;

    const bool dismissTicket =
        g_intentCancel.pending && ack.command != 0 &&
        g_intentCancel.dismissSeq == ack.command;
    if (dismissTicket &&
        (failed || ack.state == kSwipeAckSuperseded)) {
        const wchar_t* reason = ack.state == kSwipeAckSuperseded
                                    ? L"superseded"
                                    : (ack.state == kSwipeAckRefused
                                           ? L"refused"
                                           : L"expired");
        TraceLog(L"intent recovery ticket retired seq=%lld reason=%s",
                static_cast<long long>(ack.command), reason);
        g_intentCancel = CloseCancelTicket{};
    }
    if (dismissPending || dismissConfirming) {

        if (executed) {

            if (dismissPending) {
                g_pendingDismissConfirmGeneration =
                    g_pendingDismissExecGeneration;
                g_pendingDismissExec.store(0, std::memory_order_relaxed);
                g_pendingDismissExecAt.store(0, std::memory_order_relaxed);
                g_pendingDismissExecGeneration = 0;
            }
            g_pendingDismissConfirm = ack.command;
            TraceLog(L"exec dismiss executed command=%lld visible=%d "
                    L"(awaiting confirmation)",
                    static_cast<long long>(ack.command), ack.visible);
            return;
        }
        if (confirmed) {
            TraceLog(L"exec dismiss confirmed command=%lld visible=%d",
                    static_cast<long long>(ack.command), ack.visible);

            bool retained = false;
            if (ack.visible == 0) {
                const uint64_t commandGeneration =
                    dismissConfirming
                        ? g_pendingDismissConfirmGeneration
                        : g_pendingDismissExecGeneration;
                const CloakState cloak = ReadCloakState();
                if (!DismissGenerationCurrent(commandGeneration)) {

                    TraceLog(L"exec dismiss stale command=%lld gen=%llu",
                            static_cast<long long>(ack.command),
                            static_cast<unsigned long long>(commandGeneration));
                } else if (cloak == CloakState::Hidden) {

                    LandClosedMenu(GetTickCount64(),
                                   L"confirmed-without-transition");
                } else {

                    if (dismissPending) {
                        g_pendingDismissConfirm = ack.command;
                        g_pendingDismissConfirmGeneration =
                            g_pendingDismissExecGeneration;
                    }
                    g_pendingDismissConfirmUntil =
                        GetTickCount64() + kDismissConfirmGraceMs;
                    g_pendingDismissConfirmRetained = true;
                    retained = true;
                    TraceLog(L"exec dismiss retained command=%lld gen=%llu "
                            L"cloak=%s",
                            static_cast<long long>(ack.command),
                            static_cast<unsigned long long>(
                                g_pendingDismissConfirmGeneration),
                            CloakStateName(cloak));
                }
            }

            g_pendingDismissExec.store(0, std::memory_order_relaxed);
            g_pendingDismissExecAt.store(0, std::memory_order_relaxed);
            g_pendingDismissExecGeneration = 0;
            if (!retained) {
                ForgetConfirmedDismiss();
            }
            return;
        }
        if (dismissPending) {
            g_pendingDismissExec.store(0, std::memory_order_relaxed);
            g_pendingDismissExecAt.store(0, std::memory_order_relaxed);
            g_pendingDismissExecGeneration = 0;
        }
        if (unconfirmed || failed) {

            if (unconfirmed && g_intentCancel.pending &&
                g_intentCancel.dismissSeq == ack.command) {
                g_intentCancel = CloseCancelTicket{};
            }
            ForgetConfirmedDismiss();

            EndClose(L"repair");

            // The close is over, but the panel can sit off its rest with the
            // menu still visible. Restore at once when the cloak says open;
            // otherwise keep a bounded watch so the unhealed repair restores it
            // when the window is next observed open.
            const CloakState cloak = ReadCloakState();
            if (cloak == CloakState::Open) {
                ShowAtRest();
                g_commitNeedsProgressReset = false;
            } else {
                NoteVisibilityTransition(kVisibilityWantOpen);
                g_unconfirmedRepairUntil =
                    GetTickCount64() + kVisibilityGraceMs;
            }
            ReleaseExecutorOwnership();
            TraceLog(L"exec dismiss %s command=%lld state=%u hr=%08X visible=%d "
                    L"cloak=%s",
                    unconfirmed ? L"unconfirmed" : L"failed",
                    static_cast<long long>(ack.command), ack.state,
                    static_cast<unsigned>(ack.hr), ack.visible,
                    CloakStateName(cloak));
            return;
        }

        if (ack.state == kSwipeAckSuperseded) {
            ForgetConfirmedDismiss();
            TraceLog(L"exec dismiss superseded command=%lld",
                    static_cast<long long>(ack.command));
            return;
        }
        return;
    }
    if (ack.command != g_pendingShowExec.load(std::memory_order_relaxed)) {
        return;
    }
    g_pendingShowExec.store(0, std::memory_order_relaxed);
    g_pendingShowUntil.store(0, std::memory_order_relaxed);
    if (ack.state == kSwipeAckSuperseded) {
        TraceLog(L"exec show superseded command=%lld",
                static_cast<long long>(ack.command));
        return;
    }
    if (failed) {

        if (g_pendingDismissExec.load(std::memory_order_relaxed) == 0) {
            ReleaseExecutorOwnership();
        }
        TraceLog(L"exec show failed command=%lld state=%u hr=%08X",
                static_cast<long long>(ack.command), ack.state,
                static_cast<unsigned>(ack.hr));
        return;
    }
    if (executed) {

        // The visibility probe is advisory: the Start side has not observed the
        // open until its own cloak sample says so.
        ArmShowWatch(ack.command);
    }
}

void CheckExecAckTimeout(uint64_t now) {
    const LONG64 show = g_pendingShowExec.load(std::memory_order_relaxed);
    if (show != 0 && now >= g_pendingShowUntil.load(std::memory_order_relaxed)) {
        g_pendingShowExec.store(0, std::memory_order_relaxed);
        g_pendingShowUntil.store(0, std::memory_order_relaxed);

        const bool dismissPending =
            g_pendingDismissExec.load(std::memory_order_relaxed) != 0;
        if (!dismissPending && g_ipc.state) {
            SwipeIpcPublishExec(&g_ipc, kSwipeExecCancel, 0, 0, 0, 0, 0);
            TraceLog(L"exec cancel request target=0 reason=show-expired");
        }

        if (!dismissPending) {
            ReleaseExecutorOwnership();
        }
        TraceLog(L"exec ack timeout seq=%lld kind=show open=%d (resync from "
                L"cloak)",
                static_cast<long long>(show), StartWindowIsOpen() ? 1 : 0);
    }
    const LONG64 dismiss = g_pendingDismissExec.load(std::memory_order_relaxed);
    if (dismiss != 0) {
        const uint64_t executeAt =
            g_pendingDismissExecAt.load(std::memory_order_relaxed);
        if (executeAt != 0 && now >= executeAt + kBridgeAckTimeoutMs) {
            g_pendingDismissExec.store(0, std::memory_order_relaxed);
            g_pendingDismissExecAt.store(0, std::memory_order_relaxed);
            g_pendingDismissExecGeneration = 0;
            if (g_ipc.state) {
                SwipeIpcPublishExec(&g_ipc, kSwipeExecCancel, 0, 0, 0, 0,
                                    dismiss);
                TraceLog(L"exec cancel request target=%lld "
                        L"reason=dismiss-expired",
                        static_cast<long long>(dismiss));
            }
            ForgetConfirmedDismiss();

            EndClose(L"repair");
            ReleaseExecutorOwnership();
            TraceLog(L"exec ack timeout seq=%lld kind=dismiss open=%d (resync "
                    L"from cloak)",
                    static_cast<long long>(dismiss),
                    StartWindowIsOpen() ? 1 : 0);
            return;
        }
    }

    const LONG64 retained = g_pendingDismissConfirm;
    if (!g_pendingDismissConfirmRetained || retained == 0 ||
        g_pendingDismissConfirmUntil == 0 ||
        now < g_pendingDismissConfirmUntil) {
        return;
    }
    TraceLog(L"exec dismiss retained expired command=%lld gen=%llu (resync "
            L"from cloak)",
            static_cast<long long>(retained),
            static_cast<unsigned long long>(
                g_pendingDismissConfirmGeneration));
    ForgetPendingDismiss();
    EndClose(L"repair");
    ReleaseExecutorOwnership();
}

void ReconcileSupersedingIntentBeforeLanding() {
    SwipeIpcIntent intent{};
    if (!SwipeIpcReadIntent(&g_ipc, &intent) || intent.seq == 0 ||
        intent.seq == g_lastIntentSeq) {
        return;
    }
    if (intent.kind != kSwipeIntentExternal) {
        return;
    }
    SwipeIpcTakeIntent(&g_ipc, intent.seq);
    SwipeIpcIntent current{};
    if (!SwipeIpcReadIntent(&g_ipc, &current) || current.seq != intent.seq) {
        TraceLog(L"intent reconcile drop seq=%lld reason=record-moved",
                static_cast<long long>(intent.seq));
        AnswerRevokedIntent(intent.seq);
        return;
    }
    TraceLog(L"intent reconcile seq=%lld kind=external (before landing recovery)",
            static_cast<long long>(intent.seq));

    ApplyBridgeIntentRecord(intent);
}

bool BeginLandingOpen() {
    if (!RendererReady() || StartWindowIsOpen() ||
        g_intentOpenActive.load(std::memory_order_relaxed)) {
        return false;
    }
    const GestureGeometry geometry = ReadGestureGeometry();
    float entry = 0.0f;
    if (!PublishOpenClaim(geometry, false, entry)) {

        TraceLog(L"landing open failed entry=%.3f reason=no-executor", entry);
        return false;
    }
    TraceLog(L"intent open recovered entry=%.3f", entry);
    return true;
}

void LandClosedMenu(uint64_t tickMs, const wchar_t* reason) {

    ReconcileSupersedingIntentBeforeLanding();

    const bool execCommand =
        g_pendingDismissExec.load(std::memory_order_relaxed) != 0 &&
        DismissGenerationCurrent(g_pendingDismissExecGeneration);
    const bool confirmCommand =
        g_pendingDismissConfirm != 0 &&
        DismissGenerationCurrent(g_pendingDismissConfirmGeneration);
    const bool closeCommand = execCommand || confirmCommand;

    const bool ticketCurrent =
        g_intentCancel.pending && g_intentCancel.openTick == g_ipcOpenTickLow;
    const bool ticketLive =
        ticketCurrent &&
        (g_intentCancel.expireAt == 0 || tickMs <= g_intentCancel.expireAt);
    const bool intentCancelLost = ticketLive;
    if (g_intentCancel.pending) {
        TraceLog(L"intent recovery ticket open=%u now=%u seq=%lld served=%d "
                L"expired=%d",
                g_intentCancel.openTick, g_ipcOpenTickLow,
                static_cast<long long>(g_intentCancel.dismissSeq),
                intentCancelLost ? 1 : 0,
                ticketCurrent && !ticketLive ? 1 : 0);
    }
    g_intentCancel = CloseCancelTicket{};

    const bool closeOurs = closeCommand || intentCancelLost;
    if (closeOurs && confirmCommand && g_pendingDismissConfirmRetained) {

        TraceLog(L"exec dismiss retained served command=%lld gen=%llu",
                static_cast<long long>(g_pendingDismissConfirm),
                static_cast<unsigned long long>(
                    g_pendingDismissConfirmGeneration));
    }

    EndClose(closeOurs ? L"landing" : L"external");
    if (!closeOurs) {

        const bool contactLive =
            g_contactDown || g_hidSession.active || g_reveal.active ||
            g_reveal.edgeCandidate;
        if (g_reveal.active || g_reveal.edgeCandidate ||
            g_reveal.granted) {
            g_reveal = HidRevealSession{};
            TraceLog(L"reveal dropped (window closed by shell)");
        }
        if (g_hidSession.active) {
            g_hidSession.active = false;
            EndContactTiming();
            TraceLog(L"hid dropped (window closed by shell)");
        }
        if (contactLive) {
            g_hidSession.suppress = true;
            g_contactOrigin = HidContactOrigin{};
        }
        g_lateRelease = RevealLateRelease{};
    }

    const uint32_t closedGeneration =
        g_openGeneration.load(std::memory_order_relaxed);
    const SettledMeasureRequest closing = SnapshotMeasureRequest();
    AdvanceOpenGeneration();

    const int muted = g_hidSession.suppress ? 1 : 0;
    if (reason) {

        TraceLog(L"win close observed reason=%s muted=%d", reason, muted);
    } else {
        TraceLog(L"win close observed muted=%d", muted);
    }
    SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionUnowned);
    SetControllerTouch(false);
    g_intentOpenActive.store(false, std::memory_order_relaxed);
    g_unconfirmedRepairUntil = 0;
    RetireShowWatch(L"closed");
    g_controllerShowSeq = 0;

    ForgetPendingDismiss();

    PostUiOpenWatchRelease(closedGeneration, closing);

    if (intentCancelLost) {
        BeginLandingOpen();
    }

    g_rootWasOpen = false;
}

bool ServiceRootVisibility(uint64_t tickMs, bool viaFallback) {

    const CloakState sample = ReadCloakState();
    if (sample == CloakState::Unknown) {
        return false;
    }
    const bool open = sample == CloakState::Open;
    if (open) {

        RetireShowWatch(L"cloak-open");
    }
    const bool unconfirmedRepair =
        open && g_unconfirmedRepairUntil != 0 &&
        tickMs <= g_unconfirmedRepairUntil;
    if (unconfirmedRepair) {
        g_unconfirmedRepairUntil = 0;
        ShowAtRest();
        g_commitNeedsProgressReset = false;
        TraceLog(L"win open repair unconfirmed progress=%.3f",
                PeekProgressNow());
    }
    const bool wasOpen = g_rootWasOpen;

    const bool capable = BridgeCapabilityNow();

    const uint32_t want = g_visibilityWant;
    if ((want == kVisibilityWantOpen && open) ||
        (want == kVisibilityWantClosed && !open)) {
        g_visibilityWant = kVisibilityWantNone;
    }
    if (!viaFallback) {
        const uint64_t watch = tickMs + kVisibilitySettleWatchMs;
        if (watch > g_visibilityPollUntil) {
            g_visibilityPollUntil = watch;
        }
    }

    const bool closeInFlight = CloseInFlight();
    const bool justOpened = open && !wasOpen;
    if (justOpened) {

        const bool openDuringClose = closeInFlight;
        if (openDuringClose) {
            TraceLog(L"win open during close progress=%.3f", PeekProgressNow());

            CancelCloseSchedules(L"open");
            g_intentCancel = CloseCancelTicket{};
            SwipeIpcSetControllerDirection(&g_ipc, kSwipeDirectionOpen);
            SetControllerTouch(false);
            EndClose(L"open");

        }

        const LONG64 publishedOpen = SwipeIpcPublishOpen(&g_ipc, tickMs);
        g_ipcOpenTickLow = static_cast<uint32_t>(tickMs);

        RetirePendingDismissForFreshOpen(
            static_cast<uint64_t>(publishedOpen));

        AdvanceOpenGeneration();

        if (!capable) {

            ShowAtRest();
            g_intentOpenActive.store(false, std::memory_order_relaxed);
            g_commitNeedsProgressReset = false;
            g_rootWasOpen = open;
            return true;
        }

        const GestureGeometry geometry = ReadGestureGeometry();
        PromoteRevealContact(geometry, tickMs);

        const bool gestureOwnsProgress = g_reveal.active;

        const bool lateAdopted =
            !gestureOwnsProgress && ConsumeLateRevealRelease(geometry);

        if (!gestureOwnsProgress && !lateAdopted) {
            const bool intentEntry =
                g_intentOpenActive.exchange(false, std::memory_order_relaxed);
            const float intentFrom =
                g_intentOpenEntry.load(std::memory_order_relaxed);
            const bool unowned = !intentEntry && !openDuringClose;
            if (unowned) {

                ShowAtRest();
                g_commitNeedsProgressReset = false;
                TraceLog(L"win open adopted reason=unowned");
            } else if (geometry.distance() > 0.0f) {

                const float from = intentEntry
                                       ? intentFrom
                                       : TakeVisibleProgressNow(
                                             L"win-during-close");
                std::chrono::milliseconds duration{0};
                if (BeginGestureSettle(from, 1.0f, 0.0f, duration)) {

                    g_commitNeedsProgressReset = false;
                    if (intentEntry) {
                        TraceLog(L"win open intent from=%.3f dur=%llu", from,
                                static_cast<unsigned long long>(duration.count()));
                    } else {
                        TraceLog(L"win open during close from=%.3f dur=%llu", from,
                                static_cast<unsigned long long>(duration.count()));
                    }
                }
            } else {
                ShowAtRest();

                g_intentOpenActive.store(false, std::memory_order_relaxed);
                TraceLog(L"win open restore (no geometry)");
            }
        }

        const SettledMeasureRequest request = StartMeasureRequest(
            !g_hidSession.active && !g_reveal.edgeCandidate);
        PostGeometryMeasurement(request, false);
    } else if (!capable) {

    } else if (!open && g_rootWasOpen) {
        LandClosedMenu(tickMs, nullptr);
    } else if (open) {

        if (g_intentCancel.pending && g_intentCancel.expireAt != 0 &&
            tickMs > g_intentCancel.expireAt) {
            TraceLog(L"intent recovery ticket retired open=%u now=%u seq=%lld "
                    L"reason=expired",
                    g_intentCancel.openTick, g_ipcOpenTickLow,
                    static_cast<long long>(g_intentCancel.dismissSeq));
            g_intentCancel = CloseCancelTicket{};
        }

        const GestureGeometry geometry = ReadGestureGeometry();
        const float distance = geometry.distance();
        const float live = PeekProgressNow();
        const bool offScreen =
            distance > 0.0f && live < RevealEntryProgress(geometry);

        const bool closeOwnsPanel = CloseInFlight();
        const bool hideMayLand = CloseHideMayLand();
        if (offScreen && !closeOwnsPanel && !hideMayLand &&
            !ProgressMotionActive() && !g_reveal.active &&
            !g_hidSession.active) {
            if (tickMs > g_unhealedLogTick &&
                tickMs - g_unhealedLogTick >= 1000) {
                g_unhealedLogTick = tickMs;
                TraceLog(L"win open unhealed progress=%.3f", live);
            }
            std::chrono::milliseconds duration{0};
            if (!BeginGestureSettle(live, 1.0f, 0.0f, duration)) {
                ShowAtRest();
            }
            g_commitNeedsProgressReset = false;
        }
    }

    if (viaFallback && open != wasOpen) {

        TraceLog(L"win state fallback open=%d want=%u", open ? 1 : 0, want);
    }
    g_rootWasOpen = open;
    return true;
}

uint64_t NextInputDeadlineMs(uint64_t nowTick, uint64_t nowQpc) {
    uint64_t soonest = 0;
    const auto consider = [&soonest](uint64_t delay) {
        if (delay != 0 && (soonest == 0 || delay < soonest)) {
            soonest = delay;
        }
    };
    const auto considerTick = [&](uint64_t due) {
        consider(due > nowTick ? due - nowTick : 1);
    };
    const auto considerQpc = [&](uint64_t due) {
        consider(due > nowQpc ? due - nowQpc : 1);
    };
    const auto considerQpcAhead = [&](uint64_t due) {
        if (due > nowQpc) {
            consider(due - nowQpc);
        }
    };

    if (!g_rootHookReady.load(std::memory_order_acquire) &&
        !g_rootHookRefused.load(std::memory_order_acquire)) {
        consider(kRootProbeMs);
    }

    if (!g_ipc.capabilityEvent && !BridgeCapabilityNow() &&
        g_rootHookReady.load(std::memory_order_acquire) && RendererReady()) {
        consider(kCapabilityProbeMs);
    }

    const bool coasting = g_panel.coasting.load(std::memory_order_relaxed);

    if (coasting) {
        considerQpc(g_panel.coastStartQpc +
                    static_cast<uint64_t>(kCoastDuration.count()));
    }

    // A hold that is still owed must wake when it expires: nothing else arms a
    // deadline while it holds the idle release back.
    const uint64_t holdUntil = OpenClaimHoldUntil();
    if (holdUntil > nowTick) {
        consider(holdUntil - nowTick);
    }

    if (!IdleReleaseHeld()) {

        if (!coasting) {
            if (g_hidSession.active) {
                considerQpcAhead(g_hidSession.lastSampleQpc + kCoastStartMs);
            }
            if (g_reveal.active) {

                considerQpcAhead(std::max(
                    g_reveal.lastSampleQpc + kCoastStartMs,
                    g_reveal.activeSinceQpc + kRevealCoastSettleMs));
            }
        }
        if (g_hidSession.active) {
            considerQpc(g_hidSession.lastAliveQpc + kIdleReleaseMs);
        }
        if (g_reveal.edgeCandidate) {
            considerQpc(g_reveal.lastAliveQpc + kCandidateIdleHoldMs);
        }

        const bool holdLatch =
            CloseInFlight() ||
            (g_hidSession.suppress && g_trackedContact.hasIdentifier);
        if (!g_hidSession.active && !g_reveal.edgeCandidate && !holdLatch) {
            if (g_contactDown) {
                considerTick(g_contactTick + kIdleReleaseMs);
            }
            if (g_trackedContact.slot >= 0) {
                considerTick(g_trackedContact.lastEvidenceTick + kIdleReleaseMs);
            }
        }
    }

    if (CloseInFlight()) {
        considerTick(g_close.repairAt);
    }

    if (g_pendingTapTick != 0) {
        considerTick(g_pendingTapTick);
    }

    const uint64_t showUntil = g_pendingShowUntil.load(std::memory_order_relaxed);
    if (showUntil != 0) {
        considerTick(showUntil);
    }
    if (g_showWatchUntil != 0) {
        considerTick(g_showWatchUntil);
    }
    const uint64_t dismissAt =
        g_pendingDismissExecAt.load(std::memory_order_relaxed);
    if (g_pendingDismissExec.load(std::memory_order_relaxed) != 0 &&
        dismissAt != 0) {
        considerTick(dismissAt + kBridgeAckTimeoutMs);
    }

    if (g_pendingDismissConfirmRetained && g_pendingDismissConfirmUntil != 0) {
        considerTick(g_pendingDismissConfirmUntil);
    }

    const uint64_t measureDue = PeekMeasureDeadlineTick();
    if (measureDue != 0) {
        considerTick(measureDue);
    }
    const uint64_t scrollerRetry =
        g_scrollerRetryAt.load(std::memory_order_relaxed);
    if (scrollerRetry != 0) {
        considerTick(scrollerRetry);
    }

    if (g_visibilityPollUntil != 0 && nowTick < g_visibilityPollUntil) {
        const uint64_t due =
            (g_lastVisibilityPollTick != 0
                 ? g_lastVisibilityPollTick + kVisibilityFallbackMs
                 : nowTick + kVisibilityFallbackMs);
        considerTick(due < g_visibilityPollUntil ? due : g_visibilityPollUntil);
    }

    return soonest;
}

void ServiceDeadlineWork(uint64_t tickMs) {
    RunPendingCloseTap(tickMs);

    RepairExpiredClose(tickMs);
    CheckHidIdleRelease(tickMs);
    CheckExecAckTimeout(tickMs);
    CheckShowWatch(tickMs);
    if (g_visibilityPollUntil != 0) {
        if (tickMs >= g_visibilityPollUntil) {

            g_visibilityPollUntil = 0;
            g_visibilityWant = kVisibilityWantNone;
            g_unconfirmedRepairUntil = 0;
            ServiceRootVisibility(tickMs, true);
        } else if (g_lastVisibilityPollTick == 0 ||
                   tickMs - g_lastVisibilityPollTick >= kVisibilityFallbackMs) {
            g_lastVisibilityPollTick = tickMs;
            ServiceRootVisibility(tickMs, true);
        }
    }
}

void ResetDrivenStateForCapabilityLoss() {
    CancelCloseSchedules(L"capability");
    EndClose(L"capability");
    ForgetPendingDismiss();
    g_pendingShowExec.store(0, std::memory_order_relaxed);
    g_pendingShowUntil.store(0, std::memory_order_relaxed);
    g_showWatchUntil = 0;
    g_showWatchSeq = 0;
    g_controllerShowSeq = 0;
    g_unconfirmedRepairUntil = 0;
    g_intentCancel = CloseCancelTicket{};
    ReleaseExecutorOwnership();
    if (g_hidSession.active) {
        g_hidSession.active = false;
        EndContactTiming();
    }
    g_reveal = HidRevealSession{};
    g_contactOrigin = HidContactOrigin{};
    g_lateRelease = RevealLateRelease{};
    g_commitNeedsProgressReset = false;
    if (RendererReady()) {

        ShowAtRest();
    }
}

void NoteContactAlive(uint64_t tickMs) {
    g_contactDown = true;
    g_contactTick = tickMs;
    const uint64_t qpc = QpcMsNow();
    if (g_hidSession.active) {
        g_hidSession.lastTick = tickMs;
        g_hidSession.lastAliveQpc = qpc;
    }
    if (g_reveal.edgeCandidate) {
        g_reveal.lastTick = tickMs;
        g_reveal.lastAliveQpc = qpc;
    }
}

void FeedContactPoint(HidDevice& device, long rawX, long rawY, USHORT reportId,
                      USHORT collection, uint64_t tickMs) {

    PublishPeerTick(tickMs);

    const int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    const int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    if (screenWidth <= 1 || screenHeight <= 1) {
        LogDecodeFail(device, 0);
        return;
    }

    const HidAxisRange xRange =
        FindAxisRange(device, kUsageX, reportId, collection);
    const HidAxisRange yRange =
        FindAxisRange(device, kUsageY, reportId, collection);
    const float dpiScale = HidDpiScale();

    const int32_t pixelX = ToScreenPixel(rawX, xRange, screenWidth);
    const int32_t pixelY = ToScreenPixel(rawY, yRange, screenHeight);
    const float xDip = static_cast<float>(pixelX) / dpiScale;
    const float yDip = static_cast<float>(pixelY) / dpiScale;

    g_screenDipWidth = static_cast<float>(screenWidth) / dpiScale;
    g_screenDipHeight = static_cast<float>(screenHeight) / dpiScale;

    OnHidSample(1, xDip, yDip, tickMs);

    g_hidSession.lastRawX = rawX;
    g_hidSession.lastRawY = rawY;
}

bool ReportAssertsContact(const HidDevice& device, const HidContactSlot* slots,
                          int slotCount, HidField countField,
                          ULONG topLevelCount) {
    if (device.coordinatePath) {
        if (countField == HidField::Ok) {
            return topLevelCount > 0;
        }
        for (int i = 0; i < slotCount; ++i) {
            if (slots[i].hasPosition && !slots[i].atOrigin) {
                return true;
            }
        }
        return false;
    }
    for (int i = 0; i < slotCount; ++i) {
        if (SlotContactState(slots[i]) == HidContactState::Contact) {
            return true;
        }
    }
    return false;
}

bool ReportProvesNewContact(const HidDevice& device, const HidContactSlot* slots,
                            int slotCount, HidField countField,
                            ULONG topLevelCount) {
    if (!ReportAssertsContact(device, slots, slotCount, countField,
                              topLevelCount)) {
        return false;
    }
    if (g_trackedContact.device != device.handle) {
        return true;
    }
    if (!g_trackedContact.hasIdentifier) {

        return true;
    }
    for (int i = 0; i < slotCount; ++i) {
        if (SlotContactState(slots[i]) == HidContactState::Contact &&
            slots[i].hasIdentifier) {

            return true;
        }
    }
    return false;
}

void FeedHidReport(HidDevice& device, const BYTE* report,
                   ULONG reportLength, uint64_t tickMs) {
    if (!device.ready || !device.preparsed) {
        LogDecodeFail(device, 0);
        return;
    }

    const USHORT reportId =
        device.hasReportIds && reportLength > 0 ? report[0] : 0;

    USHORT collections[kMaxContactCollections] = {};
    int slotCount = 0;
    if (device.contactNodeCount > 0) {
        slotCount = device.contactNodeCount;
        for (int i = 0; i < slotCount; ++i) {
            collections[i] = device.contactNodes[i];
        }
    } else {
        slotCount = 1;
        collections[0] = 0;
    }

    ULONG topLevelCount = 0;
    const HidField countField =
        device.hasContactCount
            ? ReadUsageValue(device, kUsagePageDigitizer,
                             device.contactCountLink, kUsageContactCount,
                             report, reportLength, &topLevelCount)
            : HidField::Absent;
    if (countField == HidField::Error) {
        LogDecodeFail(device, 0);
        return;
    }

    HidContactSlot slots[kMaxContactCollections];
    bool anyDeclared = countField == HidField::Ok;
    for (int i = 0; i < slotCount; ++i) {
        NTSTATUS error = HIDP_STATUS_SUCCESS;
        if (!DecodeContactSlot(device, collections[i], reportId, report,
                               reportLength, slots[i], error)) {
            LogDecodeFail(device, static_cast<unsigned>(error));
            return;
        }
        anyDeclared = anyDeclared || slots[i].declared;
    }

    if (g_trackedContact.slot >= 0) {
        do {
            if (g_trackedContact.device != device.handle) {

                if (!g_hidSession.suppress ||
                    !ReportProvesNewContact(device, slots, slotCount, countField,
                                            topLevelCount)) {
                    return;
                }
                TraceLog(L"hid consumed ended slot=%d id=%lu reason=device",
                        g_trackedContact.slot, g_trackedContact.identifier);
                EndConsumedContact();
                break;
            }

            int trackedSlot = -1;
            if (g_trackedContact.hasIdentifier) {

                int firstMatch = -1;
                int trackedMatch = -1;
                int assertedMatch = -1;
                int assertedCount = 0;
                for (int i = 0; i < slotCount; ++i) {
                    if (!slots[i].hasIdentifier ||
                        slots[i].identifier != g_trackedContact.identifier) {
                        continue;
                    }
                    if (firstMatch < 0) {
                        firstMatch = i;
                    }
                    if (i == g_trackedContact.slot) {
                        trackedMatch = i;
                    }
                    if (SlotContactState(slots[i]) == HidContactState::Contact) {
                        assertedCount++;
                        if (assertedMatch < 0 || i == g_trackedContact.slot) {
                            assertedMatch = i;
                        }
                    }
                }
                if (assertedMatch >= 0) {
                    trackedSlot = assertedMatch;
                    if (assertedCount > 1 && !g_duplicateContactIdLogged) {
                        g_duplicateContactIdLogged = true;
                        TraceLog(L"hid contact id=%lu declared down in %d slots; "
                                L"using=%d",
                                g_trackedContact.identifier, assertedCount,
                                trackedSlot);
                    }
                } else {

                    trackedSlot = trackedMatch >= 0 ? trackedMatch : firstMatch;
                }
            }
            if (trackedSlot < 0 && g_trackedContact.slot < slotCount &&
                slots[g_trackedContact.slot].declared) {

                const HidContactSlot& candidate = slots[g_trackedContact.slot];
                const bool namesAnother =
                    g_trackedContact.hasIdentifier && candidate.hasIdentifier &&
                    candidate.identifier != g_trackedContact.identifier;
                if (!namesAnother) {
                    trackedSlot = g_trackedContact.slot;
                }
            }
            if (trackedSlot < 0) {

                if (g_hidSession.suppress &&
                    ReportProvesNewContact(device, slots, slotCount, countField,
                                           topLevelCount)) {
                    TraceLog(L"hid consumed ended slot=%d id=%lu "
                            L"reason=new-contact",
                            g_trackedContact.slot,
                            g_trackedContact.identifier);
                    EndConsumedContact();
                    break;
                }
                if (g_trackedContact.slot >= slotCount) {

                    ForgetTrackedContact();
                }

                return;
            }

            const HidContactSlot& slot = slots[trackedSlot];

            if (slot.hasIdentifier) {
                g_trackedContact.hasIdentifier = true;
                g_trackedContact.identifier = slot.identifier;
            }
            g_trackedContact.slot = trackedSlot;

            const HidContactState state = SlotContactState(slot);
            if (state == HidContactState::NotContact) {

                TraceLog(L"hid tracked notcontact field=%d tip=%d tipdown=%d range=%d "
                        L"rangedown=%d id=%d count=%d coord=%d report=%u pos=%d",
                        static_cast<int>(slot.buttons.field),
                        slot.buttons.tipDeclared ? 1 : 0,
                        slot.buttons.tipDown ? 1 : 0,
                        slot.buttons.inRangeDeclared ? 1 : 0,
                        slot.buttons.inRangeDown ? 1 : 0,
                        slot.hasIdentifier ? 1 : 0,
                        countField == HidField::Ok ? topLevelCount : -1,
                        device.coordinatePath ? 1 : 0, reportId,
                        slot.hasPosition ? 1 : 0);
                ForgetTrackedContact();
                OnHidSample(0, 0.0f, 0.0f, tickMs);
                return;
            }

            if (device.coordinatePath && countField == HidField::Ok &&
                topLevelCount == 0) {
                ForgetTrackedContact();
                OnHidSample(0, 0.0f, 0.0f, tickMs);
                return;
            }
            if (!slot.hasPosition) {

                if (state == HidContactState::Contact) {
                    g_trackedContact.lastEvidenceTick = tickMs;
                    NoteContactAlive(tickMs);
                }
                return;
            }
            if (device.coordinatePath && slot.atOrigin) {

                return;
            }

            g_trackedContact.lastEvidenceTick = tickMs;
            FeedContactPoint(device, slot.rawX, slot.rawY, reportId,
                             collections[trackedSlot], tickMs);
            return;
        } while (false);
    }

    if (!anyDeclared) {
        return;
    }

    if (device.coordinatePath) {

        int nonZero = 0;
        int chosen = -1;
        long chosenX = 0;
        long chosenY = 0;
        long bestDistance = 0;
        for (int i = 0; i < slotCount; ++i) {
            if (!slots[i].hasPosition || slots[i].atOrigin) {
                continue;
            }
            nonZero++;
            if (!g_hidSession.active) {
                if (chosen < 0) {
                    chosen = i;
                    chosenX = slots[i].rawX;
                    chosenY = slots[i].rawY;
                }
                continue;
            }
            const long dx = slots[i].rawX - g_hidSession.lastRawX;
            const long dy = slots[i].rawY - g_hidSession.lastRawY;
            const long distance = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
            if (chosen < 0 || distance < bestDistance) {
                chosen = i;
                chosenX = slots[i].rawX;
                chosenY = slots[i].rawY;
                bestDistance = distance;
            }
        }

        const int contacts = countField == HidField::Ok
                                 ? static_cast<int>(topLevelCount)
                                 : nonZero;
        if (countField == HidField::Ok && contacts == 0) {

            OnHidSample(0, 0.0f, 0.0f, tickMs);
            return;
        }
        if (chosen < 0) {

            if (contacts <= 0) {
                OnHidSample(0, 0.0f, 0.0f, tickMs);
            } else {
                LogDecodeFail(device, 0);
            }
            return;
        }

        g_trackedContact.device = device.handle;
        g_trackedContact.slot = chosen;
        g_trackedContact.hasIdentifier = false;
        g_trackedContact.identifier = 0;
        g_trackedContact.lastEvidenceTick = tickMs;
        FeedContactPoint(device, chosenX, chosenY, reportId, collections[chosen],
                         tickMs);
        return;
    }

    int realContacts = 0;
    int chosen = -1;
    long bestDistance = 0;
    for (int i = 0; i < slotCount; ++i) {
        if (SlotContactState(slots[i]) != HidContactState::Contact) {
            continue;
        }
        realContacts++;
        if (!slots[i].hasPosition) {
            continue;
        }
        const long dx = slots[i].rawX - g_hidSession.lastRawX;
        const long dy = slots[i].rawY - g_hidSession.lastRawY;
        const long distance = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy);
        if (chosen < 0 || distance < bestDistance) {
            chosen = i;
            bestDistance = distance;
        }
    }

    if (chosen < 0) {
        if (realContacts > 0) {

            return;
        }
        if (countField == HidField::Ok && topLevelCount > 0) {

            return;
        }

        OnHidSample(0, 0.0f, 0.0f, tickMs);
        return;
    }

    g_trackedContact.device = device.handle;
    g_trackedContact.slot = chosen;
    g_trackedContact.hasIdentifier = slots[chosen].hasIdentifier;
    g_trackedContact.identifier = slots[chosen].identifier;
    g_trackedContact.lastEvidenceTick = tickMs;
    FeedContactPoint(device, slots[chosen].rawX, slots[chosen].rawY, reportId,
                     collections[chosen], tickMs);
}

void HandleHidInput(const RAWINPUT* raw) {
    const RAWHID& hid = raw->data.hid;
    if (hid.dwSizeHid == 0 || hid.dwCount == 0) {
        return;
    }

    HidDevice* device = GetHidDevice(raw->header.hDevice);
    if (!device) {
        return;
    }

    const uint64_t tickMs = GetTickCount64();
    for (UINT i = 0; i < hid.dwCount; ++i) {
        const BYTE* report =
            hid.bRawData + static_cast<size_t>(i) * hid.dwSizeHid;
        FeedHidReport(*device, report, hid.dwSizeHid, tickMs);
    }
}

void HandleRawInput(HRAWINPUT rawHandle) {
    BYTE stackBuffer[512];
    UINT size = sizeof(stackBuffer);
    SetLastError(0);
    UINT read = GetRawInputData(rawHandle, RID_INPUT, stackBuffer, &size,
                                sizeof(RAWINPUTHEADER));
    BYTE* heapBuffer = nullptr;

    if (read == static_cast<UINT>(-1)) {
        if (size <= sizeof(stackBuffer) || size > 64 * 1024) {
            return;
        }
        heapBuffer = new (std::nothrow) BYTE[size];
        if (!heapBuffer) {
            return;
        }
        UINT heapSize = size;
        read = GetRawInputData(rawHandle, RID_INPUT, heapBuffer, &heapSize,
                               sizeof(RAWINPUTHEADER));
        if (read == static_cast<UINT>(-1)) {
            delete[] heapBuffer;
            return;
        }
    }

    const auto* raw = reinterpret_cast<const RAWINPUT*>(
        heapBuffer ? heapBuffer : stackBuffer);
    if (raw->header.dwType == RIM_TYPEHID) {
        HandleHidInput(raw);
    }

    delete[] heapBuffer;
}

UINT g_taskbarCreatedMessage = 0;

LRESULT CALLBACK HidWindowProc(HWND hwnd, UINT msg, WPARAM wParam,
                               LPARAM lParam) {
    // A restart of the taskbar invalidates the appbar cache; refresh it here
    // rather than blocking on SHAppBarMessage from the next input.
    if (g_taskbarCreatedMessage != 0 && msg == g_taskbarCreatedMessage) {
        TraceLog(L"taskbar recreated; appbar cache refreshed");
        RefreshTaskbarAppBarCache();
        return 0;
    }
    switch (msg) {
        case WM_INPUT:
            HandleRawInput(reinterpret_cast<HRAWINPUT>(lParam));

            return DefWindowProcW(hwnd, msg, wParam, lParam);
        case WM_TIMER:

            if (wParam == kIdleTimerId) {
                ServiceDeadlineWork(GetTickCount64());
            }
            return 0;

        case WM_SETTINGCHANGE:
            ResetHidDpiScale();
            if (wParam == SPI_SETWORKAREA) {
                TraceLog(L"work area changed");
                RefreshTaskbarAppBarCache();
                PostGeometryMeasurement(
                    StartMeasureRequest(!g_hidSession.active &&
                                        !g_reveal.edgeCandidate),
                    false);
            }
            return 0;
        case WM_DISPLAYCHANGE:
            TraceLog(L"work area changed");
            ResetHidDpiScale();
            RefreshTaskbarAppBarCache();
            PostGeometryMeasurement(
                StartMeasureRequest(!g_hidSession.active &&
                                    !g_reveal.edgeCandidate),
                false);
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case kInputWakeMessage:

            return 0;
        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

void RegisterHidUsage(USAGE usage, HWND hwnd) {
    RAWINPUTDEVICE device{};
    device.usUsagePage = kUsagePageDigitizer;
    device.usUsage = usage;
    device.dwFlags = RIDEV_INPUTSINK;
    device.hwndTarget = hwnd;

    SetLastError(0);
    const BOOL ok = RegisterRawInputDevices(&device, 1, sizeof(device));
    const DWORD error = ok ? 0 : GetLastError();
    if (usage == kUsageTouchScreen) {
        g_touchRegistered = ok != FALSE;
    } else if (usage == kUsagePen) {
        g_penRegistered = ok != FALSE;
    }
    TraceLog(L"hid reg usage=%u ok=%d err=%u", static_cast<unsigned>(usage),
            ok ? 1 : 0, static_cast<unsigned>(error));
}

void UnregisterHidUsage(USAGE usage) {
    RAWINPUTDEVICE device{};
    device.usUsagePage = kUsagePageDigitizer;
    device.usUsage = usage;
    device.dwFlags = RIDEV_REMOVE;
    device.hwndTarget = nullptr;
    RegisterRawInputDevices(&device, 1, sizeof(device));
}

void ProbeRootSymbolFromInputThread();
void AttachToRunningStartHost();
std::atomic<bool> g_attachAttempted{false};

void MaybeAttachToRunningStartHost() {
    if (g_unloading.load(std::memory_order_relaxed) ||
        g_attachAttempted.load(std::memory_order_acquire)) {
        return;
    }

    if (!g_rootHookReady.load(std::memory_order_acquire)) {
        return;
    }
    if (g_attachAttempted.exchange(true, std::memory_order_acq_rel)) {
        return;
    }
    AttachToRunningStartHost();
}

[[noreturn]] void BlockUnload(const wchar_t* reason);

extern "C" IMAGE_DOS_HEADER __ImageBase;
void UnregisterHidWindowClass(HINSTANCE instance) {
    if (UnregisterClassW(kHidWindowClass, instance)) {
        TraceLog(L"hid window class unregistered");
        return;
    }
    const DWORD error = GetLastError();
    if (error == ERROR_CLASS_DOES_NOT_EXIST) {
        TraceLog(L"hid window class already gone");
        return;
    }

    BlockUnload(L"hid window class unregister failed");
}

DWORD WINAPI HidInputThread(LPVOID) {
    g_hidThreadId.store(GetCurrentThreadId(), std::memory_order_relaxed);

    RefreshTaskbarAppBarCache();
    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.lpfnWndProc = HidWindowProc;

    windowClass.hInstance = reinterpret_cast<HINSTANCE>(&__ImageBase);
    windowClass.lpszClassName = kHidWindowClass;

    if (!RegisterClassExW(&windowClass)) {

        TraceLog(L"hid window class failed err=%u", GetLastError());
        return 0;
    }

    HWND hwnd = CreateWindowExW(0, kHidWindowClass, L"", 0, 0, 0, 0, 0, nullptr,
                                nullptr, windowClass.hInstance, nullptr);
    if (!hwnd) {
        TraceLog(L"hid window create failed err=%u",
                static_cast<unsigned>(GetLastError()));
        UnregisterHidWindowClass(windowClass.hInstance);
        return 0;
    }

    g_hidHwnd.store(hwnd, std::memory_order_relaxed);

    g_taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");

    RegisterHidUsage(kUsageTouchScreen, hwnd);
    RegisterHidUsage(kUsagePen, hwnd);

    HANDLE deadlineTimer = CreateDeadlineTimer();
    g_highResolutionDeadlineTimer.store(deadlineTimer != nullptr,
                                        std::memory_order_relaxed);
    if (!deadlineTimer) {
        TraceLog(L"hid timer fallback err=%u",
                static_cast<unsigned>(GetLastError()));
    }

    HANDLE cloakEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!cloakEvent) {
        TraceLog(L"hid cloak event failed err=%u",
                static_cast<unsigned>(GetLastError()));
    }
    g_cloakEvent.store(cloakEvent, std::memory_order_relaxed);
    if (cloakEvent) {
        g_cloakHook = SetWinEventHook(
            EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED, nullptr,
            CloakWinEventProc, GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT);
        if (!g_cloakHook) {
            TraceLog(L"hid cloak hook failed err=%u",
                    static_cast<unsigned>(GetLastError()));
        }
    }

    HANDLE waits[8] = {};
    DWORD waitCount = 0;
    if (g_hidStopEvent) {
        waits[waitCount++] = g_hidStopEvent;
    }
    if (deadlineTimer) {
        waits[waitCount++] = deadlineTimer;
    }
    if (g_ipc.intentEvent) {
        waits[waitCount++] = g_ipc.intentEvent;
    }
    if (g_ipc.execAckEvent) {
        waits[waitCount++] = g_ipc.execAckEvent;
    }
    if (g_ipc.capabilityEvent) {
        waits[waitCount++] = g_ipc.capabilityEvent;
    }
    if (g_rendererEvent) {
        waits[waitCount++] = g_rendererEvent;
    }
    if (cloakEvent) {
        waits[waitCount++] = cloakEvent;
    }

    MSG msg;
    bool running = true;

    bool timerArmed = false;
    bool fallbackArmed = false;
    DWORD fallbackPeriod = 0;
    bool timerFailLogged = false;

    bool peerAnnounced = false;
    bool capabilitySeen = BridgeCapabilityNow();
    bool capabilityWaitLogged = false;
    bool rendererWaitLogged = false;

    int rendererWaitRetries = 0;
    bool rendererParkLogged = false;

    bool visibilitySeeded = false;

    ProbeRootSymbolFromInputThread();
    MaybeAttachToRunningStartHost();
    if (g_rootHookReady.load(std::memory_order_acquire) && RendererReady() &&
        capabilitySeen) {
        peerAnnounced = PublishPeerReady(L"consumer");
    }
    while (running) {

        PublishPeerTick(GetTickCount64());
        ProbeRootSymbolFromInputThread();
        MaybeAttachToRunningStartHost();

        const bool capableNow = BridgeCapabilityNow();
        if (capabilitySeen && !capableNow) {
            capabilitySeen = false;
            capabilityWaitLogged = false;
            TraceLog(L"ipc capability lost");
            if (peerAnnounced) {
                SetPeerGone(L"capability-lost", false);
                peerAnnounced = false;
            }
            visibilitySeeded = false;
            ResetDrivenStateForCapabilityLoss();
        } else if (!capabilitySeen && capableNow) {
            capabilitySeen = true;
            capabilityWaitLogged = false;
            TraceLog(L"ipc capability gained");
        }

        if (peerAnnounced && !RendererReady()) {
            SetPeerGone(L"renderer-lost", false);
            peerAnnounced = false;
            visibilitySeeded = false;
        }
        if (!peerAnnounced &&
            g_rootHookReady.load(std::memory_order_acquire) && RendererReady() &&
            capableNow) {
            peerAnnounced = PublishPeerReady(L"consumer");
        }
        if (!peerAnnounced && !capabilityWaitLogged &&
            g_rootHookReady.load(std::memory_order_acquire) && RendererReady() &&
            !capableNow) {

            capabilityWaitLogged = true;
            TraceLog(L"ipc peer withheld reason=no-capability");
        }
        if (!peerAnnounced && !rendererWaitLogged &&
            g_rootHookReady.load(std::memory_order_acquire) &&
            !RendererReady()) {

            rendererWaitLogged = true;
            TraceLog(L"ipc peer withheld reason=no-renderer");
        }
        if (peerAnnounced && !visibilitySeeded &&
            ServiceRootVisibility(GetTickCount64(), false)) {

            visibilitySeeded = true;
        }

        const uint64_t nowTick = GetTickCount64();
        const uint64_t deadlineMs = NextInputDeadlineMs(nowTick, QpcMsNow());
        if (deadlineTimer && deadlineMs != 0) {
            const bool armed = ArmDeadlineTimer(deadlineTimer, deadlineMs);
            if (!armed && !timerFailLogged) {
                timerFailLogged = true;
                TraceLog(L"hid timer arm failed err=%u",
                        static_cast<unsigned>(GetLastError()));
            }
            timerArmed = armed;
        } else if (timerArmed) {
            DisarmDeadlineTimer(deadlineTimer);
            timerArmed = false;
        }

        if (!timerArmed && deadlineMs != 0) {
            const DWORD period = static_cast<DWORD>(
                deadlineMs < 0x7FFFFFFF ? deadlineMs : 0x7FFFFFFF);

            if (!fallbackArmed || fallbackPeriod != period) {
                fallbackArmed =
                    SetTimer(hwnd, kIdleTimerId, period, nullptr) != 0;
                fallbackPeriod = fallbackArmed ? period : 0;
            }
        } else if (fallbackArmed) {
            KillTimer(hwnd, kIdleTimerId);
            fallbackArmed = false;
            fallbackPeriod = 0;
        }

        bool rendererRetry = false;
        if (!g_peerPublished.load(std::memory_order_relaxed) && !g_rendererEvent) {
            const bool rendererPending =
                !peerAnnounced &&
                g_rootHookReady.load(std::memory_order_acquire) &&
                !RendererReady();
            if (rendererPending) {
                if (rendererWaitRetries < kRendererWaitRetries) {
                    ++rendererWaitRetries;
                    rendererRetry = true;
                } else if (!rendererParkLogged) {
                    rendererParkLogged = true;
                    TraceLog(L"ipc peer withheld reason=no-renderer-event");
                }
            }
        }
        DWORD waitMs = (g_peerPublished.load(std::memory_order_relaxed) ||
                        rendererRetry)
                           ? static_cast<DWORD>(kPeerHeartbeatMs)
                           : INFINITE;
        if (deadlineMs != 0 && !timerArmed) {
            const DWORD due = static_cast<DWORD>(
                deadlineMs < 0x7FFFFFFF ? deadlineMs : 0x7FFFFFFF);
            if (waitMs == INFINITE || due < waitMs) {
                waitMs = due;
            }
        }
        const DWORD wait =
            MsgWaitForMultipleObjects(waitCount, waits, FALSE, waitMs,
                                      QS_ALLINPUT);
        if (wait == WAIT_TIMEOUT) {
            if (g_hidStopEvent &&
                WaitForSingleObject(g_hidStopEvent, 0) == WAIT_OBJECT_0) {
                running = false;
                continue;
            }

            ServiceDeadlineWork(GetTickCount64());
            continue;
        }
        if (wait == WAIT_OBJECT_0 + waitCount) {
            while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    running = false;
                    break;
                }
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            if (running && g_hidStopEvent &&
                WaitForSingleObject(g_hidStopEvent, 0) == WAIT_OBJECT_0) {
                running = false;
            }
            continue;
        }
        if (wait >= WAIT_OBJECT_0 && wait < WAIT_OBJECT_0 + waitCount) {
            const HANDLE signalled = waits[wait - WAIT_OBJECT_0];
            if (signalled == g_hidStopEvent) {
                running = false;
                continue;
            }
            if (signalled == deadlineTimer) {
                ServiceDeadlineWork(GetTickCount64());
                continue;
            }
            if (signalled == cloakEvent) {

                ServiceRootVisibility(GetTickCount64(), false);
                continue;
            }
            if (signalled == g_ipc.intentEvent) {
                ApplyBridgeIntent();
                continue;
            }
            if (signalled == g_ipc.execAckEvent) {
                ApplyExecAck();
                continue;
            }
            if (signalled == g_ipc.capabilityEvent) {

                continue;
            }
            if (signalled == g_rendererEvent) {

                if (peerAnnounced && !RendererReady()) {
                    SetPeerGone(L"renderer-lost", false);
                    peerAnnounced = false;
                    visibilitySeeded = false;
                } else if (!peerAnnounced &&
                           g_rootHookReady.load(std::memory_order_acquire) &&
                           RendererReady() && BridgeCapabilityNow()) {
                    peerAnnounced = PublishPeerReady(L"renderer");
                }
                continue;
            }
            continue;
        }
        break;
    }
    if (running) {
        TraceLog(L"hid wait failed err=%u",
                static_cast<unsigned>(GetLastError()));
    }

    if (g_cloakHook) {
        UnhookWinEvent(g_cloakHook);
        g_cloakHook = nullptr;
        TraceLog(L"hid cloak hook removed");
    }
    g_cloakEvent.store(nullptr, std::memory_order_relaxed);
    if (cloakEvent) {
        CloseHandle(cloakEvent);
    }
    KillTimer(hwnd, kIdleTimerId);
    if (deadlineTimer) {
        CancelWaitableTimer(deadlineTimer);
        CloseHandle(deadlineTimer);
    }

    PublishPeerGone(L"consumer-exit");
    if (g_touchRegistered) {
        UnregisterHidUsage(kUsageTouchScreen);
        g_touchRegistered = false;
    }
    if (g_penRegistered) {
        UnregisterHidUsage(kUsagePen);
        g_penRegistered = false;
    }

    ResetActivationState();

    CancelMeasureRequest();
    FreeHidDevices();

    EndContactTiming();
    if (IsWindow(hwnd)) {
        DestroyWindow(hwnd);
    }
    g_hidHwnd.store(nullptr, std::memory_order_relaxed);
    UnregisterHidWindowClass(windowClass.hInstance);
    g_hidThreadId.store(0, std::memory_order_relaxed);
    return 0;
}

float DpiScaleForWindow(HWND hwnd) {
    const UINT dpi = hwnd ? GetDpiForWindow(hwnd) : 0;
    return dpi ? static_cast<float>(dpi) / 96.0f : 1.0f;
}

void StoreGestureGeometry(const GestureGeometry& candidate) {
    AcquireSRWLockExclusive(&g_geometryLock);
    const bool sameLayout = SameLayoutIdentity(g_geometry.layout, candidate.layout);
    GestureGeometry published = candidate;
    if (sameLayout && g_geometry.layoutVersion != 0) {
        published.layoutVersion = g_geometry.layoutVersion;
    } else {

        uint32_t version = g_layoutVersion.load(std::memory_order_acquire);
        if (version <= g_geometry.layoutVersion) {
            version =
                g_layoutVersion.fetch_add(1, std::memory_order_acq_rel) + 1;
        }
        published.layoutVersion = version;
    }
    g_geometry = published;
    ReleaseSRWLockExclusive(&g_geometryLock);
}

void PublishWindowRectDip() {
    HWND hwnd = g_coreHwnd.load(std::memory_order_relaxed);
    if (!hwnd) {
        return;
    }

    RECT rect{};
    if (!GetWindowRect(hwnd, &rect)) {
        return;
    }

    const float scale = DpiScaleForWindow(hwnd);
    g_winRectDip[0].store(
        static_cast<int64_t>(std::llround(rect.left / scale)),
        std::memory_order_relaxed);
    g_winRectDip[1].store(
        static_cast<int64_t>(std::llround(rect.top / scale)),
        std::memory_order_relaxed);
    g_winRectDip[2].store(
        static_cast<int64_t>(std::llround(rect.right / scale)),
        std::memory_order_relaxed);
    g_winRectDip[3].store(
        static_cast<int64_t>(std::llround(rect.bottom / scale)),
        std::memory_order_relaxed);
}

void PublishGestureGeometry(const GestureGeometry& candidate) {
    StoreGestureGeometry(candidate);
    PublishWindowRectDip();
}

bool XamlOriginDip(float& originX, float& originY) {
    HWND hwnd = g_coreHwnd.load(std::memory_order_relaxed);
    if (!hwnd) {
        return false;
    }

    POINT clientOrigin{0, 0};
    if (!ClientToScreen(hwnd, &clientOrigin)) {
        return false;
    }

    const float scale = DpiScaleForWindow(hwnd);
    if (!(scale > 0.0f)) {
        return false;
    }

    originX = static_cast<float>(clientOrigin.x) / scale;
    originY = static_cast<float>(clientOrigin.y) / scale;
    return true;
}

struct PanelMeasurement {
    float left = 0.0f;
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;
};

struct DisplayRectDip {
    float left = 0.0f;
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;
};

SRWLOCK g_taskbarAppBarLock = SRWLOCK_INIT;
bool g_taskbarAppBarValid = false;
uint32_t g_taskbarAppBarEdge = 0;
RECT g_taskbarAppBarRect{};

void RefreshTaskbarAppBarCache() {
    APPBARDATA data{};
    data.cbSize = sizeof(data);
    const bool valid = SHAppBarMessage(ABM_GETTASKBARPOS, &data) != 0;
    const uint32_t edge = valid ? data.uEdge : 0;
    const RECT rect = valid ? data.rc : RECT{};
    AcquireSRWLockExclusive(&g_taskbarAppBarLock);
    g_taskbarAppBarValid = valid;
    g_taskbarAppBarEdge = edge;
    g_taskbarAppBarRect = rect;
    ReleaseSRWLockExclusive(&g_taskbarAppBarLock);
    TraceLog(L"taskbar appbar cache valid=%d edge=%u rect=%ld,%ld,%ld,%ld",
            valid ? 1 : 0, edge, rect.left, rect.top, rect.right, rect.bottom);
}

struct TaskbarProbe {
    bool appBar = false;
    uint32_t appBarEdge = 0;
    bool appBarRect = false;
    RECT appBarRectPx{};
    bool barWindow = false;
    DismissAxis barWindowEdge = DismissAxis::Left;
    RECT barWindowRectPx{};
    bool decided = false;
    DismissAxis edge = DismissAxis::Bottom;
    const wchar_t* source = L"none";
    bool band = false;
    float bandLow = 0.0f;
    float bandHigh = 0.0f;
};

TaskbarProbe g_taskbarProbe;
HMONITOR g_taskbarProbeMonitor = nullptr;

bool g_axisKnown = false;
bool g_axisKeptLogged = false;

bool AxisFromAppBarEdge(uint32_t edge, DismissAxis& axis) {
    switch (edge) {
        case ABE_LEFT:
            axis = DismissAxis::Left;
            return true;
        case ABE_TOP:
            axis = DismissAxis::Top;
            return true;
        case ABE_RIGHT:
            axis = DismissAxis::Right;
            return true;
        case ABE_BOTTOM:
            axis = DismissAxis::Bottom;
            return true;
    }
    return false;
}

bool AxisFromBarRect(const RECT& bar, const RECT& monitor, DismissAxis& axis) {
    const long width = bar.right - bar.left;
    const long height = bar.bottom - bar.top;
    if (!(width > 0) || !(height > 0)) {
        return false;
    }

    constexpr long kRevealSlackPx = 8;

    const auto overlaps = [](long low, long high, long otherLow, long otherHigh) {
        return low < otherHigh && otherLow < high;
    };

    int found = 0;
    long bestDistance = 0;
    DismissAxis best = DismissAxis::Bottom;
    const auto consider = [&](DismissAxis candidate, long distance) {
        if (found == 0 || distance < bestDistance) {
            found = 1;
            bestDistance = distance;
            best = candidate;
        } else if (distance == bestDistance) {
            ++found;
        }
    };

    if (width < height && overlaps(bar.top, bar.bottom, monitor.top,
                                   monitor.bottom)) {
        const long left = std::labs(bar.left - monitor.left);
        const long right = std::labs(monitor.right - bar.right);
        if (left <= width + kRevealSlackPx) {
            consider(DismissAxis::Left, left);
        }
        if (right <= width + kRevealSlackPx) {
            consider(DismissAxis::Right, right);
        }
    }
    if (height < width && overlaps(bar.left, bar.right, monitor.left,
                                   monitor.right)) {
        const long top = std::labs(bar.top - monitor.top);
        const long bottom = std::labs(monitor.bottom - bar.bottom);
        if (top <= height + kRevealSlackPx) {
            consider(DismissAxis::Top, top);
        }
        if (bottom <= height + kRevealSlackPx) {
            consider(DismissAxis::Bottom, bottom);
        }
    }

    if (found != 1) {
        return false;
    }
    axis = best;
    return true;
}

bool BarWindowForMonitor(const RECT& monitor, RECT& bar, DismissAxis& edge) {
    HWND primary = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (primary) {
        RECT rect{};
        if (GetWindowRect(primary, &rect) &&
            AxisFromBarRect(rect, monitor, edge)) {
            bar = rect;
            return true;
        }
    }

    for (HWND current = nullptr;;) {
        current = FindWindowExW(nullptr, current, L"Shell_SecondaryTrayWnd",
                                nullptr);
        if (!current) {
            break;
        }
        RECT rect{};
        if (!GetWindowRect(current, &rect)) {
            continue;
        }
        if (AxisFromBarRect(rect, monitor, edge)) {
            bar = rect;
            return true;
        }
    }
    return false;
}

bool TaskbarBandDip(DismissAxis axis, const RECT& bar, const RECT& monitor,
                    float scale, float& low, float& high) {
    if (!(scale > 0.0f)) {
        return false;
    }

    const float monLeft = static_cast<float>(monitor.left) / scale;
    const float monTop = static_cast<float>(monitor.top) / scale;
    const float monRight = static_cast<float>(monitor.right) / scale;
    const float monBottom = static_cast<float>(monitor.bottom) / scale;
    const float width = static_cast<float>(bar.right - bar.left) / scale;
    const float height = static_cast<float>(bar.bottom - bar.top) / scale;

    switch (axis) {
        case DismissAxis::Left:
            if (!(width > 0.0f) || width >= (monRight - monLeft) / 2.0f) {
                return false;
            }
            low = monLeft;
            high = monLeft + 2.0f * width;
            return true;
        case DismissAxis::Top:
            if (!(height > 0.0f) || height >= (monBottom - monTop) / 2.0f) {
                return false;
            }
            low = monTop;
            high = monTop + 2.0f * height;
            return true;
        case DismissAxis::Right:
            if (!(width > 0.0f) || width >= (monRight - monLeft) / 2.0f) {
                return false;
            }
            low = monRight - 2.0f * width;
            high = monRight;
            return true;
        case DismissAxis::Bottom:
            if (!(height > 0.0f) || height >= (monBottom - monTop) / 2.0f) {
                return false;
            }
            low = monBottom - 2.0f * height;
            high = monBottom;
            return true;
    }
    return false;
}

const wchar_t* TaskbarEdgeName(DismissAxis axis) {
    switch (axis) {
        case DismissAxis::Left:
            return L"left";
        case DismissAxis::Top:
            return L"top";
        case DismissAxis::Right:
            return L"right";
        case DismissAxis::Bottom:
            return L"bottom";
    }
    return L"none";
}

TaskbarProbe ProbeTaskbar(const MONITORINFO& info, float scale) {
    TaskbarProbe probe;
    const RECT& monitorRect = info.rcMonitor;

    if (info.dwFlags & MONITORINFOF_PRIMARY) {
        bool valid = false;
        uint32_t edge = 0;
        RECT rect{};
        AcquireSRWLockShared(&g_taskbarAppBarLock);
        valid = g_taskbarAppBarValid;
        edge = g_taskbarAppBarEdge;
        rect = g_taskbarAppBarRect;
        ReleaseSRWLockShared(&g_taskbarAppBarLock);
        DismissAxis appBarAxis = DismissAxis::Bottom;
        if (valid && AxisFromAppBarEdge(edge, appBarAxis)) {
            probe.appBar = true;
            probe.appBarEdge = edge;
            probe.appBarRect = rect.right > rect.left && rect.bottom > rect.top;
            probe.appBarRectPx = rect;
            probe.decided = true;
            probe.edge = appBarAxis;
            probe.source = L"appbar";
        }
    }

    RECT bar{};
    DismissAxis barAxis = DismissAxis::Bottom;
    bool liveBarWins = false;
    if (BarWindowForMonitor(monitorRect, bar, barAxis)) {
        probe.barWindow = true;
        probe.barWindowEdge = barAxis;
        probe.barWindowRectPx = bar;
        if (!probe.decided) {
            probe.decided = true;
            probe.edge = barAxis;
            probe.source = L"bar";
        } else if (probe.edge != barAxis) {

            TraceLog(L"taskbar edge appbar=%s bar=%s (live bar wins)",
                    TaskbarEdgeName(probe.edge), TaskbarEdgeName(barAxis));
            probe.edge = barAxis;
            probe.source = L"bar";
            liveBarWins = true;
        }
    }

    const RECT* bandRect = nullptr;
    if (liveBarWins) {
        bandRect = &probe.barWindowRectPx;
    } else if (probe.appBarRect) {
        bandRect = &probe.appBarRectPx;
    } else if (probe.barWindow) {
        bandRect = &probe.barWindowRectPx;
    }
    if (probe.decided && bandRect) {
        probe.band = TaskbarBandDip(probe.edge, *bandRect, monitorRect, scale,
                                    probe.bandLow, probe.bandHigh);
    }

    if (!probe.decided) {

        const long insetLeft = info.rcWork.left - info.rcMonitor.left;
        const long insetTop = info.rcWork.top - info.rcMonitor.top;
        const long insetRight = info.rcMonitor.right - info.rcWork.right;
        const long insetBottom = info.rcMonitor.bottom - info.rcWork.bottom;
        const long largest = std::max(std::max(insetLeft, insetTop),
                                      std::max(insetRight, insetBottom));
        if (largest > 4) {
            probe.decided = true;
            probe.source = L"work";
            probe.edge = largest == insetLeft
                             ? DismissAxis::Left
                             : (largest == insetTop
                                    ? DismissAxis::Top
                                    : (largest == insetRight ? DismissAxis::Right
                                                             : DismissAxis::Bottom));
        }
    }

    return probe;
}

struct PanelHomeDip {
    DisplayRectDip work;
    DisplayRectDip monitor;
    bool taskbarBand = false;
    float taskbarLow = 0.0f;
    float taskbarHigh = 0.0f;
};

bool WorkAreaBandDip(const PanelHomeDip& home, int32_t axis, float& low,
                     float& high) {
    float inset = 0.0f;
    switch (static_cast<DismissAxis>(axis)) {
        case DismissAxis::Left:
            inset = home.work.left - home.monitor.left;
            break;
        case DismissAxis::Top:
            inset = home.work.top - home.monitor.top;
            break;
        case DismissAxis::Right:
            inset = home.monitor.right - home.work.right;
            break;
        case DismissAxis::Bottom:
            inset = home.monitor.bottom - home.work.bottom;
            break;
    }
    if (!(inset > 0.0f)) {
        return false;
    }
    switch (static_cast<DismissAxis>(axis)) {
        case DismissAxis::Left:
            low = home.monitor.left;
            high = home.monitor.left + 2.0f * inset;
            break;
        case DismissAxis::Top:
            low = home.monitor.top;
            high = home.monitor.top + 2.0f * inset;
            break;
        case DismissAxis::Right:
            low = home.monitor.right - 2.0f * inset;
            high = home.monitor.right;
            break;
        case DismissAxis::Bottom:
            low = home.monitor.bottom - 2.0f * inset;
            high = home.monitor.bottom;
            break;
    }
    return true;
}

bool MeasurePanelRect(PanelMeasurement& panel) {
    wux::FrameworkElement root = g_root;
    HWND hwnd = g_coreHwnd.load(std::memory_order_relaxed);
    if (!root || !hwnd) {
        return false;
    }

    float frameLeft = 0.0f;
    float frameTop = 0.0f;
    float frameRight = 0.0f;
    float frameBottom = 0.0f;
    bool measured = false;
    float bestArea = -1.0f;
    bool haveNonFilling = false;
    float nonFillingArea = -1.0f;
    float nfLeft = 0.0f, nfTop = 0.0f, nfRight = 0.0f, nfBottom = 0.0f;

    const float rootWidth = static_cast<float>(root.ActualWidth());
    const float rootHeight = static_cast<float>(root.ActualHeight());

    const int32_t childCount =
        wux::Media::VisualTreeHelper::GetChildrenCount(root);
    for (int32_t i = 0; i < childCount; ++i) {
        wux::FrameworkElement child =
            wux::Media::VisualTreeHelper::GetChild(root, i)
                .try_as<wux::FrameworkElement>();
        if (!child) {
            continue;
        }
        const float width = static_cast<float>(child.ActualWidth());
        const float height = static_cast<float>(child.ActualHeight());
        if (!(width > 0.0f) || !(height > 0.0f) ||
            child.Visibility() != wux::Visibility::Visible) {
            continue;
        }

        wux::Media::GeneralTransform transform = child.TransformToVisual(nullptr);
        const winrt::Windows::Foundation::Point topLeft =
            transform.TransformPoint({0.0f, 0.0f});
        const float left = topLeft.X;
        const float top = topLeft.Y;
        const float right = transform.TransformPoint({width, 0.0f}).X;
        const float bottom = transform.TransformPoint({0.0f, height}).Y;

        const float overlapWidth =
            std::max(0.0f, std::min(right, rootWidth) - std::max(left, 0.0f));
        const float overlapHeight =
            std::max(0.0f, std::min(bottom, rootHeight) - std::max(top, 0.0f));
        const float area = overlapWidth * overlapHeight;
        if (!(area > 0.0f)) {
            continue;
        }

        const float rootArea = rootWidth * rootHeight;
        if (rootArea <= 0.0f || area < 0.98f * rootArea) {
            if (area > nonFillingArea) {
                nonFillingArea = area;
                nfLeft = left;
                nfTop = top;
                nfRight = right;
                nfBottom = bottom;
                haveNonFilling = true;
            }
        }
        if (area > bestArea) {
            bestArea = area;
            frameLeft = left;
            frameTop = top;
            frameRight = right;
            frameBottom = bottom;
            measured = true;
        }
    }

    if (haveNonFilling) {
        frameLeft = nfLeft;
        frameTop = nfTop;
        frameRight = nfRight;
        frameBottom = nfBottom;
    }

    if (!measured) {
        return false;
    }

    float originX = 0.0f;
    float originY = 0.0f;
    if (!XamlOriginDip(originX, originY)) {
        return false;
    }

    panel.left = originX + frameLeft;
    panel.top = originY + frameTop;
    panel.right = originX + frameRight;
    panel.bottom = originY + frameBottom;
    return true;
}

bool PanelHomeForWindow(PanelHomeDip& home) {
    HWND hwnd = g_coreHwnd.load(std::memory_order_relaxed);
    if (!hwnd) {
        return false;
    }

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (!monitor || !GetMonitorInfoW(monitor, &info)) {
        return false;
    }

    const float scale = DpiScaleForWindow(hwnd);
    if (!(scale > 0.0f)) {
        return false;
    }

    home.work.left = info.rcWork.left / scale;
    home.work.top = info.rcWork.top / scale;
    home.work.right = info.rcWork.right / scale;
    home.work.bottom = info.rcWork.bottom / scale;
    home.monitor.left = info.rcMonitor.left / scale;
    home.monitor.top = info.rcMonitor.top / scale;
    home.monitor.right = info.rcMonitor.right / scale;
    home.monitor.bottom = info.rcMonitor.bottom / scale;

    if (g_taskbarProbeMonitor == monitor && g_taskbarProbe.band) {
        home.taskbarBand = true;
        home.taskbarLow = g_taskbarProbe.bandLow;
        home.taskbarHigh = g_taskbarProbe.bandHigh;
    }
    return true;
}

bool PanelRectAccepted(const PanelMeasurement& panel,
                       const PanelHomeDip& home, int32_t axis) {
    if (!(panel.right > panel.left) || !(panel.bottom > panel.top)) {
        return false;
    }
    if (panel.left < home.monitor.left - kPanelLayoutMarginDip ||
        panel.right > home.monitor.right + kPanelLayoutMarginDip ||
        panel.top < home.monitor.top - kPanelLayoutMarginDip ||
        panel.bottom > home.monitor.bottom + kPanelLayoutMarginDip) {
        return false;
    }

    float edge = 0.0f;
    float low = 0.0f;
    float high = 0.0f;
    switch (static_cast<DismissAxis>(axis)) {
        case DismissAxis::Left:
            edge = panel.left;
            low = home.work.left;
            high = home.monitor.left;
            break;
        case DismissAxis::Top:
            edge = panel.top;
            low = home.work.top;
            high = home.monitor.top;
            break;
        case DismissAxis::Right:
            edge = panel.right;
            low = home.work.right;
            high = home.monitor.right;
            break;
        case DismissAxis::Bottom:
            edge = panel.bottom;
            low = home.work.bottom;
            high = home.monitor.bottom;
            break;
        default:
            return false;
    }
    if (home.taskbarBand) {
        low = home.taskbarLow;
        high = home.taskbarHigh;
    }

    const float from = std::min(low, high);
    const float to = std::max(low, high);
    return edge >= from - kPanelAnchorMarginDip &&
           edge <= to + kPanelAnchorMarginDip;
}

bool PublishPanelTrailDip(bool measurePanel, uint32_t expectedGeneration,
                          uint64_t requestId) {
    if (g_unloading.load(std::memory_order_relaxed)) {
        return false;
    }
    if (!(g_dismissDistance > 0.0f)) {
        return false;
    }

    const uint32_t generation = g_openGeneration.load(std::memory_order_relaxed);
    if (expectedGeneration != 0 && generation != expectedGeneration) {
        return false;
    }

    GestureGeometry previous;
    AcquireSRWLockShared(&g_geometryLock);
    previous = g_geometry;
    ReleaseSRWLockShared(&g_geometryLock);

    GestureGeometry candidate;
    candidate.generation = generation;
    candidate.layout.axis = g_dismissAxis;
    candidate.layout.distance = g_dismissDistance;

    float originX = 0.0f;
    float originY = 0.0f;
    const bool haveOrigin = XamlOriginDip(originX, originY);
    if (haveOrigin) {
        candidate.layout.originX = originX;
        candidate.layout.originY = originY;
    }

    PanelHomeDip home;
    const bool haveHome = PanelHomeForWindow(home);
    if (haveHome) {
        candidate.layout.workLeft = home.work.left;
        candidate.layout.workTop = home.work.top;
        candidate.layout.workRight = home.work.right;
        candidate.layout.workBottom = home.work.bottom;
        candidate.layout.monitorLeft = home.monitor.left;
        candidate.layout.monitorTop = home.monitor.top;
        candidate.layout.monitorRight = home.monitor.right;
        candidate.layout.monitorBottom = home.monitor.bottom;

        if (home.taskbarBand) {
            candidate.band = true;
            candidate.bandLow = home.taskbarLow;
            candidate.bandHigh = home.taskbarHigh;
        } else {
            candidate.band = WorkAreaBandDip(home, candidate.layout.axis,
                                             candidate.bandLow,
                                             candidate.bandHigh);
        }
    }

    bool carriedPanel = false;
    PanelMeasurement carried;
    if (previous.panel && haveHome && haveOrigin &&
        SameLayoutIdentity(previous.layout, candidate.layout)) {
        carried.left = previous.left;
        carried.top = previous.top;
        carried.right = previous.right;
        carried.bottom = previous.bottom;
        if (PanelRectAccepted(carried, home, candidate.layout.axis)) {
            carriedPanel = true;
            candidate.panel = true;
            candidate.left = previous.left;
            candidate.top = previous.top;
            candidate.right = previous.right;
            candidate.bottom = previous.bottom;
        }
    }

    bool measuredPanel = false;
    if (measurePanel) {
        PanelMeasurement panel;
        bool havePanel = false;
        try {
            havePanel = MeasurePanelRect(panel);
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"Panel rect mapping failed: %08X", e.code());
        } catch (...) {

        }

        if (havePanel && haveHome) {
            if (PanelRectAccepted(panel, home, candidate.layout.axis)) {
                candidate.panel = true;
                candidate.left = panel.left;
                candidate.top = panel.top;
                candidate.right = panel.right;
                candidate.bottom = panel.bottom;
                measuredPanel = true;
            } else {

                const uint32_t openGeneration =
                    g_openGeneration.load(std::memory_order_relaxed);
                if (expectedGeneration == 0 ||
                    expectedGeneration == openGeneration) {
                    const uint32_t rejectedGeneration =
                        expectedGeneration != 0 ? expectedGeneration
                                                : openGeneration;
                    if (!g_panelTrailRejectedLogged ||
                        g_panelTrailRejectedGeneration != rejectedGeneration) {
                        g_panelTrailRejectedLogged = true;
                        g_panelTrailRejectedGeneration = rejectedGeneration;
                        TraceLog(
                            L"reveal panelTrail rejected %.0f axis=%d "
                            L"rect=%.0f,%.0f,%.0f,%.0f work=%.0f,%.0f,%.0f,%.0f "
                            L"mon=%.0f,%.0f,%.0f,%.0f band=%.0f,%.0f",
                            TrailEdge(candidate.layout.axis, panel.left,
                                      panel.top, panel.right, panel.bottom),
                            candidate.layout.axis, panel.left, panel.top,
                            panel.right, panel.bottom, home.work.left,
                            home.work.top, home.work.right, home.work.bottom,
                            home.monitor.left, home.monitor.top,
                            home.monitor.right, home.monitor.bottom,
                            home.taskbarBand ? home.taskbarLow : -1.0f,
                            home.taskbarBand ? home.taskbarHigh : -1.0f);
                    }
                }
            }
        }
    }

    {
        std::lock_guard lock(g_measureMutex);
        if (g_unloading.load(std::memory_order_relaxed)) {
            return false;
        }
        if (expectedGeneration != 0 &&
            g_openGeneration.load(std::memory_order_relaxed) !=
                expectedGeneration) {
            return false;
        }
        if (requestId != 0 &&
            !MeasureRequestLiveLocked(expectedGeneration, requestId)) {
            return false;
        }
        if (g_dismissAxis != candidate.layout.axis ||
            !SameLayoutDistance(g_dismissDistance, candidate.layout.distance)) {
            return false;
        }
        if (g_panelOwned.load(std::memory_order_relaxed) && measuredPanel) {

            measuredPanel = false;
            candidate.panel = carriedPanel;
            if (carriedPanel) {
                candidate.left = carried.left;
                candidate.top = carried.top;
                candidate.right = carried.right;
                candidate.bottom = carried.bottom;
            }
        }

        candidate.generation = g_openGeneration.load(std::memory_order_relaxed);
        StoreGestureGeometry(candidate);
    }

    PublishWindowRectDip();

    if (candidate.panel) {
        const float trail = GeometryTrailDip(candidate);
        if (measuredPanel) {

            const float previousTrail =
                previous.panel ? GeometryTrailDip(previous) : -1.0f;
            const bool moved = !previous.panel ||
                               std::fabs(trail - previousTrail) > 1.0f;
            if (moved || !g_panelTrailLogged) {
                g_panelTrailLogged = true;
                TraceLog(L"reveal panelTrail=%.0f axis=%d rect=%.0f,%.0f,%.0f,%.0f "
                        L"work=%.0f,%.0f,%.0f,%.0f band=%.0f,%.0f gen=%u",
                        trail, candidate.layout.axis, candidate.left,
                        candidate.top, candidate.right, candidate.bottom,
                        home.work.left, home.work.top, home.work.right,
                        home.work.bottom,
                        home.taskbarBand ? home.taskbarLow : -1.0f,
                        home.taskbarBand ? home.taskbarHigh : -1.0f,
                        candidate.generation);
            }
        }
        return true;
    }

    if (!g_panelTrailFallbackLogged) {
        g_panelTrailFallbackLogged = true;
        TraceLog(L"reveal panelTrail=%.0f axis=%d (fallback)",
                FallbackTrailDip(candidate.layout.axis,
                                 candidate.layout.distance),
                candidate.layout.axis);
    }
    return true;
}

SettledMeasureRequest g_layoutRequest{};

void StopSettledTrailWatch() {
    wux::FrameworkElement root = g_layoutRoot ? g_layoutRoot : g_root;
    if (g_layoutToken.value) {
        if (!root) {
            Wh_Log(L"LayoutUpdated registration has no owner to revoke on");
            return;
        }
        try {
            root.LayoutUpdated(g_layoutToken);
            g_uiRegistrationCount.fetch_sub(1, std::memory_order_relaxed);
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"LayoutUpdated revoke failed: %08X", e.code());

            return;
        } catch (...) {
            Wh_Log(L"LayoutUpdated revoke failed with an unknown exception");
            return;
        }
    }
    g_layoutToken = {};
    g_layoutRoot = nullptr;
    g_layoutTicksLeft = 0;
    g_layoutWatchSerial = 0;
    g_layoutRequest = SettledMeasureRequest{};
}

void StopSettledTrailWatchIfGeneration(uint32_t maxGeneration) {
    if (g_layoutTicksLeft != 0 && g_layoutRequest.generation > maxGeneration) {
        return;
    }
    StopSettledTrailWatch();
}

void ArmLayoutWatch(const SettledMeasureRequest& request) {
    wux::FrameworkElement root = g_root;
    if (!root) {
        return;
    }

    StopSettledTrailWatch();
    if (g_layoutToken.value) {
        Wh_Log(L"LayoutUpdated registration survived its revoke; this open gets "
               L"no layout watch");
        TraceLog(L"xaml layout handler left attached; new registration refused");
        return;
    }
    g_layoutTicksLeft = 2;
    g_layoutRequest = request;
    g_layoutRoot = root;

    uint32_t watchSerial = ++g_layoutWatchMint;
    if (watchSerial == 0) {
        watchSerial = ++g_layoutWatchMint;
    }

    g_layoutWatchSerial = watchSerial;
    try {
        g_layoutToken = root.LayoutUpdated(
            [watchSerial, request](
                winrt::Windows::Foundation::IInspectable const&,
                winrt::Windows::Foundation::IInspectable const&) {
                UiDepthGuard depth;
                try {
                    if (watchSerial != g_layoutWatchSerial) {
                        return;
                    }
                    if (g_layoutTicksLeft == 0) {
                        return;
                    }
                    if (--g_layoutTicksLeft > 0) {
                        return;
                    }
                    RunGeometryMeasurement(request, true);
                } catch (winrt::hresult_error const& e) {
                    Wh_Log(L"Settled trail failed: %08X", e.code());
                } catch (...) {

                }

                if (watchSerial == g_layoutWatchSerial) {
                    StopSettledTrailWatch();
                }
            });
        if (g_layoutToken.value) {
            g_uiRegistrationCount.fetch_add(1, std::memory_order_relaxed);
        } else {
            g_layoutWatchSerial = 0;
            g_layoutRoot = nullptr;
            g_layoutTicksLeft = 0;
            g_layoutRequest = SettledMeasureRequest{};
        }
    } catch (winrt::hresult_error const& e) {
        g_layoutToken = {};
        g_layoutWatchSerial = 0;
        g_layoutRoot = nullptr;
        g_layoutTicksLeft = 0;
        g_layoutRequest = SettledMeasureRequest{};
        TraceLog(L"layout watch failed err=%08X", static_cast<unsigned>(e.code()));
    } catch (...) {
        g_layoutToken = {};
        g_layoutWatchSerial = 0;
        g_layoutRoot = nullptr;
        g_layoutTicksLeft = 0;
        g_layoutRequest = SettledMeasureRequest{};
        TraceLog(L"layout watch failed err=unknown");
    }
}

struct ScrollerFound {
    wuxc::ScrollViewer viewer{nullptr};
    float left = 0.0f;
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;
    float area = 0.0f;
    uint32_t depth = 0;
    wchar_t label[kScrollerLabelChars] = {};
};

struct ScrollerSurvey {
    ScrollerFound found[kMaxScrollRegions];
    int count = 0;

    bool complete = true;

    bool overflow = false;
};

void CopyScrollerLabel(const wchar_t* text, wchar_t (&label)[kScrollerLabelChars]) {
    const size_t length = text ? std::min(wcslen(text),
                                          static_cast<size_t>(kScrollerLabelChars - 1))
                               : 0;
    if (length > 0) {
        std::wmemcpy(label, text, length);
    }
    label[length] = L'\0';
}

bool GenericContainerName(const winrt::hstring& name) {
    static const wchar_t* const kGeneric[] = {
        L"Windows.UI.Xaml.Controls.ScrollViewer",
        L"Windows.UI.Xaml.Controls.Border",
        L"Windows.UI.Xaml.Controls.Grid",
        L"Windows.UI.Xaml.Controls.ContentPresenter",
        L"Windows.UI.Xaml.Controls.ScrollContentPresenter",
        L"Windows.UI.Xaml.Controls.ItemsPresenter",
        L"Windows.UI.Xaml.Controls.StackPanel",
        L"ScrollViewer",
        L"Border",
        L"Grid",
        L"ContentPresenter",
        L"ScrollContentPresenter",
        L"ItemsPresenter",
        L"StackPanel",
    };
    for (const wchar_t* generic : kGeneric) {
        if (name == generic) {
            return true;
        }
    }
    return false;
}

winrt::hstring ShortTypeName(const winrt::hstring& type) {
    const std::wstring_view view = type;
    const size_t dot = view.rfind(L'.');
    if (dot == std::wstring_view::npos) {
        return type;
    }
    return winrt::hstring(view.substr(dot + 1));
}

bool MeaningfulElementName(const winrt::hstring& name,
                           const winrt::hstring& type) {
    if (name.empty() || GenericContainerName(name)) {
        return false;
    }
    return name != type && name != ShortTypeName(type);
}

void DescribeScrollerElement(const wux::UIElement& element,
                             wchar_t (&label)[kScrollerLabelChars]) {
    winrt::hstring elementType;
    try {
        elementType = winrt::get_class_name(element);
    } catch (...) {
    }

    winrt::hstring ownName;
    try {
        wux::FrameworkElement named = element.try_as<wux::FrameworkElement>();
        if (named && MeaningfulElementName(named.Name(), elementType)) {
            ownName = named.Name();
        }
    } catch (...) {
    }

    winrt::hstring ownerType;
    wux::DependencyObject ancestor = element;
    for (int level = 0; level < 12 && ancestor; ++level) {
        try {
            ancestor = wux::Media::VisualTreeHelper::GetParent(ancestor);
        } catch (...) {
            break;
        }
        if (!ancestor) {
            break;
        }
        try {
            const winrt::hstring type = winrt::get_class_name(ancestor);
            if (ownerType.empty() && !GenericContainerName(type) &&
                !GenericContainerName(ShortTypeName(type))) {
                ownerType = ShortTypeName(type);
            }
        } catch (...) {
        }
    }

    wchar_t text[kScrollerLabelChars * 2] = {};
    if (!ownName.empty() && !ownerType.empty()) {
        _snwprintf_s(text, ARRAYSIZE(text), _TRUNCATE, L"%s>%s",
                     ownName.c_str(), ownerType.c_str());
    } else if (!ownName.empty()) {
        _snwprintf_s(text, ARRAYSIZE(text), _TRUNCATE, L"%s", ownName.c_str());
    } else if (!ownerType.empty()) {
        _snwprintf_s(text, ARRAYSIZE(text), _TRUNCATE, L"%s",
                     ownerType.c_str());
    } else if (!elementType.empty()) {
        _snwprintf_s(text, ARRAYSIZE(text), _TRUNCATE, L"%s",
                     ShortTypeName(elementType).c_str());
    } else {
        _snwprintf_s(text, ARRAYSIZE(text), _TRUNCATE, L"ScrollViewer");
    }
    CopyScrollerLabel(text, label);
}

void AddScrollerFound(ScrollerSurvey& survey, const ScrollerFound& found) {
    if (survey.count < kMaxScrollRegions) {
        survey.found[survey.count++] = found;
        return;
    }
    survey.overflow = true;
    int shallowest = 0;
    for (int i = 1; i < survey.count; ++i) {
        if (survey.found[i].depth < survey.found[shallowest].depth) {
            shallowest = i;
        }
    }
    if (found.depth > survey.found[shallowest].depth) {
        survey.found[shallowest] = found;
    }
}

void CollectScrollableElements(const wux::UIElement& element, uint32_t depth,
                               float originX, float originY,
                               ScrollerSurvey& survey) {
    if (!element) {
        return;
    }
    if (depth > kScrollerScanMaxDepth) {
        survey.complete = false;
        return;
    }

    try {
        wuxc::ScrollViewer viewer = element.try_as<wuxc::ScrollViewer>();
        if (viewer) {
            const winrt::Windows::Foundation::Size size = element.ActualSize();
            const wux::Media::GeneralTransform transform =
                element.TransformToVisual(nullptr);
            const winrt::Windows::Foundation::Point topLeft =
                transform.TransformPoint({0.0f, 0.0f});
            const winrt::Windows::Foundation::Point bottomRight =
                transform.TransformPoint({size.Width, size.Height});

            const float left = originX + topLeft.X;
            const float top = originY + topLeft.Y;
            const float right = originX + bottomRight.X;
            const float bottom = originY + bottomRight.Y;
            const float area = (right - left) * (bottom - top);

            if (viewer.ScrollableWidth() <= kScrollerAxisSlopDip &&
                viewer.ScrollableHeight() > kScrollerAxisSlopDip &&
                right > left && bottom > top && area > 1.0f) {
                ScrollerFound found;
                found.viewer = viewer;
                found.left = left;
                found.top = top;
                found.right = right;
                found.bottom = bottom;
                found.area = area;
                found.depth = depth;
                DescribeScrollerElement(element, found.label);
                AddScrollerFound(survey, found);
            }
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Scroller measure failed: %08X", e.code());
        survey.complete = false;
    } catch (...) {

        survey.complete = false;
    }

    try {
        const int32_t count =
            wux::Media::VisualTreeHelper::GetChildrenCount(element);
        if (depth >= kScrollerScanMaxDepth && count > 0) {
            survey.complete = false;
            return;
        }
        for (int32_t i = 0; i < count; ++i) {
            CollectScrollableElements(
                wux::Media::VisualTreeHelper::GetChild(element, i)
                    .try_as<wux::UIElement>(),
                depth + 1, originX, originY, survey);
        }
    } catch (...) {

        survey.complete = false;
    }
}

void RevokeScrollerWatch(int slot) {
    if (slot < 0 || slot >= kMaxScrollRegions) {
        return;
    }
    ScrollerWatch& watch = ScrollerWatches()[slot];
    wuxc::ScrollViewer owner = watch.owner;
    if (watch.token.value) {
        if (!owner) {
            Wh_Log(L"ViewChanged registration has no owner to revoke on");
            return;
        }
        try {
            owner.ViewChanged(watch.token);
            g_uiRegistrationCount.fetch_sub(1, std::memory_order_relaxed);
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"ViewChanged revoke failed: %08X", e.code());
            return;
        } catch (...) {
            Wh_Log(L"ViewChanged revoke failed with an unknown exception");
            return;
        }
    }
    watch.token = {};
    watch.owner = nullptr;
}

void RefreshScrollerOffsets(wuxc::ScrollViewer const& watched) {
    if (!watched) {
        return;
    }

    int slot = -1;
    for (int i = 0; i < kMaxScrollRegions; ++i) {
        if (ScrollerWatches()[i].viewer == watched) {
            slot = i;
            break;
        }
    }
    if (slot < 0) {
        return;
    }

    try {
        const float verticalOffset = watched.VerticalOffset();
        const float scrollableHeight = watched.ScrollableHeight();
        std::lock_guard lock(g_scrollerMutex);
        if (g_scroller.state != ScrollerState::Scrollable ||
            g_scroller.generation !=
                g_openGeneration.load(std::memory_order_relaxed) ||
            g_scroller.layoutVersion !=
                g_layoutVersion.load(std::memory_order_acquire)) {
            return;
        }
        for (int i = 0; i < g_scroller.count; ++i) {
            if (g_scroller.regions[i].watchSlot == slot) {
                g_scroller.regions[i].verticalOffset = verticalOffset;
                g_scroller.regions[i].scrollableHeight = scrollableHeight;
            }
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Scroller offset read failed: %08X", e.code());
    } catch (...) {

    }
}

enum class ScrollerCommitState {
    Commit = 0,
    Obsolete = 1,
    Owned = 2,
    Moved = 3,
};

ScrollerCommitState CommitScrollerCache(const ScrollerCache& answer,
                                        uint32_t expectedGeneration,
                                        uint64_t requestId,
                                        uint32_t layoutVersion) {
    std::lock_guard lock(g_measureMutex);
    if (g_unloading.load(std::memory_order_relaxed)) {
        return ScrollerCommitState::Obsolete;
    }
    if (expectedGeneration != 0 &&
        g_openGeneration.load(std::memory_order_relaxed) != expectedGeneration) {
        return ScrollerCommitState::Obsolete;
    }
    if (requestId != 0 &&
        !MeasureRequestLiveLocked(expectedGeneration, requestId)) {
        return ScrollerCommitState::Obsolete;
    }
    if (g_panelOwned.load(std::memory_order_relaxed)) {
        return ScrollerCommitState::Owned;
    }
    if (g_layoutVersion.load(std::memory_order_acquire) != layoutVersion) {
        return ScrollerCommitState::Moved;
    }
    std::lock_guard scrollerLock(g_scrollerMutex);
    g_scroller = answer;
    return ScrollerCommitState::Commit;
}

uint32_t g_scrollerRetryCount = 0;
uint32_t g_scrollerRetryGenerationScheduled = 0;
bool g_scrollerRetryExhaustedLogged = false;

void ScheduleScrollerRetry(uint32_t generation) {
    if (generation == 0) {
        return;
    }
    if (generation != g_scrollerRetryGenerationScheduled) {
        g_scrollerRetryGenerationScheduled = generation;
        g_scrollerRetryCount = 0;
        g_scrollerRetryExhaustedLogged = false;
    }
    if (g_scrollerRetryCount >= kScrollerRetryLimit) {
        g_scrollerRetryAt.store(0, std::memory_order_relaxed);
        g_scrollerRetryGeneration.store(generation, std::memory_order_relaxed);
        if (!g_scrollerRetryExhaustedLogged) {
            g_scrollerRetryExhaustedLogged = true;
            TraceLog(L"scroller retry exhausted gen=%u", generation);
        }
        return;
    }
    const uint32_t attempt = g_scrollerRetryCount++;
    uint64_t delay = kScrollerRetryMs << std::min(attempt, 2u);
    if (delay > kScrollerRetryBackoffMaxMs) {
        delay = kScrollerRetryBackoffMaxMs;
    }
    g_scrollerRetryGeneration.store(generation, std::memory_order_relaxed);
    g_scrollerRetryAt.store(GetTickCount64() + delay, std::memory_order_relaxed);

    if (!g_unloading.load(std::memory_order_relaxed)) {
        HWND hwnd = g_hidHwnd.load(std::memory_order_relaxed);
        if (hwnd) {
            PostMessageW(hwnd, kInputWakeMessage, 0, 0);
        }
    }
}

bool g_scrollerRetryLogged = false;

void NoteScrollerFailure(const wchar_t* reason, uint32_t generation,
                         bool retry) {
    if (!g_scrollerRetryLogged) {
        g_scrollerRetryLogged = true;
        TraceLog(L"scroller retry why=%s gen=%u try=%u/%u", reason, generation,
                static_cast<unsigned>(g_scrollerRetryCount + 1),
                static_cast<unsigned>(kScrollerRetryLimit));
    }
    if (g_panelOwned.load(std::memory_order_relaxed)) {
        g_measureDeferred.store(true, std::memory_order_relaxed);
        return;
    }
    if (retry) {
        ScheduleScrollerRetry(generation);
    }
}

void ClearScrollerRetry() {
    g_scrollerRetryAt.store(0, std::memory_order_relaxed);
}

void PublishScrollerUnknown(uint32_t generation) {

    std::lock_guard lock(g_scrollerMutex);
    g_scroller = ScrollerCache{};
    g_scroller.generation = generation;
    g_scroller.layoutVersion =
        g_layoutVersion.load(std::memory_order_acquire);
}

void CacheScrollableElement(uint32_t expectedGeneration, uint64_t requestId) {
    wux::FrameworkElement root = g_root;
    if (!root) {
        return;
    }

    float originX = 0.0f;
    float originY = 0.0f;
    if (!XamlOriginDip(originX, originY)) {
        PublishScrollerUnknown(expectedGeneration);
        NoteScrollerFailure(L"origin", expectedGeneration, true);
        return;
    }

    const uint32_t layoutVersion = g_layoutVersion.load(std::memory_order_acquire);

    {
        std::lock_guard lock(g_scrollerMutex);
        g_scroller = ScrollerCache{};
    }

    ScrollerSurvey survey;
    try {
        CollectScrollableElements(root, 0, originX, originY, survey);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Scroller search failed: %08X", e.code());
        survey.complete = false;
    } catch (...) {

        survey.complete = false;
    }

    if (survey.overflow && !g_scrollerOverflowLogged) {
        g_scrollerOverflowLogged = true;
        TraceLog(L"scroller overflow (kept %d deepest regions)",
                kMaxScrollRegions);
    }
    if (survey.count == 0 && !survey.complete) {

        PublishScrollerUnknown(expectedGeneration);
        NoteScrollerFailure(L"incomplete", expectedGeneration, true);
        return;
    }

    if (survey.count == 0) {

        ScrollerCache answer;
        answer.state = ScrollerState::Nothing;
        answer.generation = expectedGeneration;
        answer.layoutVersion = layoutVersion;
        switch (CommitScrollerCache(answer, expectedGeneration, requestId,
                                    layoutVersion)) {
            case ScrollerCommitState::Commit:
                break;
            case ScrollerCommitState::Obsolete:
                return;
            case ScrollerCommitState::Owned:
                PublishScrollerUnknown(expectedGeneration);
                NoteScrollerFailure(L"not-allowed", expectedGeneration, false);
                return;
            case ScrollerCommitState::Moved:
                PublishScrollerUnknown(expectedGeneration);
                NoteScrollerFailure(L"commit", expectedGeneration, true);
                return;
        }

        for (int i = 0; i < kMaxScrollRegions; ++i) {
            RevokeScrollerWatch(i);
        }
        if (!g_scrollerNoneLogged) {
            g_scrollerNoneLogged = true;
            TraceLog(L"scroller none (measured, nothing scrolls)");
        }
        return;
    }

    ScrollerCache answer;
    answer.state = ScrollerState::Scrollable;
    answer.generation = expectedGeneration;
    answer.layoutVersion = layoutVersion;
    wuxc::ScrollViewer answerViewers[kMaxScrollRegions]{
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    for (int i = 0; i < survey.count && answer.count < kMaxScrollRegions; ++i) {
        const ScrollerFound& found = survey.found[i];
        float verticalOffset = 0.0f;
        float scrollableHeight = 0.0f;
        try {
            verticalOffset = found.viewer.VerticalOffset();
            scrollableHeight = found.viewer.ScrollableHeight();
        } catch (...) {
            continue;
        }
        ScrollerRegion& region = answer.regions[answer.count];
        answerViewers[answer.count] = found.viewer;
        answer.count++;
        region.left = found.left;
        region.top = found.top;
        region.right = found.right;
        region.bottom = found.bottom;
        region.verticalOffset = verticalOffset;
        region.scrollableHeight = scrollableHeight;
        region.depth = found.depth;
        std::wmemcpy(region.label, found.label, kScrollerLabelChars);
    }

    if (answer.count == 0) {

        PublishScrollerUnknown(expectedGeneration);
        NoteScrollerFailure(L"offsets", expectedGeneration, true);
        return;
    }

    float commitOriginX = 0.0f;
    float commitOriginY = 0.0f;
    if (!XamlOriginDip(commitOriginX, commitOriginY) ||
        commitOriginX != originX || commitOriginY != originY) {
        PublishScrollerUnknown(expectedGeneration);
        NoteScrollerFailure(L"commit", expectedGeneration, true);
        return;
    }

    switch (CommitScrollerCache(answer, expectedGeneration, requestId,
                                layoutVersion)) {
        case ScrollerCommitState::Commit:
            break;
        case ScrollerCommitState::Obsolete:
            return;
        case ScrollerCommitState::Owned:
            PublishScrollerUnknown(expectedGeneration);
            NoteScrollerFailure(L"not-allowed", expectedGeneration, false);
            return;
        case ScrollerCommitState::Moved:
            PublishScrollerUnknown(expectedGeneration);
            NoteScrollerFailure(L"commit", expectedGeneration, true);
            return;
    }

    for (int i = 0; i < kMaxScrollRegions; ++i) {
        ScrollerWatch& watch = ScrollerWatches()[i];
        if (!watch.owner) {
            continue;
        }
        bool named = false;
        for (int r = 0; r < answer.count; ++r) {
            if (watch.owner == answerViewers[r]) {
                named = true;
                break;
            }
        }
        if (!named) {
            RevokeScrollerWatch(i);
            watch.viewer = nullptr;
        }
    }
    bool watchRefused = false;
    for (int r = 0; r < answer.count; ++r) {
        const wuxc::ScrollViewer viewer = answerViewers[r];
        int slot = -1;
        for (int i = 0; i < kMaxScrollRegions; ++i) {
            if (ScrollerWatches()[i].owner == viewer &&
                ScrollerWatches()[i].token.value) {
                slot = i;
                break;
            }
        }
        if (slot < 0) {
            for (int i = 0; i < kMaxScrollRegions; ++i) {
                ScrollerWatch& watch = ScrollerWatches()[i];
                if (watch.owner || watch.token.value) {
                    continue;
                }
                watch.owner = viewer;
                watch.viewer = viewer;
                try {
                    watch.token = viewer.ViewChanged(
                        [viewer](auto const&, auto const&) {
                            UiDepthGuard depth;
                            RefreshScrollerOffsets(viewer);
                        });
                    if (watch.token.value) {
                        g_uiRegistrationCount.fetch_add(1,
                                                        std::memory_order_relaxed);
                        slot = i;
                    }
                } catch (winrt::hresult_error const& e) {
                    watch.token = {};
                    watch.owner = nullptr;
                    watch.viewer = nullptr;
                    TraceLog(L"scroller watch failed err=%08X",
                            static_cast<unsigned>(e.code()));
                } catch (...) {
                    watch.token = {};
                    watch.owner = nullptr;
                    watch.viewer = nullptr;
                    TraceLog(L"scroller watch failed err=unknown");
                }
                break;
            }
        }
        if (slot < 0) {

            watchRefused = true;
            continue;
        }
        ScrollerWatches()[slot].viewer = viewer;
        answer.regions[r].watchSlot = slot;
    }
    {

        std::lock_guard lock(g_scrollerMutex);
        if (g_scroller.generation == expectedGeneration &&
            g_scroller.layoutVersion == layoutVersion) {
            for (int r = 0; r < answer.count; ++r) {
                if (r < g_scroller.count) {
                    g_scroller.regions[r].watchSlot = answer.regions[r].watchSlot;
                }
            }
        }
    }
    if (watchRefused) {
        Wh_Log(L"ViewChanged registration is still attached; this pass adds no "
               L"scroll watcher");
        TraceLog(L"xaml viewchanged handler left attached; new registration "
                L"refused");
    }

    for (int r = 0; r < answer.count; ++r) {
        const ScrollerRegion& region = answer.regions[r];
        const bool moved =
            !g_scrollerRegionLogged[r] ||
            std::fabs(region.left - g_scrollerLoggedRect[r][0]) > 1.0f ||
            std::fabs(region.top - g_scrollerLoggedRect[r][1]) > 1.0f ||
            std::fabs(region.right - g_scrollerLoggedRect[r][2]) > 1.0f ||
            std::fabs(region.bottom - g_scrollerLoggedRect[r][3]) > 1.0f;
        if (!moved) {
            continue;
        }
        g_scrollerRegionLogged[r] = true;
        g_scrollerLoggedRect[r][0] = region.left;
        g_scrollerLoggedRect[r][1] = region.top;
        g_scrollerLoggedRect[r][2] = region.right;
        g_scrollerLoggedRect[r][3] = region.bottom;
        TraceLog(L"scroller rect=%.0f,%.0f,%.0f,%.0f vo=%.0f sh=%.0f "
                L"name=%s depth=%u",
                region.left, region.top, region.right, region.bottom,
                region.verticalOffset, region.scrollableHeight, region.label,
                region.depth);
    }
}

void ReleaseScrollableElement() {
    for (int i = 0; i < kMaxScrollRegions; ++i) {
        RevokeScrollerWatch(i);

        ScrollerWatches()[i].viewer = nullptr;
    }
    g_scrollerNoneLogged = false;
    g_scrollerOverflowLogged = false;
    g_scrollerRetryLogged = false;
    g_scrollerRetryCount = 0;
    g_scrollerRetryGenerationScheduled = 0;
    g_scrollerRetryExhaustedLogged = false;
    for (int i = 0; i < kMaxScrollRegions; ++i) {
        g_scrollerRegionLogged[i] = false;
        g_scrollerLoggedRect[i][0] = 0.0f;
        g_scrollerLoggedRect[i][1] = 0.0f;
        g_scrollerLoggedRect[i][2] = 0.0f;
        g_scrollerLoggedRect[i][3] = 0.0f;
    }
    std::lock_guard lock(g_scrollerMutex);
    g_scroller = ScrollerCache{};
}

void ReleaseScrollableElementIfGeneration(uint32_t maxGeneration) {
    {
        std::lock_guard lock(g_scrollerMutex);
        if (g_scroller.generation > maxGeneration) {
            return;
        }
    }
    ReleaseScrollableElement();
}

void CaptureCoreWindowHandle() {
    HWND previous = g_coreHwnd.load(std::memory_order_relaxed);

    try {
        wucore::CoreWindow coreWindow =
            wucore::CoreWindow::GetForCurrentThread();
        if (!coreWindow) {
            if (!previous || !IsWindow(previous)) {
                g_coreHwnd.store(nullptr, std::memory_order_relaxed);
            }
            return;
        }

        void* windowAbi = winrt::get_abi(coreWindow);
        if (!windowAbi) {
            if (!previous || !IsWindow(previous)) {
                g_coreHwnd.store(nullptr, std::memory_order_relaxed);
            }
            return;
        }

        void* interop = nullptr;
        if (FAILED(static_cast<IUnknown*>(windowAbi)->QueryInterface(
                kCoreWindowInteropIid, &interop)) ||
            !interop) {
            if (!previous || !IsWindow(previous)) {
                g_coreHwnd.store(nullptr, std::memory_order_relaxed);
            }
            return;
        }

        HWND hwnd = nullptr;
        auto getWindowHandle = reinterpret_cast<GetCoreWindowHandleAbi>(
            (*reinterpret_cast<void***>(interop))[3]);
        if (getWindowHandle) {
            getWindowHandle(interop, &hwnd);
        }
        static_cast<IUnknown*>(interop)->Release();
        if (hwnd) {
            g_coreHwnd.store(hwnd, std::memory_order_relaxed);
        } else if (!previous || !IsWindow(previous)) {
            g_coreHwnd.store(nullptr, std::memory_order_relaxed);
        }
    } catch (...) {

        if (!previous || !IsWindow(previous)) {
            g_coreHwnd.store(nullptr, std::memory_order_relaxed);
        }
    }
}

void UpdateDismissAxis(bool logAxis) {
    HWND hwnd = g_coreHwnd.load(std::memory_order_relaxed);
    if (!hwnd) {
        return;
    }

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
        return;
    }

    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, &info)) {
        return;
    }

    const RECT& work = info.rcWork;
    const RECT& monitorRect = info.rcMonitor;

    const TaskbarProbe probe =
        ProbeTaskbar(info, DpiScaleForWindow(hwnd));

    if (probe.decided || g_taskbarProbeMonitor != monitor) {
        g_taskbarProbe = probe;
        g_taskbarProbeMonitor = monitor;
    }

    const int32_t previousAxis = g_dismissAxis;
    if (probe.decided) {
        g_dismissAxis = static_cast<int32_t>(probe.edge);
        g_axisKnown = true;
    }

    bool log = logAxis || g_dismissAxis != previousAxis;
    if (!probe.decided) {

        if (g_axisKeptLogged) {
            log = false;
        } else {
            g_axisKeptLogged = true;
            log = true;
        }
        if (log) {
            TraceLog(L"dismiss axis=%d kept, no source named an edge "
                    L"(0=left,1=top,2=right,3=bottom) %s "
                    L"appbar=%d rc=%d,%d,%d,%d bar=%d rect=%d,%d,%d,%d "
                    L"work=%d,%d,%d,%d mon=%d,%d,%d,%d",
                    g_dismissAxis, g_axisKnown ? L"last-known" : L"initializer",
                    probe.appBar ? static_cast<int>(probe.appBarEdge) : -1,
                    static_cast<int>(probe.appBarRectPx.left),
                    static_cast<int>(probe.appBarRectPx.top),
                    static_cast<int>(probe.appBarRectPx.right),
                    static_cast<int>(probe.appBarRectPx.bottom),
                    probe.barWindow ? static_cast<int>(probe.barWindowEdge) : -1,
                    static_cast<int>(probe.barWindowRectPx.left),
                    static_cast<int>(probe.barWindowRectPx.top),
                    static_cast<int>(probe.barWindowRectPx.right),
                    static_cast<int>(probe.barWindowRectPx.bottom),
                    static_cast<int>(work.left), static_cast<int>(work.top),
                    static_cast<int>(work.right), static_cast<int>(work.bottom),
                    static_cast<int>(monitorRect.left),
                    static_cast<int>(monitorRect.top),
                    static_cast<int>(monitorRect.right),
                    static_cast<int>(monitorRect.bottom));
        }
    } else {
        g_axisKeptLogged = false;
        if (log) {
            TraceLog(L"dismiss axis=%d (0=left,1=top,2=right,3=bottom) src=%s "
                    L"appbar=%d rc=%d,%d,%d,%d bar=%d rect=%d,%d,%d,%d "
                    L"work=%d,%d,%d,%d mon=%d,%d,%d,%d",
                    g_dismissAxis, probe.source,
                    probe.appBar ? static_cast<int>(probe.appBarEdge) : -1,
                    static_cast<int>(probe.appBarRectPx.left),
                    static_cast<int>(probe.appBarRectPx.top),
                    static_cast<int>(probe.appBarRectPx.right),
                    static_cast<int>(probe.appBarRectPx.bottom),
                    probe.barWindow ? static_cast<int>(probe.barWindowEdge) : -1,
                    static_cast<int>(probe.barWindowRectPx.left),
                    static_cast<int>(probe.barWindowRectPx.top),
                    static_cast<int>(probe.barWindowRectPx.right),
                    static_cast<int>(probe.barWindowRectPx.bottom),
                    static_cast<int>(work.left), static_cast<int>(work.top),
                    static_cast<int>(work.right), static_cast<int>(work.bottom),
                    static_cast<int>(monitorRect.left),
                    static_cast<int>(monitorRect.top),
                    static_cast<int>(monitorRect.right),
                    static_cast<int>(monitorRect.bottom));
        }
    }

}

bool RefreshGestureGeometry() {
    wux::FrameworkElement root = g_root;
    if (!root) {
        return false;
    }

    {
        std::lock_guard lock(g_compositionMutex);
        if (!g_state) {
            return false;
        }
    }

    const int32_t previousAxis = g_dismissAxis;
    const float previousDistance = g_dismissDistance;
    UpdateDismissAxis(false);

    float dirX = 0.0f;
    float dirY = 0.0f;
    float distance = 0.0f;
    AxisTranslation(g_dismissAxis, static_cast<float>(root.ActualWidth()),
                    static_cast<float>(root.ActualHeight()), dirX, dirY,
                    distance);
    if (!(distance > 0.0f)) {

        g_dismissAxis = previousAxis;
        return false;
    }

    const bool previouslyInvalid =
        g_rendererInvalid.load(std::memory_order_acquire);
    const bool changed = previouslyInvalid || g_dismissAxis != previousAxis ||
                         !SameLayoutDistance(distance, previousDistance);

    {
        std::lock_guard lock(g_compositionMutex);
        if (!g_state) {
            g_dismissAxis = previousAxis;
            return false;
        }
        if (changed) {
            try {
                g_state.InsertScalar(L"Distance", distance);
                g_state.InsertScalar(L"DirX", dirX);
                g_state.InsertScalar(L"DirY", dirY);
            } catch (winrt::hresult_error const& e) {
                TraceLog(L"gesture geometry write failed err=%08X",
                        static_cast<unsigned>(e.code()));

                g_dismissAxis = previousAxis;
                SetRendererInvalid(true);
                TraceLog(L"gesture renderer invalid until a successful reapply");
                return false;
            } catch (...) {
                TraceLog(L"gesture geometry write failed err=unknown");
                g_dismissAxis = previousAxis;
                SetRendererInvalid(true);
                TraceLog(L"gesture renderer invalid until a successful reapply");
                return false;
            }
        }
    }

    if (changed) {
        g_dismissDistance = distance;
        g_layoutVersion.fetch_add(1, std::memory_order_acq_rel);
    }
    if (previouslyInvalid) {

        SetRendererInvalid(false);
    }

    TraceLog(changed ? L"gesture geometry updated axis=%d dist=%.0f"
                    : L"gesture geometry axis=%d dist=%.0f (unchanged)",
            g_dismissAxis, distance);
    return true;
}

bool Setup(wux::FrameworkElement const& root) {
    if (!root) {
        return false;
    }

    {
        std::lock_guard lock(g_compositionMutex);
        if (g_state) {
            return true;
        }
    }

    RecordPeerUiThread();
    g_root = root;
    g_panelTrailLogged = false;

    CaptureCoreWindowHandle();
    if (!g_coreHwnd.load(std::memory_order_relaxed)) {
        Wh_Log(L"No CoreWindow HWND; gestures stay disabled");
    }
    UpdateDismissAxis(true);

    wuxh::ElementCompositionPreview::SetIsTranslationEnabled(root, true);

    wuc::Visual visual = wuxh::ElementCompositionPreview::GetElementVisual(root);
    wuc::Compositor compositor = visual.Compositor();

    const float width = static_cast<float>(root.ActualWidth());
    const float height = static_cast<float>(root.ActualHeight());

    float dirX = 0.0f;
    float dirY = 0.0f;
    float distance = 0.0f;
    AxisTranslation(g_dismissAxis, width, height, dirX, dirY, distance);

    wuc::CompositionPropertySet state = compositor.CreatePropertySet();
    state.InsertScalar(L"Progress", 1.0f);
    state.InsertScalar(L"Distance", distance);
    state.InsertScalar(L"DirX", dirX);
    state.InsertScalar(L"DirY", dirY);

    visual.Properties().InsertVector3(L"Translation",
                                      float3{0.0f, 0.0f, 0.0f});

    wuc::ExpressionAnimation translation = compositor.CreateExpressionAnimation(
        L"Vector3((1 - state.Progress) * state.DirX * state.Distance, "
        L"(1 - state.Progress) * state.DirY * state.Distance, 0)");
    translation.SetReferenceParameter(L"state", state);
    visual.StartAnimation(L"Translation", translation);

    {

        wucore::CoreDispatcher dispatcher = root.Dispatcher();
        std::lock_guard lock(g_compositionMutex);
        g_compositor = compositor;
        g_visual = visual;
        g_state = state;
        g_translationExpression = translation;

        g_dispatcher = dispatcher;
    }

    g_dismissDistance = distance;
    SetRendererInvalid(false);

    if (g_sizeToken.value) {
        Wh_Log(L"SizeChanged registration from the previous root is still "
               L"attached; this root gets no resize handler");
        TraceLog(L"xaml size handler left attached; new registration refused");
    } else {
        const uint32_t sizeGeneration =
            g_setupGeneration.load(std::memory_order_relaxed);
        g_sizeRoot = root;
        g_sizeToken = root.SizeChanged(
            [sizeGeneration](winrt::Windows::Foundation::IInspectable const&,
                             wux::SizeChangedEventArgs const&) {
                UiDepthGuard depth;
                try {

                    if (sizeGeneration !=
                        g_setupGeneration.load(std::memory_order_relaxed)) {
                        return;
                    }

                    if (g_panelOwned.load(std::memory_order_relaxed)) {
                        g_measureDeferred.store(true,
                                                std::memory_order_relaxed);

                        g_panelTrailLogged = false;
                        TraceLog(L"gesture geometry deferred "
                                L"reason=panel-owned gen=%u",
                                g_openGeneration.load(
                                    std::memory_order_relaxed));
                        return;
                    }
                    LayoutApplyGuard apply;
                    if (!apply.Begin()) {
                        g_measureDeferred.store(true,
                                                std::memory_order_relaxed);
                        g_panelTrailLogged = false;
                        TraceLog(L"gesture geometry deferred "
                                L"reason=panel-owned gen=%u",
                                g_openGeneration.load(
                                    std::memory_order_relaxed));
                        return;
                    }
                    if (!RefreshGestureGeometry()) {
                        return;
                    }

                    PublishPanelTrailDip(true, 0, 0);

                    CacheScrollableElement(
                        g_openGeneration.load(std::memory_order_relaxed), 0);
                } catch (winrt::hresult_error const& e) {
                    Wh_Log(L"SizeChanged update failed: %08X", e.code());
                } catch (...) {

                }
            });
        if (g_sizeToken.value) {
            g_uiRegistrationCount.fetch_add(1, std::memory_order_relaxed);
        }
    }

    PublishPanelTrailDip(true, 0, 0);
    SetRendererReady(true);

    const winrt::hstring rootClassName = winrt::get_class_name(root);
    TraceLog(L"attached root=%s size=%.0fx%.0f", rootClassName.c_str(), width,
            height);

    Wh_Log(L"Dismiss input armed; composition translation enabled "
           L"root=%s (distance=%.1f)",
           rootClassName.c_str(), distance);
    return true;
}

using PrepareRootElement_t = void* (*)(void*, void*, bool);
PrepareRootElement_t PrepareRootElement_Original = nullptr;

bool EnsureUiRendezvousWindow();

void AttachRootElement(wux::FrameworkElement const& root, bool replaceExisting);

void* PrepareRootElement_Hook(void* frame, void* perFrameData, bool flag) {

    RootAdmissionShared admission;
    if (g_unloading.load(std::memory_order_relaxed)) {

        return PrepareRootElement_Original
                   ? PrepareRootElement_Original(frame, perFrameData, flag)
                   : nullptr;
    }
    void* result = PrepareRootElement_Original
                       ? PrepareRootElement_Original(frame, perFrameData, flag)
                       : nullptr;
    if (!result || g_unloading.load(std::memory_order_relaxed)) {
        return result;
    }

    RecordPeerUiThread();
    CaptureCoreWindowHandle();
    if (!EnsureUiRendezvousWindow()) {

        const bool hadRenderer = g_root || g_dispatcher || RendererReady();
        if (hadRenderer) {
            static bool withdrawLogged = false;
            ResetRenderer();
            if (!withdrawLogged) {
                withdrawLogged = true;
                TraceLog(L"root renderer withdrawn "
                        L"reason=no-rendezvous-window");
            }
        }
        static bool rendezvousFailureLogged = false;
        if (!rendezvousFailureLogged) {
            rendezvousFailureLogged = true;
            Wh_Log(L"UI rendezvous window could not be created; the root gets "
                   L"no renderer and the unload has no window to drain");
            TraceLog(L"root registration skipped reason=no-rendezvous-window");
        }
        return result;
    }

    try {
        wux::UIElement element{nullptr};
        winrt::copy_from_abi(element, result);

        wux::FrameworkElement root = element.try_as<wux::FrameworkElement>();
        if (!root) {
            Wh_Log(L"PrepareRootElement returned a non-FrameworkElement root");
            return result;
        }

        AttachRootElement(root, true);
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"PrepareRootElement hook failed: %08X %s", e.code(),
               e.message().c_str());
    } catch (...) {
        Wh_Log(L"PrepareRootElement hook failed with an unknown exception");
    }

    return result;
}

void AttachRootElement(wux::FrameworkElement const& root, bool replaceExisting) {
    if (!root) {
        return;
    }
    if (!replaceExisting) {
        std::lock_guard lock(g_compositionMutex);
        if (g_root || g_state) {
            TraceLog(L"root attach skipped reason=already-attached");
            return;
        }
    }

    Wh_Log(L"Attaching root class=%s name=%s",
           winrt::get_class_name(root).c_str(), root.Name().c_str());

    ResetRenderer();

    try {
        wucore::CoreDispatcher dispatcher = root.Dispatcher();
        if (dispatcher) {
            std::lock_guard lock(g_compositionMutex);
            g_dispatcher = dispatcher;
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Dispatcher capture failed: %08X", e.code());
    } catch (...) {
        Wh_Log(L"Dispatcher capture failed with an unknown exception");
    }

    bool alreadyLoaded = false;
    try {
        alreadyLoaded = root.IsLoaded();
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"IsLoaded check failed: %08X", e.code());
    }

    if (g_unloading.load(std::memory_order_relaxed)) {
        TraceLog(L"root registration skipped reason=unloading");
        return;
    }

    if (alreadyLoaded) {
        Setup(root);
        return;
    }
    if (g_loadedToken.value) {

        Wh_Log(L"Loaded registration from the previous root is still attached; "
               L"this root gets no renderer");
        TraceLog(L"xaml loaded handler left attached; new registration refused");
        return;
    }

    const winrt::hstring rootClassName = winrt::get_class_name(root);
    TraceLog(L"root pending load class=%s", rootClassName.c_str());

    const uint32_t generation =
        g_setupGeneration.load(std::memory_order_relaxed);
    auto weak = winrt::make_weak(root);
    g_loadedRoot = root;
    g_loadedToken = root.Loaded(
        [weak, generation](winrt::Windows::Foundation::IInspectable const& sender,
                           wux::RoutedEventArgs const&) {
            UiDepthGuard depth;
            try {
                if (generation !=
                    g_setupGeneration.load(std::memory_order_relaxed)) {
                    return;
                }
                wux::FrameworkElement current =
                    sender.try_as<wux::FrameworkElement>();
                if (!current) {
                    current = weak.get();
                }
                if (current) {
                    Setup(current);
                }
            } catch (winrt::hresult_error const& e) {
                Wh_Log(L"Loaded setup failed: %08X %s", e.code(),
                       e.message().c_str());
            } catch (...) {
                Wh_Log(L"Loaded setup failed with an unknown exception");
            }
        });
    if (g_loadedToken.value) {
        g_uiRegistrationCount.fetch_add(1, std::memory_order_relaxed);
    }
}

std::atomic<HMODULE> g_startUIModuleSeen{nullptr};
std::atomic<HMODULE> g_startUIHookModule{nullptr};

constexpr uint64_t kRootPendingDeadlineMs = 120000;
uint64_t g_rootProbeNextTick = 0;
uint64_t g_rootProbeFirstTick = 0;

std::atomic<bool> g_startUIAttempted{false};

SRWLOCK g_startUIHookLock = SRWLOCK_INIT;

bool InstallStartUIHooks(HMODULE module) {
    if (!module) {
        return false;
    }

    AcquireSRWLockExclusive(&g_startUIHookLock);

    HMODULE expected = nullptr;
    if (!g_startUIHookModule.compare_exchange_strong(expected, module)) {
        ReleaseSRWLockExclusive(&g_startUIHookLock);
        return false;
    }
    if (g_disabled.load(std::memory_order_relaxed)) {

        ReleaseSRWLockExclusive(&g_startUIHookLock);
        return false;
    }
    TraceLog(L"root install begin module=%p", module);

    WindhawkUtils::SYMBOL_HOOK startuiDllHooks[] = {
        {
            {L"private: static class Windows::UI::Xaml::UIElement ^ __cdecl "
             L"StartUI::App::PrepareRootElement(class Windows::UI::Xaml::"
             L"Controls::Frame ^,class StartUI::PerFrameData ^,bool)"},
            &PrepareRootElement_Original,
            &PrepareRootElement_Hook,
        },
    };
    const uintptr_t base = reinterpret_cast<uintptr_t>(module);
    const bool hooked = WindhawkUtils::HookSymbols(
        module, startuiDllHooks, ARRAYSIZE(startuiDllHooks));
    const uintptr_t resolved =
        reinterpret_cast<uintptr_t>(PrepareRootElement_Original);
    if (!hooked || !resolved) {

        g_startUIHookModule.store(nullptr, std::memory_order_release);
        ReleaseSRWLockExclusive(&g_startUIHookLock);
        return false;
    }

    const uintptr_t rva = resolved - base;
    g_rootHookReady.store(true, std::memory_order_release);

    Wh_ApplyHookOperations();
    TraceLog(L"root install applied tid=%u", GetCurrentThreadId());
    ReleaseSRWLockExclusive(&g_startUIHookLock);

    TraceLog(L"startui root symbol resolved base=0x%llX at=0x%llX rva=0x%llX "
            L"expected=0x%llX match=%d",
            static_cast<unsigned long long>(base),
            static_cast<unsigned long long>(resolved),
            static_cast<unsigned long long>(rva),
            static_cast<unsigned long long>(kPrepareRootElementExpectedRva),
            rva == kPrepareRootElementExpectedRva ? 1 : 0);
    if (rva == kPrepareRootElementExpectedRva) {
        Wh_Log(L"StartUI::App::PrepareRootElement resolved at %p "
               L"(module+0x%llX, the reference address)",
               reinterpret_cast<void*>(resolved),
               static_cast<unsigned long long>(rva));
    } else {
        Wh_Log(L"StartUI::App::PrepareRootElement resolved at %p "
               L"(module+0x%llX; the reference is module+0x%llX)",
               reinterpret_cast<void*>(resolved),
               static_cast<unsigned long long>(rva),
               static_cast<unsigned long long>(kPrepareRootElementExpectedRva));
    }
    return true;
}

void RecordLoadedStartUIModule() {
    if (g_disabled.load(std::memory_order_relaxed)) {
        return;
    }
    HMODULE startUI = GetModuleHandleW(kStartUIModuleName);
    if (startUI) {
        g_startUIModuleSeen.store(startUI, std::memory_order_release);
    }
}

void ProbeRootSymbolFromInputThread() {
    if (g_rootHookReady.load(std::memory_order_acquire) ||
        g_rootHookRefused.load(std::memory_order_acquire) ||
        g_disabled.load(std::memory_order_relaxed)) {
        return;
    }

    const uint64_t now = GetTickCount64();
    if (g_rootProbeFirstTick == 0) {
        g_rootProbeFirstTick = now;
    }
    if (now - g_rootProbeFirstTick >= kRootPendingDeadlineMs) {
        g_rootHookRefused.store(true, std::memory_order_release);
        TraceLog(L"startui root symbol unresolved name=StartUI::App::"
                L"PrepareRootElement attempts=%d waited=%llums "
                L"deadline=%llums",
                g_startUIAttempted.load(std::memory_order_acquire) ? 1 : 0,
                static_cast<unsigned long long>(now - g_rootProbeFirstTick),
                static_cast<unsigned long long>(kRootPendingDeadlineMs));
        Wh_Log(L"StartUI::App::PrepareRootElement could not be resolved in "
               L"StartUI_.dll within the %llu ms module wait; the root hook is "
               L"refused and the controller advertises no service",
               static_cast<unsigned long long>(kRootPendingDeadlineMs));
        PublishPeerGone(L"root-hook-refused");
        return;
    }

    if (now < g_rootProbeNextTick) {
        return;
    }
    g_rootProbeNextTick = now + kRootProbeMs;

    HMODULE module = g_startUIModuleSeen.load(std::memory_order_acquire);
    if (!module) {
        module = GetModuleHandleW(kStartUIModuleName);
        if (!module) {
            return;
        }
        g_startUIModuleSeen.store(module, std::memory_order_release);
    }

    if (g_startUIAttempted.exchange(true, std::memory_order_acq_rel)) {
        return;
    }
    if (InstallStartUIHooks(module)) {

        return;
    }
    if (g_rootHookReady.load(std::memory_order_acquire) ||
        g_disabled.load(std::memory_order_relaxed)) {
        return;
    }

    g_rootHookRefused.store(true, std::memory_order_release);
    TraceLog(L"startui root symbol unresolved name=StartUI::App::"
            L"PrepareRootElement attempts=1 waited=%llums",
            static_cast<unsigned long long>(GetTickCount64() -
                                            g_rootProbeFirstTick));
    Wh_Log(L"StartUI::App::PrepareRootElement could not be resolved in "
           L"StartUI_.dll on the one attempt this session makes; the root hook "
           L"is refused and the controller advertises no service");
    PublishPeerGone(L"root-hook-refused");
}

void RevokeHandlers() {

    StopSettledTrailWatch();
    ReleaseScrollableElement();

    wux::FrameworkElement root = g_root;
    wux::FrameworkElement sizeRoot = g_sizeRoot ? g_sizeRoot : root;
    wux::FrameworkElement loadedRoot = g_loadedRoot ? g_loadedRoot : root;

    if (g_sizeToken.value) {
        if (!sizeRoot) {
            Wh_Log(L"SizeChanged registration has no owner to revoke on");
        } else {
            try {
                sizeRoot.SizeChanged(g_sizeToken);
                g_uiRegistrationCount.fetch_sub(1, std::memory_order_relaxed);
                g_sizeToken = {};
                g_sizeRoot = nullptr;
            } catch (winrt::hresult_error const& e) {

                Wh_Log(L"Failed to revoke SizeChanged: %08X %s", e.code(),
                       e.message().c_str());
            } catch (...) {
                Wh_Log(L"Failed to revoke SizeChanged with an unknown "
                       L"exception");
            }
        }
    } else {
        g_sizeRoot = nullptr;
    }
    if (g_loadedToken.value) {
        if (!loadedRoot) {
            Wh_Log(L"Loaded registration has no owner to revoke on");
        } else {
            try {
                loadedRoot.Loaded(g_loadedToken);
                g_uiRegistrationCount.fetch_sub(1, std::memory_order_relaxed);
                g_loadedToken = {};
                g_loadedRoot = nullptr;
            } catch (winrt::hresult_error const& e) {
                Wh_Log(L"Failed to revoke Loaded: %08X %s", e.code(),
                       e.message().c_str());
            } catch (...) {
                Wh_Log(L"Failed to revoke Loaded with an unknown exception");
            }
        }
    } else {
        g_loadedRoot = nullptr;
    }

    g_root = nullptr;
}

void ResetComposition() {

    const bool batchRevoked = RevokeDismissBatch();

    std::lock_guard lock(g_compositionMutex);

    InvalidateDismissBatchLocked();
    g_panel.frames = 0;
    g_panel.durationMs = 0;

    try {
        if (g_state) {
            g_state.StopAnimation(L"Progress");
            g_panel.coasting.store(false, std::memory_order_relaxed);
            g_state.InsertScalar(L"Progress", kProgressRest);
            g_panel.progress.store(kProgressRest, std::memory_order_relaxed);
        }
        if (g_visual) {
            g_visual.StopAnimation(L"Translation");
            g_visual.Properties().InsertVector3(
                L"Translation", float3{0.0f, 0.0f, 0.0f});
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Composition cleanup failed: %08X", e.code());
    } catch (...) {
    }

    if (batchRevoked) {
        g_dismissBatch = nullptr;
        g_dismissBatchToken = {};
        g_settleLease.reset();
    }
    g_translationExpression = nullptr;
    g_compositor = nullptr;
    g_state = nullptr;
    g_visual = nullptr;

    SetRendererReady(false);
    SetRendererInvalid(false);

    g_dispatcher = nullptr;

    CancelMeasureRequest();
    ClearScrollerRetry();
    g_dismissDistance = 0.0f;
    const uint32_t emptyGeneration = AdvanceOpenGeneration();
    GestureGeometry empty;
    empty.generation = emptyGeneration;
    empty.layout.axis = g_dismissAxis;
    PublishGestureGeometry(empty);

    g_setupGeneration.fetch_add(1, std::memory_order_relaxed);
}

void ResetRenderer() {
    RevokeHandlers();
    ResetComposition();
}

void TeardownUi() {

    RevokeHandlers();

    RevokeDismissBatch();

    ResetComposition();
    g_scrollerWatches.reset();
    g_root = nullptr;
    g_loadedRoot = nullptr;
    g_sizeRoot = nullptr;
    g_layoutRoot = nullptr;
    g_dispatcher = nullptr;
    ClearPeerUiThread();
    std::lock_guard lock(g_compositionMutex);
    g_settleLease.reset();
}

[[noreturn]] void BlockUnload(const wchar_t* reason) {
    TraceLog(L"unload blocked reason=%s", reason);
    Wh_Log(L"Unload blocked: %s; the module image cannot be freed safely",
           reason);
    Sleep(INFINITE);
    __builtin_unreachable();
}

constexpr wchar_t kUiRendezvousClassName[] =
    L"WindhawkLegacyStartSwipeUiRendezvous";
constexpr UINT kUiTeardownMessage = WM_APP + 0x62;
constexpr UINT kUiDestroyMessage = WM_APP + 0x63;
constexpr UINT kUiDrainMessage = WM_APP + 0x64;
constexpr WPARAM kUiTeardownMagic = 0x53575455;
constexpr WPARAM kUiDestroyMagic = 0x53575458;
constexpr WPARAM kUiDrainMagic = 0x53575452;
constexpr int kUiDrainRetries = 8;

bool g_uiRendezvousClassRegistered = false;
HINSTANCE g_uiRendezvousClassInstance = nullptr;

// The unload handshakes carry no pointer: a same-user process can reach the
// message-only window, and these flags are the whole answer.
std::atomic<bool> g_uiTeardownServed{false};
std::atomic<bool> g_uiDrainServed{false};

LRESULT CALLBACK UiRendezvousProc(HWND hwnd, UINT message, WPARAM wParam,
                                  LPARAM lParam) {
    if (message == kUiTeardownMessage) {
        if (wParam != kUiTeardownMagic ||
            hwnd != g_uiRendezvousHwnd.load(std::memory_order_acquire)) {
            TraceLog(L"ui rendezvous teardown ignored hwnd=%p", hwnd);
            return 0;
        }
        if (g_uiOwnerDepth > 0) {

            g_uiTeardownPending = true;
            TraceLog(L"ui teardown deferred depth=%d",
                    g_uiOwnerDepth.load(std::memory_order_relaxed));
            g_uiTeardownServed.store(true, std::memory_order_release);
            return 0;
        }
        try {
            TeardownUi();
        } catch (...) {

        }
        g_uiTeardownServed.store(true, std::memory_order_release);
        if (g_uiTeardownAck) {
            SetEvent(g_uiTeardownAck);
        }
        return 0;
    }
    if (message == kUiDeferredTeardownMessage) {
        if (wParam != kUiDeferredTeardownMagic ||
            hwnd != g_uiRendezvousHwnd.load(std::memory_order_acquire)) {
            TraceLog(L"ui deferred teardown ignored hwnd=%p", hwnd);
            return 0;
        }
        if (g_uiOwnerDepth > 0) {

            g_uiTeardownPending = true;
            TraceLog(L"ui deferred teardown still nested depth=%d",
                    g_uiOwnerDepth.load(std::memory_order_relaxed));
            return 0;
        }
        try {
            TeardownUi();
        } catch (...) {

        }
        if (g_uiTeardownAck) {
            SetEvent(g_uiTeardownAck);
        }

        TraceLog(L"ui deferred teardown served hwnd=%p", hwnd);
        return 0;
    }
    if (message == kUiDrainMessage) {
        if (wParam != kUiDrainMagic ||
            hwnd != g_uiRendezvousHwnd.load(std::memory_order_acquire)) {
            TraceLog(L"ui rendezvous drain ignored hwnd=%p", hwnd);
            return 0;
        }
        g_uiDrainServed.store(true, std::memory_order_release);
        return 0;
    }
    if (message == kUiDestroyMessage) {
        if (wParam != kUiDestroyMagic ||
            hwnd != g_uiRendezvousHwnd.load(std::memory_order_acquire)) {
            TraceLog(L"ui rendezvous destroy ignored hwnd=%p", hwnd);
            return 0;
        }
        if (!DestroyWindow(hwnd)) {
            TraceLog(L"ui rendezvous destroy failed err=%u", GetLastError());
        }
        return 0;
    }
    if (message == WM_NCDESTROY) {
        if (g_uiRendezvousHwnd.load(std::memory_order_acquire) == hwnd) {
            g_uiRendezvousHwnd.store(nullptr, std::memory_order_release);
        }
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

bool EnsureUiRendezvousWindow() {

    const DWORD uiThread = GetCurrentThreadId();
    HWND existing = g_uiRendezvousHwnd.load(std::memory_order_acquire);
    if (existing) {
        if (IsWindow(existing)) {
            return true;
        }
        g_uiRendezvousHwnd.store(nullptr, std::memory_order_release);
    }
    if (!g_uiRendezvousClassRegistered) {
        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.lpfnWndProc = UiRendezvousProc;
        windowClass.hInstance = reinterpret_cast<HINSTANCE>(&__ImageBase);
        windowClass.lpszClassName = kUiRendezvousClassName;
        if (!RegisterClassExW(&windowClass)) {
            TraceLog(L"ui rendezvous class failed err=%u", GetLastError());
            return false;
        }
        g_uiRendezvousClassRegistered = true;
        g_uiRendezvousClassInstance = windowClass.hInstance;
    }
    HWND window = CreateWindowExW(0, kUiRendezvousClassName, L"", 0, 0, 0, 0,
                                  0, HWND_MESSAGE, nullptr,
                                  g_uiRendezvousClassInstance, nullptr);
    if (!window) {
        TraceLog(L"ui rendezvous create failed err=%u", GetLastError());
        return false;
    }
    g_uiRendezvousHwnd.store(window, std::memory_order_release);
    TraceLog(L"ui rendezvous hwnd=%p tid=%u", window,
            static_cast<unsigned>(uiThread));
    return true;
}

void AttachToRunningStartHost() {
    HWND coreWindow = FindStartCoreWindow();
    if (!coreWindow) {
        TraceLog(L"attach existing host: no core window");
        return;
    }
    const bool posted = RunFromWindowThread(coreWindow, [](PVOID) {
        UiDepthGuard depth;
        RootAdmissionShared admission;
        if (g_unloading.load(std::memory_order_relaxed)) {
            TraceLog(L"attach existing host: unloading");
            return;
        }
        RecordPeerUiThread();
        CaptureCoreWindowHandle();
        if (!EnsureUiRendezvousWindow()) {
            TraceLog(L"attach existing host: no rendezvous window");
            return;
        }
        try {
            wux::Window window = wux::Window::Current();
            if (!window) {
                TraceLog(L"attach existing host: no xaml window");
                return;
            }
            wux::UIElement content = window.Content();
            if (!content) {
                TraceLog(L"attach existing host: no xaml content");
                return;
            }
            wux::FrameworkElement root =
                content.try_as<wux::FrameworkElement>();
            if (!root) {
                TraceLog(L"attach existing host: content is not a "
                         L"FrameworkElement");
                return;
            }
            TraceLog(L"attach existing host root class=%s",
                    winrt::get_class_name(root).c_str());
            AttachRootElement(root, false);
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"Attach to running Start host failed: %08X %s", e.code(),
                   e.message().c_str());
        } catch (...) {
            Wh_Log(L"Attach to running Start host failed with an unknown "
                   L"exception");
        }
    }, nullptr);
    TraceLog(L"attach existing host posted=%d", posted ? 1 : 0);
}

void UnregisterUiRendezvousClass() {
    if (!g_uiRendezvousClassRegistered) {
        return;
    }
    if (UnregisterClassW(kUiRendezvousClassName, g_uiRendezvousClassInstance)) {
        TraceLog(L"ui rendezvous class unregistered");
    } else {
        const DWORD error = GetLastError();
        if (error == ERROR_CLASS_DOES_NOT_EXIST) {
            TraceLog(L"ui rendezvous class already gone");
        } else {

            BlockUnload(L"ui rendezvous class unregister failed");
        }
    }
    g_uiRendezvousClassRegistered = false;
    g_uiRendezvousClassInstance = nullptr;
}

void DestroyUiRendezvousWindow() {
    HWND hwnd = g_uiRendezvousHwnd.load(std::memory_order_acquire);
    if (hwnd) {
        if (IsWindow(hwnd)) {
            SendMessageW(hwnd, kUiDestroyMessage, kUiDestroyMagic, 0);
        }
        if (g_uiRendezvousHwnd.load(std::memory_order_acquire) == hwnd) {
            if (!IsWindow(hwnd)) {
                g_uiRendezvousHwnd.store(nullptr, std::memory_order_release);
                TraceLog(L"ui rendezvous gone without destroy hwnd=%p", hwnd);
            } else {
                BlockUnload(L"ui rendezvous window still alive");
            }
        }
    }
    UnregisterUiRendezvousClass();
}

HWND TeardownUiOnUiThread() {
    const DWORD uiThread = g_uiThreadId.load(std::memory_order_relaxed);
    if (uiThread != 0 && uiThread == GetCurrentThreadId()) {
        if (g_uiOwnerDepth > 0) {

            BlockUnload(L"teardown while ui handler is on the stack");
        }
        TeardownUi();
        return nullptr;
    }

    if (uiThread == 0 &&
        g_uiRegistrationCount.load(std::memory_order_acquire) == 0) {
        TeardownUi();
        return nullptr;
    }

    HWND hwnd = g_uiRendezvousHwnd.load(std::memory_order_acquire);
    if (!hwnd || !IsWindow(hwnd)) {
        if (g_uiRegistrationCount.load(std::memory_order_acquire) == 0) {

            TraceLog(L"unload teardown without a UI window");
            return nullptr;
        }
        BlockUnload(L"teardown has no UI window");
    }

    if (!g_uiTeardownAck) {
        BlockUnload(L"UI teardown acknowledgement event missing");
    }
    ResetEvent(g_uiTeardownAck);
    g_uiTeardownPending = false;

    g_uiTeardownServed.store(false, std::memory_order_release);
    SendMessageW(hwnd, kUiTeardownMessage, kUiTeardownMagic, 0);
    if (g_uiTeardownServed.load(std::memory_order_acquire)) {

        TraceLog(L"unload teardown wait hwnd=%p depth=%d", hwnd,
                g_uiOwnerDepth.load(std::memory_order_relaxed));
        if (WaitForSingleObject(g_uiTeardownAck, INFINITE) != WAIT_OBJECT_0) {
            BlockUnload(L"UI teardown acknowledgement failed");
        }
        return hwnd;
    }
    if (IsWindow(hwnd)) {
        BlockUnload(L"teardown message refused");
    }

    TraceLog(L"unload teardown window gone");
    return nullptr;
}

void WaitForModuleEntriesOnly() {
    const bool haveEvent = g_moduleEntriesGone != nullptr;
    for (;;) {
        if (g_moduleEntryCount.load(std::memory_order_acquire) == 0) {
            return;
        }
        if (!haveEvent) {
            BlockUnload(L"module entry event missing");
        }
        const int pending = g_moduleEntryCount.load(std::memory_order_acquire);
        TraceLog(L"unload waiting for ui turns count=%d", pending);
        WaitForSingleObject(g_moduleEntriesGone, INFINITE);
    }
}

void RequireNoModuleCallbacks(bool waitForEntries) {
    if (waitForEntries) {
        WaitForModuleEntriesOnly();
    } else if (g_moduleEntryCount.load(std::memory_order_acquire) != 0) {
        BlockUnload(L"unload on ui thread with pending entries");
    }
    if (g_uiRegistrationCount.load(std::memory_order_acquire) != 0) {
        BlockUnload(L"xaml revokes remain");
    }
}

void RequireNoInlineUiTurns() {
    if (g_moduleEntryCount.load(std::memory_order_acquire) == 0 &&
        g_uiRegistrationCount.load(std::memory_order_acquire) == 0 &&
        g_uiOwnerDepth.load(std::memory_order_acquire) == 0) {
        return;
    }
    BlockUnload(L"inline unload left a ui turn");
}

bool DrainUiThread(HWND hwnd) {
    g_uiDrainServed.store(false, std::memory_order_release);
    if (hwnd) {
        if (!IsWindow(hwnd)) {
            TraceLog(L"unload drain window gone; no acknowledgement");
        } else {
            SendMessageW(hwnd, kUiDrainMessage, kUiDrainMagic, 0);
        }
    }
    RequireNoModuleCallbacks(true);
    return g_uiDrainServed.load(std::memory_order_acquire);
}

void WaitForAttachFramesThenDrainUi(HWND hwnd) {
    WaitForStartAttachFrames();
    if (!hwnd) {

        if (g_startAttachFrames.load(std::memory_order_acquire) != 0) {
            BlockUnload(L"attach hook frames without a window");
        }
        // No drain message means no acknowledgement. A zero frame count now
        // does not prove that no frame enters after the image is freed.
        BlockUnload(L"ui drain window gone");
    }
    for (int attempt = 0; attempt < kUiDrainRetries; ++attempt) {
        if (!IsWindow(hwnd)) {
            if (g_startAttachFrames.load(std::memory_order_acquire) != 0) {
                BlockUnload(L"attach hook frames without a window");
            }
            BlockUnload(L"ui drain window gone");
        }
        if (!DrainUiThread(hwnd)) {
            BlockUnload(L"ui drain not acknowledged");
        }
        if (g_startAttachFrames.load(std::memory_order_acquire) == 0) {
            TraceLog(L"unload ui drained ack=1 attempt=%d", attempt + 1);
            return;
        }
        WaitForStartAttachFrames();
    }
    BlockUnload(L"attach hook frames after drain");
}

BOOL RoleInit() {
    SetTraceFileName(L"renderer.log");
    RefreshTraceSetting();
    TraceLog(L"mod init");
    Wh_Log(L"Swipe for 10Start %s initializing (start role)", kModVersion);

    LoadAnimationStyle();
    LoadGestureStartSpeed();
    LoadCommitDistance();

    g_uiTeardownAck = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_uiTeardownAck) {
        Wh_Log(L"UI teardown event unavailable err=%u; the start role stays "
               L"inert",
               static_cast<unsigned>(GetLastError()));
        return FALSE;
    }

    g_moduleEntriesGone = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_moduleEntriesGone) {
        Wh_Log(L"Module entry event unavailable err=%u; the start role stays "
               L"inert",
               static_cast<unsigned>(GetLastError()));
        CloseHandle(g_uiTeardownAck);
        g_uiTeardownAck = nullptr;
        return FALSE;
    }

    g_startAttachIdle = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_startAttachIdle) {
        Wh_Log(L"Attach frame event unavailable err=%u; the start role stays "
               L"inert",
               static_cast<unsigned>(GetLastError()));
        CloseHandle(g_uiTeardownAck);
        g_uiTeardownAck = nullptr;
        CloseHandle(g_moduleEntriesGone);
        g_moduleEntriesGone = nullptr;
        return FALSE;
    }

    g_hidStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hidStopEvent) {
        TraceLog(L"hid stop event failed err=%u",
                static_cast<unsigned>(GetLastError()));
    }

    g_rendererEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_rendererEvent) {
        TraceLog(L"renderer event failed err=%u",
                static_cast<unsigned>(GetLastError()));
    }

    OpenIpcChannel();
    TraceLog(L"settle plan speed=%.2f dip/ms dur=%llu..%llu segment=%.1f ms "
            L"frames=%d..%d",
            static_cast<double>(kSettleSpeedDipPerMs),
            static_cast<unsigned long long>(kSettleMinDuration.count()),
            static_cast<unsigned long long>(kSettleMaxDuration.count()),
            kSettleSegmentMs, kSettleMinSteps + 1, kSettleMaxFrames);
    TraceLog(L"settle spring win10 zeta=%.2f", kWin10SpringDamping);
    TraceLog(L"settle spring win11 zeta=%.2f", kWin11SpringDamping);

    HMODULE dwmapi = LoadLibraryW(L"dwmapi.dll");
    if (dwmapi) {
        g_dwmGetWindowAttribute.store(
            reinterpret_cast<DwmGetWindowAttribute_t>(
                GetProcAddress(dwmapi, "DwmGetWindowAttribute")),
            std::memory_order_relaxed);
    }
    if (!g_dwmGetWindowAttribute.load(std::memory_order_relaxed)) {
        Wh_Log(L"DwmGetWindowAttribute not resolved; gestures stay disabled");
    }

    HMODULE winmm = LoadLibraryW(L"winmm.dll");
    if (winmm) {
        g_timeBeginPeriod.store(
            reinterpret_cast<TimePeriod_t>(
                GetProcAddress(winmm, "timeBeginPeriod")),
            std::memory_order_relaxed);
        g_timeEndPeriod.store(
            reinterpret_cast<TimePeriod_t>(GetProcAddress(winmm, "timeEndPeriod")),
            std::memory_order_relaxed);
    }
    if (!g_timeBeginPeriod.load(std::memory_order_relaxed) ||
        !g_timeEndPeriod.load(std::memory_order_relaxed)) {
        Wh_Log(L"timeBeginPeriod not resolved; release keeps the default timer "
               L"quantum");
    }

    RecordLoadedStartUIModule();

    g_initialised = true;
    return TRUE;
}

void RoleAfterInit() {
    if (!g_initialised) {
        return;
    }

    RecordLoadedStartUIModule();

    TraceLog(L"hid init");
    if (!g_hidStopEvent) {

        TraceLog(L"hid thread not started (stop event unavailable)");
        return;
    }
    g_hidThread =
        CreateThread(nullptr, 0, HidInputThread, nullptr, 0, nullptr);
    if (!g_hidThread) {
        TraceLog(L"hid thread fail err=%u",
                static_cast<unsigned>(GetLastError()));
    }
}

void RoleBeforeUninit() {
    if (!g_initialised) {
        return;
    }
    g_unloadStartTick = GetTickCount64();
    TraceLog(L"unload begin tid=%u", GetCurrentThreadId());

    AcquireSRWLockExclusive(&g_startUIHookLock);
    g_disabled = true;
    ReleaseSRWLockExclusive(&g_startUIHookLock);
    g_unloading = true;

    RemoveStartAttachHook();

    g_stopDismissCallback = true;

    PublishPeerGone(L"unload");

    if (g_hidStopEvent) {
        SetEvent(g_hidStopEvent);
    }
    HWND hwnd = g_hidHwnd.load(std::memory_order_relaxed);
    if (hwnd) {

        PostMessageW(hwnd, WM_QUIT, 0, 0);
    }

    if (g_hidThread) {
        if (GetCurrentThreadId() ==
            g_hidThreadId.load(std::memory_order_relaxed)) {
            TraceLog(L"unload hid thread join refused: called on that thread");
        } else if (WaitForSingleObject(g_hidThread, INFINITE) ==
                   WAIT_OBJECT_0) {
            CloseHandle(g_hidThread);
            g_hidThread = nullptr;
            TraceLog(L"unload hid thread joined");
        } else {

            BlockUnload(L"hid thread join failed");
        }
    } else {
        TraceLog(L"unload hid thread joined");
    }
}

void RoleUninit() {
    if (!g_initialised) {
        return;
    }

    g_unloading = true;

    RemoveStartAttachHook();
    WaitForStartAttachFrames();

    PublishPeerGone(L"uninit");

    if (g_hidThread) {
        if (g_hidStopEvent) {
            SetEvent(g_hidStopEvent);
        }
        if (WaitForSingleObject(g_hidThread, INFINITE) == WAIT_OBJECT_0) {
            CloseHandle(g_hidThread);
            g_hidThread = nullptr;
            TraceLog(L"unload hid thread joined (uninit)");
        } else {
            BlockUnload(L"hid thread join failed (uninit)");
        }
    }

    DWORD uiOwner = 0;
    bool inlineUnload = false;
    HWND uiWindow = nullptr;
    {
        RootAdmissionExclusive admission;
        TraceLog(L"unload root admission exclusive");
        uiOwner = g_uiThreadId.load(std::memory_order_relaxed);
        inlineUnload =
            uiOwner != 0 && uiOwner == GetCurrentThreadId();
        if (inlineUnload) {
            TraceLog(L"unload inline on ui thread entries=%d",
                    g_moduleEntryCount.load(std::memory_order_acquire));
        }

        uiWindow = TeardownUiOnUiThread();
    }

    if (inlineUnload) {
        RequireNoInlineUiTurns();
    }

    if (!inlineUnload && !uiWindow &&
        g_uiTurnEverUsed.load(std::memory_order_acquire)) {
        BlockUnload(L"no window to drain module entries");
    }

    RequireNoModuleCallbacks(!inlineUnload);

    if (inlineUnload) {

        WaitForStartAttachFrames();
    } else if (uiWindow) {
        WaitForAttachFramesThenDrainUi(uiWindow);
    } else {

        WaitForStartAttachFrames();
        TraceLog(L"unload teardown without a window; no ui turn");
    }

    if (g_startAttachIdle) {
        CloseHandle(g_startAttachIdle);
        g_startAttachIdle = nullptr;
    }

    DestroyUiRendezvousWindow();

    ClearScrollerRetry();
    SwipeIpcClose(&g_ipc);
    ReleaseExecProcess();

    if (g_hidStopEvent) {
        CloseHandle(g_hidStopEvent);
        g_hidStopEvent = nullptr;
    }
    if (g_rendererEvent) {
        CloseHandle(g_rendererEvent);
        g_rendererEvent = nullptr;
    }
    if (g_uiTeardownAck) {
        CloseHandle(g_uiTeardownAck);
        g_uiTeardownAck = nullptr;
    }
    if (g_moduleEntriesGone) {
        CloseHandle(g_moduleEntriesGone);
        g_moduleEntriesGone = nullptr;
    }

    TraceLog(L"unload done elapsed=%llums",
            static_cast<unsigned long long>(
                g_unloadStartTick != 0 && GetTickCount64() >= g_unloadStartTick
                    ? GetTickCount64() - g_unloadStartTick
                    : 0));
    g_initialised = false;
    Wh_Log(L"Swipe for 10Start uninitialized (start role)");
}

void RoleSettingsChanged() {
    if (!g_initialised) {
        return;
    }
    LoadAnimationStyle();
    LoadGestureStartSpeed();
    LoadCommitDistance();
    RefreshTraceSetting();
}

const SwipeModRole Role = {RoleInit, RoleAfterInit, RoleBeforeUninit,
                           RoleUninit, RoleSettingsChanged};

}

namespace bridge {

void EnsureExecutorWindow();
DWORD NoteLauncherThread(const wchar_t* source, bool adopt);
void EnsureExecutorOnLauncherThread(const wchar_t* source);
LRESULT CALLBACK ExecutorThreadHook(int code, WPARAM wParam, LPARAM lParam);
bool EnsureExecutorThreadHook(DWORD target);
bool RemoveExecutorThreadHook();
void* LauncherNow(uint64_t* stateOut, void** outerOut = nullptr);
LONG64 PublishBridgeIntent(int32_t kind, int32_t trigger, uint32_t method,
                           uint32_t flags);
void CancelArmedDismissForNativeShow();

constexpr uintptr_t kRvaShow = 0x2516B0;
constexpr uintptr_t kRvaHideStartAndCortana = 0x2485E0;
constexpr uintptr_t kRvaOnStartShown = 0xF71C0;
constexpr uintptr_t kRvaOnStartHidden = 0x692B0;
constexpr uintptr_t kRvaOnHotkeyInvoke = 0xF6D2C;
constexpr uintptr_t kRvaOnStartButtonPressed = 0x4934E0;
constexpr uintptr_t kRvaDismiss = 0x25A060;
constexpr uintptr_t kRvaShowStartView = 0xF6EC0;
constexpr uintptr_t kRvaLauncherSlot = 0x95F0E8;
constexpr uintptr_t kRvaLauncherIsVisible = 0x1F6790;
constexpr uintptr_t kRvaDismissStartAndCortana = 0x499A10;
constexpr uintptr_t kRvaDismissSynchronously = 0x48FF20;
constexpr uintptr_t kRvaActiveZBand = 0x24C550;

constexpr uint64_t kTwinuiReferenceVersion =
    (static_cast<uint64_t>(10) << 48) |
    (static_cast<uint64_t>(26100) << 16) | 10068;
std::atomic<bool> g_launcherLayoutVerified{false};

uint64_t ModuleFixedFileVersion(HMODULE module) {
    using Query_t = BOOL(WINAPI*)(LPCVOID, LPCWSTR, LPVOID*, PUINT);
    static HMODULE versionDll = LoadLibraryW(L"version.dll");
    static Query_t query =
        versionDll ? reinterpret_cast<Query_t>(
                         GetProcAddress(versionDll, "VerQueryValueW"))
                   : nullptr;
    if (!module || !query) {
        return 0;
    }
    const HRSRC resource = FindResourceW(
        module, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION);
    if (!resource) {
        return 0;
    }
    const HGLOBAL loaded = LoadResource(module, resource);
    if (!loaded) {
        return 0;
    }
    const void* data = LockResource(loaded);
    if (!data) {
        return 0;
    }
    VS_FIXEDFILEINFO* info = nullptr;
    UINT length = 0;
    if (!query(data, L"\\", reinterpret_cast<void**>(&info), &length) ||
        !info || length < sizeof(VS_FIXEDFILEINFO) ||
        info->dwSignature != 0xFEEF04BD) {
        return 0;
    }
    return (static_cast<uint64_t>(info->dwFileVersionMS) << 32) |
           info->dwFileVersionLS;
}

constexpr uintptr_t kLauncherStatePointerOffset = 0x160;
constexpr uintptr_t kHotkeyLauncherOffset = 0x18;

constexpr uintptr_t kHotkeyShellOpenFlagOffset = 0x161;

constexpr uintptr_t kLauncherInternalStateOffset = 0x28;

constexpr wchar_t kTwinuiModuleName[] = L"twinui.pcshell.dll";

constexpr uint32_t kDismissMethodWin = 1;
constexpr uint32_t kDismissMethodWinTab = 5;
constexpr uint32_t kDismissMethodStartButton = 6;
constexpr uint32_t kDismissMethodClickAway = 7;

constexpr int32_t kPressEntryHotkey = 0;
constexpr int32_t kPressEntryButton = 1;
constexpr int32_t kPressEntryShow = 2;
constexpr int32_t kPressEntryDismiss = 3;
constexpr int32_t kPressEntryHide = 4;

SwipeIpcChannel g_ipc;

std::atomic<HWND> g_execWindow{nullptr};

DWORD g_execThreadId = 0;
std::atomic<DWORD> g_launcherThread{0};

std::atomic<bool> g_launcherProven{false};
thread_local int t_execCall = 0;
HANDLE g_execStopEvent = nullptr;

HANDLE g_execTeardownAck = nullptr;
int g_execOwnerDepth = 0;
bool g_execTeardownPending = false;

bool g_execRetirePending = false;

std::atomic<HWND> g_execRetirePosted{nullptr};

std::atomic<HHOOK> g_execThreadHook{nullptr};
std::atomic<DWORD> g_execThreadHookTarget{0};

std::atomic<int> g_execHookFrames{0};
HANDLE g_execHookIdle = nullptr;

void StopExecutorTeardown(HWND window);
void RetireExecutorTeardown(HWND window);
bool RemoveExecutorThreadHook();

constexpr UINT kExecTeardownMessage = WM_APP + 0x5B;
constexpr WPARAM kExecTeardownMagic = 0x53575444;

constexpr UINT kExecRetireMessage = WM_APP + 0x5F;
constexpr WPARAM kExecRetireMagic = 0x53575252;

constexpr UINT kExecDrainMessage = WM_APP + 0x60;
constexpr WPARAM kExecDrainMagic = 0x53575244;
constexpr int kExecDrainRetries = 8;

// The unload handshakes carry no pointer: a same-user process can reach this
// window, and these flags are the whole answer.
std::atomic<bool> g_execDrainServed{false};
std::atomic<bool> g_execDestroyServed{false};

class ExecDepthGuard {
  public:
    ExecDepthGuard() { ++g_execOwnerDepth; }
    ~ExecDepthGuard() {
        if (--g_execOwnerDepth > 0) {
            return;
        }
        if (g_execTeardownPending) {
            g_execTeardownPending = false;
            const HWND window = g_execWindow.load(std::memory_order_acquire);
            if (!window) {
                TraceLog(L"exec teardown post skipped hwnd=null; acknowledging");
                if (g_execTeardownAck) {
                    SetEvent(g_execTeardownAck);
                }
                return;
            }

            if (!PostMessageW(window, kExecTeardownMessage, kExecTeardownMagic,
                              0)) {
                const DWORD error = GetLastError();
                TraceLog(
                        L"exec teardown post failed hwnd=%p err=%u; "
                        L"acknowledging",
                        window, error);
                if (g_execTeardownAck) {
                    SetEvent(g_execTeardownAck);
                }
                return;
            }
            TraceLog(L"exec teardown posted hwnd=%p", window);
            return;
        }
        if (g_execRetirePending) {
            g_execRetirePending = false;
            const HWND window = g_execWindow.load(std::memory_order_acquire);
            if (window) {

                if (PostMessageW(window, kExecRetireMessage, kExecRetireMagic,
                                 0)) {
                    g_execRetirePosted.store(window, std::memory_order_release);
                    TraceLog(L"exec retire posted hwnd=%p", window);
                } else {

                    g_execRetirePosted.store(nullptr, std::memory_order_release);
                    TraceLog(L"exec retire post failed hwnd=%p err=%u", window,
                            GetLastError());
                }
            }
        }
    }

    ExecDepthGuard(const ExecDepthGuard&) = delete;
    ExecDepthGuard& operator=(const ExecDepthGuard&) = delete;
};
HANDLE g_execWorker = nullptr;
DWORD g_execWorkerId = 0;

extern "C" IMAGE_DOS_HEADER __ImageBase;
constexpr wchar_t kExecClassName[] = L"WindhawkSwipeFor10Start.Executor";
bool g_execClassRegistered = false;
HINSTANCE g_execClassInstance = nullptr;

SRWLOCK g_execAdmissionLock = SRWLOCK_INIT;

class ExecAdmissionShared {
  public:
    ExecAdmissionShared() {
        held_ = TryAcquireSRWLockShared(&g_execAdmissionLock) != 0;
    }
    ~ExecAdmissionShared() {
        if (held_) {
            ReleaseSRWLockShared(&g_execAdmissionLock);
        }
    }
    bool held() const { return held_; }
    ExecAdmissionShared(const ExecAdmissionShared&) = delete;
    ExecAdmissionShared& operator=(const ExecAdmissionShared&) = delete;

  private:
    bool held_ = false;
};

class ExecAdmissionExclusive {
  public:
    ExecAdmissionExclusive() {
        AcquireSRWLockExclusive(&g_execAdmissionLock);
    }
    ~ExecAdmissionExclusive() {
        ReleaseSRWLockExclusive(&g_execAdmissionLock);
    }
    ExecAdmissionExclusive(const ExecAdmissionExclusive&) = delete;
    ExecAdmissionExclusive& operator=(const ExecAdmissionExclusive&) = delete;
};

constexpr UINT kExecMessage = WM_APP + 0x5A;

constexpr UINT kExecLatchMessage = WM_APP + 0x5C;

constexpr UINT kExecIntentAckMessage = WM_APP + 0x5D;
constexpr UINT kExecDestroyMessage = WM_APP + 0x5E;
constexpr WPARAM kExecLatchMagic = 0x53574C41;
constexpr WPARAM kExecIntentAckMagic = 0x53574141;
constexpr WPARAM kExecDestroyMagic = 0x53574458;
constexpr int kExecDestroyRetries = 1000;

constexpr UINT_PTR kExecDismissTimer = 0x5358;
constexpr UINT_PTR kExecProbeTimer = 0x5359;
constexpr UINT_PTR kPressLatchTimer = 0x535A;
constexpr UINT_PTR kExecDismissProbeTimer = 0x535B;
constexpr UINT_PTR kPressAckTimer = 0x535C;

constexpr UINT_PTR kPressOpenGenTimer = 0x535D;
constexpr uint64_t kPressOpenGenPollMs = 20;
constexpr uint64_t kPressOpenGenMaxWaitMs = 900;
constexpr uint64_t kPressLatchTimeoutMs = 400;
constexpr uint64_t kPressLatchRetryMs = 100;

constexpr uint64_t kPressAckTimeoutMs = 1000;

constexpr uint64_t kPressReservePollMs = 100;
constexpr uint64_t kPressQueueWaitMs = 1500;

constexpr uint64_t kPressQueueStaleMs = kPressQueueWaitMs + kPressAckTimeoutMs;

constexpr uint64_t kClaimAbandonMs = 2000;
constexpr int kPressQueueCapacity = 4;

constexpr uint64_t kExecExpiryMs = 400;

constexpr UINT kExecDismissConfirmMs = 300;

std::atomic<bool> g_execReady{false};

std::atomic<bool> g_execDispatchClosed{false};

std::atomic<bool> g_execCapable{false};

SRWLOCK g_execCapabilityLock = SRWLOCK_INIT;

constexpr uint64_t kPressServeDelayMs = 50;
constexpr uint64_t kLauncherStateReadyFloor = 0x10000;

using LauncherIsVisibleFn = HRESULT(WINAPI*)(void*, BOOL*);

void** g_launcherSlot = nullptr;
LauncherIsVisibleFn g_launcherIsVisible = nullptr;

LONG64 g_pendingDismissSeq = 0;
LONG64 g_pendingShowSeq = 0;

LONG64 g_pendingDismissConfirmSeq = 0;
int32_t g_pendingDismissConfirmVisible = 0;

uint64_t g_pendingDismissConfirmTick = 0;

uint32_t g_pendingDismissEpoch = 0;

uint32_t g_dispatchEpoch = 0;

LONG64 g_execAcceptedSeq = 0;

LONG64 g_suppressedExecSeq = 0;

int g_dismissReadRetries = 0;
constexpr int kExecDismissReadRetries = 10;
constexpr uint32_t kExecDismissRetryMs = 10;

HMODULE g_explorerModuleRef = nullptr;
std::atomic<HMODULE> g_explorerSymbolClaim{nullptr};
std::atomic<bool> g_explorerSymbolsReady{false};
std::atomic<bool> g_explorerSymbolsRefused{false};
std::atomic<bool> g_afterInit{false};
std::atomic<bool> g_disabled{false};

SRWLOCK g_explorerSymbolLock = SRWLOCK_INIT;

constexpr uint64_t kExecSymbolProbeMs = 100;
std::atomic<bool> g_explorerSymbolAttempted{false};

std::atomic<bool> g_closeHookEnabled{false};

std::atomic<bool> g_initialised{false};

uint64_t g_unloadStartTick = 0;

using Show_t = HRESULT (*)(void*, uint8_t, uint8_t, uint32_t);
Show_t Show_Original = nullptr;

HRESULT Show_Hook(void* self, uint8_t a, uint8_t b, uint32_t c) {
    TraceLog(L"show a=%u b=%u c=%u tid=%u", static_cast<unsigned int>(a),
            static_cast<unsigned int>(b), static_cast<unsigned int>(c),
            GetCurrentThreadId());

    EnsureExecutorOnLauncherThread(L"show");

    return Show_Original ? Show_Original(self, a, b, c) : S_OK;
}

using HideStartAndCortana_t = void (*)(void*, uint32_t);
HideStartAndCortana_t HideStartAndCortana_Original = nullptr;

enum class OwnCaller : int {
    Ordinary = 0,
    ClickAway,
    Lock,
    Unknown,
};

thread_local int t_ownCaller = static_cast<int>(OwnCaller::Unknown);

struct OwnCallerScope {
    OwnCaller previous;
    explicit OwnCallerScope(OwnCaller caller)
        : previous(static_cast<OwnCaller>(t_ownCaller)) {
        t_ownCaller = static_cast<int>(caller);
    }
    ~OwnCallerScope() { t_ownCaller = static_cast<int>(previous); }
    OwnCallerScope(const OwnCallerScope&) = delete;
    OwnCallerScope& operator=(const OwnCallerScope&) = delete;
};

const wchar_t* OwnCallerName(OwnCaller caller) {
    switch (caller) {
        case OwnCaller::Ordinary:
            return L"ordinary";
        case OwnCaller::ClickAway:
            return L"clickaway";
        case OwnCaller::Lock:
            return L"lock";
        default:
            return L"unknown";
    }
}

bool CloseIntentFromCaller(uint32_t method, OwnCaller caller,
                           const wchar_t** reason) {
    if (method == kDismissMethodClickAway && caller != OwnCaller::ClickAway) {
        if (reason) {
            *reason = L"caller-scope";
        }
        return false;
    }
    if (!SwipeCloseIntentAllowed(method)) {
        if (reason) {
            *reason = L"unsupported";
        }
        return false;
    }
    return true;
}

using DismissFn = HRESULT (*)(void*, uint32_t);
using ShowStartViewFn = HRESULT (*)(void*, uint32_t, uint32_t);

DismissFn g_dismiss = nullptr;
ShowStartViewFn g_showStartView = nullptr;

SRWLOCK g_ownLock = SRWLOCK_INIT;
bool g_ownOpen = false;

std::atomic<bool> g_ownWinEnabled{false};
std::atomic<bool> g_ownButtonEnabled{false};
std::atomic<bool> g_ownEventsEnabled{false};
std::atomic<bool> g_ownPrimitivesEnabled{false};

std::atomic<bool> g_callersVerified{false};

thread_local uint32_t t_ownExternalMethod = 0;

struct OwnExternalForwardGuard {
    uint32_t previous;
    explicit OwnExternalForwardGuard(uint32_t method)
        : previous(t_ownExternalMethod) {
        t_ownExternalMethod = method;
    }
    ~OwnExternalForwardGuard() { t_ownExternalMethod = previous; }

    OwnExternalForwardGuard(const OwnExternalForwardGuard&) = delete;
    OwnExternalForwardGuard& operator=(const OwnExternalForwardGuard&) = delete;
};

constexpr uint32_t kOwnExternalForwardAny = 0xFFFFFFFFu;

bool OwnExternalForwarded(uint32_t method) {
    return t_ownExternalMethod != 0 &&
           (t_ownExternalMethod == kOwnExternalForwardAny ||
            t_ownExternalMethod == method);
}

bool OwnTakeExternalForward(uint32_t method) {
    if (!OwnExternalForwarded(method)) {
        return false;
    }
    t_ownExternalMethod = 0;
    return true;
}

void* OwnStateOf(void* launcher) {
    if (!launcher) {
        return nullptr;
    }
    return *reinterpret_cast<void* const*>(
        static_cast<const uint8_t*>(launcher) + kLauncherStatePointerOffset);
}

bool LauncherStateReady(uint64_t state) {
    return state > kLauncherStateReadyFloor;
}

bool OwnIsVisible(void* state) {
    if (!LauncherStateReady(
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(state)))) {
        return false;
    }
    void** vtable = *reinterpret_cast<void***>(state);
    if (!vtable) {
        return false;
    }
    const auto fn =
        reinterpret_cast<bool (*)(void*)>(vtable[0x70 / sizeof(void*)]);
    return fn && fn(state);
}

uint32_t OwnInternalState(void* state) {
    if (!LauncherStateReady(
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(state)))) {
        return 0;
    }
    return *reinterpret_cast<const volatile uint32_t*>(
        static_cast<const uint8_t*>(state) + kLauncherInternalStateOffset);
}

OwnCaller OwnCallerNow() {
    if (!g_callersVerified.load(std::memory_order_relaxed)) {
        return OwnCaller::Unknown;
    }
    return static_cast<OwnCaller>(t_ownCaller);
}

struct OwnSnapshot {
    bool ready;
    bool open;
    uint32_t state;
};

OwnSnapshot OwnSyncState(void* state) {
    const uint64_t raw =
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(state));
    if (!LauncherStateReady(raw)) {
        return OwnSnapshot{false, false, 0};
    }
    const bool shellOpen = OwnIsVisible(state);
    const uint32_t internal = OwnInternalState(state);

    bool resync = false;
    {
        AcquireSRWLockExclusive(&g_ownLock);
        if (shellOpen != g_ownOpen) {
            resync = true;
            g_ownOpen = shellOpen;
        }
        ReleaseSRWLockExclusive(&g_ownLock);
    }

    if (resync) {
        TraceLog(L"own state resync cached=%d shell=%d", shellOpen ? 0 : 1,
                shellOpen ? 1 : 0);
    }
    return OwnSnapshot{true, shellOpen, internal};
}

OwnSnapshot OwnSync(void* launcher) {
    return OwnSyncState(OwnStateOf(launcher));
}

void DispatchToggle(const OwnSnapshot& own, int32_t trigger, uint32_t showMethod,
                    uint32_t dismissMethod, uint32_t flags,
                    uint32_t admittingEpoch, int32_t entry,
                    bool pointer = false);

void OwnResyncFromState(void* state) {
    const uint64_t raw =
        static_cast<uint64_t>(reinterpret_cast<uintptr_t>(state));
    if (!LauncherStateReady(raw)) {
        TraceLog(L"own state resync skipped reason=state-not-ready raw=0x%llX",
                static_cast<unsigned long long>(raw));
        return;
    }
    const bool shellOpen = OwnIsVisible(state);
    bool resync = false;
    {
        AcquireSRWLockExclusive(&g_ownLock);
        if (shellOpen != g_ownOpen) {
            resync = true;
            g_ownOpen = shellOpen;
        }
        ReleaseSRWLockExclusive(&g_ownLock);
    }
    if (resync) {
        TraceLog(L"own state resync cached=%d shell=%d", shellOpen ? 0 : 1,
                shellOpen ? 1 : 0);
    }
}

void OwnEventShown() {
    bool changed = false;
    {
        AcquireSRWLockExclusive(&g_ownLock);
        if (!g_ownOpen) {
            g_ownOpen = true;
            changed = true;
        }
        ReleaseSRWLockExclusive(&g_ownLock);
    }
    if (changed) {
        TraceLog(L"own state event=shown open=1 tid=%u", GetCurrentThreadId());
    }
}

void OwnEventHidden() {
    bool changed = false;
    {
        AcquireSRWLockExclusive(&g_ownLock);
        if (g_ownOpen) {
            g_ownOpen = false;
            changed = true;
        }
        ReleaseSRWLockExclusive(&g_ownLock);
    }
    if (changed) {
        TraceLog(L"own state event=hidden open=0 tid=%u", GetCurrentThreadId());
    }
}

bool PeerAliveNow() {
    return SwipeIpcPeerAlive(&g_ipc, kSwipePeerStaleMs);
}

bool PeerStateAliveNow() {
    return g_ipc.state != nullptr &&
           SwipeIpcPeerState(&g_ipc) == kSwipePeerAlive;
}

uint32_t PeerEpochNow() {
    return g_ipc.state ? SwipeIpcPeerEpoch(&g_ipc) : 0;
}

DWORD PeerUiThreadNow() {
    return g_ipc.state ? static_cast<DWORD>(SwipeIpcPeerUiThread(&g_ipc)) : 0;
}

uint32_t PeerTickAgeMs() {
    if (!g_ipc.state) {
        return 0xFFFFFFFFu;
    }
    return static_cast<uint32_t>(GetTickCount64()) -
           SwipeIpcPeerTick(&g_ipc);
}

SRWLOCK g_peerProcessLock = SRWLOCK_INIT;
HANDLE g_peerProcess = nullptr;
uint32_t g_peerProcessPid = 0;
uint32_t g_peerProcessEpoch = 0;

void ClosePeerProcessLocked() {
    if (g_peerProcess) {
        CloseHandle(g_peerProcess);
        g_peerProcess = nullptr;
    }
    g_peerProcessPid = 0;
    g_peerProcessEpoch = 0;
}

enum class PeerProcess : int { Unknown = 0, Alive, Dead };

PeerProcess PeerProcessProbe() {
    if (!g_ipc.state || g_disabled.load(std::memory_order_relaxed)) {
        return PeerProcess::Unknown;
    }
    const uint32_t epoch = SwipeIpcPeerEpoch(&g_ipc);
    const uint32_t pid = SwipeIpcPeerPid(&g_ipc);
    if (epoch == 0 || pid == 0 || SwipeIpcPeerEpoch(&g_ipc) != epoch) {
        return PeerProcess::Unknown;
    }
    PeerProcess answer = PeerProcess::Unknown;
    AcquireSRWLockExclusive(&g_peerProcessLock);
    if (g_peerProcessEpoch != 0 &&
        (g_peerProcessEpoch != epoch || g_peerProcessPid != pid)) {

        ClosePeerProcessLocked();
    }
    if (!g_peerProcess) {
        HANDLE process = OpenProcess(SYNCHRONIZE, FALSE, pid);
        if (!process) {
            const DWORD error = GetLastError();
            TraceLog(L"peer process open failed pid=%u epoch=%u err=%u", pid,
                    epoch, error);
            ReleaseSRWLockExclusive(&g_peerProcessLock);
            return error == ERROR_INVALID_PARAMETER ? PeerProcess::Dead
                                                    : PeerProcess::Unknown;
        }
        g_peerProcess = process;
        g_peerProcessPid = pid;
        g_peerProcessEpoch = epoch;
    }
    const DWORD wait = WaitForSingleObject(g_peerProcess, 0);
    answer = wait == WAIT_OBJECT_0
                 ? PeerProcess::Dead
                 : (wait == WAIT_TIMEOUT ? PeerProcess::Alive
                                         : PeerProcess::Unknown);
    ReleaseSRWLockExclusive(&g_peerProcessLock);
    return answer;
}

PeerProcess WarmPeerProcess() {
    return PeerProcessProbe();
}

void ReleasePeerProcess() {
    AcquireSRWLockExclusive(&g_peerProcessLock);
    ClosePeerProcessLocked();
    ReleaseSRWLockExclusive(&g_peerProcessLock);
}

bool ExecutorWorkerReady() {
    return g_ipc.state != nullptr &&
           g_execReady.load(std::memory_order_acquire) &&
           !g_disabled.load(std::memory_order_relaxed);
}

bool ExecutorCapabilityNow() {
    // A takeover cancels an intent by setting the channel's intent event. A
    // bridge without that event can publish a show nothing can cancel, so it
    // never reports itself capable, whatever else is ready.
    if (!g_ipc.state || !g_ipc.intentEvent) {
        return false;
    }
    if (!ExecutorWorkerReady() ||
        g_execDispatchClosed.load(std::memory_order_acquire) ||
        !g_launcherProven.load(std::memory_order_acquire) ||
        !g_launcherLayoutVerified.load(std::memory_order_acquire)) {
        return false;
    }
    if (!g_execWindow.load(std::memory_order_acquire)) {
        return false;
    }
    const DWORD launcher = g_launcherThread.load(std::memory_order_relaxed);
    if (launcher == 0 || g_execThreadId == 0 || g_execThreadId != launcher) {
        return false;
    }
    return g_dismiss && g_showStartView && g_launcherSlot &&
           g_launcherIsVisible;
}

void PublishExecutorCapability(const wchar_t* reason) {
    if (!g_ipc.state) {
        return;
    }

    AcquireSRWLockExclusive(&g_execCapabilityLock);
    const bool capable = ExecutorCapabilityNow();
    const bool was = g_execCapable.exchange(capable, std::memory_order_acq_rel);
    SwipeIpcPublishExecCapable(&g_ipc, capable);
    ReleaseSRWLockExclusive(&g_execCapabilityLock);
    if (was != capable) {
        TraceLog(L"exec capable=%d reason=%s window=%p tid=%u launcher=%u "
                L"layout=%d worker=%d",
                capable ? 1 : 0, reason,
                g_execWindow.load(std::memory_order_relaxed), g_execThreadId,
                g_launcherThread.load(std::memory_order_relaxed),
                g_launcherLayoutVerified.load(std::memory_order_relaxed) ? 1 : 0,
                g_execReady.load(std::memory_order_relaxed) ? 1 : 0);
    }
}

bool ExecutorWindowOnThisThread() {
    if (!g_execWindow.load(std::memory_order_acquire)) {
        return false;
    }
    return g_execThreadId == GetCurrentThreadId();
}

bool LauncherTriggerMayAdopt(const wchar_t* trigger) {
    return wcscmp(trigger, L"win") == 0 ||
           wcscmp(trigger, L"startbutton") == 0;
}

bool OnRecordedLauncherThread() {
    const DWORD known = g_launcherThread.load(std::memory_order_relaxed);
    return known != 0 && known == GetCurrentThreadId();
}

void ReplaceLauncherThread(const wchar_t* source, DWORD previous);

void ProveLauncherEntry(const wchar_t* source) {
    const DWORD tid = GetCurrentThreadId();
    DWORD known = g_launcherThread.load(std::memory_order_acquire);
    if (known == 0 && g_launcherThread.compare_exchange_strong(known, tid)) {
        known = tid;
        TraceLog(L"exec launcher thread tid=%u source=%s", tid, source);
    }
    if (known != tid) {
        if (PeerUiThreadNow() != 0 &&
            !g_launcherProven.load(std::memory_order_acquire)) {

            ReplaceLauncherThread(source, known);
        } else {
            TraceLog(L"exec launcher thread mismatch known=%u tid=%u source=%s",
                    known, tid, source);
        }
        return;
    }
    if (!g_launcherProven.exchange(true, std::memory_order_release)) {
        TraceLog(L"exec launcher thread proven tid=%u source=%s", tid, source);

        PublishExecutorCapability(L"proven");
    }
}

bool PressQueueHeld();

bool BridgeCanServeOnLauncherThread(const wchar_t* trigger,
                                    uint32_t* epochOut) {
    if (epochOut) {
        *epochOut = 0;
    }
    if (!PeerAliveNow()) {

        if (OnRecordedLauncherThread() && PressQueueHeld()) {
            TraceLog(L"own pass trigger=%s reason=peer-stale-queued", trigger);
        } else {

            TraceLog(L"own pass trigger=%s reason=no-peer", trigger);
            return false;
        }
    }
    const uint32_t epoch = PeerEpochNow();

    const PeerProcess process = WarmPeerProcess();
    if (process != PeerProcess::Alive) {
        const wchar_t* processName =
            process == PeerProcess::Dead ? L"dead" : L"unknown";

        if (OnRecordedLauncherThread() && PressQueueHeld()) {
            TraceLog(L"own pass trigger=%s reason=process-%s-queued", trigger,
                    processName);
        } else {
            TraceLog(L"own pass trigger=%s reason=process-%s", trigger,
                    processName);
            return false;
        }
    }
    if (!ExecutorWorkerReady()) {
        TraceLog(L"own pass trigger=%s reason=no-executor", trigger);
        return false;
    }
    if (NoteLauncherThread(trigger, LauncherTriggerMayAdopt(trigger)) == 0) {

        TraceLog(L"own pass trigger=%s reason=wrong-thread", trigger);
        return false;
    }
    EnsureExecutorWindow();
    if (!ExecutorWindowOnThisThread()) {
        TraceLog(L"own pass trigger=%s reason=no-window", trigger);
        return false;
    }

    PublishExecutorCapability(L"press");
    if (!ExecutorCapabilityNow()) {

        TraceLog(L"own pass trigger=%s reason=no-capability", trigger);
        return false;
    }
    if (epochOut) {
        *epochOut = epoch;
    }
    return true;
}

bool HideCanServeClose(const wchar_t** reason) {
    if (!PeerAliveNow()) {

        if (OnRecordedLauncherThread() && PressQueueHeld()) {
            TraceLog(L"exec hide peer-stale-queued");
        } else {
            *reason = L"no-peer";
            return false;
        }
    }
    if (!g_callersVerified.load(std::memory_order_relaxed)) {

        *reason = L"callers-unverified";
        return false;
    }

    const PeerProcess process = WarmPeerProcess();
    if (process != PeerProcess::Alive) {
        const wchar_t* processName =
            process == PeerProcess::Dead ? L"process-dead" : L"process-unknown";
        if (OnRecordedLauncherThread() && PressQueueHeld()) {
            TraceLog(L"exec hide process-held reason=%s", processName);
        } else {
            *reason = processName;
            return false;
        }
    }
    if (!ExecutorWorkerReady()) {
        *reason = L"no-executor";
        return false;
    }
    if (!OnRecordedLauncherThread()) {

        *reason = L"wrong-thread";
        return false;
    }
    if (!ExecutorWindowOnThisThread()) {
        *reason = L"no-window";
        return false;
    }

    PublishExecutorCapability(L"hide");
    if (!ExecutorCapabilityNow()) {
        *reason = L"no-capability";
        return false;
    }
    if (!g_dismiss) {

        *reason = L"no-primitive";
        return false;
    }
    return true;
}

bool ClickAwayReopensNow() {

    const int primary =
        GetSystemMetrics(SM_SWAPBUTTON) != 0 ? VK_RBUTTON : VK_LBUTTON;
    if ((GetAsyncKeyState(primary) & 0x8000) == 0) {
        return false;
    }
    POINT point{};
    if (!GetCursorPos(&point)) {
        return false;
    }
    const HWND under = WindowFromPoint(point);
    if (!under) {
        return false;
    }

    const HWND root = GetAncestor(under, GA_ROOT);
    if (!root) {
        return false;
    }
    wchar_t className[64]{};
    if (GetClassNameW(root, className, ARRAYSIZE(className)) == 0) {
        return false;
    }
    if (wcscmp(className, L"Shell_TrayWnd") == 0 ||
        wcscmp(className, L"Shell_SecondaryTrayWnd") == 0) {
        return false;
    }

    // Only the menu's own window or the desktop can reverse a close; a click on
    // another app keeps its close.
    DWORD pid = 0;
    GetWindowThreadProcessId(root, &pid);
    if (pid != 0 && pid == SwipeIpcPeerPid(&g_ipc)) {
        return true;
    }
    return root == GetShellWindow() || wcscmp(className, L"Progman") == 0 ||
           wcscmp(className, L"WorkerW") == 0;
}

void HideStartAndCortana_Hook(void* self, uint32_t method) {
    auto callOriginal = [self, method]() {
        if (HideStartAndCortana_Original) {
            HideStartAndCortana_Original(self, method);
        }
    };
    if (!g_closeHookEnabled.load(std::memory_order_relaxed) ||
        g_disabled.load(std::memory_order_relaxed)) {
        callOriginal();
        return;
    }

    if (method == kDismissMethodClickAway &&
        OwnCallerNow() == OwnCaller::ClickAway && !OwnExternalForwarded(method)) {
        ProveLauncherEntry(L"clickaway");
    }
    if (t_execCall > 0) {
        const uint32_t execState = OwnInternalState(self);
        const OwnCaller execCaller = OwnCallerNow();

        if (method == kDismissMethodClickAway &&
            (execState == 1 || execState == 3) &&
            execCaller == OwnCaller::Unknown) {
            TraceLog(L"exec swallow hide caller=%s method=%u state=%u depth=%d "
                    L"reason=own-show",
                    OwnCallerName(execCaller), method, execState, t_execCall);
            return;
        }
        if (execCaller == OwnCaller::ClickAway &&
            method == kDismissMethodClickAway) {

            if (execState == 1 || execState == 3) {
                TraceLog(L"intent ignore trigger=clickaway caller=%s method=%u "
                        L"state=%u depth=%d reason=opening",
                        OwnCallerName(execCaller), method, execState, t_execCall);
                return;
            }
            if (SwipeIpcControllerTouch(&g_ipc)) {
                TraceLog(L"intent ignore trigger=clickaway caller=%s method=%u "
                        L"state=%u depth=%d reason=touch-owner",
                        OwnCallerName(execCaller), method, execState, t_execCall);
                return;
            }
            const OwnSnapshot clickOwn = OwnSyncState(self);
            if (!clickOwn.ready) {
                TraceLog(L"intent pass clickaway method=%u reason=state-not-ready",
                        method);
                callOriginal();
                OwnResyncFromState(self);
                return;
            }
            TraceLog(L"own hide clickaway state=%u depth=%d reason=executor",
                    execState, t_execCall);
            DispatchToggle(clickOwn, kSwipeTriggerClickAway,
                           kSwipeShowMethodStartButton, method, 0,
                           PeerEpochNow(), kPressEntryHide,
                           ClickAwayReopensNow());
            return;
        }
        TraceLog(L"exec pass hide caller=%s method=%u state=%u depth=%d "
                L"reason=executor",
                OwnCallerName(execCaller), method, execState, t_execCall);
        callOriginal();
        OwnResyncFromState(self);
        return;
    }

    if (OwnTakeExternalForward(method)) {
        TraceLog(L"exec pass hide method=%u reason=forwarded", method);
        callOriginal();
        OwnResyncFromState(self);
        return;
    }

    const wchar_t* reason = nullptr;
    if (!HideCanServeClose(&reason)) {
        TraceLog(L"exec pass hide method=%u reason=%s", method,
                reason ? reason : L"unknown");
        callOriginal();
        OwnResyncFromState(self);
        return;
    }

    const OwnCaller caller = OwnCallerNow();

    if (caller == OwnCaller::ClickAway && method != kDismissMethodClickAway) {
        TraceLog(L"intent external trigger=hide caller=unknown method=%u "
                L"reason=unexpected-method",
                method);
        PublishBridgeIntent(kSwipeIntentExternal, kSwipeTriggerOverride,
                            method, 0);
        callOriginal();
        OwnResyncFromState(self);
        return;
    }

    if (caller == OwnCaller::Lock || caller == OwnCaller::Unknown ||
        method == kDismissMethodWinTab) {
        TraceLog(L"intent external trigger=hide caller=%s method=%u",
                OwnCallerName(caller), method);
        PublishBridgeIntent(kSwipeIntentExternal, kSwipeTriggerOverride,
                            method, 0);
        callOriginal();
        OwnResyncFromState(self);
        return;
    }

    const uint32_t currentState = OwnInternalState(self);

    if (caller == OwnCaller::ClickAway &&
        method == kDismissMethodClickAway &&
        (currentState == 1 || currentState == 3)) {
        TraceLog(L"intent ignore trigger=clickaway caller=%s method=%u state=%u "
                L"depth=%d reason=opening",
                OwnCallerName(caller), method, currentState, t_execCall);
        return;
    }
    if (caller == OwnCaller::ClickAway &&
        method == kDismissMethodClickAway &&
        SwipeIpcControllerTouch(&g_ipc)) {
        TraceLog(L"intent ignore trigger=clickaway caller=%s method=%u state=%u "
                L"depth=%d reason=touch-owner",
                OwnCallerName(caller), method, currentState, t_execCall);
        return;
    }

    if (caller == OwnCaller::ClickAway) {
        const OwnSnapshot clickOwn = OwnSyncState(self);
        if (!clickOwn.ready) {
            TraceLog(L"intent pass clickaway method=%u reason=state-not-ready",
                    method);
            callOriginal();
            OwnResyncFromState(self);
            return;
        }

        DispatchToggle(clickOwn, kSwipeTriggerClickAway,
                       kSwipeShowMethodStartButton,
                       kDismissMethodClickAway, 0, PeerEpochNow(),
                       kPressEntryHide, ClickAwayReopensNow());
        return;
    }

    const int32_t trigger = kSwipeTriggerWin;

    const wchar_t* methodReason = nullptr;
    if (!CloseIntentFromCaller(method, caller, &methodReason)) {
        TraceLog(L"intent pass close trigger=%s caller=%s method=%u reason=%s",
                trigger == kSwipeTriggerClickAway ? L"clickaway" : L"win",
                OwnCallerName(caller), method, methodReason);
        callOriginal();
        OwnResyncFromState(self);
        return;
    }

    const OwnSnapshot own = OwnSyncState(self);
    if (!own.ready) {
        TraceLog(L"intent pass close trigger=win caller=%s method=%u "
                L"reason=state-not-ready",
                OwnCallerName(caller), method);
        callOriginal();
        OwnResyncFromState(self);
        return;
    }
    DispatchToggle(own, kSwipeTriggerWin, 0, method, 0, PeerEpochNow(),
                   kPressEntryHide);
}

bool IntentShouldClose(const OwnSnapshot& own, int32_t direction) {
    if (direction != kSwipeDirectionUnowned) {
        return direction != kSwipeDirectionClosing;
    }
    if (own.open) {
        return true;
    }
    return own.state == 1 || own.state == 3 || own.state == 5;
}

struct QueuedPress {
    int32_t trigger = kSwipeTriggerWin;

    int32_t entry = kPressEntryHotkey;
    uint32_t showMethod = 0;
    uint32_t dismissMethod = 0;
    uint32_t flags = 0;
    uint32_t admittingEpoch = 0;
    uint64_t admittedTick = 0;

    uint64_t servableTick = 0;

    bool pointer = false;
};

const wchar_t* PressEntryName(int32_t entry) {
    switch (entry) {
        case kPressEntryButton:
            return L"button";
        case kPressEntryShow:
            return L"show";
        case kPressEntryDismiss:
            return L"dismiss";
        case kPressEntryHide:
            return L"hide";
        default:
            return L"hotkey";
    }
}

void RunPressOriginal(const QueuedPress& press, const wchar_t* reason);

LONG64 PublishPressIntent(int32_t kind, const QueuedPress& press,
                          const wchar_t* source);

void ServeNextPress(const wchar_t* reason);

constexpr int kPressLatchCapacity = kPressQueueCapacity;

struct PressLatch {
    bool pending = false;
    uint64_t tick = 0;
    int32_t count = 0;
    QueuedPress presses[kPressLatchCapacity];
};

PressLatch g_pressLatch;

bool g_pressOpenGenPending = false;
bool g_pressOpenGenHeld = false;
uint64_t g_pressOpenGenBefore = 0;
uint64_t g_pressOpenGenDeadline = 0;

uint64_t CurrentOpenGeneration() {
    uint64_t generation = 0;
    uint64_t tick = 0;
    if (!SwipeIpcReadOpen(&g_ipc, &generation, &tick)) {
        return 0;
    }
    return generation;
}

bool OpenGenerationAdvanced() {
    const uint64_t generation = CurrentOpenGeneration();
    return generation != 0 && generation != g_pressOpenGenBefore;
}

void BeginPressOpenGeneration(uint64_t before) {
    g_pressOpenGenPending = true;
    g_pressOpenGenHeld = false;
    g_pressOpenGenBefore = before;
    g_pressOpenGenDeadline = 0;
}

void ClearPressOpenGeneration() {
    g_pressOpenGenPending = false;
    g_pressOpenGenHeld = false;
    g_pressOpenGenBefore = 0;
    g_pressOpenGenDeadline = 0;
}

bool ArmPressOpenGenTimer() {
    HWND window = g_execWindow.load(std::memory_order_relaxed);
    return window && SetTimer(window, kPressOpenGenTimer,
                              static_cast<UINT>(kPressOpenGenPollMs),
                              nullptr) != 0;
}

bool OpenGenerationGateHolds() {
    if (!g_pressOpenGenPending) {
        return false;
    }
    const uint64_t now = GetTickCount64();
    if (g_pressOpenGenDeadline == 0) {
        g_pressOpenGenDeadline = now + kPressOpenGenMaxWaitMs;
    }
    const uint64_t wait =
        now - (g_pressOpenGenDeadline - kPressOpenGenMaxWaitMs);
    if (OpenGenerationAdvanced()) {
        TraceLog(L"own press open gen ready gen=%llu wait=%llu",
                static_cast<unsigned long long>(CurrentOpenGeneration()),
                static_cast<unsigned long long>(wait));
        ClearPressOpenGeneration();
        return false;
    }
    if (now >= g_pressOpenGenDeadline) {

        TraceLog(L"own press open gen expired before=%llu wait=%llu",
                static_cast<unsigned long long>(g_pressOpenGenBefore),
                static_cast<unsigned long long>(wait));
        ClearPressOpenGeneration();
        return false;
    }
    if (!ArmPressOpenGenTimer()) {

        TraceLog(L"own press open gen timer failed err=%u before=%llu wait=%llu",
                GetLastError(),
                static_cast<unsigned long long>(g_pressOpenGenBefore),
                static_cast<unsigned long long>(wait));
        ClearPressOpenGeneration();
        return false;
    }
    return true;
}

const wchar_t* g_pressServeReason = L"timeout";

constexpr uint64_t kPressLatchMaxWaitMs = 1200;

bool ServeLatchedPress(const wchar_t* reason);

bool QueuePress(const QueuedPress& press, const wchar_t* source);

void DropPressLatch(const wchar_t* reason, const wchar_t* detail) {
    if (!g_pressLatch.pending) {
        return;
    }
    TraceLog(L"own press latch dropped reason=%s %s owed=%d wait=%llu", reason,
            detail, g_pressLatch.count,
            static_cast<unsigned long long>(GetTickCount64() -
                                            g_pressLatch.tick));
    g_pressLatch = PressLatch{};
}

const wchar_t* PressTriggerName(int32_t trigger) {
    switch (trigger) {
        case kSwipeTriggerStartButton:
            return L"startbutton";
        case kSwipeTriggerClickAway:
            return L"clickaway";
        case kSwipeTriggerTouch:
            return L"touch";
        case kSwipeTriggerOverride:
            return L"override";
        default:
            return L"win";
    }
}

bool ShellHideInFlight(const OwnSnapshot& own) {
    return own.state == 9 || own.state == 11;
}

void LatchPress(const OwnSnapshot& own, const QueuedPress& press) {
    const uint64_t now = GetTickCount64();
    HWND window = g_execWindow.load(std::memory_order_relaxed);
    if (!window) {

        TraceLog(L"own press latch dropped reason=no-window state=%u",
                own.state);
        RunPressOriginal(press, L"no-window");
        return;
    }
    if (g_pressLatch.pending &&
        now > g_pressLatch.tick && now - g_pressLatch.tick > kPressLatchMaxWaitMs) {

        DropPressLatch(L"expired", L"new-press");
    }
    if (g_pressLatch.pending) {
        if (g_pressLatch.count >= kPressLatchCapacity) {

            TraceLog(L"own press latch dropped trigger=%s entry=%s "
                    L"reason=latch-full owed=%d state=%u",
                    PressTriggerName(press.trigger),
                    PressEntryName(press.entry), g_pressLatch.count, own.state);
            return;
        }
        g_pressLatch.presses[g_pressLatch.count++] = press;
        TraceLog(L"own press latch owed count=%d state=%u trigger=%s entry=%s "
                L"epoch=%u",
                g_pressLatch.count, own.state,
                PressTriggerName(press.trigger), PressEntryName(press.entry),
                press.admittingEpoch);

        return;
    }
    g_pressLatch = PressLatch{};
    g_pressLatch.pending = true;
    g_pressLatch.tick = now;
    g_pressLatch.count = 1;
    g_pressLatch.presses[0] = press;
    TraceLog(L"own press latched state=%u trigger=%s entry=%s tick=%llu "
            L"epoch=%u owed=1 (hide or dismiss call in flight)",
            own.state, PressTriggerName(press.trigger),
            PressEntryName(press.entry),
            static_cast<unsigned long long>(now), press.admittingEpoch);
    g_pressServeReason = L"timeout";
    if (SetTimer(window, kPressLatchTimer, kPressLatchTimeoutMs, nullptr) == 0) {

        TraceLog(L"own press latch timer failed err=%u", GetLastError());
        DropPressLatch(L"timer-failed", L"latch");
        RunPressOriginal(press, L"latch-timer");
    }
}

void ArmLatchedServe() {
    HWND window = g_execWindow.load(std::memory_order_relaxed);
    if (!ExecutorWindowOnThisThread()) {
        if (!window) {

            return;
        }
        if (!PostMessageW(window, kExecLatchMessage, kExecLatchMagic, 0)) {
            TraceLog(L"own press latch arm post failed err=%u", GetLastError());
        } else {
            TraceLog(L"own press latch arm posted tid=%u owner=%u",
                    GetCurrentThreadId(), g_execThreadId);
        }
        return;
    }
    if (!g_pressLatch.pending) {
        return;
    }
    if (!window) {
        DropPressLatch(L"no-window", L"hidden");
        return;
    }
    g_pressServeReason = L"hidden";
    if (SetTimer(window, kPressLatchTimer,
                 static_cast<UINT>(kPressServeDelayMs), nullptr) == 0) {

        DropPressLatch(L"timer-failed", L"hidden");
    }
}

bool ServeLatchedPress(const wchar_t* reason) {
    if (!g_pressLatch.pending) {
        return false;
    }
    const uint64_t now = GetTickCount64();
    if (now > g_pressLatch.tick && now - g_pressLatch.tick > kPressLatchMaxWaitMs) {
        DropPressLatch(L"expired", reason);
        return false;
    }
    if (!PeerAliveNow() || !ExecutorCapabilityNow()) {
        DropPressLatch(L"peer-gone", reason);
        return false;
    }

    const PeerProcess process = WarmPeerProcess();
    if (process != PeerProcess::Alive) {
        DropPressLatch(process == PeerProcess::Dead ? L"process-dead"
                                                    : L"process-unknown",
                       reason);
        return false;
    }
    const uint32_t epoch = PeerEpochNow();
    if (epoch != g_pressLatch.presses[0].admittingEpoch) {
        wchar_t detail[80] = {};
        swprintf_s(detail, L"latch_epoch=%u now=%u",
                   g_pressLatch.presses[0].admittingEpoch, epoch);
        DropPressLatch(L"peer-changed", detail);
        return false;
    }
    uint64_t state = 0;
    void* launcher = LauncherNow(&state);
    if (!launcher) {
        TraceLog(L"own press serve refused reason=%s launcher=0 state=0x%llX",
                reason, static_cast<unsigned long long>(state));
        return false;
    }
    const OwnSnapshot own = OwnSync(launcher);
    if (!own.ready) {
        return false;
    }
    if (own.open || ShellHideInFlight(own)) {

        return false;
    }
    const PressLatch latch = g_pressLatch;
    g_pressLatch = PressLatch{};
    const uint64_t wait = GetTickCount64() - latch.tick;
    const QueuedPress& first = latch.presses[0];

    const uint64_t openBefore = CurrentOpenGeneration();
    const LONG64 sequence = PublishPressIntent(kSwipeIntentOpen, first, L"latch");
    if (sequence <= 0) {
        RunPressOriginal(first, L"latch-publish");
    } else {
        BeginPressOpenGeneration(openBefore);
    }
    for (int i = 1; i < latch.count; ++i) {
        QueuePress(latch.presses[i], L"latch");
    }
    TraceLog(L"own press served wait=%llu state=%u trigger=%s entry=%s "
            L"reason=%s epoch=%u owed=%d",
            static_cast<unsigned long long>(wait), own.state,
            PressTriggerName(first.trigger), PressEntryName(first.entry), reason,
            epoch, latch.count);
    if (sequence <= 0) {

        ServeNextPress(L"latch");
    }
    return true;
}

QueuedPress g_pressQueue[kPressQueueCapacity];
int g_pressQueueHead = 0;
int g_pressQueueCount = 0;

struct OutstandingPress {
    bool pending = false;
    LONG64 seq = 0;
    uint64_t publishedTick = 0;
    QueuedPress press{};

    bool external = false;

    bool aborted = false;

    uint32_t publishedEpoch = 0;

    bool consumedAwaitingAck = false;
};
OutstandingPress g_pressOutstanding;

bool PressQueueHeld() {
    return g_pressOutstanding.pending || g_pressQueueCount > 0;
}

void DropPressQueue(const wchar_t* reason) {
    int dropped = 0;
    ClearPressOpenGeneration();
    HWND gateWindow = g_execWindow.load(std::memory_order_relaxed);
    if (gateWindow) {
        KillTimer(gateWindow, kPressOpenGenTimer);
    }
    while (g_pressQueueCount > 0) {
        const QueuedPress& press = g_pressQueue[g_pressQueueHead];
        TraceLog(L"own press dropped trigger=%s entry=%s reason=%s wait=%llu",
                PressTriggerName(press.trigger), PressEntryName(press.entry),
                reason,
                static_cast<unsigned long long>(GetTickCount64() -
                                                press.admittedTick));
        g_pressQueueHead = (g_pressQueueHead + 1) % kPressQueueCapacity;
        --g_pressQueueCount;
        ++dropped;
    }
    if (g_pressOutstanding.pending) {
        HWND window = g_execWindow.load(std::memory_order_relaxed);
        if (window) {
            KillTimer(window, kPressAckTimer);
        }
        if (g_pressOutstanding.external) {
            TraceLog(L"own external dropped seq=%lld reason=%s wait=%llu",
                    static_cast<long long>(g_pressOutstanding.seq), reason,
                    static_cast<unsigned long long>(
                        GetTickCount64() - g_pressOutstanding.publishedTick));
        } else {
            TraceLog(L"own press dropped trigger=%s entry=%s reason=%s "
                    L"seq=%lld wait=%llu",
                    PressTriggerName(g_pressOutstanding.press.trigger),
                    PressEntryName(g_pressOutstanding.press.entry), reason,
                    static_cast<long long>(g_pressOutstanding.seq),
                    static_cast<unsigned long long>(
                        GetTickCount64() -
                        g_pressOutstanding.publishedTick));
        }
        g_pressOutstanding = OutstandingPress{};
        ++dropped;
    }
    if (dropped > 1) {
        TraceLog(L"own press queue cleared count=%d reason=%s", dropped, reason);
    }
}

bool ArmConsumedPressPoll() {
    HWND window = g_execWindow.load(std::memory_order_relaxed);
    return window && SetTimer(window, kPressAckTimer,
                              static_cast<UINT>(kPressReservePollMs),
                              nullptr) != 0;
}

bool QueuePress(const QueuedPress& press, const wchar_t* source) {
    if (g_pressQueueCount >= kPressQueueCapacity) {
        TraceLog(L"own press dropped trigger=%s entry=%s reason=queue-full q=%d "
                L"outstanding=%d source=%s",
                PressTriggerName(press.trigger), PressEntryName(press.entry),
                g_pressQueueCount, g_pressOutstanding.pending ? 1 : 0, source);
        return false;
    }
    const int tail = (g_pressQueueHead + g_pressQueueCount) % kPressQueueCapacity;
    g_pressQueue[tail] = press;
    ++g_pressQueueCount;

    const bool executing =
        g_pressOutstanding.pending && !g_pressOutstanding.external;
    if (g_pressQueueCount == 1 && !executing) {
        g_pressQueue[tail].servableTick = GetTickCount64();
    }
    TraceLog(L"own press queued trigger=%s entry=%s q=%d outstanding=%d "
            L"seq=%lld source=%s",
            PressTriggerName(press.trigger), PressEntryName(press.entry),
            g_pressQueueCount, g_pressOutstanding.pending ? 1 : 0,
            static_cast<long long>(g_pressOutstanding.seq), source);
    if (g_pressOutstanding.pending && g_pressOutstanding.consumedAwaitingAck &&
        !ArmConsumedPressPoll()) {

        TraceLog(L"own press poll timer failed err=%u q=%d", GetLastError(),
                g_pressQueueCount);
    }
    return true;
}

void RequeuePressFront(const QueuedPress& press) {
    g_pressQueueHead =
        (g_pressQueueHead + kPressQueueCapacity - 1) % kPressQueueCapacity;
    g_pressQueue[g_pressQueueHead] = press;
    g_pressQueue[g_pressQueueHead].servableTick = GetTickCount64();
    ++g_pressQueueCount;
}

bool ReplayPastBound(const QueuedPress& press) {
    if (press.admittedTick == 0) {
        return false;
    }
    const uint64_t now = GetTickCount64();
    return now > press.admittedTick &&
           now - press.admittedTick > kPressQueueStaleMs;
}

void DropPastBoundReplay(const QueuedPress& press) {
    const uint64_t now = GetTickCount64();
    TraceLog(L"own press dropped trigger=%s entry=%s reason=stale wait=%llu",
            PressTriggerName(press.trigger), PressEntryName(press.entry),
            static_cast<unsigned long long>(
                now > press.admittedTick ? now - press.admittedTick : 0));
}

bool DropStaleQueuedPress() {
    if (g_pressQueueCount == 0) {
        return false;
    }
    const QueuedPress& head = g_pressQueue[g_pressQueueHead];
    const uint64_t now = GetTickCount64();
    if (now <= head.admittedTick ||
        now - head.admittedTick <= kPressQueueStaleMs) {
        return false;
    }
    TraceLog(L"own press dropped trigger=%s entry=%s reason=stale wait=%llu "
            L"q=%d",
            PressTriggerName(head.trigger), PressEntryName(head.entry),
            static_cast<unsigned long long>(now - head.admittedTick),
            g_pressQueueCount - 1);
    g_pressQueueHead = (g_pressQueueHead + 1) % kPressQueueCapacity;
    --g_pressQueueCount;
    if (g_pressQueueCount > 0) {
        g_pressQueue[g_pressQueueHead].servableTick = now;
    }
    return true;
}

void ServeQueuedOriginal(const wchar_t* reason) {
    if (DropStaleQueuedPress()) {
        return;
    }
    if (g_pressQueueCount == 0) {
        return;
    }
    const QueuedPress press = g_pressQueue[g_pressQueueHead];
    g_pressQueueHead = (g_pressQueueHead + 1) % kPressQueueCapacity;
    --g_pressQueueCount;
    if (g_pressQueueCount > 0) {
        g_pressQueue[g_pressQueueHead].servableTick = GetTickCount64();
    }
    const uint64_t ageStart =
        press.servableTick != 0 ? press.servableTick : press.admittedTick;
    TraceLog(L"own press fallback trigger=%s entry=%s reason=%s wait=%llu q=%d",
            PressTriggerName(press.trigger), PressEntryName(press.entry), reason,
            static_cast<unsigned long long>(GetTickCount64() - ageStart),
            g_pressQueueCount);
    RunPressOriginal(press, reason);
}

bool AgeOutQueuedPress() {
    bool dropped = false;
    while (DropStaleQueuedPress()) {
        dropped = true;
    }
    if (g_pressQueueCount == 0) {
        return dropped;
    }
    const QueuedPress& head = g_pressQueue[g_pressQueueHead];
    if (head.servableTick == 0) {
        return dropped;
    }
    const uint64_t now = GetTickCount64();
    if (now <= head.servableTick ||
        now - head.servableTick <= kPressQueueWaitMs) {
        return dropped;
    }
    ServeQueuedOriginal(L"queue-wait");
    return true;
}

void ResumePressQueueAge() {
    if (g_pressQueueCount > 0) {
        g_pressQueue[g_pressQueueHead].servableTick = GetTickCount64();
    }
}

LONG64 PublishPressIntent(int32_t kind, const QueuedPress& press,
                          const wchar_t* source) {
    HWND window = g_execWindow.load(std::memory_order_relaxed);
    if (!window) {
        TraceLog(L"own press publish refused trigger=%s reason=no-window",
                PressTriggerName(press.trigger));
        return 0;
    }
    if (SetTimer(window, kPressAckTimer,
                 static_cast<UINT>(kPressAckTimeoutMs), nullptr) == 0) {
        TraceLog(L"own press publish refused trigger=%s reason=no-timer err=%u",
                PressTriggerName(press.trigger), GetLastError());
        return 0;
    }
    const uint32_t method =
        kind == kSwipeIntentClose ? press.dismissMethod : press.showMethod;

    const uint32_t publishedEpoch = PeerEpochNow();
    const LONG64 sequence =
        PublishBridgeIntent(kind, press.trigger, method, press.flags);
    if (sequence <= 0) {
        KillTimer(window, kPressAckTimer);
        TraceLog(L"own press publish refused trigger=%s reason=no-delivery",
                PressTriggerName(press.trigger));
        return 0;
    }
    g_pressOutstanding = OutstandingPress{};
    g_pressOutstanding.pending = true;
    g_pressOutstanding.seq = sequence;
    g_pressOutstanding.publishedTick = GetTickCount64();
    g_pressOutstanding.press = press;
    g_pressOutstanding.publishedEpoch = publishedEpoch;

    if (g_pressQueueCount > 0) {
        g_pressQueue[g_pressQueueHead].servableTick = 0;
    }
    TraceLog(L"own press published trigger=%s entry=%s seq=%lld kind=%s q=%d "
            L"source=%s",
            PressTriggerName(press.trigger), PressEntryName(press.entry),
            static_cast<long long>(sequence),
            kind == kSwipeIntentClose ? L"close" : L"open",
            g_pressQueueCount, source);
    return sequence;
}

bool ServePress(const QueuedPress& press, const OwnSnapshot* admission) {
    const uint64_t now = GetTickCount64();
    if (now > press.admittedTick &&
        now - press.admittedTick > kPressQueueStaleMs) {

        TraceLog(L"own press dropped trigger=%s entry=%s reason=stale wait=%llu",
                PressTriggerName(press.trigger), PressEntryName(press.entry),
                static_cast<unsigned long long>(now - press.admittedTick));
        return true;
    }

    const uint64_t ageStart =
        press.servableTick != 0 ? press.servableTick : press.admittedTick;
    if (now > ageStart && now - ageStart > kPressQueueWaitMs) {
        TraceLog(L"own press fallback trigger=%s entry=%s reason=queue-wait "
                L"wait=%llu",
                PressTriggerName(press.trigger), PressEntryName(press.entry),
                static_cast<unsigned long long>(now - ageStart));
        RunPressOriginal(press, L"queue-wait");
        return true;
    }
    if (!PeerAliveNow() || PeerEpochNow() != press.admittingEpoch ||
        !ExecutorCapabilityNow()) {
        TraceLog(L"own press fallback trigger=%s entry=%s reason=peer-gone "
                L"wait=%llu",
                PressTriggerName(press.trigger), PressEntryName(press.entry),
                static_cast<unsigned long long>(now - press.admittedTick));
        RunPressOriginal(press, L"peer-gone");
        return true;
    }
    uint64_t state = 0;
    void* launcher = LauncherNow(&state);
    if (!launcher) {
        TraceLog(L"own press fallback trigger=%s entry=%s reason=not-ready "
                L"state=0x%llX",
                PressTriggerName(press.trigger), PressEntryName(press.entry),
                static_cast<unsigned long long>(state));
        RunPressOriginal(press, L"not-ready");
        return true;
    }
    const OwnSnapshot own = admission ? *admission : OwnSync(launcher);
    if (!own.ready) {
        TraceLog(L"own press fallback trigger=%s entry=%s "
                L"reason=state-not-ready",
                PressTriggerName(press.trigger), PressEntryName(press.entry));
        RunPressOriginal(press, L"state-not-ready");
        return true;
    }
    if (g_pressLatch.pending) {

        LatchPress(own, press);
        return true;
    }

    const int32_t direction = SwipeIpcControllerDirection(&g_ipc);
    int32_t kind = kSwipeIntentOpen;
    if (press.entry == kPressEntryHide &&
        press.trigger != kSwipeTriggerClickAway) {

        kind = kSwipeIntentClose;
    } else if (press.trigger == kSwipeTriggerClickAway) {

        if (direction == kSwipeDirectionClosing && press.pointer) {
            kind = kSwipeIntentOpen;
        } else if (own.open || own.state == 1 || own.state == 3 ||
                   own.state == 5) {
            kind = kSwipeIntentClose;
        } else {

            TraceLog(L"own press pass trigger=clickaway entry=hide "
                    L"reason=no-panel state=%u dir=%d",
                    own.state, direction);
            RunPressOriginal(press, L"no-panel");
            return true;
        }
    } else {
        kind = IntentShouldClose(own, direction) ? kSwipeIntentClose
                                                 : kSwipeIntentOpen;
    }

    const uint32_t method =
        kind == kSwipeIntentClose ? press.dismissMethod : press.showMethod;
    const bool supported = kind == kSwipeIntentClose
                               ? SwipeCloseIntentAllowed(method)
                               : SwipeShowAllowed(method, press.flags);
    if (!supported) {
        TraceLog(L"own pass trigger=%s entry=%s state=%u open=%d kind=%s "
                L"method=%u flags=%u reason=unsupported",
                PressTriggerName(press.trigger), PressEntryName(press.entry),
                own.state, own.open ? 1 : 0,
                kind == kSwipeIntentClose ? L"close" : L"open", method,
                press.flags);
        RunPressOriginal(press, L"unsupported");
        return true;
    }
    if (kind == kSwipeIntentClose && OpenGenerationGateHolds()) {

        g_pressOpenGenHeld = true;
        RequeuePressFront(press);
        TraceLog(L"own press held trigger=%s entry=%s reason=open-gen before=%llu "
                L"q=%d",
                PressTriggerName(press.trigger), PressEntryName(press.entry),
                static_cast<unsigned long long>(g_pressOpenGenBefore),
                g_pressQueueCount);
        return false;
    }

    const PeerProcess process = WarmPeerProcess();
    if (process != PeerProcess::Alive) {
        const wchar_t* processReason =
            process == PeerProcess::Dead ? L"process-dead" : L"process-unknown";
        TraceLog(L"own press fallback trigger=%s entry=%s reason=%s wait=%llu",
                PressTriggerName(press.trigger), PressEntryName(press.entry),
                processReason,
                static_cast<unsigned long long>(now - press.admittedTick));
        RunPressOriginal(press, processReason);
        return true;
    }

    TraceLog(L"own toggle decide trigger=%s entry=%s state=%u open=%d kind=%s "
            L"dir=%d q=%d ptr=%d",
            PressTriggerName(press.trigger), PressEntryName(press.entry),
            own.state, own.open ? 1 : 0,
            kind == kSwipeIntentClose ? L"close" : L"open", direction,
            g_pressQueueCount, press.pointer ? 1 : 0);
    if (kind == kSwipeIntentOpen &&
        (ShellHideInFlight(own) || t_execCall > 0)) {

        LatchPress(own, press);
        return true;
    }
    if (kind == kSwipeIntentOpen && direction != kSwipeDirectionClosing) {

        const uint64_t openBefore = CurrentOpenGeneration();
        if (PublishPressIntent(kind, press, L"press") <= 0) {
            RunPressOriginal(press, L"publish");
        } else {
            BeginPressOpenGeneration(openBefore);
        }
        return true;
    }
    if (PublishPressIntent(kind, press, L"press") <= 0) {
        RunPressOriginal(press, L"publish");
    }
    return true;
}

void ServeNextPress(const wchar_t* reason) {
    if (g_pressOpenGenHeld) {

        if (g_pressQueueCount == 0) {
            ClearPressOpenGeneration();
        } else if (OpenGenerationGateHolds()) {
            return;
        }
    }
    while (!g_pressOutstanding.pending && g_pressQueueCount > 0) {
        if (DropStaleQueuedPress()) {
            continue;
        }
        const QueuedPress press = g_pressQueue[g_pressQueueHead];
        g_pressQueueHead = (g_pressQueueHead + 1) % kPressQueueCapacity;
        --g_pressQueueCount;
        if (g_pressQueueCount > 0) {

            g_pressQueue[g_pressQueueHead].servableTick = GetTickCount64();
        }
        TraceLog(L"own press dequeued trigger=%s entry=%s q=%d reason=%s "
                L"wait=%llu",
                PressTriggerName(press.trigger), PressEntryName(press.entry),
                g_pressQueueCount, reason,
                static_cast<unsigned long long>(GetTickCount64() -
                                                press.admittedTick));
        if (!ServePress(press, nullptr)) {

            return;
        }
    }
}

void HoldOutstandingAtAdmission(const OutstandingPress& press,
                                const wchar_t* reason, uint32_t epochNow,
                                uint64_t wait, bool claimed) {
    g_pressOutstanding.consumedAwaitingAck = true;
    if (press.external) {
        TraceLog(L"own external barrier seq=%lld wait=%llu epoch=%u->%u "
                L"claimed=%d decision=reserved reason=%s",
                static_cast<long long>(press.seq),
                static_cast<unsigned long long>(wait), press.publishedEpoch,
                epochNow, claimed ? 1 : 0, reason);
    } else {
        TraceLog(L"own press abandoned seq=%lld trigger=%s entry=%s "
                L"wait=%llu epoch=%u->%u decision=consumed awaiting=ack "
                L"reason=%s",
                static_cast<long long>(press.seq),
                PressTriggerName(press.press.trigger),
                PressEntryName(press.press.entry),
                static_cast<unsigned long long>(wait), press.publishedEpoch,
                epochNow, reason);
    }
    AgeOutQueuedPress();
    if (g_pressQueueCount > 0 && !ArmConsumedPressPoll()) {

        TraceLog(L"own press poll timer failed err=%u seq=%lld q=%d",
                GetLastError(), static_cast<long long>(press.seq),
                g_pressQueueCount);
    }
}

bool ReconcileOutstandingAtAdmission(const wchar_t* reason, uint32_t epochNow) {
    const OutstandingPress press = g_pressOutstanding;
    const uint64_t wait = GetTickCount64() - press.publishedTick;
    const bool epochChanged =
        press.publishedEpoch != 0 && epochNow != press.publishedEpoch;
    const LONG64 taken = SwipeIpcTakenIntent(&g_ipc);
    if (taken == press.seq && PeerProcessProbe() == PeerProcess::Dead &&
        !epochChanged &&
        PeerStateAliveNow()) {

        g_pressOutstanding = OutstandingPress{};
        HWND window = g_execWindow.load(std::memory_order_relaxed);
        if (window) {
            KillTimer(window, kPressAckTimer);
        }
        if (press.external) {
            TraceLog(L"own external barrier seq=%lld wait=%llu epoch=%u->%u "
                    L"decision=released reason=peer-dead",
                    static_cast<long long>(press.seq),
                    static_cast<unsigned long long>(wait), press.publishedEpoch,
                    epochNow);
        } else {
            TraceLog(L"own press abandoned seq=%lld trigger=%s entry=%s "
                    L"wait=%llu epoch=%u->%u decision=consumed "
                    L"reason=peer-dead",
                    static_cast<long long>(press.seq),
                    PressTriggerName(press.press.trigger),
                    PressEntryName(press.press.entry),
                    static_cast<unsigned long long>(wait), press.publishedEpoch,
                    epochNow);
        }
        ResumePressQueueAge();
        ServeNextPress(L"admission");
        return true;
    }
    if (taken == press.seq && !epochChanged && PeerStateAliveNow()) {
        if (wait >= kClaimAbandonMs) {

            g_pressOutstanding = OutstandingPress{};
            HWND window = g_execWindow.load(std::memory_order_relaxed);
            if (window) {
                KillTimer(window, kPressAckTimer);
            }
            if (press.external) {
                TraceLog(L"own external barrier seq=%lld wait=%llu epoch=%u->%u "
                        L"decision=released reason=claim-deadline bound=%llu",
                        static_cast<long long>(press.seq),
                        static_cast<unsigned long long>(wait), press.publishedEpoch,
                        epochNow,
                        static_cast<unsigned long long>(kClaimAbandonMs));
            } else {
                TraceLog(L"own press abandoned seq=%lld trigger=%s entry=%s "
                        L"wait=%llu epoch=%u->%u decision=consumed "
                        L"reason=claim-deadline bound=%llu",
                        static_cast<long long>(press.seq),
                        PressTriggerName(press.press.trigger),
                        PressEntryName(press.press.entry),
                        static_cast<unsigned long long>(wait), press.publishedEpoch,
                        epochNow,
                        static_cast<unsigned long long>(kClaimAbandonMs));
            }
            ResumePressQueueAge();
            ServeNextPress(L"claim-deadline");
            return true;
        }

        HoldOutstandingAtAdmission(press, L"claimed-at-admission", epochNow,
                                   wait, true);
        return false;
    }
    bool replay = false;
    if (!press.external) {
        const bool revoked = SwipeIpcAbortIntent(&g_ipc, press.seq);
        if (revoked) {
            g_pressOutstanding.aborted = true;
        }
        const LONG64 takenAfter = SwipeIpcTakenIntent(&g_ipc);
        if (takenAfter == press.seq && !epochChanged && PeerStateAliveNow()) {

            HoldOutstandingAtAdmission(press, L"claim-after-abort", epochNow,
                                       wait, true);
            return false;
        }
        replay = revoked && takenAfter != press.seq;
    } else if (!epochChanged && PeerStateAliveNow()) {

        const bool revoked = SwipeIpcAbortIntent(&g_ipc, press.seq);
        if (revoked) {
            g_pressOutstanding.aborted = true;
        }
        const LONG64 takenAfter = SwipeIpcTakenIntent(&g_ipc);
        if (takenAfter == press.seq) {
            HoldOutstandingAtAdmission(press, L"claim-after-abort", epochNow,
                                       wait, true);
            return false;
        }
        if (!revoked) {
            HoldOutstandingAtAdmission(press, L"record-moved", epochNow, wait,
                                       false);
            return false;
        }
    }
    g_pressOutstanding = OutstandingPress{};
    HWND window = g_execWindow.load(std::memory_order_relaxed);
    if (window) {
        KillTimer(window, kPressAckTimer);
    }
    const bool lateReplay = replay && ReplayPastBound(press.press);
    if (press.external) {
        TraceLog(L"own external barrier seq=%lld wait=%llu epoch=%u->%u "
                L"decision=released reason=%s",
                static_cast<long long>(press.seq),
                static_cast<unsigned long long>(wait), press.publishedEpoch,
                epochNow, reason);
    } else {
        TraceLog(L"own press abandoned seq=%lld trigger=%s entry=%s wait=%llu "
                L"epoch=%u->%u decision=%s reason=%s",
                static_cast<long long>(press.seq),
                PressTriggerName(press.press.trigger),
                PressEntryName(press.press.entry),
                static_cast<unsigned long long>(wait), press.publishedEpoch,
                epochNow,
                replay ? (lateReplay ? L"dropped" : L"replay") : L"consumed",
                lateReplay ? L"stale" : reason);
    }
    if (replay && !lateReplay) {
        RunPressOriginal(press.press, L"abandoned");
    }
    ResumePressQueueAge();
    ServeNextPress(L"abandoned");
    return true;
}

void AdmitPress(const QueuedPress& press, const OwnSnapshot& own) {
    bool reconciled = false;
    if (g_pressOutstanding.pending) {

        const uint32_t epochNow = PeerEpochNow();
        const bool epochChanged =
            g_pressOutstanding.publishedEpoch != 0 &&
            epochNow != g_pressOutstanding.publishedEpoch;
        const bool peerGone = !PeerStateAliveNow();
        const bool claimed =
            SwipeIpcTakenIntent(&g_ipc) == g_pressOutstanding.seq;
        const bool staleUnclaimed = !claimed && !PeerAliveNow();

        const bool deadClaimed =
            claimed && PeerProcessProbe() == PeerProcess::Dead;

        const bool claimStale =
            GetTickCount64() - g_pressOutstanding.publishedTick >=
                kClaimAbandonMs &&
            (claimed || g_pressOutstanding.external);
        if (epochChanged || peerGone || staleUnclaimed || deadClaimed ||
            claimStale) {
            reconciled = ReconcileOutstandingAtAdmission(
                epochChanged
                    ? L"epoch-changed"
                    : (peerGone ? L"peer-gone"
                                : (deadClaimed ? L"peer-dead"
                                               : (claimStale ? L"claim-deadline"
                                                             : L"stale-unclaimed"))),
                epochNow);
        }
    }

    if (g_pressOutstanding.pending || g_pressQueueCount > 0) {
        QueuePress(press, L"press");
        return;
    }
    OwnSnapshot current = own;
    if (reconciled) {

        uint64_t state = 0;
        void* launcher = LauncherNow(&state);
        if (!launcher) {
            TraceLog(L"own press pass trigger=%s entry=%s "
                    L"reason=state-after-replay state=0x%llX",
                    PressTriggerName(press.trigger),
                    PressEntryName(press.entry),
                    static_cast<unsigned long long>(state));
            RunPressOriginal(press, L"state-after-replay");
            return;
        }
        current = OwnSync(launcher);
        if (!current.ready) {
            TraceLog(L"own press pass trigger=%s entry=%s "
                    L"reason=state-after-replay state=%u",
                    PressTriggerName(press.trigger),
                    PressEntryName(press.entry), current.state);
            RunPressOriginal(press, L"state-after-replay");
            return;
        }
    }

    ServePress(press, &current);
}

bool AckProvesNothingRan(const OutstandingPress& press, int32_t state) {
    if (press.external) {
        return false;
    }
    return state == kSwipeAckRefused ||
           (press.aborted && state == kSwipeAckSuperseded);
}

void HandlePressAck() {
    SwipeIpcIntentAck ack{};
    if (!SwipeIpcReadIntentAck(&g_ipc, &ack)) {
        return;
    }
    if (!g_pressOutstanding.pending || ack.intent != g_pressOutstanding.seq) {

        return;
    }
    const OutstandingPress press = g_pressOutstanding;

    const bool replayed = AckProvesNothingRan(press, ack.state);
    g_pressOutstanding = OutstandingPress{};
    HWND window = g_execWindow.load(std::memory_order_relaxed);
    if (window) {
        KillTimer(window, kPressAckTimer);
    }
    if (press.external) {
        TraceLog(L"own external ack seq=%lld state=%d wait=%llu "
                L"decision=reconciled",
                static_cast<long long>(ack.intent), ack.state,
                static_cast<unsigned long long>(GetTickCount64() -
                                                press.publishedTick));
    } else {
        TraceLog(L"own press ack seq=%lld state=%d trigger=%s entry=%s "
                L"wait=%llu",
                static_cast<long long>(ack.intent), ack.state,
                PressTriggerName(press.press.trigger),
                PressEntryName(press.press.entry),
                static_cast<unsigned long long>(GetTickCount64() -
                                                press.publishedTick));
    }
    if (replayed) {

        if (ReplayPastBound(press.press)) {
            DropPastBoundReplay(press.press);
        } else {
            RunPressOriginal(press.press,
                             ack.state == kSwipeAckSuperseded
                                 ? L"revoked-before-copy"
                                 : L"refused");
        }
    }
    ResumePressQueueAge();
    ServeNextPress(L"ack");
}

bool ReserveExternalBarrier(const OutstandingPress& press, const wchar_t* peer,
                            uint32_t peerAge, uint32_t epochNow, uint64_t wait,
                            bool claimed, const wchar_t* reason) {
    g_pressOutstanding.consumedAwaitingAck = true;
    TraceLog(L"own external ack timeout seq=%lld wait=%llu peer=%s age=%u "
            L"epoch=%u claimed=%d decision=reserved reason=%s",
            static_cast<long long>(press.seq),
            static_cast<unsigned long long>(wait), peer, peerAge, epochNow,
            claimed ? 1 : 0, reason);
    AgeOutQueuedPress();
    HWND window = g_execWindow.load(std::memory_order_relaxed);
    if (!window ||
        SetTimer(window, kPressAckTimer,
                 static_cast<UINT>(kPressReservePollMs), nullptr) == 0) {
        TraceLog(L"own external reserve timer failed err=%u seq=%lld",
                GetLastError(), static_cast<long long>(press.seq));
        while (g_pressQueueCount > 0) {
            ServeQueuedOriginal(L"timer-failed");
        }
        return false;
    }
    return true;
}

void HandlePressAckTimeout() {
    if (!g_pressOutstanding.pending) {

        HWND window = g_execWindow.load(std::memory_order_relaxed);
        if (window) {
            KillTimer(window, kPressAckTimer);
        }
        return;
    }
    const OutstandingPress press = g_pressOutstanding;
    SwipeIpcIntentAck ack{};
    if (SwipeIpcReadIntentAck(&g_ipc, &ack) && ack.intent == press.seq) {

        const bool replayed = AckProvesNothingRan(press, ack.state);
        g_pressOutstanding = OutstandingPress{};
        HWND window = g_execWindow.load(std::memory_order_relaxed);
        if (window) {
            KillTimer(window, kPressAckTimer);
        }
        if (press.external) {
            TraceLog(L"own external ack late seq=%lld state=%d wait=%llu "
                    L"decision=reconciled",
                    static_cast<long long>(ack.intent), ack.state,
                    static_cast<unsigned long long>(GetTickCount64() -
                                                    press.publishedTick));
        } else {
            TraceLog(L"own press ack late seq=%lld state=%d trigger=%s "
                    L"entry=%s wait=%llu",
                    static_cast<long long>(ack.intent), ack.state,
                    PressTriggerName(press.press.trigger),
                    PressEntryName(press.press.entry),
                    static_cast<unsigned long long>(GetTickCount64() -
                                                    press.publishedTick));
        }
        if (replayed) {
            if (ReplayPastBound(press.press)) {
                DropPastBoundReplay(press.press);
            } else {

                RunPressOriginal(press.press,
                                 ack.state == kSwipeAckSuperseded
                                     ? L"revoked-before-copy"
                                     : L"refused");
            }
        }
        ResumePressQueueAge();
        ServeNextPress(L"late-ack");
        return;
    }

    const bool haveChannel = g_ipc.state != nullptr;
    const bool peerAlive = PeerAliveNow();
    const uint32_t peerAge = PeerTickAgeMs();
    const wchar_t* peer =
        !haveChannel ? L"absent" : (peerAlive ? L"alive" : L"stale");
    const LONG64 taken = SwipeIpcTakenIntent(&g_ipc);
    const uint32_t epochNow = PeerEpochNow();
    const bool epochChanged =
        press.publishedEpoch != 0 && epochNow != press.publishedEpoch;
    const uint64_t wait = GetTickCount64() - press.publishedTick;
    if (press.external) {

        if (epochChanged) {
            g_pressOutstanding = OutstandingPress{};
            HWND window = g_execWindow.load(std::memory_order_relaxed);
            if (window) {
                KillTimer(window, kPressAckTimer);
            }
            TraceLog(L"own external barrier seq=%lld wait=%llu peer=%s age=%u "
                    L"epoch=%u->%u decision=released reason=epoch-changed",
                    static_cast<long long>(press.seq),
                    static_cast<unsigned long long>(wait), peer, peerAge,
                    press.publishedEpoch, epochNow);
            ResumePressQueueAge();
            ServeNextPress(L"epoch");
            return;
        }
        const bool claimed = taken == press.seq;

        const bool deadClaimed =
            claimed && PeerProcessProbe() == PeerProcess::Dead;
        if (!deadClaimed && PeerStateAliveNow() && wait >= kClaimAbandonMs) {

            // The deadline is the same whether or not the controller claimed
            // the intent: an event that never woke it must not pin the queue.
            g_pressOutstanding = OutstandingPress{};
            HWND window = g_execWindow.load(std::memory_order_relaxed);
            if (window) {
                KillTimer(window, kPressAckTimer);
            }
            TraceLog(L"own external barrier seq=%lld wait=%llu peer=%s age=%u "
                    L"epoch=%u claimed=%d decision=released "
                    L"reason=claim-deadline bound=%llu",
                    static_cast<long long>(press.seq),
                    static_cast<unsigned long long>(wait), peer, peerAge,
                    epochNow, claimed ? 1 : 0,
                    static_cast<unsigned long long>(kClaimAbandonMs));
            ResumePressQueueAge();
            ServeNextPress(L"claim-deadline");
            return;
        }
        if (!deadClaimed &&
            (peerAlive || (claimed && PeerStateAliveNow()))) {
            ReserveExternalBarrier(press, peer, peerAge, epochNow, wait, claimed,
                                   peerAlive ? L"peer-alive" : L"claimed");
            return;
        }
        const wchar_t* releaseReason = nullptr;
        bool claimedNow = claimed;
        if (deadClaimed) {
            releaseReason = L"peer-dead";
        } else if (!PeerStateAliveNow()) {
            releaseReason = L"peer-gone";
        } else {
            const bool revoked = SwipeIpcAbortIntent(&g_ipc, press.seq);
            if (revoked) {
                g_pressOutstanding.aborted = true;
            }
            const LONG64 takenAfter = SwipeIpcTakenIntent(&g_ipc);
            claimedNow = takenAfter == press.seq;
            if (!PeerStateAliveNow()) {
                releaseReason = L"peer-gone";
            } else if (claimedNow) {
                ReserveExternalBarrier(press, peer, peerAge, epochNow, wait,
                                       true, L"claim-after-abort");
                return;
            } else if (!revoked) {
                ReserveExternalBarrier(press, peer, peerAge, epochNow, wait,
                                       false, L"record-moved");
                return;
            } else {
                releaseReason = L"stale-unclaimed";
            }
        }
        g_pressOutstanding = OutstandingPress{};
        HWND window = g_execWindow.load(std::memory_order_relaxed);
        if (window) {
            KillTimer(window, kPressAckTimer);
        }
        TraceLog(L"own external barrier seq=%lld wait=%llu peer=%s age=%u "
                L"epoch=%u claimed=%d decision=released reason=%s",
                static_cast<long long>(press.seq),
                static_cast<unsigned long long>(wait), peer, peerAge,
                epochNow, claimedNow ? 1 : 0, releaseReason);
        ResumePressQueueAge();
        ServeNextPress(L"timeout");
        return;
    }
    if (taken == press.seq) {
        if (epochChanged) {

            g_pressOutstanding = OutstandingPress{};
            HWND window = g_execWindow.load(std::memory_order_relaxed);
            if (window) {
                KillTimer(window, kPressAckTimer);
            }
            TraceLog(L"own press ack timeout seq=%lld trigger=%s entry=%s "
                    L"wait=%llu taken=%lld peer=%s age=%u epoch=%u->%u "
                    L"decision=consumed reason=epoch-changed",
                    static_cast<long long>(press.seq),
                    PressTriggerName(press.press.trigger),
                    PressEntryName(press.press.entry),
                    static_cast<unsigned long long>(wait),
                    static_cast<long long>(taken), peer, peerAge,
                    press.publishedEpoch, epochNow);
            ResumePressQueueAge();
            ServeNextPress(L"epoch");
            return;
        }
        if (PeerProcessProbe() == PeerProcess::Dead) {

            g_pressOutstanding = OutstandingPress{};
            HWND window = g_execWindow.load(std::memory_order_relaxed);
            if (window) {
                KillTimer(window, kPressAckTimer);
            }
            TraceLog(L"own press ack timeout seq=%lld trigger=%s entry=%s "
                    L"wait=%llu taken=%lld peer=%s age=%u epoch=%u "
                    L"decision=consumed reason=peer-dead",
                    static_cast<long long>(press.seq),
                    PressTriggerName(press.press.trigger),
                    PressEntryName(press.press.entry),
                    static_cast<unsigned long long>(wait),
                    static_cast<long long>(taken), peer, peerAge, epochNow);
            ResumePressQueueAge();
            ServeNextPress(L"timeout");
            return;
        }
        if (!g_pressOutstanding.consumedAwaitingAck) {

            g_pressOutstanding.consumedAwaitingAck = true;
            TraceLog(L"own press ack timeout seq=%lld trigger=%s entry=%s "
                    L"wait=%llu taken=%lld peer=%s age=%u epoch=%u "
                    L"decision=consumed awaiting=ack",
                    static_cast<long long>(press.seq),
                    PressTriggerName(press.press.trigger),
                    PressEntryName(press.press.entry),
                    static_cast<unsigned long long>(wait),
                    static_cast<long long>(taken), peer, peerAge, epochNow);
        }
        if (!PeerStateAliveNow()) {

            g_pressOutstanding = OutstandingPress{};
            HWND window = g_execWindow.load(std::memory_order_relaxed);
            if (window) {
                KillTimer(window, kPressAckTimer);
            }
            TraceLog(L"own press ack timeout seq=%lld trigger=%s entry=%s "
                    L"wait=%llu taken=%lld peer=%s age=%u epoch=%u "
                    L"decision=consumed reason=peer-gone",
                    static_cast<long long>(press.seq),
                    PressTriggerName(press.press.trigger),
                    PressEntryName(press.press.entry),
                    static_cast<unsigned long long>(wait),
                    static_cast<long long>(taken), peer, peerAge, epochNow);
            ResumePressQueueAge();
            ServeNextPress(L"timeout");
            return;
        }
        if (wait >= kClaimAbandonMs) {

            g_pressOutstanding = OutstandingPress{};
            HWND window = g_execWindow.load(std::memory_order_relaxed);
            if (window) {
                KillTimer(window, kPressAckTimer);
            }
            TraceLog(L"own press ack timeout seq=%lld trigger=%s entry=%s "
                    L"wait=%llu taken=%lld peer=%s age=%u epoch=%u "
                    L"decision=consumed reason=claim-deadline bound=%llu",
                    static_cast<long long>(press.seq),
                    PressTriggerName(press.press.trigger),
                    PressEntryName(press.press.entry),
                    static_cast<unsigned long long>(wait),
                    static_cast<long long>(taken), peer, peerAge, epochNow,
                    static_cast<unsigned long long>(kClaimAbandonMs));
            ResumePressQueueAge();
            ServeNextPress(L"claim-deadline");
            return;
        }
        AgeOutQueuedPress();
        if (g_pressQueueCount > 0 && !ArmConsumedPressPoll()) {

            TraceLog(L"own press poll timer failed err=%u seq=%lld q=%d",
                    GetLastError(), static_cast<long long>(press.seq),
                    g_pressQueueCount);
        }
        return;
    }

    const bool revoked = SwipeIpcAbortIntent(&g_ipc, press.seq);
    if (revoked) {
        g_pressOutstanding.aborted = true;
    }
    const LONG64 takenAfter = SwipeIpcTakenIntent(&g_ipc);
    if (!revoked) {
        g_pressOutstanding = OutstandingPress{};
        HWND window = g_execWindow.load(std::memory_order_relaxed);
        if (window) {
            KillTimer(window, kPressAckTimer);
        }
        TraceLog(L"own press ack timeout seq=%lld trigger=%s entry=%s wait=%llu "
                L"taken=%lld peer=%s age=%u epoch=%u->%u "
                L"decision=superseded reason=record-moved",
                static_cast<long long>(press.seq),
                PressTriggerName(press.press.trigger),
                PressEntryName(press.press.entry),
                static_cast<unsigned long long>(wait),
                static_cast<long long>(taken), peer, peerAge,
                press.publishedEpoch, epochNow);
        ResumePressQueueAge();
        ServeNextPress(L"timeout");
        return;
    }
    if (takenAfter == press.seq) {

        if (!g_pressOutstanding.consumedAwaitingAck) {
            g_pressOutstanding.consumedAwaitingAck = true;
            TraceLog(L"own press ack timeout seq=%lld trigger=%s entry=%s "
                    L"wait=%llu taken=%lld peer=%s age=%u epoch=%u "
                    L"decision=consumed awaiting=ack reason=claim-after-abort",
                    static_cast<long long>(press.seq),
                    PressTriggerName(press.press.trigger),
                    PressEntryName(press.press.entry),
                    static_cast<unsigned long long>(wait),
                    static_cast<long long>(takenAfter), peer, peerAge, epochNow);
        }
        if (g_pressQueueCount > 0 && !ArmConsumedPressPoll()) {
            TraceLog(L"own press poll timer failed err=%u seq=%lld q=%d",
                    GetLastError(), static_cast<long long>(press.seq),
                    g_pressQueueCount);
        }
        return;
    }
    g_pressOutstanding = OutstandingPress{};
    HWND window = g_execWindow.load(std::memory_order_relaxed);
    if (window) {
        KillTimer(window, kPressAckTimer);
    }
    const bool lateReplay = ReplayPastBound(press.press);
    TraceLog(L"own press ack timeout seq=%lld trigger=%s entry=%s wait=%llu "
            L"taken=%lld peer=%s age=%u epoch=%u->%u decision=%s reason=%s",
            static_cast<long long>(press.seq),
            PressTriggerName(press.press.trigger),
            PressEntryName(press.press.entry),
            static_cast<unsigned long long>(wait),
            static_cast<long long>(taken), peer, peerAge, press.publishedEpoch,
            epochNow, lateReplay ? L"dropped" : L"replay",
            lateReplay ? L"stale"
                       : (epochChanged ? L"epoch-changed" : L"revoked"));
    if (!lateReplay) {
        RunPressOriginal(press.press, L"no-ack");
    }
    ResumePressQueueAge();
    ServeNextPress(L"timeout");
}

void DispatchToggle(const OwnSnapshot& own, int32_t trigger, uint32_t showMethod,
                    uint32_t dismissMethod, uint32_t flags,
                    uint32_t admittingEpoch, int32_t entry, bool pointer) {
    const QueuedPress press{trigger, entry, showMethod, dismissMethod, flags,
                            admittingEpoch, GetTickCount64(), 0, pointer};
    AdmitPress(press, own);
}

using OnHotkeyInvoke_t = HRESULT (*)(void*, uint32_t);
OnHotkeyInvoke_t OnHotkeyInvoke_Original = nullptr;

HRESULT OnHotkeyInvoke_Hook(void* self, uint32_t flags) {
    uint32_t admittingEpoch = 0;
    if (!g_ownWinEnabled.load(std::memory_order_relaxed) ||
        g_disabled.load(std::memory_order_relaxed)) {
        return OnHotkeyInvoke_Original ? OnHotkeyInvoke_Original(self, flags)
                                       : S_OK;
    }

    ProveLauncherEntry(L"win");
    EnsureExecutorOnLauncherThread(L"hotkey");
    if (!BridgeCanServeOnLauncherThread(L"win", &admittingEpoch)) {

        return OnHotkeyInvoke_Original ? OnHotkeyInvoke_Original(self, flags)
                                       : S_OK;
    }

    void* launcher = static_cast<uint8_t*>(self) + kHotkeyLauncherOffset;
    const bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    const bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    const bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    TraceLog(L"own win enter flags=%u shift=%d ctrl=%d alt=%d tid=%u",
            flags, shift ? 1 : 0, control ? 1 : 0, alt ? 1 : 0,
            GetCurrentThreadId());

    if (shift || control || alt) {
        const OwnSnapshot own = OwnSync(launcher);
        if (!own.ready) {
            TraceLog(L"own win pass reason=state-not-ready flags=%u", flags);
            return OnHotkeyInvoke_Original
                       ? OnHotkeyInvoke_Original(self, flags)
                       : S_OK;
        }
        if (own.open) {
            PublishBridgeIntent(kSwipeIntentExternal, kSwipeTriggerOverride,
                                kDismissMethodWin, flags);
            TraceLog(L"own win pass reason=external open=1 state=%u", own.state);
            OwnExternalForwardGuard guard(kDismissMethodWin);
            return OnHotkeyInvoke_Original
                       ? OnHotkeyInvoke_Original(self, flags)
                       : S_OK;
        }
        TraceLog(L"own win pass reason=external open=0 state=%u (forwarded)",
                own.state);
        PublishBridgeIntent(kSwipeIntentExternal, kSwipeTriggerOverride,
                            kDismissMethodWin, flags);

        OwnExternalForwardGuard guard(kDismissMethodWin);
        return OnHotkeyInvoke_Original ? OnHotkeyInvoke_Original(self, flags)
                                       : S_OK;
    }

    if (static_cast<const uint8_t*>(self)[kHotkeyShellOpenFlagOffset] != 0) {

        TraceLog(L"own win pass reason=shell-open-flag");
        return OnHotkeyInvoke_Original ? OnHotkeyInvoke_Original(self, flags)
                                       : S_OK;
    }

    const OwnSnapshot own = OwnSync(launcher);
    if (!own.ready) {
        TraceLog(L"own win pass reason=state-not-ready flags=%u", flags);
        return OnHotkeyInvoke_Original ? OnHotkeyInvoke_Original(self, flags)
                                       : S_OK;
    }
    DispatchToggle(own, kSwipeTriggerWin, kDismissMethodWin,
                   kDismissMethodWin, flags, admittingEpoch,
                   kPressEntryHotkey);
    return S_OK;
}
using OnStartButtonPressed_t = HRESULT (*)(void*, uint32_t, uint32_t);
OnStartButtonPressed_t OnStartButtonPressed_Original = nullptr;

HRESULT OnStartButtonPressed_Hook(void* self, uint32_t showMethod,
                                  uint32_t dismissMethod) {
    uint32_t admittingEpoch = 0;
    if (!g_ownButtonEnabled.load(std::memory_order_relaxed) ||
        g_disabled.load(std::memory_order_relaxed)) {
        return OnStartButtonPressed_Original
                   ? OnStartButtonPressed_Original(self, showMethod,
                                                   dismissMethod)
                   : S_OK;
    }

    ProveLauncherEntry(L"startbutton");
    EnsureExecutorOnLauncherThread(L"startbutton");
    if (!BridgeCanServeOnLauncherThread(L"startbutton", &admittingEpoch)) {
        return OnStartButtonPressed_Original
                   ? OnStartButtonPressed_Original(self, showMethod,
                                                   dismissMethod)
                   : S_OK;
    }

    TraceLog(L"own button enter show=%u dismiss=%u", showMethod, dismissMethod);
    const OwnSnapshot own = OwnSync(self);
    if (!own.ready) {
        TraceLog(L"own button pass reason=state-not-ready show=%u dismiss=%u",
                showMethod, dismissMethod);
        return OnStartButtonPressed_Original
                   ? OnStartButtonPressed_Original(self, showMethod,
                                                   dismissMethod)
                   : S_OK;
    }
    DispatchToggle(own, kSwipeTriggerStartButton, showMethod,
                   dismissMethod, 0, admittingEpoch, kPressEntryButton);
    return S_OK;
}

using OnStartShown_t = HRESULT (*)(void*, void*, void*);
OnStartShown_t OnStartShown_Original = nullptr;

HRESULT OnStartShown_Hook(void* self, void* manager, void* inspectable) {
    const HRESULT hr = OnStartShown_Original
                           ? OnStartShown_Original(self, manager, inspectable)
                           : S_OK;
    if (!g_ownEventsEnabled.load(std::memory_order_relaxed) ||
        g_disabled.load(std::memory_order_relaxed)) {
        return hr;
    }
    OwnEventShown();
    return hr;
}

using OnStartHidden_t = HRESULT (*)(void*, void*, void*);
OnStartHidden_t OnStartHidden_Original = nullptr;

HRESULT OnStartHidden_Hook(void* self, void* manager, void* inspectable) {
    const HRESULT hr = OnStartHidden_Original
                           ? OnStartHidden_Original(self, manager, inspectable)
                           : S_OK;
    if (!g_ownEventsEnabled.load(std::memory_order_relaxed) ||
        g_disabled.load(std::memory_order_relaxed)) {
        return hr;
    }
    OwnEventHidden();

    ArmLatchedServe();
    return hr;
}

using DismissHook_t = HRESULT (*)(void*, uint32_t);
DismissHook_t Dismiss_Original = nullptr;

HRESULT Dismiss_Hook(void* launcher, uint32_t method) {
    uint32_t admittingEpoch = 0;
    if (!g_ownPrimitivesEnabled.load(std::memory_order_relaxed) ||
        g_disabled.load(std::memory_order_relaxed) || t_execCall > 0) {
        return Dismiss_Original ? Dismiss_Original(launcher, method) : E_FAIL;
    }
    if (OwnExternalForwarded(method)) {

        // The hotkey hook published this press's external intent and forwarded
        // it; the shell's own Dismiss is that forward, not a second takeover.
        TraceLog(L"exec pass dismiss method=%u reason=forwarded", method);
        const HRESULT hr =
            Dismiss_Original ? Dismiss_Original(launcher, method) : E_FAIL;
        OwnResyncFromState(OwnStateOf(launcher));
        return hr;
    }
    if (!BridgeCanServeOnLauncherThread(L"dismiss", &admittingEpoch)) {
        return Dismiss_Original ? Dismiss_Original(launcher, method) : E_FAIL;
    }
    const OwnSnapshot own = OwnSync(launcher);
    if (!own.ready) {
        TraceLog(L"exec pass dismiss method=%u reason=state-not-ready", method);
        return Dismiss_Original ? Dismiss_Original(launcher, method) : E_FAIL;
    }

    if (method == kDismissMethodStartButton) {
        DispatchToggle(own, kSwipeTriggerStartButton, 11, method, 0,
                       admittingEpoch, kPressEntryDismiss);
        return S_OK;
    }
    if (SwipeIpcControllerDirection(&g_ipc) != kSwipeDirectionUnowned) {
        if (method == kDismissMethodWinTab) {

            TraceLog(L"exec pass dismiss method=%u caller=wintab state=%u "
                    L"reason=external",
                    method, own.state);
            PublishBridgeIntent(kSwipeIntentExternal, kSwipeTriggerOverride,
                                method, 0);
            OwnExternalForwardGuard guard(method);
            const HRESULT hr =
                Dismiss_Original ? Dismiss_Original(launcher, method) : E_FAIL;
            OwnResyncFromState(OwnStateOf(launcher));
            return hr;
        }
        TraceLog(L"exec pass dismiss method=%u state=%u reason=external",
                method, own.state);
        PublishBridgeIntent(kSwipeIntentExternal, kSwipeTriggerOverride, method,
                            0);
        OwnExternalForwardGuard guard(method);
        const HRESULT hr =
            Dismiss_Original ? Dismiss_Original(launcher, method) : E_FAIL;
        OwnResyncFromState(OwnStateOf(launcher));
        return hr;
    }
    const HRESULT hr =
        Dismiss_Original ? Dismiss_Original(launcher, method) : E_FAIL;
    OwnResyncFromState(OwnStateOf(launcher));
    return hr;
}

using ShowStartViewHook_t = HRESULT (*)(void*, uint32_t, uint32_t);
ShowStartViewHook_t ShowStartView_Original = nullptr;

HRESULT ShowStartView_Hook(void* launcher, uint32_t method, uint32_t flags) {
    uint32_t admittingEpoch = 0;
    if (!g_ownPrimitivesEnabled.load(std::memory_order_relaxed) ||
        g_disabled.load(std::memory_order_relaxed) || t_execCall > 0) {
        return ShowStartView_Original
                   ? ShowStartView_Original(launcher, method, flags)
                   : E_FAIL;
    }
    if (!BridgeCanServeOnLauncherThread(L"showstartview", &admittingEpoch)) {
        return ShowStartView_Original
                   ? ShowStartView_Original(launcher, method, flags)
                   : E_FAIL;
    }
    const OwnSnapshot own = OwnSync(launcher);
    if (!own.ready) {
        TraceLog(L"exec pass show method=%u flags=%u reason=state-not-ready",
                method, flags);
        return ShowStartView_Original
                   ? ShowStartView_Original(launcher, method, flags)
                   : E_FAIL;
    }

    if (method == 11) {
        DispatchToggle(own, kSwipeTriggerStartButton, method, 6, flags,
                       admittingEpoch, kPressEntryShow);
        return S_OK;
    }
    if (SwipeIpcControllerDirection(&g_ipc) != kSwipeDirectionUnowned) {
        TraceLog(L"exec pass show method=%u flags=%u state=%u reason=external",
                method, flags, own.state);
        PublishBridgeIntent(kSwipeIntentExternal, kSwipeTriggerOverride, method,
                            flags);

        CancelArmedDismissForNativeShow();
        OwnExternalForwardGuard guard(kOwnExternalForwardAny);
        const HRESULT hr = ShowStartView_Original
                               ? ShowStartView_Original(launcher, method, flags)
                               : E_FAIL;
        OwnResyncFromState(OwnStateOf(launcher));
        return hr;
    }
    const HRESULT hr = ShowStartView_Original
                           ? ShowStartView_Original(launcher, method, flags)
                           : E_FAIL;
    OwnResyncFromState(OwnStateOf(launcher));
    return hr;
}

void RunPressOriginal(const QueuedPress& press, const wchar_t* reason) {

    ExecDepthGuard depth;
    uint64_t state = 0;
    void* outer = nullptr;
    void* launcher = LauncherNow(&state, &outer);
    TraceLog(L"own press fallback trigger=%s entry=%s reason=%s launcher=%p "
            L"state=0x%llX",
            PressTriggerName(press.trigger), PressEntryName(press.entry), reason,
            launcher, static_cast<unsigned long long>(state));
    if (!launcher) {

        return;
    }
    ++t_execCall;
    switch (press.entry) {
        case kPressEntryHotkey:
            if (OnHotkeyInvoke_Original && outer) {
                OnHotkeyInvoke_Original(outer, press.flags);
            }
            break;
        case kPressEntryButton:
            if (OnStartButtonPressed_Original) {
                OnStartButtonPressed_Original(launcher, press.showMethod,
                                              press.dismissMethod);
            }
            break;
        case kPressEntryShow:
            if (ShowStartView_Original) {
                ShowStartView_Original(launcher, press.showMethod, press.flags);
            }
            break;
        case kPressEntryDismiss:
            if (Dismiss_Original) {
                Dismiss_Original(launcher, press.dismissMethod);
            }
            break;
        case kPressEntryHide:
            if (HideStartAndCortana_Original) {
                HideStartAndCortana_Original(OwnStateOf(launcher),
                                             press.dismissMethod);
            }
            break;
        default:
            break;
    }
    --t_execCall;
}

using DismissStartAndCortana_t = void (*)(void*, uint32_t);
DismissStartAndCortana_t DismissStartAndCortana_Original = nullptr;

void DismissStartAndCortana_Hook(void* self, uint32_t method) {
    const OwnCallerScope scope(OwnCaller::Ordinary);
    if (DismissStartAndCortana_Original) {
        DismissStartAndCortana_Original(self, method);
    }
}

using DismissSynchronously_t = HRESULT (*)(void*);
DismissSynchronously_t DismissSynchronously_Original = nullptr;

HRESULT DismissSynchronously_Hook(void* self) {
    const OwnCallerScope scope(OwnCaller::Lock);
    return DismissSynchronously_Original ? DismissSynchronously_Original(self)
                                         : E_FAIL;
}

using ActiveZBand_t = HRESULT (*)(void*, unsigned long, unsigned long, void*,
                                  int);
ActiveZBand_t ActiveZBand_Original = nullptr;

HRESULT ActiveZBand_Hook(void* self, unsigned long a, unsigned long b,
                         void* monitor, int reason) {
    const OwnCallerScope scope(OwnCaller::ClickAway);
    return ActiveZBand_Original ? ActiveZBand_Original(self, a, b, monitor, reason)
                                : E_FAIL;
}

enum class ExplorerHookOutcome {
    Settled,
    VersionRefused,
    Unresolved,
};

ExplorerHookOutcome InstallExplorerHooks(HMODULE module) {
    if (!module) {
        return ExplorerHookOutcome::Unresolved;
    }

    AcquireSRWLockExclusive(&g_explorerSymbolLock);
    HMODULE expected = nullptr;
    if (!g_explorerSymbolClaim.compare_exchange_strong(expected, module)) {
        ReleaseSRWLockExclusive(&g_explorerSymbolLock);
        return ExplorerHookOutcome::Unresolved;
    }
    if (g_disabled.load(std::memory_order_relaxed)) {
        ReleaseSRWLockExclusive(&g_explorerSymbolLock);
        return ExplorerHookOutcome::Unresolved;
    }

    const uint64_t version = ModuleFixedFileVersion(module);
    if (version != kTwinuiReferenceVersion) {
        g_launcherLayoutVerified.store(false, std::memory_order_release);
        g_explorerSymbolsRefused.store(true, std::memory_order_release);
        TraceLog(L"exec launcher layout unverified version=%u.%u.%u.%u matches=0 "
                L"native=1 reason=version skipped=version",
                static_cast<unsigned>((version >> 48) & 0xFFFF),
                static_cast<unsigned>((version >> 32) & 0xFFFF),
                static_cast<unsigned>((version >> 16) & 0xFFFF),
                static_cast<unsigned>(version & 0xFFFF));
        Wh_Log(L"twinui.pcshell.dll fixed version %u.%u.%u.%u is not the "
               L"verified reference %u.%u.%u.%u; symbols were not requested and "
               L"every press stays native",
               static_cast<unsigned>((version >> 48) & 0xFFFF),
               static_cast<unsigned>((version >> 32) & 0xFFFF),
               static_cast<unsigned>((version >> 16) & 0xFFFF),
               static_cast<unsigned>(version & 0xFFFF),
               static_cast<unsigned>((kTwinuiReferenceVersion >> 48) & 0xFFFF),
               static_cast<unsigned>((kTwinuiReferenceVersion >> 32) & 0xFFFF),
               static_cast<unsigned>((kTwinuiReferenceVersion >> 16) & 0xFFFF),
               static_cast<unsigned>(kTwinuiReferenceVersion & 0xFFFF));
        ReleaseSRWLockExclusive(&g_explorerSymbolLock);
        PublishExecutorCapability(L"symbols-refused");
        return ExplorerHookOutcome::VersionRefused;
    }
    TraceLog(L"exec launcher symbols begin module=%p", module);

    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {{L"public: virtual long __cdecl "
          L"CStartExperienceManager::Show(unsigned char,unsigned char,"
          L"unsigned int)"},
         &Show_Original, &Show_Hook},
        {{L"public: virtual void __cdecl "
          L"XamlLauncherState::HideStartAndCortana(enum "
          L"IMMERSIVELAUNCHERDISMISSMETHOD)"},
         &HideStartAndCortana_Original, &HideStartAndCortana_Hook},
        {{L"public: long __cdecl XamlLauncherState::OnStartShown(struct "
          L"Windows::Internal::Shell::Experience::IStartExperienceManager *,"
          L"struct IInspectable *)"},
         &OnStartShown_Original, &OnStartShown_Hook},
        {{L"public: long __cdecl XamlLauncherState::OnStartHidden(struct "
          L"Windows::Internal::Shell::Experience::IStartExperienceManager *,"
          L"struct IInspectable *)"},
         &OnStartHidden_Original, &OnStartHidden_Hook},
        {{L"private: long __cdecl XamlLauncher::OnHotkeyInvoke(enum "
          L"IMMERSIVELAUNCHERSHOWFLAGS)"},
         &OnHotkeyInvoke_Original, &OnHotkeyInvoke_Hook},
        {{L"public: virtual long __cdecl "
          L"XamlLauncher::OnStartButtonPressed(enum "
          L"IMMERSIVELAUNCHERSHOWMETHOD,enum IMMERSIVELAUNCHERDISMISSMETHOD)"},
         &OnStartButtonPressed_Original, &OnStartButtonPressed_Hook},
        {{L"public: virtual long __cdecl XamlLauncher::Dismiss(enum "
          L"IMMERSIVELAUNCHERDISMISSMETHOD)"},
         &Dismiss_Original, &Dismiss_Hook},
        {{L"public: virtual long __cdecl XamlLauncher::ShowStartView(enum "
          L"IMMERSIVELAUNCHERSHOWMETHOD,enum IMMERSIVELAUNCHERSHOWFLAGS)"},
         &ShowStartView_Original, &ShowStartView_Hook},
        {{L"public: virtual void __cdecl "
          L"XamlLauncherState::DismissStartAndCortana(enum "
          L"IMMERSIVELAUNCHERDISMISSMETHOD)"},
         &DismissStartAndCortana_Original, &DismissStartAndCortana_Hook},
        {{L"public: virtual long __cdecl "
          L"XamlLauncher::DismissSynchronouslyWithoutTransition(void)"},
         &DismissSynchronously_Original, &DismissSynchronously_Hook},
        {{L"public: virtual long __cdecl "
          L"ImmersiveMonitorHelper::ActiveZBandChangedForMonitor(unsigned "
          L"long,unsigned long,struct IImmersiveMonitor *,"
          L"enum MODE_CHANGE_REASON)"},
         &ActiveZBand_Original, &ActiveZBand_Hook},
        {{L"private: static class XamlLauncher * XamlLauncher::s_pLauncher"},
         &g_launcherSlot, nullptr},
        {{L"public: virtual long __cdecl XamlLauncher::IsVisible(int *)"},
         &g_launcherIsVisible, nullptr},
    };
    const bool hooked =
        WindhawkUtils::HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));

    if (!hooked || !Show_Original || !HideStartAndCortana_Original ||
        !OnStartShown_Original || !OnStartHidden_Original ||
        !OnHotkeyInvoke_Original || !OnStartButtonPressed_Original ||
        !Dismiss_Original || !ShowStartView_Original ||
        !DismissStartAndCortana_Original || !DismissSynchronously_Original ||
        !ActiveZBand_Original || !g_launcherSlot || !g_launcherIsVisible) {
        g_explorerSymbolClaim.store(nullptr, std::memory_order_release);
        ReleaseSRWLockExclusive(&g_explorerSymbolLock);
        return ExplorerHookOutcome::Unresolved;
    }

    const uintptr_t base = reinterpret_cast<uintptr_t>(module);
    struct ResolvedSymbol {
        const wchar_t* label;
        const void* address;
        uintptr_t expectedRva;
    };
    const ResolvedSymbol resolved[] = {
        {L"show", reinterpret_cast<const void*>(Show_Original), kRvaShow},
        {L"hide", reinterpret_cast<const void*>(HideStartAndCortana_Original),
         kRvaHideStartAndCortana},
        {L"shown", reinterpret_cast<const void*>(OnStartShown_Original),
         kRvaOnStartShown},
        {L"hidden", reinterpret_cast<const void*>(OnStartHidden_Original),
         kRvaOnStartHidden},
        {L"win", reinterpret_cast<const void*>(OnHotkeyInvoke_Original),
         kRvaOnHotkeyInvoke},
        {L"button", reinterpret_cast<const void*>(OnStartButtonPressed_Original),
         kRvaOnStartButtonPressed},
        {L"dismiss", reinterpret_cast<const void*>(Dismiss_Original),
         kRvaDismiss},
        {L"showview", reinterpret_cast<const void*>(ShowStartView_Original),
         kRvaShowStartView},
        {L"ordinary", reinterpret_cast<const void*>(DismissStartAndCortana_Original),
         kRvaDismissStartAndCortana},
        {L"lock", reinterpret_cast<const void*>(DismissSynchronously_Original),
         kRvaDismissSynchronously},
        {L"clickaway", reinterpret_cast<const void*>(ActiveZBand_Original),
         kRvaActiveZBand},
        {L"slot", reinterpret_cast<const void*>(g_launcherSlot),
         kRvaLauncherSlot},
        {L"visible", reinterpret_cast<const void*>(g_launcherIsVisible),
         kRvaLauncherIsVisible},
    };

    int matches = 0;
    for (const ResolvedSymbol& symbol : resolved) {
        const uintptr_t rva =
            reinterpret_cast<uintptr_t>(symbol.address) - base;
        matches += rva == symbol.expectedRva ? 1 : 0;
    }
    const bool layoutVerified =
        matches == static_cast<int>(ARRAYSIZE(resolved)) &&
        version == kTwinuiReferenceVersion;
    g_launcherLayoutVerified.store(layoutVerified, std::memory_order_release);

    g_dismiss = Dismiss_Original;
    g_showStartView = ShowStartView_Original;
    g_closeHookEnabled.store(layoutVerified, std::memory_order_relaxed);
    g_ownPrimitivesEnabled.store(layoutVerified, std::memory_order_relaxed);
    g_ownWinEnabled.store(layoutVerified, std::memory_order_relaxed);
    g_ownButtonEnabled.store(layoutVerified, std::memory_order_relaxed);
    g_ownEventsEnabled.store(layoutVerified, std::memory_order_relaxed);
    g_callersVerified.store(layoutVerified, std::memory_order_relaxed);
    g_explorerSymbolsReady.store(true, std::memory_order_release);

    Wh_ApplyHookOperations();
    TraceLog(L"exec launcher hooks applied tid=%u", GetCurrentThreadId());
    PublishExecutorCapability(L"symbols");
    ReleaseSRWLockExclusive(&g_explorerSymbolLock);

    for (const ResolvedSymbol& symbol : resolved) {
        const uintptr_t rva =
            reinterpret_cast<uintptr_t>(symbol.address) - base;
        TraceLog(L"exec launcher symbol label=%s rva=0x%llX expected=0x%llX "
                L"match=%d",
                symbol.label, static_cast<unsigned long long>(rva),
                static_cast<unsigned long long>(symbol.expectedRva),
                rva == symbol.expectedRva ? 1 : 0);
    }
    TraceLog(L"exec launcher symbols resolved base=0x%llX count=%d matches=%d "
            L"match=%d",
            static_cast<unsigned long long>(base),
            static_cast<int>(ARRAYSIZE(resolved)), matches,
            matches == static_cast<int>(ARRAYSIZE(resolved)) ? 1 : 0);

    TraceLog(L"exec launcher layout %s version=%u.%u.%u.%u matches=%d "
            L"native=%d reason=%s",
            layoutVerified ? L"verified" : L"unverified",
            static_cast<unsigned>((version >> 48) & 0xFFFF),
            static_cast<unsigned>((version >> 32) & 0xFFFF),
            static_cast<unsigned>((version >> 16) & 0xFFFF),
            static_cast<unsigned>(version & 0xFFFF), matches,
            layoutVerified ? 0 : 1,
            version != kTwinuiReferenceVersion
                ? L"version"
                : (matches != static_cast<int>(ARRAYSIZE(resolved)) ? L"rva"
                                                                    : L"none"));
    Wh_Log(L"twinui.pcshell.dll symbols resolved at %p (%d/%d at the reference "
           L"RVAs); launcher hooks installed layout=%d",
           reinterpret_cast<void*>(base), matches,
           static_cast<int>(ARRAYSIZE(resolved)), layoutVerified ? 1 : 0);
    return ExplorerHookOutcome::Settled;
}

void ProbeExplorerSymbolsFromWorker() {
    if (!g_afterInit.load(std::memory_order_relaxed) ||
        g_disabled.load(std::memory_order_relaxed) ||
        g_explorerSymbolAttempted.load(std::memory_order_acquire) ||
        g_explorerSymbolsReady.load(std::memory_order_acquire) ||
        g_explorerSymbolsRefused.load(std::memory_order_acquire)) {
        return;
    }
    HMODULE module = g_explorerModuleRef;
    if (!module) {

        if (!GetModuleHandleExW(0, kTwinuiModuleName, &module) || !module) {
            return;
        }
        g_explorerModuleRef = module;
    }

    g_explorerSymbolAttempted.store(true, std::memory_order_release);
    const ExplorerHookOutcome outcome = InstallExplorerHooks(module);
    if (outcome != ExplorerHookOutcome::Unresolved) {

        return;
    }
    if (g_disabled.load(std::memory_order_relaxed)) {
        return;
    }
    g_explorerSymbolsRefused.store(true, std::memory_order_release);
    PublishExecutorCapability(L"symbols-refused");
    TraceLog(L"exec launcher symbols unresolved attempts=1 "
            L"name=twinui.pcshell.dll (symbol, signature or hook refused)");
    Wh_Log(L"twinui.pcshell.dll symbols could not be resolved on the one "
           L"attempt this session makes; no launcher hook is installed and "
           L"every press stays native");
}

void* LauncherNow(uint64_t* stateOut, void** outerOut) {
    if (stateOut) {
        *stateOut = 0;
    }
    if (outerOut) {
        *outerOut = nullptr;
    }

    if (!g_launcherLayoutVerified.load(std::memory_order_acquire)) {
        return nullptr;
    }
    if (!g_launcherSlot) {
        return nullptr;
    }

    void* outer = *g_launcherSlot;
    if (outerOut) {
        *outerOut = outer;
    }
    if (!outer) {
        return nullptr;
    }
    void* launcher = static_cast<uint8_t*>(outer) + kHotkeyLauncherOffset;
    const uint64_t state = *reinterpret_cast<uint64_t*>(
        static_cast<uint8_t*>(launcher) + kLauncherStatePointerOffset);
    if (stateOut) {
        *stateOut = state;
    }
    if (!LauncherStateReady(state)) {
        return nullptr;
    }
    return launcher;
}

void AnswerExec(LONG64 sequence, int32_t state, HRESULT hr, int visible) {
    if (sequence > 0) {
        SwipeIpcAnswerExec(&g_ipc, sequence, state, hr, visible);
    }
}

enum class ProbeVisibility { Hidden, Visible, Unknown };

int32_t ProbeVisibilityValue(ProbeVisibility value) {
    switch (value) {
        case ProbeVisibility::Hidden:
            return kSwipeVisibilityHidden;
        case ProbeVisibility::Visible:
            return kSwipeVisibilityVisible;
        default:
            return kSwipeVisibilityUnknown;
    }
}

const wchar_t* ProbeVisibilityName(ProbeVisibility value) {
    switch (value) {
        case ProbeVisibility::Hidden:
            return L"hidden";
        case ProbeVisibility::Visible:
            return L"visible";
        default:
            return L"unknown";
    }
}

ProbeVisibility LauncherVisibleNow(void* launcher) {
    if (!launcher || !g_launcherIsVisible) {
        return ProbeVisibility::Unknown;
    }
    BOOL visible = FALSE;
    const HRESULT hr = g_launcherIsVisible(launcher, &visible);
    if (FAILED(hr)) {
        return ProbeVisibility::Unknown;
    }
    return visible != FALSE ? ProbeVisibility::Visible
                            : ProbeVisibility::Hidden;
}

void ArmVisibleProbe(LONG64 sequence) {
    g_pendingShowSeq = sequence;
    if (g_execWindow.load(std::memory_order_relaxed)) {
        SetTimer(g_execWindow.load(std::memory_order_relaxed), kExecProbeTimer,
                 300, nullptr);
    }
}

bool ExecCommandExpired(const SwipeIpcExecCommand& command, uint64_t now) {
    const uint64_t due = command.kind == kSwipeExecDismiss ? command.executeAt
                                                           : command.tick;
    return due != 0 && now >= due && now - due >= kExecExpiryMs;
}

void ArmDismissConfirm(LONG64 sequence, ProbeVisibility visible) {
    if (g_pendingDismissConfirmSeq != 0 &&
        g_pendingDismissConfirmSeq != sequence) {
        TraceLog(L"exec dismiss superseded seq=%lld by=%lld",
                static_cast<long long>(g_pendingDismissConfirmSeq),
                static_cast<long long>(sequence));
        AnswerExec(g_pendingDismissConfirmSeq, kSwipeAckSuperseded, S_OK,
                   g_pendingDismissConfirmVisible);
    }
    g_pendingDismissConfirmSeq = sequence;
    g_pendingDismissConfirmVisible = ProbeVisibilityValue(visible);
    HWND window = g_execWindow.load(std::memory_order_relaxed);
    if (!window) {
        TraceLog(L"exec dismiss confirm timer failed seq=%lld reason=no-window",
                static_cast<long long>(sequence));
        g_pendingDismissConfirmSeq = 0;
        AnswerExec(sequence, kSwipeAckUnknown, S_OK, kSwipeVisibilityUnknown);
        return;
    }
    if (SetTimer(window, kExecDismissProbeTimer,
                 static_cast<UINT>(kExecDismissConfirmMs), nullptr) == 0) {
        TraceLog(L"exec dismiss confirm timer failed seq=%lld err=%u",
                static_cast<long long>(sequence), GetLastError());
        g_pendingDismissConfirmSeq = 0;
        AnswerExec(sequence, kSwipeAckUnknown, S_OK, kSwipeVisibilityUnknown);
        return;
    }
    g_pendingDismissConfirmTick = GetTickCount64();
}

void KillDismissConfirm() {
    g_pendingDismissConfirmSeq = 0;
    g_pendingDismissConfirmVisible = 0;
    g_pendingDismissConfirmTick = 0;
    HWND window = g_execWindow.load(std::memory_order_relaxed);
    if (window) {
        KillTimer(window, kExecDismissProbeTimer);
    }
}

bool ExecCommandForCurrentMenu(const SwipeIpcExecCommand& command,
                               const wchar_t* kind) {
    if (command.kind == kSwipeExecShow) {

        // A show names no menu: it claims the next open, it does not dismiss
        // one, so the epoch and the ack bound are its only checks.
        return true;
    }
    uint64_t open = 0;
    uint64_t openTick = 0;
    if (!SwipeIpcReadOpen(&g_ipc, &open, &openTick)) {

        // A failed read leaves the generation unknown; a dismiss naming a menu
        // cannot be proven to hit that menu.
        TraceLog(L"exec %s seq=%lld superseded reason=unreadable menu=%llu",
                kind, static_cast<long long>(command.seq),
                static_cast<unsigned long long>(command.open));
        return false;
    }
    if (open == command.open) {
        return true;
    }
    TraceLog(L"exec %s seq=%lld superseded menu=%llu now=%llu", kind,
            static_cast<long long>(command.seq),
            static_cast<unsigned long long>(command.open),
            static_cast<unsigned long long>(open));
    return false;
}

bool ValidExecCommand(const SwipeIpcExecCommand& command) {
    switch (command.kind) {
        case kSwipeExecShow:
            return command.executeAt == 0 && command.target == 0 &&
                   SwipeShowAllowed(static_cast<uint32_t>(command.method),
                                    static_cast<uint32_t>(command.flags));
        case kSwipeExecDismiss:
            return command.flags == 0 && command.target == 0 &&
                   SwipeDismissAllowed(static_cast<uint32_t>(command.method));
        case kSwipeExecCancel:
            return command.method == 0 && command.flags == 0 &&
                   command.executeAt == 0;
        default:
            return false;
    }
}

void ExecuteShow(const SwipeIpcExecCommand& command) {
    const uint64_t now = GetTickCount64();
    if (ExecCommandExpired(command, now)) {

        TraceLog(L"exec show seq=%lld expired age=%llu (publisher released it)",
                static_cast<long long>(command.seq),
                static_cast<unsigned long long>(now - command.tick));
        AnswerExec(command.seq, kSwipeAckExpired,
                   HRESULT_FROM_WIN32(ERROR_TIMEOUT), kSwipeVisibilityUnknown);
        return;
    }
    if (g_pressOutstanding.pending && g_pressOutstanding.external) {
        TraceLog(L"exec show seq=%lld refused reason=external-barrier "
                L"barrier=%lld",
                static_cast<long long>(command.seq),
                static_cast<long long>(g_pressOutstanding.seq));
        AnswerExec(command.seq, kSwipeAckRefused,
                   HRESULT_FROM_WIN32(ERROR_RETRY), kSwipeVisibilityUnknown);
        return;
    }
    if (!ExecCommandForCurrentMenu(command, L"show")) {
        AnswerExec(command.seq, kSwipeAckSuperseded,
                   HRESULT_FROM_WIN32(ERROR_TIMEOUT), kSwipeVisibilityUnknown);
        return;
    }

    if (g_pendingDismissConfirmSeq != 0) {
        TraceLog(L"exec dismiss confirm superseded seq=%lld reason=show",
                static_cast<long long>(g_pendingDismissConfirmSeq));
        AnswerExec(g_pendingDismissConfirmSeq, kSwipeAckSuperseded, S_OK,
                   g_pendingDismissConfirmVisible);
    }
    KillDismissConfirm();
    uint64_t state = 0;
    void* outer = nullptr;
    void* launcher = LauncherNow(&state, &outer);
    if (!launcher) {
        TraceLog(L"exec show seq=%lld refused outer=%p launcher=%p state=0x%llX "
                L"reason=not-ready",
                static_cast<long long>(command.seq), outer, launcher,
                static_cast<unsigned long long>(state));
        AnswerExec(command.seq, kSwipeAckRefused,
                   HRESULT_FROM_WIN32(ERROR_NOT_FOUND), kSwipeVisibilityUnknown);
        return;
    }
    const uint32_t epoch = PeerEpochNow();
    if (epoch != g_dispatchEpoch) {

        TraceLog(L"exec show seq=%lld superseded epoch=%u now=%u",
                static_cast<long long>(command.seq), g_dispatchEpoch, epoch);
        AnswerExec(command.seq, kSwipeAckSuperseded,
                   HRESULT_FROM_WIN32(ERROR_TIMEOUT), kSwipeVisibilityUnknown);
        return;
    }
    ++t_execCall;
    const HRESULT hr = g_showStartView
                           ? g_showStartView(launcher, command.method,
                                             command.flags)
                           : E_FAIL;

    const ProbeVisibility visible = LauncherVisibleNow(launcher);
    --t_execCall;
    TraceLog(L"exec show seq=%lld outer=%p launcher=%p state=0x%llX hr=%08X "
            L"visible=%s",
            static_cast<long long>(command.seq), outer, launcher,
            static_cast<unsigned long long>(state),
            static_cast<unsigned>(hr), ProbeVisibilityName(visible));
    AnswerExec(command.seq, SUCCEEDED(hr) ? kSwipeAckExecuted : kSwipeAckRefused,
               hr, ProbeVisibilityValue(visible));
    if (SUCCEEDED(hr)) {
        ArmVisibleProbe(command.seq);
    }
}

void ExecuteDismiss(const SwipeIpcExecCommand& command) {
    const uint64_t now = GetTickCount64();
    if (ExecCommandExpired(command, now)) {

        TraceLog(L"exec dismiss seq=%lld expired age=%llu candidate=%lld",
                static_cast<long long>(command.seq),
                static_cast<unsigned long long>(now > command.executeAt
                                                    ? now - command.executeAt
                                                    : 0),
                static_cast<long long>(g_pendingDismissSeq));
        AnswerExec(command.seq, kSwipeAckExpired,
                   HRESULT_FROM_WIN32(ERROR_TIMEOUT), kSwipeVisibilityUnknown);
        DropPressLatch(L"dismiss-expired", L"executor");
        return;
    }
    if (!ExecCommandForCurrentMenu(command, L"dismiss")) {
        AnswerExec(command.seq, kSwipeAckSuperseded,
                   HRESULT_FROM_WIN32(ERROR_TIMEOUT), kSwipeVisibilityUnknown);
        DropPressLatch(L"dismiss-superseded", L"menu");
        return;
    }

    g_pendingShowSeq = 0;
    if (g_execWindow.load(std::memory_order_relaxed)) {
        KillTimer(g_execWindow.load(std::memory_order_relaxed), kExecProbeTimer);
    }
    uint64_t state = 0;
    void* outer = nullptr;
    void* launcher = LauncherNow(&state, &outer);
    if (!launcher) {
        TraceLog(L"exec dismiss seq=%lld refused outer=%p launcher=%p "
                L"state=0x%llX reason=not-ready",
                static_cast<long long>(command.seq), outer, launcher,
                static_cast<unsigned long long>(state));
        AnswerExec(command.seq, kSwipeAckRefused,
                   HRESULT_FROM_WIN32(ERROR_NOT_FOUND), kSwipeVisibilityUnknown);
        DropPressLatch(L"dismiss-refused", L"executor");
        return;
    }

    const uint32_t epoch = PeerEpochNow();
    if (epoch != g_pendingDismissEpoch) {
        TraceLog(L"exec dismiss seq=%lld superseded epoch=%u now=%u",
                static_cast<long long>(command.seq), g_pendingDismissEpoch,
                epoch);
        AnswerExec(command.seq, kSwipeAckSuperseded,
                   HRESULT_FROM_WIN32(ERROR_TIMEOUT), kSwipeVisibilityUnknown);
        DropPressLatch(L"dismiss-superseded", L"epoch");
        return;
    }
    ++t_execCall;
    const uint64_t enterAt = start::QpcMsNow();
    const HRESULT hr =
        g_dismiss ? g_dismiss(launcher, command.method)
                  : E_FAIL;

    const ProbeVisibility visible = LauncherVisibleNow(launcher);
    --t_execCall;

    TraceLog(L"exec dismiss enter seq=%llu gen=%llu epoch=%llu at=%llu",
            static_cast<unsigned long long>(command.seq),
            static_cast<unsigned long long>(command.open),
            static_cast<unsigned long long>(epoch),
            static_cast<unsigned long long>(enterAt));
    TraceLog(L"exec dismiss seq=%lld outer=%p launcher=%p state=0x%llX hr=%08X "
            L"visible=%s",
            static_cast<long long>(command.seq), outer, launcher,
            static_cast<unsigned long long>(state),
            static_cast<unsigned>(hr), ProbeVisibilityName(visible));
    AnswerExec(command.seq, SUCCEEDED(hr) ? kSwipeAckExecuted : kSwipeAckRefused,
               hr, ProbeVisibilityValue(visible));
    if (SUCCEEDED(hr)) {
        ArmDismissConfirm(command.seq, visible);
    } else {

        DropPressLatch(L"dismiss-refused", L"executor");
    }
}

bool ArmDismissTimer(uint32_t delayMs) {
    HWND window = g_execWindow.load(std::memory_order_relaxed);
    if (!window) {
        return false;
    }
    if (SetTimer(window, kExecDismissTimer, delayMs, nullptr) == 0) {
        TraceLog(L"exec dismiss timer failed err=%u", GetLastError());
        return false;
    }
    return true;
}

void ExecuteDueDismiss() {
    if (g_pendingDismissSeq == 0) {
        return;
    }
    SwipeIpcExecCommand command{};
    if (!SwipeIpcReadExec(&g_ipc, &command)) {

        if (++g_dismissReadRetries <= kExecDismissReadRetries &&
            ArmDismissTimer(kExecDismissRetryMs)) {
            TraceLog(L"exec dismiss seq=%lld read retry=%d",
                    static_cast<long long>(g_pendingDismissSeq),
                    g_dismissReadRetries);
            return;
        }
        const LONG64 sequence = g_pendingDismissSeq;
        const int retries = g_dismissReadRetries;
        g_pendingDismissSeq = 0;
        g_dismissReadRetries = 0;
        TraceLog(L"exec dismiss seq=%lld refused reason=unreadable retries=%d",
                static_cast<long long>(sequence), retries);
        AnswerExec(sequence, kSwipeAckRefused, E_FAIL, 0);
        DropPressLatch(L"dismiss-refused", L"unreadable");
        return;
    }
    g_dismissReadRetries = 0;
    if (command.seq != g_pendingDismissSeq ||
        command.kind != kSwipeExecDismiss) {
        TraceLog(L"exec dismiss seq=%lld superseded by %lld",
                static_cast<long long>(g_pendingDismissSeq),
                static_cast<long long>(command.seq));
        if (g_pendingDismissSeq != 0) {
            AnswerExec(g_pendingDismissSeq, kSwipeAckSuperseded, S_OK,
                       kSwipeVisibilityUnknown);
        }
        g_pendingDismissSeq = 0;
        g_pendingDismissEpoch = 0;
        return;
    }
    g_pendingDismissSeq = 0;
    ExecuteDismiss(command);
}

// A command the executor has not dispatched is still in the section. Answering
// it superseded and naming it for the dispatch loop is how a native show or an
// external takeover skips an action its publisher has withdrawn.
bool SuppressPublishedCommand(int32_t kind, const wchar_t* reason) {
    SwipeIpcExecCommand published{};
    if (!SwipeIpcReadExec(&g_ipc, &published) || published.seq == 0) {
        return false;
    }
    if (published.kind != kind || published.seq <= g_execAcceptedSeq ||
        published.seq == g_pendingDismissSeq ||
        published.seq == g_pendingShowSeq) {
        return false;
    }
    g_suppressedExecSeq = published.seq;
    AnswerExec(published.seq, kSwipeAckSuperseded, S_OK,
               kSwipeVisibilityUnknown);
    TraceLog(L"exec cancel target=%lld reason=%s",
            static_cast<long long>(published.seq), reason);
    return true;
}

void CancelArmedDismissForNativeShow() {
    if (g_pendingDismissSeq != 0) {
        const LONG64 target = g_pendingDismissSeq;
        g_pendingDismissSeq = 0;
        g_pendingDismissEpoch = 0;
        HWND window = g_execWindow.load(std::memory_order_relaxed);
        if (window) {
            KillTimer(window, kExecDismissTimer);
        }
        AnswerExec(target, kSwipeAckSuperseded, S_OK, kSwipeVisibilityUnknown);
        TraceLog(L"exec cancel target=%lld reason=native-show",
                static_cast<long long>(target));
    }

    SuppressPublishedCommand(kSwipeExecDismiss, L"native-show-published");
}

void ExecutePendingCommand() {
    // Sample the epoch before the copy. Sampling it after the copy labels a
    // command from the previous controller with the next controller's epoch.
    const uint32_t epochBefore = PeerEpochNow();
    SwipeIpcExecCommand command{};
    if (!SwipeIpcReadExec(&g_ipc, &command) || command.seq == 0) {
        return;
    }
    if (command.seq == g_suppressedExecSeq) {

        g_suppressedExecSeq = 0;
        g_execAcceptedSeq = command.seq;
        TraceLog(L"exec superseded seq=%lld reason=suppressed",
                static_cast<long long>(command.seq));
        return;
    }
    if (command.seq == g_execAcceptedSeq) {

        return;
    }
    g_execAcceptedSeq = command.seq;

    g_dispatchEpoch = epochBefore;
    if (!ValidExecCommand(command)) {
        TraceLog(L"exec refused seq=%lld kind=%d method=%u flags=%u target=%lld "
                L"reason=invalid",
                static_cast<long long>(command.seq),
                static_cast<int>(command.kind), command.method, command.flags,
                static_cast<long long>(command.target));
        AnswerExec(command.seq, kSwipeAckRefused, E_INVALIDARG,
                   kSwipeVisibilityUnknown);
        return;
    }
    if (command.kind == kSwipeExecCancel) {
        if (g_pendingDismissSeq != 0 &&
            (command.target == 0 || command.target == g_pendingDismissSeq)) {
            KillTimer(g_execWindow.load(), kExecDismissTimer);
            TraceLog(L"exec cancel seq=%lld target=%lld",
                    static_cast<long long>(command.seq),
                    static_cast<long long>(g_pendingDismissSeq));

            AnswerExec(command.seq, kSwipeAckExecuted, S_OK, 0);
            AnswerExec(g_pendingDismissSeq, kSwipeAckSuperseded, S_OK,
                       kSwipeVisibilityUnknown);
            g_pendingDismissSeq = 0;
            g_pendingDismissEpoch = 0;
        } else {
            AnswerExec(command.seq, kSwipeAckExecuted, S_OK, 0);
        }
        if (g_pendingShowSeq != 0 &&
            (command.target == 0 || command.target == g_pendingShowSeq)) {
            KillTimer(g_execWindow.load(), kExecProbeTimer);
            AnswerExec(g_pendingShowSeq, kSwipeAckSuperseded, S_OK,
                       kSwipeVisibilityUnknown);
            TraceLog(L"exec cancel show target=%lld reason=withdrawn",
                    static_cast<long long>(g_pendingShowSeq));
            g_pendingShowSeq = 0;
        }
        return;
    }
    const uint32_t epochAfter = PeerEpochNow();
    if (epochAfter != epochBefore) {

        TraceLog(L"exec %s seq=%lld superseded epoch=%u now=%u",
                command.kind == kSwipeExecShow ? L"show" : L"dismiss",
                static_cast<long long>(command.seq), epochBefore, epochAfter);
        AnswerExec(command.seq, kSwipeAckSuperseded,
                   HRESULT_FROM_WIN32(ERROR_TIMEOUT), kSwipeVisibilityUnknown);
        if (command.kind == kSwipeExecDismiss) {
            DropPressLatch(L"dismiss-superseded", L"epoch");
        }
        return;
    }
    if (g_pendingDismissSeq != 0 && command.seq != g_pendingDismissSeq) {
        KillTimer(g_execWindow.load(), kExecDismissTimer);
        AnswerExec(g_pendingDismissSeq, kSwipeAckSuperseded, S_OK,
                   kSwipeVisibilityUnknown);
        g_pendingDismissSeq = 0;
        g_pendingDismissEpoch = 0;
    }
    if (command.kind == kSwipeExecShow) {
        ExecuteShow(command);
        return;
    }
    if (command.kind == kSwipeExecDismiss) {
        const uint64_t now = GetTickCount64();
        if (command.executeAt > now) {
            const uint32_t delay = static_cast<UINT>(command.executeAt - now);
            if (ArmDismissTimer(delay)) {
                g_pendingDismissSeq = command.seq;
                g_pendingDismissEpoch = g_dispatchEpoch;
                g_dismissReadRetries = 0;
                TraceLog(L"exec dismiss seq=%lld armed in=%u ms epoch=%u",
                        static_cast<long long>(command.seq), delay,
                        g_pendingDismissEpoch);
                return;
            }

            // The command must not run before executeAt, and without a timer
            // it cannot wait. Refuse it; the controller repairs.
            TraceLog(L"exec dismiss seq=%lld refused reason=no-timer delay=%u",
                    static_cast<long long>(command.seq), delay);
            AnswerExec(command.seq, kSwipeAckRefused, E_FAIL,
                       kSwipeVisibilityUnknown);
            DropPressLatch(L"dismiss-refused", L"no-timer");
            return;
        }
        g_pendingDismissEpoch = g_dispatchEpoch;
        ExecuteDismiss(command);
    }
}

void StopExecutorTeardown(HWND window) {
    g_execTeardownPending = false;
    g_execRetirePending = false;
    KillTimer(window, kExecDismissTimer);
    KillTimer(window, kExecProbeTimer);
    KillTimer(window, kPressLatchTimer);
    KillTimer(window, kExecDismissProbeTimer);
    KillTimer(window, kPressAckTimer);
    KillTimer(window, kPressOpenGenTimer);
    g_execReady.store(false, std::memory_order_release);
    PublishExecutorCapability(L"teardown");
    if (g_execTeardownAck) {
        SetEvent(g_execTeardownAck);
    }
}

void RetireExecutorTeardown(HWND window) {
    g_execTeardownPending = false;
    g_execRetirePending = false;
    KillTimer(window, kExecDismissTimer);
    KillTimer(window, kExecProbeTimer);
    KillTimer(window, kPressLatchTimer);
    KillTimer(window, kExecDismissProbeTimer);
    KillTimer(window, kPressAckTimer);
    KillTimer(window, kPressOpenGenTimer);
    g_execDispatchClosed.store(true, std::memory_order_release);
    PublishExecutorCapability(L"retire");
}

LRESULT CALLBACK ExecutorWindowProc(HWND window, UINT message, WPARAM wParam,
                                    LPARAM lParam) {
    if (message == kExecDrainMessage) {

        if (wParam != kExecDrainMagic ||
            window != g_execWindow.load(std::memory_order_acquire)) {
            return 0;
        }
        g_execDrainServed.store(true, std::memory_order_release);
        return 0;
    }
    if (message == kExecRetireMessage) {

        if (wParam != kExecRetireMagic ||
            window != g_execWindow.load(std::memory_order_acquire)) {
            TraceLog(L"exec retire ignored hwnd=%p", window);
            return 0;
        }
        if (g_execOwnerDepth > 0) {
            g_execRetirePending = true;
            TraceLog(L"exec retire deferred depth=%d", g_execOwnerDepth);
            return 0;
        }
        TraceLog(L"exec window retire served hwnd=%p tid=%u", window,
                GetCurrentThreadId());
        RetireExecutorTeardown(window);
        DestroyWindow(window);
        return 0;
    }
    if (message == kExecTeardownMessage) {

        if (wParam != kExecTeardownMagic ||
            window != g_execWindow.load(std::memory_order_acquire)) {
            TraceLog(L"exec teardown ignored hwnd=%p", window);
            return 0;
        }
        if (g_execOwnerDepth > 0) {

            g_execTeardownPending = true;
            TraceLog(L"exec teardown deferred depth=%d", g_execOwnerDepth);
            return 0;
        }
        TraceLog(L"exec teardown served hwnd=%p tid=%u", window,
                GetCurrentThreadId());
        StopExecutorTeardown(window);
        return 0;
    }
    if (message == kExecDestroyMessage) {
        if (wParam != kExecDestroyMagic ||
            window != g_execWindow.load(std::memory_order_acquire)) {
            return 0;
        }
        if (g_execOwnerDepth > 0) {
            return 0;
        }
        g_execDestroyServed.store(true, std::memory_order_release);
        TraceLog(L"exec destroy served hwnd=%p tid=%u", window,
                GetCurrentThreadId());
        DestroyWindow(window);
        return 0;
    }
    ExecDepthGuard depth;
    if (message == WM_NCDESTROY) {

        const bool identified =
            g_execWindow.load(std::memory_order_acquire) == window;
        if (identified) {
            g_execWindow.store(nullptr, std::memory_order_release);
        }
        PublishExecutorCapability(L"window-gone");
        TraceLog(L"exec window gone hwnd=%p identified=%d tid=%u", window,
                identified ? 1 : 0, GetCurrentThreadId());
        if (g_execTeardownAck) {
            SetEvent(g_execTeardownAck);
        }
        return 0;
    }

    if (window != g_execWindow.load(std::memory_order_acquire)) {
        return DefWindowProcW(window, message, wParam, lParam);
    }
    if (!g_execReady.load(std::memory_order_acquire) ||
        g_execDispatchClosed.load(std::memory_order_acquire) ||
        g_disabled.load(std::memory_order_relaxed)) {

        return DefWindowProcW(window, message, wParam, lParam);
    }
    if (message == kExecLatchMessage) {
        if (wParam == kExecLatchMagic) {

            ArmLatchedServe();
        }
        return 0;
    }
    if (message == kExecIntentAckMessage) {
        if (wParam == kExecIntentAckMagic) {

            HandlePressAck();
        }
        return 0;
    }
    if (message == kExecMessage) {
        ExecutePendingCommand();
        return 0;
    }
    if (message == WM_TIMER && wParam == kExecDismissTimer) {
        KillTimer(window, kExecDismissTimer);
        ExecuteDueDismiss();
        return 0;
    }
    if (message == WM_TIMER && wParam == kExecProbeTimer) {
        KillTimer(window, kExecProbeTimer);
        uint64_t state = 0;
        void* outer = nullptr;
        void* launcher = LauncherNow(&state, &outer);
        const ProbeVisibility visible = LauncherVisibleNow(launcher);
        TraceLog(L"exec visible seq=%lld launcher=%p state=0x%llX visible=%s",
                static_cast<long long>(g_pendingShowSeq), launcher,
                static_cast<unsigned long long>(state),
                ProbeVisibilityName(visible));
        return 0;
    }
    if (message == WM_TIMER && wParam == kExecDismissProbeTimer) {
        KillTimer(window, kExecDismissProbeTimer);
        const LONG64 sequence = g_pendingDismissConfirmSeq;
        const int32_t atCall = g_pendingDismissConfirmVisible;
        const uint64_t armedAt = g_pendingDismissConfirmTick;
        g_pendingDismissConfirmSeq = 0;
        g_pendingDismissConfirmVisible = 0;
        g_pendingDismissConfirmTick = 0;
        if (sequence != 0) {
            uint64_t state = 0;
            void* outer = nullptr;
            void* launcher = LauncherNow(&state, &outer);
            const ProbeVisibility visible = LauncherVisibleNow(launcher);
            const uint64_t elapsed =
                armedAt != 0 && GetTickCount64() > armedAt
                    ? GetTickCount64() - armedAt
                    : 0;

            const int32_t answer =
                visible == ProbeVisibility::Hidden
                    ? kSwipeAckConfirmed
                    : (visible == ProbeVisibility::Visible ? kSwipeAckUnconfirmed
                                                           : kSwipeAckUnknown);
            AnswerExec(sequence, answer, S_OK, ProbeVisibilityValue(visible));
            TraceLog(L"exec dismiss %s seq=%lld visible=%s at_call=%d after=%llu "
                    L"ms",
                    answer == kSwipeAckConfirmed
                        ? L"confirmed"
                        : (answer == kSwipeAckUnconfirmed ? L"unconfirmed"
                                                          : L"unknown"),
                    static_cast<long long>(sequence),
                    ProbeVisibilityName(visible), atCall,
                    static_cast<unsigned long long>(elapsed));
            if (visible != ProbeVisibility::Hidden) {

                DropPressLatch(answer == kSwipeAckUnconfirmed
                                   ? L"dismiss-unconfirmed"
                                   : L"dismiss-unknown",
                               L"confirmation");
            }
        }
        return 0;
    }
    if (message == WM_TIMER && wParam == kPressLatchTimer) {
        KillTimer(window, kPressLatchTimer);
        if (!ServeLatchedPress(g_pressServeReason) && g_pressLatch.pending) {

            g_pressServeReason = L"timeout";
            if (SetTimer(window, kPressLatchTimer,
                         static_cast<UINT>(kPressLatchRetryMs), nullptr) == 0) {
                DropPressLatch(L"timer-failed", L"retry");
            }
        }
        return 0;
    }
    if (message == WM_TIMER && wParam == kPressAckTimer) {
        KillTimer(window, kPressAckTimer);
        HandlePressAckTimeout();
        return 0;
    }
    if (message == WM_TIMER && wParam == kPressOpenGenTimer) {
        KillTimer(window, kPressOpenGenTimer);
        ServeNextPress(L"open-gen");
        return 0;
    }
    return DefWindowProcW(window, message, wParam, lParam);
}

void ReplaceLauncherThread(const wchar_t* source, DWORD previous);

DWORD NoteLauncherThread(const wchar_t* source, bool adopt) {
    const DWORD tid = GetCurrentThreadId();
    DWORD known = g_launcherThread.load(std::memory_order_relaxed);
    if (known == 0) {
        if (!adopt) {
            TraceLog(L"exec launcher thread not adopted source=%s", source);
            return 0;
        }
        if (g_launcherThread.compare_exchange_strong(known, tid)) {
            g_launcherProven.store(true, std::memory_order_release);
            TraceLog(L"exec launcher thread tid=%u source=%s", tid, source);
            known = tid;
        }
    }
    if (known != tid) {
        if (adopt && PeerUiThreadNow() != 0 &&
            !g_launcherProven.load(std::memory_order_acquire)) {

            ReplaceLauncherThread(source, known);
            return tid;
        }
        TraceLog(L"exec launcher thread mismatch known=%u tid=%u source=%s",
                known, tid, source);
        return 0;
    }
    if (adopt) {
        g_launcherProven.store(true, std::memory_order_release);
    }
    return tid;
}

void ReplaceLauncherThread(const wchar_t* source, DWORD previous) {
    const DWORD tid = GetCurrentThreadId();
    g_launcherThread.store(tid, std::memory_order_release);
    g_launcherProven.store(true, std::memory_order_release);
    TraceLog(L"exec launcher thread corrected known=%u tid=%u source=%s peerTid=%u",
            previous, tid, source, PeerUiThreadNow());

    RemoveExecutorThreadHook();

    g_execDispatchClosed.store(true, std::memory_order_release);
    PublishExecutorCapability(L"thread-corrected");
    HWND window = g_execWindow.load(std::memory_order_acquire);
    if (!window) {
        return;
    }
    if (g_execThreadId == tid) {

        if (g_execOwnerDepth == 0) {
            RetireExecutorTeardown(window);
            DestroyWindow(window);
        } else {
            g_execRetirePending = true;
        }
        return;
    }
    if (!PostMessageW(window, kExecRetireMessage, kExecRetireMagic, 0)) {
        TraceLog(L"exec window retire post failed hwnd=%p err=%u", window,
                GetLastError());
        g_execRetirePosted.store(nullptr, std::memory_order_release);
        return;
    }
    g_execRetirePosted.store(window, std::memory_order_release);
    TraceLog(L"exec window retire hwnd=%p owner=%u", window, g_execThreadId);
}

SRWLOCK g_execThreadHookLock = SRWLOCK_INIT;

bool RemoveExecutorThreadHookLocked() {
    HHOOK hook = g_execThreadHook.load(std::memory_order_acquire);
    if (!hook) {
        g_execThreadHookTarget.store(0, std::memory_order_release);
        return true;
    }
    if (UnhookWindowsHookEx(hook)) {
        g_execThreadHook.store(nullptr, std::memory_order_release);
        g_execThreadHookTarget.store(0, std::memory_order_release);
        return true;
    }
    const DWORD error = GetLastError();
    if (error == ERROR_INVALID_HOOK_HANDLE) {

        // The error proves the hook handle is invalid; it does not prove the
        // owning thread exited. The handle can unhook nothing, so it is
        // dropped: a retry with the same handle cannot succeed.
        g_execThreadHook.store(nullptr, std::memory_order_release);
        g_execThreadHookTarget.store(0, std::memory_order_release);
        TraceLog(L"exec thread hook already gone err=%u", error);
        return true;
    }

    // Keep the handle: a zero frame count proves nothing about the next
    // message entering a hook that is still installed.
    TraceLog(L"exec thread hook remove failed err=%u (kept for retry)", error);
    return false;
}

bool EnsureExecutorThreadHook(DWORD target) {
    if (target == 0) {
        return false;
    }
    AcquireSRWLockExclusive(&g_execThreadHookLock);

    if (g_disabled.load(std::memory_order_relaxed)) {
        ReleaseSRWLockExclusive(&g_execThreadHookLock);
        return false;
    }

    const DWORD known = g_launcherThread.load(std::memory_order_acquire);
    if (known != 0 && known != target) {
        ReleaseSRWLockExclusive(&g_execThreadHookLock);
        TraceLog(L"exec thread proposal refused known=%u target=%u", known,
                target);
        return false;
    }
    if (g_execThreadHook.load(std::memory_order_acquire) &&
        g_execThreadHookTarget.load(std::memory_order_acquire) == target) {
        ReleaseSRWLockExclusive(&g_execThreadHookLock);
        return true;
    }
    if (!RemoveExecutorThreadHookLocked()) {
        ReleaseSRWLockExclusive(&g_execThreadHookLock);
        TraceLog(L"exec thread hook replace refused target=%u", target);
        return false;
    }
    HHOOK hook = SetWindowsHookExW(WH_GETMESSAGE, ExecutorThreadHook, nullptr,
                                   target);
    if (!hook) {
        ReleaseSRWLockExclusive(&g_execThreadHookLock);
        TraceLog(L"exec thread hook failed target=%u err=%u", target,
                GetLastError());
        return false;
    }
    g_execThreadHook.store(hook, std::memory_order_release);
    g_execThreadHookTarget.store(target, std::memory_order_release);
    ReleaseSRWLockExclusive(&g_execThreadHookLock);
    TraceLog(L"exec thread hook target=%u", target);
    return true;
}

bool RemoveExecutorThreadHook() {
    AcquireSRWLockExclusive(&g_execThreadHookLock);
    const bool removed = RemoveExecutorThreadHookLocked();
    ReleaseSRWLockExclusive(&g_execThreadHookLock);
    return removed;
}

void RemoveExecutorThreadHookIfCurrent(HHOOK serviced) {
    AcquireSRWLockExclusive(&g_execThreadHookLock);
    if (serviced &&
        g_execThreadHook.load(std::memory_order_acquire) == serviced) {
        RemoveExecutorThreadHookLocked();
    }
    ReleaseSRWLockExclusive(&g_execThreadHookLock);
}

bool RetireSupersededWindow(const wchar_t* source) {
    if (g_disabled.load(std::memory_order_relaxed)) {
        return false;
    }
    HWND window = g_execWindow.load(std::memory_order_acquire);
    if (!window) {
        g_execRetirePosted.store(nullptr, std::memory_order_release);
        return false;
    }
    const DWORD committed = g_launcherThread.load(std::memory_order_acquire);
    if (committed == 0 || g_execThreadId == committed) {
        return false;
    }
    if (g_execRetirePosted.load(std::memory_order_acquire) == window) {

        return true;
    }
    if (g_execThreadId == GetCurrentThreadId()) {

        if (g_execOwnerDepth == 0) {
            TraceLog(L"exec window retire retry source=%s hwnd=%p tid=%u", source,
                    window, g_execThreadId);
            RetireExecutorTeardown(window);
            DestroyWindow(window);
        } else {
            g_execRetirePending = true;
        }
        return true;
    }
    if (!PostMessageW(window, kExecRetireMessage, kExecRetireMagic, 0)) {

        g_execRetirePosted.store(nullptr, std::memory_order_release);
        TraceLog(L"exec window retire retry failed source=%s hwnd=%p err=%u",
                source, window, GetLastError());
        return true;
    }
    g_execRetirePosted.store(window, std::memory_order_release);
    TraceLog(L"exec window retire retry source=%s hwnd=%p owner=%u", source,
            window, g_execThreadId);
    return true;
}

void EnsureExecutorOnLauncherThread(const wchar_t* source) {
    RetireSupersededWindow(source);
    if (ExecutorWindowOnThisThread()) {
        return;
    }
    const DWORD tid = GetCurrentThreadId();
    const DWORD known = g_launcherThread.load(std::memory_order_acquire);
    if (known != 0) {
        if (known != tid) {
            TraceLog(L"exec launcher thread mismatch known=%u tid=%u source=%s",
                    known, tid, source);
            return;
        }

        EnsureExecutorThreadHook(tid);
        return;
    }
    TraceLog(L"exec launcher thread candidate tid=%u source=%s", tid, source);
    EnsureExecutorThreadHook(tid);
}

void EnsureExecutorWindow() {
    if (g_disabled.load(std::memory_order_relaxed)) {
        return;
    }
    if (RetireSupersededWindow(L"window")) {

        return;
    }
    if (g_execWindow.load(std::memory_order_relaxed)) {
        return;
    }

    const DWORD committed = g_launcherThread.load(std::memory_order_acquire);
    if (committed != 0 && committed != GetCurrentThreadId()) {
        TraceLog(L"exec window refused tid=%u launcher=%u reason=wrong-thread",
                GetCurrentThreadId(), committed);
        return;
    }

    ExecAdmissionShared admission;
    if (!admission.held()) {
        return;
    }
    if (g_disabled.load(std::memory_order_relaxed) ||
        g_execWindow.load(std::memory_order_relaxed)) {
        return;
    }

    if (!g_execClassRegistered) {
        WNDCLASSEXW windowClass{};
        windowClass.cbSize = sizeof(windowClass);
        windowClass.lpfnWndProc = ExecutorWindowProc;
        windowClass.hInstance = reinterpret_cast<HINSTANCE>(&__ImageBase);
        windowClass.lpszClassName = kExecClassName;
        if (!RegisterClassExW(&windowClass)) {
            TraceLog(L"exec window class failed err=%u", GetLastError());
            return;
        }
        g_execClassRegistered = true;
        g_execClassInstance = windowClass.hInstance;
    }
    HWND window = CreateWindowExW(0, kExecClassName, L"", 0, 0, 0, 0,
                                  0, HWND_MESSAGE, nullptr,
                                  g_execClassInstance, nullptr);
    if (!window) {
        TraceLog(L"exec window create failed err=%u", GetLastError());
        return;
    }

    g_execThreadId = GetCurrentThreadId();
    g_execWindow.store(window, std::memory_order_release);

    g_execRetirePosted.store(nullptr, std::memory_order_release);

    g_execDispatchClosed.store(false, std::memory_order_release);

    PublishExecutorCapability(L"window");

    const DWORD launcherThread = g_launcherThread.load(std::memory_order_relaxed);
    if (launcherThread != 0 && launcherThread != g_execThreadId) {
        TraceLog(L"exec window thread mismatch owner=%u launcher=%u",
                g_execThreadId, launcherThread);
    }
    TraceLog(L"exec window hwnd=%p tid=%u class=%s", window, g_execThreadId,
            kExecClassName);
}

LRESULT CALLBACK ExecutorThreadHook(int code, WPARAM wParam, LPARAM lParam) {
    g_execHookFrames.fetch_add(1, std::memory_order_acq_rel);
    HHOOK serviced = nullptr;
    DWORD target = 0;
    AcquireSRWLockShared(&g_execThreadHookLock);
    serviced = g_execThreadHook.load(std::memory_order_acquire);
    target = g_execThreadHookTarget.load(std::memory_order_acquire);
    ReleaseSRWLockShared(&g_execThreadHookLock);
    if (code == HC_ACTION && serviced &&
        GetCurrentThreadId() == target) {
        const DWORD tid = GetCurrentThreadId();
        DWORD known = g_launcherThread.load(std::memory_order_acquire);
        if (known == 0) {
            if (g_launcherThread.compare_exchange_strong(
                    known, tid, std::memory_order_acq_rel)) {

                TraceLog(L"exec launcher thread tid=%u source=show-pump "
                        L"provisional=1",
                        tid);
                known = tid;
            }
        }
        if (known == tid) {

            EnsureExecutorWindow();
        } else {

            TraceLog(L"exec thread pump refused known=%u tid=%u", known, tid);
        }

        RemoveExecutorThreadHookIfCurrent(serviced);
    }

    const LRESULT result = CallNextHookEx(nullptr, code, wParam, lParam);
    if (g_execHookFrames.load(std::memory_order_acquire) == 1 &&
        g_execHookIdle) {
        SetEvent(g_execHookIdle);
    }
    g_execHookFrames.fetch_sub(1, std::memory_order_acq_rel);
    return result;
}

bool CreateExecutorWindowOnUiThread(DWORD timeoutMs) {
    if (g_execWindow.load(std::memory_order_relaxed)) {
        return true;
    }
    if (g_disabled.load(std::memory_order_relaxed)) {
        return false;
    }
    const DWORD target = g_launcherThread.load(std::memory_order_relaxed);
    if (target == 0) {

        return false;
    }
    if (target == GetCurrentThreadId()) {
        EnsureExecutorWindow();
        return g_execWindow.load(std::memory_order_relaxed) != nullptr;
    }

    if (!EnsureExecutorThreadHook(target)) {
        return false;
    }
    const ULONGLONG start = GetTickCount64();
    while (!g_execWindow.load(std::memory_order_relaxed) &&
           !g_disabled.load(std::memory_order_relaxed) &&
           !(g_execStopEvent &&
             WaitForSingleObject(g_execStopEvent, 0) == WAIT_OBJECT_0) &&
           GetTickCount64() - start < timeoutMs) {

        Sleep(5);
    }
    const bool ready = g_execWindow.load(std::memory_order_relaxed) != nullptr;
    if (!ready) {
        TraceLog(L"exec window deferred: launcher thread %u did not pump in "
                L"%llu ms",
                target, static_cast<unsigned long long>(timeoutMs));

    }
    return ready;
}

DWORD WINAPI ExecutorWorkerProc(LPVOID) {

    struct ReadyOnExit {
        ~ReadyOnExit() {
            g_execReady.store(false, std::memory_order_release);
            PublishExecutorCapability(L"worker-exit");
        }
    } readyOnExit;

    CreateExecutorWindowOnUiThread(2000);

    if (!g_disabled.load(std::memory_order_relaxed)) {
        g_execReady.store(true, std::memory_order_release);
        PublishExecutorCapability(L"worker");
        TraceLog(L"exec worker ready tid=%u window=%p", GetCurrentThreadId(),
                g_execWindow.load(std::memory_order_relaxed));
    }

    HANDLE events[3] = {};
    DWORD waitCount = 0;
    events[waitCount++] = g_execStopEvent;
    if (g_ipc.execEvent) {
        events[waitCount++] = g_ipc.execEvent;
    }
    if (g_ipc.intentAckEvent) {
        events[waitCount++] = g_ipc.intentAckEvent;
    }
    for (;;) {

        DWORD waitMs = INFINITE;
        if (!g_explorerSymbolAttempted.load(std::memory_order_acquire) &&
            !g_explorerSymbolsReady.load(std::memory_order_acquire) &&
            !g_explorerSymbolsRefused.load(std::memory_order_acquire)) {
            waitMs = static_cast<DWORD>(kExecSymbolProbeMs);
        }
        const DWORD wait = WaitForMultipleObjects(waitCount, events, FALSE, waitMs);
        if (wait == WAIT_OBJECT_0) {
            return 0;
        }
        if (wait == WAIT_FAILED) {
            TraceLog(L"exec worker wait failed err=%u", GetLastError());
            return 0;
        }
        if (wait == WAIT_TIMEOUT) {
            ProbeExplorerSymbolsFromWorker();
            continue;
        }
        if (wait < WAIT_OBJECT_0 || wait >= WAIT_OBJECT_0 + waitCount) {
            TraceLog(L"exec worker wait ended unexpectedly result=%u", wait);
            return 0;
        }
        const HANDLE signalled = events[wait - WAIT_OBJECT_0];
        if (signalled == g_ipc.intentAckEvent) {
            HWND window = g_execWindow.load(std::memory_order_relaxed);
            if (window &&
                !PostMessageW(window, kExecIntentAckMessage,
                              kExecIntentAckMagic, 0)) {
                TraceLog(L"exec ack post failed err=%u", GetLastError());
            }
            continue;
        }
        if (signalled != g_ipc.execEvent) {
            TraceLog(L"exec worker wait ended unexpectedly result=%u", wait);
            return 0;
        }
        HWND window = g_execWindow.load(std::memory_order_relaxed);
        for (int i = 0; i < 50 && !window; ++i) {

            Sleep(1);
            window = g_execWindow.load(std::memory_order_relaxed);
        }
        if (!window) {
            TraceLog(L"exec command dropped: no window");
            CreateExecutorWindowOnUiThread(500);
            window = g_execWindow.load(std::memory_order_relaxed);
            if (!window) {
                continue;
            }
        }
        if (!PostMessageW(window, kExecMessage, 0, 0)) {
            TraceLog(L"exec command post failed err=%u", GetLastError());
        }
    }
}

void StartExecutorWorker() {
    if (g_execWorker || !g_ipc.execEvent) {
        return;
    }
    g_execStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_execStopEvent) {
        TraceLog(L"exec worker stop event failed err=%u", GetLastError());
        return;
    }
    g_execWorker = CreateThread(nullptr, 0, ExecutorWorkerProc, nullptr, 0,
                                nullptr);
    if (!g_execWorker) {
        TraceLog(L"exec worker thread failed err=%u", GetLastError());
        CloseHandle(g_execStopEvent);
        g_execStopEvent = nullptr;
        return;
    }
    g_execWorkerId = GetThreadId(g_execWorker);
    TraceLog(L"exec worker started tid=%u", g_execWorkerId);
}

[[noreturn]] void BlockBridgeUnload(const wchar_t* reason);

void JoinExecutorWorker() {
    if (!g_execWorker) {
        return;
    }
    if (WaitForSingleObject(g_execWorker, INFINITE) == WAIT_OBJECT_0) {
        CloseHandle(g_execWorker);
        g_execWorker = nullptr;
        TraceLog(L"exec worker joined tid=%u", g_execWorkerId);
        g_execWorkerId = 0;
        return;
    }
    BlockBridgeUnload(L"executor worker join failed");
}

[[noreturn]] void BlockBridgeUnload(const wchar_t* reason) {
    TraceLog(L"unload blocked reason=%s", reason);
    Wh_Log(L"Unload blocked: %s; the module image cannot be freed safely",
           reason);
    Sleep(INFINITE);
    __builtin_unreachable();
}

constexpr DWORD kHookFrameWaitMs = 200;

void WaitForExecutorHookFrames() {

    if (g_execHookFrames.load(std::memory_order_acquire) != 0 &&
        g_execThreadId == GetCurrentThreadId()) {
        BlockBridgeUnload(L"own hook frame");
    }
    if (g_execHookFrames.load(std::memory_order_acquire) == 0) {
        return;
    }
    if (!g_execHookIdle) {
        BlockBridgeUnload(L"executor hook frame event missing");
    }
    while (g_execHookFrames.load(std::memory_order_acquire) != 0) {
        const DWORD wait = WaitForSingleObject(g_execHookIdle, kHookFrameWaitMs);
        if (wait != WAIT_OBJECT_0 && wait != WAIT_TIMEOUT) {
            BlockBridgeUnload(L"executor hook frame wait failed");
        }
    }
    TraceLog(L"unload executor hook frames drained");
}

void DrainExecutorThread(HWND window) {
    WaitForExecutorHookFrames();
    if (!window) {

        if (g_execHookFrames.load(std::memory_order_acquire) != 0) {
            BlockBridgeUnload(L"executor hook frames without a window");
        }
        TraceLog(L"unload exec drain skipped reason=no-window");
        return;
    }
    for (int attempt = 0; attempt < kExecDrainRetries; ++attempt) {
        if (!IsWindow(window)) {
            if (g_execHookFrames.load(std::memory_order_acquire) != 0) {
                BlockBridgeUnload(L"executor hook frames without a window");
            }
            TraceLog(L"unload exec drain skipped reason=window-gone");
            return;
        }
        g_execDrainServed.store(false, std::memory_order_release);
        SendMessageW(window, kExecDrainMessage, kExecDrainMagic, 0);
        if (!g_execDrainServed.load(std::memory_order_acquire)) {
            TraceLog(L"unload exec drain not served hwnd=%p", window);
            return;
        }
        if (g_execHookFrames.load(std::memory_order_acquire) == 0) {
            TraceLog(L"unload exec drain served hwnd=%p attempt=%d", window,
                    attempt + 1);
            return;
        }
        WaitForExecutorHookFrames();
    }
    BlockBridgeUnload(L"executor hook frames after drain");
}

void DestroyExecutorWindow() {
    HWND window = g_execWindow.load(std::memory_order_acquire);
    if (!window) {
        return;
    }
    if (g_execThreadId == GetCurrentThreadId()) {
        if (g_execOwnerDepth > 0) {
            BlockBridgeUnload(L"executor teardown while a module frame is active");
        }
        StopExecutorTeardown(window);
        DestroyWindow(window);
    } else {
        if (!g_execTeardownAck) {
            BlockBridgeUnload(L"executor teardown event missing");
        }
        ResetEvent(g_execTeardownAck);
        TraceLog(L"exec window teardown send hwnd=%p tid=%u", window,
                static_cast<unsigned>(g_execThreadId));
        TraceLog(L"exec window teardown wait hwnd=%p tid=%u", window,
                static_cast<unsigned>(g_execThreadId));
        const LRESULT sent =
            SendMessageW(window, kExecTeardownMessage, kExecTeardownMagic, 0);
        if (!IsWindow(window)) {

            TraceLog(L"exec window gone before teardown hwnd=%p sent=%lld", window,
                    static_cast<long long>(sent));
            if (g_execWindow.load(std::memory_order_acquire) == window) {
                g_execWindow.store(nullptr, std::memory_order_release);
            }
        } else if (WaitForSingleObject(g_execTeardownAck, INFINITE) !=
                   WAIT_OBJECT_0) {
            BlockBridgeUnload(L"executor teardown acknowledgement failed");
        }
        bool destroyed = false;
        for (int attempt = 0; attempt < kExecDestroyRetries; ++attempt) {
            if (!IsWindow(window)) {
                break;
            }
            g_execDestroyServed.store(false, std::memory_order_release);
            SendMessageW(window, kExecDestroyMessage, kExecDestroyMagic, 0);
            if (g_execDestroyServed.load(std::memory_order_acquire)) {
                destroyed = true;
                break;
            }
            Sleep(1);
        }
        if (!destroyed && IsWindow(window)) {
            BlockBridgeUnload(L"executor destroy not served");
        }
    }
    if (g_execWindow.load(std::memory_order_acquire) == window) {
        if (!IsWindow(window)) {

            g_execWindow.store(nullptr, std::memory_order_release);
            TraceLog(L"exec window gone without teardown hwnd=%p", window);
        } else {
            BlockBridgeUnload(L"executor window still published");
        }
    }
    TraceLog(L"exec window destroyed tid=%u",
            static_cast<unsigned>(g_execThreadId));
}

void UnregisterExecutorClass() {
    if (!g_execClassRegistered) {
        return;
    }
    if (UnregisterClassW(kExecClassName, g_execClassInstance)) {
        TraceLog(L"exec window class unregistered");
    } else {
        const DWORD error = GetLastError();
        if (error == ERROR_CLASS_DOES_NOT_EXIST) {
            TraceLog(L"exec window class already gone");
        } else {

            BlockBridgeUnload(L"executor class unregister failed");
        }
    }
    g_execClassRegistered = false;
    g_execClassInstance = nullptr;
}

LONG64 PublishBridgeIntent(int32_t kind, int32_t trigger, uint32_t method,
                           uint32_t flags) {
    if (!g_ipc.state) {
        return 0;
    }
    if (!g_ipc.intentEvent) {

        // A record nobody can be woken for is not a publication: the caller
        // has to pass the press natively.
        TraceLog(L"intent refused kind=%d reason=no-event", kind);
        return 0;
    }
    if (kind == kSwipeIntentExternal) {

        DropPressLatch(L"external", L"intent");
        DropPressQueue(L"external");

        // A show the executor has not dispatched must not open the menu the
        // takeover is about to hand back to the shell.
        SuppressPublishedCommand(kSwipeExecShow, L"external");
    }
    uint64_t open = 0;
    uint64_t openTick = 0;
    SwipeIpcReadOpen(&g_ipc, &open, &openTick);

    const uint32_t publishedEpoch = PeerEpochNow();
    const LONG64 sequence =
        SwipeIpcPublishIntent(&g_ipc, kind, trigger, method, flags, open);
    TraceLog(L"intent kind=%s trigger=%s seq=%lld open=%llu method=%u flags=%u",
            kind == kSwipeIntentOpen
                ? L"open"
                : (kind == kSwipeIntentClose ? L"close" : L"external"),
            trigger == kSwipeTriggerWin
                ? L"win"
                : (trigger == kSwipeTriggerStartButton
                       ? L"startbutton"
                       : (trigger == kSwipeTriggerClickAway ? L"clickaway"
                                                            : L"override")),
            static_cast<long long>(sequence),
            static_cast<unsigned long long>(open), method, flags);
    if (kind == kSwipeIntentExternal && sequence > 0) {

        g_pressOutstanding = OutstandingPress{};
        g_pressOutstanding.pending = true;
        g_pressOutstanding.seq = sequence;
        g_pressOutstanding.publishedTick = GetTickCount64();
        g_pressOutstanding.external = true;
        g_pressOutstanding.publishedEpoch = publishedEpoch;
        HWND window = g_execWindow.load(std::memory_order_relaxed);
        if (window && SetTimer(window, kPressAckTimer,
                               static_cast<UINT>(kPressAckTimeoutMs),
                               nullptr) != 0) {
            TraceLog(L"own external barrier seq=%lld armed",
                    static_cast<long long>(sequence));
        } else {

            TraceLog(L"own external barrier seq=%lld unarmed err=%u",
                    static_cast<long long>(sequence), GetLastError());
        }
    }
    return sequence;
}

constexpr DWORD kShellOwnerWaitStepMs = 250;
constexpr ULONGLONG kShellOwnerWaitMs = 120000;

HANDLE g_shellElectThread = nullptr;
DWORD g_shellElectThreadId = 0;
HANDLE g_shellElectStop = nullptr;

// The bridge body: the events, the channel, the executor pid and capability
// word, and the executor worker. It runs once, in the explorer.exe that owns
// Shell_TrayWnd, on that process's election thread.
void InitialiseBridge() {
    Wh_Log(L"Swipe for 10Start %s initializing (explorer role)", kModVersion);

    g_pressQueueHead = 0;
    g_pressQueueCount = 0;
    g_pressOutstanding = OutstandingPress{};
    g_pressLatch = PressLatch{};
    g_suppressedExecSeq = 0;
    ClearPressOpenGeneration();

    g_execTeardownAck = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_execTeardownAck) {
        Wh_Log(L"Executor teardown event unavailable err=%u; the bridge stays "
               L"inert",
               static_cast<unsigned>(GetLastError()));
        return;
    }

    g_execHookIdle = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_execHookIdle) {
        Wh_Log(L"Executor hook event unavailable err=%u; the bridge stays inert",
               static_cast<unsigned>(GetLastError()));
        CloseHandle(g_execTeardownAck);
        g_execTeardownAck = nullptr;
        return;
    }

    bool created = false;
    DWORD ipcError = 0;
    if (!SwipeIpcOpen(&g_ipc, &created, &ipcError)) {
        Wh_Log(L"Swipe IPC channel unavailable (%u); the shell's own path "
               L"serves every trigger",
               static_cast<unsigned>(ipcError));
    } else {
        TraceLog(L"ipc open created=%d", created ? 1 : 0);

        // The section outlives an Explorer crash, so the word can still be 1
        // from the dead bridge. Clear it before the new pid becomes visible:
        // a reader that paired the old word with the new identity would
        // publish work before the worker exists. The worker publishes 1.
        PublishExecutorCapability(L"init");
        SwipeIpcSetExecPid(&g_ipc, GetCurrentProcessId());
    }

    g_execOwnerDepth = 0;
    g_execTeardownPending = false;
    g_execRetirePending = false;
    g_execDispatchClosed.store(false, std::memory_order_release);

    if (g_ipc.state) {
        StartExecutorWorker();
    }

    g_initialised.store(true, std::memory_order_release);
}

// Waits for the shell's taskbar window in bounded steps, then runs the bridge
// body once in its owner. A second explorer.exe owns no Shell_TrayWnd, so it
// never opens the channel; its thread ends at the deadline or at unload.
DWORD WINAPI ShellOwnerElectionProc(LPVOID) {
    TraceLog(L"bridge election start step=%ums deadline=%llums",
            static_cast<unsigned>(kShellOwnerWaitStepMs),
            static_cast<unsigned long long>(kShellOwnerWaitMs));

    const ULONGLONG started = GetTickCount64();
    const ULONGLONG deadline = started + kShellOwnerWaitMs;
    HWND ownerTray = nullptr;
    for (;;) {
        if (WaitForSingleObject(g_shellElectStop, 0) == WAIT_OBJECT_0) {
            const ULONGLONG waited = GetTickCount64() - started;
            TraceLog(L"bridge inert: election stopped after %llums before a "
                     L"Shell_TrayWnd owned by this explorer.exe appeared; the "
                     L"shell's path serves every trigger",
                     static_cast<unsigned long long>(waited));
            return 0;
        }
        const HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
        if (tray) {
            DWORD pid = 0;
            GetWindowThreadProcessId(tray, &pid);
            if (pid == GetCurrentProcessId()) {
                ownerTray = tray;
                break;
            }
        }
        const ULONGLONG now = GetTickCount64();
        if (now >= deadline) {
            TraceLog(L"bridge inert: this explorer.exe owns no Shell_TrayWnd "
                     L"after %llums; the shell's path serves every trigger",
                     static_cast<unsigned long long>(kShellOwnerWaitMs));
            return 0;
        }
        const ULONGLONG left = deadline - now;
        const DWORD step = static_cast<DWORD>(
            left < kShellOwnerWaitStepMs ? left : kShellOwnerWaitStepMs);
        WaitForSingleObject(g_shellElectStop, step);
    }
    if (WaitForSingleObject(g_shellElectStop, 0) == WAIT_OBJECT_0) {
        TraceLog(L"bridge election stopped before init");
        return 0;
    }

    TraceLog(L"bridge elected tid=%u hwnd=%p", GetCurrentThreadId(), ownerTray);
    InitialiseBridge();
    g_afterInit.store(true, std::memory_order_relaxed);
    return 0;
}

// The trace is the only work safe on the host's init thread: the bridge waits
// in RoleAfterInit for the explorer.exe that owns Shell_TrayWnd, so a restarted
// shell can still elect a new owner. Every callback keys on g_initialised.
BOOL RoleInit() {
    SetTraceFileName(L"bridge.log");
    RefreshTraceSetting();
    return TRUE;
}

void RoleAfterInit() {
    if (g_shellElectThread) {
        return;
    }
    g_shellElectStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_shellElectStop) {
        TraceLog(L"bridge election event failed err=%u", GetLastError());
        return;
    }
    g_shellElectThread = CreateThread(nullptr, 0, ShellOwnerElectionProc,
                                      nullptr, 0, &g_shellElectThreadId);
    if (!g_shellElectThread) {
        TraceLog(L"bridge election thread failed err=%u", GetLastError());
        CloseHandle(g_shellElectStop);
        g_shellElectStop = nullptr;
    }
}

// Signals a pending election and proves it gone before the bridge teardown.
// An election inside the init body is allowed to finish; the join then hands
// its g_initialised write to this thread.
void StopShellOwnerElection() {
    if (!g_shellElectThread) {
        if (g_shellElectStop) {
            CloseHandle(g_shellElectStop);
            g_shellElectStop = nullptr;
        }
        return;
    }
    if (g_shellElectStop) {
        SetEvent(g_shellElectStop);
    }
    if (g_shellElectThreadId == GetCurrentThreadId()) {
        BlockBridgeUnload(L"bridge election join on its own thread");
    }
    if (WaitForSingleObject(g_shellElectThread, INFINITE) != WAIT_OBJECT_0) {
        BlockBridgeUnload(L"bridge election join failed");
    }
    CloseHandle(g_shellElectThread);
    g_shellElectThread = nullptr;
    g_shellElectThreadId = 0;
    if (g_shellElectStop) {
        CloseHandle(g_shellElectStop);
        g_shellElectStop = nullptr;
    }
    TraceLog(L"bridge election joined elected=%d",
            g_initialised.load(std::memory_order_acquire) ? 1 : 0);
}

void RoleBeforeUninit() {
    StopShellOwnerElection();
    if (!g_initialised.load(std::memory_order_acquire)) {
        return;
    }
    g_unloadStartTick = GetTickCount64();
    TraceLog(L"unload begin tid=%u", GetCurrentThreadId());

    AcquireSRWLockExclusive(&g_explorerSymbolLock);
    g_disabled = true;
    ReleaseSRWLockExclusive(&g_explorerSymbolLock);

    g_execReady.store(false, std::memory_order_release);
    PublishExecutorCapability(L"unload");
    if (g_execStopEvent) {
        SetEvent(g_execStopEvent);
    }

    JoinExecutorWorker();

    RemoveExecutorThreadHook();
    WaitForExecutorHookFrames();
}

void RoleUninit() {
    StopShellOwnerElection();
    if (!g_initialised.load(std::memory_order_acquire)) {
        return;
    }

    g_disabled = true;

    g_execReady.store(false, std::memory_order_release);
    if (g_execStopEvent) {
        SetEvent(g_execStopEvent);
    }
    JoinExecutorWorker();

    if (!RemoveExecutorThreadHook()) {
        BlockBridgeUnload(L"executor thread hook removal failed");
    }
    DrainExecutorThread(g_execWindow.load(std::memory_order_acquire));

    // Every hook frame has returned, so a zero count means no later SetEvent.
    HANDLE hookIdle = g_execHookIdle;
    g_execHookIdle = nullptr;
    if (hookIdle) {
        CloseHandle(hookIdle);
    }

    {
        ExecAdmissionExclusive admission;
        TraceLog(L"exec unload admission exclusive");
        DestroyExecutorWindow();

        UnregisterExecutorClass();
    }

    if (g_explorerModuleRef) {
        FreeLibrary(g_explorerModuleRef);
        g_explorerModuleRef = nullptr;
        TraceLog(L"exec launcher module released");
    }

    PublishExecutorCapability(L"uninit");

    if (g_execStopEvent) {
        CloseHandle(g_execStopEvent);
        g_execStopEvent = nullptr;
    }
    if (g_execTeardownAck) {
        CloseHandle(g_execTeardownAck);
        g_execTeardownAck = nullptr;
    }

    ReleasePeerProcess();

    if (g_ipc.state) {
        SwipeIpcClose(&g_ipc);
        TraceLog(L"ipc close");
    }

    TraceLog(L"unload done elapsed=%llums",
            static_cast<unsigned long long>(
                g_unloadStartTick != 0 && GetTickCount64() >= g_unloadStartTick
                    ? GetTickCount64() - g_unloadStartTick
                    : 0));
    g_initialised.store(false, std::memory_order_release);
    Wh_Log(L"Swipe for 10Start uninitialized (explorer role)");
}

void RoleSettingsChanged() {
    // The trace setting is independent of the role's state: a change can arrive
    // while the election is still waiting for the shell.
    RefreshTraceSetting();
    if (!g_initialised.load(std::memory_order_acquire)) {
        return;
    }
}

const SwipeModRole Role = {RoleInit, RoleAfterInit, RoleBeforeUninit,
                           RoleUninit, RoleSettingsChanged};

}

namespace {

const SwipeModRole* ResolveModRole() {
    wchar_t path[MAX_PATH] = {};
    const DWORD length = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (length == 0 || length >= ARRAYSIZE(path)) {
        return nullptr;
    }

    const wchar_t* name = path;
    for (const wchar_t* cursor = path; *cursor; cursor++) {
        if (*cursor == L'\\' || *cursor == L'/') {
            name = cursor + 1;
        }
    }

    if (CompareStringOrdinal(name, -1, L"explorer.exe", -1, TRUE) ==
        CSTR_EQUAL) {
        return &bridge::Role;
    }
    if (CompareStringOrdinal(name, -1, L"StartMenuExperienceHost.exe", -1,
                             TRUE) == CSTR_EQUAL) {
        return &start::Role;
    }
    return nullptr;
}

const SwipeModRole* g_modRole = nullptr;

}

bool LegacyStartMenuEnabled() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    LSTATUS status = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
        L"Start_ShowClassicMode", RRF_RT_REG_DWORD, nullptr, &value, &size);
    if (status == ERROR_SUCCESS) {
        Wh_Log(L"Start_ShowClassicMode=%u source=advanced", value);
        return value == 1;
    }
    size = sizeof(value);
    status = RegGetValueW(HKEY_CURRENT_USER, L"Software\\ExplorerPatcher",
                          L"Start_ShowClassicMode", RRF_RT_REG_DWORD, nullptr,
                          &value, &size);
    if (status == ERROR_SUCCESS) {
        Wh_Log(L"Start_ShowClassicMode=%u source=explorerpatcher", value);
        return value == 1;
    }
    return false;
}

BOOL Wh_ModInit() {

    g_modRole = ResolveModRole();
    if (!g_modRole) {
        return FALSE;
    }
    if (!LegacyStartMenuEnabled()) {
        Wh_Log(L"ExplorerPatcher Start_ShowClassicMode is not 1; Swipe for "
               L"10Start stays disabled");
        return FALSE;
    }
    return g_modRole->modInit();
}

void Wh_ModAfterInit() {
    if (g_modRole) {
        g_modRole->modAfterInit();
    }
}

void Wh_ModBeforeUninit() {
    if (g_modRole) {
        g_modRole->modBeforeUninit();
    }
}

void Wh_ModUninit() {

    if (g_modRole) {
        g_modRole->modUninit();
    }
}

void Wh_ModSettingsChanged() {
    if (g_modRole) {
        g_modRole->modSettingsChanged();
    }
}

// ==WindhawkMod==
// @id              taskbar-fluentflyout-reserve
// @name            Taskbar Reserve Space for FluentFlyout
// @name:ja-JP      FluentFlyout用タスクバー予約領域
// @description     Reserves space on the Windows 11 taskbar for the FluentFlyout taskbar widget, on the primary monitor only by default
// @description:ja-JP FluentFlyoutのタスクバーウィジェット用に、Windows 11タスクバーへ空き領域を確保する。既定ではプライマリーモニターのみ
// @version         0.8.1
// @author          Ren9618
// @github          https://github.com/Ren9618
// @twitter         https://x.com/Ray_9618_2
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -luser32
// @license         GPL-3.0
// ==/WindhawkMod==

// ---------------------------------------------------------------------------
// LICENSE NOTE (read before reusing or submitting this file)
//
// This file adapts code from the "Multirow taskbar for Windows 11" mod
// (taskbar-multirow v1.1.4, by m417z, from ramensoftware/windhawk-mods):
//   - XamlRootFromTaskbarHostSharedPtr()
//   - GetTaskbarXamlRoot() / GetSecondaryTaskbarXamlRoot()
//   - RunFromWindowThread()
//   - HookTaskbarDllSymbols() (reduced to the symbols listed below)
// (FindCurrentProcessTaskbarWnd() was also adapted in v0.4-v0.7
// and removed in v0.8, when the lifecycle stopped needing it.)
//
// v0.8 additionally adapts code from the "Taskbar Margin" mod
// (taskbar-margin v1.0, by loliri, from ramensoftware/windhawk-mods,
// also GPL v3.0):
//   - OnWindowCreated() and the CreateWindowExW /
//     CreateWindowInBand / CreateWindowInBandEx hooks
//   - ForEachTaskbarWindow() (thread-deduplicated enumeration)
//   - the per-thread DispatcherQueueTimer retry
//     (StartRetryTimer / StopRetryTimer / RetryTimerTick)
// The timer placement follows taskbar-margin: thread_local,
// one timer per taskbar thread, created, used and stopped on
// that thread (stopped from Wh_ModBeforeUninit via a dispatch
// to the taskbar thread), so it never outlives the unload.
// The taskbar-multirow source states: "Source code is published under
// The GNU General Public License v3.0." Because this file is a
// derivative of that GPL code, it is NOT MIT-licensed (unlike the
// v0.1-v0.3 files of this project, which contain no borrowed code).
// The @license above is therefore GPL-3.0. If a future version is to
// be published under MIT, the adapted functions must first be
// rewritten clean-room; only the approach and the symbol names
// themselves are facts, the copied/adapted code text is GPL.
//
// Deviations from taskbar-multirow, made deliberately for this
// project (see impl-notes-v04-2026-11-05.md for the reasoning):
//   1. FAIL-SAFE BAIL: if the TaskbarHost::FrameHeight prologue
//      pattern does not match, multirow logs and proceeds with a
//      default offset of 0x10. This file instead logs and returns
//      nullptr (no XamlRoot), per this project's design rule
//      "never guess an offset" and the PR #5175 reviewer guidance.
//      (Note: on the author's Windows build 26300.9818 the pattern
//      DID match in the v0.4.1 test and decoded offset 0x10 from the
//      prologue bytes - that is a decoded value, not the guessed
//      fallback.)
//   2. A null check for the TaskbarHost element IUnknown pointer was
//      added before QueryInterface (multirow dereferences it
//      unchecked).
//   3. Only the 6 address-only symbols needed for XamlRoot
//      acquisition are resolved from taskbar.dll. multirow
//      additionally hooks functions in taskbar.dll and
//      Taskbar.View.dll; none of that is needed here, and the
//      resolved GetTaskbarHost addresses are *called*, not
//      hooked. (v0.8 does install function hooks, but only the
//      user32 window-creation hooks adapted from taskbar-margin -
//      still no taskbar.dll function hooks.)
//
// STATUS: v0.8.1. The mod hooks taskbar window creation
// (CreateWindowExW / CreateWindowInBand / CreateWindowInBandEx,
// adapted from taskbar-margin) and retries on a per-thread
// DispatcherQueueTimer on the taskbar thread itself until the
// XAML tree is ready, so taskbars created after the mod loads
// (Explorer restart, monitor reconnect / dock) are covered and
// no worker thread can outlive the DLL at unload. The XamlRoot
// acquisition path, the margin writing / restoring logic and
// the `specific` monitor matching were confirmed working on
// the author's real machine (Windows 11 Pro 26H2, build
// 26300.9818, three monitors), including the v0.8.1 checks:
// an empty specificMonitorDevice matches nothing (and logs
// so), and the reservation is restored after an Explorer
// restart. The ARM64
// prologue branch (with the same fail-safe bail) is kept so
// the file compiles for ARM64 in the catalog compatibility
// check; ARM64 runtime behavior is untested on real ARM64
// hardware.
// ---------------------------------------------------------------------------

// ==WindhawkModReadme==
/*
# Taskbar Reserve Space for FluentFlyout

Reserves space on the right side of the Windows 11 taskbar app list
for the [FluentFlyout](https://github.com/unchihugo/FluentFlyout)
taskbar widget, so the widget does not overlap app buttons.

## How it works

For every taskbar in the target scope, this mod finds the
`Microsoft.UI.Xaml.Controls.ItemsRepeater` named
`TaskbarFrameRepeater` and adds `reservedWidth` to the **right** of
its original `Margin`:

- The element's margin at first application is saved per element
  as the original value. Every (re)application recomputes the
  target from that saved original, so the reservation never
  accumulates (350 -> 700 -> 1050 cannot happen).
- If the current margin already equals the target, the write is
  skipped.
- When the mod is disabled / unloaded, the saved original margins
  are restored.

Writes happen when the mod loads, when its settings change,
and when a taskbar window is created, always on the taskbar's
UI thread. This mod deliberately never writes from layout
callbacks (`LayoutUpdated` etc.): writing layout-affecting
properties from inside a layout pass is what causes the
"Layout cycle detected" Explorer crash (0x802b0014).

A newly created taskbar's XAML tree is not built yet at the
moment the window appears - which is also the normal case
right after Explorer (re)starts, because the mod is loaded
before the taskbar UI exists. The application is therefore
retried on a timer on the taskbar thread itself (every
500 ms, up to 30 seconds), so the reservation appears on its
own a few seconds after the taskbar is up. Because taskbar
creation is hooked, this also covers taskbars that appear
while Explorer keeps running, for example when a monitor is
reconnected or a laptop is docked.

## Relation to Taskbar Margin

[Taskbar Margin](https://windhawk.net/mods/taskbar-margin)
also leaves empty space on the taskbar, and its suggested use
mentions FluentFlyout - but for the widget placed at the
**bottom left**: it shifts the whole taskbar content to the
right, leaving a margin on the left side, with an
all / primary / secondary display scope.

This mod is for the widget's usual place at the **right end**
of the taskbar, next to the system tray, where it overlaps the
right end of the app list: it widens the right margin of the
app list itself (`TaskbarFrameRepeater`), and it can pin the
reservation to one specific monitor. Use Taskbar Margin for
left-side space; use this mod for the right side.

## Settings

- `reservedWidth` (default 350): width to reserve, in XAML logical
  units. 0 removes the reservation (restores the original margin).
- `monitorScope`: which taskbars to apply to.
  - `primary` (default): only the taskbar on the primary monitor.
    The primary monitor is detected per taskbar window with
    `MonitorFromWindow` + `GetMonitorInfo`, not by resolution or
    position.
  - `all`: every taskbar.
  - `specific`: only the taskbar on the monitor named by
    `specificMonitorDevice`. The target stays pinned to that
    monitor even if you change which monitor is the primary one -
    that is the whole point of this mode. For example, you can
    keep the reservation on your second monitor regardless of
    the primary setting.
- `specificMonitorDevice` (default `\\.\DISPLAY2`): the monitor
  device name to pin to when `monitorScope` is `specific`. The
  comparison is case-insensitive. If the field is cleared, the
  `specific` scope matches no monitor (a line is logged about
  it), so nothing is reserved until a name is set. **How to
  find your monitor's
  name:** enable this mod (any scope) and look at the Windhawk
  log - every taskbar is logged with a line like
  `device=\\.\DISPLAY2`, one per taskbar/monitor. Copy the device
  name shown for the monitor you want into this setting.
  Known limitation: DISPLAY numbers are assigned by Windows and
  can change, for example when you move a cable to a different
  port or change the monitor connection order. If the pinned
  monitor stops matching after such a change, check the log for
  the new device name and update this setting. (Matching by a
  stable monitor device-path ID instead is a future task.)

## Known limitations

- Taskbars created while the mod runs are picked up
  automatically, and after an Explorer restart the reservation
  is restored on its own within a few seconds. What is not
  tracked is a change of *which* monitor is primary (or a
  display rearrangement) that does not recreate the taskbars:
  the scope decision is only re-evaluated when an application
  runs. Toggle the mod or change any setting to re-apply after
  such a change. With `specific`, the pinned target never moves
  when the primary monitor changes - that is the point of the
  mode.
- Horizontal taskbars only; no special handling for vertical
  taskbars.
- Do not let another mod write the same element's Margin at
  the same time. The Multirow taskbar mod writes
  `TaskbarFrameRepeater.Margin.Right` too, so do not enable it
  together with this mod. Likewise, if you used the
  FluentFlyout rules in Windows 11 Taskbar Styler (the fixed
  350 margin, the `FFTaskbarWidth` capture, or the conditional
  margin on `TaskbarFrameRepeater`), remove those rules from
  Styler before using this mod, otherwise the two will
  overwrite each other and the saved original margins go
  stale.

## If something goes wrong

The margin values this mod sets are runtime XAML properties;
nothing is written to disk or to the registry.

- Disable this mod in Windhawk: on unload it tries to restore the
  saved original margins first.
- If the taskbar layout still looks wrong, restart Explorer
  (e.g. Task Manager -> Windows Explorer -> Restart). All runtime
  XAML changes disappear with the process, so this always returns
  the taskbar to its stock layout.
---

# （日本語）FluentFlyout用タスクバー予約領域

Windows 11 のタスクバーにあるアプリ一覧の右側に空き領域を確保し、
[FluentFlyout](https://github.com/unchihugo/FluentFlyout) のタスクバー
ウィジェットがアプリボタンと重ならないようにする Mod です。

## 仕組み

対象範囲の各タスクバーについて、`TaskbarFrameRepeater` という名前の
`Microsoft.UI.Xaml.Controls.ItemsRepeater` を探し、その元の `Margin` の
**右側** に `reservedWidth` を加えます。

- 初回適用時の Margin を要素ごとに「元の値」として保存します。再適用の
  たびに保存した元の値から目標値を計算し直すため、予約幅が累積する
  （350 → 700 → 1050 のようになる）ことはありません。
- 現在の Margin が目標値と一致していれば、書き込みを省略します。
- Mod を無効化／アンロードすると、保存した元の Margin に復元します。

書き込みは、Mod の読み込み時、設定変更時、そしてタスクバーの
ウィンドウが生成されたときに行い、常にタスクバーの UI スレッドで
実行します。新しく作られたタスクバーの XAML ツリーは、ウィンドウが
現れた時点ではまだ構築されていません（Explorer 再起動直後に Mod が
読み込まれる場合も同じです）。そのため、タスクバーのスレッド上の
タイマーで 500 ミリ秒ごとに最大 30 秒再試行し、準備ができ次第、
数秒以内に予約が付きます。タスクバーの生成をフックしているため、
Explorer が動作している間にモニターの再接続やドッキングで現れた
タスクバーにも自動で適用されます。
レイアウト処理中のコールバック（`LayoutUpdated` など）から
は決して書き込みません。レイアウトパスの中でレイアウトに影響する
プロパティを変更すると、「Layout cycle detected」による Explorer の
クラッシュ（0x802b0014）の原因になるためです。

## Taskbar Margin との関係

Taskbar Margin もタスクバーに空き領域を作る Mod で、その推奨用途には
FluentFlyout が挙がっていますが、あちらはウィジェットを**左下**に置く
前提です。タスクバーの内容全体を右へずらして左側に空きを作り、対象は
全モニター／プライマリーのみ／セカンダリーのみから選びます。

この Mod は、ウィジェットの通常位置であるタスクバーの**右端**（システム
トレイの横）用です。アプリ一覧の右端にウィジェットが重なるため、アプリ
一覧そのもの（TaskbarFrameRepeater）の右 Margin を広げます。特定の
1台のモニターへの固定もできます。左側に空きが欲しい場合は Taskbar
Margin を、この Mod は右側用です。

## 設定

- `reservedWidth`（既定値 350）: 確保する幅（XAML 論理単位）。
  0 にすると予約なし（元の Margin に復元）になります。
- `monitorScope`: どのタスクバーに適用するか。
  - `primary`（既定）: プライマリーモニターのタスクバーのみ。
    プライマリー判定は解像度や位置ではなく、タスクバーのウィンドウ
    ごとに `MonitorFromWindow` + `GetMonitorInfo` で行います。
  - `all`: すべてのタスクバー。
  - `specific`: `specificMonitorDevice` で指定したモニターの
    タスクバーのみ。プライマリーモニターを変更しても適用先はその
    モニターに固定されたまま動きません。それがこのモードの目的です。
    たとえば、プライマリー設定に関係なく2台目のモニターに予約を
    維持できます。
- `specificMonitorDevice`（既定値 `\\.\DISPLAY2`）:
  `monitorScope` が `specific` のときに固定するモニターのデバイス名。
  大文字小文字は区別しません。欄を空にすると、`specific` はどの
  モニターにも一致せず（その旨がログに残ります）、名前を設定する
  まで予約は付きません。**自分のモニターのデバイス名の調べ方:**
  この Mod を有効化する（スコープはどれでも可）と、Windhawk のログに
  タスクバーごとに `device=\\.\DISPLAY2` のような行が出力されます。
  対象にしたいモニターの行に表示されたデバイス名を、この設定に
  コピーしてください。
  既知の制限: DISPLAY 番号は Windows が割り当てるため、ケーブルを
  別のポートに挿し替えるなど接続順が変わると変わることがあります。
  固定したモニターが一致しなくなった場合は、ログで新しいデバイス名を
  確認してこの設定を更新してください。（安定したモニターのデバイス
  パス ID で照合する方法は将来課題です。）

## 既知の制限

- 動作中に生成されたタスクバーには自動で適用され、Explorer の
  再起動後も予約は数秒以内に自動で復旧します。追従できないのは、
  タスクバーが再生成されない形でのプライマリーモニターの変更や
  画面配置の変更で、その場合はスコープ判定が次の適用まで古いまま
  です。ディスプレイ構成を変えたら、Mod のトグルか設定変更で
  再適用してください。`specific` では固定先はプライマリーの変更で
  動きません（それがこのモードの目的です）。
- 横向きタスクバーのみ対応。縦置きタスクバーの特別処理はありません。
- 同じ要素の Margin を別の Mod と同時に書き込まないでください。
  Multirow taskbar も `TaskbarFrameRepeater.Margin.Right` を
  書き込むため、この Mod と同時に有効化しないでください。また、
  Windows 11 Taskbar Styler で FluentFlyout 用のルール
  （350固定Margin、`FFTaskbarWidth` キャプチャ、`TaskbarFrameRepeater`
  への条件付き Margin）を使っていた場合は、この Mod を使う前に
  Styler 側のルールを削除してください。残っていると互いに上書き
  し合い、保存した元の Margin が古くなります。

## 問題が起きたら

この Mod が設定する Margin は実行時の XAML プロパティであり、
ディスクやレジストリには何も書き込みません。

- Windhawk でこの Mod を無効化してください。アンロード時に保存した
  元の Margin への復元を試みます。
- それでもタスクバーのレイアウトがおかしい場合は、Explorer を
  再起動してください（タスクマネージャー → Windows エクスプローラー
  → 再起動）。実行時の XAML 変更はプロセスと一緒に消えるため、
  必ず標準のレイアウトに戻ります。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- reservedWidth: 350
  $name: Reserved width
  $name:ja-JP: 予約幅
  $description: Width reserved on the right side of the taskbar app list, in XAML logical units. 0 means no reservation. 350 was verified with FluentFlyout by the author.
  $description:ja-JP: アプリ一覧の右側に確保する幅（XAML論理単位）。0で予約なし。350は作者がFluentFlyoutで検証した値。
- monitorScope: primary
  $name: Target monitors
  $name:ja-JP: 適用対象モニター
  $options:
    - primary: Primary monitor only
    - all: All monitors
    - specific: Specific monitor only (see specificMonitorDevice)
  $options:ja-JP:
    - primary: プライマリーモニターのみ
    - all: すべてのモニター
    - specific: 特定のモニターのみ（specificMonitorDeviceで指定）
- specificMonitorDevice: '\\.\DISPLAY2'
  $name: Specific monitor device name
  $name:ja-JP: 特定モニターのデバイス名
  $description: Only used when monitorScope is "specific". The taskbar on the monitor with this device name (as shown in the mod log as device=...) is the target. Case-insensitive. Note that DISPLAY numbers can change when monitor cables/connections change.
  $description:ja-JP: monitorScopeが「specific」のときだけ使われます。このデバイス名のモニター（Modのログに device=... と出る名前）のタスクバーが対象になります。大文字小文字は区別しません。DISPLAY番号はケーブル接続先の変更などで変わることがあります。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <vector>

// Fix (from v0.4.1): winbase.h (pulled in via windows.h) defines a
// GetCurrentTime macro, which breaks the winrt header
// Windows.UI.Xaml.Media.Animation (it declares a virtual
// GetCurrentTime method). Undefine the macro before including
// any winrt headers. This is the same workaround used by other
// Windhawk mods that include Windows.UI.Xaml headers.
#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

// ---------------------------------------------------------------------------
// Settings (reservedWidth/monitorScope as in v0.3-v0.5; the
// "specific" scope and specificMonitorDevice are new in v0.6)
// ---------------------------------------------------------------------------

enum class MonitorScope {
    Primary,
    All,
    Specific,
};

struct {
    int reservedWidth;
    MonitorScope monitorScope = MonitorScope::Primary;
    // Only used when monitorScope == MonitorScope::Specific: the
    // GetMonitorInfo device name (e.g. L"\\.\DISPLAY2") of the one
    // monitor whose taskbar this mod applies to, pinned regardless
    // of which monitor is primary. Sized generously; Windows device
    // names are short (MONITORINFOEX::szDevice is 32 chars), but a
    // user can paste something longer into the setting by mistake.
    wchar_t specificMonitorDevice[64] = {};
} g_settings;

PCWSTR MonitorScopeName() {
    switch (g_settings.monitorScope) {
        case MonitorScope::All:
            return L"all";
        case MonitorScope::Specific:
            return L"specific";
        default:
            return L"primary";
    }
}

void LoadSettings() {
    g_settings.reservedWidth = Wh_GetIntSetting(L"reservedWidth");
    if (g_settings.reservedWidth < 0) {
        g_settings.reservedWidth = 0;
    }
    if (g_settings.reservedWidth > 2000) {
        g_settings.reservedWidth = 2000;
    }

    WindhawkUtils::StringSetting scope =
        WindhawkUtils::StringSetting::make(L"monitorScope");
    if (wcscmp(scope, L"all") == 0) {
        g_settings.monitorScope = MonitorScope::All;
    } else if (wcscmp(scope, L"specific") == 0) {
        g_settings.monitorScope = MonitorScope::Specific;
    } else {
        g_settings.monitorScope = MonitorScope::Primary;
    }

    WindhawkUtils::StringSetting device =
        WindhawkUtils::StringSetting::make(L"specificMonitorDevice");
    wcsncpy_s(g_settings.specificMonitorDevice, device, _TRUNCATE);
    if (g_settings.monitorScope == MonitorScope::Specific &&
        g_settings.specificMonitorDevice[0] == L'\0') {
        // An empty name is not a typo for the default device:
        // it simply matches no monitor, so nothing is reserved.
        Wh_Log(L"specificMonitorDevice is empty: the 'specific' "
               L"scope matches no monitor, nothing is reserved");
    }
}

// ---------------------------------------------------------------------------
// Monitor logging (same as v0.3/v0.4). Returns true if the window is
// on the primary monitor.
// ---------------------------------------------------------------------------

bool LogMonitorForWindow(HWND hwnd, PCWSTR label) {
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
    if (!monitor) {
        Wh_Log(L"%s: hwnd=%p, MonitorFromWindow returned NULL", label, hwnd);
        return false;
    }

    MONITORINFOEX monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfo(monitor, &monitorInfo)) {
        Wh_Log(L"%s: hwnd=%p, GetMonitorInfo failed", label, hwnd);
        return false;
    }

    bool isPrimary = (monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;
    Wh_Log(L"%s: hwnd=%p, device=%s, monitor=%ld,%ld - %ld,%ld, primary=%s",
           label, hwnd, monitorInfo.szDevice,
           monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top,
           monitorInfo.rcMonitor.right, monitorInfo.rcMonitor.bottom,
           isPrimary ? L"yes" : L"no");
    return isPrimary;
}

// New in v0.6: resolves the monitor device name (GetMonitorInfo's
// szDevice, e.g. "\\.\DISPLAY2") for a window, for the "specific"
// scope comparison. Returns false (and leaves deviceName empty) if
// the monitor cannot be resolved. The device= logging itself stays
// in LogMonitorForWindow above; this helper does not log, so the
// caller can log the comparison result on one line.
bool GetMonitorDeviceForWindow(HWND hwnd, PWSTR deviceName,
                               size_t deviceNameChars) {
    deviceName[0] = L'\0';

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
    if (!monitor) {
        return false;
    }

    MONITORINFOEX monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfo(monitor, &monitorInfo)) {
        return false;
    }

    wcsncpy_s(deviceName, deviceNameChars, monitorInfo.szDevice,
              _TRUNCATE);
    return true;
}

// Decides whether the taskbar window `hwnd` is in the current
// scope. New in v0.6 for the "specific" branch; the "primary" and
// "all" branches are the v0.5 behavior (LogMonitorForWindow's
// return value for primary, unconditional for all).
bool IsTaskbarInScope(HWND hwnd, bool isPrimaryMonitor) {
    switch (g_settings.monitorScope) {
        case MonitorScope::All:
            return true;

        case MonitorScope::Specific: {
            wchar_t deviceName[64] = {};
            if (!GetMonitorDeviceForWindow(hwnd, deviceName,
                                           ARRAYSIZE(deviceName))) {
                Wh_Log(L"scope=specific: could not resolve the "
                       L"monitor device for hwnd=%p, not applying "
                       L"(expected device=%s)",
                       hwnd, g_settings.specificMonitorDevice);
                return false;
            }

            // Case-insensitive comparison against the configured
            // device name.
            if (_wcsicmp(deviceName,
                         g_settings.specificMonitorDevice) == 0) {
                Wh_Log(L"scope=specific: device=%s matches "
                       L"expected=%s -> in scope",
                       deviceName, g_settings.specificMonitorDevice);
                return true;
            }

            Wh_Log(L"scope=specific: device mismatch, "
                   L"device=%s, expected=%s -> not in scope",
                   deviceName, g_settings.specificMonitorDevice);
            return false;
        }

        default:  // MonitorScope::Primary
            return isPrimaryMonitor;
    }
}

// ---------------------------------------------------------------------------
// XamlRoot acquisition, adapted from taskbar-multirow v1.1.4
// (GPL v3.0, see the license note at the top of this file).
// Verified working on a real machine in v0.4.1 - unchanged here.
//
// The six symbol addresses below are resolved from taskbar.dll with
// WindhawkUtils::HookSymbols in HookTaskbarDllSymbols(). None of
// them is hooked; the two GetTaskbarHost addresses are called
// directly, the two vftable addresses are only compared against,
// TaskbarHost::FrameHeight is only used as bytes to decode the
// TaskbarHost layout offset from, and _Ref_count_base::_Decref is
// called to release the shared_ptr control block.
// ---------------------------------------------------------------------------

void* CTaskBand_ITaskListWndSite_vftable;

void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    // The offset of the taskbar element's IUnknown inside TaskbarHost
    // is decoded from the TaskbarHost::FrameHeight prologue, exactly
    // as in taskbar-multirow:
    //   48:83EC 28 | sub rsp,28
    //   48:83C1 48 | add rcx,48     <- the added byte is the offset
    size_t taskbarElementIUnknownOffset;

#if defined(_M_X64)
    {
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b && b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC &&
            b[4] == 0x48 && b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            // DEVIATION from taskbar-multirow (fail-safe): multirow
            // logs "Unsupported TaskbarHost::FrameHeight" here and
            // proceeds with a default offset of 0x10. Guessing the
            // offset can make the QueryInterface below dereference
            // an arbitrary pointer, so this mod bails out instead.
            // The shared_ptr still has to be released.
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight prologue, "
                   L"bailing out (no offset guessed)");
            if (taskbarHostSharedPtr[1]) {
                std__Ref_count_base__Decref_Original(
                    taskbarHostSharedPtr[1]);
            }
            return nullptr;
        }
    }
#elif defined(_M_ARM64)
    {
        // ARM64 branch, also adapted from taskbar-multirow v1.1.4
        // (added so this file compiles for ARM64, which the official
        // catalog's compatibility check builds for explorer.exe
        // targets; not tested on real ARM64 hardware):
        //   7f2303d5 pacibsp
        //   fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        //   fd030091 mov     fp, sp
        //   080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p && p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        } else {
            // Same fail-safe deviation as the x86-64 branch above:
            // bail out instead of proceeding with a default offset.
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight prologue, "
                   L"bailing out (no offset guessed)");
            if (taskbarHostSharedPtr[1]) {
                std__Ref_count_base__Decref_Original(
                    taskbarHostSharedPtr[1]);
            }
            return nullptr;
        }
    }
#else
#error "Unsupported architecture"
#endif

    Wh_Log(L"TaskbarHost element IUnknown offset: 0x%IX",
           taskbarElementIUnknownOffset);

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    if (taskbarElementIUnknown) {
        taskbarElementIUnknown->QueryInterface(
            winrt::guid_of<FrameworkElement>(),
            winrt::put_abi(taskbarElement));
    } else {
        Wh_Log(L"TaskbarHost element IUnknown is null");
    }

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    HWND hTaskSwWnd =
        (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                               taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

// ---------------------------------------------------------------------------
// RunFromWindowThread, copied from taskbar-multirow v1.1.4
// (GPL v3.0). Runs proc on the thread that owns hWnd, synchronously.
// All margin reads/writes in this mod go through this, so they
// always execute on the taskbar's UI thread - never from a layout
// callback (see the layout-cycle note in the readme).
// ---------------------------------------------------------------------------

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

// ---------------------------------------------------------------------------
// Visual tree search (this project's own code, unchanged from
// v0.4.1 where it was verified on a real machine).
//
// Recursively walks the tree under `element` with VisualTreeHelper,
// looking for a FrameworkElement whose class name is
// "Microsoft.UI.Xaml.Controls.ItemsRepeater" and whose Name is
// "TaskbarFrameRepeater" - the identification the design spec asks
// for. Elements that match the Name but not the class are remembered
// too, so the log shows what the class name actually is on this
// Windows build if it ever differs.
// ---------------------------------------------------------------------------

struct RepeaterSearch {
    FrameworkElement found = nullptr;
    FrameworkElement nameOnlyMatch = nullptr;
    winrt::hstring nameOnlyMatchClass;
    int visited = 0;
};

void SearchRepeaterRecursive(FrameworkElement element,
                             RepeaterSearch* search,
                             int depth) {
    if (!element || search->found || depth > 64) {
        return;
    }

    search->visited++;

    if (element.Name() == L"TaskbarFrameRepeater") {
        auto className = winrt::get_class_name(element);
        if (className == L"Microsoft.UI.Xaml.Controls.ItemsRepeater") {
            search->found = element;
            return;
        }
        if (!search->nameOnlyMatch) {
            search->nameOnlyMatch = element;
            search->nameOnlyMatchClass = className;
        }
    }

    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount && !search->found; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (child) {
            SearchRepeaterRecursive(child, search, depth + 1);
        }
    }
}

// ---------------------------------------------------------------------------
// Tracked repeaters: the per-element saved original margins (new in
// v0.5, this project's own code).
//
// For every repeater this mod has touched, the margin it had at
// first application is stored as its original value. All later
// applications recompute the target from this stored original, so
// the reservation can never accumulate across re-applications.
//
// Elements are referenced through winrt::weak_ref, never through a
// raw pointer or a strong reference:
// - a destroyed element's weak_ref simply stops resolving, and its
//   entry is pruned instead of being reused - so if a taskbar is
//   recreated and a NEW element happens to land on the same
//   address, the old element's saved margin is never applied to it
//   by mistake (the design spec explicitly forbids reusing saved
//   values across element generations);
// - holding no strong reference means this mod never keeps a dead
//   taskbar's element alive.
//
// The list is a plain global vector. Its elements hold only a
// weak_ref and a Thickness: a weak_ref's destructor is safe to
// run from any thread (see the "Global objects and process
// shutdown" wiki page, section 4), and every access happens on
// the taskbar UI thread anyway (apply and restore both go
// through RunFromWindowThread, and unload dispatches the
// restore to that thread before returning).
// ---------------------------------------------------------------------------

struct TrackedRepeater {
    winrt::weak_ref<FrameworkElement> weak;
    Thickness originalMargin{};
};

std::vector<TrackedRepeater> g_trackedRepeaters;

// Drops entries whose element no longer exists. Must be called on
// the taskbar UI thread.
void PruneStaleTrackedRepeaters() {
    auto& tracked = g_trackedRepeaters;
    for (size_t i = 0; i < tracked.size();) {
        if (tracked[i].weak.get()) {
            i++;
        } else {
            Wh_Log(L"Tracked repeater #%u: element is gone, dropping "
                   L"its saved original margin",
                   (unsigned)(i + 1));
            tracked.erase(tracked.begin() + i);
        }
    }
}

// Finds the entry for `element`, or creates one that saves
// `currentMargin` as the element's original margin. Returns the
// entry index (indices stay valid because the vector is only
// appended to here). Must be called on the taskbar UI thread.
int FindOrAddTrackedRepeater(FrameworkElement const& element,
                             Thickness currentMargin,
                             bool* added) {
    auto& tracked = g_trackedRepeaters;
    for (size_t i = 0; i < tracked.size(); i++) {
        auto resolved = tracked[i].weak.get();
        if (resolved && resolved == element) {
            *added = false;
            return (int)i;
        }
    }

    TrackedRepeater entry;
    entry.weak = winrt::make_weak(element);
    entry.originalMargin = currentMargin;
    tracked.push_back(entry);
    *added = true;
    return (int)tracked.size() - 1;
}

bool ThicknessEquals(Thickness const& a, Thickness const& b) {
    return a.Left == b.Left && a.Top == b.Top && a.Right == b.Right &&
           a.Bottom == b.Bottom;
}

void LogMargin(PCWSTR label, PCWSTR what, Thickness const& margin) {
    Wh_Log(L"%s: %s: Left=%f, Top=%f, Right=%f, Bottom=%f", label, what,
           margin.Left, margin.Top, margin.Right, margin.Bottom);
}

// ---------------------------------------------------------------------------
// Application. Runs on the taskbar window thread (via
// RunFromWindowThread), because GetTaskbarXamlRoot and all XAML
// access must happen there - this mirrors how taskbar-multirow
// enumerates the current thread's windows in
// ApplySettingsFromTaskbarThread().
//
// This is the only place in the mod that writes the Margin
// property. It is entered from the lifecycle entry points and
// from the window-creation hooks, always dispatched to the
// taskbar UI thread - never from a layout event.
// ---------------------------------------------------------------------------

// Readiness aggregation for one application pass (new in v0.7,
// kept in v0.8). The timer retry uses this to decide
// whether the taskbar UI was fully up: right after Explorer
// (re)starts, the mod is loaded before the taskbar's XAML tree
// exists, so the one-shot application of v0.6.1 found no (or
// only some) XamlRoots / repeaters and silently stayed
// unapplied.
struct ApplyStats {
    int taskbarCount = 0;
    int xamlRootCount = 0;
    int inScopeApplied = 0;
    int inScopeTotal = 0;
};

// Ready means: at least one taskbar window exists, a XamlRoot
// was obtained for EVERY taskbar window, and the repeater was
// found (and its margin written, or already equal to the
// target) on EVERY in-scope taskbar. Out-of-scope taskbars do
// not need a repeater. A thread with no taskbar window yields
// all-zero stats, never ready.
bool IsReady(const ApplyStats& stats) {
    return stats.taskbarCount >= 1 &&
           stats.xamlRootCount >= stats.taskbarCount &&
           stats.inScopeApplied >= stats.inScopeTotal;
}

// One inspected XamlRoot and whether its repeater was found.
// Kept per pass so that if the same root is returned for more
// than one taskbar window, the second window can still count
// the repeater found via the first one.
struct InspectedRoot {
    void* rootAbi;
    bool repeaterFound;
};

struct ApplyState {
    int taskbarCount = 0;
    int xamlRootCount = 0;
    int repeaterCount = 0;
    int inScopeTotal = 0;
    int inScopeApplied = 0;
    int marginsWritten = 0;
    int marginsRestored = 0;
    int writesSkipped = 0;
    std::vector<InspectedRoot> inspectedRoots;
};

void ApplyOneRoot(XamlRoot xamlRoot,
                  PCWSTR taskbarLabel,
                  bool inScope,
                  ApplyState* state) {
    // The same XamlRoot could in principle be returned for more
    // than one window; inspect each root only once. The result
    // of the first inspection is remembered, so a later window
    // sharing this root still counts the repeater found via the
    // first one (for the v0.7 readiness aggregation).
    void* rootAbi = winrt::get_abi(xamlRoot);
    for (auto& inspected : state->inspectedRoots) {
        if (inspected.rootAbi == rootAbi) {
            Wh_Log(L"%s: XamlRoot already inspected, skipping", taskbarLabel);
            if (inScope && inspected.repeaterFound) {
                state->inScopeApplied++;
            }
            return;
        }
    }
    state->inspectedRoots.push_back({rootAbi, false});
    size_t inspectedIndex = state->inspectedRoots.size() - 1;
    state->xamlRootCount++;

    Wh_Log(L"%s: XamlRoot #%d obtained", taskbarLabel, state->xamlRootCount);

    try {
        auto content = xamlRoot.Content().try_as<FrameworkElement>();
        if (!content) {
            Wh_Log(L"%s: XamlRoot content is not a FrameworkElement",
                   taskbarLabel);
            return;
        }

        RepeaterSearch search;
        SearchRepeaterRecursive(content, &search, 0);
        Wh_Log(L"%s: visual tree elements visited: %d", taskbarLabel,
               search.visited);

        if (!search.found) {
            if (search.nameOnlyMatch) {
                Wh_Log(L"%s: an element named TaskbarFrameRepeater was "
                       L"found but its class is '%s', not "
                       L"Microsoft.UI.Xaml.Controls.ItemsRepeater",
                       taskbarLabel, search.nameOnlyMatchClass.c_str());
            } else {
                Wh_Log(L"%s: TaskbarFrameRepeater NOT found in this "
                       L"XamlRoot, nothing to apply",
                       taskbarLabel);
            }
            return;
        }
        state->inspectedRoots[inspectedIndex].repeaterFound = true;
        state->repeaterCount++;
        if (inScope) {
            // Found and handled below: the margin is either
            // written now or already equals the target - both
            // count as "applied" for the v0.7 readiness check.
            state->inScopeApplied++;
        }

        FrameworkElement repeater = search.found;
        Thickness currentMargin = repeater.Margin();
        LogMargin(taskbarLabel, L"current Margin", currentMargin);

        // Save this element's original margin on first sight, and
        // always recompute from the saved original afterwards.
        bool added = false;
        int trackedIndex =
            FindOrAddTrackedRepeater(repeater, currentMargin, &added);
        Thickness originalMargin =
            g_trackedRepeaters[trackedIndex].originalMargin;
        if (added) {
            LogMargin(taskbarLabel,
                      L"saved as this element's original Margin",
                      originalMargin);
        }

        if (inScope) {
            Thickness targetMargin = originalMargin;
            targetMargin.Right =
                originalMargin.Right + g_settings.reservedWidth;

            if (ThicknessEquals(currentMargin, targetMargin)) {
                Wh_Log(L"%s: Margin already equals the target, "
                       L"write skipped",
                       taskbarLabel);
                state->writesSkipped++;
                return;
            }

            LogMargin(taskbarLabel, L"Margin before write", currentMargin);
            LogMargin(taskbarLabel, L"Margin target", targetMargin);
            repeater.Margin(targetMargin);
            state->marginsWritten++;

            // Read back and verify what actually took effect.
            Thickness afterMargin = repeater.Margin();
            LogMargin(taskbarLabel, L"Margin after write", afterMargin);
            if (!ThicknessEquals(afterMargin, targetMargin)) {
                Wh_Log(L"%s: WARNING: Margin after write does not "
                       L"equal the target (something else may be "
                       L"writing this Margin too)",
                       taskbarLabel);
            }
        } else {
            // Not a target under the current scope (e.g. monitorScope
            // was changed from "all" to "primary", or "specific" was
            // re-pinned to another monitor): put the element back to
            // its saved original margin.
            if (ThicknessEquals(currentMargin, originalMargin)) {
                Wh_Log(L"%s: not in scope and Margin already equals "
                       L"the original, write skipped",
                       taskbarLabel);
                state->writesSkipped++;
                return;
            }

            LogMargin(taskbarLabel, L"Margin before restore",
                      currentMargin);
            LogMargin(taskbarLabel, L"Margin restore target",
                      originalMargin);
            repeater.Margin(originalMargin);
            state->marginsRestored++;

            Thickness afterMargin = repeater.Margin();
            LogMargin(taskbarLabel, L"Margin after restore", afterMargin);
            if (!ThicknessEquals(afterMargin, originalMargin)) {
                Wh_Log(L"%s: WARNING: Margin after restore does not "
                       L"equal the original",
                       taskbarLabel);
            }
        }
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"%s: XAML access failed: %s", taskbarLabel,
               e.message().c_str());
    } catch (...) {
        Wh_Log(L"%s: XAML access failed with an unknown exception",
               taskbarLabel);
    }
}

ApplyStats ApplyFromTaskbarThread() {
    PruneStaleTrackedRepeaters();

    ApplyState state;

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto* state = reinterpret_cast<ApplyState*>(lParam);

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) ==
                0) {
                return TRUE;
            }

            bool isPrimaryTaskbar =
                _wcsicmp(szClassName, L"Shell_TrayWnd") == 0;
            bool isSecondaryTaskbar =
                _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
            if (!isPrimaryTaskbar && !isSecondaryTaskbar) {
                return TRUE;
            }

            state->taskbarCount++;
            Wh_Log(L"taskbar #%d: class=%s", state->taskbarCount,
                   szClassName);

            bool isPrimaryMonitor = LogMonitorForWindow(hWnd, L"taskbar");
            bool inScope = IsTaskbarInScope(hWnd, isPrimaryMonitor);
            if (inScope) {
                state->inScopeTotal++;
            }
            Wh_Log(L"taskbar #%d: scope=%s -> inScope=%s "
                   L"(reservedWidth=%d)",
                   state->taskbarCount, MonitorScopeName(),
                   inScope ? L"yes" : L"no", g_settings.reservedWidth);

            XamlRoot xamlRoot = isPrimaryTaskbar
                                    ? GetTaskbarXamlRoot(hWnd)
                                    : GetSecondaryTaskbarXamlRoot(hWnd);
            if (!xamlRoot) {
                Wh_Log(L"taskbar #%d: GetTaskbarXamlRoot failed "
                       L"(see the log lines above for the reason)",
                       state->taskbarCount);
                return TRUE;
            }

            WCHAR label[64];
            swprintf_s(label, L"taskbar #%d", state->taskbarCount);
            ApplyOneRoot(xamlRoot, label, inScope, state);

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&state));

    Wh_Log(L"SUMMARY: taskbars=%d, XamlRoots=%d, repeatersFound=%d, "
           L"marginsWritten=%d, marginsRestored=%d, writesSkipped=%d, "
           L"reservedWidth=%d, monitorScope=%s",
           state.taskbarCount, state.xamlRootCount, state.repeaterCount,
           state.marginsWritten, state.marginsRestored, state.writesSkipped,
           g_settings.reservedWidth, MonitorScopeName());

    ApplyStats stats;
    stats.taskbarCount = state.taskbarCount;
    stats.xamlRootCount = state.xamlRootCount;
    stats.inScopeApplied = state.inScopeApplied;
    stats.inScopeTotal = state.inScopeTotal;
    return stats;
}

// ---------------------------------------------------------------------------
// Window-creation hooks and timer retry.
//
// Why not a worker thread that retries until the taskbar is
// ready: a detached thread can still be running mod code when
// Windhawk unloads the DLL after Wh_ModUninit returns - a
// crash risk, plus races between the retry passes, the
// restore, and LoadSettings. And a load-time-only pass misses
// taskbars created later (monitor reconnect, docking, sleep
// resume).
//
// The design follows Taskbar Margin (taskbar-margin v1.0, GPL
// v3.0, see the license note at the top of this file): hook
// taskbar window creation, and retry on a DispatcherQueueTimer
// that lives on the taskbar thread itself. No worker thread
// exists anymore, so nothing can outlive the unload, and a
// taskbar that appears at any time is applied to.
//
// The timer is thread_local (one per taskbar thread, as in
// taskbar-margin), is only ever touched on that thread, and is
// stopped on that thread from Wh_ModBeforeUninit via
// RemoveSettings, so no timer outlives the unload.
// ---------------------------------------------------------------------------

std::atomic<bool> g_unloading{false};

thread_local winrt::Windows::System::DispatcherQueueTimer g_retryTimer{
    nullptr};
thread_local winrt::Windows::System::DispatcherQueueTimer::Tick_revoker
    g_retryTimerRevoker;
thread_local int g_retryAttempts;

constexpr int kMaxApplyAttempts = 60;
constexpr int kApplyRetryIntervalMs = 500;

void StopRetryTimer() {
    if (g_retryTimer) {
        try {
            g_retryTimer.Stop();
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"StopRetryTimer: error %08X: %s", ex.code(),
                   ex.message().c_str());
        }
    }

    g_retryTimerRevoker.revoke();
    g_retryTimer = nullptr;
    g_retryAttempts = 0;
}

void RetryTimerTick(
    winrt::Windows::System::DispatcherQueueTimer const&,
    winrt::Windows::Foundation::IInspectable const&) {
    if (g_unloading) {
        StopRetryTimer();
        return;
    }

    ApplyStats stats = ApplyFromTaskbarThread();
    if (IsReady(stats)) {
        Wh_Log(L"apply: ready on timer attempt %d", g_retryAttempts + 1);
        StopRetryTimer();
        return;
    }

    if (++g_retryAttempts >= kMaxApplyAttempts) {
        Wh_Log(L"apply: giving up after %d timer attempts, the "
               L"taskbar UI did not become ready in time (toggle "
               L"the mod or change any setting to retry)",
               g_retryAttempts);
        StopRetryTimer();
    }
}

void StartRetryTimer() {
    if (g_retryTimer) {
        // Already retrying on this thread. A new trigger (for
        // example, another taskbar was just created) gets the
        // full retry budget instead of whatever is left of the
        // current window.
        g_retryAttempts = 0;
        return;
    }

    try {
        auto dispatcherQueue =
            winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        if (!dispatcherQueue) {
            Wh_Log(L"StartRetryTimer: no dispatcher queue on this "
                   L"thread, cannot retry");
            return;
        }

        g_retryTimer = dispatcherQueue.CreateTimer();
        g_retryTimer.IsRepeating(true);
        g_retryTimer.Interval(
            std::chrono::milliseconds{kApplyRetryIntervalMs});
        g_retryTimerRevoker =
            g_retryTimer.Tick(winrt::auto_revoke, RetryTimerTick);
        g_retryTimer.Start();
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"StartRetryTimer: error %08X: %s", ex.code(),
               ex.message().c_str());
    }
}

// One application trigger on the taskbar thread: a single pass,
// and if the taskbar UI is not fully up yet, the retry timer
// keeps trying from this thread.
void ApplySettingsOnTaskbarThread() {
    // The creation hooks stay active until Wh_ModBeforeUninit
    // returns; a taskbar created in that window must not
    // re-apply the margin after the unload restore.
    if (g_unloading) {
        return;
    }

    ApplyStats stats = ApplyFromTaskbarThread();
    if (!IsReady(stats) && !g_unloading) {
        Wh_Log(L"apply: taskbar UI not ready yet (taskbars=%d, "
               L"roots=%d, inScopeApplied=%d/%d), retrying on a "
               L"timer",
               stats.taskbarCount, stats.xamlRootCount,
               stats.inScopeApplied, stats.inScopeTotal);
        StartRetryTimer();
    }
}

void ApplySettings(HWND hTaskbarWnd) {
    RunFromWindowThread(
        hTaskbarWnd,
        [](void* pParam) -> void { ApplySettingsOnTaskbarThread(); },
        nullptr);
}

void RestoreFromTaskbarThread();  // defined below

void RemoveSettingsFromTaskbarThread() {
    StopRetryTimer();
    RestoreFromTaskbarThread();
}

void RemoveSettings(HWND hTaskbarWnd) {
    RunFromWindowThread(
        hTaskbarWnd,
        [](void* pParam) -> void { RemoveSettingsFromTaskbarThread(); },
        nullptr);
}

// Every taskbar window of this process, visiting each taskbar
// thread only once (the apply pass already covers all the
// taskbars on the thread it runs on). Adapted from
// taskbar-margin.
void ForEachTaskbarWindow(void (*proc)(HWND)) {
    struct ENUM_PARAM {
        void (*proc)(HWND);
        std::vector<DWORD> visitedThreadIds;
    };

    ENUM_PARAM param{proc};

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto* param = reinterpret_cast<ENUM_PARAM*>(lParam);

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) ==
                0) {
                return TRUE;
            }

            if (_wcsicmp(szClassName, L"Shell_TrayWnd") != 0 &&
                _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") != 0) {
                return TRUE;
            }

            DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
            if (std::find(param->visitedThreadIds.begin(),
                          param->visitedThreadIds.end(),
                          dwThreadId) != param->visitedThreadIds.end()) {
                return TRUE;
            }

            param->visitedThreadIds.push_back(dwThreadId);
            param->proc(hWnd);

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&param));
}

// A taskbar window was just created in this process. The XAML
// tree does not exist yet at this point; ApplySettings runs the
// first pass on the new window's thread right away and the
// retry timer takes it from there. Adapted from taskbar-margin.
void OnWindowCreated(HWND hWnd, LPCWSTR lpClassName) {
    if (!lpClassName) {
        return;
    }

    // Class names can also be atoms; only compare textual ones.
    BOOL bTextualClassName =
        ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (!bTextualClassName) {
        return;
    }

    if (_wcsicmp(lpClassName, L"Shell_TrayWnd") == 0 ||
        _wcsicmp(lpClassName, L"Shell_SecondaryTrayWnd") == 0) {
        Wh_Log(L"Taskbar window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        ApplySettings(hWnd);
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName, DWORD dwStyle,
                                 int X, int Y, int nWidth, int nHeight,
                                 HWND hWndParent, HMENU hMenu,
                                 HINSTANCE hInstance, PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName,
                                         lpWindowName, dwStyle, X, Y,
                                         nWidth, nHeight, hWndParent,
                                         hMenu, hInstance, lpParam);
    if (!hWnd) {
        return hWnd;
    }

    OnWindowCreated(hWnd, lpClassName);

    return hWnd;
}

using CreateWindowInBand_t = HWND(WINAPI*)(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, PVOID lpParam,
    DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle, LPCWSTR lpClassName,
                                    LPCWSTR lpWindowName, DWORD dwStyle,
                                    int X, int Y, int nWidth, int nHeight,
                                    HWND hWndParent, HMENU hMenu,
                                    HINSTANCE hInstance, PVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth,
        nHeight, hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd) {
        return hWnd;
    }

    OnWindowCreated(hWnd, lpClassName);

    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, PVOID lpParam,
    DWORD dwBand, DWORD dwTypeFlags);
CreateWindowInBandEx_t CreateWindowInBandEx_Original;
HWND WINAPI CreateWindowInBandEx_Hook(DWORD dwExStyle,
                                      LPCWSTR lpClassName,
                                      LPCWSTR lpWindowName, DWORD dwStyle,
                                      int X, int Y, int nWidth, int nHeight,
                                      HWND hWndParent, HMENU hMenu,
                                      HINSTANCE hInstance, PVOID lpParam,
                                      DWORD dwBand, DWORD dwTypeFlags) {
    HWND hWnd = CreateWindowInBandEx_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth,
        nHeight, hWndParent, hMenu, hInstance, lpParam, dwBand,
        dwTypeFlags);
    if (!hWnd) {
        return hWnd;
    }

    OnWindowCreated(hWnd, lpClassName);

    return hWnd;
}

void HookWindowCreationFunctions() {
    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);

    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!user32Module) {
        return;
    }

    auto pCreateWindowInBand = (CreateWindowInBand_t)GetProcAddress(
        user32Module, "CreateWindowInBand");
    if (pCreateWindowInBand) {
        WindhawkUtils::SetFunctionHook(pCreateWindowInBand,
                                       CreateWindowInBand_Hook,
                                       &CreateWindowInBand_Original);
    }

    auto pCreateWindowInBandEx = (CreateWindowInBandEx_t)GetProcAddress(
        user32Module, "CreateWindowInBandEx");
    if (pCreateWindowInBandEx) {
        WindhawkUtils::SetFunctionHook(pCreateWindowInBandEx,
                                       CreateWindowInBandEx_Hook,
                                       &CreateWindowInBandEx_Original);
    }
}

// ---------------------------------------------------------------------------
// Restore. Runs on the taskbar window thread (via
// RunFromWindowThread), called from Wh_ModBeforeUninit so the saved
// original margins are put back before the mod's hooks are removed
// and before the module is unloaded.
// ---------------------------------------------------------------------------

void RestoreFromTaskbarThread() {
    PruneStaleTrackedRepeaters();

    auto& tracked = g_trackedRepeaters;
    int restoredCount = 0;
    int alreadyOriginalCount = 0;

    for (size_t i = 0; i < tracked.size(); i++) {
        auto element = tracked[i].weak.get();
        if (!element) {
            continue;
        }

        WCHAR label[64];
        swprintf_s(label, L"tracked repeater #%u", (unsigned)(i + 1));

        try {
            Thickness currentMargin = element.Margin();
            Thickness originalMargin = tracked[i].originalMargin;

            if (ThicknessEquals(currentMargin, originalMargin)) {
                Wh_Log(L"%s: Margin already equals the original, "
                       L"restore skipped",
                       label);
                alreadyOriginalCount++;
                continue;
            }

            LogMargin(label, L"Margin before restore", currentMargin);
            LogMargin(label, L"Margin restore target", originalMargin);
            element.Margin(originalMargin);
            restoredCount++;

            Thickness afterMargin = element.Margin();
            LogMargin(label, L"Margin after restore", afterMargin);
            if (!ThicknessEquals(afterMargin, originalMargin)) {
                Wh_Log(L"%s: WARNING: Margin after restore does not "
                       L"equal the original",
                       label);
            }
        } catch (winrt::hresult_error const& e) {
            Wh_Log(L"%s: restore failed: %s", label, e.message().c_str());
        } catch (...) {
            Wh_Log(L"%s: restore failed with an unknown exception", label);
        }
    }

    Wh_Log(L"RESTORE SUMMARY: tracked=%u, restored=%d, "
           L"alreadyOriginal=%d",
           (unsigned)tracked.size(), restoredCount, alreadyOriginalCount);

    tracked.clear();
}

// ---------------------------------------------------------------------------
// Symbol resolution. Adapted from HookTaskbarDllSymbols() in
// taskbar-multirow v1.1.4 (GPL v3.0), reduced to the 6 symbols that
// XamlRoot acquisition needs. WindhawkUtils::HookSymbols fails as a
// whole if any (non-optional) symbol cannot be resolved; in that
// case Wh_ModInit returns FALSE and the mod does nothing, instead
// of guessing addresses.
// ---------------------------------------------------------------------------

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

    if (!WindhawkUtils::HookSymbols(module, taskbarDllHooks,
                                    ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed for taskbar.dll, bailing out");
        return false;
    }

    return true;
}

// ---------------------------------------------------------------------------
// Mod lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    LoadSettings();

    Wh_Log(L"Taskbar Reserve Space for FluentFlyout v0.8.1 "
           L"(window-creation hooks + timer retry on the taskbar "
           L"thread)");
    Wh_Log(L"reservedWidth=%d, monitorScope=%s, "
           L"specificMonitorDevice=%s",
           g_settings.reservedWidth, MonitorScopeName(),
           g_settings.specificMonitorDevice);

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"taskbar.dll symbols could not be resolved, "
               L"the mod will do nothing");
        return FALSE;
    }

    HookWindowCreationFunctions();

    return TRUE;
}

void Wh_ModAfterInit() {
    // Taskbars that already exist (the usual case when the mod
    // is enabled while Explorer is running). Taskbars created
    // later are covered by the window-creation hooks.
    ForEachTaskbarWindow(ApplySettings);
}

void Wh_ModBeforeUninit() {
    // Stop the retry timers and restore the saved original
    // margins, on each taskbar thread. After this returns, no
    // code of this mod runs on any thread: there is no worker
    // thread, and the timers are stopped above.
    g_unloading = true;
    ForEachTaskbarWindow(RemoveSettings);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"Settings changed: reservedWidth=%d, monitorScope=%s, "
           L"specificMonitorDevice=%s",
           g_settings.reservedWidth, MonitorScopeName(),
           g_settings.specificMonitorDevice);
    ForEachTaskbarWindow(ApplySettings);
}

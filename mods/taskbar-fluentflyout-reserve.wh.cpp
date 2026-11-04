// ==WindhawkMod==
// @id              taskbar-fluentflyout-reserve
// @name            Taskbar Reserve Space for FluentFlyout
// @name:ja-JP      FluentFlyout用タスクバー予約領域
// @description     Reserves space on the Windows 11 taskbar for the FluentFlyout taskbar widget, on the primary monitor only by default
// @description:ja-JP FluentFlyoutのタスクバーウィジェット用に、Windows 11タスクバーへ空き領域を確保する。既定ではプライマリーモニターのみ
// @version         0.7
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
//   - FindCurrentProcessTaskbarWnd()
//   - HookTaskbarDllSymbols() (reduced to the symbols listed below)
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
//      acquisition are resolved. multirow additionally hooks
//      functions in taskbar.dll and Taskbar.View.dll; none of that
//      is needed here, and this file installs NO function hooks at
//      all - the resolved GetTaskbarHost addresses are *called*,
//      not hooked. (Note: multirow does not use a
//      TrayUI::StartTaskbar hook either; that is a different mods'
//      pattern.)
//
// STATUS: v0.7, which adds a "retry until the taskbar is ready"
// re-application loop on top of v0.6.1, fixing the reservation
// being lost after an Explorer restart (the one-shot application
// at load time ran before the taskbar's XAML tree existed and
// never retried). The XamlRoot acquisition path below is the one
// verified on a real machine with v0.4.1 (symbols resolved, 3
// XamlRoots obtained, 3 repeaters found, margins read, non-host
// explorer processes skipped), and the margin writing / restoring
// logic, including the `specific` monitor matching, is the one
// the user confirmed working on a real machine with v0.5-v0.6.1.
// The retry loop was confirmed working on the author's real
// machine with v0.7 (Windows 11 Pro 26H2, build 26300.9818,
// three monitors). The ARM64 prologue branch (from
// taskbar-multirow, with the same fail-safe bail) is included
// so the file compiles for ARM64; ARM64 runtime behavior is
// untested on real ARM64 hardware.
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
- After every write, the margin is read back and logged, so the
  Windhawk log shows before / after values for verification.
- When the mod is disabled / unloaded, the saved original margins
  are restored (and the restore is also read back and logged).

Writes happen only when the mod loads and when its settings
change, always on the taskbar's UI thread. This mod deliberately
never writes from layout callbacks (`LayoutUpdated` etc.):
writing layout-affecting properties from inside a layout pass is
what causes the "Layout cycle detected" Explorer crash
(0x802b0014).

If the taskbar UI is not ready yet when the mod loads - which
is the normal case right after Explorer (re)starts, because the
XAML tree is built only after the mod is already loaded - the
application is retried with increasing intervals for up to
about a minute, so the reservation comes back on its own within
a few seconds once the taskbar is up.

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
  comparison is case-insensitive. **How to find your monitor's
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

## Known limitations (v0.7)

- After Explorer restarts, the mod retries the application
  until the taskbar UI is ready (up to about a minute), so the
  reservation is restored automatically within a few seconds.
  If a taskbar is recreated while Explorer keeps running,
  however, the new taskbar is not picked up automatically yet;
  toggling the mod off and on (or changing any setting)
  re-applies it.
- Adding / removing a monitor while the mod is running is not
  tracked yet. Changing the primary monitor is only relevant to
  the `primary` scope; with `specific`, the pinned target does
  not move when the primary monitor changes. Toggle the mod or
  change any setting to re-apply after a display change.
- Horizontal taskbars only; no special handling for vertical
  taskbars.
- Do not let another mod write the same element's Margin at the
  same time. In particular, if you used the FluentFlyout rules in
  Windows 11 Taskbar Styler (the fixed 350 margin, the
  `FFTaskbarWidth` capture, or the conditional margin on
  `TaskbarFrameRepeater`), remove those rules from Styler before
  using this mod, otherwise the two mods will overwrite each
  other.

## If something goes wrong

The margin values this mod sets are runtime XAML properties;
nothing is written to disk or to the registry.

- Disable this mod in Windhawk: on unload it tries to restore the
  saved original margins first.
- If the taskbar layout still looks wrong, restart Explorer
  (e.g. Task Manager -> Windows Explorer -> Restart). All runtime
  XAML changes disappear with the process, so this always returns
  the taskbar to its stock layout.

## Status

Margin writing, restore on unload, the `specific` monitor
scope (device-name matching), and the retry-until-ready
re-application after an Explorer restart were all confirmed
working on the author's real machine (Windows 11 Pro 26H2,
build 26300.9818, three monitors).
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
- 書き込みのたびに Margin を読み戻してログに残すため、Windhawk の
  ログで変更前／変更後の値を確認できます。
- Mod を無効化／アンロードすると、保存した元の Margin に復元します
  （復元結果も読み戻してログに残します）。

書き込みは Mod の読み込み時と設定変更時のみ、タスクバーの UI スレッド
で行います。読み込み直後はタスクバーの XAML ツリーがまだ構築されていないことがある
（特に Explorer 再起動直後）ため、その場合は間隔を広げながら最大約1分間自動で再試行します。
レイアウト処理中のコールバック（`LayoutUpdated` など）から
は決して書き込みません。レイアウトパスの中でレイアウトに影響する
プロパティを変更すると、「Layout cycle detected」による Explorer の
クラッシュ（0x802b0014）の原因になるためです。

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
  大文字小文字は区別しません。**自分のモニターのデバイス名の調べ方:**
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

- Explorer の再起動後は、タスクバーの UI の準備ができるまで Mod が
  自動で再試行する（最大約1分）ため、予約は数秒以内に自動で復旧
  します。ただし、Explorer が動いたままタスクバーが再生成された
  場合は、新しいタスクバーへの自動追従は未実装です。Mod の
  無効化→有効化か、設定変更で再適用されます。
- 動作中のモニターの追加／取り外しには未対応です。プライマリー
  モニターの変更が関係するのは `primary` スコープのみで、`specific`
  では固定先は動きません。ディスプレイ構成を変えたら、Mod の
  トグルか設定変更で再適用してください。
- 横向きタスクバーのみ対応。縦置きタスクバーの特別処理はありません。
- 同じ要素の Margin を別の Mod と同時に書き込まないでください。
  特に、Windows 11 Taskbar Styler で FluentFlyout 用のルール
  （350固定Margin、`FFTaskbarWidth` キャプチャ、`TaskbarFrameRepeater`
  への条件付き Margin）を使っていた場合は、この Mod を使う前に
  Styler 側のルールを削除してください。残っていると互いに上書き
  し合います。

## 問題が起きたら

この Mod が設定する Margin は実行時の XAML プロパティであり、
ディスクやレジストリには何も書き込みません。

- Windhawk でこの Mod を無効化してください。アンロード時に保存した
  元の Margin への復元を試みます。
- それでもタスクバーのレイアウトがおかしい場合は、Explorer を
  再起動してください（タスクマネージャー → Windows エクスプローラー
  → 再起動）。実行時の XAML 変更はプロセスと一緒に消えるため、
  必ず標準のレイアウトに戻ります。

## 状態

Margin の書き込み、アンロード時の復元、`specific` スコープの照合、
Explorer 再起動後の再試行による再適用は、作者の実機
（Windows 11 Pro 26H2 / build 26300.9818、モニター3台）で動作を
確認しています。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- reservedWidth: 350
  $name: Reserved width (pixels)
  $name:ja-JP: 予約幅（ピクセル）
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

#include <atomic>
#include <thread>
#include <vector>

// Fix (from v0.4.1): winbase.h (pulled in via windows.h) defines a
// GetCurrentTime macro, which breaks the winrt header
// Windows.UI.Xaml.Media.Animation (it declares a virtual
// GetCurrentTime method). Undefine the macro before including
// any winrt headers. This is the same workaround used by other
// Windhawk mods that include Windows.UI.Xaml headers.
#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
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

    PCWSTR scope = Wh_GetStringSetting(L"monitorScope");
    if (scope && wcscmp(scope, L"all") == 0) {
        g_settings.monitorScope = MonitorScope::All;
    } else if (scope && wcscmp(scope, L"specific") == 0) {
        g_settings.monitorScope = MonitorScope::Specific;
    } else {
        g_settings.monitorScope = MonitorScope::Primary;
    }
    Wh_FreeStringSetting(scope);

    PCWSTR device = Wh_GetStringSetting(L"specificMonitorDevice");
    if (device && device[0]) {
        wcsncpy_s(g_settings.specificMonitorDevice, device,
                  _TRUNCATE);
    } else {
        wcscpy_s(g_settings.specificMonitorDevice, L"\\\\.\\DISPLAY2");
    }
    Wh_FreeStringSetting(device);
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
// FindCurrentProcessTaskbarWnd, copied from taskbar-multirow v1.1.4
// (GPL v3.0). This is also how this mod tells whether the current
// explorer process hosts the taskbar at all: folder-window explorer
// processes have no Shell_TrayWnd of their own, this returns
// nullptr there, and the mod then does nothing.
// ---------------------------------------------------------------------------

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
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
// The list lives in a heap-allocated vector that is intentionally
// never freed, so no XAML-related destructor runs at mod unload.
// All accesses happen on the taskbar UI thread (apply and restore
// both go through RunFromWindowThread), except the size check in
// Wh_ModBeforeUninit before dispatching.
// ---------------------------------------------------------------------------

struct TrackedRepeater {
    winrt::weak_ref<FrameworkElement> weak;
    Thickness originalMargin{};
};

std::vector<TrackedRepeater>& TrackedRepeaters() {
    static auto* tracked = new std::vector<TrackedRepeater>();
    return *tracked;
}

// Drops entries whose element no longer exists. Must be called on
// the taskbar UI thread.
void PruneStaleTrackedRepeaters() {
    auto& tracked = TrackedRepeaters();
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
    auto& tracked = TrackedRepeaters();
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
// property, and it is only ever entered via StartApplyWithRetry
// (called from Wh_ModAfterInit and Wh_ModSettingsChanged) -
// never from a layout event.
// ---------------------------------------------------------------------------

// Readiness aggregation for one application pass (new in v0.7).
// The retry loop in StartApplyWithRetry uses this to decide
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
// not need a repeater. The "no taskbar window at all" case is
// reported by RunApplyOnce as all-zero stats, never ready.
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
            TrackedRepeaters()[trackedIndex].originalMargin;
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
// Retry-until-ready application (new in v0.7).
//
// The bug this fixes: in v0.6.1 the application ran exactly
// once, from Wh_ModAfterInit / Wh_ModSettingsChanged. Right
// after Explorer (re)starts, Windhawk loads this mod into the
// new explorer process BEFORE the taskbar's XAML tree
// (XamlRoot / TaskbarFrameRepeater) has been built, so that
// single attempt found no (or only some) roots / repeaters and
// the reservation silently stayed off until the next settings
// change or mod toggle.
//
// Now every application runs on a dedicated retry thread (NOT
// a hook thread - hooks are still zero in this mod): it tries
// immediately, and while the pass is not "ready" (see
// IsReady) it retries with growing intervals for up to about
// a minute. All XAML access still happens on the taskbar UI
// thread via RunFromWindowThread - the retry thread itself
// never touches XAML; it only sleeps and dispatches. A pass in
// which the taskbar window does not exist yet counts as "not
// ready" (all-zero stats) and never blocks.
//
// Generation management: every StartApplyWithRetry call bumps
// g_applyGeneration and captures the new value; a retry loop
// exits as soon as its captured generation is stale, so a
// settings change supersedes the previous loop instead of two
// loops running concurrently. Wh_ModBeforeUninit sets
// g_modStopping and bumps the generation BEFORE restoring, so
// a retry can never re-apply margins while (or after) the
// restore runs.
//
// Settings note: LoadSettings() runs on the caller thread
// (Wh_ModAfterInit / Wh_ModSettingsChanged) before the retry
// thread starts; the thread creation synchronizes-with the new
// thread, so it observes the new values. A superseded loop
// exits before its next pass, so no pass reads g_settings
// while the next LoadSettings writes it.
// ---------------------------------------------------------------------------

std::atomic<bool> g_modStopping{false};
std::atomic<unsigned int> g_applyGeneration{0};

// One application pass. Returns the readiness stats of the
// pass; all-zero stats mean "no taskbar window yet" (or the
// dispatch to the UI thread failed), which is never ready.
// Runs on the retry thread; the actual work is dispatched to
// the taskbar UI thread by RunFromWindowThread.
ApplyStats RunApplyOnce() {
    ApplyStats stats;

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        // Either a folder-window explorer process (which never
        // hosts a taskbar - same as v0.6.1, it just keeps
        // retrying harmlessly until the attempts run out), or
        // the taskbar window has not been created yet.
        Wh_Log(L"No taskbar window in this explorer process "
               L"(yet), counting this attempt as not ready");
        return stats;
    }

    struct ApplyPassParam {
        ApplyStats stats;
    };
    ApplyPassParam param;

    if (!RunFromWindowThread(
            hTaskbarWnd,
            [](void* pParam) -> void {
                auto* param = static_cast<ApplyPassParam*>(pParam);
                param->stats = ApplyFromTaskbarThread();
            },
            &param)) {
        Wh_Log(L"RunFromWindowThread failed");
        return stats;
    }

    return param.stats;
}

void ApplyRetryLoop(unsigned int generation) {
    // Growing intervals between attempts; with the immediate
    // first attempt the last retry happens about a minute
    // after load / settings change, then the loop gives up.
    static const int kRetryDelaysMs[] = {500, 1000, 2000, 3000,
                                         5000, 8000, 10000, 15000,
                                         20000};

    for (int attempt = 1;; attempt++) {
        if (g_modStopping.load() ||
            g_applyGeneration.load() != generation) {
            Wh_Log(L"apply attempt %d: superseded or stopping, "
                   L"retry loop exits",
                   attempt);
            return;
        }

        ApplyStats stats = RunApplyOnce();
        bool ready = IsReady(stats);
        Wh_Log(L"apply attempt %d: taskbars=%d, roots=%d, "
               L"inScopeApplied=%d/%d -> %s",
               attempt, stats.taskbarCount, stats.xamlRootCount,
               stats.inScopeApplied, stats.inScopeTotal,
               ready ? L"ready" : L"not ready");
        if (ready) {
            Wh_Log(L"apply: ready after %d attempt(s)", attempt);
            return;
        }

        if (attempt > (int)ARRAYSIZE(kRetryDelaysMs)) {
            Wh_Log(L"apply: giving up after %d attempts, the "
                   L"taskbar UI did not become ready in time "
                   L"(toggle the mod or change any setting to "
                   L"retry)",
                   attempt);
            return;
        }

        // Interruptible sleep: bail out promptly when a newer
        // generation starts or the mod is unloading, instead
        // of sleeping through the whole delay.
        int delayMs = kRetryDelaysMs[attempt - 1];
        for (int sleptMs = 0; sleptMs < delayMs; sleptMs += 50) {
            if (g_modStopping.load() ||
                g_applyGeneration.load() != generation) {
                Wh_Log(L"apply: retry wait interrupted "
                       L"(superseded or stopping)");
                return;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}

// Starts a new retry-driven application, superseding any
// retry loop that is still running. Safe to call from
// Wh_ModAfterInit and Wh_ModSettingsChanged: it returns
// immediately, the attempts run on a detached thread.
void StartApplyWithRetry() {
    unsigned int generation = g_applyGeneration.fetch_add(1) + 1;
    try {
        std::thread(ApplyRetryLoop, generation).detach();
    } catch (...) {
        // Thread creation failed (extremely unlikely): fall
        // back to the v0.6.1 behavior of a single synchronous
        // attempt instead of never applying at all.
        Wh_Log(L"StartApplyWithRetry: could not start the retry "
               L"thread, applying once synchronously");
        RunApplyOnce();
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

    auto& tracked = TrackedRepeaters();
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

    Wh_Log(L"Taskbar Reserve Space for FluentFlyout v0.7 "
           L"(adds retry-until-ready re-application after load; "
           L"retry logic not yet tested)");
    Wh_Log(L"reservedWidth=%d, monitorScope=%s, "
           L"specificMonitorDevice=%s",
           g_settings.reservedWidth, MonitorScopeName(),
           g_settings.specificMonitorDevice);

    if (!HookTaskbarDllSymbols()) {
        Wh_Log(L"taskbar.dll symbols could not be resolved, "
               L"the mod will do nothing");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    StartApplyWithRetry();
}

void Wh_ModBeforeUninit() {
    // Stop any in-flight retry loop first (v0.7): a retry that
    // is still waiting for the taskbar to become ready must not
    // re-apply margins while - or after - the restore below
    // runs. Retry loops notice within ~50ms and exit without
    // starting another pass.
    g_modStopping = true;
    g_applyGeneration.fetch_add(1);

    if (TrackedRepeaters().empty()) {
        Wh_Log(L"Wh_ModBeforeUninit: no tracked repeaters, "
               L"nothing to restore");
        return;
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"Wh_ModBeforeUninit: this explorer process does not "
               L"host the taskbar, cannot restore (nothing was "
               L"applied from this process)");
        return;
    }

    if (!RunFromWindowThread(
            hTaskbarWnd,
            [](void* pParam) -> void { RestoreFromTaskbarThread(); },
            nullptr)) {
        Wh_Log(L"Wh_ModBeforeUninit: RunFromWindowThread failed, "
               L"margins could not be restored by the mod (restart "
               L"Explorer to clear them)");
    }
}

void Wh_ModUninit() {
    Wh_Log(L"v0.7 unloaded");
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    Wh_Log(L"Settings changed: reservedWidth=%d, monitorScope=%s, "
           L"specificMonitorDevice=%s",
           g_settings.reservedWidth, MonitorScopeName(),
           g_settings.specificMonitorDevice);
    StartApplyWithRetry();
}

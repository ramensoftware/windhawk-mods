// ==WindhawkMod==
// @id              hide-taskbar-only-on-desktop
// @name            Hide Taskbar Only on Desktop
// @description     Hides selected taskbars only while their display is showing the desktop
// @version         4.1.0
// @author          Sahil Dashoni
// @github          https://github.com/Sahil-Dashoni
// @include         windhawk.exe
// @include         explorer.exe
// @compilerOptions -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Taskbar Only on Desktop

Hides selected taskbars only while their own display is showing the desktop.
When an application window is present on that display, the taskbar remains
visible. Displays are evaluated independently.

## Demo

### Multiple Displays

![Multiple Display](https://raw.githubusercontent.com/Sahil-Dashoni/Hide-Taskbar-Only-on-Desktop-Windhawk-Mod/refs/heads/main/Assets/multiple-display.gif)

Each selected display is evaluated independently. An application can keep one display's taskbar visible while another display remains in the desktop-only state.

### Single Display

![Single Display](https://raw.githubusercontent.com/Sahil-Dashoni/Hide-Taskbar-Only-on-Desktop-Windhawk-Mod/refs/heads/main/Assets/single-display.gif)

The taskbar hides when the display returns to the desktop and can be revealed by moving the cursor into the configured bottom-edge area.

## Behavior

- A selected taskbar hides only when its display has no visible, non-minimized
  application window.
- Maximized windows use Windows' monitor assignment. Normal windows spanning
  displays count on every display they intersect.
- Bottom-edge hover reveal can be enabled independently per display.
- The hover zone is based on the current taskbar height and display DPI, plus
  the configured extra margin.
- The hover reveal feature is limited to bottom-docked taskbars.
- The configured post-hover delay applies to hover dismissal.
- Relevant Windows shell surfaces such as Start, taskbar popups/overflow,
  notification/Quick Settings, and supported Alt+Tab surfaces are treated as
  shell UI rather than normal application windows.
- Clearing the taskbar selection means no taskbar is hidden.
- Windows' native taskbar auto-hide setting is not changed.

## Why this is different from `taskbar-auto-hide-when-maximized`

These mods answer different visibility questions.

`taskbar-auto-hide-when-maximized` is based on the state of application
windows, with modes such as `intersected`, `maximized`, and `never`. This mod is
based on a different predicate: **is this display currently showing only the
desktop?**

That means a normal, non-maximized application keeps the selected taskbar
visible here. The taskbar hides only after the last visible, non-minimized
application on that display is gone. A maximized window is not required for the
taskbar to stay visible. Conversely, a completely idle display can hide its
taskbar even while another display is actively being used.

The distinction is therefore the user-visible policy, not merely the mechanism
used internally:

| Scenario | This mod | `taskbar-auto-hide-when-maximized` |
| --- | --- | --- |
| Desktop only on a selected display | Hide taskbar | Depends on its selected window-state mode |
| One normal, non-maximized app visible | Keep taskbar visible | Depends on mode/window state |
| One display has an app, another is idle | Evaluate each display separately | Uses its own monitor/window-state policy |
| Goal | Desktop-only taskbar visibility | Auto-hide driven by maximized/intersection state |

This mod also deliberately leaves Windows' native auto-hide preference alone
and implements the desktop-only rule directly, including its explicit
per-display selection and configurable hover-dismiss behavior.

The project remains standalone because its primary purpose is this specific
desktop-only policy. The existing mod is still the better choice for users who
want taskbar auto-hide tied to maximized/intersected window state.

## Per-display configuration

You can independently choose:

- Which displays should hide their taskbar when desktop-only.
- Which displays should allow bottom-edge hover reveal.

Selections use Windows display device names such as `\\.\DISPLAY1`. These
identifiers may differ from the numbers shown in Windows Display Settings.

The mod tracks monitor identity during the current session so a reconnected
display that receives a different `DISPLAYn` number can retain its selection
when the same monitor identity is detected.

## Hover reveal

When a selected, bottom-docked taskbar is hidden by the mod, moving the pointer
into its configured bottom-edge zone reveals it. After leaving the zone, the
taskbar remains visible for the configured dismissal delay before returning to
its normal desktop/application state.

Hover tracking is adaptive: it samples more frequently when a configured hover
zone can matter, uses a slower idle interval otherwise, and backs off further
when cursor queries repeatedly fail (for example on the secure desktop).

## Windows shell interactions

The mod distinguishes relevant shell surfaces from ordinary application
windows. It uses both window classes and the owning process for supported
Windows shell components, including taskbar popup/overflow surfaces,
Start/notification/Quick Settings hosts, relevant XAML shell hosts, and known
Alt+Tab window classes. These surfaces can keep the relevant taskbar visible
while the user is interacting with the shell.

## Explorer integration and crash recovery

The main state manager runs in a dedicated `windhawk.exe` process. A small
Explorer-side hook exists only to prevent the specific secondary-taskbar
`SW_SHOWNA` transition that caused a visible re-show after the mod had hidden
the taskbar. It does not alter Explorer's general taskbar behavior.

Each taskbar hidden by the mod is marked with the PID of the owning tool
process. Explorer caches and checks that process's liveness. If a stale marker
is detected, the marker is removed and the taskbar is restored through the
original Explorer `ShowWindow` path. Explorer also performs a stale-marker
sweep when the Explorer-side component initializes, covering both the primary
and secondary taskbars.

This means a normal unload restores taskbars, while stale state left by an
unexpected tool-process termination can be cleaned up without requiring an
Explorer restart. Taskbars that were never hidden by this mod are not restored
by the cleanup path.

## Multi-monitor behavior

Example with two displays:

1. Display 1 has an application open.
2. Display 2 shows only the desktop.
3. The selected taskbar on display 1 stays visible.
4. The selected taskbar on display 2 hides.
5. Moving to the configured bottom edge of display 2 reveals its taskbar.
6. Leaving the hover zone starts the configured dismissal delay.
7. Opening an application on display 2 keeps its taskbar visible.

Applications spanning multiple displays count on every display they intersect.
Maximized windows use Windows' monitor assignment.

## Display and taskbar changes

Taskbars are rediscovered during reconciliation, so Explorer taskbar recreation
does not leave the mod tied to an old window handle. State is refreshed for
relevant taskbar, display, settings, theme, window, and shell visibility
changes, with a periodic safety poll for missed transitions.

A display-topology signature resets transient hover state when monitor geometry
or the connected display set changes.

## Limitations

- Hover reveal is supported only for bottom-docked taskbars.
- Display selection currently exposes `DISPLAY1` through `DISPLAY16`.
- `DISPLAYn` identifiers may differ from the numbering shown in Windows Display
  Settings and can change after display configuration changes.
- A taskbar docked to the top or side is not hidden by the desktop-only rule.
- A display selection that no longer matches any connected display remains
  configured until the user changes the setting.
- The mod does not modify Windows' native taskbar auto-hide setting.

## Demo

A real recording is recommended here for the repository submission: show a
selected taskbar hiding when its display becomes desktop-only, revealing when
the pointer reaches the bottom edge, and hiding again after the dismissal
delay. Do not use a generated or illustrative image in place of a real
recording.

## Goal

The goal is a specific visibility rule:

> **Hide the taskbar when its display is showing only the desktop.**

The mod combines that per-display application-state rule with independent
display selection, shell-interaction handling, taskbar recreation recovery,
and configurable bottom-edge hover reveal.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- extraHoverMarginPx: 8
  $name: Extra hover margin (px)
  $description: >-
    The reveal zone at the bottom of the screen automatically matches your
    taskbar's actual height (including your display scaling), so hovering
    anywhere over where the taskbar would be reveals it. This setting adds
    a bit of extra margin above that, in pixels, so you don't need to be pixel-perfect. Default 8px.
- autoHideDelayMs: 700
  $name: Auto-hide delay after hover (ms)
  $description: >-
    How long to wait, after moving the mouse away from the bottom-edge
    hover zone, before the taskbar hides again. Only applies to that case
    - other hides (e.g. minimizing the last window) are instant.
    Default 700ms.
- hoverRevealOnMonitors: ["all"]
  $name: Reveal taskbar on bottom-edge hover
  $description: >-
    Select the displays where bottom-edge hovering should reveal the taskbar.
    The display options use Windows `\\.\DISPLAYn` device numbers, which may
    differ from the numbers shown in Windows Display Settings. Select All
    displays to enable it everywhere, or replace it with individual displays.
  $options:
  - all: All displays
  - monitor1: DISPLAY1
  - monitor2: DISPLAY2
  - monitor3: DISPLAY3
  - monitor4: DISPLAY4
  - monitor5: DISPLAY5
  - monitor6: DISPLAY6
  - monitor7: DISPLAY7
  - monitor8: DISPLAY8
  - monitor9: DISPLAY9
  - monitor10: DISPLAY10
  - monitor11: DISPLAY11
  - monitor12: DISPLAY12
  - monitor13: DISPLAY13
  - monitor14: DISPLAY14
  - monitor15: DISPLAY15
  - monitor16: DISPLAY16
- hideOnMonitors: ["all"]
  $name: Taskbars to hide on desktop
  $description: >-
    Select one or more displays using the Windows `\\.\DISPLAYn` device number.
    These numbers may differ from the display numbers shown in Windows Display
    Settings. Choose All displays to hide every connected display. Only
    bottom-docked taskbars participate in desktop-based hiding. Use Add to
    select multiple displays. When a physical display is reconnected and Windows
    renumbers DISPLAYn, the mod keeps the selection bound to the same detected
    monitor device when possible.
  $options:
  - all: All displays
  - monitor1: DISPLAY1
  - monitor2: DISPLAY2
  - monitor3: DISPLAY3
  - monitor4: DISPLAY4
  - monitor5: DISPLAY5
  - monitor6: DISPLAY6
  - monitor7: DISPLAY7
  - monitor8: DISPLAY8
  - monitor9: DISPLAY9
  - monitor10: DISPLAY10
  - monitor11: DISPLAY11
  - monitor12: DISPLAY12
  - monitor13: DISPLAY13
  - monitor14: DISPLAY14
  - monitor15: DISPLAY15
  - monitor16: DISPLAY16
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <windhawk_utils.h>
#include <wchar.h>


constexpr size_t kMaxMonitorNumbers = 16;
constexpr size_t kMaxTaskbars = 16;

constexpr UINT WM_APP_REFRESH = WM_APP + 1;
constexpr UINT WM_APP_SETTINGS = WM_APP + 2;
constexpr UINT_PTR kSafetyTimerId = 1;
constexpr UINT_PTR kHoverExpireTimerId = 2;

struct {
    int extraHoverMarginPx;
    DWORD autoHideDelayMs;
    bool hideAllMonitors;
    bool hideMonitor[kMaxMonitorNumbers + 1];
    bool hoverAllMonitors;
    bool hoverMonitor[kMaxMonitorNumbers + 1];
} g_settings = {};

struct MonitorEntry {
    HMONITOR monitor;
    RECT rect;
    wchar_t deviceName[32];
    wchar_t stableDeviceId[256];
};

struct MonitorList {
    MonitorEntry entries[kMaxMonitorNumbers];
    size_t count;
};

struct TaskbarMonitorState {
    HWND hwnd;
    HMONITOR monitor;
    int monitorNumber;
    wchar_t stableDeviceId[256];
    bool desktopOnly;
    bool hiddenByMod;
};

struct MonitorSelectionBinding {
    bool configured;
    wchar_t stableDeviceId[256];
};

struct WindowScanResult {
    bool applicationOnMonitor[kMaxMonitorNumbers];
    bool shellSurfaceOnMonitor[kMaxMonitorNumbers];
};

HWINEVENTHOOK g_foregroundHook = nullptr;
HWINEVENTHOOK g_minimizeHook = nullptr;
HWINEVENTHOOK g_moveHook = nullptr;
HWINEVENTHOOK g_taskbarShowHook = nullptr;
DWORD g_taskbarShowHookProcessId = 0;

HANDLE g_workerThread = nullptr;
DWORD g_workerThreadId = 0;
HANDLE g_workerReadyEvent = nullptr;
HANDLE g_cursorThread = nullptr;
HANDLE g_cursorStopEvent = nullptr;

struct CursorHoverSnapshot {
    HMONITOR monitor;
    RECT monitorRect;
    int hotZonePx;
    bool enabled;
};

CursorHoverSnapshot g_cursorHoverSnapshots[kMaxTaskbars] = {};
size_t g_cursorHoverSnapshotCount = 0;
SRWLOCK g_cursorHoverSnapshotLock = SRWLOCK_INIT;

constexpr wchar_t kHiddenByModProperty[] =
    L"Windhawk.HideTaskbarOnlyOnDesktop.HiddenByMod";

// Explorer-side protection for the secondary-taskbar shell transition.
// The dedicated tool process remains authoritative for taskbar state. Explorer
// only consults the per-window property and blocks SW_SHOWNA for an explicitly
// hidden secondary taskbar while its owning tool process is still alive.
using ShowWindow_t = decltype(&ShowWindow);
ShowWindow_t g_explorerShowWindowOriginal = nullptr;
bool g_isExplorerProcess = false;

DWORD g_hiddenTaskbarOwnerPid = 0;
HANDLE g_hiddenTaskbarOwnerProcess = nullptr;
SRWLOCK g_hiddenTaskbarOwnerCacheLock = SRWLOCK_INIT;

void ResetHiddenTaskbarOwnerCache() {
    AcquireSRWLockExclusive(
        &g_hiddenTaskbarOwnerCacheLock
    );

    if (g_hiddenTaskbarOwnerProcess) {
        CloseHandle(
            g_hiddenTaskbarOwnerProcess
        );
        g_hiddenTaskbarOwnerProcess = nullptr;
    }

    g_hiddenTaskbarOwnerPid = 0;

    ReleaseSRWLockExclusive(
        &g_hiddenTaskbarOwnerCacheLock
    );
}

bool IsExplorerTaskbar(HWND hwnd) {
    if (!hwnd) {
        return false;
    }

    WCHAR className[128] = {};

    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0) {
        return false;
    }

    return wcscmp(className, L"Shell_TrayWnd") == 0 ||
           wcscmp(className, L"Shell_SecondaryTrayWnd") == 0;
}

bool IsExplorerSecondaryTaskbar(HWND hwnd) {
    if (!hwnd) {
        return false;
    }

    WCHAR className[128] = {};

    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0) {
        return false;
    }

    return wcscmp(className, L"Shell_SecondaryTrayWnd") == 0;
}

bool IsHiddenTaskbarOwnerAlive(HWND hwnd) {
    if (!IsExplorerTaskbar(hwnd)) {
        return false;
    }

    HANDLE marker = GetPropW(hwnd, kHiddenByModProperty);

    if (!marker) {
        return false;
    }

    DWORD ownerPid = static_cast<DWORD>(reinterpret_cast<ULONG_PTR>(marker));

    if (!ownerPid) {
        RemovePropW(hwnd, kHiddenByModProperty);
        return false;
    }

    AcquireSRWLockExclusive(&g_hiddenTaskbarOwnerCacheLock);

    if (ownerPid != g_hiddenTaskbarOwnerPid) {
        if (g_hiddenTaskbarOwnerProcess) {
            CloseHandle(g_hiddenTaskbarOwnerProcess);
            g_hiddenTaskbarOwnerProcess = nullptr;
        }

        g_hiddenTaskbarOwnerPid = ownerPid;
        g_hiddenTaskbarOwnerProcess =
            OpenProcess(SYNCHRONIZE, FALSE, ownerPid);
    }

    bool ownerAlive =
        g_hiddenTaskbarOwnerProcess &&
        WaitForSingleObject(g_hiddenTaskbarOwnerProcess, 0) == WAIT_TIMEOUT;

    if (!ownerAlive) {
        RemovePropW(hwnd, kHiddenByModProperty);

        if (g_hiddenTaskbarOwnerProcess) {
            CloseHandle(g_hiddenTaskbarOwnerProcess);
            g_hiddenTaskbarOwnerProcess = nullptr;
        }

        g_hiddenTaskbarOwnerPid = 0;
    }

    ReleaseSRWLockExclusive(&g_hiddenTaskbarOwnerCacheLock);

    return ownerAlive;
}

void RecoverStaleHiddenTaskbar(HWND hwnd) {
    if (!IsExplorerTaskbar(hwnd) ||
        !GetPropW(hwnd, kHiddenByModProperty)) {
        return;
    }

    if (IsHiddenTaskbarOwnerAlive(hwnd)) {
        return;
    }

    if (g_explorerShowWindowOriginal) {
        g_explorerShowWindowOriginal(hwnd, SW_SHOW);
    }
}

void RecoverStaleHiddenTaskbars() {
    RecoverStaleHiddenTaskbar(FindWindowW(L"Shell_TrayWnd", nullptr));

    HWND secondary = nullptr;

    while ((secondary = FindWindowExW(
                nullptr,
                secondary,
                L"Shell_SecondaryTrayWnd",
                nullptr)) != nullptr) {
        RecoverStaleHiddenTaskbar(secondary);
    }
}

BOOL WINAPI ExplorerShowWindowHook(HWND hwnd, int nCmdShow) {
    if (nCmdShow == SW_SHOWNA && IsExplorerTaskbar(hwnd)) {
        const bool marked =
            GetPropW(hwnd, kHiddenByModProperty) != nullptr;

        if (marked) {
            const bool ownerAlive = IsHiddenTaskbarOwnerAlive(hwnd);

            if (!ownerAlive && g_explorerShowWindowOriginal) {
                return g_explorerShowWindowOriginal(hwnd, SW_SHOW);
            }

            if (ownerAlive && IsExplorerSecondaryTaskbar(hwnd)) {
                return FALSE;
            }
        }
    }

    return g_explorerShowWindowOriginal(hwnd, nCmdShow);
}

bool InstallExplorerVisibilityHook() {
    if (
        !WindhawkUtils::SetFunctionHook(
            ShowWindow,
            ExplorerShowWindowHook,
            &g_explorerShowWindowOriginal
        )
    ) {
        Wh_Log(
            L"Failed to hook Explorer ShowWindow"
        );
        return false;
    }

    return true;
}

HWND g_workerMessageWindow = nullptr;
ATOM g_workerWindowClassAtom = 0;
UINT g_taskbarCreatedMessage = 0;
ULONGLONG g_displayTopologySignature = 0;

TaskbarMonitorState g_taskbarStates[kMaxTaskbars] = {};
size_t g_taskbarStateCount = 0;
bool g_nativeAutoHideEnabled = false;

MonitorSelectionBinding
    g_hideMonitorBindings[kMaxMonitorNumbers + 1] = {};
MonitorSelectionBinding
    g_hoverMonitorBindings[kMaxMonitorNumbers + 1] = {};

bool g_hoverActive = false;
HMONITOR g_hoverMonitor = nullptr;
ULONGLONG g_hoverDeadline = 0;

LONG g_refreshPosted = 0;
void LoadSettings();
void WhTool_ModUninit();
void ArmHoverExpireTimer();
void CancelHoverExpireTimer();
void RestoreAllTaskbars();
bool WaitForThreadWithTimeout(
    HANDLE thread,
    DWORD timeoutMs,
    const wchar_t* threadName
);

bool IsShellChromeClass(
    const WCHAR* className
) {
    if (!className) {
        return false;
    }

    static const WCHAR* kClasses[] = {
        L"Progman",
        L"WorkerW",
        L"Shell_TrayWnd",
        L"Shell_SecondaryTrayWnd",
        L"TaskListThumbnailWnd",
        L"SysShadow",
        L"tooltips_class32",
        L"MSCTFIME UI",
        L"IME",
    };

    for (const WCHAR* shellClass : kClasses) {
        if (wcscmp(className, shellClass) == 0) {
            return true;
        }
    }

    return false;
}

bool IsDesktopInfrastructureWindow(
    HWND hwnd,
    const WCHAR* className
) {
    if (!hwnd || !className) {
        return false;
    }

    if (
        wcscmp(className, L"Progman") == 0 ||
        wcscmp(className, L"WorkerW") == 0
    ) {
        return true;
    }

    HWND shellWindow = GetShellWindow();

    return shellWindow && shellWindow == hwnd;
}

struct StableMonitorDeviceCacheEntry {
    bool valid;
    wchar_t deviceName[32];
    wchar_t stableDeviceId[256];
};

StableMonitorDeviceCacheEntry
    g_stableMonitorDeviceCache[kMaxMonitorNumbers] = {};

void ClearStableMonitorDeviceIdCache() {
    for (auto& entry : g_stableMonitorDeviceCache) {
        entry = {};
    }
}

bool GetStableMonitorDeviceId(
    const wchar_t* deviceName,
    wchar_t* output,
    size_t outputCount
) {
    if (
        !deviceName ||
        !output ||
        outputCount == 0
    ) {
        return false;
    }

    output[0] = L'\0';

    for (const auto& entry : g_stableMonitorDeviceCache) {
        if (
            entry.valid &&
            wcscmp(
                entry.deviceName,
                deviceName
            ) == 0
        ) {
            wcsncpy_s(
                output,
                outputCount,
                entry.stableDeviceId,
                _TRUNCATE
            );
            return output[0] != L'\0';
        }
    }

    wchar_t stableDeviceId[256] = {};
    DISPLAY_DEVICEW adapter = {};
    adapter.cb = sizeof(adapter);

    for (
        DWORD adapterIndex = 0;
        EnumDisplayDevicesW(
            nullptr,
            adapterIndex,
            &adapter,
            0
        );
        ++adapterIndex
    ) {
        if (
            wcscmp(
                adapter.DeviceName,
                deviceName
            ) != 0
        ) {
            adapter = {};
            adapter.cb = sizeof(adapter);
            continue;
        }

        DISPLAY_DEVICEW monitor = {};
        monitor.cb = sizeof(monitor);

        if (
            EnumDisplayDevicesW(
                adapter.DeviceName,
                0,
                &monitor,
                0
            ) &&
            monitor.DeviceID[0] != L'\0'
        ) {
            wcsncpy_s(
                stableDeviceId,
                ARRAYSIZE(stableDeviceId),
                monitor.DeviceID,
                _TRUNCATE
            );
        } else if (adapter.DeviceID[0] != L'\0') {
            wcsncpy_s(
                stableDeviceId,
                ARRAYSIZE(stableDeviceId),
                adapter.DeviceID,
                _TRUNCATE
            );
        }

        break;
    }

    if (stableDeviceId[0] == L'\0') {
        return false;
    }

    for (auto& entry : g_stableMonitorDeviceCache) {
        if (!entry.valid) {
            entry.valid = true;

            wcsncpy_s(
                entry.deviceName,
                ARRAYSIZE(entry.deviceName),
                deviceName,
                _TRUNCATE
            );

            wcsncpy_s(
                entry.stableDeviceId,
                ARRAYSIZE(entry.stableDeviceId),
                stableDeviceId,
                _TRUNCATE
            );

            break;
        }
    }

    wcsncpy_s(
        output,
        outputCount,
        stableDeviceId,
        _TRUNCATE
    );

    return output[0] != L'\0';
}

int GetDisplayDeviceNumber(
    const wchar_t* deviceName
) {
    if (!deviceName) {
        return 0;
    }

    constexpr wchar_t kPrefix[] = L"\\\\.\\DISPLAY";
    constexpr size_t kPrefixLength =
        ARRAYSIZE(kPrefix) - 1;

    if (
        wcsncmp(
            deviceName,
            kPrefix,
            kPrefixLength
        ) != 0
    ) {
        return 0;
    }

    wchar_t* endNumber = nullptr;

    long number =
        wcstol(
            deviceName + kPrefixLength,
            &endNumber,
            10
        );

    if (
        endNumber == deviceName + kPrefixLength ||
        *endNumber != L'\0' ||
        number < 1 ||
        number > static_cast<long>(
            kMaxMonitorNumbers
        )
    ) {
        Wh_Log(
            L"Unsupported display device number for %s: %ld (supported range: 1-%d)",
            deviceName,
            number,
            static_cast<int>(kMaxMonitorNumbers)
        );
        return 0;
    }

    return static_cast<int>(number);
}

void ResetMonitorSelectionBindings() {
    for (
        size_t i = 0;
        i <= kMaxMonitorNumbers;
        ++i
    ) {
        g_hideMonitorBindings[i] = {};
        g_hoverMonitorBindings[i] = {};
    }
}

void BindSelectionIdentity(
    int configuredNumber,
    const MonitorList& monitors,
    const bool* selected,
    MonitorSelectionBinding* bindings
) {
    if (
        configuredNumber < 1 ||
        configuredNumber > static_cast<int>(kMaxMonitorNumbers) ||
        !selected ||
        !bindings ||
        !selected[configuredNumber] ||
        bindings[configuredNumber].configured
    ) {
        return;
    }

    for (size_t i = 0; i < monitors.count; ++i) {
        if (
            GetDisplayDeviceNumber(
                monitors.entries[i].deviceName
            ) == configuredNumber
        ) {
            if (monitors.entries[i].stableDeviceId[0] != L'\0') {
                bindings[configuredNumber].configured = true;
                wcsncpy_s(
                    bindings[configuredNumber].stableDeviceId,
                    ARRAYSIZE(
                        bindings[configuredNumber].stableDeviceId
                    ),
                    monitors.entries[i].stableDeviceId,
                    _TRUNCATE
                );
            }

            return;
        }
    }
}

void BindConfiguredMonitorSelections(
    const MonitorList& monitors
) {
    for (
        int configuredNumber = 1;
        configuredNumber <= static_cast<int>(kMaxMonitorNumbers);
        ++configuredNumber
    ) {
        BindSelectionIdentity(
            configuredNumber,
            monitors,
            g_settings.hideMonitor,
            g_hideMonitorBindings
        );

        BindSelectionIdentity(
            configuredNumber,
            monitors,
            g_settings.hoverMonitor,
            g_hoverMonitorBindings
        );
    }
}

BOOL CALLBACK CollectMonitorProc(
    HMONITOR monitor,
    HDC,
    LPRECT,
    LPARAM lParam
) {
    MonitorList* list =
        reinterpret_cast<MonitorList*>(lParam);

    if (!list || list->count >= kMaxMonitorNumbers) {
        return FALSE;
    }

    MONITORINFOEXW info = {};
    info.cbSize = sizeof(info);

    if (!GetMonitorInfoW(monitor, &info)) {
        return TRUE;
    }

    MonitorEntry& entry =
        list->entries[list->count++];

    entry.monitor = monitor;
    entry.rect = info.rcMonitor;
    wcsncpy_s(
        entry.deviceName,
        ARRAYSIZE(entry.deviceName),
        info.szDevice,
        _TRUNCATE
    );

    GetStableMonitorDeviceId(
        entry.deviceName,
        entry.stableDeviceId,
        ARRAYSIZE(entry.stableDeviceId)
    );

    return TRUE;
}

MonitorList GetCurrentMonitors() {
    MonitorList list = {};

    EnumDisplayMonitors(
        nullptr,
        nullptr,
        CollectMonitorProc,
        reinterpret_cast<LPARAM>(&list)
    );

    return list;
}

ULONGLONG HashDisplayTopology(
    const MonitorList& monitors
) {
    // FNV-1a style hash over monitor device names and geometry. The signature
    // is only used to detect that the topology changed, not as a stable ID.
    ULONGLONG hash = 1469598103934665603ull;

    auto mixByte = [&](unsigned char value) {
        hash ^= value;
        hash *= 1099511628211ull;
    };

    auto mixInt = [&](LONG value) {
        unsigned long v = static_cast<unsigned long>(value);
        for (int shift = 0; shift < 32; shift += 8) {
            mixByte(static_cast<unsigned char>((v >> shift) & 0xff));
        }
    };

    mixInt(static_cast<LONG>(monitors.count));

    for (size_t i = 0; i < monitors.count; ++i) {
        const MonitorEntry& entry = monitors.entries[i];

        mixInt(entry.rect.left);
        mixInt(entry.rect.top);
        mixInt(entry.rect.right);
        mixInt(entry.rect.bottom);

        for (const wchar_t* pName = entry.deviceName; *pName; ++pName) {
            wchar_t ch = *pName;
            mixByte(static_cast<unsigned char>(ch & 0xff));
            mixByte(static_cast<unsigned char>((ch >> 8) & 0xff));
        }

        mixByte(0);
    }

    return hash;
}

int GetMonitorNumber(
    const MonitorList& list,
    HMONITOR monitor
) {
    for (size_t i = 0; i < list.count; ++i) {
        if (list.entries[i].monitor == monitor) {
            return GetDisplayDeviceNumber(
                list.entries[i].deviceName
            );
        }
    }

    return 0;
}

bool IsBottomDockedTaskbar(HWND hTaskbar, HMONITOR monitor);

bool StableDeviceIdsMatch(
    const wchar_t* left,
    const wchar_t* right
) {
    return
        left &&
        right &&
        left[0] != L'\0' &&
        right[0] != L'\0' &&
        wcscmp(left, right) == 0;
}

bool IsMonitorSelected(
    int monitorNumber,
    const wchar_t* stableDeviceId,
    const bool* selected,
    const MonitorSelectionBinding* bindings
) {
    if (!selected || !bindings) {
        return false;
    }

    if (
        monitorNumber >= 1 &&
        monitorNumber <= static_cast<int>(kMaxMonitorNumbers) &&
        selected[monitorNumber] &&
        !bindings[monitorNumber].configured
    ) {
        return true;
    }

    for (
        int configuredNumber = 1;
        configuredNumber <= static_cast<int>(kMaxMonitorNumbers);
        ++configuredNumber
    ) {
        if (
            selected[configuredNumber] &&
            bindings[configuredNumber].configured &&
            StableDeviceIdsMatch(
                stableDeviceId,
                bindings[configuredNumber].stableDeviceId
            )
        ) {
            return true;
        }
    }

    return false;
}

bool ShouldHideMonitor(
    const TaskbarMonitorState& state
) {
    if (g_settings.hideAllMonitors) {
        return true;
    }

    return IsMonitorSelected(
        state.monitorNumber,
        state.stableDeviceId,
        g_settings.hideMonitor,
        g_hideMonitorBindings
    );
}

bool ShouldRevealOnHover(
    const TaskbarMonitorState& state
) {
    if (g_settings.hoverAllMonitors) {
        return true;
    }

    return IsMonitorSelected(
        state.monitorNumber,
        state.stableDeviceId,
        g_settings.hoverMonitor,
        g_hoverMonitorBindings
    );
}

bool GetWindowProcessImageName(
    DWORD pid,
    wchar_t* output,
    size_t outputCount
) {
    if (!pid || !output || outputCount == 0) {
        return false;
    }

    output[0] = L'\0';

    HANDLE process =
        OpenProcess(
            PROCESS_QUERY_LIMITED_INFORMATION,
            FALSE,
            pid
        );

    if (!process) {
        return false;
    }

    DWORD size =
        static_cast<DWORD>(
            outputCount
        );

    BOOL result =
        QueryFullProcessImageNameW(
            process,
            0,
            output,
            &size
        );

    CloseHandle(process);

    return result && output[0] != L'\0';
}

bool IsKnownShellProcess(DWORD pid) {
    wchar_t imagePath[MAX_PATH] = {};

    if (!GetWindowProcessImageName(
            pid,
            imagePath,
            ARRAYSIZE(imagePath)
        )) {
        return false;
    }

    const wchar_t* baseName =
        wcsrchr(
            imagePath,
            L'\\'
        );

    baseName =
        baseName
            ? baseName + 1
            : imagePath;

    return
        _wcsicmp(
            baseName,
            L"StartMenuExperienceHost.exe"
        ) == 0 ||
        _wcsicmp(
            baseName,
            L"ShellExperienceHost.exe"
        ) == 0 ||
        _wcsicmp(
            baseName,
            L"ShellHost.exe"
        ) == 0;
}

bool IsExplorerProcess(DWORD pid) {
    wchar_t imagePath[MAX_PATH] = {};

    if (!GetWindowProcessImageName(
            pid,
            imagePath,
            ARRAYSIZE(imagePath)
        )) {
        return false;
    }

    const wchar_t* baseName =
        wcsrchr(
            imagePath,
            L'\\'
        );

    baseName =
        baseName
            ? baseName + 1
            : imagePath;

    return _wcsicmp(
        baseName,
        L"explorer.exe"
    ) == 0;
}

bool IsTaskbarPopupClass(
    const WCHAR* className
) {
    if (!className) {
        return false;
    }

    static const WCHAR* kClasses[] = {
        L"#32768",
        L"#32771",
        L"Xaml_WindowedPopupClass",
        L"TopLevelWindowForOverflowXamlIsland",
        L"NotifyIconOverflowWindow",
        L"TaskbarOverflowWnd",
    };

    for (const WCHAR* shellClass : kClasses) {
        if (wcscmp(className, shellClass) == 0) {
            return true;
        }
    }

    return false;
}

bool IsAltTabClass(
    const WCHAR* className
) {
    if (!className) {
        return false;
    }

    static const WCHAR* kClasses[] = {
        L"MultitaskingViewFrame",
        L"TaskSwitcherWnd",
        L"TaskSwitcherOverlayWnd",
        L"ForegroundStaging",
    };

    for (const WCHAR* shellClass : kClasses) {
        if (wcscmp(className, shellClass) == 0) {
            return true;
        }
    }

    return false;
}

bool IsShellSurfaceWindow(
    HWND hwnd,
    const WCHAR* className
) {
    if (
        !hwnd ||
        !className ||
        !IsWindowVisible(hwnd)
    ) {
        return false;
    }

    DWORD pid = 0;

    GetWindowThreadProcessId(
        hwnd,
        &pid
    );

    if (IsTaskbarPopupClass(className)) {
        return true;
    }

    if (
        IsAltTabClass(className) &&
        (
            IsExplorerProcess(pid) ||
            IsKnownShellProcess(pid)
        )
    ) {
        return true;
    }

    // Windows 11 uses XAML host windows for several shell surfaces,
    // including Start and Alt+Tab variants. Limit this class to Explorer
    // and the known Windows shell processes.
    if (
        wcscmp(
            className,
            L"XamlExplorerHostIslandWindow"
        ) == 0 &&
        (
            IsExplorerProcess(pid) ||
            IsKnownShellProcess(pid)
        )
    ) {
        return true;
    }

    // Start, notification center and quick settings can expose
    // Windows.UI.Core.CoreWindow instances from their dedicated
    // Windows shell processes.
    if (
        wcscmp(
            className,
            L"Windows.UI.Core.CoreWindow"
        ) == 0 &&
        IsKnownShellProcess(pid)
    ) {
        return true;
    }

    return false;
}

bool IsTransientShellWindow(
    HWND hwnd,
    const WCHAR* className
) {
    return IsShellSurfaceWindow(
        hwnd,
        className
    );
}

bool IsApplicationWindowCandidate(
    HWND hwnd,
    const WCHAR* className
) {
    if (
        !hwnd ||
        !className ||
        !IsWindowVisible(hwnd) ||
        IsIconic(hwnd)
    ) {
        return false;
    }

    LONG_PTR exStyle =
        GetWindowLongPtrW(
            hwnd,
            GWL_EXSTYLE
        );

    if (
        GetWindow(
            hwnd,
            GW_OWNER
        ) != nullptr &&
        !(exStyle & WS_EX_APPWINDOW)
    ) {
        return false;
    }

    if (IsTransientShellWindow(
            hwnd,
            className
        )) {
        return false;
    }

    if (exStyle & WS_EX_TOOLWINDOW) {
        return false;
    }

    BOOL cloaked = FALSE;

    if (
        SUCCEEDED(
            DwmGetWindowAttribute(
                hwnd,
                DWMWA_CLOAKED,
                &cloaked,
                sizeof(cloaked)
            )
        ) &&
        cloaked
    ) {
        return false;
    }

    if (
        IsDesktopInfrastructureWindow(
            hwnd,
            className
        ) ||
        IsShellChromeClass(className)
    ) {
        return false;
    }

    // Background title-less helper surfaces are ignored. A title-less
    // application becomes covered by the foreground-window path.
    return
        (exStyle & WS_EX_APPWINDOW) != 0 ||
        GetWindowTextLengthW(hwnd) > 0;
}

struct ScanContext {
    const MonitorList* monitors;
    WindowScanResult* result;
};

BOOL CALLBACK ScanWindowsWithMonitorsProc(
    HWND hwnd,
    LPARAM lParam
) {
    ScanContext* context =
        reinterpret_cast<ScanContext*>(lParam);

    if (
        !context ||
        !context->monitors ||
        !context->result
    ) {
        return TRUE;
    }

    WCHAR className[256] = {};

    if (
        GetClassNameW(
            hwnd,
            className,
            ARRAYSIZE(className)
        ) == 0
    ) {
        return TRUE;
    }

    if (IsShellSurfaceWindow(
            hwnd,
            className
        )) {
        RECT popupRect = {};

        if (GetWindowRect(hwnd, &popupRect)) {
            // Mark every display intersected by the popup rather than using
            // its center point. This handles shell surfaces that straddle
            // monitor boundaries and avoids arbitrarily selecting one display.
            for (
                size_t i = 0;
                i < context->monitors->count;
                ++i
            ) {
                RECT intersection = {};

                if (IntersectRect(
                        &intersection,
                        &popupRect,
                        &context->monitors->entries[i].rect
                    )) {
                    context->result->shellSurfaceOnMonitor[i] = true;
                }
            }
        }
    }

    if (!IsApplicationWindowCandidate(
            hwnd,
            className
        )) {
        return TRUE;
    }

    RECT rect = {};

    if (
        !GetWindowRect(
            hwnd,
            &rect
        ) ||
        rect.right <= rect.left ||
        rect.bottom <= rect.top
    ) {
        return TRUE;
    }

    WINDOWPLACEMENT placement = {};
    placement.length = sizeof(placement);

    if (
        GetWindowPlacement(
            hwnd,
            &placement
        ) &&
        placement.showCmd == SW_SHOWMAXIMIZED
    ) {
        HMONITOR windowMonitor =
            MonitorFromWindow(
                hwnd,
                MONITOR_DEFAULTTONEAREST
            );

        for (
            size_t i = 0;
            i < context->monitors->count;
            ++i
        ) {
            if (
                context->monitors->entries[i].monitor ==
                windowMonitor
            ) {
                context->result->applicationOnMonitor[i] = true;
                return TRUE;
            }
        }

        return TRUE;
    }

    for (
        size_t i = 0;
        i < context->monitors->count;
        ++i
    ) {
        RECT intersection = {};

        if (
            IntersectRect(
                &intersection,
                &rect,
                &context->monitors->entries[i].rect
            )
        ) {
            context->result->applicationOnMonitor[i] = true;
        }
    }

    return TRUE;
}

void ScanWindowsOnce(
    const MonitorList& monitors,
    WindowScanResult& result
) {
    result = {};

    ScanContext context = {
        &monitors,
        &result
    };

    EnumWindows(
        ScanWindowsWithMonitorsProc,
        reinterpret_cast<LPARAM>(&context)
    );

    /*
     * A title-less foreground application is allowed even when it did not
     * meet the conservative background candidate rule.
     */
    HWND foreground =
        GetForegroundWindow();

    if (
        foreground &&
        IsWindowVisible(foreground) &&
        !IsIconic(foreground)
    ) {
        WCHAR className[256] = {};

        if (
            GetClassNameW(
                foreground,
                className,
                ARRAYSIZE(className)
            ) != 0 &&
            !IsDesktopInfrastructureWindow(
                foreground,
                className
            ) &&
            !IsShellChromeClass(className)
        ) {
            BOOL cloaked = FALSE;

            if (
                !(
                    SUCCEEDED(
                        DwmGetWindowAttribute(
                            foreground,
                            DWMWA_CLOAKED,
                            &cloaked,
                            sizeof(cloaked)
                        )
                    ) &&
                    cloaked
                )
            ) {
                HMONITOR foregroundMonitor =
                    MonitorFromWindow(
                        foreground,
                        MONITOR_DEFAULTTONEAREST
                    );

                for (
                    size_t i = 0;
                    i < monitors.count;
                    ++i
                ) {
                    if (
                        monitors.entries[i].monitor ==
                        foregroundMonitor
                    ) {
                        result.applicationOnMonitor[i] = true;
                        break;
                    }
                }
            }
        }
    }
}

void RefreshTaskbarMonitorStates(
    const MonitorList& monitors
) {
    TaskbarMonitorState oldStates[kMaxTaskbars] = {};
    const size_t oldCount =
        g_taskbarStateCount;

    for (size_t i = 0; i < oldCount; ++i) {
        oldStates[i] =
            g_taskbarStates[i];
    }

    g_taskbarStateCount = 0;

    auto addTaskbar = [&](HWND hwnd) {
        if (
            !hwnd ||
            g_taskbarStateCount >= kMaxTaskbars
        ) {
            return;
        }

        HMONITOR monitor =
            MonitorFromWindow(
                hwnd,
                MONITOR_DEFAULTTONEAREST
            );

        TaskbarMonitorState state = {};
        state.hwnd = hwnd;
        state.monitor = monitor;
        state.monitorNumber =
            GetMonitorNumber(
                monitors,
                monitor
            );

        for (
            size_t monitorIndex = 0;
            monitorIndex < monitors.count;
            ++monitorIndex
        ) {
            if (
                monitors.entries[monitorIndex].monitor ==
                monitor
            ) {
                wcsncpy_s(
                    state.stableDeviceId,
                    ARRAYSIZE(state.stableDeviceId),
                    monitors.entries[monitorIndex].stableDeviceId,
                    _TRUNCATE
                );
                break;
            }
        }

        state.desktopOnly = true;
        state.hiddenByMod =
            GetPropW(
                hwnd,
                kHiddenByModProperty
            ) != nullptr;

        for (size_t i = 0; i < oldCount; ++i) {
            if (oldStates[i].hwnd == hwnd) {
                state.desktopOnly =
                    oldStates[i].desktopOnly;
                state.hiddenByMod =
                    oldStates[i].hiddenByMod ||
                    state.hiddenByMod;
                break;
            }
        }

        g_taskbarStates[
            g_taskbarStateCount++
        ] = state;
    };

    addTaskbar(
        FindWindowW(
            L"Shell_TrayWnd",
            nullptr
        )
    );

    HWND secondary = nullptr;

    while (
        (secondary = FindWindowExW(
            nullptr,
            secondary,
            L"Shell_SecondaryTrayWnd",
            nullptr
        )) != nullptr
    ) {
        addTaskbar(secondary);
    }
}

bool IsNativeAutoHideEnabled() {
    APPBARDATA data = {};
    data.cbSize = sizeof(data);

    return (SHAppBarMessage(ABM_GETSTATE, &data) & ABS_AUTOHIDE) != 0;
}

bool ShouldHideTaskbar(
    const TaskbarMonitorState& state
) {
    return
        state.hwnd &&
        state.monitor &&
        ShouldHideMonitor(state) &&
        IsBottomDockedTaskbar(
            state.hwnd,
            state.monitor
        ) &&
        !g_nativeAutoHideEnabled;
}


void SetTaskbarState(
    TaskbarMonitorState& state,
    bool show
) {
    if (
        !state.hwnd ||
        !IsWindow(state.hwnd)
    ) {
        return;
    }

    if (show) {
        if (!state.hiddenByMod) {
            return;
        }

        // Keep normal state transitions synchronous. Explorer can immediately
        // re-show a taskbar while processing shell/minimize transitions; using
        // ShowWindowAsync here can leave several SHOW/HIDE requests queued and
        // cause a visible flicker loop.
        ShowWindow(
            state.hwnd,
            SW_SHOW
        );

        if (IsWindowVisible(state.hwnd)) {
            RemovePropW(
                state.hwnd,
                kHiddenByModProperty
            );
            state.hiddenByMod = false;
        }

        return;
    }

    if (!IsWindowVisible(state.hwnd)) {
        state.hiddenByMod =
            state.hiddenByMod ||
            GetPropW(
                state.hwnd,
                kHiddenByModProperty
            ) != nullptr;
        return;
    }

    if (!SetPropW(
            state.hwnd,
            kHiddenByModProperty,
            reinterpret_cast<HANDLE>(
                static_cast<ULONG_PTR>(
                    GetCurrentProcessId()
                )
            )
        )) {
        Wh_Log(
            L"SetPropW failed for taskbar 0x%p: %lu",
            state.hwnd,
            GetLastError()
        );
    }

    ShowWindow(
        state.hwnd,
        SW_HIDE
    );

    if (!IsWindowVisible(state.hwnd)) {
        state.hiddenByMod = true;
    } else {
        RemovePropW(
            state.hwnd,
            kHiddenByModProperty
        );
        state.hiddenByMod = false;
    }
}

void ApplyBaseTaskbarState() {
    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        TaskbarMonitorState& state =
            g_taskbarStates[i];

        SetTaskbarState(
            state,
            !state.desktopOnly ||
            !ShouldHideTaskbar(state)
        );
    }
}

int GetHoverZonePx(
    HWND hTaskbar,
    UINT dpi
) {
    RECT rect = {};

    if (
        hTaskbar &&
        GetWindowRect(
            hTaskbar,
            &rect
        )
    ) {
        int taskbarHeight =
            rect.bottom - rect.top;

        if (taskbarHeight > 0) {
            return
                taskbarHeight +
                MulDiv(
                    g_settings.extraHoverMarginPx,
                    static_cast<int>(dpi),
                    96
                );
        }
    }

    return
        MulDiv(
            48,
            static_cast<int>(dpi),
            96
        ) +
        MulDiv(
            g_settings.extraHoverMarginPx,
            static_cast<int>(dpi),
            96
        );
}

bool IsBottomDockedTaskbar(
    HWND hTaskbar,
    HMONITOR monitor
) {
    if (!hTaskbar || !monitor) {
        return false;
    }

    RECT taskbarRect = {};

    if (!GetWindowRect(
            hTaskbar,
            &taskbarRect
        )) {
        return false;
    }

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);

    if (!GetMonitorInfoW(
            monitor,
            &mi
        )) {
        return false;
    }

    const LONG tolerance = 4;

    const LONG monitorWidth =
        mi.rcMonitor.right -
        mi.rcMonitor.left;

    const LONG monitorHeight =
        mi.rcMonitor.bottom -
        mi.rcMonitor.top;

    const LONG taskbarWidth =
        taskbarRect.right -
        taskbarRect.left;

    const LONG taskbarHeight =
        taskbarRect.bottom -
        taskbarRect.top;

    return
        taskbarRect.bottom >=
            mi.rcMonitor.bottom - tolerance &&
        taskbarRect.top >
            mi.rcMonitor.top &&
        taskbarWidth >=
            monitorWidth / 2 &&
        taskbarHeight <
            monitorHeight / 2;
}

bool IsPointNearBottomEdge(
    HWND hTaskbar,
    HMONITOR cursorMonitor,
    POINT pt
) {
    if (!hTaskbar || !cursorMonitor) {
        return false;
    }

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);

    if (!GetMonitorInfoW(
            cursorMonitor,
            &mi
        )) {
        return false;
    }

    if (!IsBottomDockedTaskbar(
            hTaskbar,
            cursorMonitor
        )) {
        return false;
    }

    UINT dpi =
        GetDpiForWindow(hTaskbar);

    if (dpi == 0) {
        dpi = 96;
    }

    int hotZonePx =
        GetHoverZonePx(
            hTaskbar,
            dpi
        );

    if (hotZonePx < 1) {
        hotZonePx = 1;
    }

    return
        pt.y >=
            mi.rcMonitor.bottom - hotZonePx &&
        pt.y < mi.rcMonitor.bottom;
}

bool IsCursorNearBottomEdge(
    HWND hTaskbar,
    HMONITOR cursorMonitor
) {
    POINT pt = {};

    if (!GetCursorPos(&pt)) {
        return false;
    }

    return IsPointNearBottomEdge(
        hTaskbar,
        cursorMonitor,
        pt
    );
}

void UpdateCursorHoverSnapshot() {
    CursorHoverSnapshot snapshots[kMaxTaskbars] = {};
    size_t snapshotCount = 0;

    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        if (snapshotCount >= kMaxTaskbars) {
            break;
        }

        const TaskbarMonitorState& state = g_taskbarStates[i];

        if (!ShouldRevealOnHover(state) ||
            !IsBottomDockedTaskbar(state.hwnd, state.monitor)) {
            continue;
        }

        CursorHoverSnapshot& snapshot = snapshots[snapshotCount++];
        snapshot.monitor = state.monitor;
        snapshot.enabled = true;

        MONITORINFO mi = {};
        mi.cbSize = sizeof(mi);

        if (!snapshot.monitor || !GetMonitorInfoW(snapshot.monitor, &mi)) {
            --snapshotCount;
            continue;
        }

        snapshot.monitorRect = mi.rcMonitor;

        UINT dpi = GetDpiForWindow(state.hwnd);

        if (dpi == 0) {
            dpi = 96;
        }

        snapshot.hotZonePx = GetHoverZonePx(state.hwnd, dpi);

        if (snapshot.hotZonePx < 1) {
            snapshot.hotZonePx = 1;
        }
    }

    AcquireSRWLockExclusive(&g_cursorHoverSnapshotLock);

    for (size_t i = 0; i < snapshotCount; ++i) {
        g_cursorHoverSnapshots[i] = snapshots[i];
    }

    g_cursorHoverSnapshotCount = snapshotCount;

    ReleaseSRWLockExclusive(&g_cursorHoverSnapshotLock);
}

bool IsCursorInConfiguredHoverZoneAtSnapshot(
    POINT pt,
    HMONITOR cursorMonitor
) {
    if (!cursorMonitor) {
        return false;
    }

    AcquireSRWLockShared(
        &g_cursorHoverSnapshotLock
    );

    bool result = false;

    for (
        size_t i = 0;
        i < g_cursorHoverSnapshotCount;
        ++i
    ) {
        const CursorHoverSnapshot& snapshot =
            g_cursorHoverSnapshots[i];

        if (
            !snapshot.enabled ||
            snapshot.monitor != cursorMonitor
        ) {
            continue;
        }

        result =
            pt.y >=
                snapshot.monitorRect.bottom -
                snapshot.hotZonePx &&
            pt.y < snapshot.monitorRect.bottom;
        break;
    }

    ReleaseSRWLockShared(
        &g_cursorHoverSnapshotLock
    );

    return result;
}

void UpdateTaskbarState() {
    MonitorList monitors =
        GetCurrentMonitors();

    ULONGLONG topologySignature =
        HashDisplayTopology(monitors);

    if (
        g_displayTopologySignature != 0 &&
        topologySignature != g_displayTopologySignature
    ) {
        ClearStableMonitorDeviceIdCache();

        // Rebuild the monitor list after clearing the cache so every current
        // monitor gets a fresh stable device identity before selection binding.
        monitors = GetCurrentMonitors();
        topologySignature = HashDisplayTopology(monitors);

        // A display add/remove, arrangement change, or geometry/DPI transition
        // can invalidate the current hover monitor. Reconcile from the base
        // state instead of carrying old hover state across the transition.
        g_hoverActive = false;
        g_hoverMonitor = nullptr;
        g_hoverDeadline = 0;
        CancelHoverExpireTimer();
    }

    g_displayTopologySignature = topologySignature;

    BindConfiguredMonitorSelections(
        monitors
    );

    RefreshTaskbarMonitorStates(
        monitors
    );

    UpdateCursorHoverSnapshot();

    // ABM_GETSTATE reports the native auto-hide setting globally. Sample it
    // once per reconciliation and reuse the result for every taskbar.
    g_nativeAutoHideEnabled =
        IsNativeAutoHideEnabled();

    WindowScanResult scan = {};

    ScanWindowsOnce(
        monitors,
        scan
    );

    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        TaskbarMonitorState& state =
            g_taskbarStates[i];

        state.desktopOnly = true;

        for (
            size_t monitorIndex = 0;
            monitorIndex < monitors.count;
            ++monitorIndex
        ) {
            if (
                monitors.entries[monitorIndex].monitor ==
                state.monitor
            ) {
                state.desktopOnly =
                    !scan.applicationOnMonitor[
                        monitorIndex
                    ];
                break;
            }
        }
    }

    POINT cursorPoint = {};
    HMONITOR cursorMonitor = nullptr;

    if (GetCursorPos(&cursorPoint)) {
        cursorMonitor =
            MonitorFromPoint(
                cursorPoint,
                MONITOR_DEFAULTTONEAREST
            );

    }

    HWND cursorTaskbar = nullptr;
    bool cursorHoverConfigured = false;

    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        if (
            g_taskbarStates[i].monitor !=
            cursorMonitor
        ) {
            continue;
        }

        cursorTaskbar =
            g_taskbarStates[i].hwnd;
        cursorHoverConfigured =
            ShouldRevealOnHover(
                g_taskbarStates[i]
            );
        break;
    }

    const bool cursorInHoverZone =
        cursorTaskbar &&
        cursorMonitor &&
        cursorHoverConfigured &&
        IsCursorNearBottomEdge(
            cursorTaskbar,
            cursorMonitor
        );

    const bool hovering =
        cursorInHoverZone;

    auto shellSurfaceOnMonitor =
        [&](HMONITOR monitor) {
            for (
                size_t monitorIndex = 0;
                monitorIndex < monitors.count;
                ++monitorIndex
            ) {
                if (
                    monitors.entries[monitorIndex].monitor ==
                    monitor
                ) {
                    return scan.shellSurfaceOnMonitor[
                        monitorIndex
                    ];
                }
            }

            return false;
        };

    if (hovering) {
        g_hoverActive = true;
        g_hoverMonitor = cursorMonitor;
        g_hoverDeadline = 0;
        CancelHoverExpireTimer();

        for (size_t i = 0; i < g_taskbarStateCount; ++i) {
            TaskbarMonitorState& state =
                g_taskbarStates[i];

            SetTaskbarState(
                state,
                state.monitor == g_hoverMonitor ||
                shellSurfaceOnMonitor(state.monitor) ||
                !state.desktopOnly ||
                !ShouldHideTaskbar(state)
            );
        }

        return;
    }

    if (g_hoverActive) {
        const ULONGLONG now =
            GetTickCount64();

        if (g_hoverDeadline == 0) {
            g_hoverDeadline =
                now +
                g_settings.autoHideDelayMs;

            ArmHoverExpireTimer();
        }

        if (now < g_hoverDeadline) {
            for (size_t i = 0; i < g_taskbarStateCount; ++i) {
                TaskbarMonitorState& state =
                    g_taskbarStates[i];

                SetTaskbarState(
                    state,
                    state.monitor == g_hoverMonitor ||
                    shellSurfaceOnMonitor(state.monitor) ||
                    !state.desktopOnly ||
                    !ShouldHideTaskbar(state)
                );
            }

            return;
        }

        g_hoverActive = false;
        g_hoverMonitor = nullptr;
        g_hoverDeadline = 0;

        CancelHoverExpireTimer();

        ApplyBaseTaskbarState();
        return;
    }

    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        TaskbarMonitorState& state =
            g_taskbarStates[i];

        SetTaskbarState(
            state,
            shellSurfaceOnMonitor(state.monitor) ||
            !state.desktopOnly ||
            !ShouldHideTaskbar(state)
        );
    }
}

void ArmHoverExpireTimer() {
    if (!g_workerMessageWindow) {
        return;
    }

    UINT delay =
        g_settings.autoHideDelayMs == 0
            ? 1
            : g_settings.autoHideDelayMs;

    SetTimer(
        g_workerMessageWindow,
        kHoverExpireTimerId,
        delay,
        nullptr
    );
}

void CancelHoverExpireTimer() {
    if (g_workerMessageWindow) {
        KillTimer(
            g_workerMessageWindow,
            kHoverExpireTimerId
        );
    }
}

bool IsTaskbarWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return false;
    }

    WCHAR className[128] = {};

    if (
        GetClassNameW(
            hwnd,
            className,
            ARRAYSIZE(className)
        ) == 0
    ) {
        return false;
    }

    return
        wcscmp(className, L"Shell_TrayWnd") == 0 ||
        wcscmp(className, L"Shell_SecondaryTrayWnd") == 0;
}

bool IsCursorInConfiguredHoverZoneAtPoint(
    POINT pt,
    HMONITOR cursorMonitor
) {
    if (!cursorMonitor) {
        return false;
    }

    TaskbarMonitorState* cursorState = nullptr;

    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        if (
            g_taskbarStates[i].monitor ==
            cursorMonitor
        ) {
            cursorState = &g_taskbarStates[i];
            break;
        }
    }

    if (
        !cursorState ||
        !ShouldRevealOnHover(*cursorState)
    ) {
        return false;
    }

    return
        cursorState->hwnd &&
        IsPointNearBottomEdge(
            cursorState->hwnd,
            cursorMonitor,
            pt
        );
}

bool IsCursorInConfiguredHoverZone() {
    POINT pt = {};

    if (!GetCursorPos(&pt)) {
        return false;
    }

    HMONITOR cursorMonitor =
        MonitorFromPoint(
            pt,
            MONITOR_DEFAULTTONEAREST
        );

    return IsCursorInConfiguredHoverZoneAtPoint(
        pt,
        cursorMonitor
    );
}


void SafeUnhookWinEvent(HWINEVENTHOOK& hook) {
    if (hook) {
        UnhookWinEvent(hook);
        hook = nullptr;
    }
}

void SafeCloseHandle(HANDLE& handle) {
    if (handle) {
        CloseHandle(handle);
        handle = nullptr;
    }
}

void PostRefresh() {
    if (
        InterlockedExchange(
            &g_refreshPosted,
            1
        ) != 0
    ) {
        return;
    }

    if (!PostThreadMessageW(
            g_workerThreadId,
            WM_APP_REFRESH,
            0,
            0
        )) {
        InterlockedExchange(
            &g_refreshPosted,
            0
        );
    }
}

bool HasEnabledHoverSnapshot() {
    AcquireSRWLockShared(
        &g_cursorHoverSnapshotLock
    );

    bool result = false;

    for (size_t i = 0; i < g_cursorHoverSnapshotCount; ++i) {
        if (g_cursorHoverSnapshots[i].enabled) {
            result = true;
            break;
        }
    }

    ReleaseSRWLockShared(
        &g_cursorHoverSnapshotLock
    );

    return result;
}

DWORD WINAPI CursorSamplingThread(LPVOID) {
    bool lastHoverZone = false;
    HMONITOR lastMonitor = nullptr;
    int cursorPositionFailures = 0;

    for (;;) {
        const bool hoverTrackingActive =
            HasEnabledHoverSnapshot();

        DWORD waitMs =
            hoverTrackingActive
                ? 25
                : 100;

        if (cursorPositionFailures >= 3) {
            waitMs = 1000;
        }

        DWORD waitResult =
            WaitForSingleObject(
                g_cursorStopEvent,
                waitMs
            );

        if (waitResult == WAIT_OBJECT_0) {
            break;
        }

        POINT pt = {};

        if (!GetCursorPos(&pt)) {
            ++cursorPositionFailures;
            continue;
        }

        cursorPositionFailures = 0;

        HMONITOR monitor =
            MonitorFromPoint(
                pt,
                MONITOR_DEFAULTTONEAREST
            );

        const bool hoverZone =
            IsCursorInConfiguredHoverZoneAtSnapshot(
                pt,
                monitor
            );

        const bool changed =
            hoverZone != lastHoverZone ||
            (hoverZone && monitor != lastMonitor);

        lastHoverZone = hoverZone;
        lastMonitor = monitor;

        if (changed) {
            PostRefresh();
        }
    }

    return 0;
}

void CALLBACK WinEventProc(
    HWINEVENTHOOK,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG,
    DWORD,
    DWORD
) {
    /*
     * Explorer can re-show a secondary taskbar while the user interacts with
     * the primary taskbar or other shell UI. Handle only SHOW for the exact
     * taskbar window classes. The callback never changes Explorer windows
     * directly; it only schedules the serialized worker update. We intentionally
     * do not watch HIDE.
     */
    if (
        event == EVENT_OBJECT_SHOW &&
        idObject == OBJID_WINDOW &&
        IsTaskbarWindow(hwnd)
    ) {
        /*
         * While the cursor is already inside the configured hover zone,
         * Explorer may emit SHOW for more than one taskbar during the same
         * shell transition. The worker's hover calculation is authoritative,
         * so don't enqueue an additional refresh that can immediately undo
         * and repeat the hover visibility transition.
         *
         * Outside the hover zone the narrow SHOW hook is retained so an
         * Explorer re-show is reconciled promptly.
         */
        if (IsCursorInConfiguredHoverZone()) {
            return;
        }

        PostRefresh();
        return;
    }

    if (
        event != EVENT_SYSTEM_MINIMIZESTART &&
        event != EVENT_SYSTEM_MINIMIZEEND &&
        event != EVENT_SYSTEM_MOVESIZEEND
    ) {
        return;
    }

    PostRefresh();
}


void EnsureTaskbarShowHook() {
    HWND taskbar =
        FindWindowW(
            L"Shell_TrayWnd",
            nullptr
        );

    DWORD explorerPid = 0;
    if (taskbar) {
        GetWindowThreadProcessId(
            taskbar,
            &explorerPid
        );
    }

    if (explorerPid == g_taskbarShowHookProcessId) {
        return;
    }

    SafeUnhookWinEvent(g_taskbarShowHook);
    g_taskbarShowHookProcessId = 0;

    if (explorerPid) {
        g_taskbarShowHook = SetWinEventHook(
            EVENT_OBJECT_SHOW,
            EVENT_OBJECT_SHOW,
            nullptr,
            WinEventProc,
            explorerPid,
            0,
            WINEVENT_OUTOFCONTEXT
        );

        if (g_taskbarShowHook) {
            g_taskbarShowHookProcessId = explorerPid;
        }
    }
}



LRESULT CALLBACK WorkerMessageWindowProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
) {
    if (
        message == WM_TIMER &&
        wParam == kHoverExpireTimerId
    ) {
        KillTimer(hwnd, kHoverExpireTimerId);
        PostRefresh();
        return 0;
    }

    if (
        message == g_taskbarCreatedMessage ||
        message == WM_DISPLAYCHANGE ||
        message == WM_SETTINGCHANGE ||
        message == WM_THEMECHANGED
    ) {
        PostRefresh();
        return 0;
    }

    return DefWindowProcW(
        hwnd,
        message,
        wParam,
        lParam
    );
}

bool CreateWorkerMessageWindow() {
    g_taskbarCreatedMessage =
        RegisterWindowMessageW(L"TaskbarCreated");

    if (!g_taskbarCreatedMessage) {
        Wh_Log(L"RegisterWindowMessage(TaskbarCreated) failed: %lu", GetLastError());
        return false;
    }

    const wchar_t* kClassName =
        L"WindhawkHideTaskbarOnlyOnDesktopMessageWindow";

    HINSTANCE instance =
        GetModuleHandleW(nullptr);

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.hInstance = instance;
    wc.lpfnWndProc = WorkerMessageWindowProc;
    wc.lpszClassName = kClassName;

    g_workerWindowClassAtom =
        RegisterClassExW(&wc);

    if (!g_workerWindowClassAtom) {
        Wh_Log(
            L"RegisterClassExW failed: %lu",
            GetLastError()
        );
        return false;
    }

    g_workerMessageWindow =
        CreateWindowExW(
            WS_EX_TOOLWINDOW,
            kClassName,
            L"",
            WS_POPUP,
            0,
            0,
            0,
            0,
            nullptr,
            nullptr,
            instance,
            nullptr
        );

    if (!g_workerMessageWindow) {
        Wh_Log(L"CreateWindowExW(message window) failed: %lu", GetLastError());

        UnregisterClassW(
            kClassName,
            instance
        );
        g_workerWindowClassAtom = 0;

        return false;
    }

    return true;
}

void DestroyWorkerMessageWindow() {
    if (g_workerMessageWindow) {
        DestroyWindow(g_workerMessageWindow);
        g_workerMessageWindow = nullptr;
    }

    if (g_workerWindowClassAtom) {
        const wchar_t* kClassName =
            L"WindhawkHideTaskbarOnlyOnDesktopMessageWindow";

        UnregisterClassW(
            kClassName,
            GetModuleHandleW(nullptr)
        );

        g_workerWindowClassAtom = 0;
    }

    g_taskbarCreatedMessage = 0;
}

void UpdateSafetyTimer(UINT_PTR timerId) {
    if (!timerId) {
        return;
    }

    constexpr UINT kSafetyPollIntervalMs = 1000;

    SetTimer(
        nullptr,
        timerId,
        kSafetyPollIntervalMs,
        nullptr
    );
}

DWORD WINAPI WorkerThread(
    LPVOID
) {
    MSG msg = {};

    PeekMessageW(
        &msg,
        nullptr,
        WM_USER,
        WM_USER,
        PM_NOREMOVE
    );

    if (g_workerReadyEvent) {
        SetEvent(
            g_workerReadyEvent
        );
    }

    if (!CreateWorkerMessageWindow()) {
        Wh_Log(
            L"CreateWorkerMessageWindow failed; continuing with WinEvent/safety-poll handling"
        );
    }

    g_foregroundHook =
        SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND,
            EVENT_SYSTEM_FOREGROUND,
            nullptr,
            WinEventProc,
            0,
            0,
            WINEVENT_OUTOFCONTEXT
        );

    g_minimizeHook =
        SetWinEventHook(
            EVENT_SYSTEM_MINIMIZESTART,
            EVENT_SYSTEM_MINIMIZEEND,
            nullptr,
            WinEventProc,
            0,
            0,
            WINEVENT_OUTOFCONTEXT
        );

    g_moveHook =
        SetWinEventHook(
            EVENT_SYSTEM_MOVESIZEEND,
            EVENT_SYSTEM_MOVESIZEEND,
            nullptr,
            WinEventProc,
            0,
            0,
            WINEVENT_OUTOFCONTEXT
        );

    EnsureTaskbarShowHook();
    UpdateTaskbarState();

    UINT_PTR timerId =
        SetTimer(
            nullptr,
            kSafetyTimerId,
            1000,
            nullptr
        );

    for (;;) {
        BOOL result =
            GetMessageW(
                &msg,
                nullptr,
                0,
                0
            );

        if (result <= 0) {
            break;
        }

        if (msg.message == WM_TIMER) {
            if (msg.hwnd && msg.hwnd == g_workerMessageWindow) {
                DispatchMessageW(&msg);
                continue;
            }

            EnsureTaskbarShowHook();
            UpdateTaskbarState();
            UpdateSafetyTimer(timerId);

            continue;
        }

        if (msg.message == WM_APP_REFRESH) {
            InterlockedExchange(
                &g_refreshPosted,
                0
            );

            EnsureTaskbarShowHook();
            UpdateTaskbarState();

            UpdateSafetyTimer(timerId);

            continue;
        }

        if (msg.message == WM_APP_SETTINGS) {
            LoadSettings();
            EnsureTaskbarShowHook();
            UpdateTaskbarState();

            UpdateSafetyTimer(timerId);

            continue;
        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);

        UpdateSafetyTimer(timerId);
    }

    if (timerId) {
        KillTimer(
            nullptr,
            timerId
        );
    }

    CancelHoverExpireTimer();

    SafeUnhookWinEvent(g_foregroundHook);
    SafeUnhookWinEvent(g_minimizeHook);
    SafeUnhookWinEvent(g_moveHook);
    SafeUnhookWinEvent(g_taskbarShowHook);
    g_taskbarShowHookProcessId = 0;

    DestroyWorkerMessageWindow();

    return 0;
}

void LoadSettings() {
    ResetMonitorSelectionBindings();

    int hoverMargin =
        Wh_GetIntSetting(
            L"extraHoverMarginPx"
        );

    if (hoverMargin < 0) {
        hoverMargin = 0;
    } else if (hoverMargin > 200) {
        hoverMargin = 200;
    }

    g_settings.extraHoverMarginPx =
        hoverMargin;

    int delay =
        Wh_GetIntSetting(
            L"autoHideDelayMs"
        );

    if (delay < 0) {
        delay = 0;
    } else if (delay > 10000) {
        delay = 10000;
    }

    g_settings.autoHideDelayMs =
        static_cast<DWORD>(delay);

    g_settings.hideAllMonitors = false;

    for (
        size_t i = 1;
        i <= kMaxMonitorNumbers;
        ++i
    ) {
        g_settings.hideMonitor[i] = false;
        g_settings.hoverMonitor[i] = false;
    }

    g_settings.hoverAllMonitors = false;

    for (
        size_t i = 0;
        i < kMaxMonitorNumbers;
        ++i
    ) {
        auto value =
            WindhawkUtils::StringSetting::make(
                L"hideOnMonitors[%d]",
                static_cast<int>(i)
            );

        if (!*value) {
            break;
        }

        if (wcscmp(value, L"all") == 0) {
            g_settings.hideAllMonitors = true;
        } else if (
            wcsncmp(
                value,
                L"monitor",
                7
            ) == 0
        ) {
            wchar_t* endNumber = nullptr;

            long number =
                wcstol(
                    value + 7,
                    &endNumber,
                    10
                );

            if (
                endNumber &&
                *endNumber == L'\0' &&
                number >= 1 &&
                number <= static_cast<long>(
                    kMaxMonitorNumbers
                )
            ) {
                g_settings.hideMonitor[number] = true;
            }
        }

    }

    // Empty selection intentionally means "hide nothing".
    // "All displays" is handled independently of DISPLAYn parsing so it still
    // applies if Windows reports an unexpected device-number format.

    for (
        size_t i = 0;
        i < kMaxMonitorNumbers;
        ++i
    ) {
        auto value =
            WindhawkUtils::StringSetting::make(
                L"hoverRevealOnMonitors[%d]",
                static_cast<int>(i)
            );

        if (!*value) {
            break;
        }

        if (wcscmp(value, L"all") == 0) {
            g_settings.hoverAllMonitors = true;
        } else if (
            wcsncmp(
                value,
                L"monitor",
                7
            ) == 0
        ) {
            wchar_t* endNumber = nullptr;

            long number =
                wcstol(
                    value + 7,
                    &endNumber,
                    10
                );

            if (
                endNumber &&
                *endNumber == L'\0' &&
                number >= 1 &&
                number <= static_cast<long>(
                    kMaxMonitorNumbers
                )
            ) {
                g_settings.hoverMonitor[number] = true;
            }
        }

    }
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Init");

    // Load the persisted Windhawk settings before the worker performs its
    // first state calculation. Without this, g_settings remains zeroed and
    // ShouldHideMonitor() returns false even when the UI says "All displays".
    LoadSettings();

    g_workerReadyEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr
        );

    if (!g_workerReadyEvent) {
        Wh_Log(L"CreateEvent failed");
        return FALSE;
    }

    g_workerThread =
        CreateThread(
            nullptr,
            0,
            WorkerThread,
            nullptr,
            0,
            &g_workerThreadId
        );

    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed");

        SafeCloseHandle(g_workerReadyEvent);
        return FALSE;
    }

    DWORD readyResult =
        WaitForSingleObject(
            g_workerReadyEvent,
            5000
        );

    if (readyResult != WAIT_OBJECT_0) {
        Wh_Log(
            L"Worker startup wait failed: %lu",
            readyResult == WAIT_TIMEOUT
                ? ERROR_TIMEOUT
                : GetLastError()
        );

        RestoreAllTaskbars();

        if (!WaitForThreadWithTimeout(
                g_workerThread,
                5000,
                L"worker"
            )) {
            ExitProcess(1);
        }

        SafeCloseHandle(g_workerThread);
        SafeCloseHandle(g_workerReadyEvent);

        return FALSE;
    }

    g_cursorStopEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr
        );

    if (!g_cursorStopEvent) {
        Wh_Log(
            L"CreateEvent for cursor sampler failed: %lu",
            GetLastError()
        );
        return TRUE;
    }

    g_cursorThread =
        CreateThread(
            nullptr,
            0,
            CursorSamplingThread,
            nullptr,
            0,
            nullptr
        );

    if (!g_cursorThread) {
        Wh_Log(
            L"CreateThread for cursor sampler failed: %lu",
            GetLastError()
        );
        SafeCloseHandle(g_cursorStopEvent);
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_workerThread) {
        PostThreadMessageW(
            g_workerThreadId,
            WM_APP_SETTINGS,
            0,
            0
        );
    }
}

void RestoreAllTaskbars() {
    auto restoreTaskbarIfMarked = [](HWND hwnd) {
        if (
            !hwnd ||
            !IsWindow(hwnd) ||
            !GetPropW(
                hwnd,
                kHiddenByModProperty
            )
        ) {
            return;
        }

        /*
         * Teardown/recovery is intentionally asynchronous. This path can run
         * during tool shutdown, where blocking on Explorer's UI thread
         * could otherwise hang Windhawk itself.
         */
        ShowWindowAsync(
            hwnd,
            SW_SHOW
        );

        RemovePropW(
            hwnd,
            kHiddenByModProperty
        );
    };

    restoreTaskbarIfMarked(
        FindWindowW(
            L"Shell_TrayWnd",
            nullptr
        )
    );

    HWND secondary = nullptr;

    while (
        (secondary = FindWindowExW(
            nullptr,
            secondary,
            L"Shell_SecondaryTrayWnd",
            nullptr
        )) != nullptr
    ) {
        restoreTaskbarIfMarked(secondary);
    }
}

bool WaitForThreadWithTimeout(
    HANDLE thread,
    DWORD timeoutMs,
    const wchar_t* threadName
) {
    if (!thread) {
        return true;
    }

    DWORD result =
        WaitForSingleObject(
            thread,
            timeoutMs
        );

    if (result == WAIT_OBJECT_0) {
        return true;
    }

    if (result == WAIT_TIMEOUT) {
        Wh_Log(
            L"Timed out waiting for %s thread shutdown; terminating the dedicated tool process",
            threadName ? threadName : L"worker"
        );
    } else {
        Wh_Log(
            L"Wait for %s thread failed: %lu; terminating the dedicated tool process",
            threadName ? threadName : L"worker",
            GetLastError()
        );
    }

    /*
     * The caller must restore any taskbars hidden by this mod and terminate the
     * dedicated process immediately if a worker remains stuck. Returning here
     * is safe only because the caller handles that termination.
     */
    return false;
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_cursorStopEvent) {
        SetEvent(g_cursorStopEvent);
    }

    if (g_cursorThread) {
        if (!WaitForThreadWithTimeout(
                g_cursorThread,
                3000,
                L"cursor sampler"
            )) {
            RestoreAllTaskbars();
            ExitProcess(1);
        }

        SafeCloseHandle(g_cursorThread);
    }

    SafeCloseHandle(g_cursorStopEvent);

    if (g_workerThread) {
        if (!PostThreadMessageW(
                g_workerThreadId,
                WM_QUIT,
                0,
                0
            )) {
            Wh_Log(
                L"PostThreadMessageW(WM_QUIT) failed: %lu",
                GetLastError()
            );
        }

        if (!WaitForThreadWithTimeout(
                g_workerThread,
                5000,
                L"worker"
            )) {
            RestoreAllTaskbars();
            ExitProcess(1);
        }

        SafeCloseHandle(g_workerThread);
    }

    if (g_workerReadyEvent) {
        SafeCloseHandle(g_workerReadyEvent);
    }

    /*
     * Restore all currently discoverable taskbars when the tool exits.
     */
    RestoreAllTaskbars();
}

////////////////////////////////////////////////////////////////////////////////
// Explorer is a special target for this hybrid mod. The standard tool-mod
// launcher is kept below as the documented Windhawk boilerplate; the Explorer
// branch wraps around it without changing the tool-process logic.

bool IsCurrentProcessExplorer() {
    WCHAR modulePath[MAX_PATH] = {};

    DWORD length =
        GetModuleFileNameW(
            nullptr,
            modulePath,
            ARRAYSIZE(modulePath)
        );

    if (
        length == 0 ||
        length >= ARRAYSIZE(modulePath)
    ) {
        return false;
    }

    const WCHAR* moduleName =
        wcsrchr(
            modulePath,
            L'\\'
        );

    moduleName =
        moduleName
            ? moduleName + 1
            : modulePath;

    return _wcsicmp(
        moduleName,
        L"explorer.exe"
    ) == 0;
}

#define Wh_ModInit WindhawkToolModLauncher_Wh_ModInit
#define Wh_ModAfterInit WindhawkToolModLauncher_Wh_ModAfterInit
#define Wh_ModSettingsChanged WindhawkToolModLauncher_Wh_ModSettingsChanged
#define Wh_ModUninit WindhawkToolModLauncher_Wh_ModUninit

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isService = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isService) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}

#undef Wh_ModInit
#undef Wh_ModAfterInit
#undef Wh_ModSettingsChanged
#undef Wh_ModUninit

BOOL Wh_ModInit() {
    if (IsCurrentProcessExplorer()) {
        g_isExplorerProcess = true;
        if (!InstallExplorerVisibilityHook()) {
            return FALSE;
        }

        RecoverStaleHiddenTaskbars();
        return TRUE;
    }

    return WindhawkToolModLauncher_Wh_ModInit();
}

void Wh_ModAfterInit() {
    if (g_isExplorerProcess) {
        return;
    }

    WindhawkToolModLauncher_Wh_ModAfterInit();
}

void Wh_ModSettingsChanged() {
    if (g_isExplorerProcess) {
        return;
    }

    WindhawkToolModLauncher_Wh_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isExplorerProcess) {
        ResetHiddenTaskbarOwnerCache();
        return;
    }

    WindhawkToolModLauncher_Wh_ModUninit();
}


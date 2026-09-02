// ==WindhawkMod==
// @id              hide-taskbar-only-on-desktop
// @name            Hide Taskbar Only on Desktop
// @description     Hides selected taskbars only while their display is showing the desktop
// @version         3.4.0
// @author          Sahil Dashoni
// @github          https://github.com/Sahil-Dashoni
// @include         windhawk.exe
// @compilerOptions -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Taskbar Only on Desktop

Hides each selected taskbar when its own display has no visible,
non-minimized application window, and shows it again when an application or
relevant Windows shell interaction requires it.

The mod is implemented as a Windhawk tool in a dedicated `windhawk.exe` process
instead of being injected into `explorer.exe`. This avoids one worker copy
per Explorer process and keeps the state-management code outside the shell.

### Behavior

- Each display is evaluated independently.
- A selected taskbar is hidden only while its display has no visible,
  non-minimized application window.
- Maximized windows use Windows' monitor assignment. Normal windows that
  span displays count on every display they actually intersect.
- Bottom-edge hover reveal is selectable per display. The reveal zone uses
  the taskbar's actual height and DPI. Hover reveal is intentionally limited
  to bottom-docked taskbars; non-bottom-docked taskbars are not affected by
  the hover logic.
- The configured post-hover delay applies only to hover reveal.
- Start/taskbar menus, tray overflow, notification/quick-settings surfaces,
  and Alt+Tab are treated as transient shell interactions.
- Clearing the "Taskbars to hide on desktop" selection means hide nothing.


### Robustness

- The tool re-discovers primary and secondary taskbars during every reconciliation,
  so Explorer taskbar recreation is picked up automatically.
- A message-only worker window reacts to `TaskbarCreated`, display-configuration,
  settings, and theme changes instead of waiting for the next safety-poll tick.
- Selected `DISPLAYn` entries are bound to the display's monitor device identity
  during the current Windows session, so a dock/reconnect that renumbers
  `DISPLAYn` can keep the selection attached to the same physical display.
- A display-topology signature resets stale hover state after monitor geometry or
  display-set changes.
- Native Windows auto-hide state is sampled once per reconciliation and reused
  for the rest of that reconciliation.
- Known taskbar/XAML popup classes are treated as shell surfaces; generic
  `Windows.UI.Core.CoreWindow` windows are no longer rejected solely because of
  their class name, which avoids misclassifying visible UWP application windows.

### How this differs from related taskbar mods

This mod overlaps with existing per-monitor taskbar mods in that each display
is evaluated independently. The key behavioral difference from
**`taskbar-auto-hide-when-maximized`** is the predicate used for hiding: this
mod keeps a selected taskbar visible whenever its display has any visible,
non-minimized application window, and hides it only when that display has no
such window. It is therefore not limited to maximized, snapped, fullscreen,
or taskbar-intersecting application states.

**`taskbar-auto-hide-per-monitor`** provides explicit per-monitor control over
Windows' native auto-hide. This mod instead derives visibility from application
presence and directly controls the taskbar window.

**`taskbar-auto-hide-custom-activation-area`** changes the activation area for
Windows' native auto-hide. This mod supplies its own bottom-edge hover reveal
while leaving the normal desktop work area unchanged.

**`taskbar-fade`** is a broader taskbar customization/fading mod with a Smart
Idle option. This mod is specifically focused on desktop-only visibility and
its associated per-display application-state rule.

A key implementation difference is the **work-area behavior**: the mod uses
`ShowWindow(SW_HIDE)` and does not change the Windows desktop work area.

The launcher keeps a lightweight watchdog for the dedicated tool process. If the
tool exits unexpectedly while a taskbar is hidden, the watchdog re-shows the
currently discoverable primary and secondary taskbars. A normal mod unload also
restores taskbars. If Explorer itself is unresponsive or has already recreated a
taskbar, restarting Explorer may still be required.

### Process and state model

The mod runs as a dedicated Windhawk tool process. A single worker thread
owns the mutable runtime state and settings. Window-state WinEvents request
an immediate reconciliation, while a periodic safety poll handles missed
or unusual transitions.

Each safety poll performs one display enumeration and one top-level-window
enumeration. The result is reused for every taskbar. Hover reveal uses a gated
cursor sampler only while a hidden taskbar can actually be revealed, and its
post-hover delay expires through a one-shot worker timer rather than a fast
full-state polling loop.
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
    Settings. Choose All displays to hide every connected display. Use Add to
    select multiple displays.
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
};

struct MonitorSelectionBinding {
    bool configured;
    wchar_t stableDeviceId[256];
};

struct WindowScanResult {
    bool applicationOnMonitor[kMaxMonitorNumbers];
    bool taskbarPopupOnMonitor[kMaxMonitorNumbers];
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
HANDLE g_cursorWakeEvent = nullptr;

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
LONG g_cursorSamplingEnabled = 0;

void LoadSettings();
void WhTool_ModUninit();
void UpdateCursorSamplingState();
void ArmHoverExpireTimer();
void CancelHoverExpireTimer();

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

bool GetStableMonitorDeviceId(
    const wchar_t* deviceName,
    wchar_t* output,
    size_t outputCount
);

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
                output,
                outputCount,
                monitor.DeviceID,
                _TRUNCATE
            );
            return output[0] != L'\0';
        }

        if (adapter.DeviceID[0] != L'\0') {
            wcsncpy_s(
                output,
                outputCount,
                adapter.DeviceID,
                _TRUNCATE
            );
            return output[0] != L'\0';
        }

        return false;
    }

    return false;
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

bool IsTransientShellWindow(
    HWND hwnd,
    const WCHAR* className
) {
    if (!hwnd || !className) {
        return false;
    }

    // Classic shell popup/menu windows.
    if (
        wcscmp(className, L"#32768") == 0 ||
        wcscmp(className, L"#32771") == 0
    ) {
        return true;
    }

    // Common Windows shell/XAML popup hosts.
    if (
        wcscmp(className, L"Xaml_WindowedPopupClass") == 0 ||
        wcscmp(className, L"TopLevelWindowForOverflowXamlIsland") == 0 ||
        wcscmp(
            className,
            L"NotifyIconOverflowWindow"
        ) == 0 ||
        wcscmp(
            className,
            L"TaskbarOverflowWnd"
        ) == 0
    ) {
        return true;
    }

    LONG_PTR exStyle =
        GetWindowLongPtrW(
            hwnd,
            GWL_EXSTYLE
        );

    // Do not let no-activate transient surfaces make a display look
    // application-active.
    return (exStyle & WS_EX_NOACTIVATE) != 0;
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
        GetWindow(hwnd, GW_OWNER) != nullptr &&
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

    /*
     * Background title-less helper surfaces are ignored. A title-less
     * application becomes covered by the foreground-window path.
     */
    return
        (exStyle & WS_EX_APPWINDOW) != 0 ||
        GetWindowTextLengthW(hwnd) > 0;
}

bool IsTaskbarPopupClass(
    const WCHAR* className
) {
    if (!className) {
        return false;
    }

    return
        wcscmp(className, L"#32768") == 0 ||
        wcscmp(className, L"Xaml_WindowedPopupClass") == 0 ||
        wcscmp(
            className,
            L"TopLevelWindowForOverflowXamlIsland"
        ) == 0 ||
        wcscmp(
            className,
            L"NotifyIconOverflowWindow"
        ) == 0 ||
        wcscmp(
            className,
            L"TaskbarOverflowWnd"
        ) == 0;
}

bool IsExplorerShellPopup(
    HWND hwnd,
    const WCHAR* className,
    DWORD explorerPid
) {
    if (
        !hwnd ||
        !className ||
        !IsWindowVisible(hwnd) ||
        !IsTaskbarPopupClass(className)
    ) {
        return false;
    }

    DWORD pid = 0;

    GetWindowThreadProcessId(
        hwnd,
        &pid
    );

    if (
        explorerPid != 0 &&
        pid == explorerPid
    ) {
        return true;
    }

    HWND shellWindow = GetShellWindow();
    DWORD shellPid = 0;

    if (shellWindow) {
        GetWindowThreadProcessId(
            shellWindow,
            &shellPid
        );
    }

    return
        shellPid != 0 &&
        pid == shellPid;
}

struct ScanContext {
    const MonitorList* monitors;
    WindowScanResult* result;
    DWORD explorerPid;
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

    if (IsExplorerShellPopup(
            hwnd,
            className,
            context->explorerPid
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
                    context->result->taskbarPopupOnMonitor[i] = true;
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

    DWORD explorerPid = 0;
    HWND primaryTaskbar =
        FindWindowW(
            L"Shell_TrayWnd",
            nullptr
        );

    if (primaryTaskbar) {
        GetWindowThreadProcessId(
            primaryTaskbar,
            &explorerPid
        );
    }

    ScanContext context = {
        &monitors,
        &result,
        explorerPid
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

        for (size_t i = 0; i < oldCount; ++i) {
            if (oldStates[i].hwnd == hwnd) {
                state.desktopOnly =
                    oldStates[i].desktopOnly;
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

    bool visible =
        IsWindowVisible(state.hwnd) != FALSE;

    if (visible != show) {
        // Keep the visibility transition synchronous. Explorer can immediately
        // re-show a taskbar while processing shell/minimize transitions; using
        // ShowWindowAsync here can leave several SHOW/HIDE requests queued and
        // cause a visible flicker loop. The worker is already serialized, so a
        // synchronous transition gives us the stable state machine behavior.
        ShowWindow(
            state.hwnd,
            show ? SW_SHOW : SW_HIDE
        );
    }
}

bool IsTaskbarHiddenByMod(
    const TaskbarMonitorState& state
) {
    return
        state.hwnd &&
        !IsWindowVisible(state.hwnd) &&
        ShouldHideTaskbar(state);
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

bool IsCursorNearBottomEdge(
    HWND hTaskbar,
    HMONITOR cursorMonitor
) {
    POINT pt = {};

    if (
        !hTaskbar ||
        !cursorMonitor ||
        !GetCursorPos(&pt)
    ) {
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

void UpdateTaskbarState() {
    MonitorList monitors =
        GetCurrentMonitors();

    BindConfiguredMonitorSelections(
        monitors
    );

    ULONGLONG topologySignature =
        HashDisplayTopology(monitors);

    if (
        g_displayTopologySignature != 0 &&
        topologySignature != g_displayTopologySignature
    ) {
        // A display add/remove, arrangement change, or geometry/DPI transition
        // can invalidate the current hover monitor. Reconcile from the base
        // state instead of carrying old hover state across the transition.
        g_hoverActive = false;
        g_hoverMonitor = nullptr;
        g_hoverDeadline = 0;
        CancelHoverExpireTimer();
    }

    g_displayTopologySignature = topologySignature;

    RefreshTaskbarMonitorStates(
        monitors
    );

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
    bool cursorTaskbarHiddenByMod = false;

    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        if (
            g_taskbarStates[i].monitor ==
                cursorMonitor
        ) {
            cursorTaskbar =
                g_taskbarStates[i].hwnd;
            cursorTaskbarHiddenByMod =
                IsTaskbarHiddenByMod(g_taskbarStates[i]);
            break;
        }
    }

    bool cursorHoverConfigured = false;

    for (
        size_t i = 0;
        i < g_taskbarStateCount;
        ++i
    ) {
        if (
            g_taskbarStates[i].monitor ==
            cursorMonitor
        ) {
            cursorHoverConfigured =
                ShouldRevealOnHover(
                    g_taskbarStates[i]
                );
            break;
        }
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
        cursorInHoverZone &&
        (
            cursorTaskbarHiddenByMod ||
            (g_hoverActive && g_hoverMonitor == cursorMonitor)
        );

    // Keep the taskbar visible for an open shell context menu/overflow
    // surface even after the cursor leaves the popup. Popup state was collected
    // during the main EnumWindows pass above, so no second full enumeration is
    // needed here.
    bool popupPresent = false;

    for (size_t monitorIndex = 0;
         monitorIndex < monitors.count;
         ++monitorIndex) {
        if (scan.taskbarPopupOnMonitor[monitorIndex]) {
            popupPresent = true;
            break;
        }
    }

    if (popupPresent) {
        g_hoverActive = false;
        g_hoverMonitor = nullptr;
        g_hoverDeadline = 0;
        CancelHoverExpireTimer();

        for (size_t i = 0; i < g_taskbarStateCount; ++i) {
            TaskbarMonitorState& state =
                g_taskbarStates[i];

            bool popupOnThisMonitor = false;

            for (size_t monitorIndex = 0;
                 monitorIndex < monitors.count;
                 ++monitorIndex) {
                if (
                    monitors.entries[monitorIndex].monitor ==
                        state.monitor &&
                    scan.taskbarPopupOnMonitor[monitorIndex]
                ) {
                    popupOnThisMonitor = true;
                    break;
                }
            }

            SetTaskbarState(
                state,
                popupOnThisMonitor ||
                !state.desktopOnly ||
                !ShouldHideTaskbar(state)
            );
        }

        UpdateCursorSamplingState();
        return;
    }

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
                !state.desktopOnly ||
                !ShouldHideTaskbar(state)
            );
        }

        UpdateCursorSamplingState();
        return;
    }

    if (g_hoverActive) {
        if (
            !g_hoverMonitor ||
            cursorMonitor != g_hoverMonitor
        ) {
            g_hoverActive = false;
            g_hoverMonitor = nullptr;
            g_hoverDeadline = 0;
            CancelHoverExpireTimer();

            ApplyBaseTaskbarState();
            UpdateCursorSamplingState();
            return;
        }

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
                    !state.desktopOnly ||
                    !ShouldHideTaskbar(state)
                );
            }

            UpdateCursorSamplingState();
            return;
        }

        g_hoverActive = false;
        g_hoverMonitor = nullptr;
        g_hoverDeadline = 0;

        CancelHoverExpireTimer();

        ApplyBaseTaskbarState();
        UpdateCursorSamplingState();
        return;
    }

    ApplyBaseTaskbarState();
    UpdateCursorSamplingState();
}

void UpdateCursorSamplingState() {
    bool shouldSample = g_hoverActive;

    if (!shouldSample) {
        for (size_t i = 0; i < g_taskbarStateCount; ++i) {
            const TaskbarMonitorState& state =
                g_taskbarStates[i];

            if (
                IsTaskbarHiddenByMod(state) &&
                ShouldRevealOnHover(state)
            ) {
                shouldSample = true;
                break;
            }
        }
    }

    LONG oldValue =
        InterlockedExchange(
            &g_cursorSamplingEnabled,
            shouldSample ? 1 : 0
        );

    if (shouldSample && oldValue == 0 && g_cursorWakeEvent) {
        SetEvent(g_cursorWakeEvent);
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

    if (!cursorMonitor) {
        return false;
    }

    MonitorList monitors =
        GetCurrentMonitors();

    TaskbarMonitorState* cursorState = nullptr;

    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        if (
            g_taskbarStates[i].monitor ==
            cursorMonitor
        ) {
            cursorState =
                &g_taskbarStates[i];
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
        IsCursorNearBottomEdge(
            cursorState->hwnd,
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

bool IsCursorInBottomTriggerBand(POINT pt, HMONITOR monitor) {
    if (!monitor) {
        return false;
    }

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);

    if (!GetMonitorInfoW(monitor, &mi)) {
        return false;
    }

    // This is only a lightweight trigger band. The worker performs the exact
    // taskbar-height/DPI hover-zone check before changing visibility. Keeping
    // this sampler separate from the worker avoids putting window scanning on
    // the system input hook path.
    constexpr LONG kCursorTriggerBandPx = 256;

    return
        pt.y >= mi.rcMonitor.bottom - kCursorTriggerBandPx &&
        pt.y < mi.rcMonitor.bottom;
}

DWORD WINAPI CursorSamplingThread(LPVOID) {
    HMONITOR lastMonitor = nullptr;
    bool lastInBand = false;

    HANDLE waitHandles[2] = {
        g_cursorStopEvent,
        g_cursorWakeEvent
    };

    for (;;) {
        if (
            InterlockedCompareExchange(
                &g_cursorSamplingEnabled,
                0,
                0
            ) == 0
        ) {
            DWORD waitResult =
                WaitForMultipleObjects(
                    ARRAYSIZE(waitHandles),
                    waitHandles,
                    FALSE,
                    INFINITE
                );

            if (waitResult == WAIT_OBJECT_0) {
                break;
            }

            lastMonitor = nullptr;
            lastInBand = false;
            continue;
        }

        DWORD waitResult =
            WaitForSingleObject(
                g_cursorStopEvent,
                25
            );

        if (waitResult == WAIT_OBJECT_0) {
            break;
        }

        if (
            InterlockedCompareExchange(
                &g_cursorSamplingEnabled,
                0,
                0
            ) == 0
        ) {
            continue;
        }

        POINT pt = {};
        HMONITOR monitor = nullptr;
        bool inBand = false;

        if (GetCursorPos(&pt)) {
            monitor =
                MonitorFromPoint(
                    pt,
                    MONITOR_DEFAULTTONEAREST
                );

            inBand =
                IsCursorInBottomTriggerBand(
                    pt,
                    monitor
                );
        }

        const bool hoverTriggerChanged =
            inBand != lastInBand ||
            (inBand && monitor != lastMonitor);

        lastMonitor = monitor;
        lastInBand = inBand;

        if (hoverTriggerChanged) {
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


bool HasHiddenTaskbar() {
    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        if (
            g_taskbarStates[i].hwnd &&
            !IsWindowVisible(g_taskbarStates[i].hwnd) &&
            ShouldHideTaskbar(g_taskbarStates[i])
        ) {
            return true;
        }
    }

    return false;
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
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            Wh_Log(L"RegisterClassExW failed: %lu", GetLastError());
            return false;
        }

        g_workerWindowClassAtom = 1;
    }

    g_workerMessageWindow =
        CreateWindowExW(
            0,
            kClassName,
            nullptr,
            0,
            0,
            0,
            0,
            0,
            HWND_MESSAGE,
            nullptr,
            instance,
            nullptr
        );

    if (!g_workerMessageWindow) {
        Wh_Log(L"CreateWindowExW(message window) failed: %lu", GetLastError());

        if (g_workerWindowClassAtom == 1) {
            g_workerWindowClassAtom = 0;
        } else {
            UnregisterClassW(kClassName, instance);
            g_workerWindowClassAtom = 0;
        }

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

    const UINT kIdlePollIntervalMs = 1000;
    const UINT kHiddenPollIntervalMs = 250;

    SetTimer(
        nullptr,
        timerId,
        HasHiddenTaskbar()
            ? kHiddenPollIntervalMs
            : kIdlePollIntervalMs,
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

    CreateWorkerMessageWindow();

    if (g_workerReadyEvent) {
        SetEvent(
            g_workerReadyEvent
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
        PCWSTR value =
            Wh_GetStringSetting(
                L"hideOnMonitors[%d]",
                static_cast<int>(i)
            );

        if (!value || !*value) {
            Wh_FreeStringSetting(value);
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

        Wh_FreeStringSetting(value);
    }

    // Empty selection intentionally means "hide nothing".
    // "All displays" is handled independently of DISPLAYn parsing so it still
    // applies if Windows reports an unexpected device-number format.

    for (
        size_t i = 0;
        i < kMaxMonitorNumbers;
        ++i
    ) {
        PCWSTR value =
            Wh_GetStringSetting(
                L"hoverRevealOnMonitors[%d]",
                static_cast<int>(i)
            );

        if (!value || !*value) {
            Wh_FreeStringSetting(value);
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

        Wh_FreeStringSetting(value);
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

    WaitForSingleObject(
        g_workerReadyEvent,
        INFINITE
    );

    g_cursorStopEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr
        );

    if (!g_cursorStopEvent) {
        Wh_Log(L"CreateEvent for cursor sampler failed");
        WhTool_ModUninit();
        return FALSE;
    }

    g_cursorWakeEvent =
        CreateEventW(
            nullptr,
            FALSE,
            FALSE,
            nullptr
        );

    if (!g_cursorWakeEvent) {
        Wh_Log(L"CreateEvent for cursor sampler wake failed");
        WhTool_ModUninit();
        return FALSE;
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
        Wh_Log(L"CreateThread for cursor sampler failed");
        SafeCloseHandle(g_cursorStopEvent);
        WhTool_ModUninit();
        return FALSE;
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
    HWND taskbar =
        FindWindowW(
            L"Shell_TrayWnd",
            nullptr
        );

    if (taskbar) {
        ShowWindow(
            taskbar,
            SW_SHOW
        );
    }

    HWND secondary = nullptr;

    while (
        (secondary = FindWindowExW(
            nullptr,
            secondary,
            L"Shell_SecondaryTrayWnd",
            nullptr
        )) != nullptr
    ) {
        ShowWindow(
            secondary,
            SW_SHOW
        );
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
     * This function is used only by the dedicated tool process. Do not return
     * while a worker may still execute mod code, because that could unload the
     * module underneath the thread. The launcher watchdog restores taskbars
     * after this process exits.
     */
    ExitProcess(1);
    return false;
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_cursorStopEvent) {
        SetEvent(g_cursorStopEvent);
    }

    if (g_cursorWakeEvent) {
        SetEvent(g_cursorWakeEvent);
    }

    if (g_cursorThread) {
        WaitForThreadWithTimeout(
            g_cursorThread,
            3000,
            L"cursor sampler"
        );

        SafeCloseHandle(g_cursorThread);
    }

    if (g_cursorStopEvent) {
        SafeCloseHandle(g_cursorStopEvent);
    }

    if (g_cursorWakeEvent) {
        SafeCloseHandle(g_cursorWakeEvent);
    }

    if (g_workerThread) {
        DWORD readyResult =
            WaitForSingleObject(
                g_workerReadyEvent,
                1000
            );

        if (readyResult != WAIT_OBJECT_0) {
            Wh_Log(
                L"Worker ready wait during shutdown did not complete: %lu",
                readyResult == WAIT_TIMEOUT
                    ? ERROR_TIMEOUT
                    : GetLastError()
            );
        }

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

        WaitForThreadWithTimeout(
            g_workerThread,
            5000,
            L"worker"
        );

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
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// The launcher below is the documented Windhawk tool-mod boilerplate.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

HANDLE g_launcherWatchdogThread = nullptr;
HANDLE g_launcherWatchdogStopEvent = nullptr;
HANDLE g_toolModChildProcess = nullptr;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;

    if (
        ProcessIdToSessionId(
            GetCurrentProcessId(),
            &sessionId
        ) &&
        sessionId == 0
    ) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;

    int argc;

    LPWSTR* argv =
        CommandLineToArgvW(
            GetCommandLine(),
            &argc
        );

    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (
            wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0
        ) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (
            wcscmp(argv[i], L"-tool-mod") == 0
        ) {
            isToolModProcess = true;

            if (
                wcscmp(
                    argv[i + 1],
                    WH_MOD_ID
                ) == 0
            ) {
                isCurrentToolModProcess = true;
            }

            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(
                nullptr,
                TRUE,
                L"windhawk-tool-mod_" WH_MOD_ID
            );

        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(
                L"Tool mod already running (%s)",
                WH_MOD_ID
            );
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(
                nullptr
            );

        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)(
                (BYTE*)dosHeader +
                dosHeader->e_lfanew
            );

        DWORD entryPointRVA =
            ntHeaders->OptionalHeader.AddressOfEntryPoint;

        void* entryPoint =
            (BYTE*)dosHeader +
            entryPointRVA;

        Wh_SetFunctionHook(
            entryPoint,
            (void*)EntryPoint_Hook,
            nullptr
        );

        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

DWORD WINAPI ToolModWatchdogThread(LPVOID) {
    HANDLE waitHandles[2] = {
        g_launcherWatchdogStopEvent,
        g_toolModChildProcess
    };

    DWORD result =
        WaitForMultipleObjects(
            ARRAYSIZE(waitHandles),
            waitHandles,
            FALSE,
            INFINITE
        );

    if (result == WAIT_OBJECT_0 + 1) {
        /*
         * The dedicated tool process ended. Whether it crashed, was killed,
         * or completed a normal shutdown, make sure Explorer taskbars are not
         * left hidden.
         */
        Wh_Log(
            L"Tool process exited; restoring taskbars"
        );
        RestoreAllTaskbars();
    }

    return 0;
}

void StopLauncherWatchdog() {
    if (g_launcherWatchdogStopEvent) {
        SetEvent(
            g_launcherWatchdogStopEvent
        );
    }

    if (g_launcherWatchdogThread) {
        WaitForSingleObject(
            g_launcherWatchdogThread,
            INFINITE
        );
        CloseHandle(
            g_launcherWatchdogThread
        );
        g_launcherWatchdogThread = nullptr;
    }

    if (g_launcherWatchdogStopEvent) {
        CloseHandle(
            g_launcherWatchdogStopEvent
        );
        g_launcherWatchdogStopEvent = nullptr;
    }

    if (g_toolModChildProcess) {
        CloseHandle(
            g_toolModChildProcess
        );
        g_toolModChildProcess = nullptr;
    }
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];

    switch (
        GetModuleFileName(
            nullptr,
            currentProcessPath,
            ARRAYSIZE(currentProcessPath)
        )
    ) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(
                L"GetModuleFileName failed"
            );
            return;
    }

    WCHAR commandLine[
        MAX_PATH +
        2 +
        (
            sizeof(
                L" -tool-mod \"" WH_MOD_ID "\""
            ) / sizeof(WCHAR)
        ) -
        1
    ];

    swprintf_s(
        commandLine,
        L"\"%s\" -tool-mod \"%s\"",
        currentProcessPath,
        WH_MOD_ID
    );

    HMODULE kernelModule =
        GetModuleHandle(
            L"kernelbase.dll"
        );

    if (!kernelModule) {
        kernelModule =
            GetModuleHandle(
                L"kernel32.dll"
            );

        if (!kernelModule) {
            Wh_Log(
                L"No kernelbase.dll/kernel32.dll"
            );
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken,
        LPCWSTR lpApplicationName,
        LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        WINBOOL bInheritHandles,
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken
    );

    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(
            kernelModule,
            "CreateProcessInternalW"
        );

    if (!pCreateProcessInternalW) {
        Wh_Log(
            L"No CreateProcessInternalW"
        );
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };

    PROCESS_INFORMATION pi;

    if (
        !pCreateProcessInternalW(
            nullptr,
            currentProcessPath,
            commandLine,
            nullptr,
            nullptr,
            FALSE,
            NORMAL_PRIORITY_CLASS,
            nullptr,
            nullptr,
            &si,
            &pi,
            nullptr
        )
    ) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    g_toolModChildProcess = pi.hProcess;

    g_launcherWatchdogStopEvent =
        CreateEventW(
            nullptr,
            TRUE,
            FALSE,
            nullptr
        );

    if (!g_launcherWatchdogStopEvent) {
        Wh_Log(
            L"CreateEvent for launcher watchdog failed: %lu",
            GetLastError()
        );
        CloseHandle(pi.hProcess);
        g_toolModChildProcess = nullptr;
        CloseHandle(pi.hThread);
        return;
    }

    g_launcherWatchdogThread =
        CreateThread(
            nullptr,
            0,
            ToolModWatchdogThread,
            nullptr,
            0,
            nullptr
        );

    if (!g_launcherWatchdogThread) {
        Wh_Log(
            L"CreateThread for launcher watchdog failed: %lu",
            GetLastError()
        );
        CloseHandle(
            g_launcherWatchdogStopEvent
        );
        g_launcherWatchdogStopEvent = nullptr;
        CloseHandle(pi.hProcess);
        g_toolModChildProcess = nullptr;
        CloseHandle(pi.hThread);
        return;
    }

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
        StopLauncherWatchdog();
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}

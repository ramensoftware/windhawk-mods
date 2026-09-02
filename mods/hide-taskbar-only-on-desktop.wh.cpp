// ==WindhawkMod==
// @id              hide-taskbar-only-on-desktop
// @name            Hide Taskbar Only on Desktop
// @description     Hides selected taskbars only while their display is showing the desktop
// @version         3.1.0
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

### Why this is a separate mod

**`taskbar-auto-hide-when-maximized`** primarily bases its behavior on
application-window geometry and maximized state. This mod instead evaluates
whether each display currently has any visible application window. An
application can remain open on display 2 while the taskbar on display 1
hides because display 1 is showing only the desktop.

**`taskbar-auto-hide-per-monitor`** provides explicit per-monitor control
over native auto-hide. This mod also has per-display selection, but its
visibility decision is derived automatically from application presence.

**`taskbar-auto-hide-custom-activation-area`** changes the activation area
for Windows' native auto-hide. This mod directly hides/shows the taskbar
window and supplies its own hover-reveal behavior.

**`taskbar-fade`** is a broader taskbar customization/fading mod with a
Smart Idle option. This mod is specifically focused on desktop-only visibility,
independent per-display application state, and explicit per-display hover
reveal.

A key difference is the **work-area behavior**: the mod uses
`ShowWindow(SW_HIDE)` and does not change the Windows desktop work area.

### Process and state model

The mod runs as a dedicated Windhawk tool process. A single worker thread
owns the mutable runtime state and settings. Window-state WinEvents request
an immediate reconciliation, while a periodic safety poll handles missed
or unusual transitions.

Each safety poll performs one display enumeration and one top-level-window
enumeration. The result is reused for every taskbar.
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
};

struct MonitorList {
    MonitorEntry entries[kMaxMonitorNumbers];
    size_t count;
};

struct TaskbarMonitorState {
    HWND hwnd;
    HMONITOR monitor;
    int monitorNumber;
    bool desktopOnly;
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

TaskbarMonitorState g_taskbarStates[kMaxTaskbars] = {};
size_t g_taskbarStateCount = 0;

bool g_hoverActive = false;
HMONITOR g_hoverMonitor = nullptr;
ULONGLONG g_hoverDeadline = 0;

LONG g_refreshPosted = 0;

void LoadSettings();
void WhTool_ModUninit();

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

    return TRUE;
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
        return 0;
    }

    return static_cast<int>(number);
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

bool ShouldHideMonitor(
    int monitorNumber
) {
    if (g_settings.hideAllMonitors) {
        return true;
    }

    if (
        monitorNumber < 1 ||
        monitorNumber > static_cast<int>(
            kMaxMonitorNumbers
        )
    ) {
        return false;
    }

    return g_settings.hideMonitor[monitorNumber];
}

bool ShouldRevealOnHover(
    int monitorNumber
) {
    if (g_settings.hoverAllMonitors) {
        return true;
    }

    if (
        monitorNumber < 1 ||
        monitorNumber > static_cast<int>(
            kMaxMonitorNumbers
        )
    ) {
        return false;
    }

    return g_settings.hoverMonitor[monitorNumber];
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
        wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0 ||
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

    if (GetWindow(hwnd, GW_OWNER) != nullptr) {
        return false;
    }

    if (IsTransientShellWindow(
            hwnd,
            className
        )) {
        return false;
    }

    LONG_PTR exStyle =
        GetWindowLongPtrW(
            hwnd,
            GWL_EXSTYLE
        );

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
            POINT center = {
                popupRect.left +
                    (popupRect.right - popupRect.left) / 2,
                popupRect.top +
                    (popupRect.bottom - popupRect.top) / 2
            };

            HMONITOR popupMonitor =
                MonitorFromPoint(
                    center,
                    MONITOR_DEFAULTTONEAREST
                );

            for (
                size_t i = 0;
                i < context->monitors->count;
                ++i
            ) {
                if (
                    context->monitors->entries[i].monitor ==
                    popupMonitor
                ) {
                    context->result->taskbarPopupOnMonitor[i] = true;
                    break;
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

bool IsNativeAutoHideEnabled(HWND taskbar) {
    if (!taskbar || !IsWindow(taskbar)) {
        return false;
    }

    APPBARDATA data = {};
    data.cbSize = sizeof(data);
    data.hWnd = taskbar;

    return (SHAppBarMessage(ABM_GETSTATE, &data) & ABS_AUTOHIDE) != 0;
}


bool ShouldHideTaskbar(
    const TaskbarMonitorState& state
) {
    return
        state.hwnd &&
        state.monitor &&
        ShouldHideMonitor(state.monitorNumber) &&
        IsBottomDockedTaskbar(
            state.hwnd,
            state.monitor
        ) &&
        !IsNativeAutoHideEnabled(state.hwnd);
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
        ShowWindow(
            state.hwnd,
            show ? SW_SHOW : SW_HIDE
        );
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
                g_settings.extraHoverMarginPx;
        }
    }

    return
        MulDiv(
            48,
            static_cast<int>(dpi),
            96
        ) +
        g_settings.extraHoverMarginPx;
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

bool IsAltTabActive() {
    HWND foreground = GetForegroundWindow();
    if (!foreground || !IsWindowVisible(foreground)) {
        return false;
    }

    WCHAR className[128] = {};
    if (!GetClassNameW(
            foreground,
            className,
            ARRAYSIZE(className))) {
        return false;
    }

    return
        wcscmp(className, L"TaskSwitcherWnd") == 0 ||
        wcscmp(className, L"MultitaskingViewFrame") == 0;
}

void UpdateTaskbarState() {
    MonitorList monitors =
        GetCurrentMonitors();

    RefreshTaskbarMonitorStates(
        monitors
    );

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
    int cursorMonitorNumber = 0;

    if (GetCursorPos(&cursorPoint)) {
        cursorMonitor =
            MonitorFromPoint(
                cursorPoint,
                MONITOR_DEFAULTTONEAREST
            );

        cursorMonitorNumber =
            GetMonitorNumber(
                monitors,
                cursorMonitor
            );
    }

    HWND cursorTaskbar = nullptr;

    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        if (
            g_taskbarStates[i].monitor ==
                cursorMonitor
        ) {
            cursorTaskbar =
                g_taskbarStates[i].hwnd;
            break;
        }
    }

    const bool hovering =
        cursorTaskbar &&
        cursorMonitor &&
        ShouldRevealOnHover(
            cursorMonitorNumber
        ) &&
        IsCursorNearBottomEdge(
            cursorTaskbar,
            cursorMonitor
        );

    // Keep the taskbar visible for an open shell context menu/overflow
    // surface even after the cursor leaves the popup. Popup state was collected
    // during the main EnumWindows pass above, so no second full enumeration is
    // needed here.
    HMONITOR popupMonitor = nullptr;

    for (
        size_t monitorIndex = 0;
        monitorIndex < monitors.count;
        ++monitorIndex
    ) {
        if (scan.taskbarPopupOnMonitor[monitorIndex]) {
            popupMonitor =
                monitors.entries[monitorIndex].monitor;
            break;
        }
    }

    if (popupMonitor) {
        g_hoverActive = false;
        g_hoverMonitor = nullptr;
        g_hoverDeadline = 0;

        for (size_t i = 0; i < g_taskbarStateCount; ++i) {
            TaskbarMonitorState& state =
                g_taskbarStates[i];

            SetTaskbarState(
                state,
                state.monitor == popupMonitor ||
                !state.desktopOnly ||
                !ShouldHideTaskbar(state)
            );
        }

        return;
    }

    if (IsAltTabActive()) {
        g_hoverActive = false;
        g_hoverMonitor = nullptr;
        g_hoverDeadline = 0;

        for (size_t i = 0; i < g_taskbarStateCount; ++i) {
            TaskbarMonitorState& state =
                g_taskbarStates[i];

            SetTaskbarState(
                state,
                !state.desktopOnly ||
                !ShouldHideTaskbar(state)
            );
        }

        return;
    }

    if (hovering) {
        g_hoverActive = true;
        g_hoverMonitor = cursorMonitor;
        g_hoverDeadline = 0;

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

            ApplyBaseTaskbarState();
            return;
        }

        const ULONGLONG now =
            GetTickCount64();

        if (g_hoverDeadline == 0) {
            g_hoverDeadline =
                now +
                g_settings.autoHideDelayMs;
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

            return;
        }

        g_hoverActive = false;
        g_hoverMonitor = nullptr;
        g_hoverDeadline = 0;

        ApplyBaseTaskbarState();
        return;
    }

    ApplyBaseTaskbarState();
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

    const int monitorNumber =
        GetMonitorNumber(
            monitors,
            cursorMonitor
        );

    if (
        monitorNumber == 0 ||
        !ShouldRevealOnHover(
            monitorNumber
        )
    ) {
        return false;
    }

    HWND taskbar = nullptr;

    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        if (
            g_taskbarStates[i].monitor ==
            cursorMonitor
        ) {
            taskbar =
                g_taskbarStates[i].hwnd;
            break;
        }
    }

    return
        taskbar &&
        IsCursorNearBottomEdge(
            taskbar,
            cursorMonitor
        );
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

    for (;;) {
        DWORD waitResult =
            WaitForSingleObject(
                g_cursorStopEvent,
                25
            );

        if (waitResult == WAIT_OBJECT_0) {
            break;
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

    if (g_taskbarShowHook) {
        UnhookWinEvent(g_taskbarShowHook);
        g_taskbarShowHook = nullptr;
        g_taskbarShowHookProcessId = 0;
    }

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

    const UINT kIdlePollIntervalMs = 1000;
    const UINT kHiddenPollIntervalMs = 250;
    const UINT kHoverPollIntervalMs = 50;

    UINT_PTR timerId =
        SetTimer(
            nullptr,
            1,
            kIdlePollIntervalMs,
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

            if (timerId) {
                SetTimer(
                    nullptr,
                    timerId,
                    g_hoverActive
                        ? kHoverPollIntervalMs
                        : HasHiddenTaskbar()
                            ? kHiddenPollIntervalMs
                            : kIdlePollIntervalMs,
                    nullptr
                );
            }

            continue;
        }

        if (msg.message == WM_APP_REFRESH) {
            InterlockedExchange(
                &g_refreshPosted,
                0
            );

            EnsureTaskbarShowHook();
            UpdateTaskbarState();

            if (timerId) {
                SetTimer(
                    nullptr,
                    timerId,
                    g_hoverActive
                        ? kHoverPollIntervalMs
                        : HasHiddenTaskbar()
                            ? kHiddenPollIntervalMs
                            : kIdlePollIntervalMs,
                    nullptr
                );
            }

            continue;
        }

        if (msg.message == WM_APP_SETTINGS) {
            LoadSettings();
            EnsureTaskbarShowHook();
            UpdateTaskbarState();

            if (timerId) {
                SetTimer(
                    nullptr,
                    timerId,
                    g_hoverActive
                        ? kHoverPollIntervalMs
                        : HasHiddenTaskbar()
                            ? kHiddenPollIntervalMs
                            : kIdlePollIntervalMs,
                    nullptr
                );
            }

            continue;
        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);

        if (timerId) {
            SetTimer(
                nullptr,
                timerId,
                g_hoverActive
                    ? kHoverPollIntervalMs
                    : HasHiddenTaskbar()
                        ? kHiddenPollIntervalMs
                        : kIdlePollIntervalMs,
                nullptr
            );
        }
    }

    if (timerId) {
        KillTimer(
            nullptr,
            timerId
        );
    }

    if (g_foregroundHook) {
        UnhookWinEvent(
            g_foregroundHook
        );
        g_foregroundHook = nullptr;
    }

    if (g_minimizeHook) {
        UnhookWinEvent(
            g_minimizeHook
        );
        g_minimizeHook = nullptr;
    }

    if (g_moveHook) {
        UnhookWinEvent(
            g_moveHook
        );
        g_moveHook = nullptr;
    }

    if (g_taskbarShowHook) {
        UnhookWinEvent(
            g_taskbarShowHook
        );
        g_taskbarShowHook = nullptr;
        g_taskbarShowHookProcessId = 0;
    }

    return 0;
}

void LoadSettings() {
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

        CloseHandle(
            g_workerReadyEvent
        );

        g_workerReadyEvent = nullptr;
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
        CloseHandle(g_cursorStopEvent);
        g_cursorStopEvent = nullptr;
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

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_cursorStopEvent) {
        SetEvent(g_cursorStopEvent);
    }

    if (g_cursorThread) {
        WaitForSingleObject(
            g_cursorThread,
            INFINITE
        );

        CloseHandle(g_cursorThread);
        g_cursorThread = nullptr;
    }

    if (g_cursorStopEvent) {
        CloseHandle(g_cursorStopEvent);
        g_cursorStopEvent = nullptr;
    }

    if (g_workerThread) {
        WaitForSingleObject(
            g_workerReadyEvent,
            INFINITE
        );

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

        WaitForSingleObject(
            g_workerThread,
            INFINITE
        );

        CloseHandle(
            g_workerThread
        );

        g_workerThread = nullptr;
    }

    if (g_workerReadyEvent) {
        CloseHandle(
            g_workerReadyEvent
        );

        g_workerReadyEvent = nullptr;
    }

    /*
     * Restore all currently discoverable taskbars when the tool exits.
     */
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

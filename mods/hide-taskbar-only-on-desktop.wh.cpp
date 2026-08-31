// ==WindhawkMod==
// @id              hide-taskbar-only-on-desktop
// @name            Hide Taskbar Only on Desktop
// @description     Hides the taskbar on the desktop, shows it for other windows or when you hover near the bottom edge
// @version 2.0.0
// @author          Sahil Dashoni
// @github          https://github.com/Sahil-Dashoni
// @include         explorer.exe
// @compilerOptions -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Taskbar on Desktop

Hides each selected taskbar whenever its own monitor is showing only the
desktop (or its application windows are all minimized), and shows it again
when an application or relevant Windows shell UI needs it.

You can also peek at the taskbar by moving the mouse to the bottom of the
screen while on the desktop - the reveal zone matches the taskbar's own
height (plus a small extra margin you can configure), not just a thin
strip at the very bottom, so hovering anywhere over where the taskbar
would be counts. The hover reveal is selectable per display; select individual displays or All displays. The delay before hiding only applies to that peek: hover
in, then move away, and it waits a moment before hiding again. Every other
hide (switching to the desktop, closing or minimizing the last window,
etc.) hides instantly, no delay.

### Notes
- Works with a single taskbar, and optionally with secondary taskbars on
  extra monitors (toggle in the settings).
- Pressing the Windows key or clicking the Start button still works even
  while the taskbar is hidden, and will bring the taskbar back, since the
  Start menu counts as "another window".
- Each monitor's "on desktop" state re-verifies itself roughly 10 times a
  second by checking for any other visible, non-minimized window on that
  monitor, rather than depending on a single event. Maximized windows use
  Windows' monitor assignment, while normal/spanning windows count on each
  display they actually intersect. This makes minimizing the last window on
  one display hide that display's taskbar without affecting another display
  that still has an application open.
- Opening Start, the system tray overflow ("show hidden icons"), or the
  notification/clock panel keeps the corresponding taskbar visible. Real foreground applications also take precedence over a stale desktop
  scan, including foreground windows that do not expose a normal title or
  APPWINDOW style.
- On multi-display setups, the hover zone uses only the taskbar and DPI of
  the display under the cursor; it never falls back to the primary taskbar.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- extraHoverMarginMm: 5
  $name: Extra hover margin (mm)
  $description: >-
    The reveal zone at the bottom of the screen automatically matches your
    taskbar's actual height (including your display scaling), so hovering
    anywhere over where the taskbar would be reveals it. This setting adds
    a bit of extra margin above that, in millimeters, so you don't need to
    be pixel-perfect. Default 5mm.
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
    Select All displays to enable it everywhere, or replace it with one or
    more individual displays.
  $options:
  - all: All displays
  - monitor1: Display 1 (primary)
  - monitor2: Display 2
  - monitor3: Display 3
  - monitor4: Display 4
  - monitor5: Display 5
  - monitor6: Display 6
  - monitor7: Display 7
  - monitor8: Display 8
  - monitor9: Display 9
  - monitor10: Display 10
  - monitor11: Display 11
  - monitor12: Display 12
  - monitor13: Display 13
  - monitor14: Display 14
  - monitor15: Display 15
  - monitor16: Display 16
- hideOnMonitors: ["all"]
  $name: Taskbars to hide on desktop
  $description: >-
    Select one or more displays. Display 1 is the primary display.
    Choose All displays to hide every connected display. Use Add to select
    multiple displays.
  $options:
  - all: All displays
  - monitor1: Display 1 (primary)
  - monitor2: Display 2
  - monitor3: Display 3
  - monitor4: Display 4
  - monitor5: Display 5
  - monitor6: Display 6
  - monitor7: Display 7
  - monitor8: Display 8
  - monitor9: Display 9
  - monitor10: Display 10
  - monitor11: Display 11
  - monitor12: Display 12
  - monitor13: Display 13
  - monitor14: Display 14
  - monitor15: Display 15
  - monitor16: Display 16
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <wchar.h>

constexpr size_t kMaxMonitorNumbers = 16;

struct {
    int extraHoverMarginMm;
    DWORD autoHideDelayMs;
    bool hideAllMonitors;
    bool hideMonitor[kMaxMonitorNumbers + 1];
    bool hoverAllMonitors;
    bool hoverMonitor[kMaxMonitorNumbers + 1];
} settings = {};

HWINEVENTHOOK g_hWinEventHookForeground;
HANDLE g_hThread;
DWORD g_threadId;
bool g_shownDueToHover = false;  // taskbar currently shown because of hover
HMONITOR g_hoverMonitor = nullptr; // monitor whose taskbar was revealed
ULONGLONG g_hideDeadline = 0;      // pending hover-dismiss deadline

bool IsShellChromeClass(const WCHAR* className) {
    static const WCHAR* kShellClasses[] = {
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

    for (const WCHAR* shellClass : kShellClasses) {
        if (wcscmp(className, shellClass) == 0) {
            return true;
        }
    }

    return false;
}

bool IsDesktopInfrastructureWindow(HWND hwnd, const WCHAR* className) {
    if (!hwnd || !className) {
        return false;
    }

    /*
     * Progman and WorkerW are Explorer's desktop/wallpaper infrastructure.
     * WorkerW can exist with or without SHELLDLL_DefView, so neither should
     * ever be classified as an application.
     */
    if (
        wcscmp(className, L"Progman") == 0 ||
        wcscmp(className, L"WorkerW") == 0
    ) {
        return true;
    }

    HWND shellWindow = GetShellWindow();

    return shellWindow && hwnd == shellWindow;
}


constexpr size_t kMaxTaskbars = 32;

struct TaskbarMonitorState {
    HWND hwnd = nullptr;
    HMONITOR monitor = nullptr;
    int monitorNumber = 0;
    bool desktopOnly = true;
};

TaskbarMonitorState g_taskbarStates[kMaxTaskbars] = {};
size_t g_taskbarStateCount = 0;

struct MonitorWindowScanContext {
    HMONITOR monitor;
    bool found;
};


bool IsLikelyApplicationWindow(
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
        IsShellChromeClass(
            className
        )
    ) {
        return false;
    }

    /*
     * Do not count a visible, titleless helper surface as an application
     * unless it explicitly opts into normal app-window treatment.
     */
    if (exStyle & WS_EX_APPWINDOW) {
        return true;
    }

    return GetWindowTextLengthW(hwnd) > 0;
}


BOOL CALLBACK EnumWindowsForMonitorProc(
    HWND hwnd,
    LPARAM lParam
) {
    MonitorWindowScanContext* context =
        reinterpret_cast<MonitorWindowScanContext*>(lParam);

    if (!context || !context->monitor) {
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

    if (!IsLikelyApplicationWindow(
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

    MONITORINFO targetInfo = {};
    targetInfo.cbSize = sizeof(targetInfo);

    if (
        !GetMonitorInfoW(
            context->monitor,
            &targetInfo
        )
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

        if (
            windowMonitor ==
            context->monitor
        ) {
            context->found = true;
            return FALSE;
        }

        return TRUE;
    }

    /*
     * Normal and spanning windows use their actual visible rectangle. This
     * lets one window legitimately count on more than one display.
     */
    RECT intersection = {};

    if (
        IntersectRect(
            &intersection,
            &rect,
            &targetInfo.rcMonitor
        )
    ) {
        context->found = true;
        return FALSE;
    }

    return TRUE;
}


bool HasOtherVisibleWindowOnMonitor(
    HMONITOR monitor
) {
    MonitorWindowScanContext context = {
        monitor,
        false
    };

    EnumWindows(
        EnumWindowsForMonitorProc,
        reinterpret_cast<LPARAM>(&context)
    );

    return context.found;
}


bool IsForegroundApplicationWindow(
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

    if (
        IsDesktopInfrastructureWindow(
            hwnd,
            className
        ) ||
        IsShellChromeClass(
            className
        )
    ) {
        return false;
    }

    /*
     * The foreground window is a stronger signal than a background helper
     * window discovered by EnumWindows. Do not require a title or
     * WS_EX_APPWINDOW here; this covers shell-integrated applications and
     * virtual-display/application windows which may be titleless.
     */
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

    return true;
}


void RefreshDesktopState() {
    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        TaskbarMonitorState& state =
            g_taskbarStates[i];

        if (!state.monitor) {
            state.desktopOnly = true;
            continue;
        }

        state.desktopOnly =
            !HasOtherVisibleWindowOnMonitor(
                state.monitor
            );
    }

    /*
     * The per-monitor scan is the authoritative baseline. A real foreground
     * application is an additional immediate signal for its own monitor.
     *
     * Crucially, Progman/WorkerW are never used for this override because
     * Windows may associate the desktop foreground window with the primary
     * display even when the user clicked another display's desktop.
     */
    HWND foreground = GetForegroundWindow();

    if (!foreground ||
        !IsWindowVisible(foreground) ||
        IsIconic(foreground)) {
        return;
    }

    WCHAR className[256] = {};

    if (
        GetClassNameW(
            foreground,
            className,
            ARRAYSIZE(className)
        ) == 0
    ) {
        return;
    }

    if (
        IsDesktopInfrastructureWindow(
            foreground,
            className
        ) ||
        IsShellChromeClass(
            className
        )
    ) {
        return;
    }

    if (!IsForegroundApplicationWindow(
            foreground,
            className
        )) {
        return;
    }

    /*
     * The foreground window is an immediate signal that its display is
     * active. Prefer the monitor Windows assigns to the window. For unusual
     * virtual/maximized display geometry, verify that choice against the
     * actual foreground rectangle and use the monitor with the largest
     * visible intersection when it differs.
     */
    HMONITOR foregroundMonitor =
        MonitorFromWindow(
            foreground,
            MONITOR_DEFAULTTONEAREST
        );

    /*
     * Keep the foreground override tied to the monitor Windows assigns to
     * the foreground window. The per-monitor scan already handles normal
     * windows that span multiple displays, so there is no need to enumerate
     * MonitorList here (which would also create a declaration-order
     * dependency).
     */

    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        if (
            g_taskbarStates[i].monitor ==
            foregroundMonitor
        ) {
            g_taskbarStates[i].desktopOnly = false;
            break;
        }
    }
}

struct MonitorEntry {
    HMONITOR monitor;
    RECT rect;
    bool primary;
    wchar_t deviceName[32];
};

struct MonitorList {
    MonitorEntry entries[kMaxMonitorNumbers];
    size_t count;
};


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

    if (!GetMonitorInfoW(
            monitor,
            &info
        )) {
        return TRUE;
    }

    MonitorEntry& entry =
        list->entries[list->count++];

    entry.monitor = monitor;
    entry.rect = info.rcMonitor;
    entry.primary =
        (info.dwFlags & MONITORINFOF_PRIMARY) != 0;

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

    /*
     * Windows normally exposes the desktop display devices as DISPLAY1,
     * DISPLAY2, ... . We use that OS-provided identity for numbering instead
     * of the screen coordinates. This means moving a display left/right in
     * Windows Display Settings does not change its selected display number.
     */
    constexpr wchar_t kPrefix[] = L"\\\\.\\DISPLAY";

    size_t prefixLength =
        ARRAYSIZE(kPrefix) - 1;

    if (
        wcsncmp(
            deviceName,
            kPrefix,
            prefixLength
        ) != 0
    ) {
        return 0;
    }

    wchar_t* endNumber = nullptr;

    long number =
        wcstol(
            deviceName + prefixLength,
            &endNumber,
            10
        );

    if (
        endNumber == deviceName + prefixLength ||
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


void SortDisplaysForSelection(
    MonitorList& list
) {
    /*
     * Keep the primary display as Display 1 for the user-facing setting.
     * For all other displays, prefer Windows' DISPLAYn number. This is much
     * more stable than sorting by x/y coordinates.
     *
     * If Windows gives us an unexpected device name, fall back to the
     * previous deterministic coordinate ordering.
     */
    size_t primaryIndex = list.count;

    for (size_t i = 0; i < list.count; ++i) {
        if (list.entries[i].primary) {
            primaryIndex = i;
            break;
        }
    }

    if (
        primaryIndex < list.count &&
        primaryIndex != 0
    ) {
        MonitorEntry primary =
            list.entries[primaryIndex];

        for (size_t i = primaryIndex; i > 0; --i) {
            list.entries[i] =
                list.entries[i - 1];
        }

        list.entries[0] = primary;
    }

    for (size_t i = 1; i < list.count; ++i) {
        size_t best = i;
        int bestNumber =
            GetDisplayDeviceNumber(
                list.entries[best].deviceName
            );

        for (size_t j = i + 1; j < list.count; ++j) {
            int candidateNumber =
                GetDisplayDeviceNumber(
                    list.entries[j].deviceName
                );

            bool candidateIsBetter = false;

            if (
                candidateNumber > 0 &&
                bestNumber == 0
            ) {
                candidateIsBetter = true;
            } else if (
                candidateNumber > 0 &&
                bestNumber > 0 &&
                candidateNumber < bestNumber
            ) {
                candidateIsBetter = true;
            } else if (
                candidateNumber == 0 &&
                bestNumber == 0
            ) {
                const RECT& a =
                    list.entries[best].rect;
                const RECT& b =
                    list.entries[j].rect;

                candidateIsBetter =
                    b.top < a.top ||
                    (
                        b.top == a.top &&
                        b.left < a.left
                    );
            }

            if (candidateIsBetter) {
                best = j;
                bestNumber = candidateNumber;
            }
        }

        if (best != i) {
            MonitorEntry temp =
                list.entries[i];

            list.entries[i] =
                list.entries[best];

            list.entries[best] =
                temp;
        }
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

    SortDisplaysForSelection(list);

    return list;
}


int GetMonitorNumber(
    const MonitorList& list,
    HMONITOR monitor
) {
    for (size_t i = 0; i < list.count; ++i) {
        if (list.entries[i].monitor == monitor) {
            return static_cast<int>(i + 1);
        }
    }

    return 0;
}


bool ShouldHideMonitor(int monitorNumber) {
    if (
        monitorNumber < 1 ||
        monitorNumber > static_cast<int>(kMaxMonitorNumbers)
    ) {
        return false;
    }

    return
        settings.hideAllMonitors ||
        settings.hideMonitor[monitorNumber];
}

bool ShouldRevealOnHover(int monitorNumber) {
    if (
        monitorNumber < 1 ||
        monitorNumber > static_cast<int>(kMaxMonitorNumbers)
    ) {
        return false;
    }

    return
        settings.hoverAllMonitors ||
        settings.hoverMonitor[monitorNumber];
}


void RefreshTaskbarMonitorStates() {
    TaskbarMonitorState oldStates[kMaxTaskbars] = {};
    size_t oldCount = g_taskbarStateCount;

    MonitorList monitors =
        GetCurrentMonitors();

    for (size_t i = 0; i < oldCount; ++i) {
        oldStates[i] = g_taskbarStates[i];
    }

    g_taskbarStateCount = 0;

    auto addTaskbar = [&](HWND hwnd, bool secondary) {
        (void)secondary;

        if (
            !hwnd ||
            g_taskbarStateCount >= kMaxTaskbars
        ) {
            return;
        }

        RECT taskbarRect = {};

        if (!GetWindowRect(
                hwnd,
                &taskbarRect
            )) {
            return;
        }

        // The Shell taskbar HWND is tied to its actual display. Using
        // MonitorFromWindow() avoids inferring ownership from transient
        // taskbar geometry, which can be misleading with virtual displays.
        HMONITOR monitor =
            MonitorFromWindow(
                hwnd,
                MONITOR_DEFAULTTONEAREST
            );

        if (!monitor) {
            POINT taskbarCenter = {
                taskbarRect.left +
                    (taskbarRect.right - taskbarRect.left) / 2,
                taskbarRect.top +
                    (taskbarRect.bottom - taskbarRect.top) / 2
            };

            monitor =
                MonitorFromPoint(
                    taskbarCenter,
                    MONITOR_DEFAULTTONEAREST
                );
        }

        TaskbarMonitorState state = {};
        state.hwnd = hwnd;
        state.monitor = monitor;
        state.monitorNumber =
            GetMonitorNumber(
                monitors,
                monitor
            );

        for (size_t i = 0; i < oldCount; ++i) {
            if (oldStates[i].hwnd == hwnd) {
                state.desktopOnly =
                    oldStates[i].desktopOnly;
                break;
            }
        }

        g_taskbarStates[g_taskbarStateCount++] = state;
    };

    addTaskbar(
        FindWindowW(
            L"Shell_TrayWnd",
            nullptr
        ),
        false
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
        addTaskbar(
            secondary,
            true
        );
    }
}


bool IsShellFlyoutWindow(HWND hwnd) {
    if (
        !hwnd ||
        !IsWindow(hwnd) ||
        !IsWindowVisible(hwnd)
    ) {
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

    /*
     * These are shell/XAML popup classes used by Windows 11 for taskbar
     * flyouts such as the notification/calendar and quick-settings surfaces.
     * The class check is intentionally combined with the Explorer process
     * check below so an identically named third-party window isn't treated as
     * shell UI.
     */
    bool knownFlyoutClass =
        wcscmp(
            className,
            L"Xaml_WindowedPopupClass"
        ) == 0 ||
        wcscmp(
            className,
            L"Windows.UI.Core.CoreWindow"
        ) == 0 ||
        wcscmp(
            className,
            L"TopLevelWindowForOverflowXamlIsland"
        ) == 0 ||
        wcscmp(
            className,
            L"NotifyIconOverflowWindow"
        ) == 0;

    if (!knownFlyoutClass) {
        return false;
    }

    DWORD processId = 0;

    GetWindowThreadProcessId(
        hwnd,
        &processId
    );

    if (!processId) {
        return false;
    }

    HWND taskbar =
        FindWindowW(
            L"Shell_TrayWnd",
            nullptr
        );

    DWORD taskbarProcessId = 0;

    if (taskbar) {
        GetWindowThreadProcessId(
            taskbar,
            &taskbarProcessId
        );
    }

    /*
     * Use the taskbar's Explorer process as the primary trust boundary.
     * For secondary taskbars, Explorer is still the owning process, but a
     * taskbar recreation can briefly leave the primary lookup unavailable.
     */
    if (
        taskbarProcessId != 0 &&
        processId == taskbarProcessId
    ) {
        return true;
    }

    DWORD explorerProcessId = 0;
    HWND shellWindow = GetShellWindow();

    if (shellWindow) {
        GetWindowThreadProcessId(
            shellWindow,
            &explorerProcessId
        );
    }

    return
        explorerProcessId != 0 &&
        processId == explorerProcessId;
}


struct ShellFlyoutScanContext {
    HMONITOR targetMonitor;
    bool found;
};


BOOL CALLBACK ShellFlyoutEnumProc(
    HWND hwnd,
    LPARAM lParam
) {
    ShellFlyoutScanContext* context =
        reinterpret_cast<ShellFlyoutScanContext*>(lParam);

    if (
        !context ||
        !IsShellFlyoutWindow(hwnd)
    ) {
        return TRUE;
    }

    RECT rect = {};

    if (!GetWindowRect(hwnd, &rect)) {
        return TRUE;
    }

    POINT center = {
        rect.left +
            (rect.right - rect.left) / 2,
        rect.top +
            (rect.bottom - rect.top) / 2
    };

    HMONITOR flyoutMonitor =
        MonitorFromPoint(
            center,
            MONITOR_DEFAULTTONEAREST
        );

    if (flyoutMonitor == context->targetMonitor) {
        context->found = true;
        return FALSE;
    }

    return TRUE;
}


bool HasVisibleShellFlyoutOnMonitor(
    HMONITOR monitor
) {
    if (!monitor) {
        return false;
    }

    ShellFlyoutScanContext context = {
        monitor,
        false
    };

    EnumWindows(
        ShellFlyoutEnumProc,
        reinterpret_cast<LPARAM>(&context)
    );

    return context.found;
}


bool IsAltTabActive() {
    /*
     * The legacy Alt+Tab switcher uses #32771. Windows 11's XAML host can
     * remain present after the switcher closes, so do not treat that host
     * alone as proof that Alt+Tab is active.
     */
    HWND foreground = GetForegroundWindow();

    if (foreground) {
        WCHAR className[128] = {};

        if (
            GetClassNameW(
                foreground,
                className,
                ARRAYSIZE(className)
            ) != 0 &&
            wcscmp(className, L"#32771") == 0
        ) {
            return true;
        }
    }

    /*
     * While the Alt+Tab chord is physically held, keep the taskbar visible
     * even if the switcher's foreground window is hosted differently.
     */
    return
        (GetAsyncKeyState(VK_MENU) & 0x8000) != 0 &&
        (GetAsyncKeyState(VK_TAB) & 0x8000) != 0;
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


bool IsTaskbarPopupOpen(HWND taskbar) {
    if (
        !taskbar ||
        !IsWindow(taskbar)
    ) {
        return false;
    }

    POINT pt = {};

    if (!GetCursorPos(&pt)) {
        return false;
    }

    HMONITOR taskbarMonitor =
        MonitorFromWindow(
            taskbar,
            MONITOR_DEFAULTTONEAREST
        );

    HMONITOR cursorMonitor =
        MonitorFromPoint(
            pt,
            MONITOR_DEFAULTTONEAREST
        );

    if (
        !taskbarMonitor ||
        cursorMonitor != taskbarMonitor
    ) {
        return false;
    }

    HWND current = WindowFromPoint(pt);

    if (!current) {
        return false;
    }

    HWND root = GetAncestor(
        current,
        GA_ROOT
    );

    if (root) {
        current = root;
    }

    DWORD taskbarProcessId = 0;

    GetWindowThreadProcessId(
        taskbar,
        &taskbarProcessId
    );

    if (!taskbarProcessId) {
        return false;
    }

    /*
     * A taskbar/Start popup may not become the foreground window. Walk the
     * window's parent/owner chain from the window under the cursor instead.
     * This works for normal #32768 menus as well as XAML shell popups.
     */
    for (
        int depth = 0;
        current && depth < 12;
        ++depth
    ) {
        if (current == taskbar) {
            return false;
        }

        WCHAR className[128] = {};

        if (
            GetClassNameW(
                current,
                className,
                ARRAYSIZE(className)
            ) != 0
        ) {
            bool popupClass =
                IsTaskbarPopupClass(
                    className
                );

            if (
                popupClass &&
                IsWindowVisible(current)
            ) {
                DWORD popupProcessId = 0;

                GetWindowThreadProcessId(
                    current,
                    &popupProcessId
                );

                /*
                 * Prefer the taskbar's Explorer process. A shell popup from
                 * one of the known shell UI processes is also accepted.
                 */
                if (
                    popupProcessId == taskbarProcessId
                ) {
                    return true;
                }

            }
        }

        HWND parent = GetParent(current);
        HWND owner = GetWindow(
            current,
            GW_OWNER
        );

        if (
            parent &&
            parent != current
        ) {
            current = parent;
        } else if (
            owner &&
            owner != current
        ) {
            current = owner;
        } else {
            break;
        }
    }

    return false;
}


bool IsTaskbarShellPopupForeground(HWND taskbar) {
    HWND foreground =
        GetForegroundWindow();

    if (
        !foreground ||
        foreground == taskbar
    ) {
        return false;
    }

    DWORD foregroundThreadId =
        GetWindowThreadProcessId(
            foreground,
            nullptr
        );

    DWORD taskbarThreadId =
        GetWindowThreadProcessId(
            taskbar,
            nullptr
        );

    if (
        !foregroundThreadId ||
        !taskbarThreadId ||
        foregroundThreadId != taskbarThreadId
    ) {
        return false;
    }

    WCHAR className[128] = {};

    if (
        !GetClassNameW(
            foreground,
            className,
            ARRAYSIZE(className)
        )
    ) {
        return false;
    }

    if (!IsTaskbarPopupClass(className)) {
        return false;
    }

    HMONITOR taskbarMonitor =
        MonitorFromWindow(
            taskbar,
            MONITOR_DEFAULTTONEAREST
        );

    HMONITOR popupMonitor =
        MonitorFromWindow(
            foreground,
            MONITOR_DEFAULTTONEAREST
        );

    POINT cursorPoint = {};
    if (!GetCursorPos(&cursorPoint)) {
        return false;
    }

    HMONITOR cursorMonitor =
        MonitorFromPoint(
            cursorPoint,
            MONITOR_DEFAULTTONEAREST
        );

    return
        taskbarMonitor &&
        popupMonitor &&
        cursorMonitor &&
        taskbarMonitor == popupMonitor &&
        popupMonitor == cursorMonitor;
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


void SetTaskbarVisibility(bool show) {
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (hTaskbar) {
        ShowWindow(hTaskbar, show ? SW_SHOW : SW_HIDE);
    }

    HWND hSecondary = nullptr;
    while (
        (hSecondary = FindWindowExW(
            nullptr,
            hSecondary,
            L"Shell_SecondaryTrayWnd",
            nullptr
        )) != nullptr
    ) {
        ShowWindow(
            hSecondary,
            show ? SW_SHOW : SW_HIDE
        );
    }
}

// The reveal zone matches the taskbar's own current height (whatever size
// and display scaling you actually have it set to), plus a configurable
// extra margin on top. Falls back to a small DPI-scaled default if the
// taskbar's rect can't be read for some reason.
int GetHoverZonePx(HWND hTaskbar, UINT dpi) {
    int marginPx = (int)((double)settings.extraHoverMarginMm / 25.4 * dpi);

    RECT tbRect;
    if (hTaskbar && GetWindowRect(hTaskbar, &tbRect)) {
        int taskbarHeight = tbRect.bottom - tbRect.top;
        if (taskbarHeight > 0) {
            return taskbarHeight + marginPx;
        }
    }

    // Fallback: assume a ~48px (at 100% scaling) default taskbar height.
    int fallbackHeight = (int)(48.0 * dpi / 96.0);
    return fallbackHeight + marginPx;
}

// True if the cursor is within the taskbar-height-sized zone at the bottom
// edge of whichever monitor it's currently on.
bool IsCursorNearBottomEdge(HMONITOR cursorMonitor) {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return false;
    }

    if (!cursorMonitor) {
        return false;
    }

    MONITORINFO mi = {sizeof(mi)};

    if (!GetMonitorInfoW(
            cursorMonitor,
            &mi
        )) {
        return false;
    }

    if (
        pt.x < mi.rcMonitor.left ||
        pt.x > mi.rcMonitor.right
    ) {
        return false;
    }

    HWND hTaskbar = nullptr;

    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        if (
            g_taskbarStates[i].monitor == cursorMonitor &&
            g_taskbarStates[i].hwnd
        ) {
            hTaskbar = g_taskbarStates[i].hwnd;
            break;
        }
    }

    /*
     * Do not fall back to the primary taskbar for a secondary display.
     * A primary-taskbar fallback can give the secondary display the wrong
     * reveal-zone geometry during taskbar transitions.
     */
    if (!hTaskbar) {
        return false;
    }

    UINT dpi =
        GetDpiForWindow(hTaskbar);

    int hotZonePx =
        GetHoverZonePx(
            hTaskbar,
            dpi
        );
    if (hotZonePx < 1) {
        hotZonePx = 1;
    }

    return pt.y >= mi.rcMonitor.bottom - hotZonePx;
}

// Single place that decides whether the taskbar should be shown or hidden.
// Only the "hover then move away" case gets a delay; every other hide is
// instant.
void UpdateTaskbarState() {
    RefreshTaskbarMonitorStates();

    /*
     * Determine the cursor monitor first. Each taskbar is evaluated
     * independently below; an application on one display must never make
     * another display's taskbar follow the same desktop state.
     */
    int cursorMonitorNumber = 0;
    HMONITOR cursorMonitor = nullptr;

    POINT cursorPoint = {};
    if (GetCursorPos(&cursorPoint)) {
        MonitorList monitors = GetCurrentMonitors();

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

    bool hovering =
        cursorMonitorNumber != 0 &&
        ShouldRevealOnHover(cursorMonitorNumber) &&
        IsCursorNearBottomEdge(cursorMonitor);

    /*
     * Desktop/application state is already calculated independently for each
     * monitor. Hover only affects the taskbar on the display under the cursor;
     * all other taskbars retain their own state.
     */
    /*
     * Keep the taskbar visible while a Windows shell flyout is open.
     * This covers notification/calendar and quick-settings style surfaces
     * without classifying ordinary application windows as shell UI.
     */
    /*
     * Shell flyouts are local to the display under the cursor for the
     * interaction model of this mod. Do not globally preempt the state of
     * unrelated displays.
     */
    bool shellFlyoutOpen =
        cursorMonitor &&
        HasVisibleShellFlyoutOnMonitor(
            cursorMonitor
        );

    if (shellFlyoutOpen) {
        g_hideDeadline = 0;
        g_shownDueToHover = false;
        g_hoverMonitor = nullptr;

        for (size_t i = 0; i < g_taskbarStateCount; ++i) {
            TaskbarMonitorState& state =
                g_taskbarStates[i];

            bool show =
                state.monitor == cursorMonitor ||
                !state.desktopOnly ||
                !ShouldHideMonitor(
                    state.monitorNumber
                );

            SetTaskbarState(
                state,
                show
            );
        }

        return;
    }

    /*
     * Alt+Tab/Task View is another shell interaction where hiding the
     * taskbar underneath the switcher is undesirable.
     */
    if (IsAltTabActive()) {
        g_hideDeadline = 0;
        g_shownDueToHover = false;
        g_hoverMonitor = nullptr;

        for (size_t i = 0; i < g_taskbarStateCount; ++i) {
            SetTaskbarState(
                g_taskbarStates[i],
                true
            );
        }

        return;
    }

    // Check taskbar-owned menus before hover so an actual right-click menu
    // always wins over the generic bottom-edge hover state.
    bool taskbarPopupOpen = false;

    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        if (
            IsTaskbarPopupOpen(
                g_taskbarStates[i].hwnd
            ) ||
            IsTaskbarShellPopupForeground(
                g_taskbarStates[i].hwnd
            )
        ) {
            taskbarPopupOpen = true;
            break;
        }
    }

    if (taskbarPopupOpen) {
        g_hideDeadline = 0;
        g_shownDueToHover = false;
        g_hoverMonitor = nullptr;

        /*
         * Only keep the taskbar whose own popup is active visible.
         * Other displays continue following their independent state.
         */
        for (size_t i = 0; i < g_taskbarStateCount; ++i) {
            TaskbarMonitorState& state =
                g_taskbarStates[i];

            bool ownPopup =
                IsTaskbarPopupOpen(state.hwnd) ||
                IsTaskbarShellPopupForeground(state.hwnd);

            bool show =
                ownPopup ||
                !state.desktopOnly ||
                !ShouldHideMonitor(
                    state.monitorNumber
                );

            SetTaskbarState(
                state,
                show
            );
        }

        return;
    }

    if (hovering) {
        g_hideDeadline = 0;
        g_shownDueToHover = true;
        g_hoverMonitor = cursorMonitor;

        for (size_t i = 0; i < g_taskbarStateCount; ++i) {
            TaskbarMonitorState& state =
                g_taskbarStates[i];

            bool show =
                !state.desktopOnly ||
                !ShouldHideMonitor(
                    state.monitorNumber
                ) ||
                state.monitor == g_hoverMonitor;

            SetTaskbarState(
                state,
                show
            );
        }

        return;
    }

    if (g_shownDueToHover) {
        ULONGLONG now = GetTickCount64();

        /*
         * A hover reveal belongs to one monitor. Crossing to another display
         * cancels the old monitor's temporary hover ownership immediately.
         */
        if (
            !g_hoverMonitor ||
            cursorMonitor != g_hoverMonitor
        ) {
            for (size_t i = 0; i < g_taskbarStateCount; ++i) {
                TaskbarMonitorState& state =
                    g_taskbarStates[i];

                SetTaskbarState(
                    state,
                    !state.desktopOnly ||
                    !ShouldHideMonitor(
                        state.monitorNumber
                    )
                );
            }

            g_hideDeadline = 0;
            g_shownDueToHover = false;
            g_hoverMonitor = nullptr;

            return;
        }

        if (g_hideDeadline == 0) {
            g_hideDeadline =
                now +
                settings.autoHideDelayMs;
        }

        if (
            g_hideDeadline != 0 &&
            now < g_hideDeadline
        ) {
            for (size_t i = 0; i < g_taskbarStateCount; ++i) {
                TaskbarMonitorState& state =
                    g_taskbarStates[i];

                bool show =
                    !state.desktopOnly ||
                    !ShouldHideMonitor(
                        state.monitorNumber
                    ) ||
                    state.monitor == g_hoverMonitor;

                SetTaskbarState(
                    state,
                    show
                );
            }

            return;
        }

        for (size_t i = 0; i < g_taskbarStateCount; ++i) {
            TaskbarMonitorState& state =
                g_taskbarStates[i];

            SetTaskbarState(
                state,
                !state.desktopOnly ||
                !ShouldHideMonitor(
                    state.monitorNumber
                )
            );
        }

        g_hideDeadline = 0;
        g_shownDueToHover = false;
        g_hoverMonitor = nullptr;
        return;
    }

    /*
     * Each taskbar now follows its own monitor's desktop/application state.
     * Only selected displays are hidden while they are desktop-only.
     */
    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        TaskbarMonitorState& state =
            g_taskbarStates[i];

        SetTaskbarState(
            state,
            !state.desktopOnly ||
            !ShouldHideMonitor(
                state.monitorNumber
            )
        );
    }
    g_hoverMonitor = nullptr;
}


void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event,
                            HWND hwnd, LONG idObject, LONG idChild,
                            DWORD idEventThread, DWORD dwmsEventTime) {
    if (idObject != OBJID_WINDOW || event != EVENT_SYSTEM_FOREGROUND) {
        return;
    }

    // Quick, instant reaction for the common case (switching to a real
    // window). The poll timer re-verifies everything ~10x/sec regardless,
    // which is what reliably catches the minimize/close-last-window case.
    RefreshDesktopState();
    UpdateTaskbarState();
}

void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent,
                         DWORD dwTime) {
    RefreshDesktopState();
    UpdateTaskbarState();
}

// WINEVENT_OUTOFCONTEXT and a polling SetTimer both need a thread that
// pumps messages, so we spin up a dedicated thread for this instead of
// relying on whichever thread happens to call Wh_ModInit.
DWORD WINAPI HookThread(LPVOID param) {
    g_hWinEventHookForeground = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);

    if (!g_hWinEventHookForeground) {
        Wh_Log(L"SetWinEventHook(EVENT_SYSTEM_FOREGROUND) failed; using 100 ms polling fallback");
    }

    RefreshDesktopState();
    UpdateTaskbarState();

    const UINT kPollIntervalMs = 100;
    UINT_PTR timerId = SetTimer(nullptr, 0, kPollIntervalMs, TimerProc);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (timerId) {
        KillTimer(nullptr, timerId);
    }
    if (g_hWinEventHookForeground) {
        UnhookWinEvent(g_hWinEventHookForeground);
        g_hWinEventHookForeground = nullptr;
    }

    return 0;
}

void LoadSettings() {
    int extraHoverMarginMm =
        Wh_GetIntSetting(
            L"extraHoverMarginMm"
        );

    if (extraHoverMarginMm < 0) {
        extraHoverMarginMm = 0;
    } else if (extraHoverMarginMm > 50) {
        extraHoverMarginMm = 50;
    }

    settings.extraHoverMarginMm =
        extraHoverMarginMm;

    int autoHideDelayMs =
        Wh_GetIntSetting(
            L"autoHideDelayMs"
        );

    if (autoHideDelayMs < 0) {
        autoHideDelayMs = 0;
    } else if (autoHideDelayMs > 10000) {
        autoHideDelayMs = 10000;
    }

    settings.autoHideDelayMs =
        static_cast<DWORD>(
            autoHideDelayMs
        );

    settings.hideAllMonitors = false;

    for (
        size_t i = 1;
        i <= kMaxMonitorNumbers;
        ++i
    ) {
        settings.hideMonitor[i] = false;
        settings.hoverMonitor[i] = false;
    }

    settings.hoverAllMonitors = false;

    bool foundSelection = false;

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

        foundSelection = true;

        if (
            wcscmp(
                value,
                L"all"
            ) == 0
        ) {
            settings.hideAllMonitors = true;
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
                number <=
                    static_cast<long>(
                        kMaxMonitorNumbers
                    )
            ) {
                settings.hideMonitor[number] = true;
            }
        }

        Wh_FreeStringSetting(value);
    }

    if (!foundSelection) {
        settings.hideAllMonitors = true;
    }

    // Empty hover selection intentionally means disabled.
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

        if (
            wcscmp(
                value,
                L"all"
            ) == 0
        ) {
            settings.hoverAllMonitors = true;
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
                number <=
                    static_cast<long>(
                        kMaxMonitorNumbers
                    )
            ) {
                settings.hoverMonitor[number] = true;
            }
        }

        Wh_FreeStringSetting(value);
    }
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    g_hThread =
        CreateThread(
            nullptr,
            0,
            HookThread,
            nullptr,
            0,
            &g_threadId
        );

    if (!g_hThread) {
        Wh_Log(L"CreateThread failed");
        return FALSE;
    }

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_hThread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hThread, 3000);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }

    // Always leave the taskbar visible when the mod is disabled/unloaded.
    /*
     * Always restore every taskbar when the mod is unloaded. The state list
     * may be empty if Explorer recreated the taskbar during shutdown, so also
     * use the existing direct visibility helper as a fallback.
     */
    SetTaskbarVisibility(true);
}

// The mod settings were changed, reload them.
void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}

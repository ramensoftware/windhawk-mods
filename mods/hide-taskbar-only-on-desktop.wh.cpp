// ==WindhawkMod==
// @id              hide-taskbar-only-on-desktop
// @name            Hide Taskbar Only on Desktop
// @description     Hides selected taskbars while their displays show only the desktop
// @version         5.8.0
// @author          Sahil Dashoni
// @github          https://github.com/Sahil-Dashoni
// @include         windhawk.exe
// @compilerOptions -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

# Hide Taskbar Only on Desktop

This Windhawk mod hides selected taskbars when their corresponding display is showing only the desktop. Each display is evaluated independently.

## Demo

### Multiple Displays

![Multiple Display](https://raw.githubusercontent.com/Sahil-Dashoni/Hide-Taskbar-Only-on-Desktop-Windhawk-Mod/refs/heads/main/Assets/multiple-display.gif)

### Single Display

![Single Display](https://raw.githubusercontent.com/Sahil-Dashoni/Hide-Taskbar-Only-on-Desktop-Windhawk-Mod/refs/heads/main/Assets/single-display.gif)

## How It Works

For each selected display, the mod checks whether a relevant visible, non-minimized application is present. Supported Windows shell surfaces and desktop infrastructure are excluded from the normal application check.

When a display is showing only the desktop, its selected bottom-docked taskbar can be hidden. An application on that display, keyboard-driven taskbar focus such as Win+T or Win+B, or supported shell interaction can keep the taskbar visible. Mouse interaction with the taskbar does not prevent it from hiding after hover dismissal.

Applications spanning multiple displays are considered for every display they intersect, so each affected display can independently remain visible.

The taskbar is hidden using layered-window transparency rather than Windows' native auto-hide mode, so the mod does not intentionally change the desktop work area.

## Per-Display Settings

You can configure independently:

- Which displays should hide their taskbar on the desktop
- Which displays should support bottom-edge hover reveal

You can select all displays or individual logical display numbers. The settings use the current logical monitor order rather than the internal Windows `DISPLAYn` device identifier.

Display selections are evaluated using the current logical monitor numbering each time the display list is refreshed. If Windows changes the logical display order after a display is added, removed, or rearranged, the configured display numbers follow the new current numbering.

## Hover Reveal

For bottom-docked taskbars, moving the cursor into the configured bottom-edge area reveals the taskbar. The hover zone follows the taskbar's actual height and display scaling, with an optional extra margin.

After the cursor leaves the area, the taskbar hides again after the configured delay. Moving the cursor between displays also updates which taskbar is currently revealed.

Hover tracking uses a dedicated cursor-sampling thread. It samples faster when hover tracking is needed, backs off when it is not needed, and backs off further after repeated cursor-position failures. Hover-eligible taskbars that would currently be hidden are published to the sampler so cursor-leave detection does not depend solely on the periodic safety poll.

Hover reveal does not apply to taskbars docked to the top or sides. Desktop-based hiding also applies only to bottom-docked taskbars.

## Windows Shell Interactions

The mod recognizes supported Windows shell surfaces separately from normal application windows. This prevents shell UI from being mistaken for an ordinary application when deciding whether a display is desktop-only.

Supported shell surfaces include:

- Start menu
- Taskbar menus and popups
- Tray and notification overflow
- Notification and Quick Settings surfaces
- Alt+Tab and related task-switching UI

Supported shell popup classes are checked during hover dismissal. A supported popup can keep the corresponding taskbar visible while it is being dismissed, and the currently hovered taskbar remains visible during that grace period as well.

The taskbar is treated as occupied when it receives keyboard-driven foreground focus, preventing keyboard navigation such as `Win+T`, `Win+B`, or `Win+number` from operating on an invisible taskbar. A taskbar focused by mouse interaction can still hide normally after the hover grace period ends.

## Taskbar State and Recovery

The mod uses `WS_EX_LAYERED` with `SetLayeredWindowAttributes` and alpha 0 to hide the taskbar without changing its normal `ShowWindow` visibility state. It runs the state-management logic in a dedicated `windhawk.exe` tool-mod process rather than injecting a taskbar `ShowWindow` hook into Explorer.

Before hiding a taskbar, the mod records the relevant original extended-window style and layered-window attributes on the taskbar itself. An ownership marker identifies taskbars whose transparency was applied by this mod.

If a taskbar is recreated, the new taskbar is rediscovered and evaluated again. If the dedicated tool process is restarted after an unexpected termination, a new instance can reclaim taskbars still carrying the ownership marker. If another component removes `WS_EX_LAYERED` while the mod still has ownership, the stale ownership data is discarded so the current taskbar state can be captured again safely on a later hide.

## Multi-Monitor Behavior

Each selected display is evaluated independently. For example, an application can remain open on display 1 while the selected taskbar on display 2 hides because display 2 is showing only the desktop.

An application spanning multiple displays keeps the taskbars on every intersected display visible. A taskbar can also be revealed independently by hovering its own configured bottom-edge area.

The mod supports up to 16 display/taskbar entries and uses the current logical display numbering reported by monitor enumeration.

## Performance and Refreshing

The full application and display scan runs in the dedicated tool process rather than inside Explorer.

The mod uses:

- A dedicated worker thread for state management
- A lightweight cursor-sampling thread for hover detection
- Event-driven refreshes for relevant foreground, minimize/move, window show/hide/destroy, display, theme, settings, and taskbar recreation changes
- A periodic 250 ms safety poll for missed or unusual transitions
- A one-shot timer for hover dismissal

The 250 ms safety poll is intentionally retained as a fallback and does not replace the normal event-driven refresh path. Native Windows taskbar auto-hide state is cached and refreshed when settings or relevant shell/taskbar changes occur rather than being queried on every safety tick.

When no displays are configured for desktop-based hiding, the mod skips the application and shell-popup scans and restores any taskbars that may still be hidden by an earlier configuration.

## Limitations

- Desktop-based hiding and hover reveal are supported only for bottom-docked taskbars.
- Hiding the taskbar does not increase the desktop work area, so maximized windows may still leave the normal taskbar space reserved.
- Windows display device names such as `\\.\DISPLAY1` may differ from the logical display numbers used by the settings UI.
- The display-selection configuration supports up to 16 display entries.
- The mod keeps Windows' native taskbar auto-hide setting separate from its own hiding behavior. If native auto-hide is enabled, this mod does not take over that taskbar.
- Because the taskbar is made fully transparent, flashing taskbar buttons and tray notifications are not visually available while that taskbar is hidden by the mod.
- If the dedicated tool process is terminated unexpectedly, a taskbar may remain invisible and click-through until the mod is started again or the taskbar is otherwise recreated; the next mod instance can reclaim marked taskbars.
- Other taskbar transparency/customization mods that modify the same taskbar window can conflict with this mod.
- Windows shell window classes and processes can change between Windows releases, so shell-interaction detection may need updates for future Windows versions.

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
    Select the displays by their current logical number (Display 1, Display 2, and
    so on). Internal Windows device identifiers can differ and are not used for
    the selection number. Select All displays to enable it everywhere, or replace
    it with individual displays.
  $options:
  - all: All displays
  - monitor1: Display 1
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
    Select one or more displays using their current logical display number. Internal
    Windows `\\.\DISPLAYn` identifiers are not used for the selection number.
    These numbers may differ from the display numbers shown in Windows Display
    Settings. Choose All displays to hide every connected display. Only
    bottom-docked taskbars participate in desktop-based hiding. Use Add to
    select multiple displays. Selections use the current logical display
    numbering, so the selected number may refer to a different physical
    display after Windows changes the display order.
  $options:
  - all: All displays
  - monitor1: Display 1
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
#include <shellapi.h>
#include <windhawk_utils.h>
#include <wchar.h>


constexpr size_t kMaxMonitorNumbers = 16;
constexpr size_t kMaxTaskbars = 16;

constexpr UINT WM_APP_REFRESH = WM_APP + 1;
constexpr UINT WM_APP_SETTINGS = WM_APP + 2;
constexpr UINT WM_APP_NATIVE_AUTOHIDE = WM_APP + 3;
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
    bool hiddenByMod;
};

struct WindowScanResult {
    bool applicationOnMonitor[kMaxMonitorNumbers];
};

HWINEVENTHOOK g_foregroundHook = nullptr;
HWINEVENTHOOK g_minimizeHook = nullptr;
HWINEVENTHOOK g_moveHook = nullptr;
HWINEVENTHOOK g_objectHook = nullptr;

HANDLE g_workerThread = nullptr;
DWORD g_workerThreadId = 0;
HANDLE g_workerReadyEvent = nullptr;
HANDLE g_cursorThread = nullptr;
HANDLE g_cursorStopEvent = nullptr;

struct CursorHoverSnapshot {
    HMONITOR monitor;
    RECT monitorRect;
    int hotZonePx;
};

CursorHoverSnapshot g_cursorHoverSnapshots[kMaxTaskbars] = {};
size_t g_cursorHoverSnapshotCount = 0;
SRWLOCK g_cursorHoverSnapshotLock = SRWLOCK_INIT;

// A layered window with alpha 0 is used as the physical taskbar visibility
// mechanism. The taskbar state machine remains in the dedicated
// Windhawk tool process.
//
// The properties below are deliberately stored on the taskbar window itself so
// a newly started tool process can reclaim a taskbar hidden by an older process.
constexpr wchar_t kTaskbarOwnershipProp[] =
    L"windhawk-hide-taskbar-only-on-desktop-ownership";
constexpr wchar_t kTaskbarOriginalExStyleProp[] =
    L"windhawk-hide-taskbar-only-on-desktop-original-exstyle";
constexpr wchar_t kTaskbarOriginalLayeredColorKeyProp[] =
    L"windhawk-hide-taskbar-only-on-desktop-original-color-key";
constexpr wchar_t kTaskbarOriginalLayeredAlphaProp[] =
    L"windhawk-hide-taskbar-only-on-desktop-original-alpha";
constexpr wchar_t kTaskbarOriginalLayeredFlagsProp[] =
    L"windhawk-hide-taskbar-only-on-desktop-original-layered-flags";
constexpr wchar_t kTaskbarOriginalLayeredAttributesValidProp[] =
    L"windhawk-hide-taskbar-only-on-desktop-original-layered-valid";

bool GetWindowUlongPtrProp(
    HWND hwnd,
    const wchar_t* name,
    ULONG_PTR* value
) {
    if (!hwnd || !name || !value) {
        return false;
    }

    HANDLE prop = GetPropW(hwnd, name);
    if (!prop) {
        return false;
    }

    *value = reinterpret_cast<ULONG_PTR>(prop) - 1;
    return true;
}

bool SetWindowUlongPtrProp(
    HWND hwnd,
    const wchar_t* name,
    ULONG_PTR value
) {
    return SetPropW(
        hwnd,
        name,
        reinterpret_cast<HANDLE>(value + 1)
    ) != 0;
}

void RemoveTaskbarOwnershipProperties(HWND hwnd) {
    if (!hwnd) {
        return;
    }

    RemovePropW(hwnd, kTaskbarOwnershipProp);
    RemovePropW(hwnd, kTaskbarOriginalExStyleProp);
    RemovePropW(hwnd, kTaskbarOriginalLayeredColorKeyProp);
    RemovePropW(hwnd, kTaskbarOriginalLayeredAlphaProp);
    RemovePropW(hwnd, kTaskbarOriginalLayeredFlagsProp);
    RemovePropW(hwnd, kTaskbarOriginalLayeredAttributesValidProp);
}

bool MakeTaskbarTransparent(HWND hwnd, bool hide) {
    if (!hwnd || !IsWindow(hwnd)) {
        return false;
    }

    SetLastError(ERROR_SUCCESS);
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (exStyle == 0 && GetLastError() != ERROR_SUCCESS) {
        return false;
    }

    if (hide) {
        // If another component removed WS_EX_LAYERED while this mod still had
        // ownership, the saved properties no longer describe the current window
        // state. Drop the stale ownership and recapture the current state below.
        if (GetPropW(hwnd, kTaskbarOwnershipProp) != nullptr &&
            (exStyle & WS_EX_LAYERED) != 0) {
            return SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA) != 0;
        }

        if (GetPropW(hwnd, kTaskbarOwnershipProp) != nullptr) {
            RemoveTaskbarOwnershipProperties(hwnd);
        }

        COLORREF colorKey = 0;
        BYTE alpha = 255;
        DWORD layeredFlags = 0;
        const bool originalLayered =
            (exStyle & WS_EX_LAYERED) != 0;
        const bool originalLayeredAttributesValid =
            originalLayered &&
            GetLayeredWindowAttributes(
                hwnd,
                &colorKey,
                &alpha,
                &layeredFlags
            ) != FALSE;

        if (!SetWindowUlongPtrProp(
                hwnd,
                kTaskbarOriginalExStyleProp,
                static_cast<ULONG_PTR>(exStyle))) {
            Wh_Log(L"Failed to save original taskbar extended style for 0x%p", hwnd);
            return false;
        }

        if (!SetWindowUlongPtrProp(
                hwnd,
                kTaskbarOriginalLayeredColorKeyProp,
                static_cast<ULONG_PTR>(colorKey)) ||
            !SetWindowUlongPtrProp(
                hwnd,
                kTaskbarOriginalLayeredAlphaProp,
                static_cast<ULONG_PTR>(alpha)) ||
            !SetWindowUlongPtrProp(
                hwnd,
                kTaskbarOriginalLayeredFlagsProp,
                static_cast<ULONG_PTR>(layeredFlags)) ||
            !SetWindowUlongPtrProp(
                hwnd,
                kTaskbarOriginalLayeredAttributesValidProp,
                originalLayeredAttributesValid ? 1 : 0)) {
            RemoveTaskbarOwnershipProperties(hwnd);
            Wh_Log(L"Failed to save original layered attributes for 0x%p", hwnd);
            return false;
        }

        // Mark ownership before changing the window so another tool process
        // can reclaim the taskbar even if this process terminates immediately
        // after this point.
        if (!SetPropW(hwnd, kTaskbarOwnershipProp, reinterpret_cast<HANDLE>(1))) {
            RemoveTaskbarOwnershipProperties(hwnd);
            Wh_Log(L"Failed to mark taskbar ownership for 0x%p", hwnd);
            return false;
        }

        SetLastError(ERROR_SUCCESS);
        LONG_PTR previousExStyle = SetWindowLongPtrW(
            hwnd,
            GWL_EXSTYLE,
            exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT
        );
        if (previousExStyle == 0 && GetLastError() != ERROR_SUCCESS) {
            RemoveTaskbarOwnershipProperties(hwnd);
            Wh_Log(
                L"SetWindowLongPtrW(GWL_EXSTYLE, hide flags) failed for 0x%p: %lu",
                hwnd,
                GetLastError()
            );
            return false;
        }

        if (!SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA)) {
            // Restore the original style if applying the hiding operation failed.
            SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);
            RemoveTaskbarOwnershipProperties(hwnd);
            Wh_Log(
                L"SetLayeredWindowAttributes(alpha=0) failed for 0x%p: %lu",
                hwnd,
                GetLastError()
            );
            return false;
        }

        SetWindowPos(
            hwnd,
            nullptr,
            0, 0, 0, 0,
            SWP_NOMOVE |
            SWP_NOSIZE |
            SWP_NOZORDER |
            SWP_NOACTIVATE |
            SWP_FRAMECHANGED |
            SWP_ASYNCWINDOWPOS
        );

        return true;
    }

    if (GetPropW(hwnd, kTaskbarOwnershipProp) == nullptr) {
        return false;
    }

    ULONG_PTR originalExStyleValue = 0;
    ULONG_PTR originalColorKeyValue = 0;
    ULONG_PTR originalAlphaValue = 255;
    ULONG_PTR originalFlagsValue = 0;
    ULONG_PTR originalLayeredAttributesValidValue = 0;

    const bool haveOriginalExStyle = GetWindowUlongPtrProp(
        hwnd,
        kTaskbarOriginalExStyleProp,
        &originalExStyleValue
    );
    const bool haveOriginalColorKey = GetWindowUlongPtrProp(
        hwnd,
        kTaskbarOriginalLayeredColorKeyProp,
        &originalColorKeyValue
    );
    const bool haveOriginalAlpha = GetWindowUlongPtrProp(
        hwnd,
        kTaskbarOriginalLayeredAlphaProp,
        &originalAlphaValue
    );
    const bool haveOriginalFlags = GetWindowUlongPtrProp(
        hwnd,
        kTaskbarOriginalLayeredFlagsProp,
        &originalFlagsValue
    );
    const bool haveOriginalLayeredValid = GetWindowUlongPtrProp(
        hwnd,
        kTaskbarOriginalLayeredAttributesValidProp,
        &originalLayeredAttributesValidValue
    );

    if (!haveOriginalExStyle) {
        Wh_Log(L"Missing original taskbar style for owned taskbar 0x%p", hwnd);
        return false;
    }

    const LONG_PTR originalExStyle =
        static_cast<LONG_PTR>(originalExStyleValue);
    const bool originalLayered =
        (originalExStyle & WS_EX_LAYERED) != 0;
    const bool originalAttributesValid =
        haveOriginalLayeredValid &&
        originalLayeredAttributesValidValue != 0 &&
        haveOriginalColorKey &&
        haveOriginalAlpha &&
        haveOriginalFlags;

    bool restoredAttributes = true;
    if (originalLayered) {
        if (originalAttributesValid) {
            restoredAttributes = SetLayeredWindowAttributes(
                hwnd,
                static_cast<COLORREF>(originalColorKeyValue),
                static_cast<BYTE>(originalAlphaValue),
                static_cast<DWORD>(originalFlagsValue)
            ) != FALSE;
        } else {
            // GetLayeredWindowAttributes can fail for a layered window that was
            // not configured through SetLayeredWindowAttributes. In that case
            // the exact original alpha state is unknowable, so prefer restoring
            // visibility rather than leaving the taskbar at the mod's alpha=0.
            restoredAttributes = SetLayeredWindowAttributes(
                hwnd,
                0,
                255,
                LWA_ALPHA
            ) != FALSE;
        }
    } else {
        restoredAttributes = SetLayeredWindowAttributes(
            hwnd,
            0,
            255,
            LWA_ALPHA
        ) != FALSE;
    }

    if (!restoredAttributes) {
        Wh_Log(
            L"Failed to restore layered attributes for owned taskbar 0x%p: %lu",
            hwnd,
            GetLastError()
        );
        return false;
    }

    constexpr LONG_PTR kModExStyleBits =
        WS_EX_LAYERED | WS_EX_TRANSPARENT;

    SetLastError(ERROR_SUCCESS);
    LONG_PTR currentExStyle = GetWindowLongPtrW(
        hwnd,
        GWL_EXSTYLE
    );
    if (currentExStyle == 0 && GetLastError() != ERROR_SUCCESS) {
        return false;
    }

    LONG_PTR previousExStyle = SetWindowLongPtrW(
        hwnd,
        GWL_EXSTYLE,
        (currentExStyle & ~kModExStyleBits) |
        (originalExStyle & kModExStyleBits)
    );
    if (previousExStyle == 0 && GetLastError() != ERROR_SUCCESS) {
        Wh_Log(
            L"SetWindowLongPtrW(restore original style) failed for 0x%p: %lu",
            hwnd,
            GetLastError()
        );
        return false;
    }

    SetWindowPos(
        hwnd,
        nullptr,
        0, 0, 0, 0,
        SWP_NOMOVE |
        SWP_NOSIZE |
        SWP_NOZORDER |
        SWP_NOACTIVATE |
        SWP_FRAMECHANGED |
        SWP_ASYNCWINDOWPOS
    );

    RemoveTaskbarOwnershipProperties(hwnd);
    return true;
}

HWND g_workerMessageWindow = nullptr;
ATOM g_workerWindowClassAtom = 0;
UINT g_taskbarCreatedMessage = 0;
TaskbarMonitorState g_taskbarStates[kMaxTaskbars] = {};
size_t g_taskbarStateCount = 0;
bool g_nativeAutoHideEnabled = false;
bool g_appBarRegistered = false;

bool g_hoverActive = false;
HMONITOR g_hoverMonitor = nullptr;
ULONGLONG g_hoverDeadline = 0;
HWND g_keyboardFocusedTaskbar = nullptr;
bool g_suppressHoverUntilCursorLeaves = false;

LONG g_refreshPosted = 0;
void LoadSettings();
void WhTool_ModUninit();
void ArmHoverExpireTimer(DWORD delayMs);
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

int GetMonitorNumber(
    const MonitorList& list,
    HMONITOR monitor
) {
    for (size_t i = 0; i < list.count; ++i) {
        if (list.entries[i].monitor == monitor) {
            // The settings UI is intentionally based on the logical monitor
            // order, not the internal Windows DISPLAYn device identifier.
            return static_cast<int>(i + 1);
        }
    }

    return 0;
}

bool IsBottomDockedTaskbar(HWND hTaskbar, HMONITOR monitor);

bool IsMonitorSelected(
    int monitorNumber,
    const bool* selected
) {
    return
        selected &&
        monitorNumber >= 1 &&
        monitorNumber <= static_cast<int>(kMaxMonitorNumbers) &&
        selected[monitorNumber];
}

bool ShouldHideMonitor(
    const TaskbarMonitorState& state
) {
    if (g_settings.hideAllMonitors) {
        return true;
    }

    return IsMonitorSelected(
        state.monitorNumber,
        g_settings.hideMonitor
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
        g_settings.hoverMonitor
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

    if (
    IsTaskbarPopupClass(className) &&
    (IsExplorerProcess(pid) || IsKnownShellProcess(pid))
) {
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

    if (IsShellSurfaceWindow(
            hwnd,
            className
        )) {
        return false;
    }

    if (exStyle & WS_EX_TOOLWINDOW) {
        return false;
    }

    // Background title-less helper surfaces are ignored. A title-less
    // application becomes covered by the foreground-window path.
    if (!((exStyle & WS_EX_APPWINDOW) != 0 ||
          GetWindowTextLengthW(hwnd) > 0)) {
        return false;
    }

    BOOL transparent = FALSE;
    if (
        SUCCEEDED(
            DwmGetWindowAttribute(
                hwnd,
                DWMWA_CLOAKED,
                &transparent,
                sizeof(transparent)
            )
        ) &&
        transparent
    ) {
        return false;
    }

    return true;
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

    bool allMonitorsOccupied = true;
    for (size_t i = 0; i < context->monitors->count; ++i) {
        if (!context->result->applicationOnMonitor[i]) {
            allMonitorsOccupied = false;
            break;
        }
    }

    if (allMonitorsOccupied && context->monitors->count != 0) {
        return FALSE;
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
            BOOL transparent = FALSE;

            if (
                !(
                    SUCCEEDED(
                        DwmGetWindowAttribute(
                            foreground,
                            DWMWA_CLOAKED,
                            &transparent,
                            sizeof(transparent)
                        )
                    ) &&
                    transparent
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
        state.hiddenByMod = false;

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

    // Restore any previously hidden taskbar that was not rediscovered.
    // A transient enumeration failure must not leave a valid taskbar stranded.
    for (size_t i = 0; i < oldCount; ++i) {
        if (!oldStates[i].hiddenByMod ||
            !oldStates[i].hwnd ||
            !IsWindow(oldStates[i].hwnd)) {
            continue;
        }

        bool rediscovered = false;
        for (size_t j = 0; j < g_taskbarStateCount; ++j) {
            if (g_taskbarStates[j].hwnd == oldStates[i].hwnd) {
                rediscovered = true;
                break;
            }
        }

        if (!rediscovered) {
            MakeTaskbarTransparent(oldStates[i].hwnd, false);
        }
    }

}

bool IsNativeAutoHideEnabled() {
    APPBARDATA data = {};
    data.cbSize = sizeof(data);

    return (SHAppBarMessage(ABM_GETSTATE, &data) & ABS_AUTOHIDE) != 0;
}

void RefreshNativeAutoHideState() {
    g_nativeAutoHideEnabled = IsNativeAutoHideEnabled();
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

        SetLastError(ERROR_SUCCESS);
        LONG_PTR exStyle = GetWindowLongPtrW(
            state.hwnd,
            GWL_EXSTYLE
        );
        if (exStyle == 0 && GetLastError() != ERROR_SUCCESS) {
            return;
        }

        if (!(exStyle & WS_EX_LAYERED)) {
            // Another component removed the layering style while the taskbar
            // was owned by this mod. Drop the stale marker so a later hide can
            // recapture the taskbar's current state instead of using stale data.
            RemoveTaskbarOwnershipProperties(state.hwnd);
            state.hiddenByMod = false;
            return;
        }

        if (MakeTaskbarTransparent(state.hwnd, false)) {
            state.hiddenByMod = false;
        }

        return;
    }

    if (state.hiddenByMod) {
        SetLastError(ERROR_SUCCESS);
        LONG_PTR exStyle = GetWindowLongPtrW(
            state.hwnd,
            GWL_EXSTYLE
        );
        if (exStyle == 0 && GetLastError() != ERROR_SUCCESS) {
            return;
        }

        if (!(exStyle & WS_EX_LAYERED) ||
            GetPropW(state.hwnd, kTaskbarOwnershipProp) == nullptr) {
            RemoveTaskbarOwnershipProperties(state.hwnd);
            state.hiddenByMod = false;
        } else {
            COLORREF colorKey = 0;
            BYTE alpha = 0;
            DWORD layeredFlags = 0;

            const bool attributesAvailable =
                GetLayeredWindowAttributes(
                    state.hwnd,
                    &colorKey,
                    &alpha,
                    &layeredFlags
                ) != FALSE;

            if (!attributesAvailable ||
                !(layeredFlags & LWA_ALPHA) ||
                alpha != 0) {
                SetLayeredWindowAttributes(
                    state.hwnd,
                    0,
                    0,
                    LWA_ALPHA
                );
            }

            return;
        }
    }

    // Do not take ownership of a taskbar that is already hidden by another
    // mechanism. The mod tracks its own transparency state separately.
    if (!IsWindowVisible(state.hwnd)) {
        return;
    }

    if (MakeTaskbarTransparent(state.hwnd, true)) {
        state.hiddenByMod = true;
    }
}

struct ShellPopupScanResult {
    bool visibleOnMonitor[kMaxMonitorNumbers];
};

struct PopupScanContext {
    const MonitorList* monitors;
    ShellPopupScanResult* result;
};

BOOL CALLBACK ScanVisibleShellPopupsProc(
    HWND hwnd,
    LPARAM lParam
) {
    auto* context = reinterpret_cast<PopupScanContext*>(lParam);
    if (!context || !context->monitors || !context->result ||
        !IsWindowVisible(hwnd)) {
        return TRUE;
    }

    WCHAR className[256] = {};
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0 ||
        !IsTaskbarPopupClass(className)) {
        return TRUE;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!(IsExplorerProcess(pid) || IsKnownShellProcess(pid))) {
        return TRUE;
    }

    RECT rect = {};
    if (!GetWindowRect(hwnd, &rect)) {
        return TRUE;
    }

    for (size_t i = 0; i < context->monitors->count; ++i) {
        RECT intersection = {};
        if (IntersectRect(
                &intersection,
                &rect,
                &context->monitors->entries[i].rect)) {
            context->result->visibleOnMonitor[i] = true;
        }
    }

    return TRUE;
}

void ScanVisibleShellPopupsOnce(
    const MonitorList& monitors,
    ShellPopupScanResult& result
) {
    result = {};

    PopupScanContext context = {&monitors, &result};

    EnumWindows(
        ScanVisibleShellPopupsProc,
        reinterpret_cast<LPARAM>(&context)
    );
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

void UpdateCursorHoverSnapshot() {
    CursorHoverSnapshot snapshots[kMaxTaskbars] = {};
    size_t snapshotCount = 0;

    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        if (snapshotCount >= kMaxTaskbars) {
            break;
        }

        const TaskbarMonitorState& state = g_taskbarStates[i];

        if (!ShouldRevealOnHover(state) ||
            !state.desktopOnly ||
            !ShouldHideTaskbar(state) ||
            !IsBottomDockedTaskbar(state.hwnd, state.monitor)) {
            continue;
        }

        CursorHoverSnapshot& snapshot = snapshots[snapshotCount++];
        snapshot.monitor = state.monitor;

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

        if (snapshot.monitor != cursorMonitor) {
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

    RefreshTaskbarMonitorStates(
        monitors
    );

    // If no display is configured for desktop-based hiding, there is no reason
    // to continue with cursor or shell-popup work. Reconcile any taskbars that
    // may still be hidden from an earlier configuration before returning.
    if (!g_settings.hideAllMonitors) {
        bool anyHideMonitorSelected = false;
        for (size_t i = 1; i <= kMaxMonitorNumbers; ++i) {
            if (g_settings.hideMonitor[i]) {
                anyHideMonitorSelected = true;
                break;
            }
        }

        if (!anyHideMonitorSelected) {
            g_hoverActive = false;
            g_hoverMonitor = nullptr;
            g_hoverDeadline = 0;
            g_suppressHoverUntilCursorLeaves = false;
            CancelHoverExpireTimer();
            ApplyBaseTaskbarState();
            UpdateCursorHoverSnapshot();
            return;
        }
    }

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

    if (g_keyboardFocusedTaskbar) {
        bool found = false;
        for (size_t i = 0; i < g_taskbarStateCount; ++i) {
            if (g_taskbarStates[i].hwnd == g_keyboardFocusedTaskbar) {
                g_taskbarStates[i].desktopOnly = false;
                found = true;
                break;
            }
        }

        if (!found) {
            g_keyboardFocusedTaskbar = nullptr;
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
        IsPointNearBottomEdge(
            cursorTaskbar,
            cursorMonitor,
            cursorPoint
        );

    if (!cursorInHoverZone) {
        g_suppressHoverUntilCursorLeaves = false;
    }

    const bool hovering =
        cursorInHoverZone &&
        !g_suppressHoverUntilCursorLeaves;

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

        // Publish hover-eligible taskbars that would be hidden so the cursor
        // sampler can also detect the cursor leaving a revealed hover zone.
        UpdateCursorHoverSnapshot();
        return;
    }

    if (g_hoverActive) {
        const ULONGLONG now =
            GetTickCount64();

        if (g_hoverDeadline == 0) {
            g_hoverDeadline =
                now +
                g_settings.autoHideDelayMs;

            ArmHoverExpireTimer(g_settings.autoHideDelayMs);
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

            UpdateCursorHoverSnapshot();
            return;
        }

        ShellPopupScanResult shellPopups = {};
        ScanVisibleShellPopupsOnce(
            monitors,
            shellPopups
        );

        bool shellPopupPresent = false;
        for (size_t i = 0; i < g_taskbarStateCount; ++i) {
            TaskbarMonitorState& state = g_taskbarStates[i];
            for (size_t monitorIndex = 0; monitorIndex < monitors.count; ++monitorIndex) {
                if (monitors.entries[monitorIndex].monitor == state.monitor &&
                    shellPopups.visibleOnMonitor[monitorIndex]) {
                    shellPopupPresent = true;
                    break;
                }
            }
            if (shellPopupPresent) {
                break;
            }
        }

        if (shellPopupPresent) {
            // Keep the revealed taskbar visible while a shell popup/context
            // menu is still open, even when the cursor has moved outside the
            // popup. The popup itself is what keeps the interaction alive.
            for (size_t i = 0; i < g_taskbarStateCount; ++i) {
                TaskbarMonitorState& state =
                    g_taskbarStates[i];

                bool shellPopupOnMonitor = false;
                for (size_t monitorIndex = 0; monitorIndex < monitors.count; ++monitorIndex) {
                    if (monitors.entries[monitorIndex].monitor == state.monitor) {
                        shellPopupOnMonitor =
                            shellPopups.visibleOnMonitor[monitorIndex];
                        break;
                    }
                }

                SetTaskbarState(
                    state,
                    state.monitor == g_hoverMonitor ||
                    !state.desktopOnly ||
                    !ShouldHideTaskbar(state) ||
                    shellPopupOnMonitor
                );
            }

            g_hoverDeadline = now + 100;
            ArmHoverExpireTimer(100);
            UpdateCursorHoverSnapshot();
            return;
        }

        g_hoverActive = false;
        g_hoverMonitor = nullptr;
        g_hoverDeadline = 0;

        CancelHoverExpireTimer();

        for (size_t i = 0; i < g_taskbarStateCount; ++i) {
            TaskbarMonitorState& state =
                g_taskbarStates[i];

            SetTaskbarState(
                state,
                !state.desktopOnly ||
                !ShouldHideTaskbar(state)
            );
        }
        UpdateCursorHoverSnapshot();
        return;
    }

    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        TaskbarMonitorState& state =
            g_taskbarStates[i];

        SetTaskbarState(
            state,
            !state.desktopOnly ||
            !ShouldHideTaskbar(state)
        );
    }

    UpdateCursorHoverSnapshot();
}

void ArmHoverExpireTimer(DWORD delayMs) {
    if (!g_workerMessageWindow) {
        return;
    }

    UINT delay = delayMs == 0 ? 1 : delayMs;

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

bool HasHoverSnapshots() {
    AcquireSRWLockShared(
        &g_cursorHoverSnapshotLock
    );

    const bool result =
        g_cursorHoverSnapshotCount != 0;

    ReleaseSRWLockShared(
        &g_cursorHoverSnapshotLock
    );

    return result;
}

DWORD WINAPI CursorSamplingThread(LPVOID) {
    SetThreadDpiAwarenessContext(
        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
    );

    bool lastHoverZone = false;
    HMONITOR lastMonitor = nullptr;
    int cursorPositionFailures = 0;

    for (;;) {
        const bool hoverTrackingActive =
            HasHoverSnapshots();

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
    LONG idChild,
    DWORD,
    DWORD
) {
    if (event == EVENT_SYSTEM_FOREGROUND) {
        g_keyboardFocusedTaskbar = nullptr;

        if (hwnd) {
            WCHAR className[64] = {};
            if (GetClassNameW(
                    hwnd,
                    className,
                    ARRAYSIZE(className)
                ) != 0 &&
                (wcscmp(className, L"Shell_TrayWnd") == 0 ||
                 wcscmp(className, L"Shell_SecondaryTrayWnd") == 0)) {
                POINT cursorPoint = {};
                RECT taskbarRect = {};
                bool cursorOverTaskbar = false;

                if (GetCursorPos(&cursorPoint) &&
                    GetWindowRect(hwnd, &taskbarRect)) {
                    cursorOverTaskbar =
                        PtInRect(&taskbarRect, cursorPoint) != FALSE;
                }

                // A taskbar focused while the cursor is elsewhere is treated
                // as keyboard-driven focus (for example Win+T/Win+B/Win+number).
                // Do not infer keyboard focus during a just-completed minimize
                // while hover suppression is active; that foreground transition
                // can simply be the taskbar taking focus after the app closes.
                // A taskbar focused with the cursor over it is treated as mouse
                // interaction and may hide normally after hover dismissal.
                if (!cursorOverTaskbar && !g_suppressHoverUntilCursorLeaves) {
                    g_keyboardFocusedTaskbar = hwnd;
                }
            }
        }

        PostRefresh();
        return;
    }

    if (event == EVENT_SYSTEM_MINIMIZESTART) {
        POINT cursorPoint = {};
        bool cursorOverTaskbar = false;

        if (GetCursorPos(&cursorPoint)) {
            for (size_t i = 0; i < g_taskbarStateCount; ++i) {
                RECT taskbarRect = {};
                if (GetWindowRect(g_taskbarStates[i].hwnd, &taskbarRect) &&
                    PtInRect(&taskbarRect, cursorPoint) != FALSE) {
                    cursorOverTaskbar = true;
                    break;
                }
            }
        }

        g_keyboardFocusedTaskbar = nullptr;
        g_suppressHoverUntilCursorLeaves = !cursorOverTaskbar;
        g_hoverActive = false;
        g_hoverMonitor = nullptr;
        g_hoverDeadline = 0;
        CancelHoverExpireTimer();
        PostRefresh();
        return;
    }

    if (event == EVENT_SYSTEM_MINIMIZEEND) {
        // The start event already decided whether this was a mouse/taskbar
        // interaction or an external minimize. Reconcile the final state now.
        g_keyboardFocusedTaskbar = nullptr;
        PostRefresh();
        return;
    }

    if (event == EVENT_SYSTEM_MOVESIZEEND) {
        PostRefresh();
        return;
    }

    if (
        event == EVENT_OBJECT_DESTROY &&
        hwnd &&
        hwnd == g_keyboardFocusedTaskbar
    ) {
        g_keyboardFocusedTaskbar = nullptr;
    }

    if (
        (event == EVENT_OBJECT_DESTROY ||
         event == EVENT_OBJECT_SHOW ||
         event == EVENT_OBJECT_HIDE) &&
        hwnd &&
        idObject == OBJID_WINDOW &&
        idChild == CHILDID_SELF
    ) {
        PostRefresh();
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

    if (message == WM_APP_NATIVE_AUTOHIDE) {
        if (wParam == ABN_STATECHANGE) {
            RefreshNativeAutoHideState();
            PostRefresh();
        }
        return 0;
    }

    if (
        message == g_taskbarCreatedMessage ||
        message == WM_DISPLAYCHANGE ||
        message == WM_SETTINGCHANGE ||
        message == WM_THEMECHANGED
    ) {
        if (
            message == g_taskbarCreatedMessage ||
            message == WM_SETTINGCHANGE
        ) {
            RefreshNativeAutoHideState();
        }

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

    APPBARDATA appBar = {};
    appBar.cbSize = sizeof(appBar);
    appBar.hWnd = g_workerMessageWindow;
    appBar.uCallbackMessage = WM_APP_NATIVE_AUTOHIDE;

    g_appBarRegistered =
        SHAppBarMessage(ABM_NEW, &appBar) != 0;

    if (!g_appBarRegistered) {
        Wh_Log(L"SHAppBarMessage(ABM_NEW) failed; native auto-hide state-change notifications unavailable");
    }

    return true;
}

void DestroyWorkerMessageWindow() {
    if (g_appBarRegistered && g_workerMessageWindow) {
        APPBARDATA appBar = {};
        appBar.cbSize = sizeof(appBar);
        appBar.hWnd = g_workerMessageWindow;
        SHAppBarMessage(ABM_REMOVE, &appBar);
        g_appBarRegistered = false;
    }

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

DWORD WINAPI WorkerThread(
    LPVOID
) {
    SetThreadDpiAwarenessContext(
        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
    );

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

    g_objectHook =
        SetWinEventHook(
            EVENT_OBJECT_DESTROY,
            EVENT_OBJECT_HIDE,
            nullptr,
            WinEventProc,
            0,
            0,
            WINEVENT_OUTOFCONTEXT
        );

    UpdateTaskbarState();

    // Keep a true periodic safety poll. It is only a fallback for shell/window
    // transitions that do not produce a usable accessibility event. Refresh
    // events never re-arm this timer, so frequent events cannot postpone it.
    constexpr UINT kSafetyPollIntervalMs = 250;
    UINT_PTR timerId =
        SetTimer(
            nullptr,
            kSafetyTimerId,
            kSafetyPollIntervalMs,
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

                    UpdateTaskbarState();

            continue;
        }

        if (msg.message == WM_APP_REFRESH) {
            // Clear before processing. A new event that arrives during the
            // reconciliation can then set the flag again instead of being
            // silently coalesced into the refresh already in progress.
            InterlockedExchange(
                &g_refreshPosted,
                0
            );

                    UpdateTaskbarState();

            continue;
        }

        if (msg.message == WM_APP_SETTINGS) {
            LoadSettings();
            RefreshNativeAutoHideState();
            UpdateTaskbarState();

            continue;
        }

        TranslateMessage(&msg);
        DispatchMessageW(&msg);

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
    SafeUnhookWinEvent(g_objectHook);

    DestroyWorkerMessageWindow();

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

    // Cache the native auto-hide setting at startup. It is refreshed only when
    // settings or shell/taskbar recreation messages indicate it may have changed.
    RefreshNativeAutoHideState();

    // Recover ownership left by an unexpectedly terminated previous tool
    // process before the new worker starts making visibility decisions.
    RestoreAllTaskbars();

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

        if (g_workerThread) {
            if (!PostThreadMessageW(
                    g_workerThreadId,
                    WM_QUIT,
                    0,
                    0
                )) {
                Wh_Log(
                    L"PostThreadMessageW(WM_QUIT) during startup cleanup failed: %lu",
                    GetLastError()
                );
            }
        }

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

BOOL CALLBACK RestoreMarkedTaskbarProc(HWND hwnd, LPARAM) {
    if (!hwnd || GetPropW(hwnd, kTaskbarOwnershipProp) == nullptr) {
        return TRUE;
    }

    MakeTaskbarTransparent(hwnd, false);
    return TRUE;
}

void RestoreAllTaskbars() {
    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        TaskbarMonitorState& state = g_taskbarStates[i];

        if (!state.hwnd || !state.hiddenByMod) {
            continue;
        }

        if (MakeTaskbarTransparent(state.hwnd, false)) {
            state.hiddenByMod = false;
        }
    }

    // Reclaim taskbars hidden by a previous dedicated tool-process instance.
    // This also covers taskbars that were hidden shortly before an unexpected
    // process termination and therefore never reached the local state table.
    EnumWindows(
        RestoreMarkedTaskbarProc,
        0
    );
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

    SafeCloseHandle(g_workerReadyEvent);

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

    bool isExcluded = false;
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
            isExcluded = true;
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

    if (isExcluded) {
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

    WCHAR commandLine[MAX_PATH + 2 +
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

// ==WindhawkMod==
// @id              hide-taskbar-only-on-desktop
// @name            Hide Taskbar Only on Desktop
// @description     Hides selected taskbars while their displays show only the desktop
// @version         7.3.0
// @author          Sahil Dashoni
// @github          https://github.com/Sahil-Dashoni
// @include         windhawk.exe
// @compilerOptions -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Taskbar Only on Desktop

Hides selected bottom-docked taskbars when their display is showing only the desktop. Each display is evaluated independently.

## Demo

### Multiple Displays

![Multiple Display](https://raw.githubusercontent.com/Sahil-Dashoni/Hide-Taskbar-Only-on-Desktop-Windhawk-Mod/refs/heads/main/Assets/multiple-display.gif)

### Single Display

![Single Display](https://raw.githubusercontent.com/Sahil-Dashoni/Hide-Taskbar-Only-on-Desktop-Windhawk-Mod/refs/heads/main/Assets/single-display.gif)

## Features

- Per-display desktop-only hiding
- Independent bottom-edge hover reveal
- Multi-monitor and spanning-window support
- Keyboard taskbar interaction such as Win+T and Win+B
- Shell UI, taskbar popups, and desktop context-menu activity handled separately
- Per-display borderless fullscreen tracking
- Intentional support for up to 16 logical display entries
- Recovery after taskbar recreation or tool-process restart
- Optional stable monitor interface-name mapping

## Settings

**Extra hover margin (px)** adds to the taskbar-height-based bottom-edge reveal zone and is scaled for the display DPI.

**Auto-hide delay after hover (ms)** controls how long the revealed taskbar stays visible after the cursor leaves the bottom-edge hover zone. It only affects hover dismissal; desktop-only hides remain immediate.

**Reveal taskbar on bottom-edge hover** selects the displays where bottom-edge hover can reveal the taskbar.

**Taskbars to hide on desktop** selects the displays whose taskbars participate in desktop-only hiding.

**Stable monitor interface names** lets you add mappings by selecting a display and pasting its Windows monitor interface name returned by `EnumDisplayDevicesW` with `EDD_GET_DEVICE_INTERFACE_NAME`. Each mapping has its own display selector and text field, so a mapping for Display 10 can be added directly without creating mappings for Displays 1 through 9. Interface mappings reassign the selected display slot to the matching physical monitor. If that monitor replaces another monitor already using that slot, the displaced monitor is automatically assigned to the next available display slot after the currently detected displays; when all 16 slots are occupied, it uses a slot left empty by the remapping.

To find the interface names on Windows, run this in PowerShell:

```powershell
Add-Type @'
using System;
using System.Runtime.InteropServices;
public static class DisplayDevices2 {
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public struct DISPLAY_DEVICE {
        public int cb;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)] public string DeviceName;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)] public string DeviceString;
        public int StateFlags;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)] public string DeviceID;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)] public string DeviceKey;
    }
    [DllImport("user32.dll", CharSet = CharSet.Unicode, EntryPoint = "EnumDisplayDevicesW")]
    public static extern bool EnumDisplayDevices(string lpDevice, uint iDevNum, ref DISPLAY_DEVICE lpDisplayDevice, uint dwFlags);
}
'@
for ($i = 1; $i -le 16; $i++) {
    $m = New-Object DisplayDevices2+DISPLAY_DEVICE
    $m.cb = [Runtime.InteropServices.Marshal]::SizeOf($m)
    if ([DisplayDevices2]::EnumDisplayDevices("\\.\DISPLAY$i", 0, [ref]$m, 1)) {
        Write-Host "Display $i : $($m.DeviceString)"
        Write-Host "Interface : $($m.DeviceID)"
        Write-Host ""
    }
}
```

Paste the full value shown after **Interface :** into the mapping for the display slot you want to attach to that physical monitor.

The mod intentionally supports up to 16 logical display entries. This limit is part of the current fixed settings/state design and is retained intentionally.

## Difference from `taskbar-fade`

`taskbar-fade` and this mod both use layered taskbar transparency and can reveal the taskbar from the bottom edge, so they overlap in mechanism. This mod has a different primary state model: **desktop-only state is evaluated independently for each display and the taskbar hides immediately when that display has no relevant application**. It also carries the per-display fullscreen, shell-surface, keyboard, minimize, and recovery handling needed by that model.

The two mods should not be used on the same taskbar because both modify the taskbar window's transparency/style state.

This mod is intentionally maintained as a separate mod because its desktop-only per-display predicate and display-selection model are its primary behavior, rather than optional details around an idle/fade state. Combining those rules into a fade-oriented state machine would change that mod's primary visibility model rather than simply adding a presentation option.

## Implementation

The state logic runs in a dedicated Windhawk tool process. Taskbars are hidden with layered-window transparency plus click-through behavior instead of Windows' native taskbar auto-hide, so the normal desktop work area is intentionally unchanged.

Fullscreen ownership is tracked per display for borderless monitor-sized windows. Foreground, move/size, and window-location events are used to update fullscreen transitions promptly, while short validation timers and a periodic safety refresh cover transitions that do not produce a single reliable event.

## Limitations

- Desktop-only hiding and hover reveal apply to bottom-docked taskbars.
- The hidden taskbar remains part of the normal work area and is click-through.
- If a taskbar's monitor cannot be resolved during a state refresh, the mod fails safe by leaving that taskbar visible.
- Flashing taskbar buttons and tray notifications are not visible while the taskbar is transparent.
- Native Windows taskbar auto-hide remains separate from this mod; when it is enabled, this mod does not take over that taskbar.
- Other taskbar transparency/style mods can conflict when they modify the same taskbar.
- A visible, monitor-sized, captionless, non-resizable application may be treated as fullscreen.
- If the dedicated tool process terminates unexpectedly while a taskbar is hidden, disable and re-enable this mod in Windhawk or restart Windows Explorer to restore it. A later tool-process startup also performs ownership recovery. If the taskbar window itself no longer exists, restarting Windows Explorer recreates it.
- Windows shell classes/processes can change between Windows releases.
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
    a bit of extra margin above that. The value is scaled for the display DPI, so the default 8 corresponds to 8 physical pixels at 100% scaling. Default 8px.
- autoHideDelayMs: 700
  $name: Auto-hide delay after hover (ms)
  $description: >-
    How long to wait, after moving the mouse away from the bottom-edge
    hover zone, before the taskbar hides again. Only applies to that case
    - other hides (e.g. minimizing the last window) are instant.
    Default 700ms.
- monitorInterfaceMappings:
  - - display: none
      $name: Display
      $options:
      - none: No display selected
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
    - interfaceName: ""
      $name: Monitor interface name
      $description: >-
        Paste the full value after "Interface :" from EnumDisplayDevicesW
        with EDD_GET_DEVICE_INTERFACE_NAME.
- hoverRevealOnMonitors: ["all"]
  $name: Reveal taskbar on bottom-edge hover
  $description: >-
    Select the displays where bottom-edge hovering should reveal the taskbar.
    Select the displays by their current logical number (Display 1, Display 2, and so on).
    An optional stable interface name can be configured in Stable monitor interface names.
    Select All displays to enable it everywhere, or replace
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
    Select one or more displays using their current logical display number.
    Stable monitor interface names can optionally be assigned in the
    Stable monitor interface names setting so an individual selection can stay
    attached to the same physical monitor when the logical display order changes.
    Choose All displays to hide every connected display. Only bottom-docked taskbars
    participate in desktop-based hiding. Use Add to select multiple displays.
    If a selected entry has no interface name configured, its current logical
    display number is used.
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
#include <cstdlib>
constexpr size_t kMaxMonitorNumbers = 16;
constexpr size_t kMonitorInterfaceNameLength = 128;
constexpr size_t kMaxTaskbars = 16;
constexpr UINT WM_APP_REFRESH = WM_APP + 1;
constexpr UINT WM_APP_SETTINGS = WM_APP + 2;
constexpr UINT_PTR kHoverExpireTimerId = 2;
constexpr UINT_PTR kPostMinimizeReassertTimerId = 3;
constexpr UINT_PTR kFullscreenValidationTimerId = 4;
struct MonitorInterfaceMapping {
    int monitorNumber;
    WCHAR interfaceName[kMonitorInterfaceNameLength];
};
struct {
    int extraHoverMarginPx;
    DWORD autoHideDelayMs;
    bool hideAllMonitors;
    bool hideMonitor[kMaxMonitorNumbers + 1];
    MonitorInterfaceMapping monitorInterfaceMappings[kMaxMonitorNumbers];
    size_t monitorInterfaceMappingCount;
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
    bool fullscreenOnMonitor[kMaxMonitorNumbers];
    bool shellSurfaceOnMonitor[kMaxMonitorNumbers];
};
HWINEVENTHOOK g_foregroundHook = nullptr;
HWINEVENTHOOK g_minimizeHook = nullptr;
HWINEVENTHOOK g_moveHook = nullptr;
HWINEVENTHOOK g_fullscreenLocationHooks[kMaxMonitorNumbers] = {};
HWINEVENTHOOK g_shellSurfaceHook = nullptr;
HWINEVENTHOOK g_taskbarFocusHook = nullptr;
HANDLE g_workerThread = nullptr;
DWORD g_workerThreadId = 0;
HANDLE g_workerReadyEvent = nullptr;
LONG g_workerInitializationResult = 0;
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
constexpr LONG_PTR kModTaskbarExStyleBits =
    WS_EX_LAYERED | WS_EX_TRANSPARENT;
bool GetWindowExStyle(HWND hwnd, LONG_PTR* exStyle) {
    if (!hwnd || !exStyle) {
        return false;
    }
    SetLastError(ERROR_SUCCESS);
    *exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    return *exStyle != 0 || GetLastError() == ERROR_SUCCESS;
}
bool GetWindowUlongPtrProp(HWND hwnd, const wchar_t* name, ULONG_PTR* value) {
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
bool SetWindowUlongPtrProp(HWND hwnd, const wchar_t* name, ULONG_PTR value) {
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
bool DropStaleTaskbarOwnership(HWND hwnd, LONG_PTR currentExStyle) {
    if (!hwnd) {
        return false;
    }
    ULONG_PTR originalExStyleValue = 0;
    const bool haveOriginalExStyle = GetWindowUlongPtrProp(
        hwnd,
        kTaskbarOriginalExStyleProp,
        &originalExStyleValue
    );
    LONG_PTR restoredExStyle = currentExStyle;
    if (haveOriginalExStyle) {
        const LONG_PTR originalExStyle =
            static_cast<LONG_PTR>(originalExStyleValue);
        const LONG_PTR bitsAddedByMod =
            kModTaskbarExStyleBits & ~originalExStyle;
        restoredExStyle = currentExStyle & ~bitsAddedByMod;
    }
    if (restoredExStyle != currentExStyle) {
        SetLastError(ERROR_SUCCESS);
        LONG_PTR previousExStyle = SetWindowLongPtrW(
            hwnd,
            GWL_EXSTYLE,
            restoredExStyle
        );
        if (previousExStyle == 0 && GetLastError() != ERROR_SUCCESS) {
            return false;
        }
        SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_ASYNCWINDOWPOS);
    }
    RemoveTaskbarOwnershipProperties(hwnd);
    return true;
}
bool ForceRestoreTaskbar(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return false;
    }
    if (!SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA)) {
        Wh_Log(L"Failed to force taskbar alpha visible for %p", hwnd);
    }
    LONG_PTR exStyle = 0;
    if (!GetWindowExStyle(hwnd, &exStyle)) {
        return false;
    }
    const LONG_PTR restoredExStyle =
        exStyle & ~kModTaskbarExStyleBits;
    if (restoredExStyle != exStyle) {
        SetLastError(ERROR_SUCCESS);
        LONG_PTR previousExStyle = SetWindowLongPtrW(
            hwnd,
            GWL_EXSTYLE,
            restoredExStyle
        );
        if (previousExStyle == 0 && GetLastError() != ERROR_SUCCESS) {
            Wh_Log(L"Failed to force taskbar input visible for %p", hwnd);
            return false;
        }
    }
    RemoveTaskbarOwnershipProperties(hwnd);
    SetWindowPos(
        hwnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
        SWP_FRAMECHANGED | SWP_ASYNCWINDOWPOS
    );
    return true;
}
bool MakeTaskbarTransparent(HWND hwnd, bool hide) {
    if (!hwnd || !IsWindow(hwnd)) {
        return false;
    }
    LONG_PTR exStyle = 0;
    if (!GetWindowExStyle(hwnd, &exStyle)) {
        return false;
    }
    if (hide) {
        const bool ownedByMod =
            GetPropW(hwnd, kTaskbarOwnershipProp) != nullptr;
        if (ownedByMod && (exStyle & WS_EX_LAYERED) == 0) {
            if (!DropStaleTaskbarOwnership(hwnd, exStyle)) {
                return false;
            }
            if (!GetWindowExStyle(hwnd, &exStyle)) {
                return false;
            }
        } else if (ownedByMod) {
            if (!SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA)) {
                return false;
            }
            if ((exStyle & WS_EX_TRANSPARENT) == 0) {
                SetLastError(ERROR_SUCCESS);
                LONG_PTR previousExStyle = SetWindowLongPtrW(
                    hwnd,
                    GWL_EXSTYLE,
                    exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT
                );
                if (previousExStyle == 0 && GetLastError() != ERROR_SUCCESS) {
                    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
                    return false;
                }
                SetWindowPos(
                    hwnd, nullptr, 0, 0, 0, 0,
                    SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                    SWP_NOACTIVATE | SWP_FRAMECHANGED
                );
            }
            return true;
        }
        COLORREF colorKey = 0;
        BYTE alpha = 255;
        DWORD layeredFlags = 0;
        const bool originalLayered =
            (exStyle & WS_EX_LAYERED) != 0;
        const bool originalLayeredAttributesValid =
            originalLayered &&
            GetLayeredWindowAttributes(hwnd, &colorKey, &alpha, &layeredFlags) != FALSE;
        if (originalLayeredAttributesValid && alpha == 0) {
            return false;
        }
        if (!SetWindowUlongPtrProp(
                hwnd,
                kTaskbarOriginalExStyleProp,
                static_cast<ULONG_PTR>(exStyle))) {
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
            return false;
        }
        if (!SetPropW(hwnd, kTaskbarOwnershipProp, reinterpret_cast<HANDLE>(1))) {
            RemoveTaskbarOwnershipProperties(hwnd);
            return false;
        }
// Make the taskbar layered first, but keep it input-enabled while it
// is still visible. Apply alpha=0 before adding WS_EX_TRANSPARENT so
// there is no interval where an opaque taskbar is click-through.
        SetLastError(ERROR_SUCCESS);
        LONG_PTR previousExStyle = SetWindowLongPtrW(
            hwnd,
            GWL_EXSTYLE,
            exStyle | WS_EX_LAYERED
        );
        if (previousExStyle == 0 && GetLastError() != ERROR_SUCCESS) {
            RemoveTaskbarOwnershipProperties(hwnd);
            return false;
        }
        if (!SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA)) {
            SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);
            SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_ASYNCWINDOWPOS);
            RemoveTaskbarOwnershipProperties(hwnd);
            return false;
        }
        SetLastError(ERROR_SUCCESS);
        previousExStyle = SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT);
        if (previousExStyle == 0 && GetLastError() != ERROR_SUCCESS) {
            SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
            SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);
            SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_ASYNCWINDOWPOS);
            RemoveTaskbarOwnershipProperties(hwnd);
            return false;
        }
        SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_ASYNCWINDOWPOS);
        return true;
    }
    if (GetPropW(hwnd, kTaskbarOwnershipProp) == nullptr) {
        return false;
    }
    if ((exStyle & WS_EX_LAYERED) == 0) {
        return DropStaleTaskbarOwnership(hwnd, exStyle);
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
    LONG_PTR currentExStyle = 0;
    if (!GetWindowExStyle(hwnd, &currentExStyle)) {
        return false;
    }
    SetLastError(ERROR_SUCCESS);
    const LONG_PTR inputEnabledExStyle =
        currentExStyle & ~WS_EX_TRANSPARENT;
    LONG_PTR previousExStyle = SetWindowLongPtrW(
        hwnd,
        GWL_EXSTYLE,
        inputEnabledExStyle
    );
    if (previousExStyle == 0 && GetLastError() != ERROR_SUCCESS) {
        return false;
    }
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
            restoredAttributes = SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA) != FALSE;
        }
    } else {
        restoredAttributes = SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA) != FALSE;
    }
    if (!restoredAttributes) {
        return false;
    }
    SetLastError(ERROR_SUCCESS);
    const LONG_PTR restoredExStyle =
        (inputEnabledExStyle &
         ~(WS_EX_LAYERED | WS_EX_TRANSPARENT)) |
        (originalExStyle &
         (WS_EX_LAYERED | WS_EX_TRANSPARENT));
    if (restoredExStyle != inputEnabledExStyle) {
        previousExStyle = SetWindowLongPtrW(hwnd, GWL_EXSTYLE, restoredExStyle);
        if (previousExStyle == 0 && GetLastError() != ERROR_SUCCESS) {
            return false;
        }
    }
    SetWindowPos(
        hwnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
        SWP_FRAMECHANGED | SWP_ASYNCWINDOWPOS
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
bool g_hoverActive = false;
HMONITOR g_hoverMonitor = nullptr;
ULONGLONG g_hoverDeadline = 0;
LONG g_refreshPosted = 0;
UINT g_fullscreenValidationAttempt = 0;
// True while the taskbar is foreground because keyboard navigation focused it.
// This is tracked separately so keyboard taskbar navigation works even when
// the cursor is on another display.
bool g_taskbarForegroundKeyboardActivated = false;
bool g_taskbarForegroundAfterShell = false;
bool g_lastForegroundWasShellSurface = false;
ULONGLONG g_lastMinimizeEventTick = 0;
// True from minimize start until minimize end. Taskbar focus generated by
// Windows during this transition must not be treated as keyboard activation.
bool g_minimizeInProgress = false;
// A fullscreen window is cached only after that window has actually entered
// the foreground. The cache is keyed by the HMONITOR itself rather than by
// monitor-enumeration index. Once claimed, ownership is sticky until an explicit
// fullscreen lifecycle event ends it. Interaction suppression additionally
// requires the cached owner to remain visible and uncloaked.
struct FullscreenMonitorOwner {
    HMONITOR monitor;
    HWND hwnd;
};
FullscreenMonitorOwner g_fullscreenOwners[kMaxMonitorNumbers] = {};
void LoadSettings();
void WhTool_ModUninit();
BOOL CALLBACK RestoreMarkedTaskbarProc(HWND hwnd, LPARAM lParam);
void ArmHoverExpireTimer(DWORD delayMs);
void CancelHoverExpireTimer();
void RestoreAllTaskbars();
bool WaitForThreadWithTimeout(HANDLE thread, DWORD timeoutMs, const wchar_t* threadName);
void SafeUnhookWinEvent(HWINEVENTHOOK& hook);
void InstallFullscreenLocationHook(size_t index);
void InstallTaskbarFocusHook();
void CALLBACK WinEventProc(
    HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD);
bool IsShellChromeClass(const WCHAR* className) {
    if (!className) {
        return false;
    }
    static const WCHAR* kClasses[] = {
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
bool IsDesktopInfrastructureWindow(HWND hwnd, const WCHAR* className) {
    if (!hwnd || !className) {
        return false;
    }
    if (
        wcscmp(className, L"Progman") == 0 ||
        wcscmp(className, L"WorkerW") == 0
    ) {
        return true;
    }
// Windows can make the zero-sized Explorer XAML host foreground when the
// desktop context menu is opened. It is shell infrastructure, not an
// application window. Treat only this transient zero-sized host as desktop
// infrastructure so it cannot turn desktopOnly off and reveal the taskbar.
    if (wcscmp(className, L"XamlExplorerHostIslandWindow_WASDK") == 0) {
        RECT rect = {};
        if (GetWindowRect(hwnd, &rect) &&
            rect.left == rect.right &&
            rect.top == rect.bottom) {
            return true;
        }
    }
    HWND shellWindow = GetShellWindow();
    if (shellWindow && shellWindow == hwnd) {
        return true;
    }
    return false;
}
BOOL CALLBACK CollectMonitorProc(HMONITOR monitor, HDC, LPRECT, LPARAM lParam) {
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
    EnumDisplayMonitors(nullptr, nullptr, CollectMonitorProc, reinterpret_cast<LPARAM>(&list));
    return list;
}
bool GetMonitorInterfaceName(HMONITOR monitor, WCHAR* output, size_t outputCount) {
    if (!monitor || !output || outputCount == 0) {
        return false;
    }
    output[0] = L'\0';
    MONITORINFOEXW info = {};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, &info)) {
        return false;
    }
    DISPLAY_DEVICEW device = {};
    device.cb = sizeof(device);
    if (!EnumDisplayDevicesW(
            info.szDevice,
            0,
            &device,
            EDD_GET_DEVICE_INTERFACE_NAME
        )) {
        return false;
    }
    if (!device.DeviceID[0]) {
        return false;
    }
    wcsncpy_s(output, outputCount, device.DeviceID, _TRUNCATE);
    return output[0] != L'\0';
}
void BuildMonitorSelectionSlots(
    const MonitorList& monitors,
    int* selectionSlotForMonitor
) {
    if (!selectionSlotForMonitor) {
        return;
    }

    for (size_t i = 0; i < monitors.count; ++i) {
        selectionSlotForMonitor[i] = 0;
    }

    if (monitors.count == 0) {
        return;
    }

    WCHAR interfaceNames[kMaxMonitorNumbers][kMonitorInterfaceNameLength] = {};
    bool haveInterfaceName[kMaxMonitorNumbers] = {};
    for (size_t i = 0; i < monitors.count; ++i) {
        haveInterfaceName[i] = GetMonitorInterfaceName(
            monitors.entries[i].monitor,
            interfaceNames[i],
            ARRAYSIZE(interfaceNames[i])
        );
    }

    int targetMonitor[kMaxMonitorNumbers + 1] = {};
    for (size_t i = 0; i < g_settings.monitorInterfaceMappingCount; ++i) {
        const MonitorInterfaceMapping& mapping =
            g_settings.monitorInterfaceMappings[i];
        if (mapping.monitorNumber < 1 ||
            mapping.monitorNumber > static_cast<int>(kMaxMonitorNumbers) ||
            !mapping.interfaceName[0]) {
            continue;
        }

        int matchedMonitor = -1;
        for (size_t monitorIndex = 0; monitorIndex < monitors.count; ++monitorIndex) {
            if (haveInterfaceName[monitorIndex] &&
                _wcsicmp(
                    interfaceNames[monitorIndex],
                    mapping.interfaceName
                ) == 0) {
                matchedMonitor = static_cast<int>(monitorIndex);
                break;
            }
        }
        if (matchedMonitor >= 0) {
            // When duplicate targets exist, the last configured mapping wins.
            targetMonitor[mapping.monitorNumber] = matchedMonitor + 1;
        }
    }

    // A physical monitor can only occupy one display slot. If duplicate
    // mappings point the same monitor at multiple slots, the highest slot
    // processed last wins and the earlier slot becomes available again.
    int targetForMonitor[kMaxMonitorNumbers] = {};
    for (int slot = 1; slot <= static_cast<int>(kMaxMonitorNumbers); ++slot) {
        int monitorNumber = targetMonitor[slot];
        if (monitorNumber < 1 ||
            monitorNumber > static_cast<int>(monitors.count)) {
            continue;
        }

        const int monitorIndex = monitorNumber - 1;
        if (targetForMonitor[monitorIndex] != 0) {
            targetMonitor[targetForMonitor[monitorIndex]] = 0;
        }
        targetForMonitor[monitorIndex] = slot;
    }

    bool slotOccupied[kMaxMonitorNumbers + 1] = {};
    for (int slot = 1; slot <= static_cast<int>(kMaxMonitorNumbers); ++slot) {
        int monitorNumber = targetMonitor[slot];
        if (monitorNumber < 1 ||
            monitorNumber > static_cast<int>(monitors.count)) {
            continue;
        }
        const int monitorIndex = monitorNumber - 1;
        selectionSlotForMonitor[monitorIndex] = slot;
        slotOccupied[slot] = true;
    }

    // Preserve the original logical slot for monitors that were not moved.
    // A displaced monitor gets the next slot after the currently detected
    // displays, so mapping Display 2 to the physical monitor currently
    // occupying Display 3 moves the old Display 2 to Display 4 on a
    // three-monitor system instead of colliding with Display 3.
    int nextOverflowSlot = static_cast<int>(monitors.count) + 1;
    for (size_t monitorIndex = 0; monitorIndex < monitors.count; ++monitorIndex) {
        if (selectionSlotForMonitor[monitorIndex] != 0) {
            continue;
        }

        const int originalSlot = static_cast<int>(monitorIndex) + 1;
        if (originalSlot <= static_cast<int>(kMaxMonitorNumbers) &&
            !slotOccupied[originalSlot]) {
            selectionSlotForMonitor[monitorIndex] = originalSlot;
            slotOccupied[originalSlot] = true;
            continue;
        }

        while (nextOverflowSlot <= static_cast<int>(kMaxMonitorNumbers) &&
               slotOccupied[nextOverflowSlot]) {
            ++nextOverflowSlot;
        }
        if (nextOverflowSlot <= static_cast<int>(kMaxMonitorNumbers)) {
            selectionSlotForMonitor[monitorIndex] = nextOverflowSlot;
            slotOccupied[nextOverflowSlot] = true;
            ++nextOverflowSlot;
            continue;
        }

        // With all 16 slots occupied, fall back to any slot left empty by a
        // remap so the displaced monitor remains individually selectable.
        for (int slot = 1;
             slot <= static_cast<int>(kMaxMonitorNumbers);
             ++slot) {
            if (!slotOccupied[slot]) {
                selectionSlotForMonitor[monitorIndex] = slot;
                slotOccupied[slot] = true;
                break;
            }
        }
    }
}
bool IsBottomDockedTaskbar(HWND hTaskbar, HMONITOR monitor);
int FindMonitorIndex(const MonitorList& monitors, HMONITOR monitor);
int ParseMonitorNumber(const WCHAR* value) {
    if (!value || wcsncmp(value, L"monitor", 7) != 0) {
        return 0;
    }
    wchar_t* endNumber = nullptr;
    long number = wcstol(value + 7, &endNumber, 10);
    return endNumber && *endNumber == L'\0' &&
                   number >= 1 &&
                   number <= static_cast<long>(kMaxMonitorNumbers)
               ? static_cast<int>(number)
               : 0;
}
bool IsMonitorSelected(
    int monitorNumber,
    const bool* selected
) {
    if (!selected ||
        monitorNumber < 1 ||
        monitorNumber > static_cast<int>(kMaxMonitorNumbers)) {
        return false;
    }
    return selected[monitorNumber];
}
bool ShouldHideMonitor(const TaskbarMonitorState& state) {
    return
        g_settings.hideAllMonitors ||
        IsMonitorSelected(
            state.monitorNumber,
            g_settings.hideMonitor
        );
}
bool ShouldRevealOnHover(const TaskbarMonitorState& state) {
    return
        g_settings.hoverAllMonitors ||
        IsMonitorSelected(
            state.monitorNumber,
            g_settings.hoverMonitor
        );
}
bool GetWindowProcessImageName(DWORD pid, wchar_t* output, size_t outputCount) {
    if (!pid || !output || outputCount == 0) {
        return false;
    }
    output[0] = L'\0';
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }
    DWORD size =
        static_cast<DWORD>(outputCount);
    BOOL result =
        QueryFullProcessImageNameW(process, 0, output, &size);
    CloseHandle(process);
    return result && output[0] != L'\0';
}
enum class ShellProcessKind {
    None,
    Explorer,
    KnownShell,
};
ShellProcessKind GetShellProcessKind(DWORD pid) {
    wchar_t imagePath[MAX_PATH] = {};
    if (!GetWindowProcessImageName(
            pid,
            imagePath,
            ARRAYSIZE(imagePath)
        )) {
        return ShellProcessKind::None;
    }
    const wchar_t* baseName = wcsrchr(imagePath, L'\\');
    baseName = baseName ? baseName + 1 : imagePath;
    if (_wcsicmp(baseName, L"explorer.exe") == 0) {
        return ShellProcessKind::Explorer;
    }
    static const wchar_t* kKnownShellProcesses[] = {
        L"StartMenuExperienceHost.exe",
        L"ShellExperienceHost.exe",
        L"ShellHost.exe",
        L"SearchHost.exe",
        L"SearchApp.exe",
    };
    for (const wchar_t* name : kKnownShellProcesses) {
        if (_wcsicmp(baseName, name) == 0) {
            return ShellProcessKind::KnownShell;
        }
    }
    return ShellProcessKind::None;
}
constexpr size_t kMaxShellProcessCacheEntries = 64;
struct ShellProcessKindCache {
    DWORD pids[kMaxShellProcessCacheEntries] = {};
    ShellProcessKind kinds[kMaxShellProcessCacheEntries] = {};
    size_t count = 0;
};
ShellProcessKind GetShellProcessKindCached(
    ShellProcessKindCache& cache,
    DWORD pid
) {
    if (!pid) {
        return ShellProcessKind::None;
    }
    for (size_t i = 0; i < cache.count; ++i) {
        if (cache.pids[i] == pid) {
            return cache.kinds[i];
        }
    }
    const ShellProcessKind kind = GetShellProcessKind(pid);
    if (cache.count < kMaxShellProcessCacheEntries) {
        cache.pids[cache.count] = pid;
        cache.kinds[cache.count] = kind;
        ++cache.count;
    }
    return kind;
}
bool IsWindowCloaked(HWND hwnd) {
    BOOL cloaked = FALSE;
    return
        SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
        cloaked;
}
bool IsTaskbarPopupClass(const WCHAR* className) {
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
bool IsAltTabClass(const WCHAR* className) {
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
bool IsTaskbarWindow(HWND hwnd);
bool IsPopupOwnedByTaskbar(HWND hwnd);
bool IsShellSurfaceWindow(
    HWND hwnd,
    const WCHAR* className,
    ShellProcessKind processKind
) {
    if (
        !hwnd ||
        !className ||
        !IsWindowVisible(hwnd)
    ) {
        return false;
    }
    const bool isShellProcess =
        processKind == ShellProcessKind::Explorer ||
        processKind == ShellProcessKind::KnownShell;
    if (IsTaskbarPopupClass(className) && isShellProcess) {
        const bool genericPopup =
            wcscmp(className, L"#32768") == 0 ||
            wcscmp(className, L"#32771") == 0 ||
            wcscmp(className, L"Xaml_WindowedPopupClass") == 0;
        return !genericPopup || IsPopupOwnedByTaskbar(hwnd);
    }
    if (IsAltTabClass(className) && isShellProcess) {
        return true;
    }
    const bool isXamlHost =
        wcsncmp(
            className,
            L"XamlExplorerHostIslandWindow",
            wcslen(L"XamlExplorerHostIslandWindow")
        ) == 0;
    if (isXamlHost && isShellProcess) {
        if (wcscmp(className, L"XamlExplorerHostIslandWindow") == 0) {
            return true;
        }
        RECT rect = {};
        return
            GetWindowRect(hwnd, &rect) &&
            rect.right > rect.left &&
            rect.bottom > rect.top;
    }
    return
        wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0 &&
        processKind == ShellProcessKind::KnownShell;
}
bool IsPopupOwnedByTaskbar(HWND hwnd) {
    HWND owner = GetWindow(hwnd, GW_OWNER);
    while (owner) {
        if (IsTaskbarWindow(GetAncestor(owner, GA_ROOT))) {
            return true;
        }
        owner = GetWindow(owner, GW_OWNER);
    }
    return false;
}
bool MarkShellSurfaceOnMonitors(HWND hwnd, const WCHAR* className, ShellProcessKind processKind, const MonitorList& monitors, bool* shellSurfaceOnMonitor) {
    if (
        !shellSurfaceOnMonitor ||
        !IsShellSurfaceWindow(hwnd, className, processKind)
    ) {
        return false;
    }
    if (IsWindowCloaked(hwnd)) {
        return true;
    }
    RECT rect = {};
    if (!GetWindowRect(hwnd, &rect)) {
        return true;
    }
    for (size_t i = 0; i < monitors.count; ++i) {
        RECT intersection = {};
        if (IntersectRect(
                &intersection,
                &rect,
                &monitors.entries[i].rect
            )) {
            shellSurfaceOnMonitor[i] = true;
        }
    }
    return true;
}
bool IsApplicationWindowCandidate(HWND hwnd, const WCHAR* className, ShellProcessKind processKind) {
    if (
        !hwnd ||
        !className ||
        !IsWindowVisible(hwnd) ||
        IsIconic(hwnd)
    ) {
        return false;
    }
    LONG_PTR exStyle =
        GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (
        GetWindow(hwnd, GW_OWNER) != nullptr &&
        !(exStyle & WS_EX_APPWINDOW)
    ) {
        return false;
    }
    if (IsDesktopInfrastructureWindow(
            hwnd,
            className
        )) {
        return false;
    }
    if (IsShellSurfaceWindow(
            hwnd,
            className,
            processKind
        ) ||
        IsTaskbarPopupClass(className)) {
        return false;
    }
    if (IsShellChromeClass(className) || IsTaskbarWindow(hwnd)) {
        return false;
    }
    if (exStyle & WS_EX_TOOLWINDOW) {
        return false;
    }
    return !IsWindowCloaked(hwnd);
}
struct ScanContext {
    const MonitorList* monitors;
    WindowScanResult* result;
    ShellProcessKindCache* processCache;
};
bool IsFullscreenWindowForMonitor(
    HWND hwnd,
    const MonitorEntry& monitorEntry,
    ShellProcessKind processKind
) {
    if (
        !hwnd ||
        !IsWindow(hwnd) ||
        !IsWindowVisible(hwnd) ||
        IsIconic(hwnd)
    ) {
        return false;
    }
    WCHAR className[256] = {};
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0) {
        return false;
    }
// Desktop, shell chrome, shell surfaces, and taskbars can also be monitor
// sized and borderless. They must never become fullscreen owners merely
// because their geometry happens to match a monitor.
    if (
        IsDesktopInfrastructureWindow(hwnd, className) ||
        IsShellChromeClass(className) ||
        IsShellSurfaceWindow(hwnd, className, processKind) ||
        IsTaskbarWindow(hwnd)
    ) {
        return false;
    }
    LONG_PTR style =
        GetWindowLongPtrW(hwnd, GWL_STYLE);
// Do not require WS_POPUP here. Chromium/Edge-style borderless fullscreen
// windows can retain a normal overlapped style while removing the caption
// and resize frame. The authoritative signal for this mod is an exact
// monitor-sized rectangle together with the absence of caption/frame.
    if (
        (style & WS_CAPTION) != 0 ||
        (style & WS_THICKFRAME) != 0
    ) {
        return false;
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
        return false;
    }
    constexpr LONG kFullscreenTolerance = 2;
    return
        abs(rect.left - monitorEntry.rect.left) <= kFullscreenTolerance &&
        abs(rect.top - monitorEntry.rect.top) <= kFullscreenTolerance &&
        abs(rect.right - monitorEntry.rect.right) <= kFullscreenTolerance &&
        abs(rect.bottom - monitorEntry.rect.bottom) <= kFullscreenTolerance;
}
int FindFullscreenOwnerIndex(HMONITOR monitor) {
    if (!monitor) {
        return -1;
    }
    for (size_t i = 0; i < kMaxMonitorNumbers; ++i) {
        if (g_fullscreenOwners[i].monitor == monitor) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
bool IsFullscreenOwnerOnSameMonitor(HWND hwnd, HMONITOR monitor) {
    if (!hwnd || !IsWindow(hwnd) || !monitor) {
        return false;
    }
    return MonitorFromWindow(
        hwnd,
        MONITOR_DEFAULTTONEAREST
    ) == monitor;
}
bool IsFullscreenOwnerVisible(HMONITOR monitor) {
    const int index = FindFullscreenOwnerIndex(monitor);
    if (index < 0 || !g_fullscreenOwners[index].hwnd) {
        return false;
    }
    HWND owner = g_fullscreenOwners[index].hwnd;
    if (!IsFullscreenOwnerOnSameMonitor(owner, monitor) ||
        !IsWindowVisible(owner) || IsIconic(owner)) {
        return false;
    }
    return !IsWindowCloaked(owner);
}
int FindFullscreenOwnerForHookScope(
    DWORD processId,
    DWORD threadId,
    int excludeIndex
) {
    if (!processId || !threadId) {
        return -1;
    }
    for (size_t i = 0; i < kMaxMonitorNumbers; ++i) {
        if (static_cast<int>(i) == excludeIndex ||
            !g_fullscreenOwners[i].hwnd) {
            continue;
        }
        DWORD existingProcessId = 0;
        DWORD existingThreadId =
            GetWindowThreadProcessId(
                g_fullscreenOwners[i].hwnd,
                &existingProcessId
            );
        if (existingProcessId == processId &&
            existingThreadId == threadId) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
int FindFullscreenLocationHookIndex(
    DWORD processId,
    DWORD threadId,
    int excludeIndex
) {
    if (!processId || !threadId) {
        return -1;
    }
    for (size_t i = 0; i < kMaxMonitorNumbers; ++i) {
        if (static_cast<int>(i) == excludeIndex ||
            !g_fullscreenLocationHooks[i] ||
            !g_fullscreenOwners[i].hwnd) {
            continue;
        }
        DWORD existingProcessId = 0;
        DWORD existingThreadId =
            GetWindowThreadProcessId(
                g_fullscreenOwners[i].hwnd,
                &existingProcessId
            );
        if (existingProcessId == processId &&
            existingThreadId == threadId) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
void ClearFullscreenOwnerAtIndex(size_t index) {
    if (index >= kMaxMonitorNumbers) {
        return;
    }
    HWND owner = g_fullscreenOwners[index].hwnd;
    HWINEVENTHOOK hook = g_fullscreenLocationHooks[index];
    DWORD processId = 0;
    DWORD threadId = owner
        ? GetWindowThreadProcessId(owner, &processId)
        : 0;
    g_fullscreenLocationHooks[index] = nullptr;
    g_fullscreenOwners[index] = {};
    if (!hook) {
        return;
    }
    if (processId && threadId) {
        const int existingHookIndex =
            FindFullscreenLocationHookIndex(
                processId,
                threadId,
                static_cast<int>(index)
            );
        if (existingHookIndex >= 0) {
            UnhookWinEvent(hook);
            return;
        }
        const int replacementIndex =
            FindFullscreenOwnerForHookScope(
                processId,
                threadId,
                static_cast<int>(index)
            );
        if (replacementIndex >= 0) {
            g_fullscreenLocationHooks[replacementIndex] = hook;
            return;
        }
    }
    UnhookWinEvent(hook);
}
void InstallFullscreenLocationHook(size_t index) {
    if (index >= kMaxMonitorNumbers || !g_fullscreenOwners[index].hwnd) {
        return;
    }
    SafeUnhookWinEvent(g_fullscreenLocationHooks[index]);
    DWORD processId = 0;
    DWORD threadId =
        GetWindowThreadProcessId(
            g_fullscreenOwners[index].hwnd,
            &processId
        );
    if (!processId || !threadId) {
        return;
    }
    if (FindFullscreenLocationHookIndex(
            processId,
            threadId,
            static_cast<int>(index)
        ) >= 0) {
        return;
    }
    g_fullscreenLocationHooks[index] = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE,
        EVENT_OBJECT_LOCATIONCHANGE,
        nullptr,
        WinEventProc,
        processId,
        threadId,
        WINEVENT_OUTOFCONTEXT
    );
    if (!g_fullscreenLocationHooks[index]) {
        Wh_Log(L"Failed to install fullscreen location WinEvent hook for owner %p", g_fullscreenOwners[index].hwnd);
    }
}
void SetFullscreenOwner(HMONITOR monitor, HWND hwnd) {
    if (!monitor || !hwnd) {
        return;
    }
    int index = FindFullscreenOwnerIndex(monitor);
    if (index < 0) {
        for (size_t i = 0; i < kMaxMonitorNumbers; ++i) {
            if (!g_fullscreenOwners[i].monitor) {
                index = static_cast<int>(i);
                break;
            }
        }
    }
    if (index >= 0) {
        if (g_fullscreenOwners[index].hwnd != hwnd) {
            SafeUnhookWinEvent(g_fullscreenLocationHooks[index]);
        }
        g_fullscreenOwners[index].monitor = monitor;
        g_fullscreenOwners[index].hwnd = hwnd;
        if (!g_fullscreenLocationHooks[index]) {
            InstallFullscreenLocationHook(static_cast<size_t>(index));
        }
    }
}
void ClearFullscreenOwnersForWindow(HWND hwnd) {
    if (!hwnd) {
        return;
    }
    for (size_t i = 0; i < kMaxMonitorNumbers; ++i) {
        if (g_fullscreenOwners[i].hwnd == hwnd) {
            ClearFullscreenOwnerAtIndex(i);
        }
    }
}
void ValidateFullscreenOwnerForMonitor(
    HMONITOR monitor,
    const MonitorList& monitors,
    ShellProcessKindCache& processCache
) {
    const int index = FindFullscreenOwnerIndex(monitor);
    if (index < 0 || !g_fullscreenOwners[index].hwnd) {
        return;
    }
    HWND owner = g_fullscreenOwners[index].hwnd;
    if (!IsFullscreenOwnerOnSameMonitor(owner, monitor)) {
        ClearFullscreenOwnerAtIndex(static_cast<size_t>(index));
        return;
    }
    if (GetForegroundWindow() != owner) {
        return;
    }
    for (size_t monitorIndex = 0;
         monitorIndex < monitors.count;
         ++monitorIndex) {
        if (monitors.entries[monitorIndex].monitor != monitor) {
            continue;
        }
        DWORD pid = 0;
        GetWindowThreadProcessId(owner, &pid);
        if (!IsFullscreenWindowForMonitor(
                owner,
                monitors.entries[monitorIndex],
                GetShellProcessKindCached(processCache, pid)
            )) {
            ClearFullscreenOwnerAtIndex(static_cast<size_t>(index));
        }
        break;
    }
}
void ClearInvalidFullscreenWindowCache(const MonitorList& monitors) {
    for (size_t i = 0; i < kMaxMonitorNumbers; ++i) {
        if (!g_fullscreenOwners[i].monitor) {
            continue;
        }
        HMONITOR monitor = g_fullscreenOwners[i].monitor;
        HWND owner = g_fullscreenOwners[i].hwnd;
        bool monitorStillPresent = false;
        for (size_t monitorIndex = 0;
             monitorIndex < monitors.count;
             ++monitorIndex) {
            if (monitors.entries[monitorIndex].monitor == monitor) {
                monitorStillPresent = true;
                break;
            }
        }
        if (!monitorStillPresent ||
            !IsFullscreenOwnerOnSameMonitor(owner, monitor)) {
            ClearFullscreenOwnerAtIndex(i);
        }
    }
}
bool IsTaskbarWindow(HWND hwnd) {
    if (!hwnd) {
        return false;
    }
    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        if (g_taskbarStates[i].hwnd == hwnd) {
            return true;
        }
    }
    WCHAR className[256] = {};
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0) {
        return false;
    }
    return
        wcscmp(className, L"Shell_TrayWnd") == 0 ||
        wcscmp(className, L"Shell_SecondaryTrayWnd") == 0;
}
void ClearFullscreenOwnerForForegroundApplication(
    const MonitorList& monitors,
    HWND hwnd,
    ShellProcessKindCache& processCache
) {
    WCHAR className[256] = {};
    DWORD pid = 0;
    if (
        !hwnd ||
        GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0
    ) {
        return;
    }
    GetWindowThreadProcessId(hwnd, &pid);
    if (!IsApplicationWindowCandidate(
            hwnd,
            className,
            GetShellProcessKindCached(processCache, pid)
        )) {
        return;
    }
    HMONITOR monitor =
        MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    const int ownerIndex =
        FindFullscreenOwnerIndex(monitor);
    if (ownerIndex < 0 ||
        g_fullscreenOwners[ownerIndex].hwnd == hwnd) {
        ValidateFullscreenOwnerForMonitor(
            monitor,
            monitors,
            processCache
        );
        return;
    }
    ClearFullscreenOwnerAtIndex(static_cast<size_t>(ownerIndex));
}
void NoteForegroundFullscreenWindow(
    const MonitorList& monitors,
    HWND hwnd,
    ShellProcessKindCache& processCache
) {
    if (!hwnd || !IsWindow(hwnd)) {
        return;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    const ShellProcessKind processKind =
        GetShellProcessKindCached(processCache, pid);
    for (size_t i = 0; i < monitors.count; ++i) {
        if (IsFullscreenWindowForMonitor(
                hwnd,
                monitors.entries[i],
                processKind
            )) {
            SetFullscreenOwner(monitors.entries[i].monitor, hwnd);
        }
    }
}
void RefreshFullscreenWindowCache(
    const MonitorList& monitors,
    ShellProcessKindCache& processCache
) {
    ClearInvalidFullscreenWindowCache(monitors);
    HWND foreground = GetForegroundWindow();
    ClearFullscreenOwnerForForegroundApplication(
        monitors,
        foreground,
        processCache
    );
    if (foreground) {
        HMONITOR foregroundMonitor = MonitorFromWindow(
            foreground,
            MONITOR_DEFAULTTONEAREST
        );
        ValidateFullscreenOwnerForMonitor(
            foregroundMonitor,
            monitors,
            processCache
        );
    }
    NoteForegroundFullscreenWindow(
        monitors,
        foreground,
        processCache
    );
}
BOOL CALLBACK ScanWindowsWithMonitorsProc(HWND hwnd, LPARAM lParam) {
    ScanContext* context =
        reinterpret_cast<ScanContext*>(lParam);
    if (
        !context ||
        !context->monitors ||
        !context->result
    ) {
        return TRUE;
    }
    bool allMonitorsClassified = true;
    bool anyFullscreenMonitor = false;
    for (size_t i = 0; i < context->monitors->count; ++i) {
        if (context->result->fullscreenOnMonitor[i]) {
            anyFullscreenMonitor = true;
        }
        if (
            !context->result->applicationOnMonitor[i] &&
            !context->result->fullscreenOnMonitor[i]
        ) {
            allMonitorsClassified = false;
            break;
        }
    }
    if (
        allMonitorsClassified &&
        !anyFullscreenMonitor &&
        context->monitors->count != 0
    ) {
        return FALSE;
    }
    WCHAR className[256] = {};
    if (
        GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0
    ) {
        return TRUE;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    const ShellProcessKind processKind =
        GetShellProcessKindCached(*context->processCache, pid);
    if (MarkShellSurfaceOnMonitors(
            hwnd,
            className,
            processKind,
            *context->monitors,
            context->result->shellSurfaceOnMonitor
        )) {
        return TRUE;
    }
    if (!IsApplicationWindowCandidate(
            hwnd,
            className,
            processKind
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
        GetWindowPlacement(hwnd, &placement) &&
        placement.showCmd == SW_SHOWMAXIMIZED
    ) {
        HMONITOR windowMonitor =
            MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
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
            IntersectRect(&intersection, &rect, &context->monitors->entries[i].rect)
        ) {
            context->result->applicationOnMonitor[i] = true;
        }
    }
    return TRUE;
}
void ScanWindowsOnce(const MonitorList& monitors, WindowScanResult& result) {
    result = {};
    ShellProcessKindCache processCache = {};
    RefreshFullscreenWindowCache(monitors, processCache);
    for (size_t i = 0; i < monitors.count; ++i) {
        result.fullscreenOnMonitor[i] =
            IsFullscreenOwnerVisible(monitors.entries[i].monitor);
    }
    ScanContext context = {
        &monitors,
        &result,
        &processCache
    };
    EnumWindows(ScanWindowsWithMonitorsProc, reinterpret_cast<LPARAM>(&context));
    HWND foreground =
        GetForegroundWindow();
    WCHAR foregroundClassName[256] = {};
    bool foregroundIsShellSurface = false;
    if (
        foreground &&
        GetClassNameW(foreground, foregroundClassName, ARRAYSIZE(foregroundClassName)) != 0
    ) {
        DWORD foregroundPid = 0;
        GetWindowThreadProcessId(foreground, &foregroundPid);
        const ShellProcessKind foregroundProcessKind =
            GetShellProcessKindCached(processCache, foregroundPid);
        foregroundIsShellSurface =
            IsShellSurfaceWindow(
                foreground,
                foregroundClassName,
                foregroundProcessKind
            );
        if (foregroundIsShellSurface) {
            const HMONITOR foregroundMonitor =
                MonitorFromWindow(foreground, MONITOR_DEFAULTTONEAREST);
            for (size_t i = 0; i < monitors.count; ++i) {
                if (monitors.entries[i].monitor == foregroundMonitor) {
                    result.shellSurfaceOnMonitor[i] = true;
                    break;
                }
            }
        } else {
            MarkShellSurfaceOnMonitors(
                foreground,
                foregroundClassName,
                foregroundProcessKind,
                monitors,
                result.shellSurfaceOnMonitor
            );
        }
    }
    if (
        foreground &&
        IsWindowVisible(foreground) &&
        !IsIconic(foreground) &&
        foregroundClassName[0] != L'\0' &&
        !IsDesktopInfrastructureWindow(
            foreground,
            foregroundClassName
        ) &&
        !IsShellChromeClass(foregroundClassName) &&
        !foregroundIsShellSurface &&
        !IsTaskbarPopupClass(foregroundClassName) &&
        !IsTaskbarWindow(foreground)
    ) {
        if (!IsWindowCloaked(foreground)) {
            HMONITOR foregroundMonitor =
                MonitorFromWindow(foreground, MONITOR_DEFAULTTONEAREST);
            for (
                size_t i = 0;
                i < monitors.count;
                ++i
            ) {
                if (
                    monitors.entries[i].monitor ==
                    foregroundMonitor
                ) {
                    if (!result.fullscreenOnMonitor[i]) {
                        result.applicationOnMonitor[i] = true;
                    }
                    break;
                }
            }
        }
    }
}
void RefreshTaskbarMonitorStates(const MonitorList& monitors) {
    int selectionSlots[kMaxMonitorNumbers] = {};
    BuildMonitorSelectionSlots(monitors, selectionSlots);

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
            MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        TaskbarMonitorState state = {};
        state.hwnd = hwnd;
        state.monitor = monitor;
        const int logicalMonitorIndex =
            FindMonitorIndex(monitors, monitor);
        state.monitorNumber =
            logicalMonitorIndex >= 0
                ? selectionSlots[logicalMonitorIndex]
                : 0;
        state.desktopOnly = false;
        state.hiddenByMod = false;
        for (size_t i = 0; i < oldCount; ++i) {
            if (oldStates[i].hwnd == hwnd) {
                state.desktopOnly =
                    oldStates[i].desktopOnly;
                state.hiddenByMod = oldStates[i].hiddenByMod;
                break;
            }
        }
        g_taskbarStates[
            g_taskbarStateCount++
        ] = state;
    };
    HWND primary = nullptr;
    while (
        (primary = FindWindowExW(
            nullptr,
            primary,
            L"Shell_TrayWnd",
            nullptr
        )) != nullptr
    ) {
        addTaskbar(primary);
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
        addTaskbar(secondary);
    }
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
            if (!MakeTaskbarTransparent(oldStates[i].hwnd, false)) {
                Wh_Log(L"Undiscovered taskbar restore failed for %p", oldStates[i].hwnd);
                ForceRestoreTaskbar(oldStates[i].hwnd);
            }
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
bool ShouldHideTaskbar(const TaskbarMonitorState& state) {
    return
        state.hwnd &&
        state.monitor &&
        ShouldHideMonitor(state) &&
        IsBottomDockedTaskbar(state.hwnd, state.monitor) &&
        !g_nativeAutoHideEnabled;
}
void SetTaskbarState(TaskbarMonitorState& state, bool show) {
    if (!state.hwnd || !IsWindow(state.hwnd)) {
        return;
    }
    if (show) {
        if (!state.hiddenByMod) {
            return;
        }
        LONG_PTR exStyle = 0;
        if (!GetWindowExStyle(state.hwnd, &exStyle)) {
            return;
        }
        if (!(exStyle & WS_EX_LAYERED)) {
            if (DropStaleTaskbarOwnership(state.hwnd, exStyle)) {
                state.hiddenByMod = false;
            }
            return;
        }
        if (MakeTaskbarTransparent(state.hwnd, false)) {
            state.hiddenByMod = false;
        } else {
            Wh_Log(L"Taskbar restore failed for %p, forcing visible", state.hwnd);
            if (ForceRestoreTaskbar(state.hwnd)) {
                state.hiddenByMod = false;
            } else {
                Wh_Log(L"Taskbar force-restore failed for %p", state.hwnd);
            }
        }
        return;
    }
    if (state.hiddenByMod) {
        LONG_PTR exStyle = 0;
        if (!GetWindowExStyle(state.hwnd, &exStyle)) {
            return;
        }
        if (!(exStyle & WS_EX_LAYERED) ||
            GetPropW(state.hwnd, kTaskbarOwnershipProp) == nullptr) {
            if (DropStaleTaskbarOwnership(state.hwnd, exStyle)) {
                state.hiddenByMod = false;
            }
        } else {
            COLORREF colorKey = 0;
            BYTE alpha = 0;
            DWORD layeredFlags = 0;
            const bool attributesAvailable =
                GetLayeredWindowAttributes(state.hwnd, &colorKey, &alpha, &layeredFlags) != FALSE;
            if (!attributesAvailable ||
                !(layeredFlags & LWA_ALPHA) ||
                alpha != 0) {
                if (!SetLayeredWindowAttributes(state.hwnd, 0, 0, LWA_ALPHA)) {
                    Wh_Log(L"Failed to keep taskbar transparent for %p", state.hwnd);
                    if (ForceRestoreTaskbar(state.hwnd)) {
                        state.hiddenByMod = false;
                    }
                    return;
                }
            }
            if (GetWindowExStyle(state.hwnd, &exStyle) &&
                !(exStyle & WS_EX_TRANSPARENT)) {
                SetLastError(ERROR_SUCCESS);
                if (SetWindowLongPtrW(
                        state.hwnd,
                        GWL_EXSTYLE,
                        exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT
                    ) == 0 &&
                    GetLastError() != ERROR_SUCCESS) {
                    Wh_Log(L"Failed to restore taskbar click-through style for %p", state.hwnd);
                    if (ForceRestoreTaskbar(state.hwnd)) {
                        state.hiddenByMod = false;
                    }
                }
            }
            return;
        }
    }
    if (!IsWindowVisible(state.hwnd)) {
        return;
    }
    if (MakeTaskbarTransparent(state.hwnd, true)) {
        state.hiddenByMod = true;
    } else {
        Wh_Log(L"Taskbar hide failed for %p", state.hwnd);
    }
}
struct ShellPopupScanResult {
    bool visibleOnMonitor[kMaxMonitorNumbers];
};
struct PopupScanContext {
    const MonitorList* monitors;
    ShellPopupScanResult* result;
    ShellProcessKindCache* processCache;
};
BOOL CALLBACK ScanVisibleShellPopupsProc(HWND hwnd, LPARAM lParam) {
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
    if (GetShellProcessKindCached(*context->processCache, pid) ==
        ShellProcessKind::None) {
        return TRUE;
    }
    if (IsWindowCloaked(hwnd)) {
        return TRUE;
    }
    const bool genericPopup =
        wcscmp(className, L"#32768") == 0 ||
        wcscmp(className, L"#32771") == 0 ||
        wcscmp(className, L"Xaml_WindowedPopupClass") == 0;
    if (genericPopup && !IsPopupOwnedByTaskbar(hwnd)) {
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
void ScanVisibleShellPopupsOnce(const MonitorList& monitors, ShellPopupScanResult& result) {
    result = {};
    ShellProcessKindCache processCache = {};
    PopupScanContext context = {&monitors, &result, &processCache};
    EnumWindows(ScanVisibleShellPopupsProc, reinterpret_cast<LPARAM>(&context));
}
int FindMonitorIndex(const MonitorList& monitors, HMONITOR monitor) {
    if (!monitor) {
        return -1;
    }
    for (size_t i = 0; i < monitors.count; ++i) {
        if (monitors.entries[i].monitor == monitor) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
int GetHoverZonePx(HWND hTaskbar, UINT dpi) {
    RECT rect = {};
    if (
        hTaskbar &&
        GetWindowRect(hTaskbar, &rect)
    ) {
        int taskbarHeight =
            rect.bottom - rect.top;
        if (taskbarHeight > 0) {
            return
                taskbarHeight +
                MulDiv(g_settings.extraHoverMarginPx, static_cast<int>(dpi), 96);
        }
    }
    return
        MulDiv(48, static_cast<int>(dpi), 96) +
        MulDiv(g_settings.extraHoverMarginPx, static_cast<int>(dpi), 96);
}
bool IsBottomDockedTaskbar(HWND hTaskbar, HMONITOR monitor) {
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
bool IsPointNearBottomEdge(HWND hTaskbar, HMONITOR cursorMonitor, POINT pt) {
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
        GetHoverZonePx(hTaskbar, dpi);
    if (hotZonePx < 1) {
        hotZonePx = 1;
    }
    return
        pt.x >= mi.rcMonitor.left &&
        pt.x < mi.rcMonitor.right &&
        pt.y >= mi.rcMonitor.bottom - hotZonePx &&
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
            !IsBottomDockedTaskbar(state.hwnd, state.monitor) ||
            IsFullscreenOwnerVisible(state.monitor)) {
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
bool IsCursorInConfiguredHoverZoneAtSnapshot(POINT pt, HMONITOR cursorMonitor) {
    if (!cursorMonitor) {
        return false;
    }
    AcquireSRWLockShared(&g_cursorHoverSnapshotLock);
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
            pt.x >= snapshot.monitorRect.left &&
            pt.x < snapshot.monitorRect.right &&
            pt.y >=
                snapshot.monitorRect.bottom -
                snapshot.hotZonePx &&
            pt.y < snapshot.monitorRect.bottom;
        break;
    }
    ReleaseSRWLockShared(&g_cursorHoverSnapshotLock);
    return result;
}
void UpdateTaskbarState() {
    MonitorList monitors =
        GetCurrentMonitors();
    RefreshTaskbarMonitorStates(monitors);
    if (!g_settings.hideAllMonitors) {
        bool anyHideMonitorSelected = false;
        for (size_t i = 1; !anyHideMonitorSelected &&
             i <= kMaxMonitorNumbers; ++i) {
            anyHideMonitorSelected = g_settings.hideMonitor[i];
        }
        if (!anyHideMonitorSelected) {
            g_hoverActive = false;
            g_hoverMonitor = nullptr;
            g_hoverDeadline = 0;
            CancelHoverExpireTimer();
            for (size_t i = 0; i < g_taskbarStateCount; ++i) {
                TaskbarMonitorState& state =
                    g_taskbarStates[i];
                SetTaskbarState(state, !state.desktopOnly || !ShouldHideTaskbar(state));
            }
            UpdateCursorHoverSnapshot();
            return;
        }
    }
    WindowScanResult scan = {};
    ScanWindowsOnce(monitors, scan);
    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        TaskbarMonitorState& state =
            g_taskbarStates[i];
        bool monitorKnown = false;
        state.desktopOnly = false;
        for (
            size_t monitorIndex = 0;
            monitorIndex < monitors.count;
            ++monitorIndex
        ) {
            if (
                monitors.entries[monitorIndex].monitor ==
                state.monitor
            ) {
                monitorKnown = true;
                // Fullscreen counts as display activity, so keep the taskbar
                // in the desktop-only state machine while fullscreen is active.
                // Shell UI (such as Start/Search) and explicit keyboard taskbar
                // activation may temporarily reveal it below.
                state.desktopOnly =
                    scan.fullscreenOnMonitor[monitorIndex] ||
                    !scan.applicationOnMonitor[monitorIndex];
                break;
            }
        }
        if (!monitorKnown) {
            // Unknown monitor state must fail safe: do not hide a taskbar
            // when its display could not be classified.
            state.desktopOnly = false;
        }
    }
// Treat the currently foreground taskbar as occupied only when it has
// keyboard-driven focus. A taskbar foreground activation caused by a mouse
// click, including clicking a taskbar icon to minimize/maximize, must not
// prevent a desktop-only taskbar from hiding after hover dismissal.
    HWND foreground = GetForegroundWindow();
// After minimizing an application, Windows may briefly leave the primary
// taskbar as the global foreground window. Treat that foreground state as
// stale for the duration of this post-minimize window instead of allowing
// it to make the primary desktop-only taskbar appear occupied.
    const bool postMinimizeTaskbarForeground =
        g_lastMinimizeEventTick != 0 &&
        GetTickCount64() - g_lastMinimizeEventTick < 1000;
    if (g_lastMinimizeEventTick != 0 && !postMinimizeTaskbarForeground) {
        g_lastMinimizeEventTick = 0;
    }
    const bool taskbarForegroundKeyboardActivated =
        g_taskbarForegroundKeyboardActivated;
    POINT cursorPoint = {};
    HMONITOR cursorMonitor = nullptr;
    if (GetCursorPos(&cursorPoint)) {
        cursorMonitor =
            MonitorFromPoint(cursorPoint, MONITOR_DEFAULTTONEAREST);
    }
    const bool taskbarForegroundAfterShell =
        g_taskbarForegroundAfterShell;
    if (!postMinimizeTaskbarForeground &&
        !taskbarForegroundAfterShell &&
        foreground) {
        for (size_t i = 0; i < g_taskbarStateCount; ++i) {
            if (g_taskbarStates[i].hwnd != foreground) {
                continue;
            }
            if (taskbarForegroundKeyboardActivated) {
                // Explicit keyboard taskbar navigation is allowed to reveal
                // the taskbar even during fullscreen. Once focus returns to
                // the fullscreen application, the foreground handler clears
                // this flag and the normal fullscreen state hides it again.
                g_taskbarStates[i].desktopOnly = false;
            }
            break;
        }
    }
    auto IsShellSurfaceVisibleOnMonitor =
        [&](HMONITOR monitor) {
            const int index = FindMonitorIndex(monitors, monitor);
            // Shell UI is an explicit interaction with the taskbar and is
            // allowed to reveal it even while a fullscreen owner is active.
            return index >= 0 &&
                   scan.shellSurfaceOnMonitor[index];
        };
    HWND cursorTaskbar = nullptr;
    bool cursorHoverConfigured = false;
    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        const TaskbarMonitorState& state = g_taskbarStates[i];
        if (
            state.monitor != cursorMonitor ||
            !state.desktopOnly ||
            !ShouldHideTaskbar(state)
        ) {
            continue;
        }
        cursorTaskbar = state.hwnd;
        cursorHoverConfigured = ShouldRevealOnHover(state);
        break;
    }
    bool cursorMonitorFullscreen = false;
    for (size_t i = 0; i < monitors.count; ++i) {
        if (monitors.entries[i].monitor == cursorMonitor) {
            cursorMonitorFullscreen =
                scan.fullscreenOnMonitor[i];
            break;
        }
    }
    bool hoverMonitorFullscreen = false;
    if (g_hoverMonitor) {
        for (size_t i = 0; i < monitors.count; ++i) {
            if (monitors.entries[i].monitor == g_hoverMonitor) {
                hoverMonitorFullscreen =
                    scan.fullscreenOnMonitor[i];
                break;
            }
        }
    }
// A fullscreen transition invalidates an existing hover reveal on that
// same display. The cursor may already be on another monitor, so use
// g_hoverMonitor rather than cursorMonitor for this check.
    if (g_hoverActive && hoverMonitorFullscreen) {
        g_hoverActive = false;
        g_hoverMonitor = nullptr;
        g_hoverDeadline = 0;
        hoverMonitorFullscreen = false;
        CancelHoverExpireTimer();
    }
    const bool hovering =
        cursorTaskbar &&
        cursorMonitor &&
        cursorHoverConfigured &&
        !cursorMonitorFullscreen &&
        IsPointNearBottomEdge(cursorTaskbar, cursorMonitor, cursorPoint);
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
                (state.monitor == g_hoverMonitor &&
                 !cursorMonitorFullscreen) ||
                !state.desktopOnly ||
                IsShellSurfaceVisibleOnMonitor(state.monitor) ||
                !ShouldHideTaskbar(state)
            );
        }
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
                    (state.monitor == g_hoverMonitor &&
                     !hoverMonitorFullscreen) ||
                    !state.desktopOnly ||
                    IsShellSurfaceVisibleOnMonitor(state.monitor) ||
                    !ShouldHideTaskbar(state)
                );
            }
            UpdateCursorHoverSnapshot();
            return;
        }
        ShellPopupScanResult shellPopups = {};
        const bool ignorePostMinimizeShellPopup =
            g_lastMinimizeEventTick != 0 &&
            GetTickCount64() - g_lastMinimizeEventTick < 1000;
        if (!ignorePostMinimizeShellPopup) {
            ScanVisibleShellPopupsOnce(monitors, shellPopups);
        }
        const int hoverMonitorIndex =
            FindMonitorIndex(monitors, g_hoverMonitor);
        const bool shellPopupPresent =
            hoverMonitorIndex >= 0 &&
            shellPopups.visibleOnMonitor[hoverMonitorIndex];
        if (shellPopupPresent) {
// Keep the revealed taskbar visible while a shell popup/context
// menu is still open, even when the cursor has moved outside the
// popup. The popup itself is what keeps the interaction alive.
// A popup observed immediately after minimize is ignored above so
// stale taskbar UI cannot pin the first post-minimize hover cycle.
            for (size_t i = 0; i < g_taskbarStateCount; ++i) {
                TaskbarMonitorState& state =
                    g_taskbarStates[i];
                const int monitorIndex =
                    FindMonitorIndex(monitors, state.monitor);
                const bool shellPopupOnMonitor =
                    monitorIndex >= 0 &&
                    shellPopups.visibleOnMonitor[monitorIndex];
                const bool stateFullscreenOnMonitor =
                    monitorIndex >= 0 &&
                    scan.fullscreenOnMonitor[monitorIndex];
                SetTaskbarState(
                    state,
                    (state.monitor == g_hoverMonitor &&
                     !hoverMonitorFullscreen) ||
                    !state.desktopOnly ||
                    IsShellSurfaceVisibleOnMonitor(state.monitor) ||
                    !ShouldHideTaskbar(state) ||
                    (shellPopupOnMonitor &&
                     !stateFullscreenOnMonitor)
                );
            }
            g_hoverDeadline = now + 250;
            ArmHoverExpireTimer(250);
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
            SetTaskbarState(state, !state.desktopOnly || IsShellSurfaceVisibleOnMonitor(state.monitor) || !ShouldHideTaskbar(state));
        }
        UpdateCursorHoverSnapshot();
        return;
    }
    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        TaskbarMonitorState& state =
            g_taskbarStates[i];
        const bool shellSurface = IsShellSurfaceVisibleOnMonitor(state.monitor);
        const bool shouldHide = ShouldHideTaskbar(state);
        const bool show =
            !state.desktopOnly ||
            shellSurface ||
            !shouldHide;
        SetTaskbarState(state, show);
    }
    UpdateCursorHoverSnapshot();
}
void ArmHoverExpireTimer(DWORD delayMs) {
    if (!g_workerMessageWindow) {
        return;
    }
    UINT delay = delayMs == 0 ? 1 : delayMs;
    if (!SetTimer(g_workerMessageWindow, kHoverExpireTimerId, delay, nullptr)) {
        Wh_Log(L"Hover expiry timer could not be armed");
    }
}
void CancelHoverExpireTimer() {
    if (g_workerMessageWindow) {
        KillTimer(g_workerMessageWindow, kHoverExpireTimerId);
    }
}
HINSTANCE GetWorkerWindowModuleInstance() {
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&GetWorkerWindowModuleInstance),
            &module
        )) {
        return nullptr;
    }
    return reinterpret_cast<HINSTANCE>(module);
}
void SafeUnhookWinEvent(HWINEVENTHOOK& hook) {
    if (hook) {
        UnhookWinEvent(hook);
        hook = nullptr;
    }
}
void InstallShellSurfaceHook() {
    SafeUnhookWinEvent(g_shellSurfaceHook);
    HWND shellWindow = GetShellWindow();
    if (!shellWindow) {
        return;
    }
    DWORD processId = 0;
    GetWindowThreadProcessId(shellWindow, &processId);
    if (!processId) {
        return;
    }
    g_shellSurfaceHook = SetWinEventHook(
        EVENT_OBJECT_SHOW,
        EVENT_OBJECT_HIDE,
        nullptr,
        WinEventProc,
        processId,
        0,
        WINEVENT_OUTOFCONTEXT
    );
    if (!g_shellSurfaceHook) {
        Wh_Log(L"Failed to install shell surface WinEvent hook");
    }
}
void InstallTaskbarFocusHook() {
    SafeUnhookWinEvent(g_taskbarFocusHook);
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!taskbar) {
        taskbar = GetShellWindow();
    }
    if (!taskbar) {
        return;
    }
    DWORD processId = 0;
    GetWindowThreadProcessId(taskbar, &processId);
    if (!processId) {
        return;
    }
    g_taskbarFocusHook = SetWinEventHook(
        EVENT_OBJECT_FOCUS,
        EVENT_OBJECT_FOCUS,
        nullptr,
        WinEventProc,
        processId,
        0,
        WINEVENT_OUTOFCONTEXT
    );
    if (!g_taskbarFocusHook) {
        Wh_Log(L"Failed to install taskbar focus WinEvent hook");
    }
}
void SafeCloseHandle(HANDLE& handle) {
    if (handle) {
        CloseHandle(handle);
        handle = nullptr;
    }
}
void PostRefresh() {
    if (InterlockedExchange(&g_refreshPosted, 1) != 0) {
        return;
    }
    if (!PostThreadMessageW(
            g_workerThreadId,
            WM_APP_REFRESH,
            0,
            0
        )) {
        InterlockedExchange(&g_refreshPosted, 0);
        Wh_Log(L"Failed to post refresh message to worker thread");
    }
}
bool HasHoverSnapshots() {
    AcquireSRWLockShared(&g_cursorHoverSnapshotLock);
    const bool result =
        g_cursorHoverSnapshotCount != 0;
    ReleaseSRWLockShared(&g_cursorHoverSnapshotLock);
    return result;
}
DWORD WINAPI CursorSamplingThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    bool lastHoverZone = false;
    HMONITOR lastMonitor = nullptr;
    int cursorPositionFailures = 0;
    for (;;) {
        const bool hoverTrackingActive =
            HasHoverSnapshots();
        DWORD waitMs =
            hoverTrackingActive
                ? 50
                : 250;
        if (cursorPositionFailures >= 3) {
            waitMs = 1000;
        }
        DWORD waitResult =
            WaitForSingleObject(g_cursorStopEvent, waitMs);
        if (waitResult == WAIT_OBJECT_0) {
            break;
        }
        if (waitResult == WAIT_FAILED) {
            Wh_Log(L"Cursor sampler wait failed: %lu", GetLastError());
            break;
        }
        POINT pt = {};
        if (!GetCursorPos(&pt)) {
            ++cursorPositionFailures;
            continue;
        }
        cursorPositionFailures = 0;
        HMONITOR monitor =
            MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        const bool hoverZone =
            IsCursorInConfiguredHoverZoneAtSnapshot(pt, monitor);
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
void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
    if (event == EVENT_OBJECT_FOCUS) {
        if (!hwnd) {
            return;
        }
        HWND root = GetAncestor(hwnd, GA_ROOT);
        if (!root) {
            return;
        }
// Ignore a plain window-level focus notification for Shell_TrayWnd.
// Windows can generate that as a side effect of desktop interaction,
// including desktop context-menu invocation. Actual keyboard taskbar
// navigation focuses a taskbar child/control and is still tracked.
        if (root == hwnd &&
            idObject == OBJID_WINDOW &&
            idChild == CHILDID_SELF) {
            return;
        }
// A taskbar child can also receive an accessibility focus event while
// desktop interaction is active. Count it as keyboard taskbar focus
// only when the taskbar root is actually the foreground window. This
// keeps desktop context-menu/focus changes from pinning the taskbar
// visible while preserving Win+T/Win+B style keyboard navigation.
        if (GetForegroundWindow() != root) {
            return;
        }
        if (g_minimizeInProgress) {
            return;
        }
        for (size_t i = 0; i < g_taskbarStateCount; ++i) {
            if (g_taskbarStates[i].hwnd == root) {
                g_taskbarForegroundKeyboardActivated = true;
                g_taskbarForegroundAfterShell = false;
                PostRefresh();
                break;
            }
        }
        return;
    }
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }
    if (event == EVENT_OBJECT_LOCATIONCHANGE) {
        if (!hwnd) {
            return;
        }
        for (size_t i = 0; i < kMaxMonitorNumbers; ++i) {
            if (g_fullscreenOwners[i].hwnd == hwnd) {
                PostRefresh();
                break;
            }
        }
        return;
    }
    if (event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_HIDE) {
        WCHAR className[256] = {};
        if (!hwnd ||
            GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0) {
            return;
        }
        const bool isXamlShell =
            wcsncmp(className, L"XamlExplorerHostIslandWindow", wcslen(L"XamlExplorerHostIslandWindow")) == 0;
        if (!isXamlShell) {
            return;
        }
        if (event == EVENT_OBJECT_SHOW) {
            // Opening Start/Search is an explicit shell interaction and may
            // reveal a desktop-only taskbar even during fullscreen.
            g_taskbarForegroundAfterShell = false;
        } else {
            // When the shell surface closes, release any keyboard/taskbar
            // reveal state left by the interaction so a fullscreen display can
            // return to its normal hidden-taskbar state immediately.
            g_taskbarForegroundKeyboardActivated = false;
            g_taskbarForegroundAfterShell = true;
        }
        PostRefresh();
        return;
    }
    if (event == EVENT_SYSTEM_FOREGROUND) {
        WCHAR foregroundClassName[256] = {};
        if (hwnd) {
            GetClassNameW(hwnd, foregroundClassName, ARRAYSIZE(foregroundClassName));
        }
        const bool postMinimizeTaskbarForeground =
            g_lastMinimizeEventTick != 0 &&
            GetTickCount64() - g_lastMinimizeEventTick < 1000;
        bool isTaskbarForeground = false;
        if (hwnd) {
            for (size_t i = 0; i < g_taskbarStateCount; ++i) {
                if (g_taskbarStates[i].hwnd == hwnd) {
                    isTaskbarForeground = true;
                    break;
                }
            }
        }
        if (!isTaskbarForeground) {
            g_taskbarForegroundAfterShell = false;
            g_taskbarForegroundKeyboardActivated = false;
            DWORD pid = 0;
            if (hwnd) {
                GetWindowThreadProcessId(hwnd, &pid);
            }
            ShellProcessKindCache processCache = {};
            g_lastForegroundWasShellSurface =
                hwnd &&
                foregroundClassName[0] &&
                IsShellSurfaceWindow(
                    hwnd,
                    foregroundClassName,
                    GetShellProcessKindCached(processCache, pid)
                );
            const HMONITOR foregroundMonitor =
                hwnd
                    ? MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST)
                    : nullptr;
            const int fullscreenOwnerIndex =
                FindFullscreenOwnerIndex(foregroundMonitor);
            if (g_workerMessageWindow && fullscreenOwnerIndex >= 0 && g_fullscreenOwners[fullscreenOwnerIndex].hwnd == hwnd) {
                g_fullscreenValidationAttempt = 0;
                if (!SetTimer(g_workerMessageWindow, kFullscreenValidationTimerId, 16, nullptr)) {
                    Wh_Log(L"Fullscreen validation timer could not be armed");
                }
            }
            PostRefresh();
            return;
        }
        const bool wasShellForeground =
            g_lastForegroundWasShellSurface;
        g_lastForegroundWasShellSurface = false;
        if (g_minimizeInProgress || postMinimizeTaskbarForeground) {
            g_taskbarForegroundKeyboardActivated = false;
            g_taskbarForegroundAfterShell = false;
            PostRefresh();
            return;
        }
        POINT cursorPoint = {};
        bool cursorOverTaskbar = false;
        if (GetCursorPos(&cursorPoint)) {
            HWND hit = WindowFromPoint(cursorPoint);
            cursorOverTaskbar = hit && GetAncestor(hit, GA_ROOT) == hwnd;
            if (!cursorOverTaskbar) {
                RECT taskbarRect = {};
                if (GetWindowRect(hwnd, &taskbarRect)) {
                    cursorOverTaskbar = PtInRect(&taskbarRect, cursorPoint) != FALSE;
                }
            }
        }
        const bool mouseButtonDown =
            (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 ||
            (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 ||
            (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
        const bool mouseActivated = cursorOverTaskbar || mouseButtonDown;
        const bool keyboardActivated =
            g_taskbarForegroundKeyboardActivated && !mouseActivated;
        g_taskbarForegroundKeyboardActivated = keyboardActivated;
        g_taskbarForegroundAfterShell =
            wasShellForeground && !mouseActivated && !keyboardActivated;
// A mouse click can reveal the taskbar without first creating a hover
// session. Start the same hover-dismiss lifecycle so leaving the
// taskbar starts the normal auto-hide countdown.
        if (mouseActivated) {
            const HMONITOR taskbarMonitor =
                MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            if (taskbarMonitor) {
                g_hoverActive = true;
                g_hoverMonitor = taskbarMonitor;
                g_hoverDeadline = 0;
                CancelHoverExpireTimer();
            }
        }
        PostRefresh();
        return;
    }
    if (event == EVENT_SYSTEM_MINIMIZESTART) {
        g_minimizeInProgress = true;
        g_lastMinimizeEventTick = GetTickCount64();
        g_taskbarForegroundKeyboardActivated = false;
        g_hoverActive = false;
        g_hoverMonitor = nullptr;
        g_hoverDeadline = 0;
        CancelHoverExpireTimer();
        PostRefresh();
        return;
    }
    if (event == EVENT_SYSTEM_MINIMIZEEND) {
        g_minimizeInProgress = false;
        g_lastMinimizeEventTick = GetTickCount64();
        g_hoverActive = false;
        g_hoverMonitor = nullptr;
        g_hoverDeadline = 0;
        CancelHoverExpireTimer();
        HWND foreground = GetForegroundWindow();
        bool foregroundIsTaskbar = false;
        if (foreground) {
            for (size_t i = 0; i < g_taskbarStateCount; ++i) {
                if (g_taskbarStates[i].hwnd == foreground) {
                    foregroundIsTaskbar = true;
                    break;
                }
            }
        }
        if (foregroundIsTaskbar) {
            g_taskbarForegroundKeyboardActivated = false;
        }
        if (hwnd && IsIconic(hwnd)) {
            ClearFullscreenOwnersForWindow(hwnd);
        }
// Windows can re-show the taskbar shortly after minimize completes.
// Re-assert the desired state after that transient shell activity.
        if (g_workerMessageWindow) {
            SetTimer(
                g_workerMessageWindow,
                kPostMinimizeReassertTimerId,
                1200,
                nullptr
            );
        }
        PostRefresh();
        UpdateCursorHoverSnapshot();
        return;
    }
    if (event == EVENT_SYSTEM_MOVESIZEEND) {
        MonitorList monitors = GetCurrentMonitors();
        ShellProcessKindCache processCache = {};
        if (hwnd) {
            for (size_t i = 0; i < kMaxMonitorNumbers; ++i) {
                if (g_fullscreenOwners[i].hwnd != hwnd) continue;
                if (!IsFullscreenOwnerOnSameMonitor(hwnd, g_fullscreenOwners[i].monitor)) {
                    ClearFullscreenOwnerAtIndex(i);
                } else {
                    ValidateFullscreenOwnerForMonitor(
                        g_fullscreenOwners[i].monitor,
                        monitors,
                        processCache
                    );
                }
            }
        }
        ClearInvalidFullscreenWindowCache(monitors);
        PostRefresh();
        return;
    }
}
LRESULT CALLBACK WorkerMessageWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_TIMER && wParam == kHoverExpireTimerId) {
        KillTimer(hwnd, kHoverExpireTimerId);
        PostRefresh();
        return 0;
    }
    if (message == WM_TIMER && wParam == kPostMinimizeReassertTimerId) {
        KillTimer(hwnd, kPostMinimizeReassertTimerId);
        g_lastMinimizeEventTick = 0;
        g_taskbarForegroundKeyboardActivated = false;
        UpdateTaskbarState();
        return 0;
    }
    if (message == WM_TIMER && wParam == kFullscreenValidationTimerId) {
        KillTimer(hwnd, kFullscreenValidationTimerId);
        MonitorList monitors = GetCurrentMonitors();
        ShellProcessKindCache processCache = {};
        RefreshFullscreenWindowCache(monitors, processCache);
        UpdateTaskbarState();
        HWND foreground = GetForegroundWindow();
        HMONITOR monitor = foreground
            ? MonitorFromWindow(foreground, MONITOR_DEFAULTTONEAREST)
            : nullptr;
        const int index = FindFullscreenOwnerIndex(monitor);
        const bool ownerStillFullscreen =
            index >= 0 &&
            g_fullscreenOwners[index].hwnd == foreground &&
            IsFullscreenOwnerVisible(monitor);
        static constexpr UINT kValidationDelaysMs[] = {16, 64};
        if (ownerStillFullscreen &&
            g_fullscreenValidationAttempt < ARRAYSIZE(kValidationDelaysMs)) {
            if (!SetTimer(
                    hwnd,
                    kFullscreenValidationTimerId,
                    kValidationDelaysMs[g_fullscreenValidationAttempt++],
                    nullptr
                )) {
                Wh_Log(L"Fullscreen validation timer could not be armed");
                g_fullscreenValidationAttempt = 0;
            }
        } else {
            g_fullscreenValidationAttempt = 0;
        }
        return 0;
    }
    if (message == g_taskbarCreatedMessage ||
        message == WM_DISPLAYCHANGE ||
        message == WM_SETTINGCHANGE ||
        message == WM_THEMECHANGED) {
        if (message == g_taskbarCreatedMessage) {
            InstallShellSurfaceHook();
            InstallTaskbarFocusHook();
            RefreshNativeAutoHideState();
        } else if (message == WM_SETTINGCHANGE) {
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
        return false;
    }
    const wchar_t* kClassName =
        L"WindhawkHideTaskbarOnlyOnDesktopMessageWindow";
    HINSTANCE instance =
        GetWorkerWindowModuleInstance();
    if (!instance) {
        return false;
    }
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.hInstance = instance;
    wc.lpfnWndProc = WorkerMessageWindowProc;
    wc.lpszClassName = kClassName;
    g_workerWindowClassAtom =
        RegisterClassExW(&wc);
    if (!g_workerWindowClassAtom) {
        return false;
    }
    g_workerMessageWindow =
        CreateWindowExW(WS_EX_TOOLWINDOW, kClassName, L"", WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, instance, nullptr);
    if (!g_workerMessageWindow) {
        UnregisterClassW(kClassName, instance);
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
        UnregisterClassW(kClassName, GetWorkerWindowModuleInstance());
        g_workerWindowClassAtom = 0;
    }
    g_taskbarCreatedMessage = 0;
}
DWORD WINAPI WorkerThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    MSG msg = {};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    if (!CreateWorkerMessageWindow()) {
        Wh_Log(L"Failed to create worker message window");
        InterlockedExchange(&g_workerInitializationResult, -1);
        DestroyWorkerMessageWindow();
        if (g_workerReadyEvent) {
            SetEvent(g_workerReadyEvent);
        }
        return 0;
    }
    InterlockedExchange(&g_workerInitializationResult, 1);
    if (g_workerReadyEvent) {
        SetEvent(g_workerReadyEvent);
    }
    g_foregroundHook =
        SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!g_foregroundHook) {
        Wh_Log(L"Failed to install foreground WinEvent hook");
    }
    g_minimizeHook =
        SetWinEventHook(EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!g_minimizeHook) {
        Wh_Log(L"Failed to install minimize WinEvent hook");
    }
    g_moveHook =
        SetWinEventHook(EVENT_SYSTEM_MOVESIZEEND, EVENT_SYSTEM_MOVESIZEEND, nullptr, WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!g_moveHook) {
        Wh_Log(L"Failed to install move/size WinEvent hook");
    }
    InstallShellSurfaceHook();
    InstallTaskbarFocusHook();
    UpdateTaskbarState();
    constexpr UINT kSafetyPollIntervalMs = 2000;
    UINT_PTR timerId =
        SetTimer(nullptr, 0, kSafetyPollIntervalMs, nullptr);
    if (!timerId) {
        Wh_Log(L"Safety timer could not be armed");
    }
    for (;;) {
        BOOL result =
            GetMessageW(&msg, nullptr, 0, 0);
        if (result <= 0) {
            break;
        }
        if (msg.message == WM_TIMER) {
            if (msg.hwnd && msg.hwnd == g_workerMessageWindow) {
                DispatchMessageW(&msg);
                continue;
            }
            RefreshNativeAutoHideState();
            UpdateTaskbarState();
            continue;
        }
        if (msg.message == WM_APP_REFRESH) {
            InterlockedExchange(&g_refreshPosted, 0);
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
        KillTimer(nullptr, timerId);
    }
    if (g_workerMessageWindow) {
        KillTimer(g_workerMessageWindow, kFullscreenValidationTimerId);
    }
    g_fullscreenValidationAttempt = 0;
    CancelHoverExpireTimer();
    SafeUnhookWinEvent(g_foregroundHook);
    SafeUnhookWinEvent(g_minimizeHook);
    SafeUnhookWinEvent(g_moveHook);
    for (size_t i = 0; i < kMaxMonitorNumbers; ++i) {
        SafeUnhookWinEvent(g_fullscreenLocationHooks[i]);
    }
    SafeUnhookWinEvent(g_shellSurfaceHook);
    SafeUnhookWinEvent(g_taskbarFocusHook);
    DestroyWorkerMessageWindow();
    return 0;
}
void LoadSettings() {
    int hoverMargin =
        Wh_GetIntSetting(L"extraHoverMarginPx");
    if (hoverMargin < 0) {
        hoverMargin = 0;
    } else if (hoverMargin > 200) {
        hoverMargin = 200;
    }
    g_settings.extraHoverMarginPx =
        hoverMargin;
    int delay =
        Wh_GetIntSetting(L"autoHideDelayMs");
    if (delay < 0) {
        delay = 0;
    } else if (delay > 10000) {
        delay = 10000;
    }
    g_settings.autoHideDelayMs =
        static_cast<DWORD>(delay);
    g_settings.hideAllMonitors = false;
    g_settings.hoverAllMonitors = false;
    g_settings.monitorInterfaceMappingCount = 0;
    for (
        size_t i = 1;
        i <= kMaxMonitorNumbers;
        ++i
    ) {
        g_settings.hideMonitor[i] = false;
        g_settings.hoverMonitor[i] = false;
    }
    for (
        size_t i = 0;
        i < kMaxMonitorNumbers;
        ++i
    ) {
        auto display =
            WindhawkUtils::StringSetting::make(
                L"monitorInterfaceMappings[%d].display",
                static_cast<int>(i)
            );
        auto interfaceName =
            WindhawkUtils::StringSetting::make(
                L"monitorInterfaceMappings[%d].interfaceName",
                static_cast<int>(i)
            );

        if (!*display && !*interfaceName) {
            continue;
        }

        const int displayNumber = ParseMonitorNumber(display.get());
        if (displayNumber == 0 || !*interfaceName) {
            continue;
        }

        if (g_settings.monitorInterfaceMappingCount >=
            kMaxMonitorNumbers) {
            break;
        }

        MonitorInterfaceMapping& mapping =
            g_settings.monitorInterfaceMappings[
                g_settings.monitorInterfaceMappingCount++
            ];
        mapping.monitorNumber = static_cast<int>(displayNumber);
        wcsncpy_s(
            mapping.interfaceName,
            ARRAYSIZE(mapping.interfaceName),
            interfaceName.get(),
            _TRUNCATE
        );
    }
    for (
        size_t i = 0;
        i < kMaxMonitorNumbers;
        ++i
    ) {
        auto value =
            WindhawkUtils::StringSetting::make(L"hideOnMonitors[%d]", static_cast<int>(i));
        if (!*value) {
            break;
        }
        if (wcscmp(value, L"all") == 0) {
            g_settings.hideAllMonitors = true;
        } else {
            const int number = ParseMonitorNumber(value);
            if (number > 0) {
                g_settings.hideMonitor[number] = true;
            }
        }
    }
    for (
        size_t i = 0;
        i < kMaxMonitorNumbers;
        ++i
    ) {
        auto value =
            WindhawkUtils::StringSetting::make(L"hoverRevealOnMonitors[%d]", static_cast<int>(i));
        if (!*value) {
            break;
        }
        if (wcscmp(value, L"all") == 0) {
            g_settings.hoverAllMonitors = true;
        } else {
            const int number = ParseMonitorNumber(value);
            if (number > 0) {
                g_settings.hoverMonitor[number] = true;
            }
        }
    }
}
BOOL WhTool_ModInit() {
    LoadSettings();
    RefreshNativeAutoHideState();
// Recover ownership left by an unexpectedly terminated previous tool
// process before the new worker starts making visibility decisions.
    RestoreAllTaskbars();
    g_workerReadyEvent =
        CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_workerReadyEvent) {
        Wh_Log(L"CreateEvent for worker readiness failed");
        return FALSE;
    }
    InterlockedExchange(&g_workerInitializationResult, 0);
    g_workerThread =
        CreateThread(nullptr, 0, WorkerThread, nullptr, 0, &g_workerThreadId);
    if (!g_workerThread) {
        SafeCloseHandle(g_workerReadyEvent);
        return FALSE;
    }
    DWORD readyResult =
        WaitForSingleObject(g_workerReadyEvent, 5000);
    const LONG workerInitializationResult =
        InterlockedCompareExchange(&g_workerInitializationResult, 0, 0);
    if (readyResult != WAIT_OBJECT_0) {
        EnumWindows(RestoreMarkedTaskbarProc, 0);
        if (g_workerThread) {
            if (!PostThreadMessageW(
                    g_workerThreadId,
                    WM_QUIT,
                    0,
                    0
                )) {
                Wh_Log(L"Failed to stop worker after readiness timeout");
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
    if (workerInitializationResult != 1) {
        EnumWindows(RestoreMarkedTaskbarProc, 0);
        if (g_workerThread &&
            !WaitForThreadWithTimeout(g_workerThread, 5000, L"worker")) {
            ExitProcess(1);
        }
        SafeCloseHandle(g_workerThread);
        SafeCloseHandle(g_workerReadyEvent);
        return FALSE;
    }
    g_cursorStopEvent =
        CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_cursorStopEvent) {
        Wh_Log(L"CreateEvent for cursor sampler failed");
        WhTool_ModUninit();
        return FALSE;
    }
    g_cursorThread =
        CreateThread(nullptr, 0, CursorSamplingThread, nullptr, 0, nullptr);
    if (!g_cursorThread) {
        Wh_Log(L"Failed to create cursor sampler thread");
        SafeCloseHandle(g_cursorStopEvent);
        WhTool_ModUninit();
        return FALSE;
    }
    return TRUE;
}
void WhTool_ModSettingsChanged() {
    if (g_workerThread &&
        !PostThreadMessageW(g_workerThreadId, WM_APP_SETTINGS, 0, 0)) {
        Wh_Log(L"Failed to post settings message to worker thread");
    }
}
BOOL CALLBACK RestoreMarkedTaskbarProc(HWND hwnd, LPARAM) {
    if (!hwnd || GetPropW(hwnd, kTaskbarOwnershipProp) == nullptr) {
        return TRUE;
    }
    if (!MakeTaskbarTransparent(hwnd, false)) {
        Wh_Log(L"Orphaned taskbar restore failed for %p, forcing visible", hwnd);
        if (!ForceRestoreTaskbar(hwnd)) {
            Wh_Log(L"Orphaned taskbar force-restore failed for %p", hwnd);
        }
    }
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
        } else {
            Wh_Log(L"Taskbar restore during recovery failed for %p", state.hwnd);
            if (ForceRestoreTaskbar(state.hwnd)) {
                state.hiddenByMod = false;
            } else {
                Wh_Log(L"Taskbar force-restore during recovery failed for %p", state.hwnd);
            }
        }
    }
    EnumWindows(RestoreMarkedTaskbarProc, 0);
}
bool WaitForThreadWithTimeout(HANDLE thread, DWORD timeoutMs, const wchar_t* threadName) {
    DWORD result =
        WaitForSingleObject(thread, timeoutMs);
    if (result == WAIT_OBJECT_0) {
        return true;
    }
    if (result == WAIT_TIMEOUT) {
        Wh_Log(L"%s thread did not exit within %lu ms", threadName, timeoutMs);
    } else {
        Wh_Log(L"WaitForSingleObject failed for %s thread: %lu", threadName, result);
    }
    return false;
}
void WhTool_ModUninit() {
// Stop new cursor-triggered refreshes first, then shut down the worker that
// owns all visibility decisions. This prevents the worker from changing the
// taskbar while final restoration is in progress.
    if (g_cursorStopEvent) {
        SetEvent(g_cursorStopEvent);
    }
    if (g_workerThread) {
        if (!PostThreadMessageW(
                g_workerThreadId,
                WM_QUIT,
                0,
                0
            )) {
            Wh_Log(L"Failed to post worker shutdown message");
        }
        if (!WaitForThreadWithTimeout(
                g_workerThread,
                5000,
                L"worker"
            )) {
            EnumWindows(RestoreMarkedTaskbarProc, 0);
            ExitProcess(1);
        }
        SafeCloseHandle(g_workerThread);
    }
    if (g_cursorThread) {
        if (!WaitForThreadWithTimeout(
                g_cursorThread,
                3000,
                L"cursor sampler"
            )) {
            EnumWindows(RestoreMarkedTaskbarProc, 0);
            ExitProcess(1);
        }
        SafeCloseHandle(g_cursorThread);
    }
    SafeCloseHandle(g_cursorStopEvent);
    SafeCloseHandle(g_workerReadyEvent);
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

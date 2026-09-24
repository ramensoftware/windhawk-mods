// ==WindhawkMod==
// @id              hide-taskbar-only-on-desktop
// @name            Hide Taskbar Only on Desktop
// @description     Hides selected taskbars on desktop-only displays with per-display hover, keyboard, shell, and fullscreen handling
// @version         7.8.0
// @author          Sahil Dashoni
// @github          https://github.com/Sahil-Dashoni
// @include         windhawk.exe
// @compilerOptions -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Taskbar Only on Desktop

This mod hides selected bottom-docked taskbars when their display has no relevant visible application. A detected fullscreen window can also keep that display in the desktop-only state. Each display is evaluated independently.

## Demo

### Multiple Displays

![Multiple Display](https://raw.githubusercontent.com/Sahil-Dashoni/Hide-Taskbar-Only-on-Desktop-Windhawk-Mod/refs/heads/main/Assets/multiple-display.gif)

### Single Display

![Single Display](https://raw.githubusercontent.com/Sahil-Dashoni/Hide-Taskbar-Only-on-Desktop-Windhawk-Mod/refs/heads/main/Assets/single-display.gif)

## Default Behavior

By default:

- Desktop-only hiding is enabled for all connected displays.
- Bottom-edge hover reveal is enabled for all connected displays.
- The extra hover margin is **8 px** at 100% scaling.
- The hover dismissal delay is **700 ms**.
- No stable monitor mappings are configured.
- Windows' native taskbar auto-hide is not enabled or changed by this mod.

## How It Works

For each selected display, the mod scans visible, non-minimized, non-cloaked windows and excludes Windows shell/taskbar infrastructure from normal application detection.

A display is treated as desktop-only when no relevant application is present. A detected fullscreen window is also tracked separately for that display. Windows that intersect more than one display can affect each display they intersect; maximized windows are assigned to the monitor Windows reports for that window.

The following interactions can temporarily keep a taskbar visible or change the normal desktop-only state:

- Start and Windows Search
- Recognized Windows shell surfaces and taskbar popups
- Keyboard-driven taskbar focus
- Bottom-edge hover reveal
- Minimize transitions
- Detected fullscreen transitions

Taskbars are hidden by changing their layered-window alpha and adding `WS_EX_TRANSPARENT`. The mod does not enable Windows' native auto-hide mode, so it does not intentionally change the normal desktop work area.

## Per-Display Configuration

The mod supports up to **16 fixed logical display slots**.

`Taskbars to hide on desktop` selects which display slots participate in desktop-only hiding.

`Reveal taskbar on bottom-edge hover` selects which display slots can be revealed by moving the cursor into the configured bottom-edge zone.

`Stable monitor interface names` can pin a display slot to a physical monitor using the interface name returned by `EnumDisplayDevicesW` with `EDD_GET_DEVICE_INTERFACE_NAME`.

Pinned monitors keep their configured slots. Unpinned monitors fill the remaining slots in the current logical monitor order. A disconnected mapped monitor keeps its mapping but does not occupy that slot until it is detected again. When duplicate mappings target the same slot or monitor, the last matching mapping wins.

## Finding Monitor Interface Names

Run this in PowerShell and copy the value shown after **Interface :** into the corresponding mapping:

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

## Hover Reveal

For bottom-docked taskbars, the reveal zone follows the taskbar's current height and display DPI, plus the configured extra margin.

When the cursor enters that zone on a selected display, the mod can reveal its taskbar. After the cursor leaves, the taskbar is normally hidden again after the configured delay, unless supported shell popup activity is still keeping the hover session alive.

Cursor tracking runs in a dedicated sampling thread. It samples every 50 ms while hover tracking is active and normally backs off to 250 ms when it is not needed.

## Fullscreen

Fullscreen detection is geometry-based. A visible application window that matches monitor bounds can be treated as fullscreen. A borderless foreground window matching the current monitor work area can also be treated as fullscreen.

Fullscreen ownership is tracked independently for each display. Foreground fullscreen windows and eligible foreground child surfaces, such as browser/video rendering surfaces, can establish the fullscreen owner.

A short exit guard prevents stale child surfaces from immediately recreating fullscreen ownership after a fullscreen root exits.

Fullscreen detection is heuristic and can classify some borderless monitor-sized applications as fullscreen.

While fullscreen is detected on a display, bottom-edge hover does not reveal that display's taskbar. Start, Search, and keyboard-driven taskbar focus remain available through their supported paths.

## Shell and Taskbar Interaction

Recognized shell surfaces are excluded from normal application detection where appropriate. This includes Start, Search, recognized shell classes, taskbar popups, and several Windows task-switching surfaces.

Taskbar focus is treated as keyboard activation only when focus is on a tracked taskbar, that taskbar is actually foreground, and the transition is not identified as mouse activation, minimize activity, or a recent shell-close transition.

The mod does not use `ShowWindow` to hide the taskbar.

## Taskbar State and Recovery

When hiding a taskbar, the mod records its relevant extended style and layered-window attributes when available, then applies the hidden state. Taskbars hidden by the mod are marked with a window property so the mod can recognize its own ownership later.

When restoring a taskbar, the mod removes only the style state it owns and restores the recorded layered attributes when they are available. If the original layered attributes could not be read, restoration falls back to a fully visible alpha state.

Taskbars are rediscovered when Explorer recreates them or the display topology changes. The dedicated tool process also scans for taskbars carrying the ownership marker when it starts, allowing recovery after an unexpected previous-process termination.

## Native Auto-Hide

This mod does not enable or replace Windows' native taskbar auto-hide.

When native auto-hide is already enabled, the mod does not apply its own desktop-only hiding to that taskbar.

## Limitations

- Desktop-only hiding and hover reveal apply only to bottom-docked taskbars.
- Display configuration is limited to 16 fixed logical display slots.
- A hidden taskbar remains part of the normal Windows work area.
- A taskbar hidden by the mod is click-through, so its buttons and tray notifications are not visible while hidden.
- Other software that changes the same taskbar transparency or extended-style state can conflict with this mod.
- Shell window classes and shell process names can change between Windows releases.
- Fullscreen detection is heuristic and geometry-based rather than application-aware.
- The mod uses a 5-second safety refresh in addition to its handled Windows events.
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
  $name: Stable monitor interface names
  $description: >-
    Optional mappings that pin a display slot to a specific physical monitor.
- hoverRevealOnMonitors: ["all"]
  $name: Reveal taskbar on bottom-edge hover
  $description: >-
    Select the displays where bottom-edge hovering should reveal the taskbar.
    Select the display slots (Display 1, Display 2, and so on).
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
    Select one or more display slots (Display 1, Display 2, and so on).
    Stable monitor interface names can optionally be assigned in the
    Stable monitor interface names setting so a display slot can stay attached
    to the same physical monitor when the logical display order changes.
    Choose All displays to hide every connected display. Only bottom-docked taskbars
    participate in desktop-based hiding. Use Add to select multiple display slots.
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
constexpr UINT kTaskbarFrameChangeFlags =
    SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
    SWP_FRAMECHANGED | SWP_ASYNCWINDOWPOS;
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
HWND g_recentFullscreenExitRoot = nullptr;
ULONGLONG g_recentFullscreenExitUntilTick = 0;
constexpr ULONGLONG kFullscreenExitChildScanSuppressMs = 1500;
HWINEVENTHOOK g_taskbarFocusHook = nullptr;
HWINEVENTHOOK g_objectHook = nullptr;
HWINEVENTHOOK g_cloakHook = nullptr;
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
constexpr wchar_t kTaskbarOwnershipProp[] = L"windhawk-hide-taskbar-only-on-desktop-ownership";
constexpr wchar_t kTaskbarOriginalExStyleProp[] = L"windhawk-hide-taskbar-only-on-desktop-original-exstyle";
constexpr wchar_t kTaskbarOriginalLayeredColorKeyProp[] = L"windhawk-hide-taskbar-only-on-desktop-original-color-key";
constexpr wchar_t kTaskbarOriginalLayeredAlphaProp[] = L"windhawk-hide-taskbar-only-on-desktop-original-alpha";
constexpr wchar_t kTaskbarOriginalLayeredFlagsProp[] = L"windhawk-hide-taskbar-only-on-desktop-original-layered-flags";
constexpr wchar_t kTaskbarOriginalLayeredAttributesValidProp[] =
    L"windhawk-hide-taskbar-only-on-desktop-original-layered-valid";
constexpr LONG_PTR kModTaskbarExStyleBits = WS_EX_LAYERED | WS_EX_TRANSPARENT;

bool GetWindowExStyle(HWND hwnd, LONG_PTR* exStyle) {
    if (!hwnd || !exStyle) return false;
    SetLastError(ERROR_SUCCESS);
    *exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    return *exStyle != 0 || GetLastError() == ERROR_SUCCESS;
}

bool GetWindowUlongPtrProp(HWND hwnd, const wchar_t* name, ULONG_PTR* value) {
    if (!hwnd || !name || !value) return false;
    HANDLE prop = GetPropW(hwnd, name);
    if (!prop) return false;
    *value = reinterpret_cast<ULONG_PTR>(prop) - 1;
    return true;
}

bool SetWindowUlongPtrProp(HWND hwnd, const wchar_t* name, ULONG_PTR value) {
    return SetPropW(hwnd, name, reinterpret_cast<HANDLE>(value + 1)) != 0;
}

void RemoveTaskbarOwnershipProperties(HWND hwnd) {
    if (!hwnd) return;
    RemovePropW(hwnd, kTaskbarOwnershipProp);
    RemovePropW(hwnd, kTaskbarOriginalExStyleProp);
    RemovePropW(hwnd, kTaskbarOriginalLayeredColorKeyProp);
    RemovePropW(hwnd, kTaskbarOriginalLayeredAlphaProp);
    RemovePropW(hwnd, kTaskbarOriginalLayeredFlagsProp);
    RemovePropW(hwnd, kTaskbarOriginalLayeredAttributesValidProp);
}
bool DropStaleTaskbarOwnership(HWND hwnd, LONG_PTR currentExStyle) {
    if (!hwnd) return false;

    ULONG_PTR originalExStyleValue = 0;
    const bool haveOriginalExStyle = GetWindowUlongPtrProp(
        hwnd, kTaskbarOriginalExStyleProp, &originalExStyleValue);

    const LONG_PTR originalExStyle = static_cast<LONG_PTR>(originalExStyleValue);
    // With a recorded original, strip only what the mod added; without one,
    // there is nothing to compare against, so strip every bit it could have set.
    const LONG_PTR bitsToStrip =
        haveOriginalExStyle
            ? kModTaskbarExStyleBits & ~originalExStyle
            : kModTaskbarExStyleBits;
    const LONG_PTR restoredExStyle = currentExStyle & ~bitsToStrip;

    if (restoredExStyle != currentExStyle) {
        SetLastError(ERROR_SUCCESS);
        const LONG_PTR previousExStyle = SetWindowLongPtrW(
            hwnd, GWL_EXSTYLE, restoredExStyle);
        if (previousExStyle == 0 && GetLastError() != ERROR_SUCCESS) {
            return false;
        }

        SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, kTaskbarFrameChangeFlags);
    }

    RemoveTaskbarOwnershipProperties(hwnd);
    return true;
}

bool ForceRestoreTaskbar(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return false;

    if (!SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA)) {
        Wh_Log(L"Failed to force taskbar alpha visible for %p", hwnd);
    }

    LONG_PTR exStyle = 0;
    if (!GetWindowExStyle(hwnd, &exStyle)) return false;

    return DropStaleTaskbarOwnership(hwnd, exStyle);
}

bool MakeTaskbarTransparent(HWND hwnd, bool hide) {
    if (!hwnd || !IsWindow(hwnd)) return false;
    LONG_PTR exStyle = 0;
    if (!GetWindowExStyle(hwnd, &exStyle)) return false;
    if (hide) {
        const bool ownedByMod = GetPropW(hwnd, kTaskbarOwnershipProp) != nullptr;
        if (ownedByMod && (exStyle & WS_EX_LAYERED) == 0) {
            if (!DropStaleTaskbarOwnership(hwnd, exStyle)) return false;
            if (!GetWindowExStyle(hwnd, &exStyle)) return false;
        } else if (ownedByMod) {
            COLORREF colorKey = 0;
            BYTE alpha = 0;
            DWORD layeredFlags = 0;
            if (!GetLayeredWindowAttributes(
                    hwnd, &colorKey, &alpha, &layeredFlags) ||
                !(layeredFlags & LWA_ALPHA) || alpha != 0) {
                if (!SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA)) {
                    return false;
                }
            }
            if ((exStyle & WS_EX_TRANSPARENT) == 0) {
                SetLastError(ERROR_SUCCESS);
                LONG_PTR previousExStyle = SetWindowLongPtrW(
                    hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT);
                if (previousExStyle == 0 && GetLastError() != ERROR_SUCCESS) {
                    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
                    return false;
                }
                SetWindowPos( hwnd, nullptr, 0, 0, 0, 0, kTaskbarFrameChangeFlags );
            }
            return true;
        }
        COLORREF colorKey = 0;
        BYTE alpha = 255;
        DWORD layeredFlags = 0;
        const bool originalLayered = (exStyle & WS_EX_LAYERED) != 0;
        const bool originalLayeredAttributesValid =
            originalLayered &&
            GetLayeredWindowAttributes(hwnd, &colorKey, &alpha, &layeredFlags) != FALSE;
        if (originalLayeredAttributesValid && alpha == 0) return false;
        if (!SetWindowUlongPtrProp( hwnd, kTaskbarOriginalExStyleProp, static_cast<ULONG_PTR>(exStyle))) return false;
        if (!SetWindowUlongPtrProp(
                hwnd, kTaskbarOriginalLayeredColorKeyProp,
                static_cast<ULONG_PTR>(colorKey)) ||
            !SetWindowUlongPtrProp(
                hwnd, kTaskbarOriginalLayeredAlphaProp,
                static_cast<ULONG_PTR>(alpha)) ||
            !SetWindowUlongPtrProp(
                hwnd, kTaskbarOriginalLayeredFlagsProp,
                static_cast<ULONG_PTR>(layeredFlags)) ||
            !SetWindowUlongPtrProp(
                hwnd, kTaskbarOriginalLayeredAttributesValidProp,
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
        LONG_PTR previousExStyle = SetWindowLongPtrW( hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED );
        if (previousExStyle == 0 && GetLastError() != ERROR_SUCCESS) {
            RemoveTaskbarOwnershipProperties(hwnd);
            return false;
        }
        if (!SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA)) {
            SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);
            SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, kTaskbarFrameChangeFlags);
            RemoveTaskbarOwnershipProperties(hwnd);
            return false;
        }
        SetLastError(ERROR_SUCCESS);
        previousExStyle = SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT);
        if (previousExStyle == 0 && GetLastError() != ERROR_SUCCESS) {
            SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
            SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);
            SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, kTaskbarFrameChangeFlags);
            RemoveTaskbarOwnershipProperties(hwnd);
            return false;
        }
        SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, kTaskbarFrameChangeFlags);
        return true;
    }
    if (GetPropW(hwnd, kTaskbarOwnershipProp) == nullptr) return false;
    if ((exStyle & WS_EX_LAYERED) == 0) return DropStaleTaskbarOwnership(hwnd, exStyle);
    ULONG_PTR originalExStyleValue = 0;
    ULONG_PTR originalColorKeyValue = 0;
    ULONG_PTR originalAlphaValue = 255;
    ULONG_PTR originalFlagsValue = 0;
    ULONG_PTR originalLayeredAttributesValidValue = 0;
    const bool haveOriginalExStyle = GetWindowUlongPtrProp( hwnd, kTaskbarOriginalExStyleProp, &originalExStyleValue );
    const bool haveOriginalColorKey = GetWindowUlongPtrProp(
        hwnd, kTaskbarOriginalLayeredColorKeyProp, &originalColorKeyValue);
    const bool haveOriginalAlpha = GetWindowUlongPtrProp( hwnd, kTaskbarOriginalLayeredAlphaProp, &originalAlphaValue );
    const bool haveOriginalFlags = GetWindowUlongPtrProp( hwnd, kTaskbarOriginalLayeredFlagsProp, &originalFlagsValue );
    const bool haveOriginalLayeredValid = GetWindowUlongPtrProp(
        hwnd, kTaskbarOriginalLayeredAttributesValidProp,
        &originalLayeredAttributesValidValue);
    if (!haveOriginalExStyle) return false;
    const LONG_PTR originalExStyle = static_cast<LONG_PTR>(originalExStyleValue);
    const bool originalLayered = (originalExStyle & WS_EX_LAYERED) != 0;
    const bool originalAttributesValid =
        haveOriginalLayeredValid && originalLayeredAttributesValidValue != 0 &&
        haveOriginalColorKey && haveOriginalAlpha && haveOriginalFlags;
    LONG_PTR currentExStyle = 0;
    if (!GetWindowExStyle(hwnd, &currentExStyle)) return false;
    SetLastError(ERROR_SUCCESS);
    const LONG_PTR inputEnabledExStyle = currentExStyle & ~WS_EX_TRANSPARENT;
    LONG_PTR previousExStyle = SetWindowLongPtrW( hwnd, GWL_EXSTYLE, inputEnabledExStyle );
    if (previousExStyle == 0 && GetLastError() != ERROR_SUCCESS) return false;
    bool restoredAttributes = true;
    if (originalLayered) {
        if (originalAttributesValid) {
            restoredAttributes =
                SetLayeredWindowAttributes(
                    hwnd, static_cast<COLORREF>(originalColorKeyValue),
                    static_cast<BYTE>(originalAlphaValue),
                    static_cast<DWORD>(originalFlagsValue)) != FALSE;
        } else {
            restoredAttributes = SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA) != FALSE;
        }
    } else {
        restoredAttributes = SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA) != FALSE;
    }
    if (!restoredAttributes) return false;
    SetLastError(ERROR_SUCCESS);
    const LONG_PTR restoredExStyle =
        (inputEnabledExStyle & ~(WS_EX_LAYERED | WS_EX_TRANSPARENT)) |
        (originalExStyle & (WS_EX_LAYERED | WS_EX_TRANSPARENT));
    if (restoredExStyle != inputEnabledExStyle) {
        previousExStyle = SetWindowLongPtrW(hwnd, GWL_EXSTYLE, restoredExStyle);
        if (previousExStyle == 0 && GetLastError() != ERROR_SUCCESS) return false;
    }
    SetWindowPos( hwnd, nullptr, 0, 0, 0, 0, kTaskbarFrameChangeFlags );
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
// Search runs outside Explorer (normally SearchHost.exe). Track its active
// surface separately so Search can reveal the taskbar without being folded
// into the general shell-surface scan.
bool g_searchSurfaceVisible = false;
HWND g_searchSurfaceWindow = nullptr;
HMONITOR g_searchSurfaceMonitor = nullptr;
DWORD g_searchHostProcessId = 0;
// Track Start independently from generic XAML visibility. Start visibility is
// driven by the StartMenuExperienceHost foreground transition, not transient
// object show/hide notifications from its internal windows.
bool g_startSurfaceVisible = false;
// After Start closes, the cursor can remain on the Start button/taskbar. Do not
// immediately recreate the hover reveal from that same cursor position; clear
// this suppression as soon as the cursor leaves the taskbar hot zone.
bool g_startCloseHoverSuppressed = false;
HWND g_startSurfaceWindow = nullptr;
HMONITOR g_startSurfaceMonitor = nullptr;
ULONGLONG g_lastMinimizeEventTick = 0;
// True from minimize start until minimize end. Taskbar focus generated by
// Windows during this transition must not be treated as keyboard activation.
bool g_minimizeInProgress = false;
ULONGLONG g_minimizeStartTick = 0;
ULONGLONG g_minimizeFocusIgnoreUntilTick = 0;
ULONGLONG g_shellCloseFocusIgnoreUntilTick = 0;

bool IsRecentMinimizeTaskbarFocusSuppressed() {
    return g_minimizeFocusIgnoreUntilTick != 0 &&
           GetTickCount64() < g_minimizeFocusIgnoreUntilTick;
}

bool IsRecentShellCloseTaskbarFocusSuppressed() {
    return g_shellCloseFocusIgnoreUntilTick != 0 &&
           GetTickCount64() < g_shellCloseFocusIgnoreUntilTick;
}

bool IsMinimizeInProgress() {
    if (!g_minimizeInProgress) return false;

    if (GetTickCount64() - g_minimizeStartTick > 2000) {
        g_minimizeInProgress = false;
        g_minimizeStartTick = 0;
    }

    return g_minimizeInProgress;
}
// A fullscreen window is cached only after that window has actually entered
// the foreground. The cache is keyed by the HMONITOR itself rather than by
// monitor-enumeration index. Once claimed, ownership is sticky until an explicit
// fullscreen lifecycle event ends it or the owner changes monitors.
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
void InstallShellObjectHooks();
void CALLBACK WinEventProc( HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD);

bool IsShellChromeClass(const WCHAR* className) {
    if (!className) return false;
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
        if (wcscmp(className, shellClass) == 0) return true;
    }
    return false;
}

bool IsDesktopInfrastructureWindow(HWND hwnd, const WCHAR* className) {
    if (!hwnd || !className) return false;
    if ( wcscmp(className, L"Progman") == 0 || wcscmp(className, L"WorkerW") == 0 ) return true;
    // Windows can make the zero-sized Explorer XAML host foreground when the
    // desktop context menu is opened. It is shell infrastructure, not an
    // application window. Treat only this transient zero-sized host as desktop
    // infrastructure so it cannot turn desktopOnly off and reveal the taskbar.
    if (wcscmp(className, L"XamlExplorerHostIslandWindow_WASDK") == 0) {
        RECT rect = {};
        if (GetWindowRect(hwnd, &rect) && rect.left == rect.right && rect.top == rect.bottom) return true;
    }
    HWND shellWindow = GetShellWindow();
    if (shellWindow && shellWindow == hwnd) return true;
    return false;
}

BOOL CALLBACK CollectMonitorProc(HMONITOR monitor, HDC, LPRECT, LPARAM lParam) {
    MonitorList* list = reinterpret_cast<MonitorList*>(lParam);
    if (!list || list->count >= kMaxMonitorNumbers) return FALSE;
    MONITORINFOEXW info = {};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, &info)) return TRUE;
    MonitorEntry& entry = list->entries[list->count++];
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
    if (!monitor || !output || outputCount == 0) return false;
    output[0] = L'\0';
    MONITORINFOEXW info = {};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, &info)) return false;
    DISPLAY_DEVICEW device = {};
    device.cb = sizeof(device);
    if (!EnumDisplayDevicesW( info.szDevice, 0, &device, EDD_GET_DEVICE_INTERFACE_NAME )) return false;
    if (!device.DeviceID[0]) return false;
    wcsncpy_s(output, outputCount, device.DeviceID, _TRUNCATE);
    return output[0] != L'\0';
}
void BuildMonitorSelectionSlots(
    const MonitorList& monitors, int* selectionSlotForMonitor) {
    if (!selectionSlotForMonitor) return;

    for (size_t i = 0; i < monitors.count; ++i) {
        selectionSlotForMonitor[i] = 0;
    }
    if (monitors.count == 0) return;

    if (g_settings.monitorInterfaceMappingCount == 0) {
        for (size_t i = 0; i < monitors.count; ++i) {
            selectionSlotForMonitor[i] = static_cast<int>(i) + 1;
        }
        return;
    }

    WCHAR interfaceNames[kMaxMonitorNumbers][kMonitorInterfaceNameLength] = {};
    bool haveInterfaceName[kMaxMonitorNumbers] = {};
    for (size_t i = 0; i < monitors.count; ++i) {
        haveInterfaceName[i] = GetMonitorInterfaceName(
            monitors.entries[i].monitor, interfaceNames[i],
            ARRAYSIZE(interfaceNames[i]));
    }

    // A mapping pins one physical monitor to one fixed display slot.
    // Later mappings replace earlier mappings that use the same slot or
    // target the same physical monitor.
    int targetMonitor[kMaxMonitorNumbers + 1] = {};
    int pinnedSlotForMonitor[kMaxMonitorNumbers] = {};
    for (size_t i = 0; i < g_settings.monitorInterfaceMappingCount; ++i) {
        const MonitorInterfaceMapping& mapping =
            g_settings.monitorInterfaceMappings[i];
        if (mapping.monitorNumber < 1 ||
            mapping.monitorNumber > static_cast<int>(kMaxMonitorNumbers) ||
            !mapping.interfaceName[0]) {
            continue;
        }

        int matchedMonitor = -1;
        for (size_t monitorIndex = 0; monitorIndex < monitors.count;
             ++monitorIndex) {
            if (haveInterfaceName[monitorIndex] &&
                _wcsicmp(interfaceNames[monitorIndex], mapping.interfaceName) ==
                    0) {
                matchedMonitor = static_cast<int>(monitorIndex);
                break;
            }
        }
        if (matchedMonitor < 0) continue;

        const int slot = mapping.monitorNumber;
        const int previousMonitor = targetMonitor[slot];
        if (previousMonitor > 0 &&
            previousMonitor <= static_cast<int>(monitors.count)) {
            pinnedSlotForMonitor[previousMonitor - 1] = 0;
        }

        const int previousSlot = pinnedSlotForMonitor[matchedMonitor];
        if (previousSlot > 0 &&
            previousSlot <= static_cast<int>(kMaxMonitorNumbers)) {
            targetMonitor[previousSlot] = 0;
        }

        targetMonitor[slot] = matchedMonitor + 1;
        pinnedSlotForMonitor[matchedMonitor] = slot;
    }

    // Keep pinned monitors exactly where the user selected them.
    bool slotOccupied[kMaxMonitorNumbers + 1] = {};
    for (int slot = 1; slot <= static_cast<int>(kMaxMonitorNumbers); ++slot) {
        const int monitorNumber = targetMonitor[slot];
        if (monitorNumber < 1 ||
            monitorNumber > static_cast<int>(monitors.count)) {
            continue;
        }

        const int monitorIndex = monitorNumber - 1;
        selectionSlotForMonitor[monitorIndex] = slot;
        slotOccupied[slot] = true;
    }

    // Unpinned monitors simply fill the remaining fixed slots in their
    // current logical order. There are never overflow or displacement slots.
    int nextSlot = 1;
    for (size_t monitorIndex = 0; monitorIndex < monitors.count;
         ++monitorIndex) {
        if (selectionSlotForMonitor[monitorIndex] != 0) continue;

        while (nextSlot <= static_cast<int>(kMaxMonitorNumbers) &&
               slotOccupied[nextSlot]) {
            ++nextSlot;
        }
        if (nextSlot > static_cast<int>(kMaxMonitorNumbers)) break;

        selectionSlotForMonitor[monitorIndex] = nextSlot;
        slotOccupied[nextSlot] = true;
        ++nextSlot;
    }
}

bool IsBottomDockedTaskbar(HWND hTaskbar, HMONITOR monitor);
int FindMonitorIndex(const MonitorList& monitors, HMONITOR monitor);

int ParseMonitorNumber(const WCHAR* value) {
    if (!value || wcsncmp(value, L"monitor", 7) != 0) return 0;
    wchar_t* endNumber = nullptr;
    long number = wcstol(value + 7, &endNumber, 10);
    return endNumber && *endNumber == L'\0' && number >= 1 && number <= static_cast<long>(kMaxMonitorNumbers)
               ? static_cast<int>(number)
               : 0;
}

bool IsMonitorSelected( int monitorNumber, const bool* selected ) {
    if (!selected || monitorNumber < 1 || monitorNumber > static_cast<int>(kMaxMonitorNumbers)) return false;
    return selected[monitorNumber];
}

bool ShouldRevealOnHover(const TaskbarMonitorState& state) {
    return
        g_settings.hoverAllMonitors || IsMonitorSelected(state.monitorNumber, g_settings.hoverMonitor);
}

bool GetWindowProcessImageName(DWORD pid, wchar_t* output, size_t outputCount) {
    if (!pid || !output || outputCount == 0) return false;
    output[0] = L'\0';
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return false;
    DWORD size = static_cast<DWORD>(outputCount);
    BOOL result = QueryFullProcessImageNameW(process, 0, output, &size);
    CloseHandle(process);
    return result && output[0] != L'\0';
}
enum class ShellProcessKind {
    None, Explorer, KnownShell, StartHost, SearchHost, };

ShellProcessKind GetShellProcessKind(DWORD pid) {
    wchar_t imagePath[MAX_PATH] = {};
    if (!GetWindowProcessImageName( pid, imagePath, ARRAYSIZE(imagePath) )) return ShellProcessKind::None;
    const wchar_t* baseName = wcsrchr(imagePath, L'\\');
    baseName = baseName ? baseName + 1 : imagePath;
    if (_wcsicmp(baseName, L"explorer.exe") == 0) return ShellProcessKind::Explorer;
    if (_wcsicmp(baseName, L"SearchHost.exe") == 0 ||
        _wcsicmp(baseName, L"SearchApp.exe") == 0) {
        return ShellProcessKind::SearchHost;
    }
    if (_wcsicmp(baseName, L"StartMenuExperienceHost.exe") == 0) {
        return ShellProcessKind::StartHost;
    }
    static const wchar_t* kKnownShellProcesses[] = {
        L"ShellExperienceHost.exe",
        L"ShellHost.exe",
    };
    for (const wchar_t* name : kKnownShellProcesses) {
        if (_wcsicmp(baseName, name) == 0) return ShellProcessKind::KnownShell;
    }
    return ShellProcessKind::None;
}
constexpr size_t kMaxShellProcessCacheEntries = 64;
struct ShellProcessKindCache {
    DWORD pids[kMaxShellProcessCacheEntries] = {};
    ShellProcessKind kinds[kMaxShellProcessCacheEntries] = {};
    size_t count = 0;
};

ShellProcessKind GetShellProcessKindCached( ShellProcessKindCache& cache, DWORD pid ) {
    if (!pid) return ShellProcessKind::None;
    for (size_t i = 0; i < cache.count; ++i) {
        if (cache.pids[i] == pid) return cache.kinds[i];
    }
    const ShellProcessKind kind = GetShellProcessKind(pid);
    if (cache.count < kMaxShellProcessCacheEntries) {
        cache.pids[cache.count] = pid;
        cache.kinds[cache.count] = kind;
        ++cache.count;
    }
    return kind;
}

bool ShouldHideTaskbar(const TaskbarMonitorState& state);
void SetTaskbarState(TaskbarMonitorState& state, bool show);

void HideTaskbarsForStartClose(HMONITOR monitor) {
    if (!monitor) return;

    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        TaskbarMonitorState& state = g_taskbarStates[i];
        if (state.monitor != monitor) continue;
        if (!state.desktopOnly || !ShouldHideTaskbar(state)) continue;

        // Start has already yielded focus. Hide immediately using the state
        // established by the preceding minimize/refresh cycle instead of
        // waiting for a full EnumWindows scan. The normal posted refresh then
        // validates the result and restores visibility if the display is not
        // actually desktop-only.
        SetTaskbarState(state, false);
        break;
    }
}

bool IsWindowCloaked(HWND hwnd) {
    BOOL cloaked = FALSE;
    return
        SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked;
}

bool IsTaskbarPopupClass(const WCHAR* className) {
    if (!className) return false;
    static const WCHAR* kClasses[] = {
        L"#32768",
        L"#32771",
        L"Xaml_WindowedPopupClass",
        L"TopLevelWindowForOverflowXamlIsland",
        L"NotifyIconOverflowWindow",
        L"TaskbarOverflowWnd",
    };
    for (const WCHAR* shellClass : kClasses) {
        if (wcscmp(className, shellClass) == 0) return true;
    }
    return false;
}

bool IsAltTabClass(const WCHAR* className) {
    if (!className) return false;
    static const WCHAR* kClasses[] = {
        L"MultitaskingViewFrame", L"TaskSwitcherWnd", L"TaskSwitcherOverlayWnd", L"ForegroundStaging", };
    for (const WCHAR* shellClass : kClasses) {
        if (wcscmp(className, shellClass) == 0) return true;
    }
    return false;
}
bool IsTaskbarWindow(HWND hwnd);
bool IsPopupOwnedByTaskbar(HWND hwnd);

bool IsShellSurfaceWindow( HWND hwnd, const WCHAR* className, ShellProcessKind processKind ) {
    if ( !hwnd || !className || !IsWindowVisible(hwnd) ) return false;
    const bool isShellProcess =
        processKind == ShellProcessKind::Explorer ||
        processKind == ShellProcessKind::KnownShell ||
        processKind == ShellProcessKind::StartHost;
    if (IsTaskbarPopupClass(className) && isShellProcess) {
        const bool genericPopup =
            wcscmp(className, L"#32768") == 0 ||
            wcscmp(className, L"#32771") == 0 ||
            wcscmp(className, L"Xaml_WindowedPopupClass") == 0;
        return !genericPopup || IsPopupOwnedByTaskbar(hwnd);
    }
    if (IsAltTabClass(className) && isShellProcess) return true;
    const bool isXamlHost =
        wcsncmp(className, L"XamlExplorerHostIslandWindow",
                wcslen(L"XamlExplorerHostIslandWindow")) == 0;
    // XamlExplorerHostIslandWindow is tracked explicitly through show/hide/
    // cloak events for Start. Explorer may keep the host alive between
    // interactions, so it must not act as a permanently visible generic shell
    // surface during the monitor scan.
    if (isXamlHost && isShellProcess) return false;

    return
        wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0 && processKind == ShellProcessKind::KnownShell;
}

bool IsPopupOwnedByTaskbar(HWND hwnd) {
    HWND owner = GetWindow(hwnd, GW_OWNER);
    while (owner) {
        if (IsTaskbarWindow(GetAncestor(owner, GA_ROOT))) return true;
        owner = GetWindow(owner, GW_OWNER);
    }
    return false;
}
bool MarkShellSurfaceOnMonitors(HWND hwnd, const WCHAR* className,
                                 ShellProcessKind processKind,
                                 const MonitorList& monitors,
                                 bool* shellSurfaceOnMonitor) {
    if ( !shellSurfaceOnMonitor || !IsShellSurfaceWindow(hwnd, className, processKind) ) return false;
    if (IsWindowCloaked(hwnd)) return true;
    RECT rect = {};
    if (!GetWindowRect(hwnd, &rect)) return true;
    for (size_t i = 0; i < monitors.count; ++i) {
        RECT intersection = {};
        if (IntersectRect( &intersection, &rect, &monitors.entries[i].rect )) shellSurfaceOnMonitor[i] = true;
    }
    return true;
}

bool IsApplicationWindowCandidate(HWND hwnd, const WCHAR* className, ShellProcessKind processKind) {
    if ( !hwnd || !className || !IsWindowVisible(hwnd) || IsIconic(hwnd) ) return false;
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if ( GetWindow(hwnd, GW_OWNER) != nullptr && !(exStyle & WS_EX_APPWINDOW) ) return false;
    if (IsDesktopInfrastructureWindow( hwnd, className )) return false;
    if (IsShellSurfaceWindow( hwnd, className, processKind ) || IsTaskbarPopupClass(className)) return false;
    if (IsShellChromeClass(className) || IsTaskbarWindow(hwnd)) return false;
    if (exStyle & WS_EX_TOOLWINDOW) return false;
    return !IsWindowCloaked(hwnd);
}
struct ScanContext {
    const MonitorList* monitors;
    WindowScanResult* result;
    ShellProcessKindCache* processCache;
    HWND foreground;
    bool foregroundIsApplication;
    bool foregroundOnMonitor[kMaxMonitorNumbers];
};

void MarkWindowOnMonitors(
    HWND hwnd, const MonitorList& monitors, bool* onMonitor) {
    if (!hwnd || !onMonitor) return;
    RECT rect = {};
    if (!GetWindowRect(hwnd, &rect)) return;

    WINDOWPLACEMENT placement = {};
    placement.length = sizeof(placement);
    if (GetWindowPlacement(hwnd, &placement) &&
        placement.showCmd == SW_SHOWMAXIMIZED) {
        const HMONITOR windowMonitor =
            MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        for (size_t i = 0; i < monitors.count; ++i) {
            onMonitor[i] = monitors.entries[i].monitor == windowMonitor;
        }
        return;
    }

    for (size_t i = 0; i < monitors.count; ++i) {
        RECT intersection = {};
        if (IntersectRect(&intersection, &rect, &monitors.entries[i].rect)) {
            onMonitor[i] = true;
        }
    }
}

bool IsFullscreenGeometryForMonitor(HWND hwnd, const MonitorEntry& monitorEntry) {
    if (!hwnd || !IsWindow(hwnd)) return false;

    RECT rect = {};
    if (!GetWindowRect(hwnd, &rect) || rect.right <= rect.left ||
        rect.bottom <= rect.top) {
        return false;
    }

    constexpr LONG kFullscreenTolerance = 2;

    const bool coversMonitor =
        abs(rect.left - monitorEntry.rect.left) <= kFullscreenTolerance &&
        abs(rect.top - monitorEntry.rect.top) <= kFullscreenTolerance &&
        abs(rect.right - monitorEntry.rect.right) <= kFullscreenTolerance &&
        abs(rect.bottom - monitorEntry.rect.bottom) <= kFullscreenTolerance;

    if (coversMonitor) return true;

    // Some browser/video implementations keep the work-area boundary while
    // entering borderless fullscreen. In that case, the foreground window can
    // cover the entire current work area without literally reaching rcMonitor.
    // Accept that only when the top-level window is truly borderless and not
    // maximized, which separates it from an ordinary maximized application.
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    const bool borderless =
        (style & WS_CAPTION) == 0 &&
        (style & WS_THICKFRAME) == 0 &&
        (style & WS_MAXIMIZE) == 0;

    if (!borderless) return false;

    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitorEntry.monitor, &monitorInfo)) return false;

    return
        abs(rect.left - monitorInfo.rcWork.left) <= kFullscreenTolerance &&
        abs(rect.top - monitorInfo.rcWork.top) <= kFullscreenTolerance &&
        abs(rect.right - monitorInfo.rcWork.right) <= kFullscreenTolerance &&
        abs(rect.bottom - monitorInfo.rcWork.bottom) <= kFullscreenTolerance;
}

bool IsFullscreenWindowForMonitor( HWND hwnd, const MonitorEntry& monitorEntry, ShellProcessKind processKind ) {
    if ( !hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd) || IsIconic(hwnd) ) return false;
    WCHAR className[256] = {};
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0) return false;
    // Desktop, shell chrome, shell surfaces, and taskbars can also be monitor
    // sized and borderless. They must never become fullscreen owners merely
    // because their geometry happens to match a monitor.
    if (IsDesktopInfrastructureWindow(hwnd, className) ||
        IsShellChromeClass(className) ||
        IsShellSurfaceWindow(hwnd, className, processKind) ||
        IsTaskbarWindow(hwnd)) {
        return false;
    }
    // Foreground application + exact monitor geometry is treated as fullscreen.
    // This covers browser/video fullscreen windows whose style flags remain
    // overlapped internally while the visible frame has been removed.
    return IsFullscreenGeometryForMonitor(hwnd, monitorEntry);
}

int FindFullscreenOwnerIndex(HMONITOR monitor) {
    if (!monitor) return -1;
    for (size_t i = 0; i < kMaxMonitorNumbers; ++i) {
        if (g_fullscreenOwners[i].monitor == monitor) return static_cast<int>(i);
    }
    return -1;
}

bool IsFullscreenOwnerOnSameMonitor(HWND hwnd, HMONITOR monitor) {
    if (!hwnd || !IsWindow(hwnd) || !monitor) return false;
    return MonitorFromWindow( hwnd, MONITOR_DEFAULTTONEAREST ) == monitor;
}

bool IsFullscreenOwnerForForeground(HWND owner, HWND foreground) {
    if (!owner || !foreground) return false;
    return owner == foreground || GetAncestor(owner, GA_ROOT) == foreground;
}

void ClearFullscreenOwnerAtIndex(size_t index);

void NoteRecentFullscreenExit(HWND owner) {
    if (!owner) return;
    g_recentFullscreenExitRoot = owner;
    g_recentFullscreenExitUntilTick =
        GetTickCount64() + kFullscreenExitChildScanSuppressMs;
}

bool IsRecentFullscreenExitRoot(HWND root) {
    if (!root || g_recentFullscreenExitRoot != root) return false;
    const ULONGLONG now = GetTickCount64();
    if (now >= g_recentFullscreenExitUntilTick) {
        g_recentFullscreenExitRoot = nullptr;
        g_recentFullscreenExitUntilTick = 0;
        return false;
    }
    return true;
}

bool HasFullscreenOwnerOnMonitor(HMONITOR monitor) {
    const int index = FindFullscreenOwnerIndex(monitor);
    if (index < 0 || !g_fullscreenOwners[index].hwnd) return false;

    // Fullscreen ownership is a sticky per-display session.
    // Generic visibility/cloak notifications are transient during browser/video
    // fullscreen transitions and must not erase a valid owner during a refresh.
    HWND owner = g_fullscreenOwners[index].hwnd;
    if (!IsFullscreenOwnerOnSameMonitor(owner, monitor)) {
        ClearFullscreenOwnerAtIndex(static_cast<size_t>(index));
        return false;
    }

    return true;
}

void ClearFullscreenOwnerAtIndex(size_t index) {
    if (index >= kMaxMonitorNumbers) return;
    HWINEVENTHOOK hook = g_fullscreenLocationHooks[index];
    g_fullscreenLocationHooks[index] = nullptr;
    g_fullscreenOwners[index] = {};
    if (hook) UnhookWinEvent(hook);
}

void InstallFullscreenLocationHook(size_t index) {
    if (index >= kMaxMonitorNumbers || !g_fullscreenOwners[index].hwnd) return;
    SafeUnhookWinEvent(g_fullscreenLocationHooks[index]);
    DWORD processId = 0;
    DWORD threadId = GetWindowThreadProcessId( g_fullscreenOwners[index].hwnd, &processId );
    if (!processId || !threadId) return;
    g_fullscreenLocationHooks[index] = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
        WinEventProc, processId, threadId, WINEVENT_OUTOFCONTEXT);
    if (!g_fullscreenLocationHooks[index]) {
        Wh_Log(L"Failed to install fullscreen location WinEvent hook for owner %p",
               g_fullscreenOwners[index].hwnd);
    }
}

void SetFullscreenOwner(HMONITOR monitor, HWND hwnd) {
    if (!monitor || !hwnd) return;
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
        if (g_fullscreenOwners[index].hwnd != hwnd) SafeUnhookWinEvent(g_fullscreenLocationHooks[index]);
        if (g_recentFullscreenExitRoot == hwnd) {
            g_recentFullscreenExitRoot = nullptr;
            g_recentFullscreenExitUntilTick = 0;
        }
        g_fullscreenOwners[index].monitor = monitor;
        g_fullscreenOwners[index].hwnd = hwnd;
        if (!g_fullscreenLocationHooks[index]) InstallFullscreenLocationHook(static_cast<size_t>(index));
    }
}

void ClearFullscreenOwnersForWindow(HWND hwnd) {
    if (!hwnd) return;
    for (size_t i = 0; i < kMaxMonitorNumbers; ++i) {
        if (g_fullscreenOwners[i].hwnd == hwnd) ClearFullscreenOwnerAtIndex(i);
    }
}
void ValidateFullscreenOwnerForMonitor(
    HMONITOR monitor, const MonitorList& monitors,
    ShellProcessKindCache& processCache) {
    const int index = FindFullscreenOwnerIndex(monitor);
    if (index < 0 || !g_fullscreenOwners[index].hwnd) return;
    HWND owner = g_fullscreenOwners[index].hwnd;
    if (!IsFullscreenOwnerOnSameMonitor(owner, monitor)) {
        ClearFullscreenOwnerAtIndex(static_cast<size_t>(index));
        return;
    }
    HWND foreground = GetForegroundWindow();
    if (!IsFullscreenOwnerForForeground(owner, foreground)) {
        // Fullscreen ownership is sticky per display. Once an owner has
        // been established, do not destructively revalidate it while focus
        // is elsewhere. During cross-display focus changes Windows may
        // transiently report the fullscreen window as hidden, cloaked, or
        // otherwise not matching its fullscreen geometry. Clearing the
        // cached owner here would prevent it from being rediscovered while
        // it remains fullscreen in the background.
        return;
    }
    for (size_t monitorIndex = 0; monitorIndex < monitors.count; ++monitorIndex) {
        if (monitors.entries[monitorIndex].monitor != monitor) continue;
        DWORD pid = 0;
        GetWindowThreadProcessId(owner, &pid);
        if (!IsFullscreenWindowForMonitor(
                owner, monitors.entries[monitorIndex],
                GetShellProcessKindCached(processCache, pid))) {
            NoteRecentFullscreenExit(owner);
            ClearFullscreenOwnerAtIndex(static_cast<size_t>(index));
        }
        break;
    }
}

void ClearInvalidFullscreenWindowCache(const MonitorList& monitors) {
    for (size_t i = 0; i < kMaxMonitorNumbers; ++i) {
        if (!g_fullscreenOwners[i].monitor) continue;
        HMONITOR monitor = g_fullscreenOwners[i].monitor;
        HWND owner = g_fullscreenOwners[i].hwnd;
        bool monitorStillPresent = false;
        for (size_t monitorIndex = 0; monitorIndex < monitors.count; ++monitorIndex) {
            if (monitors.entries[monitorIndex].monitor == monitor) {
                monitorStillPresent = true;
                break;
            }
        }
        if (!monitorStillPresent || !IsFullscreenOwnerOnSameMonitor(owner, monitor)) ClearFullscreenOwnerAtIndex(i);
    }
}

bool IsTaskbarWindow(HWND hwnd) {
    if (!hwnd) return false;
    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        if (g_taskbarStates[i].hwnd == hwnd) return true;
    }
    WCHAR className[256] = {};
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0) return false;
    return
        wcscmp(className, L"Shell_TrayWnd") == 0 || wcscmp(className, L"Shell_SecondaryTrayWnd") == 0;
}
void ClearFullscreenOwnerForForegroundApplication(
    const MonitorList& monitors, HWND hwnd,
    ShellProcessKindCache& processCache) {
    WCHAR className[256] = {};
    DWORD pid = 0;
    if ( !hwnd || GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0 ) return;
    GetWindowThreadProcessId(hwnd, &pid);
    const ShellProcessKind processKind =
        GetShellProcessKindCached(processCache, pid);

    // The normal candidate filter excludes some active dialogs/tool windows
    // that are still meaningful foreground application UI. Use the same broad
    // foreground fallback as the main scan so a background fullscreen owner
    // cannot keep a taskbar hidden behind such a window.
    bool foregroundIsApplication =
        IsApplicationWindowCandidate(hwnd, className, processKind);
    if (!foregroundIsApplication) {
        if (!IsWindowVisible(hwnd) || IsIconic(hwnd) ||
            IsDesktopInfrastructureWindow(hwnd, className) ||
            IsShellChromeClass(className) ||
            IsShellSurfaceWindow(hwnd, className, processKind) ||
            IsTaskbarPopupClass(className) || IsTaskbarWindow(hwnd) ||
            IsWindowCloaked(hwnd)) {
            return;
        }
        foregroundIsApplication = true;
    }
    if (!foregroundIsApplication) return;

    bool foregroundOnMonitor[kMaxMonitorNumbers] = {};
    MarkWindowOnMonitors(hwnd, monitors, foregroundOnMonitor);

    for (size_t i = 0; i < kMaxMonitorNumbers; ++i) {
        if (!g_fullscreenOwners[i].monitor ||
            !g_fullscreenOwners[i].hwnd) {
            continue;
        }

        const int monitorIndex =
            FindMonitorIndex(monitors, g_fullscreenOwners[i].monitor);
        if (monitorIndex < 0 || !foregroundOnMonitor[monitorIndex]) {
            continue;
        }

        if (IsFullscreenOwnerForForeground(
                g_fullscreenOwners[i].hwnd, hwnd)) {
            ValidateFullscreenOwnerForMonitor(
                g_fullscreenOwners[i].monitor, monitors, processCache);
        } else {
            ClearFullscreenOwnerAtIndex(i);
        }
    }
}

void NoteForegroundFullscreenWindow( const MonitorList& monitors, HWND hwnd, ShellProcessKindCache& processCache ) {
    if (!hwnd || !IsWindow(hwnd)) return;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    const ShellProcessKind processKind = GetShellProcessKindCached(processCache, pid);
    for (size_t i = 0; i < monitors.count; ++i) {
        if (IsFullscreenWindowForMonitor(hwnd, monitors.entries[i], processKind)) {
            SetFullscreenOwner(monitors.entries[i].monitor, hwnd);
        }
    }
}

void RefreshFullscreenWindowCache( const MonitorList& monitors, ShellProcessKindCache& processCache ) {
    ClearInvalidFullscreenWindowCache(monitors);
    HWND foreground = GetForegroundWindow();
    ClearFullscreenOwnerForForegroundApplication( monitors, foreground, processCache );
    if (foreground) {
        HMONITOR foregroundMonitor = MonitorFromWindow( foreground, MONITOR_DEFAULTTONEAREST );
        ValidateFullscreenOwnerForMonitor( foregroundMonitor, monitors, processCache );
    }
    NoteForegroundFullscreenWindow( monitors, foreground, processCache );
}

BOOL CALLBACK ScanWindowsWithMonitorsProc(HWND hwnd, LPARAM lParam) {
    ScanContext* context = reinterpret_cast<ScanContext*>(lParam);
    if ( !context || !context->monitors || !context->result ) return TRUE;

    // Do not early-terminate the enumeration. A fullscreen video window can
    // appear below another application window in Z-order, and stopping once
    // every monitor has an ordinary application would prevent us from seeing it.

    WCHAR className[256] = {};
    if ( GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0 ) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    const ShellProcessKind processKind = GetShellProcessKindCached(*context->processCache, pid);

    if (MarkShellSurfaceOnMonitors(
            hwnd, className, processKind, *context->monitors,
            context->result->shellSurfaceOnMonitor)) {
        return TRUE;
    }

    if (!IsApplicationWindowCandidate( hwnd, className, processKind )) return TRUE;

    // Inspect visible application windows for fullscreen geometry. A background
    // fullscreen window must not keep the taskbar hidden when another application
    // is active on that same display.
    for (size_t i = 0; i < context->monitors->count; ++i) {
        if (context->foregroundIsApplication && context->foreground != hwnd &&
            context->foregroundOnMonitor[i]) {
            continue;
        }
        if (IsFullscreenWindowForMonitor(
                hwnd,
                context->monitors->entries[i],
                processKind)) {
            context->result->fullscreenOnMonitor[i] = true;
            if (context->foreground == hwnd) {
                SetFullscreenOwner(
                    context->monitors->entries[i].monitor,
                    hwnd);
            }
        }
    }

    MarkWindowOnMonitors(
        hwnd, *context->monitors, context->result->applicationOnMonitor);
    return TRUE;
}

struct FullscreenChildScanContext {
    HMONITOR monitors[kMaxMonitorNumbers];
    RECT monitorRects[kMaxMonitorNumbers];
    size_t count;
    WindowScanResult* result;
};

BOOL CALLBACK ScanFullscreenChildProc(HWND hwnd, LPARAM lParam) {
    FullscreenChildScanContext* context =
        reinterpret_cast<FullscreenChildScanContext*>(lParam);
    if (!context || !context->result || !hwnd ||
        !IsWindowVisible(hwnd) || IsWindowCloaked(hwnd)) {
        return TRUE;
    }

    RECT rect = {};
    if (!GetWindowRect(hwnd, &rect) ||
        rect.right <= rect.left || rect.bottom <= rect.top) {
        return TRUE;
    }

    constexpr LONG kFullscreenTolerance = 2;
    for (size_t i = 0; i < context->count; ++i) {
        const RECT& monitorRect = context->monitorRects[i];
        if (abs(rect.left - monitorRect.left) <= kFullscreenTolerance &&
            abs(rect.top - monitorRect.top) <= kFullscreenTolerance &&
            abs(rect.right - monitorRect.right) <= kFullscreenTolerance &&
            abs(rect.bottom - monitorRect.bottom) <= kFullscreenTolerance) {
            context->result->fullscreenOnMonitor[i] = true;
            // The child that actually occupies the monitor is the fullscreen
            // owner. Keeping the browser root here can leave the cached owner
            // fullscreen after the video surface has already exited.
            SetFullscreenOwner(context->monitors[i], hwnd);
        }
    }

    return TRUE;
}

void ScanForegroundChildWindowsForFullscreen(
    const MonitorList& monitors,
    WindowScanResult& result,
    HWND foreground) {
    if (!foreground || !IsWindow(foreground) ||
        !IsWindowVisible(foreground) || IsIconic(foreground)) {
        return;
    }

    WCHAR className[256] = {};
    if (GetClassNameW(foreground, className, ARRAYSIZE(className)) == 0) {
        return;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(foreground, &pid);
    ShellProcessKindCache processCache = {};
    const ShellProcessKind processKind =
        GetShellProcessKindCached(processCache, pid);

    if (IsDesktopInfrastructureWindow(foreground, className) ||
        IsShellChromeClass(className) ||
        IsShellSurfaceWindow(foreground, className, processKind) ||
        IsTaskbarPopupClass(className) ||
        IsTaskbarWindow(foreground)) {
        return;
    }

    // If the foreground top-level window recently exited fullscreen, its
    // renderer/composition children can remain monitor-sized for a short
    // transition interval. Do not promote those stale descendants back to
    // fullscreen ownership during that interval. Child scanning remains
    // enabled for normal fullscreen entry and for unrelated roots.
    if (IsRecentFullscreenExitRoot(foreground)) {
        return;
    }

    // If the foreground top-level window already owns fullscreen geometry on
    // a monitor, keep that top-level window as the authoritative owner. Browser
    // fullscreen can expose multiple monitor-sized renderer/composition child
    // windows during exit; promoting those children here can re-create a stale
    // fullscreen owner immediately after the real owner has exited.
    FullscreenChildScanContext context = {};
    size_t childMonitorCount = 0;
    for (size_t i = 0; i < monitors.count; ++i) {
        if (IsFullscreenWindowForMonitor(
                foreground,
                monitors.entries[i],
                processKind)) {
            continue;
        }
        context.monitors[childMonitorCount] = monitors.entries[i].monitor;
        context.monitorRects[childMonitorCount] = monitors.entries[i].rect;
        ++childMonitorCount;
    }

    context.count = childMonitorCount;
    context.result = &result;

    if (context.count != 0) {
        EnumChildWindows(
            foreground,
            ScanFullscreenChildProc,
            reinterpret_cast<LPARAM>(&context));
    }
}

void ScanWindowsOnce(const MonitorList& monitors, WindowScanResult& result) {
    result = {};
    ShellProcessKindCache processCache = {};
    RefreshFullscreenWindowCache(monitors, processCache);
    for (size_t i = 0; i < monitors.count; ++i) {
        result.fullscreenOnMonitor[i] =
            HasFullscreenOwnerOnMonitor(monitors.entries[i].monitor);
    }
    ScanContext context = {
        &monitors, &result, &processCache, nullptr, false, {}
    };
    context.foreground = GetForegroundWindow();
    if (context.foreground) {
        WCHAR foregroundClassName[256] = {};
        if (GetClassNameW(context.foreground, foregroundClassName,
                          ARRAYSIZE(foregroundClassName)) != 0) {
            DWORD foregroundPid = 0;
            GetWindowThreadProcessId(context.foreground, &foregroundPid);
            context.foregroundIsApplication = IsApplicationWindowCandidate(
                context.foreground, foregroundClassName,
                GetShellProcessKindCached(processCache, foregroundPid));
            if (context.foregroundIsApplication) {
                MarkWindowOnMonitors(
                    context.foreground, monitors, context.foregroundOnMonitor);
            }
        }
    }
    EnumWindows(ScanWindowsWithMonitorsProc, reinterpret_cast<LPARAM>(&context));
    HWND foreground = GetForegroundWindow();

    // Browser fullscreen video can be represented by a renderer/composition
    // child window that fills the monitor while the browser's top-level window
    // retains work-area geometry. Inspect foreground descendants only on
    // monitors where the top-level foreground window is not already the
    // fullscreen owner, preventing stale child windows from re-creating an
    // owner during fullscreen exit.
    ScanForegroundChildWindowsForFullscreen(monitors, result, foreground);
    WCHAR foregroundClassName[256] = {};
    bool foregroundIsShellSurface = false;
    if ( foreground && GetClassNameW(foreground, foregroundClassName, ARRAYSIZE(foregroundClassName)) != 0 ) {
        DWORD foregroundPid = 0;
        GetWindowThreadProcessId(foreground, &foregroundPid);
        const ShellProcessKind foregroundProcessKind = GetShellProcessKindCached(processCache, foregroundPid);
        foregroundIsShellSurface = IsShellSurfaceWindow( foreground, foregroundClassName, foregroundProcessKind );

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
                foreground, foregroundClassName, foregroundProcessKind,
                monitors, result.shellSurfaceOnMonitor);
        }
    }
    if (foreground && IsWindowVisible(foreground) && !IsIconic(foreground) &&
        foregroundClassName[0] != L'\0' &&
        !IsDesktopInfrastructureWindow(foreground, foregroundClassName) &&
        !IsShellChromeClass(foregroundClassName) && !foregroundIsShellSurface &&
        !IsTaskbarPopupClass(foregroundClassName) && !IsTaskbarWindow(foreground)) {
        // Intentionally keep this foreground fallback broader than
        // IsApplicationWindowCandidate: a currently foreground dialog or
        // tool window can still represent active application UI even when
        // taskbar-style enumeration filters it out.
        if (!IsWindowCloaked(foreground)) {
            HMONITOR foregroundMonitor = MonitorFromWindow(foreground, MONITOR_DEFAULTTONEAREST);
            for ( size_t i = 0; i < monitors.count; ++i ) {
                if ( monitors.entries[i].monitor == foregroundMonitor ) {
                    if (!result.fullscreenOnMonitor[i]) result.applicationOnMonitor[i] = true;
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
    const size_t oldCount = g_taskbarStateCount;
    for (size_t i = 0; i < oldCount; ++i) oldStates[i] = g_taskbarStates[i];
    g_taskbarStateCount = 0;
    auto addTaskbar = [&](HWND hwnd) {
        if ( !hwnd || g_taskbarStateCount >= kMaxTaskbars ) return;
        HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        TaskbarMonitorState state = {};
        state.hwnd = hwnd;
        state.monitor = monitor;
        const int logicalMonitorIndex = FindMonitorIndex(monitors, monitor);
        state.monitorNumber = logicalMonitorIndex >= 0
                ? selectionSlots[logicalMonitorIndex]
                : 0;
        state.desktopOnly = false;
        state.hiddenByMod = false;
        for (size_t i = 0; i < oldCount; ++i) {
            if (oldStates[i].hwnd == hwnd) {
                state.desktopOnly = oldStates[i].desktopOnly;
                state.hiddenByMod = oldStates[i].hiddenByMod;
                break;
            }
        }
        g_taskbarStates[ g_taskbarStateCount++ ] = state;
    };
    HWND primary = nullptr;
    while ( (primary = FindWindowExW( nullptr, primary, L"Shell_TrayWnd", nullptr )) != nullptr ) addTaskbar(primary);
    HWND secondary = nullptr;
    while ((secondary = FindWindowExW(
                nullptr, secondary, L"Shell_SecondaryTrayWnd", nullptr)) != nullptr) {
        addTaskbar(secondary);
    }
    for (size_t i = 0; i < oldCount; ++i) {
        if (!oldStates[i].hiddenByMod || !oldStates[i].hwnd || !IsWindow(oldStates[i].hwnd)) continue;
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

void RefreshNativeAutoHideState() {
    APPBARDATA data = {};
    data.cbSize = sizeof(data);
    g_nativeAutoHideEnabled =
        (SHAppBarMessage(ABM_GETSTATE, &data) & ABS_AUTOHIDE) != 0;
}

bool ShouldHideTaskbar(const TaskbarMonitorState& state) {
    return
        state.hwnd && state.monitor &&
        (g_settings.hideAllMonitors ||
         IsMonitorSelected(state.monitorNumber, g_settings.hideMonitor)) &&
        IsBottomDockedTaskbar(state.hwnd, state.monitor) &&
        !g_nativeAutoHideEnabled;
}

void SetTaskbarState(TaskbarMonitorState& state, bool show) {
    if (!state.hwnd || !IsWindow(state.hwnd)) return;

    if (show) {
        if (!state.hiddenByMod) return;
        if (MakeTaskbarTransparent(state.hwnd, false) ||
            ForceRestoreTaskbar(state.hwnd)) {
            state.hiddenByMod = false;
        } else {
            Wh_Log(L"Taskbar restore failed for %p", state.hwnd);
        }
        return;
    }

    if (!state.hiddenByMod && !IsWindowVisible(state.hwnd)) return;

    // MakeTaskbarTransparent(hide=true) re-asserts alpha 0 and
    // WS_EX_TRANSPARENT on an owned window, and safely drops stale ownership
    // if another component removed WS_EX_LAYERED.
    if (MakeTaskbarTransparent(state.hwnd, true)) {
        state.hiddenByMod = true;
    } else {
        Wh_Log(L"Taskbar hide failed for %p", state.hwnd);
        // Never leave a click-through taskbar visible after a partial hide.
        if (state.hiddenByMod && ForceRestoreTaskbar(state.hwnd)) {
            state.hiddenByMod = false;
        }
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
    if (!context || !context->monitors || !context->result || !IsWindowVisible(hwnd)) return TRUE;
    WCHAR className[256] = {};
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0 || !IsTaskbarPopupClass(className)) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (GetShellProcessKindCached(*context->processCache, pid) == ShellProcessKind::None) return TRUE;
    if (IsWindowCloaked(hwnd)) return TRUE;
    const bool genericPopup =
        wcscmp(className, L"#32768") == 0 ||
        wcscmp(className, L"#32771") == 0 ||
        wcscmp(className, L"Xaml_WindowedPopupClass") == 0;
    if (genericPopup && !IsPopupOwnedByTaskbar(hwnd)) return TRUE;
    RECT rect = {};
    if (!GetWindowRect(hwnd, &rect)) return TRUE;
    for (size_t i = 0; i < context->monitors->count; ++i) {
        RECT intersection = {};
        if (IntersectRect(&intersection, &rect,
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
    if (!monitor) return -1;
    for (size_t i = 0; i < monitors.count; ++i) {
        if (monitors.entries[i].monitor == monitor) return static_cast<int>(i);
    }
    return -1;
}

int GetHoverZonePx(HWND hTaskbar, UINT dpi) {
    RECT rect = {};
    if ( hTaskbar && GetWindowRect(hTaskbar, &rect) ) {
        int taskbarHeight = rect.bottom - rect.top;
        if (taskbarHeight > 0) {
            return
                taskbarHeight + MulDiv(g_settings.extraHoverMarginPx, static_cast<int>(dpi), 96);
        }
    }
    return
        MulDiv(48, static_cast<int>(dpi), 96) + MulDiv(g_settings.extraHoverMarginPx, static_cast<int>(dpi), 96);
}

bool IsBottomDockedTaskbar(HWND hTaskbar, HMONITOR monitor) {
    if (!hTaskbar || !monitor) return false;
    RECT taskbarRect = {};
    if (!GetWindowRect( hTaskbar, &taskbarRect )) return false;
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW( monitor, &mi )) return false;
    const LONG tolerance = 4;
    const LONG monitorWidth = mi.rcMonitor.right - mi.rcMonitor.left;
    const LONG monitorHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;
    const LONG taskbarWidth = taskbarRect.right - taskbarRect.left;
    const LONG taskbarHeight = taskbarRect.bottom - taskbarRect.top;
    return
        taskbarRect.bottom >= mi.rcMonitor.bottom - tolerance && taskbarRect.top >
            mi.rcMonitor.top && taskbarWidth >= monitorWidth / 2 && taskbarHeight <
            monitorHeight / 2;
}

bool IsPointNearBottomEdge(HWND hTaskbar, HMONITOR cursorMonitor, POINT pt) {
    if (!hTaskbar || !cursorMonitor) return false;
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW( cursorMonitor, &mi )) return false;
    if (!IsBottomDockedTaskbar( hTaskbar, cursorMonitor )) return false;
    UINT dpi = GetDpiForWindow(hTaskbar);
    if (dpi == 0) dpi = 96;
    int hotZonePx = GetHoverZonePx(hTaskbar, dpi);
    if (hotZonePx < 1) hotZonePx = 1;
    return
        pt.x >= mi.rcMonitor.left && pt.x < mi.rcMonitor.right &&
        pt.y >= mi.rcMonitor.bottom - hotZonePx && pt.y < mi.rcMonitor.bottom;
}

void UpdateCursorHoverSnapshot() {
    CursorHoverSnapshot snapshots[kMaxTaskbars] = {};
    size_t snapshotCount = 0;
    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        if (snapshotCount >= kMaxTaskbars) break;
        const TaskbarMonitorState& state = g_taskbarStates[i];
        if (!ShouldRevealOnHover(state) || !state.desktopOnly ||
            !ShouldHideTaskbar(state) || HasFullscreenOwnerOnMonitor(state.monitor)) {
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
        if (dpi == 0) dpi = 96;
        snapshot.hotZonePx = GetHoverZonePx(state.hwnd, dpi);
        if (snapshot.hotZonePx < 1) snapshot.hotZonePx = 1;
    }
    AcquireSRWLockExclusive(&g_cursorHoverSnapshotLock);
    for (size_t i = 0; i < snapshotCount; ++i) g_cursorHoverSnapshots[i] = snapshots[i];
    g_cursorHoverSnapshotCount = snapshotCount;
    ReleaseSRWLockExclusive(&g_cursorHoverSnapshotLock);
}

bool IsCursorInConfiguredHoverZoneAtSnapshot(POINT pt, HMONITOR cursorMonitor) {
    if (!cursorMonitor) return false;
    AcquireSRWLockShared(&g_cursorHoverSnapshotLock);
    bool result = false;
    for ( size_t i = 0; i < g_cursorHoverSnapshotCount; ++i ) {
        const CursorHoverSnapshot& snapshot = g_cursorHoverSnapshots[i];
        if (snapshot.monitor != cursorMonitor) continue;
        result =
            pt.x >= snapshot.monitorRect.left &&
            pt.x < snapshot.monitorRect.right &&
            pt.y >= snapshot.monitorRect.bottom - snapshot.hotZonePx &&
            pt.y < snapshot.monitorRect.bottom;
        break;
    }
    ReleaseSRWLockShared(&g_cursorHoverSnapshotLock);
    return result;
}

void UpdateTaskbarState() {
    MonitorList monitors = GetCurrentMonitors();
    RefreshTaskbarMonitorStates(monitors);

    if (!g_settings.hideAllMonitors) {
        bool anyHideMonitorSelected = false;
        for (size_t i = 1;
             !anyHideMonitorSelected && i <= kMaxMonitorNumbers; ++i) {
            anyHideMonitorSelected = g_settings.hideMonitor[i];
        }

        if (!anyHideMonitorSelected) {
            g_hoverActive = false;
            g_hoverMonitor = nullptr;
            g_hoverDeadline = 0;
            CancelHoverExpireTimer();
            for (size_t i = 0; i < g_taskbarStateCount; ++i) {
                TaskbarMonitorState& state = g_taskbarStates[i];
                SetTaskbarState(state,
                                !state.desktopOnly || !ShouldHideTaskbar(state));
            }
            UpdateCursorHoverSnapshot();
            return;
        }
    }

    WindowScanResult scan = {};
    ScanWindowsOnce(monitors, scan);

    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        TaskbarMonitorState& state = g_taskbarStates[i];
        bool monitorKnown = false;
        state.desktopOnly = false;

        for (size_t monitorIndex = 0; monitorIndex < monitors.count;
             ++monitorIndex) {
            if (monitors.entries[monitorIndex].monitor != state.monitor) {
                continue;
            }

            monitorKnown = true;
            // Fullscreen counts as display activity, so keep the taskbar in
            // the desktop-only state machine while fullscreen is active.
            // Shell UI and explicit keyboard taskbar activation may temporarily
            // reveal it below.
            state.desktopOnly =
                scan.fullscreenOnMonitor[monitorIndex] ||
                !scan.applicationOnMonitor[monitorIndex];
            break;
        }

        if (!monitorKnown) {
            // Unknown monitor state must fail safe: do not hide a taskbar when
            // its display could not be classified.
            state.desktopOnly = false;
        }
    }

    // Treat the currently foreground taskbar as occupied only when it has
    // keyboard-driven focus. Mouse activation is handled separately so a
    // taskbar click cannot pin a desktop-only taskbar visible.
    HWND foreground = GetForegroundWindow();
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

    const bool taskbarForegroundAfterShell = g_taskbarForegroundAfterShell;
    if (!postMinimizeTaskbarForeground && !taskbarForegroundAfterShell &&
        foreground) {
        for (size_t i = 0; i < g_taskbarStateCount; ++i) {
            if (g_taskbarStates[i].hwnd != foreground) continue;

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

    auto IsMonitorFullscreen = [&](HMONITOR monitor) {
        const int index = FindMonitorIndex(monitors, monitor);
        return index >= 0 && scan.fullscreenOnMonitor[index];
    };
    HMONITOR hoverMonitorBeforeRefresh = g_hoverMonitor;
    const bool hoverMonitorFullscreen =
        g_hoverActive && IsMonitorFullscreen(hoverMonitorBeforeRefresh);
    if (hoverMonitorFullscreen) {
        // A fullscreen transition invalidates an existing hover reveal on that
        // same display. The cursor may already be on another monitor, so use
        // g_hoverMonitor rather than cursorMonitor for this check.
        g_hoverActive = false;
        g_hoverMonitor = nullptr;
        g_hoverDeadline = 0;
        CancelHoverExpireTimer();
    }

    HWND cursorTaskbar = nullptr;
    bool cursorHoverConfigured = false;
    bool cursorMonitorFullscreen = IsMonitorFullscreen(cursorMonitor);
    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        const TaskbarMonitorState& state = g_taskbarStates[i];
        if (state.monitor != cursorMonitor || !state.desktopOnly ||
            !ShouldHideTaskbar(state)) {
            continue;
        }

        cursorTaskbar = state.hwnd;
        cursorHoverConfigured = ShouldRevealOnHover(state);
        break;
    }

    const bool cursorInTaskbarHotZone =
        cursorTaskbar && cursorMonitor &&
        IsPointNearBottomEdge(cursorTaskbar, cursorMonitor, cursorPoint);

    if (g_startCloseHoverSuppressed && !cursorInTaskbarHotZone) {
        // The cursor has left the taskbar area after Start closed. Normal hover
        // detection can resume for all subsequent interactions.
        g_startCloseHoverSuppressed = false;
    }

    const bool hovering =
        cursorInTaskbarHotZone && cursorHoverConfigured &&
        !cursorMonitorFullscreen && !g_startCloseHoverSuppressed;

    if (g_startCloseHoverSuppressed && g_hoverActive) {
        // Start closed while the cursor remained over its taskbar button. Kill
        // the existing hover session so it cannot immediately reveal the
        // taskbar again and cause the one-frame flash.
        g_hoverActive = false;
        g_hoverMonitor = nullptr;
        g_hoverDeadline = 0;
        CancelHoverExpireTimer();
    }

    // This flag is only true while the popup that kept the hover session alive
    // is actually present on the hover monitor. Popups on other displays are
    // then allowed to keep their own taskbars visible as well.
    bool shellPopupKeepAlive = false;
    ShellPopupScanResult shellPopups = {};

    if (hovering) {
        g_hoverActive = true;
        g_hoverMonitor = cursorMonitor;
        g_hoverDeadline = 0;
        CancelHoverExpireTimer();
    } else if (g_hoverActive) {
        const ULONGLONG now = GetTickCount64();
        if (g_hoverDeadline == 0) {
            g_hoverDeadline = now + g_settings.autoHideDelayMs;
            ArmHoverExpireTimer(g_settings.autoHideDelayMs);
        }

        if (now >= g_hoverDeadline) {
            const bool ignorePostMinimizeShellPopup =
                g_lastMinimizeEventTick != 0 &&
                GetTickCount64() - g_lastMinimizeEventTick < 1000;
            if (!ignorePostMinimizeShellPopup) {
                ScanVisibleShellPopupsOnce(monitors, shellPopups);
            }

            const int hoverMonitorIndex =
                FindMonitorIndex(monitors, g_hoverMonitor);
            shellPopupKeepAlive =
                hoverMonitorIndex >= 0 &&
                shellPopups.visibleOnMonitor[hoverMonitorIndex];

            if (shellPopupKeepAlive) {
                g_hoverDeadline = now + 250;
                ArmHoverExpireTimer(250);
            } else {
                g_hoverActive = false;
                g_hoverMonitor = nullptr;
                g_hoverDeadline = 0;
                CancelHoverExpireTimer();
            }
        }
    }

    const HMONITOR revealMonitor = g_hoverActive ? g_hoverMonitor : nullptr;
    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        TaskbarMonitorState& state = g_taskbarStates[i];
        const int monitorIndex = FindMonitorIndex(monitors, state.monitor);
        const bool shellSurface =
            monitorIndex >= 0 && scan.shellSurfaceOnMonitor[monitorIndex];
        const bool searchKeepsVisible =
            g_searchSurfaceVisible &&
            state.monitor == g_searchSurfaceMonitor;
        const bool startKeepsVisible =
            g_startSurfaceVisible &&
            state.monitor == g_startSurfaceMonitor;
        const bool popupKeepsVisible =
            shellPopupKeepAlive && monitorIndex >= 0 &&
            shellPopups.visibleOnMonitor[monitorIndex] &&
            !scan.fullscreenOnMonitor[monitorIndex];
        const bool hoverKeepsVisible =
            state.monitor == revealMonitor &&
            !IsMonitorFullscreen(state.monitor);
        const bool show =
            !state.desktopOnly || !ShouldHideTaskbar(state) || shellSurface ||
            searchKeepsVisible || startKeepsVisible ||
            hoverKeepsVisible || popupKeepsVisible;

        SetTaskbarState(state, show);
    }

    UpdateCursorHoverSnapshot();
}

void ArmHoverExpireTimer(DWORD delayMs) {
    if (!g_workerMessageWindow) return;
    UINT delay = delayMs == 0 ? 1 : delayMs;
    if (!SetTimer(g_workerMessageWindow, kHoverExpireTimerId, delay, nullptr)) {
        Wh_Log(L"Hover expiry timer could not be armed");
    }
}

void CancelHoverExpireTimer() {
    if (g_workerMessageWindow) KillTimer(g_workerMessageWindow, kHoverExpireTimerId);
}

HINSTANCE GetWorkerWindowModuleInstance() {
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&GetWorkerWindowModuleInstance), &module)) {
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

void InstallShellObjectHooks() {
    SafeUnhookWinEvent(g_objectHook);
    SafeUnhookWinEvent(g_cloakHook);

    // SearchHost.exe is a separate process from Explorer, so these global
    // object/cloak hooks are filtered down to Search's CoreWindow.
    g_objectHook = SetWinEventHook(
        EVENT_OBJECT_DESTROY,
        EVENT_OBJECT_HIDE,
        nullptr,
        WinEventProc,
        0,
        0,
        WINEVENT_OUTOFCONTEXT
    );
    if (!g_objectHook) {
        Wh_Log(L"Failed to install global shell object WinEvent hook");
    }

    g_cloakHook = SetWinEventHook(
        EVENT_OBJECT_CLOAKED,
        EVENT_OBJECT_UNCLOAKED,
        nullptr,
        WinEventProc,
        0,
        0,
        WINEVENT_OUTOFCONTEXT
    );
    if (!g_cloakHook) {
        Wh_Log(L"Failed to install global shell cloak WinEvent hook");
    }
}

void InstallTaskbarFocusHook() {
    SafeUnhookWinEvent(g_taskbarFocusHook);
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!taskbar) taskbar = GetShellWindow();
    if (!taskbar) return;
    DWORD processId = 0;
    GetWindowThreadProcessId(taskbar, &processId);
    if (!processId) return;
    g_taskbarFocusHook = SetWinEventHook(
        EVENT_OBJECT_FOCUS, EVENT_OBJECT_FOCUS, nullptr, WinEventProc,
        processId, 0, WINEVENT_OUTOFCONTEXT);
    if (!g_taskbarFocusHook) Wh_Log(L"Failed to install taskbar focus WinEvent hook");
}

void SafeCloseHandle(HANDLE& handle) {
    if (handle) {
        CloseHandle(handle);
        handle = nullptr;
    }
}

void PostRefresh() {
    if (InterlockedExchange(&g_refreshPosted, 1) != 0) return;
    if (!PostThreadMessageW( g_workerThreadId, WM_APP_REFRESH, 0, 0 )) {
        InterlockedExchange(&g_refreshPosted, 0);
        Wh_Log(L"Failed to post refresh message to worker thread");
    }
}

bool HasHoverSnapshots() {
    AcquireSRWLockShared(&g_cursorHoverSnapshotLock);
    const bool result = g_cursorHoverSnapshotCount != 0;
    ReleaseSRWLockShared(&g_cursorHoverSnapshotLock);
    return result;
}

DWORD WINAPI CursorSamplingThread(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    bool lastHoverZone = false;
    HMONITOR lastMonitor = nullptr;
    int cursorPositionFailures = 0;
    for (;;) {
        const bool hoverTrackingActive = HasHoverSnapshots();
        DWORD waitMs = hoverTrackingActive
                ? 50
                : 250;
        if (cursorPositionFailures >= 3) waitMs = 1000;
        DWORD waitResult = WaitForSingleObject(g_cursorStopEvent, waitMs);
        if (waitResult == WAIT_OBJECT_0) break;
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
        HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        const bool hoverZone = IsCursorInConfiguredHoverZoneAtSnapshot(pt, monitor);
        const bool changed = hoverZone != lastHoverZone || (hoverZone && monitor != lastMonitor);
        lastHoverZone = hoverZone;
        lastMonitor = monitor;
        if (changed) PostRefresh();
    }
    return 0;
}

bool IsTaskbarMouseActivated(HWND hwnd) {
    if (!hwnd) return false;

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
    return cursorOverTaskbar || mouseButtonDown;
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
    if (event == EVENT_OBJECT_FOCUS) {
        if (!hwnd) return;
        HWND root = GetAncestor(hwnd, GA_ROOT);
        if (!root) return;
        // Ignore a plain window-level focus notification for Shell_TrayWnd.
        // Windows can generate that as a side effect of desktop interaction,
        // including desktop context-menu invocation. Actual keyboard taskbar
        // navigation focuses a taskbar child/control and is still tracked.
        if (root == hwnd && idObject == OBJID_WINDOW && idChild == CHILDID_SELF) return;
        // A taskbar child can also receive an accessibility focus event while
        // desktop interaction is active. Count it as keyboard taskbar focus
        // only when the taskbar root is actually the foreground window. This
        // keeps desktop context-menu/focus changes from pinning the taskbar
        // visible while preserving Win+T/Win+B style keyboard navigation.
        if (GetForegroundWindow() != root) return;
        if (IsMinimizeInProgress() || IsRecentMinimizeTaskbarFocusSuppressed() ||
            IsRecentShellCloseTaskbarFocusSuppressed() ||
            IsTaskbarMouseActivated(root)) return;
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
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return;
    if (event == EVENT_OBJECT_LOCATIONCHANGE) {
        if (!hwnd) return;
        MonitorList monitors = {};
        bool haveMonitors = false;
        bool ownerFound = false;
        for (size_t i = 0; i < kMaxMonitorNumbers; ++i) {
            if (g_fullscreenOwners[i].hwnd != hwnd) continue;

            ownerFound = true;
            if (!haveMonitors) {
                monitors = GetCurrentMonitors();
                haveMonitors = true;
            }

            const HMONITOR monitor = g_fullscreenOwners[i].monitor;
            const int monitorIndex = FindMonitorIndex(monitors, monitor);
            if (monitorIndex < 0 ||
                !IsFullscreenOwnerOnSameMonitor(hwnd, monitor) ||
                !IsFullscreenGeometryForMonitor(hwnd, monitors.entries[monitorIndex])) {
                if (monitorIndex >= 0 &&
                    IsFullscreenOwnerOnSameMonitor(hwnd, monitor) &&
                    !IsFullscreenGeometryForMonitor(hwnd, monitors.entries[monitorIndex])) {
                    NoteRecentFullscreenExit(hwnd);
                }
                ClearFullscreenOwnerAtIndex(i);
            }
        }
        if (ownerFound) PostRefresh();
        return;
    }
    if (event == EVENT_OBJECT_SHOW ||
        event == EVENT_OBJECT_HIDE ||
        event == EVENT_OBJECT_CLOAKED ||
        event == EVENT_OBJECT_UNCLOAKED ||
        event == EVENT_OBJECT_DESTROY) {
        // Start: only the exact surface recorded from the foreground transition
        // matters. Do this before any process lookup because the HWND was already
        // verified as StartMenuExperienceHost.exe when it was recorded.
        if (hwnd == g_startSurfaceWindow &&
            (event == EVENT_OBJECT_HIDE || event == EVENT_OBJECT_DESTROY)) {
            const HMONITOR startMonitor = g_startSurfaceMonitor;
            g_startSurfaceVisible = false;
            g_startSurfaceWindow = nullptr;
            g_startSurfaceMonitor = nullptr;
            g_taskbarForegroundKeyboardActivated = false;
            g_taskbarForegroundAfterShell = true;
            g_shellCloseFocusIgnoreUntilTick = GetTickCount64() + 600;
            g_startCloseHoverSuppressed = true;
            g_hoverActive = false;
            g_hoverMonitor = nullptr;
            g_hoverDeadline = 0;
            CancelHoverExpireTimer();
            HideTaskbarsForStartClose(startMonitor);
            PostRefresh();
            return;
        }

        if (hwnd == g_searchSurfaceWindow &&
            (event == EVENT_OBJECT_HIDE || event == EVENT_OBJECT_DESTROY ||
             event == EVENT_OBJECT_CLOAKED)) {
            g_searchSurfaceVisible = false;
            g_searchSurfaceWindow = nullptr;
            g_searchSurfaceMonitor = nullptr;
            g_searchHostProcessId = 0;
            // Search can return focus to Shell_TrayWnd immediately after closing.
            g_shellCloseFocusIgnoreUntilTick = GetTickCount64() + 600;
            g_taskbarForegroundKeyboardActivated = false;
            g_taskbarForegroundAfterShell = true;
            PostRefresh();
            return;
        }

        if (!hwnd || GetAncestor(hwnd, GA_ROOT) != hwnd) return;

        WCHAR className[256] = {};
        if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0) return;

        // SearchHost.exe is outside Explorer. CoreWindow is top-level, so after
        // the child-window filter this is the only global class we need to
        // inspect for Search state. Keep the known PID so normal events do not
        // open a new process handle.
        if (wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0) {
            DWORD processId = 0;
            GetWindowThreadProcessId(hwnd, &processId);
            if (g_searchHostProcessId) {
                if (processId != g_searchHostProcessId) return;
            } else if (event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_UNCLOAKED) {
                if (GetShellProcessKind(processId) != ShellProcessKind::SearchHost) return;
                g_searchHostProcessId = processId;
            } else {
                return;
            }

            if (event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_UNCLOAKED) {
                g_searchSurfaceVisible = true;
                g_searchSurfaceWindow = hwnd;
                g_searchSurfaceMonitor =
                    MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
                g_shellCloseFocusIgnoreUntilTick = 0;
            } else if (hwnd == g_searchSurfaceWindow) {
                g_searchSurfaceVisible = false;
                g_searchSurfaceWindow = nullptr;
                g_searchSurfaceMonitor = nullptr;
                g_searchHostProcessId = 0;
                // Search can return focus to Shell_TrayWnd immediately
                // after closing. Do not mistake that transition for
                // keyboard taskbar navigation; it is the shell-close
                // transition itself.
                g_shellCloseFocusIgnoreUntilTick = GetTickCount64() + 600;
                g_taskbarForegroundKeyboardActivated = false;
                g_taskbarForegroundAfterShell = true;
            } else {
                return;
            }
            PostRefresh();
            return;
        }

        // Avoid process inspection for tool windows and obvious shell chrome.
        // Other top-level window changes are coalesced into the normal state scan,
        // which closes the background-application transition gap without adding
        // another per-window process lookup.
        LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        if ((exStyle & WS_EX_TOOLWINDOW) ||
            IsDesktopInfrastructureWindow(hwnd, className) ||
            IsShellChromeClass(className) ||
            IsTaskbarPopupClass(className) || IsTaskbarWindow(hwnd)) {
            return;
        }
        PostRefresh();
        return;
    }
    if (event == EVENT_SYSTEM_FOREGROUND) {
        WCHAR foregroundClassName[256] = {};
        if (hwnd) GetClassNameW(hwnd, foregroundClassName, ARRAYSIZE(foregroundClassName));

        DWORD foregroundPid = 0;
        if (hwnd) GetWindowThreadProcessId(hwnd, &foregroundPid);
        ShellProcessKind foregroundProcessKind = ShellProcessKind::None;
        if (hwnd) foregroundProcessKind = GetShellProcessKind(foregroundPid);
        const bool foregroundIsStartSurface =
            hwnd && foregroundProcessKind == ShellProcessKind::StartHost;
        if (hwnd && foregroundProcessKind == ShellProcessKind::SearchHost &&
            wcscmp(foregroundClassName, L"Windows.UI.Core.CoreWindow") == 0) {
            g_searchHostProcessId = foregroundPid;
            g_searchSurfaceVisible = true;
            g_searchSurfaceWindow = hwnd;
            g_searchSurfaceMonitor =
                MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            g_shellCloseFocusIgnoreUntilTick = 0;
        }
        bool startJustClosed = false;
        bool startCloseForegroundIsApplication = false;
        HMONITOR startCloseMonitor = nullptr;
        if (foregroundIsStartSurface) {
            g_startCloseHoverSuppressed = false;
            g_startSurfaceVisible = true;
            g_startSurfaceWindow = hwnd;
            g_startSurfaceMonitor =
                MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            g_shellCloseFocusIgnoreUntilTick = 0;
        } else if (g_startSurfaceVisible) {
            // Foreground returned to the desktop, taskbar, or an application:
            // this is the authoritative Start-close transition. Preserve the
            // affected monitor before clearing Start's state so the taskbar can
            // be hidden immediately on that display.
            startCloseMonitor = g_startSurfaceMonitor;
            g_startSurfaceVisible = false;
            g_startSurfaceWindow = nullptr;
            g_startSurfaceMonitor = nullptr;
            g_taskbarForegroundKeyboardActivated = false;
            g_taskbarForegroundAfterShell = true;
            g_shellCloseFocusIgnoreUntilTick = GetTickCount64() + 600;
            g_startCloseHoverSuppressed = true;
            g_hoverActive = false;
            g_hoverMonitor = nullptr;
            g_hoverDeadline = 0;
            CancelHoverExpireTimer();
            startJustClosed = true;
            if (hwnd && foregroundClassName[0] != L'\0') {
                ShellProcessKindCache processCache = {};
                startCloseForegroundIsApplication = IsApplicationWindowCandidate(
                    hwnd, foregroundClassName,
                    GetShellProcessKindCached(processCache, foregroundPid));
            }
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
            if (startJustClosed) {
                // Hide immediately without performing a full window scan first.
                // The close event already cleared Start's reveal state, and the
                // following queued refresh performs the authoritative recheck.
                if (!startCloseForegroundIsApplication) {
                    HideTaskbarsForStartClose(startCloseMonitor);
                }
                PostRefresh();
                return;
            }
            g_taskbarForegroundAfterShell = false;
            g_taskbarForegroundKeyboardActivated = false;
            g_lastForegroundWasShellSurface =
                hwnd && foregroundClassName[0] &&
                IsShellSurfaceWindow(
                    hwnd, foregroundClassName, foregroundProcessKind);
            const HMONITOR foregroundMonitor = hwnd
                    ? MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST)
                    : nullptr;
            const int fullscreenOwnerIndex = FindFullscreenOwnerIndex(foregroundMonitor);
            if (g_workerMessageWindow && fullscreenOwnerIndex >= 0 &&
                IsFullscreenOwnerForForeground(
                    g_fullscreenOwners[fullscreenOwnerIndex].hwnd, hwnd)) {
                g_fullscreenValidationAttempt = 0;
                if (!SetTimer(g_workerMessageWindow, kFullscreenValidationTimerId, 16,
                              nullptr)) {
                    Wh_Log(L"Fullscreen validation timer could not be armed");
                }
            }
            PostRefresh();
            return;
        }
        if (startJustClosed) {
            g_taskbarForegroundKeyboardActivated = false;
            g_taskbarForegroundAfterShell = true;
            HideTaskbarsForStartClose(startCloseMonitor);
            PostRefresh();
            return;
        }
        const bool wasShellForeground = g_lastForegroundWasShellSurface;
        g_lastForegroundWasShellSurface = false;
        if (IsMinimizeInProgress() ||
            postMinimizeTaskbarForeground ||
            IsRecentMinimizeTaskbarFocusSuppressed() ||
            IsRecentShellCloseTaskbarFocusSuppressed()) {
            g_taskbarForegroundKeyboardActivated = false;
            g_taskbarForegroundAfterShell = false;
            PostRefresh();
            return;
        }
        const bool mouseActivated = IsTaskbarMouseActivated(hwnd);
        const bool keyboardActivated = g_taskbarForegroundKeyboardActivated && !mouseActivated;
        g_taskbarForegroundKeyboardActivated = keyboardActivated;
        g_taskbarForegroundAfterShell = wasShellForeground && !mouseActivated && !keyboardActivated;
        // A mouse click can reveal the taskbar without first creating a hover
        // session. Start the same hover-dismiss lifecycle so leaving the
        // taskbar starts the normal auto-hide countdown.
        if (mouseActivated) {
            const HMONITOR taskbarMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
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
        g_minimizeStartTick = GetTickCount64();
        g_minimizeFocusIgnoreUntilTick = g_minimizeStartTick + 2000;
        g_lastMinimizeEventTick = g_minimizeStartTick;
        g_taskbarForegroundKeyboardActivated = false;
        g_startSurfaceVisible = false;
        g_startSurfaceWindow = nullptr;
        g_startSurfaceMonitor = nullptr;
        g_startCloseHoverSuppressed = false;
        g_hoverActive = false;
        g_hoverMonitor = nullptr;
        g_hoverDeadline = 0;
        CancelHoverExpireTimer();
        PostRefresh();
        return;
    }
    if (event == EVENT_SYSTEM_MINIMIZEEND) {
        g_minimizeInProgress = false;
        g_minimizeStartTick = 0;
        g_minimizeFocusIgnoreUntilTick = GetTickCount64() + 2000;
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
        if (foregroundIsTaskbar) g_taskbarForegroundKeyboardActivated = false;
        if (hwnd && IsIconic(hwnd)) ClearFullscreenOwnersForWindow(hwnd);
        // Windows can re-show the taskbar shortly after minimize completes.
        // Re-assert the desired state after that transient shell activity.
        if (g_workerMessageWindow) SetTimer( g_workerMessageWindow, kPostMinimizeReassertTimerId, 2000, nullptr );
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
                    ValidateFullscreenOwnerForMonitor( g_fullscreenOwners[i].monitor, monitors, processCache );
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
        g_minimizeStartTick = 0;
        g_minimizeInProgress = false;
        g_minimizeFocusIgnoreUntilTick = 0;
        g_taskbarForegroundKeyboardActivated = false;
        g_startSurfaceVisible = false;
        g_startSurfaceWindow = nullptr;
        g_startSurfaceMonitor = nullptr;
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
            IsFullscreenOwnerForForeground(g_fullscreenOwners[index].hwnd, foreground) &&
            HasFullscreenOwnerOnMonitor(monitor);
        static constexpr UINT kValidationDelaysMs[] = {16, 64};
        if (ownerStillFullscreen && g_fullscreenValidationAttempt < ARRAYSIZE(kValidationDelaysMs)) {
            if (!SetTimer(
                    hwnd, kFullscreenValidationTimerId,
                    kValidationDelaysMs[g_fullscreenValidationAttempt++],
                    nullptr)) {
                Wh_Log(L"Fullscreen validation timer could not be armed");
                g_fullscreenValidationAttempt = 0;
            }
        } else {
            g_fullscreenValidationAttempt = 0;
        }
        return 0;
    }
    if (message == g_taskbarCreatedMessage || message == WM_DISPLAYCHANGE ||
        message == WM_SETTINGCHANGE || message == WM_THEMECHANGED) {
        if (message == g_taskbarCreatedMessage) {
            InstallShellObjectHooks();
            InstallTaskbarFocusHook();
            RefreshNativeAutoHideState();
        } else if (message == WM_SETTINGCHANGE) {
            RefreshNativeAutoHideState();
        }
        PostRefresh();
        return 0;
    }
    return DefWindowProcW( hwnd, message, wParam, lParam );
}

bool CreateWorkerMessageWindow() {
    g_taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");
    if (!g_taskbarCreatedMessage) return false;
    const wchar_t* kClassName = L"WindhawkHideTaskbarOnlyOnDesktopMessageWindow";
    HINSTANCE instance = GetWorkerWindowModuleInstance();
    if (!instance) return false;
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.hInstance = instance;
    wc.lpfnWndProc = WorkerMessageWindowProc;
    wc.lpszClassName = kClassName;
    g_workerWindowClassAtom = RegisterClassExW(&wc);
    if (!g_workerWindowClassAtom) return false;
    g_workerMessageWindow = CreateWindowExW(
        WS_EX_TOOLWINDOW, kClassName, L"", WS_POPUP,
        0, 0, 0, 0, nullptr, nullptr, instance, nullptr);
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
        const wchar_t* kClassName = L"WindhawkHideTaskbarOnlyOnDesktopMessageWindow";
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
        if (g_workerReadyEvent) SetEvent(g_workerReadyEvent);
        return 0;
    }
    InterlockedExchange(&g_workerInitializationResult, 1);
    if (g_workerReadyEvent) SetEvent(g_workerReadyEvent);
    g_foregroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!g_foregroundHook) Wh_Log(L"Failed to install foreground WinEvent hook");
    g_minimizeHook = SetWinEventHook(
        EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND, nullptr,
        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!g_minimizeHook) Wh_Log(L"Failed to install minimize WinEvent hook");
    g_moveHook = SetWinEventHook(
        EVENT_SYSTEM_MOVESIZEEND, EVENT_SYSTEM_MOVESIZEEND, nullptr,
        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!g_moveHook) Wh_Log(L"Failed to install move/size WinEvent hook");
    InstallShellObjectHooks();
    InstallTaskbarFocusHook();
    UpdateTaskbarState();
    constexpr UINT kSafetyPollIntervalMs = 5000;
    UINT_PTR timerId = SetTimer(nullptr, 0, kSafetyPollIntervalMs, nullptr);
    if (!timerId) Wh_Log(L"Safety timer could not be armed");
    for (;;) {
        BOOL result = GetMessageW(&msg, nullptr, 0, 0);
        if (result <= 0) break;
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
    if (timerId) KillTimer(nullptr, timerId);
    if (g_workerMessageWindow) KillTimer(g_workerMessageWindow, kFullscreenValidationTimerId);
    g_fullscreenValidationAttempt = 0;
    CancelHoverExpireTimer();
    SafeUnhookWinEvent(g_foregroundHook);
    SafeUnhookWinEvent(g_minimizeHook);
    SafeUnhookWinEvent(g_moveHook);
    for (size_t i = 0; i < kMaxMonitorNumbers; ++i) SafeUnhookWinEvent(g_fullscreenLocationHooks[i]);
    SafeUnhookWinEvent(g_objectHook);
    SafeUnhookWinEvent(g_cloakHook);
    SafeUnhookWinEvent(g_taskbarFocusHook);
    DestroyWorkerMessageWindow();
    return 0;
}

void LoadSettings() {
    int hoverMargin = Wh_GetIntSetting(L"extraHoverMarginPx");
    if (hoverMargin < 0) {
        hoverMargin = 0;
    } else if (hoverMargin > 200) {
        hoverMargin = 200;
    }
    g_settings.extraHoverMarginPx = hoverMargin;
    int delay = Wh_GetIntSetting(L"autoHideDelayMs");
    if (delay < 0) {
        delay = 0;
    } else if (delay > 10000) {
        delay = 10000;
    }
    g_settings.autoHideDelayMs = static_cast<DWORD>(delay);
    g_settings.hideAllMonitors = false;
    g_settings.hoverAllMonitors = false;
    g_settings.monitorInterfaceMappingCount = 0;
    for ( size_t i = 1; i <= kMaxMonitorNumbers; ++i ) {
        g_settings.hideMonitor[i] = false;
        g_settings.hoverMonitor[i] = false;
    }
    for ( size_t i = 0; i < kMaxMonitorNumbers; ++i ) {
        auto display = WindhawkUtils::StringSetting::make(
            L"monitorInterfaceMappings[%d].display", static_cast<int>(i));
        auto interfaceName = WindhawkUtils::StringSetting::make(
            L"monitorInterfaceMappings[%d].interfaceName", static_cast<int>(i));
        if (!*display && !*interfaceName) break;
        const int displayNumber = ParseMonitorNumber(display.get());
        if (displayNumber == 0 || !*interfaceName) continue;
        if (g_settings.monitorInterfaceMappingCount >= kMaxMonitorNumbers) break;
        MonitorInterfaceMapping& mapping =
            g_settings.monitorInterfaceMappings[g_settings.monitorInterfaceMappingCount++];
        mapping.monitorNumber = static_cast<int>(displayNumber);
        wcsncpy_s( mapping.interfaceName, ARRAYSIZE(mapping.interfaceName), interfaceName.get(), _TRUNCATE );
    }
    for ( size_t i = 0; i < kMaxMonitorNumbers; ++i ) {
        auto value = WindhawkUtils::StringSetting::make(L"hideOnMonitors[%d]", static_cast<int>(i));
        if (!*value) break;
        if (wcscmp(value, L"all") == 0) {
            g_settings.hideAllMonitors = true;
        } else {
            const int number = ParseMonitorNumber(value);
            if (number > 0) g_settings.hideMonitor[number] = true;
        }
    }
    for ( size_t i = 0; i < kMaxMonitorNumbers; ++i ) {
        auto value = WindhawkUtils::StringSetting::make(L"hoverRevealOnMonitors[%d]", static_cast<int>(i));
        if (!*value) break;
        if (wcscmp(value, L"all") == 0) {
            g_settings.hoverAllMonitors = true;
        } else {
            const int number = ParseMonitorNumber(value);
            if (number > 0) g_settings.hoverMonitor[number] = true;
        }
    }
}

BOOL WhTool_ModInit() {
    LoadSettings();
    RefreshNativeAutoHideState();
    // Recover ownership left by an unexpectedly terminated previous tool
    // process before the new worker starts making visibility decisions.
    RestoreAllTaskbars();
    g_workerReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_workerReadyEvent) {
        Wh_Log(L"CreateEvent for worker readiness failed");
        return FALSE;
    }
    InterlockedExchange(&g_workerInitializationResult, 0);
    g_workerThread = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, &g_workerThreadId);
    if (!g_workerThread) {
        SafeCloseHandle(g_workerReadyEvent);
        return FALSE;
    }
    DWORD readyResult = WaitForSingleObject(g_workerReadyEvent, 5000);
    const LONG workerInitializationResult = InterlockedCompareExchange(&g_workerInitializationResult, 0, 0);
    if (readyResult != WAIT_OBJECT_0) {
        EnumWindows(RestoreMarkedTaskbarProc, 0);
        if (g_workerThread) {
            if (!PostThreadMessageW(g_workerThreadId, WM_QUIT, 0, 0)) {
                Wh_Log(L"Failed to stop worker after readiness timeout");
            }
        }
        if (!WaitForThreadWithTimeout( g_workerThread, 5000, L"worker" )) ExitProcess(1);
        SafeCloseHandle(g_workerThread);
        SafeCloseHandle(g_workerReadyEvent);
        return FALSE;
    }
    if (workerInitializationResult != 1) {
        EnumWindows(RestoreMarkedTaskbarProc, 0);
        if (g_workerThread && !WaitForThreadWithTimeout(g_workerThread, 5000, L"worker")) ExitProcess(1);
        SafeCloseHandle(g_workerThread);
        SafeCloseHandle(g_workerReadyEvent);
        return FALSE;
    }
    g_cursorStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_cursorStopEvent) {
        Wh_Log(L"CreateEvent for cursor sampler failed");
        WhTool_ModUninit();
        return FALSE;
    }
    g_cursorThread = CreateThread(nullptr, 0, CursorSamplingThread, nullptr, 0, nullptr);
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
    if (!hwnd || GetPropW(hwnd, kTaskbarOwnershipProp) == nullptr) return TRUE;
    if (!MakeTaskbarTransparent(hwnd, false)) {
        Wh_Log(L"Orphaned taskbar restore failed for %p, forcing visible", hwnd);
        if (!ForceRestoreTaskbar(hwnd)) Wh_Log(L"Orphaned taskbar force-restore failed for %p", hwnd);
    }
    return TRUE;
}

void RestoreAllTaskbars() {
    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        TaskbarMonitorState& state = g_taskbarStates[i];
        if (!state.hwnd || !state.hiddenByMod) continue;
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
    DWORD result = WaitForSingleObject(thread, timeoutMs);
    if (result == WAIT_OBJECT_0) return true;
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
    if (g_cursorStopEvent) SetEvent(g_cursorStopEvent);
    if (g_workerThread) {
        if (!PostThreadMessageW( g_workerThreadId, WM_QUIT, 0, 0 )) Wh_Log(L"Failed to post worker shutdown message");
        if (!WaitForThreadWithTimeout( g_workerThread, 5000, L"worker" )) {
            EnumWindows(RestoreMarkedTaskbarProc, 0);
            ExitProcess(1);
        }
        SafeCloseHandle(g_workerThread);
    }
    if (g_cursorThread) {
        if (!WaitForThreadWithTimeout( g_cursorThread, 3000, L"cursor sampler" )) {
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

// ==WindhawkMod==
// @id              hide-taskbar-only-on-desktop
// @name            Hide Taskbar Only on Desktop
// @description     Hides selected taskbars when their displays are desktop-only or in detected fullscreen
// @version         8.1.8
// @author          Sahil Dashoni
// @github          https://github.com/Sahil-Dashoni
// @include         windhawk.exe
// @compilerOptions -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide Taskbar Only on Desktop

Hides selected bottom-docked taskbars when their display has no relevant
application. Detected borderless fullscreen windows are also treated as
desktop-only display state, with shell, keyboard, and hover reveals handled
separately. Each display is evaluated independently.

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
- Recognized shell UI, taskbar popups, and desktop shell surfaces
- Per-display borderless fullscreen tracking
- Recovery after taskbar recreation or tool-process restart
- Optional stable monitor interface-name mapping
- Optional compatibility with the `windows-animations` Windhawk mod
- Cross-monitor animation protection for minimize, restore/maximize, close, and shell-cloak
- Minimize-aware filtering of animation-session surfaces to avoid delaying
  desktop-only taskbar hiding when the last application is minimized or restored

## Settings

**Extra hover margin (px)** adds to the taskbar-height-based bottom-edge hover
zone and is scaled for the display DPI.

**Auto-hide delay after hover (ms)** controls how long the revealed taskbar
stays visible after the cursor leaves the bottom-edge hover zone. It only
affects hover dismissal; desktop-only hides remain immediate.

**Reveal taskbar on bottom-edge hover** selects the displays where bottom-edge
hover can reveal the taskbar.

**Taskbars to hide on desktop** selects the display slots whose taskbars
participate in desktop-only hiding.

**Stable monitor interface names** lets you pin a display slot to a specific
physical monitor by selecting the slot and pasting its Windows monitor
interface name returned by `EnumDisplayDevicesW` with
`EDD_GET_DEVICE_INTERFACE_NAME`. Each mapping has its own display selector and
text field, so a mapping for Display 10 can be added directly without creating
mappings for Displays 1 through 9. Pinned monitors always use their configured
display slots. Any monitors that are not pinned fill the remaining display
slots in their current logical order. If multiple mappings target the same slot
or physical monitor, the last matching mapping wins.

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

Paste the full value shown after **Interface :** into the mapping for the
display slot you want to attach to that physical monitor.

The mod supports up to 16 fixed logical display slots. Pinned monitors stay in
their selected slots, while unpinned monitors fill the remaining slots in
current logical order.

## Difference from `taskbar-fade`

`taskbar-fade` and this mod both use layered taskbar transparency and can reveal
the taskbar from the bottom edge, so they overlap in mechanism. This mod has a
different primary state model: **desktop-only state is evaluated independently
for each display, and the taskbar becomes immediately eligible for hiding when
that display has no relevant application, subject to explicit hover, shell, or
keyboard reveals**. It also carries the per-display fullscreen, shell-surface,
keyboard, minimize, transition, and recovery handling needed by that model.

The two mods should not be used on the same taskbar because both modify the
taskbar window's transparency/style state.

## Implementation

The state logic runs in a dedicated Windhawk tool process. Taskbars are hidden
with layered-window transparency plus click-through behavior instead of
Windows' native taskbar auto-hide, so the normal desktop work area is
intentionally unchanged.

Fullscreen ownership is tracked per display for borderless monitor-sized
windows. Foreground, move/size, per-owner location, and recognized shell-surface
show/hide/cloak events update state promptly, including geometry changes while
the owner is in the background. Cached fullscreen ownership tolerates transient visibility or
cloak changes while focus moves between displays and remains active while the
owner still matches fullscreen geometry.

Short validation and integrity rechecks cover lifecycle races. Minimize and
restore transitions track their actual lifecycle events so keyboard taskbar
navigation is not unnecessarily suppressed after a completed minimize.
Cross-monitor
window occupancy ignores tiny edge slivers caused by invisible DWM resize
borders while retaining genuine spanning-window activity. Taskbar control focus
is tracked through out-of-context accessibility focus events, so keyboard
navigation such as Win+T and Win+B can reveal a taskbar even when Windows has
not yet made that taskbar the foreground window. A short release debounce keeps
the taskbar visible across transient focus transitions between taskbar buttons.
Mouse-button activity and shell-menu focus are excluded so ordinary mouse
interaction is not misclassified as keyboard navigation.

## Windows Animations Compatibility

This mod is **compatible with the `windows-animations` Windhawk mod**, while
remaining fully functional without it.

The integration is optional and recognizes the window properties exposed by
`windows-animations` when they are present:

- `windows-animations.AnimationSessionV1`
- `windows-animations.Closed`
- `windows-animations.CloseBypass`
- `windows-animations.AnimationGhostV1`

No code from `windows-animations` is loaded or required. When those properties
are absent, the normal desktop-only taskbar logic continues to operate.

During restore/launch animations, the real application can temporarily be
hidden or cloaked while the animated surface still represents the active
application. The compatibility path can preserve that display activity during
the transition.

During close animations, the close-session guard associates the animation with
the application's monitor and prevents transient shell or animation windows on
another desktop-only display from revealing that display's taskbar. The guard
also handles the foreground/destroy ordering Windows can use during animated
window teardown. When an application closes without a Windows Animations close
session, the mod also recognizes the short automatic taskbar-focus handoff and
does not treat it as keyboard taskbar navigation.

The close-session probe runs only when the `windows-animations` mod shows an
`AnimationGhostV1` surface, rather than polling on unrelated window events.
Recognized shell surfaces are also refreshed on DWM cloak and uncloak events so
shell UI that closes by cloaking does not wait for an unrelated event. It
uses a short 16 ms burst while a close session is present, with the existing
grace period keeping the probe alive through the end of the transition.

Normal hover reveal, Start-menu access, and explicit keyboard taskbar
navigation continue to take priority over transition protection.

## Limitations

- **Recovery:** If the dedicated tool process terminates unexpectedly while a
  taskbar is hidden, disable and re-enable this mod in Windhawk or restart
  Windows Explorer to restore it. A later tool-process startup also performs
  ownership recovery. If the taskbar window itself no longer exists, restarting
  Windows Explorer recreates it.
- Desktop-only hiding and hover reveal apply to bottom-docked taskbars.
- The hidden taskbar remains part of the normal work area and is click-through.
- If a taskbar's monitor cannot be resolved during a state refresh, the mod
  fails safe by leaving that taskbar visible.
- Flashing taskbar buttons and tray notifications are not visible while the
  taskbar is transparent.
- Native Windows taskbar auto-hide remains separate from this mod; when it is
  enabled, this mod does not take over that taskbar.
- Other taskbar transparency/style mods can conflict when they modify the same
  taskbar.
- A visible, monitor-sized, captionless, non-resizable application may be
  treated as fullscreen. Cached fullscreen ownership can remain active through
  transient visibility or cloak changes while the owner still matches
  fullscreen geometry. During fullscreen detection, bottom-edge mouse hover
  does not reveal the taskbar; use **Win+T** or **Start** to access it.
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
constexpr UINT_PTR kKeyboardTaskbarReleaseTimerId = 5;
constexpr UINT_PTR kWindowTransitionValidationTimerId = 6;
constexpr UINT_PTR kTaskbarIntegrityTimerId = 7;
constexpr UINT_PTR kWindowsAnimationsCloseProbeTimerId = 8;
constexpr DWORD kWindowsAnimationsCloseProbeIntervalMs = 16;
constexpr DWORD kWindowsAnimationsCloseProbeBurstMs = 750;
constexpr DWORD kWindowsAnimationsCloseGuardGraceMs = 250;
constexpr DWORD kTaskbarIntegrityGuardMs = 1200;
constexpr DWORD kTaskbarIntegrityGuardIntervalMs = 16;
constexpr DWORD kKeyboardTaskbarReleaseDelayMs = 350;
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
HWINEVENTHOOK g_shellSurfaceHook = nullptr;
HWINEVENTHOOK g_shellSurfaceCloakHook = nullptr;
HWINEVENTHOOK g_taskbarFocusHook = nullptr;
HWINEVENTHOOK g_windowDestroyHook = nullptr;
HWINEVENTHOOK g_foregroundLocationHook = nullptr;
HWND g_foregroundLocationWindow = nullptr;
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
            COLORREF currentColorKey = 0;
            BYTE currentAlpha = 255;
            DWORD currentLayeredFlags = 0;
            const bool layeredAttributesValid =
                GetLayeredWindowAttributes(
                    hwnd, &currentColorKey, &currentAlpha, &currentLayeredFlags) != FALSE;
            if (!layeredAttributesValid || currentAlpha != 0) {
                if (!SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA)) return false;
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
LONG g_settingsReloadPending = 0;
UINT g_fullscreenValidationAttempt = 0;
UINT g_windowTransitionValidationAttempt = 0;
ULONGLONG g_taskbarIntegrityDeadline = 0;
HWND g_taskbarIntegrityProtected[kMaxTaskbars] = {};
size_t g_taskbarIntegrityProtectedCount = 0;
bool g_windowsAnimationsCloseActive = false;
HMONITOR g_windowsAnimationsCloseMonitors[kMaxTaskbars] = {};
size_t g_windowsAnimationsCloseMonitorCount = 0;
ULONGLONG g_windowsAnimationsCloseLastSeenTick = 0;
ULONGLONG g_windowsAnimationsCloseProbeDeadline = 0;
HWND g_foregroundTransitionWindow = nullptr;
HMONITOR g_foregroundTransitionMonitor = nullptr;
ULONGLONG g_foregroundTransitionDeadline = 0;
// Close/restore transitions can make the foreground jump through shell windows.
// Keep a short-lived application history so those handoffs don't reveal a
// taskbar that was supposed to remain hidden.
HWND g_lastForegroundApplicationWindow = nullptr;
HMONITOR g_lastForegroundApplicationMonitor = nullptr;
HWND g_previousForegroundApplicationWindow = nullptr;
HMONITOR g_previousForegroundApplicationMonitor = nullptr;
// Windows can automatically move focus to Shell_TrayWnd after the last
// application window disappears. Keep a short record of that real departure
// so the resulting accessibility focus is not treated as keyboard navigation.
constexpr ULONGLONG kTaskbarAutomaticFocusSuppressMs = 1200;
HWND g_recentlyDepartedApplicationWindow = nullptr;
DWORD g_recentlyDepartedApplicationProcessId = 0;
HMONITOR g_recentlyDepartedApplicationMonitor = nullptr;
ULONGLONG g_recentlyDepartedApplicationTick = 0;
bool g_taskbarForegroundKeyboardActivated = false;
HWND g_keyboardTaskbarWindow = nullptr;
bool g_taskbarForegroundAfterShell = false;
bool g_lastForegroundWasShellSurface = false;
bool g_startMenuSessionActive = false;
HWND g_startMenuSessionWindow = nullptr;
ULONGLONG g_lastMinimizeTransitionTick = 0;
// True only while a minimize/restore transition is still in flight. The
// transition tick remains briefly afterward for post-transition reassertion.
HWND g_minimizingWindow = nullptr;
bool g_minimizeInProgress = false;
struct FullscreenMonitorOwner {
    HMONITOR monitor;
    HWND hwnd;
};
FullscreenMonitorOwner g_fullscreenOwners[kMaxMonitorNumbers] = {};

void LoadSettings();

void ApplyPendingSettings();

void WhTool_ModUninit();

BOOL CALLBACK RestoreMarkedTaskbarProc(HWND hwnd, LPARAM lParam);

void ArmHoverExpireTimer(DWORD delayMs);

void CancelHoverExpireTimer();

void ArmKeyboardTaskbarReleaseTimer();

void CancelKeyboardTaskbarReleaseTimer();

void RestoreAllTaskbars();

bool WaitForThreadWithTimeout(HANDLE thread, DWORD timeoutMs, const wchar_t* threadName);

void SafeUnhookWinEvent(HWINEVENTHOOK& hook);

void InstallFullscreenLocationHook(size_t index);

void InstallTaskbarFocusHook();

void InstallWindowDestroyHook();

void InstallForegroundLocationHook(HWND hwnd);

void ArmWindowTransitionValidation();

void ArmTaskbarIntegrityGuard(DWORD durationMs = kTaskbarIntegrityGuardMs);

void CancelTaskbarIntegrityGuard();
bool ScanWindowsAnimationsCloseOnce(HMONITOR* closingMonitors,
                                    size_t* closingMonitorCount);

bool IsMonitorInWindowsAnimationsCloseGuard(HMONITOR monitor);

bool RefreshWindowsAnimationsCloseGuard();
void ArmWindowsAnimationsCloseProbeTimer(
    DWORD durationMs = kWindowsAnimationsCloseProbeBurstMs);

void CancelWindowsAnimationsCloseProbeTimer();

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
    if (!list || list->count >= kMaxMonitorNumbers) return TRUE;
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

bool ShouldHideMonitor(const TaskbarMonitorState& state) {
    return
        g_settings.hideAllMonitors || IsMonitorSelected(state.monitorNumber, g_settings.hideMonitor);
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
    None, Explorer, KnownShell, StartMenu, };

ShellProcessKind GetShellProcessKind(DWORD pid) {
    wchar_t imagePath[MAX_PATH] = {};
    if (!GetWindowProcessImageName( pid, imagePath, ARRAYSIZE(imagePath) )) return ShellProcessKind::None;
    const wchar_t* baseName = wcsrchr(imagePath, L'\\');
    baseName = baseName ? baseName + 1 : imagePath;
    if (_wcsicmp(baseName, L"explorer.exe") == 0) return ShellProcessKind::Explorer;
    if (_wcsicmp(baseName, L"StartMenuExperienceHost.exe") == 0) {
        return ShellProcessKind::StartMenu;
    }
    static const wchar_t* kKnownShellProcesses[] = {
        L"ShellExperienceHost.exe",
        L"ShellHost.exe",
        L"SearchHost.exe",
        L"SearchApp.exe",
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

bool IsShellSurfaceCandidateClass(const WCHAR* className);
bool IsShellSurfaceEventWindow(HWND hwnd, const WCHAR* className,
                               ShellProcessKind processKind);
bool IsStartMenuShellWindow(HWND hwnd, const WCHAR* className,
                             ShellProcessKind processKind) {
    if (!hwnd || !className || processKind != ShellProcessKind::StartMenu) {
        return false;
    }
    return !IsDesktopInfrastructureWindow(hwnd, className) &&
           !IsTaskbarWindow(hwnd);
}
struct StartMenuScanContext {
    const MonitorList* monitors;
    bool* visibleOnMonitor;
    ShellProcessKindCache* processCache;
};

BOOL CALLBACK ScanVisibleStartMenuProc(HWND hwnd, LPARAM lParam) {
    auto* context = reinterpret_cast<StartMenuScanContext*>(lParam);
    if (!context || !context->monitors || !context->visibleOnMonitor ||
        !hwnd || !IsWindowVisible(hwnd) || IsIconic(hwnd)) {
        return TRUE;
    }
    WCHAR className[256] = {};
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0) return TRUE;
    if (!IsShellSurfaceCandidateClass(className)) return TRUE;
    if (IsWindowCloaked(hwnd)) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (GetShellProcessKindCached(*context->processCache, pid) !=
        ShellProcessKind::StartMenu) {
        return TRUE;
    }
    if (IsDesktopInfrastructureWindow(hwnd, className) ||
        IsTaskbarWindow(hwnd)) return TRUE;
    const HMONITOR monitor =
        MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    for (size_t i = 0; i < context->monitors->count; ++i) {
        if (context->monitors->entries[i].monitor == monitor) {
            context->visibleOnMonitor[i] = true;
            break;
        }
    }
    return TRUE;
}
bool ScanVisibleStartMenuOnce(const MonitorList& monitors,
                              bool* visibleOnMonitor) {
    if (!visibleOnMonitor) return false;
    for (size_t i = 0; i < monitors.count; ++i) visibleOnMonitor[i] = false;
    ShellProcessKindCache processCache = {};
    StartMenuScanContext context = {&monitors, visibleOnMonitor, &processCache};
    EnumWindows(ScanVisibleStartMenuProc, reinterpret_cast<LPARAM>(&context));
    bool anyVisible = false;
    for (size_t i = 0; i < monitors.count; ++i) {
        if (visibleOnMonitor[i]) {
            anyVisible = true;
            break;
        }
    }
    return anyVisible;
}

bool IsStartMenuCurrentlyVisible() {
    ShellProcessKindCache processCache = {};
    DWORD shellPid = 0;
    HWND hwnd = nullptr;
    while ((hwnd = FindWindowExW(nullptr, hwnd, nullptr, nullptr)) != nullptr) {
        if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) continue;
        WCHAR className[256] = {};
        if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0) continue;
        if (!IsShellSurfaceCandidateClass(className)) continue;
        if (IsWindowCloaked(hwnd)) continue;
        GetWindowThreadProcessId(hwnd, &shellPid);
        if (GetShellProcessKindCached(processCache, shellPid) !=
            ShellProcessKind::StartMenu) continue;
        if (IsDesktopInfrastructureWindow(hwnd, className) ||
            IsTaskbarWindow(hwnd)) continue;
        return true;
    }
    return false;
}

bool IsShellSurfaceCandidateClass(const WCHAR* className) {
    if (!className) return false;
    return
        IsTaskbarPopupClass(className) ||
        IsAltTabClass(className) ||
        wcsncmp(className, L"XamlExplorerHostIslandWindow",
                wcslen(L"XamlExplorerHostIslandWindow")) == 0 ||
        wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0 ||
        wcscmp(className, L"Windows.UI.Composition.DesktopWindowContentBridge") == 0;
}
bool IsShellSurfaceEventWindow(HWND hwnd, const WCHAR* className,
                               ShellProcessKind processKind) {
    if (!hwnd || !className) return false;
    const bool isShellProcess =
        processKind == ShellProcessKind::Explorer ||
        processKind == ShellProcessKind::KnownShell ||
        processKind == ShellProcessKind::StartMenu;
    if (!isShellProcess) return false;
    if (IsTaskbarPopupClass(className)) {
        const bool genericPopup =
            wcscmp(className, L"#32768") == 0 ||
            wcscmp(className, L"#32771") == 0 ||
            wcscmp(className, L"Xaml_WindowedPopupClass") == 0;
        return !genericPopup || IsPopupOwnedByTaskbar(hwnd);
    }
    if (IsAltTabClass(className)) return true;
    if (wcscmp(className, L"Windows.UI.Composition.DesktopWindowContentBridge") == 0) {
        return processKind == ShellProcessKind::StartMenu;
    }
    if (wcsncmp(className, L"XamlExplorerHostIslandWindow",
                wcslen(L"XamlExplorerHostIslandWindow")) == 0) {
        if (wcscmp(className, L"XamlExplorerHostIslandWindow_WASDK") == 0) {
            RECT rect = {};
            return
                GetWindowRect(hwnd, &rect) &&
                rect.right > rect.left && rect.bottom > rect.top;
        }
        if (wcscmp(className, L"XamlExplorerHostIslandWindow") == 0) {
            return true;
        }
        RECT rect = {};
        return
            GetWindowRect(hwnd, &rect) &&
            rect.right > rect.left && rect.bottom > rect.top;
    }
    return wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0 &&
           (processKind == ShellProcessKind::KnownShell ||
            processKind == ShellProcessKind::StartMenu);
}
bool IsShellSurfaceWindow(HWND hwnd, const WCHAR* className,
                           ShellProcessKind processKind) {
    return hwnd && className && IsWindowVisible(hwnd) &&
           IsShellSurfaceEventWindow(hwnd, className, processKind);
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
bool IsApplicationWindowCandidate(
    HWND hwnd,
    const WCHAR* className,
    ShellProcessKind processKind,
    bool allowCloaked = false) {
    if ( !hwnd || !className || !IsWindowVisible(hwnd) || IsIconic(hwnd) ) return false;
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if ( GetWindow(hwnd, GW_OWNER) != nullptr && !(exStyle & WS_EX_APPWINDOW) ) return false;
    if (IsDesktopInfrastructureWindow( hwnd, className )) return false;
    if (IsShellSurfaceWindow( hwnd, className, processKind ) || IsTaskbarPopupClass(className)) return false;
    if (IsShellChromeClass(className) || IsTaskbarWindow(hwnd)) return false;
    if (exStyle & WS_EX_TOOLWINDOW) return false;
    return allowCloaked || !IsWindowCloaked(hwnd);
}
constexpr wchar_t kWindowsAnimationsSessionProp[] =
    L"windows-animations.AnimationSessionV1";
constexpr wchar_t kWindowsAnimationsClosedProp[] =
    L"windows-animations.Closed";
constexpr wchar_t kWindowsAnimationsCloseBypassProp[] =
    L"windows-animations.CloseBypass";
constexpr wchar_t kWindowsAnimationsAnimationGhostProp[] =
    L"windows-animations.AnimationGhostV1";

bool HasWindowsAnimationsAnimationSession(HWND hwnd) {
    return hwnd &&
           GetPropW(hwnd, kWindowsAnimationsSessionProp) != nullptr;
}

bool IsWindowsAnimationsCloseSession(HWND hwnd) {
    if (!hwnd) return false;
    return
        GetPropW(hwnd, kWindowsAnimationsClosedProp) != nullptr ||
        GetPropW(hwnd, kWindowsAnimationsCloseBypassProp) != nullptr;
}
bool IsWindowsAnimationsMinimizeSuppressedWindow(HWND hwnd) {
    if (!hwnd || !g_minimizingWindow) return false;
    const bool minimizeTransitionActive =
        g_minimizeInProgress ||
        (g_lastMinimizeTransitionTick != 0 &&
         GetTickCount64() - g_lastMinimizeTransitionTick <
             kTaskbarIntegrityGuardMs);
    if (!minimizeTransitionActive) return false;
    if (hwnd == g_minimizingWindow) return true;
    const HANDLE ghostTarget =
        GetPropW(hwnd, kWindowsAnimationsAnimationGhostProp);
    return ghostTarget &&
           reinterpret_cast<HWND>(ghostTarget) == g_minimizingWindow;
}

bool IsWindowsAnimationsActiveSessionCandidate(
    HWND hwnd, const WCHAR* className, ShellProcessKind processKind) {
    if (!hwnd || !className || !HasWindowsAnimationsAnimationSession(hwnd) ||
        IsWindowsAnimationsCloseSession(hwnd)) {
        return false;
    }
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (GetWindow(hwnd, GW_OWNER) != nullptr && !(exStyle & WS_EX_APPWINDOW)) return false;
    if (IsDesktopInfrastructureWindow(hwnd, className)) return false;
    if (IsShellSurfaceWindow(hwnd, className, processKind) ||
        IsTaskbarPopupClass(className)) return false;
    if (IsShellChromeClass(className) || IsTaskbarWindow(hwnd)) return false;
    if (exStyle & WS_EX_TOOLWINDOW) return false;
    return true;
}
struct WindowsAnimationsCloseScanContext {
    HMONITOR* closingMonitors;
    size_t* closingMonitorCount;
};
bool AddWindowsAnimationsCloseMonitor(HWND target,
                                       HMONITOR* closingMonitors,
                                       size_t* closingMonitorCount) {
    if (!target || !closingMonitors || !closingMonitorCount ||
        !HasWindowsAnimationsAnimationSession(target) ||
        !IsWindowsAnimationsCloseSession(target)) {
        return false;
    }
    const HMONITOR monitor =
        MonitorFromWindow(target, MONITOR_DEFAULTTONEAREST);
    if (!monitor) return false;
    for (size_t i = 0; i < *closingMonitorCount; ++i) {
        if (closingMonitors[i] == monitor) return true;
    }
    if (*closingMonitorCount >= kMaxTaskbars) return false;
    closingMonitors[(*closingMonitorCount)++] = monitor;
    return true;
}

BOOL CALLBACK ScanWindowsAnimationsCloseProc(HWND hwnd, LPARAM lParam) {
    auto* context =
        reinterpret_cast<WindowsAnimationsCloseScanContext*>(lParam);
    if (!context || !context->closingMonitors ||
        !context->closingMonitorCount || !hwnd) {
        return TRUE;
    }
    AddWindowsAnimationsCloseMonitor(
        hwnd, context->closingMonitors, context->closingMonitorCount);
    const HANDLE ghostTarget =
        GetPropW(hwnd, kWindowsAnimationsAnimationGhostProp);
    if (ghostTarget) {
        HWND target = reinterpret_cast<HWND>(ghostTarget);
        if (target && IsWindow(target)) {
            AddWindowsAnimationsCloseMonitor(
                target, context->closingMonitors,
                context->closingMonitorCount);
        }
    }
    return TRUE;
}
bool ScanWindowsAnimationsCloseOnce(HMONITOR* closingMonitors,
                                    size_t* closingMonitorCount) {
    if (!closingMonitors || !closingMonitorCount) return false;
    *closingMonitorCount = 0;
    for (size_t i = 0; i < kMaxTaskbars; ++i) closingMonitors[i] = nullptr;
    WindowsAnimationsCloseScanContext context = {
        closingMonitors, closingMonitorCount};
    EnumWindows(ScanWindowsAnimationsCloseProc, reinterpret_cast<LPARAM>(&context));
    return *closingMonitorCount != 0;
}

bool IsMonitorInWindowsAnimationsCloseGuard(HMONITOR monitor) {
    if (!monitor || !g_windowsAnimationsCloseActive) return false;
    for (size_t i = 0; i < g_windowsAnimationsCloseMonitorCount; ++i) {
        if (g_windowsAnimationsCloseMonitors[i] == monitor) return true;
    }
    return false;
}

// The close-session scan is intentionally burst-based: a SHOW of an animation
// ghost arms it, and the grace period keeps it alive until the close settles.
bool RefreshWindowsAnimationsCloseGuard() {
    HMONITOR closingMonitors[kMaxTaskbars] = {};
    size_t closingMonitorCount = 0;
    const bool active = ScanWindowsAnimationsCloseOnce(
        closingMonitors, &closingMonitorCount);
    const ULONGLONG now = GetTickCount64();
    if (active) {
        g_windowsAnimationsCloseActive = true;
        g_windowsAnimationsCloseMonitorCount = closingMonitorCount;
        for (size_t i = 0; i < kMaxTaskbars; ++i) {
            g_windowsAnimationsCloseMonitors[i] =
                i < closingMonitorCount ? closingMonitors[i] : nullptr;
        }
        g_windowsAnimationsCloseLastSeenTick = now;
        ArmTaskbarIntegrityGuard();
        const ULONGLONG probeDeadline =
            now + kWindowsAnimationsCloseProbeBurstMs;
        if (probeDeadline > g_windowsAnimationsCloseProbeDeadline) {
            g_windowsAnimationsCloseProbeDeadline = probeDeadline;
        }
        return true;
    }
    if (g_windowsAnimationsCloseActive &&
        g_windowsAnimationsCloseLastSeenTick != 0 &&
        now - g_windowsAnimationsCloseLastSeenTick <
            kWindowsAnimationsCloseGuardGraceMs) {
        return true;
    }
    if (g_windowsAnimationsCloseActive) {
        g_windowsAnimationsCloseActive = false;
        g_windowsAnimationsCloseMonitorCount = 0;
        for (size_t i = 0; i < kMaxTaskbars; ++i) {
            g_windowsAnimationsCloseMonitors[i] = nullptr;
        }
        g_windowsAnimationsCloseLastSeenTick = 0;
    }
    return false;
}
struct ScanContext {
    const MonitorList* monitors;
    WindowScanResult* result;
    ShellProcessKindCache* processCache;
};

bool IsFullscreenGeometryForMonitor(HWND hwnd, const MonitorEntry& monitorEntry) {
    if (!hwnd || !IsWindow(hwnd)) return false;
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if ((style & WS_CAPTION) != 0 || (style & WS_THICKFRAME) != 0) return false;
    RECT rect = {};
    if (!GetWindowRect(hwnd, &rect) || rect.right <= rect.left ||
        rect.bottom <= rect.top) {
        return false;
    }
    constexpr LONG kFullscreenTolerance = 2;
    return abs(rect.left - monitorEntry.rect.left) <= kFullscreenTolerance &&
           abs(rect.top - monitorEntry.rect.top) <= kFullscreenTolerance &&
           abs(rect.right - monitorEntry.rect.right) <= kFullscreenTolerance &&
           abs(rect.bottom - monitorEntry.rect.bottom) <= kFullscreenTolerance;
}

bool IsFullscreenWindowForMonitor( HWND hwnd, const MonitorEntry& monitorEntry, ShellProcessKind processKind ) {
    if ( !hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd) || IsIconic(hwnd) ) return false;
    WCHAR className[256] = {};
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0) return false;
    if (IsDesktopInfrastructureWindow(hwnd, className) ||
        IsShellChromeClass(className) ||
        IsShellSurfaceWindow(hwnd, className, processKind) ||
        IsTaskbarWindow(hwnd)) {
        return false;
    }
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

bool IsFullscreenOwnerActive(HMONITOR monitor) {
    const int index = FindFullscreenOwnerIndex(monitor);
    if (index < 0 || !g_fullscreenOwners[index].hwnd) return false;
    HWND owner = g_fullscreenOwners[index].hwnd;
    if (!IsFullscreenOwnerOnSameMonitor(owner, monitor) || IsIconic(owner)) return false;
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(monitor, &mi)) return false;
    MonitorEntry monitorEntry = { monitor, mi.rcMonitor };
    return IsFullscreenGeometryForMonitor(owner, monitorEntry);
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
    if (foreground != owner) {
        return;
    }
    for (size_t monitorIndex = 0; monitorIndex < monitors.count; ++monitorIndex) {
        if (monitors.entries[monitorIndex].monitor != monitor) continue;
        DWORD pid = 0;
        GetWindowThreadProcessId(owner, &pid);
        if (!IsFullscreenWindowForMonitor(
                owner, monitors.entries[monitorIndex],
                GetShellProcessKindCached(processCache, pid))) {
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
    if (!IsApplicationWindowCandidate( hwnd, className, GetShellProcessKindCached(processCache, pid) )) return;
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    const int ownerIndex = FindFullscreenOwnerIndex(monitor);
    if (ownerIndex < 0 || g_fullscreenOwners[ownerIndex].hwnd == hwnd) {
        ValidateFullscreenOwnerForMonitor( monitor, monitors, processCache );
        return;
    }
    ClearFullscreenOwnerAtIndex(static_cast<size_t>(ownerIndex));
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
bool IsMeaningfulMonitorIntersection(const RECT& windowRect,
                                      const RECT& monitorRect) {
    RECT intersection = {};
    if (!IntersectRect(&intersection, &windowRect, &monitorRect)) {
        return false;
    }
    const LONG width = intersection.right - intersection.left;
    const LONG height = intersection.bottom - intersection.top;
    if (width <= 0 || height <= 0) {
        return false;
    }
    constexpr LONG kMinCrossMonitorDimension = 24;
    constexpr unsigned long long kMinWindowCoveragePercent = 5;
    const unsigned long long windowWidth =
        static_cast<unsigned long long>(windowRect.right - windowRect.left);
    const unsigned long long windowHeight =
        static_cast<unsigned long long>(windowRect.bottom - windowRect.top);
    const unsigned long long windowArea = windowWidth * windowHeight;
    const unsigned long long intersectionArea =
        static_cast<unsigned long long>(width) *
        static_cast<unsigned long long>(height);
    const bool meaningful =
        width >= kMinCrossMonitorDimension &&
        height >= kMinCrossMonitorDimension &&
        windowArea != 0 &&
        intersectionArea * 100 >= windowArea * kMinWindowCoveragePercent;
    return meaningful;
}

BOOL CALLBACK ScanWindowsWithMonitorsProc(HWND hwnd, LPARAM lParam) {
    ScanContext* context = reinterpret_cast<ScanContext*>(lParam);
    if ( !context || !context->monitors || !context->result ) return TRUE;
    bool allMonitorsClassified = true;
    bool anyFullscreenMonitor = false;
    for (size_t i = 0; i < context->monitors->count; ++i) {
        if (context->result->fullscreenOnMonitor[i]) anyFullscreenMonitor = true;
        if ( !context->result->applicationOnMonitor[i] && !context->result->fullscreenOnMonitor[i] ) {
            allMonitorsClassified = false;
            break;
        }
    }
    if ( allMonitorsClassified && !anyFullscreenMonitor && context->monitors->count != 0 ) {
        return FALSE;
    }
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
    if (IsWindowsAnimationsMinimizeSuppressedWindow(hwnd)) {
        return TRUE;
    }
    const bool windowsAnimationsCloseSession =
        IsWindowsAnimationsCloseSession(hwnd);
    if (windowsAnimationsCloseSession) return TRUE;
    if (IsWindowsAnimationsActiveSessionCandidate(hwnd, className, processKind)) {
        RECT sessionRect = {};
        if (GetWindowRect(hwnd, &sessionRect) &&
            sessionRect.right > sessionRect.left &&
            sessionRect.bottom > sessionRect.top) {
            for (size_t i = 0; i < context->monitors->count; ++i) {
                if (IsMeaningfulMonitorIntersection(
                        sessionRect, context->monitors->entries[i].rect)) {
                    context->result->applicationOnMonitor[i] = true;
                }
            }
            if (!IsWindowVisible(hwnd) || IsIconic(hwnd) || IsWindowCloaked(hwnd)) {
                const HMONITOR sessionMonitor =
                    MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
                for (size_t i = 0; i < context->monitors->count; ++i) {
                    if (context->monitors->entries[i].monitor == sessionMonitor) {
                        context->result->applicationOnMonitor[i] = true;
                        break;
                    }
                }
            }
            return TRUE;
        }
    }
    if (!IsApplicationWindowCandidate(hwnd, className, processKind)) {
        return TRUE;
    }
    RECT rect = {};
    if ( !GetWindowRect( hwnd, &rect ) || rect.right <= rect.left || rect.bottom <= rect.top ) return TRUE;
    WINDOWPLACEMENT placement = {};
    placement.length = sizeof(placement);
    if ( GetWindowPlacement(hwnd, &placement) && placement.showCmd == SW_SHOWMAXIMIZED ) {
        HMONITOR windowMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        for ( size_t i = 0; i < context->monitors->count; ++i ) {
            if ( context->monitors->entries[i].monitor == windowMonitor ) {
                context->result->applicationOnMonitor[i] = true;
                return TRUE;
            }
        }
        return TRUE;
    }
    for ( size_t i = 0; i < context->monitors->count; ++i ) {
        if (IsMeaningfulMonitorIntersection(
                rect, context->monitors->entries[i].rect)) {
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
            IsFullscreenOwnerActive(monitors.entries[i].monitor);
    }
    ScanContext context = {
        &monitors, &result, &processCache
    };
    EnumWindows(ScanWindowsWithMonitorsProc, reinterpret_cast<LPARAM>(&context));
    HWND foreground = GetForegroundWindow();
    WCHAR foregroundClassName[256] = {};
    bool foregroundIsShellSurface = false;
    if ( foreground && GetClassNameW(foreground, foregroundClassName, ARRAYSIZE(foregroundClassName)) != 0 ) {
        DWORD foregroundPid = 0;
        GetWindowThreadProcessId(foreground, &foregroundPid);
        const ShellProcessKind foregroundProcessKind = GetShellProcessKindCached(processCache, foregroundPid);
        foregroundIsShellSurface = IsShellSurfaceWindow( foreground, foregroundClassName, foregroundProcessKind );
        if (foregroundIsShellSurface) {
            const HMONITOR foregroundMonitor = MonitorFromWindow(foreground, MONITOR_DEFAULTTONEAREST);
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
    if (g_keyboardTaskbarWindow) {
        bool keyboardTaskbarStillExists = false;
        for (size_t i = 0; i < g_taskbarStateCount; ++i) {
            if (g_taskbarStates[i].hwnd == g_keyboardTaskbarWindow) {
                keyboardTaskbarStillExists = true;
                break;
            }
        }
        if (!keyboardTaskbarStillExists) {
            g_keyboardTaskbarWindow = nullptr;
            g_taskbarForegroundKeyboardActivated = false;
        }
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
        state.hwnd && state.monitor && ShouldHideMonitor(state) &&
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
    if (MakeTaskbarTransparent(state.hwnd, true)) {
        state.hiddenByMod = true;
    } else {
        Wh_Log(L"Taskbar hide failed for %p", state.hwnd);
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
            !ShouldHideTaskbar(state) ||
            !IsBottomDockedTaskbar(state.hwnd, state.monitor) ||
            IsFullscreenOwnerActive(state.monitor)) {
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

bool IsForegroundTransitionHoldActive(HMONITOR monitor) {
    if (!monitor || !g_foregroundTransitionMonitor ||
        g_foregroundTransitionDeadline == 0) {
        return false;
    }
    const ULONGLONG now = GetTickCount64();
    if (now >= g_foregroundTransitionDeadline) {
        g_foregroundTransitionWindow = nullptr;
        g_foregroundTransitionMonitor = nullptr;
        g_foregroundTransitionDeadline = 0;
        return false;
    }
    if (g_foregroundTransitionMonitor != monitor) return false;
    return true;
}

bool IsTrackedTaskbar(HWND hwnd, size_t* indexOut = nullptr) {
    if (!hwnd) return false;
    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        if (g_taskbarStates[i].hwnd == hwnd) {
            if (indexOut) *indexOut = i;
            return true;
        }
    }
    return false;
}

bool IsTaskbarIntegrityProtected(HWND hwnd) {
    if (!hwnd) return false;
    for (size_t i = 0; i < g_taskbarIntegrityProtectedCount; ++i) {
        if (g_taskbarIntegrityProtected[i] == hwnd) return true;
    }
    return false;
}

void CaptureHiddenTaskbarsForIntegrityGuard() {
    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        const TaskbarMonitorState& state = g_taskbarStates[i];
        if (!state.hwnd || !state.hiddenByMod ||
            !ShouldHideTaskbar(state)) {
            continue;
        }
        if (IsTaskbarIntegrityProtected(state.hwnd)) continue;
        if (g_taskbarIntegrityProtectedCount >= kMaxTaskbars) break;
        g_taskbarIntegrityProtected[g_taskbarIntegrityProtectedCount++] = state.hwnd;
    }
}

bool IsTaskbarInHoverRevealZone(const TaskbarMonitorState& state) {
    if (!state.hwnd || !state.monitor || !ShouldHideTaskbar(state) ||
        !ShouldRevealOnHover(state) || !state.desktopOnly ||
        IsFullscreenOwnerActive(state.monitor)) {
        return false;
    }
    POINT cursorPoint = {};
    if (!GetCursorPos(&cursorPoint)) return false;
    const HMONITOR cursorMonitor =
        MonitorFromPoint(cursorPoint, MONITOR_DEFAULTTONEAREST);
    if (cursorMonitor != state.monitor) return false;
    return IsPointNearBottomEdge(state.hwnd, state.monitor, cursorPoint);
}

void RemoveTaskbarIntegrityProtection(HWND hwnd) {
    if (!hwnd) return;
    for (size_t i = 0; i < g_taskbarIntegrityProtectedCount; ++i) {
        if (g_taskbarIntegrityProtected[i] != hwnd) continue;
        for (size_t j = i + 1; j < g_taskbarIntegrityProtectedCount; ++j) {
            g_taskbarIntegrityProtected[j - 1] =
                g_taskbarIntegrityProtected[j];
        }
        --g_taskbarIntegrityProtectedCount;
        g_taskbarIntegrityProtected[g_taskbarIntegrityProtectedCount] = nullptr;
        break;
    }
}

void UpdateTaskbarIntegritySnapshot() {
    CaptureHiddenTaskbarsForIntegrityGuard();
    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        TaskbarMonitorState& state = g_taskbarStates[i];
        if (!state.hwnd || !IsTaskbarIntegrityProtected(state.hwnd)) {
            continue;
        }
        if (!state.hiddenByMod) {
            const bool closeGuardKeepsHidden =
                g_windowsAnimationsCloseActive &&
                !IsMonitorInWindowsAnimationsCloseGuard(state.monitor) &&
                !IsTaskbarInHoverRevealZone(state);
            if (!closeGuardKeepsHidden) {
                RemoveTaskbarIntegrityProtection(state.hwnd);
                continue;
            }
            if (!MakeTaskbarTransparent(state.hwnd, true)) {
                RemoveTaskbarIntegrityProtection(state.hwnd);
                continue;
            }
            state.hiddenByMod = true;
        }
        if (IsTaskbarInHoverRevealZone(state)) {
            RemoveTaskbarIntegrityProtection(state.hwnd);
            continue;
        }
        MakeTaskbarTransparent(state.hwnd, true);
    }
}

void ArmTaskbarIntegrityGuard(DWORD durationMs) {
    if (!g_workerMessageWindow) return;
    const DWORD duration = durationMs == 0 ? 1 : durationMs;
    const ULONGLONG deadline = GetTickCount64() + duration;
    if (deadline > g_taskbarIntegrityDeadline) {
        g_taskbarIntegrityDeadline = deadline;
    }
    CaptureHiddenTaskbarsForIntegrityGuard();
    if (!SetTimer(g_workerMessageWindow, kTaskbarIntegrityTimerId,
                  kTaskbarIntegrityGuardIntervalMs, nullptr)) {
        Wh_Log(L"Taskbar integrity timer could not be armed");
    }
}

void CancelTaskbarIntegrityGuard() {
    g_taskbarIntegrityDeadline = 0;
    g_taskbarIntegrityProtectedCount = 0;
    for (size_t i = 0; i < kMaxTaskbars; ++i) {
        g_taskbarIntegrityProtected[i] = nullptr;
    }
    if (g_workerMessageWindow) {
        KillTimer(g_workerMessageWindow, kTaskbarIntegrityTimerId);
    }
}

void UpdateTaskbarState() {
    ApplyPendingSettings();
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
    const bool windowsAnimationsCloseActive =
        RefreshWindowsAnimationsCloseGuard();
    bool startMenuVisibleOnMonitor[kMaxMonitorNumbers] = {};
    const bool startMenuCurrentlyVisible =
        ScanVisibleStartMenuOnce(monitors, startMenuVisibleOnMonitor);
    if (startMenuCurrentlyVisible) {
        g_startMenuSessionActive = true;
        for (size_t monitorIndex = 0; monitorIndex < monitors.count; ++monitorIndex) {
            if (startMenuVisibleOnMonitor[monitorIndex]) {
                scan.shellSurfaceOnMonitor[monitorIndex] = true;
            }
        }
    } else if (g_startMenuSessionActive) {
        g_startMenuSessionActive = false;
        g_startMenuSessionWindow = nullptr;
        g_taskbarForegroundKeyboardActivated = false;
        g_keyboardTaskbarWindow = nullptr;
        CancelKeyboardTaskbarReleaseTimer();
        g_taskbarForegroundAfterShell = false;
    }
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
            state.desktopOnly =
                scan.fullscreenOnMonitor[monitorIndex] ||
                !scan.applicationOnMonitor[monitorIndex];
            break;
        }
        if (!monitorKnown) {
            state.desktopOnly = false;
        }
    }
    if (g_foregroundTransitionWindow) {
        const HMONITOR transitionMonitor = g_foregroundTransitionMonitor;
        if (IsForegroundTransitionHoldActive(transitionMonitor)) {
            const int transitionIndex =
                FindMonitorIndex(monitors, transitionMonitor);
            if (transitionIndex >= 0 &&
                !scan.fullscreenOnMonitor[transitionIndex]) {
                for (size_t i = 0; i < g_taskbarStateCount; ++i) {
                    if (g_taskbarStates[i].monitor == transitionMonitor) {
                        g_taskbarStates[i].desktopOnly = false;
                        break;
                    }
                }
            }
        }
    }
    const bool postMinimizeTaskbarForeground =
        g_lastMinimizeTransitionTick != 0 &&
        GetTickCount64() - g_lastMinimizeTransitionTick < 1000;
    if (g_lastMinimizeTransitionTick != 0 && !postMinimizeTaskbarForeground) {
        g_lastMinimizeTransitionTick = 0;
    }
    POINT cursorPoint = {};
    HMONITOR cursorMonitor = nullptr;
    if (GetCursorPos(&cursorPoint)) {
        cursorMonitor =
            MonitorFromPoint(cursorPoint, MONITOR_DEFAULTTONEAREST);
    }
    const bool taskbarForegroundAfterShell = g_taskbarForegroundAfterShell;
    if (!postMinimizeTaskbarForeground && !taskbarForegroundAfterShell &&
        g_taskbarForegroundKeyboardActivated && g_keyboardTaskbarWindow) {
        for (size_t i = 0; i < g_taskbarStateCount; ++i) {
            if (g_taskbarStates[i].hwnd != g_keyboardTaskbarWindow) continue;
            g_taskbarStates[i].desktopOnly = false;
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
    const bool hovering =
        cursorTaskbar && cursorMonitor && cursorHoverConfigured &&
        !cursorMonitorFullscreen &&
        IsPointNearBottomEdge(cursorTaskbar, cursorMonitor, cursorPoint);
    bool shellPopupKeepAlive = false;
    ShellPopupScanResult shellPopups = {};
    if (hovering) {
        g_hoverActive = true;
        g_hoverMonitor = cursorMonitor;
        g_hoverDeadline = 0;
        CancelHoverExpireTimer();
        if (cursorTaskbar) {
            RemoveTaskbarIntegrityProtection(cursorTaskbar);
        }
    } else if (g_hoverActive) {
        const ULONGLONG now = GetTickCount64();
        if (g_hoverDeadline == 0) {
            g_hoverDeadline = now + g_settings.autoHideDelayMs;
            ArmHoverExpireTimer(g_settings.autoHideDelayMs);
        }
        if (now >= g_hoverDeadline) {
            const bool ignorePostMinimizeShellPopup =
                g_lastMinimizeTransitionTick != 0 &&
                GetTickCount64() - g_lastMinimizeTransitionTick < 1000;
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
    const bool transitionHoldForCrossMonitor =
        IsForegroundTransitionHoldActive(g_foregroundTransitionMonitor);
    for (size_t i = 0; i < g_taskbarStateCount; ++i) {
        TaskbarMonitorState& state = g_taskbarStates[i];
        const int monitorIndex = FindMonitorIndex(monitors, state.monitor);
        const bool shellSurface =
            monitorIndex >= 0 && scan.shellSurfaceOnMonitor[monitorIndex];
        const bool popupKeepsVisible =
            shellPopupKeepAlive && monitorIndex >= 0 &&
            shellPopups.visibleOnMonitor[monitorIndex] &&
            !scan.fullscreenOnMonitor[monitorIndex];
        const bool hoverKeepsVisible =
            state.monitor == revealMonitor &&
            !IsMonitorFullscreen(state.monitor);
        const bool startMenuKeepsVisible =
            monitorIndex >= 0 && startMenuVisibleOnMonitor[monitorIndex];
        const bool secondaryMonitorHasRealActivity =
            monitorIndex >= 0 &&
            (scan.applicationOnMonitor[monitorIndex] ||
             scan.fullscreenOnMonitor[monitorIndex]);
        const bool keyboardTaskbarKeepsVisible =
            g_taskbarForegroundKeyboardActivated &&
            g_keyboardTaskbarWindow == state.hwnd;
        const bool closeGuardKeepsSecondaryHidden =
            windowsAnimationsCloseActive &&
            monitorIndex >= 0 &&
            !IsMonitorInWindowsAnimationsCloseGuard(state.monitor) &&
            !scan.applicationOnMonitor[monitorIndex] &&
            !scan.fullscreenOnMonitor[monitorIndex] &&
            ShouldHideTaskbar(state) &&
            !hoverKeepsVisible &&
            !startMenuKeepsVisible &&
            !keyboardTaskbarKeepsVisible;
        if (closeGuardKeepsSecondaryHidden) {
            state.desktopOnly = true;
        }
        // A transition on the primary display must not override the
        // independent state of another display that still has a real app or
        // fullscreen owner. The integrity hold is only for a genuinely
        // desktop-only secondary monitor.
        const bool transitionKeepsSecondaryHidden =
            transitionHoldForCrossMonitor &&
            state.monitor != g_foregroundTransitionMonitor &&
            !secondaryMonitorHasRealActivity &&
            ShouldHideTaskbar(state) &&
            !hoverKeepsVisible &&
            !startMenuKeepsVisible &&
            !keyboardTaskbarKeepsVisible;
        if (transitionKeepsSecondaryHidden) {
            state.desktopOnly = true;
        }
        bool show =
            !state.desktopOnly || !ShouldHideTaskbar(state) || shellSurface ||
            hoverKeepsVisible || popupKeepsVisible || startMenuKeepsVisible;
        if (transitionKeepsSecondaryHidden || closeGuardKeepsSecondaryHidden) {
            show = false;
        }
        if (show) {
            RemoveTaskbarIntegrityProtection(state.hwnd);
        }
        SetTaskbarState(state, show);
    }
    UpdateCursorHoverSnapshot();
}

void ArmWindowsAnimationsCloseProbeTimer(DWORD durationMs) {
    if (!g_workerMessageWindow) return;
    const DWORD duration = durationMs == 0 ? 1 : durationMs;
    const ULONGLONG deadline = GetTickCount64() + duration;
    if (deadline > g_windowsAnimationsCloseProbeDeadline) {
        g_windowsAnimationsCloseProbeDeadline = deadline;
    }
    if (!SetTimer(g_workerMessageWindow, kWindowsAnimationsCloseProbeTimerId,
                  kWindowsAnimationsCloseProbeIntervalMs, nullptr)) {
        Wh_Log(L"Windows animations close probe timer could not be armed");
    }
}

void CancelWindowsAnimationsCloseProbeTimer() {
    g_windowsAnimationsCloseProbeDeadline = 0;
    if (g_workerMessageWindow) {
        KillTimer(g_workerMessageWindow, kWindowsAnimationsCloseProbeTimerId);
    }
}

// Keep validation tied to an actual/recent transition. Outside these short
// windows a full UpdateTaskbarState() pass is unnecessary and expensive.
bool IsWindowTransitionValidationActive() {
    const ULONGLONG now = GetTickCount64();
    return
        IsForegroundTransitionHoldActive(g_foregroundTransitionMonitor) ||
        g_windowsAnimationsCloseActive || g_minimizeInProgress ||
        (g_lastMinimizeTransitionTick != 0 &&
         now - g_lastMinimizeTransitionTick < 1000);
}

void ArmWindowTransitionValidation() {
    if (!g_workerMessageWindow) return;
    if (!IsWindowTransitionValidationActive()) {
        g_windowTransitionValidationAttempt = 0;
        KillTimer(g_workerMessageWindow, kWindowTransitionValidationTimerId);
        return;
    }
    static constexpr UINT kValidationDelaysMs[] = {16, 64, 128, 256, 512, 1000};
    if (g_windowTransitionValidationAttempt >= ARRAYSIZE(kValidationDelaysMs)) {
        g_windowTransitionValidationAttempt = 0;
        return;
    }
    const UINT delay =
        kValidationDelaysMs[g_windowTransitionValidationAttempt++];
    if (!SetTimer(
            g_workerMessageWindow, kWindowTransitionValidationTimerId,
            delay, nullptr)) {
        Wh_Log(L"Window transition validation timer could not be armed");
        g_windowTransitionValidationAttempt = 0;
    }
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

void ArmKeyboardTaskbarReleaseTimer() {
    if (!g_workerMessageWindow) return;
    if (!SetTimer(g_workerMessageWindow, kKeyboardTaskbarReleaseTimerId,
                  kKeyboardTaskbarReleaseDelayMs, nullptr)) {
        Wh_Log(L"Keyboard taskbar release timer could not be armed");
    }
}

void CancelKeyboardTaskbarReleaseTimer() {
    if (g_workerMessageWindow) {
        KillTimer(g_workerMessageWindow, kKeyboardTaskbarReleaseTimerId);
    }
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

void InstallShellSurfaceHook() {
    SafeUnhookWinEvent(g_shellSurfaceHook);
    SafeUnhookWinEvent(g_shellSurfaceCloakHook);
    g_shellSurfaceHook = SetWinEventHook(
        EVENT_OBJECT_SHOW, EVENT_OBJECT_HIDE, nullptr, WinEventProc,
        0, 0, WINEVENT_OUTOFCONTEXT);
    if (!g_shellSurfaceHook) {
        Wh_Log(L"Failed to install global shell surface WinEvent hook");
    }
    g_shellSurfaceCloakHook = SetWinEventHook(
        EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED, nullptr, WinEventProc,
        0, 0, WINEVENT_OUTOFCONTEXT);
    if (!g_shellSurfaceCloakHook) {
        Wh_Log(L"Failed to install global shell surface cloak WinEvent hook");
    }
}

void InstallTaskbarFocusHook() {
    SafeUnhookWinEvent(g_taskbarFocusHook);
    g_taskbarFocusHook = SetWinEventHook(
        EVENT_OBJECT_FOCUS, EVENT_OBJECT_FOCUS, nullptr, WinEventProc,
        0, 0, WINEVENT_OUTOFCONTEXT);
    if (!g_taskbarFocusHook) {
        Wh_Log(L"Failed to install global taskbar focus WinEvent hook");
    }
}

void InstallWindowDestroyHook() {
    SafeUnhookWinEvent(g_windowDestroyHook);
    g_windowDestroyHook = SetWinEventHook(
        EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY, nullptr, WinEventProc,
        0, 0, WINEVENT_OUTOFCONTEXT);
    if (!g_windowDestroyHook) {
        Wh_Log(L"Failed to install global window-destroy WinEvent hook");
    }
}

void InstallForegroundLocationHook(HWND hwnd) {
    SafeUnhookWinEvent(g_foregroundLocationHook);
    g_foregroundLocationWindow = nullptr;
    if (!hwnd) return;
    DWORD processId = 0;
    DWORD threadId = GetWindowThreadProcessId(hwnd, &processId);
    if (!processId || !threadId) return;
    g_foregroundLocationHook = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
        WinEventProc, processId, threadId, WINEVENT_OUTOFCONTEXT);
    if (!g_foregroundLocationHook) {
        Wh_Log(L"Failed to install foreground location WinEvent hook for %p",
               hwnd);
        return;
    }
    g_foregroundLocationWindow = hwnd;
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

bool IsAnyMouseButtonDown() {
    return
        (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 ||
        (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 ||
        (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
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
    return cursorOverTaskbar || IsAnyMouseButtonDown();
}

void ClearRecentlyDepartedApplication() {
    g_recentlyDepartedApplicationWindow = nullptr;
    g_recentlyDepartedApplicationProcessId = 0;
    g_recentlyDepartedApplicationMonitor = nullptr;
    g_recentlyDepartedApplicationTick = 0;
}

bool IsWindowsKeyDown() {
    return (GetAsyncKeyState(VK_LWIN) & 0x8000) ||
           (GetAsyncKeyState(VK_RWIN) & 0x8000);
}

bool IsRecentAutomaticTaskbarFocus(HWND taskbar, bool taskbarMouseActivated) {
    if (!taskbar || GetForegroundWindow() != taskbar ||
        taskbarMouseActivated || g_startMenuSessionActive ||
        IsStartMenuCurrentlyVisible() || !g_recentlyDepartedApplicationTick ||
        !g_recentlyDepartedApplicationProcessId) {
        return false;
    }

    // Win+T/Win+B is explicit keyboard navigation. When the Windows key is
    // still down, never classify the taskbar focus as an automatic close
    // handoff even if it occurs immediately after an application closes.
    if (IsWindowsKeyDown()) {
        ClearRecentlyDepartedApplication();
        return false;
    }

    const HMONITOR taskbarMonitor =
        MonitorFromWindow(taskbar, MONITOR_DEFAULTTONEAREST);
    // Automatic close-focus suppression is intentionally monitor-local. A
    // taskbar on another display may be legitimately activated independently.
    if (g_recentlyDepartedApplicationMonitor &&
        taskbarMonitor != g_recentlyDepartedApplicationMonitor) {
        return false;
    }

    const ULONGLONG now = GetTickCount64();
    if (now - g_recentlyDepartedApplicationTick >
        kTaskbarAutomaticFocusSuppressMs) {
        ClearRecentlyDepartedApplication();
        return false;
    }

    const HWND departedWindow = g_recentlyDepartedApplicationWindow;
    if (departedWindow && IsWindow(departedWindow)) {
        DWORD processId = 0;
        if (!GetWindowThreadProcessId(departedWindow, &processId) ||
            processId != g_recentlyDepartedApplicationProcessId) {
            ClearRecentlyDepartedApplication();
            return false;
        }

        if (IsWindowVisible(departedWindow) &&
            !IsWindowCloaked(departedWindow)) {
            return false;
        }
    }

    // The close target may already have been destroyed. In that case the
    // stored process ID and monitor are the stable identity for this short
    // handoff window; requiring GetWindowThreadProcessId on the dead HWND made
    // automatic taskbar focus look like Win+T and left the taskbar visible.
    return true;
}

bool IsShellSurfaceLifecycleEvent(DWORD event) {
    return event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_HIDE ||
           event == EVENT_OBJECT_CLOAKED || event == EVENT_OBJECT_UNCLOAKED;
}

bool IsShellSurfaceLifecycleShowEvent(DWORD event) {
    return event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_UNCLOAKED;
}

void CompleteMinimizeTransitionForWindow(HWND hwnd) {
    if (!hwnd || hwnd != g_minimizingWindow || !g_minimizeInProgress ||
        !IsIconic(hwnd)) {
        return;
    }

    // MINIMIZEEND is the restore event, not the completion of minimizing.
    // Mark the minimize transition complete as soon as the target is actually
    // minimized so keyboard taskbar navigation is not blocked until restore.
    g_minimizeInProgress = false;
    g_lastMinimizeTransitionTick = GetTickCount64();
    if (g_workerMessageWindow) {
        SetTimer(g_workerMessageWindow, kPostMinimizeReassertTimerId, 1200,
                 nullptr);
    }
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD) {
    if (event == EVENT_OBJECT_FOCUS) {
        if (!hwnd) return;
        HWND root = GetAncestor(hwnd, GA_ROOT);
        if (!root) return;
        bool taskbarRoot = false;
        for (size_t i = 0; i < g_taskbarStateCount; ++i) {
            if (g_taskbarStates[i].hwnd == root) {
                taskbarRoot = true;
                break;
            }
        }
        if (!taskbarRoot && !IsTaskbarWindow(root)) {
            if (g_keyboardTaskbarWindow && !g_minimizeInProgress) {
                ArmKeyboardTaskbarReleaseTimer();
            } else {
                g_keyboardTaskbarWindow = nullptr;
                g_taskbarForegroundKeyboardActivated = false;
            }
            return;
        }
        if (!taskbarRoot) return;
        if (root == hwnd && idObject == OBJID_WINDOW && idChild == CHILDID_SELF) return;
        if (g_minimizeInProgress || IsAnyMouseButtonDown()) return;
        if (g_startMenuSessionActive || IsStartMenuCurrentlyVisible()) return;
        const bool mouseActivated = IsTaskbarMouseActivated(root);
        if (!mouseActivated && IsWindowsKeyDown()) {
            // A real Win+T/Win+B activation must win over the short close
            // handoff suppression, including arrow-key navigation that follows.
            ClearRecentlyDepartedApplication();
        }
        HWND foreground = GetForegroundWindow();
        if (foreground == root &&
            IsRecentAutomaticTaskbarFocus(root, mouseActivated)) {
            CancelKeyboardTaskbarReleaseTimer();
            g_taskbarForegroundKeyboardActivated = false;
            g_keyboardTaskbarWindow = nullptr;
            return;
        }
        if (foreground && foreground != root) {
            WCHAR foregroundClass[256] = {};
            if (GetClassNameW(
                    foreground, foregroundClass, ARRAYSIZE(foregroundClass)) != 0) {
                const bool desktopShellPopup =
                    IsTaskbarPopupClass(foregroundClass) ||
                    IsAltTabClass(foregroundClass);
                const bool desktopContextHost =
                    wcscmp(foregroundClass, L"XamlExplorerHostIslandWindow_WASDK") == 0;
                if (desktopShellPopup || desktopContextHost) return;
            }
        }
        CancelKeyboardTaskbarReleaseTimer();
        g_keyboardTaskbarWindow = root;
        g_taskbarForegroundKeyboardActivated = true;
        g_taskbarForegroundAfterShell = false;
        PostRefresh();
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
                ClearFullscreenOwnerAtIndex(i);
            }
        }
        const bool foregroundLocationEvent =
            hwnd == g_foregroundLocationWindow && hwnd == GetForegroundWindow();
        const bool minimizeTransitionActive =
            g_minimizeInProgress ||
            (g_lastMinimizeTransitionTick != 0 &&
             GetTickCount64() - g_lastMinimizeTransitionTick < 1000);
        if (foregroundLocationEvent && !minimizeTransitionActive) {
            g_foregroundTransitionWindow = hwnd;
            g_foregroundTransitionMonitor =
                MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            g_foregroundTransitionDeadline =
                GetTickCount64() + kTaskbarIntegrityGuardMs;
            ArmTaskbarIntegrityGuard();
        }
        if (ownerFound || foregroundLocationEvent) PostRefresh();
        return;
    }
    // Shell surfaces may transition through visibility or DWM cloak state.
    // Treat both forms as lifecycle changes so Start/Search can close without
    // waiting for an unrelated foreground or safety-poll event.
    if (IsShellSurfaceLifecycleEvent(event) && IsTaskbarWindow(hwnd)) {
        if (IsShellSurfaceLifecycleShowEvent(event)) {
            RefreshWindowsAnimationsCloseGuard();
            size_t taskbarIndex = 0;
            if (IsTrackedTaskbar(hwnd, &taskbarIndex) &&
                g_taskbarStates[taskbarIndex].hiddenByMod) {
                MakeTaskbarTransparent(hwnd, true);
                ArmTaskbarIntegrityGuard();
            }
        }
        PostRefresh();
        return;
    }
    if (IsShellSurfaceLifecycleEvent(event)) {
        if (event == EVENT_OBJECT_HIDE && hwnd) {
            const HWND root = GetAncestor(hwnd, GA_ROOT);
            if (root && root == g_lastForegroundApplicationWindow) {
                g_recentlyDepartedApplicationWindow = root;
                g_recentlyDepartedApplicationProcessId = 0;
                GetWindowThreadProcessId(
                    root, &g_recentlyDepartedApplicationProcessId);
                g_recentlyDepartedApplicationMonitor =
                    MonitorFromWindow(root, MONITOR_DEFAULTTONEAREST);
                g_recentlyDepartedApplicationTick = GetTickCount64();
            }
        }
        if (event == EVENT_OBJECT_HIDE && hwnd &&
            hwnd == g_minimizingWindow && IsIconic(hwnd)) {
            CompleteMinimizeTransitionForWindow(hwnd);
        }
        if (event == EVENT_OBJECT_SHOW && hwnd &&
            GetPropW(hwnd, kWindowsAnimationsAnimationGhostProp)) {
            ArmWindowsAnimationsCloseProbeTimer();
        }
        WCHAR className[256] = {};
        if (!hwnd || GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0) return;
        if (!IsShellSurfaceCandidateClass(className)) return;
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        ShellProcessKindCache processCache = {};
        const ShellProcessKind processKind =
            GetShellProcessKindCached(processCache, pid);
        const bool isStartMenu =
            IsStartMenuShellWindow(hwnd, className, processKind);
        if (!isStartMenu &&
            !IsShellSurfaceEventWindow(hwnd, className, processKind)) {
            return;
        }
        const bool showLikeEvent = IsShellSurfaceLifecycleShowEvent(event);
        if (isStartMenu) {
            if (showLikeEvent) {
                g_startMenuSessionActive = true;
                g_startMenuSessionWindow = hwnd;
                g_taskbarForegroundKeyboardActivated = false;
                g_keyboardTaskbarWindow = nullptr;
                CancelKeyboardTaskbarReleaseTimer();
                g_taskbarForegroundAfterShell = false;
                g_lastMinimizeTransitionTick = 0;
            } else {
                if (!g_startMenuSessionWindow ||
                    g_startMenuSessionWindow == hwnd) {
                    g_startMenuSessionActive = false;
                    g_startMenuSessionWindow = nullptr;
                }
                g_taskbarForegroundKeyboardActivated = false;
                g_keyboardTaskbarWindow = nullptr;
                CancelKeyboardTaskbarReleaseTimer();
                g_taskbarForegroundAfterShell = false;
                g_lastMinimizeTransitionTick = 0;
                g_windowTransitionValidationAttempt = 0;
                ArmWindowTransitionValidation();
            }
            PostRefresh();
            return;
        }
        if (showLikeEvent) {
            g_taskbarForegroundAfterShell = false;
        } else if (!g_taskbarForegroundKeyboardActivated) {
            g_taskbarForegroundAfterShell = true;
        }
        PostRefresh();
        return;
    }
    if (event == EVENT_OBJECT_DESTROY &&
        idObject == OBJID_WINDOW && idChild == CHILDID_SELF) {
        // A minimize target can disappear before the post-minimize guard
        // expires. Clear the stored HWND at destruction time so Windows HWND
        // reuse cannot make an unrelated new window inherit the suppression.
        if (hwnd == g_minimizingWindow) {
            g_minimizingWindow = nullptr;
        }
        bool isFullscreenOwner = false;
        for (size_t i = 0; i < kMaxMonitorNumbers; ++i) {
            if (g_fullscreenOwners[i].hwnd == hwnd) {
                isFullscreenOwner = true;
                break;
            }
        }
        const bool isLastForegroundApplication =
            hwnd && hwnd == g_lastForegroundApplicationWindow;
        const bool isPreviousForegroundApplication =
            hwnd && hwnd == g_previousForegroundApplicationWindow;
        const bool isKeyboardTaskbar = hwnd && hwnd == g_keyboardTaskbarWindow;
        const bool isTrackedTaskbar = hwnd && IsTrackedTaskbar(hwnd);
        const bool isStartMenuSessionWindow =
            hwnd && hwnd == g_startMenuSessionWindow;
        HMONITOR destroyedApplicationMonitor = nullptr;
        if (isLastForegroundApplication) {
            destroyedApplicationMonitor = g_lastForegroundApplicationMonitor;
        } else if (isPreviousForegroundApplication) {
            destroyedApplicationMonitor = g_previousForegroundApplicationMonitor;
        }
        if (hwnd == g_foregroundTransitionWindow) {
            g_foregroundTransitionWindow = nullptr;
        }
        if (!destroyedApplicationMonitor && !isKeyboardTaskbar &&
            !isTrackedTaskbar && !isStartMenuSessionWindow &&
            !isFullscreenOwner) {
            return;
        }
        if (isFullscreenOwner) {
            ClearFullscreenOwnersForWindow(hwnd);
        }
        if (hwnd == g_foregroundLocationWindow) {
            InstallForegroundLocationHook(nullptr);
        }
        if (destroyedApplicationMonitor) {
            g_foregroundTransitionWindow = nullptr;
            g_foregroundTransitionMonitor = destroyedApplicationMonitor;
            g_foregroundTransitionDeadline =
                GetTickCount64() + kTaskbarIntegrityGuardMs;
            ArmTaskbarIntegrityGuard();
        }
        if (isLastForegroundApplication) {
            g_lastForegroundApplicationWindow =
                g_previousForegroundApplicationWindow;
            g_lastForegroundApplicationMonitor =
                g_previousForegroundApplicationMonitor;
            g_previousForegroundApplicationWindow = nullptr;
            g_previousForegroundApplicationMonitor = nullptr;
        } else if (isPreviousForegroundApplication) {
            g_previousForegroundApplicationWindow = nullptr;
            g_previousForegroundApplicationMonitor = nullptr;
        }
        if (isStartMenuSessionWindow) {
            g_startMenuSessionActive = false;
            g_startMenuSessionWindow = nullptr;
            g_taskbarForegroundAfterShell = false;
        }
        if (isKeyboardTaskbar) {
            g_taskbarForegroundKeyboardActivated = false;
            g_keyboardTaskbarWindow = nullptr;
            CancelKeyboardTaskbarReleaseTimer();
        }
        if (isTrackedTaskbar) {
            g_hoverActive = false;
            g_hoverMonitor = nullptr;
            g_hoverDeadline = 0;
            CancelHoverExpireTimer();
        }
        g_windowTransitionValidationAttempt = 0;
        ArmTaskbarIntegrityGuard();
        ArmWindowTransitionValidation();
        PostRefresh();
        return;
    }
    if (event == EVENT_SYSTEM_FOREGROUND) {
        WCHAR foregroundClassName[256] = {};
        if (hwnd) GetClassNameW(hwnd, foregroundClassName, ARRAYSIZE(foregroundClassName));
        if (g_minimizeInProgress && g_minimizingWindow) {
            if (hwnd == g_minimizingWindow && !IsIconic(hwnd)) {
                // MINIMIZEEND starts the restore transition. Once the restored
                // application actually becomes foreground, that transition is done.
                g_minimizeInProgress = false;
                g_lastMinimizeTransitionTick = GetTickCount64();
                if (g_workerMessageWindow) {
                    SetTimer(g_workerMessageWindow, kPostMinimizeReassertTimerId,
                             1200, nullptr);
                }
            } else if (hwnd != g_minimizingWindow && IsIconic(g_minimizingWindow)) {
                CompleteMinimizeTransitionForWindow(g_minimizingWindow);
            }
        }
        const bool postMinimizeTaskbarForeground =
            g_lastMinimizeTransitionTick != 0 &&
            GetTickCount64() - g_lastMinimizeTransitionTick < 1000;
        const bool minimizeTransitionActive =
            g_minimizeInProgress || postMinimizeTaskbarForeground;
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
            if (g_keyboardTaskbarWindow && !g_minimizeInProgress &&
                !postMinimizeTaskbarForeground) {
                ArmKeyboardTaskbarReleaseTimer();
            } else {
                g_taskbarForegroundKeyboardActivated = false;
                g_keyboardTaskbarWindow = nullptr;
            }
            DWORD pid = 0;
            if (hwnd) GetWindowThreadProcessId(hwnd, &pid);
            ShellProcessKindCache processCache = {};
            const ShellProcessKind foregroundProcessKind =
                GetShellProcessKindCached(processCache, pid);
            const bool foregroundIsShellSurface =
                hwnd && foregroundClassName[0] &&
                IsShellSurfaceWindow(
                    hwnd, foregroundClassName, foregroundProcessKind);
            const bool foregroundIsApplication =
                hwnd && foregroundClassName[0] &&
                !IsDesktopInfrastructureWindow(hwnd, foregroundClassName) &&
                !foregroundIsShellSurface &&
                !IsShellChromeClass(foregroundClassName) &&
                !IsTaskbarPopupClass(foregroundClassName) &&
                !IsTaskbarWindow(hwnd) &&
                IsApplicationWindowCandidate(
                    hwnd, foregroundClassName, foregroundProcessKind, true);
            if (foregroundIsApplication && hwnd == g_minimizingWindow &&
                !IsIconic(hwnd)) {
                g_minimizingWindow = nullptr;
            }
            if (foregroundIsApplication &&
                hwnd != g_lastForegroundApplicationWindow) {
                ClearRecentlyDepartedApplication();
                g_previousForegroundApplicationWindow =
                    g_lastForegroundApplicationWindow;
                g_previousForegroundApplicationMonitor =
                    g_lastForegroundApplicationMonitor;
                g_lastForegroundApplicationWindow = hwnd;
                g_lastForegroundApplicationMonitor =
                    MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            }
            if (hwnd && IsStartMenuShellWindow(
                    hwnd, foregroundClassName, foregroundProcessKind)) {
                g_startMenuSessionActive = true;
                g_startMenuSessionWindow = hwnd;
            } else if (g_startMenuSessionActive && !IsStartMenuCurrentlyVisible()) {
                g_startMenuSessionActive = false;
                g_startMenuSessionWindow = nullptr;
                g_taskbarForegroundKeyboardActivated = false;
                g_keyboardTaskbarWindow = nullptr;
                CancelKeyboardTaskbarReleaseTimer();
                g_taskbarForegroundAfterShell = false;
            }
            g_lastForegroundWasShellSurface = foregroundIsShellSurface;
            const HMONITOR foregroundMonitor = hwnd
                    ? MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST)
                    : nullptr;
            const int fullscreenOwnerIndex = FindFullscreenOwnerIndex(foregroundMonitor);
            const bool hasFullscreenLocationHook =
                fullscreenOwnerIndex >= 0 &&
                g_fullscreenOwners[fullscreenOwnerIndex].hwnd == hwnd;
            InstallForegroundLocationHook(
                foregroundIsApplication && !hasFullscreenLocationHook ? hwnd : nullptr);
            if (g_workerMessageWindow && fullscreenOwnerIndex >= 0 &&
                g_fullscreenOwners[fullscreenOwnerIndex].hwnd == hwnd) {
                g_fullscreenValidationAttempt = 0;
                if (!SetTimer(g_workerMessageWindow, kFullscreenValidationTimerId, 16,
                              nullptr)) {
                    Wh_Log(L"Fullscreen validation timer could not be armed");
                }
            }
            const bool transitionAlreadyActive =
                IsForegroundTransitionHoldActive(g_foregroundTransitionMonitor);
            const bool isDesktopInfrastructure =
                hwnd && foregroundClassName[0] &&
                IsDesktopInfrastructureWindow(hwnd, foregroundClassName);
            if (foregroundIsApplication && hwnd && !minimizeTransitionActive) {
                g_foregroundTransitionWindow = hwnd;
                g_foregroundTransitionMonitor =
                    MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
                g_foregroundTransitionDeadline =
                    GetTickCount64() + kTaskbarIntegrityGuardMs;
                ArmTaskbarIntegrityGuard();
            } else if (isDesktopInfrastructure ||
                       !transitionAlreadyActive || minimizeTransitionActive) {
                g_foregroundTransitionWindow = nullptr;
                g_foregroundTransitionMonitor = nullptr;
                g_foregroundTransitionDeadline = 0;
            }
            g_windowTransitionValidationAttempt = 0;
            ArmWindowTransitionValidation();
            PostRefresh();
            return;
        }
        const bool wasShellForeground = g_lastForegroundWasShellSurface;
        g_lastForegroundWasShellSurface = false;
        CancelKeyboardTaskbarReleaseTimer();
        InstallForegroundLocationHook(nullptr);
        const bool mouseActivated = IsTaskbarMouseActivated(hwnd);
        const bool windowsKeyDown = !mouseActivated && IsWindowsKeyDown();
        if (g_minimizeInProgress ||
            (postMinimizeTaskbarForeground && isTaskbarForeground &&
             !windowsKeyDown)) {
            g_taskbarForegroundKeyboardActivated = false;
            g_taskbarForegroundAfterShell = false;
            PostRefresh();
            return;
        }
        if (windowsKeyDown) {
            ClearRecentlyDepartedApplication();
        }
        const bool automaticTaskbarFocus =
            !mouseActivated &&
            IsRecentAutomaticTaskbarFocus(hwnd, mouseActivated);
        if (automaticTaskbarFocus) {
            g_taskbarForegroundKeyboardActivated = false;
            g_keyboardTaskbarWindow = nullptr;
            g_taskbarForegroundAfterShell = false;
        } else {
            const bool keyboardActivated =
                g_taskbarForegroundKeyboardActivated && !mouseActivated;
            g_taskbarForegroundKeyboardActivated = keyboardActivated;
            if (keyboardActivated) {
                g_keyboardTaskbarWindow = hwnd;
            } else if (g_keyboardTaskbarWindow == hwnd) {
                g_keyboardTaskbarWindow = nullptr;
            }
            g_taskbarForegroundAfterShell =
                wasShellForeground && !mouseActivated && !keyboardActivated;
        }
        if (mouseActivated) {
            const HMONITOR taskbarMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            if (taskbarMonitor) {
                g_hoverActive = true;
                g_hoverMonitor = taskbarMonitor;
                g_hoverDeadline = 0;
                CancelHoverExpireTimer();
            }
        }
        g_windowTransitionValidationAttempt = 0;
        ArmWindowTransitionValidation();
        PostRefresh();
        return;
    }
    if (event == EVENT_SYSTEM_MINIMIZESTART) {
        if (hwnd && hwnd == g_foregroundTransitionWindow) {
            g_foregroundTransitionWindow = nullptr;
        }
        ArmTaskbarIntegrityGuard();
        g_minimizeInProgress = true;
        g_minimizingWindow = hwnd;
        g_lastMinimizeTransitionTick = GetTickCount64();
        CancelKeyboardTaskbarReleaseTimer();
        g_taskbarForegroundKeyboardActivated = false;
        g_keyboardTaskbarWindow = nullptr;
        g_hoverActive = false;
        g_hoverMonitor = nullptr;
        g_hoverDeadline = 0;
        CancelHoverExpireTimer();
        if (g_workerMessageWindow) {
            SetTimer(g_workerMessageWindow, kPostMinimizeReassertTimerId, 2000, nullptr);
        }
        PostRefresh();
        return;
    }
    if (event == EVENT_SYSTEM_MINIMIZEEND) {
        // MINIMIZEEND is emitted when the window is about to be restored. Keep
        // transition protection active until the restored foreground event arrives.
        ArmTaskbarIntegrityGuard();
        g_minimizeInProgress = hwnd != nullptr;
        g_minimizingWindow = hwnd;
        g_lastMinimizeTransitionTick = GetTickCount64();
        CancelKeyboardTaskbarReleaseTimer();
        g_keyboardTaskbarWindow = nullptr;
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
        if (g_workerMessageWindow) SetTimer( g_workerMessageWindow, kPostMinimizeReassertTimerId, 1200, nullptr );
        g_windowTransitionValidationAttempt = 0;
        ArmWindowTransitionValidation();
        PostRefresh();
        UpdateCursorHoverSnapshot();
        return;
    }
    if (event == EVENT_SYSTEM_MOVESIZESTART) {
        if (hwnd && hwnd == GetForegroundWindow()) {
            ArmTaskbarIntegrityGuard();
            g_foregroundTransitionWindow = hwnd;
            g_foregroundTransitionMonitor =
                MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            g_foregroundTransitionDeadline =
                GetTickCount64() + kTaskbarIntegrityGuardMs;
            PostRefresh();
        }
        return;
    }
    if (event == EVENT_SYSTEM_MOVESIZEEND) {
        if (hwnd && hwnd == GetForegroundWindow()) {
            ArmTaskbarIntegrityGuard();
        }
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
        g_lastMinimizeTransitionTick = 0;
        g_minimizingWindow = nullptr;
        g_minimizeInProgress = false;
        g_taskbarForegroundKeyboardActivated = false;
        g_keyboardTaskbarWindow = nullptr;
        UpdateTaskbarState();
        return 0;
    }
    if (message == WM_TIMER && wParam == kWindowsAnimationsCloseProbeTimerId) {
        const bool wasActive = g_windowsAnimationsCloseActive;
        const bool closeActive = RefreshWindowsAnimationsCloseGuard();
        if (closeActive != wasActive) {
            UpdateTaskbarState();
        }
        const ULONGLONG now = GetTickCount64();
        const bool closeGuardGraceActive =
            g_windowsAnimationsCloseActive &&
            g_windowsAnimationsCloseLastSeenTick != 0 &&
            now - g_windowsAnimationsCloseLastSeenTick <
                kWindowsAnimationsCloseGuardGraceMs;
        const bool keepProbeRunning =
            closeGuardGraceActive ||
            (g_windowsAnimationsCloseProbeDeadline != 0 &&
             now < g_windowsAnimationsCloseProbeDeadline);
        if (!keepProbeRunning) {
            CancelWindowsAnimationsCloseProbeTimer();
        }
        return 0;
    }
    if (message == WM_TIMER && wParam == kTaskbarIntegrityTimerId) {
        const ULONGLONG now = GetTickCount64();
        if (g_taskbarIntegrityDeadline == 0 ||
            now >= g_taskbarIntegrityDeadline) {
            CancelTaskbarIntegrityGuard();
            return 0;
        }
        UpdateTaskbarIntegritySnapshot();
        if (g_taskbarIntegrityProtectedCount == 0) {
            CancelTaskbarIntegrityGuard();
            return 0;
        }
        return 0;
    }
    if (message == WM_TIMER && wParam == kWindowTransitionValidationTimerId) {
        KillTimer(hwnd, kWindowTransitionValidationTimerId);
        UpdateTaskbarState();
        if (IsWindowTransitionValidationActive()) {
            ArmWindowTransitionValidation();
        } else {
            g_windowTransitionValidationAttempt = 0;
        }
        return 0;
    }
    if (message == WM_TIMER && wParam == kKeyboardTaskbarReleaseTimerId) {
        KillTimer(hwnd, kKeyboardTaskbarReleaseTimerId);
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
        if (!foregroundIsTaskbar) {
            g_taskbarForegroundKeyboardActivated = false;
            g_keyboardTaskbarWindow = nullptr;
            PostRefresh();
        }
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
            index >= 0 && g_fullscreenOwners[index].hwnd == foreground &&
            IsFullscreenOwnerActive(monitor);
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
            InstallShellSurfaceHook();
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

void SeedForegroundApplicationHistory() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) return;
    WCHAR className[256] = {};
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) == 0) return;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    ShellProcessKindCache processCache = {};
    const ShellProcessKind processKind =
        GetShellProcessKindCached(processCache, pid);
    const bool shellSurface =
        IsShellSurfaceWindow(hwnd, className, processKind);
    if (shellSurface || IsDesktopInfrastructureWindow(hwnd, className) ||
        IsShellChromeClass(className) || IsTaskbarPopupClass(className) ||
        IsTaskbarWindow(hwnd)) {
        return;
    }
    if (IsApplicationWindowCandidate(hwnd, className, processKind, true)) {
        g_lastForegroundApplicationWindow = hwnd;
        g_lastForegroundApplicationMonitor =
            MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        InstallForegroundLocationHook(hwnd);
    }
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
        EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND, nullptr,
        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!g_moveHook) Wh_Log(L"Failed to install move/size WinEvent hook");
    InstallShellSurfaceHook();
    InstallTaskbarFocusHook();
    InstallWindowDestroyHook();
    SeedForegroundApplicationHistory();
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
            if (msg.wParam == timerId) {
                RefreshNativeAutoHideState();
                UpdateTaskbarState();
            }
            continue;
        }
        if (msg.message == WM_APP_REFRESH) {
            InterlockedExchange(&g_refreshPosted, 0);
            UpdateTaskbarState();
            continue;
        }
        if (msg.message == WM_APP_SETTINGS) {
            UpdateTaskbarState();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    if (timerId) KillTimer(nullptr, timerId);
    if (g_workerMessageWindow) {
        KillTimer(g_workerMessageWindow, kFullscreenValidationTimerId);
        KillTimer(g_workerMessageWindow, kWindowTransitionValidationTimerId);
        KillTimer(g_workerMessageWindow, kTaskbarIntegrityTimerId);
        KillTimer(g_workerMessageWindow, kWindowsAnimationsCloseProbeTimerId);
        KillTimer(g_workerMessageWindow, kKeyboardTaskbarReleaseTimerId);
    }
    CancelTaskbarIntegrityGuard();
    g_foregroundTransitionWindow = nullptr;
    g_foregroundTransitionMonitor = nullptr;
    g_foregroundTransitionDeadline = 0;
    g_windowsAnimationsCloseActive = false;
    g_windowsAnimationsCloseMonitorCount = 0;
    g_windowsAnimationsCloseLastSeenTick = 0;
    g_windowsAnimationsCloseProbeDeadline = 0;
    for (size_t i = 0; i < kMaxTaskbars; ++i) {
        g_windowsAnimationsCloseMonitors[i] = nullptr;
    }
    g_lastForegroundApplicationWindow = nullptr;
    g_lastForegroundApplicationMonitor = nullptr;
    ClearRecentlyDepartedApplication();
    g_previousForegroundApplicationWindow = nullptr;
    g_previousForegroundApplicationMonitor = nullptr;
    g_fullscreenValidationAttempt = 0;
    g_windowTransitionValidationAttempt = 0;
    g_keyboardTaskbarWindow = nullptr;
    g_taskbarForegroundKeyboardActivated = false;
    g_startMenuSessionActive = false;
    g_startMenuSessionWindow = nullptr;
    g_minimizingWindow = nullptr;
    g_minimizeInProgress = false;
    CancelHoverExpireTimer();
    SafeUnhookWinEvent(g_foregroundHook);
    SafeUnhookWinEvent(g_minimizeHook);
    SafeUnhookWinEvent(g_moveHook);
    for (size_t i = 0; i < kMaxMonitorNumbers; ++i) SafeUnhookWinEvent(g_fullscreenLocationHooks[i]);
    SafeUnhookWinEvent(g_shellSurfaceHook);
    SafeUnhookWinEvent(g_shellSurfaceCloakHook);
    SafeUnhookWinEvent(g_taskbarFocusHook);
    SafeUnhookWinEvent(g_windowDestroyHook);
    SafeUnhookWinEvent(g_foregroundLocationHook);
    g_foregroundLocationWindow = nullptr;
    DestroyWorkerMessageWindow();
    return 0;
}

void ApplyPendingSettings() {
    while (InterlockedExchange(&g_settingsReloadPending, 0) != 0) {
        LoadSettings();
        RefreshNativeAutoHideState();
    }
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
        if (!*display && !*interfaceName) continue;
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
        if (!*value) continue;
        if (wcscmp(value, L"all") == 0) {
            g_settings.hideAllMonitors = true;
        } else {
            const int number = ParseMonitorNumber(value);
            if (number > 0) g_settings.hideMonitor[number] = true;
        }
    }
    for ( size_t i = 0; i < kMaxMonitorNumbers; ++i ) {
        auto value = WindhawkUtils::StringSetting::make(L"hoverRevealOnMonitors[%d]", static_cast<int>(i));
        if (!*value) continue;
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
    InterlockedExchange(&g_settingsReloadPending, 1);
    if (g_workerThread &&
        !PostThreadMessageW(g_workerThreadId, WM_APP_SETTINGS, 0, 0)) {
        Wh_Log(L"Failed to post settings message to worker thread; "
               L"settings reload remains pending");
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
        Wh_Log(L"WaitForSingleObject failed for %s thread: %lu", threadName, GetLastError());
    }
    return false;
}

void WhTool_ModUninit() {
    if (g_cursorStopEvent) SetEvent(g_cursorStopEvent);
    if (g_cursorThread) {
        if (!WaitForThreadWithTimeout( g_cursorThread, 3000, L"cursor sampler" )) {
            EnumWindows(RestoreMarkedTaskbarProc, 0);
            ExitProcess(1);
        }
        SafeCloseHandle(g_cursorThread);
    }
    if (g_workerThread) {
        if (!PostThreadMessageW( g_workerThreadId, WM_QUIT, 0, 0 )) Wh_Log(L"Failed to post worker shutdown message");
        if (!WaitForThreadWithTimeout( g_workerThread, 5000, L"worker" )) {
            EnumWindows(RestoreMarkedTaskbarProc, 0);
            ExitProcess(1);
        }
        SafeCloseHandle(g_workerThread);
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

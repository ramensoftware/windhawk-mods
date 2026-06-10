// ==WindhawkMod==
// @id              taskbar-extrass-scroll-actions
// @name            Taskbar Extras Scroll Actions
// @description     Assign actions for scrolling over the taskbar: virtual desktops, brightness (with HDR/SDR support), microphone volume, and system volume
// @version         2.1
// @author          Nairodorian (merged with mods by m417z)
// @github          https://github.com/NairoDorian
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// @include         SndVol.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -ldwmapi -ldxgi -ldxva2 -lgdi32 -lole32 -loleaut32 -lversion -lwbemuuid -lmsimg32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// This mod merges:
//   - taskbar-extra-scroll-actions v1.1.1 (Nairodorian): SDR/HDR brightness
//   - taskbar-scroll-actions v1.2 (m417z): per-entry array config, DDC/CI
//     brightness, microphone volume, additional scroll regions
//   - taskbar-volume-control v1.3.1 (m417z): system volume with all volume
//     indicators (None/Classic/Modern/Win11), scroll anywhere with modifier
//     keys, full screen scrolling, middle-click mute, no-auto-mute toggle

// ==WindhawkModReadme==
/*
# Taskbar Extra Scroll Actions (All-In-One)

Assign multiple scroll actions to different regions of the taskbar.

## Supported actions

* **Switch virtual desktop** - keyboard-shortcut driven, preserves animation
* **Change monitor brightness** - four-layer fallback: IOCTL direct backlight
  (bypasses WDDM smoothing), DDC/CI for external monitors, WMI for laptop
  internal displays, PowerScheme last-resort fallback
* **Software dimming overlay** - goes BELOW hardware minimum brightness.
  On OLED laptops this means deeper blacks, less burn-in, and longer battery
  life. On IPS/VA panels it kills the eye-searing glow when even 0% is
  still too bright at night.
* **Change SDR brightness on HDR displays** - adjusts the SDR boost level via
  the undocumented `DwmpSDRToHDRBoost` API
* **Master brightness control** - automatically picks SDR-on-HDR for HDR
  monitors, regular brightness otherwise (per-monitor, with dim support)
* **Change microphone volume**
* **Change system volume** - with optional Windows 11/10 modern indicator,
  classic SndVol indicator, or no indicator

## Multiple entries

You can configure several entries, each with its own scroll action, scroll
area (entire taskbar / notification area / taskbar without notification area /
custom regions / none), step size, throttle, direction, and **per-entry scroll-
anywhere modifier keys**. The first entry that matches the cursor position
(or the modifier-key combo, when scrolling outside the taskbar) wins.

## Per-entry scroll-anywhere modifier keys

Each entry exposes its own Shift/Ctrl/Alt/Win checkboxes. When all of the
checked modifiers (and only those) are held while scrolling anywhere on
screen, that entry's action runs - regardless of cursor position. This makes
brightness, virtual-desktop switching, microphone volume, etc. all available
as global hotkey-like shortcuts, not just system volume.

## Full-screen scrolling

When the taskbar is auto-hidden, scrolling in the strip where the taskbar
*would* be now dispatches to the matching entry based on the entry's scroll
area - the zones you defined on the visible taskbar are preserved. For
example, with volume in the notification area and brightness in the rest of
the taskbar, full-screen scrolling at the bottom-right adjusts volume while
the bottom-left/center adjusts brightness.

## Visual indicators

In addition to the existing volume indicators, the mod now renders a custom
OSD overlay for brightness, SDR-on-HDR brightness, and (as a fallback)
microphone volume. The overlay matches the placement and timing of the
native Win11 volume flyout. It can be disabled with `customOsdEnabled: false`
if you prefer silent adjustments.

For microphone volume, the mod also tries the native Win11 OSD via the
APPCOMMAND_MICROPHONE_VOLUME_* messages first, falling back to the custom
overlay only when the native indicator is unavailable.

## Extra system-volume features

When a `volumeChange` entry is configured, these global settings also apply:

* Volume indicator: None, Classic, Modern, or Win11 native
* Middle-click the volume tray icon to mute (Win11)
* Ctrl+scroll-inside-taskbar to change volume (gate)
* Disable Windows' automatic mute when volume reaches 0
* Custom volume step

## HDR / SDR brightness

Range is 1.0 to your configured max (default 6.0). Each scroll click changes by
0.05. The last value can be persisted across sessions. This affects the same
slider as Windows Settings > System > Display > HDR > SDR brightness balance.

## Software dimming overlay

When enabled, scrolling brightness below 0% smoothly applies a black overlay.
The overlay alpha animates at ~60 FPS for a butter-smooth transition. The
maximum opacity is configurable (default 230/255 — almost black but still
showing the screen faintly). On OLED panels this physically reduces pixel
emission for true deeper blacks, lower burn-in risk, and extended battery life.

## Notes

Some touchpads do not send scroll events over the taskbar; the pinch-to-zoom
gesture often works as a fallback.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ScrollActions:
  - - scrollAction: virtualDesktopSwitch
      $name: Scroll action
      $description: The action to perform when scrolling over this region
      $options:
      - virtualDesktopSwitch: Switch virtual desktop
      - brightnessChange: Change monitor brightness
      - sdrToHdrBrightnessChange: Change SDR brightness on HDR displays
      - masterBrightnessChange: Master brightness (auto SDR/HDR vs monitor)
      - micVolumeChange: Change microphone volume
      - volumeChange: Change system volume
    - scrollArea: taskbar
      $name: Scroll area
      $description: The taskbar region on which scrolling triggers this action
      $options:
      - taskbar: The entire taskbar
      - notificationArea: The notification area (system tray and clock)
      - taskbarWithoutNotificationArea: The taskbar without the notification area
      - none: None (use only additional scroll regions and/or modifier keys below)
    - additionalScrollRegions: ""
      $name: Additional scroll regions
      $description: >-
        Comma-separated list of regions. Each region is "start-end" measured
        from the taskbar's leading edge, in pixels or percent (e.g.
        "100-200, 25%-50%").
    - scrollStep: 1
      $name: Scroll step
      $description: How many steps to change per scroll click
    - throttleMs: 0
      $name: Throttle (ms)
      $description: >-
        Minimum milliseconds between actions. 0 disables throttling. When
        throttled, fast scrolls collapse to a single step.
    - reverseScrollingDirection: false
      $name: Reverse scrolling direction
      $description: Reverse the direction of the scrolling action
    - scrollAnywhereKeys:
      - shift: false
        $name: Shift
      - ctrl: false
        $name: Ctrl
      - alt: false
        $name: Alt
      - win: false
        $name: Win
      $name: Scroll-anywhere modifier keys
      $description: >-
        When all (and only) the checked modifiers are held, this action is
        triggered by scrolling anywhere on screen - not just over the
        taskbar. Leave all unchecked to disable scroll-anywhere for this
        entry. Each entry can have its own combo; the first entry whose
        combo matches wins.
  $name: Scroll actions
  $description: >-
    Configure one or more scroll actions. The first matching entry wins.
- storeCurrentSdrToHdrBrightness: true
  $name: Remember SDR to HDR brightness
  $description: Remember the last SDR-to-HDR brightness value between sessions
- maxSdrBrightness: 6
  $name: Maximum SDR brightness
  $description: >-
    Maximum SDR brightness on HDR displays. Integer 1-10. Default 6.
- customOsdEnabled: true
  $name: Show custom OSD for brightness and mic
  $description: >-
    Show a small overlay indicator when changing brightness, SDR-to-HDR
    brightness, or microphone volume (used as a fallback when no native
    indicator is available).
- volumeIndicator: win11
  $name: Volume indicator
  $description: Which volume indicator to show when changing system volume
  $options:
  - win11: Windows 11 native indicator (Win11 22H2+)
  - modern: Modern (Win10-style) indicator
  - classic: Classic SndVol indicator
  - none: No indicator
- middleClickToMute: false
  $name: Middle-click volume icon to mute
  $description: Middle-click the volume tray icon to toggle mute (Win11 22H2+)
- ctrlScrollVolumeChange: false
  $name: Volume change requires Ctrl
  $description: When enabled, system-volume entries only trigger while Ctrl is held
- noAutomaticMuteToggle: false
  $name: Disable automatic mute at 0%
  $description: Prevent Windows from auto-muting when volume reaches 0
- volumeChangeStep: 2
  $name: Volume change step (percent)
  $description: Percent change per system-volume scroll click (default 2)
- fullScreenScrolling: disabled
  $name: Full-screen scrolling
  $description: >-
    Scroll in the strip where the taskbar would be (even when auto-hidden) to
    trigger the matching entry's action. The entry is selected based on the
    cursor's horizontal position within the (hidden) taskbar, using the same
    zones you configured for normal taskbar scrolling.
  $options:
  - disabled: Disabled
  - withIndicator: Enabled, show indicator
  - withoutIndicator: Enabled, no indicator
- softwareDimmingEnabled: true
  $name: Software dimming overlay
  $description: >-
    Adds a black overlay that goes BELOW hardware minimum brightness.
    On OLED laptops this means deeper blacks, less burn-in, and longer
    battery life. On IPS/VA panels this reduces eye strain at night
    when even 0% is still too bright.
- maxOverlayAlpha: 230
  $name: Max overlay darkness (0-255)
  $description: >-
    How dark the overlay can go. 200 = very dark but still visible.
    230 = almost black (default). 255 = completely black screen.
    For OLED use 240-255, for IPS 200-230 is usually enough.
- oldTaskbarOnWin11: false
  $name: Old taskbar on Windows 11
  $description: Enable if you're using the old Windows 10 taskbar on Windows 11 (Explorer Patcher)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <initguid.h>

#include <combaseapi.h>
#include <commctrl.h>
#include <comutil.h>
#include <dwmapi.h>
#include <dxgi1_6.h>
#include <endpointvolume.h>
#include <highlevelmonitorconfigurationapi.h>
#include <mmdeviceapi.h>
#include <physicalmonitorenumerationapi.h>
#include <psapi.h>
#include <wbemcli.h>
#include <windowsx.h>

#include <algorithm>
#include <atomic>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// IOCTL: direct backlight control via \\.\LCD (bypasses WDDM smoothing)
#pragma pack(push, 1)
struct DISPLAY_BRIGHTNESS_IOCTL {
    BYTE ucDisplayPolicy;
    BYTE ucACBrightness;
    BYTE ucDCBrightness;
};
#pragma pack(pop)
#define IOCTL_VIDEO_QUERY_DISPLAY_BRIGHTNESS_VAL 0x00230498UL
#define IOCTL_VIDEO_SET_DISPLAY_BRIGHTNESS_VAL   0x0023049CUL

// -----------------------------------------------------------------------------
// Enums and structs
// -----------------------------------------------------------------------------

enum class ScrollAction {
    virtualDesktopSwitch,
    brightnessChange,
    sdrToHdrBrightnessChange,
    masterBrightnessChange,
    micVolumeChange,
    volumeChange,
};

enum class ScrollArea {
    taskbar,
    notificationArea,
    taskbarWithoutNotificationArea,
    none,
};

enum class VolumeIndicator {
    None,
    Classic,
    Modern,
    Win11,
};

enum class FullScreenScrolling {
    disabled,
    withIndicator,
    withoutIndicator,
};

struct Region {
    bool isPercentage;
    int start;
    int end;
};

// A single Shift/Ctrl/Alt/Win combination. Used both per-entry (for scroll-
// anywhere) and as a small helper struct elsewhere if needed.
struct ModifierKeys {
    bool shift;
    bool ctrl;
    bool alt;
    bool win;

    // True if at least one modifier is set.
    bool Any() const { return shift || ctrl || alt || win; }
};

struct ScrollActionEntry {
    ScrollAction scrollAction;
    ScrollArea scrollArea;
    std::vector<Region> additionalScrollRegions;
    int scrollStep;
    int throttleMs;
    bool reverseScrollingDirection;
    // Per-entry scroll-anywhere modifier keys. When all the checked
    // modifiers (and only those) are held while scrolling anywhere, this
    // entry is triggered regardless of cursor position. If none of the
    // four are checked, scroll-anywhere is disabled for this entry.
    ModifierKeys scrollAnywhereKeys;
};

struct {
    std::vector<ScrollActionEntry> scrollActions;
    // SDR/HDR brightness
    bool storeCurrentSdrToHdrBrightness;
    double maxSdrBrightness;
    // Custom OSD for brightness, SDR-to-HDR brightness, and mic (fallback).
    bool customOsdEnabled;
    // System volume
    VolumeIndicator volumeIndicator;
    bool middleClickToMute;
    bool ctrlScrollVolumeChange;
    bool noAutomaticMuteToggle;
    int volumeChangeStep;
    FullScreenScrolling fullScreenScrolling;
    bool oldTaskbarOnWin11;
    // Software dimming overlay
    bool softwareDimmingEnabled;
    int maxOverlayAlpha;
} g_settings;

// -----------------------------------------------------------------------------
// Target / globals
// -----------------------------------------------------------------------------

enum class Target {
    Explorer,
    ShellExperienceHost,
    SndVol,
};

Target g_target = Target::Explorer;

std::atomic<bool> g_systemTrayModuleHooked;
std::atomic<bool> g_initialized;
bool g_inputSiteProcHooked;
std::unordered_set<HWND> g_secondaryTaskbarWindows;

UINT g_scrollAnywhereMsg =
    RegisterWindowMessage(L"Windhawk_ScrollAnywhere_" WH_MOD_ID);
// Custom message used by the low-level mouse hook to route a scroll event
// (with a specific entry index) to the taskbar thread for dispatch.
//   wParam: HIWORD = wheel delta (signed),
//           LOWORD = (entryIndex << 1) | suppressIndicator
//   lParam: MAKELPARAM(cursor.x, cursor.y)
UINT g_dispatchEntryMsg =
    RegisterWindowMessage(L"Windhawk_DispatchEntry_" WH_MOD_ID);
HANDLE g_scrollAnywhereThread;

// Tunables / magic-number constants pulled out for clarity.
namespace Constants {
    // Monitor HDR state is cached for this duration (Windows can change HDR
    // state at any time so we don't want to assume forever).
    constexpr DWORD kHdrCacheTtlMs = 5000;
    // After this long with no scroll, the wheel-delta remainder resets.
    constexpr DWORD kWheelAccumulationWindowMs = 5000;
    // Threshold below which auto-mute kicks in (in 0..1 scalar volume).
    constexpr float kAutoMuteThreshold = 0.005f;
    // Step for the Win11 indicator's manual top-up before the AppCommand.
    constexpr float kWin11ManualStep = 0.02f;
    // WMI brightness readback is slow to update; cache the last known value
    // for this long before re-querying.
    constexpr DWORD kWmiBrightnessCacheTtlMs = 3000;
    // SDR-to-HDR brightness range and step.
    constexpr double kSdrMinBrightness = 1.0001;
    constexpr double kSdrStep = 0.05;
    // Maximum number of monitors to enumerate in HDR detection (safety).
    constexpr UINT kMaxMonitorsToProbe = 64;
}

enum {
    WIN_VERSION_UNSUPPORTED = 0,
    WIN_VERSION_7,
    WIN_VERSION_8,
    WIN_VERSION_81,
    WIN_VERSION_811,
    WIN_VERSION_10_T1,
    WIN_VERSION_10_T2,
    WIN_VERSION_10_R1,
    WIN_VERSION_10_R2,
    WIN_VERSION_10_R3,
    WIN_VERSION_10_R4,
    WIN_VERSION_10_R5,
    WIN_VERSION_10_19H1,
    WIN_VERSION_10_20H1,
    WIN_VERSION_SERVER_2022,
    WIN_VERSION_11_21H2,
    WIN_VERSION_11_22H2,
};

#ifndef WM_POINTERWHEEL
#define WM_POINTERWHEEL 0x024E
#endif

#if defined(__GNUC__) && __GNUC__ > 8
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t WINAPI
#elif defined(__GNUC__)
#define WINAPI_LAMBDA_RETURN(return_t) WINAPI->return_t
#else
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t
#endif

int g_nWinVersion;
int g_nExplorerVersion;
HWND g_hTaskbarWnd;
DWORD g_dwTaskbarThreadId;

// SDR/HDR brightness state
typedef void(WINAPI* DwmpSDRToHDRBoostPtr_t)(HMONITOR hMonitor,
                                             double brightness);
typedef HRESULT(WINAPI* PFN_CreateDXGIFactory1)(REFIID riid, void** ppFactory);

HMODULE g_hDwmApiDll = nullptr;
DwmpSDRToHDRBoostPtr_t g_pDwmpSDRToHDRBoost = nullptr;
double g_currentSdrToHdrBrightness = 1.0;

// WMI brightness cache to avoid stale reads during rapid scrolling.
int g_cachedWmiBrightness = -1;
DWORD g_lastWmiBrightnessTime = 0;

struct MonitorState {
    bool isHdr;
    DWORD lastCheckTime;
};
std::unordered_map<HMONITOR, MonitorState> g_monitorHdrCache;

struct SDRBrightnessCallbackData {
    DwmpSDRToHDRBoostPtr_t pFunc;
    double brightness;
};

// IOCTL backlight handle (direct backlight, bypasses WDDM smoothing)
HANDLE g_hLCD = INVALID_HANDLE_VALUE;

// PowerScheme last-resort fallback types and globals
typedef DWORD(WINAPI* PowerGetActiveScheme_t)(HKEY, GUID**);
typedef DWORD(WINAPI* PowerReadACValueIndex_t)(HKEY, const GUID*, const GUID*, const GUID*, LPDWORD);
typedef DWORD(WINAPI* PowerWriteACValueIndex_t)(HKEY, const GUID*, const GUID*, const GUID*, DWORD);
typedef DWORD(WINAPI* PowerWriteDCValueIndex_t)(HKEY, const GUID*, const GUID*, const GUID*, DWORD);
typedef DWORD(WINAPI* PowerSetActiveScheme_t)(HKEY, const GUID*);
static HMODULE g_hPowrProf;
static PowerGetActiveScheme_t pPowerGetActiveScheme;
static PowerReadACValueIndex_t pPowerReadACValueIndex;
static PowerWriteACValueIndex_t pPowerWriteACValueIndex;
static PowerWriteDCValueIndex_t pPowerWriteDCValueIndex;
static PowerSetActiveScheme_t pPowerSetActiveScheme;
static const GUID VID_BRIGHTNESS_SUB = {0x7516b95f,0xf776,0x4464,
    {0x8c,0x53,0x06,0x16,0x7f,0x40,0xcc,0x99}};
static const GUID VID_BRIGHTNESS_VAL = {0xaded5e82,0xb909,0x4619,
    {0x99,0x49,0xf5,0xd7,0x1d,0xac,0x0b,0xcb}};
static DWORD g_lastPowerWriteMs;

// Timestamps for brightness write/input tracking (anti-jerk poll cooldown).
DWORD g_lastBrightnessInputMs;
DWORD g_lastBrightnessWriteMs;

// -----------------------------------------------------------------------------
// Utility functions
// -----------------------------------------------------------------------------

UINT GetDpiForWindowWithFallback(HWND hWnd) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND hwnd);
    static GetDpiForWindow_t pGetDpiForWindow = []() {
        HMODULE hUser32 = GetModuleHandle(L"user32.dll");
        if (hUser32) {
            return (GetDpiForWindow_t)GetProcAddress(hUser32,
                                                     "GetDpiForWindow");
        }
        return (GetDpiForWindow_t) nullptr;
    }();

    int iDpi = 96;
    if (pGetDpiForWindow) {
        iDpi = pGetDpiForWindow(hWnd);
    } else {
        HDC hdc = GetDC(NULL);
        if (hdc) {
            iDpi = GetDeviceCaps(hdc, LOGPIXELSX);
            ReleaseDC(NULL, hdc);
        }
    }
    return iDpi;
}

bool IsTaskbarWindow(HWND hWnd) {
    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return false;
    }
    return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI_LAMBDA_RETURN(BOOL) {
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

bool GetNotificationAreaRect(HWND hMMTaskbarWnd, RECT* rcResult) {
    if (hMMTaskbarWnd == g_hTaskbarWnd) {
        HWND hTrayNotifyWnd =
            FindWindowEx(hMMTaskbarWnd, NULL, L"TrayNotifyWnd", NULL);
        if (hTrayNotifyWnd && GetWindowRect(hTrayNotifyWnd, rcResult) &&
            !IsRectEmpty(rcResult)) {
            return true;
        }
    } else if (g_nExplorerVersion >= WIN_VERSION_11_21H2) {
        RECT rcTaskbar;
        if (GetWindowRect(hMMTaskbarWnd, &rcTaskbar)) {
            HWND hBridgeWnd = FindWindowEx(
                hMMTaskbarWnd, NULL,
                L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
            while (hBridgeWnd) {
                RECT rcBridge;
                if (!GetWindowRect(hBridgeWnd, &rcBridge)) {
                    break;
                }
                if (!EqualRect(&rcBridge, &rcTaskbar)) {
                    if (IsRectEmpty(&rcBridge)) {
                        break;
                    }
                    CopyRect(rcResult, &rcBridge);
                    return true;
                }
                hBridgeWnd = FindWindowEx(
                    hMMTaskbarWnd, hBridgeWnd,
                    L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
            }
        }
    } else if (g_nExplorerVersion >= WIN_VERSION_10_R1) {
        HWND hClockButtonWnd =
            FindWindowEx(hMMTaskbarWnd, NULL, L"ClockButton", NULL);
        if (hClockButtonWnd && GetWindowRect(hClockButtonWnd, rcResult) &&
            !IsRectEmpty(rcResult)) {
            return true;
        }
    } else {
        SetRectEmpty(rcResult);
        return true;
    }

    RECT rcTaskbar;
    if (!GetWindowRect(hMMTaskbarWnd, &rcTaskbar)) {
        return false;
    }

    int lastPixels = MulDiv(50, GetDpiForWindowWithFallback(hMMTaskbarWnd), 96);
    CopyRect(rcResult, &rcTaskbar);
    if (rcResult->right - rcResult->left > lastPixels) {
        if (GetWindowLong(hMMTaskbarWnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) {
            rcResult->right = rcResult->left + lastPixels;
        } else {
            rcResult->left = rcResult->right - lastPixels;
        }
    }
    return true;
}

bool IsPointInsideTaskbarScrollArea(HWND hMMTaskbarWnd,
                                    POINT pt,
                                    ScrollArea scrollArea) {
    switch (scrollArea) {
        case ScrollArea::taskbar: {
            RECT rc;
            return GetWindowRect(hMMTaskbarWnd, &rc) && PtInRect(&rc, pt);
        }
        case ScrollArea::notificationArea: {
            RECT rc;
            return GetNotificationAreaRect(hMMTaskbarWnd, &rc) &&
                   PtInRect(&rc, pt);
        }
        case ScrollArea::taskbarWithoutNotificationArea: {
            RECT rc;
            return GetWindowRect(hMMTaskbarWnd, &rc) && PtInRect(&rc, pt) &&
                   (!GetNotificationAreaRect(hMMTaskbarWnd, &rc) ||
                    !PtInRect(&rc, pt));
        }
        case ScrollArea::none:
            return false;
    }
    return false;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    HRSRC hResource;
    HGLOBAL hGlobal;
    void* pData;
    void* pFixedFileInfo = NULL;
    UINT uPtrLen = 0;

    hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = NULL;
                    uPtrLen = 0;
                }
            }
        }
    }
    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }
    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

BOOL WindowsVersionInit() {
    g_nWinVersion = WIN_VERSION_UNSUPPORTED;
    VS_FIXEDFILEINFO* pFixedFileInfo = GetModuleVersionInfo(NULL, NULL);
    if (!pFixedFileInfo) {
        return FALSE;
    }

    WORD nMajor = HIWORD(pFixedFileInfo->dwFileVersionMS);
    WORD nMinor = LOWORD(pFixedFileInfo->dwFileVersionMS);
    WORD nBuild = HIWORD(pFixedFileInfo->dwFileVersionLS);
    WORD nQFE = LOWORD(pFixedFileInfo->dwFileVersionLS);

    switch (nMajor) {
        case 6:
            switch (nMinor) {
                case 1: g_nWinVersion = WIN_VERSION_7; break;
                case 2: g_nWinVersion = WIN_VERSION_8; break;
                case 3:
                    g_nWinVersion = (nQFE < 17000) ? WIN_VERSION_81
                                                   : WIN_VERSION_811;
                    break;
                case 4: g_nWinVersion = WIN_VERSION_10_T1; break;
            }
            break;
        case 10:
            if (nBuild <= 10240) g_nWinVersion = WIN_VERSION_10_T1;
            else if (nBuild <= 10586) g_nWinVersion = WIN_VERSION_10_T2;
            else if (nBuild <= 14393) g_nWinVersion = WIN_VERSION_10_R1;
            else if (nBuild <= 15063) g_nWinVersion = WIN_VERSION_10_R2;
            else if (nBuild <= 16299) g_nWinVersion = WIN_VERSION_10_R3;
            else if (nBuild <= 17134) g_nWinVersion = WIN_VERSION_10_R4;
            else if (nBuild <= 17763) g_nWinVersion = WIN_VERSION_10_R5;
            else if (nBuild <= 18362) g_nWinVersion = WIN_VERSION_10_19H1;
            else if (nBuild <= 19041) g_nWinVersion = WIN_VERSION_10_20H1;
            else if (nBuild <= 20348) g_nWinVersion = WIN_VERSION_SERVER_2022;
            else if (nBuild <= 22000) g_nWinVersion = WIN_VERSION_11_21H2;
            else g_nWinVersion = WIN_VERSION_11_22H2;
            break;
    }
    return g_nWinVersion != WIN_VERSION_UNSUPPORTED;
}

std::wstring_view TrimStringView(std::wstring_view s) {
    s.remove_prefix(std::min(s.find_first_not_of(L" \t\r\v\n"), s.size()));
    s.remove_suffix(
        std::min(s.size() - s.find_last_not_of(L" \t\r\v\n") - 1, s.size()));
    return s;
}

std::vector<std::wstring_view> SplitStringView(std::wstring_view s,
                                               std::wstring_view delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::wstring_view token;
    std::vector<std::wstring_view> res;
    while ((pos_end = s.find(delimiter, pos_start)) !=
           std::wstring_view::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }
    res.push_back(s.substr(pos_start));
    return res;
}

bool SvToInt(std::wstring_view s, int* result) {
    if (s.empty()) {
        return false;
    }
    int value = 0;
    for (WCHAR c : s) {
        if (c < L'0' || c > L'9') {
            return false;
        }
        int digit = c - L'0';
        if (value > (INT_MAX - digit) / 10) {
            return false;
        }
        value = value * 10 + digit;
    }
    *result = value;
    return true;
}

std::optional<Region> ParseRegion(std::wstring_view regionStr) {
    auto parts = SplitStringView(regionStr, L"-");
    if (parts.size() != 2) {
        return std::nullopt;
    }
    auto startStr = TrimStringView(parts[0]);
    auto endStr = TrimStringView(parts[1]);

    bool startIsPercentage = !startStr.empty() && startStr.back() == L'%';
    bool endIsPercentage = !endStr.empty() && endStr.back() == L'%';
    if (startIsPercentage != endIsPercentage) {
        return std::nullopt;
    }

    bool isPercentage = startIsPercentage;
    if (isPercentage) {
        startStr.remove_suffix(1);
        endStr.remove_suffix(1);
    }

    int start;
    int end;
    if (!SvToInt(startStr, &start) || !SvToInt(endStr, &end) || start >= end) {
        return std::nullopt;
    }
    return Region{isPercentage, start, end};
}

bool IsPointInsideAdditionalRegion(HWND hMMTaskbarWnd,
                                   POINT pt,
                                   const std::vector<Region>& regions) {
    if (regions.empty()) {
        return false;
    }
    RECT rc;
    if (!GetWindowRect(hMMTaskbarWnd, &rc) || !PtInRect(&rc, pt)) {
        return false;
    }

    bool isHorizontal = (rc.right - rc.left) >= (rc.bottom - rc.top);
    int taskbarLength;
    int cursorOffset;
    if (isHorizontal) {
        taskbarLength = rc.right - rc.left;
        if (GetWindowLong(hMMTaskbarWnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) {
            cursorOffset = rc.right - pt.x;
        } else {
            cursorOffset = pt.x - rc.left;
        }
    } else {
        taskbarLength = rc.bottom - rc.top;
        cursorOffset = pt.y - rc.top;
    }

    UINT dpi = GetDpiForWindowWithFallback(hMMTaskbarWnd);
    for (const auto& region : regions) {
        int start, end;
        if (region.isPercentage) {
            start = MulDiv(taskbarLength, region.start, 100);
            end = MulDiv(taskbarLength, region.end, 100);
        } else {
            start = MulDiv(region.start, dpi, 96);
            end = MulDiv(region.end, dpi, 96);
        }
        if (cursorOffset >= start && cursorOffset <= end) {
            return true;
        }
    }
    return false;
}

bool IsPointInsideEntryScrollArea(HWND hMMTaskbarWnd,
                                  POINT pt,
                                  const ScrollActionEntry& entry) {
    return IsPointInsideTaskbarScrollArea(hMMTaskbarWnd, pt,
                                          entry.scrollArea) ||
           IsPointInsideAdditionalRegion(hMMTaskbarWnd, pt,
                                         entry.additionalScrollRegions);
}

// Used for the global "scroll anywhere" check (covers any entry's scroll area)
bool IsPointInsideAnyScrollArea(HWND hMMTaskbarWnd, POINT pt) {
    for (const auto& entry : g_settings.scrollActions) {
        if (IsPointInsideEntryScrollArea(hMMTaskbarWnd, pt, entry)) {
            return true;
        }
    }
    return false;
}

// -----------------------------------------------------------------------------
// Brightness (WMI for internal + DDC/CI for external)
// -----------------------------------------------------------------------------

int GetBrightnessWmi() {
    int ret = -1;
    IWbemLocator* pLocator = NULL;
    IWbemServices* pNamespace = 0;
    IEnumWbemClassObject* pEnum = NULL;
    HRESULT hr = S_OK;

    BSTR path = SysAllocString(L"root\\wmi");
    BSTR ClassPath = SysAllocString(L"WmiMonitorBrightness");
    BSTR bstrQuery = SysAllocString(L"Select * from WmiMonitorBrightness");

    if (!path || !ClassPath) goto cleanup;

    hr = CoInitialize(0);
    if (FAILED(hr)) goto cleanup;

    CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
                         RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_SECURE_REFS,
                         NULL);

    hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                          IID_IWbemLocator, (LPVOID*)&pLocator);
    if (FAILED(hr)) goto cleanup;

    hr = pLocator->ConnectServer(path, NULL, NULL, NULL, 0, NULL, NULL,
                                 &pNamespace);
    if (hr != WBEM_S_NO_ERROR) goto cleanup;

    hr = CoSetProxyBlanket(pNamespace, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
                           NULL, RPC_C_AUTHN_LEVEL_PKT,
                           RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
    if (hr != WBEM_S_NO_ERROR) goto cleanup;

    hr = pNamespace->ExecQuery(_bstr_t(L"WQL"), bstrQuery,
                               WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnum);
    if (hr != WBEM_S_NO_ERROR) goto cleanup;

    hr = WBEM_S_NO_ERROR;
    while (WBEM_S_NO_ERROR == hr) {
        ULONG ulReturned;
        IWbemClassObject* pObj = NULL;
        hr = pEnum->Next(WBEM_INFINITE, 1, &pObj, &ulReturned);
        if (FAILED(hr)) goto cleanup;
        if (hr == WBEM_S_FALSE) goto cleanup;

        VARIANT var1;
        hr = pObj->Get(_bstr_t(L"CurrentBrightness"), 0, &var1, NULL, NULL);
        ret = V_UI1(&var1);
        VariantClear(&var1);
        pObj->Release();
        if (hr != WBEM_S_NO_ERROR) goto cleanup;
    }

cleanup:
    SysFreeString(path);
    SysFreeString(ClassPath);
    SysFreeString(bstrQuery);
    if (pLocator) pLocator->Release();
    if (pNamespace) pNamespace->Release();
    if (pEnum) pEnum->Release();
    CoUninitialize();
    return ret;
}

bool SetBrightnessWmi(int val) {
    bool bRet = true;
    IWbemLocator* pLocator = NULL;
    IWbemServices* pNamespace = 0;
    IWbemClassObject* pClass = NULL;
    IWbemClassObject* pInClass = NULL;
    IWbemClassObject* pInInst = NULL;
    IEnumWbemClassObject* pEnum = NULL;
    HRESULT hr = S_OK;

    BSTR path = SysAllocString(L"root\\wmi");
    BSTR ClassPath = SysAllocString(L"WmiMonitorBrightnessMethods");
    BSTR MethodName = SysAllocString(L"WmiSetBrightness");
    BSTR ArgName0 = SysAllocString(L"Timeout");
    BSTR ArgName1 = SysAllocString(L"Brightness");
    BSTR bstrQuery =
        SysAllocString(L"Select * from WmiMonitorBrightnessMethods");

    if (!path || !ClassPath || !MethodName || !ArgName0) {
        bRet = false; goto cleanup;
    }

    hr = CoInitialize(0);
    if (FAILED(hr)) { bRet = false; goto cleanup; }

    CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
                         RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_SECURE_REFS,
                         NULL);

    hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                          IID_IWbemLocator, (LPVOID*)&pLocator);
    if (FAILED(hr)) { bRet = false; goto cleanup; }

    hr = pLocator->ConnectServer(path, NULL, NULL, NULL, 0, NULL, NULL,
                                 &pNamespace);
    if (hr != WBEM_S_NO_ERROR) { bRet = false; goto cleanup; }

    hr = CoSetProxyBlanket(pNamespace, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
                           NULL, RPC_C_AUTHN_LEVEL_PKT,
                           RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
    if (hr != WBEM_S_NO_ERROR) { bRet = false; goto cleanup; }

    hr = pNamespace->ExecQuery(_bstr_t(L"WQL"), bstrQuery,
                               WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnum);
    if (hr != WBEM_S_NO_ERROR) { bRet = false; goto cleanup; }

    hr = WBEM_S_NO_ERROR;
    while (WBEM_S_NO_ERROR == hr) {
        ULONG ulReturned;
        IWbemClassObject* pObj;
        hr = pEnum->Next(WBEM_INFINITE, 1, &pObj, &ulReturned);
        if (FAILED(hr)) { bRet = false; goto cleanup; }
        if (hr == WBEM_S_FALSE) goto cleanup;

        hr = pNamespace->GetObject(ClassPath, 0, NULL, &pClass, NULL);
        if (hr != WBEM_S_NO_ERROR) { bRet = false; goto cleanup; }

        hr = pClass->GetMethod(MethodName, 0, &pInClass, NULL);
        if (hr != WBEM_S_NO_ERROR) { bRet = false; goto cleanup; }

        hr = pInClass->SpawnInstance(0, &pInInst);
        if (hr != WBEM_S_NO_ERROR) { bRet = false; goto cleanup; }

        VARIANT var1;
        VariantInit(&var1);
        V_VT(&var1) = VT_BSTR;
        V_BSTR(&var1) = SysAllocString(L"0");
        hr = pInInst->Put(ArgName0, 0, &var1, CIM_UINT32);
        VariantClear(&var1);
        if (hr != WBEM_S_NO_ERROR) { bRet = false; goto cleanup; }

        VARIANT var;
        VariantInit(&var);
        V_VT(&var) = VT_BSTR;
        WCHAR buf[10] = {0};
        swprintf_s(buf, _countof(buf), L"%d", val);
        V_BSTR(&var) = SysAllocString(buf);
        hr = pInInst->Put(ArgName1, 0, &var, CIM_UINT8);
        VariantClear(&var);
        if (hr != WBEM_S_NO_ERROR) { bRet = false; goto cleanup; }

        VARIANT pathVariable;
        VariantInit(&pathVariable);
        hr = pObj->Get(_bstr_t(L"__PATH"), 0, &pathVariable, NULL, NULL);
        if (hr != WBEM_S_NO_ERROR) goto cleanup;

        hr = pNamespace->ExecMethod(pathVariable.bstrVal, MethodName, 0, NULL,
                                    pInInst, NULL, NULL);
        VariantClear(&pathVariable);
        if (hr != WBEM_S_NO_ERROR) { bRet = false; goto cleanup; }
        pObj->Release();
    }

cleanup:
    SysFreeString(path);
    SysFreeString(ClassPath);
    SysFreeString(MethodName);
    SysFreeString(ArgName0);
    SysFreeString(ArgName1);
    SysFreeString(bstrQuery);
    if (pClass) pClass->Release();
    if (pInInst) pInInst->Release();
    if (pInClass) pInClass->Release();
    if (pLocator) pLocator->Release();
    if (pNamespace) pNamespace->Release();
    if (pEnum) pEnum->Release();
    CoUninitialize();
    return bRet;
}

// IOCTL direct backlight — bypasses WDDM brightness pipeline entirely.
// This is the fastest method on laptops with ACPI-compatible backlights.
bool AdjustBrightnessIoctl(int delta, int* pNewBrightness) {
    if (g_hLCD == INVALID_HANDLE_VALUE) {
        g_hLCD = CreateFileW(L"\\\\.\\LCD", GENERIC_READ | GENERIC_WRITE,
                             FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                             OPEN_EXISTING, 0, NULL);
        if (g_hLCD == INVALID_HANDLE_VALUE) return false;
    }
    DISPLAY_BRIGHTNESS_IOCTL db = {};
    DWORD ret = 0;
    if (!DeviceIoControl(g_hLCD, IOCTL_VIDEO_QUERY_DISPLAY_BRIGHTNESS_VAL,
                         NULL, 0, &db, sizeof(db), &ret, NULL))
        return false;
    int newVal = std::clamp((int)db.ucACBrightness + delta, 0, 100);
    db.ucDisplayPolicy = 3;
    db.ucACBrightness = (BYTE)newVal;
    db.ucDCBrightness = (BYTE)newVal;
    if (!DeviceIoControl(g_hLCD, IOCTL_VIDEO_SET_DISPLAY_BRIGHTNESS_VAL,
                         &db, sizeof(db), NULL, 0, &ret, NULL))
        return false;
    Wh_Log(L"IOCTL: brightness %d -> %d", db.ucACBrightness, newVal);
    if (pNewBrightness) *pNewBrightness = newVal;
    return true;
}

int ReadBrightnessIoctl() {
    if (g_hLCD == INVALID_HANDLE_VALUE) return -1;
    DISPLAY_BRIGHTNESS_IOCTL db = {};
    DWORD ret = 0;
    if (!DeviceIoControl(g_hLCD, IOCTL_VIDEO_QUERY_DISPLAY_BRIGHTNESS_VAL,
                         NULL, 0, &db, sizeof(db), &ret, NULL))
        return -1;
    return (int)db.ucACBrightness;
}

// PowerScheme last-resort fallback — uses PowerGet/SetActiveScheme with
// rate-limited writes to avoid the infamous brightness "jerking" bug.
bool InitPowerScheme() {
    if (g_hPowrProf) return true;
    g_hPowrProf = LoadLibraryW(L"PowrProf.dll");
    if (!g_hPowrProf) return false;
    pPowerGetActiveScheme = (PowerGetActiveScheme_t)
        GetProcAddress(g_hPowrProf, "PowerGetActiveScheme");
    pPowerReadACValueIndex = (PowerReadACValueIndex_t)
        GetProcAddress(g_hPowrProf, "PowerReadACValueIndex");
    pPowerWriteACValueIndex = (PowerWriteACValueIndex_t)
        GetProcAddress(g_hPowrProf, "PowerWriteACValueIndex");
    pPowerWriteDCValueIndex = (PowerWriteDCValueIndex_t)
        GetProcAddress(g_hPowrProf, "PowerWriteDCValueIndex");
    pPowerSetActiveScheme = (PowerSetActiveScheme_t)
        GetProcAddress(g_hPowrProf, "PowerSetActiveScheme");
    if (!pPowerGetActiveScheme || !pPowerReadACValueIndex ||
        !pPowerWriteACValueIndex || !pPowerSetActiveScheme) {
        FreeLibrary(g_hPowrProf); g_hPowrProf = nullptr; return false;
    }
    return true;
}

int ReadBrightnessPowerScheme() {
    if (!InitPowerScheme()) return -1;
    GUID* s = NULL; DWORD v = 0;
    if (pPowerGetActiveScheme(NULL, &s) != 0) return -1;
    DWORD r = pPowerReadACValueIndex(NULL, s, &VID_BRIGHTNESS_SUB,
                                      &VID_BRIGHTNESS_VAL, &v);
    LocalFree(s);
    return (r == 0) ? (int)v : -1;
}

bool WriteBrightnessPowerScheme(int val) {
    if (!InitPowerScheme()) return false;
    val = std::clamp(val, 0, 100);
    GUID* s = NULL;
    if (pPowerGetActiveScheme(NULL, &s) != 0) return false;
    pPowerWriteACValueIndex(NULL, s, &VID_BRIGHTNESS_SUB,
                            &VID_BRIGHTNESS_VAL, (DWORD)val);
    if (pPowerWriteDCValueIndex)
        pPowerWriteDCValueIndex(NULL, s, &VID_BRIGHTNESS_SUB,
                                &VID_BRIGHTNESS_VAL, (DWORD)val);
    DWORD r = pPowerSetActiveScheme(NULL, s);
    LocalFree(s);
    return (r == 0);
}

bool AdjustBrightnessPowerScheme(int delta, int* pNewBrightness) {
    // Rate-limit PowerScheme writes — competing WDDM transitions cause
    // the "jerking" effect if we write faster than ~250 ms.
    DWORD now = GetTickCount();
    if (now - g_lastPowerWriteMs < 250 && pNewBrightness) {
        // Coalesce: drop but report the intended value so the OSD stays in sync.
        int cur = ReadBrightnessPowerScheme();
        if (cur >= 0) {
            *pNewBrightness = std::clamp(cur + delta, 0, 100);
        }
        return true;
    }
    int cur = ReadBrightnessPowerScheme();
    if (cur < 0) return false;
    int newVal = std::clamp(cur + delta, 0, 100);
    if (WriteBrightnessPowerScheme(newVal)) {
        g_lastPowerWriteMs = now;
        if (pNewBrightness) *pNewBrightness = newVal;
        Wh_Log(L"PowerScheme: brightness %d -> %d", cur, newVal);
        return true;
    }
    return false;
}

// DDC/CI brightness (external monitors). Returns true on success. If a
// successful change occurred, *pNewBrightness receives the new value as a
// 0..100 percent (mapped from the monitor's reported min/max range).
bool AdjustBrightnessDdcCi(HMONITOR hMonitor, int delta, int* pNewBrightness) {
    DWORD numPhysical = 0;
    if (!GetNumberOfPhysicalMonitorsFromHMONITOR(hMonitor, &numPhysical) ||
        numPhysical == 0) {
        return false;
    }

    std::vector<PHYSICAL_MONITOR> physical(numPhysical);
    if (!GetPhysicalMonitorsFromHMONITOR(hMonitor, numPhysical,
                                         physical.data())) {
        return false;
    }

    bool anySuccess = false;
    for (DWORD i = 0; i < numPhysical; i++) {
        DWORD dwMin = 0, dwCurrent = 0, dwMax = 0;
        if (!GetMonitorBrightness(physical[i].hPhysicalMonitor, &dwMin,
                                  &dwCurrent, &dwMax)) {
            continue;
        }

        DWORD newVal = dwCurrent;
        if (delta > 0) {
            newVal = std::min(dwCurrent + (DWORD)delta, dwMax);
        } else if (delta < 0) {
            DWORD sub = (DWORD)(-delta);
            newVal = sub <= dwCurrent - dwMin ? dwCurrent - sub : dwMin;
        }

        Wh_Log(L"DDC/CI: %s brightness %lu -> %lu (min %lu, max %lu)",
               physical[i].szPhysicalMonitorDescription, dwCurrent, newVal,
               dwMin, dwMax);

        if (SetMonitorBrightness(physical[i].hPhysicalMonitor, newVal)) {
            anySuccess = true;
            if (pNewBrightness && dwMax > dwMin) {
                // Normalize to 0..100 for the OSD.
                *pNewBrightness =
                    (int)((newVal - dwMin) * 100 / (dwMax - dwMin));
            }
        }
    }

    DestroyPhysicalMonitors(numPhysical, physical.data());
    return anySuccess;
}

// Multi-backend brightness: IOCTL → DDC/CI → WMI → PowerScheme.
// On success, *pNewBrightness gets the post-change value in 0..100.
bool AdjustBrightness(HWND hTaskbarWnd, int delta,
                      int* pNewBrightness = nullptr) {
    // 1) IOCTL — direct backlight, bypasses WDDM smoothing (fastest).
    if (AdjustBrightnessIoctl(delta, pNewBrightness)) {
        g_lastBrightnessWriteMs = GetTickCount();
        // Invalidate WMI cache so fallback reads stay fresh.
        g_cachedWmiBrightness = -1;
        return true;
    }

    // 2) DDC/CI — external monitors.
    HMONITOR hMonitor =
        MonitorFromWindow(hTaskbarWnd, MONITOR_DEFAULTTONEAREST);
    if (hMonitor && AdjustBrightnessDdcCi(hMonitor, delta, pNewBrightness)) {
        g_lastBrightnessWriteMs = GetTickCount();
        g_cachedWmiBrightness = -1;
        return true;
    }

    // 3) WMI — internal laptop displays.
    int brightness = g_cachedWmiBrightness;
    if (brightness < 0 ||
        GetTickCount() - g_lastWmiBrightnessTime >
            Constants::kWmiBrightnessCacheTtlMs) {
        brightness = GetBrightnessWmi();
        if (brightness >= 0) {
            g_cachedWmiBrightness = brightness;
            g_lastWmiBrightnessTime = GetTickCount();
        }
    }
    if (brightness >= 0) {
        int newBrightness = std::clamp(brightness + delta, 0, 100);
        Wh_Log(L"WMI: Changing brightness from %d to %d", brightness,
               newBrightness);
        if (SetBrightnessWmi(newBrightness)) {
            g_cachedWmiBrightness = newBrightness;
            g_lastWmiBrightnessTime = GetTickCount();
            g_lastBrightnessWriteMs = GetTickCount();
            if (pNewBrightness) *pNewBrightness = newBrightness;
            return true;
        }
    }

    // 4) PowerScheme — last-resort fallback.
    if (AdjustBrightnessPowerScheme(delta, pNewBrightness)) {
        g_cachedWmiBrightness = -1;
        return true;
    }
    return false;
}

bool IsMonitorHdrEnabled(HMONITOR hMonitor) {
    // HDR state can change at runtime (user toggling in Settings, going
    // fullscreen with auto-HDR, etc.) so we cache for a short TTL only.
    auto it = g_monitorHdrCache.find(hMonitor);
    if (it != g_monitorHdrCache.end()) {
        if (GetTickCount() - it->second.lastCheckTime <
            Constants::kHdrCacheTtlMs) {
            return it->second.isHdr;
        }
    }

    bool isHdr = false;
    HMODULE hDXGI = LoadLibrary(L"dxgi.dll");
    if (hDXGI) {
        auto createDXGIFactory1 =
            (PFN_CreateDXGIFactory1)GetProcAddress(hDXGI, "CreateDXGIFactory1");
        if (createDXGIFactory1) {
            IDXGIFactory1* pFactory = nullptr;
            if (SUCCEEDED(createDXGIFactory1(__uuidof(IDXGIFactory1),
                                             (void**)&pFactory)) &&
                pFactory) {
                IDXGIAdapter1* pAdapter = nullptr;
                for (UINT i = 0;
                     i < Constants::kMaxMonitorsToProbe &&
                     SUCCEEDED(pFactory->EnumAdapters1(i, &pAdapter)); i++) {
                    IDXGIOutput* pOutput = nullptr;
                    for (UINT j = 0;
                         j < Constants::kMaxMonitorsToProbe &&
                         SUCCEEDED(pAdapter->EnumOutputs(j, &pOutput)); j++) {
                        DXGI_OUTPUT_DESC desc;
                        if (SUCCEEDED(pOutput->GetDesc(&desc)) &&
                            desc.Monitor == hMonitor) {
                            IDXGIOutput6* pOutput6 = nullptr;
                            if (SUCCEEDED(pOutput->QueryInterface(
                                    __uuidof(IDXGIOutput6),
                                    (void**)&pOutput6))) {
                                DXGI_OUTPUT_DESC1 desc1;
                                if (SUCCEEDED(pOutput6->GetDesc1(&desc1))) {
                                    isHdr =
                                        desc1.ColorSpace ==
                                        DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;
                                }
                                pOutput6->Release();
                            }
                        }
                        if (pOutput) pOutput->Release();
                    }
                    if (pAdapter) pAdapter->Release();
                }
                pFactory->Release();
            }
        }
        FreeLibrary(hDXGI);
    }

    g_monitorHdrCache[hMonitor] = {isHdr, GetTickCount()};
    return isHdr;
}

bool InitializeSdrToHdrBrightness() {
    if (g_pDwmpSDRToHDRBoost) return true;

    if (!g_hDwmApiDll) {
        g_hDwmApiDll = LoadLibrary(L"dwmapi.dll");
        if (!g_hDwmApiDll) {
            Wh_Log(L"Failed to load dwmapi.dll for SDR brightness control.");
            return false;
        }
    }

    FARPROC pFunc = GetProcAddress(g_hDwmApiDll, (LPCSTR)171);
    if (!pFunc) {
        Wh_Log(L"Failed to get DwmpSDRToHDRBoost ordinal 171.");
        return false;
    }
    g_pDwmpSDRToHDRBoost = (DwmpSDRToHDRBoostPtr_t)pFunc;

    if (g_settings.storeCurrentSdrToHdrBrightness &&
        g_currentSdrToHdrBrightness == 1.0) {
        int savedValue = Wh_GetIntValue(L"CurrentSdrToHdrBrightness", 10);
        g_currentSdrToHdrBrightness = savedValue / 10.0;
        if (g_currentSdrToHdrBrightness <= 1.0) {
            g_currentSdrToHdrBrightness = Constants::kSdrMinBrightness;
        } else if (g_currentSdrToHdrBrightness > g_settings.maxSdrBrightness) {
            g_currentSdrToHdrBrightness = g_settings.maxSdrBrightness;
        }
        Wh_Log(L"Loaded saved SDR brightness: %.2f",
               g_currentSdrToHdrBrightness);
    }
    return true;
}

BOOL CALLBACK MonitorEnumProcForSDRBrightness(HMONITOR hMonitor,
                                              HDC hdcMonitor,
                                              LPRECT lprcMonitor,
                                              LPARAM dwData) {
    auto* pData = reinterpret_cast<SDRBrightnessCallbackData*>(dwData);
    if (pData && pData->pFunc) {
        pData->pFunc(hMonitor, pData->brightness);
    }
    return TRUE;
}

bool AdjustSdrToHdrBrightness(int clicks, HMONITOR targetMonitor = NULL) {
    if (!InitializeSdrToHdrBrightness() || !g_pDwmpSDRToHDRBoost) {
        return false;
    }

    double newBrightness =
        g_currentSdrToHdrBrightness + clicks * Constants::kSdrStep;
    newBrightness = std::clamp(newBrightness, Constants::kSdrMinBrightness,
                               g_settings.maxSdrBrightness);

    if (newBrightness != g_currentSdrToHdrBrightness) {
        g_currentSdrToHdrBrightness = newBrightness;
        Wh_Log(L"Setting SDR-to-HDR brightness to %.4f",
               g_currentSdrToHdrBrightness);

        if (g_settings.storeCurrentSdrToHdrBrightness) {
            // Persisted as integer tenths to avoid floating-point storage.
            Wh_SetIntValue(L"CurrentSdrToHdrBrightness",
                           (int)(g_currentSdrToHdrBrightness * 10));
        }

        SDRBrightnessCallbackData data = {g_pDwmpSDRToHDRBoost,
                                          g_currentSdrToHdrBrightness};
        if (targetMonitor) {
            // Apply only to the requested monitor.
            g_pDwmpSDRToHDRBoost(targetMonitor, g_currentSdrToHdrBrightness);
        } else {
            // No specific monitor: fan out to every monitor.
            EnumDisplayMonitors(NULL, NULL, MonitorEnumProcForSDRBrightness,
                                reinterpret_cast<LPARAM>(&data));
        }
    }
    return true;
}

// -----------------------------------------------------------------------------
// Virtual desktop switching
// -----------------------------------------------------------------------------

bool SwitchDesktopViaKeyboardShortcut(int clicks) {
    if (GetKeyState(VK_CONTROL) < 0 || GetKeyState(VK_MENU) < 0 ||
        GetKeyState(VK_SHIFT) < 0 || GetKeyState(VK_PRIOR) < 0 ||
        GetKeyState(VK_NEXT) < 0) {
        return false;
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        SetForegroundWindow(hTaskbarWnd);
    }

    WORD key = VK_LEFT;
    if (clicks < 0) {
        clicks = -clicks;
        key = VK_RIGHT;
    }

    size_t numInputs = static_cast<size_t>(clicks) * 2 + 4;
    INPUT* input = new INPUT[numInputs];
    ZeroMemory(input, numInputs * sizeof(INPUT));
    for (size_t i = 0; i < numInputs; i++) {
        input[i].type = INPUT_KEYBOARD;
    }

    input[0].ki.wVk = VK_LWIN;
    input[1].ki.wVk = VK_LCONTROL;
    for (int i = 0; i < clicks; i++) {
        input[2 + i * 2].ki.wVk = key;
        input[2 + i * 2 + 1].ki.wVk = key;
        input[2 + i * 2 + 1].ki.dwFlags = KEYEVENTF_KEYUP;
    }
    input[2 + clicks * 2].ki.wVk = VK_LCONTROL;
    input[2 + clicks * 2].ki.dwFlags = KEYEVENTF_KEYUP;
    input[2 + clicks * 2 + 1].ki.wVk = VK_LWIN;
    input[2 + clicks * 2 + 1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(static_cast<UINT>(numInputs), input, sizeof(INPUT));
    delete[] input;
    return true;
}

// -----------------------------------------------------------------------------
// Audio: shared COM constants and device enumerator
// -----------------------------------------------------------------------------

const static GUID XIID_IMMDeviceEnumerator = {
    0xA95664D2, 0x9614, 0x4F35,
    {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6}};
const static GUID XIID_MMDeviceEnumerator = {
    0xBCDE0395, 0xE52F, 0x467C,
    {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E}};
const static GUID XIID_IAudioEndpointVolume = {
    0x5CDF2C82, 0x841E, 0x4546,
    {0x97, 0x22, 0x0C, 0xF7, 0x40, 0x78, 0x22, 0x9A}};

static IMMDeviceEnumerator* g_pDeviceEnumerator;
bool g_bAudioInitialized;

void AudioInit() {
    if (g_bAudioInitialized) return;
    g_bAudioInitialized = true;
    HRESULT hr = CoCreateInstance(
        XIID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER,
        XIID_IMMDeviceEnumerator, (LPVOID*)&g_pDeviceEnumerator);
    if (FAILED(hr)) {
        g_pDeviceEnumerator = NULL;
    }
}

void AudioUninit() {
    if (g_pDeviceEnumerator) {
        g_pDeviceEnumerator->Release();
        g_pDeviceEnumerator = NULL;
    }
    g_bAudioInitialized = false;
}

// -----------------------------------------------------------------------------
// Microphone volume
// -----------------------------------------------------------------------------

BOOL AddMicMasterVolumeLevelScalar(float fMasterVolumeAdd) {
    IMMDevice* defaultDevice = NULL;
    IAudioEndpointVolume* endpointVolume = NULL;
    HRESULT hr;
    float fMasterVolume;
    BOOL bSuccess = FALSE;

    AudioInit();
    if (!g_pDeviceEnumerator) return FALSE;

    hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole,
                                                     &defaultDevice);
    if (SUCCEEDED(hr)) {
        hr = defaultDevice->Activate(XIID_IAudioEndpointVolume,
                                     CLSCTX_INPROC_SERVER, NULL,
                                     (LPVOID*)&endpointVolume);
        if (SUCCEEDED(hr)) {
            if (SUCCEEDED(endpointVolume->GetMasterVolumeLevelScalar(
                    &fMasterVolume))) {
                fMasterVolume = std::clamp(fMasterVolume + fMasterVolumeAdd,
                                           0.0f, 1.0f);
                if (SUCCEEDED(endpointVolume->SetMasterVolumeLevelScalar(
                        fMasterVolume, NULL))) {
                    bSuccess = TRUE;
                }
            }
            endpointVolume->Release();
        }
        defaultDevice->Release();
    }
    return bSuccess;
}

// Returns the current microphone volume in 0.0..1.0, or -1.0 on failure.
// Used by the custom OSD; cheap enough to call after each scroll.
float GetCurrentMicVolumeScalar() {
    AudioInit();
    if (!g_pDeviceEnumerator) return -1.0f;
    IMMDevice* defaultDevice = NULL;
    IAudioEndpointVolume* endpointVolume = NULL;
    float fVol = -1.0f;
    HRESULT hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(
        eCapture, eConsole, &defaultDevice);
    if (SUCCEEDED(hr)) {
        hr = defaultDevice->Activate(XIID_IAudioEndpointVolume,
                                     CLSCTX_INPROC_SERVER, NULL,
                                     (LPVOID*)&endpointVolume);
        if (SUCCEEDED(hr)) {
            endpointVolume->GetMasterVolumeLevelScalar(&fVol);
            endpointVolume->Release();
        }
        defaultDevice->Release();
    }
    return fVol;
}

// Returns the current system volume in 0.0..1.0, or -1.0 on failure.
float GetCurrentVolumeScalar() {
    AudioInit();
    if (!g_pDeviceEnumerator) return -1.0f;
    IMMDevice* defaultDevice = NULL;
    IAudioEndpointVolume* endpointVolume = NULL;
    float fVol = -1.0f;
    HRESULT hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(
        eRender, eConsole, &defaultDevice);
    if (SUCCEEDED(hr)) {
        hr = defaultDevice->Activate(XIID_IAudioEndpointVolume,
                                     CLSCTX_INPROC_SERVER, NULL,
                                     (LPVOID*)&endpointVolume);
        if (SUCCEEDED(hr)) {
            endpointVolume->GetMasterVolumeLevelScalar(&fVol);
            endpointVolume->Release();
        }
        defaultDevice->Release();
    }
    return fVol;
}

// -----------------------------------------------------------------------------
// System volume (from Taskbar Volume Control)
// -----------------------------------------------------------------------------

BOOL IsDefaultAudioEndpointAvailable() {
    AudioInit();
    if (!g_pDeviceEnumerator) return FALSE;
    IMMDevice* defaultDevice = NULL;
    HRESULT hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(
        eRender, eConsole, &defaultDevice);
    if (SUCCEEDED(hr)) {
        defaultDevice->Release();
        return TRUE;
    }
    return FALSE;
}

BOOL ToggleVolMuted() {
    AudioInit();
    if (!g_pDeviceEnumerator) return FALSE;
    IMMDevice* defaultDevice = NULL;
    IAudioEndpointVolume* endpointVolume = NULL;
    BOOL bMuted;
    BOOL bSuccess = FALSE;

    HRESULT hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(
        eRender, eConsole, &defaultDevice);
    if (SUCCEEDED(hr)) {
        hr = defaultDevice->Activate(XIID_IAudioEndpointVolume,
                                     CLSCTX_INPROC_SERVER, NULL,
                                     (LPVOID*)&endpointVolume);
        if (SUCCEEDED(hr)) {
            if (SUCCEEDED(endpointVolume->GetMute(&bMuted))) {
                if (SUCCEEDED(endpointVolume->SetMute(!bMuted, NULL))) {
                    bSuccess = TRUE;
                }
            }
            endpointVolume->Release();
        }
        defaultDevice->Release();
    }
    return bSuccess;
}

BOOL AddMasterVolumeLevelScalar(float fMasterVolumeAdd) {
    AudioInit();
    if (!g_pDeviceEnumerator) return FALSE;
    IMMDevice* defaultDevice = NULL;
    IAudioEndpointVolume* endpointVolume = NULL;
    float fMasterVolume;
    BOOL bSuccess = FALSE;

    HRESULT hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(
        eRender, eConsole, &defaultDevice);
    if (SUCCEEDED(hr)) {
        hr = defaultDevice->Activate(XIID_IAudioEndpointVolume,
                                     CLSCTX_INPROC_SERVER, NULL,
                                     (LPVOID*)&endpointVolume);
        if (SUCCEEDED(hr)) {
            if (SUCCEEDED(endpointVolume->GetMasterVolumeLevelScalar(
                    &fMasterVolume))) {
                fMasterVolume = std::clamp(fMasterVolume + fMasterVolumeAdd,
                                           0.0f, 1.0f);
                if (SUCCEEDED(endpointVolume->SetMasterVolumeLevelScalar(
                        fMasterVolume, NULL))) {
                    bSuccess = TRUE;
                    if (!g_settings.noAutomaticMuteToggle) {
                        // Match Windows' default behaviour: mute when we hit
                        // the floor, unmute as soon as we move off it.
                        endpointVolume->SetMute(
                            fMasterVolume < Constants::kAutoMuteThreshold,
                            NULL);
                    }
                }
            }
            endpointVolume->Release();
        }
        defaultDevice->Release();
    }
    return bSuccess;
}

static BOOL AdjustVolumeLevelWithMouseWheel(int nWheelDelta, int nStep) {
    if (!nStep) {
        nStep = g_settings.volumeChangeStep;
        if (!nStep) nStep = 2;
    }
    return AddMasterVolumeLevelScalar((float)nWheelDelta * nStep *
                                      ((float)0.01 / 120));
}

// -----------------------------------------------------------------------------
// Win11 native volume indicator (post-message based)
// -----------------------------------------------------------------------------

UINT g_uShellHookMsg = RegisterWindowMessage(L"SHELLHOOK");
DWORD g_volLastScrollTime;
short g_volLastScrollDeltaRemainder;

// Posts a Windows app command (APPCOMMAND_VOLUME_*, APPCOMMAND_MICROPHONE_*)
// to MSTaskSwWClass via SHELLHOOK. On Win11 22H2+ this triggers the native
// OSD; on older builds it just adjusts the relevant level. The `count`
// parameter posts multiple successive commands - useful where a single
// command produces too small a change but we still want the OSD shown once.
bool PostAppCommand(SHORT appCommand, int count) {
    if (!g_hTaskbarWnd) return false;
    HWND hReBarWindow32 =
        FindWindowEx(g_hTaskbarWnd, nullptr, L"ReBarWindow32", nullptr);
    if (!hReBarWindow32) return false;
    HWND hMSTaskSwWClass =
        FindWindowEx(hReBarWindow32, nullptr, L"MSTaskSwWClass", nullptr);
    if (!hMSTaskSwWClass) return false;

    for (int i = 0; i < count; i++) {
        PostMessage(hMSTaskSwWClass, g_uShellHookMsg, HSHELL_APPCOMMAND,
                    MAKELPARAM(0, appCommand));
    }
    return true;
}

bool Win11IndicatorAdjustVolumeLevelWithMouseWheel(short delta) {
    if (GetTickCount() - g_volLastScrollTime <
        Constants::kWheelAccumulationWindowMs) {
        delta += g_volLastScrollDeltaRemainder;
    }

    int clicks = delta / WHEEL_DELTA;
    Wh_Log(L"Win11 indicator: %d clicks (delta=%d)", clicks, delta);

    if (clicks) {
        SHORT appCommand = APPCOMMAND_VOLUME_UP;
        if (clicks < 0) {
            clicks = -clicks;
            appCommand = APPCOMMAND_VOLUME_DOWN;
        }
        if (g_settings.volumeChangeStep) {
            // Each AppCommand changes by ~2%, so map step accordingly.
            clicks *= g_settings.volumeChangeStep / 2;
        }
        if (clicks > 1) {
            // Apply all but the last 2% step manually; then issue ONE
            // AppCommand to show the OSD with the new value.
            AddMasterVolumeLevelScalar(
                (appCommand == APPCOMMAND_VOLUME_UP
                     ? Constants::kWin11ManualStep
                     : -Constants::kWin11ManualStep) *
                (float)(clicks - 1));
        }
        if (!PostAppCommand(appCommand, 1)) return false;
    }

    g_volLastScrollTime = GetTickCount();
    g_volLastScrollDeltaRemainder = delta % WHEEL_DELTA;
    return true;
}

// -----------------------------------------------------------------------------
// Microphone volume indicator (Win11 22H2+ via APPCOMMAND, with custom-OSD
// fallback handled in the dispatch path).
// -----------------------------------------------------------------------------

// Returns true if the native Win11 mic OSD is available (i.e., we're on
// Win11 22H2+ where APPCOMMAND_MICROPHONE_VOLUME_* trigger the flyout).
bool CanUseNativeMicIndicator() {
    return g_nWinVersion >= WIN_VERSION_11_22H2;
}

DWORD g_micLastScrollTime;
short g_micLastScrollDeltaRemainder;

// Mirror of Win11IndicatorAdjustVolumeLevelWithMouseWheel for mic. Each
// AppCommand changes mic level by ~2%; we top up the rest manually.
// Returns true on success, false if the AppCommand couldn't be posted
// (caller should fall back to the custom OSD path).
bool Win11IndicatorAdjustMicLevelWithMouseWheel(short delta) {
    if (GetTickCount() - g_micLastScrollTime <
        Constants::kWheelAccumulationWindowMs) {
        delta += g_micLastScrollDeltaRemainder;
    }

    int clicks = delta / WHEEL_DELTA;
    Wh_Log(L"Mic indicator: %d clicks (delta=%d)", clicks, delta);

    bool ok = true;
    if (clicks) {
        SHORT appCommand = APPCOMMAND_MICROPHONE_VOLUME_UP;
        if (clicks < 0) {
            clicks = -clicks;
            appCommand = APPCOMMAND_MICROPHONE_VOLUME_DOWN;
        }
        if (clicks > 1) {
            // Top up the manual part; AppCommand handles last 2%.
            AddMicMasterVolumeLevelScalar(
                (appCommand == APPCOMMAND_MICROPHONE_VOLUME_UP
                     ? Constants::kWin11ManualStep
                     : -Constants::kWin11ManualStep) *
                (float)(clicks - 1));
        }
        ok = PostAppCommand(appCommand, 1);
    }

    g_micLastScrollTime = GetTickCount();
    g_micLastScrollDeltaRemainder = delta % WHEEL_DELTA;
    return ok;
}

// -----------------------------------------------------------------------------
// SndVol launching (classic + modern indicators)
// -----------------------------------------------------------------------------

static HANDLE hSndVolProcess;
static HWND hSndVolWnd;
static UINT_PTR nCloseSndVolTimer;
static int nCloseSndVolTimerCount;
static HWND hSndVolModernPreviousForegroundWnd;
static BOOL bSndVolModernLaunched;
static BOOL bSndVolModernAppeared;

static BOOL CanUseModernIndicator() {
    if (g_nWinVersion < WIN_VERSION_10_T1 ||
        g_settings.volumeIndicator == VolumeIndicator::Classic) {
        return FALSE;
    }
    DWORD dwEnabled = 1;
    DWORD dwValueSize = sizeof(dwEnabled);
    RegGetValue(HKEY_LOCAL_MACHINE,
                L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\MTCUVC",
                L"EnableMTCUVC", RRF_RT_REG_DWORD, NULL, &dwEnabled,
                &dwValueSize);
    return dwEnabled != 0;
}

static HWND GetOpenSndVolModernIndicatorWnd() {
    HWND hForegroundWnd = GetForegroundWindow();
    if (!hForegroundWnd) return NULL;
    WCHAR szBuffer[32];
    if (!GetClassName(hForegroundWnd, szBuffer, 32) ||
        wcscmp(szBuffer, L"Windows.UI.Core.CoreWindow") != 0) {
        return NULL;
    }
    WCHAR szVerifyPropName[sizeof(
        "ApplicationView_CustomWindowTitle#1234567890#MtcUvc")];
    wsprintf(szVerifyPropName, L"ApplicationView_CustomWindowTitle#%u#MtcUvc",
             (DWORD)(DWORD_PTR)hForegroundWnd);
    SetLastError(0);
    GetProp(hForegroundWnd, szVerifyPropName);
    if (GetLastError() != 0) return NULL;
    return hForegroundWnd;
}

static BOOL CALLBACK EnumThreadFindSndVolTrayControlWnd(HWND hWnd,
                                                        LPARAM lParam) {
    HMODULE hInstance = (HMODULE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
    if (hInstance && hInstance == GetModuleHandle(L"sndvolsso.dll")) {
        *(HWND*)lParam = hWnd;
        return FALSE;
    }
    return TRUE;
}

static HWND GetSndVolTrayControlWnd() {
    HWND hBluetoothNotificationWnd =
        FindWindow(L"BluetoothNotificationAreaIconWindowClass", NULL);
    if (!hBluetoothNotificationWnd) return NULL;
    HWND hWnd = NULL;
    EnumThreadWindows(
        GetWindowThreadProcessId(hBluetoothNotificationWnd, NULL),
        EnumThreadFindSndVolTrayControlWnd, (LPARAM)&hWnd);
    return hWnd;
}

static BOOL ShowSndVolModernIndicator() {
    if (bSndVolModernLaunched) return TRUE;
    HWND hExisting = GetOpenSndVolModernIndicatorWnd();
    if (hExisting) return TRUE;
    HWND hForegroundWnd = GetForegroundWindow();
    if (hForegroundWnd && hForegroundWnd != g_hTaskbarWnd) {
        hSndVolModernPreviousForegroundWnd = hForegroundWnd;
    }
    HWND hSndVolTrayControlWnd = GetSndVolTrayControlWnd();
    if (!hSndVolTrayControlWnd) return FALSE;
    if (!PostMessage(hSndVolTrayControlWnd, 0x460, 0,
                     MAKELPARAM(NIN_SELECT, 100))) {
        return FALSE;
    }
    bSndVolModernLaunched = TRUE;
    return TRUE;
}

static BOOL HideSndVolModernIndicator() {
    HWND hSndVolModernIndicatorWnd = GetOpenSndVolModernIndicatorWnd();
    if (hSndVolModernIndicatorWnd) {
        if (!hSndVolModernPreviousForegroundWnd ||
            !SetForegroundWindow(hSndVolModernPreviousForegroundWnd)) {
            SetForegroundWindow(g_hTaskbarWnd);
        }
    }
    return TRUE;
}

static void EndSndVolModernIndicatorSession() {
    hSndVolModernPreviousForegroundWnd = NULL;
    bSndVolModernLaunched = FALSE;
    bSndVolModernAppeared = FALSE;
}

static BOOL ValidateSndVolProcess() {
    if (!hSndVolProcess) return FALSE;
    if (WaitForSingleObject(hSndVolProcess, 0) != WAIT_TIMEOUT) {
        CloseHandle(hSndVolProcess);
        hSndVolProcess = NULL;
        hSndVolWnd = NULL;
        return FALSE;
    }
    return TRUE;
}

static BOOL ValidateSndVolWnd() {
    HWND hForegroundWnd = GetForegroundWindow();
    if (hSndVolWnd == hForegroundWnd) return TRUE;
    DWORD dwProcessId;
    GetWindowThreadProcessId(hForegroundWnd, &dwProcessId);
    if (GetProcessId(hSndVolProcess) == dwProcessId) {
        WCHAR szClass[sizeof("#32770") + 1];
        GetClassName(hForegroundWnd, szClass, sizeof("#32770") + 1);
        if (lstrcmp(szClass, L"#32770") == 0) {
            hSndVolWnd = hForegroundWnd;
            return TRUE;
        }
    }
    hSndVolWnd = NULL;
    return FALSE;
}

static BOOL CALLBACK EnumThreadFindSndVolWnd(HWND hWnd, LPARAM lParam) {
    WCHAR szClass[16];
    GetClassName(hWnd, szClass, _countof(szClass));
    if (lstrcmp(szClass, L"#32770") == 0) {
        *(HWND*)lParam = hWnd;
        return FALSE;
    }
    return TRUE;
}

static HWND GetSndVolDlg(HWND hVolumeAppWnd) {
    HWND hWnd = NULL;
    EnumThreadWindows(GetWindowThreadProcessId(hVolumeAppWnd, NULL),
                      EnumThreadFindSndVolWnd, (LPARAM)&hWnd);
    return hWnd;
}

static BOOL IsSndVolWndInitialized(HWND hWnd) {
    HWND hChildDlg = FindWindowEx(hWnd, NULL, L"#32770", NULL);
    if (!hChildDlg) return FALSE;
    if (!(GetWindowLong(hChildDlg, GWL_STYLE) & WS_VISIBLE)) return FALSE;
    return TRUE;
}

static BOOL MoveSndVolCenterMouse(HWND hWnd) {
    NOTIFYICONIDENTIFIER notifyiconidentifier = {sizeof(NOTIFYICONIDENTIFIER)};
    memcpy(&notifyiconidentifier.guidItem,
           "\x73\xAE\x20\x78\xE3\x23\x29\x42\x82\xC1\xE4\x1C\xB6\x7D\x5B\x9C",
           sizeof(GUID));
    RECT rcExclude;
    if (Shell_NotifyIconGetRect(&notifyiconidentifier, &rcExclude) != S_OK) {
        SetRectEmpty(&rcExclude);
    }

    POINT pt;
    GetCursorPos(&pt);
    RECT rc;
    GetWindowRect(hWnd, &rc);

    int nInflate = 0;
    BOOL bCompositionEnabled;
    if (DwmIsCompositionEnabled(&bCompositionEnabled) == S_OK &&
        bCompositionEnabled) {
        memcpy(
            &notifyiconidentifier.guidItem,
            "\x43\x65\x4B\x96\xAD\xBB\xEE\x44\x84\x8A\x3A\x95\xD8\x59\x51\xEA",
            sizeof(GUID));
        RECT rcInflate;
        if (Shell_NotifyIconGetRect(&notifyiconidentifier, &rcInflate) ==
            S_OK) {
            nInflate = rcInflate.bottom - rcInflate.top;
            InflateRect(&rc, nInflate, nInflate);
        }
    }

    SIZE size = {rc.right - rc.left, rc.bottom - rc.top};
    if (!CalculatePopupWindowPosition(
            &pt, &size,
            TPM_CENTERALIGN | TPM_VCENTERALIGN | TPM_VERTICAL | TPM_WORKAREA,
            &rcExclude, &rc)) {
        return FALSE;
    }
    SetWindowPos(hWnd, NULL, rc.left + nInflate, rc.top + nInflate, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
    return TRUE;
}

static BOOL OpenScrollSndVolInternal(WPARAM wParam,
                                     LPARAM lMousePosParam,
                                     HWND hVolumeAppWnd,
                                     BOOL* pbOpened) {
    HWND hSndVolDlg = GetSndVolDlg(hVolumeAppWnd);
    if (hSndVolDlg) {
        if (GetWindowTextLength(hSndVolDlg) == 0) {
            if (IsSndVolWndInitialized(hSndVolDlg) &&
                MoveSndVolCenterMouse(hSndVolDlg)) {
                if (g_nExplorerVersion <= WIN_VERSION_7) {
                    SendMessage(g_hTaskbarWnd, WM_USER + 12, 0, 0);
                }
                SetForegroundWindow(hVolumeAppWnd);
                PostMessage(hVolumeAppWnd, WM_USER + 35, 0, 0);
                *pbOpened = TRUE;
                return TRUE;
            }
        } else if (IsWindowVisible(hSndVolDlg)) {
            if (g_nExplorerVersion <= WIN_VERSION_7) {
                SendMessage(g_hTaskbarWnd, WM_USER + 12, 0, 0);
            }
            SetForegroundWindow(hVolumeAppWnd);
            PostMessage(hVolumeAppWnd, WM_USER + 35, 0, 0);
            *pbOpened = FALSE;
            return TRUE;
        }
    }
    return FALSE;
}

static void CALLBACK CloseSndVolTimerProc(HWND hWnd,
                                          UINT uMsg,
                                          UINT_PTR idEvent,
                                          DWORD dwTime) {
    if (CanUseModernIndicator()) {
        HWND hSndVolModernIndicatorWnd = GetOpenSndVolModernIndicatorWnd();
        if (!bSndVolModernAppeared) {
            if (hSndVolModernIndicatorWnd) {
                bSndVolModernAppeared = TRUE;
                nCloseSndVolTimerCount = 1;
                if (g_nWinVersion >= WIN_VERSION_11_21H2) {
                    PostMessage(hSndVolModernIndicatorWnd, WM_ACTIVATE,
                                MAKEWPARAM(WA_INACTIVE, FALSE), 0);
                }
                return;
            } else {
                nCloseSndVolTimerCount++;
                if (nCloseSndVolTimerCount < 10) return;
            }
        } else {
            if (hSndVolModernIndicatorWnd) {
                POINT pt;
                GetCursorPos(&pt);
                HWND hPointWnd = GetAncestor(WindowFromPoint(pt), GA_ROOT);
                if (!hPointWnd) nCloseSndVolTimerCount++;
                else if (hPointWnd == hSndVolModernIndicatorWnd)
                    nCloseSndVolTimerCount = 0;
                else if (IsTaskbarWindow(hPointWnd) &&
                         IsPointInsideAnyScrollArea(hPointWnd, pt))
                    nCloseSndVolTimerCount = 0;
                else
                    nCloseSndVolTimerCount++;
                if (nCloseSndVolTimerCount < 10) return;
                HideSndVolModernIndicator();
            }
        }
        EndSndVolModernIndicatorSession();
    } else {
        if (ValidateSndVolProcess()) {
            if (WaitForInputIdle(hSndVolProcess, 0) != 0) return;
            if (ValidateSndVolWnd()) {
                POINT pt;
                GetCursorPos(&pt);
                HWND hPointWnd = GetAncestor(WindowFromPoint(pt), GA_ROOT);
                if (!hPointWnd) nCloseSndVolTimerCount++;
                else if (hPointWnd == hSndVolWnd)
                    nCloseSndVolTimerCount = 0;
                else if (IsTaskbarWindow(hPointWnd) &&
                         IsPointInsideAnyScrollArea(hPointWnd, pt))
                    nCloseSndVolTimerCount = 0;
                else
                    nCloseSndVolTimerCount++;
                if (nCloseSndVolTimerCount < 10) return;
                if (hPointWnd != hSndVolWnd) {
                    PostMessage(hSndVolWnd, WM_ACTIVATE,
                                MAKEWPARAM(WA_INACTIVE, FALSE), (LPARAM)NULL);
                }
            }
        }
    }

    KillTimer(NULL, nCloseSndVolTimer);
    nCloseSndVolTimer = 0;
}

void SetSndVolTimer() {
    nCloseSndVolTimer =
        SetTimer(NULL, nCloseSndVolTimer, 100, CloseSndVolTimerProc);
    nCloseSndVolTimerCount = 0;
}

void KillSndVolTimer() {
    if (nCloseSndVolTimer) {
        KillTimer(NULL, nCloseSndVolTimer);
        nCloseSndVolTimer = 0;
    }
}

void CleanupSndVol() {
    KillSndVolTimer();
    if (hSndVolProcess) {
        CloseHandle(hSndVolProcess);
        hSndVolProcess = NULL;
        hSndVolWnd = NULL;
    }
}

BOOL OpenScrollSndVol(WPARAM wParam, LPARAM lMousePosParam) {
    if (g_settings.volumeIndicator == VolumeIndicator::Win11 &&
        g_nWinVersion >= WIN_VERSION_11_22H2) {
        return Win11IndicatorAdjustVolumeLevelWithMouseWheel(
            GET_WHEEL_DELTA_WPARAM(wParam));
    }

    if (g_settings.volumeIndicator == VolumeIndicator::None) {
        return AdjustVolumeLevelWithMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam),
                                               0);
    }

    if (CanUseModernIndicator()) {
        if (!AdjustVolumeLevelWithMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam), 0))
            return FALSE;
        ShowSndVolModernIndicator();
        SetSndVolTimer();
        return TRUE;
    }

    if (!IsDefaultAudioEndpointAvailable()) return FALSE;
    if (!AdjustVolumeLevelWithMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam), 0))
        return FALSE;

    if (ValidateSndVolProcess()) {
        if (WaitForInputIdle(hSndVolProcess, 0) == 0) {
            if (ValidateSndVolWnd()) return FALSE;
            HWND hVolumeAppWnd = FindWindow(L"Windows Volume App Window",
                                            L"Windows Volume App Window");
            if (hVolumeAppWnd) {
                DWORD dwProcessId;
                GetWindowThreadProcessId(hVolumeAppWnd, &dwProcessId);
                if (GetProcessId(hSndVolProcess) == dwProcessId) {
                    BOOL bOpened;
                    if (OpenScrollSndVolInternal(wParam, lMousePosParam,
                                                 hVolumeAppWnd, &bOpened)) {
                        if (bOpened) SetSndVolTimer();
                        return bOpened;
                    }
                }
            }
        }
        return FALSE;
    }

    HANDLE hMutex =
        OpenMutex(SYNCHRONIZE, FALSE, L"Windows Volume App Window");
    if (hMutex) {
        CloseHandle(hMutex);
        HWND hVolumeAppWnd = FindWindow(L"Windows Volume App Window",
                                        L"Windows Volume App Window");
        if (hVolumeAppWnd) {
            DWORD dwProcessId;
            GetWindowThreadProcessId(hVolumeAppWnd, &dwProcessId);
            hSndVolProcess =
                OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE,
                            FALSE, dwProcessId);
            if (hSndVolProcess) {
                if (WaitForInputIdle(hSndVolProcess, 0) == 0) {
                    if (ValidateSndVolWnd()) return FALSE;
                    BOOL bOpened;
                    if (OpenScrollSndVolInternal(wParam, lMousePosParam,
                                                 hVolumeAppWnd, &bOpened)) {
                        if (bOpened) SetSndVolTimer();
                        return bOpened;
                    }
                }
            }
        }
        return FALSE;
    }

    WCHAR szCommandLine[sizeof("SndVol.exe -f 4294967295")];
    wsprintf(szCommandLine, L"SndVol.exe -f %u", (DWORD)lMousePosParam);
    STARTUPINFO si = {sizeof(STARTUPINFO)};
    PROCESS_INFORMATION pi;
    if (!CreateProcess(NULL, szCommandLine, NULL, NULL, FALSE,
                       ABOVE_NORMAL_PRIORITY_CLASS | CREATE_SUSPENDED, NULL,
                       NULL, &si, &pi))
        return FALSE;

    if (g_nExplorerVersion <= WIN_VERSION_7) {
        SendMessage(g_hTaskbarWnd, WM_USER + 12, 0, 0);
    }
    AllowSetForegroundWindow(pi.dwProcessId);
    ResumeThread(pi.hThread);
    CloseHandle(pi.hThread);
    hSndVolProcess = pi.hProcess;
    SetSndVolTimer();
    return TRUE;
}

// -----------------------------------------------------------------------------
// Custom OSD overlay
//
// Renders a small flyout near the bottom-center of the cursor's monitor when
// adjusting brightness, SDR-on-HDR brightness, or microphone volume - actions
// for which Windows offers no first-party indicator (or, in the mic case,
// when the native one fails). The overlay is a layered, click-through,
// topmost window managed on the taskbar thread and auto-hides after a short
// timeout. Drawn with plain GDI to keep dependencies minimal.
// -----------------------------------------------------------------------------

namespace OsdOverlay {

enum class Kind {
    None,
    Brightness,         // 0..100 percent, sun icon
    SdrToHdrBrightness, // 1.0..maxSdrBrightness mapped to 0..100, brightness+ icon
    MicVolume,          // 0..100 percent, mic icon (fallback)
};

constexpr int kWindowWidthDip = 300;
constexpr int kWindowHeightDip = 68;
constexpr int kHideTimeoutMs = 1500;
constexpr int kBottomMarginDip = 80;  // distance from bottom edge of monitor
constexpr UINT_PTR kHideTimerId = 0xB401;
constexpr BYTE kWindowAlpha = 235;    // ~92% opaque, like Win11 OSD

static PCWSTR kClassName = L"WindhawkTaskbarExtraOsdOverlay";

struct State {
    HWND hWnd = nullptr;
    ATOM classAtom = 0;
    Kind kind = Kind::None;
    double valueNorm = 0.0;      // 0..1 for the bar
    int valueDisplay = 0;         // numeric label
    UINT_PTR hideTimer = 0;
};
static State g;

static WCHAR GetIconChar(Kind kind) {
    switch (kind) {
        case Kind::Brightness:         return 0xE706;  // Brightness sun
        case Kind::SdrToHdrBrightness: return 0xE7B5;  // Brightness/contrast
        case Kind::MicVolume:          return 0xE720;  // Microphone
        default:                       return L'?';
    }
}

static PCWSTR GetLabel(Kind kind) {
    switch (kind) {
        case Kind::SdrToHdrBrightness: return L"HDR/SDR";
        default:                       return nullptr;
    }
}

static HFONT CreateIconFont(int sizePx) {
    // Try Segoe Fluent Icons first (Win11), then fall back to MDL2 (Win10).
    HFONT hFont = CreateFontW(-sizePx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                              CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                              DEFAULT_PITCH, L"Segoe Fluent Icons");
    if (!hFont) {
        hFont = CreateFontW(-sizePx, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                            CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                            DEFAULT_PITCH, L"Segoe MDL2 Assets");
    }
    return hFont;
}

static HFONT CreateLabelFont(int sizePx, bool bold) {
    return CreateFontW(-sizePx, 0, 0, 0, bold ? FW_SEMIBOLD : FW_NORMAL, FALSE,
                       FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                       CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH,
                       L"Segoe UI Variable");
}

static void Paint(HWND hWnd) {
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);
    int w = rcClient.right - rcClient.left;
    int h = rcClient.bottom - rcClient.top;
    UINT dpi = GetDpiForWindowWithFallback(hWnd);

    PAINTSTRUCT ps;
    HDC hdcWnd = BeginPaint(hWnd, &ps);

    // Double-buffer to avoid flicker on repaint.
    HDC hdc = CreateCompatibleDC(hdcWnd);
    HBITMAP hbm = CreateCompatibleBitmap(hdcWnd, w, h);
    HBITMAP hbmOld = (HBITMAP)SelectObject(hdc, hbm);

    // Background: solid dark, slightly inset border for a subtle frame.
    HBRUSH brBg = CreateSolidBrush(RGB(32, 32, 32));
    RECT rcBg = {0, 0, w, h};
    FillRect(hdc, &rcBg, brBg);
    DeleteObject(brBg);

    HPEN penBorder = CreatePen(PS_SOLID, 1, RGB(60, 60, 60));
    HPEN penOld = (HPEN)SelectObject(hdc, penBorder);
    HBRUSH brHollow = (HBRUSH)GetStockObject(NULL_BRUSH);
    HBRUSH brOld = (HBRUSH)SelectObject(hdc, brHollow);
    Rectangle(hdc, 0, 0, w, h);
    SelectObject(hdc, brOld);
    SelectObject(hdc, penOld);
    DeleteObject(penBorder);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(240, 240, 240));

    // Layout: [icon | content], where content is [value text]\n[bar].
    int iconBoxW = MulDiv(64, dpi, 96);
    int padding = MulDiv(12, dpi, 96);

    // Icon.
    {
        int iconSize = MulDiv(28, dpi, 96);
        HFONT hFont = CreateIconFont(iconSize);
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
        WCHAR ch = GetIconChar(g.kind);
        RECT rcIcon = {0, 0, iconBoxW, h};
        DrawTextW(hdc, &ch, 1, &rcIcon,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);
    }

    int contentLeft = iconBoxW;
    int contentRight = w - padding;
    int contentW = contentRight - contentLeft;

    // Value label (percentage or numeric).
    int textH = MulDiv(20, dpi, 96);
    int labelY = MulDiv(10, dpi, 96);
    {
        HFONT hFont = CreateLabelFont(textH, true);
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
        WCHAR buf[32];
        PCWSTR label = GetLabel(g.kind);
        if (label) {
            swprintf_s(buf, _countof(buf), L"%s  %d%%", label, g.valueDisplay);
        } else {
            swprintf_s(buf, _countof(buf), L"%d%%", g.valueDisplay);
        }
        RECT rcLabel = {contentLeft, labelY, contentRight, labelY + textH + 4};
        DrawTextW(hdc, buf, -1, &rcLabel,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);
    }

    // Progress bar.
    int barH = MulDiv(6, dpi, 96);
    int barY = h - MulDiv(16, dpi, 96) - barH;
    int barW = contentW;
    {
        // Track.
        HBRUSH brTrack = CreateSolidBrush(RGB(72, 72, 72));
        RECT rcTrack = {contentLeft, barY, contentLeft + barW, barY + barH};
        FillRect(hdc, &rcTrack, brTrack);
        DeleteObject(brTrack);

        // Fill.
        int fillW = (int)(barW * std::clamp(g.valueNorm, 0.0, 1.0));
        if (fillW > 0) {
            HBRUSH brFill = CreateSolidBrush(RGB(230, 230, 230));
            RECT rcFill = {contentLeft, barY, contentLeft + fillW, barY + barH};
            FillRect(hdc, &rcFill, brFill);
            DeleteObject(brFill);
        }
    }

    // Blit to screen.
    BitBlt(hdcWnd, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

    SelectObject(hdc, hbmOld);
    DeleteObject(hbm);
    DeleteDC(hdc);
    EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT:
            Paint(hWnd);
            return 0;
        case WM_TIMER:
            if (wParam == kHideTimerId) {
                KillTimer(hWnd, kHideTimerId);
                g.hideTimer = 0;
                ShowWindow(hWnd, SW_HIDE);
                return 0;
            }
            break;
        case WM_NCHITTEST:
            // Make the window click-through belt-and-braces (we already have
            // WS_EX_TRANSPARENT, but some input paths bypass that).
            return HTTRANSPARENT;
        case WM_ERASEBKGND:
            return 1;  // Painted entirely in WM_PAINT.
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static void EnsureWindowCreated() {
    if (g.hWnd && IsWindow(g.hWnd)) return;
    g.hWnd = nullptr;

    if (!g.classAtom) {
        WNDCLASSEX wcex = {sizeof(wcex)};
        wcex.lpfnWndProc = WndProc;
        wcex.hInstance = GetModuleHandle(nullptr);
        wcex.lpszClassName = kClassName;
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = nullptr;
        g.classAtom = RegisterClassEx(&wcex);
        if (!g.classAtom) {
            if (GetLastError() == ERROR_CLASS_ALREADY_EXISTS) {
                g.classAtom = (ATOM)1;  // sentinel: class already registered
            } else {
                Wh_Log(L"OSD: failed to register class (err=%lu)",
                       GetLastError());
                return;
            }
        }
    }
    g.hWnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT |
            WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kClassName, L"", WS_POPUP, 0, 0, kWindowWidthDip, kWindowHeightDip,
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
    if (g.hWnd) {
        SetLayeredWindowAttributes(g.hWnd, 0, kWindowAlpha, LWA_ALPHA);
    } else {
        Wh_Log(L"OSD: failed to create window (err=%lu)", GetLastError());
    }
}

void Show(Kind kind, double valueNorm, int valueDisplay) {
    if (!g_settings.customOsdEnabled) return;

    EnsureWindowCreated();
    if (!g.hWnd) return;

    g.kind = kind;
    g.valueNorm = std::clamp(valueNorm, 0.0, 1.0);
    g.valueDisplay = valueDisplay;

    // Position at bottom-center of the monitor under the cursor, above
    // where the taskbar would be (or just above the bottom edge if no
    // taskbar is shown). This roughly mirrors the Win11 OSD placement.
    POINT pt;
    GetCursorPos(&pt);
    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {sizeof(mi)};
    int x = 0, y = 0, w, h;
    UINT dpi = GetDpiForWindowWithFallback(g.hWnd);
    w = MulDiv(kWindowWidthDip, dpi, 96);
    h = MulDiv(kWindowHeightDip, dpi, 96);
    if (GetMonitorInfo(hMon, &mi)) {
        int margin = MulDiv(kBottomMarginDip, dpi, 96);
        x = mi.rcMonitor.left +
            (mi.rcMonitor.right - mi.rcMonitor.left - w) / 2;
        y = mi.rcMonitor.bottom - h - margin;
    }
    SetWindowPos(g.hWnd, HWND_TOPMOST, x, y, w, h,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(g.hWnd, nullptr, TRUE);

    if (g.hideTimer) KillTimer(g.hWnd, kHideTimerId);
    g.hideTimer = SetTimer(g.hWnd, kHideTimerId, kHideTimeoutMs, nullptr);
}

void Hide() {
    if (!g.hWnd) return;
    if (g.hideTimer) {
        KillTimer(g.hWnd, kHideTimerId);
        g.hideTimer = 0;
    }
    ShowWindow(g.hWnd, SW_HIDE);
}

void Cleanup() {
    if (g.hWnd) {
        if (g.hideTimer) {
            KillTimer(g.hWnd, kHideTimerId);
            g.hideTimer = 0;
        }
        DestroyWindow(g.hWnd);
        g.hWnd = nullptr;
    }
    if (g.classAtom) {
        UnregisterClass(kClassName, GetModuleHandle(nullptr));
        g.classAtom = 0;
    }
}

}  // namespace OsdOverlay

// -----------------------------------------------------------------------------
// Dim overlay — software dimming below hardware minimum brightness.
//
// Renders a fullscreen black, click-through, topmost layered window when
// the user scrolls brightness below the hardware floor (0% or the configured
// minimum). On OLED laptops this means deeper blacks, less burn-in, and
// lower power draw; on IPS/VA panels it kills the eye-searing glow at night
// when even 0% hardware brightness is still too bright.
//
// The overlay alpha animates smoothly (~60 FPS via a timer on g_hTaskbarWnd)
// and auto-hides completely when brightness rises back above the floor.
// -----------------------------------------------------------------------------

namespace DimOverlay {

PCWSTR kDimClassName = L"WhTaskbarExtraDimOverlay";
HWND g_hDimWnd;
ATOM g_dimClassAtom;
int g_dimAlphaTarget;    // 0..maxOverlayAlpha
float g_dimAlphaSmooth;  // lerped towards target

void EnsureDimWindowCreated() {
    if (g_hDimWnd && IsWindow(g_hDimWnd)) return;
    g_hDimWnd = nullptr;

    if (!g_dimClassAtom) {
        WNDCLASSEX wcex = {sizeof(wcex)};
        wcex.lpfnWndProc = DefWindowProc;
        wcex.hInstance = GetModuleHandle(nullptr);
        wcex.lpszClassName = kDimClassName;
        wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        g_dimClassAtom = RegisterClassEx(&wcex);
        if (!g_dimClassAtom) {
            if (GetLastError() == ERROR_CLASS_ALREADY_EXISTS) {
                g_dimClassAtom = (ATOM)1;
            } else {
                Wh_Log(L"DimOverlay: failed to register class (err=%lu)",
                       GetLastError());
                return;
            }
        }
    }

    g_hDimWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED |
            WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kDimClassName, L"", WS_POPUP,
        GetSystemMetrics(SM_XVIRTUALSCREEN),
        GetSystemMetrics(SM_YVIRTUALSCREEN),
        GetSystemMetrics(SM_CXVIRTUALSCREEN),
        GetSystemMetrics(SM_CYVIRTUALSCREEN),
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

    if (g_hDimWnd) {
        SetLayeredWindowAttributes(g_hDimWnd, 0, 0, LWA_ALPHA);
        ShowWindow(g_hDimWnd, SW_SHOWNOACTIVATE);
    } else {
        Wh_Log(L"DimOverlay: failed to create window (err=%lu)",
               GetLastError());
    }
}

void SetDimAlpha(int alpha) {
    if (!g_settings.softwareDimmingEnabled) return;
    g_dimAlphaTarget = std::clamp(alpha, 0, g_settings.maxOverlayAlpha);
}

void TickAnimation() {
    if (!g_hDimWnd || !IsWindow(g_hDimWnd)) {
        if (g_dimAlphaTarget > 0) EnsureDimWindowCreated();
        if (!g_hDimWnd || !IsWindow(g_hDimWnd)) return;
    }

    float t = (float)g_dimAlphaTarget;
    float d = t - g_dimAlphaSmooth;
    if (d > 0.5f || d < -0.5f) {
        g_dimAlphaSmooth += d * 0.25f;
    } else if (g_dimAlphaSmooth != t) {
        g_dimAlphaSmooth = t;
    } else {
        // Idle — hide window when fully transparent to save resources.
        if (g_dimAlphaTarget == 0 && g_dimAlphaSmooth < 0.5f) {
            ShowWindow(g_hDimWnd, SW_HIDE);
        }
        return;
    }

    ShowWindow(g_hDimWnd, SW_SHOWNOACTIVATE);
    SetLayeredWindowAttributes(g_hDimWnd, 0,
                               (BYTE)(g_dimAlphaSmooth + 0.5f), LWA_ALPHA);

    // Bring back to topmost periodically (other apps may steal this).
    SetWindowPos(g_hDimWnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void Cleanup() {
    if (g_hDimWnd) {
        DestroyWindow(g_hDimWnd);
        g_hDimWnd = nullptr;
    }
    if (g_dimClassAtom) {
        UnregisterClass(kDimClassName, GetModuleHandle(nullptr));
        g_dimClassAtom = 0;
    }
    g_dimAlphaTarget = 0;
    g_dimAlphaSmooth = 0.0f;
}

}  // namespace DimOverlay

// -----------------------------------------------------------------------------
// Scroll action dispatch
// -----------------------------------------------------------------------------

DWORD g_lastScrollTime;
int g_lastScrollDeltaRemainder;
DWORD g_lastActionTime;
int g_lastScrollActionIndex = -1;

// Dim-overlay helper. Dim overlay is orthogonal to hardware brightness:
// scrolling below 0% keeps hardware at 0 and darkens the overlay instead.
// Scrolling up from a dimmed state reduces overlay before raising hardware.
int ApplyBrightnessWithDim(HWND hWnd, int clicks) {
    if (!g_settings.softwareDimmingEnabled) {
        int newVal = -1;
        AdjustBrightness(hWnd, clicks, &newVal);
        return newVal >= 0 ? newVal : -1;
    }

    int newHwVal = -1;
    int curDim = DimOverlay::g_dimAlphaTarget;
    int dimStep = std::max(1, g_settings.maxOverlayAlpha / 20);

    if (curDim > 0 && clicks > 0) {
        // Scrolling up while dimmed: reduce overlay first.
        int dimReduction = clicks * dimStep;
        int newDim = curDim - dimReduction;
        if (newDim < 0) newDim = 0;
        DimOverlay::SetDimAlpha(newDim);
        int dimClicksConsumed = (curDim - newDim + dimStep - 1) / dimStep;
        int remainingClicks = clicks - dimClicksConsumed;
        if (remainingClicks > 0 && newDim == 0) {
            AdjustBrightness(hWnd, remainingClicks, &newHwVal);
            g_lastBrightnessInputMs = GetTickCount();
        } else {
            newHwVal = 0;
        }
    } else {
        AdjustBrightness(hWnd, clicks, &newHwVal);
        g_lastBrightnessInputMs = GetTickCount();
    }

    if (newHwVal == 0 && clicks < 0 && g_settings.softwareDimmingEnabled) {
        // At hardware minimum and still scrolling down: apply dim overlay.
        int newDim = std::min(curDim + (-clicks) * dimStep,
                              g_settings.maxOverlayAlpha);
        DimOverlay::SetDimAlpha(newDim);
    }

    if (newHwVal > 0) {
        DimOverlay::SetDimAlpha(0);
    }

    return newHwVal >= 0 ? newHwVal : -1;
}

// Reset dim overlay if brightness changed externally (Windows slider, etc.).
void MaybeSyncDimOverlay() {
    if (DimOverlay::g_dimAlphaTarget == 0) return;
    DWORD now = GetTickCount();
    if (now - g_lastBrightnessWriteMs < 5000) return;
    if (now - g_lastBrightnessInputMs < 8000) return;

    int real = ReadBrightnessIoctl();
    if (real < 0) real = GetBrightnessWmi();
    if (real > 0) {
        // Brightness was raised externally — clear the dim overlay.
        DimOverlay::SetDimAlpha(0);
    }
}

// Dispatches a single scroll event to one configured action.
//   entryIndex: which entry in g_settings.scrollActions to run.
//   suppressIndicator: when true (used for full-screen "without indicator"
//                      mode), skip showing volume/brightness/mic OSDs.
void InvokeScrollAction(HWND hWnd,
                        WPARAM wParam,
                        LPARAM lMousePosParam,
                        int entryIndex,
                        bool suppressIndicator = false) {
    if (entryIndex < 0 ||
        entryIndex >= (int)g_settings.scrollActions.size()) {
        return;
    }
    const auto& entry = g_settings.scrollActions[entryIndex];

    // Reset the wheel accumulator if we're now dispatching to a different
    // entry than last time - otherwise a fast switch between actions can
    // carry over a stale fractional delta.
    if (entryIndex != g_lastScrollActionIndex) {
        g_lastScrollTime = 0;
        g_lastScrollDeltaRemainder = 0;
        g_lastActionTime = 0;
        g_lastScrollActionIndex = entryIndex;
    }

    int delta = GET_WHEEL_DELTA_WPARAM(wParam) * entry.scrollStep;
    if (entry.reverseScrollingDirection) delta = -delta;

    // Carry over fractional clicks from the previous scroll if it happened
    // recently enough - matches Windows' own wheel accumulation behaviour.
    if (GetTickCount() - g_lastScrollTime <
        Constants::kWheelAccumulationWindowMs) {
        delta += g_lastScrollDeltaRemainder;
    }

    int clicks = delta / WHEEL_DELTA;
    Wh_Log(L"entry[%d] %d clicks (delta=%d, suppress=%d)", entryIndex, clicks,
           delta, suppressIndicator ? 1 : 0);

    if (clicks != 0 && entry.throttleMs > 0) {
        // Throttling: drop the event entirely if too soon after the last
        // one, or collapse a multi-step burst to a single step otherwise.
        if (GetTickCount() - g_lastActionTime < (DWORD)entry.throttleMs) {
            clicks = 0;
            delta = 0;
        } else if (clicks < -1 || clicks > 1) {
            clicks = clicks > 0 ? 1 : -1;
            delta = 0;
        }
    }

    if (clicks != 0) {
        HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        bool showOsd = !suppressIndicator && g_settings.customOsdEnabled;

        switch (entry.scrollAction) {
            case ScrollAction::virtualDesktopSwitch:
                SwitchDesktopViaKeyboardShortcut(clicks);
                break;

            case ScrollAction::brightnessChange: {
                int displayVal = ApplyBrightnessWithDim(hWnd, clicks);
                if (showOsd && displayVal >= 0) {
                    OsdOverlay::Show(OsdOverlay::Kind::Brightness,
                                     displayVal / 100.0, displayVal);
                }
                break;
            }

            case ScrollAction::sdrToHdrBrightnessChange: {
                if (AdjustSdrToHdrBrightness(clicks, hMonitor) && showOsd) {
                    double range = g_settings.maxSdrBrightness -
                                   Constants::kSdrMinBrightness;
                    double norm =
                        (range > 0.0001)
                            ? (g_currentSdrToHdrBrightness -
                               Constants::kSdrMinBrightness) /
                                  range
                            : 0.0;
                    OsdOverlay::Show(OsdOverlay::Kind::SdrToHdrBrightness, norm,
                                     (int)(norm * 100));
                }
                break;
            }

            case ScrollAction::masterBrightnessChange: {
                bool isHdr = hMonitor && IsMonitorHdrEnabled(hMonitor);
                Wh_Log(L"Master brightness on monitor %p (HDR: %s)", hMonitor,
                       isHdr ? L"yes" : L"no");
                if (isHdr) {
                    if (AdjustSdrToHdrBrightness(clicks, hMonitor) && showOsd) {
                        double range = g_settings.maxSdrBrightness -
                                       Constants::kSdrMinBrightness;
                        double norm =
                            (range > 0.0001)
                                ? (g_currentSdrToHdrBrightness -
                                   Constants::kSdrMinBrightness) /
                                      range
                                : 0.0;
                        OsdOverlay::Show(OsdOverlay::Kind::SdrToHdrBrightness,
                                         norm, (int)(norm * 100));
                    }
                } else {
                    int displayVal = ApplyBrightnessWithDim(hWnd, clicks);
                    if (showOsd && displayVal >= 0) {
                        OsdOverlay::Show(OsdOverlay::Kind::Brightness,
                                         displayVal / 100.0, displayVal);
                    }
                }
                break;
            }

            case ScrollAction::micVolumeChange: {
                // Prefer the native Win11 mic OSD (AppCommand path) when
                // available; otherwise change the level directly and show
                // our custom overlay (if enabled / not suppressed).
                bool usedNative = false;
                if (!suppressIndicator && CanUseNativeMicIndicator()) {
                    usedNative = Win11IndicatorAdjustMicLevelWithMouseWheel(
                        (short)(clicks * WHEEL_DELTA));
                }
                if (!usedNative) {
                    if (AddMicMasterVolumeLevelScalar(clicks * 0.01f)) {
                        Wh_Log(L"Changed microphone volume by %d%%", clicks);
                        if (showOsd) {
                            float v = GetCurrentMicVolumeScalar();
                            if (v >= 0.0f) {
                                OsdOverlay::Show(OsdOverlay::Kind::MicVolume,
                                                 v, (int)(v * 100.0f + 0.5f));
                            }
                        }
                    } else {
                        Wh_Log(L"Error changing microphone volume");
                    }
                }
                break;
            }

            case ScrollAction::volumeChange:
                // Re-pack wheel delta for OpenScrollSndVol (uses
                // GET_WHEEL_DELTA_WPARAM); convert clicks back to a delta.
                if (suppressIndicator) {
                    // Caller wants "fullScreenScrolling: withoutIndicator"
                    // behavior - change the level silently.
                    AdjustVolumeLevelWithMouseWheel(
                        (short)(clicks * WHEEL_DELTA), 0);
                } else {
                    OpenScrollSndVol(
                        MAKEWPARAM(0, (SHORT)(clicks * WHEEL_DELTA)),
                        lMousePosParam);
                }
                break;
        }
        g_lastActionTime = GetTickCount();
    }

    g_lastScrollTime = GetTickCount();
    g_lastScrollDeltaRemainder = delta % WHEEL_DELTA;
}

// -----------------------------------------------------------------------------
// Window message handlers
// -----------------------------------------------------------------------------

bool OnMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    if (GetCapture()) return false;

    POINT pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);

    for (int i = 0; i < (int)g_settings.scrollActions.size(); i++) {
        const auto& entry = g_settings.scrollActions[i];
        if (!IsPointInsideEntryScrollArea(hWnd, pt, entry)) continue;

        // For system-volume entries with ctrlScrollVolumeChange, require Ctrl.
        if (entry.scrollAction == ScrollAction::volumeChange &&
            g_settings.ctrlScrollVolumeChange &&
            GetKeyState(VK_CONTROL) >= 0) {
            continue;
        }

        // Steal focus from elevated windows.
        INPUT input;
        ZeroMemory(&input, sizeof(INPUT));
        SendInput(1, &input, sizeof(INPUT));

        InvokeScrollAction(hWnd, wParam, lParam, i);
        return true;
    }
    return false;
}

LRESULT CALLBACK TaskbarWindowSubclassProc(_In_ HWND hWnd,
                                           _In_ UINT uMsg,
                                           _In_ WPARAM wParam,
                                           _In_ LPARAM lParam,
                                           _In_ DWORD_PTR dwRefData) {
    LRESULT result = 0;

    switch (uMsg) {
        case WM_COPYDATA: {
            // Used by SndVol to query the volume icon rect for positioning.
            // Patch the response so SndVol respects our command-line position.
            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            typedef struct _notifyiconidentifier_internal {
                DWORD dwMagic;
                DWORD dwRequest;
                DWORD cbSize;
                DWORD hWndHigh;
                DWORD hWndLow;
                UINT uID;
                GUID guidItem;
            } NOTIFYICONIDENTIFIER_INTERNAL;
            COPYDATASTRUCT* p_copydata = (COPYDATASTRUCT*)lParam;
            if (result == 0 && p_copydata->dwData == 0x03 &&
                p_copydata->cbData == sizeof(NOTIFYICONIDENTIFIER_INTERNAL)) {
                NOTIFYICONIDENTIFIER_INTERNAL* p_icon_ident =
                    (NOTIFYICONIDENTIFIER_INTERNAL*)p_copydata->lpData;
                if (p_icon_ident->dwMagic == 0x34753423 &&
                    (p_icon_ident->dwRequest == 0x01 ||
                     p_icon_ident->dwRequest == 0x02) &&
                    p_icon_ident->cbSize == 0x20 &&
                    memcmp(&p_icon_ident->guidItem,
                           "\x73\xAE\x20\x78\xE3\x23\x29\x42\x82\xC1\xE4"
                           "\x1C\xB6\x7D\x5B\x9C",
                           sizeof(GUID)) == 0) {
                    RECT rc;
                    GetWindowRect(hWnd, &rc);
                    if (p_icon_ident->dwRequest == 0x01) {
                        result = MAKEWORD(rc.left, rc.top);
                    } else {
                        result =
                            MAKEWORD(rc.right - rc.left, rc.bottom - rc.top);
                    }
                }
            }
            break;
        }

        case WM_MOUSEWHEEL:
            if (g_nExplorerVersion < WIN_VERSION_11_21H2 &&
                OnMouseWheel(hWnd, wParam, lParam)) {
                result = 0;
            } else {
                result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }
            break;

        case WM_NCDESTROY:
            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            if (hWnd != g_hTaskbarWnd) {
                g_secondaryTaskbarWindows.erase(hWnd);
            }
            break;

        default:
            if (uMsg == g_dispatchEntryMsg) {
                // Routed here from LowLevelMouseProc. wParam packs:
                //   LOWORD: (entryIndex << 1) | suppressIndicatorBit
                //   HIWORD: wheel delta (signed)
                // lParam: MAKELPARAM(cursor.x, cursor.y) - already in the
                // format InvokeScrollAction expects for lMousePosParam.
                WORD lo = LOWORD(wParam);
                int entryIndex = (int)(lo >> 1);
                bool suppress = (lo & 1) != 0;
                short delta = (short)HIWORD(wParam);
                // Repack so GET_WHEEL_DELTA_WPARAM gives back `delta`.
                WPARAM repackedWParam = MAKEWPARAM(0, (WORD)delta);
                InvokeScrollAction(hWnd, repackedWParam, lParam, entryIndex,
                                   suppress);
                result = 0;
            } else if (uMsg == g_scrollAnywhereMsg) {
                // Legacy path - older versions of this mod (or other
                // mods using the same message name) dispatch volume changes
                // here. We preserve the original semantics for compatibility:
                // LOWORD 1 = silent, 0 = with indicator.
                if (LOWORD(wParam) == 1) {
                    AdjustVolumeLevelWithMouseWheel(
                        GET_WHEEL_DELTA_WPARAM(wParam), 0);
                } else {
                    OpenScrollSndVol(wParam, lParam);
                }
                result = 0;
            } else {
                result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }
            break;
    }
    return result;
}

WNDPROC InputSiteWindowProc_Original;
LRESULT CALLBACK InputSiteWindowProc_Hook(HWND hWnd,
                                          UINT uMsg,
                                          WPARAM wParam,
                                          LPARAM lParam) {
    switch (uMsg) {
        case WM_POINTERWHEEL:
            if (HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
                IsTaskbarWindow(hRootWnd) &&
                OnMouseWheel(hRootWnd, wParam, lParam)) {
                return 0;
            }
            break;
    }
    return InputSiteWindowProc_Original(hWnd, uMsg, wParam, lParam);
}

void SubclassTaskbarWindow(HWND hWnd) {
    WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd,
                                                  TaskbarWindowSubclassProc, 0);
}

void UnsubclassTaskbarWindow(HWND hWnd) {
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd,
                                                     TaskbarWindowSubclassProc);
}

void HandleIdentifiedInputSiteWindow(HWND hWnd) {
    if (!g_dwTaskbarThreadId ||
        GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId) {
        return;
    }

    HWND hParentWnd = GetParent(hWnd);
    WCHAR szClassName[64];
    if (!hParentWnd ||
        !GetClassName(hParentWnd, szClassName, ARRAYSIZE(szClassName)) ||
        _wcsicmp(szClassName,
                 L"Windows.UI.Composition.DesktopWindowContentBridge") != 0) {
        return;
    }
    hParentWnd = GetParent(hParentWnd);
    if (!hParentWnd || !IsTaskbarWindow(hParentWnd)) return;

    auto wndProc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
    WindhawkUtils::SetFunctionHook(wndProc, InputSiteWindowProc_Hook,
                                       &InputSiteWindowProc_Original);
    if (g_initialized) {
        Wh_ApplyHookOperations();
    }
    Wh_Log(L"Hooked InputSite wndproc %p", wndProc);
    g_inputSiteProcHooked = true;
}

void HandleIdentifiedTaskbarWindow(HWND hWnd) {
    g_hTaskbarWnd = hWnd;
    g_dwTaskbarThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    AudioInit();
    SubclassTaskbarWindow(hWnd);
    for (HWND hSecondaryWnd : g_secondaryTaskbarWindows) {
        SubclassTaskbarWindow(hSecondaryWnd);
    }
    if (g_nExplorerVersion >= WIN_VERSION_11_21H2 && !g_inputSiteProcHooked) {
        HWND hXamlIslandWnd = FindWindowEx(
            hWnd, nullptr,
            L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
        if (hXamlIslandWnd) {
            HWND hInputSiteWnd =
                FindWindowEx(hXamlIslandWnd, nullptr,
                             L"Windows.UI.Input.InputSite.WindowClass", nullptr);
            if (hInputSiteWnd) HandleIdentifiedInputSiteWindow(hInputSiteWnd);
        }
    }
}

void HandleIdentifiedSecondaryTaskbarWindow(HWND hWnd) {
    if (!g_dwTaskbarThreadId ||
        GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId) {
        return;
    }
    g_secondaryTaskbarWindows.insert(hWnd);
    SubclassTaskbarWindow(hWnd);
    if (g_nExplorerVersion >= WIN_VERSION_11_21H2 && !g_inputSiteProcHooked) {
        HWND hXamlIslandWnd = FindWindowEx(
            hWnd, nullptr,
            L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
        if (hXamlIslandWnd) {
            HWND hInputSiteWnd =
                FindWindowEx(hXamlIslandWnd, nullptr,
                             L"Windows.UI.Input.InputSite.WindowClass", nullptr);
            if (hInputSiteWnd) HandleIdentifiedInputSiteWindow(hInputSiteWnd);
        }
    }
}

HWND FindCurrentProcessTaskbarWindows(
    std::unordered_set<HWND>* secondaryTaskbarWindows) {
    struct ENUM_WINDOWS_PARAM {
        HWND* hWnd;
        std::unordered_set<HWND>* secondaryTaskbarWindows;
    };

    HWND hWnd = nullptr;
    ENUM_WINDOWS_PARAM param = {&hWnd, secondaryTaskbarWindows};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI_LAMBDA_RETURN(BOOL) {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;
            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }
            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                *param.hWnd = hWnd;
            } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                param.secondaryTaskbarWindows->insert(hWnd);
            }
            return TRUE;
        },
        (LPARAM)&param);
    return hWnd;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd) return hWnd;

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (bTextualClassName && _wcsicmp(lpClassName, L"Shell_TrayWnd") == 0) {
        Wh_Log(L"Taskbar window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedTaskbarWindow(hWnd);
    } else if (bTextualClassName &&
               _wcsicmp(lpClassName, L"Shell_SecondaryTrayWnd") == 0) {
        Wh_Log(L"Secondary taskbar window created: %08X",
               (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedSecondaryTaskbarWindow(hWnd);
    }
    return hWnd;
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle,
                                           LPCWSTR lpClassName,
                                           LPCWSTR lpWindowName,
                                           DWORD dwStyle,
                                           int X,
                                           int Y,
                                           int nWidth,
                                           int nHeight,
                                           HWND hWndParent,
                                           HMENU hMenu,
                                           HINSTANCE hInstance,
                                           LPVOID lpParam,
                                           DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
                                    LPCWSTR lpClassName,
                                    LPCWSTR lpWindowName,
                                    DWORD dwStyle,
                                    int X,
                                    int Y,
                                    int nWidth,
                                    int nHeight,
                                    HWND hWndParent,
                                    HMENU hMenu,
                                    HINSTANCE hInstance,
                                    LPVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd) return hWnd;

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"Windows.UI.Input.InputSite.WindowClass") == 0) {
        Wh_Log(L"InputSite window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        if (g_nExplorerVersion >= WIN_VERSION_11_21H2 &&
            !g_inputSiteProcHooked) {
            HandleIdentifiedInputSiteWindow(hWnd);
        }
    }
    return hWnd;
}

// -----------------------------------------------------------------------------
// SndVol.exe / ShellExperienceHost.exe: disable focus-based mouse wheel routing
// -----------------------------------------------------------------------------

using ForceFocusBasedMouseWheelRouting_t = DWORD_PTR(WINAPI*)(BOOL enabled);
ForceFocusBasedMouseWheelRouting_t ForceFocusBasedMouseWheelRouting_Original;
DWORD_PTR WINAPI ForceFocusBasedMouseWheelRouting_Hook(BOOL enabled) {
    Wh_Log(L">");
    return ForceFocusBasedMouseWheelRouting_Original(FALSE);
}

// -----------------------------------------------------------------------------
// Taskbar.View.dll hook for middle-click-to-mute (Win11 22H2+)
// -----------------------------------------------------------------------------

// The return type is int to match the produce<> COM thunk variant below, which
// returns an HRESULT. For the standalone implementation variant (which returns
// void), its caller is a thin thunk that overwrites the return value, so the
// returned value is harmlessly discarded.
using VolumeSystemTrayIconDataModel_OnIconClicked_t =
    int(WINAPI*)(void* pThis, void* param1);
VolumeSystemTrayIconDataModel_OnIconClicked_t
    VolumeSystemTrayIconDataModel_OnIconClicked_Original;
int WINAPI VolumeSystemTrayIconDataModel_OnIconClicked_Hook(void* pThis,
                                                            void* param1) {
    Wh_Log(L">");
    if (g_settings.middleClickToMute && GetKeyState(VK_MBUTTON) < 0) {
        ToggleVolMuted();
        return 0;  // S_OK
    }
    return VolumeSystemTrayIconDataModel_OnIconClicked_Original(pThis, param1);
}

bool HookSystemTraySymbols(HMODULE module) {
    // In SystemTray.dll, the implementation function was inlined into the
    // produce<> COM thunk, so hook that.
    bool isSystemTrayDll = module == GetModuleHandle(L"SystemTray.dll");

    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {isSystemTrayDll
                 ? LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel,struct winrt::SystemTray::IIconDataModel>::OnIconClicked(void *))"
                 : LR"(public: void __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::OnIconClicked(struct winrt::SystemTray::IconClickedEventArgs const &))"},
            &VolumeSystemTrayIconDataModel_OnIconClicked_Original,
            VolumeSystemTrayIconDataModel_OnIconClicked_Hook,
            true,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // Starting with Taskbar.View.dll 2604.8002.200.6000, the SystemTray
            // types moved out of Taskbar.View.dll into SystemTray.dll, so don't
            // hook Taskbar.View.dll at this version and above.
            VS_FIXEDFILEINFO* fixedFileInfo =
                GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor =
                fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                Wh_Log(L"Skipping Taskbar.View.dll version %d", moduleMajor);
                module = nullptr;
            }
        }
    }
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

bool ShouldHookSystemTraySymbols() {
    return g_nWinVersion >= WIN_VERSION_11_22H2 && g_settings.middleClickToMute;
}

void HandleLoadedModuleIfSystemTray(HMODULE module, LPCWSTR lpLibFileName) {
    if (ShouldHookSystemTraySymbols() && !g_systemTrayModuleHooked &&
        GetSystemTrayModuleHandle() == module &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

// -----------------------------------------------------------------------------
// ExplorerPatcher detection
// -----------------------------------------------------------------------------

bool IsExplorerPatcherModule(HMODULE module) {
    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            return false;
    }
    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) return false;
    moduleFileName++;
    if (_wcsnicmp(L"ep_taskbar.", moduleFileName, sizeof("ep_taskbar.") - 1) ==
        0) {
        Wh_Log(L"ExplorerPatcher taskbar module: %s", moduleFileName);
        return true;
    }
    return false;
}

void HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            if (IsExplorerPatcherModule(hMods[i])) {
                if (g_nExplorerVersion >= WIN_VERSION_11_21H2) {
                    g_nExplorerVersion = WIN_VERSION_10_20H1;
                }
                break;
            }
        }
    }
}

void HandleLoadedModuleIfExplorerPatcher(HMODULE module) {
    if (module && !((ULONG_PTR)module & 3)) {
        if (IsExplorerPatcherModule(module)) {
            if (g_nExplorerVersion >= WIN_VERSION_11_21H2) {
                g_nExplorerVersion = WIN_VERSION_10_20H1;
            }
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfExplorerPatcher(module);
        HandleLoadedModuleIfSystemTray(module, lpLibFileName);
    }
    return module;
}

// -----------------------------------------------------------------------------
// Scroll-anywhere low-level mouse hook (volume only)
// -----------------------------------------------------------------------------

// True if at least one entry in the configured list is a system-volume action.
// Kept for places that need to differentiate volume vs. other actions (e.g.
// deciding whether the SndVol helper hook makes sense to keep installed).
bool HasAnyVolumeEntry() {
    for (const auto& entry : g_settings.scrollActions) {
        if (entry.scrollAction == ScrollAction::volumeChange) {
            return true;
        }
    }
    return false;
}

// Read the current modifier-key state once into a ModifierKeys snapshot.
// VK_SHIFT/CONTROL/MENU cover both left and right variants of each key.
static ModifierKeys SnapshotHeldModifiers() {
    ModifierKeys m{};
    m.shift = GetAsyncKeyState(VK_SHIFT) < 0;
    m.ctrl  = GetAsyncKeyState(VK_CONTROL) < 0;
    m.alt   = GetAsyncKeyState(VK_MENU) < 0;
    m.win   = (GetAsyncKeyState(VK_LWIN) < 0) ||
              (GetAsyncKeyState(VK_RWIN) < 0);
    return m;
}

// Exact-match comparison between two ModifierKeys combinations - the user
// must hold *exactly* the configured modifiers, no more, no less. This makes
// it safe to assign, e.g., Ctrl to one action and Ctrl+Shift to another
// without one swallowing the other.
static bool ModifierKeysEqual(const ModifierKeys& a, const ModifierKeys& b) {
    return a.shift == b.shift && a.ctrl == b.ctrl &&
           a.alt == b.alt && a.win == b.win;
}

// Find the first configured entry whose scroll-anywhere modifier combo is
// non-empty and exactly matches the currently held keys. Returns -1 if no
// entry should fire for the current key state.
int FindEntryIndexForCurrentModifiers() {
    ModifierKeys held = SnapshotHeldModifiers();
    if (!held.Any()) return -1;
    for (int i = 0; i < (int)g_settings.scrollActions.size(); i++) {
        const auto& entry = g_settings.scrollActions[i];
        if (!entry.scrollAnywhereKeys.Any()) continue;
        if (ModifierKeysEqual(entry.scrollAnywhereKeys, held)) {
            return i;
        }
    }
    return -1;
}

// Find the first configured entry whose scroll area (or one of its additional
// regions) contains pt. Used by the LL mouse hook to route full-screen scroll
// events to the action whose zone covers the cursor, instead of defaulting
// to volume for everything below the screen.
int FindEntryIndexAtPoint(HWND hMMTaskbarWnd, POINT pt) {
    for (int i = 0; i < (int)g_settings.scrollActions.size(); i++) {
        const auto& entry = g_settings.scrollActions[i];
        if (IsPointInsideEntryScrollArea(hMMTaskbarWnd, pt, entry)) {
            return i;
        }
    }
    return -1;
}

// True if any entry has at least one scroll-anywhere modifier configured.
// (The actual matching of held keys against an entry's combo is per-call.)
bool IsScrollAnywhereEnabled() {
    for (const auto& entry : g_settings.scrollActions) {
        if (entry.scrollAnywhereKeys.Any()) return true;
    }
    return false;
}

// We need the low-level mouse hook if either (a) any entry uses
// scroll-anywhere modifiers or (b) full-screen scrolling is on (regardless
// of which actions are configured).
bool IsMouseHookNeeded() {
    return IsScrollAnywhereEnabled() ||
           g_settings.fullScreenScrolling != FullScreenScrolling::disabled;
}

// -----------------------------------------------------------------------------
// Modifier-key masking for Win/Alt (prevents Start menu / menu bar on release)
// -----------------------------------------------------------------------------
//
// Using the Win or Alt key as a scroll-anywhere modifier would trigger a menu
// on release: a lone Win tap opens the Start menu, and a lone Alt tap activates
// the window's menu bar. To prevent this, the mouse hook flags the key as used
// for scrolling, and the keyboard hook below masks its release: it suppresses
// the real key-up and re-injects a dummy key (0xE8, unassigned) before it, so
// Windows sees a non-modifier key and skips the menu. 0xE8 is the masking key
// recommended by AutoHotkey for exactly this, both for Win and Alt:
// https://www.autohotkey.com/docs/v2/lib/A_MenuMaskKey.htm
//
// Masking must happen at release time, not during the scroll, since holding the
// key auto-repeats key-down events that would re-arm the lone-tap detection.

struct ModifierMaskState {
    bool down;
    bool usedForScroll;
};

ModifierMaskState g_winMaskState;
ModifierMaskState g_altMaskState;

// Returns true if the original key-up was suppressed and re-sent masked, in
// which case the caller should swallow it.
bool MaskModifierKeyEvent(ModifierMaskState* state,
                          WPARAM wParam,
                          KBDLLHOOKSTRUCT* pKbdStruct) {
    if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
        // Reset the flag on the initial press, but not on auto-repeat events
        // (which keep firing while the key is held).
        if (!state->down) {
            state->down = true;
            state->usedForScroll = false;
        }
    } else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
        state->down = false;

        // Ignore injected key-ups (such as the one we re-send below) to avoid
        // masking them again.
        if (state->usedForScroll && !(pKbdStruct->flags & LLKHF_INJECTED)) {
            state->usedForScroll = false;

            INPUT input[3] = {};
            input[0].type = INPUT_KEYBOARD;
            input[0].ki.wVk = 0xE8;
            input[1].type = INPUT_KEYBOARD;
            input[1].ki.wVk = 0xE8;
            input[1].ki.dwFlags = KEYEVENTF_KEYUP;
            // Faithfully replay the original key-up. The extended-key flag must
            // be preserved, otherwise extended modifiers (right Alt/AltGr and
            // the Win keys) may not be released correctly and could get stuck
            // down.
            input[2].type = INPUT_KEYBOARD;
            input[2].ki.wVk = (WORD)pKbdStruct->vkCode;
            input[2].ki.wScan = (WORD)pKbdStruct->scanCode;
            input[2].ki.dwFlags = KEYEVENTF_KEYUP;
            if (pKbdStruct->flags & LLKHF_EXTENDED) {
                input[2].ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
            }

            // Only suppress the original key-up if we managed to re-send the
            // whole sequence, otherwise the key would get stuck down (e.g. when
            // SendInput is blocked by an elevated foreground window).
            if (SendInput(ARRAYSIZE(input), input, sizeof(INPUT)) ==
                ARRAYSIZE(input)) {
                return true;
            }
        }
    }

    return false;
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    KBDLLHOOKSTRUCT* pKbdStruct = (KBDLLHOOKSTRUCT*)lParam;

    ModifierMaskState* state = nullptr;
    switch (pKbdStruct->vkCode) {
        case VK_LWIN:
        case VK_RWIN:
            state = &g_winMaskState;
            break;
        case VK_LMENU:
        case VK_RMENU:
            state = &g_altMaskState;
            break;
    }

    if (state && MaskModifierKeyEvent(state, wParam, pKbdStruct)) {
        return 1;
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// -----------------------------------------------------------------------------
// Low-level mouse hook
// -----------------------------------------------------------------------------
//
// The hook covers two features:
//
//   1. Scroll-anywhere: scrolling anywhere on the desktop while holding an
//      entry's configured modifier-key combo triggers that entry. Each entry
//      has its own combo (Shift/Ctrl/Alt/Win), so multiple actions can be
//      bound at once.
//
//   2. Full-screen scrolling: when full-screen apps are running, the taskbar
//      area is still considered active for scroll events. The cursor's
//      position is checked against each entry's configured scroll area, so
//      the same zone layout (volume in the tray, brightness elsewhere, etc.)
//      keeps working under a full-screen window. Set
//      `fullScreenScrolling: withoutIndicator` to mute the on-screen UI for
//      the volume entry in that mode.
//
// Both paths post g_dispatchEntryMsg to the taskbar window, packing the entry
// index and a "suppress indicator" bit into wParam. The taskbar's subclass
// proc unpacks them and calls InvokeScrollAction on the taskbar thread.
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION || wParam != WM_MOUSEWHEEL) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }
    HWND hTaskbarWnd = g_hTaskbarWnd;
    if (!hTaskbarWnd) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;
    short wheelDelta = (short)HIWORD(pMouseStruct->mouseData);
    POINT pt = pMouseStruct->pt;

    // Helper to pack (entryIndex, suppressIndicator) into a WORD. Using only
    // 15 bits for the index gives us up to 32767 entries which is well
    // beyond any sane configuration.
    auto packLoWord = [](int entryIndex, bool suppress) -> WORD {
        return (WORD)(((entryIndex & 0x7FFF) << 1) | (suppress ? 1 : 0));
    };

    // (1) Scroll-anywhere: any entry whose configured modifiers exactly
    //     match the currently-held keys takes the event, regardless of
    //     cursor position.
    int modEntryIdx = FindEntryIndexForCurrentModifiers();
    if (modEntryIdx >= 0) {
        // Mark the Win/Alt keys as used so that the keyboard hook masks their
        // release and no menu (Start menu / menu bar) opens.
        if (g_settings.scrollActions[modEntryIdx].scrollAnywhereKeys.win) {
            g_winMaskState.usedForScroll = true;
        }
        if (g_settings.scrollActions[modEntryIdx].scrollAnywhereKeys.alt) {
            g_altMaskState.usedForScroll = true;
        }
        PostMessage(hTaskbarWnd, g_dispatchEntryMsg,
                    MAKEWPARAM(packLoWord(modEntryIdx, /*suppress=*/false),
                               (WORD)wheelDelta),
                    MAKELPARAM(pt.x, pt.y));
        return 1;
    }

    // (2) Full-screen scrolling: only kicks in when enabled, and only when
    //     the cursor is NOT directly over a taskbar window (otherwise the
    //     normal WM_MOUSEWHEEL handler should run).
    if (g_settings.fullScreenScrolling != FullScreenScrolling::disabled) {
        HWND hPointWnd = WindowFromPoint(pt);
        if (hPointWnd) {
            HWND hRootWnd = GetAncestor(hPointWnd, GA_ROOT);
            if (hRootWnd && IsTaskbarWindow(hRootWnd)) {
                return CallNextHookEx(nullptr, nCode, wParam, lParam);
            }
        }

        // Find which entry's zone covers the cursor on the primary taskbar.
        int entryIdx = FindEntryIndexAtPoint(hTaskbarWnd, pt);

        // If we didn't find a match on the primary taskbar, scan secondary
        // (multi-monitor) taskbars. We capture the entry index via the
        // LPARAM context struct since EnumThreadWindows only gives us
        // a stop/continue boolean.
        if (entryIdx < 0) {
            DWORD dwTaskbarThreadId = g_dwTaskbarThreadId;
            if (dwTaskbarThreadId) {
                struct EnumCtx {
                    POINT pt;
                    int matchedIdx;
                } ctx{pt, -1};
                EnumThreadWindows(
                    dwTaskbarThreadId,
                    [](HWND hWnd, LPARAM lParamCtx)
                        WINAPI_LAMBDA_RETURN(BOOL) {
                            WCHAR szClassName[32];
                            if (GetClassName(hWnd, szClassName,
                                             ARRAYSIZE(szClassName)) &&
                                _wcsicmp(szClassName,
                                         L"Shell_SecondaryTrayWnd") == 0) {
                                EnumCtx* c = (EnumCtx*)lParamCtx;
                                int idx = FindEntryIndexAtPoint(hWnd, c->pt);
                                if (idx >= 0) {
                                    c->matchedIdx = idx;
                                    return FALSE;  // stop enumeration
                                }
                            }
                            return TRUE;
                        },
                    (LPARAM)&ctx);
                entryIdx = ctx.matchedIdx;
            }
        }

        if (entryIdx >= 0) {
            // For the volume-change entry only, the user can choose to mute
            // the indicator in full-screen mode. For all other actions, the
            // indicator setting is irrelevant (each one decides for itself
            // whether to show our custom OSD, based on customOsdEnabled).
            const auto& entry = g_settings.scrollActions[entryIdx];
            bool suppress = entry.scrollAction == ScrollAction::volumeChange &&
                            g_settings.fullScreenScrolling ==
                                FullScreenScrolling::withoutIndicator;
            PostMessage(hTaskbarWnd, g_dispatchEntryMsg,
                        MAKEWPARAM(packLoWord(entryIdx, suppress),
                                   (WORD)wheelDelta),
                        MAKELPARAM(pt.x, pt.y));
            return 1;
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

DWORD WINAPI ScrollAnywhereThread(LPVOID lpParameter) {
    HANDLE hReadyEvent = (HANDLE)lpParameter;

    g_winMaskState = {};
    g_altMaskState = {};

    HHOOK mouseHook =
        SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, nullptr, 0);

    // Only needed to mask the Win/Alt key release, see LowLevelKeyboardProc.
    // We also need it when any entry has win/alt as a scroll-anywhere modifier.
    bool needKeyboardHook = false;
    for (const auto& entry : g_settings.scrollActions) {
        if (entry.scrollAnywhereKeys.win || entry.scrollAnywhereKeys.alt) {
            needKeyboardHook = true;
            break;
        }
    }
    HHOOK keyboardHook = nullptr;
    if (needKeyboardHook) {
        keyboardHook =
            SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0);
    }

    SetEvent(hReadyEvent);

    if (!mouseHook) {
        Wh_Log(L"SetWindowsHookEx failed: %u", GetLastError());
        if (keyboardHook) {
            UnhookWindowsHookEx(keyboardHook);
        }
        return 1;
    }

    BOOL bRet;
    MSG msg;
    while ((bRet = GetMessage(&msg, nullptr, 0, 0)) != 0) {
        if (bRet == -1) break;
        if (msg.hwnd == nullptr && msg.message == WM_APP) {
            PostQuitMessage(0);
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (keyboardHook) {
        UnhookWindowsHookEx(keyboardHook);
    }
    UnhookWindowsHookEx(mouseHook);
    return 0;
}

void ScrollAnywhereThreadInit() {
    if (g_scrollAnywhereThread) return;
    HANDLE hReadyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!hReadyEvent) return;
    g_scrollAnywhereThread = CreateThread(nullptr, 0, ScrollAnywhereThread,
                                          hReadyEvent, 0, nullptr);
    if (g_scrollAnywhereThread) {
        WaitForSingleObject(hReadyEvent, INFINITE);
    }
    CloseHandle(hReadyEvent);
}

void ScrollAnywhereThreadUninit() {
    if (g_scrollAnywhereThread) {
        PostThreadMessage(GetThreadId(g_scrollAnywhereThread), WM_APP, 0, 0);
        WaitForSingleObject(g_scrollAnywhereThread, INFINITE);
        CloseHandle(g_scrollAnywhereThread);
        g_scrollAnywhereThread = nullptr;
    }
}

// -----------------------------------------------------------------------------
// Settings
// -----------------------------------------------------------------------------

void LoadSettings() {
    g_settings.scrollActions.clear();

    for (int i = 0;; i++) {
        PCWSTR scrollAction =
            Wh_GetStringSetting(L"ScrollActions[%d].scrollAction", i);
        PCWSTR scrollArea =
            Wh_GetStringSetting(L"ScrollActions[%d].scrollArea", i);
        bool hasEntry = *scrollAction || *scrollArea;
        if (!hasEntry) {
            Wh_FreeStringSetting(scrollAction);
            Wh_FreeStringSetting(scrollArea);
            break;
        }

        ScrollActionEntry entry{};

        entry.scrollAction = ScrollAction::virtualDesktopSwitch;
        if (wcscmp(scrollAction, L"brightnessChange") == 0) {
            entry.scrollAction = ScrollAction::brightnessChange;
        } else if (wcscmp(scrollAction, L"sdrToHdrBrightnessChange") == 0) {
            entry.scrollAction = ScrollAction::sdrToHdrBrightnessChange;
        } else if (wcscmp(scrollAction, L"masterBrightnessChange") == 0) {
            entry.scrollAction = ScrollAction::masterBrightnessChange;
        } else if (wcscmp(scrollAction, L"micVolumeChange") == 0) {
            entry.scrollAction = ScrollAction::micVolumeChange;
        } else if (wcscmp(scrollAction, L"volumeChange") == 0) {
            entry.scrollAction = ScrollAction::volumeChange;
        }
        Wh_FreeStringSetting(scrollAction);

        entry.scrollArea = ScrollArea::taskbar;
        if (wcscmp(scrollArea, L"notificationArea") == 0) {
            entry.scrollArea = ScrollArea::notificationArea;
        } else if (wcscmp(scrollArea, L"taskbarWithoutNotificationArea") == 0) {
            entry.scrollArea = ScrollArea::taskbarWithoutNotificationArea;
        } else if (wcscmp(scrollArea, L"none") == 0) {
            entry.scrollArea = ScrollArea::none;
        }
        Wh_FreeStringSetting(scrollArea);

        PCWSTR additionalScrollRegions = Wh_GetStringSetting(
            L"ScrollActions[%d].additionalScrollRegions", i);
        for (auto regionStr : SplitStringView(additionalScrollRegions, L",")) {
            regionStr = TrimStringView(regionStr);
            if (regionStr.empty()) continue;
            if (auto region = ParseRegion(regionStr)) {
                entry.additionalScrollRegions.push_back(*region);
            }
        }
        Wh_FreeStringSetting(additionalScrollRegions);

        entry.scrollStep =
            std::max(1, Wh_GetIntSetting(L"ScrollActions[%d].scrollStep", i));
        entry.throttleMs =
            Wh_GetIntSetting(L"ScrollActions[%d].throttleMs", i);
        entry.reverseScrollingDirection = Wh_GetIntSetting(
            L"ScrollActions[%d].reverseScrollingDirection", i);

        // Per-entry scroll-anywhere modifier keys. Each entry's combo is
        // independent, so different actions can be bound to different
        // modifier combinations (or none at all).
        entry.scrollAnywhereKeys.shift = Wh_GetIntSetting(
            L"ScrollActions[%d].scrollAnywhereKeys.shift", i);
        entry.scrollAnywhereKeys.ctrl = Wh_GetIntSetting(
            L"ScrollActions[%d].scrollAnywhereKeys.ctrl", i);
        entry.scrollAnywhereKeys.alt = Wh_GetIntSetting(
            L"ScrollActions[%d].scrollAnywhereKeys.alt", i);
        entry.scrollAnywhereKeys.win = Wh_GetIntSetting(
            L"ScrollActions[%d].scrollAnywhereKeys.win", i);

        g_settings.scrollActions.push_back(std::move(entry));
    }

    // SDR/HDR brightness
    g_settings.storeCurrentSdrToHdrBrightness =
        Wh_GetIntSetting(L"storeCurrentSdrToHdrBrightness");
    int maxSdrInt = Wh_GetIntSetting(L"maxSdrBrightness");
    if (maxSdrInt < 1) maxSdrInt = 1;
    if (maxSdrInt > 20) maxSdrInt = 20;  // safety cap
    g_settings.maxSdrBrightness = (double)maxSdrInt;

    // Volume indicator
    PCWSTR volumeIndicator = Wh_GetStringSetting(L"volumeIndicator");
    g_settings.volumeIndicator = VolumeIndicator::Win11;
    if (wcscmp(volumeIndicator, L"modern") == 0) {
        g_settings.volumeIndicator = VolumeIndicator::Modern;
    } else if (wcscmp(volumeIndicator, L"classic") == 0) {
        g_settings.volumeIndicator = VolumeIndicator::Classic;
    } else if (wcscmp(volumeIndicator, L"none") == 0) {
        g_settings.volumeIndicator = VolumeIndicator::None;
    }
    Wh_FreeStringSetting(volumeIndicator);

    g_settings.middleClickToMute = Wh_GetIntSetting(L"middleClickToMute");
    g_settings.ctrlScrollVolumeChange =
        Wh_GetIntSetting(L"ctrlScrollVolumeChange");
    g_settings.noAutomaticMuteToggle =
        Wh_GetIntSetting(L"noAutomaticMuteToggle");
    g_settings.volumeChangeStep = Wh_GetIntSetting(L"volumeChangeStep");

    // Custom OSD for brightness / HDR-SDR / mic (the volume entry uses
    // the system's own indicator). Defaults to enabled in the schema.
    g_settings.customOsdEnabled = Wh_GetIntSetting(L"customOsdEnabled");

    PCWSTR fullScreenScrolling = Wh_GetStringSetting(L"fullScreenScrolling");
    g_settings.fullScreenScrolling = FullScreenScrolling::disabled;
    if (wcscmp(fullScreenScrolling, L"withIndicator") == 0) {
        g_settings.fullScreenScrolling = FullScreenScrolling::withIndicator;
    } else if (wcscmp(fullScreenScrolling, L"withoutIndicator") == 0) {
        g_settings.fullScreenScrolling = FullScreenScrolling::withoutIndicator;
    }
    Wh_FreeStringSetting(fullScreenScrolling);

    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");

    // Software dimming overlay
    g_settings.softwareDimmingEnabled =
        Wh_GetIntSetting(L"softwareDimmingEnabled");
    int maxAlpha = Wh_GetIntSetting(L"maxOverlayAlpha");
    if (maxAlpha < 0) maxAlpha = 0;
    if (maxAlpha > 255) maxAlpha = 255;
    g_settings.maxOverlayAlpha = maxAlpha;

    // Eager-init SDR-on-HDR if any entry needs it.
    for (const auto& entry : g_settings.scrollActions) {
        if (entry.scrollAction == ScrollAction::sdrToHdrBrightnessChange ||
            entry.scrollAction == ScrollAction::masterBrightnessChange) {
            InitializeSdrToHdrBrightness();
            break;
        }
    }
}

// -----------------------------------------------------------------------------
// Mod init / uninit
// -----------------------------------------------------------------------------

bool HasAnyVolumeEntryWithIndicator(VolumeIndicator indicator) {
    if (!HasAnyVolumeEntry()) return false;
    return g_settings.volumeIndicator == indicator;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");
    LoadSettings();

    if (!WindowsVersionInit()) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    // Determine which target process we're in.
    g_target = Target::Explorer;
    WCHAR moduleFilePath[MAX_PATH];
    if (GetModuleFileName(nullptr, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
            moduleFileName++;
            if (_wcsicmp(moduleFileName, L"ShellExperienceHost.exe") == 0) {
                g_target = Target::ShellExperienceHost;
            } else if (_wcsicmp(moduleFileName, L"SndVol.exe") == 0) {
                g_target = Target::SndVol;
            }
        } else {
            Wh_Log(L"GetModuleFileName returned an unsupported path");
            return FALSE;
        }
    } else {
        Wh_Log(L"GetModuleFileName failed");
        return FALSE;
    }

    // ShellExperienceHost.exe and SndVol.exe: only the focus-wheel-routing
    // hook is needed, and only when the matching indicator is configured.
    if (g_target == Target::ShellExperienceHost || g_target == Target::SndVol) {
        if (!HasAnyVolumeEntry()) {
            return FALSE;
        }
        if (g_settings.volumeChangeStep == 2 &&
            !g_settings.noAutomaticMuteToggle) {
            return FALSE;
        }
        if (g_target == Target::ShellExperienceHost) {
            bool useModernIndicator =
                g_settings.volumeIndicator == VolumeIndicator::Modern &&
                CanUseModernIndicator();
            if (!useModernIndicator) return FALSE;
        } else if (g_target == Target::SndVol) {
            bool useClassicIndicator =
                g_settings.volumeIndicator == VolumeIndicator::Classic ||
                (g_settings.volumeIndicator == VolumeIndicator::Modern &&
                 !CanUseModernIndicator());
            if (!useClassicIndicator) return FALSE;
        }

        HMODULE user32Module = GetModuleHandle(L"user32.dll");
        if (user32Module) {
            auto pFunc = (ForceFocusBasedMouseWheelRouting_t)GetProcAddress(
                user32Module, MAKEINTRESOURCEA(2575));
            if (pFunc) {
                WindhawkUtils::SetFunctionHook(
                    pFunc, ForceFocusBasedMouseWheelRouting_Hook,
                    &ForceFocusBasedMouseWheelRouting_Original);
            }
        }
        return TRUE;
    }

    // Explorer.exe path.
    g_nExplorerVersion = g_nWinVersion;
    if (g_nExplorerVersion >= WIN_VERSION_11_21H2 &&
        g_settings.oldTaskbarOnWin11) {
        g_nExplorerVersion = WIN_VERSION_10_20H1;
    }

    if (ShouldHookSystemTraySymbols()) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            g_systemTrayModuleHooked = true;
            if (!HookSystemTraySymbols(systemTrayModule)) {
                // Continue anyway - middle-click-to-mute won't work but the
                // rest of the mod still should.
            }
        }
    }

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                       &CreateWindowExW_Original);

    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32Module) {
        auto pCreateWindowInBand = (CreateWindowInBand_t)GetProcAddress(
            user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand) {
            WindhawkUtils::SetFunctionHook(pCreateWindowInBand,
                                               CreateWindowInBand_Hook,
                                               &CreateWindowInBand_Original);
        }
    }

    HandleLoadedExplorerPatcher();

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    if (kernelBaseModule) {
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                     "LoadLibraryExW");
        if (pKernelBaseLoadLibraryExW) {
            WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                               LoadLibraryExW_Hook,
                                               &LoadLibraryExW_Original);
        }
    }

    g_initialized = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    if (g_target != Target::Explorer) return;

    if (ShouldHookSystemTraySymbols() && !g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"Got system tray module");

                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    HandleLoadedExplorerPatcher();

    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(NULL), L"Shell_TrayWnd", &wndclass)) {
        HWND hWnd =
            FindCurrentProcessTaskbarWindows(&g_secondaryTaskbarWindows);
        if (hWnd) HandleIdentifiedTaskbarWindow(hWnd);
    }

    if (IsMouseHookNeeded()) {
        ScrollAnywhereThreadInit();
    }

    DWORD neededTimers = 0;
    // Dim overlay animation timer (~60 FPS).
    if (g_settings.softwareDimmingEnabled) {
        SetTimer(NULL, 0xE000, 16,
            [](HWND, UINT, UINT_PTR, DWORD) {
                DimOverlay::TickAnimation();
            });
        neededTimers++;
    }
    // External brightness sync poll (every 2 seconds).
    SetTimer(NULL, 0xE001, 2000,
        [](HWND, UINT, UINT_PTR, DWORD) {
            MaybeSyncDimOverlay();
        });
    neededTimers++;
    Wh_Log(L"Started %lu periodic timers", neededTimers);
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    if (g_settings.storeCurrentSdrToHdrBrightness &&
        g_currentSdrToHdrBrightness != 1.0) {
        Wh_SetIntValue(L"CurrentSdrToHdrBrightness",
                       (int)(g_currentSdrToHdrBrightness * 10));
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_target != Target::Explorer) return;

    ScrollAnywhereThreadUninit();

    if (g_hTaskbarWnd) {
        UnsubclassTaskbarWindow(g_hTaskbarWnd);
        for (HWND hSecondaryWnd : g_secondaryTaskbarWindows) {
            UnsubclassTaskbarWindow(hSecondaryWnd);
        }
    }

    CleanupSndVol();
    AudioUninit();
    OsdOverlay::Cleanup();
    DimOverlay::Cleanup();

    KillTimer(NULL, 0xE000);
    KillTimer(NULL, 0xE001);

    if (g_hDwmApiDll) {
        FreeLibrary(g_hDwmApiDll);
        g_hDwmApiDll = nullptr;
        g_pDwmpSDRToHDRBoost = nullptr;
    }

    if (g_hLCD != INVALID_HANDLE_VALUE) {
        CloseHandle(g_hLCD);
        g_hLCD = INVALID_HANDLE_VALUE;
    }
    if (g_hPowrProf) {
        FreeLibrary(g_hPowrProf);
        g_hPowrProf = nullptr;
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;
    bool prevMiddleClickToMute = g_settings.middleClickToMute;
    bool prevStoreSdr = g_settings.storeCurrentSdrToHdrBrightness;
    VolumeIndicator prevVolumeIndicator = g_settings.volumeIndicator;

    LoadSettings();

    if (g_target == Target::ShellExperienceHost || g_target == Target::SndVol) {
        // Force reload if indicator changed - the helper processes have to be
        // re-evaluated for hook applicability.
        *bReload = prevVolumeIndicator != g_settings.volumeIndicator;
        return TRUE;
    }

    g_nExplorerVersion = g_nWinVersion;
    if (g_nExplorerVersion >= WIN_VERSION_11_21H2 &&
        g_settings.oldTaskbarOnWin11) {
        g_nExplorerVersion = WIN_VERSION_10_20H1;
    }

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11 ||
               g_settings.middleClickToMute != prevMiddleClickToMute ||
               g_settings.storeCurrentSdrToHdrBrightness != prevStoreSdr;

    if (!*bReload) {
        ScrollAnywhereThreadUninit();
        if (IsMouseHookNeeded()) {
            ScrollAnywhereThreadInit();
        }
    }

    Wh_Log(L"Settings changed. bReload=%s", *bReload ? L"TRUE" : L"FALSE");
    return TRUE;
}

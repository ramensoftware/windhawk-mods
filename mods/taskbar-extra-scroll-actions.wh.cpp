// ==WindhawkMod==
// @id              taskbar-extra-scroll-actions
// @name            Taskbar Extra Scroll Actions
// @description     Scroll over the taskbar to switch virtual desktops, change brightness per screen from -100% to 100% (HDR SDR Brightness + Dark Overlay), microphone, system and per-app volume
// @version         1.6.0
// @author          NairoDorian
// @github          https://github.com/NairoDorian
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// @include         SndVol.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -ldwmapi -ldxva2 -lgdi32 -lole32 -loleaut32 -lversion -lwbemuuid -lpowrprof -lshcore -lntdll -lruntimeobject
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// This mod merges and extends:
//   - taskbar-scroll-actions v1.2 (m417z): per-entry config, DDC/CI and WMI
//     brightness, microphone volume, additional scroll regions
//   - taskbar-volume-control v1.3.1 (m417z): system volume, indicators,
//     scroll anywhere, full screen scrolling, middle-click mute
//   - taskbar-volume-control-per-app v1.1.4 (m417z): per-app volume on task
//     buttons

// ==WindhawkModReadme==
/*
# Taskbar Extra Scroll Actions

Scroll over the taskbar (or anywhere on screen while holding modifier keys) to:

* **Switch virtual desktop**
* **Change brightness**, per screen, from **−100 % to 100 %**
* **Change SDR brightness on HDR screens**
* **Master brightness**: SDR brightness on HDR screens, monitor brightness on
  the others
* **Change microphone volume**
* **Change system volume** with the Windows 11, Windows 10, Windows 7 or no
  indicator
* **Per-app volume**: hold a modifier key and scroll over a task button

## Brightness from −100 % to 100 %

Every screen has its own level, and changing one screen never changes another.

* **0 % … 100 %** is the screen's real brightness:
  * laptop panel: the backlight (method selectable: Auto, IOCTL, WMI, power
    plan)
  * external monitor with DDC/CI: the monitor's own brightness
  * HDR screen: the brightness of SDR content, from 80 nits up to the
    *Maximum SDR brightness* setting
* **−1 % … −100 % is the Dark Overlay**. Once the hardware is at its minimum, a
  black click-through layer darkens that screen further. This helps at night
  when even 0 % is too bright.

If you change a screen's brightness somewhere else (Fn keys, Windows Settings,
the monitor's buttons with DDC/CI), **that screen's** Dark Overlay is reset
automatically (optional). Other screens are not affected.

The Dark Overlay also covers the taskbar thumbnail previews, Start, the
notification center and the other shell popups. The mouse pointer isn't part of
the screen image (the graphics card draws it on top of everything), so while
it is over a darkened screen the standard pointers are swapped for copies
darkened the same way. Your pointer scheme comes back as soon as the pointer
leaves that screen. Pointers that an app draws itself (games, some editors)
stay as they are.

On HDR screens, Windows shows the pointer at the SDR brightness of the
Settings app, while this mod changes the SDR brightness of what's on screen
only. So whenever the mod's SDR brightness is lower than the Settings one (at
0% it's 80 nits), the pointer is darkened by the same ratio, also above 0%.

The on-screen indicator is darkened along with the screen (optional).

The Dark Overlay can be hidden from screenshots and screen sharing.

## Per-app volume

Hold the configured modifier (Alt by default) and scroll over a task button to
change that app's volume. While the modifier is held, the rest of the taskbar
(outside the notification area) doesn't run your scroll actions; without it,
scrolling over task buttons keeps doing your normal scroll action. Modifier +
click toggles the app's mute (optional), and so can a middle-click on a task
button (optional, off by default). This requires the Windows 11 taskbar.

## Scroll-anywhere and full-screen scrolling

Each entry can have its own scroll-anywhere modifier combination. Full-screen
scrolling works when a full-screen window covers a visible taskbar; with an
auto-hidden taskbar, only the thin strip at the screen edge reacts.

## How it differs from similar mods

* **[Taskbar Scroll Actions](https://windhawk.net/mods/taskbar-scroll-actions)**
  switches virtual desktops and changes the monitor brightness and the
  microphone volume. This mod does all of that, and adds brightness below 0%
  (the Dark Overlay), SDR brightness on HDR screens, a master brightness, more
  laptop backlight methods, its own indicator, and a scroll-anywhere modifier
  combination per entry.
* **[Taskbar Volume Control](https://windhawk.net/mods/taskbar-volume-control)**
  and **[Taskbar Volume Control Per-App](https://windhawk.net/mods/taskbar-volume-control-per-app)**
  change the system and per-app volume. This mod includes both, as actions of
  the same scroll setup.

Use one of these mods at a time: they all react to scrolling over the taskbar.

## Debugging

Turn on logging for this mod in Windhawk (the mod's **Advanced** tab) and open
Windhawk's log output. Every scroll logs the screen, its type
(internal/external, HDR), the backend used, the level before → after, the Dark
Overlay alpha, worker write timings, and the reason for every automatic Dark
Overlay reset. **Debug info on the indicator** adds the same details as an extra
line on the on-screen indicator.

## Notes

When the mod is disabled, the Dark Overlay and the pointers are removed, and
the SDR brightness of HDR screens goes back to the Windows Settings level.

The darkened pointer replaces the standard pointers for every app while the
pointer is over a darkened screen, then reloads your pointer scheme. Mods and
tools that change the pointers themselves (pointer themes, idle fade, and the
like) can overwrite the darkened pointers, and lose their own pointers when the
scheme is reloaded.

Some touchpads don't send scroll events over the taskbar. The pinch-to-zoom
gesture often works instead.

The mod runs in explorer.exe (plus ShellExperienceHost.exe and SndVol.exe for
the volume indicators). It has nothing to do in dwm.exe: if dwm.exe is in this
mod's process inclusion list, the mod unloads itself there.

While an app running as administrator has the keyboard focus, Windows hides
the keyboard from Explorer. Scrolling over the taskbar (per-app volume and
scroll-anywhere combos included) still works, but scroll-anywhere combos over
other windows don't, and releasing Alt or Win can still trigger that app's
menu bar or the Start menu.

## Credits

Built on three mods by **m417z**, which this mod merges and extends:
[Taskbar Scroll Actions](https://windhawk.net/mods/taskbar-scroll-actions)
(per-entry actions, DDC/CI and WMI brightness, microphone volume, scroll
regions), [Taskbar Volume Control](https://windhawk.net/mods/taskbar-volume-control)
(system volume, indicators, scroll-anywhere, full-screen scrolling,
middle-click mute) and
[Taskbar Volume Control Per-App](https://windhawk.net/mods/taskbar-volume-control-per-app)
(per-app volume on task buttons). Thanks!
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ScrollActions:
  - - scrollAction: masterBrightnessChange
      $name: Scroll action
      $options:
      - virtualDesktopSwitch: Switch virtual desktop
      - brightnessChange: Change brightness (backlight, DDC/CI, or SDR brightness on HDR screens)
      - sdrToHdrBrightnessChange: Change SDR brightness (HDR screens only)
      - masterBrightnessChange: Master brightness (SDR brightness on HDR screens, monitor brightness otherwise)
      - micVolumeChange: Change microphone volume
      - volumeChange: Change system volume
    - scrollArea: taskbarWithoutNotificationArea
      $name: Scroll area
      $options:
      - taskbar: The entire taskbar
      - notificationArea: The notification area (system tray and clock)
      - taskbarWithoutNotificationArea: The taskbar without the notification area
      - none: None (only additional regions and/or scroll-anywhere keys)
    - additionalScrollRegions: ""
      $name: Additional scroll regions
      $description: >-
        Comma-separated list of regions along the taskbar, measured from its
        leading edge in pixels or percent, e.g. "100-200, 25%-50%".
    - scrollStep: 1
      $name: Scroll step
      $description: >-
        Change per mouse-wheel notch. For brightness and the Dark Overlay this
        is in percent.
    - throttleMs: 0
      $name: Throttle (ms)
      $description: >-
        Minimum time between actions. 0 disables throttling. When throttled,
        fast scrolls collapse to a single step.
    - reverseScrollingDirection: false
      $name: Reverse scrolling direction
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
        While exactly these keys are held, scrolling anywhere on screen
        triggers this action. Leave all unchecked to disable.
  - - scrollAction: volumeChange
    - scrollArea: notificationArea
    - additionalScrollRegions: ""
    - scrollStep: 1
    - throttleMs: 0
    - reverseScrollingDirection: false
    - scrollAnywhereKeys:
      - shift: false
      - ctrl: false
      - alt: false
      - win: false
  $name: Scroll actions
  $description: >-
    One or more scroll actions. The first entry matching the cursor position
    (or the held scroll-anywhere keys) wins.
- laptopBrightnessMethod: auto
  $name: Laptop backlight method
  $description: >-
    How the brightness of the laptop's built-in panel is changed. Auto uses the
    first method that works, in this order: IOCTL, WMI, power plan.
  $options:
  - auto: Auto (IOCTL, then WMI, then power plan)
  - ioctl: IOCTL (direct, fastest)
  - wmi: WMI (WmiSetBrightness)
  - powerScheme: Power plan setting (last resort)
- softwareDimmingEnabled: true
  $name: Dark Overlay (below 0%)
  $description: >-
    Scroll below 0% to darken a screen further with a black click-through
    overlay, down to -100%.
- maxOverlayAlpha: 240
  $name: Dark Overlay strength at -100% (0-255)
  $description: >-
    255 = completely black. 200-240 keeps the screen faintly visible.
- darkOverlayCurve: perceptual
  $name: Dark Overlay curve
  $options:
  - perceptual: Perceptual (equal-looking steps on SDR and HDR screens)
  - linear: Linear alpha
- autoResetDarkOverlay: true
  $name: Reset the Dark Overlay when brightness is changed elsewhere
  $description: >-
    If a screen's brightness changes outside this mod (Fn keys, Windows
    Settings, monitor buttons), that screen's Dark Overlay is cleared. Other
    screens are never affected.
- hideOverlayFromCapture: true
  $name: Hide the Dark Overlay from screenshots and screen sharing
- darkenCursor: true
  $name: Darken the mouse pointer too
  $description: >-
    The pointer is drawn by the graphics card on top of the screen image, so
    no overlay can cover it. While it is over a screen below 0%, the standard
    pointers are swapped for darkened copies; your pointer scheme comes back
    as soon as the pointer leaves that screen. On HDR screens Windows shows
    the pointer at the SDR brightness of the Settings app, so it's also
    darkened whenever this mod's SDR brightness is lower. Pointers drawn by
    apps themselves (games, some editors) aren't darkened.
- darkenIndicator: true
  $name: Darken the brightness indicator too
  $description: >-
    The on-screen indicator is darkened by the Dark Overlay like the rest of
    the screen. Off: it stays above the overlay, fully readable.
- overlayAboveSystemUi: true
  $name: Dark Overlay covers thumbnail previews, Start and shell popups
  $description: >-
    Puts the overlay in a window band above the taskbar thumbnail previews,
    Start, the notification center and the other shell popups. Off: a normal
    topmost window.
- storeCurrentSdrToHdrBrightness: true
  $name: Remember SDR brightness per HDR screen
  $description: >-
    Saves the SDR brightness of each HDR screen and re-applies it when the mod
    starts.
- maxSdrBrightness: 18
  $name: Maximum SDR brightness on HDR screens
  $description: >-
    SDR brightness at 100%, in multiples of 80 nits (2-18). 6 = 480 nits, the
    Windows Settings maximum. 18 = 1440 nits.
- customOsdEnabled: true
  $name: Show the on-screen indicator for brightness and microphone
- volumeIndicator: win11
  $name: Volume indicator
  $options:
  - win11: Windows 11
  - modern: Windows 10
  - classic: Windows 7
  - none: None
- middleClickToMute: false
  $name: Middle-click the volume tray icon to mute (Windows 11 22H2+)
- ctrlScrollVolumeChange: false
  $name: System volume on the taskbar requires Ctrl
- noAutomaticMuteToggle: false
  $name: No automatic mute toggle
  $description: >-
    Windows 11 indicator: volume scrolling is disabled while muted. Other
    indicators: the device is not muted/unmuted automatically at 0%.
- volumeChangeStep: 2
  $name: System volume change step (percent)
- fullScreenScrolling: withIndicator
  $name: Full-screen scrolling
  $description: >-
    Scroll at the taskbar position while a full-screen window covers the
    taskbar. The entry is chosen by the same zones as normal scrolling.
  $options:
  - disabled: Disabled
  - withIndicator: Enabled, with indicators
  - withoutIndicator: Enabled, without indicators
- perAppVolume:
  - enabled: true
    $name: Enable per-app volume
  - modifierKeys:
    - shift: false
      $name: Shift
    - ctrl: false
      $name: Ctrl
    - alt: true
      $name: Alt
    - win: false
      $name: Win
    $name: Modifier keys
    $description: >-
      Hold exactly these keys and scroll over a task button to change that
      app's volume. While they are held, scrolling on the taskbar (outside the
      notification area) is reserved for per-app volume; without them, your
      normal scroll actions run everywhere.
  - volumeChangeStep: 2
    $name: Per-app volume step (percent)
  - modifierClickToMute: true
    $name: Modifier + click on a task button toggles the app's mute
    $description: >-
      If Ctrl is the modifier, this replaces Windows' Ctrl+click (cycling a
      group's windows).
  - middleClickToMute: false
    $name: Middle-click on a task button toggles the app's mute
    $description: >-
      Replaces Windows' middle-click on a task button (opening a new window of
      the app).
  - terseFormat: false
    $name: Terse tooltip (icons instead of text)
  - tooltipColor: ""
    $name: Tooltip color
    $description: >-
      Background color of the volume tooltip, #RRGGBB or #AARRGGBB (e.g.
      #C42B1C). Leave empty to use the Windows accent color.
  - noAutomaticMuteToggle: false
    $name: No automatic mute toggle for apps
  $name: Per-app volume (Windows 11 taskbar)
- debugOsd: false
  $name: Debug info on the indicator
- oldTaskbarOnWin11: false
  $name: Old taskbar on Windows 11
  $description: Enable if you use the old taskbar on Windows 11 (ExplorerPatcher or similar)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

// For the vectors that collect the entries of the SYMBOL_HOOK arrays below,
// to resolve each module's hooks in one HookSymbols call.
using COMBINED_SH = WindhawkUtils::SYMBOL_HOOK;

#include <commctrl.h>
#include <dwmapi.h>
#include <audiopolicy.h>
#include <endpointvolume.h>
#include <highlevelmonitorconfigurationapi.h>
#include <mmdeviceapi.h>
#include <physicalmonitorenumerationapi.h>
#include <powrprof.h>
#include <psapi.h>
#include <shellscalingapi.h>
#include <tlhelp32.h>
#include <wbemcli.h>
#include <winternl.h>

#include <algorithm>
#include <atomic>
#include <climits>
#include <cmath>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace wux = winrt::Windows::UI::Xaml;

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

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

// -----------------------------------------------------------------------------
// Settings
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

enum class VolumeIndicator { None, Classic, Modern, Win11 };
enum class FullScreenScrolling { disabled, withIndicator, withoutIndicator };
enum class LaptopMethod { Auto, Ioctl, Wmi, PowerScheme };
enum class DarkCurve { Perceptual, Linear };

struct Region {
    bool isPercentage;
    int start;
    int end;
};

struct ModifierKeys {
    bool shift = false;
    bool ctrl = false;
    bool alt = false;
    bool win = false;

    bool Any() const { return shift || ctrl || alt || win; }
    bool operator==(const ModifierKeys& o) const {
        return shift == o.shift && ctrl == o.ctrl && alt == o.alt &&
               win == o.win;
    }
};

struct ScrollActionEntry {
    ScrollAction scrollAction;
    ScrollArea scrollArea;
    std::vector<Region> additionalScrollRegions;
    int scrollStep;
    int throttleMs;
    bool reverseScrollingDirection;
    ModifierKeys scrollAnywhereKeys;
};

struct PerAppSettings {
    bool enabled = true;
    ModifierKeys keys = {false, false, true, false};  // Alt
    int volumeChangeStep = 2;
    bool clickToMute = true;
    bool middleClickToMute = false;
    bool terseFormat = false;
    bool noAutomaticMuteToggle = false;
    // 0xAARRGGBB, used when hasTooltipColor (otherwise the accent color).
    bool hasTooltipColor = false;
    uint32_t tooltipColor = 0;
};

struct Settings {
    std::vector<ScrollActionEntry> scrollActions;
    LaptopMethod laptopMethod = LaptopMethod::Auto;
    bool darkOverlayEnabled = true;
    int maxOverlayAlpha = 240;
    DarkCurve darkCurve = DarkCurve::Perceptual;
    bool autoResetDarkOverlay = true;
    bool hideOverlayFromCapture = true;
    bool darkenCursor = true;
    bool darkenIndicator = true;
    bool overlayAboveSystemUi = true;
    bool storeSdr = true;
    double maxSdrBrightness = 18.0;
    bool customOsdEnabled = true;
    VolumeIndicator volumeIndicator = VolumeIndicator::Win11;
    bool middleClickToMute = false;
    bool ctrlScrollVolumeChange = false;
    bool noAutomaticMuteToggle = false;
    int volumeChangeStep = 2;
    FullScreenScrolling fullScreenScrolling = FullScreenScrolling::withIndicator;
    PerAppSettings perApp;
    bool debugOsd = false;
    bool oldTaskbarOnWin11 = false;

    bool HasBrightnessEntry() const {
        for (const auto& e : scrollActions) {
            if (e.scrollAction == ScrollAction::brightnessChange ||
                e.scrollAction == ScrollAction::sdrToHdrBrightnessChange ||
                e.scrollAction == ScrollAction::masterBrightnessChange) {
                return true;
            }
        }
        return false;
    }
    bool HasAction(ScrollAction a) const {
        for (const auto& e : scrollActions) {
            if (e.scrollAction == a) {
                return true;
            }
        }
        return false;
    }
};

using SettingsPtr = std::shared_ptr<const Settings>;

// Settings are immutable snapshots, swapped atomically. Readers on any thread
// (taskbar thread, low-level hook thread, brightness worker) take their own
// reference, so a settings change can never free data that is being read.
SRWLOCK g_settingsLock = SRWLOCK_INIT;
SettingsPtr g_settingsPtr;

SettingsPtr Cfg() {
    AcquireSRWLockShared(&g_settingsLock);
    SettingsPtr p = g_settingsPtr;
    ReleaseSRWLockShared(&g_settingsLock);
    if (!p) {
        static SettingsPtr defaults = std::make_shared<Settings>();
        return defaults;
    }
    return p;
}

void SetCfg(SettingsPtr p) {
    AcquireSRWLockExclusive(&g_settingsLock);
    g_settingsPtr = std::move(p);
    ReleaseSRWLockExclusive(&g_settingsLock);
}


// -----------------------------------------------------------------------------
// Globals
// -----------------------------------------------------------------------------

enum class Target { Explorer, ShellExperienceHost, SndVol };
Target g_target = Target::Explorer;

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

int g_nWinVersion;
int g_nExplorerVersion;
HWND g_hTaskbarWnd;
DWORD g_dwTaskbarThreadId;

std::atomic<bool> g_initialized;
std::atomic<bool> g_unloading;
std::atomic<bool> g_systemTrayModuleHooked;
std::atomic<bool> g_taskbarViewDllLoaded;
bool g_inputSiteProcHooked;
std::unordered_set<HWND> g_secondaryTaskbarWindows;

// Posted by the low-level mouse hook to the taskbar thread.
//   wParam: HIWORD = wheel delta, LOWORD = (entryIndex << 2) | flags
//           flag bit 0 = suppress indicators, bit 1 = from scroll-anywhere
//   lParam: MAKELPARAM(cursor.x, cursor.y)
UINT g_dispatchEntryMsg =
    RegisterWindowMessage(L"Windhawk_DispatchEntry_" WH_MOD_ID);

// Tag for input we inject, so our keyboard hook ignores it.
constexpr ULONG_PTR kInjectedInputTag = 0x57484558;  // 'WHEX'

constexpr DWORD kWheelAccumulationWindowMs = 5000;

// The controller window lives on the taskbar thread and owns every window and
// timer this mod creates (see the Controller section).
std::atomic<HWND> g_hController;

// -----------------------------------------------------------------------------
// Forward declarations
// -----------------------------------------------------------------------------

namespace Topo {
struct Screen;
}
namespace Bright {
int CurrentSdrRaw(const std::wstring& screenId);
}
namespace Overlay {
HWND VisibleWindowOn(HMONITOR hMon);
}
void InvokeScrollAction(HWND hWnd,
                        WPARAM wParam,
                        LPARAM lMousePosParam,
                        int entryIndex,
                        bool suppressIndicator,
                        bool fromScrollAnywhere);
bool EnsureController();
void CloseSndVolTimerTick();
namespace PerApp {
void OnTooltipHideTimer();
bool ShouldYieldWheelLowLevel(POINT pt);
bool ShowUnavailableIfRequested(HWND hTaskbarWnd, POINT pt);
}  // namespace PerApp

// -----------------------------------------------------------------------------
// Utility functions
// -----------------------------------------------------------------------------

HINSTANCE ThisModule() {
    static HINSTANCE h = []() {
        HMODULE m = nullptr;
        GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                              GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          (LPCWSTR)&Cfg, &m);
        return (HINSTANCE)m;
    }();
    return h;
}

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

UINT GetDpiForMonitorWithFallback(HMONITOR hMon) {
    UINT dx = 96, dy = 96;
    if (!hMon || FAILED(GetDpiForMonitor(hMon, MDT_EFFECTIVE_DPI, &dx, &dy)) ||
        dx == 0) {
        return 96;
    }
    return dx;
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

// Runs proc on the thread that owns hWnd and waits for it (from m417z's
// per-app volume mod). Used to create and destroy our windows on the taskbar
// thread, since a window can only be destroyed by the thread that created it.
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

    RUN_FROM_WINDOW_THREAD_PARAM param{
        .proc = proc,
        .procParam = procParam,
    };
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);
    return true;
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

    // Fallback: the last pixels of the taskbar.
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
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
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
                    g_nWinVersion =
                        (nQFE < 17000) ? WIN_VERSION_81 : WIN_VERSION_811;
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
    std::vector<std::wstring_view> res;
    while ((pos_end = s.find(delimiter, pos_start)) !=
           std::wstring_view::npos) {
        res.push_back(s.substr(pos_start, pos_end - pos_start));
        pos_start = pos_end + delim_len;
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
        Wh_Log(L"Invalid region (expected start-end): %.*s",
               (int)regionStr.size(), regionStr.data());
        return std::nullopt;
    }
    auto startStr = TrimStringView(parts[0]);
    auto endStr = TrimStringView(parts[1]);

    bool startIsPercentage = !startStr.empty() && startStr.back() == L'%';
    bool endIsPercentage = !endStr.empty() && endStr.back() == L'%';
    if (startIsPercentage != endIsPercentage) {
        Wh_Log(L"Invalid region (mixed percent and pixel): %.*s",
               (int)regionStr.size(), regionStr.data());
        return std::nullopt;
    }

    bool isPercentage = startIsPercentage;
    if (isPercentage) {
        startStr.remove_suffix(1);
        endStr.remove_suffix(1);
    }

    int start, end;
    if (!SvToInt(startStr, &start) || !SvToInt(endStr, &end) || start >= end) {
        Wh_Log(L"Invalid region: %.*s", (int)regionStr.size(),
               regionStr.data());
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
            // rc.right is exclusive: the rightmost pixel is offset 0.
            cursorOffset = rc.right - 1 - pt.x;
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

bool IsPointInsideAnyScrollArea(HWND hMMTaskbarWnd, POINT pt) {
    auto cfg = Cfg();
    for (const auto& entry : cfg->scrollActions) {
        if (IsPointInsideEntryScrollArea(hMMTaskbarWnd, pt, entry)) {
            return true;
        }
    }
    return false;
}

// Reading the modifier keys.
//
// Explorer isn't elevated. While an elevated (admin) window has the keyboard
// focus, Windows (UIPI) hides the keyboard from it: GetAsyncKeyState reports
// every key as up, and low-level keyboard hooks and raw input get nothing
// (tested on build 26300 with an admin Windows Terminal in front). That is why
// per-app volume only reacted after a click on the taskbar had made Explorer
// the foreground process.
//
// The keyboard state Windows attaches to each input message is not hidden:
// while a thread handles a wheel or click message, GetKeyState returns the
// keys as they were when that message was generated, Alt and Win included,
// whatever window has the focus. So:
//   - on the taskbar thread, while handling a wheel/click message, use
//     InputModifiers() / IsKeyDownForInput(),
//   - on the low-level hook thread, which only sees raw events, use
//     AsyncModifiers() (blind while an admin window has the focus).

// Taskbar thread: true while OnMouseWheel handles a wheel message, so code it
// calls (e.g. the virtual desktop switch) reads the message's key state.
bool g_inInputMessage;

struct InputMessageScope {
    bool prev = g_inInputMessage;
    InputMessageScope() { g_inInputMessage = true; }
    ~InputMessageScope() { g_inInputMessage = prev; }
    InputMessageScope(const InputMessageScope&) = delete;
    InputMessageScope& operator=(const InputMessageScope&) = delete;
};

ModifierKeys AsyncModifiers() {
    ModifierKeys m;
    m.shift = GetAsyncKeyState(VK_SHIFT) < 0;
    m.ctrl = GetAsyncKeyState(VK_CONTROL) < 0;
    m.alt = GetAsyncKeyState(VK_MENU) < 0;
    m.win = (GetAsyncKeyState(VK_LWIN) < 0) || (GetAsyncKeyState(VK_RWIN) < 0);
    return m;
}

// The keys held when the input message being handled by this thread was
// generated.
ModifierKeys InputModifiers() {
    ModifierKeys m;
    m.shift = GetKeyState(VK_SHIFT) < 0;
    m.ctrl = GetKeyState(VK_CONTROL) < 0;
    m.alt = GetKeyState(VK_MENU) < 0;
    m.win = (GetKeyState(VK_LWIN) < 0) || (GetKeyState(VK_RWIN) < 0);
    return m;
}

// Taskbar thread: the message's key state while handling a wheel message,
// otherwise (posted dispatches from the low-level hook) the async state.
bool IsKeyDownForInput(int vk) {
    return g_inInputMessage ? GetKeyState(vk) < 0 : GetAsyncKeyState(vk) < 0;
}

// Win/Alt release masking state (see the low-level keyboard hook).
struct ModifierMaskState {
    std::atomic<bool> down;
    std::atomic<bool> usedForScroll;
    void Reset() {
        down = false;
        usedForScroll = false;
    }
};
ModifierMaskState g_winMaskState;
ModifierMaskState g_altMaskState;

void MarkModifiersUsedForScroll(const ModifierKeys& keys) {
    if (keys.win) {
        g_winMaskState.usedForScroll = true;
    }
    if (keys.alt) {
        g_altMaskState.usedForScroll = true;
    }
}

// 32-bit FNV-1a, used to build stable per-screen storage keys.
uint32_t HashString(std::wstring_view s) {
    uint32_t h = 2166136261u;
    for (WCHAR c : s) {
        h ^= (uint32_t)c;
        h *= 16777619u;
    }
    return h;
}

// -----------------------------------------------------------------------------
// Display topology (QueryDisplayConfig)
//
// Each screen is identified by its monitor device path (stable across
// reconnects), with its GDI name (\\.\DISPLAYn), HMONITOR, internal/external
// kind, HDR state and the SDR white level Windows reports for it.
// -----------------------------------------------------------------------------

namespace Topo {

// DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO_2 (Windows 11 24H2+). Declared here
// because the SDK headers only expose it for newer NTDDI versions.
struct AdvColorInfo2 {
    DISPLAYCONFIG_DEVICE_INFO_HEADER header;
    union {
        struct {
            UINT32 advancedColorSupported : 1;
            UINT32 advancedColorActive : 1;
            UINT32 reserved1 : 1;
            UINT32 advancedColorLimitedByPolicy : 1;
            UINT32 highDynamicRangeSupported : 1;
            UINT32 highDynamicRangeUserEnabled : 1;
            UINT32 wideColorSupported : 1;
            UINT32 wideColorUserEnabled : 1;
            UINT32 reserved : 24;
        };
        UINT32 value;
    };
    DISPLAYCONFIG_COLOR_ENCODING colorEncoding;
    UINT32 bitsPerColorChannel;
    UINT32 activeColorMode;  // 0 = SDR, 1 = WCG, 2 = HDR
};
constexpr UINT32 kAdvColorInfo2Type = 15;
constexpr UINT32 kColorModeHdr = 2;

struct Screen {
    std::wstring id;       // stable key (monitor device path)
    std::wstring gdiName;  // \\.\DISPLAYn
    std::wstring name;     // friendly monitor name
    HMONITOR hMon = nullptr;
    RECT rc{};
    RECT rcWork{};
    bool internal = false;
    bool hdr = false;
    ULONG sdrWhite = 0;  // 1000 = 80 nits, 0 = unknown
    LUID adapterId{};
    UINT32 targetId = 0;
};

bool IsInternalTechnology(DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY t) {
    return t == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL ||
           t == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED ||
           t == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED ||
           t == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_LVDS;
}

// Thread-safe. Returns 0 on failure.
ULONG ReadSdrWhite(LUID adapterId, UINT32 targetId) {
    DISPLAYCONFIG_SDR_WHITE_LEVEL w = {};
    w.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SDR_WHITE_LEVEL;
    w.header.size = sizeof(w);
    w.header.adapterId = adapterId;
    w.header.id = targetId;
    if (DisplayConfigGetDeviceInfo(&w.header) != ERROR_SUCCESS) {
        return 0;
    }
    return w.SDRWhiteLevel;
}

bool ReadHdrActive(LUID adapterId, UINT32 targetId) {
    AdvColorInfo2 a2 = {};
    a2.header.type = (DISPLAYCONFIG_DEVICE_INFO_TYPE)kAdvColorInfo2Type;
    a2.header.size = sizeof(a2);
    a2.header.adapterId = adapterId;
    a2.header.id = targetId;
    if (DisplayConfigGetDeviceInfo(&a2.header) == ERROR_SUCCESS) {
        return a2.activeColorMode == kColorModeHdr;
    }

    // Older builds: advanced color without forced wide color means HDR.
    DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO a = {};
    a.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_ADVANCED_COLOR_INFO;
    a.header.size = sizeof(a);
    a.header.adapterId = adapterId;
    a.header.id = targetId;
    if (DisplayConfigGetDeviceInfo(&a.header) == ERROR_SUCCESS) {
        return a.advancedColorEnabled && !a.wideColorEnforced;
    }
    return false;
}

std::vector<Screen> QueryInternal();

// Rects are always in physical pixels (per-monitor v2), matching how the
// overlay windows are created.
std::vector<Screen> Query() {
    DPI_AWARENESS_CONTEXT prev = SetThreadDpiAwarenessContext(
        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    auto result = QueryInternal();
    if (prev) {
        SetThreadDpiAwarenessContext(prev);
    }
    return result;
}

std::vector<Screen> QueryInternal() {
    std::vector<Screen> out;
    std::vector<DISPLAYCONFIG_PATH_INFO> paths;
    std::vector<DISPLAYCONFIG_MODE_INFO> modes;
    LONG result = ERROR_GEN_FAILURE;
    for (int attempt = 0; attempt < 3; attempt++) {
        UINT32 numPaths = 0, numModes = 0;
        if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &numPaths,
                                        &numModes) != ERROR_SUCCESS) {
            return out;
        }
        paths.resize(numPaths);
        modes.resize(numModes);
        result = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &numPaths,
                                    paths.data(), &numModes, modes.data(),
                                    nullptr);
        if (result == ERROR_INSUFFICIENT_BUFFER) {
            continue;
        }
        paths.resize(numPaths);
        break;
    }
    if (result != ERROR_SUCCESS) {
        Wh_Log(L"QueryDisplayConfig failed: %ld", result);
        return out;
    }

    struct Mon {
        std::wstring name;
        HMONITOR hMon;
        RECT rc;
        RECT rcWork;
    };
    std::vector<Mon> mons;
    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR hMon, HDC, LPRECT, LPARAM lParam) WINAPI_LAMBDA_RETURN(
            BOOL) {
            MONITORINFOEXW mi = {};
            mi.cbSize = sizeof(mi);
            if (GetMonitorInfoW(hMon, &mi)) {
                ((std::vector<Mon>*)lParam)
                    ->push_back({mi.szDevice, hMon, mi.rcMonitor, mi.rcWork});
            }
            return TRUE;
        },
        (LPARAM)&mons);

    for (const auto& p : paths) {
        DISPLAYCONFIG_SOURCE_DEVICE_NAME src = {};
        src.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
        src.header.size = sizeof(src);
        src.header.adapterId = p.sourceInfo.adapterId;
        src.header.id = p.sourceInfo.id;
        if (DisplayConfigGetDeviceInfo(&src.header) != ERROR_SUCCESS) {
            continue;
        }

        DISPLAYCONFIG_TARGET_DEVICE_NAME tgt = {};
        tgt.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
        tgt.header.size = sizeof(tgt);
        tgt.header.adapterId = p.targetInfo.adapterId;
        tgt.header.id = p.targetInfo.id;
        bool haveTarget =
            DisplayConfigGetDeviceInfo(&tgt.header) == ERROR_SUCCESS;

        Screen s;
        s.gdiName = src.viewGdiDeviceName;
        bool duplicate = false;
        for (const auto& existing : out) {
            if (existing.gdiName == s.gdiName) {
                duplicate = true;  // cloned display, keep the first path
                break;
            }
        }
        if (duplicate) {
            continue;
        }

        s.name = haveTarget ? tgt.monitorFriendlyDeviceName : L"";
        s.id = (haveTarget && tgt.monitorDevicePath[0])
                   ? std::wstring(tgt.monitorDevicePath)
                   : s.gdiName;
        s.internal = haveTarget && IsInternalTechnology(tgt.outputTechnology);
        s.adapterId = p.targetInfo.adapterId;
        s.targetId = p.targetInfo.id;
        s.hdr = ReadHdrActive(s.adapterId, s.targetId);
        s.sdrWhite = ReadSdrWhite(s.adapterId, s.targetId);

        for (const auto& m : mons) {
            if (m.name == s.gdiName) {
                s.hMon = m.hMon;
                s.rc = m.rc;
                s.rcWork = m.rcWork;
                break;
            }
        }
        if (!s.hMon) {
            continue;
        }
        out.push_back(std::move(s));
    }
    // Stable order, so an unchanged setup is never seen as a display change.
    std::sort(out.begin(), out.end(),
              [](const Screen& a, const Screen& b) { return a.id < b.id; });
    return out;
}

// "DISPLAY9" from "\\.\DISPLAY9", for logs and the debug OSD.
std::wstring ShortName(const Screen& s) {
    std::wstring n = s.gdiName;
    size_t pos = n.find_last_of(L'\\');
    if (pos != std::wstring::npos) {
        n = n.substr(pos + 1);
    }
    if (!s.name.empty()) {
        n += L" (" + s.name + L")";
    }
    return n;
}

// UI-thread cache.
std::vector<Screen> g_screens;
DWORD g_lastRefreshTick;

const Screen* FindByMonitor(HMONITOR hMon) {
    for (const auto& s : g_screens) {
        if (s.hMon == hMon) {
            return &s;
        }
    }
    return nullptr;
}

const Screen* FindById(const std::wstring& id) {
    for (const auto& s : g_screens) {
        if (s.id == id) {
            return &s;
        }
    }
    return nullptr;
}

}  // namespace Topo

// -----------------------------------------------------------------------------
// Hardware brightness backends. Called on the brightness worker thread only.
// -----------------------------------------------------------------------------

namespace Backlight {

bool OnAcPower() {
    SYSTEM_POWER_STATUS ps;
    if (!GetSystemPowerStatus(&ps)) {
        return true;
    }
    return ps.ACLineStatus != 0;  // 1 = AC, 255 = unknown (treated as AC)
}

// IOCTL on \\.\LCD: direct backlight control of the internal panel.
namespace Ioctl {

#pragma pack(push, 1)
struct DisplayBrightness {
    BYTE ucDisplayPolicy;
    BYTE ucACBrightness;
    BYTE ucDCBrightness;
};
#pragma pack(pop)

constexpr DWORD kQuery = 0x00230498;  // IOCTL_VIDEO_QUERY_DISPLAY_BRIGHTNESS
constexpr DWORD kSet = 0x0023049C;    // IOCTL_VIDEO_SET_DISPLAY_BRIGHTNESS
constexpr BYTE kPolicyBoth = 3;

HANDLE g_hLcd = INVALID_HANDLE_VALUE;

bool Open() {
    if (g_hLcd != INVALID_HANDLE_VALUE) {
        return true;
    }
    g_hLcd = CreateFileW(L"\\\\.\\LCD", GENERIC_READ | GENERIC_WRITE,
                         FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                         OPEN_EXISTING, 0, nullptr);
    return g_hLcd != INVALID_HANDLE_VALUE;
}

void Close() {
    if (g_hLcd != INVALID_HANDLE_VALUE) {
        CloseHandle(g_hLcd);
        g_hLcd = INVALID_HANDLE_VALUE;
    }
}

bool Query(DisplayBrightness* db) {
    if (!Open()) {
        return false;
    }
    DWORD ret = 0;
    *db = {};
    return DeviceIoControl(g_hLcd, kQuery, nullptr, 0, db, sizeof(*db), &ret,
                           nullptr) &&
           ret >= sizeof(*db);
}

// The query's policy says which value the driver currently uses (1 = AC,
// 2 = DC). Tested on build 26300: it reports 2 even on AC power, and only a
// change of the DC value takes effect.
int Read() {
    DisplayBrightness db;
    if (!Query(&db)) {
        return -1;
    }
    switch (db.ucDisplayPolicy) {
        case 1: return db.ucACBrightness;
        case 2: return db.ucDCBrightness;
        default: return OnAcPower() ? db.ucACBrightness : db.ucDCBrightness;
    }
}

// Both values are written: Windows 11 keeps a single brightness for AC and
// battery anyway, and writing only one of them doesn't take effect on some
// drivers (tested: AC-only writes are ignored when the policy is DC).
bool Write(int value) {
    if (!Open()) {
        return false;
    }
    BYTE v = (BYTE)std::clamp(value, 0, 100);
    DisplayBrightness db = {kPolicyBoth, v, v};
    DWORD ret = 0;
    return DeviceIoControl(g_hLcd, kSet, &db, sizeof(db), nullptr, 0, &ret,
                           nullptr);
}

}  // namespace Ioctl

// WMI (root\WMI WmiMonitorBrightness / WmiMonitorBrightnessMethods). The
// connection is cached on the worker thread (MTA).
namespace Wmi {

IWbemServices* g_svc;
BSTR g_methodsPath;
IWbemClassObject* g_inParamsDef;

void Release() {
    if (g_inParamsDef) {
        g_inParamsDef->Release();
        g_inParamsDef = nullptr;
    }
    if (g_methodsPath) {
        SysFreeString(g_methodsPath);
        g_methodsPath = nullptr;
    }
    if (g_svc) {
        g_svc->Release();
        g_svc = nullptr;
    }
}

bool Connect() {
    if (g_svc) {
        return true;
    }
    IWbemLocator* locator = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_WbemLocator, nullptr,
                                  CLSCTX_INPROC_SERVER, IID_IWbemLocator,
                                  (void**)&locator);
    if (FAILED(hr)) {
        return false;
    }
    BSTR ns = SysAllocString(L"ROOT\\WMI");
    hr = locator->ConnectServer(ns, nullptr, nullptr, nullptr, 0, nullptr,
                                nullptr, &g_svc);
    SysFreeString(ns);
    locator->Release();
    if (FAILED(hr) || !g_svc) {
        g_svc = nullptr;
        return false;
    }
    CoSetProxyBlanket(g_svc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
                      RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
                      nullptr, EOAC_NONE);
    return true;
}

IWbemClassObject* QueryFirst(PCWSTR query) {
    if (!Connect()) {
        return nullptr;
    }
    BSTR lang = SysAllocString(L"WQL");
    BSTR q = SysAllocString(query);
    IEnumWbemClassObject* e = nullptr;
    HRESULT hr = g_svc->ExecQuery(
        lang, q, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
        nullptr, &e);
    SysFreeString(lang);
    SysFreeString(q);
    if (FAILED(hr) || !e) {
        return nullptr;
    }
    IWbemClassObject* obj = nullptr;
    ULONG n = 0;
    hr = e->Next(3000, 1, &obj, &n);
    e->Release();
    if (FAILED(hr) || n != 1) {
        return nullptr;
    }
    return obj;
}

int Read() {
    IWbemClassObject* obj =
        QueryFirst(L"SELECT CurrentBrightness FROM WmiMonitorBrightness");
    if (!obj) {
        return -1;
    }
    int result = -1;
    VARIANT v;
    VariantInit(&v);
    if (SUCCEEDED(obj->Get(L"CurrentBrightness", 0, &v, nullptr, nullptr))) {
        if (v.vt == VT_UI1) {
            result = v.bVal;
        } else if (v.vt == VT_I4) {
            result = v.lVal;
        }
    }
    VariantClear(&v);
    obj->Release();
    return result;
}

bool Write(int value) {
    if (!Connect()) {
        return false;
    }
    if (!g_methodsPath) {
        IWbemClassObject* inst =
            QueryFirst(L"SELECT * FROM WmiMonitorBrightnessMethods");
        if (!inst) {
            return false;
        }
        VARIANT v;
        VariantInit(&v);
        if (SUCCEEDED(inst->Get(L"__PATH", 0, &v, nullptr, nullptr)) &&
            v.vt == VT_BSTR) {
            g_methodsPath = SysAllocString(v.bstrVal);
        }
        VariantClear(&v);
        inst->Release();
        if (!g_methodsPath) {
            return false;
        }
    }
    if (!g_inParamsDef) {
        BSTR cls = SysAllocString(L"WmiMonitorBrightnessMethods");
        IWbemClassObject* classObj = nullptr;
        HRESULT hr = g_svc->GetObject(cls, 0, nullptr, &classObj, nullptr);
        SysFreeString(cls);
        if (FAILED(hr) || !classObj) {
            return false;
        }
        hr = classObj->GetMethod(L"WmiSetBrightness", 0, &g_inParamsDef,
                                 nullptr);
        classObj->Release();
        if (FAILED(hr) || !g_inParamsDef) {
            g_inParamsDef = nullptr;
            return false;
        }
    }

    IWbemClassObject* in = nullptr;
    if (FAILED(g_inParamsDef->SpawnInstance(0, &in)) || !in) {
        return false;
    }
    VARIANT timeout;
    VariantInit(&timeout);
    timeout.vt = VT_I4;
    timeout.lVal = 0;
    VARIANT brightness;
    VariantInit(&brightness);
    brightness.vt = VT_UI1;
    brightness.bVal = (BYTE)std::clamp(value, 0, 100);
    bool ok = SUCCEEDED(in->Put(L"Timeout", 0, &timeout, 0)) &&
              SUCCEEDED(in->Put(L"Brightness", 0, &brightness, 0));
    if (ok) {
        BSTR method = SysAllocString(L"WmiSetBrightness");
        HRESULT hr = g_svc->ExecMethod(g_methodsPath, method, 0, nullptr, in,
                                       nullptr, nullptr);
        SysFreeString(method);
        ok = SUCCEEDED(hr);
        if (!ok) {
            // The instance may be stale (e.g. after a display change).
            SysFreeString(g_methodsPath);
            g_methodsPath = nullptr;
        }
    }
    in->Release();
    return ok;
}

}  // namespace Wmi

// Power plan setting (Display brightness). Last resort: Windows applies it
// asynchronously and writes are persisted in the active power plan.
namespace Power {

const GUID kVideoSubgroup = {
    0x7516b95f, 0xf776, 0x4464, {0x8c, 0x53, 0x06, 0x16, 0x7f, 0x40, 0xcc, 0x99}};
const GUID kVideoBrightness = {
    0xaded5e82, 0xb909, 0x4619, {0x99, 0x49, 0xf5, 0xd7, 0x1d, 0xac, 0x0b, 0xcb}};

int Read() {
    GUID* scheme = nullptr;
    if (PowerGetActiveScheme(nullptr, &scheme) != ERROR_SUCCESS || !scheme) {
        return -1;
    }
    DWORD v = 0;
    DWORD r = OnAcPower()
                  ? PowerReadACValueIndex(nullptr, scheme, &kVideoSubgroup,
                                          &kVideoBrightness, &v)
                  : PowerReadDCValueIndex(nullptr, scheme, &kVideoSubgroup,
                                          &kVideoBrightness, &v);
    LocalFree(scheme);
    return r == ERROR_SUCCESS ? (int)v : -1;
}

bool Write(int value) {
    GUID* scheme = nullptr;
    if (PowerGetActiveScheme(nullptr, &scheme) != ERROR_SUCCESS || !scheme) {
        return false;
    }
    // Both indexes, like Windows 11 does itself.
    DWORD v = (DWORD)std::clamp(value, 0, 100);
    DWORD r = PowerWriteACValueIndex(nullptr, scheme, &kVideoSubgroup,
                                     &kVideoBrightness, v);
    if (r == ERROR_SUCCESS) {
        r = PowerWriteDCValueIndex(nullptr, scheme, &kVideoSubgroup,
                                   &kVideoBrightness, v);
    }
    if (r == ERROR_SUCCESS) {
        r = PowerSetActiveScheme(nullptr, scheme);
    }
    LocalFree(scheme);
    return r == ERROR_SUCCESS;
}

}  // namespace Power

enum class Method { None, Ioctl, Wmi, Power };

Method g_method = Method::None;
int g_configuredMethod = -1;

PCWSTR MethodName(Method m) {
    switch (m) {
        case Method::Ioctl: return L"IOCTL";
        case Method::Wmi: return L"WMI";
        case Method::Power: return L"power plan";
        default: return L"none";
    }
}

Method ResolveMethod() {
    LaptopMethod configured = Cfg()->laptopMethod;
    if ((int)configured != g_configuredMethod) {
        g_configuredMethod = (int)configured;
        g_method = Method::None;
    }
    if (g_method != Method::None) {
        return g_method;
    }
    switch (configured) {
        case LaptopMethod::Ioctl:
            g_method = Method::Ioctl;
            break;
        case LaptopMethod::Wmi:
            g_method = Method::Wmi;
            break;
        case LaptopMethod::PowerScheme:
            g_method = Method::Power;
            break;
        case LaptopMethod::Auto:
            if (Ioctl::Read() >= 0) {
                g_method = Method::Ioctl;
            } else if (Wmi::Read() >= 0) {
                g_method = Method::Wmi;
            } else if (Power::Read() >= 0) {
                g_method = Method::Power;
            }
            break;
    }
    Wh_Log(L"Laptop backlight method: %s", MethodName(g_method));
    return g_method;
}

Method CurrentMethod() {
    return g_method;
}

int Read() {
    switch (ResolveMethod()) {
        case Method::Ioctl: return Ioctl::Read();
        case Method::Wmi: return Wmi::Read();
        case Method::Power: return Power::Read();
        default: return -1;
    }
}

bool WriteWith(Method m, int value) {
    switch (m) {
        case Method::Ioctl: return Ioctl::Write(value);
        case Method::Wmi: return Wmi::Write(value);
        case Method::Power: return Power::Write(value);
        default: return false;
    }
}

bool Write(int value) {
    Method m = ResolveMethod();
    if (WriteWith(m, value)) {
        return true;
    }
    if (g_configuredMethod != (int)LaptopMethod::Auto) {
        return false;
    }
    // Auto: fall back to the next method that works.
    for (Method next : {Method::Ioctl, Method::Wmi, Method::Power}) {
        if (next == m) {
            continue;
        }
        if (WriteWith(next, value)) {
            Wh_Log(L"Laptop backlight: %s failed, switched to %s",
                   MethodName(m), MethodName(next));
            g_method = next;
            return true;
        }
    }
    return false;
}

// What the panel actually shows, independent of the write method (the power
// plan value alone changes before the panel does).
int ReadPanel() {
    int v = Ioctl::Read();
    if (v < 0) {
        v = Wmi::Read();
    }
    return v;
}

// Auto only. Some drivers accept a write (the call succeeds) but never apply
// it. After a few writes in a row that left the panel exactly where it was,
// the next method that can read the brightness is used instead.
int g_ignoredWrites;
constexpr int kIgnoredWritesBeforeSwitch = 3;

void NoteWriteOutcome(bool applied, bool panelDidNotMove) {
    if (applied || !panelDidNotMove) {
        g_ignoredWrites = 0;
        return;
    }
    if (g_configuredMethod != (int)LaptopMethod::Auto ||
        ++g_ignoredWrites < kIgnoredWritesBeforeSwitch) {
        return;
    }
    g_ignoredWrites = 0;
    Method next = Method::None;
    if (g_method == Method::Ioctl && Wmi::Read() >= 0) {
        next = Method::Wmi;
    } else if ((g_method == Method::Ioctl || g_method == Method::Wmi) &&
               Power::Read() >= 0) {
        next = Method::Power;
    }
    if (next != Method::None) {
        Wh_Log(L"Laptop backlight: %s writes are ignored by the driver, "
               L"switched to %s",
               MethodName(g_method), MethodName(next));
        g_method = next;
    }
}

DWORD PollIntervalMs() {
    return g_method == Method::Ioctl ? 1000 : 3000;
}

void Shutdown() {
    Ioctl::Close();
    Wmi::Release();
    g_method = Method::None;
    g_configuredMethod = -1;
    g_ignoredWrites = 0;
}

}  // namespace Backlight

// DDC/CI for external monitors. Physical monitor handles are cached per screen
// on the worker thread and dropped on every display change.
namespace Ddc {

struct Phys {
    HANDLE h;
    DWORD min;
    DWORD max;
    bool ok;
};

struct Entry {
    std::vector<PHYSICAL_MONITOR> list;
    std::vector<Phys> phys;
    bool probed = false;
    bool ok = false;
    DWORD lastCommandTick = 0;
};

std::unordered_map<std::wstring, Entry> g_cache;

// DDC/CI needs ~50 ms between commands to the same monitor.
void Space(Entry& e) {
    DWORD elapsed = GetTickCount() - e.lastCommandTick;
    if (elapsed < 50) {
        Sleep(50 - elapsed);
    }
}

Entry& Get(const std::wstring& id, HMONITOR hMon) {
    Entry& e = g_cache[id];
    if (e.probed) {
        return e;
    }
    e.probed = true;
    DWORD n = 0;
    if (!hMon || !GetNumberOfPhysicalMonitorsFromHMONITOR(hMon, &n) || !n) {
        return e;
    }
    e.list.resize(n);
    if (!GetPhysicalMonitorsFromHMONITOR(hMon, n, e.list.data())) {
        e.list.clear();
        return e;
    }
    for (const auto& pm : e.list) {
        DWORD mn = 0, cur = 0, mx = 0;
        bool ok = GetMonitorBrightness(pm.hPhysicalMonitor, &mn, &cur, &mx) &&
                  mx > mn;
        e.lastCommandTick = GetTickCount();
        e.phys.push_back({pm.hPhysicalMonitor, mn, mx, ok});
        if (ok) {
            e.ok = true;
        }
    }
    Wh_Log(L"DDC/CI probe: %s", e.ok ? L"supported" : L"not supported");
    return e;
}

int Read(const std::wstring& id, HMONITOR hMon) {
    Entry& e = Get(id, hMon);
    if (!e.ok) {
        return -1;
    }
    for (auto& p : e.phys) {
        if (!p.ok) {
            continue;
        }
        Space(e);
        DWORD mn = 0, cur = 0, mx = 0;
        BOOL r = GetMonitorBrightness(p.h, &mn, &cur, &mx);
        e.lastCommandTick = GetTickCount();
        if (r && mx > mn) {
            cur = std::clamp(cur, mn, mx);
            return (int)(((cur - mn) * 100 + (mx - mn) / 2) / (mx - mn));
        }
    }
    return -1;
}

bool Write(const std::wstring& id, HMONITOR hMon, int percent) {
    Entry& e = Get(id, hMon);
    if (!e.ok) {
        return false;
    }
    percent = std::clamp(percent, 0, 100);
    bool any = false;
    for (auto& p : e.phys) {
        if (!p.ok) {
            continue;
        }
        Space(e);
        DWORD v = p.min + ((p.max - p.min) * (DWORD)percent + 50) / 100;
        if (SetMonitorBrightness(p.h, v)) {
            any = true;
        }
        e.lastCommandTick = GetTickCount();
    }
    return any;
}

void Reset() {
    for (auto& [id, e] : g_cache) {
        if (!e.list.empty()) {
            DestroyPhysicalMonitors((DWORD)e.list.size(), e.list.data());
        }
    }
    g_cache.clear();
}

}  // namespace Ddc

// SDR content brightness on HDR screens via the undocumented
// DwmpSDRToHDRBoost (dwmapi.dll ordinal 171). Note: tested on Windows 11
// build 26300, this does NOT change the SDR white level reported by
// DisplayConfigGetDeviceInfo, which keeps the Windows Settings value.
namespace Sdr {

using DwmpSDRToHDRBoost_t = void(WINAPI*)(HMONITOR hMonitor, double boost);

DwmpSDRToHDRBoost_t GetBoostFunction() {
    static DwmpSDRToHDRBoost_t pFunc = []() {
        HMODULE dwmapi = GetModuleHandle(L"dwmapi.dll");
        if (!dwmapi) {
            dwmapi = LoadLibraryEx(L"dwmapi.dll", nullptr,
                                   LOAD_LIBRARY_SEARCH_SYSTEM32);
        }
        auto p = dwmapi ? (DwmpSDRToHDRBoost_t)GetProcAddress(
                              dwmapi, MAKEINTRESOURCEA(171))
                        : nullptr;
        if (!p) {
            Wh_Log(L"DwmpSDRToHDRBoost (dwmapi #171) not found");
        }
        return p;
    }();
    return pFunc;
}

// Set once the mod has changed the SDR brightness of a screen.
std::atomic<bool> g_written;

// raw: SDR white level in thousandths of 80 nits (1000 = 80 nits).
bool Write(HMONITOR hMon, int raw) {
    auto pFunc = GetBoostFunction();
    if (!pFunc || !hMon) {
        return false;
    }
    double boost = std::max(raw / 1000.0, 1.0001);
    pFunc(hMon, boost);
    g_written = true;
    return true;
}

// Puts the SDR brightness of every HDR screen back to its Windows Settings
// level (which DwmpSDRToHDRBoost doesn't change), so that disabling the mod
// leaves the screens as Windows Settings shows them.
void RestoreSettingsLevels() {
    if (!g_written.exchange(false)) {
        return;
    }
    for (const auto& screen : Topo::Query()) {
        if (screen.hdr && screen.sdrWhite) {
            Write(screen.hMon, (int)screen.sdrWhite);
        }
    }
}

}  // namespace Sdr

// -----------------------------------------------------------------------------
// Brightness worker thread
//
// The taskbar thread never touches brightness hardware. It keeps the target
// value per screen and hands it to this thread, which:
//   1. applies only the latest target (intermediate values are skipped),
//   2. waits until Windows confirms a backlight write before the next one, so
//      requests can't pile up and be applied out of order (this was the cause
//      of the laptop slider jumping back and forth),
//   3. reads hardware values back only when the user is idle, to detect
//      changes made outside the mod (Fn keys, Windows Settings, ...).
// -----------------------------------------------------------------------------

namespace Worker {

enum Domain : int { kBacklight = 0, kDdc = 1, kSdr = 2, kDomainCount = 3 };

PCWSTR DomainName(int d) {
    switch (d) {
        case kBacklight: return L"backlight";
        case kDdc: return L"DDC/CI";
        case kSdr: return L"SDR";
    }
    return L"?";
}

constexpr DWORD kIdleBeforePollMs = 1500;
constexpr int kBacklightTolerance = 1;  // percent
constexpr int kSdrTolerance = 15;       // thousandths of 80 nits

struct Slot {
    std::wstring screenId;
    int domain = 0;
    // Shared, guarded by g_lock.
    HMONITOR hMon = nullptr;
    LUID adapterId{};
    UINT32 targetId = 0;
    int pendingTarget = -1;
    DWORD lastInputTick = 0;
    bool watch = false;
    bool present = true;
    // Set when watching starts (or restarts, e.g. HDR turned back on): the
    // next poll only records the current value as the baseline, since what
    // was read before can't be compared with it.
    bool rebase = false;
    // Worker thread only.
    int lastWrite = -1;
    int lastRead = -1;
    DWORD lastWriteTick = 0;
    DWORD lastPollTick = 0;
    bool needSettle = false;
};

struct ReadRequest {
    std::wstring screenId;
    int domain = 0;
    HMONITOR hMon = nullptr;
    LUID adapterId{};
    UINT32 targetId = 0;
    bool notifyUi = false;
    std::atomic<int> result{-2};
    HANDLE hDone = nullptr;
    ~ReadRequest() {
        if (hDone) {
            CloseHandle(hDone);
        }
    }
};

enum class MsgKind { Initial, Settled, External, Written };

struct HwMsg {
    MsgKind kind;
    std::wstring screenId;
    int domain;
    int value;
    DWORD elapsedMs;
};

UINT g_msgHw = RegisterWindowMessage(L"Windhawk_HwResult_" WH_MOD_ID);

SRWLOCK g_lock = SRWLOCK_INIT;
std::vector<std::shared_ptr<Slot>> g_slots;
std::vector<std::shared_ptr<ReadRequest>> g_requests;
HANDLE g_hEvent;
HANDLE g_hThread;
std::atomic<bool> g_stop;
std::atomic<UINT> g_generation;
UINT g_seenGeneration;

void PostToUi(HwMsg* msg) {
    HWND hWnd = g_hController;
    if (!hWnd || !PostMessage(hWnd, g_msgHw, 0, (LPARAM)msg)) {
        delete msg;
    }
}

std::shared_ptr<Slot> FindSlotLocked(const std::wstring& id, int domain) {
    for (auto& s : g_slots) {
        if (s->domain == domain && s->screenId == id) {
            return s;
        }
    }
    return nullptr;
}

// UI thread. Creates the slot if needed and refreshes its monitor info.
std::shared_ptr<Slot> GetSlotLocked(const Topo::Screen& screen, int domain) {
    auto slot = FindSlotLocked(screen.id, domain);
    if (!slot) {
        slot = std::make_shared<Slot>();
        slot->screenId = screen.id;
        slot->domain = domain;
        g_slots.push_back(slot);
    }
    slot->hMon = screen.hMon;
    slot->adapterId = screen.adapterId;
    slot->targetId = screen.targetId;
    slot->present = true;
    return slot;
}

void SetTarget(const Topo::Screen& screen, int domain, int value) {
    AcquireSRWLockExclusive(&g_lock);
    auto slot = GetSlotLocked(screen, domain);
    slot->pendingTarget = value;
    slot->lastInputTick = GetTickCount();
    ReleaseSRWLockExclusive(&g_lock);
    if (g_hEvent) {
        SetEvent(g_hEvent);
    }
}

void NoteInput(const Topo::Screen& screen, int domain) {
    AcquireSRWLockExclusive(&g_lock);
    GetSlotLocked(screen, domain)->lastInputTick = GetTickCount();
    ReleaseSRWLockExclusive(&g_lock);
}

void SetWatch(const Topo::Screen& screen, int domain, bool watch) {
    AcquireSRWLockExclusive(&g_lock);
    auto slot = GetSlotLocked(screen, domain);
    if (watch && !slot->watch) {
        slot->rebase = true;
    }
    slot->watch = watch;
    ReleaseSRWLockExclusive(&g_lock);
    if (g_hEvent) {
        SetEvent(g_hEvent);  // re-evaluate the wait timeout
    }
}

std::shared_ptr<ReadRequest> QueueRead(const Topo::Screen& screen,
                                       int domain,
                                       bool notifyUi,
                                       bool wait) {
    auto req = std::make_shared<ReadRequest>();
    req->screenId = screen.id;
    req->domain = domain;
    req->hMon = screen.hMon;
    req->adapterId = screen.adapterId;
    req->targetId = screen.targetId;
    req->notifyUi = notifyUi;
    if (wait) {
        req->hDone = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    }
    AcquireSRWLockExclusive(&g_lock);
    GetSlotLocked(screen, domain);
    g_requests.push_back(req);
    ReleaseSRWLockExclusive(&g_lock);
    if (g_hEvent) {
        SetEvent(g_hEvent);
    }
    return req;
}

// UI thread: blocks up to timeoutMs. Returns -1 on failure/timeout.
int ReadSync(const Topo::Screen& screen, int domain, DWORD timeoutMs) {
    if (!g_hThread) {
        return -1;
    }
    auto req = QueueRead(screen, domain, false, true);
    if (!req->hDone ||
        WaitForSingleObject(req->hDone, timeoutMs) != WAIT_OBJECT_0) {
        Wh_Log(L"Worker: synchronous %s read timed out", DomainName(domain));
        return -1;
    }
    int r = req->result;
    return r >= 0 ? r : -1;
}

void ReadAsync(const Topo::Screen& screen, int domain) {
    if (g_hThread) {
        QueueRead(screen, domain, true, false);
    }
}

// UI thread, after a topology refresh.
void UpdateScreens(const std::vector<Topo::Screen>& screens) {
    AcquireSRWLockExclusive(&g_lock);
    for (auto& slot : g_slots) {
        slot->present = false;
        for (const auto& s : screens) {
            if (s.id == slot->screenId) {
                slot->hMon = s.hMon;
                slot->adapterId = s.adapterId;
                slot->targetId = s.targetId;
                slot->present = true;
                break;
            }
        }
    }
    ReleaseSRWLockExclusive(&g_lock);
    g_generation++;
    if (g_hEvent) {
        SetEvent(g_hEvent);
    }
}

int DoRead(int domain,
           const std::wstring& id,
           HMONITOR hMon,
           LUID adapterId,
           UINT32 targetId) {
    switch (domain) {
        case kBacklight:
            return Backlight::Read();
        case kDdc:
            return Ddc::Read(id, hMon);
        case kSdr: {
            ULONG w = Topo::ReadSdrWhite(adapterId, targetId);
            return w ? (int)w : -1;
        }
    }
    return -1;
}

bool DoWrite(Slot& slot, HMONITOR hMon, int value) {
    switch (slot.domain) {
        case kBacklight: {
            // Where the panel was before this write (after the previous write
            // or read), to spot writes that the driver silently ignores.
            int before = slot.lastRead;
            if (!Backlight::Write(value)) {
                return false;
            }
            // Windows mirrors every backlight change into the power plan
            // asynchronously (~100-200 ms later) and re-applies the power plan
            // value to the panel. If the next write happens before that, an
            // older value gets replayed and the brightness jumps back and
            // forth. So wait until both the panel and the power plan report
            // the new value (tested on build 26300).
            DWORD t0 = GetTickCount();
            int panel = -1;
            for (;;) {
                Sleep(20);
                panel = Backlight::ReadPanel();
                int plan = Backlight::Power::Read();
                bool panelOk = panel == value || panel < 0;
                bool planOk = plan == value || plan < 0;
                if ((panelOk && planOk) || GetTickCount() - t0 > 700) {
                    break;
                }
            }
            Backlight::NoteWriteOutcome(
                panel == value || panel < 0,
                panel >= 0 && panel == before && before != value);
            slot.lastRead = Backlight::Read();
            return true;
        }
        case kDdc:
            return Ddc::Write(slot.screenId, hMon, value);
        case kSdr:
            return Sdr::Write(hMon, value);
    }
    return false;
}

DWORD PollIntervalMs(int domain) {
    switch (domain) {
        case kBacklight: return Backlight::PollIntervalMs();
        case kDdc: return 5000;
        case kSdr: return 1500;
    }
    return 5000;
}

void ProcessRequests() {
    std::vector<std::shared_ptr<ReadRequest>> requests;
    AcquireSRWLockExclusive(&g_lock);
    requests.swap(g_requests);
    ReleaseSRWLockExclusive(&g_lock);

    for (auto& rq : requests) {
        int v = DoRead(rq->domain, rq->screenId, rq->hMon, rq->adapterId,
                       rq->targetId);
        rq->result = v;
        AcquireSRWLockShared(&g_lock);
        auto slot = FindSlotLocked(rq->screenId, rq->domain);
        ReleaseSRWLockShared(&g_lock);
        if (slot && v >= 0) {
            slot->lastRead = v;
            slot->lastPollTick = GetTickCount();
        }
        Wh_Log(L"Worker: read %s = %d", DomainName(rq->domain), v);
        if (rq->hDone) {
            SetEvent(rq->hDone);
        }
        if (rq->notifyUi) {
            PostToUi(new HwMsg{MsgKind::Initial, rq->screenId, rq->domain, v,
                               0});
        }
    }
}

void ProcessWrites() {
    struct Job {
        std::shared_ptr<Slot> slot;
        HMONITOR hMon;
        int value;
    };
    std::vector<Job> jobs;
    AcquireSRWLockExclusive(&g_lock);
    for (auto& slot : g_slots) {
        if (slot->pendingTarget >= 0 && slot->present) {
            jobs.push_back({slot, slot->hMon, slot->pendingTarget});
            slot->pendingTarget = -1;
        }
    }
    ReleaseSRWLockExclusive(&g_lock);

    bool debugOsd = Cfg()->debugOsd;
    for (auto& job : jobs) {
        DWORD t0 = GetTickCount();
        bool ok = DoWrite(*job.slot, job.hMon, job.value);
        DWORD elapsed = GetTickCount() - t0;
        if (ok) {
            job.slot->lastWrite = job.value;
            job.slot->needSettle = job.slot->domain != kSdr;
        }
        job.slot->lastWriteTick = GetTickCount();
        Wh_Log(L"Worker: write %s = %d %s (%lu ms)",
               DomainName(job.slot->domain), job.value, ok ? L"ok" : L"FAILED",
               elapsed);
        if (!ok) {
            Wh_Log(L"Brightness write failed (%s = %d)",
                   DomainName(job.slot->domain), job.value);
        }
        if (debugOsd) {
            PostToUi(new HwMsg{MsgKind::Written, job.slot->screenId,
                               job.slot->domain, job.value, elapsed});
        }
    }
}

void ProcessPolls() {
    struct Job {
        std::shared_ptr<Slot> slot;
        HMONITOR hMon;
        LUID adapterId;
        UINT32 targetId;
        bool rebase;
    };
    std::vector<Job> jobs;
    DWORD now = GetTickCount();
    AcquireSRWLockShared(&g_lock);
    for (auto& slot : g_slots) {
        if (!slot->watch || !slot->present || slot->pendingTarget >= 0) {
            continue;
        }
        if (now - slot->lastInputTick < kIdleBeforePollMs ||
            now - slot->lastWriteTick < kIdleBeforePollMs) {
            continue;
        }
        if (now - slot->lastPollTick < PollIntervalMs(slot->domain)) {
            continue;
        }
        jobs.push_back({slot, slot->hMon, slot->adapterId, slot->targetId,
                        slot->rebase});
    }
    ReleaseSRWLockShared(&g_lock);

    for (auto& job : jobs) {
        Slot& s = *job.slot;
        int r = DoRead(s.domain, s.screenId, job.hMon, job.adapterId,
                       job.targetId);
        s.lastPollTick = GetTickCount();
        if (r < 0) {
            continue;  // a pending rebase stays pending
        }

        if (s.needSettle || job.rebase) {
            // First read after our own writes, or after watching (re)started:
            // this is where the hardware actually is. Not an external change.
            if (job.rebase) {
                AcquireSRWLockExclusive(&g_lock);
                s.rebase = false;
                ReleaseSRWLockExclusive(&g_lock);
            }
            s.needSettle = false;
            s.lastRead = r;
            s.lastWrite = -1;
            PostToUi(new HwMsg{MsgKind::Settled, s.screenId, s.domain, r, 0});
            continue;
        }

        bool external = false;
        if (s.domain == kSdr) {
            // Our SDR writes are not reflected in the reported white level,
            // so only a change of the reported value counts (the Windows
            // Settings slider moved).
            external = s.lastRead >= 0 &&
                       std::abs(r - s.lastRead) > kSdrTolerance &&
                       (s.lastWrite < 0 ||
                        std::abs(r - s.lastWrite) > kSdrTolerance);
        } else {
            int known = s.lastWrite >= 0 ? s.lastWrite : s.lastRead;
            external = known >= 0 && std::abs(r - known) > kBacklightTolerance;
        }

        if (external) {
            Wh_Log(L"Worker: external %s change detected: %d -> %d",
                   DomainName(s.domain), s.lastWrite >= 0 ? s.lastWrite : s.lastRead,
                   r);
            s.lastWrite = -1;
            PostToUi(
                new HwMsg{MsgKind::External, s.screenId, s.domain, r, 0});
        }
        s.lastRead = r;
    }
}

// Sleep until woken when nothing is watched (no idle wakeups at all);
// otherwise wake twice a second to run the outside-change polls.
DWORD WaitTimeoutMs() {
    bool anyWatched = false;
    AcquireSRWLockShared(&g_lock);
    for (const auto& slot : g_slots) {
        if (slot->watch && slot->present) {
            anyWatched = true;
            break;
        }
    }
    ReleaseSRWLockShared(&g_lock);
    return anyWatched ? 500 : INFINITE;
}

DWORD WINAPI ThreadProc(LPVOID) {
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    while (!g_stop) {
        WaitForSingleObject(g_hEvent, WaitTimeoutMs());
        if (g_stop) {
            break;
        }
        UINT generation = g_generation;
        if (generation != g_seenGeneration) {
            g_seenGeneration = generation;
            Ddc::Reset();
        }
        ProcessRequests();
        ProcessWrites();
        ProcessPolls();
    }
    Ddc::Reset();
    Backlight::Shutdown();
    if (SUCCEEDED(hrCo)) {
        CoUninitialize();
    }
    return 0;
}

void Start() {
    if (g_hThread) {
        return;
    }
    g_stop = false;
    g_hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    g_hThread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
    if (!g_hThread) {
        Wh_Log(L"Failed to start the brightness worker");
    }
}

void Stop() {
    if (g_hThread) {
        g_stop = true;
        SetEvent(g_hEvent);
        // Must not return while the thread can still run code of this DLL.
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }
    if (g_hEvent) {
        CloseHandle(g_hEvent);
        g_hEvent = nullptr;
    }
    AcquireSRWLockExclusive(&g_lock);
    g_slots.clear();
    for (auto& rq : g_requests) {
        if (rq->hDone) {
            SetEvent(rq->hDone);
        }
    }
    g_requests.clear();
    ReleaseSRWLockExclusive(&g_lock);
}

void Wake() {
    if (g_hEvent) {
        SetEvent(g_hEvent);
    }
}

}  // namespace Worker

// -----------------------------------------------------------------------------
// Controller window
//
// A hidden window on the taskbar thread that owns all of this mod's windows
// (Dark Overlays, OSD) and timers. Everything is created and destroyed on that
// thread, so that nothing is left on unload (timers firing into an unloaded
// DLL, windows that can't be destroyed from another thread).
// -----------------------------------------------------------------------------

namespace Controller {

PCWSTR kClassName = L"WhTaskbarExtraScroll_Controller";
PCWSTR kDarkClassName = L"WhTaskbarExtraScroll_DarkOverlay";
PCWSTR kOsdClassName = L"WhTaskbarExtraScroll_Osd";

enum TimerId : UINT_PTR {
    kTimerAnim = 1,
    kTimerRaise,
    kTimerTopology,
    kTimerSdrSave,
    kTimerSndVol,
    kTimerPerAppTooltip,
};

UINT g_msgApplySettings =
    RegisterWindowMessage(L"Windhawk_ApplySettings_" WH_MOD_ID);

bool g_classesRegistered;

}  // namespace Controller

// -----------------------------------------------------------------------------
// Window bands
// -----------------------------------------------------------------------------

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

// ZBID_SYSTEM_TOOLS, an undocumented window band above the bands of the
// taskbar thumbnail previews, Start, the notification center and the other
// shell popups. Explorer may create windows in it.
constexpr DWORD kZbidSystemTools = 16;

// A popup window of this mod (Dark Overlay, OSD). With aboveSystemUi, it's
// created in the system tools band, so it's above the shell popups; otherwise,
// or if that fails, it's a normal window. *inBand tells which one it got.
HWND CreateModPopup(DWORD dwExStyle,
                    PCWSTR className,
                    const RECT& rc,
                    bool aboveSystemUi,
                    bool* inBand) {
    HWND hWnd = nullptr;
    if (aboveSystemUi) {
        static auto pCreateWindowInBand = (CreateWindowInBand_t)GetProcAddress(
            GetModuleHandle(L"user32.dll"), "CreateWindowInBand");
        if (pCreateWindowInBand) {
            hWnd = pCreateWindowInBand(
                dwExStyle, className, L"", WS_POPUP, rc.left, rc.top,
                rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr,
                ThisModule(), nullptr, kZbidSystemTools);
            if (!hWnd) {
                Wh_Log(L"%s: CreateWindowInBand failed (%lu), using a normal "
                       L"topmost window",
                       className, GetLastError());
            }
        }
    }
    if (inBand) {
        *inBand = hWnd != nullptr;
    }
    if (!hWnd) {
        hWnd = CreateWindowExW(dwExStyle, className, L"", WS_POPUP, rc.left,
                               rc.top, rc.right - rc.left, rc.bottom - rc.top,
                               nullptr, nullptr, ThisModule(), nullptr);
    }
    return hWnd;
}

// -----------------------------------------------------------------------------
// On-screen indicator (OSD)
// -----------------------------------------------------------------------------

namespace Osd {

enum class Icon { Brightness, Dark, Mic, Warning };

struct Model {
    Icon icon = Icon::Brightness;
    std::wstring label;
    std::wstring value;
    std::wstring secondary;
    std::wstring debug;
    bool showBar = true;
    bool bipolar = false;  // bar spans -100..100 instead of 0..100
    int level = 0;
    int maxLevel = 100;  // bipolar only: 0 for overlay-only screens
};

constexpr UINT_PTR kHideTimerId = 1;
constexpr UINT kHideTimeoutMs = 1600;
constexpr BYTE kWindowAlpha = 245;

HWND g_hWnd;
bool g_aboveSystemUi;  // the band g_hWnd was created for
HMONITOR g_hMon;       // the screen it's shown on
Model g_model;
UINT g_dpi = 96;
SIZE g_size{};
bool g_useFluentIcons = true;
bool g_iconFontChecked;

int S(int v) {
    return MulDiv(v, g_dpi, 96);
}

struct Theme {
    COLORREF bg, border, text, text2, track, accent, dark, tick;
};

COLORREF Mix(COLORREF a, COLORREF b, double t) {
    return RGB((int)(GetRValue(a) + (GetRValue(b) - GetRValue(a)) * t),
               (int)(GetGValue(a) + (GetGValue(b) - GetGValue(a)) * t),
               (int)(GetBValue(a) + (GetBValue(b) - GetBValue(a)) * t));
}

Theme GetTheme() {
    DWORD light = 0;
    DWORD size = sizeof(light);
    RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &light, &size);

    DWORD accent = 0;
    size = sizeof(accent);
    COLORREF ac = RGB(0, 120, 215);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                     L"AccentColor", RRF_RT_REG_DWORD, nullptr, &accent,
                     &size) == ERROR_SUCCESS) {
        // ABGR
        ac = RGB(accent & 0xFF, (accent >> 8) & 0xFF, (accent >> 16) & 0xFF);
    }

    if (light) {
        return {RGB(249, 249, 249), RGB(220, 220, 220), RGB(26, 26, 26),
                RGB(96, 96, 96),    RGB(205, 205, 205), ac,
                RGB(98, 80, 170),   RGB(140, 140, 140)};
    }
    return {RGB(43, 43, 43),        RGB(68, 68, 68),
            RGB(255, 255, 255),     RGB(190, 190, 190),
            RGB(85, 85, 85),        Mix(ac, RGB(255, 255, 255), 0.35),
            RGB(150, 130, 225),     RGB(160, 160, 160)};
}

HFONT CreateTextFont(int px, int weight) {
    return CreateFontW(-px, 0, 0, 0, weight, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
}

HFONT CreateIconFont(HDC dc, int px) {
    PCWSTR face =
        g_useFluentIcons ? L"Segoe Fluent Icons" : L"Segoe MDL2 Assets";
    HFONT f = CreateFontW(-px, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                          DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                          DEFAULT_PITCH, face);
    if (!g_iconFontChecked && f) {
        // CreateFont silently substitutes missing fonts; check what we got.
        g_iconFontChecked = true;
        HGDIOBJ old = SelectObject(dc, f);
        WCHAR got[LF_FACESIZE] = {};
        GetTextFaceW(dc, LF_FACESIZE, got);
        SelectObject(dc, old);
        if (_wcsicmp(got, face) != 0 && g_useFluentIcons) {
            g_useFluentIcons = false;
            DeleteObject(f);
            return CreateIconFont(dc, px);
        }
    }
    return f;
}

WCHAR Glyph(Icon icon) {
    switch (icon) {
        case Icon::Brightness: return 0xE706;
        case Icon::Dark: return 0xE708;
        case Icon::Mic: return 0xE720;
        case Icon::Warning: return 0xE7BA;
    }
    return L'?';
}

void FillBox(HDC dc, int l, int t, int r, int b, COLORREF c) {
    if (r <= l || b <= t) {
        return;
    }
    RECT rc = {l, t, r, b};
    HBRUSH br = CreateSolidBrush(c);
    FillRect(dc, &rc, br);
    DeleteObject(br);
}

void Paint(HWND hWnd) {
    PAINTSTRUCT ps;
    HDC hdcWnd = BeginPaint(hWnd, &ps);
    RECT rc;
    GetClientRect(hWnd, &rc);
    int w = rc.right, h = rc.bottom;

    HDC dc = CreateCompatibleDC(hdcWnd);
    HBITMAP bmp = CreateCompatibleBitmap(hdcWnd, w, h);
    HGDIOBJ oldBmp = SelectObject(dc, bmp);
    Theme t = GetTheme();

    FillBox(dc, 0, 0, w, h, t.bg);
    HPEN pen = CreatePen(PS_SOLID, 1, t.border);
    HGDIOBJ oldPen = SelectObject(dc, pen);
    HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
    RoundRect(dc, 0, 0, w, h, S(16), S(16));
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(pen);

    SetBkMode(dc, TRANSPARENT);
    int iconBox = S(52);
    int left = iconBox;
    int right = w - S(16);

    // Icon, centered on the label + bar block.
    {
        HFONT f = CreateIconFont(dc, S(20));
        HGDIOBJ old = SelectObject(dc, f);
        SetTextColor(dc, t.text);
        WCHAR glyph = Glyph(g_model.icon);
        RECT ri = {0, 0, iconBox, S(54)};
        DrawTextW(dc, &glyph, 1, &ri,
                  DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
        SelectObject(dc, old);
        DeleteObject(f);
    }

    // Label (left) and value (right).
    {
        HFONT f = CreateTextFont(S(14), FW_SEMIBOLD);
        HGDIOBJ old = SelectObject(dc, f);
        SetTextColor(dc, t.text);
        RECT rv = {left, S(10), right, S(32)};
        DrawTextW(dc, g_model.value.c_str(), -1, &rv,
                  DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
        SIZE valueSize{};
        GetTextExtentPoint32W(dc, g_model.value.c_str(),
                              (int)g_model.value.size(), &valueSize);
        RECT rl = {left, S(10), right - valueSize.cx - S(8), S(32)};
        DrawTextW(dc, g_model.label.c_str(), -1, &rl,
                  DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        SelectObject(dc, old);
        DeleteObject(f);
    }

    // Bar.
    if (g_model.showBar) {
        int top = S(38);
        int bottom = top + std::max(2, S(4));
        FillBox(dc, left, top, right, bottom, t.track);
        int width = right - left;
        if (g_model.bipolar) {
            // -100 (left) ... 0 (center) ... 100 (right). The part below 0
            // (Dark Overlay) uses its own color.
            int mid = left + width / 2;
            int level = std::clamp(g_model.level, -100, g_model.maxLevel);
            int pos = left + (int)((level + 100) * (long long)width / 200);
            FillBox(dc, left, top, std::min(pos, mid), bottom, t.dark);
            if (pos > mid) {
                FillBox(dc, mid, top, pos, bottom, t.accent);
            }
            int tickW = std::max(1, S(2));
            FillBox(dc, mid - tickW / 2, top - S(3), mid - tickW / 2 + tickW,
                    bottom + S(3), t.tick);
        } else {
            int level = std::clamp(g_model.level, 0, 100);
            int pos = left + (int)(level * (long long)width / 100);
            FillBox(dc, left, top, pos, bottom, t.accent);
        }
    }

    // Secondary and debug lines.
    {
        int y = S(50);
        HFONT f = CreateTextFont(S(12), FW_NORMAL);
        HGDIOBJ old = SelectObject(dc, f);
        SetTextColor(dc, t.text2);
        for (const std::wstring* line : {&g_model.secondary, &g_model.debug}) {
            if (line->empty()) {
                continue;
            }
            RECT r = {left, y, right, y + S(16)};
            DrawTextW(dc, line->c_str(), -1, &r,
                      DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
            y += S(18);
        }
        SelectObject(dc, old);
        DeleteObject(f);
    }

    BitBlt(hdcWnd, 0, 0, w, h, dc, 0, 0, SRCCOPY);
    SelectObject(dc, oldBmp);
    DeleteObject(bmp);
    DeleteDC(dc);
    EndPaint(hWnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT:
            Paint(hWnd);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_TIMER:
            if (wParam == kHideTimerId) {
                KillTimer(hWnd, kHideTimerId);
                ShowWindow(hWnd, SW_HIDE);
                return 0;
            }
            break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool EnsureWindow() {
    if (g_hWnd && IsWindow(g_hWnd)) {
        return true;
    }
    // Per-monitor v2, like the Dark Overlay: Show() positions it in physical
    // pixels. Same band as the Dark Overlay, so it can stay above it.
    DPI_AWARENESS_CONTEXT prev = SetThreadDpiAwarenessContext(
        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    g_aboveSystemUi = Cfg()->overlayAboveSystemUi;
    g_hWnd = CreateModPopup(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT |
                                WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                            Controller::kOsdClassName, RECT{0, 0, 1, 1},
                            g_aboveSystemUi, nullptr);
    if (prev) {
        SetThreadDpiAwarenessContext(prev);
    }
    if (!g_hWnd) {
        Wh_Log(L"OSD: CreateWindowEx failed (%lu)", GetLastError());
        return false;
    }
    SetLayeredWindowAttributes(g_hWnd, 0, kWindowAlpha, LWA_ALPHA);
    g_size = {};
    return true;
}

// Where the OSD goes in the z-order: right below the Dark Overlay of its
// screen, so it's darkened like the rest of the screen, or (option off, or no
// overlay there) on top.
HWND InsertAfter() {
    if (Cfg()->darkenIndicator) {
        if (HWND hOverlay = Overlay::VisibleWindowOn(g_hMon)) {
            return hOverlay;
        }
    }
    return HWND_TOPMOST;
}

void Show(const Model& model, HMONITOR hMon) {
    if (!EnsureWindow()) {
        return;
    }
    g_model = model;
    g_hMon = hMon;
    g_dpi = GetDpiForMonitorWithFallback(hMon);

    // Physical pixels throughout (per-monitor v2).
    DPI_AWARENESS_CONTEXT prev = SetThreadDpiAwarenessContext(
        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    bool haveMonitor = GetMonitorInfo(hMon, &mi);
    int extraLines = (model.secondary.empty() ? 0 : 1) +
                     (model.debug.empty() ? 0 : 1);
    int w = S(320);
    int h = S(58 + extraLines * 18);
    if (haveMonitor) {
        int x = mi.rcWork.left + (mi.rcWork.right - mi.rcWork.left - w) / 2;
        int y = mi.rcWork.bottom - h - S(16);
        SetWindowPos(g_hWnd, InsertAfter(), x, y, w, h,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }
    if (prev) {
        SetThreadDpiAwarenessContext(prev);
    }
    if (!haveMonitor) {
        return;
    }
    if (g_size.cx != w || g_size.cy != h) {
        g_size = {w, h};
        HRGN rgn = CreateRoundRectRgn(0, 0, w + 1, h + 1, S(16), S(16));
        if (!SetWindowRgn(g_hWnd, rgn, TRUE)) {
            DeleteObject(rgn);
        }
    }
    InvalidateRect(g_hWnd, nullptr, FALSE);
    SetTimer(g_hWnd, kHideTimerId, kHideTimeoutMs, nullptr);
}

bool IsVisible() {
    return g_hWnd && IsWindowVisible(g_hWnd);
}

// After the overlays were raised: the OSD goes back right below its screen's
// overlay, or above it (option off).
void RaiseIfVisible() {
    if (IsVisible()) {
        SetWindowPos(g_hWnd, InsertAfter(), 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                         SWP_NOOWNERZORDER);
    }
}

void Destroy() {
    if (g_hWnd) {
        KillTimer(g_hWnd, kHideTimerId);
        DestroyWindow(g_hWnd);
        g_hWnd = nullptr;
    }
}

// After a settings change: the next Show() creates the window in the new band.
void ApplyBand() {
    if (g_hWnd && g_aboveSystemUi != Cfg()->overlayAboveSystemUi) {
        Destroy();
    }
}

std::wstring FormatPercent(int level) {
    WCHAR buf[32];
    if (level < 0) {
        swprintf_s(buf, L"−%d%%", -level);
    } else {
        swprintf_s(buf, L"%d%%", level);
    }
    return buf;
}

}  // namespace Osd

// -----------------------------------------------------------------------------
// Darkened mouse pointer
//
// The pointer isn't part of the image the Dark Overlay darkens: the graphics
// card lays it on top of the finished frame (hardware cursor plane), and even
// a software pointer is drawn above every window. So while the pointer is
// over a screen below 0%, the standard system pointers are replaced with
// copies darkened exactly like the overlay darkens that screen. Reloading the
// user's scheme (SPI_SETCURSORS) puts them back.
//
// HDR screens: the SDR brightness this mod sets (DwmpSDRToHDRBoost) only
// changes what the DWM composes. Windows shows the pointer at the SDR white
// level of the Settings app (the one DisplayConfigGetDeviceInfo reports), so
// at a lower mod brightness the pointer is brighter than the screen even at
// 0% (80 nits on screen, up to 480 nits for the pointer). The pointer is
// scaled by mod level / Settings level too, at any brightness where that
// makes it darker.
//
// It runs on a thread of its own: building a set takes a few to 20 ms
// (animated pointers are rebuilt frame by frame), which must not stall the
// taskbar. The taskbar thread only publishes which screens are dimmed; the
// thread follows the pointer and always applies the latest state.
// -----------------------------------------------------------------------------

namespace CursorDim {

struct Image {
    int w = 0;
    int h = 0;
    int hx = 0;
    int hy = 0;
    std::vector<uint32_t> px;  // top-down BGRA, straight alpha
};

bool ReadBits(HDC hdc, HBITMAP hbm, int w, int h, std::vector<uint32_t>& out) {
    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    out.assign((size_t)w * h, 0);
    return GetDIBits(hdc, hbm, 0, h, out.data(), &bi, DIB_RGB_COLORS) == h;
}

// Any cursor (alpha, color with an AND mask, or monochrome) as straight-alpha
// BGRA. Inverting pixels can't be expressed with alpha; they become opaque
// white, which the darkening turns into the same grey as the screen's whites.
bool FromCursor(HCURSOR hCursor, Image& out) {
    ICONINFO ii{};
    if (!hCursor || !GetIconInfo(hCursor, &ii)) {
        return false;
    }
    bool ok = false;
    HDC hdc = CreateCompatibleDC(nullptr);
    BITMAP bm{};
    if (hdc && ii.hbmColor && GetObject(ii.hbmColor, sizeof(bm), &bm) &&
        bm.bmWidth > 0 && bm.bmWidth <= 1024 && bm.bmHeight > 0 &&
        bm.bmHeight <= 1024) {
        int w = bm.bmWidth;
        int h = bm.bmHeight;
        std::vector<uint32_t> color, mask;
        if (ReadBits(hdc, ii.hbmColor, w, h, color)) {
            bool hasAlpha = false;
            for (uint32_t c : color) {
                if (c >> 24) {
                    hasAlpha = true;
                    break;
                }
            }
            if (!hasAlpha && ii.hbmMask &&
                !ReadBits(hdc, ii.hbmMask, w, h, mask)) {
                mask.clear();
            }
            out.w = w;
            out.h = h;
            out.px.resize(color.size());
            for (size_t i = 0; i < color.size(); i++) {
                if (hasAlpha) {
                    out.px[i] = color[i];
                    continue;
                }
                uint32_t rgb = color[i] & 0xFFFFFF;
                bool transparent = !mask.empty() && (mask[i] & 0xFFFFFF);
                out.px[i] = transparent && !rgb ? 0 : 0xFF000000 | rgb;
            }
            ok = true;
        }
    } else if (hdc && !ii.hbmColor && ii.hbmMask &&
               GetObject(ii.hbmMask, sizeof(bm), &bm) && bm.bmWidth > 0 &&
               bm.bmWidth <= 1024 && bm.bmHeight >= 2 &&
               bm.bmHeight <= 2048) {
        // Monochrome: AND mask on top, XOR mask below.
        int w = bm.bmWidth;
        int h = bm.bmHeight / 2;
        std::vector<uint32_t> m;
        if (ReadBits(hdc, ii.hbmMask, w, h * 2, m)) {
            out.w = w;
            out.h = h;
            out.px.resize((size_t)w * h);
            for (size_t i = 0; i < out.px.size(); i++) {
                bool andBit = m[i] & 0xFFFFFF;
                bool xorBit = m[i + out.px.size()] & 0xFFFFFF;
                out.px[i] = xorBit ? 0xFFFFFFFF : andBit ? 0 : 0xFF000000;
            }
            ok = true;
        }
    }
    out.hx = (int)ii.xHotspot;
    out.hy = (int)ii.yHotspot;
    if (hdc) {
        DeleteDC(hdc);
    }
    if (ii.hbmColor) {
        DeleteObject(ii.hbmColor);
    }
    if (ii.hbmMask) {
        DeleteObject(ii.hbmMask);
    }
    return ok;
}

void Darken(const Image& src, const uint8_t lut[256], Image& out) {
    out.w = src.w;
    out.h = src.h;
    out.hx = src.hx;
    out.hy = src.hy;
    out.px.resize(src.px.size());
    for (size_t i = 0; i < src.px.size(); i++) {
        uint32_t p = src.px[i];
        out.px[i] = (p & 0xFF000000) |
                    ((uint32_t)lut[(p >> 16) & 0xFF] << 16) |
                    ((uint32_t)lut[(p >> 8) & 0xFF] << 8) | lut[p & 0xFF];
    }
}

// m: how bright the pointer must be. On SDR screens it scales the sRGB values
// (the DWM blends the Dark Overlay in gamma space there); on HDR screens it
// scales linear light (the DWM blends in linear light, and the pointer is
// shown as SDR content, linearized with the sRGB curve).
void BuildLut(double m, bool hdr, uint8_t lut[256]) {
    for (int v = 0; v < 256; v++) {
        double x = v / 255.0;
        if (hdr) {
            double lin =
                x <= 0.04045 ? x / 12.92 : std::pow((x + 0.055) / 1.055, 2.4);
            lin *= m;
            x = lin <= 0.0031308 ? lin * 12.92
                                 : 1.055 * std::pow(lin, 1.0 / 2.4) - 0.055;
        } else {
            x *= m;
        }
        lut[v] = (uint8_t)std::clamp((int)std::lround(x * 255.0), 0, 255);
    }
}

bool ReadFileBytes(PCWSTR path, std::vector<uint8_t>& out) {
    HANDLE h = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        return false;
    }
    LARGE_INTEGER size{};
    bool ok = false;
    if (GetFileSizeEx(h, &size) && size.QuadPart > 0 &&
        size.QuadPart <= 16 * 1024 * 1024) {
        out.resize((size_t)size.QuadPart);
        DWORD read = 0;
        ok = ReadFile(h, out.data(), (DWORD)out.size(), &read, nullptr) &&
             read == out.size();
    }
    CloseHandle(h);
    return ok;
}

uint32_t Rd32(const uint8_t* p) {
    return p[0] | (p[1] << 8) | (p[2] << 16) | ((uint32_t)p[3] << 24);
}

uint16_t Rd16(const uint8_t* p) {
    return (uint16_t)(p[0] | (p[1] << 8));
}

// Decodes the image of an .ico/.cur file (an .ani frame) closest to `size`.
bool DecodeIconFile(const uint8_t* data, size_t len, int size, Image& out) {
    if (len < 6 || Rd16(data) != 0) {
        return false;
    }
    uint16_t type = Rd16(data + 2);
    uint16_t count = Rd16(data + 4);
    if ((type != 1 && type != 2) || !count || len < 6 + (size_t)count * 16) {
        return false;
    }
    int best = -1;
    int bestW = 0;
    uint32_t bestBytes = 0;
    for (int i = 0; i < count; i++) {
        const uint8_t* e = data + 6 + i * 16;
        int w = e[0] ? e[0] : 256;
        uint32_t bytes = Rd32(e + 8);
        uint32_t offset = Rd32(e + 12);
        if (!bytes || offset > len || bytes > len - offset) {
            continue;
        }
        // The smallest image that is at least `size`, else the largest one;
        // among equal sizes, the biggest (deepest) one.
        bool better;
        if (best < 0) {
            better = true;
        } else if (w == bestW) {
            better = bytes > bestBytes;
        } else if (bestW >= size) {
            better = w >= size && w < bestW;
        } else {
            better = w > bestW;
        }
        if (better) {
            best = i;
            bestW = w;
            bestBytes = bytes;
        }
    }
    if (best < 0) {
        return false;
    }
    const uint8_t* e = data + 6 + best * 16;
    uint32_t bytes = Rd32(e + 8);
    uint32_t offset = Rd32(e + 12);
    // CreateIconFromResourceEx wants a cursor resource: the hotspot, then the
    // DIB or PNG data. It scales the image and the hotspot to `size`.
    std::vector<uint8_t> res(4 + (size_t)bytes);
    uint16_t hx = type == 2 ? Rd16(e + 4) : 0;
    uint16_t hy = type == 2 ? Rd16(e + 6) : 0;
    memcpy(res.data(), &hx, 2);
    memcpy(res.data() + 2, &hy, 2);
    memcpy(res.data() + 4, data + offset, bytes);
    HICON h = CreateIconFromResourceEx(res.data(), (DWORD)res.size(), FALSE,
                                       0x00030000, size, size, LR_DEFAULTCOLOR);
    if (!h) {
        return false;
    }
    bool ok = FromCursor(h, out);
    DestroyIcon(h);
    return ok;
}

struct Ani {
    std::vector<uint8_t> anih, rate, seq;
    std::vector<std::pair<size_t, size_t>> icons;  // offset, length in file
};

constexpr uint32_t kAfIcon = 1;

bool ParseAni(const std::vector<uint8_t>& f, Ani& out) {
    if (f.size() < 12 || memcmp(f.data(), "RIFF", 4) != 0 ||
        memcmp(f.data() + 8, "ACON", 4) != 0) {
        return false;
    }
    size_t end = std::min(f.size(), (size_t)Rd32(f.data() + 4) + 8);
    size_t pos = 12;
    while (pos + 8 <= end) {
        const uint8_t* c = f.data() + pos;
        size_t len = Rd32(c + 4);
        size_t body = pos + 8;
        if (len > end - body) {
            break;
        }
        if (memcmp(c, "anih", 4) == 0) {
            out.anih.assign(f.begin() + body, f.begin() + body + len);
        } else if (memcmp(c, "rate", 4) == 0) {
            out.rate.assign(f.begin() + body, f.begin() + body + len);
        } else if (memcmp(c, "seq ", 4) == 0) {
            out.seq.assign(f.begin() + body, f.begin() + body + len);
        } else if (memcmp(c, "LIST", 4) == 0 && len >= 4 &&
                   memcmp(f.data() + body, "fram", 4) == 0) {
            size_t p = body + 4;
            size_t listEnd = body + len;
            while (p + 8 <= listEnd) {
                size_t l = Rd32(f.data() + p + 4);
                if (l > listEnd - (p + 8)) {
                    break;
                }
                if (memcmp(f.data() + p, "icon", 4) == 0) {
                    out.icons.push_back({p + 8, l});
                }
                p += 8 + l + (l & 1);
            }
        }
        pos = body + len + (len & 1);
    }
    return out.anih.size() >= 36 && (Rd32(out.anih.data() + 32) & kAfIcon) &&
           !out.icons.empty();
}

void Put32(std::vector<uint8_t>& v, uint32_t x) {
    for (int i = 0; i < 4; i++) {
        v.push_back((uint8_t)(x >> (i * 8)));
    }
}

void Put16(std::vector<uint8_t>& v, uint16_t x) {
    v.push_back((uint8_t)x);
    v.push_back((uint8_t)(x >> 8));
}

void PutTag(std::vector<uint8_t>& v, const char* tag) {
    v.insert(v.end(), tag, tag + 4);
}

void PutChunk(std::vector<uint8_t>& v,
              const char* tag,
              const uint8_t* data,
              size_t len) {
    PutTag(v, tag);
    Put32(v, (uint32_t)len);
    v.insert(v.end(), data, data + len);
    if (len & 1) {
        v.push_back(0);
    }
}

// A one-image, 32-bit .cur file.
void PutCurFile(std::vector<uint8_t>& v, const Image& img) {
    int maskStride = ((img.w + 31) / 32) * 4;
    uint32_t dib = 40 + (uint32_t)img.w * img.h * 4 +
                   (uint32_t)maskStride * img.h;
    Put16(v, 0);
    Put16(v, 2);
    Put16(v, 1);
    v.push_back((uint8_t)(img.w < 256 ? img.w : 0));
    v.push_back((uint8_t)(img.h < 256 ? img.h : 0));
    v.push_back(0);
    v.push_back(0);
    Put16(v, (uint16_t)img.hx);
    Put16(v, (uint16_t)img.hy);
    Put32(v, dib);
    Put32(v, 22);
    Put32(v, 40);
    Put32(v, (uint32_t)img.w);
    Put32(v, (uint32_t)img.h * 2);
    Put16(v, 1);
    Put16(v, 32);
    Put32(v, BI_RGB);
    Put32(v, (uint32_t)img.w * img.h * 4);
    Put32(v, 0);
    Put32(v, 0);
    Put32(v, 0);
    Put32(v, 0);
    for (int y = img.h - 1; y >= 0; y--) {
        const uint32_t* row = img.px.data() + (size_t)y * img.w;
        for (int x = 0; x < img.w; x++) {
            Put32(v, row[x]);
        }
    }
    std::vector<uint8_t> row(maskStride);
    for (int y = img.h - 1; y >= 0; y--) {
        std::fill(row.begin(), row.end(), 0);
        for (int x = 0; x < img.w; x++) {
            if (!(img.px[(size_t)y * img.w + x] >> 24)) {
                row[x / 8] |= 0x80 >> (x & 7);
            }
        }
        v.insert(v.end(), row.begin(), row.end());
    }
}

// An .ani file made of (darkened) frames, with the original header, frame
// rates and sequence.
std::vector<uint8_t> BuildAni(const Ani& ani, const std::vector<Image>& frames) {
    std::vector<uint8_t> v;
    PutTag(v, "RIFF");
    Put32(v, 0);
    PutTag(v, "ACON");
    std::vector<uint8_t> h = ani.anih;
    h.resize(36, 0);
    uint32_t n = (uint32_t)frames.size();
    memcpy(h.data() + 4, &n, 4);  // nFrames
    uint32_t flags = Rd32(h.data() + 32) | kAfIcon;
    memcpy(h.data() + 32, &flags, 4);
    PutChunk(v, "anih", h.data(), h.size());
    if (!ani.rate.empty()) {
        PutChunk(v, "rate", ani.rate.data(), ani.rate.size());
    }
    if (!ani.seq.empty()) {
        PutChunk(v, "seq ", ani.seq.data(), ani.seq.size());
    }
    size_t listPos = v.size();
    PutTag(v, "LIST");
    Put32(v, 0);
    PutTag(v, "fram");
    std::vector<uint8_t> cur;
    for (const Image& f : frames) {
        cur.clear();
        PutCurFile(cur, f);
        PutChunk(v, "icon", cur.data(), cur.size());
    }
    uint32_t listLen = (uint32_t)(v.size() - listPos - 8);
    memcpy(v.data() + listPos + 4, &listLen, 4);
    uint32_t riffLen = (uint32_t)(v.size() - 8);
    memcpy(v.data() + 4, &riffLen, 4);
    return v;
}

// The system pointers, by their name under HKCU\Control Panel\Cursors.
struct Slot {
    PCWSTR regName;
    DWORD id;  // OCR_*
};

constexpr Slot kSlots[] = {
    {L"Arrow", 32512},    {L"IBeam", 32513},       {L"Wait", 32514},
    {L"Crosshair", 32515}, {L"UpArrow", 32516},    {L"SizeNWSE", 32642},
    {L"SizeNESW", 32643}, {L"SizeWE", 32644},      {L"SizeNS", 32645},
    {L"SizeAll", 32646},  {L"No", 32648},          {L"Hand", 32649},
    {L"AppStarting", 32650}, {L"Help", 32651},     {L"NWPen", 32631},
    {L"Pin", 32671},      {L"Person", 32672},
};

struct Source {
    DWORD id = 0;
    Image still;
    bool animated = false;
    Ani ani;  // header, rates and sequence of an animated pointer
    std::vector<Image> frames;
};

// A screen that is below 0%, as published by the taskbar thread.
// How bright the pointer must be: kLevelOne = unchanged.
constexpr int kLevelOne = 100000;

// A screen where the pointer must be darker, as published by the taskbar
// thread.
struct DimScreen {
    RECT rc;    // physical pixels
    int level;  // of kLevelOne: in linear light on HDR screens, else gamma
    bool hdr;
};

struct Want {
    bool on = false;
    int level = kLevelOne;
    bool hdr = false;
    int size = 0;
    bool operator==(const Want&) const = default;
};

constexpr PCWSTR kStorageDarkened = L"cursorsDarkened";

// While scrolling only the darkness changes; rebuild at most this often.
constexpr ULONGLONG kMinRebuildMs = 100;

SRWLOCK g_lock = SRWLOCK_INIT;
std::vector<DimScreen> g_screens;  // g_lock
bool g_schemeChanged;              // g_lock
bool g_reapply;                    // g_lock
HANDLE g_wakeEvent;
HANDLE g_thread;
bool g_startFailed;
std::atomic<bool> g_stop;

// Pointer thread only.
std::vector<Source> t_sources;
bool t_sourcesLoaded;  // t_sources is for t_sourceSize and the current scheme
int t_sourceSize;
int t_baseSize = 32;
bool t_applied;  // system pointers are replaced
Want t_current;  // what was last applied (on = false: nothing)
ULONGLONG t_lastApplyTick;
std::wstring t_dir;  // folder of the darkened pointer files
int t_set;           // its subfolder (0/1) the pointers in use come from

std::wstring SchemePath(PCWSTR name) {
    WCHAR buf[MAX_PATH * 2];
    DWORD cb = sizeof(buf);
    // REG_EXPAND_SZ values are expanded by RegGetValue.
    if (RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Cursors", name,
                     RRF_RT_REG_SZ, nullptr, buf, &cb) != ERROR_SUCCESS ||
        !buf[0]) {
        return {};
    }
    if (wcschr(buf, L'%')) {
        WCHAR expanded[MAX_PATH * 2];
        DWORD n = ExpandEnvironmentStringsW(buf, expanded, ARRAYSIZE(expanded));
        if (n && n <= ARRAYSIZE(expanded)) {
            return expanded;
        }
    }
    return buf;
}

// The pointer size at 96 DPI ("Pointer size" in Settings).
int BaseSize() {
    DWORD v = 32;
    DWORD cb = sizeof(v);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Cursors",
                     L"CursorBaseSize", RRF_RT_REG_DWORD, nullptr, &v,
                     &cb) != ERROR_SUCCESS ||
        v < 32 || v > 256) {
        v = 32;
    }
    return (int)v;
}

// The darkened pointer files live in the mod's storage directory, which
// Windhawk removes with the mod.
std::wstring PointerDir() {
    WCHAR path[MAX_PATH];
    if (!Wh_GetModStoragePath(path, ARRAYSIZE(path))) {
        return {};
    }
    CreateDirectoryW(path, nullptr);  // may not exist yet
    return std::wstring(path) + L"\\Pointers";
}

void DeletePointerFiles() {
    if (t_dir.empty()) {
        return;
    }
    for (PCWSTR sub : {L"\\0", L"\\1"}) {
        std::wstring dir = t_dir + sub;
        WIN32_FIND_DATAW fd;
        HANDLE h = FindFirstFileW((dir + L"\\*").c_str(), &fd);
        if (h != INVALID_HANDLE_VALUE) {
            do {
                if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    DeleteFileW((dir + L"\\" + fd.cFileName).c_str());
                }
            } while (FindNextFileW(h, &fd));
            FindClose(h);
        }
        RemoveDirectoryW(dir.c_str());
    }
    RemoveDirectoryW(t_dir.c_str());
}

bool WriteBytes(const std::wstring& path, const std::vector<uint8_t>& data) {
    HANDLE h = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr,
                           CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        return false;
    }
    DWORD written = 0;
    bool ok = WriteFile(h, data.data(), (DWORD)data.size(), &written,
                        nullptr) &&
              written == data.size();
    CloseHandle(h);
    return ok;
}

void Restore() {
    SystemParametersInfoW(SPI_SETCURSORS, 0, nullptr, 0);
    t_applied = false;
    t_current = {};
    Wh_SetIntValue(kStorageDarkened, 0);
    Wh_Log(L"Pointer: scheme restored");
}

// Only called while none of the pointers is replaced: the fallback for
// pointers without a file reads the system's current pointer.
void LoadSources(int size) {
    t_sources.clear();
    t_sourcesLoaded = true;
    t_sourceSize = size;
    for (const Slot& slot : kSlots) {
        Source s;
        s.id = slot.id;
        bool ok = false;
        std::wstring path = SchemePath(slot.regName);
        std::vector<uint8_t> bytes;
        if (!path.empty() && ReadFileBytes(path.c_str(), bytes)) {
            if (bytes.size() >= 4 && memcmp(bytes.data(), "RIFF", 4) == 0 &&
                ParseAni(bytes, s.ani)) {
                for (auto [offset, len] : s.ani.icons) {
                    Image f;
                    if (!DecodeIconFile(bytes.data() + offset, len, size, f)) {
                        s.frames.clear();
                        break;
                    }
                    s.frames.push_back(std::move(f));
                }
                s.ani.icons.clear();
                s.animated = !s.frames.empty();
                ok = s.animated;
            }
            if (!ok) {
                HCURSOR h = (HCURSOR)LoadImageW(nullptr, path.c_str(),
                                                IMAGE_CURSOR, size, size,
                                                LR_LOADFROMFILE);
                ok = h && FromCursor(h, s.still);
                if (h) {
                    DestroyCursor(h);
                }
            }
        }
        if (!ok) {
            // No file (e.g. the "None" scheme): the system's own pointer.
            HCURSOR sys =
                (HCURSOR)LoadImageW(nullptr, MAKEINTRESOURCEW(slot.id),
                                    IMAGE_CURSOR, 0, 0, LR_SHARED);
            if (sys) {
                HCURSOR copy =
                    (HCURSOR)CopyImage(sys, IMAGE_CURSOR, size, size, 0);
                ok = FromCursor(copy ? copy : sys, s.still);
                if (copy) {
                    DestroyCursor(copy);
                }
            }
        }
        if (ok) {
            t_sources.push_back(std::move(s));
        } else {
            Wh_Log(L"Pointer %s: no image, left as is", slot.regName);
        }
    }
}

// Windows sizes a pointer loaded from a file like the scheme's own pointers
// (pointer size and DPI), but takes a cursor made in memory for a 32 px one
// and scales it up. So each darkened pointer is written to a file, at the
// exact size, and loaded from there. The sets alternate between two folders,
// so the files of the pointers in use are never overwritten.
void Apply(const Want& want) {
    if (!t_sourcesLoaded || t_sourceSize != want.size) {
        if (t_applied) {
            Restore();
        }
        LoadSources(want.size);
    }
    t_current = want;
    t_lastApplyTick = GetTickCount64();
    if (t_sources.empty() || t_dir.empty()) {
        return;
    }
    int set = t_set ^ 1;
    std::wstring dir = t_dir + (set ? L"\\1" : L"\\0");
    CreateDirectoryW(t_dir.c_str(), nullptr);
    CreateDirectoryW(dir.c_str(), nullptr);
    // Before replacing anything, so a crash still gets the scheme restored at
    // the next start.
    if (!t_applied) {
        Wh_SetIntValue(kStorageDarkened, 1);
    }
    uint8_t lut[256];
    BuildLut((double)want.level / kLevelOne, want.hdr, lut);
    int replaced = 0;
    std::vector<Image> frames;
    Image still;
    std::vector<uint8_t> file;
    for (const Source& s : t_sources) {
        file.clear();
        if (s.animated) {
            frames.resize(s.frames.size());
            for (size_t i = 0; i < frames.size(); i++) {
                Darken(s.frames[i], lut, frames[i]);
            }
            file = BuildAni(s.ani, frames);
        } else {
            Darken(s.still, lut, still);
            PutCurFile(file, still);
        }
        WCHAR name[32];
        swprintf_s(name, L"\\%lu.%s", s.id, s.animated ? L"ani" : L"cur");
        std::wstring path = dir + name;
        if (!WriteBytes(path, file)) {
            continue;
        }
        HCURSOR h = LoadCursorFromFileW(path.c_str());
        if (!h) {
            continue;
        }
        // SetSystemCursor takes the cursor over (and destroys it).
        if (SetSystemCursor(h, s.id)) {
            replaced++;
        } else {
            DestroyCursor(h);
        }
    }
    t_set = set;
    t_applied = true;
    if (!replaced) {
        Wh_Log(L"Pointer: couldn't replace the pointers (folder %s)",
               dir.c_str());
    }
    Wh_Log(L"Pointer: darkened to %.2f%% (%s, %d px, %d/%zu pointers)",
           want.level * 100.0 / kLevelOne, want.hdr ? L"linear, HDR" : L"SDR",
           want.size, replaced, t_sources.size());
}

Want Evaluate(const std::vector<DimScreen>& screens) {
    Want want;
    POINT pt;
    if (screens.empty() || !GetCursorPos(&pt)) {
        return want;
    }
    for (const DimScreen& s : screens) {
        if (PtInRect(&s.rc, pt)) {
            want.on = true;
            want.level = s.level;
            want.hdr = s.hdr;
            UINT dpi = GetDpiForMonitorWithFallback(
                MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST));
            want.size = std::clamp(MulDiv(t_baseSize, dpi, 96), 16, 256);
            break;
        }
    }
    return want;
}

DWORD WINAPI ThreadProc(void*) {
    // Physical pixels, like the published screen rectangles.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    t_baseSize = BaseSize();
    t_dir = PointerDir();
    DeletePointerFiles();  // left over by a previous session
    std::vector<DimScreen> screens;
    // While a screen is darkened the pointer is polled, every 40 ms while it
    // moves, backing off to 200 ms while it stays still. A change of the
    // screens or of their darkness wakes the thread right away.
    constexpr DWORD kPollMinMs = 40;
    constexpr DWORD kPollMaxMs = 200;
    DWORD pollMs = kPollMinMs;
    POINT lastPt = {LONG_MIN, LONG_MIN};
    bool woken = true;
    while (!g_stop) {
        AcquireSRWLockExclusive(&g_lock);
        screens = g_screens;
        bool schemeChanged = std::exchange(g_schemeChanged, false);
        bool reapply = std::exchange(g_reapply, false);
        ReleaseSRWLockExclusive(&g_lock);

        if (schemeChanged) {
            // The scheme was (re)loaded: none of ours is in place any more,
            // and the files or the pointer size may have changed.
            if (t_applied) {
                Wh_SetIntValue(kStorageDarkened, 0);
            }
            t_applied = false;
            t_current = {};
            t_sources.clear();
            t_sourcesLoaded = false;
            t_baseSize = BaseSize();
        }
        if (reapply) {
            // After a display change Windows may have reloaded the pointers,
            // and the pointer size may have changed.
            t_current = {};
            t_baseSize = BaseSize();
        }

        // Follow the pointer while a screen is dimmed; sleep otherwise.
        DWORD wait = INFINITE;
        if (!screens.empty()) {
            // GetCursorPos fails while the input desktop isn't the user's
            // (the PC is locked, a UAC prompt is up): that counts as still.
            POINT pt = lastPt;
            bool moved = GetCursorPos(&pt) &&
                         (pt.x != lastPt.x || pt.y != lastPt.y);
            lastPt = pt;
            pollMs = (moved || woken) ? kPollMinMs
                                      : std::min(pollMs * 2, kPollMaxMs);
            wait = pollMs;
        }
        Want want = Evaluate(screens);
        if (!want.on) {
            if (t_applied) {
                Restore();
            }
            t_current = {};
        } else if (!(want == t_current)) {
            ULONGLONG since = GetTickCount64() - t_lastApplyTick;
            bool darknessOnly = t_applied && t_current.on &&
                                want.hdr == t_current.hdr &&
                                want.size == t_current.size;
            if (darknessOnly && since < kMinRebuildMs) {
                wait = (DWORD)(kMinRebuildMs - since);
            } else {
                Apply(want);
            }
        }
        woken = WaitForSingleObject(g_wakeEvent, wait) == WAIT_OBJECT_0;
    }
    if (t_applied) {
        Restore();
    }
    t_sources.clear();
    DeletePointerFiles();
    return 0;
}

// Taskbar thread. Starts the pointer thread the first time (at the first
// screen list, when the taskbar is set up), so it only ever runs in the
// Explorer that has the taskbar.
void Start() {
    if (Wh_GetIntValue(kStorageDarkened, 0)) {
        // Explorer stopped (or crashed) while the pointers were darkened.
        SystemParametersInfoW(SPI_SETCURSORS, 0, nullptr, 0);
        Wh_SetIntValue(kStorageDarkened, 0);
        Wh_Log(L"Pointer: restored the scheme left darkened by a previous "
               L"session");
    }
    g_stop = false;
    g_wakeEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (g_wakeEvent) {
        g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
    }
    if (!g_thread) {
        Wh_Log(L"Pointer: couldn't start the thread (%lu)", GetLastError());
        if (g_wakeEvent) {
            CloseHandle(g_wakeEvent);
            g_wakeEvent = nullptr;
        }
        g_startFailed = true;
    }
}

// Taskbar thread: the screens below 0% (empty: none, or the option is off).
void Publish(std::vector<DimScreen> screens, bool reapply) {
    if (!g_thread && !g_startFailed && !g_unloading) {
        Start();
    }
    if (!g_thread) {
        return;
    }
    AcquireSRWLockExclusive(&g_lock);
    g_screens = std::move(screens);
    if (reapply) {
        g_reapply = true;
    }
    ReleaseSRWLockExclusive(&g_lock);
    SetEvent(g_wakeEvent);
}

// WM_SETTINGCHANGE / SPI_SETCURSORS: the user changed the pointer scheme or
// size.
void OnSchemeChanged() {
    if (!g_thread) {
        return;
    }
    AcquireSRWLockExclusive(&g_lock);
    g_schemeChanged = true;
    ReleaseSRWLockExclusive(&g_lock);
    SetEvent(g_wakeEvent);
}

// Mod unload, after the taskbar thread's cleanup. Puts the user's pointers
// back if they are darkened.
void Stop() {
    if (!g_thread) {
        return;
    }
    g_stop = true;
    SetEvent(g_wakeEvent);
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    g_thread = nullptr;
    CloseHandle(g_wakeEvent);
    g_wakeEvent = nullptr;
}

}  // namespace CursorDim

// -----------------------------------------------------------------------------
// Dark Overlay
//
// One black, click-through, topmost layered window per screen, shown only
// while that screen is below 0%. With "covers thumbnail previews, Start and
// shell popups", the windows are in the ZBID_SYSTEM_TOOLS band, above those.
// The fade animation timer only runs while an alpha is changing.
// -----------------------------------------------------------------------------

namespace Overlay {

struct Win {
    HWND hWnd = nullptr;
    float cur = 0.0f;
    int target = 0;
    int shownAlpha = -1;
    int depth = 0;
    bool hdr = false;
    bool aboveSystemUi = false;  // the band hWnd was created for
    RECT rc{};
};

std::unordered_map<std::wstring, Win> g_wins;
bool g_animating;
bool g_raising;
HWINEVENTHOOK g_eventHooks[2];

// depth: 0..100 (0 = off). On HDR screens the DWM composes in linear light,
// so the same alpha looks weaker than on SDR screens; the perceptual curve
// compensates so each step looks about the same on both.
int AlphaForDepth(int depth, bool hdr, const Settings& cfg) {
    if (depth <= 0 || !cfg.darkOverlayEnabled) {
        return 0;
    }
    double x = std::clamp(depth / 100.0, 0.0, 1.0);
    double g = x;
    if (cfg.darkCurve == DarkCurve::Perceptual && hdr) {
        g = 1.0 - std::pow(1.0 - x, 2.2);
    }
    int a = (int)std::lround(cfg.maxOverlayAlpha * g);
    return std::clamp(a, 1, 255);
}

int TargetAlpha(const std::wstring& id) {
    auto it = g_wins.find(id);
    return it != g_wins.end() ? it->second.target : 0;
}

void ApplyCaptureAffinity(HWND hWnd, const Settings& cfg) {
    SetWindowDisplayAffinity(
        hWnd, cfg.hideOverlayFromCapture ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
}

// Creates w's window (fully transparent until the next Tick).
void Create(Win& w, const Settings& cfg) {
    DPI_AWARENESS_CONTEXT prev = SetThreadDpiAwarenessContext(
        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    bool inBand = false;
    HWND hWnd = CreateModPopup(WS_EX_TOPMOST | WS_EX_TRANSPARENT |
                                   WS_EX_LAYERED | WS_EX_TOOLWINDOW |
                                   WS_EX_NOACTIVATE,
                               Controller::kDarkClassName, w.rc,
                               cfg.overlayAboveSystemUi, &inBand);
    if (prev) {
        SetThreadDpiAwarenessContext(prev);
    }
    w.hWnd = hWnd;
    w.aboveSystemUi = cfg.overlayAboveSystemUi;
    w.shownAlpha = -1;
    if (!hWnd) {
        Wh_Log(L"Dark Overlay: CreateWindowEx failed (%lu)", GetLastError());
        return;
    }
    Wh_Log(L"Dark Overlay window created (%s)",
           inBand ? L"system tools band, above shell popups"
                : L"normal topmost window");
    SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
    ApplyCaptureAffinity(hWnd, cfg);
}

// Tells the pointer thread how bright the pointer must be on each screen:
// what the Dark Overlay leaves of it and, on HDR screens, the SDR brightness
// of the DWM (set by this mod) relative to the one Windows shows the pointer
// at (the Settings app's).
void PublishToCursor(bool reapply = false) {
    auto cfg = Cfg();
    std::vector<CursorDim::DimScreen> screens;
    if (cfg->darkenCursor) {
        for (const Topo::Screen& s : Topo::g_screens) {
            double factor = 1.0;
            auto it = g_wins.find(s.id);
            if (it != g_wins.end() && cfg->darkOverlayEnabled) {
                factor = 1.0 - it->second.target / 255.0;
            }
            double sdrRatio = 1.0;
            if (s.hdr) {
                int content = Bright::CurrentSdrRaw(s.id);
                ULONG pointer = Topo::ReadSdrWhite(s.adapterId, s.targetId);
                if (content >= 1000 && pointer >= 1000) {
                    sdrRatio = (double)content / pointer;
                    factor *= sdrRatio;
                }
            }
            int level =
                (int)std::lround(factor * CursorDim::kLevelOne);
            if (level < CursorDim::kLevelOne * 995 / 1000) {
                screens.push_back({s.rc, std::max(level, 1), s.hdr});
                Wh_Log(L"Pointer on %s: %.2f%%%s", s.gdiName.c_str(),
                       factor * 100.0,
                       s.hdr ? (sdrRatio < 1.0 ? L" (HDR: SDR level of the mod "
                                               L"below the Settings one)"
                                             : L" (HDR)")
                           : L"");
            }
        }
    }
    CursorDim::Publish(std::move(screens), reapply);
}

void StartAnimation() {
    HWND hCtrl = g_hController;
    if (!g_animating && hCtrl) {
        SetTimer(hCtrl, Controller::kTimerAnim, 16, nullptr);
        g_animating = true;
    }
}

void SetDepth(const Topo::Screen& screen, int depth) {
    auto cfg = Cfg();
    Win& w = g_wins[screen.id];
    w.depth = depth;
    w.hdr = screen.hdr;
    w.rc = screen.rc;
    w.target = AlphaForDepth(depth, screen.hdr, *cfg);
    if (w.target > 0 && (!w.hWnd || !IsWindow(w.hWnd))) {
        Create(w, *cfg);
        w.cur = 0.0f;
    }
    Wh_Log(L"Dark Overlay %s: depth %d -> alpha %d", screen.gdiName.c_str(),
           depth, w.target);
    StartAnimation();
    PublishToCursor();
}

HWND VisibleWindowOn(HMONITOR hMon) {
    if (!hMon) {
        return nullptr;
    }
    for (const auto& [id, w] : g_wins) {
        const Topo::Screen* s = Topo::FindById(id);
        if (s && s->hMon == hMon && w.hWnd && IsWindowVisible(w.hWnd)) {
            return w.hWnd;
        }
    }
    return nullptr;
}

// Pushes the visible overlays back on top (and the OSD right below or above
// them). Returns whether any overlay is visible.
bool RaiseVisible() {
    bool anyVisible = false;
    for (auto& [id, w] : g_wins) {
        if (w.hWnd && IsWindowVisible(w.hWnd)) {
            SetWindowPos(w.hWnd, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                             SWP_NOOWNERZORDER);
            anyVisible = true;
        }
    }
    Osd::RaiseIfVisible();
    return anyVisible;
}

// A top-level window was shown or activated: it may be above the overlays now
// (a window of the same band, e.g. a topmost window in the normal band when
// the overlay is there too). Cover it right away.
void CALLBACK OnWinEvent(HWINEVENTHOOK,
                         DWORD,
                         HWND hWnd,
                         LONG idObject,
                         LONG idChild,
                         DWORD,
                         DWORD) {
    if (!hWnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF ||
        g_unloading || GetAncestor(hWnd, GA_ROOT) != hWnd) {
        return;
    }
    WCHAR cls[64];
    if (GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
        (wcscmp(cls, Controller::kDarkClassName) == 0 ||
         wcscmp(cls, Controller::kOsdClassName) == 0)) {
        return;
    }
    RaiseVisible();
}

// While an overlay is visible: raise the overlays when another window shows
// up or is activated, and a few times per second in case something else
// moved above them.
void SetRaiseTimer(bool on) {
    HWND hCtrl = g_hController;
    if (on == g_raising || (on && !hCtrl)) {
        return;
    }
    g_raising = on;
    if (on) {
        SetTimer(hCtrl, Controller::kTimerRaise, 250, nullptr);
        g_eventHooks[0] = SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
            OnWinEvent, 0, 0, WINEVENT_OUTOFCONTEXT);
        g_eventHooks[1] =
            SetWinEventHook(EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW, nullptr,
                            OnWinEvent, 0, 0, WINEVENT_OUTOFCONTEXT);
    } else {
        if (hCtrl) {
            KillTimer(hCtrl, Controller::kTimerRaise);
        }
        for (HWINEVENTHOOK& h : g_eventHooks) {
            if (h) {
                UnhookWinEvent(h);
                h = nullptr;
            }
        }
    }
}

void Tick() {
    bool moving = false;
    bool anyVisible = false;
    for (auto& [id, w] : g_wins) {
        if (!w.hWnd || !IsWindow(w.hWnd)) {
            w.hWnd = nullptr;
            continue;
        }
        float t = (float)w.target;
        float d = t - w.cur;
        if (std::fabs(d) > 0.75f) {
            w.cur += d * 0.3f;
            moving = true;
        } else {
            w.cur = t;
        }
        int a = std::clamp((int)std::lround(w.cur), 0, 255);
        if (a != w.shownAlpha) {
            SetLayeredWindowAttributes(w.hWnd, 0, (BYTE)a, LWA_ALPHA);
            w.shownAlpha = a;
        }
        bool visible = IsWindowVisible(w.hWnd);
        if (a > 0 && !visible) {
            ShowWindow(w.hWnd, SW_SHOWNOACTIVATE);
            SetWindowPos(w.hWnd, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            Osd::RaiseIfVisible();
        } else if (a == 0 && w.target == 0 && visible) {
            ShowWindow(w.hWnd, SW_HIDE);
        }
        if (a > 0) {
            anyVisible = true;
        }
    }
    if (!moving) {
        HWND hCtrl = g_hController;
        if (hCtrl) {
            KillTimer(hCtrl, Controller::kTimerAnim);
        }
        g_animating = false;
    }
    SetRaiseTimer(anyVisible);
}

// The raise timer.
void RaiseAll() {
    if (!RaiseVisible()) {
        SetRaiseTimer(false);
    }
}

// Reposition/destroy after a display change.
void OnTopology() {
    auto cfg = Cfg();
    for (auto it = g_wins.begin(); it != g_wins.end();) {
        Win& w = it->second;
        const Topo::Screen* s = Topo::FindById(it->first);
        if (!s) {
            if (w.hWnd) {
                DestroyWindow(w.hWnd);
            }
            it = g_wins.erase(it);
            continue;
        }
        if (w.hWnd && !EqualRect(&w.rc, &s->rc)) {
            DPI_AWARENESS_CONTEXT prev = SetThreadDpiAwarenessContext(
                DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
            SetWindowPos(w.hWnd, nullptr, s->rc.left, s->rc.top,
                         s->rc.right - s->rc.left, s->rc.bottom - s->rc.top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
            if (prev) {
                SetThreadDpiAwarenessContext(prev);
            }
        }
        w.rc = s->rc;
        if (w.hdr != s->hdr) {
            w.hdr = s->hdr;
            w.target = AlphaForDepth(w.depth, w.hdr, *cfg);
            StartAnimation();
        }
        ++it;
    }
    PublishToCursor(true);
}

void ApplySettings() {
    auto cfg = Cfg();
    for (auto& [id, w] : g_wins) {
        w.target = AlphaForDepth(w.depth, w.hdr, *cfg);
        if (w.hWnd && w.aboveSystemUi != cfg->overlayAboveSystemUi) {
            // Moved to the other band: a new window, at the current alpha
            // (the next Tick applies it).
            DestroyWindow(w.hWnd);
            w.hWnd = nullptr;
        }
        if (!w.hWnd && w.target > 0) {
            Create(w, *cfg);
        } else if (w.hWnd) {
            ApplyCaptureAffinity(w.hWnd, *cfg);
        }
    }
    Osd::ApplyBand();
    StartAnimation();
    PublishToCursor();
}

void DestroyAll() {
    HWND hCtrl = g_hController;
    if (hCtrl) {
        KillTimer(hCtrl, Controller::kTimerAnim);
    }
    g_animating = false;
    SetRaiseTimer(false);
    for (auto& [id, w] : g_wins) {
        if (w.hWnd) {
            DestroyWindow(w.hWnd);
        }
    }
    g_wins.clear();
    // Unloading: the user's pointers back everywhere.
    CursorDim::Publish({}, false);
}

}  // namespace Overlay

// -----------------------------------------------------------------------------
// Per-screen brightness (taskbar thread)
//
// Each screen has one level from -100 to 100:
//   -100..-1  Dark Overlay depth (hardware stays at its minimum)
//      0..100 hardware/SDR brightness
// A scroll only ever changes the screen under the cursor.
// -----------------------------------------------------------------------------

namespace Bright {

enum class Dom { None, Backlight, Ddc, Sdr, OverlayOnly };

int ToWorker(Dom d) {
    switch (d) {
        case Dom::Backlight: return Worker::kBacklight;
        case Dom::Ddc: return Worker::kDdc;
        case Dom::Sdr: return Worker::kSdr;
        default: return -1;
    }
}

PCWSTR DomName(Dom d) {
    switch (d) {
        case Dom::Backlight: return L"backlight";
        case Dom::Ddc: return L"DDC/CI";
        case Dom::Sdr: return L"SDR";
        case Dom::OverlayOnly: return L"overlay only";
        default: return L"none";
    }
}

struct DomState {
    int level = -1;  // 0..100, -1 = unknown
    DWORD lastInputTick = 0;
};

struct ScreenState {
    bool primed = false;
    int darkDepth = 0;  // 0..100
    DomState dom[Worker::kDomainCount];
    int ddc = -1;     // -1 unknown, 0 not supported, 1 supported
    int sdrRaw = -1;  // last SDR target, 1000 = 80 nits
    DWORD lastWriteMs = 0;
};

std::unordered_map<std::wstring, ScreenState> g_state;
std::unordered_set<std::wstring> g_sdrDirty;

// The SDR white level this mod last set for an HDR screen (what the DWM
// composes with), 1000 = 80 nits; -1 if unknown.
int CurrentSdrRaw(const std::wstring& screenId) {
    auto it = g_state.find(screenId);
    return it != g_state.end() ? it->second.sdrRaw : -1;
}

int SdrRawFromLevel(int level, double maxSdr) {
    return (int)std::lround(1000.0 + (maxSdr * 1000.0 - 1000.0) * level / 100.0);
}

int SdrLevelFromRaw(int raw, double maxSdr) {
    double range = maxSdr * 1000.0 - 1000.0;
    if (range <= 0) {
        return 0;
    }
    return std::clamp((int)std::lround((raw - 1000) * 100.0 / range), 0, 100);
}

int NitsFromRaw(int raw) {
    return (int)std::lround(raw * 80.0 / 1000.0);
}

std::wstring SdrStoreKey(const std::wstring& screenId) {
    WCHAR buf[32];
    swprintf_s(buf, L"SdrRaw_%08X", HashString(screenId));
    return buf;
}

void FlushSdrSaves() {
    auto cfg = Cfg();
    if (cfg->storeSdr) {
        for (const auto& id : g_sdrDirty) {
            auto it = g_state.find(id);
            if (it != g_state.end() && it->second.sdrRaw >= 1000) {
                Wh_SetIntValue(SdrStoreKey(id).c_str(), it->second.sdrRaw);
            }
        }
    }
    g_sdrDirty.clear();
}

void ScheduleSdrSave(const std::wstring& id) {
    g_sdrDirty.insert(id);
    HWND hCtrl = g_hController;
    if (hCtrl) {
        SetTimer(hCtrl, Controller::kTimerSdrSave, 1500, nullptr);
    }
}

void PrimeSdr(const Topo::Screen& s, ScreenState& st, const Settings& cfg) {
    int maxRaw = (int)std::lround(cfg.maxSdrBrightness * 1000.0);
    int raw = -1;
    if (cfg.storeSdr) {
        int saved = Wh_GetIntValue(SdrStoreKey(s.id).c_str(), 0);
        if (saved >= 1000) {
            raw = std::clamp(saved, 1000, maxRaw);
            st.sdrRaw = raw;
            st.dom[Worker::kSdr].level = SdrLevelFromRaw(raw, cfg.maxSdrBrightness);
            Worker::SetTarget(s, Worker::kSdr, raw);
            Wh_Log(L"%s: restored SDR brightness %d nits", s.gdiName.c_str(),
                   NitsFromRaw(raw));
        }
    }
    if (raw < 0 && s.sdrWhite >= 1000) {
        st.sdrRaw = (int)s.sdrWhite;
        st.dom[Worker::kSdr].level =
            SdrLevelFromRaw(st.sdrRaw, cfg.maxSdrBrightness);
    }
    Worker::SetWatch(s, Worker::kSdr, true);
}

void PrimeScreen(const Topo::Screen& s) {
    auto cfg = Cfg();
    ScreenState& st = g_state[s.id];
    if (st.primed || !cfg->HasBrightnessEntry()) {
        return;
    }
    st.primed = true;
    if (s.hdr) {
        PrimeSdr(s, st, *cfg);
    }
    if (s.internal) {
        Worker::SetWatch(s, Worker::kBacklight, true);
        Worker::ReadAsync(s, Worker::kBacklight);
    } else if (cfg->HasAction(ScrollAction::brightnessChange) ||
               cfg->HasAction(ScrollAction::masterBrightnessChange)) {
        Worker::ReadAsync(s, Worker::kDdc);  // DDC/CI probe
    }
    Wh_Log(L"Screen %s: %s, %s, SDR white %lu", Topo::ShortName(s).c_str(),
           s.internal ? L"internal" : L"external", s.hdr ? L"HDR" : L"SDR",
           s.sdrWhite);
}

bool SameTopology(const std::vector<Topo::Screen>& a,
                  const std::vector<Topo::Screen>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i].id != b[i].id || a[i].hMon != b[i].hMon ||
            !EqualRect(&a[i].rc, &b[i].rc) || a[i].hdr != b[i].hdr) {
            return false;
        }
    }
    return true;
}

void RefreshTopology() {
    auto screens = Topo::Query();
    Topo::g_lastRefreshTick = GetTickCount();
    if (screens.empty()) {
        return;
    }
    auto cfg = Cfg();
    bool changed = !SameTopology(Topo::g_screens, screens);
    for (const auto& s : screens) {
        const Topo::Screen* old = Topo::FindById(s.id);
        if (old && old->hdr != s.hdr) {
            Wh_Log(L"%s: HDR turned %s", s.gdiName.c_str(),
                   s.hdr ? L"on" : L"off");
            auto it = g_state.find(s.id);
            if (it != g_state.end()) {
                it->second.dom[Worker::kSdr].level = -1;
                it->second.sdrRaw = -1;
            }
            // While HDR is off the SDR white level means nothing; don't let
            // its change look like an outside brightness change.
            Worker::SetWatch(s, Worker::kSdr, false);
        }
    }
    Topo::g_screens = std::move(screens);
    if (!changed) {
        return;
    }
    Worker::UpdateScreens(Topo::g_screens);
    Overlay::OnTopology();

    for (auto& [id, st] : g_state) {
        const Topo::Screen* s = Topo::FindById(id);
        if (!s) {
            st.darkDepth = 0;  // overlay was destroyed with the screen
            continue;
        }
        if (s->hdr && st.primed && st.dom[Worker::kSdr].level < 0) {
            PrimeSdr(*s, st, *cfg);  // HDR was turned on
        }
    }
    for (const auto& s : Topo::g_screens) {
        PrimeScreen(s);
    }
    Overlay::PublishToCursor();
}

void OnControllerCreated() {
    RefreshTopology();
}

bool EnsureDdcKnown(const Topo::Screen& s, ScreenState& st) {
    if (st.ddc == -1) {
        int v = Worker::ReadSync(s, Worker::kDdc, 1500);
        st.ddc = v >= 0 ? 1 : 0;
        if (v >= 0) {
            st.dom[Worker::kDdc].level = v;
            Worker::SetWatch(s, Worker::kDdc, true);
        }
    }
    return st.ddc == 1;
}

Dom Resolve(ScrollAction action, const Topo::Screen& s, ScreenState& st) {
    switch (action) {
        case ScrollAction::brightnessChange:
            if (s.internal) {
                return Dom::Backlight;
            }
            if (EnsureDdcKnown(s, st)) {
                return Dom::Ddc;
            }
            return s.hdr ? Dom::Sdr : Dom::OverlayOnly;
        case ScrollAction::masterBrightnessChange:
            if (s.internal) {
                return Dom::Backlight;
            }
            if (s.hdr) {
                return Dom::Sdr;
            }
            return EnsureDdcKnown(s, st) ? Dom::Ddc : Dom::OverlayOnly;
        case ScrollAction::sdrToHdrBrightnessChange:
            return s.hdr ? Dom::Sdr : Dom::None;
        default:
            return Dom::None;
    }
}

bool EnsureLevel(const Topo::Screen& s, ScreenState& st, Dom d) {
    int wd = ToWorker(d);
    if (wd < 0) {
        return true;
    }
    DomState& ds = st.dom[wd];
    if (ds.level >= 0) {
        return true;
    }
    auto cfg = Cfg();
    switch (d) {
        case Dom::Backlight: {
            int v = Worker::ReadSync(s, Worker::kBacklight, 800);
            if (v >= 0) {
                ds.level = v;
                Worker::SetWatch(s, Worker::kBacklight, true);
            }
            break;
        }
        case Dom::Ddc: {
            int v = Worker::ReadSync(s, Worker::kDdc, 1500);
            if (v >= 0) {
                ds.level = v;
                Worker::SetWatch(s, Worker::kDdc, true);
            }
            break;
        }
        case Dom::Sdr: {
            int raw = st.sdrRaw;
            if (raw < 1000) {
                raw = (int)Topo::ReadSdrWhite(s.adapterId, s.targetId);
            }
            if (raw >= 1000) {
                st.sdrRaw = raw;
                ds.level = SdrLevelFromRaw(raw, cfg->maxSdrBrightness);
                Worker::SetWatch(s, Worker::kSdr, true);
            }
            break;
        }
        default:
            break;
    }
    return ds.level >= 0;
}

int CurrentLevel(const ScreenState& st, Dom d) {
    if (st.darkDepth > 0) {
        return -st.darkDepth;
    }
    int wd = ToWorker(d);
    return wd >= 0 ? std::max(0, st.dom[wd].level) : 0;
}

void ShowOsd(const Topo::Screen& s,
             const ScreenState& st,
             Dom d,
             int level,
             const Settings& cfg) {
    Osd::Model m;
    m.bipolar = cfg.darkOverlayEnabled;
    m.level = level;
    m.maxLevel = d == Dom::OverlayOnly ? 0 : 100;
    m.value = Osd::FormatPercent(level);
    if (level < 0) {
        m.icon = Osd::Icon::Dark;
        m.label = L"Dark Overlay";
    } else {
        // Same label on every screen (laptop, DDC/CI or HDR): "Brightness".
        m.icon = Osd::Icon::Brightness;
        switch (d) {
            case Dom::OverlayOnly:
                m.icon = Osd::Icon::Dark;
                m.label = L"Dark Overlay";
                m.value = L"off";
                m.secondary = L"No brightness control on this screen";
                break;
            default:
                m.label = L"Brightness";
                break;
        }
    }
    if (!m.bipolar) {
        m.level = std::max(0, level);
    }
    if (cfg.debugOsd) {
        std::wstring display = s.gdiName;
        size_t pos = display.find_last_of(L'\\');
        if (pos != std::wstring::npos) {
            display = display.substr(pos + 1);
        }
        WCHAR buf[256];
        swprintf_s(buf, L"%s · %s · %s · α %d · %lu ms",
                   display.c_str(), s.hdr ? L"HDR" : L"SDR", DomName(d),
                   Overlay::TargetAlpha(s.id), st.lastWriteMs);
        m.debug = buf;
    }
    Osd::Show(m, s.hMon);
}

// Applies a scroll to exactly one screen. Everything it touches (level, Dark
// Overlay, SDR boost, DDC/CI, worker slot, saved value) belongs to that
// screen only, so any number of screens stay independent.
void ApplyScroll(ScrollAction action,
                 const Topo::Screen& screen,
                 int clicks,
                 bool showOsd) {
    auto cfg = Cfg();
    DWORD now = GetTickCount();
    ScreenState& st = g_state[screen.id];
    if (!st.primed) {
        PrimeScreen(screen);
    }

    Dom d = Resolve(action, screen, st);
    if (d == Dom::None) {
        if (showOsd) {
            // HDR-only action on a screen without HDR.
            Osd::Model m;
            m.icon = Osd::Icon::Warning;
            m.label = L"Brightness";
            m.value = L"not available";
            m.showBar = false;
            Osd::Show(m, screen.hMon);
        }
        return;
    }
    if (d != Dom::OverlayOnly && !EnsureLevel(screen, st, d)) {
        Wh_Log(L"%s: %s brightness unavailable, using the Dark Overlay only",
               screen.gdiName.c_str(), DomName(d));
        d = Dom::OverlayOnly;
    }

    int wd = ToWorker(d);
    int hw = wd >= 0 ? st.dom[wd].level : 0;
    int before = CurrentLevel(st, d);
    int lo = cfg->darkOverlayEnabled ? -100 : 0;
    int hi = d == Dom::OverlayOnly ? 0 : 100;
    int after = std::clamp(before + clicks, lo, hi);

    // Normally the hardware sits at 0 while darkened. If it was raised
    // elsewhere and the overlay kept (auto-reset off), it is left alone while
    // darkened, and leaving the overlay continues from it instead of dropping
    // to 0 first.
    bool hwRaisedUnderOverlay = st.darkDepth > 0 && hw > 0;
    int newHw, newDark;
    if (after < 0) {
        newDark = -after;
        newHw = hwRaisedUnderOverlay ? hw : 0;
    } else {
        newDark = 0;
        newHw = hwRaisedUnderOverlay ? std::min(hi, hw + after) : after;
    }

    if (wd >= 0) {
        DomState& ds = st.dom[wd];
        ds.lastInputTick = now;
        if (newHw != hw) {
            ds.level = newHw;
            int target = newHw;
            if (d == Dom::Sdr) {
                target = SdrRawFromLevel(newHw, cfg->maxSdrBrightness);
                st.sdrRaw = target;
                ScheduleSdrSave(screen.id);
                Overlay::PublishToCursor();
            }
            Worker::SetTarget(screen, wd, target);
        } else {
            Worker::NoteInput(screen, wd);
        }
    }
    if (newDark != st.darkDepth) {
        st.darkDepth = newDark;
        Overlay::SetDepth(screen, newDark);
    }

    int level = CurrentLevel(st, d);
    Wh_Log(L"Brightness %s [%s%s, %s]: %d -> %d (clicks %d, hw %d -> %d, dark "
           L"%d)",
           Topo::ShortName(screen).c_str(),
           screen.internal ? L"internal" : L"external",
           screen.hdr ? L" HDR" : L"", DomName(d), before, level, clicks, hw,
           newHw, newDark);

    if (showOsd) {
        ShowOsd(screen, st, d, level, *cfg);
    }
}

// The screen is always the one under the cursor.
void OnScroll(ScrollAction action, POINT pt, int clicks, bool showOsd) {
    if (!EnsureController()) {
        return;
    }
    if (Topo::g_screens.empty() ||
        GetTickCount() - Topo::g_lastRefreshTick > 2000) {
        RefreshTopology();
    }
    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    const Topo::Screen* found = Topo::FindByMonitor(hMon);
    if (!found) {
        RefreshTopology();
        found = Topo::FindByMonitor(hMon);
        if (!found) {
            Wh_Log(L"No screen found for the cursor position");
            return;
        }
    }
    const Topo::Screen screen = *found;  // copy: a refresh may reallocate
    ApplyScroll(action, screen, clicks, showOsd);
}

void OnHwMessage(const Worker::HwMsg& m) {
    auto it = g_state.find(m.screenId);
    if (it == g_state.end()) {
        return;
    }
    ScreenState& st = it->second;
    const Topo::Screen* s = Topo::FindById(m.screenId);
    auto cfg = Cfg();
    DomState& ds = st.dom[m.domain];
    DWORD now = GetTickCount();
    bool userIdle = now - ds.lastInputTick > 1000;

    switch (m.kind) {
        case Worker::MsgKind::Initial:
            if (m.domain == Worker::kDdc) {
                st.ddc = m.value >= 0 ? 1 : 0;
                if (m.value >= 0 && s) {
                    Worker::SetWatch(*s, Worker::kDdc, true);
                }
            }
            if (m.value >= 0 && ds.level < 0) {
                ds.level = m.domain == Worker::kSdr
                               ? SdrLevelFromRaw(m.value, cfg->maxSdrBrightness)
                               : m.value;
            }
            break;

        case Worker::MsgKind::Settled:
            if (userIdle && m.value >= 0 && m.domain != Worker::kSdr) {
                ds.level = m.value;
            }
            break;

        case Worker::MsgKind::External:
            if (!userIdle || m.value < 0 ||
                (m.domain == Worker::kSdr && (!s || !s->hdr))) {
                break;
            }
            if (m.domain == Worker::kSdr) {
                st.sdrRaw = m.value;
                ds.level = SdrLevelFromRaw(m.value, cfg->maxSdrBrightness);
                ScheduleSdrSave(m.screenId);
                Overlay::PublishToCursor();
            } else {
                ds.level = m.value;
            }
            Wh_Log(L"%s: %s brightness changed outside the mod (now %d)",
                   s ? s->gdiName.c_str() : L"?",
                   Worker::DomainName(m.domain), m.value);
            if (st.darkDepth > 0 && cfg->autoResetDarkOverlay && s) {
                Wh_Log(L"%s: Dark Overlay reset (was -%d%%)",
                       s->gdiName.c_str(), st.darkDepth);
                st.darkDepth = 0;
                Overlay::SetDepth(*s, 0);
            }
            break;

        case Worker::MsgKind::Written:
            st.lastWriteMs = m.elapsedMs;
            break;
    }
}

void ApplySettings() {
    auto cfg = Cfg();
    for (auto& [id, st] : g_state) {
        if (!cfg->darkOverlayEnabled) {
            st.darkDepth = 0;
        }
        if (st.sdrRaw >= 1000) {
            st.dom[Worker::kSdr].level =
                SdrLevelFromRaw(st.sdrRaw, cfg->maxSdrBrightness);
        }
        const Topo::Screen* s = Topo::FindById(id);
        if (s && Overlay::g_wins.count(id)) {
            Overlay::SetDepth(*s, st.darkDepth);
        }
    }
    Overlay::ApplySettings();
    for (const auto& s : Topo::g_screens) {
        PrimeScreen(s);
    }
    Overlay::PublishToCursor();
}

void Reset() {
    g_state.clear();
    g_sdrDirty.clear();
    Topo::g_screens.clear();
    Topo::g_lastRefreshTick = 0;
}

}  // namespace Bright

// -----------------------------------------------------------------------------
// Controller window procedure and lifetime
// -----------------------------------------------------------------------------

namespace Controller {

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_TIMER:
            switch (wParam) {
                case kTimerAnim:
                    Overlay::Tick();
                    return 0;
                case kTimerRaise:
                    Overlay::RaiseAll();
                    return 0;
                case kTimerTopology:
                    KillTimer(hWnd, kTimerTopology);
                    Bright::RefreshTopology();
                    return 0;
                case kTimerSdrSave:
                    KillTimer(hWnd, kTimerSdrSave);
                    Bright::FlushSdrSaves();
                    return 0;
                case kTimerSndVol:
                    CloseSndVolTimerTick();
                    return 0;
                case kTimerPerAppTooltip:
                    KillTimer(hWnd, kTimerPerAppTooltip);
                    PerApp::OnTooltipHideTimer();
                    return 0;
            }
            break;

        case WM_DISPLAYCHANGE:
            // Debounce: several of these arrive during a mode change.
            SetTimer(hWnd, kTimerTopology, 600, nullptr);
            break;

        case WM_SETTINGCHANGE:
            if (wParam == SPI_SETCURSORS) {
                CursorDim::OnSchemeChanged();
            }
            break;

        case WM_DESTROY: {
            MSG msg;
            while (PeekMessage(&msg, hWnd, Worker::g_msgHw, Worker::g_msgHw,
                               PM_REMOVE)) {
                delete (Worker::HwMsg*)msg.lParam;
            }
            break;
        }

        default:
            if (uMsg == Worker::g_msgHw) {
                auto* m = (Worker::HwMsg*)lParam;
                if (m) {
                    Bright::OnHwMessage(*m);
                    delete m;
                }
                return 0;
            }
            if (uMsg == g_msgApplySettings) {
                Bright::ApplySettings();
                return 0;
            }
            break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool RegisterClasses() {
    if (g_classesRegistered) {
        return true;
    }
    HINSTANCE hInst = ThisModule();
    struct {
        PCWSTR name;
        WNDPROC proc;
        HBRUSH bg;
    } classes[] = {
        {kClassName, WndProc, nullptr},
        // DefWindowProc + black background: even if something went wrong,
        // an overlay window never calls into this DLL.
        {kDarkClassName, DefWindowProcW, (HBRUSH)GetStockObject(BLACK_BRUSH)},
        {kOsdClassName, Osd::WndProc, nullptr},
    };
    bool registered[ARRAYSIZE(classes)] = {};
    for (size_t i = 0; i < ARRAYSIZE(classes); i++) {
        const auto& c = classes[i];
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = c.proc;
        wc.hInstance = hInst;
        wc.lpszClassName = c.name;
        wc.hbrBackground = c.bg;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        if (RegisterClassExW(&wc)) {
            registered[i] = true;
            continue;
        }
        DWORD error = GetLastError();
        // An existing class can only come from an earlier instance of the mod
        // that couldn't unregister it. Its window procedure would point into
        // that unloaded image, so it can't be used, unless it's the
        // DefWindowProc class of the overlays, which never calls into a mod.
        if (error == ERROR_CLASS_ALREADY_EXISTS && c.proc == DefWindowProcW) {
            continue;
        }
        Wh_Log(L"RegisterClassEx(%s) failed (%lu)", c.name, error);
        for (size_t j = 0; j < i; j++) {
            if (registered[j]) {
                UnregisterClassW(classes[j].name, hInst);
            }
        }
        return false;
    }
    g_classesRegistered = true;
    return true;
}

void UnregisterClasses() {
    if (!g_classesRegistered) {
        return;
    }
    HINSTANCE hInst = ThisModule();
    for (PCWSTR name : {kClassName, kDarkClassName, kOsdClassName}) {
        if (!UnregisterClassW(name, hInst)) {
            Wh_Log(L"UnregisterClass(%s) failed (%lu)", name, GetLastError());
        }
    }
    g_classesRegistered = false;
}

}  // namespace Controller

// Must be called on the taskbar thread.
bool EnsureController() {
    HWND h = g_hController;
    if (h && IsWindow(h)) {
        return true;
    }
    if (g_unloading || !g_dwTaskbarThreadId ||
        GetCurrentThreadId() != g_dwTaskbarThreadId) {
        return false;
    }
    if (!Controller::RegisterClasses()) {
        return false;
    }
    Bright::Reset();
    h = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                        Controller::kClassName, L"", WS_POPUP, 0, 0, 0, 0,
                        nullptr, nullptr, ThisModule(), nullptr);
    if (!h) {
        Wh_Log(L"Controller: CreateWindowEx failed (%lu)", GetLastError());
        return false;
    }
    g_hController = h;
    Bright::OnControllerCreated();
    return true;
}

void CreateControllerFromAnyThread() {
    if (!g_hTaskbarWnd) {
        return;
    }
    RunFromWindowThread(
        g_hTaskbarWnd, [](void*) { EnsureController(); }, nullptr);
}

// -----------------------------------------------------------------------------
// Virtual desktop switching
// -----------------------------------------------------------------------------

// Keyboard simulation keeps the Windows switch animation. When triggered by a
// scroll-anywhere combo, the held Shift/Alt are released for the shortcut and
// pressed again afterwards (and held Ctrl/Win are reused), so the combination
// stays Win+Ctrl+Arrow and the user's keys stay logically held.
bool SwitchDesktopViaKeyboardShortcut(int clicks, bool fromScrollAnywhere) {
    if (IsKeyDownForInput(VK_PRIOR) || IsKeyDownForInput(VK_NEXT)) {
        return false;
    }
    auto down = [](int vk) { return IsKeyDownForInput(vk); };
    bool lshift = down(VK_LSHIFT), rshift = down(VK_RSHIFT);
    bool lctrl = down(VK_LCONTROL), rctrl = down(VK_RCONTROL);
    bool lalt = down(VK_LMENU), ralt = down(VK_RMENU);
    bool lwin = down(VK_LWIN), rwin = down(VK_RWIN);
    if (!fromScrollAnywhere &&
        (lshift || rshift || lctrl || rctrl || lalt || ralt)) {
        return false;
    }

    // Allows switching even if the foreground window is elevated.
    HWND hTaskbarWnd =
        g_hTaskbarWnd ? g_hTaskbarWnd : FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        SetForegroundWindow(hTaskbarWnd);
    }

    WORD arrow = VK_LEFT;
    if (clicks < 0) {
        clicks = -clicks;
        arrow = VK_RIGHT;
    }
    clicks = std::min(clicks, 20);

    std::vector<INPUT> in;
    auto key = [&in](WORD vk, bool up, bool extended) {
        INPUT i = {};
        i.type = INPUT_KEYBOARD;
        i.ki.wVk = vk;
        i.ki.dwFlags = (up ? KEYEVENTF_KEYUP : 0) |
                       (extended ? KEYEVENTF_EXTENDEDKEY : 0);
        i.ki.dwExtraInfo = kInjectedInputTag;
        in.push_back(i);
    };

    if (lshift) key(VK_LSHIFT, true, false);
    if (rshift) key(VK_RSHIFT, true, false);
    if (lalt) key(VK_LMENU, true, false);
    if (ralt) key(VK_RMENU, true, true);
    bool injectWin = !(lwin || rwin);
    bool injectCtrl = !(lctrl || rctrl);
    if (injectWin) key(VK_LWIN, false, false);
    if (injectCtrl) key(VK_LCONTROL, false, false);
    for (int i = 0; i < clicks; i++) {
        key(arrow, false, false);
        key(arrow, true, false);
    }
    if (injectCtrl) key(VK_LCONTROL, true, false);
    if (injectWin) key(VK_LWIN, true, false);
    if (lalt) key(VK_LMENU, false, false);
    if (ralt) key(VK_RMENU, false, true);
    if (lshift) key(VK_LSHIFT, false, false);
    if (rshift) key(VK_RSHIFT, false, false);

    if (lalt || ralt) {
        g_altMaskState.usedForScroll = true;
    }
    SendInput((UINT)in.size(), in.data(), sizeof(INPUT));
    return true;
}

// -----------------------------------------------------------------------------
// Audio: shared device enumerator (created/released on the taskbar thread)
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
const static GUID XIID_IAudioSessionManager2 = {
    0x77AA99A0, 0x1BD6, 0x484F,
    {0x8B, 0xC7, 0x2C, 0x65, 0x4C, 0x9A, 0x9B, 0x6F}};

IMMDeviceEnumerator* g_pDeviceEnumerator;

void AudioInit() {
    if (g_pDeviceEnumerator) {
        return;
    }
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
}

// Calls fn with the default endpoint volume of the given data flow.
template <typename Fn>
bool WithEndpointVolume(EDataFlow flow, Fn fn) {
    AudioInit();
    if (!g_pDeviceEnumerator) {
        return false;
    }
    IMMDevice* device = NULL;
    if (FAILED(g_pDeviceEnumerator->GetDefaultAudioEndpoint(flow, eConsole,
                                                           &device))) {
        return false;
    }
    IAudioEndpointVolume* volume = NULL;
    bool result = false;
    if (SUCCEEDED(device->Activate(XIID_IAudioEndpointVolume,
                                   CLSCTX_INPROC_SERVER, NULL,
                                   (LPVOID*)&volume))) {
        result = fn(volume);
        volume->Release();
    }
    device->Release();
    return result;
}

// -----------------------------------------------------------------------------
// Microphone volume
// -----------------------------------------------------------------------------

bool AddMicMasterVolumeLevelScalar(float add) {
    return WithEndpointVolume(eCapture, [add](IAudioEndpointVolume* v) {
        float cur;
        if (FAILED(v->GetMasterVolumeLevelScalar(&cur))) {
            return false;
        }
        return SUCCEEDED(v->SetMasterVolumeLevelScalar(
            std::clamp(cur + add, 0.0f, 1.0f), NULL));
    });
}

float GetCurrentMicVolumeScalar() {
    float result = -1.0f;
    WithEndpointVolume(eCapture, [&result](IAudioEndpointVolume* v) {
        return SUCCEEDED(v->GetMasterVolumeLevelScalar(&result));
    });
    return result;
}

// -----------------------------------------------------------------------------
// System volume (from Taskbar Volume Control)
// -----------------------------------------------------------------------------

bool IsDefaultAudioEndpointAvailable() {
    AudioInit();
    if (!g_pDeviceEnumerator) {
        return false;
    }
    IMMDevice* device = NULL;
    if (SUCCEEDED(g_pDeviceEnumerator->GetDefaultAudioEndpoint(
            eRender, eConsole, &device))) {
        device->Release();
        return true;
    }
    return false;
}

bool IsVolMuted(BOOL* pbMuted) {
    return WithEndpointVolume(eRender, [pbMuted](IAudioEndpointVolume* v) {
        return SUCCEEDED(v->GetMute(pbMuted));
    });
}

bool ToggleVolMuted() {
    return WithEndpointVolume(eRender, [](IAudioEndpointVolume* v) {
        BOOL muted;
        return SUCCEEDED(v->GetMute(&muted)) &&
               SUCCEEDED(v->SetMute(!muted, NULL));
    });
}

bool AddMasterVolumeLevelScalar(float add) {
    bool noAutoMute = Cfg()->noAutomaticMuteToggle;
    return WithEndpointVolume(eRender, [add,
                                        noAutoMute](IAudioEndpointVolume* v) {
        float cur;
        if (FAILED(v->GetMasterVolumeLevelScalar(&cur))) {
            return false;
        }
        float vol = std::clamp(cur + add, 0.0f, 1.0f);
        if (FAILED(v->SetMasterVolumeLevelScalar(vol, NULL))) {
            return false;
        }
        if (!noAutoMute) {
            // Windows shows [0, 0.005) as 0% and mutes at 0; do the same.
            v->SetMute(vol < 0.005f, NULL);
        }
        return true;
    });
}

bool AdjustVolumeLevelWithMouseWheel(int nWheelDelta) {
    int step = Cfg()->volumeChangeStep;
    if (step <= 0) {
        step = 2;
    }
    return AddMasterVolumeLevelScalar((float)nWheelDelta * step *
                                      (0.01f / WHEEL_DELTA));
}

// -----------------------------------------------------------------------------
// Windows 11 native volume indicator
// -----------------------------------------------------------------------------

UINT g_uShellHookMsg = RegisterWindowMessage(L"SHELLHOOK");
DWORD g_volLastScrollTime;
short g_volLastScrollDeltaRemainder;

bool PostAppCommand(SHORT appCommand, int count) {
    if (!g_hTaskbarWnd) {
        return false;
    }
    HWND hReBarWindow32 =
        FindWindowEx(g_hTaskbarWnd, nullptr, L"ReBarWindow32", nullptr);
    if (!hReBarWindow32) {
        return false;
    }
    HWND hMSTaskSwWClass =
        FindWindowEx(hReBarWindow32, nullptr, L"MSTaskSwWClass", nullptr);
    if (!hMSTaskSwWClass) {
        return false;
    }
    for (int i = 0; i < count; i++) {
        PostMessage(hMSTaskSwWClass, g_uShellHookMsg, HSHELL_APPCOMMAND,
                    MAKELPARAM(0, appCommand));
    }
    return true;
}

// Each APPCOMMAND_VOLUME_* changes the volume by 2% and shows the flyout. The
// rest of the step is applied directly, so any step size works (including odd
// ones).
bool Win11IndicatorAdjustVolumeLevelWithMouseWheel(short delta) {
    auto cfg = Cfg();
    BOOL muted;
    if (cfg->noAutomaticMuteToggle && IsVolMuted(&muted) && muted) {
        return true;
    }

    if (GetTickCount() - g_volLastScrollTime < kWheelAccumulationWindowMs) {
        delta += g_volLastScrollDeltaRemainder;
    }

    int clicks = delta / WHEEL_DELTA;
    if (clicks) {
        int step = cfg->volumeChangeStep > 0 ? cfg->volumeChangeStep : 2;
        int percent = clicks * step;
        int sign = percent > 0 ? 1 : -1;
        SHORT appCommand =
            percent > 0 ? APPCOMMAND_VOLUME_UP : APPCOMMAND_VOLUME_DOWN;
        int manual = percent - sign * 2;
        if (manual != 0) {
            AddMasterVolumeLevelScalar(manual * 0.01f);
        }
        if (!PostAppCommand(appCommand, 1)) {
            return false;
        }
    }

    g_volLastScrollTime = GetTickCount();
    g_volLastScrollDeltaRemainder = delta % WHEEL_DELTA;
    return true;
}

// -----------------------------------------------------------------------------
// SndVol (classic + modern indicators). Timer runs on the controller window.
// -----------------------------------------------------------------------------

static HANDLE hSndVolProcess;
static HWND hSndVolWnd;
static bool g_sndVolTimerActive;
static int nCloseSndVolTimerCount;
static HWND hSndVolModernPreviousForegroundWnd;
static BOOL bSndVolModernLaunched;
static BOOL bSndVolModernAppeared;

static BOOL CanUseModernIndicator() {
    if (g_nWinVersion < WIN_VERSION_10_T1 ||
        Cfg()->volumeIndicator == VolumeIndicator::Classic) {
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
    if (!hForegroundWnd) {
        return NULL;
    }
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
    if (GetLastError() != 0) {
        return NULL;
    }
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
    if (!hBluetoothNotificationWnd) {
        return NULL;
    }
    HWND hWnd = NULL;
    EnumThreadWindows(
        GetWindowThreadProcessId(hBluetoothNotificationWnd, NULL),
        EnumThreadFindSndVolTrayControlWnd, (LPARAM)&hWnd);
    return hWnd;
}

static BOOL ShowSndVolModernIndicator() {
    if (bSndVolModernLaunched) {
        return TRUE;
    }
    if (GetOpenSndVolModernIndicatorWnd()) {
        return TRUE;
    }
    HWND hForegroundWnd = GetForegroundWindow();
    if (hForegroundWnd && hForegroundWnd != g_hTaskbarWnd) {
        hSndVolModernPreviousForegroundWnd = hForegroundWnd;
    }
    HWND hSndVolTrayControlWnd = GetSndVolTrayControlWnd();
    if (!hSndVolTrayControlWnd) {
        return FALSE;
    }
    if (!PostMessage(hSndVolTrayControlWnd, 0x460, 0,
                     MAKELPARAM(NIN_SELECT, 100))) {
        return FALSE;
    }
    bSndVolModernLaunched = TRUE;
    return TRUE;
}

static BOOL HideSndVolModernIndicator() {
    if (GetOpenSndVolModernIndicatorWnd()) {
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
    if (!hSndVolProcess) {
        return FALSE;
    }
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
    if (hSndVolWnd == hForegroundWnd) {
        return TRUE;
    }
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
    if (!hChildDlg) {
        return FALSE;
    }
    return (GetWindowLong(hChildDlg, GWL_STYLE) & WS_VISIBLE) != 0;
}

static BOOL MoveSndVolCenterMouse(HWND hWnd) {
    NOTIFYICONIDENTIFIER notifyiconidentifier{};
    notifyiconidentifier.cbSize = sizeof(NOTIFYICONIDENTIFIER);
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
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                     SWP_ASYNCWINDOWPOS);
    return TRUE;
}

static BOOL OpenScrollSndVolInternal(HWND hVolumeAppWnd, BOOL* pbOpened) {
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

void KillSndVolTimer() {
    HWND hCtrl = g_hController;
    if (g_sndVolTimerActive && hCtrl) {
        KillTimer(hCtrl, Controller::kTimerSndVol);
    }
    g_sndVolTimerActive = false;
}

void SetSndVolTimer() {
    if (!EnsureController()) {
        return;
    }
    SetTimer(g_hController, Controller::kTimerSndVol, 100, nullptr);
    g_sndVolTimerActive = true;
    nCloseSndVolTimerCount = 0;
}

void CloseSndVolTimerTick() {
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
            }
            nCloseSndVolTimerCount++;
            if (nCloseSndVolTimerCount < 10) {
                return;
            }
        } else if (hSndVolModernIndicatorWnd) {
            POINT pt;
            GetCursorPos(&pt);
            HWND hPointWnd = GetAncestor(WindowFromPoint(pt), GA_ROOT);
            if (!hPointWnd) {
                nCloseSndVolTimerCount++;
            } else if (hPointWnd == hSndVolModernIndicatorWnd) {
                nCloseSndVolTimerCount = 0;
            } else if (IsTaskbarWindow(hPointWnd) &&
                       IsPointInsideAnyScrollArea(hPointWnd, pt)) {
                nCloseSndVolTimerCount = 0;
            } else {
                nCloseSndVolTimerCount++;
            }
            if (nCloseSndVolTimerCount < 10) {
                return;
            }
            HideSndVolModernIndicator();
        }
        EndSndVolModernIndicatorSession();
    } else if (ValidateSndVolProcess()) {
        if (WaitForInputIdle(hSndVolProcess, 0) != 0) {
            return;
        }
        if (ValidateSndVolWnd()) {
            POINT pt;
            GetCursorPos(&pt);
            HWND hPointWnd = GetAncestor(WindowFromPoint(pt), GA_ROOT);
            if (!hPointWnd) {
                nCloseSndVolTimerCount++;
            } else if (hPointWnd == hSndVolWnd) {
                nCloseSndVolTimerCount = 0;
            } else if (IsTaskbarWindow(hPointWnd) &&
                       IsPointInsideAnyScrollArea(hPointWnd, pt)) {
                nCloseSndVolTimerCount = 0;
            } else {
                nCloseSndVolTimerCount++;
            }
            if (nCloseSndVolTimerCount < 10) {
                return;
            }
            if (hPointWnd != hSndVolWnd) {
                PostMessage(hSndVolWnd, WM_ACTIVATE,
                            MAKEWPARAM(WA_INACTIVE, FALSE), (LPARAM)NULL);
            }
        }
    }
    KillSndVolTimer();
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
    auto cfg = Cfg();
    short delta = GET_WHEEL_DELTA_WPARAM(wParam);

    if (cfg->volumeIndicator == VolumeIndicator::Win11 &&
        g_nWinVersion >= WIN_VERSION_11_22H2) {
        return Win11IndicatorAdjustVolumeLevelWithMouseWheel(delta);
    }

    if (cfg->volumeIndicator == VolumeIndicator::None) {
        return AdjustVolumeLevelWithMouseWheel(delta);
    }

    if (CanUseModernIndicator()) {
        if (!AdjustVolumeLevelWithMouseWheel(delta)) {
            return FALSE;
        }
        ShowSndVolModernIndicator();
        SetSndVolTimer();
        return TRUE;
    }

    if (!IsDefaultAudioEndpointAvailable()) {
        return FALSE;
    }
    if (!AdjustVolumeLevelWithMouseWheel(delta)) {
        return FALSE;
    }

    if (ValidateSndVolProcess()) {
        if (WaitForInputIdle(hSndVolProcess, 0) == 0) {
            if (ValidateSndVolWnd()) {
                return FALSE;
            }
            HWND hVolumeAppWnd = FindWindow(L"Windows Volume App Window",
                                            L"Windows Volume App Window");
            if (hVolumeAppWnd) {
                DWORD dwProcessId;
                GetWindowThreadProcessId(hVolumeAppWnd, &dwProcessId);
                if (GetProcessId(hSndVolProcess) == dwProcessId) {
                    BOOL bOpened;
                    if (OpenScrollSndVolInternal(hVolumeAppWnd, &bOpened)) {
                        if (bOpened) {
                            SetSndVolTimer();
                        }
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
                    if (ValidateSndVolWnd()) {
                        return FALSE;
                    }
                    BOOL bOpened;
                    if (OpenScrollSndVolInternal(hVolumeAppWnd, &bOpened)) {
                        if (bOpened) {
                            SetSndVolTimer();
                        }
                        return bOpened;
                    }
                }
            }
        }
        return FALSE;
    }

    WCHAR szCommandLine[sizeof("SndVol.exe -f 4294967295")];
    wsprintf(szCommandLine, L"SndVol.exe -f %u", (DWORD)lMousePosParam);
    STARTUPINFO si{};
    si.cb = sizeof(STARTUPINFO);
    PROCESS_INFORMATION pi;
    if (!CreateProcess(NULL, szCommandLine, NULL, NULL, FALSE,
                       ABOVE_NORMAL_PRIORITY_CLASS | CREATE_SUSPENDED, NULL,
                       NULL, &si, &pi)) {
        return FALSE;
    }

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
// Per-app volume (from m417z's Taskbar Volume Control Per-App 1.1.4)
//
// Hold the configured modifier keys and scroll over a task button to change
// that app's volume. Rules that keep everything else working:
//   - only acts while exactly the configured modifiers are held,
//   - only over a task button (tracked from XAML pointer events and checked
//     against the cursor position),
//   - in every other case the wheel goes through the normal scroll actions.
// If any required symbol is missing, only this feature is disabled.
// -----------------------------------------------------------------------------

namespace PerApp {

std::atomic<bool> g_active;
// Set by the task button wheel hook; used for diagnostics only.
bool g_wheelHandled;

using AudioSessionCallback =
    std::function<bool(DWORD pid, IAudioSessionControl2* sessionControl)>;

bool ForEachAudioSession(const AudioSessionCallback& callback) {
    AudioInit();
    if (!g_pDeviceEnumerator) {
        return false;
    }

    winrt::com_ptr<IMMDevice> defaultDevice;
    if (FAILED(g_pDeviceEnumerator->GetDefaultAudioEndpoint(
            eRender, eConsole, defaultDevice.put()))) {
        return false;
    }

    winrt::com_ptr<IAudioSessionManager2> sessionManager;
    if (FAILED(defaultDevice->Activate(XIID_IAudioSessionManager2, CLSCTX_ALL,
                                       NULL, sessionManager.put_void()))) {
        return false;
    }

    winrt::com_ptr<IAudioSessionEnumerator> sessionEnumerator;
    if (FAILED(sessionManager->GetSessionEnumerator(sessionEnumerator.put()))) {
        return false;
    }

    int sessionCount = 0;
    if (FAILED(sessionEnumerator->GetCount(&sessionCount))) {
        return false;
    }

    bool foundSession = false;
    for (int i = 0; i < sessionCount; i++) {
        winrt::com_ptr<IAudioSessionControl> sessionControl;
        if (FAILED(sessionEnumerator->GetSession(i, sessionControl.put()))) {
            continue;
        }
        winrt::com_ptr<IAudioSessionControl2> sessionControl2;
        if (FAILED(sessionControl->QueryInterface(
                __uuidof(IAudioSessionControl2), sessionControl2.put_void()))) {
            continue;
        }
        if (sessionControl2->IsSystemSoundsSession() == S_OK) {
            continue;
        }
        DWORD sessionPID = 0;
        if (FAILED(sessionControl2->GetProcessId(&sessionPID))) {
            continue;
        }
        foundSession = true;
        if (!callback(sessionPID, sessionControl2.get())) {
            break;
        }
    }
    return foundSession;
}

std::wstring GetProcessCommandLine(HANDLE hProcess) {
    PROCESS_BASIC_INFORMATION pbi;
    ULONG returnLength;
    NTSTATUS status = NtQueryInformationProcess(
        hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), &returnLength);
    if (!NT_SUCCESS(status) || !pbi.PebBaseAddress) {
        return {};
    }

    PEB peb;
    if (!ReadProcessMemory(hProcess, pbi.PebBaseAddress, &peb, sizeof(peb),
                           nullptr)) {
        return {};
    }

    RTL_USER_PROCESS_PARAMETERS params;
    if (!ReadProcessMemory(hProcess, peb.ProcessParameters, &params,
                           sizeof(params), nullptr)) {
        return {};
    }

    std::wstring cmdLine(params.CommandLine.Length / sizeof(WCHAR), L'\0');
    if (!ReadProcessMemory(hProcess, params.CommandLine.Buffer, cmdLine.data(),
                           params.CommandLine.Length, nullptr)) {
        return {};
    }
    return cmdLine;
}

// Chromium-based browsers play audio from a utility subprocess.
DWORD FindChromiumAudioSubprocessUncached(DWORD parentPID) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    DWORD audioSubprocessPID = 0;
    std::vector<DWORD> rendererPIDs;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);

    if (Process32First(snapshot, &pe32)) {
        do {
            if (pe32.th32ParentProcessID != parentPID) {
                continue;
            }
            HANDLE hProcess =
                OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ,
                            FALSE, pe32.th32ProcessID);
            if (!hProcess) {
                continue;
            }
            std::wstring cmdLine = GetProcessCommandLine(hProcess);
            CloseHandle(hProcess);
            if (cmdLine.empty()) {
                continue;
            }
            int argc = 0;
            LPWSTR* argv = CommandLineToArgvW(cmdLine.c_str(), &argc);
            if (!argv) {
                continue;
            }
            bool isUtility = false;
            bool isAudioService = false;
            bool isRenderer = false;
            for (int i = 0; i < argc; i++) {
                if (wcscmp(argv[i], L"--type=utility") == 0) {
                    isUtility = true;
                } else if (wcscmp(argv[i],
                                  L"--utility-sub-type=audio.mojom."
                                  L"AudioService") == 0) {
                    isAudioService = true;
                } else if (wcscmp(argv[i], L"--type=renderer") == 0) {
                    isRenderer = true;
                }
            }
            LocalFree(argv);
            if (isUtility && isAudioService) {
                audioSubprocessPID = pe32.th32ProcessID;
                break;
            } else if (isRenderer) {
                rendererPIDs.push_back(pe32.th32ProcessID);
            }
        } while (Process32Next(snapshot, &pe32));
    }
    CloseHandle(snapshot);

    if (audioSubprocessPID) {
        return audioSubprocessPID;
    }
    if (rendererPIDs.empty()) {
        return 0;
    }
    DWORD rendererAudioPID = 0;
    ForEachAudioSession([&](DWORD sessionPID, IAudioSessionControl2*) {
        for (DWORD pid : rendererPIDs) {
            if (sessionPID == pid) {
                rendererAudioPID = pid;
                return false;
            }
        }
        return true;
    });
    return rendererAudioPID;
}

struct ChromiumAudioSubprocessCache {
    DWORD audioSubprocessPID;
    ULONGLONG timestamp;
};
std::unordered_map<DWORD, ChromiumAudioSubprocessCache>
    g_chromiumAudioSubprocessCache;
constexpr ULONGLONG kChromiumCacheTTL = 5000;

DWORD FindChromiumAudioSubprocess(DWORD parentPID) {
    ULONGLONG now = GetTickCount64();
    auto it = g_chromiumAudioSubprocessCache.find(parentPID);
    if (it != g_chromiumAudioSubprocessCache.end() &&
        now - it->second.timestamp < kChromiumCacheTTL) {
        return it->second.audioSubprocessPID;
    }
    DWORD audioSubprocessPID = FindChromiumAudioSubprocessUncached(parentPID);
    for (auto cacheIt = g_chromiumAudioSubprocessCache.begin();
         cacheIt != g_chromiumAudioSubprocessCache.end();) {
        if (now - cacheIt->second.timestamp >= kChromiumCacheTTL) {
            cacheIt = g_chromiumAudioSubprocessCache.erase(cacheIt);
        } else {
            ++cacheIt;
        }
    }
    g_chromiumAudioSubprocessCache[parentPID] = {audioSubprocessPID, now};
    return audioSubprocessPID;
}

struct AppVolumeResult {
    int volume;  // 0-100
    bool muted;
};

std::optional<AppVolumeResult> AdjustAppVolumeForPID(DWORD targetPID,
                                                     float fVolumeAdd,
                                                     bool noAutoMute) {
    std::optional<AppVolumeResult> result;
    ForEachAudioSession([&](DWORD pid, IAudioSessionControl2* sessionControl) {
        if (pid != targetPID) {
            return true;
        }
        winrt::com_ptr<ISimpleAudioVolume> vol;
        if (FAILED(sessionControl->QueryInterface(__uuidof(ISimpleAudioVolume),
                                                  vol.put_void()))) {
            return true;
        }
        float currentVolume = 0.0f;
        if (FAILED(vol->GetMasterVolume(&currentVolume))) {
            return true;
        }
        float newVolume = std::clamp(currentVolume + fVolumeAdd, 0.0f, 1.0f);
        if (SUCCEEDED(vol->SetMasterVolume(newVolume, NULL))) {
            AppVolumeResult r;
            r.volume = (int)(newVolume * 100.0f + 0.5f);
            if (!noAutoMute) {
                r.muted = newVolume < 0.005f;
                vol->SetMute(r.muted, NULL);
            } else {
                BOOL isMuted = FALSE;
                r.muted = SUCCEEDED(vol->GetMute(&isMuted)) && isMuted;
            }
            result = r;
        }
        return true;
    });
    return result;
}

std::optional<AppVolumeResult> AdjustAppVolume(DWORD targetPID,
                                               float fVolumeAdd,
                                               bool noAutoMute) {
    auto result = AdjustAppVolumeForPID(targetPID, fVolumeAdd, noAutoMute);
    if (result) {
        return result;
    }
    DWORD audioSubprocessPID = FindChromiumAudioSubprocess(targetPID);
    if (audioSubprocessPID) {
        result = AdjustAppVolumeForPID(audioSubprocessPID, fVolumeAdd,
                                       noAutoMute);
    }
    return result;
}

std::optional<AppVolumeResult> ToggleAppMuteForPID(DWORD targetPID) {
    std::optional<AppVolumeResult> result;
    ForEachAudioSession([&](DWORD pid, IAudioSessionControl2* sessionControl) {
        if (pid != targetPID) {
            return true;
        }
        winrt::com_ptr<ISimpleAudioVolume> vol;
        if (FAILED(sessionControl->QueryInterface(__uuidof(ISimpleAudioVolume),
                                                  vol.put_void()))) {
            return true;
        }
        BOOL isMuted = FALSE;
        if (FAILED(vol->GetMute(&isMuted))) {
            return true;
        }
        if (SUCCEEDED(vol->SetMute(!isMuted, NULL))) {
            AppVolumeResult r;
            r.muted = !isMuted;
            float currentVolume = 0.0f;
            r.volume = SUCCEEDED(vol->GetMasterVolume(&currentVolume))
                           ? (int)(currentVolume * 100.0f + 0.5f)
                           : 0;
            result = r;
        }
        return true;
    });
    return result;
}

std::optional<AppVolumeResult> ToggleAppMute(DWORD targetPID) {
    auto result = ToggleAppMuteForPID(targetPID);
    if (result) {
        return result;
    }
    DWORD audioSubprocessPID = FindChromiumAudioSubprocess(targetPID);
    if (audioSubprocessPID) {
        result = ToggleAppMuteForPID(audioSubprocessPID);
    }
    return result;
}

// --- Task button -> process ID (taskbar.dll / Taskbar.View.dll internals) ---

WCHAR g_clickSentinel[] = L"click-sentinel";
void* g_clickSentinel_TaskGroup = nullptr;
void* g_clickSentinel_TaskItem = nullptr;
thread_local bool g_captureTaskGroup;
thread_local void* g_capturedTaskGroup;

void* QueryViaVtable(void* object, void* vtable) {
    void* ptr = object;
    for (int i = 0; i < 64 && *(void**)ptr != vtable; i++) {
        ptr = (void**)ptr + 1;
    }
    return *(void**)ptr == vtable ? ptr : nullptr;
}

using CWindowTaskItem_GetWindow_t = HWND(WINAPI*)(void* pThis);
CWindowTaskItem_GetWindow_t CWindowTaskItem_GetWindow;

using CImmersiveTaskItem_GetAppWindow_t = HWND(WINAPI*)(void* pThis);
CImmersiveTaskItem_GetAppWindow_t CImmersiveTaskItem_GetAppWindow;

void* CImmersiveTaskItem_vftable;
void* CImmersiveTaskItem_vftable_ITaskItem;

using TryGetItemFromContainer_TaskListWindowViewModel_t =
    void*(WINAPI*)(void** output, wux::UIElement* container);
TryGetItemFromContainer_TaskListWindowViewModel_t
    TryGetItemFromContainer_TaskListWindowViewModel_Original;

using TaskListWindowViewModel_get_TaskItem_t = int(WINAPI*)(void* pThis,
                                                            void** taskItem);
TaskListWindowViewModel_get_TaskItem_t
    TaskListWindowViewModel_get_TaskItem_Original;

using TryGetItemFromContainer_TaskListGroupViewModel_t =
    void*(WINAPI*)(void** output, wux::UIElement* container);
TryGetItemFromContainer_TaskListGroupViewModel_t
    TryGetItemFromContainer_TaskListGroupViewModel_Original;

using TaskListGroupViewModel_IsMultiWindow_t = bool(WINAPI*)(void* pThis);
TaskListGroupViewModel_IsMultiWindow_t
    TaskListGroupViewModel_IsMultiWindow_Original;

using ITaskGroup_IsRunning_t = bool(WINAPI*)(void* pThis);
ITaskGroup_IsRunning_t ITaskGroup_IsRunning_Original;
bool WINAPI ITaskGroup_IsRunning_Hook(void* pThis) {
    if (g_captureTaskGroup) {
        g_capturedTaskGroup = *(void**)pThis;
        return false;
    }
    return ITaskGroup_IsRunning_Original(pThis);
}

using CTaskGroup_GetNumItems_t = int(WINAPI*)(void* pThis);
CTaskGroup_GetNumItems_t CTaskGroup_GetNumItems;

// Relies on CTaskGroup::GetNumItems being `return DPA_GetPtrCount(member)`;
// calling it on an array of pointers to ints reveals the member offset.
HDPA GetTaskItemsArray(void* taskGroup) {
    static size_t offset = []() {
        constexpr int kIntArraySize = 256;
        int arrayOfInts[kIntArraySize];
        int* arrayOfIntPtrs[kIntArraySize];
        for (int i = 0; i < kIntArraySize; i++) {
            arrayOfInts[i] = i;
            arrayOfIntPtrs[i] = &arrayOfInts[i];
        }
        return (size_t)CTaskGroup_GetNumItems(arrayOfIntPtrs);
    }();
    return (HDPA)((void**)taskGroup)[offset];
}

using CTaskListWnd_HandleClick_t = HRESULT(WINAPI*)(void* pThis,
                                                    void* taskGroup,
                                                    void* taskItem,
                                                    void** launcherOptions);
CTaskListWnd_HandleClick_t CTaskListWnd_HandleClick_Original;
HRESULT WINAPI CTaskListWnd_HandleClick_Hook(void* pThis,
                                             void* taskGroup,
                                             void* taskItem,
                                             void** launcherOptions) {
    if (launcherOptions && *launcherOptions == &g_clickSentinel) {
        g_clickSentinel_TaskGroup = taskGroup;
        g_clickSentinel_TaskItem = taskItem;
        return S_OK;
    }
    return CTaskListWnd_HandleClick_Original(pThis, taskGroup, taskItem,
                                             launcherOptions);
}

using TaskItem_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskItem_ReportClicked_t TaskItem_ReportClicked_Original;

using TaskGroup_ReportClicked_t = int(WINAPI*)(void* pThis, void* param);
TaskGroup_ReportClicked_t TaskGroup_ReportClicked_Original;

void* GetNativeTaskItemFromWindowsUdkTaskItem(void* windowsUdkTaskItem) {
    g_clickSentinel_TaskItem = nullptr;
    TaskItem_ReportClicked_Original(windowsUdkTaskItem, &g_clickSentinel);
    return g_clickSentinel_TaskItem;
}

void* GetNativeTaskGroupFromWindowsUdkTaskGroup(void* windowsUdkTaskGroup) {
    g_clickSentinel_TaskGroup = nullptr;
    TaskGroup_ReportClicked_Original(windowsUdkTaskGroup, &g_clickSentinel);
    return g_clickSentinel_TaskGroup;
}

winrt::com_ptr<IUnknown> GetWindowsUdkTaskItemFromTaskListButton(
    wux::UIElement element) {
    winrt::com_ptr<IUnknown> windowViewModel;
    TryGetItemFromContainer_TaskListWindowViewModel_Original(
        windowViewModel.put_void(), &element);
    if (!windowViewModel) {
        return nullptr;
    }
    winrt::com_ptr<IUnknown> windowsUdkTaskItem;
    TaskListWindowViewModel_get_TaskItem_Original(
        windowViewModel.get(), windowsUdkTaskItem.put_void());
    return windowsUdkTaskItem;
}

void* GetWindowsUdkTaskGroupFromTaskListButton(wux::UIElement element) {
    winrt::com_ptr<IUnknown> groupViewModel = nullptr;
    TryGetItemFromContainer_TaskListGroupViewModel_Original(
        groupViewModel.put_void(), &element);
    if (!groupViewModel) {
        return nullptr;
    }
    g_capturedTaskGroup = nullptr;
    g_captureTaskGroup = true;
    TaskListGroupViewModel_IsMultiWindow_Original((void**)groupViewModel.get() -
                                                  1);
    g_captureTaskGroup = false;
    return g_capturedTaskGroup;
}

HWND GetWindowFromTaskItem(void* taskItem) {
    if (!taskItem) {
        return nullptr;
    }
    if (*(void**)taskItem == CImmersiveTaskItem_vftable_ITaskItem) {
        void* immersiveTaskItem =
            QueryViaVtable(taskItem, CImmersiveTaskItem_vftable);
        return immersiveTaskItem
                   ? CImmersiveTaskItem_GetAppWindow(immersiveTaskItem)
                   : nullptr;
    }
    return CWindowTaskItem_GetWindow(taskItem);
}

DWORD GetProcessIdFromTaskListButton(wux::UIElement element) {
    auto windowsUdkTaskItem = GetWindowsUdkTaskItemFromTaskListButton(element);
    if (windowsUdkTaskItem) {
        void* nativeTaskItem =
            GetNativeTaskItemFromWindowsUdkTaskItem(windowsUdkTaskItem.get());
        if (HWND hWnd = GetWindowFromTaskItem(nativeTaskItem)) {
            DWORD processId = 0;
            GetWindowThreadProcessId(hWnd, &processId);
            return processId;
        }
    }

    void* windowsUdkTaskGroup =
        GetWindowsUdkTaskGroupFromTaskListButton(element);
    if (windowsUdkTaskGroup) {
        void* nativeTaskGroup =
            GetNativeTaskGroupFromWindowsUdkTaskGroup(windowsUdkTaskGroup);
        if (nativeTaskGroup) {
            HDPA taskItemsArray = GetTaskItemsArray(nativeTaskGroup);
            if (taskItemsArray && DPA_GetPtrCount(taskItemsArray) > 0) {
                void* taskItem = DPA_GetPtr(taskItemsArray, 0);
                if (HWND hWnd = GetWindowFromTaskItem(taskItem)) {
                    DWORD processId = 0;
                    GetWindowThreadProcessId(hWnd, &processId);
                    return processId;
                }
            }
        }
    }
    return 0;
}

// --- Tooltip (XAML popup, taskbar thread) ---

struct TooltipState {
    winrt::weak_ref<wux::FrameworkElement> taskbarFrame;
    bool vertical = false;
    // The cursor's monitor in XAML root coordinates.
    winrt::Windows::Foundation::Rect monitorRect{};
    double cursorX = 0.0;
    double cursorY = 0.0;
};

// The tooltip popup. Released on the taskbar thread by HideTooltip
// (CleanupOnOwnerThread), and never destroyed at DLL unload, which runs on
// another thread, where releasing a XAML object could crash Explorer.
[[clang::no_destroy]] wux::Controls::Primitives::Popup g_tooltipPopup{nullptr};
TooltipState g_tooltip;

// Set on the taskbar thread while the tooltip popup opens, which is when XAML
// creates the window of a windowed popup (see CreateWindowExW_Hook).
thread_local bool g_openingTooltipPopup;

bool IsWindowedPopupClass(LPCWSTR className, HINSTANCE hInstance) {
    constexpr WCHAR kWindowedPopupClassName[] = L"Xaml_WindowedPopupClass";
    if (!IS_INTRESOURCE(className)) {
        return _wcsicmp(className, kWindowedPopupClassName) == 0;
    }
    // GetClassInfoEx returns the class atom.
    WNDCLASSEX wc{};
    wc.cbSize = sizeof(wc);
    ATOM atom = (ATOM)GetClassInfoEx(hInstance, kWindowedPopupClassName, &wc);
    return atom && atom == (ATOM)(ULONG_PTR)className;
}

// Parses #RRGGBB or #AARRGGBB into 0xAARRGGBB.
std::optional<uint32_t> ParseHexColor(PCWSTR s) {
    size_t len = wcslen(s);
    if (s[0] != L'#' || (len != 7 && len != 9)) {
        return std::nullopt;
    }
    for (size_t i = 1; i < len; i++) {
        WCHAR c = s[i];
        bool hex = (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f') ||
                   (c >= L'A' && c <= L'F');
        if (!hex) {
            return std::nullopt;
        }
    }
    uint32_t v = (uint32_t)wcstoul(s + 1, nullptr, 16);
    if (len == 7) {
        v |= 0xFF000000;
    }
    return v;
}

struct TaskbarFrameAncestorResult {
    wux::FrameworkElement element = nullptr;
    bool isOverflowPopup = false;
};

TaskbarFrameAncestorResult FindTaskbarFrameAncestor(wux::UIElement element) {
    auto parent = wux::Media::VisualTreeHelper::GetParent(element);
    while (parent) {
        auto className = winrt::get_class_name(parent);
        if (className == L"Taskbar.TaskbarFrame") {
            return {parent.try_as<wux::FrameworkElement>(), false};
        }
        if (className == L"Taskbar.FlyoutFrame") {
            return {parent.try_as<wux::FrameworkElement>(), true};
        }
        parent = wux::Media::VisualTreeHelper::GetParent(parent);
    }
    return {};
}

constexpr int kTooltipOffset = 12;
constexpr int kTooltipOffsetTaskbarEdge = 4;

double GetRotationAngle(wux::UIElement element) {
    auto transform = element.RenderTransform();
    if (!transform) {
        return 0.0;
    }
    if (auto rotate = transform.try_as<wux::Media::RotateTransform>()) {
        return rotate.Angle();
    }
    if (auto group = transform.try_as<wux::Media::TransformGroup>()) {
        for (auto child : group.Children()) {
            if (auto rotate = child.try_as<wux::Media::RotateTransform>()) {
                return rotate.Angle();
            }
        }
    }
    return 0.0;
}

void UpdateTooltipPosition() {
    if (!g_tooltipPopup) {
        return;
    }
    auto popup = g_tooltipPopup;
    auto border = popup.Child().try_as<wux::FrameworkElement>();
    double tooltipWidth = border ? border.ActualWidth() : 0;
    double tooltipHeight = border ? border.ActualHeight() : 0;

    double cursorX = g_tooltip.cursorX;
    double cursorY = g_tooltip.cursorY;
    const auto& monitorRect = g_tooltip.monitorRect;
    double monitorRight = monitorRect.X + monitorRect.Width;

    auto taskbarFrame = g_tooltip.taskbarFrame.get();
    if (!taskbarFrame) {
        // Overflow flyout: right of the cursor, or left of it if there's no
        // room (e.g. a vertical taskbar docked to the right).
        double x = cursorX + kTooltipOffset;
        if (x + tooltipWidth > monitorRight) {
            x = cursorX - kTooltipOffset - tooltipWidth;
        }
        popup.HorizontalOffset(x);
        popup.VerticalOffset(cursorY - tooltipHeight / 2);
        return;
    }

    if (!g_tooltip.vertical) {
        // The frame may sit in a taller element with an invisible region
        // above it, so center on the frame itself.
        auto transform = taskbarFrame.TransformToVisual(nullptr);
        auto frameOrigin = transform.TransformPoint({0, 0});
        double taskbarHeight = taskbarFrame.ActualHeight();
        popup.HorizontalOffset(cursorX + kTooltipOffset);
        popup.VerticalOffset(
            frameOrigin.Y +
            std::fmax((taskbarHeight - tooltipHeight) / 2,
                      static_cast<double>(kTooltipOffsetTaskbarEdge)));
    } else {
        // At the screen edge the taskbar is docked to, extending over the
        // taskbar towards the screen center.
        bool dockedRight = cursorX > monitorRect.X + monitorRect.Width / 2;
        popup.HorizontalOffset(dockedRight ? monitorRight -
                                                 kTooltipOffsetTaskbarEdge -
                                                 tooltipWidth
                                           : monitorRect.X +
                                                 kTooltipOffsetTaskbarEdge);
        popup.VerticalOffset(cursorY + kTooltipOffset);
    }
}

// The cursor's monitor in XAML root coordinates, given the cursor's root
// coordinates.
winrt::Windows::Foundation::Rect GetCursorMonitorRect(
    double cursorX,
    double cursorY,
    double rasterizationScale) {
    POINT pt;
    GetCursorPos(&pt);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfo(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST),
                   &monitorInfo);
    const RECT& rc = monitorInfo.rcMonitor;
    if (rasterizationScale <= 0) {
        rasterizationScale = 1.0;
    }
    return {
        static_cast<float>(cursorX + (rc.left - pt.x) / rasterizationScale),
        static_cast<float>(cursorY + (rc.top - pt.y) / rasterizationScale),
        static_cast<float>((rc.right - rc.left) / rasterizationScale),
        static_cast<float>((rc.bottom - rc.top) / rasterizationScale),
    };
}

void ShowTooltip(wux::FrameworkElement taskbarFrame,
                 double cursorX,
                 double cursorY,
                 bool isOverflowPopup,
                 PCWSTR text,
                 const PerAppSettings& perApp) {
    auto xamlRoot = taskbarFrame ? taskbarFrame.XamlRoot() : nullptr;
    if (!xamlRoot) {
        return;
    }

    // The XAML root of a vertical taskbar is taller than it's wide, unless a
    // mod rotates the XAML root content to make the taskbar vertical.
    bool vertical = false;
    if (!isOverflowPopup) {
        auto xamlRootSize = xamlRoot.Size();
        auto xamlRootContent = xamlRoot.Content();
        vertical = xamlRootSize.Height > xamlRootSize.Width ||
                   (xamlRootContent && GetRotationAngle(xamlRootContent) != 0.0);
    }

    if (!g_tooltipPopup) {
        wux::Controls::Border border;
        border.IsHitTestVisible(false);
        border.Padding(wux::ThicknessHelper::FromLengths(12, 6, 12, 6));
        border.CornerRadius(wux::CornerRadiusHelper::FromUniformRadius(4));

        wux::Controls::TextBlock textBlock;
        textBlock.FontSize(12);
        border.Child(textBlock);

        wux::Controls::Primitives::Popup popup;
        popup.IsHitTestVisible(false);
        // A vertical taskbar is usually narrower than the tooltip.
        if (isOverflowPopup || vertical) {
            popup.ShouldConstrainToRootBounds(false);
        }
        popup.Child(border);
        border.SizeChanged([](auto&&, auto&&) { UpdateTooltipPosition(); });
        g_tooltipPopup = popup;
    }

    // Only the main taskbar keeps its frame (element-relative placement); a
    // null frame means placement next to the cursor.
    g_tooltip.taskbarFrame = isOverflowPopup ? nullptr : taskbarFrame;
    g_tooltip.vertical = vertical;
    g_tooltip.monitorRect =
        GetCursorMonitorRect(cursorX, cursorY, xamlRoot.RasterizationScale());
    g_tooltip.cursorX = cursorX;
    g_tooltip.cursorY = cursorY;

    auto popup = g_tooltipPopup;
    auto border = popup.Child().try_as<wux::Controls::Border>();
    if (!border) {
        return;
    }
    if (auto textBlock = border.Child().try_as<wux::Controls::TextBlock>()) {
        textBlock.Text(text);
        textBlock.Foreground(
            wux::Media::SolidColorBrush(winrt::Windows::UI::Colors::White()));
    }
    winrt::Windows::UI::Color bgColor;
    if (perApp.hasTooltipColor) {
        uint32_t c = perApp.tooltipColor;
        bgColor = winrt::Windows::UI::Color{
            static_cast<uint8_t>((c >> 24) & 0xFF),
            static_cast<uint8_t>((c >> 16) & 0xFF),
            static_cast<uint8_t>((c >> 8) & 0xFF),
            static_cast<uint8_t>(c & 0xFF),
        };
    } else {
        winrt::Windows::UI::ViewManagement::UISettings uiSettings;
        bgColor = uiSettings.GetColorValue(
            winrt::Windows::UI::ViewManagement::UIColorType::Accent);
    }
    border.Background(wux::Media::SolidColorBrush(bgColor));

    popup.XamlRoot(xamlRoot);
    UpdateTooltipPosition();
    g_openingTooltipPopup = true;
    popup.IsOpen(true);
    g_openingTooltipPopup = false;

    if (EnsureController()) {
        SetTimer(g_hController, Controller::kTimerPerAppTooltip, 1000,
                 nullptr);
    }
}

void OnTooltipHideTimer() {
    if (g_tooltipPopup) {
        g_tooltipPopup.IsOpen(false);
    }
}

// Closes and drops the popup (it is tied to a XamlRoot).
void HideTooltip() {
    HWND hCtrl = g_hController;
    if (hCtrl) {
        KillTimer(hCtrl, Controller::kTimerPerAppTooltip);
    }
    if (g_tooltipPopup) {
        g_tooltipPopup.IsOpen(false);
    }
    g_tooltipPopup = nullptr;
    g_tooltip.taskbarFrame = nullptr;
    g_tooltip.vertical = false;
    g_tooltip.monitorRect = {};
    g_tooltip.cursorX = 0.0;
    g_tooltip.cursorY = 0.0;
}

void ShowResultTooltip(wux::UIElement element,
                       wux::Input::PointerRoutedEventArgs args,
                       std::optional<AppVolumeResult> volumeResult,
                       const PerAppSettings& perApp) {
    auto [taskbarFrame, isOverflowPopup] = FindTaskbarFrameAncestor(element);
    if (!taskbarFrame) {
        return;
    }
    // Element-relative to XAML-root-relative coordinates.
    auto rootPoint = element.TransformToVisual(nullptr).TransformPoint(
        args.GetCurrentPoint(element).Position());

    bool terse = perApp.terseFormat;
    WCHAR tooltipText[64];
    if (!volumeResult) {
        wcscpy_s(tooltipText, terse ? L"\U0001F515" : L"No audio session");
    } else if (volumeResult->muted) {
        wcscpy_s(tooltipText, terse ? L"\U0001F507" : L"Muted");
    } else {
        swprintf_s(tooltipText, terse ? L"\U0001F50A %d%%" : L"Volume: %d%%",
                   volumeResult->volume);
    }
    ShowTooltip(taskbarFrame, rootPoint.X, rootPoint.Y, isOverflowPopup,
                tooltipText, perApp);
}

// --- Gating helpers ---

bool IsActiveFor(const Settings& cfg) {
    return g_active && !g_unloading && cfg.perApp.enabled;
}

// Taskbar thread, while handling the wheel/click message: the keys attached to
// that message, which stay correct while an admin window has the focus.
bool ModifiersMatch(const Settings& cfg) {
    return InputModifiers() == cfg.perApp.keys;
}

// Low-level hook thread: only the async state is available there.
bool AsyncModifiersMatch(const Settings& cfg) {
    return AsyncModifiers() == cfg.perApp.keys;
}

// How the wheel reaches per-app volume (same principle as m417z's original
// mod, which never intercepts the wheel): while the per-app modifier keys are
// held and the pointer is on the taskbar outside the notification area, the
// wheel is NOT handled by the scroll actions. It is passed on unchanged, so
// the taskbar delivers it to the element under the pointer: a task button
// runs TaskListButton_OnPointerWheelChanged_Hook (per-app volume), anything
// else ignores it. No hover tracking or position guessing is involved.
//
// Taskbar thread (InputSite wheel hook). pt is in the same coordinates as the
// taskbar window rects (the message's own coordinates).
bool ShouldRouteWheelToTaskButtons(HWND hTaskbarWnd, POINT pt) {
    auto cfg = Cfg();
    if (!IsActiveFor(*cfg) || !ModifiersMatch(*cfg)) {
        return false;
    }
    RECT rcTray;
    if (GetNotificationAreaRect(hTaskbarWnd, &rcTray) && PtInRect(&rcTray, pt)) {
        return false;  // the tray keeps the normal scroll actions
    }
    // A Win/Alt release after this must not open the Start menu / menu bar.
    MarkModifiersUsedForScroll(cfg->perApp.keys);
    return true;
}

// Low-level hook thread: a scroll-anywhere combo equal to the per-app keys is
// not swallowed on the taskbar (outside the tray), so per-app volume gets it.
// pt is physical; the hook thread is per-monitor-v2 DPI aware, so window rects
// are physical too.
bool ShouldYieldWheelLowLevel(POINT pt) {
    auto cfg = Cfg();
    if (!IsActiveFor(*cfg) || !AsyncModifiersMatch(*cfg)) {
        return false;
    }
    HWND hPointWnd = WindowFromPhysicalPoint(pt);
    HWND hRootWnd = hPointWnd ? GetAncestor(hPointWnd, GA_ROOT) : nullptr;
    if (!hRootWnd || !IsTaskbarWindow(hRootWnd)) {
        return false;
    }
    RECT rcTray;
    if (GetNotificationAreaRect(hRootWnd, &rcTray) && PtInRect(&rcTray, pt)) {
        return false;
    }
    return true;
}

// Per-app volume is enabled but its taskbar hooks couldn't be installed
// (usually: Windhawk failed to download the taskbar symbols). Instead of
// silently running the normal action (e.g. brightness) on modifier+scroll,
// say so on the indicator. The notification area keeps working normally.
bool ShowUnavailableIfRequested(HWND hTaskbarWnd, POINT pt) {
    auto cfg = Cfg();
    if (!cfg->perApp.enabled || g_active || g_unloading ||
        !cfg->perApp.keys.Any() || !ModifiersMatch(*cfg) ||
        g_nExplorerVersion < WIN_VERSION_11_21H2) {
        return false;
    }
    RECT rcTray;
    if (GetNotificationAreaRect(hTaskbarWnd, &rcTray) && PtInRect(&rcTray, pt)) {
        return false;
    }
    if (!EnsureController()) {
        return false;
    }
    Osd::Model m;
    m.icon = Osd::Icon::Warning;
    m.label = L"Per-app volume";
    m.value = L"unavailable";
    m.showBar = false;
    m.secondary = L"Taskbar symbols couldn't be loaded (see the mod log)";
    Osd::Show(m, MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST));
    return true;
}

// --- XAML hooks (TaskListButton, Taskbar.View.dll) ---

wux::UIElement GetTaskListButton(void* pThis) {
    wux::UIElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<wux::UIElement>(),
                         winrt::put_abi(element));
    if (!element || winrt::get_class_name(element) != L"Taskbar.TaskListButton") {
        return nullptr;
    }
    return element;
}

wux::Input::PointerRoutedEventArgs GetPointerArgs(void* pArgs) {
    wux::Input::PointerRoutedEventArgs args = nullptr;
    ((IUnknown*)pArgs)
        ->QueryInterface(winrt::guid_of<wux::Input::PointerRoutedEventArgs>(),
                         winrt::put_abi(args));
    return args;
}

using TaskListButton_PointerHandler_t = int(WINAPI*)(void* pThis, void* pArgs);

TaskListButton_PointerHandler_t TaskListButton_OnPointerWheelChanged_Original;
int WINAPI TaskListButton_OnPointerWheelChanged_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskListButton_OnPointerWheelChanged_Original(pThis, pArgs);
    };
    auto cfg = Cfg();
    if (!IsActiveFor(*cfg) || !ModifiersMatch(*cfg)) {
        return original();
    }
    auto element = GetTaskListButton(pThis);
    if (!element) {
        return original();
    }
    auto args = GetPointerArgs(pArgs);
    if (!args) {
        return original();
    }
    double delta = args.GetCurrentPoint(element).Properties().MouseWheelDelta();
    if (!delta) {
        return original();
    }
    DWORD processId = GetProcessIdFromTaskListButton(element);
    if (!processId) {
        Wh_Log(L"Per-app volume: no process for this task button");
        return original();
    }

    int step = cfg->perApp.volumeChangeStep > 0 ? cfg->perApp.volumeChangeStep
                                                : 2;
    float change = (float)delta * step * (0.01f / WHEEL_DELTA);
    auto result =
        AdjustAppVolume(processId, change, cfg->perApp.noAutomaticMuteToggle);
    Wh_Log(L"Per-app volume: PID %u, delta %.0f -> %d%%", processId, delta,
           result ? result->volume : -1);
    ShowResultTooltip(element, args, result, cfg->perApp);
    MarkModifiersUsedForScroll(cfg->perApp.keys);
    g_wheelHandled = true;
    args.Handled(true);
    return 0;
}

TaskListButton_PointerHandler_t TaskListButton_OnPointerExited_Original;
int WINAPI TaskListButton_OnPointerExited_Hook(void* pThis, void* pArgs) {
    if (GetTaskListButton(pThis)) {
        HideTooltip();
    }
    return TaskListButton_OnPointerExited_Original(pThis, pArgs);
}

// Only keeps the tooltip next to the pointer (as in the original mod).
TaskListButton_PointerHandler_t TaskListButton_OnPointerMoved_Original;
int WINAPI TaskListButton_OnPointerMoved_Hook(void* pThis, void* pArgs) {
    if (!g_tooltipPopup || !g_tooltipPopup.IsOpen()) {
        return TaskListButton_OnPointerMoved_Original(pThis, pArgs);
    }
    auto element = GetTaskListButton(pThis);
    if (element) {
        {
            if (auto args = GetPointerArgs(pArgs)) {
                auto frame = FindTaskbarFrameAncestor(element).element;
                if (frame) {
                    auto point = args.GetCurrentPoint(frame);
                    auto transform = frame.TransformToVisual(nullptr);
                    auto rootPoint = transform.TransformPoint(
                        {static_cast<float>(point.Position().X),
                         static_cast<float>(point.Position().Y)});
                    g_tooltip.cursorX = rootPoint.X;
                    g_tooltip.cursorY = rootPoint.Y;
                }
                UpdateTooltipPosition();
            }
        }
    }
    return TaskListButton_OnPointerMoved_Original(pThis, pArgs);
}

TaskListButton_PointerHandler_t TaskListButton_OnPointerPressed_Original;
int WINAPI TaskListButton_OnPointerPressed_Hook(void* pThis, void* pArgs) {
    auto original = [=]() {
        return TaskListButton_OnPointerPressed_Original(pThis, pArgs);
    };
    auto cfg = Cfg();
    // Modifier + click needs at least one modifier, otherwise it would replace
    // every click.
    bool modifierClick = cfg->perApp.clickToMute && cfg->perApp.keys.Any();
    if (!IsActiveFor(*cfg) ||
        (!modifierClick && !cfg->perApp.middleClickToMute)) {
        return original();
    }
    auto element = GetTaskListButton(pThis);
    if (!element) {
        return original();
    }
    auto args = GetPointerArgs(pArgs);
    if (!args) {
        return original();
    }
    auto properties = args.GetCurrentPoint(element).Properties();
    bool middle = properties.IsMiddleButtonPressed();
    bool left = properties.IsLeftButtonPressed();
    bool byModifier = modifierClick && left && ModifiersMatch(*cfg);
    bool byMiddle = cfg->perApp.middleClickToMute && middle;
    if (!byModifier && !byMiddle) {
        return original();
    }
    DWORD processId = GetProcessIdFromTaskListButton(element);
    if (!processId) {
        return original();
    }
    auto result = ToggleAppMute(processId);
    Wh_Log(L"Per-app mute toggled (%s): PID %u",
           byMiddle ? L"middle-click" : L"modifier + click", processId);
    ShowResultTooltip(element, args, result, cfg->perApp);
    if (byModifier) {
        MarkModifiersUsedForScroll(cfg->perApp.keys);
    }
    args.Handled(true);
    return 0;
}

bool g_taskbarDllHooked;

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Per-app volume: couldn't load taskbar.dll");
        return false;
    }
    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(public: virtual int __cdecl CTaskGroup::GetNumItems(void))"},
            &CTaskGroup_GetNumItems,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CWindowTaskItem::GetWindow(void))"},
            &CWindowTaskItem_GetWindow,
        },
        {
            {LR"(public: virtual struct HWND__ * __cdecl CImmersiveTaskItem::GetAppWindow(void))"},
            &CImmersiveTaskItem_GetAppWindow,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable')"},
            &CImmersiveTaskItem_vftable,
        },
        {
            {LR"(const CImmersiveTaskItem::`vftable'{for `ITaskItem'})"},
            &CImmersiveTaskItem_vftable_ITaskItem,
        },
        {
            {LR"(public: virtual long __cdecl CTaskListWnd::HandleClick(struct ITaskGroup *,struct ITaskItem *,struct winrt::Windows::System::LauncherOptions const &))"},
            &CTaskListWnd_HandleClick_Original,
            CTaskListWnd_HandleClick_Hook,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskItem,struct winrt::WindowsUdk::UI::Shell::ITaskItem>::ReportClicked(void *))"},
            &TaskItem_ReportClicked_Original,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::WindowsUdk::UI::Shell::implementation::TaskGroup,struct winrt::WindowsUdk::UI::Shell::ITaskGroup>::ReportClicked(void *))"},
            &TaskGroup_ReportClicked_Original,
        },
    };
    g_taskbarDllHooked =
        HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
    if (!g_taskbarDllHooked) {
        Wh_Log(L"Per-app volume disabled: taskbar.dll symbols not found");
    }
    return g_taskbarDllHooked;
}

// Appends the per-app volume hooks of Taskbar.View.dll / ExplorerExtensions.dll.
// They're optional so that a missing one doesn't fail the other hooks resolved
// in the same HookSymbols call; FinishTaskbarViewDllHooks checks them all.
void AppendTaskbarViewDllHooks(std::vector<COMBINED_SH>& hooks) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerWheelChanged(void *))"},
            &TaskListButton_OnPointerWheelChanged_Original,
            TaskListButton_OnPointerWheelChanged_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerExited(void *))"},
            &TaskListButton_OnPointerExited_Original,
            TaskListButton_OnPointerExited_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerMoved(void *))"},
            &TaskListButton_OnPointerMoved_Original,
            TaskListButton_OnPointerMoved_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerPressed(void *))"},
            &TaskListButton_OnPointerPressed_Original,
            TaskListButton_OnPointerPressed_Hook,
            true,
        },
        {
            {LR"(struct winrt::Taskbar::TaskListWindowViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListWindowViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskListWindowViewModel_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListWindowViewModel,struct winrt::Taskbar::ITaskListWindowViewModel>::get_TaskItem(void * *))"},
            &TaskListWindowViewModel_get_TaskItem_Original,
            nullptr,
            true,
        },
        {
            {LR"(struct winrt::Taskbar::TaskListGroupViewModel __cdecl TryGetItemFromContainer<struct winrt::Taskbar::TaskListGroupViewModel>(struct winrt::Windows::UI::Xaml::UIElement const &))"},
            &TryGetItemFromContainer_TaskListGroupViewModel_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::TaskListGroupViewModel::IsMultiWindow(void)const )"},
            &TaskListGroupViewModel_IsMultiWindow_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_Shell_ITaskGroup<struct winrt::WindowsUdk::UI::Shell::ITaskGroup>::IsRunning(void)const )"},
            &ITaskGroup_IsRunning_Original,
            ITaskGroup_IsRunning_Hook,
            true,
        },
    };
    for (auto& hook : symbolHooks) {
        hooks.push_back(std::move(hook));
    }
}

void FinishTaskbarViewDllHooks(bool symbolsLoaded) {
    if (!symbolsLoaded) {
        Wh_Log(L"Per-app volume disabled: couldn't load Taskbar.View.dll "
               L"symbols (Windhawk's symbol download failed, or this Windows "
               L"build renamed them)");
        return;
    }
    bool allResolved =
        g_taskbarDllHooked &&
        CTaskGroup_GetNumItems &&
        CWindowTaskItem_GetWindow &&
        CImmersiveTaskItem_GetAppWindow &&
        CImmersiveTaskItem_vftable &&
        CImmersiveTaskItem_vftable_ITaskItem &&
        TaskItem_ReportClicked_Original &&
        TaskGroup_ReportClicked_Original &&
        TryGetItemFromContainer_TaskListWindowViewModel_Original &&
        TaskListWindowViewModel_get_TaskItem_Original &&
        TryGetItemFromContainer_TaskListGroupViewModel_Original &&
        TaskListGroupViewModel_IsMultiWindow_Original &&
        TaskListButton_OnPointerWheelChanged_Original &&
        TaskListButton_OnPointerExited_Original &&
        TaskListButton_OnPointerMoved_Original &&
        TaskListButton_OnPointerPressed_Original &&
        ITaskGroup_IsRunning_Original;
    g_active = allResolved;
    Wh_Log(L"Per-app volume %s", allResolved ? L"ready" : L"disabled");
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}

bool IsWanted() {
    auto cfg = Cfg();
    return cfg->perApp.enabled && g_nExplorerVersion >= WIN_VERSION_11_21H2;
}

}  // namespace PerApp

// -----------------------------------------------------------------------------
// Scroll action dispatch (taskbar thread)
// -----------------------------------------------------------------------------

DWORD g_lastScrollTime;
int g_lastScrollDeltaRemainder;
DWORD g_lastActionTime;
int g_lastScrollActionIndex = -1;

void ShowMicOsd(POINT pt) {
    float v = GetCurrentMicVolumeScalar();
    if (v < 0.0f || !EnsureController()) {
        return;
    }
    Osd::Model m;
    m.icon = Osd::Icon::Mic;
    m.label = L"Microphone";
    m.level = (int)(v * 100.0f + 0.5f);
    m.value = Osd::FormatPercent(m.level);
    Osd::Show(m, MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST));
}

void InvokeScrollAction([[maybe_unused]] HWND hWnd,
                        WPARAM wParam,
                        LPARAM lMousePosParam,
                        int entryIndex,
                        bool suppressIndicator,
                        bool fromScrollAnywhere) {
    if (g_unloading) {
        return;
    }
    auto cfg = Cfg();
    if (entryIndex < 0 || entryIndex >= (int)cfg->scrollActions.size()) {
        return;
    }
    const auto& entry = cfg->scrollActions[entryIndex];

    // Don't carry a fractional wheel delta over to a different entry.
    if (entryIndex != g_lastScrollActionIndex) {
        g_lastScrollTime = 0;
        g_lastScrollDeltaRemainder = 0;
        g_lastActionTime = 0;
        g_lastScrollActionIndex = entryIndex;
    }

    int delta = GET_WHEEL_DELTA_WPARAM(wParam) * entry.scrollStep;
    if (entry.reverseScrollingDirection) {
        delta = -delta;
    }
    if (GetTickCount() - g_lastScrollTime < kWheelAccumulationWindowMs) {
        delta += g_lastScrollDeltaRemainder;
    }

    int clicks = delta / WHEEL_DELTA;
    Wh_Log(L"Entry %d: %d clicks (delta %d, suppress %d, anywhere %d)",
           entryIndex, clicks, delta, suppressIndicator, fromScrollAnywhere);

    if (clicks != 0 && entry.throttleMs > 0) {
        if (GetTickCount() - g_lastActionTime < (DWORD)entry.throttleMs) {
            clicks = 0;
            delta = 0;
        } else if (clicks < -1 || clicks > 1) {
            clicks = clicks > 0 ? 1 : -1;
            delta = 0;
        }
    }

    if (clicks != 0) {
        POINT pt = {GET_X_LPARAM(lMousePosParam), GET_Y_LPARAM(lMousePosParam)};
        bool showOsd = !suppressIndicator && cfg->customOsdEnabled;

        switch (entry.scrollAction) {
            case ScrollAction::virtualDesktopSwitch:
                SwitchDesktopViaKeyboardShortcut(clicks, fromScrollAnywhere);
                break;

            case ScrollAction::brightnessChange:
            case ScrollAction::sdrToHdrBrightnessChange:
            case ScrollAction::masterBrightnessChange:
                Bright::OnScroll(entry.scrollAction, pt, clicks, showOsd);
                break;

            case ScrollAction::micVolumeChange:
                // Windows has no microphone-level flyout, so the level is
                // changed directly and shown on our own indicator.
                if (AddMicMasterVolumeLevelScalar(clicks * 0.01f)) {
                    if (showOsd) {
                        ShowMicOsd(pt);
                    }
                } else {
                    Wh_Log(L"Error changing microphone volume");
                }
                break;

            case ScrollAction::volumeChange:
                if (suppressIndicator) {
                    AdjustVolumeLevelWithMouseWheel(clicks * WHEEL_DELTA);
                } else {
                    OpenScrollSndVol(
                        MAKEWPARAM(0, (WORD)(SHORT)(clicks * WHEEL_DELTA)),
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

int FindEntryIndexForModifiers(const Settings& cfg, const ModifierKeys& held);

// Taskbar thread, handling a wheel message over a taskbar.
bool OnMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    if (g_unloading || GetCapture()) {
        return false;
    }

    // Key checks below (and in the actions) use this message's key state.
    InputMessageScope inputScope;

    POINT pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);

    if (PerApp::ShowUnavailableIfRequested(hWnd, pt)) {
        return true;
    }

    auto cfg = Cfg();

    // Scroll-anywhere combos. Normally the low-level hook takes them before
    // the taskbar sees the wheel, but it can't see the keyboard while an admin
    // window has the focus. Here the message's key state is reliable. (A combo
    // equal to the per-app keys never gets here outside the notification
    // area: the InputSite hook routes it to the task buttons first.)
    int anywhereIdx = FindEntryIndexForModifiers(*cfg, InputModifiers());
    if (anywhereIdx >= 0) {
        MarkModifiersUsedForScroll(
            cfg->scrollActions[anywhereIdx].scrollAnywhereKeys);
        InvokeScrollAction(hWnd, wParam, lParam, anywhereIdx, false, true);
        return true;
    }

    for (int i = 0; i < (int)cfg->scrollActions.size(); i++) {
        const auto& entry = cfg->scrollActions[i];
        if (!IsPointInsideEntryScrollArea(hWnd, pt, entry)) {
            continue;
        }
        if (entry.scrollAction == ScrollAction::volumeChange &&
            cfg->ctrlScrollVolumeChange && GetKeyState(VK_CONTROL) >= 0) {
            continue;
        }

        // Allows to steal focus from elevated windows.
        INPUT input;
        ZeroMemory(&input, sizeof(INPUT));
        SendInput(1, &input, sizeof(INPUT));

        InvokeScrollAction(hWnd, wParam, lParam, i, false, false);
        return true;
    }
    return false;
}

LRESULT CALLBACK TaskbarWindowSubclassProc(_In_ HWND hWnd,
                                           _In_ UINT uMsg,
                                           _In_ WPARAM wParam,
                                           _In_ LPARAM lParam,
                                           _In_ [[maybe_unused]] DWORD_PTR
                                               dwRefData) {
    LRESULT result = 0;

    switch (uMsg) {
        case WM_COPYDATA: {
            // SndVol asks the volume icon rect for positioning; patch the
            // reply so that it respects our command-line position.
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
            if (result == 0 && p_copydata && p_copydata->dwData == 0x03 &&
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
                // From the low-level mouse hook, see LowLevelMouseProc.
                WORD lo = LOWORD(wParam);
                int entryIndex = (int)(lo >> 2);
                bool suppress = (lo & 1) != 0;
                bool fromScrollAnywhere = (lo & 2) != 0;
                short delta = (short)HIWORD(wParam);
                InvokeScrollAction(hWnd, MAKEWPARAM(0, (WORD)delta), lParam,
                                   entryIndex, suppress, fromScrollAnywhere);
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
        case WM_POINTERWHEEL: {
            HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
            if (!IsTaskbarWindow(hRootWnd) || g_unloading) {
                break;
            }
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            // Per-app volume: with its modifier keys held, pass the wheel on
            // unchanged so the task button under the pointer receives it.
            if (PerApp::ShouldRouteWheelToTaskButtons(hRootWnd, pt)) {
                PerApp::g_wheelHandled = false;
                LRESULT result =
                    InputSiteWindowProc_Original(hWnd, uMsg, wParam, lParam);
                Wh_Log(L"Per-app: wheel passed to the taskbar buttons (%s)",
                       PerApp::g_wheelHandled ? L"a task button handled it"
                                              : L"not over a task button");
                return result;
            }
            if (OnMouseWheel(hRootWnd, wParam, lParam)) {
                return 0;
            }
            break;
        }
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
    if (!hParentWnd || !IsTaskbarWindow(hParentWnd)) {
        return;
    }

    // Subclassing doesn't work here (inputsite.dll verifies the wndproc).
    auto wndProc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
    WindhawkUtils::SetFunctionHook(wndProc, InputSiteWindowProc_Hook,
                                   &InputSiteWindowProc_Original);
    if (g_initialized) {
        Wh_ApplyHookOperations();
    }
    Wh_Log(L"Hooked InputSite wndproc %p", wndProc);
    g_inputSiteProcHooked = true;
}

void HookInputSiteOfTaskbar(HWND hWnd) {
    if (g_nExplorerVersion < WIN_VERSION_11_21H2 || g_inputSiteProcHooked) {
        return;
    }
    HWND hXamlIslandWnd = FindWindowEx(
        hWnd, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge",
        nullptr);
    if (hXamlIslandWnd) {
        HWND hInputSiteWnd = FindWindowEx(
            hXamlIslandWnd, nullptr, L"Windows.UI.Input.InputSite.WindowClass",
            nullptr);
        if (hInputSiteWnd) {
            HandleIdentifiedInputSiteWindow(hInputSiteWnd);
        }
    }
}

bool IsMouseHookNeeded(const Settings& cfg);
void ScrollAnywhereThreadInit();

void HandleIdentifiedTaskbarWindow(HWND hWnd) {
    g_hTaskbarWnd = hWnd;
    g_dwTaskbarThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    SubclassTaskbarWindow(hWnd);
    for (HWND hSecondaryWnd : g_secondaryTaskbarWindows) {
        SubclassTaskbarWindow(hSecondaryWnd);
    }
    HookInputSiteOfTaskbar(hWnd);
    CreateControllerFromAnyThread();
    // The low-level hooks are only installed in the explorer.exe that has the
    // taskbar, not in Explorer's other processes (folder windows, COM servers).
    if (IsMouseHookNeeded(*Cfg())) {
        ScrollAnywhereThreadInit();
    }
}

void HandleIdentifiedSecondaryTaskbarWindow(HWND hWnd) {
    if (!g_dwTaskbarThreadId ||
        GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId) {
        return;
    }
    g_secondaryTaskbarWindows.insert(hWnd);
    SubclassTaskbarWindow(hWnd);
    HookInputSiteOfTaskbar(hWnd);
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

CreateWindowInBand_t CreateWindowInBand_Original;

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

// The window of the per-app volume tooltip, when XAML makes it a windowed
// popup (overflow flyout, vertical taskbar). Such a window takes mouse input
// regardless of the popup's IsHitTestVisible, and the tooltip follows the
// cursor, so it would take the pointer away from the task button (and the
// full-screen scrolling hook would see a non-taskbar window). It's made
// transparent to mouse input, and put in a band above thumbnail previews.
// From taskbar-volume-control-per-app 1.2 (m417z).
HWND CreateTooltipPopupWindow(DWORD dwExStyle,
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
    dwExStyle |= WS_EX_LAYERED | WS_EX_TRANSPARENT;
    // The export (possibly through CreateWindowInBand_Hook, which passes this
    // class through), as upstream does.
    static auto pCreateWindowInBand = (CreateWindowInBand_t)GetProcAddress(
        GetModuleHandle(L"user32.dll"), "CreateWindowInBand");
    HWND hWnd = nullptr;
    if (pCreateWindowInBand) {
        // A window can't be owned by a window of another band. Without an
        // owner, it's a tool window to stay off the taskbar and Alt+Tab.
        hWnd = pCreateWindowInBand(
            dwExStyle | WS_EX_TOOLWINDOW, lpClassName, lpWindowName, dwStyle, X,
            Y, nWidth, nHeight, nullptr, hMenu, hInstance, lpParam,
            kZbidSystemTools);
        if (!hWnd) {
            Wh_Log(L"Per-app tooltip: CreateWindowInBand failed (%lu)",
                   GetLastError());
        }
    }
    if (!hWnd) {
        hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                        dwStyle, X, Y, nWidth, nHeight,
                                        hWndParent, hMenu, hInstance, lpParam);
    }
    if (hWnd) {
        SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
    }
    return hWnd;
}

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
    if (PerApp::g_openingTooltipPopup &&
        PerApp::IsWindowedPopupClass(lpClassName, hInstance)) {
        return CreateTooltipPopupWindow(dwExStyle, lpClassName, lpWindowName,
                                        dwStyle, X, Y, nWidth, nHeight,
                                        hWndParent, hMenu, hInstance, lpParam);
    }
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd) {
        return hWnd;
    }
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
    if (!hWnd) {
        return hWnd;
    }
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
// SndVol.exe / ShellExperienceHost.exe: disable focus-based wheel routing
// -----------------------------------------------------------------------------

using ForceFocusBasedMouseWheelRouting_t = DWORD_PTR(WINAPI*)(BOOL enabled);
ForceFocusBasedMouseWheelRouting_t ForceFocusBasedMouseWheelRouting_Original;
DWORD_PTR WINAPI
ForceFocusBasedMouseWheelRouting_Hook([[maybe_unused]] BOOL enabled) {
    // Prevents the volume control from stealing mouse wheel messages.
    return ForceFocusBasedMouseWheelRouting_Original(FALSE);
}

// -----------------------------------------------------------------------------
// Middle-click the volume tray icon to mute (Windows 11 22H2+)
// -----------------------------------------------------------------------------

using VolumeSystemTrayIconDataModel_OnIconClicked_t =
    int(WINAPI*)(void* pThis, void* param1);
VolumeSystemTrayIconDataModel_OnIconClicked_t
    VolumeSystemTrayIconDataModel_OnIconClicked_Original;
int WINAPI VolumeSystemTrayIconDataModel_OnIconClicked_Hook(void* pThis,
                                                            void* param1) {
    if (Cfg()->middleClickToMute && GetKeyState(VK_MBUTTON) < 0) {
        ToggleVolMuted();
        return 0;
    }
    return VolumeSystemTrayIconDataModel_OnIconClicked_Original(pThis, param1);
}

void AppendSystemTrayHooks(HMODULE module,
                           std::vector<COMBINED_SH>& hooks) {
    // In SystemTray.dll the implementation was inlined into the produce<>
    // COM thunk, so hook that.
    bool isSystemTrayDll = module == GetModuleHandle(L"SystemTray.dll");
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
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
    for (auto& hook : symbolHooks) {
        hooks.push_back(std::move(hook));
    }
}

HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // From Taskbar.View.dll 2604.8002.200.6000 the SystemTray types
            // moved to SystemTray.dll.
            VS_FIXEDFILEINFO* fixedFileInfo =
                GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor =
                fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
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
    return g_nWinVersion >= WIN_VERSION_11_22H2 && Cfg()->middleClickToMute;
}

// Hooks the symbols that the per-app volume and the middle-click mute features
// need in this module, if it's one of theirs. Both can target the same module
// (Taskbar.View.dll before 2604, or ExplorerExtensions.dll), and each
// HookSymbols call resolves the module's symbols again, so all the hooks of a
// module are resolved in a single call. Each feature's module is handled once.
bool HookTaskbarModuleSymbols(HMODULE module) {
    const bool tray = !g_systemTrayModuleHooked &&
                      ShouldHookSystemTraySymbols() &&
                      GetSystemTrayModuleHandle() == module &&
                      !g_systemTrayModuleHooked.exchange(true);
    const bool perApp = !g_taskbarViewDllLoaded && PerApp::g_taskbarDllHooked &&
                        PerApp::IsWanted() &&
                        PerApp::GetTaskbarViewModuleHandle() == module &&
                        !g_taskbarViewDllLoaded.exchange(true);
    if (!tray && !perApp) {
        return false;
    }

    std::vector<COMBINED_SH> hooks;
    if (perApp) {
        PerApp::AppendTaskbarViewDllHooks(hooks);
    }
    if (tray) {
        AppendSystemTrayHooks(module, hooks);
    }
    const bool loaded =
        WindhawkUtils::HookSymbols(module, hooks.data(), hooks.size());
    if (perApp) {
        PerApp::FinishTaskbarViewDllHooks(loaded);
    }
    if (tray &&
        (!loaded || !VolumeSystemTrayIconDataModel_OnIconClicked_Original)) {
        Wh_Log(L"Middle-click to mute unavailable: symbol not found");
    }
    return loaded;
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
    if (!moduleFileName) {
        return false;
    }
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
    if (module && !((ULONG_PTR)module & 3) && IsExplorerPatcherModule(module)) {
        if (g_nExplorerVersion >= WIN_VERSION_11_21H2) {
            g_nExplorerVersion = WIN_VERSION_10_20H1;
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
        if (HookTaskbarModuleSymbols(module)) {
            Wh_Log(L"Loaded %s", lpLibFileName);
            Wh_ApplyHookOperations();
        }
    }
    return module;
}

// -----------------------------------------------------------------------------
// Low-level hooks: scroll anywhere, full-screen scrolling, Win/Alt masking
// -----------------------------------------------------------------------------

HANDLE g_scrollAnywhereThread;
// Guards g_scrollAnywhereThread: the thread is started from the taskbar thread
// (when the taskbar is found) and restarted from the engine thread (settings).
std::mutex g_scrollAnywhereThreadLock;

// The entry whose scroll-anywhere combo exactly matches held, or -1.
int FindEntryIndexForModifiers(const Settings& cfg, const ModifierKeys& held) {
    if (!held.Any()) {
        return -1;
    }
    for (int i = 0; i < (int)cfg.scrollActions.size(); i++) {
        const auto& keys = cfg.scrollActions[i].scrollAnywhereKeys;
        if (keys.Any() && keys == held) {
            return i;
        }
    }
    return -1;
}

// Low-level hook thread (async key state: blind while an admin window has the
// focus; OnMouseWheel covers the taskbar in that case).
int FindEntryIndexForCurrentModifiers(const Settings& cfg) {
    return FindEntryIndexForModifiers(cfg, AsyncModifiers());
}

// Low-level hook thread (full-screen scrolling). Same rules as OnMouseWheel,
// including "System volume on the taskbar requires Ctrl".
int FindEntryIndexAtPoint(const Settings& cfg, HWND hMMTaskbarWnd, POINT pt) {
    bool ctrlDown = GetAsyncKeyState(VK_CONTROL) < 0;
    for (int i = 0; i < (int)cfg.scrollActions.size(); i++) {
        const auto& entry = cfg.scrollActions[i];
        if (entry.scrollAction == ScrollAction::volumeChange &&
            cfg.ctrlScrollVolumeChange && !ctrlDown) {
            continue;
        }
        if (IsPointInsideEntryScrollArea(hMMTaskbarWnd, pt, entry)) {
            return i;
        }
    }
    return -1;
}

bool IsScrollAnywhereEnabled(const Settings& cfg) {
    for (const auto& entry : cfg.scrollActions) {
        if (entry.scrollAnywhereKeys.Any()) {
            return true;
        }
    }
    return false;
}

bool IsKeyboardHookNeeded(const Settings& cfg) {
    for (const auto& entry : cfg.scrollActions) {
        if (entry.scrollAnywhereKeys.win || entry.scrollAnywhereKeys.alt) {
            return true;
        }
    }
    return cfg.perApp.enabled && (cfg.perApp.keys.win || cfg.perApp.keys.alt);
}

bool IsMouseHookNeeded(const Settings& cfg) {
    return IsScrollAnywhereEnabled(cfg) ||
           cfg.fullScreenScrolling != FullScreenScrolling::disabled ||
           IsKeyboardHookNeeded(cfg);
}

// A lone Win or Alt tap opens the Start menu / menu bar. When the key was used
// for scrolling, its release is masked by injecting a dummy key (0xE8, as
// recommended by AutoHotkey) before replaying the key-up.
bool MaskModifierKeyEvent(ModifierMaskState* state,
                          WPARAM wParam,
                          KBDLLHOOKSTRUCT* pKbdStruct) {
    if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
        if (!state->down) {
            state->down = true;
            state->usedForScroll = false;
        }
    } else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
        state->down = false;
        if (state->usedForScroll && !(pKbdStruct->flags & LLKHF_INJECTED)) {
            state->usedForScroll = false;

            INPUT input[3] = {};
            input[0].type = INPUT_KEYBOARD;
            input[0].ki.wVk = 0xE8;
            input[0].ki.dwExtraInfo = kInjectedInputTag;
            input[1].type = INPUT_KEYBOARD;
            input[1].ki.wVk = 0xE8;
            input[1].ki.dwFlags = KEYEVENTF_KEYUP;
            input[1].ki.dwExtraInfo = kInjectedInputTag;
            input[2].type = INPUT_KEYBOARD;
            input[2].ki.wVk = (WORD)pKbdStruct->vkCode;
            input[2].ki.wScan = (WORD)pKbdStruct->scanCode;
            input[2].ki.dwFlags = KEYEVENTF_KEYUP;
            input[2].ki.dwExtraInfo = kInjectedInputTag;
            if (pKbdStruct->flags & LLKHF_EXTENDED) {
                input[2].ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
            }
            // Only swallow the original key-up if the whole sequence was
            // sent, otherwise the key would get stuck down.
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
    if (pKbdStruct->dwExtraInfo == kInjectedInputTag) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }
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

// Packs (entryIndex, suppressIndicator, fromScrollAnywhere) for
// g_dispatchEntryMsg.
WORD PackDispatchLoWord(int entryIndex, bool suppress, bool fromScrollAnywhere) {
    return (WORD)(((entryIndex & 0x3FFF) << 2) |
                  (fromScrollAnywhere ? 2 : 0) | (suppress ? 1 : 0));
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION || wParam != WM_MOUSEWHEEL || g_unloading) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }
    HWND hTaskbarWnd = g_hTaskbarWnd;
    if (!hTaskbarWnd) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    auto cfg = Cfg();
    MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;
    short wheelDelta = (short)HIWORD(pMouseStruct->mouseData);
    POINT pt = pMouseStruct->pt;

    // (1) Scroll anywhere: the entry whose combo exactly matches the held
    //     keys wins, regardless of the cursor position.
    int modEntryIdx = FindEntryIndexForCurrentModifiers(*cfg);
    if (modEntryIdx >= 0) {
        // Same combo as per-app volume and over a task button: let per-app
        // volume handle it.
        if (PerApp::ShouldYieldWheelLowLevel(pt)) {
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        }
        MarkModifiersUsedForScroll(
            cfg->scrollActions[modEntryIdx].scrollAnywhereKeys);
        PostMessage(hTaskbarWnd, g_dispatchEntryMsg,
                    MAKEWPARAM(PackDispatchLoWord(modEntryIdx, false, true),
                               (WORD)wheelDelta),
                    MAKELPARAM(pt.x, pt.y));
        return 1;
    }

    // (2) Full-screen scrolling: the cursor is in a taskbar zone but a
    //     full-screen window covers the taskbar.
    if (cfg->fullScreenScrolling != FullScreenScrolling::disabled) {
        HWND hPointWnd = WindowFromPoint(pt);
        if (hPointWnd) {
            HWND hRootWnd = GetAncestor(hPointWnd, GA_ROOT);
            if (hRootWnd && IsTaskbarWindow(hRootWnd)) {
                return CallNextHookEx(nullptr, nCode, wParam, lParam);
            }
        }

        int entryIdx = FindEntryIndexAtPoint(*cfg, hTaskbarWnd, pt);
        if (entryIdx < 0 && g_dwTaskbarThreadId) {
            struct EnumCtx {
                const Settings* cfg;
                POINT pt;
                int matchedIdx;
            } ctx{cfg.get(), pt, -1};
            EnumThreadWindows(
                g_dwTaskbarThreadId,
                [](HWND hWnd, LPARAM lParamCtx) WINAPI_LAMBDA_RETURN(BOOL) {
                    WCHAR szClassName[32];
                    if (GetClassName(hWnd, szClassName,
                                     ARRAYSIZE(szClassName)) &&
                        _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                        EnumCtx* c = (EnumCtx*)lParamCtx;
                        int idx = FindEntryIndexAtPoint(*c->cfg, hWnd, c->pt);
                        if (idx >= 0) {
                            c->matchedIdx = idx;
                            return FALSE;
                        }
                    }
                    return TRUE;
                },
                (LPARAM)&ctx);
            entryIdx = ctx.matchedIdx;
        }

        if (entryIdx >= 0) {
            bool suppress = cfg->fullScreenScrolling ==
                            FullScreenScrolling::withoutIndicator;
            PostMessage(hTaskbarWnd, g_dispatchEntryMsg,
                        MAKEWPARAM(PackDispatchLoWord(entryIdx, suppress, false),
                                   (WORD)wheelDelta),
                        MAKELPARAM(pt.x, pt.y));
            return 1;
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

DWORD WINAPI ScrollAnywhereThread(LPVOID lpParameter) {
    HANDLE hReadyEvent = (HANDLE)lpParameter;

    // Low-level hook coordinates are physical pixels; make window rects
    // (taskbar zones, notification area) physical on this thread too.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    g_winMaskState.Reset();
    g_altMaskState.Reset();

    HHOOK mouseHook =
        SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, nullptr, 0);
    HHOOK keyboardHook = nullptr;
    if (IsKeyboardHookNeeded(*Cfg())) {
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
        if (bRet == -1) {
            break;
        }
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
    std::lock_guard<std::mutex> lock(g_scrollAnywhereThreadLock);
    if (g_scrollAnywhereThread || g_unloading) {
        return;
    }
    HANDLE hReadyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!hReadyEvent) {
        return;
    }
    g_scrollAnywhereThread = CreateThread(nullptr, 0, ScrollAnywhereThread,
                                          hReadyEvent, 0, nullptr);
    if (g_scrollAnywhereThread) {
        WaitForSingleObject(hReadyEvent, INFINITE);
    }
    CloseHandle(hReadyEvent);
}

void ScrollAnywhereThreadUninit() {
    std::lock_guard<std::mutex> lock(g_scrollAnywhereThreadLock);
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

SettingsPtr LoadSettings() {
    auto s = std::make_shared<Settings>();

    for (int i = 0;; i++) {
        auto scrollAction = WindhawkUtils::StringSetting::make(
            L"ScrollActions[%d].scrollAction", i);
        auto scrollArea = WindhawkUtils::StringSetting::make(
            L"ScrollActions[%d].scrollArea", i);
        bool hasEntry = *scrollAction.get() || *scrollArea.get();
        if (!hasEntry) {
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

        entry.scrollArea = ScrollArea::taskbar;
        if (wcscmp(scrollArea, L"notificationArea") == 0) {
            entry.scrollArea = ScrollArea::notificationArea;
        } else if (wcscmp(scrollArea, L"taskbarWithoutNotificationArea") == 0) {
            entry.scrollArea = ScrollArea::taskbarWithoutNotificationArea;
        } else if (wcscmp(scrollArea, L"none") == 0) {
            entry.scrollArea = ScrollArea::none;
        }

        auto additionalScrollRegions = WindhawkUtils::StringSetting::make(
            L"ScrollActions[%d].additionalScrollRegions", i);
        for (auto regionStr : SplitStringView(additionalScrollRegions.get(), L",")) {
            regionStr = TrimStringView(regionStr);
            if (regionStr.empty()) {
                continue;
            }
            if (auto region = ParseRegion(regionStr)) {
                entry.additionalScrollRegions.push_back(*region);
            }
        }

        entry.scrollStep = std::clamp(
            Wh_GetIntSetting(L"ScrollActions[%d].scrollStep", i), 1, 100);
        entry.throttleMs =
            std::max(0, Wh_GetIntSetting(L"ScrollActions[%d].throttleMs", i));
        entry.reverseScrollingDirection = Wh_GetIntSetting(
            L"ScrollActions[%d].reverseScrollingDirection", i);
        entry.scrollAnywhereKeys.shift = Wh_GetIntSetting(
            L"ScrollActions[%d].scrollAnywhereKeys.shift", i);
        entry.scrollAnywhereKeys.ctrl = Wh_GetIntSetting(
            L"ScrollActions[%d].scrollAnywhereKeys.ctrl", i);
        entry.scrollAnywhereKeys.alt = Wh_GetIntSetting(
            L"ScrollActions[%d].scrollAnywhereKeys.alt", i);
        entry.scrollAnywhereKeys.win = Wh_GetIntSetting(
            L"ScrollActions[%d].scrollAnywhereKeys.win", i);

        s->scrollActions.push_back(std::move(entry));
    }

    auto method =
        WindhawkUtils::StringSetting::make(L"laptopBrightnessMethod");
    s->laptopMethod = LaptopMethod::Auto;
    if (wcscmp(method, L"ioctl") == 0) {
        s->laptopMethod = LaptopMethod::Ioctl;
    } else if (wcscmp(method, L"wmi") == 0) {
        s->laptopMethod = LaptopMethod::Wmi;
    } else if (wcscmp(method, L"powerScheme") == 0) {
        s->laptopMethod = LaptopMethod::PowerScheme;
    }

    s->darkOverlayEnabled = Wh_GetIntSetting(L"softwareDimmingEnabled");
    s->maxOverlayAlpha =
        std::clamp(Wh_GetIntSetting(L"maxOverlayAlpha"), 0, 255);
    auto curve = WindhawkUtils::StringSetting::make(L"darkOverlayCurve");
    s->darkCurve = wcscmp(curve, L"linear") == 0 ? DarkCurve::Linear
                                                  : DarkCurve::Perceptual;
    s->autoResetDarkOverlay = Wh_GetIntSetting(L"autoResetDarkOverlay");
    s->hideOverlayFromCapture = Wh_GetIntSetting(L"hideOverlayFromCapture");
    s->darkenCursor = Wh_GetIntSetting(L"darkenCursor");
    s->darkenIndicator = Wh_GetIntSetting(L"darkenIndicator");
    s->overlayAboveSystemUi = Wh_GetIntSetting(L"overlayAboveSystemUi");

    s->storeSdr = Wh_GetIntSetting(L"storeCurrentSdrToHdrBrightness");
    s->maxSdrBrightness =
        (double)std::clamp(Wh_GetIntSetting(L"maxSdrBrightness"), 2, 18);

    s->customOsdEnabled = Wh_GetIntSetting(L"customOsdEnabled");

    auto volumeIndicator =
        WindhawkUtils::StringSetting::make(L"volumeIndicator");
    s->volumeIndicator = VolumeIndicator::Win11;
    if (wcscmp(volumeIndicator, L"modern") == 0) {
        s->volumeIndicator = VolumeIndicator::Modern;
    } else if (wcscmp(volumeIndicator, L"classic") == 0) {
        s->volumeIndicator = VolumeIndicator::Classic;
    } else if (wcscmp(volumeIndicator, L"none") == 0) {
        s->volumeIndicator = VolumeIndicator::None;
    }

    s->middleClickToMute = Wh_GetIntSetting(L"middleClickToMute");
    s->ctrlScrollVolumeChange = Wh_GetIntSetting(L"ctrlScrollVolumeChange");
    s->noAutomaticMuteToggle = Wh_GetIntSetting(L"noAutomaticMuteToggle");
    // 0 (or an empty field) means the Windows default of 2.
    s->volumeChangeStep =
        std::clamp(Wh_GetIntSetting(L"volumeChangeStep"), 0, 100);
    if (s->volumeChangeStep == 0) {
        s->volumeChangeStep = 2;
    }

    auto fullScreenScrolling =
        WindhawkUtils::StringSetting::make(L"fullScreenScrolling");
    s->fullScreenScrolling = FullScreenScrolling::disabled;
    if (wcscmp(fullScreenScrolling, L"withIndicator") == 0) {
        s->fullScreenScrolling = FullScreenScrolling::withIndicator;
    } else if (wcscmp(fullScreenScrolling, L"withoutIndicator") == 0) {
        s->fullScreenScrolling = FullScreenScrolling::withoutIndicator;
    }

    s->perApp.enabled = Wh_GetIntSetting(L"perAppVolume.enabled");
    s->perApp.keys.shift = Wh_GetIntSetting(L"perAppVolume.modifierKeys.shift");
    s->perApp.keys.ctrl = Wh_GetIntSetting(L"perAppVolume.modifierKeys.ctrl");
    s->perApp.keys.alt = Wh_GetIntSetting(L"perAppVolume.modifierKeys.alt");
    s->perApp.keys.win = Wh_GetIntSetting(L"perAppVolume.modifierKeys.win");
    s->perApp.volumeChangeStep =
        std::clamp(Wh_GetIntSetting(L"perAppVolume.volumeChangeStep"), 1, 50);
    s->perApp.clickToMute = Wh_GetIntSetting(L"perAppVolume.modifierClickToMute");
    s->perApp.middleClickToMute =
        Wh_GetIntSetting(L"perAppVolume.middleClickToMute");
    s->perApp.terseFormat = Wh_GetIntSetting(L"perAppVolume.terseFormat");
    s->perApp.noAutomaticMuteToggle =
        Wh_GetIntSetting(L"perAppVolume.noAutomaticMuteToggle");
    auto tooltipColor =
        WindhawkUtils::StringSetting::make(L"perAppVolume.tooltipColor");
    if (auto color = PerApp::ParseHexColor(tooltipColor.get())) {
        s->perApp.hasTooltipColor = true;
        s->perApp.tooltipColor = *color;
    } else if (*tooltipColor.get()) {
        Wh_Log(L"Invalid tooltip color (expected #RRGGBB or #AARRGGBB): %s",
               tooltipColor.get());
    }

    s->debugOsd = Wh_GetIntSetting(L"debugOsd");
    s->oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");

    if (s->perApp.enabled) {
        for (const auto& e : s->scrollActions) {
            if (e.scrollAnywhereKeys.Any() &&
                e.scrollAnywhereKeys == s->perApp.keys) {
                Wh_Log(L"Note: a scroll-anywhere combo equals the per-app "
                       L"modifier; per-app volume wins over task buttons");
            }
        }
    }
    return s;
}

// -----------------------------------------------------------------------------
// Mod init / uninit
// -----------------------------------------------------------------------------

// ShellExperienceHost.exe / SndVol.exe: only the focus-wheel-routing hook is
// needed, and only for the matching indicator (as in Taskbar Volume Control).
bool HelperProcessNeedsHook(const Settings& cfg) {
    if (!cfg.HasAction(ScrollAction::volumeChange)) {
        return false;
    }
    if (cfg.volumeChangeStep == 2 && !cfg.noAutomaticMuteToggle) {
        return false;
    }
    if (g_target == Target::ShellExperienceHost) {
        return cfg.volumeIndicator == VolumeIndicator::Modern &&
               CanUseModernIndicator();
    }
    return cfg.volumeIndicator == VolumeIndicator::Classic ||
           (cfg.volumeIndicator == VolumeIndicator::Modern &&
            !CanUseModernIndicator());
}

void CleanupOnOwnerThread() {
    // On the taskbar thread, where WM_NCDESTROY also erases from
    // g_secondaryTaskbarWindows, so the set isn't changed while it's iterated.
    if (g_hTaskbarWnd) {
        UnsubclassTaskbarWindow(g_hTaskbarWnd);
        for (HWND hSecondaryWnd : g_secondaryTaskbarWindows) {
            UnsubclassTaskbarWindow(hSecondaryWnd);
        }
    }
    Bright::FlushSdrSaves();
    Overlay::DestroyAll();
    Osd::Destroy();
    PerApp::HideTooltip();
    CleanupSndVol();
    HWND h = g_hController.exchange(nullptr);
    if (h) {
        DestroyWindow(h);
    }
    Controller::UnregisterClasses();
    AudioUninit();
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    SetCfg(LoadSettings());

    if (!WindowsVersionInit()) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    g_target = Target::Explorer;
    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(nullptr, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            Wh_Log(L"GetModuleFileName failed");
            return FALSE;
        default:
            if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                moduleFileName++;
                if (_wcsicmp(moduleFileName, L"ShellExperienceHost.exe") == 0) {
                    g_target = Target::ShellExperienceHost;
                } else if (_wcsicmp(moduleFileName, L"SndVol.exe") == 0) {
                    g_target = Target::SndVol;
                } else if (_wcsicmp(moduleFileName, L"explorer.exe") != 0) {
                    // E.g. dwm.exe added to the process inclusion list. The
                    // taskbar logic (hooks, brightness worker, mouse hook)
                    // must only run in Explorer, and nothing of this mod is
                    // needed anywhere else: the Dark Overlay windows, their
                    // band and the darkened pointer all come from Explorer.
                    Wh_Log(L"%s: nothing to do in this process, not loading",
                           moduleFileName);
                    return FALSE;
                }
            } else {
                Wh_Log(L"GetModuleFileName returned an unsupported path");
                return FALSE;
            }
            break;
    }

    if (g_target == Target::ShellExperienceHost || g_target == Target::SndVol) {
        if (!HelperProcessNeedsHook(*Cfg())) {
            return FALSE;
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

    g_nExplorerVersion = g_nWinVersion;
    if (g_nExplorerVersion >= WIN_VERSION_11_21H2 && Cfg()->oldTaskbarOnWin11) {
        g_nExplorerVersion = WIN_VERSION_10_20H1;
    }

    if (PerApp::IsWanted()) {
        PerApp::HookTaskbarDllSymbols();
    }
    // The modules that are already loaded. When one module serves both
    // features, the second call finds it handled.
    for (HMODULE module :
         {GetSystemTrayModuleHandle(), PerApp::GetTaskbarViewModuleHandle()}) {
        if (module) {
            HookTaskbarModuleSymbols(module);
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

    Worker::Start();

    g_initialized = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    if (g_target != Target::Explorer) {
        return;
    }

    // Modules loaded since Wh_ModInit, in case of a race with the
    // LoadLibraryExW hook.
    bool hooked = false;
    for (HMODULE module :
         {GetSystemTrayModuleHandle(), PerApp::GetTaskbarViewModuleHandle()}) {
        if (module && HookTaskbarModuleSymbols(module)) {
            hooked = true;
        }
    }
    if (hooked) {
        Wh_ApplyHookOperations();
    }

    // Try again in case of a race with the LoadLibraryExW hook.
    HandleLoadedExplorerPatcher();

    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(NULL), L"Shell_TrayWnd", &wndclass)) {
        HWND hWnd =
            FindCurrentProcessTaskbarWindows(&g_secondaryTaskbarWindows);
        if (hWnd) {
            HandleIdentifiedTaskbarWindow(hWnd);
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
    if (g_target != Target::Explorer) {
        return;
    }

    g_unloading = true;
    ScrollAnywhereThreadUninit();

    // Unsubclass the taskbar windows and destroy windows/timers/COM objects on
    // the thread that owns them. Run it through the taskbar window (the
    // controller destroys itself in there).
    // This runs first: once it returns, the taskbar thread is outside this
    // mod's code and, with g_unloading set, never re-enters the brightness
    // code, so nothing can hand work to the worker while it stops.
    HWND hTarget = g_hTaskbarWnd;
    if (!hTarget || !IsWindow(hTarget)) {
        hTarget = g_hController;
    }
    if (hTarget && IsWindow(hTarget)) {
        RunFromWindowThread(
            hTarget, [](void*) { CleanupOnOwnerThread(); }, nullptr);
    }

    // Puts the user's pointers back if they are darkened.
    CursorDim::Stop();

    // Results it still posts are dropped (no controller any more).
    Worker::Stop();

    // Only if the mod changed the SDR brightness of a screen.
    Sdr::RestoreSettingsLevels();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    SettingsPtr prev = Cfg();
    SettingsPtr next = LoadSettings();

    if (g_target == Target::ShellExperienceHost || g_target == Target::SndVol) {
        // Helper processes: re-evaluate from scratch (Wh_ModInit decides
        // whether the hook is still needed).
        *bReload = TRUE;
        SetCfg(next);
        return TRUE;
    }

    *bReload = prev->oldTaskbarOnWin11 != next->oldTaskbarOnWin11 ||
               prev->middleClickToMute != next->middleClickToMute ||
               prev->perApp.enabled != next->perApp.enabled;
    if (*bReload) {
        SetCfg(next);
        return TRUE;
    }

    // Stop the hook thread before swapping settings, then restart it with the
    // new configuration (it decides which hooks it needs at startup).
    ScrollAnywhereThreadUninit();
    SetCfg(next);
    if (g_hTaskbarWnd && IsMouseHookNeeded(*next)) {
        ScrollAnywhereThreadInit();
    }

    Worker::Wake();
    HWND hCtrl = g_hController;
    if (hCtrl) {
        PostMessage(hCtrl, Controller::g_msgApplySettings, 0, 0);
    }

    Wh_Log(L"Settings changed");
    return TRUE;
}

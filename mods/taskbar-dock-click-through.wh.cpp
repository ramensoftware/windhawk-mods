// ==WindhawkMod==
// @id              taskbar-dock-click-through
// @name            Taskbar Dock Click-Through
// @description     Keeps floating taskbar docks click-through and adds dock-aware auto-hide when Windows auto-hide is enabled.
// @version         1.0.0
// @author          primez-x
// @github          https://github.com/primez-x
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lgdi32 -lshell32 -lshcore -ldwmapi
// @license         GPL-3.0-only
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# Taskbar Dock Click-Through

Companion to **Windows 11 Taskbar Styler**. When a theme shrinks the taskbar
into a centered or floating dock, the native taskbar window still spans the
screen edge. This mod clips its input region to the actual visible dock so
clicks in the empty strip reach the desktop or window underneath.

Windows **Automatically hide the taskbar** setting is the master auto-hide
switch. When it is on, the selected dock-aware rule controls when the taskbar
hides. When it is off, every auto-hide feature in this mod is inactive and only
click-through remains enabled. The mod never changes the Windows setting. It
does no styling and does not replace Taskbar Styler. Disabling the mod returns
the taskbar input region to full width.

## How it works
The worker takes one cached UI Automation subtree snapshot per taskbar, in
physical pixels, and groups visible taskbar and system-tray content into small
clickable islands. Windows accessibility events are delivered asynchronously
to the worker and normally drive refreshes. Possible additions immediately
queue a full clickable region for the affected taskbar before it is measured
again. Scans are deferred while the pointer is interacting with a taskbar,
debounced for 25 ms, and a subsequent shrink requires matching geometry from a
fresh scan after the 110 ms settling window. A low-frequency fallback scan
covers missing events. Ambiguous snapshots fail safe to a fully clickable
taskbar.

When Windows auto-hide is enabled, the mod uses the same last-good dock islands
for overlap decisions and screen-edge reveal hit testing. The selected rule
decides whether native hiding is allowed, while Windows remains the only
component that moves and renders the taskbar. This preserves the taskbar's
native DPI handling, animation state, and XAML hover behavior. Geometry is
retained while the taskbar is hidden. If a taskbar starts hidden before its
dock can be measured, a centered startup band remains available at the monitor
edge until the first reliable dock snapshot is captured.

## Related work and attribution

The auto-hide implementation adapts prior Windhawk work by m417z on
[overlap and maximized-window policy](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-auto-hide-when-maximized.wh.cpp),
[screen-edge activation areas](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-auto-hide-custom-activation-area.wh.cpp),
[native animation timing](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-auto-hide-speed.wh.cpp),
and [native taskbar state coordination](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-auto-hide-keyboard-only.wh.cpp),
plus Cirn09's
[notification recovery](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-autohide-better.wh.cpp)
and Bo0ii's
[hide/show timing and animation](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-autohide-instant-show.wh.cpp).

This integration is intentional. The referenced mods evaluate the native
taskbar window or apply independent policy at the same hide, show, and reveal
paths, while this mod must coordinate those decisions around its measured dock
islands. Running the policies separately would make empty parts of the
full-width taskbar window participate in overlap or reveal decisions and would
make shared hooks order-dependent. Windows remains responsible for moving and
rendering the taskbar.

## Notes
- Designed for a centered or floating taskbar. With a normal full-width
  taskbar, the input region spans the full bar and has no visible effect.
- `taskbar-dock-animation-plus` is supported. This mod never changes the
  taskbar's XAML transforms and ignores its transient icon-hover movement.
- Disable `taskbar-autohide-better`,
  `taskbar-auto-hide-when-maximized`, and
  `taskbar-autohide-instant-show`, plus
  `taskbar-auto-hide-keyboard-only`, `taskbar-auto-hide-per-monitor`, and
  `taskbar-auto-hide-custom-activation-area`, and
  `taskbar-auto-hide-speed`. Those mods hook or mutate the same native paths
  and must not run at the same time.
- If Windows changes a private taskbar symbol used by the dock-aware auto-hide
  rules, click-through remains active and Windows continues using its normal
  auto-hide behavior. The Windhawk log identifies the unavailable hook.
- Windows exposes one native taskbar auto-hide state. By default the selected
  rule applies to every taskbar; an optional primary-monitor-only setting
  leaves secondary taskbars under normal Windows auto-hide behavior.
- Windows 11's native taskbar is supported. Restored Windows 10 taskbars and
  third-party taskbar replacements are outside this mod's supported scope.
- UI Automation and cross-thread taskbar calls are time-bounded during normal
  operation. Unloading waits until every taskbar callback is detached, so
  Explorer never calls code from an unloaded module.
- Normal hide/show transitions retain the last valid shown dock geometry.
  Display-topology and DPI changes discard that geometry immediately so a
  removed monitor cannot leave stale clipping or auto-hide state behind. The
  dock is measured again after Windows lays it out on the current monitor.
- Mouse-edge reveal stays suppressed for a foreground borderless fullscreen
  window. Normal maximized applications still reveal the taskbar, and the
  Windows key keeps its native behavior.

## Changelog

### 1.0.0

- Initial public release with dock-island click-through, dock-aware native
  auto-hide, multi-monitor and mixed-DPI handling, notification recovery, and
  fullscreen-aware edge reveal.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- fallbackScanMs: 5000
  $name: Fallback refresh interval (ms)
  $description: >-
    Sets how often the add-on checks the taskbar layout when Windows does not
    report a change. Windows normally reports changes as they happen, so this
    is a backup interval rather than the main refresh rate. Default: 5000
    milliseconds. Range: 2000-15000 milliseconds. Lower values may increase
    CPU usage.
- horizontalPaddingLeft: 0
  $name: Clickable padding, left edge (px)
  $description: >-
    Widens the clickable area at the outer left edge of the taskbar dock. Only
    that one edge is affected. When the dock is split into separate islands,
    the inner edges facing each other are left alone, because a click between
    them already lands on one island or the other. Unlike "Unmeasurable element
    fallback width", this is applied once, at the edge, even when every taskbar
    item reports its size correctly. Use it when the left end of the dock feels
    too narrow to click. Default: 0 (disabled). Range: 0-600 pixels. DPI-scaled
    to match the display.
- horizontalPaddingRight: 0
  $name: Clickable padding, right edge (px)
  $description: >-
    Widens the clickable area at the outer right edge of the taskbar dock,
    independently of the left edge. This is the usual fix when the trailing end
    of the dock, such as the clock or the last tray icon, has a sliver that
    passes clicks through to the window behind the taskbar instead of
    responding. The two edges are set separately so that correcting one does
    not add unwanted padding to the other. Default: 0 (disabled). Range: 0-600
    pixels. DPI-scaled to match the display.
- ghostAllowance: 0
  $name: Unmeasurable element fallback width (px)
  $description: >-
    Sets a fallback width for taskbar items that do not report their size.
    Unlike the clickable edge padding settings, this is applied separately to
    each affected item. The add-on determines clickable areas using the item
    widths reported by Windows. If a visible item, such as the clock, does not
    report its width, it may be mistaken for empty space and pass clicks
    through to the window behind the taskbar. On a single dock, this issue is
    most likely near the outer edges, and the result is similar to adjusting
    the clickable edge padding. This setting is most useful for fixing
    unclickable areas within split-dock layouts, where the center taskbar
    island reports width correctly but the side notifications / clock island
    does not. Default: 0 (disabled). Range: 0-600 pixels. DPI-scaled to match
    the display.
- maxContentWidthPercent: 90
  $name: Background element width cutoff (%)
  $description: >-
    Prevents a taskbar-wide background panel from being mistaken for the
    visible dock. Windows can report a large background element that spans
    most or all of the taskbar. If the add-on treats it as dock content, empty
    space around a floating dock captures clicks instead of passing them
    through. A reported element is excluded when it covers at least this
    percentage of the taskbar width. Default: 90 percent. Lower this value only
    if a wide background area is still blocking clicks outside the dock. Raise
    it if legitimate wide dock content is being excluded. Set to 100 percent
    to disable this protection. Range: 10-100 percent.
- islandGapPx: 120
  $name: Dock island separation gap (px)
  $description: >-
    Sets the largest empty horizontal gap that remains part of one dock island.
    A gap larger than this value starts a separate clickable island. Lower
    values separate nearby groups more easily; higher values combine them.
    Default: 120 pixels. Range: 8-4000 physical screen pixels.
- autoHideMode: overlap
  $name: When to hide the dock
  $description: >-
    Chooses the condition that hides the dock while Windows "Automatically
    hide the taskbar" is enabled. When the Windows setting is off, Windows
    keeps the taskbar visible and this add-on's auto-hide enhancements do not
    run; click-through continues to work. To keep the dock visible, turn off
    the Windows setting. The add-on never changes it. The selection applies to
    every taskbar unless "Enhance auto-hide on the primary monitor only" is
    enabled. Default: When a window covers the dock.
  $options:
    - overlap: When a window covers the dock
    - maximized: When a window is maximized
    - always: Whenever the pointer leaves the dock
- foregroundOnly: false
  $name: Foreground window only
  $description: >-
    Controls which windows are checked by "When a window covers the dock" and
    "When a window is maximized". When enabled, only the foreground window can
    hide the dock. When disabled, any visible eligible window can hide it. It
    does not affect "Whenever the pointer leaves the dock" and is ignored
    while Windows auto-hide is off. Default: Disabled, so any visible eligible
    window occupying the dock space can hide it.
- excludedPrograms: [""]
  $name: Programs that do not hide the dock
  $description: >-
    Lists programs that the overlap and maximized rules ignore. Enter a file
    name such as "notepad.exe", a complete executable path, or an application
    ID. This is useful for desktop widgets, launchers, or other windows that
    may touch the dock but should not make it hide. One entry is checked
    against each visible window. Leave the list empty to include every
    eligible program. This setting does not affect "Whenever the pointer
    leaves the dock" and is ignored while Windows auto-hide is off. Default:
    Empty.
- primaryMonitorOnly: false
  $name: Enhance auto-hide on the primary monitor only
  $description: >-
    Applies the selected dock-aware rule only to the primary monitor.
    Secondary taskbars continue using normal Windows auto-hide behavior. When
    disabled, the selected rule applies independently to every taskbar.
    Windows "Automatically hide the taskbar" must still be enabled. Default:
    Disabled.
- edgeReveal: true
  $name: Reveal from the monitor edge
  $description: >-
    Controls whether a hidden taskbar appears when the pointer reaches the
    monitor edge next to a known dock island. Empty parts of that monitor edge
    do not reveal the taskbar. If Windows starts with a taskbar already hidden,
    a centered startup band remains available until the add-on can measure that
    dock for the first time. This setting is ignored while Windows auto-hide is
    off. Default: Enabled.
- showDelayMs: 40
  $name: Show delay (ms)
  $description: >-
    Sets how long a show condition must remain active before the taskbar begins
    to appear. A show condition occurs when the selected hide condition clears
    or edge reveal is active. Increase the value to reduce accidental reveals.
    This setting is ignored while Windows auto-hide is off. Default: 40
    milliseconds. Range: 0-2000 milliseconds.
- hideDelayMs: 350
  $name: Hide delay (ms)
  $description: >-
    Sets how long the selected hide condition must remain active before the
    taskbar begins to disappear. Increase this value to keep brief window or
    pointer movements from hiding the dock. This setting is ignored while
    Windows auto-hide is off. Default: 350 milliseconds. Range: 0-5000
    milliseconds.
- notificationHoldMs: 1500
  $name: Notification visibility time (ms)
  $description: >-
    Sets how long a taskbar notification may keep the taskbar visible before
    the selected hide rule can hide it again. Without this recovery, an
    inactive application's notification can leave an auto-hidden taskbar
    visible until that application is opened. Repeated attention signals from
    the same ongoing notification do not restart this timer. Set to 0 to let
    the dock hide immediately after a notification. This setting is ignored
    while Windows auto-hide is off. Default: 1500 milliseconds. Range:
    0-10000 milliseconds.
- animation: native
  $name: Auto-hide animation
  $description: >-
    Chooses whether Windows animates the taskbar window while showing or
    hiding it. "Native" uses Windows' normal taskbar movement. "None" asks
    Windows to complete the same show or hide transition without visible
    movement. The add-on never moves the taskbar window itself. This setting
    is ignored while Windows auto-hide is off. Default: Native.
  $options:
    - native: Native
    - none: None
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <windhawk_utils.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <shobjidl.h>
#include <uiautomation.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace {

constexpr LONG kEdgeSnapLogicalPx = 48;
constexpr LONG kBootstrapRevealLogicalPx = 960;
constexpr DWORD kWindowMessageTimeoutMs = 750;
constexpr DWORD kAutomationTimeoutMs = 150;
constexpr DWORD kWorkerStartupTimeoutMs = 5000;
constexpr DWORD kWorkerShutdownTimeoutMs = 4000;
constexpr DWORD kStatePollMs = 250;
constexpr DWORD kTaskbarViewHookFastWindowMs = 60000;
constexpr DWORD kTaskbarViewHookBackoffMs = 5000;
constexpr DWORD kGeometryDebounceMs = 25;
constexpr DWORD kGeometryShrinkDelayMs = 110;
constexpr DWORD kPointerInteractionRetryMs = 200;
constexpr DWORD kAutoHidePolicyIntervalMs = 50;
constexpr DWORD kWindowPolicyFallbackMs = 1000;
// A quiet interval separates distinct notifications from repeated attention
// signals emitted for the same notification.
constexpr ULONGLONG kNotificationBurstQuietMs = 5000;
constexpr ULONGLONG kRegionWatchdogMs = 1000;
constexpr ULONGLONG kLogThrottleMs = 5000;
constexpr int kMaxAutomationNodes = 256;
constexpr int kMaxExcludedPrograms = 128;
constexpr size_t kMaxRegions = 8;
constexpr UINT_PTR kTaskbarSubclassId = 0x54444354;  // "TDCT"
constexpr WPARAM kTaskbarCommandSync = 1;
constexpr WPARAM kTaskbarCommandRemove = 2;
constexpr WPARAM kTaskbarCommandPolicy = 3;
constexpr WPARAM kTaskbarCommandBindNative = 4;
constexpr WPARAM kTaskbarCommandDockPointer = 5;
constexpr UINT kTrayPrivateSettingMessage = WM_USER + 0x1CA;
constexpr WPARAM kTrayPrivateSettingAutoHideSet = 4;
constexpr WPARAM kTaskbandNotificationActivate =
    HSHELL_HIGHBIT | HSHELL_REDRAW;
constexpr UINT_PTR kTrayUITimerHide = 2;
constexpr UINT_PTR kTrayUITimerUnhide = 3;
constexpr UINT_PTR kDockExitHideTimer = 0x54444348;  // "TDCH"
constexpr LRESULT kTaskbarRemoveSucceeded = 1;
constexpr size_t kMaxVtableSearchSlots = 64;
constexpr int kTrayUnhideRequestDefault = 0;
constexpr int kViewReasonPointerOverChanged = 7;
constexpr int kViewReasonScreenEdgeEntered = 8;
constexpr PROPERTYKEY kAppUserModelIdKey = {
    {0x9F4C2855, 0x9F79, 0x4B39,
     {0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3}},
    5};
constexpr WCHAR kSubclassMarkerName[] =
    L"WindhawkTaskbarDockClickThrough_Subclass_" WH_MOD_ID;

enum class AutoHideMode {
    Overlap,
    Maximized,
    Always,
};

enum class AnimationStyle {
    None,
    Native,
};

struct Settings {
    DWORD fallbackScanMs = 5000;
    LONG horizontalPaddingLeft = 0;
    LONG horizontalPaddingRight = 0;
    LONG ghostAllowance = 0;
    LONG maxContentWidthPercent = 90;
    LONG islandGapPx = 120;
    AutoHideMode autoHideMode = AutoHideMode::Overlap;
    bool foregroundOnly = false;
    std::vector<std::wstring> excludedPrograms;
    bool primaryMonitorOnly = false;
    bool edgeReveal = true;
    DWORD showDelayMs = 40;
    DWORD hideDelayMs = 350;
    DWORD notificationHoldMs = 1500;
    AnimationStyle animation = AnimationStyle::Native;
    uint64_t revision = 0;
};

struct DesiredState {
    // Empty means no region: the complete shown taskbar remains clickable.
    std::vector<RECT> rects;
    std::vector<RECT> dockRects;
    std::vector<RECT> pendingShrinkRects;
    RECT shownRect{};
    HMONITOR monitor = nullptr;
    SIZE measuredSize{};
    UINT dpi = USER_DEFAULT_SCREEN_DPI;
    bool initialized = false;
    bool hasGeometry = false;
    bool hidden = false;
    bool syncPending = false;
    bool nativeBindingPending = false;
    bool nativeAutoHideAvailable = true;
    bool shrinkPending = false;
    bool structureRefreshPending = false;
    bool dockPointerInside = false;
    bool policyMatch = false;
    bool keepShown = false;
    bool policyInitialized = false;
    int pendingTransition = 0;  // 1=show, 2=hide.
    ULONGLONG transitionDueTick = 0;
    ULONGLONG shrinkDueTick = 0;
    ULONGLONG notificationBurstStartTick = 0;
    ULONGLONG notificationLastTick = 0;
    ULONGLONG lastPostTick = 0;
};

struct HorizontalSpan {
    LONG left;
    LONG right;
};

std::mutex g_stateMutex;
std::mutex g_nativeHookMutex;
Settings g_settings{};
std::unordered_map<HWND, DesiredState> g_desired;
std::unordered_map<HWND, void*> g_viewCoordinators;
std::unordered_map<void*, HWND> g_nativeTaskbarObjects;

UINT g_taskbarControlMsg = 0;
UINT g_installSubclassMsg = 0;
UINT g_taskbandNotificationMessage = 0;
HANDLE g_workerThread = nullptr;
HANDLE g_stopEvent = nullptr;
HANDLE g_refreshEvent = nullptr;
HANDLE g_workerReadyEvent = nullptr;
HANDLE g_installHookIdleEvent = nullptr;
HANDLE g_subclassIdleEvent = nullptr;
DWORD g_workerThreadId = 0;
std::atomic<bool> g_workerStartupSucceeded{false};
std::atomic<bool> g_initialized{false};
std::atomic<bool> g_unloading{false};
std::atomic<bool> g_windowsAutoHideEnabled{false};
std::atomic<bool> g_nativeAutoHideHooksInstalled{false};
std::atomic<bool> g_taskbarViewHooksInstalled{false};
std::atomic<bool> g_taskbarViewHookAttempted{false};
std::atomic<bool> g_geometryDirty{true};
std::atomic<bool> g_policyDirty{true};
std::atomic<uint64_t> g_settingsRevision{0};
std::atomic<uint64_t> g_topologyRevision{0};
std::atomic<ULONGLONG> g_lastErrorLogTick{0};
std::atomic<unsigned int> g_installHookCallbacks{0};
std::atomic<unsigned int> g_subclassCallbacks{0};
HWINEVENTHOOK g_geometryLifecycleWinEventHook = nullptr;
HWINEVENTHOOK g_geometryLocationWinEventHook = nullptr;
HWINEVENTHOOK g_foregroundWinEventHook = nullptr;
HMODULE g_taskbarModule = nullptr;

void* g_trayUIVtableInspectable = nullptr;
void* g_trayUIVtableComponentHost = nullptr;
void* g_secondaryTrayVtable = nullptr;

using TrayUIGetStuckMonitor_t = HMONITOR(WINAPI*)(void*);
TrayUIGetStuckMonitor_t g_trayUIGetStuckMonitorOriginal = nullptr;
using SecondaryTrayGetMonitor_t = HMONITOR(WINAPI*)(void*);
SecondaryTrayGetMonitor_t g_secondaryTrayGetMonitorOriginal = nullptr;
using TrayUIHide_t = void(WINAPI*)(void*);
TrayUIHide_t g_trayUIHideOriginal = nullptr;
using TrayUIUnhide_t = void(WINAPI*)(void*, int, int);
TrayUIUnhide_t g_trayUIUnhideOriginal = nullptr;
using SecondaryTrayAutoHide_t = void(WINAPI*)(void*, bool);
SecondaryTrayAutoHide_t g_secondaryTrayAutoHideOriginal = nullptr;
using SecondaryTrayUnhide_t = void(WINAPI*)(void*, int, int);
SecondaryTrayUnhide_t g_secondaryTrayUnhideOriginal = nullptr;
using TrayUISlideWindow_t =
    void(WINAPI*)(void*, HWND, const RECT*, HMONITOR, bool, bool);
TrayUISlideWindow_t g_trayUISlideWindowOriginal = nullptr;
using TrayUIWndProc_t =
    LRESULT(WINAPI*)(void*, HWND, UINT, WPARAM, LPARAM, bool*);
TrayUIWndProc_t g_trayUIWndProcOriginal = nullptr;
using SecondaryTrayWndProc_t =
    LRESULT(WINAPI*)(void*, HWND, UINT, WPARAM, LPARAM);
SecondaryTrayWndProc_t g_secondaryTrayWndProcOriginal = nullptr;

using ViewCoordinatorShouldExpand_t =
    bool(WINAPI*)(void*, HWND, bool);
ViewCoordinatorShouldExpand_t
    g_viewCoordinatorShouldExpandOriginal = nullptr;
using ViewCoordinatorPointerChanged_t =
    void(WINAPI*)(void*, HWND, bool, int);
ViewCoordinatorPointerChanged_t
    g_viewCoordinatorPointerChangedOriginal = nullptr;
using ViewCoordinatorUpdateExpanded_t =
    void(WINAPI*)(void*, HWND, int);
ViewCoordinatorUpdateExpanded_t
    g_viewCoordinatorUpdateExpandedOriginal = nullptr;

using SetTimer_t = decltype(&SetTimer);
SetTimer_t g_setTimerOriginal = nullptr;
thread_local bool g_revealUpdateActive = false;

static bool NativeAutoHideDispatchEnabled() {
    return g_initialized.load(std::memory_order_acquire) &&
           !g_unloading.load(std::memory_order_acquire) &&
           g_nativeAutoHideHooksInstalled.load(
               std::memory_order_acquire) &&
           g_taskbarViewHooksInstalled.load(
               std::memory_order_acquire);
}

static LRESULT CALLBACK TaskbarSubclassProc(HWND hWnd,
                                            UINT uMsg,
                                            WPARAM wParam,
                                            LPARAM lParam,
                                            UINT_PTR uIdSubclass,
                                            DWORD_PTR dwRefData);
static void WakeWorker();
static void SignalRefresh(bool geometry, bool policy);
static void HandleDockExitHideTimer(HWND hWnd);

static constexpr bool PolicyRefreshDue(ULONGLONG now,
                                       ULONGLONG earliestRefresh,
                                       ULONGLONG fallbackRefresh,
                                       bool dirty) {
    return now >= fallbackRefresh || (dirty && now >= earliestRefresh);
}

static constexpr ULONGLONG NextPolicyRefreshTick(ULONGLONG now,
                                                  ULONGLONG interval) {
    return now + interval;
}

static bool RectValid(const RECT& rect) {
    return rect.right > rect.left && rect.bottom > rect.top;
}

static bool RectEqual(const RECT& a, const RECT& b) {
    return a.left == b.left && a.top == b.top && a.right == b.right &&
           a.bottom == b.bottom;
}

static bool RectContains(const RECT& outer, const RECT& inner) {
    return outer.left <= inner.left && outer.top <= inner.top &&
           outer.right >= inner.right && outer.bottom >= inner.bottom;
}

static bool RectIntersects(const RECT& left, const RECT& right) {
    return left.left < right.right && right.left < left.right &&
           left.top < right.bottom && right.top < left.bottom;
}

static bool PointInShownDockArea(const DesiredState& state,
                                 POINT point) {
    if (!state.hasGeometry || !RectValid(state.shownRect)) {
        return false;
    }
    for (const RECT& local : state.dockRects) {
        const RECT screen{
            state.shownRect.left + local.left,
            state.shownRect.top + local.top,
            state.shownRect.left + local.right,
            state.shownRect.top + local.bottom,
        };
        if (point.x >= screen.left && point.x < screen.right &&
            point.y >= screen.top && point.y < screen.bottom) {
            return true;
        }
    }
    return false;
}

static constexpr bool IsFullscreenWindowShapeForReveal(
    LONG_PTR style,
    UINT showCommand,
    const RECT& windowRect,
    const RECT& monitorRect) {
    const bool coversMonitor =
        windowRect.left <= monitorRect.left + 2 &&
        windowRect.top <= monitorRect.top + 2 &&
        windowRect.right >= monitorRect.right - 2 &&
        windowRect.bottom >= monitorRect.bottom - 2;
    if (!coversMonitor) {
        return false;
    }

    const bool normalMaximizedFrame =
        showCommand == SW_SHOWMAXIMIZED &&
        (style & WS_CAPTION) == WS_CAPTION &&
        (style & WS_THICKFRAME) == WS_THICKFRAME &&
        !(style & WS_POPUP);
    if (normalMaximizedFrame) {
        return false;
    }

    return (style & WS_POPUP) ||
           (style & WS_CAPTION) != WS_CAPTION ||
           (style & WS_THICKFRAME) != WS_THICKFRAME;
}

static bool IsPrimaryTaskbarWindow(HWND hWnd) {
    WCHAR className[32]{};
    return GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
           wcscmp(className, L"Shell_TrayWnd") == 0;
}

static HMONITOR GetAssignedTaskbarMonitor(HWND hWnd) {
    if (IsPrimaryTaskbarWindow(hWnd)) {
        POINT origin{};
        return MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
    }

    HMONITOR monitor = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto it = g_desired.find(hWnd);
        if (it != g_desired.end() && it->second.monitor) {
            monitor = it->second.monitor;
        }
    }

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (monitor && GetMonitorInfoW(monitor, &monitorInfo)) {
        return monitor;
    }

    // A hidden secondary taskbar can physically overlap another monitor.
    // Wait for CSecondaryTray::GetMonitor instead of inferring ownership from
    // the off-screen HWND.
    return nullptr;
}

static UINT GetEffectiveMonitorDpi(HMONITOR monitor) {
    UINT dpiX = USER_DEFAULT_SCREEN_DPI;
    UINT dpiY = USER_DEFAULT_SCREEN_DPI;
    if (monitor &&
        SUCCEEDED(GetDpiForMonitor(
            monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY)) &&
        dpiX) {
        return dpiX;
    }
    return USER_DEFAULT_SCREEN_DPI;
}

static UINT GetTaskbarDpi(HWND hWnd,
                          const RECT* shownRect = nullptr) {
    HMONITOR monitor = nullptr;
    if (shownRect && RectValid(*shownRect)) {
        monitor = MonitorFromRect(
            shownRect, MONITOR_DEFAULTTONEAREST);
    }
    if (!monitor) {
        monitor = GetAssignedTaskbarMonitor(hWnd);
    }
    return GetEffectiveMonitorDpi(monitor);
}

static void ResetTopologyStateLocked(DesiredState& state,
                                     HMONITOR monitor,
                                     UINT dpi,
                                     bool hidden) {
    state = DesiredState{};
    state.monitor = monitor;
    state.dpi = dpi ? dpi : USER_DEFAULT_SCREEN_DPI;
    state.hidden = hidden;
}

class ScopedPhysicalCoordinates {
   public:
    ScopedPhysicalCoordinates() {
        const DPI_AWARENESS_CONTEXT current =
            GetThreadDpiAwarenessContext();
        if (!AreDpiAwarenessContextsEqual(
                current, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) {
            previous_ = SetThreadDpiAwarenessContext(
                DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
            valid_ = previous_ != nullptr;
        }
    }

    ~ScopedPhysicalCoordinates() {
        if (previous_) {
            SetThreadDpiAwarenessContext(previous_);
        }
    }

    ScopedPhysicalCoordinates(const ScopedPhysicalCoordinates&) = delete;
    ScopedPhysicalCoordinates& operator=(
        const ScopedPhysicalCoordinates&) = delete;

    explicit operator bool() const {
        return valid_;
    }

   private:
    DPI_AWARENESS_CONTEXT previous_ = nullptr;
    bool valid_ = true;
};

static bool QueryTaskbarPhysicalVisibility(HWND hWnd,
                                           RECT& windowRect,
                                           bool& shown) {
    const ScopedPhysicalCoordinates coordinates;
    if (!coordinates) {
        return false;
    }

    shown = false;
    if (!GetWindowRect(hWnd, &windowRect) || !RectValid(windowRect)) {
        return false;
    }

    const HMONITOR monitor = GetAssignedTaskbarMonitor(hWnd);
    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!monitor || !GetMonitorInfoW(monitor, &monitorInfo)) {
        return false;
    }

    shown = RectContains(monitorInfo.rcMonitor, windowRect);
    return true;
}

static LONG ClampToLong(int64_t value) {
    constexpr int64_t kLongMinimum = -2147483647LL - 1;
    constexpr int64_t kLongMaximum = 2147483647LL;
    if (value < kLongMinimum) {
        return static_cast<LONG>(MINLONG);
    }
    if (value > kLongMaximum) {
        return MAXLONG;
    }
    return static_cast<LONG>(value);
}

static RECT MakeBootstrapRegion(LONG width, LONG height, UINT dpi) {
    const LONG bootstrapWidth = std::min<LONG>(
        width,
        std::max<LONG>(
            1, MulDiv(kBootstrapRevealLogicalPx,
                      static_cast<int>(dpi),
                      USER_DEFAULT_SCREEN_DPI)));
    const LONG left = (width - bootstrapWidth) / 2;
    return {left, 0, left + bootstrapWidth, height};
}

static bool RectVectorsEqual(const std::vector<RECT>& a,
                             const std::vector<RECT>& b) {
    return a.size() == b.size() &&
           std::equal(a.begin(), a.end(), b.begin(), RectEqual);
}

static bool StopRequested() {
    return g_stopEvent &&
           WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0;
}

static void LogFailureThrottled(PCWSTR operation, ULONG code) {
    const ULONGLONG now = GetTickCount64();
    ULONGLONG previous = g_lastErrorLogTick.load(std::memory_order_relaxed);
    if (now - previous < kLogThrottleMs ||
        !g_lastErrorLogTick.compare_exchange_strong(
            previous, now, std::memory_order_relaxed)) {
        return;
    }

    Wh_Log(L"%s failed (code 0x%08lX)", operation, code);
}

static AutoHideMode ReadAutoHideMode() {
    auto value =
        WindhawkUtils::StringSetting::make(L"autoHideMode");
    AutoHideMode result = AutoHideMode::Overlap;
    if (wcscmp(value.get(), L"maximized") == 0) {
        result = AutoHideMode::Maximized;
    } else if (wcscmp(value.get(), L"always") == 0) {
        result = AutoHideMode::Always;
    }
    return result;
}

static AnimationStyle ReadAnimationStyle() {
    auto value =
        WindhawkUtils::StringSetting::make(L"animation");
    const AnimationStyle result =
        wcscmp(value.get(), L"none") == 0
            ? AnimationStyle::None
            : AnimationStyle::Native;
    return result;
}

static Settings ReadSettings() {
    Settings settings{};
    settings.fallbackScanMs = static_cast<DWORD>(
        std::clamp(Wh_GetIntSetting(L"fallbackScanMs"), 2000, 15000));
    settings.horizontalPaddingLeft = static_cast<LONG>(
        std::clamp(Wh_GetIntSetting(L"horizontalPaddingLeft"), 0, 600));
    settings.horizontalPaddingRight = static_cast<LONG>(
        std::clamp(Wh_GetIntSetting(L"horizontalPaddingRight"), 0, 600));
    settings.ghostAllowance = static_cast<LONG>(
        std::clamp(Wh_GetIntSetting(L"ghostAllowance"), 0, 600));
    settings.maxContentWidthPercent = static_cast<LONG>(
        std::clamp(Wh_GetIntSetting(L"maxContentWidthPercent"), 10, 100));
    settings.islandGapPx = static_cast<LONG>(
        std::clamp(Wh_GetIntSetting(L"islandGapPx"), 8, 4000));
    settings.autoHideMode = ReadAutoHideMode();
    settings.foregroundOnly =
        Wh_GetIntSetting(L"foregroundOnly") != 0;
    settings.excludedPrograms.reserve(8);
    for (int i = 0; i < kMaxExcludedPrograms; i++) {
        auto value = WindhawkUtils::StringSetting::make(
            L"excludedPrograms[%d]", i);
        const bool empty = !*value.get();
        if (!empty) {
            settings.excludedPrograms.emplace_back(value.get());
        }
        if (empty) {
            break;
        }
    }
    settings.primaryMonitorOnly =
        Wh_GetIntSetting(L"primaryMonitorOnly") != 0;
    settings.edgeReveal =
        Wh_GetIntSetting(L"edgeReveal") != 0;
    settings.showDelayMs = static_cast<DWORD>(
        std::clamp(Wh_GetIntSetting(L"showDelayMs"), 0, 2000));
    settings.hideDelayMs = static_cast<DWORD>(
        std::clamp(Wh_GetIntSetting(L"hideDelayMs"), 0, 5000));
    settings.notificationHoldMs = static_cast<DWORD>(
        std::clamp(Wh_GetIntSetting(L"notificationHoldMs"), 0, 10000));
    settings.animation = ReadAnimationStyle();
    return settings;
}

static void LoadSettings() {
    Settings settings = ReadSettings();
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        settings.revision = g_settings.revision + 1;
        g_settings = settings;
    }
    g_settingsRevision.store(settings.revision,
                             std::memory_order_release);
}

static Settings GetSettingsSnapshot() {
    std::lock_guard<std::mutex> lock(g_stateMutex);
    return g_settings;
}

class UniqueRegion {
   public:
    explicit UniqueRegion(HRGN region = nullptr) : region_(region) {}
    ~UniqueRegion() {
        if (region_) {
            DeleteObject(region_);
        }
    }

    UniqueRegion(const UniqueRegion&) = delete;
    UniqueRegion& operator=(const UniqueRegion&) = delete;

    HRGN get() const {
        return region_;
    }

    HRGN release() {
        HRGN region = region_;
        region_ = nullptr;
        return region;
    }

   private:
    HRGN region_;
};

static HRGN BuildCombinedRegion(const std::vector<RECT>& rects) {
    if (rects.empty()) {
        return nullptr;
    }

    UniqueRegion combined(CreateRectRgn(rects[0].left, rects[0].top,
                                        rects[0].right, rects[0].bottom));
    if (!combined.get()) {
        return nullptr;
    }

    for (size_t i = 1; i < rects.size(); i++) {
        UniqueRegion piece(CreateRectRgn(rects[i].left, rects[i].top,
                                         rects[i].right, rects[i].bottom));
        if (!piece.get() ||
            CombineRgn(combined.get(), combined.get(), piece.get(), RGN_OR) ==
                ERROR) {
            return nullptr;
        }
    }

    return combined.release();
}

static bool ClearWindowRegion(HWND hWnd) {
    UniqueRegion current(CreateRectRgn(0, 0, 0, 0));
    if (!current.get()) {
        LogFailureThrottled(L"CreateRectRgn(clear)", GetLastError());
        return false;
    }

    if (GetWindowRgn(hWnd, current.get()) == ERROR) {
        return true;
    }

    if (!SetWindowRgn(hWnd, nullptr, TRUE)) {
        LogFailureThrottled(L"SetWindowRgn(clear)", GetLastError());
        return false;
    }

    return true;
}

// Runs on the taskbar UI thread.
static void ApplyDesiredRegion(HWND hWnd,
                               const std::vector<RECT>& desiredRects) {
    if (desiredRects.empty()) {
        ClearWindowRegion(hWnd);
        return;
    }

    RECT windowRect{};
    bool shown = false;
    if (!QueryTaskbarPhysicalVisibility(hWnd, windowRect, shown) ||
        !shown) {
        // Native auto-hide owns the narrow hidden reveal region.
        return;
    }

    UniqueRegion desired(BuildCombinedRegion(desiredRects));
    if (!desired.get()) {
        LogFailureThrottled(L"BuildCombinedRegion", GetLastError());
        ClearWindowRegion(hWnd);
        return;
    }

    UniqueRegion current(CreateRectRgn(0, 0, 0, 0));
    if (!current.get()) {
        LogFailureThrottled(L"CreateRectRgn(current)", GetLastError());
        ClearWindowRegion(hWnd);
        return;
    }

    const bool hasCurrent = GetWindowRgn(hWnd, current.get()) != ERROR;
    if (hasCurrent && EqualRgn(current.get(), desired.get())) {
        return;
    }

    if (SetWindowRgn(hWnd, desired.get(), TRUE)) {
        // SetWindowRgn owns the region only after a successful call.
        desired.release();
        return;
    }

    LogFailureThrottled(L"SetWindowRgn(apply)", GetLastError());
    ClearWindowRegion(hWnd);
}

static void EraseTrackedWindow(HWND hWnd) {
    KillTimer(hWnd, kDockExitHideTimer);
    std::lock_guard<std::mutex> lock(g_stateMutex);
    g_desired.erase(hWnd);
    g_viewCoordinators.erase(hWnd);
    std::erase_if(g_nativeTaskbarObjects,
                  [hWnd](const auto& item) {
                      return item.second == hWnd;
                  });
}

static HANDLE GetSubclassMarkerValue() {
    return reinterpret_cast<HANDLE>(
        static_cast<ULONG_PTR>(kTaskbarSubclassId));
}

static bool HasSubclassMarker(HWND hWnd) {
    return GetPropW(hWnd, kSubclassMarkerName) == GetSubclassMarkerValue();
}

class SubclassCallbackGuard {
   public:
    SubclassCallbackGuard() {
        if (g_subclassCallbacks.fetch_add(1, std::memory_order_acq_rel) == 0 &&
            g_subclassIdleEvent) {
            ResetEvent(g_subclassIdleEvent);
        }
    }

    ~SubclassCallbackGuard() {
        if (g_subclassCallbacks.fetch_sub(1, std::memory_order_acq_rel) == 1 &&
            g_subclassIdleEvent) {
            SetEvent(g_subclassIdleEvent);
        }
    }

    SubclassCallbackGuard(const SubclassCallbackGuard&) = delete;
    SubclassCallbackGuard& operator=(const SubclassCallbackGuard&) = delete;
};

static LRESULT CALLBACK TaskbarSubclassProc(HWND hWnd,
                                            UINT uMsg,
                                            WPARAM wParam,
                                            LPARAM lParam,
                                            UINT_PTR uIdSubclass,
                                            DWORD_PTR) {
    const SubclassCallbackGuard callbackGuard;
    const bool displayChanged = uMsg == WM_DISPLAYCHANGE;
    const bool dpiChanged = uMsg == WM_DPICHANGED;
    if (uMsg == WM_MOUSELEAVE) {
        // This is only a low-cost wake hint. The worker re-reads the cursor
        // and remains the source of truth for the measured dock boundary.
        WakeWorker();
    }
    if (displayChanged) {
        g_topologyRevision.fetch_add(1, std::memory_order_acq_rel);
        HMONITOR monitor = nullptr;
        if (IsPrimaryTaskbarWindow(hWnd)) {
            POINT origin{};
            monitor =
                MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
        }
        const UINT dpi = GetEffectiveMonitorDpi(monitor);
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            auto it = g_desired.find(hWnd);
            if (it != g_desired.end()) {
                const bool hidden = it->second.hidden;
                ResetTopologyStateLocked(
                    it->second, monitor, dpi, hidden);
            }
        }
        // Remove the old monitor's physical clipping before Windows lays out
        // the taskbar on the new topology.
        ClearWindowRegion(hWnd);
    }

    if (uMsg == g_taskbarControlMsg) {
        if (wParam == kTaskbarCommandRemove) {
            if (!RemoveWindowSubclass(hWnd, TaskbarSubclassProc,
                                      uIdSubclass)) {
                LogFailureThrottled(L"RemoveWindowSubclass", GetLastError());
                return 0;
            }

            RemovePropW(hWnd, kSubclassMarkerName);
            // Detaching executable callback state takes priority over restoring
            // the visual region. Region cleanup is best-effort after detach.
            ClearWindowRegion(hWnd);
            EraseTrackedWindow(hWnd);
            return kTaskbarRemoveSucceeded;
        }

        if (wParam == kTaskbarCommandSync) {
            std::vector<RECT> desiredRects;
            bool haveDesired = false;
            {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                auto it = g_desired.find(hWnd);
                if (it != g_desired.end()) {
                    desiredRects = it->second.rects;
                    it->second.syncPending = false;
                    haveDesired = true;
                }
            }

            if (haveDesired) {
                ApplyDesiredRegion(hWnd, desiredRects);
            }
            return 0;
        }

        // Native-binding and policy commands continue to the taskbar WndProc
        // hooks, where the exact TrayUI or CSecondaryTray object is available.
    }

    if (uMsg == WM_TIMER && wParam == kDockExitHideTimer) {
        HandleDockExitHideTimer(hWnd);
        return 0;
    }

    if (uMsg == WM_NCDESTROY) {
        RemovePropW(hWnd, kSubclassMarkerName);
        RemoveWindowSubclass(hWnd, TaskbarSubclassProc, uIdSubclass);
        EraseTrackedWindow(hWnd);
    }

    const LRESULT result =
        DefSubclassProc(hWnd, uMsg, wParam, lParam);
    if (displayChanged || dpiChanged) {
        SignalRefresh(true, true);
    }
    return result;
}

static bool IsTaskbarWindow(HWND hWnd) {
    DWORD processId = 0;
    if (!GetWindowThreadProcessId(hWnd, &processId) ||
        processId != GetCurrentProcessId()) {
        return false;
    }

    WCHAR className[32];
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return wcscmp(className, L"Shell_TrayWnd") == 0 ||
           wcscmp(className, L"Shell_SecondaryTrayWnd") == 0;
}

class InstallHookCallbackGuard {
   public:
    InstallHookCallbackGuard() {
        if (g_installHookCallbacks.fetch_add(1, std::memory_order_acq_rel) ==
            0) {
            ResetEvent(g_installHookIdleEvent);
        }
    }

    ~InstallHookCallbackGuard() {
        if (g_installHookCallbacks.fetch_sub(1, std::memory_order_acq_rel) ==
            1) {
            SetEvent(g_installHookIdleEvent);
        }
    }
};

static LRESULT CALLBACK InstallSubclassHookProc(int code,
                                                 WPARAM wParam,
                                                 LPARAM lParam) {
    InstallHookCallbackGuard callbackGuard;

    if (code == HC_ACTION) {
        const CWPSTRUCT* message = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (message && message->message == g_installSubclassMsg &&
            IsTaskbarWindow(message->hwnd) &&
            !HasSubclassMarker(message->hwnd)) {
            if (SetPropW(message->hwnd, kSubclassMarkerName,
                         GetSubclassMarkerValue())) {
                if (SetWindowSubclass(message->hwnd, TaskbarSubclassProc,
                                      kTaskbarSubclassId, 0)) {
                    std::lock_guard<std::mutex> lock(g_stateMutex);
                    g_desired.try_emplace(message->hwnd);
                } else {
                    RemovePropW(message->hwnd, kSubclassMarkerName);
                    LogFailureThrottled(L"SetWindowSubclass",
                                        GetLastError());
                }
            } else {
                LogFailureThrottled(L"SetPropW(subclass marker)",
                                    GetLastError());
            }
        }
    }

    return CallNextHookEx(nullptr, code, wParam, lParam);
}

static void WaitForInstallHookCallbacks() {
    while (g_installHookCallbacks.load(std::memory_order_acquire) != 0) {
        WaitForSingleObject(g_installHookIdleEvent, INFINITE);
    }
}

static bool TryInstallTaskbarSubclass(HWND hWnd) {
    if (!IsTaskbarWindow(hWnd)) {
        return false;
    }

    if (HasSubclassMarker(hWnd)) {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        g_desired.try_emplace(hWnd);
        return true;
    }

    const DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId) {
        return false;
    }

    if (threadId == GetCurrentThreadId()) {
        if (!SetPropW(hWnd, kSubclassMarkerName,
                      GetSubclassMarkerValue())) {
            return false;
        }
        if (!SetWindowSubclass(hWnd, TaskbarSubclassProc,
                               kTaskbarSubclassId, 0)) {
            RemovePropW(hWnd, kSubclassMarkerName);
            return false;
        }
        std::lock_guard<std::mutex> lock(g_stateMutex);
        g_desired.try_emplace(hWnd);
        return true;
    }

    HHOOK hook =
        SetWindowsHookExW(WH_CALLWNDPROC, InstallSubclassHookProc, nullptr,
                          threadId);
    if (!hook) {
        LogFailureThrottled(L"SetWindowsHookExW", GetLastError());
        return false;
    }

    DWORD_PTR result = 0;
    SetLastError(ERROR_SUCCESS);
    const LRESULT sent = SendMessageTimeoutW(
        hWnd, g_installSubclassMsg, 0, 0,
        SMTO_BLOCK | SMTO_ABORTIFHUNG | SMTO_ERRORONEXIT,
        kWindowMessageTimeoutMs, &result);
    if (!sent) {
        DWORD error = GetLastError();
        if (error == ERROR_SUCCESS) {
            error = ERROR_TIMEOUT;
        }
        LogFailureThrottled(L"SendMessageTimeoutW(install)", error);
    }

    while (!UnhookWindowsHookEx(hook)) {
        const DWORD error = GetLastError();
        if (error == ERROR_INVALID_HOOK_HANDLE) {
            break;
        }
        LogFailureThrottled(L"UnhookWindowsHookEx", error);
        Sleep(50);
    }
    WaitForInstallHookCallbacks();

    if (!HasSubclassMarker(hWnd)) {
        return false;
    }

    std::lock_guard<std::mutex> lock(g_stateMutex);
    g_desired.try_emplace(hWnd);
    return true;
}

static void RequestNativeTaskbarBinding(HWND hWnd) {
    if (!NativeAutoHideDispatchEnabled()) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto it = g_desired.find(hWnd);
        if (it == g_desired.end() ||
            !it->second.nativeAutoHideAvailable ||
            it->second.nativeBindingPending) {
            return;
        }
        if (it->second.monitor) {
            MONITORINFO monitorInfo{};
            monitorInfo.cbSize = sizeof(monitorInfo);
            if (GetMonitorInfoW(
                    it->second.monitor, &monitorInfo)) {
                return;
            }
            it->second.monitor = nullptr;
        }
        it->second.nativeBindingPending = true;
    }

    if (!PostMessageW(hWnd, g_taskbarControlMsg,
                      kTaskbarCommandBindNative, 0)) {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto it = g_desired.find(hWnd);
        if (it != g_desired.end()) {
            it->second.nativeBindingPending = false;
        }
        LogFailureThrottled(
            L"PostMessageW(native taskbar binding)", GetLastError());
    }
}

static std::vector<HWND> SnapshotSubclassedWindows() {
    std::lock_guard<std::mutex> lock(g_stateMutex);
    std::vector<HWND> windows;
    windows.reserve(g_desired.size());
    for (const auto& entry : g_desired) {
        windows.push_back(entry.first);
    }
    return windows;
}

static void PruneStaleTrackedWindows() {
    for (HWND hWnd : SnapshotSubclassedWindows()) {
        if (!IsWindow(hWnd)) {
            EraseTrackedWindow(hWnd);
        }
    }
}

static void WaitForSubclassCallbacks() {
    while (g_subclassCallbacks.load(std::memory_order_acquire) != 0) {
        WaitForSingleObject(g_subclassIdleEvent, INFINITE);
    }
}

static void RemoveAllTaskbarSubclasses() {
    for (;;) {
        PruneStaleTrackedWindows();
        std::vector<HWND> windows = SnapshotSubclassedWindows();
        if (windows.empty()) {
            WaitForSubclassCallbacks();
            return;
        }

        bool removedAny = false;
        for (HWND hWnd : windows) {
            if (!IsWindow(hWnd)) {
                EraseTrackedWindow(hWnd);
                removedAny = true;
                continue;
            }

            DWORD_PTR result = 0;
            SetLastError(ERROR_SUCCESS);
            if (!SendMessageTimeoutW(
                    hWnd, g_taskbarControlMsg, kTaskbarCommandRemove, 0,
                    SMTO_BLOCK | SMTO_ABORTIFHUNG | SMTO_ERRORONEXIT,
                    kWindowMessageTimeoutMs, &result)) {
                DWORD error = GetLastError();
                if (error == ERROR_SUCCESS) {
                    error = ERROR_TIMEOUT;
                }
                LogFailureThrottled(L"SendMessageTimeoutW(remove)", error);
            } else if (result == kTaskbarRemoveSucceeded) {
                // The owner thread positively confirmed callback detachment.
                // RemovePropW is repeated here in case the first call failed.
                RemovePropW(hWnd, kSubclassMarkerName);
                removedAny = true;
            }
        }

        if (!removedAny) {
            // A hung owner thread leaves no safe bounded detach path. Keep the
            // module loaded instead of returning with a live subclass pointer.
            Sleep(50);
        }
    }
}

template <typename T>
class ComPtr {
   public:
    ComPtr() = default;
    ~ComPtr() {
        reset();
    }

    ComPtr(const ComPtr&) = delete;
    ComPtr& operator=(const ComPtr&) = delete;

    T* get() const {
        return value_;
    }

    T** put() {
        reset();
        return &value_;
    }

    T* operator->() const {
        return value_;
    }

    explicit operator bool() const {
        return value_ != nullptr;
    }

    void reset(T* value = nullptr) {
        if (value_) {
            value_->Release();
        }
        value_ = value;
    }

   private:
    T* value_ = nullptr;
};

static HRESULT CreateClassCondition(
    IUIAutomation2* automation,
    PCWSTR className,
    IUIAutomationCondition** condition) {
    VARIANT value{};
    value.vt = VT_BSTR;
    value.bstrVal = SysAllocString(className);
    if (!value.bstrVal) {
        return E_OUTOFMEMORY;
    }
    const HRESULT result = automation->CreatePropertyCondition(
        UIA_ClassNamePropertyId, value, condition);
    VariantClear(&value);
    return result;
}

static HRESULT CreateIntCondition(
    IUIAutomation2* automation,
    PROPERTYID property,
    LONG value,
    IUIAutomationCondition** condition) {
    VARIANT propertyValue{};
    propertyValue.vt = VT_I4;
    propertyValue.lVal = value;
    return automation->CreatePropertyCondition(
        property, propertyValue, condition);
}

struct GhostRun {
    RECT previous{};
    RECT next{};
    unsigned int itemCount = 0;
    bool hasPrevious = false;
    bool hasNext = false;
};

struct WorkerBuffers {
    std::vector<HWND> taskbars;
    std::vector<RECT> dockItems;
    std::vector<HorizontalSpan> spans;
    std::vector<GhostRun> ghostRuns;

    WorkerBuffers() {
        taskbars.reserve(8);
        dockItems.reserve(96);
        spans.reserve(kMaxRegions);
        ghostRuns.reserve(8);
    }
};

static void NormalizeSpans(std::vector<HorizontalSpan>& spans) {
    spans.erase(std::remove_if(spans.begin(), spans.end(),
                               [](const HorizontalSpan& span) {
                                   return span.right <= span.left;
                               }),
                spans.end());
    std::sort(spans.begin(), spans.end(),
              [](const HorizontalSpan& a, const HorizontalSpan& b) {
                  return a.left < b.left;
              });

    size_t output = 0;
    for (const HorizontalSpan& span : spans) {
        if (output == 0 || span.left > spans[output - 1].right) {
            spans[output++] = span;
        } else if (span.right > spans[output - 1].right) {
            spans[output - 1].right = span.right;
        }
    }
    spans.resize(output);
}

static void GroupDockItems(std::vector<RECT>& items,
                           LONG gapThreshold,
                           std::vector<HorizontalSpan>& spans) {
    std::sort(items.begin(), items.end(),
              [](const RECT& a, const RECT& b) {
                  return a.left < b.left;
              });

    spans.clear();
    LONG left = items[0].left;
    LONG right = items[0].right;
    for (size_t i = 1; i < items.size(); i++) {
        if (static_cast<int64_t>(items[i].left) - right >
            gapThreshold) {
            spans.push_back({left, right});
            left = items[i].left;
            right = items[i].right;
        } else if (items[i].right > right) {
            right = items[i].right;
        }
    }
    spans.push_back({left, right});
}

static size_t FindSpanForRect(
    const std::vector<HorizontalSpan>& spans,
    const RECT& rect) {
    for (size_t i = 0; i < spans.size(); i++) {
        if (rect.right > spans[i].left &&
            rect.left < spans[i].right) {
            return i;
        }
    }
    return spans.size();
}

enum class ComputeDockResult {
    Complete,
    Unavailable,
    Incomplete,
    Cancelled,
};

// FindAllBuildCache is the only provider subtree call. Every per-element
// property below comes from the immutable cache, keeping Explorer's taskbar
// thread out of long chains of synchronous UI Automation requests.
static ComputeDockResult ComputeDock(
    IUIAutomation2* automation,
    IUIAutomationCondition* dockCondition,
    IUIAutomationCacheRequest* cacheRequest,
    HWND hWnd,
    const Settings& settings,
    WorkerBuffers& buffers,
    RECT& windowRect,
    std::vector<HorizontalSpan>& spans) {
    if (StopRequested()) {
        return ComputeDockResult::Cancelled;
    }
    if (!GetWindowRect(hWnd, &windowRect) || !RectValid(windowRect)) {
        return ComputeDockResult::Unavailable;
    }

    ComPtr<IUIAutomationElement> root;
    HRESULT result = automation->ElementFromHandle(hWnd, root.put());
    if (FAILED(result) || !root) {
        return ComputeDockResult::Unavailable;
    }

    ComPtr<IUIAutomationElementArray> snapshot;
    result = root->FindAllBuildCache(TreeScope_Subtree, dockCondition,
                                     cacheRequest, snapshot.put());
    if (FAILED(result) || !snapshot) {
        return StopRequested() ? ComputeDockResult::Cancelled
                               : ComputeDockResult::Unavailable;
    }

    int length = 0;
    if (FAILED(snapshot->get_Length(&length)) || length <= 0) {
        return ComputeDockResult::Unavailable;
    }
    if (length > kMaxAutomationNodes) {
        return ComputeDockResult::Incomplete;
    }

    buffers.dockItems.clear();
    buffers.ghostRuns.clear();
    const LONG taskbarWidth = windowRect.right - windowRect.left;
    RECT previousDockItem{};
    bool havePreviousDockItem = false;
    bool ghostPending = false;
    GhostRun pendingGhost{};

    for (int i = 0; i < length; i++) {
        if (StopRequested()) {
            return ComputeDockResult::Cancelled;
        }

        ComPtr<IUIAutomationElement> element;
        if (FAILED(snapshot->GetElement(i, element.put())) || !element) {
            return ComputeDockResult::Incomplete;
        }

        BOOL offscreen = TRUE;
        if (FAILED(element->get_CachedIsOffscreen(&offscreen))) {
            return ComputeDockResult::Incomplete;
        }
        if (offscreen) {
            continue;
        }

        BSTR className = nullptr;
        if (FAILED(element->get_CachedClassName(&className))) {
            if (className) {
                SysFreeString(className);
            }
            return ComputeDockResult::Incomplete;
        }
        const std::wstring_view classView =
            className
                ? std::wstring_view(
                      className, SysStringLen(className))
                : std::wstring_view{};
        const bool dockClass =
            classView == L"Taskbar.TaskbarFrameAutomationPeer" ||
            classView.rfind(L"SystemTray.", 0) == 0;
        if (className) {
            SysFreeString(className);
        }
        if (!dockClass) {
            continue;
        }

        RECT rect{};
        if (FAILED(element->get_CachedBoundingRectangle(&rect)) ||
            !RectValid(rect)) {
            if (settings.ghostAllowance <= 0) {
                continue;
            }
            if (!ghostPending) {
                ghostPending = true;
                pendingGhost = {};
                pendingGhost.hasPrevious = havePreviousDockItem;
                pendingGhost.previous = previousDockItem;
            }
            pendingGhost.itemCount++;
            continue;
        }

        const bool intersectsTaskbar =
            rect.right > windowRect.left && rect.left < windowRect.right &&
            rect.bottom > windowRect.top && rect.top < windowRect.bottom;
        const int64_t width =
            static_cast<int64_t>(rect.right) - rect.left;
        const bool tooWide =
            settings.maxContentWidthPercent < 100 &&
            width * 100 >=
                static_cast<int64_t>(taskbarWidth) *
                    settings.maxContentWidthPercent;
        if (!intersectsTaskbar || tooWide) {
            continue;
        }

        if (ghostPending) {
            pendingGhost.hasNext = true;
            pendingGhost.next = rect;
            buffers.ghostRuns.push_back(pendingGhost);
            ghostPending = false;
        }
        buffers.dockItems.push_back(rect);
        previousDockItem = rect;
        havePreviousDockItem = true;
    }

    if (ghostPending) {
        buffers.ghostRuns.push_back(pendingGhost);
    }
    if (buffers.dockItems.empty()) {
        return ComputeDockResult::Unavailable;
    }

    GroupDockItems(buffers.dockItems, settings.islandGapPx, spans);
    if (spans.empty() || spans.size() > kMaxRegions) {
        return ComputeDockResult::Incomplete;
    }

    const UINT dpi = GetTaskbarDpi(hWnd, &windowRect);
    const LONG allowance = static_cast<LONG>(
        MulDiv(settings.ghostAllowance, static_cast<int>(dpi),
               USER_DEFAULT_SCREEN_DPI));
    for (const GhostRun& ghost : buffers.ghostRuns) {
        if (allowance <= 0 || !ghost.itemCount) {
            continue;
        }
        const int64_t fallbackWidth =
            static_cast<int64_t>(allowance) * ghost.itemCount;
        if (ghost.hasPrevious && ghost.hasNext) {
            if (ghost.next.right <= ghost.previous.left) {
                return ComputeDockResult::Incomplete;
            }
            const size_t previousSpan =
                FindSpanForRect(spans, ghost.previous);
            const size_t nextSpan = FindSpanForRect(spans, ghost.next);
            if (previousSpan == spans.size() ||
                nextSpan == spans.size() ||
                previousSpan > nextSpan) {
                return ComputeDockResult::Incomplete;
            }
            if (previousSpan == nextSpan) {
                continue;
            }
            if (previousSpan + 1 != nextSpan ||
                ghost.previous.right != spans[previousSpan].right ||
                ghost.next.left != spans[nextSpan].left) {
                return ComputeDockResult::Incomplete;
            }
            const int64_t gap =
                static_cast<int64_t>(spans[nextSpan].left) -
                spans[previousSpan].right;
            if (gap <= 1) {
                continue;
            }
            const int64_t width =
                std::min(fallbackWidth, gap - 1);
            const int64_t previousShare = (width + 1) / 2;
            spans[previousSpan].right = ClampToLong(
                static_cast<int64_t>(spans[previousSpan].right) +
                previousShare);
            spans[nextSpan].left = ClampToLong(
                static_cast<int64_t>(spans[nextSpan].left) -
                (width - previousShare));
        } else if (ghost.hasPrevious) {
            const size_t span =
                FindSpanForRect(spans, ghost.previous);
            if (span == spans.size() || span + 1 != spans.size() ||
                ghost.previous.right != spans[span].right) {
                return ComputeDockResult::Incomplete;
            }
            spans[span].right = ClampToLong(
                static_cast<int64_t>(spans[span].right) +
                fallbackWidth);
        } else if (ghost.hasNext) {
            const size_t span = FindSpanForRect(spans, ghost.next);
            if (span != 0 || ghost.next.left != spans[span].left) {
                return ComputeDockResult::Incomplete;
            }
            spans[span].left = ClampToLong(
                static_cast<int64_t>(spans[span].left) -
                fallbackWidth);
        } else {
            return ComputeDockResult::Incomplete;
        }
    }

    NormalizeSpans(spans);
    return spans.empty() ? ComputeDockResult::Unavailable
                         : ComputeDockResult::Complete;
}

static void NormalizeRegionRects(std::vector<RECT>& rects) {
    rects.erase(std::remove_if(rects.begin(), rects.end(),
                               [](const RECT& rect) {
                                   return !RectValid(rect);
                               }),
                rects.end());
    std::sort(rects.begin(), rects.end(),
              [](const RECT& a, const RECT& b) {
                  return a.left < b.left;
              });

    size_t output = 0;
    for (const RECT& rect : rects) {
        if (output == 0 || rect.left > rects[output - 1].right) {
            rects[output++] = rect;
        } else if (rect.right > rects[output - 1].right) {
            rects[output - 1].right = rect.right;
        }
    }
    rects.resize(output);
}

static bool RegionAddsCoverage(const std::vector<RECT>& candidate,
                               const std::vector<RECT>& current) {
    if (current.empty()) {
        return false;
    }
    if (candidate.empty()) {
        return true;
    }
    for (const RECT& rect : candidate) {
        bool contained = false;
        for (const RECT& previous : current) {
            if (RectContains(previous, rect)) {
                contained = true;
                break;
            }
        }
        if (!contained) {
            return true;
        }
    }
    return false;
}

static bool PostRegionSync(HWND hWnd, DesiredState& state, bool force) {
    const ULONGLONG now = GetTickCount64();
    if (!force && state.syncPending &&
        now - state.lastPostTick < kRegionWatchdogMs) {
        return true;
    }
    state.syncPending = true;
    state.lastPostTick = now;
    if (PostMessageW(hWnd, g_taskbarControlMsg,
                     kTaskbarCommandSync, 0)) {
        return true;
    }
    state.syncPending = false;
    LogFailureThrottled(L"PostMessageW(region sync)", GetLastError());
    return false;
}

static bool PublishFailSafeRegion(HWND hWnd) {
    bool changed = false;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto it = g_desired.find(hWnd);
        if (it == g_desired.end()) {
            return false;
        }
        DesiredState& state = it->second;
        if (state.hasGeometry && !state.dockRects.empty() &&
            RectValid(state.shownRect)) {
            // A transient UI Automation failure must not widen a verified
            // dock boundary to the full taskbar window. Keep the last-good
            // region and retry on the next normal geometry opportunity.
            state.shrinkPending = false;
            state.structureRefreshPending = false;
            state.pendingShrinkRects.clear();
            return false;
        }
        changed = !state.initialized || !state.rects.empty();
        state.rects.clear();
        state.initialized = true;
        state.shrinkPending = false;
        state.structureRefreshPending = false;
        state.pendingShrinkRects.clear();
        PostRegionSync(hWnd, state, changed);
    }
    return changed;
}

static bool InvalidateShownGeometry(HWND hWnd,
                                    const RECT& shownRect,
                                    HMONITOR monitor,
                                    UINT dpi) {
    bool changed = false;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto it = g_desired.find(hWnd);
        if (it == g_desired.end()) {
            return false;
        }

        DesiredState& state = it->second;
        changed = !state.initialized || state.hasGeometry ||
                  !state.rects.empty();
        state.rects.clear();
        state.dockRects.clear();
        state.pendingShrinkRects.clear();
        state.shownRect = shownRect;
        state.monitor = monitor;
        state.measuredSize = {
            shownRect.right - shownRect.left,
            shownRect.bottom - shownRect.top};
        state.dpi = dpi;
        state.initialized = true;
        state.hasGeometry = false;
        state.hidden = false;
        state.syncPending = false;
        state.shrinkPending = false;
        state.structureRefreshPending = false;
        PostRegionSync(hWnd, state, changed);
    }
    return changed;
}

static bool PublishDesiredRegion(HWND hWnd,
                                 std::vector<RECT> desiredRects,
                                 std::vector<RECT> dockRects,
                                 const RECT& shownRect,
                                 UINT dpi,
                                 uint64_t topologyRevision) {
    const ULONGLONG now = GetTickCount64();
    const HMONITOR monitor =
        MonitorFromRect(&shownRect, MONITOR_DEFAULTTONEAREST);
    bool changed = false;
    std::lock_guard<std::mutex> lock(g_stateMutex);
    if (topologyRevision !=
        g_topologyRevision.load(std::memory_order_acquire)) {
        return false;
    }
    auto it = g_desired.find(hWnd);
    if (it == g_desired.end()) {
        return false;
    }

    DesiredState& state = it->second;
    const bool firstGeometry = !state.hasGeometry;
    state.hidden = false;
    state.shownRect = shownRect;
    state.monitor = monitor;
    state.measuredSize = {
        shownRect.right - shownRect.left,
        shownRect.bottom - shownRect.top};
    state.dpi = dpi;
    state.dockRects = std::move(dockRects);
    state.hasGeometry = !state.dockRects.empty();
    state.structureRefreshPending = false;

    const bool same =
        state.initialized &&
        RectVectorsEqual(state.rects, desiredRects);
    if (same) {
        state.shrinkPending = false;
        state.pendingShrinkRects.clear();
        PostRegionSync(hWnd, state, false);
        return false;
    }

    const bool applyImmediately =
        !state.initialized || firstGeometry ||
        RegionAddsCoverage(desiredRects, state.rects);
    if (!applyImmediately) {
        if (!state.shrinkPending ||
            !RectVectorsEqual(state.pendingShrinkRects, desiredRects)) {
            state.pendingShrinkRects = std::move(desiredRects);
            state.shrinkPending = true;
            state.shrinkDueTick = now + kGeometryShrinkDelayMs;
            return false;
        }
        if (now < state.shrinkDueTick) {
            return false;
        }
        desiredRects = std::move(state.pendingShrinkRects);
    }

    state.rects = std::move(desiredRects);
    state.initialized = true;
    state.shrinkPending = false;
    state.pendingShrinkRects.clear();
    changed = true;
    PostRegionSync(hWnd, state, true);
    return changed;
}

static bool UpdateWindowRegion(
    IUIAutomation2* automation,
    IUIAutomationCondition* dockCondition,
    IUIAutomationCacheRequest* cacheRequest,
    HWND hWnd,
    const Settings& settings,
    WorkerBuffers& buffers,
    bool& geometryChanged) {
    geometryChanged = false;
    if (StopRequested()) {
        return false;
    }
    const uint64_t topologyRevision =
        g_topologyRevision.load(std::memory_order_acquire);

    RECT physicalRect{};
    bool shown = false;
    if (!QueryTaskbarPhysicalVisibility(hWnd, physicalRect, shown)) {
        PublishFailSafeRegion(hWnd);
        return true;
    }
    if (!shown) {
        const HMONITOR assignedMonitor =
            GetAssignedTaskbarMonitor(hWnd);
        const UINT assignedDpi =
            GetEffectiveMonitorDpi(assignedMonitor);
        MONITORINFO assignedInfo{};
        assignedInfo.cbSize = sizeof(assignedInfo);
        const bool assignedMonitorValid =
            assignedMonitor &&
            GetMonitorInfoW(assignedMonitor, &assignedInfo);

        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto it = g_desired.find(hWnd);
        if (it != g_desired.end()) {
            DesiredState& state = it->second;
            const bool topologyChanged =
                assignedMonitorValid &&
                ((state.monitor &&
                  state.monitor != assignedMonitor) ||
                 (RectValid(state.shownRect) &&
                  !RectContains(assignedInfo.rcMonitor,
                                state.shownRect)) ||
                 (state.hasGeometry && state.dpi &&
                  state.dpi != assignedDpi));
            if (topologyChanged) {
                g_topologyRevision.fetch_add(
                    1, std::memory_order_acq_rel);
                ResetTopologyStateLocked(
                    state, assignedMonitor, assignedDpi, true);
                PostRegionSync(hWnd, state, true);
                geometryChanged = true;
            } else {
                state.hidden = true;
                state.syncPending = false;
            }
            if (!state.hasGeometry && assignedMonitorValid) {
                state.monitor = assignedMonitor;
                state.dpi = assignedDpi;
            }
        }
        return true;
    }

    RECT windowRect{};
    buffers.spans.clear();
    const ComputeDockResult result = ComputeDock(
        automation, dockCondition, cacheRequest, hWnd, settings,
        buffers, windowRect, buffers.spans);
    if (result == ComputeDockResult::Cancelled) {
        return false;
    }
    if (result != ComputeDockResult::Complete) {
        if (result == ComputeDockResult::Incomplete) {
            LogFailureThrottled(L"Cached UI Automation snapshot",
                                ERROR_INVALID_DATA);
        }
        geometryChanged = PublishFailSafeRegion(hWnd);
        return true;
    }

    const LONG windowWidth = windowRect.right - windowRect.left;
    const LONG windowHeight = windowRect.bottom - windowRect.top;
    const UINT dpi = GetTaskbarDpi(hWnd, &windowRect);
    const LONG paddingLeft = static_cast<LONG>(
        MulDiv(settings.horizontalPaddingLeft, static_cast<int>(dpi),
               USER_DEFAULT_SCREEN_DPI));
    const LONG paddingRight = static_cast<LONG>(
        MulDiv(settings.horizontalPaddingRight, static_cast<int>(dpi),
               USER_DEFAULT_SCREEN_DPI));
    const LONG edgeSnap = static_cast<LONG>(
        MulDiv(kEdgeSnapLogicalPx, static_cast<int>(dpi),
               USER_DEFAULT_SCREEN_DPI));

    std::vector<RECT> dockRects;
    dockRects.reserve(buffers.spans.size());
    for (size_t i = 0; i < buffers.spans.size(); i++) {
        const HorizontalSpan& span = buffers.spans[i];
        const LONG leftPadding = i == 0 ? paddingLeft : 0;
        const LONG rightPadding =
            i + 1 == buffers.spans.size() ? paddingRight : 0;
        LONG left = ClampToLong(
            static_cast<int64_t>(span.left) - windowRect.left -
            leftPadding);
        LONG right = ClampToLong(
            static_cast<int64_t>(span.right) - windowRect.left +
            rightPadding);
        left = std::clamp(left, static_cast<LONG>(0), windowWidth);
        right = std::clamp(right, static_cast<LONG>(0), windowWidth);
        if (left <= edgeSnap) {
            left = 0;
        }
        if (right >= windowWidth - edgeSnap) {
            right = windowWidth;
        }
        if (right > left) {
            dockRects.push_back({left, 0, right, windowHeight});
        }
    }
    NormalizeRegionRects(dockRects);
    if (dockRects.empty()) {
        geometryChanged = PublishFailSafeRegion(hWnd);
        return true;
    }

    std::vector<RECT> desiredRects = dockRects;
    if (desiredRects.size() == 1 &&
        desiredRects[0].left == 0 &&
        desiredRects[0].right == windowWidth) {
        desiredRects.clear();
    }

    geometryChanged = PublishDesiredRegion(
        hWnd, std::move(desiredRects), std::move(dockRects),
        windowRect, dpi, topologyRevision);
    return true;
}

static bool EnumTaskbars(std::vector<HWND>& taskbars) {
    taskbars.clear();
    if (EnumWindows(
            [](HWND hWnd, LPARAM parameter) -> BOOL {
                if (IsTaskbarWindow(hWnd)) {
                    reinterpret_cast<std::vector<HWND>*>(parameter)
                        ->push_back(hWnd);
                }
                return TRUE;
            },
            reinterpret_cast<LPARAM>(&taskbars))) {
        return true;
    }
    LogFailureThrottled(L"EnumWindows(taskbars)", GetLastError());
    return false;
}

static HWND ResolveTaskbarForEvent(HWND eventWindow) {
    if (!eventWindow) {
        return nullptr;
    }
    HWND root = GetAncestor(eventWindow, GA_ROOT);
    if (IsTaskbarWindow(root)) {
        return root;
    }
    HWND current = GetParent(eventWindow);
    for (int depth = 0; current && depth < 12; depth++) {
        if (IsTaskbarWindow(current)) {
            return current;
        }
        HWND parent = GetParent(current);
        if (!parent || parent == current) {
            break;
        }
        current = parent;
    }
    return nullptr;
}

static void WakeWorker() {
    if (g_refreshEvent) {
        SetEvent(g_refreshEvent);
    }
}

static void SignalRefresh(bool geometry, bool policy) {
    if (geometry) {
        g_geometryDirty.store(true, std::memory_order_release);
    }
    if (policy) {
        g_policyDirty.store(true, std::memory_order_release);
    }
    WakeWorker();
}

static void MarkStructureRefresh(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_stateMutex);
    auto it = g_desired.find(hWnd);
    if (it != g_desired.end()) {
        it->second.structureRefreshPending = true;
    }
}

static void CALLBACK GeometryWinEventProc(HWINEVENTHOOK,
                                          DWORD event,
                                          HWND hWnd,
                                          LONG objectId,
                                          LONG childId,
                                          DWORD,
                                          DWORD) {
    if (StopRequested()) {
        return;
    }

    const bool rootWindowEvent =
        objectId == OBJID_WINDOW && childId == CHILDID_SELF &&
        hWnd && GetAncestor(hWnd, GA_ROOT) == hWnd;
    if (event == EVENT_OBJECT_LOCATIONCHANGE) {
        if (rootWindowEvent && IsTaskbarWindow(hWnd)) {
            RECT rect{};
            bool shown = false;
            if (!QueryTaskbarPhysicalVisibility(
                    hWnd, rect, shown) ||
                !shown) {
                return;
            }
            const HMONITOR monitor =
                GetAssignedTaskbarMonitor(hWnd);
            const UINT dpi = GetTaskbarDpi(hWnd, &rect);
            bool geometryChanged = true;
            {
                std::lock_guard<std::mutex> lock(g_stateMutex);
                auto it = g_desired.find(hWnd);
                if (it != g_desired.end() &&
                    it->second.measuredSize.cx > 0) {
                    const DesiredState& state = it->second;
                    geometryChanged =
                        rect.right - rect.left !=
                            state.measuredSize.cx ||
                        rect.bottom - rect.top !=
                            state.measuredSize.cy ||
                        !RectEqual(rect, state.shownRect) ||
                        (state.monitor && monitor &&
                         state.monitor != monitor) ||
                        (state.dpi && dpi && state.dpi != dpi);
                }
            }
            if (geometryChanged) {
                InvalidateShownGeometry(
                    hWnd, rect, monitor, dpi);
                SignalRefresh(true, true);
            }
            return;
        }
        if (rootWindowEvent) {
            SignalRefresh(false, true);
        }
        return;
    }

    if (rootWindowEvent && !IsTaskbarWindow(hWnd)) {
        SignalRefresh(false, true);
        return;
    }

    if (event != EVENT_OBJECT_REORDER &&
        (objectId != OBJID_WINDOW || childId != CHILDID_SELF)) {
        return;
    }

    HWND taskbar = ResolveTaskbarForEvent(hWnd);
    if (!taskbar) {
        return;
    }
    if (event == EVENT_OBJECT_CREATE ||
        event == EVENT_OBJECT_DESTROY ||
        event == EVENT_OBJECT_REORDER) {
        // Keep the last-good input boundary while taskbar content changes.
        // The worker may scan once during pointer interaction so additions
        // and removals catch up without making the whole taskbar clickable.
        MarkStructureRefresh(taskbar);
    }
    SignalRefresh(true, true);
}

static void CALLBACK ForegroundWinEventProc(HWINEVENTHOOK,
                                            DWORD,
                                            HWND,
                                            LONG,
                                            LONG,
                                            DWORD,
                                            DWORD) {
    if (!StopRequested()) {
        SignalRefresh(false, true);
    }
}

static bool PointerInteractingWithTaskbar() {
    POINT cursor{};
    if (!GetCursorPos(&cursor)) {
        return false;
    }
    HWND hovered = WindowFromPoint(cursor);
    return hovered && ResolveTaskbarForEvent(hovered);
}

static bool NeedsShownGeometryWork() {
    std::lock_guard<std::mutex> lock(g_stateMutex);
    return std::any_of(
        g_desired.begin(), g_desired.end(),
        [](const auto& item) {
            const DesiredState& state = item.second;
            return !state.hidden &&
                   (!state.hasGeometry || state.shrinkPending ||
                    state.structureRefreshPending);
        });
}

static bool AutoHideAppliesLocked(HWND hWnd) {
    const auto state = g_desired.find(hWnd);
    return NativeAutoHideDispatchEnabled() &&
           g_windowsAutoHideEnabled.load(std::memory_order_acquire) &&
           g_taskbarViewHooksInstalled.load(std::memory_order_acquire) &&
           (state == g_desired.end() ||
            state->second.nativeAutoHideAvailable) &&
           (!g_settings.primaryMonitorOnly ||
            IsPrimaryTaskbarWindow(hWnd));
}

static bool ReadWindowsAutoHideEnabled() {
    APPBARDATA data{};
    data.cbSize = sizeof(data);
    return (SHAppBarMessage(ABM_GETSTATE, &data) & ABS_AUTOHIDE) != 0;
}

static bool EqualsOrdinalInsensitive(std::wstring_view left,
                                     std::wstring_view right) {
    return left.size() == right.size() &&
           CompareStringOrdinal(
               left.data(), static_cast<int>(left.size()),
               right.data(), static_cast<int>(right.size()), TRUE) ==
               CSTR_EQUAL;
}

static bool MatchesExcludedIdentifier(
    std::wstring_view value,
    const std::vector<std::wstring>& excludedPrograms) {
    if (value.empty()) {
        return false;
    }
    return std::any_of(
        excludedPrograms.begin(), excludedPrograms.end(),
        [value](const std::wstring& excluded) {
            return EqualsOrdinalInsensitive(value, excluded);
        });
}

static bool IsWindowExcluded(
    HWND hWnd,
    const std::vector<std::wstring>& excludedPrograms) {
    if (excludedPrograms.empty()) {
        return false;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hWnd, &processId);
    if (processId) {
        HANDLE process = OpenProcess(
            PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
        if (process) {
            thread_local std::vector<WCHAR> pathBuffer(32768);
            DWORD length = static_cast<DWORD>(pathBuffer.size());
            if (QueryFullProcessImageNameW(
                    process, 0, pathBuffer.data(), &length) &&
                length) {
                const std::wstring_view path(
                    pathBuffer.data(), length);
                const size_t separator = path.find_last_of(L"\\/");
                const std::wstring_view fileName =
                    separator == std::wstring::npos
                        ? path
                        : path.substr(separator + 1);
                if (MatchesExcludedIdentifier(
                        path, excludedPrograms) ||
                    MatchesExcludedIdentifier(
                        fileName, excludedPrograms)) {
                    CloseHandle(process);
                    return true;
                }
            }
            CloseHandle(process);
        }
    }

    IPropertyStore* store = nullptr;
    if (FAILED(SHGetPropertyStoreForWindow(
            hWnd, IID_PPV_ARGS(&store))) ||
        !store) {
        return false;
    }
    PROPVARIANT value{};
    const HRESULT result =
        store->GetValue(kAppUserModelIdKey, &value);
    const bool excluded =
        SUCCEEDED(result) && value.vt == VT_LPWSTR && value.pwszVal &&
        MatchesExcludedIdentifier(
            value.pwszVal, excludedPrograms);
    PropVariantClear(&value);
    store->Release();
    return excluded;
}

static bool IsEligibleApplicationWindow(HWND hWnd) {
    if (!hWnd || !IsWindowVisible(hWnd) || hWnd == GetShellWindow() ||
        IsIconic(hWnd)) {
        return false;
    }
    const LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) {
        return false;
    }

    WCHAR className[48]{};
    GetClassNameW(hWnd, className, ARRAYSIZE(className));
    if (wcscmp(className, L"Shell_TrayWnd") == 0 ||
        wcscmp(className, L"Shell_SecondaryTrayWnd") == 0 ||
        wcscmp(className, L"Progman") == 0 ||
        wcscmp(className, L"WorkerW") == 0 ||
        wcscmp(className, L"#32768") == 0) {
        return false;
    }

    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(
            hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
        cloaked) {
        return false;
    }
    return true;
}

static bool GetVisibleWindowBounds(HWND hWnd, RECT& bounds) {
    if (SUCCEEDED(DwmGetWindowAttribute(
            hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &bounds,
            sizeof(bounds))) &&
        RectValid(bounds)) {
        return true;
    }
    return GetWindowRect(hWnd, &bounds) && RectValid(bounds);
}

static bool ForegroundWindowSuppressesEdgeReveal(HWND taskbar) {
    const HWND foreground = GetForegroundWindow();
    if (!foreground || foreground == taskbar ||
        !IsEligibleApplicationWindow(foreground)) {
        return false;
    }

    const HMONITOR taskbarMonitor =
        GetAssignedTaskbarMonitor(taskbar);
    const HMONITOR foregroundMonitor =
        MonitorFromWindow(foreground, MONITOR_DEFAULTTONEAREST);
    if (!taskbarMonitor || foregroundMonitor != taskbarMonitor) {
        return false;
    }

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    RECT foregroundRect{};
    WINDOWPLACEMENT placement{};
    placement.length = sizeof(placement);
    if (!GetMonitorInfoW(taskbarMonitor, &monitorInfo) ||
        !GetVisibleWindowBounds(foreground, foregroundRect) ||
        !GetWindowPlacement(foreground, &placement)) {
        return false;
    }

    return IsFullscreenWindowShapeForReveal(
        GetWindowLongPtrW(foreground, GWL_STYLE),
        placement.showCmd, foregroundRect, monitorInfo.rcMonitor);
}

struct PolicyTarget {
    HWND taskbar = nullptr;
    HMONITOR monitor = nullptr;
    RECT shownRect{};
    std::vector<RECT> dockRects;
    bool hasGeometry = false;
    bool match = false;
};

struct PolicyEnumContext {
    std::vector<PolicyTarget>* targets;
    const Settings* settings;
    size_t unmatched;
};

static BOOL CALLBACK EnumPolicyWindowsProc(HWND hWnd, LPARAM parameter) {
    auto* context =
        reinterpret_cast<PolicyEnumContext*>(parameter);
    if (!context->unmatched) {
        return FALSE;
    }
    if (context->settings->autoHideMode ==
        AutoHideMode::Maximized) {
        WINDOWPLACEMENT placement{};
        placement.length = sizeof(placement);
        if (!GetWindowPlacement(hWnd, &placement) ||
            placement.showCmd != SW_SHOWMAXIMIZED) {
            return TRUE;
        }
    }
    if (!IsEligibleApplicationWindow(hWnd) ||
        IsWindowExcluded(
            hWnd, context->settings->excludedPrograms)) {
        return TRUE;
    }

    RECT windowRect{};
    HMONITOR windowMonitor = nullptr;
    if (context->settings->autoHideMode ==
        AutoHideMode::Maximized) {
        windowMonitor =
            MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    } else if (!GetVisibleWindowBounds(hWnd, windowRect)) {
        return TRUE;
    }

    for (PolicyTarget& target : *context->targets) {
        if (target.match || !target.hasGeometry) {
            continue;
        }
        bool matches = false;
        if (context->settings->autoHideMode ==
            AutoHideMode::Maximized) {
            matches = windowMonitor == target.monitor;
        } else {
            for (const RECT& local : target.dockRects) {
                RECT dock{
                    target.shownRect.left + local.left,
                    target.shownRect.top + local.top,
                    target.shownRect.left + local.right,
                    target.shownRect.top + local.bottom};
                if (RectIntersects(windowRect, dock)) {
                    matches = true;
                    break;
                }
            }
        }
        if (matches) {
            target.match = true;
            context->unmatched--;
        }
    }
    return context->unmatched ? TRUE : FALSE;
}

static void EvaluateWindowPolicy(const Settings& settings) {
    std::vector<PolicyTarget> targets;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        targets.reserve(g_desired.size());
        for (const auto& [hWnd, state] : g_desired) {
            if (settings.primaryMonitorOnly &&
                !IsPrimaryTaskbarWindow(hWnd)) {
                continue;
            }
            PolicyTarget target;
            target.taskbar = hWnd;
            target.shownRect = state.shownRect;
            target.dockRects = state.dockRects;
            target.hasGeometry =
                !state.dockRects.empty() &&
                RectValid(state.shownRect);
            if (target.hasGeometry) {
                target.monitor = MonitorFromRect(
                    &target.shownRect, MONITOR_DEFAULTTONEAREST);
            }
            if (!target.monitor) {
                target.monitor = state.monitor;
            }
            if (!target.monitor && IsPrimaryTaskbarWindow(hWnd)) {
                POINT unused{};
                target.monitor = MonitorFromPoint(
                    unused, MONITOR_DEFAULTTOPRIMARY);
            }
            if (!target.monitor) {
                target.monitor = MonitorFromWindow(
                    hWnd, MONITOR_DEFAULTTONEAREST);
            }
            target.match =
                settings.autoHideMode == AutoHideMode::Always;
            targets.push_back(std::move(target));
        }
    }

    if (settings.autoHideMode != AutoHideMode::Always) {
        PolicyEnumContext context{
            &targets, &settings, 0};
        for (const PolicyTarget& target : targets) {
            if (target.hasGeometry && !target.match) {
                context.unmatched++;
            }
        }
        if (context.unmatched) {
            if (settings.foregroundOnly) {
                EnumPolicyWindowsProc(
                    GetForegroundWindow(),
                    reinterpret_cast<LPARAM>(&context));
            } else {
                EnumWindows(
                    EnumPolicyWindowsProc,
                    reinterpret_cast<LPARAM>(&context));
            }
        }
    }

    std::lock_guard<std::mutex> lock(g_stateMutex);
    if (settings.revision != g_settings.revision) {
        return;
    }
    for (const PolicyTarget& target : targets) {
        auto it = g_desired.find(target.taskbar);
        if (it != g_desired.end()) {
            it->second.policyMatch = target.match;
        }
    }
}

static void RecordTaskbarNotification(HWND hWnd) {
    const ULONGLONG now = GetTickCount64();
    std::lock_guard<std::mutex> lock(g_stateMutex);
    auto it = g_desired.find(hWnd);
    if (it == g_desired.end()) {
        return;
    }

    DesiredState& state = it->second;
    const ULONGLONG quietMs = std::max<ULONGLONG>(
        kNotificationBurstQuietMs, g_settings.notificationHoldMs);
    if (!state.notificationLastTick ||
        now < state.notificationLastTick ||
        now - state.notificationLastTick >= quietMs) {
        state.notificationBurstStartTick = now;
    }
    state.notificationLastTick = now;
}

static bool NotificationHoldActive(const Settings& settings,
                                   const DesiredState& state,
                                   ULONGLONG now) {
    const ULONGLONG start = state.notificationBurstStartTick;
    return settings.notificationHoldMs && start && now >= start &&
           now - start < settings.notificationHoldMs;
}

static void UpdatePolicyTransitions(const Settings& settings) {
    const ULONGLONG now = GetTickCount64();
    std::vector<HWND> due;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        for (auto& [hWnd, state] : g_desired) {
            if (!AutoHideAppliesLocked(hWnd)) {
                state.keepShown = false;
                state.policyInitialized = false;
                state.pendingTransition = 0;
                state.transitionDueTick = 0;
                state.notificationBurstStartTick = 0;
                state.notificationLastTick = 0;
                continue;
            }

            // Until the first visible measurement, leave native auto-hide
            // untouched. A user reveal supplies geometry without creating a
            // competing show/hide loop.
            const bool notificationActive =
                NotificationHoldActive(settings, state, now);
            const bool forceShown =
                notificationActive ||
                (!state.dockRects.empty() &&
                 RectValid(state.shownRect) &&
                 settings.autoHideMode != AutoHideMode::Always &&
                 !state.policyMatch);
            if (!state.policyInitialized ||
                forceShown != state.keepShown) {
                state.policyInitialized = true;
                state.keepShown = forceShown;
                if (!forceShown) {
                    state.dockPointerInside = false;
                }
                state.pendingTransition = forceShown ? 1 : 2;
                state.transitionDueTick =
                    now + (forceShown ? settings.showDelayMs : 0);
            }
            if (state.pendingTransition &&
                now >= state.transitionDueTick) {
                due.push_back(hWnd);
                state.pendingTransition = 0;
                state.transitionDueTick = 0;
            }
        }
    }

    for (HWND hWnd : due) {
        if (!PostMessageW(hWnd, g_taskbarControlMsg,
                          kTaskbarCommandPolicy, 0)) {
            LogFailureThrottled(
                L"PostMessageW(auto-hide policy)",
                GetLastError());
        }
    }
}

static bool GetPolicyCommand(HWND hWnd, bool& show) {
    std::lock_guard<std::mutex> lock(g_stateMutex);
    auto it = g_desired.find(hWnd);
    if (it == g_desired.end() || !AutoHideAppliesLocked(hWnd)) {
        return false;
    }
    show = it->second.keepShown;
    return true;
}

static bool ShouldKeepShownForObject(void* object, HWND& hWnd) {
    std::lock_guard<std::mutex> lock(g_stateMutex);
    auto objectIt = g_nativeTaskbarObjects.find(object);
    if (objectIt == g_nativeTaskbarObjects.end()) {
        return false;
    }
    hWnd = objectIt->second;
    auto stateIt = g_desired.find(hWnd);
    return stateIt != g_desired.end() &&
           AutoHideAppliesLocked(hWnd) &&
           stateIt->second.keepShown;
}

static HWND TaskbarForNativeObject(void* object) {
    std::lock_guard<std::mutex> lock(g_stateMutex);
    auto it = g_nativeTaskbarObjects.find(object);
    return it != g_nativeTaskbarObjects.end() ? it->second : nullptr;
}

enum class ExpansionOverride {
    Native,
    Show,
};

static ExpansionOverride RememberViewCoordinator(
    HWND hWnd,
    void* coordinator) {
    if (!coordinator || !IsTaskbarWindow(hWnd)) {
        return ExpansionOverride::Native;
    }
    std::lock_guard<std::mutex> lock(g_stateMutex);
    g_viewCoordinators[hWnd] = coordinator;
    auto state = g_desired.find(hWnd);
    if (state == g_desired.end() ||
        !AutoHideAppliesLocked(hWnd)) {
        return ExpansionOverride::Native;
    }
    if (state->second.keepShown || g_revealUpdateActive) {
        return ExpansionOverride::Show;
    }
    return ExpansionOverride::Native;
}

// Runs on the taskbar UI thread after a keep-shown policy transition.
static void UpdateViewCoordinator(HWND hWnd,
                                  bool allowReveal = false) {
    void* coordinator = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto it = g_viewCoordinators.find(hWnd);
        if (it != g_viewCoordinators.end()) {
            coordinator = it->second;
        }
    }
    if (coordinator && g_viewCoordinatorUpdateExpandedOriginal) {
        const bool previousRevealUpdate = g_revealUpdateActive;
        g_revealUpdateActive = previousRevealUpdate || allowReveal;
        g_viewCoordinatorUpdateExpandedOriginal(
            coordinator, hWnd, kViewReasonPointerOverChanged);
        g_revealUpdateActive = previousRevealUpdate;
    }
}

// Runs on the taskbar UI thread when the pointer crosses a dock boundary.
static bool NotifyDockPointerState(HWND hWnd, bool inside) {
    void* coordinator = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto it = g_viewCoordinators.find(hWnd);
        if (it != g_viewCoordinators.end()) {
            coordinator = it->second;
        }
    }
    if (!coordinator || !g_viewCoordinatorPointerChangedOriginal) {
        return false;
    }
    g_viewCoordinatorPointerChangedOriginal(
        coordinator, hWnd, inside, 0);
    return true;
}

static bool CursorInActivationArea(HWND hWnd) {
    std::array<RECT, kMaxRegions> dockRects{};
    RECT shownRect{};
    HMONITOR monitor = nullptr;
    UINT dpi = USER_DEFAULT_SCREEN_DPI;
    size_t dockCount = 0;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (!AutoHideAppliesLocked(hWnd)) {
            return true;
        }
        auto it = g_desired.find(hWnd);
        if (it == g_desired.end() || !it->second.hidden) {
            return true;
        }
        if (!g_settings.edgeReveal) {
            return false;
        }
        const DesiredState& state = it->second;
        shownRect = state.shownRect;
        monitor = state.monitor;
        dpi = state.dpi ? state.dpi : USER_DEFAULT_SCREEN_DPI;
        if (state.hasGeometry) {
            dockCount = std::min(
                state.dockRects.size(), dockRects.size());
            std::copy_n(
                state.dockRects.begin(), dockCount,
                dockRects.begin());
        }
    }

    const ScopedPhysicalCoordinates coordinates;
    if (!coordinates) {
        return false;
    }

    POINT cursor{};
    if (!GetCursorPos(&cursor)) {
        return false;
    }
    if (!monitor && IsPrimaryTaskbarWindow(hWnd)) {
        POINT unused{};
        monitor = MonitorFromPoint(
            unused, MONITOR_DEFAULTTOPRIMARY);
    }
    if (!monitor) {
        monitor = MonitorFromWindow(
            hWnd, MONITOR_DEFAULTTONEAREST);
    }
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (!monitor || !GetMonitorInfoW(monitor, &info)) {
        return false;
    }
    const LONG edgeBand = std::max<LONG>(
        1, MulDiv(2, static_cast<int>(
                         dpi),
                  USER_DEFAULT_SCREEN_DPI));
    if (cursor.y < info.rcMonitor.bottom - edgeBand ||
        cursor.y >= info.rcMonitor.bottom) {
        return false;
    }

    if (dockCount && RectValid(shownRect)) {
        for (size_t i = 0; i < dockCount; i++) {
            const RECT& local = dockRects[i];
            const LONG left = shownRect.left + local.left;
            const LONG right = shownRect.left + local.right;
            if (cursor.x >= left && cursor.x < right) {
                return true;
            }
        }
        return false;
    }

    const LONG monitorWidth =
        info.rcMonitor.right - info.rcMonitor.left;
    const LONG bootstrapWidth = std::min<LONG>(
        monitorWidth,
        std::max<LONG>(
            1, MulDiv(kBootstrapRevealLogicalPx,
                      static_cast<int>(dpi),
                      USER_DEFAULT_SCREEN_DPI)));
    const LONG left =
        info.rcMonitor.left + (monitorWidth - bootstrapWidth) / 2;
    return cursor.x >= left && cursor.x < left + bootstrapWidth;
}

static bool ScreenEdgeRevealAllowed(HWND hWnd) {
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (!AutoHideAppliesLocked(hWnd)) {
            return true;
        }
    }
    return !ForegroundWindowSuppressesEdgeReveal(hWnd) &&
           CursorInActivationArea(hWnd);
}

static void UpdateDockPointerTransitions() {
    const ScopedPhysicalCoordinates coordinates;
    POINT cursor{};
    if (!coordinates || !GetCursorPos(&cursor)) {
        return;
    }

    std::vector<std::pair<HWND, bool>> changes;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        for (auto& [hWnd, state] : g_desired) {
            const bool track =
                AutoHideAppliesLocked(hWnd) && !state.hidden &&
                state.hasGeometry && RectValid(state.shownRect);
            if (!track) {
                state.dockPointerInside = false;
                continue;
            }

            const bool inHorizontalPlane =
                cursor.y >= state.shownRect.top &&
                cursor.y < state.shownRect.bottom;
            const bool inside = PointInShownDockArea(state, cursor);
            if (inHorizontalPlane && !state.keepShown &&
                inside != state.dockPointerInside) {
                changes.emplace_back(hWnd, inside);
            }
            state.dockPointerInside = inside;
        }
    }

    for (const auto& [hWnd, inside] : changes) {
        if (!PostMessageW(hWnd, g_taskbarControlMsg,
                          kTaskbarCommandDockPointer,
                          static_cast<LPARAM>(inside))) {
            LogFailureThrottled(
                L"PostMessageW(dock pointer transition)",
                GetLastError());
        }
    }
}

static UINT_PTR WINAPI SetTimerHook(HWND hWnd,
                                    UINT_PTR timerId,
                                    UINT interval,
                                    TIMERPROC timerProc) {
    if (!NativeAutoHideDispatchEnabled()) {
        return g_setTimerOriginal(hWnd, timerId, interval, timerProc);
    }
    if (timerId != kTrayUITimerHide &&
        timerId != kTrayUITimerUnhide) {
        return g_setTimerOriginal(hWnd, timerId, interval, timerProc);
    }

    if (IsTaskbarWindow(hWnd)) {
        if (timerId == kTrayUITimerUnhide &&
            !ScreenEdgeRevealAllowed(hWnd)) {
            return timerId;
        }
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (AutoHideAppliesLocked(hWnd)) {
            interval =
                timerId == kTrayUITimerUnhide
                    ? g_settings.showDelayMs
                    : g_settings.hideDelayMs;
        }
    }
    return g_setTimerOriginal(hWnd, timerId, interval, timerProc);
}

static void ArmDockMouseLeaveWake(HWND hWnd) {
    TRACKMOUSEEVENT tracking{
        sizeof(tracking), TME_LEAVE, hWnd, HOVER_DEFAULT};
    if (!TrackMouseEvent(&tracking)) {
        LogFailureThrottled(
            L"TrackMouseEvent(dock leave wake)", GetLastError());
    }
}

static void HandleDockExitHideTimer(HWND hWnd) {
    KillTimer(hWnd, kDockExitHideTimer);
    if (StopRequested()) {
        return;
    }

    const ScopedPhysicalCoordinates coordinates;
    POINT cursor{};
    if (!coordinates || !GetCursorPos(&cursor)) {
        return;
    }

    void* taskbarObject = nullptr;
    bool reentered = false;
    const bool primary = IsPrimaryTaskbarWindow(hWnd);
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto state = g_desired.find(hWnd);
        if (state == g_desired.end() ||
            !AutoHideAppliesLocked(hWnd) || state->second.keepShown ||
            state->second.hidden) {
            return;
        }
        if (PointInShownDockArea(state->second, cursor)) {
            if (!state->second.dockPointerInside) {
                state->second.dockPointerInside = true;
                reentered = true;
            } else {
                return;
            }
        } else if (state->second.dockPointerInside) {
            return;
        }
        if (!reentered) {
            auto object = std::find_if(
                g_nativeTaskbarObjects.begin(),
                g_nativeTaskbarObjects.end(),
                [hWnd](const auto& item) {
                    return item.second == hWnd;
                });
            if (object != g_nativeTaskbarObjects.end()) {
                taskbarObject = object->first;
            }
        }
    }

    if (reentered) {
        NotifyDockPointerState(hWnd, true);
        ArmDockMouseLeaveWake(hWnd);
        return;
    }

    if (!taskbarObject) {
        return;
    }
    if (primary) {
        g_trayUIHideOriginal(taskbarObject);
    } else {
        g_secondaryTrayAutoHideOriginal(taskbarObject, true);
    }
}

static bool IsReadablePointerSlot(const void* address) {
    MEMORY_BASIC_INFORMATION memory{};
    if (!address || !VirtualQuery(address, &memory, sizeof(memory)) ||
        memory.State != MEM_COMMIT ||
        (memory.Protect & (PAGE_GUARD | PAGE_NOACCESS))) {
        return false;
    }

    const DWORD baseProtection = memory.Protect & 0xFF;
    if (baseProtection != PAGE_READONLY &&
        baseProtection != PAGE_READWRITE &&
        baseProtection != PAGE_WRITECOPY &&
        baseProtection != PAGE_EXECUTE_READ &&
        baseProtection != PAGE_EXECUTE_READWRITE &&
        baseProtection != PAGE_EXECUTE_WRITECOPY) {
        return false;
    }

    const uintptr_t slot = reinterpret_cast<uintptr_t>(address);
    const uintptr_t regionEnd =
        reinterpret_cast<uintptr_t>(memory.BaseAddress) + memory.RegionSize;
    return slot <= regionEnd && regionEnd - slot >= sizeof(void*);
}

static void* QueryViaVtable(void* object, void* vtable) {
    if (!object || !vtable) {
        return nullptr;
    }

    const uintptr_t base = reinterpret_cast<uintptr_t>(object);
    for (size_t i = 0; i < kMaxVtableSearchSlots; i++) {
        auto* slot = reinterpret_cast<void**>(base + i * sizeof(void*));
        if (!IsReadablePointerSlot(slot)) {
            break;
        }
        if (*slot == vtable) {
            return slot;
        }
    }
    return nullptr;
}

static void* QueryViaVtableBackwards(void* object, void* vtable) {
    if (!object || !vtable) {
        return nullptr;
    }

    const uintptr_t base = reinterpret_cast<uintptr_t>(object);
    for (size_t i = 0; i < kMaxVtableSearchSlots; i++) {
        const uintptr_t offset = i * sizeof(void*);
        if (offset > base) {
            break;
        }
        auto* slot = reinterpret_cast<void**>(base - offset);
        if (!IsReadablePointerSlot(slot)) {
            break;
        }
        if (*slot == vtable) {
            return slot;
        }
    }
    return nullptr;
}

static void MarkNativeTaskbarUnavailable(HWND hWnd, PCWSTR reason) {
    bool changed = false;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        auto state = g_desired.find(hWnd);
        if (state != g_desired.end()) {
            changed = state->second.nativeAutoHideAvailable;
            state->second.nativeBindingPending = false;
            state->second.nativeAutoHideAvailable = false;
            state->second.pendingTransition = 0;
            state->second.transitionDueTick = 0;
        }
    }
    if (changed) {
        Wh_Log(L"%s; leaving this taskbar under native auto-hide", reason);
    }
    WakeWorker();
}

static void RememberNativeTaskbarState(void* object,
                                       HWND hWnd,
                                       HMONITOR monitor) {
    const UINT dpi = GetEffectiveMonitorDpi(monitor);
    bool monitorChanged = false;
    bool geometryReset = false;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        std::erase_if(g_nativeTaskbarObjects,
                      [object, hWnd](const auto& item) {
                          return item.first != object &&
                                 item.second == hWnd;
                      });
        g_nativeTaskbarObjects[object] = hWnd;
        auto it = g_desired.find(hWnd);
        if (it != g_desired.end()) {
            it->second.nativeBindingPending = false;
            if (monitor && it->second.monitor != monitor) {
                monitorChanged = true;
                if (it->second.monitor) {
                    g_topologyRevision.fetch_add(
                        1, std::memory_order_acq_rel);
                    const bool hidden = it->second.hidden;
                    ResetTopologyStateLocked(
                        it->second, monitor, dpi, hidden);
                    geometryReset = true;
                } else {
                    it->second.monitor = monitor;
                    it->second.dpi = dpi;
                }
            }
        }
    }
    if (geometryReset) {
        ClearWindowRegion(hWnd);
    }
    if (monitorChanged) {
        SignalRefresh(true, true);
    }
}

static void RememberPrimaryNativeState(void* pThis, HWND hWnd) {
    void* inspectable = QueryViaVtableBackwards(
        pThis, g_trayUIVtableInspectable);
    if (!inspectable) {
        LogFailureThrottled(L"TrayUI IInspectable lookup", ERROR_NOT_FOUND);
        MarkNativeTaskbarUnavailable(
            hWnd, L"TrayUI IInspectable lookup failed");
        return;
    }
    RememberNativeTaskbarState(
        inspectable, hWnd,
        g_trayUIGetStuckMonitorOriginal(pThis));
}

static void RememberSecondaryNativeState(void* pThis, HWND hWnd) {
    void* secondaryTray =
        QueryViaVtable(pThis, g_secondaryTrayVtable);
    if (!secondaryTray) {
        LogFailureThrottled(L"CSecondaryTray lookup", ERROR_NOT_FOUND);
        MarkNativeTaskbarUnavailable(
            hWnd, L"CSecondaryTray lookup failed");
        return;
    }
    RememberNativeTaskbarState(
        pThis, hWnd,
        g_secondaryTrayGetMonitorOriginal(secondaryTray));
}

static void RestoreTaskbarRevealZOrder(HWND hWnd) {
    if (!IsTaskbarWindow(hWnd) ||
        ForegroundWindowSuppressesEdgeReveal(hWnd)) {
        return;
    }
    if (!SetWindowPos(
            hWnd, HWND_TOPMOST, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                SWP_NOOWNERZORDER | SWP_NOSENDCHANGING)) {
        LogFailureThrottled(
            L"SetWindowPos(taskbar reveal z-order)",
            GetLastError());
    }
}

static void ScheduleDockExitHide(HWND hWnd) {
    const Settings settings = GetSettingsSnapshot();
    const SetTimer_t setTimer =
        g_setTimerOriginal ? g_setTimerOriginal : SetTimer;
    KillTimer(hWnd, kDockExitHideTimer);
    if (!setTimer(hWnd, kDockExitHideTimer, settings.hideDelayMs,
                  nullptr)) {
        LogFailureThrottled(
            L"SetTimer(dock pointer exit)", GetLastError());
    }
}

static void ProcessDockPointerCommand(HWND hWnd, bool inside) {
    bool show = false;
    if (!GetPolicyCommand(hWnd, show)) {
        return;
    }
    if (!show) {
        NotifyDockPointerState(hWnd, inside);
        if (inside) {
            KillTimer(hWnd, kDockExitHideTimer);
            ArmDockMouseLeaveWake(hWnd);
        } else {
            ScheduleDockExitHide(hWnd);
        }
    }
}

static void ProcessPrimaryPolicyCommand(void* pThis, HWND hWnd) {
    bool show = false;
    if (!GetPolicyCommand(hWnd, show)) {
        return;
    }
    if (show) {
        void* componentHost =
            QueryViaVtable(pThis, g_trayUIVtableComponentHost);
        if (!componentHost) {
            LogFailureThrottled(
                L"TrayUI ITrayComponentHost lookup", ERROR_NOT_FOUND);
            MarkNativeTaskbarUnavailable(
                hWnd, L"TrayUI ITrayComponentHost lookup failed");
            return;
        }
        g_trayUIUnhideOriginal(
            componentHost, 0, kTrayUnhideRequestDefault);
        RestoreTaskbarRevealZOrder(hWnd);
    } else {
        // Intentionally enter SetTimerHook so the configured hide delay is
        // applied to this native policy transition.
        SetTimer(hWnd, kTrayUITimerHide, 0, nullptr);
    }
    UpdateViewCoordinator(hWnd);
}

static void ProcessSecondaryPolicyCommand(void* pThis, HWND hWnd) {
    bool show = false;
    if (!GetPolicyCommand(hWnd, show)) {
        return;
    }
    if (show) {
        g_secondaryTrayUnhideOriginal(
            pThis, 0, kTrayUnhideRequestDefault);
        RestoreTaskbarRevealZOrder(hWnd);
    } else {
        // Intentionally enter SetTimerHook so the configured hide delay is
        // applied to this native policy transition.
        SetTimer(hWnd, kTrayUITimerHide, 0, nullptr);
    }
    UpdateViewCoordinator(hWnd);
}

static void WINAPI TrayUIHideHook(void* pThis) {
    if (!NativeAutoHideDispatchEnabled()) {
        g_trayUIHideOriginal(pThis);
        return;
    }
    HWND hWnd = nullptr;
    if (ShouldKeepShownForObject(pThis, hWnd)) {
        KillTimer(hWnd, kTrayUITimerHide);
        return;
    }
    g_trayUIHideOriginal(pThis);
}

static void WINAPI TrayUIUnhideHook(void* pThis,
                                    int flags,
                                    int request) {
    g_trayUIUnhideOriginal(pThis, flags, request);
    if (!NativeAutoHideDispatchEnabled()) {
        return;
    }
    HWND hWnd = FindWindowW(L"Shell_TrayWnd", nullptr);
    bool unused = false;
    if (hWnd && GetPolicyCommand(hWnd, unused)) {
        RestoreTaskbarRevealZOrder(hWnd);
        UpdateViewCoordinator(hWnd, true);
    }
}

static void WINAPI SecondaryTrayAutoHideHook(void* pThis, bool value) {
    if (!NativeAutoHideDispatchEnabled()) {
        g_secondaryTrayAutoHideOriginal(pThis, value);
        return;
    }
    HWND hWnd = nullptr;
    if (ShouldKeepShownForObject(pThis, hWnd)) {
        KillTimer(hWnd, kTrayUITimerHide);
        return;
    }
    g_secondaryTrayAutoHideOriginal(pThis, value);
}

static void WINAPI SecondaryTrayUnhideHook(void* pThis,
                                           int flags,
                                           int request) {
    g_secondaryTrayUnhideOriginal(pThis, flags, request);
    if (!NativeAutoHideDispatchEnabled()) {
        return;
    }
    HWND hWnd = TaskbarForNativeObject(pThis);
    bool unused = false;
    if (hWnd && GetPolicyCommand(hWnd, unused)) {
        RestoreTaskbarRevealZOrder(hWnd);
        UpdateViewCoordinator(hWnd, true);
    }
}

static std::vector<RECT> BootstrapRegion(HWND hWnd) {
    const ScopedPhysicalCoordinates coordinates;
    if (!coordinates) {
        return {};
    }

    RECT rect{};
    if (!GetWindowRect(hWnd, &rect) || !RectValid(rect)) {
        return {};
    }
    const LONG width = rect.right - rect.left;
    const LONG height = rect.bottom - rect.top;
    const UINT dpi = GetTaskbarDpi(hWnd);
    return {MakeBootstrapRegion(width, height, dpi)};
}

static void CheckTaskbarVisibilityTransitions() {
    std::vector<std::pair<HWND, bool>> tracked;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        tracked.reserve(g_desired.size());
        for (const auto& [hWnd, state] : g_desired) {
            tracked.emplace_back(hWnd, state.hidden);
        }
    }

    bool changed = false;
    for (const auto& [hWnd, wasHidden] : tracked) {
        RECT rect{};
        bool shown = false;
        if (!QueryTaskbarPhysicalVisibility(hWnd, rect, shown) ||
            shown == !wasHidden) {
            continue;
        }

        std::vector<RECT> bootstrap;
        HMONITOR monitor = nullptr;
        if (shown) {
            bootstrap = BootstrapRegion(hWnd);
            monitor = MonitorFromRect(
                &rect, MONITOR_DEFAULTTONEAREST);
        }
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            auto it = g_desired.find(hWnd);
            if (it == g_desired.end() ||
                it->second.hidden != wasHidden) {
                continue;
            }
            DesiredState& state = it->second;
            state.hidden = !shown;
            state.syncPending = false;
            if (shown) {
                state.shownRect = rect;
                state.monitor = monitor;
                if (!state.hasGeometry && !bootstrap.empty()) {
                    state.rects = std::move(bootstrap);
                    state.initialized = true;
                }
                PostRegionSync(hWnd, state, true);
            }
        }
        changed = true;
    }
    if (changed) {
        SignalRefresh(true, true);
    }
}

static void WINAPI TrayUISlideWindowHook(void* pThis,
                                         HWND hWnd,
                                         const RECT* rect,
                                         HMONITOR monitor,
                                         bool show,
                                         bool animate) {
    if (!NativeAutoHideDispatchEnabled()) {
        g_trayUISlideWindowOriginal(
            pThis, hWnd, rect, monitor, show, animate);
        return;
    }
    bool nativeAnimation = animate;
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        if (AutoHideAppliesLocked(hWnd) &&
            g_settings.animation == AnimationStyle::None) {
            nativeAnimation = false;
        }
    }

    g_trayUISlideWindowOriginal(
        pThis, hWnd, rect, monitor, show, nativeAnimation);

    std::vector<RECT> desired;
    RECT shownRect{};
    bool physicallyShown = false;
    std::vector<RECT> bootstrap;
    if (show) {
        QueryTaskbarPhysicalVisibility(
            hWnd, shownRect, physicallyShown);
        bootstrap = BootstrapRegion(hWnd);
    }
    if (!g_unloading.load(std::memory_order_acquire) &&
        IsTaskbarWindow(hWnd)) {
        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            auto it = g_desired.find(hWnd);
            if (it != g_desired.end()) {
                it->second.hidden = !show;
                it->second.syncPending = false;
                if (monitor) {
                    it->second.monitor = monitor;
                }
                if (show) {
                    if (physicallyShown) {
                        it->second.shownRect = shownRect;
                    }
                    desired = it->second.hasGeometry
                                  ? it->second.rects
                                  : std::move(bootstrap);
                }
            }
        }
        if (show) {
            ApplyDesiredRegion(hWnd, desired);
        }
        SignalRefresh(show, true);
    }
}

static LRESULT WINAPI TrayUIWndProcHook(void* pThis,
                                        HWND hWnd,
                                        UINT message,
                                        WPARAM wParam,
                                        LPARAM lParam,
                                        bool* handled) {
    if (!NativeAutoHideDispatchEnabled()) {
        return g_trayUIWndProcOriginal(
            pThis, hWnd, message, wParam, lParam, handled);
    }
    const bool policyCommand =
        message == g_taskbarControlMsg &&
        wParam == kTaskbarCommandPolicy;
    const bool bindingCommand =
        message == g_taskbarControlMsg &&
        wParam == kTaskbarCommandBindNative;
    const bool dockPointerCommand =
        message == g_taskbarControlMsg &&
        wParam == kTaskbarCommandDockPointer;
    if (message == WM_WINDOWPOSCHANGED || policyCommand ||
        bindingCommand || dockPointerCommand) {
        RememberPrimaryNativeState(pThis, hWnd);
    }
    if (bindingCommand) {
        return 0;
    }

    if (message == kTrayPrivateSettingMessage &&
        wParam == kTrayPrivateSettingAutoHideSet) {
        g_windowsAutoHideEnabled.store(
            static_cast<BOOL>(lParam) != FALSE,
            std::memory_order_release);
        SignalRefresh(false, true);
    } else if (message == g_taskbandNotificationMessage &&
               wParam == kTaskbandNotificationActivate) {
        RecordTaskbarNotification(hWnd);
        WakeWorker();
    } else if (dockPointerCommand) {
        ProcessDockPointerCommand(hWnd, lParam != 0);
        return 0;
    } else if (policyCommand) {
        ProcessPrimaryPolicyCommand(pThis, hWnd);
        return 0;
    }

    const LRESULT result = g_trayUIWndProcOriginal(
        pThis, hWnd, message, wParam, lParam, handled);
    if (message == WM_NCDESTROY) {
        EraseTrackedWindow(hWnd);
    }
    return result;
}

static LRESULT WINAPI SecondaryTrayWndProcHook(void* pThis,
                                               HWND hWnd,
                                               UINT message,
                                               WPARAM wParam,
                                               LPARAM lParam) {
    if (!NativeAutoHideDispatchEnabled()) {
        return g_secondaryTrayWndProcOriginal(
            pThis, hWnd, message, wParam, lParam);
    }
    const bool policyCommand =
        message == g_taskbarControlMsg &&
        wParam == kTaskbarCommandPolicy;
    const bool bindingCommand =
        message == g_taskbarControlMsg &&
        wParam == kTaskbarCommandBindNative;
    const bool dockPointerCommand =
        message == g_taskbarControlMsg &&
        wParam == kTaskbarCommandDockPointer;
    if (message == WM_WINDOWPOSCHANGED || policyCommand ||
        bindingCommand || dockPointerCommand) {
        RememberSecondaryNativeState(pThis, hWnd);
    }
    if (bindingCommand) {
        return 0;
    }

    if (message == g_taskbandNotificationMessage &&
        wParam == kTaskbandNotificationActivate) {
        RecordTaskbarNotification(hWnd);
        WakeWorker();
    } else if (dockPointerCommand) {
        ProcessDockPointerCommand(hWnd, lParam != 0);
        return 0;
    } else if (policyCommand) {
        ProcessSecondaryPolicyCommand(pThis, hWnd);
        return 0;
    }

    const LRESULT result = g_secondaryTrayWndProcOriginal(
        pThis, hWnd, message, wParam, lParam);
    if (message == WM_NCDESTROY) {
        EraseTrackedWindow(hWnd);
    }
    return result;
}

static bool WINAPI ViewCoordinatorShouldExpandHook(void* pThis,
                                                   HWND hWnd,
                                                   bool expanded) {
    if (!NativeAutoHideDispatchEnabled()) {
        return g_viewCoordinatorShouldExpandOriginal(
            pThis, hWnd, expanded);
    }
    switch (RememberViewCoordinator(hWnd, pThis)) {
        case ExpansionOverride::Show:
            return true;
        case ExpansionOverride::Native:
            break;
    }
    return g_viewCoordinatorShouldExpandOriginal(pThis, hWnd, expanded);
}

static void WINAPI ViewCoordinatorUpdateExpandedHook(
    void* pThis,
    HWND hWnd,
    int reason) {
    if (!NativeAutoHideDispatchEnabled()) {
        g_viewCoordinatorUpdateExpandedOriginal(pThis, hWnd, reason);
        return;
    }
    const bool revealRequest =
        reason == kViewReasonScreenEdgeEntered;
    if (revealRequest && !ScreenEdgeRevealAllowed(hWnd)) {
        return;
    }

    const bool previousRevealUpdate = g_revealUpdateActive;
    g_revealUpdateActive = previousRevealUpdate || revealRequest;
    g_viewCoordinatorUpdateExpandedOriginal(
        pThis, hWnd, reason);
    g_revealUpdateActive = previousRevealUpdate;
}

static HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandleW(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandleW(L"ExplorerExtensions.dll");
    }
    return module;
}

static bool HookTaskbarViewDllSymbols(HMODULE module) {
    std::lock_guard<std::mutex> hookLock(g_nativeHookMutex);
    if (g_unloading.load(std::memory_order_acquire)) {
        return false;
    }
    if (!module) {
        return false;
    }
    if (g_taskbarViewHooksInstalled.load(std::memory_order_acquire)) {
        return true;
    }
    if (g_taskbarViewHookAttempted.exchange(
            true, std::memory_order_acq_rel)) {
        return false;
    }

    struct ViewCoordinatorSymbols {
        ViewCoordinatorShouldExpand_t shouldExpand = nullptr;
        ViewCoordinatorPointerChanged_t pointerChanged = nullptr;
        ViewCoordinatorUpdateExpanded_t updateExpanded = nullptr;
    } resolved;

    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK resolveHooks[] = {
        {
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::ShouldTaskbarBeExpanded(unsigned __int64,bool))"},
            &resolved.shouldExpand,
            nullptr,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::HandleIsPointerOverTaskbarFrameChanged(unsigned __int64,bool,enum winrt::WindowsUdk::UI::Shell::InputDeviceKind))"},
            &resolved.pointerChanged,
            nullptr,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::UpdateIsExpanded(unsigned __int64,enum TaskbarTipTest::TaskbarExpandCollapseReason))"},
            &resolved.updateExpanded,
            nullptr,
            true,
        },
    };
    if (!WindhawkUtils::HookSymbols(
            module, resolveHooks, ARRAYSIZE(resolveHooks)) ||
        !resolved.shouldExpand || !resolved.pointerChanged ||
        !resolved.updateExpanded) {
        Wh_Log(L"Taskbar view symbols were unavailable; "
               L"Windows auto-hide remains native");
        return false;
    }

    g_viewCoordinatorShouldExpandOriginal = resolved.shouldExpand;
    g_viewCoordinatorPointerChangedOriginal = resolved.pointerChanged;
    g_viewCoordinatorUpdateExpandedOriginal = resolved.updateExpanded;

    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK installHooks[] = {
        {
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::ShouldTaskbarBeExpanded(unsigned __int64,bool))"},
            &g_viewCoordinatorShouldExpandOriginal,
            ViewCoordinatorShouldExpandHook,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::HandleIsPointerOverTaskbarFrameChanged(unsigned __int64,bool,enum winrt::WindowsUdk::UI::Shell::InputDeviceKind))"},
            &g_viewCoordinatorPointerChangedOriginal,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::UpdateIsExpanded(unsigned __int64,enum TaskbarTipTest::TaskbarExpandCollapseReason))"},
            &g_viewCoordinatorUpdateExpandedOriginal,
            ViewCoordinatorUpdateExpandedHook,
        },
    };

    bool installed = WindhawkUtils::HookSymbols(
        module, installHooks, ARRAYSIZE(installHooks));
    if (installed && g_initialized.load(std::memory_order_acquire)) {
        installed = Wh_ApplyHookOperations() != FALSE;
    }
    g_taskbarViewHooksInstalled.store(
        installed, std::memory_order_release);
    if (!installed) {
        Wh_Log(L"Taskbar view symbols were unavailable; "
               L"Windows auto-hide remains native");
    }
    SignalRefresh(false, true);
    return installed;
}

enum class NativeAutoHideHookResult {
    Installed,
    Unavailable,
    Failed,
};

#define NATIVE_TASKBAR_SYMBOLS(X)                                      \
    X(void*, trayUIVtableInspectable, g_trayUIVtableInspectable,       \
      L"TrayUI IInspectable vftable",                                 \
      LR"(const TrayUI::`vftable'{for `IInspectable'})", nullptr)      \
    X(void*, trayUIVtableComponentHost, g_trayUIVtableComponentHost,   \
      L"TrayUI ITrayComponentHost vftable",                           \
      LR"(const TrayUI::`vftable'{for `ITrayComponentHost'})",         \
      nullptr)                                                         \
    X(void*, secondaryTrayVtable, g_secondaryTrayVtable,               \
      L"CSecondaryTray ISecondaryTray vftable",                       \
      LR"(const CSecondaryTray::`vftable'{for `ISecondaryTray'})",     \
      nullptr)                                                         \
    X(TrayUIGetStuckMonitor_t, trayUIGetStuckMonitor,                  \
      g_trayUIGetStuckMonitorOriginal, L"TrayUI::GetStuckMonitor",    \
      LR"(public: virtual struct HMONITOR__ * __cdecl TrayUI::GetStuckMonitor(void))", \
      nullptr)                                                         \
    X(SecondaryTrayGetMonitor_t, secondaryTrayGetMonitor,              \
      g_secondaryTrayGetMonitorOriginal,                               \
      L"CSecondaryTray::GetMonitor",                                  \
      LR"(public: virtual struct HMONITOR__ * __cdecl CSecondaryTray::GetMonitor(void))", \
      nullptr)                                                         \
    X(TrayUIHide_t, trayUIHide, g_trayUIHideOriginal,                  \
      L"TrayUI::_Hide",                                               \
      LR"(public: void __cdecl TrayUI::_Hide(void))", TrayUIHideHook)  \
    X(TrayUIUnhide_t, trayUIUnhide, g_trayUIUnhideOriginal,            \
      L"TrayUI::Unhide",                                              \
      LR"(public: virtual void __cdecl TrayUI::Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))", \
      TrayUIUnhideHook)                                                \
    X(SecondaryTrayAutoHide_t, secondaryTrayAutoHide,                  \
      g_secondaryTrayAutoHideOriginal, L"CSecondaryTray::_AutoHide",  \
      LR"(private: void __cdecl CSecondaryTray::_AutoHide(bool))",     \
      SecondaryTrayAutoHideHook)                                      \
    X(SecondaryTrayUnhide_t, secondaryTrayUnhide,                      \
      g_secondaryTrayUnhideOriginal, L"CSecondaryTray::_Unhide",      \
      LR"(private: void __cdecl CSecondaryTray::_Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))", \
      SecondaryTrayUnhideHook)                                        \
    X(TrayUISlideWindow_t, trayUISlideWindow,                          \
      g_trayUISlideWindowOriginal, L"TrayUI::SlideWindow",            \
      LR"(public: virtual void __cdecl TrayUI::SlideWindow(struct HWND__ *,struct tagRECT const *,struct HMONITOR__ *,bool,bool))", \
      TrayUISlideWindowHook)                                          \
    X(TrayUIWndProc_t, trayUIWndProc, g_trayUIWndProcOriginal,         \
      L"TrayUI::WndProc",                                             \
      LR"(public: virtual __int64 __cdecl TrayUI::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *))", \
      TrayUIWndProcHook)                                               \
    X(SecondaryTrayWndProc_t, secondaryTrayWndProc,                    \
      g_secondaryTrayWndProcOriginal, L"CSecondaryTray::v_WndProc",   \
      LR"(private: virtual __int64 __cdecl CSecondaryTray::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))", \
      SecondaryTrayWndProcHook)

struct NativeTaskbarSymbols {
#define DECLARE_NATIVE_TASKBAR_SYMBOL(type, field, target, label, symbol, hook) \
    type field = nullptr;
    NATIVE_TASKBAR_SYMBOLS(DECLARE_NATIVE_TASKBAR_SYMBOL)
#undef DECLARE_NATIVE_TASKBAR_SYMBOL
};

static bool NativeAutoHideSymbolsResolved(
    const NativeTaskbarSymbols& symbols) {
    bool complete = true;
#define CHECK_NATIVE_TASKBAR_SYMBOL(type, field, target, label, symbol, hook) \
    complete = complete && symbols.field != nullptr;
    NATIVE_TASKBAR_SYMBOLS(CHECK_NATIVE_TASKBAR_SYMBOL)
#undef CHECK_NATIVE_TASKBAR_SYMBOL
    return complete;
}

static NativeAutoHideHookResult HookTaskbarSymbols() {
    g_taskbarModule = GetModuleHandleW(L"taskbar.dll");
    if (!g_taskbarModule) {
        // Keep the fallback reference for the lifetime of the mod. Windhawk's
        // hooks can still target taskbar.dll until engine teardown.
        g_taskbarModule = LoadLibraryExW(
            L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (!g_taskbarModule) {
        Wh_Log(L"Couldn't load taskbar.dll (error %lu)",
               GetLastError());
        return NativeAutoHideHookResult::Unavailable;
    }

    NativeTaskbarSymbols resolved;
    // taskbar.dll
    WindhawkUtils::SYMBOL_HOOK resolveHooks[] = {
#define RESOLVE_NATIVE_TASKBAR_SYMBOL(type, field, target, label, symbol, hook) \
        {{symbol}, &resolved.field, nullptr, true},
        NATIVE_TASKBAR_SYMBOLS(RESOLVE_NATIVE_TASKBAR_SYMBOL)
#undef RESOLVE_NATIVE_TASKBAR_SYMBOL
    };
    if (!WindhawkUtils::HookSymbols(
            g_taskbarModule, resolveHooks, ARRAYSIZE(resolveHooks))) {
        Wh_Log(L"Couldn't inspect native auto-hide symbols; "
               L"click-through remains active");
        return NativeAutoHideHookResult::Unavailable;
    }
    if (!NativeAutoHideSymbolsResolved(resolved)) {
#define LOG_MISSING_NATIVE_TASKBAR_SYMBOL(type, field, target, label, symbol, hook) \
        if (!resolved.field) {                                                \
            Wh_Log(L"Native auto-hide symbol unavailable: %s", label);       \
        }
        NATIVE_TASKBAR_SYMBOLS(LOG_MISSING_NATIVE_TASKBAR_SYMBOL)
#undef LOG_MISSING_NATIVE_TASKBAR_SYMBOL
        Wh_Log(L"Click-through remains active with Windows native auto-hide");
        return NativeAutoHideHookResult::Unavailable;
    }

#define PUBLISH_NATIVE_TASKBAR_SYMBOL(type, field, target, label, symbol, hook) \
    target = resolved.field;
    NATIVE_TASKBAR_SYMBOLS(PUBLISH_NATIVE_TASKBAR_SYMBOL)
#undef PUBLISH_NATIVE_TASKBAR_SYMBOL

    // taskbar.dll
    WindhawkUtils::SYMBOL_HOOK installHooks[] = {
#define INSTALL_NATIVE_TASKBAR_SYMBOL(type, field, target, label, symbol, hook) \
        {{symbol}, &target, hook},
        NATIVE_TASKBAR_SYMBOLS(INSTALL_NATIVE_TASKBAR_SYMBOL)
#undef INSTALL_NATIVE_TASKBAR_SYMBOL
    };
    if (!WindhawkUtils::HookSymbols(
            g_taskbarModule, installHooks, ARRAYSIZE(installHooks))) {
        Wh_Log(L"Native auto-hide hook registration failed");
        return NativeAutoHideHookResult::Failed;
    }
    return NativeAutoHideHookResult::Installed;
}

#undef NATIVE_TASKBAR_SYMBOLS

static void SignalWorkerReady(bool succeeded) {
    g_workerStartupSucceeded.store(
        succeeded, std::memory_order_release);
    SetEvent(g_workerReadyEvent);
}

static void DrainWorkerMessages() {
    MSG message{};
    unsigned int count = 0;
    while (count++ < 256 &&
           PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
        if (message.message == WM_QUIT) {
            break;
        }
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
}

static ULONGLONG EarliestShrinkDeadline() {
    ULONGLONG result = UINT64_MAX;
    std::lock_guard<std::mutex> lock(g_stateMutex);
    for (const auto& [hWnd, state] : g_desired) {
        if (state.shrinkPending) {
            result = std::min(result, state.shrinkDueTick);
        }
    }
    return result;
}

static bool DockPointerPollingNeeded() {
    std::lock_guard<std::mutex> lock(g_stateMutex);
    for (const auto& [hWnd, state] : g_desired) {
        if (AutoHideAppliesLocked(hWnd) && !state.hidden &&
            !state.keepShown && state.hasGeometry &&
            RectValid(state.shownRect)) {
            return true;
        }
    }
    return false;
}

static ULONGLONG EarliestAutoHideStateDeadline(
    const Settings& settings,
    ULONGLONG now) {
    ULONGLONG result = UINT64_MAX;
    std::lock_guard<std::mutex> lock(g_stateMutex);
    for (const auto& [hWnd, state] : g_desired) {
        if (!AutoHideAppliesLocked(hWnd)) {
            continue;
        }
        if (state.pendingTransition) {
            result = std::min(result, state.transitionDueTick);
        }
        if (NotificationHoldActive(settings, state, now)) {
            result = std::min(
                result,
                state.notificationBurstStartTick +
                    settings.notificationHoldMs);
        }
    }
    return result;
}

static DWORD WINAPI WorkerProc(LPVOID) {
    if (!SetThreadDpiAwarenessContext(
            DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) {
        LogFailureThrottled(
            L"SetThreadDpiAwarenessContext", GetLastError());
        SignalWorkerReady(false);
        return 0;
    }
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);

    const HRESULT coResult =
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(coResult)) {
        LogFailureThrottled(
            L"CoInitializeEx", static_cast<ULONG>(coResult));
        SignalWorkerReady(false);
        return 0;
    }
    const bool cancellationEnabled =
        SUCCEEDED(CoEnableCallCancellation(nullptr));

    ComPtr<IUIAutomation2> automation;
    ComPtr<IUIAutomationCondition> taskbarFrameCondition;
    ComPtr<IUIAutomationCondition> buttonCondition;
    ComPtr<IUIAutomationCondition> dockCondition;
    ComPtr<IUIAutomationCondition> rawViewCondition;
    ComPtr<IUIAutomationCacheRequest> cacheRequest;
    bool ready = false;

    do {
        HRESULT result = CoCreateInstance(
            __uuidof(CUIAutomation8), nullptr, CLSCTX_INPROC_SERVER,
            __uuidof(IUIAutomation2),
            reinterpret_cast<void**>(automation.put()));
        if (FAILED(result) || !automation) {
            LogFailureThrottled(
                L"Creating CUIAutomation8",
                static_cast<ULONG>(result));
            break;
        }
        result = automation->put_ConnectionTimeout(
            kAutomationTimeoutMs);
        if (SUCCEEDED(result)) {
            result = automation->put_TransactionTimeout(
                kAutomationTimeoutMs);
        }
        if (SUCCEEDED(result)) {
            result = CreateClassCondition(
                automation.get(),
                L"Taskbar.TaskbarFrameAutomationPeer",
                taskbarFrameCondition.put());
        }
        if (SUCCEEDED(result)) {
            result = CreateIntCondition(
                automation.get(), UIA_ControlTypePropertyId,
                UIA_ButtonControlTypeId, buttonCondition.put());
        }
        if (SUCCEEDED(result)) {
            result = automation->CreateOrCondition(
                taskbarFrameCondition.get(),
                buttonCondition.get(),
                dockCondition.put());
        }
        if (SUCCEEDED(result)) {
            result = automation->get_RawViewCondition(
                rawViewCondition.put());
        }
        if (SUCCEEDED(result)) {
            result = automation->CreateCacheRequest(
                cacheRequest.put());
        }
        if (FAILED(result)) {
            LogFailureThrottled(
                L"Configuring UI Automation",
                static_cast<ULONG>(result));
            break;
        }
        if (FAILED(cacheRequest->put_AutomationElementMode(
                AutomationElementMode_None)) ||
            FAILED(cacheRequest->put_TreeScope(TreeScope_Element)) ||
            FAILED(cacheRequest->put_TreeFilter(
                rawViewCondition.get())) ||
            FAILED(cacheRequest->AddProperty(
                UIA_ClassNamePropertyId)) ||
            FAILED(cacheRequest->AddProperty(
                UIA_IsOffscreenPropertyId)) ||
            FAILED(cacheRequest->AddProperty(
                UIA_BoundingRectanglePropertyId))) {
            LogFailureThrottled(
                L"Configuring UI Automation cache",
                ERROR_INVALID_DATA);
            break;
        }

        MSG bootstrap{};
        PeekMessageW(&bootstrap, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
        g_geometryLifecycleWinEventHook = SetWinEventHook(
            EVENT_OBJECT_CREATE, EVENT_OBJECT_REORDER,
            nullptr, GeometryWinEventProc, GetCurrentProcessId(), 0,
            WINEVENT_OUTOFCONTEXT);
        g_geometryLocationWinEventHook = SetWinEventHook(
            EVENT_OBJECT_LOCATIONCHANGE,
            EVENT_OBJECT_LOCATIONCHANGE,
            nullptr, GeometryWinEventProc, 0, 0,
            WINEVENT_OUTOFCONTEXT);
        g_foregroundWinEventHook = SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
            nullptr, ForegroundWinEventProc, 0, 0,
            WINEVENT_OUTOFCONTEXT);
        if (!g_geometryLifecycleWinEventHook ||
            !g_geometryLocationWinEventHook ||
            !g_foregroundWinEventHook) {
            Wh_Log(L"One or more accessibility event hooks were unavailable; "
                   L"fallback refresh remains active");
        }

        ready = true;
    } while (false);

    SignalWorkerReady(ready);
    if (!ready) {
        if (cancellationEnabled) {
            CoDisableCallCancellation(nullptr);
        }
        CoUninitialize();
        return 0;
    }

    WorkerBuffers buffers;
    Settings settings = GetSettingsSnapshot();
    uint64_t settingsRevision = settings.revision;
    ULONGLONG now = GetTickCount64();
    ULONGLONG geometryDue = now;
    ULONGLONG nextFallbackScan = now;
    ULONGLONG nextGateRead = now;
    ULONGLONG nextPolicyRefresh = now;
    ULONGLONG nextWindowPolicy = now;
    const ULONGLONG taskbarViewHookFastDeadline =
        now + kTaskbarViewHookFastWindowMs;
    ULONGLONG nextTaskbarViewHookProbe = now;
    bool taskbarViewHookDiscoveryFinished = false;

    HANDLE handles[] = {g_stopEvent, g_refreshEvent};
    while (!StopRequested()) {
        DrainWorkerMessages();
        now = GetTickCount64();

        const uint64_t publishedRevision =
            g_settingsRevision.load(std::memory_order_acquire);
        if (publishedRevision != settingsRevision) {
            settings = GetSettingsSnapshot();
            settingsRevision = settings.revision;
            geometryDue = now;
            nextPolicyRefresh = now;
            nextWindowPolicy = now;
        }

        if (now >= nextGateRead) {
            const bool nativeHooksInstalled =
                g_nativeAutoHideHooksInstalled.load(
                    std::memory_order_acquire);
            const bool enabled =
                nativeHooksInstalled && ReadWindowsAutoHideEnabled();
            if (g_windowsAutoHideEnabled.exchange(
                    enabled, std::memory_order_acq_rel) != enabled) {
                g_policyDirty.store(true, std::memory_order_release);
                nextPolicyRefresh = now;
            }
            if (!taskbarViewHookDiscoveryFinished &&
                g_initialized.load(std::memory_order_acquire) &&
                now >= nextTaskbarViewHookProbe) {
                if (!nativeHooksInstalled ||
                    g_taskbarViewHooksInstalled.load(
                        std::memory_order_acquire) ||
                    g_taskbarViewHookAttempted.load(
                        std::memory_order_acquire)) {
                    taskbarViewHookDiscoveryFinished = true;
                } else if (HMODULE viewModule =
                               GetTaskbarViewModuleHandle()) {
                    Wh_Log(L"Found the taskbar view module");
                    HookTaskbarViewDllSymbols(viewModule);
                    taskbarViewHookDiscoveryFinished = true;
                } else {
                    nextTaskbarViewHookProbe =
                        now + (now < taskbarViewHookFastDeadline
                                   ? kStatePollMs
                                   : kTaskbarViewHookBackoffMs);
                }
            }
            nextGateRead = now + kStatePollMs;
        }

        CheckTaskbarVisibilityTransitions();
        if (DockPointerPollingNeeded()) {
            UpdateDockPointerTransitions();
        }

        if (g_geometryDirty.exchange(
                false, std::memory_order_acq_rel)) {
            geometryDue = std::min(
                geometryDue ? geometryDue : now + kGeometryDebounceMs,
                now + kGeometryDebounceMs);
        }
        if (now >= nextFallbackScan) {
            if (!geometryDue) {
                geometryDue = now;
            }
            // A deferred geometry attempt has its own short retry deadline.
            // Advance the fallback deadline now so it can't remain overdue
            // and force a zero-timeout worker loop while the pointer is busy.
            nextFallbackScan = now + settings.fallbackScanMs;
        }

        if (geometryDue && now >= geometryDue) {
            if (PointerInteractingWithTaskbar() &&
                !NeedsShownGeometryWork()) {
                geometryDue = now + kPointerInteractionRetryMs;
            } else {
                if (EnumTaskbars(buffers.taskbars)) {
                    bool completed = true;
                    for (HWND hWnd : buffers.taskbars) {
                        if (StopRequested()) {
                            completed = false;
                            break;
                        }
                        if (!TryInstallTaskbarSubclass(hWnd)) {
                            continue;
                        }
                        RequestNativeTaskbarBinding(hWnd);
                        bool changed = false;
                        if (!UpdateWindowRegion(
                                automation.get(), dockCondition.get(),
                                cacheRequest.get(), hWnd, settings,
                                buffers, changed)) {
                            completed = false;
                            break;
                        }
                    }
                    if (!completed) {
                        break;
                    }
                }
                PruneStaleTrackedWindows();
                geometryDue = 0;
                nextFallbackScan =
                    now + settings.fallbackScanMs;
                g_policyDirty.store(true, std::memory_order_release);
                const ULONGLONG shrink = EarliestShrinkDeadline();
                if (shrink != UINT64_MAX) {
                    geometryDue = shrink;
                }
                if (NeedsShownGeometryWork()) {
                    const ULONGLONG retry =
                        GetTickCount64() + kPointerInteractionRetryMs;
                    geometryDue = geometryDue
                                      ? std::min(geometryDue, retry)
                                      : retry;
                }
            }
        }

        const bool policyDirty =
            g_policyDirty.load(std::memory_order_acquire);
        if (PolicyRefreshDue(now, nextPolicyRefresh,
                             nextWindowPolicy, policyDirty)) {
            const bool refreshRequested =
                g_policyDirty.exchange(false, std::memory_order_acq_rel);
            if (refreshRequested || now >= nextWindowPolicy) {
                if (NativeAutoHideDispatchEnabled() &&
                    g_windowsAutoHideEnabled.load(
                        std::memory_order_acquire)) {
                    EvaluateWindowPolicy(settings);
                }
                now = GetTickCount64();
                nextPolicyRefresh = NextPolicyRefreshTick(
                    now, kAutoHidePolicyIntervalMs);
                nextWindowPolicy = NextPolicyRefreshTick(
                    now, kWindowPolicyFallbackMs);
            }
        }
        UpdatePolicyTransitions(settings);

        now = GetTickCount64();
        ULONGLONG deadline = nextGateRead;
        if (DockPointerPollingNeeded()) {
            deadline = std::min(
                deadline, now + kAutoHidePolicyIntervalMs);
        }
        const ULONGLONG autoHideStateDeadline =
            EarliestAutoHideStateDeadline(settings, now);
        if (autoHideStateDeadline != UINT64_MAX) {
            deadline = std::min(deadline, autoHideStateDeadline);
        }
        if (geometryDue) {
            deadline = std::min(deadline, geometryDue);
        }
        deadline = std::min(deadline, nextFallbackScan);
        deadline = std::min(deadline, nextGateRead);
        deadline = std::min(deadline, nextWindowPolicy);
        if (g_policyDirty.load(std::memory_order_acquire)) {
            deadline = std::min(deadline, nextPolicyRefresh);
        }
        const DWORD timeout =
            deadline <= now
                ? 0
                : static_cast<DWORD>(std::min<ULONGLONG>(
                      deadline - now, MAXDWORD - 1));

        const DWORD waitResult = MsgWaitForMultipleObjectsEx(
            ARRAYSIZE(handles), handles, timeout, QS_ALLINPUT,
            MWMO_INPUTAVAILABLE);
        if (waitResult == WAIT_OBJECT_0) {
            break;
        }
        if (waitResult == WAIT_FAILED) {
            LogFailureThrottled(
                L"MsgWaitForMultipleObjectsEx", GetLastError());
            break;
        }
    }

    if (g_foregroundWinEventHook) {
        UnhookWinEvent(g_foregroundWinEventHook);
        g_foregroundWinEventHook = nullptr;
    }
    if (g_geometryLocationWinEventHook) {
        UnhookWinEvent(g_geometryLocationWinEventHook);
        g_geometryLocationWinEventHook = nullptr;
    }
    if (g_geometryLifecycleWinEventHook) {
        UnhookWinEvent(g_geometryLifecycleWinEventHook);
        g_geometryLifecycleWinEventHook = nullptr;
    }

    RemoveAllTaskbarSubclasses();
    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        g_desired.clear();
        g_viewCoordinators.clear();
        g_nativeTaskbarObjects.clear();
    }

    if (cancellationEnabled) {
        CoDisableCallCancellation(nullptr);
    }
    CoUninitialize();
    return 0;
}

static void RequestWorkerStop() {
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_refreshEvent) {
        SetEvent(g_refreshEvent);
    }
    if (g_workerThreadId) {
        CoCancelCall(g_workerThreadId, 0);
    }
}

static void CloseRuntimeHandles() {
    if (g_workerThread) {
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    if (g_installHookIdleEvent) {
        CloseHandle(g_installHookIdleEvent);
        g_installHookIdleEvent = nullptr;
    }
    if (g_subclassIdleEvent) {
        CloseHandle(g_subclassIdleEvent);
        g_subclassIdleEvent = nullptr;
    }
    if (g_workerReadyEvent) {
        CloseHandle(g_workerReadyEvent);
        g_workerReadyEvent = nullptr;
    }
    if (g_refreshEvent) {
        CloseHandle(g_refreshEvent);
        g_refreshEvent = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    g_workerThreadId = 0;
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Init v%s", WH_MOD_VERSION);

    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (taskbar) {
        DWORD processId = 0;
        if (GetWindowThreadProcessId(taskbar, &processId) &&
            processId != GetCurrentProcessId()) {
            Wh_Log(L"Skipping a non-shell Explorer process");
            return FALSE;
        }
    } else {
        int argumentCount = 0;
        LPWSTR* arguments =
            CommandLineToArgvW(GetCommandLineW(), &argumentCount);
        const bool primaryStartup =
            arguments && argumentCount == 1;
        if (arguments) {
            LocalFree(arguments);
        }
        if (!primaryStartup) {
            Wh_Log(L"Skipping an auxiliary Explorer process");
            return FALSE;
        }
    }

    g_initialized.store(false, std::memory_order_release);
    g_unloading.store(false, std::memory_order_release);
    g_windowsAutoHideEnabled.store(false, std::memory_order_release);
    g_nativeAutoHideHooksInstalled.store(false,
                                         std::memory_order_release);
    g_taskbarViewHooksInstalled.store(false,
                                      std::memory_order_release);
    g_taskbarViewHookAttempted.store(false,
                                     std::memory_order_release);
    g_geometryDirty.store(true, std::memory_order_release);
    g_policyDirty.store(true, std::memory_order_release);
    g_topologyRevision.store(0, std::memory_order_release);
    g_lastErrorLogTick.store(0, std::memory_order_release);
    g_installHookCallbacks.store(0, std::memory_order_release);
    g_subclassCallbacks.store(0, std::memory_order_release);
    g_workerStartupSucceeded.store(false,
                                   std::memory_order_release);

    g_taskbarControlMsg = RegisterWindowMessageW(
        L"WindhawkTaskbarDockClickThrough_Control_" WH_MOD_ID);
    g_installSubclassMsg = RegisterWindowMessageW(
        L"WindhawkTaskbarDockClickThrough_Install_" WH_MOD_ID);
    g_taskbandNotificationMessage = RegisterWindowMessageW(L"SHELLHOOK");
    if (!g_taskbarControlMsg || !g_installSubclassMsg) {
        Wh_Log(L"RegisterWindowMessageW failed (error %lu)",
               GetLastError());
        return FALSE;
    }
    if (!g_taskbandNotificationMessage) {
        Wh_Log(L"SHELLHOOK message registration failed; "
               L"click-through remains active with Windows native auto-hide");
    }

    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        g_desired.clear();
        g_viewCoordinators.clear();
        g_nativeTaskbarObjects.clear();
    }
    LoadSettings();

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_refreshEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_workerReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_installHookIdleEvent =
        CreateEventW(nullptr, TRUE, TRUE, nullptr);
    g_subclassIdleEvent = CreateEventW(nullptr, TRUE, TRUE, nullptr);
    if (!g_stopEvent || !g_refreshEvent || !g_workerReadyEvent ||
        !g_installHookIdleEvent || !g_subclassIdleEvent) {
        Wh_Log(L"CreateEventW failed (error %lu)", GetLastError());
        CloseRuntimeHandles();
        return FALSE;
    }

    g_workerThread = CreateThread(
        nullptr, 0, WorkerProc, nullptr, 0, &g_workerThreadId);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed (error %lu)", GetLastError());
        CloseRuntimeHandles();
        return FALSE;
    }

    const DWORD readyResult = WaitForSingleObject(
        g_workerReadyEvent, kWorkerStartupTimeoutMs);
    if (readyResult != WAIT_OBJECT_0 ||
        !g_workerStartupSucceeded.load(std::memory_order_acquire)) {
        Wh_Log(L"Worker startup failed or timed out");
        RequestWorkerStop();
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseRuntimeHandles();
        return FALSE;
    }

    if (g_taskbandNotificationMessage) {
        const NativeAutoHideHookResult hookResult = HookTaskbarSymbols();
        if (hookResult == NativeAutoHideHookResult::Failed ||
            (hookResult == NativeAutoHideHookResult::Installed &&
             !WindhawkUtils::SetFunctionHook(
                 SetTimer, SetTimerHook, &g_setTimerOriginal))) {
            Wh_Log(L"Native auto-hide hook registration failed");
            RequestWorkerStop();
            WaitForSingleObject(g_workerThread, INFINITE);
            CloseRuntimeHandles();
            return FALSE;
        }
        if (hookResult == NativeAutoHideHookResult::Installed) {
            g_nativeAutoHideHooksInstalled.store(
                true, std::memory_order_release);
            if (HMODULE viewModule = GetTaskbarViewModuleHandle()) {
                HookTaskbarViewDllSymbols(viewModule);
            } else {
                Wh_Log(L"Taskbar view module isn't loaded yet");
            }
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    g_initialized.store(true, std::memory_order_release);
    SignalRefresh(true, true);
}

void Wh_ModBeforeUninit() {
    g_initialized.store(false, std::memory_order_release);
    g_unloading.store(true, std::memory_order_release);
    g_windowsAutoHideEnabled.store(false, std::memory_order_release);
    RequestWorkerStop();
    {
        // Wh_ApplyHookOperations can't run after this callback returns.
        std::lock_guard<std::mutex> hookLock(g_nativeHookMutex);
    }
    std::vector<HWND> taskbars;
    if (EnumTaskbars(taskbars)) {
        for (HWND hWnd : taskbars) {
            KillTimer(hWnd, kDockExitHideTimer);
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    RequestWorkerStop();
    if (g_workerThread) {
        const DWORD result = WaitForSingleObject(
            g_workerThread, kWorkerShutdownTimeoutMs);
        if (result != WAIT_OBJECT_0) {
            Wh_Log(L"Worker shutdown exceeded %lu ms; waiting for "
                   L"safe callback removal",
                   kWorkerShutdownTimeoutMs);
            CoCancelCall(g_workerThreadId, 0);
            WaitForSingleObject(g_workerThread, INFINITE);
        }
    }
    CloseRuntimeHandles();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    g_geometryDirty.store(true, std::memory_order_release);
    g_policyDirty.store(true, std::memory_order_release);
    if (g_refreshEvent) {
        SetEvent(g_refreshEvent);
    }
}
